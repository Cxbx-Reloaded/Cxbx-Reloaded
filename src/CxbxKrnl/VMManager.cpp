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

#define LOG_PREFIX "VMEM"

#include "VMManager.h"
#include "Logging.h"
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

// Checks if any of the EXECUTE flags are set
inline bool HasPageExecutionFlag(DWORD protect)
{
	return protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY);
}

bool VirtualMemoryArea::CanBeMergedWith(const VirtualMemoryArea& next) const
{
	assert(base + size == next.base);

	if (permissions != next.permissions || vma_type != next.vma_type || page_type != next.page_type ||
		vma_type == VMAType::Lock || next.vma_type == VMAType::Lock) { return false; }
	if (vma_type == VMAType::Allocated && backing_block != next.backing_block) { return false; }

	return true;
}

void VMManager::Initialize(HANDLE file_view)
{
	// This reserves a large enough memory region to map the second physical memory file view
	UINT_PTR start = (UINT_PTR)VirtualAlloc(NULL, CHIHIRO_MEMORY_SIZE, MEM_RESERVE, PAGE_READWRITE);
	if (!start)
	{
		CxbxKrnlCleanup(LOG_PREFIX ": VirtualAlloc could not find a suitable region to allocate the second physical memory view!");
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
		if (m_Base)
			UnmapViewOfFile((void*)m_Base);

		CxbxKrnlCleanup(LOG_PREFIX ": MapViewOfFileEx could not map the second physical memory view!");
	}
	m_hAliasedView = file_view;

	m_Vma_map.clear();
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
	first_page_vma.page_type = PageType::SystemMemory;
	first_page_vma.base = ZERO_PAGE_ADDR;
	first_page_vma.size = PAGE_SIZE;
	first_page_vma.permissions = PAGE_GUARD;
	m_Vma_map.emplace(first_page_vma.base, first_page_vma);
	UpdatePageTableForVMA(first_page_vma);

	// D3D uses the first physical page to initialize the push buffer. At the moment, this doesn't seem to be emulated though
	Allocate(PAGE_SIZE, PageType::Contiguous, 0, PAGE_SIZE - 1, PAGE_SIZE, PAGE_READWRITE);

	// Allocate the nv2a instance memory and the memory holding the PFN database (the latter is not not emulated)
	// REMARK: I Can't simply call Allocate here since MapMemoryBlock checks if the high addr is higher than m_MaxContiguousAddress,
	// which is the case here, so we must call AllocatePhysicalMemoryRange directly to bypass the check
	VMAIter upper_mem_vma_handle = CarveVMA(CONTIGUOUS_MEMORY_BASE + m_MaxContiguousAddress, 32 * PAGE_SIZE);
	VirtualMemoryArea& upper_mem_vma = upper_mem_vma_handle->second;
	upper_mem_vma.vma_type = VMAType::Allocated;
	upper_mem_vma.page_type = PageType::SystemMemory;
	upper_mem_vma.permissions = PAGE_READWRITE;
	upper_mem_vma.backing_block = AllocatePhysicalMemoryRange(32 * PAGE_SIZE, upper_mem_vma.page_type, m_MaxContiguousAddress, XBOX_MEMORY_SIZE);
	UpdatePageTableForVMA(upper_mem_vma);

	// Allocate memory for the dummy kernel
	// NOTE: change PAGE_SIZE if the size of the dummy kernel increases!
	Allocate(KERNEL_SIZE, PageType::Contiguous, XBE_IMAGE_BASE, XBE_IMAGE_BASE + PAGE_SIZE - 1, KERNEL_SIZE & ~PAGE_MASK, PAGE_EXECUTE_READWRITE);

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

	printf("Page table initialized!\n");
}

