// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *  This file is part of the Cxbx project.
// *
// *  Cxbx and Cxbe are free software; you can redistribute them
// *  and/or modify them under the terms of the GNU General Public
// *  License as published by the Free Software Foundation; either
// *  version 2 of the license, or (at your option) any later version.
// *
// *  This program is distributed in the hope that it will be useful,
// *  but WITHOUT ANY WARRANTY; without even the implied warranty of
// *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// *  GNU General Public License for more details.
// *
// *  You should have recieved a copy of the GNU General Public License
// *  along with this program; see the file COPYING.
// *  If not, write to the Free Software Foundation, Inc.,
// *  59 Temple Place - Suite 330, Bostom, MA 02111-1307, USA.
// *
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *  (c) 2016 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************

#define LOG_PREFIX CXBXR_MODULE::PS
#include <common\util\CxbxUtil.h>


#include <core\kernel\exports\xboxkrnl.h> // For PsCreateSystemThreadEx, etc.
#include "EmuKrnlPs.hpp"
#include "core\kernel\exports\EmuKrnlKi.h"
#include "core\kernel\exports\EmuKrnlKe.h"
#include <process.h> // For __beginthreadex(), etc.
#include <float.h> // For _controlfp constants

#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnlLogging.h"
#include "core\kernel\init\CxbxKrnl.h" // For CxbxKrnl_TLS
#include "EmuKrnl.h"
#include "core\kernel\support\Emu.h" // For EmuLog(LOG_LEVEL::WARNING, )
#include "core\kernel\support\EmuFS.h" // For EmuGenerateFS
#include "core\kernel\support\NativeHandle.h"

// prevent name collisions
namespace NtDll
{
#include "core\kernel\support\EmuNtDll.h"
};

#define PSP_MAX_CREATE_THREAD_NOTIFY 8

// PsCreateSystemThread proxy parameters
typedef struct _PCSTProxyParam
{
	IN xbox::PVOID  Ethread;
	IN xbox::ulong_xt TlsDataSize;
}
PCSTProxyParam;

static xbox::PCREATE_THREAD_NOTIFY_ROUTINE g_pfnThreadNotification[PSP_MAX_CREATE_THREAD_NOTIFY] = { xbox::zeroptr };
static std::atomic_int g_iThreadNotificationCount = 0;
static std::mutex g_ThreadNotificationMtx;

// Separate function for logging, otherwise in PCSTProxy __try wont work (Compiler Error C2712)
void LOG_PCSTProxy
(
	xbox::PVOID StartRoutine,
	xbox::PVOID StartContext,
	xbox::PVOID SystemRoutine,
	xbox::PVOID Ethread,
	xbox::ulong_xt TlsDataSize
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(StartRoutine)
		LOG_FUNC_ARG(StartContext)
		LOG_FUNC_ARG(SystemRoutine)
		LOG_FUNC_ARG(Ethread)
		LOG_FUNC_ARG(TlsDataSize)
		LOG_FUNC_END;
}

// PsCreateSystemThread proxy procedure
// Dxbx Note : The signature of PCSTProxy should conform to System.TThreadFunc !
static unsigned int WINAPI PCSTProxy
(
	IN PVOID Parameter
)
{
	CxbxSetThreadName("PsCreateSystemThread Proxy");

	PCSTProxyParam *iPCSTProxyParam = static_cast<PCSTProxyParam*>(Parameter);

	// Copy params to the stack so they can be freed
	PCSTProxyParam params = *iPCSTProxyParam;
	delete iPCSTProxyParam;
#ifndef ENABLE_KTHREAD_SWITCHING
	unsigned Host2XbStackBaseReserved = 0;
	__asm mov Host2XbStackBaseReserved, esp;
	unsigned Host2XbStackSizeReserved = EmuGenerateStackSize(Host2XbStackBaseReserved, params.TlsDataSize);
	__asm sub esp, Host2XbStackSizeReserved;
#endif

	// Do minimal thread initialization
	xbox::PETHREAD eThread = static_cast<xbox::PETHREAD>(params.Ethread);
#ifndef ENABLE_KTHREAD_SWITCHING
	EmuGenerateFS(eThread, Host2XbStackBaseReserved, Host2XbStackSizeReserved);
#else
	EmuGenerateFS(eThread);
#endif
	xbox::PKSTART_FRAME StartFrame = reinterpret_cast<xbox::PKSTART_FRAME>(reinterpret_cast<xbox::addr_xt>(eThread->Tcb.KernelStack) + sizeof(xbox::KSWITCHFRAME));

	LOG_PCSTProxy(
		StartFrame->StartRoutine,
		StartFrame->StartContext,
		StartFrame->SystemRoutine,
		params.Ethread,
		params.TlsDataSize);

	xbox::KiExecuteKernelApc();

	auto routine = (xbox::PKSYSTEM_ROUTINE)StartFrame->SystemRoutine;
	// Debugging notice : When the below line shows up with an Exception dialog and a
	// message like: "Exception thrown at 0x00026190 in cxbx.exe: 0xC0000005: Access
	// violation reading location 0xFD001804.", then this is AS-DESIGNED behaviour!
	// (To avoid repetitions, uncheck "Break when this exception type is thrown").
	routine(StartFrame->StartRoutine, StartFrame->StartContext);

	// This will also handle thread notification :
	LOG_TEST_CASE("Thread returned from SystemRoutine");
	xbox::PsTerminateSystemThread(X_STATUS_SUCCESS);

#ifndef ENABLE_KTHREAD_SWITCHING
	__asm add esp, Host2XbStackSizeReserved;
#endif

	return 0; // will never be reached
}

