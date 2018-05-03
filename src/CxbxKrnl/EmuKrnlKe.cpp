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
// *   Cxbx->Win32->CxbxKrnl->EmuKrnlKe.cpp
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
#define _XBOXKRNL_DEFEXTRN_

#define LOG_PREFIX "KRNL"

// prevent name collisions
namespace xboxkrnl
{
#include <xboxkrnl/xboxkrnl.h> // For KeBugCheck, etc.
};

#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnlLogging.h"

// prevent name collisions
namespace NtDll
{
	#include "EmuNtDll.h" // For NtDelayExecution(), etc.
};

#include "CxbxKrnl.h" // For CxbxKrnlCleanup
#include "Emu.h" // For EmuWarning()
#include "EmuKrnl.h" // For InitializeListHead(), etc.
#include "EmuKrnlKi.h" // For KiRemoveTreeTimer(), KiInsertTreeTimer()
#include "EmuFile.h" // For IsEmuHandle(), NtStatusToString()

#include <chrono>
#include <thread>
#include <windows.h>
#include <map>

// Copied over from Dxbx. 
// TODO : Move towards thread-simulation based Dpc emulation
typedef struct _DpcData {
	CRITICAL_SECTION Lock;
	HANDLE DpcEvent;
	xboxkrnl::LIST_ENTRY DpcQueue; // TODO : Use KeGetCurrentPrcb()->DpcListHead instead
	xboxkrnl::LIST_ENTRY TimerQueue;
} DpcData;

DpcData g_DpcData = { 0 }; // Note : g_DpcData is initialized in InitDpcAndTimerThread()

std::map<xboxkrnl::PRKEVENT, HANDLE> g_KeEventHandles;

xboxkrnl::ULONGLONG LARGE_INTEGER2ULONGLONG(xboxkrnl::LARGE_INTEGER value)
{
	// Weird construction because there doesn't seem to exist an implicit
	// conversion of LARGE_INTEGER to ULONGLONG :
	return *((PULONGLONG)&value);
}

// ******************************************************************
// * KeGetPcr()
// * NOTE: This is a macro on the Xbox, however we implement it 
// * as a function so it can suit our emulated KPCR structure
// ******************************************************************
xboxkrnl::KPCR* KeGetPcr()
{
	xboxkrnl::PKPCR Pcr;

	// See EmuKeSetPcr()
	Pcr = (xboxkrnl::PKPCR)__readfsdword(TIB_ArbitraryDataSlot);

	if (Pcr == nullptr) {
		EmuWarning("KeGetPCR returned nullptr: Was this called from a non-xbox thread?");
		// Attempt to salvage the situation by calling InitXboxThread to setup KPCR in place
		InitXboxThread(g_CPUXbox);
		Pcr = (xboxkrnl::PKPCR)__readfsdword(TIB_ArbitraryDataSlot);
	}

	return Pcr;
}

// ******************************************************************
// * KeGetCurrentPrcb()
// ******************************************************************
xboxkrnl::KPRCB *KeGetCurrentPrcb()
{
	return &(KeGetPcr()->PrcbData);
}

// Forward KeLowerIrql() to KfLowerIrql()
#define KeLowerIrql(NewIrql) \
	KfLowerIrql(NewIrql)

// Forward KeRaiseIrql() to KfRaiseIrql()
#define KeRaiseIrql(NewIrql, OldIrql) \
	*(OldIrql) = KfRaiseIrql(NewIrql)

ULONGLONG BootTickCount = 0;

// The Xbox GetTickCount is measured in milliseconds, just like the native GetTickCount.
// The only difference we'll take into account here, is that the Xbox will probably reboot
// much more often than Windows, so we correct this with a 'BootTickCount' value :
DWORD CxbxXboxGetTickCount()
{
	return (DWORD)(GetTickCount64() - BootTickCount);
}

DWORD ExecuteDpcQueue()
{
	xboxkrnl::PKDPC pkdpc;
	DWORD dwWait;
	DWORD dwNow;
	LONG lWait;
	xboxkrnl::PKTIMER pktimer;

	// While we're working with the DpcQueue, we need to be thread-safe :
	EnterCriticalSection(&(g_DpcData.Lock));

//    if (g_DpcData._fShutdown)
//        break; // while 

//    Assert(g_DpcData._dwThreadId == GetCurrentThreadId());
//    Assert(g_DpcData._dwDpcThreadId == 0);
//    g_DpcData._dwDpcThreadId = g_DpcData._dwThreadId;
//    Assert(g_DpcData._dwDpcThreadId != 0);

	// Are there entries in the DpqQueue?
	while (!IsListEmpty(&(g_DpcData.DpcQueue)))
	{
		// Extract the head entry and retrieve the containing KDPC pointer for it:
		pkdpc = CONTAINING_RECORD(RemoveHeadList(&(g_DpcData.DpcQueue)), xboxkrnl::KDPC, DpcListEntry);
		// Mark it as no longer linked into the DpcQueue
		pkdpc->Inserted = FALSE;
		// Set DpcRoutineActive to support KeIsExecutingDpc:
		KeGetCurrentPrcb()->DpcRoutineActive = TRUE; // Experimental
		DbgPrintf("KRNL: Global DpcQueue, calling DPC at 0x%.8X\n", pkdpc->DeferredRoutine);
		__try {
			// Call the Deferred Procedure  :
			pkdpc->DeferredRoutine(
				pkdpc,
				pkdpc->DeferredContext,
				pkdpc->SystemArgument1,
				pkdpc->SystemArgument2);
		} __except (EmuException(GetExceptionInformation()))
		{
			EmuWarning("Problem with ExceptionFilter!");
		}

		KeGetCurrentPrcb()->DpcRoutineActive = FALSE; // Experimental
	}

	dwWait = INFINITE;
	if (!IsListEmpty(&(g_DpcData.TimerQueue)))
	{
		while (true)
		{
			dwNow = CxbxXboxGetTickCount();
			dwWait = INFINITE;
			pktimer = (xboxkrnl::PKTIMER)g_DpcData.TimerQueue.Flink;
			pkdpc = nullptr;
			while (pktimer != (xboxkrnl::PKTIMER)&(g_DpcData.TimerQueue))
			{
				lWait = (LONG)pktimer->DueTime.u.LowPart - dwNow;
				if (lWait <= 0)
				{
					pktimer->DueTime.u.LowPart = pktimer->Period + dwNow;
					pkdpc = pktimer->Dpc;
					break; // while
				}

				if (dwWait > (DWORD)lWait)
					dwWait = (DWORD)lWait;

				pktimer = (xboxkrnl::PKTIMER)pktimer->TimerListEntry.Flink;
			}

			if (pkdpc == nullptr)
				break; // while

			DbgPrintf("KRNL: Global TimerQueue, calling DPC at 0x%.8X\n", pkdpc->DeferredRoutine);
			__try {
				pkdpc->DeferredRoutine(
					pkdpc,
					pkdpc->DeferredContext,
					pkdpc->SystemArgument1,
					pkdpc->SystemArgument2);
			} __except (EmuException(GetExceptionInformation()))
			{
				EmuWarning("Problem with ExceptionFilter!");
			}
		}
	}

//    Assert(g_DpcData._dwThreadId == GetCurrentThreadId());
//    Assert(g_DpcData._dwDpcThreadId == g_DpcData._dwThreadId);
//    g_DpcData._dwDpcThreadId = 0;
	LeaveCriticalSection(&(g_DpcData.Lock));

	return dwWait;
}

void InitDpcAndTimerThread()
{
	DWORD dwThreadId = 0;

	InitializeCriticalSection(&(g_DpcData.Lock));
	InitializeListHead(&(g_DpcData.DpcQueue));
	InitializeListHead(&(g_DpcData.TimerQueue));

	DbgPrintf("INIT: Creating DPC event\n");
	g_DpcData.DpcEvent = CreateEvent(/*lpEventAttributes=*/nullptr, /*bManualReset=*/FALSE, /*bInitialState=*/FALSE, /*lpName=*/nullptr);
}

