// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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

#include "CxbxUtil.h"
#include "CxbxKrnl.h"
#include "Emu.h"
#include "EmuFS.h"
#include "EmuShared.h"
#include "DbgConsole.h"
#include "ResourceTracker.h"
#include "EmuAlloc.h"
#include "EmuXTL.h"

#include <process.h>
#include <clocale>

// Global(s)
HWND                                g_hEmuWindow   = NULL; // rendering window
XTL::LPDIRECT3DDEVICE8              g_pD3DDevice8  = NULL; // Direct3D8 Device
XTL::LPDIRECTDRAWSURFACE7           g_pDDSPrimary  = NULL; // DirectDraw7 Primary Surface
XTL::LPDIRECTDRAWSURFACE7           g_pDDSOverlay7 = NULL; // DirectDraw7 Overlay Surface
XTL::LPDIRECTDRAWCLIPPER            g_pDDClipper   = NULL; // DirectDraw7 Clipper
DWORD                               g_CurrentVertexShader = 0;
DWORD								g_dwCurrentPixelShader = 0;
XTL::PIXEL_SHADER                  *g_CurrentPixelShader = NULL;
BOOL                                g_bFakePixelShaderLoaded = FALSE;
BOOL                                g_bIsFauxFullscreen = FALSE;
BOOL								g_bHackUpdateSoftwareOverlay = FALSE;

// Static Function(s)
static BOOL WINAPI                  EmuEnumDisplayDevices(GUID FAR *lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext, HMONITOR hm);
static DWORD WINAPI                 EmuRenderWindow(LPVOID);
static DWORD WINAPI                 EmuCreateDeviceProxy(LPVOID);
static LRESULT WINAPI               EmuMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static DWORD WINAPI                 EmuUpdateTickCount(LPVOID);
static inline void                  EmuVerifyResourceIsRegistered(XTL::X_D3DResource *pResource);
static void                         EmuAdjustPower2(UINT *dwWidth, UINT *dwHeight);

// Static Variable(s)
static HMONITOR                     g_hMonitor      = NULL; // Handle to DirectDraw monitor
static BOOL                         g_bSupportsYUY2 = FALSE;// Does device support YUY2 overlays?
static XTL::LPDIRECTDRAW7           g_pDD7          = NULL; // DirectDraw7
static DWORD                        g_dwOverlayW    = 640;  // Cached Overlay Width
static DWORD                        g_dwOverlayH    = 480;  // Cached Overlay Height
static DWORD                        g_dwOverlayP    = 640;  // Cached Overlay Pitch
static Xbe::Header                 *g_XbeHeader     = NULL; // XbeHeader
static uint32                       g_XbeHeaderSize = 0;    // XbeHeaderSize
static HBRUSH                       g_hBgBrush      = NULL; // Background Brush
static volatile bool                g_bRenderWindowActive = false;
static XBVideo                      g_XBVideo;
static XTL::D3DVBLANKCALLBACK       g_pVBCallback   = NULL; // Vertical-Blank callback routine
static XTL::D3DSWAPCALLBACK			g_pSwapCallback = NULL;	// Swap/Present callback routine
static XTL::D3DCALLBACK				g_pCallback		= NULL;	// D3DDevice::InsertCallback routine
static XTL::X_D3DCALLBACKTYPE		g_CallbackType;			// Callback type
static DWORD						g_CallbackParam;		// Callback param
static BOOL                         g_bHasDepthStencil = FALSE;  // Does device have a Depth/Stencil Buffer?
//static DWORD						g_dwPrimPerFrame = 0;	// Number of primitives within one frame

// D3D based variables
static GUID                         g_ddguid;               // DirectDraw driver GUID
static XTL::LPDIRECT3D8             g_pD3D8 = NULL;			// Direct3D8
static XTL::D3DCAPS8                g_D3DCaps;              // Direct3D8 Caps

// wireframe toggle
static int                          g_iWireframe    = 0;

// build version
extern uint32						g_BuildVersion;

// resource caching for _Register
static XTL::X_D3DResource pCache[16] = {0};

// current active index buffer
static XTL::X_D3DIndexBuffer       *g_pIndexBuffer  = NULL; // current active index buffer
static DWORD                        g_dwBaseVertexIndex = 0;// current active index buffer base index

// current active vertex stream
static XTL::X_D3DVertexBuffer      *g_pVertexBuffer = NULL; // current active vertex buffer
static XTL::IDirect3DVertexBuffer8 *g_pDummyBuffer = NULL;  // Dummy buffer, used to set unused stream sources with
static DWORD						g_dwLastSetStream = 0;	// The last stream set by D3DDevice::SetStreamSource

// current vertical blank information
static XTL::D3DVBLANKDATA           g_VBData = {0};
static DWORD                        g_VBLastSwap = 0;

// current swap information
static XTL::D3DSWAPDATA				g_SwapData = {0};
static DWORD						g_SwapLast = 0;

// cached Direct3D state variable(s)
static XTL::X_D3DSurface           *g_pCachedRenderTarget = NULL;
static XTL::X_D3DSurface           *g_pCachedDepthStencil = NULL;
static XTL::X_D3DSurface           *g_pCachedYuvSurface = NULL;
static BOOL                         g_fYuvEnabled = FALSE;
static DWORD                        g_dwVertexShaderUsage = 0;
static DWORD                        g_VertexShaderSlots[136];

// cached palette pointer
static PVOID pCurrentPalette = nullptr;
// cached palette size
static DWORD dwCurrentPaletteSize = -1;

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

VOID XTL::CxbxInitWindow(Xbe::Header *XbeHeader, uint32 XbeHeaderSize)
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
		// Ported from Dxbx :
        // If possible, assign this thread to another core than the one that runs Xbox1 code :
        SetThreadAffinityMask(hThread, g_CPUOthers);
        // We set the priority of this thread a bit higher, to assure reliable timing :
        SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);

        // we must duplicate this handle in order to retain Suspend/Resume thread rights from a remote thread
        {
            HANDLE hDupHandle = NULL;

            DuplicateHandle(g_CurrentProcessHandle, hThread, g_CurrentProcessHandle, &hDupHandle, 0, FALSE, DUPLICATE_SAME_ACCESS);

            CxbxKrnlRegisterThread(hDupHandle);
        }
    }

/* TODO : Port this Dxbx code :
  // create vblank handling thread
    {
        dwThreadId = 0;
        {hThread :=} CreateThread(NULL, 0, EmuThreadHandleVBlank, NULL, 0, &dwThreadId);
        // Make sure VBlank callbacks run on the same core as the one that runs Xbox1 code :
        SetThreadAffinityMask(dwThreadId, g_CPUXbox);
    }
*/
    // create window message processing thread
    {
        DWORD dwThreadId;

        g_bRenderWindowActive = false;

        HANDLE hRenderWindowThread = CreateThread(NULL, NULL, EmuRenderWindow, NULL, NULL, &dwThreadId);

		if (hRenderWindowThread == NULL) {
			char szBuffer[1024] = { 0 };
			sprintf(szBuffer, "Creating EmuRenderWindowThread Failed: %08X", GetLastError());
			MessageBoxA(NULL, szBuffer, "CreateThread Failed", 0);
			ExitProcess(0);
		}

		// Ported from Dxbx :
		// If possible, assign this thread to another core than the one that runs Xbox1 code :
		SetThreadAffinityMask(hRenderWindowThread, g_CPUOthers);

        while(!g_bRenderWindowActive)
            Sleep(10); // Dxbx: Should we use SwitchToThread() or YieldProcessor() ?

        Sleep(50);
    }

	SetFocus(g_hEmuWindow);
}

int GetD3DResourceRefCount(XTL::IDirect3DResource8 *EmuResource)
{
	if (EmuResource != nullptr)
	{
		// Get actual reference count by increasing it using AddRef,
		// and relying on the return value of Release (which is
		// probably more reliable than AddRef)
		EmuResource->AddRef();
		return EmuResource->Release();
	}

	return 0;
}

XTL::X_D3DSurface *EmuNewD3DSurface()
{
	XTL::X_D3DSurface *result = (XTL::X_D3DSurface *)calloc(1, sizeof(XTL::X_D3DSurface));
	result->Common = X_D3DCOMMON_D3DCREATED | X_D3DCOMMON_TYPE_SURFACE | 1; // Set refcount to 1
	return result;
}

XTL::X_D3DTexture *EmuNewD3DTexture()
{
	XTL::X_D3DTexture *result = (XTL::X_D3DTexture *)calloc(1, sizeof(XTL::X_D3DTexture));
	result->Common = X_D3DCOMMON_D3DCREATED | X_D3DCOMMON_TYPE_TEXTURE | 1; // Set refcount to 1
	return result;
}

XTL::X_D3DVolumeTexture *EmuNewD3DVolumeTexture()
{
	XTL::X_D3DVolumeTexture *result = (XTL::X_D3DVolumeTexture *)calloc(1, sizeof(XTL::X_D3DVolumeTexture));
	result->Common = X_D3DCOMMON_D3DCREATED | X_D3DCOMMON_TYPE_TEXTURE | 1; // Set refcount to 1
	return result;
}

XTL::X_D3DCubeTexture *EmuNewD3DCubeTexture()
{
	XTL::X_D3DCubeTexture *result = (XTL::X_D3DCubeTexture *)calloc(1, sizeof(XTL::X_D3DCubeTexture));
	result->Common = X_D3DCOMMON_D3DCREATED | X_D3DCOMMON_TYPE_TEXTURE | 1; // Set refcount to 1
	return result;
}

XTL::X_D3DIndexBuffer *EmuNewD3DIndexBuffer()
{
	XTL::X_D3DIndexBuffer *result = (XTL::X_D3DIndexBuffer *)calloc(1, sizeof(XTL::X_D3DIndexBuffer));
	result->Common = X_D3DCOMMON_D3DCREATED | X_D3DCOMMON_TYPE_INDEXBUFFER | 1; // Set refcount to 1
	return result;
}

XTL::X_D3DVertexBuffer *EmuNewD3DVertexBuffer()
{
	XTL::X_D3DVertexBuffer *result = (XTL::X_D3DVertexBuffer *)calloc(1, sizeof(XTL::X_D3DVertexBuffer));
	result->Common = X_D3DCOMMON_D3DCREATED | X_D3DCOMMON_TYPE_VERTEXBUFFER | 1; // Set refcount to 1
	return result;
}

XTL::X_D3DPalette *EmuNewD3DPalette()
{
	XTL::X_D3DPalette *result = (XTL::X_D3DPalette *)calloc(1, sizeof(XTL::X_D3DPalette));
	result->Common = X_D3DCOMMON_D3DCREATED | X_D3DCOMMON_TYPE_PALETTE | 1; // Set refcount to 1
	return result;
}

VOID CxbxSetPixelContainerHeader
(
	XTL::X_D3DPixelContainer* pPixelContainer,
	DWORD				Common,
	UINT				Width,
	UINT				Height,
	UINT				Levels,
	XTL::X_D3DFORMAT	Format,
	UINT				Dimensions,
	UINT				Pitch
)
{
	// Set X_D3DResource field(s) :
	pPixelContainer->Common = Common;
	// DON'T SET pPixelContainer->Data
	// DON'T SET pPixelContainer->Lock

	// Are Width and Height both a power of two?
	DWORD l2w; _BitScanReverse(&l2w, Width); // MSVC intrinsic; GCC has __builtin_clz
	DWORD l2h; _BitScanReverse(&l2h, Height);
	if (((1 << l2w) == Width) && ((1 << l2h) == Height)) {
		Width = Height = Pitch = 1; // When setting Format, clear Size field
	}
	else {
		l2w = l2h = 0; // When setting Size, clear D3DFORMAT_USIZE and VSIZE
	}

	// TODO : Must this be set using Usage / Pool / something else?
	const int Depth = 1;

	// Set X_D3DPixelContainer field(s) :
	pPixelContainer->Format = 0
		| ((Dimensions << X_D3DFORMAT_DIMENSION_SHIFT) & X_D3DFORMAT_DIMENSION_MASK)
		| (((DWORD)Format << X_D3DFORMAT_FORMAT_SHIFT) & X_D3DFORMAT_FORMAT_MASK)
		| ((Levels << X_D3DFORMAT_MIPMAP_SHIFT) & X_D3DFORMAT_MIPMAP_MASK)
		| ((l2w << X_D3DFORMAT_USIZE_SHIFT) & X_D3DFORMAT_USIZE_MASK)
		| ((l2h << X_D3DFORMAT_VSIZE_SHIFT) & X_D3DFORMAT_VSIZE_MASK)
		| ((Depth << X_D3DFORMAT_PSIZE_SHIFT) & X_D3DFORMAT_PSIZE_MASK)
		;
	pPixelContainer->Size = 0
		| (((Width - 1) /*X_D3DSIZE_WIDTH_SHIFT*/) & X_D3DSIZE_WIDTH_MASK)
		| (((Height - 1) << X_D3DSIZE_HEIGHT_SHIFT) & X_D3DSIZE_HEIGHT_MASK)
		| (((Pitch - 1) << X_D3DSIZE_PITCH_SHIFT) & X_D3DSIZE_PITCH_MASK)
		;
}

// Direct3D initialization (called before emulation begins)
VOID XTL::EmuD3DInit()
{
	// create the create device proxy thread
	{
		DWORD dwThreadId;

		CreateThread(NULL, NULL, EmuCreateDeviceProxy, NULL, NULL, &dwThreadId);
		// Ported from Dxbx :
		// If possible, assign this thread to another core than the one that runs Xbox1 code :
		SetThreadAffinityMask(&dwThreadId, g_CPUOthers);
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

    // create default device
    {
        XTL::X_D3DPRESENT_PARAMETERS PresParam;

        ZeroMemory(&PresParam, sizeof(PresParam));

        PresParam.BackBufferWidth  = 640;
        PresParam.BackBufferHeight = 480;
        PresParam.BackBufferFormat = X_D3DFMT_A8R8G8B8;
        PresParam.BackBufferCount  = 1;
        PresParam.EnableAutoDepthStencil = TRUE;
		PresParam.AutoDepthStencilFormat = X_D3DFMT_D24S8;
        PresParam.SwapEffect = XTL::D3DSWAPEFFECT_DISCARD;

            
        XTL::EMUPATCH(Direct3D_CreateDevice)(0, XTL::D3DDEVTYPE_HAL, 0, D3DCREATE_HARDWARE_VERTEXPROCESSING, &PresParam, &g_pD3DDevice8);
            
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
    char AsciiTitle[MAX_PATH];

    // register window class
    {
        LOGBRUSH logBrush = {BS_SOLID, RGB(0,0,0)};

        g_hBgBrush = CreateBrushIndirect(&logBrush);

        WNDCLASSEX wc =
        {
            sizeof(WNDCLASSEX),
            CS_CLASSDC,
            EmuMsgProc,
            0, 0, GetModuleHandle(NULL),
			0, // TODO : LoadIcon(hmodule, ?)
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

#define CertTitleNameLength 40

		// Does the title fall entirely inside the read XbeHeader?
        if(CertAddr + offsetof(Xbe::Certificate, wszTitleName) + CertTitleNameLength < g_XbeHeaderSize)
        {
            Xbe::Certificate *XbeCert = (Xbe::Certificate*)((uint32)g_XbeHeader + CertAddr);

            setlocale( LC_ALL, "English" );

            wcstombs(tAsciiTitle, XbeCert->wszTitleName, CertTitleNameLength);
        }

		sprintf(AsciiTitle, "Cxbx-Reloaded %s : Emulating %s", _CXBX_VERSION, tAsciiTitle);
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

    ShowWindow(g_hEmuWindow, ((CxbxKrnl_hEmuParent == 0) || g_XBVideo.GetFullscreen()) ? SW_SHOWDEFAULT : SW_SHOWMAXIMIZED);
    UpdateWindow(g_hEmuWindow);

    if(!g_XBVideo.GetFullscreen() && (CxbxKrnl_hEmuParent != NULL))
    {
        SetFocus(CxbxKrnl_hEmuParent);
    }

    // initialize direct input
    if(!XTL::EmuDInputInit())
        CxbxKrnlCleanup("Could not initialize DirectInput!");

    DbgPrintf("EmuD3D8: Message-Pump thread is running.\n");

    SetFocus(g_hEmuWindow);

    DbgConsole *dbgConsole = new DbgConsole();

    // message processing loop
    {
        MSG msg;

        ZeroMemory(&msg, sizeof(msg));

        bool lPrintfOn = g_bPrintfOn;

        g_bRenderWindowActive = true;

        while(msg.message != WM_QUIT)
        {
            if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
            {
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
            return D3D_OK; // = 0
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
                return D3D_OK; // = 0
            }

            return DefWindowProc(hWnd, msg, wParam, lParam);
        }
        break;

        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return D3D_OK; // = 0
}

// timing thread procedure
static DWORD WINAPI EmuUpdateTickCount(LPVOID)
{
    // since callbacks come from here
    EmuGenerateFS(CxbxKrnl_TLS, CxbxKrnl_TLSData);

    DbgPrintf("EmuD3D8: Timing thread is running.\n");

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

			// TODO: Fixme.  This may not be right...
			g_SwapData.SwapVBlank = 1;

            if(g_pVBCallback != NULL)
            {
                    
                g_pVBCallback(&g_VBData);
                    
            }

            g_VBData.Swap = 0;

			// TODO: This can't be accurate...
			g_SwapData.TimeUntilSwapVBlank = 0;

			// TODO: Recalculate this for PAL version if necessary.
			// Also, we should check the D3DPRESENT_INTERVAL value for accurracy.
		//	g_SwapData.TimeBetweenSwapVBlanks = 1/60;
			g_SwapData.TimeBetweenSwapVBlanks = 0;
        }
    }

    timeEndPeriod(0);
}

// thread dedicated to create devices
static DWORD WINAPI EmuCreateDeviceProxy(LPVOID)
{
    DbgPrintf("EmuD3D8: CreateDevice proxy thread is running.\n");

    while(true)
    {
        // if we have been signalled, create the device with cached parameters
        if(g_EmuCDPD.bReady)
        {
            DbgPrintf("EmuD3D8: CreateDevice proxy thread recieved request.\n");

            if(g_EmuCDPD.bCreate)
            {
                // only one device should be created at once
                // TODO: ensure all surfaces are somehow cleaned up?
                if(g_pD3DDevice8 != nullptr)
                {
                    DbgPrintf("EmuD3D8: CreateDevice proxy thread releasing old Device.\n");

                    g_pD3DDevice8->EndScene();

                    // Address DirectX Debug Runtime reported error in _DEBUG builds
                    // Direct3D8: (ERROR) :Not all objects were freed: the following indicate the types of unfreed objects.
                    #ifndef _DEBUG
                        while(g_pD3DDevice8->Release() != 0);
                    #endif

                    g_pD3DDevice8 = nullptr;
                }

                if(g_EmuCDPD.pPresentationParameters->BufferSurfaces[0] != NULL)
                    EmuWarning("BufferSurfaces[0] : 0x%.08X", g_EmuCDPD.pPresentationParameters->BufferSurfaces[0]);

                if(g_EmuCDPD.pPresentationParameters->DepthStencilSurface != NULL)
                    EmuWarning("DepthStencilSurface : 0x%.08X", g_EmuCDPD.pPresentationParameters->DepthStencilSurface);

                // make adjustments to parameters to make sense with windows Direct3D
                {
                    g_EmuCDPD.DeviceType = (g_XBVideo.GetDirect3DDevice() == 0) ? XTL::D3DDEVTYPE_HAL : XTL::D3DDEVTYPE_REF;
                    g_EmuCDPD.Adapter = g_XBVideo.GetDisplayAdapter();

                    g_EmuCDPD.pPresentationParameters->Windowed = !g_XBVideo.GetFullscreen();

                    if(g_XBVideo.GetVSync())
                        g_EmuCDPD.pPresentationParameters->SwapEffect = XTL::D3DSWAPEFFECT_COPY_VSYNC;

                    g_EmuCDPD.hFocusWindow = g_hEmuWindow;

                    g_EmuCDPD.pPresentationParameters->BackBufferFormat       = (XTL::X_D3DFORMAT)XTL::EmuXB2PC_D3DFormat(g_EmuCDPD.pPresentationParameters->BackBufferFormat);
					g_EmuCDPD.pPresentationParameters->AutoDepthStencilFormat = (XTL::X_D3DFORMAT)XTL::EmuXB2PC_D3DFormat(g_EmuCDPD.pPresentationParameters->AutoDepthStencilFormat);

                    if(!g_XBVideo.GetVSync() && (g_D3DCaps.PresentationIntervals & D3DPRESENT_INTERVAL_IMMEDIATE) && g_XBVideo.GetFullscreen())
                        g_EmuCDPD.pPresentationParameters->FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
                    else
                    {
                        if(g_D3DCaps.PresentationIntervals & D3DPRESENT_INTERVAL_ONE && g_XBVideo.GetFullscreen())
                            g_EmuCDPD.pPresentationParameters->FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
                        else
                            g_EmuCDPD.pPresentationParameters->FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
                    }

					// HACK: Disable Tripple Buffering for now...
					// TODO: Enumerate maximum BackBufferCount if possible.
					if(g_EmuCDPD.pPresentationParameters->BackBufferCount > 1)
					{
						EmuWarning("Limiting BackBufferCount to 1...");
						g_EmuCDPD.pPresentationParameters->BackBufferCount = 1;
					}

                    // TODO: Support Xbox extensions if possible
                    if(g_EmuCDPD.pPresentationParameters->MultiSampleType != 0)
                    {
                        EmuWarning("MultiSampleType 0x%.08X is not supported!", g_EmuCDPD.pPresentationParameters->MultiSampleType);

                        g_EmuCDPD.pPresentationParameters->MultiSampleType = XTL::D3DMULTISAMPLE_NONE;

                        // TODO: Check card for multisampling abilities
            //            if(pPresentationParameters->MultiSampleType == X_D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX) // = 0x00001121
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

                        g_EmuCDPD.pPresentationParameters->BackBufferFormat = (XTL::X_D3DFORMAT)D3DDisplayMode.Format;
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
							g_EmuCDPD.pPresentationParameters->BackBufferFormat = (XTL::X_D3DFORMAT)XTL::D3DFMT_X1R5G5B5;
                        else if(strcmp(szBackBufferFormat, "r5g6r5") == 0)
							g_EmuCDPD.pPresentationParameters->BackBufferFormat = (XTL::X_D3DFORMAT)XTL::D3DFMT_R5G6B5;
                        else if(strcmp(szBackBufferFormat, "x8r8g8b8") == 0)
							g_EmuCDPD.pPresentationParameters->BackBufferFormat = (XTL::X_D3DFORMAT)XTL::D3DFMT_X8R8G8B8;
                        else if(strcmp(szBackBufferFormat, "a8r8g8b8") == 0)
							g_EmuCDPD.pPresentationParameters->BackBufferFormat = (XTL::X_D3DFORMAT)XTL::D3DFMT_A8R8G8B8;
                    }
                }

                // detect vertex processing capabilities
                if((g_D3DCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) && g_EmuCDPD.DeviceType == XTL::D3DDEVTYPE_HAL)
                {
                    DbgPrintf("EmuD3D8: Using hardware vertex processing\n");

                    g_EmuCDPD.BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
                    g_dwVertexShaderUsage = 0;
                }
                else
                {
                    DbgPrintf("EmuD3D8: Using software vertex processing\n");

                    g_EmuCDPD.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
                    g_dwVertexShaderUsage = D3DUSAGE_SOFTWAREPROCESSING;
                }
   
				// Dxbx addition : Prevent Direct3D from changing the FPU Control word :
				g_EmuCDPD.BehaviorFlags |= D3DCREATE_FPU_PRESERVE;

	            // Address debug DirectX runtime warning in _DEBUG builds
                // Direct3D8: (WARN) :Device that was created without D3DCREATE_MULTITHREADED is being used by a thread other than the creation thread.
                #ifdef _DEBUG
                    g_EmuCDPD.BehaviorFlags |= D3DCREATE_MULTITHREADED;
                #endif

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
					else
					{
						// Does the user want to use Hardware accelerated YUV surfaces?
						if (g_XBVideo.GetHardwareYUV())
							DbgPrintf("EmuD3D8: Hardware accelerated YUV surfaces Enabled...\n");
						else
						{
							g_bSupportsYUY2 = false;
							DbgPrintf("EmuD3D8: Hardware accelerated YUV surfaces Disabled...\n");
						}
					}
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
					if (FAILED(hRet))
					{
						CxbxKrnlCleanup("Could not create primary surface (0x%.08X)", hRet);
						g_bSupportsYUY2 = false;
					}
                }

                // update render target cache
                g_pCachedRenderTarget = EmuNewD3DSurface();
                g_pCachedRenderTarget->Data = X_D3DRESOURCE_DATA_RENDER_TARGET;
                g_pD3DDevice8->GetRenderTarget(&g_pCachedRenderTarget->EmuSurface8);

                // update z-stencil surface cache
                g_pCachedDepthStencil = EmuNewD3DSurface();
                g_pCachedDepthStencil->Data = X_D3DRESOURCE_DATA_DEPTH_STENCIL;
				g_bHasDepthStencil = SUCCEEDED(g_pD3DDevice8->GetDepthStencilSurface(&g_pCachedDepthStencil->EmuSurface8));
                (void)g_pD3DDevice8->CreateVertexBuffer
                (
                    1, 0, 0, XTL::D3DPOOL_MANAGED,
                    &g_pDummyBuffer
                );

                for(int Streams = 0; Streams < 8; Streams++)
                {
                    g_pD3DDevice8->SetStreamSource(Streams, g_pDummyBuffer, 1);
                }

                // initially, show a black screen
                // Only clear depth buffer and stencil if present
                //
                // Avoids following DirectX Debug Runtime error report
                //    [424] Direct3D8: (ERROR) :Invalid flag D3DCLEAR_ZBUFFER: no zbuffer is associated with device. Clear failed. 
                //
                g_pD3DDevice8->Clear(
					/*Count=*/0, 
					/*pRects=*/nullptr, 
					D3DCLEAR_TARGET | (g_bHasDepthStencil ? D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL : 0),
					/*Color=*/0xFF000000, // TODO : Use constant for this
					/*Z=*/g_bHasDepthStencil ? 1.0f : 0.0f,
					/*Stencil=*/0);
				g_pD3DDevice8->BeginScene();
				g_pD3DDevice8->EndScene();
                g_pD3DDevice8->Present(0, 0, 0, 0);

                // begin scene
                g_pD3DDevice8->BeginScene();

                // signal completion
                g_EmuCDPD.bReady = false;
            }
            else
            {
                // release direct3d
                if(g_pD3DDevice8 != 0)
                {
                    DbgPrintf("EmuD3D8: CreateDevice proxy thread releasing old Device.\n");

                    g_pD3DDevice8->EndScene();

                    g_EmuCDPD.hRet = g_pD3DDevice8->Release();

                    if(g_EmuCDPD.hRet == 0)
                        g_pD3DDevice8 = 0;
                }

				// cleanup overlay clipper
				if (g_pDDClipper != 0)
				{
					g_pDDClipper->Release();
					g_pDDClipper = 0;
				}

				// cleanup overlay surface
				if (g_pDDSOverlay7 != 0)
				{
					g_pDDSOverlay7->Release();
					g_pDDSOverlay7 = 0;
				}

				// cleanup directdraw surface
                if(g_pDDSPrimary != 0)
                {
                    g_pDDSPrimary->Release();
                    g_pDDSPrimary = 0;
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
    if((pResource->Data & X_D3DRESOURCE_DATA_FLAG_SPECIAL) == X_D3DRESOURCE_DATA_FLAG_SPECIAL)
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

    XTL::EMUPATCH(D3DResource_Register)(pResource, 0/*(PVOID)pResource->Data*/);
        

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

// Derived from EmuUnswizzleActiveTexture
static void EmuUnswizzleTextureStages()
{
	for( int i = 0; i < 4; i++ )
	{
		// for current usages, we're always on stage 0
		XTL::X_D3DPixelContainer *pPixelContainer = (XTL::X_D3DPixelContainer*)XTL::EmuD3DActiveTexture[i];

		if(pPixelContainer == NULL || !(pPixelContainer->Common & X_D3DCOMMON_ISLOCKED))
			return;

		XTL::X_D3DFORMAT XBFormat = (XTL::X_D3DFORMAT)((pPixelContainer->Format & X_D3DFORMAT_FORMAT_MASK) >> X_D3DFORMAT_FORMAT_SHIFT);
		DWORD dwBPP = 0;

		if(!XTL::EmuXBFormatIsSwizzled(XBFormat, &dwBPP))
			return;

		// remove lock
		pPixelContainer->EmuTexture8->UnlockRect(0);
		pPixelContainer->Common &= ~X_D3DCOMMON_ISLOCKED;

		// TODO: potentially XXHash32::hash() to see if this surface was actually modified..

		//
		// unswizzle texture
		//

		{
			XTL::IDirect3DTexture8 *pTexture = pPixelContainer->EmuTexture8;

			DWORD dwLevelCount = pTexture->GetLevelCount();

			for(uint32 v=0;v<dwLevelCount;v++)
			{
				XTL::D3DSURFACE_DESC SurfaceDesc;

				HRESULT hRet = pTexture->GetLevelDesc(v, &SurfaceDesc);

				if(FAILED(hRet))
					continue;

				//
				// perform unswizzle
				//

				{
					XTL::D3DLOCKED_RECT LockedRect;

					//if(SurfaceDesc.Format != XTL::D3DFMT_A8R8G8B8)
					//    break;
					//CxbxKrnlCleanup("Temporarily unsupported format for active texture unswizzle (0x%.08X)", SurfaceDesc.Format);

					hRet = pTexture->LockRect(v, &LockedRect, NULL, NULL);

					if(FAILED(hRet))
						continue;

					DWORD dwWidth = SurfaceDesc.Width;
					DWORD dwHeight = SurfaceDesc.Height;
					DWORD dwDepth = 1;
					DWORD dwPitch = LockedRect.Pitch;
					RECT  iRect = {0,0,0,0};
					POINT iPoint = {0,0};

					void *pTemp = malloc(dwHeight*dwPitch);

					XTL::EmuUnswizzleRect
					(
						LockedRect.pBits, dwWidth, dwHeight, dwDepth,
						pTemp, dwPitch, iRect, iPoint, dwBPP
					);

					memcpy(LockedRect.pBits, pTemp, dwPitch*dwHeight);

					pTexture->UnlockRect(0);

					free(pTemp);
				}
			}

			DbgPrintf("Texture Stage %d was unswizzled\n", i);
		}
	}
}

// ******************************************************************
// * patch: Direct3D_CreateDevice
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(Direct3D_CreateDevice)
(
    UINT                        Adapter,
    D3DDEVTYPE                  DeviceType,
    HWND                        hFocusWindow,
    DWORD                       BehaviorFlags,
    X_D3DPRESENT_PARAMETERS    *pPresentationParameters,
    IDirect3DDevice8          **ppReturnedDeviceInterface
)
{
    

    DbgPrintf("EmuD3D8: EmuIDirect3D8_CreateDevice\n"
           "(\n"
           "   Adapter                   : 0x%.08X\n"
           "   DeviceType                : 0x%.08X\n"
           "   hFocusWindow              : 0x%.08X\n"
           "   BehaviorFlags             : 0x%.08X\n"
           "   pPresentationParameters   : 0x%.08X\n"
           "   ppReturnedDeviceInterface : 0x%.08X\n"
           ");\n",
           Adapter, DeviceType, hFocusWindow,
           BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

	// Print a few of the pPresentationParameters contents to the console
	DbgPrintf("BackBufferWidth:        = %d\n"
			  "BackBufferHeight:       = %d\n"
			  "BackBufferFormat:       = 0x%.08X\n"
			  "BackBufferCount:        = 0x%.08X\n"
			  "SwapEffect:             = 0x%.08X\n"
			  "EnableAutoDepthStencil: = 0x%.08X\n"
			  "AutoDepthStencilFormat: = 0x%.08X\n"
			  "Flags:                  = 0x%.08X\n\n",
			  pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight,
			  pPresentationParameters->BackBufferFormat, pPresentationParameters->BackBufferCount,
			  pPresentationParameters->SwapEffect, pPresentationParameters->EnableAutoDepthStencil,
			  pPresentationParameters->AutoDepthStencilFormat, pPresentationParameters->Flags );

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

    

    return g_EmuCDPD.hRet;
}

// ******************************************************************
// * patch: IDirect3DResource8_IsBusy
// ******************************************************************
BOOL WINAPI XTL::EMUPATCH(D3DDevice_IsBusy)()
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_IsBusy();\n");

    EmuWarning("EmuD3DDevice_IsBusy ignored!");

    

    return FALSE;
}

// ******************************************************************
// * patch: D3DDevice_GetCreationParameters
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetCreationParameters)(D3DDEVICE_CREATION_PARAMETERS *pParameters)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_GetCreationParameters\n"
           "(\n"
           "   pParameters               : 0x%.08X\n"
           ");\n",
           pParameters);

    pParameters->AdapterOrdinal = D3DADAPTER_DEFAULT;
    pParameters->DeviceType = D3DDEVTYPE_HAL;
    pParameters->hFocusWindow = NULL;
    pParameters->BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;

    

    return;
}

// ******************************************************************
// * patch: D3D_CheckDeviceFormat
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3D_CheckDeviceFormat)
(
    UINT                        Adapter,
    D3DDEVTYPE                  DeviceType,
    X_D3DFORMAT                 AdapterFormat,
    DWORD                       Usage,
    X_D3DRESOURCETYPE           RType,
    X_D3DFORMAT                 CheckFormat
)
{
    

    DbgPrintf("EmuD3D8: EmuIDirect3D8_CheckDeviceFormat\n"
           "(\n"
           "   Adapter                   : 0x%.08X\n"
           "   DeviceType                : 0x%.08X\n"
           "   AdapterFormat             : 0x%.08X\n"
           "   Usage                     : 0x%.08X\n"
           "   RType                     : 0x%.08X\n"
           "   CheckFormat               : 0x%.08X\n"
           ");\n",
           Adapter, DeviceType, AdapterFormat,
           Usage, RType, CheckFormat);

    if(RType > 7)
        CxbxKrnlCleanup("RType > 7");

	// HACK: Return true for everything? (Hunter the Reckoning)

    HRESULT hRet = S_OK; /*g_pD3D8->CheckDeviceFormat
    (
        g_XBVideo.GetDisplayAdapter(), (g_XBVideo.GetDirect3DDevice() == 0) ? XTL::D3DDEVTYPE_HAL : XTL::D3DDEVTYPE_REF,
        EmuXB2PC_D3DFormat(AdapterFormat), Usage, (D3DRESOURCETYPE)RType, EmuXB2PC_D3DFormat(CheckFormat)
    );*/

    

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_GetDisplayFieldStatus
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetDisplayFieldStatus)(X_D3DFIELD_STATUS *pFieldStatus)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_GetDisplayFieldStatus\n"
           "(\n"
           "   pFieldStatus              : 0x%.08X\n"
           ");\n",
           pFieldStatus);

#if 1
    pFieldStatus->Field = (g_VBData.VBlank%2 == 0) ? X_D3DFIELD_ODD : X_D3DFIELD_EVEN;
    pFieldStatus->VBlankCount = g_VBData.VBlank;
#else
	pFieldStatus->Field = X_D3DFIELD_PROGRESSIVE;
	pFieldStatus->VBlankCount = 0;
#endif

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_BeginPush
// ******************************************************************
PDWORD WINAPI XTL::EMUPATCH(D3DDevice_BeginPush)(DWORD Count)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_BeginPush(%d);\n", Count);

    DWORD *pRet = new DWORD[Count];

    g_dwPrimaryPBCount = Count;
    g_pPrimaryPB = pRet;

    

    return pRet;
}

// ******************************************************************
// * patch: D3DDevice_EndPush
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_EndPush)(DWORD *pPush)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_EndPush(0x%.08X);\n", pPush);

