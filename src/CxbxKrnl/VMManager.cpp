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
// *   Cxbx->Win32->CxbxKrnl->VMManager.cpp
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

// Acknowledgment:
// The core logic of the VMManager class is based on the virtual management code of the citra emulator (GPLv2 license),
// with some modest changes and adaptions to suit Cxbx-Reloaded and Xbox emulation.
// Citra website: https://citra-emu.org/


#include "VMManager.h"
#include "Logging.h"
#include <assert.h>
#include <array>


VMManager g_VMManager;


/* PTEflags */

#define Present  1 << 0
#define OutsideMapped 1 << 11


/* Global page table */

static struct PageTable
{
	// array of memory pointers backing each page
	std::array<PAddr, MAX_NUM_OF_PAGES> addresses;
	// array of fine grained page attributes
	std::array<PTEflags, MAX_NUM_OF_PAGES> attributes;
}page_table;


bool VirtualMemoryArea::CanBeMergedWith(const VirtualMemoryArea& next) const
{
	assert(base + size == next.base);

	if (permissions != next.permissions || type != next.type) { return false; }
	if (type == VMAType::Allocated && backing_block != next.backing_block) { return false; }

	return true;
}

void VMManager::Initialize(HANDLE file_view)
{
	// This reserves a large enough memory region to map the second physical memory file view, and aligns the start address.
	// It must be page aligned otherwise the mapping/unmapping functions will produce incorrect results and on
	// debug builds they will assert.
	UINT_PTR addr = (UINT_PTR)VirtualAlloc(NULL, CHIHIRO_MEMORY_SIZE + PAGE_SIZE, MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!addr)
	{
		CxbxKrnlCleanup("VMManager: VirtualAlloc could not find a suitable region to allocate the second physical memory view!");
	}
	VAddr aligned_start = addr & ~(UINT_PTR)PAGE_MASK;
	VirtualFree((void*)addr, 0, MEM_RELEASE);
	m_Base = (VAddr)MapViewOfFileEx(
		file_view,
		FILE_MAP_READ | FILE_MAP_WRITE | FILE_MAP_EXECUTE,
		0,
		0,
		CHIHIRO_MEMORY_SIZE,
		(void*)aligned_start);
	if (m_Base != aligned_start)
	{
		CxbxKrnlCleanup("VMManager: MapViewOfFileEx could not map the second physical memory view!");
	}
	m_hAliasedView = file_view;

	m_Vma_map.clear();
	::page_table.addresses.fill(NULL);
	::page_table.attributes.fill(0);

	// Initialize the map with a single free region covering the entire virtual address space, less the first page
	VirtualMemoryArea initial_vma;
	initial_vma.base = FIRST_PAGE_ADDR;
	initial_vma.size = MAX_VIRTUAL_ADDRESS - PAGE_SIZE;
	m_Vma_map.emplace(initial_vma.base, initial_vma);
	UpdatePageTableForVMA(initial_vma);

	// Initialize the vma's representing the first page, which is used as guard page
	VirtualMemoryArea first_page_vma;
	first_page_vma.base = ZERO_PAGE_ADDR;
	first_page_vma.size = PAGE_SIZE;
	first_page_vma.permissions = PAGE_GUARD;
	m_Vma_map.emplace(first_page_vma.base, first_page_vma);
	UpdatePageTableForVMA(first_page_vma);

	// D3D uses the first physical page to initialize the push buffer. At the moment, this doesn't seem to be emulated though
	Allocate(PAGE_SIZE, 0, PAGE_SIZE);

	// Allocate the nv2a instance memory and the memory holding the PFN database (the latter is not not emulated)
	// REMARK: I Can't simply call Allocate here since MapMemoryBlock checks if the high addr is higher than m_MaxContiguousAddress,
	// which is the case here, so we must call AllocatePhysicalMemoryRange directly to bypass the check
	VMAIter upper_mem_vma_handle = CarveVMA(m_Base + m_MaxContiguousAddress, 32 * PAGE_SIZE);
	VirtualMemoryArea& upper_mem_vma = upper_mem_vma_handle->second;
	upper_mem_vma.type = VMAType::Allocated;
	upper_mem_vma.permissions = PAGE_EXECUTE_READWRITE;
	upper_mem_vma.backing_block = AllocatePhysicalMemoryRange(32 * PAGE_SIZE, m_MaxContiguousAddress, XBOX_MEMORY_SIZE);
	UpdatePageTableForVMA(upper_mem_vma);
	m_ImageMemoryInUse += 32 * PAGE_SIZE;

	// Allocate memory for the dummy kernel
	// NOTE: change PAGE_SIZE if the size of the dummy kernel increases!
	Allocate(sizeof(DUMMY_KERNEL), XBE_IMAGE_BASE, XBE_IMAGE_BASE + PAGE_SIZE);

	// Map the contiguous memory
	VMAIter contiguous_memory_vma_handle = CarveVMA(CONTIGUOUS_MEMORY_BASE, CONTIGUOUS_MEMORY_XBOX_SIZE);
	VirtualMemoryArea& contiguous_memory_vma = contiguous_memory_vma_handle->second;
	contiguous_memory_vma.type = VMAType::MemContiguous;
	UpdatePageTableForVMA(contiguous_memory_vma);
	m_NonImageMemoryInUse += CONTIGUOUS_MEMORY_XBOX_SIZE;

	// Map the tiled memory
	VMAIter tiled_memory_vma_handle = CarveVMA(TILED_MEMORY_BASE, TILED_MEMORY_XBOX_SIZE);
	VirtualMemoryArea& tiled_memory_vma = tiled_memory_vma_handle->second;
	tiled_memory_vma.type = VMAType::MemTiled;
	UpdatePageTableForVMA(tiled_memory_vma);
	m_NonImageMemoryInUse += TILED_MEMORY_XBOX_SIZE;

	// Map the nv2a (1st region)
	VMAIter nv2a_vma_handle = CarveVMA(NV2A_MEMORY_BASE, NV2A_PRAMIN_ADDR - NV2A_MEMORY_BASE);
	VirtualMemoryArea& nv2a_vma = nv2a_vma_handle->second;
	nv2a_vma.type = VMAType::IO_DeviceNV2A;
	UpdatePageTableForVMA(nv2a_vma);
	m_NonImageMemoryInUse += (NV2A_PRAMIN_ADDR - NV2A_MEMORY_BASE);

	// Map the nv2a pramin memory
	VMAIter nv2a_pramin_vma_handle = CarveVMA(NV2A_PRAMIN_ADDR, NV2A_PRAMIN_SIZE);
	VirtualMemoryArea& nv2a_pramin_vma = nv2a_pramin_vma_handle->second;
	nv2a_pramin_vma.type = VMAType::MemNV2A_PRAMIN;
	UpdatePageTableForVMA(nv2a_pramin_vma);
	m_NonImageMemoryInUse += NV2A_PRAMIN_SIZE;

	// Map the nv2a user region
	VMAIter nv2a_user_vma_handle = CarveVMA(NV2A_USER_ADDR, NV2A_USER_SIZE);
	VirtualMemoryArea& nv2a_user_vma = nv2a_user_vma_handle->second;
	nv2a_user_vma.type = VMAType::IO_DeviceNV2A;
	UpdatePageTableForVMA(nv2a_user_vma);
	m_NonImageMemoryInUse += NV2A_USER_SIZE;

	// Map the apu
	VMAIter apu_vma_handle = CarveVMA(APU_BASE, APU_SIZE);
	VirtualMemoryArea& apu_vma = apu_vma_handle->second;
	apu_vma.type = VMAType::IO_DeviceAPU;
	UpdatePageTableForVMA(apu_vma);
	m_NonImageMemoryInUse += APU_SIZE;

	// Map the ac97
	VMAIter ac97_vma_handle = CarveVMA(AC97_BASE, AC97_SIZE);
	VirtualMemoryArea& ac97_vma = ac97_vma_handle->second;
	ac97_vma.type = VMAType::IO_DeviceAC97;
	UpdatePageTableForVMA(ac97_vma);
	m_NonImageMemoryInUse += AC97_SIZE;

	// Map usb0
	VMAIter usb0_vma_handle = CarveVMA(USB0_BASE, USB0_SIZE);
	VirtualMemoryArea& usb0_vma = usb0_vma_handle->second;
	usb0_vma.type = VMAType::IO_DeviceUSB0;
	UpdatePageTableForVMA(usb0_vma);
	m_NonImageMemoryInUse += USB0_SIZE;

	// Map usb1
	VMAIter usb1_vma_handle = CarveVMA(USB1_BASE, USB1_SIZE);
	VirtualMemoryArea& usb1_vma = usb1_vma_handle->second;
	usb1_vma.type = VMAType::IO_DeviceUSB1;
	UpdatePageTableForVMA(usb1_vma);
	m_NonImageMemoryInUse += USB1_SIZE;

	// Map NVNet
	// NOTE: I can't use NVNet_SIZE because it's not aligned
	VMAIter NVNet_vma_handle = CarveVMA(NVNet_BASE, PAGE_SIZE);
	VirtualMemoryArea& NVNet_vma = NVNet_vma_handle->second;
	NVNet_vma.type = VMAType::IO_DeviceNVNet;
	UpdatePageTableForVMA(NVNet_vma);
	m_NonImageMemoryInUse += PAGE_SIZE;

	// Map the bios
	VMAIter bios_vma_handle = CarveVMA(BIOS_BASE, BIOS_CHIHIRO_SIZE - PAGE_SIZE);
	VirtualMemoryArea& bios_vma = bios_vma_handle->second;
	bios_vma.type = VMAType::DeviceBIOS;
	UpdatePageTableForVMA(bios_vma);
	m_NonImageMemoryInUse += BIOS_CHIHIRO_SIZE;

	// Map the mcpx
	// NOTE: Again, I can't use MCPX_BASE and MCPX_SIZE because those are not aligned
	VMAIter mcpx_vma_handle = CarveVMA(MAX_VIRTUAL_ADDRESS - PAGE_SIZE, PAGE_SIZE);
	VirtualMemoryArea& mcpx_vma = mcpx_vma_handle->second;
	mcpx_vma.type = VMAType::DeviceMCPX;
	MergeAdjacentVMA(mcpx_vma_handle);
	UpdatePageTableForVMA(mcpx_vma);
	m_NonImageMemoryInUse += PAGE_SIZE;

	printf("Page table initialized!\n");
}

