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

#include <assert.h> // For assert()
#include <malloc.h> // For _aligned_malloc()

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

TypedMemoryBlock *MemoryManager::FindContainingTypedMemoryBlock(void* addr)
{
	if (m_MemoryBlockInfo.empty())
		return nullptr;

	// Find the first block whose address is not less than the requested address
	auto low = m_MemoryBlockInfo.lower_bound(addr);
	if (low == m_MemoryBlockInfo.end())
		// If there's no such block, it might fall into the last block
		low = --m_MemoryBlockInfo.end();

	// Check if the requested address lies inside this block
	TypedMemoryBlock *info = &(low->second);
	if ((size_t)addr >= ((size_t)info->block.addr + info->block.size))
		return nullptr;

	return info;
}

void* MemoryManager::Allocate(size_t size)
{
	LOG_FUNC_ONE_ARG(size);

	assert(size > 0);

	void * addr = malloc(size);

	if (addr != NULL) {
		TypedMemoryBlock info;

		info.type = MemoryType::STANDARD;
		info.block.addr = addr;
		info.block.size = size;

		EnterCriticalSection(&m_CriticalSection);
		m_MemoryBlockInfo[addr] = info;
		LeaveCriticalSection(&m_CriticalSection);
	} else	{
#if 1 // TODO : Only log this in DEBUG builds (as a failure is already indicated by a null result)
		EmuWarning("MemoryManager::Allocate Failed!");
#endif
	}

	RETURN((void*)addr);
}

void* MemoryManager::AllocateAligned(size_t size, size_t alignment)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(size) ;
		LOG_FUNC_ARG(alignment);
	LOG_FUNC_END;

	assert(size > 0);

	void * addr = _aligned_malloc(size, alignment);

	if (addr != NULL) {
		TypedMemoryBlock info;

		info.type = MemoryType::ALIGNED;
		info.block.addr = addr;
		info.block.size = size;

		EnterCriticalSection(&m_CriticalSection);
		m_MemoryBlockInfo[addr] = info;
		LeaveCriticalSection(&m_CriticalSection);
	}

	RETURN((void*)addr);
}

void* MemoryManager::AllocateContiguous(size_t size, size_t alignment)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(size);
		LOG_FUNC_ARG(alignment);
	LOG_FUNC_END;

	assert(size > 0);
	assert(alignment >= 4096); // TODO : Pull PAGE_SIZE in scope for this?
	assert((alignment & (alignment - 1)) == 0);

	xbaddr alignMask = (xbaddr)(alignment - 1);

	xbaddr addr = NULL;

	EnterCriticalSection(&m_CriticalSection);
	// Is the contiguous allocation table empty?
	if (m_ContiguousMemoryBlocks.size() == 0) {
		// Start allocating Contiguous Memory after the Kernel image header to prevent overwriting our dummy Kernel
		addr = XBOX_KERNEL_BASE + sizeof(DUMMY_KERNEL);
		addr = (addr + alignMask) & ~alignMask;
	} else {
		// Locate the first available Memory Region with enough space for the requested buffer
		// TODO : This could be improved later on by always locating the smallest block with enough space
		// in order to reduce memory fragmentation.
		for (auto it = m_ContiguousMemoryBlocks.begin(); it != m_ContiguousMemoryBlocks.end(); ++it) {
			MemoryBlock current = it->second;
			xbaddr after_current = (xbaddr)current.addr + current.size;
			after_current = (after_current + alignMask) & ~alignMask;

			// Is this the last entry?
			if (std::next(it) == m_ContiguousMemoryBlocks.end()) {
				// Continue allocating after the last block :
				addr = after_current;
				break;
			}

			// Is there an empty gap after the current entry?
			MemoryBlock next = std::next(it)->second;
			if (after_current < (xbaddr)next.addr) {
				// does this allocation fit inside the gap?
				if (after_current + size < (xbaddr)next.addr) {
					addr = after_current;
					break;
				}
			}
		}
	}
	
	if (addr + size > MM_SYSTEM_PHYSICAL_MAP + CONTIGUOUS_MEMORY_SIZE)  {
		EmuWarning("MemoryManager::AllocateContiguous exhausted it's allowed memory buffer");
		addr = NULL;	
	}
	 
	if (addr != NULL) {
		MemoryBlock block;

		block.addr = (void *)addr;
		block.size = size;
		m_ContiguousMemoryBlocks[(void *)addr] = block;

		TypedMemoryBlock info;

		info.type = MemoryType::CONTIGUOUS;
		info.block = block;

		m_MemoryBlockInfo[(void *)addr] = info;
	}
	
	LeaveCriticalSection(&m_CriticalSection);

	RETURN((void*)addr);
}

void* MemoryManager::AllocateZeroed(size_t num, size_t size)
{
	LOG_FORWARD(Allocate); // Log AllocateZeroed as the origin of the following RETURN log message

	void* addr = Allocate(num * size);
	
	if (addr != nullptr)  {
		memset(addr, 0, num * size);	
	}
		
	return addr; // Dont use RETURN, as Allocate already logs the result with that
}

bool MemoryManager::IsAllocated(void* addr)
{
	bool result;

	LOG_FUNC_ONE_ARG(addr);

	EnterCriticalSection(&m_CriticalSection);
	result = (FindContainingTypedMemoryBlock(addr) != nullptr);
	LeaveCriticalSection(&m_CriticalSection);
	
	RETURN(result);
}

void MemoryManager::Free(void* addr)
{
	LOG_FUNC_ONE_ARG(addr);

	EnterCriticalSection(&m_CriticalSection);

	TypedMemoryBlock *info = FindContainingTypedMemoryBlock(addr);
	if (info != nullptr) {
		if (info->block.addr == addr) {
			switch (info->type) {
			case MemoryType::ALIGNED:
				_aligned_free((void*)info->block.addr);
				m_MemoryBlockInfo.erase(info->block.addr);
				break;
			case MemoryType::STANDARD:
				free((void*)info->block.addr);
				m_MemoryBlockInfo.erase(info->block.addr);
				break;
			case MemoryType::CONTIGUOUS:
				m_ContiguousMemoryBlocks.erase(info->block.addr);
				m_MemoryBlockInfo.erase(info->block.addr);
				break;
			default:
				CxbxKrnlCleanup("Fatal: MemoryManager attempted to free memory of an unknown type");
				break;
			}
		}
		else {
			__debugbreak();
			CxbxKrnlCleanup("Fatal: MemoryManager attempted to free only a part of a block");
		}
	} else {
		__debugbreak();
		CxbxKrnlCleanup("Fatal: Attempted to free memory that was not allocated via MemoryManager");
	}

	LeaveCriticalSection(&m_CriticalSection);
}

size_t MemoryManager::QueryAllocationSize(void* addr)
{
	LOG_FUNC_ONE_ARG(addr);

	size_t ret = 0;

	EnterCriticalSection(&m_CriticalSection);
	TypedMemoryBlock *info = FindContainingTypedMemoryBlock(addr);
	if (info != nullptr) {
		// Return the total size in this block		
		ret = info->block.size - ((size_t)addr - (size_t)info->block.addr);
	}
	else {
#if 1 // TODO : Only log this in DEBUG builds (as a failure is already indicated by a null result)?
		EmuWarning("MemoryManager: Attempted to query memory that was not allocated via MemoryManager");
#endif
	}

	LeaveCriticalSection(&m_CriticalSection);

	RETURN(ret);
}
