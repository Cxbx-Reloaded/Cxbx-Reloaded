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
	// Entry :  Start             , End              , Size              , Protect , RangeFlags              , Comment
	RANGE_ENTRY(USER_ADDRESS1_BASE, USER_ADDRESS1_END, USER_ADDRESS1_SIZE, PROT_XRW, SYSTEM_ALL    | MAY_FAIL, "MemLowVirtual (General Xbox type) lower 64 MiB Optional (already reserved via virtual_memory_placeholder)"),
	RANGE_ENTRY(USER_ADDRESS2_BASE, USER_ADDRESS2_END, USER_ADDRESS2_SIZE, PROT_XRW, SYSTEM_128MB  | MAY_FAIL, "MemLowVirtual (Chihiro / DevKit)  ^ + upper 64 MiB"),
	RANGE_ENTRY(PHYSICAL_MAP1_BASE, PHYSICAL_MAP1_END, PHYSICAL_MAP1_SIZE, PROT_UNH, SYSTEM_ALL              , "MemPhysical   (General Xbox type) lower 64 MiB"),
	RANGE_ENTRY(PHYSICAL_MAP2_BASE, PHYSICAL_MAP2_END, PHYSICAL_MAP2_SIZE, PROT_UNH, SYSTEM_128MB            , "MemPhysical   (Chihiro / DevKit)  ^ + upper 64 MiB"),
	RANGE_ENTRY(DEVKIT_MEMORY_BASE, DEVKIT_MEMORY_END, DEVKIT_MEMORY_SIZE, PROT_NAC, SYSTEM_DEVKIT           , "DevKitMemory"), // TODO : Check reserved range (might behave like MemTiled)
	RANGE_ENTRY(PAGE_TABLES_BASE  , PAGE_TABLES_END  , PAGE_TABLES_SIZE  , PROT_RW , SYSTEM_ALL              , "MemPageTable"), // See PAGE_TABLES_SIZE, which contains one 4 byte entry per PAGE_SIZE
	RANGE_ENTRY(SYSTEM_MEMORY_BASE, SYSTEM_MEMORY_END, SYSTEM_MEMORY_SIZE, PROT_RW , SYSTEM_ALL    | MAY_FAIL, "SystemMemory  Optional"), // TODO : Check reserved range (might behave like MemTiled)
	RANGE_ENTRY(TILED_MEMORY_BASE , TILED_MEMORY_END , TILED_MEMORY_SIZE , PROT_UNH, SYSTEM_ALL              , "MemTiled      Optional (even though it can't be reserved, MapViewOfFileEx to this range still works!?)"),
	RANGE_ENTRY(NV2A_DEVICE1_BASE , NV2A_DEVICE1_END , NV2A_DEVICE1_SIZE , PROT_NAC, SYSTEM_ALL              , "DeviceNV2A_a  (GPU)"),
	RANGE_ENTRY(NV2A_PRAMIN_BASE  , NV2A_PRAMIN_END  , NV2A_PRAMIN_SIZE1 , PROT_RW , SYSTEM_ALL              , "MemNV2APRAMIN"),
	RANGE_ENTRY(NV2A_DEVICE2_BASE , NV2A_DEVICE2_END , NV2A_DEVICE2_SIZE , PROT_NAC, SYSTEM_ALL              , "DeviceNV2A_b  (GPU)"),
	RANGE_ENTRY(APU_DEVICE_BASE   , APU_DEVICE_END   , APU_DEVICE_SIZE   , PROT_NAC, SYSTEM_ALL              , "DeviceAPU"),
	RANGE_ENTRY(AC97_DEVICE_BASE  , AC97_DEVICE_END  , AC97_DEVICE_SIZE  , PROT_NAC, SYSTEM_ALL              , "DeviceAC97    (ACI)"),
	RANGE_ENTRY(USB_DEVICE_BASE   , USB_DEVICE_END   , USB_DEVICE_SIZE   , PROT_NAC, SYSTEM_ALL              , "DeviceUSB"),
	RANGE_ENTRY(NVNET_DEVICE_BASE , NVNET_DEVICE_END , NVNET_DEVICE_SIZE , PROT_NAC, SYSTEM_ALL              , "DeviceNVNet"),
	RANGE_ENTRY(FLASH_DEVICE1_BASE, FLASH_DEVICE1_END, FLASH_DEVICEN_SIZE, PROT_NAC, SYSTEM_ALL              , "DeviceFlash_a (Flash mirror 1)"),
	RANGE_ENTRY(FLASH_DEVICE2_BASE, FLASH_DEVICE2_END, FLASH_DEVICEN_SIZE, PROT_NAC, SYSTEM_ALL              , "DeviceFlash_b (Flash mirror 2)"),
	RANGE_ENTRY(FLASH_DEVICE3_BASE, FLASH_DEVICE3_END, FLASH_DEVICEN_SIZE, PROT_NAC, SYSTEM_ALL    | MAY_FAIL, "DeviceFlash_c (Flash mirror 3)"), // this region fails reservation when running under Ubuntu with wine, so it must be allowed to fail
	RANGE_ENTRY(FLASH_DEVICE4_BASE, FLASH_DEVICE4_END, FLASH_DEVICEN_SIZE, PROT_NAC, SYSTEM_ALL    | MAY_FAIL, "DeviceFlash_d (Flash mirror 4) Optional (will probably fail reservation, which is acceptable - the 3 other mirrors work just fine"),
	#undef RANGE_ENTRY
};

const size_t XboxAddressRanges_size = ARRAY_SIZE(XboxAddressRanges);

bool AddressRangeMatchesFlags(const size_t index, const unsigned int flags)
{
	return XboxAddressRanges[index].RangeFlags & flags;
}

bool IsOptionalAddressRange(const size_t index)
{
	return AddressRangeMatchesFlags(index, MAY_FAIL);
}

unsigned int AddressRangeGetSystemFlags(const size_t index)
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
