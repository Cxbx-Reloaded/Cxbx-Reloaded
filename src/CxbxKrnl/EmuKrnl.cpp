// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuKrnl.cpp
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
// *
// *  All rights reserved
// *
// ******************************************************************
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

// prevent name collisions
namespace xboxkrnl
{
    #include <xboxkrnl/xboxkrnl.h>
};

#include <cstdio>
#include <cctype>
#include <clocale>
//#include <process.h>

#include "Logging.h"
#include "EmuKrnlLogging.h"
#include "CxbxKrnl.h"

// prevent name collisions
namespace NtDll
{
    #include "EmuNtDll.h"
};

// ******************************************************************
// * 0x0033 - InterlockedCompareExchange()
// ******************************************************************
// Source:ReactOS
XBSYSAPI EXPORTNUM(51) xboxkrnl::LONG FASTCALL xboxkrnl::KRNL(InterlockedCompareExchange)
(
	IN OUT PLONG VOLATILE Destination,
	IN LONG  Exchange,
	IN LONG  Comparand
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Destination)
		LOG_FUNC_ARG(Exchange)
		LOG_FUNC_ARG(Comparand)
		LOG_FUNC_END;

	RETURN(InterlockedCompareExchange((NtDll::PLONG)Destination, (NtDll::LONG)Exchange, (NtDll::LONG)Comparand));
}

// ******************************************************************
// * 0x0034 - InterlockedDecrement()
// ******************************************************************
// Source:ReactOS
XBSYSAPI EXPORTNUM(52) xboxkrnl::LONG FASTCALL xboxkrnl::KRNL(InterlockedDecrement)
(
	IN OUT PLONG Addend
)
{
	LOG_FUNC_ONE_ARG(Addend);

	RETURN(InterlockedDecrement((NtDll::PLONG)Addend));
}

// ******************************************************************
// * 0x0035 - InterlockedIncrement()
// ******************************************************************
// Source:ReactOS
XBSYSAPI EXPORTNUM(53) xboxkrnl::LONG FASTCALL xboxkrnl::KRNL(InterlockedIncrement)
(
	IN OUT PLONG Addend
)
{
	LOG_FUNC_ONE_ARG(Addend);

	RETURN(InterlockedIncrement((NtDll::PLONG)Addend));
}

// ******************************************************************
// * 0x0036 - InterlockedExchange()
// ******************************************************************
// Source:ReactOS
XBSYSAPI EXPORTNUM(54) xboxkrnl::LONG FASTCALL xboxkrnl::KRNL(InterlockedExchange)
(
	IN PLONG VOLATILE Destination,
	IN LONG Value
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Destination)
		LOG_FUNC_ARG(Value)
		LOG_FUNC_END;

	RETURN(InterlockedExchange((NtDll::PLONG)Destination, (NtDll::LONG)Value));
}

// ******************************************************************
// * 0x0037 - InterlockedExchangeAdd()
// ******************************************************************
// Source:ReactOS
XBSYSAPI EXPORTNUM(55) xboxkrnl::LONG FASTCALL xboxkrnl::KRNL(InterlockedExchangeAdd)
(
	IN PLONG VOLATILE Addend,
	IN LONG	Value
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Addend)
		LOG_FUNC_ARG(Value)
		LOG_FUNC_END;

	RETURN(InterlockedExchangeAdd((NtDll::PLONG)Addend, (NtDll::LONG)Value));
}

// ******************************************************************
// * 0x0038 - InterlockedFlushSList()
// ******************************************************************
// Source:ReactOS
// Dxbx Note : The Xbox1 SINGLE_LIST strucures are the same as in WinNT
XBSYSAPI EXPORTNUM(56) xboxkrnl::PSINGLE_LIST_ENTRY FASTCALL xboxkrnl::KRNL(InterlockedFlushSList)
(
	IN xboxkrnl::PSLIST_HEADER ListHead
)
{
	LOG_FUNC_ONE_ARG(ListHead);

	RETURN((xboxkrnl::PSINGLE_LIST_ENTRY)InterlockedFlushSList((::PSLIST_HEADER)ListHead));
}

// ******************************************************************
// * 0x0039 - InterlockedPopEntrySList()
// ******************************************************************
// Source:ReactOS
XBSYSAPI EXPORTNUM(57) xboxkrnl::PSLIST_ENTRY FASTCALL xboxkrnl::KRNL(InterlockedPopEntrySList)
(
	IN PSLIST_HEADER ListHead
)
{
	LOG_FUNC_ONE_ARG(ListHead);

	RETURN((PSLIST_ENTRY)InterlockedPopEntrySList((::PSLIST_HEADER)ListHead));
}

// ******************************************************************
// * 0x003A - InterlockedPushEntrySList()
// ******************************************************************
// Source:ReactOS
XBSYSAPI EXPORTNUM(58) xboxkrnl::PSLIST_ENTRY FASTCALL xboxkrnl::KRNL(InterlockedPushEntrySList)
(
	IN PSLIST_HEADER ListHead,
	IN PSLIST_ENTRY ListEntry
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(ListHead)
		LOG_FUNC_ARG(ListEntry)
		LOG_FUNC_END;

	RETURN((xboxkrnl::PSLIST_ENTRY)InterlockedPushEntrySList((::PSLIST_HEADER)ListHead, (::PSLIST_ENTRY)ListEntry));
}