void VMManager::InitializeChihiroDebug()
{
	UnmapRange(CONTIGUOUS_MEMORY_BASE + m_MaxContiguousAddress);
	m_MaxContiguousAddress = CHIHIRO_CONTIGUOUS_MEMORY_LIMIT;
	m_MaxPhysicalMemory = CHIHIRO_MEMORY_SIZE;

	// Allocate the nv2a instance memory and the memory holding the PFN database (the latter is not not emulated)	
	VMAIter upper_mem_vma_handle = CarveVMA(CONTIGUOUS_MEMORY_BASE + m_MaxContiguousAddress, 48 * PAGE_SIZE);
	VirtualMemoryArea& upper_mem_vma = upper_mem_vma_handle->second;
	upper_mem_vma.vma_type = VMAType::Allocated;
	upper_mem_vma.page_type = PageType::SystemMemory;
	upper_mem_vma.permissions = PAGE_READWRITE;
	upper_mem_vma.backing_block = AllocatePhysicalMemoryRange(48 * PAGE_SIZE, upper_mem_vma.page_type, m_MaxContiguousAddress, CHIHIRO_MEMORY_SIZE);
	UpdatePageTableForVMA(upper_mem_vma);

	// Map the tiled memory
	UnmapRange(TILED_MEMORY_BASE);
	MapHardwareDevice(TILED_MEMORY_BASE, TILED_MEMORY_CHIHIRO_SIZE, VMAType::MemTiled);

	// NOTE: we cannot just call Unmap on the mcpx region because its base + size will overflow to 0x100000000
	// which will trigger an assert in CarveVMARange
	auto it = m_Vma_map.lower_bound(MAX_VIRTUAL_ADDRESS - PAGE_SIZE + 1);

	// The Xbox initialization always maps the mcpx at MAX_VIRTUAL_ADDRESS - PAGE_SIZE + 1,
	// so lower_bound should always succeed here
	assert(it != m_Vma_map.end());

	it->second.vma_type = VMAType::Free;
	it->second.page_type = PageType::Unknown;

	// Map the bios
	UnmapRange(BIOS_BASE);
	MapHardwareDevice(BIOS_BASE, BIOS_CHIHIRO_SIZE, VMAType::DeviceBIOS);

	if (g_bIsChihiro) {
		printf("Page table for Chihiro initialized!\n");
	}
	else { printf("Page table for Debug console initialized!\n"); }
}

void VMManager::MapHardwareDevice(VAddr base, size_t size, VMAType vma_type)
{
	Lock();
	VMAIter vma_handle = CarveVMA(base, size);
	VirtualMemoryArea& vma = vma_handle->second;
	vma.vma_type = vma_type;
	vma.page_type = PageType::SystemMemory;
	UpdatePageTableForVMA(vma);
	// Note : On a real Xbox, hardware MMIO address ranges aren't
	// backed by physical memory, so we don't count pages either.
	Unlock();
}

void VMManager::MemoryStatistics(xboxkrnl::PMM_STATISTICS memory_statistics)
{
	Lock();
	memory_statistics->TotalPhysicalPages = m_MaxPhysicalMemory / PAGE_SIZE;
	memory_statistics->AvailablePages = (m_MaxPhysicalMemory - m_PhysicalMemoryInUse) / PAGE_SIZE;
	memory_statistics->VirtualMemoryBytesCommitted = (m_PageCount[(int)PageType::VirtualMemory] + m_PageCount[(int)PageType::Image]) * PAGE_SIZE;
	memory_statistics->VirtualMemoryBytesReserved = m_VirtualMemoryBytesReserved;
	memory_statistics->CachePagesCommitted = m_PageCount[(int)PageType::Cache];
	memory_statistics->PoolPagesCommitted = m_PageCount[(int)PageType::Pool];
	memory_statistics->StackPagesCommitted = m_PageCount[(int)PageType::Stack];
	memory_statistics->ImagePagesCommitted = m_PageCount[(int)PageType::Image];
	Unlock();
}

VAddr VMManager::Allocate(size_t size, PageType page_type, PAddr low_addr, PAddr high_addr, ULONG alignment, DWORD protect)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(size);
		LOG_FUNC_ARG_TYPE(int, page_type); // TODO : Add a logging trait
		LOG_FUNC_ARG(low_addr);
		LOG_FUNC_ARG(high_addr);
		LOG_FUNC_ARG(alignment);
		LOG_FUNC_ARG(protect); // TODO : LOG_FUNC_ARG_TYPE(PROTECTION_TYPE, protect);
	LOG_FUNC_END;

	if (size <= 0) {
		EmuWarning(LOG_PREFIX ": Allocate : Request for zero bytes!");
		RETURN(0);
	}

	if (alignment == 0) {
		EmuWarning(LOG_PREFIX ": Allocate : Request with zero alignment, using PAGE_SIZE instead.");
		alignment = PAGE_SIZE;
	}

	// Treat VirtualMemory pages with execute rights distinctly as Image pages :
	if (page_type == PageType::VirtualMemory && HasPageExecutionFlag(protect)) {
		page_type = PageType::Image;
	}

	Lock();
	size_t ReturnedSize = size;
	VAddr v_addr = MapMemoryBlock(&ReturnedSize, page_type, low_addr, high_addr, alignment);
	if (v_addr)
	{
		ReprotectVMARange(v_addr, ReturnedSize, protect);
	}
	Unlock();

	RETURN(v_addr);
}

