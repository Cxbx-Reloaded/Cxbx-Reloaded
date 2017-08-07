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
#include "CxbxKrnl/EmuShared.h"
#include "ResCxbx.h"
#include "CxbxVersion.h"
#include "Shlwapi.h"

#include <io.h>

#include <sstream> // for std::stringstream
#include "CxbxKrnl/xxhash32.h" // for XXHash32::hash

#define STBI_ONLY_JPEG
#define STBI_NO_LINEAR
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

typedef BOOL (WINAPI *ChangeWindowMessageFilterType)(UINT, DWORD); // for GetProcAddress()

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

WndMain::WndMain(HINSTANCE x_hInstance) :
	Wnd(x_hInstance),
	m_bCreated(false),
	m_Xbe(0),
	m_bXbeChanged(false),
	m_bCanStart(true),
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

        m_w         = 640;
        m_h         = 480;

        m_XbeFilename = (char*)calloc(1, MAX_PATH);

        m_CxbxDebugFilename = (char*)calloc(1, MAX_PATH);
        m_KrnlDebugFilename = (char*)calloc(1, MAX_PATH);

        for(int v=0;v<RECENT_XBE_SIZE;v++)
            m_szRecentXbe[v] = 0;
    }

    // center to desktop
    {
        RECT rect;

        GetWindowRect(GetDesktopWindow(), &rect);

        m_x = rect.left + (rect.right - rect.left)/2 - m_w/2;
        m_y = rect.top + (rect.bottom - rect.top)/2 - m_h/2;
    }

    // load configuration from registry
    {
        DWORD   dwDisposition, dwType, dwSize;
        HKEY    hKey;

        if(RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Cxbx-Reloaded", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_QUERY_VALUE, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS)
        {
			dwType = REG_DWORD; dwSize = sizeof(DWORD);
			RegQueryValueEx(hKey, "LLEFLAGS", NULL, &dwType, (PBYTE)&m_FlagsLLE, &dwSize);

			dwType = REG_DWORD; dwSize = sizeof(DWORD);
			RegQueryValueEx(hKey, "XInputEnabled", NULL, &dwType, (PBYTE)&m_XInputEnabled, &dwSize);

			dwType = REG_DWORD; dwSize = sizeof(DWORD);
			RegQueryValueEx(hKey, "CxbxDebug", NULL, &dwType, (PBYTE)&m_CxbxDebug, &dwSize);

			dwType = REG_DWORD; dwSize = sizeof(DWORD);
            RegQueryValueEx(hKey, "KrnlDebug", NULL, &dwType, (PBYTE)&m_KrnlDebug, &dwSize);

            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            RegQueryValueEx(hKey, "RecentXbe", NULL, &dwType, (PBYTE)&m_dwRecentXbe, &dwSize);

            dwType = REG_SZ; dwSize = MAX_PATH; LONG lErrCodeCxbxDebugFilename;
			lErrCodeCxbxDebugFilename = RegQueryValueEx(hKey, "CxbxDebugFilename", NULL, &dwType, (PBYTE)m_CxbxDebugFilename, &dwSize);

			dwType = REG_SZ; dwSize = MAX_PATH; LONG lErrCodeKrnlDebugFilename;
			lErrCodeKrnlDebugFilename = RegQueryValueEx(hKey, "KrnlDebugFilename", NULL, &dwType, (PBYTE)m_KrnlDebugFilename, &dwSize);

			// Prevent using an incorrect path from the registry if the debug folders have been moved
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
                RegQueryValueEx(hKey, buffer, NULL, &dwType, (PBYTE)m_szRecentXbe[v], &dwSize);
            }

            RegCloseKey(hKey);
        }
    }

	// Allow Drag and Drop if Cxbx is run with elevated privileges on Windows Vista and above

	HMODULE hUser32 = LoadLibrary("User32.dll");
	ChangeWindowMessageFilterType pChangeWindowMessageFilter = (ChangeWindowMessageFilterType)GetProcAddress(hUser32, "ChangeWindowMessageFilter");
	if(pChangeWindowMessageFilter)
	{
		ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
		ChangeWindowMessageFilter(WM_COPYDATA, MSGFLT_ADD);
		ChangeWindowMessageFilter(0x0049, MSGFLT_ADD);
	}
	FreeLibrary(hUser32);

    return;
}

