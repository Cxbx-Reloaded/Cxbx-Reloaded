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
#include "Wnd.h"
#include "resource/ResCxbx.h"

// ******************************************************************
// * constructor
// ******************************************************************
Wnd::Wnd(HINSTANCE x_hInstance) : m_hInstance(x_hInstance)
{
    m_classname   = "CxbxWnd";
    m_wndname     = "Cxbx-Reloaded Generic Window";
    m_x           = 150;
    m_y           = 150;
    m_w           = 640;
    m_h           = 480;
    m_parent      = NULL;
    m_clsstyle    = CS_HREDRAW | CS_VREDRAW;
    m_wndstyle    = WS_CLIPCHILDREN | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE;
    m_background  = (HBRUSH)GetStockObject(BLACK_BRUSH);
    m_initialized = false;

    return;
}

// ******************************************************************
// * deconstructor
// ******************************************************************
Wnd::~Wnd()
{
}

// ******************************************************************
// * ProcessMessages
// ******************************************************************
bool Wnd::ProcessMessages()
{
    // initialize window
    if(!m_initialized)
    {
        m_initialized = true;

        WNDCLASS wnd_class;

        wnd_class.hInstance     = m_hInstance;
        wnd_class.lpszClassName = m_classname;
        wnd_class.lpfnWndProc   = WndProcForward;
        wnd_class.style         = m_clsstyle;
        wnd_class.hIcon         = 0; // TODO : LoadIcon(hmodule, ?)
        wnd_class.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wnd_class.lpszMenuName  = nullptr;
        wnd_class.cbClsExtra    = 0;
        wnd_class.cbWndExtra    = 0;
        wnd_class.hbrBackground = m_background;

        m_class = RegisterClass(&wnd_class);

        m_hwnd = CreateWindowEx
        (
            0,
            m_classname,
            m_wndname.c_str(),
            m_wndstyle,
            m_x,
            m_y,
            m_w,
            m_h,
            m_parent,
            NULL,
            m_hInstance,
            this
        );

        if(m_hwnd == 0)
        {
            SetFatalError("Could not create window.");
            UnregisterClass(m_classname, m_hInstance);
            goto cleanup;
        }

        SetWindowLong(m_hwnd, GWL_USERDATA, (LONG)this);

cleanup:;

    }

    MSG msg;

    if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
    {
        if(!GetMessage(&msg, NULL, 0, 0))
        {
            UnregisterClass(m_classname, m_hInstance);
            return false;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return true;
}

// ******************************************************************
// * WndProcForward
// ******************************************************************
LRESULT CALLBACK Wnd::WndProcForward(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Wnd *forward_to = (Wnd*)GetWindowLong(hwnd, GWL_USERDATA);

    if(forward_to == 0)
    {
        if(uMsg == WM_CREATE)
        {
            LPCREATESTRUCT cs = (LPCREATESTRUCT)lParam;

            forward_to = (Wnd*)cs->lpCreateParams;
        }else{
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }

    return forward_to->WndProc(hwnd, uMsg, wParam, lParam);
}

// ******************************************************************
// * WndProc
// ******************************************************************
LRESULT CALLBACK Wnd::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}
