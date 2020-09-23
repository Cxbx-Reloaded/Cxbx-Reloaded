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
XBSYSAPI EXPORTNUM(264) void_t NTAPI RtlAssert
(
    PCHAR   FailedAssertion,
	PCHAR   FileName,
    ULONG   LineNumber,
    PCHAR   Message
);

// ******************************************************************
// * 0x0109 - RtlCaptureContext()
// ******************************************************************
XBSYSAPI EXPORTNUM(265) void_t NTAPI RtlCaptureContext
(
	IN PCONTEXT ContextRecord
);

// ******************************************************************
// * 0x010A - RtlCaptureStackBackTrace()
// ******************************************************************
XBSYSAPI EXPORTNUM(266) USHORT NTAPI RtlCaptureStackBackTrace
(
	IN ULONG FramesToSkip,
	IN ULONG FramesToCapture,
	OUT PVOID *BackTrace,
	OUT PULONG BackTraceHash
);

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
  IN CONST void_t *Source1,
  IN CONST void_t *Source2,
  IN SIZE_T      Length
);

// ******************************************************************
// * 0x010D - RtlCompareMemoryUlong()
// ******************************************************************
XBSYSAPI EXPORTNUM(269) SIZE_T NTAPI RtlCompareMemoryUlong
(
	IN PVOID Source,
	IN SIZE_T Length,
	IN ULONG Pattern
);

// ******************************************************************
// * 0x010E - RtlCompareString()
// ******************************************************************
XBSYSAPI EXPORTNUM(270) LONG NTAPI RtlCompareString
(
	IN PSTRING String1,
	IN PSTRING String2,
	IN BOOLEAN CaseInSensitive
);

// ******************************************************************
// * 0x010F - RtlCompareUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(271) LONG NTAPI RtlCompareUnicodeString
(
	IN PUNICODE_STRING String1,
	IN PUNICODE_STRING String2,
	IN BOOLEAN CaseInSensitive
);

// ******************************************************************
// * 0x0110 - RtlCopyString()
// ******************************************************************
// *
// * Copy Source to Destination
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(272) void_t NTAPI RtlCopyString
(
	OUT PSTRING DestinationString,
	IN PSTRING SourceString OPTIONAL
);

// ******************************************************************
// * 0x0111 - RtlCopyUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(273) void_t NTAPI RtlCopyUnicodeString
(
	OUT PUNICODE_STRING DestinationString,
	IN PUNICODE_STRING SourceString OPTIONAL
);

// ******************************************************************
// * 0x0112 - RtlCreateUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(274) BOOLEAN NTAPI RtlCreateUnicodeString
(
	OUT PUNICODE_STRING DestinationString,
	IN PCWSTR SourceString
);

// ******************************************************************
// * 0x0113 - RtlDowncaseUnicodeChar()
// ******************************************************************
XBSYSAPI EXPORTNUM(275) WCHAR NTAPI RtlDowncaseUnicodeChar
(
	IN WCHAR SourceCharacter
);

// ******************************************************************
// * 0x0114 - RtlDowncaseUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(276) NTSTATUS NTAPI RtlDowncaseUnicodeString
(
	OUT PUNICODE_STRING DestinationString,
	IN PUNICODE_STRING SourceString,
	IN BOOLEAN AllocateDestinationString
);

// ******************************************************************
// * RtlEnterCriticalSection
// ******************************************************************
XBSYSAPI EXPORTNUM(277) void_t NTAPI RtlEnterCriticalSection
(
  IN PRTL_CRITICAL_SECTION CriticalSection
);

// ******************************************************************
// * 0x0116 - RtlEnterCriticalSectionAndRegion()
// ******************************************************************
XBSYSAPI EXPORTNUM(278) void_t NTAPI RtlEnterCriticalSectionAndRegion
(
	IN PRTL_CRITICAL_SECTION CriticalSection
);

