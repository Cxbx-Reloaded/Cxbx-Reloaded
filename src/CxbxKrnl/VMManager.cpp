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


#define LOG_PREFIX CXBXR_MODULE::VMEM

#include "PoolManager.h"
#include "Logging.h"
#include "EmuShared.h"
#include <assert.h>


VMManager g_VMManager;


bool VirtualMemoryArea::CanBeMergedWith(const VirtualMemoryArea& next) const
{
	assert(base + size == next.base);

	if (type == FreeVma && next.type == FreeVma) { return true; }

	return false;
}

void VMManager::Initialize(HANDLE memory_view, HANDLE pagetables_view, int BootFlags)
{
	// Set up the critical section to synchronize accesses
	InitializeCriticalSectionAndSpinCount(&m_CriticalSection, 0x400);

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	m_AllocationGranularity = si.dwAllocationGranularity;
	g_SystemMaxMemory = XBOX_MEMORY_SIZE;
	m_hContiguousFile = memory_view;
	m_hPTFile = pagetables_view;

	// Set up the structs tracking the memory regions
	ConstructMemoryRegion(LOWEST_USER_ADDRESS, USER_MEMORY_SIZE, UserRegion);
	ConstructMemoryRegion(CONTIGUOUS_MEMORY_BASE, CONTIGUOUS_MEMORY_XBOX_SIZE, ContiguousRegion);
	ConstructMemoryRegion(SYSTEM_MEMORY_BASE, SYSTEM_MEMORY_SIZE, SystemRegion);
	ConstructMemoryRegion(DEVKIT_MEMORY_BASE, DEVKIT_MEMORY_SIZE, DevkitRegion);

	unsigned char PreviousLayout;
	if ((BootFlags & BOOT_QUICK_REBOOT) != 0)
	{
		// Restore the memory layout we were emulating in the previous session

		PreviousLayout = *(unsigned char*)(CONTIGUOUS_MEMORY_BASE + PAGE_SIZE - 9);
		m_MmLayoutChihiro = (PreviousLayout == MmChihiro);
		m_MmLayoutDebug = (PreviousLayout == MmDebug);
		m_MmLayoutRetail = (PreviousLayout == MmRetail);
	}
	else
	{
		// Save the type of xbe we are emulating in this emulation session. This information will be needed if the current xbe performs
		// a quick reboot

		m_MmLayoutChihiro = (g_XbeType == xtChihiro);
		m_MmLayoutDebug = (g_XbeType == xtDebug);
		m_MmLayoutRetail = (g_XbeType == xtRetail);

		PreviousLayout = m_MmLayoutChihiro ? MmChihiro : (m_MmLayoutDebug ? MmDebug : MmRetail);
	}

	// Set up general memory variables according to the xbe type
	if (m_MmLayoutChihiro)
	{
		m_MaxContiguousPfn = CHIHIRO_CONTIGUOUS_MEMORY_LIMIT;
		g_SystemMaxMemory = CHIHIRO_MEMORY_SIZE;
		m_PhysicalPagesAvailable = g_SystemMaxMemory >> PAGE_SHIFT;
		m_HighestPage = CHIHIRO_HIGHEST_PHYSICAL_PAGE;
		m_NV2AInstancePage = CHIHIRO_INSTANCE_PHYSICAL_PAGE;
		m_MemoryRegionArray[ContiguousRegion].RegionMap[CONTIGUOUS_MEMORY_BASE].size = CONTIGUOUS_MEMORY_CHIHIRO_SIZE;
	}
	else if (m_MmLayoutDebug)
	{
		g_SystemMaxMemory = CHIHIRO_MEMORY_SIZE;
		m_DebuggerPagesAvailable = X64M_PHYSICAL_PAGE;
		m_HighestPage = CHIHIRO_HIGHEST_PHYSICAL_PAGE;
		m_MemoryRegionArray[ContiguousRegion].RegionMap[CONTIGUOUS_MEMORY_BASE].size = CONTIGUOUS_MEMORY_CHIHIRO_SIZE;

		// Note that even if this is true, only the heap/Nt functions of the title are affected, the Mm functions
		// will still use only the lower 64 MiB and the same is true for the debugger pages, meaning they will only
		// use the upper extra 64 MiB regardless of this flag
		if (CxbxKrnl_Xbe->m_Header.dwInitFlags.bLimit64MB) { m_bAllowNonDebuggerOnTop64MiB = false; }
	}

	// Insert all the pages available on the system in the free list
	xboxkrnl::PLIST_ENTRY ListEntry = FreeList.Blink;
	PFreeBlock block = new FreeBlock;
	block->start = 0;
	block->size = m_HighestPage + 1;
	LIST_ENTRY_INITIALIZE(&block->ListEntry);
	LIST_ENTRY_INSERT_HEAD(ListEntry, &block->ListEntry);

	// Set up the pfn database
	if ((BootFlags & BOOT_QUICK_REBOOT) == 0) {

		// Quote from LukeUsher "Yeah, known issue. Contiguous memory persists more than it should so the framebuffer doesn't get cleared.
		// The memory manager needs updating to only persist areas of memory marked with MmPersistContiguousMemory and discard the rest.
		// But right now it persists the whole block". So we also clear the entire mapped memory.bin since we are not quick rebooting
		xboxkrnl::RtlFillMemoryUlong((void*)CONTIGUOUS_MEMORY_BASE, g_SystemMaxMemory, 0);
		xboxkrnl::RtlFillMemoryUlong((void*)PAGE_TABLES_BASE, PAGE_TABLES_SIZE, 0);
		*(unsigned char*)(CONTIGUOUS_MEMORY_BASE + PAGE_SIZE - 9) = PreviousLayout;
		InitializePfnDatabase();
	}
	else {
		// Restore persistent memory allocations, if there are any
		RestorePersistentMemory();
		ReinitializePfnDatabase();
	}

	// Initialize the pool manager
	g_PoolManager.InitializePool();

	// Construct the page directory
	InitializePageDirectory();

	// Reserve the xbe image memory. Doing this now allows us to avoid calling XbAllocateVirtualMemory later
	ConstructVMA(XBE_IMAGE_BASE, ROUND_UP_4K(CxbxKrnl_Xbe->m_Header.dwSizeofImage), UserRegion, ReservedVma, false, XBOX_PAGE_READWRITE);

	if (m_MmLayoutChihiro) {
		printf("Page table for Chihiro arcade initialized!\n");
	}
	else if (m_MmLayoutDebug) {
		printf("Page table for Debug console initialized!\n");
	}
	else { printf("Page table for Retail console initialized!\n"); }
}

void VMManager::ConstructMemoryRegion(VAddr Start, size_t Size, MemoryRegionType Type)
{
	VirtualMemoryArea vma;
	vma.base = Start;
	vma.size = Size;
	m_MemoryRegionArray[Type].LastFree = m_MemoryRegionArray[Type].RegionMap.emplace(Start, vma).first;
}

void VMManager::DestroyMemoryRegions()
{
	// VirtualAlloc and MapViewOfFileEx cannot be used in the contiguous region so skip it
	for (int i = 0; i < COUNTRegion - 1; ++i)
	{
		for (auto& it : m_MemoryRegionArray[i].RegionMap)
		{
			if (it.second.type != FreeVma && it.first >= XBE_MAX_VA)
			{
				if (it.second.bFragmented)
				{
					VirtualFree((void*)it.first, 0, MEM_RELEASE);
				}
				else
				{
					UnmapViewOfFile((void*)(ROUND_DOWN(it.first, m_AllocationGranularity)));
				}
			}
		}
	}
}

void VMManager::InitializePfnDatabase()
{
	PFN pfn;
	PFN pfn_end;
	PFN result;
	PMMPTE PointerPte;
	PMMPTE EndingPte;
	MMPTE TempPte;
	VAddr addr;


	// Construct the pfn of the page used by D3D
	AllocateContiguous(PAGE_SIZE, D3D_PHYSICAL_PAGE, D3D_PHYSICAL_PAGE + PAGE_SIZE - 1, 0, XBOX_PAGE_READWRITE);
	PersistMemory(CONTIGUOUS_MEMORY_BASE, PAGE_SIZE, true);


	// Construct the pfn of the page directory
	AllocateContiguous(PAGE_SIZE, PAGE_DIRECTORY_PHYSICAL_ADDRESS, PAGE_DIRECTORY_PHYSICAL_ADDRESS + PAGE_SIZE - 1, 0, XBOX_PAGE_READWRITE);
	PersistMemory(CONTIGUOUS_MEMORY_BASE + PAGE_DIRECTORY_PHYSICAL_ADDRESS, PAGE_SIZE, true);


	// Construct the pfn's of the kernel pages
	AllocateContiguous(KERNEL_SIZE, XBE_IMAGE_BASE, XBE_IMAGE_BASE + ROUND_UP_4K(KERNEL_SIZE) - 1, 0, XBOX_PAGE_READWRITE);
	PersistMemory(CONTIGUOUS_MEMORY_BASE + XBE_IMAGE_BASE, KERNEL_SIZE, true);


	// NOTE: we cannot just use AllocateContiguous for the pfn and the instance memory since they both need to do their
	// allocations above the contiguous limit, which is forbidden by the function. We also don't need to call
	// UpdateMemoryPermissions since the contiguous region has already R/W/E rights
	// ergo720: on devkits, the pfn allocation spans across the retail-debug region boundary (it's 16 pages before and
	// 16 pages after). I decided to split this 32 pages equally between the retail and debug regions, however, this is
	// just a guess of mine, I could be wrong on this...

	// Construct the pfn's of the pages holding the pfn database
	if (m_MmLayoutRetail) {
		pfn = XBOX_PFN_DATABASE_PHYSICAL_PAGE;
		pfn_end = XBOX_PFN_DATABASE_PHYSICAL_PAGE + 16 - 1;
	}
	else if (m_MmLayoutDebug) {
		pfn = XBOX_PFN_DATABASE_PHYSICAL_PAGE;
		pfn_end = XBOX_PFN_DATABASE_PHYSICAL_PAGE + 32 - 1;
	}
	else {
		pfn = CHIHIRO_PFN_DATABASE_PHYSICAL_PAGE;
		pfn_end = CHIHIRO_PFN_DATABASE_PHYSICAL_PAGE + 32 - 1;
	}
	addr = (VAddr)CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn);
	PointerPte = GetPteAddress(addr);
	EndingPte = GetPteAddress(CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn_end));
	TempPte.Default = ValidKernelPteBits | PTE_PERSIST_MASK;

	RemoveFree(pfn_end - pfn + 1, &result, 0, pfn, pfn_end);
	AllocatePT((EndingPte - PointerPte + 1) << PAGE_SHIFT, addr);
	WritePfn(pfn, pfn_end, &TempPte, UnknownType);
	WritePte(PointerPte, EndingPte, TempPte, pfn);
	ConstructVMA(addr, (pfn_end - pfn + 1) << PAGE_SHIFT, ContiguousRegion, AllocatedVma, false);

	if (m_MmLayoutDebug) { m_PhysicalPagesAvailable += 16; m_DebuggerPagesAvailable -= 16; }

	// Construct the pfn's of the pages holding the nv2a instance memory
	if (m_MmLayoutRetail || m_MmLayoutDebug) {
		pfn = XBOX_INSTANCE_PHYSICAL_PAGE;
		pfn_end = XBOX_INSTANCE_PHYSICAL_PAGE + NV2A_INSTANCE_PAGE_COUNT - 1;
	}
	else {
		pfn = CHIHIRO_INSTANCE_PHYSICAL_PAGE;
		pfn_end = CHIHIRO_INSTANCE_PHYSICAL_PAGE + NV2A_INSTANCE_PAGE_COUNT - 1;
	}
	addr = (VAddr)CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn);
	PointerPte = GetPteAddress(addr);
	EndingPte = GetPteAddress(CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn_end));
	TempPte.Default = ValidKernelPteBits;
	DISABLE_CACHING(TempPte);

	RemoveFree(pfn_end - pfn + 1, &result, 0, pfn, pfn_end);
	AllocatePT((EndingPte - PointerPte + 1) << PAGE_SHIFT, addr);
	WritePfn(pfn, pfn_end, &TempPte, ContiguousType);
	WritePte(PointerPte, EndingPte, TempPte, pfn);
	ConstructVMA(addr, NV2A_INSTANCE_PAGE_COUNT << PAGE_SHIFT, ContiguousRegion, AllocatedVma, false);


	if (m_MmLayoutDebug)
	{
		// Debug kits have two nv2a instance memory, another at the top of the 128 MiB

		pfn += DEBUGKIT_FIRST_UPPER_HALF_PAGE;
		pfn_end += DEBUGKIT_FIRST_UPPER_HALF_PAGE;
		addr = (VAddr)CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn);
		PointerPte = GetPteAddress(addr);
		EndingPte = GetPteAddress(CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn_end));

		RemoveFree(pfn_end - pfn + 1, &result, 0, pfn, pfn_end);
		AllocatePT((EndingPte - PointerPte + 1) << PAGE_SHIFT, addr);
		WritePfn(pfn, pfn_end, &TempPte, ContiguousType);
		WritePte(PointerPte, EndingPte, TempPte, pfn);
		ConstructVMA(addr, NV2A_INSTANCE_PAGE_COUNT << PAGE_SHIFT, ContiguousRegion, AllocatedVma, false);
	}
}

