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

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace xg
{
    #include "EmuXG.h"
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
static DWORD WINAPI   EmuRenderWindow(LPVOID);
static LRESULT WINAPI EmuMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static DWORD WINAPI   EmuUpdateTickCount(LPVOID);
static DWORD          EmuCheckAllocationSize(LPVOID);
static inline void    EmuVerifyResourceIsRegistered(xd3d8::X_D3DResource *pResource);

// ******************************************************************
// * Static Variable(s)
// ******************************************************************
static xd3d8::LPDIRECT3D8       g_pD3D8         = NULL;   // Direct3D8
static xd3d8::LPDIRECT3DDEVICE8 g_pD3DDevice8   = NULL;   // Direct3D8 Device
static Xbe::Header             *g_XbeHeader     = NULL;   // XbeHeader
static uint32                   g_XbeHeaderSize = 0;      // XbeHeaderSize
static xd3d8::D3DCAPS8          g_D3DCaps;                // Direct3D8 Caps
static HBRUSH                   g_hBgBrush      = NULL;   // Background Brush
static volatile bool            g_bThreadInitialized = false;
static XBVideo                  g_XBVideo;

// ******************************************************************
// * Cached Direct3D State Variable(s)
// ******************************************************************
static xd3d8::X_D3DSurface     *g_pCachedRenderTarget = NULL;
static xd3d8::X_D3DSurface     *g_pCachedZStencilSurface = NULL;
static DWORD                    g_dwVertexShaderUsage = 0;

// ******************************************************************
// * EmuD3DTiles (8 Tiles Max)
// ******************************************************************
xd3d8::X_D3DTILE xd3d8::EmuD3DTileCache[0x08] = {0};

// ******************************************************************
// * EmuD3DDeferredRenderState
// ******************************************************************
DWORD *xd3d8::EmuD3DDeferredRenderState;

// ******************************************************************
// * EmuD3DDeferredTextureState
// ******************************************************************
DWORD *xd3d8::EmuD3DDeferredTextureState;

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

    g_bThreadInitialized = false;

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

        while(!g_bThreadInitialized)
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
            EmuCleanup("Could not initialize Direct3D8!");

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
// * func: EmuRenderWindow
// ******************************************************************
static DWORD WINAPI EmuRenderWindow(LPVOID)
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
                g_bThreadInitialized = true;

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
static LRESULT WINAPI EmuMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
// * func: EmuUpdateTickCount
// ******************************************************************
static DWORD WINAPI EmuUpdateTickCount(LPVOID)
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
// * func: EmuCheckAllocationSize
// ******************************************************************
static DWORD EmuCheckAllocationSize(PVOID pBase)
{
    MEMORY_BASIC_INFORMATION MemoryBasicInfo;

    DWORD dwRet = VirtualQuery(pBase, &MemoryBasicInfo, sizeof(MemoryBasicInfo));

    if(dwRet == 0)
        return 0;

    if(MemoryBasicInfo.State != MEM_COMMIT)
        return 0;

    return MemoryBasicInfo.RegionSize - ((DWORD)pBase - (DWORD)MemoryBasicInfo.BaseAddress);
}

// ******************************************************************
// * func: EmuVerifyResourceIsRegistered
// ******************************************************************
static inline void EmuVerifyResourceIsRegistered(xd3d8::X_D3DResource *pResource)
{
    if(pResource->Lock == 0)
    {
        EmuSwapFS();    // XBox FS;
        xd3d8::EmuIDirect3DResource8_Register(pResource, (PVOID)pResource->Data);
        EmuSwapFS();    // Win2k/XP FS
    }
}

// ******************************************************************
// * func: EmuIDirect3D8_CreateDevice
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3D8_CreateDevice
(
    UINT                        Adapter,
    D3DDEVTYPE                  DeviceType,
    HWND                        hFocusWindow,
    DWORD                       BehaviorFlags,
    X_D3DPRESENT_PARAMETERS    *pPresentationParameters,
    IDirect3DDevice8          **ppReturnedDeviceInterface
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
    // * verify no ugly circumstances
    // ******************************************************************
    if(pPresentationParameters->BufferSurfaces[0] != NULL || pPresentationParameters->DepthStencilSurface != NULL)
        EmuCleanup("EmuIDirect3D8_CreateDevice: BufferSurfaces or DepthStencilSurface != NULL");

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

        // TODO: This should be detected from D3DCAPS8 ? (FrameSkip?)
        pPresentationParameters->FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

        // TODO: Support Xbox extensions if possible
        if(pPresentationParameters->MultiSampleType != 0)
        {
            printf("*Warning* MultiSampleType (0x%.08X) Not Supported!\n", pPresentationParameters->MultiSampleType);

            pPresentationParameters->MultiSampleType = D3DMULTISAMPLE_NONE;

            // TODO: Check card for multisampling abilities
//            if(pPresentationParameters->MultiSampleType == 0x00001121)
//                pPresentationParameters->MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
//            else
//                EmuCleanup("Unknown MultiSampleType (0x%.08X)", pPresentationParameters->MultiSampleType);
        }

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
            else if(strcmp(szBackBufferFormat, "r5g6r5") == 0)
                pPresentationParameters->BackBufferFormat = D3DFMT_R5G6B5;
            else if(strcmp(szBackBufferFormat, "x8r8g8b8") == 0)
                pPresentationParameters->BackBufferFormat = D3DFMT_X8R8G8B8;
            else if(strcmp(szBackBufferFormat, "a8r8g8b8") == 0)
                pPresentationParameters->BackBufferFormat = D3DFMT_A8R8G8B8;
        }
    }

    // ******************************************************************
    // * Detect vertex processing capabilities
    // ******************************************************************
    if((g_D3DCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) && DeviceType == D3DDEVTYPE_HAL)
    {
        #ifdef _DEBUG_TRACE
        printf("EmuD3D8 (0x%X): Using hardware vertex processing\n", GetCurrentThreadId());
        #endif
        BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
        g_dwVertexShaderUsage = 0;
    }
    else
    {
        #ifdef _DEBUG_TRACE
        printf("EmuD3D8 (0x%X): Using software vertex processing\n", GetCurrentThreadId());
        #endif
        BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        g_dwVertexShaderUsage = D3DUSAGE_SOFTWAREPROCESSING;
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
        (xd3d8::D3DPRESENT_PARAMETERS*)pPresentationParameters,
        ppReturnedDeviceInterface
    );

    // ******************************************************************
    // * it is necessary to store this pointer globally for emulation
    // ******************************************************************
    g_pD3DDevice8 = *ppReturnedDeviceInterface;

    // ******************************************************************
    // * Update Caches
    // ******************************************************************
    {
        g_pCachedRenderTarget = new X_D3DSurface();
        g_pD3DDevice8->GetRenderTarget(&g_pCachedRenderTarget->EmuSurface8);

        g_pCachedZStencilSurface = new X_D3DSurface();
        g_pD3DDevice8->GetDepthStencilSurface(&g_pCachedZStencilSurface->EmuSurface8);
    }

    // ******************************************************************
    // * Begin Scene
    // ******************************************************************
    g_pD3DDevice8->BeginScene();


    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3D8_GetAdapterDisplayMode
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3D8_GetAdapterDisplayMode
(
    UINT                        Adapter,
    X_D3DDISPLAYMODE           *pMode
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3D8_GetAdapterDisplayMode\n"
               "(\n"
               "   Adapter                   : 0x%.08X\n"
               "   pMode                     : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Adapter, pMode);
    }
    #endif

    // NOTE: WARNING: We should cache the "Emulated" display mode and return
    // This value. We can initialize the cache with the default Xbox mode data.

    // ******************************************************************
    // * redirect to windows d3d
    // ******************************************************************
    HRESULT hRet = g_pD3D8->GetAdapterDisplayMode
    (
        g_XBVideo.GetDisplayAdapter(),
        (D3DDISPLAYMODE*)pMode
    );

    // ******************************************************************
    // * make adjustments to parameters to make sense with windows d3d
    // ******************************************************************
    {
        D3DDISPLAYMODE *pPCMode = (D3DDISPLAYMODE*)pMode;

        // Convert Format (PC->Xbox)
        pMode->Format = EmuPC2XB_D3DFormat(pPCMode->Format);

        // TODO: Make this configurable in the future?
        // D3DPRESENTFLAG_FIELD | D3DPRESENTFLAG_INTERLACED | D3DPRESENTFLAG_LOCKABLE_BACKBUFFER
        pMode->Flags  = 0x000000A1;
    }

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3D8_KickOffAndWaitForIdle
// ******************************************************************
VOID WINAPI xd3d8::EmuIDirect3D8_KickOffAndWaitForIdle()
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3D8_KickOffAndWaitForIdle()\n", GetCurrentThreadId());
    }
    #endif

    // TODO: Actually do something here?

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_CopyRects
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DDevice8_CopyRects
(
    X_D3DSurface       *pSourceSurface,
    CONST RECT         *pSourceRectsArray,
    UINT                cRects,
    X_D3DSurface       *pDestinationSurface,
    CONST POINT        *pDestPointsArray
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_CopyRects\n"
               "(\n"
               "   pSourceSurface      : 0x%.08X\n"
               "   pSourceRectsArray   : 0x%.08X\n"
               "   cRects              : 0x%.08X\n"
               "   pDestinationSurface : 0x%.08X\n"
               "   pDestPointsArray    : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pSourceSurface, pSourceRectsArray, cRects,
               pDestinationSurface, pDestPointsArray);
    }
    #endif

    // ******************************************************************
    // * Redirect to PC D3D
    // ******************************************************************
    HRESULT hRet = g_pD3DDevice8->CopyRects
    (
        pSourceSurface->EmuSurface8, 
        pSourceRectsArray,
        cRects,
        pDestinationSurface->EmuSurface8,
        pDestPointsArray
    );

    EmuSwapFS();   // Xbox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateImageSurface
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DDevice8_CreateImageSurface
(
    UINT                Width,
    UINT                Height,
    X_D3DFORMAT         Format,
    X_D3DSurface      **ppBackBuffer
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_CreateImageSurface\n"
               "(\n"
               "   Width               : 0x%.08X\n"
               "   Height              : 0x%.08X\n"
               "   Format              : 0x%.08X\n"
               "   ppBackBuffer        : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Width, Height, Format, ppBackBuffer);
    }
    #endif

    *ppBackBuffer = new X_D3DSurface();

    D3DFORMAT PCFormat = EmuXB2PC_D3DFormat(Format);

    HRESULT hRet = g_pD3DDevice8->CreateImageSurface(Width, Height, PCFormat, &((*ppBackBuffer)->EmuSurface8));

    EmuSwapFS();   // Xbox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetBackBuffer
// ******************************************************************
VOID WINAPI xd3d8::EmuIDirect3DDevice8_GetBackBuffer
(
    INT                 BackBuffer,
    D3DBACKBUFFER_TYPE  Type,
    X_D3DSurface      **ppBackBuffer
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetBackBuffer\n"
               "(\n"
               "   BackBuffer          : 0x%.08X\n"
               "   Type                : 0x%.08X\n"
               "   ppBackBuffer        : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), BackBuffer, Type, ppBackBuffer);
    }
    #endif

    *ppBackBuffer = new X_D3DSurface();

    g_pD3DDevice8->GetBackBuffer(BackBuffer, D3DBACKBUFFER_TYPE_MONO, &((*ppBackBuffer)->EmuSurface8));

    EmuSwapFS();   // Xbox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetRenderTarget
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DDevice8_GetRenderTarget
(
    X_D3DSurface  **ppRenderTarget
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        EmuSwapFS();   // Win2k/XP FS
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetRenderTarget\n"
               "(\n"
               "   ppRenderTarget      : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), ppRenderTarget);
        EmuSwapFS();   // Xbox FS
    }
    #endif

    IDirect3DSurface8 *pSurface8 = g_pCachedRenderTarget->EmuSurface8;

    pSurface8->AddRef();

    *ppRenderTarget = g_pCachedRenderTarget;

    EmuSwapFS();   // Xbox FS

    return D3D_OK;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetRenderTarget2
// ******************************************************************
xd3d8::X_D3DSurface * WINAPI xd3d8::EmuIDirect3DDevice8_GetRenderTarget2()
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetRenderTarget2()\n",
               GetCurrentThreadId());
    }
    #endif

    IDirect3DSurface8 *pSurface8 = g_pCachedRenderTarget->EmuSurface8;

    pSurface8->AddRef();

    EmuSwapFS();   // Xbox FS

    return g_pCachedRenderTarget;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetDepthStencilSurface
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DDevice8_GetDepthStencilSurface
(
    X_D3DSurface  **ppZStencilSurface
)
{
    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        EmuSwapFS();   // Win2k/XP FS
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetDepthStencilSurface\n"
               "(\n"
               "   ppZStencilSurface   : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), ppZStencilSurface);
        EmuSwapFS();   // Xbox FS
    }
    #endif

    IDirect3DSurface8 *pSurface8 = g_pCachedZStencilSurface->EmuSurface8;

    pSurface8->AddRef();

    *ppZStencilSurface = g_pCachedZStencilSurface;

    return D3D_OK;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetDepthStencilSurface2
