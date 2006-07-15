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
#define _XBOXKRNL_DEFEXTRN_

// prevent name collisions
namespace xboxkrnl
{
    #include <xboxkrnl/xboxkrnl.h>
};

#include "CxbxKrnl.h"
#include "Emu.h"
#include "EmuFS.h"
#include "EmuShared.h"
#include "DbgConsole.h"
#include "ResourceTracker.h"
#include "EmuAlloc.h"
#include "EmuXTL.h"
#include "ResCxbxDll.h"

#include <process.h>
#include <locale.h>

// Global(s)
HWND                                g_hEmuWindow   = NULL; // rendering window
XTL::LPDIRECT3DDEVICE8              g_pD3DDevice8  = NULL; // Direct3D8 Device
XTL::LPDIRECTDRAWSURFACE7           g_pDDSPrimary  = NULL; // DirectDraw7 Primary Surface
XTL::LPDIRECTDRAWSURFACE7           g_pDDSOverlay7 = NULL; // DirectDraw7 Overlay Surface
XTL::LPDIRECTDRAWCLIPPER            g_pDDClipper   = NULL; // DirectDraw7 Clipper
DWORD                               g_CurrentVertexShader = 0;
BOOL                                g_bFakePixelShaderLoaded = FALSE;
BOOL                                g_bIsFauxFullscreen = FALSE;

// Static Function(s)
static BOOL WINAPI                  EmuEnumDisplayDevices(GUID FAR *lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext, HMONITOR hm);
static DWORD WINAPI                 EmuRenderWindow(LPVOID);
static DWORD WINAPI                 EmuCreateDeviceProxy(LPVOID);
static LRESULT WINAPI               EmuMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static DWORD WINAPI                 EmuUpdateTickCount(LPVOID);
static inline void                  EmuVerifyResourceIsRegistered(XTL::X_D3DResource *pResource);
static void                         EmuAdjustPower2(UINT *dwWidth, UINT *dwHeight);

// Static Variable(s)
static GUID                         g_ddguid;               // DirectDraw driver GUID
static HMONITOR                     g_hMonitor      = NULL; // Handle to DirectDraw monitor
static XTL::LPDIRECT3D8             g_pD3D8         = NULL; // Direct3D8
static BOOL                         g_bSupportsYUY2 = FALSE;// Does device support YUY2 overlays?
static XTL::LPDIRECTDRAW7           g_pDD7          = NULL; // DirectDraw7
static DWORD                        g_dwOverlayW    = 640;  // Cached Overlay Width
static DWORD                        g_dwOverlayH    = 480;  // Cached Overlay Height
static DWORD                        g_dwOverlayP    = 640;  // Cached Overlay Pitch
static Xbe::Header                 *g_XbeHeader     = NULL; // XbeHeader
static uint32                       g_XbeHeaderSize = 0;    // XbeHeaderSize
static XTL::D3DCAPS8                g_D3DCaps;              // Direct3D8 Caps
static HBRUSH                       g_hBgBrush      = NULL; // Background Brush
static volatile bool                g_bRenderWindowActive = false;
static XBVideo                      g_XBVideo;
static XTL::D3DVBLANKCALLBACK       g_pVBCallback   = NULL; // Vertical-Blank callback routine

// wireframe toggle
static int                          g_iWireframe    = 0;

// resource caching for _Register
static XTL::X_D3DResource pCache[16] = {0};

// current active index buffer
static XTL::X_D3DIndexBuffer       *g_pIndexBuffer  = NULL; // current active index buffer
static DWORD                        g_dwBaseVertexIndex = 0;// current active index buffer base index

// current active vertex stream
static XTL::X_D3DVertexBuffer      *g_pVertexBuffer = NULL; // current active vertex buffer
static XTL::IDirect3DVertexBuffer8 *g_pDummyBuffer = NULL;  // Dummy buffer, used to set unused stream sources with

// current vertical blank information
static XTL::D3DVBLANKDATA           g_VBData = {0};
static DWORD                        g_VBLastSwap = 0;

// cached Direct3D state variable(s)
static XTL::X_D3DSurface           *g_pCachedRenderTarget = NULL;
static XTL::X_D3DSurface           *g_pCachedZStencilSurface = NULL;
static DWORD                        g_dwVertexShaderUsage = 0;
static DWORD                        g_VertexShaderSlots[136];

static XTL::X_VERTEXSHADERCONSTANTMODE g_VertexShaderConstantMode = X_VSCM_192;

// cached Direct3D tiles
XTL::X_D3DTILE XTL::EmuD3DTileCache[0x08] = {0};

// cached active texture
XTL::X_D3DResource *XTL::EmuD3DActiveTexture[4] = {0,0,0,0};

// information passed to the create device proxy thread
struct EmuD3D8CreateDeviceProxyData
{
    XTL::UINT                        Adapter;
    XTL::D3DDEVTYPE                  DeviceType;
    HWND                             hFocusWindow;
    XTL::DWORD                       BehaviorFlags;
    XTL::X_D3DPRESENT_PARAMETERS    *pPresentationParameters;
    XTL::IDirect3DDevice8          **ppReturnedDeviceInterface;
    volatile bool                    bReady;

    union
    {
        volatile HRESULT  hRet;
        volatile bool     bCreate;   // false : release
    };
}
g_EmuCDPD = {0};

// Direct3D initialization (called before emulation begins)
VOID XTL::EmuD3DInit(Xbe::Header *XbeHeader, uint32 XbeHeaderSize)
{
    g_EmuShared->GetXBVideo(&g_XBVideo);

    if(g_XBVideo.GetFullscreen())
        CxbxKrnl_hEmuParent = NULL;

    // cache XbeHeader and size of XbeHeader
    g_XbeHeader     = XbeHeader;
    g_XbeHeaderSize = XbeHeaderSize;

    // create timing thread
    {
        DWORD dwThreadId;

        HANDLE hThread = CreateThread(NULL, NULL, EmuUpdateTickCount, NULL, NULL, &dwThreadId);

        // we must duplicate this handle in order to retain Suspend/Resume thread rights from a remote thread
        {
            HANDLE hDupHandle = NULL;

            DuplicateHandle(GetCurrentProcess(), hThread, GetCurrentProcess(), &hDupHandle, 0, FALSE, DUPLICATE_SAME_ACCESS);

            CxbxKrnlRegisterThread(hDupHandle);
        }
    }

    // create the create device proxy thread
    {
        DWORD dwThreadId;

        CreateThread(NULL, NULL, EmuCreateDeviceProxy, NULL, NULL, &dwThreadId);
    }

    // create window message processing thread
    {
        DWORD dwThreadId;

        g_bRenderWindowActive = false;

        CreateThread(NULL, NULL, EmuRenderWindow, NULL, NULL, &dwThreadId);

        while(!g_bRenderWindowActive)
            Sleep(10);

        Sleep(50);
    }

    // create Direct3D8 and retrieve caps
    {
        using namespace XTL;

        // xbox Direct3DCreate8 returns "1" always, so we need our own ptr
        g_pD3D8 = Direct3DCreate8(D3D_SDK_VERSION);

        if(g_pD3D8 == NULL)
            CxbxKrnlCleanup("Could not initialize Direct3D8!");

        D3DDEVTYPE DevType = (g_XBVideo.GetDirect3DDevice() == 0) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF;

        g_pD3D8->GetDeviceCaps(g_XBVideo.GetDisplayAdapter(), DevType, &g_D3DCaps);
    }

	SetFocus(g_hEmuWindow);

    // create default device
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

// cleanup Direct3D
VOID XTL::EmuD3DCleanup()
{
    XTL::EmuDInputCleanup();

    return;
}

// enumeration procedure for locating display device GUIDs
static BOOL WINAPI EmuEnumDisplayDevices(GUID FAR *lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext, HMONITOR hm)
{
    static DWORD dwEnumCount = 0;

    if(dwEnumCount++ == g_XBVideo.GetDisplayAdapter()+1)
    {
        g_hMonitor = hm;
        dwEnumCount = 0;
        if(lpGUID != 0)
        {
            memcpy(&g_ddguid, lpGUID, sizeof(GUID));
        }
        else
        {
            memset(&g_ddguid, 0, sizeof(GUID));
        }

        return FALSE;
    }

    return TRUE;
}

// window message processing thread
static DWORD WINAPI EmuRenderWindow(LPVOID lpVoid)
{
    char AsciiTitle[50];

    // register window class
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

    // retrieve Xbe title (if possible)
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

    // create the window
    {
        DWORD dwStyle = (g_XBVideo.GetFullscreen() || (CxbxKrnl_hEmuParent == 0))? WS_OVERLAPPEDWINDOW : WS_CHILD;

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

        HWND hwndParent = GetDesktopWindow();

        if(!g_XBVideo.GetFullscreen())
        {
            hwndParent = CxbxKrnl_hEmuParent;
        }

        g_hEmuWindow = CreateWindow
        (
            "CxbxRender", AsciiTitle,
            dwStyle, x, y, nWidth, nHeight,
            hwndParent, NULL, GetModuleHandle(NULL), NULL
        );
    }

    ShowWindow(g_hEmuWindow, (g_XBVideo.GetFullscreen() || (CxbxKrnl_hEmuParent == 0) ) ? SW_SHOWDEFAULT : SW_SHOWMAXIMIZED);
    UpdateWindow(g_hEmuWindow);

    if(!g_XBVideo.GetFullscreen() && (CxbxKrnl_hEmuParent != NULL))
    {
        SetFocus(CxbxKrnl_hEmuParent);
    }

    // initialize direct input
    if(!XTL::EmuDInputInit())
        CxbxKrnlCleanup("Could not initialize DirectInput!");

    DbgPrintf("EmuD3D8 (0x%X): Message-Pump thread is running.\n", GetCurrentThreadId());

	SetFocus(g_hEmuWindow);

    DbgConsole *dbgConsole = new DbgConsole();

    // message processing loop
    {
        MSG msg;

        ZeroMemory(&msg, sizeof(msg));

        bool lPrintfOn = g_bPrintfOn;

        while(msg.message != WM_QUIT)
        {
            if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
            {
                g_bRenderWindowActive = true;

                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                Sleep(10);

                // if we've just switched back to display off, clear buffer & display prompt
                if(!g_bPrintfOn && lPrintfOn)
                {
                    dbgConsole->Reset();
                }

                lPrintfOn = g_bPrintfOn;

                dbgConsole->Process();
            }
        }

        g_bRenderWindowActive = false;

        delete dbgConsole;

        CxbxKrnlCleanup(NULL);
    }

    return 0;
}

// simple helper function
void ToggleFauxFullscreen(HWND hWnd)
{
    if(g_XBVideo.GetFullscreen())
        return;

    static LONG lRestore = 0, lRestoreEx = 0;
    static RECT lRect = {0};

    if(!g_bIsFauxFullscreen)
    {
        if(CxbxKrnl_hEmuParent != NULL)
        {
            SetParent(hWnd, NULL);
        }
        else
        {
            lRestore = GetWindowLong(hWnd, GWL_STYLE);
            lRestoreEx = GetWindowLong(hWnd, GWL_EXSTYLE);

            GetWindowRect(hWnd, &lRect);
        }

        SetWindowLong(hWnd, GWL_STYLE, WS_POPUP);
        ShowWindow(hWnd, SW_MAXIMIZE);
        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    }
    else
    {
        if(CxbxKrnl_hEmuParent != NULL)
        {
            SetParent(hWnd, CxbxKrnl_hEmuParent);
            SetWindowLong(hWnd, GWL_STYLE, WS_CHILD);
            ShowWindow(hWnd, SW_MAXIMIZE);
            SetFocus(CxbxKrnl_hEmuParent);
        }
        else
        {
            SetWindowLong(hWnd, GWL_STYLE, lRestore);
            SetWindowLong(hWnd, GWL_EXSTYLE, lRestoreEx);
            ShowWindow(hWnd, SW_RESTORE);
            SetWindowPos(hWnd, HWND_NOTOPMOST, lRect.left, lRect.top, lRect.right - lRect.left, lRect.bottom - lRect.top, 0);
            SetFocus(hWnd);
        }
    }

    g_bIsFauxFullscreen = !g_bIsFauxFullscreen;
}

// rendering window message procedure
static LRESULT WINAPI EmuMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static bool bAutoPaused = false;

    switch(msg)
    {
        case WM_DESTROY:
        {
            DeleteObject(g_hBgBrush);
            PostQuitMessage(0);
            return 0;
        }
        break;

        case WM_SYSKEYDOWN:
        {
            if(wParam == VK_RETURN)
            {
                ToggleFauxFullscreen(hWnd);
            }
            else if(wParam == VK_F4)
            {
                PostMessage(hWnd, WM_CLOSE, 0, 0);
            }
        }
        break;

        case WM_KEYDOWN:
        {
            /*! disable fullscreen if we are set to faux mode, and faux fullscreen is active */
            if(wParam == VK_ESCAPE)
            {
                if(g_XBVideo.GetFullscreen())
                {
                    SendMessage(hWnd, WM_CLOSE, 0, 0);
                }
                else if(g_bIsFauxFullscreen)
                {
                    ToggleFauxFullscreen(hWnd);
                }
            }
            else if(wParam == VK_F8)
            {
                g_bPrintfOn = !g_bPrintfOn;
            }
            else if(wParam == VK_F9)
            {
                XTL::g_bBrkPush = TRUE;
            }
            else if(wParam == VK_F10)
            {
                ToggleFauxFullscreen(hWnd);
            }
            else if(wParam == VK_F11)
            {
                if(g_iWireframe++ == 2)
                    g_iWireframe = 0;
            }
            else if(wParam == VK_F12)
            {
                XTL::g_bStepPush = !XTL::g_bStepPush;
            }
        }
        break;

        case WM_SIZE:
        {
            switch(wParam)
            {
                case SIZE_RESTORED:
                case SIZE_MAXIMIZED:
                {
                    if(bAutoPaused)
                    {
                        bAutoPaused = false;
                        CxbxKrnlResume();
                    }
                }
                break;

                case SIZE_MINIMIZED:
                {
                    if(g_XBVideo.GetFullscreen())
                        CxbxKrnlCleanup(NULL);

                    if(!g_bEmuSuspended)
                    {
                        bAutoPaused = true;
                        CxbxKrnlSuspend();
                    }
                }
                break;
            }
        }
        break;

        case WM_CLOSE:
            DestroyWindow(hWnd);
            break;

        case WM_SETFOCUS:
        {
            if(CxbxKrnl_hEmuParent != NULL)
            {
                SetFocus(CxbxKrnl_hEmuParent);
            }
        }
        break;

        case WM_SETCURSOR:
        {
            if(g_XBVideo.GetFullscreen() || g_bIsFauxFullscreen)
            {
                SetCursor(NULL);
                return 0;
            }

            return DefWindowProc(hWnd, msg, wParam, lParam);
        }
        break;

        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return 0;
}

// timing thread procedure
static DWORD WINAPI EmuUpdateTickCount(LPVOID)
{
    // since callbacks come from here
    EmuGenerateFS(CxbxKrnl_TLS, CxbxKrnl_TLSData);

    DbgPrintf("EmuD3D8 (0x%X): Timing thread is running.\n", GetCurrentThreadId());

    timeBeginPeriod(0);

    // current vertical blank count
    int curvb = 0;

    while(true)
    {
        xboxkrnl::KeTickCount = timeGetTime();
        Sleep(1);

        //
        // Poll input
        //

        {
            int v;

            for(v=0;v<XINPUT_SETSTATE_SLOTS;v++)
            {
                HANDLE hDevice = g_pXInputSetStateStatus[v].hDevice;

                if(hDevice == 0)
                    continue;
                
                DWORD dwLatency = g_pXInputSetStateStatus[v].dwLatency++;

                if(dwLatency < XINPUT_SETSTATE_LATENCY)
                    continue;

                g_pXInputSetStateStatus[v].dwLatency = 0;

                XTL::PXINPUT_FEEDBACK pFeedback = (XTL::PXINPUT_FEEDBACK)g_pXInputSetStateStatus[v].pFeedback;

                if(pFeedback == 0)
                    continue;

                //
                // Only update slot if it has not already been updated
                //

                if(pFeedback->Header.dwStatus != ERROR_SUCCESS)
                {
                    if(pFeedback->Header.hEvent != 0)
                    {
                        SetEvent(pFeedback->Header.hEvent);
                    }

                    pFeedback->Header.dwStatus = ERROR_SUCCESS;
                }
            }
        }

        // trigger vblank callback
        {
            g_VBData.VBlank++;

            if(g_pVBCallback != NULL)
            {
                EmuSwapFS();    // Xbox FS
                g_pVBCallback(&g_VBData);
                EmuSwapFS();    // Win2k/XP FS
            }

            g_VBData.Swap = 0;
        }
    }

    timeEndPeriod(0);
}

// thread dedicated to create devices
static DWORD WINAPI EmuCreateDeviceProxy(LPVOID)
{
    DbgPrintf("EmuD3D8 (0x%X): CreateDevice proxy thread is running.\n", GetCurrentThreadId());

    while(true)
    {
        // if we have been signalled, create the device with cached parameters
        if(g_EmuCDPD.bReady)
        {
            DbgPrintf("EmuD3D8 (0x%X): CreateDevice proxy thread recieved request.\n", GetCurrentThreadId());

            if(g_EmuCDPD.bCreate)
            {
                // only one device should be created at once
                // TODO: ensure all surfaces are somehow cleaned up?
                if(g_pD3DDevice8 != 0)
                {
                    DbgPrintf("EmuD3D8 (0x%X): CreateDevice proxy thread releasing old Device.\n", GetCurrentThreadId());

                    g_pD3DDevice8->EndScene();

                    while(g_pD3DDevice8->Release() != 0);

                    g_pD3DDevice8 = 0;
                }

                if(g_EmuCDPD.pPresentationParameters->BufferSurfaces[0] != NULL)
                    EmuWarning("BufferSurfaces[0] : 0x%.08X", g_EmuCDPD.pPresentationParameters->BufferSurfaces[0]);

                if(g_EmuCDPD.pPresentationParameters->DepthStencilSurface != NULL)
                    EmuWarning("DepthStencilSurface : 0x%.08X", g_EmuCDPD.pPresentationParameters->DepthStencilSurface);

                // make adjustments to parameters to make sense with windows Direct3D
                {
                    g_EmuCDPD.DeviceType =(g_XBVideo.GetDirect3DDevice() == 0) ? XTL::D3DDEVTYPE_HAL : XTL::D3DDEVTYPE_REF;
                    g_EmuCDPD.Adapter    = g_XBVideo.GetDisplayAdapter();

                    g_EmuCDPD.pPresentationParameters->Windowed = !g_XBVideo.GetFullscreen();

                    if(g_XBVideo.GetVSync())
                        g_EmuCDPD.pPresentationParameters->SwapEffect = XTL::D3DSWAPEFFECT_COPY_VSYNC;

                    g_EmuCDPD.hFocusWindow = g_hEmuWindow;

                    g_EmuCDPD.pPresentationParameters->BackBufferFormat       = XTL::EmuXB2PC_D3DFormat(g_EmuCDPD.pPresentationParameters->BackBufferFormat);
                    g_EmuCDPD.pPresentationParameters->AutoDepthStencilFormat = XTL::EmuXB2PC_D3DFormat(g_EmuCDPD.pPresentationParameters->AutoDepthStencilFormat);

                    if(!g_XBVideo.GetVSync() && (g_D3DCaps.PresentationIntervals & D3DPRESENT_INTERVAL_IMMEDIATE) && g_XBVideo.GetFullscreen())
                        g_EmuCDPD.pPresentationParameters->FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
                    else
                    {
                        if(g_D3DCaps.PresentationIntervals & D3DPRESENT_INTERVAL_ONE && g_XBVideo.GetFullscreen())
                            g_EmuCDPD.pPresentationParameters->FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
                        else
                            g_EmuCDPD.pPresentationParameters->FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
                    }

                    // TODO: Support Xbox extensions if possible
                    if(g_EmuCDPD.pPresentationParameters->MultiSampleType != 0)
                    {
                        EmuWarning("MultiSampleType 0x%.08X is not supported!", g_EmuCDPD.pPresentationParameters->MultiSampleType);

                        g_EmuCDPD.pPresentationParameters->MultiSampleType = XTL::D3DMULTISAMPLE_NONE;

                        // TODO: Check card for multisampling abilities
            //            if(pPresentationParameters->MultiSampleType == 0x00001121)
            //                pPresentationParameters->MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
            //            else
            //                CxbxKrnlCleanup("Unknown MultiSampleType (0x%.08X)", pPresentationParameters->MultiSampleType);
                    }

                    g_EmuCDPD.pPresentationParameters->Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
        
                    // retrieve resolution from configuration
                    if(g_EmuCDPD.pPresentationParameters->Windowed)
                    {
                        sscanf(g_XBVideo.GetVideoResolution(), "%d x %d", &g_EmuCDPD.pPresentationParameters->BackBufferWidth, &g_EmuCDPD.pPresentationParameters->BackBufferHeight);

                        XTL::D3DDISPLAYMODE D3DDisplayMode;

                        g_pD3D8->GetAdapterDisplayMode(g_XBVideo.GetDisplayAdapter(), &D3DDisplayMode);

                        g_EmuCDPD.pPresentationParameters->BackBufferFormat = D3DDisplayMode.Format;
                        g_EmuCDPD.pPresentationParameters->FullScreen_RefreshRateInHz = 0;
                    }
                    else
                    {
                        char szBackBufferFormat[16];

                        sscanf(g_XBVideo.GetVideoResolution(), "%d x %d %*dbit %s (%d hz)", 
                            &g_EmuCDPD.pPresentationParameters->BackBufferWidth, 
                            &g_EmuCDPD.pPresentationParameters->BackBufferHeight,
                            szBackBufferFormat,
                            &g_EmuCDPD.pPresentationParameters->FullScreen_RefreshRateInHz);

                        if(strcmp(szBackBufferFormat, "x1r5g5b5") == 0)
                            g_EmuCDPD.pPresentationParameters->BackBufferFormat = XTL::D3DFMT_X1R5G5B5;
                        else if(strcmp(szBackBufferFormat, "r5g6r5") == 0)
                            g_EmuCDPD.pPresentationParameters->BackBufferFormat = XTL::D3DFMT_R5G6B5;
                        else if(strcmp(szBackBufferFormat, "x8r8g8b8") == 0)
                            g_EmuCDPD.pPresentationParameters->BackBufferFormat = XTL::D3DFMT_X8R8G8B8;
                        else if(strcmp(szBackBufferFormat, "a8r8g8b8") == 0)
                            g_EmuCDPD.pPresentationParameters->BackBufferFormat = XTL::D3DFMT_A8R8G8B8;
                    }
                }

                // detect vertex processing capabilities
                if((g_D3DCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) && g_EmuCDPD.DeviceType == XTL::D3DDEVTYPE_HAL)
                {
                    DbgPrintf("EmuD3D8 (0x%X): Using hardware vertex processing\n", GetCurrentThreadId());

                    g_EmuCDPD.BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
                    g_dwVertexShaderUsage = 0;
                }
                else
                {
                    DbgPrintf("EmuD3D8 (0x%X): Using software vertex processing\n", GetCurrentThreadId());

                    g_EmuCDPD.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
                    g_dwVertexShaderUsage = D3DUSAGE_SOFTWAREPROCESSING;
                }

                // redirect to windows Direct3D
                g_EmuCDPD.hRet = g_pD3D8->CreateDevice
                (
                    g_EmuCDPD.Adapter,
                    g_EmuCDPD.DeviceType,
                    g_EmuCDPD.hFocusWindow,
                    g_EmuCDPD.BehaviorFlags,
                    (XTL::D3DPRESENT_PARAMETERS*)g_EmuCDPD.pPresentationParameters,
                    g_EmuCDPD.ppReturnedDeviceInterface
                );

                // report error
                if(FAILED(g_EmuCDPD.hRet))
                {
                    if(g_EmuCDPD.hRet == D3DERR_INVALIDCALL)
                        CxbxKrnlCleanup("IDirect3D8::CreateDevice failed (Invalid Call)");
                    else if(g_EmuCDPD.hRet == D3DERR_NOTAVAILABLE)
                        CxbxKrnlCleanup("IDirect3D8::CreateDevice failed (Not Available)");
                    else if(g_EmuCDPD.hRet == D3DERR_OUTOFVIDEOMEMORY)
                        CxbxKrnlCleanup("IDirect3D8::CreateDevice failed (Out of Video Memory)");

                    CxbxKrnlCleanup("IDirect3D8::CreateDevice failed (Unknown)");
                }

                // cache device pointer
                g_pD3DDevice8 = *g_EmuCDPD.ppReturnedDeviceInterface;

                // default NULL guid
                ZeroMemory(&g_ddguid, sizeof(GUID));

                // enumerate device guid for this monitor, for directdraw
                HRESULT hRet = XTL::DirectDrawEnumerateExA(EmuEnumDisplayDevices, NULL, DDENUM_ATTACHEDSECONDARYDEVICES);

                // create DirectDraw7
                {
                    if(FAILED(hRet))
                        hRet = XTL::DirectDrawCreateEx(NULL, (void**)&g_pDD7, XTL::IID_IDirectDraw7, NULL);
                    else
                        hRet = XTL::DirectDrawCreateEx(&g_ddguid, (void**)&g_pDD7, XTL::IID_IDirectDraw7, NULL);

                    if(FAILED(hRet))
                        CxbxKrnlCleanup("Could not initialize DirectDraw7");

                    hRet = g_pDD7->SetCooperativeLevel(0, DDSCL_NORMAL);

                    if(FAILED(hRet))
                        CxbxKrnlCleanup("Could not set cooperative level");
                }

                // check for YUY2 overlay support TODO: accept other overlay types
                {
                    DWORD  dwCodes = 0;
                    DWORD *lpCodes = 0;

                    g_pDD7->GetFourCCCodes(&dwCodes, lpCodes);

                    lpCodes = (DWORD*)CxbxMalloc(dwCodes*sizeof(DWORD));

                    g_pDD7->GetFourCCCodes(&dwCodes, lpCodes);

                    g_bSupportsYUY2 = false;
                    for(DWORD v=0;v<dwCodes;v++)
                    {
                        if(lpCodes[v] == MAKEFOURCC('Y','U','Y','2'))
                        {
                            g_bSupportsYUY2 = true;
                            break;
                        }
                    }

                    CxbxFree(lpCodes);

                    if(!g_bSupportsYUY2)
                        EmuWarning("YUY2 overlays are not supported in hardware, could be slow!");
                }

                // initialize primary surface
                if(g_bSupportsYUY2)
                {
                    XTL::DDSURFACEDESC2 ddsd2;

                    ZeroMemory(&ddsd2, sizeof(ddsd2));

                    ddsd2.dwSize = sizeof(ddsd2);
                    ddsd2.dwFlags = DDSD_CAPS;
                    ddsd2.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
                        
                    HRESULT hRet = g_pDD7->CreateSurface(&ddsd2, &g_pDDSPrimary, 0);

                    if(FAILED(hRet))
                        CxbxKrnlCleanup("Could not create primary surface (0x%.08X)", hRet);
                }

                // update render target cache
                g_pCachedRenderTarget = new XTL::X_D3DSurface();
                g_pCachedRenderTarget->Common = 0;
                g_pCachedRenderTarget->Data = X_D3DRESOURCE_DATA_FLAG_SPECIAL | X_D3DRESOURCE_DATA_FLAG_D3DREND;
                g_pD3DDevice8->GetRenderTarget(&g_pCachedRenderTarget->EmuSurface8);

                // update z-stencil surface cache
                g_pCachedZStencilSurface = new XTL::X_D3DSurface();
                g_pCachedZStencilSurface->Common = 0;
                g_pCachedZStencilSurface->Data = X_D3DRESOURCE_DATA_FLAG_SPECIAL | X_D3DRESOURCE_DATA_FLAG_D3DSTEN;
                g_pD3DDevice8->GetDepthStencilSurface(&g_pCachedZStencilSurface->EmuSurface8);


                (void)g_pD3DDevice8->CreateVertexBuffer
                (
                    1, 0, 0, XTL::D3DPOOL_MANAGED,
                    &g_pDummyBuffer
                );

                for(int Streams = 0; Streams < 8; Streams++)
                {
                    g_pD3DDevice8->SetStreamSource(Streams, g_pDummyBuffer, 1);
                }

                // begin scene
                g_pD3DDevice8->BeginScene();

                // initially, show a black screen
                g_pD3DDevice8->Clear(0, 0, D3DCLEAR_TARGET, 0xFF000000, 0, 0);
                g_pD3DDevice8->Present(0, 0, 0, 0);

                // signal completion
                g_EmuCDPD.bReady = false;
            }
            else
            {
                // release direct3d
                if(g_pD3DDevice8 != 0)
                {
                    DbgPrintf("EmuD3D8 (0x%X): CreateDevice proxy thread releasing old Device.\n", GetCurrentThreadId());

                    g_pD3DDevice8->EndScene();

                    g_EmuCDPD.hRet = g_pD3DDevice8->Release();
                    
                    if(g_EmuCDPD.hRet == 0)
                        g_pD3DDevice8 = 0;
                }

                if(g_bSupportsYUY2)
                {
                    // cleanup directdraw surface
                    if(g_pDDSPrimary != 0)
                    {
                        g_pDDSPrimary->Release();
                        g_pDDSPrimary = 0;
                    }
                }

                // cleanup directdraw
                if(g_pDD7 != 0)
                {
                    g_pDD7->Release();
                    g_pDD7 = 0;
                }

                // signal completion
                g_EmuCDPD.bReady = false;
            }
        }

        Sleep(1);
    }

    return 0;
}

// check if a resource has been registered yet (if not, register it)
static void EmuVerifyResourceIsRegistered(XTL::X_D3DResource *pResource)
{
    // 0xEEEEEEEE and 0xFFFFFFFF are somehow set in Halo :(
    if(pResource->Lock != 0 && pResource->Lock != 0xEEEEEEEE && pResource->Lock != 0xFFFFFFFF)
        return;

    // Already "Registered" implicitly
    if((IsSpecialResource(pResource->Data) && ((pResource->Data & X_D3DRESOURCE_DATA_FLAG_D3DREND) || (pResource->Data & X_D3DRESOURCE_DATA_FLAG_D3DSTEN)))
     ||(pResource->Data == 0xB00BBABE))
        return;

    int v=0;

    for(v=0;v<16;v++)
    {
        if(pCache[v].Data == pResource->Data && pResource->Data != 0)
        {
            pResource->EmuResource8 = pCache[v].EmuResource8;
            return;
        }
    }

    EmuSwapFS();    // XBox FS;
    XTL::EmuIDirect3DResource8_Register(pResource, 0/*(PVOID)pResource->Data*/);
    EmuSwapFS();    // Win2k/XP FS

    if(pResource->Lock != X_D3DRESOURCE_LOCK_FLAG_NOSIZE)
    {
        for(v=0;v<16;v++)
        {
            if(pCache[v].Data == 0)
            {
                pCache[v].Data = pResource->Data;
                pCache[v].EmuResource8 = pResource->EmuResource8;
                break;
            }

            if(v == 16)
                CxbxKrnlCleanup("X_D3DResource cache is maxed out!");
        }
    }
}

// ensure a given width/height are powers of 2
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
        EmuWarning("Needed to resize width (%d->%d)", *dwWidth, NewWidth);
    }

    if(*dwHeight != NewHeight)
    {
        NewHeight <<= 1;
        EmuWarning("Needed to resize height (%d->%d)", *dwHeight, NewHeight);
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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3D8_CreateDevice\n"
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

    // Cache parameters
    g_EmuCDPD.Adapter = Adapter;
    g_EmuCDPD.DeviceType = DeviceType;
    g_EmuCDPD.hFocusWindow = hFocusWindow;
    g_EmuCDPD.pPresentationParameters = pPresentationParameters;
    g_EmuCDPD.ppReturnedDeviceInterface = ppReturnedDeviceInterface;

    // Wait until proxy is done with an existing call (i highly doubt this situation will come up)
    while(g_EmuCDPD.bReady)
        Sleep(10);

    // Signal proxy thread, and wait for completion
    g_EmuCDPD.bReady = true;
    g_EmuCDPD.bCreate = true;

    // Wait until proxy is completed
    while(g_EmuCDPD.bReady)
        Sleep(10);

    EmuSwapFS();   // XBox FS

    return g_EmuCDPD.hRet;
}

