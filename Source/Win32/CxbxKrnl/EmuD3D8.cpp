// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuD3D8.cpp
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
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_LOCAL_

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace xboxkrnl
{
    #include <xboxkrnl/xboxkrnl.h>
};

#include "Emu.h"
#include "EmuFS.h"
#include "EmuKrnl.h"
#include "EmuDInput.h"
#include "EmuShared.h"

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace xd3d8
{
    #include "xd3d8.h"
};

#include "EmuD3D8.h"

#include "ResCxbxDll.h"

#include <process.h>
#include <locale.h>

// ******************************************************************
// * Global(s)
// ******************************************************************
HWND g_hEmuWindow = NULL;   // Rendering Window

// ******************************************************************
// * Static Function(s)
// ******************************************************************
static LRESULT WINAPI EmuMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static DWORD WINAPI   EmuRenderWindow(LPVOID);
static DWORD WINAPI   EmuUpdateTickCount(LPVOID);

// ******************************************************************
// * Static Variable(s)
// ******************************************************************
static xd3d8::LPDIRECT3D8       g_pD3D8         = NULL;   // Direct3D8
static xd3d8::LPDIRECT3DDEVICE8 g_pD3D8Device   = NULL;   // Direct3D8 Device
static Xbe::Header             *g_XbeHeader     = NULL;   // XbeHeader
static uint32                   g_XbeHeaderSize = 0;      // XbeHeaderSize
static xd3d8::D3DCAPS8          g_D3DCaps;                // Direct3D8 Caps
static HBRUSH                   g_hBgBrush      = NULL;   // Background Brush
static volatile bool            g_ThreadInitialized = false;
static XBVideo                  g_XBVideo;

// ******************************************************************
// * EmuD3DDefferedRenderState
// ******************************************************************
DWORD *xd3d8::EmuD3DDefferedRenderState;

// ******************************************************************
// * func: EmuD3DInit
// ******************************************************************
VOID EmuD3DInit(Xbe::Header *XbeHeader, uint32 XbeHeaderSize)
{
    g_EmuShared->GetXBVideo(&g_XBVideo);

    // ******************************************************************
    // * store XbeHeader and XbeHeaderSize for further use
    // ******************************************************************
    g_XbeHeader     = XbeHeader;
    g_XbeHeaderSize = XbeHeaderSize;

    g_ThreadInitialized = false;

    // ******************************************************************
    // * Create a thread dedicated to timing
    // ******************************************************************
    {
        DWORD dwThreadId;

        CreateThread(NULL, NULL, EmuUpdateTickCount, NULL, NULL, &dwThreadId);
    }

    // ******************************************************************
    // * spark up a new thread to handle window message processing
    // ******************************************************************
    {
        DWORD dwThreadId;

        CreateThread(NULL, NULL, EmuRenderWindow, NULL, NULL, &dwThreadId);

        while(!g_ThreadInitialized)
            Sleep(10);

        Sleep(50);
    }

    // ******************************************************************
    // * create Direct3D8 and retrieve caps
    // ******************************************************************
    {
        using namespace xd3d8;

        // xbox Direct3DCreate8 returns "1" always, so we need our own ptr
        g_pD3D8 = Direct3DCreate8(D3D_SDK_VERSION);

        if(g_pD3D8 == NULL)
            EmuCleanup("Could not initialize Direct3D!");

        D3DDEVTYPE DevType = (g_XBVideo.GetDirect3DDevice() == 0) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF;

        g_pD3D8->GetDeviceCaps(g_XBVideo.GetDisplayAdapter(), DevType, &g_D3DCaps);
    }
}

// ******************************************************************
// * func: EmuD3DCleanup
// ******************************************************************
VOID EmuD3DCleanup()
{
    EmuDInputCleanup();

    return;
}

// ******************************************************************
// * func: EmuUpdateTickCount
// ******************************************************************
DWORD WINAPI EmuUpdateTickCount(LPVOID)
{
    timeBeginPeriod(0);

    while(true)
    {
        xboxkrnl::KeTickCount = timeGetTime();
        Sleep(1);
    }

    timeEndPeriod(0);
}

