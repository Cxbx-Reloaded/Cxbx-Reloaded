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
// with some changes and adaptions to suit Cxbx-Reloaded and Xbox emulation.
// Citra website: https://citra-emu.org/


#include "VMManager.h"
#include "Logging.h"
#include "EmuShared.h"
#include <assert.h>
#include <array>


VMManager g_VMManager;


/* PTEflags */

#define Present  1 << 0


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

	if (permissions != next.permissions || type != next.type ||
		type == VMAType::Lock || next.type == VMAType::Lock) { return false; }
	if (type == VMAType::Allocated && backing_block != next.backing_block) { return false; }

	return true;
}

void VMManager::Initialize(HANDLE file_view)
{
	// This reserves a large enough memory region to map the second physical memory file view
	uintptr_t start = (uintptr_t)VirtualAlloc(NULL, CHIHIRO_MEMORY_SIZE, MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!start)
	{
		CxbxKrnlCleanup("VMManager: VirtualAlloc could not find a suitable region to allocate the second physical memory view!");
	}
	VirtualFree((void*)start, 0, MEM_RELEASE);
	m_Base = (VAddr)MapViewOfFileEx(
		file_view,
		FILE_MAP_READ | FILE_MAP_WRITE | FILE_MAP_EXECUTE,
		0,
		0,
		CHIHIRO_MEMORY_SIZE,
		(void*)start);
	if (m_Base != start)
	{
		UnmapViewOfFile((void*)start);
		CxbxKrnlCleanup("VMManager: MapViewOfFileEx could not map the second physical memory view!");
	}

	// I'm still unsure about this...
	// Map the page tables
	void *PageTableAddr = (void *)VirtualAlloc(
		(void *)PAGE_TABLES_BASE,
		PAGE_TABLES_END - PAGE_TABLES_BASE + 1,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_READWRITE);
	if (PageTableAddr != (void *)PAGE_TABLES_BASE)
	{
		CxbxKrnlCleanup("Couldn't map the page table range to 0xC0000000!");
	}

	m_hAliasedView = file_view;
	m_Vma_map.clear();

	// Set up general memory variables according to the xbe type
	if (!g_IsRetail)
	{
		m_MaxContiguousAddress = CHIHIRO_CONTIGUOUS_MEMORY_LIMIT;
		m_UpperPMemorySize = 48 * PAGE_SIZE;
		m_MaxPhysicalMemory = CHIHIRO_MEMORY_SIZE;
	}

	// Set up the pfn database
	bool bQuickReboot;
	g_EmuShared->GetQuickRebootFlag(&bQuickReboot);
	if (bQuickReboot) {
		ReinitializePfnDatabase();
	}
	else {
		InitializePfnDatabase();
	}
	::page_table.addresses.fill(NULL);
	::page_table.attributes.fill(0);

	// Initialize the map with a single free region covering the entire virtual address space, less the first page
	VirtualMemoryArea initial_vma;
	initial_vma.base = FIRST_PAGE_ADDR;
	initial_vma.size = MAX_VIRTUAL_ADDRESS - PAGE_SIZE + 1;
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
	Allocate(PAGE_SIZE, 0, PAGE_SIZE - 1, PAGE_SIZE, PAGE_EXECUTE_READWRITE, false);

	// Allocate the nv2a instance memory and the memory holding the PFN database
	// REMARK: I Can't simply call Allocate here since MapMemoryBlock checks if the high addr is higher than m_MaxContiguousAddress,
	// which is the case here, so we must call AllocatePhysicalMemoryRange directly to bypass the check
	VMAIter upper_mem_vma_handle = CarveVMA(CONTIGUOUS_MEMORY_BASE + m_MaxContiguousAddress, 32 * PAGE_SIZE);
	VirtualMemoryArea& upper_mem_vma = upper_mem_vma_handle->second;
	upper_mem_vma.type = VMAType::Allocated;
	upper_mem_vma.permissions = PAGE_EXECUTE_READWRITE;
	upper_mem_vma.backing_block = AllocatePhysicalMemoryRange(32 * PAGE_SIZE, m_MaxContiguousAddress, XBOX_MEMORY_SIZE);
	UpdatePageTableForVMA(upper_mem_vma);
	m_ImageMemoryInUse += 32 * PAGE_SIZE;

	// Allocate memory for the dummy kernel
	// NOTE: change PAGE_SIZE if the size of the dummy kernel increases!
	Allocate(KERNEL_SIZE, XBE_IMAGE_BASE, XBE_IMAGE_BASE + PAGE_SIZE - 1, KERNEL_SIZE & ~PAGE_MASK, PAGE_EXECUTE_READWRITE, false);

	// Map the tiled memory
	MapHardwareDevice(TILED_MEMORY_BASE, TILED_MEMORY_XBOX_SIZE, VMAType::MemTiled);

	// Map the nv2a (1st region)
	MapHardwareDevice(NV2A_MEMORY_BASE, NV2A_PRAMIN_ADDR - NV2A_MEMORY_BASE, VMAType::IO_DeviceNV2A);

	// Map the nv2a pramin memory
	MapHardwareDevice(NV2A_PRAMIN_ADDR, NV2A_PRAMIN_SIZE, VMAType::MemNV2A_PRAMIN);

	// Map the nv2a user region
	MapHardwareDevice(NV2A_USER_ADDR, NV2A_USER_SIZE, VMAType::IO_DeviceNV2A);

	// Map the apu
	MapHardwareDevice(APU_BASE, APU_SIZE, VMAType::IO_DeviceAPU);

	// Map the ac97
	MapHardwareDevice(AC97_BASE, AC97_SIZE, VMAType::IO_DeviceAC97);

	// Map usb0
	MapHardwareDevice(USB0_BASE, USB0_SIZE, VMAType::IO_DeviceUSB0);

	// Map usb1
	MapHardwareDevice(USB1_BASE, USB1_SIZE, VMAType::IO_DeviceUSB1);

	// Map NVNet
	// NOTE: I can't use NVNet_SIZE because it's not aligned
	MapHardwareDevice(NVNet_BASE, PAGE_SIZE, VMAType::IO_DeviceNVNet);

	// Map the bios
	MapHardwareDevice(BIOS_BASE, BIOS_CHIHIRO_SIZE - PAGE_SIZE, VMAType::DeviceBIOS);

	// Map the mcpx
	// NOTE: Again, I can't use MCPX_BASE and MCPX_SIZE because those are not aligned
	MapHardwareDevice(MAX_VIRTUAL_ADDRESS - PAGE_SIZE + 1, PAGE_SIZE, VMAType::DeviceMCPX);

	if (g_bIsChihiro || g_bIsDebug)
	{
		UnmapRange(CONTIGUOUS_MEMORY_BASE + m_MaxContiguousAddress);

		// Allocate the nv2a instance memory and the memory holding the PFN database
		VMAIter upper_mem_vma_handle = CarveVMA(CONTIGUOUS_MEMORY_BASE + m_MaxContiguousAddress, 48 * PAGE_SIZE);
		VirtualMemoryArea& upper_mem_vma = upper_mem_vma_handle->second;
		upper_mem_vma.type = VMAType::Allocated;
		upper_mem_vma.permissions = PAGE_EXECUTE_READWRITE;
		upper_mem_vma.backing_block = AllocatePhysicalMemoryRange(48 * PAGE_SIZE, m_MaxContiguousAddress, CHIHIRO_MEMORY_SIZE);
		UpdatePageTableForVMA(upper_mem_vma);
		m_ImageMemoryInUse += 48 * PAGE_SIZE;

		// Map the tiled memory
		UnmapRange(TILED_MEMORY_BASE);
		MapHardwareDevice(TILED_MEMORY_BASE, TILED_MEMORY_CHIHIRO_SIZE, VMAType::MemTiled);

		// NOTE: we cannot just call Unmap on the mcpx region because its base + size will overflow to 0x100000000
		// which will trigger an assert in CarveVMARange
		m_Vma_map.lower_bound(MAX_VIRTUAL_ADDRESS - PAGE_SIZE + 1)->second.type = VMAType::Free;
		m_NonImageMemoryInUse -= PAGE_SIZE;

		// Map the bios
		UnmapRange(BIOS_BASE);
		MapHardwareDevice(BIOS_BASE, BIOS_CHIHIRO_SIZE, VMAType::DeviceBIOS);
	}

	if (g_bIsChihiro) {
		printf("Page table for Chihiro initialized!\n");
	}
	else if (g_bIsDebug) {
		printf("Page table for Debug console initialized!\n");
	}
	else { printf("Page table for Retail console initialized!\n"); }
}

