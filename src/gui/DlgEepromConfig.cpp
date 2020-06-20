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
// *  (c) 2018 ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

#define LOG_PREFIX CXBXR_MODULE::GUI

#include <fstream>
#include <cstring> // For memcpy
#include "EmuEEPROM.h" // For EEPROMInfo, EEPROMInfos
#include "core\kernel\init\CxbxKrnl.h"
#include "DlgEepromConfig.h"
#include "resource/ResCxbx.h"
#include "common/Logging.h"
#include <Commctrl.h>


// Generic function called from the gui to display the eeprom menu
void ShowEepromConfig(HWND hwnd);
// Windows dialog procedure for the eeprom menu
static INT_PTR CALLBACK DlgEepromConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
// Indicates that changes have been made in the eeprom menu
static bool g_bHasChanges = false;
// Pointer to the cached eeprom in memory
static xboxkrnl::XBOX_EEPROM* pEEPROM_GUI = nullptr;
// Window procedure of the subclass
LRESULT CALLBACK ControlSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
// Array containing all the values used to initialize the combo box controls
static unsigned int ComboboxArray[] = {
	XC_GAME_REGION_NA, XC_GAME_REGION_JAPAN, XC_GAME_REGION_RESTOFWORLD, XC_GAME_REGION_MANUFACTURING, AV_STANDARD_NTSC_M | AV_FLAGS_60Hz,
	AV_STANDARD_NTSC_J | AV_FLAGS_60Hz, AV_STANDARD_PAL_I | AV_FLAGS_50Hz, XC_LANGUAGE_ENGLISH, XC_LANGUAGE_JAPANESE,
	XC_LANGUAGE_GERMAN, XC_LANGUAGE_FRENCH, XC_LANGUAGE_SPANISH, XC_LANGUAGE_ITALIAN, XC_LANGUAGE_KOREAN, XC_LANGUAGE_CHINESE, XC_LANGUAGE_PORTUGUESE,
	AV_FLAGS_NORMAL, AV_FLAGS_WIDESCREEN, AV_FLAGS_LETTERBOX, XC_AUDIO_FLAGS_STEREO, XC_AUDIO_FLAGS_STEREO | XC_AUDIO_FLAGS_ENABLE_AC3,
	XC_AUDIO_FLAGS_STEREO | XC_AUDIO_FLAGS_ENABLE_DTS, XC_AUDIO_FLAGS_STEREO | XC_AUDIO_FLAGS_ENABLE_AC3 | XC_AUDIO_FLAGS_ENABLE_DTS,
	XC_AUDIO_FLAGS_MONO, XC_AUDIO_FLAGS_MONO | XC_AUDIO_FLAGS_ENABLE_AC3, XC_AUDIO_FLAGS_MONO | XC_AUDIO_FLAGS_ENABLE_DTS,
	XC_AUDIO_FLAGS_MONO | XC_AUDIO_FLAGS_ENABLE_AC3 | XC_AUDIO_FLAGS_ENABLE_DTS, XC_AUDIO_FLAGS_SURROUND, XC_AUDIO_FLAGS_SURROUND
	| XC_AUDIO_FLAGS_ENABLE_AC3, XC_AUDIO_FLAGS_SURROUND | XC_AUDIO_FLAGS_ENABLE_DTS, XC_AUDIO_FLAGS_SURROUND | XC_AUDIO_FLAGS_ENABLE_AC3
	| XC_AUDIO_FLAGS_ENABLE_DTS, XC_PC_ESRB_ALL, XC_PC_ESRB_ADULT, XC_PC_ESRB_MATURE, XC_PC_ESRB_TEEN, XC_PC_ESRB_EVERYONE,
	XC_PC_ESRB_KIDS_TO_ADULTS, XC_PC_ESRB_EARLY_CHILDHOOD, XC_PC_MAX, XC_PC_NC17, XC_PC_A, XC_PC_5, XC_PC_PG13, XC_PC_PG, XC_PC_2, XC_PC_G,
	DVD_REGION_FREE, DVD_REGION_US, DVD_REGION_EU, DVD_REGION_SE_ASIA, DVD_REGION_OCEANIA, DVD_REGION_AFRICA, DVD_REGION_CHINA,
	DVD_REGION_RESERVED, DVD_REGION_INTERNATIONAL
};

