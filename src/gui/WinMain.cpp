// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
// *  (c) 2002-2006 Aaron Robinson <caustik@caustik.com>
// *
// *  All rights reserved
// *
// ******************************************************************

#include "WndMain.h"

#include "AddressRanges.h" // For VerifyWow64()
#include "VerifyAddressRanges.h" // For VerifyBaseAddr()
#include "core\kernel\init\CxbxKrnl.h"
#include "core\kernel\support\Emu.h"
#include "EmuShared.h"
#include "common\Settings.hpp"
#include <commctrl.h>

// Enable Visual Styles
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name = 'Microsoft.Windows.Common-Controls' version = '6.0.0.0' \
processorArchitecture = '*' publicKeyToken = '6595b64144ccf1df' language = '*'\"")

/*! program entry point */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	hActiveModule = hInstance; // == GetModuleHandle(NULL); // Points to GUI (Cxbx.exe) ImageBase

	// First detect if we are running on WoW64, if not, prevent Cxbx-Reloaded from starting
	// Cxbx-Relaoded needs access to high memory, only exposed to WoW64.
	if (!VerifyWow64()) {
		CxbxShowError("Cxbx-Reloaded can only run under WoW64\nThis means either a 64-bit version of Windows or Wine with a 64-bit prefix");
		return EXIT_FAILURE;
	}

	/*! verify Cxbx.exe is loaded to base address 0x00010000 */
	if (!VerifyBaseAddr()) {
		CxbxShowError("Cxbx.exe is not loaded to base address 0x00010000 (which is a requirement for Xbox emulation)");
		return EXIT_FAILURE;
	}

	DWORD guiProcessID = 0;
	// TODO: Convert ALL __argc & __argv to use main(int argc, char** argv) method.
	bool bHasLoadArgument = CheckLoadArgument(__argc, __argv, &guiProcessID);

	/*! initialize shared memory */
	if (!EmuShared::Init(guiProcessID)) {
		CxbxShowError("Could not map shared memory!");
		return EXIT_FAILURE;
	}

	if (!HandleFirstLaunch()) {
		EmuShared::Cleanup();
		return EXIT_FAILURE;
	}

	if (bHasLoadArgument) {
		CxbxKrnlMain(__argc, __argv);
		EmuShared::Cleanup();
		return EXIT_SUCCESS;
	}

	// If 2nd GUI executable is launched, load settings file for GUI for editable support.
	if (g_Settings == nullptr) {
		if (!CreateSettings()) {
			EmuShared::Cleanup();
			return EXIT_FAILURE;
		}
	}

	// Possible optional output for GUI
	log_generate_active_filter_output(CXBXR_MODULE::INIT);

	INITCOMMONCONTROLSEX icc;
	icc.dwSize = sizeof(icc);
	icc.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icc);

    WndMain *MainWindow = new WndMain(hInstance);

	// NOTE: CxbxInitFilePaths must be initalize AFTER WndMain for data directory option from user.
	/* Initialize Cxbx File Paths */
	CxbxInitFilePaths();

    /*! wait for window to be created, or failure */
    while(!MainWindow->isCreated() && MainWindow->ProcessMessages())
    {
        Sleep(20);
    }

    /*! optionally open xbe and start emulation, if command line parameter was specified */
    if(__argc > 1 && false == MainWindow->HasError())
    {
        MainWindow->OpenXbe(__argv[1]);

        MainWindow->StartEmulation(MainWindow->GetHwnd());
    }

    /*! wait for window to be closed, or failure */
    while(!MainWindow->HasError() && MainWindow->ProcessMessages())
    {
        Sleep(10);
    }

    /*! if an error occurred, notify user */
    if(MainWindow->HasError()) {
		CxbxShowError(MainWindow->GetError().c_str());
    }

    delete MainWindow;

    /*! cleanup shared memory */
    EmuShared::Cleanup();

    return EXIT_SUCCESS;
}
