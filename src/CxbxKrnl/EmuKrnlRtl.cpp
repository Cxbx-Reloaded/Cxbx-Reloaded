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
// *   Cxbx->Win32->CxbxKrnl->EmuKrnlRtl.cpp
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
#define _XBOXKRNL_DEFEXTRN_

#define LOG_PREFIX "KRNL"

// prevent name collisions
namespace xboxkrnl
{
#include <xboxkrnl/xboxkrnl.h> // For RtlAnsiStringToUnicodeString, etc.
};

#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnlLogging.h"

// prevent name collisions
namespace NtDll
{
	#include "EmuNtDll.h"
};

#include "CxbxKrnl.h" // For CxbxKrnlCleanup()
#include "Emu.h" // For EmuWarning()

// A critical section containing the PC and Xbox equivalent
struct INTERNAL_CRITICAL_SECTION
{
	xboxkrnl::PRTL_CRITICAL_SECTION XboxCriticalSection;
	NtDll::_RTL_CRITICAL_SECTION NativeCriticalSection;
};

#define MAX_XBOX_CRITICAL_SECTIONS 1024
INTERNAL_CRITICAL_SECTION GlobalCriticalSections[MAX_XBOX_CRITICAL_SECTIONS] = { 0 };

void InitializeSectionStructures(void)
{
	ZeroMemory(GlobalCriticalSections, sizeof(GlobalCriticalSections));
}

int FindCriticalSection(xboxkrnl::PRTL_CRITICAL_SECTION CriticalSection)
{
	int FreeSection = -1;

	int iSection = 0;
	for (iSection = 0; iSection < MAX_XBOX_CRITICAL_SECTIONS; ++iSection)
	{
		if (GlobalCriticalSections[iSection].XboxCriticalSection == CriticalSection)
		{
			FreeSection = iSection;
			break;
		}
		else if (FreeSection < 0 && GlobalCriticalSections[iSection].XboxCriticalSection == NULL)
		{
			FreeSection = iSection;
		}
	}

	if (FreeSection < 0)
	{
		EmuWarning("Too many critical sections in use!\n");
	}

	return FreeSection;
}
extern uint8_t* get_thread_Teb();

// ******************************************************************
// * 0x0104 - RtlAnsiStringToUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(260) xboxkrnl::NTSTATUS NTAPI xboxkrnl::RtlAnsiStringToUnicodeString
(
	OUT PUNICODE_STRING DestinationString,
	IN PSTRING         SourceString,
	IN UCHAR           AllocateDestinationString
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(DestinationString)
		LOG_FUNC_ARG(SourceString)
		LOG_FUNC_ARG(AllocateDestinationString)
		LOG_FUNC_END;

	NTSTATUS ret = NtDll::RtlAnsiStringToUnicodeString((NtDll::UNICODE_STRING*)DestinationString, (NtDll::STRING*)SourceString, AllocateDestinationString);

	RETURN(ret);
}

// ******************************************************************
// * 0x0105 - RtlAppendStringToString()
// ******************************************************************
XBSYSAPI EXPORTNUM(261) xboxkrnl::NTSTATUS NTAPI xboxkrnl::RtlAppendStringToString
(
	IN PSTRING Destination,
	IN PSTRING Source
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Destination)
		LOG_FUNC_ARG(Source)
		LOG_FUNC_END;

	NTSTATUS result = NtDll::RtlAppendStringToString((NtDll::PSTRING)Destination, (NtDll::PSTRING)Source);

	RETURN(result);
}

// ******************************************************************
// * 0x0106 - RtlAppendUnicodeStringToString()
// ******************************************************************
XBSYSAPI EXPORTNUM(262) xboxkrnl::NTSTATUS NTAPI xboxkrnl::RtlAppendUnicodeStringToString
(
	IN PUNICODE_STRING Destination,
	IN PUNICODE_STRING Source
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Destination)
		LOG_FUNC_ARG(Source)
		LOG_FUNC_END;

	NTSTATUS result = NtDll::RtlAppendUnicodeStringToString((NtDll::PUNICODE_STRING)Destination, (NtDll::PUNICODE_STRING)Source);

	RETURN(result);
}

// ******************************************************************
// * 0x0107 - RtlAppendUnicodeToString()
// ******************************************************************
XBSYSAPI EXPORTNUM(263) xboxkrnl::NTSTATUS NTAPI xboxkrnl::RtlAppendUnicodeToString
(
	IN OUT PUNICODE_STRING Destination,
	IN LPCWSTR Source
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Destination)
		LOG_FUNC_ARG(Source)
		LOG_FUNC_END;

	NTSTATUS result = NtDll::RtlAppendUnicodeToString((NtDll::PUNICODE_STRING)Destination, (NtDll::PCWSTR)Source);

	RETURN(result);
}

