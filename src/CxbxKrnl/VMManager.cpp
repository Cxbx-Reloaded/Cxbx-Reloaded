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
		return;
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
		return;
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
	VMAIter nv2a_pushbuffer_vma_handle = CarveVMA(m_Base, PAGE_SIZE);
	VirtualMemoryArea& nv2a_pushbuffer_vma = nv2a_pushbuffer_vma_handle->second;
	nv2a_pushbuffer_vma.type = VMAType::Allocated;
	nv2a_pushbuffer_vma.permissions = PAGE_EXECUTE_READWRITE;
	nv2a_pushbuffer_vma.backing_block = AllocatePhysicalMemoryRange(PAGE_SIZE, 0, PAGE_SIZE);
	MergeAdjacentVMA(nv2a_pushbuffer_vma_handle);
	UpdatePageTableForVMA(nv2a_pushbuffer_vma);

	// Allocate the nv2a instance memory and the memory holding the PFN database (the latter is not not emulated)
	VMAIter upper_mem_vma_handle = CarveVMA(m_Base + m_MaxContiguousAddress, 32 * PAGE_SIZE);
	VirtualMemoryArea& upper_mem_vma = upper_mem_vma_handle->second;
	upper_mem_vma.type = VMAType::Allocated;
	upper_mem_vma.permissions = PAGE_EXECUTE_READWRITE;
	upper_mem_vma.backing_block = AllocatePhysicalMemoryRange(32 * PAGE_SIZE, m_MaxContiguousAddress, XBOX_MEMORY_SIZE);
	MergeAdjacentVMA(upper_mem_vma_handle);
	UpdatePageTableForVMA(upper_mem_vma);

	// Allocate memory for the dummy kernel
	VMAIter dummy_kernel_vma_handle = CarveVMA(m_Base + XBE_IMAGE_BASE, sizeof(DUMMY_KERNEL));
	VirtualMemoryArea& dummy_kernel_vma = dummy_kernel_vma_handle->second;
	dummy_kernel_vma.type = VMAType::Allocated;
	dummy_kernel_vma.permissions = PAGE_EXECUTE_READWRITE;
	dummy_kernel_vma.backing_block = AllocatePhysicalMemoryRange(sizeof(DUMMY_KERNEL), XBE_IMAGE_BASE, XBE_IMAGE_BASE + sizeof(DUMMY_KERNEL));
	MergeAdjacentVMA(dummy_kernel_vma_handle);
	UpdatePageTableForVMA(dummy_kernel_vma);

	// Map the contiguous memory
	VMAIter contiguous_memory_vma_handle = CarveVMA(CONTIGUOUS_MEMORY_BASE, CONTIGUOUS_MEMORY_XBOX_SIZE);
	VirtualMemoryArea& contiguous_memory_vma = contiguous_memory_vma_handle->second;
	contiguous_memory_vma.type = VMAType::MemContiguous;
	MergeAdjacentVMA(contiguous_memory_vma_handle);
	UpdatePageTableForVMA(contiguous_memory_vma);

	// Map the tiled memory
	VMAIter tiled_memory_vma_handle = CarveVMA(TILED_MEMORY_BASE, TILED_MEMORY_XBOX_SIZE);
	VirtualMemoryArea& tiled_memory_vma = tiled_memory_vma_handle->second;
	tiled_memory_vma.type = VMAType::MemTiled;
	MergeAdjacentVMA(tiled_memory_vma_handle);
	UpdatePageTableForVMA(tiled_memory_vma);

	// Map the nv2a (1st region)
	VMAIter nv2a_vma_handle = CarveVMA(NV2A_MEMORY_BASE, NV2A_PRAMIN_ADDR - NV2A_MEMORY_BASE);
	VirtualMemoryArea& nv2a_vma = nv2a_vma_handle->second;
	nv2a_vma.type = VMAType::IO_DeviceNV2A;
	MergeAdjacentVMA(nv2a_vma_handle);
	UpdatePageTableForVMA(nv2a_vma);

	// Map the nv2a pramin memory
	VMAIter nv2a_pramin_vma_handle = CarveVMA(NV2A_PRAMIN_ADDR, NV2A_PRAMIN_SIZE);
	VirtualMemoryArea& nv2a_pramin_vma = nv2a_pramin_vma_handle->second;
	nv2a_pramin_vma.type = VMAType::MemNV2A_PRAMIN;
	MergeAdjacentVMA(nv2a_pramin_vma_handle);
	UpdatePageTableForVMA(nv2a_pramin_vma);

	// Map the nv2a user region
	VMAIter nv2a_user_vma_handle = CarveVMA(NV2A_USER_ADDR, NV2A_USER_SIZE);
	VirtualMemoryArea& nv2a_user_vma = nv2a_user_vma_handle->second;
	nv2a_user_vma.type = VMAType::IO_DeviceNV2A;
	MergeAdjacentVMA(nv2a_user_vma_handle);
	UpdatePageTableForVMA(nv2a_user_vma);

	// Map the apu
	VMAIter apu_vma_handle = CarveVMA(APU_BASE, APU_SIZE);
	VirtualMemoryArea& apu_vma = apu_vma_handle->second;
	apu_vma.type = VMAType::IO_DeviceAPU;
	MergeAdjacentVMA(apu_vma_handle);
	UpdatePageTableForVMA(apu_vma);

	// Map the ac97
	VMAIter ac97_vma_handle = CarveVMA(AC97_BASE, AC97_SIZE);
	VirtualMemoryArea& ac97_vma = ac97_vma_handle->second;
	ac97_vma.type = VMAType::IO_DeviceAC97;
	MergeAdjacentVMA(ac97_vma_handle);
	UpdatePageTableForVMA(ac97_vma);

	// Map usb0
	VMAIter usb0_vma_handle = CarveVMA(USB0_BASE, USB0_SIZE);
	VirtualMemoryArea& usb0_vma = usb0_vma_handle->second;
	usb0_vma.type = VMAType::IO_DeviceUSB0;
	MergeAdjacentVMA(usb0_vma_handle);
	UpdatePageTableForVMA(usb0_vma);

	// Map usb1
	VMAIter usb1_vma_handle = CarveVMA(USB1_BASE, USB1_SIZE);
	VirtualMemoryArea& usb1_vma = usb1_vma_handle->second;
	usb1_vma.type = VMAType::IO_DeviceUSB1;
	MergeAdjacentVMA(usb1_vma_handle);
	UpdatePageTableForVMA(usb1_vma);

	// Map NVNet
	VMAIter NVNet_vma_handle = CarveVMA(NVNet_BASE, NVNet_SIZE);
	VirtualMemoryArea& NVNet_vma = NVNet_vma_handle->second;
	NVNet_vma.type = VMAType::IO_DeviceNVNet;
	MergeAdjacentVMA(NVNet_vma_handle);
	UpdatePageTableForVMA(NVNet_vma);

	// Map the bios
	VMAIter bios_vma_handle = CarveVMA(BIOS_BASE, BIOS_XBOX_SIZE);
	VirtualMemoryArea& bios_vma = bios_vma_handle->second;
	bios_vma.type = VMAType::DeviceBIOS;
	MergeAdjacentVMA(bios_vma_handle);
	UpdatePageTableForVMA(bios_vma);

	// Map the mcpx
	VMAIter mcpx_vma_handle = CarveVMA(MCPX_BASE, MCPX_SIZE);
	VirtualMemoryArea& mcpx_vma = mcpx_vma_handle->second;
	mcpx_vma.type = VMAType::DeviceMCPX;
	MergeAdjacentVMA(mcpx_vma_handle);
	UpdatePageTableForVMA(mcpx_vma);

	printf("Page table initialized!");
}

