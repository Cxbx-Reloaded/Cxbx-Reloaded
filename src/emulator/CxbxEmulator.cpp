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

// CxbxEmulator.cpp : Defines the exported functions for the DLL application.

#include "Cxbx.h" // For FUNC_EXPORTS
#include "VerifyAddressRanges.h" // For VerifyBaseAddr() and VerifyAddressRanges()
//#include "CxbxKrnl/Emu.h"
#include "EmuShared.h"
//#include <commctrl.h>

PCHAR*
CommandLineToArgvA(
	PCHAR CmdLine,
	int* _argc
)
{
	PCHAR* argv;
	PCHAR  _argv;
	ULONG   len;
	ULONG   argc;
	CHAR   a;
	ULONG   i, j;

	BOOLEAN  in_QM;
	BOOLEAN  in_TEXT;
	BOOLEAN  in_SPACE;

	len = strlen(CmdLine);
	i = ((len + 2) / 2) * sizeof(PVOID) + sizeof(PVOID);

	argv = (PCHAR*)LocalAlloc(GMEM_FIXED,
		i + (len + 2) * sizeof(CHAR));

	_argv = (PCHAR)(((PUCHAR)argv) + i);

	argc = 0;
	argv[argc] = _argv;
	in_QM = FALSE;
	in_TEXT = FALSE;
	in_SPACE = TRUE;
	i = 0;
	j = 0;

	while (a = CmdLine[i]) {
		if (in_QM) {
			if (a == '\"') {
				in_QM = FALSE;
			}
			else {
				_argv[j] = a;
				j++;
			}
		}
		else {
			switch (a) {
			case '\"':
				in_QM = TRUE;
				in_TEXT = TRUE;
				if (in_SPACE) {
					argv[argc] = _argv + j;
					argc++;
				}
				in_SPACE = FALSE;
				break;
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				if (in_TEXT) {
					_argv[j] = '\0';
					j++;
				}
				in_TEXT = FALSE;
				in_SPACE = TRUE;
				break;
			default:
				in_TEXT = TRUE;
				if (in_SPACE) {
					argv[argc] = _argv + j;
					argc++;
				}
				_argv[j] = a;
				j++;
				in_SPACE = FALSE;
				break;
			}
		}
		i++;
	}
	_argv[j] = '\0';
	argv[argc] = NULL;

	(*_argc) = argc;
	return argv;
}

DWORD WINAPI Emulate(int system)
{
	FUNC_EXPORTS

	/*! Verify our host executable, CxbxLoader.exe, is loaded to base address 0x00010000 */
	if (!VerifyBaseAddr()) {
		MessageBox(NULL, "CxbxLoader.exe was not loaded to base address 0x00010000 (which is a requirement for Xbox emulation)", "Cxbx-Reloaded", MB_OK);
		return EXIT_FAILURE;
	}

	// Before doing anything else that might cause memory fragmentation,
	// verify that we still got control over all ranges the loader reserved
	if (!VerifyAddressRanges(system)) {
		MessageBox(NULL, "Failed to claim required address ranges (which is a requirement for Xbox emulation)", "Cxbx-Reloaded", MB_OK);
		return EXIT_FAILURE;
	}

	/* Initialize Cxbx File Paths */
//	CxbxInitFilePaths();

	/*! initialize shared memory */
	if (!EmuShared::Init(0)) {
		MessageBox(NULL, "Could not map shared memory!", "Cxbx-Reloaded", MB_OK);
		return EXIT_FAILURE;
	}

	LPSTR CommandLine = GetCommandLine();
	int argc;
	PCHAR *argv = CommandLineToArgvA(CommandLine, &argc);

//	CxbxKrnlMain(argc, argv);

	LocalFree(argv);

	/*! cleanup shared memory */
	EmuShared::Cleanup();

	// Note : Emulate() must never return to it's caller (rawMain() in loader.cpp),
	// because that function resides in a block of memory that's overwritten with
	// xbox executable contents. Returning there would lead to undefined behaviour.
	// Since we're done emulating, it's al right to terminate here :
	TerminateProcess(GetCurrentProcess(), EXIT_SUCCESS);

	// This line will never be reached:
	return EXIT_FAILURE;
}
