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
// *  (c) 2018 wutno (#/g/punk - Rizon)
// *  (c) 2018 RadWolfie
// *  (c) 2019 ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

#define LOG_PREFIX CXBXR_MODULE::CXBXR

#include "Settings.hpp"
#include "core\kernel\support\Emu.h"
#include "EmuShared.h"
#include <filesystem>
#include "common\input\InputManager.h"
#include "common\input\layout_xbox_device.h"
#include <fstream>
#include "common/util/cliConfig.hpp"

// TODO: Implement Qt support when real CPU emulation is available.
#ifndef QT_VERSION // NOTE: Non-Qt will be using current directory for data
#include <ShlObj.h> // For SHGetSpecialFolderPath and CSIDL_APPDATA
#else
static_assert(false, "Please implement support for cross-platform's user profile data.");

#include <QDir> // for create directory
#include <QFile> // for check file existance
#include <QStandardPaths> // for cross-platform's user profile support
#endif

// Individual library version
uint16_t g_LibVersion_D3D8 = 0;
uint16_t g_LibVersion_DSOUND = 0;

// NOTE: Update settings_version when add/edit/delete setting's structure.
///////////////////////////
// * History:
// * 2: (RadWolfie), initial version
// * 3: (ergo720),   added logging settings
// * 4: (LukeUsher), added network settings
// * 5: (ergo720),   added new input gui settings and revision to core
// * 6: (RadWolfie), added loader executable member to core, only for clean up loader expertimental setting
// * 7: (RadWolfie), fix allowAdminPrivilege not align with other boolean members
// * 8: (ergo720),   added general input settings
// * 9: (LukeUsher), replaced HardwareYUV with MaintainAspect
///////////////////////////
const unsigned int settings_version = 9;

Settings* g_Settings = nullptr;

#define szSettings_setup_error "ERROR: Either setup have a problem or do not have write permission to directory."
#define szSettings_init_error "ERROR: Unable to initialize Settings class."
#define szSettings_save_user_option_message "Use Cxbx-Reloaded in Portable Mode?" \
                                            "\n\nClicking 'Yes' will save your setting in the current directory, rather than your User Profile."

#define szSettings_settings_file "/settings.ini"
#define szSettings_cxbx_reloaded_directory "/Cxbx-Reloaded"

static const char* section_gui = "gui";
static struct {
	const char* CxbxDebugMode = "CxbxDebugMode";
	const char* CxbxDebugLogFile = "CxbxDebugLogFile";
	const char* RecentXbeFiles = "RecentXbeFiles";
	const char* DataStorageToggle = "DataStorageToggle";
	const char* DataCustomLocation = "DataCustomLocation";
	const char* IgnoreInvalidXbeSig = "IgnoreInvalidXbeSig";
	const char *IgnoreInvalidXbeSec = "IgnoreInvalidXbeSec";
} sect_gui_keys;

static const char* section_core = "core";
static struct {
	const char* Revision = "Revision";
	const char* FlagsLLE = "FlagsLLE";
	const char* KrnlDebugMode = "KrnlDebugMode";
	const char* KrnlDebugLogFile = "KrnlDebugLogFile";
	const char* AllowAdminPrivilege = "AllowAdminPrivilege";
	const char* LoggedModules = "LoggedModules";
	const char* LogLevel = "LogLevel";
	const char* LoaderExecutable = "LoaderExecutable";
	const char* LogPopupTestCase = "LogPopupTestCase";
} sect_core_keys;

static const char* section_video = "video";
static struct {
	const char* VideoResolution = "VideoResolution";
	const char* adapter = "adapter";
	const char* Direct3DDevice = "Direct3DDevice";
	const char* VSync = "VSync";
	const char* FullScreen = "FullScreen";
	const char* MaintainAspect = "MaintainAspect";
	const char* RenderResolution = "RenderResolution";
} sect_video_keys;

static const char* section_overlay = "overlay";
static struct {
	const char* FPS = "FPS";
	const char* hle_lle_stats = "HLE/LLE Stats";
} sect_overlay_keys;

static const char* section_audio = "audio";
static struct {
	const char* adapter = "adapter";
	const char* adapter_value = "%08X %04X %04X %02X%02X %02X%02X%02X%02X%02X%02X";
	const char* codec_pcm = "PCM";
	const char* codec_xadpcm = "XADPCM";
	const char* codec_unknown = "UnknownCodec";
	const char* mute_on_unfocus = "MuteOnUnfocus";
} sect_audio_keys;

static const char* section_network = "network";
static struct {
	const char* adapter_name = "adapter_name";
} sect_network_keys;

static const char *section_input_general = "input-general";
static struct {
	const char *mo_axis_range = "MouseAxisRange";
	const char *mo_wheel_range = "MouseWheelRange";
	const char *ignore_kbmo_unfocus = "IgnoreKbMoUnfocus";
} sect_input_general;

static const char* section_controller_dinput = "controller-dinput";
static const char* section_controller_port = "controller-port";

static const char* section_input_port = "input-port-";
static struct {
	const char* type = "Type";
	const char* device = "DeviceName";
	const char* config = "ProfileName";
} sect_input_port;

static const char* section_input_profiles = "input-profile-";
static struct {
	const char* type = "Type";
	const char* config = "ProfileName";
	const char* device = "DeviceName";
	const char* control = "%s";
} sect_input_profiles;

