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
// *   Cxbx->Win32->Cxbx->WndMain.cpp
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
#include "WndMain.h"
#include "DlgAbout.h"
#include "DlgControllerConfig.h"
#include "DlgVideoConfig.h"
#include "DlgAudioConfig.h"
#include "DlgEepromConfig.h"
#include "Common/XbePrinter.h" // For DumpInformation
#include "CxbxKrnl/EmuShared.h"
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

bool g_SaveOnExit = true;

void ClearHLECache()
{
	std::string cacheDir = std::string(XTL::szFolder_CxbxReloadedData) + "\\HLECache\\";
	std::string fullpath = cacheDir + "*.ini";

	WIN32_FIND_DATA data;
	HANDLE hFind = FindFirstFile(fullpath.c_str(), &data);

	if (hFind != INVALID_HANDLE_VALUE) {
		BOOL bContinue = TRUE;
		do {
			if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
				fullpath = cacheDir + data.cFileName;

				if (!DeleteFile(fullpath.c_str())) {
					break;
				}
			}

			bContinue = FindNextFile(hFind, &data);
		} while (bContinue);

		FindClose(hFind);
	}

	printf("Cleared HLE Cache\n");
}

void WndMain::InitializeSettings() {
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Cxbx-Reloaded", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
		for (int v = 0; v < m_dwRecentXbe; v++) {
			char buffer[32];
			sprintf(buffer, "RecentXbe%d", v);
			RegDeleteValue(hKey, buffer);
		}
		RegDeleteValue(hKey, "CxbxDebug"); RegDeleteValue(hKey, "CxbxDebugFilename");
		RegDeleteValue(hKey, "HackDisablePixelShaders"); RegDeleteValue(hKey, "HackUncapFrameRate");
		RegDeleteValue(hKey, "HackUseAllCores");  RegDeleteValue(hKey, "KrnlDebug");
		RegDeleteValue(hKey, "KrnlDebugFilename"); RegDeleteValue(hKey, "LLEFLAGS");
		RegDeleteValue(hKey, "RecentXbe"); RegDeleteValue(hKey, "XInputEnabled");

		RegDeleteTree(hKey, "XBVideo"); RegDeleteTree(hKey, "XBAudio"); RegDeleteTree(hKey, "XBController");

		RegCloseKey(hKey);

		g_SaveOnExit = false;
	}
}

#define TIMERID_FPS 0
#define TIMERID_LED 1

void WndMain::ResizeWindow(HWND hwnd, bool bForGUI)
{
	RECT desktopRect;
	RECT windowRect;

	// Set the window size back to it's GUI dimensions
	m_w = 640;
	m_h = 480;
	if (!bForGUI) {
		// For emulation, get the configured window dimensions
		XBVideo XBVideoConf;
		g_EmuShared->GetXBVideo(&XBVideoConf);

		const char* resolution = XBVideoConf.GetVideoResolution();
		if (2 != sscanf(resolution, "%d x %d", &m_w, &m_h)) {
			DbgPrintf("Couldn't parse resolution : %s.\n", resolution);
		}
	}

	// TODO : Acknowledge DPI scaling here
	GetWindowRect(GetDesktopWindow(), &desktopRect);

	// Limit width/height to desktop resolution
	int dWidth = desktopRect.right - desktopRect.left;
	int dHeight = desktopRect.bottom - desktopRect.top;
	if (m_w > dWidth)
		m_w = dWidth;
	if (m_h > dHeight)
		m_h = dHeight;

	// Center to desktop
	m_x = desktopRect.left + ((desktopRect.right - desktopRect.left - m_w) / 2);
	m_y = desktopRect.top + ((desktopRect.bottom - desktopRect.top - m_h) / 2);

	// Resize the window so it's client area can contain the requested resolution
	windowRect = { m_x, m_y, m_x + m_w, m_y + m_h };
	AdjustWindowRectEx(&windowRect, GetWindowLong(hwnd, GWL_STYLE), GetMenu(hwnd) != NULL, GetWindowLong(hwnd, GWL_EXSTYLE));
	// TODO : For DPI screens, replace AdjustWindowRectEx by DwmGetWindowAttribute using DWMWA_EXTENDED_FRAME_BOUNDS
	SetWindowPos(hwnd, 0,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		SWP_NOOWNERZORDER | SWP_NOZORDER);
}

