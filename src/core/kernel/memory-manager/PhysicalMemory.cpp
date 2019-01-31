// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
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


#define LOG_PREFIX CXBXR_MODULE::PMEM

#include "PhysicalMemory.h"
#include "Logging.h"
#include "core\kernel\exports\EmuKrnl.h" // For InitializeListHead(), etc.
#include <assert.h>

// See the links below for the details about the kernel structure LIST_ENTRY and the related functions
// https://www.codeproject.com/Articles/800404/Understanding-LIST-ENTRY-Lists-and-Its-Importance
// https://docs.microsoft.com/en-us/windows-hardware/drivers/kernel/singly-and-doubly-linked-lists
#define LIST_ENTRY_INITIALIZE(ListEntry) ((ListEntry)->Flink = (ListEntry)->Blink = nullptr)

inline FreeBlock* ListEntryToFreeBlock(xboxkrnl::PLIST_ENTRY pListEntry)
{
	return CONTAINING_RECORD(pListEntry, FreeBlock, ListEntry);
}

void PhysicalMemory::InitializePageDirectory()
{
	PMMPTE pPde;
	PMMPTE pPde_end;
	MMPTE TempPte;


	// Write the pde's of the WC (tiled) memory - no page tables
	TempPte.Default = ValidKernelPteBits;
	TempPte.Hardware.LargePage = 1;
	TempPte.Hardware.PFN = XBOX_WRITE_COMBINED_BASE >> PAGE_SHIFT;
	SET_WRITE_COMBINE(TempPte);
	pPde_end = GetPdeAddress(XBOX_WRITE_COMBINED_END);
	for (pPde = GetPdeAddress(XBOX_WRITE_COMBINED_BASE); pPde <= pPde_end; ++pPde)
	{
		WRITE_PTE(pPde, TempPte);
		TempPte.Default += LARGE_PAGE_SIZE; // increase PFN
	}


	// Write the pde's of the UC memory region - no page tables
	TempPte.Hardware.PFN = XBOX_UNCACHED_BASE >> PAGE_SHIFT;
	DISABLE_CACHING(TempPte);
	pPde_end = GetPdeAddress(XBOX_UNCACHED_END - 1);
	for (pPde = GetPdeAddress(XBOX_UNCACHED_BASE); pPde <= pPde_end; ++pPde)
	{
		WRITE_PTE(pPde, TempPte);
		TempPte.Default += LARGE_PAGE_SIZE; // increase PFN
	}


	// NOTE: we don't need to unmap the rest of the system physical region because that mapping is done by the 2BL
	// on the Xbox, which is not present here on Cxbx-Reloaded

	// Here we should also reserve some system pte's for the file system cache. However, the implementation of the kernel
	// file cache functions is basically non-existent at the moment and relies on ExAllocatePoolWithTag, which is not
	// correctly implemented. So, for now, we keep on ignoring this allocation
}

void PhysicalMemory::WritePfn(PFN pfn_start, PFN pfn_end, PMMPTE pPte, PageType BusyType, bool bZero)
{
	XBOX_PFN TempPF;

	if (bZero)
	{
		TempPF.Default = 0;
		while (pfn_start <= pfn_end)
		{
			if (m_MmLayoutRetail || m_MmLayoutDebug) {
				*XBOX_PFN_ELEMENT(pfn_start) = TempPF;
			}
			else { *CHIHIRO_PFN_ELEMENT(pfn_start) = TempPF; }

			m_PagesByUsage[BusyType]--;
			pfn_start++;
		}
	}
	else
	{
		while (pfn_start <= pfn_end)
		{
			TempPF.Default = 0;
			TempPF.Busy.Busy = 1;
			TempPF.Busy.BusyType = BusyType;
			if (BusyType != VirtualPageTableType && BusyType != SystemPageTableType) {
				TempPF.Busy.PteIndex = GetPteOffset(GetVAddrMappedByPte(pPte));
			}
			else { TempPF.PTPageFrame.PtesUsed = 0; } // we are writing a pfn of a PT

			if (m_MmLayoutRetail || m_MmLayoutDebug) {
				*XBOX_PFN_ELEMENT(pfn_start) = TempPF;
			}
			else { *CHIHIRO_PFN_ELEMENT(pfn_start) = TempPF; }

			m_PagesByUsage[BusyType]++;
			pfn_start++;
			pPte++;
		}
	}
}

