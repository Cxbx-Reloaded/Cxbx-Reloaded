// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *  This file is part of the Cxbx-Reloaded project.
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
// *  All rights reserved
// *
// ******************************************************************
#define LOG_PREFIX CXBXR_MODULE::FILE
#define LOG_PREFIX_INIT CXBXR_MODULE::INIT

#include <filesystem>
#include "common/cxbxr.hpp"
#include "Settings.hpp"
#include "EmuShared.h"
#include "xxhash.h" // for XXH3_64bits
#include "core/kernel/common/xbox.h"
#include "Logging.h"

char szFilePath_CxbxReloaded_Exe[MAX_PATH] = { 0 };
char szFilePath_EEPROM_bin[MAX_PATH] = { 0 };

std::string g_DataFilePath;
std::string g_DiskBasePath;
std::string g_MediaBoardBasePath;
std::string g_MuBasePath;

//TODO: Possible move CxbxResolveHostToFullPath inline function someplace else if become useful elsewhere.
// Let filesystem library clean it up for us, including resolve host's symbolic link path.
// Since internal kernel do translate to full path than preserved host symoblic link path.
void CxbxResolveHostToFullPath(std::filesystem::path& file_path, std::string_view finish_error_sentence) {
	std::error_code error;
	std::filesystem::path sanityPath = std::filesystem::canonical(file_path, error);
	if (error) {

		// The MS implementation of std::filesystem::canonical internally calls GetFinalPathNameByHandleW, which fails with ERROR_FILE_NOT_FOUND when called
		// on a file inside a mounted xiso under Windows with the xbox-iso-vfs tool because of this known dokany bug https://github.com/dokan-dev/dokany/issues/343
		EmuLogInit(LOG_LEVEL::WARNING, "Could not resolve to %s: %s, dokany in use? The error was: %s",
			finish_error_sentence.data(), file_path.string().c_str(), error.message().c_str());

		sanityPath = std::filesystem::absolute(std::filesystem::weakly_canonical(file_path, error), error);
		if (error) {
			CxbxrAbortEx(LOG_PREFIX_INIT, "Could not resolve to %s: %s. The error was: %s", finish_error_sentence.data(), file_path.string().c_str(), error.message().c_str());
		}
	}
	file_path = sanityPath;
}
// TODO: Eventually, we should remove this function to start using std::filesystem::path method for all host paths.
void CxbxResolveHostToFullPath(std::string& file_path, std::string_view finish_error_sentence) {
	std::filesystem::path sanityPath(file_path);
	CxbxResolveHostToFullPath(sanityPath, finish_error_sentence);
	file_path = sanityPath.string();
}

// NOTE: Do NOT modify g_<custom>BasePath variables after this call!
void CxbxrInitFilePaths()
{
	if (g_Settings) {
		g_DataFilePath = g_Settings->GetDataLocation();
	}
	else {
		char dataLoc[MAX_PATH];
		g_EmuShared->GetDataLocation(dataLoc);
		g_DataFilePath = dataLoc;
	}

	// Make sure our data folder exists :
	bool result = std::filesystem::exists(g_DataFilePath);
	if (!result && !std::filesystem::create_directory(g_DataFilePath)) {
		CxbxrAbort("%s : Couldn't create Cxbx-Reloaded's data folder!", __func__);
	}

	// Make sure the EmuDisk folder exists
	g_DiskBasePath = g_DataFilePath + "\\EmuDisk";
	result = std::filesystem::exists(g_DiskBasePath);
	if (!result && !std::filesystem::create_directory(g_DiskBasePath)) {
		CxbxrAbort("%s : Couldn't create Cxbx-Reloaded EmuDisk folder!", __func__);
	}
	CxbxResolveHostToFullPath(g_DiskBasePath, "Cxbx-Reloaded's EmuDisk directory");
	g_DiskBasePath = std::filesystem::path(g_DiskBasePath).append("").string();

	// Make sure the EmuDMu folder exists
	g_MuBasePath = g_DataFilePath + "\\EmuMu";
	result = std::filesystem::exists(g_MuBasePath);
	if (!result && !std::filesystem::create_directory(g_MuBasePath)) {
		CxbxrAbort("%s : Couldn't create Cxbx-Reloaded EmuMu folder!", __func__);
	}
	CxbxResolveHostToFullPath(g_MuBasePath, "Cxbx-Reloaded's EmuMu directory");
	g_MuBasePath = std::filesystem::path(g_MuBasePath).append("").string();

	snprintf(szFilePath_EEPROM_bin, MAX_PATH, "%s\\EEPROM.bin", g_DataFilePath.c_str());

	// Make sure the EmuMediaBoard folder exists
	g_MediaBoardBasePath = g_DataFilePath + "\\EmuMediaBoard";
	result = std::filesystem::exists(g_MediaBoardBasePath);
	if (!result && !std::filesystem::create_directory(g_MediaBoardBasePath)) {
		CxbxrAbort("%s : Couldn't create Cxbx-Reloaded EmuMediaBoard folder!", __func__);
	}
	CxbxResolveHostToFullPath(g_MediaBoardBasePath, "Cxbx-Reloaded's EmuMediaBoard directory");
	g_MediaBoardBasePath = std::filesystem::path(g_MediaBoardBasePath).append("").string();

	GetModuleFileName(GetModuleHandle(nullptr), szFilePath_CxbxReloaded_Exe, MAX_PATH);
}

