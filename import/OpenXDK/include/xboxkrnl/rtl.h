// ******************************************************************
// *
// * proj : OpenXDK
// *
// * desc : Open Source XBox Development Kit
// *
// * file : ps.h
// *
// * note : XBox Kernel *Run-time Library* Declarations
// *
// ******************************************************************
#ifndef XBOXKRNL_RTL_H
#define XBOXKRNL_RTL_H

// ******************************************************************
// * RtlAnsiStringToUnicodeString
// ******************************************************************
XBSYSAPI EXPORTNUM(260) NTSTATUS NTAPI RtlAnsiStringToUnicodeString
(
    PUNICODE_STRING DestinationString,
    PSTRING         SourceString,
    UCHAR           AllocateDestinationString
);

// ******************************************************************
// * RtlAppendStringToString
// ******************************************************************
XBSYSAPI EXPORTNUM(261) NTSTATUS NTAPI RtlAppendStringToString
(
  IN OUT PSTRING    Destination,
  IN PSTRING    Source
);

XBSYSAPI EXPORTNUM(262) NTSTATUS NTAPI RtlAppendUnicodeStringToString
(
	IN OUT PUNICODE_STRING Destination,
	IN PUNICODE_STRING Source
);

XBSYSAPI EXPORTNUM(263) NTSTATUS NTAPI RtlAppendUnicodeToString
(
	IN OUT PUNICODE_STRING Destination,
	IN LPCWSTR Source
);

// ******************************************************************
// * RtlAssert
// ******************************************************************
XBSYSAPI EXPORTNUM(264) VOID NTAPI RtlAssert
(
    PVOID   FailedAssertion,
    PVOID   FileName,
    ULONG   LineNumber,
    PCHAR   Message
);

XBSYSAPI VOID *RtlCaptureContext;
XBSYSAPI VOID *RtlCaptureStackBackTrace;

// ******************************************************************
// * RtlCharToInteger
// ******************************************************************
XBSYSAPI EXPORTNUM(267) NTSTATUS NTAPI RtlCharToInteger
(
	IN     PCSZ   String,
	IN     ULONG  Base OPTIONAL,
	OUT    PULONG Value
);

// ******************************************************************
// * RtlCompareMemory
// ******************************************************************
// *
// * compare block of memory, return number of equivalent bytes.
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(268) SIZE_T NTAPI RtlCompareMemory
(
  IN CONST VOID *Source1,
  IN CONST VOID *Source2,
  IN SIZE_T      Length
);

XBSYSAPI VOID *RtlCompareMemoryUlong;
XBSYSAPI VOID *RtlCompareString;
XBSYSAPI VOID *RtlCompareUnicodeString;

// ******************************************************************
// * RtlCopyString
// ******************************************************************
// *
// * Copy Source to Destination
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(272) VOID NTAPI RtlCopyString
(
  IN OUT PVOID  Destination,        // TODO: should be STRING
  IN     PVOID  Source OPTIONAL     // TODO: should be STRING
);

XBSYSAPI VOID *RtlCopyUnicodeString;
XBSYSAPI VOID *RtlCreateUnicodeString;
XBSYSAPI VOID *RtlDowncaseUnicodeChar;
XBSYSAPI VOID *RtlDowncaseUnicodeString;

// ******************************************************************
// * RtlEnterCriticalSection
// ******************************************************************
XBSYSAPI EXPORTNUM(277) VOID NTAPI RtlEnterCriticalSection
(
  IN PRTL_CRITICAL_SECTION CriticalSection
);

XBSYSAPI VOID *RtlEnterCriticalSectionAndRegion;

// ******************************************************************
// * RtlEnterCriticalSection
// ******************************************************************
XBSYSAPI EXPORTNUM(279) BOOLEAN NTAPI RtlEqualString
(
  IN PSTRING String1,
  IN PSTRING String2,
  IN BOOLEAN CaseSensitive
);

XBSYSAPI VOID *RtlEqualUnicodeString;
XBSYSAPI VOID *RtlExtendedIntegerMultiply;
XBSYSAPI VOID *RtlExtendedLargeIntegerDivide;
XBSYSAPI VOID *RtlExtendedMagicDivide;
XBSYSAPI VOID *RtlFillMemory;
XBSYSAPI VOID *RtlFillMemoryUlong;

// ******************************************************************
// * RtlFreeAnsiString
// ******************************************************************
XBSYSAPI EXPORTNUM(286) VOID NTAPI RtlFreeAnsiString
(
  IN OUT PANSI_STRING AnsiString
);

XBSYSAPI VOID *RtlFreeUnicodeString;
XBSYSAPI VOID *RtlGetCallersAddress;

// ******************************************************************
// * RtlInitAnsiString
// ******************************************************************
// *
// * Initialize a counted ANSI string.
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(289) VOID NTAPI RtlInitAnsiString
(
  IN OUT PANSI_STRING DestinationString,
  IN     PCSZ         SourceString
);