WndMain::WndMain(HINSTANCE x_hInstance) :
	Wnd(x_hInstance),
	m_bCreated(false),
	m_Xbe(nullptr),
	m_bXbeChanged(false),
	m_bIsStarted(false),
	m_hwndChild(NULL),
	m_KrnlDebug(DM_NONE),
	m_CxbxDebug(DM_NONE),
	m_FlagsLLE(0),
	m_dwRecentXbe(0)
{
    // initialize members
    {
        m_classname = "WndMain";
        m_wndname   = "Cxbx-Reloaded " _CXBX_VERSION;

        m_XbeFilename = (char*)calloc(1, MAX_PATH);

        m_CxbxDebugFilename = (char*)calloc(1, MAX_PATH);
        m_KrnlDebugFilename = (char*)calloc(1, MAX_PATH);

        for(int v=0;v<RECENT_XBE_SIZE;v++)
            m_szRecentXbe[v] = 0;
    }

    // load configuration from registry
    {
        DWORD   dwDisposition, dwType, dwSize;
        HKEY    hKey;

        if(RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Cxbx-Reloaded", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_QUERY_VALUE, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS)
        {
			LONG result = ERROR_SUCCESS;

			dwType = REG_DWORD; dwSize = sizeof(DWORD);
			result = RegQueryValueEx(hKey, "LLEFLAGS", NULL, &dwType, (PBYTE)&m_FlagsLLE, &dwSize);
			if (result != ERROR_SUCCESS) {
				m_FlagsLLE = 0;
			}

			dwType = REG_DWORD; dwSize = sizeof(DWORD);
			result = RegQueryValueEx(hKey, "XInputEnabled", NULL, &dwType, (PBYTE)&m_XInputEnabled, &dwSize);
			if (result != ERROR_SUCCESS) {
				m_XInputEnabled = 0;
			}

			dwType = REG_DWORD; dwSize = sizeof(DWORD);
			result = RegQueryValueEx(hKey, "HackDisablePixelShaders", NULL, &dwType, (PBYTE)&m_DisablePixelShaders, &dwSize);
			if (result != ERROR_SUCCESS) {
				m_DisablePixelShaders = 0;
			}

			dwType = REG_DWORD; dwSize = sizeof(DWORD);
			result = RegQueryValueEx(hKey, "HackUncapFrameRate", NULL, &dwType, (PBYTE)&m_UncapFramerate, &dwSize);
			if (result != ERROR_SUCCESS) {
				m_UncapFramerate = 0;
			}

			dwType = REG_DWORD; dwSize = sizeof(DWORD);
			result = RegQueryValueEx(hKey, "HackUseAllCores", NULL, &dwType, (PBYTE)&m_UseAllCores, &dwSize);
			if (result != ERROR_SUCCESS) {
				m_UseAllCores = 0;
			}

			dwType = REG_DWORD; dwSize = sizeof(DWORD);
			result = RegQueryValueEx(hKey, "HackSkipRdtscPatching", NULL, &dwType, (PBYTE)&m_SkipRdtscPatching, &dwSize);
			if (result != ERROR_SUCCESS) {
				m_SkipRdtscPatching = 0;
			}

			dwType = REG_DWORD; dwSize = sizeof(DWORD);
			result = RegQueryValueEx(hKey, "HackScaleViewport", NULL, &dwType, (PBYTE)&m_ScaleViewport, &dwSize);
			if (result != ERROR_SUCCESS) {
				m_ScaleViewport = 0;
			}

			dwType = REG_DWORD; dwSize = sizeof(DWORD);
			result = RegQueryValueEx(hKey, "CxbxDebug", NULL, &dwType, (PBYTE)&m_CxbxDebug, &dwSize);
			if (result != ERROR_SUCCESS) {
				m_CxbxDebug = DebugMode::DM_NONE;
			}

			dwType = REG_DWORD; dwSize = sizeof(DWORD);
			result = RegQueryValueEx(hKey, "KrnlDebug", NULL, &dwType, (PBYTE)&m_KrnlDebug, &dwSize);
			if (result != ERROR_SUCCESS) {
				m_KrnlDebug = DebugMode::DM_NONE;
			}

            dwType = REG_DWORD; dwSize = sizeof(DWORD);
			result = RegQueryValueEx(hKey, "RecentXbe", NULL, &dwType, (PBYTE)&m_dwRecentXbe, &dwSize);
			if (result != ERROR_SUCCESS) {
				m_dwRecentXbe = 0;
			}

			dwType = REG_SZ; dwSize = MAX_PATH; ULONG lErrCodeCxbxDebugFilename;
			lErrCodeCxbxDebugFilename = RegQueryValueEx(hKey, "CxbxDebugFilename", NULL, &dwType, (PBYTE)m_CxbxDebugFilename, &dwSize);
			if (lErrCodeCxbxDebugFilename != ERROR_SUCCESS) {
				m_CxbxDebugFilename[0] = '\0';
			}

			dwType = REG_SZ; dwSize = MAX_PATH; LONG lErrCodeKrnlDebugFilename;
			lErrCodeKrnlDebugFilename = RegQueryValueEx(hKey, "KrnlDebugFilename", NULL, &dwType, (PBYTE)m_KrnlDebugFilename, &dwSize);
			if (lErrCodeKrnlDebugFilename != ERROR_SUCCESS) {
				m_KrnlDebugFilename[0] = '\0';
			}

			// Prevent using an incorrect path from the registry if the debug folders have been moved
			if (m_CxbxDebug == DM_FILE)
			{
				if(lErrCodeCxbxDebugFilename == ERROR_FILE_NOT_FOUND || strlen(m_CxbxDebugFilename) == 0)
				{
					m_CxbxDebug = DM_NONE;
				}
				else
				{
					char *CxbxDebugPath = (char*)calloc(1, MAX_PATH);
					char *CxbxDebugName = (char*)calloc(1, MAX_PATH);

					strcpy(CxbxDebugName, strrchr(m_CxbxDebugFilename, '\\'));

					if(strlen(m_CxbxDebugFilename) < strlen(CxbxDebugName))
					{
						memset((char*)m_CxbxDebugFilename, '\0', MAX_PATH);
						m_CxbxDebug = DM_NONE;
					}
					else
					{
						strncpy(CxbxDebugPath, m_CxbxDebugFilename, strlen(m_CxbxDebugFilename) - strlen(CxbxDebugName));
						if(PathFileExists((LPCSTR)CxbxDebugPath) == FALSE)
						{
							memset((char*)m_CxbxDebugFilename, '\0', MAX_PATH);
							m_CxbxDebug = DM_NONE;
						}
					}
					free(CxbxDebugPath);
					free(CxbxDebugName);
				}
			}

			if (m_KrnlDebug == DM_FILE)
			{
				if(lErrCodeKrnlDebugFilename == ERROR_FILE_NOT_FOUND || strlen(m_KrnlDebugFilename) == 0)
				{
					m_KrnlDebug = DM_NONE;
				}
				else
				{
					char *KrnlDebugPath = (char*)calloc(1, MAX_PATH);
					char *KrnlDebugName = (char*)calloc(1, MAX_PATH);

					strcpy(KrnlDebugName, strrchr(m_KrnlDebugFilename, '\\'));

					if(strlen(m_KrnlDebugFilename) < strlen(KrnlDebugName))
					{
						memset((char*)m_KrnlDebugFilename, '\0', MAX_PATH);
						m_KrnlDebug = DM_NONE;
					}
					else
					{
						strncpy(KrnlDebugPath, m_KrnlDebugFilename, strlen(m_KrnlDebugFilename) - strlen(KrnlDebugName));
						if(PathFileExists((LPCSTR)KrnlDebugPath) == FALSE)
						{
							memset((char*)m_KrnlDebugFilename, '\0', MAX_PATH);
							m_KrnlDebug = DM_NONE;
						}
					}
					free(KrnlDebugPath);
					free(KrnlDebugName);
				}
			}

            int v=0;

            for(v=0;v<m_dwRecentXbe;v++)
            {
                char buffer[32];

                sprintf(buffer, "RecentXbe%d", v);

                m_szRecentXbe[v] = (char*)calloc(1, MAX_PATH);

                dwType = REG_SZ; dwSize = MAX_PATH;
                result = RegQueryValueEx(hKey, buffer, NULL, &dwType, (PBYTE)m_szRecentXbe[v], &dwSize);
				if (result != ERROR_SUCCESS) {
					m_szRecentXbe[v] = "";
				}

            }

            RegCloseKey(hKey);
        }
    }

    return;
}

WndMain::~WndMain()
{
    // save configuration to registry
    if (g_SaveOnExit) {
        DWORD   dwDisposition, dwType, dwSize;
        HKEY    hKey;

        if(RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Cxbx-Reloaded", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS)
        {
            int v=0;

            for(v=0;v<m_dwRecentXbe;v++)
            {
                char buffer[32];

                sprintf(buffer, "RecentXbe%d", v);

                dwType = REG_SZ; dwSize = MAX_PATH;

                RegSetValueEx(hKey, buffer, 0, dwType, (PBYTE)m_szRecentXbe[v], dwSize);

                free(m_szRecentXbe[v]);
            }

			dwType = REG_DWORD; dwSize = sizeof(DWORD);
			RegSetValueEx(hKey, "LLEFLAGS", 0, dwType, (PBYTE)&m_FlagsLLE, dwSize);

			dwType = REG_DWORD; dwSize = sizeof(DWORD);
			RegSetValueEx(hKey, "XInputEnabled", 0, dwType, (PBYTE)&m_XInputEnabled, dwSize);

			dwType = REG_DWORD; dwSize = sizeof(DWORD);
			RegSetValueEx(hKey, "HackDisablePixelShaders", 0, dwType, (PBYTE)&m_DisablePixelShaders, dwSize);

			dwType = REG_DWORD; dwSize = sizeof(DWORD);
			RegSetValueEx(hKey, "HackUncapFrameRate", 0, dwType, (PBYTE)&m_UncapFramerate, dwSize);

			dwType = REG_DWORD; dwSize = sizeof(DWORD);
			RegSetValueEx(hKey, "HackUseAllCores", 0, dwType, (PBYTE)&m_UseAllCores, dwSize);

			dwType = REG_DWORD; dwSize = sizeof(DWORD);
			RegSetValueEx(hKey, "HackSkipRdtscPatching", 0, dwType, (PBYTE)&m_SkipRdtscPatching, dwSize);

			dwType = REG_DWORD; dwSize = sizeof(DWORD);
			RegSetValueEx(hKey, "HackScaleViewport", 0, dwType, (PBYTE)&m_ScaleViewport, dwSize);

			dwType = REG_DWORD; dwSize = sizeof(DWORD);
            RegSetValueEx(hKey, "CxbxDebug", 0, dwType, (PBYTE)&m_CxbxDebug, dwSize);

            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            RegSetValueEx(hKey, "KrnlDebug", 0, dwType, (PBYTE)&m_KrnlDebug, dwSize);

            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            RegSetValueEx(hKey, "RecentXbe", 0, dwType, (PBYTE)&m_dwRecentXbe, dwSize);

            dwType = REG_SZ; dwSize = MAX_PATH;
            RegSetValueEx(hKey, "CxbxDebugFilename", 0, dwType, (PBYTE)m_CxbxDebugFilename, dwSize);

            dwType = REG_SZ; dwSize = MAX_PATH;
            RegSetValueEx(hKey, "KrnlDebugFilename", 0, dwType, (PBYTE)m_KrnlDebugFilename, dwSize);
        }
    }

    // cleanup allocations
    {
        delete m_Xbe;

        free(m_XbeFilename);

        free(m_CxbxDebugFilename);
        free(m_KrnlDebugFilename);
    }
}

// window message processing procedure
LRESULT CALLBACK WndMain::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

			// initialize back buffer
            {
                HDC hDC = GetDC(hwnd);

                m_SplashBmp = CreateCompatibleBitmap(hDC, m_w, m_h);

				// create Xbox LED bitmap
				{
					m_xBmp = GetSystemMetrics(SM_CXMENUCHECK);
					m_yBmp = GetSystemMetrics(SM_CYMENUCHECK);
					m_LedDC = CreateCompatibleDC(hDC);
					m_LedBmp = CreateCompatibleBitmap(hDC, m_xBmp, m_yBmp);
					m_Brushes[XBOX_LED_COLOUR_OFF] = CreateSolidBrush(RGB(0, 0, 0));
					m_Brushes[XBOX_LED_COLOUR_GREEN] = CreateSolidBrush(RGB(0, 255, 0));
					m_Brushes[XBOX_LED_COLOUR_RED] = CreateSolidBrush(RGB(255, 0, 0));
					m_Brushes[XBOX_LED_COLOUR_ORANGE] = CreateSolidBrush(RGB(255, 165, 0));
					m_Pens[XBOX_LED_COLOUR_OFF] = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
					m_Pens[XBOX_LED_COLOUR_GREEN] = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
					m_Pens[XBOX_LED_COLOUR_RED] = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
					m_Pens[XBOX_LED_COLOUR_ORANGE] = CreatePen(PS_SOLID, 1, RGB(255, 165, 0));
					DrawLedBitmap(hwnd, true);
				}

                splashLogoWidth = 500;
                splashLogoHeight = 113;
                m_SplashBmp = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDR_JPEG_SPLASH), IMAGE_BITMAP, 0, 0, 0);

                m_LogoBmp  = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_LOGO), IMAGE_BITMAP, 0, 0, 0);

                m_SplashDC   = CreateCompatibleDC(hDC);
                m_LogoDC   = CreateCompatibleDC(hDC);
				
                m_OrigBmp  = (HBITMAP)SelectObject(m_SplashDC, m_SplashBmp);
                m_OrigLogo = (HBITMAP)SelectObject(m_LogoDC, m_LogoBmp);

                m_BackgroundColor = CreateSolidBrush(RGB(48, 48, 48));

                if(hDC != NULL)
                    ReleaseDC(hwnd, hDC);
            }

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
					CreateThread(NULL, NULL, CrashMonitorWrapper, (void*)this, NULL, NULL); // create the crash monitoring thread 
					if (m_hwndChild == NULL) {
						float fps = 0.0f;
						float mspf = 0.0f;
						int LedSequence[4] = { XBOX_LED_COLOUR_GREEN, XBOX_LED_COLOUR_GREEN, XBOX_LED_COLOUR_GREEN, XBOX_LED_COLOUR_GREEN };
						g_EmuShared->SetCurrentMSpF(&mspf);
						g_EmuShared->SetCurrentFPS(&fps);
						g_EmuShared->SetLedSequence(LedSequence);
						SetTimer(hwnd, TIMERID_FPS, 1000, (TIMERPROC)NULL);
						SetTimer(hwnd, TIMERID_LED, XBOX_LED_FLASH_PERIOD, (TIMERPROC)NULL);
						m_hwndChild = GetWindow(hwnd, GW_CHILD); // (HWND)HIWORD(wParam) seems to be NULL
						UpdateCaption();
						RefreshMenus();
					}
					else
					{
						m_hwndChild = GetWindow(hwnd, GW_CHILD);
					}
                }
                break;

                case WM_DESTROY:
                {
					// (HWND)HIWORD(wParam) seems to be NULL, so we can't compare to m_hwndChild
					if (m_hwndChild != NULL) { // Let's hope this signal originated from the only child window
						KillTimer(hwnd, TIMERID_FPS);
						KillTimer(hwnd, TIMERID_LED);
						m_hwndChild = NULL;
						StopEmulation();
						DrawLedBitmap(hwnd, true);
					}
                }
                break;
            }
        };
		break; // added per PVS suggestion.

		case WM_TIMER:
		{
			switch (wParam)
			{
				case TIMERID_FPS:
				{
					UpdateCaption();
				}
				break;

				case TIMERID_LED:
				{
					DrawLedBitmap(hwnd, false);
				}
				break;
			}
		}
		break;

        case WM_SYSKEYDOWN:
        {
            if(m_hwndChild != NULL)
            {
                SendMessage(m_hwndChild, uMsg, wParam, lParam);
            }
        };
		break; // added per PVS suggestion.

        case WM_PAINT:
        {
            static bool s_bInitMenu = true;

            // initialize menus if they haven't been initialized already
            if(s_bInitMenu)
            {
                UpdateRecentFiles();

                RefreshMenus();

                UpdateDebugConsoles();

                s_bInitMenu = false;
            }

            PAINTSTRUCT ps;

            BeginPaint(hwnd, &ps);

            HDC hDC = GetDC(hwnd);

            // draw splash / logo / status
            {
                static const int nLogoBmpW = 100, nLogoBmpH = 17;

                RECT bkRect = ps.rcPaint;

                bkRect.bottom -= nLogoBmpH + 10;

                FillRect(hDC, &bkRect, m_BackgroundColor);

                bkRect.top = bkRect.bottom;
                bkRect.bottom += nLogoBmpH + 10;

                FillRect(hDC, &bkRect, m_Brushes[0]);

                BitBlt(hDC, m_w/2 - splashLogoWidth/2, m_h/2 - splashLogoHeight, splashLogoWidth, splashLogoHeight, m_SplashDC, 0, 0, SRCCOPY);

				BitBlt(hDC, m_w - gameLogoWidth - 3, m_h - nLogoBmpH - 12 - gameLogoHeight, gameLogoWidth, gameLogoHeight, m_GameLogoDC, 0, 0, SRCCOPY);

                BitBlt(hDC, m_w-nLogoBmpW-4, m_h-nLogoBmpH-4, nLogoBmpW, nLogoBmpH, m_LogoDC, 0, 0, SRCCOPY);

                int nHeight = -MulDiv(8, GetDeviceCaps(hDC, LOGPIXELSY), 72);

                HFONT hFont = CreateFont(nHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_ROMAN, "Verdana");

                HGDIOBJ tmpObj = SelectObject(hDC, hFont);

                SetBkColor(hDC, RGB(0,0,0));

                SetTextColor(hDC, RGB(255,255,255));

                char buffer[255];

                if(m_Xbe != nullptr && m_Xbe->HasError())
                    sprintf(buffer, "%s Loaded!", m_Xbe->m_szAsciiTitle);
                else
                    sprintf(buffer, "%s", "Disclaimer: Cxbx-Reloaded has no affiliation with Microsoft");

                RECT rect = {0, m_h-15-5, m_w-100-4-69, m_h-5};

                ExtTextOut(hDC, 4, m_h-15-5, ETO_OPAQUE, &rect, buffer, strlen(buffer), 0);

                SelectObject(hDC, tmpObj);

                DeleteObject(hFont);
            }

            if(hDC != NULL)
                ReleaseDC(hwnd, hDC);

            EndPaint(hwnd, &ps);
        }
        break;

        case WM_KEYDOWN:
        {
            switch(wParam)
            {
                case VK_F5:
                {
					// Try to open the most recent Xbe if none is opened yet :
					if (m_Xbe == nullptr)
						OpenMRU(0);

					if (m_Xbe != nullptr)
						if (!m_bIsStarted)
							StartEmulation(hwnd);
                }
                break;

                case VK_F6:
                {
					if(m_bIsStarted)
                        StopEmulation();
                }
                break;

				case VK_F7:
				{
					// Try to open the dashboard xbe if none is opened yet :
					if (!m_bIsStarted)
					{
						if (m_Xbe != nullptr) { CloseXbe(); }

						OpenDashboard();
					}
				}
				break;

				case VK_F9:
				{
					// Try to open the most recent Xbe if none is opened yet :
					if (!m_bIsStarted) {
						if (m_Xbe == nullptr)
							OpenMRU(0);

						if (m_Xbe != nullptr)
							StartEmulation(hwnd, debuggerOn);
					}
				}
				break;

                default:
                {
                    if(m_hwndChild != NULL)
                    {
                        SendMessage(m_hwndChild, uMsg, wParam, lParam);
                    }
                }
            }
        }
        break;

		case WM_DROPFILES:
		{
			if(!m_bIsStarted) {
				char DroppedXbeFilename[MAX_PATH];
				DragQueryFile((HDROP)wParam, 0, DroppedXbeFilename, MAX_PATH);
				OpenXbe(DroppedXbeFilename);
			}
		}
		break;

        case WM_COMMAND:
        {
			switch (LOWORD(wParam))
			{
			case ID_FILE_OPEN_XBE:
			{
				OPENFILENAME ofn = { 0 };

				char filename[MAX_PATH] = { 0 };

				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = m_hwnd;
				ofn.lpstrFilter = "Xbox Executables (*.xbe)\0*.xbe\0";
				ofn.lpstrFile = filename;
				ofn.nMaxFile = MAX_PATH;
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

				if (GetOpenFileName(&ofn) == TRUE)
				{
					OpenXbe(ofn.lpstrFile);
				}
			}
			break;

			case ID_FILE_CLOSE_XBE:
				CloseXbe();
				break;


			case ID_FILE_OPEN_DASHBOARD:
			{
				if (m_Xbe != nullptr) { CloseXbe(); }

				OpenDashboard();
			}
			break;

			case ID_FILE_SAVEXBEFILE:
			{
				if (m_XbeFilename[0] == '\0')
					SaveXbeAs();
				else
					SaveXbe(m_XbeFilename);
			}
			break;

			case ID_FILE_SAVEXBEFILEAS:
				SaveXbeAs();
				break;

			case ID_FILE_RXBE_0:
			case ID_FILE_RXBE_1:
			case ID_FILE_RXBE_2:
			case ID_FILE_RXBE_3:
			case ID_FILE_RXBE_4:
			case ID_FILE_RXBE_5:
			case ID_FILE_RXBE_6:
			case ID_FILE_RXBE_7:
			case ID_FILE_RXBE_8:
			case ID_FILE_RXBE_9:
			{
				OpenMRU(LOWORD(wParam) - ID_FILE_RXBE_0);
			}
			break;

			case ID_FILE_EXIT:
				SendMessage(hwnd, WM_CLOSE, 0, 0);
				break;

			case ID_EDIT_LOGOBITMAP_EXPORT:
			{
				OPENFILENAME ofn = { 0 };

				char filename[MAX_PATH] = "logo.bmp";

				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = m_hwnd;
				ofn.lpstrFilter = "Bitmap Image Files (*.bmp)\0*.bmp\0";
				ofn.lpstrFile = filename;
				ofn.nMaxFile = MAX_PATH;
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.lpstrDefExt = "bmp";
				ofn.lpstrTitle = "Export Logo Bitmap";
				ofn.Flags = OFN_PATHMUSTEXIST;

				if (GetSaveFileName(&ofn) == TRUE)
				{
					// ask permission to overwrite if file already exists
					if (_access(ofn.lpstrFile, 0) != -1)
					{
						if (MessageBox(m_hwnd, "Overwrite existing file?", "Cxbx-Reloaded", MB_ICONQUESTION | MB_YESNO) != IDYES)
							return TRUE;
					}

					// export logo bitmap
					{
						uint08 i_gray[100 * 17];

						m_Xbe->ExportLogoBitmap(i_gray);

						if (false == m_Xbe->HasError())
						{
							FILE *LogoBitmap = fopen(ofn.lpstrFile, "wb");

							// write bitmap header
							{
								BITMAPFILEHEADER    bmfh;

								bmfh.bfType = *(uint16*)"BM";
								bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) - sizeof(RGBQUAD) + (100 * 17) * sizeof(RGBTRIPLE) + 2;
								bmfh.bfReserved1 = 0;
								bmfh.bfReserved2 = 0;
								bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) - sizeof(RGBQUAD);

								fwrite(&bmfh, sizeof(bmfh), 1, LogoBitmap);
							}

							// write bitmap info
							{
								BITMAPINFO          bmi;

								bmi.bmiHeader.biSize = sizeof(BITMAPINFO) - sizeof(RGBQUAD);
								bmi.bmiHeader.biWidth = 100;
								bmi.bmiHeader.biHeight = -17;
								bmi.bmiHeader.biPlanes = 1;
								bmi.bmiHeader.biBitCount = 24;
								bmi.bmiHeader.biCompression = BI_RGB;
								bmi.bmiHeader.biSizeImage = 0;
								bmi.bmiHeader.biXPelsPerMeter = 0;
								bmi.bmiHeader.biYPelsPerMeter = 0;
								bmi.bmiHeader.biClrUsed = 0;
								bmi.bmiHeader.biClrImportant = 0;

								fwrite(&bmi, sizeof(bmi) - 4, 1, LogoBitmap);
							}

							// write bitmap data
							{
								RGBTRIPLE bmp_data[100 * 17];

								for (uint32 v = 0; v < 100 * 17; v++)
								{
									bmp_data[v].rgbtRed = i_gray[v];
									bmp_data[v].rgbtGreen = i_gray[v];
									bmp_data[v].rgbtBlue = i_gray[v];
								}

								fwrite(bmp_data, 100 * 17 * sizeof(RGBTRIPLE), 1, LogoBitmap);
							}

							// write bitmap padding
							{
								uint16 pad = 0;

								fwrite(&pad, 2, 1, LogoBitmap);
							}

							fclose(LogoBitmap);
						}

						if (m_Xbe->HasError())
							MessageBox(m_hwnd, m_Xbe->GetError().c_str(), "Cxbx-Reloaded", MB_ICONSTOP | MB_OK);
						else
						{
							char buffer[255];

							sprintf(buffer, "%s's logo bitmap was successfully exported.", m_Xbe->m_szAsciiTitle);

							MessageBox(m_hwnd, buffer, "Cxbx-Reloaded", MB_ICONINFORMATION | MB_OK);

							printf("WndMain: %s\n", buffer);
						}
					}
				}
			}
			break;

			case ID_EDIT_LOGOBITMAP_IMPORT:
			{
				OPENFILENAME ofn = { 0 };

				char filename[MAX_PATH] = "*.bmp";

				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = m_hwnd;
				ofn.lpstrFilter = "Bitmap Image Files (*.bmp)\0*.bmp\0";
				ofn.lpstrFile = filename;
				ofn.nMaxFile = MAX_PATH;
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.lpstrDefExt = "bmp";
				ofn.lpstrTitle = "Import Logo Bitmap";
				ofn.Flags = OFN_PATHMUSTEXIST;

				if (GetOpenFileName(&ofn) == TRUE)
				{
					// import logo bitmap
					{
						uint08 i_gray[100 * 17];

						// read bitmap file
						{
							FILE *logo = fopen(ofn.lpstrFile, "rb");

							char *bmp_err = 0;

							// read bitmap header
							if (!bmp_err)
							{
								BITMAPFILEHEADER bmfh;

								fread(&bmfh, sizeof(bmfh), 1, logo);

								if (bmfh.bfType != *(uint16*)"BM")
									bmp_err = "Invalid bitmap file...\n\nonly allows 24 bit bitmaps (100 by 17 pixels) with row order swapped";
								else if (bmfh.bfSize != sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) - sizeof(RGBQUAD) + (100 * 17) * sizeof(RGBTRIPLE) + 2)
									bmp_err = "Invalid bitmap file...\n\nonly allows 24 bit bitmaps (100 by 17 pixels) with row order swapped";
							}

							// read bitmap info
							if (!bmp_err)
							{
								BITMAPINFO bmi;

								fread(&bmi, sizeof(bmi) - 4, 1, logo);

								if (bmi.bmiHeader.biWidth != 100 || bmi.bmiHeader.biHeight != -17)
									bmp_err = "Invalid bitmap file...\n\nonly allows 24 bit bitmaps (100 by 17 pixels) with row order swapped";
							}

							// read bitmap data
							if (!bmp_err)
							{
								RGBTRIPLE bmp_data[100 * 17];

								fread(bmp_data, 100 * 17 * sizeof(RGBTRIPLE), 1, logo);

								for (uint32 c = 0; c < 100 * 17; c++)
									i_gray[c] = (char)(((float)bmp_data[c].rgbtRed + (float)bmp_data[c].rgbtGreen + (float)bmp_data[c].rgbtBlue) / 3.0);
							}

							fclose(logo);

							if (bmp_err != 0)
							{
								MessageBox(m_hwnd, bmp_err, "Cxbx-Reloaded", MB_OK | MB_ICONEXCLAMATION);
								break;
							}
						}

						m_Xbe->ImportLogoBitmap(i_gray);

						if (m_Xbe->HasError())
						{
							MessageBox(m_hwnd, m_Xbe->GetError().c_str(), "Cxbx-Reloaded", MB_ICONSTOP | MB_OK);

							if (m_Xbe->HasFatalError())
							{
								CloseXbe();
							}
							else
							{
								m_Xbe->ClearError();
							}

						}
						else
						{
							m_bXbeChanged = true;

							LoadLogo();

							char buffer[255];

							sprintf(buffer, "%s's logo bitmap was successfully updated.", m_Xbe->m_szAsciiTitle);

							printf("WndMain: %s\n", buffer);

							MessageBox(m_hwnd, buffer, "Cxbx-Reloaded", MB_ICONINFORMATION | MB_OK);
						}
					}
				}
			}
			break;

			case ID_EDIT_PATCH_ALLOW64MB:
			{
				m_bXbeChanged = true;

				m_Xbe->m_Header.dwInitFlags.bLimit64MB = !m_Xbe->m_Header.dwInitFlags.bLimit64MB;

				RefreshMenus();

				if (m_Xbe->m_Header.dwInitFlags.bLimit64MB)
					printf("WndMain: %s was patched to limit to 64MB of memory usage.\n", m_Xbe->m_szAsciiTitle);
				else
					printf("WndMain: %s was patched to allow >64MB of memory usage.\n", m_Xbe->m_szAsciiTitle);
			}
			break;

			case ID_EDIT_PATCH_DEBUGMODE:
			{
				m_bXbeChanged = true;

				// patch to/from debug mode
				if ((m_Xbe->m_Header.dwEntryAddr ^ XOR_EP_RETAIL) > 0x01000000)
				{
					// we're in debug mode, so switch over to retail
					uint32 ep = m_Xbe->m_Header.dwEntryAddr ^ XOR_EP_RETAIL;            // decode from debug mode
					uint32 kt = m_Xbe->m_Header.dwKernelImageThunkAddr ^ XOR_KT_DEBUG;  // decode from debug mode

					m_Xbe->m_Header.dwEntryAddr = ep ^ XOR_EP_DEBUG;                    // encode to retail mode
					m_Xbe->m_Header.dwKernelImageThunkAddr = kt ^ XOR_KT_RETAIL;        // encode to retail mode
				}
				else
				{
					// we're in retail mode, so switch to debug
					uint32 ep = m_Xbe->m_Header.dwEntryAddr ^ XOR_EP_DEBUG;             // decode from retail mode
					uint32 kt = m_Xbe->m_Header.dwKernelImageThunkAddr ^ XOR_KT_RETAIL; // decode from retail mode

					m_Xbe->m_Header.dwEntryAddr = ep ^ XOR_EP_RETAIL;                   // encode to debug mode
					m_Xbe->m_Header.dwKernelImageThunkAddr = kt ^ XOR_KT_DEBUG;         // encode to debug mode
				}

				RefreshMenus();

				bool res = (m_Xbe->m_Header.dwEntryAddr ^ XOR_EP_RETAIL) > 0x01000000;

				if (res)
					printf("WndMain: %s was converted to debug mode.\n", m_Xbe->m_szAsciiTitle);
				else
					printf("WndMain: %s was converted to retail mode.\n", m_Xbe->m_szAsciiTitle);
			}
			break;

			case ID_EDIT_DUMPXBEINFOTO_FILE:
			{
				OPENFILENAME ofn = { 0 };

				char filename[MAX_PATH] = "Xbe.txt";

				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = m_hwnd;
				ofn.lpstrFilter = "Text Documents (*.txt)\0*.txt\0";
				ofn.lpstrFile = filename;
				ofn.nMaxFile = MAX_PATH;
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.lpstrDefExt = "txt";
				ofn.Flags = OFN_PATHMUSTEXIST;

				if (GetSaveFileName(&ofn) == TRUE)
				{
					// ask permission to overwrite if file exists
					if (_access(ofn.lpstrFile, 0) != -1)
					{
						if (MessageBox(m_hwnd, "Overwrite existing file?", "Cxbx-Reloaded", MB_ICONQUESTION | MB_YESNO) != IDYES)
							return TRUE;
					}

					// dump xbe information to file
					{
                        std::string Xbe_info = DumpInformation(m_Xbe);
                        if (m_Xbe->HasError()) {
                            MessageBox(m_hwnd, m_Xbe->GetError().c_str(), "Cxbx-Reloaded", MB_ICONSTOP | MB_OK);
                        }
                        else {
                            std::ofstream Xbe_dump_file(ofn.lpstrFile);
                            if(Xbe_dump_file.is_open()) {
                                Xbe_dump_file << Xbe_info;
                                Xbe_dump_file.close();
                                char buffer[255];
                                sprintf(buffer, "%s's .xbe info was successfully dumped.", m_Xbe->m_szAsciiTitle);
                                printf("WndMain: %s\n", buffer);
                                MessageBox(m_hwnd, buffer, "Cxbx-Reloaded", MB_ICONINFORMATION | MB_OK);
                            }
                            else {
                                MessageBox(m_hwnd, "Could not open Xbe text file.", "Cxbx-Reloaded", MB_ICONSTOP | MB_OK);
                            }
                        }
					}
				}
			}
			break;

            case ID_EDIT_DUMPXBEINFOTO_DEBUGCONSOLE:
            {
                std::string Xbe_info = DumpInformation(m_Xbe);
                if (m_Xbe->HasError()) {
                    MessageBox(m_hwnd, m_Xbe->GetError().c_str(), "Cxbx-Reloaded", MB_ICONSTOP | MB_OK);
                }
                else {
                    std::cout << Xbe_info;
                    char buffer[255];
                    sprintf(buffer, "%s's .xbe info was successfully dumped to console.", m_Xbe->m_szAsciiTitle);
                    printf("WndMain: %s\n", buffer);
                }
            }
            break;

			case ID_SETTINGS_CONFIG_CONTROLLER:
				ShowControllerConfig(hwnd);
				break;

			case ID_SETTINGS_CONFIG_VIDEO:
				ShowVideoConfig(hwnd);
				break;

            case ID_SETTINGS_CONFIG_AUDIO:
                ShowAudioConfig(hwnd);
                break;

			case ID_SETTINGS_CONFIG_EEPROM:
			{
				if (m_bIsStarted) {
					// We don't allow changing the contents of the eeprom while a game is running, mostly because we lack a "pause emulation"
					// function necessary to modify the contents safely (the game itself can modify the eeprom)
					MessageBox(hwnd, "Cannot modify eeprom file while a title is running", "Cxbx-Reloaded", MB_ICONEXCLAMATION | MB_OK);
					break;
				}
				ShowEepromConfig(hwnd);
			}
			break;

			case ID_CACHE_CLEARHLECACHE_ALL:
			{
				ClearHLECache();
				MessageBox(m_hwnd, "The entire HLE Cache has been cleared.", "Cxbx-Reloaded", MB_OK);
			}
			break;

			case ID_CACHE_CLEARHLECACHE_CURRENT:
			{
				std::string cacheDir = std::string(XTL::szFolder_CxbxReloadedData) + "\\HLECache\\";

				// Hash the loaded XBE's header, use it as a filename
				uint32_t uiHash = XXHash32::hash((void*)&m_Xbe->m_Header, sizeof(Xbe::Header), 0);
				std::stringstream sstream;
				std::string szTitleName(m_Xbe->m_szAsciiTitle);
				m_Xbe->PurgeBadChar(szTitleName);
				sstream << cacheDir << szTitleName << "-" << std::hex << uiHash << ".ini";
				std::string fullpath = sstream.str();

				if (DeleteFile(fullpath.c_str())) {
					MessageBox(m_hwnd, "This title's HLE Cache entry has been cleared.", "Cxbx-Reloaded", MB_OK);
				}
			}
			break;

			case ID_SETTINGS_INITIALIZE:
			{
				int ret = MessageBox(m_hwnd, "Warning: This will reset all Cxbx-Reloaded settings to their default values."
					"\nAre you sure you want to proceed?", "Cxbx-Reloaded", MB_ICONEXCLAMATION | MB_YESNO);

				if (ret == IDYES) {
					InitializeSettings();
					MessageBox(m_hwnd, "Cxbx-Reloaded has been initialized and will now close.", "Cxbx-Reloaded", MB_ICONINFORMATION | MB_OK);
					SendMessage(hwnd, WM_CLOSE, 0, 0);
				}
			}
			break;

			case ID_EMULATION_DEBUGOUTPUTKERNEL_CONSOLE:
			{
				if (m_KrnlDebug == DM_NONE || m_KrnlDebug == DM_FILE)
					m_KrnlDebug = DM_CONSOLE;
				else
					m_KrnlDebug = DM_NONE;

				MessageBox(m_hwnd, "This will not take effect until the next time emulation is started.\n", "Cxbx-Reloaded", MB_OK);

				RefreshMenus();

				UpdateDebugConsoles();
			}
			break;

			case ID_EMULATION_DEBUGOUTPUTKERNEL_FILE:
			{
				if (m_KrnlDebug == DM_FILE)
				{
					m_KrnlDebug = DM_NONE;

					RefreshMenus();

					UpdateDebugConsoles();
				}
				else
				{
					OPENFILENAME ofn = { 0 };

					char filename[MAX_PATH] = "KrnlDebug.txt";

					ofn.lStructSize = sizeof(OPENFILENAME);
					ofn.hwndOwner = m_hwnd;
					ofn.lpstrFilter = "Text Documents (*.txt)\0*.txt\0";
					ofn.lpstrFile = filename;
					ofn.nMaxFile = MAX_PATH;
					ofn.nFilterIndex = 1;
					ofn.lpstrFileTitle = NULL;
					ofn.nMaxFileTitle = 0;
					ofn.lpstrInitialDir = NULL;
					ofn.lpstrDefExt = "txt";
					ofn.Flags = OFN_PATHMUSTEXIST;

					if (GetSaveFileName(&ofn) != FALSE)
					{
						MessageBox(m_hwnd, "This will not take effect until emulation is (re)started.\n", "Cxbx-Reloaded", MB_OK);

						strncpy(m_KrnlDebugFilename, ofn.lpstrFile, MAX_PATH - 1);

						m_KrnlDebug = DM_FILE;

						RefreshMenus();

						UpdateDebugConsoles();
					}
				}
			}
			break;

			case ID_EMULATION_DEBUGOUTPUTGUI_CONSOLE:
			{
				if (m_CxbxDebug == DM_NONE || m_CxbxDebug == DM_FILE)
					m_CxbxDebug = DM_CONSOLE;
				else
					m_CxbxDebug = DM_NONE;

				RefreshMenus();

				UpdateDebugConsoles();
			}
			break;

			case ID_EMULATION_DEBUGOUTPUTGUI_FILE:
			{
				if (m_CxbxDebug == DM_FILE)
				{
					m_CxbxDebug = DM_NONE;

					RefreshMenus();

					UpdateDebugConsoles();
				}
				else
				{
					OPENFILENAME ofn = { 0 };

					char filename[MAX_PATH] = "CxbxDebug.txt";

					ofn.lStructSize = sizeof(OPENFILENAME);
					ofn.hwndOwner = m_hwnd;
					ofn.lpstrFilter = "Text Documents (*.txt)\0*.txt\0";
					ofn.lpstrFile = filename;
					ofn.nMaxFile = MAX_PATH;
					ofn.nFilterIndex = 1;
					ofn.lpstrFileTitle = NULL;
					ofn.nMaxFileTitle = 0;
					ofn.lpstrInitialDir = NULL;
					ofn.lpstrDefExt = "txt";
					ofn.Flags = OFN_PATHMUSTEXIST;

					if (GetSaveFileName(&ofn) != FALSE)
					{
						strncpy(m_CxbxDebugFilename, ofn.lpstrFile, MAX_PATH - 1);

						m_CxbxDebug = DM_FILE;

						RefreshMenus();

						UpdateDebugConsoles();
					}

				}
			}
			break;

			case ID_EMULATION_LLE_JIT:
			{
				m_FlagsLLE = m_FlagsLLE ^ LLE_JIT;
				ClearHLECache();
				RefreshMenus();
			}
			break;

			case ID_EMULATION_LLE_APU:
			{
				m_FlagsLLE = m_FlagsLLE ^ LLE_APU;
				ClearHLECache();
				RefreshMenus();
			}
			break;

			case ID_EMULATION_LLE_GPU:
			{
				m_FlagsLLE = m_FlagsLLE ^ LLE_GPU;
				ClearHLECache();
				RefreshMenus();
			}
			break;

			case ID_SETTINGS_XINPUT:
				m_XInputEnabled = !m_XInputEnabled;
				RefreshMenus();
				break;

            case ID_EMULATION_START:
                if (m_Xbe != nullptr)
                {
                    StartEmulation(hwnd);
                }
                break;

            case ID_EMULATION_STARTDEBUGGER:
                if (m_Xbe != nullptr)
                {
                    StartEmulation(hwnd, debuggerOn);
                }
                break;

            case ID_EMULATION_STOP:
                StopEmulation();
                break;

			case ID_HACKS_DISABLEPIXELSHADERS:
				m_DisablePixelShaders = !m_DisablePixelShaders;
				RefreshMenus();
				break;

			case ID_HACKS_UNCAPFRAMERATE:
				m_UncapFramerate = !m_UncapFramerate;
				RefreshMenus();
				break;

			case ID_HACKS_RUNXBOXTHREADSONALLCORES:
				m_UseAllCores = !m_UseAllCores;
				RefreshMenus();
				break;

			case ID_HACKS_SKIPRDTSCPATCHING:
				m_SkipRdtscPatching = !m_SkipRdtscPatching;
				RefreshMenus();
				break;

			case ID_HACKS_SCALEVIEWPORT:
				m_ScaleViewport = !m_ScaleViewport;
				RefreshMenus();
				break;

            case ID_HELP_ABOUT:
            {
				ShowAboutDialog(hwnd);
            }
            break;

            case ID_HELP_HOMEPAGE:
                ShellExecute(NULL, "open", "https://github.com/Cxbx-Reloaded/Cxbx-Reloaded", NULL, NULL, SW_SHOWNORMAL);
                break;
            }

            break;
        }

        case WM_CLOSE:
        {
            if(m_Xbe != nullptr)
                CloseXbe();

            if(m_Xbe == nullptr)
                DestroyWindow(hwnd);
        }
        break;

        case WM_DESTROY:
        {
            FreeConsole();

			HDC hDC = GetDC(hwnd);

			SelectObject(m_LogoDC, m_OrigLogo);

			SelectObject(m_SplashDC, m_OrigBmp);

			SelectObject(m_GameLogoDC, m_OrigGameLogo);

			SelectObject(m_LedDC, m_OriLed);

			DeleteObject(m_LogoDC);

			DeleteObject(m_SplashDC);

			DeleteObject(m_GameLogoDC);

			DeleteObject(m_LedDC);

			DeleteObject(m_LogoBmp);

			DeleteObject(m_SplashBmp);

            DeleteObject(m_BackgroundColor);

			DeleteObject(m_GameLogoBMP);

			DeleteObject(m_LedBmp);

			DeleteObject(m_Brushes[XBOX_LED_COLOUR_OFF]);

			DeleteObject(m_Brushes[XBOX_LED_COLOUR_GREEN]);

			DeleteObject(m_Brushes[XBOX_LED_COLOUR_RED]);

			DeleteObject(m_Brushes[XBOX_LED_COLOUR_ORANGE]);

			DeleteObject(m_Pens[XBOX_LED_COLOUR_OFF]);

			DeleteObject(m_Pens[XBOX_LED_COLOUR_GREEN]);

			DeleteObject(m_Pens[XBOX_LED_COLOUR_RED]);

			DeleteObject(m_Pens[XBOX_LED_COLOUR_ORANGE]);

			ReleaseDC(hwnd, hDC);

            delete m_Xbe;

            m_Xbe = nullptr;

            PostQuitMessage(0);
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

// suggest a file name
void WndMain::SuggestFilename(const char *x_orig_filename, char *x_filename, char x_extension[4])
{
    if(strrchr(x_orig_filename, '\\') != NULL)
    {
        strcpy(x_filename, x_orig_filename);
		char *loc = strrchr(x_filename, '.');
		if (loc != NULL)
			strncpy(loc, &x_extension[0], 4);
    }
}

// occurs when an xbe is loaded..
void WndMain::XbeLoaded()
{
    LoadLogo();

	LoadGameLogo();

	UpdateCaption();
    RefreshMenus();

    InvalidateRgn(m_hwnd, NULL, TRUE);

    printf("WndMain: %s loaded.\n", m_Xbe->m_szAsciiTitle);
}

// load logo bitmap
void WndMain::LoadLogo()
{
    uint08 i_gray[100*17];

    m_Xbe->ExportLogoBitmap(i_gray);

    if(m_Xbe->HasError())
    {
        MessageBox(m_hwnd, m_Xbe->GetError().c_str(), "Cxbx-Reloaded", MB_ICONEXCLAMATION | MB_OK);

        if (m_Xbe->HasFatalError())
        {
            CloseXbe();
        }

        return;
    }

    uint32 v=0;
    for(uint32 y=0;y<17;y++)
    {
        for(uint32 x=0;x<100;x++)
        {
            SetPixel(m_LogoDC, x, y, RGB(i_gray[v], i_gray[v], i_gray[v]));
            v++;
        }
    }

    RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE);
}