void VMManager::InitializeChihiro()
{
	UnmapRange(m_Base + m_MaxContiguousAddress);
	m_MaxContiguousAddress = CHIHIRO_CONTIGUOUS_MEMORY_LIMIT;

	// Allocate the nv2a instance memory and the memory holding the PFN database (the latter is not not emulated)
	VMAIter upper_mem_vma_handle = CarveVMA(m_Base + m_MaxContiguousAddress, 48 * PAGE_SIZE);
	VirtualMemoryArea& upper_mem_vma = upper_mem_vma_handle->second;
	upper_mem_vma.type = VMAType::Allocated;
	upper_mem_vma.permissions = PAGE_EXECUTE_READWRITE;
	upper_mem_vma.backing_block = AllocatePhysicalMemoryRange(48 * PAGE_SIZE, m_MaxContiguousAddress, CHIHIRO_MEMORY_SIZE);
	MergeAdjacentVMA(upper_mem_vma_handle);
	UpdatePageTableForVMA(upper_mem_vma);

	m_Vma_map.erase(MCPX_BASE);
	m_Vma_map.erase(BIOS_BASE);

	// Map the contiguous memory
	m_Vma_map[CONTIGUOUS_MEMORY_BASE].type = VMAType::Free;
	MergeAdjacentVMA(GetVMAIterator(CONTIGUOUS_MEMORY_BASE));
	VMAIter contiguous_memory_vma_handle = CarveVMA(CONTIGUOUS_MEMORY_BASE, CONTIGUOUS_MEMORY_CHIHIRO_SIZE);
	VirtualMemoryArea& contiguous_memory_vma = contiguous_memory_vma_handle->second;
	contiguous_memory_vma.type = VMAType::MemContiguous;
	MergeAdjacentVMA(contiguous_memory_vma_handle);
	UpdatePageTableForVMA(contiguous_memory_vma);

	// Map the tiled memory
	m_Vma_map[TILED_MEMORY_BASE].type = VMAType::Free;
	MergeAdjacentVMA(GetVMAIterator(TILED_MEMORY_BASE));
	VMAIter tiled_memory_vma_handle = CarveVMA(TILED_MEMORY_BASE, TILED_MEMORY_CHIHIRO_SIZE);
	VirtualMemoryArea& tiled_memory_vma = tiled_memory_vma_handle->second;
	tiled_memory_vma.type = VMAType::MemTiled;
	MergeAdjacentVMA(tiled_memory_vma_handle);
	UpdatePageTableForVMA(tiled_memory_vma);

	// Map the bios
	VMAIter bios_vma_handle = CarveVMA(BIOS_BASE, BIOS_CHIHIRO_SIZE);
	VirtualMemoryArea& bios_vma = bios_vma_handle->second;
	bios_vma.type = VMAType::DeviceBIOS;
	MergeAdjacentVMA(bios_vma_handle);
	UpdatePageTableForVMA(bios_vma);

	printf("Page table for Chihiro initialized!");
}