// Placeholder system function
xbox::void_xt NTAPI PspSystemThreadStartup
(
	IN xbox::PKSTART_ROUTINE StartRoutine,
	IN PVOID StartContext
)
{
	// TODO : Call PspUnhandledExceptionInSystemThread(GetExceptionInformation())
	(StartRoutine)(StartContext);

	xbox::PsTerminateSystemThread(X_STATUS_SUCCESS);
}

xbox::PETHREAD xbox::PspGetCurrentThread()
{
	// This works because we assign ethread to Prcb->CurrentThread
	return reinterpret_cast<PETHREAD>(KeGetCurrentThread());
}

static xbox::void_xt PspCallThreadNotificationRoutines(xbox::PETHREAD eThread, xbox::boolean_xt Create)
{
	std::unique_lock lck(g_ThreadNotificationMtx);
	for (int i = 0; i < PSP_MAX_CREATE_THREAD_NOTIFY; i++) {
		if (g_pfnThreadNotification[i]) {
			EmuLog(LOG_LEVEL::DEBUG, "Calling pfnNotificationRoutine[%d] (0x%.8X)", i, g_pfnThreadNotification[i]);
			(*g_pfnThreadNotification[i])(eThread, eThread->UniqueThread, Create);
		}
	}
}

// Source: ReactOS
xbox::LIST_ENTRY PspReaperListHead;
xbox::void_xt NTAPI PspReaperRoutine(
	IN xbox::PKDPC Dpc,
	IN xbox::PVOID DeferredContext,
	IN xbox::PVOID SystemArgument1,
	IN xbox::PVOID SystemArgument2
)
{
	using namespace xbox;
	xbox::PLIST_ENTRY NextEntry;
	PETHREAD Thread;
	//PSTRACE(PS_KILL_DEBUG, "Context: %p\n", Context);

	/* Write magic value and return the next entry to process */
	NextEntry = PspReaperListHead.Flink;

	/* Start loop */
	while (NextEntry != &PspReaperListHead) {
		/* Get the first Thread Entry */
		Thread = CONTAINING_RECORD(NextEntry, ETHREAD, ReaperLink);

		RemoveEntryList(NextEntry);

		// Currently, only kernel's stack portion reside on the host's stack.
		// Once we have our own kernel thread switching implement or in virtual environment.
		// Then enable ENABLE_KTHREAD_SWITCHING macro for any further implement needed.
#ifdef ENABLE_KTHREAD_SWITCHING
		/* Delete this entry's kernel stack */
		MmDeleteKernelStack(Thread->Tcb.StackBase, Thread->Tcb.StackLimit);
#else
		// Backup plan in case if certain titles did not let new thread start.
		// And therefore still have the xbox's kernel stack allocated.
		if (MmIsAddressValid(Thread->Tcb.StackBase)) {
			MmDeleteKernelStack(Thread->Tcb.StackBase, Thread->Tcb.StackLimit);
		}
#endif
		Thread->Tcb.StackBase = zeroptr;

		/* Move to the next entry */
		NextEntry = NextEntry->Flink;

		/* Dereference this thread */
		ObfDereferenceObject(Thread);
	}
}

