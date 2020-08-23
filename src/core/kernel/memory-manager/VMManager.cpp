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
// *  (c) 2017-2018-2019      ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

// Acknowledgment:
// Some of the functions with the suffix VMA are from the vm_manager.cpp file of the citra emulator

// Copyright 2015 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file of Citra at https://github.com/citra-emu/citra/blob/master/license.txt.


#define LOG_PREFIX CXBXR_MODULE::VMEM

#include "common/AddressRanges.h"
#include "PoolManager.h"
#include "Logging.h"
#include "EmuShared.h"
#include "core\kernel\exports\EmuKrnl.h" // For InitializeListHead(), etc.
#include "common/util/cliConfig.hpp" // For GetSessionID
#include <assert.h>
// Temporary usage for need ReserveAddressRanges func with cxbx.exe's emulation.
#ifndef CXBXR_EMU
#include "common/ReserveAddressRanges.h"
#endif


constexpr char str_persistent_memory_s[] = "PersistentMemory-s";
VMManager g_VMManager;


void VMManager::Shutdown()
{
	// Can't enable this yet. After the memory is deleted, other parts of cxbxr still run before process termination, and attempt to
	// access the now deleted memory, causing a crash at shutdown
	//DestroyMemoryRegions();
	DeleteCriticalSection(&m_CriticalSection);

	if (m_PersistentMemoryHandle != nullptr) {
		CloseHandle(m_PersistentMemoryHandle);
		m_PersistentMemoryHandle = nullptr;
	}
}

bool VirtualMemoryArea::CanBeMergedWith(const VirtualMemoryArea& next) const
{
	assert(base + size == next.base);

	if (type == FreeVma && next.type == FreeVma) { return true; }

	return false;
}