void PhysicalMemory::WritePte(PMMPTE pPteStart, PMMPTE pPteEnd, MMPTE Pte, PFN pfn, bool bZero)
{
	// This function is intended to write pte's, not pde's. To write those, use (De)AllocatePT which will perform
	// all the necessary housekeeping. Also, the pde's mapping these pte's should have already being commited or else
	// GetPfnOfPT will assert

	PMMPTE PointerPte = pPteStart;
	PXBOX_PFN PTpfn;

	if (bZero)
	{
		while (PointerPte <= pPteEnd)
		{
			if (PointerPte == pPteStart || IsPteOnPdeBoundary(PointerPte))
			{
				PTpfn = GetPfnOfPT(PointerPte);
			}
			if (PointerPte->Default != 0)
			{
				WRITE_ZERO_PTE(PointerPte);
				PTpfn->PTPageFrame.PtesUsed--;
			}
			PointerPte++;
		}
	}
	else
	{
		while (PointerPte <= pPteEnd)
		{
			if (PointerPte == pPteStart || IsPteOnPdeBoundary(PointerPte))
			{
				PTpfn = GetPfnOfPT(PointerPte);
			}
			if (PointerPte->Default == 0)
			{
				Pte.Hardware.PFN = pfn;
				WRITE_PTE(PointerPte, Pte);
				PTpfn->PTPageFrame.PtesUsed++;
			}
			PointerPte++;
			pfn++;
		}
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

	// The caller should already guarantee that there are enough free pages available
	if (NumberOfPages == 0) { result = nullptr; return false; }

	if (PfnAlignment)
	{
		// Calculate some alignment parameters if one is requested

		PfnAlignmentMask = ~(PfnAlignment - 1);
		PfnAlignmentSubtraction = ((NumberOfPages + PfnAlignment - 1) & PfnAlignmentMask) - NumberOfPages + 1;
	}

	ListEntry = FreeList.Blink; // search from the top

	while (ListEntry != &FreeList)
	{
		if (ListEntryToFreeBlock(ListEntry)->size >= NumberOfPages) // search for a block with enough pages
		{
			PfnStart = ListEntryToFreeBlock(ListEntry)->start;
			PfnCount = ListEntryToFreeBlock(ListEntry)->size;
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
				IntersectionEnd = ((IntersectionEnd + 1) & PfnAlignmentMask) - PfnAlignmentSubtraction;

				if (IntersectionEnd < IntersectionStart || IntersectionEnd - IntersectionStart + 1 < NumberOfPages)
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

						RemoveEntryList(ListEntry);
						delete ListEntryToFreeBlock(ListEntry);
					}
					else { ListEntryToFreeBlock(ListEntry)->size = PfnCount; }
				}
				else
				{
					// Create a new block with the remaining pages after the block

					PFreeBlock block = new FreeBlock;
					block->start = IntersectionEnd + 1;
					block->size = PfnStart + PfnCount - IntersectionEnd - 1;
					LIST_ENTRY_INITIALIZE(&block->ListEntry);
					InsertHeadList(ListEntry, &block->ListEntry);

					PfnCount = IntersectionEnd - PfnStart - NumberOfPages + 1;
					if (!PfnCount)
					{
						// delete the entry if there is no free space left

						RemoveEntryList(ListEntry);
						delete ListEntryToFreeBlock(ListEntry);
					}
					else { ListEntryToFreeBlock(ListEntry)->size = PfnCount; }
				}
			}
			else
			{
				// Starting address of the free block is lower than the intersection start

				if (IntersectionEnd == PfnEnd)
				{
					// The free block extends before IntersectionStart

					PfnCount -= NumberOfPages;
					ListEntryToFreeBlock(ListEntry)->size = PfnCount;
				}
				else
				{
					// The free block extends in both directions

					PFreeBlock block = new FreeBlock;
					block->start = IntersectionEnd + 1;
					block->size = PfnStart + PfnCount - IntersectionEnd - 1;
					LIST_ENTRY_INITIALIZE(&block->ListEntry);
					InsertHeadList(ListEntry, &block->ListEntry);

					PfnCount = IntersectionEnd - PfnStart - NumberOfPages + 1;
					ListEntryToFreeBlock(ListEntry)->size = PfnCount;
				}
			}
			if (m_MmLayoutDebug && (PfnStart + PfnCount >= DEBUGKIT_FIRST_UPPER_HALF_PAGE)) {
				m_DebuggerPagesAvailable -= NumberOfPages;
				assert(m_DebuggerPagesAvailable <= DEBUGKIT_FIRST_UPPER_HALF_PAGE);
			}
			else {
				m_PhysicalPagesAvailable -= NumberOfPages;
				assert(m_PhysicalPagesAvailable <= m_HighestPage + 1);
			}
			*result = PfnStart + PfnCount;
			return true;
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
		if (ListEntryToFreeBlock(ListEntry)->start < start || ListEntry == &FreeList)
		{
			PFreeBlock block = new FreeBlock;
			block->start = start;
			block->size = size;
			LIST_ENTRY_INITIALIZE(&block->ListEntry);
			InsertHeadList(ListEntry, &block->ListEntry);

			// Ensure that we are not freeing a part of the previous block
			if (ListEntry != &FreeList) {
				assert(ListEntryToFreeBlock(ListEntry)->start +
					ListEntryToFreeBlock(ListEntry)->size - 1 < start);
			}

			ListEntry = ListEntry->Flink; // move to the new created block

			// Ensure that we are not freeing a part of the next block
			if (ListEntry->Flink != &FreeList) {
				assert(ListEntryToFreeBlock(ListEntry->Flink)->start > end);
			}

			// Check if merging is possible
			if (ListEntry->Flink != &FreeList &&
				start + size == ListEntryToFreeBlock(ListEntry->Flink)->start)
			{
				// Merge forward
				xboxkrnl::PLIST_ENTRY temp = ListEntry->Flink;
				ListEntryToFreeBlock(ListEntry)->size +=
					ListEntryToFreeBlock(temp)->size;
				RemoveEntryList(temp);
				delete ListEntryToFreeBlock(temp);
			}
			if (ListEntry->Blink != &FreeList &&
				ListEntryToFreeBlock(ListEntry->Blink)->start +
				ListEntryToFreeBlock(ListEntry->Blink)->size == start)
			{
				// Merge backward
				ListEntryToFreeBlock(ListEntry->Blink)->size +=
					ListEntryToFreeBlock(ListEntry)->size;
				RemoveEntryList(ListEntry);
				delete block;
			}

			if (m_MmLayoutDebug && (start >= DEBUGKIT_FIRST_UPPER_HALF_PAGE)) {
				m_DebuggerPagesAvailable += size;
				assert(m_DebuggerPagesAvailable <= DEBUGKIT_FIRST_UPPER_HALF_PAGE);
			}
			else {
				m_PhysicalPagesAvailable += size;
				assert(m_PhysicalPagesAvailable <= m_HighestPage + 1);
			}

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
		goto Fail; // unknown or not allowed flag specified
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
			Mask = ValidKernelPteBits;
		}
		break;

		default:
			goto Fail; // both are specified, wrong
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
			goto Fail; // both are specified, wrong
	}

	pPte->Default = Mask;

	return true;

	Fail:
	pPte = nullptr;
	return false;
}