void VMManager::ReinitializePfnDatabase()
{
	// With a quick reboot the initialization is simplified since the previous pte's of the persistent allocations are carried over and
	// can be reused

	PFN pfn;
	PFN pfn_end;


	// Update the allocation of the page used by D3D
	RestorePersistentAllocation(CONTIGUOUS_MEMORY_BASE, D3D_PHYSICAL_PAGE, D3D_PHYSICAL_PAGE, ContiguousType);


	// Update the allocation of the page directory
	RestorePersistentAllocation(CONTIGUOUS_MEMORY_BASE + PAGE_DIRECTORY_PHYSICAL_ADDRESS, PAGE_DIRECTORY_PHYSICAL_ADDRESS >> PAGE_SHIFT,
		PAGE_DIRECTORY_PHYSICAL_ADDRESS >> PAGE_SHIFT, ContiguousType);


	// Update the allocation of the kernel
	RestorePersistentAllocation(CONTIGUOUS_MEMORY_BASE + XBE_IMAGE_BASE, XBE_IMAGE_BASE >> PAGE_SHIFT,
		((XBE_IMAGE_BASE + ROUND_UP_4K(KERNEL_SIZE)) >> PAGE_SHIFT) - 1, ContiguousType);


	// Update the allocation of the pfn database
	if (m_MmLayoutRetail) {
		pfn = XBOX_PFN_DATABASE_PHYSICAL_PAGE;
		pfn_end = XBOX_PFN_DATABASE_PHYSICAL_PAGE + 16 - 1;
	}
	else if (m_MmLayoutDebug) {
		pfn = XBOX_PFN_DATABASE_PHYSICAL_PAGE;
		pfn_end = XBOX_PFN_DATABASE_PHYSICAL_PAGE + 32 - 1;
	}
	else {
		pfn = CHIHIRO_PFN_DATABASE_PHYSICAL_PAGE;
		pfn_end = CHIHIRO_PFN_DATABASE_PHYSICAL_PAGE + 32 - 1;
	}
	RestorePersistentAllocation((VAddr)CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn), pfn, pfn_end, UnknownType);
	if (m_MmLayoutDebug) { m_PhysicalPagesAvailable += 16; m_DebuggerPagesAvailable -= 16; }


	{
		PFN result;
		MMPTE TempPte;
		VAddr addr;

		// Re-construct the allocation of the nv2a instance memory
		// NOTE: the entire instance memory is persisted during a quick reboot, however, it doesn't change anything to re-construct it
		// here since we would just have to move this block of code to a specific vmmanager reboot routine called in HalReturnToFirmware
		if (m_MmLayoutRetail || m_MmLayoutDebug) {
			pfn = XBOX_INSTANCE_PHYSICAL_PAGE;
			pfn_end = XBOX_INSTANCE_PHYSICAL_PAGE + NV2A_INSTANCE_PAGE_COUNT - 1;
		}
		else {
			pfn = CHIHIRO_INSTANCE_PHYSICAL_PAGE;
			pfn_end = CHIHIRO_INSTANCE_PHYSICAL_PAGE + NV2A_INSTANCE_PAGE_COUNT - 1;
		}
		addr = (VAddr)CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn);
		TempPte.Default = ValidKernelPteBits;
		DISABLE_CACHING(TempPte);

		RemoveFree(pfn_end - pfn + 1, &result, 0, pfn, pfn_end);
		RestorePersistentAllocation(addr, pfn, pfn_end, ContiguousType);
		WritePte(GetPteAddress(addr), GetPteAddress(CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn_end)), TempPte, pfn);


		if (m_MmLayoutDebug)
		{
			// Debug kits have two nv2a instance memory, another at the top of the 128 MiB

			pfn += DEBUGKIT_FIRST_UPPER_HALF_PAGE;
			pfn_end += DEBUGKIT_FIRST_UPPER_HALF_PAGE;
			addr = (VAddr)CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn);

			RemoveFree(pfn_end - pfn + 1, &result, 0, pfn, pfn_end);
			RestorePersistentAllocation(addr, pfn, pfn_end, ContiguousType);
			WritePte(GetPteAddress(addr), GetPteAddress(CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn_end)), TempPte, pfn);
		}
	}
}

void VMManager::RestorePersistentAllocation(VAddr addr, PFN StartingPfn, PFN EndingPfn, PageType Type)
{
	AllocatePT((EndingPfn - StartingPfn + 1) << PAGE_SHIFT, addr);
	ConstructVMA(addr, (EndingPfn - StartingPfn + 1) << PAGE_SHIFT, ContiguousRegion, AllocatedVma, false);
	WritePfn(StartingPfn, EndingPfn, GetPteAddress(addr), Type);
	GetPfnOfPT(GetPteAddress(addr))->PTPageFrame.PtesUsed += (EndingPfn - StartingPfn + 1);
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

	// Depending on the splitting done by CarveVMA and the type of the adiacent vma's, there is no guarantee that the next
	// or previous vma's are free. We are just going to iterate forward and backward until we find one.

	it = std::next(vma_handle);

	while (it != it_end)
	{
		if (it->second.type == FreeVma)
		{
			m_MemoryRegionArray[Type].LastFree = it;
			return;
		}
		++it;
	}

	if (vma_handle == it_begin)
	{
		// Already at the beginning of the map, bail out immediately

		EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "Can't find any more free space in the memory region %d! Virtual memory exhausted?", Type);
		m_MemoryRegionArray[Type].LastFree = m_MemoryRegionArray[Type].RegionMap.end();
		return;
	}

	it = std::prev(vma_handle);

	while (true)
	{
		if (it->second.type == FreeVma)
		{
			m_MemoryRegionArray[Type].LastFree = it;
			return;
		}

		if (it == it_begin) { break; }
		--it;
	}

	// ergo720: I don't expect this to happen since it would mean we have exhausted the virtual space in the memory region,
	// but it's just in case it does

	EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "Can't find any more free space in the memory region %d! Virtual memory exhausted?", Type);

	m_MemoryRegionArray[Type].LastFree = m_MemoryRegionArray[Type].RegionMap.end();

	return;
}

VAddr VMManager::DbgTestPte(VAddr addr, PMMPTE Pte, bool bWriteCheck)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(addr)
		LOG_FUNC_ARG(*(PULONG)Pte)
		LOG_FUNC_ARG(bWriteCheck)
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
	Lock();

	memory_statistics->TotalPhysicalPages = g_SystemMaxMemory >> PAGE_SHIFT;
	memory_statistics->AvailablePages = m_MmLayoutDebug && m_bAllowNonDebuggerOnTop64MiB ?
		m_PhysicalPagesAvailable + m_DebuggerPagesAvailable : m_PhysicalPagesAvailable;
	memory_statistics->VirtualMemoryBytesCommitted = (m_PagesByUsage[VirtualMemoryType] +
		m_PagesByUsage[ImageType]) << PAGE_SHIFT;
	memory_statistics->VirtualMemoryBytesReserved = m_VirtualMemoryBytesReserved;
	memory_statistics->CachePagesCommitted = m_PagesByUsage[CacheType];
	memory_statistics->PoolPagesCommitted = m_PagesByUsage[PoolType];
	memory_statistics->StackPagesCommitted = m_PagesByUsage[StackType];
	memory_statistics->ImagePagesCommitted = m_PagesByUsage[ImageType];

	Unlock();
}

