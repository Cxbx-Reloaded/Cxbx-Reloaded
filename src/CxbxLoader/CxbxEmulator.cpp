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

#include "stdafx.h"
#include "Cxbx.h"

DWORD WINAPI Emulate()
{
	FUNC_EXPORTS

		/*! verify Cxbx-Loader.exe is loaded to base address 0x00010000 */
		if ((UINT_PTR)GetModuleHandle(nullptr) != CXBX_BASE_ADDR)
		{
			/*! CXBX_BASE_ADDR is defined as 0x00010000, which is the base address of
			the Cxbx-Loader.exe host executable.
			Set in Cxbx-Loader.exe Project options, Linker, Advanced, Base Address */
			MessageBox(NULL, "Cxbx-Loader.exe was not loaded to base address 0x00010000 (which is a requirement for Xbox emulation)", "Cxbx-Reloaded", MB_OK);
			return 1;
		}

	/* Initialize Cxbx File Paths */
	CxbxInitFilePaths();

	/*! initialize shared memory */
	if (!EmuShared::Init()) {
		MessageBox(NULL, "Could not map shared memory!", "Cxbx-Reloaded", MB_OK);
		return 1;
	}

	LPSTR CommandLine = GetCommandLine();
	int argc;
	PCHAR *argv = CommandLineToArgvA(CommandLine, &argc);

	CxbxKrnlMain(argc, argv);

	LocalFree(argv);

	/*! cleanup shared memory */
	EmuShared::Cleanup();

	return 0;
}
