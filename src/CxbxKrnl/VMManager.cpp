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
// *  (c) 2017-2018      ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

// Acknowledgment:
// The core logic of the VMManager class is based on the virtual page management code of the citra emulator (GPLv2 license),
// with heavy changes and the addition of real page tables to better suit Cxbx-Reloaded and Xbox emulation.
// Citra website: https://citra-emu.org/


#include "VMManager.h"
#include "Logging.h"
#include "EmuShared.h"
#include <assert.h>


VMManager g_VMManager;


bool VirtualMemoryArea::CanBeMergedWith(const VirtualMemoryArea& next) const
{
	assert(base + size == next.base);

	if (type == VMAType::Free && next.type == VMAType::Free) { return true; }

	return false;
}

void VMManager::Initialize(HANDLE memory_view, HANDLE PT_view, bool bRestrict64MiB)
{
	// Set up the critical section to synchronize access
	InitializeCriticalSectionAndSpinCount(&m_CriticalSection, 0x400);

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	m_AllocationGranularity = si.dwAllocationGranularity;
	g_SystemMaxMemory = XBOX_MEMORY_SIZE;
	m_hContiguousFile = memory_view;
	m_hPTFile = PT_view;

	// Set up the structs tracking the memory regions
	ConstructMemoryRegion(LOWEST_USER_ADDRESS, HIGHEST_USER_ADDRESS, MemoryRegionType::User);
	ConstructMemoryRegion(CONTIGUOUS_MEMORY_BASE, CONTIGUOUS_MEMORY_XBOX_SIZE - 1, MemoryRegionType::Contiguous);
	ConstructMemoryRegion(SYSTEM_MEMORY_BASE, SYSTEM_MEMORY_END, MemoryRegionType::System);
	ConstructMemoryRegion(DEVKIT_MEMORY_BASE, DEVKIT_MEMORY_END, MemoryRegionType::Devkit);

	// Set up general memory variables according to the xbe type
	if (g_bIsChihiro)
	{
		m_MaxContiguousPfn = CHIHIRO_CONTIGUOUS_MEMORY_LIMIT;
		g_SystemMaxMemory = CHIHIRO_MEMORY_SIZE;
		m_PhysicalPagesAvailable = g_SystemMaxMemory >> PAGE_SHIFT;
		m_HighestPage = CHIHIRO_HIGHEST_PHYSICAL_PAGE;
		m_NV2AInstancePage = CHIHIRO_INSTANCE_PHYSICAL_PAGE;
		m_MemoryRegionArray[MemoryRegionType::Contiguous].RegionMap[0].size = CONTIGUOUS_MEMORY_CHIHIRO_SIZE;
	}
	else if (g_bIsDebug)
	{
		g_SystemMaxMemory = CHIHIRO_MEMORY_SIZE;
		m_DebuggerPagesAvailable = X64M_PHYSICAL_PAGE;
		m_HighestPage = CHIHIRO_HIGHEST_PHYSICAL_PAGE;
		m_MemoryRegionArray[MemoryRegionType::Contiguous].RegionMap[0].size = CONTIGUOUS_MEMORY_CHIHIRO_SIZE;
		if (bRestrict64MiB) { m_bAllowNonDebuggerOnTop64MiB = false; }
	}

	// Insert all the pages available on the system in the free list
	xboxkrnl::PLIST_ENTRY ListEntry = FreeList.Blink;
	PFreeBlock block = new FreeBlock;
	block->start = 0;
	block->size = m_HighestPage + 1;
	LIST_ENTRY_INITIALIZE(&block->ListEntry);
	LIST_ENTRY_INSERT_HEAD(ListEntry, &block->ListEntry);

	// Construct the page directory
	InitializePageDirectory();

	// Set up the pfn database
	FillMemoryUlong((void*)PAGE_TABLES_BASE, PAGE_TABLES_SIZE, 0);
	InitializePfnDatabase();

	// Restore persistent memory, if there is any
	RestorePersistentMemory();


	if (g_bIsChihiro) {
		printf("Page table for Chihiro initialized!\n");
	}
	else if (g_bIsDebug) {
		printf("Page table for Debug console initialized!\n");
	}
	else { printf("Page table for Retail console initialized!\n"); }
}

void VMManager::ConstructMemoryRegion(VAddr Start, VAddr End, MemoryRegionType Type)
{
	VirtualMemoryArea vma;
	vma.base = Start;
	vma.size = End + 1;
	m_MemoryRegionArray[Type].LastFree = m_MemoryRegionArray[Type].RegionMap.emplace(Start, vma).first;
}

void VMManager::DestroyMemoryRegions()
{
	// VirtualAlloc and MapViewOfFileEx cannot be used in the contiguous region so skip it
	for (int i = 0; i < MemoryRegionType::COUNT - 1; ++i)
	{
		for (auto it = m_MemoryRegionArray[i].RegionMap.begin(); it != m_MemoryRegionArray[i].RegionMap.end(); ++it)
		{
			if (it->second.type != VMAType::Free)
			{
				if (it->second.bFragmented)
				{
					VirtualFree((void*)it->first, 0, MEM_RELEASE);
				}
				else
				{
					UnmapViewOfFile((void*)(ROUND_DOWN(it->first, m_AllocationGranularity)));
				}
			}
		}
	}
}

