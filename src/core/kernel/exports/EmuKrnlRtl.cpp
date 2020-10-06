// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
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

#define LOG_PREFIX CXBXR_MODULE::RTL
#define CHECK_ALIGNMENT(size, alignment) (((size) % (alignment)) == 0) // For RtlFillMemoryUlong


#include <core\kernel\exports\xboxkrnl.h> // For RtlAnsiStringToUnicodeString, etc.
#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnlLogging.h"

// prevent name collisions
namespace NtDll
{
	#include "core\kernel\support\EmuNtDll.h"
};

#include "core\kernel\init\CxbxKrnl.h" // For CxbxKrnlCleanup()
#include "core\kernel\support\Emu.h" // For EmuLog(LOG_LEVEL::WARNING, )
#include <assert.h>

#ifdef _WIN32

// Prevent errors compiling
#undef RtlFillMemory
#undef RtlMoveMemory
#undef RtlZeroMemory
#undef EXCEPTION_NONCONTINUABLE
#undef EXCEPTION_UNWINDING
#undef EXCEPTION_EXIT_UNWIND
#undef EXCEPTION_STACK_INVALID
#undef EXCEPTION_NESTED_CALL
#undef EXCEPTION_TARGET_UNWIND
#undef EXCEPTION_COLLIDED_UNWIND
#undef EXCEPTION_UNWIND

#endif // _WIN32

// Exception record flags
// Source: ReactOS
// NOTE: don't put these in xboxkrnl.h, they will conflict with the macros provided by Windows
#define EXCEPTION_NONCONTINUABLE  0x01
#define EXCEPTION_UNWINDING       0x02
#define EXCEPTION_EXIT_UNWIND     0x04
#define EXCEPTION_STACK_INVALID   0x08
#define EXCEPTION_NESTED_CALL     0x10
#define EXCEPTION_TARGET_UNWIND   0x20
#define EXCEPTION_COLLIDED_UNWIND 0x40
#define EXCEPTION_UNWIND (EXCEPTION_UNWINDING | EXCEPTION_EXIT_UNWIND | EXCEPTION_TARGET_UNWIND | EXCEPTION_COLLIDED_UNWIND)

xbox::dword_xt WINAPI RtlAnsiStringToUnicodeSize(const xbox::STRING *str)
{
	return (str->Length + sizeof(ANSI_NULL)) * sizeof(WCHAR);
}

// ******************************************************************
// * 0x0104 - RtlAnsiStringToUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(260) xbox::ntstatus_xt NTAPI xbox::RtlAnsiStringToUnicodeString
(
	OUT PUNICODE_STRING DestinationString,
	IN PSTRING         SourceString,
	IN uchar_xt           AllocateDestinationString
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(DestinationString)
		LOG_FUNC_ARG(SourceString)
		LOG_FUNC_ARG(AllocateDestinationString)
		LOG_FUNC_END;

	dword_xt total = RtlAnsiStringToUnicodeSize(SourceString);

	if (total > 0xffff) {
		return xbox::status_invalid_parameter_2;
	}

	DestinationString->Length = (USHORT)(total - sizeof(WCHAR));
	if (AllocateDestinationString) {
		DestinationString->MaximumLength = (USHORT)total;
		if (!(DestinationString->Buffer = (USHORT*)ExAllocatePoolWithTag(total, 'grtS'))) {
			return xbox::status_no_memory;
		}
	}
	else {
		if (total > DestinationString->MaximumLength) {
			return xbox::status_buffer_overflow;
		}
	}

	RtlMultiByteToUnicodeN((PWSTR)DestinationString->Buffer, (ULONG)DestinationString->Length, NULL, SourceString->Buffer, SourceString->Length);
	DestinationString->Buffer[DestinationString->Length / sizeof(WCHAR)] = 0;

	RETURN(xbox::status_success);
}

// ******************************************************************
// * 0x0105 - RtlAppendStringToString()
// ******************************************************************
XBSYSAPI EXPORTNUM(261) xbox::ntstatus_xt NTAPI xbox::RtlAppendStringToString
(
	IN PSTRING Destination,
	IN PSTRING Source
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Destination)
		LOG_FUNC_ARG(Source)
		LOG_FUNC_END;

	NTSTATUS result = xbox::status_success;

	USHORT dstLen = Destination->Length;
	USHORT srcLen = Source->Length;
	if (srcLen > 0) {
		if ((srcLen + dstLen) > Destination->MaximumLength) {
			result = xbox::status_buffer_too_small;
		}
		else {
			CHAR *dstBuf = Destination->Buffer + Destination->Length;
			CHAR *srcBuf = Source->Buffer;
			memmove(dstBuf, srcBuf, srcLen);
			Destination->Length += srcLen;
		}
	}

	RETURN(result);
}

// ******************************************************************
// * 0x0106 - RtlAppendUnicodeStringToString()
// ******************************************************************
XBSYSAPI EXPORTNUM(262) xbox::ntstatus_xt NTAPI xbox::RtlAppendUnicodeStringToString
(
	IN PUNICODE_STRING Destination,
	IN PUNICODE_STRING Source
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Destination)
		LOG_FUNC_ARG(Source)
		LOG_FUNC_END;

	NTSTATUS result = xbox::status_success;

	USHORT dstLen = Destination->Length;
	USHORT srcLen = Source->Length;
	if (srcLen > 0) {
		if ((srcLen + dstLen) > Destination->MaximumLength) {
			result = xbox::status_buffer_too_small;
		}
		else {
			WCHAR *dstBuf = (WCHAR*)(Destination->Buffer + (Destination->Length / sizeof(WCHAR)));
			memmove(dstBuf, Source->Buffer, srcLen);
			Destination->Length += srcLen;
			if (Destination->Length < Destination->MaximumLength) {
				dstBuf[srcLen / sizeof(WCHAR)] = UNICODE_NULL;
			}
		}
	}

	RETURN(result);
}

// ******************************************************************
// * 0x0107 - RtlAppendUnicodeToString()
// ******************************************************************
XBSYSAPI EXPORTNUM(263) xbox::ntstatus_xt NTAPI xbox::RtlAppendUnicodeToString
(
	IN OUT PUNICODE_STRING Destination,
	IN LPCWSTR Source
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Destination)
		LOG_FUNC_ARG(Source)
		LOG_FUNC_END;

	NTSTATUS result = xbox::status_success;
	if (Source != NULL) {
		UNICODE_STRING unicodeString;
		RtlInitUnicodeString(&unicodeString, Source);

		result = RtlAppendUnicodeStringToString(Destination, &unicodeString);
	}

	RETURN(result);
}

// ******************************************************************
// * 0x0108 - RtlAssert()
// ******************************************************************
// Debug API?
XBSYSAPI EXPORTNUM(264) xbox::void_xt NTAPI xbox::RtlAssert
(
	PCHAR   FailedAssertion,
	PCHAR   FileName,
	ulong_xt   LineNumber,
	PCHAR   Message
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(FailedAssertion)
		LOG_FUNC_ARG(FileName)
		LOG_FUNC_ARG(LineNumber)
		LOG_FUNC_ARG(Message)
		LOG_FUNC_END;

	std::stringstream ss;
	ss << "RtlAssert() raised by emulated program\n" << FileName << ":" << LineNumber << ":" << FailedAssertion ;
	if (Message) {
		ss << " " << Message;
	}

	ss << ")";

	PopupWarning(nullptr, ss.str().c_str());
}

