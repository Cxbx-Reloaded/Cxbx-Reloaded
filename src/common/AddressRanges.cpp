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
// *
// *  All rights reserved
// *
// ******************************************************************

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <Windows.h> // For PAGE_READWRITE, PAGE_EXECUTE_READWRITE, and PAGE_NOACCESS

#include "util/std_extend.hpp"
#include "AddressRanges.h"

// NOTE: Cannot be use in the header file.
// For InitialMemoryProtection field only. Memory page protection usage, for use by VirtualAlloc
// Shortend symbol aliasses for memory page protection
#define PROT_UNH 0 // UNHANDLED
#define PROT_RW  PAGE_READWRITE
#define PROT_XRW PAGE_EXECUTE_READWRITE
#define PROT_NAC PAGE_NOACCESS

const _XboxAddressRanges XboxAddressRanges[] = {
#ifdef DEBUG
	#define RANGE_ENTRY(START, END, SIZE, PROT, FLAGS, COMMENT) { START, END, SIZE, PROT, FLAGS, COMMENT }
#else
	#define RANGE_ENTRY(START, END, SIZE, PROT, FLAGS, COMMENT) { START, SIZE, PROT, FLAGS }
#endif
	// See http://xboxdevwiki.net/Memory
	// and http://xboxdevwiki.net/Boot_Process#Paging
	// Entry :  Start     , End       , Size            , Protect , RangeFlags              , Comment
	RANGE_ENTRY(0x00010000, 0x03FFFFFF, MB( 64) - KB(64), PROT_XRW, SYSTEM_ALL    | MAY_FAIL, "MemLowVirtual (General Xbox type) lower 64 MB Optional (already reserved via virtual_memory_placeholder)"),
	RANGE_ENTRY(0x04000000, 0x07FFFFFF, MB( 64)         , PROT_XRW, SYSTEM_128MB  | MAY_FAIL, "MemLowVirtual (Chihiro / DevKit)  ^ + upper 64 MB"),
	RANGE_ENTRY(0x80000000, 0x83FFFFFF, MB( 64)         , PROT_UNH, SYSTEM_ALL              , "MemPhysical   (General Xbox type) lower 64 MB"),
	RANGE_ENTRY(0x84000000, 0x87FFFFFF, MB( 64)         , PROT_UNH, SYSTEM_128MB            , "MemPhysical   (Chihiro / DevKit)  ^ + upper 64 MB"),
	RANGE_ENTRY(0xB0000000, 0xBFFFFFFF, MB(256)         , PROT_NAC, SYSTEM_DEVKIT           , "DevKitMemory"), // TODO : Check reserved range (might behave like MemTiled)
	RANGE_ENTRY(0xC0000000, 0xC03FFFFF, MB(  4)         , PROT_RW,  SYSTEM_ALL              , "MemPageTable"), // See PAGE_TABLES_SIZE, which contains one 4 byte entry per PAGE_SIZE
	RANGE_ENTRY(0xD0000000, 0xEFFFFFFF, MB(512)         , PROT_RW,  SYSTEM_ALL    | MAY_FAIL, "SystemMemory  Optional"), // TODO : Check reserved range (might behave like MemTiled)
	RANGE_ENTRY(0xF0000000, 0xF3FFFFFF, MB( 64)         , PROT_UNH, SYSTEM_ALL              , "MemTiled      Optional (even though it can't be reserved, MapViewOfFileEx to this range still works!?)"),
	RANGE_ENTRY(0xFD000000, 0xFD6FFFFF, MB(  7)         , PROT_NAC, SYSTEM_ALL              , "DeviceNV2A_a  (GPU)"),
	RANGE_ENTRY(0xFD700000, 0xFD7FFFFF, MB(  1)         , PROT_RW,  SYSTEM_ALL              , "MemNV2APRAMIN"),
	RANGE_ENTRY(0xFD800000, 0xFDFFFFFF, MB(  8)         , PROT_NAC, SYSTEM_ALL              , "DeviceNV2A_b  (GPU)"),
	RANGE_ENTRY(0xFE800000, 0xFE87FFFF, KB(512)         , PROT_NAC, SYSTEM_ALL              , "DeviceAPU"),
	RANGE_ENTRY(0xFEC00000, 0xFEC00FFF, KB(  4)         , PROT_NAC, SYSTEM_ALL              , "DeviceAC97    (ACI)"),
	RANGE_ENTRY(0xFED00000, 0xFED00FFF, KB(  4)         , PROT_NAC, SYSTEM_ALL              , "DeviceUSB"),
	RANGE_ENTRY(0xFEF00000, 0xFEF003FF, KB(  1)         , PROT_NAC, SYSTEM_ALL              , "DeviceNVNet"),
	RANGE_ENTRY(0xFF000000, 0xFF3FFFFF, MB(  4)         , PROT_NAC, SYSTEM_ALL              , "DeviceFlash_a (Flash mirror 1)"),
	RANGE_ENTRY(0xFF400000, 0xFF7FFFFF, MB(  4)         , PROT_NAC, SYSTEM_ALL              , "DeviceFlash_b (Flash mirror 2)"),
	RANGE_ENTRY(0xFF800000, 0xFFBFFFFF, MB(  4)         , PROT_NAC, SYSTEM_ALL              , "DeviceFlash_c (Flash mirror 3)"),
	RANGE_ENTRY(0xFFC00000, 0xFFFFFFFF, MB(  4)         , PROT_NAC, SYSTEM_ALL    | MAY_FAIL, "DeviceFlash_d (Flash mirror 4) Optional (will probably fail reservation, which is acceptable - the 3 other mirrors work just fine"),
	#undef RANGE_ENTRY
};

const size_t XboxAddressRanges_size = ARRAY_SIZE(XboxAddressRanges);

bool AddressRangeMatchesFlags(const int index, const unsigned int flags)
{
	return XboxAddressRanges[index].RangeFlags & flags;
}

bool IsOptionalAddressRange(const int index)
{
	return AddressRangeMatchesFlags(index, MAY_FAIL);
}

int AddressRangeGetSystemFlags(const int index)
{
	return XboxAddressRanges[index].RangeFlags & SYSTEM_ALL;
}

bool VerifyWow64()
{
	typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	BOOL bIsWow64 = FALSE;
	HMODULE hKernel32 = GetModuleHandle(TEXT("kernel32"));
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(hKernel32, "IsWow64Process");
	if (fnIsWow64Process != nullptr) {
		HANDLE hCurrentProcess = GetCurrentProcess();
		fnIsWow64Process(hCurrentProcess, &bIsWow64);
	}

	return (bIsWow64 != FALSE);
}
