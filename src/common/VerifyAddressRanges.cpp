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
// *  (c) 2019 ego720
// *
// *  All rights reserved
// *
// ******************************************************************

#include "AddressRanges.h"
#include "core\kernel\init\CxbxKrnl.h" // For CXBX_BASE_ADDR

bool VerifyBaseAddr()
{
	/*! CXBX_BASE_ADDR is defined as 0x00010000, which is the base address of
	the cxbxr-ldr.exe host executable.
	Set in cxbxr-ldr.exe Project options, Linker, Advanced, Base Address */
	return ((UINT_PTR)GetModuleHandle(nullptr) == CXBX_BASE_ADDR);
}

void UnreserveMemoryRange(const int index)
{
	uint32_t Start = XboxAddressRanges[index].Start;
	int Size = XboxAddressRanges[index].Size;

	while (Size > 0) {
		VirtualFree((LPVOID)Start, (SIZE_T)0, MEM_RELEASE); // To release, dwSize must be zero!
		Start += BLOCK_SIZE;
		Size -= BLOCK_SIZE;
	}
}

bool AllocateMemoryRange(const int index)
{
	uint32_t Start = XboxAddressRanges[index].Start;
	int Size = XboxAddressRanges[index].Size;

	while (Size > 0) {
		int BlockSize = (Size > BLOCK_SIZE) ? BLOCK_SIZE : Size;
		if (nullptr == VirtualAlloc((void*)Start, BlockSize, MEM_COMMIT, PAGE_READWRITE)) { // MEM_RESERVE already done by Cxbx-Loader.exe
			return false;
		}

		Start += BLOCK_SIZE;
		Size -= BLOCK_SIZE;
	}

	return true;
}
