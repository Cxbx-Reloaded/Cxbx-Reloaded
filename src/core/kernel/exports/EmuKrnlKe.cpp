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

// Acknowledgment (timer functions): ReactOS (GPLv2)
// https://github.com/reactos/reactos

// KeSetSystemTime
/*
* PROJECT:         ReactOS Kernel
* LICENSE:         GPL - See COPYING in the top level directory
* FILE:            ntoskrnl/ke/clock.c
* PURPOSE:         System Clock Support
* PROGRAMMERS:     Alex Ionescu (alex.ionescu@reactos.org)
*/

// KeInitializeTimerEx, KeSetTimer, KeSetTimerEx, KeCancelTimer
/*
* PROJECT:         ReactOS Kernel
* LICENSE:         GPL - See COPYING in the top level directory
* FILE:            ntoskrnl/ke/timerobj.c
* PURPOSE:         Handle Kernel Timers (Kernel-part of Executive Timers)
* PROGRAMMERS:     Alex Ionescu (alex.ionescu@reactos.org)
*/

// COPYING file:
/*
GNU GENERAL PUBLIC LICENSE
Version 2, June 1991

ReactOS may be used, runtime linked, and distributed with non-free software
(meaning that such software has no obligations to open-source, or render
free, their non-free code) such as commercial device drivers and commercial
applications. This exception does not alter any other responsibilities of
the licensee under the GPL (meaning that such software must still obey
the GPL for the free ("open-sourced") code that has been integrated into
the said software).
*/


#define LOG_PREFIX CXBXR_MODULE::KE

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
	#include "core\kernel\support\EmuNtDll.h" // For NtDelayExecution(), etc.
};

#include "core\kernel\init\CxbxKrnl.h" // For CxbxKrnlCleanup
#include "core\kernel\support\Emu.h" // For EmuLog(LOG_LEVEL::WARNING, )
#include "EmuKrnl.h" // For InitializeListHead(), etc.
#include "EmuKrnlKi.h" // For KiRemoveTreeTimer(), KiInsertTreeTimer()
#include "EmuKrnlKe.h"
#include "core\kernel\support\EmuFile.h" // For IsEmuHandle(), NtStatusToString()
#include "Timer.h"

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
} DpcData;

DpcData g_DpcData = { 0 }; // Note : g_DpcData is initialized in InitDpcThread()

xboxkrnl::ULONGLONG LARGE_INTEGER2ULONGLONG(xboxkrnl::LARGE_INTEGER value)
{
	// Weird construction because there doesn't seem to exist an implicit
	// conversion of LARGE_INTEGER to ULONGLONG :
	return *((PULONGLONG)&value);
}

#define TestForAlertPending(Alertable) \
    if (Alertable) { \
        if (Thread->Alerted[WaitMode] != FALSE) { \
            Thread->Alerted[WaitMode] = FALSE; \
            WaitStatus = STATUS_ALERTED; \
            break; \
        } else if ((WaitMode != KernelMode) && \
                  (IsListEmpty(&Thread->ApcState.ApcListHead[UserMode])) == FALSE) { \
            Thread->ApcState.UserApcPending = TRUE; \
            WaitStatus = STATUS_USER_APC; \
            break; \
        } else if (Thread->Alerted[KernelMode] != FALSE) { \
            Thread->Alerted[KernelMode] = FALSE; \
            WaitStatus = STATUS_ALERTED; \
            break; \
        } \
    } else if ((WaitMode != KernelMode) && (Thread->ApcState.UserApcPending)) { \
        WaitStatus = STATUS_USER_APC; \
        break; \
    }