// ******************************************************************
// * 0x0109 - RtlCaptureContext()
// ******************************************************************
__declspec(naked) // REQUIRED - No registers can be touched by the compiler or the state will be corrupted.
XBSYSAPI EXPORTNUM(265) xbox::void_xt NTAPI xbox::RtlCaptureContext
(
	IN PCONTEXT ContextRecord
)
{
	// NOTE: this function expects the caller to be __cdecl, or else it fails
	__asm {
		push ebx
		mov ebx, [esp + 8]           // ebx = ContextRecord;

		mov [ebx + CONTEXT.Eax], eax // ContextRecord->Eax = eax;
		mov eax, [esp]				 // eax = original value of ebx
		mov [ebx + CONTEXT.Ebx], eax // ContextRecord->Ebx = original value of ebx
		mov [ebx + CONTEXT.Ecx], ecx // ContextRecord->Ecx = ecx;
		mov [ebx + CONTEXT.Edx], edx // ContextRecord->Edx = edx;
		mov [ebx + CONTEXT.Esi], esi // ContextRecord->Esi = esi;
		mov [ebx + CONTEXT.Edi], edi // ContextRecord->Edi = edi;

		mov word ptr [ebx + CONTEXT.SegCs], cs // ContextRecord->SegCs = cs;
		mov word ptr [ebx + CONTEXT.SegSs], ss // ContextRecord->SegSs = ss;
		pushfd
		pop [ebx + CONTEXT.EFlags]   // ContextRecord->EFlags = flags;

		mov[ebx + CONTEXT.Ebp], ebp  // ContextRecord->Ebp = ebp;
		mov eax, [ebp + 4]           // eax = return address;
		mov [ebx + CONTEXT.Eip], eax // ContextRecord->Eip = return address;
		lea eax, [ebp + 8]
		mov [ebx + CONTEXT.Esp], eax // ContextRecord->Esp = original esp value;

		pop ebx
		ret 4
	}
}

// ******************************************************************
// * 0x010A - RtlCaptureStackBackTrace()
// ******************************************************************
XBSYSAPI EXPORTNUM(266) xbox::ushort_xt NTAPI xbox::RtlCaptureStackBackTrace
(
	IN ulong_xt FramesToSkip,
	IN ulong_xt FramesToCapture,
	OUT PVOID *BackTrace,
	OUT PULONG BackTraceHash
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(FramesToSkip)
		LOG_FUNC_ARG(FramesToCapture)
		LOG_FUNC_ARG_OUT(BackTrace)
		LOG_FUNC_ARG_OUT(BackTraceHash)
	LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(NULL);
}

// ******************************************************************
// * 0x010B - RtlCharToInteger()
// ******************************************************************
XBSYSAPI EXPORTNUM(267) xbox::ntstatus_xt NTAPI xbox::RtlCharToInteger
(
	IN     PCSZ   String,
	IN     ulong_xt  Base OPTIONAL,
	OUT    PULONG Value
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(String)
		LOG_FUNC_ARG(Base)
		LOG_FUNC_ARG(Value)
		LOG_FUNC_END;

    char bMinus = 0;

    // skip leading whitespaces
	while (*String != '\0' && *String <= ' ') {
		String++;
	}

    // Check for +/-
    if (*String == '+') {
		String++;
    }
    else if (*String == '-') {
        bMinus = 1;
		String++;
    }

    // base = 0 means autobase
	if (Base == 0) {
		Base = 10;
		if (String[0] == '0') {
			if (String[1] == 'b') {
				String += 2;
				Base = 2;
			}
			else if (String[1] == 'o') {
				String += 2;
				Base = 8;
			}
			else if (String[1] == 'x') {
				String += 2;
				Base = 16;
			}
		}
	}
	else if (Base != 2 && Base != 8 && Base != 10 && Base != 16) {
		return STATUS_INVALID_PARAMETER;
	}

	if (Value == NULL) {
		return STATUS_ACCESS_VIOLATION;
	}

    ULONG RunningTotal = 0;
	while (*String != '\0') {
		CHAR chCurrent = *String;
		int digit;

		if (chCurrent >= '0' && chCurrent <= '9') {
			digit = chCurrent - '0';
		}
		else if (chCurrent >= 'A' && chCurrent <= 'Z') {
			digit = chCurrent - 'A' + 10;
		}
		else if (chCurrent >= 'a' && chCurrent <= 'z') {
			digit = chCurrent - 'a' + 10;
		}
		else {
			digit = -1;
		}

		if (digit < 0 || digit >= (int)Base)
			break;

		RunningTotal = RunningTotal * Base + digit;
		String++;
	}

    *Value = bMinus ? (0 - RunningTotal) : RunningTotal;
	RETURN(xbox::status_success);
}

// ******************************************************************
// * 0x010C - RtlCompareMemory()
// ******************************************************************
// * compare block of memory, return number of equivalent bytes.
// ******************************************************************
XBSYSAPI EXPORTNUM(268) xbox::size_xt NTAPI xbox::RtlCompareMemory
(
	IN CONST void_xt *Source1,
	IN CONST void_xt *Source2,
	IN size_xt      Length
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Source1)
		LOG_FUNC_ARG(Source2)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_END;

	SIZE_T result = Length;

	uint8_t *pBytes1 = (uint8_t *)Source1;
	uint8_t *pBytes2 = (uint8_t *)Source2;
	for (uint32_t i = 0; i < Length; i++) {
		if (pBytes1[i] != pBytes2[i]) {
			result = i;
			break;
		}
	}

	RETURN(result);
}

// ******************************************************************
// * 0x010D - RtlCompareMemoryUlong()
// ******************************************************************
XBSYSAPI EXPORTNUM(269) xbox::size_xt NTAPI xbox::RtlCompareMemoryUlong
(
	IN PVOID Source,
	IN size_xt Length,
	IN ulong_xt Pattern
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Source)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(Pattern)
		LOG_FUNC_END;

	PULONG ptr = (PULONG)Source;
	ULONG_PTR len = Length / sizeof(ULONG);
	ULONG_PTR i;

	for (i = 0; i < len; i++) {
		if (*ptr != Pattern)
			break;

		ptr++;
	}

	SIZE_T result = (SIZE_T)((PCHAR)ptr - (PCHAR)Source);

	RETURN(result);
}

// ******************************************************************
// * 0x010E - RtlCompareString()
// ******************************************************************
XBSYSAPI EXPORTNUM(270) xbox::long_xt NTAPI xbox::RtlCompareString
(
	IN PSTRING String1,
	IN PSTRING String2,
	IN boolean_xt CaseInSensitive
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(String1)
		LOG_FUNC_ARG(String2)
		LOG_FUNC_ARG(CaseInSensitive)
		LOG_FUNC_END;

	LONG result;

	USHORT l1 = String1->Length;
	USHORT l2 = String2->Length;
	USHORT maxLen = l1 <= l2 ? l1 : l2;

	CHAR *str1 = String1->Buffer;
	CHAR *str2 = String2->Buffer;

	if (CaseInSensitive) {
		result = _strnicmp(str1, str2, maxLen);
	}
	else {
		result = strncmp(str1, str2, maxLen);
	}

	RETURN(result);
}

// ******************************************************************
// * 0x010F - RtlCompareUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(271) xbox::long_xt NTAPI xbox::RtlCompareUnicodeString
(
	IN PUNICODE_STRING String1,
	IN PUNICODE_STRING String2,
	IN boolean_xt CaseInSensitive
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(String1)
		LOG_FUNC_ARG(String2)
		LOG_FUNC_ARG(CaseInSensitive)
		LOG_FUNC_END;

	LONG result;

	USHORT l1 = String1->Length;
	USHORT l2 = String2->Length;
	USHORT maxLen = l1 <= l2 ? l1 : l2;

	WCHAR *str1 = (WCHAR*)(String1->Buffer);
	WCHAR *str2 = (WCHAR*)(String2->Buffer);

	if (CaseInSensitive) {
		result = _wcsnicmp(str1, str2, maxLen);
	}
	else {
		result = wcsncmp(str1, str2, maxLen);
	}

	RETURN(result);
}

// ******************************************************************
// * 0x0110 - RtlCopyString()
// ******************************************************************
XBSYSAPI EXPORTNUM(272) xbox::void_xt NTAPI xbox::RtlCopyString
(
	OUT PSTRING DestinationString,
	IN PSTRING SourceString OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(DestinationString)
		LOG_FUNC_ARG(SourceString)
		LOG_FUNC_END;

	if (SourceString == NULL) {
		DestinationString->Length = 0;
		return;
	}

	CHAR *pd = DestinationString->Buffer;
	CHAR *ps = SourceString->Buffer;
	USHORT len = SourceString->Length;
	if ((USHORT)len > DestinationString->MaximumLength) {
		len = DestinationString->MaximumLength;
	}

	DestinationString->Length = (USHORT)len;
	memcpy(pd, ps, len);
}

