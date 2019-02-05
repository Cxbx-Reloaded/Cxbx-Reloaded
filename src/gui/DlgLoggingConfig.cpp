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

#include "Logging.h"
#include "EmuShared.h"
#include "DlgLoggingConfig.h"
#include "resource/ResCxbx.h"
#include "common\IPCHybrid.hpp"


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
	unsigned int index;
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			HWND hHandle;
			int counter;
			int TempLevel;
			unsigned int LoggedModules[NUM_INTEGERS_LOG];
			int LogLevel;

			// Set window icon
			SetClassLong(hWndDlg, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CXBX)));

			LoggedModules[0] = g_Settings->m_core.LoggedModules[0];
			LoggedModules[1] = g_Settings->m_core.LoggedModules[1];
			LogLevel = g_Settings->m_core.LogLevel;

			hHandle = GetDlgItem(hWndDlg, IDC_EVENT_LV);
			TempLevel = to_underlying(LOG_LEVEL::DEBUG);
			for (auto i : { "Debug" , "Info" ,"Warning", "Fatal" }) {
				LRESULT index = SendMessage(hHandle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(i));
				SendMessage(hHandle, CB_SETITEMDATA, index, TempLevel);
				TempLevel++;
			}
			TempLevel = to_underlying(LOG_LEVEL::DEBUG);
			for (; TempLevel < to_underlying(LOG_LEVEL::MAX); TempLevel++) {
				if (TempLevel == LogLevel) {
					SendMessage(hHandle, CB_SETCURSEL, TempLevel, 0);
					break;
				}
			}

			counter = 0;
			for (index = to_underlying(CXBXR_MODULE::CXBXR); index < to_underlying(CXBXR_MODULE::KRNL); index++) {
				if (LoggedModules[index / 32] & (1 << (index % 32))) {
					SendMessage(GetDlgItem(hWndDlg, g_DlgIndexes[index]), BM_SETCHECK, BST_CHECKED, 0);
					counter++;
				}
				else {
					SendMessage(GetDlgItem(hWndDlg, g_DlgIndexes[index]), BM_SETCHECK, BST_UNCHECKED, 0);
				}
			}

			if (counter == to_underlying(CXBXR_MODULE::KRNL)) {
				for (index = to_underlying(CXBXR_MODULE::CXBXR); index < to_underlying(CXBXR_MODULE::KRNL); index++) {
					EnableWindow(GetDlgItem(hWndDlg, g_DlgIndexes[index]), FALSE);
				}
				SendMessage(GetDlgItem(hWndDlg, IDC_LOG_ENABLE_GENERAL), BM_SETCHECK, BST_CHECKED, 0);
			}
			else if (counter == 0) {
				for (index = to_underlying(CXBXR_MODULE::CXBXR); index < to_underlying(CXBXR_MODULE::KRNL); index++) {
					EnableWindow(GetDlgItem(hWndDlg, g_DlgIndexes[index]), FALSE);
				}
				SendMessage(GetDlgItem(hWndDlg, IDC_LOG_DISABLE_GENERAL), BM_SETCHECK, BST_CHECKED, 0);
			}
			else {
				SendMessage(GetDlgItem(hWndDlg, IDC_LOG_CUSTOM_GENERAL), BM_SETCHECK, BST_CHECKED, 0);
			}

			counter = 0;
			for (index = to_underlying(CXBXR_MODULE::KRNL); index < to_underlying(CXBXR_MODULE::MAX); index++) {
				if (LoggedModules[index / 32] & (1 << (index % 32))) {
					SendMessage(GetDlgItem(hWndDlg, g_DlgIndexes[index]), BM_SETCHECK, BST_CHECKED, 0);
					counter++;
				}
				else {
					SendMessage(GetDlgItem(hWndDlg, g_DlgIndexes[index]), BM_SETCHECK, BST_UNCHECKED, 0);
				}
			}

			if (counter == (to_underlying(CXBXR_MODULE::MAX) - to_underlying(CXBXR_MODULE::KRNL))) {
				for (index = to_underlying(CXBXR_MODULE::KRNL); index < to_underlying(CXBXR_MODULE::MAX); index++) {
					EnableWindow(GetDlgItem(hWndDlg, g_DlgIndexes[index]), FALSE);
				}
				SendMessage(GetDlgItem(hWndDlg, IDC_LOG_ENABLE_KERNEL), BM_SETCHECK, BST_CHECKED, 0);
			}
			else if (counter == 0) {
				for (index = to_underlying(CXBXR_MODULE::KRNL); index < to_underlying(CXBXR_MODULE::MAX); index++) {
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
			PostMessage(hWndDlg, WM_COMMAND, IDC_LOG_CANCEL, 0);
		}
		break;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDC_LOG_CANCEL:
				{
					g_ChildWnd = NULL;
					EndDialog(hWndDlg, wParam);
				}
				break;

				case IDC_LOG_ACCEPT:
				{
					if (g_bHasChanges) {
						unsigned int LoggedModules[NUM_INTEGERS_LOG] = { 0 };
						HWND hControl = GetDlgItem(hWndDlg, IDC_EVENT_LV);
						int LogLevel = SendMessage(hControl, CB_GETITEMDATA, SendMessage(hControl, CB_GETCURSEL, 0, 0), 0);

						for (index = to_underlying(CXBXR_MODULE::CXBXR); index < to_underlying(CXBXR_MODULE::MAX); index++) {
							if (SendMessage(GetDlgItem(hWndDlg, g_DlgIndexes[index]), BM_GETCHECK, 0, 0) == BST_CHECKED) {
								LoggedModules[index / 32] |= (1 << (index % 32));
							}
						}

						g_Settings->m_core.LoggedModules[0] = LoggedModules[0];
						g_Settings->m_core.LoggedModules[1] = LoggedModules[1];
						g_Settings->m_core.LogLevel = LogLevel;

						// Update the logging variables for the GUI process
						log_set_config(LogLevel, LoggedModules);
						log_generate_active_filter_output(CXBXR_MODULE::GUI);

						// Also inform the kernel process if it exists
						if (g_ChildWnd) {
							// Sync updated log to kernel process to use run-time settings.
							g_EmuShared->SetLogLv(&LogLevel);
							g_EmuShared->SetLogModules(LoggedModules);
							ipc_send_kernel_update(IPC_UPDATE_KERNEL::CONFIG_LOGGING_SYNC, 0, reinterpret_cast<std::uintptr_t>(g_ChildWnd));
						}
					}
					PostMessage(hWndDlg, WM_COMMAND, IDC_LOG_CANCEL, 0);
				}
				break;

				case IDC_EVENT_LV:
					if (HIWORD(wParam) == CBN_SELCHANGE) {
						g_bHasChanges = true;
					}
					break;

				case IDC_LOG_ENABLE_GENERAL: {
					if (HIWORD(wParam) == BN_CLICKED) {
						for (index = to_underlying(CXBXR_MODULE::CXBXR); index < to_underlying(CXBXR_MODULE::KRNL);
							index++) {
							SendMessage(GetDlgItem(hWndDlg, g_DlgIndexes[index]), BM_SETCHECK, BST_CHECKED, 0);
							EnableWindow(GetDlgItem(hWndDlg, g_DlgIndexes[index]), FALSE);
						}
						g_bHasChanges = true;
					}
				}
				break;

				case IDC_LOG_DISABLE_GENERAL: {
					if (HIWORD(wParam) == BN_CLICKED) {
						for (index = to_underlying(CXBXR_MODULE::CXBXR); index < to_underlying(CXBXR_MODULE::KRNL);
							index++) {
							SendMessage(GetDlgItem(hWndDlg, g_DlgIndexes[index]), BM_SETCHECK, BST_UNCHECKED, 0);
							EnableWindow(GetDlgItem(hWndDlg, g_DlgIndexes[index]), FALSE);
						}
						g_bHasChanges = true;
					}
				}
				break;

				case IDC_LOG_ENABLE_KERNEL: {
					if (HIWORD(wParam) == BN_CLICKED) {
						for (index = to_underlying(CXBXR_MODULE::KRNL); index < to_underlying(CXBXR_MODULE::MAX);
							index++) {
							SendMessage(GetDlgItem(hWndDlg, g_DlgIndexes[index]), BM_SETCHECK, BST_CHECKED, 0);
							EnableWindow(GetDlgItem(hWndDlg, g_DlgIndexes[index]), FALSE);
						}
						g_bHasChanges = true;
					}
				}
				break;

				case IDC_LOG_DISABLE_KERNEL: {
					if (HIWORD(wParam) == BN_CLICKED) {
						for (index = to_underlying(CXBXR_MODULE::KRNL); index < to_underlying(CXBXR_MODULE::MAX);
							index++) {
							SendMessage(GetDlgItem(hWndDlg, g_DlgIndexes[index]), BM_SETCHECK, BST_UNCHECKED, 0);
							EnableWindow(GetDlgItem(hWndDlg, g_DlgIndexes[index]), FALSE);
						}
						g_bHasChanges = true;
					}
				}
				break;

				case IDC_LOG_CUSTOM_GENERAL: {
					if (HIWORD(wParam) == BN_CLICKED) {
						for (index = to_underlying(CXBXR_MODULE::CXBXR); index < to_underlying(CXBXR_MODULE::KRNL);
							index++) {
							EnableWindow(GetDlgItem(hWndDlg, g_DlgIndexes[index]), TRUE);
						}
						g_bHasChanges = true;
					}
				}
				break;

				case IDC_LOG_CUSTOM_KERNEL: {
					if (HIWORD(wParam) == BN_CLICKED) {
						for (index = to_underlying(CXBXR_MODULE::KRNL); index < to_underlying(CXBXR_MODULE::MAX);
							index++) {
							EnableWindow(GetDlgItem(hWndDlg, g_DlgIndexes[index]), TRUE);
						}
						g_bHasChanges = true;
					}
				}
				break;

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
