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
#include "EmuShared.h"

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace XTL
{
    #include "EmuXTL.h"
};

#include "ResCxbxDll.h"

#include <process.h>
#include <locale.h>

// ******************************************************************
// * Global(s)
// ******************************************************************
HWND XTL::g_hEmuWindow = NULL;   // Rendering Window

// ******************************************************************
// * Static Function(s)
// ******************************************************************
static DWORD WINAPI   EmuRenderWindow(LPVOID);
static DWORD WINAPI   EmuCreateDeviceProxy(LPVOID);
static LRESULT WINAPI EmuMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static DWORD WINAPI   EmuUpdateTickCount(LPVOID);
static DWORD          EmuCheckAllocationSize(LPVOID);
static inline void    EmuVerifyResourceIsRegistered(XTL::X_D3DResource *pResource);
static void           EmuAdjustPower2(UINT *dwWidth, UINT *dwHeight);

// ******************************************************************
// * Static Variable(s)
// ******************************************************************
static XTL::LPDIRECT3D8             g_pD3D8         = NULL; // Direct3D8
static XTL::LPDIRECT3DDEVICE8       g_pD3DDevice8   = NULL; // Direct3D8 Device
static BOOL                         g_bSupportsYUY2 = FALSE;// Does device support YUY2 overlays?
static XTL::LPDIRECTDRAW7           g_pDD7          = NULL; // DirectDraw7
static XTL::LPDIRECTDRAWSURFACE7    g_pDDSPrimary   = NULL; // DirectDraw7 Primary Surface
static XTL::LPDIRECTDRAWSURFACE7    g_pDDSOverlay7  = NULL; // DirectDraw7 Overlay Surface
static DWORD                        g_dwOverlayW    = 640;  // Cached Overlay Width
static DWORD                        g_dwOverlayH    = 480;  // Cached Overlay Height
static Xbe::Header                 *g_XbeHeader     = NULL; // XbeHeader
static uint32                       g_XbeHeaderSize = 0;    // XbeHeaderSize
static XTL::D3DCAPS8                g_D3DCaps;              // Direct3D8 Caps
static HBRUSH                       g_hBgBrush      = NULL; // Background Brush
static volatile bool                g_bRenderWindowActive = false;
static XBVideo                      g_XBVideo;

// ******************************************************************
// * Cached Direct3D State Variable(s)
// ******************************************************************
static XTL::X_D3DSurface      *g_pCachedRenderTarget = NULL;
static XTL::X_D3DSurface      *g_pCachedZStencilSurface = NULL;
static DWORD                   g_dwVertexShaderUsage = 0;

// ******************************************************************
// * EmuD3DTiles (8 Tiles Max)
// ******************************************************************
XTL::X_D3DTILE XTL::EmuD3DTileCache[0x08] = {0};

// ******************************************************************
// * EmuD3DDeferredRenderState
// ******************************************************************
DWORD *XTL::EmuD3DDeferredRenderState;

// ******************************************************************
// * EmuD3DDeferredTextureState
// ******************************************************************
DWORD *XTL::EmuD3DDeferredTextureState;

// ******************************************************************
// * EmuD3D8CreateDeviceProxyData
// ******************************************************************
struct EmuD3D8CreateDeviceProxyData
{
    XTL::UINT                        Adapter;
    XTL::D3DDEVTYPE                  DeviceType;
    HWND                             hFocusWindow;
    XTL::DWORD                       BehaviorFlags;
    XTL::X_D3DPRESENT_PARAMETERS    *pPresentationParameters;
    XTL::IDirect3DDevice8          **ppReturnedDeviceInterface;
    volatile bool                    bReady;
    volatile HRESULT                 hRet;
}
g_EmuD3D8CreateDeviceProxyData = {0};

// ******************************************************************
// * func: XTL::EmuD3DInit
// ******************************************************************
VOID XTL::EmuD3DInit(Xbe::Header *XbeHeader, uint32 XbeHeaderSize)
{
    g_EmuShared->GetXBVideo(&g_XBVideo);

    // ******************************************************************
    // * store XbeHeader and XbeHeaderSize for further use
    // ******************************************************************
    g_XbeHeader     = XbeHeader;
    g_XbeHeaderSize = XbeHeaderSize;

    // ******************************************************************
    // * create a thread dedicated to timing
    // ******************************************************************
    {
        DWORD dwThreadId;

        CreateThread(NULL, NULL, EmuUpdateTickCount, NULL, NULL, &dwThreadId);
    }

    // ******************************************************************
    // * create a thread dedicated to creating devices
    // ******************************************************************
    {
        DWORD dwThreadId;

        CreateThread(NULL, NULL, EmuCreateDeviceProxy, NULL, NULL, &dwThreadId);
    }

    // ******************************************************************
    // * spark up a new thread to handle window message processing
    // ******************************************************************
    {
        DWORD dwThreadId;

        g_bRenderWindowActive = false;

        CreateThread(NULL, NULL, EmuRenderWindow, NULL, NULL, &dwThreadId);

        while(!g_bRenderWindowActive)
            Sleep(10);

        Sleep(50);
    }

    // ******************************************************************
    // * create Direct3D8 and retrieve caps
    // ******************************************************************
    {
        using namespace XTL;

        // xbox Direct3DCreate8 returns "1" always, so we need our own ptr
        g_pD3D8 = Direct3DCreate8(D3D_SDK_VERSION);

        if(g_pD3D8 == NULL)
            EmuCleanup("Could not initialize Direct3D8!");

        D3DDEVTYPE DevType = (g_XBVideo.GetDirect3DDevice() == 0) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF;

        g_pD3D8->GetDeviceCaps(g_XBVideo.GetDisplayAdapter(), DevType, &g_D3DCaps);
    }

    // ******************************************************************
    // * create DirectDraw7
    // ******************************************************************
    {
        using namespace XTL;

        HRESULT hRet = DirectDrawCreateEx(NULL, (void**)&g_pDD7, IID_IDirectDraw7, NULL);

        if(FAILED(hRet))
            EmuCleanup("Could not initialize DirectDraw7");

        hRet = g_pDD7->SetCooperativeLevel(0, DDSCL_NORMAL);

        if(FAILED(hRet))
            EmuCleanup("Could not set cooperative level");
    }

    // ******************************************************************
    // * create default device
    // ******************************************************************
    {
        XTL::X_D3DPRESENT_PARAMETERS PresParam;

        ZeroMemory(&PresParam, sizeof(PresParam));

        PresParam.BackBufferWidth  = 640;
        PresParam.BackBufferHeight = 480;
        PresParam.BackBufferFormat = 6; /* X_D3DFMT_A8R8G8B8 */
        PresParam.BackBufferCount  = 1;
        PresParam.EnableAutoDepthStencil = TRUE;
        PresParam.AutoDepthStencilFormat = 0x2A; /* X_D3DFMT_D24S8 */
        PresParam.SwapEffect = XTL::D3DSWAPEFFECT_DISCARD;

        EmuSwapFS();    // XBox FS
        XTL::EmuIDirect3D8_CreateDevice(0, XTL::D3DDEVTYPE_HAL, 0, 0x00000040, &PresParam, &g_pD3DDevice8);
        EmuSwapFS();    // Win2k/XP FS
    }
}

// ******************************************************************
// * func: XTL::EmuD3DCleanup
// ******************************************************************
VOID XTL::EmuD3DCleanup()
{
    XTL::EmuDInputCleanup();

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

            int nTitleHeight  = GetSystemMetrics(SM_CYCAPTION);
            int nBorderWidth  = GetSystemMetrics(SM_CXSIZEFRAME);
            int nBorderHeight = GetSystemMetrics(SM_CYSIZEFRAME);

            int x = 100, y = 100, nWidth = 640, nHeight = 480;

            nWidth  += nBorderWidth*2;
            nHeight += nBorderHeight*2 + nTitleHeight;

            sscanf(g_XBVideo.GetVideoResolution(), "%d x %d", &nWidth, &nHeight);

            if(g_XBVideo.GetFullscreen())
            {
                x = y = nWidth = nHeight = 0;
                dwStyle = WS_POPUP;
            }

            XTL::g_hEmuWindow = CreateWindow
            (
                "CxbxRender", AsciiTitle,
                dwStyle, x, y, nWidth, nHeight,
                GetDesktopWindow(), NULL, GetModuleHandle(NULL), NULL
            );
        }
    }

    ShowWindow(XTL::g_hEmuWindow, SW_SHOWDEFAULT);
    UpdateWindow(XTL::g_hEmuWindow);

    // ******************************************************************
    // * initialize direct input
    // ******************************************************************
    if(!XTL::EmuDInputInit())
        EmuCleanup("Could not initialize DirectInput!");

    printf("EmuD3D8 (0x%X): Message-Pump thread is running.\n", GetCurrentThreadId());

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
                g_bRenderWindowActive = true;

                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
                Sleep(10);
        }

        g_bRenderWindowActive = false;

        EmuCleanup(NULL);
    }

    return 0;
}

// ******************************************************************
// * func: EmuMsgProc
// ******************************************************************
static LRESULT WINAPI EmuMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static DWORD dwRestoreSleepRate = EmuAutoSleepRate;

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

        case WM_SETFOCUS:
            EmuAutoSleepRate = dwRestoreSleepRate;
            break;

        case WM_KILLFOCUS:
            dwRestoreSleepRate = EmuAutoSleepRate;
            EmuAutoSleepRate = 0;
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
    printf("EmuD3D8 (0x%X): Timing thread is running.\n", GetCurrentThreadId());

    timeBeginPeriod(0);

    while(true)
    {
        xboxkrnl::KeTickCount = timeGetTime();
        Sleep(1);
    }

    timeEndPeriod(0);
}

