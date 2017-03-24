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
// *   Cxbx->Win32->CxbxKrnl->MemoryManager.cpp
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

#include "CxbxKrnl.h"
#include "Emu.h"
#include "Logging.h"
#include "MemoryManager.h"

MemoryManager g_MemoryManager;

MemoryManager::MemoryManager()
{
	InitializeCriticalSectionAndSpinCount(&m_CriticalSection, 0x400);
}

MemoryManager::~MemoryManager()
{
	DeleteCriticalSection(&m_CriticalSection);
}

void* MemoryManager::Allocate(size_t size)
{
	LOG_FUNC_ONE_ARG(size);

	EnterCriticalSection(&m_CriticalSection);

	void* ptr = malloc(size);

	if (ptr != nullptr) {
		MemoryBlockInfo info;
		info.offset = (uint32_t)ptr;
		info.size = size;
		info.type = MemoryType::STANDARD;

		m_MemoryBlockInfo[info.offset] = info;
	}

	LeaveCriticalSection(&m_CriticalSection);

	RETURN(ptr);
}

void* MemoryManager::AllocateAligned(size_t size, size_t alignment)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(size) ;
		LOG_FUNC_ARG(alignment);
	LOG_FUNC_END;

	EnterCriticalSection(&m_CriticalSection);

	void* ptr = _aligned_malloc(size, alignment);

	if (ptr != nullptr) {
		MemoryBlockInfo info;
		info.offset = (uint32_t)ptr;
		info.size = size;
		info.type = MemoryType::ALIGNED;

		m_MemoryBlockInfo[info.offset] = info;
	}

	LeaveCriticalSection(&m_CriticalSection);

	RETURN(ptr);
}

void* MemoryManager::AllocateContiguous(size_t size, size_t alignment)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(size);
		LOG_FUNC_ARG(alignment);
	LOG_FUNC_END;

	EnterCriticalSection(&m_CriticalSection);

	// If the end address of the block won't meet the alignment, adjust the size
	if (size % alignment > 0) {
		size = (size + alignment) + (size % alignment);
	}
	
	uint32_t addr = NULL;

	// If the allocation table is empty, we can allocate wherever we please
	if (m_ContiguousMemoryRegions.size() == 0) {
		// Start allocating Contiguous Memory after the Kernel image header to prevent overwriting our dummy Kernel
		addr = XBOX_KERNEL_BASE + sizeof(DUMMY_KERNEL);
	} else {
		// Locate the first available Memory Region with enough space for the requested buffer
		// This could be improved later on by always locating the smallest block with enough space
		// in order to reduce memory fragmentation.
		for (auto it = m_ContiguousMemoryRegions.begin(); it != m_ContiguousMemoryRegions.end(); ++it) {
			ContiguousMemoryRegion current = it->second;

			if (std::next(it) == m_ContiguousMemoryRegions.end()) {
				addr = current.offset + current.size;
				break;
			}

			ContiguousMemoryRegion next = std::next(it)->second;
			if (((current.offset + current.size + size) < next.offset)) {
				addr = current.offset + current.size;
				break;
			}
		}
	}
	
	if (addr + size > MM_SYSTEM_PHYSICAL_MAP + CONTIGUOUS_MEMORY_SIZE)  {
		EmuWarning("MemoryManager::AllocateContiguous exhausted it's allowed memory buffer");
		addr = NULL;	
	}
	 
	if (addr != NULL) {
		ContiguousMemoryRegion region;
		region.offset = addr;
		region.size = size;
		m_ContiguousMemoryRegions[addr] = region;

		MemoryBlockInfo info;
		info.type = MemoryType::CONTIGUOUS;
		info.offset = region.offset;
		info.size = region.size;
		m_MemoryBlockInfo[addr] = info;
	}
	
	LeaveCriticalSection(&m_CriticalSection);
	RETURN((void*)addr);
}

void* MemoryManager::AllocateZeroed(size_t num, size_t size)
{
	void* buffer = Allocate(num * size);
	memset(buffer, 0, num * size);
	return buffer;
}

bool MemoryManager::IsAllocated(void* block)
{
	LOG_FUNC_ONE_ARG(block);

	EnterCriticalSection(&m_CriticalSection);
	bool result = m_MemoryBlockInfo.find((uint32_t)block) != m_MemoryBlockInfo.end();
	LeaveCriticalSection(&m_CriticalSection);
	
	RETURN(result);
}

void MemoryManager::Free(void* block)
{
	LOG_FUNC_ONE_ARG(block);

	EnterCriticalSection(&m_CriticalSection);

	if (IsAllocated(block)) {
		MemoryBlockInfo info = m_MemoryBlockInfo[info.offset];
		switch (info.type) {
			case MemoryType::ALIGNED:
				_aligned_free((void*)info.offset);
				m_MemoryBlockInfo.erase(info.offset);
				break;
			case MemoryType::STANDARD:
				free((void*)info.offset);
				m_MemoryBlockInfo.erase(info.offset);
				break;
			case MemoryType::CONTIGUOUS:
				m_ContiguousMemoryRegions.erase(info.offset);
				m_MemoryBlockInfo.erase(info.offset);
				break;
			default:
				CxbxKrnlCleanup("Fatal: MemoryManager attempted to free memory of an unknown type");
				break;
		}
	} else {
		__debugbreak();
		CxbxKrnlCleanup("Fatal: Attempted to free memory that was not allocated via MemoryManager");
	}

	LeaveCriticalSection(&m_CriticalSection);
}

size_t MemoryManager::QueryAllocationSize(void* block)
{
	LOG_FUNC_ONE_ARG(block);

	EnterCriticalSection(&m_CriticalSection);

	size_t ret = 0;

	if (IsAllocated(block)) {
		MemoryBlockInfo info = m_MemoryBlockInfo[(uint32_t)block];
		ret = info.size;
	}
	else {
		EmuWarning("MemoryManager: Attempted to query memory that was not allocated via MemoryManager");
	}

	LeaveCriticalSection(&m_CriticalSection);
	RETURN(ret);
}