void VMManager::Initialize(unsigned int SystemType, int BootFlags, blocks_reserved_t blocks_reserved)
{
	if ((BootFlags & BOOT_QUICK_REBOOT) == 0) {
		g_EmuShared->SetMmLayout(&SystemType);
	}
	else {
		g_EmuShared->GetMmLayout(&SystemType);
	}

	m_MmLayoutChihiro = (SystemType == SYSTEM_CHIHIRO);
	m_MmLayoutDebug = (SystemType == SYSTEM_DEVKIT);
	m_MmLayoutRetail = (SystemType == SYSTEM_XBOX);

	// Set up the critical section to synchronize accesses
	InitializeCriticalSectionAndSpinCount(&m_CriticalSection, 0x400);

	m_AllocationGranularity = KiB(64);
	g_SystemMaxMemory = XBOX_MEMORY_SIZE;

	// Set up the structs tracking the memory regions
	ConstructMemoryRegion(LOWEST_USER_ADDRESS, USER_MEMORY_SIZE, UserRegion);
	ConstructMemoryRegion(CONTIGUOUS_MEMORY_BASE, XBOX_CONTIGUOUS_MEMORY_SIZE, ContiguousRegion);
	ConstructMemoryRegion(SYSTEM_MEMORY_BASE, SYSTEM_MEMORY_SIZE, SystemRegion);
	ConstructMemoryRegion(DEVKIT_MEMORY_BASE, DEVKIT_MEMORY_SIZE, DevkitRegion);

	// Commit all the memory reserved by the loader for the PTs
	// We are looping here because memory-reservation happens in 64 KiB increments
	for (int i = 0; i < 64; i++) {
		LPVOID ret = VirtualAlloc((LPVOID)(PAGE_TABLES_BASE + i * m_AllocationGranularity), m_AllocationGranularity, MEM_COMMIT, PAGE_READWRITE);
		if (ret != (LPVOID)(PAGE_TABLES_BASE + i * KiB(64))) {
			CxbxKrnlCleanup("VirtualAlloc failed to commit the memory for the page tables. The error was 0x%08X", GetLastError());
		}
	}

	// Construct VMAs base on reserved bit indexes for devkit and system region blocks.
	// See "blocks_reserved_t" notes.
	if (SystemType == SYSTEM_DEVKIT) {
		for (unsigned int i = BLOCK_REGION_DEVKIT_INDEX_BEGIN; i < BLOCK_REGION_DEVKIT_INDEX_END; i++) {
			if ((blocks_reserved[i / 32] & (1 << (i % 32))) == 0) {
				// The loader was unable to reserve this block, so discard it from the memory region
				ConstructVMA(DEVKIT_MEMORY_BASE + i * KiB(64), KiB(64), DevkitRegion, ReservedVma, false);
			}
		}
	}
	for (unsigned int i = BLOCK_REGION_SYSTEM_INDEX_BEGIN; i < BLOCK_REGION_SYSTEM_INDEX_END; i++) {
		if ((blocks_reserved[i / 32] & (1 << (i % 32))) == 0) {
			// The loader was unable to reserve this block, so discard it from the memory region
			ConstructVMA(SYSTEM_MEMORY_BASE + i * KiB(64), KiB(64), SystemRegion, ReservedVma, false);
		}
	}

	// Set up general memory variables according to the xbe type
	if (m_MmLayoutChihiro)
	{
		m_MaxContiguousPfn = CHIHIRO_CONTIGUOUS_MEMORY_LIMIT;
		g_SystemMaxMemory = CHIHIRO_MEMORY_SIZE;
		m_PhysicalPagesAvailable = g_SystemMaxMemory >> PAGE_SHIFT;
		m_HighestPage = CHIHIRO_HIGHEST_PHYSICAL_PAGE;
		m_NV2AInstancePage = CHIHIRO_INSTANCE_PHYSICAL_PAGE;
		m_MemoryRegionArray[ContiguousRegion].RegionMap[CONTIGUOUS_MEMORY_BASE].size = CHIHIRO_CONTIGUOUS_MEMORY_SIZE;
	}
	else if (m_MmLayoutDebug)
	{
		g_SystemMaxMemory = CHIHIRO_MEMORY_SIZE;
		m_DebuggerPagesAvailable = X64M_PHYSICAL_PAGE;
		m_HighestPage = CHIHIRO_HIGHEST_PHYSICAL_PAGE;
		m_MemoryRegionArray[ContiguousRegion].RegionMap[CONTIGUOUS_MEMORY_BASE].size = CHIHIRO_CONTIGUOUS_MEMORY_SIZE;

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
	block->ListEntry.Flink = block->ListEntry.Blink = nullptr; // Was LIST_ENTRY_INITIALIZE()
	InsertHeadList(ListEntry, &block->ListEntry);

	if ((BootFlags & BOOT_QUICK_REBOOT) == 0) {
		InitializeSystemAllocations();
	}
	else {
		RestorePersistentMemory();
	}

	// Initialize the pool manager
	g_PoolManager.InitializePool();

	// Construct the page directory
	InitializePageDirectory();

	// Reserve the xbe image memory. Doing this now allows us to avoid calling XbAllocateVirtualMemory later
	ConstructVMA(XBE_IMAGE_BASE, ROUND_UP_4K(CxbxKrnl_Xbe->m_Header.dwSizeofImage), UserRegion, ReservedVma, XBOX_PAGE_READWRITE);
	m_VirtualMemoryBytesReserved += ROUND_UP_4K(CxbxKrnl_Xbe->m_Header.dwSizeofImage);

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
	// VirtualAlloc cannot be used in the contiguous region so skip it
	for (int i = 0; i < COUNTRegion - 1; ++i)
	{
		for (auto& it : m_MemoryRegionArray[i].RegionMap)
		{
			if (it.second.type != FreeVma && it.first >= XBE_MAX_VA)
			{
				VirtualFree((void*)it.first, 0, MEM_RELEASE);
			}
		}
	}
}

void VMManager::InitializeSystemAllocations()
{
	PFN pfn;
	PFN pfn_end;
	PMMPTE PointerPte;
	PMMPTE EndingPte;
	VAddr addr;


	// Construct the pfn of the page used by D3D
	AllocateContiguousMemory(PAGE_SIZE, D3D_PHYSICAL_PAGE, D3D_PHYSICAL_PAGE + PAGE_SIZE - 1, 0, XBOX_PAGE_READWRITE);
	PersistMemory(CONTIGUOUS_MEMORY_BASE, PAGE_SIZE, true);


	// Construct the pfn of the page directory
	AllocateContiguousMemory(PAGE_SIZE, PAGE_DIRECTORY_PHYSICAL_ADDRESS, PAGE_DIRECTORY_PHYSICAL_ADDRESS + PAGE_SIZE - 1, 0, XBOX_PAGE_READWRITE);
	PersistMemory(CONTIGUOUS_MEMORY_BASE + PAGE_DIRECTORY_PHYSICAL_ADDRESS, PAGE_SIZE, true);


	// Construct the pfn's of the kernel pages
	AllocateContiguousMemory(KERNEL_SIZE, XBE_IMAGE_BASE, XBE_IMAGE_BASE + ROUND_UP_4K(KERNEL_SIZE) - 1, 0, XBOX_PAGE_READWRITE);
	PersistMemory(CONTIGUOUS_MEMORY_BASE + XBE_IMAGE_BASE, KERNEL_SIZE, true);


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

	AllocateContiguousMemoryInternal(pfn_end - pfn + 1, pfn, pfn_end, 1, XBOX_PAGE_READWRITE);
	PersistMemory(addr, (pfn_end - pfn + 1) << PAGE_SHIFT, true);
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

	AllocateContiguousMemoryInternal(pfn_end - pfn + 1, pfn, pfn_end, 1, XBOX_PAGE_READWRITE);
	while (PointerPte <= EndingPte) {
		DISABLE_CACHING(*PointerPte);
		PointerPte++;
	}

	if (m_MmLayoutDebug)
	{
		// Debug kits have two nv2a instance memory, another at the top of the 128 MiB

		pfn += DEBUGKIT_FIRST_UPPER_HALF_PAGE;
		pfn_end += DEBUGKIT_FIRST_UPPER_HALF_PAGE;
		addr = (VAddr)CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn);
		PointerPte = GetPteAddress(addr);
		EndingPte = GetPteAddress(CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn_end));

		AllocateContiguousMemoryInternal(pfn_end - pfn + 1, pfn, pfn_end, 1, XBOX_PAGE_READWRITE);
		while (PointerPte <= EndingPte) {
			DISABLE_CACHING(*PointerPte);
			PointerPte++;
		}
	}
}

void VMManager::GetPersistentMemory()
{
	if (m_PersistentMemoryHandle != nullptr) {
		CxbxKrnlCleanup("Persistent memory is already opened!");
		return;
	}

	std::string persisted_mem_sid = str_persistent_memory_s + std::to_string(cli_config::GetSessionID());
	m_PersistentMemoryHandle = OpenFileMapping(FILE_MAP_READ, FALSE, persisted_mem_sid.c_str());
	if (m_PersistentMemoryHandle == nullptr) {
		CxbxKrnlCleanup("Couldn't open persistent memory! OpenFileMapping failed with error 0x%08X", GetLastError());
		return;
	}
}