VAddr VMManager::AllocateZeroed(size_t size)
{
	LOG_FUNC_ONE_ARG(size);

	assert(size > 0);

	Lock();
	size_t ReturnedSize = size;
	VAddr v_addr = MapMemoryBlock(&ReturnedSize, PageType::VirtualMemory);
	if (v_addr)
	{
		ReprotectVMARange(v_addr, ReturnedSize, PAGE_READWRITE);
		memset((void*)v_addr, 0, ReturnedSize);
	}
	Unlock();

	RETURN(v_addr);
}

VAddr VMManager::AllocateStack(size_t size)
{
	LOG_FUNC_ONE_ARG(size);

	assert(size > 0); // Size must be given
	assert((size & PAGE_MASK) == 0); // Size must be expressed in pages

	Lock();
	size_t ReturnedSize = size + PAGE_SIZE;
	VAddr v_addr = MapMemoryBlock(&ReturnedSize, PageType::Stack);
	if (v_addr)
	{
		auto it = m_Vma_map.lower_bound(v_addr);
		assert(it != m_Vma_map.end());

		it->second.vma_type = VMAType::Stack;
		it->second.page_type = PageType::Stack;
		ReprotectVMARange(v_addr, PAGE_SIZE, PAGE_NOACCESS); // guard page of the stack
		ReprotectVMARange(v_addr + PAGE_SIZE, size, PAGE_READWRITE); // actual stack pages
		v_addr += ReturnedSize;
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
	// UnmapRange will only dellocate a single vma and merge it with the next and the previous if possible.
	// This means that only the StackBottom page will be dellocated since the Stack VMA was split in two
	// by ReprotecVMARange when the stack was created.
	// For example, a stack allocation of 16384 bytes will lead to two vma:
	// 1- base StackBottom, 4096, PAGE_NOACCESS,
	// 2- base StackBottom + PAGE_SIZE, 16384, PAGE_READWRITE.
	// ReprotectVMARange will merge them back to a single VMA that UnmapRange will deallocate entirely.
	// Test case : Tested with Dead or Alive 3, which calls this from CreateFiber
	ReprotectVMARange(addr, PAGE_SIZE, PAGE_READWRITE);
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
		if (it->second.vma_type == VMAType::Free)
		{
			size = 0;
			EmuWarning(LOG_PREFIX ": QuerySize : queried a free region!");
		}
		else
		{
			if (it->second.base != addr)
			{
				// This shouldn't happen for MmQueryAllocationSize, but if this function is called by other callers then it's possible
				auto prev_it = std::prev(it);
				PAddr prev_backing_block = prev_it->second.backing_block;
				const auto it_begin = m_Vma_map.begin();
				while (prev_it != it_begin && prev_backing_block == prev_it->second.backing_block)
				{
					--prev_it;
				}
				it = std::next(prev_it);
				//EmuWarning(LOG_PREFIX ": QuerySize : querying not the start address of an allocation");
			}
			// We can't just return the size of the vma because it could have been split by ReprotectVMARange so, instead,
			// we must check the corresponding physical allocation size
			size = it->second.size;
			auto next_it = std::next(it);
			const auto end = m_Vma_map.end();
			while (next_it != end && it->second.backing_block == next_it->second.backing_block)
			{
				size += next_it->second.size;
				++next_it;
			}
		}
	}
	Unlock();

	RETURN(size);
}

