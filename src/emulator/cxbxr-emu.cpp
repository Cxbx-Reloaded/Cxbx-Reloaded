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

// cxbxr-emu.cpp : Defines the exported functions for the DLL application.

#define LOG_PREFIX CXBXR_MODULE::CXBXR

#include "Cxbx.h" // For FUNC_EXPORTS
#include "VerifyAddressRanges.h" // For VerifyBaseAddr()
//#include "CxbxKrnl/Emu.h"
#include "EmuShared.h"
#include "core\kernel\init\CxbxKrnl.h" // For HandleFirstLaunch() and LaunchEmulation()
//#include <commctrl.h>
#include "common/util/cliConverter.hpp"
#include "common/util/cliConfig.hpp"

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

DWORD WINAPI Emulate(unsigned int reserved_systems, blocks_reserved_t blocks_reserved)
{
	FUNC_EXPORTS

	/*! Verify our host executable, cxbxr-ldr.exe, is loaded to base address 0x00010000 */
	if (!VerifyBaseAddr()) {
		PopupError(nullptr, "cxbx-ldr.exe was not loaded to base address 0x00010000 (which is a requirement for Xbox emulation)");
		return EXIT_FAILURE;
	}

	LPSTR CommandLine = GetCommandLine();
	if (!CommandLine) {
		PopupError(nullptr, "Couldn't retrieve command line!");
		return EXIT_FAILURE;
	}

	int argc = 0;
	PCHAR *argv = CommandLineToArgvA(CommandLine, &argc);
	if (!argv) {
		PopupError(nullptr, "Couldn't parse command line!");
		return EXIT_FAILURE;
	}

	if (!cli_config::GenConfig(argv, argc)) {
		PopupError(nullptr, "Couldn't convert parsed command line!");
		LocalFree(argv);
		return EXIT_FAILURE;
	}
	LocalFree(argv);

	/*! verify load argument is included */
	if (!cli_config::hasKey("load")) {
		PopupError(nullptr, "No /load argument in command line!");
		return EXIT_FAILURE;
	}

	/*! initialize shared memory */
	if (!EmuShared::Init(cli_config::GetSessionID())) {
		PopupError(nullptr, "Could not map shared memory!");
		return EXIT_FAILURE;
	}

	if (!HandleFirstLaunch()) {
		PopupError(nullptr, "First launch failed!");
		EmuShared::Cleanup();
		return EXIT_FAILURE;
	}

	if (!reserved_systems) {
		PopupError(nullptr, "Unable to preserve any system's memory ranges!");
		EmuShared::Cleanup();
		return EXIT_FAILURE;
	}

	CxbxKrnlEmulate(reserved_systems, blocks_reserved);

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
