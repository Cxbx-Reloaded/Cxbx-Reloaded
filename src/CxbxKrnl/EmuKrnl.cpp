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

#include "CxbxKrnl.h"

// prevent name collisions
namespace NtDll
{
    #include "EmuNtDll.h"
};

// TODO : Move operator<< to a central place
std::ostream& operator<<(std::ostream& os, const xboxkrnl::PSTRING& value); // forward

//std::ostream& operator<<(std::ostream& os, const xboxkrnl::LARGE_INTEGER& value)
//{
//	return os << value.QuadPart;
//}

//std::ostream& operator<<(std::ostream& os, const xboxkrnl::LPCSTR& value);

std::ostream& operator<<(std::ostream& os, const PULONG& value)
{
	os << "0x" << (void*)value;
	if (value)
		os << " (*value: " << (void*)*value << ")";

	return os;
}

std::ostream& operator<<(std::ostream& os, const xboxkrnl::PMM_STATISTICS& value)
{
	os << "0x" << (void*)value;
	if (value)
		os << " (->Length: " << value->Length << ")";

	return os;
}

std::ostream& operator<<(std::ostream& os, const xboxkrnl::POBJECT_ATTRIBUTES& value)
{
	os << "0x" << (void*)value;
	if (value)
		os << " (->ObjectName: " << value->ObjectName << ")";

	return os;
}

// std::ostream& operator<<(std::ostream& os, const xboxkrnl::PIO_STATUS_BLOCK& value); // ->u1.Pointer, ->Information

std::ostream& operator<<(std::ostream& os, const xboxkrnl::PSTRING& value)
{
	os << "0x" << (void*)value;
	if (value)
		os << " (->Buffer: \"" << value->Buffer << "\")";

	return os;
}

std::ostream& operator<<(std::ostream& os, const xboxkrnl::PLARGE_INTEGER& value)
{
	os << "0x" << (void*)value;
	if (value)
		os << " (->QuadPart: " << value->QuadPart << ")";

	return os;
}

// std::ostream& operator<<(std::ostream& os, const xboxkrnl::PUNICODE_STRING& value);
// std::ostream& operator<<(std::ostream& os, const PVOID*& value); // * value, *value
// std::ostream& operator<<(std::ostream& os, const xboxkrnl::PXDEVICE_PREALLOC_TYPE& value);
// std::ostream& operator<<(std::ostream& os, const xboxkrnl::PXINPUT_CAPABILITIES& value);
// std::ostream& operator<<(std::ostream& os, const xboxkrnl::PXINPUT_STATE& value);
// std::ostream& operator<<(std::ostream& os, const xboxkrnl::PXPP_DEVICE_TYPE& value);
// std::ostream& operator<<(std::ostream& os, const xboxkrnl::PXTHREAD_NOTIFICATION& value); // -> pfnNotifyRoutine
// std::ostream& operator<<(std::ostream& os, const xboxkrnl::UCHAR& value);


using namespace xboxkrnl;