// ******************************************************************
// * 0x0117 - RtlEqualString()
// ******************************************************************
XBSYSAPI EXPORTNUM(279) BOOLEAN NTAPI RtlEqualString
(
  IN PSTRING String1,
  IN PSTRING String2,
  IN BOOLEAN CaseInsensitive
);

// ******************************************************************
// * 0x0118 - RtlEqualUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(280) BOOLEAN NTAPI RtlEqualUnicodeString
(
	IN PUNICODE_STRING String1,
	IN PUNICODE_STRING String2,
	IN BOOLEAN CaseInSensitive
);

// ******************************************************************
// * 0x0119 - RtlExtendedIntegerMultiply()
// ******************************************************************
XBSYSAPI EXPORTNUM(281) LARGE_INTEGER NTAPI RtlExtendedIntegerMultiply
(
	IN LARGE_INTEGER Multiplicand,
	IN LONG Multiplier
);

// ******************************************************************
// * 0x011A - RtlExtendedLargeIntegerDivide()
// ******************************************************************
XBSYSAPI EXPORTNUM(282) LARGE_INTEGER NTAPI RtlExtendedLargeIntegerDivide
(
	IN	LARGE_INTEGER Dividend,
	IN	ULONG Divisor,
	IN	PULONG Remainder // OUT? OPTIONAL?
);

// ******************************************************************
// * 0x011B - RtlExtendedMagicDivide()
// ******************************************************************
XBSYSAPI EXPORTNUM(283) LARGE_INTEGER NTAPI RtlExtendedMagicDivide
(
	IN	LARGE_INTEGER Dividend,
	IN	LARGE_INTEGER MagicDivisor,
	IN	CCHAR ShiftCount
);

// ******************************************************************
// * 0x011C - RtlFillMemory()
// ******************************************************************
XBSYSAPI EXPORTNUM(284) void_t NTAPI RtlFillMemory
(
	IN void_t UNALIGNED *Destination,
	IN DWORD Length,
	IN BYTE  Fill
);

// ******************************************************************
// * 0x011D - RtlFillMemoryUlong()
// ******************************************************************
XBSYSAPI EXPORTNUM(285) void_t NTAPI RtlFillMemoryUlong
(
	IN PVOID Destination,
	IN SIZE_T Length,
	IN ULONG Pattern
);

// ******************************************************************
// * RtlFreeAnsiString
// ******************************************************************
XBSYSAPI EXPORTNUM(286) void_t NTAPI RtlFreeAnsiString
(
  IN OUT PANSI_STRING AnsiString
);

// ******************************************************************
// * 0x011F - RtlFreeUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(287) void_t NTAPI RtlFreeUnicodeString
(
	IN OUT PUNICODE_STRING UnicodeString
);

// ******************************************************************
// * 0x0120 - RtlGetCallersAddress()
// ******************************************************************
XBSYSAPI EXPORTNUM(288) void_t NTAPI RtlGetCallersAddress
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
XBSYSAPI EXPORTNUM(289) void_t NTAPI RtlInitAnsiString
(
  IN OUT PANSI_STRING DestinationString,
  IN     PCSZ         SourceString
);

XBSYSAPI EXPORTNUM(290) void_t NTAPI RtlInitUnicodeString
(
  IN OUT PUNICODE_STRING DestinationString,
  IN     PCWSTR          SourceString
);

// ******************************************************************
// * RtlInitializeCriticalSection
// ******************************************************************
XBSYSAPI EXPORTNUM(291) void_t NTAPI RtlInitializeCriticalSection
(
  IN PRTL_CRITICAL_SECTION CriticalSection
);

// ******************************************************************
// * 0x0124 - RtlIntegerToChar()
// ******************************************************************
XBSYSAPI EXPORTNUM(292) NTSTATUS NTAPI RtlIntegerToChar
(
	IN ULONG Value,
	IN ULONG Base,
	IN LONG OutputLength,
	IN PSZ String
);