bool PhysicalMemory::ConvertXboxToPteProtection(DWORD perms, PMMPTE pPte)
{
	ULONG Mask = 0;
	ULONG LowNibble;
	ULONG HighNibble;

	if (perms & ~(XBOX_PAGE_GUARD | XBOX_PAGE_NOCACHE | XBOX_PAGE_WRITECOMBINE | 0xFF))
	{
		goto Fail; // unknown or not allowed flag specified
	}

	//
	// None of the protection attributes are valid for "no access" pages.
	//

	if (perms & XBOX_PAGE_NOACCESS)
	{
		if (perms & (XBOX_PAGE_GUARD | XBOX_PAGE_NOCACHE | XBOX_PAGE_WRITECOMBINE))
		{
			goto Fail; // PAGE_NOACCESS cannot be specified together with the other access modifiers
		}
	}

	if ((perms & XBOX_PAGE_NOCACHE) && (perms & XBOX_PAGE_WRITECOMBINE))
	{
		goto Fail; // PAGE_NOCACHE and PAGE_WRITECOMBINE cannot be specified together
	}

	LowNibble = perms & 0xF;
	HighNibble = (perms >> 4) & 0xF;

	if ((LowNibble == 0 && HighNibble == 0) || (LowNibble != 0 && HighNibble != 0))
	{
		// High and low permission flags cannot be mixed together

		goto Fail;
	}

	if ((LowNibble | HighNibble) == 1 || (LowNibble | HighNibble) == 2) { Mask = PTE_READONLY; }
	else if ((LowNibble | HighNibble) == 4) { Mask = PTE_READWRITE; }
	else
	{
		// ergo720: all the other combinations are invalid. This effectively filters out XBOX_PAGE_WRITECOPY and
		// XBOX_PAGE_EXECUTE_WRITECOPY which, afaik, are unsupported on the Xbox

		goto Fail;
	}

	// Apply the rest of the access modifiers to the pte mask

	if ((perms & (XBOX_PAGE_NOACCESS | XBOX_PAGE_GUARD)) == 0) { Mask |= PTE_VALID_MASK; }
	else if (perms & XBOX_PAGE_GUARD) { Mask |= PTE_GUARD; }

	if (perms & XBOX_PAGE_NOCACHE) { Mask |= PTE_CACHE_DISABLE_MASK; }
	else if (perms & XBOX_PAGE_WRITECOMBINE) { Mask |= PTE_WRITE_THROUGH_MASK; }

	assert((Mask & ~(PTE_VALID_PROTECTION_MASK)) == 0); // ensure that we've created a valid permission mask

	pPte->Default = Mask;

	return true;

	Fail:
	pPte = nullptr;
	return false;
}

