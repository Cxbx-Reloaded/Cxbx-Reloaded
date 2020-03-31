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

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <Windows.h> // For DWORD, CALLBACK, VirtualAlloc, LPVOID, SIZE_T, HMODULE 

// NOTE: Cannot be use in loader project due to force exclude std libraries.
//#define DEBUG // Uncomment whenever need to verify memory leaks or bad configure.

#ifdef DEBUG
#include <cstdio> // For printf
#endif
#include <cstdint> // For uint32_t

#include "util/std_extend.hpp"
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
	unsigned int arr_index = BLOCK_REGION_DEVKIT_INDEX_BEGIN;
#ifdef DEBUG
	std::printf("DEBUG: ReserveMemoryRange call begin\n");
	std::printf("     : Comment = %s\n", XboxAddressRanges[index].Comment);
#endif
	switch (Start) {
		case PHYSICAL_MAP1_BASE:
		case PHYSICAL_MAP2_BASE:
		case TILED_MEMORY_BASE: {
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
				(Start == PHYSICAL_MAP1_BASE || Start == PHYSICAL_MAP2_BASE) ?
				(FILE_MAP_READ | FILE_MAP_WRITE | FILE_MAP_EXECUTE) : (FILE_MAP_READ | FILE_MAP_WRITE),
				0,
				0,
				Size,
				(LPVOID)Start);
#ifdef DEBUG
			std::printf("     : MapViewOfFile; Start = 0x%08X; Result = %p\n", Start, Result);
#endif
			if (Result == nullptr) {
				HadAnyFailure = true;
			}
		}
		break;

		case SYSTEM_MEMORY_BASE:
			// If additional addresses need to be assign in region's block.
			// Then check for nonzero value.
			arr_index = BLOCK_REGION_SYSTEM_INDEX_BEGIN;
		[[fallthrough]];
		case DEVKIT_MEMORY_BASE: {
			// arr_index's default is BLOCK_REGION_DEVKIT_INDEX_BEGIN which is zero.
			// Any block region above zero should be place above this case to override zero value.
			//arr_index = BLOCK_REGION_DEVKIT_INDEX_BEGIN;
			NeedsReservationTracking = true;
		}
		[[fallthrough]];

		default: {
			while (Size > 0) {
				SIZE_T BlockSize = (SIZE_T)(Size > BLOCK_SIZE) ? BLOCK_SIZE : Size;
				LPVOID Result = VirtualAlloc((LPVOID)Start, BlockSize, MEM_RESERVE, Protect);
				if (Result == nullptr) {
					HadAnyFailure = true;
				}
#ifdef DEBUG
				std::printf("     : Start = %08X; Result = %p;\n", Start, Result);
#endif
				// Handle the next block
				Start += BLOCK_SIZE;
				Size -= BLOCK_SIZE;
				if (NeedsReservationTracking) {
					if (Result != nullptr) {
						blocks_reserved[arr_index / 32] |= (1 << (arr_index % 32));
#ifdef DEBUG
						std::printf("     : arr_index = 0x%08X; set bit = 0x%08X;\n", arr_index, (1 << (arr_index % 32)));
						std::printf("     : blocks_reserved[%08X] = 0x%08X\n", arr_index/32, blocks_reserved[arr_index/32]);
#endif
					}
					arr_index++;
				}
			}
		}
	}
#ifdef DEBUG
	std::printf("     : ReserveMemoryRange call end: HadAnyFailure = %d\n\n", HadAnyFailure);
#endif

	// Only a complete success when the entire request was reserved in a single range
	// (Otherwise, we have either a complete failure, or reserved it partially over multiple ranges)
	return !HadAnyFailure;
}

