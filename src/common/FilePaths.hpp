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
#pragma once

static void CxbxInitFilePaths()
{
	if (g_Settings) {
		std::string dataLoc = g_Settings->GetDataLocation();
		std::strncpy(szFolder_CxbxReloadedData, dataLoc.c_str(), dataLoc.length() + 1);
	}
	else {
		g_EmuShared->GetDataLocation(szFolder_CxbxReloadedData);
	}

	// Make sure our data folder exists :
	bool result = std::filesystem::exists(szFolder_CxbxReloadedData);
	if (!result && !std::filesystem::create_directory(szFolder_CxbxReloadedData)) {
		CxbxKrnlCleanup("%s : Couldn't create Cxbx-Reloaded's data folder!", __func__);
	}

	// Make sure the EmuDisk folder exists
	std::string emuDisk = std::string(szFolder_CxbxReloadedData) + std::string("\\EmuDisk");
	result = std::filesystem::exists(emuDisk);
	if (!result && !std::filesystem::create_directory(emuDisk)) {
		CxbxKrnlCleanup("%s : Couldn't create Cxbx-Reloaded EmuDisk folder!", __func__);
	}

	// Make sure the EmuDMu folder exists
	std::string emuMu = std::string(szFolder_CxbxReloadedData) + std::string("\\EmuMu");
	result = std::filesystem::exists(emuMu);
	if (!result && !std::filesystem::create_directory(emuMu)) {
		CxbxKrnlCleanup("%s : Couldn't create Cxbx-Reloaded EmuMu folder!", __func__);
	}

	snprintf(szFilePath_EEPROM_bin, MAX_PATH, "%s\\EEPROM.bin", szFolder_CxbxReloadedData);

	GetModuleFileName(GetModuleHandle(nullptr), szFilePath_CxbxReloaded_Exe, MAX_PATH);
}

static HANDLE hMapDataHash = nullptr;

static bool CxbxLockFilePath()
{
	std::stringstream filePathHash("Local\\");
	uint64_t hashValue = XXH3_64bits(szFolder_CxbxReloadedData, strlen(szFolder_CxbxReloadedData) + 1);
	if (!hashValue) {
		CxbxKrnlCleanup("%s : Couldn't generate Cxbx-Reloaded's data folder hash!", __func__);
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

static void CxbxUnlockFilePath()
{
	// Close opened file path lockdown shared memory.
	if (hMapDataHash) {
		CloseHandle(hMapDataHash);
		hMapDataHash = nullptr;
	}
}