VAddr VMManager::ClaimGpuMemory(size_t Size, size_t* BytesToSkip)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Size)
		LOG_FUNC_ARG(*BytesToSkip)
	LOG_FUNC_END;

	// Note that, even though devkits have 128 MiB, there's no need to have a different case for those, since the instance
	// memory is still located 0x10000 bytes from the top of memory just like retail consoles

	if (m_MmLayoutChihiro)
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

		// Actually deallocate the requested number of instance pages. Note that we can't just call DeallocateContiguous
		// since that function will always deallocate the entire original allocation

		Lock();

		Size = ROUND_UP_4K(Size);

		// This check is necessary because some games (e.g. Halo) call this twice but they provide the same size, meaning that we don't need
		// to change anything
		if (Size != m_NV2AInstanceMemoryBytes)
		{
			pfn = m_NV2AInstancePage + NV2A_INSTANCE_PAGE_COUNT - (ROUND_UP_4K(m_NV2AInstanceMemoryBytes) >> PAGE_SHIFT);
			EndingPfn = m_NV2AInstancePage + NV2A_INSTANCE_PAGE_COUNT - (Size >> PAGE_SHIFT) - 1;
			PointerPte = GetPteAddress(CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn));
			EndingPte = GetPteAddress(CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(EndingPfn));

			WritePte(PointerPte, EndingPte, *PointerPte, 0, true);
			WritePfn(pfn, EndingPfn, PointerPte, ContiguousType, true);
			InsertFree(pfn, EndingPfn);
			DestructVMA((VAddr)CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn), ContiguousRegion, m_NV2AInstanceMemoryBytes - Size);

			if (m_MmLayoutDebug)
			{
				// Devkits have also another nv2a instance memory at the top of memory, so free also that
				// 3fe0: nv2a; 3ff0: pfn; 4000 + 3fe0: nv2a; 4000 + 3fe0 + 10: free

				pfn += DEBUGKIT_FIRST_UPPER_HALF_PAGE;
				EndingPfn += DEBUGKIT_FIRST_UPPER_HALF_PAGE;
				PointerPte = GetPteAddress(CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn));
				EndingPte = GetPteAddress(CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(EndingPfn));

				WritePte(PointerPte, EndingPte, *PointerPte, 0, true);
				WritePfn(pfn, EndingPfn, PointerPte, ContiguousType, true);
				InsertFree(pfn, EndingPfn);
				DestructVMA((VAddr)CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn), ContiguousRegion, m_NV2AInstanceMemoryBytes - Size);
			}
		}
		m_NV2AInstanceMemoryBytes = Size;

		DbgPrintf(LOG_PREFIX, "MmClaimGpuInstanceMemory : Allocated bytes remaining = 0x%.8X\n", m_NV2AInstanceMemoryBytes);

		Unlock();
	}

	RETURN((VAddr)CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(m_HighestPage + 1) - *BytesToSkip);
}

void VMManager::PersistMemory(VAddr addr, size_t Size, bool bPersist)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(addr)
		LOG_FUNC_ARG(Size)
		LOG_FUNC_ARG(bPersist)
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

	// Now, if the supplied address is that of the launch data page or the frame buffer, then we store it inside the free
	// space of the allocation of the d3d page, since we know it's persisted and it's always located at 0x80000000

	if (addr != CONTIGUOUS_MEMORY_BASE && // D3D
		addr != CONTIGUOUS_MEMORY_BASE + PAGE_DIRECTORY_PHYSICAL_ADDRESS && // page directory
		addr != CONTIGUOUS_MEMORY_BASE + XBE_IMAGE_BASE && // dummy kernel
		addr != CONTIGUOUS_MEMORY_BASE + (VAddr)CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(XBOX_PFN_DATABASE_PHYSICAL_PAGE) && // pfn
		addr != CONTIGUOUS_MEMORY_BASE + (VAddr)CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(CHIHIRO_PFN_DATABASE_PHYSICAL_PAGE) // pfn
		)
	{
		if ((xboxkrnl::PVOID)addr == xboxkrnl::LaunchDataPage)
		{
			if (bPersist)
			{
				*(VAddr*)(CONTIGUOUS_MEMORY_BASE + PAGE_SIZE - 4) = addr;
				DbgPrintf(LOG_PREFIX, "Persisting LaunchDataPage\n");
			}
			else
			{
				*(VAddr*)(CONTIGUOUS_MEMORY_BASE + PAGE_SIZE - 4) = NULL;
				DbgPrintf(LOG_PREFIX, "Forgetting LaunchDataPage\n");
			}
		}
		else
		{
			if (bPersist)
			{
				*(VAddr*)(CONTIGUOUS_MEMORY_BASE + PAGE_SIZE - 8) = addr;
				DbgPrintf(LOG_PREFIX, "Persisting FrameBuffer\n");
			}
			else
			{
				*(VAddr*)(CONTIGUOUS_MEMORY_BASE + PAGE_SIZE - 8) = NULL;
				DbgPrintf(LOG_PREFIX, "Forgetting FrameBuffer\n");
			}
		}
	}


	Unlock();
}

void VMManager::RestorePersistentMemory()
{
	// We are going to loop on the contiguous pte region looking for persistent memory marked by the Persist bit in the pte.
	// If we find them we keep the pte in that state and remove the page from the free list, otherwise we zero the pte.

	PMMPTE PointerPte = GetPteAddress(CONTIGUOUS_MEMORY_BASE);
	PMMPTE EndingPte;

	if (m_MmLayoutRetail) {
		EndingPte = GetPteAddress(CONTIGUOUS_MEMORY_BASE + CONTIGUOUS_MEMORY_XBOX_SIZE - 1);
	}
	else {
		EndingPte = GetPteAddress(CONTIGUOUS_MEMORY_BASE + CONTIGUOUS_MEMORY_CHIHIRO_SIZE - 1);
	}

	while (PointerPte <= EndingPte)
	{
		PFN pfn;
		if (PointerPte->Hardware.Valid != 0 && PointerPte->Hardware.Persist != 0) {
			RemoveFree(1, &pfn, 0, PointerPte->Hardware.PFN, PointerPte->Hardware.PFN);
		}
		else {
			// Clear also the page. We could use RtlFillMemoryULong, but that will fill up the kernel log quite quickly, so we just
			// use memset instead
			memset((void*)(GetVAddrMappedByPte(PointerPte)), 0, PAGE_SIZE);
			WRITE_ZERO_PTE(PointerPte);
		}
		PointerPte++;
	}

	// Zero all the remaining pte's
	EndingPte += 1;
	xboxkrnl::RtlFillMemoryUlong((void*)PAGE_TABLES_BASE, (VAddr)GetPteAddress(CONTIGUOUS_MEMORY_BASE) - PAGE_TABLES_BASE, 0);
	xboxkrnl::RtlFillMemoryUlong((void*)EndingPte, PAGE_TABLES_END + 1 - (VAddr)EndingPte, 0);

	// Zero all the entries of the PFN database
	if (m_MmLayoutRetail) {
		xboxkrnl::RtlFillMemoryUlong((void*)XBOX_PFN_ADDRESS, X64KB, 0); // Xbox: 64 KiB
	}
	else if (m_MmLayoutChihiro) {
		xboxkrnl::RtlFillMemoryUlong((void*)CHIHIRO_PFN_ADDRESS, X64KB * 2, 0); // Chihiro: 128 KiB
	}
	else {
		xboxkrnl::RtlFillMemoryUlong((void*)XBOX_PFN_ADDRESS, X64KB * 2, 0); // Debug: 128 KiB
	}

	// Now we need to restore the launch data page and the frame buffer pointers to their correct values

	{
		VAddr LauchDataAddress = *(VAddr*)(CONTIGUOUS_MEMORY_BASE + PAGE_SIZE - 4);
		VAddr FrameBufferAddress = *(VAddr*)(CONTIGUOUS_MEMORY_BASE + PAGE_SIZE - 8);

		if (LauchDataAddress != 0 && IS_PHYSICAL_ADDRESS(LauchDataAddress)) {
			xboxkrnl::LaunchDataPage = (xboxkrnl::PLAUNCH_DATA_PAGE)LauchDataAddress;
			*(VAddr*)(CONTIGUOUS_MEMORY_BASE + PAGE_SIZE - 4) = NULL;

			RestorePersistentAllocation(LauchDataAddress, GetPteAddress(LauchDataAddress)->Hardware.PFN,
				GetPteAddress(LauchDataAddress)->Hardware.PFN, ContiguousType);

			DbgPrintf(LOG_PREFIX, "Restored LaunchDataPage\n");
		}

		if (FrameBufferAddress != 0 && IS_PHYSICAL_ADDRESS(FrameBufferAddress)) {
			xboxkrnl::AvSavedDataAddress = (xboxkrnl::PVOID)FrameBufferAddress;
			*(VAddr*)(CONTIGUOUS_MEMORY_BASE + PAGE_SIZE - 8) = NULL;

			RestorePersistentAllocation(FrameBufferAddress, GetPteAddress(FrameBufferAddress)->Hardware.PFN,
				GetPteAddress(FrameBufferAddress)->Hardware.PFN + (QuerySize(FrameBufferAddress, false) >> PAGE_SHIFT) - 1, ContiguousType);

			DbgPrintf(LOG_PREFIX, "Restored FrameBuffer\n");
		}
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

	if (!IsMappable(PteNumber, true, m_MmLayoutDebug && m_bAllowNonDebuggerOnTop64MiB ? true : false)) { goto Fail; }

	ConvertXboxToPteProtection(XBOX_PAGE_EXECUTE_READWRITE, &TempPte);

	if (RemoveFree(PteNumber, &pfn, 0, 0, m_MmLayoutDebug && !m_bAllowNonDebuggerOnTop64MiB ? XBOX_HIGHEST_PHYSICAL_PAGE
		: m_HighestPage)) // MapViewOfFileEx path
	{
		MappingRoutine = &VMManager::MapBlockWithMapViewOfFileEx;
		addr = MapMemoryBlock(MappingRoutine, UserRegion, PteNumber, pfn);

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

		bVAlloc = true;
		MappingRoutine = &VMManager::MapBlockWithVirtualAlloc;
		addr = MapMemoryBlock(MappingRoutine, UserRegion, PteNumber, 0);

		if (!addr) { goto Fail; }
	}

	// check if we have to construct the PT's for this allocation
	if (!AllocatePT(PteNumber << PAGE_SHIFT, addr))
	{
		if (bVAlloc)
		{
			VirtualFree((void*)addr, 0, MEM_RELEASE);
		}
		else
		{
			UnmapViewOfFile((void*)ROUND_DOWN(addr, m_AllocationGranularity));
			InsertFree(pfn, pfn + PteNumber - 1);
		}
		goto Fail;
	}

	// Finally, write the pte's and the pfn's
	PointerPte = GetPteAddress(addr);
	EndingPte = PointerPte + PteNumber - 1;

	if (bVAlloc)
	{
		PFN TempPfn;
		// With VirtualAlloc we grab one page at a time to avoid fragmentation issues
		while (PointerPte <= EndingPte)
		{
			RemoveFree(1, &TempPfn, 0, 0, m_MmLayoutDebug && !m_bAllowNonDebuggerOnTop64MiB ? XBOX_HIGHEST_PHYSICAL_PAGE
				: m_HighestPage);
			WritePfn(TempPfn, TempPfn, PointerPte, ImageType);
			WritePte(PointerPte, PointerPte, TempPte, TempPfn);

			PointerPte++;
		}
	}
	else
	{
		EndingPfn = pfn + PteNumber - 1;
		WritePfn(pfn, EndingPfn, PointerPte, ImageType);
		WritePte(PointerPte, EndingPte, TempPte, pfn);
	}

	ConstructVMA(addr, PteNumber << PAGE_SHIFT, UserRegion, AllocatedVma, bVAlloc);
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
	if (addr) { xboxkrnl::RtlFillMemoryUlong((void*)addr, ROUND_UP_4K(Size), 0); }

	RETURN(addr);
}

VAddr VMManager::AllocateSystemMemory(PageType BusyType, DWORD Perms, size_t Size, bool bAddGuardPage)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(BusyType)
		LOG_FUNC_ARG(Perms)
		LOG_FUNC_ARG(Size)
		LOG_FUNC_ARG(bAddGuardPage)
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
	MemoryRegionType MemoryType = SystemRegion;

	// NOTE: AllocateSystemMemory won't allocate a physical page for the guard page (if requested) and just adds an extra
	// unallocated virtual page in front of the mapped allocation. For this reason we will decommmit later the extra guard page allocated

	if (!Size || !ConvertXboxToSystemPteProtection(Perms, &TempPte)) { RETURN(NULL); }

	Lock();

	PteNumber = ROUND_UP_4K(Size) >> PAGE_SHIFT;
	PagesNumber = PteNumber;

	if (bAddGuardPage) { PteNumber++; }
	if (BusyType == DebuggerType)
	{
		// Debugger pages are only allocated from the extra 64 MiB available on devkits and are mapped in the
		// devkit system region

		if (!IsMappable(PteNumber, false, true)) { goto Fail; }
		LowestAcceptablePfn = DEBUGKIT_FIRST_UPPER_HALF_PAGE;
		HighestAcceptablePfn = m_HighestPage;
		MemoryType = DevkitRegion;
	}
	else { if (!IsMappable(PteNumber, true, false)) { goto Fail; } }

	if ((BusyType != DebuggerType) && (RemoveFree(PteNumber, &pfn, 0, LowestAcceptablePfn, HighestAcceptablePfn))) // MapViewOfFileEx path
	{
		MappingRoutine = &VMManager::MapBlockWithMapViewOfFileEx;
		addr = MapMemoryBlock(MappingRoutine, MemoryType, PteNumber, pfn);

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

		bVAlloc = true;
		MappingRoutine = &VMManager::MapBlockWithVirtualAlloc;
		addr = MapMemoryBlock(MappingRoutine, MemoryType, PteNumber, 0);

		if (!addr) { goto Fail; }
	}

	// check if we have to construct the PT's for this allocation
	if (!AllocatePT(PteNumber << PAGE_SHIFT, addr))
	{
		// If we reach here it means we had enough memory for the allocation but not for PT's mapping it, so this
		// allocation must fail instead.

		if (bVAlloc)
		{
			VirtualFree((void*)addr, 0, MEM_RELEASE);
		}
		else
		{
			// Recalculate the granularity aligned addr
			UnmapViewOfFile((void*)ROUND_DOWN(addr, m_AllocationGranularity));
			InsertFree(pfn, pfn + PteNumber - 1);
		}
		goto Fail;
	}

	// Finally, write the pte's and the pfn's
	PointerPte = GetPteAddress(addr);
	if (bAddGuardPage)
	{
		// Also increment by one the number of pte's used for the guard page. Note that we can't simply call WritePte
		// with bZero set because that will decrease the number of pte's used

		PXBOX_PFN PTpfn = GetPfnOfPT(PointerPte);
		PTpfn->PTPageFrame.PtesUsed++;
		WRITE_ZERO_PTE(PointerPte);
		PointerPte++;
	}
	EndingPte = PointerPte + PagesNumber - 1;

	if (bVAlloc)
	{
		PFN TempPfn;
		// With VirtualAlloc we grab one page at a time to avoid fragmentation issues
		while (PointerPte <= EndingPte)
		{
			RemoveFree(1, &TempPfn, 0, LowestAcceptablePfn, HighestAcceptablePfn);
			WritePfn(TempPfn, TempPfn, PointerPte, BusyType);
			WritePte(PointerPte, PointerPte, TempPte, TempPfn);

			PointerPte++;
		}
	}
	else
	{
		if (bAddGuardPage) { InsertFree(pfn, pfn); pfn++; } // Free the commited guard page
		EndingPfn = pfn + PagesNumber - 1;
		WritePte(PointerPte, EndingPte, TempPte, pfn);
		WritePfn(pfn, EndingPfn, PointerPte, BusyType);
	}
	EndingPte->Hardware.GuardOrEnd = 1;

	UpdateMemoryPermissions(bAddGuardPage ? addr + PAGE_SIZE : addr, PagesNumber << PAGE_SHIFT, Perms);

	ConstructVMA(addr, PteNumber << PAGE_SHIFT, MemoryType, AllocatedVma, bVAlloc);

	Unlock();
	RETURN(addr);

	Fail:
	Unlock();
	RETURN(NULL);
}

