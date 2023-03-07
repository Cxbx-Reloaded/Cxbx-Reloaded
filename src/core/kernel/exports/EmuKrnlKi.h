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
// *  (c) 2018 Patrick van Logchem <pvanlogchem@gmail.com>
// *  (c) 2019 ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

#pragma once

#include <mutex>

// ReactOS uses a size of 512, but disassembling the kernel reveals it to be 32 instead
#define TIMER_TABLE_SIZE 32

namespace xbox
{
	typedef struct _KTIMER_TABLE_ENTRY
	{
		LIST_ENTRY Entry;
		ULARGE_INTEGER Time;
	} KTIMER_TABLE_ENTRY, *PKTIMER_TABLE_ENTRY;

	// Actually, this lock isn't required, but because raising the irql to dpc level doesn't really prevent thread switching at the
	// moment, we need it for now to prevent concurrent access to the timer table
	typedef struct _KI_TIMER_LOCK
	{
		std::recursive_mutex Mtx;
		int Acquired;
	} KI_TIMER_LOCK;

	// NOTE: since the apc list is per-thread, we could also create a different mutex for each kthread
	extern std::mutex KiApcListMtx;

	void_xt KiInitSystem();

	void_xt KiTimerLock();

	void_xt KiTimerUnlock();

	void_xt KiClockIsr();

	xbox::void_xt NTAPI KiCheckTimerTable
	(
		IN ULARGE_INTEGER CurrentTime
	);

	void_xt KxInsertTimer
	(
		IN PKTIMER Timer,
		IN ulong_xt Hand
	);

	void_xt FASTCALL KiCompleteTimer
	(
		IN PKTIMER Timer,
		IN ulong_xt Hand
	);

	void_xt KiRemoveEntryTimer
	(
		IN PKTIMER Timer,
		IN ulong_xt Hand
	);

	void_xt KxRemoveTreeTimer
	(
		IN PKTIMER Timer
	);

	boolean_xt FASTCALL KiInsertTimerTable
	(
		IN PKTIMER Timer,
		IN ulong_xt Hand
	);

	boolean_xt FASTCALL KiInsertTreeTimer
	(
		IN PKTIMER Timer,
		IN LARGE_INTEGER Interval
	);

	ulong_xt KiComputeTimerTableIndex
	(
		IN ulonglong_xt Interval
	);

	boolean_xt KiComputeDueTime
	(
		IN PKTIMER Timer,
		IN LARGE_INTEGER DueTime,
		OUT PULONG Hand
	);

	boolean_xt FASTCALL KiSignalTimer
	(
		IN PKTIMER Timer
	);

	void_xt NTAPI KiTimerExpiration
	(
		IN PKDPC Dpc,
		IN PVOID DeferredContext,
		IN PVOID SystemArgument1,
		IN PVOID SystemArgument2
	);

	void_xt FASTCALL KiTimerListExpire
	(
		IN PLIST_ENTRY ExpiredListHead,
		IN KIRQL OldIrql
	);

	void_xt FASTCALL KiWaitSatisfyAll
	(
		IN PKWAIT_BLOCK WaitBlock
	);

	void_xt KiExecuteKernelApc();
	void_xt KiExecuteUserApc();

	PLARGE_INTEGER FASTCALL KiComputeWaitInterval
	(
		IN PLARGE_INTEGER OriginalTime,
		IN PLARGE_INTEGER DueTime,
		IN OUT PLARGE_INTEGER NewTime,
		OUT ulonglong_xt *Now
	);

	PLARGE_INTEGER FASTCALL KiComputeWaitInterval
	(
		IN PLARGE_INTEGER OriginalTime,
		IN PLARGE_INTEGER DueTime,
		IN OUT PLARGE_INTEGER NewTime
	);

	// Source: ReactOS
	void_xt NTAPI KiSuspendNop
	(
		IN PKAPC Apc,
		IN PKNORMAL_ROUTINE* NormalRoutine,
		IN PVOID* NormalContext,
		IN PVOID* SystemArgument1,
		IN PVOID* SystemArgument2
	);

	void_xt NTAPI KiFreeUserApc
	(
		IN PKAPC Apc,
		IN PKNORMAL_ROUTINE *NormalRoutine,
		IN PVOID *NormalContext,
		IN PVOID *SystemArgument1,
		IN PVOID *SystemArgument2
	);

