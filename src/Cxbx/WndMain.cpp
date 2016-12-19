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
#include "WndAbout.h"
#include "DlgControllerConfig.h"
#include "DlgVideoConfig.h"
#include "CxbxKrnl/EmuShared.h"
#include "ResCxbx.h"
#include "EmuExe.h"
#include "jpegdec/jpegdec.h"

#include <io.h>

/**
 * Silly little hack to fix link error with libjpeg on MSVC 2015
 */
FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE * __cdecl __iob_func(void) { return _iob; }

WndMain::WndMain(HINSTANCE x_hInstance) : Wnd(x_hInstance), m_bCreated(false), m_Xbe(0), m_Exe(0), m_bExeChanged(false), m_bXbeChanged(false), m_bCanStart(true), m_hwndChild(NULL), m_AutoConvertToExe(AUTO_CONVERT_WINDOWS_TEMP), m_KrnlDebug(DM_NONE), m_CxbxDebug(DM_NONE), m_dwRecentXbe(0), m_dwRecentExe(0)
{
    // initialize members
    {
        m_classname = "WndMain";
        m_wndname   = "Cxbx-Reloaded " _CXBX_VERSION;

        m_w         = 640;
        m_h         = 480;

        m_ExeFilename = (char*)calloc(1, MAX_PATH);
        m_XbeFilename = (char*)calloc(1, MAX_PATH);

        m_CxbxDebugFilename = (char*)calloc(1, MAX_PATH);
        m_KrnlDebugFilename = (char*)calloc(1, MAX_PATH);

        int v=0;

        for(v=0;v<RECENT_XBE_SIZE;v++)
            m_szRecentXbe[v] = 0;

        for(v=0;v<RECENT_EXE_SIZE;v++)
            m_szRecentExe[v] = 0;
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
            RegQueryValueEx(hKey, "CxbxDebug", NULL, &dwType, (PBYTE)&m_CxbxDebug, &dwSize);

            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            RegQueryValueEx(hKey, "KrnlDebug", NULL, &dwType, (PBYTE)&m_KrnlDebug, &dwSize);

            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            RegQueryValueEx(hKey, "RecentXbe", NULL, &dwType, (PBYTE)&m_dwRecentXbe, &dwSize);

            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            RegQueryValueEx(hKey, "RecentExe", NULL, &dwType, (PBYTE)&m_dwRecentExe, &dwSize);

            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            RegQueryValueEx(hKey, "AutoConvertToExe", NULL, &dwType, (PBYTE)&m_AutoConvertToExe, &dwSize);

            dwType = REG_SZ; dwSize = MAX_PATH;
            RegQueryValueEx(hKey, "CxbxDebugFilename", NULL, &dwType, (PBYTE)m_CxbxDebugFilename, &dwSize);

            dwType = REG_SZ; dwSize = MAX_PATH;
            RegQueryValueEx(hKey, "KrnlDebugFilename", NULL, &dwType, (PBYTE)m_KrnlDebugFilename, &dwSize);

            int v=0;

            for(v=0;v<m_dwRecentXbe;v++)
            {
                char buffer[32];

                sprintf(buffer, "RecentXbe%d", v);

                m_szRecentXbe[v] = (char*)calloc(1, MAX_PATH);

                dwType = REG_SZ; dwSize = MAX_PATH;
                RegQueryValueEx(hKey, buffer, NULL, &dwType, (PBYTE)m_szRecentXbe[v], &dwSize);
            }

            for(v=0;v<m_dwRecentExe;v++)
            {
                char buffer[32];

                sprintf(buffer, "RecentExe%d", v);

                m_szRecentExe[v] = (char*)calloc(1, MAX_PATH);

                dwType = REG_SZ; dwSize = MAX_PATH;
                RegQueryValueEx(hKey, buffer, NULL, &dwType, (PBYTE)m_szRecentExe[v], &dwSize);
            }

            RegCloseKey(hKey);
        }
    }

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

            for(v=0;v<m_dwRecentExe;v++)
            {
                char buffer[32];

                sprintf(buffer, "RecentExe%d", v);

                dwType = REG_SZ; dwSize = MAX_PATH;

                RegSetValueEx(hKey, buffer, 0, dwType, (PBYTE)m_szRecentExe[v], dwSize);

                free(m_szRecentExe[v]);
            }

            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            RegSetValueEx(hKey, "CxbxDebug", 0, dwType, (PBYTE)&m_CxbxDebug, dwSize);

            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            RegSetValueEx(hKey, "KrnlDebug", 0, dwType, (PBYTE)&m_KrnlDebug, dwSize);

            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            RegSetValueEx(hKey, "RecentXbe", 0, dwType, (PBYTE)&m_dwRecentXbe, dwSize);

            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            RegSetValueEx(hKey, "RecentExe", 0, dwType, (PBYTE)&m_dwRecentExe, dwSize);

            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            RegSetValueEx(hKey, "AutoConvertToExe", 0, dwType, (PBYTE)&m_AutoConvertToExe, dwSize);

            dwType = REG_SZ; dwSize = MAX_PATH;
            RegSetValueEx(hKey, "CxbxDebugFilename", 0, dwType, (PBYTE)m_CxbxDebugFilename, dwSize);

            dwType = REG_SZ; dwSize = MAX_PATH;
            RegSetValueEx(hKey, "KrnlDebugFilename", 0, dwType, (PBYTE)m_KrnlDebugFilename, dwSize);
        }
    }

    // cleanup allocations
    {
        delete m_Xbe;
        delete m_Exe;

        free(m_XbeFilename);
        free(m_ExeFilename);

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

                MoveWindow(hwnd, wRect.left, wRect.top, difW + 640, difH + 480, TRUE);
            }

            // initialize back buffer
            {
                HDC hDC = GetDC(hwnd);

                m_BackBmp = CreateCompatibleBitmap(hDC, 640, 480);

                // decompress jpeg, convert to bitmap resource
                {
                    HRSRC hSrc = FindResource(NULL, MAKEINTRESOURCE(IDR_JPEG_SPLASH), "JPEG");
                    HGLOBAL hRes = LoadResource(NULL, hSrc);
                    uint08 *jpgData = (uint08*)LockResource(hRes);

                    uint32 jpgFileSize = SizeofResource(NULL, hSrc);
                    uint32 bmpFileSize = 0;
                    uint32 bmpWidth, bmpHeight;

                    uint08 *bmpBuff = jpeg2bmp(jpgData, jpgFileSize, &bmpFileSize, &bmpWidth, &bmpHeight);

                    // create bitmap
                    {
                        BITMAPINFO BmpInfo;

                        BmpInfo.bmiHeader.biSize          = sizeof(BITMAPINFO) - sizeof(RGBQUAD);
                        BmpInfo.bmiHeader.biWidth         = bmpWidth;
                        BmpInfo.bmiHeader.biHeight        = 0 - (int)bmpHeight;
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

                    free(bmpBuff);

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

                    char AsciiTitle[255];

                    sprintf(AsciiTitle, "Cxbx-Reloaded : Emulating %s...", m_Xbe->m_szAsciiTitle);

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

                BitBlt(hDC, 0, 0, 640, 480, m_BackDC, 0, 0, SRCCOPY);
//              BitBlt(hDC, 0, 10, 320, 160, m_BackDC, 0, 0, SRCCOPY);
                BitBlt(hDC, 640-nLogoBmpW-4, 480-nLogoBmpH-4, nLogoBmpW, nLogoBmpH, m_LogoDC, 0, 0, SRCCOPY);

                int nHeight = -MulDiv(8, GetDeviceCaps(hDC, LOGPIXELSY), 72);

                HFONT hFont = CreateFont(nHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_ROMAN, "Verdana");

                HGDIOBJ tmpObj = SelectObject(hDC, hFont);

                SetBkColor(hDC, RGB(0,0,0));

                SetTextColor(hDC, RGB(255,255,255));

                char buffer[255];

                if(m_Xbe != 0 && m_Xbe->GetError() == 0)
                    sprintf(buffer, "%s Loaded!", m_Xbe->m_szAsciiTitle);
                else
                    sprintf(buffer, "%s", "Disclaimer: Cxbx-Reloaded has no affiliation with Microsoft");

                RECT rect = {0, 480-15-5, 640-100-4-69, 480-5};

                ExtTextOut(hDC, 4, 480-15-5, ETO_OPAQUE, &rect, buffer, strlen(buffer), 0);

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
                        StartEmulation(m_AutoConvertToExe, hwnd);
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

        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case ID_FILE_OPEN_XBE:
                {
                    OPENFILENAME ofn = {0};

                    char filename[MAX_PATH] = {0};

                    ofn.lStructSize     = sizeof(OPENFILENAME);
                    ofn.hwndOwner       = m_hwnd;
                    ofn.lpstrFilter     = "Xbox Executables (*.xbe)\0*.xbe\0";
                    ofn.lpstrFile       = filename;
                    ofn.nMaxFile        = MAX_PATH;
                    ofn.nFilterIndex    = 1;
                    ofn.lpstrFileTitle  = NULL;
                    ofn.nMaxFileTitle   = 0;
                    ofn.lpstrInitialDir = NULL;
                    ofn.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                    if(GetOpenFileName(&ofn) == TRUE)
                    {
                        if(m_Xbe != 0)
                            CloseXbe();

                        if(m_Xbe != 0)
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
                    if(m_XbeFilename[0] == '\0')
                        SaveXbeAs();
                    else
                        SaveXbe(m_XbeFilename);
                }
                break;

                case ID_FILE_SAVEXBEFILEAS:
                    SaveXbeAs();
                    break;

                case ID_FILE_IMPORTFROMEXE:
                {
                    m_ExeFilename[0] = '\0';

                    OPENFILENAME ofn = {0};

                    char filename[MAX_PATH] = {0};

                    ofn.lStructSize     = sizeof(OPENFILENAME);
                    ofn.hwndOwner       = m_hwnd;
                    ofn.lpstrFilter     = "Windows Executables (*.exe)\0*.exe\0";
                    ofn.lpstrFile       = filename;
                    ofn.nMaxFile        = MAX_PATH;
                    ofn.nFilterIndex    = 1;
                    ofn.lpstrFileTitle  = NULL;
                    ofn.nMaxFileTitle   = 0;
                    ofn.lpstrInitialDir = NULL;
                    ofn.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                    if(GetOpenFileName(&ofn) == TRUE)
                    {
                        if(m_Xbe != 0)
                            CloseXbe();

                        if(m_Xbe != 0)
                            break;

                        ImportExe(ofn.lpstrFile);
                    }
                }
                break;

                case ID_FILE_EXPORTTOEXE:
                    ConvertToExe(NULL, true, hwnd);
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
                    if(m_Xbe != 0)
                        CloseXbe();

                    if(m_Xbe != 0)
                        break;

                    HMENU menu = GetMenu(m_hwnd);
                    HMENU file_menu = GetSubMenu(menu, 0);
                    HMENU rxbe_menu = GetSubMenu(file_menu, 9);

                    char szBuffer[270];

                    GetMenuString(rxbe_menu, LOWORD(wParam), szBuffer, 269, MF_BYCOMMAND);

                    char *szFilename = (char*)((uint32)szBuffer + 5);

                    OpenXbe(szFilename);
                }
                break;

                case ID_FILE_REXE_0:
                case ID_FILE_REXE_1:
                case ID_FILE_REXE_2:
                case ID_FILE_REXE_3:
                case ID_FILE_REXE_4:
                case ID_FILE_REXE_5:
                case ID_FILE_REXE_6:
                case ID_FILE_REXE_7:
                case ID_FILE_REXE_8:
                case ID_FILE_REXE_9:
                {
                    if(m_Xbe != 0)
                        CloseXbe();

                    if(m_Xbe != 0)
                        break;

                    HMENU menu = GetMenu(m_hwnd);
                    HMENU file_menu = GetSubMenu(menu, 0);
                    HMENU rexe_menu = GetSubMenu(file_menu, 10);

                    char szBuffer[270];

                    GetMenuString(rexe_menu, LOWORD(wParam), szBuffer, 269, MF_BYCOMMAND);

                    char *szFilename = (char*)((uint32)szBuffer + 5);

                    ImportExe(szFilename);
                }
                break;

                case ID_FILE_EXIT:
                    SendMessage(hwnd, WM_CLOSE, 0, 0);
                    break;

                case ID_EDIT_LOGOBITMAP_EXPORT:
                {
                    OPENFILENAME ofn = {0};

                    char filename[MAX_PATH] = "logo.bmp";

                    ofn.lStructSize     = sizeof(OPENFILENAME);
                    ofn.hwndOwner       = m_hwnd;
                    ofn.lpstrFilter     = "Bitmap Image Files (*.bmp)\0*.bmp\0";
                    ofn.lpstrFile       = filename;
                    ofn.nMaxFile        = MAX_PATH;
                    ofn.nFilterIndex    = 1;
                    ofn.lpstrFileTitle  = NULL;
                    ofn.nMaxFileTitle   = 0;
                    ofn.lpstrInitialDir = NULL;
                    ofn.lpstrDefExt     = "bmp";
                    ofn.lpstrTitle      = "Export Logo Bitmap";
                    ofn.Flags           = OFN_PATHMUSTEXIST;

                    if(GetSaveFileName(&ofn) == TRUE)
                    {
                        // ask permission to overwrite if file already exists
                        if(_access(ofn.lpstrFile, 0) != -1)
                        {
                            if(MessageBox(m_hwnd, "Overwrite existing file?", "Cxbx-Reloaded", MB_ICONQUESTION | MB_YESNO) != IDYES)
                                return TRUE;
                        }

                        // export logo bitmap
                        {
                            uint08 i_gray[100*17];

                            m_Xbe->ExportLogoBitmap(i_gray);

                            if(m_Xbe->GetError() == 0)
                            {
                                FILE *LogoBitmap = fopen(ofn.lpstrFile, "wb");

                                // write bitmap header
                                {
                                    BITMAPFILEHEADER    bmfh;

                                    bmfh.bfType         = *(uint16*)"BM";
                                    bmfh.bfSize         = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) - sizeof(RGBQUAD) + (100*17)*sizeof(RGBTRIPLE) + 2;
                                    bmfh.bfReserved1    = 0;
                                    bmfh.bfReserved2    = 0;
                                    bmfh.bfOffBits      = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) - sizeof(RGBQUAD);

                                    fwrite(&bmfh, sizeof(bmfh), 1, LogoBitmap);
                                }

                                // write bitmap info
                                {
                                    BITMAPINFO          bmi;

                                    bmi.bmiHeader.biSize          = sizeof(BITMAPINFO) - sizeof(RGBQUAD);
                                    bmi.bmiHeader.biWidth         = 100;
                                    bmi.bmiHeader.biHeight        = -17;
                                    bmi.bmiHeader.biPlanes        = 1;
                                    bmi.bmiHeader.biBitCount      = 24;
                                    bmi.bmiHeader.biCompression   = BI_RGB;
                                    bmi.bmiHeader.biSizeImage     = 0;
                                    bmi.bmiHeader.biXPelsPerMeter = 0;
                                    bmi.bmiHeader.biYPelsPerMeter = 0;
                                    bmi.bmiHeader.biClrUsed       = 0;
                                    bmi.bmiHeader.biClrImportant  = 0;

                                    fwrite(&bmi, sizeof(bmi) - 4, 1, LogoBitmap);
                                }

                                // write bitmap data
                                {
                                    RGBTRIPLE bmp_data[100*17];

                                    for(uint32 v=0;v<100*17;v++)
                                    {
                                        bmp_data[v].rgbtRed   = i_gray[v];
                                        bmp_data[v].rgbtGreen = i_gray[v];
                                        bmp_data[v].rgbtBlue  = i_gray[v];
                                    }

                                    fwrite(bmp_data, 100*17*sizeof(RGBTRIPLE), 1, LogoBitmap);
                                }

                                // write bitmap padding
                                {
                                    uint16 pad = 0;

                                    fwrite(&pad, 2, 1, LogoBitmap);
                                }

                                fclose(LogoBitmap);
                            }

                            if(m_Xbe->GetError() != 0)
                                MessageBox(m_hwnd, m_Xbe->GetError(), "Cxbx-Reloaded", MB_ICONSTOP | MB_OK);
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
                    OPENFILENAME ofn = {0};

                    char filename[MAX_PATH] = "*.bmp";

                    ofn.lStructSize     = sizeof(OPENFILENAME);
                    ofn.hwndOwner       = m_hwnd;
                    ofn.lpstrFilter     = "Bitmap Image Files (*.bmp)\0*.bmp\0";
                    ofn.lpstrFile       = filename;
                    ofn.nMaxFile        = MAX_PATH;
                    ofn.nFilterIndex    = 1;
                    ofn.lpstrFileTitle  = NULL;
                    ofn.nMaxFileTitle   = 0;
                    ofn.lpstrInitialDir = NULL;
                    ofn.lpstrDefExt     = "bmp";
                    ofn.lpstrTitle      = "Import Logo Bitmap";
                    ofn.Flags           = OFN_PATHMUSTEXIST;

                    if(GetOpenFileName(&ofn) == TRUE)
                    {
                        // import logo bitmap
                        {
                            uint08 i_gray[100*17];

                            // read bitmap file
                            {
                                FILE *logo = fopen(ofn.lpstrFile, "rb");

                                char *bmp_err = 0;

                                // read bitmap header
                                if(!bmp_err)
                                {
                                    BITMAPFILEHEADER bmfh;

                                    fread(&bmfh, sizeof(bmfh), 1, logo);

                                    if(bmfh.bfType != *(uint16*)"BM")
                                        bmp_err = "Invalid bitmap file...\n\nonly allows 24 bit bitmaps (100 by 17 pixels) with row order swapped";
                                    else if(bmfh.bfSize != sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) - sizeof(RGBQUAD) + (100*17)*sizeof(RGBTRIPLE) + 2)
                                        bmp_err = "Invalid bitmap file...\n\nonly allows 24 bit bitmaps (100 by 17 pixels) with row order swapped";
                                }

                                // read bitmap info
                                if(!bmp_err)
                                {
                                    BITMAPINFO bmi;

                                    fread(&bmi, sizeof(bmi) - 4, 1, logo);

                                    if(bmi.bmiHeader.biWidth != 100 || bmi.bmiHeader.biHeight != -17)
                                        bmp_err = "Invalid bitmap file...\n\nonly allows 24 bit bitmaps (100 by 17 pixels) with row order swapped";
                                }

                                // read bitmap data
                                if(!bmp_err)
                                {
                                    RGBTRIPLE bmp_data[100*17];

                                    fread(bmp_data, 100*17*sizeof(RGBTRIPLE), 1, logo);

                                    for(uint32 c=0;c<100*17;c++)
                                        i_gray[c] = (char)(((float)bmp_data[c].rgbtRed + (float)bmp_data[c].rgbtGreen + (float)bmp_data[c].rgbtBlue) / 3.0);
                                }

                                fclose(logo);

                                if(bmp_err != 0)
                                {
                                    MessageBox(m_hwnd, bmp_err, "Cxbx-Reloaded", MB_OK | MB_ICONEXCLAMATION);
                                    break;
                                }
                            }

                            m_Xbe->ImportLogoBitmap(i_gray);

                            if(m_Xbe->GetError() != 0)
                            {
                                MessageBox(m_hwnd, m_Xbe->GetError(), "Cxbx-Reloaded", MB_ICONSTOP | MB_OK);

                                if(m_Xbe->IsFatal())
                                    CloseXbe();
                                else
                                    m_Xbe->ClearError();
                            }
                            else
                            {
                                m_bExeChanged = true;
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
                    m_bExeChanged = true;
                    m_bXbeChanged = true;

                    m_Xbe->m_Header.dwInitFlags.bLimit64MB = !m_Xbe->m_Header.dwInitFlags.bLimit64MB;

                    RefreshMenus();

                    if(m_Xbe->m_Header.dwInitFlags.bLimit64MB)
                        printf("WndMain: %s was patched to limit to 64MB of memory usage.\n", m_Xbe->m_szAsciiTitle);
                    else
                        printf("WndMain: %s was patched to allow >64MB of memory usage.\n", m_Xbe->m_szAsciiTitle);
                }
                break;

                case ID_EDIT_PATCH_DEBUGMODE:
                {
                    m_bExeChanged = true;
                    m_bXbeChanged = true;

                    // patch to/from debug mode
                    if((m_Xbe->m_Header.dwEntryAddr ^ XOR_EP_RETAIL) > 0x01000000)
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

                    if(res)
                        printf("WndMain: %s was converted to debug mode.\n", m_Xbe->m_szAsciiTitle);
                    else
                        printf("WndMain: %s was converted to retail mode.\n", m_Xbe->m_szAsciiTitle);
                }
                break;

                case ID_EDIT_DUMPXBEINFOTO_FILE:
                {
                    OPENFILENAME ofn = {0};

                    char filename[MAX_PATH] = "Xbe.txt";

                    ofn.lStructSize     = sizeof(OPENFILENAME);
                    ofn.hwndOwner       = m_hwnd;
                    ofn.lpstrFilter     = "Text Documents (*.txt)\0*.txt\0";
                    ofn.lpstrFile       = filename;
                    ofn.nMaxFile        = MAX_PATH;
                    ofn.nFilterIndex    = 1;
                    ofn.lpstrFileTitle  = NULL;
                    ofn.nMaxFileTitle   = 0;
                    ofn.lpstrInitialDir = NULL;
                    ofn.lpstrDefExt     = "txt";
                    ofn.Flags           = OFN_PATHMUSTEXIST;

                    if(GetSaveFileName(&ofn) == TRUE)
                    {
                        // ask permission to overwrite if file exists
                        if(_access(ofn.lpstrFile, 0) != -1)
                        {
                            if(MessageBox(m_hwnd, "Overwrite existing file?", "Cxbx-Reloaded", MB_ICONQUESTION | MB_YESNO) != IDYES)
                                return TRUE;
                        }

                        // dump xbe information to file
                        {
                            FILE *TxtFile = fopen(ofn.lpstrFile, "wt");

                            // verify file was opened
                            if(TxtFile == 0)
                                MessageBox(m_hwnd, "Could not open text file.", "Cxbx-Reloaded", MB_ICONSTOP | MB_OK);
                            else
                            {
                                m_Xbe->DumpInformation(TxtFile);

                                fclose(TxtFile);

                                if(m_Xbe->GetError())
                                {
                                    MessageBox(m_hwnd, m_Xbe->GetError(), "Cxbx-Reloaded", MB_ICONSTOP | MB_OK);
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

                    if(m_Xbe->GetError())
                    {
                        MessageBox(m_hwnd, m_Xbe->GetError(), "Cxbx-Reloaded", MB_ICONSTOP | MB_OK);
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

                case ID_EMULATION_DEBUGOUTPUTKERNEL_CONSOLE:
                {
                    if(m_KrnlDebug == DM_NONE || m_KrnlDebug == DM_FILE)
                        m_KrnlDebug = DM_CONSOLE;
                    else
                        m_KrnlDebug = DM_NONE;

                    MessageBox(m_hwnd, "This will not take effect until the next time emulation is started.\n", "Cxbx-Reloaded", MB_OK);

                    m_bExeChanged = true;

                    RefreshMenus();

                    UpdateDebugConsoles();
                }
                break;

                case ID_EMULATION_DEBUGOUTPUTKERNEL_FILE:
                {
                    if(m_KrnlDebug == DM_FILE)
                    {
                        m_KrnlDebug = DM_NONE;

                        RefreshMenus();

                        UpdateDebugConsoles();
                    }
                    else
                    {
                        OPENFILENAME ofn = {0};

                        char filename[MAX_PATH] = "KrnlDebug.txt";

                        ofn.lStructSize     = sizeof(OPENFILENAME);
                        ofn.hwndOwner       = m_hwnd;
                        ofn.lpstrFilter     = "Text Documents (*.txt)\0*.txt\0";
                        ofn.lpstrFile       = filename;
                        ofn.nMaxFile        = MAX_PATH;
                        ofn.nFilterIndex    = 1;
                        ofn.lpstrFileTitle  = NULL;
                        ofn.nMaxFileTitle   = 0;
                        ofn.lpstrInitialDir = NULL;
                        ofn.lpstrDefExt     = "txt";
                        ofn.Flags           = OFN_PATHMUSTEXIST;

                        if(GetSaveFileName(&ofn) != FALSE)
                        {
                            MessageBox(m_hwnd, "This will not take effect until emulation is (re)started.\n", "Cxbx-Reloaded", MB_OK);

                            strncpy(m_KrnlDebugFilename, ofn.lpstrFile, MAX_PATH-1);

                            m_bExeChanged = true;

                            m_KrnlDebug = DM_FILE;

                            RefreshMenus();

                            UpdateDebugConsoles();
                        }
                    }
                }
                break;

                case ID_EMULATION_DEBUGOUTPUTGUI_CONSOLE:
                {
                    if(m_CxbxDebug == DM_NONE || m_CxbxDebug == DM_FILE)
                        m_CxbxDebug = DM_CONSOLE;
                    else
                        m_CxbxDebug = DM_NONE;

                    RefreshMenus();

                    UpdateDebugConsoles();
                }
                break;

                case ID_EMULATION_DEBUGOUTPUTGUI_FILE:
                {
                    if(m_CxbxDebug == DM_FILE)
                    {
                        m_CxbxDebug = DM_NONE;

                        RefreshMenus();

                        UpdateDebugConsoles();
                    }
                    else
                    {
                        OPENFILENAME ofn = {0};

                        char filename[MAX_PATH] = "CxbxDebug.txt";

                        ofn.lStructSize     = sizeof(OPENFILENAME);
                        ofn.hwndOwner       = m_hwnd;
                        ofn.lpstrFilter     = "Text Documents (*.txt)\0*.txt\0";
                        ofn.lpstrFile       = filename;
                        ofn.nMaxFile        = MAX_PATH;
                        ofn.nFilterIndex    = 1;
                        ofn.lpstrFileTitle  = NULL;
                        ofn.nMaxFileTitle   = 0;
                        ofn.lpstrInitialDir = NULL;
                        ofn.lpstrDefExt     = "txt";
                        ofn.Flags           = OFN_PATHMUSTEXIST;

                        if(GetSaveFileName(&ofn) != FALSE)
                        {
                            strncpy(m_CxbxDebugFilename, ofn.lpstrFile, MAX_PATH-1);

                            m_CxbxDebug = DM_FILE;

                            RefreshMenus();

                            UpdateDebugConsoles();
                        }

                    }
                }
                break;

                case ID_EMULATION_START:
                    StartEmulation(m_AutoConvertToExe, hwnd);
                    break;

                case ID_EMULATION_STOP:
                    StopEmulation();
                    break;

                case ID_SETTINGS_GENWT:
                {
                    m_AutoConvertToExe = AUTO_CONVERT_WINDOWS_TEMP;

                    RefreshMenus();
                }
                break;

                case ID_SETTINGS_GENXP:
                {
                    m_AutoConvertToExe = AUTO_CONVERT_XBE_PATH;

                    RefreshMenus();
                }
                break;

                case ID_SETTINGS_GENMA:
                {
                    m_AutoConvertToExe = AUTO_CONVERT_MANUAL;

                    RefreshMenus();
                }
                break;

                case ID_HELP_ABOUT:
                {
                    WndAbout *AboutWnd = new WndAbout(m_hInstance, m_hwnd);

                    while(AboutWnd->GetError() == 0 && AboutWnd->ProcessMessages())
                        Sleep(10);

                    if(AboutWnd->GetError() != 0)
                        MessageBox(m_hwnd, AboutWnd->GetError(), "Cxbx-Reloaded", MB_ICONSTOP | MB_OK);

                    delete AboutWnd;
                }
                break;

                case ID_HELP_HOMEPAGE:
                    ShellExecute(NULL, "open", "http://www.github.com/LukeUsher/Cxbx-Reloaded/", NULL, NULL, SW_SHOWNORMAL);
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

    if(m_Xbe->GetError() != 0)
    {
        MessageBox(m_hwnd, m_Xbe->GetError(), "Cxbx-Reloaded", MB_ICONEXCLAMATION | MB_OK);

        if(m_Xbe->IsFatal())
            CloseXbe();

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
    // disable/enable appropriate menus
    {
        HMENU menu = GetMenu(m_hwnd);

        // file menu
        {
            HMENU file_menu = GetSubMenu(menu, 0);

            // enable/disable close .xbe file
            EnableMenuItem(file_menu, ID_FILE_CLOSE_XBE, MF_BYCOMMAND | ((m_Xbe == 0) ? MF_GRAYED : MF_ENABLED));

            // enable/disable save .xbe file
            EnableMenuItem(file_menu, ID_FILE_SAVEXBEFILE, MF_BYCOMMAND | ((m_Xbe == 0) ? MF_GRAYED : MF_ENABLED));

            // enable/disable save .xbe file as
            EnableMenuItem(file_menu, ID_FILE_SAVEXBEFILEAS, MF_BYCOMMAND | ((m_Xbe == 0) ? MF_GRAYED : MF_ENABLED));

            // enable/disable export to .exe
            EnableMenuItem(file_menu, ID_FILE_EXPORTTOEXE, MF_BYCOMMAND | ((m_Xbe == 0) ? MF_GRAYED : MF_ENABLED));

            // recent xbe files menu
            {
                HMENU rxbe_menu = GetSubMenu(file_menu, 9);

                int max = m_dwRecentXbe;
                for(int v=0;v<max;v++)
                    EnableMenuItem(rxbe_menu, ID_FILE_RXBE_0 + v, MF_BYCOMMAND | MF_ENABLED);
            }

            // recent exe files menu
            {
                HMENU rexe_menu = GetSubMenu(file_menu, 10);

                int max = m_dwRecentExe;
                for(int v=0;v<max;v++)
                    EnableMenuItem(rexe_menu, ID_FILE_REXE_0 + v, MF_BYCOMMAND | MF_ENABLED);
            }
        }

        // edit menu
        {
            HMENU edit_menu = GetSubMenu(menu, 1);
            HMENU logo_menu = GetSubMenu(edit_menu, 0);
            HMENU pach_menu = GetSubMenu(edit_menu, 1);

            // enable export .xbe info
            EnableMenuItem(edit_menu, ID_EDIT_DUMPXBEINFOTO_FILE, MF_BYCOMMAND | ((m_Xbe == 0) ? MF_GRAYED : MF_ENABLED));
            EnableMenuItem(edit_menu, ID_EDIT_DUMPXBEINFOTO_DEBUGCONSOLE, MF_BYCOMMAND | ((m_Xbe == 0) ? MF_GRAYED : MF_ENABLED));

            // enable logo bitmap menu
            EnableMenuItem(edit_menu, 0, MF_BYPOSITION | ((m_Xbe == 0) ? MF_GRAYED : MF_ENABLED));

            // enable patch menu
            EnableMenuItem(edit_menu, 1, MF_BYPOSITION | ((m_Xbe == 0) ? MF_GRAYED : MF_ENABLED));

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

            if(m_KrnlDebug == DM_CONSOLE)
            {
                CheckMenuItem(emul_krnl, ID_EMULATION_DEBUGOUTPUTKERNEL_CONSOLE, MF_CHECKED);
                CheckMenuItem(emul_krnl, ID_EMULATION_DEBUGOUTPUTKERNEL_FILE, MF_UNCHECKED);
            }
            else if(m_KrnlDebug == DM_FILE)
            {
                CheckMenuItem(emul_krnl, ID_EMULATION_DEBUGOUTPUTKERNEL_CONSOLE, MF_UNCHECKED);
                CheckMenuItem(emul_krnl, ID_EMULATION_DEBUGOUTPUTKERNEL_FILE, MF_CHECKED);
            }
            else
            {
                CheckMenuItem(emul_krnl, ID_EMULATION_DEBUGOUTPUTKERNEL_CONSOLE, MF_UNCHECKED);
                CheckMenuItem(emul_krnl, ID_EMULATION_DEBUGOUTPUTKERNEL_FILE, MF_UNCHECKED);
            }

            if(m_CxbxDebug == DM_CONSOLE)
            {
                CheckMenuItem(emul_debg, ID_EMULATION_DEBUGOUTPUTGUI_CONSOLE, MF_CHECKED);
                CheckMenuItem(emul_debg, ID_EMULATION_DEBUGOUTPUTGUI_FILE, MF_UNCHECKED);
            }
            else if(m_CxbxDebug == DM_FILE)
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
            HMENU sett_menu = GetSubMenu(menu, 3);
            HMENU auto_menu = GetSubMenu(sett_menu, 4);

            // check appropriate choice
            if(m_AutoConvertToExe == AUTO_CONVERT_WINDOWS_TEMP)
            {
                CheckMenuItem(auto_menu, ID_SETTINGS_GENWT, MF_BYCOMMAND | MF_CHECKED);
                CheckMenuItem(auto_menu, ID_SETTINGS_GENXP, MF_BYCOMMAND | MF_UNCHECKED);
                CheckMenuItem(auto_menu, ID_SETTINGS_GENMA, MF_BYCOMMAND | MF_UNCHECKED);
            }
            else if(m_AutoConvertToExe == AUTO_CONVERT_XBE_PATH)
            {
                CheckMenuItem(auto_menu, ID_SETTINGS_GENWT, MF_BYCOMMAND | MF_UNCHECKED);
                CheckMenuItem(auto_menu, ID_SETTINGS_GENXP, MF_BYCOMMAND | MF_CHECKED);
                CheckMenuItem(auto_menu, ID_SETTINGS_GENMA, MF_BYCOMMAND | MF_UNCHECKED);
            }
            else
            {
                CheckMenuItem(auto_menu, ID_SETTINGS_GENWT, MF_BYCOMMAND | MF_UNCHECKED);
                CheckMenuItem(auto_menu, ID_SETTINGS_GENXP, MF_BYCOMMAND | MF_UNCHECKED);
                CheckMenuItem(auto_menu, ID_SETTINGS_GENMA, MF_BYCOMMAND | MF_CHECKED);
            }
        }

        // emulation menu
        {
            HMENU emul_menu = GetSubMenu(menu, 4);

            // enable emulation start
            EnableMenuItem(emul_menu, ID_EMULATION_START, MF_BYCOMMAND | ((m_Xbe == 0 || (m_hwndChild != NULL)) ? MF_GRAYED : MF_ENABLED));

            // enable emulation stop
            EnableMenuItem(emul_menu, ID_EMULATION_STOP, MF_BYCOMMAND | ((m_hwndChild == NULL) ? MF_GRAYED : MF_ENABLED));
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
    HMENU RXbeMenu = GetSubMenu(FileMenu, 9);
    HMENU RExeMenu = GetSubMenu(FileMenu, 10);

    // clear existing menu items
    {
        int v, max;

        max = GetMenuItemCount(RXbeMenu);
        for(v=0;v<max;v++)
            RemoveMenu(RXbeMenu, 0, MF_BYPOSITION);

        max = GetMenuItemCount(RExeMenu);
        for(v=0;v<max;v++)
            RemoveMenu(RExeMenu, 0, MF_BYPOSITION);
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

    // insert recent exe files
    {
        char szBuffer[270];

        int max = m_dwRecentExe;

        // if there are no recent files, throw in a disabled "(none)"
        if(max == 0)
        {
            AppendMenu(RExeMenu, MF_STRING, ID_FILE_REXE_0, "(none)");
            EnableMenuItem(RExeMenu, ID_FILE_REXE_0, MF_BYCOMMAND | MF_GRAYED);
        }

        // NOTE: Resource defines ID_FILE_REXE_0 through ID_FILE_REXE_9 must be in order
        for(int v=0;v<max;v++)
        {
            sprintf(szBuffer, "&%d : %s", v, m_szRecentExe[v]);
            AppendMenu(RExeMenu, MF_STRING, ID_FILE_REXE_0 + v, szBuffer);
        }
    }
}

// open an xbe file
void WndMain::OpenXbe(const char *x_filename)
{
    if(m_Xbe != 0)
        return;

    m_ExeFilename[0] = '\0';

    strcpy(m_XbeFilename, x_filename);

    m_Xbe = new Xbe(m_XbeFilename);

    if(m_Xbe->GetError() != 0)
    {
        MessageBox(m_hwnd, m_Xbe->GetError(), "Cxbx-Reloaded", MB_ICONSTOP | MB_OK);

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

        if(m_Xbe->GetError() != 0)
            MessageBox(m_hwnd, m_Xbe->GetError(), "Cxbx-Reloaded", MB_ICONSTOP | MB_OK);
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

// import an exe file
void WndMain::ImportExe(const char *x_filename)
{
    m_XbeFilename[0] = '\0';

    Exe *i_exe = new Exe(x_filename);

    if(i_exe->GetError() != 0)
    {
        MessageBox(m_hwnd, i_exe->GetError(), "Cxbx-Reloaded", MB_ICONSTOP | MB_OK);

        delete i_exe;

        return;
    }

    m_Xbe = new Xbe(i_exe, "Untitled", true);

    if(m_Xbe->GetError() != 0)
    {
        MessageBox(m_hwnd, m_Xbe->GetError(), "Cxbx-Reloaded", MB_ICONSTOP | MB_OK);

        delete m_Xbe; m_Xbe = 0;

        return;
    }

    // save this exe to the list of recent exe files
    if(m_ExeFilename[0] != '\0')
    {
        bool found = false;

        // if this filename already exists, temporarily remove it
        for(int c=0, r=0;c<m_dwRecentExe;c++, r++)
        {
            if(strcmp(m_szRecentExe[c], m_ExeFilename) == 0)
            {
                found = true;
                r++;
            }

            if(r != c)
            {
                if(m_szRecentExe[r] == 0 || r > m_dwRecentExe - 1)
                    m_szRecentExe[c] = 0;
                else
                    strncpy(m_szRecentExe[c], m_szRecentExe[r], MAX_PATH-1);
            }
        }

        if(found)
            m_dwRecentExe--;

        // move all items down one, removing the last one if necessary
        for(int v=RECENT_EXE_SIZE-1;v>0;v--)
        {
            if(m_szRecentExe[v-1] == 0)
                m_szRecentExe[v] = 0;
            else
            {
                if(m_szRecentExe[v] == 0)
                    m_szRecentExe[v] = (char*)calloc(1, MAX_PATH);
                strncpy(m_szRecentExe[v], m_szRecentExe[v-1], MAX_PATH-1);
            }
        }

        // add new item as first index
        {
            if(m_szRecentExe[0] == 0)
                m_szRecentExe[0] = (char*)calloc(1, MAX_PATH);

            strcpy(m_szRecentExe[0], m_ExeFilename);
        }

        if(m_dwRecentExe < RECENT_EXE_SIZE)
            m_dwRecentExe++;
    }

    UpdateRecentFiles();

    XbeLoaded();

    m_bExeChanged = true;
}

// convert to exe file
bool WndMain::ConvertToExe(const char *x_filename, bool x_bVerifyIfExists, HWND hwndParent)
{
    char filename[MAX_PATH] = "default.exe";

    if(x_filename == NULL)
    {
        OPENFILENAME ofn = {0};

        SuggestFilename(m_XbeFilename, filename, ".exe");

        ofn.lStructSize     = sizeof(OPENFILENAME);
        ofn.hwndOwner       = m_hwnd;
        ofn.lpstrFilter     = "Windows Executables (*.exe)\0*.exe\0";
        ofn.lpstrFile       = filename;
        ofn.nMaxFile        = MAX_PATH;
        ofn.nFilterIndex    = 1;
        ofn.lpstrFileTitle  = NULL;
        ofn.nMaxFileTitle   = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.lpstrDefExt     = "exe";
        ofn.Flags           = OFN_PATHMUSTEXIST;

        if(GetSaveFileName(&ofn) == FALSE)
            return false;

        strcpy(filename, ofn.lpstrFile);
    }
    else
    {
        strcpy(filename, x_filename);
    }

    // ask permission to overwrite if this file already exists
    if(x_bVerifyIfExists)
    {
        if(_access(filename, 0) != -1)
        {
            if(MessageBox(m_hwnd, "Overwrite existing file?", "Cxbx-Reloaded", MB_ICONQUESTION | MB_YESNO) != IDYES)
                return false;
        }
    }

    // convert file
    {
        EmuExe i_EmuExe(m_Xbe, m_KrnlDebug, m_KrnlDebugFilename, hwndParent);

        i_EmuExe.Export(filename);

        if(i_EmuExe.GetError() != 0)
        {
            MessageBox(m_hwnd, i_EmuExe.GetError(), "Cxbx-Reloaded", MB_ICONSTOP | MB_OK);
            return false;
        }
        else
        {
            strcpy(m_ExeFilename, filename);

            printf("WndMain: %s was converted to .exe.\n", m_Xbe->m_szAsciiTitle);

            m_bExeChanged = false;
        }
    }

    return true;
}

// start emulation
void WndMain::StartEmulation(EnumAutoConvert x_AutoConvert, HWND hwndParent)
{
    char szBuffer[MAX_PATH];

    // register xbe path with CxbxKrnl.dll
    g_EmuShared->SetXbePath(m_Xbe->m_szPath);

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
