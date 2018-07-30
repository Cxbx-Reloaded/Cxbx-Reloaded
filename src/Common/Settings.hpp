// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Common->Settings.hpp
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
// *  (c) 2018 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef SETTINGS_HPP
#define SETTINGS_HPP
#include "Cxbx.h"

#include "SimpleIni.h"
#include <string>

#define szSettings_alloc_error "ERROR: Unable to allocate Settings class."

// ******************************************************************
// * Maximum number of devices allowed
// ******************************************************************
#define XBCTRL_MAX_GAMEPAD_PORTS 4

class Settings
{
public:
	bool Init();
	bool LoadUserConfig();
	bool LoadFile(std::string file_path);
	bool Save(std::string file_path = "");

	// Video settings
	struct s_video {
		char szVideoResolution[100] = "";
		uint adapter;
		uint direct3DDevice;
		bool bVSync;
		bool bFullScreen;
		bool bHardwareYUV;
	} m_video;

	// Audio settings
	struct s_audio {
		GUID adapterGUID;
		bool codec_pcm ;
		bool codec_xadpcm;
		bool codec_unknown;
	} m_audio;

	struct s_controller_port {
		uint XboxPortMapHostType[XBCTRL_MAX_GAMEPAD_PORTS] = { 1, 1, 1, 1 };
		uint XboxPortMapHostPort[XBCTRL_MAX_GAMEPAD_PORTS] = { 0, 1, 2, 3 };
	} m_controller_port;

	// Hack settings
	struct s_hack {
		bool DisablePixelShaders;
		bool UncapFramerate;
		bool UseAllCores;
		bool SkipRdtscPatching;
		bool ScaleViewport;
		bool DirectHostBackBufferAccess;
	} m_hacks;

private:
	std::string m_file_path = "";
	CSimpleIniA m_si;
};

// Settings manager
extern Settings* g_Settings;

#endif