VAddr VMManager::AllocateContiguous(size_t Size, PAddr LowestAddress, PAddr HighestAddress, ULONG Alignment, DWORD Perms)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Size)
		LOG_FUNC_ARG(LowestAddress)
		LOG_FUNC_ARG(HighestAddress)
		LOG_FUNC_ARG(Alignment)
		LOG_FUNC_ARG(Perms)
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
	if (!AllocatePT(PteNumber << PAGE_SHIFT, addr))
	{
		InsertFree(pfn, EndingPfn);
		goto Fail;
	}

	// Finally, write the pte's and the pfn's
	PointerPte = GetPteAddress(addr);
	EndingPte = PointerPte + PteNumber - 1;

	WritePte(PointerPte, EndingPte, TempPte, pfn);
	WritePfn(pfn, EndingPfn, PointerPte, ContiguousType);
	EndingPte->Hardware.GuardOrEnd = 1;

	ConstructVMA(addr, PteNumber << PAGE_SHIFT, ContiguousRegion, AllocatedVma, false);
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
		LOG_FUNC_ARG(Paddr)
		LOG_FUNC_ARG(Size)
		LOG_FUNC_ARG(Perms)
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

		RETURN(Paddr);
	}

	// The requested address is not a known device address so we have to create a mapping for it. Even though this won't
	// allocate any physical memory for it, we still need to reserve the memory with VirtualAlloc. If we don't, then
	// we are likely to collide with other system allocations in the region at some point, which will overwrite and corrupt
	// the affected allocation. We only reserve the memory so that the access will trigger an access violation and it will
	// be handled by EmuException. However, RegisterBAR doesn't register any known device in the system region and so
	// EmuX86_Read/Write are going to fail with the error "Unknown address" and resume execution as it is...

	PteNumber = PAGES_SPANNED(Paddr, Size);

	Lock();

	MappingRoutine = &VMManager::ReserveBlockWithVirtualAlloc;
	addr = MapMemoryBlock(MappingRoutine, SystemRegion, PteNumber, 0);

	if (!addr) { goto Fail; }

	// check if we have to construct the PT's for this allocation
	if (!AllocatePT(PteNumber << PAGE_SHIFT, addr)) {
		VirtualFree((void*)addr, 0, MEM_RELEASE);
		goto Fail;
	}

	// Finally, write the pte's
	PointerPte = GetPteAddress(addr);
	EndingPte = PointerPte + PteNumber - 1;
	pfn = Paddr >> PAGE_SHIFT;

	WritePte(PointerPte, EndingPte, TempPte, pfn);

	ConstructVMA(addr, PteNumber << PAGE_SHIFT, SystemRegion, ReservedVma, true);

	Unlock();
	RETURN(addr + BYTE_OFFSET(Paddr));

	Fail:
	Unlock();
	RETURN(NULL);
}

void VMManager::Deallocate(VAddr addr)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(addr)
	LOG_FUNC_END;

	PMMPTE StartingPte;
	PMMPTE EndingPte;
	PFN pfn;
	PFN EndingPfn;
	PFN_COUNT PteNumber;
	VMAIter it;

	assert(CHECK_ALIGNMENT(addr, PAGE_SIZE)); // all starting addresses in the user region are page aligned
	assert(IS_USER_ADDRESS(addr));

	Lock();

	it = CheckExistenceVMA(addr, UserRegion);

	if (it == m_MemoryRegionArray[UserRegion].RegionMap.end() || it->second.type == FreeVma)
	{
		Unlock();
		return;
	}

	StartingPte = GetPteAddress(addr);
	EndingPte = StartingPte + (it->second.size >> PAGE_SHIFT) - 1;

	PteNumber = EndingPte - StartingPte + 1;

	if (it->second.bFragmented)
	{
		PFN TempPfn;
		// With VirtualAlloc we free one page at a time since the allocated pfn's are not contiguous
		while (StartingPte <= EndingPte)
		{
			TempPfn = StartingPte->Hardware.PFN;
			InsertFree(TempPfn, TempPfn);
			WritePfn(TempPfn, TempPfn, StartingPte, ImageType, true);

			StartingPte++;
		}
	}
	else
	{
		pfn = StartingPte->Hardware.PFN;
		EndingPfn = pfn + (EndingPte - StartingPte);
		InsertFree(pfn, EndingPfn);
		WritePfn(pfn, EndingPfn, StartingPte, ImageType, true);
	}

	WritePte(StartingPte, EndingPte, *StartingPte, 0, true);
	DestructVMA(it->first, UserRegion, it->second.size); // does Deallocate ever attempt to decommit only a part of the original allocation?
	DeallocatePT(PteNumber << PAGE_SHIFT, addr);

	Unlock();
}

void VMManager::DeallocateContiguous(VAddr addr)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(addr)
	LOG_FUNC_END;

	PMMPTE StartingPte;
	PMMPTE EndingPte;
	PFN pfn;
	PFN EndingPfn;
	VMAIter it;

	assert(CHECK_ALIGNMENT(addr, PAGE_SIZE)); // all starting addresses in the contiguous region are page aligned
	assert(IS_PHYSICAL_ADDRESS(addr));

	Lock();

	it = CheckExistenceVMA(addr, ContiguousRegion);

	if (it == m_MemoryRegionArray[ContiguousRegion].RegionMap.end() || it->second.type == FreeVma)
	{
		Unlock();
		return;
	}

	StartingPte = GetPteAddress(addr);
	EndingPte = StartingPte + (it->second.size >> PAGE_SHIFT) - 1;

	pfn = StartingPte->Hardware.PFN;
	EndingPfn = pfn + (EndingPte - StartingPte);

	InsertFree(pfn, EndingPfn);
	WritePfn(pfn, EndingPfn, StartingPte, ContiguousType, true);
	WritePte(StartingPte, EndingPte, *StartingPte, 0, true);
	DestructVMA(it->first, ContiguousRegion, it->second.size);
	DeallocatePT((EndingPte - StartingPte + 1) << PAGE_SHIFT, addr);

	Unlock();
}

