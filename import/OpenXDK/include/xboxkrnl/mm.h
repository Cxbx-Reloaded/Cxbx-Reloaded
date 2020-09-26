// ******************************************************************
// *
// * proj : OpenXDK
// *
// * desc : Open Source XBox Development Kit
// *
// * file : mm.h
// *
// * note : XBox Kernel *Memory Manager* Declarations
// *
// ******************************************************************
#ifndef XBOXKRNL_MM_H
#define XBOXKRNL_MM_H

#include "xboxkrnl/xboxkrnl_types.h"

namespace xbox
{

XBSYSAPI EXPORTNUM(102) PVOID MmGlobalData[8];

// ******************************************************************
// * MmAllocateContiguousMemory
// ******************************************************************
// *
// * Allocates a range of physically contiguous, cache-aligned
// * memory from nonpaged pool (main pool on xbox).
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(165) PVOID NTAPI MmAllocateContiguousMemory
(
    IN ulong_t NumberOfBytes
);

// ******************************************************************
// * MmAllocateContiguousMemoryEx
// ******************************************************************
XBSYSAPI EXPORTNUM(166) PVOID NTAPI MmAllocateContiguousMemoryEx
(
    IN ulong_t            NumberOfBytes,
    IN physical_address_t LowestAcceptableAddress,
    IN physical_address_t HighestAcceptableAddress,
    IN ulong_t            Alignment OPTIONAL,
    IN ulong_t            ProtectionType
);

// ******************************************************************
// * MmAllocateSystemMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(167) PVOID NTAPI MmAllocateSystemMemory
(
    ulong_t NumberOfBytes,
    ulong_t Protect
);

// ******************************************************************
// * MmClaimGpuInstanceMemory;
// ******************************************************************
XBSYSAPI EXPORTNUM(168) PVOID NTAPI MmClaimGpuInstanceMemory
(
	IN size_t NumberOfBytes,
	OUT size_t *NumberOfPaddingBytes
);

// ******************************************************************
// * MmCreateKernelStack
// ******************************************************************
XBSYSAPI EXPORTNUM(169) PVOID NTAPI MmCreateKernelStack
(
    IN ulong_t	NumberOfBytes,
    IN boolean_t	DebuggerThread
);

// ******************************************************************
// * MmDeleteKernelStack
// ******************************************************************
XBSYSAPI EXPORTNUM(170) void_t NTAPI MmDeleteKernelStack
(
    IN PVOID StackBase,
    IN PVOID StackLimit
);

// ******************************************************************
// * MmFreeContiguousMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(171) void_t NTAPI MmFreeContiguousMemory
(
    IN PVOID BaseAddress
);

// ******************************************************************
// * MmFreeSystemMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(172) ulong_t NTAPI MmFreeSystemMemory
(
    PVOID BaseAddress,
    ulong_t NumberOfBytes
);

// ******************************************************************
// * MmGetPhysicalAddress
// ******************************************************************
XBSYSAPI EXPORTNUM(173) physical_address_t NTAPI MmGetPhysicalAddress
(
    IN PVOID   BaseAddress
);

// ******************************************************************
// * MmIsAddressValid
// ******************************************************************
XBSYSAPI EXPORTNUM(174) boolean_t NTAPI MmIsAddressValid
(
	IN PVOID   VirtualAddress
);

// ******************************************************************
// * MmLockUnlockBufferPages
// ******************************************************************
XBSYSAPI EXPORTNUM(175) void_t NTAPI MmLockUnlockBufferPages
(
    IN PVOID             BaseAddress,
    IN size_t            NumberOfBytes,
    IN boolean_t           UnlockPages
);

// ******************************************************************
// * MmLockUnlockPhysicalPage
// ******************************************************************
XBSYSAPI EXPORTNUM(176) void_t NTAPI MmLockUnlockPhysicalPage
(
	IN ulong_ptr_t PhysicalAddress,
	IN boolean_t UnlockPage
);

// ******************************************************************
// * MmMapIoSpace
// ******************************************************************
XBSYSAPI EXPORTNUM(177) PVOID NTAPI MmMapIoSpace
(
    IN physical_address_t PhysicalAddress,
    IN ulong_t            NumberOfBytes,
    IN ulong_t            ProtectionType
);

// ******************************************************************
// * MmPersistContiguousMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(178) void_t NTAPI MmPersistContiguousMemory
(
    IN PVOID   BaseAddress,
    IN ulong_t   NumberOfBytes,
    IN boolean_t Persist
);

// ******************************************************************
// * MmQueryAddressProtect
// ******************************************************************
XBSYSAPI EXPORTNUM(179) ulong_t NTAPI MmQueryAddressProtect
(
	IN PVOID VirtualAddress
);

// ******************************************************************
// * MmQueryAllocationSize
// ******************************************************************
XBSYSAPI EXPORTNUM(180) ulong_t NTAPI MmQueryAllocationSize
(
    IN PVOID   BaseAddress
);

// ******************************************************************
// * MmQueryStatistics
// ******************************************************************
XBSYSAPI EXPORTNUM(181) NTSTATUS NTAPI MmQueryStatistics
(
    OUT PMM_STATISTICS MemoryStatistics
);

// ******************************************************************
// * MmSetAddressProtect
// ******************************************************************
XBSYSAPI EXPORTNUM(182) void_t NTAPI MmSetAddressProtect
(
    IN PVOID BaseAddress,
    IN ulong_t NumberOfBytes,
    IN ulong_t NewProtect
);

// ******************************************************************
// * MmUnmapIoSpace
// ******************************************************************
XBSYSAPI EXPORTNUM(183) void_t NTAPI MmUnmapIoSpace
(
    IN PVOID BaseAddress,
    IN ulong_t NumberOfBytes
);

// ******************************************************************
// * MmDbgAllocateMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(374) PVOID NTAPI MmDbgAllocateMemory
(
	IN ulong_t NumberOfBytes,
	IN ulong_t Protect
);

// ******************************************************************
// * MmDbgFreeMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(375) ulong_t NTAPI MmDbgFreeMemory
(
	IN PVOID BaseAddress,
	IN ulong_t NumberOfBytes
);

// ******************************************************************
// * MmDbgQueryAvailablePages
// ******************************************************************
XBSYSAPI EXPORTNUM(376) ulong_t NTAPI MmDbgQueryAvailablePages(void);

// ******************************************************************
// * MmDbgReleaseAddress
// ******************************************************************
XBSYSAPI EXPORTNUM(377) void_t NTAPI MmDbgReleaseAddress
(
	IN PVOID VirtualAddress,
	IN PULONG Opaque
);

// ******************************************************************
// * MmDbgWriteCheck
// ******************************************************************
XBSYSAPI EXPORTNUM(378) PVOID NTAPI MmDbgWriteCheck
(
	IN PVOID VirtualAddress,
	IN PULONG Opaque
);

}

#endif



