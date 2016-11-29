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

// prevent name collisions
namespace NtDll
{
	#include "EmuNtDll.h" // For NtAllocateVirtualMemory(), etc.
};

#include "CxbxKrnl.h" // For CxbxKrnlCleanup
#include "Emu.h" // For EmuWarning()
#include "EmuAlloc.h" // For CxbxFree(), CxbxMalloc(), etc.
#include "ResourceTracker.h" // For g_AlignCache


using namespace xboxkrnl;


XBSYSAPI EXPORTNUM(102) xboxkrnl::PVOID xboxkrnl::MmGlobalData[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

// ******************************************************************
// * xLaunchDataPage (pointed to by LaunchDataPage)
// ******************************************************************
LAUNCH_DATA_PAGE xLaunchDataPage =
{
	{   // header
		2,  // 2: dashboard, 0: title
		0,
		"D:\\default.xbe",
		0
	}
};

// ******************************************************************
// * 0x00A4 - LaunchDataPage (actually a pointer)
// ******************************************************************
XBSYSAPI EXPORTNUM(164) xboxkrnl::PLAUNCH_DATA_PAGE xboxkrnl::LaunchDataPage = &xLaunchDataPage;

// ******************************************************************
// * 0x00A5 - MmAllocateContiguousMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(165) xboxkrnl::PVOID NTAPI xboxkrnl::MmAllocateContiguousMemory
(
	IN ULONG NumberOfBytes
)
{
	LOG_FUNC_ONE_ARG(NumberOfBytes);

	//
	// NOTE: Kludgey (but necessary) solution:
	//
	// Since this memory must be aligned on a page boundary, we must allocate an extra page
	// so that we can return a valid page aligned pointer
	//

	PVOID pRet = CxbxMalloc(NumberOfBytes + 0x1000);

	// align to page boundary
	{
		DWORD dwRet = (DWORD)pRet;

		dwRet += 0x1000 - dwRet % 0x1000;

		g_AlignCache.insert(dwRet, pRet);

		pRet = (PVOID)dwRet;
	}

	DbgPrintf("EmuKrnl (0x%X): MmAllocateContiguousMemory returned 0x%.08X\n", GetCurrentThreadId(), pRet);

	RETURN(pRet);
}

// ******************************************************************
// * 0x00A6 - MmAllocateContiguousMemoryEx
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
		LOG_FUNC_ARG(ProtectionType)
		LOG_FUNC_END;

	//
	// NOTE: Kludgey (but necessary) solution:
	//
	// Since this memory must be aligned on a page boundary, we must allocate an extra page
	// so that we can return a valid page aligned pointer
	//

	PVOID pRet = CxbxMalloc(NumberOfBytes + 0x1000);

	// align to page boundary
	{
		DWORD dwRet = (DWORD)pRet;

		dwRet += 0x1000 - dwRet % 0x1000;

		g_AlignCache.insert(dwRet, pRet);

		pRet = (PVOID)dwRet;
	}

	static int count = 0;

	RETURN(pRet);
}

// ******************************************************************
// * 0x00A7 - MmAllocateSystemMemory
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
	PVOID pRet = CxbxMalloc(NumberOfBytes);

	RETURN(pRet);
}

// ******************************************************************
// * 0x00A9 - MmCreateKernelStack
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

	NtDll::PVOID pRet = NULL;

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

	if (FAILED(NtDll::NtAllocateVirtualMemory(GetCurrentProcess(), &pRet, 0, &NumberOfBytes, MEM_COMMIT, PAGE_READWRITE)))
		EmuWarning("MmCreateKernelStack failed!\n");
	else
		pRet = (PVOID)((ULONG)pRet + NumberOfBytes);

	RETURN(pRet);
}