void VMManager::VMStatistics() const
{
	for (auto it = m_Vma_map.begin(); it != m_Vma_map.end(); ++it)
	{
		printf("VAddr is 0x%X\n\tbase: 0x%X\n\tsize: %u\n\ttype: %u\n\tpermissions: %u\n\tbacking_block: 0x%X\n",
			it->first,
			it->second.base,
			it->second.size,
			it->second.type,
			it->second.permissions,
			it->second.backing_block);
	}
}

VAddr VMManager::Allocate(size_t size, PAddr low_addr, PAddr high_addr, VAddr addr = NULL, ULONG Alignment = PAGE_SIZE, DWORD protect = PAGE_EXECUTE_READWRITE)
{
	Lock();
	VAddr addr = MapMemoryBlock(size, low_addr, high_addr, addr, Alignment);
	if (addr)
	{
		ReprotectVMARange(addr, size, protect);
		size_t aligned_size = (PAGE_MASK & size) ? ((size + PAGE_SIZE) & ~PAGE_MASK) : size;
		protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY) ? 
			ImageMemoryInUse += aligned_size : NonImageMemoryInUse += aligned_size;
	}
	Unlock();
	return addr;
}

VAddr VMManager::AllocateStack(size_t size)
{
	Lock();
	VAddr addr = MapMemoryBlock(size + PAGE_SIZE, 0, MAXULONG_PTR);
	if (addr)
	{
		ReprotectVMARange(addr, PAGE_SIZE, PAGE_NOACCESS);
		size_t aligned_size = (PAGE_MASK & size) ? ((size + PAGE_SIZE) & ~PAGE_MASK) : size;
		addr = addr + PAGE_SIZE + aligned_size;
		StackMemoryInUse += aligned_size;
	}
	Unlock();
	return addr;
}