// ******************************************************************
// * func: EmuCreateDeviceProxy
// ******************************************************************
static DWORD WINAPI EmuCreateDeviceProxy(LPVOID)
{
    printf("EmuD3D8 (0x%X): CreateDevice proxy thread is running.\n", GetCurrentThreadId());

    while(true)
    {
        // if we have been signalled, create the device with cached parameters
        if(g_EmuD3D8CreateDeviceProxyData.bReady)
        {
            printf("EmuD3D8 (0x%X): CreateDevice proxy thread recieved request.\n", GetCurrentThreadId());

            // only one device should be created at once
            // TODO: ensure all surfaces are somehow cleaned up?
            if(g_pD3DDevice8 != 0)
            {
                printf("EmuD3D8 (0x%X): CreateDevice proxy thread releasing old Device.\n", GetCurrentThreadId());

                g_pD3DDevice8->EndScene();

                while(g_pD3DDevice8->Release() != 0);

                g_pD3DDevice8 = 0;
            }

            // ******************************************************************
            // * verify no ugly circumstances
            // ******************************************************************
            if(g_EmuD3D8CreateDeviceProxyData.pPresentationParameters->BufferSurfaces[0] != NULL || g_EmuD3D8CreateDeviceProxyData.pPresentationParameters->DepthStencilSurface != NULL)
                EmuWarning("DepthStencilSurface != NULL and/or BufferSurfaces[0] != NULL");

            // ******************************************************************
            // * make adjustments to parameters to make sense with windows d3d
            // ******************************************************************
            {
                g_EmuD3D8CreateDeviceProxyData.DeviceType =(g_XBVideo.GetDirect3DDevice() == 0) ? XTL::D3DDEVTYPE_HAL : XTL::D3DDEVTYPE_REF;
                g_EmuD3D8CreateDeviceProxyData.Adapter    = g_XBVideo.GetDisplayAdapter();

                g_EmuD3D8CreateDeviceProxyData.pPresentationParameters->Windowed = !g_XBVideo.GetFullscreen();

                if(g_XBVideo.GetVSync())
                    g_EmuD3D8CreateDeviceProxyData.pPresentationParameters->SwapEffect = XTL::D3DSWAPEFFECT_COPY_VSYNC;

                g_EmuD3D8CreateDeviceProxyData.hFocusWindow = XTL::g_hEmuWindow;

                g_EmuD3D8CreateDeviceProxyData.pPresentationParameters->BackBufferFormat       = XTL::EmuXB2PC_D3DFormat(g_EmuD3D8CreateDeviceProxyData.pPresentationParameters->BackBufferFormat);
                g_EmuD3D8CreateDeviceProxyData.pPresentationParameters->AutoDepthStencilFormat = XTL::EmuXB2PC_D3DFormat(g_EmuD3D8CreateDeviceProxyData.pPresentationParameters->AutoDepthStencilFormat);

                if(!g_XBVideo.GetVSync() && (g_D3DCaps.PresentationIntervals & D3DPRESENT_INTERVAL_IMMEDIATE) && g_XBVideo.GetFullscreen())
                    g_EmuD3D8CreateDeviceProxyData.pPresentationParameters->FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
                else
                {
                    if(g_D3DCaps.PresentationIntervals & D3DPRESENT_INTERVAL_ONE && g_XBVideo.GetFullscreen())
                        g_EmuD3D8CreateDeviceProxyData.pPresentationParameters->FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
                    else
                        g_EmuD3D8CreateDeviceProxyData.pPresentationParameters->FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
                }

                // TODO: Support Xbox extensions if possible
                if(g_EmuD3D8CreateDeviceProxyData.pPresentationParameters->MultiSampleType != 0)
                {
                    EmuWarning("MultiSampleType 0x%.08X is not supported!", g_EmuD3D8CreateDeviceProxyData.pPresentationParameters->MultiSampleType);

                    g_EmuD3D8CreateDeviceProxyData.pPresentationParameters->MultiSampleType = XTL::D3DMULTISAMPLE_NONE;

                    // TODO: Check card for multisampling abilities
        //            if(pPresentationParameters->MultiSampleType == 0x00001121)
        //                pPresentationParameters->MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
        //            else
        //                EmuCleanup("Unknown MultiSampleType (0x%.08X)", pPresentationParameters->MultiSampleType);
                }

                g_EmuD3D8CreateDeviceProxyData.pPresentationParameters->Flags |= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
        
                // ******************************************************************
                // * Retrieve Resolution from Configuration
                // ******************************************************************
                if(g_EmuD3D8CreateDeviceProxyData.pPresentationParameters->Windowed)
                {
                    sscanf(g_XBVideo.GetVideoResolution(), "%d x %d", &g_EmuD3D8CreateDeviceProxyData.pPresentationParameters->BackBufferWidth, &g_EmuD3D8CreateDeviceProxyData.pPresentationParameters->BackBufferHeight);

                    XTL::D3DDISPLAYMODE D3DDisplayMode;

                    g_pD3D8->GetAdapterDisplayMode(g_XBVideo.GetDisplayAdapter(), &D3DDisplayMode);

                    g_EmuD3D8CreateDeviceProxyData.pPresentationParameters->BackBufferFormat = D3DDisplayMode.Format;
                    g_EmuD3D8CreateDeviceProxyData.pPresentationParameters->FullScreen_RefreshRateInHz = 0;
                }
                else
                {
                    char szBackBufferFormat[16];

                    sscanf(g_XBVideo.GetVideoResolution(), "%d x %d %*dbit %s (%d hz)", 
                        &g_EmuD3D8CreateDeviceProxyData.pPresentationParameters->BackBufferWidth, 
                        &g_EmuD3D8CreateDeviceProxyData.pPresentationParameters->BackBufferHeight,
                        szBackBufferFormat,
                        &g_EmuD3D8CreateDeviceProxyData.pPresentationParameters->FullScreen_RefreshRateInHz);

                    if(strcmp(szBackBufferFormat, "x1r5g5b5") == 0)
                        g_EmuD3D8CreateDeviceProxyData.pPresentationParameters->BackBufferFormat = XTL::D3DFMT_X1R5G5B5;
                    else if(strcmp(szBackBufferFormat, "r5g6r5") == 0)
                        g_EmuD3D8CreateDeviceProxyData.pPresentationParameters->BackBufferFormat = XTL::D3DFMT_R5G6B5;
                    else if(strcmp(szBackBufferFormat, "x8r8g8b8") == 0)
                        g_EmuD3D8CreateDeviceProxyData.pPresentationParameters->BackBufferFormat = XTL::D3DFMT_X8R8G8B8;
                    else if(strcmp(szBackBufferFormat, "a8r8g8b8") == 0)
                        g_EmuD3D8CreateDeviceProxyData.pPresentationParameters->BackBufferFormat = XTL::D3DFMT_A8R8G8B8;
                }
            }

            // ******************************************************************
            // * Detect vertex processing capabilities
            // ******************************************************************
            if((g_D3DCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) && g_EmuD3D8CreateDeviceProxyData.DeviceType == XTL::D3DDEVTYPE_HAL)
            {
                #ifdef _DEBUG_TRACE
                printf("EmuD3D8 (0x%X): Using hardware vertex processing\n", GetCurrentThreadId());
                #endif
                g_EmuD3D8CreateDeviceProxyData.BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
                g_dwVertexShaderUsage = 0;
            }
            else
            {
                #ifdef _DEBUG_TRACE
                printf("EmuD3D8 (0x%X): Using software vertex processing\n", GetCurrentThreadId());
                #endif
                g_EmuD3D8CreateDeviceProxyData.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
                g_dwVertexShaderUsage = D3DUSAGE_SOFTWAREPROCESSING;
            }

            // ******************************************************************
            // * redirect to windows d3d
            // ******************************************************************
            g_EmuD3D8CreateDeviceProxyData.hRet = g_pD3D8->CreateDevice
            (
                g_EmuD3D8CreateDeviceProxyData.Adapter,
                g_EmuD3D8CreateDeviceProxyData.DeviceType,
                g_EmuD3D8CreateDeviceProxyData.hFocusWindow,
                g_EmuD3D8CreateDeviceProxyData.BehaviorFlags,
                (XTL::D3DPRESENT_PARAMETERS*)g_EmuD3D8CreateDeviceProxyData.pPresentationParameters,
                g_EmuD3D8CreateDeviceProxyData.ppReturnedDeviceInterface
            );

            // ******************************************************************
            // * report error
            // ******************************************************************
            if(FAILED(g_EmuD3D8CreateDeviceProxyData.hRet))
            {
                if(g_EmuD3D8CreateDeviceProxyData.hRet == D3DERR_INVALIDCALL)
                    EmuCleanup("IDirect3D8::CreateDevice failed (Invalid Call)");
                else if(g_EmuD3D8CreateDeviceProxyData.hRet == D3DERR_NOTAVAILABLE)
                    EmuCleanup("IDirect3D8::CreateDevice failed (Not Available)");
                else if(g_EmuD3D8CreateDeviceProxyData.hRet == D3DERR_OUTOFVIDEOMEMORY)
                    EmuCleanup("IDirect3D8::CreateDevice failed (Out of Video Memory)");

                EmuCleanup("IDirect3D8::CreateDevice failed (Unknown)");
            }

            // ******************************************************************
            // * it is necessary to store this pointer globally for emulation
            // ******************************************************************
            g_pD3DDevice8 = *g_EmuD3D8CreateDeviceProxyData.ppReturnedDeviceInterface;

            // ******************************************************************
            // * check for YUY2 overlay support
            // ******************************************************************
            {
                XTL::D3DDISPLAYMODE DisplayMode;

                if(g_pD3DDevice8->GetDisplayMode(&DisplayMode) != D3D_OK)
                    g_bSupportsYUY2 = FALSE;
                else
                {
                    ::HRESULT hRet = g_pD3D8->CheckDeviceFormat
                    (
                        g_EmuD3D8CreateDeviceProxyData.Adapter, g_EmuD3D8CreateDeviceProxyData.DeviceType,
                        (XTL::D3DFORMAT)DisplayMode.Format, 0, XTL::D3DRTYPE_TEXTURE, XTL::D3DFMT_YUY2
                    );

                    g_bSupportsYUY2 = SUCCEEDED(hRet);

                    if(!g_bSupportsYUY2)
                        EmuWarning("YUY2 overlays are not supported in hardware, could be slow!");
                }
            }

            // ******************************************************************
            // * Update Caches
            // ******************************************************************
            {
                g_pCachedRenderTarget = new XTL::X_D3DSurface();
                g_pD3DDevice8->GetRenderTarget(&g_pCachedRenderTarget->EmuSurface8);

                g_pCachedZStencilSurface = new XTL::X_D3DSurface();
                g_pD3DDevice8->GetDepthStencilSurface(&g_pCachedZStencilSurface->EmuSurface8);
            }

            // ******************************************************************
            // * Begin Scene
            // ******************************************************************
            g_pD3DDevice8->BeginScene();

            // ******************************************************************
            // * Initially, show a black screen
            // ******************************************************************
            g_pD3DDevice8->Clear(0, 0, D3DCLEAR_TARGET, 0, 0, 0);
            g_pD3DDevice8->Present(0, 0, 0, 0);

            // signal completion
            g_EmuD3D8CreateDeviceProxyData.bReady = false;
        }

        Sleep(1);
    }
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
static inline void EmuVerifyResourceIsRegistered(XTL::X_D3DResource *pResource)
{
    if(pResource->Lock == 0)
    {
        EmuSwapFS();    // XBox FS;
        XTL::EmuIDirect3DResource8_Register(pResource, 0/*(PVOID)pResource->Data*/);
        EmuSwapFS();    // Win2k/XP FS
    }
}

// ******************************************************************
// * func: EmuAdjustPower2
// ******************************************************************
static void EmuAdjustPower2(UINT *dwWidth, UINT *dwHeight)
{
    UINT NewWidth=0, NewHeight=0;

    int v;

    for(v=0;v<32;v++)
    {
        int mask = 1 << v;

        if(*dwWidth & mask)
            NewWidth = mask;

        if(*dwHeight & mask)
            NewHeight = mask;
    }

    if(*dwWidth != NewWidth)
    {
        NewWidth <<= 1;
        printf("*Warning* needed to resize width (%d->%d)\n", *dwWidth, NewWidth);
    }

    if(*dwHeight != NewHeight)
    {
        NewHeight <<= 1;
        printf("*Warning* needed to resize height (%d->%d)\n", *dwHeight, NewHeight);
    }

    *dwWidth = NewWidth;
    *dwHeight = NewHeight;
}

// ******************************************************************
// * func: EmuIDirect3D8_CreateDevice
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3D8_CreateDevice
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

    // Cache parameters
    g_EmuD3D8CreateDeviceProxyData.Adapter = Adapter;
    g_EmuD3D8CreateDeviceProxyData.DeviceType = DeviceType;
    g_EmuD3D8CreateDeviceProxyData.hFocusWindow = hFocusWindow;
    g_EmuD3D8CreateDeviceProxyData.pPresentationParameters = pPresentationParameters;
    g_EmuD3D8CreateDeviceProxyData.ppReturnedDeviceInterface = ppReturnedDeviceInterface;

    // Signal proxy thread, and wait for completion
    g_EmuD3D8CreateDeviceProxyData.bReady = true;

    while(g_EmuD3D8CreateDeviceProxyData.bReady)
        Sleep(10);

    EmuSwapFS();   // XBox FS

    return g_EmuD3D8CreateDeviceProxyData.hRet;
}

// ******************************************************************
// * func: EmuIDirect3D8_GetAdapterModeCount
// ******************************************************************
UINT WINAPI XTL::EmuIDirect3D8_GetAdapterModeCount
(
    UINT                        Adapter
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3D8_GetAdapterModeCount\n"
               "(\n"
               "   Adapter                   : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Adapter);
    }
    #endif

    // NOTE: WARNING: We should return only modes that should exist on a real
    // Xbox. This could even be configurable, if desirable.

    // ******************************************************************
    // * redirect to windows d3d
    // ******************************************************************
    UINT ret = g_pD3D8->GetAdapterModeCount(g_XBVideo.GetDisplayAdapter());

    EmuSwapFS();   // XBox FS

    return ret;
}