static const char* section_hack = "hack";
static struct {
	const char* DisablePixelShaders = "DisablePixelShaders";
	const char* UseAllCores = "UseAllCores";
	const char* SkipRdtscPatching = "SkipRdtscPatching";
} sect_hack_keys;

std::string GenerateExecDirectoryStr()
{
	std::string exec_path;
	(void)cli_config::GetValue(cli_config::exec, &exec_path);
	return exec_path.substr(0, exec_path.find_last_of("\\/"));
}

// NOTE: This function will be only have Qt support, std::filesystem doesn't have generic support.
// Plus appending support for each OSes are not worthy to work on.
std::string GenerateUserProfileDirectoryStr()
{
	// ========================================================
	// TODO: Use QT's QDir and QFile classes for cross-platform
	// with QStandardPaths::GenericDataLocation for generic User Profile location.
	// NOTE: LibRetro compile build will not have user profile option support.
	// ========================================================
	char folderOption[MAX_PATH];
	std::string genDirectory;
	// TODO: Use QDir and QStandardPaths::GenericDataLocation for get user profile directory to support cross-platform
	BOOL bRet = SHGetSpecialFolderPathA(NULL, folderOption, CSIDL_APPDATA, TRUE); // NOTE: Windows only support
	if (!bRet) {
		return "";
	}
	genDirectory = folderOption;
	genDirectory.append(szSettings_cxbx_reloaded_directory);

	return genDirectory;
}

std::string TrimQuoteFromString(const char* data)
{
	// Safeguard before continue on.
	if (data == nullptr) {
		return "";
	}

	std::string trim_str = data;
	size_t len = trim_str.size();
	if (len > 0) {
		// New method, in order to support spaces inside DeviceName value.
		if (data[0] == '"') {
			len--; // Let's decrement since it also can act as offset too.
			if (trim_str.at(len) == '"') {
				len--;
			}
			trim_str = trim_str.substr(1, len);
		}
		// Old method, don't do anything special.
	}
	return trim_str;
}

#define AppendQuoteToString(d) "\"" + std::string(d) + "\""

bool Settings::Init()
{
	m_si.SetMultiKey(true);

	bool bRet = LoadUserConfig();

	// Enter setup installer process
	if (!bRet) {

		std::string setupFile;
		m_gui.DataStorageToggle = SetupFile(setupFile);

		if (m_gui.DataStorageToggle == CXBX_DATA_INVALID) {
			return false;
		}

		// Call LoadConfig, this will load the config, applying defaults for any missing fields
		bRet = LoadConfig();

		if (!bRet) {
			PopupError(nullptr, szSettings_setup_error);
			return false;
		}

		bRet = Save(setupFile);
	}
	return bRet;
}

bool Settings::LoadUserConfig()
{
	std::string fileSearch;
	m_gui.DataStorageToggle = FindSettingsLocation(fileSearch);

	if (m_gui.DataStorageToggle == CXBX_DATA_INVALID) {
		return false;
	}

	return LoadFile(fileSearch);
}

bool Settings::LoadFile(std::string file_path)
{
	std::list<CSimpleIniA::Entry> si_list;
	std::list<CSimpleIniA::Entry>::iterator si_list_iterator;

	SI_Error siError = m_si.LoadFile(file_path.c_str());

	if (siError != SI_OK) {
		return false;
	}
	m_file_path = file_path;

	return LoadConfig();
}