// ******************************************************************
xd3d8::X_D3DSurface * WINAPI xd3d8::EmuIDirect3DDevice8_GetDepthStencilSurface2()
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetDepthStencilSurface2()\n",
               GetCurrentThreadId());
    }
    #endif

    IDirect3DSurface8 *pSurface8 = g_pCachedZStencilSurface->EmuSurface8;

    pSurface8->AddRef();

    EmuSwapFS();   // Xbox FS

    return g_pCachedZStencilSurface;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetTile
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DDevice8_GetTile
(
    DWORD           Index,
    X_D3DTILE      *pTile
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetTile\n"
               "(\n"
               "   Index               : 0x%.08X\n"
               "   pTile               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Index, pTile);
    }
    #endif

    if(pTile != NULL)
        memcpy(pTile, &EmuD3DTileCache[Index], sizeof(X_D3DTILE));

    EmuSwapFS();   // XBox FS

    return D3D_OK;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTileNoWait
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DDevice8_SetTileNoWait
(
    DWORD               Index,
    CONST X_D3DTILE    *pTile
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetTileNoWait\n"
               "(\n"
               "   Index               : 0x%.08X\n"
               "   pTile               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Index, pTile);
    }
    #endif

    if(pTile != NULL)
        memcpy(&EmuD3DTileCache[Index], pTile, sizeof(X_D3DTILE));

    EmuSwapFS();   // XBox FS

    return D3D_OK;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateVertexShader
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DDevice8_CreateVertexShader
(
    CONST DWORD    *pDeclaration,
    CONST DWORD    *pFunction,
    DWORD          *pHandle,
    DWORD           Usage
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_CreateVertexShader\n"
               "(\n"
               "   pDeclaration        : 0x%.08X\n"
               "   pFunction           : 0x%.08X\n"
               "   pHandle             : 0x%.08X\n"
               "   Usage               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pDeclaration, pFunction, pHandle, Usage);
    }
    #endif

    // ******************************************************************
    // * create emulated shader struct
    // ******************************************************************
    X_D3DVertexShader *pD3DVertexShader = new X_D3DVertexShader();

    // Todo: Intelligently fill out these fields as necessary
    ZeroMemory(pD3DVertexShader, sizeof(X_D3DVertexShader));

    // ******************************************************************
    // * redirect to windows d3d
    // ******************************************************************
    HRESULT hRet = g_pD3DDevice8->CreateVertexShader
    (
        pDeclaration,
        pFunction,
        &pD3DVertexShader->Handle,
        g_dwVertexShaderUsage   // TODO: HACK: Xbox has extensions!
    );

    *pHandle = (DWORD)pD3DVertexShader;

    if(FAILED(hRet))
    {
        printf("*Warning* we're lying about the creation of a vertex shader!\n");

        hRet = D3D_OK;
    }

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexShaderConstant
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DDevice8_SetVertexShaderConstant
(
    INT         Register,
    CONST PVOID pConstantData,
    DWORD       ConstantCount
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetVertexShaderConstant\n"
               "(\n"
               "   Register            : 0x%.08X\n"
               "   pConstantData       : 0x%.08X\n"
               "   ConstantCount       : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Register, pConstantData, ConstantCount);
    }
    #endif

    // ******************************************************************
    // * redirect to windows d3d
    // ******************************************************************
    HRESULT hRet = g_pD3DDevice8->SetVertexShaderConstant
    (
        Register,
        pConstantData,
        ConstantCount
    );

    if(FAILED(hRet))
    {
        printf("*Warning* we're lying about setting a vertex shader constant!\n");

        hRet = D3D_OK;
    }

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreatePixelShader
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DDevice8_CreatePixelShader
(
    CONST DWORD    *pFunction,
    DWORD          *pHandle
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_CreatePixelShader\n"
               "(\n"
               "   pFunction           : 0x%.08X\n"
               "   pHandle             : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pFunction, pHandle);
    }
    #endif

    // ******************************************************************
    // * redirect to windows d3d
    // ******************************************************************
    HRESULT hRet = g_pD3DDevice8->CreatePixelShader
    (
        pFunction,
        pHandle
    );

    if(FAILED(hRet))
    {
        printf("*Warning* we're lying about the creation of a pixel shader!\n");

        hRet = D3D_OK;
    }

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetPixelShader
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DDevice8_SetPixelShader
(
    DWORD           Handle
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetPixelShader\n"
               "(\n"
               "   Handle             : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Handle);
    }
    #endif

    // ******************************************************************
    // * redirect to windows d3d
    // ******************************************************************
    HRESULT hRet = g_pD3DDevice8->SetPixelShader
    (
        Handle
    );

    if(FAILED(hRet))
    {
        printf("*Warning* we're lying about setting a pixel shader!\n");

        hRet = D3D_OK;
    }

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateTexture2
// ******************************************************************
xd3d8::X_D3DResource * WINAPI xd3d8::EmuIDirect3DDevice8_CreateTexture2
(
    UINT                Width,
    UINT                Height,
    UINT                Depth,
    UINT                Levels,
    DWORD               Usage,
    D3DFORMAT           Format,
    D3DRESOURCETYPE     D3DResource
)
{
    X_D3DResource *pTexture;

    EmuIDirect3DDevice8_CreateTexture(Width, Height, Levels, Usage, Format, D3DPOOL_MANAGED, &pTexture);

    return pTexture;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateTexture
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DDevice8_CreateTexture
(
    UINT            Width,
    UINT            Height,
    UINT            Levels,
    DWORD           Usage,
    D3DFORMAT       Format,
    D3DPOOL         Pool,
    X_D3DResource **ppTexture
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
    D3DFORMAT PCFormat = EmuXB2PC_D3DFormat(Format);

    // TODO: HACK: Devices that don't support this should somehow emulate it!
    if(PCFormat == D3DFMT_D16)
    {
        printf("*Warning* D3DFMT_16 is an unsupported texture format!\n");
        PCFormat = D3DFMT_X8R8G8B8;
    }

    *ppTexture = new X_D3DResource();

    // ******************************************************************
    // * redirect to windows d3d
    // ******************************************************************
    HRESULT hRet = g_pD3DDevice8->CreateTexture
    (
        Width, Height, Levels, 
        0,  // TODO: Xbox Allows a border to be drawn (maybe hack this in software ;[)
        PCFormat, D3DPOOL_MANAGED, &((*ppTexture)->EmuTexture8)
    );

    if(FAILED(hRet))
        printf("*Warning* CreateTexture FAILED\n");

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetIndices
// ******************************************************************
VOID WINAPI xd3d8::EmuIDirect3DDevice8_SetIndices
(
    X_D3DIndexBuffer   *pIndexData,
    UINT                BaseVertexIndex
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetIndices\n"
               "(\n"
               "   pIndexData          : 0x%.08X\n"
               "   BaseVertexIndex     : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pIndexData, BaseVertexIndex);
    }
    #endif

    EmuVerifyResourceIsRegistered(pIndexData);

    IDirect3DIndexBuffer8 *pIndexBuffer = pIndexData->EmuIndexBuffer8;

    g_pD3DDevice8->SetIndices(pIndexBuffer, BaseVertexIndex);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTexture
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DDevice8_SetTexture
(
    DWORD           Stage,
    X_D3DResource  *pTexture
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

    IDirect3DBaseTexture8 *pBaseTexture8 = NULL;

    if(pTexture != NULL)
    {
        EmuVerifyResourceIsRegistered(pTexture);
        pBaseTexture8 = pTexture->EmuBaseTexture8;
    }

    HRESULT hRet = g_pD3DDevice8->SetTexture(Stage, pBaseTexture8);

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SwitchTexture
// ******************************************************************
VOID __fastcall xd3d8::EmuIDirect3DDevice8_SwitchTexture
(
    DWORD           Method,
    DWORD           Data,
    DWORD           Format
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SwitchTexture\n"
               "(\n"
               "   Method              : 0x%.08X\n"
               "   Data                : 0x%.08X\n"
               "   Format              : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Method, Data, Format);
    }
    #endif

    EmuCleanup("EmuIDirect3DDevice8_SwitchTexture is not implemented!");
/*** 
    IDirect3DBaseTexture8 *pBaseTexture8 = pTexture->EmuBaseTexture8;
    IDirect3DBaseTexture8 *pPrevTexture8 = NULL;
    
    // Xbox SwitchTexture does not decrement the reference count on the
    // old texture, but SetTexture does, so we need to pre-increment
    g_pD3DDevice8->GetTexture(Stage, &pPrevTexture8);

    HRESULT hRet = g_pD3DDevice8->SetTexture(Stage, pBaseTexture8);

    // Xbox SwitchTexture does not increment reference count, but the
    // above SetTexture does, so we need to remove it.
    pBaseTexture8->Release();
***/
    EmuSwapFS();   // XBox FS

    return;
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

        hRet = g_pD3DDevice8->GetDisplayMode(pPCMode);

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

    HRESULT ret = g_pD3DDevice8->Clear(Count, pRects, Flags, Color, Z, Stencil);

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

    HRESULT hRet = g_pD3DDevice8->Present(pSourceRect, pDestRect, (HWND)pDummy1, (CONST RGNDATA*)pDummy2);

    EmuSwapFS();   // XBox FS

    return hRet;
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

    HRESULT hRet = g_pD3DDevice8->Present(0, 0, 0, 0);

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DResource8_Register
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DResource8_Register
(
    X_D3DResource      *pThis,
    PVOID               pBase
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DResource8_Register\n"
               "(\n"
               "   pThis               : 0x%.08X\n"
               "   pBase               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, pBase);
    }
    #endif
/** i don't know about this
    static DWORD dwStaticBase = -1, dwTrueBase = -1, dwBaseIncrement;

    if((DWORD)pBase == dwStaticBase)
    {
        dwTrueBase += dwBaseIncrement;
        pBase = (PVOID)dwTrueBase;
    }
    else
    {
        dwStaticBase = dwTrueBase = (DWORD)pBase;
        dwBaseIncrement = 0;
    }
//*/
    HRESULT hRet;

    X_D3DResource *pResource = (X_D3DResource*)pThis;

    DWORD dwCommonType = pResource->Common & X_D3DCOMMON_TYPE_MASK;

    // ******************************************************************
    // * Determine the resource type, and initialize
    // ******************************************************************
    switch(dwCommonType)
    {
        case X_D3DCOMMON_TYPE_VERTEXBUFFER:
        {
            #ifdef _DEBUG_TRACE
            printf("EmuIDirect3DResource8_Register :-> VertexBuffer...\n");
            #endif

            X_D3DVertexBuffer *pVertexBuffer = (X_D3DVertexBuffer*)pResource;

            // ******************************************************************
            // * Create the vertex buffer
            // ******************************************************************
            {
                DWORD dwSize = EmuCheckAllocationSize(pBase);

                hRet = g_pD3DDevice8->CreateVertexBuffer
                (
                    dwSize, 0, 0, D3DPOOL_MANAGED,
                    &pResource->EmuVertexBuffer8
                );

                BYTE *pData = 0;

                hRet = pResource->EmuVertexBuffer8->Lock(0, 0, &pData, 0);

                if(FAILED(hRet))
                    EmuCleanup("VertexBuffer Lock failed");

                memcpy(pData, (void*)pBase, dwSize);

                pResource->EmuVertexBuffer8->Unlock();

                pResource->Data = (ULONG)pData;
            }
        }
        break;

        case X_D3DCOMMON_TYPE_INDEXBUFFER:
        {
            #ifdef _DEBUG_TRACE
            printf("EmuIDirect3DResource8_Register :-> IndexBuffer...\n");
            #endif

            X_D3DIndexBuffer *pIndexBuffer = (X_D3DIndexBuffer*)pResource;

            // ******************************************************************
            // * Create the index buffer
            // ******************************************************************
            {
                DWORD dwSize = EmuCheckAllocationSize(pBase);

                HRESULT hRet = g_pD3DDevice8->CreateIndexBuffer
                (
                    dwSize, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED,
                    &pIndexBuffer->EmuIndexBuffer8
                );

                BYTE *pData = 0;

                hRet = pResource->EmuIndexBuffer8->Lock(0, 0, &pData, 0);

                if(FAILED(hRet))
                    EmuCleanup("IndexBuffer Lock failed");

                memcpy(pData, (void*)pBase, dwSize);

                pResource->EmuIndexBuffer8->Unlock();

                pResource->Data = (ULONG)pData;
            }
        }
        break;

        case X_D3DCOMMON_TYPE_SURFACE:
            EmuCleanup("X_D3DCOMMON_TYPE_SURFACE temporarily unsupported");
        case X_D3DCOMMON_TYPE_TEXTURE:
        {
            #ifdef _DEBUG_TRACE
            printf("EmuIDirect3DResource8_Register :-> Texture...\n");
            #endif

            X_D3DPixelContainer *pPixelContainer = (X_D3DPixelContainer*)pResource;

            X_D3DFORMAT X_Format = (X_D3DFORMAT)((pPixelContainer->Format & X_D3DFORMAT_FORMAT_MASK) >> X_D3DFORMAT_FORMAT_SHIFT);
            D3DFORMAT   Format   = EmuXB2PC_D3DFormat(X_Format);

            DWORD       dwWidth, dwHeight, dwBPP, dwMipMap;
            BOOL        bSwizzled;

            dwMipMap = (pPixelContainer->Format & X_D3DFORMAT_MIPMAP_MASK) >> X_D3DFORMAT_MIPMAP_SHIFT;

            // ******************************************************************
            // * Interpret Width/Height/BPP
            // ******************************************************************
            if(X_Format == 0x07 /* X_D3DFMT_X8R8G8B8 */ || X_Format == 0x06 /* X_D3DFMT_A8R8G8B8 */)
            {
                bSwizzled = TRUE;

                // Swizzled 32 Bit
                dwWidth  = 1 << ((pPixelContainer->Format & X_D3DFORMAT_USIZE_MASK) >> X_D3DFORMAT_USIZE_SHIFT);
                dwHeight = 1 << ((pPixelContainer->Format & X_D3DFORMAT_VSIZE_MASK) >> X_D3DFORMAT_VSIZE_SHIFT);
                dwBPP = 4;
            }
            else if(X_Format == 0x05 /* X_D3DFMT_R5G6B5 */ || X_Format == 0x04 /* X_D3DFMT_A4R4G4B4 */)
            {
                bSwizzled = TRUE;

                // Swizzled 16 Bit
                dwWidth  = 1 << ((pPixelContainer->Format & X_D3DFORMAT_USIZE_MASK) >> X_D3DFORMAT_USIZE_SHIFT);
                dwHeight = 1 << ((pPixelContainer->Format & X_D3DFORMAT_VSIZE_MASK) >> X_D3DFORMAT_VSIZE_SHIFT);
                dwBPP = 2;
            }
            else if(X_Format == 0x12 /* X_D3DFORMAT_A8R8G8B8 */)
            {
                bSwizzled = FALSE;

                // Linear 32 Bit
                dwWidth  = (pPixelContainer->Size & D3DSIZE_WIDTH_MASK) + 1;
                dwHeight = ((pPixelContainer->Size & D3DSIZE_HEIGHT_MASK) >> D3DSIZE_HEIGHT_SHIFT) + 1;
                dwBPP = 4;
            }
            else
            {
                EmuCleanup("Temporarily Unrecognized Format (0x%.08X)", Format);
            }

            // ******************************************************************
            // * Create the happy little texture
            // ******************************************************************
            if(dwCommonType == X_D3DCOMMON_TYPE_SURFACE)
            {
                EmuCleanup("X_D3DCOMMON_TYPE_SURFACE temporarily unsupported");
            }
            else
            {
                hRet = g_pD3DDevice8->CreateTexture
                (
                    dwWidth, dwHeight, dwMipMap, 0, Format,
                    D3DPOOL_MANAGED, &pResource->EmuTexture8
                );
            }

            // ******************************************************************
            // * Copy over data (deswizzle if necessary)
            // ******************************************************************
            if(dwCommonType == X_D3DCOMMON_TYPE_SURFACE)
            {
                EmuCleanup("X_D3DCOMMON_TYPE_SURFACE temporarily unsupported");
            }
            else
            {
                D3DLOCKED_RECT LockedRect;

                pResource->EmuTexture8->LockRect(0, &LockedRect, NULL, 0);

                RECT  iRect  = {0,0,0,0};
                POINT iPoint = {0,0};

                if(bSwizzled)
                {
                    DWORD dwDepth = 1 << ((pPixelContainer->Format & X_D3DFORMAT_PSIZE_MASK) >> X_D3DFORMAT_PSIZE_SHIFT);

                    xg::EmuXGUnswizzleRect
                    (
                        pBase, dwWidth, dwHeight, dwDepth, LockedRect.pBits, 
                        LockedRect.Pitch, iRect, iPoint, dwBPP
                    );
                }
                else
                {
                    BYTE *pDest = (BYTE*)LockedRect.pBits;
                    BYTE *pSrc  = (BYTE*)pBase;

                    // TODO: Faster copy (maybe unnecessary)
                    for(DWORD v=0;v<dwHeight;v++)
                    {
                        memcpy(pDest, pSrc, dwWidth*dwBPP);

                        pDest += LockedRect.Pitch;
                        pSrc  += dwWidth*dwBPP;
                    }
                }

                pResource->EmuTexture8->UnlockRect(0);
            }
        }
        break;

        default:
            EmuCleanup("IDirect3DResource8::Register -> Common Type 0x%.08X not yet supported", dwCommonType);
    }

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DResource8_Release
// ******************************************************************
ULONG WINAPI xd3d8::EmuIDirect3DResource8_Release
(
    X_D3DResource      *pThis
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

    IDirect3DResource8 *pResource8 = pThis->EmuResource8;

    ULONG uRet = pResource8->Release();

    if(uRet == 0)
    {
        #ifdef _DEBUG_TRACE
        printf("EmuIDirect3DResource8_Release (0x%X): Cleaned up a Resource!\n", GetCurrentThreadId());
        #endif
        delete pThis;
    }

    EmuSwapFS();   // XBox FS

    return uRet;
}

// ******************************************************************
// * func: EmuIDirect3DResource8_IsBusy
// ******************************************************************
BOOL WINAPI xd3d8::EmuIDirect3DResource8_IsBusy
(
    X_D3DResource      *pThis
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DResource8_IsBusy\n"
               "(\n"
               "   pThis               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis);
    }
    #endif

    IDirect3DResource8 *pResource8 = pThis->EmuResource8;

    // I guess we arent doing anything, just return false..

    EmuSwapFS();   // XBox FS

    return FALSE;
}

// ******************************************************************
// * func: EmuGet2DSurfaceDesc
// ******************************************************************
VOID WINAPI xd3d8::EmuGet2DSurfaceDesc
(
    X_D3DPixelContainer *pPixelContainer,
    DWORD                dwLevel,
    X_D3DSURFACE_DESC   *pDesc
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuGet2DSurfaceDesc\n"
               "(\n"
               "   pPixelContainer     : 0x%.08X\n"
               "   dwLevel             : 0x%.08X\n"
               "   pDesc               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pPixelContainer, dwLevel, pDesc);
    }
    #endif

    EmuVerifyResourceIsRegistered(pPixelContainer);

    // TODO: HACK: notice that this is only safe to wrap a Texture Get2DSurfaceDesc,
    // as it makes an assumption here. If there is some way to determine if this pointer
    // is a texture or a surface, it'd be alot better!
    IDirect3DTexture8 *pTexture8 = pPixelContainer->EmuTexture8;

    D3DSURFACE_DESC SurfaceDesc;

	HRESULT hRet = pTexture8->GetLevelDesc(dwLevel, &SurfaceDesc);

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
            EmuCleanup("EmuGet2DSurfaceDesc Unknown Multisample format! (%d)", SurfaceDesc.MultiSampleType);

        pDesc->Width  = SurfaceDesc.Width;
        pDesc->Height = SurfaceDesc.Height;
    }

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DSurface8_GetDesc
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DSurface8_GetDesc
(
    X_D3DResource      *pThis,
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

    EmuVerifyResourceIsRegistered(pThis);

    IDirect3DSurface8 *pSurface8 = pThis->EmuSurface8;

    D3DSURFACE_DESC SurfaceDesc;

	HRESULT hRet = pSurface8->GetDesc(&SurfaceDesc);

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
            EmuCleanup("EmuIDirect3DSurface8_GetDesc Unknown Multisample format! (%d)", SurfaceDesc.MultiSampleType);

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
    X_D3DResource      *pThis,
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
        
        if(Flags & 0x40)
            printf("*Warning* D3DLOCK_TILED ignored!\n");
    }
    #endif

    EmuVerifyResourceIsRegistered(pThis);

    IDirect3DSurface8 *pSurface8 = pThis->EmuSurface8;

    DWORD NewFlags = 0;

    if(Flags & 0x80)
        NewFlags |= D3DLOCK_READONLY;

    if(Flags & !(0x80 | 0x40))
        EmuCleanup("EmuIDirect3DSurface8_LockRect: Unknown Flags! (0x%.08X)", Flags);

    // Remove old lock(s)
    pSurface8->UnlockRect();

    HRESULT hRet = pSurface8->LockRect(pLockedRect, pRect, NewFlags);

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DBaseTexture8_GetLevelCount
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DBaseTexture8_GetLevelCount
(
    X_D3DBaseTexture   *pThis
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

    EmuVerifyResourceIsRegistered(pThis);

    IDirect3DBaseTexture8 *pBaseTexture8 = pThis->EmuBaseTexture8;

    HRESULT hRet = pBaseTexture8->GetLevelCount();

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DTexture8_GetSurfaceLevel2
// ******************************************************************
xd3d8::X_D3DResource * WINAPI xd3d8::EmuIDirect3DTexture8_GetSurfaceLevel2
(
    X_D3DTexture   *pThis,
    UINT            Level
)
{
    X_D3DSurface *pSurfaceLevel;

    EmuIDirect3DTexture8_GetSurfaceLevel(pThis, Level, &pSurfaceLevel);

    return pSurfaceLevel;
}

// ******************************************************************
// * func: EmuIDirect3DTexture8_LockRect
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DTexture8_LockRect
(
    X_D3DTexture   *pThis,
    UINT            Level,
    D3DLOCKED_RECT *pLockedRect,
    CONST RECT     *pRect,
    DWORD           Flags
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DTexture8_LockRect\n"
               "(\n"
               "   pThis               : 0x%.08X\n"
               "   Level               : 0x%.08X\n"
               "   pLockedRect         : 0x%.08X\n"
               "   pRect               : 0x%.08X\n"
               "   Flags               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, Level, pLockedRect, pRect, Flags);
    }
    #endif

    EmuVerifyResourceIsRegistered(pThis);

    IDirect3DTexture8 *pTexture8 = pThis->EmuTexture8;

    DWORD NewFlags = 0;

    if(Flags & 0x80)
        NewFlags |= D3DLOCK_READONLY;

    if(Flags & !(0x80 | 0x40))
        EmuCleanup("EmuIDirect3DTexture8_LockRect: Unknown Flags! (0x%.08X)", Flags);

    // Remove old lock(s)
    pTexture8->UnlockRect(Level);

    HRESULT hRet = pTexture8->LockRect(Level, pLockedRect, pRect, NewFlags);

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DTexture8_GetSurfaceLevel
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DTexture8_GetSurfaceLevel
(
    X_D3DTexture       *pThis,
    UINT                Level,
    X_D3DSurface      **ppSurfaceLevel
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

    EmuVerifyResourceIsRegistered(pThis);

    IDirect3DTexture8 *pTexture8 = pThis->EmuTexture8;

    *ppSurfaceLevel = new X_D3DSurface();

    HRESULT hRet = pTexture8->GetSurfaceLevel(Level, &((*ppSurfaceLevel)->EmuSurface8));

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateVertexBuffer
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DDevice8_CreateVertexBuffer
(
    UINT                Length,
    DWORD               Usage,
    DWORD               FVF,
    D3DPOOL             Pool,
    X_D3DVertexBuffer **ppVertexBuffer
)
{
    *ppVertexBuffer = EmuIDirect3DDevice8_CreateVertexBuffer2(Length);

    return D3D_OK;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateVertexBuffer2
// ******************************************************************
xd3d8::X_D3DVertexBuffer* WINAPI xd3d8::EmuIDirect3DDevice8_CreateVertexBuffer2
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

    X_D3DVertexBuffer *pD3DVertexBuffer = new X_D3DVertexBuffer();

    IDirect3DVertexBuffer8 *ppVertexBuffer=NULL;

    HRESULT hRet = g_pD3DDevice8->CreateVertexBuffer
    (
        Length, 
        0,
        0,
        D3DPOOL_MANAGED, 
        &pD3DVertexBuffer->EmuVertexBuffer8
    );

    EmuSwapFS();   // XBox FS

    return pD3DVertexBuffer;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTextureState_TexCoordIndex
// ******************************************************************
VOID WINAPI xd3d8::EmuIDirect3DDevice8_SetTextureState_TexCoordIndex
(
    DWORD Stage,
    DWORD Value
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetTextureState_TexCoordIndex\n"
               "(\n"
               "   Stage               : 0x%.08X\n"
               "   Value               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Stage, Value);
    }
    #endif

    if(Value > 0x00030000)
        EmuCleanup("EmuIDirect3DDevice8_SetTextureState_TexCoordIndex: Unknown TexCoordIndex Value (0x%.08X)", Value);

    g_pD3DDevice8->SetTextureStageState(Stage, D3DTSS_TEXCOORDINDEX, Value);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_NormalizeNormals
// ******************************************************************
VOID WINAPI xd3d8::EmuIDirect3DDevice8_SetRenderState_NormalizeNormals
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
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_NormalizeNormals\n"
               "(\n"
               "   Value               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Value);
    }
    #endif

    g_pD3DDevice8->SetRenderState(D3DRS_NORMALIZENORMALS, Value);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_TextureFactor
// ******************************************************************
VOID WINAPI xd3d8::EmuIDirect3DDevice8_SetRenderState_TextureFactor
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
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_TextureFactor\n"
               "(\n"
               "   Value               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Value);
    }
    #endif

    g_pD3DDevice8->SetRenderState(D3DRS_TEXTUREFACTOR, Value);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_ZBias
// ******************************************************************
VOID WINAPI xd3d8::EmuIDirect3DDevice8_SetRenderState_ZBias
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
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_ZBias\n"
               "(\n"
               "   Value               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Value);
    }
    #endif

    g_pD3DDevice8->SetRenderState(D3DRS_ZBIAS, Value);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_EdgeAntiAlias
// ******************************************************************
VOID WINAPI xd3d8::EmuIDirect3DDevice8_SetRenderState_EdgeAntiAlias
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
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_EdgeAntiAlias\n"
               "(\n"
               "   Value               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Value);
    }
    #endif