// ******************************************************************
// * KeGetPcr()
// * NOTE: This is a macro on the Xbox, however we implement it 
// * as a function so it can suit our emulated KPCR structure
// ******************************************************************
xboxkrnl::KPCR* WINAPI KeGetPcr()
{
	xboxkrnl::PKPCR Pcr;

	// See EmuKeSetPcr()
	Pcr = (xboxkrnl::PKPCR)__readfsdword(TIB_ArbitraryDataSlot);
	
	if (Pcr == nullptr) {
		EmuLog(LOG_LEVEL::WARNING, "KeGetPCR returned nullptr: Was this called from a non-xbox thread?");
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

// ******************************************************************
// * KeSetSystemTime()
// ******************************************************************
xboxkrnl::VOID NTAPI xboxkrnl::KeSetSystemTime
(
	IN  xboxkrnl::PLARGE_INTEGER NewTime,
	OUT xboxkrnl::PLARGE_INTEGER OldTime
)
{
	KIRQL OldIrql, OldIrql2;
	LARGE_INTEGER DeltaTime, HostTime;
	PLIST_ENTRY ListHead, NextEntry;
	PKTIMER Timer;
	LIST_ENTRY TempList, TempList2;
	ULONG Hand, i;

	/* Sanity checks */
	assert((NewTime->u.HighPart & 0xF0000000) == 0);
	assert(KeGetCurrentIrql() <= DISPATCH_LEVEL);

	/* Lock the dispatcher, and raise IRQL */
	KiTimerLock();
	KiLockDispatcherDatabase(&OldIrql);
	OldIrql2 = KfRaiseIrql(HIGH_LEVEL);

	/* Query the system time now */
	KeQuerySystemTime(OldTime);

	/* Surely, we won't set the system time here, but we CAN remember a delta to the host system time */
	HostTime.QuadPart = OldTime->QuadPart - HostSystemTimeDelta.load();
	HostSystemTimeDelta = NewTime->QuadPart - HostTime.QuadPart;

	/* Calculate the difference between the new and the old time */
	DeltaTime.QuadPart = NewTime->QuadPart - OldTime->QuadPart;

	/* Lower IRQL back */
	KfLowerIrql(OldIrql2);

	/* Setup a temporary list of absolute timers */
	InitializeListHead(&TempList);

	/* Loop current timers */
	for (i = 0; i < TIMER_TABLE_SIZE; i++)
	{
		/* Loop the entries in this table and lock the timers */
		ListHead = &KiTimerTableListHead[i].Entry;
		NextEntry = ListHead->Flink;
		while (NextEntry != ListHead)
		{
			/* Get the timer */
			Timer = CONTAINING_RECORD(NextEntry, KTIMER, TimerListEntry);
			NextEntry = NextEntry->Flink;

			/* Is it absolute? */
			if (Timer->Header.Absolute)
			{
				/* Remove it from the timer list */
				KiRemoveEntryTimer(Timer, i);

				/* Insert it into our temporary list */
				InsertTailList(&TempList, &Timer->TimerListEntry);
			}
		}
	}

	/* Setup a temporary list of expired timers */
	InitializeListHead(&TempList2);

	/* Loop absolute timers */
	while (TempList.Flink != &TempList)
	{
		/* Get the timer */
		Timer = CONTAINING_RECORD(TempList.Flink, KTIMER, TimerListEntry);
		RemoveEntryList(&Timer->TimerListEntry);

		/* Update the due time and handle */
		Timer->DueTime.QuadPart -= DeltaTime.QuadPart;
		Hand = KiComputeTimerTableIndex(Timer->DueTime.QuadPart);

		/* Lock the timer and re-insert it */
		if (KiInsertTimerTable(Timer, Hand))
		{
			/* Remove it from the timer list */
			KiRemoveEntryTimer(Timer, Hand);

			/* Insert it into our temporary list */
			InsertTailList(&TempList2, &Timer->TimerListEntry);
		}
	}

	/* Process expired timers. This releases the dispatcher and timer locks */
	KiTimerListExpire(&TempList2, OldIrql);
}

// ******************************************************************
// * KeInitializeTimer()
// ******************************************************************
xboxkrnl::VOID NTAPI xboxkrnl::KeInitializeTimer
(
	IN PKTIMER Timer
)
{
	LOG_FORWARD("KeInitializeTimerEx");

	KeInitializeTimerEx(Timer, NotificationTimer);
}

// Forward KeLowerIrql() to KfLowerIrql()
#define KeLowerIrql(NewIrql) \
	KfLowerIrql(NewIrql)

// Forward KeRaiseIrql() to KfRaiseIrql()
#define KeRaiseIrql(NewIrql, OldIrql) \
	*(OldIrql) = KfRaiseIrql(NewIrql)

void ExecuteDpcQueue()
{
	xboxkrnl::PKDPC pkdpc;

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
		EmuLog(LOG_LEVEL::DEBUG, "Global DpcQueue, calling DPC at 0x%.8X", pkdpc->DeferredRoutine);
		__try {
			// Call the Deferred Procedure  :
			pkdpc->DeferredRoutine(
				pkdpc,
				pkdpc->DeferredContext,
				pkdpc->SystemArgument1,
				pkdpc->SystemArgument2);
		} __except (EmuException(GetExceptionInformation()))
		{
			EmuLog(LOG_LEVEL::WARNING, "Problem with ExceptionFilter!");
		}

		KeGetCurrentPrcb()->DpcRoutineActive = FALSE; // Experimental
	}

//    Assert(g_DpcData._dwThreadId == GetCurrentThreadId());
//    Assert(g_DpcData._dwDpcThreadId == g_DpcData._dwThreadId);
//    g_DpcData._dwDpcThreadId = 0;
	LeaveCriticalSection(&(g_DpcData.Lock));
}

void InitDpcThread()
{
	DWORD dwThreadId = 0;

	InitializeCriticalSection(&(g_DpcData.Lock));
	InitializeListHead(&(g_DpcData.DpcQueue));
	EmuLogEx(CXBXR_MODULE::INIT, LOG_LEVEL::DEBUG, "Creating DPC event\n");
	g_DpcData.DpcEvent = CreateEvent(/*lpEventAttributes=*/nullptr, /*bManualReset=*/FALSE, /*bInitialState=*/FALSE, /*lpName=*/nullptr);
}

#define XBOX_TSC_FREQUENCY 733333333 // Xbox Time Stamp Counter Frequency = 733333333 (CPU Clock)
#define XBOX_ACPI_FREQUENCY 3375000  // Xbox ACPI frequency (3.375 mhz)
ULONGLONG NativeToXbox_FactorForRdtsc;
ULONGLONG NativeToXbox_FactorForAcpi;

ULONGLONG CxbxGetPerformanceCounter(bool acpi) {
	LARGE_INTEGER tsc;
	ULARGE_INTEGER scaledTsc;

	QueryPerformanceCounter(&tsc);

	scaledTsc.QuadPart = 1000000000;
	scaledTsc.QuadPart *= (ULONGLONG)tsc.QuadPart;

	if (acpi == false && NativeToXbox_FactorForRdtsc) {
		scaledTsc.QuadPart /= NativeToXbox_FactorForRdtsc;
		return scaledTsc.QuadPart;
	}	else if (acpi == true && NativeToXbox_FactorForRdtsc) {
		scaledTsc.QuadPart /= NativeToXbox_FactorForAcpi;
		return scaledTsc.QuadPart;
	}

	return (uint64_t)tsc.QuadPart;
}

void CxbxInitPerformanceCounters()
{
	uint64_t t;
	t = 1000000000;
	t *= HostClockFrequency;
	t /= XBOX_TSC_FREQUENCY;
	NativeToXbox_FactorForRdtsc = t;

	t = 1000000000;
	t *= HostClockFrequency;
	t /= XBOX_ACPI_FREQUENCY;
	NativeToXbox_FactorForAcpi = t;

	// Let's initialize the Dpc handling thread too,
	// here for now (should be called by our caller)
	InitDpcThread();
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

	KIRQL OldIrql;
	BOOLEAN Inserted;

	assert(Timer);

	/* Lock the Database and Raise IRQL */
	KiTimerLock();
	KiLockDispatcherDatabase(&OldIrql);

	/* Check if it's inserted, and remove it if it is */
	Inserted = Timer->Header.Inserted;
	if (Inserted) {
		KxRemoveTreeTimer(Timer);
	}

	/* Release Dispatcher Lock */
	KiUnlockDispatcherDatabase(OldIrql);
	KiTimerUnlock();

	/* Return the old state */
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

	// HACK: Since we forward to NtDll::NtCreateEvent, this *might* be a Windows handle instead of our own
	// In this case, it is already initialized so no need tod o anything
	// Test Case: Xbox Live Dashboard, Network Test (or any other Xbox Live connection)
	DWORD flags = 0;
	if (GetHandleInformation((HANDLE)Event, &flags)) {
        return;
	}

	// Setup the Xbox event struct
	Event->Header.Type = Type;
	Event->Header.Size = sizeof(KEVENT) / sizeof(LONG);
	Event->Header.SignalState = SignalState;
	InitializeListHead(&(Event->Header.WaitListHead)); 
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

	assert(Timer);

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
XBSYSAPI EXPORTNUM(120) xboxkrnl::KSYSTEM_TIME xboxkrnl::KeInterruptTime = { 0, 0, 0 };

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

	KIRQL OldIrql;
	KiLockDispatcherDatabase(&OldIrql);

	// HACK: Since we forward to NtDll::NtCreateEvent, this *might* be a Windows handle instead of our own
	// In this case, we must call the NtDll function
	// Test Case: Xbox Live Dashboard, Network Test (or any other Xbox Live connection)
	DWORD flags = 0;
	if (GetHandleInformation((HANDLE)Event, &flags)) {
		KiUnlockDispatcherDatabase(OldIrql);
		return NtDll::NtPulseEvent((HANDLE)Event, nullptr);
	}

	LONG OldState = Event->Header.SignalState;
	if ((OldState == 0) && (IsListEmpty(&Event->Header.WaitListHead) == FALSE)) {
		Event->Header.SignalState = 1;
		// TODO: KiWaitTest(Event, Increment);
		// For now, we just sleep to give other threads time to wake
		// KiWaitTest and related functions require correct thread scheduling to implement first
		// This will have to wait until CPU emulation at v1.0
		Sleep(1);
	}

	Event->Header.SignalState = 0;

	if (Wait != FALSE) {
		PRKTHREAD Thread = KeGetCurrentThread();
		Thread->WaitIrql = OldIrql;
		Thread->WaitNext = Wait;
	} else {
		KiUnlockDispatcherDatabase(OldIrql);
	}

	RETURN(OldState);
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
		// Don't use NtDll::QueryInterruptTime, it's too new (Windows 10).
		// Instead, read KeInterruptTime from our kernel thunk table.
		InterruptTime.u.HighPart = KeInterruptTime.High1Time;
		InterruptTime.u.LowPart = KeInterruptTime.LowPart;

		// Read InterruptTime atomically with a spinloop to avoid errors
		// when High1Time and High2Time differ (during unprocessed overflow in LowPart).
		if (InterruptTime.u.HighPart == KeInterruptTime.High2Time)
			break;
	}

	ret = LARGE_INTEGER2ULONGLONG(InterruptTime);
	RETURN(ret);
}