void VMManager::InitializePfnDatabase()
{
	XBOX_PFN TempPF;
	PFN pfn;
	PFN pfn_end;
	PFN result;
	PMMPTE PointerPte;
	PMMPTE EndingPte;
	MMPTE TempPte;
	VAddr addr;


	// Default initialize all the entries of the PFN database
	TempPF.Default = 0;
	TempPF.Busy.Busy = 1;
	TempPF.Busy.LockCount = LOCK_COUNT_MAXIMUM;
	TempPF.Busy.BusyType = PageType::Unknown;
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
	AllocateContiguous(PAGE_SIZE, PAGE_DIRECTORY_PHYSICAL_ADDRESS, PAGE_DIRECTORY_PHYSICAL_ADDRESS + PAGE_SIZE - 1, 0, XBOX_PAGE_READWRITE);
	PersistMemory(CONTIGUOUS_MEMORY_BASE + PAGE_DIRECTORY_PHYSICAL_ADDRESS, PAGE_SIZE, true);


	// Construct the pfn's of the kernel pages
	AllocateContiguous(KERNEL_SIZE, XBE_IMAGE_BASE, XBE_IMAGE_BASE + KERNEL_SIZE - 1, 0, XBOX_PAGE_EXECUTE_READWRITE);
	PersistMemory(CONTIGUOUS_MEMORY_BASE + XBE_IMAGE_BASE, KERNEL_SIZE, true);


	// NOTE: we cannot just use AllocateContiguous for the pfn and the instance memory since they both need to do their
	// allocations above the contiguous limit, which is forbidden by the function. We also don't need to call
	// UpdateMemoryPermissions since the contiguous region has already R/W/E rights
	// ergo720: on devkits, the pfn allocation spans across the retail-debug region boundary (it's 16 pages before and
	// 16 pages after). I decided to split this 32 pages equally between the retail and debug regions, however, this is
	// just a guess of mine, I could be wrong on this...

	// Construct the pfn's of the pages holding the pfn database
	if (g_bIsRetail) {
		pfn = XBOX_PFN_DATABASE_PHYSICAL_PAGE;
		pfn_end = XBOX_PFN_DATABASE_PHYSICAL_PAGE + 16 - 1;
		addr = (VAddr)CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn);
		PointerPte = GetPteAddress(addr);
		EndingPte = GetPteAddress(CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn_end));
	}
	else if (g_bIsDebug) {
		pfn = XBOX_PFN_DATABASE_PHYSICAL_PAGE;
		pfn_end = XBOX_PFN_DATABASE_PHYSICAL_PAGE + 32 - 1;
		addr = (VAddr)CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn);
		PointerPte = GetPteAddress(addr);
		EndingPte = GetPteAddress(CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn_end));
	}
	else {
		pfn = CHIHIRO_PFN_DATABASE_PHYSICAL_PAGE;
		pfn_end = CHIHIRO_PFN_DATABASE_PHYSICAL_PAGE + 32 - 1;
		addr = (VAddr)CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn);
		PointerPte = GetPteAddress(addr);
		EndingPte = GetPteAddress(CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn_end));
	}
	TempPte.Default = ValidKernelPteBits | PTE_PERSIST_MASK;

	RemoveFree(pfn_end - pfn + 1, &result, 0, pfn, pfn_end);
	WritePfn(pfn, pfn_end, &TempPte, PageType::Contiguous);
	WritePte(PointerPte, EndingPte, TempPte, pfn);
	AllocatePT(EndingPte - PointerPte + 1, addr);
	ConstructVMA(addr, (pfn_end - pfn + 1) << PAGE_SHIFT, MemoryRegionType::Contiguous, VMAType::Allocated, false);

	if (g_bIsDebug) { m_PhysicalPagesAvailable += 16; m_DebuggerPagesAvailable -= 16; }


	/*int QuickReboot;
	g_EmuShared->GetBootFlags(&QuickReboot);
	if ((QuickReboot & BOOT_QUICK_REBOOT) == 0)
	{*/
		// Construct the pfn's of the pages holding the nv2a instance memory
		if (g_bIsRetail || g_bIsDebug) {
			pfn = XBOX_INSTANCE_PHYSICAL_PAGE;
			pfn_end = XBOX_INSTANCE_PHYSICAL_PAGE + NV2A_INSTANCE_PAGE_COUNT - 1;
			addr = (VAddr)CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn);
			PointerPte = GetPteAddress(addr);
			EndingPte = GetPteAddress(CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn_end));
		}
		else {
			pfn = CHIHIRO_INSTANCE_PHYSICAL_PAGE;
			pfn_end = CHIHIRO_INSTANCE_PHYSICAL_PAGE + NV2A_INSTANCE_PAGE_COUNT - 1;
			addr = (VAddr)CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn);
			PointerPte = GetPteAddress(addr);
			EndingPte = GetPteAddress(CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn_end));
		}
		TempPte.Default = ValidKernelPteBits;
		DISABLE_CACHING(TempPte);

		RemoveFree(pfn_end - pfn + 1, &result, 0, pfn, pfn_end);
		WritePfn(pfn, pfn_end, &TempPte, PageType::Contiguous);
		WritePte(PointerPte, EndingPte, TempPte, pfn);
		AllocatePT(EndingPte - PointerPte + 1, addr);
		ConstructVMA(addr, NV2A_INSTANCE_PAGE_COUNT << PAGE_SHIFT, MemoryRegionType::Contiguous, VMAType::Allocated, false);


		if (g_bIsDebug)
		{
			// Debug kits have two nv2a instance memory, another at the top of the 128 MiB

			pfn += DEBUGKIT_FIRST_UPPER_HALF_PAGE;
			pfn_end += DEBUGKIT_FIRST_UPPER_HALF_PAGE;
			addr = (VAddr)CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn);
			PointerPte = GetPteAddress(addr);
			EndingPte = GetPteAddress(CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn_end));

			RemoveFree(pfn_end - pfn + 1, &result, 0, pfn, pfn_end);
			WritePfn(pfn, pfn_end, &TempPte, PageType::Contiguous);
			WritePte(PointerPte, EndingPte, TempPte, pfn);
			AllocatePT(EndingPte - PointerPte + 1, addr);
			ConstructVMA(addr, NV2A_INSTANCE_PAGE_COUNT << PAGE_SHIFT, MemoryRegionType::Contiguous, VMAType::Allocated, false);
		}
	//}


	// Construct the pfn of the page used by D3D
	AllocateContiguous(PAGE_SIZE, D3D_PHYSICAL_PAGE, D3D_PHYSICAL_PAGE + PAGE_SIZE - 1, 0, XBOX_PAGE_READWRITE);
	PersistMemory(CONTIGUOUS_MEMORY_BASE, PAGE_SIZE, true);
}

void VMManager::ConstructVMA(VAddr Start, size_t Size, MemoryRegionType Type, VMAType VmaType, bool bFragFlag, DWORD Perms)
{
	VMAIter it_begin = m_MemoryRegionArray[Type].RegionMap.begin();
	VMAIter it_end = m_MemoryRegionArray[Type].RegionMap.end();
	VMAIter it;

	VMAIter vma_handle = CarveVMA(Start, Size, Type);
	VirtualMemoryArea& vma = vma_handle->second;
	vma.type = VmaType;
	vma.permissions = Perms;
	vma.bFragmented = bFragFlag;

	// Depending on the splitting done by CarveVMA, there is no guarantee that the next or previous vma's are free.
	// We are just going to iterate forward and backward until we find one.

	it = std::next(vma_handle);

	while (it != it_end)
	{
		if (it->second.type == VMAType::Free)
		{
			m_MemoryRegionArray[Type].LastFree = it;
			return;
		}
		++it;
	}

	if (vma_handle == it_begin)
	{
		// Already at the beginning of the map, bail out immediately

		EmuWarning("Can't find any more free space in the memory region %d! Virtual memory exhausted?", Type);
		m_MemoryRegionArray[Type].LastFree = m_MemoryRegionArray[Type].RegionMap.end();
		return;
	}

	it = std::prev(vma_handle);

	while (true)
	{
		if (it->second.type == VMAType::Free)
		{
			m_MemoryRegionArray[Type].LastFree = it;
			return;
		}

		if (it == it_begin) { break; }
		--it;
	}

	// ergo720: I don't expect this to happen since it would mean we have exhausted the virtual space in the memory region,
	// but it's just in case it does

	EmuWarning("Can't find any more free space in the memory region %d! Virtual memory exhausted?", Type);

	m_MemoryRegionArray[Type].LastFree = m_MemoryRegionArray[Type].RegionMap.end();

	return;
}

VAddr VMManager::DbgTestPte(VAddr addr, PMMPTE Pte, bool bWriteCheck)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(addr);
		LOG_FUNC_ARG(Pte);
		LOG_FUNC_ARG(bWriteCheck);
	LOG_FUNC_END;

	PMMPTE PointerPte;
	VAddr ret = 0;

	Lock();

	if (bWriteCheck)
	{
		if (!IsValidVirtualAddress(addr)) { Unlock(); RETURN(ret); }

		Pte->Default = 0;
		PointerPte = GetPdeAddress(addr);
		if (PointerPte->Hardware.LargePage == 0) { PointerPte = GetPteAddress(addr); }

		if (PointerPte->Hardware.Write == 0)
		{
			MMPTE TempPte = *PointerPte;
			*Pte = TempPte;
			TempPte.Hardware.Write = 1;
			WRITE_PTE(PointerPte, TempPte);
		}
		ret = addr;
	}
	else
	{
		if (Pte->Default != 0)
		{
			PointerPte = GetPdeAddress(addr);
			if (PointerPte->Hardware.LargePage == 0) { PointerPte = GetPteAddress(addr); }
			WRITE_PTE(PointerPte, *Pte);
		}
	}

	Unlock();

	RETURN(ret);
}

PFN_COUNT VMManager::QueryNumberOfFreeDebuggerPages()
{
	return m_DebuggerPagesAvailable;
}

void VMManager::MemoryStatistics(xboxkrnl::PMM_STATISTICS memory_statistics)
{
	memory_statistics->TotalPhysicalPages = g_SystemMaxMemory >> PAGE_SHIFT;
	memory_statistics->AvailablePages = m_PhysicalPagesAvailable;
	memory_statistics->VirtualMemoryBytesCommitted = m_ImageMemoryInUse + m_NonImageMemoryInUse;
	memory_statistics->VirtualMemoryBytesReserved = 0; // this is the num of bytes reserved with MEM_RESERVE by NtAllocateVirtualMemory
	memory_statistics->CachePagesCommitted = m_PagesByUsage[PageType::Cache];
	memory_statistics->PoolPagesCommitted = m_PagesByUsage[PageType::Pool];
	memory_statistics->StackPagesCommitted = m_PagesByUsage[PageType::Stack];
	memory_statistics->ImagePagesCommitted = m_ImageMemoryInUse;
}