// ******************************************************************
// * 0x0108 - RtlAssert()
// ******************************************************************
// Debug API?
XBSYSAPI EXPORTNUM(264) xboxkrnl::VOID NTAPI xboxkrnl::RtlAssert
(
	PCHAR   FailedAssertion,
	PCHAR   FileName,
	ULONG   LineNumber,
	PCHAR   Message
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(FailedAssertion)
		LOG_FUNC_ARG(FileName)
		LOG_FUNC_ARG(LineNumber)
		LOG_FUNC_ARG(Message)
		LOG_FUNC_END;

	CxbxPopupMessage("RtlAssert() raised by emulated program - consult Debug log");
}

// ******************************************************************
// * 0x010B - RtlCharToInteger()
// ******************************************************************
XBSYSAPI EXPORTNUM(267) xboxkrnl::NTSTATUS NTAPI xboxkrnl::RtlCharToInteger
(
	IN     PCSZ   String,
	IN     ULONG  Base OPTIONAL,
	OUT    PULONG Value
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(String)
		LOG_FUNC_ARG(Base)
		LOG_FUNC_ARG(Value)
		LOG_FUNC_END;

	NTSTATUS result = NtDll::RtlCharToInteger((NtDll::PCSZ)String, (NtDll::ULONG)Base, (NtDll::PULONG)Value);

	RETURN(result);
}

// ******************************************************************
// * 0x010C - RtlCompareMemory()
// ******************************************************************
// *
// * compare block of memory, return number of equivalent bytes.
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(268) xboxkrnl::BOOLEAN NTAPI xboxkrnl::RtlCompareMemory
(
	IN CONST VOID *Source1,
	IN CONST VOID *Source2,
	IN SIZE_T      Length
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Source1)
		LOG_FUNC_ARG(Source2)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_END;

	BOOL result = NtDll::RtlCompareMemory(Source1, Source2, Length);

	RETURN(result);
}

// ******************************************************************
// * 0x010D - RtlCompareMemoryUlong()
// ******************************************************************
XBSYSAPI EXPORTNUM(269) xboxkrnl::SIZE_T NTAPI xboxkrnl::RtlCompareMemoryUlong
(
	IN PVOID Source,
	IN SIZE_T Length,
	IN ULONG Pattern
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Source)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(Pattern)
		LOG_FUNC_END;

	SIZE_T result = NtDll::RtlCompareMemoryUlong(Source, Length, Pattern);

	RETURN(result);
}

// ******************************************************************
// * 0x010E - RtlCompareString()
// ******************************************************************
XBSYSAPI EXPORTNUM(270) xboxkrnl::LONG NTAPI xboxkrnl::RtlCompareString
(
	IN PSTRING String1,
	IN PSTRING String2,
	IN BOOLEAN CaseInSensitive
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(String1)
		LOG_FUNC_ARG(String2)
		LOG_FUNC_ARG(CaseInSensitive)
		LOG_FUNC_END;

	LONG result = NtDll::RtlCompareString((NtDll::STRING *)String1, (NtDll::STRING *)String2, CaseInSensitive);

	RETURN(result);
}

// ******************************************************************
// * 0x010F - RtlCompareUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(271) xboxkrnl::LONG NTAPI xboxkrnl::RtlCompareUnicodeString
(
	IN PUNICODE_STRING String1,
	IN PUNICODE_STRING String2,
	IN BOOLEAN CaseInSensitive
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(String1)
		LOG_FUNC_ARG(String2)
		LOG_FUNC_ARG(CaseInSensitive)
		LOG_FUNC_END;

	LONG result = NtDll::RtlCompareUnicodeString((NtDll::PUNICODE_STRING)String1, (NtDll::PUNICODE_STRING)String2, CaseInSensitive);

	RETURN(result);
}

// ******************************************************************
// * 0x0110 - RtlCopyString()
// ******************************************************************
XBSYSAPI EXPORTNUM(272) xboxkrnl::VOID NTAPI xboxkrnl::RtlCopyString
(
	OUT PSTRING DestinationString,
	IN PSTRING SourceString OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(DestinationString)
		LOG_FUNC_ARG(SourceString)
		LOG_FUNC_END;

	NtDll::RtlCopyString((NtDll::PSTRING)DestinationString, (NtDll::PSTRING)SourceString);
}

// ******************************************************************
// * 0x0111 - RtlCopyUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(273) xboxkrnl::VOID NTAPI xboxkrnl::RtlCopyUnicodeString
(
	OUT PUNICODE_STRING DestinationString,
	IN PUNICODE_STRING SourceString OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(DestinationString)
		LOG_FUNC_ARG(SourceString)
		LOG_FUNC_END;

	NtDll::RtlCopyUnicodeString((NtDll::PUNICODE_STRING)DestinationString, (NtDll::PUNICODE_STRING)SourceString);
}

