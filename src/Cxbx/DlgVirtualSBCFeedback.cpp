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
// *   Cxbx->Cxbx->DlgControllerConfig.cpp
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

#include "CxbxKrnl/EmuShared.h"
#include "DlgVirtualSBCFeedback.h"
#include "CxbxKrnl/EmuXapi.h"
#include "Windowsx.h"
#include "Commctrl.h"
#include "ResCxbx.h"

#include <cstdio>

/*! windows dialog procedure */
static INT_PTR CALLBACK DlgVirtualSBCFeedbackProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

/*! changes flag */
static BOOL g_bHasChanges = FALSE;
//handle for this dialog, one one dialog is available for visturl SteelBatallion controller.
HWND g_hDlgVirtualSBCFeedback = 0;
HWND GetDlgVirtualSBCFeedbackHandle(void)
{
    return g_hDlgVirtualSBCFeedback;
}

int DlgItemIDArray[] = {
    IDC_PB_EJECT,
    IDC_PB_COCKPIT_HATCH,
    IDC_PB_IGINITION,
    IDC_PB_START,
    IDC_PB_OPEN_CLOSE,
    IDC_PB_MAP_ZOOM_IN_OUT,
    IDC_PB_MODE_SELECT,
    IDC_PB_SUB_MONITOR_MODE_SELECT,
    IDC_PB_ZOOM_IN,
    IDC_PB_ZOOM_OUT,
    IDC_PB_FSS,
    IDC_PB_MANIPULATOR,
    IDC_PB_LINE_COLOR_CHANGE,
    IDC_PB_WASHING,
    IDC_PB_EXTINGUISHER,
    IDC_PB_CHAFF,
    IDC_PB_TANK_DETACH,
    IDC_PB_OVERRIDE,
    IDC_PB_NIGHT_SCOPE,
    IDC_PB_FUNC1,
    IDC_PB_FUNC2,
    IDC_PB_FUNC3,
    IDC_PB_MAIN_WAPON_CONTROL,
    IDC_PB_SUB_WAPON_CONTROL,
    IDC_PB_MAGAZINE_CHANGE,
    IDC_PB_COM1,
    IDC_PB_COM2,
    IDC_PB_COM3,
    IDC_PB_COM4,
    IDC_PB_COM5,
    0,
    IDC_PB_GEARLEVER_R,
    IDC_PB_GEARLEVER_N,
    IDC_PB_GEARLEVER_1,
    IDC_PB_GEARLEVER_2,
    IDC_PB_GEARLEVER_3,
    IDC_PB_GEARLEVER_4,
    IDC_PB_GEARLEVER_5
};

void UpdateVirtualSBCFeedbackDlg(UCHAR * pXboxSBCFeedback) {
    DWORD NibbleIndex = 0, ByteIndex = 0, FeedbackStatus = 0;
    for (NibbleIndex = 0; NibbleIndex<X_SBC_FEEDBACK_MAX; NibbleIndex++) {
        ByteIndex = NibbleIndex >> 1;
        //UCHAR temp=XTL::XboxSBCFeedbackNames[ByteIndex];
        UCHAR temp = pXboxSBCFeedback[ByteIndex];
        if (NibbleIndex % 2 > 0) {
        FeedbackStatus = (temp & 0xF0) >> 4;
        }
        else
        {
        FeedbackStatus = (temp & 0x0F);
        }
        //printf("%2d: %24s : %2d :%16s                  ", NibbleIndex, XboxSBCFeedbackNames[NibbleIndex], FeedbackStatus, StatusBar[FeedbackStatus]);
        
        if (DlgItemIDArray[NibbleIndex] == 0) {
            continue;
        }
        //set progress bar current position 0
        SendDlgItemMessage(
            g_hDlgVirtualSBCFeedback,    //handle of this dialog
            DlgItemIDArray[NibbleIndex], //Progress Bar ID
            PBM_SETPOS, //Set current position, redraw.
            FeedbackStatus, //position. it out of range, set to closest boundary.
            0          //not used, always 0
        );
    };
}

