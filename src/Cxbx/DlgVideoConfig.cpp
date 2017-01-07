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
// *   Cxbx->Cxbx->DlgVideoConfig.cpp
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

#include "DlgVideoConfig.h"
#include "ResCxbx.h"

#include <d3d8.h>

/*! windows dialog procedure */
static INT_PTR CALLBACK DlgVideoConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
/*! refresh UI based on selected display adapter */
static VOID RefreshDisplayAdapter();
/*! refresh UI based on selected device */
static VOID RefreshDirect3DDevice();

/*! direct3d instance */
static XTL::LPDIRECT3D8 g_pD3D8 = 0;
/*! video configuration */
static XBVideo g_XBVideo;
/*! changes flag */
static BOOL g_bHasChanges = FALSE;
/*! number of adapters */
static DWORD g_dwAdapterCount = 0;
/*! handle to display adapter list window */
static HWND g_hDisplayAdapter = NULL;
/*! handle to device list window */
static HWND g_hDirect3DDevice = NULL;
/*! handle to video resolution list window */
static HWND g_hVideoResolution = NULL;

VOID ShowVideoConfig(HWND hwnd)
{
    /*! reset changes flag */
    g_bHasChanges = FALSE;

    /*! retrieve video configuration */
    g_EmuShared->GetXBVideo(&g_XBVideo);

    /*! initialize direct3d */
    {
        g_pD3D8 = XTL::Direct3DCreate8(D3D_SDK_VERSION);

        if(g_pD3D8 == 0) { goto cleanup; }

        g_dwAdapterCount = g_pD3D8->GetAdapterCount();
    }

    /*! show dialog box */
    DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_VIDEO_CFG), hwnd, DlgVideoConfigProc);

cleanup:

    /*! cleanup direct3d */
    {
        if(g_pD3D8 != 0)
        {
            g_pD3D8->Release();
            g_pD3D8 = 0;
        }
    }
}

INT_PTR CALLBACK DlgVideoConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_INITDIALOG:
        {
            g_hDisplayAdapter  = GetDlgItem(hWndDlg, IDC_VC_DISPLAY_ADAPTER);
            g_hDirect3DDevice  = GetDlgItem(hWndDlg, IDC_VC_D3D_DEVICE);
            g_hVideoResolution = GetDlgItem(hWndDlg, IDC_VC_VIDEO_RESOLUTION);

            /*! set window icon */
            SetClassLong(hWndDlg, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CXBX)));

            /*! enumerate display adapters */
            {
                SendMessage(g_hDisplayAdapter, CB_RESETCONTENT, 0, 0);

                for(uint32 v=0;v<g_dwAdapterCount;v++)
                {
                    XTL::D3DADAPTER_IDENTIFIER8 adapterIdentifier;

                    g_pD3D8->GetAdapterIdentifier(v, D3DENUM_NO_WHQL_LEVEL, &adapterIdentifier);

                    if(v == 0)
                    {
                        SendMessage(g_hDisplayAdapter, CB_ADDSTRING, 0, (LPARAM)"Primary Display Adapter");
                    }
                    else
                    {
                        SendMessage(g_hDisplayAdapter, CB_ADDSTRING, 0, (LPARAM)adapterIdentifier.Description);
                    }
                }
            }

            /*! activate configured display adapter */
            SendMessage(g_hDisplayAdapter, CB_SETCURSEL, g_XBVideo.GetDisplayAdapter(), 0);

            /*! refresh UI based on currently selected display adapter */
            RefreshDisplayAdapter();

            /*! check appropriate options */
            {
                SendMessage(GetDlgItem(hWndDlg, IDC_CV_FULLSCREEN), BM_SETCHECK, (WPARAM)g_XBVideo.GetFullscreen(), 0);

                SendMessage(GetDlgItem(hWndDlg, IDC_CV_VSYNC), BM_SETCHECK, (WPARAM)g_XBVideo.GetVSync(), 0);

				SendMessage(GetDlgItem(hWndDlg, IDC_CV_HARDWAREYUV), BM_SETCHECK, (WPARAM)g_XBVideo.GetHardwareYUV(), 0);
            }
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
                        PostMessage(hWndDlg, WM_COMMAND, IDC_VC_ACCEPT, 0);
                        break;
                    case IDNO:
                        PostMessage(hWndDlg, WM_COMMAND, IDC_VC_CANCEL, 0);
                        break;
                }
                break;
            }

            PostMessage(hWndDlg, WM_COMMAND, IDC_VC_CANCEL, 0);
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
                    /*! save video resolution configuration */
                    {
                        HWND hVideoResolution = GetDlgItem(hWndDlg, IDC_VC_VIDEO_RESOLUTION);

                        LRESULT lRet = SendMessage(hVideoResolution, CB_GETCURSEL, 0, 0);

                        char szBuffer[128];

                        lRet = SendMessage(hVideoResolution, CB_GETLBTEXT, (WPARAM)lRet, (LPARAM)szBuffer);

                        g_XBVideo.SetVideoResolution(szBuffer);
                    }

                    /*! save fullscreen/vsync/YUV options */
                    {
                        LRESULT lRet = SendMessage(GetDlgItem(hWndDlg, IDC_CV_FULLSCREEN), BM_GETCHECK, 0, 0);

                        g_XBVideo.SetFullscreen(lRet == BST_CHECKED);

                        lRet = SendMessage(GetDlgItem(hWndDlg, IDC_CV_VSYNC), BM_GETCHECK, 0, 0);

                        g_XBVideo.SetVSync(lRet == BST_CHECKED);

						lRet = SendMessage(GetDlgItem(hWndDlg, IDC_CV_HARDWAREYUV), BM_GETCHECK, 0, 0);

						g_XBVideo.SetHardwareYUV(lRet == BST_CHECKED);
                    }

                    /*! save video configuration */
                    g_EmuShared->SetXBVideo(&g_XBVideo);

                    EndDialog(hWndDlg, wParam);
                }
                break;

                case IDC_VC_DISPLAY_ADAPTER:
                {
                    if(HIWORD(wParam) == CBN_SELCHANGE)
                    {
                        RefreshDisplayAdapter();
                        break;
                    }
                }
                break;

                case IDC_VC_D3D_DEVICE:
                {
                    if(HIWORD(wParam) == CBN_SELCHANGE)
                    {
                        RefreshDirect3DDevice();
                        break;
                    }
                }
                break;
            }
        }
        break;
    }
    return FALSE;
}