xbox::ntstatus_xt NTAPI CxbxrCreateThread
(
	OUT xbox::PHANDLE         ThreadHandle,
	OUT xbox::PHANDLE         ThreadId OPTIONAL,
	IN  xbox::PKSTART_ROUTINE StartRoutine,
	IN  xbox::PVOID           StartContext,
	IN  xbox::boolean_xt      DebuggerThread
)
{
	Xbe::TLS* XbeTls = (Xbe::TLS*)CxbxKrnl_Xbe->m_Header.dwTLSAddr;
	xbox::ulong_xt TlsDataSize = (XbeTls->dwDataEndAddr - XbeTls->dwDataStartAddr);
	TlsDataSize += XbeTls->dwSizeofZeroFill + 15;
	TlsDataSize = (TlsDataSize & ~15) + 4;
	// Verify EmuGenerateFS's TlsData's setup is carry over to XapiThreadStartup function.
	// Instead of using PspSystemThreadStartup which is entirely different function.
	return xbox::PsCreateSystemThreadEx(
		/*OUT*/ThreadHandle,
		/*ThreadExtensionSize=*/0,
		/*KernelStackSize=*/KERNEL_STACK_SIZE,
		TlsDataSize,
		/*OUT*/ThreadId,
		/*StartRoutine=*/StartRoutine,
		StartContext,
		/*CreateSuspended=*/FALSE,
		/*DebuggerThread=*/DebuggerThread,
		/*SystemRoutine=*/PspSystemThreadStartup // should use XapiThreadStartup
	);
}

static xbox::KDPC PsReaperDpc;
xbox::void_xt xbox::PsInitSystem()
{
#ifdef ENABLE_KTHREAD_SWITCHING
	assert(0); // NOTE: Verify all defined ENABLE_KTHREAD_SWITCHING check are implemented
#endif
	/* Setup the reaper */
	InitializeListHead(&PspReaperListHead);
	KeInitializeDpc(&PsReaperDpc, PspReaperRoutine, zeroptr);
}

// ******************************************************************
// * 0x00FE - PsCreateSystemThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(254) xbox::ntstatus_xt NTAPI xbox::PsCreateSystemThread
(
	OUT PHANDLE         ThreadHandle,
	OUT PHANDLE          ThreadId OPTIONAL,
	IN  PKSTART_ROUTINE StartRoutine,
	IN  PVOID           StartContext,
	IN  boolean_xt         DebuggerThread
)
{
	LOG_FORWARD("PsCreateSystemThreadEx");

	return PsCreateSystemThreadEx(
		/*OUT*/ThreadHandle,
		/*ThreadExtensionSize=*/0,
		/*KernelStackSize=*/KERNEL_STACK_SIZE,
		/*TlsDataSize=*/0,
		/*OUT*/ThreadId,
		/*StartRoutine=*/StartRoutine,
		StartContext,
		/*CreateSuspended=*/FALSE,
		/*DebuggerThread=*/DebuggerThread,
		/*SystemRoutine=*/PspSystemThreadStartup // instead of XapiThreadStartup
		);
}

