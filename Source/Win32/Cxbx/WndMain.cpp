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
#include "Cxbx.h"
#include "EmuExe.h"
#include "CxbxKrnl.h"
#include "WndMain.h"
#include "WndAbout.h"

#include "resource.h"

#include <stdio.h>

// ******************************************************************
// * constructor
// ******************************************************************
WndMain::WndMain(HINSTANCE x_hInstance) : Wnd(x_hInstance), m_xbe(0), m_exe(0), m_exe_changed(false), m_xbe_changed(false), m_KrnlDebug(DM_NONE), m_CxbxDebug(DM_NONE)
{
    m_classname = "WndMain";
    m_wndname   = "Cxbx: Version " CXBX_VERSION;

    m_w         = 327;
    m_h         = 253;

    // ******************************************************************
    // * Center to desktop
    // ******************************************************************
    {
        RECT rect;

        GetWindowRect(GetDesktopWindow(), &rect);

        m_x = rect.left + (rect.right - rect.left)/2 - m_w/2;
        m_y = rect.top + (rect.bottom - rect.top)/2 - m_h/2;
    }

    m_ExeFilename = new char[260];
    m_ExeFilename[0] = '\0';

    m_XbeFilename = new char[260];
    m_XbeFilename[0] = '\0';

    m_CxbxDebugFilename = new char[260];
    m_CxbxDebugFilename[0] = '\0';

    m_KrnlDebugFilename = new char[260];
    m_KrnlDebugFilename[0] = '\0';

    // ******************************************************************
    // * Load configuration from registry
    // ******************************************************************
    {
        DWORD   dwDisposition, dwType, dwSize;
        HKEY    hKey;

        if(RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Cxbx", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_QUERY_VALUE, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS)
        {
            dwType = REG_DWORD;
            dwSize = sizeof(DWORD);

            RegQueryValueEx(hKey, "CxbxDebug", NULL, &dwType, (PBYTE)&m_CxbxDebug, &dwSize);
            RegQueryValueEx(hKey, "KrnlDebug", NULL, &dwType, (PBYTE)&m_KrnlDebug, &dwSize);

            dwType = REG_SZ;
            dwSize = 260;

            RegQueryValueEx(hKey, "CxbxDebugFilename", NULL, &dwType, (PBYTE)m_CxbxDebugFilename, &dwSize);
            RegQueryValueEx(hKey, "KrnlDebugFilename", NULL, &dwType, (PBYTE)m_KrnlDebugFilename, &dwSize);

            RegCloseKey(hKey);
        }
    }

    return;
}

// ******************************************************************
// * deconstructor
// ******************************************************************
WndMain::~WndMain()
{
    // ******************************************************************
    // * Save configuration to registry
    // ******************************************************************
    {
        DWORD   dwDisposition, dwType, dwSize;
        HKEY    hKey;

        if(RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Cxbx", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS)
        {
            dwType = REG_DWORD;
            dwSize = sizeof(DWORD);

            RegSetValueEx(hKey, "CxbxDebug", 0, dwType, (PBYTE)&m_CxbxDebug, dwSize);
            RegSetValueEx(hKey, "KrnlDebug", 0, dwType, (PBYTE)&m_KrnlDebug, dwSize);

            dwType = REG_SZ;
            dwSize = 260;

            RegSetValueEx(hKey, "CxbxDebugFilename", 0, dwType, (PBYTE)m_CxbxDebugFilename, dwSize);
            RegSetValueEx(hKey, "KrnlDebugFilename", 0, dwType, (PBYTE)m_KrnlDebugFilename, dwSize);

            RegCloseKey(hKey);
        }
    }

    delete[] m_XbeFilename;
    delete[] m_ExeFilename;

    delete[] m_CxbxDebugFilename;
    delete[] m_KrnlDebugFilename;
}

// ******************************************************************
// * WndProc
// ******************************************************************
LRESULT CALLBACK WndMain::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
        case WM_CREATE:
        {
            // resize so that client area = 321x201
            {
                RECT cRect = {0};
                RECT wRect = {0};

                GetClientRect(hwnd, &cRect);
                GetWindowRect(hwnd, &wRect);

                uint32 difW = (wRect.right - wRect.left) - (cRect.right);
                uint32 difH = (wRect.bottom - wRect.top) - (cRect.bottom);

                MoveWindow(hwnd, wRect.left, wRect.top, difW + 321, difH + 221, TRUE);
            }

            // initialize menu
            {
                HMENU hMenu = LoadMenu(m_hInstance, MAKEINTRESOURCE(IDR_MAINMENU));

                SetMenu(hwnd, hMenu);
            }

            // initialize back buffer
			{
                HDC hDC = GetDC(hwnd);

                m_back_bmp  = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_SPLASH), IMAGE_BITMAP, 0, 0, 0);
                m_logo_bmp  = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_LOGO), IMAGE_BITMAP, 0, 0, 0);
                
                m_back_dc   = CreateCompatibleDC(hDC);
                m_logo_dc   = CreateCompatibleDC(hDC);

                m_orig_bmp  = (HBITMAP)SelectObject(m_back_dc, m_back_bmp);
                m_orig_logo = (HBITMAP)SelectObject(m_logo_dc, m_logo_bmp);

                if(hDC != NULL)
                    ReleaseDC(hwnd, hDC);
            }
        }
        break;

        case WM_PAINT:
        {
            static bool menuInit = false;

            if(menuInit == false)
            {
                UpdateDebugConsoles();
                menuInit = true;
            }

            PAINTSTRUCT ps;

            BeginPaint(hwnd, &ps);

            HDC hDC = GetDC(hwnd);

            BitBlt(hDC, 0, 0, 320, 160, m_back_dc, 0, 0, SRCCOPY);
            BitBlt(hDC, 220, 168, 100, 17, m_logo_dc, 0, 0, SRCCOPY);

            // draw status bar
            {
				int nHeight = -MulDiv(8, GetDeviceCaps(hDC, LOGPIXELSY), 72);

				HFONT hFont = CreateFont(nHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_ROMAN, "verdana");

                HGDIOBJ tmpObj = SelectObject(hDC, hFont);

                SetBkColor(hDC, GetSysColor(COLOR_HIGHLIGHT));

				SetTextColor(hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));

                char buffer[255];

                if(m_xbe != 0 && m_xbe->GetError() == 0)
                    sprintf(buffer, "%s Loaded!", m_xbe->m_szAsciiTitle);
                else
    				sprintf(buffer, "%s", "Disclaimer: CXBX has no affiliation with Microsoft");

                RECT rect = {0, 187, 321, 201};

                ExtTextOut(hDC, 5, 187, ETO_OPAQUE, &rect, buffer, strlen(buffer), 0);

                SelectObject(hDC, tmpObj);

                DeleteObject(hFont);
            }

            if(hDC != NULL)
                ReleaseDC(hwnd, hDC);

            EndPaint(hwnd, &ps);
        }
        break;

        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case ID_FILE_OPEN_XBE:
                {
                    m_ExeFilename[0] = '\0';

                    OPENFILENAME ofn = {0};

                    char filename[260] = {0};

                    ofn.lStructSize     = sizeof(OPENFILENAME);
                    ofn.hwndOwner       = m_hwnd;
                    ofn.lpstrFilter     = ".xbe file (*.xbe)\0*.xbe\0";
                    ofn.lpstrFile       = filename;
                    ofn.nMaxFile        = 260;
                    ofn.nFilterIndex    = 1;
                    ofn.lpstrFileTitle  = NULL;
                    ofn.nMaxFileTitle   = 0;
                    ofn.lpstrInitialDir = NULL;
                    ofn.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                    if(GetOpenFileName(&ofn) == TRUE)
                    {
                        strcpy(m_XbeFilename, ofn.lpstrFile);

                        m_xbe = new Xbe(ofn.lpstrFile);

                        if(m_xbe->GetError() != 0)
                        {
                            MessageBox(m_hwnd, m_xbe->GetError(), "Cxbx", MB_ICONSTOP | MB_OK);

                            delete m_xbe; m_xbe = 0;

                            break;
                        }

                        XbeLoaded();
                    }

                }
                break;

                case ID_FILE_CLOSE_XBE:
                {
                    CloseXbe();
                }
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
				{
                    SaveXbeAs();
				}
				break;

                case ID_FILE_IMPORTFROMEXE:
                {
                    m_ExeFilename[0] = '\0';

                    OPENFILENAME ofn = {0};

                    char filename[260] = {0};

                    ofn.lStructSize     = sizeof(OPENFILENAME);
                    ofn.hwndOwner       = m_hwnd;
                    ofn.lpstrFilter     = "Win32 Executable file (*.exe)\0*.exe\0";
                    ofn.lpstrFile       = filename;
                    ofn.nMaxFile        = 260;
                    ofn.nFilterIndex    = 1;
                    ofn.lpstrFileTitle  = NULL;
                    ofn.nMaxFileTitle   = 0;
                    ofn.lpstrInitialDir = NULL;
                    ofn.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                    if(GetOpenFileName(&ofn) == TRUE)
                    {
                        m_XbeFilename[0] = '\0';

                        Exe *tmp = new Exe(ofn.lpstrFile);

                        if(tmp->GetError() != 0)
                        {
                            MessageBox(m_hwnd, tmp->GetError(), "Cxbx", MB_ICONSTOP | MB_OK);

                            delete tmp;

                            break;
                        }

                        m_xbe = new Xbe(tmp, "Untitled", true);

                        if(m_xbe->GetError() != 0)
                        {
                            MessageBox(m_hwnd, m_xbe->GetError(), "Cxbx", MB_ICONSTOP | MB_OK);

                            delete m_xbe; m_xbe = 0;

                            break;
                        }

                        XbeLoaded();

                        m_exe_changed = true;
                    }
                }
                break;

                case ID_FILE_EXPORTTOEXE:
                    ConvertToExe();
                    break;

                case ID_FILE_EXIT:
                    SendMessage(hwnd, WM_CLOSE, 0, 0);
                    break;

				case ID_EDIT_LOGOBITMAP_EXPORT:
				{
                    OPENFILENAME ofn = {0};

					char filename[260] = "logo.bmp";

					ofn.lStructSize     = sizeof(OPENFILENAME);
					ofn.hwndOwner       = m_hwnd;
					ofn.lpstrFilter     = "Bitmap file (*.bmp)\0*.bmp\0";
					ofn.lpstrFile       = filename;
					ofn.nMaxFile        = 260;
					ofn.nFilterIndex    = 1;
					ofn.lpstrFileTitle  = NULL;
					ofn.nMaxFileTitle   = 0;
					ofn.lpstrInitialDir = NULL;
                    ofn.lpstrDefExt     = "bmp";
                    ofn.lpstrTitle      = "Export Logo Bitmap";
					ofn.Flags           = OFN_PATHMUSTEXIST;

					if(GetSaveFileName(&ofn) == TRUE)
                    {
					    // check if file exists
					    {
						    FILE *tmp = fopen(ofn.lpstrFile, "r");

						    if(tmp != 0)
						    {
							    fclose(tmp);

                                if(MessageBox(m_hwnd, "overwrite existing file?", "Cxbx", MB_ICONQUESTION | MB_YESNO) != IDYES)
								    return TRUE;
						    }
					    }

					    // export logo bitmap
					    {
                            uint08 i_gray[100*17];

							m_xbe->ExportLogoBitmap(i_gray);

							if(m_xbe->GetError() == 0)
							{
								FILE *logo = fopen(ofn.lpstrFile, "wb");

                                // write bitmap header
                                {
                                    BITMAPFILEHEADER    bmfh;

                                    bmfh.bfType         = *(uint16*)"BM";
                                    bmfh.bfSize         = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) - sizeof(RGBQUAD) + (100*17)*sizeof(RGBTRIPLE) + 2;
                                    bmfh.bfReserved1    = 0;
                                    bmfh.bfReserved2    = 0;
                                    bmfh.bfOffBits      = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) - sizeof(RGBQUAD);

                                    fwrite(&bmfh, sizeof(bmfh), 1, logo);
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

                                    fwrite(&bmi, sizeof(bmi) - 4, 1, logo);
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

                                    fwrite(bmp_data, 100*17*sizeof(RGBTRIPLE), 1, logo);
                                }

                                // padd the extra 2 bytes
                                {
                                    uint16 pad = 0;

                                    fwrite(&pad, 2, 1, logo);
                                }

								fclose(logo);
							}

							if(m_xbe->GetError() != 0)
							    MessageBox(m_hwnd, m_xbe->GetError(), "Cxbx", MB_ICONSTOP | MB_OK);
                            else
                            {
                                char buffer[255];

                                sprintf(buffer, "%s's logo bitmap was successfully exported.", m_xbe->m_szAsciiTitle);

                                MessageBox(m_hwnd, buffer, "Cxbx", MB_ICONINFORMATION | MB_OK);

                                printf("Cxbx: %s\n", buffer);
						    }
					    }
                    }
				}
				break;

				case ID_EDIT_LOGOBITMAP_IMPORT:
				{
                    OPENFILENAME ofn = {0};

					char filename[260] = "*.bmp";

					ofn.lStructSize     = sizeof(OPENFILENAME);
					ofn.hwndOwner       = m_hwnd;
					ofn.lpstrFilter     = "Bitmap file (*.bmp)\0*.bmp\0";
					ofn.lpstrFile       = filename;
					ofn.nMaxFile        = 260;
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
                                    MessageBox(m_hwnd, bmp_err, "Cxbx", MB_OK | MB_ICONEXCLAMATION);
                                    break;
                                }
                            }

							m_xbe->ImportLogoBitmap(i_gray);

							if(m_xbe->GetError() != 0)
                            {
							    MessageBox(m_hwnd, m_xbe->GetError(), "Cxbx", MB_ICONSTOP | MB_OK);

                                if(m_xbe->IsFatal())
                                    CloseXbe();
                                else
                                    m_xbe->ClearError();
                            }
                            else
                            {
                                m_exe_changed = true;
                                m_xbe_changed = true;

                                LoadLogo();

                                char buffer[255];

                                sprintf(buffer, "%s's logo bitmap was successfully updated.", m_xbe->m_szAsciiTitle);

                                printf("Cxbx: %s\n", buffer);

                                MessageBox(m_hwnd, buffer, "Cxbx", MB_ICONINFORMATION | MB_OK);
						    }
					    }
                    }
				}
				break;

                case ID_EDIT_PATCH_ALLOW64MB:
                {
                    m_exe_changed = true;
                    m_xbe_changed = true;

                    m_xbe->m_Header.dwInitFlags.bLimit64MB = !m_xbe->m_Header.dwInitFlags.bLimit64MB;

                    HMENU menu = GetMenu(m_hwnd);
                    HMENU edit_menu = GetSubMenu(menu, 1);
                    HMENU pach_menu = GetSubMenu(edit_menu, 1);

                    // check "allow >64 MB" if appropriate
                    {
                        bool res = m_xbe->m_Header.dwInitFlags.bLimit64MB;

                        UINT chk_flag = (res) ? MF_UNCHECKED : MF_CHECKED;

                        if(res)
                            printf("Cxbx: %s was patched to limit to 64MB of memory usage.\n", m_xbe->m_szAsciiTitle);
                        else
                            printf("Cxbx: %s was patched to allow >64MB of memory usage.\n", m_xbe->m_szAsciiTitle);
                        
                        CheckMenuItem(pach_menu, ID_EDIT_PATCH_ALLOW64MB, chk_flag);
                    }
                }
                break;

                case ID_EDIT_PATCH_DEBUGMODE:
                {
                    m_exe_changed = true;
                    m_xbe_changed = true;

                    // patch to/from debug mode
                    if((m_xbe->m_Header.dwEntryAddr ^ XOR_EP_RETAIL) > 0x01000000)
                    {
                        // we're in debug mode, so switch over to retail
                        uint32 ep = m_xbe->m_Header.dwEntryAddr ^ XOR_EP_RETAIL;            // decode from debug mode
                        uint32 kt = m_xbe->m_Header.dwKernelImageThunkAddr ^ XOR_KT_DEBUG;  // decode from debug mode

                        m_xbe->m_Header.dwEntryAddr = ep ^ XOR_EP_DEBUG;                    // encode to retail mode
                        m_xbe->m_Header.dwKernelImageThunkAddr = kt ^ XOR_KT_RETAIL;        // encode to retail mode
                    }
                    else
                    {
                        // we're in retail mode, so switch to debug
                        uint32 ep = m_xbe->m_Header.dwEntryAddr ^ XOR_EP_DEBUG;             // decode from retail mode
                        uint32 kt = m_xbe->m_Header.dwKernelImageThunkAddr ^ XOR_KT_RETAIL; // decode from retail mode

                        m_xbe->m_Header.dwEntryAddr = ep ^ XOR_EP_RETAIL;                   // encode to debug mode
                        m_xbe->m_Header.dwKernelImageThunkAddr = kt ^ XOR_KT_DEBUG;         // encode to debug mode
                    }

                    HMENU menu = GetMenu(m_hwnd);
                    HMENU edit_menu = GetSubMenu(menu, 1);

                    HMENU pach_menu = GetSubMenu(edit_menu, 1);

                    // check "debug mode" if appropriate
                    {
                        bool res = (m_xbe->m_Header.dwEntryAddr ^ XOR_EP_RETAIL) > 0x01000000;

                        UINT chk_flag = (res) ? MF_CHECKED : MF_UNCHECKED;

                        if(res)
                            printf("Cxbx: %s was converted to debug mode.\n", m_xbe->m_szAsciiTitle);
                        else
                            printf("Cxbx: %s was converted to retail mode.\n", m_xbe->m_szAsciiTitle);

                        CheckMenuItem(pach_menu, ID_EDIT_PATCH_DEBUGMODE, chk_flag);
                    }
                }
                break;

                case ID_EDIT_EXTRACTXBEINFO:
                {
                    OPENFILENAME ofn = {0};

					char filename[260] = "Xbe.txt";

					ofn.lStructSize     = sizeof(OPENFILENAME);
					ofn.hwndOwner       = m_hwnd;
					ofn.lpstrFilter     = "text file (*.txt)\0*.txt\0";
					ofn.lpstrFile       = filename;
					ofn.nMaxFile        = 260;
					ofn.nFilterIndex    = 1;
					ofn.lpstrFileTitle  = NULL;
					ofn.nMaxFileTitle   = 0;
					ofn.lpstrInitialDir = NULL;
                    ofn.lpstrDefExt     = "txt";
					ofn.Flags           = OFN_PATHMUSTEXIST;

					if(GetSaveFileName(&ofn) == TRUE)
                    {
					    // check if file exists
					    {
						    FILE *tmp = fopen(ofn.lpstrFile, "r");

						    if(tmp != 0)
						    {
							    fclose(tmp);

                                if(MessageBox(m_hwnd, "Overwrite existing file?", "Cxbx", MB_ICONQUESTION | MB_YESNO) != IDYES)
								    return TRUE;
						    }
					    }

					    // convert file
					    {
                            m_xbe->DumpInformation(ofn.lpstrFile);

						    if(m_xbe->GetError() != 0)
							    MessageBox(m_hwnd, m_xbe->GetError(), "Cxbx", MB_ICONSTOP | MB_OK);
                            else
                            {
                                char buffer[255];

                                sprintf(buffer, "%s's .xbe info was successfully exported.", m_xbe->m_szAsciiTitle);

                                printf("Cxbx: %s\n", buffer);

                                MessageBox(m_hwnd, buffer, "Cxbx", MB_ICONINFORMATION | MB_OK);
						    }
					    }
                    }
                }
                break;

                case ID_VIEW_DEBUGOUTPUTKERNEL_CONSOLE:
                {
                    if(m_KrnlDebug == DM_NONE || m_KrnlDebug == DM_FILE)
                        m_KrnlDebug = DM_CONSOLE;
                    else
                        m_KrnlDebug = DM_NONE;

                    MessageBox(m_hwnd, "This will not take effect until emulation is (re)started.\n", "Cxbx", MB_OK);

                    m_exe_changed = true;

                    UpdateDebugConsoles();
                }
                break;

                case ID_VIEW_DEBUGOUTPUTKERNEL_FILE:
                {
                    if(m_KrnlDebug == DM_FILE)
                    {
                        m_KrnlDebug = DM_NONE;

                        UpdateDebugConsoles();
                    }
                    else
                    {
                        OPENFILENAME ofn = {0};

	                    char filename[260] = "KrnlDebug.txt";

                        ofn.lStructSize     = sizeof(OPENFILENAME);
	                    ofn.hwndOwner       = m_hwnd;
	                    ofn.lpstrFilter     = "Text Documents (*.txt)\0*.txt\0";
	                    ofn.lpstrFile       = filename;
	                    ofn.nMaxFile        = 260;
	                    ofn.nFilterIndex    = 1;
	                    ofn.lpstrFileTitle  = NULL;
	                    ofn.nMaxFileTitle   = 0;
	                    ofn.lpstrInitialDir = NULL;
                        ofn.lpstrDefExt     = "txt";
	                    ofn.Flags           = OFN_PATHMUSTEXIST;

	                    if(GetSaveFileName(&ofn) != FALSE)
                        {
                            MessageBox(m_hwnd, "This will not take effect until emulation is (re)started.\n", "Cxbx", MB_OK);

                            strncpy(m_KrnlDebugFilename, ofn.lpstrFile, 259);

                            m_exe_changed = true;

                            m_KrnlDebug = DM_FILE;

                            UpdateDebugConsoles();
                        }
                    }
                }
                break;

                case ID_VIEW_DEBUGOUTPUTGUI_CONSOLE:
				{
                    if(m_CxbxDebug == DM_NONE || m_CxbxDebug == DM_FILE)
                        m_CxbxDebug = DM_CONSOLE;
                    else
                        m_CxbxDebug = DM_NONE;

                    UpdateDebugConsoles();
				}
				break;

                case ID_VIEW_DEBUGOUTPUTGUI_FILE:
                {
                    if(m_CxbxDebug == DM_FILE)
                    {
                        m_CxbxDebug = DM_NONE;

                        UpdateDebugConsoles();
                    }
                    else
                    {
                        OPENFILENAME ofn = {0};

	                    char filename[260] = "CxbxDebug.txt";

                        ofn.lStructSize     = sizeof(OPENFILENAME);
	                    ofn.hwndOwner       = m_hwnd;
	                    ofn.lpstrFilter     = "Text Documents (*.txt)\0*.txt\0";
	                    ofn.lpstrFile       = filename;
	                    ofn.nMaxFile        = 260;
	                    ofn.nFilterIndex    = 1;
	                    ofn.lpstrFileTitle  = NULL;
	                    ofn.nMaxFileTitle   = 0;
	                    ofn.lpstrInitialDir = NULL;
                        ofn.lpstrDefExt     = "txt";
	                    ofn.Flags           = OFN_PATHMUSTEXIST;

	                    if(GetSaveFileName(&ofn) != FALSE)
                        {
                            strncpy(m_CxbxDebugFilename, ofn.lpstrFile, 259);

                            m_CxbxDebug = DM_FILE;

                            UpdateDebugConsoles();
                        }

                    }
                }
                break;

                case ID_EMULATION_START:
                {
                    if(m_ExeFilename[0] == '\0' || m_exe_changed)
                        if(!ConvertToExe())
                            break;

                    // shell .exe
                    {
                        char dir[260];

                        GetModuleFileName(NULL, dir, 260);

                        sint32 spot=-1;
                        for(int v=0;v<260;v++)
                        {
                            if(dir[v] == '\\')
                                spot = v;
                            else if(dir[v] == '\0')
                                break;
                        }

                        if(spot != -1)
                            dir[spot] = '\0';

                        if((int)ShellExecute(NULL, "open", m_ExeFilename, NULL, dir, SW_SHOWDEFAULT) <= 32)
                        {
                            MessageBox(m_hwnd, "Shell failed. (try converting .exe again)", "Cxbx", MB_ICONSTOP | MB_OK);

                            printf("Cxbx: %s shell failed.\n", m_xbe->m_szAsciiTitle);
                        }
                        else
                        {
                            printf("Cxbx: %s emulation started.\n", m_xbe->m_szAsciiTitle);
                        }
                    }
                }
                break;

                case ID_HELP_ABOUT:
                {
                    WndAbout *AboutWnd = new WndAbout(m_hInstance, m_hwnd);

                    while(AboutWnd->GetError() == 0 && AboutWnd->ProcessMessages())
                        Sleep(10);

                    if(AboutWnd->GetError() != 0)
                        MessageBox(m_hwnd, AboutWnd->GetError(), "Cxbx", MB_ICONSTOP | MB_OK);

                    delete AboutWnd;
                }
                break;

                case ID_HELP_HOMEPAGE:
                {
                    ShellExecute(NULL, "open", "http://www.caustik.com/xbox/", NULL, NULL, SW_SHOWNORMAL);
                }
                break;
            }

            break;
        }

        case WM_CLOSE:
            if(m_xbe_changed)
            {
                int ret = MessageBox(m_hwnd, "Changes have been made, do you wish to save?", "Cxbx", MB_ICONQUESTION | MB_YESNOCANCEL);

                if(ret == IDYES)
                    SaveXbeAs();
                else if(ret == IDCANCEL)
                    break;
            }
            DestroyWindow(hwnd);
            break;

        case WM_DESTROY:
        {
            FreeConsole();

            HDC hDC = GetDC(hwnd);

            SelectObject(m_logo_dc, m_orig_logo);

            SelectObject(m_back_dc, m_orig_bmp);

            DeleteObject(m_logo_dc);

            DeleteObject(m_back_dc);

            DeleteObject(m_logo_bmp);

            DeleteObject(m_back_bmp);

			ReleaseDC(hwnd, hDC);

            if(m_xbe != 0)
            {
                delete m_xbe;
                m_xbe = 0;
            }

            PostQuitMessage(0);
        }
        break;

        default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

    return 0;
}