void WriteEepromInMemory(HWND hDlg)
{
	// Confounder
	{
		char Buffer[17];
		SendMessage(GetDlgItem(hDlg, IDC_EE_CONFOUNDER), WM_GETTEXT, 17, reinterpret_cast<LPARAM>(Buffer));
		std::string hex(Buffer);
		size_t len = hex.length();
		std::string ByteSequence;
		for (unsigned int i = 0; i < len; i += 2)
		{
			std::string byte = hex.substr(i, 2);
			char chr = static_cast<int>(stol(byte, nullptr, 16));
			ByteSequence.push_back(chr);
		}
		std::memcpy(&pEEPROM_GUI->EncryptedSettings.Confounder, ByteSequence.c_str(), 8);
	}

	// HDD key
	{
		char Buffer[33];
		SendMessage(GetDlgItem(hDlg, IDC_EE_HDDKEY), WM_GETTEXT, 33, reinterpret_cast<LPARAM>(Buffer));
		std::string hex(Buffer);
		size_t len = hex.length();
		std::string ByteSequence;
		for (unsigned int i = 0; i < len; i += 2)
		{
			std::string byte = hex.substr(i, 2);
			char chr = static_cast<int>(stol(byte, nullptr, 16));
			ByteSequence.push_back(chr);
		}
		std::memcpy(&pEEPROM_GUI->EncryptedSettings.HDKey, ByteSequence.c_str(), 16);
	}

	// Serial number
	{
		char Buffer[13];
		SendMessage(GetDlgItem(hDlg, IDC_EE_SERIAL_NUMBER), WM_GETTEXT, 25, reinterpret_cast<LPARAM>(Buffer));
		std::memcpy(&pEEPROM_GUI->FactorySettings.SerialNumber, Buffer, 12);
	}

	// Ethernet address
	{
		char Buffer[13];
		SendMessage(GetDlgItem(hDlg, IDC_EE_MAC_ADDRESS), WM_GETTEXT, 13, reinterpret_cast<LPARAM>(Buffer));
		std::string hex(Buffer);
		size_t len = hex.length();
		std::string ByteSequence;
		for (unsigned int i = 0; i < len; i += 2)
		{
			std::string byte = hex.substr(i, 2);
			char chr = static_cast<int>(stol(byte, nullptr, 16));
			ByteSequence.push_back(chr);
		}
		std::memcpy(&pEEPROM_GUI->FactorySettings.EthernetAddr, ByteSequence.c_str(), 6);
	}

	// Online key
	{
		char Buffer[33];
		SendMessage(GetDlgItem(hDlg, IDC_EE_ONLINE_KEY), WM_GETTEXT, 33, reinterpret_cast<LPARAM>(Buffer));
		std::string hex(Buffer);
		size_t len = hex.length();
		std::string ByteSequence;
		for (unsigned int i = 0; i < len; i += 2)
		{
			std::string byte = hex.substr(i, 2);
			char chr = static_cast<int>(stol(byte, nullptr, 16));
			ByteSequence.push_back(chr);
		}
		std::memcpy(&pEEPROM_GUI->FactorySettings.OnlineKey, ByteSequence.c_str(), 16);
	}

	// Parental password
	{
		char Buffer[9];
		SendMessage(GetDlgItem(hDlg, IDC_EE_PRTL_PASS), WM_GETTEXT, 9, reinterpret_cast<LPARAM>(Buffer));
		std::string hex(Buffer);
		size_t len = hex.length();
		std::string ByteSequence;
		for (unsigned int i = 0; i < len; i += 2)
		{
			std::string byte = hex.substr(i, 2);
			char chr = static_cast<int>(stol(byte, nullptr, 16));
			ByteSequence.push_back(chr);
		}
		std::memcpy(&pEEPROM_GUI->UserSettings.ParentalControlPassword, ByteSequence.c_str(), 4);
	}

	// All the combo box and check box parameters
	{
		uint32_t Buffer[8][1];
		int offset = 0;
		for (auto i : { IDC_EE_XBOX_REGION, IDC_EE_AVREGION, IDC_EE_LANGUAGE, IDC_EE_AVSETTINGS, IDC_EE_AUDIOSETTINGS, IDC_EE_GAME_PRTL_CRTL,
			IDC_EE_MOVIE_PRTL_CRTL, IDC_EE_DVDREGION }) {
			HWND hControlbox = GetDlgItem(hDlg, i);
			Buffer[offset][0] = SendMessage(hControlbox, CB_GETITEMDATA, SendMessage(hControlbox, CB_GETCURSEL, 0, 0), 0);
			offset++;
		}
		if (IsWindowEnabled(GetDlgItem(hDlg, IDC_EE_PAL60HZ)) == TRUE) {
			if (SendMessage(GetDlgItem(hDlg, IDC_EE_PAL60HZ), BM_GETCHECK, 0, 0) == BST_CHECKED) {
				Buffer[3][0] |= AV_FLAGS_60Hz;
			}
		}
		else {
			if (SendMessage(GetDlgItem(hDlg, IDC_EE_480P), BM_GETCHECK, 0, 0) == BST_CHECKED) {
				Buffer[3][0] |= AV_FLAGS_HDTV_480p;
			}
			if (SendMessage(GetDlgItem(hDlg, IDC_EE_720P), BM_GETCHECK, 0, 0) == BST_CHECKED) {
				Buffer[3][0] |= AV_FLAGS_HDTV_720p;
			}
			if (SendMessage(GetDlgItem(hDlg, IDC_EE_1080I), BM_GETCHECK, 0, 0) == BST_CHECKED) {
				Buffer[3][0] |= AV_FLAGS_HDTV_1080i;
			}
		}
		std::memcpy(&pEEPROM_GUI->EncryptedSettings.GameRegion, Buffer[0], 4);
		std::memcpy(&pEEPROM_GUI->FactorySettings.AVRegion, Buffer[1], 4);
		std::memcpy(&pEEPROM_GUI->UserSettings.Language, Buffer[2], 4);
		std::memcpy(&pEEPROM_GUI->UserSettings.VideoFlags, Buffer[3], 4);
		std::memcpy(&pEEPROM_GUI->UserSettings.AudioFlags, Buffer[4], 4);
		std::memcpy(&pEEPROM_GUI->UserSettings.ParentalControlGames, Buffer[5], 4);
		std::memcpy(&pEEPROM_GUI->UserSettings.ParentalControlMovies, Buffer[6], 4);
		std::memcpy(&pEEPROM_GUI->UserSettings.DvdRegion, Buffer[7], 4);
	}

	// Finally, recalculate the hash and the checksums
	uint8_t EepromKey[16] = { 0 };
	std::basic_ifstream<uint8_t> EepromFile(std::string(szFolder_CxbxReloadedData) + "\\keys.bin", std::ios::binary);
	if (EepromFile.is_open()) {
		EepromFile.read(EepromKey, 16);
		EepromFile.close();
	}
	xboxkrnl::XcHMAC(EepromKey, 16, pEEPROM_GUI->EncryptedSettings.Confounder, 8, pEEPROM_GUI->EncryptedSettings.HDKey, 20,
		pEEPROM_GUI->EncryptedSettings.Checksum);
	gen_section_CRCs(pEEPROM_GUI);
}