// Free address range from the host.
void FreeMemoryRange(int index, uint32_t blocks_reserved[384])
{
	uint32_t Start = XboxAddressRanges[index].Start, _Start;
	int Size = XboxAddressRanges[index].Size;
	bool NeedsReservationTracking = false;
	unsigned int arr_index = BLOCK_REGION_DEVKIT_INDEX_BEGIN;
#ifdef DEBUG
	std::printf("DEBUG: FreeMemoryRange call begin\n");
	std::printf("     : Comment = %s\n", XboxAddressRanges[index].Comment);
#endif
	switch (Start) {
		case PHYSICAL_MAP1_BASE:
		case PHYSICAL_MAP2_BASE:
		case TILED_MEMORY_BASE: {
			(void)UnmapViewOfFile((LPVOID)Start);
#ifdef DEBUG
			std::printf("     : UnmapViewOfFile; Start = 0x%08X\n", Start);
#endif
		}
		break;

		case SYSTEM_MEMORY_BASE:
			// If additional addresses need to be assign in region's block.
			// Then check for nonzero value.
			arr_index = BLOCK_REGION_SYSTEM_INDEX_BEGIN;
		[[fallthrough]];
		case DEVKIT_MEMORY_BASE: {
			// arr_index's default is BLOCK_REGION_DEVKIT_INDEX_BEGIN which is zero.
			// Any block region above zero should be place above this case to override zero value.
			//arr_index = BLOCK_REGION_DEVKIT_INDEX_BEGIN;
			NeedsReservationTracking = true;
		}
		[[fallthrough]];

		default: {
			while (Size > 0) {
				_Start = Start; // Require to silence C6001's warning complaint
				BOOL Result = VirtualFree((LPVOID)_Start, 0, MEM_RELEASE);
#ifdef DEBUG
				std::printf("     : Start = %08X; Result = %d;\n", Start, Result);
#endif
				// Handle the next block
				Start += BLOCK_SIZE;
				Size -= BLOCK_SIZE;
				if (NeedsReservationTracking) {
					if (Result != 0) {
						blocks_reserved[arr_index / 32] &= ~(1 << (arr_index % 32));
#ifdef DEBUG
						std::printf("     : arr_index = 0x%08X; clear bit = 0x%08X;\n", arr_index, (1 << (arr_index % 32)));
						std::printf("     : blocks_reserved[%08X] = 0x%08X\n", arr_index/32, blocks_reserved[arr_index/32]);
#endif
					}
					arr_index++;
				}
			}
		}
	}
#ifdef DEBUG
	std::printf("     : FreeMemoryRange call end\n\n");
#endif
}

bool ReserveAddressRanges(const unsigned int system, uint32_t blocks_reserved[384]) {
	// Loop over all Xbox address ranges
	for (size_t i = 0; i < XboxAddressRanges_size; i++) {
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

void FreeAddressRanges(const unsigned int system, unsigned int release_systems, uint32_t blocks_reserved[384]) {
	// If reserved_systems is empty, then there's nothing to be freed up.
	if (release_systems == 0) {
		return;
	}
	// Loop over all Xbox address ranges
	for (size_t i = 0; i < XboxAddressRanges_size; i++) {
		// Skip address ranges that do match specific flag
		if (AddressRangeMatchesFlags(i, system))
			continue;

		// Skip address ranges that doesn't match the reserved flags
		if (!AddressRangeMatchesFlags(i, release_systems))
			continue;

		FreeMemoryRange(i, blocks_reserved);
	}

}

bool AttemptReserveAddressRanges(unsigned int* p_reserved_systems, uint32_t blocks_reserved[384]) {

	size_t iLast = 0;
	unsigned int reserved_systems = *p_reserved_systems, clear_systems = 0;
	// Loop over all Xbox address ranges
	for (size_t i = 0; i < XboxAddressRanges_size; i++) {

		// Once back to original spot, let's resume.
		if (i == iLast && clear_systems) {
			reserved_systems &= ~clear_systems;
			if (reserved_systems == 0) {
				*p_reserved_systems = 0;
				return false;
			}
			// Resume virtual allocated range.
			clear_systems = 0;
			continue;
		}

		if (clear_systems) {
			// Skip address ranges that doesn't match the given flags
			if (!AddressRangeMatchesFlags(i, clear_systems))
				continue;

			// Release incompatible system's memory range
			FreeMemoryRange(i, blocks_reserved);
		}
		else {
			// Skip address ranges that don't match the given flags
			if (!AddressRangeMatchesFlags(i, reserved_systems))
				continue;

			// Try to reserve each address range
			if (ReserveMemoryRange(i, blocks_reserved))
				continue;

			// Some ranges are allowed to fail reserving
			if (!IsOptionalAddressRange(i)) {
				// If not, then let's free them and downgrade host's limitation.
				iLast = i;
				i = -1; // Reset index back to zero after for statement's increment.
				clear_systems = AddressRangeGetSystemFlags(i);
				continue;
			}
		}
	}

	*p_reserved_systems = reserved_systems;
	return true;
}

bool isSystemFlagSupport(unsigned int reserved_systems, unsigned int assign_system)
{
	if (reserved_systems & assign_system) {
		return true;
	}
// TODO: Once host's standalone emulation is remove from GUI, remove below as well.
#ifndef CXBXR_EMU
	if (reserved_systems == 0) {
		return true;
	}
#endif

	return false;
}
