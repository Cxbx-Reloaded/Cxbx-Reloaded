// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Logging.cpp
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
// *  (c) 2016 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************

#include <Windows.h>

#include "Logging.h"
//#include "EmuNtDll.h"

// For thread_local, see : http://en.cppreference.com/w/cpp/language/storage_duration
thread_local const DWORD _CurrentThreadId = GetCurrentThreadId();

// TODO : Use Boost.Format http://www.boost.org/doc/libs/1_53_0/libs/format/index.html
thread_local std::string _logPrefix;

std::ostream& operator<<(std::ostream& os, const LARGE_INTEGER& value);
{
	return os << value.QuadPart;
}

//std::ostream& operator<<(std::ostream& os, const LPCSTR& value);

std::ostream& operator<<(std::ostream& os, const PULONG& value)
{
	os << "0x" << (void*)value;
	if (value)
		os << " (->0x" << (void*)*value << ");

	return os;
}

// std::ostream& operator<<(std::ostream& os, const PMM_STATISTICS& value); // ->Length
// std::ostream& operator<<(std::ostream& os, const POBJECT_ATTRIBUTES& value); // ->ObjectName->Buffer
// std::ostream& operator<<(std::ostream& os, const PIO_STATUS_BLOCK& value); // ->u1.Pointer, ->Information
// std::ostream& operator<<(std::ostream& os, const PSTRING& value); // (value != 0) ? value->Buffer : ""

std::ostream& operator<<(std::ostream& os, const PLARGE_INTEGER& value)
{
	os << "0x" << (void*)value;
	if (value)
		os << " (->0x" << value->QuadPart << ");

	return os;
}

// std::ostream& operator<<(std::ostream& os, const PUNICODE_STRING& value);
// std::ostream& operator<<(std::ostream& os, const PVOID*& value); // * value, *value
// std::ostream& operator<<(std::ostream& os, const PXDEVICE_PREALLOC_TYPE& value);
// std::ostream& operator<<(std::ostream& os, const PXINPUT_CAPABILITIES& value);
// std::ostream& operator<<(std::ostream& os, const PXINPUT_STATE& value);
// std::ostream& operator<<(std::ostream& os, const PXPP_DEVICE_TYPE& value);
// std::ostream& operator<<(std::ostream& os, const PXTHREAD_NOTIFICATION& value); // -> pfnNotifyRoutine
// std::ostream& operator<<(std::ostream& os, const UCHAR& value);