// ******************************************************************
// * func: EmuIDirect3D8_GetAdapterDisplayMode
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3D8_GetAdapterDisplayMode
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
// * func: EmuIDirect3D8_EnumAdapterModes
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3D8_EnumAdapterModes
(
    UINT                        Adapter,
    UINT                        Mode,
    X_D3DDISPLAYMODE           *pMode
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3D8_EnumAdapterModes\n"
               "(\n"
               "   Adapter                   : 0x%.08X\n"
               "   Mode                      : 0x%.08X\n"
               "   pMode                     : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Adapter, Mode, pMode);
    }
    #endif

    // NOTE: WARNING: We should probably only return valid xbox display modes,
    // this should be coordinated with GetAdapterModeCount, etc.

    // ******************************************************************
    // * redirect to windows d3d
    // ******************************************************************
    HRESULT hRet = g_pD3D8->EnumAdapterModes(g_XBVideo.GetDisplayAdapter(), Mode, (D3DDISPLAYMODE*)pMode);

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
VOID WINAPI XTL::EmuIDirect3D8_KickOffAndWaitForIdle()
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
// * func: EmuIDirect3DDevice8_AddRef
// ******************************************************************
ULONG WINAPI XTL::EmuIDirect3DDevice8_AddRef()
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_AddRef()\n", GetCurrentThreadId());
    }
    #endif

    ULONG ret = g_pD3DDevice8->AddRef();

    EmuSwapFS();   // XBox FS

    return ret;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_BeginStateBlock
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_BeginStateBlock()
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_BeginStateBlock()\n", GetCurrentThreadId());
    }
    #endif

    ULONG ret = g_pD3DDevice8->BeginStateBlock();

    EmuSwapFS();   // XBox FS

    return ret;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_CaptureStateBlock
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_CaptureStateBlock(DWORD Token)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_CaptureStateBlock\n"
               "(\n"
               "   Token               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Token);
    }
    #endif

    ULONG ret = g_pD3DDevice8->CaptureStateBlock(Token);

    EmuSwapFS();   // XBox FS

    return ret;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_ApplyStateBlock
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_ApplyStateBlock(DWORD Token)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_ApplyStateBlock\n"
               "(\n"
               "   Token               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Token);
    }
    #endif

    ULONG ret = g_pD3DDevice8->ApplyStateBlock(Token);

    EmuSwapFS();   // XBox FS

    return ret;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_EndStateBlock
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_EndStateBlock(DWORD *pToken)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_EndStateBlock\n"
               "(\n"
               "   pToken              : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pToken);
    }
    #endif

    ULONG ret = g_pD3DDevice8->EndStateBlock(pToken);

    EmuSwapFS();   // XBox FS

    return ret;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_CopyRects
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_CopyRects
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
HRESULT WINAPI XTL::EmuIDirect3DDevice8_CreateImageSurface
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
// * func: EmuIDirect3DDevice8_GetBackBuffer2
// ******************************************************************
XTL::X_D3DSurface* WINAPI XTL::EmuIDirect3DDevice8_GetBackBuffer2
(
    INT                 BackBuffer
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetBackBuffer2\n"
               "(\n"
               "   BackBuffer          : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), BackBuffer);
    }
    #endif

    /* Temporarily? removed
    X_D3DSurface *pBackBuffer = new X_D3DSurface();

    if(BackBuffer == -1)
    {
        static IDirect3DSurface8 *pCachedPrimarySurface = 0;

        if(pCachedPrimarySurface == 0)
        {
            // create a buffer to return
            // TODO: Verify the surface is always 640x480
            g_pD3DDevice8->CreateImageSurface(640, 480, D3DFMT_A8R8G8B8, &pBackBuffer->EmuSurface8);
        } 
        else
            pBackBuffer->EmuSurface8 = pCachedPrimarySurface;

        HRESULT hRet = g_pD3DDevice8->GetFrontBuffer(pBackBuffer->EmuSurface8);

        if(FAILED(hRet))
        {
            EmuWarning("Could not retrieve primary surface, using backbuffer");
            pBackBuffer->EmuSurface8->Release();
            pBackBuffer->EmuSurface8 = 0;
            BackBuffer = 0;
        }

        // Debug: Save this image temporarily
        //D3DXSaveSurfaceToFile("C:\\Aaron\\Textures\\FrontBuffer.bmp", D3DXIFF_BMP, pBackBuffer->EmuSurface8, NULL, NULL);
    }

    if(BackBuffer != -1)
        g_pD3DDevice8->GetBackBuffer(BackBuffer, D3DBACKBUFFER_TYPE_MONO, &(pBackBuffer->EmuSurface8));
    */

    X_D3DSurface *pBackBuffer = new X_D3DSurface();

    if(BackBuffer == -1)
        BackBuffer = 0;

    HRESULT hRet = g_pD3DDevice8->GetBackBuffer(BackBuffer, D3DBACKBUFFER_TYPE_MONO, &(pBackBuffer->EmuSurface8));

    if(FAILED(hRet))
        EmuCleanup("Unable to retrieve back buffer");

    EmuSwapFS();   // Xbox FS

    return pBackBuffer;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetBackBuffer
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_GetBackBuffer
(
    INT                 BackBuffer,
    D3DBACKBUFFER_TYPE  Type,
    X_D3DSurface      **ppBackBuffer
)
{
    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        EmuSwapFS();   // Win2k/XP FS
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetBackBuffer\n"
               "(\n"
               "   BackBuffer          : 0x%.08X\n"
               "   Type                : 0x%.08X\n"
               "   ppBackBuffer        : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), BackBuffer, Type, ppBackBuffer);
        EmuSwapFS();   // Xbox FS
    }
    #endif

    *ppBackBuffer = EmuIDirect3DDevice8_GetBackBuffer2(BackBuffer);

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetViewport
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_SetViewport
(
    CONST D3DVIEWPORT8 *pViewport
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetViewport\n"
               "(\n"
               "   pViewport           : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pViewport);
    }
    #endif

    HRESULT hRet = g_pD3DDevice8->SetViewport(pViewport);

    EmuSwapFS();   // Xbox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetShaderConstantMode
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_SetShaderConstantMode
(
    DWORD               dwMode    // TODO: Fill out enumeration
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetShaderConstantMode\n"
               "(\n"
               "   dwMode              : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), dwMode);
    }
    #endif

    // TODO: Actually implement this

    EmuSwapFS();   // Xbox FS

    return S_OK;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetRenderTarget
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_GetRenderTarget
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
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetRenderTarget\n"
               "(\n"
               "   ppRenderTarget      : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), ppRenderTarget);
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
XTL::X_D3DSurface * WINAPI XTL::EmuIDirect3DDevice8_GetRenderTarget2()
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
HRESULT WINAPI XTL::EmuIDirect3DDevice8_GetDepthStencilSurface
(
    X_D3DSurface  **ppZStencilSurface
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetDepthStencilSurface\n"
               "(\n"
               "   ppZStencilSurface   : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), ppZStencilSurface);
    }
    #endif

    IDirect3DSurface8 *pSurface8 = g_pCachedZStencilSurface->EmuSurface8;

    if(pSurface8 != 0)
        pSurface8->AddRef();

    *ppZStencilSurface = g_pCachedZStencilSurface;

    EmuSwapFS();   // Xbox FS

    return D3D_OK;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetDepthStencilSurface2
