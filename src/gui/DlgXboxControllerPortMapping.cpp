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
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *
// *  All rights reserved
// *
// ******************************************************************

// Without this, you'll get a ton of errors from the std library for some unknown reason...
#include "Logging.h"

#include "common\Settings.hpp" // for g_Settings

#include "DlgXboxControllerPortMapping.h"
#include "../Common/Win32/XBPortMapping.h"
#include "Windowsx.h"
#include "resource/ResCxbx.h"
#include "core\kernel\support\EmuXTL.h"


/*! windows dialog procedure */
static INT_PTR CALLBACK DlgXboxControllerPortMappingProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
/*! changes flag */
static BOOL g_bHasChanges = FALSE;
static WPARAM wXboxToHostTypePORT[4][7] = {
    { IDC_HOST_NOTCONNECT_0_0 ,IDC_HOST_XINPUT_0_0,IDC_HOST_XINPUT_0_1,IDC_HOST_XINPUT_0_2,IDC_HOST_XINPUT_0_3,IDC_HOST_DINPUT_0_0,IDC_HOST_VIRTUAL_SBC_0_0 },
    { IDC_HOST_NOTCONNECT_1_0 ,IDC_HOST_XINPUT_1_0,IDC_HOST_XINPUT_1_1,IDC_HOST_XINPUT_1_2,IDC_HOST_XINPUT_1_3,IDC_HOST_DINPUT_1_0,IDC_HOST_VIRTUAL_SBC_1_0 },
    { IDC_HOST_NOTCONNECT_2_0 ,IDC_HOST_XINPUT_2_0,IDC_HOST_XINPUT_2_1,IDC_HOST_XINPUT_2_2,IDC_HOST_XINPUT_2_3,IDC_HOST_DINPUT_2_0,IDC_HOST_VIRTUAL_SBC_2_0 },
    { IDC_HOST_NOTCONNECT_3_0 ,IDC_HOST_XINPUT_3_0,IDC_HOST_XINPUT_3_1,IDC_HOST_XINPUT_3_2,IDC_HOST_XINPUT_3_3,IDC_HOST_DINPUT_3_0,IDC_HOST_VIRTUAL_SBC_3_0 }
};

void ShowXboxControllerPortMappingConfig(HWND hwnd)
{
    /*! reset changes flag */
    g_bHasChanges = FALSE;

    /*! show dialog box */
    DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_CONTROLLER_HOST_MAPPING), hwnd, DlgXboxControllerPortMappingProc);
}

