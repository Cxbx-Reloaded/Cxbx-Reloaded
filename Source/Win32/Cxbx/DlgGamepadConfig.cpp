// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->Cxbx->DlgGamepadConfig.cpp
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
#include "Cxbx.h"
#include "ResCxbx.h"
#include "DlgGamepadConfig.h"

// ******************************************************************
// * globals / static
// ******************************************************************
static INT_PTR CALLBACK DlgGamepadConfigProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

// ******************************************************************
// * Show Dialog
// ******************************************************************
void ShowGamepadConfig(HWND hwnd)
{
    DialogBox
    (
        GetModuleHandle(NULL),
        MAKEINTRESOURCE(IDD_GAMEPAD_CFG),
        hwnd,
        DlgGamepadConfigProc
    );
}

// ******************************************************************
// * Gamepad configuration dialog procedure
// ******************************************************************
INT_PTR CALLBACK DlgGamepadConfigProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_INITDIALOG:
            SetClassLong(hwndDlg, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CXBX)));
            break;
        case WM_CLOSE:
            EndDialog(hwndDlg, wParam); 
            break;
        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            { 
                case IDC_INPUT_CONFIG_CANCEL:
                    EndDialog(hwndDlg, wParam); 
                    break;
            } 
        }
        break;
    } 
    return FALSE; 
}