#ifdef _DEBUG_TRACK_PB
//	DbgDumpPushBuffer(g_pPrimaryPB, g_dwPrimaryPBCount*sizeof(DWORD));
#endif

    EmuExecutePushBufferRaw(g_pPrimaryPB);

    delete[] g_pPrimaryPB;

    g_pPrimaryPB = 0;

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_BeginVisibilityTest
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_BeginVisibilityTest)()
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_BeginVisibilityTest();\n");

    

    return D3D_OK;
}

// ******************************************************************
// * patch: D3DDevice_EndVisibilityTest
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_EndVisibilityTest)
(
    DWORD                       Index
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_EndVisibilityTest\n"
           "(\n"
           "   Index                     : 0x%.08X\n"
           ");\n",
           Index);

    

    return D3D_OK;
}

// ******************************************************************
// * patch: D3DDevice_SetBackBufferScale
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetBackBufferScale)(FLOAT x, FLOAT y)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetBackBufferScale\n"
           "(\n"
           "   x                         : %f\n"
           "   y                         : %f\n"
           ");\n",
           x, y);

    EmuWarning("SetBackBufferScale ignored");

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_GetVisibilityTestResult
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_GetVisibilityTestResult)
(
    DWORD                       Index,
    UINT                       *pResult,
    ULONGLONG                  *pTimeStamp
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_GetVisibilityTestResult\n"
           "(\n"
           "   Index                     : 0x%.08X\n"
           "   pResult                   : 0x%.08X\n"
           "   pTimeStamp                : 0x%.08X\n"
           ");\n",
           Index, pResult, pTimeStamp);

    // TODO: actually emulate this!?

    if(pResult != 0)
        *pResult = 640*480;

    if(pTimeStamp != 0)
        *pTimeStamp = 0;

    

    return D3D_OK;
}

// ******************************************************************
// * patch: D3DDevice_GetDeviceCaps
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetDeviceCaps)
(
    D3DCAPS8                   *pCaps
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_GetDeviceCaps\n"
           "(\n"
           "   pCaps                     : 0x%.08X\n"
           ");\n",
           pCaps);

    HRESULT hRet = g_pD3D8->GetDeviceCaps(g_XBVideo.GetDisplayAdapter(), (g_XBVideo.GetDirect3DDevice() == 0) ? XTL::D3DDEVTYPE_HAL : XTL::D3DDEVTYPE_REF, pCaps);
	if(FAILED(hRet))
		CxbxKrnlCleanup("EmuD3DDevice_GetDeviceCaps failed!");

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_LoadVertexShader
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_LoadVertexShader)
(
    DWORD                       Handle,
    DWORD                       Address
)
{
    

    // debug trace
    DbgPrintf( "EmuD3D8: EmuD3DDevice_LoadVertexShader\n"
               "(\n"
               "   Handle              : 0x%.08X\n"
               "   Address             : 0x%.08X\n"
               ");\n",
               Handle,Address);

    if(Address < 136 && VshHandleIsVertexShader(Handle))
    {
        VERTEX_SHADER *pVertexShader = (VERTEX_SHADER *)(VshHandleGetVertexShader(Handle))->Handle;
        for (DWORD i = Address; i < pVertexShader->Size; i++)
        {
            // TODO: This seems very fishy
            g_VertexShaderSlots[i] = Handle;
        }
    }

    

    return D3D_OK;
}

// ******************************************************************
// * patch: D3DDevice_SelectVertexShader
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SelectVertexShader)
(
    DWORD                       Handle,
    DWORD                       Address
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SelectVertexShader\n"
           "(\n"
           "   Handle              : 0x%.08X\n"
           "   Address             : 0x%.08X\n"
           ");\n",
           Handle, Address);

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

    

    return D3D_OK;
}

// ******************************************************************
// * patch: D3D_KickOffAndWaitForIdle
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3D_KickOffAndWaitForIdle)()
{
    

    DbgPrintf("EmuD3D8: EmuIDirect3D8_KickOffAndWaitForIdle()\n");

    // TODO: Actually do something here?

    

    return;
}

// ******************************************************************
// * patch: D3D_KickOffAndWaitForIdle2
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3D_KickOffAndWaitForIdle2)(DWORD dwDummy1, DWORD dwDummy2)
{
    

    DbgPrintf("EmuD3D8: EmuIDirect3D8_KickOffAndWaitForIdle\n"
           "(\n"
           "   dwDummy1            : 0x%.08X\n"
           "   dwDummy2            : 0x%.08X\n"
           ");\n",
           dwDummy1, dwDummy2);

    // TODO: Actually do something here?

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetGammaRamp
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetGammaRamp)
(
    DWORD                   dwFlags,
    CONST X_D3DGAMMARAMP   *pRamp
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetGammaRamp\n"
           "(\n"
           "   dwFlags             : 0x%.08X\n"
           "   pRamp               : 0x%.08X\n"
           ");\n",
           dwFlags, pRamp);

    // remove D3DSGR_IMMEDIATE
    DWORD dwPCFlags = dwFlags & (~0x00000002);
    D3DGAMMARAMP PCRamp;

    for(int v=0;v<255;v++)
    {
        PCRamp.red[v]   = pRamp->red[v];
        PCRamp.green[v] = pRamp->green[v];
        PCRamp.blue[v]  = pRamp->blue[v];
    }

//    g_pD3DDevice8->SetGammaRamp(dwPCFlags, &PCRamp);

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_AddRef
// ******************************************************************
ULONG WINAPI XTL::EMUPATCH(D3DDevice_AddRef)()
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_AddRef()\n");

    ULONG ret = g_pD3DDevice8->AddRef();

    

    return ret;
}

// ******************************************************************
// * patch: D3DDevice_BeginStateBlock
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_BeginStateBlock)()
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_BeginStateBlock()\n");

    ULONG ret = g_pD3DDevice8->BeginStateBlock();

    

    return ret;
}

/*// ******************************************************************
// * patch: D3DDevice_BeginStateBig
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_BeginStateBig)()
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_BeginStateBig()\n");

    //ULONG ret = g_pD3DDevice8->BeginStateBlock();

    CxbxKrnlCleanup("BeginStateBig is not implemented");

    

    return ret;
}*/

// ******************************************************************
// * patch: D3DDevice_CaptureStateBlock
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_CaptureStateBlock)(DWORD Token)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_CaptureStateBlock\n"
           "(\n"
           "   Token               : 0x%.08X\n"
           ");\n",
           Token);

    ULONG ret = g_pD3DDevice8->CaptureStateBlock(Token);

    

    return ret;
}

// ******************************************************************
// * patch: D3DDevice_ApplyStateBlock
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_ApplyStateBlock)(DWORD Token)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_ApplyStateBlock\n"
           "(\n"
           "   Token               : 0x%.08X\n"
           ");\n",
           Token);

    ULONG ret = g_pD3DDevice8->ApplyStateBlock(Token);

    

    return ret;
}

// ******************************************************************
// * patch: D3DDevice_EndStateBlock
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_EndStateBlock)(DWORD *pToken)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_EndStateBlock\n"
           "(\n"
           "   pToken              : 0x%.08X\n"
           ");\n",
           pToken);

    ULONG ret = g_pD3DDevice8->EndStateBlock(pToken);

    

    return ret;
}

// ******************************************************************
// * patch: D3DDevice_CopyRects
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_CopyRects)
(
    X_D3DSurface       *pSourceSurface,
    CONST RECT         *pSourceRectsArray,
    UINT                cRects,
    X_D3DSurface       *pDestinationSurface,
    CONST POINT        *pDestPointsArray
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_CopyRects\n"
           "(\n"
           "   pSourceSurface      : 0x%.08X\n"
           "   pSourceRectsArray   : 0x%.08X\n"
           "   cRects              : 0x%.08X\n"
           "   pDestinationSurface : 0x%.08X\n"
           "   pDestPointsArray    : 0x%.08X\n"
           ");\n",
           pSourceSurface, pSourceRectsArray, cRects,
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

    

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_CreateImageSurface
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_CreateImageSurface)
(
    UINT                Width,
    UINT                Height,
    X_D3DFORMAT         Format,
    X_D3DSurface      **ppSurface
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_CreateImageSurface\n"
           "(\n"
           "   Width               : 0x%.08X\n"
           "   Height              : 0x%.08X\n"
           "   Format              : 0x%.08X\n"
           "   ppSurface        : 0x%.08X\n"
           ");\n",
           Width, Height, Format, ppSurface);

    *ppSurface = EmuNewD3DSurface();

    D3DFORMAT PCFormat = EmuXB2PC_D3DFormat(Format);

    HRESULT hRet = g_pD3DDevice8->CreateImageSurface(Width, Height, PCFormat, &((*ppSurface)->EmuSurface8));
	if(FAILED(hRet))
		if(Format == X_D3DFMT_LIN_D24S8)
		{
			EmuWarning("CreateImageSurface: D3DFMT_LIN_D24S8 -> D3DFMT_A8R8G8B8");
			hRet = g_pD3DDevice8->CreateImageSurface(Width, Height, D3DFMT_A8R8G8B8, &((*ppSurface)->EmuSurface8));
		}
	
	if(FAILED(hRet))
		/*EmuWarning*/CxbxKrnlCleanup("CreateImageSurface failed!\nFormat = 0x%8.8X", Format);
  

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_GetGammaRamp
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetGammaRamp)
(
    X_D3DGAMMARAMP     *pRamp
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_GetGammaRamp\n"
           "(\n"
           "   pRamp               : 0x%.08X\n"
           ");\n",
           pRamp);

    D3DGAMMARAMP *pGammaRamp = (D3DGAMMARAMP *)malloc(sizeof(D3DGAMMARAMP));

    g_pD3DDevice8->GetGammaRamp(pGammaRamp);

    for(int v=0;v<256;v++)
    {
        pRamp->red[v] = (BYTE)pGammaRamp->red[v];
        pRamp->green[v] = (BYTE)pGammaRamp->green[v];
        pRamp->blue[v] = (BYTE)pGammaRamp->blue[v];
    }

    free(pGammaRamp);

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_GetBackBuffer2
// ******************************************************************
XTL::X_D3DSurface* WINAPI XTL::EMUPATCH(D3DDevice_GetBackBuffer2)
(
    INT                 BackBuffer
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_GetBackBuffer2\n"
           "(\n"
           "   BackBuffer          : 0x%.08X\n"
           ");\n",
           BackBuffer);

    /** unsafe, somehow
    HRESULT hRet = S_OK;

    X_D3DSurface *pBackBuffer = EmuNewD3DSurface();

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

    static X_D3DSurface *pBackBuffer = EmuNewD3DSurface();

    if(BackBuffer == -1)
        BackBuffer = 0;

    HRESULT hRet = g_pD3DDevice8->GetBackBuffer(BackBuffer, D3DBACKBUFFER_TYPE_MONO, &(pBackBuffer->EmuSurface8));

    if(FAILED(hRet))
        CxbxKrnlCleanup("Unable to retrieve back buffer");

    // update data pointer
    pBackBuffer->Data = X_D3DRESOURCE_DATA_BACK_BUFFER;

    

    return pBackBuffer;
}

// ******************************************************************
// * patch: D3DDevice_GetBackBuffer
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetBackBuffer)
(
    INT                 BackBuffer,
    D3DBACKBUFFER_TYPE  Type,
    X_D3DSurface      **ppBackBuffer
)
{
        DbgPrintf("EmuD3D8: EmuD3DDevice_GetBackBuffer >>\n"
               "(\n"
               "   BackBuffer          : 0x%.08X\n"
               "   Type                : 0x%.08X\n"
               "   ppBackBuffer        : 0x%.08X\n"
               ");\n",
               BackBuffer, Type, ppBackBuffer);

    *ppBackBuffer = EMUPATCH(D3DDevice_GetBackBuffer2)(BackBuffer);

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetViewport
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetViewport)
(
    CONST D3DVIEWPORT8 *pViewport
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetViewport\n"
           "(\n"
           "   pViewport           : 0x%.08X (%d, %d, %d, %d, %f, %f)\n"
           ");\n",
           pViewport, pViewport->X, pViewport->Y, pViewport->Width,
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

    

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_GetViewport
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_GetViewport)
(
    D3DVIEWPORT8 *pViewport
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_GetViewport\n"
           "(\n"
           "   pViewport           : 0x%.08X\n"
           ");\n",
           pViewport);

    HRESULT hRet = g_pD3DDevice8->GetViewport(pViewport);

    if(FAILED(hRet))
    {
        EmuWarning("Unable to get viewport!");
        hRet = D3D_OK;
    }

    

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_GetViewportOffsetAndScale
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetViewportOffsetAndScale)
(
    D3DXVECTOR4 *pOffset,
    D3DXVECTOR4 *pScale
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_GetViewportOffsetAndScale\n"
           "(\n"
           "   pOffset             : 0x%.08X\n"
           "   pScale              : 0x%.08X\n"
           ");\n",
           pOffset, pScale);

    float fScaleX = 1.0f;
    float fScaleY = 1.0f;
    float fScaleZ = 1.0f;
    float fOffsetX = 0.5 + 1.0/32;
    float fOffsetY = 0.5 + 1.0/32;
    D3DVIEWPORT8 Viewport;

    
	EMUPATCH(D3DDevice_GetViewport)(&Viewport);
    


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

    
}

// ******************************************************************
// * patch: D3DDevice_SetShaderConstantMode
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetShaderConstantMode)
(
    XTL::X_VERTEXSHADERCONSTANTMODE Mode
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetShaderConstantMode\n"
           "(\n"
           "   Mode                : 0x%.08X\n"
           ");\n",
           Mode);

    g_VertexShaderConstantMode = Mode;

    

    return S_OK;
}

// ******************************************************************
// * patch: D3DDevice_Reset
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_Reset)
(
    X_D3DPRESENT_PARAMETERS *pPresentationParameters
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_Reset\n"
           "(\n"
           "   pPresentationParameters  : 0x%.08X\n"
           ");\n",
           pPresentationParameters);

    EmuWarning("Device Reset is being utterly ignored");

    

    return D3D_OK;
}

// ******************************************************************
// * patch: D3DDevice_GetRenderTarget
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_GetRenderTarget)
(
    X_D3DSurface  **ppRenderTarget
)
{
    DbgPrintf("EmuD3D8: EmuD3DDevice_GetRenderTarget >>\n"
           "(\n"
           "   ppRenderTarget      : 0x%.08X\n"
           ");\n",
           ppRenderTarget);

	*ppRenderTarget = EMUPATCH(D3DDevice_GetRenderTarget2)();
    return D3D_OK;
}

// ******************************************************************
// * patch: D3DDevice_GetRenderTarget2
// ******************************************************************
XTL::X_D3DSurface * WINAPI XTL::EMUPATCH(D3DDevice_GetRenderTarget2)()
{
    DbgPrintf("EmuD3D8: EmuD3DDevice_GetRenderTarget2()\n");

	X_D3DSurface *result = g_pCachedRenderTarget;

	if (result != NULL)
	{
		IDirect3DSurface8 *pSurface8 = result->EmuSurface8;
		if (pSurface8 != nullptr)
			pSurface8->AddRef();

		result->Common++; // AddRef
	}

    DbgPrintf("EmuD3D8: RenderTarget := 0x%.08X\n", result);

    return result;
}

// ******************************************************************
// * patch: D3DDevice_GetDepthStencilSurface
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_GetDepthStencilSurface)
(
    X_D3DSurface  **ppZStencilSurface
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_GetDepthStencilSurface\n"
           "(\n"
           "   ppZStencilSurface   : 0x%.08X\n"
           ");\n",
           ppZStencilSurface);

    IDirect3DSurface8 *pSurface8 = g_pCachedDepthStencil->EmuSurface8;

    if(pSurface8 != 0)
        pSurface8->AddRef();

    *ppZStencilSurface = g_pCachedDepthStencil;

    DbgPrintf("EmuD3D8: DepthStencilSurface := 0x%.08X\n", pSurface8);

    

    return D3D_OK;
}

// ******************************************************************
// * patch: D3DDevice_GetDepthStencilSurface2
// ******************************************************************
XTL::X_D3DSurface * WINAPI XTL::EMUPATCH(D3DDevice_GetDepthStencilSurface2)()
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_GetDepthStencilSurface2()\n");

    IDirect3DSurface8 *pSurface8 = g_pCachedDepthStencil->EmuSurface8;

    if(pSurface8 != 0)
        pSurface8->AddRef();

    DbgPrintf("EmuD3D8: DepthStencilSurface := 0x%.08X\n", pSurface8);

    

    return g_pCachedDepthStencil;
}

// ******************************************************************
// * patch: D3DDevice_GetTile
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_GetTile)
(
    DWORD           Index,
    X_D3DTILE      *pTile
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_GetTile\n"
           "(\n"
           "   Index               : 0x%.08X\n"
           "   pTile               : 0x%.08X\n"
           ");\n",
           Index, pTile);

    if(pTile != NULL)
        memcpy(pTile, &EmuD3DTileCache[Index], sizeof(X_D3DTILE));

    

    return D3D_OK;
}

// ******************************************************************
// * patch: D3DDevice_SetTileNoWait
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetTileNoWait)
(
    DWORD               Index,
    CONST X_D3DTILE    *pTile
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetTileNoWait\n"
           "(\n"
           "   Index               : 0x%.08X\n"
           "   pTile               : 0x%.08X\n"
           ");\n",
           Index, pTile);

    if(pTile != NULL)
        memcpy(&EmuD3DTileCache[Index], pTile, sizeof(X_D3DTILE));

    

    return D3D_OK;
}

// ******************************************************************
// * patch: D3DDevice_CreateVertexShader
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_CreateVertexShader)
(
    CONST DWORD    *pDeclaration,
    CONST DWORD    *pFunction,
    DWORD          *pHandle,
    DWORD           Usage
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_CreateVertexShader\n"
           "(\n"
           "   pDeclaration        : 0x%.08X\n"
           "   pFunction           : 0x%.08X\n"
           "   pHandle             : 0x%.08X\n"
           "   Usage               : 0x%.08X\n"
           ");\n",
           pDeclaration, pFunction, pHandle, Usage);

    // create emulated shader struct
    X_D3DVertexShader *pD3DVertexShader = (X_D3DVertexShader*)CxbxCalloc(1, sizeof(X_D3DVertexShader));
    VERTEX_SHADER     *pVertexShader = (VERTEX_SHADER*)CxbxCalloc(1, sizeof(VERTEX_SHADER));

    // TODO: Intelligently fill out these fields as necessary

    // HACK: TODO: support this situation
    if(pDeclaration == NULL)
    {
        *pHandle = NULL;

        

        return S_OK;
    }

    LPD3DXBUFFER pRecompiledBuffer = NULL;
    DWORD        *pRecompiledDeclaration;
    DWORD        *pRecompiledFunction = NULL;
    DWORD        VertexShaderSize = 0;
    DWORD        DeclarationSize;
    DWORD        Handle = 0;

    HRESULT hRet = XTL::EmuRecompileVshDeclaration((DWORD*)pDeclaration,
                                                   &pRecompiledDeclaration,
                                                   &DeclarationSize,
                                                   pFunction == NULL,
                                                   &pVertexShader->VertexDynamicPatch);

    if(SUCCEEDED(hRet) && pFunction)
    {
		boolean bUseDeclarationOnly = 0;

        hRet = XTL::EmuRecompileVshFunction((DWORD*)pFunction,
                                            &pRecompiledBuffer,
                                            &VertexShaderSize,
                                            g_VertexShaderConstantMode == X_VSCM_NONERESERVED,
											&bUseDeclarationOnly);
        if(SUCCEEDED(hRet))
        {
			if(!bUseDeclarationOnly)
				pRecompiledFunction = (DWORD*)pRecompiledBuffer->GetBufferPointer();
			else
				pRecompiledFunction = NULL;
        }
        else
        {
            pRecompiledFunction = NULL;
            EmuWarning("Couldn't recompile vertex shader function.");
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

        if(pRecompiledBuffer != nullptr)
        {
            pRecompiledBuffer->Release();
            pRecompiledBuffer = nullptr;
        }

        //* Fallback to dummy shader.
        if(FAILED(hRet))
        {
            static const char dummy[] =
                "vs.1.1\n"
                "mov oPos, v0\n";

            EmuWarning("Trying fallback:\n%s", dummy);
            hRet = D3DXAssembleShader(dummy,
                                      strlen(dummy),
                                      D3DXASM_SKIPVALIDATION,
                                      NULL,
                                      &pRecompiledBuffer,
                                      NULL);
            hRet = g_pD3DDevice8->CreateVertexShader
            (
                pRecompiledDeclaration,
                (DWORD*)pRecompiledBuffer->GetBufferPointer(),
                &Handle,
                g_dwVertexShaderUsage
            );
        }
        //*/
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

    

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_SetPixelShaderConstant
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetPixelShaderConstant)
(
    DWORD       Register,
    CONST PVOID pConstantData,
    DWORD       ConstantCount
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetPixelShaderConstant\n"
           "(\n"
           "   Register            : 0x%.08X\n"
           "   pConstantData       : 0x%.08X\n"
           "   ConstantCount       : 0x%.08X\n"
           ");\n",
           Register, pConstantData, ConstantCount);

	// TODO: This hack is necessary for Vertex Shaders on XDKs prior to 4361, but if this
	// causes problems with pixel shaders, feel free to comment out the hack below.
	if(g_BuildVersion <= 4361)
		Register += 96;

    HRESULT hRet = g_pD3DDevice8->SetPixelShaderConstant
    (
        Register,
        pConstantData,
        ConstantCount
    );

    if(FAILED(hRet))
    {
        EmuWarning("We're lying about setting a pixel shader constant!");

        hRet = D3D_OK;
    }

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderConstant
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetVertexShaderConstant)
(
    INT         Register,
    CONST PVOID pConstantData,
    DWORD       ConstantCount
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetVertexShaderConstant\n"
           "(\n"
           "   Register            : 0x%.08X\n"
           "   pConstantData       : 0x%.08X\n"
           "   ConstantCount       : 0x%.08X\n"
           ");\n",
           Register, pConstantData, ConstantCount);

