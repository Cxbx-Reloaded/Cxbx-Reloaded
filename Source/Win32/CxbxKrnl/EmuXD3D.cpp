// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuXD3D.cpp
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
#include "EmuX.h"

using namespace win32;

// ******************************************************************
// * globals
// ******************************************************************
HWND EmuXWindow = NULL;

// ******************************************************************
// * static
// ******************************************************************
static LRESULT WINAPI EmuXMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// ******************************************************************
// * func: EmuXInitD3D
// ******************************************************************
VOID xboxkrnl::EmuXInitD3D()
{
    // ******************************************************************
    // * register window class
    // ******************************************************************
    {
        WNDCLASSEX wc =
        {
            sizeof(WNDCLASSEX),
            CS_CLASSDC,
            EmuXMsgProc,
            0, 0, GetModuleHandle(NULL),
            NULL, NULL, NULL, NULL,
            "CxbxRender",
            NULL
        };

        RegisterClassEx(&wc);
    }

    // ******************************************************************
    // * create the window
    // ******************************************************************
    {
        EmuXWindow = CreateWindow
        (
            "CxbxRender", "Cxbx : Rendering Window",
            WS_OVERLAPPEDWINDOW, 100, 100, 640, 480,
            GetDesktopWindow(), NULL, GetModuleHandle(NULL), NULL
        );
    }

    // ******************************************************************
    // * display the window
    // ******************************************************************
    {
        ShowWindow(EmuXWindow, SW_SHOWDEFAULT);
        UpdateWindow(EmuXWindow);
    }
}

// ******************************************************************
// * func: EmuXMsgProc
// ******************************************************************
LRESULT WINAPI EmuXMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

// ******************************************************************
// * func: EmuXIDirect3D8_CreateDevice
// ******************************************************************
HRESULT xboxkrnl::EmuXIDirect3D8_CreateDevice
(
    UINT                    Adapter,
    D3DDEVTYPE              DeviceType,
    HWND                    hFocusWindow,
    DWORD                   BehaviorFlags,
    D3DPRESENT_PARAMETERS  *pPresentationParameters,
    IDirect3DDevice8      **ppReturnedDeviceInterface
)
{
    MessageBox(NULL, "EmuXIDirect3D8_CreateDevice", "EmuXD3D", MB_OK);
    return 0;
}