// TODO : Move these types to a more appropriate place
// Source : https://msdn.microsoft.com/en-us/library/windows/desktop/bb943984(v=vs.85).aspx
struct DDS_PIXELFORMAT {
	DWORD dwSize;
	DWORD dwFlags;
	DWORD dwFourCC;
	DWORD dwRGBBitCount;
	DWORD dwRBitMask;
	DWORD dwGBitMask;
	DWORD dwBBitMask;
	DWORD dwABitMask;
};

// Source : https://msdn.microsoft.com/en-us/library/windows/desktop/bb943982(v=vs.85).aspx
typedef struct {
	DWORD           dwSize;
	DWORD           dwFlags;
	DWORD           dwHeight;
	DWORD           dwWidth;
	DWORD           dwPitchOrLinearSize;
	DWORD           dwDepth;
	DWORD           dwMipMapCount;
	DWORD           dwReserved1[11];
	DDS_PIXELFORMAT ddspf;
	DWORD           dwCaps;
	DWORD           dwCaps2;
	DWORD           dwCaps3;
	DWORD           dwCaps4;
	DWORD           dwReserved2;
} DDS_HEADER;

// load game logo bitmap
void WndMain::LoadGameLogo()
{
	// Export Game Logo bitmap (XTIMAG or XSIMAG)
	uint8 *pSection = (uint8 *)m_Xbe->FindSection("$$XTIMAGE"); // Check for XTIMAGE
	if (!pSection) {
		pSection = (uint8 *)m_Xbe->FindSection("$$XSIMAGE"); // if XTIMAGE isn't present, check for XSIMAGE (smaller)
		if (!pSection) {
			return;
		}
	}

	gameLogoWidth = 0;
	gameLogoHeight = 0;	

	uint8 *ImageData = NULL;
	XTL::X_D3DPixelContainer XboxPixelContainer = {};
	XTL::X_D3DPixelContainer *pXboxPixelContainer = &XboxPixelContainer;

	switch (*(DWORD*)pSection) {
	case MAKEFOURCC('D', 'D', 'S', ' '): {
		DDS_HEADER *pDDSHeader = (DDS_HEADER *)(pSection + sizeof(DWORD));
		XTL::D3DFORMAT Format = XTL::D3DFMT_UNKNOWN;
		if (pDDSHeader->ddspf.dwFlags & DDPF_FOURCC) {
			switch (pDDSHeader->ddspf.dwFourCC) {
			case MAKEFOURCC('D', 'X', 'T', '1'): Format = XTL::D3DFMT_DXT1; break;
			case MAKEFOURCC('D', 'X', 'T', '3'): Format = XTL::D3DFMT_DXT3; break;
			case MAKEFOURCC('D', 'X', 'T', '5'): Format = XTL::D3DFMT_DXT5; break;
			}
		}
		else {
			// TODO : Determine D3D format based on pDDSHeader->ddspf.dwABitMask, .dwRBitMask, .dwGBitMask and .dwBBitMask
		}

		if (Format == XTL::D3DFMT_UNKNOWN)
			return;

		ImageData = (uint8 *)(pSection + sizeof(DWORD) + pDDSHeader->dwSize);
		//gameLogoHeight = pDDSHeader->dwHeight;
		//gameLogoWidth = pDDSHeader->dwWidth;
		
		// TODO : Use PixelCopy code here to decode. For now, fake it :
		XTL::CxbxSetPixelContainerHeader(&XboxPixelContainer,
			0, // Common - could be X_D3DCOMMON_TYPE_TEXTURE
			(XTL::UINT)pDDSHeader->dwWidth,
			(XTL::UINT)pDDSHeader->dwHeight,
			1,
			XTL::EmuPC2XB_D3DFormat(Format),
			2,
			(XTL::UINT)pDDSHeader->dwPitchOrLinearSize);
		break;
	}
	case MAKEFOURCC('X', 'P', 'R', '0'):
	case MAKEFOURCC('X', 'P', 'R', '1'): {
		struct Xbe::XprHeader *pXprHeader = (struct Xbe::XprHeader*)pSection;

		uint SizeOfResourceHeaders = pXprHeader->dwXprHeaderSize - sizeof(Xbe::XprHeader);
		uint SizeOfResourceData = pXprHeader->dwXprTotalSize - pXprHeader->dwXprHeaderSize;

		uint8 *ResourceHeaders = pSection + sizeof(Xbe::XprHeader);
		uint8 *ResourceData = ResourceHeaders + SizeOfResourceHeaders;

		pXboxPixelContainer = (XTL::X_D3DPixelContainer*)ResourceHeaders;
		ImageData = ResourceData;

		break;
	}
	default: {
		return;
	}
	}

	void *bitmapData = XTL::ConvertD3DTextureToARGB(pXboxPixelContainer, ImageData, &gameLogoWidth, &gameLogoHeight);
	if (!bitmapData)
		return;

	HDC hDC = GetDC(m_hwnd);
	m_GameLogoBMP = CreateCompatibleBitmap(hDC, gameLogoWidth, gameLogoHeight);

	// create bitmap
	{
		BITMAPINFO BmpInfo;

		BmpInfo.bmiHeader.biSize = sizeof(BITMAPINFO) - sizeof(RGBQUAD);
		BmpInfo.bmiHeader.biWidth = gameLogoWidth;
		BmpInfo.bmiHeader.biHeight = 0 - (long)gameLogoHeight; //  If biHeight is negative, the bitmap is a top-down DIB and its origin is the upper-left corner.
		BmpInfo.bmiHeader.biPlanes = 1;
		BmpInfo.bmiHeader.biBitCount = 32;
		BmpInfo.bmiHeader.biCompression = BI_RGB;
		BmpInfo.bmiHeader.biSizeImage = 0;
		BmpInfo.bmiHeader.biXPelsPerMeter = 0;
		BmpInfo.bmiHeader.biYPelsPerMeter = 0;
		BmpInfo.bmiHeader.biClrUsed = 0;
		BmpInfo.bmiHeader.biClrImportant = 0;

		SetDIBits(hDC, m_GameLogoBMP, 0, gameLogoHeight, bitmapData, &BmpInfo, DIB_RGB_COLORS);
	}

	m_GameLogoDC = CreateCompatibleDC(hDC);
	m_OrigGameLogo = (HBITMAP)SelectObject(m_GameLogoDC, m_GameLogoBMP);

	RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE);

	if (hDC != NULL)
		ReleaseDC(m_hwnd, hDC);

	free(bitmapData);
}