// ******************************************************************
// * 0x0112 - RtlCreateUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(274) xboxkrnl::BOOLEAN NTAPI xboxkrnl::RtlCreateUnicodeString
(
	OUT PUNICODE_STRING DestinationString,
	IN PCWSTR SourceString
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(DestinationString)
		LOG_FUNC_ARG(SourceString)
		LOG_FUNC_END;

	BOOLEAN result = NtDll::RtlCreateUnicodeString((NtDll::PUNICODE_STRING)DestinationString, (NtDll::PCWSTR)SourceString);

	RETURN(result);
}

// ******************************************************************
// * 0x0113 - RtlDowncaseUnicodeChar()
// ******************************************************************
XBSYSAPI EXPORTNUM(275) xboxkrnl::WCHAR NTAPI xboxkrnl::RtlDowncaseUnicodeChar
(
	IN WCHAR SourceCharacter
)
{
	LOG_FUNC_ONE_ARG(SourceCharacter);

	WCHAR result = NtDll::RtlDowncaseUnicodeChar((NtDll::WCHAR)SourceCharacter);

	RETURN(result);
}

// ******************************************************************
// * 0x0114 - RtlDowncaseUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(276) xboxkrnl::NTSTATUS NTAPI xboxkrnl::RtlDowncaseUnicodeString
(
	OUT PUNICODE_STRING DestinationString,
	IN PUNICODE_STRING SourceString,
	IN BOOLEAN AllocateDestinationString
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(DestinationString)
		LOG_FUNC_ARG(SourceString)
		LOG_FUNC_ARG(AllocateDestinationString)
		LOG_FUNC_END;

	NTSTATUS result = NtDll::RtlDowncaseUnicodeString(
		(NtDll::PUNICODE_STRING)DestinationString,
		(NtDll::PUNICODE_STRING)SourceString,
		AllocateDestinationString);

	RETURN(result);
}

