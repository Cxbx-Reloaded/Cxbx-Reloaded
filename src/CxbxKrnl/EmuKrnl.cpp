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
#include "EmuXTL.h"

// prevent name collisions
namespace NtDll
{
    #include "EmuNtDll.h"
};

// See also :
// https://github.com/reactos/reactos/blob/40a16a9cf1cdfca399e9154b42d32c30b63480f5/reactos/drivers/filesystems/udfs/Include/env_spec_w32.h
void InitializeListHead(xboxkrnl::PLIST_ENTRY pListHead)
{
	pListHead->Flink = pListHead->Blink = pListHead;
}

bool IsListEmpty(xboxkrnl::PLIST_ENTRY pListHead)
{
	return (pListHead->Flink == pListHead);
}

void InsertHeadList(xboxkrnl::PLIST_ENTRY pListHead, xboxkrnl::PLIST_ENTRY pEntry)
{
	xboxkrnl::PLIST_ENTRY _EX_ListHead = pListHead;
	xboxkrnl::PLIST_ENTRY _EX_Flink = _EX_ListHead->Flink;

	pEntry->Flink = _EX_Flink;
	pEntry->Blink = _EX_ListHead;
	_EX_Flink->Blink = pEntry;
	_EX_ListHead->Flink = pEntry;
}

void InsertTailList(xboxkrnl::PLIST_ENTRY pListHead, xboxkrnl::PLIST_ENTRY pEntry)
{
	xboxkrnl::PLIST_ENTRY _EX_ListHead = pListHead;
	xboxkrnl::PLIST_ENTRY _EX_Blink = _EX_ListHead->Blink;

	pEntry->Flink = _EX_ListHead;
	pEntry->Blink = _EX_Blink;
	_EX_Blink->Flink = pEntry;
	_EX_ListHead->Blink = pEntry;
}

//#define RemoveEntryList(e) do { PLIST_ENTRY f = (e)->Flink, b = (e)->Blink; f->Blink = b; b->Flink = f; (e)->Flink = (e)->Blink = NULL; } while (0)

void RemoveEntryList(xboxkrnl::PLIST_ENTRY pEntry)
{
	xboxkrnl::PLIST_ENTRY _EX_Flink = pEntry->Flink;
	xboxkrnl::PLIST_ENTRY _EX_Blink = pEntry->Blink;

	if (_EX_Flink != nullptr) {
		_EX_Blink->Flink = _EX_Flink;
	}

	if (_EX_Flink != nullptr) {
		_EX_Flink->Blink = _EX_Blink;
	}
}

xboxkrnl::PLIST_ENTRY RemoveHeadList(xboxkrnl::PLIST_ENTRY pListHead)
{
	xboxkrnl::PLIST_ENTRY Result = pListHead->Flink;

	RemoveEntryList(pListHead->Flink);
	return Result;
}

xboxkrnl::PLIST_ENTRY RemoveTailList(xboxkrnl::PLIST_ENTRY pListHead)
{
	xboxkrnl::PLIST_ENTRY Result = pListHead->Blink;

	RemoveEntryList(pListHead->Blink);
	return Result;
}

// ******************************************************************
// * Declaring this in a header causes errors with xboxkrnl
// * namespace, so we must declare it within any file that uses it
// ******************************************************************
xboxkrnl::KPCR* KeGetPcr();

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

	LONG res = InterlockedCompareExchange((NtDll::PLONG)Destination, (NtDll::LONG)Exchange, (NtDll::LONG)Comparand);

	RETURN(res);
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

	LONG res = InterlockedDecrement((NtDll::PLONG)Addend);

	RETURN(res);
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

	LONG res = InterlockedIncrement((NtDll::PLONG)Addend);

	RETURN(res);
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

	LONG res = InterlockedExchange((NtDll::PLONG)Destination, (NtDll::LONG)Value);

	RETURN(res);
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

	LONG res = InterlockedExchangeAdd((NtDll::PLONG)Addend, (NtDll::LONG)Value);

	RETURN(res);
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

	PSINGLE_LIST_ENTRY res = (PSINGLE_LIST_ENTRY)InterlockedFlushSList((::PSLIST_HEADER)ListHead);

	RETURN(res);
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

	PSLIST_ENTRY res = (PSLIST_ENTRY)InterlockedPopEntrySList((::PSLIST_HEADER)ListHead);

	RETURN(res);
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

	PSLIST_ENTRY res = (PSLIST_ENTRY)InterlockedPushEntrySList((::PSLIST_HEADER)ListHead, (::PSLIST_ENTRY)ListEntry);

	RETURN(res);
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

	KPCR* Pcr = KeGetPcr();

	if (NewIrql < Pcr->Irql)	{
		KeBugCheck(0x00000009); // IRQL_NOT_GREATER_OR_EQUAL
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

	KPCR* Pcr = KeGetPcr();

	if (NewIrql > Pcr->Irql) {
		KeBugCheck(0x0000000A); // IRQL_NOT_LESS_OR_EQUAL
	}

	Pcr->Irql = NewIrql;

	// TODO: Dispatch pending interrupts
	LOG_INCOMPLETE();
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
XBSYSAPI EXPORTNUM(252) xboxkrnl::DWORD NTAPI xboxkrnl::PhyGetLinkState
(
	IN ULONG	Mode
)
{
	LOG_FUNC_ONE_ARG(Mode);

	return XNET_ETHERNET_LINK_ACTIVE | XNET_ETHERNET_LINK_100MBPS | XNET_ETHERNET_LINK_FULL_DUPLEX;
}

// ******************************************************************
// * 0x00FD - PhyInitialize()
// ******************************************************************
XBSYSAPI EXPORTNUM(253) xboxkrnl::NTSTATUS NTAPI xboxkrnl::PhyInitialize
(
	IN ULONG	forceReset,
	IN PVOID	Parameter2
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(forceReset)
		LOG_FUNC_ARG(Parameter2)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x0165 - IdexChannelObject
// ******************************************************************
// TODO : Determine size, structure & filling behind IdexChannelObject
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
	ULONG Action,
	ULONG Param
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Action)
		LOG_FUNC_ARG(Param)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x0173 - XProfpGetData()
// ******************************************************************
XBSYSAPI EXPORTNUM(371) xboxkrnl::NTSTATUS NTAPI xboxkrnl::XProfpGetData // PROFILING 
(
	// NO ARGUMENTS
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