// ******************************************************************
// * 0x0111 - RtlCopyUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(273) xbox::void_xt NTAPI xbox::RtlCopyUnicodeString
(
	OUT PUNICODE_STRING DestinationString,
	IN PUNICODE_STRING SourceString OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(DestinationString)
		LOG_FUNC_ARG(SourceString)
		LOG_FUNC_END;

	if (SourceString == NULL) {
		DestinationString->Length = 0;
		return;
	}

	CHAR *pd = (CHAR*)(DestinationString->Buffer);
	CHAR *ps = (CHAR*)(SourceString->Buffer);
	USHORT len = SourceString->Length;
	if ((USHORT)len > DestinationString->MaximumLength) {
		len = DestinationString->MaximumLength;
	}

	DestinationString->Length = (USHORT)len;
	memcpy(pd, ps, len);
}

// ******************************************************************
// * 0x0112 - RtlCreateUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(274) xbox::boolean_xt NTAPI xbox::RtlCreateUnicodeString
(
	OUT PUNICODE_STRING DestinationString,
	IN PCWSTR SourceString
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(DestinationString)
		LOG_FUNC_ARG(SourceString)
		LOG_FUNC_END;

	BOOLEAN result = TRUE;

	ULONG bufferSize = (std::u16string(SourceString).length() + 1) * sizeof(WCHAR);
	DestinationString->Buffer = (USHORT *)ExAllocatePoolWithTag(bufferSize, 'grtS');
	if (!DestinationString->Buffer) {
		result = FALSE;
	}
	else {
		RtlMoveMemory(DestinationString->Buffer, SourceString, bufferSize);
		DestinationString->MaximumLength = (USHORT)bufferSize;
		DestinationString->Length = (USHORT)(bufferSize - sizeof(UNICODE_NULL));
	}

	RETURN(result);
}

// ******************************************************************
// * 0x0113 - RtlDowncaseUnicodeChar()
// ******************************************************************
XBSYSAPI EXPORTNUM(275) xbox::wchar_xt NTAPI xbox::RtlDowncaseUnicodeChar
(
	IN wchar_xt SourceCharacter
)
{
	LOG_FUNC_ONE_ARG(SourceCharacter);

	WCHAR result = towlower(SourceCharacter);

	RETURN(result);
}

// ******************************************************************
// * 0x0114 - RtlDowncaseUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(276) xbox::ntstatus_xt NTAPI xbox::RtlDowncaseUnicodeString
(
	OUT PUNICODE_STRING DestinationString,
	IN PUNICODE_STRING SourceString,
	IN boolean_xt AllocateDestinationString
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(DestinationString)
		LOG_FUNC_ARG(SourceString)
		LOG_FUNC_ARG(AllocateDestinationString)
		LOG_FUNC_END;

	NTSTATUS result = xbox::status_success;

	if (AllocateDestinationString) {
		DestinationString->MaximumLength = SourceString->Length;
		DestinationString->Buffer = (USHORT*)ExAllocatePoolWithTag((ULONG)DestinationString->MaximumLength, 'grtS');
		if (DestinationString->Buffer == NULL) {
			return xbox::status_no_memory;
		}
	}
	else {
		if (SourceString->Length > DestinationString->MaximumLength) {
			return xbox::status_buffer_overflow;
		}
	}

	ULONG length = ((ULONG)SourceString->Length) / sizeof(WCHAR);
	WCHAR *pDst = (WCHAR*)(DestinationString->Buffer);
	WCHAR *pSrc = (WCHAR*)(SourceString->Buffer);
	for (ULONG i = 0; i < length; i++) {
		pDst[i] = (WCHAR)towlower(pSrc[i]);
	}

	DestinationString->Length = SourceString->Length;

	RETURN(result);
}

// ******************************************************************
// * 0x0115 - RtlEnterCriticalSection()
// ******************************************************************
XBSYSAPI EXPORTNUM(277) xbox::void_xt NTAPI xbox::RtlEnterCriticalSection
(
    IN PRTL_CRITICAL_SECTION CriticalSection
)
{
    LOG_FUNC_ONE_ARG(CriticalSection);

    HANDLE thread = (HANDLE)KeGetCurrentThread();

    CriticalSection->LockCount++;
    if(CriticalSection->LockCount == 0) {
        CriticalSection->OwningThread = thread;
        CriticalSection->RecursionCount = 1;
    }
    else {
        if(CriticalSection->OwningThread != thread) {
			if (CriticalSection->OwningThread != nullptr) {
				NTSTATUS result;
				result = KeWaitForSingleObject(
					(PVOID)CriticalSection,
					(KWAIT_REASON)0,
					(KPROCESSOR_MODE)0,
					(boolean_xt)0,
					(PLARGE_INTEGER)0
				);
				if (!nt_success(result))
				{
					CxbxKrnlCleanup("Waiting for event of a critical section returned %lx.", result);
				};
			}
            CriticalSection->OwningThread = thread;
            CriticalSection->RecursionCount = 1;
        }
        else {
            CriticalSection->RecursionCount++;
        }
    }
}

// ******************************************************************
// * 0x0116 - RtlEnterCriticalSectionAndRegion()
// ******************************************************************
XBSYSAPI EXPORTNUM(278) xbox::void_xt NTAPI xbox::RtlEnterCriticalSectionAndRegion
(
    IN PRTL_CRITICAL_SECTION CriticalSection
)
{
    LOG_FUNC_ONE_ARG(CriticalSection);

    KeEnterCriticalRegion();
    RtlEnterCriticalSection(CriticalSection);
}

// ******************************************************************
// * 0x0117 - RtlEqualString()
// ******************************************************************
XBSYSAPI EXPORTNUM(279) xbox::boolean_xt NTAPI xbox::RtlEqualString
(
	IN PSTRING String1,
	IN PSTRING String2,
	IN boolean_xt CaseInSensitive
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(String1)
		LOG_FUNC_ARG(String2)
		LOG_FUNC_ARG(CaseInSensitive)
		LOG_FUNC_END;

	BOOLEAN bRet = TRUE;

	USHORT l1 = String1->Length;
	USHORT l2 = String2->Length;
	if (l1 != l2) {
		return FALSE;
	}

	CHAR *p1 = String1->Buffer;
	CHAR *p2 = String2->Buffer;
	CHAR *last = p1 + l1;

	if (CaseInSensitive) {
		while (p1 < last) {
			CHAR c1 = *p1++;
			CHAR c2 = *p2++;
			if (c1 != c2) {
				c1 = RtlUpperChar(c1);
				c2 = RtlUpperChar(c2);
				if (c1 != c2) {
					return FALSE;
				}
			}
		}
		return TRUE;
	}

	while (p1 < last) {
		if (*p1++ != *p2++) {
			bRet = FALSE;
			break;
		}
	}

	RETURN(bRet);
}

// ******************************************************************
// * 0x0118 - RtlEqualUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(280) xbox::boolean_xt NTAPI xbox::RtlEqualUnicodeString
(
	IN PUNICODE_STRING String1,
	IN PUNICODE_STRING String2,
	IN boolean_xt CaseInSensitive
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(String1)
		LOG_FUNC_ARG(String2)
		LOG_FUNC_ARG(CaseInSensitive)
		LOG_FUNC_END;

	BOOLEAN bRet = FALSE;
	USHORT l1 = String1->Length;
	USHORT l2 = String2->Length;

	if (l1 == l2) {
		WCHAR *p1 = (WCHAR*)(String1->Buffer);
		WCHAR *p2 = (WCHAR*)(String2->Buffer);
		WCHAR *last = p1 + (l1 / sizeof(WCHAR));

		bRet = TRUE;

		if (CaseInSensitive) {
			while (p1 < last) {
				WCHAR c1 = *p1++;
				WCHAR c2 = *p2++;
				if (c1 != c2) {
					c1 = towupper(c1);
					c2 = towupper(c2);
					if (c1 != c2) {
						bRet = FALSE;
						break;
					}
				}
			}
		}
		else {
			while (p1 < last) {
				if (*p1++ != *p2++) {
					bRet = FALSE;
					break;
				}
			}
		}
	}

	RETURN(bRet);
}

// ******************************************************************
// * 0x0119 - RtlExtendedIntegerMultiply()
// ******************************************************************
XBSYSAPI EXPORTNUM(281) xbox::LARGE_INTEGER NTAPI xbox::RtlExtendedIntegerMultiply
(
	IN LARGE_INTEGER Multiplicand,
	IN long_xt Multiplier
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Multiplicand)
		LOG_FUNC_ARG(Multiplier)
		LOG_FUNC_END;

	LARGE_INTEGER ret;

	ret.QuadPart = Multiplicand.QuadPart* (LONGLONG)Multiplier;

	RETURN(ret);
}

