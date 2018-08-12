// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Cxbx->DlgLoggingConfig.h
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

#include "Logging.h"
#include "CxbxKrnl/EmuShared.h"
#include "DlgLoggingConfig.h"
#include "ResCxbx.h"


static bool g_bHasChanges = false;
static HWND g_ChildWnd = NULL;
INT_PTR CALLBACK DlgLogConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

static int g_DlgIndexes[] = {
	// General
	IDC_LOG_CXBXR,
	IDC_LOG_XBE,
	IDC_LOG_INIT,
	IDC_LOG_VMEM,
	IDC_LOG_PMEM,
	IDC_LOG_GUI,
	IDC_LOG_EEPR,
	IDC_LOG_RSA,
	IDC_LOG_POOLMEM,
	IDC_LOG_D3D8,
	IDC_LOG_D3DST,
	IDC_LOG_D3DCVT,
	IDC_LOG_DSOUND,
	IDC_LOG_XAPI,
	IDC_LOG_XACT,
	IDC_LOG_XGRP,
	IDC_LOG_XONLINE,
	IDC_LOG_FS,
	IDC_LOG_PSHB,
	IDC_LOG_PXSH,
	IDC_LOG_VTXSH,
	IDC_LOG_VTXB,
	IDC_LOG_DINP,
	IDC_LOG_XINP,
	IDC_LOG_SDL2,
	IDC_LOG_FILE,
	IDC_LOG_X86,
	IDC_LOG_HLE,
	IDC_LOG_NET,
	IDC_LOG_MCPX,
	IDC_LOG_NV2A,
	IDC_LOG_SMC,
	IDC_LOG_OHCI,
	IDC_LOG_USB,
	IDC_LOG_HUB,
	IDC_LOG_XIDCTRL,
	IDC_LOG_ADM,
	// Kernel
	IDC_LOG_KRNL,
	IDC_LOG_LOG,
	IDC_LOG_XBOX,
	IDC_LOG_XBDM,
	IDC_LOG_AV,
	IDC_LOG_DBG,
	IDC_LOG_EX,
	IDC_LOG_FSC,
	IDC_LOG_HAL,
	IDC_LOG_IO,
	IDC_LOG_KD,
	IDC_LOG_KE,
	IDC_LOG_KI,
	IDC_LOG_MM,
	IDC_LOG_NT,
	IDC_LOG_OB,
	IDC_LOG_PS,
	IDC_LOG_RTL,
	IDC_LOG_XC,
	IDC_LOG_XE
};


VOID ShowLoggingConfig(HWND hwnd, HWND ChildWnd)
{
	g_ChildWnd = ChildWnd;
	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_LOGGING_CFG), hwnd, DlgLogConfigProc);
}

