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
// *   Cxbx->Win32->CxbxKrnl->PhysicalMemory.cpp
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

#include "PhysicalMemory.h"
#include <assert.h>


void FillMemoryUlong(void* Destination, size_t Length, ULONG Long)
{
	assert(Length != 0);
	assert(Length % sizeof(ULONG) == 0);                 // Length must be a multiple of ULONG
	assert((uintptr_t)Destination % sizeof(ULONG) == 0); // Destination must be 4-byte aligned

	int NumOfRepeats = Length / sizeof(ULONG);
	ULONG* d = (ULONG*)Destination;

	for (int i = 0; i < NumOfRepeats; ++i)
	{
		d[i] = Long; // copy an ULONG at a time
	}
}

PMEMORY_STATUS PhysicalMemory::GetError() const
{
	return m_Status;
}

void PhysicalMemory::InitializePfnDatabase()
{
	PMMPTE pPde;
	MMPTE TempPte;
	XBOX_PFN TempPF;

	// Unmap the user virtual space (lower 2 GiB)
	for (pPde = GetPdeAddress(0); pPde < GetPdeAddress(CONTIGUOUS_MEMORY_BASE); ++pPde)
	{
		WRITE_ZERO_PTE(pPde);
	}

	// Map the WC (tiled?) memory
	TempPte.Default = ValidKernelPteBits;
	TempPte.Hardware.LargePage = 1;
	TempPte.Hardware.WriteThrough = 1;
	TempPte.Hardware.PFN = XBOX_WRITE_COMBINED_BASE >> PAGE_SHIFT;
	for (pPde = GetPdeAddress(XBOX_WRITE_COMBINED_BASE); pPde <= GetPdeAddress(XBOX_WRITE_COMBINE_END); ++pPde)
	{
		WRITE_PTE(pPde, TempPte);
		TempPte.Default += PAGE_SIZE_LARGE; // increase PFN
	}

	// Map the UC memory region
	TempPte.Hardware.PFN = XBOX_UNCACHED_BASE >> PAGE_SHIFT;
	DISABLE_CACHING(TempPte);
	for (pPde = GetPdeAddress(XBOX_UNCACHED_BASE); pPde <= GetPdeAddress(XBOX_UNCACHED_END); ++pPde)
	{
		WRITE_PTE(pPde, TempPte);
		TempPte.Default += PAGE_SIZE_LARGE; // increase PFN
	}

	// Construct the entries of the PFN database
	TempPF.Default = 0;
	TempPF.Busy.Busy = 1;
	TempPF.Busy.LockCount = LOCK_COUNT_MAXIMUM;
	TempPF.Busy.BusyType = Unknown;
	if (g_IsRetail) {
		FillMemoryUlong((void*)m_PfnDatabaseVAddress, X64KB, TempPF.Default); // Xbox: 64 KiB
	}
	else { FillMemoryUlong((void*)m_PfnDatabaseVAddress, X64KB * 2, TempPF.Default); } // Chihiro, Debug: 128 KiB

	// Insert in the free list all the entries from the bottom of memory up to the kernel
	InsertPageRangeInFreeList(0, CONVERT_CONTIGUOUS_PHYSICAL_TO_PFN(KERNEL_PHYSICAL_ADDRESS));

	size_t kernel_num_pages = ((KERNEL_SIZE + PAGE_MASK) & ~PAGE_MASK) / PAGE_SIZE; // one page at the moment
}

void PhysicalMemory::InsertPageRangeInFreeList(PFN_Number Pfn, PFN_Number PfnEndExcluded)
{
	// Note that the final pfn entry is not included
	for (; Pfn < PfnEndExcluded; ++Pfn)
	{
		InsertPageInFreeList(Pfn, false);
	}
}

void PhysicalMemory::InsertPageInFreeList(PFN_Number Pfn, bool InsertAtHead)
{
	if (g_IsRetail) { assert(Pfn <= XBOX_HIGHEST_PHYSICAL_PAGE); }
	else { assert(Pfn <= CHIHIRO_HIGHEST_PHYSICAL_PAGE); }


}

void PhysicalMemory::ReinitializePfnDatabase()
{

}