bool Settings::LoadConfig()
{
	bool bRet;
	const char* si_data;
	int iStatus;
	std::list<CSimpleIniA::Entry> si_list;
	std::list<CSimpleIniA::Entry>::iterator si_list_iterator;
	std::string trim_str;

	// ==== GUI Begin ===========

	m_gui.CxbxDebugMode = (DebugMode)m_si.GetLongValue(section_gui, sect_gui_keys.CxbxDebugMode, /*Default=*/DM_NONE);
	si_data = m_si.GetValue(section_gui, sect_gui_keys.CxbxDebugLogFile, /*Default=*/nullptr);
	// Fallback to null string if value is empty or contain bigger string.
	if (si_data == nullptr || std::strlen(si_data) >= MAX_PATH) {
		m_gui.szCxbxDebugFile = "";
	}
	else {
		m_gui.szCxbxDebugFile = si_data;
	}

	m_gui.DataStorageToggle = m_si.GetLongValue(section_gui, sect_gui_keys.DataStorageToggle, /*Default=*/m_gui.DataStorageToggle);
	si_data = m_si.GetValue(section_gui, sect_gui_keys.DataCustomLocation, /*Default=*/nullptr);
	// Fallback to null string if value is empty or contains a bigger string.
	if (si_data == nullptr || std::strlen(si_data) >= MAX_PATH) {
		m_gui.szCustomLocation = "";
	}
	else {
		m_gui.szCustomLocation = si_data;
	}

	// GUI - Recent xbe file paths
	unsigned int index = 0;
    unsigned int list_max = std::size(m_gui.szRecentXbeFiles);
	bRet = m_si.GetAllValues(section_gui, sect_gui_keys.RecentXbeFiles, si_list);/*Default=empty list*/
	if (bRet) {
		si_list_iterator = si_list.begin();
		for (si_list_iterator; si_list_iterator != si_list.end(); si_list_iterator++) {
			// Exit loop when the list has reached the limit.
			if (index == list_max) {
				break;
			}
			// Do not accept any file path greater or equal to MAX_PATH
			if (std::strlen(si_list_iterator->pItem) >= MAX_PATH) {
				continue;
			}
			m_gui.szRecentXbeFiles[index] = si_list_iterator->pItem;
			index++;
		}
	}
	// Set all or remaining recent xbe file paths to null string
	while (index < list_max) {
		m_gui.szRecentXbeFiles[index] = "";
		index++;
	}

	m_gui.bIgnoreInvalidXbeSig = m_si.GetBoolValue(section_gui, sect_gui_keys.IgnoreInvalidXbeSig, /*Default=*/false);
	m_gui.bIgnoreInvalidXbeSec = m_si.GetBoolValue(section_gui, sect_gui_keys.IgnoreInvalidXbeSec, /*Default=*/false);

	// ==== GUI End =============

	// ==== Core Begin ==========

	m_core.Revision = m_si.GetLongValue(section_core, sect_core_keys.Revision, 4);
	m_core.FlagsLLE = m_si.GetLongValue(section_core, sect_core_keys.FlagsLLE, /*Default=*/LLE_NONE);
	m_core.KrnlDebugMode = (DebugMode)m_si.GetLongValue(section_core, sect_core_keys.KrnlDebugMode, /*Default=*/DM_NONE);
	si_data = m_si.GetValue(section_core, sect_core_keys.KrnlDebugLogFile, /*Default=*/nullptr);
	// Fallback to null string if value is empty or contain bigger string.
	if (si_data == nullptr || std::strlen(si_data) >= MAX_PATH) {
		m_core.szKrnlDebug[0] = '\0';
	}
	else {
		std::strncpy(m_core.szKrnlDebug, si_data, MAX_PATH);
	}

	m_core.allowAdminPrivilege = m_si.GetBoolValue(section_core, sect_core_keys.AllowAdminPrivilege, /*Default=*/false);

	m_core.LogLevel = m_si.GetLongValue(section_core, sect_core_keys.LogLevel, 1);
	si_list.clear();
	index = 0;
	list_max = std::size(m_core.LoggedModules);
	bRet = m_si.GetAllValues(section_core, sect_core_keys.LoggedModules, si_list);
	if (bRet) {
		si_list_iterator = si_list.begin();
		for (si_list_iterator; si_list_iterator != si_list.end(); si_list_iterator++) {
			// Exit loop when the list has reached the limit.
			if (index == list_max) {
				break;
			}
			if (std::strncmp(si_list_iterator->pItem, "0x", 2) == 0) {
				si_list_iterator->pItem += 2;
			}
			m_core.LoggedModules[index] = std::strtoul(si_list_iterator->pItem, nullptr, 16);
			index++;
		}
	}
	while (index < list_max) {
		m_core.LoggedModules[index] = 0;
		index++;
	}
	m_core.bLogPopupTestCase = m_si.GetBoolValue(section_core, sect_core_keys.LogPopupTestCase, /*Default=*/true);

	m_core.bUseLoaderExec = m_si.GetBoolValue(section_core, sect_core_keys.LoaderExecutable, /*Default=*/true);

	// ==== Core End ============

	// Delete/update legacy configs from previous revisions
	RemoveLegacyConfigs(m_core.Revision);
	m_core.Revision = settings_version;

	// ==== Hack Begin ==========

	m_hacks.DisablePixelShaders = m_si.GetBoolValue(section_hack, sect_hack_keys.DisablePixelShaders, /*Default=*/false);
	m_hacks.UseAllCores = m_si.GetBoolValue(section_hack, sect_hack_keys.UseAllCores, /*Default=*/false);
	m_hacks.SkipRdtscPatching = m_si.GetBoolValue(section_hack, sect_hack_keys.SkipRdtscPatching, /*Default=*/false);

	// ==== Hack End ============

	// ==== Video Begin =========

	// Video - Resolution config
	si_data = m_si.GetValue(section_video, sect_video_keys.VideoResolution, /*Default=*/nullptr);
	// Fallback to null string if value is empty or contain bigger string.
	if (si_data == nullptr || std::strlen(si_data) >= std::size(m_video.szVideoResolution)) {
		m_video.szVideoResolution[0] = '\0';
	}
	else {
		std::strncpy(m_video.szVideoResolution, si_data, std::size(m_video.szVideoResolution));
	}

	m_video.adapter = m_si.GetLongValue(section_video, sect_video_keys.adapter, /*Default=*/0);
	m_video.direct3DDevice = m_si.GetLongValue(section_video, sect_video_keys.Direct3DDevice, /*Default=*/0);
	m_video.bVSync = m_si.GetBoolValue(section_video, sect_video_keys.VSync, /*Default=*/false);
	m_video.bFullScreen = m_si.GetBoolValue(section_video, sect_video_keys.FullScreen, /*Default=*/false);
	m_video.bMaintainAspect = m_si.GetBoolValue(section_video, sect_video_keys.MaintainAspect, /*Default=*/true);
	m_video.renderScaleFactor = m_si.GetLongValue(section_video, sect_video_keys.RenderResolution, /*Default=*/1);

	// ==== Video End ===========

	// ==== Audio Begin =========

	// Audio - Adapter config
	si_data = m_si.GetValue(section_audio, sect_audio_keys.adapter, /*Default=*/nullptr);
	if (si_data == nullptr) {
		// Default to primary audio device
		m_audio.adapterGUID = { 0 };
	}
	else {
		iStatus = std::sscanf(si_data, sect_audio_keys.adapter_value,
		            &m_audio.adapterGUID.Data1, &m_audio.adapterGUID.Data2, &m_audio.adapterGUID.Data3,
		            &m_audio.adapterGUID.Data4[0], &m_audio.adapterGUID.Data4[1], &m_audio.adapterGUID.Data4[2], &m_audio.adapterGUID.Data4[3],
		            &m_audio.adapterGUID.Data4[4], &m_audio.adapterGUID.Data4[5], &m_audio.adapterGUID.Data4[6], &m_audio.adapterGUID.Data4[7]);

		// Fallback to primary audio device if file contain invalid value.
		if (iStatus != 11 /*= total arguments*/) {
			m_audio.adapterGUID = { 0 };
		}
	}

	m_audio.codec_pcm = m_si.GetBoolValue(section_audio, sect_audio_keys.codec_pcm, /*Default=*/true, nullptr);
	m_audio.codec_xadpcm = m_si.GetBoolValue(section_audio, sect_audio_keys.codec_xadpcm, /*Default=*/true, nullptr);
	m_audio.codec_unknown = m_si.GetBoolValue(section_audio, sect_audio_keys.codec_unknown, /*Default=*/true, nullptr);

	m_audio.mute_on_unfocus = m_si.GetBoolValue(section_audio, sect_audio_keys.mute_on_unfocus, /*Default=*/true, nullptr);

	// ==== Audio End ===========

	// ==== Network Begin =======

	si_data = m_si.GetValue(section_network, sect_network_keys.adapter_name, /*Default=*/nullptr);
	// Fallback to null string if value is empty or contains a bigger string.
	if (si_data == nullptr || std::strlen(si_data) >= std::size(m_network.adapter_name)) {
		m_network.adapter_name[0] = '\0';
	} else {
		std::strncpy(m_network.adapter_name, si_data, std::size(m_network.adapter_name));
	}

	// ==== Network End =========

	// ==== Input General Begin ====

	m_input_general.MoAxisRange = m_si.GetLongValue(section_input_general, sect_input_general.mo_axis_range, MO_AXIS_DEFAULT_RANGE);
	m_input_general.MoWheelRange = m_si.GetLongValue(section_input_general, sect_input_general.mo_wheel_range, MO_WHEEL_DEFAULT_RANGE);
	m_input_general.IgnoreKbMoUnfocus = m_si.GetBoolValue(section_input_general, sect_input_general.ignore_kbmo_unfocus, true);

	// ==== Input General End ==============

	// ==== Input Port Begin ====

	for (int port_num = 0; port_num < 4; port_num++) {
		std::string current_section = std::string(section_input_port) + std::to_string(port_num);
		int ret = m_si.GetLongValue(current_section.c_str(), sect_input_port.type, -2);
		if (ret == -2) {
			m_input_port[port_num].Type = to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID);
			continue;
		}
		m_input_port[port_num].Type = ret;
		m_input_port[port_num].DeviceName = m_si.GetValue(current_section.c_str(), sect_input_port.device);
		m_input_port[port_num].ProfileName = TrimQuoteFromString(m_si.GetValue(current_section.c_str(), sect_input_port.config));
	}

	// ==== Input Port End ==============

	// ==== Input Profile Begin ====

	std::array<std::vector<std::string>, to_underlying(XBOX_INPUT_DEVICE::DEVICE_MAX)> control_names;
	for (int device = 0; device < to_underlying(XBOX_INPUT_DEVICE::DEVICE_MAX); device++) {
		if (dev_num_buttons[device] == 0) {
			continue;
		}

		auto &lambda = [&control_names, &device](int num_buttons, const char *const ctrl_names[]) {
			for (int i = 0; i < num_buttons; i++) {
				char control_name[XBOX_BUTTON_NAME_LENGTH];
				std::sprintf(control_name, sect_input_profiles.control, ctrl_names[i]);
				control_names[device].push_back(control_name);
			}
		};

		switch (device)
		{
		case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE):
		case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_S):
		case to_underlying(XBOX_INPUT_DEVICE::ARCADE_STICK):
			lambda(dev_num_buttons[device], button_xbox_ctrl_names);
			break;

		case to_underlying(XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER):
			lambda(dev_num_buttons[device], button_sbc_names);
			break;

		}
	}

	index = 0;
	while (true) {
		std::string current_section = std::string(section_input_profiles) + std::to_string(index);
		if (m_si.GetSectionSize(current_section.c_str()) == -1) {
			break;
		}
		s_input_profiles local_profile;
		local_profile.Type = m_si.GetLongValue(current_section.c_str(), sect_input_profiles.type);
		local_profile.ProfileName = TrimQuoteFromString(m_si.GetValue(current_section.c_str(), sect_input_profiles.config));
		local_profile.DeviceName = m_si.GetValue(current_section.c_str(), sect_input_profiles.device);
		for (int vec_control_index = 0; vec_control_index < dev_num_buttons[local_profile.Type]; vec_control_index++) {
			local_profile.ControlList.push_back(m_si.GetValue(current_section.c_str(),
				control_names[local_profile.Type][vec_control_index].c_str()));
		}
		m_input_profiles[local_profile.Type].push_back(std::move(local_profile));
		index++;
	}

	// ==== Input Profile End ======

	// ==== Overlay Begin =========

	m_overlay.fps = m_si.GetBoolValue(section_overlay, sect_overlay_keys.FPS, false);
	m_overlay.hle_lle_stats = m_si.GetBoolValue(section_overlay, sect_overlay_keys.hle_lle_stats, false);

	// ==== Overlay End ===========

	return true;
}