PFN_COUNT VMManager::DeallocateSystemMemory(PageType BusyType, VAddr addr, size_t Size)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(addr)
		LOG_FUNC_ARG(Size)
	LOG_FUNC_END;

	PMMPTE StartingPte;
	PMMPTE EndingPte;
	PFN pfn;
	PFN EndingPfn;
	PFN_COUNT PteNumber;
	VMAIter it;
	MemoryRegionType MemoryType = SystemRegion;
	bool bGuardPageAdded = false;

	assert(CHECK_ALIGNMENT(addr, PAGE_SIZE)); // all starting addresses in the system region are page aligned

	Lock();

	// We cannot do the size check because the functions that call this can provide 0 as size, meaning 
	// that we have to calculate it ourselves
	if (BusyType == DebuggerType)
	{
		assert(IS_DEVKIT_ADDRESS(addr));
		MemoryType = DevkitRegion;
	}
	else { assert(IS_SYSTEM_ADDRESS(addr)); }

	it = GetVMAIterator(addr, MemoryType);

	if (it == m_MemoryRegionArray[MemoryType].RegionMap.end() || it->second.type == FreeVma)
	{
		Unlock();
		RETURN(NULL);
	}

	// Calculate the size of the original allocation
	if (Size) { Size = ROUND_UP_4K(Size); }
	else { Size = it->second.size; }

	StartingPte = GetPteAddress(addr);
	if (StartingPte->Hardware.Valid == 0) {
		WritePte(StartingPte, StartingPte, *StartingPte, 0, true); // this is the guard page of the stack
		StartingPte++;
		Size -= PAGE_SIZE;
		bGuardPageAdded = true;
	}

	EndingPte = StartingPte + (Size >> PAGE_SHIFT) - 1;
	PteNumber = EndingPte - StartingPte + 1;

	if (it->second.bFragmented)
	{
		PFN TempPfn;
		// With VirtualAlloc we free one page at a time since the allocated pfn's are not contiguous
		while (StartingPte <= EndingPte)
		{
			TempPfn = StartingPte->Hardware.PFN;
			InsertFree(TempPfn, TempPfn);
			WritePfn(TempPfn, TempPfn, StartingPte, BusyType, true);

			StartingPte++;
		}
	}
	else
	{
		pfn = StartingPte->Hardware.PFN;
		EndingPfn = pfn + (EndingPte - StartingPte);
		InsertFree(pfn, EndingPfn);
		WritePfn(pfn, EndingPfn, StartingPte, BusyType, true);
	}

	WritePte(StartingPte, EndingPte, *StartingPte, 0, true);
	DestructVMA(BusyType == DebuggerType ? addr : it->first, MemoryType, bGuardPageAdded ? Size + PAGE_SIZE : Size);
	DeallocatePT(bGuardPageAdded ? Size + PAGE_SIZE : Size, addr);

	Unlock();
	RETURN(bGuardPageAdded ? ++PteNumber : PteNumber);
}

void VMManager::UnmapDeviceMemory(VAddr addr, size_t Size)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(addr)
		LOG_FUNC_ARG(Size)
	LOG_FUNC_END;

	if (IS_SYSTEM_ADDRESS(addr))
	{
		// The allocation is inside the system region, so it must have been mapped by us. Unmap it

		PMMPTE StartingPte;
		PMMPTE EndingPte;
		PFN_COUNT PteNumber;
		VMAIter it;

		// The starting address of a device can be unaligned since MapDeviceMemory returns an offset from the aligned
		// mapped address, so we won't assert the address here

		Lock();

		it = CheckExistenceVMA(addr, SystemRegion, PAGES_SPANNED(addr, Size) << PAGE_SHIFT);

		if (it == m_MemoryRegionArray[SystemRegion].RegionMap.end() || it->second.type == FreeVma)
		{
			Unlock();
			return;
		}

		StartingPte = GetPteAddress(addr);
		EndingPte = StartingPte + (it->second.size >> PAGE_SHIFT) - 1;
		PteNumber = EndingPte - StartingPte + 1;

		WritePte(StartingPte, EndingPte, *StartingPte, 0, true);
		DestructVMA(it->first, SystemRegion, it->second.size);
		DeallocatePT(PteNumber << PAGE_SHIFT, addr);

		Unlock();
	}

	// Don't free hardware devices (flash, NV2A, etc) -> no operation
}

void VMManager::Protect(VAddr addr, size_t Size, DWORD NewPerms)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(addr)
		LOG_FUNC_ARG(Size)
		LOG_FUNC_ARG(NewPerms)
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
	MemoryRegionType MemoryType = IS_PHYSICAL_ADDRESS(addr) ? ContiguousRegion : SystemRegion;
	VMAIter it = GetVMAIterator(addr, MemoryType);
	assert(it != m_MemoryRegionArray[MemoryType].RegionMap.end() && addr >= it->first &&
		addr < it->first + it->second.size && it->second.type != FreeVma);
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

