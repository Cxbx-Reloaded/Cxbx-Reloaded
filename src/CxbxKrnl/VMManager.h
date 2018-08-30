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
// *   Cxbx->Win32->CxbxKrnl->VMManager.h
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

#ifndef VMMANAGER_H
#define VMMANAGER_H


#include "PhysicalMemory.h"


/* VMATypes */
typedef enum _VMAType
{
	// vma represents an unmapped region of the address space
	FreeVma,
	// memory reserved by XbAllocateVirtualMemory or by MmMapIoSpace
	ReservedVma,
	// vma represents allocated memory
	AllocatedVma,
}VMAType;


/* VirtualMemoryArea struct */
struct VirtualMemoryArea
{
	// vma starting address
	VAddr base = 0;
	// vma size
	size_t size = 0;
	// vma kind of memory
	VMAType type = FreeVma;
	// initial vma permissions of the allocation, only used by XbVirtualMemoryStatistics
	DWORD permissions = XBOX_PAGE_NOACCESS;
	// this allocation was served by VirtualAlloc
	bool bFragmented = false;
	// tests if this area can be merged to the right with 'next'
	bool CanBeMergedWith(const VirtualMemoryArea& next) const;
};


typedef std::map<VAddr, VirtualMemoryArea>::iterator VMAIter;


/* struct representing a particular memory region of interest. Used to track and speed up searches of free areas */
typedef struct _MemoryRegion
{
	VMAIter LastFree;
	std::map<VAddr, VirtualMemoryArea> RegionMap;
}MemoryRegion, *PMemoryRegion;


/* enum describing the memory regions available for allocations on the Xbox */
typedef enum _MemoryRegionType
{
	UserRegion,
	SystemRegion,
	DevkitRegion,
	ContiguousRegion,
	COUNTRegion,
}MemoryRegionType;


