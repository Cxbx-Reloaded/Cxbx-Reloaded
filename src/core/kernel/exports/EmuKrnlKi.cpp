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
#include "EmuKrnlLogging.h"
#include "EmuKrnlKi.h"

// ReactOS uses a size of 512, but disassembling the kernel reveals it to be 32 instead
#define TIMER_TABLE_SIZE 32

xboxkrnl::KTIMER_TABLE_ENTRY KiTimerTableListHead[TIMER_TABLE_SIZE];


xboxkrnl::BOOLEAN FASTCALL xboxkrnl::KiInsertTreeTimer(
	IN xboxkrnl::PKTIMER Timer,
	IN xboxkrnl::LARGE_INTEGER Interval
)
{
	BOOLEAN Inserted = FALSE;
	ULONG Hand = 0;

	/* This should only be called at dpc level */
	assert(KeGetCurrentIrql() == DISPATCH_LEVEL);

	/* Setup the timer's due time */
	if (KiComputeDueTime(Timer, Interval, &Hand))
	{
		/* Insert the timer */
		if (KiInsertTimerTable(Timer, Hand))
		{
			/* It was already there, remove it */
			KiRemoveEntryTimer(Timer);
			Timer->Header.Inserted = FALSE;
		}
		else
		{
			/* Otherwise, we're now inserted */
			Inserted = TRUE;
		}
	}

	/* Release the lock and return insert status */
	return Inserted;
}

xboxkrnl::ULONG xboxkrnl::KiComputeTimerTableIndex(
	IN xboxkrnl::ULONGLONG Interval
)
{
	return (Interval / KeMaximumIncrement) & (TIMER_TABLE_SIZE - 1);
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