void VMManager::MapHardwareDevice(VAddr base, size_t size, VMAType type)
{
	VMAIter vma_handle = CarveVMA(base, size);
	VirtualMemoryArea& vma = vma_handle->second;
	vma.type = type;
	UpdatePageTableForVMA(vma);
	m_NonImageMemoryInUse += size;
}

void VMManager::MemoryStatistics(xboxkrnl::PMM_STATISTICS memory_statistics)
{
	memory_statistics->TotalPhysicalPages = m_MaxPhysicalMemory / PAGE_SIZE;
	memory_statistics->AvailablePages = (m_MaxPhysicalMemory - m_PhysicalMemoryInUse) / PAGE_SIZE;
	memory_statistics->VirtualMemoryBytesCommitted = m_ImageMemoryInUse + m_NonImageMemoryInUse;
	memory_statistics->VirtualMemoryBytesReserved = 0; // this is the num of bytes reserved with MEM_RESERVE by NtAllocateVirtualMemory
	memory_statistics->CachePagesCommitted = 0; // not implemented
	memory_statistics->PoolPagesCommitted = 0; // not implemented
	memory_statistics->StackPagesCommitted = m_StackMemoryInUse;
	memory_statistics->ImagePagesCommitted = m_ImageMemoryInUse;
}

VAddr VMManager::Allocate(size_t size, PAddr low_addr, PAddr high_addr, ULONG Alignment, DWORD protect, bool bNonContiguous)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(size);
		LOG_FUNC_ARG(low_addr);
		LOG_FUNC_ARG(high_addr);
		LOG_FUNC_ARG(Alignment);
		LOG_FUNC_ARG(protect);
		LOG_FUNC_ARG(bNonContiguous);
	LOG_FUNC_END;

	Lock();
	size_t ReturnedSize = size;
	VAddr v_addr = MapMemoryBlock(&ReturnedSize, low_addr, high_addr, Alignment, bNonContiguous);
	if (v_addr)
	{
		ReprotectVMARange(v_addr, ReturnedSize, protect);
		protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY) ? 
			m_ImageMemoryInUse += ReturnedSize : m_NonImageMemoryInUse += ReturnedSize;
	}
	Unlock();

	RETURN(v_addr);
}