/*#ifdef _DEBUG_TRACK_VS_CONST
    for (uint32 i = 0; i < ConstantCount; i++)
    {
        printf("SetVertexShaderConstant, c%d (c%d) = { %f, %f, %f, %f }\n",
               Register - 96 + i, Register + i,
               *((float*)pConstantData + 4 * i),
               *((float*)pConstantData + 4 * i + 1),
               *((float*)pConstantData + 4 * i + 2),
               *((float*)pConstantData + 4 * i + 3));
    }
#endif*/ // _DEBUG_TRACK_VS_CONST

	// TODO: HACK: Since Xbox vertex shader constants range from -96 to 96, during conversion
	// some shaders need to add 96 to use ranges 0 to 192.  This fixes 3911 - 4361 games and XDK
	// samples, but breaks Turok.

	if(g_BuildVersion <= 4361)
		Register += 96;

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

    

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderConstant1
// ******************************************************************
VOID __fastcall XTL::EMUPATCH(D3DDevice_SetVertexShaderConstant1)
(
    INT         Register,
    CONST PVOID pConstantData
)
{
        DbgPrintf("EmuD3D8: EmuD3DDevice_SetVertexShaderConstant1\n"
               "(\n"
               "   Register            : 0x%.08X\n"
               "   pConstantData       : 0x%.08X\n"
               ");\n",
               Register, pConstantData);

    XTL::EMUPATCH(D3DDevice_SetVertexShaderConstant)(Register, pConstantData, 1);

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderConstant4
// ******************************************************************
VOID __fastcall XTL::EMUPATCH(D3DDevice_SetVertexShaderConstant4)
(
    INT         Register,
    CONST PVOID pConstantData
)
{
        DbgPrintf("EmuD3D8: EmuD3DDevice_SetVertexShaderConstant4\n"
               "(\n"
               "   Register            : 0x%.08X\n"
               "   pConstantData       : 0x%.08X\n"
               ");\n",
               Register, pConstantData);

		XTL::EMUPATCH(D3DDevice_SetVertexShaderConstant)(Register, pConstantData, 4);

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderConstantNotInline
// ******************************************************************
VOID __fastcall XTL::EMUPATCH(D3DDevice_SetVertexShaderConstantNotInline)
(
    INT         Register,
    CONST PVOID pConstantData,
    DWORD       ConstantCount
)
{
        DbgPrintf("EmuD3D8: EmuD3DDevice_SetVertexShaderConstantNotInline\n"
               "(\n"
               "   Register            : 0x%.08X\n"
               "   pConstantData       : 0x%.08X\n"
               "   ConstantCount       : 0x%.08X\n"
               ");\n",
               Register, pConstantData, ConstantCount);

		XTL::EMUPATCH(D3DDevice_SetVertexShaderConstant)(Register, pConstantData, ConstantCount / 4);

    return;
}

// ******************************************************************
// * patch: D3DDevice_DeletePixelShader
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_DeletePixelShader)
(
    DWORD          Handle
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_DeletePixelShader\n"
           "(\n"
           "   Handle              : 0x%.08X\n"
           ");\n",
           Handle);

    if(Handle == X_PIXELSHADER_FAKE_HANDLE)
    {
        // Do Nothing!
    }
    else
    {
        g_pD3DDevice8->DeletePixelShader(Handle);
    }

	/*PIXEL_SHADER *pPixelShader = (PIXEL_SHADER*)Handle;

	if (pPixelShader)
	{
		if(pPixelShader->Handle != X_PIXELSHADER_FAKE_HANDLE)
		{
			g_pD3DDevice8->DeletePixelShader(pPixelShader->Handle);
		}
		CxbxFree(pPixelShader);
	}*/

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_CreatePixelShader
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_CreatePixelShader)
(
    X_D3DPIXELSHADERDEF    *pPSDef,
    DWORD				   *pHandle
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_CreatePixelShader\n"
           "(\n"
           "   pPSDef              : 0x%.08X\n"
           "   pHandle             : 0x%.08X (0x%.08X)\n"
           ");\n",
           pPSDef, pHandle, *pHandle);

	HRESULT hRet = E_FAIL;
	DWORD* pFunction = NULL;
	LPD3DXBUFFER pRecompiledBuffer = NULL;
	// DWORD Handle = 0;

	hRet = CreatePixelShaderFunction(pPSDef, &pRecompiledBuffer);

	if (SUCCEEDED(hRet))
	{
		pFunction = (DWORD*)pRecompiledBuffer->GetBufferPointer();

		// Redirect to Windows D3D
		hRet = g_pD3DDevice8->CreatePixelShader
		(
			pFunction,
			pHandle
			/*&Handle*/
		);
	}

	if (pRecompiledBuffer != nullptr)
	{
		pRecompiledBuffer->Release();
	}

	// This additional layer of Cxbx internal indirection seems problematic, as 
	// CreatePixelShader() is expected to return a pHandle directly to a shader interface.

	/*
	PIXEL_SHADER *pPixelShader = (PIXEL_SHADER*)CxbxCalloc(1, sizeof(PIXEL_SHADER)); // Clear, to prevent side-effects on random contents

	memcpy(&pPixelShader->PSDef, pPSDef, sizeof(X_D3DPIXELSHADERDEF));

	pPixelShader->Handle = Handle;
	pPixelShader->dwStatus = hRet;
	*pHandle = (DWORD)pPixelShader;
	*/

#if 0
    // ================================================================
	pFunction = (DWORD*) pPSDef;

	// Attempt to recompile PixelShader
	EmuRecompilePshDef( pPSDef, NULL );

    // redirect to windows d3d
    hRet = g_pD3DDevice8->CreatePixelShader
    (
        pFunction,
        pHandle
    );
    // ================================================================
#endif

    if(FAILED(hRet))
    {
        *pHandle = X_PIXELSHADER_FAKE_HANDLE;

		// This is called too frequently as Azurik creates and destroys a
		// pixel shader every frame, and makes debugging harder.
		// EmuWarning("We're lying about the creation of a pixel shader!");

        hRet = D3D_OK;
    }
    else
    {
        DbgPrintf("pHandle = 0x%.08X (0x%.08X)\n", pHandle, *pHandle);
    }

    

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_SetPixelShader
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetPixelShader)
(
    DWORD           Handle
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetPixelShader\n"
           "(\n"
           "   Handle              : 0x%.08X\n"
           ");\n",
           Handle);

    // Redirect to Windows D3D
    HRESULT hRet = D3D_OK;

    // Fake Programmable Pipeline
    if(Handle == X_PIXELSHADER_FAKE_HANDLE)
    {
        // programmable pipeline
        //*
        static DWORD dwHandle = 0;

        if(dwHandle == 0)
        {
            // simplest possible pixel shader, simply output the texture input
            static const char szDiffusePixelShader[] =
                "ps.1.0\n"
                "tex t0\n"
                "mov r0, t0\n";

            LPD3DXBUFFER pShader = 0;
            LPD3DXBUFFER pErrors = 0;

            // assemble the shader
            D3DXAssembleShader(szDiffusePixelShader, strlen(szDiffusePixelShader) - 1, 0, NULL, &pShader, &pErrors);

            // create the shader device handle
            hRet = g_pD3DDevice8->CreatePixelShader((DWORD*)pShader->GetBufferPointer(), &dwHandle);

			g_dwCurrentPixelShader = 0;

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
    else if(Handle != NULL)
    {
        EmuWarning("Trying fixed or recompiled programmable pipeline pixel shader!");
        g_bFakePixelShaderLoaded = FALSE;
		g_dwCurrentPixelShader = Handle;
        hRet = g_pD3DDevice8->SetPixelShader(Handle);
    }

    if(FAILED(hRet))
    {
        EmuWarning("We're lying about setting a pixel shader!");

        hRet = D3D_OK;
    }

    

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_CreateTexture2
// ******************************************************************
XTL::X_D3DResource * WINAPI XTL::EMUPATCH(D3DDevice_CreateTexture2)
(
    UINT                Width,
    UINT                Height,
    UINT                Depth,
    UINT                Levels,
    DWORD               Usage,
    X_D3DFORMAT         Format,
    D3DRESOURCETYPE     D3DResource
)
{
    X_D3DTexture *pTexture = NULL;

    switch(D3DResource)
    {
        case 3: /*D3DRTYPE_TEXTURE*/
			EMUPATCH(D3DDevice_CreateTexture)(Width, Height, Levels, Usage, Format, D3DPOOL_MANAGED, &pTexture);
            break;
        case 4: /*D3DRTYPE_VOLUMETEXTURE*/
			EMUPATCH(D3DDevice_CreateVolumeTexture)(Width, Height, Depth, Levels, Usage, Format, D3DPOOL_MANAGED, (X_D3DVolumeTexture**)&pTexture);
            break;
        case 5: /*D3DRTYPE_CUBETEXTURE*/
            //DbgPrintf( "D3DDevice_CreateTexture2: Width = 0x%X, Height = 0x%X\n", Width, Height );
			//CxbxKrnlCleanup("Cube textures temporarily not supported!");
			EMUPATCH(D3DDevice_CreateCubeTexture)(Width, Levels, Usage, Format, D3DPOOL_MANAGED, (X_D3DCubeTexture**) &pTexture);
            break;
        default:
            CxbxKrnlCleanup("D3DResource = %d is not supported!", D3DResource);
    }

    return pTexture;
}

// ******************************************************************
// * patch: D3DDevice_CreateTexture
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_CreateTexture)
(
    UINT            Width,
    UINT            Height,
    UINT            Levels,
    DWORD           Usage,
    X_D3DFORMAT     Format,
    D3DPOOL         Pool,
    X_D3DTexture  **ppTexture
)
{
    DbgPrintf("EmuD3D8: EmuD3DDevice_CreateTexture\n"
           "(\n"
           "   Width               : 0x%.08X\n"
           "   Height              : 0x%.08X\n"
           "   Levels              : 0x%.08X\n"
           "   Usage               : 0x%.08X\n"
           "   Format              : 0x%.08X\n"
           "   Pool                : 0x%.08X\n"
           "   ppTexture           : 0x%.08X\n"
           ");\n",
           Width, Height, Levels, Usage, Format, Pool, ppTexture);

	// Get Bytes Per Pixel, for correct Pitch calculation :
	DWORD dwBPP = EmuXBFormatBPP(Format);

	UINT Pitch = RoundUp(Width, 64) * dwBPP; // TODO : RoundUp only for (X_)D3DFMT_YUY2?

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
        PCFormat = D3DFMT_L8;
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
        g_dwOverlayP = Pitch;
    }

    HRESULT hRet;

	X_D3DTexture *pTexture = EmuNewD3DTexture();
	DWORD Texture_Data;

    if(PCFormat == D3DFMT_YUY2)
    {
        DWORD dwSize = g_dwOverlayP*g_dwOverlayH;
        DWORD dwPtr = (DWORD)CxbxMalloc(dwSize + sizeof(DWORD));
        DWORD *pRefCount = (DWORD*)(dwPtr + dwSize);

        // initialize ref count
        *pRefCount = 1;

        // If YUY2 is not supported in hardware, we'll actually mark this as a special fake texture (set highest bit)
        Texture_Data = X_D3DRESOURCE_DATA_YUV_SURFACE;
        pTexture->Lock = dwPtr;

        g_pCachedYuvSurface = (X_D3DSurface*)pTexture;

        hRet = D3D_OK;
    }
    else
    {
        DWORD   PCUsage = Usage & (D3DUSAGE_RENDERTARGET);
//        DWORD   PCUsage = Usage & (D3DUSAGE_RENDERTARGET | D3DUSAGE_DEPTHSTENCIL);
        D3DPOOL PCPool  = D3DPOOL_MANAGED;

		// DIRTY HACK: Render targets. The D3DUSAGE_RENDERTARGET
		// flag isn't always set by the XDK (if ever).
		/*if( Width != 640 && Height != 480 )
		{
		//	EmuAdjustPower2(&Width, &Height);
		}
		else
		{
			PCUsage = D3DUSAGE_RENDERTARGET;
			PCPool = D3DPOOL_DEFAULT;
		}*/

//		EmuAdjustPower2(&Width, &Height);

//        if(Usage & (D3DUSAGE_RENDERTARGET | D3DUSAGE_DEPTHSTENCIL))
        if(Usage & (D3DUSAGE_RENDERTARGET))
        {
			PCPool = D3DPOOL_DEFAULT;
        }

        hRet = g_pD3DDevice8->CreateTexture
        (
            Width, Height, Levels,
            PCUsage,  // TODO: Xbox Allows a border to be drawn (maybe hack this in software ;[)
            PCFormat, PCPool, &(pTexture->EmuTexture8)
        );

        if(FAILED(hRet))
        {
            EmuWarning("CreateTexture Failed!");
			Texture_Data = 0xBEADBEAD;
        }
        else
        {
            /**
             * Note: If CreateTexture() called with D3DPOOL_DEFAULT then unable to Lock. 
             * It will cause an Error with the DirectX Debug runtime.
             *
             * This is most commonly seen with
             *      D3DUSAGE_RENDERTARGET or
             *      D3DUSAGE_DEPTHSTENCIL
             * that can only be used with D3DPOOL_DEFAULT per MSDN.
             */
            D3DLOCKED_RECT LockedRect;

            pTexture->EmuTexture8->LockRect(0, &LockedRect, NULL, NULL);
			Texture_Data = (DWORD)LockedRect.pBits;
            g_DataToTexture.insert(Texture_Data, pTexture);
            pTexture->EmuTexture8->UnlockRect(0);
        }
    }

	// Set all X_D3DTexture members (except Lock)
	XTL::EMUPATCH(XGSetTextureHeader)(Width, Height, Levels, Usage, Format, Pool, pTexture, Texture_Data, Pitch);

	DbgPrintf("EmuD3D8: Created Texture : 0x%.08X (0x%.08X)\n", pTexture, pTexture->EmuTexture8);

	*ppTexture = pTexture;

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_CreateVolumeTexture
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_CreateVolumeTexture)
(
    UINT                 Width,
    UINT                 Height,
    UINT                 Depth,
    UINT                 Levels,
    DWORD                Usage,
    X_D3DFORMAT          Format,
    D3DPOOL              Pool,
    X_D3DVolumeTexture **ppVolumeTexture
)
{
    DbgPrintf("EmuD3D8: EmuD3DDevice_CreateVolumeTexture\n"
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
           Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture);

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
        PCFormat = D3DFMT_L8;
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

    if(PCFormat == D3DFMT_YUY2)
    {
        DWORD dwSize = g_dwOverlayP*g_dwOverlayH;
        DWORD dwPtr = (DWORD)CxbxMalloc(dwSize + sizeof(DWORD));
        DWORD *pRefCount = (DWORD*)(dwPtr + dwSize);

        // initialize ref count
        *pRefCount = 1;

        // If YUY2 is not supported in hardware, we'll actually mark this as a special fake texture (set highest bit)
        (*ppVolumeTexture)->Data = X_D3DRESOURCE_DATA_YUV_SURFACE;
        (*ppVolumeTexture)->Lock = dwPtr;
		(*ppVolumeTexture)->Format = Format; // TODO : apply << X_D3DFORMAT_FORMAT_SHIFT here?

        (*ppVolumeTexture)->Size = (g_dwOverlayW & X_D3DSIZE_WIDTH_MASK)
                                 | (g_dwOverlayH << X_D3DSIZE_HEIGHT_SHIFT)
                                 | (g_dwOverlayP << X_D3DSIZE_PITCH_SHIFT);

        hRet = D3D_OK;
    }
    else
    {
        EmuAdjustPower2(&Width, &Height);

        *ppVolumeTexture = EmuNewD3DVolumeTexture();

        hRet = g_pD3DDevice8->CreateVolumeTexture
        (
            Width, Height, Depth, Levels,
            0,  // TODO: Xbox Allows a border to be drawn (maybe hack this in software ;[)
            PCFormat, D3DPOOL_MANAGED, &((*ppVolumeTexture)->EmuVolumeTexture8)
        );

        if(FAILED(hRet))
            EmuWarning("CreateVolumeTexture Failed! (0x%.08X)", hRet);

        DbgPrintf("EmuD3D8: Created Volume Texture : 0x%.08X (0x%.08X)\n", *ppVolumeTexture, (*ppVolumeTexture)->EmuVolumeTexture8);
    }

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_CreateCubeTexture
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_CreateCubeTexture)
(
    UINT                 EdgeLength,
    UINT                 Levels,
    DWORD                Usage,
    X_D3DFORMAT          Format,
    D3DPOOL              Pool,
    X_D3DCubeTexture  **ppCubeTexture
)
{
    DbgPrintf("EmuD3D8: EmuD3DDevice_CreateCubeTexture\n"
           "(\n"
           "   EdgeLength          : 0x%.08X\n"
           "   Levels              : 0x%.08X\n"
           "   Usage               : 0x%.08X\n"
           "   Format              : 0x%.08X\n"
           "   Pool                : 0x%.08X\n"
           "   ppCubeTexture       : 0x%.08X\n"
           ");\n",
           EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture);

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
        PCFormat = D3DFMT_L8;
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

    *ppCubeTexture = EmuNewD3DCubeTexture();

    HRESULT hRet = g_pD3DDevice8->CreateCubeTexture
    (
        EdgeLength, Levels,
        0,  // TODO: Xbox Allows a border to be drawn (maybe hack this in software ;[)
        PCFormat, D3DPOOL_MANAGED, &((*ppCubeTexture)->EmuCubeTexture8)
    );

    DbgPrintf("EmuD3D8: Created Cube Texture : 0x%.08X (0x%.08X)\n", *ppCubeTexture, (*ppCubeTexture)->EmuCubeTexture8);

    if(FAILED(hRet))
        EmuWarning("CreateCubeTexture Failed!");

    

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_CreateIndexBuffer
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_CreateIndexBuffer)
(
    UINT                 Length,
    DWORD                Usage,
    X_D3DFORMAT          Format,
    D3DPOOL              Pool,
    X_D3DIndexBuffer   **ppIndexBuffer
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_CreateIndexBuffer\n"
           "(\n"
           "   Length              : 0x%.08X\n"
           "   Usage               : 0x%.08X\n"
           "   Format              : 0x%.08X\n"
           "   Pool                : 0x%.08X\n"
           "   ppIndexBuffer       : 0x%.08X\n"
           ");\n",
           Length, Usage, Format, Pool, ppIndexBuffer);

    *ppIndexBuffer = EmuNewD3DIndexBuffer();

    HRESULT hRet = g_pD3DDevice8->CreateIndexBuffer
    (
        Length, NULL, D3DFMT_INDEX16, D3DPOOL_MANAGED, &((*ppIndexBuffer)->EmuIndexBuffer8)
    );

    DbgPrintf("EmuD3D8: EmuIndexBuffer8 := 0x%.08X\n", (*ppIndexBuffer)->EmuIndexBuffer8);

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

    

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_CreateIndexBuffer2
// ******************************************************************
XTL::X_D3DIndexBuffer * WINAPI XTL::EMUPATCH(D3DDevice_CreateIndexBuffer2)(UINT Length)
{
    X_D3DIndexBuffer *pIndexBuffer = NULL;

	EMUPATCH(D3DDevice_CreateIndexBuffer)
    (
        Length,
        NULL,
        X_D3DFMT_INDEX16,
        D3DPOOL_MANAGED,
        &pIndexBuffer
    );

    return pIndexBuffer;
}

BOOL g_bBadIndexData = FALSE;

// ******************************************************************
// * patch: D3DDevice_SetIndices
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetIndices)
(
    X_D3DIndexBuffer   *pIndexData,
    UINT                BaseVertexIndex
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetIndices\n"
           "(\n"
           "   pIndexData          : 0x%.08X\n"
           "   BaseVertexIndex     : 0x%.08X\n"
           ");\n",
           pIndexData, BaseVertexIndex);

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

//#if 0
	if(pIndexData != NULL)
	{
		DbgPrintf("EmuD3DDevice_SetIndcies(): pIndexData->EmuIndexBuffer8:= 0x%.08X\n", pIndexData->EmuIndexBuffer8 );
		DbgPrintf("EmuD3DDevice_SetIndcies(): pIndexData->Lock:= 0x%.08X\n", pIndexData->Lock );
	}

    g_dwBaseVertexIndex = BaseVertexIndex;

    if(pIndexData != 0)
    {
        g_pIndexBuffer = pIndexData;

        // HACK: Halo Hack
        if(pIndexData->Lock == 0x00840863)
            pIndexData->Lock = 0;

        EmuVerifyResourceIsRegistered(pIndexData);

		// HACK: Unreal Championship
		if((pIndexData->Lock & 0xFFFF0000) == 0x00490000 || (pIndexData->Lock & 0xF0000000) != 0x00000000 || 
			pIndexData->Lock == 0x10)
		{
			hRet = E_FAIL;
			goto fail;
		}

        IDirect3DIndexBuffer8 *pIndexBuffer = pIndexData->EmuIndexBuffer8;

        if(pIndexData->Lock != X_D3DRESOURCE_LOCK_FLAG_NOSIZE)
            hRet = g_pD3DDevice8->SetIndices(pIndexBuffer, BaseVertexIndex);
    }
    else
    {
        g_pIndexBuffer = 0;

        hRet = g_pD3DDevice8->SetIndices(0, BaseVertexIndex);
    }
//#endif
fail:
    

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_SetTexture
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetTexture)
(
    DWORD           Stage,
    X_D3DResource  *pTexture
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetTexture\n"
           "(\n"
           "   Stage               : 0x%.08X\n"
           "   pTexture            : 0x%.08X\n"
           ");\n",
           Stage, pTexture);

    IDirect3DBaseTexture8 *pBaseTexture8 = NULL;

    EmuD3DActiveTexture[Stage] = pTexture;

    if(pTexture != NULL)
    {
        EmuVerifyResourceIsRegistered(pTexture);

        if(pTexture->Data == X_D3DRESOURCE_DATA_YUV_SURFACE)
        {
            //
            // NOTE: TODO: This is almost a hack! :)
            //

            
			EMUPATCH(D3DDevice_EnableOverlay)(TRUE);
			EMUPATCH(D3DDevice_UpdateOverlay)((X_D3DSurface*)pTexture, 0, 0, FALSE, 0);
            
        }
        else
        {			
			// Remove old locks before setting
			/*if(pTexture->Common & X_D3DCOMMON_ISLOCKED)
			{
				pTexture->EmuTexture8->UnlockRect(0);
				pTexture->Common &= ~X_D3DCOMMON_ISLOCKED;
			}*/

            pBaseTexture8 = pTexture->EmuBaseTexture8;

			// Let's be SURE that the texture is unlocked AND unswizzled before
			// we set it!!!
		//	EmuUnswizzleTextureStages();
		//	pBaseTexture8 = EmuD3DActiveTexture[Stage]->EmuBaseTexture8;

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

    

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_SwitchTexture
// ******************************************************************
VOID __fastcall XTL::EMUPATCH(D3DDevice_SwitchTexture)
(
    DWORD           Method,
    DWORD           Data,
    DWORD           Format
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SwitchTexture\n"
           "(\n"
           "   Method              : 0x%.08X\n"
           "   Data                : 0x%.08X\n"
           "   Format              : 0x%.08X\n"
           ");\n",
           Method, Data, Format);

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

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_GetDisplayMode
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_GetDisplayMode)
(
    X_D3DDISPLAYMODE         *pMode
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_GetDisplayMode\n"
           "(\n"
           "   pMode               : 0x%.08X\n"
           ");\n",
           pMode);

    HRESULT hRet;

    // make adjustments to parameters to make sense with windows d3d
    {
        D3DDISPLAYMODE *pPCMode = (D3DDISPLAYMODE*)pMode;

        hRet = g_pD3DDevice8->GetDisplayMode(pPCMode);

        // Convert Format (PC->Xbox)
        pMode->Format = EmuPC2XB_D3DFormat(pPCMode->Format);

        // TODO: Make this configurable in the future?
        pMode->Flags  = 0x000000A1; // D3DPRESENTFLAG_FIELD | D3DPRESENTFLAG_INTERLACED | D3DPRESENTFLAG_LOCKABLE_BACKBUFFER

        // TODO: Retrieve from current CreateDevice settings?
        pMode->Width = 640;
        pMode->Height = 480;
    }

    

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_Begin
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_Begin)
(
    X_D3DPRIMITIVETYPE     PrimitiveType
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_Begin\n"
           "(\n"
           "   PrimitiveType       : 0x%.08X\n"
           ");\n",
           PrimitiveType);

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

    

    return D3D_OK;
}

// ******************************************************************
// * patch: D3DDevice_SetVertexData2f
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetVertexData2f)
(
    int     Register,
    FLOAT   a,
    FLOAT   b
)
{
        DbgPrintf("EmuD3D8: EmuD3DDevice_SetVertexData2f >>\n"
               "(\n"
               "   Register            : 0x%.08X\n"
               "   a                   : %f\n"
               "   b                   : %f\n"
               ");\n",
               Register, a, b);

    return EMUPATCH(D3DDevice_SetVertexData4f)(Register, a, b, 0.0f, 1.0f);
}

static inline DWORD FtoDW(FLOAT f) { return *((DWORD*)&f); }
static inline FLOAT DWtoF(DWORD f) { return *((FLOAT*)&f); }

// ******************************************************************
// * patch: D3DDevice_SetVertexData2s
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetVertexData2s)
(
    int     Register,
    SHORT   a,
    SHORT   b
)
{
        DbgPrintf("EmuD3D8: EmuD3DDevice_SetVertexData2s >>\n"
               "(\n"
               "   Register            : 0x%.08X\n"
               "   a                   : %d\n"
               "   b                   : %d\n"
               ");\n",
               Register, a, b);

    DWORD dwA = a, dwB = b;

    return EMUPATCH(D3DDevice_SetVertexData4f)(Register, DWtoF(dwA), DWtoF(dwB), 0.0f, 1.0f);
}

// ******************************************************************
// * patch: D3DDevice_SetVertexData4f
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetVertexData4f)
(
    int     Register,
    FLOAT   a,
    FLOAT   b,
    FLOAT   c,
    FLOAT   d
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetVertexData4f\n"
           "(\n"
           "   Register            : 0x%.08X\n"
           "   a                   : %f\n"
           "   b                   : %f\n"
           "   c                   : %f\n"
           "   d                   : %f\n"
           ");\n",
           Register, a, b, c, d);

    HRESULT hRet = S_OK;

    switch(Register)
    {
        // TODO: Blend weight.

        case X_D3DVSDE_POSITION:
        {
            int o = g_IVBTblOffs;

            g_IVBTable[o].Position.x = a;
            g_IVBTable[o].Position.y = b;
            g_IVBTable[o].Position.z = c;
            g_IVBTable[o].Rhw = 1.0f;

            g_IVBTblOffs++;

            g_IVBFVF |= D3DFVF_XYZRHW;
        }
        break;

		case X_D3DVSDE_BLENDWEIGHT:
		{
            int o = g_IVBTblOffs;

            g_IVBTable[o].Position.x = a;
            g_IVBTable[o].Position.y = b;
            g_IVBTable[o].Position.z = c;
			g_IVBTable[o].Blend1 = d;

            g_IVBTblOffs++;

            g_IVBFVF |= D3DFVF_XYZB1;
        }
        break;

		case X_D3DVSDE_NORMAL:
        {
            int o = g_IVBTblOffs;

            g_IVBTable[o].Normal.x = a;
            g_IVBTable[o].Normal.y = b;
            g_IVBTable[o].Normal.z = c;

            g_IVBTblOffs++;

            g_IVBFVF |= D3DFVF_NORMAL;
        }
        break;

        case X_D3DVSDE_DIFFUSE:
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

		case X_D3DVSDE_SPECULAR:
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

		case X_D3DVSDE_TEXCOORD0:
        {
            int o = g_IVBTblOffs;

            g_IVBTable[o].TexCoord1.x = a;
            g_IVBTable[o].TexCoord1.y = b;

            if( (g_IVBFVF & D3DFVF_TEXCOUNT_MASK) < D3DFVF_TEX1)
            {
                g_IVBFVF |= D3DFVF_TEX1;
            }
        }
        break;

		case X_D3DVSDE_TEXCOORD1:
        {
            int o = g_IVBTblOffs;

            g_IVBTable[o].TexCoord2.x = a;
            g_IVBTable[o].TexCoord2.y = b;

            if( (g_IVBFVF & D3DFVF_TEXCOUNT_MASK) < D3DFVF_TEX2)
            {
                g_IVBFVF |= D3DFVF_TEX2;
            }
        }
        break;

		case X_D3DVSDE_TEXCOORD2:
        {
            int o = g_IVBTblOffs;

            g_IVBTable[o].TexCoord3.x = a;
            g_IVBTable[o].TexCoord3.y = b;

            if( (g_IVBFVF & D3DFVF_TEXCOUNT_MASK) < D3DFVF_TEX3)
            {
                g_IVBFVF |= D3DFVF_TEX3;
            }
        }
        break;

		case X_D3DVSDE_TEXCOORD3:
        {
            int o = g_IVBTblOffs;

            g_IVBTable[o].TexCoord4.x = a;
            g_IVBTable[o].TexCoord4.y = b;

            if( (g_IVBFVF & D3DFVF_TEXCOUNT_MASK) < D3DFVF_TEX4)
            {
                g_IVBFVF |= D3DFVF_TEX4;
            }
        }
        break;

        case X_D3DVSDE_VERTEX:
        {
            int o = g_IVBTblOffs;

            g_IVBTable[o].Position.x = a;
            g_IVBTable[o].Position.y = b;
            g_IVBTable[o].Position.z = c;
            g_IVBTable[o].Rhw = d;

            // Copy current color to next vertex
            g_IVBTable[o+1].dwDiffuse  = g_IVBTable[o].dwDiffuse;
            g_IVBTable[o+1].dwSpecular = g_IVBTable[o].dwSpecular;

            g_IVBTblOffs++;

            g_IVBFVF |= D3DFVF_XYZRHW;
        }
        break;

        default:
            CxbxKrnlCleanup("Unknown IVB Register : %d", Register);
    }

    

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_SetVertexData4ub
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetVertexData4ub)
(
	INT		Register,
	BYTE	a,
	BYTE	b,
	BYTE	c,
	BYTE	d
)
{
	

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetVertexData4ub >>\n"
           "(\n"
           "   Register            : 0x%.08X\n"
           "   a                   : 0x%.02X\n"
           "   b                   : 0x%.02X\n"
           "   c                   : 0x%.02X\n"
           "   d                   : 0x%.02X\n"
           ");\n",
           Register, a, b, c, d); 

		

	DWORD dwA = a, dwB = b, dwC = c, dwD = d;

    return EMUPATCH(D3DDevice_SetVertexData4f)(Register, DWtoF(dwA), DWtoF(dwB), DWtoF(dwC), DWtoF(dwD));
}

