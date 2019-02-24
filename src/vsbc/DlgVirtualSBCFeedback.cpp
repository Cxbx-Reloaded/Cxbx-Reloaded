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
#if 0

IDD_VIRTUAL_SBC_FEEDBACK DIALOGEX 0, 0, 1039, 214
STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Virtual SteelBattalion Controller"
FONT 8, "MS Shell Dlg", 400, 0, 0x1
BEGIN
GROUPBOX        "Left Block", IDC_LEFTBLOCK, 7, 7, 329, 200, 0, WS_EX_CLIENTEDGE | WS_EX_RIGHT
GROUPBOX        "Left Block", IDC_LEFTBLOCK2, 7, 7, 329, 200, 0, WS_EX_RIGHT
GROUPBOX        "Right Block", IDC_RIGHTBLOCK, 703, 7, 329, 200, 0, WS_EX_CLIENTEDGE
GROUPBOX        "Gear Lever", IDC_GEAR_LEVER, 24, 25, 92, 174, WS_GROUP, WS_EX_CLIENTEDGE
GROUPBOX        "Toggle Switches", IDC_STATIC, 131, 102, 191, 95, 0, WS_EX_CLIENTEDGE
CONTROL         "N", IDC_GEAR_0, "Button", BS_AUTORADIOBUTTON | BS_LEFTTEXT, 37, 131, 21, 10
CONTROL         "1", IDC_GEAR_1, "Button", BS_AUTORADIOBUTTON | BS_LEFTTEXT, 38, 113, 20, 10
CONTROL         "2", IDC_GEAR_2, "Button", BS_AUTORADIOBUTTON | BS_LEFTTEXT, 38, 95, 20, 10
CONTROL         "3", IDC_GEAR_3, "Button", BS_AUTORADIOBUTTON | BS_LEFTTEXT, 38, 77, 20, 10
CONTROL         "4", IDC_GEAR_4, "Button", BS_AUTORADIOBUTTON | BS_LEFTTEXT, 38, 59, 20, 10
CONTROL         "5", IDC_GEAR_5, "Button", BS_AUTORADIOBUTTON | BS_LEFTTEXT, 38, 41, 20, 10
CONTROL         "R", IDC_GEAR_R, "Button", BS_AUTORADIOBUTTON | BS_LEFTTEXT, 37, 149, 21, 10
CONTROL         "", IDC_ROTATION_LEVER, "msctls_trackbar32", TBS_BOTH | TBS_NOTICKS | WS_TABSTOP, 129, 38, 188, 15
CTEXT           "Rotation Lever", IDC_TXT_ROTATION_LEVER, 199, 27, 48, 8
CONTROL         "VT-LOCATION MEASUREMENT", IDC_VT_LOCATION_MEASUREMENT,
"Button", BS_AUTOCHECKBOX | WS_TABSTOP, 208, 120, 113, 10
CONTROL         "BUFFER MATERIAL", IDC_BUFFER_MATERIAL, "Button", BS_AUTOCHECKBOX | WS_TABSTOP, 208, 150, 77, 10
CONTROL         "FUEL FLOW RATE", IDC_FUEL_FLOW_RATE, "Button", BS_AUTOCHECKBOX | WS_TABSTOP, 208, 180, 73, 10
CONTROL         "OXYGEN SUPPLY SYSTEM", IDC_OXYGEN_SUPPLY_SYSTEM, "Button", BS_AUTOCHECKBOX | WS_TABSTOP, 188, 135, 96, 10
CONTROL         "FILT CONTROL SYSTEM", IDC_FILT_CONTROL_SYSTEM, "Button", BS_AUTOCHECKBOX | WS_TABSTOP, 188, 165, 91, 10
PUSHBUTTON      "Eject", IDC_BTN_EJECT, 958, 42, 60, 11
PUSHBUTTON      "COCKPIT HATCH", IDC_BTN_COCKPIT_HATCH, 958, 93, 60, 12, BS_BOTTOM
PUSHBUTTON      "IGNITION", IDC_BTN_IGNITION, 958, 144, 60, 11, BS_BOTTOM
PUSHBUTTON      "START", IDC_BTN_START, 958, 192, 60, 10, BS_BOTTOM
CONTROL         "", IDC_PB_EJECT, "msctls_progress32", WS_BORDER, 958, 15, 60, 26
CONTROL         "", IDC_PB_COCKPIT_HATCH, "msctls_progress32", WS_BORDER, 958, 67, 60, 26
CONTROL         "", IDC_PB_IGNITION, "msctls_progress32", WS_BORDER, 958, 117, 60, 26
CONTROL         "", IDC_PB_START, "msctls_progress32", WS_BORDER, 958, 165, 60, 26
CONTROL         "", IDC_PB_GEARLEVER_5, "msctls_progress32", WS_BORDER, 62, 41, 29, 10
CONTROL         "", IDC_PB_GEARLEVER_1, "msctls_progress32", WS_BORDER, 62, 113, 29, 10
CONTROL         "", IDC_PB_GEARLEVER_2, "msctls_progress32", WS_BORDER, 62, 95, 29, 10
CONTROL         "", IDC_PB_GEARLEVER_3, "msctls_progress32", WS_BORDER, 62, 77, 29, 10
CONTROL         "", IDC_PB_GEARLEVER_4, "msctls_progress32", WS_BORDER, 62, 59, 29, 10
CONTROL         "", IDC_PB_GEARLEVER_N, "msctls_progress32", WS_BORDER, 62, 131, 29, 10
CONTROL         "", IDC_PB_GEARLEVER_R, "msctls_progress32", WS_BORDER, 62, 149, 29, 10
CONTROL         "", IDC_PB_COM5, "msctls_progress32", WS_BORDER, 455, 12, 14, 27
CONTROL         "", IDC_PB_COM1, "msctls_progress32", WS_BORDER, 343, 12, 14, 27
CONTROL         "", IDC_PB_COM2, "msctls_progress32", WS_BORDER, 371, 12, 14, 27
CONTROL         "", IDC_PB_COM3, "msctls_progress32", WS_BORDER, 399, 12, 14, 27
CONTROL         "", IDC_PB_COM4, "msctls_progress32", WS_BORDER, 427, 12, 14, 27
CONTROL         "", IDC_PB_WASHING, "msctls_progress32", WS_BORDER, 359, 151, 86, 13
CONTROL         "", IDC_PB_MAIN_WEAPON_CONTROL, "msctls_progress32", WS_BORDER, 359, 179, 86, 13
CONTROL         "", IDC_PB_EXTINGUISHER, "msctls_progress32", WS_BORDER, 481, 151, 86, 13
CONTROL         "", IDC_PB_SUB_WEAPON_CONTROL, "msctls_progress32", WS_BORDER, 481, 179, 86, 13
CONTROL         "", IDC_PB_CHAFF, "msctls_progress32", WS_BORDER, 603, 151, 86, 13
CONTROL         "", IDC_PB_MAGAZINE_CHANGE, "msctls_progress32", WS_BORDER, 603, 179, 86, 13
CONTROL         "", IDC_PB_FUNC1, "msctls_progress32", WS_BORDER, 507, 20, 14, 27
CONTROL         "", IDC_PB_FUNC2, "msctls_progress32", WS_BORDER, 507, 64, 14, 27
CONTROL         "", IDC_PB_FUNC3, "msctls_progress32", WS_BORDER, 507, 106, 14, 27
CONTROL         "", IDC_PB_TANK_DETACH, "msctls_progress32", WS_BORDER, 583, 20, 14, 27
CONTROL         "", IDC_PB_OVERRIDE, "msctls_progress32", WS_BORDER, 583, 64, 14, 27
CONTROL         "", IDC_PB_NIGHT_SCOPE, "msctls_progress32", WS_BORDER, 583, 106, 14, 27
CONTROL         "", IDC_PB_FSS, "msctls_progress32", WS_BORDER, 658, 20, 14, 27
CONTROL         "", IDC_PB_MANIPULATOR, "msctls_progress32", WS_BORDER, 658, 64, 14, 27
CONTROL         "", IDC_PB_LINE_COLOR_CHANGE, "msctls_progress32", WS_BORDER, 658, 106, 14, 27
CONTROL         "", IDC_PB_OPEN_CLOSE, "msctls_progress32", WS_BORDER, 788, 98, 56, 13
CONTROL         "", IDC_PB_MAP_ZOOM_IN_OUT, "msctls_progress32", WS_BORDER, 861, 98, 56, 13
CONTROL         "", IDC_PB_ZOOM_IN, "msctls_progress32", WS_BORDER, 788, 154, 56, 13
CONTROL         "", IDC_PB_ZOOM_OUT, "msctls_progress32", WS_BORDER, 861, 154, 56, 13
CONTROL         "", IDC_PB_MODE_SELECT, "msctls_progress32", WS_BORDER, 788, 135, 56, 13
CONTROL         "", IDC_PB_SUB_MONITOR_MODE_SELECT, "msctls_progress32", WS_BORDER, 861, 135, 56, 13
PUSHBUTTON      "ZOOM IN", IDC_BTN_ZOOM_IN, 788, 167, 56, 21
PUSHBUTTON      "ZOOM OUT", IDC_BTN_ZOOM_OUT, 861, 167, 56, 21
PUSHBUTTON      "MODE SELECT", IDC_BTN_MODE_SELECT, 788, 114, 56, 21
PUSHBUTTON      "SUB MONITOR MODE SELECT", IDC_BTN_SUB_MONITOR_MODE_SELECT, 861, 113, 56, 21, BS_MULTILINE
PUSHBUTTON      "OPEN/CLOSE", IDC_BTN_OPEN_CLOSE, 788, 77, 56, 21
PUSHBUTTON      "MAP ZOOM IN/OUT", IDC_BTN_MAP_ZOON_IN_OUT, 860, 77, 56, 21, BS_MULTILINE
LTEXT           "MULTI MONITOR", IDC_STATIC, 716, 99, 55, 8
LTEXT           "MAIN MONITOR ZOOM", IDC_STATIC, 718, 163, 61, 8
GROUPBOX        "", IDC_STATIC, 949, 59, 83, 148, 0, WS_EX_CLIENTEDGE
PUSHBUTTON      "MAIN WAPON CONTROL", IDC_BTN_MAIN_WEAPON_CONTROL, 359, 193, 86, 13
PUSHBUTTON      "SUB WAPON CTONTROL", IDC_BTN_SUB_WEAPON_CONTROL, 481, 192, 86, 13
PUSHBUTTON      "EXTINGUISHER", IDC_BTN_EXTINGUISHER, 481, 162, 86, 13
PUSHBUTTON      "MAGAZINE CHANGE", IDC_BTN_MAGAZINE_CHANGE, 603, 192, 86, 13
PUSHBUTTON      "CHAFF", IDC_BTN_CHAFF, 603, 162, 86, 13
PUSHBUTTON      "WASHING", IDC_BTN_WASHING, 359, 163, 86, 13
PUSHBUTTON      "COM1", IDC_BTN_COM1, 338, 42, 24, 13
PUSHBUTTON      "COM2", IDC_BTN_COM2, 366, 42, 24, 13
PUSHBUTTON      "COM3", IDC_BTN_COM3, 394, 42, 24, 13
PUSHBUTTON      "COM4", IDC_BTN_COM4, 422, 42, 24, 13
PUSHBUTTON      "COM5", IDC_BTN_COM5, 450, 42, 24, 13
PUSHBUTTON      "FUNC1", IDC_BTN_FUNC1, 476, 7, 76, 13
PUSHBUTTON      "FUNC2", IDC_BTN_FUNC2, 476, 51, 76, 13
PUSHBUTTON      "FUNC2", IDC_BTN_FUNC3, 476, 93, 76, 13
PUSHBUTTON      "TANK DETACH", IDC_BTN_TANK_DETACH, 552, 7, 76, 13
PUSHBUTTON      "F.S.S.", IDC_BTN_FSS, 627, 7, 76, 13
PUSHBUTTON      "OVERRIDE", IDC_BTN_OVERRIDE, 552, 51, 76, 13
PUSHBUTTON      "NIGHT SCOPE", IDC_BTN_NIGHT_SCOPE, 552, 93, 76, 13
PUSHBUTTON      "MANIPULATOR", IDC_BTN_MANIPULATOR, 627, 51, 76, 13
PUSHBUTTON      "LINE COLOR CHANGE", IDC_BTN_LINE_COLOR_CHANGE, 627, 93, 76, 13
LTEXT           "TUNER DIAL", IDC_STATIC, 389, 105, 40, 8
GROUPBOX        "Tuner Dial", IDC_STATIC, 344, 58, 129, 90, WS_GROUP, WS_EX_CLIENTEDGE
CONTROL         "0", IDC_RADIO_TD0, "Button", BS_AUTORADIOBUTTON, 360, 100, 20, 10
CONTROL         "1", IDC_RADIO_TD1, "Button", BS_AUTORADIOBUTTON, 369, 91, 20, 10
CONTROL         "2", IDC_RADIO_TD2, "Button", BS_AUTORADIOBUTTON, 380, 82, 20, 10
CONTROL         "3", IDC_RADIO_TD3, "Button", BS_AUTORADIOBUTTON, 390, 73, 20, 10
CONTROL         "4", IDC_RADIO_TD4, "Button", BS_AUTORADIOBUTTON, 403, 64, 20, 10
CONTROL         "5", IDC_RADIO_TD5, "Button", BS_AUTORADIOBUTTON, 419, 73, 20, 10
CONTROL         "6", IDC_RADIO_TD6, "Button", BS_AUTORADIOBUTTON, 430, 82, 20, 10
CONTROL         "7", IDC_RADIO_TD7, "Button", BS_AUTORADIOBUTTON, 439, 91, 20, 10
CONTROL         "8", IDC_RADIO_TD8, "Button", BS_AUTORADIOBUTTON, 447, 100, 20, 10
CONTROL         "9", IDC_RADIO_TD9, "Button", BS_AUTORADIOBUTTON, 439, 109, 20, 10
CONTROL         "10", IDC_RADIO_TD10, "Button", BS_AUTORADIOBUTTON, 431, 118, 24, 10
CONTROL         "11", IDC_RADIO_TD11, "Button", BS_AUTORADIOBUTTON, 417, 127, 24, 10
CONTROL         "12", IDC_RADIO_TD12, "Button", BS_AUTORADIOBUTTON, 405, 136, 24, 10
CONTROL         "", IDC_PB_VT_LOCATION_MEASUREMENT, "msctls_progress32", WS_BORDER, 188, 120, 14, 10
CONTROL         "", IDC_PB_OXYGEN_SUPPLY_SYSTEM, "msctls_progress32", WS_BORDER, 169, 135, 14, 10
CONTROL         "", IDC_PB_BUFFER_MATERIAL, "msctls_progress32", WS_BORDER, 188, 150, 14, 10
CONTROL         "", IDC_PB_VT_LOCATION_MEASUREMENT4, "msctls_progress32", WS_BORDER, 188, 180, 14, 10
CONTROL         "", IDC_PB_FILT_CONTROL_SYSTEM, "msctls_progress32", WS_BORDER, 169, 165, 14, 10
END