VAddr VMManager::ClaimGpuMemory(size_t Size, size_t* BytesToSkip)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Size);
		LOG_FUNC_ARG(*BytesToSkip);
	LOG_FUNC_END;

	// Note that, even though devkits have 128 MiB, there's no need to have a different case for those, since the instance
	// memory is still located 0x10000 bytes from the top of memory just like retail consoles

	if (g_bIsChihiro)
		*BytesToSkip = 0;
	else
		*BytesToSkip = CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(X64M_PHYSICAL_PAGE) -
		CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(XBOX_INSTANCE_PHYSICAL_PAGE + NV2A_INSTANCE_PAGE_COUNT);

	if (Size != MAXULONG_PTR)
	{
		PFN pfn;
		PFN EndingPfn;
		PMMPTE PointerPte;
		PMMPTE EndingPte;
		MMPTE TempPte;

		// Actually deallocate the requested number of instance pages. Note that we can't just call DeallocateContiguous
		// since that function will always deallocate the entire original allocation

		Lock();

		Size = ROUND_UP_4K(Size);

		pfn = m_NV2AInstancePage + NV2A_INSTANCE_PAGE_COUNT - (ROUND_UP_4K(m_NV2AInstanceMemoryBytes) >> PAGE_SHIFT);
		EndingPfn = m_NV2AInstancePage + NV2A_INSTANCE_PAGE_COUNT - (Size >> PAGE_SHIFT) - 1;
		PointerPte = GetPteAddress(CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn));
		EndingPte = GetPteAddress(CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(EndingPfn));

		WritePte(PointerPte, EndingPte, TempPte, 0, true);
		WritePfn(pfn, EndingPfn, PointerPte, PageType::Contiguous, true);
		InsertFree(pfn, EndingPfn);
		DestructVMA(GetVMAIterator((VAddr)CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn), MemoryRegionType::Contiguous), MemoryRegionType::Contiguous);

		if (g_bIsDebug)
		{
			// Devkits have also another nv2a instance memory at the top of memory, so free also that
			// 3fe0: nv2a; 3ff0: pfn; 4000 + 3fe0: nv2a; 4000 + 3fe0 + 10: free

			pfn += DEBUGKIT_FIRST_UPPER_HALF_PAGE;
			EndingPfn += DEBUGKIT_FIRST_UPPER_HALF_PAGE;
			PointerPte = GetPteAddress(CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn));
			EndingPte = GetPteAddress(CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(EndingPfn));

			WritePte(PointerPte, EndingPte, TempPte, 0, true);
			WritePfn(pfn, EndingPfn, PointerPte, PageType::Contiguous, true);
			InsertFree(pfn, EndingPfn);
			DestructVMA(GetVMAIterator((VAddr)CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn), MemoryRegionType::Contiguous), MemoryRegionType::Contiguous);
		}
		m_NV2AInstanceMemoryBytes = Size;

		DbgPrintf("KNRL: MmClaimGpuInstanceMemory : Allocated bytes remaining = 0x%.8X\n", m_NV2AInstanceMemoryBytes);

		Unlock();
	}

	RETURN((VAddr)CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(m_HighestPage + 1) - *BytesToSkip);
}

void VMManager::PersistMemory(VAddr addr, size_t Size, bool bPersist)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(addr);
		LOG_FUNC_ARG(Size);
		LOG_FUNC_ARG(bPersist);
	LOG_FUNC_END;

	PMMPTE PointerPte;
	PMMPTE EndingPte;

	assert(IS_PHYSICAL_ADDRESS(addr)); // only contiguous memory can be made persistent

	Lock();

	PointerPte = GetPteAddress(addr);
	EndingPte = GetPteAddress(addr + Size - 1);

	while (PointerPte <= EndingPte)
	{
		PointerPte->Hardware.Persist = bPersist ? 1 : 0;
		PointerPte++;
	}

	if ((xboxkrnl::PVOID)addr == xboxkrnl::LaunchDataPage)
	{
		if (bPersist)
		{
			g_EmuShared->SetLaunchDataAddress(&addr);
			DbgPrintf("KNRL: Persisting LaunchDataPage\n");
		}
		else
		{
			addr = NULL;
			g_EmuShared->SetLaunchDataAddress(&addr);
			DbgPrintf("KNRL: Forgetting LaunchDataPage\n");
		}
	}

	Unlock();
}

void VMManager::RestorePersistentMemory()
{
	VAddr LaunchDataAddr;
	VAddr FrameBufferAddr;
	g_EmuShared->GetLaunchDataAddress(&LaunchDataAddr);
	g_EmuShared->GetFrameBufferAddress(&FrameBufferAddr);

	if (LaunchDataAddr)
	{
		xboxkrnl::LaunchDataPage = (xboxkrnl::LAUNCH_DATA_PAGE*)LaunchDataAddr;

		// Mark the launch page as allocated to prevent other allocations from overwriting it
		AllocateContiguous(PAGE_SIZE, LaunchDataAddr - CONTIGUOUS_MEMORY_BASE,
			LaunchDataAddr - CONTIGUOUS_MEMORY_BASE + PAGE_SIZE - 1, 0, XBOX_PAGE_READWRITE);
		LaunchDataAddr = NULL;
		g_EmuShared->SetLaunchDataAddress(&LaunchDataAddr);

		DbgPrintf("INIT: Restored LaunchDataPage\n");
	}

	if (FrameBufferAddr)
	{
		xboxkrnl::AvSavedDataAddress = (xboxkrnl::PVOID)FrameBufferAddr;

		// Retrieve the frame buffer size
		size_t FrameBufferSize = QuerySize(FrameBufferAddr);
		// Mark the frame buffer page as allocated to prevent other allocations from overwriting it
		AllocateContiguous(FrameBufferSize, FrameBufferAddr - CONTIGUOUS_MEMORY_BASE,
			FrameBufferAddr - CONTIGUOUS_MEMORY_BASE + FrameBufferSize - 1, 0, XBOX_PAGE_READWRITE);
		FrameBufferAddr = NULL;
		g_EmuShared->SetLaunchDataAddress(&FrameBufferAddr);

		DbgPrintf("INIT: Restored FrameBuffer\n");
	}
}