// Xbox Performance Counter Frequency = 733333333 (CPU Clock)
#define XBOX_PERFORMANCE_FREQUENCY 733333333

LARGE_INTEGER NativePerformanceCounter = { 0 };
LARGE_INTEGER NativePerformanceFrequency = { 0 };
double NativeToXbox_FactorForPerformanceFrequency;

void ConnectKeInterruptTimeToThunkTable(); // forward

void CxbxInitPerformanceCounters()
{
	BootTickCount = GetTickCount64();

	// Measure current host performance counter and frequency
	QueryPerformanceCounter(&NativePerformanceCounter);
	QueryPerformanceFrequency(&NativePerformanceFrequency);
	// TODO : If anything like speed-stepping influences this, prevent or fix it here

	// Calculate the host-to-xbox performance frequency factor,
	// used the return Xbox-like results in KeQueryPerformanceCounter:
	NativeToXbox_FactorForPerformanceFrequency = (double)XBOX_PERFORMANCE_FREQUENCY / NativePerformanceFrequency.QuadPart;

	ConnectKeInterruptTimeToThunkTable();

	// Let's initialize the Dpc and timer handling thread too,
	// here for now (should be called by our caller)
	InitDpcAndTimerThread();
}

// ******************************************************************
// * 0x005C - KeAlertResumeThread()
// ******************************************************************
// Source:Dxbx
XBSYSAPI EXPORTNUM(92) xboxkrnl::NTSTATUS NTAPI xboxkrnl::KeAlertResumeThread
(
	IN HANDLE ThreadHandle,
	IN OUT PULONG PreviousSuspendCount
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(ThreadHandle)
		LOG_FUNC_ARG_OUT(PreviousSuspendCount)
		LOG_FUNC_END;

	// TODO : Result = NtDll::NtAlertResumeThread(ThreadHandle, PreviousSuspendCount);
	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x005D - KeAlertThread()
// ******************************************************************
// Source:Dxbx
XBSYSAPI EXPORTNUM(93) xboxkrnl::NTSTATUS NTAPI xboxkrnl::KeAlertThread
(
	IN HANDLE ThreadHandle
)
{
	LOG_FUNC_ONE_ARG(ThreadHandle);

// TODO : Result = NtDll::NtAlertThread(ThreadHandle);
	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x005E - KeBoostPriorityThread()
// ******************************************************************
// Source:Dxbx
XBSYSAPI EXPORTNUM(94) xboxkrnl::NTSTATUS NTAPI xboxkrnl::KeBoostPriorityThread
(
	IN PKTHREAD Thread,
	IN KPRIORITY Increment
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Thread);
		LOG_FUNC_ARG(Increment);
		LOG_FUNC_END;


	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x005F - KeBugCheck()
// ******************************************************************
XBSYSAPI EXPORTNUM(95) xboxkrnl::VOID NTAPI xboxkrnl::KeBugCheck
(
	IN ULONG BugCheckMode
)
{
	LOG_FORWARD("KeBugCheckEx");

	KeBugCheckEx(BugCheckMode, 0, 0, 0, 0);
}

// ******************************************************************
// * 0x0060 - KeBugCheckEx()
// ******************************************************************
// Source:Dxbx
XBSYSAPI EXPORTNUM(96) xboxkrnl::NTSTATUS NTAPI xboxkrnl::KeBugCheckEx
(
	IN DWORD BugCheckCode,
	IN PVOID BugCheckParameter1,
	IN PVOID BugCheckParameter2,
	IN PVOID BugCheckParameter3,
	IN PVOID BugCheckParameter4
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(BugCheckCode)
		LOG_FUNC_ARG(BugCheckParameter1)
		LOG_FUNC_ARG(BugCheckParameter2)
		LOG_FUNC_ARG(BugCheckParameter3)
		LOG_FUNC_ARG(BugCheckParameter4)
		LOG_FUNC_END;
	
	static bool KeBugCheckIgnored = false;

	if (KeBugCheckIgnored) {
		RETURN(S_OK);
	}

	char buffer[1024];
	sprintf(buffer, "The running software triggered KeBugCheck with the following information\n"
		"BugCheckCode: 0x%.8X\n"
		"BugCheckParameter1: 0x%p\n"
		"BugCheckParameter2: 0x%p\n"
		"BugCheckParameter3: 0x%p\n"
		"BugCheckParameter4: 0x%p\n"
		"\nThis is the Xbox equivalent to a BSOD and would cause the console to automatically reboot\n"
		"\nContinue Execution (Not Recommended)?\n",
		BugCheckCode, BugCheckParameter1, BugCheckParameter2, BugCheckParameter3, BugCheckParameter4);

	int result = MessageBoxA(g_hEmuWindow, buffer, "KeBugCheck", MB_YESNO | MB_ICONWARNING);

	if (result == IDNO)	{
		CxbxKrnlCleanup(NULL);
	}

	KeBugCheckIgnored = true;
	
	RETURN(S_OK);
}

// ******************************************************************
// * 0x0061 - KeCancelTimer()
// ******************************************************************
XBSYSAPI EXPORTNUM(96) xboxkrnl::BOOLEAN NTAPI xboxkrnl::KeCancelTimer
(
	IN PKTIMER Timer
)
{
	LOG_FUNC_ONE_ARG(Timer);

	BOOLEAN Inserted;

	Inserted = Timer->Header.Inserted;
	if (Inserted != FALSE) {
		// Do some unlinking if already inserted in the linked list
		KiRemoveTreeTimer(Timer);
	}

	RETURN(Inserted);
}

xboxkrnl::PKINTERRUPT EmuInterruptList[MAX_BUS_INTERRUPT_LEVEL + 1] = { 0 };

// ******************************************************************
// * 0x0062 - KeConnectInterrupt()
// ******************************************************************
XBSYSAPI EXPORTNUM(98) xboxkrnl::BOOLEAN NTAPI xboxkrnl::KeConnectInterrupt
(
	IN PKINTERRUPT  InterruptObject
)
{
	LOG_FUNC_ONE_ARG(InterruptObject);

	BOOLEAN ret = FALSE;
	KIRQL OldIrql;

	KiLockDispatcherDatabase(&OldIrql);

	// here we have to connect the interrupt object to the vector
	if (!InterruptObject->Connected)
	{
		// One interrupt per IRQ - only set when not set yet :
		if (EmuInterruptList[InterruptObject->BusInterruptLevel] == NULL)
		{
			InterruptObject->Connected = TRUE;
			EmuInterruptList[InterruptObject->BusInterruptLevel] = InterruptObject;
			HalEnableSystemInterrupt(InterruptObject->BusInterruptLevel, InterruptObject->Mode);
			ret = TRUE;
		}
	}
	// else do nothing

	KiUnlockDispatcherDatabase(OldIrql);

	RETURN(ret);
}

// ******************************************************************
// * 0x0063 - KeDelayExecutionThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(99) xboxkrnl::NTSTATUS NTAPI xboxkrnl::KeDelayExecutionThread
(
	IN KPROCESSOR_MODE  WaitMode,
	IN BOOLEAN          Alertable,
	IN PLARGE_INTEGER   Interval
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(WaitMode)
		LOG_FUNC_ARG(Alertable)
		LOG_FUNC_ARG(Interval)
		LOG_FUNC_END;

	NTSTATUS ret = NtDll::NtDelayExecution(Alertable, (NtDll::LARGE_INTEGER*)Interval);

	RETURN(ret);
}

// ******************************************************************
// * 0x0064 - KeDisconnectInterrupt()
// ******************************************************************
XBSYSAPI EXPORTNUM(100) xboxkrnl::VOID NTAPI xboxkrnl::KeDisconnectInterrupt
(
	IN PKINTERRUPT  InterruptObject
) 
{
	LOG_FUNC_ONE_ARG(InterruptObject);

	KIRQL OldIrql;

	KiLockDispatcherDatabase(&OldIrql);

	// Do the reverse of KeConnectInterrupt
	if (InterruptObject->Connected) { // Text case : d3dbvt.xbe
		// Mark InterruptObject as not connected anymore
		HalDisableSystemInterrupt(InterruptObject->BusInterruptLevel);
		EmuInterruptList[InterruptObject->BusInterruptLevel] = NULL;
		InterruptObject->Connected = FALSE;
	}

	KiUnlockDispatcherDatabase(OldIrql);
}

// ******************************************************************
// * 0x0065 - KeEnterCriticalRegion()
// ******************************************************************
XBSYSAPI EXPORTNUM(101) xboxkrnl::VOID NTAPI xboxkrnl::KeEnterCriticalRegion
(
    VOID
)
{
    LOG_FUNC();
    PKTHREAD thread = KeGetCurrentThread();
    thread->KernelApcDisable--;
}

// ******************************************************************
// * 0x0067 - KeGetCurrentIrql()
// ******************************************************************
XBSYSAPI EXPORTNUM(103) xboxkrnl::KIRQL NTAPI xboxkrnl::KeGetCurrentIrql(void)
{
	LOG_FUNC(); // TODO : Remove nested logging on this somehow, so we can call this (instead of inlining)

	KPCR* Pcr = KeGetPcr();
	KIRQL Irql = (KIRQL)Pcr->Irql;

	RETURN_TYPE(KIRQL_TYPE, Irql);
}

// ******************************************************************
// * 0x0068 - KeGetCurrentThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(104) xboxkrnl::PKTHREAD NTAPI xboxkrnl::KeGetCurrentThread(void)
{
	LOG_FUNC();

	// Probably correct, but untested and currently faked in EmuGenerateFS
	// (to make this correct, we need to improve our thread emulation)
	KTHREAD *ret = KeGetCurrentPrcb()->CurrentThread;
	
	RETURN(ret);
}

// ******************************************************************
// * 0x0069 - KeInitializeApc()
// ******************************************************************
XBSYSAPI EXPORTNUM(105) xboxkrnl::VOID NTAPI xboxkrnl::KeInitializeApc
(
	IN PKAPC Apc,
	IN PKTHREAD Thread,
	IN PKKERNEL_ROUTINE KernelRoutine,
	IN PKRUNDOWN_ROUTINE RundownRoutine OPTIONAL,
	IN PKNORMAL_ROUTINE NormalRoutine OPTIONAL,
	IN KPROCESSOR_MODE ApcMode OPTIONAL,
	IN PVOID NormalContext OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Apc)
		LOG_FUNC_ARG(Thread)
		LOG_FUNC_ARG(KernelRoutine)
		LOG_FUNC_ARG(RundownRoutine)
		LOG_FUNC_ARG(NormalRoutine)
		LOG_FUNC_ARG(ApcMode)
		LOG_FUNC_ARG(NormalContext)
		LOG_FUNC_END;

	// inialize Apc field values
	Apc->Type = ApcObject;
	Apc->ApcMode = ApcMode;
	Apc->Inserted = FALSE;
	Apc->Thread = Thread;
	Apc->KernelRoutine = KernelRoutine;
	Apc->RundownRoutine = RundownRoutine;
	Apc->NormalRoutine = NormalRoutine;
	Apc->NormalContext = NormalContext;
	if (NormalRoutine == NULL) {
		Apc->ApcMode = KernelMode;
		Apc->NormalContext = NULL;
	}
}

// ******************************************************************
// * 0x006A - KeInitializeDeviceQueue()
// ******************************************************************
XBSYSAPI EXPORTNUM(106) xboxkrnl::VOID NTAPI xboxkrnl::KeInitializeDeviceQueue
(
	OUT PKDEVICE_QUEUE DeviceQueue
)
{
	LOG_FUNC_ONE_ARG_OUT(DeviceQueue);

	DeviceQueue->Type = DeviceQueueObject;
	DeviceQueue->Size = sizeof(KDEVICE_QUEUE);
	DeviceQueue->Busy = FALSE;

	InitializeListHead(&DeviceQueue->DeviceListHead);
}

// ******************************************************************
// * 0x006B - KeInitializeDpc()
// ******************************************************************
XBSYSAPI EXPORTNUM(107) xboxkrnl::VOID NTAPI xboxkrnl::KeInitializeDpc
(
	KDPC                *Dpc,
	PKDEFERRED_ROUTINE   DeferredRoutine,
	PVOID                DeferredContext
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Dpc)
		LOG_FUNC_ARG(DeferredRoutine)
		LOG_FUNC_ARG(DeferredContext)
		LOG_FUNC_END;

	// inialize Dpc field values
	Dpc->Type = DpcObject;
	Dpc->Inserted = FALSE;
	Dpc->DeferredRoutine = DeferredRoutine;
	Dpc->DeferredContext = DeferredContext;
}