// ******************************************************************
// * patch: D3DDevice_SetVertexData4s
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetVertexData4s)
(
	INT		Register,
	SHORT	a,
	SHORT	b,
	SHORT	c,
	SHORT	d
)
{
	

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetVertexData4s >>\n"
           "(\n"
           "   Register            : 0x%.08X\n"
           "   a                   : 0x%.04X\n"
           "   b                   : 0x%.04X\n"
           "   c                   : 0x%.04X\n"
           "   d                   : 0x%.04X\n"
           ");\n",
           Register, a, b, c, d); 

		

	DWORD dwA = a, dwB = b, dwC = c, dwD = d;

    return EMUPATCH(D3DDevice_SetVertexData4f)(Register, DWtoF(dwA), DWtoF(dwB), DWtoF(dwC), DWtoF(dwD));
}

// ******************************************************************
// * patch: D3DDevice_SetVertexDataColor
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetVertexDataColor)
(
    int         Register,
    D3DCOLOR    Color
)
{
        DbgPrintf("EmuD3D8: EmuD3DDevice_SetVertexDataColor >>\n"
               "(\n"
               "   Register            : 0x%.08X\n"
               "   Color               : 0x%.08X\n"
               ");\n",
               Register, Color);

    FLOAT a = DWtoF((Color & 0xFF000000) >> 24);
    FLOAT r = DWtoF((Color & 0x00FF0000) >> 16);
    FLOAT g = DWtoF((Color & 0x0000FF00) >> 8);
    FLOAT b = DWtoF((Color & 0x000000FF) >> 0);

    return EMUPATCH(D3DDevice_SetVertexData4f)(Register, r, g, b, a);
}

// ******************************************************************
// * patch: D3DDevice_End
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_End)()
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_End();\n");

    if(g_IVBTblOffs != 0)
        EmuFlushIVB();

    // TODO: Should technically clean this up at some point..but on XP doesnt matter much
//    CxbxFree(g_pIVBVertexBuffer);
//    CxbxFree(g_IVBTable);

    

    return D3D_OK;
}

// ******************************************************************
// * patch: D3DDevice_RunPushBuffer
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_RunPushBuffer)
(
    X_D3DPushBuffer       *pPushBuffer,
    X_D3DFixup            *pFixup
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_RunPushBuffer\n"
           "(\n"
           "   pPushBuffer         : 0x%.08X\n"
           "   pFixup              : 0x%.08X\n"
           ");\n",
           pPushBuffer, pFixup);

    XTL::EmuExecutePushBuffer(pPushBuffer, pFixup);

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_Clear
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_Clear)
(
    DWORD           Count,
    CONST D3DRECT  *pRects,
    DWORD           Flags,
    D3DCOLOR        Color,
    float           Z,
    DWORD           Stencil
)
{
    DbgPrintf("EmuD3D8: EmuD3DDevice_Clear\n"
           "(\n"
           "   Count               : 0x%.08X\n"
           "   pRects              : 0x%.08X\n"
           "   Flags               : 0x%.08X\n"
           "   Color               : 0x%.08X\n"
           "   Z                   : %f\n"
           "   Stencil             : 0x%.08X\n"
           ");\n",
           Count, pRects, Flags,
           Color, Z, Stencil);

    // make adjustments to parameters to make sense with windows d3d
    {
        DWORD newFlags = 0;

		if (Flags & X_D3DCLEAR_TARGET) {
			// TODO: D3DCLEAR_TARGET_A, *R, *G, *B don't exist on windows
			if ((Flags & X_D3DCLEAR_TARGET) != X_D3DCLEAR_TARGET)
				EmuWarning("Unsupported : Partial D3DCLEAR_TARGET flag(s) for D3DDevice_Clear : 0x%.08X", Flags & X_D3DCLEAR_TARGET);
		
			newFlags |= D3DCLEAR_TARGET;
		}

        // Do not needlessly clear Z Buffer
		if (Flags & X_D3DCLEAR_ZBUFFER) {
			if (g_bHasDepthStencil)
				newFlags |= D3DCLEAR_ZBUFFER;
			else
				EmuWarning("Unsupported : D3DCLEAR_ZBUFFER flag for D3DDevice_Clear without ZBuffer");
		}

		// Only clear depth buffer and stencil if present
		//
		// Avoids following DirectX Debug Runtime error report
		//    [424] Direct3D8: (ERROR) :Invalid flag D3DCLEAR_ZBUFFER: no zbuffer is associated with device. Clear failed. 
		if (Flags & X_D3DCLEAR_STENCIL) {
			if (g_bHasDepthStencil) // TODO : Introduce/use g_bHasStencil
				newFlags |= D3DCLEAR_STENCIL;
			else
				EmuWarning("Unsupported : D3DCLEAR_STENCIL flag for D3DDevice_Clear without ZBuffer");
		}

        if(Flags & ~(X_D3DCLEAR_TARGET | X_D3DCLEAR_ZBUFFER | X_D3DCLEAR_STENCIL))
            EmuWarning("Unsupported Flag(s) for D3DDevice_Clear : 0x%.08X", Flags & ~(X_D3DCLEAR_TARGET | X_D3DCLEAR_ZBUFFER | X_D3DCLEAR_STENCIL));

        Flags = newFlags;
    }

	DWORD dwFillMode;

	if(g_iWireframe == 0)
        dwFillMode = D3DFILL_SOLID;
    else if(g_iWireframe == 1)
        dwFillMode = D3DFILL_WIREFRAME;
    else
        dwFillMode = D3DFILL_POINT;

    g_pD3DDevice8->SetRenderState(D3DRS_FILLMODE, dwFillMode);

    HRESULT ret = g_pD3DDevice8->Clear(Count, pRects, Flags, Color, Z, Stencil);

    return ret;
}

// ******************************************************************
// * patch: D3DDevice_Present
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_Present)
(
    CONST RECT* pSourceRect,
    CONST RECT* pDestRect,
    PVOID       pDummy1,
    PVOID       pDummy2
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_Present\n"
           "(\n"
           "   pSourceRect         : 0x%.08X\n"
           "   pDestRect           : 0x%.08X\n"
           "   pDummy1             : 0x%.08X\n"
           "   pDummy2             : 0x%.08X\n"
           ");\n",
           pSourceRect, pDestRect, pDummy1, pDummy2);

	HRESULT hRet = S_OK;
	
	// release back buffer lock
	{
		IDirect3DSurface8 *pBackBuffer;

		g_pD3DDevice8->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);

		pBackBuffer->UnlockRect();
	}

	// TODO: Make a video option to wait for VBlank before calling Present.
	// Makes syncing to 30fps easier (which is the native frame rate for Azurik
	// and Halo).
//	g_pDD7->WaitForVerticalBlank( DDWAITVB_BLOCKEND, NULL );
//	g_pDD7->WaitForVerticalBlank( DDWAITVB_BLOCKEND, NULL );

	hRet = g_pD3DDevice8->Present(pSourceRect, pDestRect, (HWND)pDummy1, (CONST RGNDATA*)pDummy2);

	// Put primitives per frame in the title
	/*{
		char szString[64];

		sprintf( szString, "Cxbx: PPF(%d)", g_dwPrimPerFrame );

		SetWindowText( CxbxKrnl_hEmuParent, szString );

		g_dwPrimPerFrame = 0;
	}*/

	// not really accurate because you definately dont always present on every vblank
	g_VBData.Swap = g_VBData.VBlank;

	if(g_VBData.VBlank == g_VBLastSwap + 1)
		g_VBData.Flags = 1; // D3DVBLANK_SWAPDONE
	else
	{
		g_VBData.Flags = 2; // D3DVBLANK_SWAPMISSED
		g_SwapData.MissedVBlanks++;
	}

	// Handle Swap Callback function
	{
		g_SwapData.Swap++;

		if(g_pSwapCallback != NULL) 
		{
				
			g_pSwapCallback(&g_SwapData);
				
		}
	}

	g_bHackUpdateSoftwareOverlay = FALSE;

    

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_Swap
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_Swap)
(
    DWORD Flags
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_Swap\n"
           "(\n"
           "   Flags               : 0x%.08X\n"
           ");\n",
           Flags);

    // TODO: Ensure this flag is always the same across library versions
    if(Flags != 0)
        EmuWarning("XTL::EmuD3DDevice_Swap: Flags != 0");

    // release back buffer lock
    {
        IDirect3DSurface8 *pBackBuffer;

        g_pD3DDevice8->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);

        if(pBackBuffer) pBackBuffer->UnlockRect();
    }

	// TODO: Make a video option to wait for VBlank before calling Present.
	// Makes syncing to 30fps easier (which is the native frame rate for Azurik
	// and Halo).
//	g_pDD7->WaitForVerticalBlank( DDWAITVB_BLOCKEND, NULL );
//	g_pDD7->WaitForVerticalBlank( DDWAITVB_BLOCKEND, NULL );

    HRESULT hRet = g_pD3DDevice8->Present(0, 0, 0, 0);

	// Handle Swap Callback function
	{
		g_SwapData.Swap++;

		if(g_pSwapCallback != NULL) 
		{
				
			g_pSwapCallback(&g_SwapData);
				
		}
	}

	g_bHackUpdateSoftwareOverlay = FALSE;

    

    return hRet;
}

// ******************************************************************
// * patch: IDirect3DResource8_Register
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DResource_Register)
(
    X_D3DResource      *pThis,
    PVOID               pBase
)
{
    

    DbgPrintf("EmuD3D8: EmuIDirect3DResource8_Register\n"
           "(\n"
           "   pThis               : 0x%.08X (->Data : 0x%.08X)\n"
           "   pBase               : 0x%.08X\n"
           ");\n",
           pThis, pThis->Data, pBase);

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
            DbgPrintf("EmuIDirect3DResource8_Register : Creating VertexBuffer...\n");

            X_D3DVertexBuffer *pVertexBuffer = (X_D3DVertexBuffer*)pResource;

            // create vertex buffer
            {
                DWORD dwSize = EmuCheckAllocationSize(pBase, true);

                if(dwSize == -1)
                {
                    // TODO: once this is known to be working, remove the warning
                    EmuWarning("Vertex buffer allocation size unknown");
                    dwSize = 0x2000;  // temporarily assign a small buffer, which will be increased later
					/*hRet = E_FAIL;
					goto fail;*/
                }

                hRet = g_pD3DDevice8->CreateVertexBuffer
                (
                    dwSize, 0, 0, D3DPOOL_MANAGED,
                    &pResource->EmuVertexBuffer8
                );

				if(FAILED(hRet))
				{
					char szString[256];
					sprintf( szString, "CreateVertexBuffer Failed!\n\nVB Size = 0x%X\n\nError: \nDesc: ", dwSize/*,
						DXGetErrorString8A(hRet)*//*, DXGetErrorDescription8A(hRet)*/);
					
					EmuWarning( szString );
					return hRet;
				}

                #ifdef _DEBUG_TRACK_VB
                g_VBTrackTotal.insert(pResource->EmuVertexBuffer8);
                #endif

                BYTE *pData = 0;

                hRet = pResource->EmuVertexBuffer8->Lock(0, 0, &pData, 0);

                if(FAILED(hRet))
                    CxbxKrnlCleanup("VertexBuffer Lock Failed!\n\nError: \nDesc: "/*,
								DXGetErrorString8A(hRet)*//*, DXGetErrorDescription8A(hRet)*/);


                memcpy(pData, (void*)pBase, dwSize);

                pResource->EmuVertexBuffer8->Unlock();

                pResource->Data = (ULONG)pData;
            }

            DbgPrintf("EmuIDirect3DResource8_Register : Successfully Created VertexBuffer (0x%.08X)\n", pResource->EmuVertexBuffer8);
        }
        break;

        case X_D3DCOMMON_TYPE_INDEXBUFFER:
        {
            DbgPrintf("EmuIDirect3DResource8_Register :-> IndexBuffer...\n");

            X_D3DIndexBuffer *pIndexBuffer = (X_D3DIndexBuffer*)pResource;

            // create index buffer
            {
                DWORD dwSize = EmuCheckAllocationSize(pBase, true);

                if(dwSize == -1 || dwSize == 0)
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
					CxbxKrnlCleanup("CreateIndexBuffer Failed!\n\nError: \nDesc: \nSize: %d",
								/*DXGetErrorString8A(hRet), *//*DXGetErrorDescription8A(hRet),*/ dwSize);


                BYTE *pData = 0;

                hRet = pResource->EmuIndexBuffer8->Lock(0, dwSize, &pData, 0);

                if(FAILED(hRet))
                    CxbxKrnlCleanup("IndexBuffer Lock Failed!\n\nError: %s\nDesc: "/*,
								DXGetErrorString8A(hRet)*//*, DXGetErrorDescription8A(hRet)*/);


                memcpy(pData, (void*)pBase, dwSize);

                pResource->EmuIndexBuffer8->Unlock();

                pResource->Data = (ULONG)pData;
            }

            DbgPrintf("EmuIDirect3DResource8_Register : Successfully Created IndexBuffer (0x%.08X)\n", pResource->EmuIndexBuffer8);
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

            DbgPrintf("EmuIDirect3DResource8_Register : Successfully Created PushBuffer (0x%.08X, 0x%.08X, 0x%.08X)\n", pResource->Data, pPushBuffer->Size, pPushBuffer->AllocationSize);
        }
        break;

        case X_D3DCOMMON_TYPE_SURFACE:
        case X_D3DCOMMON_TYPE_TEXTURE:
        {
            if(dwCommonType == X_D3DCOMMON_TYPE_SURFACE) {
                DbgPrintf("EmuIDirect3DResource8_Register :-> Surface...\n");
            } else {
                DbgPrintf("EmuIDirect3DResource8_Register :-> Texture...\n");
            }

            X_D3DPixelContainer *pPixelContainer = (X_D3DPixelContainer*)pResource;

            X_D3DFORMAT X_Format = (X_D3DFORMAT)((pPixelContainer->Format & X_D3DFORMAT_FORMAT_MASK) >> X_D3DFORMAT_FORMAT_SHIFT);
            D3DFORMAT   PCFormat = EmuXB2PC_D3DFormat(X_Format);
            D3DFORMAT   CacheFormat = (XTL::D3DFORMAT)0;
            // TODO: check for dimensions

            // TODO: HACK: Temporary?
            if(X_Format == X_D3DFMT_LIN_D24S8)
            {
                /*CxbxKrnlCleanup*/EmuWarning("D3DFMT_LIN_D24S8 not yet supported!");
                X_Format = X_D3DFMT_LIN_A8R8G8B8;
                PCFormat = D3DFMT_A8R8G8B8;
            }

			if(X_Format == X_D3DFMT_LIN_D16)
            {
                /*CxbxKrnlCleanup*/EmuWarning("D3DFMT_LIN_D16 not yet supported!");
                X_Format = X_D3DFMT_LIN_R5G6B5;
                PCFormat = D3DFMT_R5G6B5;
            }

            DWORD dwWidth, dwHeight, dwBPP, dwDepth = 1, dwPitch = 0, dwMipMapLevels = 1;
            BOOL  bSwizzled = EmuXBFormatIsSwizzled(X_Format, &dwBPP), bCompressed = FALSE, dwCompressedSize = 0;
            BOOL  bCubemap = pPixelContainer->Format & X_D3DFORMAT_CUBEMAP;

            // Interpret Width/Height/BPP
            if(X_Format == X_D3DFMT_X8R8G8B8 || X_Format == X_D3DFMT_A8R8G8B8
			|| X_Format == X_D3DFMT_A8B8G8R8)
            {
                // Swizzled 32 Bit
                dwWidth  = 1 << ((pPixelContainer->Format & X_D3DFORMAT_USIZE_MASK) >> X_D3DFORMAT_USIZE_SHIFT);
                dwHeight = 1 << ((pPixelContainer->Format & X_D3DFORMAT_VSIZE_MASK) >> X_D3DFORMAT_VSIZE_SHIFT);
                dwMipMapLevels = (pPixelContainer->Format & X_D3DFORMAT_MIPMAP_MASK) >> X_D3DFORMAT_MIPMAP_SHIFT;
                dwDepth  = 1;// HACK? 1 << ((pPixelContainer->Format & X_D3DFORMAT_PSIZE_MASK) >> X_D3DFORMAT_PSIZE_SHIFT);
                dwPitch  = dwWidth * dwBPP;
            }
            else if(X_Format == X_D3DFMT_R5G6B5 || X_Format == X_D3DFMT_A4R4G4B4
                 || X_Format == X_D3DFMT_A1R5G5B5 || X_Format == X_D3DFMT_X1R5G5B5
                 || X_Format == X_D3DFMT_G8B8 || X_Format == X_D3DFMT_A8L8)
            {
                // Swizzled 16 Bit
                dwWidth  = 1 << ((pPixelContainer->Format & X_D3DFORMAT_USIZE_MASK) >> X_D3DFORMAT_USIZE_SHIFT);
                dwHeight = 1 << ((pPixelContainer->Format & X_D3DFORMAT_VSIZE_MASK) >> X_D3DFORMAT_VSIZE_SHIFT);
                dwMipMapLevels = (pPixelContainer->Format & X_D3DFORMAT_MIPMAP_MASK) >> X_D3DFORMAT_MIPMAP_SHIFT;
                dwDepth  = 1;// HACK? 1 << ((pPixelContainer->Format & X_D3DFORMAT_PSIZE_MASK) >> X_D3DFORMAT_PSIZE_SHIFT);
                dwPitch  = dwWidth * dwBPP;
            }
            else if(X_Format == X_D3DFMT_L8 || X_Format == X_D3DFMT_P8
                || X_Format == X_D3DFMT_AL8
                || X_Format == X_D3DFMT_A8)
            {
                // Swizzled 8 Bit
                dwWidth  = 1 << ((pPixelContainer->Format & X_D3DFORMAT_USIZE_MASK) >> X_D3DFORMAT_USIZE_SHIFT);
                dwHeight = 1 << ((pPixelContainer->Format & X_D3DFORMAT_VSIZE_MASK) >> X_D3DFORMAT_VSIZE_SHIFT);
                dwMipMapLevels = (pPixelContainer->Format & X_D3DFORMAT_MIPMAP_MASK) >> X_D3DFORMAT_MIPMAP_SHIFT;
                dwDepth  = 1;// HACK? 1 << ((pPixelContainer->Format & X_D3DFORMAT_PSIZE_MASK) >> X_D3DFORMAT_PSIZE_SHIFT);
                dwPitch  = dwWidth * dwBPP;
            }
            else if(X_Format == X_D3DFMT_LIN_X8R8G8B8 || X_Format == X_D3DFMT_LIN_A8R8G8B8
				 || X_Format == X_D3DFMT_LIN_D24S8 || X_Format == X_D3DFMT_LIN_A8B8G8R8)
            {
                // Linear 32 Bit
                dwWidth  = (pPixelContainer->Size & X_D3DSIZE_WIDTH_MASK) + 1;
                dwHeight = ((pPixelContainer->Size & X_D3DSIZE_HEIGHT_MASK) >> X_D3DSIZE_HEIGHT_SHIFT) + 1;
                dwPitch  = (((pPixelContainer->Size & X_D3DSIZE_PITCH_MASK) >> X_D3DSIZE_PITCH_SHIFT)+1)*64;
            }
            else if(X_Format == X_D3DFMT_LIN_R5G6B5 || X_Format == X_D3DFMT_LIN_D16
				 || X_Format == X_D3DFMT_LIN_A4R4G4B4 || X_Format == X_D3DFMT_LIN_A1R5G5B5
				 || X_Format == X_D3DFMT_LIN_X1R5G5B5 )
            {
                // Linear 16 Bit
                dwWidth  = (pPixelContainer->Size & X_D3DSIZE_WIDTH_MASK) + 1;
                dwHeight = ((pPixelContainer->Size & X_D3DSIZE_HEIGHT_MASK) >> X_D3DSIZE_HEIGHT_SHIFT) + 1;
                dwPitch  = (((pPixelContainer->Size & X_D3DSIZE_PITCH_MASK) >> X_D3DSIZE_PITCH_SHIFT)+1)*64;
            }
            else if(X_Format == X_D3DFMT_DXT1 || X_Format == X_D3DFMT_DXT3 || X_Format == X_D3DFMT_DXT5)
            {
                bCompressed = TRUE;

                // Compressed
                dwWidth  = 1 << ((pPixelContainer->Format & X_D3DFORMAT_USIZE_MASK) >> X_D3DFORMAT_USIZE_SHIFT);
                dwHeight = 1 << ((pPixelContainer->Format & X_D3DFORMAT_VSIZE_MASK) >> X_D3DFORMAT_VSIZE_SHIFT);
                dwDepth  = 1 << ((pPixelContainer->Format & X_D3DFORMAT_PSIZE_MASK) >> X_D3DFORMAT_PSIZE_SHIFT);
                dwMipMapLevels = (pPixelContainer->Format & X_D3DFORMAT_MIPMAP_MASK) >> X_D3DFORMAT_MIPMAP_SHIFT;

                // D3DFMT_DXT2...D3DFMT_DXT5 : 128bits per block/per 16 texels
                dwCompressedSize = dwWidth*dwHeight;

                if(X_Format == X_D3DFMT_DXT1) // D3DFMT_DXT1 : 64bits per block/per 16 texels
                    dwCompressedSize /= 2;
            }
            else if(X_Format == X_D3DFMT_YUY2)
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

            if(X_Format == X_D3DFMT_YUY2)
            {
                //
                // cache the overlay size
                //

                g_dwOverlayW = dwWidth;
                g_dwOverlayH = dwHeight;
                g_dwOverlayP = RoundUp(g_dwOverlayW, 64) * dwBPP;

                //
                // create texture resource
                //

                DWORD dwSize = g_dwOverlayP*g_dwOverlayH;
                DWORD dwPtr = (DWORD)CxbxMalloc(dwSize + sizeof(DWORD));

                DWORD *pRefCount = (DWORD*)(dwPtr + dwSize);

                // initialize ref count
                *pRefCount = 1;

                // If YUY2 is not supported in hardware, we'll actually mark this as a special fake texture (set highest bit)
				pPixelContainer->Common = 1; // Set refcount to 1
                pPixelContainer->Data = X_D3DRESOURCE_DATA_YUV_SURFACE;
                pPixelContainer->Lock = dwPtr;
                pPixelContainer->Format = (X_D3DFMT_YUY2 << X_D3DFORMAT_FORMAT_SHIFT);

                pPixelContainer->Size  = (g_dwOverlayW & X_D3DSIZE_WIDTH_MASK);
                pPixelContainer->Size |= (g_dwOverlayH << X_D3DSIZE_HEIGHT_SHIFT) & X_D3DSIZE_HEIGHT_MASK;
                pPixelContainer->Size |= (g_dwOverlayP << X_D3DSIZE_PITCH_SHIFT) & X_D3DSIZE_PITCH_MASK;
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
                    hRet = g_pD3DDevice8->CreateImageSurface(dwWidth, dwHeight, PCFormat, &pResource->EmuSurface8);

                    if(FAILED(hRet))
                        CxbxKrnlCleanup("CreateImageSurface Failed!\n\nError: %s\nDesc: %s"/*,
								DXGetErrorString8A(hRet), DXGetErrorDescription8A(hRet)*/);
;

                    DbgPrintf("EmuIDirect3DResource8_Register : Successfully Created ImageSurface (0x%.08X, 0x%.08X)\n", pResource, pResource->EmuSurface8);
                    DbgPrintf("EmuIDirect3DResource8_Register : Width : %d, Height : %d, Format : %d\n", dwWidth, dwHeight, PCFormat);
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

                    // HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK
                    // Since most modern graphics cards does not support
                    // palette based textures we need to expand it to
                    // ARGB texture format
                    if (PCFormat == D3DFMT_P8) //Palette
                    {
						EmuWarning("D3DFMT_P8 -> D3DFMT_A8R8G8B8");

                        CacheFormat = PCFormat;       // Save this for later
                        PCFormat = D3DFMT_A8R8G8B8;   // ARGB
                    }

                    if(bCubemap)
                    {
                        DbgPrintf("CreateCubeTexture(%d, %d, 0, %d, D3DPOOL_MANAGED, 0x%.08X)\n", dwWidth,
                            dwMipMapLevels, PCFormat, &pResource->EmuTexture8);

                        hRet = g_pD3DDevice8->CreateCubeTexture
                        (
                            dwWidth, dwMipMapLevels, 0, PCFormat,
                            D3DPOOL_MANAGED, &pResource->EmuCubeTexture8
                        );

                        if(FAILED(hRet))
                            CxbxKrnlCleanup("CreateCubeTexture Failed!\n\nError: \nDesc: "/*,
								DXGetErrorString8A(hRet), DXGetErrorDescription8A(hRet)*/);

                        DbgPrintf("EmuIDirect3DResource8_Register : Successfully Created CubeTexture (0x%.08X, 0x%.08X)\n", pResource, pResource->EmuCubeTexture8);
                    }
                    else
                    {
                    //    printf("CreateTexture(%d, %d, %d, 0, %d (X=0x%.08X), D3DPOOL_MANAGED, 0x%.08X)\n", dwWidth, dwHeight,
                     //       dwMipMapLevels, PCFormat, X_Format, &pResource->EmuTexture8);

						// HACK: Quantum Redshift
						/*if( dwMipMapLevels == 8 && X_Format == X_D3DFMT_DXT1 )
						{
							printf( "Dirty Quantum Redshift hack applied!\n" );
							dwMipMapLevels = 1;
						}*/

                        hRet = g_pD3DDevice8->CreateTexture
                        (
                            dwWidth, dwHeight, dwMipMapLevels, 0, PCFormat,
                            D3DPOOL_MANAGED, &pResource->EmuTexture8
                        );

						/*if(FAILED(hRet))
						{
							hRet = g_pD3DDevice8->CreateTexture
							(
								dwWidth, dwHeight, dwMipMapLevels, 0, PCFormat,
								D3DPOOL_SYSTEMMEM, &pResource->EmuTexture8
							);
						}*/

                        if(FAILED(hRet))
							CxbxKrnlCleanup("CreateTexture Failed!\n\nError: \nDesc: "/*,
								DXGetErrorString8A(hRet), DXGetErrorDescription8A(hRet)*/);

                        DbgPrintf("EmuIDirect3DResource8_Register : Successfully Created Texture (0x%.08X, 0x%.08X)\n", pResource, pResource->EmuTexture8);
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

                        if( pBase != NULL ) pThis->Data = (DWORD)pSrc;

                        if(( pResource->Data == X_D3DRESOURCE_DATA_BACK_BUFFER)
                         ||( (DWORD)pBase == X_D3DRESOURCE_DATA_BACK_BUFFER))
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
									// First we need to unswizzle the texture data
									XTL::EmuUnswizzleRect
									(
										pSrc + dwMipOffs, dwMipWidth, dwMipHeight, dwDepth, LockedRect.pBits,
										LockedRect.Pitch, iRect, iPoint, dwBPP
									);

									if (CacheFormat == D3DFMT_P8) //Palette
                                    {
                                        EmuWarning("Unsupported texture format D3DFMT_P8,\nexpanding to D3DFMT_A8R8G8B8");

										BYTE *pPixelData = (BYTE*)LockedRect.pBits;
                                        DWORD dwDataSize = dwMipWidth*dwMipHeight;
										DWORD* pExpandedTexture = (DWORD*)CxbxMalloc(dwDataSize * sizeof(DWORD));
										DWORD* pTexturePalette = (DWORD*)pCurrentPalette;

										//__asm int 3;
                                        unsigned int w = 0;
                                        unsigned int x = 0;
                                        for (unsigned int y = 0;y < dwDataSize;y++)
                                        {
											// Read P8 pixel :
											unsigned char p = (unsigned char)pPixelData[w++];

											// Read the corresponding ARGB from the palette and store it in the new texture :
											// HACK: Prevent crash if a pallete has not been loaded yet
											if (pTexturePalette != nullptr)	{
												pExpandedTexture[y] = pTexturePalette[p];
											} else {
												pExpandedTexture[y] = 0;
											}
											
											// are we at the end of a line?
                                            if(++x == dwMipWidth)
                                            {
                                                x = 0;
												// Since P8 contains byte pixels instead of dword ARGB pixels,
												// the next line resides 3 bytes additional per pixel further :
                                                w += dwMipWidth * (sizeof(DWORD) - sizeof(BYTE));
                                            }
                                        }

										//__asm int 3;
                                        // Copy the expanded texture back to the buffer
                                        memcpy(pPixelData, pExpandedTexture, dwDataSize * sizeof(DWORD));

                                        // Flush unused data buffers
                                        CxbxFree(pExpandedTexture);
                                    }
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

								if( pSrc )
								{
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

                    sprintf(szBuffer, _DEBUG_DUMP_TEXTURE_REGISTER "%.03d-RegSurface%.03d.dds", X_Format, dwDumpSurface++);

                    D3DXSaveSurfaceToFile(szBuffer, D3DXIFF_DDS, pResource->EmuSurface8, NULL, NULL);
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

                            sprintf(szBuffer, _DEBUG_DUMP_TEXTURE_REGISTER "%.03d-RegCubeTex%.03d-%d.dds", X_Format, dwDumpCube++, v);

                            pResource->EmuCubeTexture8->GetCubeMapSurface((D3DCUBEMAP_FACES)v, 0, &pSurface);

                            D3DXSaveSurfaceToFile(szBuffer, D3DXIFF_DDS, pSurface, NULL, NULL);
                        }
                    }
                    else
                    {
                        static int dwDumpTex = 0;

                        char szBuffer[255];

                        sprintf(szBuffer, _DEBUG_DUMP_TEXTURE_REGISTER "%.03d-RegTexture%.03d.dds", X_Format, dwDumpTex++);

                        D3DXSaveTextureToFile(szBuffer, D3DXIFF_DDS, pResource->EmuTexture8, NULL);
                    }
                }
                #endif
            }
        }
        break;

        case X_D3DCOMMON_TYPE_PALETTE:
        {
            DbgPrintf("EmuIDirect3DResource8_Register :-> Palette...\n");

            X_D3DPalette *pPalette = (X_D3DPalette*)pResource;

            // create palette
            {
                DWORD dwSize = EmuCheckAllocationSize(pBase, true);

                if(dwSize == -1)
                {
                    // TODO: once this is known to be working, remove the warning
                    EmuWarning("Palette allocation size unknown");

                    pPalette->Lock = X_D3DRESOURCE_LOCK_FLAG_NOSIZE;
                }

                pCurrentPalette = pBase;
				dwCurrentPaletteSize = dwSize;

                pResource->Data = (ULONG)pBase;
            }

            //DbgPrintf("EmuIDirect3DResource8_Register (0x%X) : Successfully Created Palette (0x%.08X, 0x%.08X, 0x%.08X)\n", pResource->Data, pResource->Size, pResource->AllocationSize);
        }
        break;

        case X_D3DCOMMON_TYPE_FIXUP:
        {
            X_D3DFixup *pFixup = (X_D3DFixup*)pResource;

            CxbxKrnlCleanup("IDirect3DResource8::Register -> X_D3DCOMMON_TYPE_FIXUP is not yet supported\n"
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

    

    return hRet;
}

// ******************************************************************
// * patch: IDirect3DResource8_AddRef
// ******************************************************************
ULONG WINAPI XTL::EMUPATCH(D3DResource_AddRef)
(
    X_D3DResource      *pThis
)
{
    

    DbgPrintf("EmuD3D8: EmuIDirect3DResource8_AddRef\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
           ");\n",
           pThis);

    ULONG uRet = 0;

    if (!pThis) {
        EmuWarning("IDirect3DResource8::AddRef() was not passed a valid pointer!");
    }
    else 
    {
        if(pThis->Data == X_D3DRESOURCE_DATA_YUV_SURFACE)
        {
            DWORD  dwPtr = (DWORD)pThis->Lock;
            DWORD *pRefCount = (DWORD*)(dwPtr + g_dwOverlayP*g_dwOverlayH);
            ++(*pRefCount);
        }
        else
        {
            IDirect3DResource8 *pResource8 = pThis->EmuResource8;

            if(pThis->Lock == X_D3DRESOURCE_LOCK_PALETTE)
                uRet = ++pThis->Lock;
            else if(pResource8 != 0)
                uRet = pResource8->AddRef();

		    if(!pResource8)
			    __asm int 3;
			    //EmuWarning("EmuResource is not a valid pointer!");

            pThis->Common = (pThis->Common & ~X_D3DCOMMON_REFCOUNT_MASK) | ((pThis->Common & X_D3DCOMMON_REFCOUNT_MASK) + 1);
        }
    }

    

    return uRet;
}

// ******************************************************************
// * patch: IDirect3DResource8_Release
// ******************************************************************
ULONG WINAPI XTL::EMUPATCH(D3DResource_Release)
(
    X_D3DResource      *pThis
)
{
    DbgPrintf("EmuD3D8: EmuIDirect3DResource8_Release\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
           ");\n",
           pThis);

    ULONG uRet = 0;

	// HACK: In case the clone technique fails...
	if(!pThis)
	{
		EmuWarning("NULL texture!");

		return 0;
	}

	if(pThis->Data == X_D3DRESOURCE_DATA_YUV_SURFACE)
    {
        DWORD  dwPtr = (DWORD)pThis->Lock;
        DWORD *pRefCount = (DWORD*)(dwPtr + g_dwOverlayP*g_dwOverlayH);

        if(--(*pRefCount) == 0)
        {
            if(g_pCachedYuvSurface == pThis)
                g_pCachedYuvSurface = NULL;

            // free memory associated with this special resource handle
            CxbxFree((PVOID)dwPtr);
        }
        
		EMUPATCH(D3DDevice_EnableOverlay)(FALSE);
    }
    else
    {
        IDirect3DResource8 *pResource8 = pThis->EmuResource8;

        if(pThis->Lock == X_D3DRESOURCE_LOCK_PALETTE)
        {
            delete[] (PVOID)pThis->Data;
            uRet = --pThis->Lock;
        }
        else if(pResource8 != nullptr)
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
                DbgPrintf("EmuIDirect3DResource8_Release : Cleaned up a Resource!\n");

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

        pThis->Common = (pThis->Common & ~X_D3DCOMMON_REFCOUNT_MASK) | ((pThis->Common & X_D3DCOMMON_REFCOUNT_MASK) - 1);
    }

    return uRet;
}

// ******************************************************************
// * patch: IDirect3DResource8_IsBusy
// ******************************************************************
BOOL WINAPI XTL::EMUPATCH(D3DResource_IsBusy)
(
    X_D3DResource      *pThis
)
{
    

    /* too much output
    DbgPrintf("EmuD3D8: EmuIDirect3DResource8_IsBusy\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
           ");\n",
           pThis);
    //*/

    IDirect3DResource8 *pResource8 = pThis->EmuResource8;

    

    return FALSE;
}

// ******************************************************************
// * patch: IDirect3DResource8_GetType
// ******************************************************************
XTL::X_D3DRESOURCETYPE WINAPI XTL::EMUPATCH(D3DResource_GetType)
(
    X_D3DResource      *pThis
)
{
    

    DbgPrintf("EmuD3D8: EmuIDirect3DResource8_GetType\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
           ");\n",
           pThis);

	D3DRESOURCETYPE rType;

	// Check for Xbox specific resources (Azurik may need this)
	DWORD dwType = pThis->Common & X_D3DCOMMON_TYPE_MASK;

	switch(dwType)
	{
	case X_D3DCOMMON_TYPE_PUSHBUFFER:
		rType = (D3DRESOURCETYPE) 8; break;
	case X_D3DCOMMON_TYPE_PALETTE:
		rType = (D3DRESOURCETYPE) 9; break;
	case X_D3DCOMMON_TYPE_FIXUP:
		rType = (D3DRESOURCETYPE) 10; break;
	default:
		rType = pThis->EmuResource8->GetType(); break;
	}

    

    return (X_D3DRESOURCETYPE)rType;
}

// ******************************************************************
// * patch: Lock2DSurface
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(Lock2DSurface)
(
    X_D3DPixelContainer *pPixelContainer,
    D3DCUBEMAP_FACES     FaceType,
    UINT                 Level,
    D3DLOCKED_RECT      *pLockedRect,
    RECT                *pRect,
    DWORD                Flags
)
{
    

    DbgPrintf("EmuD3D8: EmuLock2DSurface\n"
           "(\n"
           "   pPixelContainer     : 0x%.08X\n"
           "   FaceType            : 0x%.08X\n"
           "   Level               : 0x%.08X\n"
           "   pLockedRect         : 0x%.08X\n"
           "   pRect               : 0x%.08X\n"
           "   Flags               : 0x%.08X\n"
           ");\n",
           pPixelContainer, FaceType, Level, pLockedRect, pRect, Flags);

    EmuVerifyResourceIsRegistered(pPixelContainer);

    HRESULT hRet = pPixelContainer->EmuCubeTexture8->LockRect(FaceType, Level, pLockedRect, pRect, Flags);

    

    return;
}

// ******************************************************************
// * patch: Lock3DSurface
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(Lock3DSurface)
(
    X_D3DPixelContainer *pPixelContainer,
    UINT				Level,
	D3DLOCKED_BOX		*pLockedVolume,
	D3DBOX				*pBox,
	DWORD				Flags
)
{
	

    DbgPrintf("EmuD3D8: EmuLock2DSurface\n"
           "(\n"
           "   pPixelContainer     : 0x%.08X\n"
           "   Level               : 0x%.08X\n"
           "   pLockedVolume       : 0x%.08X\n"
           "   pBox                : 0x%.08X\n"
           "   Flags               : 0x%.08X\n"
           ");\n",
           pPixelContainer, Level, pLockedVolume, pBox, Flags);

    EmuVerifyResourceIsRegistered(pPixelContainer);

    HRESULT hRet = pPixelContainer->EmuVolumeTexture8->LockBox(Level, pLockedVolume, pBox, Flags);
	if(FAILED(hRet))
		EmuWarning("Lock3DSurface failed!");

    

    return;
}

// ******************************************************************
// * patch: Get2DSurfaceDesc
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(Get2DSurfaceDesc)
(
    X_D3DPixelContainer *pPixelContainer,
    DWORD                dwLevel,
    X_D3DSURFACE_DESC   *pDesc
)
{
    

    DbgPrintf("EmuD3D8: EmuGet2DSurfaceDesc\n"
           "(\n"
           "   pPixelContainer     : 0x%.08X\n"
           "   dwLevel             : 0x%.08X\n"
           "   pDesc               : 0x%.08X\n"
           ");\n",
           pPixelContainer, dwLevel, pDesc);

    EmuVerifyResourceIsRegistered(pPixelContainer);

    D3DSURFACE_DESC SurfaceDesc;

    ZeroMemory(&SurfaceDesc, sizeof(SurfaceDesc));

    HRESULT hRet;

    if(dwLevel == 0xFEFEFEFE)
    {
		DbgPrintf("EmuSurface8: = 0x%.08X\n", pPixelContainer->EmuSurface8 );
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
		DbgPrintf("EmuTexture8: = 0x%.08X\n", pPixelContainer->EmuTexture8 );

		if(pPixelContainer->Data == X_D3DRESOURCE_DATA_YUV_SURFACE)
		{
			hRet = E_FAIL;
		}
		else
		{
			hRet = E_FAIL;

			// TODO: Find out why EmuTexture8 is NULL in Crazy Taxi 3
			if (pPixelContainer->EmuTexture8 != nullptr) {
				hRet = pPixelContainer->EmuTexture8->GetLevelDesc(dwLevel, &SurfaceDesc);
			}
			
			if(FAILED(hRet))
				EmuWarning("IDirect3DTexture8::GetSurfaceDesc failed!");
		}

        /*
        static int dwDumpTexture = 0;

        char szBuffer[255];

        sprintf(szBuffer, "C:\\Aaron\\Textures\\GetDescTexture%.03d.bmp", dwDumpTexture++);

        D3DXSaveTextureToFile(szBuffer, D3DXIFF_BMP, pPixelContainer->EmuTexture8, NULL);
        */
    }

    // rearrange into xbox format (remove D3DPOOL)
	if(SUCCEEDED(hRet))
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

    

    return;
}

// ******************************************************************
// * patch: Get2DSurfaceDescD
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(Get2DSurfaceDescD)
(
    X_D3DPixelContainer *pPixelContainer,
    X_D3DSURFACE_DESC   *pDesc
)
{
        DbgPrintf("EmuD3D8: EmuGet2DSurfaceDescD\n"
               "(\n"
               "   pPixelContainer     : 0x%.08X\n"
               "   pDesc               : 0x%.08X\n"
               ");\n",
               pPixelContainer, pDesc);
        
		EMUPATCH(Get2DSurfaceDesc)(pPixelContainer, 0xFEFEFEFE, pDesc);

    return;
}

// ******************************************************************
// * patch: IDirect3DSurface8_GetDesc
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DSurface_GetDesc)
(
    X_D3DResource      *pThis,
    X_D3DSURFACE_DESC  *pDesc
)
{
    

    DbgPrintf("EmuD3D8: EmuIDirect3DSurface8_GetDesc\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
           "   pDesc               : 0x%.08X\n"
           ");\n",
           pThis, pDesc);

    HRESULT hRet;

    EmuVerifyResourceIsRegistered(pThis);

    if(pThis->Data == X_D3DRESOURCE_DATA_YUV_SURFACE)
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

    

    return hRet;
}

