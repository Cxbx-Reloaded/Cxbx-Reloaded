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
	using void_t = void;
	using char_t = char;
	using cchar_t = char;
	using short_t = std::int16_t;
	using cshort_t = std::int16_t;
	using long_t = std::int32_t;
	using uchar_t = std::uint8_t;
	using byte_t = std::uint8_t;
	using boolean_t = std::uint8_t;
	using ushort_t = std::uint16_t;
	using word_t = std::uint16_t;
	typedef unsigned long       ULONG;
	using dword_t = std::uint32_t;
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
	using hresult_t = long_t;
	typedef float               FLOAT;
	// TODO: Remove __stdcall once lib86cpu is implemented.
	#define XBOXAPI             __stdcall
	#define XCALLBACK           XBOXAPI


	// ******************************************************************
	// * Pointer types
	// ******************************************************************
	typedef char_t *PCHAR;
	typedef char_t *PSZ;
	typedef char_t *PCSZ;
	typedef byte_t *PBYTE;
	typedef boolean_t *PBOOLEAN;
	typedef uchar_t *PUCHAR;
	typedef ushort_t *PUSHORT;
	typedef unsigned int *PUINT;
	typedef ULONG *PULONG;
	typedef dword_t *PDWORD, *LPDWORD;
	typedef ACCESS_MASK *PACCESS_MASK;
	typedef long_t *PLONG;
	typedef int *PINT_PTR;
	typedef void_t *PVOID, *LPVOID;
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
	typedef char_t *PCHAR, *LPCH, *PCH;
	typedef const char_t *LPCCH, *PCCH;
	typedef WCHAR *LPWSTR, *PWSTR;

	typedef /*_Null_terminated_*/ const WCHAR *LPCWSTR, *PCWSTR;

	// ******************************************************************
	// Misc
	// ******************************************************************
	typedef struct _XD3DVECTOR {
		FLOAT x;
		FLOAT y;
		FLOAT z;
	} D3DVECTOR;

	template<typename A, typename B>
	inline void CopyD3DVector(A& a, const B& b)
	{
		a.x = b.x;
		a.y = b.y;
		a.z = b.z;
	}
}