PAddr PhysicalMemory::AllocatePhysicalMemory(size_t size)
{
	PAddr addr = m_MaxContiguousAddress;
	ClearError();
	size_t FreeMemory = m_MaxContiguousAddress - m_PhysicalMemoryInUse;
	if (size > FreeMemory)
	{
		EmuWarning("Out of physical memory!");
		SetError(PMEMORY_INSUFFICIENT_MEMORY);
		return addr;
	}

	// Allocate the block wherever possible
	// This attempts to counter external fragmentation by allocating big blocks top-down and small blocks bottom-up
	if (size > m_AllocationThreshold)
	{
		if (m_Mem_map.empty())
		{
			addr = m_MaxContiguousAddress - size;
			m_Mem_map[addr] = size;
			m_PhysicalMemoryInUse += size;
		}
		else
		{
			// Allocate the block starting from the top of memory
			for (auto rit = m_Mem_map.rbegin(); ; ++rit)
			{
				if (std::next(rit) == m_Mem_map.rend())
				{
					if (rit->first >= size)
					{
						addr = rit->first - size;
						m_Mem_map[addr] = size;
						m_PhysicalMemoryInUse += size;
						break;
					}

					if (FreeMemory >= size) // fragmentation
					{
						addr = AllocateFragmented(size);
						break;
					}
				}

				// Reinstate this if the nv2a instance memory allocation is found to be ever deallocated after being 
				// mapped during initialization. The only one that could do it is MmClaimGpuInstanceMemory, however it doesn't seem
				// to deallocate the region, just to repurpose it...

				//u32 offset = std::next(rit)->first + std::next(rit)->second;
				/*if (rit == max_contiguous_it && m_MaxContiguousAddress - offset >= size)
				{
					addr = m_MaxContiguousAddress - size;
					m_Mem_map[addr] = size;
					m_PhysicalMemoryInUse += size;
					break;
				}*/

				if (rit->first - (std::next(rit)->first + std::next(rit)->second) >= size)
				{
					addr = rit->first - size;
					m_Mem_map[addr] = size;
					m_PhysicalMemoryInUse += size;
					break;
				}
			}
		}
	}
	else
	{
		if (m_Mem_map.empty())
		{
			addr = 0;
			m_Mem_map[addr] = size;
			m_PhysicalMemoryInUse += size;
		}
		else
		{
			// Allocate the block starting from the bottom of memory
			auto max_contiguous_it = m_Mem_map.lower_bound(m_MaxContiguousAddress); // skip the nv2a/PFN allocation
			for (auto it = m_Mem_map.begin(); ; ++it)
			{
				if (it == m_Mem_map.begin() && it->first >= size)
				{
					addr = 0;
					m_Mem_map[addr] = size;
					m_PhysicalMemoryInUse += size;
					break;
				}

				u32 offset = it->first + it->second;
				if (std::next(it) == max_contiguous_it)
				{
					if (m_MaxContiguousAddress - offset >= size)
					{
						addr = offset;
						m_Mem_map[addr] = size;
						m_PhysicalMemoryInUse += size;
						break;
					}

					if (FreeMemory >= size) // fragmentation
					{
						addr = AllocateFragmented(size);
						break;
					}
				}

				if (std::next(it)->first - offset >= size)
				{
					addr = offset;
					m_Mem_map[addr] = size;
					m_PhysicalMemoryInUse += size;
					break;
				}
			}
		}
	}
	return addr;
}

