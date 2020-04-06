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

#if defined(__cplusplus)
//extern "C"
//{
#endif

// prevent name collisions
namespace xboxkrnl
{
#undef _WIN32 // Compile-in REG_DWORD and friends, since we lack a <windows> include here
#include <xboxkrnl/xboxkrnl.h> // For XC_VALUE_INDEX and XBOX_EEPROM
#define _WIN32
};

#define EEPROM_SIZE sizeof(xboxkrnl::XBOX_EEPROM)

typedef struct EEPROMInfo {
	xboxkrnl::XC_VALUE_INDEX index;
	int value_offset;
	int value_type;
	int value_length;
} EEPROMInfo;

#define XC_END_MARKER (xboxkrnl::XC_VALUE_INDEX)-1

#define EEPROM_INFO_ENTRY(XC, Member, REG_Type) \
	{ xboxkrnl::##XC, offsetof(xboxkrnl::XBOX_EEPROM, Member), REG_Type, sizeof(((xboxkrnl::XBOX_EEPROM *)0)->Member) }

static const EEPROMInfo EEPROMInfos[] = {
	EEPROM_INFO_ENTRY(XC_TIMEZONE_BIAS,         UserSettings.TimeZoneBias,                REG_DWORD),
	EEPROM_INFO_ENTRY(XC_TZ_STD_NAME,           UserSettings.TimeZoneStdName,             REG_BINARY),
	EEPROM_INFO_ENTRY(XC_TZ_STD_DATE,           UserSettings.TimeZoneStdDate,             REG_DWORD),
	EEPROM_INFO_ENTRY(XC_TZ_STD_BIAS,           UserSettings.TimeZoneStdBias,             REG_DWORD),
	EEPROM_INFO_ENTRY(XC_TZ_DLT_NAME,           UserSettings.TimeZoneDltName,             REG_BINARY),
	EEPROM_INFO_ENTRY(XC_TZ_DLT_DATE,           UserSettings.TimeZoneDltDate,             REG_DWORD),
	EEPROM_INFO_ENTRY(XC_TZ_DLT_BIAS,           UserSettings.TimeZoneDltBias,             REG_DWORD),
	EEPROM_INFO_ENTRY(XC_LANGUAGE,              UserSettings.Language,                    REG_DWORD),
	EEPROM_INFO_ENTRY(XC_VIDEO,                 UserSettings.VideoFlags,                  REG_DWORD),
	EEPROM_INFO_ENTRY(XC_AUDIO,                 UserSettings.AudioFlags,                  REG_DWORD),
	EEPROM_INFO_ENTRY(XC_P_CONTROL_GAMES,       UserSettings.ParentalControlGames,        REG_DWORD), // Zapper queries this. TODO : Should this be REG_NONE?
	EEPROM_INFO_ENTRY(XC_P_CONTROL_PASSWORD,    UserSettings.ParentalControlPassword,     REG_DWORD),
	EEPROM_INFO_ENTRY(XC_P_CONTROL_MOVIES,      UserSettings.ParentalControlMovies,       REG_DWORD), // Xbox Dashboard queries this.
	EEPROM_INFO_ENTRY(XC_ONLINE_IP_ADDRESS,     UserSettings.OnlineIpAddress,             REG_DWORD),
	EEPROM_INFO_ENTRY(XC_ONLINE_DNS_ADDRESS,    UserSettings.OnlineDnsAddress,            REG_DWORD),
	EEPROM_INFO_ENTRY(XC_ONLINE_DEFAULT_GATEWAY_ADDRESS, UserSettings.OnlineDefaultGatewayAddress, REG_DWORD),
	EEPROM_INFO_ENTRY(XC_ONLINE_SUBNET_ADDRESS, UserSettings.OnlineSubnetMask,            REG_DWORD),
	EEPROM_INFO_ENTRY(XC_MISC,                  UserSettings.MiscFlags,                   REG_DWORD),
	EEPROM_INFO_ENTRY(XC_DVD_REGION,            UserSettings.DvdRegion,                   REG_DWORD),
	EEPROM_INFO_ENTRY(XC_MAX_OS,                UserSettings,                             REG_BINARY),
	// XC_MAX_OS is called to return a complete XBOX_USER_SETTINGS structure
	//
	// One example is from XapipQueryTimeZoneInformation(, REG_DWORD, sizeof(DWORD), where it is used to
	// detect the local timezone information.
	EEPROM_INFO_ENTRY(XC_FACTORY_SERIAL_NUMBER, FactorySettings.SerialNumber,             REG_BINARY),
	EEPROM_INFO_ENTRY(XC_FACTORY_ETHERNET_ADDR, FactorySettings.EthernetAddr,             REG_BINARY),
	EEPROM_INFO_ENTRY(XC_FACTORY_ONLINE_KEY,    FactorySettings.OnlineKey,                REG_BINARY),
	EEPROM_INFO_ENTRY(XC_FACTORY_AV_REGION,     FactorySettings.AVRegion,                 REG_DWORD),
	// Note : XC_FACTORY_GAME_REGION is linked to a separate ULONG XboxFactoryGameRegion (of type REG_DWORD)
	EEPROM_INFO_ENTRY(XC_FACTORY_GAME_REGION,   EncryptedSettings.GameRegion,             REG_DWORD),
	EEPROM_INFO_ENTRY(XC_ENCRYPTED_SECTION,     EncryptedSettings,                        REG_BINARY),
	{ xboxkrnl::XC_MAX_ALL,                     0,                                        REG_BINARY, sizeof(xboxkrnl::XBOX_EEPROM) },
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

extern xboxkrnl::XBOX_EEPROM *CxbxRestoreEEPROM(char *szFilePath_EEPROM_bin);

extern const EEPROMInfo* EmuFindEEPROMInfo(xboxkrnl::XC_VALUE_INDEX index);

extern xboxkrnl::XBOX_EEPROM *EEPROM;

extern xboxkrnl::ULONG XboxFactoryGameRegion;

extern void EmuEEPROMReset(xboxkrnl::XBOX_EEPROM* eeprom);

void gen_section_CRCs(xboxkrnl::XBOX_EEPROM*);

#if defined(__cplusplus)
//}
#endif

#endif // EMU_EEPROM_H