// ******************************************************************
// * 0x011A - RtlExtendedLargeIntegerDivide()
// ******************************************************************
XBSYSAPI EXPORTNUM(282) xbox::LARGE_INTEGER NTAPI xbox::RtlExtendedLargeIntegerDivide
(
	IN LARGE_INTEGER Dividend,
	IN ulong_xt Divisor,
	IN OUT PULONG Remainder OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Dividend)
		LOG_FUNC_ARG(Divisor)
		LOG_FUNC_ARG(Remainder)
		LOG_FUNC_END;

	LARGE_INTEGER quotient = Dividend;

	if (Divisor == 0) {
		RtlRaiseStatus(STATUS_INTEGER_DIVIDE_BY_ZERO);
	}
	else {
		ULONG local_remainder = 0;
		BOOLEAN carry, remainder_carry;

		// Binary division algorithm reverse engineered from real hardware.
		for (uint8_t i = 64; i > 0; i--) {
			carry = (quotient.QuadPart >> 63) & 0x1;
			remainder_carry = (local_remainder >> 31) & 0x1;
			quotient.QuadPart <<= 1;
			local_remainder = (local_remainder << 1) | carry;
			if (remainder_carry || (local_remainder >= Divisor)) {
				quotient.u.LowPart += 1;
				local_remainder -= Divisor;
			}
		}
		if (Remainder) {
			*Remainder = local_remainder;
		}
	}

	RETURN(quotient);
}

#define LOWER_32(A) ((A) & 0xffffffff)
#define UPPER_32(A) ((A) >> 32)

// ******************************************************************
// * 0x011B - RtlExtendedMagicDivide()
// ******************************************************************
XBSYSAPI EXPORTNUM(283) xbox::LARGE_INTEGER NTAPI xbox::RtlExtendedMagicDivide
(
	IN	LARGE_INTEGER Dividend,
	IN	LARGE_INTEGER MagicDivisor,
	IN	cchar_xt ShiftCount
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Dividend)
		LOG_FUNC_ARG(MagicDivisor)
		LOG_FUNC_ARG(ShiftCount)
		LOG_FUNC_END;

	LARGE_INTEGER result;

	ULONGLONG dividend_high;
	ULONGLONG dividend_low;
	ULONGLONG inverse_divisor_high;
	ULONGLONG inverse_divisor_low;
	ULONGLONG ah_bl;
	ULONGLONG al_bh;
	BOOLEAN positive;
	
	if (Dividend.QuadPart < 0) {
	   dividend_high = UPPER_32((ULONGLONG) -Dividend.QuadPart);
	   dividend_low =  LOWER_32((ULONGLONG) -Dividend.QuadPart);
	   positive = FALSE;
	}
	else {
	   dividend_high = UPPER_32((ULONGLONG) Dividend.QuadPart);
	   dividend_low =  LOWER_32((ULONGLONG) Dividend.QuadPart);
	   positive = TRUE;
	}

	inverse_divisor_high = UPPER_32((ULONGLONG) MagicDivisor.QuadPart);
	inverse_divisor_low =  LOWER_32((ULONGLONG) MagicDivisor.QuadPart);

	ah_bl = dividend_high * inverse_divisor_low;
	al_bh = dividend_low * inverse_divisor_high;

	result.QuadPart =
	   (LONGLONG) ((dividend_high * inverse_divisor_high +
	                UPPER_32(ah_bl) +
	                UPPER_32(al_bh) +
	                UPPER_32(LOWER_32(ah_bl) + LOWER_32(al_bh) +
	                         UPPER_32(dividend_low * inverse_divisor_low))) >> ShiftCount);
	if (!positive) {
	   result.QuadPart = -result.QuadPart;
	}

	RETURN(result);
}

// ******************************************************************
// * 0x011C - RtlFillMemory()
// ******************************************************************
XBSYSAPI EXPORTNUM(284) xbox::void_xt NTAPI xbox::RtlFillMemory
(
	IN void_xt UNALIGNED *Destination,
	IN dword_xt Length,
	IN byte_xt  Fill
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Destination)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(Fill)
		LOG_FUNC_END;

	memset(Destination, Fill, Length);
}

// ******************************************************************
// * 0x011D - RtlFillMemoryUlong()
// ******************************************************************
XBSYSAPI EXPORTNUM(285) xbox::void_xt NTAPI xbox::RtlFillMemoryUlong
(
	IN PVOID Destination,
	IN size_t Length,
	IN ulong_xt Pattern
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Destination)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(Pattern)
	LOG_FUNC_END;

	// Fill 32 bits at a time
	// Any extra bytes are ignored

	assert(Length != 0);
	assert(CHECK_ALIGNMENT(Length, sizeof(ULONG)));                   // Length must be a multiple of ULONG
	assert(CHECK_ALIGNMENT((uintptr_t)Destination, sizeof(ULONG)));   // Destination must be 4-byte aligned

	int NumOfRepeats = Length / sizeof(ULONG);
	ULONG* d = (ULONG*)Destination;

	for (int i = 0; i < NumOfRepeats; ++i)
	{
		d[i] = Pattern; // copy an ULONG at a time
	}
}

// ******************************************************************
// * 0x011E - RtlFreeAnsiString()
// ******************************************************************
XBSYSAPI EXPORTNUM(286) xbox::void_xt NTAPI xbox::RtlFreeAnsiString
(
	IN OUT PANSI_STRING AnsiString
)
{
	LOG_FUNC_ONE_ARG(AnsiString);

	if (AnsiString->Buffer) {
		ExFreePool(AnsiString->Buffer);
		memset(AnsiString, 0, sizeof(*AnsiString));
	}
}

// ******************************************************************
// * 0x011F - RtlFreeUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(287) xbox::void_xt NTAPI xbox::RtlFreeUnicodeString
(
	IN OUT PUNICODE_STRING UnicodeString
)
{
	LOG_FUNC_ONE_ARG(UnicodeString);

	if (UnicodeString->Buffer) {
		ExFreePool(UnicodeString->Buffer);
		memset(UnicodeString, 0, sizeof(*UnicodeString));
	}
}

// ******************************************************************
// * 0x0120 - RtlGetCallersAddress()
// ******************************************************************
XBSYSAPI EXPORTNUM(288) xbox::void_xt NTAPI xbox::RtlGetCallersAddress
(
	OUT PVOID *CallersAddress,
	OUT PVOID *CallersCaller
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(CallersAddress)
		LOG_FUNC_ARG_OUT(CallersCaller)
	LOG_FUNC_END;

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x0121 - RtlInitAnsiString()
// ******************************************************************
XBSYSAPI EXPORTNUM(289) xbox::void_xt NTAPI xbox::RtlInitAnsiString
(
	IN OUT PANSI_STRING DestinationString,
	IN     PCSZ         SourceString
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(DestinationString)
		LOG_FUNC_ARG(SourceString)
		LOG_FUNC_END;

	DestinationString->Buffer = SourceString;
	if (SourceString != NULL) {
		CCHAR *pSourceString = (CCHAR*)(SourceString);
		DestinationString->Buffer = SourceString;
		DestinationString->Length = (USHORT)strlen(pSourceString);
		DestinationString->MaximumLength = DestinationString->Length + 1;
	}
	else {
		DestinationString->Length = DestinationString->MaximumLength = 0;
	}
}

// ******************************************************************
// * 0x0122 - RtlInitUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(290) xbox::void_xt NTAPI xbox::RtlInitUnicodeString
(
	IN OUT PUNICODE_STRING DestinationString,
	IN     PCWSTR         SourceString
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(DestinationString)
		LOG_FUNC_ARG(SourceString)
		LOG_FUNC_END;

	DestinationString->Buffer = (USHORT*)SourceString;
	if (SourceString != NULL) {
		DestinationString->Buffer = (USHORT*)SourceString;
		DestinationString->Length = (USHORT)std::u16string(SourceString).length() * 2;
		DestinationString->MaximumLength = DestinationString->Length + 2;
	}
	else {
		DestinationString->Length = DestinationString->MaximumLength = 0;
	}
}

// ******************************************************************
// * 0x0123 - RtlInitializeCriticalSection()
// ******************************************************************
XBSYSAPI EXPORTNUM(291) xbox::void_xt NTAPI xbox::RtlInitializeCriticalSection
(
    IN PRTL_CRITICAL_SECTION CriticalSection
)
{
    LOG_FUNC_ONE_ARG(CriticalSection);

	KeInitializeEvent((PRKEVENT)&CriticalSection->Event, SynchronizationEvent, FALSE);
	CriticalSection->LockCount = -1;
    CriticalSection->RecursionCount = 0;
    CriticalSection->OwningThread = 0;
}

// ******************************************************************
// * 0x0124 - RtlIntegerToChar()
// ******************************************************************
XBSYSAPI EXPORTNUM(292) xbox::ntstatus_xt NTAPI xbox::RtlIntegerToChar
(
	IN ulong_xt Value,
	IN ulong_xt Base,
	IN long_xt OutputLength,
	IN PSZ String
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Value)
		LOG_FUNC_ARG(Base)
		LOG_FUNC_ARG(OutputLength)
		LOG_FUNC_ARG(String)
		LOG_FUNC_END;

	if (Base == 0) {
        Base = 10;
    }
    else if (Base != 2 && Base != 8 && Base != 10 && Base != 16) {
        return STATUS_INVALID_PARAMETER;
    }

	CHAR buffer[33];
	PCHAR pos = &buffer[32];

	*pos = '\0';   
	do {
		pos--;
		CHAR digit = (CHAR)(Value % Base);
		Value /= Base;

		if (digit < 10) {
			*pos = '0' + digit;
		}
		else {
			*pos = 'A' + digit - 10;
		}
	} while (Value != 0L);

	SIZE_T len = &buffer[32] - pos;   
	if (len > (SIZE_T)OutputLength) {
		return xbox::status_buffer_overflow;
	}
	else if (String == NULL) {
		return STATUS_ACCESS_VIOLATION;
	}
	else if (len == OutputLength) {
		RtlCopyMemory(String, pos, len);
	}
	else {
		RtlCopyMemory(String, pos, len + 1);
	}

	RETURN(xbox::status_success);
}

