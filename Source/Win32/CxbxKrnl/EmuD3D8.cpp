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
// * globals
// ******************************************************************
xd3d8::LPDIRECT3D8       g_pD3D8         = NULL;   // Direct3D8
xd3d8::LPDIRECT3DDEVICE8 g_pD3D8Device   = NULL;   // Direct3D8 Device
Xbe::Header             *g_XbeHeader     = NULL;   // XbeHeader
uint32                   g_XbeHeaderSize = 0;      // XbeHeaderSize
HWND                     g_hEmuWindow    = NULL;   // Rendering Window
xd3d8::D3DCAPS8          g_D3DCaps;                // Direct3D8 Caps
HBRUSH                   g_hBgBrush      = NULL;   // Background Brush
volatile bool            g_ThreadInitialized = false;

// ******************************************************************
// * statics
// ******************************************************************
static LRESULT WINAPI EmuMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static DWORD WINAPI   EmuRenderWindow(LPVOID);
static DWORD WINAPI   EmuUpdateTickCount(LPVOID);

// ******************************************************************
// * D3DVertexToPrimitive
// ******************************************************************
UINT xd3d8::D3DVertexToPrimitive[11][2] =
{
    {0, 0},
    {1, 0},
    {2, 0},
    {1, 1},
    {1, 1},
    {3, 0},
    {1, 2},
    {1, 2},
    {4, 0},
    {2, 2},
    {0, 0},
};

// ******************************************************************
// * func: EmuD3DInit
// ******************************************************************
VOID EmuD3DInit(Xbe::Header *XbeHeader, uint32 XbeHeaderSize)
{
    // ******************************************************************
    // * store XbeHeader and XbeHeaderSize for further use
    // ******************************************************************
    {
        g_XbeHeader     = XbeHeader;
        g_XbeHeaderSize = XbeHeaderSize;
    }

    g_ThreadInitialized = false;

    // ******************************************************************
    // * spark up a new thread to handle window message processing
    // ******************************************************************
    {
        DWORD dwThreadId;

        CreateThread(NULL, NULL, EmuUpdateTickCount, NULL, NULL, &dwThreadId);
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

        g_pD3D8->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &g_D3DCaps);
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
    while(true)
    {
        LARGE_INTEGER PerformanceCount;

        QueryPerformanceCounter(&PerformanceCount);

        xboxkrnl::KeTickCount = PerformanceCount.LowPart / 1300;

        Sleep(10);
    }
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

        g_hEmuWindow = CreateWindow
        (
            "CxbxRender", AsciiTitle,
            WS_OVERLAPPEDWINDOW, 100, 100, 640, 480,
            GetDesktopWindow(), NULL, GetModuleHandle(NULL), NULL
        );
    }

    // ******************************************************************
    // * display the window
    // ******************************************************************
    {
        ShowWindow(g_hEmuWindow, SW_SHOWDEFAULT);
        UpdateWindow(g_hEmuWindow);
    }

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

        /*
        case WM_SETCURSOR:
            SetCursor(NULL);
            return 0;
        */
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
        printf("EmuD3D8 (0x%.08X): EmuIDirect3D8_CreateDevice\n"
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
        /*
        printf("BackBufferWidth  : %d\n", pPresentationParameters->BackBufferWidth);
        printf("BackBufferHeight : %d\n", pPresentationParameters->BackBufferHeight);
        printf("BackBufferFormat : %d\n", pPresentationParameters->BackBufferFormat);
        printf("BackBufferCount  : %d\n", pPresentationParameters->BackBufferCount);
        printf("EnableAutoDepthStencil : %d\n", pPresentationParameters->EnableAutoDepthStencil);
        printf("AutoDepthStencilFormat : %d\n", pPresentationParameters->AutoDepthStencilFormat);
        printf("SwapEffect : %d\n", pPresentationParameters->SwapEffect);
        */

        Adapter = D3DADAPTER_DEFAULT;

        pPresentationParameters->Windowed = TRUE;

        // TODO: More intelligently set this only when the game wants it
//        pPresentationParameters->SwapEffect = D3DSWAPEFFECT_COPY_VSYNC;

        hFocusWindow = g_hEmuWindow;

        // TODO: Use lookup table that is dependant on library version
        {
            // Xbox DirectX #defines are different from Win32 DirectX
            if(pPresentationParameters->BackBufferFormat == 0x07)
                pPresentationParameters->BackBufferFormat = D3DFMT_X8R8G8B8;
            else if(pPresentationParameters->BackBufferFormat == 0x06)
                pPresentationParameters->BackBufferFormat = D3DFMT_A8R8G8B8;

            // Xbox DirectX #defines are different from Win32 DirectX
            if(pPresentationParameters->AutoDepthStencilFormat == 0x2A)
                pPresentationParameters->AutoDepthStencilFormat = D3DFMT_D24S8;
        }
    }

    // ******************************************************************
    // * TODO: Query for Software Vertex Processing abilities!!
    // ******************************************************************
    BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