// ******************************************************************
// * 0x0115 - RtlEnterCriticalSection()
// ******************************************************************
XBSYSAPI EXPORTNUM(277) xboxkrnl::VOID NTAPI xboxkrnl::RtlEnterCriticalSection
(
    IN PRTL_CRITICAL_SECTION CriticalSection
)
{
    LOG_FUNC_ONE_ARG(CriticalSection);

    HANDLE thread_Teb = (HANDLE)get_thread_Teb();

    CriticalSection->LockCount++;
    if(CriticalSection->LockCount == 0) {
        CriticalSection->OwningThread = thread_Teb;
        CriticalSection->RecursionCount = 1;
    }
    else {
        if(CriticalSection->OwningThread != thread_Teb) {
            KeWaitForSingleObject(
                (PVOID)CriticalSection,
                (KWAIT_REASON)0,
                (KPROCESSOR_MODE)0,
                (BOOLEAN)0,
                (PLARGE_INTEGER)0
            );
            CriticalSection->OwningThread = thread_Teb;
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
XBSYSAPI EXPORTNUM(278) xboxkrnl::VOID NTAPI xboxkrnl::RtlEnterCriticalSectionAndRegion
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
XBSYSAPI EXPORTNUM(279) xboxkrnl::BOOLEAN NTAPI xboxkrnl::RtlEqualString
(
	IN PSTRING String1,
	IN PSTRING String2,
	IN BOOLEAN CaseSensitive
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(String1)
		LOG_FUNC_ARG(String2)
		LOG_FUNC_ARG(CaseSensitive)
		LOG_FUNC_END;

	BOOLEAN bRet = NtDll::RtlEqualString((NtDll::PSTRING)String1, (NtDll::PSTRING)String2, (NtDll::BOOLEAN)CaseSensitive);

	RETURN(bRet);
}

// ******************************************************************
// * 0x0118 - RtlEqualUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(280) xboxkrnl::BOOLEAN NTAPI xboxkrnl::RtlEqualUnicodeString
(
	IN PUNICODE_STRING String1,
	IN PUNICODE_STRING String2,
	IN BOOLEAN CaseSensitive
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(String1)
		LOG_FUNC_ARG(String2)
		LOG_FUNC_ARG(CaseSensitive)
		LOG_FUNC_END;

	BOOLEAN bRet = NtDll::RtlEqualUnicodeString((NtDll::PUNICODE_STRING)String1, (NtDll::PUNICODE_STRING)String2, (NtDll::BOOLEAN)CaseSensitive);

	RETURN(bRet);
}

// ******************************************************************
// * 0x0119 - RtlExtendedIntegerMultiply()
// ******************************************************************
XBSYSAPI EXPORTNUM(281) xboxkrnl::LARGE_INTEGER NTAPI xboxkrnl::RtlExtendedIntegerMultiply
(
	IN LARGE_INTEGER Multiplicand,
	IN LONG Multiplier
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Multiplicand)
		LOG_FUNC_ARG(Multiplier)
		LOG_FUNC_END;

	LARGE_INTEGER ret;

	// As long as there are no type casts for NtDll::LARGE_INTEGER to xboxkrnl::LARGE_INTEGER
	// and back, just copy the only member manually :
	// TODO : Simplify this by adding typecasts between NtDll and xboxkrnl versions of LARGE_INTEGER
	NtDll::LARGE_INTEGER NtMultiplicand;
	NtMultiplicand.QuadPart = Multiplicand.QuadPart;

	ret.QuadPart = NtDll::RtlExtendedIntegerMultiply(NtMultiplicand, (NtDll::LONG)Multiplier).QuadPart;

	RETURN(ret);
}

// ******************************************************************
// * 0x011A - RtlExtendedLargeIntegerDivide()
// ******************************************************************
XBSYSAPI EXPORTNUM(282) xboxkrnl::LARGE_INTEGER NTAPI xboxkrnl::RtlExtendedLargeIntegerDivide
(
	IN	LARGE_INTEGER Dividend,
	IN	ULONG Divisor,
	IN	PULONG Remainder // OUT? OPTIONAL?
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Dividend)
		LOG_FUNC_ARG(Divisor)
		LOG_FUNC_ARG(Remainder)
		LOG_FUNC_END;

	LARGE_INTEGER ret;

	// As long as there are no type casts for NtDll::LARGE_INTEGER to xboxkrnl::LARGE_INTEGER
	// and back, just copy the only member manually :
	// TODO : Simplify this by adding typecasts between NtDll and xboxkrnl versions of LARGE_INTEGER
	NtDll::LARGE_INTEGER NtDividend;
	NtDividend.QuadPart = Dividend.QuadPart;

	ret.QuadPart = NtDll::RtlExtendedLargeIntegerDivide(NtDividend, (NtDll::ULONG)Divisor, (NtDll::PULONG)Remainder).QuadPart;

	RETURN(ret);
}

// ******************************************************************
// * 0x011B - RtlExtendedMagicDivide()
// ******************************************************************
XBSYSAPI EXPORTNUM(283) xboxkrnl::LARGE_INTEGER NTAPI xboxkrnl::RtlExtendedMagicDivide
(
	IN	LARGE_INTEGER Dividend,
	IN	LARGE_INTEGER MagicDivisor,
	IN	CCHAR ShiftCount
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Dividend)
		LOG_FUNC_ARG(MagicDivisor)
		LOG_FUNC_ARG(ShiftCount)
		LOG_FUNC_END;

	LARGE_INTEGER ret;

	// As long as there are no type casts for NtDll::LARGE_INTEGER to xboxkrnl::LARGE_INTEGER
	// and back, just copy the only member manually :
	// TODO : Simplify this by adding typecasts between NtDll and xboxkrnl versions of LARGE_INTEGER
	NtDll::LARGE_INTEGER NtDividend;
	NtDividend.QuadPart = Dividend.QuadPart;

	NtDll::LARGE_INTEGER NtMagicDivisor;
	NtMagicDivisor.QuadPart = MagicDivisor.QuadPart;

	ret.QuadPart = NtDll::RtlExtendedMagicDivide(NtDividend, NtMagicDivisor, (NtDll::CCHAR)ShiftCount).QuadPart;

	RETURN(ret);
}

// Prevent errors compiling RtlFillMemory (TODO : How should we really do this?)
#undef RtlFillMemory

// ******************************************************************
// * 0x011C - RtlFillMemory()
// ******************************************************************
XBSYSAPI EXPORTNUM(284) xboxkrnl::VOID NTAPI xboxkrnl::RtlFillMemory
(
	IN VOID UNALIGNED *Destination,
	IN DWORD Length,
	IN BYTE  Fill
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Destination)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(Fill)
		LOG_FUNC_END;

	NtDll::RtlFillMemory(Destination, Length, Fill);
}

// ******************************************************************
// * 0x011D - RtlFillMemoryUlong()
// ******************************************************************
XBSYSAPI EXPORTNUM(285) xboxkrnl::VOID NTAPI xboxkrnl::RtlFillMemoryUlong
(
	IN PVOID Destination,
	IN SIZE_T Length,
	IN ULONG Pattern
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Destination)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(Pattern)
		LOG_FUNC_END;

	NtDll::RtlFillMemoryUlong(Destination, Length, Pattern);
}

// ******************************************************************
// * 0x011E - RtlFreeAnsiString()
// ******************************************************************
XBSYSAPI EXPORTNUM(286) xboxkrnl::VOID NTAPI xboxkrnl::RtlFreeAnsiString
(
	IN OUT PANSI_STRING AnsiString
)
{
	LOG_FUNC_ONE_ARG(AnsiString);

	NtDll::RtlFreeAnsiString((NtDll::PANSI_STRING)AnsiString);
}

