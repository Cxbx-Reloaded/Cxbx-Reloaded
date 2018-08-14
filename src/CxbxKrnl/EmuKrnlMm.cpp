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
// *  (c) 2017-2018 ergo720
// *
// *  All rights reserved
// *
// ******************************************************************
#define _XBOXKRNL_DEFEXTRN_

#define LOG_PREFIX CXBXR_MODULE::MM

// prevent name collisions
namespace xboxkrnl
{
	#include <xboxkrnl/xboxkrnl.h> // For MmGlobalData, etc.
};

#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnl.h" // For DefaultLaunchDataPage
#include "EmuKrnlLogging.h"
#include "CxbxKrnl.h" // For CxbxKrnlCleanup
#include "Emu.h" // For EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, )
#include "VMManager.h"
#include "EmuShared.h"
#include <assert.h>

// prevent name collisions
namespace NtDll
{
	#include "EmuNtDll.h" // For NtAllocateVirtualMemory(), etc.
};

// ******************************************************************
// * 0x0066 - MmGlobalData
// ******************************************************************
// ergo720: a couple of these could be implemented, but most cannot. However, I wouldn't bother with these variables
// since they are just exported but never used by the kernel
XBSYSAPI EXPORTNUM(102) xboxkrnl::PVOID xboxkrnl::MmGlobalData[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

// ******************************************************************
// * 0x00A4 - LaunchDataPage
// ******************************************************************
// Note : Originally, LaunchDataPage resides in a "STICKY" segment in
// the xbox kernel. Kernel code accessses this as a normal variable.
// XAPI code however, reference to the address of this kernel variable,
// thus use indirection (*LaunchDataPage) to get to the same contents.
XBSYSAPI EXPORTNUM(164) xboxkrnl::PLAUNCH_DATA_PAGE xboxkrnl::LaunchDataPage = xbnullptr;

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

	return MmAllocateContiguousMemoryEx(NumberOfBytes, 0, MAXULONG_PTR, 0, XBOX_PAGE_READWRITE);
}

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

	PVOID pRet = (PVOID)g_VMManager.AllocateContiguous(NumberOfBytes, LowestAcceptableAddress, HighestAcceptableAddress, Alignment, ProtectionType);

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
		LOG_FUNC_ARG_TYPE(PROTECTION_TYPE, Protect)
	LOG_FUNC_END;

	PVOID addr = (PVOID)g_VMManager.AllocateSystemMemory(SystemMemoryType, Protect, NumberOfBytes, false);

	RETURN(addr);
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

	PVOID Result = (PVOID)g_VMManager.ClaimGpuMemory(NumberOfBytes, (size_t*)NumberOfPaddingBytes);

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

	PVOID addr = (PVOID)g_VMManager.AllocateSystemMemory(DebuggerThread ? DebuggerType : StackType,
		XBOX_PAGE_READWRITE, NumberOfBytes, true);

	RETURN(addr);
}

// ******************************************************************
// * 0x00AA - MmDeleteKernelStack()
// ******************************************************************
XBSYSAPI EXPORTNUM(170) xboxkrnl::VOID NTAPI xboxkrnl::MmDeleteKernelStack
(
	PVOID StackBase,
	PVOID StackLimit
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(StackBase)
		LOG_FUNC_ARG(StackLimit)
	LOG_FUNC_END;

	g_VMManager.DeallocateSystemMemory(IS_SYSTEM_ADDRESS(StackBase) ? StackType : DebuggerType,
		(VAddr)StackBase, (VAddr)StackBase - (VAddr)StackLimit + PAGE_SIZE);
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

	g_VMManager.DeallocateContiguous((VAddr)BaseAddress);

	// TODO -oDxbx: Sokoban crashes after this, at reset time (press Black + White to hit this).
	// Tracing in assembly shows the crash takes place quite a while further, so it's probably
	// not related to this call per-se. The strangest thing is, that if we let the debugger step
	// all the way through, the crash doesn't occur. Adding a Sleep(100) here doesn't help though.
}