// ******************************************************************
// * func: EmuRenderWindow
// ******************************************************************
DWORD WINAPI EmuRenderWindow(LPVOID)
{
    // ******************************************************************
    // * register window class
    // ******************************************************************
    {
#ifdef _DEBUG
        HMODULE hCxbxDll = GetModuleHandle("CxbxKrnl.dll");
#else
        HMODULE hCxbxDll = GetModuleHandle("Cxbx.dll");
#endif

        LOGBRUSH logBrush = {BS_SOLID, RGB(0,0,0)};

        g_hBgBrush = CreateBrushIndirect(&logBrush);

        WNDCLASSEX wc =
        {
            sizeof(WNDCLASSEX),
            CS_CLASSDC,
            EmuMsgProc,
            0, 0, GetModuleHandle(NULL),
            LoadIcon(hCxbxDll, MAKEINTRESOURCE(IDI_CXBX)),
            LoadCursor(NULL, IDC_ARROW), 
            (HBRUSH)(g_hBgBrush), NULL,
            "CxbxRender",
            NULL
        };

        RegisterClassEx(&wc);
    }

    // ******************************************************************
    // * create the window
    // ******************************************************************
    {
        char AsciiTitle[50];

        // ******************************************************************
        // * retrieve xbe title (if possible)
        // ******************************************************************
        {
            char tAsciiTitle[40] = "Unknown";

            uint32 CertAddr = g_XbeHeader->dwCertificateAddr - g_XbeHeader->dwBaseAddr;

            if(CertAddr + 0x0C + 40 < g_XbeHeaderSize)
            {
                Xbe::Certificate *XbeCert = (Xbe::Certificate*)((uint32)g_XbeHeader + CertAddr);

                setlocale( LC_ALL, "English" );

                wcstombs(tAsciiTitle, XbeCert->wszTitleName, 40);
            }

            sprintf(AsciiTitle, "Cxbx : Emulating %s", tAsciiTitle);
        }

        // ******************************************************************
        // * Create Window
        // ******************************************************************
        {
            DWORD dwStyle = WS_OVERLAPPEDWINDOW;

            int x = 100, y = 100, nWidth = 640, nHeight = 480;

            sscanf(g_XBVideo.GetVideoResolution(), "%d x %d", &nWidth, &nHeight);

            if(g_XBVideo.GetFullscreen())
            {
                x = y = nWidth = nHeight = 0;
                dwStyle = WS_POPUP;
            }

            g_hEmuWindow = CreateWindow
            (
                "CxbxRender", AsciiTitle,
                dwStyle, x, y, nWidth, nHeight,
                GetDesktopWindow(), NULL, GetModuleHandle(NULL), NULL
            );
        }
    }

    ShowWindow(g_hEmuWindow, SW_SHOWDEFAULT);
    UpdateWindow(g_hEmuWindow);

    // ******************************************************************
    // * initialize direct input
    // ******************************************************************
    if(!EmuDInputInit())
        EmuCleanup("Could not initialize DirectInput!");

    // ******************************************************************
    // * message processing loop
    // ******************************************************************
    {
        MSG msg;

        ZeroMemory(&msg, sizeof(msg));

        while(msg.message != WM_QUIT)
        {
            if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
            {
                g_ThreadInitialized = true;

                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
                Sleep(10);
        }

        EmuCleanup(NULL);
    }

    return 0;
}

// ******************************************************************
// * func: EmuMsgProc
// ******************************************************************
LRESULT WINAPI EmuMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_DESTROY:
            DeleteObject(g_hBgBrush);
            PostQuitMessage(0);
            return 0;

        case WM_KEYDOWN:
            if(wParam == VK_ESCAPE)
                PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;

        case WM_CLOSE:
            DestroyWindow(hWnd);
            break;
        case WM_SETCURSOR:
            if(g_XBVideo.GetFullscreen())
            {
                SetCursor(NULL);
                return 0;
            }
            return DefWindowProc(hWnd, msg, wParam, lParam);
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return 0;
}

