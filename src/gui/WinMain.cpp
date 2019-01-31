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
	typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	BOOL bIsWow64 = FALSE;
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
	if (fnIsWow64Process != nullptr) {
		fnIsWow64Process(GetCurrentProcess(), &bIsWow64);
	}
	
	if (bIsWow64 == FALSE) {
		MessageBox(NULL, "Cxbx-Reloaded can only run under WoW64\nThis means either a 64-bit version of Windows or Wine with a 64-bit prefix", "Cxbx-Reloaded",
			MB_OK | MB_ICONERROR);
		return EXIT_FAILURE;
	}

#ifndef CXBX_LOADER
	/*! verify Cxbx.exe is loaded to base address 0x00010000 */
	if ((UINT_PTR)GetModuleHandle(nullptr) != CXBX_BASE_ADDR)
	{
		/*! CXBX_BASE_ADDR is defined as 0x00010000, which is the base address of
		    the Cxbx.exe host executable.
		    Set in Cxbx Project options, Linker, Advanced, Base Address */
		MessageBox(NULL, "Cxbx.exe is not loaded to base address 0x00010000 (which is a requirement for Xbox emulation)", "Cxbx-Reloaded",
			MB_OK | MB_ICONERROR);
		return EXIT_FAILURE;
	}
#endif

	bool bRet, bKernel;
	HWND hWnd = nullptr;
	DWORD guiProcessID = 0;

	// TODO: Convert ALL __argc & __argv to use main(int argc, char** argv) method.
	if (__argc >= 2 && std::strcmp(__argv[1], "/load") == 0 && std::strlen(__argv[2]) > 0) {
		bKernel = true;

		// Perform check if command line contain gui's hWnd value.
		if (__argc > 3) {
			hWnd = (HWND)std::stoi(__argv[3], nullptr, 10);

			hWnd = IsWindow(hWnd) ? hWnd : nullptr;
			if (hWnd != nullptr) {
				// We don't need thread ID from window handle.
				GetWindowThreadProcessId(hWnd, &guiProcessID);
			}
		}
	}
	else {
		bKernel = false;
		guiProcessID = GetCurrentProcessId();
	}
	g_exec_filepath = __argv[0]; // NOTE: Workaround solution until simulated "main" function is made.

	/*! initialize shared memory */
	EmuShared::Init(guiProcessID);

	bool bFirstLaunch;
	g_EmuShared->GetIsFirstLaunch(&bFirstLaunch);

    /* check if process is launch with elevated access then prompt for continue on or not. */
	if (!bFirstLaunch) {

		g_Settings = new Settings();

		if (g_Settings == nullptr) {
			MessageBox(nullptr, szSettings_alloc_error, "Cxbx-Reloaded", MB_OK);
			EmuShared::Cleanup();
			return EXIT_FAILURE;
		}

		bRet = g_Settings->Init();
		if (!bRet) {
			EmuShared::Cleanup();
			return EXIT_FAILURE;
		}

		log_get_settings();

		bool bElevated = CxbxIsElevated();

		if (bElevated && !g_Settings->m_core.allowAdminPrivilege) {
			int ret = MessageBox(NULL, "Cxbx-Reloaded has detected that it has been launched with Administrator rights.\n"
			                    "\nThis is dangerous, as a maliciously modified Xbox titles could take control of your system.\n"
			                    "\nAre you sure you want to continue?", "Cxbx-Reloaded", MB_YESNO | MB_ICONWARNING);
			if (ret != IDYES) {
				EmuShared::Cleanup();
				return EXIT_FAILURE;
			}
		}
		g_EmuShared->SetIsFirstLaunch(true);
	}

	if (bKernel) {

		// NOTE: This is designated for standalone kernel mode launch without GUI
		if (g_Settings != nullptr) {

			// Reset to default
			g_EmuShared->Reset();

			g_Settings->Verify();
			g_Settings->SyncToEmulator();

			// We don't need to keep Settings open plus allow emulator to use unused memory.
			delete g_Settings;
			g_Settings = nullptr;

			// Perform identical to what GUI will do to certain EmuShared's variable before launch.
			g_EmuShared->SetIsEmulating(true);

			// NOTE: This setting the ready status is optional. Internal kernel process is checking if GUI is running.
			// Except if enforce check, then we need to re-set ready status every time for non-GUI.
			//g_EmuShared->SetIsReady(true);
		}

		/* Initialize Cxbx File Paths */
		CxbxInitFilePaths();

		CxbxKrnlMain(__argc, __argv);
		return EXIT_SUCCESS;
	}

	// If 2nd GUI executable is launched, load settings file for GUI for editable support.
	if (g_Settings == nullptr) {
		g_Settings = new Settings();

		if (g_Settings == nullptr) {
			MessageBox(nullptr, szSettings_alloc_error, "Cxbx-Reloaded", MB_OK);
			EmuShared::Cleanup();
			return EXIT_FAILURE;
		}

		bRet = g_Settings->Init();
		if (!bRet) {
			EmuShared::Cleanup();
			return EXIT_FAILURE;
		}

		log_get_settings();
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
    if(MainWindow->HasError())
    {
        MessageBox(NULL, MainWindow->GetError().c_str(), "Cxbx-Reloaded", MB_ICONSTOP | MB_OK);
    }

    delete MainWindow;

    /*! cleanup shared memory */
    EmuShared::Cleanup();

    return EXIT_SUCCESS;
}
