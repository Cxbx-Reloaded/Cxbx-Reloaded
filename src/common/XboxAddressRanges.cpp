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
// *   Common->XboxAddressRanges.cpp
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
// *  (c) 2017 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************

#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h> // For DWORD, CALLBACK, VirtualAlloc, LPVOID, SIZE_T, HMODULE 

#include "XboxAddressRanges.h"

// Maps an XboxAddressRangeType to page protection flags, for use by VirtualAlloc
DWORD XboxAddressRangeTypeToVirtualAllocPageProtectionFlags(int type)
{
	if (XboxAddressRanges[type].Flags & RangeFlags::E)
		return PAGE_EXECUTE_READWRITE;

	if (XboxAddressRanges[type].Flags & RangeFlags::R)
		return PAGE_READWRITE;

	if (XboxAddressRanges[type].Flags & RangeFlags::N)
		return PAGE_NOACCESS;

	return 0; // UNHANDLED
}

typedef struct {
	int Index;
	unsigned __int32 Start;
	int Size;
} ReservedRange;

// This array keeps track of which ranges have successfully been reserved.
// Having this helps debugging, but isn't strictly necessary, as we could
// retrieve the same information using VirtualQuery.
ReservedRange ReservedRanges[128];
int ReservedRangeCount = 0;

// Reserve an address range up to the extend of what the host allows.
bool ReserveMemoryRange(int xart)
{
	//	assert(XboxAddressRanges[xart].Type == xart);
	unsigned __int32 Start = XboxAddressRanges[xart].Start;
	int Size = XboxAddressRanges[xart].Size;

	bool HadAnyFailure = false;
	if (Start == 0) {
		// The zero page (the entire first 64 KB block) can't be reserved (if we would
		// try to reserve VirtualAlloc at address zero, it would hand us another address)
		Start += BLOCK_SIZE;
		Size -= BLOCK_SIZE;
		HadAnyFailure = true;
	}

	// Safeguard against bounds overflow
	if (ReservedRangeCount < ARRAY_SIZE(ReservedRanges)) {
		// Initialize the reservation of a new range
		ReservedRanges[ReservedRangeCount].Index = xart;
		ReservedRanges[ReservedRangeCount].Start = Start;
		ReservedRanges[ReservedRangeCount].Size = 0;
	}

	// Reserve this range in 64 Kb block increments, so later on our
	// memory-management code can VirtualFree() each block individually :
	bool HadFailure = HadAnyFailure;
	const DWORD Protect = XboxAddressRangeTypeToVirtualAllocPageProtectionFlags(xart);
	while (Size > 0) {
		SIZE_T BlockSize = (SIZE_T)(Size > BLOCK_SIZE) ? BLOCK_SIZE : Size;
		LPVOID Result = VirtualAlloc((LPVOID)Start, BlockSize, MEM_RESERVE, Protect);
		if (Result == NULL) {
			HadFailure = true;
			HadAnyFailure = true;
		}
		else {
			// Safeguard against bounds overflow
			if (ReservedRangeCount < ARRAY_SIZE(ReservedRanges)) {
				if (HadFailure) {
					HadFailure = false;
					// Starting a new range - did the previous one have any size?
					if (ReservedRanges[ReservedRangeCount].Size > 0) {
						// Then start a new range, and copy over the current type
						ReservedRangeCount++;
						ReservedRanges[ReservedRangeCount].Index = xart;
					}

					// Register a new ranges starting address
					ReservedRanges[ReservedRangeCount].Start = Start;
				}

				// Accumulate the size of each successfull reservation
				ReservedRanges[ReservedRangeCount].Size += BlockSize;
			}
		}

		// Handle the next block
		Start += BLOCK_SIZE;
		Size -= BLOCK_SIZE;
	}

	// Safeguard against bounds overflow
	if (ReservedRangeCount < ARRAY_SIZE(ReservedRanges)) {
		// Keep the current block only if it contains a successfully reserved range
		if (ReservedRanges[ReservedRangeCount].Size > 0) {
			ReservedRangeCount++;
		}
	}

	// Only a complete success when the entire request was reserved in a single range
	// (Otherwise, we have either a complete failure, or reserved it partially over multiple ranges)
	return !HadAnyFailure;
}

bool AddressRangeMatchesFlags(const int xart, const int flags)
{
	return XboxAddressRanges[xart].Flags & flags;
}

bool IsOptionalAddressRange(const int xart)
{
	return AddressRangeMatchesFlags(xart, RangeFlags::O);
}