void VMManager::RestorePersistentMemory()
{
	if (m_PersistentMemoryHandle == nullptr) {
		CxbxKrnlCleanup("Persistent memory is not open!");
		return;
	}

	PersistedMemory* persisted_mem = (PersistedMemory*)MapViewOfFile(m_PersistentMemoryHandle, FILE_MAP_READ, 0, 0, 0);
	if (persisted_mem == nullptr) {
		CxbxKrnlCleanup("Couldn't restore persistent memory! MapViewOfFile failed with error 0x%08X", GetLastError());
		return;
	}

	if (persisted_mem->LaunchFrameAddresses[0] != 0 && IS_PHYSICAL_ADDRESS(persisted_mem->LaunchFrameAddresses[0])) {
		xboxkrnl::LaunchDataPage = (xboxkrnl::PLAUNCH_DATA_PAGE)persisted_mem->LaunchFrameAddresses[0];
		EmuLog(LOG_LEVEL::INFO, "Restored LaunchDataPage\n");
	}

	if (persisted_mem->LaunchFrameAddresses[1] != 0 && IS_PHYSICAL_ADDRESS(persisted_mem->LaunchFrameAddresses[1])) {
		xboxkrnl::AvSavedDataAddress = (xboxkrnl::PVOID)persisted_mem->LaunchFrameAddresses[1];
		EmuLog(LOG_LEVEL::INFO, "Restored Framebuffer\n");
	}

	MMPTE pte;
	PFN pfn;
	for (unsigned int i = 0; i < persisted_mem->NumOfPtes; i++) {
		pte.Default = persisted_mem->Data[persisted_mem->NumOfPtes + i];
		assert(pte.Hardware.Valid != 0 && pte.Hardware.Persist != 0);
		memcpy(GetPteAddress(persisted_mem->Data[i]), &pte.Default, sizeof(MMPTE));
		RemoveFree(1, &pfn, 0, pte.Hardware.PFN, pte.Hardware.PFN);
		if (m_MmLayoutChihiro) {
			memcpy(CHIHIRO_PFN_ELEMENT(pte.Hardware.PFN),
				&((PXBOX_PFN)&persisted_mem->Data[(persisted_mem->NumOfPtes * 2) + (persisted_mem->NumOfPtes - 32) * KiB(1)])[pte.Hardware.PFN],
				sizeof(XBOX_PFN));
			if ((uint32_t*)persisted_mem->Data[i] < (uint32_t*)CHIHIRO_PFN_ADDRESS) {
				memcpy((void*)(persisted_mem->Data[i]), &persisted_mem->Data[persisted_mem->NumOfPtes * 2 + i * KiB(1)], PAGE_SIZE);
			}
		}
		else {
			memcpy(XBOX_PFN_ELEMENT(pte.Hardware.PFN),
				&((PXBOX_PFN)&persisted_mem->Data[(persisted_mem->NumOfPtes * 2) + (persisted_mem->NumOfPtes - 16) * KiB(1)])[pte.Hardware.PFN],
				sizeof(XBOX_PFN));
			if ((uint32_t*)persisted_mem->Data[i] < (uint32_t*)XBOX_PFN_ADDRESS) {
				memcpy((void*)(persisted_mem->Data[i]), &persisted_mem->Data[persisted_mem->NumOfPtes * 2 + i * KiB(1)], PAGE_SIZE);
			}
		}
	}

	PFN_COUNT pages_num = 1;
	for (unsigned int i = 0; i < persisted_mem->NumOfPtes; i++) {
		pte.Default = persisted_mem->Data[persisted_mem->NumOfPtes + i];
		if (pte.Hardware.GuardOrEnd == 0) {
			pages_num++;
			continue;
		}
		size_t size = pages_num << PAGE_SHIFT;
		VAddr addr = persisted_mem->Data[i] - (size - PAGE_SIZE);
		AllocatePT(size, addr);
		ConstructVMA(addr, size, ContiguousRegion, AllocatedVma, false);
		GetPfnOfPT(GetPteAddress(addr))->PTPageFrame.PtesUsed += pages_num;
		pages_num = 1;
	}

	if (m_MmLayoutDebug) { m_PhysicalPagesAvailable += 16; m_DebuggerPagesAvailable -= 16; }

	PFN pfn_end;
	if (m_MmLayoutRetail || m_MmLayoutDebug) {
		pfn = XBOX_INSTANCE_PHYSICAL_PAGE;
		pfn_end = XBOX_INSTANCE_PHYSICAL_PAGE + NV2A_INSTANCE_PAGE_COUNT - 1;
	}
	else {
		pfn = CHIHIRO_INSTANCE_PHYSICAL_PAGE;
		pfn_end = CHIHIRO_INSTANCE_PHYSICAL_PAGE + NV2A_INSTANCE_PAGE_COUNT - 1;
	}
	VAddr addr = (VAddr)CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn);
	PMMPTE PointerPte = GetPteAddress(addr);
	PMMPTE EndingPte = GetPteAddress(CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn_end));

	AllocateContiguousMemoryInternal(pfn_end - pfn + 1, pfn, pfn_end, 1, XBOX_PAGE_READWRITE);
	while (PointerPte <= EndingPte) {
		DISABLE_CACHING(*PointerPte);
		PointerPte++;
	}

	if (m_MmLayoutDebug)
	{
		// Debug kits have two nv2a instance memory, another at the top of the 128 MiB

		pfn += DEBUGKIT_FIRST_UPPER_HALF_PAGE;
		pfn_end += DEBUGKIT_FIRST_UPPER_HALF_PAGE;
		addr = (VAddr)CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn);
		PointerPte = GetPteAddress(addr);
		EndingPte = GetPteAddress(CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(pfn_end));

		AllocateContiguousMemoryInternal(pfn_end - pfn + 1, pfn, pfn_end, 1, XBOX_PAGE_READWRITE);
		while (PointerPte <= EndingPte) {
			DISABLE_CACHING(*PointerPte);
			PointerPte++;
		}
	}

	UnmapViewOfFile(persisted_mem);
	CloseHandle(m_PersistentMemoryHandle);
	m_PersistentMemoryHandle = nullptr;
}

