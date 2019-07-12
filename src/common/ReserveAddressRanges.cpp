// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *  This file is part of Cxbx-Reloaded.
// *
// *  Cxbx-Reloaded is free software; you can redistribute it
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
// *  (c) 2019 ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

#include <cstdint> // For uint32_t

#include "AddressRanges.h"

// Reserve an address range up to the extend of what the host allows.
bool ReserveMemoryRange(int index, uint32_t blocks_reserved[384])
{
	uint32_t Start = XboxAddressRanges[index].Start;
	int Size = XboxAddressRanges[index].Size;
	bool HadAnyFailure = false;

	// Reserve this range in 64 Kb block increments, so that during emulation
	// our memory-management code can VirtualFree() each block individually :

	const DWORD Protect = XboxAddressRanges[index].InitialMemoryProtection;
	bool NeedsReservationTracking = false;
	switch (Start) {
		case 0x80000000:
		case 0xF0000000: {
			static bool NeedsInitialization = true;
			static	HANDLE hFileMapping;
			if (NeedsInitialization) {
				hFileMapping = CreateFileMapping(
					INVALID_HANDLE_VALUE,
					nullptr,
					PAGE_EXECUTE_READWRITE,
					0,
					Size,
					nullptr);
				if (hFileMapping == nullptr) {
					HadAnyFailure = true;
					break;
				}
				NeedsInitialization = false;
			}
			LPVOID Result = MapViewOfFileEx(
				hFileMapping,
				Start == 0x80000000 ?
				(FILE_MAP_READ | FILE_MAP_WRITE | FILE_MAP_EXECUTE) : (FILE_MAP_READ | FILE_MAP_WRITE),
				0,
				0,
				Size,
				(LPVOID)Start);
			if (Result == nullptr) {
				HadAnyFailure = true;
			}
		}
		break;

		case 0xB0000000:
		case 0xD0000000: {
			NeedsReservationTracking = true;
		}
		[[fallthrough]];

		default: {
			while (Size > 0) {
				static int arr_index = 0;
				SIZE_T BlockSize = (SIZE_T)(Size > BLOCK_SIZE) ? BLOCK_SIZE : Size;
				LPVOID Result = VirtualAlloc((LPVOID)Start, BlockSize, MEM_RESERVE, Protect);
				if (Result == nullptr) {
					HadAnyFailure = true;
				}
				// Handle the next block
				Start += BLOCK_SIZE;
				Size -= BLOCK_SIZE;
				if (NeedsReservationTracking) {
					if (Result != nullptr) {
						blocks_reserved[arr_index / 32] |= (1 << (arr_index % 32));
					}
					arr_index++;
				}
			}
		}
	}

	// Only a complete success when the entire request was reserved in a single range
	// (Otherwise, we have either a complete failure, or reserved it partially over multiple ranges)
	return !HadAnyFailure;
}

bool ReserveAddressRanges(const int system, uint32_t blocks_reserved[384]) {
	// Loop over all Xbox address ranges
	for (int i = 0; i < ARRAY_SIZE(XboxAddressRanges); i++) {
		// Skip address ranges that don't match the given flags
		if (!AddressRangeMatchesFlags(i, system))
			continue;

		// Try to reserve each address range
		if (ReserveMemoryRange(i, blocks_reserved))
			continue;

		// Some ranges are allowed to fail reserving
		if (!IsOptionalAddressRange(i)) {
			return false;
		}
	}

	return true;
}