// ******************************************************************
// * 0x00FF - PsCreateSystemThreadEx()
// ******************************************************************
// Creates a system thread.
// ThreadHandle: Receives the thread handle
// ThreadExtensionSize: Unsure how this works (everything I've seen uses 0)
// KernelStackSize: Size of the allocation for both stack and TLS data
// TlsDataSize: Size within KernelStackSize to use as TLS data
// ThreadId: Receives the thread ID number
// StartRoutine: Called when the thread is created (by XapiThreadStartup)
// StartContext: Parameter StartRoutine
// CreateSuspended: TRUE to create the thread as a suspended thread
// DebuggerThread: TRUE to allocate the stack from Debug Kit memory
// SystemRoutine: System function (normally XapiThreadStartup) called when the thread is created
//
// New to the XBOX.
XBSYSAPI EXPORTNUM(255) xbox::ntstatus_xt NTAPI xbox::PsCreateSystemThreadEx
(
	OUT PHANDLE         ThreadHandle,
	IN  ulong_xt           ThreadExtensionSize,
	IN  ulong_xt           KernelStackSize,
	IN  ulong_xt           TlsDataSize,
	OUT PHANDLE          ThreadId OPTIONAL,
	IN  PKSTART_ROUTINE StartRoutine,
	IN  PVOID           StartContext,
	IN  boolean_xt         CreateSuspended,
	IN  boolean_xt         DebuggerThread,
	IN  PKSYSTEM_ROUTINE SystemRoutine OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(ThreadHandle)
		LOG_FUNC_ARG(ThreadExtensionSize)
		LOG_FUNC_ARG(KernelStackSize)
		LOG_FUNC_ARG(TlsDataSize)
		LOG_FUNC_ARG_OUT(ThreadId)
		LOG_FUNC_ARG(StartRoutine)
		LOG_FUNC_ARG(StartContext)
		LOG_FUNC_ARG(CreateSuspended)
		LOG_FUNC_ARG(DebuggerThread)
		LOG_FUNC_ARG(SystemRoutine)
		LOG_FUNC_END;

	// use default kernel stack size if lesser specified
	if (KernelStackSize < KERNEL_STACK_SIZE)
		KernelStackSize = KERNEL_STACK_SIZE;

	// Double the stack size, this is to account for the overhead HLE patching adds to the stack
	uint32_t hKernelStackSize = KernelStackSize * 2;

	// round up to the next page boundary if un-aligned
	KernelStackSize = RoundUp(KernelStackSize, PAGE_SIZE);
	hKernelStackSize = RoundUp(hKernelStackSize, PAGE_SIZE);

	// create thread, using our special proxy technique
	{
		PETHREAD eThread;
		ntstatus_xt result = ObCreateObject(&PsThreadObjectType, zeroptr, sizeof(ETHREAD) + ThreadExtensionSize, reinterpret_cast<PVOID *>(&eThread));
		if (!X_NT_SUCCESS(result)) {
			RETURN(result);
		}

		std::memset(eThread, 0, sizeof(ETHREAD) + ThreadExtensionSize);

		// Create kernel stack for xbox title to able write on stack instead of host.
		PVOID KernelStack = MmCreateKernelStack(KernelStackSize, DebuggerThread);

		if (!KernelStack) {
			ObfDereferenceObject(eThread);
			RETURN(X_STATUS_INSUFFICIENT_RESOURCES);
		}

		// Start thread initialization process here before insert and create thread
		KeInitializeThread(&eThread->Tcb, KernelStack, KernelStackSize, TlsDataSize, SystemRoutine, StartRoutine, StartContext, &KiUniqueProcess);

		// The ob handle of the ethread obj is the thread id we return to the title
		result = ObInsertObject(eThread, zeroptr, 0, &eThread->UniqueThread);
		if (!X_NT_SUCCESS(result)) {
			ObfDereferenceObject(eThread);
			RETURN(result);
		}

		if (g_iThreadNotificationCount) {
			PspCallThreadNotificationRoutines(eThread, TRUE);
		}

		// Create another handle to pass back to the title in the ThreadHandle argument
		result = ObOpenObjectByPointer(eThread, &PsThreadObjectType, ThreadHandle);
		if (!X_NT_SUCCESS(result)) {
			ObfDereferenceObject(eThread);
			RETURN(result);
		}

		if (ThreadId != zeroptr) {
			*ThreadId = eThread->UniqueThread;
		}

		// PCSTProxy is responsible for cleaning up this pointer
		PCSTProxyParam *iPCSTProxyParam = new PCSTProxyParam;
		iPCSTProxyParam->Ethread = eThread;
		iPCSTProxyParam->TlsDataSize = TlsDataSize;

		unsigned int ThreadId;
		HANDLE handle = reinterpret_cast<HANDLE>(_beginthreadex(NULL, hKernelStackSize, PCSTProxy, iPCSTProxyParam, CREATE_SUSPENDED, &ThreadId));
		if (handle == zeroptr) {
			delete iPCSTProxyParam;
			ObpClose(eThread->UniqueThread);
			ObfDereferenceObject(eThread);
			RETURN(X_STATUS_INSUFFICIENT_RESOURCES);
		}

		// Increment the ref count of the thread once more. This is to guard against the case the title closes the thread handle
		// before this thread terminates with PsTerminateSystemThread
		// Test case: Amped
		ObfReferenceObject(eThread);

		KeQuerySystemTime(&eThread->CreateTime);
		RegisterXboxObject(eThread, handle);

		eThread->Tcb.Priority = GetThreadPriority(handle);
		g_AffinityPolicy->SetAffinityXbox(handle);

		// Wait for the initialization of the remaining thread state
		KeSuspendThreadEx(&eThread->Tcb);
		ResumeThread(handle);
		while (eThread->Tcb.State == Initialized) {
			std::this_thread::yield();
		}

		// Now that ThreadId is populated and affinity is changed, resume the thread (unless the guest passed CREATE_SUSPENDED), then wait until the new thread has
		// finished initialization
		if (CreateSuspended) {
			KeSuspendThread(&eThread->Tcb);
		}

		KeResumeThreadEx(&eThread->Tcb);

		// Log ThreadID identical to how GetCurrentThreadID() is rendered :
		EmuLog(LOG_LEVEL::DEBUG, "Created Xbox proxy thread. Handle : 0x%X, ThreadId : [0x%.4X], Native Handle : 0x%X, Native ThreadId : [0x%.4X]",
			*ThreadHandle, eThread->UniqueThread, handle, ThreadId);
	}

	RETURN(X_STATUS_SUCCESS);
}