// Loads a keys.bin file as generated by dump-xbox
// See https://github.com/JayFoxRox/xqemu-tools/blob/master/dump-xbox.c
void LoadXboxKeys()
{
	std::string keys_path = g_DataFilePath + "\\keys.bin";

	// Attempt to open Keys.bin
	FILE* fp = fopen(keys_path.c_str(), "rb");

	if (fp != nullptr) {
		// Determine size of Keys.bin
		xbox::XBOX_KEY_DATA keys[2];
		fseek(fp, 0, SEEK_END);
		long size = ftell(fp);
		rewind(fp);

		// If the size of Keys.bin is correct (two keys), read it
		if (size == xbox::XBOX_KEY_LENGTH * 2) {
			fread(keys, xbox::XBOX_KEY_LENGTH, 2, fp);

			memcpy(xbox::XboxEEPROMKey, &keys[0], xbox::XBOX_KEY_LENGTH);
			memcpy(xbox::XboxCertificateKey, &keys[1], xbox::XBOX_KEY_LENGTH);
		}
		else {
			EmuLog(LOG_LEVEL::WARNING, "Keys.bin has an incorrect filesize. Should be %d bytes", xbox::XBOX_KEY_LENGTH * 2);
		}

		fclose(fp);
		return;
	}

	// If we didn't already exit the function, keys.bin could not be loaded
	EmuLog(LOG_LEVEL::WARNING, "Failed to load Keys.bin. Cxbx-Reloaded will be unable to read Save Data from a real Xbox");
}

static HANDLE hMapDataHash = nullptr;

bool CxbxrLockFilePath()
{
	std::stringstream filePathHash("Local\\");
	uint64_t hashValue = XXH3_64bits(g_DataFilePath.c_str(), g_DataFilePath.length() + 1);
	if (!hashValue) {
		CxbxrAbort("%s : Couldn't generate Cxbx-Reloaded's data folder hash!", __func__);
	}

	filePathHash << std::hex << hashValue;

	hMapDataHash = CreateFileMapping(
		INVALID_HANDLE_VALUE,      // Paging file
		nullptr,                   // default security attributes
		PAGE_READONLY,             // readonly access
		0,                         // size: high 32 bits
		/*Dummy size*/4,           // size: low 32 bits
		filePathHash.str().c_str() // name of map object
	);

	if (hMapDataHash == nullptr) {
		return false;
	}

	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		PopupError(nullptr, "Data path directory is currently in use.\nUse a different data path directory or stop emulation from another process.");
		CloseHandle(hMapDataHash);
		return false;
	}

	return true;
}

void CxbxrUnlockFilePath()
{
	// Close opened file path lockdown shared memory.
	if (hMapDataHash) {
		CloseHandle(hMapDataHash);
		hMapDataHash = nullptr;
	}
}