PAddr PhysicalMemory::AllocatePhysicalMemoryRange(size_t size, PAddr low_addr, PAddr high_addr)
{
	PAddr addr = m_MaxContiguousAddress;
	ClearError();
	size_t FreeMemory = m_MaxContiguousAddress - m_PhysicalMemoryInUse;
	if (size > FreeMemory)
	{
		EmuWarning("Out of physical memory!");
		SetError(PMEMORY_INSUFFICIENT_MEMORY);
		return addr;
	}

	// TODO: it's a bit complex to properly allocate the blocks inside the requested range using the iterators,
	// a better approch would be to implement an actual PFN database and search free blocks with that, but unfortunately 
	// with the current code it's not possible to relocate already allocated blocks

	// Allocate the block inside the specified range if possible, going from the top-down
	if (m_Mem_map.empty())
	{
		addr = low_addr;
		m_Mem_map[addr] = size;
		m_PhysicalMemoryInUse += size;
	}
	else
	{
		size_t FreeMemoryInRange = 0;

		auto low_pair = m_Mem_map.emplace(low_addr, 0);
		PAddr lower_bound = low_addr;
		if (low_pair.first != m_Mem_map.begin())
		{
			auto low_it = std::prev(low_pair.first);
			if (low_it->first + low_it->second > low_addr)
			{
				lower_bound = low_it->first + low_it->second;
			}
		}
		if (!low_pair.second)
		{
			lower_bound = low_addr + low_pair.first->second;
		}
		auto high_pair = m_Mem_map.emplace(high_addr, 0);
		auto high_it = high_pair.first;
		{
			auto prev_it = std::prev(high_pair.first);
			if (prev_it->first + prev_it->second >= high_pair.first->first)
			{
				high_it = prev_it;
				if (high_it == low_pair.first)
				{
					SetError(PMEMORY_INSUFFICIENT_MEMORY);
					EmuWarning("Not enough memory in range 0x%.8X - 0x%.8X", low_addr, high_addr);
					if (high_pair.first->second == 0) { m_Mem_map.erase(high_addr); }
					if (low_pair.first->second == 0) { m_Mem_map.erase(low_addr); }
					return addr;
				}
			}
		}

		for (auto it = high_it; ; --it)
		{
			if (std::prev(it) == low_pair.first)
			{
				if (it->first - lower_bound >= size)
				{
					addr = it->first - size;
					m_Mem_map[addr] = size;
					m_PhysicalMemoryInUse += size;
					break;
				}
				FreeMemoryInRange += (it->first - lower_bound);

				if (FreeMemoryInRange >= size) // fragmentation
				{
					addr = AllocateFragmented(size);
					break;
				}
				SetError(PMEMORY_INSUFFICIENT_MEMORY);
				EmuWarning("Not enough memory in range 0x%.8X - 0x%.8X", low_addr, high_addr);
				break;
			}

			size_t FreeBetween = it->first - (std::prev(it)->first + std::prev(it)->second);
			if (FreeBetween >= size)
			{
				addr = it->first - size;
				m_Mem_map[addr] = size;
				m_PhysicalMemoryInUse += size;
				break;
			}
			FreeMemoryInRange += FreeBetween;
		}
		if (high_pair.first->second == 0) { m_Mem_map.erase(high_addr); }
		if (low_pair.first->second == 0) { m_Mem_map.erase(low_addr); }
	}
	return addr;
}

VAddr PhysicalMemory::AllocateFragmented(size_t size)
{
	PAddr addr_ptr = (PAddr)VirtualAlloc(NULL, size + PAGE_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!addr_ptr)
	{
		EmuWarning("AllocateFragmented: VirtualAlloc could not allocate the memory!");
		SetError(PMEMORY_INSUFFICIENT_MEMORY);
		return m_MaxContiguousAddress;
	}
	VAddr aligned_start = addr_ptr & ~(UINT_PTR)PAGE_MASK;

	m_Fragmented_mem_map[addr_ptr] = size;
	EmuWarning("Warning: allocated memory via AllocateFragmented.");
	SetError(PMEMORY_ALLOCATE_FRAGMENTED);
	m_PhysicalMemoryInUse += size;
	return aligned_start;
}

void PhysicalMemory::ShrinkPhysicalAllocation(PAddr addr, size_t offset, bool bFragmentedMap, bool bStart)
{
	if (!offset) { return; } // nothing to do

	if (bFragmentedMap)
	{
		auto it = std::prev(m_Fragmented_mem_map.upper_bound(addr));
		PAddr old_base = it->first;
		size_t old_size = it->second;
		m_Fragmented_mem_map.erase(old_base);

		if (old_size - offset)
		{
			if (bStart) { m_Fragmented_mem_map.emplace(old_base + offset, old_size - offset); }
			else { m_Fragmented_mem_map.emplace(old_base, old_size - offset); }
		}

		m_PhysicalMemoryInUse -= offset;
	}
	else
	{
		auto it = m_Mem_map.lower_bound(addr);
		PAddr old_base = it->first;
		size_t old_size = it->second;
		m_Mem_map.erase(old_base);

		if (old_size - offset)
		{
			if (bStart) { m_Mem_map.emplace(old_base + offset, old_size - offset); }
			else { m_Mem_map.emplace(old_base, old_size - offset); }
		}

		m_PhysicalMemoryInUse -= offset;
	}
}

void PhysicalMemory::DeAllocatePhysicalMemory(PAddr addr)
{
	auto it = m_Mem_map.lower_bound(addr);
	m_PhysicalMemoryInUse -= it->second;
	m_Mem_map.erase(addr);
}

void PhysicalMemory::DeAllocateFragmented(VAddr addr)
{
	auto it = std::prev(m_Fragmented_mem_map.upper_bound(addr));
	VirtualFree((void*)it->first, 0, MEM_RELEASE);
	m_PhysicalMemoryInUse -= it->second;
	m_Fragmented_mem_map.erase(it->first);
}

void PhysicalMemory::SetError(PMEMORY_STATUS err)
{
	m_Status = err;
}

void PhysicalMemory::ClearError()
{
	m_Status = PMEMORY_SUCCESS;
}
