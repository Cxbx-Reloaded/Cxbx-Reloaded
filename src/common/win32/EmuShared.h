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
#include "common\input\Button.h"

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
		// * Fixed memory allocation size
		// ******************************************************************
		unsigned int m_size;

		// ******************************************************************
		// * Each process needs to call this to initialize shared memory
		// ******************************************************************
		static bool Init(long long sessionID);

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
		// * Xbox Network Accessors
		// ******************************************************************
		void GetNetworkSettings(Settings::s_network *network) { Lock(); *network = m_network; Unlock(); }
		void SetNetworkSettings(const Settings::s_network *network) { Lock(); m_network = *network; Unlock(); }

		// ******************************************************************
		// * Input config Accessors
		// ******************************************************************
		void GetInputDevTypeSettings(int* type, int port) { Lock(); *type = m_DeviceType[port]; Unlock(); }
		void SetInputDevTypeSettings(const int* type, int port) { Lock(); m_DeviceType[port] = *type; Unlock(); }
		void GetInputDevNameSettings(char* name, int port) { Lock(); strncpy(name, m_DeviceName[port], 50); Unlock(); }
		void SetInputDevNameSettings(const char* name, int port) { Lock(); strncpy(m_DeviceName[port], name, 50); Unlock(); }
		void GetInputBindingsSettings(char button_str[][30], int max_num_buttons, int port)
		{
			assert(max_num_buttons <= XBOX_CTRL_NUM_BUTTONS);
			Lock();
			for (int i = 0; i < max_num_buttons; i++) {
				strncpy(button_str[i], m_DeviceControlNames[port][i], 30);
			}
			Unlock();
		}
		void SetInputBindingsSettings(const char button_str[][30], int max_num_buttons, int port)
		{
			assert(max_num_buttons <= XBOX_CTRL_NUM_BUTTONS);
			Lock();
			for (int i = 0; i < max_num_buttons; i++) {
				strncpy(m_DeviceControlNames[port][i], button_str[i], 30);
			}
			Unlock();
		}

		// ******************************************************************
		// * LLE Flags Accessors
		// ******************************************************************
		void GetFlagsLLE(unsigned int *flags) { Lock(); *flags = m_core.FlagsLLE; Unlock(); }
		void SetFlagsLLE(const unsigned int *flags) { Lock(); m_core.FlagsLLE = *flags; Unlock(); }

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
		void GetUseAllCores(int* value) { Lock(); *value = m_hacks.UseAllCores; Unlock(); }
		void SetUseAllCores(const int* value) { Lock(); m_hacks.UseAllCores = *value; Unlock(); }
		void GetSkipRdtscPatching(int* value) { Lock(); *value = m_hacks.SkipRdtscPatching; Unlock(); }
		void SetSkipRdtscPatching(const int* value) { Lock(); m_hacks.SkipRdtscPatching = *value; Unlock(); }

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
#ifndef CXBX_LOADER // Temporary usage for cxbx.exe's emu
		// ******************************************************************
		// * Previous Memory Layout value Accessors
		// ******************************************************************
		void GetMmLayout(unsigned int* value) { Lock(); *value = m_PreviousMmLayout; Unlock(); }
		void SetMmLayout(unsigned int* value) { Lock(); m_PreviousMmLayout = *value; Unlock(); }
#endif
		// ******************************************************************
		// * Log Level value Accessors
		// ******************************************************************
		void GetLogLv(int *value) { Lock(); *value = m_core.LogLevel; Unlock(); }
		void SetLogLv(int *value) { Lock(); m_core.LogLevel = *value; Unlock(); }

		// ******************************************************************
		// * Log modules value Accessors
		// ******************************************************************
		void GetLogModules(unsigned int *value)
		{
			Lock();
			for (int i = 0; i < NUM_INTEGERS_LOG; ++i) {
				value[i] = m_core.LoggedModules[i];
			}
			Unlock();
		}
		void SetLogModules(unsigned int *value)
		{
			Lock();
			for (int i = 0; i < NUM_INTEGERS_LOG; ++i) {
				m_core.LoggedModules[i] = value[i];
			}
			Unlock();
		}

		// ******************************************************************
		// * Log Level value Accessors
		// ******************************************************************
		void GetLogPopupTestCase(bool *value) { Lock(); *value = m_core.bLogPopupTestCase; Unlock(); }
		void SetLogPopupTestCase(const bool value) { Lock(); m_core.bLogPopupTestCase = value; Unlock(); }

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
#ifndef CXBX_LOADER // Temporary usage for cxbx.exe's emu
		unsigned int m_PreviousMmLayout;
		int          m_Reserved7[3];
#else
		int          m_Reserved7[4];
#endif
		bool         m_bFirstLaunch;
		bool         m_bReserved2;
		bool         m_bReserved3;
		bool         m_bReserved4;
		unsigned int m_dwKrnlProcID; // Only used for kernel mode level.
		int          m_DeviceType[4];
		char         m_DeviceControlNames[4][XBOX_CTRL_NUM_BUTTONS][30]; // macro should be num of buttons of dev with highest num buttons
		char         m_DeviceName[4][50];
		int          m_Reserved99[28]; // Reserve space

		// Settings class in memory should not be tampered by third-party.
		// Third-party program should only be allow to edit settings.ini file.
		Settings::s_core m_core;
		Settings::s_video m_video;
		Settings::s_audio m_audio;
		Settings::s_network m_network;
		Settings::s_hack m_hacks;
};

// ******************************************************************
// * Exported Global Shared Memory Pointer
// ******************************************************************
extern EmuShared *g_EmuShared;

#endif