//  TODO: Analyze performance and compatibility (undefined behavior on PC with triangles or points)
//  g_pD3DDevice8->SetRenderState(D3DRS_EDGEANTIALIAS, Value);

    printf("*Warning* SetRenderState_EdgeAntiAlias not implemented!\n");

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_FillMode
// ******************************************************************
VOID WINAPI xd3d8::EmuIDirect3DDevice8_SetRenderState_FillMode
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
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_FillMode\n"
               "(\n"
               "   Value               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Value);
    }
    #endif

    g_pD3DDevice8->SetRenderState(D3DRS_FILLMODE, EmuXB2PC_D3DFILLMODE(Value));

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_FogColor
// ******************************************************************
VOID WINAPI xd3d8::EmuIDirect3DDevice8_SetRenderState_FogColor
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
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_FogColor\n"
               "(\n"
               "   Value               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Value);
    }
    #endif

    printf("*Warning* SetRenderState_FogColor not implemented!\n");

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_Dxt1NoiseEnable
// ******************************************************************
VOID WINAPI xd3d8::EmuIDirect3DDevice8_SetRenderState_Dxt1NoiseEnable
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
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_Dxt1NoiseEnable\n"
               "(\n"
               "   Value               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Value);
    }
    #endif

    printf("*Warning* SetRenderState_Dxt1NoiseEnable not implemented!\n");

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_Simple
// ******************************************************************
VOID __fastcall xd3d8::EmuIDirect3DDevice8_SetRenderState_Simple
(
    DWORD Method,
    DWORD Value
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_Simple\n"
               "(\n"
               "   Method              : 0x%.08X\n"
               "   Value               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Method, Value);
    }
    #endif

    int State = -1;

    // Todo: make this faster and more elegant
    for(int v=0;v<82;v++)
    {
        if(EmuD3DRenderStateSimpleEncoded[v] == Method)
        {
            State = v;
            break;
        }
    }

    if(State == -1)
        printf("*Warning* RenderState_Simple(0x%.08X, 0x%.08X) is unsupported\n", Method, Value);
    else
    {
        switch(State)
        {
            case D3DRS_SRCBLEND:
            case D3DRS_DESTBLEND:
                Value = EmuXB2PC_D3DBLEND(Value);
                break;
            case D3DRS_ALPHAFUNC:
                Value = EmuXB2PC_D3DCMPFUNC(Value);
                break;
        };

        // Todo: Verify these params as you add support for them!
        g_pD3DDevice8->SetRenderState((D3DRENDERSTATETYPE)State, Value);
    }

    EmuSwapFS();   // XBox FS

    return;
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
            EmuCleanup("EmuIDirect3DDevice8_SetRenderState_CullMode: Unknown Cullmode (%d)", Value);
    }

    g_pD3DDevice8->SetRenderState(D3DRS_CULLMODE, Value);

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

    g_pD3DDevice8->SetRenderState(D3DRS_ZENABLE, Value);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_StencilEnable
