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

#include "types.h"

namespace xbox
{

typedef ulong_xt PFN;
typedef ulong_xt PFN_COUNT;
typedef ulong_xt PFN_NUMBER, *PPFN_NUMBER;

 typedef struct _MMPFNFREE
 {
     ushort_xt PackedPfnFlink;
     ushort_xt PackedPfnBlink;
 } MMPFNFREE, *PMMPFNFREE;

 typedef struct _MMPFNREGION
 {
     MMPFNFREE FreePagesByColor[32];
     PFN_COUNT AvailablePages;
 } MMPFNREGION, *PMMPFNREGION;

 /* enum describing the usage type of the memory pages */
 typedef enum _PageType
 {
     UnknownType,                   // Used by the PFN database
     StackType,                     // Used by MmCreateKernelStack
     VirtualPageTableType,          // Used by the pages holding the PTs that map the user memory (lower 2 GiB)
     SystemPageTableType,           // Used by the pages holding the PTs that map the system memory
     PoolType,                      // Used by ExAllocatePoolWithTag
     VirtualMemoryType,             // Used by NtAllocateVirtualMemory
     SystemMemoryType,              // Used by MmAllocateSystemMemory
     ImageType,                     // Used to mark executable memory
     CacheType,                     // Used by the file cache related functions
     ContiguousType,                // Used by MmAllocateContiguousMemoryEx and others
     DebuggerType,                  // xbdm-related
     COUNTtype                      // The size of the array containing the page usage per type
 } PageType;

 /* The Xbox PTE, modelled around the Intel 386 PTE specification */
 typedef struct _XBOX_PTE
 {
     ulong_xt Valid : 1;
     ulong_xt Write : 1;
     ulong_xt Owner : 1;
     ulong_xt WriteThrough : 1;
     ulong_xt CacheDisable : 1;
     ulong_xt Accessed : 1;
     ulong_xt Dirty : 1;
     ulong_xt LargePage : 1;
     ulong_xt Global : 1;
     ulong_xt GuardOrEnd : 1; // software field used for our own purposes
     ulong_xt Persist : 1;    // software field used for our own purposes
     ulong_xt Unused : 1;     // software field used for our own purposes
     ulong_xt PFN : 20;
 } XBOX_PTE, *PXBOX_PTE;


 /* PTE as used by the memory manager */
 typedef union _MMPTE
 {
     ulong_xt Default;
     XBOX_PTE Hardware;
 } MMPTE, *PMMPTE;

 typedef PFN_NUMBER (FASTCALL *PMMREMOVE_PAGE_ROUTINE) (
     IN PageType BusyType,
     IN PMMPTE TargetPte
     );

 typedef struct _MMPTERANGE
 {
     MMPTE HeadPte;
     PMMPTE FirstCommittedPte;
     PMMPTE LastCommittedPte;
     PMMPTE LastReservedPte;
     PFN_COUNT *AvailablePages;
     PMMREMOVE_PAGE_ROUTINE RemovePageRoutine;
 } MMPTERANGE, *PMMPTERANGE;

 typedef struct _MMADDRESS_NODE
 {
     ulong_ptr_xt StartingVpn;
     ulong_ptr_xt EndingVpn;
     struct _MMADDRESS_NODE *Parent;
     struct _MMADDRESS_NODE *LeftChild;
     struct _MMADDRESS_NODE *RightChild;
 } MMADDRESS_NODE, *PMMADDRESS_NODE;

typedef struct _MMGLOBALDATA
{
    PMMPFNREGION RetailPfnRegion;
    PMMPTERANGE SystemPteRange;
    PULONG AvailablePages;
    PFN_COUNT *AllocatedPagesByUsage;
    PRTL_CRITICAL_SECTION AddressSpaceLock;
    PMMADDRESS_NODE *VadRoot;
    PMMADDRESS_NODE *VadHint;
    PMMADDRESS_NODE *VadFreeHint;
} MMGLOBALDATA, *PMMGLOBALDATA;

XBSYSAPI EXPORTNUM(102) MMGLOBALDATA MmGlobalData;

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
    IN ulong_xt NumberOfBytes
);

// ******************************************************************
// * MmAllocateContiguousMemoryEx
// ******************************************************************
XBSYSAPI EXPORTNUM(166) PVOID NTAPI MmAllocateContiguousMemoryEx
(
    IN ulong_xt            NumberOfBytes,
    IN physical_address_xt LowestAcceptableAddress,
    IN physical_address_xt HighestAcceptableAddress,
    IN ulong_xt            Alignment OPTIONAL,
    IN ulong_xt            ProtectionType
);

// ******************************************************************
// * MmAllocateSystemMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(167) PVOID NTAPI MmAllocateSystemMemory
(
    ulong_xt NumberOfBytes,
    ulong_xt Protect
);