void VMManager::InitializeChihiro()
{
	UnmapRange(m_Base + m_MaxContiguousAddress);
	m_MaxContiguousAddress = CHIHIRO_CONTIGUOUS_MEMORY_LIMIT;
	m_MaxPhysicalMemory = CHIHIRO_MEMORY_SIZE;

	// Allocate the nv2a instance memory and the memory holding the PFN database (the latter is not not emulated)
	VMAIter upper_mem_vma_handle = CarveVMA(m_Base + m_MaxContiguousAddress, 48 * PAGE_SIZE);
	VirtualMemoryArea& upper_mem_vma = upper_mem_vma_handle->second;
	upper_mem_vma.type = VMAType::Allocated;
	upper_mem_vma.permissions = PAGE_EXECUTE_READWRITE;
	upper_mem_vma.backing_block = AllocatePhysicalMemoryRange(48 * PAGE_SIZE, m_MaxContiguousAddress, CHIHIRO_MEMORY_SIZE);
	MergeAdjacentVMA(upper_mem_vma_handle);
	UpdatePageTableForVMA(upper_mem_vma);
	m_ImageMemoryInUse += 16 * PAGE_SIZE;

	m_Vma_map.erase(MCPX_BASE);
	m_Vma_map.erase(BIOS_BASE);
	m_NonImageMemoryInUse -= MCPX_SIZE;
	m_NonImageMemoryInUse -= BIOS_XBOX_SIZE;

	// Map the contiguous memory
	m_Vma_map[CONTIGUOUS_MEMORY_BASE].type = VMAType::Free;
	MergeAdjacentVMA(GetVMAIterator(CONTIGUOUS_MEMORY_BASE));
	VMAIter contiguous_memory_vma_handle = CarveVMA(CONTIGUOUS_MEMORY_BASE, CONTIGUOUS_MEMORY_CHIHIRO_SIZE);
	VirtualMemoryArea& contiguous_memory_vma = contiguous_memory_vma_handle->second;
	contiguous_memory_vma.type = VMAType::MemContiguous;
	MergeAdjacentVMA(contiguous_memory_vma_handle);
	UpdatePageTableForVMA(contiguous_memory_vma);
	m_NonImageMemoryInUse += CONTIGUOUS_MEMORY_CHIHIRO_SIZE / 2;

	// Map the tiled memory
	m_Vma_map[TILED_MEMORY_BASE].type = VMAType::Free;
	MergeAdjacentVMA(GetVMAIterator(TILED_MEMORY_BASE));
	VMAIter tiled_memory_vma_handle = CarveVMA(TILED_MEMORY_BASE, TILED_MEMORY_CHIHIRO_SIZE);
	VirtualMemoryArea& tiled_memory_vma = tiled_memory_vma_handle->second;
	tiled_memory_vma.type = VMAType::MemTiled;
	MergeAdjacentVMA(tiled_memory_vma_handle);
	UpdatePageTableForVMA(tiled_memory_vma);
	m_NonImageMemoryInUse += TILED_MEMORY_CHIHIRO_SIZE / 2;

	// Map the bios
	VMAIter bios_vma_handle = CarveVMA(BIOS_BASE, BIOS_CHIHIRO_SIZE);
	VirtualMemoryArea& bios_vma = bios_vma_handle->second;
	bios_vma.type = VMAType::DeviceBIOS;
	MergeAdjacentVMA(bios_vma_handle);
	UpdatePageTableForVMA(bios_vma);
	m_NonImageMemoryInUse += BIOS_CHIHIRO_SIZE;

	printf("Page table for Chihiro initialized!\n");
}