DWORD PhysicalMemory::ConvertPteToXboxProtection(ULONG PteMask)
{
	// This routine assumes that the pte has valid protection bits. If it doesn't, it can produce invalid
	// access permissions

	ULONG Protect;

	if (PteMask & PTE_READWRITE) { Protect = XBOX_PAGE_READWRITE; }
	else { Protect = XBOX_PAGE_READONLY; }

	if ((PteMask & PTE_VALID_MASK) == 0)
	{
		if (PteMask & PTE_GUARD) { Protect |= XBOX_PAGE_GUARD; }
		else { Protect = XBOX_PAGE_NOACCESS; }
	}

	if (PteMask & PTE_CACHE_DISABLE_MASK) { Protect |= XBOX_PAGE_NOCACHE; }
	else if (PteMask & PTE_WRITE_THROUGH_MASK) { Protect |= XBOX_PAGE_WRITECOMBINE; }

	return Protect;
}

DWORD PhysicalMemory::PatchXboxPermissions(DWORD Perms)
{
	// Usage notes: this routine expects the permissions to be already sanitized by ConvertXboxToSystemPteProtection or
	// similar. If not, it can produce incorrect results

	// ergo720: this checks if the specified Xbox permission mask has the execute flag enabled. If not, it adds it. This is
	// necessary because, afaik, the Xbox grants execute rights to all allocations, even if PAGE_EXECUTE was not specified

	if ((Perms >> 4) & 0xF)
	{
		// All high nibble flags grant execute rights, nothing to do

		return Perms;
	}

	if (Perms & XBOX_PAGE_NOACCESS)
	{
		// XBOX_PAGE_NOACCESS disables all access, nothing to do

		return Perms;
	}

	switch (Perms & (XBOX_PAGE_READONLY | XBOX_PAGE_READWRITE))
	{
		case 0:
		{
			// One of XBOX_PAGE_READONLY or XBOX_PAGE_READWRITE must be specified

			DBG_PRINTF("%s: Memory permissions bug detected\n", __func__);
			return XBOX_PAGE_EXECUTE_READWRITE;
		}

		case XBOX_PAGE_READONLY:
		{
			Perms &= (~XBOX_PAGE_READONLY);
			return Perms | XBOX_PAGE_EXECUTE_READ;
		}

		case XBOX_PAGE_READWRITE:
		{
			Perms &= (~XBOX_PAGE_READWRITE);
			return Perms | XBOX_PAGE_EXECUTE_READWRITE;
		}

		default:
		{
			// If we reach here it means that both XBOX_PAGE_READONLY and XBOX_PAGE_READWRITE were specified, and so the
			// input is probably invalid

			DBG_PRINTF("%s: Memory permissions bug detected\n", __func__);
			return XBOX_PAGE_EXECUTE_READWRITE;
		}
	}
}

