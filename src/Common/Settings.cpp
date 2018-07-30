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
// *  (c) 2018 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************

#include "Settings.hpp"
#include "CxbxKrnl/Emu.h"
#include "CxbxKrnl/EmuShared.h"

#include <filesystem>
#ifdef _EXPERIMENTAL_FILESYSTEM_
namespace std {
	namespace filesystem = std::experimental::filesystem;
}
#endif
// TODO: Implement Qt support when real CPU emulation is available.
#ifndef QT_VERSION // NOTE: Non-Qt will be using current directory for data
#include <ShlObj.h> // For SHGetSpecialFolderPath and CSIDL_APPDATA
#else
static_assert(false, "Please implement support for cross-platform's user profile data.");

#include <QDir> // for create directory
#include <QFile> // for check file existance
#include <QStandardPaths> // for cross-platform's user profile support
#endif

Settings* g_Settings = nullptr;

#define szSettings_setup_error "ERROR: Either setup have a problem or do not have write permission to directory."
#define szSettings_init_error "ERROR: Unable to initialize Settings class."
#define szSettings_save_user_option_message "If you want to save your settings in current/portable directory,\nclick 'Yes'." \
											"\n\nIf you want to store your settings in user profile directory,\nclick 'No'." \
											"\n\nClicking cancel will abort Cxbx-Reloaded."

#define szSettings_settings_file "/settings.ini"
#define szSettings_cxbx_reloaded_directory "/Cxbx-Reloaded"

std::string GenerateCurrentFileDirectoryStr()
{
	// NOTE: There is no cross-platform support for getting file's current directory.
	return std::filesystem::current_path().generic_string();
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

bool Settings::Init()
{
	m_si.SetMultiKey(true);

	bool bRet = LoadUserConfig();

	return bRet;
}

bool Settings::LoadUserConfig()
{
	std::string fileSearch = GenerateCurrentFileDirectoryStr();

	fileSearch.append(szSettings_settings_file);

	// Check and see if file exist from portable, current, directory.
	FILE* fileExist = fopen(fileSearch.c_str(), "r");
	if (fileExist == nullptr) {

		fileSearch = GenerateUserProfileDirectoryStr();
		if (fileSearch.size() == 0) {
			return false;
		}
		fileSearch.append(szSettings_settings_file);

		// Check if user profile directory settings file exist
		fileExist = fopen(fileSearch.c_str(), "r");
		if (fileExist == nullptr) {
			return false;
		}
	}
	fclose(fileExist);

	return LoadFile(fileSearch);
}

bool Settings::LoadFile(std::string file_path)
{
	bool bRet;
	const char* si_data;
	std::list<CSimpleIniA::Entry> si_list;
	std::list<CSimpleIniA::Entry>::iterator si_list_iterator;

	SI_Error siError = m_si.LoadFile(file_path.c_str());

	if (siError != SI_OK) {
		return false;
	}
	m_file_path = file_path;

	return true;
}

bool Settings::Save(std::string file_path)
{
	if (m_file_path.empty() && file_path.empty()) {
		return false;
	}

	// Minimal need is 25, 0x37 for GUID.
	char si_value[64]; 

	SI_Error siError;
	if (!file_path.empty()) {
		siError = m_si.SaveFile(file_path.c_str(), true);
	}
	else {
		siError = m_si.SaveFile(m_file_path.c_str(), true);
	}

	return (siError == SI_OK);
}

