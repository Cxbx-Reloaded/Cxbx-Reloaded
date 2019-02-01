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
// *   Cxbx-Loader->loader.cpp
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
// *  (c) 2017 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************

#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h> // For DWORD, CALLBACK, VirtualAlloc, LPVOID, SIZE_T, HMODULE 

#include "..\Common\XboxAddressRanges.h" // For XboxAddressRangeType, XboxAddressRanges

// Reserve the first 128 MB MemLowVirtual address range without inflating the EXE size,
// by simply declaring an array, first thing, in global scope. It gets placed in the BSS segment,
// so it still uses space in RAM, but not the actual executable.
// This variable *MUST* be this large, for it to take up address space
// so that all other code and data in this module are placed outside of the
// maximum virtual memory range.
#define VM_PLACEHOLDER_SIZE 128 * 1024 * 1024

// Note : In the old setup, we used #pragma section(".text"); __declspec(allocate(".text"))
// to put this variable at the exact image base address 0x00010000, but that resulted in
// an increase of the executable by 128 Mb. Since we won't return to the tiny loader code,
// and the executable doesn't contain any data that we refer too once we entered the emulation DLL,
// this will be all right. The only bit of data I transfer over, is on the stack, but the stack
// (of the initial thread) resides far outside of the reserved range, so that's all right too.
unsigned char virtual_memory_placeholder[VM_PLACEHOLDER_SIZE] = { 0 }; // = { OPCODE_NOP_90 };

// Note : This executable is meant to be as tiny as humanly possible.
// The C++ runtime library is removed using https://stackoverflow.com/a/39220245/12170

DWORD CALLBACK rawMain()
{
	(void)virtual_memory_placeholder; // prevent optimization removing this data

	if (!ReserveAddressRanges()) {
		// If we get here, emulation lacks important address ranges; Don't launch
		OutputDebugStringA("Required address range couldn't be reserved!");
		return ERROR_NOT_ENOUGH_MEMORY;
	}

	// Only after the required memory ranges are reserved, load our emulation DLL
	HMODULE hEmulationDLL = LoadLibraryA("CxbxEmulator.dll");
	if (!hEmulationDLL) {
		// TODO : Move the following towards a tooling function
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

		// Free the buffer.
		if (Error) {
			OutputDebugString(Error);
			::LocalFree(Error);
			Error = nullptr;
		}

		return ERROR_RESOURCE_NOT_FOUND;
	}

	// Find the main emulation function in our DLL
	typedef DWORD (WINAPI * Emulate_t)();
	Emulate_t pfnEmulate = (Emulate_t)GetProcAddress(hEmulationDLL, "Emulate");
	if (!pfnEmulate) {
		return ERROR_RESOURCE_NOT_FOUND;
	}

	// Call the main emulation function in our DLL, passing in the results
	// of the address range reservations
	return pfnEmulate(); // TODO : Pass along the command line
}