// ******************************************************************
// * MmClaimGpuInstanceMemory;
// ******************************************************************
XBSYSAPI EXPORTNUM(168) PVOID NTAPI MmClaimGpuInstanceMemory
(
	IN size_xt NumberOfBytes,
	OUT size_xt *NumberOfPaddingBytes
);

// ******************************************************************
// * MmCreateKernelStack
// ******************************************************************
XBSYSAPI EXPORTNUM(169) PVOID NTAPI MmCreateKernelStack
(
    IN ulong_xt	NumberOfBytes,
    IN boolean_xt	DebuggerThread
);

// ******************************************************************
// * MmDeleteKernelStack
// ******************************************************************
XBSYSAPI EXPORTNUM(170) void_xt NTAPI MmDeleteKernelStack
(
    IN PVOID StackBase,
    IN PVOID StackLimit
);

// ******************************************************************
// * MmFreeContiguousMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(171) void_xt NTAPI MmFreeContiguousMemory
(
    IN PVOID BaseAddress
);

// ******************************************************************
// * MmFreeSystemMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(172) ulong_xt NTAPI MmFreeSystemMemory
(
    PVOID BaseAddress,
    ulong_xt NumberOfBytes
);

// ******************************************************************
// * MmGetPhysicalAddress
// ******************************************************************
XBSYSAPI EXPORTNUM(173) physical_address_xt NTAPI MmGetPhysicalAddress
(
    IN PVOID   BaseAddress
);

// ******************************************************************
// * MmIsAddressValid
// ******************************************************************
XBSYSAPI EXPORTNUM(174) boolean_xt NTAPI MmIsAddressValid
(
	IN PVOID   VirtualAddress
);

// ******************************************************************
// * MmLockUnlockBufferPages
// ******************************************************************
XBSYSAPI EXPORTNUM(175) void_xt NTAPI MmLockUnlockBufferPages
(
    IN PVOID             BaseAddress,
    IN size_xt            NumberOfBytes,
    IN boolean_xt           UnlockPages
);

// ******************************************************************
// * MmLockUnlockPhysicalPage
// ******************************************************************
XBSYSAPI EXPORTNUM(176) void_xt NTAPI MmLockUnlockPhysicalPage
(
	IN ulong_ptr_xt PhysicalAddress,
	IN boolean_xt UnlockPage
);

// ******************************************************************
// * MmMapIoSpace
// ******************************************************************
XBSYSAPI EXPORTNUM(177) PVOID NTAPI MmMapIoSpace
(
    IN physical_address_xt PhysicalAddress,
    IN ulong_xt            NumberOfBytes,
    IN ulong_xt            ProtectionType
);

// ******************************************************************
// * MmPersistContiguousMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(178) void_xt NTAPI MmPersistContiguousMemory
(
    IN PVOID   BaseAddress,
    IN ulong_xt   NumberOfBytes,
    IN boolean_xt Persist
);

// ******************************************************************
// * MmQueryAddressProtect
// ******************************************************************
XBSYSAPI EXPORTNUM(179) ulong_xt NTAPI MmQueryAddressProtect
(
	IN PVOID VirtualAddress
);

// ******************************************************************
// * MmQueryAllocationSize
// ******************************************************************
XBSYSAPI EXPORTNUM(180) ulong_xt NTAPI MmQueryAllocationSize
(
    IN PVOID   BaseAddress
);

// ******************************************************************
// * MmQueryStatistics
// ******************************************************************
XBSYSAPI EXPORTNUM(181) ntstatus_xt NTAPI MmQueryStatistics
(
    OUT PMM_STATISTICS MemoryStatistics
);

// ******************************************************************
// * MmSetAddressProtect
// ******************************************************************
XBSYSAPI EXPORTNUM(182) void_xt NTAPI MmSetAddressProtect
(
    IN PVOID BaseAddress,
    IN ulong_xt NumberOfBytes,
    IN ulong_xt NewProtect
);

// ******************************************************************
// * MmUnmapIoSpace
// ******************************************************************
XBSYSAPI EXPORTNUM(183) void_xt NTAPI MmUnmapIoSpace
(
    IN PVOID BaseAddress,
    IN ulong_xt NumberOfBytes
);

// ******************************************************************
// * MmDbgAllocateMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(374) PVOID NTAPI MmDbgAllocateMemory
(
	IN ulong_xt NumberOfBytes,
	IN ulong_xt Protect
);

// ******************************************************************
// * MmDbgFreeMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(375) ulong_xt NTAPI MmDbgFreeMemory
(
	IN PVOID BaseAddress,
	IN ulong_xt NumberOfBytes
);

// ******************************************************************
// * MmDbgQueryAvailablePages
// ******************************************************************
XBSYSAPI EXPORTNUM(376) ulong_xt NTAPI MmDbgQueryAvailablePages(void);

// ******************************************************************
// * MmDbgReleaseAddress
// ******************************************************************
XBSYSAPI EXPORTNUM(377) void_xt NTAPI MmDbgReleaseAddress
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



