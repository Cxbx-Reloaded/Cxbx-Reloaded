// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->Cxbx->WndAbout.cpp
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
#include "WndAbout.h"
#include "ResCxbx.h"

WndAbout::WndAbout(HINSTANCE x_hInstance, HWND x_parent) : Wnd(x_hInstance)
{
    m_classname = "WndAbout";
    m_wndname   = "Cxbx : About";

    m_w         = 400;
    m_h         = 300;

    // center to parent
    {
        RECT rect;

        GetWindowRect(x_parent, &rect);

        m_x = rect.left + (rect.right - rect.left)/2 - m_w/2;
        m_y = rect.top + (rect.bottom - rect.top)/2 - m_h/2;
    }

    m_parent    = x_parent;
    m_wndstyle  = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE;

    return;
}

WndAbout::~WndAbout()
{
}

// window message processing procedure
LRESULT CALLBACK WndAbout::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
        case WM_CREATE:
        {
            // resize window so that client area := 400x300
            {
                RECT cRect;
                RECT wRect;

                GetClientRect(hwnd, &cRect);
                GetWindowRect(hwnd, &wRect);

                uint32 difW = (wRect.right  - wRect.left) - (cRect.right);
                uint32 difH = (wRect.bottom - wRect.top)  - (cRect.bottom);

                MoveWindow(hwnd, wRect.left, wRect.top, difW + 400, difH + 300, TRUE);
            }

            EnableWindow(m_parent, FALSE);

            HDC hDC = GetDC(hwnd);

            int nHeight = -MulDiv(8, GetDeviceCaps(hDC, LOGPIXELSY), 72);

			m_hFont = CreateFont(nHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_ROMAN, "Verdana");

            m_BackBmp = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_ABOUT), IMAGE_BITMAP, 0, 0, 0);

            m_BackDC  = CreateCompatibleDC(hDC);

            m_OrigBmp  = (HBITMAP)SelectObject(m_BackDC, m_BackBmp);

            ReleaseDC(hwnd, hDC);

            SetClassLong(hwnd, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CXBX)));
        }
        break;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;

            BeginPaint(hwnd, &ps);

            HDC hDC = GetDC(hwnd);

            HGDIOBJ OrgObj = SelectObject(hDC, m_hFont);

            // draw bitmap
            BitBlt(hDC, 0, 0, 400, 300, m_BackDC, 0, 0, SRCCOPY);

            SelectObject(hDC, OrgObj);

            if(hDC != NULL)
                ReleaseDC(hwnd, hDC);

            EndPaint(hwnd, &ps);
        }
        break;

        case WM_LBUTTONUP:
            SendMessage(hwnd, WM_CLOSE, 0, 0);
			break;

        case WM_CLOSE:
            EnableWindow(m_parent, TRUE);
            DestroyWindow(hwnd);
			break;

        case WM_DESTROY:
            DeleteObject(m_hFont);
            PostQuitMessage(0);
			break;

        default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

    return 0;
}