// refresh menu items
void WndMain::RefreshMenus()
{
	bool XbeLoaded = (m_Xbe != nullptr);
	bool Running = (m_hwndChild != NULL); // TODO : Use m_bIsStarted?
	UINT MF_WhenXbeLoaded = XbeLoaded ? MF_ENABLED : MF_GRAYED;
	UINT MF_WhenXbeLoadedNotRunning = (XbeLoaded && !Running) ? MF_ENABLED : MF_GRAYED;
	UINT MF_WhenXbeLoadedAndRunning = (XbeLoaded && Running) ? MF_ENABLED : MF_GRAYED;

    // disable/enable appropriate menus
    {
        HMENU menu = GetMenu(m_hwnd);

        // file menu
        {
            HMENU file_menu = GetSubMenu(menu, 0);

            // enable/disable close .xbe file
            EnableMenuItem(file_menu, ID_FILE_CLOSE_XBE, MF_BYCOMMAND | MF_WhenXbeLoaded);

            // enable/disable save .xbe file
            EnableMenuItem(file_menu, ID_FILE_SAVEXBEFILE, MF_BYCOMMAND | MF_WhenXbeLoaded);

            // enable/disable save .xbe file as
            EnableMenuItem(file_menu, ID_FILE_SAVEXBEFILEAS, MF_BYCOMMAND | MF_WhenXbeLoaded);

            // recent xbe files menu
            {
                HMENU rxbe_menu = GetSubMenu(file_menu, 7);

                int max = m_dwRecentXbe;
                for(int v=0;v<max;v++)
                    EnableMenuItem(rxbe_menu, ID_FILE_RXBE_0 + v, MF_BYCOMMAND | MF_ENABLED);
            }
        }

        // edit menu
        {
            HMENU edit_menu = GetSubMenu(menu, 1);
            HMENU logo_menu = GetSubMenu(edit_menu, 0);
            HMENU pach_menu = GetSubMenu(edit_menu, 1);

            // enable export .xbe info
            EnableMenuItem(edit_menu, ID_EDIT_DUMPXBEINFOTO_FILE, MF_BYCOMMAND | MF_WhenXbeLoaded);
            EnableMenuItem(edit_menu, ID_EDIT_DUMPXBEINFOTO_DEBUGCONSOLE, MF_BYCOMMAND | MF_WhenXbeLoaded);

            // enable logo bitmap menu
            EnableMenuItem(edit_menu, 0, MF_BYPOSITION | MF_WhenXbeLoaded);

            // enable patch menu
            EnableMenuItem(edit_menu, 1, MF_BYPOSITION | MF_WhenXbeLoaded);

            // patch menu
            {
                // check "allow >64 MB" if appropriate
                if(m_Xbe != nullptr)
                {
                    UINT chk_flag = (m_Xbe->m_Header.dwInitFlags.bLimit64MB) ? MF_UNCHECKED : MF_CHECKED;

                    CheckMenuItem(pach_menu, ID_EDIT_PATCH_ALLOW64MB, chk_flag);
                }

                // check "debug mode" if appropriate
                if(m_Xbe != nullptr)
                {
                    UINT chk_flag = ((m_Xbe->m_Header.dwEntryAddr ^ XOR_EP_RETAIL) > 0x01000000) ? MF_CHECKED : MF_UNCHECKED;

                    CheckMenuItem(pach_menu, ID_EDIT_PATCH_DEBUGMODE, chk_flag);
                }
            }
        }

        // view menu
		{
			HMENU view_menu = GetSubMenu(menu, 2);
			HMENU emul_debg = GetSubMenu(view_menu, 0);
			HMENU emul_krnl = GetSubMenu(view_menu, 1);

			if (m_KrnlDebug == DM_CONSOLE)
			{
				CheckMenuItem(emul_krnl, ID_EMULATION_DEBUGOUTPUTKERNEL_CONSOLE, MF_CHECKED);
				CheckMenuItem(emul_krnl, ID_EMULATION_DEBUGOUTPUTKERNEL_FILE, MF_UNCHECKED);
			}
			else if (m_KrnlDebug == DM_FILE)
			{
				CheckMenuItem(emul_krnl, ID_EMULATION_DEBUGOUTPUTKERNEL_CONSOLE, MF_UNCHECKED);
				CheckMenuItem(emul_krnl, ID_EMULATION_DEBUGOUTPUTKERNEL_FILE, MF_CHECKED);
			}
			else
			{
				CheckMenuItem(emul_krnl, ID_EMULATION_DEBUGOUTPUTKERNEL_CONSOLE, MF_UNCHECKED);
				CheckMenuItem(emul_krnl, ID_EMULATION_DEBUGOUTPUTKERNEL_FILE, MF_UNCHECKED);
			}

			if (m_CxbxDebug == DM_CONSOLE)
			{
				CheckMenuItem(emul_debg, ID_EMULATION_DEBUGOUTPUTGUI_CONSOLE, MF_CHECKED);
				CheckMenuItem(emul_debg, ID_EMULATION_DEBUGOUTPUTGUI_FILE, MF_UNCHECKED);
			}
			else if (m_CxbxDebug == DM_FILE)
			{
				CheckMenuItem(emul_debg, ID_EMULATION_DEBUGOUTPUTGUI_CONSOLE, MF_UNCHECKED);
				CheckMenuItem(emul_debg, ID_EMULATION_DEBUGOUTPUTGUI_FILE, MF_CHECKED);
			}
			else
			{
				CheckMenuItem(emul_debg, ID_EMULATION_DEBUGOUTPUTGUI_CONSOLE, MF_UNCHECKED);
				CheckMenuItem(emul_debg, ID_EMULATION_DEBUGOUTPUTGUI_FILE, MF_UNCHECKED);
			}
		}

		// settings menu
		{
			HMENU settings_menu = GetSubMenu(menu, 3);

			// enable/disable clear current hle cache
			EnableMenuItem(settings_menu, ID_CACHE_CLEARHLECACHE_CURRENT, MF_BYCOMMAND | MF_WhenXbeLoadedNotRunning);

			UINT chk_flag = (m_FlagsLLE & LLE_JIT) ? MF_CHECKED : MF_UNCHECKED;
			CheckMenuItem(settings_menu, ID_EMULATION_LLE_JIT, chk_flag);

			chk_flag = (m_FlagsLLE & LLE_APU) ? MF_CHECKED : MF_UNCHECKED;
			CheckMenuItem(settings_menu, ID_EMULATION_LLE_APU, chk_flag);

			chk_flag = (m_FlagsLLE & LLE_GPU) ? MF_CHECKED : MF_UNCHECKED;
			CheckMenuItem(settings_menu, ID_EMULATION_LLE_GPU, chk_flag);

			chk_flag = (m_XInputEnabled) ? MF_CHECKED : MF_UNCHECKED;
			CheckMenuItem(settings_menu, ID_SETTINGS_XINPUT, chk_flag);

			chk_flag = (m_DisablePixelShaders) ? MF_CHECKED : MF_UNCHECKED;
			CheckMenuItem(settings_menu, ID_HACKS_DISABLEPIXELSHADERS, chk_flag);

			chk_flag = (m_UncapFramerate) ? MF_CHECKED : MF_UNCHECKED;
			CheckMenuItem(settings_menu, ID_HACKS_UNCAPFRAMERATE, chk_flag);

			chk_flag = (m_UseAllCores) ? MF_CHECKED : MF_UNCHECKED;
			CheckMenuItem(settings_menu, ID_HACKS_RUNXBOXTHREADSONALLCORES, chk_flag);

			chk_flag = (m_SkipRdtscPatching) ? MF_CHECKED : MF_UNCHECKED;
			CheckMenuItem(settings_menu, ID_HACKS_SKIPRDTSCPATCHING, chk_flag);
      
			chk_flag = (m_ScaleViewport) ? MF_CHECKED : MF_UNCHECKED;
			CheckMenuItem(settings_menu, ID_HACKS_SCALEVIEWPORT, chk_flag);
		}

        // emulation menu
        {
            HMENU emul_menu = GetSubMenu(menu, 4);

            // enable emulation start
            EnableMenuItem(emul_menu, ID_EMULATION_START, MF_BYCOMMAND | MF_WhenXbeLoadedNotRunning);

            // enable emulation with debugging
            EnableMenuItem(emul_menu, ID_EMULATION_STARTDEBUGGER, MF_BYCOMMAND | MF_WhenXbeLoadedNotRunning);

            // enable emulation stop
            EnableMenuItem(emul_menu, ID_EMULATION_STOP, MF_BYCOMMAND | MF_WhenXbeLoadedAndRunning);
        }
    }
}