void VMManager::MemoryStatistics(xboxkrnl::PMM_STATISTICS memory_statistics)
{
	memory_statistics->TotalPhysicalPages = m_MaxPhysicalMemory / PAGE_SIZE;
	memory_statistics->AvailablePages = (m_MaxPhysicalMemory - m_PhysicalMemoryInUse) / PAGE_SIZE;
	memory_statistics->VirtualMemoryBytesCommitted = m_ImageMemoryInUse + m_NonImageMemoryInUse;
	memory_statistics->VirtualMemoryBytesReserved = (ULONG)(MAX_VIRTUAL_ADDRESS - (m_ImageMemoryInUse + m_NonImageMemoryInUse));
	memory_statistics->CachePagesCommitted = 0; // not implemented
	memory_statistics->PoolPagesCommitted = 0; // not implemented
	memory_statistics->StackPagesCommitted = m_StackMemoryInUse;
	memory_statistics->ImagePagesCommitted = m_ImageMemoryInUse;
}

VAddr VMManager::Allocate(size_t size, PAddr low_addr, PAddr high_addr, VAddr addr, ULONG Alignment, DWORD protect)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(size);
		LOG_FUNC_ARG(low_addr);
		LOG_FUNC_ARG(high_addr);
		LOG_FUNC_ARG(addr);
		LOG_FUNC_ARG(Alignment);
		LOG_FUNC_ARG(protect);
	LOG_FUNC_END;

	// PAGE_WRITECOMBINE is not allowed for shared memory, unless SEC_WRITECOMBINE flag was specified when calling the
	// CreateFileMapping function. Considering that Cxbx doesn't emulate the caches, it's probably safe to ignore this flag

	Lock();
	size_t aligned_size = (size + PAGE_MASK) & ~PAGE_MASK;
	VAddr v_addr = MapMemoryBlock(aligned_size, low_addr, high_addr, addr, Alignment);
	if (v_addr)
	{
		ReprotectVMARange(v_addr, aligned_size, protect & (~PAGE_WRITECOMBINE));
		protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY) ? 
			m_ImageMemoryInUse += aligned_size : m_NonImageMemoryInUse += aligned_size;
	}
	Unlock();
	return v_addr;
}