void VMManager::SavePersistentMemory()
{
	PersistedMemory* persisted_mem;
	size_t num_persisted_ptes;
	std::vector<PMMPTE> cached_persisted_ptes;
	LPVOID addr;
	PMMPTE PointerPte;
	PMMPTE EndingPte;
	int i;

	Lock();

	num_persisted_ptes = 0;
	PointerPte = GetPteAddress(CONTIGUOUS_MEMORY_BASE);

	if (m_MmLayoutRetail) {
		EndingPte = GetPteAddress(CONTIGUOUS_MEMORY_BASE + XBOX_CONTIGUOUS_MEMORY_SIZE - 1);
	}
	else {
		EndingPte = GetPteAddress(CONTIGUOUS_MEMORY_BASE + CHIHIRO_CONTIGUOUS_MEMORY_SIZE - 1);
	}

	while (PointerPte <= EndingPte)
	{
		if (PointerPte->Hardware.Valid != 0 && PointerPte->Hardware.Persist != 0) {
			cached_persisted_ptes.push_back(PointerPte);
			num_persisted_ptes++;
		}
		PointerPte++;
	}

	std::string persistent_mem_sid = str_persistent_memory_s + std::to_string(cli_config::GetSessionID());
	m_PersistentMemoryHandle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, num_persisted_ptes * PAGE_SIZE + num_persisted_ptes * 4 * 2 + sizeof(PersistedMemory), persistent_mem_sid.c_str());
	if (m_PersistentMemoryHandle == NULL) {
		CxbxKrnlCleanup("Couldn't persist memory! CreateFileMapping failed with error 0x%08X", GetLastError());
		return;
	}
	addr = MapViewOfFile(m_PersistentMemoryHandle, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	if (addr == nullptr) {
		CxbxKrnlCleanup("Couldn't persist memory! MapViewOfFile failed with error 0x%08X", GetLastError());
		return;
	}

	persisted_mem = (PersistedMemory*)addr;
	persisted_mem->NumOfPtes = num_persisted_ptes;

	if (xboxkrnl::LaunchDataPage != xbnullptr) {
		persisted_mem->LaunchFrameAddresses[0] = (VAddr)xboxkrnl::LaunchDataPage;
		EmuLog(LOG_LEVEL::INFO, "Persisted LaunchDataPage\n");
	}

	if (xboxkrnl::AvSavedDataAddress != xbnullptr) {
		persisted_mem->LaunchFrameAddresses[1] = (VAddr)xboxkrnl::AvSavedDataAddress;
		EmuLog(LOG_LEVEL::INFO, "Persisted Framebuffer\n");
	}

	i = 0;

	for (const auto &pte : cached_persisted_ptes) {
		persisted_mem->Data[i] = GetVAddrMappedByPte(pte);
		persisted_mem->Data[num_persisted_ptes + i] = pte->Default;
		memcpy(&persisted_mem->Data[num_persisted_ptes * 2 + i * KiB(1)], (void *)(persisted_mem->Data[i]), PAGE_SIZE);
		i++;
	}

	assert(i == num_persisted_ptes);

	Unlock();
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

		EmuLog(LOG_LEVEL::DEBUG, "MmClaimGpuInstanceMemory : Allocated bytes remaining = 0x%.8X", m_NV2AInstanceMemoryBytes);

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

	if (bPersist) {
		while (PointerPte <= EndingPte)
		{
			PointerPte->Hardware.Persist = 1;
			PointerPte++;
		}
	}
	else {
		while (PointerPte <= EndingPte)
		{
			PointerPte->Hardware.Persist = 0;
			PointerPte++;
		}
	}

	Unlock();
}

VAddr VMManager::Allocate(size_t Size)
{
	LOG_FUNC_ONE_ARG(Size);

	MMPTE TempPte;
	PMMPTE PointerPte;
	PMMPTE EndingPte;
	PFN pfn;
	PFN_COUNT PteNumber;
	VAddr addr;

	if (!Size) { RETURN(NULL); }

	Lock();

	PteNumber = ROUND_UP_4K(Size) >> PAGE_SHIFT;

	if (!IsMappable(PteNumber, true, m_MmLayoutDebug && m_bAllowNonDebuggerOnTop64MiB ? true : false)) { goto Fail; }

	ConvertXboxToPtePermissions(XBOX_PAGE_EXECUTE_READWRITE, &TempPte);

	addr = MapMemoryBlock(UserRegion, PteNumber, MEM_RESERVE | MEM_COMMIT, false);
	if (!addr) { goto Fail; }

	// Check if we have to construct the PT's for this allocation
	if (!AllocatePT(PteNumber << PAGE_SHIFT, addr))
	{
		VirtualFree((void*)addr, 0, MEM_RELEASE);
		goto Fail;
	}

	// Finally, write the pte's and the pfn's
	PointerPte = GetPteAddress(addr);
	EndingPte = PointerPte + PteNumber - 1;

	while (PointerPte <= EndingPte)
	{
		RemoveFree(1, &pfn, 0, 0, m_MmLayoutDebug && !m_bAllowNonDebuggerOnTop64MiB ? XBOX_HIGHEST_PHYSICAL_PAGE
			: m_HighestPage);
		WritePfn(pfn, pfn, PointerPte, ImageType);
		WritePte(PointerPte, PointerPte, TempPte, pfn);

		PointerPte++;
	}

	ConstructVMA(addr, PteNumber << PAGE_SHIFT, UserRegion, AllocatedVma);
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
	PFN LowestAcceptablePfn;
	PFN HighestAcceptablePfn;
	PFN_COUNT PteNumber;
	PFN_COUNT PagesNumber;
	VAddr addr;
	MemoryRegionType MemoryType;

	// NOTE: AllocateSystemMemory won't allocate a physical page for the guard page (if requested) and just adds an extra
	// unallocated virtual page in front of the mapped allocation. For this reason we will decommmit later the extra guard page allocated

	if (!Size || !ConvertXboxToSystemPtePermissions(Perms, &TempPte)) { RETURN(NULL); }

	LowestAcceptablePfn = 0;
	HighestAcceptablePfn = m_MmLayoutDebug ? XBOX_HIGHEST_PHYSICAL_PAGE : m_HighestPage;
	MemoryType = SystemRegion;

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

	addr = MapMemoryBlock(MemoryType, PteNumber, MEM_COMMIT, true);

	if (!addr) { goto Fail; }

	// check if we have to construct the PT's for this allocation
	if (!AllocatePT(PteNumber << PAGE_SHIFT, addr))
	{
		VirtualFree((void*)addr, 0, MEM_DECOMMIT);
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

	while (PointerPte <= EndingPte)
	{
		RemoveFree(1, &pfn, 0, LowestAcceptablePfn, HighestAcceptablePfn);
		WritePfn(pfn, pfn, PointerPte, BusyType);
		WritePte(PointerPte, PointerPte, TempPte, pfn);

		PointerPte++;
	}
	EndingPte->Hardware.GuardOrEnd = 1;

	UpdateMemoryPermissions(bAddGuardPage ? addr + PAGE_SIZE : addr, PagesNumber << PAGE_SHIFT, Perms);
	ConstructVMA(addr, PteNumber << PAGE_SHIFT, MemoryType, AllocatedVma);

	Unlock();
	RETURN(addr);

	Fail:
	Unlock();
	RETURN(NULL);
}

VAddr VMManager::AllocateContiguousMemory(size_t Size, PAddr LowestAddress, PAddr HighestAddress, ULONG Alignment, DWORD Perms)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Size)
		LOG_FUNC_ARG(LowestAddress)
		LOG_FUNC_ARG(HighestAddress)
		LOG_FUNC_ARG(Alignment)
		LOG_FUNC_ARG(Perms)
	LOG_FUNC_END;

	PFN PfnAlignment;
	PFN LowestPfn;
	PFN HighestPfn;
	PFN_COUNT PteNumber;
	VAddr Addr;

	if (!Size) { RETURN(NULL); }

	PteNumber = ROUND_UP_4K(Size) >> PAGE_SHIFT;
	LowestPfn = LowestAddress >> PAGE_SHIFT;
	HighestPfn = HighestAddress >> PAGE_SHIFT;
	PfnAlignment = Alignment >> PAGE_SHIFT;

	if (HighestPfn > m_MaxContiguousPfn) { HighestPfn = m_MaxContiguousPfn; }
	if (LowestPfn > HighestPfn) { LowestPfn = HighestPfn; }
	if (!PfnAlignment) { PfnAlignment = 1; }

	Lock();

	if (!IsMappable(PteNumber, true, false)) {
		Unlock();
		RETURN(NULL);
	}

	Addr = AllocateContiguousMemoryInternal(PteNumber, LowestPfn, HighestPfn, PfnAlignment, Perms);

	Unlock();

	RETURN(Addr);
}