// ******************************************************************
// * 0x007E - KeQueryPerformanceCounter()
//   NOTE: The KeQueryPerformance* functions run at the ACPI clock
//	       The XAPI QueryPerformance* functions run at the TSC clock
// ******************************************************************
XBSYSAPI EXPORTNUM(126) xboxkrnl::ULONGLONG NTAPI xboxkrnl::KeQueryPerformanceCounter(void)
{
	LOG_FUNC();
	ULONGLONG ret;
	ret = CxbxGetPerformanceCounter(/*acpi=*/true);
	RETURN(ret);
}

// ******************************************************************
// * 0x007F - KeQueryPerformanceFrequency()
// ******************************************************************
XBSYSAPI EXPORTNUM(127) xboxkrnl::ULONGLONG NTAPI xboxkrnl::KeQueryPerformanceFrequency(void)
{
	LOG_FUNC();
	ULONGLONG ret = XBOX_ACPI_FREQUENCY;
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

	LARGE_INTEGER SystemTime;

	while (true)
	{
		SystemTime.u.HighPart = KeSystemTime.High1Time;
		SystemTime.u.LowPart = KeSystemTime.LowPart;

		// Read SystemTime atomically with a spinloop to avoid errors
		// when High1Time and High2Time differ (during unprocessed overflow in LowPart).
		if (SystemTime.u.HighPart == KeSystemTime.High2Time)
			break;
	}

	*CurrentTime = SystemTime;
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
	IN LONG Adjustment,
	IN BOOLEAN Wait
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Semaphore)
		LOG_FUNC_ARG(Increment)
		LOG_FUNC_ARG(Adjustment)
		LOG_FUNC_ARG(Wait)
		LOG_FUNC_END;

	UCHAR orig_irql = KeRaiseIrqlToDpcLevel();
	LONG initial_state = Semaphore->Header.SignalState;
	LONG adjusted_signalstate = Semaphore->Header.SignalState + Adjustment;

	BOOL limit_reached = adjusted_signalstate > Semaphore->Limit;
	BOOL signalstate_overflow = adjusted_signalstate < initial_state;
	if (limit_reached || signalstate_overflow) {
		KiUnlockDispatcherDatabase(orig_irql);
		ExRaiseStatus(STATUS_SEMAPHORE_LIMIT_EXCEEDED);
	}
	Semaphore->Header.SignalState = adjusted_signalstate;

	//TODO: Implement KiWaitTest