DWORD PhysicalMemory::ConvertXboxToWinProtection(DWORD Perms)
{
	// This function assumes that the supplied permissions have been sanitized already

	DWORD Mask = 0;

	if (Perms & XBOX_PAGE_NOACCESS)
	{
		// PAGE_NOACCESS cannot be specified with anything else

		return PAGE_NOACCESS;
	}

	DWORD LowNibble = Perms & 0xF;
	DWORD HighNibble = (Perms >> 4) & 0xF;

	if (HighNibble)
	{
		if (HighNibble == 1) { Mask |= PAGE_EXECUTE; }
		else if (HighNibble == 2) { Mask |= PAGE_EXECUTE_READ; }
		else { Mask |= PAGE_EXECUTE_READWRITE; }
	}

	if (LowNibble)
	{
		if (LowNibble == 2) { Mask |= PAGE_READONLY; }
		else { Mask |= PAGE_READWRITE; }
	}

	// Even though PAGE_NOCACHE and PAGE_WRITECOMBINE are unsupported on shared memory, that's a limitation on our side,
	// this function still adds them if they are present

	switch (Perms & (XBOX_PAGE_GUARD | XBOX_PAGE_NOCACHE | XBOX_PAGE_WRITECOMBINE))
	{
		case 0:
			break;

		case XBOX_PAGE_GUARD:
		{
			Mask |= PAGE_GUARD;
			break;
		}

		case XBOX_PAGE_NOCACHE:
		{
			Mask |= PAGE_NOCACHE;
			break;
		}

		case XBOX_PAGE_WRITECOMBINE:
		{
			Mask |= PAGE_WRITECOMBINE;
			break;
		}

		default:
		{
			// If we reach here it means that more than one permission modifier was specified, and so the input is
			// probably invalid

			DBG_PRINTF("%s: Memory permissions bug detected\n", __func__);
			return PAGE_EXECUTE_READWRITE;
		}
	}
	return Mask;
}

