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
	XBOX_PFN TempPF;
	PFN pfn;
	PFN pfn_end;


	// Default initialize the entries of the PFN database
	TempPF.Default = 0;
	TempPF.Busy.Busy = 1;
	TempPF.Busy.LockCount = LOCK_COUNT_MAXIMUM;
	TempPF.Busy.BusyType = Unknown;
	if (g_bIsRetail) {
		FillMemoryUlong((void*)XBOX_PFN_ADDRESS, X64KB, TempPF.Default); // Xbox: 64 KiB
	}
	else if (g_bIsChihiro) { 
		FillMemoryUlong((void*)CHIHIRO_PFN_ADDRESS, X64KB * 2, TempPF.Default); // Chihiro: 128 KiB
	} 
	else { 
		FillMemoryUlong((void*)XBOX_PFN_ADDRESS, X64KB * 2, TempPF.Default); // Debug: 128 KiB
	}

	// Construct the pfn of the page directory
	pfn = CONVERT_CONTIGUOUS_PHYSICAL_TO_PFN(PAGE_DIRECTORY_PHYSICAL_ADDRESS);
	TempPF.Pte.Default = ValidKernelPteBits | PTE_GUARD_END_MASK | PTE_PERSIST_MASK;

	WritePfn(TempPF, pfn, pfn, TempPF.Pte, Contiguous, true);


	// Construct the pfn's of the kernel pages
	pfn = CONVERT_CONTIGUOUS_PHYSICAL_TO_PFN(XBOX_KERNEL_BASE);
	pfn_end = CONVERT_CONTIGUOUS_PHYSICAL_TO_PFN(XBOX_KERNEL_BASE + KERNEL_SIZE - 1);
	TempPF.Pte.Default = ValidKernelPteBits | PTE_PERSIST_MASK;

	WritePfn(TempPF, pfn, pfn_end, TempPF.Pte, Contiguous, true);


	// Construct the pfn's of the pages holding the pfn database
	if (g_bIsRetail) {
		pfn = XBOX_PFN_DATABASE_PHYSICAL_PAGE;
		pfn_end = XBOX_PFN_DATABASE_PHYSICAL_PAGE + 16 - 1;
	}
	else if (g_bIsDebug) {
		pfn = XBOX_PFN_DATABASE_PHYSICAL_PAGE;
		pfn_end = XBOX_PFN_DATABASE_PHYSICAL_PAGE + 32 - 1;
	}
	else {
		pfn = CHIHIRO_PFN_DATABASE_PHYSICAL_PAGE;
		pfn_end = CHIHIRO_PFN_DATABASE_PHYSICAL_PAGE + 32 - 1;
	}
	TempPF.Pte.Default = ValidKernelPteBits | PTE_PERSIST_MASK;

	WritePfn(TempPF, pfn, pfn_end, TempPF.Pte, Contiguous, true);
	

	// Construct the pfn's of the pages holding the nv2a instance memory
	if (g_bIsRetail || g_bIsDebug) {
		pfn = XBOX_INSTANCE_PHYSICAL_PAGE;
		pfn_end = XBOX_INSTANCE_PHYSICAL_PAGE + NV2A_INSTANCE_PAGE_COUNT - 1;
	}
	else {
		pfn = CHIHIRO_INSTANCE_PHYSICAL_PAGE;
		pfn_end = CHIHIRO_INSTANCE_PHYSICAL_PAGE + NV2A_INSTANCE_PAGE_COUNT - 1;
	}
	TempPF.Pte.Default = ValidKernelPteBits;
	DISABLE_CACHING(TempPF.Pte);

	WritePfn(TempPF, pfn, pfn_end, TempPF.Pte, Contiguous, true);


	if (g_bIsDebug)
	{
		// Debug kits have two nv2a instance memory, another at the top of the 128 MiB

		pfn = XBOX_INSTANCE_PHYSICAL_PAGE + DEBUGKIT_FIRST_UPPER_HALF_PAGE - 1;
		pfn_end = XBOX_INSTANCE_PHYSICAL_PAGE + DEBUGKIT_FIRST_UPPER_HALF_PAGE + NV2A_INSTANCE_PAGE_COUNT - 1;
		TempPF.Pte.Default = ValidKernelPteBits;
		DISABLE_CACHING(TempPF.Pte);

		WritePfn(TempPF, pfn, pfn_end, TempPF.Pte, Contiguous, true);
	}

	// Construct the pfn of the page used by D3D
	pfn = D3D_PHYSICAL_PAGE;
	TempPF.Pte.Default = ValidKernelPteBits | PTE_GUARD_END_MASK | PTE_PERSIST_MASK;

	WritePfn(TempPF, pfn, pfn, TempPF.Pte, Contiguous, true);
}