// ******************************************************************
// * patch: IDirect3DSurface8_LockRect
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DSurface_LockRect)
(
    X_D3DResource      *pThis,
    D3DLOCKED_RECT     *pLockedRect,
    CONST RECT         *pRect,
    DWORD               Flags
)
{
    

    DbgPrintf("EmuD3D8: EmuIDirect3DSurface8_LockRect\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
           "   pLockedRect         : 0x%.08X\n"
           "   pRect               : 0x%.08X\n"
           "   Flags               : 0x%.08X\n"
           ");\n",
           pThis, pLockedRect, pRect, Flags);

    HRESULT hRet;

//	DbgPrintf("EmuD3D8: EmuIDirect3DSurface8_LockRect (pThis->Surface = 0x%8.8X)\n", pThis->EmuSurface8 );

	if(!pThis->EmuSurface8)
	{
		EmuWarning("Invalid Surface!" );
		__asm int 3;
		
		return E_FAIL;
	}
	
    EmuVerifyResourceIsRegistered(pThis);

    if(pThis->Data == X_D3DRESOURCE_DATA_YUV_SURFACE)
    {
        pLockedRect->Pitch = g_dwOverlayP;
        pLockedRect->pBits = (PVOID)pThis->Lock;

        hRet = D3D_OK;
    }
    else
    {
        IDirect3DSurface8 *pSurface8 = pThis->EmuSurface8;

        DWORD NewFlags = 0;

        if(Flags & X_D3DLOCK_READONLY)
            NewFlags |= D3DLOCK_READONLY;

        if(Flags & X_D3DLOCK_TILED)
            EmuWarning("D3DLOCK_TILED ignored!");

        if(!(Flags & X_D3DLOCK_READONLY) && !(Flags & X_D3DLOCK_TILED) && Flags != 0)
            CxbxKrnlCleanup("EmuIDirect3DSurface8_LockRect: Unknown Flags! (0x%.08X)", Flags);

		try
		{
			// Remove old lock(s)
			pSurface8->UnlockRect();

			hRet = pSurface8->LockRect(pLockedRect, pRect, NewFlags);

			if(FAILED(hRet))
				EmuWarning("LockRect Failed!");
		}
		catch(...)
		{
			EmuWarning("Invalid Surface!");
		}
    }

    

    return hRet;
}

// ******************************************************************
// * patch: IDirect3DBaseTexture8_GetLevelCount
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(D3DBaseTexture_GetLevelCount)
(
    X_D3DBaseTexture   *pThis
)
{
    

    DbgPrintf("EmuD3D8: EmuIDirect3DBaseTexture8_GetLevelCount\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
           ");\n",
           pThis);

	DWORD dwRet = 0;

	if( pThis )
	{
		EmuVerifyResourceIsRegistered(pThis);

		IDirect3DBaseTexture8 *pBaseTexture8 = pThis->EmuBaseTexture8;

		if( pBaseTexture8 )
			dwRet = pBaseTexture8->GetLevelCount();
	}

    

    return dwRet;
}

// ******************************************************************
// * patch: IDirect3DTexture8_GetSurfaceLevel2
// ******************************************************************
XTL::X_D3DResource * WINAPI XTL::EMUPATCH(D3DTexture_GetSurfaceLevel2)
(
    X_D3DTexture   *pThis,
    UINT            Level
)
{
    X_D3DSurface *pSurfaceLevel;

    // In a special situation, we are actually returning a memory ptr with high bit set
    if(pThis->Data == X_D3DRESOURCE_DATA_YUV_SURFACE)
    {
        DWORD dwSize = g_dwOverlayP*g_dwOverlayH;

        DWORD *pRefCount = (DWORD*)((DWORD)pThis->Lock + dwSize);

        // initialize ref count
        (*pRefCount)++;

        return pThis;
    }

	EMUPATCH(D3DTexture_GetSurfaceLevel)(pThis, Level, &pSurfaceLevel);

    return pSurfaceLevel;
}

// ******************************************************************
// * patch: IDirect3DTexture8_LockRect
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DTexture_LockRect)
(
    X_D3DTexture   *pThis,
    UINT            Level,
    D3DLOCKED_RECT *pLockedRect,
    CONST RECT     *pRect,
    DWORD           Flags
)
{
    

    DbgPrintf("EmuD3D8: EmuIDirect3DTexture8_LockRect\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
           "   Level               : 0x%.08X\n"
           "   pLockedRect         : 0x%.08X\n"
           "   pRect               : 0x%.08X\n"
           "   Flags               : 0x%.08X\n"
           ");\n",
           pThis, Level, pLockedRect, pRect, Flags);

    HRESULT hRet = D3D_OK;

	DbgPrintf("EmuD3D8: EmuIDirect3DTexture8_LockRect (pThis->Texture = 0x%8.8X)\n", pThis->EmuTexture8);

    EmuVerifyResourceIsRegistered(pThis);

    // check if we have an unregistered YUV2 resource
    if( (pThis != nullptr) && (pThis->Data == X_D3DRESOURCE_DATA_YUV_SURFACE))
    {
        pLockedRect->Pitch = g_dwOverlayP;
        pLockedRect->pBits = (PVOID)pThis->Lock;

        hRet = D3D_OK;
    }
    else
    {
        IDirect3DTexture8 *pTexture8 = pThis->EmuTexture8;

        DWORD NewFlags = 0;

        if(Flags & X_D3DLOCK_READONLY)
            NewFlags |= D3DLOCK_READONLY;

        if(Flags & X_D3DLOCK_TILED)
            EmuWarning("D3DLOCK_TILED ignored!"); 

        if(Flags & X_D3DLOCK_NOOVERWRITE)
            NewFlags |= D3DLOCK_NOOVERWRITE;

        if(Flags & X_D3DLOCK_NOFLUSH)
            EmuWarning("D3DLOCK_NOFLUSH ignored!");

        if(!(Flags & X_D3DLOCK_READONLY) && !(Flags & X_D3DLOCK_TILED) && !(Flags & X_D3DLOCK_NOOVERWRITE) && !(Flags & X_D3DLOCK_NOFLUSH) && Flags != 0)
            CxbxKrnlCleanup("EmuIDirect3DTexture8_LockRect: Unknown Flags! (0x%.08X)", Flags);

		if (pTexture8 != nullptr) {
			pTexture8->UnlockRect(Level);
			hRet = pTexture8->LockRect(Level, pLockedRect, pRect, NewFlags);
		}

		pThis->Common |= X_D3DCOMMON_ISLOCKED;
	}

    return hRet;
}

// ******************************************************************
// * patch: IDirect3DTexture8_GetSurfaceLevel
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DTexture_GetSurfaceLevel)
(
    X_D3DTexture       *pThis,
    UINT                Level,
    X_D3DSurface      **ppSurfaceLevel
)
{
    

    DbgPrintf("EmuD3D8: EmuIDirect3DTexture8_GetSurfaceLevel\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
           "   Level               : 0x%.08X\n"
           "   ppSurfaceLevel      : 0x%.08X\n"
           ");\n",
           pThis, Level, ppSurfaceLevel);

    HRESULT hRet;

    EmuVerifyResourceIsRegistered(pThis);

	if(pThis)
	{
		// if highest bit is set, this is actually a raw memory pointer (for YUY2 simulation)
		if(pThis->Data == X_D3DRESOURCE_DATA_YUV_SURFACE)
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

			*ppSurfaceLevel = EmuNewD3DSurface();

			(*ppSurfaceLevel)->Data = X_D3DRESOURCE_DATA_SURFACE_LEVEL;
			(*ppSurfaceLevel)->Format = 0;
			(*ppSurfaceLevel)->Size = 0;

			hRet = pTexture8->GetSurfaceLevel(Level, &((*ppSurfaceLevel)->EmuSurface8));

			if(FAILED(hRet))
			{
				EmuWarning("EmuIDirect3DTexture8_GetSurfaceLevel Failed!");
			}
			else
			{
				DbgPrintf("EmuD3D8: EmuIDirect3DTexture8_GetSurfaceLevel := 0x%.08X\n", (*ppSurfaceLevel)->EmuSurface8);
			}
		}
	}

    

    return hRet;
}

// ******************************************************************
// * patch: IDirect3DVolumeTexture8_LockBox
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DVolumeTexture_LockBox)
(
    X_D3DVolumeTexture *pThis,
    UINT                Level,
    D3DLOCKED_BOX      *pLockedVolume,
    CONST D3DBOX       *pBox,
    DWORD               Flags
)
{
    

    DbgPrintf("EmuD3D8: EmuIDirect3DVolumeTexture8_LockBox\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
           "   Level               : 0x%.08X\n"
           "   pLockedVolume       : 0x%.08X\n"
           "   pBox                : 0x%.08X\n"
           "   Flags               : 0x%.08X\n"
           ");\n",
           pThis, Level, pLockedVolume, pBox, Flags);

    EmuVerifyResourceIsRegistered(pThis);

    IDirect3DVolumeTexture8 *pVolumeTexture8 = pThis->EmuVolumeTexture8;

    HRESULT hRet = pVolumeTexture8->LockBox(Level, pLockedVolume, pBox, Flags);

    if(FAILED(hRet))
        EmuWarning("LockBox Failed!");

    

    return hRet;
}

// ******************************************************************
// * patch: IDirect3DCubeTexture8_LockRect
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DCubeTexture_LockRect)
(
    X_D3DCubeTexture   *pThis,
    D3DCUBEMAP_FACES    FaceType,
    UINT                Level,
    D3DLOCKED_RECT     *pLockedBox,
    CONST RECT         *pRect,
    DWORD               Flags
)
{
    

    DbgPrintf("EmuD3D8: EmuIDirect3DCubeTexture8_LockRect\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
           "   FaceType            : 0x%.08X\n"
           "   Level               : 0x%.08X\n"
           "   pLockedBox          : 0x%.08X\n"
           "   pRect               : 0x%.08X\n"
           "   Flags               : 0x%.08X\n"
           ");\n",
           pThis, FaceType, Level, pLockedBox, pRect, Flags);

    EmuVerifyResourceIsRegistered(pThis);

    IDirect3DCubeTexture8 *pCubeTexture8 = pThis->EmuCubeTexture8;

    HRESULT hRet = pCubeTexture8->LockRect(FaceType, Level, pLockedBox, pRect, Flags);

    

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_Release
// ******************************************************************
ULONG WINAPI XTL::EMUPATCH(D3DDevice_Release)()
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_Release();\n");

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

    

    return RefCount;
}

// ******************************************************************
// * patch: D3DDevice_CreateVertexBuffer
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_CreateVertexBuffer)
(
    UINT                Length,
    DWORD               Usage,
    DWORD               FVF,
    D3DPOOL             Pool,
    X_D3DVertexBuffer **ppVertexBuffer
)
{
    *ppVertexBuffer = EMUPATCH(D3DDevice_CreateVertexBuffer2)(Length);

    return D3D_OK;
}

// ******************************************************************
// * patch: D3DDevice_CreateVertexBuffer2
// ******************************************************************
XTL::X_D3DVertexBuffer* WINAPI XTL::EMUPATCH(D3DDevice_CreateVertexBuffer2)
(
    UINT Length
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_CreateVertexBuffer2\n"
           "(\n"
           "   Length              : 0x%.08X\n"
           ");\n",
           Length);

    X_D3DVertexBuffer *pD3DVertexBuffer = EmuNewD3DVertexBuffer();

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

    

    return pD3DVertexBuffer;
}

// ******************************************************************
// * patch: D3DDevice_EnableOverlay
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_EnableOverlay)
(
    BOOL Enable
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_EnableOverlay\n"
           "(\n"
           "   Enable              : 0x%.08X\n"
           ");\n",
           Enable);

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

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_UpdateOverlay
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_UpdateOverlay)
(
    X_D3DSurface *pSurface,
    CONST RECT   *SrcRect,
    CONST RECT   *DstRect,
    BOOL          EnableColorKey,
    D3DCOLOR      ColorKey
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_UpdateOverlay\n"
           "(\n"
           "   pSurface            : 0x%.08X\n"
           "   SrcRect             : 0x%.08X\n"
           "   DstRect             : 0x%.08X\n"
           "   EnableColorKey      : 0x%.08X\n"
           "   ColorKey            : 0x%.08X\n"
           ");\n",
           pSurface, SrcRect, DstRect, EnableColorKey, ColorKey);

	if(pSurface)
	{
		// manually copy data over to overlay
		if(g_bSupportsYUY2)
		{
			DDSURFACEDESC2  ddsd2;

			ZeroMemory(&ddsd2, sizeof(ddsd2));

			ddsd2.dwSize = sizeof(ddsd2);

			if(FAILED(g_pDDSOverlay7->Lock(NULL, &ddsd2, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL)))
				EmuWarning("Unable to lock overlay surface!");

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
			RECT SourRect = {0, 0, (LONG)g_dwOverlayW, (LONG)g_dwOverlayH}, DestRect;
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

			// Update overlay if present was not called since the last call to
			// EmuD3DDevice_UpdateOverlay.
			if(g_bHackUpdateSoftwareOverlay)
				g_pD3DDevice8->Present(0, 0, 0, 0);

			g_bHackUpdateSoftwareOverlay = TRUE;
		}
	}
	else
	{
		EmuWarning("pSurface == NULL!");
	}   

    return;
}
// ******************************************************************
// * patch: D3DDevice_GetOverlayUpdateStatus
// ******************************************************************
BOOL WINAPI XTL::EMUPATCH(D3DDevice_GetOverlayUpdateStatus)()
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_GetOverlayUpdateStatus();\n");

    

    // TODO: Actually check for update status
    return FALSE;
}

// ******************************************************************
// * patch: D3DDevice_BlockUntilVerticalBlank
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_BlockUntilVerticalBlank)()
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_BlockUntilVerticalBlank();\n");

    // segaGT tends to freeze with this on
//    if(g_XBVideo.GetVSync())
//        g_pDD7->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, 0);

	// HACK: For many games (when used in mutithreaded code), using 
	// DDraw::WaitForVerticalBlank will wreak havoc on CPU usage and really
	// slow things down. This is the case for various SEGA and other Japanese
	// titles.  On Xbox, it isn't a big deal, but for PC, I can't even
	// guarantee this is a good idea.  So instead, I'll be "faking" 
	// the vertical blank thing.

	Sleep( 1000/60 );

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetVerticalBlankCallback
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetVerticalBlankCallback)
(
    D3DVBLANKCALLBACK pCallback
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetVerticalBlankCallback\n"
           "(\n"
           "   pCallback           : 0x%.08X\n"
           ");\n",
           pCallback);

    g_pVBCallback = pCallback;

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetTextureState_TexCoordIndex
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetTextureState_TexCoordIndex)
(
    DWORD Stage,
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetTextureState_TexCoordIndex\n"
           "(\n"
           "   Stage               : 0x%.08X\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Stage, Value);

	// TODO: Xbox Direct3D supports sphere mapping OpenGL style.

	// BUG FIX: The lower 16 bits were causing false Unknown TexCoordIndex errors.
	// Check for 0x00040000 instead.

    if(Value >= 0x00040000)
        CxbxKrnlCleanup("EmuD3DDevice_SetTextureState_TexCoordIndex: Unknown TexCoordIndex Value (0x%.08X)", Value);

    g_pD3DDevice8->SetTextureStageState(Stage, D3DTSS_TEXCOORDINDEX, Value);

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_TwoSidedLighting
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_TwoSidedLighting)
(
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_TwoSidedLighting\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

    EmuWarning("TwoSidedLighting is not supported!");

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_BackFillMode
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_BackFillMode)
(
    DWORD Value
)
{
    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_BackFillMode\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

	// blueshogun96 12/4/07
	// I haven't had access to Cxbx sources in a few months, great to be back :)
	//
	// Anyway, since standard Direct3D doesn't support the back fill mode
	// operation, this function will be ignored.  Things like this make me
	// think even more that an OpenGL port wouldn't hurt since OpenGL supports
	// nearly all of the missing features that Direct3D lacks.  The Xbox's version
	// of Direct3D was specifically created to take advantage of certain NVIDIA
	// GPU registers and provide more OpenGL-like features IHMO.
	EmuWarning("BackFillMode is not supported!");
}