VAddr VMManager::Allocate(size_t Size)
{
	LOG_FUNC_ONE_ARG(Size);

	MMPTE TempPte;
	PMMPTE PointerPte;
	PMMPTE EndingPte;
	PFN pfn;
	PFN EndingPfn;
	PFN_COUNT PteNumber;
	VAddr addr;
	MappingFn MappingRoutine;
	bool bVAlloc = false;

	// ergo720: I'm not sure why this routine is needed at all, but its usage (together with AllocateZeroed) is quite
	// widespread in the D3D patches. I think that most of those functions should use the Nt or the heap functions instead,
	// but, until those are properly implemented, this routine is here to stay

	if (!Size) { RETURN(NULL); }

	Lock();

	PteNumber = ROUND_UP_4K(Size) >> PAGE_SHIFT;

	if (!IsMappable(PteNumber, true, m_bAllowNonDebuggerOnTop64MiB ? true : false)) { goto Fail; }

	if (RemoveFree(PteNumber, &pfn, 0, 0, g_bIsDebug && !m_bAllowNonDebuggerOnTop64MiB ? XBOX_HIGHEST_PHYSICAL_PAGE
		: m_HighestPage)) // MapViewOfFileEx path
	{
		MappingRoutine = &MapBlockWithMapViewOfFileEx;
		addr = MapMemoryBlock(MappingRoutine, MemoryRegionType::User, PteNumber, pfn);

		if (!addr)
		{
			InsertFree(pfn, pfn + PteNumber - 1);
			goto Fail;
		}
	}
	else // VirtualAlloc path
	{
		// We couldn't find a contiguous block to map with MapViewOfFileEx, so we try to salvage this allocation with
		// VirtualAlloc. Note that we don't try to map contiguous blocks from non-contiguous ones because we could run into
		// a race condition in which (for example) we can map the 1st block but in the midtime somebody else allocates in
		// our intended region before we could do.
		// With VirtualAlloc we arbitrarily choose to consume pages in the retail region, since we don't grab pages
		// from the free list here

		bVAlloc = true;
		MappingRoutine = &MapBlockWithVirtualAlloc;
		addr = MapMemoryBlock(MappingRoutine, MemoryRegionType::User, PteNumber, 0);

		if (!addr) { goto Fail; }

		// With VirtualAlloc we just consume free pages but we don't remove them from the free list
		m_PhysicalPagesAvailable -= PteNumber;
	}

	// check if we have to construct the PT's for this allocation
	if (!AllocatePT(PteNumber, addr))
	{
		if (bVAlloc)
		{
			VirtualFree((void*)addr, 0, MEM_RELEASE);
			m_PhysicalPagesAvailable += PteNumber;
		}
		else
		{
			UnmapViewOfFile((void*)ROUND_DOWN(addr, m_AllocationGranularity));
			InsertFree(pfn, pfn + PteNumber - 1);
		}
		goto Fail;
	}

	// Finally, write the pte's and the pfn's
	if (bVAlloc)
	{
		// With VirtualAlloc we identity map this allocation since it doesn't allocate physical pages

		pfn = addr >> PAGE_SHIFT;
	}

	PointerPte = GetPteAddress(addr);
	EndingPte = PointerPte + PteNumber - 1;
	EndingPfn = pfn + PteNumber - 1;

	WritePte(PointerPte, EndingPte, TempPte, pfn);

	if (!bVAlloc)
	{
		WritePfn(pfn, EndingPfn, PointerPte, PageType::VirtualMemory);
	}
	else
	{
		m_PagesByUsage[PageType::VirtualMemory] += PteNumber;
	}

	m_ImageMemoryInUse += (PteNumber << PAGE_SHIFT);

	ConstructVMA(addr, PteNumber << PAGE_SHIFT, MemoryRegionType::User, VMAType::Allocated, bVAlloc);
	UpdateMemoryPermissions(addr, PteNumber << PAGE_SHIFT, XBOX_PAGE_EXECUTE_READWRITE);

	Unlock();
	RETURN(addr);

	Fail:
	Unlock();
	RETURN(NULL);
}

VAddr VMManager::AllocateZeroed(size_t Size)
{
	LOG_FORWARD("g_VMManager.Allocate");

	VAddr addr = Allocate(Size);
	if (addr) { FillMemoryUlong((void*)addr, ROUND_UP_4K(Size), 0); }

	RETURN(addr);
}

VAddr VMManager::AllocateSystemMemory(PageType BusyType, DWORD Perms, size_t Size, bool bAddGuardPage)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(BusyType);
		LOG_FUNC_ARG(Perms);
		LOG_FUNC_ARG(Size);
		LOG_FUNC_ARG(bAddGuardPage);
	LOG_FUNC_END;

	MMPTE TempPte;
	PMMPTE PointerPte;
	PMMPTE EndingPte;
	PFN pfn;
	PFN EndingPfn;
	PFN LowestAcceptablePfn = 0;
	PFN HighestAcceptablePfn = XBOX_HIGHEST_PHYSICAL_PAGE;
	PFN_COUNT PteNumber;
	PFN_COUNT PagesNumber;
	VAddr addr;
	MappingFn MappingRoutine;
	bool bVAlloc = false;
	bool bDebuggerPage = false;
	MemoryRegionType MemoryType = MemoryRegionType::System;

	// NOTE: AllocateSystemMemory won't allocate a physical page for the guard page (if requested) and just adds an extra
	// unallocated virtual page in front of the mapped allocation. This means we need to only allocate the actual pages of
	// the allocation but still construct the vma with the extra page, otherwise successive mapping operations can put another
	// allocation next the this block, which will defeat the guard page purpose.

	if (!Size || !ConvertXboxToSystemPteProtection(Perms, &TempPte)) { RETURN(NULL); }

	Lock();

	PteNumber = ROUND_UP_4K(Size) >> PAGE_SHIFT;
	PagesNumber = PteNumber;

	if (bAddGuardPage) { PteNumber++; }
	if (BusyType == PageType::Debugger)
	{
		// Debugger pages are only allocated from the extra 64 MiB available on devkits and are mapped in the
		// devkit system region

		if (!IsMappable(PagesNumber, false, true)) { goto Fail; }
		LowestAcceptablePfn = DEBUGKIT_FIRST_UPPER_HALF_PAGE;
		HighestAcceptablePfn = m_HighestPage;
		MemoryType = MemoryRegionType::Devkit;
		bDebuggerPage = true;
	}
	else { if (!IsMappable(PagesNumber, true, false)) { goto Fail; } }

	// ergo720: if a guard page is requested, because we only commit the actual stack pages, there is the remote possibility
	// that the stack is mapped immediately after the end of a host allocation, since those will appear as free areas and
	// the VMManager can't know anything about those. In practice, I wouldn't worry about this case since, if emulation is
	// done properly, a game will never try to write beyond the stack bottom since that would imply a stack overflow

	if (RemoveFree(PagesNumber, &pfn, 0, LowestAcceptablePfn, HighestAcceptablePfn)) // MapViewOfFileEx path
	{
		MappingRoutine = &MapBlockWithMapViewOfFileEx;
		addr = MapMemoryBlock(MappingRoutine, MemoryType, PagesNumber, pfn);

		if (!addr)
		{
			InsertFree(pfn, pfn + PagesNumber - 1);
			goto Fail;
		}
	}
	else // VirtualAlloc path
	{
		// We couldn't find a contiguous block to map with MapViewOfFileEx, so we try to salvage this allocation with
		// VirtualAlloc. Note that we don't try to map contiguous blocks from non-contiguous ones because we could run into
		// a race condition in which (for example) we can map the 1st block but in the midtime somebody else allocates in
		// our intended region before we could do.
		// With VirtualAlloc we arbitrarily choose to consume pages in the retail region, since we don't grab pages
		// from the free list here

		bVAlloc = true;
		MappingRoutine = &MapBlockWithVirtualAlloc;
		addr = MapMemoryBlock(MappingRoutine, MemoryType, PagesNumber, 0);

		if (!addr) { goto Fail; }

		// With VirtualAlloc we just consume free pages but we don't remove them from the free list
		m_PhysicalPagesAvailable -= PagesNumber;
	}

	// check if we have to construct the PT's for this allocation
	if (!AllocatePT(PteNumber, addr))
	{
		// If we reach here it means we had enough memory for the allocation but not for PT's mapping it, so this
		// allocation must fail instead.

		if (bVAlloc)
		{
			VirtualFree((void*)addr, 0, MEM_RELEASE);
			m_PhysicalPagesAvailable += PagesNumber;
		}
		else
		{
			// Recalculate the granularity aligned addr
			UnmapViewOfFile((void*)ROUND_DOWN(addr, m_AllocationGranularity));
			InsertFree(pfn, pfn + PagesNumber - 1);
		}
		goto Fail;
	}

	// Finally, write the pte's and the pfn's
	if (bVAlloc)
	{
		// With VirtualAlloc we identity map this allocation since it doesn't allocate physical pages

		pfn = addr >> PAGE_SHIFT;
	}
	
	PointerPte = GetPteAddress(addr);
	if (bAddGuardPage)
	{
		WRITE_ZERO_PTE(PointerPte);
		PointerPte++;
	}
	EndingPte = PointerPte + PagesNumber - 1;
	EndingPfn = pfn + PagesNumber - 1;

	WritePte(PointerPte, EndingPte, TempPte, pfn);
	EndingPte->Hardware.GuardOrEnd = 1;

	if (bVAlloc)
	{
		// We don't write the pfn entries with VirtualAlloc since we didn't commit pages. However, we still need
		// to increase the page type usage

		m_PagesByUsage[BusyType] += PagesNumber;
	}
	else
	{
		WritePfn(pfn, EndingPfn, PointerPte, BusyType);
	}

	UpdateMemoryPermissions(addr, PagesNumber << PAGE_SHIFT, Perms);

	if (bAddGuardPage) { addr -= PAGE_SIZE; }

	ConstructVMA(addr, PteNumber << PAGE_SHIFT, MemoryType, VMAType::Allocated, bVAlloc);

	Unlock();
	RETURN(addr);

	Fail:
	Unlock();
	RETURN(NULL);
}

