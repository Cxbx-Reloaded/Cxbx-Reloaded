// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->src->CxbxKrnl->EmuKrnlKe.h
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

// Forward KeLowerIrql() to KfLowerIrql()
#define KeLowerIrql(NewIrql) \
	KfLowerIrql(NewIrql)

// Forward KeRaiseIrql() to KfRaiseIrql()
#define KeRaiseIrql(NewIrql, OldIrql) \
	*(OldIrql) = KfRaiseIrql(NewIrql)

enum class GetMode { Existing, ExistingOrNew, Erase };

HANDLE GetHostEvent(xboxkrnl::PRKEVENT Event, GetMode getMode = GetMode::ExistingOrNew);
HANDLE GetNtMutant(xboxkrnl::PRKMUTANT pMutant, GetMode getMode, BOOLEAN InitialOwner = FALSE);

void KeClearEvent(IN xboxkrnl::PRKEVENT Event);

void KeQueryMutant(xboxkrnl::PRKMUTANT pMutant, xboxkrnl::PMUTANT_BASIC_INFORMATION pMutantInformation);

namespace XboxTypes = xboxkrnl;

void KeInitializeThread
(
	XboxTypes::PKTHREAD Thread,
	XboxTypes::PVOID KernelStack,
	XboxTypes::SIZE_T KernelStackSize,
	XboxTypes::SIZE_T TlsDataSize,
	XboxTypes::PKSYSTEM_ROUTINE SystemRoutine,
	XboxTypes::PKSTART_ROUTINE StartRoutine,
	XboxTypes::PVOID StartContext,
	XboxTypes::PKPROCESS Process
);
