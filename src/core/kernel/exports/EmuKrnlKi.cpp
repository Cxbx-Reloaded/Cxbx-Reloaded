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
// *   Cxbx->src->CxbxKrnl->EmuKrnlKi.cpp
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

// Acknowledgment: ReactOS (GPLv2)
// https://github.com/reactos/reactos

// Changed from ReactOS: slight changes to the Hand parameter usage

#define _XBOXKRNL_DEFEXTRN_

#define LOG_PREFIX CXBXR_MODULE::KI

// prevent name collisions
namespace xboxkrnl
{
#include <xboxkrnl/xboxkrnl.h> // For KeBugCheck, etc.
};

#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnl.h" // for the list support functions
#include "EmuKrnlKi.h"

// ReactOS uses a size of 512, but disassembling the kernel reveals it to be 32 instead
#define TIMER_TABLE_SIZE 32

xboxkrnl::KTIMER_TABLE_ENTRY KiTimerTableListHead[TIMER_TABLE_SIZE];


VOID xboxkrnl::KxInsertTimer(
	IN xboxkrnl::PKTIMER Timer,
	IN xboxkrnl::ULONG Hand
)
{
	/* Try to insert the timer */
	if (KiInsertTimerTable(Timer, Hand))
	{
		/* Complete it */
		KiCompleteTimer(Timer, Hand);
	}
}

VOID FASTCALL xboxkrnl::KiCompleteTimer(
	IN xboxkrnl::PKTIMER Timer,
	IN xboxkrnl::ULONG Hand
)
{
	LIST_ENTRY ListHead;
	BOOLEAN RequestInterrupt = FALSE;

	/* Remove it from the timer list */
	KiRemoveEntryTimer(Timer, Hand);

	/* Link the timer list to our stack */
	ListHead.Flink = &Timer->TimerListEntry;
	ListHead.Blink = &Timer->TimerListEntry;
	Timer->TimerListEntry.Flink = &ListHead;
	Timer->TimerListEntry.Blink = &ListHead;

	/* Signal the timer if it's still on our list */
	if (!IsListEmpty(&ListHead)) {
		RequestInterrupt = KiSignalTimer(Timer);
	}

	/* Request a DPC if needed */
	if (RequestInterrupt) HalRequestSoftwareInterrupt(DISPATCH_LEVEL);
}

VOID xboxkrnl::KiRemoveEntryTimer(
	IN xboxkrnl::PKTIMER Timer,
	IN xboxkrnl::ULONG Hand
)
{
	ULONG Hand;
	PKTIMER_TABLE_ENTRY TableEntry;

	/* Remove the timer from the timer list and check if it's empty */
	if (RemoveEntryList(&Timer->TimerListEntry))
	{
		/* Get the respective timer table entry */
		TableEntry = &KiTimerTableListHead[Hand];
		if (&TableEntry->Entry == TableEntry->Entry.Flink)
		{
			/* Set the entry to an infinite absolute time */
			TableEntry->Time.u.HighPart = 0xFFFFFFFF;
		}
	}

	/* Clear the list entries so we can tell the timer is gone */
	Timer->TimerListEntry.Flink = NULL;
	Timer->TimerListEntry.Blink = NULL;
}

xboxkrnl::BOOLEAN FASTCALL xboxkrnl::KiInsertTimerTable(
	IN xboxkrnl::PKTIMER Timer,
	IN xboxkrnl::ULONG Hand
)
{
	LARGE_INTEGER InterruptTime;
	LONGLONG DueTime = Timer->DueTime.QuadPart;
	BOOLEAN Expired = FALSE;
	PLIST_ENTRY ListHead, NextEntry;
	PKTIMER CurrentTimer;

	DBG_PRINTF("%s: inserting Timer %p, Hand: %lu\n", __func__, Timer, Hand);

	/* Check if the period is zero */
	if (!Timer->Period) {
		Timer->Header.SignalState = FALSE;
	}

	/* Loop the timer list backwards */
	ListHead = &KiTimerTableListHead[Hand].Entry;
	NextEntry = ListHead->Blink;
	while (NextEntry != ListHead)
	{
		/* Get the timer */
		CurrentTimer = CONTAINING_RECORD(NextEntry, KTIMER, TimerListEntry);

		/* Now check if we can fit it before */
		if ((ULONGLONG)DueTime >= CurrentTimer->DueTime.QuadPart) break;

		/* Keep looping */
		NextEntry = NextEntry->Blink;
	}

	/* Looped all the list, insert it here and get the interrupt time again */
	InsertHeadList(NextEntry, &Timer->TimerListEntry);

	/* Check if we didn't find it in the list */
	if (NextEntry == ListHead)
	{
		/* Set the time */
		KiTimerTableListHead[Hand].Time.QuadPart = DueTime;

		/* Make sure it hasn't expired already */
		InterruptTime.QuadPart = KeQueryInterruptTime();
		if (DueTime <= InterruptTime.QuadPart) {
			Expired = TRUE;
		}
	}

	/* Return expired state */
	return Expired;
}