// ******************************************************************
// * 0x00AA - MmDeleteKernelStack
// ******************************************************************
XBSYSAPI EXPORTNUM(170) VOID NTAPI xboxkrnl::MmDeleteKernelStack
(
	PVOID EndAddress,
	PVOID BaseAddress
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(EndAddress)
		LOG_FUNC_ARG(BaseAddress)
		LOG_FUNC_END;

	/* __asm int 3;
	CxbxKrnlCleanup( "MmDeleteKernelStack unimplemented (check call stack)" );*/
	ULONG RegionSize = 0;
	if (FAILED(NtDll::NtFreeVirtualMemory(GetCurrentProcess(), &BaseAddress, &RegionSize, MEM_RELEASE)))
		EmuWarning("MmDeleteKernelStack failed!\n");
}

// ******************************************************************
// * 0x00AB - MmFreeContiguousMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(171) VOID NTAPI xboxkrnl::MmFreeContiguousMemory
(
	IN PVOID BaseAddress
)
{
	LOG_FUNC_ONE_ARG(BaseAddress);

	PVOID OrigBaseAddress = BaseAddress;

	if (g_AlignCache.exists(BaseAddress))
	{
		OrigBaseAddress = g_AlignCache.get(BaseAddress);

		g_AlignCache.remove(BaseAddress);
	}

	if (OrigBaseAddress != &xLaunchDataPage)
	{
		CxbxFree(OrigBaseAddress);
	}
	else
	{
		DbgPrintf("Ignored MmFreeContiguousMemory(&xLaunchDataPage)\n");
	}
}

// ******************************************************************
// * 0x00AC - MmFreeSystemMemory
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

	CxbxFree(BaseAddress);

	RETURN(STATUS_SUCCESS);
}

// ******************************************************************
// * 0x00AD - MmGetPhysicalAddress
// ******************************************************************
XBSYSAPI EXPORTNUM(173) PHYSICAL_ADDRESS NTAPI xboxkrnl::MmGetPhysicalAddress
(
	IN PVOID   BaseAddress
)
{
	LOG_FUNC_ONE_ARG_OUT(BaseAddress);
	
	// We emulate Virtual/Physical memory 1:1	
	return (PHYSICAL_ADDRESS)BaseAddress;
}

// ******************************************************************
// * 0x00AF - MmLockUnlockBufferPages
// ******************************************************************
XBSYSAPI EXPORTNUM(175) void NTAPI xboxkrnl::MmLockUnlockBufferPages
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
// * 0x00B1 - MmMapIoSpace
// ******************************************************************
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

	// TODO: should this be aligned?
	PVOID pRet = CxbxMalloc(NumberOfBytes);

	RETURN(pRet);
}

// ******************************************************************
// * 0x00B2 - MmPersistContiguousMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(178) VOID NTAPI xboxkrnl::MmPersistContiguousMemory
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

	// TODO: Actually set this up to be remember across a "reboot"
	LOG_IGNORED();
}

// ******************************************************************
// * 0x00B4 - MmQueryAllocationSize
// ******************************************************************
XBSYSAPI EXPORTNUM(180) xboxkrnl::ULONG NTAPI xboxkrnl::MmQueryAllocationSize
(
	IN PVOID   BaseAddress
)
{
	LOG_FUNC_ONE_ARG(BaseAddress);

	ULONG uiSize = EmuCheckAllocationSize(BaseAddress, false);

	RETURN(uiSize);
}

// ******************************************************************
// * 0x00B5 - MmQueryStatistics
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

	if (MemoryStatistics->Length == 0x24)
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
		EmuWarning("EmuKrnl (0x%X): MmQueryStatistics with unusual size -> 0x%.08X\n", GetCurrentThreadId(), MemoryStatistics->Length);
		ret = STATUS_INVALID_PARAMETER;
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x00B6 - MmSetAddressProtect
// ******************************************************************
XBSYSAPI EXPORTNUM(182) VOID NTAPI xboxkrnl::MmSetAddressProtect
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

	DbgPrintf("EmuKrnl (0x%X): VirtualProtect was 0x%.08X -> 0x%.08X\n", GetCurrentThreadId(), dwOldProtect, NewProtect & (~PAGE_WRITECOMBINE));
}

// ******************************************************************
// * 0x00B7 - MmUnmapIoSpace
// ******************************************************************
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

	CxbxFree(BaseAddress);

	RETURN(STATUS_SUCCESS);
}

