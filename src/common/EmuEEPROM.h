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
#ifndef EMU_EEPROM_H
#define EMU_EEPROM_H

#include <core\kernel\exports\xboxkrnl.h> // For XC_VALUE_INDEX and XBOX_EEPROM

#define EEPROM_SIZE sizeof(xbox::XBOX_EEPROM)

typedef struct EEPROMInfo {
	xbox::XC_VALUE_INDEX index;
	int value_offset;
	int value_type;
	int value_length;
} EEPROMInfo;

#define XC_END_MARKER (xbox::XC_VALUE_INDEX)-1

#define EEPROM_INFO_ENTRY(XC, Member, REG_Type) \
	{ xbox::XC, offsetof(xbox::XBOX_EEPROM, Member), REG_Type, sizeof(((xbox::XBOX_EEPROM *)0)->Member) }

static const EEPROMInfo EEPROMInfos[] = {
	EEPROM_INFO_ENTRY(XC_TIMEZONE_BIAS,         UserSettings.TimeZoneBias,                xbox::reg_dword),
	EEPROM_INFO_ENTRY(XC_TZ_STD_NAME,           UserSettings.TimeZoneStdName,             xbox::reg_binary),
	EEPROM_INFO_ENTRY(XC_TZ_STD_DATE,           UserSettings.TimeZoneStdDate,             xbox::reg_dword),
	EEPROM_INFO_ENTRY(XC_TZ_STD_BIAS,           UserSettings.TimeZoneStdBias,             xbox::reg_dword),
	EEPROM_INFO_ENTRY(XC_TZ_DLT_NAME,           UserSettings.TimeZoneDltName,             xbox::reg_binary),
	EEPROM_INFO_ENTRY(XC_TZ_DLT_DATE,           UserSettings.TimeZoneDltDate,             xbox::reg_dword),
	EEPROM_INFO_ENTRY(XC_TZ_DLT_BIAS,           UserSettings.TimeZoneDltBias,             xbox::reg_dword),
	EEPROM_INFO_ENTRY(XC_LANGUAGE,              UserSettings.Language,                    xbox::reg_dword),
	EEPROM_INFO_ENTRY(XC_VIDEO,                 UserSettings.VideoFlags,                  xbox::reg_dword),
	EEPROM_INFO_ENTRY(XC_AUDIO,                 UserSettings.AudioFlags,                  xbox::reg_dword),
	EEPROM_INFO_ENTRY(XC_P_CONTROL_GAMES,       UserSettings.ParentalControlGames,        xbox::reg_dword), // Zapper queries this. TODO : Should this be xbox::reg_none?
	EEPROM_INFO_ENTRY(XC_P_CONTROL_PASSWORD,    UserSettings.ParentalControlPassword,     xbox::reg_dword),
	EEPROM_INFO_ENTRY(XC_P_CONTROL_MOVIES,      UserSettings.ParentalControlMovies,       xbox::reg_dword), // Xbox Dashboard queries this.
	EEPROM_INFO_ENTRY(XC_ONLINE_IP_ADDRESS,     UserSettings.OnlineIpAddress,             xbox::reg_dword),
	EEPROM_INFO_ENTRY(XC_ONLINE_DNS_ADDRESS,    UserSettings.OnlineDnsAddress,            xbox::reg_dword),
	EEPROM_INFO_ENTRY(XC_ONLINE_DEFAULT_GATEWAY_ADDRESS, UserSettings.OnlineDefaultGatewayAddress, xbox::reg_dword),
	EEPROM_INFO_ENTRY(XC_ONLINE_SUBNET_ADDRESS, UserSettings.OnlineSubnetMask,            xbox::reg_dword),
	EEPROM_INFO_ENTRY(XC_MISC,                  UserSettings.MiscFlags,                   xbox::reg_dword),
	EEPROM_INFO_ENTRY(XC_DVD_REGION,            UserSettings.DvdRegion,                   xbox::reg_dword),
	EEPROM_INFO_ENTRY(XC_MAX_OS,                UserSettings,                             xbox::reg_binary),
	// XC_MAX_OS is called to return a complete XBOX_USER_SETTINGS structure
	//
	// One example is from XapipQueryTimeZoneInformation(, xbox::reg_dword, sizeof(DWORD), where it is used to
	// detect the local timezone information.
	EEPROM_INFO_ENTRY(XC_FACTORY_SERIAL_NUMBER, FactorySettings.SerialNumber,             xbox::reg_binary),
	EEPROM_INFO_ENTRY(XC_FACTORY_ETHERNET_ADDR, FactorySettings.EthernetAddr,             xbox::reg_binary),
	EEPROM_INFO_ENTRY(XC_FACTORY_ONLINE_KEY,    FactorySettings.OnlineKey,                xbox::reg_binary),
	EEPROM_INFO_ENTRY(XC_FACTORY_AV_REGION,     FactorySettings.AVRegion,                 xbox::reg_dword),
	// Note : XC_FACTORY_GAME_REGION is linked to a separate ULONG XboxFactoryGameRegion (of type xbox::reg_dword)
	EEPROM_INFO_ENTRY(XC_FACTORY_GAME_REGION,   EncryptedSettings.GameRegion,             xbox::reg_dword),
	EEPROM_INFO_ENTRY(XC_ENCRYPTED_SECTION,     EncryptedSettings,                        xbox::reg_binary),
	{ xbox::XC_MAX_ALL,                         0,                                        xbox::reg_binary, sizeof(xbox::XBOX_EEPROM) },
	{ XC_END_MARKER }
};