VAddr VMManager::AllocateContiguous(size_t Size, PAddr LowestAddress, PAddr HighestAddress, ULONG Alignment, DWORD Perms)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Size);
		LOG_FUNC_ARG(LowestAddress);
		LOG_FUNC_ARG(HighestAddress);
		LOG_FUNC_ARG(Alignment);
		LOG_FUNC_ARG(Perms);
	LOG_FUNC_END;

	MMPTE TempPte;
	PMMPTE PointerPte;
	PMMPTE EndingPte;
	PFN PfnAlignment;
	PFN LowerPfn;
	PFN HigherPfn;
	PFN pfn;
	PFN EndingPfn;
	PFN_COUNT PteNumber;
	VAddr addr;

	if (!Size || !ConvertXboxToSystemPteProtection(Perms, &TempPte)) { RETURN(NULL); }

	PteNumber = ROUND_UP_4K(Size) >> PAGE_SHIFT;
	LowerPfn = LowestAddress >> PAGE_SHIFT;
	HigherPfn = HighestAddress >> PAGE_SHIFT;
	PfnAlignment = Alignment >> PAGE_SHIFT;

	if (!IsMappable(PteNumber, true, false)) { goto Fail; }
	if (HigherPfn > m_MaxContiguousPfn) { HigherPfn = m_MaxContiguousPfn; }
	if (LowerPfn > HigherPfn) { LowerPfn = HigherPfn; }
	if (!PfnAlignment) { PfnAlignment = 1; }

	Lock();

	if (!RemoveFree(PteNumber, &pfn, PfnAlignment, LowerPfn, HigherPfn)) { goto Fail; }
	addr = CONTIGUOUS_MEMORY_BASE + (pfn << PAGE_SHIFT);

	assert(CHECK_ALIGNMENT(pfn, PfnAlignment)); // check if the page alignment is correct

	EndingPfn = pfn + PteNumber - 1;

	// check if we have to construct the PT's for this allocation
	if (!AllocatePT(PteNumber, addr))
	{
		InsertFree(pfn, EndingPfn);
		goto Fail;
	}

	// Finally, write the pte's and the pfn's
	PointerPte = GetPteAddress(addr);
	EndingPte = PointerPte + PteNumber - 1;

	WritePte(PointerPte, EndingPte, TempPte, pfn);
	WritePfn(pfn, EndingPfn, PointerPte, PageType::Contiguous);
	EndingPte->Hardware.GuardOrEnd = 1;

	ConstructVMA(addr, PteNumber << PAGE_SHIFT, MemoryRegionType::Contiguous, VMAType::Allocated, false);
	UpdateMemoryPermissions(addr, PteNumber << PAGE_SHIFT, Perms);

	Unlock();
	RETURN(addr);

	Fail:
	Unlock();
	RETURN(NULL);
}

VAddr VMManager::MapDeviceMemory(PAddr Paddr, size_t Size, DWORD Perms)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Paddr);
		LOG_FUNC_ARG(Size);
		LOG_FUNC_ARG(Perms);
	LOG_FUNC_END;

	MMPTE TempPte;
	PMMPTE PointerPte;
	PMMPTE EndingPte;
	PFN pfn;
	PFN_COUNT PteNumber;
	VAddr addr;
	MappingFn MappingRoutine;

	if (!Size || !ConvertXboxToSystemPteProtection(Perms, &TempPte)) { RETURN(NULL); }

	// Is it a physical address for hardware devices (flash, NV2A, etc) ?
	if (Paddr >= XBOX_WRITE_COMBINED_BASE /*&& Paddr + Size <= XBOX_UNCACHED_END*/)
	{
		// Return physical address as virtual (accesses will go through EmuException)
		// ergo720: actually, at the moment CxbxReserveNV2AMemory reserves only the region FD000000 - FE000000 but the
		// entire WC - UC region ending at FFBFFFFF is not. This means that is theoretically possible that the supplied
		// Paddr overlaps with some host allocations here. In practice, I don't expect to find them this high in
		// memory though

		RETURN(Paddr);
	}

	// The requested address is not a known device address so we have to create a mapping for it. Even though this won't
	// create any physical allocation for it, we still need to reserve the memory with VirtualAlloc. If we don't, then
	// we are likely to collide with other system allocations in the region at some point, which will overwrite and corrupt
	// the affected allocation. We only reserve the memory so that the access will trigger an access violation and it will
	// be handled by EmuException. However, RegisterBAR doesn't register any known device in the system region and so
	// EmuX86_Read/Write are going to fail with the error "Unknown address" and resume execution as it is...

	PteNumber = PAGES_SPANNED(Paddr, Size);

	Lock();

	MappingRoutine = &ReserveBlockWithVirtualAlloc;
	addr = MapMemoryBlock(MappingRoutine, MemoryRegionType::System, PteNumber, 0);

	if (!addr) { goto Fail; }

	// check if we have to construct the PT's for this allocation
	if (!AllocatePT(PteNumber, addr)) { goto Fail; }

	// Finally, write the pte's
	PointerPte = GetPteAddress(addr);
	EndingPte = PointerPte + PteNumber - 1;
	pfn = Paddr >> PAGE_SHIFT;

	WritePte(PointerPte, EndingPte, TempPte, pfn);

	ConstructVMA(addr, PteNumber << PAGE_SHIFT, MemoryRegionType::System, VMAType::Reserved, true);

	Unlock();
	RETURN(addr + BYTE_OFFSET(Paddr));

	Fail:
	Unlock();
	RETURN(NULL);
}

void VMManager::Deallocate(VAddr addr)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(addr);
	LOG_FUNC_END;

	MMPTE TempPte;
	PMMPTE StartingPte;
	PMMPTE EndingPte;
	PFN pfn;
	PFN EndingPfn;
	PFN_COUNT PteNumber;
	VMAIter it;

	assert(CHECK_ALIGNMENT(addr, PAGE_SIZE)); // all starting addresses in the user region are page aligned
	assert(IS_USER_ADDRESS(addr));

	Lock();

	it = CheckExistenceVMA(addr, MemoryRegionType::User);

	if (it == m_MemoryRegionArray[MemoryRegionType::User].RegionMap.end() || it->second.type == VMAType::Free)
	{
		Unlock();
		return;
	}

	StartingPte = GetPteAddress(addr);
	EndingPte = StartingPte + (it->second.size >> PAGE_SHIFT) - 1;

	pfn = StartingPte->Hardware.PFN;
	EndingPfn = pfn + (EndingPte - StartingPte);

	PteNumber = EndingPte - StartingPte + 1;

	if (it->second.bFragmented)
	{
		m_PhysicalPagesAvailable += PteNumber;
		m_PagesByUsage[PageType::VirtualMemory] -= PteNumber;
	}
	else
	{
		InsertFree(pfn, EndingPfn);
		WritePfn(pfn, EndingPfn, StartingPte, PageType::VirtualMemory, true);
	}

	m_ImageMemoryInUse -= it->second.size;

	WritePte(StartingPte, EndingPte, TempPte, 0, true);
	DestructVMA(it, MemoryRegionType::User);

	Unlock();
}