VAddr VMManager::AllocateZeroed(size_t size)
{
	LOG_FUNC_ONE_ARG(size);

	Lock();
	size_t ReturnedSize = size;
	VAddr v_addr = MapMemoryBlock(&ReturnedSize, 0, MAXULONG_PTR);
	if (v_addr)
	{
		ReprotectVMARange(v_addr, ReturnedSize, PAGE_EXECUTE_READWRITE);
		m_ImageMemoryInUse += ReturnedSize;

		memset((void*)v_addr, 0, ReturnedSize);
	}
	Unlock();

	RETURN(v_addr);
}

VAddr VMManager::AllocateStack(size_t size)
{
	LOG_FUNC_ONE_ARG(size);

	Lock();
	size_t ReturnedSize = size + PAGE_SIZE;
	VAddr v_addr = MapMemoryBlock(&ReturnedSize, 0, MAXULONG_PTR);
	if (v_addr)
	{
		m_Vma_map.lower_bound(v_addr)->second.type = VMAType::Stack;
		ReprotectVMARange(v_addr, PAGE_SIZE, PAGE_NOACCESS); // guard page of the stack
		v_addr += ReturnedSize;
		m_StackMemoryInUse += ReturnedSize;
	}
	Unlock();

	RETURN(v_addr);
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
	UnmapRange(addr);
	Unlock();
}

void VMManager::Protect(VAddr target, size_t size, DWORD new_perms)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(target);
		LOG_FUNC_ARG(size);
		LOG_FUNC_ARG(new_perms);
	LOG_FUNC_END;

	Lock();
	ReprotectVMARange(target, size, new_perms);
	Unlock();
}