VAddr VMManager::AllocateZeroed(size_t size)
{
	LOG_FORWARD(Allocate); // Log AllocateZeroed as the origin of the following RETURN log message

	VAddr addr = Allocate(size);
	if (addr)
	{
		size_t aligned_size = (size + PAGE_MASK) & ~PAGE_MASK;
		memset((void*)addr, 0, aligned_size);
	}

	return addr;
}

VAddr VMManager::AllocateStack(size_t size)
{
	LOG_FUNC_ONE_ARG(size);

	Lock();
	size_t aligned_size = (size + PAGE_SIZE + PAGE_MASK) & ~PAGE_MASK;
	VAddr addr = MapMemoryBlock(aligned_size, 0, MAXULONG_PTR);
	if (addr)
	{
		ReprotectVMARange(addr, PAGE_SIZE, PAGE_NOACCESS);
		addr += aligned_size;
		m_StackMemoryInUse += aligned_size;
	}
	Unlock();
	return addr;
}

void VMManager::DeallocateOverlapped(VAddr addr)
{
	LOG_FUNC_ONE_ARG(addr);

	Lock();
	VAddr aligned_addr = addr & ~(UINT_PTR)PAGE_MASK;
	auto it = m_Vma_map.lower_bound(aligned_addr);
	if (it->first == aligned_addr && it->second.size == PAGE_SIZE)
	{
		UnmapRange(aligned_addr);
	}
	Unlock();
}