bool Settings::Save(std::string file_path)
{
	if (m_file_path.empty() && file_path.empty()) {
		return false;
	}

	// Minimal need is 25, 0x37 for GUID.
	char si_value[64];
	std::string quote_str;

	// ==== GUI Begin ===========

	m_si.SetLongValue(section_gui, sect_gui_keys.CxbxDebugMode, m_gui.CxbxDebugMode, nullptr, true, true);
	m_si.SetValue(section_gui, sect_gui_keys.CxbxDebugLogFile, m_gui.szCxbxDebugFile.c_str(), nullptr, true);
	m_si.SetLongValue(section_gui, sect_gui_keys.DataStorageToggle, m_gui.DataStorageToggle, nullptr, true, true);
	m_si.SetValue(section_gui, sect_gui_keys.DataCustomLocation, m_gui.szCustomLocation.c_str(), nullptr, true);

	// First force overwrite existing value
	m_si.SetValue(section_gui, sect_gui_keys.RecentXbeFiles, m_gui.szRecentXbeFiles[0].c_str(), nullptr, true);
	// Then append recent xbe file strings
	for (unsigned int i = 1; i < 10; i++) {
		m_si.SetValue(section_gui, sect_gui_keys.RecentXbeFiles, m_gui.szRecentXbeFiles[i].c_str(), nullptr, false);
	}

	m_si.SetBoolValue(section_gui, sect_gui_keys.IgnoreInvalidXbeSig, m_gui.bIgnoreInvalidXbeSig, nullptr, true);
	m_si.SetBoolValue(section_gui, sect_gui_keys.IgnoreInvalidXbeSec, m_gui.bIgnoreInvalidXbeSec, nullptr, true);

	// ==== GUI End =============

	// ==== Core Begin ==========

	m_si.SetLongValue(section_core, sect_core_keys.Revision, m_core.Revision, nullptr, false, true);
	m_si.SetLongValue(section_core, sect_core_keys.FlagsLLE, m_core.FlagsLLE, nullptr, true, true);
	m_si.SetLongValue(section_core, sect_core_keys.KrnlDebugMode, m_core.KrnlDebugMode, nullptr, true, true);
	m_si.SetValue(section_core, sect_core_keys.KrnlDebugLogFile, m_core.szKrnlDebug, nullptr, true);
	m_si.SetBoolValue(section_core, sect_core_keys.AllowAdminPrivilege, m_core.allowAdminPrivilege, nullptr, true);
	m_si.SetLongValue(section_core, sect_core_keys.LogLevel, m_core.LogLevel, nullptr, false, true);

	std::stringstream stream;
	stream << "0x" << std::hex << m_core.LoggedModules[0];
	m_si.SetValue(section_core, sect_core_keys.LoggedModules, stream.str().c_str(), nullptr, true);
	for (int i = 1; i < NUM_INTEGERS_LOG; i++) {
		stream.str("");
		stream << "0x" << std::hex << m_core.LoggedModules[i];
		m_si.SetValue(section_core, sect_core_keys.LoggedModules, stream.str().c_str(), nullptr, false);
	}
	m_si.SetBoolValue(section_core, sect_core_keys.LogPopupTestCase, m_core.bLogPopupTestCase, nullptr, true);

	m_si.SetBoolValue(section_core, sect_core_keys.LoaderExecutable, m_core.bUseLoaderExec, nullptr, true);

	// ==== Core End ============

	// ==== Video Begin =========

	m_si.SetValue(section_video, sect_video_keys.VideoResolution, m_video.szVideoResolution, nullptr, true);

	m_si.SetLongValue(section_video, sect_video_keys.adapter, m_video.adapter, nullptr, true, true);
	m_si.SetLongValue(section_video, sect_video_keys.Direct3DDevice, m_video.direct3DDevice, nullptr, true, true);
	m_si.SetBoolValue(section_video, sect_video_keys.VSync, m_video.bVSync, nullptr, true);
	m_si.SetBoolValue(section_video, sect_video_keys.FullScreen, m_video.bFullScreen, nullptr, true);
	m_si.SetBoolValue(section_video, sect_video_keys.MaintainAspect, m_video.bMaintainAspect, nullptr, true);
	m_si.SetLongValue(section_video, sect_video_keys.RenderResolution, m_video.renderScaleFactor, nullptr, false, true);

	// ==== Video End ===========

	// ==== Audio Begin =========

	// Audio - Adapter config
	std::sprintf(si_value, sect_audio_keys.adapter_value,
		m_audio.adapterGUID.Data1, m_audio.adapterGUID.Data2, m_audio.adapterGUID.Data3,
		m_audio.adapterGUID.Data4[0], m_audio.adapterGUID.Data4[1], m_audio.adapterGUID.Data4[2], m_audio.adapterGUID.Data4[3],
		m_audio.adapterGUID.Data4[4], m_audio.adapterGUID.Data4[5], m_audio.adapterGUID.Data4[6], m_audio.adapterGUID.Data4[7]);

	m_si.SetValue(section_audio, sect_audio_keys.adapter, si_value, nullptr, true);

	m_si.SetBoolValue(section_audio, sect_audio_keys.codec_pcm, m_audio.codec_pcm, nullptr, true);
	m_si.SetBoolValue(section_audio, sect_audio_keys.codec_xadpcm, m_audio.codec_xadpcm, nullptr, true);
	m_si.SetBoolValue(section_audio, sect_audio_keys.codec_unknown, m_audio.codec_unknown, nullptr, true);
	m_si.SetBoolValue(section_audio, sect_audio_keys.mute_on_unfocus, m_audio.mute_on_unfocus, nullptr, true);

	// ==== Audio End ===========

	// ==== Network Begin =======

	m_si.SetValue(section_network, sect_network_keys.adapter_name, m_network.adapter_name, nullptr, true);
	
	// ==== Network End =========

	// ==== Input General Begin =======

	m_si.SetLongValue(section_input_general, sect_input_general.mo_axis_range, m_input_general.MoAxisRange, nullptr, false, true);
	m_si.SetLongValue(section_input_general, sect_input_general.mo_wheel_range, m_input_general.MoWheelRange, nullptr, false, true);
	m_si.SetBoolValue(section_input_general, sect_input_general.ignore_kbmo_unfocus, m_input_general.IgnoreKbMoUnfocus, nullptr, true);

	// ==== Input General End =========

	// ==== Input Port Begin ====

	for (int port_num = 0; port_num < 4; port_num++) {
		std::string current_section = std::string(section_input_port) + std::to_string(port_num);
		std::string quoted_prf_str = m_input_port[port_num].ProfileName.insert(0, "\"");
		quoted_prf_str += "\"";
		m_si.SetLongValue(current_section.c_str(), sect_input_port.type, m_input_port[port_num].Type, nullptr, false, true);
		m_si.SetValue(current_section.c_str(), sect_input_port.device, m_input_port[port_num].DeviceName.c_str(), nullptr, true);
		m_si.SetValue(current_section.c_str(), sect_input_port.config, quoted_prf_str.c_str(), nullptr, true);
	}

	// ==== Input Port End ==============

	// ==== Input Profile Begin ====

	std::array<std::vector<std::string>, to_underlying(XBOX_INPUT_DEVICE::DEVICE_MAX)> control_names;
	for (int device = 0; device < to_underlying(XBOX_INPUT_DEVICE::DEVICE_MAX); device++) {
		if (dev_num_buttons[device] == 0) {
			continue;
		}

		auto &lambda = [&control_names, &device](int num_buttons, const char *const ctrl_names[]) {
			for (int i = 0; i < num_buttons; i++) {
				char control_name[XBOX_BUTTON_NAME_LENGTH];
				std::sprintf(control_name, sect_input_profiles.control, ctrl_names[i]);
				control_names[device].push_back(control_name);
			}
		};

		switch (device)
		{
		case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE):
		case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_S):
		case to_underlying(XBOX_INPUT_DEVICE::ARCADE_STICK):
			lambda(dev_num_buttons[device], button_xbox_ctrl_names);
			break;

		case to_underlying(XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER):
			lambda(dev_num_buttons[device], button_sbc_names);
			break;

		}
	}

	int profile_num = 0;
	for (int i = 0; i < to_underlying(XBOX_INPUT_DEVICE::DEVICE_MAX); i++) {
		size_t vec_size = m_input_profiles[i].size();
		if (vec_size == 0) {
			continue;
		}
		for (unsigned int vec_index = 0; vec_index < vec_size; vec_index++, profile_num++) {
			std::string current_section = std::string(section_input_profiles) + std::to_string(profile_num);
			std::string quoted_prf_str = m_input_profiles[i][vec_index].ProfileName.insert(0, "\"");
			quoted_prf_str += "\"";
			m_si.SetLongValue(current_section.c_str(), sect_input_profiles.type, m_input_profiles[i][vec_index].Type, nullptr, false, true);
			m_si.SetValue(current_section.c_str(), sect_input_profiles.config, quoted_prf_str.c_str(), nullptr, true);
			m_si.SetValue(current_section.c_str(), sect_input_profiles.device, m_input_profiles[i][vec_index].DeviceName.c_str(), nullptr, true);
			size_t vec_control_size = m_input_profiles[i][vec_index].ControlList.size();
			if (vec_control_size == 0) {
				continue;
			}
			m_si.SetValue(current_section.c_str(), control_names[i][0].c_str(), m_input_profiles[i][vec_index].ControlList[0].c_str(), nullptr, true);
			for (unsigned int vec_control_index = 1; vec_control_index < vec_control_size; vec_control_index++) {
				m_si.SetValue(current_section.c_str(), control_names[i][vec_control_index].c_str(),
					m_input_profiles[i][vec_index].ControlList[vec_control_index].c_str(), nullptr, true);
			}
		}
	}
	while (true) {
		std::string current_section = std::string(section_input_profiles) + std::to_string(profile_num);
		if (m_si.GetSectionSize(current_section.c_str()) == -1) {
			break;
		}
		m_si.Delete(current_section.c_str(), nullptr, true);
		profile_num++;
	}

	// ==== Input Profile End ======

	// ==== Overlay Begin =======

	m_si.SetBoolValue(section_overlay, sect_overlay_keys.FPS, m_overlay.fps, nullptr, true);
	m_si.SetBoolValue(section_overlay, sect_overlay_keys.hle_lle_stats, m_overlay.hle_lle_stats, nullptr, true);

	// ==== Overlay End =========

	// ==== Hack Begin ==========

	m_si.SetBoolValue(section_hack, sect_hack_keys.DisablePixelShaders, m_hacks.DisablePixelShaders, nullptr, true);
	m_si.SetBoolValue(section_hack, sect_hack_keys.UseAllCores, m_hacks.UseAllCores, nullptr, true);
	m_si.SetBoolValue(section_hack, sect_hack_keys.SkipRdtscPatching, m_hacks.SkipRdtscPatching, nullptr, true);

	// ==== Hack End ============

	SI_Error siError;
	if (!file_path.empty()) {
		siError = m_si.SaveFile(file_path.c_str(), true);

		if (siError == SI_OK) {
			m_file_path = file_path;
		}
	}
	else {
		siError = m_si.SaveFile(m_file_path.c_str(), true);
	}

	return (siError == SI_OK);
}