VAddr VMManager::AllocateContiguousMemoryInternal(PFN_COUNT NumberOfPages, PFN LowestPfn, PFN HighestPfn, PFN PfnAlignment, DWORD Perms)
{
	MMPTE TempPte;
	PMMPTE PointerPte;
	PMMPTE EndingPte;
	PFN pfn;
	PFN EndingPfn;
	VAddr addr;

	if (!ConvertXboxToSystemPtePermissions(Perms, &TempPte)) { goto Fail; }
	if (!RemoveFree(NumberOfPages, &pfn, PfnAlignment, LowestPfn, HighestPfn)) { goto Fail; }
	addr = CONTIGUOUS_MEMORY_BASE + (pfn << PAGE_SHIFT);

	assert(CHECK_ALIGNMENT(pfn, PfnAlignment)); // check if the page alignment is correct

	EndingPfn = pfn + NumberOfPages - 1;

	// check if we have to construct the PT's for this allocation
	if (!AllocatePT(NumberOfPages << PAGE_SHIFT, addr))
	{
		InsertFree(pfn, EndingPfn);
		goto Fail;
	}

	// Finally, write the pte's and the pfn's
	PointerPte = GetPteAddress(addr);
	EndingPte = PointerPte + NumberOfPages - 1;

	WritePte(PointerPte, EndingPte, TempPte, pfn);
	WritePfn(pfn, EndingPfn, PointerPte, ContiguousType);
	EndingPte->Hardware.GuardOrEnd = 1;

	ConstructVMA(addr, NumberOfPages << PAGE_SHIFT, ContiguousRegion, AllocatedVma, false);
	UpdateMemoryPermissions(addr, NumberOfPages << PAGE_SHIFT, Perms);

	return addr;

	Fail:
	return NULL;
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

	if (!Size || !ConvertXboxToSystemPtePermissions(Perms, &TempPte)) { RETURN(NULL); }

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
	// be handled by EmuException.

	PteNumber = PAGES_SPANNED(Paddr, Size);

	Lock();

	addr = MapMemoryBlock(SystemRegion, PteNumber, 0xFFFFFFFF, true);

	if (!addr) { goto Fail; }

	// Check if we have to construct the PT's for this allocation
	if (!AllocatePT(PteNumber << PAGE_SHIFT, addr)) {
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

	PMMPTE PointerPte;
	PMMPTE StartingPte;
	PMMPTE EndingPte;
	PFN pfn;
	PFN_COUNT PteNumber;
	VMAIter it;
	bool bOverflow;

	assert(CHECK_ALIGNMENT(addr, PAGE_SIZE)); // all starting addresses in the user region are page aligned
	assert(IS_USER_ADDRESS(addr));

	Lock();

	it = CheckConflictingVMA(addr, 0, UserRegion, &bOverflow);

	if (it == m_MemoryRegionArray[UserRegion].RegionMap.end() || bOverflow)
	{
		Unlock();
		return;
	}

	PointerPte = GetPteAddress(addr);
	EndingPte = PointerPte + (it->second.size >> PAGE_SHIFT) - 1;
	StartingPte = PointerPte;
	PteNumber = EndingPte - StartingPte + 1;

	while (PointerPte <= EndingPte)
	{
		pfn = PointerPte->Hardware.PFN;
		InsertFree(pfn, pfn);
		WritePfn(pfn, pfn, PointerPte, ImageType, true);

		PointerPte++;
	}

	WritePte(StartingPte, EndingPte, *StartingPte, 0, true);
	DestructVMA(it->first, UserRegion, it->second.size);
	DeallocatePT(PteNumber << PAGE_SHIFT, addr);

	Unlock();
}

void VMManager::DeallocateContiguousMemory(VAddr addr)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(addr)
	LOG_FUNC_END;

	PMMPTE StartingPte;
	PMMPTE EndingPte;
	PFN pfn;
	PFN EndingPfn;
	VMAIter it;
	bool bOverflow;

	assert(CHECK_ALIGNMENT(addr, PAGE_SIZE)); // all starting addresses in the contiguous region are page aligned
	assert(IS_PHYSICAL_ADDRESS(addr));

	Lock();

	it = CheckConflictingVMA(addr, 0, ContiguousRegion, &bOverflow);

	if (it == m_MemoryRegionArray[ContiguousRegion].RegionMap.end() || bOverflow)
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

	PMMPTE PointerPte;
	PMMPTE StartingPte;
	PMMPTE EndingPte;
	PFN pfn;
	PFN_COUNT PteNumber;
	VMAIter it;
	MemoryRegionType MemoryType = SystemRegion;
	bool bGuardPageAdded = false;
	bool bOverflow;

	assert(CHECK_ALIGNMENT(addr, PAGE_SIZE)); // all starting addresses in the system region are page aligned

	Lock();

	if (BusyType == DebuggerType)
	{
		assert(IS_DEVKIT_ADDRESS(addr));
		MemoryType = DevkitRegion;
	}
	else { assert(IS_SYSTEM_ADDRESS(addr)); }

	it = CheckConflictingVMA(addr, 0, MemoryType, &bOverflow);

	if (it == m_MemoryRegionArray[MemoryType].RegionMap.end() || bOverflow)
	{
		Unlock();
		RETURN(NULL);
	}

	// Calculate the size of the original allocation
	if (Size) { Size = ROUND_UP_4K(Size); }
	else { Size = it->second.size; }

	PointerPte = GetPteAddress(addr);
	if (PointerPte->Hardware.Valid == 0) {
		WritePte(PointerPte, PointerPte, *PointerPte, 0, true); // this is the guard page of the stack
		PointerPte++;
		Size -= PAGE_SIZE;
		bGuardPageAdded = true;
	}

	EndingPte = PointerPte + (Size >> PAGE_SHIFT) - 1;
	StartingPte = PointerPte;
	PteNumber = EndingPte - PointerPte + 1;

	while (PointerPte <= EndingPte)
	{
		pfn = PointerPte->Hardware.PFN;
		InsertFree(pfn, pfn);
		WritePfn(pfn, pfn, PointerPte, BusyType, true);

		PointerPte++;
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
		bool bOverflow;

		// The starting address of a device can be unaligned since MapDeviceMemory returns an offset from the aligned
		// mapped address, so we won't assert the address here

		Lock();

		Size = PAGES_SPANNED(addr, Size) << PAGE_SHIFT;
		it = CheckConflictingVMA(addr, Size, SystemRegion, &bOverflow);

		if (it == m_MemoryRegionArray[SystemRegion].RegionMap.end() || bOverflow)
		{
			Unlock();
			return;
		}

		StartingPte = GetPteAddress(addr);
		EndingPte = StartingPte + (Size >> PAGE_SHIFT) - 1;
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

	if (!Size || !ConvertXboxToSystemPtePermissions(NewPerms, &NewPermsPte)) { return; }

	Lock();

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
				Protect = ConvertPteToXboxPermissions(TempPte.Default);
			}
			else
			{
				Protect = 0; // invalid page, return failure
			}
		}
		else
		{
			Protect = ConvertPteToXboxPermissions(TempPte.Default); // large page, query it immediately
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
			EmuLog(LOG_LEVEL::DEBUG, "QuerySize: Unknown memory region queried.");
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

	if (!ConvertXboxToPtePermissions(Protect, &TempPte)) { RETURN(STATUS_INVALID_PAGE_PROTECTION); }

	EmuLog(LOG_LEVEL::DEBUG, "%s requested range : 0x%.8X - 0x%.8X", __func__, CapturedBase, CapturedBase + CapturedSize);

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

			if (AllocationType & XBOX_MEM_TOP_DOWN) { m_MemoryRegionArray[UserRegion].LastFree = GetVMAIterator(MaxAllowedAddress, UserRegion); }

			// On the Xbox, blocks reserved by NtAllocateVirtualMemory are 64K aligned and the size is rounded up on a 4K boundary.

			AlignedCapturedBase = MapMemoryBlock(UserRegion, AlignedCapturedSize >> PAGE_SHIFT, MEM_RESERVE, false, MaxAllowedAddress);

			if (!AlignedCapturedBase) { status = STATUS_NO_MEMORY; goto Exit; }
		}
		else
		{
			AlignedCapturedBase = ROUND_DOWN(CapturedBase, X64KB);
			AlignedCapturedSize = ROUND_UP_4K(CapturedSize);
			it = CheckConflictingVMA(AlignedCapturedBase, AlignedCapturedSize, UserRegion, &bOverflow);

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
					ConvertXboxToWinPermissions(PatchXboxPermissions(Protect)) & ~(PAGE_WRITECOMBINE | PAGE_NOCACHE)) != AlignedCapturedBase)
				{
					// An host allocation is already mapped there, report an error

					status = STATUS_CONFLICTING_ADDRESSES;
					goto Exit;
				}
			}
		}

		m_VirtualMemoryBytesReserved += AlignedCapturedSize;
		ConstructVMA(AlignedCapturedBase, AlignedCapturedSize, UserRegion, ReservedVma, Protect);

		if ((AllocationType & XBOX_MEM_COMMIT) == 0)
		{
			// XBOX_MEM_COMMIT was not specified, so we are done with the allocation

			EmuLog(LOG_LEVEL::DEBUG, "%s resulting range : 0x%.8X - 0x%.8X", __func__, AlignedCapturedBase, AlignedCapturedBase + AlignedCapturedSize);

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

	it = CheckConflictingVMA(AlignedCapturedBase, AlignedCapturedSize, UserRegion, &bOverflow);

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

	// Attempt to commit the requested range with VirtualAlloc *before* setting up and reserving the PT
	// This allows an early-out in a failure scenario (Test Case: Star Wars Battlefront DVD Demo: LA-018 v1.02)
    // We don't commit the requested range if it's within our placeholder, since that was already allocated earlier
	if (AlignedCapturedBase >= XBE_MAX_VA)
	{
		if (!VirtualAlloc((void*)AlignedCapturedBase, AlignedCapturedSize, MEM_COMMIT,
			(ConvertXboxToWinPermissions(PatchXboxPermissions(Protect))) & ~(PAGE_WRITECOMBINE | PAGE_NOCACHE)))
		{
			EmuLog(LOG_LEVEL::DEBUG, "%s: VirtualAlloc failed to commit the memory! The error was 0x%08X", __func__, GetLastError());
			status = STATUS_NO_MEMORY;
			goto Exit;
		}
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

	// Because VirtualAlloc always zeros the memory for us, XBOX_MEM_NOZERO is still unsupported

	if (AllocationType & XBOX_MEM_NOZERO) { EmuLog(LOG_LEVEL::DEBUG, "XBOX_MEM_NOZERO flag is not supported!"); }

	// If some pte's were detected to have different permissions in the above check, we need to update those as well

	if (bUpdatePteProtections)
	{
		VAddr TempAddr = AlignedCapturedBase;
		size_t TempSize = AlignedCapturedSize;
		DWORD TempProtect = Protect;
		XbVirtualProtect(&TempAddr, &TempSize, &TempProtect);
	}

	EmuLog(LOG_LEVEL::DEBUG, "%s resulting range : 0x%.8X - 0x%.8X", __func__, AlignedCapturedBase, AlignedCapturedBase + AlignedCapturedSize);

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

	it = CheckConflictingVMA(AlignedCapturedBase, AlignedCapturedSize, UserRegion, &bOverflow);

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
				EmuLog(LOG_LEVEL::DEBUG, "%s: VirtualFree failed to decommit the memory! The error was 0x%08X", __func__, GetLastError());
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

	if (!ConvertXboxToPtePermissions(NewPerms, &NewPermsPte)) { RETURN(STATUS_INVALID_PAGE_PROTECTION); }

	AlignedCapturedBase = ROUND_DOWN_4K(CapturedBase);
	AlignedCapturedSize = (PAGES_SPANNED(CapturedBase, CapturedSize)) << PAGE_SHIFT;

	Lock();

	it = CheckConflictingVMA(AlignedCapturedBase, AlignedCapturedSize, UserRegion, &bOverflow);

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
	*Protect = ConvertPteToXboxPermissions(OldPermsPte.Default);
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

	// If it's not in the placeholder, report actual host allocations. The game will see allocations it didn't make, but at least it has a chance to
	// not try to allocate memory our emulator already occupied.
	// Note1: VirtualQuery will always report the placeholder as committed, even when the VMManager will report it as free, so we need to query it
	// ourselves to correctly report committed/free areas in the placeholder.
	// Note2: when LLE CPU is implemented, this can be removed.
	if (addr >= XBE_MAX_VA)
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

	if (addr < LOWEST_USER_ADDRESS || (it != m_MemoryRegionArray[UserRegion].RegionMap.end() && it->second.type == FreeVma)) {

		// The address belongs to a free VMA, so we have little to do.
		if (addr < LOWEST_USER_ADDRESS) {
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
		CurrentProtect = ConvertPteToXboxPermissions(PointerPte->Default);
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

VAddr VMManager::MapMemoryBlock(MemoryRegionType Type, PFN_COUNT PteNumber, DWORD Permissions, bool b64Blocks, VAddr HighestAddress)
{
	VAddr addr;
	VMAIter it = m_MemoryRegionArray[Type].LastFree;
	size_t Size = PteNumber << PAGE_SHIFT;


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

		if (Permissions == 0xFFFFFFFF) {
			if (addr + Size - 1 < it->first + it->second.size) {
				return addr;
			}
			++it;
			continue;
		}

		// Note that, even in free regions, somebody outside the manager could have allocated the memory so we just
		// keep on trying until we succeed or fail entirely.

		size_t vma_end;
		if (HighestAddress && (it->first + it->second.size > HighestAddress + 1)) { vma_end = HighestAddress + 1; }
		else { vma_end = it->first + it->second.size; }

		if (b64Blocks) {
			if (addr + Size - 1 < vma_end) {
				// The memory was reserved by the loader, commit it in 64kb blocks
				VAddr start_addr = addr;
				size_t start_size = 0;
				while (addr < vma_end) {
					addr = MapHostMemory(addr, m_AllocationGranularity, vma_end, Permissions);
					assert(addr);
					start_size += m_AllocationGranularity;
					if (start_size >= Size) {
						return start_addr;
					}
					addr += m_AllocationGranularity;
				}
				assert(0);
			}
		}
		else {
			addr = MapHostMemory(addr, Size, vma_end, Permissions);
			if (addr) { return addr; }
		}

		++it;
	}

	// If we are here, it means we reached the end of the memory region. In desperation, we also try to map it from the
	// LastFree iterator and going backwards, since there could be holes created by deallocation operations...

	VMAIter begin_it = m_MemoryRegionArray[Type].RegionMap.begin();

	if (m_MemoryRegionArray[Type].LastFree == begin_it)
	{
		// We are already at the beginning of the map, so bail out immediately

		EmuLog(LOG_LEVEL::WARNING, "Failed to map a memory block in the virtual region %d!", Type);
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

			if (Permissions == 0xFFFFFFFF) {
				if (addr + Size - 1 < vma_end) {
					return addr;
				}
				--it;
				continue;
			}

			if (b64Blocks) {
				if (addr + Size - 1 < vma_end) {
					// The memory was reserved by the loader, commit it in 64kb blocks
					VAddr start_addr = addr;
					size_t start_size = 0;
					while (addr < vma_end) {
						addr = MapHostMemory(addr, m_AllocationGranularity, vma_end, Permissions);
						assert(addr);
						start_size += m_AllocationGranularity;
						if (start_size >= Size) {
							return start_addr;
						}
						addr += m_AllocationGranularity;
					}
					assert(0);
				}
			}
			else {
				addr = MapHostMemory(addr, Size, vma_end, Permissions);
				if (addr) { return addr; }
			}
		}

		if (it == begin_it) { break; }
		--it;
	}

	// We have failed to map the block. This is likely because the virtual space is fragmented or there are too many
	// host allocations in the memory region. Log this error and bail out

	EmuLog(LOG_LEVEL::WARNING, "Failed to map a memory block in the virtual region %d!", Type);

	return NULL;
}

VAddr VMManager::MapHostMemory(VAddr StartingAddr, size_t Size, size_t VmaEnd, DWORD Permissions)
{
	for (; StartingAddr + Size - 1 < VmaEnd; StartingAddr += m_AllocationGranularity)
	{
		if ((VAddr)VirtualAlloc((void*)StartingAddr, Size, Permissions, PAGE_EXECUTE_READWRITE) == StartingAddr)
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

	// ergo720: horrendous hack, this identity maps all allocations done by the VMManager to keep the LLE USB working.
	// The problem is that if the user buffer pointed to by the TD is allocated by the VMManager with VirtualAlloc, then
	// the physical allocation will not reside in the contiguous memory and if we tried to access the physical address of it,
	// we would access a random page with undefined contents.
	// NOTE: Once LLE CPU and MMU are implemented, this can be removed.

	if (IsValidVirtualAddress(addr)) {
		RETURN(addr);
	}
	else {
		RETURN(NULL);
	}

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
	vma.type = FreeVma;
	vma.permissions = XBOX_PAGE_NOACCESS;

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

	DWORD WindowsPerms = ConvertXboxToWinPermissions(PatchXboxPermissions(Perms));

	DWORD dummy;
	if (!VirtualProtect((void*)addr, Size, WindowsPerms & ~(PAGE_WRITECOMBINE | PAGE_NOCACHE), &dummy))
	{
		EmuLog(LOG_LEVEL::DEBUG, "VirtualProtect failed. The error code was 0x%08X", GetLastError());
	}
}

VMAIter VMManager::CheckConflictingVMA(VAddr addr, size_t Size, MemoryRegionType Type, bool* bOverflow)
{
	*bOverflow = false;
	VMAIter it = GetVMAIterator(addr, Type);

	if (it == m_MemoryRegionArray[Type].RegionMap.end()) {
		// Pretend we are overflowing since an overflow is always an error. Otherwise, end() will be interpreted
		// as a found free VMA.

		*bOverflow = true;
		return it;
	}

	if (Size != 0 && (it->first + it->second.size - 1 < addr + Size - 1)) {
		*bOverflow = true;
	}

	if (it->second.type != FreeVma) {
		return it; // conflict
	}

	return m_MemoryRegionArray[Type].RegionMap.end(); // no conflict
}

void VMManager::ConstructVMA(VAddr Start, size_t Size, MemoryRegionType Type, VMAType VmaType, DWORD Perms)
{
	VMAIter it_begin = m_MemoryRegionArray[Type].RegionMap.begin();
	VMAIter it_end = m_MemoryRegionArray[Type].RegionMap.end();
	VMAIter it;

	VMAIter vma_handle = CarveVMA(Start, Size, Type);
	VirtualMemoryArea& vma = vma_handle->second;
	vma.type = VmaType;
	vma.permissions = Perms;

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

		EmuLog(LOG_LEVEL::WARNING, "Can't find any more free space in the memory region %d! Virtual memory exhausted?", Type);
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

	EmuLog(LOG_LEVEL::WARNING, "Can't find any more free space in the memory region %d! Virtual memory exhausted?", Type);

	m_MemoryRegionArray[Type].LastFree = m_MemoryRegionArray[Type].RegionMap.end();

	return;
}

void VMManager::DestructVMA(VAddr addr, MemoryRegionType Type, size_t Size)
{
	BOOL ret;
	VMAIter it = GetVMAIterator(addr, Type); // the caller should already guarantee that the vma exists

	// Don't free our memory placeholder and allocations on the contiguous region since they don't use VirtualAlloc and MapViewOfFileEx

	if ((addr >= XBE_MAX_VA) && (Type != ContiguousRegion))
	{
		if (Type == SystemRegion || Type == DevkitRegion)
		{
			// This memory was reserved by the loader, do not release it, only decommit

			ret = VirtualFree((void*)addr, Size, MEM_DECOMMIT);
		}
		else
		{
			// NOTE: unlike NtFreeVirtualMemory, VirtualFree cannot release a committed region only partially, the size must always be 0
			// with MEM_RELEASE. This is a problem because some games can call us from XbFreeVirtualMemory with XBOX_MEM_RELEASE and a
			// size != 0, so, in this case, we can only decommit the region to avoid memory leaks. This essentially means that we will
			// leave behind reserved areas, which will decrease the total amount of host virtual space available...

			if (addr == it->first && Size == it->second.size) { ret = VirtualFree((void*)addr, 0, MEM_RELEASE); }
			else { ret = VirtualFree((void*)addr, Size, MEM_DECOMMIT); }
		}

		if (!ret)
		{
			EmuLog(LOG_LEVEL::DEBUG, "Deallocation routine failed with error 0x%08X", GetLastError());
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
		EmuLog(LOG_LEVEL::DEBUG, "std::prev(CarvedVmaIt) was not free");

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

		EmuLog(LOG_LEVEL::WARNING, "Can't find any more free space in the memory region %d! Virtual memory exhausted?", Type);

		m_MemoryRegionArray[Type].LastFree = m_MemoryRegionArray[Type].RegionMap.end();

		return;
	}
}