// ******************************************************************
// * SuggestFilename
// ******************************************************************
void WndMain::SuggestFilename(const char *x_orig_filename, char *x_filename, char x_extension[4])
{
    uint32 found = 0;
    uint32 v = 0;

    while(x_orig_filename[v] != '\0')
    {
        if(x_orig_filename[v] == '\\')
            found = v;
        v++;
    }

    if(found != 0)
    {
        strcpy(x_filename, x_orig_filename + found + 1);

        uint32 loc = 0;
        uint32 c = 0;

        while(x_filename[c] != '\0')
        {
            if(x_filename[c] == '.')
                loc = c;
            c++;
        }

        if(loc != 0)
            *(uint32*)&x_filename[loc] = *(uint32*)x_extension;
    }
}

// ******************************************************************
// * XbeLoaded
// ******************************************************************
void WndMain::XbeLoaded()
{
    LoadLogo();

    // disable / enable appropriate menus
    {
        HMENU menu = GetMenu(m_hwnd);

        // file menu
        {
			HMENU file_menu = GetSubMenu(menu, 0);

            // disable open .xbe file
            EnableMenuItem(file_menu, ID_FILE_OPEN_XBE, MF_BYCOMMAND | MF_GRAYED);

            // enable close .xbe file
            EnableMenuItem(file_menu, ID_FILE_CLOSE_XBE, MF_BYCOMMAND | MF_ENABLED);

			// enable save .xbe file
			EnableMenuItem(file_menu, ID_FILE_SAVEXBEFILE, MF_BYCOMMAND | MF_ENABLED);

			// enable save .xbe file as
			EnableMenuItem(file_menu, ID_FILE_SAVEXBEFILEAS, MF_BYCOMMAND | MF_ENABLED);

            // disable import from .exe
            EnableMenuItem(file_menu, ID_FILE_IMPORTFROMEXE, MF_BYCOMMAND | MF_GRAYED);

            // enable export to .exe
            EnableMenuItem(file_menu, ID_FILE_EXPORTTOEXE, MF_BYCOMMAND | MF_ENABLED);
        }

        // edit menu
        {
            HMENU edit_menu = GetSubMenu(menu, 1);
            HMENU logo_menu = GetSubMenu(edit_menu, 0);
            HMENU pach_menu = GetSubMenu(edit_menu, 1);

            // enable export .xbe info
            EnableMenuItem(edit_menu, ID_EDIT_EXTRACTXBEINFO, MF_BYCOMMAND | MF_ENABLED);

            // enable logo bitmap menu
            EnableMenuItem(edit_menu, 0, MF_BYPOSITION | MF_ENABLED);

            // enable patch menu
            EnableMenuItem(edit_menu, 1, MF_BYPOSITION | MF_ENABLED);

            // patch menu
            {
                // check "allow >64 MB" if appropriate
                {
                    UINT chk_flag = (m_xbe->m_Header.dwInitFlags.bLimit64MB) ? MF_UNCHECKED : MF_CHECKED;

                    CheckMenuItem(pach_menu, ID_EDIT_PATCH_ALLOW64MB, chk_flag);
                }

                // check "debug mode" if appropriate
                {
                    UINT chk_flag = ((m_xbe->m_Header.dwEntryAddr ^ XOR_EP_RETAIL) > 0x01000000) ? MF_CHECKED : MF_UNCHECKED;

                    CheckMenuItem(pach_menu, ID_EDIT_PATCH_DEBUGMODE, chk_flag);
                }
            }
        }

        // view menu
        {
            HMENU view_menu = GetSubMenu(menu, 2);
        }

        // emulation menu
        {
            HMENU emul_menu = GetSubMenu(menu, 3);
            // enable emulation start
            EnableMenuItem(emul_menu, ID_EMULATION_START, MF_BYCOMMAND | MF_ENABLED);
        }
    }

    printf("Cxbx: %s loaded.\n", m_xbe->m_szAsciiTitle);    
}