// ******************************************************************
// * patch: D3DDevice_SetTextureState_BorderColor
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetTextureState_BorderColor)
(
    DWORD Stage,
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetTextureState_BorderColor\n"
           "(\n"
           "   Stage               : 0x%.08X\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Stage, Value);

    g_pD3DDevice8->SetTextureStageState(Stage, D3DTSS_BORDERCOLOR, Value);

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetTextureState_ColorKeyColor
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetTextureState_ColorKeyColor)
(
    DWORD Stage,
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetTextureState_ColorKeyColor\n"
           "(\n"
           "   Stage               : 0x%.08X\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Stage, Value);

    EmuWarning("SetTextureState_ColorKeyColor is not supported!");

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetTextureState_BumpEnv
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetTextureState_BumpEnv)
(
    DWORD                      Stage,
    X_D3DTEXTURESTAGESTATETYPE Type,
    DWORD                      Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetTextureState_BumpEnv\n"
           "(\n"
           "   Stage               : 0x%.08X\n"
           "   Type                : 0x%.08X\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Stage, Type, Value);

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

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_FrontFace
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_FrontFace)
(
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_FrontFace\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

    EmuWarning("SetRenderState_FrontFace not supported!");

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_LogicOp
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_LogicOp)
(
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_LogicOp\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

    EmuWarning("SetRenderState_LogicOp is not supported!");

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_NormalizeNormals
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_NormalizeNormals)
(
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_NormalizeNormals\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

    g_pD3DDevice8->SetRenderState(D3DRS_NORMALIZENORMALS, Value);

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_TextureFactor
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_TextureFactor)
(
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_TextureFactor\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

    g_pD3DDevice8->SetRenderState(D3DRS_TEXTUREFACTOR, Value);

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_ZBias
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_ZBias)
(
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_ZBias\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

    g_pD3DDevice8->SetRenderState(D3DRS_ZBIAS, Value);

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_EdgeAntiAlias
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_EdgeAntiAlias)
(
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_EdgeAntiAlias\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

//  TODO: Analyze performance and compatibility (undefined behavior on PC with triangles or points)
//  g_pD3DDevice8->SetRenderState(D3DRS_EDGEANTIALIAS, Value);

//    EmuWarning("SetRenderState_EdgeAntiAlias not implemented!");

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_FillMode
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_FillMode)
(
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_FillMode\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

    DWORD dwFillMode;

    if(g_iWireframe == 0)
        dwFillMode = EmuXB2PC_D3DFILLMODE(Value);
    else if(g_iWireframe == 1)
        dwFillMode = D3DFILL_WIREFRAME;
    else
        dwFillMode = D3DFILL_POINT;

    g_pD3DDevice8->SetRenderState(D3DRS_FILLMODE, dwFillMode);

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_FogColor
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_FogColor)
(
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_FogColor\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

    g_pD3DDevice8->SetRenderState(D3DRS_FOGCOLOR, Value);

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_Dxt1NoiseEnable
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_Dxt1NoiseEnable)
(
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_Dxt1NoiseEnable\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

    EmuWarning("SetRenderState_Dxt1NoiseEnable not implemented!");

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_Simple
// ******************************************************************
VOID __fastcall XTL::EMUPATCH(D3DDevice_SetRenderState_Simple)
(
    DWORD Method,
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_Simple\n"
           "(\n"
           "   Method              : 0x%.08X\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Method, Value);

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
                Value = EmuXB2PC_D3DSHADEMODE(Value);
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

			case D3DRS_STENCILZFAIL:
				Value = EmuXB2PC_D3DSTENCILOP(Value);
				DbgPrintf("D3DRS_STENCILZFAIL := 0x%.08X\n", Value);
				break;

			case D3DRS_STENCILPASS:
				Value = EmuXB2PC_D3DSTENCILOP(Value);
				DbgPrintf("D3DRS_STENCILPASS := 0x%.08X\n", Value);
				break;

			case D3DRS_STENCILFUNC:
				Value = EmuXB2PC_D3DCMPFUNC(Value);
				DbgPrintf("D3DRS_STENCILFUNC := 0x%.08X\n", Value);
				break;

			case D3DRS_STENCILREF:
				DbgPrintf("D3DRS_STENCILREF := 0x%.08X\n", Value);
				break;

			case D3DRS_STENCILMASK:
				DbgPrintf("D3DRS_STENCILMASK := 0x%.08X\n", Value);
				break;

			case D3DRS_STENCILWRITEMASK:
				DbgPrintf("D3DRS_STENCILWRITEMASK := 0x%.08X\n", Value);
				break;

            default:
                CxbxKrnlCleanup("Unsupported RenderState (0x%.08X)", State);
                break;
        };

        // TODO: verify these params as you add support for them!
        g_pD3DDevice8->SetRenderState((D3DRENDERSTATETYPE)State, Value);
    }

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_VertexBlend
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_VertexBlend)
(
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_VertexBlend\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

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

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_PSTextureModes
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_PSTextureModes)
(
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_PSTextureModes\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

    // TODO: do something..

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_CullMode
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_CullMode)
(
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_CullMode\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

    // convert from Xbox D3D to PC D3D enumeration
    // TODO: XDK-Specific Tables? So far they are the same
    switch(Value)
    {
        case X_D3DCULL_NONE:
            Value = D3DCULL_NONE;
            break;
        case X_D3DCULL_CW:
            Value = D3DCULL_CW;
            break;
        case X_D3DCULL_CCW:
            Value = D3DCULL_CCW;
            break;
        default:
            CxbxKrnlCleanup("EmuD3DDevice_SetRenderState_CullMode: Unknown Cullmode (%d)", Value);
    }

    g_pD3DDevice8->SetRenderState(D3DRS_CULLMODE, Value);

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_LineWidth
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_LineWidth)
(
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_LineWidth\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

    // TODO: Convert to PC format??
//    g_pD3DDevice8->SetRenderState(D3DRS_LINEPATTERN, Value);
    EmuWarning("SetRenderState_LineWidth is not supported!");

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_StencilFail
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_StencilFail)
(
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_StencilFail\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

    g_pD3DDevice8->SetRenderState(D3DRS_STENCILFAIL, Value);

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_OcclusionCullEnable
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_OcclusionCullEnable)
(
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_OcclusionCullEnable\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

    EmuWarning("SetRenderState_OcclusionCullEnable not supported!");

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_StencilCullEnable
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_StencilCullEnable)
(
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_StencilCullEnable\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

    EmuWarning("SetRenderState_StencilCullEnable not supported!");

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_RopZCmpAlwaysRead
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_RopZCmpAlwaysRead)
(
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_RopZCmpAlwaysRead\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

    EmuWarning("SetRenderState_RopZCmpAlwaysRead not supported!");

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_RopZRead
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_RopZRead)
(
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_RopZRead\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

    EmuWarning("SetRenderState_RopZRead not supported!");

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_DoNotCullUncompressed
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_DoNotCullUncompressed)
(
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_DoNotCullUncompressed\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

    EmuWarning("SetRenderState_DoNotCullUncompressed not supported!");

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_ZEnable
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_ZEnable)
(
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_ZEnable\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

    g_pD3DDevice8->SetRenderState(D3DRS_ZENABLE, Value);

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_StencilEnable
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_StencilEnable)
(
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_StencilEnable\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

    g_pD3DDevice8->SetRenderState(D3DRS_STENCILENABLE, Value);

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_MultiSampleAntiAlias
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_MultiSampleAntiAlias)
(
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_MultiSampleAntiAlias\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

    g_pD3DDevice8->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, Value);

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_MultiSampleMask
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_MultiSampleMask)
(
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_MultiSampleMask\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

    g_pD3DDevice8->SetRenderState(D3DRS_MULTISAMPLEMASK, Value);

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_MultiSampleMode
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_MultiSampleMode)
(
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_MultiSampleMode\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

    EmuWarning("SetRenderState_MultiSampleMode is not supported!");

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_MultiSampleRenderTargetMode
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_MultiSampleRenderTargetMode)
(
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_MultiSampleRenderTargetMode\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

    EmuWarning("SetRenderState_MultiSampleRenderTargetMode is not supported!");

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_ShadowFunc
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_ShadowFunc)
(
    DWORD Value
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_ShadowFunc\n"
           "(\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           Value);

    // ShadowFunc reflects the following Xbox-only extension
    //
    // typedef enum _D3DRENDERSTATETYPE {
    //   ...
    //   D3DRS_SHADOWFUNC = 156, // D3DCMPFUNC
    //   ...
    // } D3DRENDERSTATETYPE;
    //
    // Value is a member of the D3DCMPFUNC enumeration that 
    // specifies what function to use with a shadow buffer. 
    // The default value is D3DCMP_NEVER. 

    // EmuXB2PC_D3DCMPFUNC(Value);

    // this warning just gets annoying
    // EmuWarning("ShadowFunc not implemented");

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_YuvEnable
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_YuvEnable)
(
    BOOL Enable
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_YuvEnable\n"
           "(\n"
           "   Enable              : 0x%.08X\n"
           ");\n",
           Enable);

    // HACK: Display YUV surface by using an overlay.
    if(Enable != g_fYuvEnabled)
    {
        g_fYuvEnabled = Enable;

        EmuWarning("EmuD3DDevice_SetRenderState_YuvEnable using overlay!");

        
		EMUPATCH(D3DDevice_EnableOverlay)(g_fYuvEnabled);
        
    }

    if(g_fYuvEnabled)
    {
        
		EMUPATCH(D3DDevice_UpdateOverlay)(g_pCachedYuvSurface, 0, 0, FALSE, 0);
        
    }

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetTransform
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetTransform)
(
    D3DTRANSFORMSTATETYPE State,
    CONST D3DMATRIX      *pMatrix
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetTransform\n"
           "(\n"
           "   State               : 0x%.08X\n"
           "   pMatrix             : 0x%.08X\n"
           ");\n",
           State, pMatrix);

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

    HRESULT hRet = g_pD3DDevice8->SetTransform(State, pMatrix);

    

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_GetTransform
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_GetTransform)
(
    D3DTRANSFORMSTATETYPE State,
    D3DMATRIX            *pMatrix
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_GetTransform\n"
           "(\n"
           "   State               : 0x%.08X\n"
           "   pMatrix             : 0x%.08X\n"
           ");\n",
           State, pMatrix);

    State = EmuXB2PC_D3DTS(State);

    HRESULT hRet = g_pD3DDevice8->GetTransform(State, pMatrix);

    

    return hRet;
}

// ******************************************************************
// * patch: IDirect3DVertexBuffer8_Lock
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DVertexBuffer_Lock)
(
    X_D3DVertexBuffer  *ppVertexBuffer,
    UINT                OffsetToLock,
    UINT                SizeToLock,
    BYTE              **ppbData,
    DWORD               Flags
)
{
    

    DbgPrintf("EmuD3D8: EmuIDirect3DVertexBuffer8_Lock\n"
           "(\n"
           "   ppVertexBuffer      : 0x%.08X\n"
           "   OffsetToLock        : 0x%.08X\n"
           "   SizeToLock          : 0x%.08X\n"
           "   ppbData             : 0x%.08X\n"
           "   Flags               : 0x%.08X\n"
           ");\n",
           ppVertexBuffer, OffsetToLock, SizeToLock, ppbData, Flags);

    IDirect3DVertexBuffer8 *pVertexBuffer8 = ppVertexBuffer->EmuVertexBuffer8;

    HRESULT hRet = pVertexBuffer8->Lock(OffsetToLock, SizeToLock, ppbData, Flags);

    if(FAILED(hRet))
        EmuWarning("VertexBuffer Lock Failed!");

    

    return;
}

// ******************************************************************
// * patch: IDirect3DVertexBuffer8_Lock2
// ******************************************************************
BYTE* WINAPI XTL::EMUPATCH(D3DVertexBuffer_Lock2)
(
    X_D3DVertexBuffer  *ppVertexBuffer,
    DWORD               Flags
)
{
    

    DbgPrintf("EmuD3D8: EmuIDirect3DVertexBuffer8_Lock2\n"
           "(\n"
           "   ppVertexBuffer      : 0x%.08X\n"
           "   Flags               : 0x%.08X\n"
           ");\n",
           ppVertexBuffer, Flags);

    IDirect3DVertexBuffer8 *pVertexBuffer8 = NULL;

    BYTE *pbData = NULL;

    HRESULT hRet = S_OK;
	
	if( ppVertexBuffer->EmuVertexBuffer8 )
	{
		pVertexBuffer8 = ppVertexBuffer->EmuVertexBuffer8;
		hRet = pVertexBuffer8->Lock(0, 0, &pbData, EmuXB2PC_D3DLock(Flags));    // Fixed flags check, Battlestar Galactica now displays graphics correctly

		if( FAILED( hRet ) )
			EmuWarning( "Lock vertex buffer failed!" );
	}
	else
		EmuWarning( "ppVertexBuffer->EmuVertexBuffer8 == NULL!" );

    

    return pbData;
}

// ******************************************************************
// * patch: D3DDevice_GetStreamSource2
// ******************************************************************
XTL::X_D3DVertexBuffer* WINAPI XTL::EMUPATCH(D3DDevice_GetStreamSource2)
(
    UINT  StreamNumber,
    UINT *pStride
)
{
    

    // debug trace
    DbgPrintf( "EmuD3D8 : EmuD3DDevice_GetStreamSource2\n"
               "(\n"
               "   StreamNumber               : 0x%.08X\n"
               "   pStride                    : 0x%.08X\n"
               ");\n",
               StreamNumber, pStride);

    EmuWarning("Not correctly implemented yet!");
    X_D3DVertexBuffer* pVertexBuffer = EmuNewD3DVertexBuffer();
    g_pD3DDevice8->GetStreamSource(StreamNumber, &pVertexBuffer->EmuVertexBuffer8, pStride);

    
    return pVertexBuffer;
}

// ******************************************************************
// * patch: D3DDevice_SetStreamSource
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetStreamSource)
(
    UINT                StreamNumber,
    X_D3DVertexBuffer  *pStreamData,
    UINT                Stride
)
{
    

    DbgPrintf("EmuD3D8 : EmuD3DDevice_SetStreamSource\n"
           "(\n"
           "   StreamNumber        : 0x%.08X\n"
           "   pStreamData         : 0x%.08X (0x%.08X)\n"
           "   Stride              : 0x%.08X\n"
           ");\n",
           StreamNumber, pStreamData, (pStreamData != 0) ? pStreamData->EmuVertexBuffer8 : 0, Stride);

	// Cache stream number
	g_dwLastSetStream = StreamNumber;

    if(StreamNumber == 0)
        g_pVertexBuffer = pStreamData;

	// Test for a non-zero stream source.  Unreal Championship gives us
	// some funky number when going ingame.
//	if(StreamNumber != 0)
//	{
//		EmuWarning( "StreamNumber: = %d", StreamNumber );
//		EmuWarning( "pStreamData: = 0x%.08X (0x%.08X)", pStreamData, (pStreamData != 0) ? pStreamData->EmuVertexBuffer8 : NULL );
////		__asm int 3;
//	}

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

    

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShader
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetVertexShader)
(
    DWORD Handle
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetVertexShader\n"
           "(\n"
           "   Handle              : 0x%.08X\n"
           ");\n",
           Handle);

    HRESULT hRet = D3D_OK;

    g_CurrentVertexShader = Handle;

    // Store viewport offset and scale in constant registers 58 (c-38) and
    // 59 (c-37) used for screen space transformation.
    if(g_VertexShaderConstantMode != X_VSCM_NONERESERVED)
    {
        // TODO: Proper solution.
        static float vScale[] = { (2.0f / 640), (-2.0f / 480), 0.0f, 0.0f };
        static float vOffset[] = { -1.0f, 1.0f, 0.0f, 1.0f };

        g_pD3DDevice8->SetVertexShaderConstant(58, vScale, 1);
        g_pD3DDevice8->SetVertexShaderConstant(59, vOffset, 1);
    }

    DWORD RealHandle;
    if(VshHandleIsVertexShader(Handle))
    {
        RealHandle = ((VERTEX_SHADER *)(VshHandleGetVertexShader(Handle))->Handle)->Handle;
    }
    else
    {
        RealHandle = Handle;
		RealHandle &= ~D3DFVF_XYZ;
		RealHandle &= ~D3DFVF_XYZRHW;
		RealHandle &= ~D3DFVF_XYZB1;
		RealHandle &= ~D3DFVF_XYZB2;
		RealHandle &= ~D3DFVF_XYZB3;
		RealHandle &= ~D3DFVF_XYZB4;
		RealHandle &= ~D3DFVF_DIFFUSE;
		RealHandle &= ~D3DFVF_NORMAL;
		RealHandle &= ~D3DFVF_SPECULAR;
		RealHandle &= 0x00FF;
		if( RealHandle != 0 )
			EmuWarning("EmuD3DDevice_SetVertexShader Handle = 0x%.08X", RealHandle );
		RealHandle = Handle;
    }
    hRet = g_pD3DDevice8->SetVertexShader(RealHandle);

    

    return hRet;
}

void CxbxUpdateNativeD3DResources()
{
	XTL::EmuUpdateDeferredStates();
	EmuUnswizzleTextureStages();
/* TODO : Port these :
	DxbxUpdateActiveVertexShader();
	DxbxUpdateActiveTextures();
	DxbxUpdateActivePixelShader();
	DxbxUpdateDeferredStates(); // BeginPush sample shows us that this must come *after* texture update!
	DxbxUpdateActiveVertexBufferStreams();
	DxbxUpdateActiveRenderTarget();
*/
}

// ******************************************************************
// * patch: D3DDevice_DrawVertices
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_DrawVertices)
(
    X_D3DPRIMITIVETYPE PrimitiveType,
    UINT               StartVertex,
    UINT               VertexCount
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_DrawVertices\n"
           "(\n"
           "   PrimitiveType       : 0x%.08X\n"
           "   StartVertex         : 0x%.08X\n"
           "   VertexCount         : 0x%.08X\n"
           ");\n",
           PrimitiveType, StartVertex, VertexCount);

	// Dxbx Note : In DrawVertices and DrawIndexedVertices, PrimitiveType may not be D3DPT_POLYGON

	CxbxUpdateNativeD3DResources();

	VertexPatchDesc VPDesc;

    VPDesc.PrimitiveType = PrimitiveType;
    VPDesc.dwVertexCount = VertexCount;
    VPDesc.dwOffset = StartVertex;
    VPDesc.pVertexStreamZeroData = 0;
    VPDesc.uiVertexStreamZeroStride = 0;
    VPDesc.hVertexShader = g_CurrentVertexShader;

    VertexPatcher VertPatch;

    bool bPatched = VertPatch.Apply(&VPDesc, NULL);

    if(IsValidCurrentShader())
    {
        #ifdef _DEBUG_TRACK_VB
        if(!g_bVBSkipStream)
        {
        #endif

        g_pD3DDevice8->DrawPrimitive
        (
            EmuXB2PC_D3DPrimitiveType(VPDesc.PrimitiveType),
            StartVertex,
            VPDesc.dwPrimitiveCount
        );

		g_dwPrimPerFrame += VPDesc.dwPrimitiveCount;

        #ifdef _DEBUG_TRACK_VB
        }
        #endif
    }

    VertPatch.Restore();

	// Execute callback procedure
	if( g_CallbackType == X_D3DCALLBACK_WRITE )
	{
		if( g_pCallback )
		{
			
			g_pCallback(g_CallbackParam);
			

			// TODO: Reset pointer?
		}
	}

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_DrawVerticesUP
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_DrawVerticesUP)
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    UINT                VertexCount,
    CONST PVOID         pVertexStreamZeroData,
    UINT                VertexStreamZeroStride
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_DrawVerticesUP\n"
           "(\n"
           "   PrimitiveType            : 0x%.08X\n"
           "   VertexCount              : 0x%.08X\n"
           "   pVertexStreamZeroData    : 0x%.08X\n"
           "   VertexStreamZeroStride   : 0x%.08X\n"
           ");\n",
           PrimitiveType, VertexCount, pVertexStreamZeroData,
           VertexStreamZeroStride);

	CxbxUpdateNativeD3DResources();

/*#if 0
	// HACK: Phantom Crash...
//	if( (*(DWORD*)&pVertexStreamZeroData) == 0x81FC3080 || (*(DWORD*)&pVertexStreamZeroData) == 0x8191E080 )
	if( ((*(DWORD*)&pVertexStreamZeroData) & 0xFF000FFF) == 0x81000080 )
	{
//		EmuWarning( "Invalid vertex data! (0x%.08X)", pVertexStreamZeroData );
		EmuWarning( "Phantom Crash hack applied!" );
		return;
	}
#else
	return;
#endif*/

    VertexPatchDesc VPDesc;

    VPDesc.PrimitiveType = PrimitiveType;
    VPDesc.dwVertexCount = VertexCount;
    VPDesc.dwOffset = 0;
    VPDesc.pVertexStreamZeroData = pVertexStreamZeroData;
    VPDesc.uiVertexStreamZeroStride = VertexStreamZeroStride;
    VPDesc.hVertexShader = g_CurrentVertexShader;

    VertexPatcher VertPatch;

    bool bPatched = VertPatch.Apply(&VPDesc, NULL);

    if (IsValidCurrentShader())
    {
        #ifdef _DEBUG_TRACK_VB
        if(!g_bVBSkipStream)
        #endif
        {
			g_pD3DDevice8->DrawPrimitiveUP
			(
				EmuXB2PC_D3DPrimitiveType(VPDesc.PrimitiveType),
				VPDesc.dwPrimitiveCount,
				VPDesc.pVertexStreamZeroData,
				VPDesc.uiVertexStreamZeroStride
			);

			g_dwPrimPerFrame += VPDesc.dwPrimitiveCount;
        }
    }

    VertPatch.Restore();

	// Execute callback procedure
	if( g_CallbackType == X_D3DCALLBACK_WRITE )
	{
		if( g_pCallback )
		{
			
			g_pCallback(g_CallbackParam);
			

			// TODO: Reset pointer?
		}
	}

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_DrawIndexedVertices
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_DrawIndexedVertices)
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    UINT                VertexCount,
    CONST PWORD         pIndexData
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_DrawIndexedVertices\n"
           "(\n"
           "   PrimitiveType       : 0x%.08X\n"
           "   VertexCount         : 0x%.08X\n"
           "   pIndexData          : 0x%.08X\n"
           ");\n",
           PrimitiveType, VertexCount, pIndexData);
//#if 0
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

	CxbxUpdateNativeD3DResources();

    if( (PrimitiveType == X_D3DPT_LINELOOP) || (PrimitiveType == X_D3DPT_QUADLIST) )
        EmuWarning("Unsupported PrimitiveType! (%d)", (DWORD)PrimitiveType);

	// HACK: Azurik :(
//	if( ((DWORD)PrimitiveType) > 11 )
	/*if( PrimitiveType != X_D3DPT_POINTLIST && PrimitiveType != X_D3DPT_LINELIST && 
		PrimitiveType != X_D3DPT_LINELOOP && PrimitiveType != X_D3DPT_LINESTRIP &&
		PrimitiveType != X_D3DPT_TRIANGLELIST && PrimitiveType != X_D3DPT_TRIANGLESTRIP && 
		PrimitiveType != X_D3DPT_TRIANGLEFAN && PrimitiveType != X_D3DPT_QUADLIST &&
		PrimitiveType != X_D3DPT_QUADSTRIP && PrimitiveType != X_D3DPT_POLYGON )
	{
		CxbxKrnlCleanup("Invalid Primitive type! %d (0x%X)", (int)PrimitiveType, (DWORD)PrimitiveType);
	}*/

    VertexPatchDesc VPDesc;

    VPDesc.PrimitiveType = PrimitiveType;
    VPDesc.dwVertexCount = VertexCount;
    VPDesc.dwOffset = 0;
    VPDesc.pVertexStreamZeroData = 0;
    VPDesc.uiVertexStreamZeroStride = 0;
    VPDesc.hVertexShader = g_CurrentVertexShader;

    VertexPatcher VertPatch;
	bool FatalError = false;

    bool bPatched = VertPatch.Apply(&VPDesc, &FatalError);

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
        if(FAILED(g_pD3DDevice8->CreateIndexBuffer(VertexCount*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pIndexBuffer)))
			CxbxKrnlCleanup("Cound not create index buffer! (%d bytes)", VertexCount*2);

        if(pIndexBuffer == 0)
            CxbxKrnlCleanup("Could not create index buffer! (%d bytes)", VertexCount*2);

        BYTE *pbData = 0;

        pIndexBuffer->Lock(0, 0, &pbData, 0);

        if(pbData == 0)
            CxbxKrnlCleanup("Could not lock index buffer!");

		if(pIndexData)
			memcpy(pbData, pIndexData, VertexCount*2);

        pIndexBuffer->Unlock();

        g_pD3DDevice8->SetIndices(pIndexBuffer, g_dwBaseVertexIndex);

        uiNumVertices = VertexCount;
        uiStartIndex = 0;
    }
    else
    {
        uiNumVertices = ((DWORD)pIndexData)/2 + VertexCount;
        uiStartIndex = ((DWORD)pIndexData)/2;
    }

    if(IsValidCurrentShader() && !FatalError)
    {
        g_pD3DDevice8->DrawIndexedPrimitive
        (
            EmuXB2PC_D3DPrimitiveType(VPDesc.PrimitiveType), 0, uiNumVertices, uiStartIndex, VPDesc.dwPrimitiveCount
        );

		g_dwPrimPerFrame += VPDesc.dwPrimitiveCount;

		/*if( (PrimitiveType == X_D3DPT_LINELOOP) || (PrimitiveType == X_D3DPT_QUADLIST) ) 
		{ 
			g_pD3DDevice8->DrawPrimitive 
			( 
				EmuXB2PC_D3DPrimitiveType(VPDesc.PrimitiveType), 0, VPDesc.dwPrimitiveCount 
			); 
		} 
		else 
		{ 
			g_pD3DDevice8->DrawIndexedPrimitive 
			( 
				EmuXB2PC_D3DPrimitiveType(VPDesc.PrimitiveType), 0, uiNumVertices, uiStartIndex, VPDesc.dwPrimitiveCount 
			); 
		} */
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

	// Execute callback procedure
	if( g_CallbackType == X_D3DCALLBACK_WRITE )
	{
		if( g_pCallback )
		{
			
			g_pCallback(g_CallbackParam);
			

			// TODO: Reset pointer?
		}
	}

//#endif
}

// ******************************************************************
// * patch: D3DDevice_DrawIndexedVerticesUP
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_DrawIndexedVerticesUP)
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    UINT                VertexCount,
    CONST PVOID         pIndexData,
    CONST PVOID         pVertexStreamZeroData,
    UINT                VertexStreamZeroStride
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_DrawIndexedVerticesUP\n"
           "(\n"
           "   PrimitiveType            : 0x%.08X\n"
           "   VertexCount              : 0x%.08X\n"
           "   pIndexData               : 0x%.08X\n"
           "   pVertexStreamZeroData    : 0x%.08X\n"
           "   VertexStreamZeroStride   : 0x%.08X\n"
           ");\n",
           PrimitiveType, VertexCount, pIndexData, pVertexStreamZeroData, VertexStreamZeroStride);

    // update index buffer, if necessary
    if(g_pIndexBuffer != 0 && g_pIndexBuffer->Lock == X_D3DRESOURCE_LOCK_FLAG_NOSIZE)
        CxbxKrnlCleanup("g_pIndexBuffer != 0");

	CxbxUpdateNativeD3DResources();
	
    if( (PrimitiveType == X_D3DPT_LINELOOP) || (PrimitiveType == X_D3DPT_QUADLIST) )
        EmuWarning("Unsupported PrimitiveType! (%d)", (DWORD)PrimitiveType);

    VertexPatchDesc VPDesc;

    VPDesc.PrimitiveType = PrimitiveType;
    VPDesc.dwVertexCount = VertexCount;
    VPDesc.dwOffset = 0;
    VPDesc.pVertexStreamZeroData = pVertexStreamZeroData;
    VPDesc.uiVertexStreamZeroStride = VertexStreamZeroStride;
    VPDesc.hVertexShader = g_CurrentVertexShader;

    VertexPatcher VertPatch;

    bool bPatched = VertPatch.Apply(&VPDesc, NULL);

    #ifdef _DEBUG_TRACK_VB
    if(!g_bVBSkipStream)
    {
    #endif

    if (IsValidCurrentShader())
    {
        g_pD3DDevice8->DrawIndexedPrimitiveUP
        (
            EmuXB2PC_D3DPrimitiveType(VPDesc.PrimitiveType), 0, VPDesc.dwVertexCount, VPDesc.dwPrimitiveCount, pIndexData, D3DFMT_INDEX16, VPDesc.pVertexStreamZeroData, VPDesc.uiVertexStreamZeroStride
        );

		g_dwPrimPerFrame += VPDesc.dwPrimitiveCount;
    }

    #ifdef _DEBUG_TRACK_VB
    }
    #endif

    VertPatch.Restore();

	// Execute callback procedure
	if( g_CallbackType == X_D3DCALLBACK_WRITE )
	{
		if( g_pCallback )
		{
			
			g_pCallback(g_CallbackParam);
			

			// TODO: Reset pointer?
		}
	}

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetLight
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetLight)
(
    DWORD            Index,
    CONST D3DLIGHT8 *pLight
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetLight\n"
           "(\n"
           "   Index               : 0x%.08X\n"
           "   pLight              : 0x%.08X\n"
           ");\n",
           Index, pLight);

    HRESULT hRet = g_pD3DDevice8->SetLight(Index, pLight);

    

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_SetMaterial
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetMaterial)
(
    CONST D3DMATERIAL8 *pMaterial
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetMaterial\n"
           "(\n"
           "   pMaterial           : 0x%.08X\n"
           ");\n",
           pMaterial);

    HRESULT hRet = g_pD3DDevice8->SetMaterial(pMaterial);

    

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_LightEnable
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_LightEnable)
(
    DWORD            Index,
    BOOL             bEnable
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_LightEnable\n"
           "(\n"
           "   Index               : 0x%.08X\n"
           "   bEnable             : 0x%.08X\n"
           ");\n",
           Index, bEnable);

    HRESULT hRet = g_pD3DDevice8->LightEnable(Index, bEnable);

    

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderTarget
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetRenderTarget)
(
    X_D3DSurface    *pRenderTarget,
    X_D3DSurface    *pNewZStencil
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderTarget\n"
           "(\n"
           "   pRenderTarget       : 0x%.08X (0x%.08X)\n"
           "   pNewZStencil        : 0x%.08X (0x%.08X)\n"
           ");\n",
           pRenderTarget, (pRenderTarget != 0) ? pRenderTarget->EmuSurface8 : 0, pNewZStencil,
           (pNewZStencil != 0) ? pNewZStencil->EmuSurface8 : 0);

    IDirect3DSurface8 *pPCRenderTarget = 0;
    IDirect3DSurface8 *pPCNewZStencil  = 0;

    if(pRenderTarget != 0)
    {
		if(pRenderTarget->EmuSurface8)
		{
			EmuVerifyResourceIsRegistered(pRenderTarget);
			pPCRenderTarget = pRenderTarget->EmuSurface8;
		}
		else
		{
			pPCRenderTarget = g_pCachedRenderTarget->EmuSurface8;
		}
    }

    if(pNewZStencil != 0)
    {
        if(pNewZStencil->EmuSurface8 != 0)
        {
            EmuVerifyResourceIsRegistered(pNewZStencil);
            pPCNewZStencil = pNewZStencil->EmuSurface8;
        }
        else
        {
            pPCNewZStencil = g_pCachedDepthStencil->EmuSurface8;
        }
    }

    // TODO: Follow that stencil!
    HRESULT hRet = g_pD3DDevice8->SetRenderTarget(pPCRenderTarget, pPCNewZStencil);

    if(FAILED(hRet))
        EmuWarning("SetRenderTarget Failed! (0x%.08X)", hRet);

    

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_CreatePalette
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_CreatePalette)
(
    X_D3DPALETTESIZE    Size,
    X_D3DPalette      **ppPalette
)
{
    *ppPalette = EMUPATCH(D3DDevice_CreatePalette2)(Size);

    return D3D_OK;
}

