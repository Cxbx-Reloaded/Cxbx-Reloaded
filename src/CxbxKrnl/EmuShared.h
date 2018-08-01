// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuShared.h
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
#include "Common/Settings.hpp"
#include "Mutex.h"

#include <memory.h>

enum {
	XBOX_LED_COLOUR_OFF,
	XBOX_LED_COLOUR_GREEN,
	XBOX_LED_COLOUR_RED,
	XBOX_LED_COLOUR_ORANGE,
};

enum {
	LLE_NONE = 0,
	LLE_APU = 1 << 0,
	LLE_GPU = 1 << 1,
	LLE_JIT = 1 << 2,
	LLE_USB = 1 << 3,
};

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
		static void Init();

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
		void GetIsEmulating(bool *isEmulating) { Lock(); *isEmulating = m_bEmulating; Unlock(); }
		void SetIsEmulating(const bool isEmulating) { Lock(); m_bEmulating = isEmulating; Unlock(); }

		// ******************************************************************
		// * Each child process need to wait until parent process is ready
		// ******************************************************************
		void GetIsReady(bool *isReady) { Lock(); *isReady = m_bReady; Unlock(); }
		void SetIsReady(const bool isReady) { Lock(); m_bReady = isReady; Unlock(); }

		// ******************************************************************
		// * Check if previous kernel mode process is running.
		// ******************************************************************
		void GetKrnlProcID(unsigned int *krnlProcID) { Lock(); *krnlProcID = m_dwKrnlProcID; Unlock(); }
		void SetKrnlProcID(const unsigned int krnlProcID) { Lock(); m_dwKrnlProcID = krnlProcID; Unlock(); }

		// ******************************************************************
		// * Xbox Emulate Accessors
		// ******************************************************************
		void GetEmulateSettings(      Settings::s_emulate *emulate) { Lock(); *emulate = m_emulate; Unlock(); }
		void SetEmulateSettings(const Settings::s_emulate *emulate) { Lock(); m_emulate = *emulate; Unlock(); }

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
		// * Xbe Path Accessors
		// ******************************************************************
		void GetXbePath(      char *path) { Lock(); strncpy(path, m_XbePath, MAX_PATH); Unlock(); }
		void SetXbePath(const char *path) { Lock(); strncpy(m_XbePath, path, MAX_PATH); Unlock(); }

		// ******************************************************************
		// * LLE Flags Accessors
		// ******************************************************************
		void GetFlagsLLE(      uint *flags) { Lock(); *flags = m_emulate.FlagsLLE; Unlock(); }
		void SetFlagsLLE(const uint *flags) { Lock(); m_emulate.FlagsLLE = *flags; Unlock(); }

		// ******************************************************************
		// * Boot flag Accessors
		// ******************************************************************
		void GetBootFlags(int *value) { Lock(); *value = m_BootFlags; Unlock(); }
		void SetBootFlags(const int *value) { Lock(); m_BootFlags = *value; Unlock(); }

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
		// * MSpF/Benchmark values Accessors
		// ******************************************************************
		void GetCurrentMSpF(float *value) { Lock(); *value = m_MSpF; Unlock(); }
		void SetCurrentMSpF(const float *value) { Lock(); m_MSpF = *value; Unlock(); }

		// ******************************************************************
		// * FPS/Benchmark values Accessors
		// ******************************************************************
		void GetCurrentFPS(float *value) { Lock(); *value = m_FPS; Unlock(); }
		void SetCurrentFPS(const float *value) { Lock(); m_FPS = *value; Unlock(); }

		// ******************************************************************
		// * Debugging flag Accessors
		// ******************************************************************
		void GetDebuggingFlag(bool *value) { Lock(); *value = m_bDebugging; Unlock(); }
		void SetDebuggingFlag(const bool *value) { Lock(); m_bDebugging = *value; Unlock(); }

		// ******************************************************************
		// * Xbox LED values Accessors
		// ******************************************************************
		void GetLedSequence(int *value)
		{
			Lock();
			for (int i = 0; i < 4; ++i)
			{
				value[i] = m_LedSequence[i];
			}
			Unlock();
		}
		void SetLedSequence(int *value)
		{
			Lock();
			for (int i = 0; i < 4; ++i)
			{
				m_LedSequence[i] = value[i];
			}
			Unlock();
		}

		// ******************************************************************
		// * File storage location
		// ******************************************************************
		void GetStorageLocation(char *path) { Lock(); strncpy(path, m_emulate.szStorageLocation, MAX_PATH); Unlock(); }
		void SetStorageLocation(const char *path) { Lock(); strncpy(m_emulate.szStorageLocation, path, MAX_PATH); Unlock(); }

		// ******************************************************************
		// * Reset specific variables to default for kernel mode.
		// ******************************************************************
		void ResetKrnl()
		{
			Lock();
			m_BootFlags = 0;
			m_MSpF = 0.0f;
			m_FPS = 0.0f;
			Unlock();
		}

		// ******************************************************************
		// * Reset specific variables to default for gui mode.
		// ******************************************************************
		void Reset()
		{
			Lock();
			ResetKrnl();
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
		Settings::s_controller_dinput m_controller_dinput;
		Settings::s_controller_port m_controller_port;
		Settings::s_emulate m_emulate;
		Settings::s_video m_video;
		Settings::s_audio m_audio;
		Settings::s_hack m_hacks;
		char         m_XbePath[MAX_PATH];
		int          m_BootFlags;
		int          m_Reserved1;
		float        m_MSpF;
		float        m_FPS;
		bool         m_bReserved1;
		bool         m_bDebugging;
		bool         m_bReady;
		bool         m_bEmulating;
		int          m_LedSequence[4];
		bool         m_bFirstLaunch;
		bool         m_bReserved2;
		bool         m_bReserved3;
		bool         m_bReserved4;
		unsigned int m_dwKrnlProcID; // Only used for kernel mode level.
};

// ******************************************************************
// * Exported Global Shared Memory Pointer
// ******************************************************************
extern EmuShared *g_EmuShared;

#endif