void Settings::Delete()
{
    std::filesystem::remove(m_file_path);
}

// Universal update to EmuShared from both standalone kernel, and GUI process.
void Settings::SyncToEmulator()
{
	// register Core settings
	g_EmuShared->SetCoreSettings(&m_core);
	g_EmuShared->SetIsKrnlLogEnabled(m_core.KrnlDebugMode != DebugMode::DM_NONE);

	// register Video settings
	g_EmuShared->SetVideoSettings(&m_video);
	g_EmuShared->SetOverlaySettings(&m_overlay);

	// register Audio settings
	g_EmuShared->SetAudioSettings(&m_audio);

	// register Network settings
	g_EmuShared->SetNetworkSettings(&m_network);

	// register xbox device input settings
	for (int i = 0; i < 4; i++) {
		g_EmuShared->SetInputDevTypeSettings(&m_input_port[i].Type, i);
		if (m_input_port[i].Type != to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID)) {
			g_EmuShared->SetInputDevNameSettings(m_input_port[i].DeviceName.c_str(), i);
			auto it = std::find_if(m_input_profiles[m_input_port[i].Type].begin(),
				m_input_profiles[m_input_port[i].Type].end(), [this, i](const auto& profile) {
					if (profile.ProfileName == m_input_port[i].ProfileName) {
						return true;
					}
					return false;
				});
			if (it != m_input_profiles[m_input_port[i].Type].end()) {
				char controls_name[HIGHEST_NUM_BUTTONS][HOST_BUTTON_NAME_LENGTH];
				for (int index = 0; index < dev_num_buttons[m_input_port[i].Type]; index++) {
					strncpy(controls_name[index], it->ControlList[index].c_str(), 30);
				}
				g_EmuShared->SetInputBindingsSettings(controls_name, dev_num_buttons[m_input_port[i].Type], i);
			}
		}
	}

	// register Input general settings
	g_EmuShared->SetInputGeneralSettings(&m_input_general);

	// register Hacks settings
	g_EmuShared->SetHackSettings(&m_hacks);

	// register data location setting
	g_EmuShared->SetStorageLocation(GetDataLocation().c_str());

	// reset title mount path
	g_EmuShared->SetTitleMountPath("");
}