void VMManager::Deallocate(VAddr addr)
{
	LOG_FUNC_ONE_ARG(addr);

	Lock();
	UnmapRange(addr);
	Unlock();
}

void VMManager::DeallocateStack(VAddr addr)
{
	LOG_FUNC_ONE_ARG(addr);

	Lock();
	ReprotectVMARange(addr, PAGE_SIZE, PAGE_EXECUTE_READWRITE);
	UnmapRange(addr, true);
	Unlock();
}

void VMManager::Protect(VAddr target, size_t size, DWORD new_perms)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(target);
		LOG_FUNC_ARG(size);
		LOG_FUNC_ARG(new_perms);
	LOG_FUNC_END;

	// PAGE_WRITECOMBINE is not allowed for shared memory, unless SEC_WRITECOMBINE flag was specified when calling the
	// CreateFileMapping function. Considering that Cxbx doesn't emulate the caches, it's probably safe to ignore this flag

	Lock();
	ReprotectVMARange(target, size, new_perms & (~PAGE_WRITECOMBINE));
	Unlock();
}

bool VMManager::QueryVAddr(VAddr addr)
{
	LOG_FUNC_ONE_ARG(addr);

	Lock();
	bool bValid = IsValidVirtualAddress(addr);
	Unlock();
	return bValid;
}

PAddr VMManager::TranslateVAddr(VAddr addr)
{
	LOG_FUNC_ONE_ARG(addr);

	Lock();
	PAddr p_addr = TranslateVAddrToPAddr(addr);
	Unlock();
	return p_addr;
}

void VMManager::RestoreLaunchDataPage(PAddr LaunchDataAddr)
{
	xboxkrnl::LaunchDataPage = (xboxkrnl::LAUNCH_DATA_PAGE*)m_Base + LaunchDataAddr;

	// Mark the launch page as allocated to prevent other allocations from overwriting it
	Allocate(PAGE_SIZE, LaunchDataAddr, LaunchDataAddr + PAGE_SIZE);
}

DWORD VMManager::QueryProtection(VAddr addr)
{
	LOG_FUNC_ONE_ARG(addr);

	DWORD protect = PAGE_NOACCESS;

	Lock();
	auto it = m_Vma_map.lower_bound(addr);
	if (it != m_Vma_map.end() && addr >= it->second.base)
	{
		assert(addr <= it->second.base + it->second.size - 1);
		protect = it->second.permissions;
	}
	Unlock();

	return protect;
}

size_t VMManager::QuerySize(VAddr addr)
{
	LOG_FUNC_ONE_ARG(addr);

	// The Xbox returns at least PAGE_SIZE even for invalid addresses
	size_t size = PAGE_SIZE;

	Lock();
	auto it = m_Vma_map.lower_bound(addr);
	if (it != m_Vma_map.end() && it->second.base == addr)
	{
		// We can't just return the size of the vma because it could have been split by ReprotectVMARange so, instead,
		// we must check the corresponding physical allocation size
		size = it->second.size;
		auto next_it = std::next(it);
		while (next_it != m_Vma_map.end() && it->second.backing_block == next_it->second.backing_block)
		{
			size += next_it->second.size;
			++next_it;
		}
	}
	Unlock();

	return size;
}