// ******************************************************************
// * func: EmuIDirect3D8_CreateDevice
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3D8_CreateDevice
(
    UINT                    Adapter,
    D3DDEVTYPE              DeviceType,
    HWND                    hFocusWindow,
    DWORD                   BehaviorFlags,
    D3DPRESENT_PARAMETERS  *pPresentationParameters,
    IDirect3DDevice8      **ppReturnedDeviceInterface
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3D8_CreateDevice\n"
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
        DeviceType =(g_XBVideo.GetDirect3DDevice() == 0) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF;
        Adapter    = g_XBVideo.GetDisplayAdapter();

        pPresentationParameters->Windowed = !g_XBVideo.GetFullscreen();

        if(g_XBVideo.GetVSync())
            pPresentationParameters->SwapEffect = D3DSWAPEFFECT_COPY_VSYNC;

        hFocusWindow = g_hEmuWindow;

        pPresentationParameters->BackBufferFormat       = EmuXB2PC_D3DFormat(pPresentationParameters->BackBufferFormat);
        pPresentationParameters->AutoDepthStencilFormat = EmuXB2PC_D3DFormat(pPresentationParameters->AutoDepthStencilFormat);

        // ******************************************************************
        // * Retrieve Resolution from Configuration
        // ******************************************************************
        if(pPresentationParameters->Windowed)
        {
            D3DDISPLAYMODE D3DDisplayMode;

            sscanf(g_XBVideo.GetVideoResolution(), "%d x %d", &pPresentationParameters->BackBufferWidth, &pPresentationParameters->BackBufferHeight);

            g_pD3D8->GetAdapterDisplayMode(g_XBVideo.GetDisplayAdapter(), &D3DDisplayMode);

            pPresentationParameters->BackBufferFormat = D3DDisplayMode.Format;
            pPresentationParameters->FullScreen_RefreshRateInHz = 0;
        }
        else
        {
            char szBackBufferFormat[16];

            sscanf(g_XBVideo.GetVideoResolution(), "%d x %d %*dbit %s (%d hz)", 
                &pPresentationParameters->BackBufferWidth, 
                &pPresentationParameters->BackBufferHeight,
                szBackBufferFormat,
                &pPresentationParameters->FullScreen_RefreshRateInHz);

            if(strcmp(szBackBufferFormat, "x1r5g5b5") == 0)
                pPresentationParameters->BackBufferFormat = D3DFMT_X1R5G5B5;
            if(strcmp(szBackBufferFormat, "r5g6r5") == 0)
                pPresentationParameters->BackBufferFormat = D3DFMT_R5G6B5;
            if(strcmp(szBackBufferFormat, "x8r8g8b8") == 0)
                pPresentationParameters->BackBufferFormat = D3DFMT_X8R8G8B8;
            if(strcmp(szBackBufferFormat, "a8r8g8b8") == 0)
                pPresentationParameters->BackBufferFormat = D3DFMT_A8R8G8B8;
        }
    }

    // ******************************************************************
    // * Detect vertex processing capabilities
    // ******************************************************************
    if(g_D3DCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT && DeviceType != D3DDEVTYPE_HAL)
    {
        #ifdef _DEBUG_TRACE
        printf("EmuD3D8 (0x%X): Using hardware vertex processing\n", GetCurrentThreadId());
        #endif
        BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    }
    else
    {
        #ifdef _DEBUG_TRACE
        printf("EmuD3D8 (0x%X): Using software vertex processing\n", GetCurrentThreadId());
        #endif
        BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }

    // ******************************************************************
    // * redirect to windows d3d
    // ******************************************************************
    HRESULT hRet = g_pD3D8->CreateDevice
    (
        Adapter,
        DeviceType,
        hFocusWindow,
        BehaviorFlags,
        pPresentationParameters,
        ppReturnedDeviceInterface
    );

    // ******************************************************************
    // * it is necessary to store this pointer globally for emulation
    // ******************************************************************
    g_pD3D8Device = *ppReturnedDeviceInterface;
