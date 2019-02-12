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
#pragma once

#include <cstdint> // For uint32_t

#ifndef CXBXR_EMU_EXPORTS // Only trim Windows symbols in cxbxr-ldr.exe, not in cxbxr-emu.dll (TODO : What about cxbxr.exe and cxbx.exe?)
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif // CXBXR_EMU_EXPORTS

#undef FIELD_OFFSET     // prevent macro redefinition warnings
#include <windows.h> // For DWORD, CALLBACK, VirtualAlloc, LPVOID, SIZE_T, HMODULE 

#define KB(x) ((x) *    1024 ) // = 0x00000400
#define MB(x) ((x) * KB(1024)) // = 0x00100000

// Windows' address space allocation granularity;
// See https://blogs.msdn.microsoft.com/oldnewthing/20031008-00/?p=42223
const int BLOCK_SIZE = KB(64);

// One allocation block consists of 16 pages (since PAGE_SIZE is 4 kilobytes)
const int PAGE_SIZE = KB(4);

#define ARRAY_SIZE(a)                               \
  ((sizeof(a) / sizeof(*(a))) /                     \
  static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))

const struct {
	uint32_t Start;
#ifdef DEBUG
	uint32_t End; // TODO : Add validation that this End corresponds to specified Size
#endif
	int Size;
	DWORD InitialMemoryProtection; // Memory page protection, for use by VirtualAlloc
	// Shortend symbol aliasses for memory page protection
	#define PROT_UNH 0 // UNHANDLED
	#define PROT_RW  PAGE_READWRITE
	#define PROT_XRW PAGE_EXECUTE_READWRITE
	#define PROT_NAC PAGE_NOACCESS

	int RangeFlags;
	// Range flags (used for system selection and optional marker)
	#define MAY_FAIL       (1 << 0) // Optional (may fail address range reservation)
	#define SYSTEM_XBOX    (1 << 1)
	#define SYSTEM_DEVKIT  (1 << 2)
	#define SYSTEM_CHIHIRO (1 << 3)
	// Short-hand for sets of system configurations
	#define SYSTEM_ALL     (SYSTEM_XBOX | SYSTEM_DEVKIT | SYSTEM_CHIHIRO)
	#define SYSTEM_RETAIL  (SYSTEM_XBOX | SYSTEM_DEVKIT                 )
	#define SYSTEM_128MB   (              SYSTEM_DEVKIT | SYSTEM_CHIHIRO)
#ifdef DEBUG
	const char *Comment;
#endif
} XboxAddressRanges[] = {
#ifdef DEBUG
	#define RANGE_ENTRY(START, END, SIZE, PROT, FLAGS, COMMENT) { START, END, SIZE, PROT, FLAGS, COMMENT }
#else
	#define RANGE_ENTRY(START, END, SIZE, PROT, FLAGS, COMMENT) { START, SIZE, PROT, FLAGS }
#endif
	// See http://xboxdevwiki.net/Memory
	// and http://xboxdevwiki.net/Boot_Process#Paging
	// Entry :  Start     , End       , Size   , Protect , RangeFlags              , Comment
	RANGE_ENTRY(0x00000000, 0x03FFFFFF, MB( 64), PROT_XRW, SYSTEM_XBOX   | MAY_FAIL, "MemLowVirtual (Retail Xbox) Optional (already reserved via virtual_memory_placeholder)"),
	RANGE_ENTRY(0x00000000, 0x07FFFFFF, MB(128), PROT_XRW, SYSTEM_128MB  | MAY_FAIL, "MemLowVirtual (Chihiro / DevKit)"),
	RANGE_ENTRY(0x80000000, 0x83FFFFFF, MB( 64), PROT_XRW, SYSTEM_XBOX             , "MemPhysical   (Retail)"),
	RANGE_ENTRY(0x80000000, 0x87FFFFFF, MB(128), PROT_XRW, SYSTEM_128MB            , "MemPhysical   (Chihiro / DevKit)"),
	RANGE_ENTRY(0xB0000000, 0xB7FFFFFF, MB(128), PROT_NAC, SYSTEM_DEVKIT           , "DevKitMemory"), // TODO : Check reserved range (might behave like MemTiled)
	RANGE_ENTRY(0xC0000000, 0xC03FFFFF, MB(  4), PROT_RW,  SYSTEM_ALL              , "MemPageTable"), // See PAGE_TABLES_SIZE, which contains one 4 byte entry per PAGE_SIZE
	RANGE_ENTRY(0xD0000000, 0xEFFFFFFF, MB(512), PROT_UNH, SYSTEM_ALL    | MAY_FAIL, "SystemMemory  Optional"), // TODO : Check reserved range (might behave like MemTiled)
	RANGE_ENTRY(0xF0000000, 0xF3FFFFFF, MB( 64), PROT_UNH, SYSTEM_ALL    | MAY_FAIL, "MemTiled      Optional (even though it can't be reserved, MapViewOfFileEx to this range still works!?)"),
	RANGE_ENTRY(0xFD000000, 0xFD6FFFFF, MB(  7), PROT_NAC, SYSTEM_ALL              , "DeviceNV2A_a  (GPU)"),
	RANGE_ENTRY(0xFD700000, 0xFD7FFFFF, MB(  1), PROT_RW,  SYSTEM_ALL              , "MemNV2APRAMIN"),
	RANGE_ENTRY(0xFD800000, 0xFDFFFFFF, MB(  8), PROT_NAC, SYSTEM_ALL              , "DeviceNV2A_b  (GPU)"),
	RANGE_ENTRY(0xFE800000, 0xFE87FFFF, KB(512), PROT_NAC, SYSTEM_ALL              , "DeviceAPU"),
	RANGE_ENTRY(0xFEC00000, 0xFEC00FFF, KB(  4), PROT_NAC, SYSTEM_ALL              , "DeviceAC97    (ACI)"),
	RANGE_ENTRY(0xFED00000, 0xFED00FFF, KB(  4), PROT_NAC, SYSTEM_ALL              , "DeviceUSB"),
	RANGE_ENTRY(0xFEF00000, 0xFEF003FF, KB(  1), PROT_NAC, SYSTEM_ALL              , "DeviceNVNet"),
	RANGE_ENTRY(0xFF000000, 0xFF3FFFFF, MB(  4), PROT_NAC, SYSTEM_ALL              , "DeviceFlash_a (Flash mirror 1)"),
	RANGE_ENTRY(0xFF400000, 0xFF7FFFFF, MB(  4), PROT_NAC, SYSTEM_ALL              , "DeviceFlash_b (Flash mirror 2)"),
	RANGE_ENTRY(0xFF800000, 0xFFBFFFFF, MB(  4), PROT_NAC, SYSTEM_ALL              , "DeviceFlash_c (Flash mirror 3)"),
	RANGE_ENTRY(0xFFC00000, 0xFFFFFFFF, MB(  4), PROT_NAC, SYSTEM_ALL    | MAY_FAIL, "DeviceFlash_d (Flash mirror 4) Optional (will probably fail reservation, which is acceptable - the 3 other mirrors work just fine"),
	/* This region is only relevant if we were running the original Xbox boot sequence (including MCPX),
	   so it's completely redundant to use it: By the time the Kernel has started execution, this region is disabled
	   and cannot be re-enabled. Xbox software (and the kernel) have no access to this region whatsoever once 2BL has completed.
	RANGE_ENTRY(0xFFFFFE00, 0xFFFFFFFF,    512 , PROT_NAC, SYSTEM_RETAIL | MAY_FAIL, "DeviceMCPX    (not Chihiro, Xbox - if enabled) Optional (can safely be ignored)"),
	*/
	#undef RANGE_ENTRY
};

extern bool AddressRangeMatchesFlags(const int index, const int flags);
extern bool IsOptionalAddressRange(const int index);

extern bool VerifyWow64();

extern LPTSTR GetLastErrorString();
extern void FreeLastErrorString(LPTSTR Error);
