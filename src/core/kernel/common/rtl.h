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

#include "types.h"

namespace xbox
{

// ******************************************************************
// * RtlAnsiStringToUnicodeString
// ******************************************************************
XBSYSAPI EXPORTNUM(260) ntstatus_xt NTAPI RtlAnsiStringToUnicodeString
(
    PUNICODE_STRING DestinationString,
    PSTRING         SourceString,
    uchar_xt           AllocateDestinationString
);

// ******************************************************************
// * RtlAppendStringToString
// ******************************************************************
XBSYSAPI EXPORTNUM(261) ntstatus_xt NTAPI RtlAppendStringToString
(
  IN OUT PSTRING    Destination,
  IN PSTRING    Source
);

XBSYSAPI EXPORTNUM(262) ntstatus_xt NTAPI RtlAppendUnicodeStringToString
(
	IN OUT PUNICODE_STRING Destination,
	IN PUNICODE_STRING Source
);

XBSYSAPI EXPORTNUM(263) ntstatus_xt NTAPI RtlAppendUnicodeToString
(
	IN OUT PUNICODE_STRING Destination,
	IN LPCWSTR Source
);

// ******************************************************************
// * RtlAssert
// ******************************************************************
XBSYSAPI EXPORTNUM(264) void_xt NTAPI RtlAssert
(
    PCHAR   FailedAssertion,
	PCHAR   FileName,
    ulong_xt   LineNumber,
    PCHAR   Message
);

// ******************************************************************
// * 0x0109 - RtlCaptureContext()
// ******************************************************************
XBSYSAPI EXPORTNUM(265) void_xt NTAPI RtlCaptureContext
(
	IN PCONTEXT ContextRecord
);

// ******************************************************************
// * 0x010A - RtlCaptureStackBackTrace()
// ******************************************************************
XBSYSAPI EXPORTNUM(266) ushort_xt NTAPI RtlCaptureStackBackTrace
(
	IN ulong_xt FramesToSkip,
	IN ulong_xt FramesToCapture,
	OUT PVOID *BackTrace,
	OUT PULONG BackTraceHash
);

// ******************************************************************
// * RtlCharToInteger
// ******************************************************************
XBSYSAPI EXPORTNUM(267) ntstatus_xt NTAPI RtlCharToInteger
(
	IN     PCSZ   String,
	IN     ulong_xt  Base OPTIONAL,
	OUT    PULONG Value
);

// ******************************************************************
// * RtlCompareMemory
// ******************************************************************
// *
// * compare block of memory, return number of equivalent bytes.
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(268) size_xt NTAPI RtlCompareMemory
(
  IN CONST void_xt *Source1,
  IN CONST void_xt *Source2,
  IN size_xt      Length
);

// ******************************************************************
// * 0x010D - RtlCompareMemoryUlong()
// ******************************************************************
XBSYSAPI EXPORTNUM(269) size_xt NTAPI RtlCompareMemoryUlong
(
	IN PVOID Source,
	IN size_xt Length,
	IN ulong_xt Pattern
);

// ******************************************************************
// * 0x010E - RtlCompareString()
// ******************************************************************
XBSYSAPI EXPORTNUM(270) long_xt NTAPI RtlCompareString
(
	IN PSTRING String1,
	IN PSTRING String2,
	IN boolean_xt CaseInSensitive
);

// ******************************************************************
// * 0x010F - RtlCompareUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(271) long_xt NTAPI RtlCompareUnicodeString
(
	IN PUNICODE_STRING String1,
	IN PUNICODE_STRING String2,
	IN boolean_xt CaseInSensitive
);

// ******************************************************************
// * 0x0110 - RtlCopyString()
// ******************************************************************
// *
// * Copy Source to Destination
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(272) void_xt NTAPI RtlCopyString
(
	OUT PSTRING DestinationString,
	IN PSTRING SourceString OPTIONAL
);

// ******************************************************************
// * 0x0111 - RtlCopyUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(273) void_xt NTAPI RtlCopyUnicodeString
(
	OUT PUNICODE_STRING DestinationString,
	IN PUNICODE_STRING SourceString OPTIONAL
);

// ******************************************************************
// * 0x0112 - RtlCreateUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(274) boolean_xt NTAPI RtlCreateUnicodeString
(
	OUT PUNICODE_STRING DestinationString,
	IN PCWSTR SourceString
);

// ******************************************************************
// * 0x0113 - RtlDowncaseUnicodeChar()
// ******************************************************************
XBSYSAPI EXPORTNUM(275) wchar_xt NTAPI RtlDowncaseUnicodeChar
(
	IN wchar_xt SourceCharacter
);

// ******************************************************************
// * 0x0114 - RtlDowncaseUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(276) ntstatus_xt NTAPI RtlDowncaseUnicodeString
(
	OUT PUNICODE_STRING DestinationString,
	IN PUNICODE_STRING SourceString,
	IN boolean_xt AllocateDestinationString
);

// ******************************************************************
// * RtlEnterCriticalSection
// ******************************************************************
XBSYSAPI EXPORTNUM(277) void_xt NTAPI RtlEnterCriticalSection
(
  IN PRTL_CRITICAL_SECTION CriticalSection
);

// ******************************************************************
// * 0x0116 - RtlEnterCriticalSectionAndRegion()
// ******************************************************************
XBSYSAPI EXPORTNUM(278) void_xt NTAPI RtlEnterCriticalSectionAndRegion
(
	IN PRTL_CRITICAL_SECTION CriticalSection
);

// ******************************************************************
// * 0x0117 - RtlEqualString()
// ******************************************************************
XBSYSAPI EXPORTNUM(279) boolean_xt NTAPI RtlEqualString
(
  IN PSTRING String1,
  IN PSTRING String2,
  IN boolean_xt CaseInsensitive
);

// ******************************************************************
// * 0x0118 - RtlEqualUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(280) boolean_xt NTAPI RtlEqualUnicodeString
(
	IN PUNICODE_STRING String1,
	IN PUNICODE_STRING String2,
	IN boolean_xt CaseInSensitive
);

// ******************************************************************
// * 0x0119 - RtlExtendedIntegerMultiply()
// ******************************************************************
XBSYSAPI EXPORTNUM(281) LARGE_INTEGER NTAPI RtlExtendedIntegerMultiply
(
	IN LARGE_INTEGER Multiplicand,
	IN long_xt Multiplier
);

// ******************************************************************
// * 0x011A - RtlExtendedLargeIntegerDivide()
// ******************************************************************
XBSYSAPI EXPORTNUM(282) LARGE_INTEGER NTAPI RtlExtendedLargeIntegerDivide
(
	IN	LARGE_INTEGER Dividend,
	IN	ulong_xt Divisor,
	IN	PULONG Remainder // OUT? OPTIONAL?
);

// ******************************************************************
// * 0x011B - RtlExtendedMagicDivide()
// ******************************************************************
XBSYSAPI EXPORTNUM(283) LARGE_INTEGER NTAPI RtlExtendedMagicDivide
(
	IN	LARGE_INTEGER Dividend,
	IN	LARGE_INTEGER MagicDivisor,
	IN	cchar_xt ShiftCount
);

// ******************************************************************
// * 0x011C - RtlFillMemory()
// ******************************************************************
XBSYSAPI EXPORTNUM(284) void_xt NTAPI RtlFillMemory
(
	IN void_xt UNALIGNED *Destination,
	IN dword_xt Length,
	IN byte_xt  Fill
);

// ******************************************************************
// * 0x011D - RtlFillMemoryUlong()
// ******************************************************************
XBSYSAPI EXPORTNUM(285) void_xt NTAPI RtlFillMemoryUlong
(
	IN PVOID Destination,
	IN size_xt Length,
	IN ulong_xt Pattern
);

// ******************************************************************
// * RtlFreeAnsiString
// ******************************************************************
XBSYSAPI EXPORTNUM(286) void_xt NTAPI RtlFreeAnsiString
(
  IN OUT PANSI_STRING AnsiString
);

// ******************************************************************
// * 0x011F - RtlFreeUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(287) void_xt NTAPI RtlFreeUnicodeString
(
	IN OUT PUNICODE_STRING UnicodeString
);

// ******************************************************************
// * 0x0120 - RtlGetCallersAddress()
// ******************************************************************
XBSYSAPI EXPORTNUM(288) void_xt NTAPI RtlGetCallersAddress
(
	OUT PVOID *CallersAddress,
	OUT PVOID *CallersCaller
);

// ******************************************************************
// * RtlInitAnsiString
// ******************************************************************
// *
// * Initialize a counted ANSI string.
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(289) void_xt NTAPI RtlInitAnsiString
(
  IN OUT PANSI_STRING DestinationString,
  IN     PCSZ         SourceString
);

XBSYSAPI EXPORTNUM(290) void_xt NTAPI RtlInitUnicodeString
(
  IN OUT PUNICODE_STRING DestinationString,
  IN     PCWSTR          SourceString
);

// ******************************************************************
// * RtlInitializeCriticalSection
// ******************************************************************
XBSYSAPI EXPORTNUM(291) void_xt NTAPI RtlInitializeCriticalSection
(
  IN PRTL_CRITICAL_SECTION CriticalSection
);

// ******************************************************************
// * 0x0124 - RtlIntegerToChar()
// ******************************************************************
XBSYSAPI EXPORTNUM(292) ntstatus_xt NTAPI RtlIntegerToChar
(
	IN ulong_xt Value,
	IN ulong_xt Base,
	IN long_xt OutputLength,
	IN PSZ String
);

// ******************************************************************
// * 0x0125 - RtlIntegerToUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(293) ntstatus_xt NTAPI RtlIntegerToUnicodeString
(
	IN     ulong_xt Value,
	IN     ulong_xt Base,
	IN     PUNICODE_STRING String
);

// ******************************************************************
// * RtlLeaveCriticalSection
// ******************************************************************
XBSYSAPI EXPORTNUM(294) void_xt NTAPI RtlLeaveCriticalSection
(
  IN PRTL_CRITICAL_SECTION CriticalSection
);

// ******************************************************************
// * 0x0127 - RtlLeaveCriticalSectionAndRegion()
// ******************************************************************
XBSYSAPI EXPORTNUM(295) void_xt NTAPI RtlLeaveCriticalSectionAndRegion
(
	IN PRTL_CRITICAL_SECTION CriticalSection
);

// ******************************************************************
// * RtlLowerChar
// ******************************************************************
XBSYSAPI EXPORTNUM(296) char_xt NTAPI RtlLowerChar(char_xt Character);

// ******************************************************************
// * 0x0129 - RtlMapGenericMask()
// ******************************************************************
XBSYSAPI EXPORTNUM(297) void_xt NTAPI RtlMapGenericMask
(
	IN PACCESS_MASK AccessMask,
	IN PGENERIC_MAPPING GenericMapping
);

// ******************************************************************
// * 0x012A - RtlMoveMemory()
// ******************************************************************
// *
// * Move memory either forward or backward, aligned or unaligned,
// * in 4-byte blocks, followed by any remaining blocks.
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(298) void_xt NTAPI RtlMoveMemory
(
  IN void_xt UNALIGNED       *Destination,
  IN CONST void_xt UNALIGNED *Source,
  IN size_xt                Length
);

// ******************************************************************
// * 0x012B - RtlMultiByteToUnicodeN()
// ******************************************************************
XBSYSAPI EXPORTNUM(299) ntstatus_xt NTAPI RtlMultiByteToUnicodeN
(
	IN     PWSTR UnicodeString,
	IN     ulong_xt MaxBytesInUnicodeString,
	IN     PULONG BytesInUnicodeString,
	IN     PCHAR MultiByteString,
	IN     ulong_xt BytesInMultiByteString
);

// ******************************************************************
// * 0x012C - RtlMultiByteToUnicodeSize()
// ******************************************************************
XBSYSAPI EXPORTNUM(300) ntstatus_xt NTAPI RtlMultiByteToUnicodeSize
(
	IN PULONG BytesInUnicodeString,
	IN PCHAR MultiByteString,
	IN ulong_xt BytesInMultiByteString
);

// ******************************************************************
// * RtlNtStatusToDosError
// ******************************************************************
XBSYSAPI EXPORTNUM(301) ulong_xt NTAPI RtlNtStatusToDosError
(
    IN ntstatus_xt Status
);

// ******************************************************************
// * 0x012E - RtlRaiseException()
// ******************************************************************
XBSYSAPI EXPORTNUM(302) void_xt NTAPI RtlRaiseException
(
	IN PEXCEPTION_RECORD ExceptionRecord
);

// ******************************************************************
// * 0x012F - RtlRaiseStatus()
// ******************************************************************
XBSYSAPI EXPORTNUM(303) void_xt NTAPI RtlRaiseStatus
(
	IN ntstatus_xt Status
);

// ******************************************************************
// * RtlTimeFieldsToTime
// ******************************************************************
XBSYSAPI EXPORTNUM(304) boolean_xt NTAPI RtlTimeFieldsToTime
(
    IN  PTIME_FIELDS    TimeFields,
    OUT PLARGE_INTEGER  Time
);

// ******************************************************************
// * RtlTimeToTimeFields
// ******************************************************************
XBSYSAPI EXPORTNUM(305) void_xt NTAPI RtlTimeToTimeFields
(
    IN  PLARGE_INTEGER  Time,
    OUT PTIME_FIELDS    TimeFields
);

// ******************************************************************
// * RtlTryEnterCriticalSection
// ******************************************************************
XBSYSAPI EXPORTNUM(306) boolean_xt NTAPI RtlTryEnterCriticalSection
(
    IN PRTL_CRITICAL_SECTION CriticalSection
);

// ******************************************************************
// * 0x0133 - RtlUlongByteSwap()
// ******************************************************************
XBSYSAPI EXPORTNUM(307) ulong_xt FASTCALL RtlUlongByteSwap
(
	IN ulong_xt Source
);

// ******************************************************************
// * RtlUnicodeStringToAnsiString
// ******************************************************************
XBSYSAPI EXPORTNUM(308) ntstatus_xt NTAPI RtlUnicodeStringToAnsiString
(
    IN OUT PSTRING         DestinationString,
    IN     PUNICODE_STRING SourceString,
    IN     boolean_xt         AllocateDestinationString
);

// ******************************************************************
// * 0x0135 - RtlUnicodeStringToInteger()
// ******************************************************************
XBSYSAPI EXPORTNUM(309) ntstatus_xt NTAPI RtlUnicodeStringToInteger
(
	IN     PUNICODE_STRING String,
	IN     ulong_xt Base,
	IN     PULONG Value
);

// ******************************************************************
// * 0x0136 - RtlUnicodeToMultiByteN()
// ******************************************************************
XBSYSAPI EXPORTNUM(310) ntstatus_xt NTAPI RtlUnicodeToMultiByteN
(
	IN PCHAR MultiByteString,
	IN ulong_xt MaxBytesInMultiByteString,
	IN PULONG BytesInMultiByteString,
	IN PWSTR UnicodeString,
	IN ulong_xt BytesInUnicodeString
);

// ******************************************************************
// * 0x0137 - RtlUnicodeToMultiByteSize()
// ******************************************************************
XBSYSAPI EXPORTNUM(311) ntstatus_xt NTAPI RtlUnicodeToMultiByteSize
(
	IN PULONG BytesInMultiByteString,
	IN PWSTR UnicodeString,
	IN ulong_xt BytesInUnicodeString
);

// ******************************************************************
// * 0x0138 - RtlUnwind()
// ******************************************************************
XBSYSAPI EXPORTNUM(312) void_xt NTAPI RtlUnwind
(
	IN PVOID TargetFrame OPTIONAL,
	IN PVOID TargetIp OPTIONAL,
	IN PEXCEPTION_RECORD ExceptionRecord OPTIONAL,
	IN PVOID ReturnValue
);

// ******************************************************************
// * 0x0139 - RtlUpcaseUnicodeChar()
// ******************************************************************
XBSYSAPI EXPORTNUM(313) wchar_xt NTAPI RtlUpcaseUnicodeChar
(
	IN wchar_xt SourceCharacter
);

// ******************************************************************
// * 0x013A - RtlUpcaseUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(314) ntstatus_xt NTAPI RtlUpcaseUnicodeString
(
	OUT PUNICODE_STRING DestinationString,
	IN  PUNICODE_STRING SourceString,
	IN  boolean_xt AllocateDestinationString
);

// ******************************************************************
// * 0x013B - RtlUpcaseUnicodeToMultiByteN()
// ******************************************************************
XBSYSAPI EXPORTNUM(315) ntstatus_xt NTAPI RtlUpcaseUnicodeToMultiByteN
(
	IN OUT PCHAR MultiByteString,
	IN ulong_xt MaxBytesInMultiByteString,
	IN PULONG BytesInMultiByteString,
	IN PWSTR UnicodeString,
	IN ulong_xt BytesInUnicodeString
);

// ******************************************************************
// * 0x013C - RtlUpperChar()
// ******************************************************************
XBSYSAPI EXPORTNUM(316) char_xt NTAPI RtlUpperChar
(
	char_xt Character
);

// ******************************************************************
// * 0x013D - RtlUpperString()
// ******************************************************************
XBSYSAPI EXPORTNUM(317) void_xt NTAPI RtlUpperString
(
	OUT PSTRING DestinationString,
	IN  PSTRING SourceString
);

// ******************************************************************
// * 0x013E - RtlUshortByteSwap()
// ******************************************************************
XBSYSAPI EXPORTNUM(318) ushort_xt FASTCALL RtlUshortByteSwap
(
	IN ushort_xt Source
);

// ******************************************************************
// * 0x013F - RtlWalkFrameChain
// ******************************************************************
XBSYSAPI EXPORTNUM(319) ulong_xt NTAPI RtlWalkFrameChain
(
	OUT PVOID *Callers,
	IN ulong_xt Count,
	IN ulong_xt Flags
);

// ******************************************************************
// * 0x0140 - RtlZeroMemory
// ******************************************************************
// *
// * Fill a block of memory with zeros.
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(320) void_xt NTAPI RtlZeroMemory
(
  IN void_xt UNALIGNED  *Destination,
  IN size_xt           Length
);

// ******************************************************************
// * 0x0160 - RtlRip
// ******************************************************************
XBSYSAPI EXPORTNUM(352) void_xt NTAPI RtlRip
(
 PCHAR	ApiName,
 PCHAR	Expression,
 PCHAR	Message
);

void_xt RtlInitSystem();
extern RTL_CRITICAL_SECTION NtSystemTimeCritSec;

}

#endif