/* HACK
	LPDIRECT3DTEXTURE8 pCxbxTexture;
	D3DXCreateTextureFromFile(g_pD3D8Device, "Media\\cxbx.bmp", &pCxbxTexture);
	g_pD3D8Device->SetTexture(0, pCxbxTexture);
//*/
    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateTexture
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DDevice8_CreateTexture
(
    UINT                Width,
    UINT                Height,
    UINT                Levels,
    DWORD               Usage,
    D3DFORMAT           Format,
    D3DPOOL             Pool,
    IDirect3DTexture8 **ppTexture
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_CreateTexture\n"
               "(\n"
               "   Width               : 0x%.08X\n"
               "   Height              : 0x%.08X\n"
               "   Levels              : 0x%.08X\n"
               "   Usage               : 0x%.08X\n"
               "   Format              : 0x%.08X\n"
               "   Pool                : 0x%.08X\n"
               "   ppTexture           : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Width, Height, Levels, Usage, Format, Pool, ppTexture);
    }
    #endif

    // Convert Format (Xbox->PC)
    Format = EmuXB2PC_D3DFormat(Format);

    // ******************************************************************
    // * redirect to windows d3d
    // ******************************************************************
    HRESULT hRet = g_pD3D8Device->CreateTexture
    (
        Width, Height, Levels, 
        0,   // TODO: Xbox Allows a border to be drawn (maybe hack this in software ;[)
        Format, D3DPOOL_MANAGED, ppTexture        
    );

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTexture
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DDevice8_SetTexture
(
    DWORD                   Stage,
    IDirect3DBaseTexture8  *pTexture
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetTexture\n"
               "(\n"
               "   Stage               : 0x%.08X\n"
               "   pTexture            : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Stage, pTexture);
    }
    #endif

    HRESULT hRet = g_pD3D8Device->SetTexture(Stage, pTexture);

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetDisplayMode
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DDevice8_GetDisplayMode
(
    X_D3DDISPLAYMODE         *pMode
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetDisplayMode\n"
               "(\n"
               "   pMode               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pMode);
    }
    #endif

    HRESULT hRet;

    // ******************************************************************
    // * make adjustments to parameters to make sense with windows d3d
    // ******************************************************************
    {
        D3DDISPLAYMODE *pPCMode = (D3DDISPLAYMODE*)pMode;

        hRet = g_pD3D8Device->GetDisplayMode(pPCMode);

        // Convert Format (PC->Xbox)
        pMode->Format = EmuPC2XB_D3DFormat(pPCMode->Format);

        // TODO: Make this configurable in the future?
        pMode->Flags  = 0x000000A1; // D3DPRESENTFLAG_FIELD | D3DPRESENTFLAG_INTERLACED | D3DPRESENTFLAG_LOCKABLE_BACKBUFFER
    }

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_Clear
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DDevice8_Clear
(
    DWORD           Count,
    CONST D3DRECT  *pRects,
    DWORD           Flags,
    D3DCOLOR        Color,
    float           Z,
    DWORD           Stencil
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_Clear\n"
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
        // TODO: D3DCLEAR_TARGET_A, *R, *G, *B don't exist on windows
        DWORD newFlags = 0;

        if(Flags & 0x000000f0l)
            newFlags |= D3DCLEAR_TARGET;

        if(Flags & 0x00000001l)
            newFlags |= D3DCLEAR_ZBUFFER;

        if(Flags & 0x00000002l)
            newFlags |= D3DCLEAR_STENCIL;

        Flags = newFlags;
    }

    HRESULT ret = g_pD3D8Device->Clear(Count, pRects, Flags, Color, Z, Stencil);

    EmuSwapFS();   // XBox FS

    return ret;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_Present
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DDevice8_Present
(
    CONST RECT* pSourceRect,
    CONST RECT* pDestRect,
    PVOID       pDummy1,
    PVOID       pDummy2
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_Present\n"
               "(\n"
               "   pSourceRect         : 0x%.08X\n"
               "   pDestRect           : 0x%.08X\n"
               "   pDummy1             : 0x%.08X\n"
               "   pDummy2             : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pSourceRect, pDestRect, pDummy1, pDummy2);
    }
    #endif

    HRESULT ret = g_pD3D8Device->Present(pSourceRect, pDestRect, (HWND)pDummy1, (CONST RGNDATA*)pDummy2);

    EmuSwapFS();   // XBox FS

    return ret;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_Swap
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DDevice8_Swap
(
    DWORD Flags
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_Swap\n"
               "(\n"
               "   Flags               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Flags);
    }
    #endif

    // TODO: Ensure this flag is always the same across library versions
    if(Flags != 0)
        EmuCleanup("xd3d8::EmuIDirect3DDevice8_Swap: Flags != 0");

    // Swap(0) is equivalent to present(0,0,0,0)
    HRESULT ret = g_pD3D8Device->Present(0, 0, 0, 0);

    EmuSwapFS();   // XBox FS

    return ret;
}

