// ******************************************************************
// *
// * proj : OpenXDK
// *
// * desc : Open Source XBox Development Kit
// *
// * file : ex.h
// *
// * note : XBox Kernel *Executive* Declarations
// *
// ******************************************************************
#ifndef XBOXKRNL_EX_H
#define XBOXKRNL_EX_H

#include "types.h"

namespace xbox
{

XBSYSAPI EXPORTNUM(12) void_xt NTAPI ExAcquireReadWriteLockExclusive
(
	IN PERWLOCK ReadWriteLock
);

XBSYSAPI EXPORTNUM(13) void_xt NTAPI ExAcquireReadWriteLockShared
(
	IN PERWLOCK ReadWriteLock
);

// ******************************************************************
// * ExAllocatePool
// ******************************************************************
// *
// * Allocates pool memory
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(14) PVOID NTAPI ExAllocatePool
(
    IN size_xt NumberOfBytes
);

// ******************************************************************
// * ExAllocatePoolWithTag
// ******************************************************************
// *
// * Allocates pool memory with a tag
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(15) PVOID NTAPI ExAllocatePoolWithTag
(
    IN size_xt NumberOfBytes,
    IN ulong_xt Tag
);

XBSYSAPI EXPORTNUM(16) OBJECT_TYPE ExEventObjectType;

// ******************************************************************
// * ExFreePool
// ******************************************************************
// *
// * Deallocates a block  of pool memory
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(17) void_xt NTAPI ExFreePool
(
    IN PVOID P
);

// ******************************************************************
// * 0x0012 - ExInitializeReadWriteLock()
// ******************************************************************
XBSYSAPI EXPORTNUM(18) void_xt NTAPI ExInitializeReadWriteLock
(
	IN PERWLOCK ReadWriteLock
);

// Source:APILogger - Uncertain
XBSYSAPI EXPORTNUM(19) LARGE_INTEGER NTAPI ExInterlockedAddLargeInteger
(
	IN OUT PLARGE_INTEGER Addend,
	IN LARGE_INTEGER Increment,
	IN OUT PKSPIN_LOCK Lock
);

// Source:ReactOS
XBSYSAPI EXPORTNUM(20) void_xt FASTCALL ExInterlockedAddLargeStatistic
(
	IN PLARGE_INTEGER Addend,
	IN ulong_xt Increment
);

// Source:ReactOS
XBSYSAPI EXPORTNUM(21) longlong_xt FASTCALL ExInterlockedCompareExchange64
(
	IN OUT PLONGLONG Destination,
	IN PLONGLONG Exchange,
	IN PLONGLONG Comparand
);

XBSYSAPI EXPORTNUM(22) OBJECT_TYPE ExMutantObjectType;

// ******************************************************************
// * ExQueryPoolBlockSize
// ******************************************************************
XBSYSAPI EXPORTNUM(23) ulong_xt NTAPI ExQueryPoolBlockSize
(
    IN PVOID PoolBlock
);

// ******************************************************************
// * 0x0018 - ExQueryNonVolatileSetting()
// ******************************************************************
XBSYSAPI EXPORTNUM(24) ntstatus_xt NTAPI ExQueryNonVolatileSetting
(
	IN  dword_xt               ValueIndex,
	OUT dword_xt              *Type,
	OUT PVOID               Value,
	IN  size_xt              ValueLength,
	OUT PSIZE_T             ResultLength OPTIONAL
);

// ******************************************************************
// * ExReadWriteRefurbInfo
// ******************************************************************
XBSYSAPI EXPORTNUM(25) ntstatus_xt NTAPI ExReadWriteRefurbInfo
(
	IN OUT PXBOX_REFURB_INFO	pRefurbInfo,
	IN ulong_xt	dwBufferSize,
	IN boolean_xt	bIsWriteMode
);

XBSYSAPI EXPORTNUM(26) void_xt NTAPI ExRaiseException
(
	IN PEXCEPTION_RECORD ExceptionRecord
);

XBSYSAPI EXPORTNUM(27) void_xt NTAPI ExRaiseStatus
(
	IN ntstatus_xt Status
);

XBSYSAPI EXPORTNUM(28) void_xt NTAPI ExReleaseReadWriteLock
(
	IN PERWLOCK ReadWriteLock
);

// ******************************************************************
// * 0x001D - ExSaveNonVolatileSetting()
// ******************************************************************
XBSYSAPI EXPORTNUM(29) ntstatus_xt NTAPI ExSaveNonVolatileSetting
(
	IN  dword_xt               ValueIndex,
	IN  dword_xt               Type,
	IN  PVOID               Value,
	IN  size_xt              ValueLength
);

XBSYSAPI EXPORTNUM(30) OBJECT_TYPE ExSemaphoreObjectType;

XBSYSAPI EXPORTNUM(31) OBJECT_TYPE ExTimerObjectType;

// ******************************************************************
// * 0x0020 - ExfInterlockedInsertHeadList()
// ******************************************************************
XBSYSAPI EXPORTNUM(32) PLIST_ENTRY FASTCALL ExfInterlockedInsertHeadList
(
	IN PLIST_ENTRY ListHead,
	IN PLIST_ENTRY ListEntry
);

// ******************************************************************
// * 0x0021 - ExfInterlockedInsertTailList()
// ******************************************************************
XBSYSAPI EXPORTNUM(33) PLIST_ENTRY FASTCALL ExfInterlockedInsertTailList
(
	IN PLIST_ENTRY ListHead,
	IN PLIST_ENTRY ListEntry
);

// ******************************************************************
// * 0x0022 - ExfInterlockedRemoveHeadList()
// ******************************************************************
XBSYSAPI EXPORTNUM(34) PLIST_ENTRY FASTCALL ExfInterlockedRemoveHeadList
(
	IN PLIST_ENTRY ListHead
);

XBSYSAPI EXPORTNUM(51) long_xt FASTCALL KRNL(InterlockedCompareExchange)
(
	IN OUT volatile PLONG Destination,
	IN long_xt  Exchange,
	IN long_xt  Comparand
);

XBSYSAPI EXPORTNUM(52) long_xt FASTCALL KRNL(InterlockedDecrement)
(
	IN OUT PLONG Addend
);

XBSYSAPI EXPORTNUM(53) long_xt FASTCALL KRNL(InterlockedIncrement)
(
	IN OUT PLONG Addend
);

XBSYSAPI EXPORTNUM(54) long_xt FASTCALL KRNL(InterlockedExchange)
(
	IN volatile PLONG Destination,
	IN long_xt Value
);

XBSYSAPI EXPORTNUM(55) long_xt FASTCALL KRNL(InterlockedExchangeAdd)
(
	IN volatile PLONG Addend,
	IN long_xt	Value
);

// Dxbx Note : The Xbox1 SINGLE_LIST strucures are the same as in WinNT
XBSYSAPI EXPORTNUM(56) SINGLE_LIST_ENTRY * FASTCALL KRNL(InterlockedFlushSList)
(
	IN PSLIST_HEADER ListHead
);

XBSYSAPI EXPORTNUM(57) SLIST_ENTRY * FASTCALL KRNL(InterlockedPopEntrySList)
(
	IN PSLIST_HEADER ListHead
);

XBSYSAPI EXPORTNUM(58) SLIST_ENTRY * FASTCALL KRNL(InterlockedPushEntrySList)
(
	IN PSLIST_HEADER ListHead,
	IN PSLIST_ENTRY ListEntry
);

}

#endif