/*
XBSYSAPI EXPORTNUM(51) __fastcall xboxkrnl::InterlockedCompareExchange(
{0 EAX}FASTCALL_FIX_ARGUMENT_TAKING_EAX: DWORD;
{2 EDX}Exchange: LONG;
{1 ECX}Destination: PLONG;
{3 stack}Comparand: LONG
): LONG; register; // fastcall simulation - See Translation guide
// Source:ReactOS  Branch:Dxbx  Translator:PatrickvL  Done:100
{
if MayLog(lfUnit) then
DbgPrintf('EmuKrnl : InterlockedCompareExchange' +
#13#10'(' +
#13#10'   Exchange           : 0x%.08X' +
#13#10'   Destination        : 0x%.08X' +
#13#10'   Comparand          : 0x%.08X' +
#13#10');',
[Exchange, Destination, Comparand]);

  RETURN(InterlockedCompareExchange({var}Destination^, Exchange, Comparand);
};

XBSYSAPI EXPORTNUM(52) __fastcall xboxkrnl::InterlockedDecrement(
{0 EAX}FASTCALL_FIX_ARGUMENT_TAKING_EAX: DWORD;
{2 EDX}FASTCALL_FIX_ARGUMENT_TAKING_EDX: DWORD;
{1 ECX}Addend: PLONG // OUT, volatile
): LONG; register; // fastcall simulation - See Translation guide
// Source:ReactOS  Branch:Dxbx  Translator:PatrickvL  Done:100
{
if MayLog(lfUnit) then
DbgPrintf('EmuKrnl : InterlockedDecrement' +
#13#10'(' +
#13#10'   Addend             : 0x%.08X' +
#13#10');',
[Addend]);

  RETURN(InterlockedDecrement({var}Addend^);
};

XBSYSAPI EXPORTNUM(53) __fastcall xboxkrnl::InterlockedIncrement(
{0 EAX}FASTCALL_FIX_ARGUMENT_TAKING_EAX: DWORD;
{2 EDX}FASTCALL_FIX_ARGUMENT_TAKING_EDX: DWORD;
{1 ECX}Addend: PLONG // OUT, volatile
): LONG; register; // fastcall simulation - See Translation guide
// Source:ReactOS  Branch:Dxbx  Translator:PatrickvL  Done:100
{
if MayLog(lfUnit) then
DbgPrintf('EmuKrnl : InterlockedIncrement' +
#13#10'(' +
#13#10'   Addend             : 0x%.08X' +
#13#10');',
[Addend]);

  RETURN(InterlockedIncrement({var}Addend^);
};

XBSYSAPI EXPORTNUM(54) __fastcall xboxkrnl::InterlockedExchange(
{0 EAX}FASTCALL_FIX_ARGUMENT_TAKING_EAX: DWORD;
{2 EDX}Value: LONG;
{1 ECX}Destination: PLONG
): LONG; register; // fastcall simulation - See Translation guide
// Source:ReactOS  Branch:Dxbx  Translator:PatrickvL  Done:100
{
if MayLog(lfUnit) then
DbgPrintf('EmuKrnl : InterlockedExchange' +
#13#10'(' +
#13#10'   Value              : 0x%.08X' +
#13#10'   Destination        : 0x%.08X' +
#13#10');',
[Value, Destination]);

  RETURN(InterlockedExchange({var}Destination^, Value);
};

XBSYSAPI EXPORTNUM(55) __fastcall xboxkrnl::InterlockedExchangeAdd(
{0 EAX}FASTCALL_FIX_ARGUMENT_TAKING_EAX: DWORD;
{2 EDX}Value: LONG;
{1 ECX}Addend: PLONG
): LONG; register; // fastcall simulation - See Translation guide
// Source:ReactOS  Branch:Dxbx  Translator:PatrickvL  Done:100
{
if MayLog(lfUnit) then
DbgPrintf('EmuKrnl : InterlockedExchangeAdd' +
#13#10'(' +
#13#10'   Value              : 0x%.08X' +
#13#10'   Addend             : 0x%.08X' +
#13#10');',
[Value, Addend]);

  RETURN(InterlockedExchangeAdd({var}Addend^, Value);
};

// Dxbx Note : The Xbox1 SINGLE_LIST strucures are the same as in WinNT
XBSYSAPI EXPORTNUM(56) __fastcall xboxkrnl::InterlockedFlushSList(
{0 EAX}FASTCALL_FIX_ARGUMENT_TAKING_EAX: DWORD;
{2 EDX}FASTCALL_FIX_ARGUMENT_TAKING_EDX: DWORD;
{1 ECX}ListHead: PSLIST_HEADER
): PSINGLE_LIST_ENTRY; register; // fastcall simulation - See Translation guide
// Source:ReactOS  Branch:Dxbx  Translator:PatrickvL  Done:100
{
if MayLog(lfUnit) then
DbgPrintf('EmuKrnl : InterlockedFlushSList' +
#13#10'(' +
#13#10'   ListHead           : 0x%.08X' +
#13#10');',
[ListHead]);

  RETURN(InterlockedFlushSList(ListHead);
};

XBSYSAPI EXPORTNUM(57) __fastcall xboxkrnl::InterlockedPopEntrySList(
{0 EAX}FASTCALL_FIX_ARGUMENT_TAKING_EAX: DWORD;
{2 EDX}FASTCALL_FIX_ARGUMENT_TAKING_EDX: DWORD;
{1 ECX}ListHead: PSLIST_HEADER
): PSLIST_ENTRY; register; // fastcall simulation - See Translation guide
// Source:ReactOS  Branch:Dxbx  Translator:PatrickvL  Done:100
{
if MayLog(lfUnit) then
DbgPrintf('EmuKrnl : InterlockedPopEntrySList' +
#13#10'(' +
#13#10'   ListHead           : 0x%.08X' +
#13#10');',
[ListHead]);

  RETURN(InterlockedPopEntrySList(ListHead);
};

XBSYSAPI EXPORTNUM(58) __fastcall xboxkrnl::InterlockedPushEntrySList(
{0 EAX}FASTCALL_FIX_ARGUMENT_TAKING_EAX: DWORD;
{2 EDX}ListEntry: PSLIST_ENTRY;
{1 ECX}ListHead: PSLIST_HEADER
): PSLIST_ENTRY; register; // fastcall simulation - See Translation guide
// Source:ReactOS  Branch:Dxbx  Translator:PatrickvL  Done:100
{
if MayLog(lfUnit) then
DbgPrintf('EmuKrnl : InterlockedPopEntrySList' +
#13#10'(' +
#13#10'   ListEntry          : 0x%.08X' +
#13#10'   ListHead           : 0x%.08X' +
#13#10');',
[ListEntry, ListHead]);

  RETURN(InterlockedPushEntrySList(ListHead, ListEntry);
};
*/

