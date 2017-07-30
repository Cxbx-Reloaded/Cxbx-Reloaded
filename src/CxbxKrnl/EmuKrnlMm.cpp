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
// *   Cxbx->Win32->CxbxKrnl->EmuKrnlMm.cpp
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
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *  (c) 2016 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

// prevent name collisions
namespace xboxkrnl
{
#include <xboxkrnl/xboxkrnl.h> // For MmGlobalData, etc.
};

#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnl.h" // For DefaultLaunchDataPage
#include "EmuKrnlLogging.h"
#include "CxbxKrnl.h" // For CxbxKrnlCleanup
#include "Emu.h" // For EmuWarning()
#include "EmuAlloc.h" // For CxbxFree(), g_MemoryManager.Allocate(), etc.
#include "ResourceTracker.h" // For g_AlignCache
#include "MemoryManager.h"

// prevent name collisions
namespace NtDll
{
#include "EmuNtDll.h" // For NtAllocateVirtualMemory(), etc.
};

// ******************************************************************
// * 0x0066 - MmGlobalData
// ******************************************************************
XBSYSAPI EXPORTNUM(102) xboxkrnl::PVOID xboxkrnl::MmGlobalData[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

// ******************************************************************
// * 0x00A4 - LaunchDataPage
// ******************************************************************
// Note : Originally, LaunchDataPage resides in a "STICKY" segment in
// the xbox kernel. Kernel code accessses this as a normal variable.
// XAPI code however, reference to the address of this kernel variable,
// thus use indirection (*LaunchDataPage) to get to the same contents.
XBSYSAPI EXPORTNUM(164) xboxkrnl::PLAUNCH_DATA_PAGE xboxkrnl::LaunchDataPage = NULL;

// ******************************************************************
// * 0x00A5 - MmAllocateContiguousMemory()
// ******************************************************************
// Allocates a range of physically contiguous, cache-aligned memory from the
// non-paged pool (= main pool on XBOX).
//
// Differences from NT: HighestAcceptableAddress was deleted, opting instead
//     to not care about the highest address.
XBSYSAPI EXPORTNUM(165) xboxkrnl::PVOID NTAPI xboxkrnl::MmAllocateContiguousMemory
(
	IN ULONG NumberOfBytes
)
{
	LOG_FORWARD("MmAllocateContiguousMemoryEx");

	return MmAllocateContiguousMemoryEx(NumberOfBytes, 0, MAXULONG_PTR, 0, PAGE_READWRITE);
}

#define PAGE_KNOWN_FLAGS (PAGE_READONLY | PAGE_READWRITE | PAGE_NOCACHE | PAGE_WRITECOMBINE)

// ******************************************************************
// * 0x00A6 - MmAllocateContiguousMemoryEx()
// ******************************************************************
XBSYSAPI EXPORTNUM(166) xboxkrnl::PVOID NTAPI xboxkrnl::MmAllocateContiguousMemoryEx
(
	IN ULONG            NumberOfBytes,
	IN PHYSICAL_ADDRESS LowestAcceptableAddress,
	IN PHYSICAL_ADDRESS HighestAcceptableAddress,
	IN ULONG            Alignment OPTIONAL,
	IN ULONG            ProtectionType
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(NumberOfBytes)
		LOG_FUNC_ARG(LowestAcceptableAddress)
		LOG_FUNC_ARG(HighestAcceptableAddress)
		LOG_FUNC_ARG(Alignment)
		LOG_FUNC_ARG_TYPE(PROTECTION_TYPE, ProtectionType)
		LOG_FUNC_END;

	PVOID pRet = (PVOID)1; // Marker, never returned, overwritten with NULL on input error

	if (Alignment < PAGE_SIZE)
		Alignment = PAGE_SIZE; // page boundary at least

	// Only known flags are allowed
	if ((ProtectionType & ~PAGE_KNOWN_FLAGS) != 0)
		pRet = NULL;

	// Either PAGE_READONLY or PAGE_READWRITE must be set (not both, nor none)
	if (((ProtectionType & PAGE_READONLY) > 0) == ((ProtectionType & PAGE_READWRITE) > 0))
		pRet = NULL;

	// Combining PAGE_NOCACHE and PAGE_WRITECOMBINE isn't allowed
	if ((ProtectionType & (PAGE_NOCACHE | PAGE_WRITECOMBINE)) == (PAGE_NOCACHE | PAGE_WRITECOMBINE))
		pRet = NULL;

	// Allocate when input arguments are valid
	if (pRet != NULL)
	{
		// TODO : Allocate differently if(ProtectionType & PAGE_WRITECOMBINE)
		pRet = g_MemoryManager.AllocateContiguous(NumberOfBytes, Alignment);
	}

	RETURN(pRet);
}

// ******************************************************************
// * 0x00A7 - MmAllocateSystemMemory()
// ******************************************************************
XBSYSAPI EXPORTNUM(167) xboxkrnl::PVOID NTAPI xboxkrnl::MmAllocateSystemMemory
(
	ULONG NumberOfBytes,
	ULONG Protect
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(NumberOfBytes)
		LOG_FUNC_ARG(Protect)
		LOG_FUNC_END;

	// TODO: should this be aligned?
	PVOID pRet = g_MemoryManager.Allocate(NumberOfBytes);

	RETURN(pRet);
}

// ******************************************************************
// * 0x00A8 - MmClaimGpuInstanceMemory()
// ******************************************************************
XBSYSAPI EXPORTNUM(168) xboxkrnl::PVOID NTAPI xboxkrnl::MmClaimGpuInstanceMemory
(
	IN SIZE_T NumberOfBytes,
	OUT SIZE_T *NumberOfPaddingBytes
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(NumberOfBytes)
		LOG_FUNC_ARG_OUT(NumberOfPaddingBytes)
		LOG_FUNC_END;

	if (g_bIsChihiro)
		*NumberOfPaddingBytes = 0;
	else
		*NumberOfPaddingBytes = MI_CONVERT_PFN_TO_PHYSICAL(MM_64M_PHYSICAL_PAGE) -
		MI_CONVERT_PFN_TO_PHYSICAL(MM_INSTANCE_PHYSICAL_PAGE + MM_INSTANCE_PAGE_COUNT);

	DbgPrintf("MmClaimGpuInstanceMemory : *NumberOfPaddingBytes = 0x%08X\n", *NumberOfPaddingBytes);

#ifdef _DEBUG_TRACE
	if (NumberOfBytes != MAXULONG_PTR)
	{
		if (NumberOfBytes != 20480)
			LOG_IGNORED();
	}
#endif

	PVOID Result = (PUCHAR)MI_CONVERT_PFN_TO_PHYSICAL(MM_HIGHEST_PHYSICAL_PAGE + 1)
		- *NumberOfPaddingBytes;

	RETURN(Result);
}

// ******************************************************************
// * 0x00A9 - MmCreateKernelStack()
// ******************************************************************
// * Differences from NT: Custom stack size.
// ******************************************************************
XBSYSAPI EXPORTNUM(169) xboxkrnl::PVOID NTAPI xboxkrnl::MmCreateKernelStack
(
	IN ULONG	NumberOfBytes,
	IN BOOLEAN  DebuggerThread
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(NumberOfBytes)
		LOG_FUNC_ARG(DebuggerThread)
		LOG_FUNC_END;

	NtDll::PVOID BaseAddress = NULL;

	if (!NumberOfBytes) {
		// NumberOfBytes cannot be zero when passed to NtAllocateVirtualMemory() below
		CxbxKrnlCleanup("Assertion: 'NumberOfBytes != 0' in MmCreateKernelStack()");
	}

	if (NumberOfBytes & 0xFF) {
		// Validate NumberOfBytes for alignment with the page size
		CxbxKrnlCleanup("Assertion: '(NumberOfBytes & (PAGE_SIZE -1)) == 0' in MmCreateKernelStack()");
	}

	/**
	* Function at present does not:
	* - Create an additional guard PAGE_SIZE after allocation,
	* - Fill allocation with any values
	* - Treat DebuggerThread any differently
	*/

	NTSTATUS ret = NtDll::NtAllocateVirtualMemory(
		/*ProcessHandle=*/g_CurrentProcessHandle,
		/*BaseAddress=*/&BaseAddress,
		/*ZeroBits=*/0,
		/*RegionSize=*/&NumberOfBytes,
		/*AllocationType=*/MEM_COMMIT,
		/*Protect=*/PAGE_READWRITE);

	if (FAILED(ret))
		EmuWarning("MmCreateKernelStack failed!");
	else
		BaseAddress = (PVOID)((ULONG)BaseAddress + NumberOfBytes);

	RETURN(BaseAddress);
}

// ******************************************************************
// * 0x00AA - MmDeleteKernelStack()
// ******************************************************************
XBSYSAPI EXPORTNUM(170) xboxkrnl::VOID NTAPI xboxkrnl::MmDeleteKernelStack
(
	PVOID EndAddress,
	PVOID BaseAddress
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(EndAddress)
		LOG_FUNC_ARG(BaseAddress)
		LOG_FUNC_END;

	// TODO : Untested
	ULONG RegionSize = 0;
	NTSTATUS ret = NtDll::NtFreeVirtualMemory(
		/*ProcessHandle=*/g_CurrentProcessHandle,
		&BaseAddress,
		&RegionSize,
		/*FreeType=*/MEM_RELEASE);

	if (FAILED(ret))
		EmuWarning("MmDeleteKernelStack failed!");
}

// ******************************************************************
// * 0x00AB - MmFreeContiguousMemory()
// ******************************************************************
// Frees memory allocated with MmAllocateContiguousMemory.
//
// Differences from NT: None.
XBSYSAPI EXPORTNUM(171) xboxkrnl::VOID NTAPI xboxkrnl::MmFreeContiguousMemory
(
	IN PVOID BaseAddress
)
{
	LOG_FUNC_ONE_ARG(BaseAddress);

	if (BaseAddress == &DefaultLaunchDataPage) {
		DbgPrintf("Ignored MmFreeContiguousMemory(&DefaultLaunchDataPage)\n");
		LOG_IGNORED();
		return;
	}

	g_MemoryManager.Free(BaseAddress);

	// TODO -oDxbx: Sokoban crashes after this, at reset time (press Black + White to hit this).
	// Tracing in assembly shows the crash takes place quite a while further, so it's probably
	// not related to this call per-se. The strangest thing is, that if we let the debugger step
	// all the way through, the crash doesn't occur. Adding a Sleep(100) here doesn't help though.
}

// ******************************************************************
// * 0x00AC - MmFreeSystemMemory()
// ******************************************************************
XBSYSAPI EXPORTNUM(172) xboxkrnl::NTSTATUS NTAPI xboxkrnl::MmFreeSystemMemory
(
	PVOID BaseAddress,
	ULONG NumberOfBytes
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(BaseAddress)
		LOG_FUNC_ARG(NumberOfBytes)
		LOG_FUNC_END;

	g_MemoryManager.Free(BaseAddress);

	RETURN(STATUS_SUCCESS);
}

// ******************************************************************
// * 0x00AD - MmGetPhysicalAddress()
// ******************************************************************
// Translates a virtual address into a physical address.
//
// Differences from NT: PhysicalAddress is 32 bit, not 64.
XBSYSAPI EXPORTNUM(173) xboxkrnl::PHYSICAL_ADDRESS NTAPI xboxkrnl::MmGetPhysicalAddress
(
	IN PVOID   BaseAddress
)
{
	LOG_FUNC_ONE_ARG_OUT(BaseAddress);
	
	// this will crash if the memory pages weren't unlocked with
	// MmLockUnlockBufferPages, emulate this???
	LOG_INCOMPLETE();

	// We emulate Virtual/Physical memory 1:1	
	return (PHYSICAL_ADDRESS)BaseAddress;
}

// ******************************************************************
// * 0x00AE - MmIsAddressValid()
// ******************************************************************
XBSYSAPI EXPORTNUM(174) xboxkrnl::BOOLEAN NTAPI xboxkrnl::MmIsAddressValid
(
	IN PVOID   VirtualAddress
)
{
	LOG_FUNC_ONE_ARG_OUT(VirtualAddress);

	LOG_UNIMPLEMENTED();

	RETURN(TRUE);
}

// ******************************************************************
// * 0x00AF - MmLockUnlockBufferPages()
// ******************************************************************
XBSYSAPI EXPORTNUM(175) xboxkrnl::VOID NTAPI xboxkrnl::MmLockUnlockBufferPages
(
	IN PHYSICAL_ADDRESS	BaseAddress,
	IN ULONG			NumberOfBytes,
	IN ULONG			Protect
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(BaseAddress)
		LOG_FUNC_ARG(NumberOfBytes)
		LOG_FUNC_ARG(Protect)
		LOG_FUNC_END;

	LOG_IGNORED();
}

// ******************************************************************
// * 0x00B0 - MmLockUnlockPhysicalPage()
// ******************************************************************
XBSYSAPI EXPORTNUM(176) xboxkrnl::VOID NTAPI xboxkrnl::MmLockUnlockPhysicalPage
(
	IN ULONG_PTR PhysicalAddress,
	IN BOOLEAN UnlockPage
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(PhysicalAddress)
		LOG_FUNC_ARG(UnlockPage)
		LOG_FUNC_END;

	LOG_IGNORED();
}

// ******************************************************************
// * 0x00B1 - MmMapIoSpace()
// ******************************************************************
// Maps a physical address area into the virtual address space.
// DO NOT USE MEMORY MAPPED WITH THIS AS A BUFFER TO OTHER CALLS.  For
// example, don't WriteFile or NtWriteFile these buffers.  Copy them first.
//
// Differences from NT: PhysicalAddress is 32 bit, not 64.  ProtectionType
//     specifies the page protections, but it's a Win32 PAGE_ macro instead
//     of the normal NT enumeration.  PAGE_READWRITE is probably what you
//     want...
XBSYSAPI EXPORTNUM(177) xboxkrnl::PVOID NTAPI xboxkrnl::MmMapIoSpace
(
	IN PHYSICAL_ADDRESS PhysicalAddress,
	IN ULONG            NumberOfBytes,
	IN ULONG            ProtectionType
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(PhysicalAddress)
		LOG_FUNC_ARG(NumberOfBytes)
		LOG_FUNC_ARG(ProtectionType)
		LOG_FUNC_END;

	PVOID pRet;

	// Is it a physical address for hardware devices (flash, NV2A, etc) ?
	if (PhysicalAddress >= XBOX_WRITE_COMBINED_BASE) { // 0xF0000000
		// Return physical address as virtual (accesses will go through EmuException) :
		pRet = (PVOID)PhysicalAddress;
	}
	else {
		// TODO: should this be aligned?
		pRet = g_MemoryManager.Allocate(NumberOfBytes);
		LOG_INCOMPLETE();
	}

	RETURN(pRet);
}

// ******************************************************************
// * 0x00B2 - MmPersistContiguousMemory()
// ******************************************************************
XBSYSAPI EXPORTNUM(178) xboxkrnl::VOID NTAPI xboxkrnl::MmPersistContiguousMemory
(
	IN PVOID   BaseAddress,
	IN ULONG   NumberOfBytes,
	IN BOOLEAN Persist
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(BaseAddress)
		LOG_FUNC_ARG(NumberOfBytes)
		LOG_FUNC_ARG(Persist)
		LOG_FUNC_END;

	if (BaseAddress == LaunchDataPage)
	{
		if (Persist)
		{
			FILE* fp = fopen(szFilePath_LaunchDataPage_bin, "wb"); // TODO : Support wide char paths using _wfopen
			if (fp)
			{
				DbgPrintf("Persisting LaunchDataPage\n");
				fseek(fp, 0, SEEK_SET);
				fwrite(LaunchDataPage, sizeof(LAUNCH_DATA_PAGE), 1, fp);
				fclose(fp);
			}
			else
				DbgPrintf("Can't persist LaunchDataPage to %s!\n", szFilePath_LaunchDataPage_bin);
		}
		else
		{
			DbgPrintf("Forgetting LaunchDataPage\n");
			remove(szFilePath_LaunchDataPage_bin);
		}
	}
	else
		// TODO : Store/forget other pages to be remembered across a "reboot"
		LOG_IGNORED();

  // [PatrickvL] Shared memory would be a perfect fit for this,
  // but the supplied pointer is already allocated. In order to
  // change the 'shared' state of this memory, we would have to
  // obtain the complete memory range (via CreateFileMapping)
  // in one go, and use this for all allocation (much work, this).
  // Another way would be assume 'contiguous memory' is the
  // only type of memory being persisted, which would simplify
  // the allocation procedure significantly.
  // Another way could be to persist all registered blocks
  // of memory at application shutdown, but restoring it in
  // the next run at the same addresses could be troublesome.
}

// ******************************************************************
// * 0x00B3 - MmQueryAddressProtect()
// ******************************************************************
XBSYSAPI EXPORTNUM(179) xboxkrnl::ULONG NTAPI xboxkrnl::MmQueryAddressProtect
(
	IN PVOID VirtualAddress
)
{
	LOG_FUNC_ONE_ARG(VirtualAddress);

	// Assume read/write when page is allocated :
	ULONG Result = PAGE_NOACCESS;

	if (g_MemoryManager.IsAllocated(VirtualAddress)) {
		Result = PAGE_READWRITE;
	}

	LOG_INCOMPLETE(); // TODO : Improve the MmQueryAddressProtect implementation
	
	RETURN(Result);
}

// ******************************************************************
// * 0x00B4 - MmQueryAllocationSize()
// ******************************************************************
XBSYSAPI EXPORTNUM(180) xboxkrnl::ULONG NTAPI xboxkrnl::MmQueryAllocationSize
(
	IN PVOID   BaseAddress
)
{
	LOG_FUNC_ONE_ARG(BaseAddress);

	LOG_INCOMPLETE(); // TODO : Free PAGE_WRITECOMBINE differently

	ULONG uiSize = g_MemoryManager.QueryAllocationSize(BaseAddress);

	RETURN(uiSize);
}

// ******************************************************************
// * 0x00B5 - MmQueryStatistics()
// ******************************************************************
XBSYSAPI EXPORTNUM(181) xboxkrnl::NTSTATUS NTAPI xboxkrnl::MmQueryStatistics
(
	OUT PMM_STATISTICS MemoryStatistics
)
{
	LOG_FUNC_ONE_ARG_OUT(MemoryStatistics);

	MEMORYSTATUS MemoryStatus;
	SYSTEM_INFO SysInfo;
	NTSTATUS ret;

	if (MemoryStatistics->Length == sizeof(MM_STATISTICS))
	{
		GlobalMemoryStatus(&MemoryStatus);
		GetSystemInfo(&SysInfo);

		/**
		* When each of the PMM_STATISTICS MemoryStatistics elements
		* are setup correctly below, these two lines become redundant
		*/
		ZeroMemory(MemoryStatistics, sizeof(MM_STATISTICS));
		MemoryStatistics->Length = sizeof(MM_STATISTICS);

		MemoryStatistics->TotalPhysicalPages = MemoryStatus.dwTotalPhys / SysInfo.dwPageSize;
		MemoryStatistics->AvailablePages = MemoryStatus.dwAvailPhys / SysInfo.dwPageSize;
		MemoryStatistics->VirtualMemoryBytesCommitted = MemoryStatus.dwTotalVirtual - MemoryStatus.dwAvailVirtual;
		MemoryStatistics->VirtualMemoryBytesReserved = MemoryStatus.dwAvailVirtual;
		// MemoryStatistics->CachePagesCommitted = [ ];
		// MemoryStatistics->PoolPagesCommitted = [ ];
		// MemoryStatistics->StackPagesCommitted = [ ];
		// MemoryStatistics->ImagePagesCommitted = [ ];

		DbgPrintf("   MemoryStatistics->Length                      = 0x%.08X\n", MemoryStatistics->Length);
		DbgPrintf("   MemoryStatistics->TotalPhysicalPages          = 0x%.08X\n", MemoryStatistics->TotalPhysicalPages);
		DbgPrintf("   MemoryStatistics->AvailablePages              = 0x%.08X\n", MemoryStatistics->AvailablePages);
		DbgPrintf("   MemoryStatistics->VirtualMemoryBytesCommitted = 0x%.08X\n", MemoryStatistics->VirtualMemoryBytesCommitted);
		DbgPrintf("   MemoryStatistics->VirtualMemoryBytesReserved  = 0x%.08X\n", MemoryStatistics->VirtualMemoryBytesReserved);
		DbgPrintf("   MemoryStatistics->CachePagesCommitted         = 0x%.08X\n", MemoryStatistics->CachePagesCommitted);
		DbgPrintf("   MemoryStatistics->PoolPagesCommitted          = 0x%.08X\n", MemoryStatistics->PoolPagesCommitted);
		DbgPrintf("   MemoryStatistics->StackPagesCommitted         = 0x%.08X\n", MemoryStatistics->StackPagesCommitted);
		DbgPrintf("   MemoryStatistics->ImagePagesCommitted         = 0x%.08X\n", MemoryStatistics->ImagePagesCommitted);

		ret = STATUS_SUCCESS;
	}
	else
	{
		EmuWarning("EmuKrnl: MmQueryStatistics with unusual size -> 0x%.08X", MemoryStatistics->Length);
		ret = STATUS_INVALID_PARAMETER;
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x00B6 - MmSetAddressProtect()
// ******************************************************************
XBSYSAPI EXPORTNUM(182) xboxkrnl::VOID NTAPI xboxkrnl::MmSetAddressProtect
(
	IN PVOID BaseAddress,
	IN ULONG NumberOfBytes,
	IN ULONG NewProtect
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(BaseAddress)
		LOG_FUNC_ARG(NumberOfBytes)
		LOG_FUNC_ARG(NewProtect)
		LOG_FUNC_END;

	DWORD dwOldProtect;

	if (!VirtualProtect(BaseAddress, NumberOfBytes, NewProtect & (~PAGE_WRITECOMBINE), &dwOldProtect))
		EmuWarning("VirtualProtect Failed!");

	DbgPrintf("EmuKrnl: VirtualProtect was 0x%.08X -> 0x%.08X\n", dwOldProtect, NewProtect & (~PAGE_WRITECOMBINE));
}

// ******************************************************************
// * 0x00B7 - MmUnmapIoSpace()
// ******************************************************************
// Unmaps a virtual address mapping made by MmMapIoSpace.
//
// Differences from NT: None.
XBSYSAPI EXPORTNUM(183) xboxkrnl::NTSTATUS NTAPI xboxkrnl::MmUnmapIoSpace
(
	IN PVOID BaseAddress,
	IN ULONG NumberOfBytes
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(BaseAddress)
		LOG_FUNC_ARG(NumberOfBytes)
		LOG_FUNC_END;

	if ((xbaddr)BaseAddress >= XBOX_WRITE_COMBINED_BASE) { // 0xF0000000
		// Don't free hardware devices (flash, NV2A, etc)
	}
	else {
		g_MemoryManager.Free(BaseAddress);
		LOG_INCOMPLETE();
	}

	RETURN(STATUS_SUCCESS);
}

