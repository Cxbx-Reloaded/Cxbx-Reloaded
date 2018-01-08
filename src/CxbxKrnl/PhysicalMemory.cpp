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


PMEMORY_STATUS PhysicalMemory::GetError() const
{
	return m_Status;
}

PAddr PhysicalMemory::AllocatePhysicalMemory(const size_t size, const PageType type)
{
	PAddr addr = m_MaxContiguousAddress;
	ClearError();
	// TODO : assert(m_MaxContiguousAddress >= m_PhysicalMemoryInUse);
	size_t FreeMemory = m_MaxContiguousAddress - m_PhysicalMemoryInUse;
	if (size > FreeMemory)
	{
		EmuWarning("Out of physical memory!");
		SetError(PMEMORY_INSUFFICIENT_MEMORY);
		return addr;
	}

	// Allocate the block wherever possible
	// This attempts to counter external fragmentation by allocating big blocks top-down and small blocks bottom-up
	const bool bTopDown = size > m_AllocationThreshold;
	if (m_Mem_map.empty())
	{
		addr = bTopDown ? (m_MaxContiguousAddress - size) : 0;
		m_Mem_map[addr] = size;
		m_PhysicalMemoryInUse += size;
	}
	else
	{
		if (bTopDown)
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
						addr = AllocateFragmented(size, type);
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
		else // !bTopDown
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
						addr = AllocateFragmented(size, type);
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

	m_PageCount[(int)type] += size / PAGE_SIZE;
	return addr;
}

PAddr PhysicalMemory::AllocatePhysicalMemoryRange(const size_t size, const PageType type, const PAddr low_addr, const PAddr high_addr)
{
	PAddr addr = m_MaxContiguousAddress;
	ClearError();
	// TODO : assert(m_MaxContiguousAddress >= m_PhysicalMemoryInUse);
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
					addr = AllocateFragmented(size, type);
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

	m_PageCount[(int)type] += size / PAGE_SIZE;
	return addr;
}

VAddr PhysicalMemory::AllocateFragmented(const size_t size, const PageType type)
{
	if (type == PageType::Contiguous)
	{
		EmuWarning("Fragmentation prevented allocation of contiguous memory!");
		SetError(PMEMORY_INSUFFICIENT_MEMORY);
		return 0;
	}

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

void PhysicalMemory::ShrinkPhysicalAllocation(const PAddr addr, const size_t offset, const bool bFragmentedMap, const bool bStart)
{
	if (!offset) { return; } // nothing to do

	if (bFragmentedMap)
	{
		auto it = std::prev(m_Fragmented_mem_map.upper_bound(addr));
		if (it == m_Fragmented_mem_map.end()) { return; }

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
		if (it == m_Mem_map.end()) { return; }

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

void PhysicalMemory::DeAllocatePhysicalMemory(const PAddr addr)
{
	auto it = m_Mem_map.lower_bound(addr);
	if (it == m_Mem_map.end()) { EmuWarning("DeAllocatePhysicalMemory : addr unknown!");  return; }

	m_PhysicalMemoryInUse -= it->second;
	m_Mem_map.erase(addr);
}

void PhysicalMemory::DeAllocateFragmented(const VAddr addr)
{
	auto it = std::prev(m_Fragmented_mem_map.upper_bound(addr));
	if (it == m_Fragmented_mem_map.end()) { EmuWarning("DeAllocateFragmented : addr unknown!");  return; }

	VirtualFree((void*)it->first, 0, MEM_RELEASE);
	m_PhysicalMemoryInUse -= it->second;
	m_Fragmented_mem_map.erase(it->first);
}

void PhysicalMemory::SetError(const PMEMORY_STATUS err)
{
	m_Status = err;
}

void PhysicalMemory::ClearError()
{
	m_Status = PMEMORY_SUCCESS;
}