// ******************************************************************
VOID WINAPI xd3d8::EmuIDirect3DDevice8_SetRenderState_StencilEnable
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
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_StencilEnable\n"
               "(\n"
               "   Value               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Value);
    }
    #endif

    g_pD3DDevice8->SetRenderState(D3DRS_STENCILENABLE, Value);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias
// ******************************************************************
VOID WINAPI xd3d8::EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias
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
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias\n"
               "(\n"
               "   Value               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Value);
    }
    #endif

    g_pD3DDevice8->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, Value);

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

    State = EmuXB2PC_D3DTS(State);

    g_pD3DDevice8->SetTransform(State, pMatrix);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetTransform
// ******************************************************************
VOID WINAPI xd3d8::EmuIDirect3DDevice8_GetTransform
(
    D3DTRANSFORMSTATETYPE State,
    D3DMATRIX            *pMatrix
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetTransform\n"
               "(\n"
               "   State               : 0x%.08X\n"
               "   pMatrix             : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), State, pMatrix);
    }
    #endif

    State = EmuXB2PC_D3DTS(State);

    g_pD3DDevice8->GetTransform(State, pMatrix);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DVertexBuffer8_Lock
// ******************************************************************
VOID WINAPI xd3d8::EmuIDirect3DVertexBuffer8_Lock
(
    X_D3DVertexBuffer  *ppVertexBuffer,
    UINT                OffsetToLock,
    UINT                SizeToLock,
    BYTE              **ppbData,
    DWORD               Flags
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

    IDirect3DVertexBuffer8 *pVertexBuffer8 = ppVertexBuffer->EmuVertexBuffer8;

    HRESULT hRet = pVertexBuffer8->Lock(OffsetToLock, SizeToLock, ppbData, Flags);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DVertexBuffer8_Lock2
// ******************************************************************
BYTE* WINAPI xd3d8::EmuIDirect3DVertexBuffer8_Lock2
(
    X_D3DVertexBuffer  *ppVertexBuffer,
    DWORD               Flags
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

    IDirect3DVertexBuffer8 *pVertexBuffer8 = ppVertexBuffer->EmuVertexBuffer8;

    BYTE *pbData = NULL;

    HRESULT hRet = pVertexBuffer8->Lock(0, 0, &pbData, Flags);

    EmuSwapFS();   // XBox FS

    return pbData;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetStreamSource
// ******************************************************************
HRESULT WINAPI xd3d8::EmuIDirect3DDevice8_SetStreamSource
(
    UINT                StreamNumber,
    X_D3DVertexBuffer  *pStreamData,
    UINT                Stride
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

    IDirect3DVertexBuffer8 *pVertexBuffer8 = pStreamData->EmuVertexBuffer8;

    pVertexBuffer8->Unlock();

    HRESULT hRet = g_pD3DDevice8->SetStreamSource(StreamNumber, pVertexBuffer8, Stride);

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

    HRESULT hRet = g_pD3DDevice8->SetVertexShader(Handle);

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuUpdateDeferredStates
// ******************************************************************
static void EmuUpdateDeferredStates()
{
    using namespace xd3d8;

    // Certain D3DRS values need to be checked on each Draw[Indexed]Vertices
    if(xd3d8::EmuD3DDeferredRenderState != 0)
    {
        if(xd3d8::EmuD3DDeferredRenderState[0] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_FOGENABLE,             xd3d8::EmuD3DDeferredRenderState[0]);

        if(xd3d8::EmuD3DDeferredRenderState[1] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_FOGTABLEMODE,          xd3d8::EmuD3DDeferredRenderState[1]);

        if(xd3d8::EmuD3DDeferredRenderState[10] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_LIGHTING,              xd3d8::EmuD3DDeferredRenderState[10]);

        if(xd3d8::EmuD3DDeferredRenderState[11] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_SPECULARENABLE,        xd3d8::EmuD3DDeferredRenderState[11]);

        if(xd3d8::EmuD3DDeferredRenderState[20] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, xd3d8::EmuD3DDeferredRenderState[20]);

        if(xd3d8::EmuD3DDeferredRenderState[23] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_AMBIENT,               xd3d8::EmuD3DDeferredRenderState[23]);

        if(xd3d8::EmuD3DDeferredRenderState[24] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_POINTSIZE,             xd3d8::EmuD3DDeferredRenderState[24]);
                                                                       
        if(xd3d8::EmuD3DDeferredRenderState[25] != X_D3DRS_UNK)        
            g_pD3DDevice8->SetRenderState(D3DRS_POINTSIZE_MIN,         xd3d8::EmuD3DDeferredRenderState[25]);
                                                                       
        if(xd3d8::EmuD3DDeferredRenderState[26] != X_D3DRS_UNK)        
            g_pD3DDevice8->SetRenderState(D3DRS_POINTSPRITEENABLE,     xd3d8::EmuD3DDeferredRenderState[26]);

        if(xd3d8::EmuD3DDeferredRenderState[27] != X_D3DRS_UNK)        
            g_pD3DDevice8->SetRenderState(D3DRS_POINTSCALEENABLE,      xd3d8::EmuD3DDeferredRenderState[27]);

        if(xd3d8::EmuD3DDeferredRenderState[28] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_POINTSCALE_A,          xd3d8::EmuD3DDeferredRenderState[28]);

        if(xd3d8::EmuD3DDeferredRenderState[29] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_POINTSCALE_B,          xd3d8::EmuD3DDeferredRenderState[29]);

        if(xd3d8::EmuD3DDeferredRenderState[30] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_POINTSCALE_C,          xd3d8::EmuD3DDeferredRenderState[30]);

        /** To check for unhandled RenderStates
        for(int v=0;v<117-82;v++)
        {
            if(xd3d8::EmuD3DDeferredRenderState[v] != X_D3DRS_UNK)
            {
                if(v != 0  && v != 1  && v != 10 && v != 11 && v != 20 && v != 23
                && v != 24 && v != 25 && v != 26 && v != 27 && v != 28 && v != 29
                && v != 30)
                    printf("*Warning* Unhandled RenderState Change @ %d (%d)\n", v, v + 82);
            }
        }
        //**/
    }

    // Certain D3DTS values need to be checked on each Draw[Indexed]Vertices
    if(xd3d8::EmuD3DDeferredTextureState != 0)
    {
        for(int v=0;v<4;v++)
        {
            DWORD *pCur = xd3d8::EmuD3DDeferredTextureState+v;

            if(pCur[0] != X_D3DTSS_UNK)
            {
                if(pCur[0] == 5)
                    EmuCleanup("ClampToEdge is unsupported (temporarily)");

                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_ADDRESSU, pCur[0]);
            }

            if(pCur[1] != X_D3DTSS_UNK)
            {
                if(pCur[1] == 5)
                    EmuCleanup("ClampToEdge is unsupported (temporarily)");

                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_ADDRESSV, pCur[1]);
            }

            if(pCur[2] != X_D3DTSS_UNK)
            {
                if(pCur[2] == 5)
                    EmuCleanup("ClampToEdge is unsupported (temporarily)");

                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_ADDRESSW, pCur[2]);
            }

            if(pCur[3] != X_D3DTSS_UNK)
            {
                if(pCur[3] == 4)
                    EmuCleanup("QuinCunx is unsupported (temporarily)");

                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_MAGFILTER, pCur[3]);
            }

            if(pCur[4] != X_D3DTSS_UNK)
            {
                if(pCur[4] == 4)
                    EmuCleanup("QuinCunx is unsupported (temporarily)");

                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_MINFILTER, pCur[4]);
            }

            if(pCur[5] != X_D3DTSS_UNK)
            {
                if(pCur[5] == 4)
                    EmuCleanup("QuinCunx is unsupported (temporarily)");

                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_MIPFILTER, pCur[5]);
            }

            // TODO: Use a lookup table, this is not always a 1:1 map
            if(pCur[12] != X_D3DTSS_UNK)
            {
                if(pCur[12] > 12)
                    EmuCleanup("(Temporarily) Unsupported D3DTSS_ALPHAOP Value (%d)", pCur[12]);

                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_COLOROP, pCur[12]);
            }

            if(pCur[13] != X_D3DTSS_UNK)
                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_COLORARG0, pCur[13]);

            if(pCur[14] != X_D3DTSS_UNK)
                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_COLORARG1, pCur[14]);

            if(pCur[15] != X_D3DTSS_UNK)
                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_COLORARG2, pCur[15]);

            // TODO: Use a lookup table, this is not always a 1:1 map (same as D3DTSS_COLOROP)
            if(pCur[16] != X_D3DTSS_UNK)
            {
                if(pCur[16] > 12)
                    EmuCleanup("(Temporarily) Unsupported D3DTSS_ALPHAOP Value (%d)", pCur[16]);

                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_ALPHAOP, pCur[16]);
            }

            if(pCur[17] != X_D3DTSS_UNK)
                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_ALPHAARG0, pCur[17]);

            if(pCur[18] != X_D3DTSS_UNK)
                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_ALPHAARG1, pCur[18]);
            
            if(pCur[19] != X_D3DTSS_UNK)
                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_ALPHAARG2, pCur[19]);

            if(pCur[20] != X_D3DTSS_UNK)
                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_RESULTARG, pCur[20]);

            if(pCur[21] != X_D3DTSS_UNK)
                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_TEXTURETRANSFORMFLAGS, pCur[21]);

            if(pCur[29] != X_D3DTSS_UNK)
                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_BORDERCOLOR, pCur[29]);

            /** To check for unhandled texture stage state changes
            for(int r=0;r<32;r++)
            {
                static const int unchecked[] = 
                {
                    0, 1, 2, 3, 4, 5, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 29, 30, 31
                };

                if(pCur[r] != X_D3DTSS_UNK)
                {
                    bool pass = true;

                    for(int q=0;q<sizeof(unchecked)/sizeof(int);q++)
                    {
                        if(r == unchecked[q])
                        {
                            pass = false;
                            break;
                        }
                    }

                    if(pass)
                        printf("*Warning* Unhandled TextureState Change @ %d->%d\n", v, r);
                }
            }
            //**/
        }
    }
}