// ******************************************************************
// * 0x0125 - RtlIntegerToUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(293) xbox::ntstatus_xt NTAPI xbox::RtlIntegerToUnicodeString
(
	IN     ulong_xt Value,
	IN     ulong_xt Base,
	IN     PUNICODE_STRING String
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Value)
		LOG_FUNC_ARG(Base)
		LOG_FUNC_ARG_OUT(String)
		LOG_FUNC_END;

    CHAR Buffer[33];
    NTSTATUS Status = RtlIntegerToChar(Value, Base, sizeof(Buffer), Buffer);

    if (nt_success(Status)) {
		ANSI_STRING AnsiString;

        AnsiString.Buffer = Buffer;
        AnsiString.Length = (USHORT)strlen(Buffer);
        AnsiString.MaximumLength = sizeof(Buffer);

        Status = RtlAnsiStringToUnicodeString(String, &AnsiString, FALSE);
    }

	RETURN(Status);
}

// ******************************************************************
// * 0x0126 - RtlLeaveCriticalSection()
// ******************************************************************
XBSYSAPI EXPORTNUM(294) xbox::void_xt NTAPI xbox::RtlLeaveCriticalSection
(
    IN PRTL_CRITICAL_SECTION CriticalSection
)
{
    LOG_FUNC_ONE_ARG(CriticalSection);

    CriticalSection->RecursionCount--;
    CriticalSection->LockCount--;
    if(CriticalSection->RecursionCount == 0) {
        CriticalSection->OwningThread = 0;
        if(CriticalSection->LockCount >= 0) {
            KeSetEvent((PRKEVENT)CriticalSection, (KPRIORITY)1, (boolean_xt)0);
        }
    }
}

// ******************************************************************
// * 0x0127 - RtlLeaveCriticalSectionAndRegion()
// ******************************************************************
XBSYSAPI EXPORTNUM(295) xbox::void_xt NTAPI xbox::RtlLeaveCriticalSectionAndRegion
(
    IN PRTL_CRITICAL_SECTION CriticalSection
)
{
    LOG_FUNC_ONE_ARG(CriticalSection);

    RtlLeaveCriticalSection(CriticalSection);

	if (CriticalSection->RecursionCount == 0) {
		KeLeaveCriticalRegion();
	}
}

// ******************************************************************
// * 0x0128 - RtlLowerChar()
// ******************************************************************
XBSYSAPI EXPORTNUM(296) xbox::char_xt NTAPI xbox::RtlLowerChar
(
	char_xt Character
)
{
	LOG_FUNC_ONE_ARG(Character);
	
	byte_xt CharCode = (byte_xt)Character;

	if (CharCode >= 'A' && CharCode <= 'Z')
	{
		CharCode ^= 0x20;
	}

	// Latin alphabet (ISO 8859-1)
	else if (CharCode >= 0xc0 && CharCode <= 0xde && CharCode != 0xd7)
	{
		CharCode ^= 0x20;
	}

	RETURN((char_xt)CharCode);
}

// ******************************************************************
// * 0x0129 - RtlMapGenericMask()
// ******************************************************************
XBSYSAPI EXPORTNUM(297) xbox::void_xt NTAPI xbox::RtlMapGenericMask
(
	IN PACCESS_MASK AccessMask,
	IN PGENERIC_MAPPING GenericMapping
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(AccessMask)
		LOG_FUNC_ARG(GenericMapping)
		LOG_FUNC_END;

	if (*AccessMask & GENERIC_READ) {
		*AccessMask |= GenericMapping->GenericRead;
	}
	if (*AccessMask & GENERIC_WRITE) {
		*AccessMask |= GenericMapping->GenericWrite;
	}
	if (*AccessMask & GENERIC_EXECUTE) {
		*AccessMask |= GenericMapping->GenericExecute;
	}
	if (*AccessMask & GENERIC_ALL) {
		*AccessMask |= GenericMapping->GenericAll;
	}

	*AccessMask &= ~(GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE | GENERIC_ALL);
}

// ******************************************************************
// * 0x012A - RtlMoveMemory()
// ******************************************************************
XBSYSAPI EXPORTNUM(298) xbox::void_xt NTAPI xbox::RtlMoveMemory
(
	IN void_xt UNALIGNED       *Destination,
	IN CONST void_xt UNALIGNED *Source,
	IN size_t                Length
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Destination)
		LOG_FUNC_ARG(Source)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_END;

	memmove(Destination, Source, Length);
}

// ******************************************************************
// * 0x012B - RtlMultiByteToUnicodeN()
// ******************************************************************
XBSYSAPI EXPORTNUM(299) xbox::ntstatus_xt NTAPI xbox::RtlMultiByteToUnicodeN
(
	IN     PWSTR UnicodeString,
	IN     ulong_xt MaxBytesInUnicodeString,
	IN     PULONG BytesInUnicodeString,
	IN     PCHAR MultiByteString,
	IN     ulong_xt BytesInMultiByteString
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(UnicodeString)
		LOG_FUNC_ARG(MaxBytesInUnicodeString);
		LOG_FUNC_ARG(BytesInUnicodeString);
		LOG_FUNC_ARG(MultiByteString);
		LOG_FUNC_ARG(BytesInMultiByteString)
		LOG_FUNC_END;

	ULONG maxUnicodeChars = MaxBytesInUnicodeString / sizeof(WCHAR);
	ULONG numChars = (maxUnicodeChars < BytesInMultiByteString) ? maxUnicodeChars : BytesInMultiByteString;

	if (BytesInUnicodeString != NULL) {
		*BytesInUnicodeString = numChars * sizeof(WCHAR);
	}

	while (numChars) {
		*UnicodeString = (WCHAR)(*MultiByteString);

		UnicodeString++;
		MultiByteString++;
		numChars--;
	}

	RETURN(xbox::status_success);
}

// ******************************************************************
// * 0x012C - RtlMultiByteToUnicodeSize()
// ******************************************************************
XBSYSAPI EXPORTNUM(300) xbox::ntstatus_xt NTAPI xbox::RtlMultiByteToUnicodeSize
(
	IN PULONG BytesInUnicodeString,
	IN PCHAR MultiByteString,
	IN ulong_xt BytesInMultiByteString
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(BytesInUnicodeString)
		LOG_FUNC_ARG(MultiByteString);
		LOG_FUNC_ARG(BytesInMultiByteString)
		LOG_FUNC_END;

	*BytesInUnicodeString = BytesInMultiByteString * sizeof(WCHAR);

	RETURN(xbox::status_success);
}

