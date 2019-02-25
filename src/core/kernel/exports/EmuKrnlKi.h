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
// *
// *  All rights reserved
// *
// ******************************************************************

#pragma once

#include <mutex>

// Workaround to avoid collisions with the VOID provided by Windows and the one of xboxkrnl
#ifdef VOID
#undef VOID
#endif

// ReactOS uses a size of 512, but disassembling the kernel reveals it to be 32 instead
#define TIMER_TABLE_SIZE 32

namespace xboxkrnl
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
		bool Acquired;
	} KI_TIMER_LOCK;


	VOID KiInitSystem();

	VOID KiTimerLock();

	VOID KiTimerUnlock();

	VOID KiClockIsr();

	VOID NTAPI KiCheckTimerTable(
		IN ULARGE_INTEGER CurrentTime
	);

	VOID KxInsertTimer(
		IN PKTIMER Timer,
		IN ULONG Hand
	);

	VOID FASTCALL KiCompleteTimer(
		IN PKTIMER Timer,
		IN ULONG Hand
		);

	VOID KiRemoveEntryTimer(
		IN PKTIMER Timer,
		IN ULONG Hand
	);

	VOID KxRemoveTreeTimer(
		IN PKTIMER Timer
	);

	BOOLEAN FASTCALL KiInsertTimerTable(
		IN PKTIMER Timer,
		IN ULONG Hand
	);

	BOOLEAN FASTCALL KiInsertTreeTimer(
		IN PKTIMER Timer,
		IN LARGE_INTEGER Interval
	);

	ULONG KiComputeTimerTableIndex(
		IN ULONGLONG Interval
	);

	BOOLEAN KiComputeDueTime(
		IN PKTIMER Timer,
		IN LARGE_INTEGER DueTime,
		OUT PULONG Hand
	);

	BOOLEAN FASTCALL KiSignalTimer(
		IN PKTIMER Timer
	);

	VOID NTAPI KiTimerExpiration(
		IN PKDPC Dpc,
		IN PVOID DeferredContext,
		IN PVOID SystemArgument1,
		IN PVOID SystemArgument2
	);

	VOID FASTCALL KiTimerListExpire(
		IN PLIST_ENTRY ExpiredListHead,
		IN KIRQL OldIrql
	);

	VOID FASTCALL KiWaitSatisfyAll(
		IN PKWAIT_BLOCK WaitBlock
	);
};

extern const xboxkrnl::ULONG CLOCK_TIME_INCREMENT;
extern xboxkrnl::LIST_ENTRY KiWaitInListHead;
extern xboxkrnl::KTIMER_TABLE_ENTRY KiTimerTableListHead[TIMER_TABLE_SIZE];
extern xboxkrnl::KI_TIMER_LOCK KiTimerMtx;

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
            (_Thread_)->WaitStatus = STATUS_ABANDONED;                       \
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
                (_Thread_)->WaitStatus = STATUS_ABANDONED;                   \
            }                                                                \
                                                                             \
            InsertHeadList((_Thread_)->MutantListHead.Blink,                 \
                           &(_Object_)->MutantListEntry);                    \
        }                                                                    \
    }                                                                        \
}
