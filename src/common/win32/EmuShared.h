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
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef EMUSHARED_H
#define EMUSHARED_H

#include "Cxbx.h"
#include "common\Settings.hpp"
#include "Mutex.h"
#include "common\IPCHybrid.hpp"

#include <memory.h>

extern HMODULE hActiveModule; // Equals EXE Module handle in (GUI) Cxbx.exe / cxbxr.exe, equals DLL Module handle in cxbxr-emu.dll

typedef enum _XBOX_LED_COLOUR: unsigned char {
	XBOX_LED_COLOUR_OFF,
	XBOX_LED_COLOUR_GREEN,
	XBOX_LED_COLOUR_RED,
	XBOX_LED_COLOUR_ORANGE,
} XBOX_LED_COLOUR;

// Kernel boot flags
enum {
	BOOT_NONE =           0,
	BOOT_EJECT_PENDING =  1 << 0,
	BOOT_FATAL_ERROR =    1 << 1,
	BOOT_SKIP_ANIMATION = 1 << 2,
	BOOT_RUN_DASHBOARD =  1 << 3,
	BOOT_QUICK_REBOOT =   1 << 4,
};

// ******************************************************************
// * EmuShared : Shared memory
// ******************************************************************
class EmuShared : public Mutex
{
	public:
		int m_RefCount;

		// ******************************************************************
		// * Each process needs to call this to initialize shared memory
		// ******************************************************************
		static bool Init(DWORD guiProcessID);

		// ******************************************************************
		// * Each process needs to call this to cleanup shared memory
		// ******************************************************************
		static void Cleanup();

		// ******************************************************************
		// * Check if shared memory is used on launch
		// ******************************************************************
		void GetIsFirstLaunch(bool *isFirstLaunch) { Lock(); *isFirstLaunch = m_bFirstLaunch; Unlock(); }
		void SetIsFirstLaunch(const bool isFirstLaunch) { Lock(); m_bFirstLaunch = isFirstLaunch; Unlock(); }

		// ******************************************************************
		// * Check if parent process is emulating title
		// ******************************************************************
		void GetIsEmulating(bool *isEmulating) { Lock(); *isEmulating = m_bEmulating_status; Unlock(); }
		void SetIsEmulating(const bool isEmulating) { Lock(); m_bEmulating_status = isEmulating; Unlock(); }

		// ******************************************************************
		// * Each child process need to wait until parent process is ready
		// ******************************************************************
		void GetIsReady(bool *isReady) { Lock(); *isReady = m_bReady_status; Unlock(); }
		void SetIsReady(const bool isReady) { Lock(); m_bReady_status = isReady; Unlock(); }

		// ******************************************************************
		// * Check if previous kernel mode process is running.
		// ******************************************************************
		void GetKrnlProcID(unsigned int *krnlProcID) { Lock(); *krnlProcID = m_dwKrnlProcID; Unlock(); }
		void SetKrnlProcID(const unsigned int krnlProcID) { Lock(); m_dwKrnlProcID = krnlProcID; Unlock(); }

		// ******************************************************************
		// * Xbox Core Accessors
		// ******************************************************************
		void GetCoreSettings(      Settings::s_core *emulate) { Lock(); *emulate = m_core; Unlock(); }
		void SetCoreSettings(const Settings::s_core *emulate) { Lock(); m_core = *emulate; Unlock(); }

		// ******************************************************************
		// * Xbox Video Accessors
		// ******************************************************************
		void GetVideoSettings(      Settings::s_video *video) { Lock(); *video = m_video; Unlock(); }
		void SetVideoSettings(const Settings::s_video *video) { Lock(); m_video = *video; Unlock(); }

		// ******************************************************************
		// * Xbox Audio Accessors
		// ******************************************************************
		void GetAudioSettings(      Settings::s_audio *audio) { Lock(); *audio = m_audio; Unlock(); }
		void SetAudioSettings(const Settings::s_audio *audio) { Lock(); m_audio = *audio; Unlock(); }

		// ******************************************************************
		// * Xbox Controller Accessors
		// ******************************************************************
		void GetControllerDInputSettings(      Settings::s_controller_dinput *ctrl) { Lock(); *ctrl = m_controller_dinput; Unlock(); }
		void SetControllerDInputSettings(const Settings::s_controller_dinput *ctrl) { Lock(); m_controller_dinput = *ctrl; Unlock(); }
		void GetControllerPortSettings(      Settings::s_controller_port *ctrl) { Lock(); *ctrl = m_controller_port; Unlock(); }
		void SetControllerPortSettings(const Settings::s_controller_port *ctrl) { Lock(); m_controller_port = *ctrl; Unlock(); }

		// ******************************************************************
		// * LLE Flags Accessors
		// ******************************************************************
		void GetFlagsLLE(      uint *flags) { Lock(); *flags = m_core.FlagsLLE; Unlock(); }
		void SetFlagsLLE(const uint *flags) { Lock(); m_core.FlagsLLE = *flags; Unlock(); }

		// ******************************************************************
		// * Boot flag Accessors
		// ******************************************************************
		void GetBootFlags(int *value) { Lock(); *value = m_BootFlags_status; Unlock(); }
		void SetBootFlags(const int *value) { Lock(); m_BootFlags_status = *value; Unlock(); }

		// ******************************************************************
		// * Hack Flag Accessors
		// ******************************************************************
		void GetHackSettings(Settings::s_hack *hacks) { Lock(); *hacks = m_hacks; Unlock(); }
		void SetHackSettings(Settings::s_hack *hacks) { Lock(); m_hacks = *hacks; Unlock(); }

