// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->Cxbx->DlgControllerConfig.cpp
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
#include "DlgControllerConfig.h"
#include "ResCxbx.h"
#include "EmuShared.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <stdio.h>

// ******************************************************************
// * Static Function(s)
// ******************************************************************
static INT_PTR CALLBACK DlgControllerConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
static VOID ConfigureInput(HWND hWndDlg, HWND hWndButton, XBCtrlObject object);

// ******************************************************************
// * Static Variable(s)
// ******************************************************************
static XBController g_XBController;
static BOOL         g_bHasChanges;

// ******************************************************************
// * Show Controller Configuration Dialog Window
// ******************************************************************
void ShowControllerConfig(HWND hwnd)
{
	g_bHasChanges = FALSE;

    g_EmuShared->GetXBController(&g_XBController);

    DialogBox
    (
        GetModuleHandle(NULL),
        MAKEINTRESOURCE(IDD_CONTROLLER_CFG),
        hwnd,
        DlgControllerConfigProc
    );
}

// ******************************************************************
// * Controller Configuration Dialog Procedure
// ******************************************************************
INT_PTR CALLBACK DlgControllerConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_INITDIALOG:
            SetClassLong(hWndDlg, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CXBX)));
            SetFocus(GetDlgItem(hWndDlg, IDC_SET_X));
            break;
        case WM_CLOSE:
            if(g_bHasChanges)
            {
                int ret = MessageBox(hWndDlg, "Do you wish to apply your changes?", "Cxbx", MB_ICONQUESTION | MB_YESNOCANCEL);

                switch(ret)
                {
                    case IDYES:
                        PostMessage(hWndDlg, WM_COMMAND, IDC_INPUT_CONFIG_ACCEPT, 0);
                        break;
					case IDNO:
						PostMessage(hWndDlg, WM_COMMAND, IDC_INPUT_CONFIG_CANCEL, 0);
					default:
                        break;
                }
            }
			else
			{
				PostMessage(hWndDlg, WM_COMMAND, IDC_INPUT_CONFIG_CANCEL, 0);
				break;
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
                    g_EmuShared->SetXBController(&g_XBController);
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
            } 
        }
        break;
    } 
    return FALSE; 
}

// ******************************************************************
// * ConfigureInput
// ******************************************************************
VOID ConfigureInput(HWND hWndDlg, HWND hWndButton, XBCtrlObject object)
{
    static bConfigDone = true;

    if(!bConfigDone)
        return;
	
	g_bHasChanges = true;

    bConfigDone = false;

    char szOrgText[32];
    char szNewText[255] = "Recieved no user input, try again...";

    SetWindowText(GetDlgItem(hWndDlg, IDC_CONFIG_STATUS), "Waiting for your input...");
    GetWindowText(hWndButton, szOrgText, 32);

    g_XBController.ConfigBegin(hWndDlg, object);

    // ******************************************************************
    // * Wait for input, or 5 second timeout
    // ******************************************************************
    for(int v=100;v>0;v--)
    {
        // ******************************************************************
        // * Update the button text every second
        // ******************************************************************
        if(v%20 == 0)
        {
            char szBuffer[255];

            sprintf(szBuffer, "%d", (v+19)/20);

            SetWindowText(hWndButton, szBuffer);
        }

        if(g_XBController.GetError())
            goto cleanup;

        if(g_XBController.ConfigPoll(szNewText))
            break;

        Sleep(50);
    }

    if(g_XBController.GetError())
        goto cleanup;
    else
        g_XBController.ConfigEnd();

cleanup:

    // ******************************************************************
    // * Update Window
    // ******************************************************************
    {
        if(g_XBController.GetError())
            sprintf(szNewText, "%s", g_XBController.GetError());

        SetWindowText(hWndButton, szOrgText);

        SetWindowText(GetDlgItem(hWndDlg, IDC_CONFIG_STATUS), szNewText);

        MSG Msg;

        while(PeekMessage(&Msg, hWndDlg, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE));
        while(PeekMessage(&Msg, hWndDlg, WM_KEYFIRST,   WM_KEYLAST,   PM_REMOVE));

    }

    bConfigDone = true;
}