void VMManager::DeallocateContiguous(VAddr addr)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(addr);
	LOG_FUNC_END;

	MMPTE TempPte;
	PMMPTE StartingPte;
	PMMPTE EndingPte;
	PFN pfn;
	PFN EndingPfn;
	VMAIter it;

	assert(CHECK_ALIGNMENT(addr, PAGE_SIZE)); // all starting addresses in the contiguous region are page aligned
	assert(IS_PHYSICAL_ADDRESS(addr));

	Lock();

	it = CheckExistenceVMA(addr, MemoryRegionType::Contiguous);

	if (it == m_MemoryRegionArray[MemoryRegionType::Contiguous].RegionMap.end() || it->second.type == VMAType::Free)
	{
		Unlock();
		return;
	}

	StartingPte = GetPteAddress(addr);
	EndingPte = StartingPte + (it->second.size >> PAGE_SHIFT) - 1;

	pfn = StartingPte->Hardware.PFN;
	EndingPfn = pfn + (EndingPte - StartingPte);

	InsertFree(pfn, EndingPfn);
	WritePfn(pfn, EndingPfn, StartingPte, PageType::Contiguous, true);
	WritePte(StartingPte, EndingPte, TempPte, 0, true);
	DestructVMA(it, MemoryRegionType::Contiguous);

	Unlock();
}

PFN_COUNT VMManager::DeallocateSystemMemory(PageType BusyType, VAddr addr, size_t Size)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(addr);
		LOG_FUNC_ARG(Size);
	LOG_FUNC_END;

	MMPTE TempPte;
	PMMPTE StartingPte;
	PMMPTE EndingPte;
	PFN pfn;
	PFN EndingPfn;
	PFN_COUNT PteNumber;
	VMAIter it;
	MemoryRegionType MemoryType = MemoryRegionType::System;

	assert(CHECK_ALIGNMENT(addr, PAGE_SIZE)); // all starting addresses in the system region are page aligned

	Lock();

	if (BusyType == PageType::Debugger)
	{
		// ergo720: I'm not sure but MmDbgFreeMemory seems to be able to deallocate only a part of an allocation done by
		// MmDbgAllocateMemory. This is not a problem since CarveVMARange supports freeing only a part of an allocated
		// vma, but we won't pass its size to CheckExistenceVMA because it would fail the size check

		assert(IS_DEVKIT_ADDRESS(addr));
		MemoryType = MemoryRegionType::Devkit;
		it = CheckExistenceVMA(addr, MemoryType);
	}
	else
	{
		assert(IS_SYSTEM_ADDRESS(addr));
		it = CheckExistenceVMA(addr, MemoryType, ROUND_UP_4K(Size));
	}

	if (it == m_MemoryRegionArray[MemoryType].RegionMap.end() || it->second.type == VMAType::Free)
	{
		Unlock();
		RETURN(NULL);
	}

	StartingPte = GetPteAddress(addr);
	if (BusyType == PageType::Debugger)
	{
		EndingPte = StartingPte + (ROUND_UP_4K(Size) >> PAGE_SHIFT) - 1;
	}
	else
	{
		EndingPte = StartingPte + (it->second.size >> PAGE_SHIFT) - 1;
	}

	pfn = StartingPte->Hardware.PFN;
	EndingPfn = pfn + (EndingPte - StartingPte);

	PteNumber = EndingPte - StartingPte + 1;

	if (it->second.bFragmented)
	{
		m_PhysicalPagesAvailable += PteNumber;
		m_PagesByUsage[BusyType] -= PteNumber;
	}
	else
	{
		InsertFree(pfn, EndingPfn);
		WritePfn(pfn, EndingPfn, StartingPte, BusyType, true);
	}

	WritePte(StartingPte, EndingPte, TempPte, 0, true);
	DestructVMA(it, MemoryType);

	Unlock();
	RETURN(PteNumber);
}

void VMManager::UnmapDeviceMemory(VAddr addr, size_t Size)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(addr);
		LOG_FUNC_ARG(Size);
	LOG_FUNC_END;

	if (IS_SYSTEM_ADDRESS(addr))
	{
		// The allocation is inside the system region, so it must have been mapped by us. Unmap it

		MMPTE TempPte;
		PMMPTE StartingPte;
		PMMPTE EndingPte;
		PFN_COUNT PteNumber;
		VMAIter it;

		// The starting address of a device can be unaligned since MapDeviceMemory returns an offset from the aligned
		// mapped address, so we won't assert the address here

		Lock();

		it = CheckExistenceVMA(addr, MemoryRegionType::System, PAGES_SPANNED(addr, Size));

		if (it == m_MemoryRegionArray[MemoryRegionType::System].RegionMap.end() || it->second.type == VMAType::Free)
		{
			Unlock();
			return;
		}

		StartingPte = GetPteAddress(addr);
		EndingPte = StartingPte + (it->second.size >> PAGE_SHIFT) - 1;
		PteNumber = EndingPte - StartingPte + 1;

		WritePte(StartingPte, EndingPte, TempPte, 0, true);
		DestructVMA(it, MemoryRegionType::System);

		Unlock();
		return;
	}

	// Don't free hardware devices (flash, NV2A, etc) -> no operation
}

void VMManager::Protect(VAddr addr, size_t Size, DWORD NewPerms)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(addr);
		LOG_FUNC_ARG(Size);
		LOG_FUNC_ARG(NewPerms);
	LOG_FUNC_END;

	MMPTE TempPte;
	MMPTE NewPermsPte;
	PMMPTE PointerPte;
	PMMPTE EndingPte;

	assert(IS_PHYSICAL_ADDRESS(addr) || IS_SYSTEM_ADDRESS(addr));

	if (!Size || !ConvertXboxToSystemPteProtection(NewPerms, &NewPermsPte)) { return; }

	Lock();

	// This routine is allowed to change the protections of only a part of the pages of an allocation, and the supplied
	// address doesn't necessarily need to be the starting address of the vma, meaning we can't check the vma existence
	// with CheckExistenceVMA

	#ifdef _DEBUG_TRACE
	MemoryRegionType MemoryType = IS_PHYSICAL_ADDRESS(addr) ? MemoryRegionType::Contiguous : MemoryRegionType::System;
	VMAIter it = GetVMAIterator(addr, MemoryType);
	assert(it != m_MemoryRegionArray[MemoryType].RegionMap.end() && addr >= it->first &&
		addr < it->first + it->second.size && it->second.type != VMAType::Free);
	#endif

	PointerPte = GetPteAddress(addr);
	EndingPte = GetPteAddress(addr + Size - 1);

	while (PointerPte <= EndingPte)
	{
		TempPte = *PointerPte;

		if ((TempPte.Default & PTE_SYSTEM_PROTECTION_MASK) != NewPermsPte.Default)
		{
			// This zeroes the existent bit protections and applies the new ones

			TempPte.Default = ((TempPte.Default & ~PTE_SYSTEM_PROTECTION_MASK) | NewPermsPte.Default);
			WRITE_PTE(PointerPte, TempPte);
		}
		PointerPte++;
	}

	UpdateMemoryPermissions(addr, Size, NewPerms);

	Unlock();
}

DWORD VMManager::QueryProtection(VAddr addr)
{
	LOG_FUNC_ONE_ARG(addr);

	PMMPTE PointerPte;
	MMPTE TempPte;
	DWORD Protect;

	// This function can query any virtual address, even invalid ones, so we won't do any vma checks here

	Lock();

	PointerPte = GetPdeAddress(addr);
	TempPte = *PointerPte;

	if (TempPte.Hardware.Valid != 0)
	{
		if (TempPte.Hardware.LargePage == 0)
		{
			PointerPte = GetPteAddress(addr);
			TempPte = *PointerPte;

			if ((TempPte.Hardware.Valid != 0) || ((TempPte.Default != 0) && (addr <= HIGHEST_USER_ADDRESS)))
			{
				Protect = ConvertPteToXboxProtection(TempPte.Default);
			}
			else
			{
				Protect = 0; // invalid page, return failure
			}
		}
		else
		{
			Protect = ConvertPteToXboxProtection(TempPte.Default); // large page, query it immediately
		}
	}
	else
	{
		Protect = 0; // invalid page, return failure
	}

	Unlock();

	RETURN(Protect);
}