bool VMManager::QueryVAddr(VAddr addr)
{
	LOG_FUNC_ONE_ARG(addr);

	Lock();
	bool bValid = IsValidVirtualAddress(addr);
	Unlock();

	RETURN(bValid);
}

PAddr VMManager::TranslateVAddr(VAddr addr)
{
	LOG_FUNC_ONE_ARG(addr);

	Lock();
	PAddr p_addr = TranslateVAddrToPAddr(addr);
	Unlock();

	RETURN(p_addr);
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

	RETURN(protect);
}

size_t VMManager::QuerySize(VAddr addr)
{
	LOG_FUNC_ONE_ARG(addr);

	// The Xbox returns at least PAGE_SIZE even for invalid addresses
	size_t size = PAGE_SIZE;

	Lock();
	auto it = m_Vma_map.lower_bound(addr);
	if (it != m_Vma_map.end())
	{
		if (it->second.type == VMAType::Free)
		{
			size = 0;
			EmuWarning("VMManager: QuerySize : queried a free region!\n");
		}
		else
		{
			if (it->second.base != addr)
			{
				// This shouldn't happen for MmQueryAllocationSize, but if this function is called by other callers then it's possible
				auto prev_it = std::prev(it);
				PAddr prev_backing_block = prev_it->second.backing_block;
				while (prev_it != m_Vma_map.begin() && prev_backing_block == prev_it->second.backing_block)
				{
					--prev_it;
				}
				it = std::next(prev_it);
				EmuWarning("VMManager: QuerySize : quering not the start address of an allocation\n");
			}
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
	}
	Unlock();

	RETURN(size);
}

xboxkrnl::NTSTATUS VMManager::XbAllocateVirtualMemory(VAddr* addr, ULONG zero_bits, size_t* size, DWORD allocation_type,
	DWORD protect, bool bStub)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(*addr);
		LOG_FUNC_ARG(zero_bits);
		LOG_FUNC_ARG(*size);
		LOG_FUNC_ARG(allocation_type);
		LOG_FUNC_ARG(protect);
		LOG_FUNC_ARG(bStub);
	LOG_FUNC_END;

	VAddr CapturedBase = *addr;
	size_t CapturedSize = *size;
	xboxkrnl::NTSTATUS ret = STATUS_SUCCESS;

	// Invalid base address
	if (CapturedBase > HIGHEST_VAD_ADDRESS) { ret = STATUS_INVALID_PARAMETER; RETURN(ret); }

	// Invalid region size
	if (((HIGHEST_VAD_ADDRESS + 1) - CapturedBase) < CapturedSize) { ret = STATUS_INVALID_PARAMETER; RETURN(ret); }

	// Size cannot be zero
	if (CapturedSize == 0) { ret = STATUS_INVALID_PARAMETER; RETURN(ret); }

	VAddr AlignedCapturedBase = CapturedBase & ~PAGE_MASK;
	size_t AlignedCapturedSize = (CapturedSize + PAGE_MASK) & ~PAGE_MASK;

	Lock();

	if (bStub)
	{
		// The stub doesn't allocate anything, but the following is useful as reference
		// WARNING: because there are no allocations being done, it can't know if there is overlap and so the memory statistics
		// can be wrong in that case (this doesn't happen in XeLoadSection since it checks for the overlap with the head/tail counters)

		// REMARK: the following assumes that there is only one VMAType::Free between VAddr and VAddr + size. This is fine for XeUnloadSection,
		// not so for NtAllocateVirtualMemory. For that, an approch similar to UnmapRange (in particular VMAType::Lock) should be used.

		/*VMAIter vma_handle = GetVMAIterator(AlignedCapturedBase);

		// base address is outside the range managed by the kernel
		assert(vma_handle != m_Vma_map.end());

		if (vma_handle->second.type == VMAType::Allocated || vma_handle->second.type == VMAType::Stack)
		{
			// region is overlapped (base must lie inside the allocated vma)
			assert(AlignedCapturedBase < vma_handle->second.base + vma_handle->second.size);

			size_t overlapped_size = vma_handle->second.base + vma_handle->second.size - AlignedCapturedBase;
			if (AlignedCapturedBase + AlignedCapturedSize <= vma_handle->second.base + vma_handle->second.size)
			{
				// region is totally inside the existing allocation, so there's nothing new to commit. Bail out now
				*addr = AlignedCapturedBase;
				*size = AlignedCapturedSize;
				Unlock();
				RETURN(ret);
			}

			auto next_vma = std::next(vma_handle);
			if (next_vma->second.base + AlignedCapturedSize - overlapped_size > std::next(next_vma)->first)
			{
				AlignedCapturedSize = std::next(next_vma)->first - next_vma->first;
			}
			else
			{
				AlignedCapturedSize -= overlapped_size;
			}
		}*/

		// Update the memory manager statistics
		if (AlignedCapturedSize > m_MaxContiguousAddress - m_PhysicalMemoryInUse)
		{
			*addr = AlignedCapturedBase;
			*size = 0;
			ret = STATUS_NO_MEMORY;
			Unlock();
			RETURN(ret);
		}
		m_PhysicalMemoryInUse += AlignedCapturedSize;
		protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY) ?
			m_ImageMemoryInUse += AlignedCapturedSize : m_NonImageMemoryInUse += AlignedCapturedSize;

		*addr = AlignedCapturedBase;
		*size = AlignedCapturedSize;
	}
	else
	{
		// TODO: implement the real function
		LOG_UNIMPLEMENTED();
	}
	Unlock();
	RETURN(ret);
}