// ******************************************************************
// * func: EmuQuadHackA
// ******************************************************************
uint32 EmuQuadHackA(uint32 PrimitiveCount, xd3d8::IDirect3DVertexBuffer8 *&pOrigVertexBuffer8, xd3d8::IDirect3DVertexBuffer8 *&pHackVertexBuffer8)
{
    using namespace xd3d8;

    UINT nStride = 0;

    g_pD3DDevice8->GetStreamSource(0, &pOrigVertexBuffer8, &nStride);

    g_pD3DDevice8->CreateVertexBuffer(PrimitiveCount*nStride*6, 0, 0, D3DPOOL_DEFAULT, &pHackVertexBuffer8);

    BOOL bPatch = FALSE;

    // Determine if we need to patch the buffer (?)
    {
        DWORD dwVertexShader = NULL;

        g_pD3DDevice8->GetVertexShader(&dwVertexShader);

        if(dwVertexShader & D3DFVF_XYZRHW)
			bPatch=TRUE;
    }

    BYTE *pOrigVertexData = 0;
    BYTE *pHackVertexData = 0;

    pOrigVertexBuffer8->Lock(0, 0, &pOrigVertexData, 0);
    pHackVertexBuffer8->Lock(0, 0, &pHackVertexData, 0);

	for(DWORD i=0;i<PrimitiveCount;i++)
    {
		memcpy(&pHackVertexData[i*nStride*6+0*nStride], &pOrigVertexData[i*nStride*4+2*nStride], nStride);
		memcpy(&pHackVertexData[i*nStride*6+1*nStride], &pOrigVertexData[i*nStride*4+0*nStride], nStride);
		memcpy(&pHackVertexData[i*nStride*6+2*nStride], &pOrigVertexData[i*nStride*4+1*nStride], nStride);
		memcpy(&pHackVertexData[i*nStride*6+3*nStride], &pOrigVertexData[i*nStride*4+2*nStride], nStride);
		memcpy(&pHackVertexData[i*nStride*6+4*nStride], &pOrigVertexData[i*nStride*4+3*nStride], nStride);
		memcpy(&pHackVertexData[i*nStride*6+5*nStride], &pOrigVertexData[i*nStride*4+0*nStride], nStride);

        if(bPatch)
        {
			for(int z=0; z<6; z++)
            {
				if(((FLOAT*)&pHackVertexData[i*nStride*6+z*nStride])[2] == 0.0f)
					((FLOAT*)&pHackVertexData[i*nStride*6+z*nStride])[2] = 1.0f;
				if(((FLOAT*)&pHackVertexData[i*nStride*6+z*nStride])[3] == 0.0f)
					((FLOAT*)&pHackVertexData[i*nStride*6+z*nStride])[3] = 1.0f;
			}
        }
	}

	pOrigVertexBuffer8->Unlock();
    pHackVertexBuffer8->Unlock();

    g_pD3DDevice8->SetStreamSource(0, pHackVertexBuffer8, nStride);

    return nStride;
}

