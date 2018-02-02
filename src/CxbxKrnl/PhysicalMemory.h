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
// *   Cxbx->Win32->CxbxKrnl->PhysicalMemory.h
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

#ifndef PHYSICAL_MEMORY_H
#define PHYSICAL_MEMORY_H

#define _XBOXKRNL_DEFEXTRN_

namespace xboxkrnl
{
	#include <xboxkrnl/xboxkrnl.h>
};

#include "Emu.h"
#include "CxbxKrnl.h"
#include <windows.h>
#include <map>


/* PhysicalMemory class error codes */
#define PMEMORY_STATUS 					unsigned int
#define PMEMORY_SUCCESS					0x0
#define PMEMORY_INSUFFICIENT_MEMORY		0x1
#define PMEMORY_ALLOCATE_FRAGMENTED		0x2


/* Global typedefs */
typedef uintptr_t VAddr;
typedef uintptr_t PAddr;
typedef uint32_t u32;
typedef uint16_t u16; // Used by PageType
typedef DWORD PTEflags;
typedef ULONG PFN_Number;


/* The Xbox PTE, modelled around the Intel 386 PTE specification */
typedef struct _XBOX_PTE
{
	ULONG Valid : 1;
	ULONG Write : 1;
	ULONG Owner : 1;
	ULONG WriteThrough : 1;
	ULONG CacheDisable : 1;
	ULONG Accessed : 1;
	ULONG Dirty : 1;
	ULONG LargePage : 1;
	ULONG Global : 1;
	ULONG GuardOrEnd : 1; // software field used for our own purposes
	ULONG Persist : 1;    // software field used for our own purposes
	ULONG Unused : 1;     // software field used for our own purposes
	ULONG PFN : 20;
} XBOX_PTE, *PXBOX_PTE;


/* PTE as used by the memory manager */
typedef struct _MMPTE
{
	union
	{
		ULONG Default;
		XBOX_PTE Hardware;
		struct {
			ULONG Valid : 1;
			ULONG OneEntry : 1;
			ULONG NextEntry : 30;
		} List;
	};
} MMPTE, *PMMPTE;


/* enum describing the usage type of the memory pages */
enum PageType : u16 {
	Unknown,                   // Used by the PFN database
	Stack,				       // Used by MmCreateKernelStack / VMManager::AllocateStack
	VirtualPageTable,
	SystemPageTable,           // Not used yet
	Pool,                      // Used by ExAllocatePoolWithTag
	VirtualMemory,
	SystemMemory,              // Used by MmAllocateSystemMemory
	Image,                     // Used by XeLoadSection
	Cache,                     // Not used yet
	Contiguous,                // Used by MmAllocateContiguousMemoryEx
	Debugger,                  // xbdm-related
	COUNT
};


typedef struct _MMPFNFREE {
	USHORT PfnFlink;        // contains the pfn of the next element in the list
	USHORT PfnBlink;        // contains the pfn of the previous element in the list
} MMPFNFREE, *PMMPFNFREE;


/* PFN entry used by the memory manager */
typedef struct _XBOX_PFN {
	union
	{
		ULONG Default;
		MMPTE Pte;
		MMPFNFREE Free;
		struct {
			ULONG LockCount : 16;  // Set to prevent page relocation. Used by MmLockUnlockPhysicalPage and others
			ULONG Busy : 1;        // If set, PFN is in use
			ULONG Reserved : 1;
			ULONG PteIndex : 10;
			ULONG BusyType : 4;
		} Busy;
		// I'm still unsure about what the others below are used for...
		/*struct {
			ULONG LockCount : 16;
			ULONG Busy : 1;
			ULONG ElementIndex : 11;
			ULONG BusyType : 4;
		} FsCache;
		struct {
			ULONG LockCount : 16;
			ULONG Busy : 1;
			ULONG NumberOfUsedPtes : 11;
			ULONG BusyType : 4;
		} Directory;*/
	};
} XBOX_PFN, *PXBOX_PFN;


/* Lock count variables for the PFN database */
#define LOCK_COUNT_UNIT         2
#define LOCK_COUNT_MAXIMUM      0xFFFE


/* PTE protection masks */
//#define PTE_READWRITE            MM_PTE_WRITE_MASK
#define PTE_VALID_MASK           0x00000001
#define PTE_WRITE_MASK           0x00000002
//#define PTE_OWNER_MASK           0x00000004
//#define PTE_WRITE_THROUGH_MASK   0x00000008
//#define PTE_CACHE_DISABLE_MASK   0x00000010
#define PTE_ACCESS_MASK          0x00000020
#define PTE_DIRTY_MASK           0x00000040
//#define PTE_LARGE_PAGE_MASK      0x00000080
//#define PTE_GLOBAL_MASK          0x00000100
//#define PTE_GUARD_MASK           0x00000200
//#define PTE_NEXT_ENTRY_MASK      0xFFFFFFFC

//#define PTE_NOACCESS             0x000
//#define PTE_READONLY             0x000
//#define PTE_READWRITE            MM_PTE_WRITE_MASK
//#define PTE_NOCACHE              MM_PTE_CACHE_DISABLE_MASK
//#define PTE_GUARD                MM_PTE_GUARD_MASK
//#define PTE_CACHE                0x000


