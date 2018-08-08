// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Common->Settings.cpp
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
// *
// *  All rights reserved
// *
// ******************************************************************

#include "Settings.hpp"
#include "CxbxKrnl/Emu.h"
#include "CxbxKrnl/EmuShared.h"
#include <filesystem>
#include <fstream>

// TODO: Implement Qt support when real CPU emulation is available.
#ifndef QT_VERSION // NOTE: Non-Qt will be using current directory for data
#include <ShlObj.h> // For SHGetSpecialFolderPath and CSIDL_APPDATA
#else
static_assert(false, "Please implement support for cross-platform's user profile data.");

#include <QDir> // for create directory
#include <QFile> // for check file existance
#include <QStandardPaths> // for cross-platform's user profile support
#endif

// NOTE: Update settings_version when add/edit/delete setting's structure.
const uint settings_version = 2;

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
} sect_gui_keys;

static const char* section_core = "core";
static struct {
	const char* FlagsLLE = "FlagsLLE";
	const char* KrnlDebugMode = "KrnlDebugMode";
	const char* KrnlDebugLogFile = "KrnlDebugLogFile";
	const char* AllowAdminPrivilege = "AllowAdminPrivilege";
} sect_core_keys;

static const char* section_video = "video";
static struct {
	const char* VideoResolution = "VideoResolution";
	const char* adapter = "adapter";
	const char* Direct3DDevice = "Direct3DDevice";
	const char* VSync = "VSync";
	const char* FullScreen = "FullScreen";
	const char* HardwareYUV = "HardwareYUV";
} sect_video_keys;

static const char* section_audio = "audio";
static struct {
	const char* adapter = "adapter";
	const char* adapter_value = "%08X %04X %04X %02X%02X %02X%02X%02X%02X%02X%02X";
	const char* codec_pcm = "PCM";
	const char* codec_xadpcm = "XADPCM";
	const char* codec_unknown = "UnknownCodec";
} sect_audio_keys;

static const char* section_controller_dinput = "controller-dinput";
// All keys so far are dynamic
static struct {
	const char* device_name = "DeviceName 0x%.02X";
	const char* object_name = "Object : \"%s\"";
	const char* object_name_value = "%08X %08X %08X";
} sect_controller_dinput_keys;

static const char* section_controller_port = "controller-port";
// All keys so far are dynamic
static struct {
	const char* xbox_port_x_host_type = "XboxPort%dHostType";
	const char* xbox_port_x_host_port = "XboxPort%dHostPort";
} sect_controller_port_keys;

static const char* section_hack = "hack";
static struct {
	const char* DisablePixelShaders = "DisablePixelShaders";
	const char* UncapFramerate = "UncapFramerate";
	const char* UseAllCores = "UseAllCores";
	const char* SkipRdtscPatching = "SkipRdtscPatching";
	const char* ScaleViewPort = "ScaleViewPort";
	const char* DirectHostBackBufferAccess = "DirectHostBackBufferAccess";
} sect_hack_keys;

std::string GenerateCurrentDirectoryStr()
{
	// NOTE: There is no cross-platform support for getting file's current directory.
	return std::experimental::filesystem::current_path().generic_string();
}

// NOTE: This function will be only have Qt support, std::experimental::filesystem doesn't have generic support.
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

bool Settings::Init()
{
	m_si.SetMultiKey(true);

	bool bRet = LoadUserConfig();

	// Enter setup installer process
	if (!bRet) {

		std::string saveFile;
#ifdef RETRO_API_VERSION // TODO: Change me to #ifndef QT_VERSION
		// Can only have one option without Qt.
		saveFile = GenerateCurrentDirectoryStr();
		saveFile.append(szSettings_settings_file);

#else // Only support for Qt compile build.
		int iRet = MessageBox(nullptr, szSettings_save_user_option_message, "Cxbx-Reloaded", MB_YESNOCANCEL | MB_ICONQUESTION);

		if (iRet == IDYES) {
			saveFile = GenerateCurrentDirectoryStr();
		}
		else if (iRet == IDNO){
			saveFile = GenerateUserProfileDirectoryStr();
			if (saveFile.size() == 0) {
				return false;
			}

			// Check if data directory exist.
			bRet = std::experimental::filesystem::exists(saveFile);
			if (!bRet) {
				// Then try create data directory.
				bRet = std::experimental::filesystem::create_directory(saveFile);
				if (!bRet) {
					// Unable to create a data directory
					return false;
				}
			}
		}
		else {
			return false;
		}
#endif
		saveFile.append(szSettings_settings_file);

		// If the config file does not exists, create a blank one
		// We can't call Save here because that overrides default values with false!
		if (!std::experimental::filesystem::exists(saveFile)) {
			std::ofstream ofs(saveFile, std::ofstream::out);
			ofs << "\n";
			ofs.close();			
		}

		// Call LoadUserConfig, this will load the config, applying defaults for any missing fields
		bRet = LoadUserConfig();

		if (!bRet) {
			MessageBox(nullptr, szSettings_setup_error, "Cxbx-Reloaded", MB_OK);
			return false;
		}

		bRet = Save(saveFile);
	}
	return bRet;
}