// prevent name collisions
namespace NtDll
{
#include "EmuNtDll.h" // TODO : Remove dependancy once NtDll::NtAllocateVirtualMemory is gone again
};

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

	assert(addr); // addr must be assigned
	VAddr CapturedBase = *addr;

	assert(size); // size must be assigned
	size_t CapturedSize = *size;

	xboxkrnl::NTSTATUS ret = STATUS_SUCCESS;

	Lock();

	if (bStub)
	{
		// Invalid base address
		if (CapturedBase > HIGHEST_VAD_ADDRESS) { ret = STATUS_INVALID_PARAMETER; goto failed; }

		// Invalid region size
		if (((HIGHEST_VAD_ADDRESS + 1) - CapturedBase) < CapturedSize) { ret = STATUS_INVALID_PARAMETER; goto failed; }

		// Size cannot be zero
		if (CapturedSize == 0) { ret = STATUS_INVALID_PARAMETER; goto failed; }

		VAddr AlignedCapturedBase = CapturedBase & ~PAGE_MASK;
		size_t AlignedCapturedSize = (CapturedSize + PAGE_MASK) & ~PAGE_MASK;

		// The stub doesn't allocate anything, but the following is useful as reference
		// WARNING: because there are no allocations being done, it can't know if there is overlap and so the memory statistics
		// can be wrong in that case (this doesn't happen in XeLoadSection since it checks for the overlap with the head/tail counters)

		// REMARK: the following assumes that there is only one VMAType::Free between VAddr and VAddr + size. This is fine for XeUnloadSection,
		// not so for NtAllocateVirtualMemory. For that, an approch similar to UnmapRange (in particular VMAType::Lock) should be used.

		/*VMAIter vma_handle = GetVMAIterator(AlignedCapturedBase);

		// base address is outside the range managed by the kernel
		assert(vma_handle != m_Vma_map.end());

		if (vma_handle->second.vma_type == VMAType::Allocated || vma_handle->second.vma_type == VMAType::Stack)
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
			// TODO : Can return-values be ignored when this failure-case is hit?
			*addr = AlignedCapturedBase;
			*size = 0;
			ret = STATUS_NO_MEMORY;
			goto failed;
		}
		else
		{
			CapturedBase = AlignedCapturedBase;
			CapturedSize = AlignedCapturedSize;
			ret = STATUS_SUCCESS;
		}
	}
	else
	{
		// This is the not-bStub side of XbAllocateVirtualMemory, which (for now) forwards to Windows.

		// TODO: The flag known as MEM_NOZERO (which appears to be exclusive to Xbox) has the exact
		// same value as MEM_ROTATE which causes problems for Windows XP, but not Vista.  Removing
		// this flag fixes Azurik for XP.
		if (allocation_type & MEM_NOZERO)
			EmuWarning("MEM_NOZERO flag is not supported!");

		ret = NtDll::NtAllocateVirtualMemory(
			/*ProcessHandle=*/g_CurrentProcessHandle,
			(NtDll::PVOID *)&CapturedBase,
			zero_bits,
			/*RegionSize=*/(PULONG)(&CapturedSize),
			allocation_type & ~MEM_NOZERO,
			protect);

		// TODO: implement the real function
		LOG_UNIMPLEMENTED();
	}

	if (NT_SUCCESS(ret))
	{
		m_VirtualMemoryBytesReserved += CapturedSize;
		if (protect & MEM_COMMIT)
		{
			m_PhysicalMemoryInUse += CapturedSize;
			PageType page_type = HasPageExecutionFlag(protect) ? PageType::Image : PageType::VirtualMemory;
			m_PageCount[(int)page_type] += CapturedSize / PAGE_SIZE;
		}

		*addr = CapturedBase;
		*size = CapturedSize;
	}