// ******************************************************************
// * 0x011F - RtlFreeUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(287) xboxkrnl::VOID NTAPI xboxkrnl::RtlFreeUnicodeString
(
	IN OUT PUNICODE_STRING UnicodeString
)
{
	LOG_FUNC_ONE_ARG(UnicodeString);

	NtDll::RtlFreeUnicodeString((NtDll::PUNICODE_STRING)UnicodeString);
}

// ******************************************************************
// * 0x0121 - RtlInitAnsiString()
// ******************************************************************
XBSYSAPI EXPORTNUM(289) xboxkrnl::VOID NTAPI xboxkrnl::RtlInitAnsiString
(
	IN OUT PANSI_STRING DestinationString,
	IN     PCSZ         SourceString
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(DestinationString)
		LOG_FUNC_ARG(SourceString)
		LOG_FUNC_END;

	NtDll::RtlInitAnsiString((NtDll::PANSI_STRING)DestinationString, (NtDll::PCSZ)SourceString);
}

// ******************************************************************
// * 0x0122 - RtlInitUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(290) xboxkrnl::VOID NTAPI xboxkrnl::RtlInitUnicodeString
(
	IN OUT PUNICODE_STRING DestinationString,
	IN     PSTRING         SourceString
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(DestinationString)
		LOG_FUNC_ARG(SourceString)
		LOG_FUNC_END;

	NtDll::RtlInitUnicodeString((NtDll::PUNICODE_STRING)DestinationString, (NtDll::PCWSTR)SourceString);
}

// ******************************************************************
// * 0x0123 - RtlInitializeCriticalSection()
// ******************************************************************
XBSYSAPI EXPORTNUM(291) xboxkrnl::VOID NTAPI xboxkrnl::RtlInitializeCriticalSection
(
    IN PRTL_CRITICAL_SECTION CriticalSection
)
{
    LOG_FUNC_ONE_ARG(CriticalSection);

    CriticalSection->LockCount = -1;
    // Sets byte 0 = 1, and byte 2 = 4 of Unknown[0] for some reason
    CriticalSection->Unknown[0] = (CriticalSection->Unknown[0] & ~0xFF) | 0x1;
    CriticalSection->Unknown[0] = (CriticalSection->Unknown[0] & ~0xFF0000) | 0x40000;
    CriticalSection->Unknown[1] = 0;
    CriticalSection->Unknown[3] = (DWORD)&CriticalSection->Unknown[2];
    CriticalSection->Unknown[2] = (DWORD)&CriticalSection->Unknown[2];
    CriticalSection->RecursionCount = 0;
    CriticalSection->OwningThread = 0;
}

// ******************************************************************
// * 0x0124 - RtlIntegerToChar()
// ******************************************************************
XBSYSAPI EXPORTNUM(292) xboxkrnl::NTSTATUS NTAPI xboxkrnl::RtlIntegerToChar
(
	IN ULONG Value,
	IN ULONG Base,
	IN LONG OutputLength,
	IN PSZ String
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Value)
		LOG_FUNC_ARG(Base)
		LOG_FUNC_ARG(OutputLength)
		LOG_FUNC_ARG(String)
		LOG_FUNC_END;

	NTSTATUS result = NtDll::RtlIntegerToChar(Value, Base, OutputLength, String);

	RETURN(result);
}

// ******************************************************************
// * 0x0125 - RtlIntegerToUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(293) xboxkrnl::NTSTATUS NTAPI xboxkrnl::RtlIntegerToUnicodeString
(
	IN     ULONG Value,
	IN     ULONG Base,
	IN     PUNICODE_STRING String
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Value)
		LOG_FUNC_ARG(Base)
		LOG_FUNC_ARG_OUT(String)
		LOG_FUNC_END;

	NTSTATUS result = NtDll::RtlIntegerToUnicodeString(Value, Base, (NtDll::PUNICODE_STRING)String);

	RETURN(result);
}

// ******************************************************************
// * 0x0126 - RtlLeaveCriticalSection()
// ******************************************************************
XBSYSAPI EXPORTNUM(294) xboxkrnl::VOID NTAPI xboxkrnl::RtlLeaveCriticalSection
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
            KeSetEvent((PRKEVENT)CriticalSection, (KPRIORITY)1, (BOOLEAN)0);
        }
    }
}

// ******************************************************************
// * 0x0127 - RtlLeaveCriticalSectionAndRegion()
// ******************************************************************
XBSYSAPI EXPORTNUM(295) xboxkrnl::VOID NTAPI xboxkrnl::RtlLeaveCriticalSectionAndRegion
(
    IN PRTL_CRITICAL_SECTION CriticalSection
)
{
    LOG_FUNC_ONE_ARG(CriticalSection);

    RtlLeaveCriticalSection(CriticalSection);
    KeLeaveCriticalRegion();
}

// ******************************************************************
// * 0x0128 - RtlLowerChar()
// ******************************************************************
XBSYSAPI EXPORTNUM(296) xboxkrnl::CHAR NTAPI xboxkrnl::RtlLowerChar
(
	CHAR Character
)
{
	LOG_FUNC_ONE_ARG(Character);

	CHAR ret = tolower(Character);

	RETURN(ret);
}

