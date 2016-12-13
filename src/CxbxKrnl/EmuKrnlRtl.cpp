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
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

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
	PVOID   FailedAssertion, // PCHAR
	PVOID   FileName, // PCHAR
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

	CxbxKrnlCleanup("RtlAssert() raised by emulated program - consult Debug log");
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
// * 0x0115 - RtlEnterCriticalSection()
// ******************************************************************
XBSYSAPI EXPORTNUM(277) xboxkrnl::VOID NTAPI xboxkrnl::RtlEnterCriticalSection
(
	IN PRTL_CRITICAL_SECTION CriticalSection
)
{
	/** sorta pointless
	LOG_FUNC_ONE_ARG(CriticalSection);
	//*/

	//printf("CriticalSection->LockCount : %d\n", CriticalSection->LockCount);

	// This seems redundant, but xbox software doesn't always do it
	if (CriticalSection)
	{
		int iSection = FindCriticalSection(CriticalSection);

		if (iSection >= 0)
		{
			GlobalCriticalSections[iSection].XboxCriticalSection = CriticalSection;
			if (CriticalSection->LockCount < 0)
				NtDll::RtlInitializeCriticalSection(&GlobalCriticalSections[iSection].NativeCriticalSection);

			NtDll::RtlEnterCriticalSection(&GlobalCriticalSections[iSection].NativeCriticalSection);

			CriticalSection->LockCount = GlobalCriticalSections[iSection].NativeCriticalSection.LockCount;
			CriticalSection->RecursionCount = GlobalCriticalSections[iSection].NativeCriticalSection.RecursionCount;
			CriticalSection->OwningThread = GlobalCriticalSections[iSection].NativeCriticalSection.OwningThread;
		}

		//if(CriticalSection->LockCount == -1)
		//NtDll::RtlInitializeCriticalSection((NtDll::_RTL_CRITICAL_SECTION*)CriticalSection);

		//NtDll::RtlEnterCriticalSection((NtDll::_RTL_CRITICAL_SECTION*)CriticalSection);
	}
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
	if (CriticalSection == nullptr) {
		return;
	}

	/*
	LOG_FUNC_ONE_ARG(CriticalSection);
	//*/
	int iSection = FindCriticalSection(CriticalSection);

	if (iSection >= 0)
	{
		GlobalCriticalSections[iSection].XboxCriticalSection = CriticalSection;
		NtDll::RtlInitializeCriticalSection(&GlobalCriticalSections[iSection].NativeCriticalSection);

		CriticalSection->LockCount = GlobalCriticalSections[iSection].NativeCriticalSection.LockCount;
		CriticalSection->RecursionCount = GlobalCriticalSections[iSection].NativeCriticalSection.RecursionCount;
		CriticalSection->OwningThread = GlobalCriticalSections[iSection].NativeCriticalSection.OwningThread;
	}

	//NtDll::RtlInitializeCriticalSection((NtDll::_RTL_CRITICAL_SECTION*)CriticalSection);
}

// ******************************************************************
// * 0x0126 - RtlEnterCriticalSection()
// ******************************************************************
XBSYSAPI EXPORTNUM(294) xboxkrnl::VOID NTAPI xboxkrnl::RtlLeaveCriticalSection
(
	IN PRTL_CRITICAL_SECTION CriticalSection
)
{
	if (CriticalSection == nullptr) {
		return;
	}

	int iSection = FindCriticalSection(CriticalSection);

	if (iSection >= 0)
	{
		GlobalCriticalSections[iSection].XboxCriticalSection = CriticalSection;
		NtDll::RtlLeaveCriticalSection(&GlobalCriticalSections[iSection].NativeCriticalSection);

		CriticalSection->LockCount = GlobalCriticalSections[iSection].NativeCriticalSection.LockCount;
		CriticalSection->RecursionCount = GlobalCriticalSections[iSection].NativeCriticalSection.RecursionCount;
		CriticalSection->OwningThread = GlobalCriticalSections[iSection].NativeCriticalSection.OwningThread;
	}
	// Note: We need to execute this before debug output to avoid trouble
	//NtDll::RtlLeaveCriticalSection((NtDll::_RTL_CRITICAL_SECTION*)CriticalSection);

	/* sorta pointless
	LOG_FUNC_ONE_ARG(CriticalSection);
	//*/
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
	// Return on nullptr BEFORE logging!
	if (CriticalSection == nullptr) {
		return false;
	}

	LOG_FUNC_ONE_ARG(CriticalSection);

	BOOL bRet = FALSE;

	int iSection = FindCriticalSection(CriticalSection);

	if (iSection >= 0)
	{
		GlobalCriticalSections[iSection].XboxCriticalSection = CriticalSection;
		if (CriticalSection->LockCount < 0)
			NtDll::RtlInitializeCriticalSection(&GlobalCriticalSections[iSection].NativeCriticalSection);

		bRet = NtDll::RtlTryEnterCriticalSection(&GlobalCriticalSections[iSection].NativeCriticalSection);

		CriticalSection->LockCount = GlobalCriticalSections[iSection].NativeCriticalSection.LockCount;
		CriticalSection->RecursionCount = GlobalCriticalSections[iSection].NativeCriticalSection.RecursionCount;
		CriticalSection->OwningThread = GlobalCriticalSections[iSection].NativeCriticalSection.OwningThread;
	}

	//bRet = NtDll::RtlTryEnterCriticalSection((NtDll::PRTL_CRITICAL_SECTION)CriticalSection);

	RETURN(bRet);
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