failed:
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

	assert(addr); // Callers must pass in an assigned `addr`
	VAddr CapturedBase = *addr;

	assert(size); // Callers must pass in an assigned `size`
	size_t CapturedSize = *size;

	xboxkrnl::NTSTATUS ret = STATUS_SUCCESS;

	// Only MEM_DECOMMIT and MEM_RELEASE are valid
	if ((free_type & ~(MEM_DECOMMIT | MEM_RELEASE)) != 0) { ret = STATUS_INVALID_PARAMETER; RETURN(ret); }

	// MEM_DECOMMIT and MEM_RELEASE must not be specified together
	// TODO : Perhaps this is wrong - see https://msdn.microsoft.com/en-us/library/windows/hardware/ff566416(v=vs.85).aspx
	if (((free_type & (MEM_DECOMMIT | MEM_RELEASE)) == 0) ||
		((free_type & (MEM_DECOMMIT | MEM_RELEASE)) == (MEM_DECOMMIT | MEM_RELEASE))) {
		ret = STATUS_INVALID_PARAMETER;
		RETURN(ret);
	}

	// Invalid base address
	// TODO : Check validity differently (use m_Vma_map)
	if (CapturedBase > HIGHEST_USER_ADDRESS) { ret = STATUS_INVALID_PARAMETER; RETURN(ret); }

	VAddr AlignedCapturedBase = CapturedBase & ~PAGE_MASK;
	size_t AlignedCapturedSize = (CapturedSize + PAGE_MASK) & ~PAGE_MASK; // TODO : Retrieve allocation size from use m_Vma_map using AlignedCapturedBase

	Lock();

	if (bStub)
	{
		// This was an allocation that didn't actually allocate anything, so just update the memory usage
		ret = STATUS_SUCCESS;
	}
	else
	{
#if 0
		auto it = m_Vma_map.lower_bound(AlignedCapturedBase);

		VAddr EndingAddress = AlignedCapturedBase + AlignedCapturedSize;
		auto prev_it = std::prev(it);
		size_t overlapped_size_start = prev_it->second.base + prev_it->second.size - AlignedCapturedBase;
		VirtualMemoryArea start_vma;
		VirtualMemoryArea end_vma;
		start_vma.base = AlignedCapturedBase;
		start_vma.vma_type = VMAType::Lock;
		start_vma.page_type = PageType::Unknown;
		start_vma.size = overlapped_size_start;
		ResizeVMA(prev_it, overlapped_size_start, false);
		auto low_it = m_Vma_map.emplace(AlignedCapturedBase, start_vma).first;
		auto high_pair = m_Vma_map.emplace(EndingAddress, end_vma);

		if (high_pair.second)
		{
			size_t overlapped_size_end = EndingAddress - std::prev(high_pair.first)->first;
			end_vma.base = EndingAddress;
			end_vma.size = overlapped_size_end;
			end_vma.vma_type = VMAType::Lock;
			end_vma.page_type = PageType::Unknown;
			ResizeVMA(std::prev(high_pair.first), overlapped_size_end, true);
		}
		else
		{
			end_vma.vma_type = high_pair.first->second.vma_type; // backup the existing vma type
			end_vma.page_type = high_pair.first->second.page_type; // backup the existing page type
			high_pair.first->second.vma_type = VMAType::Lock;
			high_pair.first->second.page_type = PageType::Unknown;
		}

		auto start_it = std::next(low_it); // skip the first locked vma
		while (start_it != high_pair.first)
		{
			start_it = DestructVMA(start_it, start_it->second.base, start_it->second.size);
		}

		if (high_pair.second)
		{
			low_it->second.vma_type = VMAType::Free;
			low_it->second.page_type = PageType::Unknown;
			high_pair.first->second.vma_type = VMAType::Free;
			high_pair.first->second.page_type = PageType::Unknown;
			MergeAdjacentVMA(std::prev(start_it));
		}
		else
		{
			low_it->second.vma_type = VMAType::Free;
			low_it->second.page_type = PageType::Unknown;
			start_it->second.vma_type = end_vma.vma_type; // restore previously saved vma type
			start_it->second.page_type = end_vma.page_type; // restore previously saved page type
			MergeAdjacentVMA(std::prev(start_it));
		}
#else
		// This is the not-bStub side of XbFreeVirtualMemory, which (for now) forwards to Windows.

		ret = NtDll::NtFreeVirtualMemory(
			/*ProcessHandle=*/g_CurrentProcessHandle,
			(NtDll::PVOID *)(&AlignedCapturedBase),
			/*RegionSize=*/(PULONG)(&AlignedCapturedSize),
			free_type);

		// TODO: implement the real function (see above?)
		LOG_INCOMPLETE();
#endif
	}

	if (NT_SUCCESS(ret))
	{
		m_VirtualMemoryBytesReserved -= AlignedCapturedSize;
		if (free_type & MEM_DECOMMIT)
		{
			m_PhysicalMemoryInUse -= AlignedCapturedSize;
			PageType page_type = PageType::Image; // TODO : Determine the actual PageType
			m_PageCount[(int)page_type] -= AlignedCapturedSize / PAGE_SIZE;
		}

		*addr = AlignedCapturedBase;
		*size = AlignedCapturedSize;
	}

	Unlock();
	RETURN(ret);
}

// VMManager private functions, all called within Lock()/UnLock()