// ******************************************************************
// * patch: D3DDevice_CreatePalette2
// ******************************************************************
XTL::X_D3DPalette * WINAPI XTL::EMUPATCH(D3DDevice_CreatePalette2)
(
    X_D3DPALETTESIZE    Size
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_CreatePalette2\n"
           "(\n"
           "   Size                : 0x%.08X\n"
           ");\n",
           Size);

    X_D3DPalette *pPalette = EmuNewD3DPalette();

    static int lk[4] =
    {
        256*sizeof(D3DCOLOR),    // D3DPALETTE_256
        128*sizeof(D3DCOLOR),    // D3DPALETTE_128
        64*sizeof(D3DCOLOR),     // D3DPALETTE_64
        32*sizeof(D3DCOLOR)      // D3DPALETTE_32
    };

	pPalette->Common |= (Size << 30);
    pPalette->Data = (DWORD)new uint08[lk[Size]];
    pPalette->Lock = X_D3DRESOURCE_LOCK_PALETTE; // emulated reference count for palettes

	// TODO: Should't we register the palette with a call to
	// EmuIDirect3DResource8_Register? So far, it doesn't look
	// like the palette registration code gets used.  If not, then we
	// need to cache the palette manually during any calls to
	// EmuD3DDevice_SetPalette for 8-bit textures to work properly.

    DbgPrintf("pPalette: = 0x%.08X\n", pPalette);

    

    return pPalette;
}

// ******************************************************************
// * patch: D3DDevice_SetPalette
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetPalette)
(
    DWORD         Stage,
    X_D3DPalette *pPalette
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetPalette\n"
           "(\n"
           "   Stage               : 0x%.08X\n"
           "   pPalette            : 0x%.08X\n"
           ");\n",
           Stage, pPalette);

//    g_pD3DDevice8->SetPaletteEntries(0, (PALETTEENTRY*)pPalette->Data);

	// Cache palette data and size
	if( pPalette )
	{
		if( pPalette->Data )
		{
			pCurrentPalette = (LPVOID) pPalette->Data;
			dwCurrentPaletteSize = EmuCheckAllocationSize( (LPVOID) pPalette->Data, false );
		}
	}

    EmuWarning("Not setting palette");

    

    return D3D_OK;
}

// ******************************************************************
// * patch: D3DDevice_SetFlickerFilter
// ******************************************************************
void WINAPI XTL::EMUPATCH(D3DDevice_SetFlickerFilter)
(
    DWORD         Filter
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetFlickerFilter\n"
           "(\n"
           "   Filter              : 0x%.08X\n"
           ");\n",
           Filter);

    EmuWarning("Not setting flicker filter");

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_SetSoftDisplayFilter
// ******************************************************************
void WINAPI XTL::EMUPATCH(D3DDevice_SetSoftDisplayFilter)
(
    BOOL Enable
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetSoftDisplayFilter\n"
           "(\n"
           "   Enable              : 0x%.08X\n"
           ");\n",
           Enable);

    EmuWarning("Not setting soft display filter");

    

    return;
}

// ******************************************************************
// * patch: IDirect3DPalette8_Lock
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DPalette_Lock)
(
    X_D3DPalette   *pThis,
    D3DCOLOR      **ppColors,
    DWORD           Flags
)
{
	HRESULT hRet = D3D_OK;

	if( pThis )
		*ppColors = EMUPATCH(D3DPalette_Lock2)(pThis, Flags);
	else
	{
		EmuWarning( "EmuIDirect3DPalette8_Lock: pThis == NULL!" );
		hRet = E_FAIL;
	}

    return hRet;
}

// ******************************************************************
// * patch: IDirect3DPalette8_Lock2
// ******************************************************************
XTL::D3DCOLOR * WINAPI XTL::EMUPATCH(D3DPalette_Lock2)
(
    X_D3DPalette   *pThis,
    DWORD           Flags
)
{
    

    DbgPrintf("EmuD3D8: EmuIDirect3DPalette8_Lock\n"
           "(\n"
           "   pThis               : 0x%.08X (->Data : 0x%.08X)\n"
           "   Flags               : 0x%.08X\n"
           ");\n",
           pThis, pThis->Data, Flags);

    // If X_D3DLOCK_READONLY and X_D3DLOCK_NOOVERWRITE bitflags not set
    if( !(Flags & (X_D3DLOCK_READONLY | X_D3DLOCK_NOOVERWRITE)) )
    {
		EMUPATCH(D3DResource_BlockUntilNotBusy)(pThis);
    }

    D3DCOLOR *pColors = (D3DCOLOR*)pThis->Data;

    

    return pColors;
}

// ******************************************************************
// * patch: D3DDevice_GetVertexShaderSize
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetVertexShaderSize)
(
    DWORD Handle,
    UINT* pSize
)
{
    

    DbgPrintf( "EmuD3D8: EmuD3DDevice_GetVertexShaderSize\n"
               "(\n"
               "   Handle               : 0x%.08X\n"
               "   pSize                : 0x%.08X\n"
               ");\n",
               Handle, pSize);

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

    
}

// ******************************************************************
// * patch: D3DDevice_DeleteVertexShader
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_DeleteVertexShader)
(
    DWORD Handle
)
{
    

    DbgPrintf( "EmuD3D8: EmuD3DDevice_DeleteVertexShader\n"
               "(\n"
               "   Handle                : 0x%.08X\n"
               ");\n",
               Handle);

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

    
}

// ******************************************************************
// * patch: D3DDevice_SelectVertexShaderDirect
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SelectVertexShaderDirect)
(
    X_VERTEXATTRIBUTEFORMAT *pVAF,
    DWORD                    Address
)
{
    

    // debug trace
    DbgPrintf( "EmuD3D8: EmuD3DDevice_SelectVertexShaderDirect\n"
               "(\n"
               "   pVAF                : 0x%.08X\n"
               "   Address             : 0x%.08X\n"
               ");\n",
               pVAF,Address);

    DbgPrintf("NOT YET IMPLEMENTED!\n");

    
}

// ******************************************************************
// * patch: D3DDevice_GetShaderConstantMode
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetShaderConstantMode)
(
    DWORD *pMode
)
{
        
        DbgPrintf( "EmuD3D8: EmuD3DDevice_GetShaderConstantMode\n"
                   "(\n"
                   "   pMode               : 0x%.08X\n"
                   ");\n",
                   pMode);
        
    if(pMode)
    {
        *pMode = g_VertexShaderConstantMode;
    }
}

// ******************************************************************
// * patch: D3DDevice_GetVertexShader
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetVertexShader)
(
    DWORD *pHandle
)
{
    

    // debug trace
    DbgPrintf( "EmuD3D8: EmuD3DDevice_GetVertexShader\n"
               "(\n"
               "   pHandle             : 0x%.08X\n"
               ");\n",
               pHandle);

    if(pHandle)
    {
        (*pHandle) = g_CurrentVertexShader;
    }

    
}

// ******************************************************************
// * patch: D3DDevice_GetVertexShaderConstant
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetVertexShaderConstant)
(
    INT   Register,
    void  *pConstantData,
    DWORD ConstantCount
)
{
    

    // debug trace
    DbgPrintf( "EmuD3D8: EmuD3DDevice_GetVertexShaderConstant\n"
               "(\n"
               "   Register            : 0x%.08X\n"
               "   pConstantData       : 0x%.08X\n"
               "   ConstantCount       : 0x%.08X\n"
               ");\n",
               Register, pConstantData, ConstantCount);

    HRESULT hRet = g_pD3DDevice8->GetVertexShaderConstant
    (
        Register + 96,
        pConstantData,
        ConstantCount
    );

    
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderInputDirect
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetVertexShaderInputDirect)
(
    X_VERTEXATTRIBUTEFORMAT *pVAF,
    UINT                     StreamCount,
    X_STREAMINPUT           *pStreamInputs
)
{
    

    // debug trace
    DbgPrintf( "EmuD3D8: EmuD3DDevice_SelectVertexShaderDirect\n"
               "(\n"
               "   pVAF                : 0x%.08X\n"
               "   StreamCount         : 0x%.08X\n"
               "   pStreamInputs       : 0x%.08X\n"
               ");\n",
               pVAF, StreamCount, pStreamInputs);

    DbgPrintf("NOT YET IMPLEMENTED!\n");

    

    return 0;
}

// ******************************************************************
// * patch: D3DDevice_GetVertexShaderInput
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_GetVertexShaderInput)
(
    DWORD              *pHandle,
    UINT               *pStreamCount,
    X_STREAMINPUT      *pStreamInputs
)
{
    

    // debug trace
    DbgPrintf( "EmuD3D8: EmuD3DDevice_GetVertexShaderInput\n"
               "(\n"
               "   pHandle             : 0x%.08X\n"
               "   pStreamCount        : 0x%.08X\n"
               "   pStreamInputs       : 0x%.08X\n"
               ");\n",
               pHandle, pStreamCount, pStreamInputs);

    DbgPrintf("NOT YET IMPLEMENTED!\n");

    

    return 0;
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderInput
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetVertexShaderInput)
(
    DWORD              Handle,
    UINT               StreamCount,
    X_STREAMINPUT     *pStreamInputs
)
{
    

    // debug trace
    DbgPrintf( "EmuD3D8: EmuD3DDevice_SetVertexShaderInput\n"
               "(\n"
               "   Handle              : 0x%.08X\n"
               "   StreamCount         : 0x%.08X\n"
               "   pStreamInputs       : 0x%.08X\n"
               ");\n",
               Handle, StreamCount, pStreamInputs);

    DbgPrintf("NOT YET IMPLEMENTED!\n");

    

    return 0;
}


// ******************************************************************
// * patch: D3DDevice_RunVertexStateShader
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_RunVertexStateShader)
(
    DWORD Address,
    CONST FLOAT *pData
)
{
    

    // debug trace
    DbgPrintf( "EmuD3D8: EmuD3DDevice_RunVertexStateShader\n"
               "(\n"
               "   Address              : 0x%.08X\n"
               "   pData                : 0x%.08X\n"
               ");\n",
               Address,pData);

    DbgPrintf("NOT YET IMPLEMENTED!\n");

    
}

// ******************************************************************
// * patch: D3DDevice_LoadVertexShaderProgram
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_LoadVertexShaderProgram)
(
    CONST DWORD *pFunction,
    DWORD        Address
)
{
    

    // debug trace
    DbgPrintf( "EmuD3D8: EmuD3DDevice_LoadVertexShaderProgram\n"
               "(\n"
               "   pFunction           : 0x%.08X\n"
               "   Address             : 0x%.08X\n"
               ");\n",
               pFunction,Address);

    DbgPrintf("NOT YET IMPLEMENTED!\n");

    
}

// ******************************************************************
// * patch: D3DDevice_GetVertexShaderType
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_GetVertexShaderType)
(
    DWORD  Handle,
    DWORD *pType
)
{
    

    // debug trace
    DbgPrintf( "EmuD3D8: EmuD3DDevice_GetVertexShaderType\n"
               "(\n"
               "   Handle               : 0x%.08X\n"
               "   pType                : 0x%.08X\n"
               ");\n",
               Handle, pType);

    if(pType && VshHandleIsVertexShader(Handle))
    {
        *pType = ((VERTEX_SHADER *)(VshHandleGetVertexShader(Handle))->Handle)->Type;
    }

    
}

// ******************************************************************
// * patch: D3DDevice_GetVertexShaderDeclaration
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_GetVertexShaderDeclaration)
(
    DWORD  Handle,
    PVOID  pData,
    DWORD *pSizeOfData
)
{
    

    // debug trace
    DbgPrintf( "EmuD3D8: EmuD3DDevice_GetVertexShaderDeclaration\n"
               "(\n"
               "   Handle               : 0x%.08X\n"
               "   pData                : 0x%.08X\n"
               "   pSizeOfData          : 0x%.08X\n"
               ");\n",
               Handle, pData, pSizeOfData);

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

    
    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_GetVertexShaderFunction
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_GetVertexShaderFunction)
(
    DWORD  Handle,
    PVOID *pData,
    DWORD *pSizeOfData
)
{
    

    // debug trace
    DbgPrintf( "EmuD3D8: EmuD3DDevice_GetVertexShaderFunction\n"
               "(\n"
               "   Handle               : 0x%.08X\n"
               "   pData                : 0x%.08X\n"
               "   pSizeOfData          : 0x%.08X\n"
               ");\n",
               Handle,pData,pSizeOfData);

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

    
    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_SetDepthClipPlanes
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetDepthClipPlanes)
(
    FLOAT Near,
    FLOAT Far,
    DWORD Flags
)
{
    

    // debug trace
    DbgPrintf( "EmuD3D8: EmuD3DDevice_SetDepthClipPlanes\n"
               "(\n"
               "   Near                : 0x%.08X\n"
               "   Far                 : 0x%.08X\n"
               "   Flags               : 0x%.08X\n"
               ");\n",
               Near, Far, Flags);

    HRESULT hRet = D3D_OK;

    switch(Flags) // Member of X_D3DSET_DEPTH_CLIP_PLANES_FLAGS enum
    {
        case X_D3DSDCP_SET_VERTEXPROGRAM_PLANES:
        {
            // Sets the depth-clipping planes used whenever vertex shader programs are active
            // TODO

            // pDevice->fNear = Near
            // pDevice->fFar  = Far
        }
        break;

        case X_D3DSDCP_SET_FIXEDFUNCTION_PLANES:
        {
            // Sets the depth-clipping planes used whenever the fixed-function pipeline is in use. 
            // TODO

            // pDevice->fNear = Near
            // pDevice->fFar  = Far
        }
        break;

        case X_D3DSDCP_USE_DEFAULT_VERTEXPROGRAM_PLANES:
        {
            // Causes Direct3D to disregard the depth-clipping planes set when using X_D3DSDCP_SET_VERTEXPROGRAM_PLANE. 
            // Direct3D will resume using its own internally calculated clip planes when vertex shader programs are active. 
            // TODO
        }
        break;

        case X_D3DSDCP_USE_DEFAULT_FIXEDFUNCTION_PLANES:
        {
            // Causes Direct3D to disregard the depth-clipping planes set when using X_D3DSDCP_SET_FIXEDFUNCTION_PLANES. 
            // Direct3D will resume using its own internally calculated clip planes when the fixed-function pipeline is active.
            // TODO
        }
        break;

        default:
            EmuWarning("Unknown SetDepthClipPlanes Flags provided");;
    }

    // TODO

    

    return hRet;
}

#if 0 // DISABLED (Just calls MmAllocateContiguousMemory)
// ******************************************************************
// * patch: D3D_AllocContiguousMemory
// ******************************************************************
PVOID WINAPI XTL::EMUPATCH(D3D_AllocContiguousMemory)
(
    SIZE_T dwSize,
    DWORD dwAllocAttributes
)
{
    

    // debug trace
    DbgPrintf( "EmuD3D8: EmuIDirect3D8_AllocContiguousMemory\n"
               "(\n"
               "   dwSize              : 0x%.08X\n"
               "   dwAllocAttributes   : 0x%.08X\n"
               ");\n",
               dwSize,dwAllocAttributes);

    //
    // NOTE: Kludgey (but necessary) solution:
    //
    // Since this memory must be aligned on a page boundary, we must allocate an extra page
    // so that we can return a valid page aligned pointer
    //

    PVOID pRet = CxbxMalloc(dwSize + 0x1000);

    // align to page boundary
    {
        DWORD dwRet = (DWORD)pRet;

        dwRet += 0x1000 - dwRet%0x1000;

        g_AlignCache.insert(dwRet, pRet);

        pRet = (PVOID)dwRet;
    }

    DbgPrintf("EmuD3D8: EmuIDirect3D8_AllocContiguousMemory returned 0x%.08X\n", pRet);

    

    return pRet;
}
#endif

// ******************************************************************
// * patch: IDirect3DTexture8_GetLevelDesc
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DTexture_GetLevelDesc)
(
    UINT Level,
    X_D3DSURFACE_DESC* pDesc
)
{
    

    // debug trace
    DbgPrintf( "EmuD3D8: EmuIDirect3DTexture8_GetLevelDesc\n"
               "(\n"
               "   Level               : 0x%.08X\n"
               "   pDesc               : 0x%.08X\n"
               ");\n",
               Level,pDesc);

    

    return D3D_OK;
}

// ******************************************************************
// * patch: D3D_CheckDeviceMultiSampleType
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3D_CheckDeviceMultiSampleType)
(
    UINT                 Adapter,
    D3DDEVTYPE           DeviceType,
    X_D3DFORMAT          SurfaceFormat,
    BOOL                 Windowed,
    D3DMULTISAMPLE_TYPE  MultiSampleType
)
{
    

    DbgPrintf( "EmuD3D8: EmuIDirect3D8_CheckDeviceMultiSampleType\n"
               "(\n"
               "   Adapter             : 0x%.08X\n"
               "   DeviceType          : 0x%.08X\n"
               "   SurfaceFormat       : 0x%.08X\n"
               "   Windowed            : 0x%.08X\n"
               "   MultiSampleType     : 0x%.08X\n"
               ");\n",
               Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType);

    if(Adapter != D3DADAPTER_DEFAULT)
    {
        EmuWarning("Adapter is not D3DADAPTER_DEFAULT, correcting!");
        Adapter = D3DADAPTER_DEFAULT;
    }

    if(DeviceType == D3DDEVTYPE_FORCE_DWORD)
        EmuWarning("DeviceType == D3DDEVTYPE_FORCE_DWORD");

    // Convert SurfaceFormat (Xbox->PC)
    D3DFORMAT PCSurfaceFormat = EmuXB2PC_D3DFormat(SurfaceFormat);

    // TODO: HACK: Devices that don't support this should somehow emulate it!
    if(PCSurfaceFormat == D3DFMT_D16)
    {
        EmuWarning("D3DFMT_D16 is an unsupported texture format!");
        PCSurfaceFormat = D3DFMT_X8R8G8B8;
    }
    else if(PCSurfaceFormat == D3DFMT_P8)
    {
        EmuWarning("D3DFMT_P8 is an unsupported texture format!");
        PCSurfaceFormat = D3DFMT_X8R8G8B8;
    }
    else if(PCSurfaceFormat == D3DFMT_D24S8)
    {
        EmuWarning("D3DFMT_D24S8 is an unsupported texture format!");
        PCSurfaceFormat = D3DFMT_X8R8G8B8;
    }

    if(Windowed != FALSE)
        Windowed = FALSE;

    // TODO: Convert from Xbox to PC!!
    D3DMULTISAMPLE_TYPE PCMultiSampleType = EmuXB2PC_D3DMultiSampleFormat((DWORD) MultiSampleType);

    // Now call the real CheckDeviceMultiSampleType with the corrected parameters.
    HRESULT hRet = g_pD3D8->CheckDeviceMultiSampleType
    (
        Adapter,
        DeviceType,
        PCSurfaceFormat,
        Windowed,
        PCMultiSampleType
    );

    

    return hRet;
}

// ******************************************************************
// * patch: D3D_GetDeviceCaps
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3D_GetDeviceCaps)
(
    UINT        Adapter,
    D3DDEVTYPE  DeviceType,
    D3DCAPS8    *pCaps
)
{
    

    DbgPrintf("EmuD3D8: EmuIDirect3D8_GetDeviceCaps\n"
           "(\n"
           "   Adapter             : 0x%.08X\n"
           "   DeviceType          : 0x%.08X\n"
           "   pCaps               : 0x%.08X\n"
           ");\n",
           Adapter, DeviceType, pCaps);

    HRESULT hRet = g_pD3D8->GetDeviceCaps(Adapter, DeviceType, pCaps);
	if(FAILED(hRet))
		CxbxKrnlCleanup("IDirect3D8::GetDeviceCaps failed!");

    

    return hRet;
}

// ******************************************************************
// * patch: D3D_SetPushBufferSize
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3D_SetPushBufferSize)
(
    DWORD PushBufferSize,
    DWORD KickOffSize
)
{
    

    DbgPrintf("EmuD3D8: EmuIDirect3D8_SetPushBufferSize\n"
           "(\n"
           "   PushBufferSize      : 0x%.08X\n"
           "   KickOffSize         : 0x%.08X\n"
           ");\n",
           PushBufferSize, KickOffSize);

    HRESULT hRet = D3D_OK;

    // This is a Xbox extension, meaning there is no pc counterpart.

    

    return hRet;
}

// ******************************************************************
// * patch: D3DDevice_InsertFence
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(D3DDevice_InsertFence)()
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_InsertFence()\n");

    // TODO: Actually implement this
    DWORD dwRet = 0x8000BEEF;

    

    return dwRet;
}

// ******************************************************************
// * patch: D3DDevice_IsFencePending
// ******************************************************************
BOOL WINAPI XTL::EMUPATCH(D3DDevice_IsFencePending)
(
    DWORD Fence
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_IsFencePending\n"
           "(\n"
           "   Fence               : 0x%.08X\n"
           ");\n",
           Fence);

    // TODO: Implement

    

    return FALSE;
}

// ******************************************************************
// * patch: D3DDevice_BlockOnFence
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_BlockOnFence)
(
    DWORD Fence
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_BlockOnFence\n"
           "(\n"
           "   Fence               : 0x%.08X\n"
           ");\n",
           Fence);

    // TODO: Implement

    
}

// ******************************************************************
// * patch: IDirect3DResource8_BlockUntilNotBusy
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DResource_BlockUntilNotBusy)
(
    X_D3DResource *pThis
)
{
    // 

    DbgPrintf("EmuD3D8: EmuIDirect3DResource8_BlockUntilNotBusy\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
           ");\n",
           pThis);

    // TODO: Implement

    // 

    return;
}

// ******************************************************************
// * patch: IDirect3DVertexBuffer8_GetDesc
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DVertexBuffer_GetDesc)
(
    X_D3DVertexBuffer    *pThis,
    D3DVERTEXBUFFER_DESC *pDesc
)
{
    

    DbgPrintf("EmuD3D8: EmuIDirect3DVertexBuffer8_GetDesc\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
           "   pDesc               : 0x%.08X\n"
           ");\n",
           pThis, pDesc);

    // TODO: Implement

    
}

// ******************************************************************
// * patch: D3DDevice_SetScissors
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetScissors)
(
    DWORD          Count,
    BOOL           Exclusive,
    CONST D3DRECT  *pRects
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetScissors\n"
           "(\n"
           "   Count               : 0x%.08X\n"
           "   Exclusive           : 0x%.08X\n"
           "   pRects              : 0x%.08X\n"
           ");\n",
           Count, Exclusive, pRects);

    // TODO: Implement

    

    return D3D_OK;
}

// ******************************************************************
// * patch: D3DDevice_SetScreenSpaceOffset
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetScreenSpaceOffset)
(
    FLOAT x,
    FLOAT y
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetScreenSpaceOffset\n"
           "(\n"
           "   x                   : %f\n"
           "   y                   : %f\n"
           ");\n",
           x, y);

    EmuWarning("EmuD3DDevice_SetScreenSpaceOffset ignored");

    

    return D3D_OK;
}

// ******************************************************************
// * patch: D3DDevice_SetPixelShaderProgram
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetPixelShaderProgram)
(
	X_D3DPIXELSHADERDEF* pPSDef
)
{
		// Win2kXP FS

	DbgPrintf("EmuD3D8: EmuD3DDevice_SetPixelShaderProgram\n"
		   "(\n"
		   "    pPSDef             : 0x%.08X\n"
		   ");\n",
		   pPSDef );

	HRESULT hRet = E_FAIL;
	DWORD dwHandle;

	// Redirect this call to windows Direct3D
	hRet = g_pD3DDevice8->CreatePixelShader
    (
        (DWORD*) pPSDef,
        &dwHandle
    );

    if(FAILED(hRet))
    {
        dwHandle = X_PIXELSHADER_FAKE_HANDLE;

        EmuWarning("We're lying about the creation of a pixel shader!");
    }

	// Now, redirect this to Xbox Direct3D 
	//
	//XTL::EMUPATCH(D3DDevice_CreatePixelShader)(pPSDef, &dwHandle);
	//hRet = XTL::EMUPATCH(D3DDevice_SetPixelShader)( dwHandle );
	//

		

	return S_OK;
}

// ******************************************************************
// * patch: IDirect3DDevice_CreateStateBlock
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_CreateStateBlock)
(
	D3DSTATEBLOCKTYPE Type,
	DWORD			  *pToken
)
{
		

	DbgPrintf("EmuD3D8: EmuD3DDevice_CreateStateBlock\n"
           "(\n"
           "   Type                      : 0x%.08X\n"
           "   pToken                    : 0x%.08X\n"
           ");\n",
           Type, pToken);

	// blueshogun96 10/1/07
	// I'm assuming this is the same as the PC version...

	HRESULT hRet = g_pD3DDevice8->CreateStateBlock( Type, pToken );

	if(FAILED(hRet))
		EmuWarning("CreateStateBlock failed!");

		

	return hRet;
}

// ******************************************************************
// * patch: D3DDevice_InsertCallback
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_InsertCallback)
(
	X_D3DCALLBACKTYPE	Type,
	X_D3DCALLBACK		pCallback,
	DWORD				Context
)
{
		

	/*DbgP*/printf("EmuD3D8: EmuD3DDevice_InsertCallback\n"
           "(\n"
		   "   Type                      : 0x%.08X\n"
		   "   pCallback                 : 0x%.08X\n"
		   "   Context                   : 0x%.08X\n"
           ");\n",
           Type, pCallback, Context);

	EmuWarning("InsertCallback ignored!");

	// TODO: Implement
	g_pCallback = (D3DCALLBACK) pCallback;
	g_CallbackType = Type;
	g_CallbackParam = Context;

		
}

// ******************************************************************
// * patch: D3DDevice_DrawRectPatch
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_DrawRectPatch)
(
	UINT					Handle,
	CONST FLOAT				*pNumSegs,
	CONST D3DRECTPATCH_INFO *pRectPatchInfo
)
{
		

	DbgPrintf("EmuD3D8: EmuD3DDevice_DrawRectPatch\n"
           "(\n"
		   "   Handle                    : 0x%.08X\n"
		   "   pNumSegs                  : %f\n"
		   "   pRectPatchInfo            : 0x%.08X\n"
           ");\n",
           Handle, *pNumSegs, pRectPatchInfo);

	HRESULT hRet = g_pD3DDevice8->DrawRectPatch( Handle, pNumSegs, pRectPatchInfo );

	if(FAILED(hRet))
		EmuWarning("DrawRectPatch failed!");

		

	return hRet;
}

#pragma warning(disable:4244)
// ******************************************************************
// * patch: D3DDevice_GetProjectionViewportMatrix
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_GetProjectionViewportMatrix)
(
	D3DXMATRIX *pProjectionViewport
)
{
		

	DbgPrintf("EmuD3D8: EmuD3DDevice_GetProjectionViewportMatrix\n"
           "(\n"
		   "   pProjectionViewport       : 0x%.08X\n"
           ");\n",
           pProjectionViewport);

	// blueshogun96 1/25/10
	// It's been almost 3 years, but I think this is a better 
	// implementation.  Still probably not right, but better
	// then before.

	HRESULT hRet;
	D3DXMATRIX Out, mtxProjection, mtxViewport;
	D3DVIEWPORT8 Viewport;

	// Get current viewport
	hRet = g_pD3DDevice8->GetViewport(&Viewport);
	if(FAILED(hRet))
		EmuWarning("Unable to get viewport!");

	// Get current projection matrix
	hRet = g_pD3DDevice8->GetTransform(D3DTS_PROJECTION, &mtxProjection);
	if(FAILED(hRet))
		EmuWarning("Unable to get projection matrix!");

	// Clear the destination matrix
	::ZeroMemory(&Out, sizeof(D3DMATRIX));

	// Create the Viewport matrix manually
	// Direct3D8 doesn't give me everything I need in a viewport structure
	// (one thing I REALLY HATE!) so some constants will have to be used
	// instead.

	float ClipWidth = 2.0f;
	float ClipHeight = 2.0f;
	float ClipX = -1.0f;
	float ClipY = 1.0f;
	float Width = DWtoF(Viewport.Width);
	float Height = DWtoF(Viewport.Height);

	D3DXMatrixIdentity(&mtxViewport);
	mtxViewport._11 = Width / ClipWidth;
	mtxViewport._22 = -(Height / ClipHeight);
	mtxViewport._41 = -(ClipX * mtxViewport._11);
	mtxViewport._42 = -(ClipY * mtxViewport._22);

	// Multiply projection and viewport matrix together
	Out = mtxProjection * mtxViewport;

	*pProjectionViewport = Out;

//	__asm int 3;

		

	return S_OK;
}
#pragma warning(default:4244)