//    BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;

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

    // TODO: HACK: This needs to be auto-updated from Xbox D3D global variable(s)
    if(g_pD3D8Device != 0)
        g_pD3D8Device->SetRenderState( D3DRS_LIGHTING, FALSE );

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
        printf("EmuD3D8 (0x%.08X): EmuIDirect3DDevice8_Clear\n"
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
        // TODO: Use lookup table that is dependant on library version
        // Tricky MS randomizing .h #defines :[
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
        printf("EmuD3D8 (0x%.08X): EmuIDirect3DDevice8_Present\n"
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
        printf("EmuD3D8 (0x%.08X): EmuIDirect3DDevice8_Swap\n"
               "(\n"
               "   Flags               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Flags);
    }
    #endif

    // TODO: Ensure this flag is always the same across library versions
    if(Flags != 0)
        EmuPanic();

    // Swap(0) is equivalent to present(0,0,0,0)
    HRESULT ret = g_pD3D8Device->Present(0, 0, 0, 0);

    EmuSwapFS();   // XBox FS

    return ret;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateVertexBuffer
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
        printf("EmuD3D8 (0x%.08X): EmuIDirect3DDevice8_CreateVertexBuffer2\n"
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
        printf("EmuD3D8 (0x%.08X): EmuIDirect3DDevice8_SetRenderState_CullMode\n"
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
    }

    g_pD3D8Device->SetRenderState(D3DRS_CULLMODE, Value);

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
        printf("EmuD3D8 (0x%.08X): EmuIDirect3DDevice8_SetTransform\n"
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
BYTE* WINAPI xd3d8::EmuIDirect3DVertexBuffer8_Lock
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
        printf("EmuD3D8 (0x%.08X): EmuIDirect3DVertexBuffer8_Lock\n"
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
void WINAPI xd3d8::EmuIDirect3DDevice8_SetStreamSource
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
        printf("EmuD3D8 (0x%.08X): EmuIDirect3DDevice8_SetStreamSource\n"
               "(\n"
               "   StreamNumber        : 0x%.08X\n"
               "   pStreamData         : 0x%.08X\n"
               "   Stride              : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), StreamNumber, pStreamData, Stride);
    }
    #endif

    ((IDirect3DVertexBuffer8*)pStreamData)->Unlock();

	HRESULT ret = g_pD3D8Device->SetStreamSource(StreamNumber, (IDirect3DVertexBuffer8*)pStreamData, Stride);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexShader
// ******************************************************************
void WINAPI xd3d8::EmuIDirect3DDevice8_SetVertexShader
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
        printf("EmuD3D8 (0x%.08X): EmuIDirect3DDevice8_SetVertexShader\n"
               "(\n"
               "   Handle              : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Handle);
    }
    #endif

	HRESULT ret = g_pD3D8Device->SetVertexShader(Handle);

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuIDirect3DDevice8_DrawVertices
// ******************************************************************
void WINAPI xd3d8::EmuIDirect3DDevice8_DrawVertices
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
        printf("EmuD3D8 (0x%.08X): EmuIDirect3DDevice8_DrawVertices\n"
               "(\n"
               "   PrimitiveType       : 0x%.08X\n"
               "   StartVertex         : 0x%.08X\n"
               "   VertexCount         : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), PrimitiveType, StartVertex, VertexCount);
    }
    #endif

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

    return;
}