// ******************************************************************
// * LoadLogo
// ******************************************************************
void WndMain::LoadLogo()
{
    uint08 i_gray[100*17];

    m_xbe->ExportLogoBitmap(i_gray);

    if(m_xbe->GetError() != 0)
    {
        MessageBox(m_hwnd, m_xbe->GetError(), "Cxbx", MB_ICONEXCLAMATION | MB_OK);

        if(m_xbe->IsFatal())
            CloseXbe();

        return;
    }

    uint32 v=0;
    for(uint32 y=0;y<17;y++)
    {
        for(uint32 x=0;x<100;x++)
        {
            SetPixel(m_logo_dc, x, y, RGB(i_gray[v], i_gray[v], i_gray[v]));
            v++;
        }
    }

    RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE);
}

// ******************************************************************
// * UpdateDebugConsoles
// ******************************************************************
void WndMain::UpdateDebugConsoles()
{
    HMENU menu = GetMenu(m_hwnd);
    HMENU view_menu = GetSubMenu(menu, 2);
    HMENU view_debg = GetSubMenu(view_menu, 0);
    HMENU view_krnl = GetSubMenu(view_menu, 1);

    if(m_KrnlDebug == DM_CONSOLE)
    {
        CheckMenuItem(view_krnl, ID_VIEW_DEBUGOUTPUTKERNEL_CONSOLE, MF_CHECKED);
        CheckMenuItem(view_krnl, ID_VIEW_DEBUGOUTPUTKERNEL_FILE, MF_UNCHECKED);
    }
    else if(m_KrnlDebug == DM_FILE)
    {
        CheckMenuItem(view_krnl, ID_VIEW_DEBUGOUTPUTKERNEL_CONSOLE, MF_UNCHECKED);
        CheckMenuItem(view_krnl, ID_VIEW_DEBUGOUTPUTKERNEL_FILE, MF_CHECKED);
    }
    else
    {
        CheckMenuItem(view_krnl, ID_VIEW_DEBUGOUTPUTKERNEL_CONSOLE, MF_UNCHECKED);
        CheckMenuItem(view_krnl, ID_VIEW_DEBUGOUTPUTKERNEL_FILE, MF_UNCHECKED);
    }

    if(m_CxbxDebug == DM_CONSOLE)
    {
        if(AllocConsole())
        {
            freopen("CONOUT$", "wt", stdout);

            printf("%s", "Cxbx: Debug console allocated.\n");

            CheckMenuItem(view_debg, ID_VIEW_DEBUGOUTPUTGUI_CONSOLE, MF_CHECKED);
            CheckMenuItem(view_debg, ID_VIEW_DEBUGOUTPUTGUI_FILE, MF_UNCHECKED);
        }
    }
    else if(m_CxbxDebug == DM_FILE)
    {
        FreeConsole();

        freopen(m_CxbxDebugFilename, "wt", stdout);

        printf("%s", "Cxbx: Debug console allocated.\n");

        CheckMenuItem(view_debg, ID_VIEW_DEBUGOUTPUTGUI_CONSOLE, MF_UNCHECKED);
        CheckMenuItem(view_debg, ID_VIEW_DEBUGOUTPUTGUI_FILE, MF_CHECKED);
    }
    else
    {
        FreeConsole();

        CheckMenuItem(view_debg, ID_VIEW_DEBUGOUTPUTGUI_CONSOLE, MF_UNCHECKED);
        CheckMenuItem(view_debg, ID_VIEW_DEBUGOUTPUTGUI_FILE, MF_UNCHECKED);
    }
}