size_t VMManager::QuerySize(VAddr addr, bool bCxbxCaller)
{
	LOG_FUNC_ONE_ARG(addr);

	PMMPTE PointerPte;
	PFN_COUNT PagesNumber;
	size_t Size = 0;

	Lock();

	if (bCxbxCaller)
	{
		// This is designed to handle Cxbx callers which can provide an offset instead of the beginning of the allocation

		// Figure out the correct memory region to query
		MemoryRegionType Type;
		if (IS_PHYSICAL_ADDRESS(addr)) { Type = ContiguousRegion; }
		else if (IS_SYSTEM_ADDRESS(addr)) { Type = SystemRegion; }
		else if (IS_DEVKIT_ADDRESS(addr)) { Type = DevkitRegion; }
		else if(IS_USER_ADDRESS(addr)) { Type = UserRegion; }
		else
		{
			DbgPrintf(LOG_PREFIX, "QuerySize: Unknown memory region queried.\n");
			Unlock();
			RETURN(Size);
		}

		VMAIter it = GetVMAIterator(addr, Type);

		if (it != m_MemoryRegionArray[Type].RegionMap.end() && it->second.type != FreeVma)
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

void VMManager::LockBufferOrSinglePage(PAddr paddr, VAddr addr, size_t Size, bool bUnLock)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(paddr)
		LOG_FUNC_ARG(addr)
		LOG_FUNC_ARG(Size)
		LOG_FUNC_ARG(bUnLock)
	LOG_FUNC_END;

	PMMPTE PointerPte;
	PMMPTE EndingPte;
	PFN pfn;
	PXBOX_PFN PfnEntry;
	ULONG LockUnit;

	Lock();

	if (addr) // lock the pages of a buffer
	{
		if (!IS_PHYSICAL_ADDRESS(addr) && (GetPdeAddress(addr)->Hardware.LargePage == 0))
		{
			LockUnit = bUnLock ? -LOCK_COUNT_UNIT : LOCK_COUNT_UNIT;

			PointerPte = GetPteAddress(addr);
			EndingPte = GetPteAddress(addr + Size - 1);

			while (PointerPte <= EndingPte)
			{
				assert(PointerPte->Hardware.Valid != 0);

				pfn = PointerPte->Hardware.PFN;

				if (pfn <= m_HighestPage)
				{
					if (m_MmLayoutRetail || m_MmLayoutDebug) {
						PfnEntry = XBOX_PFN_ELEMENT(pfn);
					}
					else { PfnEntry = CHIHIRO_PFN_ELEMENT(pfn); }

					assert(PfnEntry->Busy.Busy != 0);

					PfnEntry->Busy.LockCount += LockUnit;
				}
				PointerPte++;
			}
		}
	}
	else // lock a single page
	{
		pfn = paddr >> PAGE_SHIFT;
		if (m_MmLayoutRetail || m_MmLayoutDebug) {
			PfnEntry = XBOX_PFN_ELEMENT(pfn);
		}
		else { PfnEntry = CHIHIRO_PFN_ELEMENT(pfn); }

		if (PfnEntry->Busy.BusyType != ContiguousType && pfn <= m_HighestPage)
		{
			LockUnit = bUnLock ? -LOCK_COUNT_UNIT : LOCK_COUNT_UNIT;

			assert(PfnEntry->Busy.Busy != 0);

			PfnEntry->Busy.LockCount += LockUnit;
		}
	}

	Unlock();
}

xboxkrnl::NTSTATUS VMManager::XbAllocateVirtualMemory(VAddr* addr, ULONG ZeroBits, size_t* Size, DWORD AllocationType, DWORD Protect)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(*addr)
		LOG_FUNC_ARG(ZeroBits)
		LOG_FUNC_ARG(*Size)
		LOG_FUNC_ARG(AllocationType)
		LOG_FUNC_ARG(Protect)
	LOG_FUNC_END;

	MMPTE TempPte;
	PMMPTE PointerPte;
	PMMPTE EndingPte;
	PMMPTE StartingPte;
	PFN_COUNT PteNumber = 0;
	PFN TempPfn;
	PageType BusyType;
	xboxkrnl::NTSTATUS status;
	VAddr CapturedBase = *addr;
	size_t CapturedSize = *Size;
	VAddr MaxAllowedAddress;
	MappingFn MappingRoutine;
	VAddr AlignedCapturedBase;
	size_t AlignedCapturedSize;
	VMAIter it;
	bool bDestructVmaOnFailure = false;
	bool bUpdatePteProtections = false;
	bool bOverflow;

	// Invalid base address
	if (CapturedBase > HIGHEST_VMA_ADDRESS) { RETURN(STATUS_INVALID_PARAMETER); }

	// Invalid region size
	if (((HIGHEST_VMA_ADDRESS + 1) - CapturedBase) < CapturedSize) { RETURN(STATUS_INVALID_PARAMETER); }

	// Size cannot be zero
	if (CapturedSize == 0) { RETURN(STATUS_INVALID_PARAMETER); }

	// Limit number of zero bits upto 21
	if (ZeroBits > MAXIMUM_ZERO_BITS) { RETURN(STATUS_INVALID_PARAMETER); }

	// Check for unknown MEM flags
	if(AllocationType & ~(XBOX_MEM_COMMIT | XBOX_MEM_RESERVE | XBOX_MEM_TOP_DOWN | XBOX_MEM_RESET
		| XBOX_MEM_NOZERO)) { RETURN(STATUS_INVALID_PARAMETER); }

	// No other flags allowed in combination with MEM_RESET
	if ((AllocationType & XBOX_MEM_RESET) && (AllocationType != XBOX_MEM_RESET)) { RETURN(STATUS_INVALID_PARAMETER); }

	// At least MEM_RESET, MEM_COMMIT or MEM_RESERVE must be set
	if ((AllocationType & (XBOX_MEM_COMMIT | XBOX_MEM_RESERVE | XBOX_MEM_RESET)) == 0) { RETURN(STATUS_INVALID_PARAMETER); }

	if (!ConvertXboxToPteProtection(Protect, &TempPte)) { RETURN(STATUS_INVALID_PAGE_PROTECTION); }

	DbgPrintf(LOG_PREFIX, "%s requested range : 0x%.8X - 0x%.8X\n", __func__, CapturedBase, CapturedBase + CapturedSize);

	Lock();

	if (AllocationType & XBOX_MEM_RESERVE || CapturedBase == 0)
	{
		if (CapturedBase == 0)
		{
			// We are free to decide where to put this block

			AlignedCapturedSize = ROUND_UP_4K(CapturedSize);

			if (ZeroBits != 0)
			{
				MaxAllowedAddress = MAX_VIRTUAL_ADDRESS >> ZeroBits;
				if (MaxAllowedAddress > HIGHEST_USER_ADDRESS) { status = STATUS_INVALID_PARAMETER; goto Exit; }
			}
			else
			{
				MaxAllowedAddress = HIGHEST_USER_ADDRESS;
			}

			// With XBOX_MEM_TOP_DOWN we will always set the LastFree iterator to the vma containing MaxAllowedAddress in order to map the
			// block at the highest possible address. This can become quite slow if a title repeatedly allocates virtual memory with this flag.
			// See here: https://randomascii.wordpress.com/2011/08/05/making-virtualalloc-arbitrarily-slower/

			if (AllocationType & XBOX_MEM_TOP_DOWN) { m_MemoryRegionArray[UserRegion].LastFree = GetVMAIterator(MaxAllowedAddress, UserRegion); }

			// On the Xbox, blocks reserved by NtAllocateVirtualMemory are 64K aligned and the size is rounded up on a 4K boundary.
			// This can lead to unaligned blocks if the host granularity is not 64K. Fortunately, Windows uses that

			MappingRoutine = &VMManager::ReserveBlockWithVirtualAlloc;
			AlignedCapturedBase = MapMemoryBlock(MappingRoutine, UserRegion, AlignedCapturedSize >> PAGE_SHIFT, 0, MaxAllowedAddress);

			if (!AlignedCapturedBase) { status = STATUS_NO_MEMORY; goto Exit; }
		}
		else
		{
			AlignedCapturedBase = ROUND_DOWN(CapturedBase, X64KB);
			AlignedCapturedSize = ROUND_UP_4K(CapturedSize);
			it = CheckConflictingVMA(AlignedCapturedBase, AlignedCapturedSize, &bOverflow);

			if (it != m_MemoryRegionArray[UserRegion].RegionMap.end() || bOverflow)
			{
				// Reserved vma or we are overflowing a free vma, report an error

				status = STATUS_CONFLICTING_ADDRESSES;
				goto Exit;
			}

			// Hack: check if the title is attempting to reserve in the region between the reserved xbe image memory up to XBE_MAX_VA.
			// Once the memory placeholder is gone, this can be removed.
			// Note: this will not work if the title attempts to reserve inside the placeholder with a size that makes the
			// allocation exceed the end of the placeholder at XBE_MAX_VA (e.g.: Fable does this!) and it doesn't accept
			// any other address but the requested one (e.g.: Project Zero 1 does this!). No title is known to do this, but it's
			// teoretically possible...

			if (AlignedCapturedBase < XBE_MAX_VA && AlignedCapturedBase >= ROUND_UP_4K(CxbxKrnl_Xbe->m_Header.dwSizeofImage) + XBE_IMAGE_BASE
				&& AlignedCapturedBase + AlignedCapturedSize - 1 < XBE_MAX_VA)
			{
				// Don't do anything, ConstructVMA will be called below to track the allocation inside the placeholder
			}
			else {
				if ((VAddr)VirtualAlloc((void*)AlignedCapturedBase, AlignedCapturedSize, MEM_RESERVE,
					ConvertXboxToWinProtection(PatchXboxPermissions(Protect)) & ~(PAGE_WRITECOMBINE | PAGE_NOCACHE)) != AlignedCapturedBase)
				{
					// An host allocation is already mapped there, report an error

					status = STATUS_CONFLICTING_ADDRESSES;
					goto Exit;
				}
			}
		}

		m_VirtualMemoryBytesReserved += AlignedCapturedSize;
		ConstructVMA(AlignedCapturedBase, AlignedCapturedSize, UserRegion, ReservedVma, true, Protect);

		if ((AllocationType & XBOX_MEM_COMMIT) == 0)
		{
			// XBOX_MEM_COMMIT was not specified, so we are done with the allocation

			DbgPrintf(LOG_PREFIX, "%s resulting range : 0x%.8X - 0x%.8X\n", __func__, AlignedCapturedBase, AlignedCapturedBase + AlignedCapturedSize);

			*addr = AlignedCapturedBase;
			*Size = AlignedCapturedSize;

			status = STATUS_SUCCESS;
			goto Exit;
		}
		bDestructVmaOnFailure = true;
		CapturedBase = AlignedCapturedBase;
		CapturedSize = AlignedCapturedSize;
	}


	// If we reach here then XBOX_MEM_COMMIT was specified, so we will also have to allocate physical memory for the allocation and
	// write the pte/pfn

	AlignedCapturedBase = ROUND_DOWN_4K(CapturedBase);
	AlignedCapturedSize = (PAGES_SPANNED(CapturedBase, CapturedSize)) << PAGE_SHIFT;

	it = CheckConflictingVMA(AlignedCapturedBase, AlignedCapturedSize, &bOverflow);

	if (it == m_MemoryRegionArray[UserRegion].RegionMap.end() || bOverflow)
	{
		// The specified region is not completely inside a reserved vma or it's free

		status = STATUS_CONFLICTING_ADDRESSES;
		goto Exit;
	}

	if (AllocationType == XBOX_MEM_RESET)
	{
		// XBOX_MEM_RESET is a no-operation since it implies having page file support, which the Xbox doesn't have

		*addr = AlignedCapturedBase;
		*Size = AlignedCapturedSize;

		Unlock();
		RETURN(STATUS_SUCCESS);
	}

	// Figure out the number of physical pages we need to allocate. Note that NtAllocateVirtualMemory can do overlapped allocations so we
	// cannot just page-shift the size to know this number, since a part of those pages could already be allocated

	PointerPte = GetPteAddress(AlignedCapturedBase);
	EndingPte = GetPteAddress(AlignedCapturedBase + AlignedCapturedSize - 1);
	StartingPte = PointerPte;

	while (PointerPte <= EndingPte)
	{
		if (PointerPte->Default == 0) { PteNumber++; }
		else if ((PointerPte->Default & PTE_VALID_PROTECTION_MASK) != TempPte.Default) { bUpdatePteProtections = true; }

		PointerPte++;
	}

	if (!IsMappable(PteNumber, true, m_MmLayoutDebug && m_bAllowNonDebuggerOnTop64MiB ? true : false))
	{
		status = STATUS_NO_MEMORY;
		goto Exit;
	}

	// Check if we have to construct the PT's for this allocation

	if (!AllocatePT(AlignedCapturedSize, AlignedCapturedBase))
	{
		status = STATUS_NO_MEMORY;
		goto Exit;
	}

	// With VirtualAlloc we grab one page at a time to avoid fragmentation issues

	BusyType = (Protect & (XBOX_PAGE_EXECUTE | XBOX_PAGE_EXECUTE_READ | XBOX_PAGE_EXECUTE_READWRITE
		| XBOX_PAGE_EXECUTE_WRITECOPY)) ? ImageType : VirtualMemoryType;
	PointerPte = StartingPte;
	while (PointerPte <= EndingPte)
	{
		if (PointerPte->Default == 0)
		{
			RemoveFree(1, &TempPfn, 0, 0, m_MmLayoutDebug && !m_bAllowNonDebuggerOnTop64MiB ? XBOX_HIGHEST_PHYSICAL_PAGE : m_HighestPage);
			WritePfn(TempPfn, TempPfn, PointerPte, BusyType);
			WritePte(PointerPte, PointerPte, TempPte, TempPfn);
		}

		PointerPte++;
	}

	// Actually commit the requested range but don't if it's inside the placeholder or we are committing an xbe section so that
	// XeLoadSection works as expected

	if (AlignedCapturedBase >= XBE_MAX_VA)
	{
		if (!VirtualAlloc((void*)AlignedCapturedBase, AlignedCapturedSize, MEM_COMMIT,
			(ConvertXboxToWinProtection(PatchXboxPermissions(Protect))) & ~(PAGE_WRITECOMBINE | PAGE_NOCACHE)))
		{
			DbgPrintf(LOG_PREFIX, "%s: VirtualAlloc failed to commit the memory! The error was %d\n", __func__, GetLastError());
		}
	}

	// Because VirtualAlloc always zeros the memory for us, XBOX_MEM_NOZERO is still unsupported

	if (AllocationType & XBOX_MEM_NOZERO) { DbgPrintf(LOG_PREFIX, "XBOX_MEM_NOZERO flag is not supported!\n"); }

	// If some pte's were detected to have different permissions in the above check, we need to update those as well

	if (bUpdatePteProtections)
	{
		VAddr TempAddr = AlignedCapturedBase;
		size_t TempSize = AlignedCapturedSize;
		DWORD TempProtect = Protect;
		XbVirtualProtect(&TempAddr, &TempSize, &TempProtect);
	}

	DbgPrintf(LOG_PREFIX, "%s resulting range : 0x%.8X - 0x%.8X\n", __func__, AlignedCapturedBase, AlignedCapturedBase + AlignedCapturedSize);

	*addr = AlignedCapturedBase;
	*Size = AlignedCapturedSize;
	Unlock();
	RETURN(STATUS_SUCCESS);

	Exit:
	if (bDestructVmaOnFailure)
	{
		m_VirtualMemoryBytesReserved -= AlignedCapturedSize;
		DestructVMA(AlignedCapturedBase, UserRegion, AlignedCapturedSize);
	}
	Unlock();
	RETURN(status);
}