// ******************************************************************
// * 0x00A0 - KfRaiseIrql()
// ******************************************************************
// Raises the hardware priority (irql)
// NewIrql = Irql to raise to
// RETURN VALUE previous irq level
XBSYSAPI EXPORTNUM(160) xboxkrnl::UCHAR FASTCALL xboxkrnl::KfRaiseIrql
(
    IN UCHAR NewIrql
)
{
	LOG_FUNC_ONE_ARG(NewIrql);

	UCHAR OldIrql;
	KPCR* Pcr = nullptr;

	// Fetch KPCR data structure
	__asm {
		push eax
		mov eax, fs:[0x14]
		mov Pcr, eax
		pop eax
	}

	if (NewIrql < Pcr->Irql)	{
		// TODO: Enable this after KeBugCheck is implemented
		//KeBugCheck(IRQL_NOT_GREATER_OR_EQUAL);
		// for (;;);

		CxbxKrnlCleanup("IRQL_NOT_GREATER_OR_EQUAL");
	}
	
	OldIrql = Pcr->Irql;
	Pcr->Irql = NewIrql;
	
	RETURN(OldIrql);
}

// ******************************************************************
// * 0x00A1 - KfLowerIrql()
// ******************************************************************
// Restores the irq level on the current processor
// ARGUMENTS NewIrql = Irql to lower to
XBSYSAPI EXPORTNUM(161) xboxkrnl::VOID FASTCALL xboxkrnl::KfLowerIrql
(
    IN UCHAR NewIrql
)
{
	LOG_FUNC_ONE_ARG(NewIrql);

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x00A2 - KiBugCheckData
// ******************************************************************
// Source:ReactOS
XBSYSAPI EXPORTNUM(162) xboxkrnl::ULONG_PTR xboxkrnl::KiBugCheckData[5] = { NULL, NULL, NULL, NULL, NULL };

// ******************************************************************
// * 0x00A3 - KiUnlockDispatcherDatabase()
// ******************************************************************
XBSYSAPI EXPORTNUM(163) xboxkrnl::VOID FASTCALL xboxkrnl::KiUnlockDispatcherDatabase
(
	IN KIRQL OldIrql
)
{
	LOG_FUNC_ONE_ARG(OldIrql);

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x00FC - PhyGetLinkState()
// ******************************************************************
XBSYSAPI EXPORTNUM(252) xboxkrnl::NTSTATUS NTAPI xboxkrnl::PhyGetLinkState
(
	IN ULONG	Mode
)
{
	LOG_FUNC_ONE_ARG(Mode);

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x00FD - PhyInitialize()
// ******************************************************************
XBSYSAPI EXPORTNUM(253) xboxkrnl::NTSTATUS NTAPI xboxkrnl::PhyInitialize
(
	IN ULONG	Parameter1,
	IN ULONG	Parameter2
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Parameter1)
		LOG_FUNC_ARG(Parameter2)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x0165 - IdexChannelObject
// ******************************************************************
// TODO : Determine size, structure & filling behind this
XBSYSAPI EXPORTNUM(357) xboxkrnl::BYTE xboxkrnl::IdexChannelObject[0x100] = { };

// ******************************************************************
// * 0x016F - UnknownAPI367()
// ******************************************************************
XBSYSAPI EXPORTNUM(367) xboxkrnl::NTSTATUS NTAPI xboxkrnl::UnknownAPI367
(
	// UNKNOWN ARGUMENTS
)
{
	LOG_FUNC();

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x0170 - UnknownAPI368()
// ******************************************************************
XBSYSAPI EXPORTNUM(368) xboxkrnl::NTSTATUS NTAPI xboxkrnl::UnknownAPI368
(
	// UNKNOWN ARGUMENTS
)
{
	LOG_FUNC();

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x0171 - UnknownAPI369()
// ******************************************************************
XBSYSAPI EXPORTNUM(369) xboxkrnl::NTSTATUS NTAPI xboxkrnl::UnknownAPI369
(
	// UNKNOWN ARGUMENTS
)
{
	LOG_FUNC();

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x0172 - XProfpControl()
// ******************************************************************
XBSYSAPI EXPORTNUM(370) xboxkrnl::NTSTATUS NTAPI xboxkrnl::XProfpControl // PROFILING
(
	// UNKNOWN ARGUMENTS
)
{
	LOG_FUNC();

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x0173 - XProfpGetData()
// ******************************************************************
XBSYSAPI EXPORTNUM(371) xboxkrnl::NTSTATUS NTAPI xboxkrnl::XProfpGetData // PROFILING 
(
	// UNKNOWN ARGUMENTS
)
{
	LOG_FUNC();

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x0174 - IrtClientInitFast()
// ******************************************************************
XBSYSAPI EXPORTNUM(372) xboxkrnl::NTSTATUS NTAPI xboxkrnl::IrtClientInitFast // PROFILING
(
	// UNKNOWN ARGUMENTS
)
{
	LOG_FUNC();

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x0175 - IrtSweep()
// ******************************************************************
XBSYSAPI EXPORTNUM(373) xboxkrnl::NTSTATUS NTAPI xboxkrnl::IrtSweep // PROFILING
(
	// UNKNOWN ARGUMENTS
)
{
	LOG_FUNC();

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}