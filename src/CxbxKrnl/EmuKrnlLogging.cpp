// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->EmuKrnlLogging.cpp
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
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

// prevent name collisions
namespace xboxkrnl
{
#include <xboxkrnl/xboxkrnl.h>
};


//#include <cstdio>
//#include <cctype>
//#include <clocale>
//
//#include <windows.h> // for DWORD
//#include <sstream> // for std::stringstream
//#include <iostream> // For std::cout
//#include <iomanip> // For std::setw

#include "Logging.h"
#include "EmuKrnlLogging.h"

std::ostream& operator<<(std::ostream& os, const xboxkrnl::LARGE_INTEGER& value)
{
	return os << value.QuadPart;
}

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
