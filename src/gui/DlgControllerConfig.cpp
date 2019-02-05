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

#include "DlgControllerConfig.h"
#include "resource/ResCxbx.h"
#include "common\Win32\DInputController.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <cstdio>

/*! windows dialog procedure */
static INT_PTR CALLBACK DlgControllerConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
/*! configure input for the specified controller object */
static VOID ConfigureInput(HWND hWndDlg, HWND hWndButton, XBCtrlObject object);
/*! enable / disable button windows */
static VOID EnableButtonWindows(HWND hWndDlg, HWND hExclude, BOOL bEnable);

/*! controller configuration */
static DInputController g_DInputController;
/*! changes flag */
static BOOL g_bHasChanges = FALSE;

VOID ShowControllerConfig(HWND hwnd)
{
    /*! reset changes flag */
    g_bHasChanges = FALSE;

    /*! retrieve controller configuration */
    g_DInputController.m_settings = g_Settings->m_controller_dinput;

    /*! show dialog box */
    DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_CONTROLLER_CFG), hwnd, DlgControllerConfigProc);
}

INT_PTR CALLBACK DlgControllerConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_INITDIALOG:
        {
            /*! set window icon */
            SetClassLong(hWndDlg, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CXBX)));

            /*! set default focus to X button */
            SetFocus(GetDlgItem(hWndDlg, IDC_SET_X));
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
                        PostMessage(hWndDlg, WM_COMMAND, IDC_INPUT_CONFIG_ACCEPT, 0);
                        break;
                    case IDNO:
                        PostMessage(hWndDlg, WM_COMMAND, IDC_INPUT_CONFIG_CANCEL, 0);
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
                case IDC_INPUT_CONFIG_CANCEL:
                    EndDialog(hWndDlg, wParam);
                    break;

                case IDC_INPUT_CONFIG_ACCEPT:
                    g_Settings->m_controller_dinput = g_DInputController.m_settings;
                    EndDialog(hWndDlg, wParam);
                    break;

                case IDC_SET_LEFT_POSY:
                    ConfigureInput(hWndDlg, hWndButton, XBCTRL_OBJECT_LTHUMBPOSY);
                    break;

                case IDC_SET_LEFT_NEGY:
                    ConfigureInput(hWndDlg, hWndButton, XBCTRL_OBJECT_LTHUMBNEGY);
                    break;

                case IDC_SET_LEFT_POSX:
                    ConfigureInput(hWndDlg, hWndButton, XBCTRL_OBJECT_LTHUMBPOSX);
                    break;

                case IDC_SET_LEFT_NEGX:
                    ConfigureInput(hWndDlg, hWndButton, XBCTRL_OBJECT_LTHUMBNEGX);
                    break;

                case IDC_SET_RIGHT_POSY:
                    ConfigureInput(hWndDlg, hWndButton, XBCTRL_OBJECT_RTHUMBPOSY);
                    break;

                case IDC_SET_RIGHT_NEGY:
                    ConfigureInput(hWndDlg, hWndButton, XBCTRL_OBJECT_RTHUMBNEGY);
                    break;

                case IDC_SET_RIGHT_POSX:
                    ConfigureInput(hWndDlg, hWndButton, XBCTRL_OBJECT_RTHUMBPOSX);
                    break;

                case IDC_SET_RIGHT_NEGX:
                    ConfigureInput(hWndDlg, hWndButton, XBCTRL_OBJECT_RTHUMBNEGX);
                    break;

                case IDC_SET_X:
                    ConfigureInput(hWndDlg, hWndButton, XBCTRL_OBJECT_X);
                    break;

                case IDC_SET_Y:
                    ConfigureInput(hWndDlg, hWndButton, XBCTRL_OBJECT_Y);
                    break;

                case IDC_SET_A:
                    ConfigureInput(hWndDlg, hWndButton, XBCTRL_OBJECT_A);
                    break;

                case IDC_SET_B:
                    ConfigureInput(hWndDlg, hWndButton, XBCTRL_OBJECT_B);
                    break;

                case IDC_SET_WHITE:
                    ConfigureInput(hWndDlg, hWndButton, XBCTRL_OBJECT_WHITE);
                    break;

                case IDC_SET_BLACK:
                    ConfigureInput(hWndDlg, hWndButton, XBCTRL_OBJECT_BLACK);
                    break;

                case IDC_SET_LTRIGGER:
                    ConfigureInput(hWndDlg, hWndButton, XBCTRL_OBJECT_LTRIGGER);
                    break;

                case IDC_SET_RTRIGGER:
                    ConfigureInput(hWndDlg, hWndButton, XBCTRL_OBJECT_RTRIGGER);
                    break;

                case IDC_SET_DPAD_UP:
                    ConfigureInput(hWndDlg, hWndButton, XBCTRL_OBJECT_DPADUP);
                    break;

                case IDC_SET_DPAD_DOWN:
                    ConfigureInput(hWndDlg, hWndButton, XBCTRL_OBJECT_DPADDOWN);
                    break;

                case IDC_SET_DPAD_LEFT:
                    ConfigureInput(hWndDlg, hWndButton, XBCTRL_OBJECT_DPADLEFT);
                    break;

                case IDC_SET_DPAD_RIGHT:
                    ConfigureInput(hWndDlg, hWndButton, XBCTRL_OBJECT_DPADRIGHT);
                    break;

                case IDC_SET_BACK:
                    ConfigureInput(hWndDlg, hWndButton, XBCTRL_OBJECT_BACK);
                    break;

                case IDC_SET_START:
                    ConfigureInput(hWndDlg, hWndButton, XBCTRL_OBJECT_START);
                    break;

                case IDC_SET_LTHUMB:
                    ConfigureInput(hWndDlg, hWndButton, XBCTRL_OBJECT_LTHUMB);
                    break;

                case IDC_SET_RTHUMB:
                    ConfigureInput(hWndDlg, hWndButton, XBCTRL_OBJECT_RTHUMB);
                    break;

                case IDC_CONFIGURE_ALL:
                {
                    int v=0;

                    struct _ConfigObj
                    {
                        int idcVal;
                        XBCtrlObject ctrl;
                    }
                    configObj[] =
                    {
                        { IDC_SET_X, XBCTRL_OBJECT_X },
                        { IDC_SET_Y, XBCTRL_OBJECT_Y },
                        { IDC_SET_A, XBCTRL_OBJECT_A },
                        { IDC_SET_B, XBCTRL_OBJECT_B },
                        { IDC_SET_WHITE, XBCTRL_OBJECT_WHITE },
                        { IDC_SET_BLACK, XBCTRL_OBJECT_BLACK },
                        { IDC_SET_LTRIGGER, XBCTRL_OBJECT_LTRIGGER },
                        { IDC_SET_RTRIGGER, XBCTRL_OBJECT_RTRIGGER },
                        { IDC_SET_DPAD_UP, XBCTRL_OBJECT_DPADUP },
                        { IDC_SET_DPAD_DOWN, XBCTRL_OBJECT_DPADDOWN },
                        { IDC_SET_DPAD_LEFT, XBCTRL_OBJECT_DPADLEFT },
                        { IDC_SET_DPAD_RIGHT, XBCTRL_OBJECT_DPADRIGHT },
                        { IDC_SET_BACK, XBCTRL_OBJECT_BACK },
                        { IDC_SET_START, XBCTRL_OBJECT_START },
                        { IDC_SET_LTHUMB, XBCTRL_OBJECT_LTHUMB },
                        { IDC_SET_RTHUMB, XBCTRL_OBJECT_RTHUMB },
                        { IDC_SET_LEFT_POSY, XBCTRL_OBJECT_LTHUMBPOSY },
                        { IDC_SET_LEFT_NEGY, XBCTRL_OBJECT_LTHUMBNEGY },
                        { IDC_SET_LEFT_NEGX, XBCTRL_OBJECT_LTHUMBNEGX },
                        { IDC_SET_LEFT_POSX, XBCTRL_OBJECT_LTHUMBPOSX },
                        { IDC_SET_RIGHT_POSY, XBCTRL_OBJECT_RTHUMBPOSY },
                        { IDC_SET_RIGHT_NEGY, XBCTRL_OBJECT_RTHUMBNEGY },
                        { IDC_SET_RIGHT_NEGX, XBCTRL_OBJECT_RTHUMBNEGX },
                        { IDC_SET_RIGHT_POSX, XBCTRL_OBJECT_RTHUMBPOSX },
                    };

                    for(v=0;v<sizeof(configObj) / sizeof(_ConfigObj);v++)
                    {
                        Sleep(500);
                        ConfigureInput(hWndDlg, GetDlgItem(hWndDlg, configObj[v].idcVal), configObj[v].ctrl);
                    }

                    break;
                }
            }
        }
        break;
    }
    return FALSE;
}

