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
// *   Common->EmuEEPROM.cpp
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
// *  (c) 2017 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

// prevent name collisions
namespace xboxkrnl
{
	#include <xboxkrnl/xboxkrnl.h> // For XC_VALUE_INDEX and XBOX_EEPROM
};

#include <stdio.h> // For printf
#include <shlobj.h> // For HANDLE, CreateFile, CreateFileMapping, MapViewOfFile

#include "Cxbx.h" // For DbgPrintf
#include "EmuEEPROM.h" // For EEPROMInfo, EEPROMInfos

xboxkrnl::XBOX_EEPROM *EEPROM = nullptr; // Set using CxbxRestoreEEPROM()

xboxkrnl::ULONG XboxFactoryGameRegion = 1; // = North America

const EEPROMInfo* EmuFindEEPROMInfo(xboxkrnl::XC_VALUE_INDEX index)
{
	for (int i = 0; EEPROMInfos[i].index != XC_END_MARKER; i++)
		if (EEPROMInfos[i].index == index)
			return &EEPROMInfos[i];

	return nullptr;
}

xboxkrnl::XBOX_EEPROM *CxbxRestoreEEPROM(char *szFilePath_EEPROM_bin)
{
	xboxkrnl::XBOX_EEPROM *pEEPROM;

	// First, try to open an existing EEPROM.bin file :
	HANDLE hFileEEPROM = CreateFile(szFilePath_EEPROM_bin,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		/* lpSecurityAttributes */nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, // FILE_FLAG_WRITE_THROUGH
		/* hTemplateFile */nullptr);

	bool NeedsInitialization = (hFileEEPROM == INVALID_HANDLE_VALUE);
	if (NeedsInitialization)
	{
		// If the EEPROM.bin file doesn't exist yet, create it :
		hFileEEPROM = CreateFile(szFilePath_EEPROM_bin,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			/* lpSecurityAttributes */nullptr,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, // FILE_FLAG_WRITE_THROUGH
			/* hTemplateFile */nullptr);
		if (hFileEEPROM == INVALID_HANDLE_VALUE)
		{
			DbgPrintf("INIT: Couldn't create EEPROM.bin file!\n");
			return nullptr;
		}
	}

	// TODO : Make sure EEPROM.bin is at least 256 bytes in size - FileSeek(hFileEEPROM, EEPROM_SIZE, soFromBeginning);

	HANDLE hFileMappingEEPROM = CreateFileMapping(
		hFileEEPROM,
		/* lpFileMappingAttributes */nullptr,
		PAGE_READWRITE,
		/* dwMaximumSizeHigh */0,
		/* dwMaximumSizeLow */EEPROM_SIZE,
		/**/nullptr);
	if (hFileMappingEEPROM == NULL)
	{
		DbgPrintf("INIT: Couldn't create EEPROM.bin file mapping!\n");
		return nullptr;
	}

	// Map EEPROM.bin contents into memory :
	pEEPROM = (xboxkrnl::XBOX_EEPROM *)MapViewOfFile(
		hFileMappingEEPROM,
		FILE_MAP_READ | FILE_MAP_WRITE,
		/* dwFileOffsetHigh */0,
		/* dwFileOffsetLow */0,
		EEPROM_SIZE);
	if (pEEPROM == nullptr) {
		DbgPrintf("INIT: Couldn't map EEPROM.bin into memory!\n");
		return nullptr;
	}

	// TODO : Verify checksums

	// Check for (and fix) invalid fields that were set by previous versions of Cxbx-Reloaded
	// Without this, all users would have to delete their EEPROM.bin
	// The issue was that the AV_FLAG_XXhz was set in the wrong field, we fix it by
	// resetting FactorySettings.AVRegion and setting user video flags to the default
	// The user can then set their desired settings using the Xbox Dashboard
	if (pEEPROM->FactorySettings.AVRegion == AV_STANDARD_NTSC_M) {
		DbgPrintf("INIT: Repairing bad EEPROM (from previous Cxbx-Reloaded builds)\n");
		pEEPROM->UserSettings.VideoFlags = 0;
		pEEPROM->FactorySettings.AVRegion = AV_STANDARD_NTSC_M | AV_FLAGS_60Hz;
	}

	if (NeedsInitialization)
	{
		memset(pEEPROM, 0, EEPROM_SIZE);

		// TODO: Make these configurable or autodetect of some sort :
		pEEPROM->UserSettings.Language = 0x01;  // = English
		pEEPROM->UserSettings.VideoFlags = 0;   // = Use XDK defaults
		pEEPROM->UserSettings.AudioFlags = 0;   // = Stereo, no AC3, no DTS
		pEEPROM->UserSettings.ParentalControlGames = 0; // = XC_PC_ESRB_ALL
		pEEPROM->UserSettings.ParentalControlMovies = 0; // = XC_PC_ESRB_ALL
		pEEPROM->UserSettings.MiscFlags = 0;  // No automatic power down
		pEEPROM->FactorySettings.AVRegion = AV_STANDARD_NTSC_M | AV_FLAGS_60Hz;

		XboxFactoryGameRegion = 1; // = North America - TODO : This should be derived from EncryptedSection somehow

		DbgPrintf("INIT: Initialized default EEPROM\n");
	}
	else
		DbgPrintf("INIT: Loaded EEPROM.bin\n");

	return pEEPROM;
}