// update debug consoles
void WndMain::UpdateDebugConsoles()
{
    if(m_CxbxDebug == DM_CONSOLE)
    {
        if(AllocConsole())
        {
            freopen("CONOUT$", "wt", stdout);

            SetConsoleTitle("Cxbx-Reloaded : Debug Console");

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);

            printf("%s", "WndMain: Debug console allocated.\n");

            SetForegroundWindow(m_hwnd);
        }
    }
    else if(m_CxbxDebug == DM_FILE)
    {
        FreeConsole();

        freopen(m_CxbxDebugFilename, "wt", stdout);

        printf("%s", "WndMain: Debug console allocated.\n");
    }
    else
    {
        FreeConsole();

        char buffer[16];

        if(GetConsoleTitle(buffer, 16) != NULL)
            freopen("nul", "w", stdout);
    }
}

// update recent files menu
void WndMain::UpdateRecentFiles()
{
    HMENU FileMenu = GetSubMenu(GetMenu(m_hwnd), 0);
    HMENU RXbeMenu = GetSubMenu(FileMenu, 7);

    // clear existing menu items
    {
        int v, max;

        max = GetMenuItemCount(RXbeMenu);
        for(v=0;v<max;v++)
            RemoveMenu(RXbeMenu, 0, MF_BYPOSITION);
    }

    // insert recent xbe files
    {
        char szBuffer[270];

        int max = m_dwRecentXbe;

        // if there are no recent files, throw in a disabled "(none)"
        if(max == 0)
        {
            AppendMenu(RXbeMenu, MF_STRING, ID_FILE_RXBE_0, "(none)");
            EnableMenuItem(RXbeMenu, ID_FILE_RXBE_0, MF_BYCOMMAND | MF_GRAYED);
        }

        // NOTE: Resource defines ID_FILE_RXBE_0 through ID_FILE_RXBE_9 must be in order
        for(int v=0;v<max;v++)
        {
            sprintf(szBuffer, "&%d : %s", v, m_szRecentXbe[v]);
            AppendMenu(RXbeMenu, MF_STRING, ID_FILE_RXBE_0 + v, szBuffer);
        }
    }
}