// ******************************************************************
// * 0x0129 - RtlMapGenericMask()
// ******************************************************************
XBSYSAPI EXPORTNUM(297) xboxkrnl::VOID NTAPI xboxkrnl::RtlMapGenericMask
(
	IN PACCESS_MASK AccessMask,
	IN PGENERIC_MAPPING GenericMapping
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(AccessMask)
		LOG_FUNC_ARG(GenericMapping)
		LOG_FUNC_END;

	NtDll::RtlMapGenericMask(AccessMask, (NtDll::PGENERIC_MAPPING)GenericMapping);
}

// Prevent errors compiling RtlMoveMemory (TODO : How should we really do this?)
#undef RtlMoveMemory

// ******************************************************************
// * 0x012A - RtlMoveMemory()
// ******************************************************************
XBSYSAPI EXPORTNUM(298) xboxkrnl::VOID NTAPI xboxkrnl::RtlMoveMemory
(
	IN VOID UNALIGNED       *Destination,
	IN CONST VOID UNALIGNED *Source,
	IN SIZE_T                Length
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Destination)
		LOG_FUNC_ARG(Source)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_END;

	::memmove(Destination, Source, Length);
}

// ******************************************************************
// * 0x012B - RtlMultiByteToUnicodeN()
// ******************************************************************
XBSYSAPI EXPORTNUM(299) xboxkrnl::NTSTATUS NTAPI xboxkrnl::RtlMultiByteToUnicodeN
(
	IN     PWSTR UnicodeString,
	IN     ULONG MaxBytesInUnicodeString,
	IN     PULONG BytesInUnicodeString,
	IN     PCHAR MultiByteString,
	IN     ULONG BytesInMultiByteString
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(UnicodeString)
		LOG_FUNC_ARG(MaxBytesInUnicodeString);
		LOG_FUNC_ARG(BytesInUnicodeString);
		LOG_FUNC_ARG(MultiByteString);
		LOG_FUNC_ARG(BytesInMultiByteString)
		LOG_FUNC_END;

	NTSTATUS result = NtDll::RtlMultiByteToUnicodeN(
		UnicodeString,
		MaxBytesInUnicodeString,
		BytesInUnicodeString,
		MultiByteString,
		BytesInMultiByteString);

	RETURN(result);
}

// ******************************************************************
// * 0x012C - RtlMultiByteToUnicodeSize()
// ******************************************************************
XBSYSAPI EXPORTNUM(300) xboxkrnl::NTSTATUS NTAPI xboxkrnl::RtlMultiByteToUnicodeSize
(
	IN PULONG BytesInUnicodeString,
	IN PCHAR MultiByteString,
	IN ULONG BytesInMultiByteString
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(BytesInUnicodeString)
		LOG_FUNC_ARG(MultiByteString);
		LOG_FUNC_ARG(BytesInMultiByteString)
		LOG_FUNC_END;

	NTSTATUS result = NtDll::RtlMultiByteToUnicodeSize(
		BytesInUnicodeString,
		MultiByteString,
		BytesInMultiByteString);

	RETURN(result);
}

// ******************************************************************
// * 0x012D - RtlNtStatusToDosError()
// ******************************************************************
XBSYSAPI EXPORTNUM(301) xboxkrnl::ULONG NTAPI xboxkrnl::RtlNtStatusToDosError
(
	IN NTSTATUS Status
)
{
	LOG_FUNC_ONE_ARG(Status);

	ULONG ret = NtDll::RtlNtStatusToDosError(Status);

	RETURN(ret);
}

// ******************************************************************
// * 0x0130 - RtlTimeFieldsToTime()
// ******************************************************************
XBSYSAPI EXPORTNUM(304) xboxkrnl::BOOLEAN NTAPI xboxkrnl::RtlTimeFieldsToTime
(
	IN  PTIME_FIELDS    TimeFields,
	OUT PLARGE_INTEGER  Time
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(TimeFields)
		LOG_FUNC_ARG_OUT(Time)
		LOG_FUNC_END;

	BOOLEAN bRet = NtDll::RtlTimeFieldsToTime((NtDll::TIME_FIELDS*)TimeFields, (NtDll::LARGE_INTEGER*)Time);

	RETURN(bRet);
}

// ******************************************************************
// * 0x0131 - RtlTimeToTimeFields()
// ******************************************************************
XBSYSAPI EXPORTNUM(305) xboxkrnl::VOID NTAPI xboxkrnl::RtlTimeToTimeFields
(
	IN  PLARGE_INTEGER  Time,
	OUT PTIME_FIELDS    TimeFields
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Time)
		LOG_FUNC_ARG_OUT(TimeFields)
		LOG_FUNC_END;

	NtDll::RtlTimeToTimeFields((NtDll::LARGE_INTEGER*)Time, (NtDll::TIME_FIELDS*)TimeFields);
}