// ******************************************************************
// * ConvertToExe
// ******************************************************************
bool WndMain::ConvertToExe()
{
    OPENFILENAME ofn = {0};

	char filename[260] = "default.exe";

    SuggestFilename(m_XbeFilename, filename, ".exe");

	ofn.lStructSize     = sizeof(OPENFILENAME);
	ofn.hwndOwner       = m_hwnd;
	ofn.lpstrFilter     = "Win32 Executable file (*.exe)\0*.exe\0";
	ofn.lpstrFile       = filename;
	ofn.nMaxFile        = 260;
	ofn.nFilterIndex    = 1;
	ofn.lpstrFileTitle  = NULL;
	ofn.nMaxFileTitle   = 0;
	ofn.lpstrInitialDir = NULL;
    ofn.lpstrDefExt     = "exe";
	ofn.Flags           = OFN_PATHMUSTEXIST;

	if(GetSaveFileName(&ofn) == FALSE)
        return false;

	// check if file exists
	{
		FILE *tmp = fopen(ofn.lpstrFile, "r");

		if(tmp != 0)
		{
			fclose(tmp);

            if(MessageBox(m_hwnd, "Overwrite existing file?", "Cxbx", MB_ICONQUESTION | MB_YESNO) != IDYES)
				return false;
		}
	}

	// convert file
	{
		EmuExe i_EmuExe(m_xbe, m_KrnlDebug, m_KrnlDebugFilename);

		i_EmuExe.Export(ofn.lpstrFile);

		if(i_EmuExe.GetError() != 0)
        {
			MessageBox(m_hwnd, i_EmuExe.GetError(), "Cxbx", MB_ICONSTOP | MB_OK);
            return false;
        }
        else
        {
            strcpy(m_ExeFilename, ofn.lpstrFile);

            char buffer[255];

            sprintf(buffer, "%s was successfully converted to .exe.", m_xbe->m_szAsciiTitle);

            printf("Cxbx: %s was converted to .exe.\n", m_xbe->m_szAsciiTitle);

            MessageBox(m_hwnd, buffer, "Cxbx", MB_ICONINFORMATION | MB_OK);

            m_exe_changed = false;
        }
	}

    return true;
}