void VMManager::Deallocate(VAddr addr)
{
	Lock();
	UnmapRange(addr);
	Unlock();
}

void VMManager::DeallocateStack(VAddr addr)
{
	Lock();
	ReprotectVMARange(addr, PAGE_SIZE, PAGE_EXECUTE_READWRITE);
	UnmapRange(addr, true);
	Unlock();
}

void VMManager::Protect(VAddr target, size_t size, DWORD new_perms)
{
	Lock();
	ReprotectVMARange(target, size, new_perms & (~PAGE_WRITECOMBINE));
	Unlock();
}

bool VMManager::QueryVAddr(VAddr addr)
{
	Lock();
	bool ret = IsValidVirtualAddress(addr);
	Unlock();
	return ret;
}

PAddr VMManager::TranslateVAddr(VAddr addr)
{
	Lock();
	PAddr p_addr = TranslateVAddrToPAddr(addr);
	Unlock();
	return p_addr;
}

VAddr VMManager::MapMemoryBlock(size_t size, PAddr low_addr, PAddr high_addr, VAddr addr, ULONG Alignment)
{
	// Align the allocation size to the next page boundary and find a free memory block, if any
	u32 offset;
	size_t aligned_size = (PAGE_MASK & size) ? ((size + PAGE_SIZE) & ~PAGE_MASK) : size;
	if (high_addr == MAXULONG_PTR)
	{
		offset = AllocatePhysicalMemory(aligned_size);
	}
	else
	{
		PAddr aligned_low = low_addr & ~(UINT_PTR)(Alignment - 1);
		PAddr aligned_high = high_addr & ~(UINT_PTR)(Alignment - 1);
		if (aligned_high > m_MaxContiguousAddress) { aligned_high = m_MaxContiguousAddress; }
		if (aligned_low > aligned_high) { aligned_low = aligned_high - PAGE_SIZE; }

		if (aligned_size > aligned_high - aligned_low) { return NULL; }

		offset = AllocatePhysicalMemoryRange(aligned_size, aligned_low, aligned_high);
	}

	switch (GetError())
	{
		case PMEMORY_SUCCESS:
		{
			if (!addr)
			{
				addr = m_Base + offset; // VAddr is simply the offset from the base address inside the second file view
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
			if (!addr)
			{
				addr = offset; // VAddr is the aligned address returned by VirtualAlloc
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
		{
			CxbxKrnlCleanup("Unknown error code in Physical Memory class.");
			return;
		}
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
		return;
	}
	auto it = m_Vma_map.find(aligned_start);
	if (it == m_Vma_map.end())
	{
		CxbxKrnlCleanup("An attempt to deallocate a region not allocated by the manager has been detected!");
		return;
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
				return;
			}
			default:
			{
				size_t aligned_size = it->second.size;
				if (StackFlag)
				{
					StackMemoryInUse -= aligned_size;
				}
				else
				{
					it->second.permissions & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY) ?
						ImageMemoryInUse -= aligned_size : NonImageMemoryInUse -= aligned_size;
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
	assert(target_end <= MAX_HIGHEST_PAGE_ADDRESS);
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
		CxbxKrnlCleanup("ReprotectVMA: VirtualProtect could not protect the vma!");
		return vma_handle;
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
		{
			CxbxKrnlCleanup("Unknown VMAType in UpdatePageTableForVMA");
			return;
		}	
	}
}
