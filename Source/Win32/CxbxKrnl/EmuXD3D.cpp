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
LPDIRECT3D8       g_pD3D       = NULL;  // Direct3D8
HWND              g_EmuXWindow = NULL;  // Rendering Window

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
        g_EmuXWindow = CreateWindow
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
        ShowWindow(g_EmuXWindow, SW_SHOWDEFAULT);
        UpdateWindow(g_EmuXWindow);
    }

    // ******************************************************************
    // * create Direct3D8
    // ******************************************************************
    {
        // xbox Direct3DCreate8 returns "1" always, so we need our own ptr
        g_pD3D = Direct3DCreate8(D3D_SDK_VERSION);
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
HRESULT WINAPI xboxkrnl::EmuXIDirect3D8_CreateDevice
(
    UINT                    Adapter,
    D3DDEVTYPE              DeviceType,
    HWND                    hFocusWindow,
    DWORD                   BehaviorFlags,
    D3DPRESENT_PARAMETERS  *pPresentationParameters,
    IDirect3DDevice8      **ppReturnedDeviceInterface
)
{
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXD3D8 (0x%.08X): EmuXIDirect3D8_CreateDevice\n"
               "(\n"
               "   Adapter                   : 0x%.08X\n"
               "   DeviceType                : 0x%.08X\n"
               "   hFocusWindow              : 0x%.08X\n"
               "   BehaviorFlags             : 0x%.08X\n"
               "   pPresentationParameters   : 0x%.08X\n"
               "   ppReturnedDeviceInterface : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Adapter, DeviceType, hFocusWindow,
               BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
    }
    #endif

    // ******************************************************************
    // * make adjustments to parameters to make sense with windows d3d
    // ******************************************************************
    {
        Adapter = D3DADAPTER_DEFAULT;

        pPresentationParameters->Windowed = TRUE;

        hFocusWindow = g_EmuXWindow;

        // Weird swizzled b.s.
        if(pPresentationParameters->BackBufferFormat == 0x07)
            pPresentationParameters->BackBufferFormat = D3DFMT_X8R8G8B8;

        // hrnm this is different too
        if(pPresentationParameters->AutoDepthStencilFormat == 0x2A)
            pPresentationParameters->AutoDepthStencilFormat = D3DFMT_D24S8;
    }

    // ******************************************************************
    // * redirect to windows d3d
    // ******************************************************************
    HRESULT ret = g_pD3D->CreateDevice
    (
        Adapter,
        DeviceType,
        hFocusWindow,
        BehaviorFlags,          // TODO: perhaps allow software vertex processing
        pPresentationParameters,
        ppReturnedDeviceInterface
    );

    EmuXSwapFS();   // XBox FS

    return ret;
}

// ******************************************************************
// * func: EmuXIDirect3D8_CreateDevice
// ******************************************************************
HRESULT WINAPI xboxkrnl::EmuXIDirect3DDevice8_Clear
(
    DWORD           Count,
    CONST D3DRECT  *pRects,
    DWORD           Flags,
    D3DCOLOR        Color,
    float           Z,
    DWORD           Stencil
)
{
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXD3D8 (0x%.08X): EmuXIDirect3DDevice8_Clear\n"
               "(\n"
               "   Count               : 0x%.08X\n"
               "   pRects              : 0x%.08X\n"
               "   Flags               : 0x%.08X\n"
               "   Color               : 0x%.08X\n"
               "   Z                   : 0x%.08X\n"
               "   Stencil             : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Count, pRects, Flags,
               Color, Z, Stencil);
    }
    #endif

    // ******************************************************************
    // * make adjustments to parameters to make sense with windows d3d
    // ******************************************************************
    {
    }

    HRESULT ret = NULL;
    // TODO: Retrieve "this" parameter somehow, and use it as a ptr

    EmuXSwapFS();   // XBox FS

    return ret;
}