VAddr VMManager::MapMemoryBlock(size_t size, PAddr low_addr, PAddr high_addr, VAddr addr, ULONG Alignment)
{
	// Find a free memory block for the allocation, if any
	u32 offset;
	size_t aligned_size = size;
	if (high_addr == MAXULONG_PTR)
	{
		offset = AllocatePhysicalMemory(aligned_size);
	}
	else
	{
		PAddr aligned_low = low_addr & ~(UINT_PTR)(Alignment - 1);
		// Increase high_addr of one page to handle the case when low/high addr are exactly at the boundary of the allocation
		PAddr aligned_high = (high_addr + PAGE_SIZE) & ~(UINT_PTR)(Alignment - 1);
		if (aligned_high > m_MaxContiguousAddress) { aligned_high = m_MaxContiguousAddress; }
		if (aligned_low > aligned_high) { aligned_low = aligned_high - PAGE_SIZE; }

		if (aligned_size > aligned_high - aligned_low) { return NULL; }

		offset = AllocatePhysicalMemoryRange(aligned_size, aligned_low, aligned_high);
	}

	switch (GetError())
	{
		case PMEMORY_SUCCESS:
		{
			if (!addr) {
				addr = m_Base + offset; // VAddr is simply the offset from the base address inside the second file view
			} else {
				addr &= ~PAGE_MASK;
			}

			VMAIter vma_handle = CarveVMA(addr, aligned_size);

			VirtualMemoryArea& final_vma = vma_handle->second;
			final_vma.type = VMAType::Allocated;
			final_vma.permissions = PAGE_EXECUTE_READWRITE;
			final_vma.backing_block = offset;

			UpdatePageTableForVMA(final_vma);

			MergeAdjacentVMA(vma_handle);
		}
		break;

		case PMEMORY_ALLOCATE_FRAGMENTED:
		{
			if (!addr) {
				addr = offset; // VAddr is the aligned address returned by VirtualAlloc
			} else {
				addr &= ~PAGE_MASK;
			}

			VMAIter vma_handle = CarveVMA(addr, aligned_size);

			VirtualMemoryArea& final_vma = vma_handle->second;
			final_vma.type = VMAType::Fragmented;
			final_vma.permissions = PAGE_EXECUTE_READWRITE;
			final_vma.backing_block = offset;

			UpdatePageTableForVMA(final_vma);

			MergeAdjacentVMA(vma_handle);
		}
		break;

		case PMEMORY_INSUFFICIENT_MEMORY:
		{
			return NULL;
		}

		default:
			CxbxKrnlCleanup("Unknown error code in Physical Memory class.");
	}

	return addr;
}

void VMManager::UnmapRange(VAddr target, bool StackFlag)
{
	VAddr aligned_start = target & ~(UINT_PTR)PAGE_MASK;
	if (aligned_start == 0) // forbidden page
	{
		// This should also generate a STATUS_GUARD_PAGE_VIOLATION exception from Windows
		CxbxKrnlCleanup("Access to guarded page 0x0 detected!\n");
	}
	auto it = m_Vma_map.lower_bound(aligned_start);
	if (it->first != aligned_start)
	{
		CxbxKrnlCleanup("An attempt to deallocate a region not allocated by the manager has been detected!");
	}
	else
	{
		switch (it->second.type)
		{
			case VMAType::Free:
			case VMAType::MemContiguous:
			case VMAType::MemTiled:
			case VMAType::IO_DeviceNV2A:
			case VMAType::MemNV2A_PRAMIN:
			case VMAType::IO_DeviceAPU:
			case VMAType::IO_DeviceAC97:
			case VMAType::IO_DeviceUSB0:
			case VMAType::IO_DeviceUSB1:
			case VMAType::IO_DeviceNVNet:
			case VMAType::DeviceBIOS:
			case VMAType::DeviceMCPX:
			{
				CxbxKrnlCleanup("An attempt to deallocate a region not allocated by the manager has been detected! \
The type was %u", it->second.type);
			}
			default:
			{
				size_t aligned_size = it->second.size;
				if (StackFlag)
				{
					m_StackMemoryInUse -= aligned_size;
				}
				else
				{
					it->second.permissions & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY) ?
						m_ImageMemoryInUse -= aligned_size : m_NonImageMemoryInUse -= aligned_size;
				}
				if (it->second.type == VMAType::Allocated)
				{
					DeAllocatePhysicalMemory(it->second.backing_block);
				}
				else if (it->second.type == VMAType::Fragmented)
				{
					DeAllocateFragmented(it->second.backing_block);
				}
				VMAIter vma = CarveVMARange(aligned_start, aligned_size);

				VAddr target_end = aligned_start + aligned_size;
				VMAIter end = m_Vma_map.end();
				// The comparison against the end of the range must be done using addresses since vma's can be
				// merged during this process, causing invalidation of the iterators
				while (vma != end && vma->second.base < target_end)
				{
					vma = std::next(Unmap(vma));
				}
			}
		}
	}
}