/* Various macros to manipulate PDE/PTE/PFN */
#define GetPdeAddress(Va) (PMMPTE)(((Va >> 22) << 2) + PDE_BASE)
#define WRITE_ZERO_PTE(pPte) ((pPte)->Default = 0)
#define WRITE_PTE(pPte, Pte) (*(pPte) = Pte)
// On real hardware, enabling only the cache disable bit would result in an effective caching type of USWC
// (uncacheable speculative write combining), so we set both to achieve it
#define DISABLE_CACHING(Pte) ((Pte).Hardware.CacheDisable = 1); ((Pte).Hardware.WriteThrough = 1)
#define ValidKernelPteBits (PTE_VALID_MASK | PTE_WRITE_MASK | PTE_DIRTY_MASK | PTE_ACCESS_MASK) // 0x63
// This returns the VAddr in the contiguous region
#define CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(Pfn) (SYSTEM_PHYSICAL_MAP + (Pfn << PAGE_SHIFT))
// This works with both PAddr and VAddr in the contiguous region
#define CONVERT_CONTIGUOUS_PHYSICAL_TO_PFN(Va) ((Va & (BYTES_IN_PHYSICAL_MAP - 1)) >> PAGE_SHIFT)


/* Global helper function used to copy an ULONG block of memory to another buffer */
void FillMemoryUlong(void* Destination, size_t Length, ULONG Long);


/* PhysicalMemory class */
class PhysicalMemory
{
	protected:
		// if the block to allocate is smaller than AllocationThreshold, then it will be mapped starting from the bottom of the memory,
		// otherwise it's mapped from the top. AllocationThreshold is 64KiB, the allocation granularity of the Xbox
		const unsigned int m_AllocationThreshold = 1024 * 64;
		// amount of physical memory in use
		size_t m_PhysicalMemoryInUse = 0;
		// max physical memory available on the Xbox/Chihiro
		size_t m_MaxPhysicalMemory = XBOX_MEMORY_SIZE;
		// map tracking the physical memory currently in use
		std::map<PAddr, size_t> m_Mem_map;
		// map tracking the blocks allocated with VirtualAlloc
		std::map<VAddr, size_t> m_Fragmented_mem_map;
		// current error status code of the PhysicalMemory class
		PMEMORY_STATUS m_Status = PMEMORY_SUCCESS;
		// highest address available for contiguous allocations
		PAddr m_MaxContiguousAddress = XBOX_CONTIGUOUS_MEMORY_LIMIT;
		// VAddr of the PFN Database
		VAddr m_PfnDatabaseVAddress = XBOX_PFN_DATABASE_PHYSICAL_PAGE + SYSTEM_PHYSICAL_MAP;
		// the size of memory occupied by the PFN/NV2A instance memory
		size_t m_UpperPMemorySize = 32 * PAGE_SIZE;
		// max physical memory available on the Xbox/Chihiro
		size_t m_MaxPhysicalMemory = XBOX_MEMORY_SIZE;
	
		// protected constructor so PhysicalMemory can only be inherited from
		PhysicalMemory() {};
		// destructor
		~PhysicalMemory()
		{
			for (auto it = m_Fragmented_mem_map.begin(); it != m_Fragmented_mem_map.end(); ++it)
			{
				VirtualFree((void*)it->first, 0, MEM_RELEASE);
			}
		}
		// set up the pfndatabase
		void InitializePfnDatabase();
		// set up the pfn database after a quick reboot (a new xbe is launched)
		void ReinitializePfnDatabase();
		// inserts a range of pages in the manager free list
		void InsertPageRangeInFreeList(PFN_Number Pfn, PFN_Number PfnEndExcluded);
		// inserts a single page in the manager free list
		void InsertPageInFreeList(PFN_Number Pfn, bool InsertAtHead);
		// allocates a block of the mapped file, returns m_MaxContiguousAddress and sets an error code if unsuccessful
		PAddr AllocatePhysicalMemory(size_t size);
		// allocates a block of the mapped file between the specified range if possible
		PAddr AllocatePhysicalMemoryRange(size_t size, PAddr low_addr, PAddr high_addr);
		// allocates a block of memory with VirtualAlloc when the main memory is fragmented and sets an error code
		VAddr AllocateFragmented(size_t size);
		// shrinks the size af an allocation
		void ShrinkPhysicalAllocation(PAddr addr, size_t offset, bool bFragmentedMap, bool bStart);
		// deallocates a block of the mapped file
		void DeAllocatePhysicalMemory(PAddr addr);
		// deallocates a block allocated with VirtualAlloc
		void DeAllocateFragmented(VAddr addr);
		// retrieves the current error code of the PhysicalMemory class
		PMEMORY_STATUS GetError() const;
		// sets the error code of the PhysicalMemory class
		void SetError(PMEMORY_STATUS err);
		// clears the error code of the PhysicalMemory class
		void ClearError();
};

#endif