void ShowEepromConfig(HWND hwnd)
{
	// Load the eeprom file from disk and cache it in memory
	if (pEEPROM_GUI == nullptr) {
		std::basic_ifstream<uint8_t> EepromFile(szFilePath_EEPROM_bin, std::ios::binary);
		if (EepromFile.is_open()) {
			pEEPROM_GUI = new xboxkrnl::XBOX_EEPROM;
			EepromFile.read(reinterpret_cast<uint8_t*>(pEEPROM_GUI), EEPROM_SIZE);
			EepromFile.close();
		}
		else {
			PopupWarning(hwnd, "Couldn't open eeprom file!");
			return;
		}
	}

	// Show dialog box
	DialogBox(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_EEPROM_CFG), hwnd, DlgEepromConfigProc);
}

static void RefreshEepromDialog(HWND hWndDlg)
{
	int offset;
	int starting_offset;
	HWND hHandle;
	uint32_t value;

	// Set window icon
	SetClassLong(hWndDlg, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_CXBX)));

	// Initialize the values of the drop-down lists
	offset = 0;
	hHandle = GetDlgItem(hWndDlg, IDC_EE_XBOX_REGION);
	for (auto i : { "North America" , "Japan" ,"Rest of the world", "Manufacturing" }) {
		LRESULT index = SendMessage(hHandle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(i));
		SendMessage(hHandle, CB_SETITEMDATA, index, ComboboxArray[offset]);
		offset++;
	}
	value = pEEPROM_GUI->EncryptedSettings.GameRegion;
	starting_offset = offset - 4;
	for (int i = starting_offset; i < offset; i++) {
		if (value == ComboboxArray[i]) {
			SendMessage(hHandle, CB_SETCURSEL, i - starting_offset, 0);
			break;
		}
	}

	hHandle = GetDlgItem(hWndDlg, IDC_EE_AVREGION);
	for (auto i : { "NTSC-M, 60Hz" , "NTSC-J, 60Hz", "PAL, 50Hz" }) {
		LRESULT index = SendMessage(hHandle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(i));
		SendMessage(hHandle, CB_SETITEMDATA, index, ComboboxArray[offset]);
		offset++;
	}
	value = pEEPROM_GUI->FactorySettings.AVRegion;
	starting_offset = offset - 3;
	for (int i = starting_offset; i < offset; i++) {
		if (value == ComboboxArray[i]) {
			SendMessage(hHandle, CB_SETCURSEL, i - starting_offset, 0);
			break;
		}
	}

	hHandle = GetDlgItem(hWndDlg, IDC_EE_LANGUAGE);
	for (auto i : { "English", "Japanese", "German", "French", "Spanish", "Italian", "Korean", "Chinese", "Portuguese" }) {
		LRESULT index = SendMessage(hHandle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(i));
		SendMessage(hHandle, CB_SETITEMDATA, index, ComboboxArray[offset]);
		offset++;
	}
	value = pEEPROM_GUI->UserSettings.Language;
	starting_offset = offset - 9;
	for (int i = starting_offset; i < offset; i++) {
		if (value == ComboboxArray[i]) {
			SendMessage(hHandle, CB_SETCURSEL, i - starting_offset, 0);
			break;
		}
	}

	// The available AV settings depend on the current AV region so we must read those beforehand
	hHandle = GetDlgItem(hWndDlg, IDC_EE_AVSETTINGS);
	for (auto i : { "Normal", "Widescreen", "Letterbox" }) {
		LRESULT index = SendMessage(hHandle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(i));
		SendMessage(hHandle, CB_SETITEMDATA, index, ComboboxArray[offset]);
		offset++;
	}
	value = pEEPROM_GUI->UserSettings.VideoFlags;
	value &= ~(AV_FLAGS_60Hz | AV_FLAGS_HDTV_480p | AV_FLAGS_HDTV_720p | AV_FLAGS_HDTV_1080i);
	starting_offset = offset - 3;
	for (int i = starting_offset; i < offset; i++) {
		if (value == ComboboxArray[i]) {
			SendMessage(hHandle, CB_SETCURSEL, i - starting_offset, 0);
			break;
		}
	}
	if (pEEPROM_GUI->FactorySettings.AVRegion == (AV_FLAGS_50Hz | AV_STANDARD_PAL_I)) {
		// Enable PAL 60Hz since the console is set to the PAL region
		EnableWindow(GetDlgItem(hWndDlg, IDC_EE_PAL60HZ), TRUE);
		EnableWindow(GetDlgItem(hWndDlg, IDC_EE_480P), FALSE);
		EnableWindow(GetDlgItem(hWndDlg, IDC_EE_720P), FALSE);
		EnableWindow(GetDlgItem(hWndDlg, IDC_EE_1080I), FALSE);
		if (pEEPROM_GUI->UserSettings.VideoFlags & AV_FLAGS_60Hz) {
			SendMessage(GetDlgItem(hWndDlg, IDC_EE_PAL60HZ), BM_SETCHECK, BST_CHECKED, 0);
		}
	}
	else {
		// Enable 480p, 720p and 1080i since the console is set to the NTSC region
		uint32_t value = pEEPROM_GUI->UserSettings.VideoFlags;
		if (value & AV_FLAGS_HDTV_480p) {
			SendMessage(GetDlgItem(hWndDlg, IDC_EE_480P), BM_SETCHECK, BST_CHECKED, 0);
		}
		if (value & AV_FLAGS_HDTV_720p) {
			SendMessage(GetDlgItem(hWndDlg, IDC_EE_720P), BM_SETCHECK, BST_CHECKED, 0);
		}
		if (value & AV_FLAGS_HDTV_1080i) {
			SendMessage(GetDlgItem(hWndDlg, IDC_EE_1080I), BM_SETCHECK, BST_CHECKED, 0);
		}
	}

	hHandle = GetDlgItem(hWndDlg, IDC_EE_AUDIOSETTINGS);
	for (auto i : { "Stereo", "Stereo, Dolby AC3", "Stereo, DTS", "Stereo, Dolby AC3, DTS", "Mono", "Mono, Dolby AC3", "Mono, DTS",
		 "Mono, Dolby AC3, DTS", "Dolby Surround", "Dolby Surround, Dolby AC3", "Dolby Surround, DTS", "Dolby Surround, Dolby AC3, DTS" }) {
		LRESULT index = SendMessage(hHandle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(i));
		SendMessage(hHandle, CB_SETITEMDATA, index, ComboboxArray[offset]);
		offset++;
	}
	value = pEEPROM_GUI->UserSettings.AudioFlags;
	starting_offset = offset - 12;
	for (int i = starting_offset; i < offset; i++) {
		if (value == ComboboxArray[i]) {
			SendMessage(hHandle, CB_SETCURSEL, i - starting_offset, 0);
			break;
		}
	}

	hHandle = GetDlgItem(hWndDlg, IDC_EE_GAME_PRTL_CRTL);
	for (auto i : { "Rating pending", "Adults only", "Mature", "Teen", "Everyone", "Kids to adults", "Early childhood" }) {
		LRESULT index = SendMessage(hHandle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(i));
		SendMessage(hHandle, CB_SETITEMDATA, index, ComboboxArray[offset]);
		offset++;
	}
	value = pEEPROM_GUI->UserSettings.ParentalControlGames;
	starting_offset = offset - 7;
	for (int i = starting_offset; i < offset; i++) {
		if (value == ComboboxArray[i]) {
			SendMessage(hHandle, CB_SETCURSEL, i - starting_offset, 0);
			break;
		}
	}

	hHandle = GetDlgItem(hWndDlg, IDC_EE_MOVIE_PRTL_CRTL);
	for (auto i : { "Max", "NC-17", "A", "5", "PG-13", "PG", "2", "G" }) {
		LRESULT index = SendMessage(hHandle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(i));
		SendMessage(hHandle, CB_SETITEMDATA, index, ComboboxArray[offset]);
		offset++;
	}
	value = pEEPROM_GUI->UserSettings.ParentalControlMovies;
	starting_offset = offset - 8;
	for (int i = starting_offset; i < offset; i++) {
		if (value == ComboboxArray[i]) {
			SendMessage(hHandle, CB_SETCURSEL, i - starting_offset, 0);
			break;
		}
	}

	hHandle = GetDlgItem(hWndDlg, IDC_EE_DVDREGION);
	for (auto i : { "Free", "USA, Canada", "Japan, Europe, Middle East, ...", "Southeast Asia, S. Korea, ...", "Latin America, Oceania",
		"Africa, Russia, N. Korea, ...", "China", "Reserved for future use", "International venues" }) {
		LRESULT index = SendMessage(hHandle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(i));
		SendMessage(hHandle, CB_SETITEMDATA, index, ComboboxArray[offset]);
		offset++;
	}
	value = pEEPROM_GUI->UserSettings.DvdRegion;
	starting_offset = offset - 9;
	for (int i = starting_offset; i < offset; i++) {
		if (value == ComboboxArray[i]) {
			SendMessage(hHandle, CB_SETCURSEL, i - starting_offset, 0);
			break;
		}
	}

	// Install the subclass procedure and display the current values of the parameters in the edit controls
	{
		int ByteLimit[] = { 8, 16, 12, 6, 16, 4 };
		uint8_t* Buffer[6];
		HWND hEditControlArray[6];
		int j = 0;
		for (auto i : { IDC_EE_CONFOUNDER, IDC_EE_HDDKEY, IDC_EE_SERIAL_NUMBER, IDC_EE_MAC_ADDRESS, IDC_EE_ONLINE_KEY, IDC_EE_PRTL_PASS }) {
			hEditControlArray[j] = GetDlgItem(hWndDlg, i);
			Buffer[j] = new uint8_t[ByteLimit[j]];
			SetWindowSubclass(hEditControlArray[j], ControlSubclassProc, i, 0);
			SendMessage(hEditControlArray[j], EM_SETLIMITTEXT, (i == IDC_EE_SERIAL_NUMBER ? ByteLimit[j] : ByteLimit[j] * 2), 0);
			j++;
		}
		j = 0;
		std::memcpy(Buffer[0], &pEEPROM_GUI->EncryptedSettings.Confounder, ByteLimit[0]);
		std::memcpy(Buffer[1], &pEEPROM_GUI->EncryptedSettings.HDKey, ByteLimit[1]);
		std::memcpy(Buffer[2], &pEEPROM_GUI->FactorySettings.SerialNumber, ByteLimit[2]);
		std::memcpy(Buffer[3], &pEEPROM_GUI->FactorySettings.EthernetAddr, ByteLimit[3]);
		std::memcpy(Buffer[4], &pEEPROM_GUI->FactorySettings.OnlineKey, ByteLimit[4]);
		std::memcpy(Buffer[5], &pEEPROM_GUI->UserSettings.ParentalControlPassword, ByteLimit[5]);
		for (auto i : Buffer) {
			char* CharBuffer = new char[ByteLimit[j] * 2 + 1];
			for (int z = 0, y = 0; z < ByteLimit[j]; z++) {
				// Special case for Serial Number: This field is textual
				if (hEditControlArray[j] == GetDlgItem(hWndDlg, IDC_EE_SERIAL_NUMBER)) {
					std::sprintf(&CharBuffer[y], "%c", i[z]);
					y += 1;
				} else {
					std::sprintf(&CharBuffer[y], "%02X", i[z]);
					y += 2;
				}
			}
			SendMessage(hEditControlArray[j], WM_SETTEXT, 0, reinterpret_cast<LPARAM>(CharBuffer));
			delete[] CharBuffer; CharBuffer = nullptr;
			delete[] Buffer[j]; Buffer[j] = nullptr;
			j++;
		}
	}
}