#if 0
	if ((initial_state == 0) && (IsListEmpty(&Semaphore->Header.WaitListHead) == FALSE)) {
		KiWaitTest(&Semaphore->Header, Increment);
	}
#endif

	if (Wait) {
		PKTHREAD current_thread = KeGetCurrentThread();
		current_thread->WaitNext = TRUE;
		current_thread->WaitIrql = orig_irql;
	}
	else {
		KiUnlockDispatcherDatabase(orig_irql);
	}

	RETURN(initial_state);
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

	KIRQL OldIrql;
	KiLockDispatcherDatabase(&OldIrql);

	// HACK: Since we forward to NtDll::NtCreateEvent, this *might* be a Windows handle instead of our own
	// In this case, we must call the NtDll function
	// Test Case: Xbox Live Dashboard, Network Test (or any other Xbox Live connection)
	DWORD flags = 0;
	if (GetHandleInformation((HANDLE)Event, &flags)) {
		KiUnlockDispatcherDatabase(OldIrql);
		return NtDll::NtResetEvent((HANDLE)Event, nullptr);
	}

	LONG OldState = Event->Header.SignalState;
	Event->Header.SignalState = 0;

	KiUnlockDispatcherDatabase(OldIrql);
	return OldState;	
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

	KIRQL OldIrql;
	KiLockDispatcherDatabase(&OldIrql);

	// HACK: Since we forward to NtDll::NtCreateEvent, this *might* be a Windows handle instead of our own
	// In this case, we must call the NtDll function
	// Test Case: Xbox Live Dashboard, Network Test (or any other Xbox Live connection)
	DWORD flags = 0;
	if (GetHandleInformation((HANDLE)Event, &flags)) {
		KiUnlockDispatcherDatabase(OldIrql);
		return NtDll::NtSetEvent((HANDLE)Event, nullptr);
	}

	LONG OldState = Event->Header.SignalState;
	if (IsListEmpty(&Event->Header.WaitListHead) != FALSE) {
		Event->Header.SignalState = 1;
	} else {
		PKWAIT_BLOCK WaitBlock = CONTAINING_RECORD(Event->Header.WaitListHead.Flink, KWAIT_BLOCK, WaitListEntry);
		if ((Event->Header.Type == NotificationEvent) ||
			(WaitBlock->WaitType != WaitAny)) {
			if (OldState == 0) {
				Event->Header.SignalState = 1;
				// TODO: KiWaitTest(Event, Increment);
				// For now, we just sleep to give other threads time to wake
				// See KePulseEvent
				Sleep(1);
			}
		} else {
			// TODO: KiUnwaitThread(WaitBlock->Thread, (NTSTATUS)WaitBlock->WaitKey, Increment);
			// For now, we just sleep to give other threads time to wake
			// See KePulseEvent
			Sleep(1);
		}
	}

	if (Wait != FALSE) {
		PRKTHREAD Thread = KeGetCurrentThread();
		Thread->WaitNext = Wait;
		Thread->WaitIrql = OldIrql;
	} else {
		KiUnlockDispatcherDatabase(OldIrql);
	}

	RETURN(OldState);
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
	KIRQL OldIrql;
	KiLockDispatcherDatabase(&OldIrql);

	// HACK: Since we forward to NtDll::NtCreateEvent, this *might* be a Windows handle instead of our own
	// In this case, we must do nothing. Anything else *will* cause crashes
	// Test Case: Xbox Live Dashboard, Network Test (or any other Xbox Live connection)
	DWORD flags = 0;
	if (GetHandleInformation((HANDLE)Event, &flags)) {
		KiUnlockDispatcherDatabase(OldIrql);
		return;
	}

	if (IsListEmpty(&Event->Header.WaitListHead) != FALSE) {
		Event->Header.SignalState = 1;
	} else {
		PRKTHREAD WaitThread = CONTAINING_RECORD(Event->Header.WaitListHead.Flink, KWAIT_BLOCK, WaitListEntry)->Thread;

		if (Thread != nullptr) {
			*Thread = WaitThread;
		}

		WaitThread->Quantum = WaitThread->ApcState.Process->ThreadQuantum;
		// TODO: KiUnwaitThread(WaitThread, STATUS_SUCCESS, 1);
		// For now, we just sleep to give other threads time to wake
		// See KePulseEvent
		Sleep(1);
	}

	KiUnlockDispatcherDatabase(OldIrql);
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
	BOOLEAN RequestInterrupt = FALSE;
	KIRQL OldIrql;
	ULONG Hand;

	assert(Timer);
	assert(Timer->Header.Type == TimerNotificationObject || Timer->Header.Type == TimerSynchronizationObject);

	KiTimerLock();
	KiLockDispatcherDatabase(&OldIrql);

	// Same as KeCancelTimer(Timer) :
	Inserted = Timer->Header.Inserted;
	if (Inserted != FALSE) {
		// Do some unlinking if already inserted in the linked list
		KxRemoveTreeTimer(Timer);
	}
	
	/* Set Default Timer Data */
	Timer->Dpc = Dpc;
	Timer->Period = Period;
	if (!KiComputeDueTime(Timer, DueTime, &Hand))
	{
		/* Signal the timer */
		RequestInterrupt = KiSignalTimer(Timer);

		/* Check if we need to do an interrupt */
		if (RequestInterrupt) {
			HalRequestSoftwareInterrupt(DISPATCH_LEVEL);
		}
	}
	else
	{
		/* Insert the timer */
		Timer->Header.SignalState = FALSE;
		KxInsertTimer(Timer, Hand);
	}

	/* Exit the dispatcher */
	KiUnlockDispatcherDatabase(OldIrql);
	KiTimerUnlock();

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
XBSYSAPI EXPORTNUM(154) xboxkrnl::KSYSTEM_TIME xboxkrnl::KeSystemTime = { 0, 0, 0 };

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

