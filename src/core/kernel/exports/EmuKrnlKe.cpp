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


#include <core\kernel\exports\xboxkrnl.h> // For KeBugCheck, etc.
#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnlLogging.h"

// prevent name collisions
namespace NtDll
{
	#include "core\kernel\support\EmuNtDll.h" // For NtDelayExecution(), etc.
};

#include "core\kernel\init\CxbxKrnl.h" // For CxbxrAbort
#include "core\kernel\support\Emu.h" // For EmuLog(LOG_LEVEL::WARNING, )
#include "EmuKrnl.h" // For InitializeListHead(), etc.
#include "EmuKrnlKe.h"
#include "core\kernel\support\EmuFile.h" // For IsEmuHandle(), NtStatusToString()
#include "core\kernel\support\NativeHandle.h"
#include "Timer.h"
#include "Util.h"

#pragma warning(disable:4005) // Ignore redefined status values
#include <ntstatus.h>

#include <chrono>
#include <thread>
#include <windows.h>
#include <map>

// Copied over from Dxbx. 
// TODO : Move towards thread-simulation based Dpc emulation
typedef struct _DpcData {
	CRITICAL_SECTION Lock;
	std::atomic_flag IsDpcActive;
	xbox::LIST_ENTRY DpcQueue; // TODO : Use KeGetCurrentPrcb()->DpcListHead instead
} DpcData;

DpcData g_DpcData = { 0 }; // Note : g_DpcData is initialized in InitDpcData()

xbox::ulonglong_xt LARGE_INTEGER2ULONGLONG(xbox::LARGE_INTEGER value)
{
	// Weird construction because there doesn't seem to exist an implicit
	// conversion of LARGE_INTEGER to ULONGLONG :
	return *((PULONGLONG)&value);
}

#define TestForAlertPending(Alertable) \
    if (Alertable) { \
        if (Thread->Alerted[WaitMode] != FALSE) { \
            Thread->Alerted[WaitMode] = FALSE; \
            WaitStatus = X_STATUS_ALERTED; \
            break; \
        } else if ((WaitMode != KernelMode) && \
                  (IsListEmpty(&Thread->ApcState.ApcListHead[UserMode])) == FALSE) { \
            Thread->ApcState.UserApcPending = TRUE; \
            WaitStatus = X_STATUS_USER_APC; \
            break; \
        } else if (Thread->Alerted[KernelMode] != FALSE) { \
            Thread->Alerted[KernelMode] = FALSE; \
            WaitStatus = X_STATUS_ALERTED; \
            break; \
        } \
    } else if ((WaitMode != KernelMode) && (Thread->ApcState.UserApcPending)) { \
        WaitStatus = X_STATUS_USER_APC; \
        break; \
    }


// ******************************************************************
// * EmuKeGetPcr()
// * NOTE: This is a macro on the Xbox, however we implement it 
// * as a function so it can suit our emulated KPCR structure
// ******************************************************************
xbox::KPCR* WINAPI EmuKeGetPcr()
{
	// See EmuKeSetPcr()
	xbox::PKPCR Pcr = (xbox::PKPCR)__readfsdword(TIB_ArbitraryDataSlot);

	// If this fails, it's a bug: it means we are executing xbox code from a host thread, and we have forgotten to initialize
	// the xbox thread first
	assert(Pcr);

	return Pcr;
}

// ******************************************************************
// * KeGetCurrentPrcb()
// ******************************************************************
xbox::KPRCB *KeGetCurrentPrcb()
{
	return &(EmuKeGetPcr()->PrcbData);
}