void PhysicalMemory::ReinitializePfnDatabase()
{

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
		TempPte.Hardware.PFN = (ULONG)XBOX_PFN_ADDRESS;
		pPde_end = GetPdeAddress(CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(XBOX_HIGHEST_PHYSICAL_PAGE));
	}
	else {
		TempPte.Hardware.PFN = (ULONG)CHIHIRO_PFN_ADDRESS;
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
	// correctly implemented. So, for, we keep on ignoring this allocation
}

void PhysicalMemory::WritePfn(XBOX_PFN Pfn, PFN pfn_start, PFN pfn_end, MMPTE Pte, PageType BusyType, bool bContiguous)
{
	XBOX_PFN TempPF;

	if (bContiguous)
	{
		// In the contiguous region we construct the pfn as a pte

		while (pfn_start <= pfn_end)
		{
			TempPF.Pte = Pte;
			TempPF.Pte.Hardware.PFN = pfn_start;

			if (g_bIsRetail || g_bIsDebug) {
				*XBOX_PFN_ELEMENT(pfn_start) = TempPF;
			}
			else { *CHIHIRO_PFN_ELEMENT(pfn_start) = TempPF; }

			m_PhysicalPagesAvailable--;
			m_PagesByUsage[Contiguous]++;
			pfn_start++;
		}
	}
	else
	{
		// In all the other regions we construct the pfn as a regular entry
		// TODO
	}
}

bool PhysicalMemory::RemoveFree(PFN_COUNT NumberOfPages, PFN* result, PFN start, PFN end)
{
	xboxkrnl::PLIST_ENTRY ListEntry;
	PFN PfnStart;
	PFN PfnEnd;
	PFN IntersectionStart;
	PFN IntersectionEnd;
	PFN_COUNT PfnCount;

	if (NumberOfPages == 0 || NumberOfPages > m_PhysicalPagesAvailable) { result = nullptr; return false; }

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

bool PhysicalMemory::AllocatePtes(PFN_COUNT PteNumber, VAddr addr)
{
	PMMPTE pPde;
	MMPTE TempPte;
	PFN_COUNT PdeNumber = ROUND_UP(PteNumber, PTE_PER_PAGE) / PTE_PER_PAGE;
	PFN_COUNT PTtoCommit = 0;
	PageType BusyType = SystemPageTable;
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

	if (m_PhysicalPagesAvailable < PTtoCommit)
	{
		// We don't have enough memory for PT's mapping this allocation

		return false;
	}
	if (addr < HIGHEST_USER_ADDRESS) { BusyType = VirtualPageTable; }
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
			m_PhysicalPagesAvailable--;
		}
		PdeMappedSizeIncrement += (4 * ONE_MB);
	}

	return true;
}

PFN PhysicalMemory::RemoveAndZeroAnyFreePage(PageType BusyType, PMMPTE pPte)
{
	XBOX_PFN TempPF;
	PFN pfn;

	assert(BusyType < COUNT);
	assert(pPte);

	FindFreeContiguous(1, &pfn);
	RemoveFree(pfn, pfn);

	// Fill the page with zeros
	FillMemoryUlong((void*)CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn), PAGE_SIZE, 0);

	// Construct the pfn for the page
	TempPF.Default = 0;
	TempPF.Busy.Busy = 1;
	TempPF.Busy.BusyType = BusyType;
	if (BusyType != VirtualPageTable) {
		TempPF.Busy.PteIndex = GetPteOffset(GetVAddrMappedByPte(pPte));
	}

	if (g_bIsRetail || g_bIsDebug) {
		*XBOX_PFN_ELEMENT(pfn) = TempPF;
	}
	else { *CHIHIRO_PFN_ELEMENT(pfn) = TempPF; }

	m_PagesByUsage[BusyType]++;

	return pfn;
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