void VMManager::ReprotectVMARange(VAddr target, size_t size, DWORD new_perms)
{
	VAddr aligned_start = target & ~(UINT_PTR)PAGE_MASK;
	size_t aligned_size = ((target + size - 1 + PAGE_SIZE) &
		~(UINT_PTR)PAGE_MASK) - aligned_start;

	VMAIter vma = CarveVMARange(aligned_start, aligned_size);
	VAddr target_end = aligned_start + aligned_size;

	VMAIter end = m_Vma_map.end();
	// The comparison against the end of the range must be done using addresses since vma's can be
	// merged during this process, causing invalidation of the iterators.
	while (vma != end && vma->second.base < target_end)
	{
		vma = std::next(ReprotectVMA(vma, new_perms));
	}
}

bool VMManager::IsValidVirtualAddress(const VAddr addr)
{
	if (::page_table.attributes[addr >> PAGE_SHIFT] & Present) { return true; }

	return false;
}

PAddr VMManager::TranslateVAddrToPAddr(const VAddr addr)
{
	if (IsValidVirtualAddress(addr))
	{
		PAddr page_address = ::page_table.addresses[addr >> PAGE_SHIFT];
		u32 page_offset = addr & PAGE_MASK;
		return page_address += page_offset;
	}

	return NULL;
}

void VMManager::Lock()
{
	EnterCriticalSection(&m_CriticalSection);
}

void VMManager::Unlock()
{
	LeaveCriticalSection(&m_CriticalSection);
}

void VMManager::MapMemoryRegion(VAddr base, size_t size, PAddr target)
{
	assert((base & PAGE_MASK) == 0); // unaligned address
	assert((size & PAGE_MASK) == 0); // unaligned size

	MapPages(base / PAGE_SIZE, size / PAGE_SIZE, target, Present);
}

void VMManager::MapSpecialRegion(VAddr base, size_t size, PAddr target)
{
	assert((base & PAGE_MASK) == 0); // unaligned address
	assert((size & PAGE_MASK) == 0); // unaligned size

	MapPages(base / PAGE_SIZE, size / PAGE_SIZE, target, Present | OutsideMapped);
}

void VMManager::UnmapRegion(VAddr base, size_t size)
{
	assert((base & PAGE_MASK) == 0); // unaligned address
	assert((size & PAGE_MASK) == 0); // unaligned size

	MapPages(base / PAGE_SIZE, size / PAGE_SIZE, NULL, Present ^ Present);
}


VMManager::VMAIter VMManager::Unmap(VMAIter vma_handle)
{
	VirtualMemoryArea& vma = vma_handle->second;
	vma.type = VMAType::Free;
	vma.permissions = PAGE_NOACCESS;
	vma.backing_block = NULL;

	UpdatePageTableForVMA(vma);

	return MergeAdjacentVMA(vma_handle);
}

void VMManager::MapPages(u32 page_num, u32 page_count, PAddr memory, PTEflags type)
{
	u32 end_page_num = page_num + page_count;
	while (page_num != end_page_num)
	{
		assert(page_num < MAX_NUM_OF_PAGES); // out of virtual pages

		::page_table.addresses[page_num] = memory;
		::page_table.attributes[page_num] = type;

		page_num += 1;

		if (type & Present)
			memory += PAGE_SIZE;
	}
}

VMManager::VMAIter VMManager::CarveVMA(VAddr base, size_t size)
{
	VMAIter vma_handle = GetVMAIterator(base);

	// base address is outside the range managed by the kernel
	assert(vma_handle != m_Vma_map.end());

	VirtualMemoryArea& vma = vma_handle->second;

	// region is already allocated
	assert(vma.type == VMAType::Free);

	u32 start_in_vma = base - vma.base; // VAddr - start addr of vma region found (must be VMAType::Free)
	u32 end_in_vma = start_in_vma + size; // end addr of new vma

	// requested allocation doesn't fit inside vma
	assert(end_in_vma <= vma.size);

	if (end_in_vma != vma.size)
	{
		// split vma at the end of the allocated region
		SplitVMA(vma_handle, end_in_vma);
	}
	if (start_in_vma != 0)
	{
		// split vma at the start of the allocated region
		vma_handle = SplitVMA(vma_handle, start_in_vma);
	}

	return vma_handle;
}