WndMain::~WndMain()
{
    // save configuration to registry
    {
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

            // resize window so that client area := 640x480
            {
                RECT cRect;
                RECT wRect;

                GetClientRect(hwnd, &cRect);
                GetWindowRect(hwnd, &wRect);

                uint32 difW = (wRect.right  - wRect.left) - (cRect.right);
                uint32 difH = (wRect.bottom - wRect.top)  - (cRect.bottom);

                MoveWindow(hwnd, wRect.left, wRect.top, difW + m_w, difH + m_h, TRUE);
            }

            // initialize back buffer
            {
                HDC hDC = GetDC(hwnd);

                m_BackBmp = CreateCompatibleBitmap(hDC, m_w, m_h);

                // decompress jpeg, convert to bitmap resource
                {
                    HRSRC hSrc = FindResource(NULL, MAKEINTRESOURCE(IDR_JPEG_SPLASH), "JPEG");
                    HGLOBAL hRes = LoadResource(NULL, hSrc);

                    uint08 *jpgData = (uint08*)LockResource(hRes);
                    uint32 jpgFileSize = SizeofResource(NULL, hSrc);
                    uint32 bmpFileSize = 0;
                    uint32 bmpWidth = 0;
                    uint32 bmpHeight = 0;

                    uint08 *bmpBuff = reinterpret_cast<uint08*>
                    (
                        stbi_load_from_memory
                        (
                            reinterpret_cast<const stbi_uc*>(jpgData),
                            static_cast<int>(jpgFileSize),
                            reinterpret_cast<int*>(&bmpWidth),
                            reinterpret_cast<int*>(&bmpHeight),
                            nullptr,
                            STBI_rgb
                        )
                    );

                    // create bitmap
                    {
                        BITMAPINFO BmpInfo;

                        BmpInfo.bmiHeader.biSize          = sizeof(BITMAPINFO) - sizeof(RGBQUAD);
                        BmpInfo.bmiHeader.biWidth         = bmpWidth;
                        BmpInfo.bmiHeader.biHeight        = 0 - (long)bmpHeight;
                        BmpInfo.bmiHeader.biPlanes        = 1;
                        BmpInfo.bmiHeader.biBitCount      = 24;
                        BmpInfo.bmiHeader.biCompression   = BI_RGB;
                        BmpInfo.bmiHeader.biSizeImage     = 0;
                        BmpInfo.bmiHeader.biXPelsPerMeter = 0;
                        BmpInfo.bmiHeader.biYPelsPerMeter = 0;
                        BmpInfo.bmiHeader.biClrUsed       = 0;
                        BmpInfo.bmiHeader.biClrImportant  = 0;

                        SetDIBits(hDC, m_BackBmp, 0, bmpHeight, bmpBuff, &BmpInfo, DIB_RGB_COLORS);
                    }

                    stbi_image_free(bmpBuff);

                    FreeResource(hRes);
                    UnlockResource(hRes);
                }

                m_LogoBmp  = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_LOGO), IMAGE_BITMAP, 0, 0, 0);

                m_BackDC   = CreateCompatibleDC(hDC);
                m_LogoDC   = CreateCompatibleDC(hDC);

                m_OrigBmp  = (HBITMAP)SelectObject(m_BackDC, m_BackBmp);
                m_OrigLogo = (HBITMAP)SelectObject(m_LogoDC, m_LogoBmp);

                if(hDC != NULL)
                    ReleaseDC(hwnd, hDC);
            }

            SetClassLong(hwnd, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CXBX)));
			DragAcceptFiles(hwnd, TRUE);

            m_bCreated = true;
        }
        break;

        case WM_PARENTNOTIFY:
        {
            switch(LOWORD(wParam))
            {
                case WM_CREATE:
                {
                    m_hwndChild = GetWindow(hwnd, GW_CHILD);

                    char AsciiTitle[MAX_PATH];

					sprintf(AsciiTitle, "Cxbx-Reloaded %s : Emulating %s", _CXBX_VERSION, m_Xbe->m_szAsciiTitle);

                    SetWindowText(m_hwnd, AsciiTitle);

                    RefreshMenus();
                }
                break;

                case WM_DESTROY:
                {
                    m_hwndChild = NULL;
                    SetWindowText(m_hwnd, "Cxbx-Reloaded " _CXBX_VERSION);
                    RefreshMenus();
                }
                break;
            }
        };

        case WM_SYSKEYDOWN:
        {
            if(m_hwndChild != 0)
            {
                SendMessage(m_hwndChild, uMsg, wParam, lParam);
            }
        };

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

                BitBlt(hDC, 0, 0, m_w, m_h, m_BackDC, 0, 0, SRCCOPY);
