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


// ******************************************************************
// * 0x00A0 - KfRaiseIrql
// ******************************************************************
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

// TODO : Determine size, structure & filling behind this
XBSYSAPI EXPORTNUM(357) xboxkrnl::BYTE xboxkrnl::IdexChannelObject[0x100] = { };