/* VMManager class */
class VMManager : public PhysicalMemory
{
	public:
		// constructor
		VMManager() {};
		// destructor
		~VMManager()
		{
			// DestroyMemoryRegions is not called when emulation ends, but only when the GUI process ends. This is probably because the emu
			// process is killed with TerminateProcess and so it doesn't have a chance to perform a cleanup...
			//DestroyMemoryRegions();
			DeleteCriticalSection(&m_CriticalSection);
			FlushViewOfFile((void*)CONTIGUOUS_MEMORY_BASE, CHIHIRO_MEMORY_SIZE);
			FlushViewOfFile((void*)PAGE_TABLES_BASE, PAGE_TABLES_SIZE);
			FlushFileBuffers(m_hContiguousFile);
			FlushFileBuffers(m_hPTFile);
			UnmapViewOfFile((void *)CONTIGUOUS_MEMORY_BASE);
			UnmapViewOfFile((void *)PAGE_TABLES_BASE);
			UnmapViewOfFile((void*)XBOX_WRITE_COMBINED_BASE);
			VirtualFree((void*)PAGE_TABLES_BASE, 0, MEM_RELEASE);
			CloseHandle(m_hContiguousFile);
			CloseHandle(m_hPTFile);
		}
		// initializes the memory manager to the default configuration
		void Initialize(HANDLE memory_view, HANDLE pagetables_view, int BootFlags);
		// retrieves memory statistics
		void MemoryStatistics(xboxkrnl::PMM_STATISTICS memory_statistics);
		// allocates memory in the user region
		VAddr Allocate(size_t Size);
		// allocates memory in the user region and zeros it
		VAddr AllocateZeroed(size_t size);
		// allocates memory in the system region
		VAddr AllocateSystemMemory(PageType BusyType, DWORD Perms, size_t Size, bool bAddGuardPage);
		// allocates memory for the system file cache
		xboxkrnl::NTSTATUS AllocateFileCacheMemory(PFN_COUNT NumberOfCachePages);
		// allocates memory in the contiguous region
		VAddr AllocateContiguous(size_t Size, PAddr LowestAddress, PAddr HighestAddress, ULONG Alignment, DWORD Perms);
		// maps device memory in the system region
		VAddr MapDeviceMemory(PAddr Paddr, size_t Size, DWORD Perms);
		// deallocates memory in the system region
		PFN_COUNT DeallocateSystemMemory(PageType BusyType, VAddr addr, size_t Size);
		// deallocates memory for the system file cache
		xboxkrnl::NTSTATUS DeallocateFileCacheMemory(PFN_COUNT NumberOfCachePages);
		// deallocates memory in the contiguous region
		void DeallocateContiguous(VAddr addr);
		// unmaps device memory in the system region
		void UnmapDeviceMemory(VAddr addr, size_t Size);
		// deallocates memory in the user region
		void Deallocate(VAddr addr);
		// changes the protections of a memory region
		void Protect(VAddr addr, size_t Size, DWORD NewPerms);
		// checks if a VAddr is valid
		bool IsValidVirtualAddress(const VAddr addr);
		// translates a VAddr to its corresponding PAddr if valid
		PAddr TranslateVAddrToPAddr(const VAddr addr);
		// retrieves the protection status of an address
		DWORD QueryProtection(VAddr addr);
		// retrieves the size of an allocation
		size_t QuerySize(VAddr addr, bool bCxbxCaller = true);
		// locks physical pages in memory (prevents relocations)
		void LockBufferOrSinglePage(PAddr paddr, VAddr addr, size_t Size, bool bUnLock);
		// MmClaimGpuInstanceMemory implementation
		VAddr ClaimGpuMemory(size_t Size, size_t* BytesToSkip);
		// make contiguous memory persist across a quick reboot
		void PersistMemory(VAddr addr, size_t Size, bool bPersist);
		// debug function to reset a pte or check if it is writable
		VAddr DbgTestPte(VAddr addr, PMMPTE Pte, bool bWriteCheck);
		// retrieves the number of free debugger pages
		PFN_COUNT QueryNumberOfFreeDebuggerPages();
		// xbox implementation of NtAllocateVirtualMemory
		xboxkrnl::NTSTATUS XbAllocateVirtualMemory(VAddr* addr, ULONG ZeroBits, size_t* Size, DWORD AllocationType, DWORD Protect);
		// xbox implementation of NtFreeVirtualMemory
		xboxkrnl::NTSTATUS XbFreeVirtualMemory(VAddr* addr, size_t* Size, DWORD FreeType);
		// xbox implementation of NtProtectVirtualMemory
		xboxkrnl::NTSTATUS XbVirtualProtect(VAddr* addr, size_t* Size, DWORD* Protect);
		// xbox implementation of NtQueryVirtualMemory
		xboxkrnl::NTSTATUS XbVirtualMemoryStatistics(VAddr addr, xboxkrnl::PMEMORY_BASIC_INFORMATION memory_statistics);
		// acquires the critical section
		void Lock();
		// releases the critical section
		void Unlock();

	
	private:
		// typedef of pointer to a member function mapping a memory block
		typedef VAddr (VMManager::*MappingFn) (VAddr, size_t, size_t, DWORD, PFN);
		// an array of structs used to track the free/allocated vma's in the various memory regions
		MemoryRegion m_MemoryRegionArray[COUNTRegion];
		// handle of the contiguous file mapping
		HANDLE m_hContiguousFile = NULL;
		// handle of the PT file mapping
		HANDLE m_hPTFile = NULL;
		// critical section lock to synchronize accesses
		CRITICAL_SECTION m_CriticalSection;
		// the allocation granularity of the host. Needed by MapViewOfFileEx and VirtualAlloc
		DWORD m_AllocationGranularity = 0;
		// number of bytes reserved with XBOX_MEM_RESERVE by XbAllocateVirtualMemory
		size_t m_VirtualMemoryBytesReserved = 0;

	
		// set up the pfn database
		void InitializePfnDatabase();
		// set up the pfn database after a quick reboot
		void ReinitializePfnDatabase();
		// initializes a memory region struct
		void ConstructMemoryRegion(VAddr Start, size_t Size, MemoryRegionType Type);
		// clears all memory region structs
		void DestroyMemoryRegions();
		// map a memory block with the supplied allocation routine
		VAddr MapMemoryBlock(MappingFn MappingRoutine, MemoryRegionType Type, PFN_COUNT PteNumber, PFN pfn, VAddr HighestAddress = 0);
		// helper function which maps a block with VirtualAlloc
		VAddr MapBlockWithVirtualAlloc(VAddr StartingAddr, size_t Size, size_t VmaEnd, DWORD Unused, PFN Unused2);
		// helper function which reserves a block of virtual memory with VirtualAlloc
		VAddr ReserveBlockWithVirtualAlloc(VAddr StartingAddr, size_t Size, size_t VmaEnd, DWORD Unused, PFN Unused2);
		// helper function which maps a block with MapViewOfFileEx
		VAddr MapBlockWithMapViewOfFileEx(VAddr StartingAddr, size_t ViewSize, size_t VmaEnd, DWORD OffsetLow, PFN pfn);
		// constructs a vma
		void ConstructVMA(VAddr Start, size_t Size, MemoryRegionType Type, VMAType VmaType, bool bFragFlag, DWORD Perms = XBOX_PAGE_NOACCESS);
		// destructs a vma
		void DestructVMA(VAddr addr, MemoryRegionType Type, size_t Size);
		// removes a vma block from the mapped memory
		VMAIter UnmapVMA(VMAIter vma_handle, MemoryRegionType Type);
		// carves a vma of a specific size at the specified address by splitting free vma's
		VMAIter CarveVMA(VAddr base, size_t size, MemoryRegionType Type);
		// splits the edges of the given range of non-free vma's so that there is a vma split at each end of the range
		VMAIter CarveVMARange(VAddr base, size_t size, MemoryRegionType Type);
		// gets the iterator of a vma in the specified memory region
		VMAIter GetVMAIterator(VAddr target, MemoryRegionType Type);
		// splits a parent vma into two children
		VMAIter SplitVMA(VMAIter vma_handle, u32 offset_in_vma, MemoryRegionType Type);
		// merges the specified vma with adjacent ones if possible
		VMAIter MergeAdjacentVMA(VMAIter vma_handle, MemoryRegionType Type);
		// checks if the specified range conflicts with another non-free vma
		VMAIter CheckConflictingVMA(VAddr addr, size_t Size, MemoryRegionType Type, bool* bOverflow);
		// changes the access permissions of a block of memory
		void UpdateMemoryPermissions(VAddr addr, size_t Size, DWORD Perms);
		// restores persistent memory
		void RestorePersistentMemory();
		// restores a persistent allocation
		void RestorePersistentAllocation(VAddr addr, PFN StartingPfn, PFN EndingPfn, PageType Type);
};


extern VMManager g_VMManager;

#endif