void verifyDebugFilePath(DebugMode& debug_mode, std::string& file_path)
{
	// Prevent using an incorrect path from the registry if the debug folders have been moved
	std::string szDebugPath;
	std::string szDebugName;

	if (debug_mode == DM_FILE) {

		if(file_path.size() == 0 || file_path.size() >= MAX_PATH) {
			debug_mode = DM_NONE;
		}
		else {
			szDebugName = file_path.substr(file_path.find_last_of("\\/"), std::string::npos);

			if(file_path.size() < szDebugName.size()) {
				file_path = "";
				debug_mode = DM_NONE;
			}
			else {
				szDebugPath = file_path.substr(0, file_path.size() - szDebugName.size());

				if(std::filesystem::exists(szDebugPath) == false) {
					file_path = "";
					debug_mode = DM_NONE;
				}
			}
		}
	}
}

void Settings::Verify()
{
	std::string szKrnlDebug = m_core.szKrnlDebug; // Temporary placeholder until m_core.szKrnlDebug is replace to std::string.

	verifyDebugFilePath(m_gui.CxbxDebugMode, m_gui.szCxbxDebugFile);

	verifyDebugFilePath(m_core.KrnlDebugMode, szKrnlDebug);

	// Set to null string once if contain invalid path.
	if (m_core.szKrnlDebug[0] != '\0' && szKrnlDebug.size() == 0) {
		std::memset(m_core.szKrnlDebug, 0, MAX_PATH);
	}
}

