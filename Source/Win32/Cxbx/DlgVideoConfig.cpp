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
// * Global(s)
// ******************************************************************
XTL::D3DDEVTYPE  g_DevType[2] = {XTL::D3DDEVTYPE_HAL, XTL::D3DDEVTYPE_REF};
char            *g_szDevType[2] = {"Direct3D HAL (Hardware Accelerated)", "Direct3D REF (Software)"};

// ******************************************************************
// * Static Function(s)
// ******************************************************************
static INT_PTR CALLBACK DlgVideoConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
static VOID RefreshDisplayAdapter();
static VOID RefreshDirect3DDevice();

// ******************************************************************
// * Static Variable(s)
// ******************************************************************
static XTL::LPDIRECT3D8        g_pD3D8         = NULL;   // Direct3D8
static XTL::LPDIRECT3DDEVICE8  g_pD3D8Device   = NULL;   // Direct3D8 Device
static XBVideo                 g_XBVideo;
static BOOL                    g_bHasChanges;
static DWORD                   g_dwAdapterCount = 0;
static DWORD                   g_dwVideoResolution = 0;
static HWND                    g_hDisplayAdapter  = NULL;
static HWND                    g_hDirect3DDevice  = NULL;
static HWND                    g_hVideoResolution = NULL;

// ******************************************************************
// * func: ShowVideoConfig
// ******************************************************************
void ShowVideoConfig(HWND hwnd)
{
	g_bHasChanges = FALSE;

    g_EmuShared->GetXBVideo(&g_XBVideo);

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
            g_hDisplayAdapter  = GetDlgItem(hWndDlg, IDC_VC_DISPLAY_ADAPTER);
            g_hDirect3DDevice  = GetDlgItem(hWndDlg, IDC_VC_D3D_DEVICE);
            g_hVideoResolution = GetDlgItem(hWndDlg, IDC_VC_VIDEO_RESOLUTION);

            SetClassLong(hWndDlg, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CXBX)));

            // ******************************************************************
            // * Create Direct3D
            // ******************************************************************
            {
                g_pD3D8 = XTL::Direct3DCreate8(D3D_SDK_VERSION);

                if(g_pD3D8 == NULL)
                    break;
            }

            // ******************************************************************
            // * Enumerate Display Adapters
            // ******************************************************************
            {
                SendMessage(g_hDisplayAdapter, CB_RESETCONTENT, 0, 0);

                XTL::D3DADAPTER_IDENTIFIER8 AdapterIdentifier;

                g_dwAdapterCount = g_pD3D8->GetAdapterCount();

                for(uint32 v=0;v<g_dwAdapterCount;v++)
                {
                    g_pD3D8->GetAdapterIdentifier(v, D3DENUM_NO_WHQL_LEVEL, &AdapterIdentifier);

                    if(v == 0)
                        SendMessage(g_hDisplayAdapter, CB_ADDSTRING, 0, (LPARAM)"Primary Display Adapter");
                    else
                        SendMessage(g_hDisplayAdapter, CB_ADDSTRING, 0, (LPARAM)AdapterIdentifier.Description);
                }
            }

            SendMessage(g_hDisplayAdapter, CB_SETCURSEL, g_XBVideo.GetDisplayAdapter(), 0);

            RefreshDisplayAdapter();

            // ******************************************************************
            // * Check Appropriate Options
            // ******************************************************************
            SendMessage(GetDlgItem(hWndDlg, IDC_CV_FULLSCREEN), BM_SETCHECK, (WPARAM)g_XBVideo.GetFullscreen(), 0);
            SendMessage(GetDlgItem(hWndDlg, IDC_CV_VSYNC), BM_SETCHECK, (WPARAM)g_XBVideo.GetVSync(), 0);
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
                {
                    // ******************************************************************
                    // * Store Video Resolution
                    // ******************************************************************
                    {
                        HWND hVideoResolution = GetDlgItem(hWndDlg, IDC_VC_VIDEO_RESOLUTION);

                        LRESULT lRet = SendMessage(hVideoResolution, CB_GETCURSEL, 0, 0);

                        char szBuffer[128];

                        lRet = SendMessage(hVideoResolution, CB_GETLBTEXT, (WPARAM)lRet, (LPARAM)szBuffer);

                        g_XBVideo.SetVideoResolution(szBuffer);
                    }

                    // ******************************************************************
                    // * Store Fullscreen/VSync Configuration
                    // ******************************************************************
                    {
                        LRESULT lRet;

                        lRet = SendMessage(GetDlgItem(hWndDlg, IDC_CV_FULLSCREEN), BM_GETCHECK, 0, 0);

                        g_XBVideo.SetFullscreen(lRet == BST_CHECKED);

                        lRet = SendMessage(GetDlgItem(hWndDlg, IDC_CV_VSYNC), BM_GETCHECK, 0, 0);

                        g_XBVideo.SetVSync(lRet == BST_CHECKED);
                    }

                    g_EmuShared->SetXBVideo(&g_XBVideo);

                    EndDialog(hWndDlg, wParam);
                }
                break;

                case IDC_VC_DISPLAY_ADAPTER:
                {
                    switch(HIWORD(wParam))
                    {
                        case CBN_SELCHANGE:
                            RefreshDisplayAdapter();
                            break;
                    }
                }
                break;

                case IDC_VC_D3D_DEVICE:
                {
                    switch(HIWORD(wParam))
                    {
                        case CBN_SELCHANGE:
                            RefreshDirect3DDevice();
                    }
                }
                break;
            }
        }
        break;

        case WM_CLOSE:
        {
            if(g_bHasChanges)
            {
                int ret = MessageBox(hWndDlg, "Do you wish to apply your changes?", "Cxbx", MB_ICONQUESTION | MB_YESNOCANCEL);

                switch(ret)
                {
                    case IDYES:
                        PostMessage(hWndDlg, WM_COMMAND, IDC_VC_ACCEPT, 0);
                        break;
					case IDNO:
						PostMessage(hWndDlg, WM_COMMAND, IDC_VC_CANCEL, 0);
					default:
                        break;
                }
            }
			else
			{
				PostMessage(hWndDlg, WM_COMMAND, IDC_VC_CANCEL, 0);
				break;
			}
			break;
            EndDialog(hWndDlg, wParam);
        }
        break;
    } 
    return FALSE; 
}