// ******************************************************************
// * 0x00AC - MmFreeSystemMemory()
// ******************************************************************
XBSYSAPI EXPORTNUM(172) xboxkrnl::ULONG NTAPI xboxkrnl::MmFreeSystemMemory
(
	PVOID BaseAddress,
	ULONG NumberOfBytes
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(BaseAddress)
		LOG_FUNC_ARG(NumberOfBytes)
	LOG_FUNC_END;

	ULONG FreedPagesNumber = g_VMManager.DeallocateSystemMemory(SystemMemoryType, (VAddr)BaseAddress, NumberOfBytes);

	RETURN(FreedPagesNumber);
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

	PAddr addr = g_VMManager.TranslateVAddrToPAddr((VAddr)BaseAddress);

	RETURN(addr);
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

	BOOLEAN Ret = FALSE;

	if (g_VMManager.IsValidVirtualAddress((VAddr)VirtualAddress)) { Ret = TRUE; }

	RETURN(Ret);
}

// ******************************************************************
// * 0x00AF - MmLockUnlockBufferPages()
// ******************************************************************
XBSYSAPI EXPORTNUM(175) xboxkrnl::VOID NTAPI xboxkrnl::MmLockUnlockBufferPages
(
	IN PVOID	        BaseAddress,
	IN SIZE_T			NumberOfBytes,
	IN BOOLEAN			UnlockPages
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(BaseAddress)
		LOG_FUNC_ARG(NumberOfBytes)
		LOG_FUNC_ARG(UnlockPages)
	LOG_FUNC_END;

	// REMARK: all the pages inside the main memory pool are non-relocatable so, for the moment, this function is pointless

	g_VMManager.LockBufferOrSinglePage(0, (VAddr)BaseAddress, NumberOfBytes, UnlockPages);
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

	// REMARK: all the pages inside the main memory pool are non-relocatable so, for the moment, this function is pointless

	g_VMManager.LockBufferOrSinglePage(PhysicalAddress, 0, 0, UnlockPage);
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
		LOG_FUNC_ARG_TYPE(PROTECTION_TYPE, ProtectionType)
	LOG_FUNC_END;

	PVOID pRet = (PVOID)g_VMManager.MapDeviceMemory(PhysicalAddress, NumberOfBytes, ProtectionType);

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

	g_VMManager.PersistMemory((VAddr)BaseAddress, NumberOfBytes, Persist);
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

	ULONG Result = g_VMManager.QueryProtection((VAddr)VirtualAddress);
	
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

	ULONG Size = g_VMManager.QuerySize((VAddr)BaseAddress, false);

	RETURN(Size);
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

	NTSTATUS ret;

	if (!MemoryStatistics)
	{
		EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "MmQueryStatistics : PMM_STATISTICS MemoryStatistics is nullptr!\n");
		LOG_IGNORED();
		RETURN(STATUS_INVALID_PARAMETER);
	}

	if (MemoryStatistics->Length == sizeof(MM_STATISTICS))
	{
		g_VMManager.MemoryStatistics(MemoryStatistics);

		DbgPrintf(LOG_PREFIX, "   MemoryStatistics->Length                      = 0x%.08X\n", MemoryStatistics->Length);
		DbgPrintf(LOG_PREFIX, "   MemoryStatistics->TotalPhysicalPages          = 0x%.08X\n", MemoryStatistics->TotalPhysicalPages);
		DbgPrintf(LOG_PREFIX, "   MemoryStatistics->AvailablePages              = 0x%.08X\n", MemoryStatistics->AvailablePages);
		DbgPrintf(LOG_PREFIX, "   MemoryStatistics->VirtualMemoryBytesCommitted = 0x%.08X\n", MemoryStatistics->VirtualMemoryBytesCommitted);
		DbgPrintf(LOG_PREFIX, "   MemoryStatistics->VirtualMemoryBytesReserved  = 0x%.08X\n", MemoryStatistics->VirtualMemoryBytesReserved);
		DbgPrintf(LOG_PREFIX, "   MemoryStatistics->CachePagesCommitted         = 0x%.08X\n", MemoryStatistics->CachePagesCommitted);
		DbgPrintf(LOG_PREFIX, "   MemoryStatistics->PoolPagesCommitted          = 0x%.08X\n", MemoryStatistics->PoolPagesCommitted);
		DbgPrintf(LOG_PREFIX, "   MemoryStatistics->StackPagesCommitted         = 0x%.08X\n", MemoryStatistics->StackPagesCommitted);
		DbgPrintf(LOG_PREFIX, "   MemoryStatistics->ImagePagesCommitted         = 0x%.08X\n", MemoryStatistics->ImagePagesCommitted);

		ret = STATUS_SUCCESS;
	}
	else
	{
		EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "MmQueryStatistics with unusual size -> 0x%.8X", MemoryStatistics->Length);
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
		LOG_FUNC_ARG_TYPE(PROTECTION_TYPE, NewProtect)
	LOG_FUNC_END;

	g_VMManager.Protect((VAddr)BaseAddress, NumberOfBytes, NewProtect);
}