size_t VMManager::QuerySize(VAddr addr)
{
	LOG_FUNC_ONE_ARG(addr);

	PMMPTE PointerPte;
	PFN_COUNT PagesNumber;
	size_t Size = 0;

	Lock();

	if (IS_USER_ADDRESS(addr))
	{
		// This is designed to handle Cxbx callers that provide an offset instead of the beginning of the allocation. Such
		// callers should only allocate the corresponding memory with the generic Allocate or AllocateZeroed functions, or
		// else this will fail. At the moment this is indeed the case

		VMAIter it = GetVMAIterator(addr, MemoryRegionType::User);

		if (it != m_MemoryRegionArray[MemoryRegionType::User].RegionMap.end() && it->second.type != VMAType::Free)
		{
			Size = it->second.size;
		}
	}
	else
	{
		// This will only work for allocations made by MmAllocateContiguousMemory(Ex), MmAllocateSystemMemory and
		// MmCreateKernelStack which is what MmQueryAllocationSize expects. If they are not, this will either fault
		// or return an incorrect size of at least PAGE_SIZE

		PagesNumber = 1;
		PointerPte = GetPteAddress(addr);

		while (PointerPte->Hardware.GuardOrEnd == 0)
		{
			assert(PointerPte->Hardware.Valid != 0); // pte must be valid

			PagesNumber++;
			PointerPte++;
		}
		Size = PagesNumber << PAGE_SHIFT;
	}

	Unlock();

	RETURN(Size);
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

VAddr VMManager::MapMemoryBlock(MappingFn MappingRoutine, MemoryRegionType Type, PFN_COUNT PteNumber, PFN pfn)
{
	PFN pfn;
	VAddr addr;
	VMAIter it = m_MemoryRegionArray[Type].LastFree;
	size_t Size = PteNumber << PAGE_SHIFT;
	DWORD FileOffsetLow = 0;

	if (pfn) // MapViewOfFileEx specific
	{
		FileOffsetLow = ROUND_DOWN(pfn << PAGE_SHIFT, m_AllocationGranularity);
		Size = (pfn << PAGE_SHIFT) + Size - FileOffsetLow;
	}

	VMAIter end_it = m_MemoryRegionArray[Type].RegionMap.end();

	while (it != end_it)
	{
		if (it->second.type != VMAType::Free) // already allocated by the VMManager
		{
			++it;
			continue;
		}
		addr = it->first;
		if (!CHECK_ALIGNMENT(addr, m_AllocationGranularity)) // free vma
		{
			// addr is not aligned with the granularity of the host, jump to the next granularity boundary

			addr = ROUND_UP(addr, m_AllocationGranularity);
		}

		// Note that, even in free regions, somebody outside the manager could have allocated the memory so we just
		// keep on trying until we succeed or fail entirely.

		size_t vma_end = it->first + it->second.size;

		addr = (this->*MappingRoutine)(addr, Size, vma_end, FileOffsetLow, pfn);

		if (addr) { return addr; }

		++it;
	}

	// If we are here, it means we reached the end of the memory region. In desperation, we also try to map it from the
	// LastFree iterator and going backwards, since there could be holes created by deallocation operations...

	VMAIter begin_it = m_MemoryRegionArray[Type].RegionMap.begin();

	if (m_MemoryRegionArray[Type].LastFree == begin_it)
	{
		// We are already at the beginning of the map, so bail out immediately

		EmuWarning("Failed to map a memory block in the virtual region %d!", Type);
		return NULL;
	}

	it = std::prev(m_MemoryRegionArray[Type].LastFree);

	while (true)
	{
		if (it->second.type == VMAType::Free)
		{
			addr = it->first;

			if (!CHECK_ALIGNMENT(addr, m_AllocationGranularity))
			{
				addr = ROUND_UP(addr, m_AllocationGranularity);
			}

			size_t vma_end = it->first + it->second.size;

			addr = (this->*MappingRoutine)(addr, Size, vma_end, FileOffsetLow, pfn);

			if (addr) { return addr; }
		}

		if (it == begin_it) { break; }
		--it;
	}

	// We have failed to map the block. This is likely because the virtual space is fragmented or there are too many
	// host allocations in the memory region. Log this error and bail out

	EmuWarning("Failed to map a memory block in the virtual region %d!", Type);

	return NULL;
}

VAddr VMManager::MapBlockWithMapViewOfFileEx(VAddr StartingAddr, size_t ViewSize, size_t VmaEnd, DWORD OffsetLow, PFN pfn)
{
	for (; StartingAddr + ViewSize - 1 < VmaEnd; StartingAddr += m_AllocationGranularity)
	{
		if ((VAddr)MapViewOfFileEx(m_hContiguousFile, FILE_MAP_EXECUTE | FILE_MAP_READ | FILE_MAP_WRITE, 0, OffsetLow,
			ViewSize, (void*)StartingAddr) == StartingAddr)
		{
			return StartingAddr + (pfn << PAGE_SHIFT) - OffsetLow; // sum the offset into the file view
		}
	}
	return NULL;
}

VAddr VMManager::MapBlockWithVirtualAlloc(VAddr StartingAddr, size_t Size, size_t VmaEnd, DWORD Unused, PFN Unused2)
{
	for (; StartingAddr + Size - 1 < VmaEnd; StartingAddr += m_AllocationGranularity)
	{
		if ((VAddr)VirtualAlloc((void*)StartingAddr, Size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE) == StartingAddr)
		{
			return StartingAddr;
		}
	}
	return NULL;
}

VAddr VMManager::ReserveBlockWithVirtualAlloc(VAddr StartingAddr, size_t Size, size_t VmaEnd, DWORD Unused, PFN Unused2)
{
	for (; StartingAddr + Size - 1 < VmaEnd; StartingAddr += m_AllocationGranularity)
	{
		if ((VAddr)VirtualAlloc((void*)StartingAddr, Size, MEM_RESERVE, PAGE_NOACCESS) == StartingAddr)
		{
			return StartingAddr;
		}
	}
	return NULL;
}

bool VMManager::IsValidVirtualAddress(const VAddr addr)
{
	LOG_FUNC_ONE_ARG(addr);

	PMMPTE PointerPte;

	Lock();

	PointerPte = GetPdeAddress(addr);
	if (PointerPte->Hardware.Valid == 0) // invalid pde -> addr is invalid
		goto InvalidAddress;

	if (PointerPte->Hardware.LargePage != 0) // addr is backed by a large page
		goto ValidAddress;

	PointerPte = GetPteAddress(addr);
	if (PointerPte->Hardware.Valid == 0) // invalid pte -> addr is invalid
		goto InvalidAddress;

	// The following check is needed to handle the special case where the address being queried falls inside the PTs region.
	// The first-level pte is also a second-level pte for the pages in the 0xC0000000 region, that is, the pte's of the PTs
	// are the pde's themselves. The check makes sure that we are not fooled into thinking that the PT to which addr falls
	// into is valid because the corresponding pde is valid. Addr could still be invalid but the pde is marked valid simply
	// because it's mapping a large page instead of the queried PT.

	if (PointerPte->Hardware.LargePage != 0) // pte is actually a pde and it's mapping a large page -> addr is invalid
		goto InvalidAddress;

	// If we reach here, we have a valid pte -> addr is backed by a 4K page

	ValidAddress:
	Unlock();
	RETURN(true);

	InvalidAddress:
	Unlock();
	RETURN(false);
}

PAddr VMManager::TranslateVAddrToPAddr(const VAddr addr)
{
	LOG_FUNC_ONE_ARG(addr);

	PAddr PAddr;
	PMMPTE PointerPte;

	Lock();

	PointerPte = GetPdeAddress(addr);
	if (PointerPte->Hardware.Valid == 0) { // invalid pde -> addr is invalid
		goto InvalidAddress;
	}

	if (PointerPte->Hardware.LargePage == 0)
	{
		PointerPte = GetPteAddress(addr);
		if (PointerPte->Hardware.Valid == 0) { // invalid pte -> addr is invalid
			goto InvalidAddress;
		}
		PAddr = BYTE_OFFSET(addr); // valid pte -> addr is valid
	}
	else
	{
		PAddr = BYTE_OFFSET_LARGE(addr); // this is a large page, translate it immediately
	}

	PAddr += (PointerPte->Hardware.PFN << PAGE_SHIFT);

	Unlock();
	RETURN(PAddr);

	InvalidAddress:
	Unlock();
	RETURN(NULL);
}

void VMManager::Lock()
{
	EnterCriticalSection(&m_CriticalSection);
}

void VMManager::Unlock()
{
	LeaveCriticalSection(&m_CriticalSection);
}

VMAIter VMManager::UnmapVMA(VMAIter vma_handle, MemoryRegionType Type)
{
	VirtualMemoryArea& vma = vma_handle->second;
	vma.type = VMAType::Free;
	vma.permissions = XBOX_PAGE_NOACCESS;
	vma.bFragmented = false;

	return MergeAdjacentVMA(vma_handle, Type);
}

VMAIter VMManager::CarveVMA(VAddr base, size_t size, MemoryRegionType Type)
{
	VMAIter vma_handle = GetVMAIterator(base, Type);

	// base address is outside the range managed by this memory region
	assert(vma_handle != m_MemoryRegionArray[Type].RegionMap.end());

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
		SplitVMA(vma_handle, end_in_vma, Type);
	}
	if (start_in_vma != 0)
	{
		// split vma at the start of the allocated region
		vma_handle = SplitVMA(vma_handle, start_in_vma, Type);
	}

	return vma_handle;
}

