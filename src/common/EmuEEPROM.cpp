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
// *  (c) 2017 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#define _XBOXKRNL_DEFEXTRN_
#define LOG_PREFIX CXBXR_MODULE::EEPR
#define LOG_PREFIX_INIT CXBXR_MODULE::INIT

// prevent name collisions
namespace xboxkrnl
{
	#include <xboxkrnl/xboxkrnl.h> // For XC_VALUE_INDEX and XBOX_EEPROM
};

#include <stdio.h> // For printf
#include <shlobj.h> // For HANDLE, CreateFile, CreateFileMapping, MapViewOfFile

#include "Cxbx.h" // For DBG_PRINTF_EX
#include "EmuEEPROM.h" // For EEPROMInfo, EEPROMInfos
#include "core\kernel\support\Emu.h" // For EmuWarning
#include "..\..\src\devices\LED.h" // For SetLEDSequence
#include "..\core\kernel\init\CxbxKrnl.h"

xboxkrnl::XBOX_EEPROM *EEPROM = nullptr; // Set using CxbxRestoreEEPROM()

// Default value (NA), overwritten with the actual content in the eeprom by CxbxRestoreEEPROM
xboxkrnl::ULONG XboxFactoryGameRegion = XC_GAME_REGION_NA;

const EEPROMInfo* EmuFindEEPROMInfo(xboxkrnl::XC_VALUE_INDEX index)
{
	for (int i = 0; EEPROMInfos[i].index != XC_END_MARKER; i++)
		if (EEPROMInfos[i].index == index)
			return &EEPROMInfos[i];

	return nullptr;
}

// From http://xboxdevwiki.net/EEPROM
static void EepromCRC(unsigned char *crc, unsigned char *data, long dataLen) {
    unsigned char* CRC_Data = (unsigned char *)malloc(dataLen+4);
    int pos=0;
    memset(crc,0x00,4);
                                 
    memset(CRC_Data,0x00, dataLen+4);
    //Circle shift input data one byte right
    memcpy(CRC_Data + 0x01 , data, dataLen-1);
    memcpy(CRC_Data, data + dataLen-1, 0x01);

    for (pos=0; pos<4; ++pos) {
        unsigned short CRCPosVal = 0xFFFF;
        unsigned long l;
        for (l=pos; l<(unsigned long)dataLen; l+=4) {
            CRCPosVal -= *(unsigned short*)(&CRC_Data[l]);
        }
        CRCPosVal &= 0xFF00;
        crc[pos] = (unsigned char) (CRCPosVal >> 8);
    }
    free(CRC_Data);
}