// ******************************************************************
// * 0x006C - KeInitializeEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(108) xboxkrnl::VOID NTAPI xboxkrnl::KeInitializeEvent
(
	IN PRKEVENT Event,
	IN EVENT_TYPE Type,
	IN BOOLEAN SignalState
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Event)
		LOG_FUNC_ARG(Type)
		LOG_FUNC_ARG(SignalState)
		LOG_FUNC_END;

	// Setup the Xbox event struct
	Event->Header.Type = Type;
	Event->Header.Size = sizeof(KEVENT) / sizeof(LONG);
	Event->Header.SignalState = SignalState;
	InitializeListHead(&(Event->Header.WaitListHead)); 


	// Create a Windows event, to be used in KeWaitForObject
	// TODO: This doesn't check for events that are already initialized
	// This shouldn't happen, except on shoddily coded titles so we
	// ignore it for now
	HANDLE hostEvent = CreateEvent(NULL, FALSE, SignalState, NULL);
	g_KeEventHandles[Event] = hostEvent;
}

// ******************************************************************
// * 0x006D - KeInitializeInterrupt()
// ******************************************************************
XBSYSAPI EXPORTNUM(109) xboxkrnl::VOID NTAPI xboxkrnl::KeInitializeInterrupt
(
	OUT PKINTERRUPT Interrupt,
	IN PKSERVICE_ROUTINE ServiceRoutine,
	IN PVOID ServiceContext,
	IN ULONG Vector,
	IN KIRQL Irql,
	IN KINTERRUPT_MODE InterruptMode,
	IN BOOLEAN ShareVector
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(Interrupt)
		LOG_FUNC_ARG(ServiceRoutine)
		LOG_FUNC_ARG(ServiceContext)
		LOG_FUNC_ARG(Vector)
		LOG_FUNC_ARG_TYPE(KIRQL_TYPE, Irql)
		LOG_FUNC_ARG(InterruptMode)
		LOG_FUNC_ARG(ShareVector) 
		LOG_FUNC_END;

	Interrupt->ServiceRoutine = ServiceRoutine;
	Interrupt->ServiceContext = ServiceContext;
	Interrupt->BusInterruptLevel = VECTOR2IRQ(Vector);
	Interrupt->Irql = Irql;
	Interrupt->Connected = FALSE;
	// Unused : Interrupt->ShareVector = ShareVector;
	Interrupt->Mode = InterruptMode;
	// Interrupt->ServiceCount = 0; // not neccesary?

	// Interrupt->DispatchCode = []?; //TODO : Populate this interrupt dispatch
	// code block, patch it up so it works with the address of this Interrupt
	// struct and calls the right dispatch routine (depending on InterruptMode). 
	LOG_INCOMPLETE();
}