INT_PTR CALLBACK DlgLogConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			HWND hHandle;
			int counter;
			unsigned int index;
			unsigned int TempLevel;
			unsigned int LoggedModules[NUM_INTEGERS_LOG];
			unsigned int LogLevel;

			// Set window icon
			SetClassLong(hWndDlg, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CXBX)));

			LoggedModules[0] = g_Settings->m_core.LoggedModules[0];
			LoggedModules[1] = g_Settings->m_core.LoggedModules[1];
			LogLevel = g_Settings->m_core.LogLevel;

			hHandle = GetDlgItem(hWndDlg, IDC_EVENT_LV);
			TempLevel = static_cast<unsigned int>(LOG_LEVEL::DEBUG);
			for (auto i : { "Debug" , "Info" ,"Warning", "Fatal" }) {
				LRESULT index = SendMessage(hHandle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(i));
				SendMessage(hHandle, CB_SETITEMDATA, index, TempLevel);
				TempLevel++;
			}
			TempLevel -= static_cast<unsigned int>(LOG_LEVEL::FATAL);
			for (; TempLevel < static_cast<unsigned int>(LOG_LEVEL::MAX); TempLevel++) {
				if (TempLevel == LogLevel) {
					SendMessage(hHandle, CB_SETCURSEL, TempLevel, 0);
					break;
				}
			}

			counter = 0;
			for (index = static_cast<unsigned int>(CXBXR_MODULE::CXBXR); index < static_cast<unsigned int>(CXBXR_MODULE::KRNL); index++) {
				if (LoggedModules[index / 32] & (1 << index)) {
					SendMessage(GetDlgItem(hWndDlg, g_DlgIndexes[index]), BM_SETCHECK, BST_CHECKED, 0);
					counter++;
				}
				else {
					SendMessage(GetDlgItem(hWndDlg, g_DlgIndexes[index]), BM_SETCHECK, BST_UNCHECKED, 0);
				}
			}

			if (counter == static_cast<unsigned int>(CXBXR_MODULE::KRNL)) {
				for (index = static_cast<unsigned int>(CXBXR_MODULE::CXBXR); index < static_cast<unsigned int>(CXBXR_MODULE::KRNL); index++) {
					EnableWindow(GetDlgItem(hWndDlg, g_DlgIndexes[index]), FALSE);
				}
				SendMessage(GetDlgItem(hWndDlg, IDC_LOG_ENABLE_GENERAL), BM_SETCHECK, BST_CHECKED, 0);
			}
			else if (counter == 0) {
				for (index = static_cast<unsigned int>(CXBXR_MODULE::CXBXR); index < static_cast<unsigned int>(CXBXR_MODULE::KRNL); index++) {
					EnableWindow(GetDlgItem(hWndDlg, g_DlgIndexes[index]), FALSE);
				}
				SendMessage(GetDlgItem(hWndDlg, IDC_LOG_DISABLE_GENERAL), BM_SETCHECK, BST_CHECKED, 0);
			}
			else {
				SendMessage(GetDlgItem(hWndDlg, IDC_LOG_CUSTOM_GENERAL), BM_SETCHECK, BST_CHECKED, 0);
			}

			counter = 0;
			for (index = static_cast<unsigned int>(CXBXR_MODULE::KRNL); index < static_cast<unsigned int>(CXBXR_MODULE::MAX); index++) {
				if (LoggedModules[index / 32] & (1 << index)) {
					SendMessage(GetDlgItem(hWndDlg, g_DlgIndexes[index]), BM_SETCHECK, BST_CHECKED, 0);
					counter++;
				}
				else {
					SendMessage(GetDlgItem(hWndDlg, g_DlgIndexes[index]), BM_SETCHECK, BST_UNCHECKED, 0);
				}
			}

			if (counter == static_cast<unsigned int>(CXBXR_MODULE::MAX)) {
				for (index = static_cast<unsigned int>(CXBXR_MODULE::KRNL); index < static_cast<unsigned int>(CXBXR_MODULE::MAX); index++) {
					EnableWindow(GetDlgItem(hWndDlg, g_DlgIndexes[index]), FALSE);
				}
				SendMessage(GetDlgItem(hWndDlg, IDC_LOG_ENABLE_KERNEL), BM_SETCHECK, BST_CHECKED, 0);
			}
			else if (counter == 0) {
				for (index = static_cast<unsigned int>(CXBXR_MODULE::KRNL); index < static_cast<unsigned int>(CXBXR_MODULE::KRNL); index++) {
					EnableWindow(GetDlgItem(hWndDlg, g_DlgIndexes[index]), FALSE);
				}
				SendMessage(GetDlgItem(hWndDlg, IDC_LOG_DISABLE_KERNEL), BM_SETCHECK, BST_CHECKED, 0);
			}
			else {
				SendMessage(GetDlgItem(hWndDlg, IDC_LOG_CUSTOM_KERNEL), BM_SETCHECK, BST_CHECKED, 0);
			}

			// Reset changes flag
			g_bHasChanges = false;
		}
		break;

		case WM_CLOSE:
		{
			if (g_bHasChanges) {
				unsigned int LoggedModules[NUM_INTEGERS_LOG] = { 0 };
				HWND hControl = GetDlgItem(hWndDlg, IDC_EVENT_LV);
				unsigned int LogLevel = SendMessage(hControl, CB_GETITEMDATA, SendMessage(hControl, CB_GETCURSEL, 0, 0), 0);
				COPYDATASTRUCT CopyData;

				for (unsigned int index = static_cast<unsigned int>(CXBXR_MODULE::CXBXR); index < static_cast<unsigned int>(CXBXR_MODULE::MAX); index++) {
					if (SendMessage(GetDlgItem(hWndDlg, g_DlgIndexes[index]), BM_GETCHECK, 0, 0) == BST_CHECKED) {
						LoggedModules[index / 32] |= (1 << index);
					}
				}
				
				g_Settings->m_core.LoggedModules[0] = LoggedModules[0];
				g_Settings->m_core.LoggedModules[1] = LoggedModules[1];
				g_Settings->m_core.LogLevel = LogLevel;

				// Update the logging variables for the GUI process
				g_CurrentLogLevel = LogLevel;
				for (unsigned int index = static_cast<unsigned int>(CXBXR_MODULE::CXBXR); index < static_cast<unsigned int>(CXBXR_MODULE::MAX); index++) {
					if (LoggedModules[index / 32] & (1 << index)) {
						g_EnabledModules[index] = true;
					}
					else {
						g_EnabledModules[index] = false;
					}
				}

				// Also inform the kernel process if it exists
				if (g_ChildWnd) {
					LogData Data;
					Data.Level = LogLevel;
					std::memcpy(Data.LoggedModules, LoggedModules, NUM_INTEGERS_LOG);
					CopyData.dwData = LOG_ID;
					CopyData.cbData = sizeof(LogData);
					CopyData.lpData = &Data;
					SendMessage(g_ChildWnd, WM_COPYDATA, reinterpret_cast<WPARAM>(hWndDlg), reinterpret_cast<LPARAM>(&CopyData));
				}
			}

			g_ChildWnd = NULL;
			EndDialog(hWndDlg, wParam);
		}
		break;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDC_EVENT_LV:
					if (HIWORD(wParam) == CBN_SELCHANGE) {
						g_bHasChanges = true;
					}
					break;

				case IDC_LOG_ENABLE_GENERAL:
				case IDC_LOG_DISABLE_GENERAL:
				case IDC_LOG_ENABLE_KERNEL:
				case IDC_LOG_DISABLE_KERNEL:
				case IDC_LOG_CUSTOM_GENERAL:
				case IDC_LOG_CUSTOM_KERNEL:
				case IDC_LOG_CXBXR:  
				case IDC_LOG_XBE:
				case IDC_LOG_INIT: 
				case IDC_LOG_VMEM:       
				case IDC_LOG_PMEM:       
				case IDC_LOG_GUI:        
				case IDC_LOG_EEPR:       
				case IDC_LOG_RSA:        
				case IDC_LOG_POOLMEM:    
				case IDC_LOG_D3D8:       
				case IDC_LOG_D3DST:      
				case IDC_LOG_D3DCVT:     
				case IDC_LOG_DSOUND:     
				case IDC_LOG_XAPI:       
				case IDC_LOG_XACT:       
				case IDC_LOG_XGRP:       
				case IDC_LOG_XONLINE:    
				case IDC_LOG_FS:         
				case IDC_LOG_PSHB:       
				case IDC_LOG_PXSH:       
				case IDC_LOG_VTXSH:      
				case IDC_LOG_VTXB:       
				case IDC_LOG_DINP:       
				case IDC_LOG_XINP:
				case IDC_LOG_SDL2:       
				case IDC_LOG_FILE:       
				case IDC_LOG_X86:        
				case IDC_LOG_HLE:        
				case IDC_LOG_NET:        
				case IDC_LOG_MCPX:       
				case IDC_LOG_NV2A:       
				case IDC_LOG_SMC:        
				case IDC_LOG_OHCI:       
				case IDC_LOG_USB:        
				case IDC_LOG_HUB:        
				case IDC_LOG_XIDCTRL:    
				case IDC_LOG_ADM:        
				case IDC_LOG_KRNL:       
				case IDC_LOG_LOG:        
				case IDC_LOG_XBOX:       
				case IDC_LOG_XBDM:       
				case IDC_LOG_AV:         
				case IDC_LOG_DBG:        
				case IDC_LOG_EX:         
				case IDC_LOG_FSC:        
				case IDC_LOG_HAL:        
				case IDC_LOG_IO:         
				case IDC_LOG_KD:         
				case IDC_LOG_KE:         
				case IDC_LOG_KI:         
				case IDC_LOG_MM:         
				case IDC_LOG_NT:         
				case IDC_LOG_OB:         
				case IDC_LOG_PS:         
				case IDC_LOG_RTL:        
				case IDC_LOG_XC:         
				case IDC_LOG_XE:
					if (HIWORD(wParam) == BN_CLICKED) {
						g_bHasChanges = true;
					}
					break;
			}
		}
		break;

	}
	return FALSE;
}
