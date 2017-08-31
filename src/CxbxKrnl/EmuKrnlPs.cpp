// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuKrnlPs.cpp
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
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

// prevent name collisions
namespace xboxkrnl
{
#include <xboxkrnl/xboxkrnl.h> // For PsCreateSystemThreadEx, etc.
};

#include <process.h> // For __beginthreadex(), etc.

#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnlLogging.h"
#include "CxbxKrnl.h" // For CxbxKrnl_TLS
#include "Emu.h" // For EmuWarning()
#include "EmuFS.h" // For EmuGenerateFS
#include "EmuXTL.h"

// prevent name collisions
namespace NtDll
{
#include "EmuNtDll.h"
};

// PsCreateSystemThread proxy parameters
typedef struct _PCSTProxyParam
{
	IN PVOID  StartRoutine;
	IN PVOID  StartContext;
	IN PVOID  SystemRoutine;
	IN BOOL   StartSuspended;
	IN HANDLE hStartedEvent;
}
PCSTProxyParam;

// Global Variable(s)
extern PVOID g_pfnThreadNotification[PSP_MAX_CREATE_THREAD_NOTIFY] = { NULL };
extern int g_iThreadNotificationCount = 0;

// Separate function for logging, otherwise in PCSTProxy __try wont work (Compiler Error C2712)
void LOG_PCSTProxy
(
	PVOID StartRoutine,
	PVOID StartContext,
	PVOID SystemRoutine,
	BOOL   StartSuspended,
	HANDLE hStartedEvent
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(StartRoutine)
		LOG_FUNC_ARG(StartContext)
		LOG_FUNC_ARG(SystemRoutine)
		LOG_FUNC_ARG(StartSuspended)
		LOG_FUNC_ARG(hStartedEvent)
		LOG_FUNC_END;
}

void InitXboxThread(DWORD_PTR cores)
{
	// initialize FS segment selector
	EmuGenerateFS(CxbxKrnl_TLS, CxbxKrnl_TLSData);

	_controlfp(_PC_53, _MCW_PC); // Set Precision control to 53 bits (verified setting)
	_controlfp(_RC_NEAR, _MCW_RC); // Set Rounding control to near (unsure about this)

	// Run this thread solely on the indicated core(s) :
	SetThreadAffinityMask(GetCurrentThread(), cores);
}

// PsCreateSystemThread proxy procedure
#pragma warning(push)
#pragma warning(disable: 4731)  // disable ebp modification warning
// Dxbx Note : The signature of PCSTProxy should conform to System.TThreadFunc !
static unsigned int WINAPI PCSTProxy
(
	IN PVOID Parameter
)
{
	PCSTProxyParam *iPCSTProxyParam = (PCSTProxyParam*)Parameter;

	PVOID StartRoutine = iPCSTProxyParam->StartRoutine;
	PVOID StartContext = iPCSTProxyParam->StartContext;
	PVOID SystemRoutine = iPCSTProxyParam->SystemRoutine;
	BOOL StartSuspended = iPCSTProxyParam->StartSuspended;
	HANDLE hStartedEvent = iPCSTProxyParam->hStartedEvent;

	// Once deleted, unable to directly access iPCSTProxyParam in remainder of function.
	delete iPCSTProxyParam;

	LOG_PCSTProxy(
		StartRoutine,
		StartContext,
		SystemRoutine,
		StartSuspended,
		hStartedEvent);

	// Do minimal thread initialization
	InitXboxThread(g_CPUXbox);

	if (StartSuspended == TRUE)
		// Suspend right before calling the thread notification routines
		SuspendThread(GetCurrentThread());

	// call thread notification routine(s)
	if (g_iThreadNotificationCount != 0)
	{
		for (int i = 0; i < 16; i++)
		{
			XTL::XTHREAD_NOTIFY_PROC pfnNotificationRoutine = (XTL::XTHREAD_NOTIFY_PROC)g_pfnThreadNotification[i];

			// If the routine doesn't exist, don't execute it!
			if (pfnNotificationRoutine == NULL)
				continue;

			DbgPrintf("EmuKrnl: Calling pfnNotificationRoutine[%d] (0x%.08X)\n", g_iThreadNotificationCount, pfnNotificationRoutine);

			pfnNotificationRoutine(TRUE);
		}
	}

	// use the special calling convention
	__try
	{
		SetEvent(hStartedEvent);

		// Given the non-standard calling convention (requiring
		// the first argument in ebp+4) we need the below __asm.
		//
		// Otherwise, this call would have looked something like this :
		// ((xboxkrnl::PKSYSTEM_ROUTINE)SystemRoutine)(
		//	  (xboxkrnl::PKSTART_ROUTINE)StartRoutine, 
		//	  StartContext);
		__asm
		{
			mov         esi, SystemRoutine
			push        StartContext
			push        StartRoutine
			push        offset callComplete
			lea         ebp, [esp - 4]
			jmp near    esi
		}

	}
	__except (EmuException(GetExceptionInformation()))
	{
		EmuWarning("Problem with ExceptionFilter!");
	}

callComplete:

	// This will also handle thread notification :
	xboxkrnl::PsTerminateSystemThread(STATUS_SUCCESS);

	return 0; // will never be reached
}
#pragma warning(pop)

