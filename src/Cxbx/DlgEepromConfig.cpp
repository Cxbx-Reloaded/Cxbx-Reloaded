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
// *   Cxbx->Cxbx->DlgEepromConfig.cpp
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

#define _XBOXKRNL_DEFEXTRN_

// prevent name collisions
namespace xboxkrnl
{
	#include <xboxkrnl/xboxkrnl.h> // For XC_VALUE_INDEX and XBOX_EEPROM
};

#include <fstream>
#include "EmuEEPROM.h" // For EEPROMInfo, EEPROMInfos
#include "CxbxKrnl\CxbxKrnl.h"
#include "CxbxKrnl\EmuShared.h"
#include "DlgEepromConfig.h"
#include "ResCxbx.h"


// Generic function called from the gui to display the eeprom menu
void ShowEepromConfig(HWND hwnd);
// Windows dialog procedure for the eeprom menu
static INT_PTR CALLBACK DlgEepromConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
// Indicates that changes have been made in the eeprom menu
static bool g_bHasChanges = false;
// Pointer to the cached eeprom in memory
static uint8_t* pEEPROM_GUI = nullptr;


void* ReadEepromParameter(xboxkrnl::XC_VALUE_INDEX index)
{
	return pEEPROM_GUI + EmuFindEEPROMInfo(index)->value_offset;
}

void ShowEepromConfig(HWND hwnd)
{
	// Reset changes flag
	g_bHasChanges = false;

	// Load the eeprom file from disk and cache it in memory
	if (pEEPROM_GUI == nullptr) {
		std::basic_ifstream<uint8_t> EepromFile(szFilePath_EEPROM_bin, std::ios::binary);
		if (EepromFile.is_open()) {
			pEEPROM_GUI = new uint8_t[EEPROM_SIZE];
			EepromFile.read(pEEPROM_GUI, EEPROM_SIZE);
			EepromFile.close();
		}
		else {
			MessageBox(hwnd, "Couldn't open eeprom file!", "Cxbx-Reloaded", MB_ICONEXCLAMATION | MB_OK);
			return;
		}
	}

	// Show dialog box
	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_EEPROM_CFG), hwnd, DlgEepromConfigProc);
}

INT_PTR CALLBACK DlgEepromConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			// Set window icon
			SetClassLong(hWndDlg, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CXBX)));

			// Initialize the values of the drop-down lists
			for (auto i : { "North America" , "Japan" ,"Rest of the world" , "Manufacturing" }) {
				SendMessage(GetDlgItem(hWndDlg, IDC_EE_XBOX_REGION), CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(i));
			}

			for (auto i : { "NTSC-M, 60Hz" , "NTSC-J, 60Hz", "PAL, 50Hz" }) {
				SendMessage(GetDlgItem(hWndDlg, IDC_EE_AVREGION), CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(i));
			}

			for (auto i : { "English", "Japanese", "German", "French", "Spanish", "Italian", "Korean", "Chinese", "Portuguese" }) {
				SendMessage(GetDlgItem(hWndDlg, IDC_EE_LANGUAGE), CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(i));
			}

			// The available AV settings depend on the current AV region so we must read those beforehand
			for (auto i : { "Normal", "Widescreen", "Letterbox" }) {
				SendMessage(GetDlgItem(hWndDlg, IDC_EE_AVSETTINGS), CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(i));
			}
			if ((*static_cast<uint32_t*>(ReadEepromParameter(xboxkrnl::XC_FACTORY_AV_REGION))) == (AV_FLAGS_50Hz | AV_STANDARD_PAL_I)) {
				// Enable PAL 60Hz since the console is set to the PAL region
				EnableWindow(GetDlgItem(hWndDlg, IDC_EE_PAL60HZ), TRUE);
				EnableWindow(GetDlgItem(hWndDlg, IDC_EE_480P), FALSE);
				EnableWindow(GetDlgItem(hWndDlg, IDC_EE_720P), FALSE);
				EnableWindow(GetDlgItem(hWndDlg, IDC_EE_1080I), FALSE);
				if ((*static_cast<uint32_t*>(ReadEepromParameter(xboxkrnl::XC_VIDEO))) & AV_FLAGS_60Hz) {
					SendMessage(GetDlgItem(hWndDlg, IDC_EE_PAL60HZ), BM_SETCHECK, BST_CHECKED, 0);
				}	
			}
			else {
				// Enable 480p, 720p and 1080i since the console is set to the NTSC region
				uint32_t value = *static_cast<uint32_t*>(ReadEepromParameter(xboxkrnl::XC_VIDEO));
				if (value & AV_FLAGS_HDTV_480p) {
					SendMessage(GetDlgItem(hWndDlg, IDC_EE_480P), BM_SETCHECK, BST_CHECKED, 0);
				}
				else if (value & AV_FLAGS_HDTV_720p) {
					SendMessage(GetDlgItem(hWndDlg, IDC_EE_720P), BM_SETCHECK, BST_CHECKED, 0);
				}
				else if (value & AV_FLAGS_HDTV_1080i) {
					SendMessage(GetDlgItem(hWndDlg, IDC_EE_1080I), BM_SETCHECK, BST_CHECKED, 0);
				}
			}

			for (auto i : {"Stereo", "Stereo, Dolby AC3", "Stereo, DTS", "Stereo, Dolby AC3, DTS", "Mono", "Mono, Dolby AC3", "Mono, DTS",
				 "Mono, Dolby AC3, DTS", "Dolby Surround", "Dolby Surround, Dolby AC3", "Dolby Surround, DTS", "Dolby Surround, Dolby AC3, DTS"}) {
				SendMessage(GetDlgItem(hWndDlg, IDC_EE_AUDIOSETTINGS), CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(i));
			}
			
			for (auto i : {"Rating pending", "Adults only", "Mature", "Teen", "Everyone", "Kids to adults", "Early childhood"}) {
				SendMessage(GetDlgItem(hWndDlg, IDC_EE_GAME_PRTL_CRTL), CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(i));
			}

			for (auto i : {"Max", "NC-17", "A", "5", "PG-13", "PG", "2", "G"}) {
				SendMessage(GetDlgItem(hWndDlg, IDC_EE_MOVIE_PRTL_CRTL), CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(i));
			}

			for (auto i : {"Free", "USA, Canada", "Japan, Europe, Middle East, ...", "Southeast Asia, S. Korea, ...", "Latin America, Oceania",
				"Africa, Russia, N. Korea, ...", "China", "Reserved for future use", "International venues"}) {
				SendMessage(GetDlgItem(hWndDlg, IDC_EE_DVDREGION), CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(i));
			}

			// Select a default parameter in the drop down lists
			for(auto i : { IDC_EE_XBOX_REGION , IDC_EE_AVREGION, IDC_EE_LANGUAGE, IDC_EE_AVSETTINGS, IDC_EE_AUDIOSETTINGS, IDC_EE_GAME_PRTL_CRTL,
				IDC_EE_MOVIE_PRTL_CRTL, IDC_EE_DVDREGION })
			SendMessage(GetDlgItem(hWndDlg, i), CB_SETCURSEL, 0, 0);
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
					EndDialog(hWndDlg, wParam);
					break;
			}
		}
	}
	return FALSE;
}