INT_PTR CALLBACK DlgEepromConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			RefreshEepromDialog(hWndDlg);
			// Reset changes flag
			g_bHasChanges = false;
		}
		break;

		case WM_CLOSE:
		{
			PostMessage(hWndDlg, WM_COMMAND, IDC_EE_CANCEL, 0);
		}
		break;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDC_EE_CANCEL:
				{
					delete pEEPROM_GUI;
					pEEPROM_GUI = nullptr;
					EndDialog(hWndDlg, wParam);
				}
				break;

				case IDC_EE_ACCEPT:
				{
					if (g_bHasChanges) {
						WriteEepromInMemory(hWndDlg);
						std::basic_ofstream<uint8_t> EepromFile(szFilePath_EEPROM_bin, std::ios::binary);
						if (EepromFile.is_open()) {
							EepromFile.write(reinterpret_cast<uint8_t*>(pEEPROM_GUI), EEPROM_SIZE);
							EepromFile.close();
						}
						else {
							PopupWarning(hWndDlg, "Couldn't write eeprom file to disk!");
						}
					}
					PostMessage(hWndDlg, WM_COMMAND, IDC_EE_CANCEL, 0);
				}
				break;

				case IDC_EE_RESET:
				{
					g_bHasChanges = true;
					std::memset(pEEPROM_GUI, 0, EEPROM_SIZE);
					EmuEEPROMReset(pEEPROM_GUI);

					RefreshEepromDialog(hWndDlg);
				}
				break;

				case IDC_EE_CONFOUNDER:
				case IDC_EE_HDDKEY:
				case IDC_EE_SERIAL_NUMBER:
				case IDC_EE_MAC_ADDRESS:
				case IDC_EE_ONLINE_KEY:
				case IDC_EE_PRTL_PASS:
				{
					if (HIWORD(wParam) == EN_CHANGE) {
						g_bHasChanges = true;
					}
					if (HIWORD(wParam) == EN_KILLFOCUS) {
						HWND hEditControl = GetDlgItem(hWndDlg, LOWORD(wParam));
						size_t MaxSize = SendMessage(hEditControl, EM_GETLIMITTEXT, 0, 0);
						size_t CurrentSize = SendMessage(hEditControl, EM_LINELENGTH, 0, 0);
						if (CurrentSize < MaxSize) {
							char* pBuffer = new char[MaxSize];
							SendMessage(hEditControl, WM_GETTEXT, MaxSize, reinterpret_cast<LPARAM>(pBuffer));
							std::string Text(pBuffer);
							Text.append(MaxSize - CurrentSize, '0');
							SendMessage(hEditControl, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(Text.c_str()));
							delete[] pBuffer; pBuffer = nullptr;
						}
					}
				}
				break;

				case IDC_EE_PAL60HZ:
				case IDC_EE_480P:
				case IDC_EE_720P:
				case IDC_EE_1080I:
					if (HIWORD(wParam) == BN_CLICKED) {
						g_bHasChanges = true;
					}
					break;

				case IDC_EE_XBOX_REGION:
				case IDC_EE_LANGUAGE:
				case IDC_EE_AVSETTINGS:
				case IDC_EE_AUDIOSETTINGS:
				case IDC_EE_GAME_PRTL_CRTL:
				case IDC_EE_MOVIE_PRTL_CRTL:
				case IDC_EE_DVDREGION:
					if (HIWORD(wParam) == CBN_SELCHANGE) {
						g_bHasChanges = true;
					}
					break;

				case IDC_EE_AVREGION:
					if (HIWORD(wParam) == CBN_SELCHANGE) {
						g_bHasChanges = true;
						// Check to see if we have to update the checkboxes
						HWND hControlbox = GetDlgItem(hWndDlg, IDC_EE_AVREGION);
						if (SendMessage(hControlbox, CB_GETITEMDATA, SendMessage(hControlbox, CB_GETCURSEL, 0, 0), 0)
							== (AV_FLAGS_50Hz | AV_STANDARD_PAL_I)) {
							if (IsWindowEnabled(GetDlgItem(hWndDlg, IDC_EE_PAL60HZ)) == FALSE) {
								EnableWindow(GetDlgItem(hWndDlg, IDC_EE_PAL60HZ), TRUE);
								EnableWindow(GetDlgItem(hWndDlg, IDC_EE_480P), FALSE);
								EnableWindow(GetDlgItem(hWndDlg, IDC_EE_720P), FALSE);
								EnableWindow(GetDlgItem(hWndDlg, IDC_EE_1080I), FALSE);
								SendMessage(GetDlgItem(hWndDlg, IDC_EE_480P), BM_SETCHECK, BST_UNCHECKED, 0);
								SendMessage(GetDlgItem(hWndDlg, IDC_EE_720P), BM_SETCHECK, BST_UNCHECKED, 0);
								SendMessage(GetDlgItem(hWndDlg, IDC_EE_1080I), BM_SETCHECK, BST_UNCHECKED, 0);
							}
						}
						else {
							if (IsWindowEnabled(GetDlgItem(hWndDlg, IDC_EE_PAL60HZ)) == TRUE) {
								EnableWindow(GetDlgItem(hWndDlg, IDC_EE_PAL60HZ), FALSE);
								EnableWindow(GetDlgItem(hWndDlg, IDC_EE_480P), TRUE);
								EnableWindow(GetDlgItem(hWndDlg, IDC_EE_720P), TRUE);
								EnableWindow(GetDlgItem(hWndDlg, IDC_EE_1080I), TRUE);
								SendMessage(GetDlgItem(hWndDlg, IDC_EE_PAL60HZ), BM_SETCHECK, BST_UNCHECKED, 0);
							}
						}
					}
					break;
			}
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK ControlSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
	LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (uMsg)
	{
		// Remove the window subclass when this window is destroyed
		case WM_NCDESTROY:
			RemoveWindowSubclass(hWnd, ControlSubclassProc, uIdSubclass);
			break;

		// Override the default system behaviour and process WM_CHAR ourselves
		case WM_GETDLGCODE:
			if (lParam) {
				LPMSG lpmsg = reinterpret_cast<LPMSG>(lParam);
				if (lpmsg->message == WM_CHAR) {
					return DLGC_WANTCHARS;
				}
			}
			break;

		case WM_CHAR:
		{
			// Serial Number field is numeric, stored as ASCII text
			if (uIdSubclass == IDC_EE_SERIAL_NUMBER) {
				if (!((wParam >= '0' && wParam <= '9')
					|| wParam == VK_CANCEL
					|| wParam == VK_CLEAR
					|| wParam == VK_DELETE
					|| wParam == VK_BACK))
				{
					return FALSE;
				}
				break;
			}

			// Make sure we only allow hex numbers and some special keys to delete characters
			if (!((wParam >= '0' && wParam <= '9')
				|| wParam == 'a'
				|| wParam == 'b'
				|| wParam == 'c'
				|| wParam == 'd'
				|| wParam == 'e'
				|| wParam == 'f'
				|| wParam == 'A'
				|| wParam == 'B'
				|| wParam == 'C'
				|| wParam == 'D'
				|| wParam == 'E'
				|| wParam == 'F'
				|| wParam == VK_CANCEL
				|| wParam == VK_CLEAR
				|| wParam == VK_DELETE
				|| wParam == VK_BACK))
			{
				return FALSE;
			}
		}
		break;

		// Don't allow pasting operations, they can be used to bypass the filtering done in WM_CHAR
		case WM_PASTE:
			return FALSE;
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}