// ******************************************************************
// * SaveXbeAs
// ******************************************************************
void WndMain::SaveXbeAs()
{
    OPENFILENAME ofn = {0};

	char filename[260] = "default.xbe";

    SuggestFilename(m_XbeFilename, filename, ".xbe");

	ofn.lStructSize     = sizeof(OPENFILENAME);
	ofn.hwndOwner       = m_hwnd;
	ofn.lpstrFilter     = ".xbe file (*.xbe)\0*.xbe\0";
	ofn.lpstrFile       = filename;
	ofn.nMaxFile        = 260;
	ofn.nFilterIndex    = 1;
	ofn.lpstrFileTitle  = NULL;
	ofn.nMaxFileTitle   = 0;
	ofn.lpstrInitialDir = NULL;
    ofn.lpstrDefExt     = "xbe";
	ofn.Flags           = OFN_PATHMUSTEXIST;

	if(GetSaveFileName(&ofn) == TRUE)
		SaveXbe(ofn.lpstrFile);

}

// ******************************************************************
// * SaveXbe
// ******************************************************************
void WndMain::SaveXbe(const char *x_filename)
{
	// check if file exists
	{
		FILE *tmp = fopen(x_filename, "r");

		if(tmp != 0)
		{
			fclose(tmp);

            if(MessageBox(m_hwnd, "Overwrite existing file?", "Cxbx", MB_ICONQUESTION | MB_YESNO) != IDYES)
				return;
		}
	}

	// save xbe file
	{
        m_xbe->Export(x_filename);

		if(m_xbe->GetError() != 0)
			MessageBox(m_hwnd, m_xbe->GetError(), "Cxbx", MB_ICONSTOP | MB_OK);
        else
        {
            char buffer[255];

            sprintf(buffer, "%s was successfully saved.", m_xbe->m_szAsciiTitle);

            printf("Cxbx: %s was successfully saved.\n", m_xbe->m_szAsciiTitle);

            MessageBox(m_hwnd, buffer, "Cxbx", MB_ICONINFORMATION | MB_OK);

            m_xbe_changed = false;
		}
	}
}