#include "stdafx.h"

//#include "CxbxKrnl/EmuShared.h"
#include "DlgVirtualSBCFeedback.h"
//#include "../CxbxKrnl/EmuXapi.h"
#include "Winuser.h"
#include "Windowsx.h"
#include "Commctrl.h"
#include "../gui/ResCxbx.h"

#include <cstdio>

#define    X_SBC_FEEDBACK_MAX                       38

namespace DlgVSBCFeedback
{
    /*! windows dialog procedure */
    static LRESULT CALLBACK DlgVirtualSBCFeedbackProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

    /*! changes flag */
    static BOOL g_bHasChanges = FALSE;
    //handle for this dialog, one one dialog is available for visturl SteelBatallion controller.
    static HWND g_hDlgVirtualSBCFeedback = 0;

    HWND GetDlgVirtualSBCFeedbackHandle(void)
    {
        return g_hDlgVirtualSBCFeedback;
    }

    int DlgItemIDArray[] = {
        IDC_PB_EJECT,
        IDC_PB_COCKPIT_HATCH,
        IDC_PB_IGNITION,
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
        IDC_PB_MAIN_WEAPON_CONTROL,
        IDC_PB_SUB_WEAPON_CONTROL,
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
        for (NibbleIndex = 0; NibbleIndex < X_SBC_FEEDBACK_MAX; NibbleIndex++) {
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
        INT_PTR nResult = 1;

        if (!IsWindow(g_hDlgVirtualSBCFeedback))
        {
            
            //g_hDlgVirtualSBCFeedback =
            nResult = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_VIRTUAL_SBC_FEEDBACK), hwnd, (DLGPROC)DlgVirtualSBCFeedbackProc);
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

    LRESULT CALLBACK DlgVirtualSBCFeedbackProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_INITDIALOG:
        {
            //reserve dialog handle, only one dialog can be created at a time.
            g_hDlgVirtualSBCFeedback = hWndDlg;

            /*! set window icon */
            SetClassLong(hWndDlg, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CXBX)));
            DWORD NibbleIndex = 0, ByteIndex = 0, FeedbackStatus = 0;
            for (NibbleIndex = 0; NibbleIndex < X_SBC_FEEDBACK_MAX; NibbleIndex++) {
                ByteIndex = NibbleIndex >> 1;
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
            break;
        }

        case WM_DESTROY:
        case WM_CLOSE:
        {
            //only need to destroy dialog itself when it's a modeless dialog.
            //DestroyWindow(hWndDlg);

            //reset the dialog handle, redundant.
            //g_hDlgVirtualSBCFeedback = 0;
            break;
        }
        //return true;

        case WM_COMMAND:
        {
            HWND hWndButton = GetDlgItem(hWndDlg, LOWORD(wParam));

//            switch (LOWORD(wParam))
            {
                //case IDC_HOST_CANCEL:
                  //  //EndDialog(hWndDlg, wParam);
                    //break;
//            default:
                return FALSE;
            }

            //return true;
            break;
        }
        }
        return FALSE;
    }
}

#endif
