// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->Cxbx->DlgVideoConfig.cpp
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
#include "DlgVideoConfig.h"
#include "ResCxbx.h"
#include "EmuShared.h"
#include "Emu.h"

#include <stdio.h>
#include <d3d8.h>

// ******************************************************************
// * globals
// ******************************************************************
LPDIRECT3D8       g_pD3D8         = NULL;   // Direct3D8
LPDIRECT3DDEVICE8 g_pD3D8Device   = NULL;   // Direct3D8 Device

// ******************************************************************
// * Static Function(s)
// ******************************************************************
INT_PTR CALLBACK DlgVideoConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

// ******************************************************************
// * func: ShowVideoConfig
// ******************************************************************
void ShowVideoConfig(HWND hwnd)
{
    // ******************************************************************
    // * Display Window
    // ******************************************************************
    DialogBox
    (
        GetModuleHandle(NULL),
        MAKEINTRESOURCE(IDD_VIDEO_CFG),
        hwnd,
        DlgVideoConfigProc
    );
}

// ******************************************************************
// * func: DlgVideoConfigProc
// ******************************************************************
INT_PTR CALLBACK DlgVideoConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_INITDIALOG:
        {
            HWND hDisplayAdapter = GetDlgItem(hWndDlg, IDC_VC_DISPLAY_ADAPTER);

            SetClassLong(hWndDlg, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CXBX)));

            // ******************************************************************
            // * Create Direct3D
            // ******************************************************************
            {
                g_pD3D8 = Direct3DCreate8(D3D_SDK_VERSION);

                if(g_pD3D8 == NULL)
                    break;
            }

            // ******************************************************************
            // * Enumerate Display Adapters
            // ******************************************************************
            {
                D3DADAPTER_IDENTIFIER8 AdapterIdentifier;

                uint32 dwCount = g_pD3D8->GetAdapterCount();

                for(uint32 v=0;v<dwCount;v++)
                {
                    g_pD3D8->GetAdapterIdentifier(v, D3DENUM_NO_WHQL_LEVEL, &AdapterIdentifier);

                    SendMessage(hDisplayAdapter, CB_ADDSTRING, 0, (LPARAM)AdapterIdentifier.Description);

                    // First enumeration is choosen by default
                    if(v == 0) SendMessage(hDisplayAdapter, CB_SETCURSEL, 0, 0);
                }
            }
        }
        break;

        case WM_COMMAND:
        {
            HWND hWndButton = GetDlgItem(hWndDlg, LOWORD(wParam));

            switch(LOWORD(wParam))
            { 
                case IDC_VC_CANCEL:
                    EndDialog(hWndDlg, wParam); 
                    break;
                case IDC_VC_ACCEPT:
                    EndDialog(hWndDlg, wParam);
                    break;
            }
        }
        break;
        
        case WM_CLOSE:
            EndDialog(hWndDlg, wParam);
            break;
    } 
    return FALSE; 
}