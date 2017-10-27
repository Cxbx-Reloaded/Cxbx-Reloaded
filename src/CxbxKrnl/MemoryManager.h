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

typedef struct {
	void *addr;
	void *upper_bound;
	void init(void * base, size_t size) { addr = base; upper_bound = (void *)((uintptr_t)base + size - 1); }
} MemoryBlock;

enum struct MemoryType {
	STANDARD = 0,
	ALIGNED,
	CONTIGUOUS
};

typedef struct {
	MemoryType type;
	MemoryBlock block;
} TypedMemoryBlock;

class MemoryManager
{
public:
	MemoryManager();
	~MemoryManager();
	void* Allocate(size_t size);
	void* AllocateAligned(size_t size, size_t alignment);
	void* AllocateContiguous(size_t size, size_t alignment);
	void* AllocateZeroed(size_t num, size_t size);
	bool IsAllocated(void* addr);
	void Free(void* addr);
	size_t QueryAllocationSize(void* addr);
private:
	std::map<void *, TypedMemoryBlock> m_MemoryBlockInfo;
	std::map<void *, MemoryBlock> m_ContiguousMemoryBlocks;
	CRITICAL_SECTION m_CriticalSection;
	TypedMemoryBlock *FindContainingTypedMemoryBlock(void* addr);
};

extern MemoryManager g_MemoryManager;

#endif