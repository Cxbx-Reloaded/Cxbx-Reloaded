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
#include "Common/Win32/XBController.h"
#include "Common/Win32/XBVideo.h"
#include "Common/Win32/XBAudio.h"

#include <memory.h>

enum {
	LLE_APU = 1 << 0,
	LLE_GPU = 1 << 1,
	LLE_JIT = 1 << 2,
};

typedef UINT_PTR PAddr;

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

		void EmuShared::Load();
		void EmuShared::Save();

		// ******************************************************************
		// * Each process needs to call this to cleanup shared memory
		// ******************************************************************
		static void Cleanup();

		// ******************************************************************
		// * Xbox Video Accessors
		// ******************************************************************
		void GetXBVideo(      XBVideo *video) { Lock(); *video = XBVideo(m_XBVideo); Unlock(); }
		void SetXBVideo(const XBVideo *video) { Lock(); m_XBVideo = XBVideo(*video); Unlock(); }

		// ******************************************************************
		// * Xbox Audio Accessors
		// ******************************************************************
		void GetXBAudio(      XBAudio *audio) { Lock(); *audio = XBAudio(m_XBAudio); Unlock(); }
		void SetXBAudio(const XBAudio *audio) { Lock(); m_XBAudio = XBAudio(*audio); Unlock(); }

		// ******************************************************************
		// * Xbox Controller Accessors
		// ******************************************************************
		void GetXBController(      XBController *ctrl) { Lock(); *ctrl = XBController(m_XBController); Unlock(); }
		void SetXBController(const XBController *ctrl) { Lock(); m_XBController = XBController(*ctrl); Unlock(); }

		// ******************************************************************
		// * Xbe Path Accessors
		// ******************************************************************
		void GetXbePath(      char *path) { Lock(); strcpy(path, m_XbePath); Unlock(); }
		void SetXbePath(const char *path) { Lock(); strcpy(m_XbePath, path); Unlock(); }

		// ******************************************************************
		// * LLE Flags Accessors
		// ******************************************************************
		void GetFlagsLLE(      int *flags) { Lock(); *flags = m_FlagsLLE; Unlock(); }
		void SetFlagsLLE(const int *flags) { Lock(); m_FlagsLLE = *flags; Unlock(); }

		// ******************************************************************
		// * XInput Flag Accessors
		// ******************************************************************
		void GetXInputEnabled(int* value) { Lock(); *value = m_XInputEnabled; Unlock(); }
		void SetXInputEnabled(int* value) { Lock(); m_XInputEnabled = *value; Unlock(); }

		// ******************************************************************
		// * Hack Flag Accessors
		// ******************************************************************
		void GetDisablePixelShaders(int* value) { Lock(); *value = m_DisablePixelShaders; Unlock(); }
		void SetDisablePixelShaders(int* value) { Lock(); m_DisablePixelShaders = *value; Unlock(); }

		// ******************************************************************
		// * MSpF/Benchmark values Accessors
		// ******************************************************************
		void GetCurrentMSpF(float *value) { Lock(); *value = m_MSpF; Unlock(); }
		void SetCurrentMSpF(float *value) { Lock(); m_MSpF = *value; Unlock(); }

		// ******************************************************************
		// * FPS/Benchmark values Accessors
		// ******************************************************************
		void GetCurrentFPS(float *value) { Lock(); *value = m_FPS; Unlock(); }
		void SetCurrentFPS(float *value) { Lock(); m_FPS = *value; Unlock(); }

		// ******************************************************************
		// * MultiXbe flag Accessors
		// ******************************************************************
		void GetMultiXbeFlag(bool *value) { Lock(); *value = m_bMultiXbe; Unlock(); }
		void SetMultiXbeFlag(bool *value) { Lock(); m_bMultiXbe = *value; Unlock(); }

		// ******************************************************************
		// * Launch data physical address Accessors
		// ******************************************************************
		void GetLaunchDataPAddress(PAddr *value) { Lock(); *value = m_LaunchDataPAddress; Unlock(); }
		void SetLaunchDataPAddress(PAddr *value) { Lock(); m_LaunchDataPAddress = *value; Unlock(); }


	private:
		// ******************************************************************
		// * Constructor / Deconstructor
		// ******************************************************************
		EmuShared();
		~EmuShared();

		// ******************************************************************
		// * Shared configuration
		// ******************************************************************
		XBController m_XBController;
		XBVideo      m_XBVideo;
		XBAudio      m_XBAudio;
		char         m_XbePath[MAX_PATH];
		int          m_FlagsLLE;
		int			 m_XInputEnabled;
		int			 m_DisablePixelShaders;
		float		 m_MSpF;
		float        m_FPS;
		bool		 m_bMultiXbe;
		PAddr		 m_LaunchDataPAddress;
};

// ******************************************************************
// * Exported Global Shared Memory Pointer
// ******************************************************************
extern EmuShared *g_EmuShared;

#endif