// ******************************************************************
// * 0x006E - KeInitializeMutant()
// ******************************************************************
XBSYSAPI EXPORTNUM(110) xboxkrnl::VOID NTAPI xboxkrnl::KeInitializeMutant
(
	IN PRKMUTANT Mutant,
	IN BOOLEAN InitialOwner
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Mutant)
		LOG_FUNC_ARG(InitialOwner)
		LOG_FUNC_END;

	// Initialize header :
	Mutant->Header.Type = MutantObject;
	Mutant->Header.Size = sizeof(KMUTANT) / sizeof(LONG);
	InitializeListHead(&Mutant->Header.WaitListHead);
	// Initialize specific fields :
	Mutant->Abandoned = FALSE;
	if (InitialOwner == TRUE) {
		PKTHREAD pThread = KeGetCurrentThread();
		Mutant->Header.SignalState = 0;
		Mutant->OwnerThread = pThread;

		KIRQL oldIRQL;
		KiLockDispatcherDatabase(&oldIRQL);

		PLIST_ENTRY pMutantList = pThread->MutantListHead.Blink;
		InsertHeadList(pMutantList, &Mutant->MutantListEntry);

		KiUnlockDispatcherDatabase(oldIRQL);
	}
	else {
		Mutant->Header.SignalState = 1;
		Mutant->OwnerThread = NULL;
	}
}

// ******************************************************************
// * 0x006F - KeInitializeQueue()
// ******************************************************************
XBSYSAPI EXPORTNUM(111) xboxkrnl::VOID NTAPI xboxkrnl::KeInitializeQueue
(
	IN PKQUEUE Queue,
	IN ULONG Count OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Queue)
		LOG_FUNC_ARG(Count)
		LOG_FUNC_END;

	// Initialize header :
	Queue->Header.Type = QueueObject;
	Queue->Header.Size = sizeof(KQUEUE) / sizeof(LONG);
	Queue->Header.SignalState = 0;
	InitializeListHead(&Queue->Header.WaitListHead);
	// Initialize specific fields :
	InitializeListHead(&Queue->EntryListHead);
	InitializeListHead(&Queue->ThreadListHead);
	Queue->CurrentCount = 0;
	Queue->MaximumCount = (Count > 1) ? Count : 1;
}

// ******************************************************************
// * 0x0070 - KeInitializeSemaphore()
// ******************************************************************
XBSYSAPI EXPORTNUM(112) xboxkrnl::VOID NTAPI xboxkrnl::KeInitializeSemaphore
(
	IN PRKSEMAPHORE Semaphore,
	IN LONG Count,
	IN LONG Limit
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Semaphore)
		LOG_FUNC_ARG(Count)
		LOG_FUNC_ARG(Limit)
		LOG_FUNC_END;

	// Initialize header :
	Semaphore->Header.Type = SemaphoreObject;
	Semaphore->Header.Size = sizeof(KSEMAPHORE) / sizeof(LONG);
	Semaphore->Header.SignalState = Count;
	InitializeListHead(&Semaphore->Header.WaitListHead);
	// Initialize specific fields :
	Semaphore->Limit = Limit;
}

// ******************************************************************
// * 0x0071 - KeInitializeTimerEx()
// ******************************************************************
XBSYSAPI EXPORTNUM(113) xboxkrnl::VOID NTAPI xboxkrnl::KeInitializeTimerEx
(
	IN PKTIMER      Timer,
	IN TIMER_TYPE   Type
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Timer)
		LOG_FUNC_ARG(Type)
		LOG_FUNC_END;

	// Initialize header :
	Timer->Header.Type = Type + TimerNotificationObject;
	Timer->Header.Inserted = FALSE;
	Timer->Header.Size = sizeof(KTIMER) / sizeof(ULONG);
	Timer->Header.SignalState = 0;
	InitializeListHead(&(Timer->Header.WaitListHead));
	// Initialize specific fields :
	Timer->TimerListEntry.Blink = NULL;
	Timer->TimerListEntry.Flink = NULL;
	Timer->DueTime.QuadPart = 0;
	Timer->Period = 0;
}

XBSYSAPI EXPORTNUM(114) xboxkrnl::BOOLEAN NTAPI xboxkrnl::KeInsertByKeyDeviceQueue
(
	IN PKDEVICE_QUEUE DeviceQueue,
	IN PKDEVICE_QUEUE_ENTRY DeviceQueueEntry,
	IN ULONG SortKey
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(DeviceQueue)
		LOG_FUNC_ARG(DeviceQueueEntry)
		LOG_FUNC_ARG(SortKey)
		LOG_FUNC_END;

	BOOLEAN Res = FALSE;

	// We should lock the device queue here

	DeviceQueueEntry->SortKey = SortKey;
	if (DeviceQueue->Busy) {
		LIST_ENTRY *pListEntry = DeviceQueue->DeviceListHead.Flink;
		while (pListEntry != &DeviceQueue->DeviceListHead) {
			KDEVICE_QUEUE_ENTRY *pQueueEntry = CONTAINING_RECORD(pListEntry, KDEVICE_QUEUE_ENTRY, DeviceListEntry);
			if (SortKey < pQueueEntry->SortKey) {
				break;
			}
			pListEntry = pListEntry->Flink;
		}

		pListEntry = pListEntry->Blink;
		InsertHeadList(pListEntry, &DeviceQueueEntry->DeviceListEntry);
		Res = TRUE;
	}
	else {
		DeviceQueue->Busy = TRUE;
	}

	DeviceQueueEntry->Inserted = Res;

	// We should unlock the device queue here

	RETURN(Res);
}

// ******************************************************************
// * 0x0073 - KeInsertDeviceQueue()
// * This implementation is inspired by ReactOS source code
// * Ref: https://github.com/reactos/reactos/blob/master/ntoskrnl/ke/devqueue.c
// ******************************************************************
XBSYSAPI EXPORTNUM(115) xboxkrnl::BOOLEAN NTAPI xboxkrnl::KeInsertDeviceQueue
(
	IN PKDEVICE_QUEUE DeviceQueue,
	IN PKDEVICE_QUEUE_ENTRY DeviceQueueEntry
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(DeviceQueue)
		LOG_FUNC_ARG(DeviceQueueEntry)
		LOG_FUNC_END;

	BOOLEAN Res = FALSE;

	// We should lock the device queue here
	
	if (DeviceQueue->Busy == TRUE) {
		InsertTailList(&DeviceQueue->DeviceListHead, &DeviceQueueEntry->DeviceListEntry);
		Res = TRUE;
	}
	else {
		DeviceQueue->Busy = TRUE;
	}

	DeviceQueueEntry->Inserted = Res;

	// We should unlock the device queue here

	RETURN(Res);
}

XBSYSAPI EXPORTNUM(116) xboxkrnl::LONG NTAPI xboxkrnl::KeInsertHeadQueue
(
	IN PRKQUEUE Queue,
	IN PLIST_ENTRY Entry
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Queue)
		LOG_FUNC_ARG(Entry)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(0);
}

XBSYSAPI EXPORTNUM(117) xboxkrnl::LONG NTAPI xboxkrnl::KeInsertQueue
(
	IN PRKQUEUE Queue,
	IN PLIST_ENTRY Entry
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Queue)
		LOG_FUNC_ARG(Entry)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(0);
}

XBSYSAPI EXPORTNUM(118) xboxkrnl::BOOLEAN NTAPI xboxkrnl::KeInsertQueueApc
(
	IN PRKAPC Apc,
	IN PVOID SystemArgument1,
	IN PVOID SystemArgument2,
	IN KPRIORITY Increment
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Apc)
		LOG_FUNC_ARG(SystemArgument1)
		LOG_FUNC_ARG(SystemArgument2)
		LOG_FUNC_ARG(Increment)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(TRUE);
}

