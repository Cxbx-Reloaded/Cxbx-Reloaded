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

XBSYSAPI EXPORTNUM(12) NTSTATUS NTAPI ExAcquireReadWriteLockExclusive
(
	IN PERWLOCK ReadWriteLock
);

XBSYSAPI EXPORTNUM(13) NTSTATUS NTAPI ExAcquireReadWriteLockShared
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
    IN SIZE_T NumberOfBytes
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
    IN SIZE_T NumberOfBytes,
    IN ULONG Tag
);

XBSYSAPI EXPORTNUM(16) OBJECT_TYPE ExEventObjectType;

// ******************************************************************
// * ExFreePool
// ******************************************************************
// *
// * Deallocates a block  of pool memory
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(17) VOID NTAPI ExFreePool
(
    IN PVOID P
);

XBSYSAPI EXPORTNUM(18) NTSTATUS NTAPI ExInitializeReadWriteLock
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
XBSYSAPI EXPORTNUM(20) VOID FASTCALL ExInterlockedAddLargeStatistic
(
	IN PLARGE_INTEGER Addend,
	IN ULONG Increment
);

// Source:ReactOS
XBSYSAPI EXPORTNUM(21) LONGLONG FASTCALL ExInterlockedCompareExchange64
(
	IN OUT PLONGLONG Destination,
	IN PLONGLONG Exchange,
	IN PLONGLONG Comparand,
	IN PKSPIN_LOCK Lock
);

XBSYSAPI EXPORTNUM(22) OBJECT_TYPE ExMutantObjectType;

// ******************************************************************
// * ExQueryPoolBlockSize
// ******************************************************************
XBSYSAPI EXPORTNUM(23) ULONG NTAPI ExQueryPoolBlockSize
(
    IN PVOID PoolBlock
);

// ******************************************************************
// * ExQueryNonVolatileSetting
// ******************************************************************
XBSYSAPI EXPORTNUM(24) NTSTATUS NTAPI ExQueryNonVolatileSetting
(
    IN  EEPROM_INDEX        ValueIndex,
    OUT DWORD              *Type,
    OUT PUCHAR              Value,
    IN  SIZE_T              ValueLength,
    OUT PSIZE_T             ResultLength OPTIONAL
);

// ******************************************************************
// * ExReadWriteRefurbInfo
// ******************************************************************
XBSYSAPI EXPORTNUM(25) NTSTATUS NTAPI ExReadWriteRefurbInfo
(
	IN OUT PXBOX_REFURB_INFO	pRefurbInfo,
	IN ULONG	dwBufferSize,
	IN BOOLEAN	bIsWriteMode
);

XBSYSAPI EXPORTNUM(26) VOID NTAPI ExRaiseException
(
	IN PEXCEPTION_RECORD ExceptionRecord
);

XBSYSAPI EXPORTNUM(27) VOID NTAPI ExRaiseStatus
(
	IN NTSTATUS Status
);

XBSYSAPI EXPORTNUM(28) NTSTATUS NTAPI ExReleaseReadWriteLock
(
	IN PERWLOCK ReadWriteLock
);

// ******************************************************************
// * ExSaveNonVolatileSetting
// ******************************************************************
XBSYSAPI EXPORTNUM(29) NTSTATUS NTAPI ExSaveNonVolatileSetting
(
    IN  DWORD               ValueIndex,
    OUT DWORD              *Type,
    IN  PUCHAR              Value,
    IN  SIZE_T              ValueLength
);

XBSYSAPI EXPORTNUM(30) OBJECT_TYPE ExSemaphoreObjectType;

XBSYSAPI EXPORTNUM(31) OBJECT_TYPE ExTimerObjectType;

XBSYSAPI EXPORTNUM(32) PLIST_ENTRY FASTCALL ExfInterlockedInsertHeadList
(
	IN PLIST_ENTRY ListHead,
	IN PLIST_ENTRY ListEntry,
	IN PKSPIN_LOCK Lock
);

XBSYSAPI EXPORTNUM(33) PLIST_ENTRY FASTCALL ExfInterlockedInsertTailList
(
	IN PLIST_ENTRY ListHead,
	IN PLIST_ENTRY ListEntry,
	IN PKSPIN_LOCK Lock
);

XBSYSAPI EXPORTNUM(34) PLIST_ENTRY FASTCALL ExfInterlockedRemoveHeadList
(
	IN PLIST_ENTRY ListHead,
	IN PKSPIN_LOCK Lock
);

XBSYSAPI EXPORTNUM(51) LONG FASTCALL KRNL(InterlockedCompareExchange)
(
	IN OUT volatile PLONG Destination,
	IN LONG  Exchange,
	IN LONG  Comparand
);

XBSYSAPI EXPORTNUM(52) LONG FASTCALL KRNL(InterlockedDecrement)
(
	IN OUT PLONG Addend
);

XBSYSAPI EXPORTNUM(53) LONG FASTCALL KRNL(InterlockedIncrement)
(
	IN OUT PLONG Addend
);

XBSYSAPI EXPORTNUM(54) LONG FASTCALL KRNL(InterlockedExchange)
(
	IN volatile PLONG Destination,
	IN LONG Value
);

XBSYSAPI EXPORTNUM(55) LONG FASTCALL KRNL(InterlockedExchangeAdd)
(
	IN volatile PLONG Addend,
	IN LONG	Value
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

#endif