// ******************************************************************
// * 0x00B7 - MmUnmapIoSpace()
// ******************************************************************
// Unmaps a virtual address mapping made by MmMapIoSpace.
//
// Differences from NT: None.
XBSYSAPI EXPORTNUM(183) xboxkrnl::VOID NTAPI xboxkrnl::MmUnmapIoSpace
(
	IN PVOID BaseAddress,
	IN ULONG NumberOfBytes
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(BaseAddress)
		LOG_FUNC_ARG(NumberOfBytes)
	LOG_FUNC_END;

	g_VMManager.UnmapDeviceMemory((VAddr)BaseAddress, NumberOfBytes);
}

// ******************************************************************
// * 0x0176 - MmDbgAllocateMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(374) xboxkrnl::PVOID NTAPI xboxkrnl::MmDbgAllocateMemory
(
	IN ULONG NumberOfBytes,
	IN ULONG Protect
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(NumberOfBytes)
		LOG_FUNC_ARG_TYPE(PROTECTION_TYPE, Protect)
	LOG_FUNC_END;

	// This should only be called by debug xbe's
	assert(g_bIsDebug);

	PVOID addr = (PVOID)g_VMManager.AllocateSystemMemory(DebuggerType, Protect, NumberOfBytes, false);
	if (addr) { RtlFillMemoryUlong((void*)addr, ROUND_UP_4K(NumberOfBytes), 0); } // debugger pages are zeroed

	RETURN(addr);
}

// ******************************************************************
// * 0x0177 - MmDbgFreeMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(375) xboxkrnl::ULONG NTAPI xboxkrnl::MmDbgFreeMemory
(
	IN PVOID BaseAddress,
	IN ULONG NumberOfBytes
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(BaseAddress)
		LOG_FUNC_ARG(NumberOfBytes)
	LOG_FUNC_END;

	// This should only be called by debug xbe's
	assert(g_bIsDebug);

	ULONG FreedPagesNumber = g_VMManager.DeallocateSystemMemory(DebuggerType, (VAddr)BaseAddress, NumberOfBytes);

	RETURN(FreedPagesNumber);
}

// ******************************************************************
// * 0x0178 - MmDbgQueryAvailablePages
// ******************************************************************
XBSYSAPI EXPORTNUM(376) xboxkrnl::ULONG NTAPI xboxkrnl::MmDbgQueryAvailablePages(void)
{
	LOG_FUNC();

	// This should only be called by debug xbe's
	assert(g_bIsDebug);

	ULONG FreeDebuggerPageNumber = g_VMManager.QueryNumberOfFreeDebuggerPages();

	RETURN(FreeDebuggerPageNumber);
}

// ******************************************************************
// * 0x0179 - MmDbgReleaseAddress
// ******************************************************************
XBSYSAPI EXPORTNUM(377) xboxkrnl::VOID NTAPI xboxkrnl::MmDbgReleaseAddress
(
	IN PVOID VirtualAddress,
	IN PULONG Opaque
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(VirtualAddress)
		LOG_FUNC_ARG(Opaque)
	LOG_FUNC_END;

	// This should only be called by debug xbe's
	assert(g_bIsDebug);

	g_VMManager.DbgTestPte((VAddr)VirtualAddress, (PMMPTE)Opaque, false);
}

// ******************************************************************
// * 0x017A - MmDbgWriteCheck
// ******************************************************************
XBSYSAPI EXPORTNUM(378) xboxkrnl::PVOID NTAPI xboxkrnl::MmDbgWriteCheck
(
	IN PVOID VirtualAddress,
	IN PULONG Opaque
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(VirtualAddress)
		LOG_FUNC_ARG(Opaque)
	LOG_FUNC_END;

	// This should only be called by debug xbe's
	assert(g_bIsDebug);

	PVOID addr = (PVOID)g_VMManager.DbgTestPte((VAddr)VirtualAddress, (PMMPTE)Opaque, true);

	RETURN(addr);
}