xboxkrnl::NTSTATUS VMManager::XbFreeVirtualMemory(VAddr* addr, size_t* size, DWORD free_type, bool bStub)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(*addr);
		LOG_FUNC_ARG(*size);
		LOG_FUNC_ARG(free_type);
		LOG_FUNC_ARG(bStub);
	LOG_FUNC_END;

	LOG_INCOMPLETE();

	VAddr CapturedBase = *addr;
	size_t CapturedSize = *size;
	xboxkrnl::NTSTATUS ret = STATUS_SUCCESS;

	// Only MEM_DECOMMIT and MEM_RELEASE are valid
	if ((free_type & ~(MEM_DECOMMIT | MEM_RELEASE)) != 0) { ret = STATUS_INVALID_PARAMETER; RETURN(ret); }

	// MEM_DECOMMIT and MEM_RELEASE must not be specified together
	if (((free_type & (MEM_DECOMMIT | MEM_RELEASE)) == 0) ||
		((free_type & (MEM_DECOMMIT | MEM_RELEASE)) == (MEM_DECOMMIT | MEM_RELEASE))) {
		ret = STATUS_INVALID_PARAMETER;
		RETURN(ret);
	}

	// Invalid base address
	if (CapturedBase > HIGHEST_USER_ADDRESS) { ret = STATUS_INVALID_PARAMETER; RETURN(ret); }

	// Invalid region size
	if ((HIGHEST_USER_ADDRESS - CapturedBase) < CapturedSize) { ret = STATUS_INVALID_PARAMETER; RETURN(ret); }

	VAddr AlignedCapturedBase = CapturedBase & ~PAGE_MASK;
	size_t AlignedCapturedSize = (CapturedSize + PAGE_MASK) & ~PAGE_MASK;

	Lock();

	if (bStub)
	{
		// This was an allocation that didn't actually allocate anything, so just update the memory usage
		m_PhysicalMemoryInUse -= AlignedCapturedSize;
		m_ImageMemoryInUse -= AlignedCapturedSize; // this should check the permissions of the region but for XeLoadSection it's always PAGE_EXECUTE_READWRITE
	}
	else
	{
		auto it = m_Vma_map.lower_bound(AlignedCapturedBase);

		VAddr EndingAddress = AlignedCapturedBase + AlignedCapturedSize;
		size_t overlapped_size_start = std::prev(it)->second.base + std::prev(it)->second.size - AlignedCapturedBase;
		VirtualMemoryArea start_vma;
		VirtualMemoryArea end_vma;
		start_vma.base = AlignedCapturedBase;
		start_vma.type = VMAType::Lock;
		start_vma.size = overlapped_size_start;
		ResizeVMA(std::prev(it), overlapped_size_start, false);
		auto low_it = m_Vma_map.emplace(AlignedCapturedBase, start_vma).first;
		auto high_pair = m_Vma_map.emplace(EndingAddress, end_vma);

		if (high_pair.second)
		{
			size_t overlapped_size_end = EndingAddress - std::prev(high_pair.first)->first;
			end_vma.base = EndingAddress;
			end_vma.size = overlapped_size_end;
			end_vma.type = VMAType::Lock;
			ResizeVMA(std::prev(high_pair.first), overlapped_size_end, true);
		}
		else
		{
			end_vma.type = high_pair.first->second.type; // backup the existing vma type
			high_pair.first->second.type = VMAType::Lock;
		}

		auto start_it = std::next(low_it); // skip the first locked vma
		while (start_it != high_pair.first)
		{
			start_it = DestructVMA(start_it, start_it->second.base, start_it->second.size);
		}

		if (high_pair.second)
		{
			low_it->second.type = VMAType::Free;
			high_pair.first->second.type = VMAType::Free;
			MergeAdjacentVMA(std::prev(start_it));
		}
		else
		{
			low_it->second.type = VMAType::Free;
			start_it->second.type = end_vma.type; // restore previously saved vma type
			MergeAdjacentVMA(std::prev(start_it));
		}
	}

	*addr = AlignedCapturedBase;
	*size = AlignedCapturedSize;
	Unlock();
	RETURN(ret);
}