		void GetDisablePixelShaders(int* value) { Lock(); *value = m_hacks.DisablePixelShaders; Unlock(); }
		void SetDisablePixelShaders(const int* value) { Lock(); m_hacks.DisablePixelShaders = *value; Unlock(); }
		void GetUncapFramerate(int* value) { Lock(); *value = m_hacks.UncapFramerate; Unlock(); }
		void SetUncapFramerate(const int* value) { Lock(); m_hacks.UncapFramerate = *value; Unlock(); }
		void GetUseAllCores(int* value) { Lock(); *value = m_hacks.UseAllCores; Unlock(); }
		void SetUseAllCores(const int* value) { Lock(); m_hacks.UseAllCores = *value; Unlock(); }
		void GetSkipRdtscPatching(int* value) { Lock(); *value = m_hacks.SkipRdtscPatching; Unlock(); }
		void SetSkipRdtscPatching(const int* value) { Lock(); m_hacks.SkipRdtscPatching = *value; Unlock(); }
		void GetScaleViewport(int* value) { Lock(); *value = m_hacks.ScaleViewport; Unlock(); }
		void SetScaleViewport(const int* value) { Lock(); m_hacks.ScaleViewport = *value; Unlock(); }
		void GetDirectHostBackBufferAccess(int* value) { Lock(); *value = m_hacks.DirectHostBackBufferAccess; Unlock(); }
		void SetDirectHostBackBufferAccess(const int* value) { Lock(); m_hacks.DirectHostBackBufferAccess = *value; Unlock(); }

		// ******************************************************************
		// * FPS/Benchmark values Accessors
		// ******************************************************************
		void GetCurrentFPS(float *value) { Lock(); *value = m_FPS_status; Unlock(); }
		void SetCurrentFPS(const float *value) { Lock(); m_FPS_status = *value; Unlock(); }

		// ******************************************************************
		// * FPS/Benchmark values Accessors
		// ******************************************************************
		void GetIsKrnlLogEnabled(bool *value) { Lock(); *value = m_Krnl_Log_enabled; Unlock(); }
		void SetIsKrnlLogEnabled(const bool value) { Lock(); m_Krnl_Log_enabled = value; Unlock(); }

		// ******************************************************************
		// * Debugging flag Accessors
		// ******************************************************************
		void GetDebuggingFlag(bool *value) { Lock(); *value = m_bDebugging; Unlock(); }
		void SetDebuggingFlag(const bool *value) { Lock(); m_bDebugging = *value; Unlock(); }

		// ******************************************************************
		// * Log Level value Accessors
		// ******************************************************************
		void GetLogLv(int *value) { Lock(); *value = m_core.LogLevel; Unlock(); }
		void SetLogLv(int *value) { Lock(); m_core.LogLevel = *value; Unlock(); }

		// ******************************************************************
		// * Log modules value Accessors
		// ******************************************************************
		void GetLogModules(uint *value)
		{
			Lock();
			for (int i = 0; i < NUM_INTEGERS_LOG; ++i)
			{
				value[i] = m_core.LoggedModules[i];
			}
			Unlock();
		}
		void SetLogModules(uint *value)
		{
			Lock();
			for (int i = 0; i < NUM_INTEGERS_LOG; ++i)
			{
				m_core.LoggedModules[i] = value[i];
			}
			Unlock();
		}

		// ******************************************************************
		// * File storage location
		// ******************************************************************
		void GetStorageLocation(char *path) { Lock(); strncpy(path, m_core.szStorageLocation, MAX_PATH); Unlock(); }
		void SetStorageLocation(const char *path) { Lock(); strncpy(m_core.szStorageLocation, path, MAX_PATH); Unlock(); }

		// ******************************************************************
		// * Reset specific variables to default for kernel mode.
		// ******************************************************************
		void ResetKrnl()
		{
			Lock();
			m_BootFlags_status = 0;
			m_FPS_status = 0.0f;
			Unlock();
		}

		// ******************************************************************
		// * Reset specific variables to default for gui mode.
		// ******************************************************************
		void Reset()
		{
			Lock();
			ResetKrnl();
			m_bEmulating_status = 0;
			m_dwKrnlProcID = 0;
			Unlock();
		}

	private:
		// ******************************************************************
		// * Constructor / Deconstructor
		// ******************************************************************
		EmuShared();
		~EmuShared();

		// ******************************************************************
		// * Shared configuration
		// ******************************************************************
		int          m_BootFlags_status;
		unsigned int m_Reserved5;
		float        m_Reserved6;
		float        m_FPS_status; // NOTE: If move into ipc_send_gui_update will spam GUI's message system (one message per frame)
		bool         m_Krnl_Log_enabled; // Is require in order to preserve previous set for support multi-xbe.
		bool         m_bDebugging;
		bool         m_bReady_status;
		bool         m_bEmulating_status;
		int          m_Reserved7[4];
		bool         m_bFirstLaunch;
		bool         m_bReserved2;
		bool         m_bReserved3;
		bool         m_bReserved4;
		unsigned int m_dwKrnlProcID; // Only used for kernel mode level.
		int          m_Reserved99[32]; // Reserve space

		// Settings class in memory should not be tampered by third-party.
		// Third-party program should only be allow to edit settings.ini file.
		Settings::s_controller_dinput m_controller_dinput;
		Settings::s_controller_port m_controller_port;
		Settings::s_core m_core;
		Settings::s_video m_video;
		Settings::s_audio m_audio;
		Settings::s_hack m_hacks;
};

// ******************************************************************
// * Exported Global Shared Memory Pointer
// ******************************************************************
extern EmuShared *g_EmuShared;

#endif