VMAIter VMManager::CarveVMARange(VAddr base, size_t size, MemoryRegionType Type)
{
	VAddr target_end = base + size;
	assert(target_end >= base);
	assert(target_end <= MAX_VIRTUAL_ADDRESS);
	assert(size > 0);

	VMAIter begin_vma = GetVMAIterator(base, Type);
	VMAIter it_end = m_MemoryRegionArray[Type].RegionMap.lower_bound(target_end);
	for (auto it = begin_vma; it != it_end; ++it)
	{
		if (it->second.type == VMAType::Free) { assert(0); }
	}

	if (base != begin_vma->second.base)
	{
		begin_vma = SplitVMA(begin_vma, base - begin_vma->second.base, Type);
	}

	VMAIter end_vma = GetVMAIterator(target_end, Type);
	if (end_vma != m_MemoryRegionArray[Type].RegionMap.end() && target_end != end_vma->second.base)
	{
		end_vma = SplitVMA(end_vma, target_end - end_vma->second.base, Type);
	}

	return begin_vma;
}

VMAIter VMManager::GetVMAIterator(VAddr target, MemoryRegionType Type)
{
	return std::prev(m_MemoryRegionArray[Type].RegionMap.upper_bound(target));
}

VMAIter VMManager::SplitVMA(VMAIter vma_handle, u32 offset_in_vma, MemoryRegionType Type)
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
	return m_MemoryRegionArray[Type].RegionMap.emplace_hint(std::next(vma_handle), new_vma.base, new_vma);
}

VMAIter VMManager::MergeAdjacentVMA(VMAIter vma_handle, MemoryRegionType Type)
{
	VMAIter next_vma = std::next(vma_handle);
	if (next_vma != m_MemoryRegionArray[Type].RegionMap.end() && vma_handle->second.CanBeMergedWith(next_vma->second))
	{
		vma_handle->second.size += next_vma->second.size;
		m_MemoryRegionArray[Type].RegionMap.erase(next_vma);
	}

	if (vma_handle != m_MemoryRegionArray[Type].RegionMap.begin())
	{
		VMAIter prev_vma = std::prev(vma_handle);
		if (prev_vma->second.CanBeMergedWith(vma_handle->second))
		{
			prev_vma->second.size += vma_handle->second.size;
			m_MemoryRegionArray[Type].RegionMap.erase(vma_handle);
			vma_handle = prev_vma;
		}
	}

	return vma_handle;
}

void VMManager::UpdateMemoryPermissions(VAddr addr, size_t Size, DWORD Perms)
{
	// PAGE_WRITECOMBINE/PAGE_NOCACHE are not allowed for shared memory, unless SEC_WRITECOMBINE/SEC_NOCACHE flag 
	// was specified when calling the CreateFileMapping function. Considering that Cxbx doesn't emulate the caches,
	// it's probably safe to ignore these flags

	DWORD WindowsPerms = ConvertXboxToWinProtection(PatchXboxPermissions(Perms));

	DWORD dummy;
	if (!VirtualProtect((void*)addr, Size, WindowsPerms & ~(PAGE_WRITECOMBINE | PAGE_NOCACHE), &dummy))
	{
		DbgPrintf("VirtualProtect failed. The error code was %d\n", GetLastError());
	}
}

VMAIter VMManager::CheckExistenceVMA(VAddr addr, MemoryRegionType Type, size_t Size)
{
	// Note that this routine expects an aligned size when specified (since all vma's are allocated size aligned to a page
	// boundary). The caller should do this.

	VMAIter it = GetVMAIterator(addr, Type);
	if (it != m_MemoryRegionArray[Type].RegionMap.end() && it->first == addr)
	{
		if (Size)
		{
			if (it->second.size == Size)
			{
				return it;
			}
			DbgPrintf("Located vma but sizes don't match\n");
			return m_MemoryRegionArray[Type].RegionMap.end();
		}
		return it;
	}
	else
	{
		DbgPrintf("Vma not found or doesn't start at the supplied address\n");
		return m_MemoryRegionArray[Type].RegionMap.end();
	}
}

void VMManager::DestructVMA(VMAIter it, MemoryRegionType Type)
{
	BOOL ret;

	if (Type != MemoryRegionType::Contiguous)
	{
		if (it->second.bFragmented)
		{
			ret = VirtualFree((void*)it->first, 0, MEM_RELEASE);
		}
		else
		{
			ret = UnmapViewOfFile((void*)(ROUND_DOWN(it->first, m_AllocationGranularity)));
		}

		if (!ret)
		{
			DbgPrintf("Deallocation routine failed with error %d\n", GetLastError());
		}
	}

	VMAIter CarvedVmaIt = CarveVMARange(it->first, it->second.size, Type);

	VAddr target_end = it->first + it->second.size;
	VMAIter it_end = m_MemoryRegionArray[Type].RegionMap.end();
	VMAIter it_begin = m_MemoryRegionArray[Type].RegionMap.begin();

	// The comparison against the end of the range must be done using addresses since vma's can be
	// merged during this process, causing invalidation of the iterators
	while (CarvedVmaIt != it_end && CarvedVmaIt->second.base < target_end)
	{
		CarvedVmaIt = std::next(UnmapVMA(CarvedVmaIt, Type));
	}

	// If we free an entire vma (which should always be the case), prev(CarvedVmaIt) will be the freed vma. If it is not,
	// we'll do a standard search

	if (CarvedVmaIt != it_begin && std::prev(CarvedVmaIt)->second.type == VMAType::Free)
	{
		m_MemoryRegionArray[Type].LastFree = std::prev(CarvedVmaIt);
		return;
	}
	else
	{
		DbgPrintf("std::prev(CarvedVmaIt) was not free\n");

		it = CarvedVmaIt;

		while (it != it_end)
		{
			if (it->second.type == VMAType::Free)
			{
				m_MemoryRegionArray[Type].LastFree = it;
				return;
			}
			++it;
		}

		it = std::prev(CarvedVmaIt);

		while (true)
		{
			if (it->second.type == VMAType::Free)
			{
				m_MemoryRegionArray[Type].LastFree = it;
				return;
			}

			if (it == it_begin) { break; }
			--it;
		}

		EmuWarning("Can't find any more free space in the memory region %d! Virtual memory exhausted?", Type);

		m_MemoryRegionArray[Type].LastFree = m_MemoryRegionArray[Type].RegionMap.end();

		return;
	}
}