// ******************************************************************
// * 0x0077 - KeInsertQueueDpc()
// ******************************************************************
XBSYSAPI EXPORTNUM(119) xboxkrnl::BOOLEAN NTAPI xboxkrnl::KeInsertQueueDpc
(
	IN PKDPC        Dpc,
	IN PVOID        SystemArgument1,
	IN PVOID        SystemArgument2
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Dpc)
		LOG_FUNC_ARG(SystemArgument1)
		LOG_FUNC_ARG(SystemArgument2)
		LOG_FUNC_END;

	// For thread safety, enter the Dpc lock:
	EnterCriticalSection(&(g_DpcData.Lock));
	// TODO : Instead, disable interrupts - use KeRaiseIrql(HIGH_LEVEL, &(KIRQL)OldIrql) ?

	BOOLEAN NeedsInsertion = (Dpc->Inserted == FALSE);

	if (NeedsInsertion) {
		// Remember the arguments and link it into our DpcQueue :
		Dpc->Inserted = TRUE;
		Dpc->SystemArgument1 = SystemArgument1;
		Dpc->SystemArgument2 = SystemArgument2;
		InsertTailList(&(g_DpcData.DpcQueue), &(Dpc->DpcListEntry));
		// TODO : Instead of DpcQueue, add the DPC to KeGetCurrentPrcb()->DpcListHead
		// Signal the Dpc handling code there's work to do
		HalRequestSoftwareInterrupt(DISPATCH_LEVEL);
		// OpenXbox has this instead:
		// if (!pKPRCB->DpcRoutineActive && !pKPRCB->DpcInterruptRequested) {
		//	pKPRCB->DpcInterruptRequested = TRUE;
	}

	// Thread-safety is no longer required anymore
	LeaveCriticalSection(&(g_DpcData.Lock));
	// TODO : Instead, enable interrupts - use KeLowerIrql(OldIrql) ?

	RETURN(NeedsInsertion);
}

// ******************************************************************
// * 0x0079 - KeIsExecutingDpc()
// ******************************************************************
XBSYSAPI EXPORTNUM(121) xboxkrnl::BOOLEAN NTAPI xboxkrnl::KeIsExecutingDpc
()
{
	LOG_FUNC();

	BOOLEAN ret = (BOOLEAN)KeGetCurrentPrcb()->DpcRoutineActive;

	RETURN(ret);
}

// ******************************************************************
// * 0x0078 - KeInterruptTime
// ******************************************************************
// Dxbx note : This was once a value, but instead we now point to
// the native Windows versions (see ConnectWindowsTimersToThunkTable) :
XBSYSAPI EXPORTNUM(120) xboxkrnl::PKSYSTEM_TIME xboxkrnl::KeInterruptTime = nullptr; // Set by ConnectKeInterruptTimeToThunkTable

void ConnectKeInterruptTimeToThunkTable()
{
	xboxkrnl::KeInterruptTime = (xboxkrnl::PKSYSTEM_TIME)CxbxKrnl_KernelThunkTable[120];
}

// ******************************************************************
// * 0x007A - KeLeaveCriticalRegion()
// ******************************************************************
XBSYSAPI EXPORTNUM(122) xboxkrnl::VOID NTAPI xboxkrnl::KeLeaveCriticalRegion
(
    VOID
)
{
    LOG_FUNC();

    PKTHREAD thread = KeGetCurrentThread();
    thread->KernelApcDisable++;
    if(thread->KernelApcDisable == 0) {
		LIST_ENTRY *apcListHead = &thread->ApcState.ApcListHead[0/*=KernelMode*/];
        if(apcListHead->Flink != apcListHead) {
            thread->ApcState.KernelApcPending = 1; // TRUE
            HalRequestSoftwareInterrupt(1); // APC_LEVEL
        }
    }
}

XBSYSAPI EXPORTNUM(123) xboxkrnl::LONG NTAPI xboxkrnl::KePulseEvent
(
	IN PRKEVENT Event,
	IN KPRIORITY Increment,
	IN BOOLEAN Wait
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Event)
		LOG_FUNC_ARG(Increment)
		LOG_FUNC_ARG(Wait)
		LOG_FUNC_END;

	// Fetch the host event and signal it, if present
	if (g_KeEventHandles.find(Event) == g_KeEventHandles.end()) {
		EmuWarning("KePulseEvent called on a non-existant event!");
	}
	else {
		PulseEvent(g_KeEventHandles[Event]);
	}

	LOG_UNIMPLEMENTED();

	RETURN(0);
}

XBSYSAPI EXPORTNUM(124) xboxkrnl::LONG NTAPI xboxkrnl::KeQueryBasePriorityThread
(
	IN PKTHREAD Thread
)
{
	LOG_FUNC_ONE_ARG(Thread);

	LOG_UNIMPLEMENTED();

	RETURN(0);
}

// ******************************************************************
// * 0x007D - KeQueryInterruptTime()
// ******************************************************************
XBSYSAPI EXPORTNUM(125) xboxkrnl::ULONGLONG NTAPI xboxkrnl::KeQueryInterruptTime(void)
{
	// TODO : Some software might call KeQueryInterruptTime often and fill the log quickly,
	// in which case we should not LOG_FUNC nor RETURN (use normal return instead).
	LOG_FUNC();
	
	ULONGLONG ret;

	LARGE_INTEGER InterruptTime;

	while (true)
	{
		// Don't use NtDll::QueryInterruptTime, it's too new (Windows 10)
		// Instead, read KeInterruptTime from our kernel thunk table,
		// which we coupled to the host InterruptTime in ConnectWindowsTimersToThunkTable:
		InterruptTime.u.HighPart = KeInterruptTime->High1Time;
		InterruptTime.u.LowPart = KeInterruptTime->LowPart;
		// TODO : Should we apply HostSystemTimeDelta to InterruptTime too?

		// Read InterruptTime atomically with a spinloop to avoid errors
		// when High1Time and High2Time differ (during unprocessed overflow in LowPart).
		if (InterruptTime.u.HighPart == KeInterruptTime->High2Time)
			break;
	}

	ret = LARGE_INTEGER2ULONGLONG(InterruptTime);
	RETURN(ret);
}

// ******************************************************************
// * 0x007E - KeQueryPerformanceCounter()
// ******************************************************************
XBSYSAPI EXPORTNUM(126) xboxkrnl::ULONGLONG NTAPI xboxkrnl::KeQueryPerformanceCounter(void)
{
	LOG_FUNC();

	ULONGLONG ret;
	::LARGE_INTEGER PerformanceCounter;
	//no matter rdtsc is patched or not, we should always return a scaled performance counter here.

	// Dxbx note : Xbox actually uses the RDTSC machine code instruction for this,
	// and we we're bound to a single core, so we could do that too, but on Windows
	// rdtsc is not a very stable counter, so instead, we'll use the native PeformanceCounter :
	QueryPerformanceCounter(&PerformanceCounter);

	// Re-base the performance counter to increase accuracy of the following conversion :
	PerformanceCounter.QuadPart -= NativePerformanceCounter.QuadPart;
	// We appy a conversion factor here, to fake Xbox1-like increment-speed behaviour :
	PerformanceCounter.QuadPart = (ULONGLONG)(NativeToXbox_FactorForPerformanceFrequency * PerformanceCounter.QuadPart);

	ret = PerformanceCounter.QuadPart;
	RETURN(ret);
}

