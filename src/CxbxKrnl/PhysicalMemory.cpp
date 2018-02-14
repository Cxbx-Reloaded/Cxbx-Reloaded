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

	TempPF.Pte.Default = ValidKernelPteBits;
	TempPF.Pte.Hardware.Persist = 1;
	TempPF.Pte.Hardware.GuardOrEnd = 1;
	TempPF.Pte.Hardware.PFN = pfn;

	if (g_bIsRetail || g_bIsDebug) {
		*XBOX_PFN_ELEMENT(pfn) = TempPF;
	}
	else { *CHIHIRO_PFN_ELEMENT(pfn) = TempPF; }

	m_PhysicalPagesAvailable--;
	m_PagesByUsage[Contiguous]++; // treat the page of the page directory as contiguous usage


	// Construct the pfn's of the kernel pages
	pfn = CONVERT_CONTIGUOUS_PHYSICAL_TO_PFN(XBOX_KERNEL_BASE);
	pfn_end = CONVERT_CONTIGUOUS_PHYSICAL_TO_PFN(XBOX_KERNEL_BASE + KERNEL_SIZE - 1);

	while (pfn <= pfn_end)
	{
		TempPF.Pte.Default = ValidKernelPteBits;
		TempPF.Pte.Hardware.Persist = 1;
		TempPF.Pte.Hardware.PFN = pfn;

		if (g_bIsRetail || g_bIsDebug) {
			*XBOX_PFN_ELEMENT(pfn) = TempPF;
		}
		else { *CHIHIRO_PFN_ELEMENT(pfn) = TempPF; }

		m_PhysicalPagesAvailable--;
		m_PagesByUsage[Contiguous]++; // treat the pages of the kernel as contiguous usage
		pfn++;
	}


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

	while (pfn <= pfn_end)
	{
		TempPF.Pte.Default = ValidKernelPteBits;
		TempPF.Pte.Hardware.Persist = 1;
		TempPF.Pte.Hardware.PFN = pfn;

		if (g_bIsRetail || g_bIsDebug) {
			*XBOX_PFN_ELEMENT(pfn) = TempPF;
		}
		else { *CHIHIRO_PFN_ELEMENT(pfn) = TempPF; }

		m_PhysicalPagesAvailable--;
		m_PagesByUsage[Contiguous]++; // treat the pages of the pfn as contiguous usage
		pfn++;
	}
	

	// Construct the pfn's of the pages holding the nv2a instance memory
	if (g_bIsRetail || g_bIsDebug) {
		pfn = XBOX_INSTANCE_PHYSICAL_PAGE;
		pfn_end = XBOX_INSTANCE_PHYSICAL_PAGE + NV2A_INSTANCE_PAGE_COUNT - 1;
	}
	else {
		pfn = CHIHIRO_INSTANCE_PHYSICAL_PAGE;
		pfn_end = CHIHIRO_INSTANCE_PHYSICAL_PAGE + NV2A_INSTANCE_PAGE_COUNT - 1;
	}

	while (pfn <= pfn_end)
	{
		TempPF.Pte.Default = ValidKernelPteBits;
		DISABLE_CACHING(TempPF.Pte);
		TempPF.Pte.Hardware.PFN = pfn;

		if (g_bIsRetail || g_bIsDebug) {
			*XBOX_PFN_ELEMENT(pfn) = TempPF;
		}
		else { *CHIHIRO_PFN_ELEMENT(pfn) = TempPF; }

		m_PhysicalPagesAvailable--;
		m_PagesByUsage[Contiguous]++; // treat the pages of the nv2a instance memory as contiguous usage
		pfn++;
	}

	if (g_bIsDebug)
	{
		// Debug kits have two nv2a instance memory, another at the top of the 128 MiB

		pfn = XBOX_INSTANCE_PHYSICAL_PAGE + DEBUGKIT_FIRST_UPPER_HALF_PAGE - 1;
		pfn_end = XBOX_INSTANCE_PHYSICAL_PAGE + DEBUGKIT_FIRST_UPPER_HALF_PAGE + NV2A_INSTANCE_PAGE_COUNT - 1;

		while (pfn <= pfn_end)
		{
			TempPF.Pte.Default = ValidKernelPteBits;
			DISABLE_CACHING(TempPF.Pte);
			TempPF.Pte.Hardware.PFN = pfn;

			*XBOX_PFN_ELEMENT(pfn) = TempPF;

			m_PhysicalPagesAvailable--;
			m_PagesByUsage[Contiguous]++; // treat the pages of the nv2a instance memory as contiguous usage
			pfn++;
		}
	}

	// Construct the pfn of the page used by D3D
	pfn = D3D_PHYSICAL_PAGE;

	TempPF.Pte.Default = ValidKernelPteBits;
	TempPF.Pte.Hardware.Persist = 1;
	TempPF.Pte.Hardware.GuardOrEnd = 1;
	TempPF.Pte.Hardware.PFN = pfn;

	if (g_bIsRetail || g_bIsDebug) {
		*XBOX_PFN_ELEMENT(pfn) = TempPF;
	}
	else { *CHIHIRO_PFN_ELEMENT(pfn) = TempPF; }

	m_PhysicalPagesAvailable--;
	m_PagesByUsage[Contiguous]++; // treat the initialization page of D3D as contiguous usage
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