// ******************************************************************
// * func: EmuIDirect3DResource8_IsBusy
// ******************************************************************
BOOL WINAPI XTL::EmuIDirect3DDevice8_IsBusy()
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_IsBusy();\n",
           GetCurrentThreadId());

    EmuWarning("EmuIDirect3DDevice8_IsBusy ignored!");

    EmuSwapFS();   // XBox FS

    return FALSE;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetCreationParameters
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetCreationParameters\n"
           "(\n"
           "   pParameters               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pParameters);

    pParameters->AdapterOrdinal = D3DADAPTER_DEFAULT;
    pParameters->DeviceType = D3DDEVTYPE_HAL;
    pParameters->hFocusWindow = NULL;
    pParameters->BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3D8_CheckDeviceFormat
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3D8_CheckDeviceFormat
(
    UINT                        Adapter,
    D3DDEVTYPE                  DeviceType,
    D3DFORMAT                   AdapterFormat,
    DWORD                       Usage,
    X_D3DRESOURCETYPE           RType,
    X_D3DFORMAT                 CheckFormat
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3D8_CheckDeviceFormat\n"
           "(\n"
           "   Adapter                   : 0x%.08X\n"
           "   DeviceType                : 0x%.08X\n"
           "   AdapterFormat             : 0x%.08X\n"
           "   Usage                     : 0x%.08X\n"
           "   RType                     : 0x%.08X\n"
           "   CheckFormat               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Adapter, DeviceType, AdapterFormat,
           Usage, RType, CheckFormat);

    if(RType > 7)
        CxbxKrnlCleanup("RType > 7");

    HRESULT hRet = g_pD3D8->CheckDeviceFormat
    (
        g_XBVideo.GetDisplayAdapter(), (g_XBVideo.GetDirect3DDevice() == 0) ? XTL::D3DDEVTYPE_HAL : XTL::D3DDEVTYPE_REF,
        EmuXB2PC_D3DFormat(AdapterFormat), Usage, (D3DRESOURCETYPE)RType, EmuXB2PC_D3DFormat(CheckFormat)
    );

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetDisplayFieldStatus
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_GetDisplayFieldStatus(X_D3DFIELD_STATUS *pFieldStatus)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetDisplayFieldStatus\n"
           "(\n"
           "   pFieldStatus              : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pFieldStatus);

    pFieldStatus->Field = (g_VBData.VBlank%2 == 0) ? X_D3DFIELD_ODD : X_D3DFIELD_EVEN;
    pFieldStatus->VBlankCount = g_VBData.VBlank;

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_BeginPush
// ******************************************************************
PDWORD WINAPI XTL::EmuIDirect3DDevice8_BeginPush(DWORD Count)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_BeginPush(%d);\n", GetCurrentThreadId(), Count);

    DWORD *pRet = new DWORD[Count];

    g_dwPrimaryPBCount = Count;
    g_pPrimaryPB = pRet;

    EmuSwapFS();   // XBox FS

    return pRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_EndPush
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_EndPush(DWORD *pPush)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_EndPush(0x%.08X);\n", GetCurrentThreadId(), pPush);

    EmuExecutePushBufferRaw(g_pPrimaryPB);

    delete[] g_pPrimaryPB;

    g_pPrimaryPB = 0;

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_BeginVisibilityTest
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_BeginVisibilityTest()
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_BeginVisibilityTest();\n", GetCurrentThreadId());

    EmuSwapFS();   // XBox FS

    return D3D_OK;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_EndVisibilityTest
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_EndVisibilityTest
(
    DWORD                       Index
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_EndVisibilityTest\n"
           "(\n"
           "   Index                     : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Index);

    EmuSwapFS();   // XBox FS

    return D3D_OK;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetBackBufferScale
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetBackBufferScale(FLOAT x, FLOAT y)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetBackBufferScale\n"
           "(\n"
           "   x                         : %f\n"
           "   y                         : %f\n"
           ");\n",
           GetCurrentThreadId(), x, y);

    EmuWarning("SetBackBufferScale ignored");

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetVisibilityTestResult
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_GetVisibilityTestResult
(
    DWORD                       Index,
    UINT                       *pResult,
    ULONGLONG                  *pTimeStamp
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetVisibilityTestResult\n"
           "(\n"
           "   Index                     : 0x%.08X\n"
           "   pResult                   : 0x%.08X\n"
           "   pTimeStamp                : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Index, pResult, pTimeStamp);

    // TODO: actually emulate this!?

    if(pResult != 0)
        *pResult = 640*480;

    if(pTimeStamp != 0)
        *pTimeStamp = 0;

    EmuSwapFS();   // XBox FS

    return D3D_OK;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetDeviceCaps
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_GetDeviceCaps
(
    D3DCAPS8                   *pCaps
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetDeviceCaps\n"
           "(\n"
           "   pCaps                     : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pCaps);

    g_pD3D8->GetDeviceCaps(g_XBVideo.GetDisplayAdapter(), (g_XBVideo.GetDirect3DDevice() == 0) ? XTL::D3DDEVTYPE_HAL : XTL::D3DDEVTYPE_REF, pCaps);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_LoadVertexShader
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_LoadVertexShader
(
    DWORD                       Handle,
    DWORD                       Address
)
{
    EmuSwapFS();   // Win2k/XP FS

    // debug trace
    DbgPrintf( "EmuD3D8 (0x%X): EmuIDirect3DDevice8_LoadVertexShader\n"
               "(\n"
               "   Handle              : 0x%.08X\n"
               "   Address             : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Handle,Address);

    if(Address < 136 && VshHandleIsVertexShader(Handle)) 
    {
        VERTEX_SHADER *pVertexShader = (VERTEX_SHADER *)(VshHandleGetVertexShader(Handle))->Handle;
        for (DWORD i = Address; i < pVertexShader->Size; i++)
        {
            // TODO: This seems very fishy
            g_VertexShaderSlots[i] = Handle;
        }
    }

    EmuSwapFS();   // Xbox FS

    return D3D_OK;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SelectVertexShader
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_SelectVertexShader
(
    DWORD                       Handle,
    DWORD                       Address
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SelectVertexShader\n"
           "(\n"
           "   Handle              : 0x%.08X\n"
           "   Address             : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Handle, Address);

    if(VshHandleIsVertexShader(Handle))
    {
        VERTEX_SHADER *pVertexShader = (VERTEX_SHADER *)(((X_D3DVertexShader *)(Handle & 0x7FFFFFFF))->Handle);
        g_pD3DDevice8->SetVertexShader(pVertexShader->Handle);
    }
    else if(Handle == NULL)
    {
        g_pD3DDevice8->SetVertexShader(D3DFVF_XYZ | D3DFVF_TEX0);
    }
    else if(Address < 136)
    {
        X_D3DVertexShader *pVertexShader = (X_D3DVertexShader*)g_VertexShaderSlots[Address];

        if(pVertexShader != NULL)
        {
            g_pD3DDevice8->SetVertexShader(((VERTEX_SHADER *)((X_D3DVertexShader *)g_VertexShaderSlots[Address])->Handle)->Handle);
        }
        else
        {
            EmuWarning("g_VertexShaderSlots[%d] = 0", Address);
        }
    }

    EmuSwapFS();   // XBox FS

    return D3D_OK;
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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3D8_GetAdapterModeCount\n"
           "(\n"
           "   Adapter                   : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Adapter);

    UINT ret = g_pD3D8->GetAdapterModeCount(g_XBVideo.GetDisplayAdapter());

    D3DDISPLAYMODE Mode;

    for(uint32 v=0;v<ret;v++)
    {
        HRESULT hRet = g_pD3D8->EnumAdapterModes(g_XBVideo.GetDisplayAdapter(), v, &Mode);

        if(hRet != D3D_OK)
            break;

        if(Mode.Width != 640 || Mode.Height != 480)
            ret--;
    }

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3D8_GetAdapterDisplayMode\n"
           "(\n"
           "   Adapter                   : 0x%.08X\n"
           "   pMode                     : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Adapter, pMode);

    // NOTE: WARNING: We should cache the "Emulated" display mode and return
    // This value. We can initialize the cache with the default Xbox mode data.
    HRESULT hRet = g_pD3D8->GetAdapterDisplayMode
    (
        g_XBVideo.GetDisplayAdapter(),
        (D3DDISPLAYMODE*)pMode
    );

    // make adjustments to the parameters to make sense with windows direct3d
    {
        D3DDISPLAYMODE *pPCMode = (D3DDISPLAYMODE*)pMode;

        // Convert Format (PC->Xbox)
        pMode->Format = EmuPC2XB_D3DFormat(pPCMode->Format);

        // TODO: Make this configurable in the future?
        // D3DPRESENTFLAG_FIELD | D3DPRESENTFLAG_INTERLACED | D3DPRESENTFLAG_LOCKABLE_BACKBUFFER
        pMode->Flags  = 0x000000A1;

        // TODO: Retrieve from current CreateDevice settings?
        pMode->Width = 640;
        pMode->Height = 480;
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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3D8_EnumAdapterModes\n"
           "(\n"
           "   Adapter                   : 0x%.08X\n"
           "   Mode                      : 0x%.08X\n"
           "   pMode                     : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Adapter, Mode, pMode);

    HRESULT hRet;

    static int ModeAdder = 0;

    if(Mode == 0)
        ModeAdder = 0;

    D3DDISPLAYMODE PCMode;

    do
    {
        hRet = g_pD3D8->EnumAdapterModes(g_XBVideo.GetDisplayAdapter(), Mode+ModeAdder, (D3DDISPLAYMODE*)&PCMode);

        if(hRet != D3D_OK || (PCMode.Width == 640 && PCMode.Height == 480))
            break;

        ModeAdder++;
    }
    while(true);

    // make adjustments to parameters to make sense with windows direct3d
    if(hRet == D3D_OK)
    {
        //
        // NOTE: WARNING: PC D3DDISPLAYMODE is different than Xbox D3DDISPLAYMODE!
        //

        // Convert Format (PC->Xbox)
        pMode->Width  = PCMode.Width;
        pMode->Height = PCMode.Height;
        pMode->RefreshRate = PCMode.RefreshRate;

        // TODO: Make this configurable in the future?
        // D3DPRESENTFLAG_FIELD | D3DPRESENTFLAG_INTERLACED | D3DPRESENTFLAG_LOCKABLE_BACKBUFFER
        pMode->Flags  = 0x000000A1;

        pMode->Format = EmuPC2XB_D3DFormat(PCMode.Format);
    }
    else
    {
        hRet = D3DERR_INVALIDCALL;
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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3D8_KickOffAndWaitForIdle()\n", GetCurrentThreadId());

    // TODO: Actually do something here?

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3D8_KickOffAndWaitForIdle2
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3D8_KickOffAndWaitForIdle2(DWORD dwDummy1, DWORD dwDummy2)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3D8_KickOffAndWaitForIdle\n"
           "(\n"
           "   dwDummy1            : 0x%.08X\n"
           "   dwDummy2            : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), dwDummy1, dwDummy2);

    // TODO: Actually do something here?

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetGammaRamp
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetGammaRamp
(
    DWORD                   dwFlags,
    CONST X_D3DGAMMARAMP   *pRamp
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetGammaRamp\n"
           "(\n"
           "   dwFlags             : 0x%.08X\n"
           "   pRamp               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), dwFlags, pRamp);

    // remove D3DSGR_IMMEDIATE
    DWORD dwPCFlags = dwFlags & (~0x00000002);
    D3DGAMMARAMP PCRamp;

    for(int v=0;v<255;v++)
    {
        PCRamp.red[v]   = pRamp->red[v];
        PCRamp.green[v] = pRamp->green[v];
        PCRamp.blue[v]  = pRamp->blue[v];
    }

    g_pD3DDevice8->SetGammaRamp(dwPCFlags, &PCRamp);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_AddRef
// ******************************************************************
ULONG WINAPI XTL::EmuIDirect3DDevice8_AddRef()
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_AddRef()\n", GetCurrentThreadId());

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_BeginStateBlock()\n", GetCurrentThreadId());

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_CaptureStateBlock\n"
           "(\n"
           "   Token               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Token);

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_ApplyStateBlock\n"
           "(\n"
           "   Token               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Token);

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_EndStateBlock\n"
           "(\n"
           "   pToken              : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pToken);

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_CopyRects\n"
           "(\n"
           "   pSourceSurface      : 0x%.08X\n"
           "   pSourceRectsArray   : 0x%.08X\n"
           "   cRects              : 0x%.08X\n"
           "   pDestinationSurface : 0x%.08X\n"
           "   pDestPointsArray    : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pSourceSurface, pSourceRectsArray, cRects,
           pDestinationSurface, pDestPointsArray);

    pSourceSurface->EmuSurface8->UnlockRect();

    /*
    static int kthx = 0;
    char fileName[255];

    sprintf(fileName, "C:\\Aaron\\Textures\\SourceSurface-%d.bmp", kthx++);

    D3DXSaveSurfaceToFile(fileName, D3DXIFF_BMP, pSourceSurface->EmuSurface8, NULL, NULL);
    //*/

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_CreateImageSurface\n"
           "(\n"
           "   Width               : 0x%.08X\n"
           "   Height              : 0x%.08X\n"
           "   Format              : 0x%.08X\n"
           "   ppBackBuffer        : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Width, Height, Format, ppBackBuffer);

    *ppBackBuffer = new X_D3DSurface();

    D3DFORMAT PCFormat = EmuXB2PC_D3DFormat(Format);

    HRESULT hRet = g_pD3DDevice8->CreateImageSurface(Width, Height, PCFormat, &((*ppBackBuffer)->EmuSurface8));

    EmuSwapFS();   // Xbox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetGammaRamp
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_GetGammaRamp
(
    X_D3DGAMMARAMP     *pRamp
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetGammaRamp\n"
           "(\n"
           "   pRamp               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pRamp);

    D3DGAMMARAMP *pGammaRamp = (D3DGAMMARAMP *)malloc(sizeof(D3DGAMMARAMP));

    g_pD3DDevice8->GetGammaRamp(pGammaRamp);

    for(int v=0;v<256;v++)
    {
        pRamp->red[v] = (BYTE)pGammaRamp->red[v];
        pRamp->green[v] = (BYTE)pGammaRamp->green[v];
        pRamp->blue[v] = (BYTE)pGammaRamp->blue[v];
    }

    free(pGammaRamp);

    EmuSwapFS();   // Xbox FS

    return;
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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetBackBuffer2\n"
           "(\n"
           "   BackBuffer          : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), BackBuffer);

    /** unsafe, somehow
    HRESULT hRet = S_OK;

    X_D3DSurface *pBackBuffer = new X_D3DSurface();

    if(BackBuffer == -1)
    {
        static IDirect3DSurface8 *pCachedPrimarySurface = 0;

        if(pCachedPrimarySurface == 0)
        {
            // create a buffer to return
            // TODO: Verify the surface is always 640x480
            g_pD3DDevice8->CreateImageSurface(640, 480, D3DFMT_A8R8G8B8, &pCachedPrimarySurface);
        }

        pBackBuffer->EmuSurface8 = pCachedPrimarySurface;

        hRet = g_pD3DDevice8->GetFrontBuffer(pBackBuffer->EmuSurface8);

        if(FAILED(hRet))
        {
            EmuWarning("Could not retrieve primary surface, using backbuffer");
            pCachedPrimarySurface = 0;
            pBackBuffer->EmuSurface8->Release();
            pBackBuffer->EmuSurface8 = 0;
            BackBuffer = 0;
        }

        // Debug: Save this image temporarily
        //D3DXSaveSurfaceToFile("C:\\Aaron\\Textures\\FrontBuffer.bmp", D3DXIFF_BMP, pBackBuffer->EmuSurface8, NULL, NULL);
    }

    if(BackBuffer != -1)
        hRet = g_pD3DDevice8->GetBackBuffer(BackBuffer, D3DBACKBUFFER_TYPE_MONO, &(pBackBuffer->EmuSurface8));
    //*/

    static X_D3DSurface *pBackBuffer = new X_D3DSurface();

    if(BackBuffer == -1)
        BackBuffer = 0;

    HRESULT hRet = g_pD3DDevice8->GetBackBuffer(BackBuffer, D3DBACKBUFFER_TYPE_MONO, &(pBackBuffer->EmuSurface8));

    if(FAILED(hRet))
        CxbxKrnlCleanup("Unable to retrieve back buffer");

    // update data pointer
    pBackBuffer->Data = X_D3DRESOURCE_DATA_FLAG_SPECIAL | X_D3DRESOURCE_DATA_FLAG_SURFACE;

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
    // debug trace
    #ifdef _DEBUG_TRACE
    {
        EmuSwapFS();   // Win2k/XP FS
        DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetBackBuffer\n"
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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetViewport\n"
           "(\n"
           "   pViewport           : 0x%.08X (%d, %d, %d, %d, %f, %f)\n"
           ");\n",
           GetCurrentThreadId(), pViewport, pViewport->X, pViewport->Y, pViewport->Width,
           pViewport->Height, pViewport->MinZ, pViewport->MaxZ);

    DWORD dwWidth  = pViewport->Width;
    DWORD dwHeight = pViewport->Height;

    // resize to fit screen (otherwise crashes occur)
    {
        if(dwWidth > 640)
        {
            EmuWarning("Resizing Viewport->Width to 640");
            ((D3DVIEWPORT8*)pViewport)->Width = 640;
        }

        if(dwHeight > 480)
        {
            EmuWarning("Resizing Viewport->Height to 480");
            ((D3DVIEWPORT8*)pViewport)->Height = 480;
        }
    }

    HRESULT hRet = g_pD3DDevice8->SetViewport(pViewport);

    // restore originals
    {
        if(dwWidth > 640)
            ((D3DVIEWPORT8*)pViewport)->Width = dwWidth;

        if(dwHeight > 480)
            ((D3DVIEWPORT8*)pViewport)->Height = dwHeight;
    }

    if(FAILED(hRet))
    {
        EmuWarning("Unable to set viewport!");
        hRet = D3D_OK;
    }

    EmuSwapFS();   // Xbox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetViewport
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_GetViewport
(
    D3DVIEWPORT8 *pViewport
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetViewport\n"
           "(\n"
           "   pViewport           : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pViewport);

    HRESULT hRet = g_pD3DDevice8->GetViewport(pViewport);

    if(FAILED(hRet))
    {
        EmuWarning("Unable to get viewport!");
        hRet = D3D_OK;
    }

    EmuSwapFS();   // Xbox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetViewportOffsetAndScale
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_GetViewportOffsetAndScale
(
    D3DXVECTOR4 *pOffset,
    D3DXVECTOR4 *pScale
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetViewportOffsetAndScale\n"
           "(\n"
           "   pOffset             : 0x%.08X\n"
           "   pScale              : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(),pOffset,pScale);

    float fScaleX = 1.0f;
    float fScaleY = 1.0f;
    float fScaleZ = 1.0f;
    float fOffsetX = 0.5 + 1.0/32;
    float fOffsetY = 0.5 + 1.0/32;
    D3DVIEWPORT8 Viewport;

    EmuSwapFS();
    EmuIDirect3DDevice8_GetViewport(&Viewport);
    EmuSwapFS();


    pScale->x = 1.0f;
    pScale->y = 1.0f;
    pScale->z = 1.0f;
    pScale->w = 1.0f;

    pOffset->x = 0.0f;
    pOffset->y = 0.0f;
    pOffset->z = 0.0f;
    pOffset->w = 0.0f;

/*
    pScale->x = (float)Viewport.Width * 0.5f * fScaleX;
    pScale->y = (float)Viewport.Height * -0.5f * fScaleY;
    pScale->z = (Viewport.MaxZ - Viewport.MinZ) * fScaleZ;
    pScale->w = 0;

    pOffset->x = (float)Viewport.Width * fScaleX * 0.5f + (float)Viewport.X * fScaleX + fOffsetX;
    pOffset->y = (float)Viewport.Height * fScaleY * 0.5f + (float)Viewport.Y * fScaleY + fOffsetY;
    pOffset->z = Viewport.MinZ * fScaleZ;
    pOffset->w = 0;
*/

    EmuSwapFS();   // XBox FS
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetShaderConstantMode
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_SetShaderConstantMode
(
    XTL::X_VERTEXSHADERCONSTANTMODE Mode
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetShaderConstantMode\n"
           "(\n"
           "   Mode                : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Mode);

    g_VertexShaderConstantMode = Mode;

    EmuSwapFS();   // Xbox FS

    return S_OK;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_Reset
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_Reset
(
    X_D3DPRESENT_PARAMETERS *pPresentationParameters
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_Reset\n"
           "(\n"
           "   pPresentationParameters  : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pPresentationParameters);

    EmuWarning("Device Reset is being utterly ignored");

    EmuSwapFS();   // Xbox FS

    return D3D_OK;
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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetRenderTarget\n"
           "(\n"
           "   ppRenderTarget      : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), ppRenderTarget);

    IDirect3DSurface8 *pSurface8 = g_pCachedRenderTarget->EmuSurface8;

    pSurface8->AddRef();

    *ppRenderTarget = g_pCachedRenderTarget;

    DbgPrintf("EmuD3D8 (0x%X): RenderTarget := 0x%.08X\n", GetCurrentThreadId(), pSurface8);

    EmuSwapFS();   // Xbox FS

    return D3D_OK;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetRenderTarget2
// ******************************************************************
XTL::X_D3DSurface * WINAPI XTL::EmuIDirect3DDevice8_GetRenderTarget2()
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetRenderTarget2()\n", GetCurrentThreadId());

    IDirect3DSurface8 *pSurface8 = g_pCachedRenderTarget->EmuSurface8;

    pSurface8->AddRef();

    DbgPrintf("EmuD3D8 (0x%X): RenderTarget := 0x%.08X\n", GetCurrentThreadId(), pSurface8);

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetDepthStencilSurface\n"
           "(\n"
           "   ppZStencilSurface   : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), ppZStencilSurface);

    IDirect3DSurface8 *pSurface8 = g_pCachedZStencilSurface->EmuSurface8;

    if(pSurface8 != 0)
        pSurface8->AddRef();

    *ppZStencilSurface = g_pCachedZStencilSurface;

    DbgPrintf("EmuD3D8 (0x%X): DepthStencilSurface := 0x%.08X\n", GetCurrentThreadId(), pSurface8);

    EmuSwapFS();   // Xbox FS

    return D3D_OK;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetDepthStencilSurface2
// ******************************************************************
XTL::X_D3DSurface * WINAPI XTL::EmuIDirect3DDevice8_GetDepthStencilSurface2()
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetDepthStencilSurface2()\n", GetCurrentThreadId());

    IDirect3DSurface8 *pSurface8 = g_pCachedZStencilSurface->EmuSurface8;

    if(pSurface8 != 0)
        pSurface8->AddRef();

    DbgPrintf("EmuD3D8 (0x%X): DepthStencilSurface := 0x%.08X\n", GetCurrentThreadId(), pSurface8);

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetTile\n"
           "(\n"
           "   Index               : 0x%.08X\n"
           "   pTile               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Index, pTile);

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetTileNoWait\n"
           "(\n"
           "   Index               : 0x%.08X\n"
           "   pTile               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Index, pTile);

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_CreateVertexShader\n"
           "(\n"
           "   pDeclaration        : 0x%.08X\n"
           "   pFunction           : 0x%.08X\n"
           "   pHandle             : 0x%.08X\n"
           "   Usage               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pDeclaration, pFunction, pHandle, Usage);

    // create emulated shader struct
    X_D3DVertexShader *pD3DVertexShader = (X_D3DVertexShader*)CxbxMalloc(sizeof(X_D3DVertexShader));
    VERTEX_SHADER     *pVertexShader = (VERTEX_SHADER*)CxbxMalloc(sizeof(VERTEX_SHADER));

    // TODO: Intelligently fill out these fields as necessary
    ZeroMemory(pD3DVertexShader, sizeof(X_D3DVertexShader));
    ZeroMemory(pVertexShader, sizeof(VERTEX_SHADER));

    // HACK: TODO: support this situation
    if(pDeclaration == NULL)
    {
        *pHandle = NULL;

        EmuSwapFS();   // Win2k/XP FS

        return S_OK;
    }

    LPD3DXBUFFER pRecompiledBuffer = NULL;
    DWORD        *pRecompiledDeclaration;
    DWORD        *pRecompiledFunction = NULL;
    DWORD        VertexShaderSize;
    DWORD        DeclarationSize;
    DWORD        Handle = 0;

    HRESULT hRet = XTL::EmuRecompileVshDeclaration((DWORD*)pDeclaration,
                                                   &pRecompiledDeclaration,
                                                   &DeclarationSize,
                                                   pFunction == NULL,
                                                   &pVertexShader->VertexDynamicPatch);

    if(SUCCEEDED(hRet) && pFunction)
    {
        hRet = XTL::EmuRecompileVshFunction((DWORD*)pFunction,
                                            &pRecompiledBuffer,
                                            &VertexShaderSize,
                                            g_VertexShaderConstantMode == X_VSCM_NONERESERVED);
        if(SUCCEEDED(hRet))
        {
            pRecompiledFunction = (DWORD*)pRecompiledBuffer->GetBufferPointer();
        }
        else
        {
            pRecompiledFunction = NULL;
            EmuWarning("Couldn't recompile vertex shader function.\n");
            hRet = D3D_OK; // Try using a fixed function vertex shader instead
        }
    }

    //DbgPrintf("MaxVertexShaderConst = %d\n", g_D3DCaps.MaxVertexShaderConst);

    if(SUCCEEDED(hRet))
    {
        hRet = g_pD3DDevice8->CreateVertexShader
        (
            pRecompiledDeclaration,
            pRecompiledFunction,
            &Handle,
            g_dwVertexShaderUsage   // TODO: HACK: Xbox has extensions!
        );
        if(pRecompiledBuffer)
        {
            pRecompiledBuffer->Release();
            pRecompiledBuffer = NULL;
        }
    }
    // Save the status, to remove things later
    pVertexShader->Status = hRet;

    CxbxFree(pRecompiledDeclaration);

    pVertexShader->pDeclaration = (DWORD*)CxbxMalloc(DeclarationSize);
    memcpy(pVertexShader->pDeclaration, pDeclaration, DeclarationSize);

    pVertexShader->FunctionSize = 0;
    pVertexShader->pFunction = NULL;
    pVertexShader->Type = X_VST_NORMAL;
    pVertexShader->Size = (VertexShaderSize - sizeof(VSH_SHADER_HEADER)) / VSH_INSTRUCTION_SIZE_BYTES;
    pVertexShader->DeclarationSize = DeclarationSize;

    if(SUCCEEDED(hRet))
    {
        if(pFunction != NULL)
        {
            pVertexShader->pFunction = (DWORD*)CxbxMalloc(VertexShaderSize);
            memcpy(pVertexShader->pFunction, pFunction, VertexShaderSize);
            pVertexShader->FunctionSize = VertexShaderSize;
        }
        else
        {
            pVertexShader->pFunction = NULL;
            pVertexShader->FunctionSize = 0;
        }
        pVertexShader->Handle = Handle;
    }
    else
    {
        pVertexShader->Handle = D3DFVF_XYZ | D3DFVF_TEX0;
    }

    pD3DVertexShader->Handle = (DWORD)pVertexShader;

    *pHandle = ((DWORD)pD3DVertexShader) | 0x80000000;

    if(FAILED(hRet))
    {
#ifdef _DEBUG_TRACK_VS
        if (pFunction)
        {
            char pFileName[30];
            static int FailedShaderCount = 0;
            VSH_SHADER_HEADER *pHeader = (VSH_SHADER_HEADER*)pFunction;
            EmuWarning("Couldn't create vertex shader!");
            sprintf(pFileName, "failed%05d.xvu", FailedShaderCount);
            FILE *f = fopen(pFileName, "wb");
            if(f)
            {
                fwrite(pFunction, sizeof(VSH_SHADER_HEADER) + pHeader->NumInst * 16, 1, f);
                fclose(f);
            }
            FailedShaderCount++;
        }
#endif // _DEBUG_TRACK_VS
        //hRet = D3D_OK;
    }

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetPixelShaderConstant
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetPixelShaderConstant
(
    DWORD       Register,
    CONST PVOID pConstantData,
    DWORD       ConstantCount
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetPixelShaderConstant\n"
           "(\n"
           "   Register            : 0x%.08X\n"
           "   pConstantData       : 0x%.08X\n"
           "   ConstantCount       : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Register, pConstantData, ConstantCount);

    EmuSwapFS();   // XBox FS

    return;
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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetVertexShaderConstant\n"
           "(\n"
           "   Register            : 0x%.08X\n"
           "   pConstantData       : 0x%.08X\n"
           "   ConstantCount       : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Register, pConstantData, ConstantCount);

#ifdef _DEBUG_TRACK_VS_CONST
    for (uint32 i = 0; i < ConstantCount; i++)
    {
        printf("SetVertexShaderConstant, c%d (c%d) = { %f, %f, %f, %f }\n",
               Register - 96 + i, Register + i,
               *((float*)pConstantData + 4 * i),
               *((float*)pConstantData + 4 * i + 1),
               *((float*)pConstantData + 4 * i + 2),
               *((float*)pConstantData + 4 * i + 3));
    }
#endif // _DEBUG_TRACK_VS_CONST

    HRESULT hRet = g_pD3DDevice8->SetVertexShaderConstant
    (
        Register,
        pConstantData,
        ConstantCount
    );

    if(FAILED(hRet))
    {
        EmuWarning("We're lying about setting a vertex shader constant!");

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
    // debug trace
    #ifdef _DEBUG_TRACE
    {
        EmuSwapFS();   // Win2k/XP FS
        DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetVertexShaderConstant1\n"
               "(\n"
               "   Register            : 0x%.08X\n"
               "   pConstantData       : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Register, pConstantData);
        EmuSwapFS();   // XBox FS
    }
    #endif

    XTL::EmuIDirect3DDevice8_SetVertexShaderConstant(Register, pConstantData, 1);

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
    // debug trace
    #ifdef _DEBUG_TRACE
    {
        EmuSwapFS();   // Win2k/XP FS
        DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetVertexShaderConstant4\n"
               "(\n"
               "   Register            : 0x%.08X\n"
               "   pConstantData       : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Register, pConstantData);
        EmuSwapFS();   // XBox FS
    }
    #endif

    XTL::EmuIDirect3DDevice8_SetVertexShaderConstant(Register, pConstantData, 4);

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexShaderConstantNotInline
// ******************************************************************
VOID __fastcall XTL::EmuIDirect3DDevice8_SetVertexShaderConstantNotInline
(
    INT         Register,
    CONST PVOID pConstantData,
    DWORD       ConstantCount
)
{
    // debug trace
    #ifdef _DEBUG_TRACE
    {
        EmuSwapFS();   // Win2k/XP FS
        DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetVertexShaderConstantNotInline\n"
               "(\n"
               "   Register            : 0x%.08X\n"
               "   pConstantData       : 0x%.08X\n"
               "   ConstantCount       : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Register, pConstantData, ConstantCount);
        EmuSwapFS();   // XBox FS
    }
    #endif

    XTL::EmuIDirect3DDevice8_SetVertexShaderConstant(Register, pConstantData, ConstantCount / 4);

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_DeletePixelShader
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_DeletePixelShader
(
    DWORD          Handle
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_DeletePixelShader\n"
           "(\n"
           "   Handle              : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Handle);

    if(Handle == X_PIXELSHADER_FAKE_HANDLE)
    {
        // Do Nothing!
    }
    else
    {
        g_pD3DDevice8->DeletePixelShader(Handle);
    }

    EmuSwapFS();   // XBox FS

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_CreatePixelShader\n"
           "(\n"
           "   pFunction           : 0x%.08X\n"
           "   pHandle             : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pFunction, pHandle);

    // redirect to windows d3d
    HRESULT hRet = g_pD3DDevice8->CreatePixelShader
    (
        pFunction,
        pHandle
    );

    if(FAILED(hRet))
    {
        *pHandle = X_PIXELSHADER_FAKE_HANDLE;

        EmuWarning("We're lying about the creation of a pixel shader!");

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetPixelShader\n"
           "(\n"
           "   Handle             : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Handle);

    // redirect to windows d3d
    HRESULT hRet = D3D_OK;

    // Fake Programmable Pipeline
    if(Handle == X_PIXELSHADER_FAKE_HANDLE)
    {
        // programmable pipeline
        //*
        static DWORD dwHandle = 0;

        if(dwHandle == 0)
        {
            // simplest possible pixel shader (diffuse only)
            static const char szDiffusePixelShader[] =
                "ps.1.3                         \n"
                "                               \n"
                "tex t0                         \n"
                "tex t1                         \n"
                "                               \n"
                "mul r0, t0, t1                 \n";
//                "mul r0, t0, t1.a                 \n";
//                "mov r0, v0                     \n";

            LPD3DXBUFFER pShader = 0;
            LPD3DXBUFFER pErrors = 0;

            // assemble the shader
            D3DXAssembleShader(szDiffusePixelShader, strlen(szDiffusePixelShader) - 1, 0, NULL, &pShader, &pErrors);

            // create the shader device handle
            hRet = g_pD3DDevice8->CreatePixelShader((DWORD*)pShader->GetBufferPointer(), &dwHandle);

            if(FAILED(hRet))
                EmuWarning("Could not create pixel shader");
        }

        if(!FAILED(hRet))
            hRet = g_pD3DDevice8->SetPixelShader(dwHandle);

        if(FAILED(hRet))
            EmuWarning("Could not set pixel shader!");
        //*/

        g_bFakePixelShaderLoaded = TRUE;
    }
    // Fixed Pipeline, or Recompiled Programmable Pipeline
    else if(Handle == NULL)
    {
        g_bFakePixelShaderLoaded = FALSE;
        g_pD3DDevice8->SetPixelShader(Handle);
    }

    if(FAILED(hRet))
    {
        EmuWarning("We're lying about setting a pixel shader!");

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

    switch(D3DResource)
    {
        case 3: /*D3DRTYPE_TEXTURE*/
            EmuIDirect3DDevice8_CreateTexture(Width, Height, Levels, Usage, Format, D3DPOOL_MANAGED, &pTexture);
            break;
        case 4: /*D3DRTYPE_VOLUMETEXTURE*/
            EmuIDirect3DDevice8_CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, D3DPOOL_MANAGED, (X_D3DVolumeTexture**)&pTexture);
            break;
        case 5: /*D3DRTYPE_CUBETEXTURE*/
            CxbxKrnlCleanup("Cube textures temporarily not supported!");
            break;
        default:
            CxbxKrnlCleanup("D3DResource = %d is not supported!", D3DResource);
    }

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_CreateTexture\n"
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

    // Convert Format (Xbox->PC)
    D3DFORMAT PCFormat = EmuXB2PC_D3DFormat(Format);

    // TODO: HACK: Devices that don't support this should somehow emulate it!
    //* This is OK on my GeForce FX 5600
    if(PCFormat == D3DFMT_D16)
    {
        EmuWarning("D3DFMT_D16 is an unsupported texture format!");
        PCFormat = D3DFMT_R5G6B5;
    }
    //*
    else if(PCFormat == D3DFMT_P8)
    {
        EmuWarning("D3DFMT_P8 is an unsupported texture format!");
        PCFormat = D3DFMT_X8R8G8B8;
    }
    //*/
    //* This is OK on my GeForce FX 5600
    else if(PCFormat == D3DFMT_D24S8)
    {
        EmuWarning("D3DFMT_D24S8 is an unsupported texture format!");
        PCFormat = D3DFMT_X8R8G8B8;
    }//*/
    else if(PCFormat == D3DFMT_YUY2)
    {
        // cache the overlay size
        g_dwOverlayW = Width;
        g_dwOverlayH = Height;
        g_dwOverlayP = RoundUp(g_dwOverlayW, 64)*2;
    }

    HRESULT hRet;

    if(PCFormat != D3DFMT_YUY2)
    {
        DWORD   PCUsage = Usage & (D3DUSAGE_RENDERTARGET);
//        DWORD   PCUsage = Usage & (D3DUSAGE_RENDERTARGET | D3DUSAGE_DEPTHSTENCIL);
        D3DPOOL PCPool  = D3DPOOL_MANAGED;

        EmuAdjustPower2(&Width, &Height);

        *ppTexture = new X_D3DTexture();

//        if(Usage & (D3DUSAGE_RENDERTARGET | D3DUSAGE_DEPTHSTENCIL))
        if(Usage & (D3DUSAGE_RENDERTARGET))
            PCPool = D3DPOOL_DEFAULT;

        hRet = g_pD3DDevice8->CreateTexture
        (
            Width, Height, Levels, 
            PCUsage,  // TODO: Xbox Allows a border to be drawn (maybe hack this in software ;[)
            PCFormat, PCPool, &((*ppTexture)->EmuTexture8)
        );

        if(FAILED(hRet))
        {
            EmuWarning("CreateTexture Failed!");
            (*ppTexture)->Data = 0xBEADBEAD;
        }
        else
        {
            D3DLOCKED_RECT LockedRect;

            (*ppTexture)->EmuTexture8->LockRect(0, &LockedRect, NULL, NULL);

            (*ppTexture)->Data = (DWORD)LockedRect.pBits;
            (*ppTexture)->Format = Format << X_D3DFORMAT_FORMAT_SHIFT;

            g_DataToTexture.insert((*ppTexture)->Data, *ppTexture);

            (*ppTexture)->EmuTexture8->UnlockRect(0);
        }

        DbgPrintf("EmuD3D8 (0x%X): Created Texture : 0x%.08X (0x%.08X)\n", GetCurrentThreadId(), *ppTexture, (*ppTexture)->EmuTexture8);
    }
    else
    {
        DWORD dwSize = g_dwOverlayP*g_dwOverlayH;
        DWORD dwPtr = (DWORD)CxbxMalloc(dwSize + sizeof(DWORD));

        DWORD *pRefCount = (DWORD*)(dwPtr + dwSize);

        // initialize ref count
        *pRefCount = 1;

        // If YUY2 is not supported in hardware, we'll actually mark this as a special fake texture (set highest bit)
        *ppTexture = new X_D3DTexture();
        
        (*ppTexture)->Data = X_D3DRESOURCE_DATA_FLAG_SPECIAL | X_D3DRESOURCE_DATA_FLAG_YUVSURF;
        (*ppTexture)->Lock = dwPtr;
        (*ppTexture)->Format = 0x24;

        (*ppTexture)->Size  = (g_dwOverlayW & X_D3DSIZE_WIDTH_MASK);
        (*ppTexture)->Size |= (g_dwOverlayH << X_D3DSIZE_HEIGHT_SHIFT);
        (*ppTexture)->Size |= (g_dwOverlayP << X_D3DSIZE_PITCH_SHIFT);

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_CreateVolumeTexture\n"
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

    // Convert Format (Xbox->PC)
    D3DFORMAT PCFormat = EmuXB2PC_D3DFormat(Format);

    // TODO: HACK: Devices that don't support this should somehow emulate it!
    if(PCFormat == D3DFMT_D16)
    {
        EmuWarning("D3DFMT_16 is an unsupported texture format!");
        PCFormat = D3DFMT_X8R8G8B8;
    }
    else if(PCFormat == D3DFMT_P8)
    {
        EmuWarning("D3DFMT_P8 is an unsupported texture format!");
        PCFormat = D3DFMT_X8R8G8B8;
    }
    else if(PCFormat == D3DFMT_D24S8)
    {
        EmuWarning("D3DFMT_D24S8 is an unsupported texture format!");
        PCFormat = D3DFMT_X8R8G8B8;
    }
    else if(PCFormat == D3DFMT_YUY2)
    {
        // cache the overlay size
        g_dwOverlayW = Width;
        g_dwOverlayH = Height;
        g_dwOverlayP = RoundUp(g_dwOverlayW, 64)*2;
    }

    HRESULT hRet;

    if(PCFormat != D3DFMT_YUY2)
    {
        EmuAdjustPower2(&Width, &Height);

        *ppVolumeTexture = new X_D3DVolumeTexture();

        hRet = g_pD3DDevice8->CreateVolumeTexture
        (
            Width, Height, Depth, Levels, 
            0,  // TODO: Xbox Allows a border to be drawn (maybe hack this in software ;[)
            PCFormat, D3DPOOL_MANAGED, &((*ppVolumeTexture)->EmuVolumeTexture8)
        );

        if(FAILED(hRet))
            EmuWarning("CreateVolumeTexture Failed! (0x%.08X)", hRet);

        DbgPrintf("EmuD3D8 (0x%X): Created Volume Texture : 0x%.08X (0x%.08X)\n", GetCurrentThreadId(), *ppVolumeTexture, (*ppVolumeTexture)->EmuVolumeTexture8);
    }
    else
    {
        DWORD dwSize = g_dwOverlayP*g_dwOverlayH;
        DWORD dwPtr = (DWORD)CxbxMalloc(dwSize + sizeof(DWORD));

        DWORD *pRefCount = (DWORD*)(dwPtr + dwSize);

        // initialize ref count
        *pRefCount = 1;

        // If YUY2 is not supported in hardware, we'll actually mark this as a special fake texture (set highest bit)
        (*ppVolumeTexture)->Data = X_D3DRESOURCE_DATA_FLAG_SPECIAL | X_D3DRESOURCE_DATA_FLAG_YUVSURF;
        (*ppVolumeTexture)->Lock = dwPtr;
        (*ppVolumeTexture)->Format = 0x24;

        (*ppVolumeTexture)->Size  = (g_dwOverlayW & X_D3DSIZE_WIDTH_MASK);
        (*ppVolumeTexture)->Size |= (g_dwOverlayH << X_D3DSIZE_HEIGHT_SHIFT);
        (*ppVolumeTexture)->Size |= (g_dwOverlayP << X_D3DSIZE_PITCH_SHIFT);

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_CreateCubeTexture\n"
           "(\n"
           "   EdgeLength          : 0x%.08X\n"
           "   Levels              : 0x%.08X\n"
           "   Usage               : 0x%.08X\n"
           "   Format              : 0x%.08X\n"
           "   Pool                : 0x%.08X\n"
           "   ppCubeTexture       : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture);

    // Convert Format (Xbox->PC)
    D3DFORMAT PCFormat = EmuXB2PC_D3DFormat(Format);

    // TODO: HACK: Devices that don't support this should somehow emulate it!
    if(PCFormat == D3DFMT_D16)
    {
        EmuWarning("D3DFMT_16 is an unsupported texture format!");
        PCFormat = D3DFMT_X8R8G8B8;
    }
    else if(PCFormat == D3DFMT_P8)
    {
        EmuWarning("D3DFMT_P8 is an unsupported texture format!");
        PCFormat = D3DFMT_X8R8G8B8;
    }
    else if(PCFormat == D3DFMT_D24S8)
    {
        EmuWarning("D3DFMT_D24S8 is an unsupported texture format!");
        PCFormat = D3DFMT_X8R8G8B8;
    }
    else if(PCFormat == D3DFMT_YUY2)
    {
        CxbxKrnlCleanup("YUV not supported for cube textures");
    }

    *ppCubeTexture = new X_D3DCubeTexture();

    HRESULT hRet = g_pD3DDevice8->CreateCubeTexture
    (
        EdgeLength, Levels, 
        0,  // TODO: Xbox Allows a border to be drawn (maybe hack this in software ;[)
        PCFormat, D3DPOOL_MANAGED, &((*ppCubeTexture)->EmuCubeTexture8)
    );

    DbgPrintf("EmuD3D8 (0x%X): Created Cube Texture : 0x%.08X (0x%.08X)\n", GetCurrentThreadId(), *ppCubeTexture, (*ppCubeTexture)->EmuCubeTexture8);

    if(FAILED(hRet))
        EmuWarning("CreateCubeTexture Failed!");

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_CreateIndexBuffer\n"
           "(\n"
           "   Length              : 0x%.08X\n"
           "   Usage               : 0x%.08X\n"
           "   Format              : 0x%.08X\n"
           "   Pool                : 0x%.08X\n"
           "   ppIndexBuffer       : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Length, Usage, Format, Pool, ppIndexBuffer);

    *ppIndexBuffer = new X_D3DIndexBuffer();

    HRESULT hRet = g_pD3DDevice8->CreateIndexBuffer
    (
        Length, NULL, D3DFMT_INDEX16, D3DPOOL_MANAGED, &((*ppIndexBuffer)->EmuIndexBuffer8)
    );

    DbgPrintf("EmuD3D8 (0x%X): EmuIndexBuffer8 := 0x%.08X\n", GetCurrentThreadId(), (*ppIndexBuffer)->EmuIndexBuffer8);

    if(FAILED(hRet))
        EmuWarning("CreateIndexBuffer Failed! (0x%.08X)", hRet);

    //
    // update data ptr
    //

    {
        BYTE *pData = NULL;
    
        (*ppIndexBuffer)->EmuIndexBuffer8->Lock(0, Length, &pData, NULL);

        (*ppIndexBuffer)->Data = (DWORD)pData;
    }

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateIndexBuffer2
// ******************************************************************
XTL::X_D3DIndexBuffer * WINAPI XTL::EmuIDirect3DDevice8_CreateIndexBuffer2(UINT Length)
{
    X_D3DIndexBuffer *pIndexBuffer = NULL;
    
    EmuIDirect3DDevice8_CreateIndexBuffer
    (
        Length,
        NULL,
        D3DFMT_INDEX16,
        D3DPOOL_MANAGED,
        &pIndexBuffer
    );

    return pIndexBuffer;
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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetIndices\n"
           "(\n"
           "   pIndexData          : 0x%.08X\n"
           "   BaseVertexIndex     : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pIndexData, BaseVertexIndex);

    /*
    fflush(stdout);
    if(pIndexData != 0)
    {
        static int chk = 0;
        if(chk++ == 0)
        {
            _asm int 3
        }
    }
    //*/

    HRESULT hRet = D3D_OK;

    if(pIndexData != 0)
    {
        g_pIndexBuffer = pIndexData;
        g_dwBaseVertexIndex = BaseVertexIndex;

        // HACK: Halo Hack
        if(pIndexData->Lock == 0x00840863)
            pIndexData->Lock = 0;

        EmuVerifyResourceIsRegistered(pIndexData);

        IDirect3DIndexBuffer8 *pIndexBuffer = pIndexData->EmuIndexBuffer8;

        if(pIndexData->Lock != X_D3DRESOURCE_LOCK_FLAG_NOSIZE)
            hRet = g_pD3DDevice8->SetIndices(pIndexBuffer, BaseVertexIndex);
    }
    else
    {
        g_pIndexBuffer = 0;

        hRet = g_pD3DDevice8->SetIndices(0, BaseVertexIndex);
    }

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetTexture\n"
           "(\n"
           "   Stage               : 0x%.08X\n"
           "   pTexture            : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Stage, pTexture);

    IDirect3DBaseTexture8 *pBaseTexture8 = NULL;

    EmuD3DActiveTexture[Stage] = pTexture;

    if(pTexture != NULL)
    {        
        EmuVerifyResourceIsRegistered(pTexture);

        if(IsSpecialResource(pTexture->Data) && (pTexture->Data & X_D3DRESOURCE_DATA_FLAG_YUVSURF))
        {
            //
            // NOTE: TODO: This is almost a hack! :)
            //

            EmuSwapFS();
            EmuIDirect3DDevice8_EnableOverlay(TRUE);
            EmuIDirect3DDevice8_UpdateOverlay((X_D3DSurface*)pTexture, 0, 0, FALSE, 0);
            EmuSwapFS();
        }
        else
        {
            pBaseTexture8 = pTexture->EmuBaseTexture8;

            #ifdef _DEBUG_DUMP_TEXTURE_SETTEXTURE
            if(pTexture != NULL && (pTexture->EmuTexture8 != NULL))
            {
                static int dwDumpTexture = 0;

                char szBuffer[256];

                switch(pTexture->EmuResource8->GetType())
                {
                    case D3DRTYPE_TEXTURE:
                    {
                        sprintf(szBuffer, _DEBUG_DUMP_TEXTURE_SETTEXTURE "SetTextureNorm - %.03d (0x%.08X).bmp", dwDumpTexture++, pTexture->EmuTexture8);

                        pTexture->EmuTexture8->UnlockRect(0);

                        D3DXSaveTextureToFile(szBuffer, D3DXIFF_BMP, pTexture->EmuTexture8, NULL);
                    }
                    break;

                    case D3DRTYPE_CUBETEXTURE:
                    {
                        for(int face=0;face<6;face++)
                        {
                            sprintf(szBuffer, _DEBUG_DUMP_TEXTURE_SETTEXTURE "SetTextureCube%d - %.03d (0x%.08X).bmp", face, dwDumpTexture++, pTexture->EmuTexture8);

                            pTexture->EmuCubeTexture8->UnlockRect((D3DCUBEMAP_FACES)face, 0);
                            
                            D3DXSaveTextureToFile(szBuffer, D3DXIFF_BMP, pTexture->EmuTexture8, NULL);
                        }
                    }
                    break;
                }
            } 
            #endif
        }
    }

    /*
    static IDirect3DTexture8 *pDummyTexture[4] = {0, 0, 0, 0};

    if(pDummyTexture[Stage] == 0)
    {
        if(Stage == 0)
        {
            if(D3DXCreateTextureFromFile(g_pD3DDevice8, "C:\\dummy1.bmp", &pDummyTexture[Stage]) != D3D_OK)
                CxbxKrnlCleanup("Could not create dummy texture!");
        }
        else if(Stage == 1)
        {
            if(D3DXCreateTextureFromFile(g_pD3DDevice8, "C:\\dummy2.bmp", &pDummyTexture[Stage]) != D3D_OK)
                CxbxKrnlCleanup("Could not create dummy texture!");
        }
    }
    //*/

    /*
    static int dwDumpTexture = 0;
    char szBuffer[256];
    sprintf(szBuffer, "C:\\Aaron\\Textures\\DummyTexture - %.03d (0x%.08X).bmp", dwDumpTexture++, pDummyTexture);
    pDummyTexture->UnlockRect(0);
    D3DXSaveTextureToFile(szBuffer, D3DXIFF_BMP, pDummyTexture, NULL);
    //*/

    //HRESULT hRet = g_pD3DDevice8->SetTexture(Stage, pDummyTexture[Stage]);
    HRESULT hRet = g_pD3DDevice8->SetTexture(Stage, (g_iWireframe == 0) ? pBaseTexture8 : 0);

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SwitchTexture\n"
           "(\n"
           "   Method              : 0x%.08X\n"
           "   Data                : 0x%.08X\n"
           "   Format              : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Method, Data, Format);

    DWORD StageLookup[] = { 0x00081b00, 0x00081b40, 0x00081b80, 0x00081bc0 };
    DWORD Stage = -1;

    for(int v=0;v<4;v++)
    {
        if(StageLookup[v] == Method)
        {
            Stage = v;
        }
    }

    if(Stage == -1)
    {
        EmuWarning("Unknown Method (0x%.08X)", Method);
    }
    else
    {
        //
        // WARNING: TODO: Correct reference counting has not been completely verified for this code
        //

        X_D3DTexture *pTexture = (X_D3DTexture *)g_DataToTexture.get(Data);

        EmuWarning("Switching Texture 0x%.08X (0x%.08X) @ Stage %d", pTexture, pTexture->EmuBaseTexture8, Stage);

        HRESULT hRet = g_pD3DDevice8->SetTexture(Stage, pTexture->EmuBaseTexture8);

        /*
        if(pTexture->EmuBaseTexture8 != NULL)
        {
            static int dwDumpTexture = 0;

            char szBuffer[255];

            sprintf(szBuffer, "C:\\Aaron\\Textures\\0x%.08X-SwitchTexture%.03d.bmp", pTexture, dwDumpTexture++);

            pTexture->EmuTexture8->UnlockRect(0);

            D3DXSaveTextureToFile(szBuffer, D3DXIFF_BMP, pTexture->EmuBaseTexture8, NULL);
        } 
        //*/
    }

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetDisplayMode\n"
           "(\n"
           "   pMode               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pMode);

    HRESULT hRet;

    // make adjustments to parameters to make sense with windows d3d
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
// * func: EmuIDirect3DDevice8_Begin
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_Begin
(
    X_D3DPRIMITIVETYPE     PrimitiveType
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_Begin\n"
           "(\n"
           "   PrimitiveType       : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), PrimitiveType);

    if((PrimitiveType != X_D3DPT_TRIANGLEFAN) && (PrimitiveType != X_D3DPT_QUADSTRIP) && (PrimitiveType != X_D3DPT_QUADLIST))
        CxbxKrnlCleanup("EmuIDirect3DDevice8_Begin does not support primitive : %d", PrimitiveType);

    g_IVBPrimitiveType = PrimitiveType;

    if(g_IVBTable == 0)
    {
        g_IVBTable = (struct XTL::_D3DIVB*)CxbxMalloc(sizeof(XTL::_D3DIVB)*1024);
    }

    g_IVBTblOffs = 0;
    g_IVBFVF = 0;

    // default values
    ZeroMemory(g_IVBTable, sizeof(XTL::_D3DIVB)*1024);

    if(g_pIVBVertexBuffer == 0)
    {
        g_pIVBVertexBuffer = (DWORD*)CxbxMalloc(sizeof(XTL::_D3DIVB)*1024);
    }

    EmuSwapFS();   // XBox FS

    return D3D_OK;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexData2f
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_SetVertexData2f
(
    int     Register,
    FLOAT   a,
    FLOAT   b
)
{
    // debug trace
    #ifdef _DEBUG_TRACE
    {
        EmuSwapFS();   // Win2k/XP FS
        DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetVertexData2f >>\n"
               "(\n"
               "   Register            : 0x%.08X\n"
               "   a                   : %f\n"
               "   b                   : %f\n"
               ");\n",
               GetCurrentThreadId(), Register, a, b);
        EmuSwapFS();   // XBox FS
    }
    #endif

    return EmuIDirect3DDevice8_SetVertexData4f(Register, a, b, 0.0f, 1.0f);
}

static inline DWORD FtoDW(FLOAT f) { return *((DWORD*)&f); }
static inline FLOAT DWtoF(DWORD f) { return *((FLOAT*)&f); }

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexData2s
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_SetVertexData2s
(
    int     Register,
    SHORT   a,
    SHORT   b
)
{
    // debug trace
    #ifdef _DEBUG_TRACE
    {
        EmuSwapFS();   // Win2k/XP FS
        DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetVertexData2s >>\n"
               "(\n"
               "   Register            : 0x%.08X\n"
               "   a                   : %d\n"
               "   b                   : %d\n"
               ");\n",
               GetCurrentThreadId(), Register, a, b);
        EmuSwapFS();   // XBox FS
    }
    #endif

    DWORD dwA = a, dwB = b;

    return EmuIDirect3DDevice8_SetVertexData4f(Register, DWtoF(dwA), DWtoF(dwB), 0.0f, 1.0f);
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexData4f
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_SetVertexData4f
(
    int     Register,
    FLOAT   a,
    FLOAT   b,
    FLOAT   c,
    FLOAT   d
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetVertexData4f\n"
           "(\n"
           "   Register            : 0x%.08X\n"
           "   a                   : %f\n"
           "   b                   : %f\n"
           "   c                   : %f\n"
           "   d                   : %f\n"
           ");\n",
           GetCurrentThreadId(), Register, a, b, c, d);

    HRESULT hRet = S_OK;

    switch(Register)
    {
        case 0: // D3DVSDE_POSITION
        {
            int o = g_IVBTblOffs;

            g_IVBTable[o].Position.x = a;//vertices[o*2+0];//a;
            g_IVBTable[o].Position.y = b;//vertices[o*2+1];//b;
            g_IVBTable[o].Position.z = c;
            g_IVBTable[o].Rhw = 1.0f;

            g_IVBTblOffs++;

            g_IVBFVF |= D3DFVF_XYZRHW;
        }
        break;

        case 3: // D3DVSDE_DIFFUSE
        {
            int o = g_IVBTblOffs;

            DWORD ca = FtoDW(d) << 24;
            DWORD cr = FtoDW(a) << 16;
            DWORD cg = FtoDW(b) << 8;
            DWORD cb = FtoDW(c) << 0;

            g_IVBTable[o].dwDiffuse = ca | cr | cg | cb;

            g_IVBFVF |= D3DFVF_DIFFUSE;
        }
        break;

        case 4: // D3DVSDE_SPECULAR
        {
            int o = g_IVBTblOffs;

            DWORD ca = FtoDW(d) << 24;
            DWORD cr = FtoDW(a) << 16;
            DWORD cg = FtoDW(b) << 8;
            DWORD cb = FtoDW(c) << 0;

            g_IVBTable[o].dwSpecular = ca | cr | cg | cb;

            g_IVBFVF |= D3DFVF_SPECULAR;
        }
        break;

        case 9: // D3DVSDE_TEXCOORD0
        {
            int o = g_IVBTblOffs;
            /*
            if(a > 640) a = 640;
            if(b > 480) b = 480;

            if(a > 1.0f) a /= 640.0f;
            if(b > 1.0f) b /= 480.0f;
            //*/
            g_IVBTable[o].TexCoord1.x = a;
            g_IVBTable[o].TexCoord1.y = b;

            if( (g_IVBFVF & D3DFVF_TEXCOUNT_MASK) < D3DFVF_TEX1)
            {
                g_IVBFVF |= D3DFVF_TEX1;
            }
        }
        break;

        case 10: // D3DVSDE_TEXCOORD1
        {
            int o = g_IVBTblOffs;
            /*
            if(a > 640) a = 640;
            if(b > 480) b = 480;

            if(a > 1.0f) a /= 640.0f;
            if(b > 1.0f) b /= 480.0f;
            //*/
            g_IVBTable[o].TexCoord2.x = a;
            g_IVBTable[o].TexCoord2.y = b;

            if( (g_IVBFVF & D3DFVF_TEXCOUNT_MASK) < D3DFVF_TEX2)
            {
                g_IVBFVF |= D3DFVF_TEX2;
            }
        }
        break;

        case 11: // D3DVSDE_TEXCOORD2
        {
            int o = g_IVBTblOffs;
            /*
            if(a > 640) a = 640;
            if(b > 480) b = 480;

            if(a > 1.0f) a /= 640.0f;
            if(b > 1.0f) b /= 480.0f;
            //*/
            g_IVBTable[o].TexCoord3.x = a;
            g_IVBTable[o].TexCoord3.y = b;

            if( (g_IVBFVF & D3DFVF_TEXCOUNT_MASK) < D3DFVF_TEX3)
            {
                g_IVBFVF |= D3DFVF_TEX3;
            }
        }
        break;

        case 12: // D3DVSDE_TEXCOORD3
        {
            int o = g_IVBTblOffs;
            /*
            if(a > 640) a = 640;
            if(b > 480) b = 480;

            if(a > 1.0f) a /= 640.0f;
            if(b > 1.0f) b /= 480.0f;
            //*/
            g_IVBTable[o].TexCoord4.x = a;
            g_IVBTable[o].TexCoord4.y = b;

            if( (g_IVBFVF & D3DFVF_TEXCOUNT_MASK) < D3DFVF_TEX4)
            {
                g_IVBFVF |= D3DFVF_TEX4;
            }
        }
        break;

        case 0xFFFFFFFF:
        {
            int o = g_IVBTblOffs;

            /*
            a = (a*320.0f) + 320.0f;
            b = (b*240.0f) + 240.0f;
            //*/

            g_IVBTable[o].Position.x = a;//vertices[o*2+0];//a;
            g_IVBTable[o].Position.y = b;//vertices[o*2+1];//b;
            g_IVBTable[o].Position.z = c;
            g_IVBTable[o].Rhw = 1.0f;

            g_IVBTblOffs++;

            g_IVBFVF |= D3DFVF_XYZRHW;
        }
        break;
        
        default:
            CxbxKrnlCleanup("Unknown IVB Register : %d", Register);
    }

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexDataColor
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_SetVertexDataColor
(
    int         Register,
    D3DCOLOR    Color
)
{
    // debug trace
    #ifdef _DEBUG_TRACE
    {
        EmuSwapFS();   // Win2k/XP FS
        DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetVertexDataColor >>\n"
               "(\n"
               "   Register            : 0x%.08X\n"
               "   Color               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Register, Color);
        EmuSwapFS();   // XBox FS
    }
    #endif

    FLOAT a = DWtoF((Color & 0xFF000000) >> 24);
    FLOAT r = DWtoF((Color & 0x00FF0000) >> 16);
    FLOAT g = DWtoF((Color & 0x0000FF00) >> 8);
    FLOAT b = DWtoF((Color & 0x000000FF) >> 0);

    return EmuIDirect3DDevice8_SetVertexData4f(Register, r, g, b, a);
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_End
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_End()
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_End();\n", GetCurrentThreadId());

    if(g_IVBTblOffs != 0)
        EmuFlushIVB();

    // TODO: Should technically clean this up at some point..but on XP doesnt matter much
//    CxbxFree(g_pIVBVertexBuffer);
//    CxbxFree(g_IVBTable);

    EmuSwapFS();   // XBox FS

    return D3D_OK;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_RunPushBuffer
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_RunPushBuffer
(
    X_D3DPushBuffer       *pPushBuffer,
    X_D3DFixup            *pFixup
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_RunPushBuffer\n"
           "(\n"
           "   pPushBuffer         : 0x%.08X\n"
           "   pFixup              : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pPushBuffer, pFixup);

    XTL::EmuExecutePushBuffer(pPushBuffer, pFixup);

    EmuSwapFS();   // XBox FS

    return;
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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_Clear\n"
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
    
    // make adjustments to parameters to make sense with windows d3d
    {
        // TODO: D3DCLEAR_TARGET_A, *R, *G, *B don't exist on windows
        DWORD newFlags = 0;

        if(Flags & 0x000000f0)
            newFlags |= D3DCLEAR_TARGET;

        if(Flags & 0x00000001)
            newFlags |= D3DCLEAR_ZBUFFER;

        if(Flags & 0x00000002)
            newFlags |= D3DCLEAR_STENCIL;

        if(Flags & ~(0x000000f0 | 0x00000001 | 0x00000002))
            EmuWarning("Unsupported Flag(s) for IDirect3DDevice8_Clear : 0x%.08X", Flags & ~(0x000000f0 | 0x00000001 | 0x00000002));

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_Present\n"
           "(\n"
           "   pSourceRect         : 0x%.08X\n"
           "   pDestRect           : 0x%.08X\n"
           "   pDummy1             : 0x%.08X\n"
           "   pDummy2             : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pSourceRect, pDestRect, pDummy1, pDummy2);

    // release back buffer lock
    {
        IDirect3DSurface8 *pBackBuffer;

        g_pD3DDevice8->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);

        pBackBuffer->UnlockRect();
    }

    HRESULT hRet = g_pD3DDevice8->Present(pSourceRect, pDestRect, (HWND)pDummy1, (CONST RGNDATA*)pDummy2);

    // not really accurate because you definately dont always present on every vblank
    g_VBData.Swap = g_VBData.VBlank;

    if(g_VBData.VBlank == g_VBLastSwap + 1)
        g_VBData.Flags = 1; // D3DVBLANK_SWAPDONE
    else
        g_VBData.Flags = 2; // D3DVBLANK_SWAPMISSED

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_Swap\n"
           "(\n"
           "   Flags               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Flags);

    // TODO: Ensure this flag is always the same across library versions
    if(Flags != 0)
        EmuWarning("XTL::EmuIDirect3DDevice8_Swap: Flags != 0");

    // release back buffer lock
    {
        IDirect3DSurface8 *pBackBuffer;

        g_pD3DDevice8->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);

        pBackBuffer->UnlockRect();
    }

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DResource8_Register\n"
           "(\n"
           "   pThis               : 0x%.08X (->Data : 0x%.08X)\n"
           "   pBase               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis, pThis->Data, pBase);

    HRESULT hRet = S_OK;

    X_D3DResource *pResource = (X_D3DResource*)pThis;

    DWORD dwCommonType = pResource->Common & X_D3DCOMMON_TYPE_MASK;

    // add the offset of the current texture to the base
    pBase = (PVOID)((DWORD)pBase+pThis->Data);

    // Determine the resource type, and initialize
    switch(dwCommonType)
    {
        case X_D3DCOMMON_TYPE_VERTEXBUFFER:
        {
            DbgPrintf("EmuIDirect3DResource8_Register (0x%X) : Creating VertexBuffer...\n", GetCurrentThreadId());

            X_D3DVertexBuffer *pVertexBuffer = (X_D3DVertexBuffer*)pResource;

            // create vertex buffer
            {
                DWORD dwSize = EmuCheckAllocationSize(pBase, true);

                if(dwSize == -1)
                {
                    // TODO: once this is known to be working, remove the warning
                    EmuWarning("Vertex buffer allocation size unknown");
                    dwSize = 0x2000;  // temporarily assign a small buffer, which will be increased later
                }

                hRet = g_pD3DDevice8->CreateVertexBuffer
                (
                    dwSize, 0, 0, D3DPOOL_MANAGED,
                    &pResource->EmuVertexBuffer8
                );

                #ifdef _DEBUG_TRACK_VB
                g_VBTrackTotal.insert(pResource->EmuVertexBuffer8);
                #endif

                BYTE *pData = 0;

                hRet = pResource->EmuVertexBuffer8->Lock(0, 0, &pData, 0);

                if(FAILED(hRet))
                    CxbxKrnlCleanup("VertexBuffer Lock Failed!");

                memcpy(pData, (void*)pBase, dwSize);

                pResource->EmuVertexBuffer8->Unlock();

                pResource->Data = (ULONG)pData;
            }

            DbgPrintf("EmuIDirect3DResource8_Register (0x%X) : Successfully Created VertexBuffer (0x%.08X)\n", GetCurrentThreadId(), pResource->EmuVertexBuffer8);
        }
        break;

        case X_D3DCOMMON_TYPE_INDEXBUFFER:
        {
            DbgPrintf("EmuIDirect3DResource8_Register :-> IndexBuffer...\n");

            X_D3DIndexBuffer *pIndexBuffer = (X_D3DIndexBuffer*)pResource;

            // create index buffer
            {
                DWORD dwSize = EmuCheckAllocationSize(pBase, true);

                if(dwSize == -1)
                {
                    // TODO: once this is known to be working, remove the warning
                    EmuWarning("Index buffer allocation size unknown");

                    pIndexBuffer->Lock = X_D3DRESOURCE_LOCK_FLAG_NOSIZE;

                    break;
                    // Halo dwSize = 0x336;
                }

                HRESULT hRet = g_pD3DDevice8->CreateIndexBuffer
                (
                    dwSize, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED,
                    &pIndexBuffer->EmuIndexBuffer8
                );

                if(FAILED(hRet))
                    CxbxKrnlCleanup("CreateIndexBuffer Failed!");

                BYTE *pData = 0;

                hRet = pResource->EmuIndexBuffer8->Lock(0, dwSize, &pData, 0);

                if(FAILED(hRet))
                    CxbxKrnlCleanup("IndexBuffer Lock Failed!");

                memcpy(pData, (void*)pBase, dwSize);

                pResource->EmuIndexBuffer8->Unlock();

                pResource->Data = (ULONG)pData;
            }

            DbgPrintf("EmuIDirect3DResource8_Register (0x%X) : Successfully Created IndexBuffer (0x%.08X)\n", GetCurrentThreadId(), pResource->EmuIndexBuffer8);
        }
        break;

        case X_D3DCOMMON_TYPE_PUSHBUFFER:
        {
            DbgPrintf("EmuIDirect3DResource8_Register :-> PushBuffer...\n");

            X_D3DPushBuffer *pPushBuffer = (X_D3DPushBuffer*)pResource;

            // create push buffer
            {
                DWORD dwSize = EmuCheckAllocationSize(pBase, true);

                if(dwSize == -1)
                {
                    // TODO: once this is known to be working, remove the warning
                    EmuWarning("Push buffer allocation size unknown");

                    pPushBuffer->Lock = X_D3DRESOURCE_LOCK_FLAG_NOSIZE;

                    break;
                }

                pResource->Data = (ULONG)pBase;
            }

            DbgPrintf("EmuIDirect3DResource8_Register (0x%X) : Successfully Created PushBuffer (0x%.08X, 0x%.08X, 0x%.08X)\n", GetCurrentThreadId(), pResource->Data, pPushBuffer->Size, pPushBuffer->AllocationSize);
        }
        break;

        case X_D3DCOMMON_TYPE_SURFACE:
        case X_D3DCOMMON_TYPE_TEXTURE:
        {
            if(dwCommonType == X_D3DCOMMON_TYPE_SURFACE)
                DbgPrintf("EmuIDirect3DResource8_Register :-> Surface...\n");
            else
                DbgPrintf("EmuIDirect3DResource8_Register :-> Texture...\n");

            X_D3DPixelContainer *pPixelContainer = (X_D3DPixelContainer*)pResource;

            X_D3DFORMAT X_Format = (X_D3DFORMAT)((pPixelContainer->Format & X_D3DFORMAT_FORMAT_MASK) >> X_D3DFORMAT_FORMAT_SHIFT);
            D3DFORMAT   Format   = EmuXB2PC_D3DFormat(X_Format);

            // TODO: check for dimensions

            // TODO: HACK: Temporary?
            if(X_Format == 0x2E)
            {
                CxbxKrnlCleanup("D3DFMT_LIN_D24S8 not yet supported!");
                X_Format = 0x12;
                Format   = D3DFMT_A8R8G8B8;
            }

            DWORD dwWidth, dwHeight, dwBPP, dwDepth = 1, dwPitch = 0, dwMipMapLevels = 1;
            BOOL  bSwizzled = FALSE, bCompressed = FALSE, dwCompressedSize = 0;
            BOOL  bCubemap = pPixelContainer->Format & X_D3DFORMAT_CUBEMAP;

            // Interpret Width/Height/BPP
            if(X_Format == 0x07 /* X_D3DFMT_X8R8G8B8 */ || X_Format == 0x06 /* X_D3DFMT_A8R8G8B8 */)
            {
                bSwizzled = TRUE;

                // Swizzled 32 Bit
                dwWidth  = 1 << ((pPixelContainer->Format & X_D3DFORMAT_USIZE_MASK) >> X_D3DFORMAT_USIZE_SHIFT);
                dwHeight = 1 << ((pPixelContainer->Format & X_D3DFORMAT_VSIZE_MASK) >> X_D3DFORMAT_VSIZE_SHIFT);
                dwMipMapLevels = (pPixelContainer->Format & X_D3DFORMAT_MIPMAP_MASK) >> X_D3DFORMAT_MIPMAP_SHIFT;
                dwDepth  = 1;// HACK? 1 << ((pPixelContainer->Format & X_D3DFORMAT_PSIZE_MASK) >> X_D3DFORMAT_PSIZE_SHIFT);
                dwPitch  = dwWidth*4;
                dwBPP = 4;
            }
            else if(X_Format == 0x05 /* X_D3DFMT_R5G6B5 */ || X_Format == 0x04 /* X_D3DFMT_A4R4G4B4 */
                 || X_Format == 0x1D /* X_D3DFMT_LIN_A4R4G4B4 */ || X_Format == 0x02 /* X_D3DFMT_A1R5G5B5 */
                 || X_Format == 0x28 /* X_D3DFMT_G8B8 */)
            {
                bSwizzled = TRUE;

                // Swizzled 16 Bit
                dwWidth  = 1 << ((pPixelContainer->Format & X_D3DFORMAT_USIZE_MASK) >> X_D3DFORMAT_USIZE_SHIFT);
                dwHeight = 1 << ((pPixelContainer->Format & X_D3DFORMAT_VSIZE_MASK) >> X_D3DFORMAT_VSIZE_SHIFT);
                dwMipMapLevels = (pPixelContainer->Format & X_D3DFORMAT_MIPMAP_MASK) >> X_D3DFORMAT_MIPMAP_SHIFT;
                dwDepth  = 1;// HACK? 1 << ((pPixelContainer->Format & X_D3DFORMAT_PSIZE_MASK) >> X_D3DFORMAT_PSIZE_SHIFT);
                dwPitch  = dwWidth*2;
                dwBPP = 2;
            }
            else if(X_Format == 0x00 /* X_D3DFMT_L8 */ || X_Format == 0x0B /* X_D3DFMT_P8 */ || X_Format == 0x01 /* X_D3DFMT_AL8 */ || X_Format == 0x1A /* X_D3DFMT_A8L8 */)
            {
                bSwizzled = TRUE;

                // Swizzled 8 Bit
                dwWidth  = 1 << ((pPixelContainer->Format & X_D3DFORMAT_USIZE_MASK) >> X_D3DFORMAT_USIZE_SHIFT);
                dwHeight = 1 << ((pPixelContainer->Format & X_D3DFORMAT_VSIZE_MASK) >> X_D3DFORMAT_VSIZE_SHIFT);
                dwMipMapLevels = (pPixelContainer->Format & X_D3DFORMAT_MIPMAP_MASK) >> X_D3DFORMAT_MIPMAP_SHIFT;
                dwDepth  = 1;// HACK? 1 << ((pPixelContainer->Format & X_D3DFORMAT_PSIZE_MASK) >> X_D3DFORMAT_PSIZE_SHIFT);
                dwPitch  = dwWidth;
                dwBPP = 1;
            }
            else if(X_Format == 0x1E /* X_D3DFMT_LIN_X8R8G8B8 */ || X_Format == 0x12 /* X_D3DFORMAT_A8R8G8B8 */ || X_Format == 0x2E /* D3DFMT_LIN_D24S8 */)
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
                dwMipMapLevels = (pPixelContainer->Format & X_D3DFORMAT_MIPMAP_MASK) >> X_D3DFORMAT_MIPMAP_SHIFT;

                // D3DFMT_DXT2...D3DFMT_DXT5 : 128bits per block/per 16 texels
                dwCompressedSize = dwWidth*dwHeight;

                if(X_Format == 0x0C)    // D3DFMT_DXT1 : 64bits per block/per 16 texels
                    dwCompressedSize /= 2;

                dwBPP = 1;
            }
            else if(X_Format == 0x24 /* D3DFMT_YUY2 */)
            {
                // Linear 32 Bit
                dwWidth  = (pPixelContainer->Size & X_D3DSIZE_WIDTH_MASK) + 1;
                dwHeight = ((pPixelContainer->Size & X_D3DSIZE_HEIGHT_MASK) >> X_D3DSIZE_HEIGHT_SHIFT) + 1;
                dwPitch  = (((pPixelContainer->Size & X_D3DSIZE_PITCH_MASK) >> X_D3DSIZE_PITCH_SHIFT)+1)*64;
            }
            else
            {
                CxbxKrnlCleanup("0x%.08X is not a supported format!\n", X_Format);
            }

            if(X_Format == 0x24 /* X_D3DFMT_YUY2 */)
            {
                //
                // cache the overlay size
                //

                g_dwOverlayW = dwWidth;
                g_dwOverlayH = dwHeight;
                g_dwOverlayP = RoundUp(g_dwOverlayW, 64)*2;

                // 
                // create texture resource
                //

                DWORD dwSize = g_dwOverlayP*g_dwOverlayH;
                DWORD dwPtr = (DWORD)CxbxMalloc(dwSize + sizeof(DWORD));

                DWORD *pRefCount = (DWORD*)(dwPtr + dwSize);

                // initialize ref count
                *pRefCount = 1;

                // If YUY2 is not supported in hardware, we'll actually mark this as a special fake texture (set highest bit)
                pPixelContainer->Data = X_D3DRESOURCE_DATA_FLAG_SPECIAL | X_D3DRESOURCE_DATA_FLAG_YUVSURF;
                pPixelContainer->Lock = dwPtr;
                pPixelContainer->Format = 0x24;

                pPixelContainer->Size  = (g_dwOverlayW & X_D3DSIZE_WIDTH_MASK);
                pPixelContainer->Size |= (g_dwOverlayH << X_D3DSIZE_HEIGHT_SHIFT);
                pPixelContainer->Size |= (g_dwOverlayP << X_D3DSIZE_PITCH_SHIFT);
            }
            else
            {
                if(bSwizzled || bCompressed)
                {
                    uint32 w = dwWidth;
                    uint32 h = dwHeight;

                    for(uint32 v=0;v<dwMipMapLevels;v++)
                    {
                        if( ((1u << v) >= w) || ((1u << v) >= h))
                        {
                            dwMipMapLevels = v + 1;
                            break;
                        }
                    }
                }

                // create the happy little texture
                if(dwCommonType == X_D3DCOMMON_TYPE_SURFACE)
                {
                    hRet = g_pD3DDevice8->CreateImageSurface(dwWidth, dwHeight, Format, &pResource->EmuSurface8);

                    if(FAILED(hRet))
                        CxbxKrnlCleanup("CreateImageSurface Failed!");

                    DbgPrintf("EmuIDirect3DResource8_Register (0x%X) : Successfully Created ImageSurface (0x%.08X, 0x%.08X)\n", GetCurrentThreadId(), pResource, pResource->EmuSurface8);
                    DbgPrintf("EmuIDirect3DResource8_Register (0x%X) : Width : %d, Height : %d, Format : %d\n", GetCurrentThreadId(), dwWidth, dwHeight, Format);
                }
                else
                {
                    // TODO: HACK: Figure out why this is necessary!
                    // TODO: This is necessary for DXT1 textures at least (4x4 blocks minimum)
                    if(dwWidth < 4)
                    {
                        EmuWarning("Expanding texture width (%d->4)", dwWidth);
                        dwWidth = 4;
                    
                        dwMipMapLevels = 3;
                    }

                    if(dwHeight < 4)
                    {
                        EmuWarning("Expanding texture height (%d->4)", dwHeight);
                        dwHeight = 4;

                        dwMipMapLevels = 3;
                    }

                    if(bCubemap)
                    {
                        DbgPrintf("CreateCubeTexture(%d, %d, 0, %d, D3DPOOL_MANAGED, 0x%.08X)\n", dwWidth,
                            dwMipMapLevels, Format, &pResource->EmuTexture8);

                        hRet = g_pD3DDevice8->CreateCubeTexture
                        (
                            dwWidth, dwMipMapLevels, 0, Format,
                            D3DPOOL_MANAGED, &pResource->EmuCubeTexture8
                        );

                        if(FAILED(hRet))
                            CxbxKrnlCleanup("CreateCubeTexture Failed!");

                        DbgPrintf("EmuIDirect3DResource8_Register (0x%X) : Successfully Created CubeTexture (0x%.08X, 0x%.08X)\n", GetCurrentThreadId(), pResource, pResource->EmuCubeTexture8);
                    }
                    else
                    {
                        DbgPrintf("CreateTexture(%d, %d, %d, 0, %d, D3DPOOL_MANAGED, 0x%.08X)\n", dwWidth, dwHeight,
                            dwMipMapLevels, Format, &pResource->EmuTexture8);

                        hRet = g_pD3DDevice8->CreateTexture
                        (
                            dwWidth, dwHeight, dwMipMapLevels, 0, Format,
                            D3DPOOL_MANAGED, &pResource->EmuTexture8
                        );

                        if(FAILED(hRet))
                            CxbxKrnlCleanup("CreateTexture Failed!");

                        DbgPrintf("EmuIDirect3DResource8_Register (0x%X) : Successfully Created Texture (0x%.08X, 0x%.08X)\n", GetCurrentThreadId(), pResource, pResource->EmuTexture8);
                    }
                }

                uint32 stop = bCubemap ? 6 : 1;

                for(uint32 r=0;r<stop;r++)
                {
                    // as we iterate through mipmap levels, we'll adjust the source resource offset
                    DWORD dwCompressedOffset = 0;

                    DWORD dwMipOffs = 0;
                    DWORD dwMipWidth = dwWidth;
                    DWORD dwMipHeight = dwHeight;
                    DWORD dwMipPitch = dwPitch;

                    // iterate through the number of mipmap levels
                    for(uint level=0;level<dwMipMapLevels;level++)
                    {
                        D3DLOCKED_RECT LockedRect;

                        // copy over data (deswizzle if necessary)
                        if(dwCommonType == X_D3DCOMMON_TYPE_SURFACE)
                            hRet = pResource->EmuSurface8->LockRect(&LockedRect, NULL, 0);
                        else
                        {
                            if(bCubemap)
                            {
                                hRet = pResource->EmuCubeTexture8->LockRect((D3DCUBEMAP_FACES)r, 0, &LockedRect, NULL, 0);
                            }
                            else
                            {
                                hRet = pResource->EmuTexture8->LockRect(level, &LockedRect, NULL, 0);
                            }
                        }

                        RECT  iRect  = {0,0,0,0};
                        POINT iPoint = {0,0};

                        BYTE *pSrc = (BYTE*)pBase;

                        pThis->Data = (DWORD)pSrc;

                        if(( IsSpecialResource(pResource->Data) && (pResource->Data & X_D3DRESOURCE_DATA_FLAG_SURFACE))
                         ||( IsSpecialResource(pBase) && ((DWORD)pBase & X_D3DRESOURCE_DATA_FLAG_SURFACE)))
                        {
                            EmuWarning("Attempt to registered to another resource's data (eww!)");

                            // TODO: handle this horrible situation
                            BYTE *pDest = (BYTE*)LockedRect.pBits;
                            for(DWORD v=0;v<dwMipHeight;v++)
                            {
                                memset(pDest, 0, dwMipWidth*dwBPP);

                                pDest += LockedRect.Pitch;
                                pSrc  += dwMipPitch;
                            }
                        }
                        else
                        {
                            if(bSwizzled)
                            {
                                if((DWORD)pSrc == 0x80000000)
                                {
                                    // TODO: Fix or handle this situation..?
                                }
                                else
                                {
                                    XTL::EmuXGUnswizzleRect
                                    (
                                        pSrc + dwMipOffs, dwMipWidth, dwMipHeight, dwDepth, LockedRect.pBits, 
                                        LockedRect.Pitch, iRect, iPoint, dwBPP
                                    );
                                }
                            }
                            else if(bCompressed)
                            {
                                // NOTE: compressed size is (dwWidth/2)*(dwHeight/2)/2, so each level divides by 4

                                memcpy(LockedRect.pBits, pSrc + dwCompressedOffset, dwCompressedSize >> (level*2));

                                dwCompressedOffset += (dwCompressedSize >> (level*2));
                            }
                            else
                            {
                                BYTE *pDest = (BYTE*)LockedRect.pBits;

                                if((DWORD)LockedRect.Pitch == dwMipPitch && dwMipPitch == dwMipWidth*dwBPP)
                                {
                                    memcpy(pDest, pSrc + dwMipOffs, dwMipWidth*dwMipHeight*dwBPP);
                                }
                                else
                                {
                                    for(DWORD v=0;v<dwMipHeight;v++)
                                    {
                                        memcpy(pDest, pSrc + dwMipOffs, dwMipWidth*dwBPP);

                                        pDest += LockedRect.Pitch;
                                        pSrc  += dwMipPitch;
                                    }
                                }
                            }
                        }

                        if(dwCommonType == X_D3DCOMMON_TYPE_SURFACE)
                            pResource->EmuSurface8->UnlockRect();
                        else
                        {
                            if(bCubemap)
                                pResource->EmuCubeTexture8->UnlockRect((D3DCUBEMAP_FACES)r, 0);
                            else
                                pResource->EmuTexture8->UnlockRect(level);
                        }

                        dwMipOffs += dwMipWidth*dwMipHeight*dwBPP;

                        dwMipWidth /= 2;
                        dwMipHeight /= 2;
                        dwMipPitch /= 2;
                    }
                }

                // Debug Texture Dumping
                #ifdef _DEBUG_DUMP_TEXTURE_REGISTER
                if(dwCommonType == X_D3DCOMMON_TYPE_SURFACE)
                {
                    static int dwDumpSurface = 0;

                    char szBuffer[255];

                    sprintf(szBuffer, _DEBUG_DUMP_TEXTURE_REGISTER "%.03d-RegSurface%.03d.bmp", X_Format, dwDumpSurface++);

                    D3DXSaveSurfaceToFile(szBuffer, D3DXIFF_BMP, pResource->EmuSurface8, NULL, NULL);
                }
                else
                {
                    if(bCubemap)
                    {
                        static int dwDumpCube = 0;

                        char szBuffer[255];

                        for(int v=0;v<6;v++)
                        {
                            IDirect3DSurface8 *pSurface=0;

                            sprintf(szBuffer, _DEBUG_DUMP_TEXTURE_REGISTER "%.03d-RegCubeTex%.03d-%d.bmp", X_Format, dwDumpCube++, v);

                            pResource->EmuCubeTexture8->GetCubeMapSurface((D3DCUBEMAP_FACES)v, 0, &pSurface);

                            D3DXSaveSurfaceToFile(szBuffer, D3DXIFF_BMP, pSurface, NULL, NULL);
                        }
                    }
                    else
                    {
                        static int dwDumpTex = 0;

                        char szBuffer[255];

                        sprintf(szBuffer, _DEBUG_DUMP_TEXTURE_REGISTER "%.03d-RegTexture%.03d.bmp", X_Format, dwDumpTex++);

                        D3DXSaveTextureToFile(szBuffer, D3DXIFF_BMP, pResource->EmuTexture8, NULL);
                    }
                }
                #endif
            }
        }
        break;

        case X_D3DCOMMON_TYPE_FIXUP:
        {
            X_D3DFixup *pFixup = (X_D3DFixup*)pResource;

            CxbxKrnlCleanup("IDirect3DReosurce8::Register -> X_D3DCOMMON_TYPE_FIXUP is not yet supported\n"
            "0x%.08X (pFixup->Common) \n"
            "0x%.08X (pFixup->Data)   \n"
            "0x%.08X (pFixup->Lock)   \n"
            "0x%.08X (pFixup->Run)    \n"
            "0x%.08X (pFixup->Next)   \n"
            "0x%.08X (pFixup->Size)   \n", pFixup->Common, pFixup->Data, pFixup->Lock, pFixup->Run, pFixup->Next, pFixup->Size);
        }

        default:
            CxbxKrnlCleanup("IDirect3DResource8::Register -> Common Type 0x%.08X not yet supported", dwCommonType);
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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DResource8_AddRef\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis);

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DResource8_Release\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis);

    ULONG uRet = 0;

    if(IsSpecialResource(pThis->Data) && (pThis->Data & X_D3DRESOURCE_DATA_FLAG_YUVSURF))
    {
        DWORD  dwPtr = (DWORD)pThis->Lock;
        DWORD *pRefCount = (DWORD*)(dwPtr + g_dwOverlayP*g_dwOverlayH);

        if(--(*pRefCount) == 0)
        {
            // free memory associated with this special resource handle
            CxbxFree((PVOID)dwPtr);
        }

        EmuSwapFS();
        EmuIDirect3DDevice8_EnableOverlay(FALSE);
        EmuSwapFS();
    }
    else
    {
        IDirect3DResource8 *pResource8 = pThis->EmuResource8;

        if(pThis->Lock == 0x8000BEEF)
        {
            delete[] (PVOID)pThis->Data;
            uRet = --pThis->Lock;
        }
        else if(pResource8 != 0)
        {
            for(int v=0;v<16;v++)
            {
                if(pCache[v].Data == pThis->Data && pThis->Data != 0)
                {
                    pCache[v].Data = 0;
                    break;
                }
            }

            #ifdef _DEBUG_TRACE_VB
            D3DRESOURCETYPE Type = pResource8->GetType();
            #endif

            uRet = pResource8->Release();

            if(uRet == 0)
            {
                DbgPrintf("EmuIDirect3DResource8_Release (0x%X): Cleaned up a Resource!\n", GetCurrentThreadId());

                #ifdef _DEBUG_TRACE_VB
                if(Type == D3DRTYPE_VERTEXBUFFER)
                {
                    g_VBTrackTotal.remove(pResource8);
                    g_VBTrackDisable.remove(pResource8);
                }
                #endif

                //delete pThis;
            }
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

    /* too much output
    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DResource8_IsBusy\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis);
    //*/

    IDirect3DResource8 *pResource8 = pThis->EmuResource8;

    EmuSwapFS();   // XBox FS

    return FALSE;
}

// ******************************************************************
// * func: EmuIDirect3DResource8_GetType
// ******************************************************************
XTL::X_D3DRESOURCETYPE WINAPI XTL::EmuIDirect3DResource8_GetType
(
    X_D3DResource      *pThis
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DResource8_GetType\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis);

    // TODO: Handle situation where the resource type is >7
    D3DRESOURCETYPE rType = pThis->EmuResource8->GetType();

    EmuSwapFS();   // XBox FS

    return (X_D3DRESOURCETYPE)rType;
}

// ******************************************************************
// * func: EmuLock2DSurface
// ******************************************************************
VOID WINAPI XTL::EmuLock2DSurface
(
    X_D3DPixelContainer *pPixelContainer,
    D3DCUBEMAP_FACES     FaceType,
    UINT                 Level,
    D3DLOCKED_RECT      *pLockedRect,
    RECT                *pRect,
    DWORD                Flags
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuLock2DSurface\n"
           "(\n"
           "   pPixelContainer     : 0x%.08X\n"
           "   FaceType            : 0x%.08X\n"
           "   Level               : 0x%.08X\n"
           "   pLockedRect         : 0x%.08X\n"
           "   pRect               : 0x%.08X\n"
           "   Flags               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pPixelContainer, FaceType, Level, pLockedRect, pRect, Flags);

    EmuVerifyResourceIsRegistered(pPixelContainer);

    HRESULT hRet = pPixelContainer->EmuCubeTexture8->LockRect(FaceType, Level, pLockedRect, pRect, Flags);

    EmuSwapFS();   // XBox FS

    return;
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

    DbgPrintf("EmuD3D8 (0x%X): EmuGet2DSurfaceDesc\n"
           "(\n"
           "   pPixelContainer     : 0x%.08X\n"
           "   dwLevel             : 0x%.08X\n"
           "   pDesc               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pPixelContainer, dwLevel, pDesc);

    EmuVerifyResourceIsRegistered(pPixelContainer);

    D3DSURFACE_DESC SurfaceDesc;

    ZeroMemory(&SurfaceDesc, sizeof(SurfaceDesc));

    HRESULT hRet;

    if(dwLevel == 0xFEFEFEFE)
    {
        hRet = pPixelContainer->EmuSurface8->GetDesc(&SurfaceDesc);

        /*
        static int dwDumpSurface = 0;

        char szBuffer[255];

        sprintf(szBuffer, "C:\\Aaron\\Textures\\Surface%.03d.bmp", dwDumpSurface++);

        D3DXSaveSurfaceToFile(szBuffer, D3DXIFF_BMP, pPixelContainer->EmuSurface8, NULL, NULL);
        */
    }
    else
    {
        hRet = pPixelContainer->EmuTexture8->GetLevelDesc(dwLevel, &SurfaceDesc);

        /*
        static int dwDumpTexture = 0;

        char szBuffer[255];

        sprintf(szBuffer, "C:\\Aaron\\Textures\\GetDescTexture%.03d.bmp", dwDumpTexture++);

        D3DXSaveTextureToFile(szBuffer, D3DXIFF_BMP, pPixelContainer->EmuTexture8, NULL);
        */
    }

    // rearrange into xbox format (remove D3DPOOL)
    {
        // Convert Format (PC->Xbox)
        pDesc->Format = EmuPC2XB_D3DFormat(SurfaceDesc.Format);
        pDesc->Type   = (X_D3DRESOURCETYPE)SurfaceDesc.Type;

        if(pDesc->Type > 7)
            CxbxKrnlCleanup("EmuGet2DSurfaceDesc: pDesc->Type > 7");

        pDesc->Usage  = SurfaceDesc.Usage;
        pDesc->Size   = SurfaceDesc.Size;

        // TODO: Convert from Xbox to PC!!
        if(SurfaceDesc.MultiSampleType == D3DMULTISAMPLE_NONE)
            pDesc->MultiSampleType = (XTL::D3DMULTISAMPLE_TYPE)0x0011;
        else
            CxbxKrnlCleanup("EmuGet2DSurfaceDesc Unknown Multisample format! (%d)", SurfaceDesc.MultiSampleType);

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
    // debug trace
    #ifdef _DEBUG_TRACE
    {
        EmuSwapFS();   // Win2k/XP FS
        DbgPrintf("EmuD3D8 (0x%X): EmuGet2DSurfaceDescD\n"
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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DSurface8_GetDesc\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
           "   pDesc               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis, pDesc);

    HRESULT hRet;

    EmuVerifyResourceIsRegistered(pThis);

    if(IsSpecialResource(pThis->Data) && (pThis->Data & X_D3DRESOURCE_DATA_FLAG_YUVSURF))
    {
        pDesc->Format = EmuPC2XB_D3DFormat(D3DFMT_YUY2);
        pDesc->Height = g_dwOverlayH;
        pDesc->Width  = g_dwOverlayW;
        pDesc->MultiSampleType = (D3DMULTISAMPLE_TYPE)0;
        pDesc->Size   = g_dwOverlayP*g_dwOverlayH;
        pDesc->Type   = X_D3DRTYPE_SURFACE;
        pDesc->Usage  = 0;

        hRet = D3D_OK;
    }
    else
    {
        IDirect3DSurface8 *pSurface8 = pThis->EmuSurface8;

        D3DSURFACE_DESC SurfaceDesc;

        hRet = pSurface8->GetDesc(&SurfaceDesc);

        // rearrange into windows format (remove D3DPool)
        {
            // Convert Format (PC->Xbox)
            pDesc->Format = EmuPC2XB_D3DFormat(SurfaceDesc.Format);
            pDesc->Type   = (X_D3DRESOURCETYPE)SurfaceDesc.Type;

            if(pDesc->Type > 7)
                CxbxKrnlCleanup("EmuIDirect3DSurface8_GetDesc: pDesc->Type > 7");

            pDesc->Usage  = SurfaceDesc.Usage;
            pDesc->Size   = SurfaceDesc.Size;

            // TODO: Convert from Xbox to PC!!
            if(SurfaceDesc.MultiSampleType == D3DMULTISAMPLE_NONE)
                pDesc->MultiSampleType = (XTL::D3DMULTISAMPLE_TYPE)0x0011;
            else
                CxbxKrnlCleanup("EmuIDirect3DSurface8_GetDesc Unknown Multisample format! (%d)", SurfaceDesc.MultiSampleType);

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DSurface8_LockRect\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
           "   pLockedRect         : 0x%.08X\n"
           "   pRect               : 0x%.08X\n"
           "   Flags               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis, pLockedRect, pRect, Flags);

    HRESULT hRet;

    EmuVerifyResourceIsRegistered(pThis);

    if(IsSpecialResource(pThis->Data) && (pThis->Data & X_D3DRESOURCE_DATA_FLAG_YUVSURF))
    {
        pLockedRect->Pitch = g_dwOverlayP;
        pLockedRect->pBits = (PVOID)pThis->Lock;

        hRet = D3D_OK;
    }
    else
    {
        if(Flags & 0x40)
            EmuWarning("D3DLOCK_TILED ignored!");

        IDirect3DSurface8 *pSurface8 = pThis->EmuSurface8;

        DWORD NewFlags = 0;

        if(Flags & 0x80)
            NewFlags |= D3DLOCK_READONLY;

        if(Flags & !(0x80 | 0x40))
            CxbxKrnlCleanup("EmuIDirect3DSurface8_LockRect: Unknown Flags! (0x%.08X)", Flags);

        // Remove old lock(s)
        pSurface8->UnlockRect();

        hRet = pSurface8->LockRect(pLockedRect, pRect, NewFlags);

        if(FAILED(hRet))
            EmuWarning("LockRect Failed!");
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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DBaseTexture8_GetLevelCount\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis);

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
    if(IsSpecialResource(pThis->Data) && (pThis->Data & X_D3DRESOURCE_DATA_FLAG_YUVSURF))
    {
        DWORD dwSize = g_dwOverlayP*g_dwOverlayH;

        DWORD *pRefCount = (DWORD*)((DWORD)pThis->Lock + dwSize);

        // initialize ref count
        (*pRefCount)++;

        return pThis;
    }

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DTexture8_LockRect\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
           "   Level               : 0x%.08X\n"
           "   pLockedRect         : 0x%.08X\n"
           "   pRect               : 0x%.08X\n"
           "   Flags               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis, Level, pLockedRect, pRect, Flags);

    HRESULT hRet;

    EmuVerifyResourceIsRegistered(pThis);

    // check if we have an unregistered YUV2 resource
    if( (pThis != 0) && IsSpecialResource(pThis->Data) && (pThis->Data & X_D3DRESOURCE_DATA_FLAG_YUVSURF))
    {
        pLockedRect->Pitch = g_dwOverlayP;
        pLockedRect->pBits = (PVOID)pThis->Lock;

        hRet = D3D_OK;
    }
    else
    {
        IDirect3DTexture8 *pTexture8 = pThis->EmuTexture8;

        DWORD NewFlags = 0;

        if(Flags & 0x80)
            NewFlags |= D3DLOCK_READONLY;

        if(Flags & !(0x80 | 0x40))
            CxbxKrnlCleanup("EmuIDirect3DTexture8_LockRect: Unknown Flags! (0x%.08X)", Flags);

        // Remove old lock(s)
        pTexture8->UnlockRect(Level);

        hRet = pTexture8->LockRect(Level, pLockedRect, pRect, NewFlags);

        pThis->Common |= X_D3DCOMMON_ISLOCKED;
    }

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DTexture8_GetSurfaceLevel\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
           "   Level               : 0x%.08X\n"
           "   ppSurfaceLevel      : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis, Level, ppSurfaceLevel);

    HRESULT hRet;

    EmuVerifyResourceIsRegistered(pThis);

    // if highest bit is set, this is actually a raw memory pointer (for YUY2 simulation)
    if(IsSpecialResource(pThis->Data) && (pThis->Data & X_D3DRESOURCE_DATA_FLAG_YUVSURF))
    {
        DWORD dwSize = g_dwOverlayP*g_dwOverlayH;

        DWORD *pRefCount = (DWORD*)((DWORD)pThis->Lock + dwSize);

        // initialize ref count
        (*pRefCount)++;

        *ppSurfaceLevel = (X_D3DSurface*)pThis;

        hRet = D3D_OK;
    }
    else
    {
        IDirect3DTexture8 *pTexture8 = pThis->EmuTexture8;

        *ppSurfaceLevel = new X_D3DSurface();

        (*ppSurfaceLevel)->Data = 0xB00BBABE;
        (*ppSurfaceLevel)->Common = 0;
        (*ppSurfaceLevel)->Format = 0;
        (*ppSurfaceLevel)->Size = 0;

        hRet = pTexture8->GetSurfaceLevel(Level, &((*ppSurfaceLevel)->EmuSurface8));

        if(FAILED(hRet))
        {
            EmuWarning("EmuIDirect3DTexture8_GetSurfaceLevel Failed!\n");
        }
        else
        {
            DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DTexture8_GetSurfaceLevel := 0x%.08X\n", GetCurrentThreadId(), (*ppSurfaceLevel)->EmuSurface8);
        }
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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DVolumeTexture8_LockBox\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
           "   Level               : 0x%.08X\n"
           "   pLockedVolume       : 0x%.08X\n"
           "   pBox                : 0x%.08X\n"
           "   Flags               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis, Level, pLockedVolume, pBox, Flags);

    EmuVerifyResourceIsRegistered(pThis);

    IDirect3DVolumeTexture8 *pVolumeTexture8 = pThis->EmuVolumeTexture8;

    HRESULT hRet = pVolumeTexture8->LockBox(Level, pLockedVolume, pBox, Flags);

    if(FAILED(hRet))
        EmuWarning("LockBox Failed!");

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DCubeTexture8_LockRect\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
           "   FaceType            : 0x%.08X\n"
           "   Level               : 0x%.08X\n"
           "   pLockedBox          : 0x%.08X\n"
           "   pRect               : 0x%.08X\n"
           "   Flags               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis, FaceType, Level, pLockedBox, pRect, Flags);

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_Release();\n", GetCurrentThreadId());

    g_pD3DDevice8->AddRef();
    DWORD RefCount = g_pD3DDevice8->Release();
    if (RefCount == 1)
    {
        // Signal proxy thread, and wait for completion
        g_EmuCDPD.bReady = true;
        g_EmuCDPD.bCreate = false;

        while(g_EmuCDPD.bReady)
            Sleep(10);
        RefCount = g_EmuCDPD.hRet;
    }
    else
    {
        RefCount = g_pD3DDevice8->Release();
    }

    EmuSwapFS();   // XBox FS

    return RefCount;
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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_CreateVertexBuffer2\n"
           "(\n"
           "   Length              : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Length);

    X_D3DVertexBuffer *pD3DVertexBuffer = new X_D3DVertexBuffer();

    HRESULT hRet = g_pD3DDevice8->CreateVertexBuffer
    (
        Length, 
        0,
        0,
        D3DPOOL_MANAGED, 
        &pD3DVertexBuffer->EmuVertexBuffer8
    );

    if(FAILED(hRet))
        EmuWarning("CreateVertexBuffer Failed!");

    #ifdef _DEBUG_TRACK_VB
    g_VBTrackTotal.insert(pD3DVertexBuffer->EmuVertexBuffer8);
    #endif

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_EnableOverlay\n"
           "(\n"
           "   Enable              : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Enable);

    if(Enable == FALSE && (g_pDDSOverlay7 != NULL))
    {
        g_pDDSOverlay7->UpdateOverlay(NULL, g_pDDSPrimary, NULL, DDOVER_HIDE, 0);

        // cleanup overlay clipper
        if(g_pDDClipper != 0)
        {
            g_pDDClipper->Release();
            g_pDDClipper = 0;
        }

        // cleanup overlay surface
        if(g_pDDSOverlay7 != 0)
        {
            g_pDDSOverlay7->Release();
            g_pDDSOverlay7 = 0;
        }
    }
    else if(Enable == TRUE && (g_pDDSOverlay7 == 0))
    {
        // initialize overlay surface
        if(g_bSupportsYUY2)
        {
            XTL::DDSURFACEDESC2 ddsd2;

            ZeroMemory(&ddsd2, sizeof(ddsd2));

            ddsd2.dwSize = sizeof(ddsd2);
            ddsd2.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
            ddsd2.ddsCaps.dwCaps = DDSCAPS_OVERLAY;
            ddsd2.dwWidth = g_dwOverlayW;
            ddsd2.dwHeight = g_dwOverlayH; 
            ddsd2.ddpfPixelFormat.dwSize = sizeof(XTL::DDPIXELFORMAT);
            ddsd2.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
            ddsd2.ddpfPixelFormat.dwFourCC = MAKEFOURCC('Y','U','Y','2');

            HRESULT hRet = g_pDD7->CreateSurface(&ddsd2, &g_pDDSOverlay7, NULL);

            if(FAILED(hRet))
                CxbxKrnlCleanup("Could not create overlay surface");

            hRet = g_pDD7->CreateClipper(0, &g_pDDClipper, NULL);

            if(FAILED(hRet))
                CxbxKrnlCleanup("Could not create overlay clipper");

            hRet = g_pDDClipper->SetHWnd(0, g_hEmuWindow);
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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_UpdateOverlay\n"
           "(\n"
           "   pSurface            : 0x%.08X\n"
           "   SrcRect             : 0x%.08X\n"
           "   DstRect             : 0x%.08X\n"
           "   EnableColorKey      : 0x%.08X\n"
           "   ColorKey            : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pSurface, SrcRect, DstRect, EnableColorKey, ColorKey);

    // manually copy data over to overlay
    if(g_bSupportsYUY2)
    {
        DDSURFACEDESC2  ddsd2;

        ZeroMemory(&ddsd2, sizeof(ddsd2));

        ddsd2.dwSize = sizeof(ddsd2);

        g_pDDSOverlay7->Lock(NULL, &ddsd2, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);

        // copy data
        {
            char *pDest = (char*)ddsd2.lpSurface;
            char *pSour = (char*)pSurface->Lock;

            int w = g_dwOverlayW;
            int h = g_dwOverlayH;

            // TODO: sucker the game into rendering directly to the overlay (speed boost)
            if( (ddsd2.lPitch == w*2) && ((int)g_dwOverlayP == w*2) )
                memcpy(pDest, pSour, h*w*2);
            else
            {
                for(int y=0;y<h;y++)
                {
                    memcpy(pDest, pSour, w*2);

                    pDest += ddsd2.lPitch;
                    pSour += g_dwOverlayP;
                }
            }
        }

        g_pDDSOverlay7->Unlock(NULL);
    }

    // update overlay!
    if(g_bSupportsYUY2)
    {
        RECT SourRect = {0, 0, g_dwOverlayW, g_dwOverlayH}, DestRect;
        MONITORINFO MonitorInfo = {0};

        int nTitleHeight  = 0;//GetSystemMetrics(SM_CYCAPTION);
        int nBorderWidth  = 0;//GetSystemMetrics(SM_CXSIZEFRAME);
        int nBorderHeight = 0;//GetSystemMetrics(SM_CYSIZEFRAME);

        MonitorInfo.cbSize = sizeof(MONITORINFO);
        GetMonitorInfo(g_hMonitor, &MonitorInfo);

        GetWindowRect(g_hEmuWindow, &DestRect);

        DestRect.left   += nBorderWidth;
        DestRect.right  -= nBorderWidth;
        DestRect.top    += nTitleHeight + nBorderHeight;
        DestRect.bottom -= nBorderHeight;

        DestRect.left   -= MonitorInfo.rcMonitor.left;
        DestRect.right  -= MonitorInfo.rcMonitor.left;
        DestRect.top    -= MonitorInfo.rcMonitor.top;
        DestRect.bottom -= MonitorInfo.rcMonitor.top;

        DDOVERLAYFX ddofx;

        ZeroMemory(&ddofx, sizeof(ddofx));

        ddofx.dwSize = sizeof(DDOVERLAYFX);
        ddofx.dckDestColorkey.dwColorSpaceLowValue = 0;
        ddofx.dckDestColorkey.dwColorSpaceHighValue = 0;

        HRESULT hRet = g_pDDSOverlay7->UpdateOverlay(&SourRect, g_pDDSPrimary, &DestRect, /*DDOVER_KEYDESTOVERRIDE | */DDOVER_SHOW, /*&ddofx*/0);
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
            uint08 *pCurByte = (uint08*)pSurface->Lock;

            uint08 *pDest = (uint08*)LockedRectDest.pBits;

            uint32 dx=0, dy=0;

            uint32 dwImageSize = g_dwOverlayP*g_dwOverlayH;

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetOverlayUpdateStatus();\n",
           GetCurrentThreadId());

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_BlockUntilVerticalBlank();\n",
           GetCurrentThreadId());

    // segaGT tends to freeze with this on
//    if(g_XBVideo.GetVSync())
        g_pDD7->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, 0);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVerticalBlankCallback
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetVerticalBlankCallback
(
    D3DVBLANKCALLBACK pCallback
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetVerticalBlankCallback\n"
           "(\n"
           "   pCallback           : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pCallback);

    g_pVBCallback = pCallback;

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetTextureState_TexCoordIndex\n"
           "(\n"
           "   Stage               : 0x%.08X\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Stage, Value);

    if(Value > 0x00030000)
        CxbxKrnlCleanup("EmuIDirect3DDevice8_SetTextureState_TexCoordIndex: Unknown TexCoordIndex Value (0x%.08X)", Value);

    g_pD3DDevice8->SetTextureStageState(Stage, D3DTSS_TEXCOORDINDEX, Value);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTextureState_TwoSidedLighting
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetTextureState_TwoSidedLighting
(
    DWORD Value
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetTextureState_TwoSidedLighting\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

    EmuWarning("TwoSidedLighting is not supported!");

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTextureState_BackFillMode
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetTextureState_BackFillMode
(
    DWORD Value
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetTextureState_BackFillMode\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

    EmuWarning("BackFillMode is not supported!");

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTextureState_BorderColor
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetTextureState_BorderColor
(
    DWORD Stage,
    DWORD Value
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetTextureState_BorderColor\n"
           "(\n"
           "   Stage               : 0x%.08X\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Stage, Value);

    g_pD3DDevice8->SetTextureStageState(Stage, D3DTSS_BORDERCOLOR, Value);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTextureState_ColorKeyColor
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetTextureState_ColorKeyColor
(
    DWORD Stage,
    DWORD Value
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetTextureState_ColorKeyColor\n"
           "(\n"
           "   Stage               : 0x%.08X\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Stage, Value);

    EmuWarning("SetTextureState_ColorKeyColor is not supported!");

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTextureState_BumpEnv
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetTextureState_BumpEnv
(
    DWORD                      Stage,
    X_D3DTEXTURESTAGESTATETYPE Type,
    DWORD                      Value
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetTextureState_BumpEnv\n"
           "(\n"
           "   Stage               : 0x%.08X\n"
           "   Type                : 0x%.08X\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Stage, Type, Value);

    switch(Type)
    {
        case 22:    // X_D3DTSS_BUMPENVMAT00
            g_pD3DDevice8->SetTextureStageState(Stage, D3DTSS_BUMPENVMAT00, Value);
            break;
        case 23:    // X_D3DTSS_BUMPENVMAT01
            g_pD3DDevice8->SetTextureStageState(Stage, D3DTSS_BUMPENVMAT01, Value);
            break;
        case 24:    // X_D3DTSS_BUMPENVMAT11
            g_pD3DDevice8->SetTextureStageState(Stage, D3DTSS_BUMPENVMAT11, Value);
            break;
        case 25:    // X_D3DTSS_BUMPENVMAT10
            g_pD3DDevice8->SetTextureStageState(Stage, D3DTSS_BUMPENVMAT10, Value);
            break;
        case 26:    // X_D3DTSS_BUMPENVLSCALE
            g_pD3DDevice8->SetTextureStageState(Stage, D3DTSS_BUMPENVLSCALE, Value);
            break;
    }

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_FrontFace
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetRenderState_FrontFace
(
    DWORD Value
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_FrontFace\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

    EmuWarning("SetRenderState_FrontFace not supported!\n");

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_LogicOp
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetRenderState_LogicOp
(
    DWORD Value
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_LogicOp\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

    EmuWarning("SetRenderState_LogicOp is not supported!");

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_NormalizeNormals\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_TextureFactor\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_ZBias\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_EdgeAntiAlias\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

//  TODO: Analyze performance and compatibility (undefined behavior on PC with triangles or points)
//  g_pD3DDevice8->SetRenderState(D3DRS_EDGEANTIALIAS, Value);

//    EmuWarning("SetRenderState_EdgeAntiAlias not implemented!");

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_FillMode\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

    DWORD dwFillMode;

    if(g_iWireframe == 0)
        dwFillMode = EmuXB2PC_D3DFILLMODE(Value);
    else if(g_iWireframe == 1)
        dwFillMode = D3DFILL_WIREFRAME;
    else
        dwFillMode = D3DFILL_POINT;

    g_pD3DDevice8->SetRenderState(D3DRS_FILLMODE, dwFillMode);

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_FogColor\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_Dxt1NoiseEnable\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

    EmuWarning("SetRenderState_Dxt1NoiseEnable not implemented!");

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_Simple\n"
           "(\n"
           "   Method              : 0x%.08X\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Method, Value);

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
        EmuWarning("RenderState_Simple(0x%.08X, 0x%.08X) is unsupported!", Method, Value);
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

                DbgPrintf("D3DRS_COLORWRITEENABLE := 0x%.08X\n", Value);
            }
            break;

            case D3DRS_SHADEMODE:
                Value = Value & 0x03;
                DbgPrintf("D3DRS_SHADEMODE := 0x%.08X\n", Value);
                break;

            case D3DRS_BLENDOP:
                Value = EmuXB2PC_D3DBLENDOP(Value);
                DbgPrintf("D3DRS_BLENDOP := 0x%.08X\n", Value);
                break;

            case D3DRS_SRCBLEND:
                Value = EmuXB2PC_D3DBLEND(Value);
                DbgPrintf("D3DRS_SRCBLEND := 0x%.08X\n", Value);
                break;

            case D3DRS_DESTBLEND:
                Value = EmuXB2PC_D3DBLEND(Value);
                DbgPrintf("D3DRS_DESTBLEND := 0x%.08X\n", Value);
                break;

            case D3DRS_ZFUNC:
                Value = EmuXB2PC_D3DCMPFUNC(Value);
                DbgPrintf("D3DRS_ZFUNC := 0x%.08X\n", Value);
                break;

            case D3DRS_ALPHAFUNC:
                Value = EmuXB2PC_D3DCMPFUNC(Value);
                DbgPrintf("D3DRS_ALPHAFUNC := 0x%.08X\n", Value);
                break;

            case D3DRS_ALPHATESTENABLE:
                DbgPrintf("D3DRS_ALPHATESTENABLE := 0x%.08X\n", Value);
                break;

            case D3DRS_ALPHABLENDENABLE:
                DbgPrintf("D3DRS_ALPHABLENDENABLE := 0x%.08X\n", Value);
                break;

            case D3DRS_ALPHAREF:
                DbgPrintf("D3DRS_ALPHAREF := %lf\n", DWtoF(Value));
                break;

            case D3DRS_ZWRITEENABLE:
                DbgPrintf("D3DRS_ZWRITEENABLE := 0x%.08X\n", Value);
                break;

            case D3DRS_DITHERENABLE:
                DbgPrintf("D3DRS_DITHERENABLE := 0x%.08X\n", Value);
                break;

            default:
                CxbxKrnlCleanup("Unsupported RenderState (0x%.08X)", State);
                break;
        };

        // TODO: verify these params as you add support for them!
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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_VertexBlend\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

    // convert from Xbox direct3d to PC direct3d enumeration
    if(Value <= 1)
        Value = Value;
    else if(Value == 3)
        Value = 2;
    else if(Value == 5)
        Value = 3;
    else
        CxbxKrnlCleanup("Unsupported D3DVERTEXBLENDFLAGS (%d)", Value);

    g_pD3DDevice8->SetRenderState(D3DRS_VERTEXBLEND, Value);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_PSTextureModes
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetRenderState_PSTextureModes
(
    DWORD Value
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_PSTextureModes\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

    // TODO: do something..

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_CullMode\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

    // convert from Xbox D3D to PC D3D enumeration
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
            CxbxKrnlCleanup("EmuIDirect3DDevice8_SetRenderState_CullMode: Unknown Cullmode (%d)", Value);
    }

    g_pD3DDevice8->SetRenderState(D3DRS_CULLMODE, Value);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_LineWidth
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetRenderState_LineWidth
(
    DWORD Value
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_LineWidth\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

    // TODO: Convert to PC format??
//    g_pD3DDevice8->SetRenderState(D3DRS_LINEPATTERN, Value);
    EmuWarning("SetRenderState_LineWidth is not supported!");

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_StencilFail
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetRenderState_StencilFail
(
    DWORD Value
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_StencilFail\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

    g_pD3DDevice8->SetRenderState(D3DRS_STENCILFAIL, Value);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_OcclusionCullEnable
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetRenderState_OcclusionCullEnable
(
    DWORD Value
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_OcclusionCullEnable\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

    EmuWarning("SetRenderState_OcclusionCullEnable not supported!");

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_StencilCullEnable
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetRenderState_StencilCullEnable
(
    DWORD Value
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_StencilCullEnable\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

    EmuWarning("SetRenderState_StencilCullEnable not supported!");

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead
(
    DWORD Value
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

    EmuWarning("SetRenderState_RopZCmpAlwaysRead not supported!");

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_RopZRead
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetRenderState_RopZRead
(
    DWORD Value
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_RopZRead\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

    EmuWarning("SetRenderState_RopZRead not supported!");

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_DoNotCullUncompressed
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetRenderState_DoNotCullUncompressed
(
    DWORD Value
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_DoNotCullUncompressed\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

    EmuWarning("SetRenderState_DoNotCullUncompressed not supported!");

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_ZEnable\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_StencilEnable\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

    g_pD3DDevice8->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, Value);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_MultiSampleMask
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleMask
(
    DWORD Value
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_MultiSampleMask\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

    g_pD3DDevice8->SetRenderState(D3DRS_MULTISAMPLEMASK, Value);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_MultiSampleMode
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleMode
(
    DWORD Value
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_MultiSampleMode\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

    EmuWarning("SetRenderState_MultiSampleMode is not supported!");

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode
(
    DWORD Value
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

    EmuWarning("SetRenderState_MultiSampleRenderTargetMode is not supported!");

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_ShadowFunc\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

    // this warning just gets annoying
//    EmuWarning("ShadowFunc not implemented");

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderState_YuvEnable\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Value);

    EmuWarning("YuvEnable not implemented (0x%.08X)", Value);

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetTransform\n"
           "(\n"
           "   State               : 0x%.08X\n"
           "   pMatrix             : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), State, pMatrix);

    /*
    printf("pMatrix (%d)\n", State);
    printf("{\n");
    printf("    %.08f,%.08f,%.08f,%.08f\n", pMatrix->_11, pMatrix->_12, pMatrix->_13, pMatrix->_14);
    printf("    %.08f,%.08f,%.08f,%.08f\n", pMatrix->_21, pMatrix->_22, pMatrix->_23, pMatrix->_24);
    printf("    %.08f,%.08f,%.08f,%.08f\n", pMatrix->_31, pMatrix->_32, pMatrix->_33, pMatrix->_34);
    printf("    %.08f,%.08f,%.08f,%.08f\n", pMatrix->_41, pMatrix->_42, pMatrix->_43, pMatrix->_44);
    printf("}\n");

    if(State == 6 && (pMatrix->_11 == 1.0f) && (pMatrix->_22 == 1.0f) && (pMatrix->_33 == 1.0f) && (pMatrix->_44 == 1.0f))
    {
        g_bSkipPush = TRUE;
        printf("SkipPush ON\n");
    }
    else
    {
        g_bSkipPush = FALSE;
        printf("SkipPush OFF\n");
    }
    */

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetTransform\n"
           "(\n"
           "   State               : 0x%.08X\n"
           "   pMatrix             : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), State, pMatrix);

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DVertexBuffer8_Lock\n"
           "(\n"
           "   ppVertexBuffer      : 0x%.08X\n"
           "   OffsetToLock        : 0x%.08X\n"
           "   SizeToLock          : 0x%.08X\n"
           "   ppbData             : 0x%.08X\n"
           "   Flags               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), ppVertexBuffer, OffsetToLock, SizeToLock, ppbData, Flags);

    IDirect3DVertexBuffer8 *pVertexBuffer8 = ppVertexBuffer->EmuVertexBuffer8;

    HRESULT hRet = pVertexBuffer8->Lock(OffsetToLock, SizeToLock, ppbData, Flags);

    if(FAILED(hRet))
        EmuWarning("VertexBuffer Lock Failed!");

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DVertexBuffer8_Lock2\n"
           "(\n"
           "   ppVertexBuffer      : 0x%.08X\n"
           "   Flags               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), ppVertexBuffer, Flags);

    IDirect3DVertexBuffer8 *pVertexBuffer8 = ppVertexBuffer->EmuVertexBuffer8;

    BYTE *pbData = NULL;

    HRESULT hRet = pVertexBuffer8->Lock(0, 0, &pbData, Flags);

    EmuSwapFS();   // XBox FS

    return pbData;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetStreamSource2
// ******************************************************************
XTL::X_D3DVertexBuffer* WINAPI XTL::EmuIDirect3DDevice8_GetStreamSource2
(
    UINT  StreamNumber,
    UINT *pStride
)
{
    EmuSwapFS();

    // debug trace
    DbgPrintf( "EmuD3D8 (0x%.08X): EmuIDirect3DDevice8_GetStreamSource2\n"
               "(\n"
               "   StreamNumber               : 0x%.08X\n"
               "   pStride                    : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), StreamNumber, pStride);

    EmuWarning("Not correctly implemented yet!");
    X_D3DVertexBuffer* pVertexBuffer;
    g_pD3DDevice8->GetStreamSource(StreamNumber, (struct XTL::IDirect3DVertexBuffer8 **)&pVertexBuffer, pStride);

    EmuSwapFS();
    return pVertexBuffer;
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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetStreamSource\n"
           "(\n"
           "   StreamNumber        : 0x%.08X\n"
           "   pStreamData         : 0x%.08X (0x%.08X)\n"
           "   Stride              : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), StreamNumber, pStreamData, (pStreamData != 0) ? pStreamData->EmuVertexBuffer8 : 0, Stride);

    if(StreamNumber == 0)
        g_pVertexBuffer = pStreamData;

    IDirect3DVertexBuffer8 *pVertexBuffer8 = NULL;

    if(pStreamData != NULL)
    {
        EmuVerifyResourceIsRegistered(pStreamData);

        pVertexBuffer8 = pStreamData->EmuVertexBuffer8;
        pVertexBuffer8->Unlock();
    }

    #ifdef _DEBUG_TRACK_VB
    if(pStreamData != NULL)
    {
        g_bVBSkipStream = g_VBTrackDisable.exists(pStreamData->EmuVertexBuffer8);
    }
    #endif

    HRESULT hRet = g_pD3DDevice8->SetStreamSource(StreamNumber, pVertexBuffer8, Stride);

    if(FAILED(hRet))
        CxbxKrnlCleanup("SetStreamSource Failed!");

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetVertexShader\n"
           "(\n"
           "   Handle              : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Handle);
 
    HRESULT hRet = D3D_OK;

    g_CurrentVertexShader = Handle;

    XTL::D3DXVECTOR4 vOffset;
    XTL::D3DXVECTOR4 vScale;

    EmuSwapFS();
    EmuIDirect3DDevice8_GetViewportOffsetAndScale(&vOffset, &vScale);
    EmuSwapFS();

    if(g_VertexShaderConstantMode != X_VSCM_NONERESERVED)
    {
        //g_pD3DDevice8->SetVertexShaderConstant( 58, &vScale, 1 );
        //g_pD3DDevice8->SetVertexShaderConstant( 59, &vOffset, 1 );
    }

    DWORD RealHandle;
    if(VshHandleIsVertexShader(Handle))
    {
        RealHandle = ((VERTEX_SHADER *)(VshHandleGetVertexShader(Handle))->Handle)->Handle;
    }
    else
    {
        RealHandle = Handle;
    }
    hRet = g_pD3DDevice8->SetVertexShader(RealHandle);

    EmuSwapFS();   // XBox FS

    return;
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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_DrawVertices\n"
           "(\n"
           "   PrimitiveType       : 0x%.08X\n"
           "   StartVertex         : 0x%.08X\n"
           "   VertexCount         : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), PrimitiveType, StartVertex, VertexCount);

    EmuUpdateDeferredStates();

    if( (PrimitiveType == X_D3DPT_QUADSTRIP) || (PrimitiveType == X_D3DPT_POLYGON) )
        EmuWarning("Unsupported PrimitiveType! (%d)", (DWORD)PrimitiveType);

    UINT PrimitiveCount = EmuD3DVertex2PrimitiveCount(PrimitiveType, VertexCount);

    // Convert from Xbox to PC enumeration
    D3DPRIMITIVETYPE PCPrimitiveType = EmuPrimitiveType(PrimitiveType);

    VertexPatchDesc VPDesc;

    VPDesc.dwVertexCount = VertexCount;
    VPDesc.PrimitiveType = PrimitiveType;
    VPDesc.dwPrimitiveCount = PrimitiveCount;
    VPDesc.dwOffset = StartVertex;
    VPDesc.pVertexStreamZeroData = 0;
    VPDesc.uiVertexStreamZeroStride = 0;
    VPDesc.hVertexShader = g_CurrentVertexShader;

    VertexPatcher VertPatch;

    bool bPatched = VertPatch.Apply(&VPDesc);

    if(IsValidCurrentShader())
    {
        #ifdef _DEBUG_TRACK_VB
        if(g_bVBSkipStream)
        {
            g_pD3DDevice8->DrawPrimitive
            (
                PCPrimitiveType,
                StartVertex,
                0
            );
        }
        else
        {
        #endif
            g_pD3DDevice8->DrawPrimitive
            (
                PCPrimitiveType,
                StartVertex,
                VPDesc.dwPrimitiveCount
            );
        #ifdef _DEBUG_TRACK_VB
        }
        #endif
    }

    VertPatch.Restore();

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_DrawVerticesUP\n"
           "(\n"
           "   PrimitiveType            : 0x%.08X\n"
           "   VertexCount              : 0x%.08X\n"
           "   pVertexStreamZeroData    : 0x%.08X\n"
           "   VertexStreamZeroStride   : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), PrimitiveType, VertexCount, pVertexStreamZeroData,
           VertexStreamZeroStride);

    EmuUpdateDeferredStates();

    if( (PrimitiveType == X_D3DPT_QUADSTRIP) || (PrimitiveType == X_D3DPT_POLYGON) )
        CxbxKrnlCleanup("Unsupported PrimitiveType! (%d)", (DWORD)PrimitiveType);

    /*
    // DEBUG
    {
        static FLOAT fixer[] = 
        {
            0.0f, 0.0f, 1.0f,
            0.0f, 480.0f, 1.0f,
            640.0f, 0.0f, 1.0f,
            640.0f, 480.0f, 1.0f,
        };

        DWORD *pdwVB = (DWORD*)pVertexStreamZeroData;
    
        for(uint r=0;r<VertexCount;r++)
        {
            pdwVB[0] = FtoDW(fixer[r*3+0]);
            pdwVB[1] = FtoDW(fixer[r*3+1]);
            pdwVB[2] = FtoDW(fixer[r*3+2]);
            pdwVB[5] = 0xFFFFFFFF;

            FLOAT px = DWtoF(pdwVB[0]);
            FLOAT py = DWtoF(pdwVB[1]);
            FLOAT pz = DWtoF(pdwVB[2]);
            FLOAT rhw = DWtoF(pdwVB[3]);
            DWORD dwDiffuse = pdwVB[5];
            DWORD dwSpecular = pdwVB[4];
            FLOAT tx = DWtoF(pdwVB[6]);
            FLOAT ty = DWtoF(pdwVB[7]);
            
            //D3DFVF_POSITION_MASK
            
            printf("%.02d XYZ        : {%.08f, %.08f, %.08f}\n", r, px, py, pz);
            printf("%.02d RHW        : %f\n", r, rhw);
            printf("%.02d dwDiffuse  : 0x%.08X\n", r, dwDiffuse);
            printf("%.02d dwSpecular : 0x%.08X\n", r, dwSpecular);
            printf("%.02d Tex1       : {%.08f, %.08f}\n", r, tx, ty);
            printf("\n");

            pdwVB += (VertexStreamZeroStride/4);
        }
    }
    //*/

    /*
    IDirect3DBaseTexture8 *pTexture = 0;

    g_pD3DDevice8->GetTexture(0, &pTexture);
    
    if(pTexture != NULL)
    {
        static int dwDumpTexture = 0;

        char szBuffer[255];

        sprintf(szBuffer, "C:\\Aaron\\Textures\\Texture-Active%.03d.bmp", dwDumpTexture++);

        D3DXSaveTextureToFile(szBuffer, D3DXIFF_BMP, pTexture, NULL);
    }
    //*/

    UINT PrimitiveCount = EmuD3DVertex2PrimitiveCount(PrimitiveType, VertexCount);

    // Convert from Xbox to PC enumeration
    D3DPRIMITIVETYPE PCPrimitiveType = EmuPrimitiveType(PrimitiveType);

    VertexPatchDesc VPDesc;

    VPDesc.dwVertexCount = VertexCount;
    VPDesc.PrimitiveType = PrimitiveType;
    VPDesc.dwPrimitiveCount = PrimitiveCount;
    VPDesc.dwOffset = 0;
    VPDesc.pVertexStreamZeroData = pVertexStreamZeroData;
    VPDesc.uiVertexStreamZeroStride = VertexStreamZeroStride;
    VPDesc.hVertexShader = g_CurrentVertexShader;

    VertexPatcher VertPatch;

    bool bPatched = VertPatch.Apply(&VPDesc);

    if (IsValidCurrentShader())
    {
        #ifdef _DEBUG_TRACK_VB
        if(!g_bVBSkipStream)
        {
        #endif

        g_pD3DDevice8->DrawPrimitiveUP
        (
            PCPrimitiveType,
            VPDesc.dwPrimitiveCount,
            VPDesc.pVertexStreamZeroData,
            VPDesc.uiVertexStreamZeroStride
        );

        #ifdef _DEBUG_TRACK_VB
        }
        #endif
    }

    VertPatch.Restore();

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_DrawIndexedVertices\n"
           "(\n"
           "   PrimitiveType       : 0x%.08X\n"
           "   VertexCount         : 0x%.08X\n"
           "   pIndexData          : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), PrimitiveType, VertexCount, pIndexData);

    // update index buffer, if necessary
    if(g_pIndexBuffer != 0 && g_pIndexBuffer->Lock == X_D3DRESOURCE_LOCK_FLAG_NOSIZE)
    {
        DWORD dwSize = VertexCount*2;   // 16-bit indices

        HRESULT hRet = g_pD3DDevice8->CreateIndexBuffer
        (
            dwSize, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED,
            &g_pIndexBuffer->EmuIndexBuffer8
        );

        if(FAILED(hRet))
            CxbxKrnlCleanup("CreateIndexBuffer Failed!");

        BYTE *pData = 0;

        hRet = g_pIndexBuffer->EmuIndexBuffer8->Lock(0, dwSize, &pData, 0);

        if(FAILED(hRet))
            CxbxKrnlCleanup("IndexBuffer Lock Failed!");

        memcpy(pData, (void*)g_pIndexBuffer->Data, dwSize);

        g_pIndexBuffer->EmuIndexBuffer8->Unlock();

        g_pIndexBuffer->Data = (ULONG)pData;

        hRet = g_pD3DDevice8->SetIndices(g_pIndexBuffer->EmuIndexBuffer8, g_dwBaseVertexIndex);

        if(FAILED(hRet))
            CxbxKrnlCleanup("SetIndices Failed!");
    }

    EmuUpdateDeferredStates();

    if( (PrimitiveType == X_D3DPT_QUADLIST) || (PrimitiveType == X_D3DPT_QUADSTRIP) || (PrimitiveType == X_D3DPT_POLYGON) )
        EmuWarning("Unsupported PrimitiveType! (%d)", (DWORD)PrimitiveType);

    UINT PrimitiveCount = EmuD3DVertex2PrimitiveCount(PrimitiveType, VertexCount);

    // Convert from Xbox to PC enumeration
    D3DPRIMITIVETYPE PCPrimitiveType = EmuPrimitiveType(PrimitiveType);

    VertexPatchDesc VPDesc;

    VPDesc.dwVertexCount = VertexCount;
    VPDesc.PrimitiveType = PrimitiveType;
    VPDesc.dwPrimitiveCount = PrimitiveCount;
    VPDesc.dwOffset = 0;
    VPDesc.pVertexStreamZeroData = 0;
    VPDesc.uiVertexStreamZeroStride = 0;
    VPDesc.hVertexShader = g_CurrentVertexShader;

    VertexPatcher VertPatch;

    bool bPatched = VertPatch.Apply(&VPDesc);

    #ifdef _DEBUG_TRACK_VB
    if(!g_bVBSkipStream)
    {
    #endif

    bool bActiveIB = false;

    IDirect3DIndexBuffer8 *pIndexBuffer = 0;

    // check if there is an active index buffer
    {
        UINT BaseIndex = 0;

        g_pD3DDevice8->GetIndices(&pIndexBuffer, &BaseIndex);

        if(pIndexBuffer != 0)
        {
            bActiveIB = true;
            pIndexBuffer->Release();
        }
    }

    UINT uiNumVertices = 0;
    UINT uiStartIndex = 0;

    // TODO: caching (if it becomes noticably slow to recreate the buffer each time)
    if(!bActiveIB)
    {
        g_pD3DDevice8->CreateIndexBuffer(VertexCount*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pIndexBuffer);

        if(pIndexBuffer == 0)
            CxbxKrnlCleanup("Could not create index buffer! (%d bytes)", VertexCount*2);

        BYTE *pbData = 0;

        pIndexBuffer->Lock(0, 0, &pbData, 0);

        if(pbData == 0)
            CxbxKrnlCleanup("Could not lock index buffer!");

        memcpy(pbData, pIndexData, VertexCount*2);

        pIndexBuffer->Unlock();

        g_pD3DDevice8->SetIndices(pIndexBuffer, 0);

        uiNumVertices = VertexCount;
        uiStartIndex = 0;
    }
    else
    {
        uiNumVertices = ((DWORD)pIndexData)/2 + VertexCount;
        uiStartIndex = ((DWORD)pIndexData)/2;
    }

    if(IsValidCurrentShader())
    {
        g_pD3DDevice8->DrawIndexedPrimitive
        (
            PCPrimitiveType, 0, uiNumVertices, uiStartIndex, VPDesc.dwPrimitiveCount
        );
    }

    if(!bActiveIB)
    {
        g_pD3DDevice8->SetIndices(0, 0);
        pIndexBuffer->Release();
    }

    #ifdef _DEBUG_TRACK_VB
    }
    #endif

    VertPatch.Restore();

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_DrawIndexedVerticesUP
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_DrawIndexedVerticesUP
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    UINT                VertexCount,
    CONST PVOID         pIndexData,
    CONST PVOID         pVertexStreamZeroData,
    UINT                VertexStreamZeroStride
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_DrawIndexedVerticesUP\n"
           "(\n"
           "   PrimitiveType            : 0x%.08X\n"
           "   VertexCount              : 0x%.08X\n"
           "   pIndexData               : 0x%.08X\n"
           "   pVertexStreamZeroData    : 0x%.08X\n"
           "   VertexStreamZeroStride   : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), PrimitiveType, VertexCount, pIndexData, pVertexStreamZeroData, VertexStreamZeroStride);

    // update index buffer, if necessary
    if(g_pIndexBuffer != 0 && g_pIndexBuffer->Lock == X_D3DRESOURCE_LOCK_FLAG_NOSIZE)
        CxbxKrnlCleanup("g_pIndexBuffer != 0");

    EmuUpdateDeferredStates();

    if( (PrimitiveType == X_D3DPT_QUADLIST) || (PrimitiveType == X_D3DPT_QUADSTRIP) || (PrimitiveType == X_D3DPT_POLYGON) )
        EmuWarning("Unsupported PrimitiveType! (%d)", (DWORD)PrimitiveType);

    UINT PrimitiveCount = EmuD3DVertex2PrimitiveCount(PrimitiveType, VertexCount);

    // Convert from Xbox to PC enumeration
    D3DPRIMITIVETYPE PCPrimitiveType = EmuPrimitiveType(PrimitiveType);

    VertexPatchDesc VPDesc;

    VPDesc.dwVertexCount = VertexCount;
    VPDesc.PrimitiveType = PrimitiveType;
    VPDesc.dwPrimitiveCount = PrimitiveCount;
    VPDesc.dwOffset = 0;
    VPDesc.pVertexStreamZeroData = pVertexStreamZeroData;
    VPDesc.uiVertexStreamZeroStride = VertexStreamZeroStride;
    VPDesc.hVertexShader = g_CurrentVertexShader;

    VertexPatcher VertPatch;

    bool bPatched = VertPatch.Apply(&VPDesc);

    #ifdef _DEBUG_TRACK_VB
    if(!g_bVBSkipStream)
    {
    #endif

    if (IsValidCurrentShader())
    {
        g_pD3DDevice8->DrawIndexedPrimitiveUP
        (
            PCPrimitiveType, 0, VertexCount, VPDesc.dwPrimitiveCount, pIndexData, D3DFMT_INDEX16, VPDesc.pVertexStreamZeroData, VPDesc.uiVertexStreamZeroStride
        );
    }

    #ifdef _DEBUG_TRACK_VB
    }
    #endif

    VertPatch.Restore();

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetLight\n"
           "(\n"
           "   Index               : 0x%.08X\n"
           "   pLight              : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Index, pLight);

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetMaterial\n"
           "(\n"
           "   pMaterial           : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pMaterial);

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_LightEnable\n"
           "(\n"
           "   Index               : 0x%.08X\n"
           "   bEnable             : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Index, bEnable);

    HRESULT hRet = g_pD3DDevice8->LightEnable(Index, bEnable);

    EmuSwapFS();   // XBox FS
    
    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_BlockUntilVerticalBlank
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_BlockUntilVerticalBlank()
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_BlockUntilVerticalBlank();\n", GetCurrentThreadId());

    return;
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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetRenderTarget\n"
           "(\n"
           "   pRenderTarget       : 0x%.08X (0x%.08X)\n"
           "   pNewZStencil        : 0x%.08X (0x%.08X)\n"
           ");\n",
           GetCurrentThreadId(), pRenderTarget, (pRenderTarget != 0) ? pRenderTarget->EmuSurface8 : 0, pNewZStencil,
           (pNewZStencil != 0) ? pNewZStencil->EmuSurface8 : 0);

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

    // TODO: Follow that stencil!
    HRESULT hRet = g_pD3DDevice8->SetRenderTarget(pPCRenderTarget, pPCNewZStencil);

    if(FAILED(hRet))
        EmuWarning("SetRenderTarget Failed! (0x%.08X)", hRet);

    EmuSwapFS();   // XBox FS
    
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
    *ppPalette = EmuIDirect3DDevice8_CreatePalette2(Size);

    return D3D_OK;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreatePalette2
// ******************************************************************
XTL::X_D3DPalette * WINAPI XTL::EmuIDirect3DDevice8_CreatePalette2
(
    X_D3DPALETTESIZE    Size
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_CreatePalette2\n"
           "(\n"
           "   Size                : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Size);

    X_D3DPalette *pPalette = new X_D3DPalette();
 
    static int lk[4] =
    {
        256*sizeof(D3DCOLOR),    // D3DPALETTE_256
        128*sizeof(D3DCOLOR),    // D3DPALETTE_128
        64*sizeof(D3DCOLOR),     // D3DPALETTE_64
        32*sizeof(D3DCOLOR)      // D3DPALETTE_32
    };

    pPalette->Common = 0;
    pPalette->Lock = 0x8000BEEF; // emulated reference count for palettes
    pPalette->Data = (DWORD)new uint08[lk[Size]];

    EmuSwapFS();   // XBox FS
    
    return pPalette;
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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetPalette\n"
           "(\n"
           "   Stage               : 0x%.08X\n"
           "   pPalette            : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Stage, pPalette);

//    g_pD3DDevice8->SetPaletteEntries(0, (PALETTEENTRY*)pPalette->Data);

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetFlickerFilter\n"
           "(\n"
           "   Filter              : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Filter);

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

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetSoftDisplayFilter\n"
           "(\n"
           "   Enable              : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Enable);

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
    *ppColors = EmuIDirect3DPalette8_Lock2(pThis, Flags);
    
    return D3D_OK;
}

// ******************************************************************
// * func: EmuIDirect3DPalette8_Lock2
// ******************************************************************
XTL::D3DCOLOR * WINAPI XTL::EmuIDirect3DPalette8_Lock2
(
    X_D3DPalette   *pThis,
    DWORD           Flags
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuD3D8 (0x%X): EmuIDirect3DPalette8_Lock\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
           "   Flags               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Flags);

    D3DCOLOR *pColors = (D3DCOLOR*)pThis->Data;

    EmuSwapFS();   // XBox FS

    return pColors;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetVertexShaderSize
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_GetVertexShaderSize
(
    DWORD Handle, 
    UINT* pSize
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf( "EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetVertexShaderSize\n"
               "(\n"
               "   Handle               : 0x%.08X\n"
               "   pSize                : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Handle, pSize);

    if(pSize  && VshHandleIsVertexShader(Handle))
    {
        X_D3DVertexShader *pD3DVertexShader = (X_D3DVertexShader *)(Handle & 0x7FFFFFFF);
        VERTEX_SHADER *pVertexShader = (VERTEX_SHADER *)pD3DVertexShader->Handle;
        *pSize = pVertexShader->Size;
    }
    else if(pSize)
    {
        *pSize = 0;
    }

    EmuSwapFS();   // Xbox FS
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_DeleteVertexShader
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_DeleteVertexShader
(
    DWORD Handle
)
{
    EmuSwapFS();

    DbgPrintf( "EmuD3D8 (0x%.08X): EmuIDirect3DDevice8_DeleteVertexShader\n"
               "(\n"
               "   Handle                : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Handle);

    DWORD RealHandle = 0;

    if(VshHandleIsVertexShader(Handle))
    {
        X_D3DVertexShader *pD3DVertexShader = (X_D3DVertexShader *)(Handle & 0x7FFFFFFF);
        VERTEX_SHADER *pVertexShader = (VERTEX_SHADER *)pD3DVertexShader->Handle;

        RealHandle = pVertexShader->Handle;
        CxbxFree(pVertexShader->pDeclaration);

        if(pVertexShader->pFunction)
        {
            CxbxFree(pVertexShader->pFunction);
        }

        FreeVertexDynamicPatch(pVertexShader);

        CxbxFree(pVertexShader);
        CxbxFree(pD3DVertexShader);
    }

    HRESULT hRet = g_pD3DDevice8->DeleteVertexShader(RealHandle);

    EmuSwapFS();
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SelectVertexShaderDirect
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_SelectVertexShaderDirect
(
    X_VERTEXATTRIBUTEFORMAT *pVAF,
    DWORD                    Address
)
{
    EmuSwapFS();   // Win2k/XP FS

    // debug trace
    DbgPrintf( "EmuD3D8 (0x%X): EmuIDirect3DDevice8_SelectVertexShaderDirect\n"
               "(\n"
               "   pVAF                : 0x%.08X\n"
               "   Address             : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pVAF,Address);

    DbgPrintf("NOT YET IMPLEMENTED!\n");

    EmuSwapFS();   // Xbox FS
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetShaderConstantMode
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_GetShaderConstantMode
(
    DWORD *pMode
)
{
    #ifdef _DEBUG_TRACE
    {
        EmuSwapFS();   // Win2k/XP FS
        DbgPrintf( "EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetShaderConstantMode\n"
                   "(\n"
                   "   pMode               : 0x%.08X\n"
                   ");\n",
                   GetCurrentThreadId(), pMode);
        EmuSwapFS();   // Xbox FS
    }
    #endif

    if(pMode)
    {
        *pMode = g_VertexShaderConstantMode;
    }
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetVertexShader
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_GetVertexShader
(
    DWORD *pHandle
)
{
    EmuSwapFS();

    // debug trace
    DbgPrintf( "EmuD3D8 (0x%.08X): EmuIDirect3DDevice8_GetVertexShader\n"
               "(\n"
               "   pHandle               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pHandle);

    if(pHandle)
    {
        (*pHandle) = g_CurrentVertexShader;
    }

    EmuSwapFS();
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetVertexShaderConstant
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_GetVertexShaderConstant
(
    INT   Register,
    void  *pConstantData,
    DWORD ConstantCount
)
{
    EmuSwapFS();   // Win2k/XP FS

    // debug trace
    DbgPrintf( "EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetVertexShaderConstant\n"
               "(\n"
               "   Register            : 0x%.08X\n"
               "   pConstantData       : 0x%.08X\n"
               "   ConstantCount       : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Register, pConstantData, ConstantCount);

    HRESULT hRet = g_pD3DDevice8->GetVertexShaderConstant
    (
        Register + 96,
        pConstantData,
        ConstantCount
    );

    EmuSwapFS();   // XBox FS
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexShaderInputDirect
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_SetVertexShaderInputDirect
(
    X_VERTEXATTRIBUTEFORMAT *pVAF,
    UINT                     StreamCount,
    X_STREAMINPUT           *pStreamInputs
)
{
    EmuSwapFS();   // Win2k/XP FS

    // debug trace
    DbgPrintf( "EmuD3D8 (0x%X): EmuIDirect3DDevice8_SelectVertexShaderDirect\n"
               "(\n"
               "   pVAF                : 0x%.08X\n"
               "   StreamCount         : 0x%.08X\n"
               "   pStreamInputs       : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pVAF, StreamCount, pStreamInputs);

    DbgPrintf("NOT YET IMPLEMENTED!\n");

    EmuSwapFS();   // Xbox FS

    return 0;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetVertexShaderInput
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_GetVertexShaderInput
(
    DWORD              *pHandle,
    UINT               *pStreamCount,
    X_STREAMINPUT      *pStreamInputs
)
{
    EmuSwapFS();   // Win2k/XP FS

    // debug trace
    DbgPrintf( "EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetVertexShaderInput\n"
               "(\n"
               "   pHandle             : 0x%.08X\n"
               "   pStreamCount        : 0x%.08X\n"
               "   pStreamInputs       : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pHandle, pStreamCount, pStreamInputs);

    DbgPrintf("NOT YET IMPLEMENTED!\n");

    EmuSwapFS();   // Xbox FS

    return 0;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexShaderInput
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_SetVertexShaderInput
(
    DWORD              Handle,
    UINT               StreamCount,
    X_STREAMINPUT     *pStreamInputs
)
{
    EmuSwapFS();   // Win2k/XP FS

    // debug trace
    DbgPrintf( "EmuD3D8 (0x%X): EmuIDirect3DDevice8_SetVertexShaderInput\n"
               "(\n"
               "   Handle              : 0x%.08X\n"
               "   StreamCount         : 0x%.08X\n"
               "   pStreamInputs       : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Handle, StreamCount, pStreamInputs);

    DbgPrintf("NOT YET IMPLEMENTED!\n");

    EmuSwapFS();   // Xbox FS

    return 0;
}


// ******************************************************************
// * func: EmuIDirect3DDevice8_RunVertexStateShader
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_RunVertexStateShader
(
    DWORD Address, 
    CONST FLOAT *pData
)
{
    EmuSwapFS();   // Win2k/XP FS

    // debug trace
    DbgPrintf( "EmuD3D8 (0x%X): EmuIDirect3DDevice8_RunVertexStateShader\n"
               "(\n"
               "   Address              : 0x%.08X\n"
               "   pData                : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Address,pData);

    DbgPrintf("NOT YET IMPLEMENTED!\n");

    EmuSwapFS();   // Xbox FS
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_LoadVertexShaderProgram
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_LoadVertexShaderProgram
(
    CONST DWORD *pFunction, 
    DWORD        Address
)
{
    EmuSwapFS();   // Win2k/XP FS

    // debug trace
    DbgPrintf( "EmuD3D8 (0x%X): EmuIDirect3DDevice8_LoadVertexShaderProgram\n"
               "(\n"
               "   pFunction           : 0x%.08X\n"
               "   Address             : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pFunction,Address);

    DbgPrintf("NOT YET IMPLEMENTED!\n");

    EmuSwapFS();   // Xbox FS
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetVertexShaderType
// ******************************************************************
VOID WINAPI XTL::EmuIDirect3DDevice8_GetVertexShaderType
(
    DWORD  Handle, 
    DWORD *pType
)
{
    EmuSwapFS();   // Win2k/XP FS

    // debug trace
    DbgPrintf( "EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetVertexShaderType\n"
               "(\n"
               "   Handle               : 0x%.08X\n"
               "   pType                : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Handle, pType);

    if(pType && VshHandleIsVertexShader(Handle))
    {
        *pType = ((VERTEX_SHADER *)(VshHandleGetVertexShader(Handle))->Handle)->Type;
    }

    EmuSwapFS();   // Xbox FS
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetVertexShaderDeclaration
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_GetVertexShaderDeclaration
(
    DWORD  Handle,
    PVOID  pData,
    DWORD *pSizeOfData
)
{
    EmuSwapFS();   // Win2k/XP FS

    // debug trace
    DbgPrintf( "EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetVertexShaderDeclaration\n"
               "(\n"
               "   Handle               : 0x%.08X\n"
               "   pData                : 0x%.08X\n"
               "   pSizeOfData          : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Handle, pData, pSizeOfData);

    HRESULT hRet = D3DERR_INVALIDCALL;

    if(pSizeOfData && VshHandleIsVertexShader(Handle))
    {
        VERTEX_SHADER *pVertexShader = (VERTEX_SHADER *)(VshHandleGetVertexShader(Handle))->Handle;
        if(*pSizeOfData < pVertexShader->DeclarationSize || !pData)
        {
            *pSizeOfData = pVertexShader->DeclarationSize;

            hRet = !pData ? D3D_OK : D3DERR_MOREDATA;
        }
        else
        {
            memcpy(pData, pVertexShader->pDeclaration, pVertexShader->DeclarationSize);
            hRet = D3D_OK;
        }
    }

    EmuSwapFS();   // Xbox FS
    return hRet;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetVertexShaderFunction
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirect3DDevice8_GetVertexShaderFunction
(
    DWORD  Handle,
    PVOID *pData,
    DWORD *pSizeOfData
)
{
    EmuSwapFS();   // Win2k/XP FS

    // debug trace
    DbgPrintf( "EmuD3D8 (0x%X): EmuIDirect3DDevice8_GetVertexShaderFunction\n"
               "(\n"
               "   Handle               : 0x%.08X\n"
               "   pData                : 0x%.08X\n"
               "   pSizeOfData          : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Handle,pData,pSizeOfData);

    HRESULT hRet = D3DERR_INVALIDCALL;

    if(pSizeOfData && VshHandleIsVertexShader(Handle))
    {
        VERTEX_SHADER *pVertexShader = (VERTEX_SHADER *)(VshHandleGetVertexShader(Handle))->Handle;
        if(*pSizeOfData < pVertexShader->FunctionSize || !pData)
        {
            *pSizeOfData = pVertexShader->FunctionSize;

            hRet = !pData ? D3D_OK : D3DERR_MOREDATA;
        }
        else
        {
            memcpy(pData, pVertexShader->pFunction, pVertexShader->FunctionSize);
            hRet = D3D_OK;
        }
    }

    EmuSwapFS();   // Xbox FS
    return hRet;
}
