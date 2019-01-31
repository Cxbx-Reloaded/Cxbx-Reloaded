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
// *  (c) 2017-2018 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef SETTINGS_HPP
#define SETTINGS_HPP
#include "Cxbx.h"

#include "SimpleIni.h"
#include <string>

extern std::string g_exec_filepath;

#define szSettings_alloc_error "ERROR: Unable to allocate Settings class."

// Cxbx-Reloaded's data storage location.
typedef enum _CXBX_DATA {
	CXBX_DATA_APPDATA = 0,
	CXBX_DATA_EXECDIR = 1,
	CXBX_DATA_CUSTOM = 2,
} CXBX_DATA;

// ******************************************************************
// * Xbox Controller Object IDs
// ******************************************************************
enum XBCtrlObject
{
	// ******************************************************************
	// * Analog Axis
	// ******************************************************************
	XBCTRL_OBJECT_LTHUMBPOSX = 0,
	XBCTRL_OBJECT_LTHUMBNEGX,
	XBCTRL_OBJECT_LTHUMBPOSY,
	XBCTRL_OBJECT_LTHUMBNEGY,
	XBCTRL_OBJECT_RTHUMBPOSX,
	XBCTRL_OBJECT_RTHUMBNEGX,
	XBCTRL_OBJECT_RTHUMBPOSY,
	XBCTRL_OBJECT_RTHUMBNEGY,
	// ******************************************************************
	// * Analog Buttons
	// ******************************************************************
	XBCTRL_OBJECT_A,
	XBCTRL_OBJECT_B,
	XBCTRL_OBJECT_X,
	XBCTRL_OBJECT_Y,
	XBCTRL_OBJECT_BLACK,
	XBCTRL_OBJECT_WHITE,
	XBCTRL_OBJECT_LTRIGGER,
	XBCTRL_OBJECT_RTRIGGER,
	// ******************************************************************
	// * Digital Buttons
	// ******************************************************************
	XBCTRL_OBJECT_DPADUP,
	XBCTRL_OBJECT_DPADDOWN,
	XBCTRL_OBJECT_DPADLEFT,
	XBCTRL_OBJECT_DPADRIGHT,
	XBCTRL_OBJECT_BACK,
	XBCTRL_OBJECT_START,
	XBCTRL_OBJECT_LTHUMB,
	XBCTRL_OBJECT_RTHUMB,
	// ******************************************************************
	// * Total number of components
	// ******************************************************************
	XBCTRL_OBJECT_COUNT
};

// ******************************************************************
// * Maximum number of devices allowed
// ******************************************************************
#define XBCTRL_MAX_DEVICES XBCTRL_OBJECT_COUNT

#define XBCTRL_MAX_GAMEPAD_PORTS 4

// ******************************************************************
// * Xbox Controller Object Config
// ******************************************************************
struct XBCtrlObjectCfg
{
	int32_t dwDevice;   // offset into m_InputDevice
	int32_t dwInfo;     // extended information, depending on dwFlags
	int32_t dwFlags;    // flags explaining the data format
};

// ******************************************************************
// * Define number of integers required to store logging settings
// ******************************************************************
#define NUM_INTEGERS_LOG 2

enum {
	LLE_NONE = 0,
	LLE_APU = 1 << 0,
	LLE_GPU = 1 << 1,
	LLE_JIT = 1 << 2,
	LLE_USB = 1 << 3,
};

class Settings
{
public:
	bool Init();
	bool LoadUserConfig();
	bool LoadFile(std::string file_path);
	bool LoadConfig();
	bool Save(std::string file_path = "");
	void Delete();
	void SyncToEmulator();
	void Verify();
	std::string GetDataLocation();

	// GUI settings
	struct s_gui {
		DebugMode CxbxDebugMode;
		std::string szCxbxDebugFile;
		std::string szRecentXbeFiles[10];
		uint DataStorageToggle;
		std::string szCustomLocation = "";
	} m_gui;

	// Core settings
	struct s_core {
		uint FlagsLLE;
		DebugMode KrnlDebugMode;
		char szKrnlDebug[MAX_PATH] = "";
		char szStorageLocation[MAX_PATH] = "";
		bool allowAdminPrivilege;
		uint LoggedModules[NUM_INTEGERS_LOG];
		int LogLevel = 1;
		bool Reserved2 = 0;
		bool Reserved3 = 0;
		bool Reserved4 = 0;
		int  Reserved99[10] = { 0 };
	} m_core;

	// Video settings
	struct s_video {
		char szVideoResolution[100] = "";
		uint adapter;
		uint direct3DDevice;
		bool bVSync;
		bool bFullScreen;
		bool bHardwareYUV;
		bool Reserved4 = 0;
		int  Reserved99[10] = { 0 };
	} m_video;

	// Audio settings
	struct s_audio {
		GUID adapterGUID;
		bool codec_pcm ;
		bool codec_xadpcm;
		bool codec_unknown;
		bool Reserved4 = 0;
		int  Reserved99[14] = { 0 };
	} m_audio;

	// Controller settings
	struct s_controller_dinput {

		// ******************************************************************
		// * Input Device Name Lookup Table
		// ******************************************************************
		static const char *XboxControllerObjectNameLookup[XBCTRL_OBJECT_COUNT];

		// ******************************************************************
		// * Device Names
		// ******************************************************************
		char DeviceName[XBCTRL_MAX_DEVICES][MAX_PATH];

		// ******************************************************************
		// * Object Configuration
		// ******************************************************************
		XBCtrlObjectCfg ObjectConfig[XBCTRL_OBJECT_COUNT];

	} m_controller_dinput;

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
		bool Reserved7 = 0;
		bool Reserved8 = 0;
		int  Reserved99[8] = { 0 };
	} m_hacks;

private:
	std::string m_file_path = "";
	CSimpleIniA m_si;
	std::string m_current_data_location;
	uint m_current_DataStorageToggle = -1;
};

// Settings manager
extern Settings* g_Settings;
extern const uint settings_version;

#endif