// ******************************************************************
XTL::X_D3DSurface * WINAPI XTL::EmuIDirect3DDevice8_GetDepthStencilSurface2()
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

    if(pSurface8 != 0)
        pSurface8->AddRef();

    EmuSwapFS();   // Xbox FS

    return g_pCachedZStencilSurface;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetTile
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_GetTile
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
HRESULT WINAPI XTL::EmuIDirect3DDevice8_SetTileNoWait
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
HRESULT WINAPI XTL::EmuIDirect3DDevice8_CreateVertexShader
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
        EmuWarning("VertexShader was not really created!");

        hRet = D3D_OK;
    }

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexShaderConstant
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_SetVertexShaderConstant
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

    HRESULT hRet;

    // ******************************************************************
    // * redirect to windows d3d
    // ******************************************************************
    /*
    HRESULT hRet = g_pD3DDevice8->SetVertexShaderConstant
    (
        Register,
        pConstantData,
        ConstantCount
    );

    if(FAILED(hRet))
    */
    {
        printf("*Warning* we're lying about setting a vertex shader constant!\n");

        hRet = D3D_OK;
    }

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexShaderConstant1
// ******************************************************************
VOID __fastcall XTL::EmuIDirect3DDevice8_SetVertexShaderConstant1
(
    INT         Register,
    CONST PVOID pConstantData
)
{
    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        EmuSwapFS();   // Win2k/XP FS
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetVertexShaderConstant1\n"
               "(\n"
               "   Register            : 0x%.08X\n"
               "   pConstantData       : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Register, pConstantData);
        EmuSwapFS();   // XBox FS
    }
    #endif

    XTL::EmuIDirect3DDevice8_SetVertexShaderConstant(Register - 96, pConstantData, 1);

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexShaderConstant4
// ******************************************************************
VOID __fastcall XTL::EmuIDirect3DDevice8_SetVertexShaderConstant4
(
    INT         Register,
    CONST PVOID pConstantData
)
{
    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        EmuSwapFS();   // Win2k/XP FS
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetVertexShaderConstant4\n"
               "(\n"
               "   Register            : 0x%.08X\n"
               "   pConstantData       : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Register, pConstantData);
        EmuSwapFS();   // XBox FS
    }
    #endif

    XTL::EmuIDirect3DDevice8_SetVertexShaderConstant(Register - 96, pConstantData, 4);

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreatePixelShader
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_CreatePixelShader
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
HRESULT WINAPI XTL::EmuIDirect3DDevice8_SetPixelShader
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
XTL::X_D3DResource * WINAPI XTL::EmuIDirect3DDevice8_CreateTexture2
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
    X_D3DTexture *pTexture;

    EmuIDirect3DDevice8_CreateTexture(Width, Height, Levels, Usage, Format, D3DPOOL_MANAGED, &pTexture);

    return pTexture;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateTexture
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_CreateTexture
(
    UINT            Width,
    UINT            Height,
    UINT            Levels,
    DWORD           Usage,
    D3DFORMAT       Format,
    D3DPOOL         Pool,
    X_D3DTexture  **ppTexture
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
    else if(PCFormat == D3DFMT_P8)
    {
        printf("*Warning* D3DFMT_P8 is an unsupported texture format!\n");
        PCFormat = D3DFMT_X8R8G8B8;
    }
    else if(PCFormat == D3DFMT_D24S8)
    {
        printf("*Warning* D3DFMT_D24S8 is an unsupported texture format!\n");
        PCFormat = D3DFMT_X8R8G8B8;
    }
    else if(PCFormat == D3DFMT_YUY2)
    {
        // cache the overlay size
        g_dwOverlayW = Width;
        g_dwOverlayH = Height;
    }

    HRESULT hRet;

    if(PCFormat != D3DFMT_YUY2 || g_bSupportsYUY2)
    {
        EmuAdjustPower2(&Width, &Height);

        *ppTexture = new X_D3DTexture();

        // ******************************************************************
        // * redirect to windows d3d
        // ******************************************************************
        hRet = g_pD3DDevice8->CreateTexture
        (
            Width, Height, Levels, 
            0,  // TODO: Xbox Allows a border to be drawn (maybe hack this in software ;[)
            PCFormat, D3DPOOL_MANAGED, &((*ppTexture)->EmuTexture8)
        );

        if(FAILED(hRet))
            printf("*Warning* CreateTexture FAILED\n");
    }
    else
    {
        // If YUY2 is not supported in hardware, we'll actually mark this as a special fake texture (set highest bit)
        *ppTexture = (X_D3DTexture*)((uint32)(new uint08[g_dwOverlayW*g_dwOverlayH*2]) | 0x80000000);

        hRet = D3D_OK;
    }

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateVolumeTexture
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_CreateVolumeTexture
(
    UINT                 Width,
    UINT                 Height,
    UINT                 Depth,
    UINT                 Levels,
    DWORD                Usage,
    D3DFORMAT            Format,
    D3DPOOL              Pool,
    X_D3DVolumeTexture **ppVolumeTexture
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_CreateVolumeTexture\n"
               "(\n"
               "   Width               : 0x%.08X\n"
               "   Height              : 0x%.08X\n"
               "   Depth               : 0x%.08X\n"
               "   Levels              : 0x%.08X\n"
               "   Usage               : 0x%.08X\n"
               "   Format              : 0x%.08X\n"
               "   Pool                : 0x%.08X\n"
               "   ppVolumeTexture     : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture);
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
    else if(PCFormat == D3DFMT_P8)
    {
        printf("*Warning* D3DFMT_P8 is an unsupported texture format!\n");
        PCFormat = D3DFMT_X8R8G8B8;
    }
    else if(PCFormat == D3DFMT_D24S8)
    {
        printf("*Warning* D3DFMT_D24S8 is an unsupported texture format!\n");
        PCFormat = D3DFMT_X8R8G8B8;
    }
    else if(PCFormat == D3DFMT_YUY2)
    {
        // cache the overlay size
        g_dwOverlayW = Width;
        g_dwOverlayH = Height;
    }

    HRESULT hRet;

    if(PCFormat != D3DFMT_YUY2 || g_bSupportsYUY2)
    {
        EmuAdjustPower2(&Width, &Height);

        *ppVolumeTexture = new X_D3DVolumeTexture();

        // ******************************************************************
        // * redirect to windows d3d
        // ******************************************************************
        hRet = g_pD3DDevice8->CreateVolumeTexture
        (
            Width, Height, Depth, Levels, 
            0,  // TODO: Xbox Allows a border to be drawn (maybe hack this in software ;[)
            PCFormat, D3DPOOL_MANAGED, &((*ppVolumeTexture)->EmuVolumeTexture8)
        );

        if(FAILED(hRet))
            printf("*Warning* CreateVolumeTexture FAILED\n");
    }
    else
    {
        // If YUY2 is not supported in hardware, we'll actually mark this as a special fake texture (set highest bit)
        *ppVolumeTexture = (X_D3DVolumeTexture*)((uint32)(new uint08[g_dwOverlayW*g_dwOverlayH*2]) | 0x80000000);

        hRet = D3D_OK;
    }

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateCubeTexture
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_CreateCubeTexture
(
    UINT                 EdgeLength,
    UINT                 Levels,
    DWORD                Usage,
    D3DFORMAT            Format,
    D3DPOOL              Pool,
    X_D3DCubeTexture  **ppCubeTexture
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_CreateCubeTexture\n"
               "(\n"
               "   EdgeLength          : 0x%.08X\n"
               "   Levels              : 0x%.08X\n"
               "   Usage               : 0x%.08X\n"
               "   Format              : 0x%.08X\n"
               "   Pool                : 0x%.08X\n"
               "   ppCubeTexture       : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture);
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
    else if(PCFormat == D3DFMT_P8)
    {
        printf("*Warning* D3DFMT_P8 is an unsupported texture format!\n");
        PCFormat = D3DFMT_X8R8G8B8;
    }
    else if(PCFormat == D3DFMT_D24S8)
    {
        printf("*Warning* D3DFMT_D24S8 is an unsupported texture format!\n");
        PCFormat = D3DFMT_X8R8G8B8;
    }
    else if(PCFormat == D3DFMT_YUY2)
    {
        EmuCleanup("YUV not supported for cube textures");
    }

    *ppCubeTexture = new X_D3DCubeTexture();

    // ******************************************************************
    // * redirect to windows d3d
    // ******************************************************************
    HRESULT hRet = g_pD3DDevice8->CreateCubeTexture
    (
        EdgeLength, Levels, 
        0,  // TODO: Xbox Allows a border to be drawn (maybe hack this in software ;[)
        PCFormat, D3DPOOL_MANAGED, &((*ppCubeTexture)->EmuCubeTexture8)
    );

    if(FAILED(hRet))
        printf("*Warning* CreateCubeTexture FAILED\n");

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateIndexBuffer
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_CreateIndexBuffer
(
    UINT                 Length,
    DWORD                Usage,
    D3DFORMAT            Format,
    D3DPOOL              Pool,
    X_D3DIndexBuffer   **ppIndexBuffer
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_CreateIndexBuffer\n"
               "(\n"
               "   Length              : 0x%.08X\n"
               "   Usage               : 0x%.08X\n"
               "   Format              : 0x%.08X\n"
               "   Pool                : 0x%.08X\n"
               "   ppIndexBuffer       : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Length, Usage, Format, Pool, ppIndexBuffer);
    }
    #endif

    *ppIndexBuffer = new X_D3DIndexBuffer();

    // ******************************************************************
    // * redirect to windows d3d
    // ******************************************************************
    HRESULT hRet = g_pD3DDevice8->CreateIndexBuffer
    (
        Length, D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_MANAGED, &((*ppIndexBuffer)->EmuIndexBuffer8)
    );

    if(FAILED(hRet))
        printf("*Warning* CreateIndexBuffer FAILED\n");

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetIndices
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_SetIndices
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

    IDirect3DIndexBuffer8 *pIndexBuffer = 0;

    if(pIndexData != 0)
    {
        EmuVerifyResourceIsRegistered(pIndexData);

        pIndexBuffer = pIndexData->EmuIndexBuffer8;
    }

    HRESULT hRet = g_pD3DDevice8->SetIndices(pIndexBuffer, BaseVertexIndex);

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTexture
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_SetTexture
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
VOID __fastcall XTL::EmuIDirect3DDevice8_SwitchTexture
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
HRESULT WINAPI XTL::EmuIDirect3DDevice8_GetDisplayMode
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
HRESULT WINAPI XTL::EmuIDirect3DDevice8_Clear
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
               "   Z                   : %f\n"
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

        if(Flags & 0x000000f0)
            newFlags |= D3DCLEAR_TARGET;

        if(Flags & 0x00000001)
            newFlags |= D3DCLEAR_ZBUFFER;

        if(Flags & 0x00000002)
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
HRESULT WINAPI XTL::EmuIDirect3DDevice8_Present
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
HRESULT WINAPI XTL::EmuIDirect3DDevice8_Swap
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
        EmuCleanup("XTL::EmuIDirect3DDevice8_Swap: Flags != 0");

    HRESULT hRet = g_pD3DDevice8->Present(0, 0, 0, 0);

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DResource8_Register
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DResource8_Register
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

    HRESULT hRet;

    X_D3DResource *pResource = (X_D3DResource*)pThis;

    DWORD dwCommonType = pResource->Common & X_D3DCOMMON_TYPE_MASK;

	// Add the offset of the current texture to the base
	pBase = (PVOID)((DWORD)pBase+pThis->Data);

    // ******************************************************************
    // * Determine the resource type, and initialize
    // ******************************************************************
    switch(dwCommonType)
    {
        case X_D3DCOMMON_TYPE_VERTEXBUFFER:
        {
            #ifdef _DEBUG_TRACE
            printf("EmuIDirect3DResource8_Register (0x%X) : Creating VertexBuffer...\n", GetCurrentThreadId());
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

            #ifdef _DEBUG_TRACE
            printf("EmuIDirect3DResource8_Register (0x%X) : Successfully Created VertexBuffer\n", GetCurrentThreadId());
            #endif
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

                if(FAILED(hRet))
                    EmuCleanup("CreateIndexBuffer failed");

                BYTE *pData = 0;

                hRet = pResource->EmuIndexBuffer8->Lock(0, dwSize, &pData, 0);

                if(FAILED(hRet))
                    EmuCleanup("IndexBuffer Lock failed");

                memcpy(pData, (void*)pBase, dwSize);

                pResource->EmuIndexBuffer8->Unlock();

                pResource->Data = (ULONG)pData;
            }
        }
        break;

        case X_D3DCOMMON_TYPE_PUSHBUFFER:
        {
            printf("*Warning* X_D3DCOMMON_TYPE_PUSHBUFFER is not yet implemented\n");

            X_D3DPushBuffer *pPushBuffer = (X_D3DPushBuffer*)pResource;
        }
        break;

        case X_D3DCOMMON_TYPE_SURFACE:
        case X_D3DCOMMON_TYPE_TEXTURE:
        {
            #ifdef _DEBUG_TRACE
            if(dwCommonType == X_D3DCOMMON_TYPE_SURFACE)
                printf("EmuIDirect3DResource8_Register :-> Surface...\n");
            else
                printf("EmuIDirect3DResource8_Register :-> Texture...\n");
            #endif

            X_D3DPixelContainer *pPixelContainer = (X_D3DPixelContainer*)pResource;

            X_D3DFORMAT X_Format = (X_D3DFORMAT)((pPixelContainer->Format & X_D3DFORMAT_FORMAT_MASK) >> X_D3DFORMAT_FORMAT_SHIFT);
            D3DFORMAT   Format   = EmuXB2PC_D3DFormat(X_Format);

            // TODO: HACK: Temporary?
            if(X_Format == 0x2E)
            {
                X_Format = 0x12;
                Format   = D3DFMT_A8R8G8B8;
            }

            DWORD dwWidth, dwHeight, dwBPP, dwDepth = 1, dwPitch = 0, dwMipMapLevels = 1;
            BOOL  bSwizzled = FALSE, bCompressed = FALSE, dwCompressedSize = 0;
            BOOL  bCubemap = pPixelContainer->Format & X_D3DFORMAT_CUBEMAP;

            if(bCubemap)
                EmuCleanup("Cubemaps are temporarily unsupported");

            // ******************************************************************
            // * Interpret Width/Height/BPP
            // ******************************************************************
            if(X_Format == 0x07 /* X_D3DFMT_X8R8G8B8 */ || X_Format == 0x06 /* X_D3DFMT_A8R8G8B8 */)
            {
                bSwizzled = TRUE;

                // Swizzled 32 Bit
                dwWidth  = 1 << ((pPixelContainer->Format & X_D3DFORMAT_USIZE_MASK) >> X_D3DFORMAT_USIZE_SHIFT);
                dwHeight = 1 << ((pPixelContainer->Format & X_D3DFORMAT_VSIZE_MASK) >> X_D3DFORMAT_VSIZE_SHIFT);
                dwDepth  = 1 << ((pPixelContainer->Format & X_D3DFORMAT_PSIZE_MASK) >> X_D3DFORMAT_PSIZE_SHIFT);
                dwPitch  = dwWidth*4;
                dwBPP = 4;
            }
            else if(X_Format == 0x05 /* X_D3DFMT_R5G6B5 */ || X_Format == 0x04 /* X_D3DFMT_A4R4G4B4 */)
            {
                bSwizzled = TRUE;

                // Swizzled 16 Bit
                dwWidth  = 1 << ((pPixelContainer->Format & X_D3DFORMAT_USIZE_MASK) >> X_D3DFORMAT_USIZE_SHIFT);
                dwHeight = 1 << ((pPixelContainer->Format & X_D3DFORMAT_VSIZE_MASK) >> X_D3DFORMAT_VSIZE_SHIFT);
                dwDepth  = 1 << ((pPixelContainer->Format & X_D3DFORMAT_PSIZE_MASK) >> X_D3DFORMAT_PSIZE_SHIFT);
                dwPitch  = dwWidth*2;
                dwBPP = 2;
            }
            else if(X_Format == 0x12 /* X_D3DFORMAT_A8R8G8B8 */ || X_Format == 0x2E /* D3DFMT_LIN_D24S8 */)
            {
                // Linear 32 Bit
                dwWidth  = (pPixelContainer->Size & X_D3DSIZE_WIDTH_MASK) + 1;
                dwHeight = ((pPixelContainer->Size & X_D3DSIZE_HEIGHT_MASK) >> X_D3DSIZE_HEIGHT_SHIFT) + 1;
                dwPitch  = (((pPixelContainer->Size & X_D3DSIZE_PITCH_MASK) >> X_D3DSIZE_PITCH_SHIFT)+1)*64;
                dwBPP = 4;
            }
            else if(X_Format == 0x11 /* D3DFMT_LIN_R5G6B5 */)
            {
                // Linear 16 Bit
                dwWidth  = (pPixelContainer->Size & X_D3DSIZE_WIDTH_MASK) + 1;
                dwHeight = ((pPixelContainer->Size & X_D3DSIZE_HEIGHT_MASK) >> X_D3DSIZE_HEIGHT_SHIFT) + 1;
                dwPitch  = (((pPixelContainer->Size & X_D3DSIZE_PITCH_MASK) >> X_D3DSIZE_PITCH_SHIFT)+1)*64;
                dwBPP = 2;
            }
            else if(X_Format == 0x0C /* D3DFMT_DXT1 */ || X_Format == 0x0E /* D3DFMT_DXT2 */ || X_Format == 0x0F /* D3DFMT_DXT3 */)
            {
                bCompressed = TRUE;

                // Compressed
                dwWidth  = 1 << ((pPixelContainer->Format & X_D3DFORMAT_USIZE_MASK) >> X_D3DFORMAT_USIZE_SHIFT);
                dwHeight = 1 << ((pPixelContainer->Format & X_D3DFORMAT_VSIZE_MASK) >> X_D3DFORMAT_VSIZE_SHIFT);
                dwDepth  = 1 << ((pPixelContainer->Format & X_D3DFORMAT_PSIZE_MASK) >> X_D3DFORMAT_PSIZE_SHIFT);

                // D3DFMT_DXT2->D3DFMT_DXT5 : 128bits per block/per 16 texels
                dwCompressedSize = dwWidth*dwHeight;

                if(X_Format == 0x0C)    // D3DFMT_DXT1 : 64bits per block/per 16 texels
                    dwCompressedSize /= 2;

                dwMipMapLevels = (pPixelContainer->Format & X_D3DFORMAT_MIPMAP_MASK) >> X_D3DFORMAT_MIPMAP_SHIFT;
            }
            else
            {
                EmuCleanup("0x%.08X is not a supported format!\n", X_Format);
            }

            // ******************************************************************
            // * Create the happy little texture
            // ******************************************************************
            if(dwCommonType == X_D3DCOMMON_TYPE_SURFACE)
            {
                hRet = g_pD3DDevice8->CreateImageSurface(dwWidth, dwHeight, Format, &pResource->EmuSurface8);
            }
            else
            {
                // TODO: HACK: Figure out why this is necessary!
                // TODO: This is necessary for DXT1 textures at least (4x4 blocks minimum)
                if(dwWidth < 4)
                {
                    printf("*Warning* expanding texture width (%d->4)\n", dwWidth);
                    dwWidth = 4;
                    
                    dwMipMapLevels = 3;
                }

                if(dwHeight < 4)
                {
                    printf("*Warning* expanding texture height (%d->4)\n", dwHeight);
                    dwHeight = 4;

                    dwMipMapLevels = 3;
                }

                #ifdef _DEBUG_TRACE
                printf("CreateTexture(%d, %d, %d, 0, %d, D3DPOOL_MANAGED, 0x%.08X)\n", dwWidth, dwHeight,
                    dwMipMapLevels, Format, &pResource->EmuTexture8);
                #endif

                hRet = g_pD3DDevice8->CreateTexture
                (
                    dwWidth, dwHeight, dwMipMapLevels, 0, Format,
                    D3DPOOL_MANAGED, &pResource->EmuTexture8
                );

                if(FAILED(hRet))
                    EmuCleanup("CreateTexture failed");
            }

            D3DLOCKED_RECT LockedRect;

            // ******************************************************************
            // * Copy over data (deswizzle if necessary)
            // ******************************************************************
            if(dwCommonType == X_D3DCOMMON_TYPE_SURFACE)
                hRet = pResource->EmuSurface8->LockRect(&LockedRect, NULL, 0);
            else
                hRet = pResource->EmuTexture8->LockRect(0, &LockedRect, NULL, 0);

            RECT  iRect  = {0,0,0,0};
            POINT iPoint = {0,0};

            if(bSwizzled)
            {
                XTL::EmuXGUnswizzleRect
                (
                    pBase, dwWidth, dwHeight, dwDepth, LockedRect.pBits, 
                    LockedRect.Pitch, iRect, iPoint, dwBPP
                );
            }
            else if(bCompressed)
            {
                memcpy(LockedRect.pBits, pBase, dwCompressedSize);
            }
            else
            {
                BYTE *pDest = (BYTE*)LockedRect.pBits;
                BYTE *pSrc  = (BYTE*)pBase;

                if((DWORD)LockedRect.Pitch == dwPitch && dwPitch == dwWidth*dwBPP)
                    memcpy(pDest, pSrc, dwWidth*dwHeight*dwBPP);
                else
                {
                    // TODO: Faster copy (maybe unnecessary)
                    for(DWORD v=0;v<dwHeight;v++)
                    {
                        memcpy(pDest, pSrc, dwWidth*dwBPP);

                        pDest += LockedRect.Pitch;
                        pSrc  += dwPitch;
                    }
                }
            }

            if(dwCommonType == X_D3DCOMMON_TYPE_SURFACE)
                pResource->EmuSurface8->UnlockRect();
            else
                pResource->EmuTexture8->UnlockRect(0);

            // Debug Texture Dumping
            /*
            if(dwCommonType == X_D3DCOMMON_TYPE_SURFACE)
            {
                static int dwDumpSurface = 0;

                char szBuffer[255];

                sprintf(szBuffer, "C:\\Aaron\\Textures\\Surface%.03d.bmp", dwDumpSurface++);

                D3DXSaveSurfaceToFile(szBuffer, D3DXIFF_BMP, pResource->EmuSurface8, NULL, NULL);
            }
            else
            {
                static int dwDumpTex = 0;

                char szBuffer[255];

                sprintf(szBuffer, "C:\\Aaron\\Textures\\Texture%.03d.bmp", dwDumpTex++);

                D3DXSaveTextureToFile(szBuffer, D3DXIFF_BMP, pResource->EmuTexture8, NULL);
            }
            //*/
        }
        break;

        default:
            EmuCleanup("IDirect3DResource8::Register -> Common Type 0x%.08X not yet supported", dwCommonType);
    }

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DResource8_AddRef
// ******************************************************************
ULONG WINAPI XTL::EmuIDirect3DResource8_AddRef
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
        printf("EmuD3D8 (0x%X): EmuIDirect3DResource8_AddRef\n"
               "(\n"
               "   pThis               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis);
    }
    #endif

    ULONG uRet = 0;

    IDirect3DResource8 *pResource8 = pThis->EmuResource8;

    if(pThis->Lock == 0x8000BEEF)
        uRet = ++pThis->Lock;
    else if(pResource8 != 0)
        uRet = pResource8->AddRef();

    EmuSwapFS();   // XBox FS

    return uRet;
}

// ******************************************************************
// * func: EmuIDirect3DResource8_Release
// ******************************************************************
ULONG WINAPI XTL::EmuIDirect3DResource8_Release
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

    ULONG uRet = 0;

    IDirect3DResource8 *pResource8 = pThis->EmuResource8;

    if(pThis->Lock == 0x8000BEEF)
    {
        delete[] (PVOID)pThis->Data;
        uRet = --pThis->Lock;
    }
    else if(pResource8 != 0)
    {
        uRet = pResource8->Release();

        if(uRet == 0)
        {
            #ifdef _DEBUG_TRACE
            printf("EmuIDirect3DResource8_Release (0x%X): Cleaned up a Resource!\n", GetCurrentThreadId());
            #endif
            delete pThis;
        }
    }

    EmuSwapFS();   // XBox FS

    return uRet;
}