// ******************************************************************
// * KeSetSystemTime()
// ******************************************************************
xbox::void_xt NTAPI xbox::KeSetSystemTime
(
	IN  xbox::PLARGE_INTEGER NewTime,
	OUT xbox::PLARGE_INTEGER OldTime
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
xbox::void_xt NTAPI xbox::KeInitializeTimer
(
	IN PKTIMER Timer
)
{
	LOG_FORWARD("KeInitializeTimerEx");

	KeInitializeTimerEx(Timer, NotificationTimer);
}

// ******************************************************************
// * KeEmptyQueueApc()
// ******************************************************************
xbox::void_xt xbox::KeEmptyQueueApc()
{
	PKTHREAD kThread = KeGetCurrentThread();
	KeEnterCriticalRegion();
	kThread->ApcState.ApcQueueable = FALSE;
	KeLeaveCriticalRegion();

	KiApcListMtx.lock();
	for (int Mode = KernelMode; Mode < MaximumMode; ++Mode) {
		while (!IsListEmpty(&kThread->ApcState.ApcListHead[Mode])) {
			PLIST_ENTRY Entry = kThread->ApcState.ApcListHead[Mode].Flink;
			PKAPC Apc = CONTAINING_RECORD(Entry, KAPC, ApcListEntry);
			RemoveEntryList(Entry);
			ExFreePool(Apc);
		}
	}
	KiApcListMtx.unlock();
}

// Source: ReactOS (modified to fit in xbox compatibility layer)
template<bool IsHostThread>
xbox::void_xt xbox::KeInitializeThread(
	IN OUT PKTHREAD Thread,
	IN PVOID KernelStack,
	IN ulong_xt KernelStackSize,
	IN ulong_xt TlsDataSize,
	IN PKSYSTEM_ROUTINE SystemRoutine,
	IN PKSTART_ROUTINE StartRoutine,
	IN PVOID StartContext,
	IN PKPROCESS Process
)
{
	/* ReactOS's KeInitThread inline code begin */

	/* Initialize the Dispatcher Header */
	Thread->Header.Type = xbox::ThreadObject;
	Thread->Header.Size = sizeof(xbox::KTHREAD) / sizeof(xbox::long_xt);
	// ThreadControlFlags
	// DebugActive
	Thread->Header.SignalState = 0;
	InitializeListHead(&Thread->Header.WaitListHead);

	/* Initialize the Mutant List */
	InitializeListHead(&Thread->MutantListHead);

#if 0 // Not used or not yet reverse engineered
	/* Set swap settings */
	Thread->EnableStackSwap = TRUE;
	Thread->IdealProcessor = 1;
	Thread->SwapBusy = FALSE;
	Thread->KernelStackResident = TRUE;
	Thread->AdjustReason = AdjustNone;
#endif

#if 0 // Not used or not yet reverse engineered
	/* Initialize the lock */
	KeInitializeSpinLock(&Thread->ThreadLock);
#endif

#if 0 // Not used or not yet reverse engineered
	/* Setup the Service Descriptor Table for Native Calls */
	Thread->ServiceTable = KeServiceDescriptorTable;
#endif

	/* Setup APC Fields */
	InitializeListHead(&Thread->ApcState.ApcListHead[xbox::KernelMode]);
	InitializeListHead(&Thread->ApcState.ApcListHead[xbox::UserMode]);
	Thread->KernelApcDisable = 0;
	Thread->ApcState.Process = &KiUniqueProcess;
	Thread->ApcState.ApcQueueable = TRUE;
	Thread->ApcState.Process->ThreadQuantum = KiUniqueProcess.ThreadQuantum;

	/* Initialize the Suspend APC */
	KeInitializeApc(
		&Thread->SuspendApc,
		Thread,
		KiSuspendNop,
		zeroptr,
		KiSuspendThread,
		KernelMode,
		zeroptr);

	/* Initialize the Suspend Semaphore */
	KeInitializeSemaphore(&Thread->SuspendSemaphore, 0, 2);

	/* Setup the timer */
	xbox::KeInitializeTimer(&Thread->Timer);
	xbox::PKWAIT_BLOCK TimerWaitBlock = &Thread->TimerWaitBlock;
	TimerWaitBlock->Object = &Thread->Timer;
	TimerWaitBlock->WaitKey = (xbox::cshort_xt)X_STATUS_TIMEOUT;
	TimerWaitBlock->WaitType = xbox::WaitAny;
	TimerWaitBlock->Thread = Thread;
	TimerWaitBlock->NextWaitBlock = zeroptr;

	/* Link the two wait lists together */
	TimerWaitBlock->WaitListEntry.Flink = &Thread->Timer.Header.WaitListHead;
	TimerWaitBlock->WaitListEntry.Blink = &Thread->Timer.Header.WaitListHead;

#if 0 // Not used or not yet reverse engineered
	/* Set the TEB and process */
	Thread->Teb = Teb;
	Thread->Process = Process;
#endif

	/* Set the Thread Stacks */
	Thread->StackBase = KernelStack;
	Thread->StackLimit = reinterpret_cast<PVOID>(reinterpret_cast<ulong_ptr_xt>(KernelStack) - KernelStackSize);

	/* Initialize the Thread Context */
	KiInitializeContextThread(Thread, TlsDataSize, SystemRoutine, StartRoutine, StartContext);

	/* Set the Thread to initialized */
	Thread->State = Initialized;

	/* ReactOS's KeInitThread inline code end */

	/* ReactOS's KeStartThread inline code begin */
	// NOTE: The cxbxr's kernel initialization will not be insert into ThreadListHead of Process.
	if constexpr (!IsHostThread) {
		/* Setup static fields from parent */
		Thread->DisableBoost = Process->DisableBoost;
		Thread->Quantum = Process->ThreadQuantum;

		/* Setup volatile data */
		Thread->Priority = Process->BasePriority;
		Thread->BasePriority = Process->BasePriority;

		/* Lock the Dispatcher Database */
		UCHAR orig_irql = KeRaiseIrqlToDpcLevel();

		/* Insert the thread into the process list */
		InsertTailList(&Process->ThreadListHead, &Thread->ThreadListEntry);
		/* Increase the stack count */
		Process->StackCount++;

		/* Release lock and return */
		KfLowerIrql(orig_irql);
	}
	/* ReactOS's KeStartThread inline code end */
}
template
xbox::void_xt xbox::KeInitializeThread<true>(
	IN OUT PKTHREAD Thread,
	IN PVOID KernelStack,
	IN ulong_xt KernelStackSize,
	IN ulong_xt TlsDataSize,
	IN PKSYSTEM_ROUTINE SystemRoutine,
	IN PKSTART_ROUTINE StartRoutine,
	IN PVOID StartContext,
	IN PKPROCESS Process
	);
template
xbox::void_xt xbox::KeInitializeThread<false>(
	IN OUT PKTHREAD Thread,
	IN PVOID KernelStack,
	IN ulong_xt KernelStackSize,
	IN ulong_xt TlsDataSize,
	IN PKSYSTEM_ROUTINE SystemRoutine,
	IN PKSTART_ROUTINE StartRoutine,
	IN PVOID StartContext,
	IN PKPROCESS Process
	);

// Forward KeLowerIrql() to KfLowerIrql()
#define KeLowerIrql(NewIrql) \
	KfLowerIrql(NewIrql)

// Forward KeRaiseIrql() to KfRaiseIrql()
#define KeRaiseIrql(NewIrql, OldIrql) \
	*(OldIrql) = KfRaiseIrql(NewIrql)

void ExecuteDpcQueue()
{
	xbox::PKDPC pkdpc;

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
		pkdpc = CONTAINING_RECORD(RemoveHeadList(&(g_DpcData.DpcQueue)), xbox::KDPC, DpcListEntry);
		// Mark it as no longer linked into the DpcQueue
		pkdpc->Inserted = FALSE;
		// Set DpcRoutineActive to support KeIsExecutingDpc:
		g_DpcData.IsDpcActive.test_and_set();
		KeGetCurrentPrcb()->DpcRoutineActive = TRUE; // Experimental
		LeaveCriticalSection(&(g_DpcData.Lock));
		EmuLog(LOG_LEVEL::DEBUG, "Global DpcQueue, calling DPC object 0x%.8X at 0x%.8X", pkdpc, pkdpc->DeferredRoutine);

		// Call the Deferred Procedure  :
		pkdpc->DeferredRoutine(
			pkdpc,
			pkdpc->DeferredContext,
			pkdpc->SystemArgument1,
			pkdpc->SystemArgument2);

		EnterCriticalSection(&(g_DpcData.Lock));
		KeGetCurrentPrcb()->DpcRoutineActive = FALSE; // Experimental
		g_DpcData.IsDpcActive.clear();
	}

//    Assert(g_DpcData._dwThreadId == GetCurrentThreadId());
//    Assert(g_DpcData._dwDpcThreadId == g_DpcData._dwThreadId);
//    g_DpcData._dwDpcThreadId = 0;
	LeaveCriticalSection(&(g_DpcData.Lock));
}