VOID ConfigureInput(HWND hWndDlg, HWND hWndButton, XBCtrlObject object)
{
    static bool bConfigDone = true;

    /*! ensure only one input is configured at a time */
    if(!bConfigDone) { return; }

    bConfigDone = false;

    g_bHasChanges = TRUE;

    /*! disable all buttons */
    EnableButtonWindows(hWndDlg, hWndButton, FALSE);

    char szOrgText[32];
    char szNewText[255] = "Recieved no user input, try again...";

    SetWindowText(GetDlgItem(hWndDlg, IDC_CONFIG_STATUS), "Waiting for your input...");
    GetWindowText(hWndButton, szOrgText, 32);

    g_DInputController.ConfigBegin(hWndDlg, object);

    // wait for input, or 5 second timeout
    for(int v=100;v>0;v--)
    {
        // update the button text every second
        if(v%20 == 0)
        {
            char szBuffer[255];

            sprintf(szBuffer, "%d", (v+19)/20);

            SetWindowText(hWndButton, szBuffer);

            // NOTE: This fix false positive of non-responding message when inputting all keys at once.
            // Source: https://msdn.microsoft.com/en-us/library/windows/desktop/ms633526%28v=vs.85%29.aspx
            MSG Msg;
            PeekMessage(&Msg, hWndDlg, 0, 0, PM_NOREMOVE);
        }

        if(g_DInputController.HasError())
        {
            goto cleanup;
        }

        if(g_DInputController.ConfigPoll(szNewText))
        {
            break;
        }

        Sleep(50);
    }

    if(g_DInputController.HasError())
    {
        goto cleanup;
    }
    else
    {
        g_DInputController.ConfigEnd();
    }

cleanup:

    /*! enable all buttons */
    EnableButtonWindows(hWndDlg, hWndButton, TRUE);

    /*! update window with status */
    {
        if(g_DInputController.HasError())
        {
            sprintf(szNewText, "%s", g_DInputController.GetError().c_str());
        }

        SetWindowText(hWndButton, szOrgText);

        SetWindowText(GetDlgItem(hWndDlg, IDC_CONFIG_STATUS), szNewText);

        MSG Msg;

        while(PeekMessage(&Msg, hWndDlg, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE));
        while(PeekMessage(&Msg, hWndDlg, WM_KEYFIRST,   WM_KEYLAST,   PM_REMOVE));

    }

    bConfigDone = true;
}

