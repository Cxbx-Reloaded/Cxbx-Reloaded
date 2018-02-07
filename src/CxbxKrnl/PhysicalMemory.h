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
// *   Cxbx->Win32->CxbxKrnl->PhysicalMemory.h
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
// *  (c) 2017      ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

#ifndef PHYSICAL_MEMORY_H
#define PHYSICAL_MEMORY_H

#define _XBOXKRNL_DEFEXTRN_

namespace xboxkrnl
{
	#include <xboxkrnl/xboxkrnl.h>
};

#include "Emu.h"
#include "CxbxKrnl.h"
#include <windows.h>
#include <map>


/* PhysicalMemory class error codes */

#define PMEMORY_STATUS 					unsigned int
#define PMEMORY_SUCCESS					0x0
#define PMEMORY_INSUFFICIENT_MEMORY		0x1
#define PMEMORY_ALLOCATE_FRAGMENTED		0x2


/* Global typedefs */

typedef UINT_PTR VAddr;
typedef UINT_PTR PAddr;
typedef std::uint32_t u32;
typedef DWORD PTEflags;
enum class PageType : u32 { 
	Unknown, // Verified to be called 'Unknown'
	Stack, // Used by MmCreateKernelStack / VMManager::AllocateStack
	PageTable, // Not yet used
	Unknown1, // System-related?
	Pool, // Used by ExAllocatePoolWithTag
	VirtualMemory, 
	SystemMemory, // Used by MmAllocateSystemMemory
	Image, // Used by XeLoadSection
	Cache, // Not yet used
	Contiguous, // Used by MmAllocateContiguousMemoryEx
	Unknown2, // xbdm-related?
	COUNT
};

extern DWORD DefaultPageTypeProtection(const PageType page_type);

/* PhysicalMemory class */

class PhysicalMemory
{
	protected:
		// if the block to allocate is smaller than AllocationThreshold, then it will be mapped starting from the bottom of the memory,
		// otherwise it's mapped from the top. AllocationThreshold is 64KiB, the allocation granularity of the Xbox
		const unsigned int m_AllocationThreshold = 1024 * 64;
		// amount of physical memory in use
		size_t m_PhysicalMemoryInUse = 0;
		// max physical memory available on the Xbox/Chihiro
		size_t m_MaxPhysicalMemory = XBOX_MEMORY_SIZE;
		// map tracking the physical memory currently in use
		std::map<PAddr, size_t> m_Mem_map;
		// map tracking the blocks allocated with VirtualAlloc
		std::map<VAddr, size_t> m_Fragmented_mem_map;
		// number of allocated pages per type
		u32 m_PageCount[(int)PageType::COUNT];
		// current error status code of the PhysicalMemory class
		PMEMORY_STATUS m_Status = PMEMORY_SUCCESS;
		// highest address available for contiguous allocations
		PAddr m_MaxContiguousAddress = XBOX_CONTIGUOUS_MEMORY_LIMIT;
	
		// protected constructor so PhysicalMemory can only be inherited from
		PhysicalMemory() {};
		// destructor
		~PhysicalMemory()
		{
			for (auto it = m_Fragmented_mem_map.begin(), end = m_Fragmented_mem_map.end(); it != end; ++it)
			{
				VirtualFree((void*)it->first, 0, MEM_RELEASE);
			}
		}
		// allocates a block of the mapped file, returns m_MaxContiguousAddress and sets an error code if unsuccessful
		PAddr AllocatePhysicalMemory(const size_t size, const PageType type);
		// allocates a block of the mapped file between the specified range if possible
		PAddr AllocatePhysicalMemoryRange(const size_t size, const PageType type, const PAddr low_addr, const PAddr high_addr);
		// allocates a block of memory with VirtualAlloc when the main memory is fragmented and sets an error code
		VAddr AllocateFragmented(const size_t size, const PageType type);
		// shrinks the size af an allocation
		void ShrinkPhysicalAllocation(const PAddr addr, const size_t offset, const bool bFragmentedMap, const bool bStart);
		// deallocates a block of the mapped file
		void DeAllocatePhysicalMemory(const PAddr addr);
		// deallocates a block allocated with VirtualAlloc
		void DeAllocateFragmented(const VAddr addr);
		// retrieves the current error code of the PhysicalMemory class
		PMEMORY_STATUS GetError() const;
		// sets the error code of the PhysicalMemory class
		void SetError(const PMEMORY_STATUS err);
		// clears the error code of the PhysicalMemory class
		void ClearError();
};

#endif