bool Settings::LoadUserConfig()
{
	std::string fileSearch = GenerateCurrentDirectoryStr();

	fileSearch.append(szSettings_settings_file);

	// Check and see if file exist from portable, current, directory.
	if (std::experimental::filesystem::exists(fileSearch) == false) {

		fileSearch = GenerateUserProfileDirectoryStr();
		if (fileSearch.size() == 0) {
			return false;
		}
		fileSearch.append(szSettings_settings_file);

		// Check if user profile directory settings file exist
		if (std::experimental::filesystem::exists(fileSearch) == false) {
			return false;
		}
	}

	return LoadFile(fileSearch);
}

bool Settings::LoadFile(std::string file_path)
{
	bool bRet;
	const char* si_data;
	int iStatus;
	std::list<CSimpleIniA::Entry> si_list;
	std::list<CSimpleIniA::Entry>::iterator si_list_iterator;

	SI_Error siError = m_si.LoadFile(file_path.c_str());

	if (siError != SI_OK) {
		return false;
	}
	m_file_path = file_path;

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

	m_gui.DataStorageToggle = m_si.GetLongValue(section_gui, sect_gui_keys.DataStorageToggle, /*Default=*/CXBX_DATA_APPDATA);
	si_data = m_si.GetValue(section_gui, sect_gui_keys.DataCustomLocation, /*Default=*/nullptr);
	// Fallback to null string if value is empty or contain bigger string.
	if (si_data == nullptr || std::strlen(si_data) >= MAX_PATH) {
		m_gui.szCustomLocation = "";
	}
	else {
		m_gui.szCustomLocation = si_data;
	}

	// GUI - Recent xbe file paths
	uint index = 0;
	uint list_max = std::size(m_gui.szRecentXbeFiles);
	bRet = m_si.GetAllValues(section_gui, sect_gui_keys.RecentXbeFiles, si_list);/*Default=empty list*/
	if (bRet) {
		si_list_iterator = si_list.begin();
		for (si_list_iterator; si_list_iterator != si_list.end(); si_list_iterator++) {
			// Exit loop when list has reach the limit.
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

	// ==== GUI End =============

	// ==== Core Begin ==========

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

	// ==== Core End ============

	// ==== Hack Begin ==========

	m_hacks.DisablePixelShaders = m_si.GetBoolValue(section_hack, sect_hack_keys.DisablePixelShaders, /*Default=*/false);
	m_hacks.UncapFramerate = m_si.GetBoolValue(section_hack, sect_hack_keys.UncapFramerate, /*Default=*/false);
	m_hacks.UseAllCores = m_si.GetBoolValue(section_hack, sect_hack_keys.UseAllCores, /*Default=*/false);
	m_hacks.SkipRdtscPatching = m_si.GetBoolValue(section_hack, sect_hack_keys.SkipRdtscPatching, /*Default=*/false);
	m_hacks.ScaleViewport = m_si.GetBoolValue(section_hack, sect_hack_keys.ScaleViewPort, /*Default=*/false);
	m_hacks.DirectHostBackBufferAccess = m_si.GetBoolValue(section_hack, sect_hack_keys.DirectHostBackBufferAccess, /*Default=*/true);

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
	m_video.bHardwareYUV = m_si.GetBoolValue(section_video, sect_video_keys.HardwareYUV, /*Default=*/false);

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

	// ==== Audio End ===========

	// ==== Controller Begin ====

	int v = 0;
	char szKeyName[64];

	// ******************************************************************
	// * Load Device Names
	// ******************************************************************
	for (v = 0; v < XBCTRL_MAX_DEVICES; v++) {
		std::sprintf(szKeyName, sect_controller_dinput_keys.device_name, v);
		si_data = m_si.GetValue(section_controller_dinput, szKeyName, /*Default=*/nullptr);

		// Fallback to null string if value is empty or contain bigger string.
		if (si_data == nullptr || std::strlen(si_data) >= MAX_PATH) {
			// default is a null string
			m_controller_dinput.DeviceName[v][0] = '\0';
		}
		else {
			std::strncpy(m_controller_dinput.DeviceName[v], si_data, MAX_PATH);
		}
	}

	// ******************************************************************
	// * Load Object Configuration
	// ******************************************************************
	for (v = 0; v<XBCTRL_OBJECT_COUNT; v++) {
		std::sprintf(szKeyName, sect_controller_dinput_keys.object_name, m_controller_dinput.XboxControllerObjectNameLookup[v]);
		si_data = m_si.GetValue(section_controller_dinput, szKeyName, /*Default=*/nullptr);

		if (si_data == nullptr) {
			// default object configuration
			m_controller_dinput.ObjectConfig[v].dwDevice = -1;
			m_controller_dinput.ObjectConfig[v].dwInfo = -1;
			m_controller_dinput.ObjectConfig[v].dwFlags = 0;
		}
		else {
			iStatus = std::sscanf(si_data, sect_controller_dinput_keys.object_name_value, &m_controller_dinput.ObjectConfig[v].dwDevice,
			            &m_controller_dinput.ObjectConfig[v].dwInfo, &m_controller_dinput.ObjectConfig[v].dwFlags);

			// Fallback to default object configuration if file contain invalid value.
			if (iStatus != 3 /*= total arguments*/) {
				m_controller_dinput.ObjectConfig[v].dwDevice = -1;
				m_controller_dinput.ObjectConfig[v].dwInfo = -1;
				m_controller_dinput.ObjectConfig[v].dwFlags = 0;
			}
		}
	}

	for (v = 0; v < XBCTRL_MAX_GAMEPAD_PORTS; v++) {
		std::sprintf(szKeyName, sect_controller_port_keys.xbox_port_x_host_type, v);
		m_controller_port.XboxPortMapHostType[v] = m_si.GetLongValue(section_controller_port, szKeyName, /*Default=*/1, nullptr);
	}

	for (v = 0; v < XBCTRL_MAX_GAMEPAD_PORTS; v++) {
		std::sprintf(szKeyName, sect_controller_port_keys.xbox_port_x_host_port, v);
		m_controller_port.XboxPortMapHostPort[v] = m_si.GetLongValue(section_controller_port, szKeyName, /*Default=*/v, nullptr);
	}

	// ==== Controller End ======

	return true;
}

bool Settings::Save(std::string file_path)
{
	if (m_file_path.empty() && file_path.empty()) {
		return false;
	}

	// Minimal need is 25, 0x37 for GUID.
	char si_value[64]; 

	// ==== GUI Begin ===========

	m_si.SetLongValue(section_gui, sect_gui_keys.CxbxDebugMode, m_gui.CxbxDebugMode, nullptr, true, true);
	m_si.SetValue(section_gui, sect_gui_keys.CxbxDebugLogFile, m_gui.szCxbxDebugFile.c_str(), nullptr, true);
	m_si.SetLongValue(section_gui, sect_gui_keys.DataStorageToggle, m_gui.DataStorageToggle, nullptr, true, true);
	m_si.SetValue(section_gui, sect_gui_keys.DataCustomLocation, m_gui.szCustomLocation.c_str(), nullptr, true);

	// First force overwrite existing value
	m_si.SetValue(section_gui, sect_gui_keys.RecentXbeFiles, m_gui.szRecentXbeFiles[0].c_str(), nullptr, true);
	// Then append recent xbe file strings
	for (uint i = 1; i < 10; i++) {
		m_si.SetValue(section_gui, sect_gui_keys.RecentXbeFiles, m_gui.szRecentXbeFiles[i].c_str(), nullptr, false);
	}

	// ==== GUI End =============

	// ==== Core Begin ==========

	m_si.SetLongValue(section_core, sect_core_keys.FlagsLLE, m_core.FlagsLLE, nullptr, true, true);
	m_si.SetLongValue(section_core, sect_core_keys.KrnlDebugMode, m_core.KrnlDebugMode, nullptr, true, true);
	m_si.SetValue(section_core, sect_core_keys.KrnlDebugLogFile, m_core.szKrnlDebug, nullptr, true);
	m_si.SetBoolValue(section_core, sect_core_keys.AllowAdminPrivilege, m_core.allowAdminPrivilege, nullptr, true);

	// ==== Core End ============

	// ==== Video Begin =========

	m_si.SetValue(section_video, sect_video_keys.VideoResolution, m_video.szVideoResolution, nullptr, true);

	m_si.SetLongValue(section_video, sect_video_keys.adapter, m_video.adapter, nullptr, true, true);
	m_si.SetLongValue(section_video, sect_video_keys.Direct3DDevice, m_video.direct3DDevice, nullptr, true, true);
	m_si.SetBoolValue(section_video, sect_video_keys.VSync, m_video.bVSync, nullptr, true);
	m_si.SetBoolValue(section_video, sect_video_keys.FullScreen, m_video.bFullScreen, nullptr, true);
	m_si.SetBoolValue(section_video, sect_video_keys.HardwareYUV, m_video.bHardwareYUV, nullptr, true);

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

	// ==== Audio End ===========

	// ==== Controller Begin ====

	int v = 0;
	char szKeyName[64];

	// ******************************************************************
	// * Save Device Names
	// ******************************************************************
	for (v = 0; v < XBCTRL_MAX_DEVICES; v++) {
		std::sprintf(szKeyName, sect_controller_dinput_keys.device_name, v);

		if (m_controller_dinput.DeviceName[v][0] == 0) {
			m_si.Delete(section_controller_dinput, szKeyName, true);
		}
		else {
			m_si.SetValue(section_controller_dinput, szKeyName, m_controller_dinput.DeviceName[v], nullptr, true);
		}
	}

	// ******************************************************************
	// * Save Object Configuration
	// ******************************************************************
	for (v = 0; v<XBCTRL_OBJECT_COUNT; v++) {
		std::sprintf(szKeyName, sect_controller_dinput_keys.object_name, m_controller_dinput.XboxControllerObjectNameLookup[v]);

		if (m_controller_dinput.ObjectConfig[v].dwDevice == -1) {
			m_si.Delete(section_controller_dinput, szKeyName, true);
		}
		else {
			std::sprintf(si_value, sect_controller_dinput_keys.object_name_value, m_controller_dinput.ObjectConfig[v].dwDevice,
				m_controller_dinput.ObjectConfig[v].dwInfo, m_controller_dinput.ObjectConfig[v].dwFlags);
			m_si.SetValue(section_controller_dinput, szKeyName, si_value, nullptr, true);
		}
	}

	for (v = 0; v < XBCTRL_MAX_GAMEPAD_PORTS; v++) {
		std::sprintf(szKeyName, sect_controller_port_keys.xbox_port_x_host_type, v);
		m_si.SetLongValue(section_controller_port, szKeyName, m_controller_port.XboxPortMapHostType[v], nullptr, true, true);
	}

	for (v = 0; v < XBCTRL_MAX_GAMEPAD_PORTS; v++) {
		std::sprintf(szKeyName, sect_controller_port_keys.xbox_port_x_host_port, v);
		m_si.SetLongValue(section_controller_port, szKeyName, m_controller_port.XboxPortMapHostPort[v], nullptr, true, true);
	}

	// ==== Controller End ======

	// ==== Hack Begin ==========

	m_si.SetBoolValue(section_hack, sect_hack_keys.DisablePixelShaders, m_hacks.DisablePixelShaders, nullptr, true);
	m_si.SetBoolValue(section_hack, sect_hack_keys.UncapFramerate, m_hacks.UncapFramerate, nullptr, true);
	m_si.SetBoolValue(section_hack, sect_hack_keys.UseAllCores, m_hacks.UseAllCores, nullptr, true);
	m_si.SetBoolValue(section_hack, sect_hack_keys.SkipRdtscPatching, m_hacks.SkipRdtscPatching, nullptr, true);
	m_si.SetBoolValue(section_hack, sect_hack_keys.ScaleViewPort, m_hacks.ScaleViewport, nullptr, true);
	m_si.SetBoolValue(section_hack, sect_hack_keys.DirectHostBackBufferAccess, m_hacks.DirectHostBackBufferAccess, nullptr, true);

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
    std::experimental::filesystem::remove(m_file_path);
}

// Universal update to EmuShared from both standalone kernel, and GUI process.
void Settings::SyncToEmulator()
{
	// register Core settings
	g_EmuShared->SetCoreSettings(&m_core);

	// register Video settings
	g_EmuShared->SetVideoSettings(&m_video);

	// register Audio settings
	g_EmuShared->SetAudioSettings(&m_audio);

	// register Controller settings
	g_EmuShared->SetControllerDInputSettings(&m_controller_dinput);
	g_EmuShared->SetControllerPortSettings(&m_controller_port);

	// register Hacks settings
	g_EmuShared->SetHackSettings(&m_hacks);

	// register data location setting
	g_EmuShared->SetStorageLocation(GetDataLocation().c_str());
}

void Settings::Verify()
{
	// Prevent using an incorrect path from the registry if the debug folders have been moved
	char szDebugPath[MAX_PATH];
	char szDebugName[MAX_PATH];

	if (m_gui.CxbxDebugMode == DM_FILE) {

		if(m_gui.szCxbxDebugFile.size() == 0) {
			m_gui.CxbxDebugMode = DM_NONE;
		}
		else {
			std::strcpy(szDebugName, strrchr(m_gui.szCxbxDebugFile.c_str(), '\\'));

			if(m_gui.szCxbxDebugFile.size() < std::strlen(szDebugName)) {
				m_gui.szCxbxDebugFile = "";
				m_gui.CxbxDebugMode = DM_NONE;
			}
			else {
				std::strncpy(szDebugPath, m_gui.szCxbxDebugFile.c_str(), m_gui.szCxbxDebugFile.size() - std::strlen(szDebugName));

				if(std::experimental::filesystem::exists(szDebugPath) == false) {
					m_gui.szCxbxDebugFile = "";
					m_gui.CxbxDebugMode = DM_NONE;
				}
			}
		}
	}

	if (m_core.KrnlDebugMode == DM_FILE) {

		if(std::strlen(m_core.szKrnlDebug) == 0) {
			m_core.KrnlDebugMode = DM_NONE;
		}
		else {
			std::strcpy(szDebugName, strrchr(m_core.szKrnlDebug, '\\'));

			if(std::strlen(m_core.szKrnlDebug) < std::strlen(szDebugName)) {
				memset(m_core.szKrnlDebug, '\0', MAX_PATH);
				m_core.KrnlDebugMode = DM_NONE;
			}
			else {
				std::strncpy(szDebugPath, m_core.szKrnlDebug, std::strlen(m_core.szKrnlDebug) - std::strlen(szDebugName));

				if(std::experimental::filesystem::exists(szDebugPath) == false) {
					memset(m_core.szKrnlDebug, '\0', MAX_PATH);
					m_core.KrnlDebugMode = DM_NONE;
				}
			}
		}
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

			m_gui.DataStorageToggle = CXBX_DATA_CURDIR;

#else // Only support for Qt compile build.

			m_gui.DataStorageToggle = CXBX_DATA_APPDATA;
			// If unknown value, default to CXBX_DATA_APPDATA (below, don't use break)

		case CXBX_DATA_APPDATA:
			m_current_data_location = GenerateUserProfileDirectoryStr();
			break;
#endif

		case CXBX_DATA_CURDIR:
			m_current_data_location = GenerateCurrentDirectoryStr();
			break;

		case CXBX_DATA_CUSTOM:
			m_current_data_location = m_gui.szCustomLocation;
			break;
	}
	m_current_DataStorageToggle = m_gui.DataStorageToggle;

	return m_current_data_location;
}

// ******************************************************************
// * Input Device Name Lookup Table
// ******************************************************************
const char *Settings::s_controller_dinput::XboxControllerObjectNameLookup[XBCTRL_OBJECT_COUNT] =
{
	// ******************************************************************
	// * Analog Axis
	// ******************************************************************
	"LThumbPosX", "LThumbNegX", "LThumbPosY", "LThumbNegY",
	"RThumbPosX", "RThumbNegX", "RThumbPosY", "RThumbNegY",

	// ******************************************************************
	// * Analog Buttons
	// ******************************************************************
	"A", "B", "X", "Y", "Black", "White", "LTrigger", "RTrigger",

	// ******************************************************************
	// * Digital Buttons
	// ******************************************************************
	"DPadUp", "DPadDown", "DPadLeft", "DPadRight",
	"Back", "Start", "LThumb", "RThumb"
};