// ******************************************************************
// * 0x009D - KeTimeIncrement
// ******************************************************************
XBSYSAPI EXPORTNUM(157) xboxkrnl::ULONG xboxkrnl::KeTimeIncrement = CLOCK_TIME_INCREMENT;


xboxkrnl::PLARGE_INTEGER FASTCALL KiComputeWaitInterval(
	IN xboxkrnl::PLARGE_INTEGER OriginalTime,
	IN xboxkrnl::PLARGE_INTEGER DueTime,
	IN OUT xboxkrnl::PLARGE_INTEGER NewTime
)
{
	if (OriginalTime->QuadPart >= 0) {
		return OriginalTime;

	}
	else {
		NewTime->QuadPart = xboxkrnl::KeQueryInterruptTime();
		NewTime->QuadPart -= DueTime->QuadPart;
		return NewTime;
	}
}

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

	// If the lock is not already held, lock it
	PRKTHREAD Thread = KeGetCurrentThread();
	if (Thread->WaitNext) {
		Thread->WaitNext = FALSE;
	}
	else {
		KiLockDispatcherDatabase(&Thread->WaitIrql);
	}

	// Wait Loop
	// This loop ends 
	PLARGE_INTEGER OriginalTime = Timeout;
	LARGE_INTEGER DueTime, NewTime;
	KWAIT_BLOCK StackWaitBlock;
	PKWAIT_BLOCK WaitBlock = &StackWaitBlock;
	BOOLEAN WaitSatisfied;
	NTSTATUS WaitStatus;
	PKMUTANT ObjectMutant;
	// Hack variable (remove this when the thread scheduler is here)
	bool timeout_set = false;
	do {
		// Check if we need to let an APC run. This should immediately trigger APC interrupt via a call to UnlockDispatcherDatabase
		if (Thread->ApcState.KernelApcPending && (Thread->WaitIrql < APC_LEVEL)) {
			KiUnlockDispatcherDatabase(Thread->WaitIrql);
		}
		else {
			WaitSatisfied = TRUE;
			Thread->WaitStatus = STATUS_SUCCESS;

			for (ULONG Index = 0; Index < Count; Index += 1) {
				ObjectMutant = (PKMUTANT)Object[Index];
				if (WaitType == WaitAny) {
					if (ObjectMutant->Header.Type == MutantObject) {
						// If the object is a mutant object and it has been acquired MINGLONG times, raise an exception
						// If the mutant is signalled, we can satisfy the wait
						if ((ObjectMutant->Header.SignalState > 0) || (Thread == ObjectMutant->OwnerThread)) {
							if (ObjectMutant->Header.SignalState != MINLONG) {
								KiWaitSatisfyMutant(ObjectMutant, Thread);
								WaitStatus = (NTSTATUS)(Thread->WaitStatus);
								goto NoWait;
							}
							else {
								KiUnlockDispatcherDatabase(Thread->WaitIrql);
								ExRaiseStatus(STATUS_MUTANT_LIMIT_EXCEEDED);
							}
						}
					}
					else if (ObjectMutant->Header.SignalState) {
						// Otherwise, if the signal state is > 0, we can still just satisfy the wait
						KiWaitSatisfyOther(ObjectMutant);
						WaitStatus = STATUS_SUCCESS;
						goto NoWait;
					}
				} else {
					if (ObjectMutant->Header.Type == MutantObject) {
						if ((Thread == ObjectMutant->OwnerThread) && (ObjectMutant->Header.SignalState == MINLONG)) {
							KiUnlockDispatcherDatabase(Thread->WaitIrql);
							ExRaiseStatus(STATUS_MUTANT_LIMIT_EXCEEDED);
						} else if ((ObjectMutant->Header.SignalState <= 0) && (Thread != ObjectMutant->OwnerThread)) {
							WaitSatisfied = FALSE;
						}
					} else if (ObjectMutant->Header.SignalState <= 0) {
						WaitSatisfied = FALSE;
					}
				}


				// If we reached here, the wait could not be satisfied immediately, so we must setup a WaitBlock
				WaitBlock = &WaitBlockArray[Index];
				WaitBlock->Object = ObjectMutant;
				WaitBlock->WaitKey = (CSHORT)(Index);
				WaitBlock->WaitType = WaitType;
				WaitBlock->Thread = Thread;
				WaitBlock->NextWaitBlock = &WaitBlockArray[Index + 1];
			}

			// Check if the wait can be satisfied immediately
			if ((WaitType == WaitAll) && (WaitSatisfied)) {
				WaitBlock->NextWaitBlock = &WaitBlockArray[0];
				KiWaitSatisfyAll(WaitBlock);
				WaitStatus = (NTSTATUS)Thread->WaitStatus;
				goto NoWait;
			}	

			TestForAlertPending(Alertable);

			// Handle a Timeout if specified
			if (Timeout != nullptr) {
				// If the timeout is 0, do not wait
				if (!(Timeout->u.LowPart | Timeout->u.HighPart)) {
					WaitStatus = (NTSTATUS)(STATUS_TIMEOUT);
					goto NoWait;
				}

				// Setup a timer for the thread but only once (for now)
				if (!timeout_set) {
					KiTimerLock();
					PKTIMER Timer = &Thread->Timer;
					PKWAIT_BLOCK WaitTimer = &Thread->TimerWaitBlock;
					WaitBlock->NextWaitBlock = WaitTimer;
					Timer->Header.WaitListHead.Flink = &WaitTimer->WaitListEntry;
					Timer->Header.WaitListHead.Blink = &WaitTimer->WaitListEntry;
					WaitTimer->NextWaitBlock = WaitBlock;
					if (KiInsertTreeTimer(Timer, *Timeout) == FALSE) {
						WaitStatus = (NTSTATUS)STATUS_TIMEOUT;
						KiTimerUnlock();
						goto NoWait;
					}

					// Boring, ensure that we only set the thread timer once. Otherwise, this will cause to insert the same
					// thread timer over and over in the timer list, which will prevent KiTimerExpiration from removing these
					// duplicated timers and thus it will attempt to endlessly remove the same unremoved timers, causing a deadlock.
					// This can be removed once KiSwapThread and the kernel/user APCs are implemented
					timeout_set = true;
					DueTime.QuadPart = Timer->DueTime.QuadPart;
					KiTimerUnlock();
				}

				// KiTimerExpiration has removed the timer but the objects were not signaled, so we have a timeout
				// (remove this when the thread scheduler is here)
				if (Thread->Timer.Header.Inserted == FALSE) {
					WaitStatus = (NTSTATUS)(STATUS_TIMEOUT);
					goto NoWait;
				}
			}
			else {
				WaitBlock->NextWaitBlock = WaitBlock;
			}

			WaitBlock->NextWaitBlock = &WaitBlockArray[0];
			WaitBlock = &WaitBlockArray[0];
			do {
				ObjectMutant = (PKMUTANT)WaitBlock->Object;
				//InsertTailList(&ObjectMutant->Header.WaitListHead, &WaitBlock->WaitListEntry);
				WaitBlock = WaitBlock->NextWaitBlock;
			} while (WaitBlock != &WaitBlockArray[0]);


			/*
			TODO: We can't implement this and the return values until we have our own thread scheduler
			For now, we'll have to implement waiting here instead of the scheduler.
			This code can all be enabled once we have CPU emulation and our own scheduler in v1.0
			*/

			// Insert the WaitBlock
			//InsertTailList(&ObjectMutant->Header.WaitListHead, &WaitBlock->WaitListEntry);

			// If the current thread is processing a queue object, wake other treads using the same queue
			PRKQUEUE Queue = (PRKQUEUE)Thread->Queue;
			if (Queue != NULL) {
				// TODO:KiActivateWaiterQueue(Queue);
			}

			// Insert the thread into the Wait List
			Thread->Alertable = Alertable;
			Thread->WaitMode = WaitMode;
			Thread->WaitReason = (UCHAR)WaitReason;
			Thread->WaitTime = KeTickCount;
			//Thread->State = Waiting;
			//KiInsertWaitList(WaitMode, Thread);

			//WaitStatus = (NTSTATUS)KiSwapThread();

			//if (WaitStatus == STATUS_USER_APC) {
				// TODO: KiDeliverUserApc();
			//}

			// If the thread was not awakened for an APC, return the Wait Status
			//if (WaitStatus != STATUS_KERNEL_APC) {
			//	return WaitStatus;
			//}

			// TODO: Remove this after we have our own scheduler and the above is implemented
			Sleep(0);

			// Reduce the timout if necessary
			if (Timeout != nullptr) {
				Timeout = KiComputeWaitInterval(OriginalTime, &DueTime, &NewTime);
			}
		}

		// Raise IRQL to DISPATCH_LEVEL and lock the database (only if it's not already at this level)
		if (KeGetCurrentIrql() != DISPATCH_LEVEL) {
			KiLockDispatcherDatabase(&Thread->WaitIrql);
		}
	} while (TRUE);

	// NOTE: we don't need to remove the wait blocks for the object and/or the timer because InsertTailList is disabled at
	// the moment, which means they are never attached to the object wait list. TimerWaitBlock can also stay attached to the timer wait
	// list since KiTimerExpiration disregards it for now.

	// The waiting thead has been alerted, or an APC needs to be delivered
	// So unlock the dispatcher database, lower the IRQ and return the status
	KiUnlockDispatcherDatabase(Thread->WaitIrql);
	if (WaitStatus == STATUS_USER_APC) {
		//TODO: KiDeliverUserApc();
	}

	RETURN(WaitStatus);