void WndMain::UpdateCaption()
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

		if (m_bIsStarted) {
			if (g_EmuShared != NULL) {
				float currentFPSVal = 0;
				float currentMSpFVal = 0;
				g_EmuShared->GetCurrentFPS(&currentFPSVal);
				g_EmuShared->GetCurrentMSpF(&currentMSpFVal);
				sprintf(sMenu, "FPS: %.2f  MS / F : %.2f", currentFPSVal, currentMSpFVal);
			}
		}
		else {
			// Hide FPS if we're not currently emulating
			sprintf(sMenu, " ");
		}

		SetMenuItemInfo(hMenu, ID_FPS, FALSE, &mii);
	}

	SetWindowText(m_hwnd, AsciiTitle);
}

// open an xbe file
void WndMain::OpenXbe(const char *x_filename)
{
    if (m_Xbe != nullptr) {
        CloseXbe();
        if (m_Xbe != nullptr)
            return;
    }

    strcpy(m_XbeFilename, x_filename);

    m_Xbe = new Xbe(m_XbeFilename, true);

    if(m_Xbe->HasError())
    {
		// Save the error message as a separate string. This fixes a corruption in the message "Disclaimer: Cxbx-Reloaded has no
		// affiliation with Microsoft" that would occur if loading an xbe and then launching the dashboard with the "Open dashboard"
		// option but it's not installed

		std::string ErrorMessage = m_Xbe->GetError();

		delete m_Xbe; m_Xbe = nullptr;

		RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE);

        MessageBox(m_hwnd, ErrorMessage.c_str(), "Cxbx-Reloaded", MB_ICONSTOP | MB_OK);

        UpdateCaption();

        return;
    }

    // save this xbe to the list of recent xbe files
    if(m_XbeFilename[0] != '\0')
    {
        bool found = false;

        // if this filename already exists, temporarily remove it
        for(int c=0, r=0;c<m_dwRecentXbe;c++, r++)
        {
            if(strcmp(m_szRecentXbe[c], m_XbeFilename) == 0)
            {
                found = true;
                r++;
            }

            if(r != c)
            {
                if(m_szRecentXbe[r] == 0 || r > m_dwRecentXbe - 1)
                {
                    free(m_szRecentXbe[c]);
                    m_szRecentXbe[c] = 0;
                }
                else
                {
                    strncpy(m_szRecentXbe[c], m_szRecentXbe[r], MAX_PATH-1);
                }
            }
        }

        if(found)
            m_dwRecentXbe--;

        // move all items down one, removing the last one if necessary
        for(int v=RECENT_XBE_SIZE-1;v>0;v--)
        {
            if(m_szRecentXbe[v-1] == 0)
            {
                free(m_szRecentXbe[v]);
                m_szRecentXbe[v] = 0;
            }
            else
            {
                if(m_szRecentXbe[v] == 0)
                    m_szRecentXbe[v] = (char*)calloc(1, MAX_PATH);
                strncpy(m_szRecentXbe[v], m_szRecentXbe[v-1], MAX_PATH-1);
            }
        }

        // add new item as first index
        {
            if(m_szRecentXbe[0] == 0)
                m_szRecentXbe[0] = (char*)calloc(1, MAX_PATH);

            strcpy(m_szRecentXbe[0], m_XbeFilename);
        }

        if(m_dwRecentXbe < RECENT_XBE_SIZE)
            m_dwRecentXbe++;
    }

    UpdateRecentFiles();

    XbeLoaded();
}