// ******************************************************************
// * 0x00A0 - KfRaiseIrql
// ******************************************************************
// Raises the hardware priority (irql)
// NewIrql = Irql to raise to
// RETURN VALUE previous irq level
XBSYSAPI EXPORTNUM(160) xboxkrnl::UCHAR _fastcall xboxkrnl::KfRaiseIrql
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
// * 0x00A1 - KfLowerIrql
// ******************************************************************
// Restores the irq level on the current processor
// ARGUMENTS NewIrql = Irql to lower to
XBSYSAPI EXPORTNUM(161) VOID _fastcall xboxkrnl::KfLowerIrql
(
    IN UCHAR NewIrql
)
{
	LOG_FUNC_ONE_ARG(NewIrql);

	LOG_UNIMPLEMENTED();
}

// Source:ReactOS
XBSYSAPI EXPORTNUM(162) xboxkrnl::ULONG_PTR xboxkrnl::KiBugCheckData[5] = { NULL, NULL, NULL, NULL, NULL };

/*
XBSYSAPI EXPORTNUM(163) xboxkrnl::KiUnlockDispatcherDatabase(): NTSTATUS; stdcall;
// Branch:Dxbx  Translator:PatrickvL  Done:0
{
Unimplemented('KiUnlockDispatcherDatabase');
  RETURN(S_OK;
};

XBSYSAPI EXPORTNUM(252) xboxkrnl::PhyGetLinkState(): NTSTATUS; stdcall;
// Branch:Dxbx  Translator:PatrickvL  Done:0
{
Unimplemented('PhyGetLinkState');
  RETURN(S_OK;
};

XBSYSAPI EXPORTNUM(253) xboxkrnl::PhyInitialize(): NTSTATUS; stdcall;
// Branch:Dxbx  Translator:PatrickvL  Done:0
{
Unimplemented('PhyInitialize');
  RETURN(S_OK;
};
*/

// TODO : Determine size, structure & filling behind this
XBSYSAPI EXPORTNUM(357) xboxkrnl::BYTE xboxkrnl::IdexChannelObject[0x100] = { };

/*
XBSYSAPI EXPORTNUM(367) xboxkrnl::UnknownAPI367(): NTSTATUS; stdcall;
// Branch:Dxbx  Translator:PatrickvL  Done:0
{
Unimplemented('UnknownAPI367');
  RETURN(S_OK;
};

XBSYSAPI EXPORTNUM(368) xboxkrnl::UnknownAPI368(): NTSTATUS; stdcall;
// Branch:Dxbx  Translator:PatrickvL  Done:0
{
Unimplemented('UnknownAPI368');
  RETURN(S_OK;
};

XBSYSAPI EXPORTNUM(369) xboxkrnl::UnknownAPI369(): NTSTATUS; stdcall;
// Branch:Dxbx  Translator:PatrickvL  Done:0
{
Unimplemented('UnknownAPI369');
  RETURN(S_OK;
};

XBSYSAPI EXPORTNUM(370} PROFILING XProfpControl(): NTSTATUS; stdcall;
// Branch:Dxbx  Translator:PatrickvL  Done:0
{
Unimplemented('UnknownAPI370');
  RETURN(S_OK;
};

XBSYSAPI EXPORTNUM(371} PROFILING XProfpGetData(): NTSTATUS; stdcall;
// Branch:Dxbx  Translator:PatrickvL  Done:0
{
Unimplemented('UnknownAPI371');
  RETURN(S_OK;
};

XBSYSAPI EXPORTNUM(372} PROFILING IrtClientInitFast(): NTSTATUS; stdcall;
// Branch:Dxbx  Translator:PatrickvL  Done:0
{
Unimplemented('UnknownAPI372');
  RETURN(S_OK;
};

XBSYSAPI EXPORTNUM(373} PROFILING IrtSweep(): NTSTATUS; stdcall;
// Branch:Dxbx  Translator:PatrickvL  Done:0
{
Unimplemented('UnknownAPI373');
  RETURN(S_OK;
};
*/