INT_PTR CALLBACK DlgXboxControllerPortMappingProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_INITDIALOG:
        {
            /*! set window icon */
            SetClassLong(hWndDlg, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CXBX)));
            // Load configuration from settings.
            XBPortMappingSet(g_Settings->m_controller_port);
            //Init dialog selections per global array contenst.
            XTL::DWORD port = 0;
            int index = 0;
            XTL::DWORD dwHostType = 1;
            XTL::DWORD dwHostPort = 0;
            for (port = 0; port < 4; port++) {
                GetXboxPortToHostPort(port, dwHostType, dwHostPort);
                switch (dwHostType) {
                case X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_NOTCONNECT:
                    index = 0;
                    break;
                case X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT:
                    index = 1 + dwHostPort;
                    break;
                case X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_DINPUT:
                    index = 5;
                    break;
                case X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_VIRTUAL_SBC:
                    index = 6;
                    break;

                default:
                    index = 0;
                    break;
                }
                Button_SetCheck(GetDlgItem(hWndDlg, wXboxToHostTypePORT[port][index]), BST_CHECKED);
                PostMessage(hWndDlg, WM_COMMAND, wXboxToHostTypePORT[port][index], 0);
            }


            /*! set default focus to X button */
            SetFocus(GetDlgItem(hWndDlg, IDC_HOST_APPLY));
        }
        break;

        case WM_CLOSE:
        {
            /*! if changes have been made, check if the user wants to save them */
            if(g_bHasChanges)
            {
                int ret = MessageBox(hWndDlg, "Do you wish to apply your changes?", "Cxbx-Reloaded", MB_ICONQUESTION | MB_YESNOCANCEL);

                switch(ret)
                {
                    case IDYES:
                        PostMessage(hWndDlg, WM_COMMAND, IDC_HOST_APPLY, 0);
                        break;
                    case IDNO:
                        PostMessage(hWndDlg, WM_COMMAND, IDC_HOST_CANCEL, 0);
                        break;
                }
                break;
            }

            PostMessage(hWndDlg, WM_COMMAND, IDC_INPUT_CONFIG_CANCEL, 0);
        }
        break;

        case WM_COMMAND:
        {
            HWND hWndButton = GetDlgItem(hWndDlg, LOWORD(wParam));

            switch(LOWORD(wParam))
            {
            case IDC_HOST_CANCEL:
                EndDialog(hWndDlg, wParam);
                break;
            case IDC_HOST_APPLY:
                // Save configuration to settings.
                XBPortMappingGet(g_Settings->m_controller_port);
                EndDialog(hWndDlg, wParam);
                break;
            //set host type and host port in global array xbox to host bridge for xbox port 0
            case IDC_HOST_NOTCONNECT_0_0:
                SetXboxPortToHostPort(0, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_NOTCONNECT, 0);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_0_0:
                SetXboxPortToHostPort(0, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 0);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_0_1:
                SetXboxPortToHostPort(0, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 1);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_0_2:
                SetXboxPortToHostPort(0, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 2);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_0_3:
                SetXboxPortToHostPort(0, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 3);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_DINPUT_0_0:
                SetXboxPortToHostPort(0, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_DINPUT, 0);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_VIRTUAL_SBC_0_0:
                SetXboxPortToHostPort(0, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_VIRTUAL_SBC, 0);
                g_bHasChanges = TRUE;
                break;
                //set host type and host port in global array xbox to host bridge for xbox port 1
            case IDC_HOST_NOTCONNECT_1_0:
                SetXboxPortToHostPort(1, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_NOTCONNECT, 0);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_1_0:
                SetXboxPortToHostPort(1, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 0);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_1_1:
                SetXboxPortToHostPort(1, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 1);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_1_2:
                SetXboxPortToHostPort(1, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 2);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_1_3:
                SetXboxPortToHostPort(1, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 3);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_DINPUT_1_0:
                SetXboxPortToHostPort(1, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_DINPUT, 0);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_VIRTUAL_SBC_1_0:
                SetXboxPortToHostPort(1, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_VIRTUAL_SBC, 0);
                g_bHasChanges = TRUE;
                break;
                //set host type and host port in global array xbox to host bridge for xbox port 2
            case IDC_HOST_NOTCONNECT_2_0:
                SetXboxPortToHostPort(2, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_NOTCONNECT, 0);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_2_0:
                SetXboxPortToHostPort(2, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 0);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_2_1:
                SetXboxPortToHostPort(2, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 1);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_2_2:
                SetXboxPortToHostPort(2, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 2);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_2_3:
                SetXboxPortToHostPort(2, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 3);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_DINPUT_2_0:
                SetXboxPortToHostPort(2, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_DINPUT, 0);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_VIRTUAL_SBC_2_0:
                SetXboxPortToHostPort(2, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_VIRTUAL_SBC, 0);
                g_bHasChanges = TRUE;
                break;
                //set host type and host port in global array xbox to host bridge for xbox port 3
            case IDC_HOST_NOTCONNECT_3_0:
                SetXboxPortToHostPort(3, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_NOTCONNECT, 0);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_3_0:
                SetXboxPortToHostPort(3, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 0);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_3_1:
                SetXboxPortToHostPort(3, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 1);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_3_2:
                SetXboxPortToHostPort(3, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 2);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_3_3:
                SetXboxPortToHostPort(3, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 3);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_DINPUT_3_0:
                SetXboxPortToHostPort(3, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_DINPUT, 0);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_VIRTUAL_SBC_3_0:
                SetXboxPortToHostPort(3, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_VIRTUAL_SBC, 0);
                g_bHasChanges = TRUE;
                break;
            }
        }
        break;
    }
    return FALSE;
}