// ******************************************************************
// * patch: D3DDevice_KickOff (D3D::CDevice::KickOff)
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_KickOff)()
{
	
		

	DbgPrintf("EmuD3D8: EmuD3DDevice_KickOff()\n");

	// TODO: Anything (kick off and NOT wait for idle)?
	// NOTE: We should actually emulate D3DDevice_KickPushBuffer()
	// instead of this function.  When needed, use the breakpoint (int 3)
	// to determine what is calling this function if it's something other
	// than D3DDevice_KickPushBuffer() itself.

//	__asm int 3;

		
}

// ******************************************************************
// * patch: D3DDevice_GetTexture2
// ******************************************************************
XTL::X_D3DResource* WINAPI XTL::EMUPATCH(D3DDevice_GetTexture2)(DWORD Stage)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_GetTexture2\n"
           "(\n"
           "   Stage               : 0x%.08X\n"
           ");\n",
           Stage);
	
	// Get the active texture from this stage
	X_D3DResource* pRet = EmuD3DActiveTexture[Stage];

		

	return pRet;
}

// ******************************************************************
// * patch: D3DDevice_SetStateVB (D3D::CDevice::SetStateVB)
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetStateVB)( ULONG Unknown1 )
{
		

	DbgPrintf("EmuD3D8: EmuD3DDevice_SetStateVB\n"
			"(\n"
			"   Unknown1          : 0x%.08X\n"
			")\n",
			Unknown1);

	// TODO: Anything?
	__asm int 3;

	
}

// ******************************************************************
// * patch: D3DDevice_SetStateUP (D3D::CDevice::SetStateUP)
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(D3DDevice_SetStateUP)()
{
		

	DbgPrintf("EmuD3D8: EmuD3DDevice_SetStateUP()\n");

	// TODO: Anything?
	__asm int 3;

	
}

// ******************************************************************
// * patch: D3DDevice_SetStipple
// ******************************************************************
void WINAPI XTL::EMUPATCH(D3DDevice_SetStipple)( DWORD* pPattern )
{
		

	DbgPrintf("EmuD3D8: EmuD3DDevice_SetStipple\n"
			"(\n"
			"   pPattern          : 0x%.08X\n"
			")\n",
			pPattern);

	// We need an OpenGL port... badly

	
}

// ******************************************************************
// * patch: D3DDevice_SetSwapCallback
// ******************************************************************
void WINAPI XTL::EMUPATCH(D3DDevice_SetSwapCallback)
(
	D3DSWAPCALLBACK		pCallback
)
{
    

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetSwapCallback\n"
           "(\n"
           "   pCallback           : 0x%.08X\n"
           ");\n",
           pCallback);

	DbgPrintf("pCallback: = 0x%.08X\n", pCallback);

    g_pSwapCallback = pCallback;

    

    return;
}

// ******************************************************************
// * patch: D3DDevice_PersistDisplay
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_PersistDisplay)()
{
		

	DbgPrintf("EmuD3D8: EmuD3DDevice_PersistDisplay()\n");

	HRESULT hRet = S_OK;

	// TODO: If this functionality is ever really needed, an idea for 
	// implementation would be to save a copy of the backbuffer's contents
	// and free the memory after the next call to D3DDevice::Present().
	// This temporary data could also be made available to the Xbox game
	// through AvGetSavedDataAddress() since D3DDevice::GetPersistedDisplay2
	// just contains a call to that kernel function.  So far, Unreal Champ-
	// ionship is the only game that uses this functionality that I know of.
	// Other Unreal Engine 2.x games might as well.

	IDirect3DSurface8* pBackSurface = NULL;
	if( SUCCEEDED( g_pD3DDevice8->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackSurface ) ) )
	{
		//D3DXSaveSurfaceToFile( "persisted_surface.bmp", D3DXIFF_BMP, pBackSurface, NULL, NULL );
		pBackSurface->Release();

		DbgPrintf("Persisted display surface saved to persisted_surface.bmp\n");
	}
	else
	{
		EmuWarning("(Temporarily) Not persisting display. Blueshogun can fix this.");
	}

	if(!g_pD3DDevice8)
	{
		EmuWarning("Direct3D device not initialized!");
		hRet =  E_FAIL;
	}
	/*else
	{
		IDirect3DSurface8* pBackBuffer = NULL;
		D3DLOCKED_RECT LockedRect;
		D3DSURFACE_DESC BackBufferDesc;

		g_pD3DDevice8->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
		
		pBackBuffer->GetDesc( &BackBufferDesc );

		DWORD dwBytesPerPixel = ( BackBufferDesc.Format == D3DFMT_X8R8G8B8 || BackBufferDesc.Format == D3DFMT_A8R8G8B8 ) ? 4 : 2;
		FILE* fp = fopen( "PersistedSurface.bin", "wb" );
		if(fp)
		{
			void* ptr = malloc( BackBufferDesc.Width * BackBufferDesc.Height * dwBytesPerPixel );

			if( SUCCEEDED( pBackBuffer->LockRect( &LockedRect, NULL, 0 ) ) )
			{
				CopyMemory( ptr, LockedRect.pBits, BackBufferDesc.Width * BackBufferDesc.Height * dwBytesPerPixel );
				
				fwrite( ptr, BackBufferDesc.Width * BackBufferDesc.Height * dwBytesPerPixel, 1, fp );

				pBackBuffer->UnlockRect();
			}
	
			fclose(fp);
		}
	}*/

		

	return hRet;
}


// ******************************************************************
// * patch: D3DDevice_Unknown1
// ******************************************************************
void WINAPI XTL::EMUPATCH(D3DDevice_Unknown1)()
{
		

	DbgPrintf("EmuD3D8: EmuD3DDevice_Unknown1()\n");

	// TODO: Find out what this actually is.
	// This function was only found in Run Like Hell (5233) @ 0x11FCD0.
	// So far, this function hasn't been found in any other XDKs.  Since
	// the only major thing going on inside of it is a call to the kernel
	// function AvSendTVEncoderOption, we can probably ignore it.

		
}

// ******************************************************************
// * patch: D3DDevice_PrimeVertexCache
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_PrimeVertexCache)
(
	UINT  VertexCount,
	WORD *pIndexData
)
{
	

    DbgPrintf("EmuD3D8: EmuD3DDevice_PrimeVertexCache\n"
           "(\n"
           "   VertexCount           : 0x%.08X\n"
		   "   pIndexData            : 0x%.08X\n"
           ");\n",
           VertexCount, pIndexData);

	// TODO: Implement
	EmuWarning("PrimeVertexCache is not supported!");

		

	return S_OK;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_SampleAlpha
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetRenderState_SampleAlpha)
(
	DWORD dwSampleAlpha
)
{
	

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_SampleAlpha\n"
           "(\n"
           "   dwSampleAlpha         : 0x%.08X\n"
           ");\n",
           dwSampleAlpha);

	// TODO: Implement?

	EmuWarning("SampleAlpha not supported!");

		

	return S_OK;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_Deferred
// ******************************************************************
VOID __fastcall XTL::EMUPATCH(D3DDevice_SetRenderState_Deferred)
(
	DWORD State,
	DWORD Value
)
{
	

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderState_Deferred\n"
           "(\n"
           "   State               : 0x%.08X\n"
           "   Value               : 0x%.08X\n"
           ");\n",
           State, Value);

	// TODO: HACK: Technically, this function doesn't need to be emulated.
	// The location of EmuD3DDeferredRenderState for 3911 isn't correct and at
	// the time of writing, I don't understand how to fix it.  Until then, 
	// I'm going to implement this in a reckless manner.  When the offset for
	// EmuD3DDeferredRenderState is fixed for 3911, this function should be
	// obsolete!

	if( State > 81 && State < 116 )
		EmuD3DDeferredRenderState[State-82] = Value;
	else
		CxbxKrnlCleanup("Unknown Deferred RenderState! (%d)\n", State);

	/*
	XDK 3911 Deferred RenderState values
	D3DRS_FOGENABLE                 = 82,   // TRUE to enable fog blending 
    D3DRS_FOGTABLEMODE              = 83,   // D3DFOGMODE 
    D3DRS_FOGSTART                  = 84,   // float fog start (for both vertex and pixel fog) 
    D3DRS_FOGEND                    = 85,   // float fog end      
    D3DRS_FOGDENSITY                = 86,   // float fog density  
    D3DRS_RANGEFOGENABLE            = 87,   // TRUE to enable range-based fog 
    D3DRS_WRAP0                     = 88,   // D3DWRAP* flags (D3DWRAP_U, D3DWRAPCOORD_0, etc.) for 1st texture coord.
    D3DRS_WRAP1                     = 89,   // D3DWRAP* flags (D3DWRAP_U, D3DWRAPCOORD_0, etc.) for 2nd texture coord. 
    D3DRS_WRAP2                     = 90,   // D3DWRAP* flags (D3DWRAP_U, D3DWRAPCOORD_0, etc.) for 3rd texture coord. 
    D3DRS_WRAP3                     = 91,   // D3DWRAP* flags (D3DWRAP_U, D3DWRAPCOORD_0, etc.) for 4th texture coord. 
    D3DRS_LIGHTING                  = 92,   // TRUE to enable lighting
    D3DRS_SPECULARENABLE            = 93,   // TRUE to enable specular 
    D3DRS_LOCALVIEWER               = 94,   // TRUE to enable camera-relative specular highlights
    D3DRS_COLORVERTEX               = 95,   // TRUE to enable per-vertex color
    D3DRS_BACKSPECULARMATERIALSOURCE= 96,   // D3DMATERIALCOLORSOURCE (Xbox extension)
    D3DRS_BACKDIFFUSEMATERIALSOURCE = 97,   // D3DMATERIALCOLORSOURCE (Xbox extension)
    D3DRS_BACKAMBIENTMATERIALSOURCE = 98,   // D3DMATERIALCOLORSOURCE (Xbox extension)
    D3DRS_BACKEMISSIVEMATERIALSOURCE= 99,   // D3DMATERIALCOLORSOURCE (Xbox extension)
    D3DRS_SPECULARMATERIALSOURCE    = 100,  // D3DMATERIALCOLORSOURCE 
    D3DRS_DIFFUSEMATERIALSOURCE     = 101,  // D3DMATERIALCOLORSOURCE 
    D3DRS_AMBIENTMATERIALSOURCE     = 102,  // D3DMATERIALCOLORSOURCE 
    D3DRS_EMISSIVEMATERIALSOURCE    = 103,  // D3DMATERIALCOLORSOURCE 
    D3DRS_BACKAMBIENT               = 104,  // D3DCOLOR (Xbox extension)
    D3DRS_AMBIENT                   = 105,  // D3DCOLOR 
    D3DRS_POINTSIZE                 = 106,  // float point size 
    D3DRS_POINTSIZE_MIN             = 107,  // float point size min threshold 
    D3DRS_POINTSPRITEENABLE         = 108,  // TRUE to enable point sprites
    D3DRS_POINTSCALEENABLE          = 109,  // TRUE to enable point size scaling
    D3DRS_POINTSCALE_A              = 110,  // float point attenuation A value 
    D3DRS_POINTSCALE_B              = 111,  // float point attenuation B value 
    D3DRS_POINTSCALE_C              = 112,  // float point attenuation C value 
    D3DRS_POINTSIZE_MAX             = 113,  // float point size max threshold 
    D3DRS_PATCHEDGESTYLE            = 114,  // D3DPATCHEDGESTYLE
    D3DRS_PATCHSEGMENTS             = 115,  // DWORD number of segments per edge when drawing patches
	*/

		
}

// ******************************************************************
// * patch: D3DDevice_DeleteStateBlock
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_DeleteStateBlock)
(
	DWORD Token
)
{
		

	DbgPrintf("EmuD3D8: EmuD3DDevice_DeleteStateBlock\n"
		   "(\n"
		   "   Token               : 0x%.08X\n"
		   ");\n",
		   Token);

	HRESULT hRet = g_pD3DDevice8->DeleteStateBlock(Token);

		

	return hRet;
}


// ******************************************************************
// * patch: D3DDevice_SetModelView
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetModelView)
(
	CONST D3DMATRIX *pModelView, 
	CONST D3DMATRIX *pInverseModelView, 
	CONST D3DMATRIX *pComposite
)
{
		

	DbgPrintf("EmuD3D8: EmuD3DDevice_SetModelView\n"
		   "(\n"
		   "   pModelView           : 0x%.08X\n"
		   "   pInverseModelView    : 0x%.08X\n"
		   "   pComposite           : 0x%.08X\n"
		   ");\n",
		   pModelView, pInverseModelView, pComposite);

	// TODO: Implement
//	CxbxKrnlCleanup("SetModelView not yet implemented (should be easy fix, tell blueshogun)");

		

	return S_OK;
}

// ******************************************************************
// * patch: D3DDevice_FlushVertexCache
// ******************************************************************
void WINAPI XTL::EMUPATCH(D3DDevice_FlushVertexCache)()
{
		

	DbgPrintf("EmuD3D8: EmuD3DDevice_FlushVertexCache();\n");

		
}

// ******************************************************************
// * patch: D3DDevice_BeginPushBuffer
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_BeginPushBuffer)
(
	X_D3DPushBuffer *pPushBuffer
)
{
		

	DbgPrintf("EmuD3D8: EmuD3DDevice_BeginPushBuffer\n"
		   "(\n" 
		   "   pPushBuffer          : 0x%.08X\n"
		   ");\n", pPushBuffer);

	// TODO: Implement. Easier said than done with Direct3D, but OpenGL
	// can emulate this functionality rather easily.
//	CxbxKrnlCleanup("BeginPushBuffer is not yet implemented!\n"
	//				"This is going to be a difficult fix for Direct3D but NOT OpenGL!");
	EmuWarning("BeginPushBuffer is not yet implemented!");

		

	return S_OK;
}

// ******************************************************************
// * patch: D3DDevice_EndPushBuffer
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_EndPushBuffer)()
{
		

	DbgPrintf("EmuD3D8: EmuD3DDevice_EndPushBuffer();\n");

		

	return S_OK;
}

// ******************************************************************
// * patch: XMETAL_StartPush
// ******************************************************************
void WINAPI XTL::EMUPATCH(XMETAL_StartPush)(void* Unknown)
{
		

	DbgPrintf("EmuD3D8: EmuXMETAL_StartPush\n"
		   "(\n" 
		   "   Unknown           : 0x%.08X\n"
		   ");\n", Unknown);

	// This function is too low level to actually emulate
	// Only use for debugging.
	__asm int 3;

		
}

// ******************************************************************
// * patch: D3DDevice_GetModelView
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_GetModelView)(D3DXMATRIX* pModelView)
{
		

	DbgPrintf("EmuD3D8: EmuD3DDevice_GetModelView\n"
		   "(\n" 
		   "   pModelView        : 0x%.08X\n"
		   ");\n", pModelView);

	D3DXMATRIX mtxWorld, mtxView;

	// I hope this is right
	g_pD3DDevice8->GetTransform( D3DTS_WORLD, &mtxWorld );
	g_pD3DDevice8->GetTransform( D3DTS_VIEW, &mtxView );

	*pModelView = mtxWorld * mtxView;

		

	return S_OK;
}

// ******************************************************************
// * patch: D3DDevice_SetBackMaterial
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetBackMaterial)(D3DMATERIAL8* pMaterial)
{
		

	DbgPrintf("EmuD3D8: EmuD3DDevice_SetBackMaterial\n"
		   "(\n" 
		   "   pMaterial         : 0x%.08X\n"
		   ");\n", pMaterial);

	EmuWarning("SetBackMaterial is not supported!");

		

	return S_OK;
}

// ******************************************************************
// * patch: D3D_GetAdapterIdentifier
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3D_GetAdapterIdentifier)
(
	UINT					Adapter,
	DWORD					Flags,
	D3DADAPTER_IDENTIFIER8* pIdentifier
)
{
		

	DbgPrintf("EmuD3D8: EmuIDirect3D8_GetAdapterIdentifier\n"
			"(\n"
			"   Adapter          : 0x%.08X\n"
			"   Flags            : 0x%.08X\n"
			"   pIdentifier      : 0x%.08X (0x%.08X)\n"
			");\n", Adapter, Flags, pIdentifier, pIdentifier);

	// TODO: Fill the Intentifier structure with the content of what an Xbox would return.
	// It might not matter for now, but just in case.

	// NOTE: Games do not crash when this function is not intercepted (at least not so far)
	// so it's recommended to add this function to every XDK you possibly can as it will
	// save you much hassle (at least it did for Max Payne).

	HRESULT hRet = g_pD3D8->GetAdapterIdentifier( Adapter, Flags, pIdentifier );
	if(FAILED(hRet))
		EmuWarning("GetAdapterIdentifier failed!");

		

	return hRet;
}

// ******************************************************************
// * patch: D3D_MakeRequestedSpace
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3D_MakeRequestedSpace)( DWORD Unknown1, DWORD Unknown2 )
{
		

	DbgPrintf("EmuD3D8: EmuD3D_MakeRequestedSpace\n"
			"(\n"
			"   Unknown1         : 0x%.08X\n"
			"   Unknown2         : 0x%.08X\n"
			");\n", Unknown1, Unknown2);

	// NOTE: This function is not meant to me emulated.  Just use it to find out
	// the function that is calling it, and emulate that instead!!!  If necessary,
	// create an XRef...

	__asm int 3;
	CxbxKrnlCleanup("D3D::MakeRequestedSpace not implemented (tell blueshogun)");

		

	return S_OK;
}

// ******************************************************************
// * patch: D3DDevice_MakeSpace
// ******************************************************************
void WINAPI XTL::EMUPATCH(D3DDevice_MakeSpace)()
{
		

	DbgPrintf( "EmuD3D8: EmuD3DDevice_MakeSpace();\n");

	// NOTE: Like the above function, this should not be emulated.  The intended
	// usage is the same as above.

	__asm int 3;
	CxbxKrnlCleanup("D3DDevice::MakeSpace not implemented (tell blueshogun)");

		
}

// ******************************************************************
// * patch: D3D_SetCommonDebugRegisters
// ******************************************************************
void WINAPI XTL::EMUPATCH(D3D_SetCommonDebugRegisters)()
{
		

	DbgPrintf( "EmuD3D8: EmuD3D_SetCommonDebugRegisters();\n");

	// NOTE: I added this because I was too lazy to deal with emulating certain render
	// states that use it.  

		
}

// ******************************************************************
// * patch: D3D_BlockOnTime
// ******************************************************************
void WINAPI XTL::EMUPATCH(D3D_BlockOnTime)( DWORD Unknown1, int Unknown2 )
{
		

	DbgPrintf("EmuD3D8: EmuD3D_BlockOnTime\n"
			"(\n"
			"   Unknown1         : 0x%.08X\n"
			"   Unknown2         : 0x%.08X\n"
			");\n", Unknown1, Unknown2);

	// NOTE: This function is not meant to me emulated.  Just use it to find out
	// the function that is calling it, and emulate that instead!!!  If necessary,
	// create an XRef...

//	__asm int 3;
	CxbxKrnlCleanup("D3D::BlockOnTime not implemented (tell blueshogun)");

		
}

// ******************************************************************
// * patch: D3D_BlockOnResource
// ******************************************************************
void WINAPI XTL::EMUPATCH(D3D_BlockOnResource)( X_D3DResource* pResource )
{
		

	DbgPrintf("EmuD3D8: EmuD3D_BlockOnResource\n"
			"(\n"
			"   pResource         : 0x%.08X\n"
			");\n", pResource);

	// TODO: Implement
	// NOTE: Azurik appears to call this directly from numerous points

		
}

// ******************************************************************
// * patch: D3DDevice_GetPushBufferOffset
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_GetPushBufferOffset)
(
	DWORD *pOffset
)
{
		

	DbgPrintf("EmuD3D8: EmuD3DDevice_GetPushBufferOffset\n"
			"(\n"
			"   pOffset            : 0x%.08X\n"
			");\n", pOffset);

	// TODO: Implement
	*pOffset = 0;

		

	return S_OK;
}

// ******************************************************************
// * patch: IDirect3DCubeTexture8_GetCubeMapSurface
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DCubeTexture_GetCubeMapSurface)
(
	X_D3DCubeTexture*	pThis,
	D3DCUBEMAP_FACES	FaceType,
	UINT				Level,
	X_D3DSurface**		ppCubeMapSurface
)
{
		

	DbgPrintf("EmuD3D8: EmuIDirect3DCubeTexture8_GetCubeMapSurface\n"
			"(\n"
			"   pThis              : 0x%.08X\n"
			"   FaceType           : 0x%.08X\n"
			"   Level              : 0x%.08X\n"
			"   ppCubeMapSurface   : 0x%.08X (0x%.08X)\n"
			");\n", pThis, FaceType, Level, ppCubeMapSurface, *ppCubeMapSurface);

	HRESULT hRet;

	// Create a new surface
	*ppCubeMapSurface = EmuNewD3DSurface();

	hRet = pThis->EmuCubeTexture8->GetCubeMapSurface( FaceType, Level, &(*ppCubeMapSurface)->EmuSurface8 );

		

	return hRet;
}

// ******************************************************************
// * patch: IDirect3DCubeTexture8_GetCubeMapSurface2
// ******************************************************************
XTL::X_D3DSurface* WINAPI XTL::EMUPATCH(D3DCubeTexture_GetCubeMapSurface2)
(
	X_D3DCubeTexture*	pThis,
	D3DCUBEMAP_FACES	FaceType,
	UINT				Level
)
{
		

	DbgPrintf("EmuD3D8: EmuIDirect3DCubeTexture8_GetCubeMapSurface2\n"
			"(\n"
			"   pThis              : 0x%.08X\n"
			"   FaceType           : 0x%.08X\n"
			"   Level              : 0x%.08X\n"
			");\n", pThis, FaceType, Level);

	HRESULT hRet;

	// Create a new surface
	X_D3DSurface* pCubeMapSurface = EmuNewD3DSurface();

	hRet = pThis->EmuCubeTexture8->GetCubeMapSurface( FaceType, Level, &pCubeMapSurface->EmuSurface8 );

		

	return pCubeMapSurface;
}

// ******************************************************************
// * patch: D3DDevice_GetPixelShader
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_GetPixelShader)
(
	DWORD  Name,
	DWORD* pHandle
)
{
		

	DbgPrintf("EmuD3D8: EmuD3DDevice_GetPixelShader\n"
			"(\n"
			"   Name               : 0x%.08X\n"
			"   pHandle            : 0x%.08X\n"
			");\n", Name, pHandle);

	// TODO: This implementation is very wrong, but better than nothing.
	*pHandle = g_dwCurrentPixelShader;

		

	return S_OK;
}

// ******************************************************************
// * patch: D3DDevice_GetPersistedSurface
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_GetPersistedSurface)(X_D3DSurface **ppSurface)
{
		

	DbgPrintf("EmuD3D8: EmuD3DDevice_GetPersistedSurface\n"
			"(\n"
			"   ppSurface          : 0x%.08X\n"
			");\n", ppSurface);

	// Attempt to load the persisted surface from persisted_surface.bmp

	*ppSurface = EmuNewD3DSurface();

	HRESULT hr = g_pD3DDevice8->CreateImageSurface( 640, 480, D3DFMT_X8R8G8B8, &(*ppSurface)->EmuSurface8 );
	if( SUCCEEDED( hr ) )
	{
		hr = D3DXLoadSurfaceFromFileA( (*ppSurface)->EmuSurface8, NULL, NULL, "persisted_surface.bmp",
			NULL, D3DX_DEFAULT, 0, NULL );
		if( SUCCEEDED( hr ) )
		{
			DbgPrintf( "Successfully loaded persisted_surface.bmp\n" );
		}
		else
		{
			EmuWarning( "Could not load persisted_surface.bmp!" );
		}
	}
	else
	{
		EmuWarning( "Could not create temporary surface!" );
	}
	
		

	return S_OK;
}

// ******************************************************************
// * patch: D3DDevice_GetPersistedSurface
// ******************************************************************
XTL::X_D3DSurface* WINAPI XTL::EMUPATCH(D3DDevice_GetPersistedSurface2)()
{
		

	DbgPrintf("EmuD3D8: EmuD3DDevice_GetPersistedSurface()\n");

	// Attempt to load the persisted surface from persisted_surface.bmp

	X_D3DSurface* pSurface = EmuNewD3DSurface();

	HRESULT hr = g_pD3DDevice8->CreateImageSurface( 640, 480, D3DFMT_X8R8G8B8, &pSurface->EmuSurface8 );
	if( SUCCEEDED( hr ) )
	{
		hr = D3DXLoadSurfaceFromFileA( pSurface->EmuSurface8, NULL, NULL, "persisted_surface.bmp",
			NULL, D3DX_DEFAULT, 0, NULL );
		if( SUCCEEDED( hr ) )
		{
			DbgPrintf( "Successfully loaded persisted_surface.bmp\n" );
		}
		else
		{
			EmuWarning( "Could not load persisted_surface.bmp!" );
		}
	}
	else
	{
		EmuWarning( "Could not create temporary surface!" );
	}
	
		

	return pSurface;
}

// ******************************************************************
// * patch: D3DDevice_SetRenderTargetFast
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_SetRenderTargetFast)
(
    X_D3DSurface	*pRenderTarget,
    X_D3DSurface	*pNewZStencil,
    DWORD			Flags
)
{
	

    DbgPrintf("EmuD3D8: EmuD3DDevice_SetRenderTarget >>\n"
           "(\n"
           "   pRenderTarget       : 0x%.08X (0x%.08X)\n"
           "   pNewZStencil        : 0x%.08X (0x%.08X)\n"
		   "   Flags               : 0x%.08X\n"
           ");\n",
           pRenderTarget, (pRenderTarget != 0) ? pRenderTarget->EmuSurface8 : 0, pNewZStencil,
           (pNewZStencil != 0) ? pNewZStencil->EmuSurface8 : 0, Flags);

	// Redirect to the standard version.
	
	HRESULT hr = EMUPATCH(D3DDevice_SetRenderTarget)(pRenderTarget, pNewZStencil);
	

	

	return hr;
}

// ******************************************************************
// * patch: D3DDevice_GetScissors
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_GetScissors)
(
	DWORD	*pCount, 
	BOOL	*pExclusive, 
	D3DRECT *pRects
)
{
	

    DbgPrintf("EmuD3D8: EmuD3DDevice_GetScissors\n"
           "(\n"
           "   pCount              : 0x%.08X\n"
           "   pExclusive          : 0x%.08X\n"
           "   pRects              : 0x%.08X\n"
           ");\n",
           pCount, pExclusive, pRects);

    // TODO: Save a copy of each scissor rect in case this function is called
	// in conjunction with D3DDevice::SetScissors. So far, only Outrun2 uses
	// this function. For now, just return the values within the current
	// viewport.

	D3DVIEWPORT8 vp;

	g_pD3DDevice8->GetViewport( &vp );

	pRects->x1 = pRects->y1 = 0;
	pRects->x2 = vp.Width;
	pRects->y2 = vp.Height;

	pExclusive[0] = FALSE;

    

    return D3D_OK;
}

// ******************************************************************
// * patch: D3DDevice_GetBackMaterial
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_GetBackMaterial)(D3DMATERIAL8* pMaterial)
{
		

	DbgPrintf("EmuD3D8: EmuD3DDevice_GetBackMaterial\n"
		   "(\n" 
		   "   pMaterial         : 0x%.08X\n"
		   ");\n", pMaterial);

	EmuWarning("GetBackMaterial is not supported!");

	// TODO: HACK: This is wrong, but better than nothing, right?

	if( pMaterial )
		g_pD3DDevice8->GetMaterial( pMaterial );

		

	return S_OK;
}

// ******************************************************************
// * patch: D3D::LazySetPointParams
// ******************************************************************
void WINAPI XTL::EMUPATCH(D3D_LazySetPointParams)( void* Device )
{
	

    DbgPrintf("EmuD3D8: EmuD3D_LazySetPointParams\n"
           "(\n"
           "   Device              : 0x%.08X\n"
           ");\n",
           Device);

	// Don't emulate this! Just look at the stack trace and go from there!
	__asm int 3;

    
}

// ******************************************************************
// * patch: D3DDevice_GetMaterial
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(D3DDevice_GetMaterial)(D3DMATERIAL8* pMaterial)
{
		

	DbgPrintf("EmuD3D8: EmuD3DDevice_GetMaterial\n"
		   "(\n" 
		   "   pMaterial         : 0x%.08X\n"
		   ");\n", pMaterial);

	if( pMaterial )
		g_pD3DDevice8->GetMaterial( pMaterial );

		

	return S_OK;
}