// ******************************************************************
// * func: EmuIDirect3DResource8_Release
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DResource8_Release
(
    PVOID               pThis
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DResource8_Release\n"
               "(\n"
               "   pThis               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis);
    }
    #endif

	HRESULT hRet = ((IDirect3DResource8*)pThis)->Release();

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DSurface8_GetDesc
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DSurface8_GetDesc
(
    PVOID               pThis,
    X_D3DSURFACE_DESC  *pDesc
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DSurface8_GetDesc\n"
               "(\n"
               "   pThis               : 0x%.08X\n"
               "   pDesc               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, pDesc);
    }
    #endif

	D3DSURFACE_DESC SurfaceDesc;

	HRESULT hRet = ((IDirect3DSurface8*)pThis)->GetDesc(&SurfaceDesc);

    // ******************************************************************
    // * Rearrange into windows format (remove D3DPool)
    // ******************************************************************
    {
        // Convert Format (PC->Xbox)
        pDesc->Format = EmuPC2XB_D3DFormat(SurfaceDesc.Format);
        pDesc->Type   = SurfaceDesc.Type;

        if(pDesc->Type > 7)
            EmuCleanup("EmuIDirect3DSurface8_GetDesc: pDesc->Type > 7");

        pDesc->Usage  = SurfaceDesc.Usage;
        pDesc->Size   = SurfaceDesc.Size;

        // TODO: Convert from Xbox to PC!!
        if(SurfaceDesc.MultiSampleType == D3DMULTISAMPLE_NONE)
            pDesc->MultiSampleType = (xd3d8::D3DMULTISAMPLE_TYPE)0x0011;
        else
            EmuCleanup("EmuIDirect3DSurface8_GetDesc Unknown Multisample format!");

        pDesc->Width  = SurfaceDesc.Width;
        pDesc->Height = SurfaceDesc.Height;
    }

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DSurface8_LockRect
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DSurface8_LockRect
(
    PVOID               pThis,
    D3DLOCKED_RECT     *pLockedRect,
    CONST RECT         *pRect,
    DWORD               Flags
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DSurface8_LockRect\n"
               "(\n"
               "   pThis               : 0x%.08X\n"
               "   pLockedRect         : 0x%.08X\n"
               "   pRect               : 0x%.08X\n"
               "   Flags               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, pLockedRect, pRect, Flags);
    }
    #endif

    DWORD NewFlags = 0;

    if(Flags & 0x40)
        printf("EmuIDirect3DSurface8_LockRect: *Warning* D3DLOCK_TILED ignored!\n");

    if(Flags & 0x80)
        NewFlags |= D3DLOCK_READONLY;

    if(Flags & !(0x80 | 0x40))
        EmuCleanup("EmuIDirect3DSurface8_LockRect: Unknown Flags!");

    // This is lame, but we have no choice (afaik)
    ((IDirect3DSurface8*)pThis)->UnlockRect();

    HRESULT hRet = ((IDirect3DSurface8*)pThis)->LockRect(pLockedRect, pRect, NewFlags);

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DBaseTexture8_GetLevelCount
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DBaseTexture8_GetLevelCount
(
    PVOID               pThis
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DBaseTexture8_GetLevelCount\n"
               "(\n"
               "   pThis               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis);
    }
    #endif

    HRESULT hRet = ((IDirect3DBaseTexture8*)pThis)->GetLevelCount();

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DTexture8_GetSurfaceLevel
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DTexture8_GetSurfaceLevel
(
    PVOID               pThis,
    UINT                Level,
    IDirect3DSurface8 **ppSurfaceLevel
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DTexture8_GetSurfaceLevel\n"
               "(\n"
               "   pThis               : 0x%.08X\n"
               "   Level               : 0x%.08X\n"
               "   ppSurfaceLevel      : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, Level, ppSurfaceLevel);
    }
    #endif

    HRESULT hRet = ((IDirect3DTexture8*)pThis)->GetSurfaceLevel(Level, ppSurfaceLevel);

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateVertexBuffer
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DDevice8_CreateVertexBuffer
(
    UINT              Length,
    DWORD             Usage,
    DWORD             FVF,
    D3DPOOL           Pool,
    D3DVertexBuffer **ppVertexBuffer
)
{
    *ppVertexBuffer = EmuIDirect3DDevice8_CreateVertexBuffer2(Length);

    return D3D_OK;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateVertexBuffer2
// ******************************************************************
xd3d8::D3DVertexBuffer* WINAPI xd3d8::EmuIDirect3DDevice8_CreateVertexBuffer2
(
    UINT Length
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_CreateVertexBuffer2\n"
               "(\n"
               "   Length              : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Length);
    }
    #endif

    IDirect3DVertexBuffer8 *ppVertexBuffer=NULL;

    HRESULT hRet = g_pD3D8Device->CreateVertexBuffer
    (
        Length, 
        D3DUSAGE_WRITEONLY,
        0,
        D3DPOOL_DEFAULT, 
        &ppVertexBuffer
    );

    EmuSwapFS();   // XBox FS

    return (xd3d8::D3DVertexBuffer*)ppVertexBuffer;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_CullMode
// ******************************************************************
VOID WINAPI xd3d8::EmuIDirect3DDevice8_SetRenderState_CullMode
(
    DWORD Value
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_CullMode\n"
               "(\n"
               "   Value               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Value);
    }
    #endif

    // ******************************************************************
    // * Convert from Xbox D3D to PC D3D enumeration
    // ******************************************************************
    // TODO: XDK-Specific Tables? So far they are the same
    switch(Value)
    {
        case 0:
            Value = D3DCULL_NONE;
            break;
        case 0x900:
            Value = D3DCULL_CW;
            break;
        case 0x901:
            Value = D3DCULL_CCW;
            break;
        default:
            EmuCleanup("EmuIDirect3DDevice8_SetRenderState_CullMode: Unknown Cullmode");
    }

    g_pD3D8Device->SetRenderState(D3DRS_CULLMODE, Value);

    EmuSwapFS();   // XBox FS

    return;
}


// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_ZEnable
// ******************************************************************
VOID WINAPI xd3d8::EmuIDirect3DDevice8_SetRenderState_ZEnable
(
    DWORD Value
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_ZEnable\n"
               "(\n"
               "   Value               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Value);
    }
    #endif

    g_pD3D8Device->SetRenderState(D3DRS_ZENABLE, Value);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTransform
// ******************************************************************
VOID WINAPI xd3d8::EmuIDirect3DDevice8_SetTransform
(
    D3DTRANSFORMSTATETYPE State,
    CONST D3DMATRIX      *pMatrix
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetTransform\n"
               "(\n"
               "   State               : 0x%.08X\n"
               "   pMatrix             : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), State, pMatrix);
    }
    #endif

    // ******************************************************************
    // * Convert from Xbox D3D to PC D3D enumeration
    // ******************************************************************
    // TODO: XDK-Specific Tables? So far they are the same
    if((uint32)State < 2)
        State = (D3DTRANSFORMSTATETYPE)(State + 2);
    else if((uint32)State < 6)
        State = (D3DTRANSFORMSTATETYPE)(State + 14);
    else if((uint32)State < 9)
        State = D3DTS_WORLDMATRIX(State-6);

    g_pD3D8Device->SetTransform(State, pMatrix);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DVertexBuffer8_Lock
// ******************************************************************
VOID WINAPI xd3d8::EmuIDirect3DVertexBuffer8_Lock
(
    D3DVertexBuffer *ppVertexBuffer,
    UINT             OffsetToLock,
    UINT             SizeToLock,
    BYTE           **ppbData,
    DWORD            Flags
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DVertexBuffer8_Lock\n"
               "(\n"
               "   ppVertexBuffer      : 0x%.08X\n"
               "   OffsetToLock        : 0x%.08X\n"
               "   SizeToLock          : 0x%.08X\n"
               "   ppbData             : 0x%.08X\n"
               "   Flags               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), ppVertexBuffer, OffsetToLock, SizeToLock, ppbData, Flags);
    }
    #endif

    HRESULT hRet = ((IDirect3DVertexBuffer8*)ppVertexBuffer)->Lock(OffsetToLock, SizeToLock, ppbData, Flags);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DVertexBuffer8_Lock2