bool PhysicalMemory::SearchMap(PFN searchvalue, PFN* result)
{
	auto it = m_PhysicalMap.upper_bound(searchvalue);

	if (it == m_PhysicalMap.begin()) // it->first > searchvalue
		goto fail;

	--it;  // it->first <= searchvalue
	if (searchvalue >= it->first && searchvalue <= it->second)
	{
		*result = it->first;
		return true;
	}

	fail:
		result = nullptr;
		return false;
}

void PhysicalMemory::RemoveFree(PFN start, PFN end)
{
	PFN result;
	bool bFound = SearchMap(start, &result); // this function is required in release builds so it cannot be asserted directly
	assert(bFound);
	PFN old_end = m_PhysicalMap[result];

	if (start != result)
	{
		// split at the beginning
		m_PhysicalMap[result] = start - 1;
	}
	else
	{
		// remove the old entry which is now invalid
		m_PhysicalMap.erase(start);
	}

	if (end != old_end)
	{
		// split at the end
		m_PhysicalMap[end + 1] = old_end;
	}

	m_PhysicalPagesAvailable -= (end - start + 1);
}

void PhysicalMemory::InsertFree(PFN start, PFN end)
{
	PFN result;
	assert(!SearchMap(ranges, start, &result));

	m_PhysicalMap[start] = end;

	if (SearchMap(end + 1, &result))
	{
		// merge with the following free region
		m_PhysicalMap[start] = m_PhysicalMap[result];
		m_PhysicalMap.erase(result);
	}

	if (SearchMap(start - 1, &result))
	{
		// merge with the previous free region
		m_PhysicalMap[result] = m_PhysicalMap[start];
		m_PhysicalMap.erase(start);
	}

	m_PhysicalPagesAvailable += (end - start + 1);
}

bool PhysicalMemory::FindFreeContiguous(PFN_COUNT size, PFN* result, PFN low, PFN high)
{
	std::map<PFN, PFN>::iterator it;

	if (!m_PhysicalMap.size() || high - low < size || size == 0)
		goto fail;

	for (it = --(m_PhysicalMap.end()); ; --it)
	{
		if (it->second - it->first + 1 >= size && ((low == 0 && high == MAX_VIRTUAL_ADDRESS >> PAGE_SHIFT) || 
			(low >= it->first && high <= it->second)))
		{
			*result = it->first;
			return true;
		}

		if (it == m_PhysicalMap.begin())
			break;
	}

	fail:
		result = nullptr;
		return false;
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