// close xbe file
void WndMain::CloseXbe()
{
    if (m_bIsStarted)
        StopEmulation();

    if(m_bXbeChanged)
    {
        int ret = MessageBox(m_hwnd, "Changes have been made, do you wish to save?", "Cxbx-Reloaded", MB_ICONQUESTION | MB_YESNOCANCEL);

        if(ret == IDYES)
            SaveXbeAs();
        else if(ret == IDCANCEL)
            return;
    }

    printf("WndMain: %s unloaded.\n", m_Xbe->m_szAsciiTitle);

    m_bXbeChanged = false;

    delete m_Xbe; m_Xbe = nullptr;

    UpdateCaption();
    RefreshMenus();

    // clear logo bitmap
    {
        uint32 v=0;
        for(uint32 y=0;y<17;y++)
        {
            for(uint32 x=0;x<100;x++)
            {
                SetPixel(m_LogoDC, x, y, RGB(0, 0, 0));
                v++;
            }
        }
    }

	// clear game logo bitmap
	SelectObject(m_GameLogoDC, m_OrigGameLogo);
	DeleteObject(m_GameLogoDC);
	DeleteObject(m_GameLogoBMP);

    RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE);
}

void WndMain::OpenMRU(int mru)
{
	HMENU menu = GetMenu(m_hwnd);
	HMENU file_menu = GetSubMenu(menu, 0);
	HMENU rxbe_menu = GetSubMenu(file_menu, 7);

	char szBuffer[270];

	GetMenuString(rxbe_menu, ID_FILE_RXBE_0 + mru, szBuffer, 269, MF_BYCOMMAND);

	char *szFilename = (char*)((uint32)szBuffer + 5); // +5 skips over "&%d : " prefix (see UpdateRecentFiles)

	OpenXbe(szFilename);
}