// ******************************************************************
// * func: EmuIDirect3DResource8_IsBusy
// ******************************************************************
BOOL WINAPI XTL::EmuIDirect3DResource8_IsBusy
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
VOID WINAPI XTL::EmuGet2DSurfaceDesc
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

    D3DSURFACE_DESC SurfaceDesc;

    ZeroMemory(&SurfaceDesc, sizeof(SurfaceDesc));

    HRESULT hRet;

    if(dwLevel == 0xFEFEFEFE)
        hRet = pPixelContainer->EmuSurface8->GetDesc(&SurfaceDesc);
    else
        hRet = pPixelContainer->EmuTexture8->GetLevelDesc(dwLevel, &SurfaceDesc);

    // ******************************************************************
    // * Rearrange into windows format (remove D3DPool)
    // ******************************************************************
    {
        // Convert Format (PC->Xbox)
        pDesc->Format = EmuPC2XB_D3DFormat(SurfaceDesc.Format);
        pDesc->Type   = SurfaceDesc.Type;

        if(pDesc->Type > 7)
            EmuCleanup("EmuGet2DSurfaceDesc: pDesc->Type > 7");

        pDesc->Usage  = SurfaceDesc.Usage;
        pDesc->Size   = SurfaceDesc.Size;

        // TODO: Convert from Xbox to PC!!
        if(SurfaceDesc.MultiSampleType == D3DMULTISAMPLE_NONE)
            pDesc->MultiSampleType = (XTL::D3DMULTISAMPLE_TYPE)0x0011;
        else
            EmuCleanup("EmuGet2DSurfaceDesc Unknown Multisample format! (%d)", SurfaceDesc.MultiSampleType);

        pDesc->Width  = SurfaceDesc.Width;
        pDesc->Height = SurfaceDesc.Height;
    }

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuGet2DSurfaceDescD
// ******************************************************************
VOID WINAPI XTL::EmuGet2DSurfaceDescD
(
    X_D3DPixelContainer *pPixelContainer,
    X_D3DSURFACE_DESC   *pDesc
)
{
    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        EmuSwapFS();   // Win2k/XP FS
        printf("EmuD3D8 (0x%X): EmuGet2DSurfaceDescD\n"
               "(\n"
               "   pPixelContainer     : 0x%.08X\n"
               "   pDesc               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pPixelContainer, pDesc);
        EmuSwapFS();   // Xbox FS
    }
    #endif

    EmuGet2DSurfaceDesc(pPixelContainer, 0xFEFEFEFE, pDesc);

    return;
}

// ******************************************************************
// * func: EmuIDirect3DSurface8_GetDesc
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DSurface8_GetDesc
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

    HRESULT hRet;

    if((uint32)pThis & 0x80000000)
    {
        pDesc->Format = EmuPC2XB_D3DFormat(D3DFMT_YUY2);
        pDesc->Height = g_dwOverlayH;
        pDesc->Width  = g_dwOverlayW;
        pDesc->MultiSampleType = (D3DMULTISAMPLE_TYPE)0;
        pDesc->Size   = g_dwOverlayW*g_dwOverlayH*2;
        pDesc->Type   = D3DRTYPE_SURFACE;
        pDesc->Usage  = 0;

        hRet = D3D_OK;
    }
    else
    {
        EmuVerifyResourceIsRegistered(pThis);

        IDirect3DSurface8 *pSurface8 = pThis->EmuSurface8;

        D3DSURFACE_DESC SurfaceDesc;

        hRet = pSurface8->GetDesc(&SurfaceDesc);

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
                pDesc->MultiSampleType = (XTL::D3DMULTISAMPLE_TYPE)0x0011;
            else
                EmuCleanup("EmuIDirect3DSurface8_GetDesc Unknown Multisample format! (%d)", SurfaceDesc.MultiSampleType);

            pDesc->Width  = SurfaceDesc.Width;
            pDesc->Height = SurfaceDesc.Height;
        }
    }

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DSurface8_LockRect
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DSurface8_LockRect
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
            }
    #endif

    HRESULT hRet;

    if((uint32)pThis & 0x80000000)
    {
        pLockedRect->Pitch = g_dwOverlayW*2;
        pLockedRect->pBits = (void*)((uint32)pThis & 0x7FFFFFFF);

        hRet = D3D_OK;
    }
    else
    {
        if(Flags & 0x40)
            printf("*Warning* D3DLOCK_TILED ignored!\n");

        EmuVerifyResourceIsRegistered(pThis);

        IDirect3DSurface8 *pSurface8 = pThis->EmuSurface8;

        DWORD NewFlags = 0;

        if(Flags & 0x80)
            NewFlags |= D3DLOCK_READONLY;

        if(Flags & !(0x80 | 0x40))
            EmuCleanup("EmuIDirect3DSurface8_LockRect: Unknown Flags! (0x%.08X)", Flags);

        // Remove old lock(s)
        pSurface8->UnlockRect();

        hRet = pSurface8->LockRect(pLockedRect, pRect, NewFlags);

        if(FAILED(hRet))
            printf("*Warning* LockRect failed\n");
    }

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DBaseTexture8_GetLevelCount
// ******************************************************************
DWORD WINAPI XTL::EmuIDirect3DBaseTexture8_GetLevelCount
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

    DWORD dwRet = pBaseTexture8->GetLevelCount();

    EmuSwapFS();   // XBox FS

    return dwRet;
}

