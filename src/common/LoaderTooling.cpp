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
// *   Common->LoaderTooling.cpp
// *
// *  This file is part of the Cxbx project.
// *
// *  Cxbx is free software; you can redistribute it
// *  and/or modify it under the terms of the GNU General Public
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
// *  (c) 2017-2019 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************

#include "AddressRanges.h"

bool AddressRangeMatchesFlags(const int index, const int flags)
{
	return XboxAddressRanges[index].RangeFlags & flags;
}

bool IsOptionalAddressRange(const int index)
{
	return AddressRangeMatchesFlags(index, MAY_FAIL);
}

LPTSTR GetLastErrorString()
{
	DWORD err = GetLastError();

	// Translate ErrorCode to String.
	LPTSTR Error = nullptr;
	if (::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		err,
		0,
		(LPTSTR)&Error,
		0,
		NULL) == 0) {
		// Failed in translating.
	}

	return Error;
}

void FreeLastErrorString(LPTSTR Error)
{
	if (Error) {
		::LocalFree(Error);
		Error = nullptr;
	}
}