VAddr VMManager::MapMemoryBlock(size_t* size, PageType page_type, PAddr low_addr, PAddr high_addr, ULONG Alignment)
{
	// Find a free memory block for the allocation, if any
	VAddr addr;
	u32 offset;
	size_t aligned_size = (*size + PAGE_MASK) & ~PAGE_MASK;

	if (low_addr == 0 && high_addr == MAXULONG_PTR)
	{
		offset = AllocatePhysicalMemory(aligned_size, page_type);
	}
	else
	{
		PAddr aligned_low = low_addr & ~(UINT_PTR)(Alignment - 1);
		PAddr aligned_high = (high_addr + PAGE_SIZE) & ~(UINT_PTR)(Alignment - 1);
		if (aligned_high > m_MaxContiguousAddress) { aligned_high = m_MaxContiguousAddress; }
		if (aligned_low > aligned_high) { aligned_low = aligned_high - PAGE_SIZE; }

		if (aligned_size > aligned_high - aligned_low) { return NULL; }

		offset = AllocatePhysicalMemoryRange(aligned_size, page_type, aligned_low, aligned_high);
	}

	switch (GetError())
	{
		case PMEMORY_SUCCESS:
		{
			switch (page_type) {
			case PageType::Contiguous: {
				addr = CONTIGUOUS_MEMORY_BASE + offset;  // VAddr is simply the offset from the base of the contiguous memory
				break;
			}
			default: { // PageType::VirtualMemory, etc
				addr = m_Base + offset;
				break;
			}
			}

			VMAIter vma_handle = CarveVMA(addr, aligned_size);

			VirtualMemoryArea& final_vma = vma_handle->second;
			final_vma.vma_type = VMAType::Allocated;
			final_vma.page_type = page_type;
			final_vma.permissions = DefaultPageTypeProtection(page_type);
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
			final_vma.vma_type = VMAType::Allocated;
			final_vma.page_type = page_type;
			final_vma.bFragmented = true;
			final_vma.permissions = DefaultPageTypeProtection(page_type);
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
	m_PageCount[(int)page_type] += aligned_size / PAGE_SIZE;
	return addr;
}

void VMManager::UnmapRange(VAddr target)
{
	VAddr aligned_start = target & ~(UINT_PTR)PAGE_MASK;

	auto it = m_Vma_map.lower_bound(aligned_start);
	
	if (it == m_Vma_map.end() || it->second.vma_type == VMAType::Free || it->first != aligned_start) {
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
	vma.vma_type = VMAType::Free;
	vma.page_type = PageType::Unknown;
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
	assert(vma.vma_type == VMAType::Free);

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
		if (i->second.vma_type == VMAType::Free) { assert(0); }
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
	switch (vma.vma_type)
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
			CxbxKrnlCleanup("VMAType::Lock or Unknown VMA type in UpdatePageTableForVMA");
	}
}

VMManager::VMAIter VMManager::DestructVMA(VMAIter vma_handle, VAddr addr, size_t size)
{
	if (vma_handle->second.vma_type == VMAType::Free) { return std::next(vma_handle); }

	m_PageCount[(int)vma_handle->second.page_type] -= size / PAGE_SIZE;

	if (vma_handle->second.vma_type == VMAType::Allocated || vma_handle->second.vma_type == VMAType::Stack)
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
	if (offset > old_vma.size) { return; } // sanity check

	if (old_vma.vma_type != VMAType::Free)
	{
		m_PageCount[(int)old_vma.page_type] -= old_vma.size / PAGE_SIZE;
	}

	VirtualMemoryArea new_vma = old_vma;
	if (bStart)
	{
		VAddr new_base = old_vma.base + offset;
		new_vma.base = new_base;
		new_vma.size = old_vma.size - offset;

		if (old_vma.vma_type == VMAType::Allocated || old_vma.vma_type == VMAType::Stack) {
			ShrinkPhysicalAllocation(vma_handle->second.backing_block, offset, vma_handle->second.bFragmented, bStart);
		}
		m_Vma_map.erase(old_vma.base);
		if(new_vma.size) { m_Vma_map.emplace(new_base, new_vma); }
	}
	else
	{
		VAddr new_base = old_vma.base;
		new_vma.base = new_base;
		new_vma.size = old_vma.size - offset;

		if (old_vma.vma_type == VMAType::Allocated || old_vma.vma_type == VMAType::Stack) {
			ShrinkPhysicalAllocation(vma_handle->second.backing_block, offset, vma_handle->second.bFragmented, bStart);
		}
		m_Vma_map.erase(old_vma.base);
		if (new_vma.size) { m_Vma_map.emplace(new_base, new_vma); }
	}

	if (new_vma.vma_type != VMAType::Free)
	{
		m_PageCount[(int)new_vma.page_type] += new_vma.size / PAGE_SIZE;
	}
}