VMManager::VMAIter VMManager::CarveVMARange(VAddr base, size_t size)
{
	VAddr target_end = base + size;
	assert(target_end >= base);
	assert(target_end <= MAX_VIRTUAL_ADDRESS - PAGE_SIZE);
	assert(size > 0);

	VMAIter begin_vma = GetVMAIterator(base);
	VMAIter i_end = m_Vma_map.lower_bound(target_end);
	for (auto i = begin_vma; i != i_end; ++i)
	{
		if (i->second.type == VMAType::Free) { assert(0); }
	}

	if (base != begin_vma->second.base)
	{
		begin_vma = SplitVMA(begin_vma, base - begin_vma->second.base);
	}

	VMAIter end_vma = GetVMAIterator(target_end);
	if (end_vma != m_Vma_map.end() && target_end != end_vma->second.base)
	{
		end_vma = SplitVMA(end_vma, target_end - end_vma->second.base);
	}

	return begin_vma;
}

VMManager::VMAIter VMManager::GetVMAIterator(VAddr target)
{
	return std::prev(m_Vma_map.upper_bound(target));
}

VMManager::VMAIter VMManager::SplitVMA(VMAIter vma_handle, u32 offset_in_vma)
{
	VirtualMemoryArea& old_vma = vma_handle->second;
	VirtualMemoryArea new_vma = old_vma; // make a copy of the vma

	// allow splitting at a boundary?
	assert(offset_in_vma < old_vma.size);
	assert(offset_in_vma > 0);

	old_vma.size = offset_in_vma;
	new_vma.base += offset_in_vma;
	new_vma.size -= offset_in_vma;

	// add the new splitted vma to m_Vma_map
	return m_Vma_map.emplace_hint(std::next(vma_handle), new_vma.base, new_vma);
}

VMManager::VMAIter VMManager::MergeAdjacentVMA(VMAIter it)
{
	VMAIter next_vma = std::next(it);
	if (next_vma != m_Vma_map.end() && it->second.CanBeMergedWith(next_vma->second))
	{
		it->second.size += next_vma->second.size;
		m_Vma_map.erase(next_vma);
	}

	if (it != m_Vma_map.begin())
	{
		VMAIter prev_vma = std::prev(it);
		if (prev_vma->second.CanBeMergedWith(it->second))
		{
			prev_vma->second.size += it->second.size;
			m_Vma_map.erase(it);
			it = prev_vma;
		}
	}

	return it;
}

VMManager::VMAIter VMManager::ReprotectVMA(VMAIter vma_handle, DWORD new_perms)
{
	VirtualMemoryArea& vma = vma_handle->second;
	DWORD dummy;
	vma.permissions = new_perms;
	if (!VirtualProtect((void*)vma.base, vma.size, new_perms, &dummy))
	{
		CxbxKrnlCleanup("ReprotectVMA: VirtualProtect could not protect the vma! The error code was %d", GetLastError());
	}
	UpdatePageTableForVMA(vma);

	return MergeAdjacentVMA(vma_handle);
}

void VMManager::UpdatePageTableForVMA(const VirtualMemoryArea& vma)
{
	switch (vma.type)
	{
		case VMAType::Free:
		case VMAType::MemContiguous:
		case VMAType::MemTiled:
		case VMAType::IO_DeviceNV2A:
		case VMAType::MemNV2A_PRAMIN:
		case VMAType::IO_DeviceAPU:
		case VMAType::IO_DeviceAC97:
		case VMAType::IO_DeviceUSB0:
		case VMAType::IO_DeviceUSB1:
		case VMAType::IO_DeviceNVNet:
		case VMAType::DeviceBIOS:
		case VMAType::DeviceMCPX:
		{
			UnmapRegion(vma.base, vma.size);
		}
		break;

		case VMAType::Allocated:
		{
			MapMemoryRegion(vma.base, vma.size, vma.backing_block);
		}
		break;

		case VMAType::Fragmented:
		{
			MapSpecialRegion(vma.base, vma.size, vma.backing_block);
		}
		break;

		default:
			CxbxKrnlCleanup("Unknown VMAType in UpdatePageTableForVMA");
	}
}