// ******************************************************************
// * 0x0132 - RtlTryEnterCriticalSection()
// ******************************************************************
XBSYSAPI EXPORTNUM(306) xboxkrnl::BOOLEAN NTAPI xboxkrnl::RtlTryEnterCriticalSection
(
	IN PRTL_CRITICAL_SECTION CriticalSection
)
{
	LOG_FUNC_ONE_ARG(CriticalSection);

    BOOLEAN ret = false;

    #ifdef _WIN32
        LONG exchange_result = InterlockedCompareExchange(&CriticalSection->LockCount, 0, -1);
    #elif __linux__
        // Typing will likely be different on Linux
        LONG exchange_result = __sync_val_compare_and_swap(&CriticalSection->LockCount, 0, -1);
    #endif

    if(exchange_result == -1) {
        CriticalSection->OwningThread = (HANDLE)get_thread_Teb();
        CriticalSection->RecursionCount = 1;
        ret = true;
    }
    else {
        if(CriticalSection->OwningThread == (HANDLE)get_thread_Teb()) {
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
XBSYSAPI EXPORTNUM(307) xboxkrnl::ULONG FASTCALL xboxkrnl::RtlUlongByteSwap
(
	IN ULONG Source
)
{
	LOG_FUNC_ONE_ARG(Source);

	ULONG ret = NtDll::RtlUlongByteSwap(Source);

	RETURN(ret);
}

// ******************************************************************
// * 0x0134 - RtlUnicodeStringToAnsiString()
// ******************************************************************
XBSYSAPI EXPORTNUM(308) xboxkrnl::NTSTATUS NTAPI xboxkrnl::RtlUnicodeStringToAnsiString
(
	IN OUT PSTRING         DestinationString,
	IN     PUNICODE_STRING SourceString,
	IN     BOOLEAN         AllocateDestinationString
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(DestinationString)
		LOG_FUNC_ARG(SourceString)
		LOG_FUNC_ARG(AllocateDestinationString)
		LOG_FUNC_END;

	NTSTATUS ret = NtDll::RtlUnicodeStringToAnsiString((NtDll::STRING*)DestinationString, (NtDll::UNICODE_STRING*)SourceString, AllocateDestinationString);

	RETURN(ret);
}

// ******************************************************************
// * 0x0135 - RtlUnicodeStringToInteger()
// ******************************************************************
XBSYSAPI EXPORTNUM(309) xboxkrnl::NTSTATUS NTAPI xboxkrnl::RtlUnicodeStringToInteger
(
	IN     PUNICODE_STRING String,
	IN     ULONG Base,
	IN     PULONG Value
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(String)
		LOG_FUNC_ARG(Base)
		LOG_FUNC_ARG(Value)
		LOG_FUNC_END;

	NTSTATUS ret = NtDll::RtlUnicodeStringToInteger((NtDll::PUNICODE_STRING)String, Base, Value);

	RETURN(ret);
}

// ******************************************************************
// * 0x0136 - RtlUnicodeToMultiByteN()
// ******************************************************************
XBSYSAPI EXPORTNUM(310) xboxkrnl::NTSTATUS NTAPI xboxkrnl::RtlUnicodeToMultiByteN
(
	IN PCHAR MultiByteString,
	IN ULONG MaxBytesInMultiByteString,
	IN PULONG BytesInMultiByteString,
	IN PWSTR UnicodeString,
	IN ULONG BytesInUnicodeString
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(MultiByteString)
		LOG_FUNC_ARG(MaxBytesInMultiByteString)
		LOG_FUNC_ARG(BytesInMultiByteString)
		LOG_FUNC_ARG(UnicodeString)
		LOG_FUNC_ARG(BytesInUnicodeString)
		LOG_FUNC_END;

	NTSTATUS ret = NtDll::RtlUnicodeToMultiByteN(
		MultiByteString,
		MaxBytesInMultiByteString,
		BytesInMultiByteString,
		UnicodeString,
		BytesInUnicodeString);

	RETURN(ret);
}

// ******************************************************************
// * 0x0137 - RtlUnicodeToMultiByteSize()
// ******************************************************************
XBSYSAPI EXPORTNUM(311) xboxkrnl::NTSTATUS NTAPI xboxkrnl::RtlUnicodeToMultiByteSize
(
	IN PULONG BytesInMultiByteString,
	IN PWSTR UnicodeString,
	IN ULONG BytesInUnicodeString
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(BytesInMultiByteString)
		LOG_FUNC_ARG(UnicodeString)
		LOG_FUNC_ARG(BytesInUnicodeString)
		LOG_FUNC_END;

	NTSTATUS ret = NtDll::RtlUnicodeToMultiByteSize(
		BytesInMultiByteString,
		UnicodeString,
		BytesInUnicodeString);

	RETURN(ret);
}

// ******************************************************************
// * 0x0139 - RtlUpcaseUnicodeChar()
// ******************************************************************
XBSYSAPI EXPORTNUM(313) xboxkrnl::WCHAR NTAPI xboxkrnl::RtlUpcaseUnicodeChar
(
	IN WCHAR SourceCharacter
)
{
	LOG_FUNC_ONE_ARG(SourceCharacter);

	WCHAR result = NtDll::RtlUpcaseUnicodeChar((NtDll::WCHAR)SourceCharacter);

	RETURN(result);
}

// ******************************************************************
// * 0x013A - RtlUpcaseUnicodeString()
// ******************************************************************
XBSYSAPI EXPORTNUM(314) xboxkrnl::NTSTATUS NTAPI xboxkrnl::RtlUpcaseUnicodeString
(
	OUT PUNICODE_STRING DestinationString,
	IN  PUNICODE_STRING SourceString,
	IN  BOOLEAN AllocateDestinationString
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(DestinationString)
		LOG_FUNC_ARG(SourceString)
		LOG_FUNC_ARG(AllocateDestinationString)
		LOG_FUNC_END;

	NTSTATUS result = NtDll::RtlUpcaseUnicodeString(
		(NtDll::PUNICODE_STRING)DestinationString,
		(NtDll::PUNICODE_STRING)SourceString,
		AllocateDestinationString);

	RETURN(result);
}

// ******************************************************************
// * 0x013B - RtlUpcaseUnicodeToMultiByteN()
// ******************************************************************
XBSYSAPI EXPORTNUM(315) xboxkrnl::NTSTATUS NTAPI xboxkrnl::RtlUpcaseUnicodeToMultiByteN
(
	IN OUT PCHAR MultiByteString,
	IN ULONG MaxBytesInMultiByteString,
	IN PULONG BytesInMultiByteString,
	IN PWSTR UnicodeString,
	IN ULONG BytesInUnicodeString
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(MultiByteString)
		LOG_FUNC_ARG(MaxBytesInMultiByteString)
		LOG_FUNC_ARG(BytesInMultiByteString)
		LOG_FUNC_ARG(UnicodeString)
		LOG_FUNC_ARG(BytesInUnicodeString)
		LOG_FUNC_END;

	NTSTATUS result = NtDll::RtlUpcaseUnicodeToMultiByteN(
		MultiByteString,
		MaxBytesInMultiByteString,
		BytesInMultiByteString,
		UnicodeString,
		BytesInUnicodeString);
		
	RETURN(result);
}

// ******************************************************************
// * 0x013C - RtlUpperChar()
// ******************************************************************
XBSYSAPI EXPORTNUM(316) xboxkrnl::CHAR NTAPI xboxkrnl::RtlUpperChar
(
	CHAR Character
)
{
	LOG_FUNC_ONE_ARG(Character);

	CHAR ret = toupper(Character);

	RETURN(ret);
}

// ******************************************************************
// * 0x013D - RtlUpperString()
// ******************************************************************
XBSYSAPI EXPORTNUM(317) xboxkrnl::VOID NTAPI xboxkrnl::RtlUpperString
(
	OUT PSTRING DestinationString,
	IN  PSTRING SourceString
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(DestinationString)
		LOG_FUNC_ARG(SourceString)
		LOG_FUNC_END;

	NtDll::RtlUpperString((NtDll::PSTRING)DestinationString, (NtDll::PSTRING)SourceString);
}

// ******************************************************************
// * 0x013E - RtlUshortByteSwap()
// ******************************************************************
XBSYSAPI EXPORTNUM(318) xboxkrnl::USHORT FASTCALL xboxkrnl::RtlUshortByteSwap
(
	IN USHORT Source
)
{
	LOG_FUNC_ONE_ARG(Source);

	USHORT ret = NtDll::RtlUshortByteSwap(Source);

	RETURN(ret);
}

// Prevent errors compiling RtlZeroMemory (TODO : How should we really do this?)
#undef RtlZeroMemory

// ******************************************************************
// * 0x0140 - RtlZeroMemory()
// ******************************************************************
XBSYSAPI EXPORTNUM(320) xboxkrnl::VOID NTAPI xboxkrnl::RtlZeroMemory
(
	IN VOID UNALIGNED  *Destination,
	IN SIZE_T           Length
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Destination)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_END;

	memset(Destination, 0, Length); // Don't bother with NtDll::RtlZeroMemory
}

// ******************************************************************
// * 0x0160 - RtlRip
// ******************************************************************
XBSYSAPI EXPORTNUM(352) xboxkrnl::VOID NTAPI xboxkrnl::RtlRip
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

	EmuWarning("RtlRip@%s:\n\nASSERT FAILED:\n%s\n\nDescription:\n%s",
		ApiName, Expression, Message);
}

