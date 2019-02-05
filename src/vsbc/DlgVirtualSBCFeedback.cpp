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
#include "stdafx.h"

//#include "CxbxKrnl/EmuShared.h"
#include "DlgVirtualSBCFeedback.h"
//#include "../CxbxKrnl/EmuXapi.h"
#include "Winuser.h"
#include "Windowsx.h"
#include "Commctrl.h"
#include "../gui/resource/ResCxbx.h"

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