// ******************************************************************
// * 0x012D - RtlNtStatusToDosError()
// ******************************************************************
XBSYSAPI EXPORTNUM(301) xbox::ulong_xt NTAPI xbox::RtlNtStatusToDosError
(
	IN ntstatus_xt Status
)
{
	LOG_FUNC_ONE_ARG(Status);

	ULONG ret = NtDll::RtlNtStatusToDosError(Status);
/* https://doxygen.reactos.org/de/ddc/sdk_2lib_2rtl_2error_8c.html#aaad43f3dbf8784c2ca1ef07748199f20
	struct error_table {
		DWORD       start;
		DWORD       end;
		const DWORD *table;
	};

	static const struct error_table error_table[20];

	const struct error_table *table = error_table;

    if (!Status || (Status & 0x20000000))
		return Status;

    // 0xd... is equivalent to 0xc...
    if ((Status & 0xf0000000) == 0xd0000000)
		Status &= ~0x10000000;

    while (table->start) {
        if ((ULONG)Status < table->start)
			break;

        if ((ULONG)Status < table->end) {
            DWORD ret = table->table[Status - table->start];
            // unknown entries are 0
            if (!ret)
				goto no_mapping;

            return ret;
        }

        table++;
    }

    // now some special cases
    if (HIWORD(Status) == 0xc001)
		return LOWORD(Status);

    if (HIWORD(Status) == 0x8007)
		return LOWORD(Status);

no_mapping:
    EmuLog(LOG_LEVEL::DEBUG, "no mapping for %08x", Status);
	ret = ERROR_MR_MID_NOT_FOUND;
*/
	RETURN(ret);
}

#define TICKSPERSEC        10000000
#define TICKSPERMSEC       10000
#define SECSPERDAY         86400
#define SECSPERHOUR        3600
#define SECSPERMIN         60
#define MINSPERHOUR        60
#define HOURSPERDAY        24
#define EPOCHWEEKDAY       1  /* Jan 1, 1601 was Monday */
#define DAYSPERWEEK        7
#define MONSPERYEAR        12
#define DAYSPERQUADRICENTENNIUM (365 * 400 + 97)
#define DAYSPERNORMALQUADRENNIUM (365 * 4 + 1)

/* 1601 to 1970 is 369 years plus 89 leap days */
#define SECS_1601_TO_1970  ((369 * 365 + 89) * (ULONGLONG)SECSPERDAY)
#define TICKS_1601_TO_1970 (SECS_1601_TO_1970 * TICKSPERSEC)
/* 1601 to 1980 is 379 years plus 91 leap days */
#define SECS_1601_TO_1980  ((379 * 365 + 91) * (ULONGLONG)SECSPERDAY)
#define TICKS_1601_TO_1980 (SECS_1601_TO_1980 * TICKSPERSEC)


