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
VOID ShowEepromConfig(HWND hwnd);
// Windows dialog procedure for the eeprom menu
static INT_PTR CALLBACK DlgEepromConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
// Indicates that changes have been made in the eeprom menu
static bool g_bHasChanges = false;
// Pointer to the cached eeprom in memory
static uint8_t* pEEPROM_GUI = nullptr;


// Load the eeprom file from disk and cache it in memory
bool LoadEeprom()
{
	std::basic_ifstream<uint8_t> EepromFile(szFilePath_EEPROM_bin, std::ios::binary);
	if (EepromFile.is_open()) {
		pEEPROM_GUI = new uint8_t[EEPROM_SIZE];
		EepromFile.read(pEEPROM_GUI, EEPROM_SIZE);
		EepromFile.close();
		return true;
	}
	return false;
}

VOID ShowEepromConfig(HWND hwnd)
{
	// Reset changes flag
	g_bHasChanges = false;

	// Check to see if an emulation session has been run. This is necessary because games are allowed to change the contents
	// of the eeprom with ExSaveNonVolatileSetting and so we need to reload it from disk
	bool bRefresh;
	g_EmuShared->GetRefreshEepromFlag(&bRefresh);

	if (bRefresh || pEEPROM_GUI == nullptr) {
		if (pEEPROM_GUI) {
			delete[] pEEPROM_GUI;
			pEEPROM_GUI = nullptr;
		}
		if (!LoadEeprom()) {
			MessageBox(hwnd, "Couldn't open eeprom file!", "Cxbx-Reloaded", MB_ICONEXCLAMATION | MB_OK);
			return;
		}
	}

	// Show dialog box
	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_EEPROM_CFG), hwnd, DlgEepromConfigProc);
}