// ******************************************************************
// * 0x0125 - RtlIntegerToUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(293) NTSTATUS NTAPI RtlIntegerToUnicodeString
(
	IN     ULONG Value,
	IN     ULONG Base,
	IN     PUNICODE_STRING String
);

// ******************************************************************
// * RtlLeaveCriticalSection
// ******************************************************************
XBSYSAPI EXPORTNUM(294) void_t NTAPI RtlLeaveCriticalSection
(
  IN PRTL_CRITICAL_SECTION CriticalSection
);

// ******************************************************************
// * 0x0127 - RtlLeaveCriticalSectionAndRegion()
// ******************************************************************
XBSYSAPI EXPORTNUM(295) void_t NTAPI RtlLeaveCriticalSectionAndRegion
(
	IN PRTL_CRITICAL_SECTION CriticalSection
);

// ******************************************************************
// * RtlLowerChar
// ******************************************************************
XBSYSAPI EXPORTNUM(296) CHAR NTAPI RtlLowerChar(CHAR Character);

// ******************************************************************
// * 0x0129 - RtlMapGenericMask()
// ******************************************************************
XBSYSAPI EXPORTNUM(297) void_t NTAPI RtlMapGenericMask
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
XBSYSAPI EXPORTNUM(298) void_t NTAPI RtlMoveMemory
(
  IN void_t UNALIGNED       *Destination,
  IN CONST void_t UNALIGNED *Source,
  IN SIZE_T                Length
);

// ******************************************************************
// * 0x012B - RtlMultiByteToUnicodeN()
// ******************************************************************
XBSYSAPI EXPORTNUM(299) NTSTATUS NTAPI RtlMultiByteToUnicodeN
(
	IN     PWSTR UnicodeString,
	IN     ULONG MaxBytesInUnicodeString,
	IN     PULONG BytesInUnicodeString,
	IN     PCHAR MultiByteString,
	IN     ULONG BytesInMultiByteString
);

// ******************************************************************
// * 0x012C - RtlMultiByteToUnicodeSize()
// ******************************************************************
XBSYSAPI EXPORTNUM(300) NTSTATUS NTAPI RtlMultiByteToUnicodeSize
(
	IN PULONG BytesInUnicodeString,
	IN PCHAR MultiByteString,
	IN ULONG BytesInMultiByteString
);

// ******************************************************************
// * RtlNtStatusToDosError
// ******************************************************************
XBSYSAPI EXPORTNUM(301) ULONG NTAPI RtlNtStatusToDosError
(
    IN NTSTATUS Status
);

// ******************************************************************
// * 0x012E - RtlRaiseException()
// ******************************************************************
XBSYSAPI EXPORTNUM(302) void_t NTAPI RtlRaiseException
(
	IN PEXCEPTION_RECORD ExceptionRecord
);

// ******************************************************************
// * 0x012F - RtlRaiseStatus()
// ******************************************************************
XBSYSAPI EXPORTNUM(303) void_t NTAPI RtlRaiseStatus
(
	IN NTSTATUS Status
);

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
XBSYSAPI EXPORTNUM(305) void_t NTAPI RtlTimeToTimeFields
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

// ******************************************************************
// * 0x0133 - RtlUlongByteSwap()
// ******************************************************************
XBSYSAPI EXPORTNUM(307) ULONG FASTCALL RtlUlongByteSwap
(
	IN ULONG Source
);

// ******************************************************************
// * RtlUnicodeStringToAnsiString
// ******************************************************************
XBSYSAPI EXPORTNUM(308) NTSTATUS NTAPI RtlUnicodeStringToAnsiString
(
    IN OUT PSTRING         DestinationString,
    IN     PUNICODE_STRING SourceString,
    IN     BOOLEAN         AllocateDestinationString
);

// ******************************************************************
// * 0x0135 - RtlUnicodeStringToInteger()
// ******************************************************************
XBSYSAPI EXPORTNUM(309) NTSTATUS NTAPI RtlUnicodeStringToInteger
(
	IN     PUNICODE_STRING String,
	IN     ULONG Base,
	IN     PULONG Value
);

