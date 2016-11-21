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
	IN DWORD Arg1
);

XBSYSAPI EXPORTNUM(13) NTSTATUS NTAPI ExAcquireReadWriteLockShared
(
	IN DWORD Arg1
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
    IN ULONG NumberOfBytes
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

XBSYSAPI EXPORTNUM(16) POBJECT_TYPE ExEventObjectType;

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
	IN DWORD Arg1
);

// Source:APILogger - Uncertain
XBSYSAPI EXPORTNUM(19) LARGE_INTEGER NTAPI ExInterlockedAddLargeInteger
(
	IN OUT PLARGE_INTEGER Addend,
	IN LARGE_INTEGER Increment,
	IN OUT PKSPIN_LOCK Lock
);

// Source:ReactOS
// TODO : fastcall?
XBSYSAPI EXPORTNUM(20) VOID NTAPI ExInterlockedAddLargeStatistic
(
	IN PLARGE_INTEGER Addend,
	IN ULONG Increment
);

// Source:ReactOS
// TODO : fastcall?
XBSYSAPI EXPORTNUM(21) LONGLONG NTAPI ExInterlockedCompareExchange64
(
	IN OUT PLONGLONG Destination,
	IN PLONGLONG Exchange,
	IN PLONGLONG Comparand,
	IN PKSPIN_LOCK Lock
);

XBSYSAPI EXPORTNUM(22) POBJECT_TYPE ExMutantObjectType;

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
	IN DWORD Arg1
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

XBSYSAPI EXPORTNUM(30) POBJECT_TYPE ExSemaphoreObjectType;
XBSYSAPI EXPORTNUM(31) POBJECT_TYPE ExTimerObjectType;

// TODO : fastcall?
XBSYSAPI EXPORTNUM(32) PLIST_ENTRY NTAPI ExfInterlockedInsertHeadList
(
	IN PLIST_ENTRY ListHead,
	IN PLIST_ENTRY ListEntry,
	IN PKSPIN_LOCK Lock
);

// TODO : fastcall?
XBSYSAPI EXPORTNUM(33) PLIST_ENTRY NTAPI ExfInterlockedInsertTailList
(
	IN PLIST_ENTRY ListHead,
	IN PLIST_ENTRY ListEntry,
	IN PKSPIN_LOCK Lock
);

// TODO : fastcall?
XBSYSAPI EXPORTNUM(34) PLIST_ENTRY NTAPI ExfInterlockedRemoveHeadList
(
	IN PKSPIN_LOCK Lock,
	IN PLIST_ENTRY ListHead
);

XBSYSAPI VOID *InterlockedCompareExchange;
XBSYSAPI VOID *InterlockedDecrement;
XBSYSAPI VOID *InterlockedIncrement;
XBSYSAPI VOID *InterlockedExchange;
XBSYSAPI VOID *InterlockedExchangeAdd;
XBSYSAPI VOID *InterlockedFlushSList;
XBSYSAPI VOID *InterlockedPopEntrySList;
XBSYSAPI VOID *InterlockedPushEntrySList;

#endif