// ******************************************************************
BYTE* WINAPI xd3d8::EmuIDirect3DVertexBuffer8_Lock2
(
    D3DVertexBuffer *ppVertexBuffer,
    DWORD            Flags
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DVertexBuffer8_Lock2\n"
               "(\n"
               "   ppVertexBuffer      : 0x%.08X\n"
               "   Flags               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), ppVertexBuffer, Flags);
    }
    #endif

    BYTE *pbData = NULL;

    HRESULT hRet = ((IDirect3DVertexBuffer8*)ppVertexBuffer)->Lock(0, 0, &pbData, Flags);

    EmuSwapFS();   // XBox FS

    return pbData;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetStreamSource
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DDevice8_SetStreamSource
(
    UINT             StreamNumber,
    D3DVertexBuffer *pStreamData,
    UINT             Stride
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetStreamSource\n"
               "(\n"
               "   StreamNumber        : 0x%.08X\n"
               "   pStreamData         : 0x%.08X\n"
               "   Stride              : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), StreamNumber, pStreamData, Stride);
    }
    #endif

    ((IDirect3DVertexBuffer8*)pStreamData)->Unlock();

    HRESULT hRet = g_pD3D8Device->SetStreamSource(StreamNumber, (IDirect3DVertexBuffer8*)pStreamData, Stride);

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexShader
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DDevice8_SetVertexShader
(
    DWORD Handle
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetVertexShader\n"
               "(\n"
               "   Handle              : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Handle);
    }
    #endif

    HRESULT hRet = g_pD3D8Device->SetVertexShader(Handle);

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_DrawVertices
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DDevice8_DrawVertices
(
    D3DPRIMITIVETYPE PrimitiveType,
    UINT             StartVertex,
    UINT             VertexCount
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_DrawVertices\n"
               "(\n"
               "   PrimitiveType       : 0x%.08X\n"
               "   StartVertex         : 0x%.08X\n"
               "   VertexCount         : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), PrimitiveType, StartVertex, VertexCount);
    }
    #endif

    // Certain D3DRS values need to be checked on each Draw[Indexed]Vertices
    g_pD3D8Device->SetRenderState(D3DRS_LIGHTING,              xd3d8::EmuD3DDefferedRenderState[10]);
    g_pD3D8Device->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, xd3d8::EmuD3DDefferedRenderState[20]);
    g_pD3D8Device->SetRenderState(D3DRS_AMBIENT,               xd3d8::EmuD3DDefferedRenderState[23]);

    UINT PrimitiveCount = D3DVertex2PrimitiveCount(PrimitiveType, VertexCount);

    // Convert from Xbox to PC enumeration
    PrimitiveType = EmuPrimitiveType(PrimitiveType);

    HRESULT hRet = g_pD3D8Device->DrawPrimitive
    (
        PrimitiveType,
        StartVertex,
        PrimitiveCount
    );

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetLight
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DDevice8_SetLight
(
    DWORD            Index,
    CONST D3DLIGHT8 *pLight
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetLight\n"
               "(\n"
               "   Index               : 0x%.08X\n"
               "   pLight              : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Index, pLight);
    }
    #endif

    HRESULT hRet = g_pD3D8Device->SetLight(Index, pLight);

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetMaterial
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DDevice8_SetMaterial
(
    CONST D3DMATERIAL8 *pMaterial
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetMaterial\n"
               "(\n"
               "   pMaterial           : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pMaterial);
    }
    #endif

    HRESULT hRet = g_pD3D8Device->SetMaterial(pMaterial);

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_LightEnable
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DDevice8_LightEnable
(
    DWORD            Index,
    BOOL             bEnable
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_LightEnable\n"
               "(\n"
               "   Index               : 0x%.08X\n"
               "   bEnable             : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Index, bEnable);
    }
    #endif

    HRESULT hRet = g_pD3D8Device->LightEnable(Index, bEnable);

    EmuSwapFS();   // XBox FS
    
    return hRet;
}
