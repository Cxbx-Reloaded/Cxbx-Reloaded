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

#ifndef PHYSICAL_MEMORY_H
#define PHYSICAL_MEMORY_H


#include <core\kernel\exports\xboxkrnl.h>
#include "core\kernel\support\Emu.h"
#include "core\kernel\init\CxbxKrnl.h"
#include <windows.h>
#include <map>


/* Global typedefs */
typedef xbox::ulong_ptr_xt VAddr;
typedef xbox::ulong_ptr_xt PAddr;
typedef uint32_t u32;


/* An entry of the list tracking the free pages on the system */
typedef struct _FreeBlock
{
	xbox::PFN start;                        // starting page of the block
	xbox::PFN_COUNT size;                   // number of pages in the block
	xbox::LIST_ENTRY ListEntry;
}FreeBlock, *PFreeBlock;


/* PFN entry used by the memory manager */
typedef union _XBOX_PFN
{
	xbox::ulong_xt Default;
	struct {
		xbox::ulong_xt LockCount : 16;  // Set to prevent page relocation. Used by MmLockUnlockPhysicalPage and others
		xbox::ulong_xt Busy : 1;        // If set, PFN is in use
		xbox::ulong_xt Unused : 1;
		xbox::ulong_xt PteIndex : 10;   // Offset in the PT that maps the pte (it seems to be needed only for page relocations)
		xbox::ulong_xt BusyType : 4;    // What the page is used for
	} Busy;
	struct {
		xbox::ulong_xt LockCount : 16;  // Set to prevent page relocation. Used by MmLockUnlockPhysicalPage and others
		xbox::ulong_xt Busy : 1;        // If set, PFN is in use
		xbox::ulong_xt PtesUsed : 11;   // Number of used pte's in the PT pointed by the pde
		xbox::ulong_xt BusyType : 4;    // What the page is used for (must be VirtualPageTableType or SystemPageTableType)
	} PTPageFrame;
}XBOX_PFN, *PXBOX_PFN;


/* enum describing the memory layouts the memory manager can use */
typedef enum _MmLayout
{
	MmChihiro = 1,
	MmDebug,
	MmRetail,
}MmLayout;


/* Lock count variables for the PFN database */
#define LOCK_COUNT_UNIT             2
#define LOCK_COUNT_MAXIMUM          0xFFFE


/* PTE protection masks */
#define PTE_VALID_MASK              0x00000001
#define PTE_WRITE_MASK              0x00000002
#define PTE_OWNER_MASK              0x00000004
#define PTE_WRITE_THROUGH_MASK      0x00000008
#define PTE_CACHE_DISABLE_MASK      0x00000010
#define PTE_ACCESS_MASK             0x00000020
#define PTE_DIRTY_MASK              0x00000040
#define PTE_LARGE_PAGE_MASK         0x00000080
#define PTE_GLOBAL_MASK             0x00000100
#define PTE_GUARD_END_MASK          0x00000200
#define PTE_PERSIST_MASK            0x00000400
#define PTE_NOACCESS                0x00000000
#define PTE_READONLY                0x00000000
#define PTE_READWRITE               PTE_WRITE_MASK
#define PTE_NOCACHE                 PTE_CACHE_DISABLE_MASK
#define PTE_GUARD                   PTE_GUARD_END_MASK
#define PTE_CACHE                   0x00000000
#define PTE_VALID_PROTECTION_MASK   0x0000021B // valid, write, write-through, no cache, guard/end
#define PTE_SYSTEM_PROTECTION_MASK  0x0000001B // valid, write, write-through, no cache


/* Various macros to manipulate PDE/PTE/PFN */
#define GetPdeAddress(Va) ((xbox::PMMPTE)(((((ULONG)(Va)) >> 22) << 2) + PAGE_DIRECTORY_BASE)) // (Va/4M) * 4 + PDE_BASE
#define GetPteAddress(Va) ((xbox::PMMPTE)(((((ULONG)(Va)) >> 12) << 2) + PAGE_TABLES_BASE))    // (Va/4K) * 4 + PTE_BASE
#define GetVAddrMappedByPte(Pte) ((VAddr)((ULONG_PTR)(Pte) << 10))
#define GetPteOffset(Va) ((((ULONG)(Va)) << 10) >> 22)
#define IsPteOnPdeBoundary(Pte) (((ULONG_PTR)(Pte) & (PAGE_SIZE - 1)) == 0)
#define WRITE_ZERO_PTE(pPte) ((pPte)->Default = 0)
#define WRITE_PTE(pPte, Pte) (*(pPte) = Pte)
#define DISABLE_CACHING(Pte) ((Pte).Hardware.CacheDisable = 1); ((Pte).Hardware.WriteThrough = 1)
#define SET_WRITE_COMBINE(Pte) ((Pte).Hardware.CacheDisable = 0); ((Pte).Hardware.WriteThrough = 1)
#define ValidKernelPteBits (PTE_VALID_MASK | PTE_WRITE_MASK | PTE_DIRTY_MASK | PTE_ACCESS_MASK) // 0x63
#define ValidKernelPdeBits (PTE_VALID_MASK | PTE_WRITE_MASK | PTE_OWNER_MASK | PTE_DIRTY_MASK | PTE_ACCESS_MASK) // 0x67
#define CONVERT_PFN_TO_CONTIGUOUS_PHYSICAL(Pfn) ((PCHAR)PHYSICAL_MAP_BASE + ((Pfn) << PAGE_SHIFT))
#define CONVERT_CONTIGUOUS_PHYSICAL_TO_PFN(Va) (((Va) & (BYTES_IN_PHYSICAL_MAP - 1)) >> PAGE_SHIFT)
#define XBOX_PFN_ELEMENT(pfn) (&((PXBOX_PFN)XBOX_PFN_ADDRESS)[pfn])
#define CHIHIRO_PFN_ELEMENT(pfn) (&((PXBOX_PFN)CHIHIRO_PFN_ADDRESS)[pfn])