XBSYSAPI EXPORTNUM(290) VOID NTAPI RtlInitUnicodeString
(
  IN OUT PUNICODE_STRING DestinationString,
  IN     PSTRING         SourceString
);

// ******************************************************************
// * RtlInitializeCriticalSection
// ******************************************************************
XBSYSAPI EXPORTNUM(291) VOID NTAPI RtlInitializeCriticalSection
(
  IN PRTL_CRITICAL_SECTION CriticalSection
);

XBSYSAPI VOID *RtlIntegerToChar;
XBSYSAPI VOID *RtlIntegerToUnicodeString;

// ******************************************************************
// * RtlLeaveCriticalSection
// ******************************************************************
XBSYSAPI EXPORTNUM(294) VOID NTAPI RtlLeaveCriticalSection
(
  IN PRTL_CRITICAL_SECTION CriticalSection
);

XBSYSAPI VOID *RtlLeaveCriticalSectionAndRegion;

// ******************************************************************
// * RtlLowerChar
// ******************************************************************
XBSYSAPI EXPORTNUM(296) CHAR NTAPI RtlLowerChar(CHAR Character);

XBSYSAPI VOID *RtlMapGenericMask;

// ******************************************************************
// * RtlMoveMemory
// ******************************************************************
// *
// * Move memory either forward or backward, aligned or unaligned,
// * in 4-byte blocks, followed by any remaining blocks.
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(298) VOID NTAPI RtlMoveMemory
(
  IN VOID UNALIGNED       *Destination,
  IN CONST VOID UNALIGNED *Source,
  IN SIZE_T                Length
);

XBSYSAPI VOID *RtlMultiByteToUnicodeN;
XBSYSAPI VOID *RtlMultiByteToUnicodeSize;

// ******************************************************************
// * RtlNtStatusToDosError
// ******************************************************************
XBSYSAPI EXPORTNUM(301) ULONG NTAPI RtlNtStatusToDosError
(
    IN NTSTATUS Status
);

XBSYSAPI VOID *RtlRaiseException;
XBSYSAPI VOID *RtlRaiseStatus;

// ******************************************************************
// * RtlTimeFieldsToTime
// ******************************************************************
XBSYSAPI EXPORTNUM(304) BOOLEAN NTAPI RtlTimeFieldsToTime
(
    IN  PTIME_FIELDS    TimeFields,
    OUT PLARGE_INTEGER  Time
);

// ******************************************************************
// * RtlTimeToTimeFields
// ******************************************************************
XBSYSAPI EXPORTNUM(305) VOID NTAPI RtlTimeToTimeFields
(
    IN  PLARGE_INTEGER  Time,
    OUT PTIME_FIELDS    TimeFields
);

// ******************************************************************
// * RtlTryEnterCriticalSection
// ******************************************************************
XBSYSAPI EXPORTNUM(306) BOOLEAN NTAPI RtlTryEnterCriticalSection
(
    IN PRTL_CRITICAL_SECTION CriticalSection
);

XBSYSAPI VOID *RtlUlongByteSwap;

// ******************************************************************
// * RtlUnicodeStringToAnsiString
// ******************************************************************
XBSYSAPI EXPORTNUM(308) NTSTATUS NTAPI RtlUnicodeStringToAnsiString
(
    IN OUT PSTRING         DestinationString,
    IN     PUNICODE_STRING SourceString,
    IN     BOOLEAN         AllocateDestinationString
);

XBSYSAPI VOID *RtlUnicodeStringToInteger;
XBSYSAPI VOID *RtlUnicodeToMultiByteN;
XBSYSAPI VOID *RtlUnicodeToMultiByteSize;
XBSYSAPI VOID *RtlUnwind;
XBSYSAPI VOID *RtlUpcaseUnicodeChar;
XBSYSAPI VOID *RtlUpcaseUnicodeString;
XBSYSAPI VOID *RtlUpcaseUnicodeToMultiByteN;
XBSYSAPI VOID *RtlUpperChar;
XBSYSAPI VOID *RtlUpperString;
XBSYSAPI VOID *RtlUshortByteSwap;
XBSYSAPI VOID *RtlWalkFrameChain;

// ******************************************************************
// * RtlZeroMemory
// ******************************************************************
// *
// * Fill a block of memory with zeros.
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(320) VOID NTAPI RtlZeroMemory
(
  IN VOID UNALIGNED  *Destination,
  IN SIZE_T           Length
);

// ******************************************************************
// * 0x0160 - RtlRip
// ******************************************************************
XBSYSAPI EXPORTNUM(352) VOID NTAPI RtlRip
(
 PCHAR	ApiName,
 PCHAR	Expression,
 PCHAR	Message
);

#endif