// Open the dashboard xbe if found
void WndMain::OpenDashboard()
{
	std::string DashboardPath = std::string(XTL::szFolder_CxbxReloadedData) + std::string("\\EmuDisk\\Partition2\\xboxdash.xbe");
	OpenXbe(DashboardPath.c_str());
}

// save xbe file
void WndMain::SaveXbe(const char *x_filename)
{
    // ask permission to overwrite if the file already exists
    if(_access(x_filename, 0) != -1)
    {
        if(MessageBox(m_hwnd, "Overwrite existing file?", "Cxbx-Reloaded", MB_ICONQUESTION | MB_YESNO) != IDYES)
            return;
    }

    // export xbe file
    {
        m_Xbe->Export(x_filename);

        if(m_Xbe->HasError())
            MessageBox(m_hwnd, m_Xbe->GetError().c_str(), "Cxbx-Reloaded", MB_ICONSTOP | MB_OK);
        else
        {
            char buffer[255];

            sprintf(buffer, "%s was successfully saved.", m_Xbe->m_szAsciiTitle);

            printf("WndMain: %s was successfully saved.\n", m_Xbe->m_szAsciiTitle);

            MessageBox(m_hwnd, buffer, "Cxbx-Reloaded", MB_ICONINFORMATION | MB_OK);

            m_bXbeChanged = false;
        }
    }
}

// save xbe as
void WndMain::SaveXbeAs()
{
    OPENFILENAME ofn = {0};

    char filename[MAX_PATH] = "default.xbe";

    SuggestFilename(m_XbeFilename, filename, ".xbe");

    ofn.lStructSize     = sizeof(OPENFILENAME);
    ofn.hwndOwner       = m_hwnd;
    ofn.lpstrFilter     = "Xbox Executables (*.xbe)\0*.xbe\0";
    ofn.lpstrFile       = filename;
    ofn.nMaxFile        = MAX_PATH;
    ofn.nFilterIndex    = 1;
    ofn.lpstrFileTitle  = NULL;
    ofn.nMaxFileTitle   = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrDefExt     = "xbe";
    ofn.Flags           = OFN_PATHMUSTEXIST;

    if(GetSaveFileName(&ofn) == TRUE)
        SaveXbe(ofn.lpstrFile);
}

// start emulation
void WndMain::StartEmulation(HWND hwndParent, DebuggerState LocalDebuggerState /*= debuggerOff*/)
{
    char szBuffer[MAX_PATH];

    // register xbe path with emulator process
    g_EmuShared->SetXbePath(m_Xbe->m_szPath);

	// register LLE flags with emulator process
	g_EmuShared->SetFlagsLLE(&m_FlagsLLE);

	// register XInput flags with emulator process
	g_EmuShared->SetXInputEnabled(&m_XInputEnabled);

	// register Hacks with emulator process
	g_EmuShared->SetDisablePixelShaders(&m_DisablePixelShaders);
	g_EmuShared->SetUncapFramerate(&m_UncapFramerate);
	g_EmuShared->SetUseAllCores(&m_UseAllCores);
	g_EmuShared->SetSkipRdtscPatching(&m_SkipRdtscPatching);
	g_EmuShared->SetScaleViewport(&m_ScaleViewport);

	if (m_ScaleViewport) {
		// Set the window size to emulation dimensions
		// Note : Doing this here assures the emulation process will use
		// the configured dimensions (because if done inside the emulation
		// process, that doesn't resize correctly sometimes)
		// TODO : Instead of doing the resize before launch, move it
		// towards CreateDevice emulation, using Xbox-requested dimensions
		// (and fix the issue of incorrect resizing)
		ResizeWindow(m_hwnd, /*bForGUI*/false);
	}

	// shell exe
    {
        GetModuleFileName(NULL, szBuffer, MAX_PATH);

		char *spot = strrchr(szBuffer, '\\');
		if (spot != NULL)
			*spot = '\0';

		char szExeFileName[MAX_PATH];
		GetModuleFileName(GetModuleHandle(NULL), szExeFileName, MAX_PATH);

		char szArgsBuffer[4096];
		snprintf(szArgsBuffer, 4096, "/load \"%s\" %d %d \"%s\"", m_XbeFilename, (int)hwndParent, (int)m_KrnlDebug, m_KrnlDebugFilename);

		bool AttachLocalDebugger = (LocalDebuggerState == debuggerOn);
		g_EmuShared->SetDebuggingFlag(&AttachLocalDebugger);

        if (AttachLocalDebugger)
        {
            // TODO: Set a configuration variable for this. For now it will be within the same folder as Cxbx.exe
            const char* szDebugger = "CxbxDebugger.exe";

            char szDbgArgsBuffer[4096];
            snprintf(szDbgArgsBuffer, 4096, "%s %s", szExeFileName, szArgsBuffer);

            if ((int)ShellExecute(NULL, "open", szDebugger, szDbgArgsBuffer, szBuffer, SW_SHOWDEFAULT) <= 32)
            {
                MessageBox(m_hwnd, "Failed to start emulation with the debugger.\n\nYou will need to build CxbxDebugger manually.", "Cxbx-Reloaded", MB_ICONSTOP | MB_OK);

                printf("WndMain: %s debugger shell failed.\n", m_Xbe->m_szAsciiTitle);
            }
            else
            {
                m_bIsStarted = true;
                printf("WndMain: %s emulation started with debugger.\n", m_Xbe->m_szAsciiTitle);
            }
        }
        else
        {
            if ((int)ShellExecute(NULL, "open", szExeFileName, szArgsBuffer, szBuffer, SW_SHOWDEFAULT) <= 32)
            {
                MessageBox(m_hwnd, "Emulation failed.\n\n If this message repeats, the Xbe is not supported.", "Cxbx-Reloaded", MB_ICONSTOP | MB_OK);

                printf("WndMain: %s shell failed.\n", m_Xbe->m_szAsciiTitle);
            }
            else
            {
                m_bIsStarted = true;
                printf("WndMain: %s emulation started.\n", m_Xbe->m_szAsciiTitle);
            }
        }
    }
}

// stop emulation
void WndMain::StopEmulation()
{
    m_bIsStarted = false;
    if (m_hwndChild != NULL) {
		if (IsWindow(m_hwndChild)) {
			SendMessage(m_hwndChild, WM_CLOSE, 0, 0);
		}

		m_hwndChild = NULL;
    }

	UpdateCaption();
    RefreshMenus();
	// Set the window size back to it's GUI dimensions
	ResizeWindow(m_hwnd, /*bForGUI=*/true);
}


// wrapper function to call CrashMonitor
DWORD WINAPI WndMain::CrashMonitorWrapper(LPVOID lpVoid)
{
	CxbxSetThreadName("Cxbx Crash Monitor");

	static_cast<WndMain*>(lpVoid)->CrashMonitor();
	return 0;
}

// monitor for crashes
void WndMain::CrashMonitor()
{
	bool bQuickReboot;
	HANDLE hCrashMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "CrashMutex");

	DWORD state = WaitForSingleObject(hCrashMutex, INFINITE);

	g_EmuShared->GetMultiXbeFlag(&bQuickReboot);

	if (state == WAIT_OBJECT_0) // StopEmulation
	{
		CloseHandle(hCrashMutex);
		return;
	}

	if (state == WAIT_ABANDONED && !bQuickReboot) // that's a crash
	{
		CloseHandle(hCrashMutex);
		if (m_bIsStarted) // that's a hard crash, Dr Watson is invoked
		{
			KillTimer(m_hwnd, TIMERID_FPS);
			KillTimer(m_hwnd, TIMERID_LED);
			DrawLedBitmap(m_hwnd, true);
			m_hwndChild = NULL;
			m_bIsStarted = false;
			UpdateCaption();
			RefreshMenus();
		}
		return;
	}

	// multi-xbe
	// destroy this thread and start a new one
	CloseHandle(hCrashMutex);
	bQuickReboot = false;
	g_EmuShared->SetMultiXbeFlag(&bQuickReboot);

	return;
}

// draw Xbox LED bitmap
void WndMain::DrawLedBitmap(HWND hwnd, bool bdefault)
{
	HMENU hMenu = GetMenu(hwnd);
	int ActiveLEDColor;

	// When so requested, or when not emulating, draw a black bitmap
	if (bdefault || !m_bIsStarted) {
		ActiveLEDColor = XBOX_LED_COLOUR_OFF;
	}
	else { // draw colored bitmap
		int LedSequence[4] = { XBOX_LED_COLOUR_OFF, XBOX_LED_COLOUR_OFF, XBOX_LED_COLOUR_OFF, XBOX_LED_COLOUR_OFF };
		static int LedSequenceOffset = 0;

		g_EmuShared->GetLedSequence(LedSequence);

		// Select active color and cycle through all 4 phases in the sequence
		ActiveLEDColor = LedSequence[LedSequenceOffset & 3];
		++LedSequenceOffset;
	}

	SelectObject(m_LedDC, m_Brushes[ActiveLEDColor]);
	SelectObject(m_LedDC, m_Pens[ActiveLEDColor]);

	m_OriLed = (HBITMAP)SelectObject(m_LedDC, m_LedBmp);
	Rectangle(m_LedDC, 0, 0, m_xBmp, m_yBmp);
	m_LedBmp = (HBITMAP)SelectObject(m_LedDC, m_OriLed);

	MENUITEMINFO mii = { 0 };
	mii.cbSize = sizeof(mii);
	mii.fMask = MIIM_FTYPE | MIIM_BITMAP;
	mii.fType = MFT_RIGHTJUSTIFY;
	mii.hbmpItem = m_LedBmp;
	SetMenuItemInfo(hMenu, ID_LED, FALSE, &mii);

	DrawMenuBar(hwnd);

	return;
}