// ******************************************************************
// * func: EmuIDirect3DTexture8_GetSurfaceLevel2
// ******************************************************************
XTL::X_D3DResource * WINAPI XTL::EmuIDirect3DTexture8_GetSurfaceLevel2
(
    X_D3DTexture   *pThis,
    UINT            Level
)
{
    X_D3DSurface *pSurfaceLevel;

    // In a special situation, we are actually returning a memory ptr with high bit set
    if((uint32)pThis & 0x80000000)
        return pThis;

    EmuIDirect3DTexture8_GetSurfaceLevel(pThis, Level, &pSurfaceLevel);

    return pSurfaceLevel;
}

// ******************************************************************
// * func: EmuIDirect3DTexture8_LockRect
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DTexture8_LockRect
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
HRESULT WINAPI XTL::EmuIDirect3DTexture8_GetSurfaceLevel
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

    HRESULT hRet;

    // if highest bit is set, this is actually a raw memory pointer (for YUY2 simulation)
    if((uint32)pThis & 0x80000000)
    {
        *ppSurfaceLevel = (X_D3DSurface*)pThis;
        hRet = D3D_OK;
    }
    else
    {
        EmuVerifyResourceIsRegistered(pThis);

        IDirect3DTexture8 *pTexture8 = pThis->EmuTexture8;

        *ppSurfaceLevel = new X_D3DSurface();

        hRet = pTexture8->GetSurfaceLevel(Level, &((*ppSurfaceLevel)->EmuSurface8));
    }

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DVolumeTexture8_LockBox
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DVolumeTexture8_LockBox
(
    X_D3DVolumeTexture *pThis,
    UINT                Level,
    D3DLOCKED_BOX      *pLockedVolume,
    CONST D3DBOX       *pBox,
    DWORD               Flags
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DVolumeTexture8_LockBox\n"
               "(\n"
               "   pThis               : 0x%.08X\n"
               "   Level               : 0x%.08X\n"
               "   pLockedVolume       : 0x%.08X\n"
               "   pBox                : 0x%.08X\n"
               "   Flags               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, Level, pLockedVolume, pBox, Flags);
    }
    #endif

    EmuVerifyResourceIsRegistered(pThis);

    IDirect3DVolumeTexture8 *pVolumeTexture8 = pThis->EmuVolumeTexture8;

    HRESULT hRet = pVolumeTexture8->LockBox(Level, pLockedVolume, pBox, Flags);

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DCubeTexture8_LockRect
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DCubeTexture8_LockRect
(
    X_D3DCubeTexture   *pThis,
    D3DCUBEMAP_FACES    FaceType,
    UINT                Level,
    D3DLOCKED_RECT     *pLockedBox,
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
        printf("EmuD3D8 (0x%X): EmuIDirect3DCubeTexture8_LockRect\n"
               "(\n"
               "   pThis               : 0x%.08X\n"
               "   FaceType            : 0x%.08X\n"
               "   Level               : 0x%.08X\n"
               "   pLockedBox          : 0x%.08X\n"
               "   pRect               : 0x%.08X\n"
               "   Flags               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, FaceType, Level, pLockedBox, pRect, Flags);
    }
    #endif

    EmuVerifyResourceIsRegistered(pThis);

    IDirect3DCubeTexture8 *pCubeTexture8 = pThis->EmuCubeTexture8;

    HRESULT hRet = pCubeTexture8->LockRect(FaceType, Level, pLockedBox, pRect, Flags);

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_Release
// ******************************************************************
ULONG WINAPI XTL::EmuIDirect3DDevice8_Release()
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_Release();\n", GetCurrentThreadId());
    }
    #endif

    EmuCleanup("Release should use proxy...");
    ULONG uRet = g_pD3DDevice8->Release();

    EmuSwapFS();   // XBox FS

    return uRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateVertexBuffer
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_CreateVertexBuffer
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
XTL::X_D3DVertexBuffer* WINAPI XTL::EmuIDirect3DDevice8_CreateVertexBuffer2
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
// * func: EmuIDirect3DDevice8_EnableOverlay
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_EnableOverlay
(
    BOOL Enable
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_EnableOverlay\n"
               "(\n"
               "   Enable              : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Enable);
    }
    #endif

    if(g_bSupportsYUY2)
    {
        if(Enable)
        {
            // ******************************************************************
            // * Initialize Primary Surface
            // ******************************************************************
            {
                DDSURFACEDESC2 ddsd2;

                ZeroMemory(&ddsd2, sizeof(ddsd2));

                ddsd2.dwSize = sizeof(ddsd2);
                ddsd2.dwFlags = DDSD_CAPS;
                ddsd2.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_VIDEOMEMORY;
		            
	            HRESULT hRet = g_pDD7->CreateSurface(&ddsd2, &g_pDDSPrimary, 0);

                if(FAILED(hRet))
                    EmuCleanup("Could not create primary surface");
            }

            // ******************************************************************
            // * Initialize Overlay Surface
            // ******************************************************************
            {
                DDSURFACEDESC2 ddsd2;

                ZeroMemory(&ddsd2, sizeof(ddsd2));

                ddsd2.dwSize = sizeof(ddsd2);
                ddsd2.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
                ddsd2.ddsCaps.dwCaps = DDSCAPS_OVERLAY;
                ddsd2.dwWidth = g_dwOverlayW;
                ddsd2.dwHeight = g_dwOverlayH; 
                ddsd2.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
                ddsd2.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
                ddsd2.ddpfPixelFormat.dwFourCC = MAKEFOURCC('Y','U','Y','2');

                HRESULT hRet = g_pDD7->CreateSurface(&ddsd2, &g_pDDSOverlay7, NULL);

                if(FAILED(hRet))
                    EmuCleanup("Could not create overlay surface");
            }
        }
        else
        {
            // Cleanup Primary/Overlay Surfaces
            if(g_pDDSOverlay7 != 0)
            {
                g_pDDSOverlay7->Release();
                g_pDDSOverlay7 = 0;
            }
        
            if(g_pDDSPrimary != 0)
            {
                g_pDDSPrimary->Release();
                g_pDDSPrimary = 0;
            }
        }
    }

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_UpdateOverlay
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_UpdateOverlay
(
    X_D3DSurface *pSurface,
    CONST RECT   *SrcRect,
    CONST RECT   *DstRect,
    BOOL          EnableColorKey,
    D3DCOLOR      ColorKey
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_UpdateOverlay\n"
               "(\n"
               "   pSurface            : 0x%.08X\n"
               "   SrcRect             : 0x%.08X\n"
               "   DstRect             : 0x%.08X\n"
               "   EnableColorKey      : 0x%.08X\n"
               "   ColorKey            : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pSurface, SrcRect, DstRect, EnableColorKey, ColorKey);
    }
    #endif

    // ******************************************************************
    // * manually copy data over to overlay
    // ******************************************************************
    if(g_bSupportsYUY2)
    {
        D3DSURFACE_DESC SurfaceDesc;
        DDSURFACEDESC2  ddsd2;
        D3DLOCKED_RECT  LockedRect;

        pSurface->EmuSurface8->GetDesc(&SurfaceDesc);

        ZeroMemory(&ddsd2, sizeof(ddsd2));

        ddsd2.dwSize = sizeof(ddsd2);

        g_pDDSOverlay7->Lock(NULL, &ddsd2, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
        pSurface->EmuSurface8->UnlockRect();

        pSurface->EmuSurface8->LockRect(&LockedRect, NULL, NULL);

        // Copy Data
        {
            char *pDest = (char*)ddsd2.lpSurface;
            char *pSour = (char*)LockedRect.pBits;

            int w = SurfaceDesc.Width;
            int h = SurfaceDesc.Height;

            // TODO: sucker the game into rendering directly to the overlay :]
            for(int y=0;y<h;y++)
            {
                memcpy(pDest, pSour, w*2);

                pDest += ddsd2.lPitch;
                pSour += LockedRect.Pitch;
            }
        }

        g_pDDSOverlay7->Unlock(NULL);
    }

    // ******************************************************************
    // * update overlay!
    // ******************************************************************
    if(g_bSupportsYUY2)
    {
        RECT SourRect = {0, 0, g_dwOverlayW, g_dwOverlayH}, DestRect;

        int nTitleHeight  = GetSystemMetrics(SM_CYCAPTION);
        int nBorderWidth  = GetSystemMetrics(SM_CXSIZEFRAME);
        int nBorderHeight = GetSystemMetrics(SM_CYSIZEFRAME);

        GetWindowRect(g_hEmuWindow, &DestRect);

        DestRect.left   += nBorderWidth;
        DestRect.right  -= nBorderWidth;
        DestRect.top    += nTitleHeight + nBorderHeight;
        DestRect.bottom -= nBorderHeight;

        HRESULT hRet = g_pDDSOverlay7->UpdateOverlay(&SourRect, g_pDDSPrimary, &DestRect, DDOVER_SHOW, 0);
    }
    else
    {
        // TODO: dont assume X8R8G8B8 ?
        D3DLOCKED_RECT LockedRectDest;

        IDirect3DSurface8 *pBackBuffer=0;

        HRESULT hRet = g_pD3DDevice8->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);

        // if we obtained the backbuffer, manually translate the YUY2 into the backbuffer format
        if(hRet == D3D_OK && pBackBuffer->LockRect(&LockedRectDest, NULL, NULL) == D3D_OK)
        {
            uint08 *pCurByte = (uint08*)((uint32)pSurface & 0x7FFFFFFF);

            uint08 *pDest = (uint08*)LockedRectDest.pBits;

            uint32 dx=0, dy=0;

            uint32 dwImageSize = (g_dwOverlayW * g_dwOverlayH) * 2;

            // grayscale
            if(false)
            {
                for(uint32 y=0;y<g_dwOverlayH;y++)
                {
                    uint32 stop = g_dwOverlayW*4;
                    for(uint32 x=0;x<stop;x+=4)
                    {
                        uint08 Y = *pCurByte;

                        pDest[x+0] = Y;
                        pDest[x+1] = Y;
                        pDest[x+2] = Y;
                        pDest[x+3] = 0xFF;

                        pCurByte+=2;
                    }

                    pDest += LockedRectDest.Pitch;
                }
            }
            // full color conversion (YUY2->XRGB)
            else
            {
                for(uint32 v=0;v<dwImageSize;v+=4)
                {
                    float Y[2], U, V;

                    Y[0] = *pCurByte++;
                    U    = *pCurByte++;
                    Y[1] = *pCurByte++;
                    V    = *pCurByte++;

                    int a=0;
                    for(int x=0;x<2;x++)
                    {
                        float R = Y[a] + 1.402f*(V-128);
                        float G = Y[a] - 0.344f*(U-128) - 0.714f*(V-128);
                        float B = Y[a] + 1.772f*(U-128);

                        R = (R < 0) ? 0 : ((R > 255) ? 255 : R);
                        G = (G < 0) ? 0 : ((G > 255) ? 255 : G);
                        B = (B < 0) ? 0 : ((B > 255) ? 255 : B);

                        uint32 i = (dy*LockedRectDest.Pitch+(dx+x)*4);

                        pDest[i+0] = (uint08)B;
                        pDest[i+1] = (uint08)G;
                        pDest[i+2] = (uint08)R;
                        pDest[i+3] = 0xFF;

                        a++;
                    }

                    dx+=2;

                    if((dx % g_dwOverlayW) == 0)
                    {
                        dy++;
                        dx=0;
                    }

                }
            }

            pBackBuffer->UnlockRect();
        }
    }

    EmuSwapFS();   // XBox FS

    return;
}
// ******************************************************************
// * func: EmuIDirect3DDevice8_GetOverlayUpdateStatus
// ******************************************************************
BOOL WINAPI XTL::EmuIDirect3DDevice8_GetOverlayUpdateStatus()
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetOverlayUpdateStatus();\n",
               GetCurrentThreadId());
    }
    #endif

    EmuSwapFS();   // XBox FS

    // TODO: Actually check for update status
    return FALSE;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_BlockUntilVerticalBlank
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_BlockUntilVerticalBlank()
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_BlockUntilVerticalBlank();\n",
               GetCurrentThreadId());
    }
    #endif

    if(g_XBVideo.GetVSync())
        g_pDD7->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, 0);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVerticalBlankCallback
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetVerticalBlankCallback(PVOID pCallback)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetVerticalBlankCallback\n"
               "(\n"
               "   pCallback           : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pCallback);
    }
    #endif

    printf("*Warning* EmuIDirect3DDevice8_SetVerticalBlankCallback is not implemented\n");

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTextureState_TexCoordIndex
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetTextureState_TexCoordIndex
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
VOID WINAPI XTL::EmuIDirect3DDevice8_SetRenderState_NormalizeNormals
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
VOID WINAPI XTL::EmuIDirect3DDevice8_SetRenderState_TextureFactor
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
VOID WINAPI XTL::EmuIDirect3DDevice8_SetRenderState_ZBias
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
VOID WINAPI XTL::EmuIDirect3DDevice8_SetRenderState_EdgeAntiAlias
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