// ******************************************************************
// * CloseXbe
// ******************************************************************
void WndMain::CloseXbe()
{
    if(m_xbe_changed)
    {
        int ret = MessageBox(m_hwnd, "Changes have been made, do you wish to save?", "Cxbx", MB_ICONQUESTION | MB_YESNOCANCEL);

        if(ret == IDYES)
            SaveXbeAs();
        else if(ret == IDCANCEL)
            return;
    }

    printf("Cxbx: %s unloaded.\n", m_xbe->m_szAsciiTitle);
    
    m_xbe_changed = false;

    delete m_xbe; m_xbe = 0;

    // disable / enable appropriate menus
    {
        HMENU menu = GetMenu(m_hwnd);

        // file menu
        {
		    HMENU file_menu = GetSubMenu(menu, 0);

            // enable open .xbe file
            EnableMenuItem(file_menu, ID_FILE_OPEN_XBE, MF_BYCOMMAND | MF_ENABLED);

            // disable close .xbe file
            EnableMenuItem(file_menu, ID_FILE_CLOSE_XBE, MF_BYCOMMAND | MF_GRAYED);

		    // disable save .xbe file
		    EnableMenuItem(file_menu, ID_FILE_SAVEXBEFILE, MF_BYCOMMAND | MF_GRAYED);

		    // disable save .xbe file as
		    EnableMenuItem(file_menu, ID_FILE_SAVEXBEFILEAS, MF_BYCOMMAND | MF_GRAYED);

            // enable import from .exe
            EnableMenuItem(file_menu, ID_FILE_IMPORTFROMEXE, MF_BYCOMMAND | MF_ENABLED);

            // disable convert to .exe
            EnableMenuItem(file_menu, ID_FILE_EXPORTTOEXE, MF_BYCOMMAND | MF_GRAYED);
        }

        // edit menu
        {
            HMENU edit_menu = GetSubMenu(menu, 1);

            // disable export .xbe info
            EnableMenuItem(edit_menu, ID_EDIT_EXTRACTXBEINFO, MF_BYCOMMAND | MF_GRAYED);

            // disable logo bitmap menu
            EnableMenuItem(edit_menu, 0, MF_BYPOSITION | MF_GRAYED);

            // disable patch menu
            EnableMenuItem(edit_menu, 1, MF_BYPOSITION | MF_GRAYED);
        }

        // view menu
        {
            HMENU view_menu = GetSubMenu(menu, 2);
        }

        // emulation menu
        {
            HMENU emul_menu = GetSubMenu(menu, 3);

            // disable emulation start
            EnableMenuItem(emul_menu, ID_EMULATION_START, MF_BYCOMMAND | MF_GRAYED);                            
        }
    }

    // clear logo bitmap
    {
        uint32 v=0;
        for(uint32 y=0;y<17;y++)
        {
            for(uint32 x=0;x<100;x++)
            {
                SetPixel(m_logo_dc, x, y, RGB(0, 0, 0));
                v++;
            }
        }
    }

    RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE);
}