static const int MonthLengths[2][MONSPERYEAR] =
{
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

static inline BOOL IsLeapYear(int Year)
{
    return Year % 4 == 0 && (Year % 100 != 0 || Year % 400 == 0);
}

// ******************************************************************
// * 0x012E - RtlRaiseException()
// ******************************************************************
XBSYSAPI EXPORTNUM(302) xbox::void_xt NTAPI xbox::RtlRaiseException
(
	IN PEXCEPTION_RECORD ExceptionRecord
)
{
	LOG_FUNC_ONE_ARG(ExceptionRecord);

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x012F - RtlRaiseStatus()
// ******************************************************************
XBSYSAPI EXPORTNUM(303) xbox::void_xt NTAPI xbox::RtlRaiseStatus
(
	IN ntstatus_xt Status
)
{
	LOG_FUNC_ONE_ARG(Status);

	EXCEPTION_RECORD record;
	record.ExceptionCode = Status;
	record.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
	record.ExceptionRecord = NULL;
	record.NumberParameters = 0;

	RtlRaiseException(&record);
}

// ******************************************************************
// * 0x0130 - RtlTimeFieldsToTime()
// ******************************************************************
XBSYSAPI EXPORTNUM(304) xbox::boolean_xt NTAPI xbox::RtlTimeFieldsToTime
(
	IN  PTIME_FIELDS    TimeFields,
	OUT PLARGE_INTEGER  Time
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(TimeFields)
		LOG_FUNC_ARG_OUT(Time)
		LOG_FUNC_END;

	int month, year, cleaps, day;

	/* FIXME: normalize the TIME_FIELDS structure here */
	/* No, native just returns 0 (error) if the fields are not */
	if (TimeFields->Millisecond < 0 || TimeFields->Millisecond > 999 ||
		TimeFields->Second < 0 || TimeFields->Second > 59 ||
		TimeFields->Minute < 0 || TimeFields->Minute > 59 ||
		TimeFields->Hour < 0 || TimeFields->Hour > 23 ||
		TimeFields->Month < 1 || TimeFields->Month > 12 ||
		TimeFields->Day < 1 ||
		TimeFields->Day > MonthLengths
			[TimeFields->Month == 2 || IsLeapYear(TimeFields->Year)]
			[TimeFields->Month - 1] ||
		TimeFields->Year < 1601)
		return FALSE;

	/* now calculate a day count from the date
	* First start counting years from March. This way the leap days
	* are added at the end of the year, not somewhere in the middle.
	* Formula's become so much less complicate that way.
	* To convert: add 12 to the month numbers of Jan and Feb, and
	* take 1 from the year */
	if (TimeFields->Month < 3) {
		month = TimeFields->Month + 13;
		year = TimeFields->Year - 1;
	}
	else {
		month = TimeFields->Month + 1;
		year = TimeFields->Year;
	}
	cleaps = (3 * (year / 100) + 3) / 4;   /* nr of "century leap years"*/
	day = (36525 * year) / 100 - cleaps + /* year * dayperyr, corrected */
			(1959 * month) / 64 +         /* months * daypermonth */
			TimeFields->Day -				/* day of the month */
			584817;							/* zero that on 1601-01-01 */
	/* done */

	Time->QuadPart = (((((LONGLONG)day * HOURSPERDAY +
		TimeFields->Hour) * MINSPERHOUR +
		TimeFields->Minute) * SECSPERMIN +
		TimeFields->Second) * 1000 +
		TimeFields->Millisecond) * TICKSPERMSEC;

	RETURN(TRUE);
}

// ******************************************************************
// * 0x0131 - RtlTimeToTimeFields()
// ******************************************************************
XBSYSAPI EXPORTNUM(305) xbox::void_xt NTAPI xbox::RtlTimeToTimeFields
(
	IN  PLARGE_INTEGER  Time,
	OUT PTIME_FIELDS    TimeFields
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Time)
		LOG_FUNC_ARG_OUT(TimeFields)
		LOG_FUNC_END;

	int SecondsInDay;
	long int cleaps, years, yearday, months;
	long int Days;
	LONGLONG _Time;

	/* Extract millisecond from time and convert time into seconds */
	TimeFields->Millisecond =
		(cshort_xt)((Time->QuadPart % TICKSPERSEC) / TICKSPERMSEC);
	_Time = Time->QuadPart / TICKSPERSEC;

	/* The native version of RtlTimeToTimeFields does not take leap seconds
	* into account */

	/* Split the time into days and seconds within the day */
	Days = (long int )(_Time / SECSPERDAY);
	SecondsInDay = _Time % SECSPERDAY;

	/* compute time of day */
	TimeFields->Hour = (cshort_xt)(SecondsInDay / SECSPERHOUR);
	SecondsInDay = SecondsInDay % SECSPERHOUR;
	TimeFields->Minute = (cshort_xt)(SecondsInDay / SECSPERMIN);
	TimeFields->Second = (cshort_xt)(SecondsInDay % SECSPERMIN);

	/* compute day of week */
	TimeFields->Weekday = (cshort_xt)((EPOCHWEEKDAY + Days) % DAYSPERWEEK);

	/* compute year, month and day of month. */
	cleaps = (3 * ((4 * Days + 1227) / DAYSPERQUADRICENTENNIUM) + 3) / 4;
	Days += 28188 + cleaps;
	years = (20 * Days - 2442) / (5 * DAYSPERNORMALQUADRENNIUM);
	yearday = Days - (years * DAYSPERNORMALQUADRENNIUM) / 4;
	months = (64 * yearday) / 1959;
	/* the result is based on a year starting on March.
	* To convert take 12 from Januari and Februari and
	* increase the year by one. */
	if (months < 14) {
		TimeFields->Month = (USHORT)(months - 1);
		TimeFields->Year = (USHORT)(years + 1524);
	}
	else {
		TimeFields->Month = (USHORT)(months - 13);
		TimeFields->Year = (USHORT)(years + 1525);
	}
	/* calculation of day of month is based on the wonderful
	* sequence of INT( n * 30.6): it reproduces the
	* 31-30-31-30-31-31 month lengths exactly for small n's */
	TimeFields->Day = (USHORT)(yearday - (1959 * months) / 64);
}

// ******************************************************************
// * 0x0132 - RtlTryEnterCriticalSection()
// ******************************************************************
XBSYSAPI EXPORTNUM(306) xbox::boolean_xt NTAPI xbox::RtlTryEnterCriticalSection
(
	IN PRTL_CRITICAL_SECTION CriticalSection
)
{
	LOG_FUNC_ONE_ARG(CriticalSection);

    BOOLEAN ret = false;
    HANDLE thread = (HANDLE)KeGetCurrentThread();

    if(InterlockedCompareExchange((::PLONG)(&CriticalSection->LockCount), 0, -1) == -1) {
        CriticalSection->OwningThread = thread;
        CriticalSection->RecursionCount = 1;
        ret = true;
    }
    else {
        if(CriticalSection->OwningThread == thread) {
            CriticalSection->LockCount++;
            CriticalSection->RecursionCount++;
            ret = true;
        }
    }

    RETURN(ret);
}

// ******************************************************************
// * 0x0133 - RtlUlongByteSwap()
// ******************************************************************
XBSYSAPI EXPORTNUM(307) xbox::ulong_xt FASTCALL xbox::RtlUlongByteSwap
(
	IN ulong_xt Source
)
{
	LOG_FUNC_ONE_ARG(Source);

	ULONG ret = (Source >> 24) | ((Source & 0xFF0000) >> 8) | ((Source & 0xFF00) << 8) | ((Source & 0xFF) << 24);

	RETURN(ret);
}

xbox::dword_xt WINAPI RtlUnicodeStringToAnsiSize(const xbox::UNICODE_STRING *str)
{
	const wchar_t *src = (const wchar_t *)(str->Buffer);
	xbox::dword_xt ret = wcsrtombs(nullptr, &src, (size_t)str->Length, nullptr);
	return ret + 1; // +1 for the terminating null character
}

// ******************************************************************
// * 0x0134 - RtlUnicodeStringToAnsiString()
// ******************************************************************
XBSYSAPI EXPORTNUM(308) xbox::ntstatus_xt NTAPI xbox::RtlUnicodeStringToAnsiString
(
	IN OUT PSTRING         DestinationString,
	IN     PUNICODE_STRING SourceString,
	IN     boolean_xt         AllocateDestinationString
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(DestinationString)
		LOG_FUNC_ARG(SourceString)
		LOG_FUNC_ARG(AllocateDestinationString)
		LOG_FUNC_END;

    NTSTATUS ret = xbox::status_success;
    dword_xt len = RtlUnicodeStringToAnsiSize(SourceString);

	DestinationString->Length = (USHORT)(len - 1);
    if (AllocateDestinationString) {
		DestinationString->MaximumLength = (USHORT)len;
		if (!(DestinationString->Buffer = (PCHAR)ExAllocatePoolWithTag(len, 'grtS'))) {
			return xbox::status_no_memory;
		}
    }
    else if (DestinationString->MaximumLength < len) {
		if (!DestinationString->MaximumLength) {
			return xbox::status_buffer_overflow;
		}

		DestinationString->Length = DestinationString->MaximumLength - 1;
        ret = xbox::status_buffer_overflow;
    }

    RtlUnicodeToMultiByteN(DestinationString->Buffer, DestinationString->Length, NULL, (PWSTR)SourceString->Buffer, (ULONG)SourceString->Length);
	DestinationString->Buffer[DestinationString->Length] = 0;

	RETURN(ret);
}

// ******************************************************************
// * 0x0135 - RtlUnicodeStringToInteger()
// ******************************************************************
XBSYSAPI EXPORTNUM(309) xbox::ntstatus_xt NTAPI xbox::RtlUnicodeStringToInteger
(
	IN     PUNICODE_STRING String,
	IN     ulong_xt Base,
	IN     PULONG Value
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(String)
		LOG_FUNC_ARG(Base)
		LOG_FUNC_ARG(Value)
		LOG_FUNC_END;

	LPWSTR lpwstr = (LPWSTR)String->Buffer;
	USHORT CharsRemaining = String->Length / sizeof(WCHAR);
	char bMinus = 0;
 
	while (CharsRemaining >= 1 && *lpwstr <= ' ') {
		lpwstr++;
		CharsRemaining--;
	}
 
	if (CharsRemaining >= 1) {
		if (*lpwstr == '+') {
			lpwstr++;
			CharsRemaining--;
		}
		else if (*lpwstr == '-') {
			bMinus = 1;
			lpwstr++;
			CharsRemaining--;
		}
	}
 
	if (Base == 0) {
		Base = 10;
 
		if (CharsRemaining >= 2 && lpwstr[0] == '0') {
			if (lpwstr[1] == 'b') {
				lpwstr += 2;
				CharsRemaining -= 2;
				Base = 2;
			}
			else if (lpwstr[1] == 'o') {
				lpwstr += 2;
				CharsRemaining -= 2;
				Base = 8;
			}
			else if (lpwstr[1] == 'x') {
				lpwstr += 2;
				CharsRemaining -= 2;
				Base = 16;
			}
		}
	}
	else if (Base != 2 && Base != 8 && Base != 10 && Base != 16) {
		return STATUS_INVALID_PARAMETER;
	}
 
	if (Value == NULL) {
		return STATUS_ACCESS_VIOLATION;
	}
 
	ULONG RunningTotal = 0;

	while (CharsRemaining >= 1) {
		WCHAR wchCurrent = *lpwstr;
		int digit;
 
		if (wchCurrent >= '0' && wchCurrent <= '9') {
			digit = wchCurrent - '0';
		}
		else if (wchCurrent >= 'A' && wchCurrent <= 'Z') {
			digit = wchCurrent - 'A' + 10;
		}
		else if (wchCurrent >= 'a' && wchCurrent <= 'z') {
			digit = wchCurrent - 'a' + 10;
		}
		else {
			digit = -1;
		}
 
		if (digit < 0 || (ULONG)digit >= Base) {
			break;
		}
 
		RunningTotal = RunningTotal * Base + digit;
		lpwstr++;
		CharsRemaining--;
	}
 
	*Value = bMinus ? (0 - RunningTotal) : RunningTotal;

	RETURN(xbox::status_success);
}

// ******************************************************************
// * 0x0136 - RtlUnicodeToMultiByteN()
// ******************************************************************
XBSYSAPI EXPORTNUM(310) xbox::ntstatus_xt NTAPI xbox::RtlUnicodeToMultiByteN
(
	IN PCHAR MultiByteString,
	IN ulong_xt MaxBytesInMultiByteString,
	IN PULONG BytesInMultiByteString,
	IN PWSTR UnicodeString,
	IN ulong_xt BytesInUnicodeString
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(MultiByteString)
		LOG_FUNC_ARG(MaxBytesInMultiByteString)
		LOG_FUNC_ARG(BytesInMultiByteString)
		LOG_FUNC_ARG(UnicodeString)
		LOG_FUNC_ARG(BytesInUnicodeString)
		LOG_FUNC_END;

	ULONG maxUnicodeChars = BytesInUnicodeString / sizeof(WCHAR);
	ULONG numChars = (maxUnicodeChars < MaxBytesInMultiByteString) ? maxUnicodeChars : MaxBytesInMultiByteString;

	if (BytesInMultiByteString != NULL) {
		*BytesInMultiByteString = numChars;
	}

	while (numChars) {
		*MultiByteString = (*UnicodeString < 0xff) ? (CHAR)(*UnicodeString) : '?';

		UnicodeString++;
		MultiByteString++;
		numChars--;
	}

	RETURN(xbox::status_success);
}

// ******************************************************************
// * 0x0137 - RtlUnicodeToMultiByteSize()
// ******************************************************************
XBSYSAPI EXPORTNUM(311) xbox::ntstatus_xt NTAPI xbox::RtlUnicodeToMultiByteSize
(
	IN PULONG BytesInMultiByteString,
	IN PWSTR UnicodeString,
	IN ulong_xt BytesInUnicodeString
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(BytesInMultiByteString)
		LOG_FUNC_ARG(UnicodeString)
		LOG_FUNC_ARG(BytesInUnicodeString)
		LOG_FUNC_END;

	*BytesInMultiByteString = BytesInUnicodeString * sizeof(WCHAR);

	RETURN(xbox::status_success);
}

// ******************************************************************
// * 0x0138 - RtlUnwind()
// ******************************************************************
XBSYSAPI EXPORTNUM(312) xbox::void_xt NTAPI xbox::RtlUnwind
(
	IN PVOID TargetFrame OPTIONAL,
	IN PVOID TargetIp OPTIONAL,
	IN PEXCEPTION_RECORD ExceptionRecord OPTIONAL,
	IN PVOID ReturnValue
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(TargetFrame)
		LOG_FUNC_ARG(TargetIp)
		LOG_FUNC_ARG(ExceptionRecord)
		LOG_FUNC_ARG(ReturnValue)
	LOG_FUNC_END;

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x0139 - RtlUpcaseUnicodeChar()
// ******************************************************************
XBSYSAPI EXPORTNUM(313) xbox::wchar_xt NTAPI xbox::RtlUpcaseUnicodeChar
(
	IN wchar_xt SourceCharacter
)
{
	LOG_FUNC_ONE_ARG(SourceCharacter);

	WCHAR result = towupper(SourceCharacter);

	RETURN(result);
}

// ******************************************************************
// * 0x013A - RtlUpcaseUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(314) xbox::ntstatus_xt NTAPI xbox::RtlUpcaseUnicodeString
(
	OUT PUNICODE_STRING DestinationString,
	IN  PUNICODE_STRING SourceString,
	IN  boolean_xt AllocateDestinationString
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(DestinationString)
		LOG_FUNC_ARG(SourceString)
		LOG_FUNC_ARG(AllocateDestinationString)
		LOG_FUNC_END;

	if (AllocateDestinationString) {
		DestinationString->MaximumLength = SourceString->Length;
		DestinationString->Buffer = (USHORT*)ExAllocatePoolWithTag((ULONG)DestinationString->MaximumLength, 'grtS');
		if (DestinationString->Buffer == NULL) {
			return xbox::status_no_memory;
		}
	}
	else {
		if (SourceString->Length > DestinationString->MaximumLength) {
			return xbox::status_buffer_overflow;
		}
	}

	ULONG length = ((ULONG)SourceString->Length) / sizeof(WCHAR);
	WCHAR *pDst = (WCHAR*)(DestinationString->Buffer);
	WCHAR *pSrc = (WCHAR*)(SourceString->Buffer);
	for (ULONG i = 0; i < length; i++) {
		pDst[i] = (WCHAR)towupper(pSrc[i]);
	}

	DestinationString->Length = SourceString->Length;

	RETURN(xbox::status_success);
}

// ******************************************************************
// * 0x013B - RtlUpcaseUnicodeToMultiByteN()
// ******************************************************************
XBSYSAPI EXPORTNUM(315) xbox::ntstatus_xt NTAPI xbox::RtlUpcaseUnicodeToMultiByteN
(
	IN OUT PCHAR MultiByteString,
	IN ulong_xt MaxBytesInMultiByteString,
	IN PULONG BytesInMultiByteString,
	IN PWSTR UnicodeString,
	IN ulong_xt BytesInUnicodeString
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(MultiByteString)
		LOG_FUNC_ARG(MaxBytesInMultiByteString)
		LOG_FUNC_ARG(BytesInMultiByteString)
		LOG_FUNC_ARG(UnicodeString)
		LOG_FUNC_ARG(BytesInUnicodeString)
		LOG_FUNC_END;

	ULONG maxUnicodeChars = BytesInUnicodeString / sizeof(WCHAR);
	ULONG numChars = (maxUnicodeChars < MaxBytesInMultiByteString) ? maxUnicodeChars : MaxBytesInMultiByteString;

	if (BytesInMultiByteString != NULL) {
		*BytesInMultiByteString = numChars;
	}

	while (numChars > 0) {
		WCHAR unicodeChar = (*UnicodeString < 256) ? *UnicodeString : L'?';
		unicodeChar = towupper(unicodeChar);

		*MultiByteString = (unicodeChar < 256) ? unicodeChar : '?';

		UnicodeString++;
		MultiByteString++;
		numChars--;
	}

	RETURN(xbox::status_success);
}

// ******************************************************************
// * 0x013C - RtlUpperChar()
// ******************************************************************
XBSYSAPI EXPORTNUM(316) xbox::char_xt NTAPI xbox::RtlUpperChar
(
	char_xt Character
)
{
	LOG_FUNC_ONE_ARG(Character);

	byte_xt CharCode = (byte_xt)Character;
	
	if (CharCode >= 'a' && CharCode <= 'z')
	{
		CharCode ^= 0x20;
	}
	
	// Latin alphabet (ISO 8859-1)
	else if (CharCode >= 0xe0 && CharCode <= 0xfe && CharCode != 0xf7)
	{
		CharCode ^= 0x20;
	}

	else if (CharCode == 0xFF)
	{
		CharCode = '?';
	}
	
	RETURN((char_xt)CharCode);
}

// ******************************************************************
// * 0x013D - RtlUpperString()
// ******************************************************************
XBSYSAPI EXPORTNUM(317) xbox::void_xt NTAPI xbox::RtlUpperString
(
	OUT PSTRING DestinationString,
	IN  PSTRING SourceString
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(DestinationString)
		LOG_FUNC_ARG(SourceString)
		LOG_FUNC_END;

	char_xt *pDst = DestinationString->Buffer;
	char_xt *pSrc = SourceString->Buffer;
	ULONG length = SourceString->Length;
	if ((USHORT)length > DestinationString->MaximumLength) {
		length = DestinationString->MaximumLength;
	}

	DestinationString->Length = (USHORT)length;
	while (length > 0) {
		*pDst++ = RtlUpperChar(*pSrc++);
		length--;
	}
}

// ******************************************************************
// * 0x013E - RtlUshortByteSwap()
// ******************************************************************
XBSYSAPI EXPORTNUM(318) xbox::ushort_xt FASTCALL xbox::RtlUshortByteSwap
(
	IN ushort_xt Source
)
{
	LOG_FUNC_ONE_ARG(Source);

	USHORT ret = (Source >> 8) | ((Source & 0xFF) << 8);

	RETURN(ret);
}

// ******************************************************************
// * 0x013F - RtlWalkFrameChain()
// ******************************************************************
XBSYSAPI EXPORTNUM(319) xbox::ulong_xt NTAPI xbox::RtlWalkFrameChain
(
	OUT PVOID *Callers,
	IN ulong_xt Count,
	IN ulong_xt Flags
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(Callers)
		LOG_FUNC_ARG(Count)
		LOG_FUNC_ARG(Flags)
	LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(NULL);
}

// ******************************************************************
// * 0x0140 - RtlZeroMemory()
// ******************************************************************
XBSYSAPI EXPORTNUM(320) xbox::void_xt NTAPI xbox::RtlZeroMemory
(
	IN void_xt UNALIGNED  *Destination,
	IN size_t           Length
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Destination)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_END;

	memset(Destination, 0, Length);
}

// ******************************************************************
// * 0x0160 - RtlRip
// ******************************************************************
XBSYSAPI EXPORTNUM(352) xbox::void_xt NTAPI xbox::RtlRip
(
	PCHAR	ApiName,
	PCHAR	Expression,
	PCHAR	Message
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(ApiName)
		LOG_FUNC_ARG(Expression)
		LOG_FUNC_ARG(Message)
		LOG_FUNC_END;

	EmuLog(LOG_LEVEL::WARNING, "RtlRip@%s:\n\nASSERT FAILED:\n%s\n\nDescription:\n%s",
		ApiName, Expression, Message);
}