// ******************************************************************
// * 0x007F - KeQueryPerformanceFrequency()
// ******************************************************************
XBSYSAPI EXPORTNUM(127) xboxkrnl::ULONGLONG NTAPI xboxkrnl::KeQueryPerformanceFrequency(void)
{
	LOG_FUNC();

	// Dxbx note : We return the real Xbox1 frequency here,
	// to make subsequent calculations behave the same as on the real Xbox1 :
	ULONGLONG ret = XBOX_PERFORMANCE_FREQUENCY;

	RETURN(ret);
}

// ******************************************************************
// * 0x0080 - KeQuerySystemTime()
// ******************************************************************
XBSYSAPI EXPORTNUM(128) xboxkrnl::VOID NTAPI xboxkrnl::KeQuerySystemTime
(
	PLARGE_INTEGER CurrentTime
)
{
	LOG_FUNC_ONE_ARG(CurrentTime);

	if (CurrentTime != NULL)
	{
		LARGE_INTEGER HostSystemTime;
		GetSystemTimeAsFileTime((LPFILETIME)&HostSystemTime); // Available since Windows 2000 (NOT on XP!)

		// Apply the delta set in xboxkrnl::NtSetSystemTime to get the Xbox system time :
		CurrentTime->QuadPart = HostSystemTime.QuadPart + HostSystemTimeDelta.QuadPart;
	}
}

// ******************************************************************
// * 0x0081 - KeRaiseIrqlToDpcLevel()
// ******************************************************************
XBSYSAPI EXPORTNUM(129) xboxkrnl::UCHAR NTAPI xboxkrnl::KeRaiseIrqlToDpcLevel()
{
	LOG_FORWARD(KfRaiseIrql);

	return KfRaiseIrql(DISPATCH_LEVEL);
}

// ******************************************************************
// * 0x0082 - KeRaiseIrqlToSynchLevel()
// ******************************************************************
XBSYSAPI EXPORTNUM(130) xboxkrnl::UCHAR NTAPI xboxkrnl::KeRaiseIrqlToSynchLevel()
{
	LOG_FORWARD(KfRaiseIrql);

	return KfRaiseIrql(SYNC_LEVEL);
}

XBSYSAPI EXPORTNUM(131) xboxkrnl::LONG NTAPI xboxkrnl::KeReleaseMutant
(
	IN PRKMUTANT Mutant,
	IN KPRIORITY Increment,
	IN BOOLEAN Abandoned,
	IN BOOLEAN Wait
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Mutant)
		LOG_FUNC_ARG(Increment)
		LOG_FUNC_ARG(Abandoned)
		LOG_FUNC_ARG(Wait)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();
	
	RETURN(0);
}

XBSYSAPI EXPORTNUM(132) xboxkrnl::LONG NTAPI xboxkrnl::KeReleaseSemaphore
(
	IN PRKSEMAPHORE Semaphore,
	IN KPRIORITY Increment,
	IN BOOLEAN Adjustment,
	IN BOOLEAN Wait
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Semaphore)
		LOG_FUNC_ARG(Increment)
		LOG_FUNC_ARG(Adjustment)
		LOG_FUNC_ARG(Wait)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(0);
}

XBSYSAPI EXPORTNUM(133) xboxkrnl::PKDEVICE_QUEUE_ENTRY NTAPI xboxkrnl::KeRemoveByKeyDeviceQueue
(
	IN PKDEVICE_QUEUE DeviceQueue,
	IN ULONG SortKey
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(DeviceQueue)
		LOG_FUNC_ARG(SortKey)
		LOG_FUNC_END;

	KDEVICE_QUEUE_ENTRY *pEntry;

	if (IsListEmpty(&DeviceQueue->DeviceListHead)) {
		DeviceQueue->Busy = FALSE;
		pEntry = NULL;
	}
	else {
		LIST_ENTRY *pListEntry = DeviceQueue->DeviceListHead.Flink;
		while (pListEntry != &DeviceQueue->DeviceListHead) {
			pEntry = CONTAINING_RECORD(pListEntry, KDEVICE_QUEUE_ENTRY, DeviceListEntry);
			if (SortKey <= pEntry->SortKey) {
				break;
			}
			pListEntry = pListEntry->Flink;
		}

		if (pListEntry != &DeviceQueue->DeviceListHead) {
			RemoveEntryList(&pEntry->DeviceListEntry);

		}
		else {
			pListEntry = RemoveHeadList(&DeviceQueue->DeviceListHead);
			pEntry = CONTAINING_RECORD(pListEntry, KDEVICE_QUEUE_ENTRY, DeviceListEntry);
		}

		pEntry->Inserted = FALSE;
	}

	RETURN(pEntry);
}

XBSYSAPI EXPORTNUM(134) xboxkrnl::PKDEVICE_QUEUE_ENTRY NTAPI xboxkrnl::KeRemoveDeviceQueue
(
	IN PKDEVICE_QUEUE DeviceQueue
)
{
	LOG_FUNC_ONE_ARG(DeviceQueue);

	KDEVICE_QUEUE_ENTRY *pEntry;

	// We should lock the device queue here

	if (IsListEmpty(&DeviceQueue->DeviceListHead)) {
		DeviceQueue->Busy = FALSE;
		pEntry = NULL;
	}
	else {
		LIST_ENTRY *pListEntry = RemoveHeadList(&DeviceQueue->DeviceListHead);
		pEntry = CONTAINING_RECORD(pListEntry, KDEVICE_QUEUE_ENTRY, DeviceListEntry);
		pEntry->Inserted = FALSE;
	}

	// We should unlock the device queue here

	RETURN(pEntry);
}

XBSYSAPI EXPORTNUM(135) xboxkrnl::BOOLEAN NTAPI xboxkrnl::KeRemoveEntryDeviceQueue
(
	IN PKDEVICE_QUEUE DeviceQueue,
	IN PKDEVICE_QUEUE_ENTRY DeviceQueueEntry
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(DeviceQueue)
		LOG_FUNC_ARG(DeviceQueueEntry)
		LOG_FUNC_END;


	KIRQL oldIRQL;
	KiLockDispatcherDatabase(&oldIRQL);
	// We should lock the device queue here

	BOOLEAN currentlyInserted = DeviceQueueEntry->Inserted;
	if (currentlyInserted) {
		DeviceQueueEntry->Inserted = FALSE;
		RemoveEntryList(&DeviceQueueEntry->DeviceListEntry);
	}

	KiUnlockDispatcherDatabase(oldIRQL);
	// We should unlock the device queue here

	RETURN(currentlyInserted);
}


XBSYSAPI EXPORTNUM(136) xboxkrnl::PLIST_ENTRY NTAPI xboxkrnl::KeRemoveQueue
(
	IN PRKQUEUE Queue,
	IN KPROCESSOR_MODE WaitMode,
	IN PLARGE_INTEGER Timeout
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Queue)
		LOG_FUNC_ARG(WaitMode)
		LOG_FUNC_ARG(Timeout)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(NULL);
}

// ******************************************************************
// * 0x0089 - KeRemoveQueueDpc()
// ******************************************************************
XBSYSAPI EXPORTNUM(137) xboxkrnl::BOOLEAN NTAPI xboxkrnl::KeRemoveQueueDpc
(
	IN PKDPC Dpc
)
{
	LOG_FUNC_ONE_ARG(Dpc);

	// TODO : Instead of using a lock, emulate the Clear Interrupt Flag (cli) instruction
	// See https://msdn.microsoft.com/is-is/library/y14401ab(v=vs.80).aspx
	EnterCriticalSection(&(g_DpcData.Lock));

	BOOLEAN Inserted = Dpc->Inserted;

	if (Inserted)
	{
		RemoveEntryList(&(Dpc->DpcListEntry));
		Dpc->Inserted = FALSE;
	}

	// TODO : Instead of using a lock, emulate the Set Interrupt Flag (sti) instruction
	// See https://msdn.microsoft.com/en-us/library/ad820yz3(v=vs.80).aspx
	LeaveCriticalSection(&(g_DpcData.Lock));

	RETURN(Inserted);
}