NoWait:
	// The wait was satisfied without actually waiting
	// Unlock the database and return the status
	//TODO: KiAdjustQuantumThread(Thread);

	// Don't forget to remove the thread timer if the objects were signaled before the timer expired
	// (remove this when the thread scheduler is here)
	if (timeout_set && Thread->Timer.Header.Inserted == TRUE) {
		KiTimerLock();
		KxRemoveTreeTimer(&Thread->Timer);
		KiTimerUnlock();
	}

	KiUnlockDispatcherDatabase(Thread->WaitIrql);

	if (WaitStatus == STATUS_USER_APC) {
		// TODO: KiDeliverUserApc();
	}

	RETURN(WaitStatus);
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
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Object)
		LOG_FUNC_ARG(WaitReason)
		LOG_FUNC_ARG(WaitMode)
		LOG_FUNC_ARG(Alertable)
		LOG_FUNC_ARG(Timeout)
		LOG_FUNC_END;

	// If the lock is not already held, lock it
	PRKTHREAD Thread = KeGetCurrentThread();
	if (Thread->WaitNext) {
		Thread->WaitNext = FALSE;
	}
	else {
		KiLockDispatcherDatabase(&Thread->WaitIrql);
	}

	// Wait Loop
	// This loop ends 
	PLARGE_INTEGER OriginalTime = Timeout;
	LARGE_INTEGER DueTime, NewTime;
	KWAIT_BLOCK StackWaitBlock;
	PKWAIT_BLOCK WaitBlock = &StackWaitBlock;
	NTSTATUS WaitStatus;
	// Hack variable (remove this when the thread scheduler is here)
	bool timeout_set = false;
	do {
		// Check if we need to let an APC run. This should immediately trigger APC interrupt via a call to UnlockDispatcherDatabase
		if (Thread->ApcState.KernelApcPending && (Thread->WaitIrql < APC_LEVEL)) {
			KiUnlockDispatcherDatabase(Thread->WaitIrql);
		} else {
			PKMUTANT ObjectMutant = (PKMUTANT)Object;
			Thread->WaitStatus = STATUS_SUCCESS;

			if (ObjectMutant->Header.Type == MutantObject) {
				// If the object is a mutant object and it has been acquired MINGLONG times, raise an exception
				// If the mutant is signalled, we can satisfy the wait
				if ((ObjectMutant->Header.SignalState > 0) || (Thread == ObjectMutant->OwnerThread)) {
					if (ObjectMutant->Header.SignalState != MINLONG) {
						KiWaitSatisfyMutant(ObjectMutant, Thread);
						WaitStatus = (NTSTATUS)(Thread->WaitStatus);
						goto NoWait;
					}
					else {
						KiUnlockDispatcherDatabase(Thread->WaitIrql);
						ExRaiseStatus(STATUS_MUTANT_LIMIT_EXCEEDED);
					}
				}
			}
			else if (ObjectMutant->Header.SignalState > 0) {
				// Otherwise, if the signal state is > 0, we can still just satisfy the wait
				KiWaitSatisfyOther(ObjectMutant);
				WaitStatus = STATUS_SUCCESS;
				goto NoWait;
			}

			// If we reached here, the wait could not be satisfied immediately, so we must setup a WaitBlock
			Thread->WaitBlockList = WaitBlock;
			WaitBlock->Object = Object;
			WaitBlock->WaitKey = (CSHORT)(STATUS_SUCCESS);
			WaitBlock->WaitType = WaitAny;
			WaitBlock->Thread = Thread;

			TestForAlertPending(Alertable);

			// Handle a Timeout if specified
			if (Timeout != nullptr) {
				// If the timeout is 0, do not wait
				if (!(Timeout->u.LowPart | Timeout->u.HighPart)) {
					WaitStatus = (NTSTATUS)(STATUS_TIMEOUT);
					goto NoWait;
				}

				// Setup a timer for the thread but only once (for now)
				if (!timeout_set) {
					KiTimerLock();
					PKTIMER Timer = &Thread->Timer;
					PKWAIT_BLOCK WaitTimer = &Thread->TimerWaitBlock;
					WaitBlock->NextWaitBlock = WaitTimer;
					Timer->Header.WaitListHead.Flink = &WaitTimer->WaitListEntry;
					Timer->Header.WaitListHead.Blink = &WaitTimer->WaitListEntry;
					WaitTimer->NextWaitBlock = WaitBlock;
					if (KiInsertTreeTimer(Timer, *Timeout) == FALSE) {
						WaitStatus = (NTSTATUS)STATUS_TIMEOUT;
						KiTimerUnlock();
						goto NoWait;
					}

					// Boring, ensure that we only set the thread timer once. Otherwise, this will cause to insert the same
					// thread timer over and over in the timer list, which will prevent KiTimerExpiration from removing these
					// duplicated timers and thus it will attempt to endlessly remove the same unremoved timers, causing a deadlock.
					// This can be removed once KiSwapThread and the kernel/user APCs are implemented
					timeout_set = true;
					DueTime.QuadPart = Timer->DueTime.QuadPart;
					KiTimerUnlock();
				}

				// KiTimerExpiration has removed the timer but the object was not signaled, so we have a timeout
				// (remove this when the thread scheduler is here)
				if (Thread->Timer.Header.Inserted == FALSE) {
					WaitStatus = (NTSTATUS)(STATUS_TIMEOUT);
					goto NoWait;
				}
			}
			else {
				WaitBlock->NextWaitBlock = WaitBlock;
			}

			/*
				TODO: We can't implement this and the return values until we have our own thread scheduler
				For now, we'll have to implement waiting here instead of the scheduler.
				This code can all be enabled once we have CPU emulation and our own scheduler in v1.0
			*/

			// Insert the WaitBlock
			//InsertTailList(&ObjectMutant->Header.WaitListHead, &WaitBlock->WaitListEntry);

			// If the current thread is processing a queue object, wake other treads using the same queue
			PRKQUEUE Queue = (PRKQUEUE)Thread->Queue;
			if (Queue != NULL) {
				// TODO: KiActivateWaiterQueue(Queue);
			}

			// Insert the thread into the Wait List
			Thread->Alertable = Alertable;
			Thread->WaitMode = WaitMode;
			Thread->WaitReason = (UCHAR)WaitReason;
			Thread->WaitTime = KeTickCount;
			// TODO: Thread->State = Waiting;
			//KiInsertWaitList(WaitMode, Thread);

			/*
			WaitStatus = (NTSTATUS)KiSwapThread();

			if (WaitStatus == STATUS_USER_APC) {
				// TODO: KiDeliverUserApc();
			}

			// If the thread was not awakened for an APC, return the Wait Status
			if (WaitStatus != STATUS_KERNEL_APC) {
				return WaitStatus;
			} */

			// TODO: Remove this after we have our own scheduler and the above is implemented
			Sleep(0);

			// Reduce the timout if necessary
			if (Timeout != nullptr) {
				Timeout = KiComputeWaitInterval(OriginalTime, &DueTime,	&NewTime);
			}
		}

		// Raise IRQL to DISPATCH_LEVEL and lock the database
		if (KeGetCurrentIrql() != DISPATCH_LEVEL) {
			KiLockDispatcherDatabase(&Thread->WaitIrql);
		}
	} while (TRUE);

	// NOTE: we don't need to remove the wait blocks for the object and/or the timer because InsertTailList is disabled at
	// the moment, which means they are never attached to the object wait list. TimerWaitBlock can also stay attached to the timer wait
	// list since KiTimerExpiration disregards it for now.

	// The waiting thead has been alerted, or an APC needs to be delivered
	// So unlock the dispatcher database, lower the IRQ and return the status
	KiUnlockDispatcherDatabase(Thread->WaitIrql);
	if (WaitStatus == STATUS_USER_APC) {
		//TODO: KiDeliverUserApc();
	}

	RETURN(WaitStatus);

NoWait:
	// The wait was satisfied without actually waiting
	// Unlock the database and return the status
	//TODO: KiAdjustQuantumThread(Thread);

	// Don't forget to remove the thread timer if the object was signaled before the timer expired
	// (remove this when the thread scheduler is here)
	if (timeout_set && Thread->Timer.Header.Inserted == TRUE) {
		KiTimerLock();
		KxRemoveTreeTimer(&Thread->Timer);
		KiTimerUnlock();
	}

	KiUnlockDispatcherDatabase(Thread->WaitIrql);

	if (WaitStatus == STATUS_USER_APC) {
		// TODO: KiDeliverUserApc();
	}

	RETURN(WaitStatus);
}
