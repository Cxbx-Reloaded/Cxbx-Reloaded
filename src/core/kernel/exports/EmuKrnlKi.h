// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->src->CxbxKrnl->EmuKrnlKi.h
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

namespace xboxkrnl
{
#define KiLockDispatcherDatabase(OldIrql)      \
	*(OldIrql) = KeRaiseIrqlToDpcLevel()

#define KiLockApcQueue(Thread, OldIrql)        \
    *(OldIrql) = KeRaiseIrqlToSynchLevel()

#define KiUnlockApcQueue(Thread, OldIrql)      \
	KfLowerIrql((OldIrql))

#define KiRemoveTreeTimer(Timer)               \
    (Timer)->Header.Inserted = FALSE;          \
    RemoveEntryList(&(Timer)->TimerListEntry)

	typedef struct _KTIMER_TABLE_ENTRY
	{
		LIST_ENTRY Entry;
		ULARGE_INTEGER Time;
	} KTIMER_TABLE_ENTRY, *PKTIMER_TABLE_ENTRY;

	BOOLEAN FASTCALL KiInsertTreeTimer(
		IN PKTIMER Timer,
		IN LARGE_INTEGER Interval
	);

	BOOLEAN KiComputeDueTime(
		IN PKTIMER Timer,
		IN LARGE_INTEGER DueTime,
		OUT PULONG Hand
	);
};
