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

#define LOG_PREFIX CXBXR_MODULE::GUI

// Without this, you'll get a ton of errors from the std library for some unknown reason...
#include "Logging.h"

#include "common\Settings.hpp" // for g_Settings

#include "DlgVideoConfig.h"
#include "resource/ResCxbx.h"

#include "core\hle\D3D8\Direct3D9\Direct3D9.h" // For IDirect3D

/*! windows dialog procedure */
static INT_PTR CALLBACK DlgVideoConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
/*! refresh UI based on selected display adapter */
static VOID RefreshDisplayAdapter();
/*! refresh UI based on selected device */
static VOID RefreshDirect3DDevice();
/*! refrehs UI based on selected Render Resolution */
static VOID RefreshRenderResolution();

/*! direct3d instance */
static IDirect3D *g_pDirect3D = nullptr;
/*! video configuration */
static Settings::s_video g_XBVideo;
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
/*! handle to scale factor window*/
static HWND g_hRenderResolution = NULL;

#pragma optimize("", off)

VOID ShowVideoConfig(HWND hwnd)
{
    /*! reset changes flag */
    g_bHasChanges = FALSE;

    /*! retrieve video configuration */
    g_XBVideo = g_Settings->m_video;

    /*! initialize direct3d */
    {
        g_pDirect3D = Direct3DCreate(D3D_SDK_VERSION);

        if(g_pDirect3D == 0) { goto cleanup; }

        g_dwAdapterCount = g_pDirect3D->GetAdapterCount();
    }

    /*! show dialog box */
    DialogBox(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_VIDEO_CFG), hwnd, DlgVideoConfigProc);

cleanup:

    /*! cleanup direct3d */
    {
        if(g_pDirect3D != 0)
        {
            g_pDirect3D->Release();
            g_pDirect3D = 0;
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
			g_hRenderResolution = GetDlgItem(hWndDlg, IDC_VC_RENDER_RESOLUTION);

            /*! set window icon */
            SetClassLong(hWndDlg, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_CXBX)));

            /*! enumerate display adapters */
            {
                SendMessage(g_hDisplayAdapter, CB_RESETCONTENT, 0, 0);

                for(uint32_t v=0;v<g_dwAdapterCount;v++)
                {
                    D3DADAPTER_IDENTIFIER adapterIdentifier;

                    g_pDirect3D->GetAdapterIdentifier(v, D3DENUM_NO_WHQL_LEVEL, &adapterIdentifier);
					SendMessage(g_hDisplayAdapter, CB_ADDSTRING, 0, (LPARAM)adapterIdentifier.Description);
                }
            }

            /*! activate configured display adapter */
            SendMessage(g_hDisplayAdapter, CB_SETCURSEL, g_XBVideo.adapter, 0);

            /*! refresh UI based on currently selected display adapter */
            RefreshDisplayAdapter();


			/*! generate list of render scale factors */
			{
				DWORD dwRenderResolution = g_XBVideo.renderScaleFactor;
				SendMessage(g_hRenderResolution, CB_ADDSTRING, 0, (LPARAM)"1x (Native)");
				SendMessage(g_hRenderResolution, CB_ADDSTRING, 0, (LPARAM)"2x (1280x960)");
				SendMessage(g_hRenderResolution, CB_ADDSTRING, 0, (LPARAM)"3x (1920x1440)");
				SendMessage(g_hRenderResolution, CB_ADDSTRING, 0, (LPARAM)"4x (2560x1920)");
				SendMessage(g_hRenderResolution, CB_ADDSTRING, 0, (LPARAM)"5x (3200x2400)");
				SendMessage(g_hRenderResolution, CB_ADDSTRING, 0, (LPARAM)"6x (3840x2880)");
				SendMessage(g_hRenderResolution, CB_ADDSTRING, 0, (LPARAM)"7x (4480x3360)");
				SendMessage(g_hRenderResolution, CB_ADDSTRING, 0, (LPARAM)"8x (5120x3840)");
				SendMessage(g_hRenderResolution, CB_SETCURSEL, dwRenderResolution - 1, 0);
			}

            /*! check appropriate options */
            {
                SendMessage(GetDlgItem(hWndDlg, IDC_CV_FULLSCREEN), BM_SETCHECK, (WPARAM)g_XBVideo.bFullScreen, 0);

                SendMessage(GetDlgItem(hWndDlg, IDC_CV_VSYNC), BM_SETCHECK, (WPARAM)g_XBVideo.bVSync, 0);

				SendMessage(GetDlgItem(hWndDlg, IDC_CV_HARDWAREYUV), BM_SETCHECK, (WPARAM)g_XBVideo.bHardwareYUV, 0);
            }
        }
        break;

        case WM_CLOSE:
        {
            /*! if changes have been made, check if the user wants to save them */
            if(g_bHasChanges)
            {
                PopupReturn ret = PopupQuestion(hWndDlg, "Do you wish to apply your changes?");

                switch(ret)
                {
                    case PopupReturn::Yes:
                        PostMessage(hWndDlg, WM_COMMAND, IDC_VC_ACCEPT, 0);
                        break;
                    case PopupReturn::No:
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

                        strncpy(g_XBVideo.szVideoResolution, szBuffer, std::size(g_XBVideo.szVideoResolution));

                        // For prevent buffer overflow whenever copy string from another location.
                        g_XBVideo.szVideoResolution[std::size(g_XBVideo.szVideoResolution)-1] = '\0';
                    }

					/*! save render resolution configuration */
					{
						HWND hRenderResolution = GetDlgItem(hWndDlg, IDC_VC_RENDER_RESOLUTION);
						LRESULT lRet = SendMessage(hRenderResolution, CB_GETCURSEL, 0, 0) + 1;
						g_XBVideo.renderScaleFactor = lRet;
					}

                    /*! save fullscreen/vsync/YUV options */
                    {
                        LRESULT lRet = SendMessage(GetDlgItem(hWndDlg, IDC_CV_FULLSCREEN), BM_GETCHECK, 0, 0);

                        g_XBVideo.bFullScreen = (lRet == BST_CHECKED);

                        lRet = SendMessage(GetDlgItem(hWndDlg, IDC_CV_VSYNC), BM_GETCHECK, 0, 0);

                        g_XBVideo.bVSync = (lRet == BST_CHECKED);

						lRet = SendMessage(GetDlgItem(hWndDlg, IDC_CV_HARDWAREYUV), BM_GETCHECK, 0, 0);

						g_XBVideo.bHardwareYUV = (lRet == BST_CHECKED);
                    }

                    /*! save video configuration */
                    g_Settings->m_video = g_XBVideo;

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

				case IDC_VC_RENDER_RESOLUTION:
				{
					if (HIWORD(wParam) == CBN_SELCHANGE)
					{
						RefreshRenderResolution();
						break;
					}
				}
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
        DWORD dwOld = g_XBVideo.adapter;

        DWORD dwDisplayAdapter = (DWORD)SendMessage(g_hDisplayAdapter, CB_GETCURSEL, 0, 0);

        if(dwDisplayAdapter != dwOld)
        {
            g_bHasChanges = TRUE;

            g_XBVideo.adapter = dwDisplayAdapter;
        }
    }

    /*! generate list of device types */
    {
        /*! device types */
        static const D3DDEVTYPE devType[2] = { D3DDEVTYPE_HAL, D3DDEVTYPE_REF };

        /*! human readable device types */
        static const char *szDevType[2] = { "Direct3D HAL (Hardware Accelerated)", "Direct3D REF (Software)" };

        /*! clear device listbox */
        SendMessage(g_hDirect3DDevice, CB_RESETCONTENT, 0, 0);

        /*! step through devices types */
        for(uint32_t d=0;d<2;d++)
        {
            D3DCAPS Caps;

            /*! verify device is available */
            if(g_pDirect3D->GetDeviceCaps(g_XBVideo.adapter, devType[d], &Caps) == D3D_OK)
            {
                /*! add device to list */
                SendMessage(g_hDirect3DDevice, CB_ADDSTRING, 0, (LPARAM)szDevType[d]);
            }
        }
    }

    /*! activate configured device */
    SendMessage(g_hDirect3DDevice, CB_SETCURSEL, g_XBVideo.direct3DDevice, 0);

    /*! refresh based on new device selection */
    RefreshDirect3DDevice();

    return;
}

