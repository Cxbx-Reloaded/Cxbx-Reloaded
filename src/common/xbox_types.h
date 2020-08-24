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
// *  (c) 2020 ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

#pragma once

#include <cstdint>
#include <cstddef>


namespace xbox
{
#ifndef VOID
	typedef void VOID;
#endif

	/*! addr is the type of a physical address */
	using addr = std::uint32_t;

	/*! zero is the type of null address or value */
	inline constexpr addr zero = 0;

	/*! zeroptr is the type of null pointer address */
	using zeroptr_t = std::nullptr_t;
	inline constexpr zeroptr_t zeroptr = nullptr;

	// ******************************************************************
	// * Basic types
	// ******************************************************************
	typedef char                CHAR, CCHAR;
	typedef short               SHORT, CSHORT;
	typedef long                LONG;
	typedef unsigned char       UCHAR;
	typedef unsigned char       BYTE;
	typedef unsigned char       BOOLEAN;
	typedef unsigned short      USHORT;
	typedef unsigned short      WORD;
	typedef unsigned long       ULONG;
	typedef unsigned long       DWORD;
	typedef unsigned long       SIZE_T, *PSIZE_T;
	typedef unsigned long       ACCESS_MASK, *PACCESS_MASK;
	typedef unsigned long       PHYSICAL_ADDRESS;
	typedef unsigned int        UINT;
	typedef int                 INT;
	typedef int                 INT_PTR;
	typedef signed __int64      LONGLONG;
	typedef unsigned __int64    ULONGLONG;
	typedef wchar_t             WCHAR;
	typedef unsigned __int64    QUAD; // 8 byte aligned 8 byte long
	typedef int                 BOOL;
	typedef LONG                HRESULT;


	// ******************************************************************
	// * Pointer types
	// ******************************************************************
	typedef CHAR *PCHAR;
	typedef char *PSZ;
	typedef CHAR *PCSZ;
	typedef BYTE *PBYTE;
	typedef BOOLEAN *PBOOLEAN;
	typedef UCHAR *PUCHAR;
	typedef USHORT *PUSHORT;
	typedef unsigned int *PUINT;
	typedef ULONG *PULONG;
	typedef DWORD *PDWORD, *LPDWORD;
	typedef ACCESS_MASK *PACCESS_MASK;
	typedef LONG *PLONG;
	typedef int *PINT_PTR;
	typedef VOID *PVOID, *LPVOID;
	typedef void *HANDLE;
	typedef HANDLE *PHANDLE;

	// Additional basic and pointer types :
	typedef long                LONG_PTR;
	typedef unsigned long       ULONG_PTR;

	typedef LONGLONG *PLONGLONG;
	typedef QUAD *PQUAD;

	// ******************************************************************
	// ANSI (Multi-byte Character) types
	// ******************************************************************
	typedef CHAR *PCHAR, *LPCH, *PCH;
	typedef const CHAR *LPCCH, *PCCH;
	typedef WCHAR *LPWSTR, *PWSTR;

	typedef /*_Null_terminated_*/ const WCHAR *LPCWSTR, *PCWSTR;
}