// Placeholder system function, instead of XapiThreadStartup
void PspSystemThreadStartup
(
	IN xboxkrnl::PKSTART_ROUTINE StartRoutine,
	IN PVOID StartContext
)
{
	__try
	{
		(StartRoutine)(StartContext);
	}
	__except (EmuException(GetExceptionInformation()))
	// TODO : Call PspUnhandledExceptionInSystemThread(GetExceptionInformation())
	{
		EmuWarning("Problem with ExceptionFilter!"); // TODO : Disable?
	}

	xboxkrnl::PsTerminateSystemThread(STATUS_SUCCESS);
}

// ******************************************************************
// * 0x00FE - PsCreateSystemThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(254) xboxkrnl::NTSTATUS NTAPI xboxkrnl::PsCreateSystemThread
(
	OUT PHANDLE         ThreadHandle,
	OUT PHANDLE         ThreadId OPTIONAL,
	IN  PKSTART_ROUTINE StartRoutine,
	IN  PVOID           StartContext,
	IN  BOOLEAN         DebuggerThread
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
XBSYSAPI EXPORTNUM(255) xboxkrnl::NTSTATUS NTAPI xboxkrnl::PsCreateSystemThreadEx
(
	OUT PHANDLE         ThreadHandle,
	IN  ULONG           ThreadExtensionSize,
	IN  ULONG           KernelStackSize,
	IN  ULONG           TlsDataSize,
	OUT PHANDLE         ThreadId OPTIONAL,
	IN  PKSTART_ROUTINE StartRoutine,
	IN  PVOID           StartContext,
	IN  BOOLEAN         CreateSuspended,
	IN  BOOLEAN         DebuggerThread,
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

	// TODO : Arguments to use : KernelStackSize, TlsDataSize, DebuggerThread

	// TODO : Fill KernelStackSize like this :
	//	if (KernelStackSize < KERNEL_STACK_SIZE) 
	//		KernelStackSize = KERNEL_STACK_SIZE;
	//	else
	//		KernelStackSize = round up;

	static bool bFirstTime = false;

	// create thread, using our special proxy technique
	{
		DWORD dwThreadId;

		// PCSTProxy is responsible for cleaning up this pointer
		::PCSTProxyParam *iPCSTProxyParam = new ::PCSTProxyParam();

		iPCSTProxyParam->StartRoutine = StartRoutine;
		iPCSTProxyParam->StartContext = StartContext;
		iPCSTProxyParam->SystemRoutine = SystemRoutine; // NULL, XapiThreadStartup or unknown?
		iPCSTProxyParam->StartSuspended = CreateSuspended;
		iPCSTProxyParam->hStartedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		*ThreadHandle = (HANDLE)_beginthreadex(NULL, NULL, PCSTProxy, iPCSTProxyParam, NULL, (uint*)&dwThreadId);

		WaitForSingleObject(iPCSTProxyParam->hStartedEvent, 1000);

		DbgPrintf("EmuKrnl: ThreadHandle : 0x%X, ThreadId : 0x%.08X\n", *ThreadHandle, dwThreadId);

		// we must duplicate this handle in order to retain Suspend/Resume thread rights from a remote thread
		{
			HANDLE hDupHandle = NULL;

			DuplicateHandle(g_CurrentProcessHandle, *ThreadHandle, g_CurrentProcessHandle, &hDupHandle, 0, FALSE, DUPLICATE_SAME_ACCESS);

			CxbxKrnlRegisterThread(hDupHandle);
		}

		if (ThreadId != NULL)
			*ThreadId = (xboxkrnl::HANDLE)dwThreadId;
	}

	RETURN(STATUS_SUCCESS);
}

// ******************************************************************
// * 0x0100 - PsQueryStatistics()
// ******************************************************************
XBSYSAPI EXPORTNUM(256) xboxkrnl::NTSTATUS NTAPI xboxkrnl::PsQueryStatistics
(
	IN OUT PPS_STATISTICS ProcessStatistics
)
{
	LOG_FUNC_ONE_ARG_OUT(ProcessStatistics);

	NTSTATUS ret = STATUS_SUCCESS;

	if (ProcessStatistics->Length == sizeof(PS_STATISTICS)) {
		LOG_INCOMPLETE(); // TODO : Return number of threads and handles that currently exist
		ProcessStatistics->ThreadCount = 1;
		ProcessStatistics->HandleCount = 1;
	} else {
		ret = STATUS_INVALID_PARAMETER;
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x0101 - PsSetCreateThreadNotifyRoutine()
// ******************************************************************
XBSYSAPI EXPORTNUM(257) xboxkrnl::NTSTATUS NTAPI xboxkrnl::PsSetCreateThreadNotifyRoutine
(
	IN PCREATE_THREAD_NOTIFY_ROUTINE NotifyRoutine
)
{
	LOG_FUNC_ONE_ARG(NotifyRoutine);

	NTSTATUS ret = STATUS_INSUFFICIENT_RESOURCES;

	// Taken from XTL::EmuXRegisterThreadNotifyRoutine (perhaps that can be removed now) :

	// I honestly don't expect this to happen, but if it does...
	if (g_iThreadNotificationCount >= PSP_MAX_CREATE_THREAD_NOTIFY)
		CxbxKrnlCleanup("Too many thread notification routines installed\n"
			"If you're reading this message than tell blueshogun you saw it!!!");

	// Find an empty spot in the thread notification array
	for (int i = 0; i < PSP_MAX_CREATE_THREAD_NOTIFY; i++)
	{
		// If we find one, then add it to the array, and break the loop so
		// that we don't accidently register the same routine twice!
		if (g_pfnThreadNotification[i] == NULL)
		{
			g_pfnThreadNotification[i] = NotifyRoutine;
			g_iThreadNotificationCount++;
			ret = STATUS_SUCCESS;
			break;
		}
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x0102 - PsTerminateSystemThread()
// ******************************************************************
// Exits the current system thread.  Must be called from a system thread.
//
// Differences from NT: None.
XBSYSAPI EXPORTNUM(258) xboxkrnl::VOID NTAPI xboxkrnl::PsTerminateSystemThread
(
	IN NTSTATUS ExitStatus
)
{
	LOG_FUNC_ONE_ARG(ExitStatus);

	// call thread notification routine(s)
	if (g_iThreadNotificationCount != 0)
	{
		for (int i = 0; i < 16; i++)
		{
			XTL::XTHREAD_NOTIFY_PROC pfnNotificationRoutine = (XTL::XTHREAD_NOTIFY_PROC)g_pfnThreadNotification[i];

			// If the routine doesn't exist, don't execute it!
			if (pfnNotificationRoutine == NULL)
				continue;

			DbgPrintf("EmuKrnl: Calling pfnNotificationRoutine[%d] (0x%.08X)\n", g_iThreadNotificationCount, pfnNotificationRoutine);

			pfnNotificationRoutine(FALSE);
		}
	}

	_endthreadex(ExitStatus);
	// ExitThread(ExitStatus);
	// CxbxKrnlTerminateThread();
}

// ******************************************************************
// * 0x0103 - PsThreadObjectType
// ******************************************************************
XBSYSAPI EXPORTNUM(259) xboxkrnl::OBJECT_TYPE VOLATILE xboxkrnl::PsThreadObjectType =
{
	xboxkrnl::ExAllocatePoolWithTag,
	xboxkrnl::ExFreePool,
	NULL,
	NULL,
	NULL,
	(PVOID)offsetof(xboxkrnl::KTHREAD, Header),
	'erhT' // = first four characters of "Thread" in reverse
};