VOID RefreshDisplayAdapter()
{
    /*! save configured display adapter */
    {
        DWORD dwOld = g_XBVideo.GetDisplayAdapter();

        DWORD dwDisplayAdapter = (DWORD)SendMessage(g_hDisplayAdapter, CB_GETCURSEL, 0, 0);

        if(dwDisplayAdapter != dwOld)
        {
            g_bHasChanges = TRUE;

            g_XBVideo.SetDisplayAdapter(dwDisplayAdapter);
        }
    }

    /*! generate list of device types */
    {
        /*! device types */
        static const XTL::D3DDEVTYPE devType[2] = { XTL::D3DDEVTYPE_HAL, XTL::D3DDEVTYPE_REF };

        /*! human readable device types */
        static const char *szDevType[2] = { "Direct3D HAL (Hardware Accelerated)", "Direct3D REF (Software)" };

        /*! clear device listbox */
        SendMessage(g_hDirect3DDevice, CB_RESETCONTENT, 0, 0);

        /*! step through devices types */
        for(uint32 d=0;d<2;d++)
        {
            XTL::D3DCAPS8 Caps;

            /*! verify device is available */
            if(g_pD3D8->GetDeviceCaps(g_XBVideo.GetDisplayAdapter(), devType[d], &Caps) == D3D_OK)
            {
                /*! add device to list */
                SendMessage(g_hDirect3DDevice, CB_ADDSTRING, 0, (LPARAM)szDevType[d]);
            }
        }
    }

    /*! activate configured device */
    SendMessage(g_hDirect3DDevice, CB_SETCURSEL, g_XBVideo.GetDirect3DDevice(), 0);

    /*! refresh based on new device selection */
    RefreshDirect3DDevice();

    return;
}

VOID RefreshDirect3DDevice()
{
    DWORD dwVideoResolution = 0;

    /*! save configured device */
    {
        DWORD dwOld = g_XBVideo.GetDirect3DDevice();

        DWORD dwDirect3DDevice = SendMessage(g_hDirect3DDevice, CB_GETCURSEL, 0, 0);

        if(dwDirect3DDevice != dwOld)
        {
            g_bHasChanges = TRUE;

            g_XBVideo.SetDirect3DDevice(dwDirect3DDevice);
        }
    }

    /*! generate list of display modes */
    {
        /*! clear display mode listbox */
        SendMessage(g_hVideoResolution, CB_RESETCONTENT, 0, 0);

        /*! enumerate display modes */
        {
            uint32 dwAdapterModeCount = g_pD3D8->GetAdapterModeCount(g_XBVideo.GetDisplayAdapter());

            SendMessage(g_hVideoResolution, CB_ADDSTRING, 0, (LPARAM)"Automatic (Default)");

            /*! enumerate through available adapter modes */
            for(uint32 v=0;v<dwAdapterModeCount;v++)
            {
                const char *szFormat = "<unknown>";

                XTL::D3DDISPLAYMODE displayMode;

                g_pD3D8->EnumAdapterModes(g_XBVideo.GetDisplayAdapter(), v, &displayMode);

                switch(displayMode.Format)
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

                /*! add display mode to list */
                {
                    char szBuffer[260];

                    if(displayMode.RefreshRate == 0)
                    {
                        sprintf(szBuffer, "%d x %d %s", displayMode.Width, displayMode.Height, szFormat);
                    }
                    else
                    {
                        sprintf(szBuffer, "%d x %d %s (%d hz)", displayMode.Width, displayMode.Height, szFormat, displayMode.RefreshRate);
                    }

                    /*! if current mode is the configured video resolution, activate it in the list */
                    if(strcmp(szBuffer, g_XBVideo.GetVideoResolution()) == 0)
                    {
                        dwVideoResolution = v+1;
                    }

                    SendMessage(g_hVideoResolution, CB_ADDSTRING, 0, (LPARAM)szBuffer);
                }
            }
        }
    }

    /*! activate configured display mode */
    SendMessage(g_hVideoResolution, CB_SETCURSEL, dwVideoResolution, 0);

    return;
}