// ******************************************************************
// * 0x008A - KeResetEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(138) xboxkrnl::LONG NTAPI xboxkrnl::KeResetEvent
(
	IN PRKEVENT Event
)
{
	LOG_FUNC_ONE_ARG(Event);

	// TODO : Untested & incomplete
	LONG ret = Event->Header.SignalState;
	Event->Header.SignalState = 0;

	// Fetch the host event and signal it, if present
	if (g_KeEventHandles.find(Event) == g_KeEventHandles.end()) {
		EmuWarning("KeResetEvent called on a non-existant event!");
	}
	else {
		ResetEvent(g_KeEventHandles[Event]);
	}


	return ret;
}

// ******************************************************************
// * 0x008B - KeRestoreFloatingPointState()
// ******************************************************************
XBSYSAPI EXPORTNUM(139) xboxkrnl::NTSTATUS NTAPI xboxkrnl::KeRestoreFloatingPointState
(
	IN PKFLOATING_SAVE     PublicFloatSave
)
{
	LOG_FUNC_ONE_ARG(PublicFloatSave);

	NTSTATUS ret = STATUS_SUCCESS;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

// ******************************************************************
// * 0x008C - KeResumeThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(140) xboxkrnl::ULONG NTAPI xboxkrnl::KeResumeThread
(
	IN PKTHREAD Thread
)
{
	LOG_FUNC_ONE_ARG(Thread);

	NTSTATUS ret = STATUS_SUCCESS;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

XBSYSAPI EXPORTNUM(141) xboxkrnl::PLIST_ENTRY NTAPI xboxkrnl::KeRundownQueue
(
	IN PRKQUEUE Queue
)
{
	LOG_FUNC_ONE_ARG(Queue);

	LOG_UNIMPLEMENTED();

	RETURN(NULL);
}

// ******************************************************************
// * 0x008E - KeSaveFloatingPointState()
// ******************************************************************
XBSYSAPI EXPORTNUM(142) xboxkrnl::NTSTATUS NTAPI xboxkrnl::KeSaveFloatingPointState
(
	OUT PKFLOATING_SAVE     PublicFloatSave
)
{
	LOG_FUNC_ONE_ARG_OUT(PublicFloatSave);

	NTSTATUS ret = STATUS_SUCCESS;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

// ******************************************************************
// * 0x008F - KeSetBasePriorityThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(143) xboxkrnl::LONG NTAPI xboxkrnl::KeSetBasePriorityThread
(
	IN PKTHREAD  Thread,
	IN LONG  Priority
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(Thread)
		LOG_FUNC_ARG_OUT(Priority)
		LOG_FUNC_END;

	LONG ret = GetThreadPriority((HANDLE)Thread);

	// This would work normally, but it will slow down the emulation, 
	// don't do that if the priority is higher then normal (so our own)!
	if((Priority <= THREAD_PRIORITY_NORMAL) && ((HANDLE)Thread != GetCurrentThread())) {
		SetThreadPriority((HANDLE)Thread, Priority);
	}

	RETURN(ret);
}

XBSYSAPI EXPORTNUM(144) xboxkrnl::ULONG NTAPI xboxkrnl::KeSetDisableBoostThread
(
	IN PKTHREAD Thread,
	IN ULONG Disable
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Thread)
		LOG_FUNC_ARG(Disable)
		LOG_FUNC_END;

	KIRQL oldIRQL;
	KiLockDispatcherDatabase(&oldIRQL);

	ULONG prevDisableBoost = Thread->DisableBoost;
	Thread->DisableBoost = (CHAR)Disable;

	KiUnlockDispatcherDatabase(oldIRQL);

	RETURN(prevDisableBoost);
}

// ******************************************************************
// * 0x0091 - KeSetEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(145) xboxkrnl::LONG NTAPI xboxkrnl::KeSetEvent
(
	IN PRKEVENT		Event,
	IN KPRIORITY	Increment,
	IN BOOLEAN		Wait	
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Event)
		LOG_FUNC_ARG(Increment)
		LOG_FUNC_ARG(Wait)
		LOG_FUNC_END;

	// TODO : Untested & incomplete
	LONG ret = Event->Header.SignalState;
	Event->Header.SignalState = TRUE;

	// Fetch the host event and signal it, if present
	if (g_KeEventHandles.find(Event) == g_KeEventHandles.end()) {
		EmuWarning("KeSetEvent called on a non-existant event. Creating it!");
		// TODO: Find out why some XDKs do not call KeInitializeEvent first
		KeInitializeEvent(Event, NotificationEvent, TRUE);
	} else {
		SetEvent(g_KeEventHandles[Event]);
	}

	RETURN(ret);
}

XBSYSAPI EXPORTNUM(146) xboxkrnl::VOID NTAPI xboxkrnl::KeSetEventBoostPriority
(
	IN PRKEVENT Event,
	IN PRKTHREAD *Thread
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Event)
		LOG_FUNC_ARG(Thread)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();
}

XBSYSAPI EXPORTNUM(147) xboxkrnl::KPRIORITY NTAPI xboxkrnl::KeSetPriorityProcess
(
	IN PKPROCESS Process,
	IN KPRIORITY BasePriority
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Process)
		LOG_FUNC_ARG(BasePriority)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(BasePriority);
}


// ******************************************************************
// * 0x0094 - KeSetPriorityThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(148) xboxkrnl::BOOLEAN NTAPI xboxkrnl::KeSetPriorityThread
(
    IN PKTHREAD  Thread,
    IN LONG  Priority
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(Thread)
		LOG_FUNC_ARG_OUT(Priority)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(1);
}

// ******************************************************************
// * 0x0095 - KeSetTimer()
// ******************************************************************
XBSYSAPI EXPORTNUM(149) xboxkrnl::BOOLEAN NTAPI xboxkrnl::KeSetTimer
(
	IN PKTIMER        Timer,
	IN LARGE_INTEGER  DueTime,
	IN PKDPC          Dpc OPTIONAL
)
{
	LOG_FORWARD("KeSetTimerEx");

	// Call KeSetTimerEx with a period of zero
	return KeSetTimerEx(Timer, DueTime, 0, Dpc);
}

// ******************************************************************
// * 0x0096 - KeSetTimerEx()
// ******************************************************************
XBSYSAPI EXPORTNUM(150) xboxkrnl::BOOLEAN NTAPI xboxkrnl::KeSetTimerEx
(
	IN PKTIMER        Timer,
	IN LARGE_INTEGER  DueTime,
	IN LONG           Period OPTIONAL,
	IN PKDPC          Dpc OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Timer)
		LOG_FUNC_ARG(DueTime)
		LOG_FUNC_ARG(Period)
		LOG_FUNC_ARG(Dpc)
		LOG_FUNC_END;

	BOOLEAN Inserted;
	LARGE_INTEGER Interval;
	LARGE_INTEGER SystemTime;

	if (Timer->Header.Type != TimerNotificationObject && Timer->Header.Type != TimerSynchronizationObject) {
		CxbxKrnlCleanup("Assertion: '(Timer)->Header.Type == TimerNotificationObject) || ((Timer)->Header.Type == TimerSynchronizationObject)' in KeSetTimerEx()");
	}

	// Same as KeCancelTimer(Timer) :
	Inserted = Timer->Header.Inserted;
	if (Inserted != FALSE) {
		// Do some unlinking if already inserted in the linked list
		KiRemoveTreeTimer(Timer);
	}

	Timer->Header.SignalState = FALSE;
	Timer->Dpc = Dpc;
	Timer->Period = Period;

	if (!KiInsertTreeTimer(Timer, DueTime)) {
		if (!IsListEmpty(&(Timer->Header.WaitListHead))) {
			// KiWaitTest(Timer, 0);
		}

		if (Dpc != NULL) {
			// Call the Dpc routine if one is specified
			KeQuerySystemTime(&SystemTime);
			KeInsertQueueDpc(Timer->Dpc, (PVOID)SystemTime.u.LowPart, (PVOID)SystemTime.u.HighPart);
		}

		if (Period != 0) {
			// Prepare the repetition if Timer is periodic
			Interval.QuadPart = (LONGLONG)(-10 * 1000) * Timer->Period;
			while (!KiInsertTreeTimer(Timer, Interval))
				;
		}
	}
