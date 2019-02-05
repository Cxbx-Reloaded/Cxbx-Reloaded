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

#include "EmuShared.h"

#include "CxbxVersion.h"
#include "DlgAbout.h"
#include "resource/ResCxbx.h"
#include "common\util\CxbxUtil.h"

#include <commctrl.h>
#include <string>
#include <vector>

// Array of tab pane handles
std::vector<HWND> aboutTabPanes;
// Current (active) tab pane index
unsigned int aboutCurrentTab = 0;

/*! windows dialog procedure */
static INT_PTR CALLBACK DlgAboutProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

VOID ShowAboutDialog(HWND hwnd)
{
    /*! show dialog box */
    DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUT), hwnd, DlgAboutProc);
}

INT_PTR CALLBACK DlgAboutProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_INITDIALOG:
        {
			// Set the dialog icon
			HICON hIcon = (HICON)LoadImageW(
				GetModuleHandleW(NULL), MAKEINTRESOURCEW(IDI_CXBX), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE
			);

			SendMessageW(hWndDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

			// Build the Tab Control
			TCITEM tabInfo;
			memset(&tabInfo, 0, sizeof(tabInfo));
			tabInfo.mask = TCIF_TEXT;
			tabInfo.cchTextMax = 20;
			tabInfo.pszText = "About";
			SendMessage(GetDlgItem(hWndDlg, IDC_TAB1), TCM_INSERTITEM, 0, (LPARAM)&tabInfo);
			tabInfo.pszText = "Contributors";
			SendMessage(GetDlgItem(hWndDlg, IDC_TAB1), TCM_INSERTITEM, 1, (LPARAM)&tabInfo);
			tabInfo.pszText = "License";
			SendMessage(GetDlgItem(hWndDlg, IDC_TAB1), TCM_INSERTITEM, 2, (LPARAM)&tabInfo);

			// Get tab pane dimensions
			RECT tabRect;
			GetClientRect(GetDlgItem(hWndDlg, IDC_TAB1), &tabRect);
			SendMessage(GetDlgItem(hWndDlg, IDC_TAB1), TCM_ADJUSTRECT, FALSE, (LPARAM)&tabRect);
			// Tab Pane 1
			HWND tab = CreateWindowEx
        	(NULL, "STATIC", "\nCxbx-Reloaded\nVersion " _CXBX_VERSION "\n© The Cxbx-Reloaded Team",
				WS_CHILD | WS_VISIBLE,
				tabRect.left + 10, tabRect.top + 10,
				tabRect.right - tabRect.left,
				tabRect.bottom - tabRect.top,
				GetDlgItem(hWndDlg, IDC_TAB1), (HMENU)1,
				GetModuleHandle(NULL), NULL
			);

			SendMessage(tab, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
			SetParent(tab, hWndDlg);
			aboutTabPanes.push_back(tab);
			
        	// Tab Pane 2
			HRSRC rContributors = FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_CONTRIBUTORS), "TXT");

			std::string contributors = "\n";
        	contributors += std::string(
				(char*)LockResource(LoadResource(GetModuleHandle(NULL), rContributors)), 
				SizeofResource(GetModuleHandle(NULL), rContributors)
			);

			unix2dos(contributors);

			tab = CreateWindowEx(
				NULL, "EDIT", contributors.c_str(),
				WS_CHILD | WS_VSCROLL |ES_MULTILINE | ES_READONLY,
				tabRect.left + 10, tabRect.top + 10,
				tabRect.right - tabRect.left,
				tabRect.bottom - tabRect.top,
				GetDlgItem(hWndDlg, IDC_TAB1), (HMENU)1,
				GetModuleHandle(NULL), NULL
			);

			SendMessage(tab, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
			SetParent(tab, hWndDlg);
			aboutTabPanes.push_back(tab);
			
			// Tab Pane 3
			HRSRC rCopying = FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_COPYING), "TXT");

			std::string copying = "\n";
        	copying += std::string(
				(char*)LockResource(LoadResource(GetModuleHandle(NULL), rCopying)),
				SizeofResource(GetModuleHandle(NULL), rCopying)
			);

			unix2dos(copying);

			tab = CreateWindowEx(
				NULL, "EDIT", copying.c_str(),
				WS_CHILD | WS_VSCROLL | ES_MULTILINE | ES_READONLY,
				tabRect.left + 10, tabRect.top + 10,
				tabRect.right - tabRect.left,
				tabRect.bottom - tabRect.top,
				GetDlgItem(hWndDlg, IDC_TAB1), (HMENU)1,
				GetModuleHandle(NULL), NULL
			);

			SendMessage(tab, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
			SetParent(tab, hWndDlg);
			aboutTabPanes.push_back(tab);

			aboutCurrentTab = 0;
        	UpdateWindow(hWndDlg);			
        }
        break;

        case WM_CLOSE:
        {
            PostMessage(hWndDlg, WM_COMMAND, IDOK, 0);
        }
        break;

        case WM_COMMAND:
        {
            HWND hWndButton = GetDlgItem(hWndDlg, LOWORD(wParam));

			switch (LOWORD(wParam))
			{
				case IDOK:
					for (auto it = aboutTabPanes.begin(); it != aboutTabPanes.end(); ++it) {
						DestroyWindow(*it);
					}

					aboutTabPanes.clear();
					EndDialog(hWndDlg, wParam);
					break;
			}
        }
        break;

		case WM_NOTIFY:
		{
			unsigned int index = SendMessage(GetDlgItem(hWndDlg, IDC_TAB1), TCM_GETCURSEL, 0, 0);

			// Do nothing if the tab hasnt been changed, or if we try to load a non existant tab
			if (index == aboutCurrentTab || index >= aboutTabPanes.size()) {
				break;
			}

			// Show the selected tab pane
			ShowWindow(aboutTabPanes[aboutCurrentTab], SW_HIDE);			ShowWindow(aboutTabPanes[index], SW_SHOW);		
			aboutCurrentTab = index;
		}
		break;
    }
    return FALSE;
}