void InitDpcData()
{
	// Let's initialize the Dpc handling thread too,
	// here for now (should be called by our caller)
	InitializeCriticalSection(&(g_DpcData.Lock));
	InitializeListHead(&(g_DpcData.DpcQueue));
}

bool IsDpcActive()
{
	return g_DpcData.IsDpcActive.test();
}

static constexpr uint32_t XBOX_TSC_FREQUENCY = 733333333; // Xbox Time Stamp Counter Frequency = 733333333 (CPU Clock)
static constexpr uint32_t XBOX_ACPI_FREQUENCY = 3375000;  // Xbox ACPI frequency (3.375 mhz)

ULONGLONG CxbxGetPerformanceCounter(bool acpi)
{
	const int64_t period = acpi ? XBOX_ACPI_FREQUENCY : XBOX_TSC_FREQUENCY;
	return Timer_GetScaledPerformanceCounter(period);
}

// ******************************************************************
// * 0x005C - KeAlertResumeThread()
// ******************************************************************
// Source:Dxbx
XBSYSAPI EXPORTNUM(92) xbox::ntstatus_xt NTAPI xbox::KeAlertResumeThread
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
XBSYSAPI EXPORTNUM(93) xbox::ntstatus_xt NTAPI xbox::KeAlertThread
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
XBSYSAPI EXPORTNUM(94) xbox::ntstatus_xt NTAPI xbox::KeBoostPriorityThread
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
XBSYSAPI EXPORTNUM(95) xbox::void_xt NTAPI xbox::KeBugCheck
(
	IN ulong_xt BugCheckMode
)
{
	LOG_FORWARD("KeBugCheckEx");

	KeBugCheckEx(BugCheckMode, 0, 0, 0, 0);
}

// ******************************************************************
// * 0x0060 - KeBugCheckEx()
// ******************************************************************
// Source:Dxbx
XBSYSAPI EXPORTNUM(96) xbox::ntstatus_xt NTAPI xbox::KeBugCheckEx
(
	IN dword_xt BugCheckCode,
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
		CxbxrAbort(NULL);
	}

	KeBugCheckIgnored = true;
	
	RETURN(S_OK);
}