xboxkrnl::BOOLEAN FASTCALL xboxkrnl::KiInsertTreeTimer(
	IN xboxkrnl::PKTIMER Timer,
	IN xboxkrnl::LARGE_INTEGER Interval
)
{
	BOOLEAN Inserted = FALSE;
	ULONG Hand = 0;

	/* Setup the timer's due time */
	if (KiComputeDueTime(Timer, Interval, &Hand))
	{
		/* Insert the timer */
		if (KiInsertTimerTable(Timer, Hand))
		{
			/* It was already there, remove it */
			KiRemoveEntryTimer(Timer, Hand);
			Timer->Header.Inserted = FALSE;
		}
		else
		{
			/* Otherwise, we're now inserted */
			Inserted = TRUE;
		}
	}

	return Inserted;
}

xboxkrnl::ULONG xboxkrnl::KiComputeTimerTableIndex(
	IN xboxkrnl::ULONGLONG Interval
)
{
	return (Interval / CLOCK_TIME_INCREMENT) & (TIMER_TABLE_SIZE - 1);
}

xboxkrnl::BOOLEAN xboxkrnl::KiComputeDueTime(
	IN  xboxkrnl::PKTIMER Timer,
	IN  xboxkrnl::LARGE_INTEGER DueTime,
	OUT xboxkrnl::PULONG Hand)
{
	LARGE_INTEGER InterruptTime, SystemTime, DifferenceTime;

	/* Convert to relative time if needed */
	Timer->Header.Absolute = FALSE;
	if (DueTime.u.HighPart >= 0)
	{
		/* Get System Time */
		KeQuerySystemTime(&SystemTime);

		/* Do the conversion */
		DifferenceTime.QuadPart = SystemTime.QuadPart - DueTime.QuadPart;

		/* Make sure it hasn't already expired */
		Timer->Header.Absolute = TRUE;
		if (DifferenceTime.u.HighPart >= 0)
		{
			/* Cancel everything */
			DBG_PRINTF("Timer %p already expired\n", Timer);
			Timer->Header.SignalState = TRUE;
			Timer->DueTime.QuadPart = 0;
			*Hand = 0;
			return FALSE;
		}

		/* Set the time as Absolute */
		DueTime = DifferenceTime;
	}

	/* Get the Interrupt Time */
	InterruptTime.QuadPart = KeQueryInterruptTime();

	/* Recalculate due time */
	Timer->DueTime.QuadPart = InterruptTime.QuadPart - DueTime.QuadPart;

	/* Get the handle */
	*Hand = KiComputeTimerTableIndex(Timer->DueTime.QuadPart);
	Timer->Header.Inserted = TRUE;
	return TRUE;
}

xboxkrnl::BOOLEAN FASTCALL xboxkrnl::KiSignalTimer(
	IN xboxkrnl::PKTIMER Timer
)
{
	BOOLEAN RequestInterrupt = FALSE;
	PKDPC Dpc = Timer->Dpc;
	ULONG Period = Timer->Period;
	LARGE_INTEGER Interval, SystemTime;

	/* Set default values */
	Timer->Header.Inserted = FALSE;
	Timer->Header.SignalState = TRUE;

	/* Check if the timer has waiters */
	if (!IsListEmpty(&Timer->Header.WaitListHead))
	{
		/* Check the type of event */
		if (Timer->Header.Type == TimerNotificationObject)
		{
			/* Unwait the thread */
			//KxUnwaitThread(&Timer->Header, IO_NO_INCREMENT);
		}
		else
		{
			/* Otherwise unwait the thread and signal the timer */
			//KxUnwaitThreadForEvent((PKEVENT)Timer, IO_NO_INCREMENT);
		}
	}

	/* Check if we have a period */
	if (Period)
	{
		/* Calculate the interval and insert the timer */
		Interval.QuadPart = Int32x32To64(Period, -10000);
		while (!KiInsertTreeTimer(Timer, Interval));
	}

	/* Check if we have a DPC */
	if (Dpc)
	{
		/* Insert it in the queue */
		KeQuerySystemTime(&SystemTime);
		KeInsertQueueDpc(Dpc,
			ULongToPtr(SystemTime.u.LowPart),
			ULongToPtr(SystemTime.u.HighPart));
		RequestInterrupt = TRUE;
	}

	/* Return whether we need to request a DPC interrupt or not */
	return RequestInterrupt;
}

VOID xboxkrnl::KxRemoveTreeTimer(
	IN xboxkrnl::PKTIMER Timer
)
{
	ULONG Hand = KiComputeTimerTableIndex(Timer->DueTime.QuadPart);
	PKTIMER_TABLE_ENTRY TimerEntry;

	/* Set the timer as non-inserted */
	Timer->Header.Inserted = FALSE;

	/* Remove it from the timer list */
	if (RemoveEntryList(&Timer->TimerListEntry))
	{
		/* Get the entry and check if it's empty */
		TimerEntry = &KiTimerTableListHead[Hand];
		if (IsListEmpty(&TimerEntry->Entry))
		{
			/* Clear the time then */
			TimerEntry->Time.u.HighPart = 0xFFFFFFFF;
		}
	}
}