VOID EnableButtonWindows(HWND hWndDlg, HWND hExclude, BOOL bEnable)
{
    int v=0;

    /*! list of applicable child windows */
    int itemList[] =
    {
        IDC_SET_X, IDC_SET_Y, IDC_SET_A, IDC_SET_B,
        IDC_SET_WHITE, IDC_SET_BLACK,
        IDC_SET_LTHUMB, IDC_SET_RTHUMB,
        IDC_SET_DPAD_UP, IDC_SET_DPAD_DOWN, IDC_SET_DPAD_LEFT, IDC_SET_DPAD_RIGHT,
        IDC_SET_BACK, IDC_SET_START, IDC_SET_LTRIGGER, IDC_SET_RTRIGGER,
        IDC_SET_LEFT_POSY, IDC_SET_LEFT_NEGY, IDC_SET_LEFT_NEGX, IDC_SET_LEFT_POSX,
        IDC_SET_RIGHT_POSY, IDC_SET_RIGHT_NEGY, IDC_SET_RIGHT_NEGX, IDC_SET_RIGHT_POSX,
        IDC_INPUT_CONFIG_CANCEL, IDC_INPUT_CONFIG_ACCEPT,
        IDC_CONFIGURE_ALL
    };

    /*! enable / disable all the listed windows */
    for(v=0;v<sizeof(itemList) / sizeof(int);v++)
    {
        HWND hWnd = GetDlgItem(hWndDlg, itemList[v]);

        if(hWnd != hExclude) { EnableWindow(hWnd, bEnable); }
    }
}
