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
// *   Common->ReserveAddressRanges.cpp
// *
// *  This file is part of the Cxbx project.
// *
// *  Cxbx is free software; you can redistribute it
// *  and/or modify it under the terms of the GNU General Public
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
// *  (c) 2017-2019 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************

#include "AddressRanges.h"

// Reserve an address range up to the extend of what the host allows.
bool ReserveMemoryRange(int index)
{
	unsigned __int32 Start = XboxAddressRanges[index].Start;
	int Size = XboxAddressRanges[index].Size;
	bool HadAnyFailure = false;

	if (Start == 0) {
		// The zero page (the entire first 64 KB block) can't be reserved (if we would
		// try to reserve VirtualAlloc at address zero, it would hand us another address)
		Start += BLOCK_SIZE;
		Size -= BLOCK_SIZE;
		HadAnyFailure = true;
	}

	// Reserve this range in 64 Kb block increments, so that during emulation
	// our memory-management code can VirtualFree() each block individually :
	bool HadFailure = HadAnyFailure;
	const DWORD Protect = XboxAddressRanges[index].InitialMemoryProtection;
	while (Size > 0) {
		SIZE_T BlockSize = (SIZE_T)(Size > BLOCK_SIZE) ? BLOCK_SIZE : Size;
		LPVOID Result = VirtualAlloc((LPVOID)Start, BlockSize, MEM_RESERVE, Protect);
		if (Result == NULL) {
			HadFailure = true;
			HadAnyFailure = true;
		}

		// Handle the next block
		Start += BLOCK_SIZE;
		Size -= BLOCK_SIZE;
	}

	// Only a complete success when the entire request was reserved in a single range
	// (Otherwise, we have either a complete failure, or reserved it partially over multiple ranges)
	return !HadAnyFailure;
}

bool ReserveAddressRanges(const int system) {
	// Loop over all Xbox address ranges
	for (int i = 0; i < ARRAY_SIZE(XboxAddressRanges); i++) {
		// Skip address ranges that don't match the given flags
		if (!AddressRangeMatchesFlags(i, system))
			continue;

		// Try to reserve each address range
		if (ReserveMemoryRange(i))
			continue;

		// Some ranges are allowed to fail reserving
		if (!IsOptionalAddressRange(i)) {
			return false;
		}
	}

	return true;
}
