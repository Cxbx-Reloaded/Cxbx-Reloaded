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
// *   Common->XboxAddressRanges.h
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
#pragma once

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

typedef struct { 
	const char *Name;
	unsigned __int32 Start;
	int Size;
	DWORD InitialMemoryProtection; // Memory page protection, for use by VirtualAlloc
	unsigned int RangeFlags;
} XboxAddressRange;

// Shortend symbol aliasses for memory page protection
#define PROT_UNH 0 // UNHANDLED
#define PROT_RW  PAGE_READWRITE
#define PROT_XRW PAGE_EXECUTE_READWRITE
#define PROT_NAC PAGE_NOACCESS

// Range flags (used for system selection and optional marker)
#define SYSTEM_XBOX    (1 << 0)
#define SYSTEM_DEVKIT  (1 << 1)
#define SYSTEM_CHIHIRO (1 << 2)
#define MAY_FAIL       (1 << 3) // Optional (may fail address range reservation)

// Short-hand for sets of system configurations
#define SYSTEM_ALL    (SYSTEM_XBOX | SYSTEM_DEVKIT | SYSTEM_CHIHIRO)
#define SYSTEM_RETAIL (SYSTEM_XBOX | SYSTEM_DEVKIT                 )
#define SYSTEM_128MB  (              SYSTEM_DEVKIT | SYSTEM_CHIHIRO)

const XboxAddressRange XboxAddressRanges[] = {
	// See http://xboxdevwiki.net/Memory
	// and http://xboxdevwiki.net/Boot_Process#Paging
	{ "MemLowVirtual", 0x00000000, MB( 64), PROT_XRW, SYSTEM_XBOX   | MAY_FAIL }, // - 0x07FFFFFF (Retail Xbox) Optional (already reserved via virtual_memory_placeholder)
	{ "MemLowVirtual", 0x00000000, MB(128), PROT_XRW, SYSTEM_128MB  | MAY_FAIL }, // - 0x07FFFFFF (Chihiro / DevKit)
	{ "MemPhysical",   0x80000000, MB( 64), PROT_XRW, SYSTEM_XBOX              }, // - 0x87FFFFFF (Retail)
	{ "MemPhysical",   0x80000000, MB(128), PROT_XRW, SYSTEM_128MB             }, // - 0x87FFFFFF (Chihiro / DevKit)
	{ "DevKitMemory",  0xB0000000, MB(128), PROT_NAC, SYSTEM_DEVKIT            }, // - 0xBFFFFFFF Optional (TODO : Check reserved range (might behave like MemTiled) + Flag [O]ptional?
	{ "MemPageTable",  0xC0000000, KB(128), PROT_RW,  SYSTEM_ALL               }, // - 0xC001FFFF TODO : MB(4)?
	{ "SystemMemory",  0xD0000000, MB(512), PROT_UNH, SYSTEM_ALL    | MAY_FAIL }, // - 0xEFFFFFFF Optional (TODO : Check reserved range (might behave like MemTiled)
	{ "MemTiled",      0xF0000000, MB( 64), PROT_UNH, SYSTEM_ALL    | MAY_FAIL }, // - 0xF3FFFFFF Optional (even though it can't be reserved, MapViewOfFileEx to this range still works!?)
	{ "DeviceNV2A_a",  0xFD000000, MB(  7), PROT_NAC, SYSTEM_ALL               }, // - 0xFD6FFFFF (GPU)
	{ "MemNV2APRAMIN", 0xFD700000, MB(  1), PROT_RW,  SYSTEM_ALL               }, // - 0xFD7FFFFF
	{ "DeviceNV2A_b",  0xFD800000, MB(  8), PROT_NAC, SYSTEM_ALL               }, // - 0xFDFFFFFF (GPU)
	{ "DeviceAPU",     0xFE800000, KB(512), PROT_NAC, SYSTEM_ALL               }, // - 0xFE87FFFF
	{ "DeviceAC97",    0xFEC00000, KB(  4), PROT_NAC, SYSTEM_ALL               }, // - 0xFEC00FFF (ACI)
	{ "DeviceUSB0",    0xFED00000, KB(  4), PROT_NAC, SYSTEM_ALL               }, // - 0xFED00FFF
	{ "DeviceUSB1",    0xFED08000, KB(  4), PROT_NAC, SYSTEM_ALL    | MAY_FAIL }, // - 0xFED08FFF Optional (won't be emulated for a long while?)
	{ "DeviceNVNet",   0xFEF00000, KB(  1), PROT_NAC, SYSTEM_ALL               }, // - 0xFEF003FF
	{ "DeviceFlash_a", 0xFF000000, MB(  4), PROT_NAC, SYSTEM_ALL               }, // - 0xFF3FFFFF (Flash mirror 1)
	{ "DeviceFlash_b", 0xFF400000, MB(  4), PROT_NAC, SYSTEM_ALL               }, // - 0xFF7FFFFF (Flash mirror 2)
	{ "DeviceFlash_c", 0xFF800000, MB(  4), PROT_NAC, SYSTEM_ALL               }, // - 0xFFBFFFFF (Flash mirror 3)
	{ "DeviceFlash_d", 0xFFC00000, MB(  4), PROT_NAC, SYSTEM_ALL    | MAY_FAIL }, // - 0xFFFFFFFF (Flash mirror 4) Optional (will probably fail reservation, which is acceptable - the 3 other mirrors work just fine
	{ "DeviceMCPX",    0xFFFFFE00,    512 , PROT_NAC, SYSTEM_RETAIL | MAY_FAIL }, // - 0xFFFFFFFF (not Chihiro, Xbox - if enabled) Optional (can safely be ignored)
};

extern bool ReserveAddressRanges(const int system);
extern bool VerifyAddressRanges(const int system);
extern void UnreserveMemoryRange(const int index);
extern bool AllocateMemoryRange(const int index);

extern LPTSTR GetLastErrorString();
extern void FreeLastErrorString(LPTSTR Error);