//    printf("*Warning* SetRenderState_EdgeAntiAlias not implemented!\n");

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_FillMode
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetRenderState_FillMode
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
VOID WINAPI XTL::EmuIDirect3DDevice8_SetRenderState_FogColor
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

    g_pD3DDevice8->SetRenderState(D3DRS_FOGCOLOR, Value);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_Dxt1NoiseEnable
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetRenderState_Dxt1NoiseEnable
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
VOID __fastcall XTL::EmuIDirect3DDevice8_SetRenderState_Simple
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
    for(int v=0;v<174;v++)
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
            case D3DRS_COLORWRITEENABLE:
            {
                DWORD OrigValue = Value;

                Value = 0;

                if(OrigValue & (1L<<16))
                    Value |= D3DCOLORWRITEENABLE_RED;
                if(OrigValue & (1L<<8))
                    Value |= D3DCOLORWRITEENABLE_GREEN;
                if(OrigValue & (1L<<0))
                    Value |= D3DCOLORWRITEENABLE_BLUE;
                if(OrigValue & (1L<<24))
                    Value |= D3DCOLORWRITEENABLE_ALPHA;
            }
            break;

            case D3DRS_SHADEMODE:
                Value = Value & 0x03;
                break;

            case D3DRS_BLENDOP:
                Value = EmuXB2PC_D3DBLENDOP(Value);
                break;
            
            case D3DRS_SRCBLEND:
            case D3DRS_DESTBLEND:
                Value = EmuXB2PC_D3DBLEND(Value);
                break;
            
            case D3DRS_ZFUNC:
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
// * func: EmuIDirect3DDevice8_SetRenderState_VertexBlend
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetRenderState_VertexBlend
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
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_VertexBlend\n"
               "(\n"
               "   Value               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Value);
    }
    #endif

    // ******************************************************************
    // * Convert from Xbox D3D to PC D3D enumeration
    // ******************************************************************
    if(Value <= 1)
        Value = Value;
    else if(Value == 3)
        Value = 2;
    else if(Value == 5)
        Value = 3;
    else
        EmuCleanup("Unsupported D3DVERTEXBLENDFLAGS (%d)", Value);

    g_pD3DDevice8->SetRenderState(D3DRS_VERTEXBLEND, Value);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_CullMode
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetRenderState_CullMode
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
VOID WINAPI XTL::EmuIDirect3DDevice8_SetRenderState_ZEnable
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
VOID WINAPI XTL::EmuIDirect3DDevice8_SetRenderState_StencilEnable
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
VOID WINAPI XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias
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
// * func: EmuIDirect3DDevice8_SetRenderState_ShadowFunc
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetRenderState_ShadowFunc
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
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_ShadowFunc\n"
               "(\n"
               "   Value               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Value);
    }
    #endif

    printf("*Warning* ShadowFunc not implemented\n");

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_YuvEnable
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetRenderState_YuvEnable
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
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_YuvEnable\n"
               "(\n"
               "   Value               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Value);
    }
    #endif

    printf("*Warning* YuvEnable not implemented\n");

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTransform
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetTransform
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
VOID WINAPI XTL::EmuIDirect3DDevice8_GetTransform
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
VOID WINAPI XTL::EmuIDirect3DVertexBuffer8_Lock
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
BYTE* WINAPI XTL::EmuIDirect3DVertexBuffer8_Lock2
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
HRESULT WINAPI XTL::EmuIDirect3DDevice8_SetStreamSource
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

    IDirect3DVertexBuffer8 *pVertexBuffer8 = NULL;

    if(pStreamData != NULL)
    {
        EmuVerifyResourceIsRegistered(pStreamData);

        pVertexBuffer8 = pStreamData->EmuVertexBuffer8;
        pVertexBuffer8->Unlock();
    }

    HRESULT hRet = g_pD3DDevice8->SetStreamSource(StreamNumber, pVertexBuffer8, Stride);

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexShader
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetVertexShader
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
 
    g_pD3DDevice8->SetVertexShader(Handle);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuUpdateDeferredStates
// ******************************************************************
static void EmuUpdateDeferredStates()
{
    using namespace XTL;

    // Certain D3DRS values need to be checked on each Draw[Indexed]Vertices
    if(EmuD3DDeferredRenderState != 0)
    {
        if(XTL::EmuD3DDeferredRenderState[0] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_FOGENABLE,             XTL::EmuD3DDeferredRenderState[0]);

        if(XTL::EmuD3DDeferredRenderState[1] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_FOGTABLEMODE,          XTL::EmuD3DDeferredRenderState[1]);

        if(XTL::EmuD3DDeferredRenderState[6] != X_D3DRS_UNK)
        {
            ::DWORD dwConv = 0;
            
            dwConv |= (XTL::EmuD3DDeferredRenderState[6] & 0x00000010) ? D3DWRAP_U : 0;
            dwConv |= (XTL::EmuD3DDeferredRenderState[6] & 0x00001000) ? D3DWRAP_V : 0;
            dwConv |= (XTL::EmuD3DDeferredRenderState[6] & 0x00100000) ? D3DWRAP_W : 0;

            g_pD3DDevice8->SetRenderState(D3DRS_WRAP0, dwConv);
        }

        if(XTL::EmuD3DDeferredRenderState[10] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_LIGHTING,              XTL::EmuD3DDeferredRenderState[10]);

        if(XTL::EmuD3DDeferredRenderState[11] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_SPECULARENABLE,        XTL::EmuD3DDeferredRenderState[11]);

        if(XTL::EmuD3DDeferredRenderState[20] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, XTL::EmuD3DDeferredRenderState[20]);

        if(XTL::EmuD3DDeferredRenderState[23] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_AMBIENT,               XTL::EmuD3DDeferredRenderState[23]);

        if(XTL::EmuD3DDeferredRenderState[24] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_POINTSIZE,             XTL::EmuD3DDeferredRenderState[24]);
                                                                       
        if(XTL::EmuD3DDeferredRenderState[25] != X_D3DRS_UNK)        
            g_pD3DDevice8->SetRenderState(D3DRS_POINTSIZE_MIN,         XTL::EmuD3DDeferredRenderState[25]);
                                                                       
        if(XTL::EmuD3DDeferredRenderState[26] != X_D3DRS_UNK)        
            g_pD3DDevice8->SetRenderState(D3DRS_POINTSPRITEENABLE,     XTL::EmuD3DDeferredRenderState[26]);

        if(XTL::EmuD3DDeferredRenderState[27] != X_D3DRS_UNK)        
            g_pD3DDevice8->SetRenderState(D3DRS_POINTSCALEENABLE,      XTL::EmuD3DDeferredRenderState[27]);

        if(XTL::EmuD3DDeferredRenderState[28] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_POINTSCALE_A,          XTL::EmuD3DDeferredRenderState[28]);

        if(XTL::EmuD3DDeferredRenderState[29] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_POINTSCALE_B,          XTL::EmuD3DDeferredRenderState[29]);

        if(XTL::EmuD3DDeferredRenderState[30] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_POINTSCALE_C,          XTL::EmuD3DDeferredRenderState[30]);

        if(XTL::EmuD3DDeferredRenderState[33] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_PATCHSEGMENTS,         XTL::EmuD3DDeferredRenderState[33]);

        /** To check for unhandled RenderStates
        for(int v=0;v<117-82;v++)
        {
            if(XTL::EmuD3DDeferredRenderState[v] != X_D3DRS_UNK)
            {
                if(v != 0  && v != 1  && v != 6  && v != 10 && v != 11 && v != 20 && v != 23
                && v != 24 && v != 25 && v != 26 && v != 27 && v != 28 && v != 29 && v != 30
                && v != 33)
                    printf("*Warning* Unhandled RenderState Change @ %d (%d)\n", v, v + 82);
            }
        }
        //**/
    }

    // Certain D3DTS values need to be checked on each Draw[Indexed]Vertices
    if(EmuD3DDeferredTextureState != 0)
    {
        for(int v=0;v<4;v++)
        {
            ::DWORD *pCur = &EmuD3DDeferredTextureState[v*32];

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

            if(pCur[6] != X_D3DTSS_UNK)
                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_MIPMAPLODBIAS, pCur[6]);

            if(pCur[7] != X_D3DTSS_UNK)
                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_MAXMIPLEVEL, pCur[7]);

            if(pCur[8] != X_D3DTSS_UNK)
                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_MAXANISOTROPY, pCur[8]);

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
                    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 29, 30, 31
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

        // if point sprites are enabled, copy stage 3 over to 0
        if(EmuD3DDeferredRenderState[26] == TRUE)
        {
            // pCur = Texture Stage 3 States
            ::DWORD *pCur = &EmuD3DDeferredTextureState[2*32];

            IDirect3DBaseTexture8 *pTexture;

            // set the point sprites texture 
            g_pD3DDevice8->GetTexture(3, &pTexture); 
            g_pD3DDevice8->SetTexture(0, pTexture); 

            // disable all other stages 
            g_pD3DDevice8->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE); 
            g_pD3DDevice8->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE); 

            // in that case we have to copy over the stage by hand
            for(int v=0;v<30;v++)
            { 
                if(pCur[v] != X_D3DTSS_UNK)
                {
                    ::DWORD dwValue;

                    g_pD3DDevice8->GetTextureStageState(3, (D3DTEXTURESTAGESTATETYPE)v, &dwValue);
                    g_pD3DDevice8->SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE)v, dwValue);
                }
            } 
        }
    }
}