	// Source: ReactOS
	void_xt NTAPI KiSuspendThread(
		IN PVOID NormalContext,
		IN PVOID SystemArgument1,
		IN PVOID SystemArgument2
	);

	void_xt NTAPI KiThreadStartup(void_xt);

	xbox::void_xt KiInitializeContextThread(
		IN PKTHREAD Thread,
		IN ulong_xt TlsDataSize,
		IN PKSYSTEM_ROUTINE SystemRoutine,
		IN PKSTART_ROUTINE StartRoutine,
		IN PVOID StartContext
	);

	boolean_xt KiInsertQueueApc
	(
		IN PRKAPC Apc,
		IN KPRIORITY Increment
	);
};

extern xbox::KPROCESS KiUniqueProcess;
extern const xbox::ulong_xt CLOCK_TIME_INCREMENT;
extern xbox::LIST_ENTRY KiWaitInListHead;
extern xbox::KTIMER_TABLE_ENTRY KiTimerTableListHead[TIMER_TABLE_SIZE];
extern xbox::KI_TIMER_LOCK KiTimerMtx;

#define KiLockDispatcherDatabase(OldIrql)      \
	*(OldIrql) = KeRaiseIrqlToDpcLevel()

#define KiLockDispatcherDatabaseAtDpcLevel()   \
	KeRaiseIrqlToDpcLevel()

#define KiLockApcQueue(Thread, OldIrql)        \
    *(OldIrql) = KeRaiseIrqlToSynchLevel()

#define KiUnlockApcQueue(Thread, OldIrql)      \
	KfLowerIrql((OldIrql))

#define KiInsertWaitList(_WaitMode, _Thread) {                  \
    PLIST_ENTRY _ListHead;                                      \
    _ListHead = &KiWaitInListHead;                              \
    InsertTailList(_ListHead, &(_Thread)->WaitListEntry);       \
}

#define KiWaitSatisfyMutant(_Object_, _Thread_) {                            \
    (_Object_)->Header.SignalState -= 1;                                     \
    if ((_Object_)->Header.SignalState == 0) {                               \
        (_Object_)->OwnerThread = (_Thread_);                                \
        if ((_Object_)->Abandoned == TRUE) {                                 \
            (_Object_)->Abandoned = FALSE;                                   \
            (_Thread_)->WaitStatus = X_STATUS_ABANDONED;                     \
        }                                                                    \
                                                                             \
        InsertHeadList((_Thread_)->MutantListHead.Blink,                     \
                       &(_Object_)->MutantListEntry);                        \
    }                                                                        \
}

#define KiWaitSatisfyOther(_Object_) {                                       \
    if (((_Object_)->Header.Type & DISPATCHER_OBJECT_TYPE_MASK) == EventSynchronizationObject) { \
        (_Object_)->Header.SignalState = 0;                                  \
                                                                             \
    } else if ((_Object_)->Header.Type == SemaphoreObject) {                 \
        (_Object_)->Header.SignalState -= 1;                                 \
                                                                             \
    }                                                                        \
}

#define KiWaitSatisfyAny(_Object_, _Thread_) {                               \
    if (((_Object_)->Header.Type & DISPATCHER_OBJECT_TYPE_MASK) == EventSynchronizationObject) { \
        (_Object_)->Header.SignalState = 0;                                  \
                                                                             \
    } else if ((_Object_)->Header.Type == SemaphoreObject) {                 \
        (_Object_)->Header.SignalState -= 1;                                 \
                                                                             \
    } else if ((_Object_)->Header.Type == MutantObject) {                    \
        (_Object_)->Header.SignalState -= 1;                                 \
        if ((_Object_)->Header.SignalState == 0) {                           \
            (_Object_)->OwnerThread = (_Thread_);                            \
            if ((_Object_)->Abandoned == TRUE) {                             \
                (_Object_)->Abandoned = FALSE;                               \
                (_Thread_)->WaitStatus = X_STATUS_ABANDONED;                 \
            }                                                                \
                                                                             \
            InsertHeadList((_Thread_)->MutantListHead.Blink,                 \
                           &(_Object_)->MutantListEntry);                    \
        }                                                                    \
    }                                                                        \
}
