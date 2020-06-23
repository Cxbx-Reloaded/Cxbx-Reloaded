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
// *  (c) 2019 ergo720
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef SETTINGS_HPP
#define SETTINGS_HPP
#include "Cxbx.h"

#include "SimpleIni.h"
#include "input\InputDevice.h"
#include "common\util\CxbxUtil.h"
#include <string>
#include <array>

extern std::string g_exec_filepath;

// Individual library version
extern uint16_t g_LibVersion_D3D8;
extern uint16_t g_LibVersion_DSOUND;

#define szSettings_alloc_error "ERROR: Unable to allocate Settings class."
#define assert_check_shared_memory(type) \
 "Invalid "#type" size, please verify structure is align, not adding new member, or is using placeholder reserves." \
 " Otherwise, please perform versioning upgrade and update "#type" sizeof check."

// Cxbx-Reloaded's data storage location.
typedef enum _CXBX_DATA {
	CXBX_DATA_INVALID = -1,
	CXBX_DATA_APPDATA = 0,
	CXBX_DATA_EXECDIR = 1,
	CXBX_DATA_CUSTOM = 2,
} CXBX_DATA;

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
	static CXBX_DATA FindSettingsLocation(std::string& file_path_out);
	static CXBX_DATA SetupFile(std::string& file_path_out);

	// GUI settings
	struct s_gui {
		DebugMode CxbxDebugMode;
		std::string szCxbxDebugFile;
		std::string szRecentXbeFiles[10];
		unsigned int DataStorageToggle;
		std::string szCustomLocation = "";
		bool bIgnoreInvalidXbeSig;
		bool bIgnoreInvalidXbeSec;
	} m_gui;

	// Core settings
	struct s_core {
		unsigned int Revision;
        unsigned int FlagsLLE;
		DebugMode KrnlDebugMode;
		char szKrnlDebug[MAX_PATH] = "";
		char szStorageLocation[MAX_PATH] = "";
        unsigned int LoggedModules[NUM_INTEGERS_LOG];
		int LogLevel = 1;
		bool bUseLoaderExec;
		bool allowAdminPrivilege;
		bool bLogPopupTestCase;
		bool Reserved4 = 0;
		int  Reserved99[10] = { 0 };
	} m_core;
	static_assert(sizeof(s_core) == 0x24C, assert_check_shared_memory(s_core));

	// Video settings
	struct s_video {
		char szVideoResolution[100] = "";
        unsigned int adapter;
        unsigned int direct3DDevice;
		bool bVSync;
		bool bFullScreen;
		bool bHardwareYUV;
        bool Reserved3;
		int  renderScaleFactor = 1;
		int  Reserved99[9] = { 0 };
	} m_video;
	static_assert(sizeof(s_video) == 0x98, assert_check_shared_memory(s_video));

	// Audio settings
	struct s_audio {
		GUID adapterGUID;
		bool codec_pcm ;
		bool codec_xadpcm;
		bool codec_unknown;
		bool mute_on_unfocus;
		int  Reserved99[14] = { 0 };
	} m_audio;
	static_assert(sizeof(s_audio) == 0x4C, assert_check_shared_memory(s_audio));

	struct s_input {
		int Type;
		std::string DeviceName;
		std::string ProfileName;
	};
	std::array<s_input, 4> m_input;

	struct s_input_profiles {
		int Type;
		std::string ProfileName;
		std::string DeviceName;
		std::vector<std::string> ControlList;
	};
	std::array<std::vector<s_input_profiles>, to_underlying(XBOX_INPUT_DEVICE::DEVICE_MAX)> m_input_profiles;

	// Network settings
	struct s_network {
		char adapter_name[MAX_PATH] = "";
	} m_network;
	static_assert(sizeof(s_network) == 0x104, assert_check_shared_memory(s_network));

	// Hack settings
	// NOTE: When removing fields, replace them with place-holders
	// The size and order of this structure should *not* be allowed to change
	// TODO: Fix IPC/Shared Memory so this isn't necessary
	struct s_hack {
		bool DisablePixelShaders;
		bool Reserved2;
		bool UseAllCores;
		bool SkipRdtscPatching;
		bool Reserved3;
		bool Reserved4;
		bool Reserved7 = 0;
		bool Reserved8 = 0;
		int  Reserved99[8] = { 0 };
	} m_hacks;
	static_assert(sizeof(s_hack) == 0x28, assert_check_shared_memory(s_hack));

private:
	void RemoveLegacyConfigs(unsigned int CurrentRevision);
	std::string m_file_path = "";
	CSimpleIniA m_si;
	std::string m_current_data_location;
    unsigned int m_current_DataStorageToggle = -1;
};

// Settings manager
extern Settings* g_Settings;
extern const unsigned int settings_version;

#endif