std::string Settings::GetDataLocation()
{

	// Optimization purpose for not require to re-process when toggle state has not changed.
	if (m_current_DataStorageToggle == m_gui.DataStorageToggle) {
		return m_current_data_location;
	}

	switch (m_gui.DataStorageToggle) {
		default:
#ifdef RETRO_API_VERSION // TODO: Change me to #ifndef QT_VERSION

			m_gui.DataStorageToggle = CXBX_DATA_EXECDIR;

#else // Only support for Qt compile build.

			m_gui.DataStorageToggle = CXBX_DATA_APPDATA;
			// If unknown value, default to CXBX_DATA_APPDATA (below, don't use break)

		case CXBX_DATA_APPDATA:
			m_current_data_location = GenerateUserProfileDirectoryStr();
			break;
#endif

		case CXBX_DATA_EXECDIR:
			m_current_data_location = GenerateExecDirectoryStr();
			break;

		case CXBX_DATA_CUSTOM:
			m_current_data_location = m_gui.szCustomLocation;
			break;
	}
	m_current_DataStorageToggle = m_gui.DataStorageToggle;

	return m_current_data_location;
}

// Detect where settings file is located and return default data mode.
CXBX_DATA Settings::FindSettingsLocation(std::string& file_path_out)
{
	std::string fileSearch = GenerateExecDirectoryStr();
	CXBX_DATA ret = CXBX_DATA_EXECDIR;

	fileSearch.append(szSettings_settings_file);

	// Check and see if file exists from portable, current, directory.
	if (std::filesystem::exists(fileSearch) == false) {

		fileSearch = GenerateUserProfileDirectoryStr();
		if (fileSearch.size() == 0) {
			return CXBX_DATA_INVALID;
		}
		CXBX_DATA ret = CXBX_DATA_APPDATA;
		fileSearch.append(szSettings_settings_file);

		// Check if the user profile directory settings file exists.
		if (std::filesystem::exists(fileSearch) == false) {
			return CXBX_DATA_INVALID;
		}
	}
	file_path_out = fileSearch;

	return ret;
}