// ******************************************************************
// * EEPROM specific constants
// ******************************************************************

// Game region codes
#define XC_GAME_REGION_NA             0x00000001
#define XC_GAME_REGION_JAPAN          0x00000002
#define XC_GAME_REGION_RESTOFWORLD    0x00000004
#define XC_GAME_REGION_MANUFACTURING  0x80000000

// Language ID
#define XC_LANGUAGE_NONE            0
#define XC_LANGUAGE_ENGLISH         1
#define XC_LANGUAGE_JAPANESE        2
#define XC_LANGUAGE_GERMAN          3
#define XC_LANGUAGE_FRENCH          4
#define XC_LANGUAGE_SPANISH         5
#define XC_LANGUAGE_ITALIAN         6
#define XC_LANGUAGE_KOREAN          7
#define XC_LANGUAGE_CHINESE         8
#define XC_LANGUAGE_PORTUGUESE      9
#define XC_LANGUAGE_MAX             XC_LANGUAGE_PORTUGUESE + 1

// Audio settings
#define XC_AUDIO_FLAGS_STEREO       0x00000000
#define XC_AUDIO_FLAGS_MONO         0x00000001
#define XC_AUDIO_FLAGS_SURROUND     0x00000002
#define XC_AUDIO_FLAGS_ENABLE_AC3   0x00010000
#define XC_AUDIO_FLAGS_ENABLE_DTS   0x00020000

// Game rating
#define XC_PC_ESRB_ALL              0
#define XC_PC_ESRB_ADULT            1
#define XC_PC_ESRB_MATURE           2
#define XC_PC_ESRB_TEEN             3
#define XC_PC_ESRB_EVERYONE         4
#define XC_PC_ESRB_KIDS_TO_ADULTS   5
#define XC_PC_ESRB_EARLY_CHILDHOOD  6

// Movie rating
#define XC_PC_MAX                   0
#define XC_PC_NC17                  1
#define XC_PC_A                     2
#define XC_PC_5                     3
#define XC_PC_PG13                  4
#define XC_PC_PG                    5
#define XC_PC_2                     6
#define XC_PC_G                     7

// DVD region code
#define DVD_REGION_FREE             0
#define DVD_REGION_US               1
#define DVD_REGION_EU               2
#define DVD_REGION_SE_ASIA          3
#define DVD_REGION_OCEANIA          4
#define DVD_REGION_AFRICA           5
#define DVD_REGION_CHINA            6
#define DVD_REGION_RESERVED         7
#define DVD_REGION_INTERNATIONAL    8

extern xbox::XBOX_EEPROM *CxbxRestoreEEPROM(char *szFilePath_EEPROM_bin);

extern const EEPROMInfo* EmuFindEEPROMInfo(xbox::XC_VALUE_INDEX index);

extern xbox::XBOX_EEPROM *EEPROM;

extern xbox::ulong_xt XboxFactoryGameRegion;

extern void EmuEEPROMReset(xbox::XBOX_EEPROM* eeprom);

void gen_section_CRCs(xbox::XBOX_EEPROM*);

#endif // EMU_EEPROM_H