//              BitBlt(hDC, 0, 10, 320, 160, m_BackDC, 0, 0, SRCCOPY);
                BitBlt(hDC, m_w-nLogoBmpW-4, m_h-nLogoBmpH-4, nLogoBmpW, nLogoBmpH, m_LogoDC, 0, 0, SRCCOPY);

                int nHeight = -MulDiv(8, GetDeviceCaps(hDC, LOGPIXELSY), 72);

                HFONT hFont = CreateFont(nHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_ROMAN, "Verdana");

                HGDIOBJ tmpObj = SelectObject(hDC, hFont);

                SetBkColor(hDC, RGB(0,0,0));

                SetTextColor(hDC, RGB(255,255,255));

                char buffer[255];

                if(m_Xbe != 0 && m_Xbe->HasError())
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
                    if(m_Xbe != 0 && (m_hwndChild == NULL) && m_bCanStart)
                    {
                        m_bCanStart = false;
                        StartEmulation(hwnd);
                    }
                }
                break;

                case VK_F6:
                {
                    if(m_hwndChild != NULL && !m_bCanStart)
                    {
                        StopEmulation();
                    }
                }
                break;

                default:
                {
                    if(m_hwndChild != 0)
                    {
                        SendMessage(m_hwndChild, uMsg, wParam, lParam);
                    }
                }
            }
        }
        break;

		case WM_DROPFILES:
		{
			if(m_bCanStart)
			{
				if (m_Xbe != 0)
				{
					CloseXbe();
				}
				HDROP hDropInfo = NULL; char *DroppedXbeFilename = (char*)calloc(1, MAX_PATH);
				hDropInfo = (HDROP)wParam;
				DragQueryFile(hDropInfo, 0, DroppedXbeFilename, MAX_PATH);
				OpenXbe(DroppedXbeFilename);
				free(DroppedXbeFilename);
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
					if (m_Xbe != 0)
						CloseXbe();

					if (m_Xbe != 0)
						break;

					OpenXbe(ofn.lpstrFile);
				}
			}
			break;

			case ID_FILE_CLOSE_XBE:
				CloseXbe();
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
				if (m_Xbe != 0)
					CloseXbe();

				if (m_Xbe != 0)
					break;

				HMENU menu = GetMenu(m_hwnd);
				HMENU file_menu = GetSubMenu(menu, 0);
				HMENU rxbe_menu = GetSubMenu(file_menu, 6);

				char szBuffer[270];

				GetMenuString(rxbe_menu, LOWORD(wParam), szBuffer, 269, MF_BYCOMMAND);

				char *szFilename = (char*)((uint32)szBuffer + 5);

				OpenXbe(szFilename);
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
						FILE *TxtFile = fopen(ofn.lpstrFile, "wt");

						// verify file was opened
						if (TxtFile == 0)
							MessageBox(m_hwnd, "Could not open text file.", "Cxbx-Reloaded", MB_ICONSTOP | MB_OK);
						else
						{
							m_Xbe->DumpInformation(TxtFile);

							fclose(TxtFile);

							if (m_Xbe->HasError())
							{
								MessageBox(m_hwnd, m_Xbe->GetError().c_str(), "Cxbx-Reloaded", MB_ICONSTOP | MB_OK);
							}
							else
							{
								char buffer[255];

								sprintf(buffer, "%s's .xbe info was successfully dumped.", m_Xbe->m_szAsciiTitle);

								printf("WndMain: %s\n", buffer);

								MessageBox(m_hwnd, buffer, "Cxbx-Reloaded", MB_ICONINFORMATION | MB_OK);
							}
						}
					}
				}
			}
			break;

			case ID_EDIT_DUMPXBEINFOTO_DEBUGCONSOLE:
			{
				// dump xbe information to debug console
				m_Xbe->DumpInformation(stdout);

				if (m_Xbe->HasError())
				{
					MessageBox(m_hwnd, m_Xbe->GetError().c_str(), "Cxbx-Reloaded", MB_ICONSTOP | MB_OK);
				}
				else
				{
					char buffer[255];

					sprintf(buffer, "%s's .xbe info was successfully dumped.", m_Xbe->m_szAsciiTitle);

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
				sstream << cacheDir << std::hex << uiHash << ".ini";
				std::string fullpath = sstream.str();

				if (DeleteFile(fullpath.c_str())) {
					MessageBox(m_hwnd, "This title's HLE Cache entry has been cleared.", "Cxbx-Reloaded", MB_OK);
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
                StartEmulation(hwnd);
                break;

            case ID_EMULATION_STOP:
                StopEmulation();
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
            if(m_Xbe != 0)
                CloseXbe();

            if(m_Xbe == 0)
                DestroyWindow(hwnd);
        }
        break;

        case WM_DESTROY:
        {
            FreeConsole();

            HDC hDC = GetDC(hwnd);

            SelectObject(m_LogoDC, m_OrigLogo);

            SelectObject(m_BackDC, m_OrigBmp);

            DeleteObject(m_LogoDC);

            DeleteObject(m_BackDC);

            DeleteObject(m_LogoBmp);

            DeleteObject(m_BackBmp);

            ReleaseDC(hwnd, hDC);

            delete m_Xbe;

            m_Xbe = 0;

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

// refresh menu items
void WndMain::RefreshMenus()
{
	bool XbeLoaded = (m_Xbe != 0);
	bool Running = (m_hwndChild != 0);
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
                HMENU rxbe_menu = GetSubMenu(file_menu, 6);

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
                if(m_Xbe != 0)
                {
                    UINT chk_flag = (m_Xbe->m_Header.dwInitFlags.bLimit64MB) ? MF_UNCHECKED : MF_CHECKED;

                    CheckMenuItem(pach_menu, ID_EDIT_PATCH_ALLOW64MB, chk_flag);
                }

                // check "debug mode" if appropriate
                if(m_Xbe != 0)
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
		}

        // emulation menu
        {
            HMENU emul_menu = GetSubMenu(menu, 4);

            // enable emulation start
            EnableMenuItem(emul_menu, ID_EMULATION_START, MF_BYCOMMAND | MF_WhenXbeLoadedNotRunning);

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
    HMENU RXbeMenu = GetSubMenu(FileMenu, 6);

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

// open an xbe file
void WndMain::OpenXbe(const char *x_filename)
{
    if(m_Xbe != 0)
        return;

    strcpy(m_XbeFilename, x_filename);

    m_Xbe = new Xbe(m_XbeFilename);

    if(m_Xbe->HasError())
    {
        MessageBox(m_hwnd, m_Xbe->GetError().c_str(), "Cxbx-Reloaded", MB_ICONSTOP | MB_OK);

        delete m_Xbe; m_Xbe = 0;

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

    delete m_Xbe; m_Xbe = 0;

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

    RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE);
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
void WndMain::StartEmulation(HWND hwndParent)
{
    char szBuffer[MAX_PATH];

    // register xbe path with emulator process
    g_EmuShared->SetXbePath(m_Xbe->m_szPath);

	// register LLE flags with emulator process
	g_EmuShared->SetFlagsLLE(&m_FlagsLLE);

	// register XInput flags with emulator process
	g_EmuShared->SetXInputEnabled(&m_XInputEnabled);

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

        if((int)ShellExecute(NULL, "open", szExeFileName, szArgsBuffer, szBuffer, SW_SHOWDEFAULT) <= 32)
        {
            m_bCanStart = true;
            MessageBox(m_hwnd, "Emulation failed.\n\n If this message repeats, the Xbe is not supported.", "Cxbx-Reloaded", MB_ICONSTOP | MB_OK);

            printf("WndMain: %s shell failed.\n", m_Xbe->m_szAsciiTitle);
        }
        else
        {
            printf("WndMain: %s emulation started.\n", m_Xbe->m_szAsciiTitle);
        }
    }
}

// stop emulation
void WndMain::StopEmulation()
{
    if(!IsWindow(m_hwndChild))
    {
        m_hwndChild = NULL;
        SetWindowText(m_hwnd, "Cxbx-Reloaded " _CXBX_VERSION);
        RefreshMenus();
    }

    SendMessage(m_hwndChild, WM_CLOSE, 0, 0);
    m_bCanStart = true;
}
