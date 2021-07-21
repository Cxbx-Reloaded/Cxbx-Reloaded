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
// *  (c) 2017-2018 RadWolfie
// *  (c) 2018 wutno (#/g/punk - Rizon)
// *
// *  All rights reserved
// *
// ******************************************************************

#define LOG_PREFIX CXBXR_MODULE::GUI

#include "Logging.h"
#include "WndMain.h"
#include "DlgAbout.h"
#include "DlgInputConfig.h"
#include "DlgVideoConfig.h"
#include "DlgAudioConfig.h"
#include "DlgNetworkConfig.h"
#include "DlgEepromConfig.h"
#include "DlgLoggingConfig.h"
#include "common\xbe\XbePrinter.h" // For DumpInformation
#include "EmuShared.h"
#include "core\hle\D3D8\Direct3D9\Direct3D9.h" // For CxbxSetPixelContainerHeader
#include "core\hle\D3D8\XbConvert.h" // For EmuPC2XB_D3DFormat
#include "common\Settings.hpp"
#include "common/util/cliConfig.hpp"
#include "common/win32/WineEnv.h"

#include "core\kernel\init\CxbxKrnl.h" // For CxbxExec
#include "resource/ResCxbx.h"
#include "CxbxVersion.h"
#include "Shlwapi.h"

#undef GetSystemMetrics // Force remove DirectX 8's multimon.h defined function (redirect to xGetSystemMetrics).
#include <WinUser.h> // For GetSystemMetrics

#include <io.h>
#include <shlobj.h>

#include <sstream> // for std::stringstream
#include <fstream>
#include <iostream>
#include <fcntl.h> // for _O_TEXT
#include "common\util\hasher.h"

#define XBOX_LED_FLASH_PERIOD 176 // if you know a more accurate value, put it here

static int gameLogoWidth, gameLogoHeight;
static int splashLogoWidth, splashLogoHeight;

bool g_SaveOnExit = true;

void ClearSymbolCache(const char sStorageLocation[MAX_PATH])
{
	std::string cacheDir = std::string(sStorageLocation) + "\\SymbolCache\\";
	std::string fullpath = cacheDir + "*.ini";

	WIN32_FIND_DATA data;
	HANDLE hFind = FindFirstFile(fullpath.c_str(), &data);

	if (hFind != INVALID_HANDLE_VALUE) {
		BOOL bContinue = TRUE;
		do {
			if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
				fullpath = cacheDir + data.cFileName;

				if (!std::filesystem::remove(fullpath)) {
					break;
				}
			}

			bContinue = FindNextFile(hFind, &data);
		} while (bContinue);

		FindClose(hFind);
	}

	printf("Cleared HLE Cache\n");
}

unsigned ClearCachePartitions()
{
	const std::string &partition_path = g_Settings->GetDataLocation() + "\\EmuDisk\\Partition";
	std::error_code err;
	unsigned has_any_err = 0;

	for (int partition_num = 3; partition_num < 6; ++partition_num) {
		const std::string &partition = partition_path + std::to_string(partition_num);
		for (const auto &directory_entry : std::filesystem::directory_iterator(partition, err)) {
			if (err) {
				has_any_err |= 1;
			}
			else {
				std::filesystem::remove_all(directory_entry, err);
				if (err) {
					has_any_err |= 1;
				}
			}
		}

		const std::string &partition_bin = partition + ".bin";
		std::filesystem::remove(partition_bin, err);
		if (err) {
			has_any_err |= 1;
		}
	}

	return has_any_err;
}

void WndMain::InitializeSettings()
{
	g_Settings->Delete();
	g_SaveOnExit = false;
}