bool PhysicalMemory::AllocatePT(size_t Size, VAddr addr)
{
	PFN pfn;
	PMMPTE pPde;
	MMPTE TempPte;
	PFN_COUNT PdeNumber = PAGES_SPANNED_LARGE(addr, Size);
	PFN_COUNT PTtoCommit = 0;
	PageType BusyType = SystemPageTableType;
	VAddr StartingAddr = addr;

	assert(Size);
	assert(addr);

	for (unsigned int i = 0; i < PdeNumber; ++i)
	{
		if (GetPdeAddress(StartingAddr)->Hardware.Valid == 0)
		{
			PTtoCommit++;
		}
		StartingAddr += LARGE_PAGE_SIZE;
	}

	if (!PTtoCommit)
	{
		// We don't need to commit any new page tables, so exit now

		return true;
	}

	if (m_PhysicalPagesAvailable < PTtoCommit)
	{
		// We don't have enough memory for PT's mapping this allocation

		return false;
	}
	if (addr <= HIGHEST_USER_ADDRESS) { BusyType = VirtualPageTableType; }
	StartingAddr = addr;

	// Now actually commit the PT's. Note that we won't construct the vma's for the PTs since they are outside of all
	// memory regions

	for (unsigned int i = 0; i < PdeNumber; ++i)
	{
		pPde = GetPdeAddress(StartingAddr);
		if (pPde->Hardware.Valid == 0)
		{
			// We grab one page at a time to avoid fragmentation issues. The maximum allowed page is m_MaxContiguousPfn
			// to keep AllocatePT from stealing nv2a/pfn pages during initialization

			RemoveFree(1, &pfn, 0, 0, m_MaxContiguousPfn);
			TempPte.Default = ValidKernelPdeBits;
			TempPte.Hardware.PFN = pfn;
			WRITE_PTE(pPde, TempPte);
			WritePfn(pfn, pfn, pPde, BusyType);
		}
		StartingAddr += LARGE_PAGE_SIZE;
	}

	return true;
}

void PhysicalMemory::DeallocatePT(size_t Size, VAddr addr)
{
	PMMPTE pPde;
	PXBOX_PFN PTpfn;
	PFN_COUNT PdeNumber = PAGES_SPANNED_LARGE(addr, Size);
	VAddr StartingAddr = addr;

	assert(Size);
	assert(addr);

	for (unsigned int i = 0; i < PdeNumber; ++i)
	{
		pPde = GetPdeAddress(StartingAddr);
		PTpfn = GetPfnOfPT(GetPteAddress(StartingAddr));

		if (PTpfn->PTPageFrame.PtesUsed == 0)
		{
			InsertFree(pPde->Hardware.PFN, pPde->Hardware.PFN);
			WritePfn(pPde->Hardware.PFN, pPde->Hardware.PFN, pPde, (PageType)PTpfn->PTPageFrame.BusyType, true);
			WRITE_ZERO_PTE(pPde);
		}
		StartingAddr += LARGE_PAGE_SIZE;
	}
}

bool PhysicalMemory::IsMappable(PFN_COUNT PagesRequested, bool bRetailRegion, bool bDebugRegion)
{
	bool ret = false;
	if (bRetailRegion && m_PhysicalPagesAvailable >= PagesRequested) { ret = true; }
	if (bDebugRegion && m_DebuggerPagesAvailable >= PagesRequested) { ret = true; }
	if (!ret) { EmuLog(LOG_LEVEL::WARNING, "Out of physical memory!"); }

	return ret;
}

PXBOX_PFN PhysicalMemory::GetPfnOfPT(PMMPTE pPte)
{
	PXBOX_PFN PTpfn;

	// GetPteAddress on a pte address will yield the corresponding pde which maps the supplied pte
	PMMPTE PointerPde = GetPteAddress(pPte);
	// PointerPde should have already been written to by AllocatePT
	assert(PointerPde->Hardware.Valid != 0);
	if (m_MmLayoutRetail || m_MmLayoutDebug) {
		PTpfn = XBOX_PFN_ELEMENT(PointerPde->Hardware.PFN);
	}
	else { PTpfn = CHIHIRO_PFN_ELEMENT(PointerPde->Hardware.PFN); }
	assert(PTpfn->PTPageFrame.Busy == 1);
	assert(PTpfn->PTPageFrame.BusyType == SystemPageTableType ||
		PTpfn->PTPageFrame.BusyType == VirtualPageTableType);

	return PTpfn;
}