int ShowVirtualSBCFeedback(HWND hwnd)
{

    if (g_hDlgVirtualSBCFeedback != 0) {
        printf("ShowVirtualSBCFeedback: Dialog already created! only one dialog allowed!\n");
        return -2;
    }
    /*! reset changes flag */
    g_bHasChanges = FALSE;
    INT_PTR nResult=1;
    /*! show dialog box */ //g_hEmuWindow for parent window handle. it's OK to use a NULL as well.
    //nResult=DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_VIRTUAL_SBC_FEEDBACK), hwnd, DlgVirtualSBCFeedbackProc);
    if (!IsWindow(g_hDlgVirtualSBCFeedback))
    {
        g_hDlgVirtualSBCFeedback = CreateDialog(NULL,
                                                MAKEINTRESOURCE(IDD_VIRTUAL_SBC_FEEDBACK),
                                                hwnd,
                                                (DLGPROC)DlgVirtualSBCFeedbackProc);
        ShowWindow(g_hDlgVirtualSBCFeedback, SW_SHOW);
        //SetFocus(hwnd);
    }
    if (nResult == 0) {
        printf("ShowVirtualSBCFeedback: DialogBox: failed! invalid hParent windowd handle!\n");
        return 0;
    }
    else
    if (nResult == -1) {
        printf("ShowVirtualSBCFeedback: DialogBox: failed! try GetLastError!\n");
        return -1;
    }
    return 1;
    
}