VOID RefreshDirect3DDevice()
{
    DWORD dwVideoResolution = 0;

    /*! save configured device */
    {
        DWORD dwOld = g_XBVideo.direct3DDevice;

        DWORD dwDirect3DDevice = SendMessage(g_hDirect3DDevice, CB_GETCURSEL, 0, 0);

        if(dwDirect3DDevice != dwOld)
        {
            g_bHasChanges = TRUE;

            g_XBVideo.direct3DDevice = dwDirect3DDevice;
        }
    }

    /*! generate list of display modes */
    {
        /*! clear display mode listbox */
        SendMessage(g_hVideoResolution, CB_RESETCONTENT, 0, 0);

        /*! enumerate display modes */
        {
            uint32_t dwAdapterModeCount = g_pDirect3D->GetAdapterModeCount(
                g_XBVideo.adapter
				, D3DFMT_X8R8G8B8
			);

            SendMessage(g_hVideoResolution, CB_ADDSTRING, 0, (LPARAM)"Automatic (Xbox Default)");

            /*! enumerate through available adapter modes */
            for(uint32_t v=0;v<dwAdapterModeCount;v++)
            {
                const char *szFormat = "<unknown>";

                D3DDISPLAYMODE displayMode;

                g_pDirect3D->EnumAdapterModes(
					g_XBVideo.adapter,
					D3DFMT_X8R8G8B8,
					v,
					&displayMode
				);

                switch(displayMode.Format)
                {
                    case D3DFMT_X1R5G5B5:
                        szFormat = "16bit x1r5g5b5";
                        break;
                    case D3DFMT_R5G6B5:
                        szFormat = "16bit r5g6r5";
                        break;
                    case D3DFMT_X8R8G8B8:
                        szFormat = "32bit x8r8g8b8";
                        break;
                    case D3DFMT_A8R8G8B8:
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
                    if(strcmp(szBuffer, g_XBVideo.szVideoResolution) == 0)
                    {
                        dwVideoResolution = v+1;
                    }

                    SendMessage(g_hVideoResolution, CB_ADDSTRING, 0, (LPARAM)szBuffer);
                }
            }
        }


		SendMessage(g_hVideoResolution, CB_SETCURSEL, dwVideoResolution, 0);
    }

    return;
}

VOID RefreshRenderResolution()
{
	/*! save configured render resolution */
	{
		DWORD dwOld = g_XBVideo.renderScaleFactor;
		DWORD dwRenderScaleFactor = (DWORD)(SendMessage(g_hRenderResolution, CB_GETCURSEL, 0, 0) + 1);

		if (dwRenderScaleFactor != dwOld)
		{
			g_bHasChanges = TRUE;
			g_XBVideo.renderScaleFactor = dwRenderScaleFactor;
		}
	}
}