xboxkrnl::NTSTATUS VMManager::XbFreeVirtualMemory(VAddr* addr, size_t* Size, DWORD FreeType)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(*addr)
		LOG_FUNC_ARG(*Size)
		LOG_FUNC_ARG(FreeType)
	LOG_FUNC_END;

	VAddr CapturedBase = *addr;
	size_t CapturedSize = *Size;
	VAddr AlignedCapturedBase;
	size_t AlignedCapturedSize;
	xboxkrnl::NTSTATUS status;
	PMMPTE PointerPte;
	PMMPTE EndingPte;
	PMMPTE StartingPte;
	PFN TempPfn;
	PageType BusyType;
	VMAIter it;
	bool bOverflow;


	// Only MEM_DECOMMIT and MEM_RELEASE are valid
	if ((FreeType & ~(XBOX_MEM_DECOMMIT | XBOX_MEM_RELEASE)) != 0) { RETURN(STATUS_INVALID_PARAMETER); }

	// MEM_DECOMMIT and MEM_RELEASE must not be specified together
	if (((FreeType & (XBOX_MEM_DECOMMIT | XBOX_MEM_RELEASE)) == 0) ||
		((FreeType & (XBOX_MEM_DECOMMIT | XBOX_MEM_RELEASE)) == (XBOX_MEM_DECOMMIT | XBOX_MEM_RELEASE))) {
		RETURN(STATUS_INVALID_PARAMETER);
	}

	// Invalid base address
	if (CapturedBase > HIGHEST_USER_ADDRESS) { RETURN(STATUS_INVALID_PARAMETER); }

	// Invalid region size
	if ((HIGHEST_USER_ADDRESS - CapturedBase) < CapturedSize) { RETURN(STATUS_INVALID_PARAMETER); }

	AlignedCapturedBase = ROUND_DOWN_4K(CapturedBase);
	AlignedCapturedSize = (PAGES_SPANNED(CapturedBase, CapturedSize)) << PAGE_SHIFT;

	Lock();

	it = CheckConflictingVMA(AlignedCapturedBase, AlignedCapturedSize, &bOverflow);

	if (it == m_MemoryRegionArray[UserRegion].RegionMap.end())
	{
		// Free vma, report an error

		status = STATUS_MEMORY_NOT_ALLOCATED;
		goto Exit;
	}

	if (bOverflow)
	{
		// The provided ending adddress is beyond the end of the vma, report an error

		status = STATUS_UNABLE_TO_FREE_VM;
		goto Exit;
	}

	if (FreeType & XBOX_MEM_RELEASE)
	{
		if (AlignedCapturedSize == 0)
		{
			// Size is 0, so just delete the entire vma

			if (it->first != AlignedCapturedBase)
			{
				// The provided base address is not the starting address of the vma, report an error

				status = STATUS_FREE_VM_NOT_AT_BASE;
				goto Exit;
			}

			AlignedCapturedSize = it->second.size;
			DestructVMA(it->first, UserRegion, it->second.size);
		}
		else
		{
			if (AlignedCapturedBase == it->first)
			{
				if (AlignedCapturedBase + AlignedCapturedSize == it->first + it->second.size)
				{
					// The range specified covers the entire vma, just delete it

					DestructVMA(it->first, UserRegion, it->second.size);
				}
				else
				{
					// Split the vma at the edge specified and keep the remaining memory up until the end of the vma in the reserved state

					DestructVMA(it->first, UserRegion, AlignedCapturedSize);
				}
			}
			else { DestructVMA(AlignedCapturedBase, UserRegion, AlignedCapturedSize); }
		}
		m_VirtualMemoryBytesReserved -= AlignedCapturedSize;
	}


	// The decommit path will always be executed, even if only XBOX_MEM_RELEASE was specified, because releasing the memory implies
	// deallocating all the physical pages currently commited in the vma

	if ((AlignedCapturedSize == 0) && (AlignedCapturedBase == it->first))
	{
		// This can only happen if XBOX_MEM_RELEASE was not specified, since that path would set AlignedCapturedSize to the size of the vma
		// if 0. This means we have to decommit all the physical pages allocated in the vma but still keep the memory reserved

		AlignedCapturedSize = it->second.size;
	}

	PointerPte = GetPteAddress(AlignedCapturedBase);
	EndingPte = GetPteAddress(AlignedCapturedBase + AlignedCapturedSize - 1);
	StartingPte = PointerPte;

	// With VirtualAlloc we free one page at a time since the allocated pfn's are not contiguous

	while (PointerPte <= EndingPte)
	{
		if (PointerPte->Default != 0)
		{
			TempPfn = PointerPte->Hardware.PFN;
			InsertFree(TempPfn, TempPfn);
			if (m_MmLayoutRetail || m_MmLayoutDebug) {
				BusyType = (PageType)(XBOX_PFN_ELEMENT(TempPfn)->Busy.BusyType);
			}
			else { BusyType = (PageType)(CHIHIRO_PFN_ELEMENT(TempPfn)->Busy.BusyType); }
			WritePfn(TempPfn, TempPfn, PointerPte, BusyType, true);
		}

		PointerPte++;
	}

	WritePte(StartingPte, EndingPte, *StartingPte, 0, true);
	DeallocatePT((EndingPte - StartingPte + 1) << PAGE_SHIFT, AlignedCapturedBase);

	if (FreeType & ~XBOX_MEM_RELEASE)
	{
		// With XBOX_MEM_DECOMMIT DestructVMA is not called and so we have to call VirtualFree ourselves

		if (AlignedCapturedBase >= XBE_MAX_VA)
		{
			if (!VirtualFree((void*)AlignedCapturedBase, AlignedCapturedSize, MEM_DECOMMIT))
			{
				DbgPrintf(LOG_PREFIX, "%s: VirtualFree failed to decommit the memory! The error was %d\n", __func__, GetLastError());
			}
		}
	}

	*addr = AlignedCapturedBase;
	*Size = AlignedCapturedSize;
	status = STATUS_SUCCESS;

	Exit:
	Unlock();
	RETURN(status);
}

xboxkrnl::NTSTATUS VMManager::XbVirtualProtect(VAddr* addr, size_t* Size, DWORD* Protect)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(*addr)
		LOG_FUNC_ARG(*Size)
		LOG_FUNC_ARG(*Protect)
	LOG_FUNC_END;

	DWORD NewPerms = *Protect;
	VAddr CapturedBase = *addr;
	size_t CapturedSize = *Size;
	VAddr AlignedCapturedBase;
	size_t AlignedCapturedSize;
	xboxkrnl::NTSTATUS status;
	PMMPTE PointerPte;
	PMMPTE EndingPte;
	PMMPTE StartingPte;
	PMMPTE PointerPde;
	MMPTE TempPte;
	MMPTE NewPermsPte;
	MMPTE OldPermsPte;
	VMAIter it;
	bool bOverflow;


	// Invalid base address
	if (CapturedBase > HIGHEST_USER_ADDRESS) { RETURN(STATUS_INVALID_PARAMETER); }

	// Invalid region size
	if ((HIGHEST_USER_ADDRESS - CapturedBase) < CapturedSize) { RETURN(STATUS_INVALID_PARAMETER); }

	// Size cannot be zero
	if (CapturedSize == 0) { RETURN(STATUS_INVALID_PARAMETER); }

	if (!ConvertXboxToPteProtection(NewPerms, &NewPermsPte)) { RETURN(STATUS_INVALID_PAGE_PROTECTION); }

	AlignedCapturedBase = ROUND_DOWN_4K(CapturedBase);
	AlignedCapturedSize = (PAGES_SPANNED(CapturedBase, CapturedSize)) << PAGE_SHIFT;

	Lock();

	it = CheckConflictingVMA(AlignedCapturedBase, AlignedCapturedSize, &bOverflow);

	if (it == m_MemoryRegionArray[UserRegion].RegionMap.end() || bOverflow)
	{
		// Free vma or the requested ending address is beyond the vma, report an error

		status = STATUS_CONFLICTING_ADDRESSES;
		goto Exit;
	}

	PointerPte = GetPteAddress(AlignedCapturedBase);
	EndingPte = GetPteAddress(AlignedCapturedBase + AlignedCapturedSize -1);
	StartingPte = PointerPte;

	// Ensure that we are not trying to change the permissions of non-committed pages

	while (PointerPte <= EndingPte)
	{
		if ((PointerPte == StartingPte) || (IsPteOnPdeBoundary(PointerPte)))
		{
			PointerPde = GetPteAddress(PointerPte);

			if (PointerPde->Hardware.Valid == 0)
			{
				// Pde is not committed, report an error

				status = STATUS_NOT_COMMITTED;
				goto Exit;
			}
		}

		if (PointerPte->Default == 0)
		{
			// Pte is not committed, report an error

			status = STATUS_NOT_COMMITTED;
			goto Exit;
		}

		PointerPte++;
	}

	PointerPte = StartingPte;
	OldPermsPte = NewPermsPte;

	while (PointerPte <= EndingPte)
	{
		TempPte = *PointerPte;

		if ((TempPte.Default & PTE_VALID_PROTECTION_MASK) != NewPermsPte.Default)
		{
			// The following trick ensures we save only the first page with non-matching pte permissions we encounter

			if (OldPermsPte.Default == NewPermsPte.Default)
				OldPermsPte.Default = (TempPte.Default & PTE_VALID_PROTECTION_MASK);

			// This zeroes the existent bit protections and applies the new ones

			TempPte.Default = ((TempPte.Default & ~PTE_VALID_PROTECTION_MASK) | NewPermsPte.Default);
			WRITE_PTE(PointerPte, TempPte);
		}

		PointerPte++;
	}

	UpdateMemoryPermissions(AlignedCapturedBase, AlignedCapturedSize, NewPerms);

	*addr = AlignedCapturedBase;
	*Size = AlignedCapturedSize;
	*Protect = ConvertPteToXboxProtection(OldPermsPte.Default);
	status = STATUS_SUCCESS;

	Exit:
	Unlock();
	RETURN(status);
}

