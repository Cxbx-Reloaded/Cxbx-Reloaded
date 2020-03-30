// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *  This file is part of Cxbx-Reloaded.
// *
// *  Cxbx-Reloaded is free software; you can redistribute it
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

#include "util/std_extend.hpp"
#include "AddressRanges.h"

bool AddressRangeMatchesFlags(const int index, const unsigned int flags)
{
	return XboxAddressRanges[index].RangeFlags & flags;
}

bool IsOptionalAddressRange(const int index)
{
	return AddressRangeMatchesFlags(index, MAY_FAIL);
}

int AddressRangeGetSystemFlags(const int index)
{
	return XboxAddressRanges[index].RangeFlags & SYSTEM_ALL;
}

bool VerifyWow64()
{
	typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	BOOL bIsWow64 = FALSE;
	HMODULE hKernel32 = GetModuleHandle(TEXT("kernel32"));
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(hKernel32, "IsWow64Process");
	if (fnIsWow64Process != nullptr) {
		HANDLE hCurrentProcess = GetCurrentProcess();
		fnIsWow64Process(hCurrentProcess, &bIsWow64);
	}

	return (bIsWow64 != FALSE);
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