#define TIMERID_ACTIVE_EMULATION 0
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
		Settings::s_video XBVideoConf;
		g_EmuShared->GetVideoSettings(&XBVideoConf);

		const char* resolution = XBVideoConf.szVideoResolution;
		if (2 != sscanf(resolution, "%d x %d", &m_w, &m_h)) {
			EmuLog(LOG_LEVEL::DEBUG, "Couldn't parse resolution : %s.", resolution);
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

	if (bForGUI && m_prevWindowLoc.x != -1 && m_prevWindowLoc.y != -1) {
		// Restore to previous Window location
		m_x = m_prevWindowLoc.x;
		m_y = m_prevWindowLoc.y;
	}
	else {
		// Center to desktop
		m_x = desktopRect.left + ((dWidth - m_w) / 2);
		m_y = desktopRect.top + ((dHeight - m_h) / 2);
	}

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
	Wnd(x_hInstance)
	, m_bCreated(false)
	, m_Xbe(nullptr)
	, m_bXbeChanged(false)
	, m_bIsStarted(false)
	, m_hwndChild(nullptr)
	, m_hDebuggerProc(nullptr)
	, m_hDebuggerMonitorThread()
	, m_prevWindowLoc({ -1, -1 })
	, m_LogKrnl_status(false)
{
	// initialize members
	{
		m_classname = "WndMain";
		m_wndname   = "Cxbx-Reloaded " + std::string(CxbxrHashBuild);
	}

	// load configuration from settings file
	{
		// NOTE: Settings has already been initalized/load from file before WndMain constructed.

		g_Settings->Verify();

		// NOTE: This is a requirement for pre-verification from GUI. Used in CxbxInitFilePaths function.
		g_EmuShared->SetStorageLocation(g_Settings->GetDataLocation().c_str());

		unsigned int i = 0;
		do {
			if (g_Settings->m_gui.szRecentXbeFiles[i].size() == 0) {
				break;
			}
			i++;
		} while (i < RECENT_XBE_LIST_MAX);

		m_dwRecentXbe = i;
	}
}

WndMain::~WndMain()
{
    // save configuration to registry
    if (g_SaveOnExit) {
        g_Settings->Save();
    }

    // Close opened debugger monitor if there is one
    DebuggerMonitorClose();

    // cleanup allocations
    {
        delete m_Xbe;
        delete g_Settings;
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

            // Check if running in Wine environment. If it is, then we don't need admin warning message popup on startup.
            if (isWineEnv()) {
                BOOL bAdminCheckRemoveRet = RemoveMenu(GetMenu(hwnd), ID_SETTINGS_ALLOWADMINPRIVILEGE, MF_GRAYED);
                assert(bAdminCheckRemoveRet != -1);
            }

            SetClassLong(hwnd, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_CXBX)));
			DragAcceptFiles(hwnd, TRUE);

			// Allow Drag and Drop if Cxbx is run with elevated privileges on Windows Vista and above

			ChangeWindowMessageFilterEx(hwnd, WM_DROPFILES, MSGFLT_ALLOW, nullptr);
			ChangeWindowMessageFilterEx(hwnd, WM_COPYDATA, MSGFLT_ALLOW, nullptr);
			ChangeWindowMessageFilterEx(hwnd, 0x0049, MSGFLT_ALLOW, nullptr);

            m_bCreated = true;
        }
        break;

		// NOTE: WM_PARENTNOTIFY was triggered by kernel process' graphic window creation.
		case WM_PARENTNOTIFY:
		{
			switch(LOWORD(wParam))
			{
				case WM_CREATE:
				{
					if (m_hwndChild == NULL) {
						m_FPS_status = 0.0f;
						m_MSpF_status = 0.0f;
						m_LedSeq_status_block = (XBOX_LED_COLOUR_GREEN << 24) |
						                        (XBOX_LED_COLOUR_GREEN << 16) |
						                        (XBOX_LED_COLOUR_GREEN << 8) |
						                        (XBOX_LED_COLOUR_GREEN);
						SetTimer(hwnd, TIMERID_ACTIVE_EMULATION, 1000, (TIMERPROC)nullptr);
						SetTimer(hwnd, TIMERID_LED, XBOX_LED_FLASH_PERIOD, (TIMERPROC)nullptr);
						m_hwndChild = GetWindow(hwnd, GW_CHILD);
						UpdateCaption();
						RefreshMenus();
					}
					else {
						m_hwndChild = GetWindow(hwnd, GW_CHILD);
					}
				}
				break;

				case WM_DESTROY:
				{
					// (HWND)HIWORD(wParam) seems to be NULL, so we can't compare to m_hwndChild
					if (m_hwndChild != NULL) { // Let's hope this signal originated from the only child window
						KillTimer(hwnd, TIMERID_ACTIVE_EMULATION);
						KillTimer(hwnd, TIMERID_LED);
						m_hwndChild = NULL;
						StopEmulation();
						DrawLedBitmap(hwnd, true);
					}
				}
				break;

				case WM_COMMAND:
				{
					switch (HIWORD(wParam)) {
						case ID_GUI_STATUS_LLE_FLAGS:
							m_FlagsLLE_status = static_cast<UINT>(lParam);
							break;

						case ID_GUI_STATUS_XBOX_LED_COLOUR:
							m_LedSeq_status_block = static_cast<UINT>(lParam);
							break;

						case ID_GUI_STATUS_LOG_ENABLED:
							m_LogKrnl_status = static_cast<bool>(lParam);
							UpdateLogStatus();
							break;

						// NOTE: If anything need to set before kernel process start do anything, do it here.
						case ID_GUI_STATUS_KRNL_IS_READY: {
							Crash_Manager_Data* pCMD = (Crash_Manager_Data*)malloc(sizeof(Crash_Manager_Data));
							pCMD->pWndMain = this;
							pCMD->dwChildProcID = lParam; // lParam is process ID.
							std::thread(CrashMonitorWrapper, pCMD).detach();

							g_EmuShared->SetIsEmulating(true); // NOTE: Putting in here raise to low or medium risk due to debugger will launch itself. (Current workaround)
							g_EmuShared->SetIsReady(true);
						}
						break;

						case ID_GUI_STATUS_OVERLAY:
							g_EmuShared->GetOverlaySettings(&g_Settings->m_overlay);
							break;
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
				case TIMERID_ACTIVE_EMULATION:
				{
					RefreshAllStatus();
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
            else
            {
				return DefWindowProc(hwnd, uMsg, wParam, lParam);
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
					// Start emulation normally
					if (!m_bIsStarted) {
						// Try to open the most recent Xbe if none is opened yet :
						if (m_Xbe == nullptr)
							OpenMRU(0);

						if (m_Xbe != nullptr)
							StartEmulation(hwnd);

						break;
					}
					// fall through
                }

                case VK_F6:
                {
					// Stop emulation
					if (m_bIsStarted)
					{
						StopEmulation();
						break;
					}
					// fall through
                }

				case VK_F7:
				{
					// Open the dashboard xbe
					if (!m_bIsStarted)
					{
						if (m_Xbe != nullptr) { CloseXbe(); }

						OpenDashboard();
						break;
					}
					// fall through
				}

				case VK_F9:
				{
					// Start emulation with the debugger
					if (!m_bIsStarted) {
						// Try to open the most recent Xbe if none is opened yet
						if (m_Xbe == nullptr)
							OpenMRU(0);

						if (m_Xbe != nullptr)
							StartEmulation(hwnd, debuggerOn);

						break;
					}
					// fall through
				}

                default:
                {
                    if(m_hwndChild != NULL)
                    {
                        SendMessage(m_hwndChild, uMsg, wParam, lParam);
                    }
                    else
                    {
                        DefWindowProc(hwnd, uMsg, wParam, lParam);
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
				ofn.lpstrFileTitle = nullptr;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = nullptr;
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
				ofn.lpstrFileTitle = nullptr;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = nullptr;
				ofn.lpstrDefExt = "bmp";
				ofn.lpstrTitle = "Export Logo Bitmap";
				ofn.Flags = OFN_PATHMUSTEXIST;

				if (GetSaveFileName(&ofn) == TRUE)
				{
					// ask permission to overwrite if file already exists
					if (_access(ofn.lpstrFile, 0) != -1)
					{
						if (PopupQuestionEx(m_hwnd, LOG_LEVEL::WARNING, PopupButtons::YesNo, PopupReturn::No,
							"Overwrite existing file?") != PopupReturn::Yes)
							return TRUE;
					}

					// export logo bitmap
					{
						uint8_t i_gray[100 * 17];

						m_Xbe->ExportLogoBitmap(i_gray);

						if (false == m_Xbe->HasError())
						{
							FILE *LogoBitmap = fopen(ofn.lpstrFile, "wb");

							// write bitmap header
							{
								BITMAPFILEHEADER    bmfh;

								bmfh.bfType = *(uint16_t*)"BM";
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

								for (uint32_t v = 0; v < 100 * 17; v++)
								{
									bmp_data[v].rgbtRed = i_gray[v];
									bmp_data[v].rgbtGreen = i_gray[v];
									bmp_data[v].rgbtBlue = i_gray[v];
								}

								fwrite(bmp_data, 100 * 17 * sizeof(RGBTRIPLE), 1, LogoBitmap);
							}

							// write bitmap padding
							{
								uint16_t pad = 0;

								fwrite(&pad, 2, 1, LogoBitmap);
							}

							fclose(LogoBitmap);
						}

						if (m_Xbe->HasError())
							PopupError(m_hwnd, m_Xbe->GetError().c_str());
						else
						{
							char buffer[255];

							sprintf(buffer, "%s's logo bitmap was successfully exported.", m_Xbe->m_szAsciiTitle);

							PopupInfo(m_hwnd, buffer);
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
				ofn.lpstrFileTitle = nullptr;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = nullptr;
				ofn.lpstrDefExt = "bmp";
				ofn.lpstrTitle = "Import Logo Bitmap";
				ofn.Flags = OFN_PATHMUSTEXIST;

				if (GetOpenFileName(&ofn) == TRUE)
				{
					// import logo bitmap
					{
						uint8_t i_gray[100 * 17];

						// read bitmap file
						{
							FILE *logo = fopen(ofn.lpstrFile, "rb");

							char *bmp_err = 0;

							// read bitmap header
							if (!bmp_err)
							{
								BITMAPFILEHEADER bmfh;

								fread(&bmfh, sizeof(bmfh), 1, logo);

								if (bmfh.bfType != *(uint16_t*)"BM")
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

								for (uint32_t c = 0; c < 100 * 17; c++)
									i_gray[c] = (char)(((float)bmp_data[c].rgbtRed + (float)bmp_data[c].rgbtGreen + (float)bmp_data[c].rgbtBlue) / 3.0);
							}

							fclose(logo);

							if (bmp_err != 0)
							{
								PopupError(m_hwnd, bmp_err);
								break;
							}
						}

						m_Xbe->ImportLogoBitmap(i_gray);

						if (m_Xbe->HasError())
						{
							PopupError(m_hwnd, m_Xbe->GetError().c_str());

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

							PopupInfo(m_hwnd, buffer);
						}
					}
				}
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
				ofn.lpstrFileTitle = nullptr;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = nullptr;
				ofn.lpstrDefExt = "txt";
				ofn.Flags = OFN_PATHMUSTEXIST;

				if (GetSaveFileName(&ofn) == TRUE)
				{
					// ask permission to overwrite if file exists
					if (_access(ofn.lpstrFile, 0) != -1)
					{
						if (PopupQuestion(m_hwnd, "Overwrite existing file?") != PopupReturn::Yes)
							return TRUE;
					}

					// dump xbe information to file
					{
						std::string Xbe_info = DumpInformation(m_Xbe);
						if (m_Xbe->HasError()) {
							PopupError(m_hwnd, m_Xbe->GetError().c_str());
						}
						else {
							std::ofstream Xbe_dump_file(ofn.lpstrFile);
							if (Xbe_dump_file.is_open()) {
								Xbe_dump_file << Xbe_info;
								Xbe_dump_file.close();
								char buffer[255];
								sprintf(buffer, "%s's .xbe info was successfully dumped.", m_Xbe->m_szAsciiTitle);
								PopupInfo(m_hwnd, buffer);
							}
							else {
								PopupError(m_hwnd, "Could not open Xbe text file.");
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
					PopupError(m_hwnd, m_Xbe->GetError().c_str());
				}
				else {
					std::cout << Xbe_info;
					char buffer[255];
					sprintf(buffer, "%s's .xbe info was successfully dumped to console.", m_Xbe->m_szAsciiTitle);
					printf("WndMain: %s\n", buffer);
				}
			}
			break;

			case ID_SETTINGS_CONFIG_INPUT:
				ShowInputConfig(hwnd, m_hwndChild);
				break;

			case ID_SETTINGS_CONFIG_VIDEO:
				ShowVideoConfig(hwnd);
				break;

			case ID_SETTINGS_CONFIG_AUDIO:
				ShowAudioConfig(hwnd);
				break;

			case ID_SETTINGS_CONFIG_NETWORK:
				ShowNetworkConfig(hwnd);
				break;

			case ID_SETTINGS_CONFIG_EEPROM:
			{
				if (m_bIsStarted) {
					// We don't allow changing the contents of the eeprom while a game is running, mostly because we lack a "pause emulation"
					// function necessary to modify the contents safely (the game itself can modify the eeprom)
					PopupError(hwnd, "Cannot modify eeprom file while a title is running.");
					break;
				}
				ShowEepromConfig(hwnd);
			}
			break;

			case ID_SETTINGS_CONFIG_LOGGING:
			{
				ShowLoggingConfig(hwnd, m_hwndChild);
			}
			break;

			case ID_SETTINGS_CONFIG_DLOCCUSTOM:
			{
				char szDir[MAX_PATH];

				BROWSEINFO bInfo;
				bInfo.hwndOwner = NULL;
				bInfo.pidlRoot = nullptr;
				bInfo.pszDisplayName = szDir;
				bInfo.lpszTitle = "Please, select a folder";
				bInfo.ulFlags = BIF_NEWDIALOGSTYLE, BIF_EDITBOX, BIF_VALIDATE;
				bInfo.lpfn = nullptr;
				bInfo.lParam = 0;
				bInfo.iImage = -1;

				LPITEMIDLIST lpItem = SHBrowseForFolder(&bInfo);

				if (lpItem != nullptr)
				{
					SHGetPathFromIDList(lpItem, szDir);

					// -14 is for \\Cxbx-Reloaded string to be include later down below.
					size_t szLen = strnlen(szDir, MAX_PATH - 14);
					if (szLen == 0) {
						PopupError(hwnd, "You've selected an invalid folder... Go back and try again.");
						break;
					}
					else if (szLen == MAX_PATH - 14) {
						PopupError(hwnd, "You've selected a folder path which is too long... Go back and try again.");
						break;
					}

					std::string szDirTemp = std::string(szDir) + std::string("\\Cxbx-Reloaded");

					if (szDirTemp.size() > MAX_PATH) {
						PopupError(hwnd, "Directory path is too long. Go back and choose a shorter path.");
						break;
					}

					int result = SHCreateDirectoryEx(nullptr, szDirTemp.c_str(), nullptr);
					if ((result != ERROR_SUCCESS) && (result != ERROR_ALREADY_EXISTS)) {
						PopupError(hwnd, "You don't have write permissions on that directory...");
						break;
					}

					g_Settings->m_gui.DataStorageToggle = CXBX_DATA_CUSTOM;
					g_Settings->m_gui.szCustomLocation = szDirTemp;
					RefreshMenus();
				}
			}
			break;

			case ID_SETTINGS_CONFIG_DLOCAPPDATA:
			{
				g_Settings->m_gui.DataStorageToggle = CXBX_DATA_APPDATA;
				RefreshMenus();
			}
			break;

			case ID_SETTINGS_CONFIG_DLOCEXECDIR:
			{
				g_Settings->m_gui.DataStorageToggle = CXBX_DATA_EXECDIR;
				RefreshMenus();
			}
			break;

			case ID_CACHE_CLEARHLECACHE_ALL:
			{
				ClearSymbolCache(g_Settings->GetDataLocation().c_str());
				PopupInfo(m_hwnd, "The entire Symbol Cache has been cleared.");
			}
			break;

			case ID_CACHE_CLEARHLECACHE_CURRENT:
			{
				std::string cacheDir = g_Settings->GetDataLocation() + "\\SymbolCache\\";

				// Hash the loaded XBE's header, use it as a filename
				uint64_t uiHash = ComputeHash((void*)&m_Xbe->m_Header, sizeof(Xbe::Header));
				std::stringstream sstream;
				std::string szTitleName(m_Xbe->m_szAsciiTitle);
				m_Xbe->PurgeBadChar(szTitleName);
				sstream << cacheDir << szTitleName << "-" << std::hex << uiHash << ".ini";
				std::string fullpath = sstream.str();

				if (std::filesystem::remove(fullpath)) {
					PopupInfo(m_hwnd, "This title's Symbol Cache entry has been cleared.");
				}
			}
			break;

			case ID_SETTINGS_CLEAR_PARTITIONS:
			{
				if (m_bIsStarted) {
					PopupError(m_hwnd, "Cannot clear the cache partitions while a title is running.");
				}
				else {
					if (ClearCachePartitions()) {
						PopupError(m_hwnd, "An error has occurred while cleaning the cache partitions.");
					}
					else {
						PopupInfo(m_hwnd, "Cleared the cache partitions.");
					}
				}
			}
			break;

			case ID_SETTINGS_INITIALIZE:
			{
				PopupReturn ret = PopupWarningEx(m_hwnd, PopupButtons::YesNo, PopupReturn::No,
					"Warning: This will reset all Cxbx-Reloaded settings to their default values.\nAre you sure you want to proceed?", "Cxbx-Reloaded");

				if (ret == PopupReturn::Yes) {
					InitializeSettings();
					PopupInfo(m_hwnd, "Cxbx-Reloaded has been initialized and will now close.");
					SendMessage(hwnd, WM_CLOSE, 0, 0);
				}
			}
			break;

			case ID_EMULATION_DEBUGOUTPUTKERNEL_CONSOLE:
			{
				if (g_Settings->m_core.KrnlDebugMode == DM_NONE || g_Settings->m_core.KrnlDebugMode == DM_FILE) {
					g_Settings->m_core.KrnlDebugMode = DM_CONSOLE;
				}
				else {
					g_Settings->m_core.KrnlDebugMode = DM_NONE;
				}
				PopupInfo(m_hwnd, "This will not take effect until the next time emulation is started.");

				RefreshMenus();

				UpdateDebugConsoles();
			}
			break;

			case ID_EMULATION_DEBUGOUTPUTKERNEL_FILE:
			{
				if (g_Settings->m_core.KrnlDebugMode == DM_FILE) {
					g_Settings->m_core.KrnlDebugMode = DM_NONE;

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
					ofn.lpstrFileTitle = nullptr;
					ofn.nMaxFileTitle = 0;
					ofn.lpstrInitialDir = nullptr;
					ofn.lpstrDefExt = "txt";
					ofn.Flags = OFN_PATHMUSTEXIST;

					if (GetSaveFileName(&ofn) != FALSE)
					{
						PopupInfo(m_hwnd, "This will not take effect until emulation is (re)started.");

						strncpy(g_Settings->m_core.szKrnlDebug, ofn.lpstrFile, MAX_PATH - 1);

						g_Settings->m_core.KrnlDebugMode = DM_FILE;

						RefreshMenus();

						UpdateDebugConsoles();
					}
				}
			}
			break;

			case ID_EMULATION_DEBUGOUTPUTGUI_CONSOLE:
			{
				if (g_Settings->m_gui.CxbxDebugMode == DM_NONE || g_Settings->m_gui.CxbxDebugMode == DM_FILE) {
					g_Settings->m_gui.CxbxDebugMode = DM_CONSOLE;
				}
				else {
					g_Settings->m_gui.CxbxDebugMode = DM_NONE;
				}
				RefreshMenus();

				UpdateDebugConsoles();
			}
			break;

			case ID_EMULATION_DEBUGOUTPUTGUI_FILE:
			{
				if (g_Settings->m_gui.CxbxDebugMode == DM_FILE)
				{
					g_Settings->m_gui.CxbxDebugMode = DM_NONE;

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
					ofn.lpstrFileTitle = nullptr;
					ofn.nMaxFileTitle = 0;
					ofn.lpstrInitialDir = nullptr;
					ofn.lpstrDefExt = "txt";
					ofn.Flags = OFN_PATHMUSTEXIST;

					if (GetSaveFileName(&ofn) != FALSE)
					{
						g_Settings->m_gui.szCxbxDebugFile = ofn.lpstrFile;

						g_Settings->m_gui.CxbxDebugMode = DM_FILE;

						RefreshMenus();

						UpdateDebugConsoles();
					}

				}
			}
			break;

			case ID_EMULATION_LLE_JIT:
			{
				g_Settings->m_core.FlagsLLE = g_Settings->m_core.FlagsLLE ^ LLE_JIT;
				RefreshMenus();
			}
			break;

			case ID_EMULATION_LLE_APU:
			{
				g_Settings->m_core.FlagsLLE = g_Settings->m_core.FlagsLLE ^ LLE_APU;
				RefreshMenus();
			}
			break;

			case ID_EMULATION_LLE_GPU:
			{
				g_Settings->m_core.FlagsLLE = g_Settings->m_core.FlagsLLE ^ LLE_GPU;
				RefreshMenus();
			}
			break;
#if 0 // Reenable this when LLE USB actually works
			case ID_EMULATION_LLE_USB:
			{
				g_Settings->m_core.FlagsLLE = g_Settings->m_core.FlagsLLE ^ LLE_USB;
				RefreshMenus();
			}
			break;
#endif
			case ID_USELOADEREXEC:
			{
				g_Settings->m_core.bUseLoaderExec = !g_Settings->m_core.bUseLoaderExec;
				RefreshMenus();
			}
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
				g_Settings->m_hacks.DisablePixelShaders = !g_Settings->m_hacks.DisablePixelShaders;
				RefreshMenus();
				break;

			case ID_HACKS_RUNXBOXTHREADSONALLCORES:
				if (g_Settings->m_hacks.UseAllCores == false) {
					PopupReturn ret = PopupWarningEx(hwnd, PopupButtons::YesNo, PopupReturn::No,
						"Activating this hack will make the emulator more likely to crash and/or hang."
						"\nPlease do not report issues with games while this hack is active. Are you sure you want to turn it on?");
					if (ret != PopupReturn::Yes) {
						break;
					}
				}
				g_Settings->m_hacks.UseAllCores = !g_Settings->m_hacks.UseAllCores;
				RefreshMenus();
				break;

			case ID_HACKS_SKIPRDTSCPATCHING:
				g_Settings->m_hacks.SkipRdtscPatching = !g_Settings->m_hacks.SkipRdtscPatching;
				RefreshMenus();
				break;

			case ID_SETTINGS_IGNOREINVALIDXBESIG:
				g_Settings->m_gui.bIgnoreInvalidXbeSig = !g_Settings->m_gui.bIgnoreInvalidXbeSig;
				RefreshMenus();
				break;

			case ID_SETTINGS_IGNOREINVALIDXBESEC:
				g_Settings->m_gui.bIgnoreInvalidXbeSec = !g_Settings->m_gui.bIgnoreInvalidXbeSec;
				RefreshMenus();
				break;

			case ID_SETTINGS_ALLOWADMINPRIVILEGE:
				g_Settings->m_core.allowAdminPrivilege = !g_Settings->m_core.allowAdminPrivilege;
				RefreshMenus();
				break;

            case ID_HELP_ABOUT:
            {
				ShowAboutDialog(hwnd);
            }
            break;

            case ID_HELP_HOMEPAGE:
                ShellExecute(NULL, "open", "https://github.com/Cxbx-Reloaded/Cxbx-Reloaded", nullptr, nullptr, SW_SHOWNORMAL);
                break;

            }

            break;
        }

		case WM_MOVE:
		{
			// Redraw the window on move, prevents corrupt background image that happens
			// when windows doesn't call the WM_DRAW event when the window is moved too quickly.
			RedrawWindow(hwnd, nullptr, NULL, RDW_INVALIDATE);
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
    if(strrchr(x_orig_filename, '\\') != nullptr)
    {
        strcpy(x_filename, x_orig_filename);
		char *loc = strrchr(x_filename, '.');
		if (loc != nullptr)
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
    uint8_t i_gray[100*17];

    m_Xbe->ExportLogoBitmap(i_gray);

    if(m_Xbe->HasError())
    {
        PopupError(m_hwnd, m_Xbe->GetError().c_str());

        if (m_Xbe->HasFatalError())
        {
            CloseXbe();
        }

        return;
    }

    uint32_t v=0;
    for(uint32_t y=0;y<17;y++)
    {
        for(uint32_t x=0;x<100;x++)
        {
            SetPixel(m_LogoDC, x, y, RGB(i_gray[v], i_gray[v], i_gray[v]));
            v++;
        }
    }

    RedrawWindow(m_hwnd, nullptr, NULL, RDW_INVALIDATE);
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
	uint8_t *pSection = (uint8_t *)m_Xbe->FindSection<false>("$$XTIMAGE"); // Check for XTIMAGE
	if (!pSection) {
		pSection = (uint8_t *)m_Xbe->FindSection<false>("$$XSIMAGE"); // if XTIMAGE isn't present, check for XSIMAGE (smaller)
		if (!pSection) {
			return;
		}
	}

	gameLogoWidth = 0;
	gameLogoHeight = 0;

    uint8_t *ImageData = nullptr;
	xbox::X_D3DPixelContainer XboxPixelContainer = {};
	xbox::X_D3DPixelContainer *pXboxPixelContainer = &XboxPixelContainer;

	switch (*(DWORD*)pSection) {
	case MAKEFOURCC('D', 'D', 'S', ' '): {
		DDS_HEADER *pDDSHeader = (DDS_HEADER *)(pSection + sizeof(DWORD));
		D3DFORMAT Format = D3DFMT_UNKNOWN;
		if (pDDSHeader->ddspf.dwFlags & DDPF_FOURCC) {
			switch (pDDSHeader->ddspf.dwFourCC) {
			case MAKEFOURCC('D', 'X', 'T', '1'): Format = D3DFMT_DXT1; break;
			case MAKEFOURCC('D', 'X', 'T', '3'): Format = D3DFMT_DXT3; break;
			case MAKEFOURCC('D', 'X', 'T', '5'): Format = D3DFMT_DXT5; break;
			}
		}
		else {
			// TODO : Determine D3D format based on pDDSHeader->ddspf.dwABitMask, .dwRBitMask, .dwGBitMask and .dwBBitMask
		}

		if (Format == D3DFMT_UNKNOWN)
			return;

		ImageData = (uint8_t *)(pSection + sizeof(DWORD) + pDDSHeader->dwSize);
		//gameLogoHeight = pDDSHeader->dwHeight;
		//gameLogoWidth = pDDSHeader->dwWidth;

		// TODO : Use PixelCopy code here to decode. For now, fake it :
		CxbxSetPixelContainerHeader(&XboxPixelContainer,
			0, // Common - could be X_D3DCOMMON_TYPE_TEXTURE
			(UINT)pDDSHeader->dwWidth,
			(UINT)pDDSHeader->dwHeight,
			1,
			EmuPC2XB_D3DFormat(Format),
			2,
			(UINT)pDDSHeader->dwPitchOrLinearSize);
		break;
	}
	case MAKEFOURCC('X', 'P', 'R', '0'):
	case MAKEFOURCC('X', 'P', 'R', '1'): {
		struct Xbe::XprHeader *pXprHeader = (struct Xbe::XprHeader*)pSection;

		unsigned int SizeOfResourceHeaders = pXprHeader->dwXprHeaderSize - sizeof(Xbe::XprHeader);
		unsigned int SizeOfResourceData = pXprHeader->dwXprTotalSize - pXprHeader->dwXprHeaderSize;

		uint8_t *ResourceHeaders = pSection + sizeof(Xbe::XprHeader);
		uint8_t *ResourceData = ResourceHeaders + SizeOfResourceHeaders;

		pXboxPixelContainer = (xbox::X_D3DPixelContainer*)ResourceHeaders;
		ImageData = ResourceData;

		break;
	}
	default: {
		return;
	}
	}

	void *bitmapData = ConvertD3DTextureToARGB(pXboxPixelContainer, ImageData, &gameLogoWidth, &gameLogoHeight);
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

	RedrawWindow(m_hwnd, nullptr, NULL, RDW_INVALIDATE);

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

            // recent xbe files menu
            {
                HMENU rxbe_menu = GetSubMenu(file_menu, 4);

                int max = m_dwRecentXbe;
                for(int v=0;v<max;v++)
                    EnableMenuItem(rxbe_menu, ID_FILE_RXBE_0 + v, MF_BYCOMMAND | MF_ENABLED);
            }
        }

        // edit menu
        {
            HMENU edit_menu = GetSubMenu(menu, 1);
            HMENU logo_menu = GetSubMenu(edit_menu, 0);

            // enable export .xbe info
            EnableMenuItem(edit_menu, ID_EDIT_DUMPXBEINFOTO_FILE, MF_BYCOMMAND | MF_WhenXbeLoaded);
            EnableMenuItem(edit_menu, ID_EDIT_DUMPXBEINFOTO_DEBUGCONSOLE, MF_BYCOMMAND | MF_WhenXbeLoaded);

            // enable logo bitmap menu
            EnableMenuItem(edit_menu, 0, MF_BYPOSITION | MF_WhenXbeLoaded);

            // enable patch menu
            EnableMenuItem(edit_menu, 1, MF_BYPOSITION | MF_WhenXbeLoaded);
        }

        // view menu
		{
			HMENU view_menu = GetSubMenu(menu, 2);
			HMENU emul_debg = GetSubMenu(view_menu, 0);
			HMENU emul_krnl = GetSubMenu(view_menu, 1);

			switch (g_Settings->m_core.KrnlDebugMode) {
				case DM_CONSOLE:
					CheckMenuItem(emul_krnl, ID_EMULATION_DEBUGOUTPUTKERNEL_CONSOLE, MF_CHECKED);
					CheckMenuItem(emul_krnl, ID_EMULATION_DEBUGOUTPUTKERNEL_FILE, MF_UNCHECKED);
					break;

				case DM_FILE:
					CheckMenuItem(emul_krnl, ID_EMULATION_DEBUGOUTPUTKERNEL_CONSOLE, MF_UNCHECKED);
					CheckMenuItem(emul_krnl, ID_EMULATION_DEBUGOUTPUTKERNEL_FILE, MF_CHECKED);
					break;

				default:
					CheckMenuItem(emul_krnl, ID_EMULATION_DEBUGOUTPUTKERNEL_CONSOLE, MF_UNCHECKED);
					CheckMenuItem(emul_krnl, ID_EMULATION_DEBUGOUTPUTKERNEL_FILE, MF_UNCHECKED);
					break;
			}

			switch (g_Settings->m_gui.CxbxDebugMode) {
				case DM_CONSOLE:
					CheckMenuItem(emul_debg, ID_EMULATION_DEBUGOUTPUTGUI_CONSOLE, MF_CHECKED);
					CheckMenuItem(emul_debg, ID_EMULATION_DEBUGOUTPUTGUI_FILE, MF_UNCHECKED);
					break;

				case DM_FILE:
					CheckMenuItem(emul_debg, ID_EMULATION_DEBUGOUTPUTGUI_CONSOLE, MF_UNCHECKED);
					CheckMenuItem(emul_debg, ID_EMULATION_DEBUGOUTPUTGUI_FILE, MF_CHECKED);
					break;

				default:
					CheckMenuItem(emul_debg, ID_EMULATION_DEBUGOUTPUTGUI_CONSOLE, MF_UNCHECKED);
					CheckMenuItem(emul_debg, ID_EMULATION_DEBUGOUTPUTGUI_FILE, MF_UNCHECKED);
					break;
			}
			UpdateLogStatus();
		}

		// settings menu
		{
			HMENU settings_menu = GetSubMenu(menu, 3);

			// enable/disable clear current hle cache
			EnableMenuItem(settings_menu, ID_CACHE_CLEARHLECACHE_CURRENT, MF_BYCOMMAND | MF_WhenXbeLoadedNotRunning);

			UINT chk_flag = (g_Settings->m_core.FlagsLLE & LLE_JIT) ? MF_CHECKED : MF_UNCHECKED;
			CheckMenuItem(settings_menu, ID_EMULATION_LLE_JIT, chk_flag);

			chk_flag = (g_Settings->m_core.FlagsLLE & LLE_APU) ? MF_CHECKED : MF_UNCHECKED;
			CheckMenuItem(settings_menu, ID_EMULATION_LLE_APU, chk_flag);

			chk_flag = (g_Settings->m_core.FlagsLLE & LLE_GPU) ? MF_CHECKED : MF_UNCHECKED;
			CheckMenuItem(settings_menu, ID_EMULATION_LLE_GPU, chk_flag);

			//chk_flag = (g_Settings->m_core.FlagsLLE & LLE_USB) ? MF_CHECKED : MF_UNCHECKED; // Reenable this when LLE USB actually works
			//CheckMenuItem(settings_menu, ID_EMULATION_LLE_USB, chk_flag);

			chk_flag = g_Settings->m_core.bUseLoaderExec ? MF_CHECKED : MF_UNCHECKED;
			CheckMenuItem(settings_menu, ID_USELOADEREXEC, chk_flag);

			chk_flag = (g_Settings->m_hacks.DisablePixelShaders) ? MF_CHECKED : MF_UNCHECKED;
			CheckMenuItem(settings_menu, ID_HACKS_DISABLEPIXELSHADERS, chk_flag);

			chk_flag = (g_Settings->m_hacks.UseAllCores) ? MF_CHECKED : MF_UNCHECKED;
			CheckMenuItem(settings_menu, ID_HACKS_RUNXBOXTHREADSONALLCORES, chk_flag);

			chk_flag = (g_Settings->m_hacks.SkipRdtscPatching) ? MF_CHECKED : MF_UNCHECKED;
			CheckMenuItem(settings_menu, ID_HACKS_SKIPRDTSCPATCHING, chk_flag);

			switch (g_Settings->m_gui.DataStorageToggle) {
				case CXBX_DATA_APPDATA:
					CheckMenuItem(settings_menu, ID_SETTINGS_CONFIG_DLOCAPPDATA, MF_CHECKED);
					CheckMenuItem(settings_menu, ID_SETTINGS_CONFIG_DLOCEXECDIR, MF_UNCHECKED);
					CheckMenuItem(settings_menu, ID_SETTINGS_CONFIG_DLOCCUSTOM, MF_UNCHECKED);
					break;

				case CXBX_DATA_EXECDIR:
					CheckMenuItem(settings_menu, ID_SETTINGS_CONFIG_DLOCAPPDATA, MF_UNCHECKED);
					CheckMenuItem(settings_menu, ID_SETTINGS_CONFIG_DLOCEXECDIR, MF_CHECKED);
					CheckMenuItem(settings_menu, ID_SETTINGS_CONFIG_DLOCCUSTOM, MF_UNCHECKED);
					break;

				case CXBX_DATA_CUSTOM:
					CheckMenuItem(settings_menu, ID_SETTINGS_CONFIG_DLOCAPPDATA, MF_UNCHECKED);
					CheckMenuItem(settings_menu, ID_SETTINGS_CONFIG_DLOCEXECDIR, MF_UNCHECKED);
					CheckMenuItem(settings_menu, ID_SETTINGS_CONFIG_DLOCCUSTOM, MF_CHECKED);
					break;
			}

			chk_flag = (g_Settings->m_gui.bIgnoreInvalidXbeSig) ? MF_CHECKED : MF_UNCHECKED;
			CheckMenuItem(settings_menu, ID_SETTINGS_IGNOREINVALIDXBESIG, chk_flag);

			chk_flag = (g_Settings->m_gui.bIgnoreInvalidXbeSec) ? MF_CHECKED : MF_UNCHECKED;
			CheckMenuItem(settings_menu, ID_SETTINGS_IGNOREINVALIDXBESEC, chk_flag);

			chk_flag = (g_Settings->m_core.allowAdminPrivilege) ? MF_CHECKED : MF_UNCHECKED;
			CheckMenuItem(settings_menu, ID_SETTINGS_ALLOWADMINPRIVILEGE, chk_flag);
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
    // NOTE: Must force draw menu bar since sometime status doesn't show the new change.
    DrawMenuBar(m_hwnd);
}

// update debug consoles
void WndMain::UpdateDebugConsoles()
{
#ifdef _WINDOWS_
	HANDLE stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
	switch (g_Settings->m_gui.CxbxDebugMode) {
		case DM_CONSOLE:
			if (AllocConsole()) {
				std::freopen("CONOUT$", "wt", stdout);

				SetConsoleTitle("Cxbx-Reloaded : Debug Console");

				SetConsoleTextAttribute(stdHandle, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);

				std::printf("%s", "WndMain: Debug console allocated.\n");

				SetForegroundWindow(m_hwnd);
			}
			break;

		case DM_FILE:

			std::freopen(g_Settings->m_gui.szCxbxDebugFile.c_str(), "wt", stdout);
			FreeConsole();

			std::printf("%s", "WndMain: Debug console allocated.\n");
			break;

		default:

			if (GetConsoleWindow() != NULL) {
				std::fclose(stdout);
				FreeConsole();
			}
			std::freopen("nul", "w", stdout);

			break;
	}

	// NOTE: This is a Windows fix for ability to get std::cout to output onto console/file.
	// Not sure if linux/unix is affected too.
#ifdef _WINDOWS_
	if (stdHandle != INVALID_HANDLE_VALUE) {
		int fileDesc = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);

		if (fileDesc != -1) {
			FILE* file = _fdopen(fileDesc, "wt");

			if (file != nullptr) {
				int dup2Result = _dup2(_fileno(file), _fileno(stdout));
				if (dup2Result == 0) {
					std::setvbuf(stdout, nullptr, _IONBF, 0);
				}
				std::fclose(file);
			}
		}
	}
	std::wcout.clear();
	std::cout.clear();
#endif
}

// update recent files menu
void WndMain::UpdateRecentFiles()
{
    HMENU FileMenu = GetSubMenu(GetMenu(m_hwnd), 0);
    HMENU RXbeMenu = GetSubMenu(FileMenu, 4);

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
            sprintf(szBuffer, "&%d : %s", v, g_Settings->m_gui.szRecentXbeFiles[v].c_str());
            AppendMenu(RXbeMenu, MF_STRING, ID_FILE_RXBE_0 + v, szBuffer);
        }
    }
}

extern std::string FormatTitleId(uint32_t title_id);

void WndMain::UpdateCaption()
{
	char AsciiTitle[MAX_PATH];

	int i = sprintf(AsciiTitle, "Cxbx-Reloaded %s", CxbxrHashBuild);
	if (m_Xbe != nullptr) {
		i += sprintf(AsciiTitle + i, " : %s v1.%02d (%s)", FormatTitleId(m_Xbe->m_Certificate.dwTitleId).c_str(), m_Xbe->m_Certificate.dwVersion, m_Xbe->m_szAsciiTitle);

		UpdateFpsStatus();
		UpdateLogStatus();

	}

	SetWindowText(m_hwnd, AsciiTitle);
}

void WndMain::UpdateFpsStatus()
{
	// Append FPS menu text
	HMENU hMenu = GetMenu(m_hwnd);
	MENUITEMINFO mii;
	mii.cbSize = sizeof mii;
	mii.fMask = MIIM_STRING;
	char sMenu[32];
	mii.dwTypeData = &sMenu[0];

	if (m_bIsStarted) {
		if (g_EmuShared != nullptr) {
			g_EmuShared->GetCurrentFPS(&m_FPS_status);

			if (m_FPS_status == 0.0f) {
				m_MSpF_status = 0.0f;
			}
			else {
				m_MSpF_status = (float)(1000.0 / m_FPS_status);
			}
			std::sprintf(sMenu, "FPS: %.2f  MS / F : %.2f", m_FPS_status, m_MSpF_status);
		}
	}
	else {
		// Hide FPS if we're not currently emulating
		std::sprintf(sMenu, " ");
	}

	SetMenuItemInfo(hMenu, ID_FPS, FALSE, &mii);
}

void WndMain::UpdateLogStatus()
{
	// Append FPS menu text
	char sMenu[32];
	HMENU hMenu = GetMenu(m_hwnd);
	MENUITEMINFO mii;
	mii.cbSize = sizeof mii;
	mii.fMask = MIIM_STRING;
	mii.dwTypeData = &sMenu[0];

	std::strcpy(sMenu, "LOG:");

	if (g_Settings->m_gui.CxbxDebugMode != DebugMode::DM_NONE) {
		std::strcat(sMenu, "G");
	}

	if (m_bIsStarted && m_LogKrnl_status) {
		std::strcat(sMenu, "K");
	}

	SetMenuItemInfo(hMenu, ID_LOG, FALSE, &mii);
}

void WndMain::RefreshAllStatus()
{
	UpdateFpsStatus();
	UpdateLogStatus();
	DrawMenuBar(m_hwnd);
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

		RedrawWindow(m_hwnd, nullptr, NULL, RDW_INVALIDATE);

        PopupError(m_hwnd, ErrorMessage.c_str());

        UpdateCaption();

        return;
    }

	std::string errorMsg;
	
	if (!g_Settings->m_gui.bIgnoreInvalidXbeSig && !m_Xbe->CheckSignature()) {
		errorMsg += "- XBE signature check failed!\n";
	}

	if (!g_Settings->m_gui.bIgnoreInvalidXbeSec) {
		for (uint32_t sectionIndex = 0; sectionIndex < m_Xbe->m_Header.dwSections; sectionIndex++) {
			if (!m_Xbe->CheckSectionIntegrity(sectionIndex)) {
				errorMsg += "- One or more XBE section(s) are corrupted!\n";

				// if we find a corrupted section, we won't bother checking the remaining sections since we know
				// already at this point that the xbe is invalid
				break;
			}
		}
	}

	if (!errorMsg.empty()) {
		errorMsg += ("\nThis is dangerous, as maliciously modified Xbox applications could take control of your system."
			"\nPlease do not report issues for this application.\n"
			"\nAre you sure you want to continue?");

		PopupReturn ret = PopupWarningEx(m_hwnd, PopupButtons::YesNo, PopupReturn::No, errorMsg.c_str());
		if (ret != PopupReturn::Yes)
		{
			delete m_Xbe; m_Xbe = nullptr;

			RedrawWindow(m_hwnd, nullptr, NULL, RDW_INVALIDATE);

			UpdateCaption();

			return;
		}
	}

    // save this xbe to the list of recent xbe files
    if(m_XbeFilename[0] != '\0') {
        bool found = false;

        // if this filename already exists, temporarily remove it
        for(int c=0, r=0;c<m_dwRecentXbe;c++, r++) {
            if(strcmp(g_Settings->m_gui.szRecentXbeFiles[c].c_str(), m_XbeFilename) == 0) {
                found = true;
                r++;
            }

            if(r != c) {
                if(g_Settings->m_gui.szRecentXbeFiles[r].c_str() == 0 || r > m_dwRecentXbe - 1) {
                    g_Settings->m_gui.szRecentXbeFiles[c] = "";
                }
                else {
                    g_Settings->m_gui.szRecentXbeFiles[c] = g_Settings->m_gui.szRecentXbeFiles[r];
                }
            }
        }

        if (found) {
            m_dwRecentXbe--;
        }

        // move all items down one, removing the last one if necessary
        for (int v = RECENT_XBE_LIST_MAX - 1;v > 0; v--) {

            if(g_Settings->m_gui.szRecentXbeFiles[v-1].size() == 0) {
                g_Settings->m_gui.szRecentXbeFiles[v] = "";
            }
            else {
                g_Settings->m_gui.szRecentXbeFiles[v] = g_Settings->m_gui.szRecentXbeFiles[v - 1];
            }
        }

        // add new item as first index
        g_Settings->m_gui.szRecentXbeFiles[0] = m_XbeFilename;

        if (m_dwRecentXbe < RECENT_XBE_LIST_MAX) {
            m_dwRecentXbe++;
        }
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
        PopupReturn ret = PopupQuestion(m_hwnd, "Changes have been made, do you wish to save?");

        if(ret == PopupReturn::Yes)
            SaveXbeAs();
        else if(ret == PopupReturn::Cancel)
            return;
    }

    printf("WndMain: %s unloaded.\n", m_Xbe->m_szAsciiTitle);

    m_bXbeChanged = false;

    delete m_Xbe; m_Xbe = nullptr;

    UpdateCaption();
    RefreshMenus();

    DebuggerMonitorClose();

    // clear logo bitmap
    {
        uint32_t v=0;
        for(uint32_t y=0;y<17;y++)
        {
            for(uint32_t x=0;x<100;x++)
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

    RedrawWindow(m_hwnd, nullptr, NULL, RDW_INVALIDATE);
}

void WndMain::OpenMRU(int mru)
{
	HMENU menu = GetMenu(m_hwnd);
	HMENU file_menu = GetSubMenu(menu, 0);
	HMENU rxbe_menu = GetSubMenu(file_menu, 4);

	char szBuffer[270];

	GetMenuString(rxbe_menu, ID_FILE_RXBE_0 + mru, szBuffer, 269, MF_BYCOMMAND);

	char *szFilename = (char*)((uint32_t)szBuffer + 5); // +5 skips over "&%d : " prefix (see UpdateRecentFiles)

	OpenXbe(szFilename);
}

// Open the dashboard xbe if found
void WndMain::OpenDashboard()
{
	std::string DashboardPath = g_Settings->GetDataLocation() + std::string("\\EmuDisk\\Partition2\\xboxdash.xbe");
	OpenXbe(DashboardPath.c_str());
}

// save xbe file
void WndMain::SaveXbe(const char *x_filename)
{
    // ask permission to overwrite if the file already exists
    if(_access(x_filename, 0) != -1)
    {
        if(PopupQuestionEx(m_hwnd, LOG_LEVEL::INFO, PopupButtons::YesNo, PopupReturn::No, "Overwrite existing file?") != PopupReturn::Yes)
            return;
    }

    // export xbe file
    {
        m_Xbe->Export(x_filename);

        if(m_Xbe->HasError())
            PopupError(m_hwnd, m_Xbe->GetError().c_str());
        else
        {
            char buffer[255];

            sprintf(buffer, "%s was successfully saved.", m_Xbe->m_szAsciiTitle);

            PopupInfo(m_hwnd, buffer);

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
    ofn.lpstrFileTitle  = nullptr;
    ofn.nMaxFileTitle   = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.lpstrDefExt     = "xbe";
    ofn.Flags           = OFN_PATHMUSTEXIST;

    if(GetSaveFileName(&ofn) == TRUE)
        SaveXbe(ofn.lpstrFile);
}

// Only grant access to GUI end.
namespace cli_config {
extern void SetValue(const std::string key, const std::string value);
extern void SetValue(const std::string key, const char* value);
extern void SetValue(const std::string key, const void* value);
extern void SetValue(const std::string key, int value);
}
// start emulation
void WndMain::StartEmulation(HWND hwndParent, DebuggerState LocalDebuggerState /*= debuggerOff*/)
{
    bool isEmulating = false;

    g_EmuShared->GetIsEmulating(&isEmulating);

    if (isEmulating) {
        PopupError(m_hwnd, "A title is currently emulating, please stop emulation before attempting to start again.");
        return;
    }

	// Reset to default
	g_EmuShared->Reset();
	m_FPS_status = 0.0f;
	m_MSpF_status = 0.0f;
	m_FlagsLLE_status = g_Settings->m_core.FlagsLLE;
	m_LogKrnl_status = g_Settings->m_core.KrnlDebugMode != DebugMode::DM_NONE;

	// register all emulator settings to kernel process
	g_Settings->SyncToEmulator();

	// Preserve previous GUI window location.
	HWND hOwner = GetParent(m_hwnd);
	RECT curWindowPos;
	GetWindowRect((hOwner != nullptr ? hOwner : m_hwnd), &curWindowPos);
	m_prevWindowLoc.x = curWindowPos.left;
	m_prevWindowLoc.y = curWindowPos.top;
	ScreenToClient((hOwner != nullptr ? hOwner : m_hwnd), &m_prevWindowLoc);
	m_prevWindowLoc.x = curWindowPos.left - m_prevWindowLoc.x;
	m_prevWindowLoc.y = curWindowPos.top - m_prevWindowLoc.y;

	// Set the window size to emulation dimensions (The configured 'display' resolution)
	// Note : Doing this here assures the emulation process will use
	// the configured dimensions (because if done inside the emulation
	// process, that doesn't resize correctly sometimes)
	// We always resize, as we no longer tie host resolution to Xbox resolution
	// 'Higher Resolution' rendering is handled as a scale factor.
	ResizeWindow(m_hwnd, /*bForGUI*/false);

	// shell exe
    {

		char szExeFileName[MAX_PATH];
		GetModuleFileName(GetModuleHandle(nullptr), szExeFileName, MAX_PATH);
		if (g_Settings->m_core.bUseLoaderExec) {
			PathRemoveFileSpec(szExeFileName);
			PathAppend(szExeFileName, "\\cxbxr-ldr.exe");
		}

		bool AttachLocalDebugger = (LocalDebuggerState == debuggerOn);
		g_EmuShared->SetDebuggingFlag(AttachLocalDebugger);

        /* Main process to generate emulation command line begin. */
        // If we are adding more arguments, this is the place to do so.
        cli_config::SetValue(cli_config::exec, szExeFileName);
        cli_config::SetLoad(m_XbeFilename);
        cli_config::SetValue(cli_config::hwnd, hwndParent);
        cli_config::SetValue(cli_config::debug_mode, g_Settings->m_core.KrnlDebugMode);
        if (g_Settings->m_core.KrnlDebugMode == DM_FILE) {
            cli_config::SetValue(cli_config::debug_file, g_Settings->m_core.szKrnlDebug);
        }
        else {
            cli_config::SetValue(cli_config::debug_file, "");
        }
        /* Main process to generate emulation command line end. */

        if (AttachLocalDebugger) {

            // Check then close existing debugger monitor.
            DebuggerMonitorClose();

            if (CxbxExec(true, &m_hDebuggerProc, true)) {
                PopupError(m_hwnd, "Failed to start emulation with the debugger.\n\nYou will need to build CxbxDebugger manually.");

                printf("WndMain: %s debugger shell failed.\n", m_Xbe->m_szAsciiTitle);
            }
            else {
                m_bIsStarted = true;
                printf("WndMain: %s emulation started with debugger.\n", m_Xbe->m_szAsciiTitle);
                m_hDebuggerMonitorThread = std::thread(DebuggerMonitor, this); // create the debugger monitoring thread
            }
        }
        else {

            if (const auto &err = CxbxExec(false, nullptr, false)) {
                PopupError(m_hwnd, err->c_str());

                printf("WndMain: %s shell failed.\n", m_Xbe->m_szAsciiTitle);
            }
            else {
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

	g_EmuShared->SetIsEmulating(false);
}

// wrapper function to call CrashMonitor
DWORD WndMain::CrashMonitorWrapper(LPVOID lpParam)
{
	CxbxSetThreadName("Cxbx Crash Monitor");

	Crash_Manager_Data* pCMD = (Crash_Manager_Data*)lpParam;
	static_cast<WndMain*>(pCMD->pWndMain)->CrashMonitor(pCMD->dwChildProcID);
	free(lpParam);

	return 0;
}

// monitor for crashes
void WndMain::CrashMonitor(DWORD dwChildProcID)
{
	int iBootFlags;
	DWORD dwExitCode = 0;

	// If we do receive valid process ID, let's do the next step.
	if (dwChildProcID != 0) {

	HANDLE hProcess = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION, FALSE, dwChildProcID);

	 	// If we do receive valid handle, let's do the next step.
	 	if (hProcess != NULL) {

	 		WaitForSingleObject(hProcess, INFINITE);

	 		GetExitCodeProcess(hProcess, &dwExitCode);
	 		CloseHandle(hProcess);

	 		g_EmuShared->GetBootFlags(&iBootFlags);

	 		if (!iBootFlags) {
	 			if (dwExitCode == EXIT_SUCCESS) {// StopEmulation
	 				return;
	 			}
				// Or else, it's a crash
	 		}
	 		else {

	 			// multi-xbe
	 			// destroy this thread and start a new one
	 			return;
	 		}
	 	}
	}

	// Crash clean up.

	KillTimer(m_hwnd, TIMERID_ACTIVE_EMULATION);
	KillTimer(m_hwnd, TIMERID_LED);
	m_hwndChild = NULL;
	m_bIsStarted = false;
	g_EmuShared->SetIsEmulating(false);
	UpdateCaption();
	RefreshMenus();
	DrawLedBitmap(m_hwnd, true);
}

// monitor for Debugger to close then set as "available" (For limit to 1 debugger per Cxbx GUI.)
DWORD WndMain::DebuggerMonitor(LPVOID lpVoid)
{
	CxbxSetThreadName("Cxbx Debugger Monitor");
	WndMain* pThis = static_cast<WndMain*>(lpVoid);

	if (pThis->m_hDebuggerProc != nullptr) {

		// Peform a wait until Debugger is closed.
		WaitForSingleObject(pThis->m_hDebuggerProc, INFINITE);

		if (pThis->m_hDebuggerProc != nullptr) {
			CloseHandle(pThis->m_hDebuggerProc);
			pThis->m_hDebuggerProc = nullptr;
		}
	}

	if (pThis->m_hDebuggerMonitorThread.joinable()) {
		pThis->m_hDebuggerMonitorThread.detach();
	}

	return 0;
}
void WndMain::DebuggerMonitorClose()
{
	if (m_hDebuggerProc != nullptr) {
		HANDLE hDebuggerProcTemp = m_hDebuggerProc;
		std::thread hDebuggerMonitorThreadTemp = std::thread(std::move(m_hDebuggerMonitorThread));

		// Set member to null pointer before terminate, this way debugger monitor thread will remain thread-safe.
		m_hDebuggerProc = nullptr;


		TerminateProcess(hDebuggerProcTemp, EXIT_SUCCESS);
		CloseHandle(hDebuggerProcTemp);

		hDebuggerMonitorThreadTemp.join();
	}
}

// draw Xbox LED bitmap
void WndMain::DrawLedBitmap(HWND hwnd, bool bdefault)
{
	HMENU hMenu = GetMenu(hwnd);
	int ActiveLEDColor;

	MENUITEMINFO mii = { 0 };
	mii.cbSize = sizeof(mii);
	mii.fMask = MIIM_FTYPE | MIIM_BITMAP | MIIM_STRING;
	char flagString[10] = "LLE-";
	mii.dwTypeData = &flagString[0];
	mii.fType = MFT_RIGHTJUSTIFY;
	mii.hbmpItem = m_LedBmp;

	// When so requested, or when not emulating, draw a black bitmap and hide LLE flags string
	if (bdefault || !m_bIsStarted) {
		ActiveLEDColor = XBOX_LED_COLOUR_OFF;
		sprintf(flagString, " ");
	}
	else { // draw colored bitmap
		static int LedSequenceOffset = 0;
		unsigned int FlagsLLE = 0;

		// Select active color and cycle through all 4 phases in the sequence
		ActiveLEDColor = m_LedSeq_status[LedSequenceOffset & 3];
		++LedSequenceOffset;

		// Set LLE flags string based on selected LLE flags
		if (m_FlagsLLE_status & LLE_APU) {
			strcat(flagString, "A");
		}
		if (m_FlagsLLE_status & LLE_GPU) {
			strcat(flagString, "G");
		}
		if (m_FlagsLLE_status & LLE_USB) {
			strcat(flagString, "U");
		}
		if (m_FlagsLLE_status & LLE_JIT) {
			strcat(flagString, "J");
		}
		if (m_FlagsLLE_status == 0) {
			sprintf(flagString, "HLE");
		}
	}

	SelectObject(m_LedDC, m_Brushes[ActiveLEDColor]);
	SelectObject(m_LedDC, m_Pens[ActiveLEDColor]);

	m_OriLed = (HBITMAP)SelectObject(m_LedDC, m_LedBmp);
	Rectangle(m_LedDC, 0, 0, m_xBmp, m_yBmp);
	m_LedBmp = (HBITMAP)SelectObject(m_LedDC, m_OriLed);

	SetMenuItemInfo(hMenu, ID_LED, FALSE, &mii);

	DrawMenuBar(hwnd);

	return;
}