VAddr VMManager::MapMemoryBlock(size_t* size, PAddr low_addr, PAddr high_addr, ULONG Alignment, bool bNonContiguous)
{
	// Find a free memory block for the allocation, if any
	VAddr addr;
	u32 offset;
	size_t aligned_size = (*size + PAGE_MASK) & ~PAGE_MASK;

	if (high_addr == MAXULONG_PTR)
	{
		offset = AllocatePhysicalMemory(aligned_size);
	}
	else
	{
		PAddr aligned_low = low_addr & ~(UINT_PTR)(Alignment - 1);
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
			if (bNonContiguous) {
				addr = m_Base + offset;
			}
			else { addr = CONTIGUOUS_MEMORY_BASE + offset; } // VAddr is simply the offset from the base of the contiguous memory
			
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
			addr = offset; // VAddr is the aligned address returned by VirtualAlloc

			VMAIter vma_handle = CarveVMA(addr, aligned_size);

			VirtualMemoryArea& final_vma = vma_handle->second;
			final_vma.type = VMAType::Allocated;
			final_vma.bFragmented = true;
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

	*size = aligned_size;
	return addr;
}

void VMManager::UnmapRange(VAddr target)
{
	VAddr aligned_start = target & ~(UINT_PTR)PAGE_MASK;

	auto it = m_Vma_map.lower_bound(aligned_start);
	
	if (it->second.type == VMAType::Free || it->first != aligned_start) {
		CxbxKrnlCleanup("An attempt to deallocate a region not allocated by the manager has been detected!"); 
	}

	DestructVMA(it, aligned_start, it->second.size);
}

void VMManager::ReprotectVMARange(VAddr target, size_t size, DWORD new_perms)
{
	VAddr aligned_start = target & ~(UINT_PTR)PAGE_MASK;
	size_t aligned_size = (size + PAGE_MASK) & ~PAGE_MASK;

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
	assert(target_end <= MAX_VIRTUAL_ADDRESS);
	assert(size > 0);

	VMAIter begin_vma = GetVMAIterator(base);
	VMAIter it_end = m_Vma_map.lower_bound(target_end);
	for (auto i = begin_vma; i != it_end; ++i)
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

VMManager::VMAIter VMManager::MergeAdjacentVMA(VMAIter vma_handle)
{
	VMAIter next_vma = std::next(vma_handle);
	if (next_vma != m_Vma_map.end() && vma_handle->second.CanBeMergedWith(next_vma->second))
	{
		vma_handle->second.size += next_vma->second.size;
		m_Vma_map.erase(next_vma);
	}

	if (vma_handle != m_Vma_map.begin())
	{
		VMAIter prev_vma = std::prev(vma_handle);
		if (prev_vma->second.CanBeMergedWith(vma_handle->second))
		{
			prev_vma->second.size += vma_handle->second.size;
			m_Vma_map.erase(vma_handle);
			vma_handle = prev_vma;
		}
	}

	return vma_handle;
}

VMManager::VMAIter VMManager::ReprotectVMA(VMAIter vma_handle, DWORD new_perms)
{
	// PAGE_WRITECOMBINE/PAGE_NOCACHE are not allowed for shared memory, unless SEC_WRITECOMBINE/SEC_NOCACHE flag 
	// was specified when calling the CreateFileMapping function. Considering that Cxbx doesn't emulate the caches,
	// it's probably safe to ignore these flags

	VirtualMemoryArea& vma = vma_handle->second;
	DWORD dummy;
	vma.permissions = new_perms;
	if (!VirtualProtect((void*)vma.base, vma.size, vma.permissions & ~(PAGE_WRITECOMBINE | PAGE_NOCACHE), &dummy))
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
		case VMAType::Stack:
		{
			MapMemoryRegion(vma.base, vma.size, vma.backing_block);
		}
		break;

		default:
			CxbxKrnlCleanup("VMAType::Lock or Unknown type in UpdatePageTableForVMA");
	}
}

VMManager::VMAIter VMManager::DestructVMA(VMAIter vma_handle, VAddr addr, size_t size)
{
	if (vma_handle->second.type == VMAType::Free) { return std::next(vma_handle); }


	if (vma_handle->second.type != VMAType::Stack)
	{
		vma_handle->second.permissions & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY) ?
			m_ImageMemoryInUse -= size : m_NonImageMemoryInUse -= size;
	}
	else { m_StackMemoryInUse -= size; }


	if (vma_handle->second.type == VMAType::Allocated || vma_handle->second.type == VMAType::Stack)
	{
		if (vma_handle->second.bFragmented) { DeAllocateFragmented(vma_handle->second.backing_block); }
		else { DeAllocatePhysicalMemory(vma_handle->second.backing_block); }
	}

	VMAIter vma = CarveVMARange(addr, size);

	VAddr target_end = addr + size;
	VMAIter end = m_Vma_map.end();
	// The comparison against the end of the range must be done using addresses since vma's can be
	// merged during this process, causing invalidation of the iterators
	while (vma != end && vma->second.base < target_end)
	{
		vma = std::next(Unmap(vma));
	}

	return vma;
}