xboxkrnl::NTSTATUS VMManager::XbVirtualMemoryStatistics(VAddr addr, xboxkrnl::PMEMORY_BASIC_INFORMATION memory_statistics)
{
	VMAIter it;
	PMMPTE PointerPte;
	PMMPTE EndingPte;
	PMMPTE StartingPte;
	PMMPTE PointerPde;
	DWORD CurrentProtect;
	DWORD InitialProtect;
	DWORD CurrentState;
	DWORD PermissionsOfFirstPte;
	size_t RegionSize;


	if (addr > HIGHEST_VMA_ADDRESS)
	{
		// The memory in the user region is allocated in multiples of 64K, so this means that the last possible vma cannot start beyond
		// HIGHEST_VMA_ADDRESS. This is true for allocations done by both XbAllocateVirtualMemory and Allocate, which both will be counted.

		return STATUS_INVALID_PARAMETER;
	}

	// If it's not in the XBE, report actual host allocations
	// The game will see allocations it didn't make, but at least it has a chance to
	// not try to allocate memory our emulator already occupied.
	if (addr > XBE_IMAGE_BASE + ROUND_UP_4K(CxbxKrnl_Xbe->m_Header.dwSizeofImage))
	{
		MEMORY_BASIC_INFORMATION info;
		if (VirtualQuery((void*)addr, &info, sizeof(info)))
		{
			memory_statistics->AllocationBase = info.AllocationBase;
			memory_statistics->AllocationProtect = info.AllocationProtect;
			memory_statistics->BaseAddress = info.BaseAddress;
			memory_statistics->Protect = info.Protect;
			memory_statistics->RegionSize = info.RegionSize;
			memory_statistics->State = info.State;
			memory_statistics->Type = info.Type;
			return STATUS_SUCCESS;
		}
		else {
			return STATUS_INVALID_PARAMETER;
		}
	}

	Lock();

	// Locate the vma containing the supplied address

	it = GetVMAIterator(addr, UserRegion);

	// NOTE: we count the first 64K block below 0x10000 and the reserved area in the memory placeholder after the xbe image as free areas

	if (addr < LOWEST_USER_ADDRESS
		|| (addr >= XBE_IMAGE_BASE + ROUND_UP_4K(CxbxKrnl_Xbe->m_Header.dwSizeofImage) && addr < XBE_MAX_VA)
		|| (it != m_MemoryRegionArray[UserRegion].RegionMap.end() && it->second.type == FreeVma)) {

		if (addr < LOWEST_USER_ADDRESS)
		{
			RegionSize = LOWEST_USER_ADDRESS - ROUND_DOWN_4K(addr);
		}
		else { RegionSize = it->first + it->second.size - ROUND_DOWN_4K(addr); }

		memory_statistics->AllocationBase = NULL;
		memory_statistics->AllocationProtect = 0;
		memory_statistics->BaseAddress = (void*)ROUND_DOWN_4K(addr);
		memory_statistics->RegionSize = RegionSize;
		memory_statistics->State = XBOX_MEM_FREE;
		memory_statistics->Protect = XBOX_PAGE_NOACCESS;
		memory_statistics->Type = 0;

		Unlock();
		return STATUS_SUCCESS;
	}

	// The vma is in the reserved or allocated state

	PointerPte = GetPteAddress(addr);
	EndingPte = GetPteAddress(it->first + it->second.size - 1);
	PointerPde = GetPteAddress(PointerPte);
	StartingPte = PointerPte;
	CurrentProtect = 0;
	InitialProtect = it->second.permissions;

	if (PointerPde->Hardware.Valid != 0 && PointerPte->Default != 0) {
		CurrentState = XBOX_MEM_COMMIT;
		PermissionsOfFirstPte = (PointerPte->Default & PTE_VALID_PROTECTION_MASK);
		CurrentProtect = ConvertPteToXboxProtection(PointerPte->Default);
	}
	else { CurrentState = XBOX_MEM_RESERVE; }

	while (PointerPte <= EndingPte)
	{
		if ((PointerPte == StartingPte) || IsPteOnPdeBoundary(PointerPte))
		{
			PointerPde = GetPteAddress(PointerPte);

			if (PointerPde->Hardware.Valid == 0)
			{
				// Pde is invalid but we are looking for committed memory, so stop now
				if (CurrentState == XBOX_MEM_COMMIT) { break; }

				// Pde is invalid and we are looking for reserved memory, so skip this pde and keep searching if we are not at EndingPte yet
				PointerPte = (PMMPTE)GetVAddrMappedByPte(PointerPde + 1);
				continue;
			}
		}

		if (PointerPte->Default != 0)
		{
			// Pte is valid but we are looking for reserved memory, so stop now
			if (CurrentState == XBOX_MEM_RESERVE) { break; }

			// Pte is valid but its permissions are different, so stop now
			if ((PointerPte->Default & PTE_VALID_PROTECTION_MASK) != PermissionsOfFirstPte) { break; }
		}
		else
		{
			// Pte is invalid but we are looking for committed memory, so stop now
			if (CurrentState == XBOX_MEM_COMMIT) { break; }
		}

		PointerPte++;
	}

	// This can happen if we reach EndingPte in the loop above (and PointerPte will be EndingPte + 1) or if we are looking for reserved memory and
	// PointerPte/EndingPte are inside a single page: in this case, GetVAddrMappedByPte(PointerPde + 1) will make PointerPte much larger than
	// EndingPte, hence this check
	if (PointerPte > EndingPte) { PointerPte = EndingPte + 1; }

	RegionSize = GetVAddrMappedByPte(PointerPte) - ROUND_DOWN_4K(addr);

	memory_statistics->AllocationBase = (void*)it->first;
	memory_statistics->AllocationProtect = InitialProtect;
	memory_statistics->BaseAddress = (void*)ROUND_DOWN_4K(addr);
	memory_statistics->RegionSize = RegionSize;
	memory_statistics->State = CurrentState;
	memory_statistics->Protect = CurrentProtect;
	memory_statistics->Type = XBOX_MEM_PRIVATE;

	Unlock();
	return STATUS_SUCCESS;
}

VAddr VMManager::MapMemoryBlock(MappingFn MappingRoutine, MemoryRegionType Type, PFN_COUNT PteNumber, PFN pfn, VAddr HighestAddress)
{
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

	if (HighestAddress) // XbAllocateVirtualMemory specific
	{
		end_it = std::next(GetVMAIterator(HighestAddress, Type));
	}

	while (it != end_it)
	{
		if (it->second.type != FreeVma) // already allocated by the VMManager
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

		size_t vma_end;
		if (HighestAddress && (it->first + it->second.size > HighestAddress + 1)) { vma_end = HighestAddress + 1; }
		else { vma_end = it->first + it->second.size; }

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

		EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "Failed to map a memory block in the virtual region %d!", Type);
		return NULL;
	}

	it = std::prev(m_MemoryRegionArray[Type].LastFree);

	while (true)
	{
		if (it->second.type == FreeVma)
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

	EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "Failed to map a memory block in the virtual region %d!", Type);

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
	//MemoryRegionType Type;

	Lock();

	// ergo720: horrendous hack, this identity maps all allocations done by the VMManager to keep the LLE OHCI working
	// (see OHCI_ReadHCCA for more details). Once LLE CPU and MMU are implemented, this can be removed

	//if (IS_USER_ADDRESS(addr)) { Type = UserRegion; }
	//else if (IS_PHYSICAL_ADDRESS(addr)) { Type = ContiguousRegion; }
	//else if (IS_SYSTEM_ADDRESS(addr)) { Type = SystemRegion; }
	//else if (IS_DEVKIT_ADDRESS(addr)) { Type = DevkitRegion; }
	//else { Type = COUNTRegion; }

	if (true/*(addr >= PAGE_TABLES_BASE && addr <= PAGE_TABLES_END) || (Type != COUNTRegion && Type != ContiguousRegion)*/) {
		if (IsValidVirtualAddress(addr)) {
			EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "Applying identity mapping hack to allocation at address 0x%X", addr);
			Unlock();
			RETURN(addr);
			/*
			if (Type == UserRegion) {
				EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "Applying identity mapping hack to allocation at address 0x%X", addr);
				Unlock();
				RETURN(addr); // committed pages in the user region always use VirtualAlloc
			}
			else if (Type != COUNTRegion) {
				VMAIter it = GetVMAIterator(addr, Type);
				if (it != m_MemoryRegionArray[Type].RegionMap.end() && it->second.type != FreeVma && it->second.bFragmented) {
					EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "Applying identity mapping hack to allocation at address 0x%X", addr);
					Unlock();
					RETURN(addr); // committed pages in the system-devkit regions can use VirtualAlloc because of fragmentation
				}
			}
			else {
				// This is the special case of the page tables region at 0xC0000000. This area doesn't have a memory region assigned to,
				// and never uses VirtualAlloc, but it's still affected by the above problem since its physical pages don't come from
				// our memory.bin at 0x80000000, so it needs the hack as well.

				EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "Applying identity mapping hack to allocation at address 0x%X", addr);
				Unlock();
				RETURN(addr);
			}
			*/
		}
		else {
			goto InvalidAddress;
		}
	}

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
	vma.type = FreeVma;
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
	assert(vma.type == FreeVma);

	u32 start_in_vma = base - vma.base; // VAddr - start addr of vma region found (must be Free)
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
		if (it->second.type == FreeVma) { assert(0); }
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
		DbgPrintf(LOG_PREFIX, "VirtualProtect failed. The error code was %d\n", GetLastError());
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
			DbgPrintf(LOG_PREFIX, "Located vma but sizes don't match\n");
			return m_MemoryRegionArray[Type].RegionMap.end();
		}
		return it;
	}
	else
	{
		DbgPrintf(LOG_PREFIX, "Vma not found or doesn't start at the supplied address\n");
		return m_MemoryRegionArray[Type].RegionMap.end();
	}
}

VMAIter VMManager::CheckConflictingVMA(VAddr addr, size_t Size, bool* bOverflow)
{
	*bOverflow = false;
	VMAIter it = GetVMAIterator(addr, UserRegion);

	if (it == m_MemoryRegionArray[UserRegion].RegionMap.end()) {
		// Pretend we are overflowing since an overflow is always an error. Otherwise, end() will be interpreted
		// as a found free VMA.

		*bOverflow = true;
		return it;
	}

	if (it->first + it->second.size - 1 < addr + Size - 1) {
		*bOverflow = true;
	}

	if (it->second.type != FreeVma) {
		return it; // conflict
	}

	return m_MemoryRegionArray[UserRegion].RegionMap.end(); // no conflict
}

void VMManager::DestructVMA(VAddr addr, MemoryRegionType Type, size_t Size)
{
	BOOL ret;
	VMAIter it = GetVMAIterator(addr, Type); // the caller should already guarantee that the vma exists

	// Don't free our memory placeholder and allocations on the contiguous region since they don't use VirtualAlloc and MapViewOfFileEx

	if ((addr >= XBE_MAX_VA) && (Type != ContiguousRegion))
	{
			if (it->second.bFragmented)
			{
				// NOTE: unlike NtFreeVirtualMemory, VirtualFree cannot release a committed region only partially, the size must always be 0
				// with MEM_RELEASE. This is a problem because some games can call us from XbFreeVirtualMemory with XBOX_MEM_RELEASE and a
				// size != 0, so, in this case, we can only decommit the region to avoid memory leaks. This essentially means that we will
				// leave behind reserved areas, which will decrease the total amount of host virtual space available...

				if (addr == it->first && Size == it->second.size) { ret = VirtualFree((void*)addr, 0, MEM_RELEASE); }
				else { ret = VirtualFree((void*)addr, Size, MEM_DECOMMIT); }
			}
			else
			{
				ret = UnmapViewOfFile((void*)(ROUND_DOWN(it->first, m_AllocationGranularity)));
			}

			if (!ret)
			{
				DbgPrintf(LOG_PREFIX, "Deallocation routine failed with error %d\n", GetLastError());
			}
	}

	VMAIter CarvedVmaIt = CarveVMARange(addr, Size, Type);

	VAddr target_end = addr + Size;
	VMAIter it_end = m_MemoryRegionArray[Type].RegionMap.end();
	VMAIter it_begin = m_MemoryRegionArray[Type].RegionMap.begin();

	// The comparison against the end of the range must be done using addresses since vma's can be
	// merged during this process, causing invalidation of the iterators
	while (CarvedVmaIt != it_end && CarvedVmaIt->second.base < target_end)
	{
		CarvedVmaIt = std::next(UnmapVMA(CarvedVmaIt, Type));
	}

	// If we free an entire vma, prev(CarvedVmaIt) will be the freed vma. If it is not, we'll do a standard search

	if (CarvedVmaIt != it_begin && std::prev(CarvedVmaIt)->second.type == FreeVma)
	{
		m_MemoryRegionArray[Type].LastFree = std::prev(CarvedVmaIt);
		return;
	}
	else
	{
		DbgPrintf(LOG_PREFIX, "std::prev(CarvedVmaIt) was not free\n");

		it = CarvedVmaIt;

		while (it != it_end)
		{
			if (it->second.type == FreeVma)
			{
				m_MemoryRegionArray[Type].LastFree = it;
				return;
			}
			++it;
		}

		it = std::prev(CarvedVmaIt);

		while (true)
		{
			if (it->second.type == FreeVma)
			{
				m_MemoryRegionArray[Type].LastFree = it;
				return;
			}

			if (it == it_begin) { break; }
			--it;
		}

		EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "Can't find any more free space in the memory region %d! Virtual memory exhausted?", Type);

		m_MemoryRegionArray[Type].LastFree = m_MemoryRegionArray[Type].RegionMap.end();

		return;
	}
}