// ******************************************************************
// * 0x0100 - PsQueryStatistics()
// ******************************************************************
XBSYSAPI EXPORTNUM(256) xbox::ntstatus_xt NTAPI xbox::PsQueryStatistics
(
	IN OUT PPS_STATISTICS ProcessStatistics
)
{
	LOG_FUNC_ONE_ARG_OUT(ProcessStatistics);

	ntstatus_xt ret = X_STATUS_SUCCESS;

	if (ProcessStatistics->Length == sizeof(PS_STATISTICS)) {
		ProcessStatistics->ThreadCount = KiUniqueProcess.StackCount;
		ProcessStatistics->HandleCount = ObpObjectHandleTable.HandleCount; // This currently doesn't count native handles that we use as xbox handles
	} else {
		ret = X_STATUS_INVALID_PARAMETER;
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x0101 - PsSetCreateThreadNotifyRoutine()
// ******************************************************************
XBSYSAPI EXPORTNUM(257) xbox::ntstatus_xt NTAPI xbox::PsSetCreateThreadNotifyRoutine
(
	IN PCREATE_THREAD_NOTIFY_ROUTINE NotifyRoutine
)
{
	LOG_FUNC_ONE_ARG(NotifyRoutine);

	std::unique_lock lck(g_ThreadNotificationMtx);
	for (int i = 0; i < PSP_MAX_CREATE_THREAD_NOTIFY; i++) {
		if (g_pfnThreadNotification[i] == zeroptr) {
			g_pfnThreadNotification[i] = NotifyRoutine;
			g_iThreadNotificationCount++;
			RETURN(X_STATUS_SUCCESS);
		}
	}

	RETURN(X_STATUS_INSUFFICIENT_RESOURCES);
}

// ******************************************************************
// * 0x0102 - PsTerminateSystemThread()
// ******************************************************************
// Exits the current system thread.  Must be called from a system thread.
//
// Differences from NT: None.
XBSYSAPI EXPORTNUM(258) xbox::void_xt NTAPI xbox::PsTerminateSystemThread
(
	IN ntstatus_xt ExitStatus
)
{
	LOG_FUNC_ONE_ARG(ExitStatus);

	xbox::PETHREAD eThread = xbox::PspGetCurrentThread();
	eThread->Tcb.HasTerminated = 1;

	KfLowerIrql(PASSIVE_LEVEL);

	if (eThread->UniqueThread && g_iThreadNotificationCount) {
		PspCallThreadNotificationRoutines(eThread, FALSE);
	}

	KeEmptyQueueApc();

	// Emulate our exit strategy for GetExitCodeThread
	KeQuerySystemTime(&eThread->ExitTime);
	eThread->ExitStatus = ExitStatus;
	eThread->Tcb.Header.SignalState = 1;
	KiWaitListLock();
	if (!IsListEmpty(&eThread->Tcb.Header.WaitListHead)) {
		KiWaitTest((PVOID)&eThread->Tcb, 0);
		std::this_thread::yield();
	}
	else {
		KiWaitListUnlock();
	}

	if (GetNativeHandle(eThread->UniqueThread)) {
		NtClose(eThread->UniqueThread);
		eThread->UniqueThread = xbox::zeroptr;
	}

	// Remove thread from the process
	RemoveEntryList(&eThread->Tcb.ThreadListEntry);
	eThread->Tcb.State = Terminated;
	KiUniqueProcess.StackCount--;

	// PspReaperRoutine technically free'd the memory allocation from MmCreateKernelStack function.
	// Therefore is run from another thread.
	InsertTailList(&PspReaperListHead, &((PETHREAD)eThread)->ReaperLink);
	KeInsertQueueDpc(&PsReaperDpc, NULL, NULL);

	EmuKeFreePcr();

	_endthreadex(ExitStatus);
}

// ******************************************************************
// * 0x0103 - PsThreadObjectType
// ******************************************************************
XBSYSAPI EXPORTNUM(259) xbox::OBJECT_TYPE xbox::PsThreadObjectType =
{
	xbox::ExAllocatePoolWithTag,
	xbox::ExFreePool,
	NULL,
	NULL,
	NULL,
	(PVOID)offsetof(xbox::KTHREAD, Header),
	'erhT' // = first four characters of "Thread" in reverse
};