// ******************************************************************
// * func: EmuQuadHackA
// ******************************************************************
uint32 EmuQuadHackA(uint32 PrimitiveCount, XTL::IDirect3DVertexBuffer8 *&pOrigVertexBuffer8, XTL::IDirect3DVertexBuffer8 *&pHackVertexBuffer8, UINT dwOffset, PVOID pVertexStreamZeroData, UINT VertexStreamZeroStride, PVOID *ppNewVertexStreamZeroData)
{
    UINT uiStride = 0;

    // These are the sizes of our part in the vertex buffer
    DWORD dwOriginalSize    = 0;
    DWORD dwNewSize         = 0;

    // These are the sizes with the rest of the buffer
    DWORD dwOriginalSizeWR  = 0; // the size of the original vertex buffer
    DWORD dwNewSizeWR       = 0; // the size of the new buffer!!!

    BYTE *pOrigVertexData = 0;
    BYTE *pHackVertexData = 0;

    if(ppNewVertexStreamZeroData != 0)
        *ppNewVertexStreamZeroData = pVertexStreamZeroData;

    if(pVertexStreamZeroData == 0)
    {
        g_pD3DDevice8->GetStreamSource(0, &pOrigVertexBuffer8, &uiStride);

        // This is a list of sqares/rectangles, so we convert it to a list of triangles
        dwOriginalSize  = PrimitiveCount*uiStride*2;
        dwNewSize       = PrimitiveCount*uiStride*3;

        // Retrieve the original buffer size
        {
            XTL::D3DVERTEXBUFFER_DESC Desc;

            if(FAILED(pOrigVertexBuffer8->GetDesc(&Desc))) 
                EmuCleanup("Could not retrieve buffer size");

            // Here we save the full buffer size
            dwOriginalSizeWR = Desc.Size;

            // So we can now calculate the size of the rest (dwOriginalSizeWR - dwOriginalSize) and
            // add it to our new calculated size of the patched buffer
            dwNewSizeWR = dwNewSize + dwOriginalSizeWR - dwOriginalSize;
        }

        g_pD3DDevice8->CreateVertexBuffer(dwNewSizeWR, 0, 0, XTL::D3DPOOL_MANAGED, &pHackVertexBuffer8);

        if(pOrigVertexBuffer8 != 0)
            pOrigVertexBuffer8->Lock(0, 0, &pOrigVertexData, 0);

        if(pHackVertexBuffer8 != 0)
            pHackVertexBuffer8->Lock(0, 0, &pHackVertexData, 0);
    }
    else
    {
        uiStride = VertexStreamZeroStride;

        // This is a list of sqares/rectangles, so we convert it to a list of triangles
        dwOriginalSize  = PrimitiveCount*uiStride*2;
        dwNewSize       = PrimitiveCount*uiStride*3;

        dwOriginalSizeWR = dwOriginalSize;
        dwNewSizeWR = dwNewSize;

        pHackVertexData = (uint08*)malloc(dwNewSizeWR);
        pOrigVertexData = (uint08*)pVertexStreamZeroData;

        *ppNewVertexStreamZeroData = pHackVertexData;
    }

    DWORD dwVertexShader = NULL;

    g_pD3DDevice8->GetVertexShader(&dwVertexShader);

    // Copy the nonmodified data
    memcpy(pHackVertexData, pOrigVertexData, dwOffset);
    memcpy(&pHackVertexData[dwOffset+dwNewSize], &pOrigVertexData[dwOffset+dwOriginalSize], dwOriginalSizeWR-dwOffset-dwOriginalSize);

    for(DWORD i=0;i<(PrimitiveCount/2);i++)
    {
        memcpy(&pHackVertexData[dwOffset+i*uiStride*6+0*uiStride], &pOrigVertexData[dwOffset+i*uiStride*4+0*uiStride], uiStride);
        memcpy(&pHackVertexData[dwOffset+i*uiStride*6+1*uiStride], &pOrigVertexData[dwOffset+i*uiStride*4+1*uiStride], uiStride);
        memcpy(&pHackVertexData[dwOffset+i*uiStride*6+2*uiStride], &pOrigVertexData[dwOffset+i*uiStride*4+2*uiStride], uiStride);
        memcpy(&pHackVertexData[dwOffset+i*uiStride*6+3*uiStride], &pOrigVertexData[dwOffset+i*uiStride*4+2*uiStride], uiStride);
        memcpy(&pHackVertexData[dwOffset+i*uiStride*6+4*uiStride], &pOrigVertexData[dwOffset+i*uiStride*4+3*uiStride], uiStride);
        memcpy(&pHackVertexData[dwOffset+i*uiStride*6+5*uiStride], &pOrigVertexData[dwOffset+i*uiStride*4+0*uiStride], uiStride);

        if(dwVertexShader & D3DFVF_XYZRHW)
        {
            for(int z=0;z<6;z++)
            {
                if(((FLOAT*)&pHackVertexData[dwOffset+i*uiStride*6+z*uiStride])[2] == 0.0f)
                    ((FLOAT*)&pHackVertexData[dwOffset+i*uiStride*6+z*uiStride])[2] = 1.0f;
                if(((FLOAT*)&pHackVertexData[dwOffset+i*uiStride*6+z*uiStride])[3] == 0.0f)
                    ((FLOAT*)&pHackVertexData[dwOffset+i*uiStride*6+z*uiStride])[3] = 1.0f;
            }
        }
    }

    if(pVertexStreamZeroData == 0)
    {
        pOrigVertexBuffer8->Unlock();
        pHackVertexBuffer8->Unlock();

        g_pD3DDevice8->SetStreamSource(0, pHackVertexBuffer8, uiStride);
    }

    return uiStride;
}

// ******************************************************************
// * func: EmuQuadHackB
// ******************************************************************
VOID EmuQuadHackB(uint32 nStride, XTL::IDirect3DVertexBuffer8 *&pOrigVertexBuffer8, XTL::IDirect3DVertexBuffer8 *&pHackVertexBuffer8)
{
    if(pOrigVertexBuffer8 != 0 && pHackVertexBuffer8 != 0)
        g_pD3DDevice8->SetStreamSource(0, pOrigVertexBuffer8, nStride);

    if(pOrigVertexBuffer8 != 0)
        pOrigVertexBuffer8->Release();

    if(pHackVertexBuffer8 != 0)
        pHackVertexBuffer8->Release();
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_DrawVertices
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_DrawVertices
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
        nStride = EmuQuadHackA(PrimitiveCount, pOrigVertexBuffer8, pHackVertexBuffer8, StartVertex, 0, 0, 0);
    }

    g_pD3DDevice8->DrawPrimitive
    (
        PCPrimitiveType,
        StartVertex,
        PrimitiveCount
    );

    // TODO: use original stride here (duh!)
    if(PrimitiveType == 8)  // Quad List
        EmuQuadHackB(nStride, pOrigVertexBuffer8, pHackVertexBuffer8);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_DrawVerticesUP
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_DrawVerticesUP
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

    PVOID pNewVertexStreamZeroData = pVertexStreamZeroData;

    if(PrimitiveType == 8)  // Quad List
    {
        PrimitiveCount *= 2;
        nStride = EmuQuadHackA(PrimitiveCount, pOrigVertexBuffer8, pHackVertexBuffer8, 0, pVertexStreamZeroData, VertexStreamZeroStride, &pNewVertexStreamZeroData);
    }

    g_pD3DDevice8->DrawPrimitiveUP
    (
        PCPrimitiveType,
        PrimitiveCount,
        pNewVertexStreamZeroData,
        VertexStreamZeroStride
    );

    if(PrimitiveType == 8)  // Quad List
    {
        EmuQuadHackB(nStride, pOrigVertexBuffer8, pHackVertexBuffer8);

        if(pNewVertexStreamZeroData != 0)
            free(pNewVertexStreamZeroData);
    }

    EmuSwapFS();   // XBox FS

    return;
}


// ******************************************************************
// * func: EmuIDirect3DDevice8_DrawIndexedVertices
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_DrawIndexedVertices
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

    EmuUpdateDeferredStates();

    if((DWORD)PrimitiveType == 0x03 || (DWORD)PrimitiveType == 0x08 || (DWORD)PrimitiveType == 0x09 || (DWORD)PrimitiveType == 0x10)
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
        nStride = EmuQuadHackA(PrimitiveCount, pOrigVertexBuffer8, pHackVertexBuffer8, 0, 0, 0, 0);
    }

    g_pD3DDevice8->DrawIndexedPrimitive
    (
        PCPrimitiveType, 0, VertexCount, ((DWORD)pIndexData)/2, PrimitiveCount
    );

    if(PrimitiveType == 8)  // Quad List
        EmuQuadHackB(nStride, pOrigVertexBuffer8, pHackVertexBuffer8);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetLight
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_SetLight
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
HRESULT WINAPI XTL::EmuIDirect3DDevice8_SetMaterial
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
HRESULT WINAPI XTL::EmuIDirect3DDevice8_LightEnable
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

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderTarget
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_SetRenderTarget
(
    X_D3DSurface    *pRenderTarget,
    X_D3DSurface    *pNewZStencil
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderTarget\n"
               "(\n"
               "   pRenderTarget       : 0x%.08X\n"
               "   pNewZStencil        : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pRenderTarget, pNewZStencil);
    }
    #endif

    IDirect3DSurface8 *pPCRenderTarget = 0;
    IDirect3DSurface8 *pPCNewZStencil  = 0;

    if(pRenderTarget != 0)
    {
        EmuVerifyResourceIsRegistered(pRenderTarget);
        pPCRenderTarget = pRenderTarget->EmuSurface8;
    }

    if(pNewZStencil != 0)
    {
        EmuVerifyResourceIsRegistered(pNewZStencil);
        pPCNewZStencil  = pNewZStencil->EmuSurface8;
    }

    HRESULT hRet = g_pD3DDevice8->SetRenderTarget(pPCRenderTarget, pPCNewZStencil);

    EmuSwapFS();   // XBox FS
    
    if(FAILED(hRet))
        EmuWarning("SetRenderTarget Failed! (0x%.08X)", hRet);

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreatePalette
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_CreatePalette
(
    X_D3DPALETTESIZE    Size,
    X_D3DPalette      **ppPalette
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_CreatePalette\n"
               "(\n"
               "   Size                : 0x%.08X\n"
               "   ppPalette           : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Size, ppPalette);
    }
    #endif

    *ppPalette = new X_D3DPalette();

    static int lk[4] =
    {
        256*sizeof(D3DCOLOR),    // D3DPALETTE_256
        128*sizeof(D3DCOLOR),    // D3DPALETTE_128
        64*sizeof(D3DCOLOR),     // D3DPALETTE_64
        32*sizeof(D3DCOLOR)      // D3DPALETTE_32
    };

    (*ppPalette)->Common = 0;
    (*ppPalette)->Lock = 0x8000BEEF; // emulated reference count for palettes
    (*ppPalette)->Data = (DWORD)new uint08[lk[Size]];

    EmuSwapFS();   // XBox FS
    
    return D3D_OK;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetPalette
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_SetPalette
(
    DWORD         Stage,
    X_D3DPalette *pPalette
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetPalette\n"
               "(\n"
               "   Stage               : 0x%.08X\n"
               "   pPalette            : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Stage, pPalette);
    }
    #endif

    EmuWarning("Not setting palette");

    EmuSwapFS();   // XBox FS
    
    return D3D_OK;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetFlickerFilter
// ******************************************************************
void WINAPI XTL::EmuIDirect3DDevice8_SetFlickerFilter
(
    DWORD         Filter
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetFlickerFilter\n"
               "(\n"
               "   Filter              : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Filter);
    }
    #endif

    EmuWarning("Not setting flicker filter");

    EmuSwapFS();   // XBox FS
    
    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetSoftDisplayFilter
// ******************************************************************
void WINAPI XTL::EmuIDirect3DDevice8_SetSoftDisplayFilter
(
    BOOL Enable
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetSoftDisplayFilter\n"
               "(\n"
               "   Enable              : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Enable);
    }
    #endif

    EmuWarning("Not setting soft display filter");

    EmuSwapFS();   // XBox FS
    
    return;
}

// ******************************************************************
// * func: EmuIDirect3DPalette8_Lock
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DPalette8_Lock
(
    X_D3DPalette   *pThis,
    D3DCOLOR      **ppColors,
    DWORD           Flags
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuD3D8 (0x%X): EmuIDirect3DPalette8_Lock\n"
               "(\n"
               "   pThis               : 0x%.08X\n"
               "   ppColors            : 0x%.08X\n"
               "   Flags               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), ppColors, Flags);
    }
    #endif

    *ppColors = (D3DCOLOR*)pThis->Data;

    EmuSwapFS();   // XBox FS
    
    return D3D_OK;
}
