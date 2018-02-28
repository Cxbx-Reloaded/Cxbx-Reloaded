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
// *  (c) 2017-2018      ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

#include "PhysicalMemory.h"
#include <assert.h>


void FillMemoryUlong(void* Destination, size_t Length, ULONG Long)
{
	assert(Length != 0);
	assert(CHECK_ALIGNMENT(Length, sizeof(ULONG)));                   // Length must be a multiple of ULONG
	assert(CHECK_ALIGNMENT((uintptr_t)Destination, sizeof(ULONG)));   // Destination must be 4-byte aligned

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

void PhysicalMemory::InitializePageDirectory()
{
	PMMPTE pPde;
	PMMPTE pPde_end;
	MMPTE TempPte;


	// Write the pde's representing the user virtual space (lower 2 GiB) - no page tables
	pPde_end = GetPdeAddress(CONTIGUOUS_MEMORY_BASE - 1);
	for (pPde = GetPdeAddress(0); pPde <= pPde_end; ++pPde)
	{
		WRITE_ZERO_PTE(pPde);
	}


	// Write the pde's of the WC (tiled) memory - no page tables
	TempPte.Default = ValidKernelPteBits;
	TempPte.Hardware.LargePage = 1;
	TempPte.Hardware.PFN = XBOX_WRITE_COMBINED_BASE >> PAGE_SHIFT;
	SET_WRITE_COMBINE(TempPte);
	pPde_end = GetPdeAddress(XBOX_WRITE_COMBINE_END - 1);
	for (pPde = GetPdeAddress(XBOX_WRITE_COMBINED_BASE); pPde <= pPde_end; ++pPde)
	{
		WRITE_PTE(pPde, TempPte);
		TempPte.Default += PAGE_SIZE_LARGE; // increase PFN
	}


	// Write the pde's of the UC memory region - no page tables
	TempPte.Hardware.PFN = XBOX_UNCACHED_BASE >> PAGE_SHIFT;
	DISABLE_CACHING(TempPte);
	pPde_end = GetPdeAddress(XBOX_UNCACHED_END - 1);
	for (pPde = GetPdeAddress(XBOX_UNCACHED_BASE); pPde <= pPde_end; ++pPde)
	{
		WRITE_PTE(pPde, TempPte);
		TempPte.Default += PAGE_SIZE_LARGE; // increase PFN
	}


	// Write the pde's of the contiguous region
	TempPte.Default = ValidKernelPdeBits;
	if (g_bIsRetail || g_bIsDebug) {
		TempPte.Hardware.PFN = (ULONG)XBOX_PFN_ADDRESS >> PAGE_SHIFT;
		pPde_end = GetPdeAddress(CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(XBOX_HIGHEST_PHYSICAL_PAGE));
	}
	else {
		TempPte.Hardware.PFN = (ULONG)CHIHIRO_PFN_ADDRESS >> PAGE_SHIFT;
		pPde_end = GetPdeAddress(CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(CHIHIRO_HIGHEST_PHYSICAL_PAGE));
	}
	for (pPde = GetPdeAddress(SYSTEM_PHYSICAL_MAP); pPde <= pPde_end; ++pPde)
	{
		WRITE_PTE(pPde, TempPte);
		TempPte.Hardware.PFN++; // increase PFN
	}

	// NOTE: we don't need to unmap the rest of the system physical region because that mapping is done by the 2BL
	// on the Xbox, which is not present here on Cxbx-Reloaded

	// Here we should also reserve some system pte's for the file system cache. However, the implementation of the kernel
	// file cache function is basically non-existent at the moment and relies on ExAllocatePoolWithTag, which is not
	// correctly implemented. So, for now, we keep on ignoring this allocation
}

void PhysicalMemory::WritePfn(PFN pfn_start, PFN pfn_end, PMMPTE pPte, PageType BusyType, bool bContiguous)
{
	XBOX_PFN TempPF;

	if (bContiguous)
	{
		// In the contiguous region we construct the pfn as a pte

		while (pfn_start <= pfn_end)
		{
			TempPF.Pte = *pPte;
			TempPF.Pte.Hardware.PFN = pfn_start;

			if (g_bIsRetail || g_bIsDebug) {
				*XBOX_PFN_ELEMENT(pfn_start) = TempPF;
			}
			else { *CHIHIRO_PFN_ELEMENT(pfn_start) = TempPF; }

			m_PagesByUsage[PageType::Contiguous]++;
			pfn_start++;
		}
	}
	else
	{
		TempPF.Default = 0;
		TempPF.Busy.Busy = 1;
		TempPF.Busy.BusyType = BusyType;
		if (BusyType != PageType::VirtualPageTable) {
			TempPF.Busy.PteIndex = GetPteOffset(GetVAddrMappedByPte(pPte));
		}

		if (g_bIsRetail || g_bIsDebug) {
			*XBOX_PFN_ELEMENT(pfn_start) = TempPF;
		}
		else { *CHIHIRO_PFN_ELEMENT(pfn_start) = TempPF; }

		m_PagesByUsage[BusyType]++;
	}
}

bool PhysicalMemory::RemoveFree(PFN_COUNT NumberOfPages, PFN* result, PFN_COUNT PfnAlignment, PFN start, PFN end)
{
	xboxkrnl::PLIST_ENTRY ListEntry;
	PFN PfnStart;
	PFN PfnEnd;
	PFN IntersectionStart;
	PFN IntersectionEnd;
	PFN_COUNT PfnCount;
	PFN_COUNT PfnAlignmentMask;
	PFN_COUNT PfnAlignmentSubtraction;

	if (NumberOfPages == 0 || NumberOfPages > m_PhysicalPagesAvailable) { result = nullptr; return false; }

	if (PfnAlignment)
	{
		// Calculate some alignment parameters if one is requested

		PfnAlignmentMask = ~(PfnAlignment - 1);
		PfnAlignmentSubtraction = ((NumberOfPages + PfnAlignment - 1) & PfnAlignmentMask) - NumberOfPages + 1;
	}

	ListEntry = FreeList.Blink; // search from the top

	while (ListEntry != &FreeList)
	{
		if (LIST_ENTRY_ACCESS_RECORD(ListEntry, FreeBlock, ListEntry)->size >= NumberOfPages)  // not enough space
		{
			PfnStart = LIST_ENTRY_ACCESS_RECORD(ListEntry, FreeBlock, ListEntry)->start;
			PfnCount = LIST_ENTRY_ACCESS_RECORD(ListEntry, FreeBlock, ListEntry)->size;
			PfnEnd = PfnStart + PfnCount - 1;
			IntersectionStart = start >= PfnStart ? start : PfnStart;
			IntersectionEnd = end <= PfnEnd ? end : PfnEnd;

			if (IntersectionEnd < IntersectionStart)
			{
				// Not inside the requested range, keep searching

				goto InvalidBlock;
			}

			if (IntersectionEnd - IntersectionStart + 1 < NumberOfPages)
			{
				// There is not enough free space inside the free block so this is an invalid block.
				// We have to check again since the free size could have shrinked because of the intersection
				// check done above

				goto InvalidBlock;
			}

			if (PfnAlignment)
			{
				IntersectionEnd = (IntersectionEnd & PfnAlignmentMask) - PfnAlignmentSubtraction;

				if (IntersectionEnd - IntersectionStart + 1 < NumberOfPages)
				{
					// This free block doesn't honor the alignment requested, so this is another invalid block

					goto InvalidBlock;
				}
			}

			// Now we know that we have a usable free block with enough pages

			if (IntersectionStart == PfnStart)
			{
				if (IntersectionEnd == PfnEnd)
				{
					// The block is totally inside the range, just shrink its size

					PfnCount -= NumberOfPages;
					if (!PfnCount)
					{
						// delete the entry if there is no free space left

						LIST_ENTRY_REMOVE(ListEntry);
						delete LIST_ENTRY_ACCESS_RECORD(ListEntry, FreeBlock, ListEntry);
					}
					else { LIST_ENTRY_ACCESS_RECORD(ListEntry, FreeBlock, ListEntry)->size = PfnCount; }
					m_PhysicalPagesAvailable -= NumberOfPages;
					*result = PfnStart + PfnCount;
					return true;
				}
				else
				{
					// Create a new block with the remaining pages after the block

					PFreeBlock block = new FreeBlock;
					block->start = IntersectionEnd + 1;
					block->size = PfnStart + PfnCount - IntersectionEnd - 1;
					LIST_ENTRY_INITIALIZE(&block->ListEntry);
					LIST_ENTRY_INSERT_HEAD(ListEntry, &block->ListEntry);

					PfnCount = IntersectionEnd - PfnStart - NumberOfPages + 1;
					if (!PfnCount)
					{
						// delete the entry if there is no free space left

						LIST_ENTRY_REMOVE(ListEntry);
						delete LIST_ENTRY_ACCESS_RECORD(ListEntry, FreeBlock, ListEntry);
					}
					else { LIST_ENTRY_ACCESS_RECORD(ListEntry, FreeBlock, ListEntry)->size = PfnCount; }
					m_PhysicalPagesAvailable -= NumberOfPages;
					*result = PfnStart + PfnCount;
					return true;
				}
			}
			else
			{
				// Starting address of the free block is lower than the intersection start

				if (IntersectionEnd == PfnEnd)
				{
					// The free block extends before IntersectionStart

					PfnCount -= NumberOfPages;
					LIST_ENTRY_ACCESS_RECORD(ListEntry, FreeBlock, ListEntry)->size = PfnCount;

					m_PhysicalPagesAvailable -= NumberOfPages;
					*result = PfnStart + PfnCount;
					return true;
				}
				else
				{
					// The free block extends in both directions

					PFreeBlock block = new FreeBlock;
					block->start = IntersectionEnd + 1;
					block->size = PfnStart + PfnCount - IntersectionEnd - 1;
					LIST_ENTRY_INITIALIZE(&block->ListEntry);
					LIST_ENTRY_INSERT_HEAD(ListEntry, &block->ListEntry);

					PfnCount = IntersectionEnd - PfnStart - NumberOfPages + 1;
					LIST_ENTRY_ACCESS_RECORD(ListEntry, FreeBlock, ListEntry)->size = PfnCount;

					m_PhysicalPagesAvailable -= NumberOfPages;
					*result = PfnStart + PfnCount;
					return true;
				}
			}
		}
		InvalidBlock:
		ListEntry = ListEntry->Blink;
	}
	result = nullptr;
	return false;
}

void PhysicalMemory::InsertFree(PFN start, PFN end)
{
	xboxkrnl::PLIST_ENTRY ListEntry;
	PFN_COUNT size = end - start + 1;

	ListEntry = FreeList.Blink; // search from the top

	while (true)
	{
		if (LIST_ENTRY_ACCESS_RECORD(ListEntry, FreeBlock, ListEntry)->start < start || ListEntry == &FreeList)
		{
			PFreeBlock block = new FreeBlock;
			block->start = start;
			block->size = size;
			LIST_ENTRY_INITIALIZE(&block->ListEntry);
			LIST_ENTRY_INSERT_HEAD(ListEntry, &block->ListEntry);

			ListEntry = ListEntry->Flink; // move to the new created block

			// Check if merging is possible
			if (ListEntry->Flink != &FreeList &&
				start + size == LIST_ENTRY_ACCESS_RECORD(ListEntry->Flink, FreeBlock, ListEntry)->start)
			{
				// Merge forward
				xboxkrnl::PLIST_ENTRY temp = ListEntry->Flink;
				LIST_ENTRY_ACCESS_RECORD(ListEntry, FreeBlock, ListEntry)->size +=
					LIST_ENTRY_ACCESS_RECORD(temp, FreeBlock, ListEntry)->size;
				LIST_ENTRY_REMOVE(temp);
				delete LIST_ENTRY_ACCESS_RECORD(temp, FreeBlock, ListEntry);
			}
			if (ListEntry->Blink != &FreeList &&
				LIST_ENTRY_ACCESS_RECORD(ListEntry->Blink, FreeBlock, ListEntry)->start +
				LIST_ENTRY_ACCESS_RECORD(ListEntry->Blink, FreeBlock, ListEntry)->size == start)
			{
				// Merge backward
				LIST_ENTRY_ACCESS_RECORD(ListEntry->Blink, FreeBlock, ListEntry)->size +=
					LIST_ENTRY_ACCESS_RECORD(ListEntry, FreeBlock, ListEntry)->size;
				LIST_ENTRY_REMOVE(ListEntry);
				delete block;
			}
			m_PhysicalPagesAvailable += size;

			return;
		}
		ListEntry = ListEntry->Blink;
	}
}

bool PhysicalMemory::ConvertXboxToSystemPteProtection(DWORD perms, PMMPTE pPte)
{
	ULONG Mask = 0;

	if (perms & ~(XBOX_PAGE_NOCACHE | XBOX_PAGE_WRITECOMBINE | XBOX_PAGE_READWRITE | XBOX_PAGE_READONLY))
	{
		return false; // unknown or not allowed flag specified
	}

	switch (perms & (XBOX_PAGE_READONLY | XBOX_PAGE_READWRITE))
	{
		case XBOX_PAGE_READONLY:
		{
			Mask = (PTE_VALID_MASK | PTE_DIRTY_MASK | PTE_ACCESS_MASK);
		}
		break;

		case XBOX_PAGE_READWRITE:
		{
			Mask = (PTE_VALID_MASK | PTE_WRITE_MASK | PTE_DIRTY_MASK | PTE_ACCESS_MASK);
		}
		break;

		default:
			return false; // both are specified, wrong
	}

	switch (perms & (XBOX_PAGE_NOCACHE | XBOX_PAGE_WRITECOMBINE))
	{
		case 0:
			break; // none is specified, ok

		case XBOX_PAGE_NOCACHE:
		{
			Mask |= PTE_CACHE_DISABLE_MASK;
		}
		break;

		case XBOX_PAGE_WRITECOMBINE:
		{
			Mask |= PTE_WRITE_THROUGH_MASK;
		}
		break;

		default:
			return false; // both are specified, wrong
	}

	pPte->Default = Mask;

	return true;
}

bool PhysicalMemory::AllocatePT(PFN_COUNT PteNumber, VAddr addr)
{
	PMMPTE pPde;
	MMPTE TempPte;
	PFN_COUNT PdeNumber = ROUND_UP(PteNumber, PTE_PER_PAGE) / PTE_PER_PAGE;
	PFN_COUNT PTtoCommit = 0;
	PageType BusyType = PageType::SystemPageTable;
	int PdeMappedSizeIncrement = 0;

	assert(PteNumber);
	assert(addr);

	for (int i = 0; i < PdeNumber; ++i)
	{
		if (GetPdeAddress(addr += PdeMappedSizeIncrement)->Hardware.Valid == 0)
		{
			PTtoCommit++;
		}
		PdeMappedSizeIncrement += (4 * ONE_MB);
	}

	if (!PTtoCommit)
	{
		// We don't need to commit any new page table, so exit now

		return true;
	}

	if (m_PhysicalPagesAvailable < PTtoCommit)
	{
		// We don't have enough memory for PT's mapping this allocation

		return false;
	}
	if (addr < HIGHEST_USER_ADDRESS) { BusyType = PageType::VirtualPageTable; }
	PdeMappedSizeIncrement = 0;

	// Now actually commit the PT's
	for (int i = 0; i < PdeNumber; ++i)
	{
		pPde = GetPdeAddress(addr += PdeMappedSizeIncrement);
		if (pPde->Hardware.Valid == 0)
		{
			// We grab one page at a time to avoid fragmentation issues

			TempPte.Default = ValidKernelPdeBits;
			TempPte.Hardware.PFN = RemoveAndZeroAnyFreePage(BusyType, pPde);
			WRITE_PTE(pPde, TempPte);
		}
		PdeMappedSizeIncrement += (4 * ONE_MB);
	}

	return true;
}

PFN PhysicalMemory::RemoveAndZeroAnyFreePage(PageType BusyType, PMMPTE pPte)
{
	XBOX_PFN TempPF;
	PFN pfn;

	assert(BusyType < PageType::COUNT);
	assert(pPte);

	// NOTE: for now this doesn't require a check for success but if called from other callers it will...
	RemoveFree(1, &pfn, 0, m_HighestPage);

	// Fill the page with zeros
	FillMemoryUlong((void*)CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn), PAGE_SIZE, 0);

	// Construct the pfn for the page
	WritePfn(0, 0, pPte, BusyType, false); // at the moment I'm not sure if this needs a check or not...

	return pfn;
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

void PhysicalMemory::SetError(PMEMORY_STATUS err)
{
	m_Status = err;
}

void PhysicalMemory::ClearError()
{
	m_Status = PMEMORY_SUCCESS;
}
