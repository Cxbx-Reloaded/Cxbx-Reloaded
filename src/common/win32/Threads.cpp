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

#include <windows.h>
#include "Threads.h"

// Exception structure and method from:
// https://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx

const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)  
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.  
	LPCSTR szName; // Pointer to name (in user addr space).  
	DWORD dwThreadID; // Thread ID (-1=caller thread).  
	DWORD dwFlags; // Reserved for future use, must be zero.  
} THREADNAME_INFO;
#pragma pack(pop)  

#ifdef _MSC_VER
void SetThreadName(DWORD dwThreadID, const char* szThreadName)
{
	if (!IsDebuggerPresent())
		return;

	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = szThreadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;
	__try {
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
	}
#pragma warning(suppress : 6320) // silence /analyze: Exception-filter expression is the constant
	// EXCEPTION_EXECUTE_HANDLER. This might mask exceptions that were
	// not intended to be handled
#pragma warning(suppress : 6322) // silence /analyze: Empty _except block
	__except (EXCEPTION_EXECUTE_HANDLER) {
	}
}
#else
void SetThreadName(DWORD dwThreadID, const char* szThreadName)
{
	// TODO: Use SetThreadDescription
}
#endif

void SetCurrentThreadName(const char* szThreadName)
{
	SetThreadName(GetCurrentThreadId(), szThreadName);
}
