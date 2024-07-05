// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *  This file is part of the Cxbx-Reloaded project.
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
// *  All rights reserved
// *
// ******************************************************************
#define LOG_PREFIX CXBXR_MODULE::FILE

#ifdef CXBXR_EMU
#include "core/kernel/support/EmuFile.h" // For g_io_mu_metadata
#include "common/Timer.h" // For Timer_Shutdown
#include "core/common/video/RenderBase.hpp" // For g_renderbase
#include "core/kernel/memory-manager/VMManager.h"
extern void CxbxrKrnlSuspendThreads();
#endif

#include "cxbxr.hpp"

#include "EmuShared.h"
#include "Settings.hpp"
#include "Logging.h"
#include "win32/WineEnv.h"

#include "Settings.hpp"

volatile bool    g_bPrintfOn = true;

bool CreateSettings()
{
	g_Settings = new Settings();
	if (g_Settings == nullptr) {
		PopupError(nullptr, szSettings_alloc_error);
		return false;
	}

	if (!g_Settings->Init()) {
		return false;
	}

	log_get_settings();
	return true;
}

bool HandleFirstLaunch()
{
	bool bFirstLaunch;
	g_EmuShared->GetIsFirstLaunch(&bFirstLaunch);

	/* check if process is launch with elevated access then prompt for continue on or not. */
	if (!bFirstLaunch) {
		if (!CreateSettings()) {
			return false;
		}

		// Wine will always run programs as administrator by default, it can be safely disregard.
		// Since Wine doesn't use root permission. Unless user is running Wine as root.
		bool bElevated = CxbxrIsElevated();
		if (bElevated && !isWineEnv() && !g_Settings->m_core.allowAdminPrivilege) {
			PopupReturn ret = PopupWarningEx(nullptr, PopupButtons::YesNo, PopupReturn::No,
				"Cxbx-Reloaded has detected that it has been launched with Administrator rights.\n"
				"\nThis is dangerous, as a maliciously modified Xbox titles could take control of your system.\n"
				"\nAre you sure you want to continue?");
			if (ret != PopupReturn::Yes) {
				return false;
			}
		}

		g_EmuShared->SetIsFirstLaunch(true);
	}

	return true;
}

[[noreturn]] void CxbxrShutDown(bool is_reboot)
{
	if (!is_reboot) {
		// Clear all kernel boot flags. These (together with the shared memory) persist until Cxbx-Reloaded is closed otherwise.
		int BootFlags = 0;
		g_EmuShared->SetBootFlags(&BootFlags);
	}

	// NOTE: This causes a hang when exiting while NV2A is processing
	// This is okay for now: It won't leak memory or resources since TerminateProcess will free everything
	// delete g_NV2A; // TODO : g_pXbox

	// Shutdown the input device manager
	g_InputDeviceManager.Shutdown();

#ifdef CXBXR_EMU
	// NOTE: this code causes freezes/crashes at shutdown, so avoid for now
	// This is very important process to prevent false positive report and allow IDEs to continue debug multiple reboots.
	//CxbxrKrnlSuspendThreads();

	if (g_io_mu_metadata) {
		delete g_io_mu_metadata;
		g_io_mu_metadata = nullptr;
	}

	// Shutdown the render manager
	if (g_renderbase != nullptr) {
		g_renderbase->Shutdown();
		g_renderbase = nullptr;
	}

	// NOTE: Require to be after g_renderbase's shutdown process.
	// NOTE: Must be last step of shutdown process and before CxbxUnlockFilePath call!
	// Shutdown the memory manager
	g_VMManager.Shutdown();

	CxbxrUnlockFilePath();

	if (CxbxKrnl_hEmuParent != NULL && !is_reboot) {
		SendMessage(CxbxKrnl_hEmuParent, WM_PARENTNOTIFY, WM_DESTROY, 0);
	}
#endif

	EmuShared::Cleanup();

	TerminateProcess(GetCurrentProcess(), 0);
}

[[noreturn]] void CxbxrAbortEx(CXBXR_MODULE cxbxr_module, const char* szErrorMessage, ...)
{
	// print out error message (if exists)
	if (szErrorMessage != NULL)
	{
		char szBuffer2[1024];
		va_list argp;

		va_start(argp, szErrorMessage);
		vsprintf(szBuffer2, szErrorMessage, argp);
		va_end(argp);

		(void)PopupCustomEx(nullptr, cxbxr_module, LOG_LEVEL::FATAL, PopupIcon::Error, PopupButtons::Ok, PopupReturn::Ok, "Received Fatal Message:\n\n* %s\n", szBuffer2); // Will also EmuLogEx
	}

	EmuLogInit(LOG_LEVEL::INFO, "MAIN: Terminating Process");
	fflush(stdout);

	// cleanup debug output
	{
		FreeConsole();

		char buffer[16];

		if (GetConsoleTitle(buffer, 16) != NULL)
			freopen("nul", "w", stdout);
	}

	CxbxrShutDown();
}