// ******************************************************************
// * func: RefreshDisplayAdapter
// ******************************************************************
VOID RefreshDisplayAdapter()
{
    g_XBVideo.SetDisplayAdapter(SendMessage(g_hDisplayAdapter, CB_GETCURSEL, 0, 0));

    SendMessage(g_hDirect3DDevice, CB_RESETCONTENT, 0, 0);

    // ******************************************************************
    // * Enumerate Direct3D Devices
    // ******************************************************************
    {
        XTL::D3DCAPS8 Caps;

        for(uint32 d=0;d<2;d++)
            if(g_pD3D8->GetDeviceCaps(g_XBVideo.GetDisplayAdapter(), g_DevType[d], &Caps) == D3D_OK)
                SendMessage(g_hDirect3DDevice, CB_ADDSTRING, 0, (LPARAM)g_szDevType[d]);
    }

    SendMessage(g_hDirect3DDevice, CB_SETCURSEL, g_XBVideo.GetDirect3DDevice(), 0);

    RefreshDirect3DDevice();

    return;
}

// ******************************************************************
// * func: RefreshDirect3DDevice
// ******************************************************************
VOID RefreshDirect3DDevice()
{
    g_XBVideo.SetDirect3DDevice(SendMessage(g_hDirect3DDevice, CB_GETCURSEL, 0, 0));

    SendMessage(g_hVideoResolution, CB_RESETCONTENT, 0, 0);

    g_dwVideoResolution = 0;

    // ******************************************************************
    // * Enumerate Video Modes
    // ******************************************************************
    {
        XTL::D3DDISPLAYMODE DisplayMode;

        uint32 dwAdapterModeCount = g_pD3D8->GetAdapterModeCount(g_XBVideo.GetDisplayAdapter());

        char szBuffer[260];

        SendMessage(g_hVideoResolution, CB_ADDSTRING, 0, (LPARAM)"Automatic (Default)");

        for(uint32 v=0;v<dwAdapterModeCount;v++)
        {
            char *szFormat = 0;

            g_pD3D8->EnumAdapterModes(g_XBVideo.GetDisplayAdapter(), v, &DisplayMode);

            switch(DisplayMode.Format)
            {
                case XTL::D3DFMT_X1R5G5B5:
                    szFormat = "16bit x1r5g5b5";
                    break;
                case XTL::D3DFMT_R5G6B5:
                    szFormat = "16bit r5g6r5";
                    break;
                case XTL::D3DFMT_X8R8G8B8:
                    szFormat = "32bit x8r8g8b8";
                    break;
                case XTL::D3DFMT_A8R8G8B8:
                    szFormat = "32bit a8r8g8b8";
                    break;
                default:
                    szFormat = "Unknown";
                    break;
            };

            if(DisplayMode.RefreshRate == 0)
                sprintf(szBuffer, "%d x %d %s", DisplayMode.Width, DisplayMode.Height, szFormat);
            else
                sprintf(szBuffer, "%d x %d %s (%d hz)", DisplayMode.Width, DisplayMode.Height, szFormat, DisplayMode.RefreshRate);

            if(strcmp(szBuffer, g_XBVideo.GetVideoResolution()) == 0)
                g_dwVideoResolution = v+1;

            SendMessage(g_hVideoResolution, CB_ADDSTRING, 0, (LPARAM)szBuffer);
        }
    }

    SendMessage(g_hVideoResolution, CB_SETCURSEL, g_dwVideoResolution, 0);
    
    return;
}