void gen_section_CRCs(xboxkrnl::XBOX_EEPROM* eeprom) {
    const long Factory_size = sizeof(eeprom->FactorySettings) - sizeof(eeprom->FactorySettings.Checksum);
    const long User_size = sizeof(eeprom->UserSettings) - sizeof(eeprom->UserSettings.Checksum);
    EepromCRC(
        (unsigned char*)&eeprom->FactorySettings.Checksum,
        (unsigned char*)&eeprom->FactorySettings.SerialNumber[0],
        Factory_size
    );
    EepromCRC(
        (unsigned char*)&eeprom->UserSettings.Checksum,
        (unsigned char*)&eeprom->UserSettings.TimeZoneBias,
        User_size
    );
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
			DBG_PRINTF_EX(LOG_PREFIX_INIT, "Couldn't create EEPROM.bin file!\n");
			return nullptr;
		}
	}

	// Make sure EEPROM.bin is at least 256 bytes in size
	SetFilePointer(hFileEEPROM, EEPROM_SIZE, nullptr, FILE_BEGIN);
	SetEndOfFile(hFileEEPROM);

	HANDLE hFileMappingEEPROM = CreateFileMapping(
		hFileEEPROM,
		/* lpFileMappingAttributes */nullptr,
		PAGE_READWRITE,
		/* dwMaximumSizeHigh */0,
		/* dwMaximumSizeLow */EEPROM_SIZE,
		/**/nullptr);
	if (hFileMappingEEPROM == NULL)
	{
		DBG_PRINTF_EX(LOG_PREFIX_INIT, "Couldn't create EEPROM.bin file mapping!\n");
		return nullptr;
	}

	LARGE_INTEGER  len_li;
	GetFileSizeEx(hFileEEPROM, &len_li);
	unsigned int FileSize = len_li.u.LowPart;
	if (FileSize != 256)
	{
		CxbxKrnlCleanup("%s : EEPROM.bin file is not 256 bytes large!\n", __func__);
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
		DBG_PRINTF_EX(LOG_PREFIX_INIT, "Couldn't map EEPROM.bin into memory!\n");
		return nullptr;
	}

    // Recalculates the checksum field for User and Factory settings each time
    // so that users do not need to delete their EEPROM.bin from older versions
    gen_section_CRCs(pEEPROM);

	if (NeedsInitialization)
	{
		EmuEEPROMReset(pEEPROM);
		XboxFactoryGameRegion = pEEPROM->EncryptedSettings.GameRegion;

        // This must be done last to include all initialized data in the CRC
        gen_section_CRCs(pEEPROM);

		DBG_PRINTF_EX(LOG_PREFIX_INIT, "Initialized default EEPROM\n");
	}
	else
	{
		XboxFactoryGameRegion = pEEPROM->EncryptedSettings.GameRegion;
		DBG_PRINTF_EX(LOG_PREFIX_INIT, "Loaded EEPROM.bin\n");
	}

	// Read the HDD (and eventually also the online) keys stored in the eeprom file. Users can input them in the eeprom menu
	memcpy(xboxkrnl::XboxHDKey, pEEPROM->EncryptedSettings.HDKey, xboxkrnl::XBOX_KEY_LENGTH);

	// Verify the checksum of the eeprom header
	UCHAR Checksum[20] = { 0 };
	xboxkrnl::XcHMAC(xboxkrnl::XboxEEPROMKey, 16, pEEPROM->EncryptedSettings.Confounder, 8, pEEPROM->EncryptedSettings.HDKey, 20, Checksum);
	if (memcmp(Checksum, pEEPROM->EncryptedSettings.Checksum, 20))
	{
		// The checksums do not match. Log this error and flash the LED (red, off, red, off)
		EmuLog(LOG_LEVEL::WARNING, "Stored and calculated checksums don't match. Possible eeprom corruption");
		SetLEDSequence(0xA0);
	}

	return pEEPROM;
}

void EmuEEPROMReset(xboxkrnl::XBOX_EEPROM* eeprom)
{
	memset(eeprom, 0, sizeof(xboxkrnl::XBOX_EEPROM));

	// Set Factory Settings
	eeprom->FactorySettings.AVRegion = AV_STANDARD_NTSC_M | AV_FLAGS_60Hz;
	memcpy(eeprom->FactorySettings.SerialNumber, "Cxbx-R      ", 12);
	// TODO: Ethernet Address
	// TODO: Online Key

	// Encrypted Section
	eeprom->EncryptedSettings.GameRegion = XC_GAME_REGION_NA;
	// TODO: HDD Key
	xboxkrnl::XcHMAC(xboxkrnl::XboxEEPROMKey, 16, eeprom->EncryptedSettings.Confounder, 8, eeprom->EncryptedSettings.HDKey, 20, eeprom->EncryptedSettings.Checksum);

	// User Settings
	eeprom->UserSettings.Language = XC_LANGUAGE_ENGLISH;  // = English
	eeprom->UserSettings.VideoFlags = AV_FLAGS_NORMAL;   // = Use XDK defaults
	eeprom->UserSettings.AudioFlags = XC_AUDIO_FLAGS_STEREO;   // = Stereo, no AC3, no DTS
	eeprom->UserSettings.ParentalControlGames = XC_PC_ESRB_ALL; // = XC_PC_ESRB_ALL
	eeprom->UserSettings.ParentalControlMovies = XC_PC_MAX; // = XC_PRTL_CRTL_MAX
	eeprom->UserSettings.MiscFlags = 0;  // No automatic power down

	// TODO: Online Settings
	// TODO: TimeZone Settings
}