/* These macros check if the supplied address is inside a known range */
#define IS_PHYSICAL_ADDRESS(Va) (((VAddr)(Va) - PHYSICAL_MAP_BASE) <= (PHYSICAL_MAP_END - PHYSICAL_MAP_BASE))
#define IS_SYSTEM_ADDRESS(Va) (((VAddr)(Va) - SYSTEM_MEMORY_BASE) <= (SYSTEM_MEMORY_END - SYSTEM_MEMORY_BASE))
#define IS_DEVKIT_ADDRESS(Va) (((VAddr)(Va) - DEVKIT_MEMORY_BASE) <= (DEVKIT_MEMORY_END - DEVKIT_MEMORY_BASE))
#define IS_USER_ADDRESS(Va) (((VAddr)(Va) - LOWEST_USER_ADDRESS) <= (HIGHEST_USER_ADDRESS - LOWEST_USER_ADDRESS))


/* PhysicalMemory class */
class PhysicalMemory
{
	protected:
		// doubly linked list tracking the free physical pages
		xbox::LIST_ENTRY FreeList = { &FreeList , &FreeList };
		// highest pfn available for contiguous allocations
		PAddr m_MaxContiguousPfn = XBOX_CONTIGUOUS_MEMORY_LIMIT;
		// amount of free physical pages available for non-debugger usage
		xbox::PFN_COUNT m_PhysicalPagesAvailable = X64M_PHYSICAL_PAGE;
		// amount of free physical pages available for usage by the debugger (devkit only)
		xbox::PFN_COUNT m_DebuggerPagesAvailable = 0;
		// array containing the number of pages in use per type
		xbox::PFN_COUNT m_PagesByUsage[xbox::COUNTtype] = { 0 };
		// highest page on the system
		xbox::PFN m_HighestPage = XBOX_HIGHEST_PHYSICAL_PAGE;
		// first page of the nv2a instance memory
		xbox::PFN m_NV2AInstancePage = XBOX_INSTANCE_PHYSICAL_PAGE;
		// number of allocated bytes for the nv2a instance memory
		size_t m_NV2AInstanceMemoryBytes = NV2A_INSTANCE_PAGE_COUNT << PAGE_SHIFT;
		// boolean that indicates that the extra 64 MiB on a devkit can be used for heap/Nt allocations
		bool m_bAllowNonDebuggerOnTop64MiB = true;
		// the memory layout that the VMManager is emulating
		bool m_MmLayoutChihiro = false;
		bool m_MmLayoutDebug = false;
		bool m_MmLayoutRetail = false;

	
		// protected constructor so PhysicalMemory can only be inherited from
		PhysicalMemory() {};
		// destructor
		~PhysicalMemory() {};
		// set up the page directory
		void InitializePageDirectory();
		// write a contiguous range of pfn's
		void WritePfn(xbox::PFN pfn_start, xbox::PFN pfn_end, xbox::PMMPTE pPte, xbox::PageType BusyType, bool bZero = false);
		// write a contiguous range of pte's
		void WritePte(xbox::PMMPTE pPteStart, xbox::PMMPTE pPteEnd, xbox::MMPTE Pte, xbox::PFN pfn, bool bZero = false);
		// retrieves the pfn entry which maps a PT
		PXBOX_PFN GetPfnOfPT(xbox::PMMPTE pPte);
		// commit a contiguous number of pages
		bool RemoveFree(xbox::PFN_COUNT NumberOfPages, xbox::PFN* result, xbox::PFN_COUNT PfnAlignment, xbox::PFN start, xbox::PFN end);
		// release a contiguous number of pages
		void InsertFree(xbox::PFN start, xbox::PFN end);
		// convert from Xbox to the desired system pte protection (if possible) and return it
		bool ConvertXboxToSystemPtePermissions(DWORD perms, xbox::PMMPTE pPte);
		// convert from Xbox to non-system pte protection (if possible) and return it
		bool ConvertXboxToPtePermissions(DWORD perms, xbox::PMMPTE pPte);
		// convert from pte permissions to the corresponding Xbox protection code
		DWORD ConvertPteToXboxPermissions(ULONG PteMask);
		// convert from Xbox to Windows permissions
		DWORD ConvertXboxToWinPermissions(DWORD Perms);
		// add execute rights if the permission mask doesn't include it
		DWORD PatchXboxPermissions(DWORD Perms);
		// commit page tables (if necessary)
		bool AllocatePT(size_t Size, VAddr addr);
		// deallocate page tables (if possible)
		void DeallocatePT(size_t Size, VAddr addr);
		// checks if enough free pages are available for the allocation (doesn't account for fragmentation)
		bool IsMappable(xbox::PFN_COUNT PagesRequested, bool bRetailRegion, bool bDebugRegion);
};

#endif