// Enter setup installer process
CXBX_DATA Settings::SetupFile(std::string& file_path_out)
{
	std::string setupFile;
	CXBX_DATA data_ret = CXBX_DATA_INVALID;
#ifdef RETRO_API_VERSION // TODO: Change me to #ifndef QT_VERSION
	// Can only have one option without Qt.
	setupFile = GenerateExecDirectoryStr();

#else // Only support for Qt compile build.
	PopupReturn eRet = PopupQuestion(nullptr, szSettings_save_user_option_message);

	if (eRet == PopupReturn::Yes) {
		setupFile = GenerateExecDirectoryStr();
		data_ret = CXBX_DATA_EXECDIR;
	}
	else if (eRet == PopupReturn::No) {
		setupFile = GenerateUserProfileDirectoryStr();
		data_ret = CXBX_DATA_APPDATA;
		if (setupFile.size() != 0) {
			// Check if data directory exists.
			if (!std::filesystem::exists(setupFile)) {
				// Then try create data directory.
				if (!std::filesystem::create_directory(setupFile)) {
					// Unable to create a data directory
					data_ret = CXBX_DATA_INVALID;
				}
			}
		}
	}
#endif

	if (data_ret == CXBX_DATA_INVALID) {
		PopupError(nullptr, szSettings_setup_error);
	}
	else {
		setupFile.append(szSettings_settings_file);
		// Create the file, that's it. Load the default configuration later on;
		std::ofstream createFile(setupFile);
		if (createFile.is_open()) {
			createFile.close();
		}
		file_path_out = setupFile;
	}

	return data_ret;
}

void Settings::RemoveLegacyConfigs(unsigned int CurrentRevision)
{
	if (CurrentRevision < 5) {
		m_si.Delete(section_controller_dinput, nullptr, true);
		m_si.Delete(section_controller_port, nullptr, true);
	}

	if (CurrentRevision == 5) {
		m_si.Delete(section_core, "LoaderExperiment", true);
	}


	if (CurrentRevision < 8) {
		const std::string kb_str = "Keyboard";

		for (unsigned port_num = 0; port_num < 4; ++port_num) {
			std::string current_section = std::string(section_input_port) + std::to_string(port_num);
			std::string device_name = m_si.GetValue(current_section.c_str(), sect_input_port.device, "");

			if (StrEndsWith(device_name, kb_str)) {
				device_name += "Mouse";
				m_si.SetValue(current_section.c_str(), sect_input_port.device, device_name.c_str(), nullptr, true);
			}
		}

		for (unsigned index = 0; ; ++index) {
			std::string current_section = std::string(section_input_profiles) + std::to_string(index);
			if (m_si.GetSectionSize(current_section.c_str()) == -1) {
				break;
			}

			std::string device_name = m_si.GetValue(current_section.c_str(), sect_input_profiles.device, "");

			// NOTE: with C++20, this can be simplified by simply calling device_name.ends_with()
			if (device_name.length() >= kb_str.length()) {
				if (device_name.compare(device_name.length() - kb_str.length(), kb_str.length(), kb_str) == 0) {
					device_name += "Mouse";
					m_si.SetValue(current_section.c_str(), sect_input_profiles.device, device_name.c_str(), nullptr, true);
				}
			}
		}
	}

	if(CurrentRevision < 9) {
		m_si.Delete(section_video, "HardwareYUV", true);
	}
}