INT_PTR CALLBACK DlgVirtualSBCFeedbackProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_INITDIALOG:
        {
            //reserve dialog handle, only one dialog can be created at a time.
            g_hDlgVirtualSBCFeedback = hWndDlg;

            /*! set window icon */
            SetClassLong(hWndDlg, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CXBX)));
            //Load saved configuration from registry.
            //XTL::Load("Software\\Cxbx-Reloaded\\XboxPortHostMapping");//"Software\\Cxbx-Reloaded\\XboxPortHostMapping"
            //Init dialog selections per global array contenst.
            //Button_SetCheck(GetDlgItem(hWndDlg, wXboxToHostTypePORT[port][index]), BST_CHECKED);
            //PostMessage(hWndDlg, WM_COMMAND, wXboxToHostTypePORT[port][index], 0);
            /*
            LRESULT WINAPI SendDlgItemMessage(
                _In_ HWND   hWndDlg,//handle of this dialog
                _In_ int    nIDDlgItem,
                _In_ UINT   Msg,
                _In_ WPARAM wParam,
                _In_ LPARAM lParam
            );
            */
            //Prgoress Control related function:

            //Set progress bar position.
            /*SendDlgItemMessage(
                hWndDlg,    //handle of this dialog
                nIDDlgItem, //Progress Bar ID
                PBM_SETRANGE32, //Set current position,with 32bit value;
                dwRangeMin,     //Minimum range value.By default, the minimum value is zero.
                dwRangeMax      //Maximum range value.This value must be greater than wParam.By default, the maximum value is 100.
            );
            */
            //Set progress bar position.
            /*
            SendDlgItemMessage(
                hWndDlg,    //handle of this dialog
                nIDDlgItem, //Progress Bar ID
                PBM_SETPOS, //Set current position, redraw.
                dwPosition, //position. it out of range, set to closest boundary.
                0;          //not used, always 0
            );
            */
            DWORD NibbleIndex = 0, ByteIndex = 0, FeedbackStatus = 0;
            for (NibbleIndex = 0; NibbleIndex<X_SBC_FEEDBACK_MAX; NibbleIndex++) {
                ByteIndex = NibbleIndex >> 1;
                //UCHAR temp=XTL::XboxSBCFeedbackNames[ByteIndex];
                /*UCHAR temp = pXboxSBCFeedback[ByteIndex];
                if (NibbleIndex % 2 > 0) {
                    FeedbackStatus = (temp & 0xF0) >> 4;
                }
                else
                {
                    FeedbackStatus = (temp & 0x0F);
                }
                printf("%2d: %24s : %2d :%16s                  ", NibbleIndex, XboxSBCFeedbackNames[NibbleIndex], FeedbackStatus, StatusBar[FeedbackStatus]);
                */
                if (DlgItemIDArray[NibbleIndex] == 0) {
                    continue;
                }
                //set progress bar range 0~15
                SendDlgItemMessage(
                    hWndDlg,    //handle of this dialog
                    DlgItemIDArray[NibbleIndex], //Progress Bar ID
                    PBM_SETRANGE32, //Set current position,with 32bit value;
                    0,     //Minimum range value.By default, the minimum value is zero.
                    15      //Maximum range value.This value must be greater than wParam.By default, the maximum value is 100.
                );
                //set progress bar current position 0
                SendDlgItemMessage(
                    hWndDlg,    //handle of this dialog
                    DlgItemIDArray[NibbleIndex], //Progress Bar ID
                    PBM_SETPOS, //Set current position, redraw.
                    0, //position. it out of range, set to closest boundary.
                    0          //not used, always 0
                );
            }
            /*! set default focus to X button */
            //SetFocus(GetDlgItem(hWndDlg, IDC_HOST_APPLY));
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
            /*
            switch(LOWORD(wParam))
            {
            case IDC_HOST_CANCEL:
                //EndDialog(hWndDlg, wParam);
                break;
            case IDC_HOST_APPLY:
                //save configuration to registry.
                //XTL::Save("Software\\Cxbx-Reloaded\\XboxPortHostMapping");
                //EndDialog(hWndDlg, wParam);
                break;
            //set host type and host port in global array xbox to host bridge for xbox port 0
            case IDC_HOST_NOTCONNECT_0_0:
                XTL::SetXboxPortToHostPort(0, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_NOTCONNECT, 0);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_0_0:
                XTL::SetXboxPortToHostPort(0, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 0);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_0_1:
                XTL::SetXboxPortToHostPort(0, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 1);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_0_2:
                XTL::SetXboxPortToHostPort(0, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 2);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_0_3:
                XTL::SetXboxPortToHostPort(0, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 3);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_DINPUT_0_0:
                XTL::SetXboxPortToHostPort(0, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_DINPUT, 0);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_VIRTUAL_SBC_0_0:
                XTL::SetXboxPortToHostPort(0, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_VIRTUAL_SBC, 0);
                g_bHasChanges = TRUE;
                break;
                //set host type and host port in global array xbox to host bridge for xbox port 1
            case IDC_HOST_NOTCONNECT_1_0:
                XTL::SetXboxPortToHostPort(1, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_NOTCONNECT, 0);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_1_0:
                XTL::SetXboxPortToHostPort(1, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 0);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_1_1:
                XTL::SetXboxPortToHostPort(1, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 1);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_1_2:
                XTL::SetXboxPortToHostPort(1, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 2);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_1_3:
                XTL::SetXboxPortToHostPort(1, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 3);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_DINPUT_1_0:
                XTL::SetXboxPortToHostPort(1, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_DINPUT, 0);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_VIRTUAL_SBC_1_0:
                XTL::SetXboxPortToHostPort(1, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_VIRTUAL_SBC, 0);
                g_bHasChanges = TRUE;
                break;
                //set host type and host port in global array xbox to host bridge for xbox port 2
            case IDC_HOST_NOTCONNECT_2_0:
                XTL::SetXboxPortToHostPort(2, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_NOTCONNECT, 0);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_2_0:
                XTL::SetXboxPortToHostPort(2, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 0);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_2_1:
                XTL::SetXboxPortToHostPort(2, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 1);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_2_2:
                XTL::SetXboxPortToHostPort(2, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 2);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_2_3:
                XTL::SetXboxPortToHostPort(2, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 3);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_DINPUT_2_0:
                XTL::SetXboxPortToHostPort(2, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_DINPUT, 0);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_VIRTUAL_SBC_2_0:
                XTL::SetXboxPortToHostPort(2, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_VIRTUAL_SBC, 0);
                g_bHasChanges = TRUE;
                break;
                //set host type and host port in global array xbox to host bridge for xbox port 3
            case IDC_HOST_NOTCONNECT_3_0:
                XTL::SetXboxPortToHostPort(3, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_NOTCONNECT, 0);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_3_0:
                XTL::SetXboxPortToHostPort(3, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 0);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_3_1:
                XTL::SetXboxPortToHostPort(3, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 1);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_3_2:
                XTL::SetXboxPortToHostPort(3, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 2);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_XINPUT_3_3:
                XTL::SetXboxPortToHostPort(3, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT, 3);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_DINPUT_3_0:
                XTL::SetXboxPortToHostPort(3, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_DINPUT, 0);
                g_bHasChanges = TRUE;
                break;
            case IDC_HOST_VIRTUAL_SBC_3_0:
                XTL::SetXboxPortToHostPort(3, X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_VIRTUAL_SBC, 0);
                g_bHasChanges = TRUE;
                break;
            }
            */
        }
        break;
    }
    return FALSE;
}
