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
// *
// *  All rights reserved
// *
// ******************************************************************
#define _XBOXKRNL_DEFEXTRN_

#define LOG_PREFIX CXBXR_MODULE::KRNL

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
#include "EmuKrnl.h" // for HalSystemInterrupts
#include "EmuKrnlKi.h" // for KiLockDispatcherDatabase
#include "core\kernel\init\CxbxKrnl.h"
#include "core\kernel\support\EmuXTL.h"

// prevent name collisions
namespace NtDll
{
    #include "core\kernel\support\EmuNtDll.h"
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

// Returns TRUE if the list has become empty after removing the element, FALSE otherwise.
xboxkrnl::BOOLEAN RemoveEntryList(xboxkrnl::PLIST_ENTRY pEntry)
{
	xboxkrnl::PLIST_ENTRY _EX_Flink = pEntry->Flink;
	xboxkrnl::PLIST_ENTRY _EX_Blink = pEntry->Blink;

	if (_EX_Blink != nullptr) {
		_EX_Blink->Flink = _EX_Flink;
	}

	if (_EX_Flink != nullptr) {
		_EX_Flink->Blink = _EX_Blink;
	}

	if (_EX_Blink != nullptr && _EX_Flink != nullptr) {
		return (_EX_Flink == _EX_Blink);
	}
	// If we reach here then it means we have erroneously been called on a detached element. In this case,
	// always report FALSE to avoid possible side effects
	return FALSE;
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
xboxkrnl::KPCR* WINAPI KeGetPcr();

// Interrupts

extern volatile DWORD HalInterruptRequestRegister;

volatile bool g_bInterruptsEnabled = true;

bool DisableInterrupts()
{
	bool Result = g_bInterruptsEnabled;
	g_bInterruptsEnabled = false;
	return Result;
}

void RestoreInterruptMode(bool value)
{
	g_bInterruptsEnabled = value;
}

extern void ExecuteDpcQueue();

void KiUnexpectedInterrupt()
{
	xboxkrnl::KeBugCheck(TRAP_CAUSE_UNKNOWN); // see
	CxbxKrnlCleanup("Unexpected Software Interrupt!");
}

void CallSoftwareInterrupt(const xboxkrnl::KIRQL SoftwareIrql)
{
	switch (SoftwareIrql) {
	case PASSIVE_LEVEL:
		KiUnexpectedInterrupt();
		break;
	case APC_LEVEL: // = 1 // HalpApcInterrupt        
		EmuLog(LOG_LEVEL::WARNING, "Unimplemented Software Interrupt (APC)"); // TODO : ExecuteApcQueue();
		break;
	case DISPATCH_LEVEL: // = 2
		ExecuteDpcQueue();
		break;
	case APC_LEVEL | DISPATCH_LEVEL: // = 3
		KiUnexpectedInterrupt();
		break;
	default:
		// Software Interrupts > 3 map to Hardware Interrupts [4 = IRQ0]
		// This is used to trigger hardware interrupt routines from software
		if (EmuInterruptList[SoftwareIrql - 4]->Connected) {
			HalSystemInterrupts[SoftwareIrql - 4].Trigger(EmuInterruptList[SoftwareIrql - 4]);
		}
		break;
	}
}

const DWORD IrqlMasks[] = {
	0xFFFFFFFE, // IRQL 0
	0xFFFFFFFC, // IRQL 1 (APC_LEVEL)
	0xFFFFFFF8, // IRQL 2 (DISPATCH_LEVEL)
	0xFFFFFFF0, // IRQL 3
	0x03FFFFF0, // IRQL 4
	0x01FFFFF0, // IRQL 5
	0x00FFFFF0, // IRQL 6
	0x00EFFFF0, // IRQL 7
	0x007FFFF0, // IRQL 8
	0x003FFFF0, // IRQL 9
	0x001FFFF0, // IRQL 10
	0x000EFFF0, // IRQL 11
	0x0007FFF0, // IRQL 12
	0x0003FFF0, // IRQL 13
	0x0001FFF0, // IRQL 14
	0x00017FF0, // IRQL 15
	0x00013FF0, // IRQL 16
	0x00011FF0, // IRQL 17
	0x00011FF0, // IRQL 18
	0x000117F0, // IRQL 19
	0x000113F0, // IRQL 20
	0x000111F0, // IRQL 21
	0x000110F0, // IRQL 22
	0x00011070, // IRQL 23
	0x00011030, // IRQL 24
	0x00011010, // IRQL 25
	0x00010010, // IRQL 26 (PROFILE_LEVEL)
	0x00000010, // IRQL 27
	0x00000000, // IRQL 28 (SYNC_LEVEL)
	0x00000000, // IRQL 29
	0x00000000, // IRQL 30
	0x00000000, // IRQL 31 (HIGH_LEVEL)
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
// Raises the hardware priority (irq level)
// NewIrql = Irq level to raise to
// RETURN VALUE previous irq level
XBSYSAPI EXPORTNUM(160) xboxkrnl::KIRQL FASTCALL xboxkrnl::KfRaiseIrql
(
    IN KIRQL NewIrql
)
{
	LOG_FUNC_ONE_ARG_TYPE(KIRQL_TYPE, NewIrql);

	// Inlined KeGetCurrentIrql() :
	PKPCR Pcr = KeGetPcr();
	KIRQL OldIrql = (KIRQL)Pcr->Irql;

	// Set new before check
	Pcr->Irql = NewIrql;

	if (NewIrql < OldIrql)	{
		Pcr->Irql = 0; // Probably to avoid recursion?
		KeBugCheckEx(IRQL_NOT_GREATER_OR_EQUAL, (PVOID)OldIrql, (PVOID)NewIrql, 0, 0);
	}

	RETURN_TYPE(KIRQL_TYPE, OldIrql);
}

inline int bsr(const uint32_t a) { DWORD result; _BitScanReverse(&result, a); return result; }

// ******************************************************************
// * 0x00A1 - KfLowerIrql()
// ******************************************************************
// Restores the irq level on the current processor
// ARGUMENTS NewIrql = Irql to lower to
XBSYSAPI EXPORTNUM(161) xboxkrnl::VOID FASTCALL xboxkrnl::KfLowerIrql
(
    IN KIRQL NewIrql
)
{
	LOG_FUNC_ONE_ARG_TYPE(KIRQL_TYPE, NewIrql);

	KPCR* Pcr = KeGetPcr();

	if (g_bIsDebugKernel && NewIrql > Pcr->Irql) {
		KIRQL OldIrql = Pcr->Irql;
		Pcr->Irql = HIGH_LEVEL; // Probably to avoid recursion?
		KeBugCheckEx(IRQL_NOT_LESS_OR_EQUAL, (PVOID)OldIrql, (PVOID)NewIrql, 0, 0);
		return;
	}

	bool interrupt_flag = DisableInterrupts();
	Pcr->Irql = NewIrql;

	DWORD MaskedInterrupts = HalInterruptRequestRegister & IrqlMasks[NewIrql];

	// Are there any interrupts pending?
	if (MaskedInterrupts > 0) {
		// Determine the highest IRQ level 
		KIRQL HighestIrql = (KIRQL)bsr(MaskedInterrupts);

		// Above dispatch level, clear the highest interrupt request bit
		if (HighestIrql > DISPATCH_LEVEL) {
			HalInterruptRequestRegister ^= 1 << HighestIrql;
		}

		CallSoftwareInterrupt(HighestIrql);
	}

	RestoreInterruptMode(interrupt_flag);
}

// ******************************************************************
// * 0x00A2 - KiBugCheckData
// ******************************************************************
// Source:ReactOS
XBSYSAPI EXPORTNUM(162) xboxkrnl::ULONG_PTR xboxkrnl::KiBugCheckData[5] = { NULL, NULL, NULL, NULL, NULL };

extern xboxkrnl::KPRCB *KeGetCurrentPrcb();

// ******************************************************************
// * 0x00A3 - KiUnlockDispatcherDatabase()
// ******************************************************************
XBSYSAPI EXPORTNUM(163) xboxkrnl::VOID FASTCALL xboxkrnl::KiUnlockDispatcherDatabase
(
	IN KIRQL OldIrql
)
{
	LOG_FUNC_ONE_ARG_TYPE(KIRQL_TYPE, OldIrql);

	if (!(KeGetCurrentPrcb()->DpcRoutineActive)) // Avoid KeIsExecutingDpc(), as that logs
		HalRequestSoftwareInterrupt(DISPATCH_LEVEL);

	LOG_INCOMPLETE(); // TODO : Thread-switch?

	KfLowerIrql(OldIrql);
}

// ******************************************************************
// * 0x0165 - IdexChannelObject
// ******************************************************************
// TODO : Determine size, structure & filling behind IdexChannelObject
XBSYSAPI EXPORTNUM(357) xboxkrnl::BYTE xboxkrnl::IdexChannelObject[0x100] = { };

// ******************************************************************
// * 0x0169 - RtlSnprintf()
// ******************************************************************
XBSYSAPI EXPORTNUM(361) xboxkrnl::INT CDECL xboxkrnl::RtlSnprintf
(
	IN PCHAR string,
	IN SIZE_T count,
	IN LPCCH format,
	...
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(string)
		LOG_FUNC_ARG(count)
		LOG_FUNC_ARG(format)
		LOG_FUNC_END;

	// UNTESTED. Possible test-case : debugchannel.xbe

	va_list ap;
	va_start(ap, format);
	INT Result = snprintf(string, count, format, ap);
	va_end(ap);

	RETURN(Result);
}

// ******************************************************************
// * 0x016A - RtlSprintf()
// ******************************************************************
XBSYSAPI EXPORTNUM(362) xboxkrnl::INT CDECL xboxkrnl::RtlSprintf
(
	IN PCHAR string,
	IN LPCCH format,
	...
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(string)
		LOG_FUNC_ARG(format)
		LOG_FUNC_END;

	// UNTESTED. Possible test-case : debugchannel.xbe

	va_list ap;
	va_start(ap, format);
	INT Result = sprintf(string, format, ap);
	va_end(ap);

	RETURN(Result);
}

// ******************************************************************
// * 0x016B - RtlVsnprintf()
// ******************************************************************
XBSYSAPI EXPORTNUM(363) xboxkrnl::INT CDECL xboxkrnl::RtlVsnprintf
(
	IN PCHAR string,
	IN SIZE_T count,
	IN LPCCH format,
	...
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(string)
		LOG_FUNC_ARG(count)
		LOG_FUNC_ARG(format)
		LOG_FUNC_END;

	// UNTESTED. Possible test-case : debugchannel.xbe

	va_list ap;
	va_start(ap, format);
	INT Result = vsnprintf(string, count, format, ap);
	va_end(ap);

	RETURN(Result);
}

// ******************************************************************
// * 0x016C - RtlVsprintf()
// ******************************************************************
XBSYSAPI EXPORTNUM(364) xboxkrnl::INT CDECL xboxkrnl::RtlVsprintf
(
	IN PCHAR string,
	IN LPCCH format,
	...
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(string)
		LOG_FUNC_ARG(format)
		LOG_FUNC_END;

	// UNTESTED. Possible test-case : debugchannel.xbe

	va_list ap;
	va_start(ap, format);
	INT Result = vsprintf(string, format, ap);
	va_end(ap);

	RETURN(Result);
}

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
