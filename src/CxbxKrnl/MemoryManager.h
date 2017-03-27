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
// *   Cxbx->Win32->CxbxKrnl->MemoryManager.h
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
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *
// *  All rights reserved
// *
// ******************************************************************

#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <Windows.h>
#include <cstdint>
#include <map>
#include <unordered_map>

// Define virtual base and alternate virtual base of kernel.
#define KSEG0_BASE                  0x80000000
// Define virtual base addresses for physical memory windows.
#define MM_SYSTEM_PHYSICAL_MAP      KSEG0_BASE
#define MM_HIGHEST_PHYSICAL_PAGE    0x07FFF
#define MM_64M_PHYSICAL_PAGE        0x04000
#define MM_INSTANCE_PHYSICAL_PAGE   0x03FE0 // Chihiro arcade should use 0x07FF0
#define MM_INSTANCE_PAGE_COUNT      16
#define CONTIGUOUS_MEMORY_SIZE (64 * ONE_MB)

enum struct MemoryType {
	STANDARD = 0,
	ALIGNED,
	CONTIGUOUS
};

typedef struct {
	MemoryType type;
	uint32_t offset;
	size_t size;
} MemoryBlockInfo;

typedef struct {
	uint32_t offset;
	size_t size;
} ContiguousMemoryRegion;

class MemoryManager
{
public:
	MemoryManager();
	~MemoryManager();
	void* Allocate(size_t size);
	void* AllocateAligned(size_t size, size_t alignment);
	void* AllocateContiguous(size_t size, size_t alignment);
	void* AllocateZeroed(size_t num, size_t size);
	bool IsAllocated(void* block);
	void Free(void* block);
	size_t QueryAllocationSize(void* block);
private:
	std::unordered_map<uint32_t, MemoryBlockInfo> m_MemoryBlockInfo;
	std::map<uint32_t, ContiguousMemoryRegion> m_ContiguousMemoryRegions;
	CRITICAL_SECTION m_CriticalSection;
};

extern MemoryManager g_MemoryManager;

#endif