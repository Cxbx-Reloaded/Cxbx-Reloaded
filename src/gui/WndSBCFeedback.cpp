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
// *  You should have7 recieved a copy of the GNU General Public License
// *  along with this program; see the file COPYING.
// *  If not, write to the Free Software Foundation, Inc.,
// *  59 Temple Place - Suite 330, Bostom, MA 02111-1307, USA.
// *
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#include "WndSBCFeedback.h"
#include "DlgAbout.h"
#include "DlgControllerConfig.h"
#include "DlgVideoConfig.h"
#include "DlgAudioConfig.h"
#include "DlgEepromConfig.h"
#include "DlgXboxControllerPortMapping.h"
#include "Common/XbePrinter.h" // For DumpInformation
#include "CxbxKrnl/EmuShared.h"
#include "DlgVirtualSBCFeedback.h"
#include "ResCxbx.h"
#include "CxbxVersion.h"
#include "Shlwapi.h"
#include <multimon.h>

#include <io.h>

#include <sstream> // for std::stringstream
#include <fstream>
#include <iostream>
#include "CxbxKrnl/xxhash32.h" // for XXHash32::hash

#define XBOX_LED_FLASH_PERIOD 176 // if you know a more accurate value, put it here

static int gameLogoWidth, gameLogoHeight;
static int splashLogoWidth, splashLogoHeight;


void WndSBCFeedback::InitializeSettings() {
		//g_SaveOnExit = false;
	
}

void WndSBCFeedback::ResizeWindow(HWND hwnd, bool bForGUI)
{
    ;
}

WndSBCFeedback::WndSBCFeedback(HINSTANCE x_hInstance) :
	Wnd(x_hInstance),
	m_bCreated(false)
{
    // initialize members
    {
        m_classname = "WndSBCFeedback";
        m_wndname   = "Cxbx-Reloaded " _CXBX_VERSION;

    }

    // load configuration from registry
    {
        DWORD   dwDisposition, dwType, dwSize;
        HKEY    hKey;
    }

    return;
}

WndSBCFeedback::~WndSBCFeedback()
{
    // save configuration to registry

}

// window message processing procedure
LRESULT CALLBACK WndSBCFeedback::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_CREATE:
        {
            // initialize menu
            {
                HMENU hMenu = LoadMenu(m_hInstance, MAKEINTRESOURCE(IDR_MAINMENU));

                SetMenu(hwnd, hMenu);
            }

			// Set window size to GUI dimensions
			ResizeWindow(hwnd, /*bForGUI=*/true);


            SetClassLong(hwnd, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CXBX)));
			DragAcceptFiles(hwnd, TRUE);

			// Allow Drag and Drop if Cxbx is run with elevated privileges on Windows Vista and above

			ChangeWindowMessageFilterEx(hwnd, WM_DROPFILES, MSGFLT_ALLOW, nullptr);
			ChangeWindowMessageFilterEx(hwnd, WM_COPYDATA, MSGFLT_ALLOW, nullptr);
			ChangeWindowMessageFilterEx(hwnd, 0x0049, MSGFLT_ALLOW, nullptr);

            m_bCreated = true;
        }
        break;

        case WM_PARENTNOTIFY:
        {
            switch(LOWORD(wParam))
            {
                case WM_CREATE:
                {
                }
                break;

                case WM_DESTROY:
                {
					// (HWND)HIWORD(wParam) seems to be NULL, so we can't compare to m_hwndChild
                }
                break;
            }
        };
		break; // added per PVS suggestion.

		case WM_TIMER:
		{
			switch (wParam)
			{
			}
		}
		break;

        case WM_SYSKEYDOWN:
        {
            if(m_hwndDlg != NULL)
            {
                SendMessage(m_hwndDlg, uMsg, wParam, lParam);
            }
        };
		break; // added per PVS suggestion.

        case WM_PAINT:
        {
            static bool s_bInitMenu = true;

            // initialize menus if they haven't been initialized already
            if(s_bInitMenu)
            {
                s_bInitMenu = false;
            }

        }
        break;

        case WM_KEYDOWN:
        {
            switch(wParam)
            {
                case VK_F5:
                {
                }
                break;

                default:
                {
                    if(m_hwndDlg != NULL)
                    {
                        SendMessage(m_hwndDlg, uMsg, wParam, lParam);
                    }
                }
            }
        }
        break;

		case WM_DROPFILES:
		{
			if(!m_bIsStarted) {
                ;
			}
		}
		break;

        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
            }
        }
        case WM_CLOSE:
        {
        }
        break;

        case WM_DESTROY:
        {
            PostQuitMessage(0);
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

/*
void WndSBCFeedback::UpdateCaption()
{
	char AsciiTitle[MAX_PATH];

	int i = sprintf(AsciiTitle, "Cxbx-Reloaded %s", _CXBX_VERSION);
	if (m_Xbe != nullptr) {
		if (m_bIsStarted) {
			i += sprintf(AsciiTitle + i, " : Emulating ");
		}
		else {
			i += sprintf(AsciiTitle + i, " : Loaded ");
		}

		i += sprintf(AsciiTitle + i, m_Xbe->m_szAsciiTitle);

		// Append FPS menu text
		HMENU hMenu = GetMenu(m_hwnd);
		MENUITEMINFO mii;
		mii.cbSize = sizeof mii;
		mii.fMask = MIIM_STRING;
		char sMenu[32];
		mii.dwTypeData = &sMenu[0];
		SetMenuItemInfo(hMenu, ID_FPS, FALSE, &mii);
	}

	SetWindowText(m_hwnd, AsciiTitle);
}
*/