void VMManager::ResizeVMA(VMAIter vma_handle, size_t offset, bool bStart)
{
	if (!offset) { return; } // nothing to do

	VirtualMemoryArea& old_vma = vma_handle->second;
	VirtualMemoryArea new_vma = old_vma;

	if (bStart)
	{
		if (offset > old_vma.size) { return; } // sanity check
		VAddr new_base = old_vma.base + offset;
		new_vma.base = new_base;
		new_vma.size = old_vma.size - offset;

		if (old_vma.type == VMAType::Allocated || old_vma.type == VMAType::Stack) {
			ShrinkPhysicalAllocation(vma_handle->second.backing_block, offset, vma_handle->second.bFragmented, bStart);
		}
		m_Vma_map.erase(old_vma.base);
		if(new_vma.size) { m_Vma_map.emplace(new_base, new_vma); }
	}
	else
	{
		if (offset > old_vma.size) { return; } // sanity check
		VAddr new_base = old_vma.base;
		new_vma.base = new_base;
		new_vma.size = old_vma.size - offset;

		if (old_vma.type == VMAType::Allocated || old_vma.type == VMAType::Stack) {
			ShrinkPhysicalAllocation(vma_handle->second.backing_block, offset, vma_handle->second.bFragmented, bStart);
		}
		m_Vma_map.erase(old_vma.base);
		if (new_vma.size) { m_Vma_map.emplace(new_base, new_vma); }
	}

	if (new_vma.type != VMAType::Free)
	{
		if (new_vma.type != VMAType::Stack)
		{
			new_vma.permissions & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY) ?
				m_ImageMemoryInUse -= offset : m_NonImageMemoryInUse -= offset;
		}
		else { m_StackMemoryInUse -= offset; }
	}
}