bool ReserveAddressRanges(const int flags) {
	// Loop over all Xbox address ranges
	for (int i = 0; i < ARRAY_SIZE(XboxAddressRanges); i++) {
		// Skip address ranges that don't match the given flags
		if (!AddressRangeMatchesFlags(i, flags))
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

bool VerifyAddressRange(int xart)
{
	const int PAGE_SIZE = KB(4);

	unsigned __int32 BaseAddress = XboxAddressRanges[xart].Start;
	int Size = XboxAddressRanges[xart].Size;

	if (BaseAddress == 0) {
		// The zero page (the entire first 64 KB block) can't be verified
		BaseAddress += BLOCK_SIZE;
		Size -= BLOCK_SIZE;
	}

	// Verify this range in 64 Kb block increments, as they are supposed
	// to have been reserved like that too:
	const DWORD AllocationProtect = (XboxAddressRanges[xart].Start == 0) ? PAGE_EXECUTE_WRITECOPY : XboxAddressRangeTypeToVirtualAllocPageProtectionFlags(xart);
	MEMORY_BASIC_INFORMATION mbi;
	while (Size > 0) {
		// Expected values
		PVOID AllocationBase = (PVOID)BaseAddress;
		SIZE_T RegionSize = (SIZE_T)(Size > BLOCK_SIZE) ? BLOCK_SIZE : (Size > PAGE_SIZE) ? Size : PAGE_SIZE;
		DWORD State = MEM_RESERVE;
		DWORD Protect = 0;
		DWORD Type = MEM_PRIVATE;

		// Allowed deviations
		if (XboxAddressRanges[xart].Start == 0) {
			AllocationBase = (PVOID)0x10000;
			switch (BaseAddress) {
				case 0x10000: // Image Header
					RegionSize = PAGE_SIZE;
					Protect = PAGE_READONLY;
					break;
				case 0x11000: // section .textbss
					RegionSize = BLOCK_SIZE;
					Protect = PAGE_EXECUTE_READWRITE;
					break;
				case 0x21000: // section .text
					RegionSize = 0x2000;
					Protect = PAGE_EXECUTE_READ;
					break;
				case 0x23000: // section .data 
					RegionSize = PAGE_SIZE;
					Protect = PAGE_READONLY;
					break;
				case 0x24000: // section .idata
					RegionSize = 0x0999b000; // == ? - ?
					Protect = PAGE_READWRITE;
					break;
				}

			State = MEM_COMMIT;
			Type = MEM_IMAGE;
			break;
		}

		// Verify each block
		bool Okay = (VirtualQuery((LPVOID)BaseAddress, &mbi, sizeof(mbi)) != 0);
		if (Okay) 
			Okay = (mbi.BaseAddress == (LPVOID)BaseAddress);
		if (Okay) 
			Okay = (mbi.AllocationBase == AllocationBase);
		if (Okay) 
			Okay = (mbi.AllocationProtect == AllocationProtect);
		if (Okay) 
			Okay = (mbi.RegionSize == RegionSize);
		if (Okay) 
			Okay = (mbi.State == State);
		if (Okay) 
			Okay = (mbi.Protect == Protect);
		if (Okay) 
			Okay = (mbi.Type == Type);

		if (!Okay)
			if (!IsOptionalAddressRange(xart))
				return false;

		// Handle the next region
		BaseAddress += RegionSize;
		Size -= RegionSize;
	}

	return true;
}

bool VerifyAddressRanges()
{
	// Loop over all Xbox address ranges
	for (int i = 0; i < ARRAY_SIZE(XboxAddressRanges); i++) {
		if (!VerifyAddressRange(i)) {
			if (!IsOptionalAddressRange(i)) {
				return false;
			}
		}
	}

	return true;
}

void UnreserveMemoryRange(int xart)
{
	unsigned __int32 Start = XboxAddressRanges[xart].Start;
	int Size = XboxAddressRanges[xart].Size;

	while (Size > 0) {
		// To release, dwSize must be zero!
		VirtualFree((LPVOID)Start, (SIZE_T)0, MEM_RELEASE);
		Start += BLOCK_SIZE;
		Size -= BLOCK_SIZE;
	}
}

bool AllocateMemoryRange(int xart)
{
	unsigned __int32 Start = XboxAddressRanges[xart].Start;
	int Size = XboxAddressRanges[xart].Size;

	while (Size > 0) {
		int BlockSize = (Size > BLOCK_SIZE) ? BLOCK_SIZE : Size;
		if (nullptr == VirtualAlloc((void*)Start, BlockSize, MEM_COMMIT, PAGE_READWRITE)) { // MEM_RESERVE already done by Cxbx-Loader.exe
			return false;
		}

		Start += BLOCK_SIZE;
		Size -= BLOCK_SIZE;
	}

	return true;
}