// ******************************************************************
// * func: EmuQuadHackB
// ******************************************************************
VOID EmuQuadHackB(uint32 nStride, xd3d8::IDirect3DVertexBuffer8 *&pOrigVertexBuffer8, xd3d8::IDirect3DVertexBuffer8 *&pHackVertexBuffer8)
{
    g_pD3DDevice8->SetStreamSource(0, pOrigVertexBuffer8, nStride);

    pOrigVertexBuffer8->Release();
    pHackVertexBuffer8->Release();
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_DrawVertices
// ******************************************************************
VOID WINAPI xd3d8::EmuIDirect3DDevice8_DrawVertices
(
    X_D3DPRIMITIVETYPE PrimitiveType,
    UINT               StartVertex,
    UINT               VertexCount
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

    EmuUpdateDeferredStates();

    if((DWORD)PrimitiveType == 0x03 || (DWORD)PrimitiveType == 0x09 || (DWORD)PrimitiveType == 0x10)
        printf("*Warning* unsupported PrimitiveType! (%d)\n", (DWORD)PrimitiveType);

    UINT PrimitiveCount = EmuD3DVertex2PrimitiveCount(PrimitiveType, VertexCount);

    // Convert from Xbox to PC enumeration
    D3DPRIMITIVETYPE PCPrimitiveType = EmuPrimitiveType(PrimitiveType);

    IDirect3DVertexBuffer8 *pOrigVertexBuffer8 = 0;
    IDirect3DVertexBuffer8 *pHackVertexBuffer8 = 0;

    uint32 nStride = 0;

    if(PrimitiveType == 8)  // Quad List
    {
        PrimitiveCount *= 2;
        nStride = EmuQuadHackA(PrimitiveCount, pOrigVertexBuffer8, pHackVertexBuffer8);
    }

    g_pD3DDevice8->DrawPrimitive
    (
        PCPrimitiveType,
        StartVertex,
        PrimitiveCount
    );

    if(PrimitiveType == 8)  // Quad List
        EmuQuadHackB(nStride, pOrigVertexBuffer8, pHackVertexBuffer8);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_DrawVerticesUP
// ******************************************************************
VOID WINAPI xd3d8::EmuIDirect3DDevice8_DrawVerticesUP
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    UINT                VertexCount,
    CONST PVOID         pVertexStreamZeroData,
    UINT                VertexStreamZeroStride
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_DrawVerticesUP\n"
               "(\n"
               "   PrimitiveType            : 0x%.08X\n"
               "   VertexCount              : 0x%.08X\n"
               "   pVertexStreamZeroData    : 0x%.08X\n"
               "   VertexStreamZeroStride   : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), PrimitiveType, VertexCount, pVertexStreamZeroData,
               VertexStreamZeroStride);
    }
    #endif

    EmuUpdateDeferredStates();

    if((DWORD)PrimitiveType == 0x03 || (DWORD)PrimitiveType == 0x09 || (DWORD)PrimitiveType == 0x10)
        printf("Unsupported PrimitiveType! (%d)\n", (DWORD)PrimitiveType);

    UINT PrimitiveCount = EmuD3DVertex2PrimitiveCount(PrimitiveType, VertexCount);

    // Convert from Xbox to PC enumeration
    D3DPRIMITIVETYPE PCPrimitiveType = EmuPrimitiveType(PrimitiveType);

    IDirect3DVertexBuffer8 *pOrigVertexBuffer8 = 0;
    IDirect3DVertexBuffer8 *pHackVertexBuffer8 = 0;

    uint32 nStride = 0;

    if(PrimitiveType == 8)  // Quad List
    {
        PrimitiveCount *= 2;
        nStride = EmuQuadHackA(PrimitiveCount, pOrigVertexBuffer8, pHackVertexBuffer8);
    }

    g_pD3DDevice8->DrawPrimitiveUP
    (
        PCPrimitiveType,
        PrimitiveCount,
        pVertexStreamZeroData,
        VertexStreamZeroStride
    );

    if(PrimitiveType == 8)  // Quad List
        EmuQuadHackB(nStride, pOrigVertexBuffer8, pHackVertexBuffer8);

    EmuSwapFS();   // XBox FS

    return;
}


