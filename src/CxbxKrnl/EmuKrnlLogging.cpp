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

std::ostream& operator<<(std::ostream& os, const PULONG& value)
{
	os << "0x" << (void*)value;
	if (value)
		os << " (*value: " << (void*)*value << ")";

	return os;
}

#define RENDER_MEMBER(Member) << " ."#Member":" << value.Member

std::ostream& operator<<(std::ostream& os, const xboxkrnl::LARGE_INTEGER& value)
{
	return os 
		RENDER_MEMBER(QuadPart);
}

std::ostream& operator<<(std::ostream& os, const xboxkrnl::MM_STATISTICS& value)
{
	return os
		RENDER_MEMBER(Length)
		RENDER_MEMBER(TotalPhysicalPages)
		RENDER_MEMBER(AvailablePages)
		RENDER_MEMBER(VirtualMemoryBytesCommitted)
		RENDER_MEMBER(VirtualMemoryBytesCommitted)
		RENDER_MEMBER(VirtualMemoryBytesReserved)
		RENDER_MEMBER(CachePagesCommitted)
		RENDER_MEMBER(PoolPagesCommitted)
		RENDER_MEMBER(StackPagesCommitted)
		RENDER_MEMBER(ImagePagesCommitted);
}

std::ostream& operator<<(std::ostream& os, const xboxkrnl::OBJECT_ATTRIBUTES& value)
{
	return os
		RENDER_MEMBER(RootDirectory)
		RENDER_MEMBER(ObjectName)
		RENDER_MEMBER(Attributes);
}

std::ostream& operator<<(std::ostream& os, const xboxkrnl::STRING& value)
{
	return os
		RENDER_MEMBER(Length)
		RENDER_MEMBER(MaximumLength)
		RENDER_MEMBER(Buffer);
}

std::ostream& operator<<(std::ostream& os, const xboxkrnl::UNICODE_STRING& value)
{
	return os
		RENDER_MEMBER(Length)
		RENDER_MEMBER(MaximumLength)
		RENDER_MEMBER(Buffer);
}

#undef RENDER_MEMBER

// Pointers to all of the above :

#define RENDER_POINTER(TYPE)                                                     \
std::ostream& operator<<(std::ostream& os, const xboxkrnl::P##TYPE& value)       \
{                                                                                \
	os << "0x" << (void*)value;                                                  \
	if (value)                                                                   \
		os << " (->"#TYPE":\n" << *value << ")";                                 \
                                                                                 \
	return os;                                                                   \
}

RENDER_POINTER(LARGE_INTEGER)
RENDER_POINTER(MM_STATISTICS)
RENDER_POINTER(OBJECT_ATTRIBUTES)
RENDER_POINTER(STRING)
RENDER_POINTER(UNICODE_STRING)

#undef RENDER_POINTER

/*
std::ostream& operator<<(std::ostream& os, const xboxkrnl::PLARGE_INTEGER& value)
{
	os << "0x" << (void*)value;
	if (value)
		os << " (->LARGE_INTEGER:\n" << *value << ")";

	return os;
}

std::ostream& operator<<(std::ostream& os, const xboxkrnl::PMM_STATISTICS& value)
{
	os << "0x" << (void*)value;
	if (value)
		os << " (->MM_STATISTICS:\n" << *value << ")";

	return os;
}

std::ostream& operator<<(std::ostream& os, const xboxkrnl::POBJECT_ATTRIBUTES& value)
{
	os << "0x" << (void*)value;
	if (value)
		os << " (->OBJECT_ATTRIBUTES:\n" << *value << ")";

	return os;
}

std::ostream& operator<<(std::ostream& os, const xboxkrnl::PSTRING& value)
{
	os << "0x" << (void*)value;
	if (value)
		os << " (->STRING:\n" << *value << ")";


	return os;
}
*/