// ******************************************************************
// * 0x0136 - RtlUnicodeToMultiByteN()
// ******************************************************************
XBSYSAPI EXPORTNUM(310) NTSTATUS NTAPI RtlUnicodeToMultiByteN
(
	IN PCHAR MultiByteString,
	IN ULONG MaxBytesInMultiByteString,
	IN PULONG BytesInMultiByteString,
	IN PWSTR UnicodeString,
	IN ULONG BytesInUnicodeString
);

// ******************************************************************
// * 0x0137 - RtlUnicodeToMultiByteSize()
// ******************************************************************
XBSYSAPI EXPORTNUM(311) NTSTATUS NTAPI RtlUnicodeToMultiByteSize
(
	IN PULONG BytesInMultiByteString,
	IN PWSTR UnicodeString,
	IN ULONG BytesInUnicodeString
);

// ******************************************************************
// * 0x0138 - RtlUnwind()
// ******************************************************************
XBSYSAPI EXPORTNUM(312) void_t NTAPI RtlUnwind
(
	IN PVOID TargetFrame OPTIONAL,
	IN PVOID TargetIp OPTIONAL,
	IN PEXCEPTION_RECORD ExceptionRecord OPTIONAL,
	IN PVOID ReturnValue
);

// ******************************************************************
// * 0x0139 - RtlUpcaseUnicodeChar()
// ******************************************************************
XBSYSAPI EXPORTNUM(313) WCHAR NTAPI RtlUpcaseUnicodeChar
(
	IN WCHAR SourceCharacter
);

// ******************************************************************
// * 0x013A - RtlUpcaseUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(314) NTSTATUS NTAPI RtlUpcaseUnicodeString
(
	OUT PUNICODE_STRING DestinationString,
	IN  PUNICODE_STRING SourceString,
	IN  BOOLEAN AllocateDestinationString
);

// ******************************************************************
// * 0x013B - RtlUpcaseUnicodeToMultiByteN()
// ******************************************************************
XBSYSAPI EXPORTNUM(315) NTSTATUS NTAPI RtlUpcaseUnicodeToMultiByteN
(
	IN OUT PCHAR MultiByteString,
	IN ULONG MaxBytesInMultiByteString,
	IN PULONG BytesInMultiByteString,
	IN PWSTR UnicodeString,
	IN ULONG BytesInUnicodeString
);

// ******************************************************************
// * 0x013C - RtlUpperChar()
// ******************************************************************
XBSYSAPI EXPORTNUM(316) CHAR NTAPI RtlUpperChar
(
	CHAR Character
);

// ******************************************************************
// * 0x013D - RtlUpperString()
// ******************************************************************
XBSYSAPI EXPORTNUM(317) void_t NTAPI RtlUpperString
(
	OUT PSTRING DestinationString,
	IN  PSTRING SourceString
);

// ******************************************************************
// * 0x013E - RtlUshortByteSwap()
// ******************************************************************
XBSYSAPI EXPORTNUM(318) USHORT FASTCALL RtlUshortByteSwap
(
	IN USHORT Source
);

// ******************************************************************
// * 0x013F - RtlWalkFrameChain
// ******************************************************************
XBSYSAPI EXPORTNUM(319) ULONG NTAPI RtlWalkFrameChain
(
	OUT PVOID *Callers,
	IN ULONG Count,
	IN ULONG Flags
);

// ******************************************************************
// * 0x0140 - RtlZeroMemory
// ******************************************************************
// *
// * Fill a block of memory with zeros.
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(320) void_t NTAPI RtlZeroMemory
(
  IN void_t UNALIGNED  *Destination,
  IN SIZE_T           Length
);

// ******************************************************************
// * 0x0160 - RtlRip
// ******************************************************************
XBSYSAPI EXPORTNUM(352) void_t NTAPI RtlRip
(
 PCHAR	ApiName,
 PCHAR	Expression,
 PCHAR	Message
);

#endif