/* Dxbx has this :
	EnterCriticalSection(&(g_DpcData.Lock));
	if (Timer->TimerListEntry.Flink == nullptr) 
	{
		Timer->DueTime.QuadPart := (DueTime.QuadPart / -10000) + CxbxXboxGetTickCount();
		Timer->Period = Period;
		Timer->Dpc = Dpc;
		InsertTailList(&(g_DpcData.TimerQueue), &(Timer->TimerListEntry));
	}

	LeaveCriticalSection(&(g_DpcData.Lock));
	SetEvent(g_DpcData.DpcEvent);
*/

	RETURN(Inserted);
}

// ******************************************************************
// * 0x0097 - KeStallExecutionProcessor()
// ******************************************************************
XBSYSAPI EXPORTNUM(151) xboxkrnl::VOID NTAPI xboxkrnl::KeStallExecutionProcessor
(
	IN ULONG MicroSeconds
)
{
	LOG_FUNC_ONE_ARG(MicroSeconds);

	// WinAPI Sleep usually sleeps for a minimum of 15ms, we want us. 
	// Thanks to C++11, we can do this in a nice way without resorting to
	// QueryPerformanceCounter
	std::this_thread::sleep_for(std::chrono::microseconds(MicroSeconds));
}

// ******************************************************************
// * 0x0098 - KeSuspendThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(152) xboxkrnl::ULONG NTAPI xboxkrnl::KeSuspendThread
(
	IN PKTHREAD Thread
)
{
	LOG_FUNC_ONE_ARG(Thread);

	NTSTATUS ret = STATUS_SUCCESS;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

// ******************************************************************
// * 0x0099 - KeSynchronizeExecution()
// ******************************************************************
XBSYSAPI EXPORTNUM(153) xboxkrnl::BOOLEAN NTAPI xboxkrnl::KeSynchronizeExecution
(
	IN PKINTERRUPT Interrupt,
	IN PKSYNCHRONIZE_ROUTINE SynchronizeRoutine,
	IN PVOID SynchronizeContext
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Interrupt)
		LOG_FUNC_ARG(SynchronizeRoutine)
		LOG_FUNC_ARG(SynchronizeContext)
		LOG_FUNC_END;

	BOOLEAN ret = TRUE;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

// ******************************************************************
// * 0x009A - KeSystemTime
// ******************************************************************
// Dxbx note : This was once a value, but instead we now point to
// the native Windows versions (see ConnectWindowsTimersToThunkTable) :
// XBSYSAPI EXPORTNUM(154) xboxkrnl::PKSYSTEM_TIME xboxkrnl::KeSystemTime; // Used for KernelThunk[154]

// ******************************************************************
// * 0x009B - KeTestAlertThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(155) xboxkrnl::BOOLEAN NTAPI xboxkrnl::KeTestAlertThread
(
	IN KPROCESSOR_MODE AlertMode
)
{
	LOG_FUNC_ONE_ARG(AlertMode);

	BOOLEAN ret = TRUE;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

// ******************************************************************
// * 0x009C - KeTickCount
// ******************************************************************
XBSYSAPI EXPORTNUM(156) xboxkrnl::DWORD VOLATILE xboxkrnl::KeTickCount = 0;

const xboxkrnl::ULONG CLOCK_TIME_INCREMENT = 0x2710;

// ******************************************************************
// * 0x009D - KeTimeIncrement
// ******************************************************************
XBSYSAPI EXPORTNUM(157) xboxkrnl::ULONG xboxkrnl::KeTimeIncrement = CLOCK_TIME_INCREMENT;

// ******************************************************************
// * 0x009E - KeWaitForMultipleObjects()
// ******************************************************************
XBSYSAPI EXPORTNUM(158) xboxkrnl::NTSTATUS NTAPI xboxkrnl::KeWaitForMultipleObjects
(
	IN ULONG Count,
	IN PVOID Object[],
	IN WAIT_TYPE WaitType,
	IN KWAIT_REASON WaitReason,
	IN KPROCESSOR_MODE WaitMode,
	IN BOOLEAN Alertable,
	IN PLARGE_INTEGER Timeout OPTIONAL,
	IN PKWAIT_BLOCK WaitBlockArray OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Count)
		LOG_FUNC_ARG(Object)
		LOG_FUNC_ARG(WaitType)
		LOG_FUNC_ARG(WaitReason)
		LOG_FUNC_ARG(WaitMode)
		LOG_FUNC_ARG(Alertable)
		LOG_FUNC_ARG(Timeout)
		LOG_FUNC_ARG(WaitBlockArray)
		LOG_FUNC_END;

	NTSTATUS ret = STATUS_SUCCESS;

	// Take the input and build two arrays: One of handles created by our kernel and one for not
	// Handles created by our kernel need to be forwarded to WaitForMultipleObjects while handles
	// created by Windows need to be forwarded to NtDll::KeWaitForMultipleObjects
	std::vector<HANDLE> nativeObjects;
	std::vector<HANDLE> ntdllObjects;

	for (uint i = 0; i < Count; i++) {
		DbgPrintf("Object: 0x%08X\n", Object[i]);
		if (g_KeEventHandles.find((PKEVENT)Object[i]) == g_KeEventHandles.end()) {
			ntdllObjects.push_back(Object[i]);
		} else {
			nativeObjects.push_back(g_KeEventHandles[(PRKEVENT)Object[i]]);
		}
	}

	if (ntdllObjects.size() > 0) {
		// TODO : What should we do with the (currently ignored)
		//        WaitReason, WaitMode, WaitBlockArray?

		// Unused arguments : WaitReason, WaitMode, WaitBlockArray
		ret = NtDll::NtWaitForMultipleObjects(
			ntdllObjects.size(),
			&ntdllObjects[0],
			(NtDll::OBJECT_WAIT_TYPE)WaitType,
			Alertable,
			(NtDll::PLARGE_INTEGER)Timeout);

		if (FAILED(ret))
			EmuWarning("KeWaitForMultipleObjects failed! (%s)", NtStatusToString(ret));
	}
	
	if (nativeObjects.size() > 0) {
		ret = NtDll::NtWaitForMultipleObjects(
			nativeObjects.size(),
			&nativeObjects[0],
			(NtDll::OBJECT_WAIT_TYPE)WaitType,
			Alertable,
			(NtDll::PLARGE_INTEGER)Timeout);

		if (FAILED(ret))
			EmuWarning("KeWaitForMultipleObjects failed! (%s)", NtStatusToString(ret));
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x009F - KeWaitForSingleObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(159) xboxkrnl::NTSTATUS NTAPI xboxkrnl::KeWaitForSingleObject
(
	IN PVOID Object,
	IN KWAIT_REASON WaitReason,
	IN KPROCESSOR_MODE WaitMode,
	IN BOOLEAN Alertable,
	IN PLARGE_INTEGER Timeout OPTIONAL
)
{
	LOG_FORWARD("KeWaitForMultipleObjects");

	return xboxkrnl::KeWaitForMultipleObjects(
		/*Count=*/1,
		&Object,
		/*WaitType=*/WaitAll,
		WaitReason,
		WaitMode,
		Alertable,
		Timeout,
		/*WaitBlockArray*/NULL
	);
}