// ******************************************************************
// * 0x0061 - KeCancelTimer()
// ******************************************************************
XBSYSAPI EXPORTNUM(96) xbox::boolean_xt NTAPI xbox::KeCancelTimer
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

xbox::PKINTERRUPT EmuInterruptList[MAX_BUS_INTERRUPT_LEVEL + 1] = { 0 };

// ******************************************************************
// * 0x0062 - KeConnectInterrupt()
// ******************************************************************
XBSYSAPI EXPORTNUM(98) xbox::boolean_xt NTAPI xbox::KeConnectInterrupt
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
XBSYSAPI EXPORTNUM(99) xbox::ntstatus_xt NTAPI xbox::KeDelayExecutionThread
(
	IN KPROCESSOR_MODE  WaitMode,
	IN boolean_xt          Alertable,
	IN PLARGE_INTEGER   Interval
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(WaitMode)
		LOG_FUNC_ARG(Alertable)
		LOG_FUNC_ARG(Interval)
		LOG_FUNC_END;

	// Because user APCs from NtQueueApcThread are now handled by the kernel, we need to wait for them ourselves
	// We can't remove NtDll::NtDelayExecution until all APCs queued by Io are implemented by our kernel as well
	// Test case: Metal Slug 3

	xbox::ntstatus_xt ret = WaitApc([Alertable]() -> std::optional<ntstatus_xt> {
		NtDll::LARGE_INTEGER ExpireTime;
		ExpireTime.QuadPart = 0;
		NTSTATUS Status = NtDll::NtDelayExecution(Alertable, &ExpireTime);
		// Any success codes that are not related to alerting should be masked out
		if (Status >= 0 && Status != STATUS_ALERTED && Status != STATUS_USER_APC) {
			return std::nullopt;
		}
		return std::make_optional<ntstatus_xt>(Status);
		}, Interval, Alertable, WaitMode);

	if (ret == X_STATUS_TIMEOUT) {
		// NOTE: this function considers a timeout a success
		ret = X_STATUS_SUCCESS;
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x0064 - KeDisconnectInterrupt()
// ******************************************************************
XBSYSAPI EXPORTNUM(100) xbox::void_xt NTAPI xbox::KeDisconnectInterrupt
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
XBSYSAPI EXPORTNUM(101) xbox::void_xt NTAPI xbox::KeEnterCriticalRegion
(
    void_xt
)
{
    LOG_FUNC();
    PKTHREAD thread = KeGetCurrentThread();
    thread->KernelApcDisable--;
}

// ******************************************************************
// * 0x0067 - KeGetCurrentIrql()
// ******************************************************************
XBSYSAPI EXPORTNUM(103) xbox::KIRQL NTAPI xbox::KeGetCurrentIrql(void)
{
	LOG_FUNC(); // TODO : Remove nested logging on this somehow, so we can call this (instead of inlining)

	KPCR* Pcr = EmuKeGetPcr();
	KIRQL Irql = (KIRQL)Pcr->Irql;

	RETURN_TYPE(KIRQL_TYPE, Irql);
}

// ******************************************************************
// * 0x0068 - KeGetCurrentThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(104) xbox::PKTHREAD NTAPI xbox::KeGetCurrentThread(void)
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
XBSYSAPI EXPORTNUM(105) xbox::void_xt NTAPI xbox::KeInitializeApc
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
XBSYSAPI EXPORTNUM(106) xbox::void_xt NTAPI xbox::KeInitializeDeviceQueue
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
XBSYSAPI EXPORTNUM(107) xbox::void_xt NTAPI xbox::KeInitializeDpc
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
XBSYSAPI EXPORTNUM(108) xbox::void_xt NTAPI xbox::KeInitializeEvent
(
	IN PRKEVENT Event,
	IN EVENT_TYPE Type,
	IN boolean_xt SignalState
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Event)
		LOG_FUNC_ARG(Type)
		LOG_FUNC_ARG(SignalState)
		LOG_FUNC_END;

	// HACK: Since we forward to NtDll::NtCreateEvent, this *might* be a Windows handle instead of our own
	// In this case, it is already initialized so no need todo anything
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
XBSYSAPI EXPORTNUM(109) xbox::void_xt NTAPI xbox::KeInitializeInterrupt
(
	OUT PKINTERRUPT Interrupt,
	IN PKSERVICE_ROUTINE ServiceRoutine,
	IN PVOID ServiceContext,
	IN ulong_xt Vector,
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
XBSYSAPI EXPORTNUM(110) xbox::void_xt NTAPI xbox::KeInitializeMutant
(
	IN PRKMUTANT Mutant,
	IN boolean_xt InitialOwner
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
XBSYSAPI EXPORTNUM(111) xbox::void_xt NTAPI xbox::KeInitializeQueue
(
	IN PKQUEUE Queue,
	IN ulong_xt Count OPTIONAL
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
XBSYSAPI EXPORTNUM(112) xbox::void_xt NTAPI xbox::KeInitializeSemaphore
(
	IN PRKSEMAPHORE Semaphore,
	IN long_xt Count,
	IN long_xt Limit
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
XBSYSAPI EXPORTNUM(113) xbox::void_xt NTAPI xbox::KeInitializeTimerEx
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

XBSYSAPI EXPORTNUM(114) xbox::boolean_xt NTAPI xbox::KeInsertByKeyDeviceQueue
(
	IN PKDEVICE_QUEUE DeviceQueue,
	IN PKDEVICE_QUEUE_ENTRY DeviceQueueEntry,
	IN ulong_xt SortKey
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
XBSYSAPI EXPORTNUM(115) xbox::boolean_xt NTAPI xbox::KeInsertDeviceQueue
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

XBSYSAPI EXPORTNUM(116) xbox::long_xt NTAPI xbox::KeInsertHeadQueue
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

XBSYSAPI EXPORTNUM(117) xbox::long_xt NTAPI xbox::KeInsertQueue
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

// ******************************************************************
// * 0x0076 - KeInsertQueueApc()
// ******************************************************************
XBSYSAPI EXPORTNUM(118) xbox::boolean_xt NTAPI xbox::KeInsertQueueApc
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

	KIRQL OldIrql = KeRaiseIrqlToDpcLevel();

	PKTHREAD kThread = Apc->Thread;
	if (kThread->ApcState.ApcQueueable == FALSE) {
		KfLowerIrql(OldIrql);
		RETURN(FALSE);
	}
	else {
		Apc->SystemArgument1 = SystemArgument1;
		Apc->SystemArgument2 = SystemArgument2;

		if (Apc->Inserted) {
			KfLowerIrql(OldIrql);
			RETURN(FALSE);
		}
		else {
			KiApcListMtx.lock();
			InsertTailList(&kThread->ApcState.ApcListHead[Apc->ApcMode], &Apc->ApcListEntry);
			Apc->Inserted = TRUE;
			KiApcListMtx.unlock();

			// We can only attempt to execute the queued apc right away if it is been inserted in the current thread, because otherwise the KTHREAD
			// in the fs selector will not be correct
			if (kThread == KeGetCurrentThread()) {
				if (Apc->ApcMode == KernelMode) { // kernel apc
					// NOTE: this is wrong, we should check the thread state instead of just signaling the kernel apc, but we currently
					// don't set the appropriate state in kthread
					kThread->ApcState.KernelApcPending = TRUE;
					KiExecuteKernelApc();
				}
				else if ((kThread->WaitMode == UserMode) && (kThread->Alertable)) { // user apc
					// NOTE: this should also check the thread state
					kThread->ApcState.UserApcPending = TRUE;
					KiExecuteUserApc();
				}
			}

			KfLowerIrql(OldIrql);
			RETURN(TRUE);
		}
	}
}

// ******************************************************************
// * 0x0077 - KeInsertQueueDpc()
// ******************************************************************
XBSYSAPI EXPORTNUM(119) xbox::boolean_xt NTAPI xbox::KeInsertQueueDpc
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
		if (!IsDpcActive()) {
			HalRequestSoftwareInterrupt(DISPATCH_LEVEL);
		}
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
XBSYSAPI EXPORTNUM(121) xbox::boolean_xt NTAPI xbox::KeIsExecutingDpc
()
{
	LOG_FUNC();

#if 0
	// This is the correct implementation, but it doesn't work because our Prcb is per-thread instead of being per-processor
	BOOLEAN ret = (BOOLEAN)KeGetCurrentPrcb()->DpcRoutineActive;
#else
	BOOLEAN ret = (BOOLEAN)IsDpcActive();
#endif

	RETURN(ret);
}

// ******************************************************************
// * 0x0078 - KeInterruptTime
// ******************************************************************
XBSYSAPI EXPORTNUM(120) xbox::KSYSTEM_TIME xbox::KeInterruptTime = { 0, 0, 0 };

// ******************************************************************
// * 0x007A - KeLeaveCriticalRegion()
// ******************************************************************
XBSYSAPI EXPORTNUM(122) xbox::void_xt NTAPI xbox::KeLeaveCriticalRegion
(
    void_xt
)
{
    LOG_FUNC();

    PKTHREAD thread = KeGetCurrentThread();
    thread->KernelApcDisable++;
    if(thread->KernelApcDisable == 0) {
		LIST_ENTRY *apcListHead = &thread->ApcState.ApcListHead[KernelMode];
        if(apcListHead->Flink != apcListHead) {
            thread->ApcState.KernelApcPending = TRUE;
            HalRequestSoftwareInterrupt(APC_LEVEL);
        }
    }
}

XBSYSAPI EXPORTNUM(123) xbox::long_xt NTAPI xbox::KePulseEvent
(
	IN PRKEVENT Event,
	IN KPRIORITY Increment,
	IN boolean_xt Wait
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

XBSYSAPI EXPORTNUM(124) xbox::long_xt NTAPI xbox::KeQueryBasePriorityThread
(
	IN PKTHREAD Thread
)
{
	LOG_FUNC_ONE_ARG(Thread);

	KIRQL OldIrql;
	KiLockDispatcherDatabase(&OldIrql);

	// It cannot fail because all thread handles are created by ob
	const auto& nativeHandle = GetNativeHandle<true>(reinterpret_cast<PETHREAD>(Thread)->UniqueThread);
	long_xt ret = GetThreadPriority(*nativeHandle);

	KiUnlockDispatcherDatabase(OldIrql);

	RETURN(ret);
}

// ******************************************************************
// * 0x007D - KeQueryInterruptTime()
// ******************************************************************
XBSYSAPI EXPORTNUM(125) xbox::ulonglong_xt NTAPI xbox::KeQueryInterruptTime(void)
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
XBSYSAPI EXPORTNUM(126) xbox::ulonglong_xt NTAPI xbox::KeQueryPerformanceCounter(void)
{
	LOG_FUNC();
	ULONGLONG ret;
	ret = CxbxGetPerformanceCounter(/*acpi=*/true);
	RETURN(ret);
}

// ******************************************************************
// * 0x007F - KeQueryPerformanceFrequency()
// ******************************************************************
XBSYSAPI EXPORTNUM(127) xbox::ulonglong_xt NTAPI xbox::KeQueryPerformanceFrequency(void)
{
	LOG_FUNC();
	ULONGLONG ret = XBOX_ACPI_FREQUENCY;
	RETURN(ret);
}

// ******************************************************************
// * 0x0080 - KeQuerySystemTime()
// ******************************************************************
XBSYSAPI EXPORTNUM(128) xbox::void_xt NTAPI xbox::KeQuerySystemTime
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
XBSYSAPI EXPORTNUM(129) xbox::uchar_xt NTAPI xbox::KeRaiseIrqlToDpcLevel()
{
	LOG_FORWARD(KfRaiseIrql);

	return KfRaiseIrql(DISPATCH_LEVEL);
}

// ******************************************************************
// * 0x0082 - KeRaiseIrqlToSynchLevel()
// ******************************************************************
XBSYSAPI EXPORTNUM(130) xbox::uchar_xt NTAPI xbox::KeRaiseIrqlToSynchLevel()
{
	LOG_FORWARD(KfRaiseIrql);

	return KfRaiseIrql(SYNC_LEVEL);
}

XBSYSAPI EXPORTNUM(131) xbox::long_xt NTAPI xbox::KeReleaseMutant
(
	IN PRKMUTANT Mutant,
	IN KPRIORITY Increment,
	IN boolean_xt Abandoned,
	IN boolean_xt Wait
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

XBSYSAPI EXPORTNUM(132) xbox::long_xt NTAPI xbox::KeReleaseSemaphore
(
	IN PRKSEMAPHORE Semaphore,
	IN KPRIORITY Increment,
	IN long_xt Adjustment,
	IN boolean_xt Wait
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
		ExRaiseStatus(X_STATUS_SEMAPHORE_LIMIT_EXCEEDED);
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

XBSYSAPI EXPORTNUM(133) xbox::PKDEVICE_QUEUE_ENTRY NTAPI xbox::KeRemoveByKeyDeviceQueue
(
	IN PKDEVICE_QUEUE DeviceQueue,
	IN ulong_xt SortKey
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

XBSYSAPI EXPORTNUM(134) xbox::PKDEVICE_QUEUE_ENTRY NTAPI xbox::KeRemoveDeviceQueue
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

XBSYSAPI EXPORTNUM(135) xbox::boolean_xt NTAPI xbox::KeRemoveEntryDeviceQueue
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


XBSYSAPI EXPORTNUM(136) xbox::PLIST_ENTRY NTAPI xbox::KeRemoveQueue
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
XBSYSAPI EXPORTNUM(137) xbox::boolean_xt NTAPI xbox::KeRemoveQueueDpc
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
XBSYSAPI EXPORTNUM(138) xbox::long_xt NTAPI xbox::KeResetEvent
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
XBSYSAPI EXPORTNUM(139) xbox::ntstatus_xt NTAPI xbox::KeRestoreFloatingPointState
(
	IN PKFLOATING_SAVE     PublicFloatSave
)
{
	LOG_FUNC_ONE_ARG(PublicFloatSave);

	NTSTATUS ret = X_STATUS_SUCCESS;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

// ******************************************************************
// * 0x008C - KeResumeThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(140) xbox::ulong_xt NTAPI xbox::KeResumeThread
(
	IN PKTHREAD Thread
)
{
	LOG_FUNC_ONE_ARG(Thread);

	NTSTATUS ret = X_STATUS_SUCCESS;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

XBSYSAPI EXPORTNUM(141) xbox::PLIST_ENTRY NTAPI xbox::KeRundownQueue
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
XBSYSAPI EXPORTNUM(142) xbox::ntstatus_xt NTAPI xbox::KeSaveFloatingPointState
(
	OUT PKFLOATING_SAVE     PublicFloatSave
)
{
	LOG_FUNC_ONE_ARG_OUT(PublicFloatSave);

	NTSTATUS ret = X_STATUS_SUCCESS;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

// ******************************************************************
// * 0x008F - KeSetBasePriorityThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(143) xbox::long_xt NTAPI xbox::KeSetBasePriorityThread
(
	IN PKTHREAD  Thread,
	IN long_xt  Priority
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Thread)
		LOG_FUNC_ARG(Priority)
		LOG_FUNC_END;

	KIRQL oldIRQL;
	KiLockDispatcherDatabase(&oldIRQL);

	// It cannot fail because all thread handles are created by ob
	const auto &nativeHandle = GetNativeHandle<true>(reinterpret_cast<PETHREAD>(Thread)->UniqueThread);

	if (Priority == 16) {
		Priority = THREAD_PRIORITY_TIME_CRITICAL;
	}
	else if (Priority == -16) {
		Priority = THREAD_PRIORITY_IDLE;
	}

	BOOL result = SetThreadPriority(*nativeHandle, Priority);
	if (!result) {
		EmuLog(LOG_LEVEL::WARNING, "SetThreadPriority failed: %s", WinError2Str().c_str());
	}

	KiUnlockDispatcherDatabase(oldIRQL);

	RETURN(result);
}

XBSYSAPI EXPORTNUM(144) xbox::boolean_xt NTAPI xbox::KeSetDisableBoostThread
(
	IN PKTHREAD Thread,
	IN boolean_xt Disable
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Thread)
		LOG_FUNC_ARG(Disable)
		LOG_FUNC_END;

	KIRQL oldIRQL;
	KiLockDispatcherDatabase(&oldIRQL);

	// It cannot fail because all thread handles are created by ob
	const auto &nativeHandle = GetNativeHandle<true>(reinterpret_cast<PETHREAD>(Thread)->UniqueThread);

	boolean_xt prevDisableBoost = Thread->DisableBoost;
	Thread->DisableBoost = (CHAR)Disable;

	BOOL bRet = SetThreadPriorityBoost(*nativeHandle, Disable);
	if (!bRet) {
		EmuLog(LOG_LEVEL::WARNING, "SetThreadPriorityBoost failed: %s", WinError2Str().c_str());
	}

	KiUnlockDispatcherDatabase(oldIRQL);

	RETURN(prevDisableBoost);
}

// ******************************************************************
// * 0x0091 - KeSetEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(145) xbox::long_xt NTAPI xbox::KeSetEvent
(
	IN PRKEVENT		Event,
	IN KPRIORITY	Increment,
	IN boolean_xt		Wait	
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

XBSYSAPI EXPORTNUM(146) xbox::void_xt NTAPI xbox::KeSetEventBoostPriority
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
		// TODO: KiUnwaitThread(WaitThread, X_STATUS_SUCCESS, 1);
		// For now, we just sleep to give other threads time to wake
		// See KePulseEvent
		Sleep(1);
	}

	KiUnlockDispatcherDatabase(OldIrql);
}

XBSYSAPI EXPORTNUM(147) xbox::KPRIORITY NTAPI xbox::KeSetPriorityProcess
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
XBSYSAPI EXPORTNUM(148) xbox::boolean_xt NTAPI xbox::KeSetPriorityThread
(
    IN PKTHREAD  Thread,
    IN long_xt  Priority
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Thread)
		LOG_FUNC_ARG(Priority)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(1);
}

// ******************************************************************
// * 0x0095 - KeSetTimer()
// ******************************************************************
XBSYSAPI EXPORTNUM(149) xbox::boolean_xt NTAPI xbox::KeSetTimer
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
XBSYSAPI EXPORTNUM(150) xbox::boolean_xt NTAPI xbox::KeSetTimerEx
(
	IN PKTIMER        Timer,
	IN LARGE_INTEGER  DueTime,
	IN long_xt           Period OPTIONAL,
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
	if (!KiComputeDueTime(Timer, DueTime, (PULONG)&Hand))
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
	KiTimerUnlock();
	KiUnlockDispatcherDatabase(OldIrql);

	RETURN(Inserted);
}

// ******************************************************************
// * 0x0097 - KeStallExecutionProcessor()
// ******************************************************************
XBSYSAPI EXPORTNUM(151) xbox::void_xt NTAPI xbox::KeStallExecutionProcessor
(
	IN ulong_xt MicroSeconds
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
XBSYSAPI EXPORTNUM(152) xbox::ulong_xt NTAPI xbox::KeSuspendThread
(
	IN PKTHREAD Thread
)
{
	LOG_FUNC_ONE_ARG(Thread);

	NTSTATUS ret = X_STATUS_SUCCESS;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

// ******************************************************************
// * 0x0099 - KeSynchronizeExecution()
// ******************************************************************
XBSYSAPI EXPORTNUM(153) xbox::boolean_xt NTAPI xbox::KeSynchronizeExecution
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
XBSYSAPI EXPORTNUM(154) xbox::KSYSTEM_TIME xbox::KeSystemTime = { 0, 0, 0 };

// ******************************************************************
// * 0x009B - KeTestAlertThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(155) xbox::boolean_xt NTAPI xbox::KeTestAlertThread
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
XBSYSAPI EXPORTNUM(156) xbox::dword_xt VOLATILE xbox::KeTickCount = 0;

// ******************************************************************
// * 0x009D - KeTimeIncrement
// ******************************************************************
XBSYSAPI EXPORTNUM(157) xbox::ulong_xt xbox::KeTimeIncrement = CLOCK_TIME_INCREMENT;

// ******************************************************************
// * 0x009E - KeWaitForMultipleObjects()
// ******************************************************************
XBSYSAPI EXPORTNUM(158) xbox::ntstatus_xt NTAPI xbox::KeWaitForMultipleObjects
(
	IN ulong_xt Count,
	IN PVOID Object[],
	IN WAIT_TYPE WaitType,
	IN KWAIT_REASON WaitReason,
	IN KPROCESSOR_MODE WaitMode,
	IN boolean_xt Alertable,
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
			Thread->WaitStatus = X_STATUS_SUCCESS;

			for (ULONG Index = 0; Index < Count; Index += 1) {
				ObjectMutant = (PKMUTANT)Object[Index];
				if (WaitType == WaitAny) {
					if (ObjectMutant->Header.Type == MutantObject) {
						// If the object is a mutant object and it has been acquired MINGLONG times, raise an exception
						// If the mutant is signalled, we can satisfy the wait
						if ((ObjectMutant->Header.SignalState > 0) || (Thread == ObjectMutant->OwnerThread)) {
							if (ObjectMutant->Header.SignalState != MINLONG) {
								KiWaitSatisfyMutant(ObjectMutant, Thread);
								WaitStatus = (NTSTATUS)(Index | Thread->WaitStatus);
								goto NoWait;
							}
							else {
								KiUnlockDispatcherDatabase(Thread->WaitIrql);
								ExRaiseStatus(X_STATUS_MUTANT_LIMIT_EXCEEDED);
							}
						}
					}
					else if (ObjectMutant->Header.SignalState) {
						// Otherwise, if the signal state is > 0, we can still just satisfy the wait
						KiWaitSatisfyOther(ObjectMutant);
						WaitStatus = Index;
						goto NoWait;
					}
				} else {
					if (ObjectMutant->Header.Type == MutantObject) {
						if ((Thread == ObjectMutant->OwnerThread) && (ObjectMutant->Header.SignalState == MINLONG)) {
							KiUnlockDispatcherDatabase(Thread->WaitIrql);
							ExRaiseStatus(X_STATUS_MUTANT_LIMIT_EXCEEDED);
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
				WaitBlock->WaitKey = (cshort_xt)(Index);
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

			//if (WaitStatus == X_STATUS_USER_APC) {
				// KiExecuteUserApc();
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
	if (WaitStatus == X_STATUS_USER_APC) {
		KiExecuteUserApc();
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

	if (WaitStatus == X_STATUS_USER_APC) {
		KiExecuteUserApc();
	}

	RETURN(WaitStatus);
}

// ******************************************************************
// * 0x009F - KeWaitForSingleObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(159) xbox::ntstatus_xt NTAPI xbox::KeWaitForSingleObject
(
	IN PVOID Object,
	IN KWAIT_REASON WaitReason,
	IN KPROCESSOR_MODE WaitMode,
	IN boolean_xt Alertable,
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
			Thread->WaitStatus = X_STATUS_SUCCESS;

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
						ExRaiseStatus(X_STATUS_MUTANT_LIMIT_EXCEEDED);
					}
				}
			}
			else if (ObjectMutant->Header.SignalState > 0) {
				// Otherwise, if the signal state is > 0, we can still just satisfy the wait
				KiWaitSatisfyOther(ObjectMutant);
				WaitStatus = X_STATUS_SUCCESS;
				goto NoWait;
			}

			// If we reached here, the wait could not be satisfied immediately, so we must setup a WaitBlock
			Thread->WaitBlockList = WaitBlock;
			WaitBlock->Object = Object;
			WaitBlock->WaitKey = (cshort_xt)(X_STATUS_SUCCESS);
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

			if (WaitStatus == X_STATUS_USER_APC) {
				KiExecuteUserApc();
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
	if (WaitStatus == X_STATUS_USER_APC) {
		KiExecuteUserApc();
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

	if (WaitStatus == X_STATUS_USER_APC) {
		KiExecuteUserApc();
	}

	RETURN(WaitStatus);
}