// ******************************************************************
// * func: EmuIDirect3DDevice8_DrawIndexedVertices
// ******************************************************************
VOID WINAPI xd3d8::EmuIDirect3DDevice8_DrawIndexedVertices
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    UINT                VertexCount,
    CONST PWORD         pIndexData
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_DrawIndexedVertices\n"
               "(\n"
               "   PrimitiveType       : 0x%.08X\n"
               "   VertexCount         : 0x%.08X\n"
               "   pIndexData          : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), PrimitiveType, VertexCount, pIndexData);
    }
    #endif

    if(PrimitiveType == 8)  // Quad List
        EmuCleanup("DrawIndexVertices does not handle Quad->Tri yet!");

    EmuUpdateDeferredStates();

    if((DWORD)PrimitiveType == 0x03 || (DWORD)PrimitiveType == 0x08 || (DWORD)PrimitiveType == 0x09 || (DWORD)PrimitiveType == 0x10)
        printf("Unsupported PrimitiveType! (%d)\n", (DWORD)PrimitiveType);

    UINT PrimitiveCount = EmuD3DVertex2PrimitiveCount(PrimitiveType, VertexCount);

    // Convert from Xbox to PC enumeration
    D3DPRIMITIVETYPE PCPrimitiveType = EmuPrimitiveType(PrimitiveType);

    IDirect3DVertexBuffer8 *pOrigVertexBuffer8 = 0;
    IDirect3DVertexBuffer8 *pHackVertexBuffer8 = 0;

    uint32 nStride = 0;

    if(PrimitiveType == 8)  // Quad List
    {
        PrimitiveCount *= 2;
        nStride = EmuQuadHackA(PrimitiveCount, pOrigVertexBuffer8, pHackVertexBuffer8);
    }

    g_pD3DDevice8->DrawIndexedPrimitive
    (
        PCPrimitiveType, 0, PrimitiveCount,
        0, PrimitiveCount
    );

    if(PrimitiveType == 8)  // Quad List
        EmuQuadHackB(nStride, pOrigVertexBuffer8, pHackVertexBuffer8);

    EmuSwapFS();   // XBox FS

    return;
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

    HRESULT hRet = g_pD3DDevice8->SetLight(Index, pLight);

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

    HRESULT hRet = g_pD3DDevice8->SetMaterial(pMaterial);

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

    HRESULT hRet = g_pD3DDevice8->LightEnable(Index, bEnable);

    EmuSwapFS();   // XBox FS
    
    return hRet;
}
