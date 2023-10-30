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
#define LOG_PREFIX CXBXR_MODULE::D3D8

#include "common\util\hasher.h" // For ComputeHash
#include <condition_variable>
#include <stack>


#include <core\kernel\exports\xboxkrnl.h>
#include "common\util\CxbxUtil.h"
#include "CxbxVersion.h"
#include "core\kernel\init\CxbxKrnl.h"
#include "core\kernel\support\Emu.h"
#include "core\kernel\support\EmuFS.h"
#include "core\kernel\support\NativeHandle.h"
#include "EmuShared.h"
#include "..\FixedFunctionState.h"
#include "core\hle\D3D8\ResourceTracker.h"
#include "core\hle\D3D8\Direct3D9\Direct3D9.h" // For LPDIRECTDRAWSURFACE7
#include "core\hle\D3D8\XbVertexBuffer.h"
#include "core\hle\D3D8\XbVertexShader.h"
#include "core\hle\D3D8\XbPixelShader.h" // For DxbxUpdateActivePixelShader
#include "core\hle\D3D8\XbPushBuffer.h"
#include "core\kernel\memory-manager\VMManager.h" // for g_VMManager
#include "core\hle\XAPI\Xapi.h" // For EMUPATCH
#include "core\hle\D3D8\XbConvert.h"
#include "Logging.h"
#include "..\XbD3D8Logging.h"
#include "core\hle\Intercept.hpp" // for bLLE_GPU
#include "devices\video\nv2a.h" // For GET_MASK, NV_PGRAPH_CONTROL_0, PUSH_METHOD
#include "gui/resource/ResCxbx.h"
#include "RenderStates.h"
#include "TextureStates.h"
#include "WalkIndexBuffer.h"
#include "core\kernel\common\strings.hpp" // For uem_str
#include "common\input\SdlJoystick.h"
#include "common\input\DInputKeyboardMouse.h"
#include "common\input\InputManager.h"
#include "common/util/strConverter.hpp" // for utf8_to_utf16
#include "VertexShaderSource.h"
#include "Timer.h"

#include <imgui.h>
#include <backends/imgui_impl_dx9.h>
#include <backends/imgui_impl_win32.h>
#include "core/common/video/RenderBase.hpp"

#include <assert.h>
#include <process.h>
#include <clocale>
#include <functional>
#include <unordered_map>
#include <thread>
#include <vector>

#include <wrl/client.h>

#include "nv2a_vsh_emulator.h"
#include "HleInNv2a.h"
//#include "dxgi.h"
#include "D3dx9tex.h""
using namespace Microsoft::WRL;

XboxRenderStateConverter XboxRenderStates;
XboxTextureStateConverter XboxTextureStates;

extern XboxRenderStateConverter NV2ARenderStates;
extern XboxTextureStateConverter NV2ATextureStates;


D3D8LightState d3d8LightState = D3D8LightState();
D3D8TransformState d3d8TransformState = D3D8TransformState();
FixedFunctionVertexShaderState ffShaderState = {0}; // TODO find a home for this and associated code

// Allow use of time duration literals (making 16ms, etc possible)
using namespace std::literals::chrono_literals;

// Global(s)
HWND                                g_hEmuWindow   = NULL; // rendering window
bool                                g_bClipCursor  = false; // indicates that the mouse cursor should be confined inside the rendering window
IDirect3DDevice9Ex                 *g_pD3DDevice   = nullptr; // Direct3D Device

// Static Variable(s)
static bool                         g_bSupportsFormatSurface[xbox::X_D3DFMT_LIN_R8G8B8A8 + 1]; // Does device support surface format?
static bool                         g_bSupportsFormatSurfaceRenderTarget[xbox::X_D3DFMT_LIN_R8G8B8A8 + 1]; // Does device support surface format?
static bool                         g_bSupportsFormatSurfaceDepthStencil[xbox::X_D3DFMT_LIN_R8G8B8A8 + 1]; // Does device support surface format?
static bool                         g_bSupportsFormatTexture[xbox::X_D3DFMT_LIN_R8G8B8A8 + 1]; // Does device support texture format?
static bool                         g_bSupportsFormatTextureRenderTarget[xbox::X_D3DFMT_LIN_R8G8B8A8 + 1]; // Does device support texture format?
static bool                         g_bSupportsFormatTextureDepthStencil[xbox::X_D3DFMT_LIN_R8G8B8A8 + 1]; // Does device support texture format?
static bool                         g_bSupportsFormatVolumeTexture[xbox::X_D3DFMT_LIN_R8G8B8A8 + 1]; // Does device support surface format?
static bool                         g_bSupportsFormatCubeTexture[xbox::X_D3DFMT_LIN_R8G8B8A8 + 1]; // Does device support surface format?
static HBRUSH                       g_hBgBrush = NULL; // Background Brush
static bool                         g_bIsFauxFullscreen = false;
static DWORD						g_OverlaySwap = 0; // Set in D3DDevice_UpdateOverlay
static int                          g_iWireframe = 0; // wireframe toggle
static bool                         g_bHack_UnlockFramerate = false; // ignore the xbox presentation interval
static bool                         g_bHasDepth = false;    // Does device have a Depth Buffer?
       float                        g_ZScale = 1.0;
	   float                        g_WScale = 1.0;
	   float                        g_SuperSampleScaleX = 1.0;
       float                        g_WNear = 1.0;
	   float                        g_WFar = 1.0;
	   float                        g_WInverseWFar = 1.0;
	   float                        g_SuperSampleScaleY = 1.0;
	   float                        g_ScreenSpaceOffsetX = 1.0;
	   float                        g_ScreenSpaceOffsetY = 1.0;
	   xbox::X_D3DVIEWPORT8         g_NV2A_Viewport;
static bool                         g_bHasStencil = false;  // Does device have a Stencil Buffer?
static DWORD						g_dwPrimPerFrame = 0;	// Number of primitives within one frame
       bool                         g_bUsePassthroughHLSL = true;
static float                        g_AspectRatioScale = 1.0f;
static UINT                         g_AspectRatioScaleWidth = 0;
static UINT                         g_AspectRatioScaleHeight = 0;
static D3DSURFACE_DESC              g_HostBackBufferDesc;
static Settings::s_video            g_XBVideo;

// D3D based variables
static IDirect3D9Ex                *g_pDirect3D = nullptr;
       D3DCAPS						g_D3DCaps = {};         // Direct3D Caps
static IDirect3DIndexBuffer        *g_pClosingLineLoopHostIndexBuffer = nullptr;
static IDirect3DIndexBuffer        *g_pQuadToTriangleHostIndexBuffer = nullptr;

static bool                         g_bEnableHostQueryVisibilityTest = true;
static std::stack<IDirect3DQuery*>  g_HostQueryVisibilityTests;
static std::map<int, IDirect3DQuery*> g_HostVisibilityTestMap;

// cached Direct3D state variable(s)
static size_t                       g_QuadToTriangleHostIndexBuffer_Size = 0; // = NrOfQuadIndices
static INDEX16                     *g_pQuadListToTriangleIndexData = nullptr;
static size_t                       g_QuadListToTriangleIndexData_Size = 0; // = NrOfQuadIndices
static INDEX16                     *g_pQuadStripToTriangleIndexData = nullptr;
static size_t                       g_QuadStripToTriangleIndexData_Size = 0; // = NrOfQuadIndices

struct {
	xbox::X_D3DSurface Surface;
	RECT SrcRect;
	RECT DstRect;
	BOOL EnableColorKey;
	D3DCOLOR ColorKey;
} g_OverlayProxy;

typedef struct {
	// Arguments to D3DDevice_InsertCallback :
	xbox::X_D3DCALLBACK			    pCallback;
	xbox::X_D3DCALLBACKTYPE          Type;
	xbox::dword_xt                      Context;
} s_Xbox_Callback;

static std::queue<s_Xbox_Callback>  g_Xbox_CallbackQueue;
static bool                         g_bHack_DisableHostGPUQueries = false; // TODO : Make configurable
static IDirect3DQuery              *g_pHostQueryWaitForIdle = nullptr;
static IDirect3DQuery              *g_pHostQueryCallbackEvent = nullptr;
static int                          g_RenderUpscaleFactor = 1;

static std::condition_variable		g_VBConditionVariable;	// Used in BlockUntilVerticalBlank
static std::mutex					g_VBConditionMutex;		// Used in BlockUntilVerticalBlank
static DWORD                        g_VBLastSwap = 0;

static xbox::dword_xt                   g_Xbox_PresentationInterval_Default = D3DPRESENT_INTERVAL_IMMEDIATE;
       xbox::dword_xt                   g_Xbox_PresentationInterval_Override = 0;
static xbox::X_D3DSWAPDATA			g_Xbox_SwapData = {0}; // current swap information
static xbox::X_D3DSWAPCALLBACK		g_pXbox_SwapCallback = xbox::zeroptr;	// Swap/Present callback routine
static xbox::X_D3DVBLANKDATA			g_Xbox_VBlankData = {0}; // current vertical blank information
static xbox::X_D3DVBLANKCALLBACK     g_pXbox_VerticalBlankCallback   = xbox::zeroptr; // Vertical-Blank callback routine

       xbox::X_D3DSurface           *g_pXbox_BackBufferSurface = xbox::zeroptr;
	   xbox::X_D3DSurface            g_Xbox_BackBufferSurface = {0};

	   bool                          g_bXbox_BackBufferSurfaceLocked = false;
static xbox::X_D3DSurface           *g_pXbox_DefaultDepthStencilSurface = xbox::zeroptr;
static xbox::X_D3DSurface            g_Xbox_DefaultDepthStencilSurface = {0};
       xbox::X_D3DSurface           *g_pXbox_RenderTarget = xbox::zeroptr;
	   xbox::X_D3DSurface            g_Xbox_RenderTarget = { 0 };
static xbox::X_D3DSurface           *g_pXbox_DepthStencil = xbox::zeroptr;
static xbox::X_D3DSurface            g_Xbox_DepthStencil = { 0 };
       xbox::X_D3DMULTISAMPLE_TYPE   g_Xbox_MultiSampleType = xbox::X_D3DMULTISAMPLE_NONE;

       xbox::X_VERTEXSHADERCONSTANTMODE g_Xbox_VertexShaderConstantMode = X_D3DSCM_192CONSTANTS; // Set by D3DDevice_SetShaderConstantMode, TODO : Move to XbVertexShader.cpp
static xbox::dword_xt                   g_Xbox_BaseVertexIndex = 0; // Set by D3DDevice_SetIndices, read by D3DDevice_DrawIndexedVertices : a value that's effectively added to every vertex index (as stored in an index buffer) by multiplying this by vertex stride and added to the vertex buffer start (see BaseVertexIndex in CxbxDrawIndexed)
static xbox::dword_xt                  *g_pXbox_BeginPush_Buffer = xbox::zeroptr; // primary push buffer
bool is_pushbuffer_recording(void)
{
	//return (NV2A_stateFlags & X_STATE_RECORDPUSHBUFFER) != 0;
	return g_pXbox_BeginPush_Buffer == nullptr ? false : true;
}
       xbox::X_PixelShader*			g_pXbox_PixelShader = xbox::zeroptr;
	   xbox::X_PixelShader          g_Xbox_PixelShader = {0};
static xbox::PVOID                   g_pXbox_Palette_Data[xbox::X_D3DTS_STAGECOUNT] = { xbox::zeroptr, xbox::zeroptr, xbox::zeroptr, xbox::zeroptr }; // cached palette pointer
static unsigned                     g_Xbox_Palette_Size[xbox::X_D3DTS_STAGECOUNT] = { 0 }; // cached palette size
// NV2A Kelvin alternatives
xbox::X_D3DPalette           g_NV2A_Palette_Data[xbox::X_D3DTS_STAGECOUNT] = { 0 };
xbox::PVOID                  g_pNV2A_Palette_Data[xbox::X_D3DTS_STAGECOUNT] = { xbox::zeroptr, xbox::zeroptr, xbox::zeroptr, xbox::zeroptr }; // cached palette pointer
unsigned                     g_NV2A_Palette_Size[xbox::X_D3DTS_STAGECOUNT] = { 0 }; // cached palette size


       xbox::X_D3DBaseTexture       *g_pXbox_SetTexture[xbox::X_D3DTS_STAGECOUNT] = {0,0,0,0}; // Set by our D3DDevice_SetTexture and D3DDevice_SwitchTexture patches
	   xbox::X_D3DSurface            g_Xbox_SetTexture[xbox::X_D3DTS_STAGECOUNT] = {0};
	   std::map<UINT64, xbox::X_D3DSurface> g_TextureCache;// cache all pTexture passed to SetTexture() and SwitchTexture()
	   std::map<UINT64, xbox::X_D3DSurface> g_RenderTargetCache;// cache all pRenderTarget passed to SetRenderTarget() 
struct DepthBufferResource
{
	xbox::X_D3DSurface XboxDepthBufferSurface;
	//IDirect3DSurface* pHostDepthStencil;
	IDirect3DSurface* pHostDepthStencilTextureSurface;
	IDirect3DTexture* pHostDepthStencilTexture;
};
       std::map<UINT64, DepthBufferResource> g_DepthBufferCache;  // cache all pDepthBuffer passed to SetRenderTarget(), this is for texture stage input to look up possible depth stencil texture.
	   std::map<UINT32, xbox::X_D3DSurface>  g_BufferSurfaceCache;// cache all xbox surfaces possible for being use as render target or depth stencil, for D3DDevice_SetTile() to look up xbox surface with pMemory. using Surface.Data as key.
static xbox::X_D3DSurface        CxbxActiveTextureCopies[xbox::X_D3DTS_STAGECOUNT] = {}; // Set by D3DDevice_SwitchTexture. Cached active texture


xbox::X_D3DVIEWPORT8 g_Xbox_Viewport = { 0 };
float g_Xbox_BackbufferScaleX = 1;
float g_Xbox_BackbufferScaleY = 1;
xbox::X_D3DSWAP g_LastD3DSwap = (xbox::X_D3DSWAP) -1;


static constexpr size_t INDEX_BUFFER_CACHE_SIZE = 10000;

static void CxbxImGui_RenderD3D9(ImGuiUI* m_imgui, IDirect3DSurface9* renderTarget)
{
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	m_imgui->DrawMenu();
	m_imgui->DrawWidgets();

	ImGui::EndFrame();

	ImGui::Render();
	ImDrawData* drawData = ImGui::GetDrawData();
	if (drawData->TotalVtxCount > 0) {
		IDirect3DSurface9* pExistingRenderTarget = nullptr;
		if (SUCCEEDED(g_pD3DDevice->GetRenderTarget(0, &pExistingRenderTarget))) {
			g_pD3DDevice->SetRenderTarget(0, renderTarget);
			ImGui_ImplDX9_RenderDrawData(drawData);
			g_pD3DDevice->SetRenderTarget(0, pExistingRenderTarget);
			pExistingRenderTarget->Release();
		}
	}
}



static xbox::dword_xt                  *g_pXbox_D3DDevice=nullptr; // TODO: This is a pointer to a D3DDevice structure,
static xbox::dword_xt                 **g_pXbox_pPush = nullptr; // TODO: This is a pointer to current pushbuffer,
static xbox::dword_xt                 **g_pXbox_pPushLimit = nullptr; // TODO: This is a pointer to the end of current pushbuffer,

// Static Function(s)
static DWORD WINAPI                 EmuRenderWindow(LPVOID);
static LRESULT WINAPI               EmuMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static xbox::void_xt NTAPI          EmuUpdateTickCount(xbox::PVOID Arg);
static inline void                  EmuVerifyResourceIsRegistered(xbox::X_D3DResource *pResource, DWORD D3DUsage, int iTextureStage, DWORD dwSize);
static void							UpdateCurrentMSpFAndFPS(); // Used for benchmarking/fps count
void							CxbxrImpl_SetRenderTarget(xbox::X_D3DSurface *pRenderTarget, xbox::X_D3DSurface *pNewZStencil);

#define CXBX_D3DCOMMON_IDENTIFYING_MASK (X_D3DCOMMON_TYPE_MASK)// | X_D3DCOMMON_D3DCREATED)


// Those should be used with LTCG patches which use __declspec(naked)
#define LTCG_PROLOGUE \
		__asm push ebp \
		__asm mov  ebp, esp \
		__asm sub  esp, __LOCAL_SIZE \
		__asm push esi \
		__asm push edi \
		__asm push ebx

#define LTCG_EPILOGUE \
		__asm pop  ebx \
		__asm pop  edi \
		__asm pop  esi \
		__asm mov  esp, ebp \
		__asm pop  ebp


typedef struct resource_key_hash {
	// All Xbox X_D3DResource structs have these fields :
	DWORD Common; // We set this to the CXBX_D3DCOMMON_IDENTIFYING_MASK bits of the source Common field
	DWORD Data; // We set this as-is to a copy of the source Data field
	// DWORD Lock; // We leave away the source Lock field, since it's entirely volatile (not deterministic)
	union {
		struct {
			// For non-pixel-containers, we set the Xbox resource address for now (TODO : Come up with something better) :
			xbox::addr_xt ResourceAddr; // We set this as-is
		};
		struct {
			// For xbox::X_D3DPixelContainer's we also set these fields :
			DWORD Format; // We set this as-is
			DWORD Size; // We set this as-is
			// For X_D3DFMT_P8 paletized pixel-containers, we also set this field :
			uint64_t PaletteHash;
		};
	};

	// These operator overloads are required to use resource_key_t in resource_cache_t :
	bool operator==(const struct resource_key_hash& other) const
	{
		return (Common == other.Common)
			&& (Data == other.Data)
			&& (Format == other.Format)
			&& (Size == other.Size)
			&& (PaletteHash == other.PaletteHash);
		// Note : ResourceAddr doesn't need comparison, since it's union'ed with Format,Size,PaletteHash already
	}

	// See https://marknelson.us/posts/2011/09/03/hash-functions-for-c-unordered-containers.html
	size_t operator()(const struct resource_key_hash& value) const
	{
		return (size_t)ComputeHash((void*)&value, sizeof(value));
	}
} resource_key_t;

// information passed to the create device proxy thread
struct EmuD3D8CreateDeviceProxyData
{
	// Set by EmuD3DInit()
    xbox::uint_xt                        Adapter;
    D3DDEVTYPE                       DeviceType;
	// Set by EmuCreateDeviceProxy()
	D3DPRESENT_PARAMETERS            HostPresentationParameters;
}
g_EmuCDPD;

// Declare trampolines
#define XB_TRAMPOLINES(XB_MACRO)                                                                                                                                                                            \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_CreateVertexShader,                       (CONST xbox::dword_xt*, CONST xbox::dword_xt*, xbox::dword_xt*, xbox::dword_xt)                       );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_DeleteVertexShader,                       (xbox::dword_xt)                                                                                      );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_DeleteVertexShader_0,                     ()                                                                                                    );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_ApplyStateBlock,                          (xbox::dword_xt)                                                                                      );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_Begin,                                    (xbox::dword_xt)                                                                                      );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_BeginPush_4,                              (xbox::dword_xt)                                                                                      );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_BeginPush_8,                              (xbox::dword_xt, xbox::dword_xt**)                                                                    );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_BeginPushBuffer,                          (xbox::dword_xt*)                                                                                     );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_CopyRects,                                (xbox::X_D3DSurface*,CONST RECT*, xbox::uint_xt, xbox::X_D3DSurface*,CONST POINT*)                    );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_Clear,                                    (xbox::dword_xt,CONST D3DRECT *, xbox::dword_xt, D3DCOLOR,float, xbox::dword_xt)                      );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_DrawIndexedVertices,                      (xbox::X_D3DPRIMITIVETYPE, xbox::uint_xt, CONST PWORD)                                                );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_DrawIndexedVerticesUP,                    (xbox::X_D3DPRIMITIVETYPE, xbox::uint_xt, CONST PVOID, CONST PVOID, xbox::uint_xt)                    );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_DrawVertices,                             (xbox::X_D3DPRIMITIVETYPE, xbox::uint_xt, xbox::uint_xt)                                              );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_DrawVertices_4__LTCG_ecx2_eax3,           (xbox::X_D3DPRIMITIVETYPE)                                                                            );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_DrawVertices_8__LTCG_eax3,                (xbox::X_D3DPRIMITIVETYPE, xbox::uint_xt)                                                                            );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_DrawVerticesUP,                           (xbox::X_D3DPRIMITIVETYPE, xbox::uint_xt, CONST PVOID, xbox::uint_xt)                                 );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_DrawVerticesUP_12__LTCG_ebx3,             (xbox::X_D3DPRIMITIVETYPE, xbox::uint_xt, xbox::uint_xt)                                              );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_End,                                      ()                                                                                                    );  \
    XB_MACRO(xbox::dword_xt*,     WINAPI,     D3DDevice_EndPush,                                  (xbox::dword_xt*)                                                                                     );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_EndPushBuffer,                            ()                                                                                                    );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_EndVisibilityTest,                        (xbox::dword_xt)                                                                                      );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_EndVisibilityTest_0,                      ()                                                                                                    );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_GetBackBuffer,                            (xbox::int_xt, D3DBACKBUFFER_TYPE, xbox::X_D3DSurface**)                                              );  \
    XB_MACRO(xbox::X_D3DSurface*, WINAPI,     D3DDevice_GetBackBuffer2,                           (xbox::int_xt)                                                                                        );  \
    XB_MACRO(xbox::X_D3DSurface*, WINAPI,     D3DDevice_GetBackBuffer2_0__LTCG_eax1,              ()                                                                                                    );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_GetBackBuffer_8,                          (D3DBACKBUFFER_TYPE, xbox::X_D3DSurface**)                                                            );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_GetDepthStencilSurface,                   (xbox::X_D3DSurface**)                                                                                );  \
    XB_MACRO(xbox::X_D3DSurface*, WINAPI,     D3DDevice_GetDepthStencilSurface2,                  (xbox::void_xt)                                                                                       );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_GetDisplayMode,                           (xbox::X_D3DDISPLAYMODE*)                                                                             );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_GetProjectionViewportMatrix,              (CONST D3DMATRIX*)                                                                                    );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_GetPersistedSurface,                      (xbox::X_D3DSurface**)                                                                                );  \
    XB_MACRO(xbox::X_D3DSurface*, WINAPI,     D3DDevice_GetPersistedSurface2,                     (xbox::void_xt)                                                                                       );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_GetRenderTarget,                          (xbox::X_D3DSurface**)                                                                                );  \
    XB_MACRO(xbox::X_D3DSurface*, WINAPI,     D3DDevice_GetRenderTarget2,                         (xbox::void_xt)                                                                                       );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_GetTile,                                  (xbox::dword_xt, CONST xbox::X_D3DTILE*)                                                              );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_GetTransform,                             (xbox::X_D3DTRANSFORMSTATETYPE, CONST D3DMATRIX*)                                                     );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     CDevice_KickOff,                                    ()                                                                                                    );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     CDevice_KickOff_4,                                  (DWORD)                                                                                               );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     CDevice_KickOff_0__LTCG_eax1,                       ()                                                                                                    );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     CDevice_KickOff_0__LTCG_edx1,                       ()                                                                                                    );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_LightEnable,                              (xbox::dword_xt, xbox::bool_xt)                                                                       );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_LoadVertexShader,                         (xbox::dword_xt, xbox::dword_xt)                                                                      );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_LoadVertexShaderProgram,                  (CONST xbox::dword_xt*, xbox::dword_xt)                                                               );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_LoadVertexShader_4,                       (xbox::dword_xt)                                                                                      );  \
    XB_MACRO(xbox::void_xt,       WINAPI, D3DDevice_LoadVertexShader_0__LTCG_eax_Address_ecx_Handle,()                                                                                                  );  \
    XB_MACRO(xbox::void_xt,       WINAPI, D3DDevice_LoadVertexShader_0__LTCG_eax_Address_edx_Handle,()                                                                                                  );  \
    XB_MACRO(xbox::dword_xt*,     WINAPI,     CDevice_MakeSpace,                                  ()                                                                                                    );  \
    XB_MACRO(xbox::dword_xt*,     WINAPI,     D3DDevice_MakeSpace,                                ()                                                                                                    );  \
    XB_MACRO(xbox::dword_xt*,     WINAPI,     D3DDevice_MakeSpace_0,                              ()                                                                                                    );  \
    XB_MACRO(xbox::dword_xt*,     WINAPI,     D3D_MakeRequestedSpace_4__LTCG_eax_RequestedSpace,  (xbox::void_xt)                                                                                                    );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_MultiplyTransform,                        (xbox::X_D3DTRANSFORMSTATETYPE, CONST D3DMATRIX*)                                                     );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_PersistDisplay,                           (xbox::void_xt)                                                                                       );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_Reset,                                    (xbox::X_D3DPRESENT_PARAMETERS*)                                                                      );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_Reset_0__LTCG_edi1,                       ()                                                                                                    );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_Reset_0__LTCG_ebx1,                       ()                                                                                                    );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_RunPushBuffer,                            (xbox::X_D3DPushBuffer*, xbox::X_D3DFixup*)                                                           );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetBackBufferScale,                       (xbox::float_xt, xbox::float_xt)                                                                      );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SelectVertexShader,                       (xbox::dword_xt, xbox::dword_xt)                                                                      );  \
    XB_MACRO(xbox::void_xt,       __stdcall,  D3DDevice_SelectVertexShader_0__LTCG_eax1_ebx2,     ()                                                                                                    );  \
    XB_MACRO(xbox::void_xt,       __stdcall,  D3DDevice_SelectVertexShader_4__LTCG_eax1,          (xbox::dword_xt)                                                                                      );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetFlickerFilter_0,                       ()                                                                                                    );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetFlickerFilter,                         (xbox::dword_xt)                                                                                      );  \
  /*XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetGammaRamp,                             (xbox::dword_xt, CONST X_D3DGAMMARAMP*)                                                               );*/\
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetIndices,                               (xbox::X_D3DIndexBuffer*, xbox::uint_xt)                                                              );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetIndices_4,                             (xbox::uint_xt)                                                                                       );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_SetLight,                                 (xbox::dword_xt, CONST xbox::X_D3DLIGHT8*)                                                            );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetModelView,                             (CONST D3DMATRIX*, CONST D3DMATRIX*, CONST D3DMATRIX*)                                                );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_SetMaterial,                              (CONST xbox::X_D3DMATERIAL8* pMaterial)                                                               );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetPalette,                               (xbox::dword_xt, xbox::X_D3DPalette*)                                                                 );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetPalette_4,                             (xbox::X_D3DPalette*)                                                                                 );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetPixelShader,                           (xbox::dword_xt)                                                                                      );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetPixelShader_0__LTCG_eax_handle,        ()                                                                                                    );  \
    XB_MACRO(xbox::void_xt,       __fastcall, D3DDevice_SetRenderState_Simple,                    (xbox::dword_xt, xbox::dword_xt)                                                                      );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetRenderTarget,                          (xbox::X_D3DSurface*, xbox::X_D3DSurface*)                                                            );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetRenderTargetFast,                      (xbox::X_D3DSurface*, xbox::X_D3DSurface*, xbox::dword_xt)                                            );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetRenderTarget_0,                        ()                                                                                                    );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetScreenSpaceOffset,                     (xbox::float_xt, xbox::float_xt)                                                                      );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetShaderConstantMode,                    (xbox::X_VERTEXSHADERCONSTANTMODE)                                                                    );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetShaderConstantMode_0__LTCG_eax1,       ()                                                                                                    );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetStreamSource,                          (xbox::uint_xt, xbox::X_D3DVertexBuffer*, xbox::uint_xt)                                              );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetStreamSource_0__LTCG_eax_StreamNumber_edi_pStreamData_ebx_Stride, ()                                                                         );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetStreamSource_4,                        (xbox::uint_xt)                                                                                       );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetStreamSource_8,                        (xbox::X_D3DVertexBuffer*, xbox::uint_xt)                                                             );  \
    XB_MACRO(xbox::void_xt,       __fastcall, D3DDevice_SetStreamSource_8__LTCG_edx_StreamNumber, (void*, xbox::uint_xt, xbox::X_D3DVertexBuffer*, xbox::uint_xt)                                       );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetTexture,                               (xbox::dword_xt, xbox::X_D3DBaseTexture*)                                                             );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetTexture_4__LTCG_eax_pTexture,          (xbox::dword_xt)                                                                                      );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetTexture_4__LTCG_eax_Stage,             (xbox::X_D3DBaseTexture*)                                                                             );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_SetTile,                                  (xbox::dword_xt, CONST xbox::X_D3DTILE*)                                                              );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetTransform,                             (xbox::X_D3DTRANSFORMSTATETYPE, CONST D3DMATRIX*)                                                     );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetTransform_0__LTCG_eax1_edx2,           ()                                                                                                    );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetVertexData2f,                          (xbox::int_xt, xbox::float_xt, xbox::float_xt)                                                        );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetVertexData2s,                          (xbox::int_xt, xbox::short_xt, xbox::short_xt)                                                        );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetVertexData4f,                          (xbox::int_xt, xbox::float_xt, xbox::float_xt, xbox::float_xt, xbox::float_xt)                        );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetVertexData4f_16,                       (xbox::float_xt, xbox::float_xt, xbox::float_xt, xbox::float_xt)                                      );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetVertexData4ub,                         (xbox::int_xt, xbox::byte_xt, xbox::byte_xt, xbox::byte_xt, xbox::byte_xt)                            );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetVertexData4s,                          (xbox::int_xt, xbox::short_xt, xbox::short_xt, xbox::short_xt, xbox::short_xt)                        );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetVertexDataColor,                       (xbox::int_xt, D3DCOLOR)                                                                              );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetVertexShader,                          (xbox::dword_xt)                                                                                      );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetVertexShader_0,                        ()                                                                                                    );  \
    XB_MACRO(xbox::void_xt,       __fastcall, D3DDevice_SetVertexShaderConstant_8,                (void *, xbox::dword_xt, xbox::int_xt, CONST xbox::PVOID)                                             );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetVertexShaderConstant,                  (xbox::int_xt, CONST xbox::PVOID, xbox::dword_xt)                                                     );  \
    XB_MACRO(xbox::void_xt,       __fastcall, D3DDevice_SetVertexShaderConstant1,                 (xbox::int_xt, CONST xbox::PVOID)                                                                     );  \
    XB_MACRO(xbox::void_xt,       __fastcall, D3DDevice_SetVertexShaderConstant1Fast,             (xbox::int_xt, CONST xbox::PVOID)                                                                     );  \
    XB_MACRO(xbox::void_xt,       __fastcall, D3DDevice_SetVertexShaderConstant4,                 (xbox::int_xt, CONST xbox::PVOID)                                                                     );  \
    XB_MACRO(xbox::void_xt,       __fastcall, D3DDevice_SetVertexShaderConstantNotInline,         (xbox::int_xt, CONST xbox::PVOID, xbox::dword_xt)                                                     );  \
    XB_MACRO(xbox::void_xt,       __fastcall, D3DDevice_SetVertexShaderConstantNotInlineFast,     (xbox::int_xt, CONST xbox::PVOID, xbox::dword_xt)                                                     );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetVertexShaderInput,                     (xbox::dword_xt, xbox::uint_xt, xbox::X_STREAMINPUT*)                                                 );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetViewport,                              (CONST xbox::X_D3DVIEWPORT8*)                                                                         );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetSoftDisplayFilter,                     (bool)                                                                                                );  \
    XB_MACRO(xbox::dword_xt,      WINAPI,     D3DDevice_Swap,                                     (xbox::dword_xt)                                                                                      );  \
    XB_MACRO(xbox::dword_xt,      WINAPI,     D3DDevice_Swap_0,                                   ()                                                                                                    );  \
    XB_MACRO(xbox::void_xt,       __fastcall, D3DDevice_SwitchTexture,                            (xbox::dword_xt,xbox::dword_xt,xbox::dword_xt)                                                        );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3D_CommonSetRenderTarget,                          (xbox::X_D3DSurface*, xbox::X_D3DSurface*, void*)                                                     );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3D_DestroyResource,                                (xbox::X_D3DResource*)                                                                                );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3D_DestroyResource__LTCG,                          (xbox::void_xt)                                                                                       );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3D_UpdateProjectionViewportTransform,              (xbox::void_xt)                                                                                       );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     Direct3D_CreateDevice,                              (xbox::uint_xt, D3DDEVTYPE, HWND, xbox::dword_xt, xbox::X_D3DPRESENT_PARAMETERS*, xbox::X_D3DDevice**));  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     Direct3D_CreateDevice_16__LTCG_eax_BehaviorFlags_ebx_ppReturnedDeviceInterface, (xbox::uint_xt, D3DDEVTYPE, HWND, xbox::X_D3DPRESENT_PARAMETERS*)         );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     Direct3D_CreateDevice_16__LTCG_eax_BehaviorFlags_ecx_ppReturnedDeviceInterface, (xbox::uint_xt, D3DDEVTYPE, HWND, xbox::X_D3DPRESENT_PARAMETERS*)         );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     Direct3D_CreateDevice_4,                            (xbox::X_D3DPRESENT_PARAMETERS*)                                                                      );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     Lock2DSurface,                                      (xbox::X_D3DPixelContainer*, D3DCUBEMAP_FACES, xbox::uint_xt, D3DLOCKED_RECT*, RECT*, xbox::dword_xt) );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     Lock3DSurface,                                      (xbox::X_D3DPixelContainer*, xbox::uint_xt, D3DLOCKED_BOX*, D3DBOX*, xbox::dword_xt)                  );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     CreateStandAloneSurface,                            (DWORD, DWORD, xbox::X_D3DFORMAT, xbox::X_D3DSurface**)                                               );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DTexture_GetSurfaceLevel,                         (xbox::X_D3DTexture*, xbox::uint_xt, xbox::X_D3DSurface**)                                            );  \
    XB_MACRO(xbox::X_D3DSurface*, WINAPI,     D3DTexture_GetSurfaceLevel2,                        (xbox::X_D3DTexture*, xbox::uint_xt)                                                                  );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DResource_Register,                               (xbox::X_D3DResource*, PVOID)                                                                         );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     XGSetSurfaceHeader,                                 (UINT , UINT, xbox::X_D3DFORMAT, xbox::X_D3DSurface*, UINT, UINT)                                     );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     XGSetTextureHeader,                                 (UINT , UINT, UINT, DWORD, xbox::X_D3DFORMAT, D3DPOOL, xbox::X_D3DTexture*, UINT, UINT)               );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     XGSetVertexBufferHeader,                            (UINT , DWORD, DWORD, D3DPOOL, xbox::X_D3DVertexBuffer*, UINT)                                        );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DResource_AddRef,                                 (xbox::X_D3DResource*)                                                                                );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DResource_Release,                                (xbox::X_D3DResource*)                                                                                );  \

XB_TRAMPOLINES(XB_trampoline_declare);

void LookupTrampolinesD3D()
{
	XB_TRAMPOLINES(XB_trampoline_lookup);
}

#undef XB_TRAMPOLINES

const char *CxbxGetErrorDescription(HRESULT hResult)
{
	// TODO : For D3D9, Use DXGetErrorDescription9(hResult) (requires another DLL though)
	// See : https://www.fairyengine.com/articles/dxmultiviews.htm
	// and : https://web.archive.org/web/20101231193248/https://www.gamedev.net/community/forums/showfaq.asp?forum_id=10
	// and : https://www.gamedev.net/community/forums/topic.asp?topic_id=16157
	// But https://blogs.msdn.microsoft.com/chuckw/2012/04/24/wheres-dxerr-lib/
	// suggests to use FormatMessage with FORMAT_MESSAGE_FROM_SYSTEM for DirectX errors
	switch (hResult)
	{
	case D3DERR_INVALIDCALL: return "Invalid Call";
	case D3DERR_NOTAVAILABLE: return "Not Available";
	// case D3DERR_OUTOFVIDEOMEMORY: return "Out of Video Memory"; // duplicate of DDERR_OUTOFVIDEOMEMORY

	case D3D_OK: return "No error occurred.";
#if 0
	case D3DERR_BADMAJORVERSION: return "The service that you requested is unavailable in this major version of DirectX. (A major version denotes a primary release, such as DirectX 6.0.) ";
	case D3DERR_BADMINORVERSION: return "The service that you requested is available in this major version of DirectX, but not in this minor version. Get the latest version of the component run time from Microsoft. (A minor version denotes a secondary release, such as DirectX 6.1.) ";
	case D3DERR_COLORKEYATTACHED: return "The application attempted to create a texture with a surface that uses a color key for transparency. ";
#endif
	case D3DERR_CONFLICTINGTEXTUREFILTER: return "The current texture filters cannot be used together. ";
	case D3DERR_CONFLICTINGTEXTUREPALETTE: return "The current textures cannot be used simultaneously. This generally occurs when a multitexture device requires that all palettized textures simultaneously enabled also share the same palette. ";
	case D3DERR_CONFLICTINGRENDERSTATE: return "The currently set render states cannot be used together. ";
#if 0
	case D3DERR_DEVICEAGGREGATED: return "The IDirect3DDevice7::SetRenderTarget method was called on a device that was retrieved from the render target surface. ";
	case D3DERR_EXECUTE_CLIPPED_FAILED: return "The execute buffer could not be clipped during execution. ";
	case D3DERR_EXECUTE_CREATE_FAILED: return "The execute buffer could not be created. This typically occurs when no memory is available to allocate the execute buffer. ";
	case D3DERR_EXECUTE_DESTROY_FAILED: return "The memory for the execute buffer could not be deallocated. ";
	case D3DERR_EXECUTE_FAILED: return "The contents of the execute buffer are invalid and cannot be executed. ";
	case D3DERR_EXECUTE_LOCK_FAILED: return "The execute buffer could not be locked. ";
	case D3DERR_EXECUTE_LOCKED: return "The operation requested by the application could not be completed because the execute buffer is locked. ";
	case D3DERR_EXECUTE_NOT_LOCKED: return "The execute buffer could not be unlocked because it is not currently locked. ";
	case D3DERR_EXECUTE_UNLOCK_FAILED: return "The execute buffer could not be unlocked. ";
	case D3DERR_INBEGIN: return "The requested operation cannot be completed while scene rendering is taking place. Try again after the scene is completed and the IDirect3DDevice7::EndScene method is called. ";
	case D3DERR_INBEGINSTATEBLOCK: return "The operation cannot be completed while recording states for a state block. Complete recording by calling the IDirect3DDevice7::EndStateBlock method, and try again. ";
	case D3DERR_INITFAILED: return "A rendering device could not be created because the new device could not be initialized. ";
	case D3DERR_INVALID_DEVICE: return "The requested device type is not valid. ";
	case D3DERR_INVALIDCURRENTVIEWPORT: return "The currently selected viewport is not valid. ";
	case D3DERR_INVALIDMATRIX: return "The requested operation could not be completed because the combination of the currently set world, view, and projection matrices is invalid (the determinant of the combined matrix is 0). ";
	case D3DERR_INVALIDPALETTE: return "The palette associated with a surface is invalid. ";
	case D3DERR_INVALIDPRIMITIVETYPE: return "The primitive type specified by the application is invalid. ";
	case D3DERR_INVALIDRAMPTEXTURE: return "Ramp mode is being used, and the texture handle in the current material does not match the current texture handle that is set as a render state. ";
	case D3DERR_INVALIDSTATEBLOCK: return "The state block handle is invalid. ";
	case D3DERR_INVALIDVERTEXFORMAT: return "The combination of flexible vertex format flags specified by the application is not valid. ";
	case D3DERR_INVALIDVERTEXTYPE: return "The vertex type specified by the application is invalid. ";
	case D3DERR_LIGHT_SET_FAILED: return "The attempt to set lighting parameters for a light object failed. ";
	case D3DERR_LIGHTHASVIEWPORT: return "The requested operation failed because the light object is associated with another viewport. ";
	case D3DERR_LIGHTNOTINTHISVIEWPORT: return "The requested operation failed because the light object has not been associated with this viewport. ";
	case D3DERR_MATERIAL_CREATE_FAILED: return "The material could not be created. This typically occurs when no memory is available to allocate for the material. ";
	case D3DERR_MATERIAL_DESTROY_FAILED: return "The memory for the material could not be deallocated. ";
	case D3DERR_MATERIAL_GETDATA_FAILED: return "The material parameters could not be retrieved. ";
	case D3DERR_MATERIAL_SETDATA_FAILED: return "The material parameters could not be set. ";
	case D3DERR_MATRIX_CREATE_FAILED: return "The matrix could not be created. This can occur when no memory is available to allocate for the matrix. ";
	case D3DERR_MATRIX_DESTROY_FAILED: return "The memory for the matrix could not be deallocated. ";
	case D3DERR_MATRIX_GETDATA_FAILED: return "The matrix data could not be retrieved. This can occur when the matrix was not created by the current device. ";
	case D3DERR_MATRIX_SETDATA_FAILED: return "The matrix data could not be set. This can occur when the matrix was not created by the current device. ";
	case D3DERR_NOCURRENTVIEWPORT: return "The viewport parameters could not be retrieved because none have been set. ";
	case D3DERR_NOTINBEGIN: return "The requested rendering operation could not be completed because scene rendering has not begun. Call IDirect3DDevice7::BeginScene to begin rendering, and try again. ";
	case D3DERR_NOTINBEGINSTATEBLOCK: return "The requested operation could not be completed because it is only valid while recording a state block. Call the IDirect3DDevice7::BeginStateBlock method, and try again. ";
	case D3DERR_NOVIEWPORTS: return "The requested operation failed because the device currently has no viewports associated with it. ";
	case D3DERR_SCENE_BEGIN_FAILED: return "Scene rendering could not begin. ";
	case D3DERR_SCENE_END_FAILED: return "Scene rendering could not be completed. ";
	case D3DERR_SCENE_IN_SCENE: return "Scene rendering could not begin because a previous scene was not completed by a call to the IDirect3DDevice7::EndScene method. ";
	case D3DERR_SCENE_NOT_IN_SCENE: return "Scene rendering could not be completed because a scene was not started by a previous call to the IDirect3DDevice7::BeginScene method. ";
	case D3DERR_SETVIEWPORTDATA_FAILED: return "The viewport parameters could not be set. ";
	case D3DERR_STENCILBUFFER_NOTPRESENT: return "The requested stencil buffer operation could not be completed because there is no stencil buffer attached to the render target surface. ";
	case D3DERR_SURFACENOTINVIDMEM: return "The device could not be created because the render target surface is not located in video memory. (Hardware-accelerated devices require video-memory render target surfaces.) ";
	case D3DERR_TEXTURE_BADSIZE: return "The dimensions of a current texture are invalid. This can occur when an application attempts to use a texture that has dimensions that are not a power of 2 with a device that requires them. ";
	case D3DERR_TEXTURE_CREATE_FAILED: return "The texture handle for the texture could not be retrieved from the driver. ";
	case D3DERR_TEXTURE_DESTROY_FAILED: return "The device was unable to deallocate the texture memory. ";
	case D3DERR_TEXTURE_GETSURF_FAILED: return "The DirectDraw surface used to create the texture could not be retrieved. ";
	case D3DERR_TEXTURE_LOAD_FAILED: return "The texture could not be loaded. ";
	case D3DERR_TEXTURE_LOCK_FAILED: return "The texture could not be locked. ";
	case D3DERR_TEXTURE_LOCKED: return "The requested operation could not be completed because the texture surface is currently locked. ";
	case D3DERR_TEXTURE_NO_SUPPORT: return "The device does not support texture mapping. ";
	case D3DERR_TEXTURE_NOT_LOCKED: return "The requested operation could not be completed because the texture surface is not locked. ";
	case D3DERR_TEXTURE_SWAP_FAILED: return "The texture handles could not be swapped. ";
	case D3DERR_TEXTURE_UNLOCK_FAILED: return "The texture surface could not be unlocked. ";
#endif
	case D3DERR_TOOMANYOPERATIONS: return "The application is requesting more texture-filtering operations than the device supports. ";
#if 0
	case D3DERR_TOOMANYPRIMITIVES: return "The device is unable to render the provided number of primitives in a single pass. ";
#endif
	case D3DERR_UNSUPPORTEDALPHAARG: return "The device does not support one of the specified texture-blending arguments for the alpha channel. ";
	case D3DERR_UNSUPPORTEDALPHAOPERATION: return "The device does not support one of the specified texture-blending operations for the alpha channel. ";
	case D3DERR_UNSUPPORTEDCOLORARG: return "The device does not support one of the specified texture-blending arguments for color values. ";
	case D3DERR_UNSUPPORTEDCOLOROPERATION: return "The device does not support one of the specified texture-blending operations for color values. ";
	case D3DERR_UNSUPPORTEDFACTORVALUE: return "The specified texture factor value is not supported by the device. ";
	case D3DERR_UNSUPPORTEDTEXTUREFILTER: return "The specified texture filter is not supported by the device. ";
#if 0
	case D3DERR_VBUF_CREATE_FAILED: return "The vertex buffer could not be created. This can happen when there is insufficient memory to allocate a vertex buffer. ";
	case D3DERR_VERTEXBUFFERLOCKED: return "The requested operation could not be completed because the vertex buffer is locked. ";
	case D3DERR_VERTEXBUFFEROPTIMIZED: return "The requested operation could not be completed because the vertex buffer is optimized. (The contents of optimized vertex buffers are driver-specific and considered private.) ";
	case D3DERR_VERTEXBUFFERUNLOCKFAILED: return "The vertex buffer could not be unlocked because the vertex buffer memory was overrun. Be sure that your application does not write beyond the size of the vertex buffer. ";
	case D3DERR_VIEWPORTDATANOTSET: return "The requested operation could not be completed because viewport parameters have not yet been set. Set the viewport parameters by calling the IDirect3DDevice7::SetViewport method, and try again. ";
	case D3DERR_VIEWPORTHASNODEVICE: return "The requested operation could not be completed because the viewport has not yet been associated with a device. Associate the viewport with a rendering device by calling the IDirect3DDevice3::AddViewport method, and try again. ";
#endif
	case D3DERR_WRONGTEXTUREFORMAT: return "The pixel format of the texture surface is not valid. ";
#if 0
	case D3DERR_ZBUFF_NEEDS_SYSTEMMEMORY: return "The requested operation could not be completed because the specified device requires system-memory depth-buffer surfaces. (Software rendering devices require system-memory depth buffers.) ";
	case D3DERR_ZBUFF_NEEDS_VIDEOMEMORY: return "The requested operation could not be completed because the specified device requires video-memory depth-buffer surfaces. (Hardware-accelerated devices require video-memory depth buffers.) ";
	case D3DERR_ZBUFFER_NOTPRESENT: return "The requested operation could not be completed because the render target surface does not have an attached depth buffer. ";
	case DD_OK: return "The request completed successfully.";
#endif
	case DDERR_ALREADYINITIALIZED: return "The object has already been initialized.";
	case DDERR_BLTFASTCANTCLIP: return "A DirectDrawClipper object is attached to a source surface that has passed into a call to the IDirectDrawSurface7::BltFast method.";
	case DDERR_CANNOTATTACHSURFACE: return "A surface cannot be attached to another requested surface.";
	case DDERR_CANNOTDETACHSURFACE: return "A surface cannot be detached from another requested surface.";
	case DDERR_CANTCREATEDC: return "Windows cannot create any more device contexts (DCs), or a DC has requested a palette-indexed surface when the surface had no palette and the display mode was not palette-indexed (in this case DirectDraw cannot select a proper palette into the DC).";
	case DDERR_CANTDUPLICATE: return "Primary and 3-D surfaces, or surfaces that are implicitly created, cannot be duplicated.";
	case DDERR_CANTLOCKSURFACE: return "Access to this surface is refused because an attempt was made to lock the primary surface without DCI support.";
	case DDERR_CANTPAGELOCK: return "An attempt to page-lock a surface failed. Page lock does not work on a display-memory surface or an emulated primary surface.";
	case DDERR_CANTPAGEUNLOCK: return "An attempt to page-unlock a surface failed. Page unlock does not work on a display-memory surface or an emulated primary surface.";
	case DDERR_CLIPPERISUSINGHWND: return "An attempt was made to set a clip list for a DirectDrawClipper object that is already monitoring a window handle.";
	case DDERR_COLORKEYNOTSET: return "No source color key is specified for this operation.";
	case DDERR_CURRENTLYNOTAVAIL: return "No support is currently available.";
	case DDERR_DDSCAPSCOMPLEXREQUIRED: return "New for DirectX 7.0. The surface requires the DDSCAPS_COMPLEX flag.";
	case DDERR_DCALREADYCREATED: return "A device context (DC) has already been returned for this surface. Only one DC can be retrieved for each surface.";
	case DDERR_DEVICEDOESNTOWNSURFACE: return "Surfaces created by one DirectDraw device cannot be used directly by another DirectDraw device.";
	case DDERR_DIRECTDRAWALREADYCREATED: return "A DirectDraw object representing this driver has already been created for this process.";
	case DDERR_EXCEPTION: return "An exception was encountered while performing the requested operation.";
	case DDERR_EXCLUSIVEMODEALREADYSET: return "An attempt was made to set the cooperative level when it was already set to exclusive.";
	case DDERR_EXPIRED: return "The data has expired and is therefore no longer valid.";
	case DDERR_GENERIC: return "There is an undefined error condition.";
	case DDERR_HEIGHTALIGN: return "The height of the provided rectangle is not a multiple of the required alignment.";
	case DDERR_HWNDALREADYSET: return "The DirectDraw cooperative-level window handle has already been set. It cannot be reset while the process has surfaces or palettes created.";
	case DDERR_HWNDSUBCLASSED: return "DirectDraw is prevented from restoring state because the DirectDraw cooperative-level window handle has been subclassed.";
	case DDERR_IMPLICITLYCREATED: return "The surface cannot be restored because it is an implicitly created surface.";
	case DDERR_INCOMPATIBLEPRIMARY: return "The primary surface creation request does not match the existing primary surface.";
	case DDERR_INVALIDCAPS: return "One or more of the capability bits passed to the callback function are incorrect.";
	case DDERR_INVALIDCLIPLIST: return "DirectDraw does not support the provided clip list.";
	case DDERR_INVALIDDIRECTDRAWGUID: return "The globally unique identifier (GUID) passed to the DirectDrawCreate function is not a valid DirectDraw driver identifier.";
	case DDERR_INVALIDMODE: return "DirectDraw does not support the requested mode.";
	case DDERR_INVALIDOBJECT: return "DirectDraw received a pointer that was an invalid DirectDraw object.";
	case DDERR_INVALIDPARAMS: return "One or more of the parameters passed to the method are incorrect.";
	case DDERR_INVALIDPIXELFORMAT: return "The pixel format was invalid as specified.";
	case DDERR_INVALIDPOSITION: return "The position of the overlay on the destination is no longer legal.";
	case DDERR_INVALIDRECT: return "The provided rectangle was invalid.";
	case DDERR_INVALIDSTREAM: return "The specified stream contains invalid data.";
	case DDERR_INVALIDSURFACETYPE: return "The surface was of the wrong type.";
	case DDERR_LOCKEDSURFACES: return "One or more surfaces are locked, causing the failure of the requested operation.";
	case DDERR_MOREDATA: return "There is more data available than the specified buffer size can hold.";
	case DDERR_NEWMODE: return "New for DirectX 7.0. When IDirectDraw7::StartModeTest is called with the DDSMT_ISTESTREQUIRED flag, it may return this value to denote that some or all of the resolutions can and should be tested. IDirectDraw7::EvaluateMode returns this value to indicate that the test has switched to a new display mode.";
	case DDERR_NO3D: return "No 3-D hardware or emulation is present.";
	case DDERR_NOALPHAHW: return "No alpha-acceleration hardware is present or available, causing the failure of the requested operation.";
	case DDERR_NOBLTHW: return "No blitter hardware is present.";
	case DDERR_NOCLIPLIST: return "No clip list is available.";
	case DDERR_NOCLIPPERATTACHED: return "No DirectDrawClipper object is attached to the surface object.";
	case DDERR_NOCOLORCONVHW: return "No color-conversion hardware is present or available.";
	case DDERR_NOCOLORKEY: return "The surface does not currently have a color key.";
	case DDERR_NOCOLORKEYHW: return "There is no hardware support for the destination color key.";
	case DDERR_NOCOOPERATIVELEVELSET: return "A create function was called without the IDirectDraw7::SetCooperativeLevel method.";
	case DDERR_NODC: return "No device context (DC) has ever been created for this surface.";
	case DDERR_NODDROPSHW: return "No DirectDraw raster-operation (ROP) hardware is available.";
	case DDERR_NODIRECTDRAWHW: return "Hardware-only DirectDraw object creation is not possible; the driver does not support any hardware.";
	case DDERR_NODIRECTDRAWSUPPORT: return "DirectDraw support is not possible with the current display driver.";
	case DDERR_NODRIVERSUPPORT: return "New for DirectX 7.0. Testing cannot proceed because the display adapter driver does not enumerate refresh rates.";
	case DDERR_NOEMULATION: return "Software emulation is not available.";
	case DDERR_NOEXCLUSIVEMODE: return "The operation requires the application to have exclusive mode, but the application does not have exclusive mode.";
	case DDERR_NOFLIPHW: return "Flipping visible surfaces is not supported.";
	case DDERR_NOFOCUSWINDOW: return "An attempt was made to create or set a device window without first setting the focus window.";
	case DDERR_NOGDI: return "No GDI is present.";
	case DDERR_NOHWND: return "Clipper notification requires a window handle, or no window handle has been previously set as the cooperative level window handle.";
	case DDERR_NOMIPMAPHW: return "No mipmap-capable texture mapping hardware is present or available.";
	case DDERR_NOMIRRORHW: return "No mirroring hardware is present or available.";
	case DDERR_NOMONITORINFORMATION: return "New for DirectX 7.0. Testing cannot proceed because the monitor has no associated EDID data.";
	case DDERR_NONONLOCALVIDMEM: return "An attempt was made to allocate nonlocal video memory from a device that does not support nonlocal video memory.";
	case DDERR_NOOPTIMIZEHW: return "The device does not support optimized surfaces.";
	case DDERR_NOOVERLAYDEST: return "The IDirectDrawSurface7::GetOverlayPosition method is called on an overlay that the IDirectDrawSurface7::UpdateOverlay method has not been called on to establish as a destination.";
	case DDERR_NOOVERLAYHW: return "No overlay hardware is present or available.";
	case DDERR_NOPALETTEATTACHED: return "No palette object is attached to this surface.";
	case DDERR_NOPALETTEHW: return "There is no hardware support for 16- or 256-color palettes.";
	case DDERR_NORASTEROPHW: return "No appropriate raster-operation hardware is present or available.";
	case DDERR_NOROTATIONHW: return "No rotation hardware is present or available.";
	case DDERR_NOSTEREOHARDWARE: return "There is no stereo hardware present or available.";
	case DDERR_NOSTRETCHHW: return "There is no hardware support for stretching.";
	case DDERR_NOSURFACELEFT: return "There is no hardware present that supports stereo surfaces.";
	case DDERR_NOT4BITCOLOR: return "The DirectDrawSurface object is not using a 4-bit color palette, and the requested operation requires a 4-bit color palette.";
	case DDERR_NOT4BITCOLORINDEX: return "The DirectDrawSurface object is not using a 4-bit color index palette, and the requested operation requires a 4-bit color index palette.";
	case DDERR_NOT8BITCOLOR: return "The DirectDrawSurface object is not using an 8-bit color palette, and the requested operation requires an 8-bit color palette.";
	case DDERR_NOTAOVERLAYSURFACE: return "An overlay component is called for a nonoverlay surface.";
	case DDERR_NOTEXTUREHW: return "The operation cannot be carried out because no texture-mapping hardware is present or available.";
	case DDERR_NOTFLIPPABLE: return "An attempt was made to flip a surface that cannot be flipped.";
	case DDERR_NOTFOUND: return "The requested item was not found.";
	case DDERR_NOTINITIALIZED: return "An attempt was made to call an interface method of a DirectDraw object created by CoCreateInstance before the object was initialized.";
	case DDERR_NOTLOADED: return "The surface is an optimized surface, but it has not yet been allocated any memory.";
	case DDERR_NOTLOCKED: return "An attempt was made to unlock a surface that was not locked.";
	case DDERR_NOTPAGELOCKED: return "An attempt was made to page-unlock a surface with no outstanding page locks.";
	case DDERR_NOTPALETTIZED: return "The surface being used is not a palette-based surface.";
	case DDERR_NOVSYNCHW: return "There is no hardware support for vertical blanksynchronized operations.";
	case DDERR_NOZBUFFERHW: return "The operation to create a z-buffer in display memory or to perform a blit, using a z-buffer cannot be carried out because there is no hardware support for z-buffers.";
	case DDERR_NOZOVERLAYHW: return "The overlay surfaces cannot be z-layered, based on the z-order because the hardware does not support z-ordering of overlays.";
	case DDERR_OUTOFCAPS: return "The hardware needed for the requested operation has already been allocated.";
	case DDERR_OUTOFMEMORY: return "DirectDraw does not have enough memory to perform the operation.";
	case DDERR_OUTOFVIDEOMEMORY: return "DirectDraw does not have enough display memory to perform the operation.";
	case DDERR_OVERLAPPINGRECTS: return "The source and destination rectangles are on the same surface and overlap each other.";
	case DDERR_OVERLAYCANTCLIP: return "The hardware does not support clipped overlays.";
	case DDERR_OVERLAYCOLORKEYONLYONEACTIVE: return "An attempt was made to have more than one color key active on an overlay.";
	case DDERR_OVERLAYNOTVISIBLE: return "The IDirectDrawSurface7::GetOverlayPosition method was called on a hidden overlay.";
	case DDERR_PALETTEBUSY: return "Access to this palette is refused because the palette is locked by another thread.";
	case DDERR_PRIMARYSURFACEALREADYEXISTS: return "This process has already created a primary surface.";
	case DDERR_REGIONTOOSMALL: return "The region passed to the IDirectDrawClipper::GetClipList method is too small.";
	case DDERR_SURFACEALREADYATTACHED: return "An attempt was made to attach a surface to another surface to which it is already attached.";
	case DDERR_SURFACEALREADYDEPENDENT: return "An attempt was made to make a surface a dependency of another surface on which it is already dependent.";
	case DDERR_SURFACEBUSY: return "Access to the surface is refused because the surface is locked by another thread.";
	case DDERR_SURFACEISOBSCURED: return "Access to the surface is refused because the surface is obscured.";
	case DDERR_SURFACELOST: return "Access to the surface is refused because the surface memory is gone. Call the IDirectDrawSurface7::Restore method on this surface to restore the memory associated with it.";
	case DDERR_SURFACENOTATTACHED: return "The requested surface is not attached.";
	case DDERR_TESTFINISHED: return "New for DirectX 7.0. When returned by the IDirectDraw7::StartModeTest method, this value means that no test could be initiated because all the resolutions chosen for testing already have refresh rate information in the registry. When returned by IDirectDraw7::EvaluateMode, the value means that DirectDraw has completed a refresh rate test.";
	case DDERR_TOOBIGHEIGHT: return "The height requested by DirectDraw is too large.";
	case DDERR_TOOBIGSIZE: return "The size requested by DirectDraw is too large. However, the individual height and width are valid sizes.";
	case DDERR_TOOBIGWIDTH: return "The width requested by DirectDraw is too large.";
	case DDERR_UNSUPPORTED: return "The operation is not supported.";
	case DDERR_UNSUPPORTEDFORMAT: return "The pixel format requested is not supported by DirectDraw.";
	case DDERR_UNSUPPORTEDMASK: return "The bitmask in the pixel format requested is not supported by DirectDraw.";
	case DDERR_UNSUPPORTEDMODE: return "The display is currently in an unsupported mode.";
	case DDERR_VERTICALBLANKINPROGRESS: return "A vertical blank is in progress.";
	case DDERR_VIDEONOTACTIVE: return "The video port is not active.";
	case DDERR_WASSTILLDRAWING: return "The previous blit operation that is transferring information to or from this surface is incomplete.";
	case DDERR_WRONGMODE: return "This surface cannot be restored because it was created in a different mode.";
	case DDERR_XALIGN: return "The provided rectangle was not horizontally aligned on a required boundary.";
	}

	return nullptr;
}

static constexpr UINT WM_CXBXR_RUN_ON_MESSAGE_THREAD = WM_USER+0;

// A helper function to run any code on a window message thread
// Used for those D3D9 function which *must* be ran on this particular thread
static void RunOnWndMsgThread(const std::function<void()>& func)
{
	const void* param = &func;
	SendMessage(g_hEmuWindow, WM_CXBXR_RUN_ON_MESSAGE_THREAD, reinterpret_cast<WPARAM>(param), 0);
}


const char *D3DErrorString(HRESULT hResult)
{
	static char buffer[1024];
	buffer[0] = 0; // Reset static buffer!

	const char* errorCodeString = DXGetErrorString(hResult);
	if (errorCodeString)
	{
		strcat(buffer, errorCodeString);
		strcat(buffer, ": ");
	}

	const char* errorDescription = CxbxGetErrorDescription(hResult);
	if (errorDescription)
		strcat(buffer, errorDescription);
	else
		strcat(buffer, "Unknown D3D error.");

	return buffer;
}

void CxbxInitWindow(bool bFullInit)
{
    g_EmuShared->GetVideoSettings(&g_XBVideo);

    if(g_XBVideo.bFullScreen)
        CxbxKrnl_hEmuParent = NULL;

    // create timing thread
	if (bFullInit && !bLLE_GPU)
    {
		xbox::HANDLE hThread;
		xbox::PsCreateSystemThread(&hThread, xbox::zeroptr, EmuUpdateTickCount, xbox::zeroptr, FALSE);
        // We set the priority of this thread a bit higher, to assure reliable timing :
		auto nativeHandle = GetNativeHandle(hThread);
		assert(nativeHandle);
        SetThreadPriority(*nativeHandle, THREAD_PRIORITY_ABOVE_NORMAL);
        g_AffinityPolicy->SetAffinityOther(*nativeHandle);
    }

/* TODO : Port this Dxbx code :
  // create vblank handling thread
    {
        dwThreadId = 0;
        {hThread :=} CreateThread(nullptr, 0, EmuThreadHandleVBlank, nullptr, 0, &dwThreadId);
    }
*/
    // create window message processing thread
    {
		HANDLE hStartEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
		HANDLE hRenderWindowThread = CreateThread(nullptr, 0, EmuRenderWindow, &hStartEvent, 0, nullptr);

		if (hRenderWindowThread == NULL) {
			char szBuffer[1024] = { 0 };
			sprintf(szBuffer, "Creating EmuRenderWindowThread Failed: %08X", GetLastError());
			PopupFatal(nullptr, szBuffer);
			EmuShared::Cleanup();
			ExitProcess(0);
		}
		g_AffinityPolicy->SetAffinityOther(hRenderWindowThread);

		// Wait for the window to create
		WaitForSingleObject(hStartEvent, INFINITE);
		CloseHandle(hStartEvent);
		CloseHandle(hRenderWindowThread);
    }

	SetFocus(g_hEmuWindow);
	g_renderbase = std::unique_ptr<RenderBase>(new RenderBase());
	g_renderbase->Initialize();

	ImGui_ImplWin32_Init(g_hEmuWindow);
	g_renderbase->SetWindowRelease([] {
		ImGui_ImplWin32_Shutdown();
	});
}

void DrawUEM(HWND hWnd)
{
	// Draw the universal error message (UEM)
	// See https://xboxdevwiki.net/Fatal_Error
	// Only call this from WM_PAINT message!

	PAINTSTRUCT ps;

	BeginPaint(hWnd, &ps);

	HDC hDC = GetDC(hWnd);
	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hUEMBmp = CreateCompatibleBitmap(hDC, 640, 480);
	HBITMAP hOriUEMBmp = (HBITMAP)SelectObject(hMemDC, hUEMBmp);


	int nHeight = -MulDiv(8, GetDeviceCaps(hMemDC, LOGPIXELSY), 72);

	HFONT hFont = CreateFont(nHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_ROMAN, "Verdana");

	HGDIOBJ tmpObj = SelectObject(hMemDC, hFont);

	SetBkColor(hMemDC, RGB(0, 0, 0));

	SetTextColor(hMemDC, RGB(0, 204, 0));

	std::wstring utf16str = utf8_to_utf16(uem_str.c_str());

	// Unfortunately, DrawTextW doesn't support vertical alignemnt, so we have to do the calculation
	// ourselves. See here: https://social.msdn.microsoft.com/Forums/vstudio/en-US/abd89aae-16a0-41c6-8db6-b119ea90b42a/win32-drawtext-how-center-in-vertical-with-new-lines-and-tabs?forum=vclanguage

	RECT rect = { 0, 0, 640, 480 };
	RECT textrect = { 0, 0, 640, 480 };
	DrawTextW(hMemDC, utf16str.c_str(), utf16str.length(), &textrect, DT_CALCRECT);
	rect.top = (rect.bottom - textrect.bottom) / 2;
	DrawTextW(hMemDC, utf16str.c_str(), utf16str.length(), &rect, DT_CENTER);


	// Draw the Xbox error code

	SetTextColor(hMemDC, RGB(255, 255, 255));
	std::string err_str(std::to_string(g_CxbxFatalErrorCode));
	rect.left = 20;
	DrawText(hMemDC, err_str.c_str(), err_str.length(), &rect, DT_LEFT);

	GetClientRect(hWnd, &rect);
	SetStretchBltMode(hDC, COLORONCOLOR);
	StretchBlt(hDC, rect.left, rect.top, rect.right, rect.bottom, hMemDC, 0, 0, 640, 480, SRCCOPY);

	SelectObject(hMemDC, hOriUEMBmp);
	SelectObject(hDC, tmpObj);

	DeleteObject(hUEMBmp);
	DeleteObject(hFont);
	DeleteObject(hMemDC);

	if (hDC != NULL)
		ReleaseDC(hWnd, hDC);

	EndPaint(hWnd, &ps);
}

void CxbxClipCursor(HWND hWnd)
{
	RECT wnd_rect;
	GetWindowRect(hWnd, &wnd_rect);
	ClipCursor(&wnd_rect);
}

void CxbxReleaseCursor()
{
	ClipCursor(nullptr);
}

static void CxbxUpdateCursor(bool forceShow = false) {
	// Getting cursor info is a requirement in order to prevent a bug occur with ShowCursor redundant calls.
	CURSORINFO cursorInfo;
	cursorInfo.cbSize = sizeof(cursorInfo);
	if (!GetCursorInfo(&cursorInfo)) {
		// If cursor info is not available, then ignore the cursor update.
		return;
	}

	if (g_renderbase && g_renderbase->IsImGuiFocus() || forceShow) {
		if (cursorInfo.flags == 0) {
			ShowCursor(TRUE);
		}
	}
	else {
		if ((cursorInfo.flags & CURSOR_SHOWING) != 0) {
			ShowCursor(FALSE);
		}
	}
}

inline DWORD GetXboxCommonResourceType(const xbox::dword_xt XboxResource_Common)
{
	DWORD dwCommonType = XboxResource_Common & X_D3DCOMMON_TYPE_MASK;
	return dwCommonType;
}

inline DWORD GetXboxCommonResourceType(const xbox::X_D3DResource* pXboxResource)
{
	// Don't pass in unassigned Xbox resources
	assert(pXboxResource != xbox::zeroptr);

	return GetXboxCommonResourceType(pXboxResource->Common);
}

inline int GetXboxPixelContainerDimensionCount(const xbox::X_D3DPixelContainer *pXboxPixelContainer)
{
	// Don't pass in unassigned Xbox pixel container
	assert(pXboxPixelContainer != xbox::zeroptr);

	return (xbox::X_D3DFORMAT)((pXboxPixelContainer->Format & X_D3DFORMAT_DIMENSION_MASK) >> X_D3DFORMAT_DIMENSION_SHIFT);
}

xbox::X_D3DRESOURCETYPE GetXboxD3DResourceType(const xbox::X_D3DResource *pXboxResource)
{
	DWORD Type = GetXboxCommonResourceType(pXboxResource);
	switch (Type)
	{
	case X_D3DCOMMON_TYPE_VERTEXBUFFER:
		return xbox::X_D3DRTYPE_VERTEXBUFFER;
	case X_D3DCOMMON_TYPE_INDEXBUFFER:
		return xbox::X_D3DRTYPE_INDEXBUFFER;
	case X_D3DCOMMON_TYPE_PUSHBUFFER:
		return xbox::X_D3DRTYPE_PUSHBUFFER;
	case X_D3DCOMMON_TYPE_PALETTE:
		return xbox::X_D3DRTYPE_PALETTE;
	case X_D3DCOMMON_TYPE_TEXTURE:
	{
		DWORD Format = ((xbox::X_D3DPixelContainer *)pXboxResource)->Format;
		if (Format & X_D3DFORMAT_CUBEMAP)
			return xbox::X_D3DRTYPE_CUBETEXTURE;

		if (GetXboxPixelContainerDimensionCount((xbox::X_D3DPixelContainer *)pXboxResource) > 2)
			return xbox::X_D3DRTYPE_VOLUMETEXTURE;

		return xbox::X_D3DRTYPE_TEXTURE;
	}
	case X_D3DCOMMON_TYPE_SURFACE:
	{
		if (GetXboxPixelContainerDimensionCount((xbox::X_D3DPixelContainer *)pXboxResource) > 2)
			return xbox::X_D3DRTYPE_VOLUME;

		return xbox::X_D3DRTYPE_SURFACE;
	}
	case X_D3DCOMMON_TYPE_FIXUP:
		return xbox::X_D3DRTYPE_FIXUP;
	}

	return xbox::X_D3DRTYPE_NONE;
}

// This can be used to determine if resource Data adddresses
// need the PHYSICAL_MAP_BASE  bit set or cleared
inline bool IsResourceTypeGPUReadable(const DWORD ResourceType)
{
	switch (ResourceType) {
	case X_D3DCOMMON_TYPE_VERTEXBUFFER:
		return true;
	case X_D3DCOMMON_TYPE_INDEXBUFFER:
		/// assert(false); // Index buffers are not allowed to be registered
		break;
	case X_D3DCOMMON_TYPE_PUSHBUFFER:
		return false;
	case X_D3DCOMMON_TYPE_PALETTE:
		return true;
	case X_D3DCOMMON_TYPE_TEXTURE:
		return true;
	case X_D3DCOMMON_TYPE_SURFACE:
		return true;
	case X_D3DCOMMON_TYPE_FIXUP:
		// assert(false); // Fixup's are not allowed to be registered
		break;
	default:
		CxbxrAbort("Unhandled resource type");
	}

	return false;
}

inline bool IsPaletizedTexture(const xbox::dword_xt XboxPixelContainer_Format)
{
	return GetXboxPixelContainerFormat(XboxPixelContainer_Format) == xbox::X_D3DFMT_P8;
}

#if 0 // unused
inline bool IsYuvSurfaceOrTexture(const xbox::X_D3DResource* pXboxResource)
{
	if (GetXboxPixelContainerFormat((xbox::X_D3DPixelContainer *)pXboxResource) == xbox::X_D3DFMT_YUY2)
		return true;

	return false;
}
#endif

#if 0 // unused
inline bool IsXboxResourceLocked(const xbox::X_D3DResource *pXboxResource)
{
	bool result = !!(pXboxResource->Common & X_D3DCOMMON_ISLOCKED);
	return result;
}
#endif

#if 0 // unused
inline bool IsXboxResourceD3DCreated(const xbox::X_D3DResource *pXboxResource)
{
	bool result = !!(pXboxResource->Common & X_D3DCOMMON_D3DCREATED);
	return result;
}
#endif

void *GetDataFromXboxResource(xbox::X_D3DResource *pXboxResource)
{
	// Don't pass in unassigned Xbox resources
	if (pXboxResource == xbox::zeroptr)
		return nullptr;

	xbox::addr_xt pData = pXboxResource->Data;
	if (pData == xbox::zero)
		return nullptr;

	DWORD dwCommonType = GetXboxCommonResourceType(pXboxResource);
	if (IsResourceTypeGPUReadable(dwCommonType))
		pData |= PHYSICAL_MAP_BASE;

	return (uint8_t*)pData;
}

typedef struct _resource_info_t {
	ComPtr<IDirect3DResource> pHostResource;
	D3DFORMAT PCFormat = D3DFMT_UNKNOWN;
	DWORD dwXboxResourceType = 0;
	void* pXboxData = xbox::zeroptr;
	size_t szXboxDataSize = 0;
	uint64_t hash = 0;
	bool forceRehash = false;
	std::chrono::time_point<std::chrono::steady_clock> nextHashTime;
	std::chrono::milliseconds hashLifeTime = 1ms;
    std::chrono::time_point<std::chrono::steady_clock> lastUpdate;
} resource_info_t;

typedef std::unordered_map<resource_key_t, resource_info_t, resource_key_hash> resource_cache_t;
resource_cache_t g_Cxbx_Cached_Direct3DResources;
resource_cache_t g_Cxbx_Cached_PaletizedTextures;

bool IsResourceAPixelContainer(xbox::dword_xt XboxResource_Common)
{
	DWORD Type = GetXboxCommonResourceType(XboxResource_Common);
	switch (Type)
	{
	case X_D3DCOMMON_TYPE_TEXTURE:
	case X_D3DCOMMON_TYPE_SURFACE:
		return true;
	}

	return false;
}

bool IsResourceAPixelContainer(xbox::X_D3DResource* pXboxResource)
{
	// Don't pass in unassigned Xbox resources
	assert(pXboxResource != xbox::zeroptr);

	return IsResourceAPixelContainer(pXboxResource->Common);
}

resource_cache_t& GetResourceCache(resource_key_t& key)
{
	return IsResourceAPixelContainer(key.Common) && IsPaletizedTexture(key.Format)
		? g_Cxbx_Cached_PaletizedTextures : g_Cxbx_Cached_Direct3DResources;
}
extern bool is_pgraph_using_NV2A_Kelvin(void);
resource_key_t GetHostResourceKey(xbox::X_D3DResource* pXboxResource, int iTextureStage = -1)
{
	resource_key_t key = {};
	if (pXboxResource != xbox::zeroptr) {
		// Initially, don't base the key on the address of the resource, but on it's uniquely identifying values
		key.Data = pXboxResource->Data;
		key.Common = pXboxResource->Common & CXBX_D3DCOMMON_IDENTIFYING_MASK;
		if (IsResourceAPixelContainer(pXboxResource)) {
			// Pixel containers have more values they must be identified by:
			auto pPixelContainer = (xbox::X_D3DPixelContainer*)pXboxResource;
			key.Format = pPixelContainer->Format;
			key.Size = pPixelContainer->Size;

			void* data;
			int size;
			data = g_pXbox_Palette_Data[iTextureStage];
			size = g_Xbox_Palette_Size[iTextureStage];
			if (is_pgraph_using_NV2A_Kelvin()) {
				data = g_pNV2A_Palette_Data[iTextureStage];
				size = g_NV2A_Palette_Size[iTextureStage];
			}

			// For paletized textures, include the current palette hash as well
			if (IsPaletizedTexture(pPixelContainer->Format)) {
				if (iTextureStage < 0) {
					// ForceResourceRehash (called by Lock[23]DSurface) could hit this (not knowing the texture-stage)
					LOG_TEST_CASE("Unknown texture stage!");
				} else {
					assert(iTextureStage < xbox::X_D3DTS_STAGECOUNT);
					// Protect for when this gets hit before an actual palette is set
					if (size > 0) {
						// This caters for palette changes (only the active one will be used,
						// any intermediate changes have no effect). Obsolete palette texture
						// conversions will be pruned from g_Cxbx_Cached_PaletizedTextures
						key.PaletteHash = ComputeHash(data, size);
					}
				}
			}
		} else {
			// For other resource types, do include their Xbox resource address (TODO : come up with something better)
			key.ResourceAddr = (xbox::addr_xt)pXboxResource;
		}
	}

	return key;
}

void FreeHostResource(resource_key_t key)
{
	// Release the host resource and remove it from the list
	auto& ResourceCache = GetResourceCache(key);
	ResourceCache.erase(key);
}

void ClearResourceCache(resource_cache_t& ResourceCache)
{
	ResourceCache.clear();
}

void PrunePaletizedTexturesCache()
{
	// TODO : Implement a better cache eviction algorithm (like least-recently used, or just at-random)
	// Poor mans cache eviction policy: just clear it once it overflows
	if (g_Cxbx_Cached_PaletizedTextures.size() >= 1500) {
		ClearResourceCache(g_Cxbx_Cached_PaletizedTextures);
	}
}

void ForceResourceRehash(xbox::X_D3DResource* pXboxResource)
{
	auto key = GetHostResourceKey(pXboxResource); // Note : iTextureStage is unknown here!
	auto& ResourceCache = GetResourceCache(key);
	auto it = ResourceCache.find(key);
	if (it != ResourceCache.end() && it->second.pHostResource) {
		it->second.forceRehash = true;
	}
}

IDirect3DResource *GetHostResource(xbox::X_D3DResource *pXboxResource, DWORD D3DUsage = 0, int iTextureStage = -1)
{
	if (pXboxResource == xbox::zeroptr || pXboxResource->Data == xbox::zero)
		return nullptr;

	EmuVerifyResourceIsRegistered(pXboxResource, D3DUsage, iTextureStage, /*dwSize=*/0);

	auto key = GetHostResourceKey(pXboxResource, iTextureStage);
	auto& ResourceCache = GetResourceCache(key);
	auto it = ResourceCache.find(key);
	if (it == ResourceCache.end() || !it->second.pHostResource) {
		EmuLog(LOG_LEVEL::WARNING, "GetHostResource: Resource not registered or does not have a host counterpart!");
		return nullptr;
	}

	return it->second.pHostResource.Get();
}

void CxbxrSetSuperSampleScaleXY(float x, float y)
{
	g_SuperSampleScaleX = x;
	g_SuperSampleScaleY = y;
}
void CxbxrSetScreenSpaceOffsetXY(float x, float y)
{
	g_ScreenSpaceOffsetX = x;
	g_ScreenSpaceOffsetY = y;
}

void CxbxrSetZScale(float z)
{
	g_ZScale=z;
}
void CxbxrSetWScale(float w)
{
	g_ZScale = w;
}
void CxbxrSetViewport(xbox::X_D3DVIEWPORT8& Viewport)
{
	g_NV2A_Viewport=Viewport;
}
float CxbxrGetZScale(void)
{
	float ssScale = g_ZScale;
	if (ssScale < 1.0)
	{
		ssScale = 1.0;
	}
	return ssScale;
}
float CxbxrGetWScale(void)
{
	float ssScale = g_WScale;
	if (ssScale < 1.0)
	{
		ssScale = 1.0;
	}
	return ssScale;
}

float CxbxrGetSuperSampleScale(void)
{
	float ssScale=MIN(g_SuperSampleScaleX,g_SuperSampleScaleY);
	if (ssScale<1.0)
	{
		ssScale = 1.0;
	}
	return ssScale;
}
extern void GetMultiSampleScaleRaw(float& xScale, float& yScale);
void CxbxrGetSuperSampleScaleXY(NV2AState* d,float& x, float& y)
{
	PGRAPHState* pg = &d->pgraph;
	//get aaScaleX, aaScaleY
	GetMultiSampleScaleRaw(x, y);
	DWORD surfaceFormat = pg->KelvinPrimitive.SetSurfaceFormat;
	
	//D3DMULTISAMPLEMODE_4X
	if ((surfaceFormat & (NV097_SET_SURFACE_FORMAT_ANTI_ALIASING_SQUARE_OFFSET_4<<12)) != 0) {
		y /= 2.0;
	}
	//D3DMULTISAMPLEMODE_2X
	if ((surfaceFormat & (NV097_SET_SURFACE_FORMAT_ANTI_ALIASING_CENTER_CORNER_2<<12)) != 0) {
		x /= 2.0;
	}
	/*x = g_SuperSampleScaleX;
	y = g_SuperSampleScaleY;
	*/
}
void CxbxrSetWNearFarInverseWFar(float& WNear, float& WFar, float& WInverseWFar)
{
	g_WNear = WNear;
	g_WFar = WFar;
	g_WInverseWFar = WInverseWFar;
}
void CxbxrGetWNearFarInverseWFar(float& WNear, float& WFar, float& WInverseWFar)
{
	WNear = g_WNear ;
	WFar = g_WFar  ;
	WInverseWFar = g_WInverseWFar;
}
void CxbxrGetScreenSpaceOffsetXY(float &x, float &y)
{
	x = g_ScreenSpaceOffsetX;
	y = g_ScreenSpaceOffsetY;
}
void CxbxrGetViewport(xbox::X_D3DVIEWPORT8& Viewport)
{
	Viewport=g_NV2A_Viewport;
}
// Forward declaration of CxbxGetPixelContainerMeasures to prevent
// polluting the diff too much by reshuffling functions around
void CxbxGetPixelContainerMeasures
(
	xbox::X_D3DPixelContainer *pPixelContainer,
	DWORD dwMipMapLevel,
	UINT *pWidth,
	UINT *pHeight,
	UINT *pDepth,
	UINT *pRowPitch,
	UINT *pSlicePitch
);

size_t GetXboxResourceSize(xbox::X_D3DResource* pXboxResource)
{
	// TODO: Smart size calculation based around format of resource
	if (IsResourceAPixelContainer(pXboxResource)) {
		unsigned int Width, Height, Depth, RowPitch, SlicePitch;
		// TODO : Accumulate all mipmap levels!!!
		CxbxGetPixelContainerMeasures(
			(xbox::X_D3DPixelContainer*)pXboxResource,
			0, // dwMipMapLevel
			&Width,
			&Height,
			&Depth,
			&RowPitch,
			&SlicePitch
		);

		return SlicePitch * Depth;
	} else {
		// Fallback to querying the allocation size, if no other calculation was present
		return xbox::MmQueryAllocationSize(GetDataFromXboxResource(pXboxResource));
	}
	
}

bool HostResourceRequiresUpdate(resource_key_t key, xbox::X_D3DResource* pXboxResource, DWORD dwSize)
{
	if (pXboxResource == nullptr) {
		return false;
	}

	// Currently, we only dynamically update Textures and Surfaces, so if our resource
	// isn't of these types, do nothing
	if (!IsResourceAPixelContainer(pXboxResource)) {
		return false;
	}

	auto& ResourceCache = GetResourceCache(key);
	auto it = ResourceCache.find(key);
	if (it == ResourceCache.end()) {
		return false;
	}

	// If the resource size got bigger, we need to re-create it
	// if it got smaller, just hashing will suffice
	if (dwSize > it->second.szXboxDataSize) {
		return true;
	}

	// If the resource type changed, we need to re-create it
	if (it->second.dwXboxResourceType != GetXboxCommonResourceType(pXboxResource)) {
		return true;
	}

	bool modified = false;

	auto now = std::chrono::steady_clock::now();
	if (now > it->second.nextHashTime || it->second.forceRehash) {
		uint64_t oldHash = it->second.hash;
		it->second.hash = ComputeHash(it->second.pXboxData, it->second.szXboxDataSize);

		if (it->second.hash != oldHash) {
			// The data changed, so reset the hash lifetime
			it->second.hashLifeTime = 1ms;
            it->second.lastUpdate = now;
			modified = true;
		} else if (it->second.lastUpdate + 1000ms < now) {
			// The data did not change, so increase the hash lifetime
			// TODO: choose a sensible upper limit
			if (it->second.hashLifeTime < 1000ms) {
				it->second.hashLifeTime += 10ms;
			}
		}
		
		it->second.forceRehash = false;
	}

	// Update the next hash time based on the hash lifetime
	it->second.nextHashTime = now + it->second.hashLifeTime;

	return modified;
}

void SetHostResource(xbox::X_D3DResource* pXboxResource, IDirect3DResource* pHostResource, int iTextureStage = -1, DWORD dwSize = 0)
{
	auto key = GetHostResourceKey(pXboxResource, iTextureStage);
	auto& ResourceCache = GetResourceCache(key);
	auto& resourceInfo = ResourceCache[key];	// Implicitely inserts a new entry if not already existing

	if (resourceInfo.pHostResource) {
		EmuLog(LOG_LEVEL::WARNING, "SetHostResource: Overwriting an existing host resource");
	}

	// Increments reference count
	resourceInfo.pHostResource = pHostResource;
	resourceInfo.dwXboxResourceType = GetXboxCommonResourceType(pXboxResource);
	resourceInfo.pXboxData = GetDataFromXboxResource(pXboxResource);
	resourceInfo.szXboxDataSize = dwSize > 0 ? dwSize : GetXboxResourceSize(pXboxResource);
	resourceInfo.hash = ComputeHash(resourceInfo.pXboxData, resourceInfo.szXboxDataSize);
	resourceInfo.hashLifeTime = 1ms;
	resourceInfo.lastUpdate = std::chrono::steady_clock::now();
	resourceInfo.nextHashTime = resourceInfo.lastUpdate + resourceInfo.hashLifeTime;
	resourceInfo.forceRehash = false;
}

void EraseHostResource(xbox::X_D3DResource* pXboxResource,  int iTextureStage = -1)
{
	auto key = GetHostResourceKey(pXboxResource, iTextureStage);
	auto& ResourceCache = GetResourceCache(key);
	auto  it = ResourceCache.find(key);
	if (it != ResourceCache.end()) {
		ResourceCache.erase(key);
	}
}

void SetHostResourceForcedXboxData(xbox::X_D3DResource* pXboxResource, IDirect3DResource* pHostResource, int iTextureStage = -1, DWORD dwSize = 0)
{
	auto key = GetHostResourceKey(pXboxResource, iTextureStage);
	auto& ResourceCache = GetResourceCache(key);
	auto& resourceInfo = ResourceCache[key];	// Implicitely inserts a new entry if not already existing

	if (resourceInfo.pHostResource) {
		EmuLog(LOG_LEVEL::WARNING, "SetHostResource: Overwriting an existing host resource");
	}

	// Increments reference count
	resourceInfo.pHostResource = pHostResource;
	resourceInfo.dwXboxResourceType = GetXboxCommonResourceType(pXboxResource);
	resourceInfo.pXboxData = (void *)pXboxResource->Data;
	resourceInfo.szXboxDataSize = dwSize > 0 ? dwSize : GetXboxResourceSize(pXboxResource);
	resourceInfo.hash = ComputeHash(resourceInfo.pXboxData, resourceInfo.szXboxDataSize);
	resourceInfo.hashLifeTime = 1ms;
	resourceInfo.lastUpdate = std::chrono::steady_clock::now();
	resourceInfo.nextHashTime = resourceInfo.lastUpdate + resourceInfo.hashLifeTime;
	resourceInfo.forceRehash = false;
}

void SetHostResourcehashLifeTime(xbox::X_D3DResource* pXboxResource, std::chrono::milliseconds hashLifeTime=1ms, int iTextureStage = -1)
{
	auto key = GetHostResourceKey(pXboxResource, iTextureStage);
	auto& ResourceCache = GetResourceCache(key);
	auto& resourceInfo = ResourceCache[key];	// Implicitely inserts a new entry if not already existing

	resourceInfo.hashLifeTime = hashLifeTime;
	resourceInfo.lastUpdate = std::chrono::steady_clock::now();
	resourceInfo.nextHashTime = resourceInfo.lastUpdate + resourceInfo.hashLifeTime;
	resourceInfo.forceRehash = false;
}

void SetHostResourcePCFormat(xbox::X_D3DResource* pXboxResource, D3DFORMAT PCFormat, int iTextureStage = -1)
{
	auto key = GetHostResourceKey(pXboxResource, iTextureStage);
	auto& ResourceCache = GetResourceCache(key);
	auto& resourceInfo = ResourceCache[key];	// Implicitely inserts a new entry if not already existing

	// Increments reference count
	resourceInfo.PCFormat = PCFormat;
}

IDirect3DSurface *GetHostSurface(xbox::X_D3DResource *pXboxResource, DWORD D3DUsage = 0)
{
	if (pXboxResource == xbox::zeroptr)
		return nullptr;

	if (GetXboxCommonResourceType(pXboxResource) != X_D3DCOMMON_TYPE_SURFACE) // Allows breakpoint below
		assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_SURFACE);

	return (IDirect3DSurface*) GetHostResource(pXboxResource, D3DUsage);
}

IDirect3DBaseTexture *GetHostBaseTexture(xbox::X_D3DResource *pXboxResource, DWORD D3DUsage = 0, int iTextureStage = 0)
{
	if (pXboxResource == xbox::zeroptr)
		return nullptr;

	if (GetXboxCommonResourceType(pXboxResource) != X_D3DCOMMON_TYPE_TEXTURE) { // Allows breakpoint below
		// test-case : Burnout and Outrun 2006 hit this case (retrieving a surface instead of a texture)
		// TODO : Surfaces can be set in the texture stages, instead of textures - see preparations in CxbxConvertXboxSurfaceToHostTexture
		// We'll need to wrap the surface somehow before using it as a texture
		LOG_TEST_CASE("GetHostBaseTexture called on a non-texture object");
		return nullptr;
		// Note : We'd like to remove the above and do the following instead,
		// but we can't yet since that seems to cause a "CreateCubeTexture Failed!"
		// regression. The root cause for that seems to stem from the X_D3DRTYPE_SURFACE
		// handling in CreateHostResource.
		//assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_TEXTURE);
	}

	return (IDirect3DBaseTexture*)GetHostResource(pXboxResource, D3DUsage, iTextureStage);
}

#if 0 // unused
IDirect3DTexture *GetHostTexture(xbox::X_D3DResource *pXboxResource, int iTextureStage = 0)
{
	if (pXboxResource == xbox::zeroptr)
		return nullptr;

	return (IDirect3DTexture *)GetHostBaseTexture(pXboxResource, 0, iTextureStage);

	// TODO : Check for 1 face (and 2 dimensions)?
}
#endif

IDirect3DVolumeTexture *GetHostVolumeTexture(xbox::X_D3DResource *pXboxResource, int iTextureStage = 0)
{
	return (IDirect3DVolumeTexture *)GetHostBaseTexture(pXboxResource, 0, iTextureStage);

	// TODO : Check for 1 face (and 2 dimensions)?
}

#if 0 // unused
IDirect3DIndexBuffer *GetHostIndexBuffer(xbox::X_D3DResource *pXboxResource)
{
	if (pXboxResource == xbox::zeroptr)
		return nullptr;

	assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_INDEXBUFFER);

	return (IDirect3DIndexBuffer*)GetHostResource(pXboxResource);
}
#endif

void SetHostSurface(xbox::X_D3DResource *pXboxResource, IDirect3DSurface *pHostSurface, int iTextureStage)
{
	assert(pXboxResource != xbox::zeroptr);
	assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_SURFACE);

	SetHostResource(pXboxResource, pHostSurface, iTextureStage);
}

void SetHostTexture(xbox::X_D3DResource *pXboxResource, IDirect3DTexture *pHostTexture, int iTextureStage)
{
	assert(pXboxResource != xbox::zeroptr);
	assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_TEXTURE);

	SetHostResource(pXboxResource, pHostTexture, iTextureStage);
}

void SetHostCubeTexture(xbox::X_D3DResource *pXboxResource, IDirect3DCubeTexture *pHostCubeTexture, int iTextureStage)
{
	assert(pXboxResource != xbox::zeroptr);
	assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_TEXTURE);

	SetHostResource(pXboxResource, pHostCubeTexture, iTextureStage);
}

void SetHostVolumeTexture(xbox::X_D3DResource *pXboxResource, IDirect3DVolumeTexture *pHostVolumeTexture, int iTextureStage)
{
	assert(pXboxResource != xbox::zeroptr);
	assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_TEXTURE);

	SetHostResource(pXboxResource, pHostVolumeTexture, iTextureStage);
}

void SetHostVolume(xbox::X_D3DResource *pXboxResource, IDirect3DVolume *pHostVolume, int iTextureStage)
{
	assert(pXboxResource != xbox::zeroptr);
	assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_TEXTURE);

	// TODO: IDirect3DVolume9 is not a IDirect3DResource9!
	SetHostResource(pXboxResource, (IDirect3DResource*)pHostVolume, iTextureStage);
}

void SetHostIndexBuffer(xbox::X_D3DResource *pXboxResource, IDirect3DIndexBuffer *pHostIndexBuffer)
{
	assert(pXboxResource != xbox::zeroptr);
	assert(GetXboxCommonResourceType(pXboxResource) == X_D3DCOMMON_TYPE_INDEXBUFFER);

	SetHostResource(pXboxResource, pHostIndexBuffer);
}

int XboxD3DPaletteSizeToBytes(const xbox::X_D3DPALETTESIZE Size)
{
/*
	static int lk[4] =
	{
		256 * sizeof(D3DCOLOR),    // D3DPALETTE_256
		128 * sizeof(D3DCOLOR),    // D3DPALETTE_128
		64 * sizeof(D3DCOLOR),     // D3DPALETTE_64
		32 * sizeof(D3DCOLOR)      // D3DPALETTE_32
	};

	return lk[Size];
*/
	return (256 * sizeof(D3DCOLOR)) >> (unsigned)Size;
}

inline xbox::X_D3DPALETTESIZE GetXboxPaletteSize(const xbox::X_D3DPalette *pPalette)
{
	xbox::X_D3DPALETTESIZE PaletteSize = (xbox::X_D3DPALETTESIZE)
		((pPalette->Common & X_D3DPALETTE_COMMON_PALETTESIZE_MASK) >> X_D3DPALETTE_COMMON_PALETTESIZE_SHIFT);

	return PaletteSize;
}

int GetD3DResourceRefCount(IDirect3DResource *EmuResource)
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

/*
xbox::X_D3DSurface *EmuNewD3DSurface()
{
	xbox::X_D3DSurface *result = (xbox::X_D3DSurface *)xbox::ExAllocatePool(sizeof(xbox::X_D3DSurface));
	result->Common = X_D3DCOMMON_D3DCREATED | X_D3DCOMMON_TYPE_SURFACE | 1; // Set refcount to 1
	return result;
}
*/

unsigned int CxbxGetPixelContainerDepth
(
	xbox::X_D3DPixelContainer *pPixelContainer
)
{
	if (pPixelContainer->Size == 0) {
		DWORD l2d = (pPixelContainer->Format & X_D3DFORMAT_PSIZE_MASK) >> X_D3DFORMAT_PSIZE_SHIFT;
		return  1 << l2d;
	}

	return 1;
}

unsigned int CxbxGetPixelContainerMipMapLevels
(
	xbox::X_D3DPixelContainer *pPixelContainer
)
{
	if (pPixelContainer->Size == 0) {
		return (pPixelContainer->Format & X_D3DFORMAT_MIPMAP_MASK) >> X_D3DFORMAT_MIPMAP_SHIFT;
	}

	return 1;
}

uint32_t GetPixelContainerWidth(xbox::X_D3DPixelContainer *pPixelContainer)
{
	DWORD Size = pPixelContainer->Size;
	uint32_t Result;

	if (Size != 0) {
		Result = ((Size & X_D3DSIZE_WIDTH_MASK) /* >> X_D3DSIZE_WIDTH_SHIFT*/) + 1;
	}
	else {
		DWORD l2w = (pPixelContainer->Format & X_D3DFORMAT_USIZE_MASK) >> X_D3DFORMAT_USIZE_SHIFT;

		Result = 1 << l2w;
	}

	return Result;
}

uint32_t GetPixelContainerHeight(xbox::X_D3DPixelContainer *pPixelContainer)
{
	DWORD Size = pPixelContainer->Size;
	uint32_t Result;

	if (Size != 0) {
		Result = ((Size & X_D3DSIZE_HEIGHT_MASK) >> X_D3DSIZE_HEIGHT_SHIFT) + 1;
	}
	else {
		DWORD l2h = (pPixelContainer->Format & X_D3DFORMAT_VSIZE_MASK) >> X_D3DFORMAT_VSIZE_SHIFT;

		Result = 1 << l2h;
	}

	return Result;
}

void GetSurfaceFaceAndLevelWithinTexture(xbox::X_D3DSurface* pSurface, xbox::X_D3DBaseTexture* pTexture, UINT& Level, D3DCUBEMAP_FACES& Face)
{
    auto pSurfaceData = (uintptr_t)GetDataFromXboxResource(pSurface);
    auto pTextureData = (uintptr_t)GetDataFromXboxResource(pTexture);

    // Fast path: If the data pointers match, this must be the first surface within the texture
    if (pSurfaceData == pTextureData) {
        Level = 0;
        Face = D3DCUBEMAP_FACE_POSITIVE_X;
        return;
    }

    int numLevels = CxbxGetPixelContainerMipMapLevels(pTexture);
    int numFaces = pTexture->Format & X_D3DFORMAT_CUBEMAP ? 6 : 1;

    CxbxGetPixelContainerMipMapLevels(pTexture);

    // First, we need to fetch the dimensions of both the surface and the texture, for use within our calculations
    UINT textureWidth, textureHeight, textureDepth, textureRowPitch, textureSlicePitch;
    CxbxGetPixelContainerMeasures(pTexture, 0, &textureWidth, &textureHeight, &textureDepth, &textureRowPitch, &textureSlicePitch);

    UINT surfaceWidth, surfaceHeight, surfaceDepth, surfaceRowPitch, surfaceSlicePitch;
    CxbxGetPixelContainerMeasures(pSurface, 0, &surfaceWidth, &surfaceHeight, &surfaceDepth, &surfaceRowPitch, &surfaceSlicePitch);

    // Iterate through all faces and levels, until we find a matching pointer
    bool isCompressed = EmuXBFormatIsCompressed(GetXboxPixelContainerFormat(pTexture));
    int minSize = (isCompressed) ? 4 : 1;
    int cubeFaceOffset = 0; int cubeFaceSize = 0;
    auto pData = pTextureData;

    for (int face = D3DCUBEMAP_FACE_POSITIVE_X; face <= numFaces; face++) {
        int mipWidth = textureWidth;
        int mipHeight = textureHeight;
        int mipDepth = textureDepth;
        int mipRowPitch = textureRowPitch;
        int mipDataOffset = 0;

        for (int level = 0; level < numLevels; level++) {
            if (pData == pSurfaceData) {
                Level = level;
                Face = (D3DCUBEMAP_FACES)face;
                return;
            }

            // Calculate size of this mipmap level
            UINT dwMipSize = mipRowPitch * mipHeight;
            if (isCompressed) {
                dwMipSize /= 4;
            }

            // If this is the first face, set the cube face size
            if (face == D3DCUBEMAP_FACE_POSITIVE_X) {
                cubeFaceSize = ROUND_UP(textureDepth * dwMipSize, X_D3DTEXTURE_CUBEFACE_ALIGNMENT);
            }

            // Move to the next mip-map and calculate dimensions for the next iteration
            mipDataOffset += dwMipSize;

            if (mipWidth > minSize) {
                mipWidth /= 2;
                mipRowPitch /= 2;
            }

            if (mipHeight > minSize) {
                mipHeight /= 2;
            }

            if (mipDepth > 1) {
                mipDepth /= 2;
            }
        }

        // Move to the next face
        pData += cubeFaceSize;
    }

    LOG_TEST_CASE("Could not find Surface within Texture, falling back to Level = 0, Face = D3DCUBEMAP_FACE_POSITIVE_X");
    Level = 0;
    Face = D3DCUBEMAP_FACE_POSITIVE_X;
}

// Wrapper function to allow calling without passing a face
void GetSurfaceFaceAndLevelWithinTexture(xbox::X_D3DSurface* pSurface, xbox::X_D3DBaseTexture* pBaseTexture, UINT& Level)
{
    D3DCUBEMAP_FACES face;
    GetSurfaceFaceAndLevelWithinTexture(pSurface, pBaseTexture, Level, face);
}

extern void HLE_init_pgraph_plugins(); // implemented in XbPushBuffer.cpp

// Direct3D initialization (called before emulation begins)
void EmuD3DInit()
{

	HLE_init_pgraph_plugins(); // TODO : Hook more nv_dma_map() result uses in EmuNV2A_PGRAPH.cpp

	// Initialise CreateDevice Proxy Data struct
	{
		g_EmuCDPD.Adapter = g_XBVideo.adapter;
		g_EmuCDPD.DeviceType = (g_XBVideo.direct3DDevice == 0) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF;
	}

	// create Direct3D8 and retrieve caps
    {
        // xbox Direct3DCreate8 returns "1" always, so we need our own ptr
        if(FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &g_pDirect3D)))
            CxbxrAbort("Could not initialize Direct3D8!");

        g_pDirect3D->GetDeviceCaps(g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType, &g_D3DCaps);

		// Dump Host D3DCaps to log unconditionally
		std::cout << "----------------------------------------\n";
		std::cout << "Host D3DCaps : " << g_D3DCaps << "\n";
		std::cout << "----------------------------------------\n";
	}
}

// cleanup Direct3D
void EmuD3DCleanup() {}

// window message processing thread
static DWORD WINAPI EmuRenderWindow(LPVOID lpParam)
{
	CxbxSetThreadName("Cxbx Render Window");

    // register window class
    {
        LOGBRUSH logBrush = {BS_SOLID, RGB(0,0,0)};

        g_hBgBrush = CreateBrushIndirect(&logBrush);

        WNDCLASSEX wc =
        {
            sizeof(WNDCLASSEX),
            CS_CLASSDC,
            EmuMsgProc,
            0, 0, hActiveModule, // Was GetModuleHandle(nullptr),
			0, // TODO : LoadIcon(hmodule, ?)
            LoadCursor(NULL, IDC_ARROW),
            (HBRUSH)(g_hBgBrush), NULL,
            "CxbxRender",
			nullptr
        };

        RegisterClassEx(&wc);
    }

    // create the window
    {
		// Peform selection if running in GUI or kernel mode first.
		HWND hwndParent = (!CxbxKrnl_hEmuParent ? GetDesktopWindow() : CxbxKrnl_hEmuParent);
		DWORD dwStyle = WS_POPUP;
		RECT windowRect = { 0 };

		// Obtain the selected resolution from GUI or full desktop screen in kernel mode.
		if (!GetWindowRect(hwndParent, &windowRect)) {
			// Fall back resolution if failed
			windowRect = { 0, 0, 640, 480 };
		}

		// Then perform additional checks if not running in full screen.
		if (!g_XBVideo.bFullScreen) {

			// If running as kernel mode, force use the xbox's default resolution.
			if (!CxbxKrnl_hEmuParent) {
				// Xbox default resolution (standalone window is resizable by the way)
				windowRect.right = 640;
				windowRect.bottom = 480;
				dwStyle = WS_OVERLAPPEDWINDOW;
			}
			else {
				dwStyle = WS_CHILD;
			}
		}

        g_hEmuWindow = CreateWindow
        (
            "CxbxRender", "Cxbx-Reloaded",
            dwStyle, 
			windowRect.left,
			windowRect.top,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
            hwndParent, nullptr, hActiveModule, // Was GetModuleHandle(nullptr),
            nullptr
        );
    }

    ShowWindow(g_hEmuWindow, ((CxbxKrnl_hEmuParent == 0) || g_XBVideo.bFullScreen) ? SW_SHOWDEFAULT : SW_SHOWMAXIMIZED);
    UpdateWindow(g_hEmuWindow);

    if(!g_XBVideo.bFullScreen && (CxbxKrnl_hEmuParent != NULL))
    {
        SetFocus(CxbxKrnl_hEmuParent);
    }

    EmuLog(LOG_LEVEL::DEBUG, "Message-Pump thread is running.");

    SetFocus(g_hEmuWindow);

    SetEvent(*reinterpret_cast<PHANDLE>(lpParam));

    // message processing loop
    {
        MSG msg;
        BOOL bRet;
        while((bRet = GetMessage(&msg, NULL, 0U, 0U)) != FALSE)
        {
            if(bRet == -1)
            {
                CxbxrAbort("GetMessage failed!");
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        CxbxrAbort(nullptr);
    }

    return 0;
}

// simple helper function
void ToggleFauxFullscreen(HWND hWnd)
{
    if(g_XBVideo.bFullScreen) {
        return;
    }

    // Remember last known position and style before go into faux full screen mode.
    static RECT lRect = {};
    static LONG gwl_style = {};

    // Require to toggle before start process due to WM_SETFOCUS will get trigger earlier.
    g_bIsFauxFullscreen = !g_bIsFauxFullscreen;
    if (g_bIsFauxFullscreen) {
        GetWindowRect(hWnd, &lRect);
        gwl_style = GetWindowLong(hWnd, GWL_STYLE);
        SetWindowLong(hWnd, GWL_STYLE, WS_POPUP);
        // NOTE: Window style must be set before call SetParent.
        if (CxbxKrnl_hEmuParent) {
            LONG parent_style = gwl_style & ~WS_CHILD;
            parent_style |= WS_POPUP;
            SetWindowLong(hWnd, GWL_STYLE, gwl_style);
            SetParent(hWnd, NULL);
        }
        SetWindowPos(hWnd, HWND_TOPMOST, lRect.left, lRect.top, 0, 0, SWP_NOSIZE);
        ShowWindow(hWnd, SW_MAXIMIZE);
    }
    else {
        SetWindowLong(hWnd, GWL_STYLE, gwl_style);
        if(CxbxKrnl_hEmuParent) {
            // NOTE: This call makes sure that emulation rendering will reappear back into the main window after leaving "faux fullscreen" on non-primary displays.
            SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_HIDEWINDOW);

            // NOTE: Window style must be set before call SetParent.
            SetParent(hWnd, CxbxKrnl_hEmuParent);
            ShowWindow(hWnd, SW_MAXIMIZE);
            SetFocus(CxbxKrnl_hEmuParent);
        }
        else {
            ShowWindow(hWnd, SW_RESTORE);
            SetWindowPos(hWnd, HWND_NOTOPMOST, lRect.left, lRect.top, lRect.right - lRect.left, lRect.bottom - lRect.top, 0);
            SetFocus(hWnd);
        }
    }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// rendering window message procedure
static LRESULT WINAPI EmuMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	const LRESULT imguiResult = ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
	if (imguiResult != 0) return imguiResult;

    switch(msg)
    {
        case WM_DESTROY:
        {
            CxbxReleaseCursor();
            DeleteObject(g_hBgBrush);
            PostQuitMessage(0);
            return D3D_OK; // = 0
        }
        break;

		case WM_PAINT:
		{
			if (g_CxbxPrintUEM)
			{
				DrawUEM(hWnd);
			}
			else
			{
				DefWindowProc(hWnd, msg, wParam, lParam);
			}
		}
		break;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case ID_SYNC_CONFIG_LOGGING:
				{
					// Sync run-time config log settings from GUI process.
					log_sync_config();
					log_generate_active_filter_output(CXBXR_MODULE::CXBXR);
				}
				break;

				case ID_SYNC_CONFIG_INPUT:
				{
					SDL_Event UpdateInputEvent;
					SDL_memset(&UpdateInputEvent, 0, sizeof(SDL_Event));
					UpdateInputEvent.type = Sdl::UpdateInputEvent_t;
					UpdateInputEvent.user.data1 = new int(lParam);
					SDL_PushEvent(&UpdateInputEvent);
				}
				break;

				default:
					break;
			}
		}
		break;

		case WM_INPUT_DEVICE_CHANGE:
		{
			// sent by rawinput when it detects changes for the registered device types

			if (wParam == GIDC_ARRIVAL) {
				g_InputDeviceManager.HotplugHandler(false);
			}
			return DefWindowProc(hWnd, msg, wParam, lParam);
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
            // NOTE: Windows does not send F10 key message to WM_KEYDOWN.
            // Source: https://docs.microsoft.com/en-us/windows/desktop/inputdev/wm-syskeydown
            else if(wParam == VK_F10)
            {
                ToggleFauxFullscreen(hWnd);
            }
            else
            {
                return DefWindowProc(hWnd, msg, wParam, lParam);
            }
        }
        break;

        case WM_KEYDOWN:
        {
            /*! disable fullscreen if we are set to faux mode, and faux fullscreen is active */
            if(wParam == VK_ESCAPE)
            {
                if(g_XBVideo.bFullScreen)
                {
                    SendMessage(hWnd, WM_CLOSE, 0, 0);
                }
                else if(g_bIsFauxFullscreen)
                {
                    ToggleFauxFullscreen(hWnd);
                }
            }
            else if (wParam == VK_F1)
            {
				g_renderbase->ToggleImGui();
                CxbxUpdateCursor();
            }
			else if (wParam == VK_F2)
			{
				g_UseFixedFunctionPixelShader = !g_UseFixedFunctionPixelShader;
			}
            else if (wParam == VK_F3)
            {
                g_bClipCursor = !g_bClipCursor;
                g_EmuShared->SetClipCursorFlag(g_bClipCursor);

                if (g_bClipCursor) {
                    CxbxClipCursor(hWnd);
                }
                else {
                    CxbxReleaseCursor();
                }
            }
            else if (wParam == VK_F6)
            {
                // For some unknown reason, F6 isn't handled in WndMain::WndProc
                // sometimes, so detect it and stop emulation from here too :
                SendMessage(hWnd, WM_CLOSE, 0, 0); // See StopEmulation();
            }
			else if (wParam == VK_F7)
			{
				g_bUsePassthroughHLSL = !g_bUsePassthroughHLSL;
			}
            else if(wParam == VK_F8)
            {
                g_bPrintfOn = !g_bPrintfOn;
                g_EmuShared->SetIsKrnlLogEnabled(g_bPrintfOn);
                LOG_THREAD_INIT;
                std::cout << _logThreadPrefix << g_EnumModules2String[to_underlying(CXBXR_MODULE::CXBXR)] << "Enable log is " << g_bPrintfOn << std::endl;
                ipc_send_gui_update(IPC_UPDATE_GUI::LOG_ENABLED, static_cast<UINT>(g_bPrintfOn));
            }
			else if (wParam == VK_F9)
			{
				// Toggle frame-limiting
				g_bHack_UnlockFramerate = !g_bHack_UnlockFramerate;
			}
			else if (wParam == VK_F11)
			{
				if (g_iWireframe++ == 2) {
					g_iWireframe = 0;
				}

				XboxRenderStates.SetWireFrameMode(g_iWireframe);
				NV2ARenderStates.SetWireFrameMode(g_iWireframe);
			}
            else
            {
                return DefWindowProc(hWnd, msg, wParam, lParam);
            }
        }
        break;

        case WM_SIZE:
        {
            switch(wParam)
            {
                case SIZE_MINIMIZED:
                {
                    if(g_XBVideo.bFullScreen)
                        CxbxrAbort(nullptr);
                }
                break;
            }

            if (g_bClipCursor) {
                CxbxClipCursor(hWnd);
            }
        }
        break;

        case WM_MOVE:
        {
            if (g_bClipCursor) {
                CxbxClipCursor(hWnd);
            }
        }
        break;

        case WM_MOUSELEAVE:
        {
            DInput::mo_leave_wnd = true;
            g_bIsTrackingMoLeave = false;
            g_bIsTrackingMoMove = true;
            CxbxUpdateCursor(true);
        }
        break;

        case WM_MOUSEMOVE:
        {
            if (g_bClipCursor) {
                CxbxClipCursor(hWnd);
            }

            if (!g_bIsTrackingMoLeave) {
                TRACKMOUSEEVENT tme;
                tme.cbSize = sizeof(TRACKMOUSEEVENT);
                tme.hwndTrack = hWnd;
                tme.dwFlags = TME_LEAVE;
                TrackMouseEvent(&tme);
                g_bIsTrackingMoLeave = true;
                CxbxUpdateCursor();

                if (g_bIsTrackingMoMove) {
                    DInput::mo_leave_wnd = false;
                    g_bIsTrackingMoMove = false;
                }
            }
        }
        break;

        case WM_CLOSE:
            CxbxReleaseCursor();
            DestroyWindow(hWnd);
            CxbxrShutDown();
            break;

        case WM_SETFOCUS:
        {
            if(CxbxKrnl_hEmuParent && !g_XBVideo.bFullScreen && !g_bIsFauxFullscreen)
            {
                SetFocus(CxbxKrnl_hEmuParent);
            }
        }
        break;

        case WM_SETCURSOR:
        {
            if(g_XBVideo.bFullScreen || g_bIsFauxFullscreen)
            {
                SetCursor(NULL);
                return S_OK; // = Is not part of D3D8 handling.
            }
            return DefWindowProc(hWnd, msg, wParam, lParam);
        }
        break;

		case WM_CXBXR_RUN_ON_MESSAGE_THREAD:
		{
			auto func = reinterpret_cast<const std::function<void()>*>(wParam);
			(*func)();
			return 0;
		}

        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return S_OK; // = Is not part of D3D8 handling.
}

std::chrono::steady_clock::time_point GetNextVBlankTime()
{
	using namespace std::chrono;
	// TODO: Read display frequency from Xbox Display Adapter
	// This is accessed by calling CMiniport::GetRefreshRate(); 
	// This reads from the structure located at CMinpPort::m_CurrentAvInfo
	// This will require at least Direct3D_CreateDevice being unpatched
	// otherwise, m_CurrentAvInfo will never be initialised!
	// 20ms should be used in the case of 50hz
	auto ms = 16.6666666667ms;
	return steady_clock::now() + duration_cast<steady_clock::duration>(ms);
}

// timing thread procedure
static xbox::void_xt NTAPI EmuUpdateTickCount(xbox::PVOID Arg)
{
	CxbxSetThreadName("Cxbx Timing Thread");

    EmuLog(LOG_LEVEL::DEBUG, "Timing thread is running.");

	auto nextVBlankTime = GetNextVBlankTime();

    while(true)
    {
		// Wait for VBlank
        // Note: This whole code block can be removed once NV2A interrupts are implemented
		// And Both Swap and Present can be ran unpatched
		// Once that is in place, MiniPort + Direct3D will handle this on it's own!
		SleepPrecise(nextVBlankTime);
		nextVBlankTime = GetNextVBlankTime();

		// Increment the VBlank Counter and Wake all threads there were waiting for the VBlank to occur
		std::unique_lock<std::mutex> lk(g_VBConditionMutex);
		g_Xbox_VBlankData.VBlank++;
		g_VBConditionVariable.notify_all();

		// TODO: Fixme.  This may not be right...
		g_Xbox_SwapData.SwapVBlank = 1;

		if(g_pXbox_VerticalBlankCallback != xbox::zeroptr)
		{
			g_pXbox_VerticalBlankCallback(&g_Xbox_VBlankData);
		}

		g_Xbox_VBlankData.Swap = 0;

		// TODO: This can't be accurate...
		g_Xbox_SwapData.TimeUntilSwapVBlank = 0;

		// TODO: Recalculate this for PAL version if necessary.
		// Also, we should check the D3DPRESENT_INTERVAL value for accurracy.
	//	g_Xbox_SwapData.TimeBetweenSwapVBlanks = 1/60;
		g_Xbox_SwapData.TimeBetweenSwapVBlanks = 0;
	}
}

void UpdateDepthStencilFlags(IDirect3DSurface *pDepthStencilSurface)
{
	g_bHasDepth = false;
	g_bHasStencil = false;
	if (pDepthStencilSurface != nullptr) {
		D3DSURFACE_DESC Desc;
		pDepthStencilSurface->GetDesc(&Desc);

		switch (Desc.Format) {
		case D3DFMT_D16:
			g_bHasDepth = true;
			break;
		case D3DFMT_D15S1:
			g_bHasDepth = true;
			g_bHasStencil = true;
			break;
		case D3DFMT_D24X8:
			g_bHasDepth = true;
			break;
		case D3DFMT_D24S8:
			g_bHasDepth = true;
			g_bHasStencil = true;
			break;
		case D3DFMT_D24X4S4:
			g_bHasDepth = true;
			g_bHasStencil = true;
			break;
		case D3DFMT_D32:
			g_bHasDepth = true;
			break;
		default:
			g_bHasDepth = true;
			g_bHasStencil = true;
			break;
		}
	}
}

static void SetupPresentationParameters
(
    const xbox::X_D3DPRESENT_PARAMETERS     *pXboxPresentationParameters
)
{
    D3DPRESENT_PARAMETERS& params = g_EmuCDPD.HostPresentationParameters;

    params.Windowed = !g_XBVideo.bFullScreen;

    // TODO: Investigate the best option for this
    params.SwapEffect = D3DSWAPEFFECT_COPY;

    // Any backbuffer format should do, since we render to a separate xbox backbuffer
    // We need to specify something to support fullscreen exclusive mode
    // Take the current displaymode format
    D3DDISPLAYMODE D3DDisplayMode;
    g_pDirect3D->GetAdapterDisplayMode(g_EmuCDPD.Adapter, &D3DDisplayMode);
    params.BackBufferFormat = D3DDisplayMode.Format;

    params.PresentationInterval = g_XBVideo.bVSync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
    g_Xbox_PresentationInterval_Default = pXboxPresentationParameters->PresentationInterval;

    // We only want *one* backbuffer on the host, triple buffering, etc should be handled by our Present/Swap impl
    params.BackBufferCount = 1;

    // We don't want multisampling on the host backbuffer, it should be applied to Xbox surfaces if required
    params.MultiSampleType = D3DMULTISAMPLE_NONE;
    params.MultiSampleQuality = 0;

    // We want a lockable backbuffer for swapping/blitting purposes
    params.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

    // retrieve resolution from configuration
    char szBackBufferFormat[16] = {};
    const char* resolution = g_XBVideo.szVideoResolution;
    if (4 != sscanf(resolution, "%u x %u %*dbit %s (%u hz)",
        &params.BackBufferWidth,
        &params.BackBufferHeight,
        szBackBufferFormat,
        &params.FullScreen_RefreshRateInHz)) {
        EmuLog(LOG_LEVEL::DEBUG, "EmuCreateDeviceProxy: Couldn't parse resolution : %s. Using Xbox Default (%d, %d @ %uhz)", resolution,
            pXboxPresentationParameters->BackBufferWidth, pXboxPresentationParameters->BackBufferHeight,
            pXboxPresentationParameters->FullScreen_RefreshRateInHz);
        params.BackBufferWidth = pXboxPresentationParameters->BackBufferWidth;
        params.BackBufferHeight = pXboxPresentationParameters->BackBufferHeight;
        params.FullScreen_RefreshRateInHz = pXboxPresentationParameters->FullScreen_RefreshRateInHz;
    }

    if (params.Windowed) {
        // Refresh rate must be 0 in windowed mode, as is documented
        params.FullScreen_RefreshRateInHz = 0;
    }
}

static void DetermineSupportedD3DFormats
(
)
{
    memset(g_bSupportsFormatSurface, false, sizeof(g_bSupportsFormatSurface));
    memset(g_bSupportsFormatSurfaceRenderTarget, false, sizeof(g_bSupportsFormatSurfaceRenderTarget));
    memset(g_bSupportsFormatSurfaceDepthStencil, false, sizeof(g_bSupportsFormatSurfaceDepthStencil));
    memset(g_bSupportsFormatTexture, false, sizeof(g_bSupportsFormatTexture));
    memset(g_bSupportsFormatTextureRenderTarget, false, sizeof(g_bSupportsFormatTextureRenderTarget));
    memset(g_bSupportsFormatTextureDepthStencil, false, sizeof(g_bSupportsFormatTextureDepthStencil));
    memset(g_bSupportsFormatVolumeTexture, false, sizeof(g_bSupportsFormatVolumeTexture));
    memset(g_bSupportsFormatCubeTexture, false, sizeof(g_bSupportsFormatCubeTexture));
    for (int X_Format = xbox::X_D3DFMT_L8; X_Format <= xbox::X_D3DFMT_LIN_R8G8B8A8; X_Format++) {
        // Only process Xbox formats that are directly mappable to host
        if (!EmuXBFormatRequiresConversionToARGB((xbox::X_D3DFORMAT)X_Format)) {
            // Convert the Xbox format into host format (without warning, thanks to the above restriction)
            const D3DFORMAT PCFormat = EmuXB2PC_D3DFormat((xbox::X_D3DFORMAT)X_Format);
            if (PCFormat != D3DFMT_UNKNOWN) {
                // Index with Xbox D3DFormat, because host FourCC codes are too big to be used as indices
                g_bSupportsFormatSurface[X_Format] = SUCCEEDED(g_pDirect3D->CheckDeviceFormat(
                        g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
                        g_EmuCDPD.HostPresentationParameters.BackBufferFormat, 0,
                        D3DRTYPE_SURFACE, PCFormat));

                g_bSupportsFormatSurfaceRenderTarget[X_Format] = SUCCEEDED(g_pDirect3D->CheckDeviceFormat(
                        g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
                        g_EmuCDPD.HostPresentationParameters.BackBufferFormat, D3DUSAGE_RENDERTARGET,
                        D3DRTYPE_SURFACE, PCFormat));

                g_bSupportsFormatSurfaceDepthStencil[X_Format] = SUCCEEDED(g_pDirect3D->CheckDeviceFormat(
                        g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
                        g_EmuCDPD.HostPresentationParameters.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
                        D3DRTYPE_SURFACE, PCFormat));

                g_bSupportsFormatTexture[X_Format] = SUCCEEDED(g_pDirect3D->CheckDeviceFormat(
                        g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
                        g_EmuCDPD.HostPresentationParameters.BackBufferFormat, 0,
                        D3DRTYPE_TEXTURE, PCFormat));

                g_bSupportsFormatTextureRenderTarget[X_Format] = SUCCEEDED(g_pDirect3D->CheckDeviceFormat(
                        g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
                        g_EmuCDPD.HostPresentationParameters.BackBufferFormat, D3DUSAGE_RENDERTARGET,
                        D3DRTYPE_TEXTURE, PCFormat));

                g_bSupportsFormatTextureDepthStencil[X_Format] = SUCCEEDED(g_pDirect3D->CheckDeviceFormat(
                        g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
                        g_EmuCDPD.HostPresentationParameters.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
                        D3DRTYPE_TEXTURE, PCFormat));

                g_bSupportsFormatVolumeTexture[X_Format] = SUCCEEDED(g_pDirect3D->CheckDeviceFormat(
                        g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
                        g_EmuCDPD.HostPresentationParameters.BackBufferFormat, 0,
                        D3DRTYPE_VOLUMETEXTURE, PCFormat));

                g_bSupportsFormatCubeTexture[X_Format] = SUCCEEDED(g_pDirect3D->CheckDeviceFormat(
                        g_EmuCDPD.Adapter, g_EmuCDPD.DeviceType,
                        g_EmuCDPD.HostPresentationParameters.BackBufferFormat, 0,
                        D3DRTYPE_CUBETEXTURE, PCFormat));
            }
        }
    }
}

static void DrawInitialBlackScreen
(
)
{
    LOG_INIT;

    // initially, show a black screen
    // Only clear depth buffer and stencil if present
    //
    // Avoids following DirectX Debug Runtime error report
    //    [424] Direct3D8: (ERROR) :Invalid flag D3DCLEAR_ZBUFFER: no zbuffer is associated with device. Clear failed. 
    //
    HRESULT hRet = g_pD3DDevice->Clear(
        /*Count=*/0, 
        /*pRects=*/nullptr, 
        D3DCLEAR_TARGET | (g_bHasDepth ? D3DCLEAR_ZBUFFER : 0) | (g_bHasStencil ? D3DCLEAR_STENCIL : 0),
        /*Color=*/0xFF000000, // TODO : Use constant for this
        /*Z=*/g_bHasDepth ? 1.0f : 0.0f,
        /*Stencil=*/0);
    DEBUG_D3DRESULT(hRet, "g_pD3DDevice->Clear");

    hRet = g_pD3DDevice->BeginScene();
    DEBUG_D3DRESULT(hRet, "g_pD3DDevice->BeginScene");

    hRet = g_pD3DDevice->EndScene();
    DEBUG_D3DRESULT(hRet, "g_pD3DDevice->EndScene");

    hRet = g_pD3DDevice->Present(0, 0, 0, 0);
    DEBUG_D3DRESULT(hRet, "g_pD3DDevice->Present");

    // begin scene
    hRet = g_pD3DDevice->BeginScene();
    DEBUG_D3DRESULT(hRet, "g_pD3DDevice->BeginScene(2nd)");
}

static void CreateDefaultD3D9Device
(
    const xbox::X_D3DPRESENT_PARAMETERS     *pPresentationParameters
)
{
    LOG_INIT;

    // only one device should be created at once
    if (g_pD3DDevice != nullptr) {
        EmuLog(LOG_LEVEL::DEBUG, "CreateDefaultD3D9Device releasing old Device.");

        g_pD3DDevice->EndScene();

        ClearResourceCache(g_Cxbx_Cached_PaletizedTextures);
        ClearResourceCache(g_Cxbx_Cached_Direct3DResources);

        // TODO: ensure all other resources are cleaned up too

        // Final release of IDirect3DDevice9 must be called from the window message thread
        // See https://docs.microsoft.com/en-us/windows/win32/direct3d9/multithreading-issues
        RunOnWndMsgThread([] {
            // We only need to call bundled device release once here.
            g_renderbase->DeviceRelease();
        });
    }

    // Apply render scale factor for high-resolution rendering
    g_RenderUpscaleFactor = g_XBVideo.renderScaleFactor;

    // Setup the HostPresentationParameters
    SetupPresentationParameters(pPresentationParameters);

    // detect vertex processing capabilities
    DWORD BehaviorFlags;
    if((g_D3DCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) && g_EmuCDPD.DeviceType == D3DDEVTYPE_HAL)
    {
        EmuLog(LOG_LEVEL::DEBUG, "Using hardware vertex processing");

        BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    }
    else
    {
        EmuLog(LOG_LEVEL::DEBUG, "Using software vertex processing");

        BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }

    // Dxbx addition : Prevent Direct3D from changing the FPU Control word :
    BehaviorFlags |= D3DCREATE_FPU_PRESERVE;

    // Direct3D8: (WARN) :Device that was created without D3DCREATE_MULTITHREADED is being used by a thread other than the creation thread.
    BehaviorFlags |= D3DCREATE_MULTITHREADED;

    // We never want auto-depth stencil on the host, Xbox D3D will handle this for us
    g_EmuCDPD.HostPresentationParameters.EnableAutoDepthStencil = FALSE;

	D3DDISPLAYMODEEX displayMode { sizeof(displayMode) };
	{
		const auto& presentParameters = g_EmuCDPD.HostPresentationParameters;
		displayMode.Width = presentParameters.BackBufferWidth;
		displayMode.Height = presentParameters.BackBufferHeight;
		displayMode.RefreshRate = presentParameters.FullScreen_RefreshRateInHz;
		displayMode.Format = presentParameters.BackBufferFormat;
		displayMode.ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;
	}

    // IDirect3D9::CreateDevice must be called from the window message thread
    // See https://docs.microsoft.com/en-us/windows/win32/direct3d9/multithreading-issues
    HRESULT hr;
    RunOnWndMsgThread([&hr, BehaviorFlags, &displayMode] {
        hr = g_pDirect3D->CreateDeviceEx(
            g_EmuCDPD.Adapter,
            g_EmuCDPD.DeviceType,
            g_hEmuWindow,
            BehaviorFlags,
            &g_EmuCDPD.HostPresentationParameters,
			g_EmuCDPD.HostPresentationParameters.Windowed ? nullptr : &displayMode,
            &g_pD3DDevice);
    });
    DEBUG_D3DRESULT(hr, "IDirect3D::CreateDeviceEx");

    if(FAILED(hr))
        CxbxrAbort("IDirect3D::CreateDeviceEx failed");

    // Which texture formats does this device support?
    DetermineSupportedD3DFormats();

    // Can host driver create event queries?
    if (SUCCEEDED(g_pD3DDevice->CreateQuery(D3DQUERYTYPE_EVENT, nullptr))) {
        // Is host GPU query creation enabled?
        if (!g_bHack_DisableHostGPUQueries) {
            // Create a D3D event query to handle "wait-for-idle" with
            hr = g_pD3DDevice->CreateQuery(D3DQUERYTYPE_EVENT, &g_pHostQueryWaitForIdle);
            DEBUG_D3DRESULT(hr, "g_pD3DDevice->CreateQuery (wait for idle)");

            // Create a D3D event query to handle "callback events" with
            hr = g_pD3DDevice->CreateQuery(D3DQUERYTYPE_EVENT, &g_pHostQueryCallbackEvent);
            DEBUG_D3DRESULT(hr, "g_pD3DDevice->CreateQuery (callback event)");
        }
    } else {
        LOG_TEST_CASE("Can't CreateQuery(D3DQUERYTYPE_EVENT) on host!");
    }

    // Can host driver create occlusion queries?
    g_bEnableHostQueryVisibilityTest = false;
    if (SUCCEEDED(g_pD3DDevice->CreateQuery(D3DQUERYTYPE_OCCLUSION, nullptr))) {
        // Is host GPU query creation enabled?
        if (!g_bHack_DisableHostGPUQueries) {
            g_bEnableHostQueryVisibilityTest = true;
        } else {
            LOG_TEST_CASE("Disabled D3DQUERYTYPE_OCCLUSION on host!");
        }
    } else {
        LOG_TEST_CASE("Can't CreateQuery(D3DQUERYTYPE_OCCLUSION) on host!");
    }

    DrawInitialBlackScreen();

    // Set up cache
    g_VertexShaderSource.ResetD3DDevice(g_pD3DDevice);

    // Set up ImGui's render backend
    ImGui_ImplDX9_Init(g_pD3DDevice);
    g_renderbase->SetDeviceRelease([] {
        ImGui_ImplDX9_Shutdown();
        g_pD3DDevice->Release();
    });
}


// check if a resource has been registered yet (if not, register it)
void CreateHostResource(xbox::X_D3DResource *pResource, DWORD D3DUsage, int iTextureStage, DWORD dwSize); // Forward declartion to prevent restructure of code
static void EmuVerifyResourceIsRegistered(xbox::X_D3DResource *pResource, DWORD D3DUsage, int iTextureStage, DWORD dwSize)
{
	// Skip resources without data
	if (pResource->Data == xbox::zero)
		return;

	auto key = GetHostResourceKey(pResource, iTextureStage);
	auto& ResourceCache = GetResourceCache(key);
	auto it = ResourceCache.find(key);
	if (it != ResourceCache.end()) {
		if (D3DUsage == D3DUSAGE_RENDERTARGET && IsResourceAPixelContainer(pResource) && EmuXBFormatIsRenderTarget(GetXboxPixelContainerFormat((xbox::X_D3DPixelContainer*)pResource))) {
            // Render targets have special behavior: We can't trash them on guest modification
            // this fixes an issue where CubeMaps were broken because the surface Set in GetCubeMapSurface
            // would be overwritten by the surface created in SetRenderTarget
            // However, if a non-rendertarget surface is used here, we'll need to recreate it as a render target!
            auto hostResource = it->second.pHostResource.Get();
            auto xboxSurface = (xbox::X_D3DSurface*)pResource;
            auto xboxTexture = (xbox::X_D3DTexture*)pResource;
            auto xboxResourceType = GetXboxD3DResourceType(pResource);

            // Determine if the associated host resource is a render target already, if so, do nothing
            HRESULT hRet = STATUS_INVALID_PARAMETER; // Default to an error condition, so we can use D3D_OK to check for success
            D3DSURFACE_DESC surfaceDesc;
            if (xboxResourceType == xbox::X_D3DRTYPE_SURFACE) {
                hRet = ((IDirect3DSurface*)hostResource)->GetDesc(&surfaceDesc);
            } else if (xboxResourceType == xbox::X_D3DRTYPE_TEXTURE) {
                hRet = ((IDirect3DTexture*)hostResource)->GetLevelDesc(0, &surfaceDesc);
            }

            // Only continue checking if we were able to get the surface desc, if it failed, we fall-through
            // to previous resource management behavior
            if (SUCCEEDED(hRet)) {
                // If this resource is already created as a render target on the host, simply return
                if (surfaceDesc.Usage & D3DUSAGE_RENDERTARGET) {
                    return;
                }

                // The host resource is not a render target, but the Xbox surface is
                // We need to re-create it as a render target
                switch (xboxResourceType) {
                    case xbox::X_D3DRTYPE_SURFACE: {
                        // Free the host surface
                        FreeHostResource(key);

                        // Free the parent texture, if present
                        xbox::X_D3DTexture* pParentXboxTexture = (pResource) ? (xbox::X_D3DTexture*)xboxSurface->Parent : xbox::zeroptr;

                        if (pParentXboxTexture) {
                            // Re-create the texture with D3DUSAGE_RENDERTARGET, this will automatically create any child-surfaces
                            FreeHostResource(GetHostResourceKey(pParentXboxTexture, iTextureStage));
                            CreateHostResource(pParentXboxTexture, D3DUsage, iTextureStage, dwSize);
                        }

                        // Re-create the surface with D3DUSAGE_RENDERTARGET
                        CreateHostResource(pResource, D3DUsage, iTextureStage, dwSize);
                    } break;
                    case xbox::X_D3DRTYPE_TEXTURE: {
                        auto xboxTexture = (xbox::X_D3DTexture*)pResource;

                        // Free the host texture
                        FreeHostResource(key);

                        // And re-create the texture with D3DUSAGE_RENDERTARGET
                        CreateHostResource(pResource, D3DUsage, iTextureStage, dwSize);
                    } break;
                    default:
                        LOG_TEST_CASE("Unimplemented rendertarget type");
                }

                return;
            }
		}

		if (!HostResourceRequiresUpdate(key, pResource, dwSize)) {
			return;
		}

		FreeHostResource(key);
	} else {
		resource_info_t newResource;
		ResourceCache[key] = newResource;
	}

	CreateHostResource(pResource, D3DUsage, iTextureStage, dwSize);
}

// TODO : Move to own file
constexpr UINT QuadListToTriangleListVertexCount(UINT NrOfQuadVertices)
{
	return (NrOfQuadVertices * VERTICES_PER_TRIANGLE * TRIANGLES_PER_QUAD) / VERTICES_PER_QUAD;
}

// TODO : Move to own file (or argument of all users)
bool bUseClockWiseWindingOrder = true; // TODO : Should this be fetched from X_D3DRS_FRONTFACE (or X_D3DRS_CULLMODE)?

// TODO : Move to own file
// This function convertes quad to triangle indices.
// When pXboxQuadIndexData is set, original quad indices are read from this buffer
// (this use-case is for when an indexed quad draw is to be emulated).
// When pXboxQuadIndexData is null, quad-emulating indices are generated
// (this use-case is for when a non-indexed quad draw is to be emulated).
// The number of indices to generate is specified through uNrOfTriangleIndices.
// Resulting triangle indices are written to pTriangleIndexData, which must
// be pre-allocated to fit the output data.
// (Note, this function is marked 'constexpr' to allow the compiler to optimize
// the case when pXboxQuadIndexData is null)
constexpr void CxbxConvertQuadListToTriangleListIndices(
	INDEX16* pXboxQuadIndexData,
	unsigned uNrOfTriangleIndices,
	INDEX16* pTriangleIndexData)
{
	assert(uNrOfTriangleIndices > 0);
	assert(pTriangleIndexData);

	unsigned i = 0;
	unsigned j = 0;
	while (i + (VERTICES_PER_TRIANGLE * TRIANGLES_PER_QUAD) <= uNrOfTriangleIndices) {
		if (bUseClockWiseWindingOrder) {
			// ABCD becomes ABC+CDA, so this is triangle 1 :
			pTriangleIndexData[i + 0] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 0] : j + 0; // A
			pTriangleIndexData[i + 1] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 1] : j + 1; // B
			pTriangleIndexData[i + 2] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 2] : j + 2; // C
			i += VERTICES_PER_TRIANGLE;
			// And this is triangle 2 :
			pTriangleIndexData[i + 0] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 2] : j + 2; // C
			pTriangleIndexData[i + 1] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 3] : j + 3; // D
			pTriangleIndexData[i + 2] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 0] : j + 0; // A
			i += VERTICES_PER_TRIANGLE;
		} else {
			// ABCD becomes ADC+CBA, so this is triangle 1 :
			pTriangleIndexData[i + 0] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 0] : j + 0; // A
			pTriangleIndexData[i + 1] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 3] : j + 3; // D
			pTriangleIndexData[i + 2] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 2] : j + 2; // C
			i += VERTICES_PER_TRIANGLE;
			// And this is triangle 2 :
			pTriangleIndexData[i + 0] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 2] : j + 2; // C
			pTriangleIndexData[i + 1] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 1] : j + 1; // B
			pTriangleIndexData[i + 2] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 0] : j + 0; // A
			i += VERTICES_PER_TRIANGLE;
		}

		// Next quad, please :
		j += VERTICES_PER_QUAD;
	}
}
// caller must make sure it allocates space enough for even numbers of quads.
constexpr void CxbxConvertQuadStripToTriangleListIndices(
	INDEX16* pXboxQuadIndexData,
	unsigned uNrOfTriangleIndices,
	INDEX16* pTriangleIndexData)
{
	assert(uNrOfTriangleIndices > 0);
	assert(pTriangleIndexData);

	unsigned i = 0;
	unsigned j = 0;
	while (i + (VERTICES_PER_TRIANGLE * TRIANGLES_PER_QUAD) <= uNrOfTriangleIndices) {
		if (bUseClockWiseWindingOrder) {
			// ABCD becomes ABC+CDA, so this is triangle 1 :
			pTriangleIndexData[i + 0] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 0] : j + 0; // A
			pTriangleIndexData[i + 1] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 1] : j + 1; // B
			pTriangleIndexData[i + 2] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 2] : j + 2; // C
			i += VERTICES_PER_TRIANGLE;
			// And this is triangle 2 :
			pTriangleIndexData[i + 0] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 2] : j + 2; // C
			pTriangleIndexData[i + 1] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 3] : j + 3; // D
			pTriangleIndexData[i + 2] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 0] : j + 0; // A
			i += VERTICES_PER_TRIANGLE;
			// CDEF becomes CED+CFE, so this is triangle 3 :
			pTriangleIndexData[i + 0] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 0] : j + 2; // C
			pTriangleIndexData[i + 1] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 1] : j + 4; // E
			pTriangleIndexData[i + 2] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 2] : j + 3; // D
			i += VERTICES_PER_TRIANGLE;
			// And this is triangle 4 :
			pTriangleIndexData[i + 0] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 2] : j + 2; // C
			pTriangleIndexData[i + 1] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 3] : j + 5; // F
			pTriangleIndexData[i + 2] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 0] : j + 4; // E
			i += VERTICES_PER_TRIANGLE;
		}
		else {
			// ABCD becomes ADC+CBA, so this is triangle 1 :
			pTriangleIndexData[i + 0] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 0] : j + 0; // A
			pTriangleIndexData[i + 1] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 3] : j + 3; // D
			pTriangleIndexData[i + 2] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 2] : j + 2; // C
			i += VERTICES_PER_TRIANGLE;
			// And this is triangle 2 :
			pTriangleIndexData[i + 0] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 2] : j + 2; // C
			pTriangleIndexData[i + 1] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 1] : j + 1; // B
			pTriangleIndexData[i + 2] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 0] : j + 0; // A
			i += VERTICES_PER_TRIANGLE;
			// CDEF becomes CDE+CEF, so this is triangle 3 :
			pTriangleIndexData[i + 0] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 0] : j + 2; // C
			pTriangleIndexData[i + 1] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 1] : j + 3; // D
			pTriangleIndexData[i + 2] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 2] : j + 4; // E
			i += VERTICES_PER_TRIANGLE;
			// And this is triangle 4 :
			pTriangleIndexData[i + 0] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 2] : j + 2; // C
			pTriangleIndexData[i + 1] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 3] : j + 4; // E
			pTriangleIndexData[i + 2] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 0] : j + 5; // F
			i += VERTICES_PER_TRIANGLE;
		}

		// Next quad, please :
		j += VERTICES_PER_QUAD;
	}
}

// TODO : Move to own file
// Called from EMUPATCH(D3DDevice_DrawIndexedVerticesUP) when PrimitiveType == X_D3DPT_QUADLIST.
// This API receives the number of vertices to draw (VertexCount), the index data that references
// vertices and a single stream of vertex data. The number of vertices to draw indicates the number
// of indices that are going to be fetched. The vertex data is referenced up to the highest index
// number present in the index data.
// To emulate drawing indexed quads, g_pD3DDevice->DrawIndexedPrimitiveUP is called on host,
// whereby the quad indices are converted to triangle indices. This implies for every four
// quad indices, we have to generate (two times three is) six triangle indices. (Note, that
// vertex data undergoes it's own Xbox-to-host conversion, independent from these indices.)
INDEX16* CxbxCreateQuadListToTriangleListIndexData(INDEX16* pXboxQuadIndexData, unsigned QuadVertexCount)
{
	UINT NrOfTriangleIndices = QuadListToTriangleListVertexCount(QuadVertexCount);
	INDEX16* pQuadToTriangleIndexBuffer = (INDEX16*)malloc(NrOfTriangleIndices * sizeof(INDEX16));
	CxbxConvertQuadListToTriangleListIndices(pXboxQuadIndexData, NrOfTriangleIndices, pQuadToTriangleIndexBuffer);
	return pQuadToTriangleIndexBuffer;
}

// TODO : Move to own file
void CxbxReleaseQuadListToTriangleListIndexData(void* pHostIndexData)
{
	free(pHostIndexData);
}

class ConvertedIndexBuffer {
public:
	uint64_t Hash = 0;
	DWORD IndexCount = 0;
	IDirect3DIndexBuffer* pHostIndexBuffer = nullptr;
	INDEX16 LowIndex = 0;
	INDEX16 HighIndex = 0;

	~ConvertedIndexBuffer()
	{
		if (pHostIndexBuffer != nullptr) {
			pHostIndexBuffer->Release();
		}
	}
};

	std::unordered_map<uint32_t, ConvertedIndexBuffer> g_IndexBufferCache;

void CxbxRemoveIndexBuffer(PWORD pData)
{
	// HACK: Never Free
}

IDirect3DIndexBuffer* CxbxCreateIndexBuffer(unsigned IndexCount)
{
	LOG_INIT; // Allows use of DEBUG_D3DRESULT

	IDirect3DIndexBuffer* Result = nullptr;

	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb147168(v=vs.85).aspx
	// "Managing Resources (Direct3D 9)"
	// suggests "for resources which change with high frequency" [...]
	// "D3DPOOL_DEFAULT along with D3DUSAGE_DYNAMIC should be used."
	const D3DPOOL D3DPool = D3DPOOL_DEFAULT;
	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb172625(v=vs.85).aspx
	// "Buffers created with D3DPOOL_DEFAULT that do not specify D3DUSAGE_WRITEONLY may suffer a severe performance penalty."
	const DWORD D3DUsage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY; // Was D3DUSAGE_WRITEONLY

	// Create a new native index buffer of the requested size :
	UINT uiIndexBufferSize = IndexCount * sizeof(INDEX16);
	HRESULT hRet = g_pD3DDevice->CreateIndexBuffer(
		uiIndexBufferSize, // Size of the index buffer, in bytes.
		D3DUsage,
		/*Format=*/D3DFMT_INDEX16,
		D3DPool,
		&Result,
		nullptr // pSharedHandle
	);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateIndexBuffer");
	if (FAILED(hRet)) {
		assert(Result == nullptr);
	}

	return Result;
}
enum convertIndexBuffer
{
	passthrough=0,
	convertQuadList=1,
	convertQuadStrip=2,
};
ConvertedIndexBuffer& CxbxUpdateActiveIndexBuffer
(
	INDEX16* pXboxIndexData,
	unsigned XboxIndexCount,
	convertIndexBuffer convertIndexBuffer
)
{
	LOG_INIT; // Allows use of DEBUG_D3DRESULT

	uint32_t LookupKey = (uint32_t)pXboxIndexData;
	unsigned RequiredIndexCount = XboxIndexCount;

	if (convertIndexBuffer==convertQuadList) {
		LOG_TEST_CASE("ConvertQuadListToTriangleList");
		RequiredIndexCount = QuadListToTriangleListVertexCount(XboxIndexCount);
		// For now, indicate the quad-to-triangles case using the otherwise
		// (due to alignment) always-zero least significant bit :
		LookupKey |= 1;
	}
	/* // quadstrip doesn't need convertion. only need to convert the primitive type to triangle strip.
	else if(convertIndexBuffer == convertQuadStrip) {
			LOG_TEST_CASE("ConvertQuadStripToTriangleList");
			RequiredIndexCount = QuadListToTriangleListVertexCount( (XboxIndexCount-2)*2+6 );//in quadstrip, (vertex count -2 )/2 == quad count, quad count X2=triangle count, triangle count X3 = triangle list vertex count. we need two more triangles for quad strip convertion.
			// For now, indicate the quad-to-triangles case using the otherwise
			// (due to alignment) always-zero least significant bit :
			LookupKey |= 1;
	}
	*/
	// Poor-mans eviction policy : when exceeding treshold, clear entire cache :
	if (g_IndexBufferCache.size() > INDEX_BUFFER_CACHE_SIZE) {
		g_IndexBufferCache.clear(); // Note : ConvertedIndexBuffer destructor will release any assigned pHostIndexBuffer
	}

	// Create a reference to the active buffer
	ConvertedIndexBuffer& CacheEntry = g_IndexBufferCache[LookupKey];

	// If the current index buffer size is too small, free it, so it'll get re-created
	bool bNeedRepopulation = CacheEntry.IndexCount < RequiredIndexCount;
	if (bNeedRepopulation) {
		if (CacheEntry.pHostIndexBuffer != nullptr)
			CacheEntry.pHostIndexBuffer->Release();

		CacheEntry = {};
	}

	// If we need to create an index buffer, do so.
	if (CacheEntry.pHostIndexBuffer == nullptr) {
		CacheEntry.pHostIndexBuffer = CxbxCreateIndexBuffer(RequiredIndexCount);
		if (!CacheEntry.pHostIndexBuffer)
			CxbxrAbort("CxbxUpdateActiveIndexBuffer: IndexBuffer Create Failed!");
	}

	// TODO : Speeds this up, perhaps by hashing less often, and/or by
	// doing two hashes : a small subset regularly, all data less frequently.
	uint64_t uiHash = ComputeHash(pXboxIndexData, XboxIndexCount * sizeof(INDEX16));

	// If the data needs updating, do so
	bNeedRepopulation |= (uiHash != CacheEntry.Hash);
	if (bNeedRepopulation)	{
		// Update the Index Count and the hash
		CacheEntry.IndexCount = RequiredIndexCount;
		CacheEntry.Hash = uiHash;

		// Update the host index buffer
		INDEX16* pHostIndexBufferData = nullptr;
		HRESULT hRet = CacheEntry.pHostIndexBuffer->Lock(0, /*entire SizeToLock=*/0, (D3DLockData **)&pHostIndexBufferData, D3DLOCK_DISCARD);
		DEBUG_D3DRESULT(hRet, "CacheEntry.pHostIndexBuffer->Lock");
		if (pHostIndexBufferData == nullptr) {
			CxbxrAbort("CxbxUpdateActiveIndexBuffer: Could not lock index buffer!");
		}

		// Determine highest and lowest index in use :
		WalkIndexBuffer(CacheEntry.LowIndex, CacheEntry.HighIndex, pXboxIndexData, XboxIndexCount);
		if (convertIndexBuffer == convertQuadList) {
			// Note, that LowIndex and HighIndex won't change due to any quad-to-triangle conversion,
			// so it's less work to WalkIndexBuffer over the input instead of the converted index buffer.
			EmuLog(LOG_LEVEL::DEBUG, "CxbxUpdateActiveIndexBuffer: Converting quads to %d triangle indices (D3DFMT_INDEX16)", RequiredIndexCount);
			CxbxConvertQuadListToTriangleListIndices(pXboxIndexData, RequiredIndexCount, pHostIndexBufferData);
		}else if (convertIndexBuffer == convertQuadStrip) {
			// Note, that LowIndex and HighIndex won't change due to any quad-to-triangle conversion,
    		// so it's less work to WalkIndexBuffer over the input instead of the converted index buffer.
			EmuLog(LOG_LEVEL::DEBUG, "CxbxUpdateActiveIndexBuffer: Converting quads to %d triangle indices (D3DFMT_INDEX16)", RequiredIndexCount);
			CxbxConvertQuadStripToTriangleListIndices(pXboxIndexData, RequiredIndexCount, pHostIndexBufferData);
		} else {
			EmuLog(LOG_LEVEL::DEBUG, "CxbxUpdateActiveIndexBuffer: Copying %d indices (D3DFMT_INDEX16)", XboxIndexCount);
			memcpy(pHostIndexBufferData, pXboxIndexData, XboxIndexCount * sizeof(INDEX16));
		}

		CacheEntry.pHostIndexBuffer->Unlock();
	}

	// Activate the new native index buffer :
	HRESULT hRet = g_pD3DDevice->SetIndices(CacheEntry.pHostIndexBuffer);
	// Note : Under Direct3D 9, the BaseVertexIndex argument is moved towards DrawIndexedPrimitive
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetIndices");

	if (FAILED(hRet))
		CxbxrAbort("CxbxUpdateActiveIndexBuffer: SetIndices Failed!");

	return CacheEntry;
}

void UpdateHostBackBufferDesc()
{
    IDirect3DSurface *pCurrentHostBackBuffer = nullptr;
    auto hRet = g_pD3DDevice->GetBackBuffer(
        0, // iSwapChain
        0, D3DBACKBUFFER_TYPE_MONO, &pCurrentHostBackBuffer);

    if (hRet != D3D_OK) {
        CxbxrAbort("Unable to get host backbuffer surface");
    }

    hRet = pCurrentHostBackBuffer->GetDesc(&g_HostBackBufferDesc);
    if (hRet != D3D_OK) {
        pCurrentHostBackBuffer->Release();
        CxbxrAbort("Unable to determine host backbuffer dimensions");
    }

    pCurrentHostBackBuffer->Release();
}

void SetAspectRatioScale(const xbox::X_D3DPRESENT_PARAMETERS* pPresentationParameters)
{
    // NOTE: Some games use anamorphic widesceen (expecting a 4:3 surface to be displayed at 16:9)
    // For those, we *lie* about the default width, for the scaler
    // 720p / 1080i are *always* widescreen, and will have the correct backbuffer size, so we only
    // apply this 'hack' for non-hd resolutions
    g_AspectRatioScaleWidth = pPresentationParameters->BackBufferWidth;
    g_AspectRatioScaleHeight = pPresentationParameters->BackBufferHeight;

    if (pPresentationParameters->Flags & X_D3DPRESENTFLAG_WIDESCREEN &&
        pPresentationParameters->BackBufferHeight < 720) {
        // Lie and pretend we are 1280x720, this works because this ratio is only used in calculations
        // and not used as actual raw input values
        g_AspectRatioScaleWidth = 1280;
        g_AspectRatioScaleHeight = 720;
    }
    
    const auto imageAspect = (float)g_AspectRatioScaleWidth / (float)g_AspectRatioScaleHeight;
    const auto screenAspect = (float)g_HostBackBufferDesc.Width / (float)g_HostBackBufferDesc.Height;
    g_AspectRatioScale = screenAspect > imageAspect ? (float)g_HostBackBufferDesc.Height / (float)g_AspectRatioScaleHeight : (float)g_HostBackBufferDesc.Width / (float)g_AspectRatioScaleWidth;
}

#define CXBX_D3DMULTISAMPLE_XSCALE(type) (((type) & xbox::X_D3DMULTISAMPLE_XSCALE_MASK) >> xbox::X_D3DMULTISAMPLE_XSCALE_SHIFT)
#define CXBX_D3DMULTISAMPLE_YSCALE(type) (((type) & xbox::X_D3DMULTISAMPLE_YSCALE_MASK) >> xbox::X_D3DMULTISAMPLE_YSCALE_SHIFT)

void SetXboxMultiSampleType(xbox::X_D3DMULTISAMPLE_TYPE value)
{
	// Validate & correct input, to detect test cases and avoid trouble when using g_Xbox_MultiSampleType :
	if (value == 0) {
		// Correcting zero to X_D3DMULTISAMPLE_NONE
		value = xbox::X_D3DMULTISAMPLE_NONE;
	}
	if (value & ~xbox::X_D3DMULTISAMPLE_KNOWN_MASK) {
		LOG_TEST_CASE("Removing unknown X_D3DMULTISAMPLE bits");
		value &= xbox::X_D3DMULTISAMPLE_KNOWN_MASK;
	}
	if (CXBX_D3DMULTISAMPLE_XSCALE(value) == 0) {
		LOG_TEST_CASE("Correcting XSCALE 0 to 1");
		value |= 1 << xbox::X_D3DMULTISAMPLE_XSCALE_SHIFT;
	}
	if (CXBX_D3DMULTISAMPLE_YSCALE(value) == 0) {
		LOG_TEST_CASE("Correcting YSCALE 0 to 1");
		value |= 1 << xbox::X_D3DMULTISAMPLE_YSCALE_SHIFT;
	}
	if ((CXBX_D3DMULTISAMPLE_XSCALE(value) == 1) && (CXBX_D3DMULTISAMPLE_YSCALE(value) == 1)) {
		if (value & xbox::X_D3DMULTISAMPLE_ALGO_MASK) {
			LOG_TEST_CASE("Removing algorithm for 1 by 1 scale");
			value &= ~xbox::X_D3DMULTISAMPLE_ALGO_MASK;
		}
		if (value & xbox::X_D3DMULTISAMPLE_SAMPLING_MASK) {
			LOG_TEST_CASE("Removing sampling for 1 by 1 scale");
			value &= ~xbox::X_D3DMULTISAMPLE_SAMPLING_MASK;
		}
	}
	if (value == xbox::X_D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN) {
		LOG_TEST_CASE("X_D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN"); // This used to use scale 1.5f
	}
	// Set the now-validated g_Xbox_MultiSampleType value :
	g_Xbox_MultiSampleType = value;
}

float GetMultiSampleOffsetDelta()
{
	// TODO : What impact does X_D3DMULTISAMPLE_SAMPLING_SUPER have on offset?
	return (g_Xbox_MultiSampleType & xbox::X_D3DMULTISAMPLE_SAMPLING_MULTI) ? 0.0f : 0.5f;
}

static inline void GetMultiSampleOffset(float& xOffset, float& yOffset)
{
	xOffset = yOffset = GetMultiSampleOffsetDelta();
}

// Get the raw X and Y scale of the multisample mode
// Both MSAA and SSAA result in increased rendertarget size
// add BuckBufferScale to comply xbox behavior
void GetMultiSampleScaleRaw(float& xScale, float& yScale) {
	xScale = static_cast<float>(CXBX_D3DMULTISAMPLE_XSCALE(g_Xbox_MultiSampleType))* g_Xbox_BackbufferScaleX;
	yScale = static_cast<float>(CXBX_D3DMULTISAMPLE_YSCALE(g_Xbox_MultiSampleType))* g_Xbox_BackbufferScaleY;
}

// Get the "screen" scale factors
// Multisampling modes and backbufferscale affect the screen coordinate space
// Titles can render pre-transformed vertices to screen space (using XYZRHW vertex position data or otherwise)
// so we need to know the space they are in to interpret it correctly
void GetScreenScaleFactors(float& scaleX, float& scaleY) {
	scaleX = 1;
	scaleY = 1;

	// With fixed-function mode, titles don't have to account for these scale factors,
	// so we don't have reverse them.
	// Test cases:
	// Fixed-func passthrough, title does not apply SSAA scale:
	// - Shenmue II (Menu)
	// Fixed-func passthrough, title does not apply backbuffer scale:
	// - Antialias sample(background gradient)
	extern bool is_pgraph_using_NV2A_Kelvin(void);
	extern VertexShaderMode g_NV2A_VertexShaderMode;
	VertexShaderMode VSHMode=g_Xbox_VertexShaderMode;
	if (is_pgraph_using_NV2A_Kelvin())
		VSHMode = g_NV2A_VertexShaderMode;
	if (VSHMode != VertexShaderMode::ShaderProgram) {
		return;
	}

	if (g_LastD3DSwap == xbox::X_D3DSWAP_COPY) {
		// HACK: Pretend we are drawing to the frontbuffer
		// by disabling scale factors.
		return;
	}

	// Example:
	// NFS HP2 renders in-game at 640*480
	// The title uses MSAA, which increases the rendertarget size, but leaves the screen scale unaffected
	// The backbuffer scale is (0.75, 1), which contributes to the screen scale
	// So the Xbox expects vertices in 480*480 coordinate space
	// todo: find supersample type info from kelvin and use it here.
	bool isSuperSampleMode = g_Xbox_MultiSampleType & xbox::X_D3DMULTISAMPLE_SAMPLING_SUPER;
	extern void CxbxrGetSuperSampleScaleXY(NV2AState *,float& x, float& y);

	if (is_pgraph_using_NV2A_Kelvin()) {
		bool isMultiSampleEnabled = NV2ARenderStates.GetXboxRenderState(xbox::X_D3DRS_MULTISAMPLEANTIALIAS);

		// Apply multisample scale if supersampling is enabled
		// Test cases:
		// Antialias sample (text overlay is drawn with antialiasing disabled)
		if (isMultiSampleEnabled && isSuperSampleMode) {
			GetMultiSampleScaleRaw(scaleX, scaleY);
			
			//CxbxrGetSuperSampleScaleXY(scaleX, scaleY);
			//scaleX*= g_Xbox_BackbufferScaleX;
			//scaleY*= g_Xbox_BackbufferScaleY;
		}
	}
	else {
		// SSAA increases the screen scale (but MSAA does not)
		bool isMultiSampleEnabled = XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_MULTISAMPLEANTIALIAS);
		//bool isSuperSampleMode = g_Xbox_MultiSampleType & xbox::X_D3DMULTISAMPLE_SAMPLING_SUPER;

		// Apply multisample scale if supersampling is enabled
		// Test cases:
		// Antialias sample (text overlay is drawn with antialiasing disabled)
		if (isMultiSampleEnabled && isSuperSampleMode) {
			GetMultiSampleScaleRaw(scaleX, scaleY);
		}
	}

	// Account for the backbuffer scale
	// Test cases:
	// Vertex program passthrough equivalent (title does apply backbuffer scale):
	// - NFS:HP2 (car speed and other in-game UI elements)
	//scaleX *= g_Xbox_BackbufferScaleX;
	//scaleY *= g_Xbox_BackbufferScaleY;
}

// Get the raw subpixel dimensions of the rendertarget buffer
void GetRenderTargetRawDimensions(float& x, float&y, xbox::X_D3DSurface* rt) {
	x = (float) GetPixelContainerWidth(rt);
	y = (float) GetPixelContainerHeight(rt);
}

// Get the base rendertarget dimensions excluding multisample scaling
// e.g. a raw 1280*960 rendertarget with 2x MSAA would be have a base 640*480
void GetRenderTargetBaseDimensions(float& x, float& y) {
	GetRenderTargetRawDimensions(x, y, g_pXbox_RenderTarget);

	float aaX, aaY;
	GetMultiSampleScaleRaw(aaX, aaY);

	x /= aaX;
	y /= aaY;
}

// Get the pixel dimensions of the backbuffer, accounting for multisample mode
void GetBackBufferPixelDimensions(float& x, float& y) {
	GetRenderTargetRawDimensions(x, y, g_pXbox_BackBufferSurface);

	// MSAA introduces subpixels, so scale them away
	if (g_Xbox_MultiSampleType & xbox::X_D3DMULTISAMPLE_SAMPLING_MULTI) {
		float aaX, aaY;
		GetMultiSampleScaleRaw(aaX, aaY);
		x /= aaX;
		y /= aaY;
	}
}

static DWORD makeSpaceCount = 0;

__declspec(naked) xbox::dword_xt* CxbxrImpl_MakeSpace(void)
{
	makeSpaceCount++;
	DWORD d3ddevice;
	d3ddevice = *g_pXbox_D3DDevice;
	xbox::dword_xt* result;
	if (XB_TRMP(D3DDevice_MakeSpace)){
		__asm {
			push edx
			push ecx
			push eax
			call XB_TRMP(D3DDevice_MakeSpace)
			mov result, eax
			pop eax
			pop ecx
			pop edx
			mov eax, result
			ret
		}
	}
	else if (XB_TRMP(D3DDevice_MakeSpace_0)) {
		__asm {
			push edx
			push ecx
			push eax
			call XB_TRMP(D3DDevice_MakeSpace_0)
			mov result, eax
			pop eax
			pop ecx
			pop edx
            mov eax, result
			ret
		}
	}
	else if (XB_TRMP(CDevice_MakeSpace)) {
		__asm {
			push edx
			push ecx
            // CDevice_MakeSpace() is a __thiscall which requires this pointer to be in ecx
			mov  ecx, d3ddevice
			call XB_TRMP(CDevice_MakeSpace)
			mov result, eax
			pop ecx
			pop edx
			mov eax, result
			ret
		}
	}
	else if (XB_TRMP(D3D_MakeRequestedSpace_4__LTCG_eax_RequestedSpace)) {
		__asm {
			LTCG_PROLOGUE
			// D3D_MakeRequestedSpace_4__LTCG_eax_RequestedSpace() doesn't require this pointer., max size in eax (0x8000), min. size in stack (0x4000)
			mov  eax, 0x4000
			push eax
			shl  eax,1
			call XB_TRMP(D3D_MakeRequestedSpace_4__LTCG_eax_RequestedSpace)
			mov result, eax
			pop eax
			mov eax, result
			LTCG_EPILOGUE
			ret 
		}
	}
	else{
		CxbxrAbort("MakeSpace not available!");
		__asm {
			ret
		}
    }
}
static D3DMATRIX* g_xbox_transform_matrix = nullptr;
void init_xboxD3dGlobalPointer(void)
{
	// restore the usage for EmuKickOff()/SetTransform()/GetTransform()
	// Set g_pXbox_D3DDevice to point to the Xbox D3D Device
	if (g_pXbox_D3DDevice == nullptr) {
		auto it = g_SymbolAddresses.find("D3D_g_pDevice");
		if (it != g_SymbolAddresses.end()) {
			g_pXbox_D3DDevice = (xbox::dword_xt*)it->second;
		}
	}
	if ((g_pXbox_D3DDevice == nullptr)
		|| (*g_pXbox_D3DDevice == 0)) {
		CxbxrAbort("D3D_g_pDevice not available!");
	}
	else {
		g_pXbox_pPush = (xbox::dword_xt**)*g_pXbox_D3DDevice;
		g_pXbox_pPushLimit = g_pXbox_pPush + 1;
	}
	// get the transform matrix pointer in xbox d3d and store it in g_xbox_transform_matrix if it's not set yet
	if (g_xbox_transform_matrix == nullptr) {
		byte* pSetTransform = nullptr;
		auto it = g_SymbolAddresses.find("D3DDevice_SetTransform");
		if (it != g_SymbolAddresses.end()) {
			pSetTransform = (byte*)it->second;
			if ((*(byte*)(pSetTransform + 0x11)) == 0x8D) {
				g_xbox_transform_matrix = (D3DMATRIX*)(((*(byte*)(pSetTransform + 0x13)) * 0x40) + *g_pXbox_D3DDevice);
			}
			else {
				g_xbox_transform_matrix = (D3DMATRIX*)((*(DWORD*)(pSetTransform + 0x19)) + *g_pXbox_D3DDevice);
			}
		}
		else {
			it = g_SymbolAddresses.find("D3DDevice_SetTransform_0__LTCG_eax1_edx2");
			if (it != g_SymbolAddresses.end()) {
				pSetTransform = (byte*)it->second;
				// currently only fit for LTCG 5849
				if ((*(byte*)(pSetTransform + 0x10)) == 0x19) {
					// not sure the offset is 0x19 or not, need to verify.
					g_xbox_transform_matrix = (D3DMATRIX*)((*(DWORD*)(pSetTransform + 0x11)) + *g_pXbox_D3DDevice);
				}
			}
		}
	}
	if (g_xbox_transform_matrix == nullptr)
		CxbxrAbort("D3DDevice_SetTransform not available!");

}
void Direct3D_CreateDevice_Start
(
	const xbox::X_D3DPRESENT_PARAMETERS     *pPresentationParameters
)
{
#if 1
	// init xbox d3d related pointers, D3DDevice, transform matrix, etx..
	if (g_pXbox_D3DDevice == nullptr) {
		auto it = g_SymbolAddresses.find("D3D_g_pDevice");
		if (it != g_SymbolAddresses.end()) {
			g_pXbox_D3DDevice = (xbox::dword_xt*)it->second;
		}
	}
	if (g_pXbox_D3DDevice == nullptr)
		CxbxrAbort("D3D_g_pDevice not available!");

#endif
	CxbxVertexShaderSetFlags();

    if (!XboxRenderStates.Init()) {
        CxbxrAbort("Failed to init XboxRenderStates");
    }

    if (!XboxTextureStates.Init(&XboxRenderStates)) {
        CxbxrAbort("Failed to init XboxTextureStates");
    }
	extern XboxRenderStateConverter NV2ARenderStates; // this var directly access Xbox internal renderstate variables.
	extern XboxTextureStateConverter NV2ATextureStates; // this var directly access Xbox intern TextureState variables.
	if (!NV2ARenderStates.InitWithNV2A()) {
		CxbxrAbort("Failed to init NV2ARenderStates");
	}

	if (!NV2ATextureStates.InitWithNV2A(&NV2ARenderStates)) {
		CxbxrAbort("Failed to init NV2ATextureStates");
	}

	SetXboxMultiSampleType(pPresentationParameters->MultiSampleType);

	// create default device *before* calling Xbox Direct3D_CreateDevice trampoline
	// to avoid hitting EMUPATCH'es that need a valid g_pD3DDevice

	if (g_pD3DDevice != nullptr) { // Check to make sure device is null, otherwise no need to create it
		return;
	}

	CreateDefaultD3D9Device(pPresentationParameters);
}

void Direct3D_CreateDevice_End
(
	const xbox::X_D3DPRESENT_PARAMETERS     *pPresentationParameters
)
{
#if 1 // restore the usage for EmuKickOff()
	init_xboxD3dGlobalPointer();
#endif

    UpdateHostBackBufferDesc();
    SetAspectRatioScale(pPresentationParameters);

    // If the Xbox version of CreateDevice didn't call SetRenderTarget, we must derive the default backbuffer ourselves
    // This works because CreateDevice always sets the current render target to the Xbox Backbuffer
    // In later XDKs, it does this inline rather than by calling D3DDevice_SetRenderTarget
    // meaning our patch doesn't always get called in these cases.
    // We fix the situation by calling the Xbox GetRenderTarget function, which immediately after CreateDevice
    // WILL always return the Backbuffer!
    // Test Case: Shin Megami Tensei: Nine
    if (g_pXbox_BackBufferSurface == xbox::zeroptr && g_pXbox_DefaultDepthStencilSurface == xbox::zeroptr) {
        // First, log the test case
        LOG_TEST_CASE("Xbox CreateDevice did not call SetRenderTarget");
    }

    if (g_pXbox_BackBufferSurface == xbox::zeroptr) {
        if (XB_TRMP(D3DDevice_GetRenderTarget)) {
            XB_TRMP(D3DDevice_GetRenderTarget)(&g_pXbox_BackBufferSurface);
        }
        else if (XB_TRMP(D3DDevice_GetRenderTarget2)) {
            g_pXbox_BackBufferSurface = XB_TRMP(D3DDevice_GetRenderTarget2)();
        }

        // At this point, g_pXbox_BackBufferSurface should now point to a valid render target
        // if it still doesn't, we cannot continue without crashing at draw time
        if (g_pXbox_BackBufferSurface == xbox::zeroptr) {
            CxbxrAbort("Unable to determine default Xbox backbuffer");
        }

        // Set the backbuffer as the initial rendertarget
        CxbxrImpl_SetRenderTarget(g_pXbox_BackBufferSurface, xbox::zeroptr);
    }

    // Now do the same, but for the default depth stencil surface
    if (g_pXbox_DefaultDepthStencilSurface == xbox::zeroptr) {
        if (XB_TRMP(D3DDevice_GetDepthStencilSurface)) {
            XB_TRMP(D3DDevice_GetDepthStencilSurface)(&g_pXbox_DefaultDepthStencilSurface);
        }
        else if (XB_TRMP(D3DDevice_GetDepthStencilSurface2)) {
            g_pXbox_DefaultDepthStencilSurface = XB_TRMP(D3DDevice_GetDepthStencilSurface2)();
        }

        // At this point, g_pXbox_DefaultDepthStencilSurface should now point to a valid depth stencil
        // If it doesn't, just log and carry on: Unlike RenderTarget, this situation is not fatal
        if (g_pXbox_DefaultDepthStencilSurface == xbox::zeroptr) {
            LOG_TEST_CASE("Unable to determine default Xbox depth stencil");
        } else {
            // Update only the depth stencil
            CxbxrImpl_SetRenderTarget(xbox::zeroptr, g_pXbox_DefaultDepthStencilSurface);
        }
    }
}

// Overload for logging
static void Direct3D_CreateDevice_4
(
    xbox::dword_xt                BehaviorFlags,
    xbox::X_D3DPRESENT_PARAMETERS *pPresentationParameters,
    xbox::X_D3DDevice            **ppReturnedDeviceInterface
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(BehaviorFlags)
		LOG_FUNC_ARG(pPresentationParameters)
		LOG_FUNC_ARG_OUT(ppReturnedDeviceInterface)
		LOG_FUNC_END;
}

// LTCG specific Direct3D_CreateDevice function...
// This uses a custom calling with parameters passed in EAX, ECX and the stack
// Test-case: Ninja Gaiden, Halo 2
__declspec(naked) xbox::hresult_xt WINAPI xbox::EMUPATCH(Direct3D_CreateDevice_4)
(
    X_D3DPRESENT_PARAMETERS     *pPresentationParameters
)
{
    DWORD BehaviorFlags;
    xbox::X_D3DDevice **ppReturnedDeviceInterface;
    __asm {
		LTCG_PROLOGUE
		mov  BehaviorFlags, eax
		mov  ppReturnedDeviceInterface, ecx
	}

	// Log
	Direct3D_CreateDevice_4(BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

	Direct3D_CreateDevice_Start(pPresentationParameters);

	// Only then call Xbox CreateDevice function
	hresult_xt hRet;
	__asm {
		push pPresentationParameters
		mov  eax, BehaviorFlags
		mov  ecx, ppReturnedDeviceInterface
		call XB_TRMP(Direct3D_CreateDevice_4)
		mov  hRet, eax
	}

	Direct3D_CreateDevice_End(pPresentationParameters);

	__asm {
		mov  eax, hRet
		LTCG_EPILOGUE
		ret  4
	}
}

// Overload for logging
static void Direct3D_CreateDevice_16__LTCG_eax_BehaviorFlags_ecx_ppReturnedDeviceInterface
(
    xbox::uint_xt                 Adapter,
    D3DDEVTYPE                    DeviceType,
    HWND                          hFocusWindow,
    xbox::dword_xt                BehaviorFlags,
    xbox::X_D3DPRESENT_PARAMETERS *pPresentationParameters,
    xbox::X_D3DDevice            **ppReturnedDeviceInterface
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Adapter)
		LOG_FUNC_ARG(DeviceType)
		LOG_FUNC_ARG(hFocusWindow)
		LOG_FUNC_ARG(BehaviorFlags)
		LOG_FUNC_ARG(pPresentationParameters)
		LOG_FUNC_ARG_OUT(ppReturnedDeviceInterface)
		LOG_FUNC_END;
}

// LTCG specific Direct3D_CreateDevice function...
// This uses a custom calling convention passing parameters on stack, in EAX and ECX
// Test-case: Aggressive Inline, Midtown Madness 3
__declspec(naked) xbox::hresult_xt WINAPI xbox::EMUPATCH(Direct3D_CreateDevice_16__LTCG_eax_BehaviorFlags_ecx_ppReturnedDeviceInterface)
(
    uint_xt                     Adapter,
    D3DDEVTYPE                  DeviceType,
    HWND                        hFocusWindow,
    X_D3DPRESENT_PARAMETERS     *pPresentationParameters
)
{
	dword_xt BehaviorFlags;
	xbox::X_D3DDevice **ppReturnedDeviceInterface;
	__asm {
		LTCG_PROLOGUE
		mov  BehaviorFlags, eax
		mov  ppReturnedDeviceInterface, ecx
	}

	// Log
	Direct3D_CreateDevice_16__LTCG_eax_BehaviorFlags_ecx_ppReturnedDeviceInterface(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

	Direct3D_CreateDevice_Start(pPresentationParameters);

	// Only then call Xbox CreateDevice function
	hresult_xt hRet;
	__asm {
		push pPresentationParameters
		push hFocusWindow
		push DeviceType
		push Adapter
		mov  eax, BehaviorFlags
		mov  ecx, ppReturnedDeviceInterface
		call XB_TRMP(Direct3D_CreateDevice_16__LTCG_eax_BehaviorFlags_ecx_ppReturnedDeviceInterface)
		mov  hRet, eax
	}

	Direct3D_CreateDevice_End(pPresentationParameters);

	__asm {
		mov  eax, hRet
		LTCG_EPILOGUE
		ret  10h
	}
}

// Overload for logging
static void Direct3D_CreateDevice_16__LTCG_eax_BehaviorFlags_ebx_ppReturnedDeviceInterface
(
    xbox::uint_xt                 Adapter,
    D3DDEVTYPE                    DeviceType,
    HWND                          hFocusWindow,
    xbox::dword_xt                BehaviorFlags,
    xbox::X_D3DPRESENT_PARAMETERS *pPresentationParameters,
    xbox::X_D3DDevice            **ppReturnedDeviceInterface
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Adapter)
		LOG_FUNC_ARG(DeviceType)
		LOG_FUNC_ARG(hFocusWindow)
		LOG_FUNC_ARG(BehaviorFlags)
		LOG_FUNC_ARG(pPresentationParameters)
		LOG_FUNC_ARG_OUT(ppReturnedDeviceInterface)
		LOG_FUNC_END;
}

// LTCG specific Direct3D_CreateDevice function...
// This uses a custom calling convention passing parameters on stack, in EAX and EBX
// Test-case: NASCAR Heat 2002
__declspec(naked) xbox::hresult_xt WINAPI xbox::EMUPATCH(Direct3D_CreateDevice_16__LTCG_eax_BehaviorFlags_ebx_ppReturnedDeviceInterface)
(
    uint_xt                     Adapter,
    D3DDEVTYPE                  DeviceType,
    HWND                        hFocusWindow,
    X_D3DPRESENT_PARAMETERS     *pPresentationParameters
)
{
	dword_xt BehaviorFlags;
	xbox::X_D3DDevice **ppReturnedDeviceInterface;
	__asm {
		LTCG_PROLOGUE
		mov  BehaviorFlags, eax
		mov  ppReturnedDeviceInterface, ebx
	}

	// Log
	Direct3D_CreateDevice_16__LTCG_eax_BehaviorFlags_ebx_ppReturnedDeviceInterface(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

	Direct3D_CreateDevice_Start(pPresentationParameters);

	// Only then call Xbox CreateDevice function
	hresult_xt hRet;
	__asm {
		push pPresentationParameters
		push hFocusWindow
		push DeviceType
		push Adapter
		mov  eax, BehaviorFlags
		mov  ebx, ppReturnedDeviceInterface
		call XB_TRMP(Direct3D_CreateDevice_16__LTCG_eax_BehaviorFlags_ebx_ppReturnedDeviceInterface)
		mov  hRet, eax
	}

	Direct3D_CreateDevice_End(pPresentationParameters);

	__asm {
		mov  eax, hRet
		LTCG_EPILOGUE
		ret  10h
	}
}

// Overload for logging
static void D3DDevice_SetIndices_4
(
    xbox::X_D3DIndexBuffer   *pIndexData,
    xbox::uint_xt             BaseVertexIndex
)
{
    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pIndexData)
        LOG_FUNC_ARG(BaseVertexIndex)
        LOG_FUNC_END;
}

void WINAPI CxbxrImpl_SetIndices
(
	xbox::X_D3DIndexBuffer* pIndexData,
	xbox::uint_xt           BaseVertexIndex
)
{
// Cache the base vertex index then call the Xbox function
 g_Xbox_BaseVertexIndex = BaseVertexIndex;
}

// ******************************************************************
// * patch: D3DDevice_SetIndices_4
// LTCG specific D3DDevice_SetIndices function...
// This uses a custom calling convention where parameter is passed in EBX and Stack
// Test Case: Conker
// ******************************************************************
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetIndices_4)
(
    uint_xt                BaseVertexIndex
)
{
    X_D3DIndexBuffer   *pIndexData;
    __asm {
        LTCG_PROLOGUE
        mov  pIndexData, ebx
	//	mov  ebx, pIndexData
		push BaseVertexIndex
		call XB_TRMP(D3DDevice_SetIndices_4);
	}
    // Log
    D3DDevice_SetIndices_4(pIndexData, BaseVertexIndex);

    // Cache the base vertex index
    // g_Xbox_BaseVertexIndex = BaseVertexIndex;

	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)pIndexData;
	PBTokenArray[3] = (DWORD)BaseVertexIndex;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetIndices_4, 2, PBTokenArray);//argCount 14

    // Call LTCG-specific trampoline
    __asm {

        LTCG_EPILOGUE
        ret  4
    }
}


// ******************************************************************
// * patch: D3DDevice_SetIndices
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetIndices)
(
	X_D3DIndexBuffer   *pIndexData,
	uint_xt                BaseVertexIndex
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pIndexData)
		LOG_FUNC_ARG(BaseVertexIndex)
		LOG_FUNC_END;
	// Cache the base vertex index then call the Xbox function
	// g_Xbox_BaseVertexIndex = BaseVertexIndex;

	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)pIndexData;
	PBTokenArray[3] = (DWORD)BaseVertexIndex;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetIndices, 2, PBTokenArray);//argCount 14

	XB_TRMP(D3DDevice_SetIndices)(pIndexData, BaseVertexIndex);
}

// ******************************************************************
// * patch: Direct3D_CreateDevice
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(Direct3D_CreateDevice)
(
    uint_xt                        Adapter,
    D3DDEVTYPE                  DeviceType,
    HWND                        hFocusWindow,
    dword_xt                       BehaviorFlags,
    X_D3DPRESENT_PARAMETERS    *pPresentationParameters,
    xbox::X_D3DDevice         **ppReturnedDeviceInterface
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Adapter)
		LOG_FUNC_ARG(DeviceType)
		LOG_FUNC_ARG(hFocusWindow)
		LOG_FUNC_ARG(BehaviorFlags)
		LOG_FUNC_ARG(pPresentationParameters)
		LOG_FUNC_ARG_OUT(ppReturnedDeviceInterface)
		LOG_FUNC_END;

	Direct3D_CreateDevice_Start(pPresentationParameters);

	// Only then call Xbox CreateDevice function
	hresult_xt hRet = XB_TRMP(Direct3D_CreateDevice)(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

	Direct3D_CreateDevice_End(pPresentationParameters);

	return hRet;
}

// ******************************************************************
// * patch: D3DDevice_Reset
// ******************************************************************

static void CxbxrImpl_Reset(xbox::X_D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	// Unlike the host version of Reset, The Xbox version does not actually reset the entire device
	// Instead, it simply re-creates the backbuffer with a new configuration

	// Store the new multisampling configuration
	SetXboxMultiSampleType(pPresentationParameters->MultiSampleType);

	// Update scaling aspect ratio
	SetAspectRatioScale(pPresentationParameters);

	// Since Reset will call create a new backbuffer surface, we can clear our current association
	// NOTE: We don't actually free the Xbox data, the Xbox side will do this for us when we call the trampoline below.
	// We must not reset the values to nullptr, since the XDK will re-use the same addresses for the data headers
	// (they are members of the Direct3DDevice object). if we overwrite then, the reference to the xbox backbuffer will be lost
	// and we'll get a black screen.
	FreeHostResource(GetHostResourceKey(g_pXbox_BackBufferSurface));
	FreeHostResource(GetHostResourceKey(g_pXbox_DefaultDepthStencilSurface));

	// Below requirement for patched function(s) in order to function properly.
	// Perform xbox's D3DDevice_Reset call.

	// Perform CxbxrImpl_SetRenderTarget call.
}

xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_Reset)
(
	X_D3DPRESENT_PARAMETERS* pPresentationParameters
)
{
	LOG_FUNC_ONE_ARG(pPresentationParameters);

	bool WaitForPGRAPH;
	WaitForPGRAPH = true;
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)&WaitForPGRAPH;// (DWORD)PrimitiveType;

	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_Reset, 1, PBTokenArray);//argCount, not necessary, default to 14

	EmuKickOff();

	while (WaitForPGRAPH)
		; //this line is must have

	CxbxrImpl_Reset(pPresentationParameters);

	// Call the Xbox Reset function to do the rest of the work for us
	hresult_xt hRet = XB_TRMP(D3DDevice_Reset)(pPresentationParameters);

	// Refresh the current render target and depth stencil, to apply changes made within D3DDevice_Reset
	// Some XDKs do this for us, but not all do!
	CxbxrImpl_SetRenderTarget(g_pXbox_RenderTarget, g_pXbox_DepthStencil);

	return hRet;
}

static void D3DDevice_Reset_0__LTCG_edi1(xbox::X_D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	LOG_FUNC_ONE_ARG(pPresentationParameters);
}

__declspec(naked) xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_Reset_0__LTCG_edi1)()
{
	X_D3DPRESENT_PARAMETERS* pPresentationParameters;
	__asm {
		LTCG_PROLOGUE
		mov  pPresentationParameters, edi
	}

	// Log
	D3DDevice_Reset_0__LTCG_edi1(pPresentationParameters);

	bool WaitForPGRAPH;
	WaitForPGRAPH = true;
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)&WaitForPGRAPH;// (DWORD)PrimitiveType;

	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_Reset_0__LTCG_edi1, 1, PBTokenArray);//argCount, not necessary, default to 14

	EmuKickOff();

	while (WaitForPGRAPH)
		; //this line is must have

	CxbxrImpl_Reset(pPresentationParameters);

	// Call the Xbox version of DestroyResource
	__asm {
		mov  edi, pPresentationParameters
		call XB_TRMP(D3DDevice_Reset_0__LTCG_edi1)
	}

	CxbxrImpl_SetRenderTarget(g_pXbox_RenderTarget, g_pXbox_DepthStencil);

	__asm {
		LTCG_EPILOGUE
		ret
	}
}

static void D3DDevice_Reset_0__LTCG_ebx1(xbox::X_D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	LOG_FUNC_ONE_ARG(pPresentationParameters);
}

__declspec(naked) xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_Reset_0__LTCG_ebx1)()
{
	X_D3DPRESENT_PARAMETERS* pPresentationParameters;
	__asm {
		LTCG_PROLOGUE
		mov  pPresentationParameters, ebx
	}

	// Log
	D3DDevice_Reset_0__LTCG_ebx1(pPresentationParameters);

	bool WaitForPGRAPH;
	WaitForPGRAPH = true;
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)&WaitForPGRAPH;// (DWORD)PrimitiveType;

	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_Reset_0__LTCG_ebx1, 1, PBTokenArray);//argCount, not necessary, default to 14

	EmuKickOff();

	while (WaitForPGRAPH)
		; //this line is must have

	CxbxrImpl_Reset(pPresentationParameters);

	// Call the Xbox version of DestroyResource
	__asm {
		mov  ebx, pPresentationParameters
		call XB_TRMP(D3DDevice_Reset_0__LTCG_ebx1)
	}

	CxbxrImpl_SetRenderTarget(g_pXbox_RenderTarget, g_pXbox_DepthStencil);

	__asm {
		LTCG_EPILOGUE
		ret
	}
}


// ******************************************************************
// * patch: D3DDevice_GetDisplayFieldStatus
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_GetDisplayFieldStatus)(X_D3DFIELD_STATUS *pFieldStatus)
{
	// NOTE: This can be unpatched only when NV2A does it's own VBlank and HLE _Swap function is unpatched


	LOG_FUNC_ONE_ARG(pFieldStatus);

	// Read AV Flags to determine if Progressive or Interlaced
	// The xbox does this by reading from pDevice->m_Miniport.m_CurrentAvInfo
	// but we don't have an OOVPA for that. Instead, we call the Xbox implementation of 
	// D3DDevice_GetDisplayMode and read the result

	X_D3DDISPLAYMODE displayMode;

	// If we can find the Xbox version of GetDisplayMode, use the real data returned, otherwise
	// use a sensible default
	if (XB_TRMP(D3DDevice_GetDisplayMode) != nullptr) {
		XB_TRMP(D3DDevice_GetDisplayMode)(&displayMode);
	} else {
		// We don't show a warning because doing so pollutes the kernel debug log as this function gets called every frame
		displayMode.Flags = X_D3DPRESENTFLAG_INTERLACED;
	}
	
	// Set the VBlank count
	pFieldStatus->VBlankCount = g_Xbox_VBlankData.VBlank;

	// If we are interlaced, return the current field, otherwise, return progressive scan
	if (displayMode.Flags & X_D3DPRESENTFLAG_INTERLACED) {
		pFieldStatus->Field = (g_Xbox_VBlankData.VBlank % 2 == 0) ? X_D3DFIELD_ODD : X_D3DFIELD_EVEN;
	} else {
		pFieldStatus->Field = X_D3DFIELD_PROGRESSIVE;
	}
}

// ******************************************************************
// * patch: D3DDevice_BeginPush_4
// TODO: Find a test case and verify this
// Starting from XDK 4531, this changed to 1 parameter only.
// Is this definition incorrect, or did it change at some point?
// ******************************************************************
xbox::PDWORD WINAPI xbox::EMUPATCH(D3DDevice_BeginPush_4)(dword_xt Count)
{
	LOG_FUNC_ONE_ARG(Count);
	if (is_pushbuffer_recording()) {
		XB_TRMP(D3DDevice_BeginPush_4)(Count);
	}
	xbox::hresult_xt result = 0;
	return (xbox::PDWORD)result;
}

// ******************************************************************
// * patch: D3DDevice_BeginPush_8
// TODO: Find a test case and verify this: RalliSport Challenge XDK 4134
// For XDK before 4531
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_BeginPush_8)(dword_xt Count, dword_xt** ppPush)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Count)
		LOG_FUNC_ARG(ppPush)
		LOG_FUNC_END;
	if (is_pushbuffer_recording()) {
		XB_TRMP(D3DDevice_BeginPush_8)(Count, ppPush);
	}
	xbox::hresult_xt result = 0;

}

extern DWORD NV2A_stateFlags;

// ******************************************************************
// * patch: D3DDevice_BeginPushBuffer
// TODO: Find a test case and verify this
// Starting from XDK 4531, this changed to 1 parameter only.
// Is this definition incorrect, or did it change at some point?
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_BeginPushBuffer)(dword_xt * pPush)
{
	LOG_FUNC_ONE_ARG(pPush);

	XB_TRMP(D3DDevice_BeginPushBuffer)(pPush);
	// dry NV2A state X_STATE_RECORDPUSHBUFFER flags
	NV2A_stateFlags |= X_STATE_RECORDPUSHBUFFER;
	if (g_pXbox_BeginPush_Buffer != nullptr)
	{
		EmuLog(LOG_LEVEL::WARNING, "D3DDevice_BeginPush called without D3DDevice_EndPush in between?!");
		delete[] g_pXbox_BeginPush_Buffer; // prevent a memory leak
	}

	//dword_xt *pRet = new dword_xt[Count];
	//we should setup an PFIFO pusher interceptor to record the pushbuffer
	//this won't work in any near future if we keep patching any D3D functions which might be recorded in the pushbuffer.
	g_pXbox_BeginPush_Buffer = pPush;

	//return pRet;
	xbox::hresult_xt result = 0;
	return result;
}
//global for pfifo_run_pusher() to indicate whether it has completed the pushbuffer pasing or not.
bool g_nv2a_fifo_is_busy = false;
extern NV2ADevice* g_NV2A;// tmp glue
extern bool is_nv2a_pfifo_busy(NV2AState* d);//tmp glue

void EmuKickOff(void)
{
#if !PGRAPHUSE_EmuKickOff
	return;
#endif

	DWORD * pXbox_D3DDevice;
	DWORD  Xbox_D3DDevice;
	// Set g_Xbox_D3DDevice to point to the Xbox D3D Device
	// g_Xbox_D3DDevice is supposed to be set in D3DDevice_CreateDevice(), g_Xbox_D3DDevice to point to the Xbox D3D Device

	Xbox_D3DDevice = *g_pXbox_D3DDevice;
	if (XB_TRMP(CDevice_KickOff)) {
		__asm {
			//XB_TRAMPOLINE_D3DDevice_KickOff() require D3DDEVICE in ecx as this pointer.
			mov  ecx, Xbox_D3DDevice
		}
		XB_TRMP(CDevice_KickOff)();
	}
	else if (XB_TRMP(CDevice_KickOff_4)) {
		XB_TRMP(CDevice_KickOff_4)(Xbox_D3DDevice);
	}
	//LTCG version, CDevice in edx
	else if (XB_TRMP(CDevice_KickOff_0__LTCG_edx1)) {
		__asm {
			//XB_TRAMPOLINE_D3DDevice_KickOff() require D3DDEVICE in ecx as this pointer.
			mov  edx, Xbox_D3DDevice
		}
		XB_TRMP(CDevice_KickOff_0__LTCG_edx1)();
	}
	else if (XB_TRMP(CDevice_KickOff_0__LTCG_eax1)) {
		__asm {
			//XB_TRAMPOLINE_D3DDevice_KickOff() require D3DDEVICE in ecx as this pointer.
			mov  eax, Xbox_D3DDevice
		}
		XB_TRMP(CDevice_KickOff_0__LTCG_eax1)();
	}
	else
		CxbxrAbort("EmuKickOff:CDevice_KickOff not available!!!");
}

void EmuKickOffWait(void)
{
#if !PGRAPHUSE_EmuKickOffWait
	return;
#endif
	NV2AState* d = g_NV2A->GetDeviceState();
	EmuKickOff();
	//while (g_nv2a_fifo_is_busy) {
	while (is_nv2a_pfifo_busy(d)) {
		//__asm {
			//mov  ecx, Xbox_D3DDevice
		//}
		// KickOff xbox d3d pushbuffer just in case pfifo_pusher_thread() gets trapped in qemu_cond_wait(). 
		EmuKickOff();
	}
}
static DWORD* pFixupInfoBase = nullptr;
static DWORD* pFixupInfoIntrCount = nullptr;
DWORD* pgraph_get_xbox_fixupInfo_address(DWORD index) {
	
	if(pFixupInfoBase==nullptr)
		pFixupInfoBase=(DWORD*)(*g_pXbox_D3DDevice + 0x264);
	    pFixupInfoIntrCount= (DWORD*)(*g_pXbox_D3DDevice + 0x50);
		if (pFixupInfoBase[1] != 0 && pFixupInfoBase[0] == 0) {
			pFixupInfoBase--;
			pFixupInfoIntrCount--;
		}
		return (DWORD*)((DWORD)pFixupInfoBase + index*20);
}

void pgraph_dec_xbox_fixupInfoIntrCount(void)
{
	if(pFixupInfoIntrCount!=nullptr)
		*pFixupInfoIntrCount= *pFixupInfoIntrCount-1;
}

void D3DBuffer_ApplyFixup
(
	DWORD* pStart,
	DWORD* pFixupData,
	DWORD* pReturnOffset,
	DWORD ReturnAddress
)
{
	if (pFixupData != nullptr) {
		DWORD FixupSize = pFixupData[0];
		DWORD FixupOffset = pFixupData[1];
		void* src = &pFixupData[2];
		void* dst = (void*)((DWORD)pStart + FixupOffset);
		while (FixupSize != 0xFFFFFFFF) {
			memcpy(dst, src, FixupSize);
			pFixupData = (DWORD*)((DWORD)pFixupData + 4 + 4 + FixupSize);
			FixupSize = pFixupData[0];
			FixupOffset = pFixupData[1];
			src = &pFixupData[2];
			dst = (void*)((DWORD)pStart + FixupOffset);
		}
	}
	//setup return address jump. 
	*pReturnOffset = ReturnAddress | 1;
}
// ******************************************************************
// * patch: D3DDevice_EndPush
// ******************************************************************
xbox::dword_xt* WINAPI xbox::EMUPATCH(D3DDevice_EndPush)(dword_xt *pPush)
{
	LOG_FUNC_ONE_ARG(pPush);

	//this is a tmp patch before we remove all HLE patches. only to prevent multi entrance confliction
	//1st we trampoline back to guest code D3DDevice_EndPush()
	xbox::dword_xt* result = XB_TRMP(D3DDevice_EndPush)(pPush);
	//in D3DDevice_EndPush() before it returns, it has g_pDevice in ECX, which we need it to pass to XB_TRMP(D3DDevice_KickOff)() as this pointer.
	//DWORD * Xbox_D3DDevice;
	//__asm {
	//	mov  Xbox_D3DDevice, ecx
	//}
	
	//2nd we wait for pfifo_run_pusher() to complete the pushbuffer paring, then we return to guest code.
	//with this wait, we can make sure the pfifo_run_pusher()running in another thread won't conflict with the following guest code we're going to run.
	//this wait is not necessary once we remove all HLE patches.
	// Set pushbuffer parsing busy flag. pfifo_run_pusher() will set this to false once it complete the pushbuffer parsing.
	// g_nv2a_fifo_is_busy = true;//set this flag only after we trampoline the D3DDevice_EndPush, make sure pfifo starts parsing pushbuffer before we set this flag to true.
	
	//	we can't use this while loop to wait for the global flag. because XB_TRMP(D3DDevice_EndPush)(pPush) is only update g_pDevice->pPush, not the hardware mPut.
	//  hardware mPut will only be update by Kickoff(), which will be called inside MakeRequestedSpace(). Only when a segment of pushbuffer was switched, then the hardware mPut will be update.
	//  we have to figure out a way to force a kickoff here.
	//__asm {
	//	mov  ecx, Xbox_D3DDevice
	//}
	// KickOff xbox d3d pushbuffer first. 
	// this EmuKickOffWait() must be called in order to keep pushbuffer/HLE from race condition.
	EmuKickOff();

	return result;
}

// ******************************************************************
// * patch: D3DDevice_EndPushBuffer
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_EndPushBuffer)(void)
{

	XB_TRMP(D3DDevice_EndPushBuffer)();
	// clear NV2A state X_STATE_RECORDPUSHBUFFER flags
	NV2A_stateFlags &= ~X_STATE_RECORDPUSHBUFFER;
	if (g_pXbox_BeginPush_Buffer == nullptr)
		EmuLog(LOG_LEVEL::WARNING, "D3DDevice_EndPush called without preceding D3DDevice_BeginPush?!");
	else
	{
		// Note: We don't use the count from BeginPush because that specifies the *maximum* count
		// rather than the count actually in the pushbuffer.
		// D3DDevice_BeginPush and D3DDevice_EndPush are used to RECORD a pushbuffer, not to execute a pushbuffer.
		//EmuExecutePushBufferRaw(g_pXbox_BeginPush_Buffer, (uintptr_t)pPush - (uintptr_t)g_pXbox_BeginPush_Buffer);
		//we should setup an PFIFO pusher interceptor to record the pushbuffer, and stop here then setup the size of recorded pushbuffer.
		//this won't work in any near future if we keep patching any D3D functions which might be recorded in the pushbuffer.
		//for now, the pPush-> shall be cleared, or atleat the pPush->size shall be zero out to prevent errors in executing pPush in later guest codes.
		//delete[] g_pXbox_BeginPush_Buffer;
		g_pXbox_BeginPush_Buffer = nullptr;
	}
}

// ******************************************************************
// * patch: D3DDevice_BeginVisibilityTest
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_BeginVisibilityTest)()
{
	LOG_FUNC();
	//template for syncing HLE apis with pgraf using waiting lock
	bool WaitForPGRAPH;
	WaitForPGRAPH = true;
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)&WaitForPGRAPH;// (DWORD)PrimitiveType;

	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_BeginVisibilityTest, 1, PBTokenArray);//argCount, not necessary, default to 14

	EmuKickOff();

	while (WaitForPGRAPH)
		; //this line is must have
	if (g_bEnableHostQueryVisibilityTest) {
		// Create a D3D occlusion query to handle "visibility test" with
		IDirect3DQuery* pHostQueryVisibilityTest = nullptr;
		HRESULT hRet = g_pD3DDevice->CreateQuery(D3DQUERYTYPE_OCCLUSION, &pHostQueryVisibilityTest);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateQuery (visibility test)");
		if (pHostQueryVisibilityTest != nullptr) {
			hRet = pHostQueryVisibilityTest->Issue(D3DISSUE_BEGIN);
			DEBUG_D3DRESULT(hRet, "g_pHostQueryVisibilityTest->Issue(D3DISSUE_BEGIN)");
			if (SUCCEEDED(hRet)) {
				g_HostQueryVisibilityTests.push(pHostQueryVisibilityTest);
			} else {
				LOG_TEST_CASE("Failed to issue query");
				pHostQueryVisibilityTest->Release();
			}

			pHostQueryVisibilityTest = nullptr;
		}
	}

	return D3D_OK;
}

xbox::hresult_xt WINAPI CxbxrImpl_EndVisibilityTest
(
	xbox::dword_xt  Index
)
{
	if (g_bEnableHostQueryVisibilityTest) {
		// Check that the dedicated storage for the given Index isn't in use
		if (g_HostVisibilityTestMap[Index] != nullptr) {
			return E_OUTOFMEMORY;
		}

		if (g_HostQueryVisibilityTests.empty()) {
			return 2088; // visibility test incomplete (a prior BeginVisibilityTest call is needed)
		}

		IDirect3DQuery* pHostQueryVisibilityTest = g_HostQueryVisibilityTests.top();
		g_HostQueryVisibilityTests.pop();
		assert(pHostQueryVisibilityTest != nullptr);

		HRESULT hRet = pHostQueryVisibilityTest->Issue(D3DISSUE_END);
		//DEBUG_D3DRESULT(hRet, "g_pHostQueryVisibilityTest->Issue(D3DISSUE_END)");
		if (hRet == D3D_OK) {
			// Associate the result of this call with the given Index
			g_HostVisibilityTestMap[Index] = pHostQueryVisibilityTest;
		}
		else {
			LOG_TEST_CASE("Failed to issue query");
			pHostQueryVisibilityTest->Release();
		}
	}

	return D3D_OK;

}
// LTCG specific D3DDevice_EndVisibilityTest function...
// This uses a custom calling convention where parameter is passed in EAX
// Test-case: Test Drive: Eve of Destruction
__declspec(naked) xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_EndVisibilityTest_0)
(
)
{
    dword_xt Index;
	xbox::hresult_xt result;
    __asm {
        LTCG_PROLOGUE
        mov  Index, eax
    }

	XB_TRMP(D3DDevice_EndVisibilityTest_0)();
	//template for syncing HLE apis with pgraf using waiting lock
	bool WaitForPGRAPH;
	WaitForPGRAPH = true;
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)&WaitForPGRAPH;// (DWORD)PrimitiveType;

	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_EndVisibilityTest_0, 1, PBTokenArray);//argCount, not necessary, default to 14

	EmuKickOff();

	while (WaitForPGRAPH)
		; //this line is must have
	result = CxbxrImpl_EndVisibilityTest(Index);//EMUPATCH(D3DDevice_EndVisibilityTest)(Index);

    __asm {
        mov  eax, result
        LTCG_EPILOGUE
        ret
    }
}

// ******************************************************************
// * patch: D3DDevice_EndVisibilityTest
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_EndVisibilityTest)
(
    dword_xt                       Index
)
{
	LOG_FUNC_ONE_ARG(Index);

	XB_TRMP(D3DDevice_EndVisibilityTest)(Index);
	//template for syncing HLE apis with pgraf using waiting lock
	bool WaitForPGRAPH;
	WaitForPGRAPH = true;
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)&WaitForPGRAPH;// (DWORD)PrimitiveType;

	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_EndVisibilityTest, 1, PBTokenArray);//argCount, not necessary, default to 14

	EmuKickOff();

	while (WaitForPGRAPH)
		; //this line is must have
    return CxbxrImpl_EndVisibilityTest(Index);
}

void WINAPI CxbxrImpl_SetBackBufferScale(xbox::float_xt x, xbox::float_xt y)
{
	g_Xbox_BackbufferScaleX = x;
	g_Xbox_BackbufferScaleY = y;
}

// ******************************************************************
// * patch: D3DDevice_SetBackBufferScale
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetBackBufferScale)(float_xt x, float_xt y)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(x)
		LOG_FUNC_ARG(y)
		LOG_FUNC_END;


	// always trampoline
	XB_TRMP(D3DDevice_SetBackBufferScale)(x, y);
	/*
	xbox SetBackBufferScale takes effect on aaScaleX/ssScaleY, which will be multiplied to render starge width/height
	SuperSampleScaleX/Y are aaSaleX/Y multiplied by 0.5 if super sample mode was set. width/height will also be devided by 2 when super sample mode was set.
	then it calls SetViwport(0) which resulted in calling SetScissors(0,0,0) which resulted in updating the scaled width/height be set to SetSurfaceClip,
	and then it scaled the internal Viewport to SetWindowClip
	*/
	
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = FtoDW(x);
	PBTokenArray[3] = FtoDW(y);
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetBackBufferScale, 2, PBTokenArray);//argCount 14
}

// ******************************************************************
// * patch: D3DDevice_GetVisibilityTestResult
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_GetVisibilityTestResult)
(
    dword_xt                       Index,
    uint_xt                       *pResult,
    ulonglong_xt                  *pTimeStamp
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Index)
		LOG_FUNC_ARG(pResult)
		LOG_FUNC_ARG(pTimeStamp)
		LOG_FUNC_END;

	if (g_bEnableHostQueryVisibilityTest) {
		IDirect3DQuery* pHostQueryVisibilityTest = g_HostVisibilityTestMap[Index];
		if (pHostQueryVisibilityTest == nullptr) {
			return E_OUTOFMEMORY;
		}

		// In order to prevent an endless loop if the D3D device becomes lost, we pass
		// the D3DGETDATA_FLUSH flag. This tells GetData to return D3DERR_DEVICELOST if
		// such a situation occurs, and break out of the loop as a result.
		// Note: By Cxbx's design, we cannot do drawing within this while loop in order
		// to further prevent any other endless loop situations.
		while (S_FALSE == pHostQueryVisibilityTest->GetData(pResult, sizeof(DWORD), D3DGETDATA_FLUSH));

		g_HostVisibilityTestMap[Index] = nullptr;
		pHostQueryVisibilityTest->Release();
	} else {
		// Fallback to old faked result when there's no host occlusion query :
		if (pResult != xbox::zeroptr) {
			*pResult = 640 * 480; // TODO : Use actual backbuffer dimensions
		}
	}

	if (pTimeStamp != xbox::zeroptr) {
		LOG_TEST_CASE("requested value for pTimeStamp");
		*pTimeStamp = sizeof(DWORD); // TODO : This should be an incrementing GPU-memory based DWORD-aligned memory address
	}

    return D3D_OK;
}

// Overload for logging
static void D3DDevice_LoadVertexShader_0__LTCG_eax_Address_ecx_Handle
(
    xbox::dword_xt                 Handle,
    xbox::dword_xt                 Address
)
{
    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(Handle)
        LOG_FUNC_ARG(Address)
    LOG_FUNC_END;
}

// LTCG specific D3DDevice_LoadVertexShader function...
// This uses a custom calling convention where parameter is passed in EAX, ECX
// Test-case: Aggressive Inline
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_LoadVertexShader_0__LTCG_eax_Address_ecx_Handle)
(
)
{
    dword_xt Handle;
    dword_xt Address;
    __asm {
        LTCG_PROLOGUE
        mov  Address, eax
        mov  Handle, ecx
		call XB_TRMP(D3DDevice_LoadVertexShader_0__LTCG_eax_Address_ecx_Handle)
    }
	
    // Log
    D3DDevice_LoadVertexShader_0__LTCG_eax_Address_ecx_Handle(Handle, Address);

    //CxbxrImpl_LoadVertexShader(Handle, Address);

	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)Handle;
	PBTokenArray[3] = (DWORD)Address;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_LoadVertexShader_0__LTCG_eax_Address_ecx_Handle, 2, PBTokenArray);//argCount, not necessary, default to 14

    __asm {
        LTCG_EPILOGUE
        ret
    }
}

// Overload for logging
static void D3DDevice_LoadVertexShader_0__LTCG_eax_Address_edx_Handle
(
    xbox::dword_xt                 Handle,
    xbox::dword_xt                 Address
)
{
    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(Handle)
        LOG_FUNC_ARG(Address)
    LOG_FUNC_END;
}

// LTCG specific D3DDevice_LoadVertexShader function...
// This uses a custom calling convention where parameter is passed in EAX, EDX
// Test-case: World Racing 2, Project Zero 2 (PAL)
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_LoadVertexShader_0__LTCG_eax_Address_edx_Handle)
(
)
{
    dword_xt Handle;
    dword_xt Address;
    __asm {
        LTCG_PROLOGUE
        mov  Address, eax
        mov  Handle, edx
		call XB_TRMP(D3DDevice_LoadVertexShader_0__LTCG_eax_Address_edx_Handle)
    }

	// Log
    D3DDevice_LoadVertexShader_0__LTCG_eax_Address_edx_Handle(Handle, Address);

    //CxbxrImpl_LoadVertexShader(Handle, Address);

	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)Handle;
	PBTokenArray[3] = (DWORD)Address;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_LoadVertexShader_0__LTCG_eax_Address_edx_Handle, 2, PBTokenArray);//argCount, not necessary, default to 14

    __asm {
        LTCG_EPILOGUE
        ret
    }
}

// Overload for logging
static void D3DDevice_LoadVertexShader_4
(
    xbox::dword_xt                 Handle,
    xbox::dword_xt                 Address
)
{
    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(Handle)
        LOG_FUNC_ARG(Address)
    LOG_FUNC_END;
}

// This uses a custom calling convention where parameter is passed in EAX
// Test-case: Ninja Gaiden
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_LoadVertexShader_4)
(
    dword_xt                       Address
)
{
    dword_xt Handle;
    __asm {
        LTCG_PROLOGUE
        mov  Handle, eax
    }

	XB_TRMP(D3DDevice_LoadVertexShader_4)(Address);

	// Log
    D3DDevice_LoadVertexShader_4(Handle, Address);

    //CxbxrImpl_LoadVertexShader(Handle, Address);

	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)Handle;
	PBTokenArray[3] = (DWORD)Address;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_LoadVertexShader_4, 2, PBTokenArray);//argCount, not necessary, default to 14

    __asm {
        LTCG_EPILOGUE
        ret  4
    }
}

// ******************************************************************
// * patch: D3DDevice_LoadVertexShader
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_LoadVertexShader)
(
    dword_xt                       Handle,
    dword_xt                       Address
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Handle)
		LOG_FUNC_ARG(Address)
	LOG_FUNC_END;

	XB_TRMP(D3DDevice_LoadVertexShader)(Handle, Address);

#if !USEPGRAPH_LoadVertexShader
	CxbxrImpl_LoadVertexShader(Handle, Address);
#else
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)Handle;
	PBTokenArray[3] = (DWORD)Address;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_LoadVertexShader, 2, PBTokenArray);//argCount, not necessary, default to 14
#endif
}

// Overload for logging
static void D3DDevice_SelectVertexShader_0__LTCG_eax1_ebx2
(
    xbox::dword_xt                 Handle,
    xbox::dword_xt                 Address
)
{
    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(Handle)
        LOG_FUNC_ARG(Address)
        LOG_FUNC_END;
#if USEPGRAPH_SelectVertexShader
	
#endif
}

// LTCG specific D3DDevice_SelectVertexShader function...
// This uses a custom calling convention where parameter is passed in EAX, EBX
// Test-case: Star Wars - Battlefront
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SelectVertexShader_0__LTCG_eax1_ebx2)
(
)
{
    dword_xt Handle;
    dword_xt Address;
    __asm {
        LTCG_PROLOGUE
        mov  Handle, eax
        mov  Address, ebx
		call XB_TRMP(D3DDevice_SelectVertexShader_0__LTCG_eax1_ebx2)
    }
	// Log
    D3DDevice_SelectVertexShader_0__LTCG_eax1_ebx2(Handle, Address);
#if !USEPGRAPH_SelectVertexShader
    CxbxrImpl_SelectVertexShader(Handle, Address);
#else
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)Handle;
	PBTokenArray[3] = (DWORD)Address;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SelectVertexShader_0__LTCG_eax1_ebx2, 2, PBTokenArray);//argCount, not necessary, default to 14
#endif
    __asm {
        LTCG_EPILOGUE
        ret
    }
}

// Overload for logging
static void D3DDevice_SelectVertexShader_4__LTCG_eax1
(
    xbox::dword_xt                 Handle,
    xbox::dword_xt                 Address
)
{
    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(Handle)
        LOG_FUNC_ARG(Address)
        LOG_FUNC_END;

}

// LTCG specific D3DDevice_SelectVertexShader function...
// This uses a custom calling convention where parameter is passed in EAX
// Test-case: Aggressive Inline
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SelectVertexShader_4__LTCG_eax1)
(
    dword_xt                       Address
)
{
    dword_xt Handle;
    __asm {
        LTCG_PROLOGUE
        mov  Handle, eax
    }

	XB_TRMP(D3DDevice_SelectVertexShader_4__LTCG_eax1)(Address);
    // Log
    D3DDevice_SelectVertexShader_4__LTCG_eax1(Handle, Address);

#if !USEPGRAPH_SelectVertexShader
	CxbxrImpl_SelectVertexShader(Handle, Address);
#else
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)Handle;
	PBTokenArray[3] = (DWORD)Address;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SelectVertexShader_4__LTCG_eax1, 2, PBTokenArray);//argCount, not necessary, default to 14
#endif

    __asm {
        LTCG_EPILOGUE
        ret  4
    }
}

// ******************************************************************
// * patch: D3DDevice_SelectVertexShader
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SelectVertexShader)
(
    dword_xt                       Handle,
    dword_xt                       Address
)
{
    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(Handle)
        LOG_FUNC_ARG(Address)
        LOG_FUNC_END;

    XB_TRMP(D3DDevice_SelectVertexShader)(Handle, Address);

#if !USEPGRAPH_SelectVertexShader
	CxbxrImpl_SelectVertexShader(Handle, Address);
#else
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)Handle;
	PBTokenArray[3] = (DWORD)Address;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SelectVertexShader, 2, PBTokenArray);//argCount, not necessary, default to 14

#endif
	
}
void CxbxrImpl__SetGammaRamp(
	xbox::dword_xt                   dwFlags,
	CONST D3DGAMMARAMP * pRamp
)
{
	// remove D3DSGR_IMMEDIATE
	DWORD dwPCFlags = D3DSGR_NO_CALIBRATION;// dwFlags& (~0x00000002);

#if 1 // TODO : Why is this disabled?
	g_pD3DDevice->SetGammaRamp(
		0, // iSwapChain
		dwPCFlags, pRamp);

	//g_pD3DDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
	//g_pD3DDevice->SetSamplerState(0, D3DSAMP_SRGBTEXTURE,true);

#endif

}
// ******************************************************************
// * patch: D3DDevice_SetGammaRamp
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetGammaRamp)
(
    dword_xt                   dwFlags,
    CONST X_D3DGAMMARAMP   *pRamp
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(dwFlags)
		LOG_FUNC_ARG(pRamp)
		LOG_FUNC_END;

    static D3DGAMMARAMP PCRamp;

    for(int v=0;v<256;v++)
    {
        PCRamp.red[v]   = pRamp->red[v];
        PCRamp.green[v] = pRamp->green[v];
        PCRamp.blue[v]  = pRamp->blue[v];
    }
	bool WaitForPGRAPH;
	WaitForPGRAPH = true;
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)dwFlags;
	PBTokenArray[3] = (DWORD)&PCRamp;
	PBTokenArray[4] = (DWORD)&WaitForPGRAPH;

	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetGammaRamp, 3, PBTokenArray);//argCount, not necessary, default to 14

	EmuKickOff();

	while (WaitForPGRAPH)
		; //this line is must have
	WaitForPGRAPH = false;
}

// ******************************************************************
// * patch: D3DDevice_GetGammaRamp
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_GetGammaRamp)
(
    X_D3DGAMMARAMP     *pRamp
)
{
	LOG_FUNC_ONE_ARG(pRamp);
	static D3DGAMMARAMP PCGammaRamp = { 0 };
	D3DGAMMARAMP* pGammaRamp = &PCGammaRamp;// (D3DGAMMARAMP*)malloc(sizeof(D3DGAMMARAMP));

	bool WaitForPGRAPH;
	WaitForPGRAPH = true;
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)&WaitForPGRAPH;

	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_GetGammaRamp, 3, PBTokenArray);//argCount, not necessary, default to 14

	EmuKickOff();

	while (WaitForPGRAPH)
		; //this line is must have


    g_pD3DDevice->GetGammaRamp(
		0, // iSwapChain
		pGammaRamp);

    for(int v=0;v<256;v++)
    {
        pRamp->red[v] = (BYTE)pGammaRamp->red[v];
        pRamp->green[v] = (BYTE)pGammaRamp->green[v];
        pRamp->blue[v] = (BYTE)pGammaRamp->blue[v];
    }

	//free(pGammaRamp);
}


#define COPY_BACKBUFFER_TO_XBOX_SURFACE // Uncomment to enable writing Host Backbuffers back to Xbox surfaces
xbox::X_D3DSurface* CxbxrImpl_GetBackBuffer2
(
    xbox::int_xt BackBuffer
)
{
	xbox::X_D3DSurface* pXboxBackBuffer = nullptr;


#ifndef COPY_BACKBUFFER_TO_XBOX_SURFACE
	/** unsafe, somehow
	HRESULT hRet = D3D_OK;

	X_D3DSurface *pXboxBackBuffer = EmuNewD3DSurface();

	if(BackBuffer == -1) {
		static IDirect3DSurface *pCachedPrimarySurface = nullptr;

		if(pCachedPrimarySurface == nullptr) {
			// create a buffer to return
			// TODO: Verify the surface is always 640x480
			hRet = g_pD3DDevice->CreateOffscreenPlainSurface(640, 480, D3DFMT_A8R8G8B8, /*D3DPool=* /0, &pCachedPrimarySurface, nullptr);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateOffscreenPlainSurface");
		}

		SetHostSurface(pXboxBackBuffer, pCachedPrimarySurface); // No iTextureStage!

		hRet = g_pD3DDevice->GetFrontBuffer(pCachedPrimarySurface);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetFrontBuffer");

		if (FAILED(hRet)) {
			EmuLog(LOG_LEVEL::WARNING, "Could not retrieve primary surface, using backbuffer");
			SetHostSurface(pXboxBackBuffer, nullptr); // No iTextureStage!
			pCachedPrimarySurface->Release();
			pCachedPrimarySurface = nullptr;
			BackBuffer = 0;
		}

		// Debug: Save this image temporarily
		//D3DXSaveSurfaceToFile("C:\\Aaron\\Textures\\FrontBuffer.bmp", D3DXIFF_BMP, GetHostSurface(pXboxBackBuffer), nullptr, nullptr);
	}

	if(BackBuffer != -1) {
		hRet = g_pD3DDevice->GetBackBuffer(
			0, // iSwapChain
			BackBuffer, D3DBACKBUFFER_TYPE_MONO, &pCachedPrimarySurface);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetBackBuffer");
	}
	//*/

	static X_D3DSurface *pXboxBackBuffer = EmuNewD3DSurface();
	IDirect3DSurface *pCurrentHostBackBuffer = nullptr;

	 if (BackBuffer == -1) {
		 BackBuffer = 0;
	 }

	HRESULT hRet = g_pD3DDevice->GetBackBuffer(
		0, // iSwapChain
		BackBuffer, D3DBACKBUFFER_TYPE_MONO, &pCurrentHostBackBuffer);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetBackBuffer");

	if (FAILED(hRet))
		CxbxrAbort("Unable to retrieve back buffer");

	SetHostSurface(pXboxBackBuffer, pCurrentHostBackBuffer); // No iTextureStage!

	// Increment reference count
	pXboxBackBuffer->Common++; // EMUPATCH(D3DResource_AddRef)(pXboxBackBuffer);

	return pXboxBackBuffer;
#else // COPY_BACKBUFFER_TO_XBOX_SURFACE
	// Rather than create a new surface, we should forward to the Xbox version of GetBackBuffer,
	// This gives us the correct Xbox surface to update.
	// We get signatures for both backbuffer functions as it changed in later XDKs

	// This also updates the reference count, so we don't need to do this ourselves
	if (XB_TRMP(D3DDevice_GetBackBuffer) != nullptr) {
		XB_TRMP(D3DDevice_GetBackBuffer)(BackBuffer, D3DBACKBUFFER_TYPE_MONO, &pXboxBackBuffer);
	}
	else if (XB_TRMP(D3DDevice_GetBackBuffer2) != nullptr) {
		pXboxBackBuffer = XB_TRMP(D3DDevice_GetBackBuffer2)(BackBuffer);
	}
	else if(XB_TRMP(D3DDevice_GetBackBuffer2_0__LTCG_eax1)!=nullptr) {
		__asm {
			mov  eax, BackBuffer
			call XB_TRMP(D3DDevice_GetBackBuffer2_0__LTCG_eax1)
			mov pXboxBackBuffer, eax
		}
	}

	// Now pXboxBackbuffer points to the requested Xbox backbuffer
	if (pXboxBackBuffer == nullptr) {
		CxbxrAbort("D3DDevice_GetBackBuffer2: Could not get Xbox backbuffer");
	}


    // HACK: Disabled: Enabling this breaks DOA3 at native res/without hacks+
    // Also likely to effect Other games, but it has no known benefit at this point in time
    // There are currently no known games that depend on backbuffer readback on the CPU!
#if 0
	// TODO: Downscale the host surface to the same size as the Xbox surface during copy
	// Otherwise, we will overflow memory and crash
	// HACK: For now, when using a non-zero scale factor, we can just skip the copy to prevent a crash
	if (g_RenderScaleFactor == 1) {
		auto pCopySrcSurface = GetHostSurface(pXboxBackBuffer, D3DUSAGE_RENDERTARGET);
		if (pCopySrcSurface == nullptr) {
			EmuLog(LOG_LEVEL::WARNING, "Failed to get Host Resource for Xbox Back Buffer");
			return pXboxBackBuffer;
		}

		D3DLOCKED_RECT copyLockedRect;
		HRESULT hRet = pCopySrcSurface->LockRect(&copyLockedRect, NULL, D3DLOCK_READONLY);
		if (hRet != D3D_OK) {
			EmuLog(LOG_LEVEL::WARNING, "Could not lock Host Resource for Xbox Back Buffer");
			return pXboxBackBuffer;
		}

		D3DSURFACE_DESC copySurfaceDesc;
		hRet = pCopySrcSurface->GetDesc(&copySurfaceDesc);
		if (hRet != D3D_OK) {
			EmuLog(LOG_LEVEL::WARNING, "Could not get Xbox Back Buffer Host Surface Desc");
		}
		else {
			DWORD Size = copyLockedRect.Pitch * copySurfaceDesc.Height; // TODO : What about mipmap levels? (Backbuffer does not support mipmap)
			// Finally, do the copy from the converted host resource to the xbox resource
			memcpy((void*)GetDataFromXboxResource(pXboxBackBuffer), copyLockedRect.pBits, Size);
		}

		pCopySrcSurface->UnlockRect();
	}
#endif

	return pXboxBackBuffer;
#endif // COPY_BACKBUFFER_TO_XBOX_SURFACE
}

// ******************************************************************
// * patch: D3DDevice_GetBackBuffer2
// ******************************************************************
xbox::X_D3DSurface* WINAPI xbox::EMUPATCH(D3DDevice_GetBackBuffer2)
(
	int_xt                 BackBuffer
	)
{
	LOG_FUNC_ONE_ARG(BackBuffer);

	//return CxbxrImpl_GetBackBuffer2(BackBuffer);
	CxbxrImpl_Resource_AddRef(g_pXbox_BackBufferSurface);
	return g_pXbox_BackBufferSurface;
}

static void D3DDevice_GetBackBuffer2_0__LTCG_eax1(xbox::int_xt BackBuffer)
{
	LOG_FUNC_ONE_ARG(BackBuffer);
}

// LTCG specific GetBackBuffer2 function...
// This uses a custom calling convention where parameter is passed in EAX
__declspec(naked) xbox::X_D3DSurface* WINAPI xbox::EMUPATCH(D3DDevice_GetBackBuffer2_0__LTCG_eax1)
(
)
{

	int_xt BackBuffer;
	xbox::X_D3DSurface* pBackBuffer;
	__asm {
		LTCG_PROLOGUE
		mov  BackBuffer, eax
	}

	// Log
	D3DDevice_GetBackBuffer2_0__LTCG_eax1(BackBuffer);

	//pBackBuffer = CxbxrImpl_GetBackBuffer2(BackBuffer);
	CxbxrImpl_Resource_AddRef(g_pXbox_BackBufferSurface);
	pBackBuffer = g_pXbox_BackBufferSurface;

	__asm {
		mov  eax, pBackBuffer
		LTCG_EPILOGUE
		ret
	}
}

// ******************************************************************
// * patch: D3DDevice_GetBackBuffer
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_GetBackBuffer)
(
    int_xt                 BackBuffer,
    D3DBACKBUFFER_TYPE  Type,
    X_D3DSurface      **ppBackBuffer
)
{
	LOG_FORWARD("D3DDevice_GetBackBuffer2");
	CxbxrImpl_Resource_AddRef(g_pXbox_BackBufferSurface);
	*ppBackBuffer = g_pXbox_BackBufferSurface;// CxbxrImpl_GetBackBuffer2(BackBuffer);
}

bool GetHostRenderTargetDimensions(DWORD *pHostWidth, DWORD *pHostHeight, IDirect3DSurface* pHostRenderTarget = nullptr)
{
	bool shouldRelease = false;
	if (pHostRenderTarget == nullptr) {
		g_pD3DDevice->GetRenderTarget(
			0, // RenderTargetIndex
			&pHostRenderTarget);

		shouldRelease = true;
	}

	// The following can only work if we could retrieve a host render target
	if (!pHostRenderTarget) {
		return false;
	}

	// Get current host render target dimensions
	D3DSURFACE_DESC HostRenderTarget_Desc;
	pHostRenderTarget->GetDesc(&HostRenderTarget_Desc);

	if (shouldRelease) {
		pHostRenderTarget->Release();
	}

	*pHostWidth = HostRenderTarget_Desc.Width;
	*pHostHeight = HostRenderTarget_Desc.Height;

	return true;
}

DWORD ScaleDWORD(DWORD Value, DWORD FromMax, DWORD ToMax)
{
	uint64_t tmp = Value;
	tmp *= ToMax;
	tmp /= FromMax;
	return (DWORD)tmp;
}

void ValidateRenderTargetDimensions(DWORD HostRenderTarget_Width, DWORD HostRenderTarget_Height, DWORD XboxRenderTarget_Width, DWORD XboxRenderTarget_Height)
{
    // This operation is often used to change the display resolution without calling SetRenderTarget!
    // This works by updating the underlying Width & Height of the Xbox surface, without reallocating the data
    // Because of this, we need to validate that the associated host resource still matches the dimensions of the Xbox Render Target
    // If not, we must force them to be re-created
    // TEST CASE: Chihiro Factory Test Program
    DWORD XboxRenderTarget_Width_Scaled = XboxRenderTarget_Width * g_RenderUpscaleFactor;
    DWORD XboxRenderTarget_Height_Scaled = XboxRenderTarget_Height * g_RenderUpscaleFactor;
    if (HostRenderTarget_Width != XboxRenderTarget_Width_Scaled || HostRenderTarget_Height != XboxRenderTarget_Height_Scaled) {
        LOG_TEST_CASE("Existing RenderTarget width/height changed");

        FreeHostResource(GetHostResourceKey(g_pXbox_RenderTarget)); g_pD3DDevice->SetRenderTarget(0, GetHostSurface(g_pXbox_RenderTarget, D3DUSAGE_RENDERTARGET));
        FreeHostResource(GetHostResourceKey(g_pXbox_DepthStencil)); g_pD3DDevice->SetDepthStencilSurface(GetHostSurface(g_pXbox_DepthStencil, D3DUSAGE_DEPTHSTENCIL));
    }
}

float GetZScaleForPixelContainer(xbox::X_D3DPixelContainer* pSurface)
{
    // If no surface was present, fallback to 1
    if (pSurface == xbox::zeroptr) {
        return 1.0f;
    }

    auto format = GetXboxPixelContainerFormat(pSurface);
    switch (format) {
        case xbox::X_D3DFMT_D16:
        case xbox::X_D3DFMT_LIN_D16:
            return 65535.0f;

        case xbox::X_D3DFMT_D24S8:
        case xbox::X_D3DFMT_LIN_D24S8:
            return 16777215.0f;

        case xbox::X_D3DFMT_F16:
        case xbox::X_D3DFMT_LIN_F16:
            return 511.9375f;

        case xbox::X_D3DFMT_F24S8:
        case xbox::X_D3DFMT_LIN_F24S8:
            // 24bit floating point is close to precision maximum, so a lower value is used
            // We can't use a double here since the vertex shader is only at float precision
            return 1.0e30f; 
    }

    // Default to 1 if unknown depth format
    LOG_TEST_CASE("GetZScaleForSurface: Unknown Xbox Depth Format");
    return 1.0f;
}

extern float* HLE_get_NV2A_vertex_constant_float4_ptr(unsigned const_index); //tmp glue

// Get viewport offset and scale values intended to match the XDK calculations
// We should remove this when we can get the real viewport values from the xbox reliably via NV2A LLE
void GetXboxViewportOffsetAndScale(float (&vOffset)[4], float(&vScale)[4])
{
	if (is_pgraph_using_NV2A_Kelvin) {
		float* fptr = HLE_get_NV2A_vertex_constant_float4_ptr(96 - 38);
		vScale[0] = fptr[0];
		vScale[1] = fptr[1];
		vScale[2] = fptr[2];
		// todo, in this function, it sets default scale[3] as 1.0, but xbox api actually set scale[3] as 0.0 in vsh_constants[][].
		// need to verify whether 1.0 or 0.0 is correct.
		vScale[3] = fptr[3];// or vScale[3]=1.0?
		fptr = HLE_get_NV2A_vertex_constant_float4_ptr(96 - 37);
		vOffset[0] = fptr[0];
		vOffset[1] = fptr[1];
		vOffset[2] = fptr[2];
		vOffset[3] = fptr[3];
		return;
	}
	// Antialiasing mode affects the viewport offset and scale
	float aaScaleX, aaScaleY;
	float aaOffsetX, aaOffsetY;
	//GetScreenScaleFactors() returns scales from kelvin when we're in pgraph draw calls
	GetScreenScaleFactors(aaScaleX, aaScaleY);
	GetMultiSampleOffset(aaOffsetX, aaOffsetY);

	float scaledX = g_Xbox_Viewport.X * aaScaleX;
	float scaledY = g_Xbox_Viewport.Y * aaScaleY;
	float scaledWidth = g_Xbox_Viewport.Width * aaScaleX;
	float scaledHeight = g_Xbox_Viewport.Height * aaScaleY;

	// D3D9 viewport info
	// https://docs.microsoft.com/en-us/windows/win32/direct3d9/viewports-and-clipping
	// Unlike D3D9, Xbox should scale by the z buffer scale
	// Test case: GTA III

	auto zRange = g_Xbox_Viewport.MaxZ - g_Xbox_Viewport.MinZ;
	vScale[0] = scaledWidth / 2;
	vScale[1] = -scaledHeight / 2;
	vScale[2] = zRange * g_ZScale;
	vScale[3] = 1;
	vOffset[0] = scaledWidth / 2 + scaledX;
	vOffset[1] = scaledHeight / 2 + scaledY;
	vOffset[2] = g_Xbox_Viewport.MinZ * g_ZScale;
	vOffset[3] = 0;
}

void CxbxUpdateHostViewPortOffsetAndScaleConstants()
{
    float vScaleOffset[2][4]; // 0 - scale 1 - offset
	// todo: use viewport offset and scale from pg->vsh_constants[-38+96][]
	GetXboxViewportOffsetAndScale(vScaleOffset[1], vScaleOffset[0]);

	// Xbox outputs vertex positions in rendertarget pixel coordinate space, with non-normalized Z
	// e.g. 0 < x < 640 and 0 < y < 480
	// We want to scale it back to normalized device coordinates i.e. XY are (-1, +1) and Z is (0, 1)

	// The screenspace is a combination of the rendertarget
	// and various scale factors
	// Get the rendertarget width and height
	float xboxRenderTargetWidth;
	float xboxRenderTargetHeight;
	GetRenderTargetBaseDimensions(xboxRenderTargetWidth, xboxRenderTargetHeight);

	float screenScaleX, screenScaleY;
	float aaOffsetX, aaOffsetY;
	//GetScreenScaleFactors() returns scales from kelvin when we're in pgraph draw calls
	GetScreenScaleFactors(screenScaleX, screenScaleY);
	GetMultiSampleOffset(aaOffsetX, aaOffsetY);

	// Add D3D9 half-pixel offset (-0.5 given this offset is subtracted)
	// We should be able to remove this when off D3D9
	// https://aras-p.info/blog/2016/04/08/solving-dx9-half-pixel-offset/
	aaOffsetX -= 0.5f;
	aaOffsetY -= 0.5f;

	float xboxScreenspaceWidth = xboxRenderTargetWidth * screenScaleX;
	float xboxScreenspaceHeight = xboxRenderTargetHeight * screenScaleY;

	// Passthrough should range 0 to 1, instead of 0 to zbuffer depth
	// Test case: DoA3 character select
	extern bool is_pgraph_using_NV2A_Kelvin(void);
	extern VertexShaderMode g_NV2A_VertexShaderMode;
	VertexShaderMode VSHMode = g_Xbox_VertexShaderMode;
	if (is_pgraph_using_NV2A_Kelvin())
		VSHMode = g_NV2A_VertexShaderMode;
	float zOutputScale = VSHMode == VertexShaderMode::Passthrough ? 1 : g_ZScale;

	float screenspaceScale[4] = { xboxScreenspaceWidth / 2,  -xboxScreenspaceHeight / 2, zOutputScale, 1 };
	float screenspaceOffset[4] = { xboxScreenspaceWidth / 2 + aaOffsetX, xboxScreenspaceHeight / 2 + aaOffsetY, 0, 0 };
	g_pD3DDevice->SetVertexShaderConstantF(CXBX_D3DVS_SCREENSPACE_SCALE_BASE, screenspaceScale, CXBX_D3DVS_NORMALIZE_SCALE_SIZE);
	g_pD3DDevice->SetVertexShaderConstantF(CXBX_D3DVS_SCREENSPACE_OFFSET_BASE, screenspaceOffset, CXBX_D3DVS_NORMALIZE_OFFSET_SIZE);

	// Store viewport offset and scale in constant registers 58 (c-38) and
	// 59 (c-37) used for screen space transformation.
	// We only do this if X_D3DSCM_NORESERVEDCONSTANTS is not set,
	// since enabling this flag frees up these registers for shader use
	// Treat this as a flag
	// Test Case: GTA III, Soldier of Fortune II
	if (!(g_Xbox_VertexShaderConstantMode & X_D3DSCM_NORESERVEDCONSTANTS)) {
		g_pD3DDevice->SetVertexShaderConstantF(X_D3DSCM_RESERVED_CONSTANT_SCALE_CORRECTED, reinterpret_cast<float*>(vScaleOffset), 2);
	}

}

// ******************************************************************
// * patch: D3DDevice_SetViewport
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetViewport)
(
	X_D3DVIEWPORT8 *pViewport
)
{
	LOG_FUNC_ONE_ARG(pViewport);

	// Always call the Xbox SetViewPort to update D3D Internal State
	XB_TRMP(D3DDevice_SetViewport)(pViewport);

#if !USEPGRAPH_SetViewport
	CxbxrImpl_SetViewport(pViewport);
#else
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	//special case, we pass the pViewport and it's content to pushbuffer.
	if (pViewport) {
		PBTokenArray[2] = (DWORD)&PBTokenArray[3]; //total 14 DWORD space for arguments.
		//extern xbox::X_D3DVIEWPORT8 g_Xbox_Viewport;
		//todo: we preserve the viewport for debug. could be removed in future.
		g_Xbox_Viewport=*pViewport;
		*(X_D3DVIEWPORT8*)&PBTokenArray[3] = g_Xbox_Viewport;
			
	}
	else {
		PBTokenArray[2] = (DWORD)pViewport;
	}
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetViewport, 7, PBTokenArray);//argCount, not necessary, default to 14

#endif
}

// Set the viewport
void CxbxrImpl_SetViewport(xbox::X_D3DVIEWPORT8* pViewport)
{
	LOG_INIT;

	// Unclear what to do when no viewport is passed
	// Set the default viewport?
	// Clamp the current viewport to the current rendertarget?
	if (pViewport == nullptr) {
		LOG_TEST_CASE("pViewport = null");
		return;
	}

	// We need a rendertarget to clamp the viewport
	// Pretty much everything hits this on boot because of the way our SetRenderTarget patch works
	if (!g_pXbox_RenderTarget) {
		return;
	}

	float rendertargetBaseWidth;
	float rendertargetBaseHeight;
	GetRenderTargetBaseDimensions(rendertargetBaseWidth, rendertargetBaseHeight);

	// Update the current viewport
	g_Xbox_Viewport = *pViewport;

	// The SetRenderTarget trampoline calls SetViewport with
	// both Width and Height set to INT_MAX
	if ((pViewport->Width == INT_MAX) ^ (pViewport->Height == INT_MAX)) {
		LOG_TEST_CASE("SetViewport called with only one of width/height set to INT_MAX");
	}

	// Cap width and height to screen bounds
	// Test case: Need for Speed: HP2 (rear view mirror)
	g_Xbox_Viewport.Width = std::min(g_Xbox_Viewport.Width, (DWORD)rendertargetBaseWidth - g_Xbox_Viewport.X);
	g_Xbox_Viewport.Height = std::min(g_Xbox_Viewport.Height, (DWORD)rendertargetBaseHeight - g_Xbox_Viewport.Y);
}
const DWORD FixedFunctionPipelineConstants[] =
{

	0x00000000,   // 0.0
	0x3f000000,   // 0.5
	0x3f800000,   // 1.0
	0x40000000,   // 2.0

	0xbf800000,   // -1.0
	0x00000000,   //  0.0
	0x3f800000,   //  1.0
	0x40000000,   //  2.0

	0x00000000,   //  0.0
	0x00000000,   //  0.0
	0xbf800000,   // -1.0
	0x00000000,   //  0.0
};

xbox::void_xt WINAPI CxbxrImpl_SetShaderConstantMode
(
	xbox::X_VERTEXSHADERCONSTANTMODE Mode
)
{
	g_Xbox_VertexShaderConstantMode = Mode;
	// if using 96 constants mode, copy fixed pipe line constatnts back to vertex shader constants.
	if (Mode == X_D3DSCM_96CONSTANTS) { // X_D3DSCM_96CONSTANTS = 0
		// set xbox vertex shader constant NV_IGRAPH_XF_XFCTX_CONS0 = 0x3C,
		// Slot 0x3c, 60(-36) is 'cSMAP', followed by 'cSKIN' and 'cREFL':
		//xbox::EMUPATCH(D3DDevice_SetVertexShaderConstant)(NV_IGRAPH_XF_XFCTX_CONS0,(xbox::PVOID)&FixedFunctionPipelineConstants[0],3);
		// reset lights

		// reset Texture Transform

		// reset Transform

		// reset fixed function T&L constants
		CxbxrImpl_SetVertexShaderConstant(NV_IGRAPH_XF_XFCTX_CONS0 - X_D3DSCM_CORRECTION, (xbox::PVOID)&FixedFunctionPipelineConstants[0], 3);
		// reset 4 Texgen Planes with Identity Matrix

		// reset Fog Plane with (0.0f, 0.0f, 0.0f, 1.0f)

		// reset Eye Position with (0.0f, 0.0f, 0.0f, 1.0f)
	}
}
//generalize pushbuffer token setup.
DWORD PBTokenArray[16] = { HLE_API_PUSHBFFER_COMMAND_64 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM token, int argCount=14, DWORD* argv= PBTokenArray)
{
	// init pushbuffer related pointers
	DWORD* pPush_local = (DWORD*)*g_pXbox_pPush;         //pointer to current pushbuffer
	DWORD* pPush_limit = (DWORD*)*g_pXbox_pPushLimit;    //pointer to the end of current pushbuffer
	if ((unsigned int)pPush_local + 64 >= (unsigned int)pPush_limit)//check if we still have enough space
		pPush_local = (DWORD*)CxbxrImpl_MakeSpace(); //make new pushbuffer space and get the pointer to it.

	// process xbox D3D API enum and arguments and push them to pushbuffer for pgraph to handle later.
	PBTokenArray[1] = (DWORD)token;
	// always copy the whole buffer
	memcpy(pPush_local, PBTokenArray,16*sizeof(DWORD));

	//set pushbuffer pointer to the new beginning
	// always reserve 1 command DWORD, 1 API enum, and 14 argmenet DWORDs.
	*(DWORD**)g_pXbox_pPush += 0x10;

}

// LTCG specific D3DDevice_SetShaderConstantMode function...
// This uses a custom calling convention where parameter is passed in EAX
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetShaderConstantMode_0__LTCG_eax1)
(
)
{
    X_VERTEXSHADERCONSTANTMODE Mode;
    __asm {
        LTCG_PROLOGUE
        mov  Mode, eax
		call XB_TRMP(D3DDevice_SetShaderConstantMode_0__LTCG_eax1)
    }

    //EMUPATCH(D3DDevice_SetShaderConstantMode)(Mode);
	//CxbxrImpl_SetShaderConstantMode(Mode);

    __asm {
        LTCG_EPILOGUE
        ret
    }
}

// ******************************************************************
// * patch: D3DDevice_SetShaderConstantMode
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetShaderConstantMode)
(
    X_VERTEXSHADERCONSTANTMODE Mode
)
{
	LOG_FUNC_ONE_ARG(Mode);

	XB_TRMP(D3DDevice_SetShaderConstantMode)(Mode);

	if(g_pXbox_pPush==nullptr)
	    CxbxrImpl_SetShaderConstantMode(Mode);
	else{
		//EMUPATCH(D3DDevice_SetShaderConstantMode)(Mode);
		//CxbxrImpl_SetShaderConstantMode(Mode);
		//1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
		PBTokenArray[2] = (DWORD)Mode;
		Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetShaderConstantMode, 1);
	}
}

// LTCG specific D3DDevice_SetVertexShaderConstant function...
// This uses a custom calling convention where ConstantCount parameter is passed in EDX
// Test-case: Murakumo
xbox::void_xt __fastcall xbox::EMUPATCH(D3DDevice_SetVertexShaderConstant_8)
(
    void* tmp,
    dword_xt    ConstantCount,
    int_xt      Register,
    CONST PVOID pConstantData
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Register)
		LOG_FUNC_ARG(pConstantData)
		LOG_FUNC_ARG(ConstantCount)
		LOG_FUNC_END;
	if (is_pushbuffer_recording()) {
		XB_TRMP(D3DDevice_SetVertexShaderConstant_8)(tmp, ConstantCount, Register, pConstantData);
	}
	CxbxrImpl_SetVertexShaderConstant(Register, pConstantData, ConstantCount);
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderConstant
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetVertexShaderConstant)
(
	xbox::int_xt         Register,
    CONST xbox::PVOID    pConstantData,
	xbox::dword_xt       ConstantCount
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Register)
		LOG_FUNC_ARG(pConstantData)
		LOG_FUNC_ARG(ConstantCount)
		LOG_FUNC_END;

	// TODO : Should we trampoline into Xbox code as well here,
	// so that besides pushing NV2A commands, Xbox internal D3D
	// state gets updated?
	if (is_pushbuffer_recording()) {
		XB_TRMP(D3DDevice_SetVertexShaderConstant)(Register, pConstantData, ConstantCount);
	}
	// Or better yet, remove all D3DDevice_SetVertexShaderConstant patches
	// once CxbxUpdateHostVertexShaderConstants is reliable (ie. : when we're
	// able to flush the NV2A push buffer)
	CxbxrImpl_SetVertexShaderConstant(Register, pConstantData, ConstantCount);
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderConstant1
// ******************************************************************
xbox::void_xt __fastcall xbox::EMUPATCH(D3DDevice_SetVertexShaderConstant1)
(
	xbox::int_xt Register,
    CONST xbox::PVOID  pConstantData
)
{
	LOG_FORWARD("D3DDevice_SetVertexShaderConstant1");

	if (is_pushbuffer_recording()) {
		XB_TRMP(D3DDevice_SetVertexShaderConstant1)(Register, pConstantData);
	}
    // The XDK uses a macro to automatically adjust to 0..191 range
    // but D3DDevice_SetVertexShaderConstant expects -96..95 range
    // so we adjust before forwarding
    //EMUPATCH(D3DDevice_SetVertexShaderConstant)(Register - X_D3DSCM_CORRECTION, pConstantData, 1);
	CxbxrImpl_SetVertexShaderConstant(Register - X_D3DSCM_CORRECTION, pConstantData, 1);
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderConstant1Fast
// ******************************************************************
xbox::void_xt __fastcall xbox::EMUPATCH(D3DDevice_SetVertexShaderConstant1Fast)
(
    int_xt         Register,
    CONST PVOID pConstantData
)
{
	LOG_FORWARD("D3DDevice_SetVertexShaderConstant1Fast");
	if (is_pushbuffer_recording()) {
		XB_TRMP(D3DDevice_SetVertexShaderConstant1Fast)(Register, pConstantData);
	}
    // The XDK uses a macro to automatically adjust to 0..191 range
    // but D3DDevice_SetVertexShaderConstant expects -96..95 range
    // so we adjust before forwarding
    //EMUPATCH(D3DDevice_SetVertexShaderConstant)(Register - X_D3DSCM_CORRECTION, pConstantData, 1);
	CxbxrImpl_SetVertexShaderConstant(Register - X_D3DSCM_CORRECTION, pConstantData, 1);
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderConstant4
// ******************************************************************
xbox::void_xt __fastcall xbox::EMUPATCH(D3DDevice_SetVertexShaderConstant4)
(
	xbox::int_xt Register,
    CONST xbox::PVOID  pConstantData
)
{
	LOG_FORWARD("D3DDevice_SetVertexShaderConstant");
	if (is_pushbuffer_recording()) {
		XB_TRMP(D3DDevice_SetVertexShaderConstant4)(Register, pConstantData);
	}

	// The XDK uses a macro to automatically adjust to 0..191 range
    // but D3DDevice_SetVertexShaderConstant expects -96..95 range
    // so we adjust before forwarding
	//EMUPATCH(D3DDevice_SetVertexShaderConstant)(Register - X_D3DSCM_CORRECTION, pConstantData, 4);
	CxbxrImpl_SetVertexShaderConstant(Register - X_D3DSCM_CORRECTION, pConstantData, 4);
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderConstantNotInline
// ******************************************************************
xbox::void_xt __fastcall xbox::EMUPATCH(D3DDevice_SetVertexShaderConstantNotInline)
(
    int_xt         Register,
    CONST PVOID pConstantData,
    dword_xt       ConstantCount
)
{
	LOG_FORWARD("D3DDevice_SetVertexShaderConstant");
	if (is_pushbuffer_recording()) {
		XB_TRMP(D3DDevice_SetVertexShaderConstantNotInline)(Register, pConstantData, ConstantCount);
	}
    // The XDK uses a macro to automatically adjust to 0..191 range
    // but D3DDevice_SetVertexShaderConstant expects -96..95 range
    // so we adjust before forwarding
	//EMUPATCH(D3DDevice_SetVertexShaderConstant)(Register - X_D3DSCM_CORRECTION, pConstantData, ConstantCount / 4);
	CxbxrImpl_SetVertexShaderConstant(Register - X_D3DSCM_CORRECTION, pConstantData, ConstantCount / 4);
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderConstantNotInlineFast
// ******************************************************************
xbox::void_xt __fastcall xbox::EMUPATCH(D3DDevice_SetVertexShaderConstantNotInlineFast)
(
    int_xt         Register,
    CONST PVOID pConstantData,
    dword_xt       ConstantCount
)
{
	LOG_FORWARD("D3DDevice_SetVertexShaderConstant");
	if (is_pushbuffer_recording()) {
		XB_TRMP(D3DDevice_SetVertexShaderConstantNotInlineFast)(Register, pConstantData, ConstantCount);
	}
    // The XDK uses a macro to automatically adjust to 0..191 range
    // but D3DDevice_SetVertexShaderConstant expects -96..95 range
    // so we adjust before forwarding
	//EMUPATCH(D3DDevice_SetVertexShaderConstant)(Register - X_D3DSCM_CORRECTION, pConstantData, ConstantCount / 4);
	CxbxrImpl_SetVertexShaderConstant(Register - X_D3DSCM_CORRECTION, pConstantData, ConstantCount / 4);
}

// Overload for logging
static void D3DDevice_SetTexture_4__LTCG_eax_pTexture
(
    xbox::dword_xt           Stage,
    xbox::X_D3DBaseTexture  *pTexture
)
{
    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(Stage)
        LOG_FUNC_ARG(pTexture)
        LOG_FUNC_END;
}

// LTCG specific D3DDevice_SetTexture function...
// This uses a custom calling convention where pTexture is passed in EAX
// Test-case: NASCAR Heat 2002
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetTexture_4__LTCG_eax_pTexture)
(
    dword_xt           Stage
)
{
    X_D3DBaseTexture *pTexture;
    __asm {
        LTCG_PROLOGUE
        mov  pTexture, eax
    }

    // Log
    D3DDevice_SetTexture_4__LTCG_eax_pTexture(Stage, pTexture);

    // Call the Xbox implementation of this function, to properly handle reference counting for us
    __asm {
        mov eax, pTexture
        push Stage
        call XB_TRMP(D3DDevice_SetTexture_4__LTCG_eax_pTexture)
    }

    g_pXbox_SetTexture[Stage] = pTexture;

    __asm {
        LTCG_EPILOGUE
        ret  4
    }
}

// Overload for logging
static void D3DDevice_SetTexture_4__LTCG_eax_Stage
(
    xbox::dword_xt           Stage,
    xbox::X_D3DBaseTexture  *pTexture
)
{
    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(Stage)
        LOG_FUNC_ARG(pTexture)
        LOG_FUNC_END;
}

// LTCG specific D3DDevice_SetTexture function...
// This uses a custom calling convention where Stage is passed in EAX
// Test-case: Metal Wolf Chaos
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetTexture_4__LTCG_eax_Stage)
(
    X_D3DBaseTexture  *pTexture
)
{
    dword_xt Stage;
    __asm {
        LTCG_PROLOGUE
        mov  Stage, eax
    }

    // Log
	D3DDevice_SetTexture_4__LTCG_eax_Stage(Stage, pTexture);

    // Call the Xbox implementation of this function, to properly handle reference counting for us
    __asm {
        mov eax, Stage
        push pTexture
        call XB_TRMP(D3DDevice_SetTexture_4__LTCG_eax_Stage)
    }

    g_pXbox_SetTexture[Stage] = pTexture;

    __asm {
        LTCG_EPILOGUE
        ret  4
    }
}

bool IsXboxResourceCached(xbox::X_D3DResource* pXboxResource, DWORD D3DUsage = 0, int iTextureStage = -1)
{
	auto key = GetHostResourceKey(pXboxResource, iTextureStage);
	auto& ResourceCache = GetResourceCache(key);
	auto it = ResourceCache.find(key);
	if (it == ResourceCache.end()) {
		return false;
	}
	return true;
}
void WINAPI CxbxrImpl_SetTexture(xbox::dword_xt Stage, xbox::X_D3DBaseTexture* pTexture)
{
	//update the currently used stage texture
	g_pXbox_SetTexture[Stage] = pTexture;
	HRESULT hRet;
	if (pTexture != nullptr) {
		g_Xbox_SetTexture[Stage] = *(xbox::X_D3DSurface*)pTexture;
		g_pXbox_SetTexture[Stage] = (xbox::X_D3DBaseTexture*)&g_Xbox_SetTexture[Stage];

		UINT64 key = ((UINT64)(pTexture->Format) << 32) | pTexture->Data;
		auto it = g_TextureCache.find(key);
		if (it != g_TextureCache.end()) {
				if ((it->second.Common & CXBX_D3DCOMMON_IDENTIFYING_MASK) != (g_pXbox_SetTexture[Stage]->Common & CXBX_D3DCOMMON_IDENTIFYING_MASK)) {
					//g_TextureCache.erase(key);
					//g_TextureCache.insert(std::pair<UINT64, xbox::X_D3DSurface >(key, g_Xbox_SetTexture[Stage]));
					g_TextureCache[key] = g_Xbox_SetTexture[Stage];
				}
				//UINT64 key = ((UINT64)(g_pXbox_DepthStencil->Format) << 32) | g_pXbox_DepthStencil->Data;
				//xbox::X_D3DFORMAT X_Format = GetXboxPixelContainerFormat((xbox::X_D3DPixelContainer*)pTexture);
				//if (EmuXBFormatIsDepthBuffer(X_Format)) {
				//}
		}
		else {
			g_TextureCache.insert(std::pair<UINT64, xbox::X_D3DSurface >(key, g_Xbox_SetTexture[Stage]));
		}

		for (auto it2 = g_DepthBufferCache.begin(); it2 != g_DepthBufferCache.end(); it2++) {
			if (pTexture->Data == it2->second.XboxDepthBufferSurface.Data) {
				/*
				hRet = D3DXLoadSurfaceFromSurface(it2->second.pHostDepthStencilTextureSurface, nullptr, NULL, it2->second.pHostDepthStencil, nullptr, NULL, D3DX_FILTER_NONE, 0);
				if (hRet != S_OK) {
					hRet = g_pD3DDevice->StretchRect(it2->second.pHostDepthStencil, NULL, it2->second.pHostDepthStencilTextureSurface, NULL, D3DTEXF_NONE);
					if (hRet != S_OK) {
						D3DSURFACE_DESC hostSourceDesc, hostDestDesc;
						it2->second.pHostDepthStencil->GetDesc(&hostSourceDesc);
						D3DLOCKED_RECT HostSrcLockedRect, HostDstLockedRect;
						hRet = it2->second.pHostDepthStencil->LockRect(&HostSrcLockedRect, NULL, D3DLOCK_NOOVERWRITE);
						if (hRet == D3D_OK) {
							hRet = it2->second.pHostDepthStencilTextureSurface->LockRect(&HostDstLockedRect, NULL, D3DLOCK_NOSYSLOCK);
							if (hRet == D3D_OK) {
								memcpy(HostDstLockedRect.pBits, HostSrcLockedRect.pBits, HostSrcLockedRect.Pitch * hostSourceDesc.Height);
							}
						}
					}
				}
				*/
				//if (hRet == S_OK)
				SetHostResource(pTexture, it2->second.pHostDepthStencilTexture);
			}
		}

#if 0	//link pTexture to possible render target or depth buffer is implemented in D3DDevice_SetRenderTarget(). Disable this section of code for testing. test case: DynamicGamma and HeatShimmer sample.
		//let's process the resource cache here for some special cases. some title will use xbox backbuffer as texture stage texture but not using the backbuffer surface directly,
		//instead the title might use XGSetTextureHeader() to create a new texture which using the Foramt/Data/Size from backbuffer, but with totally different Common type.
		//test case: DynamicGamma sample. in this case, the backbuffer surface is a surface without parent texture, Common is 0x01050001, but when setting a nex texture header to get a new texture using the backbuffer data,
		//the new Common becomes 0x00040001. it mean not created by D3D, and the resource type is texture instead of surface. when we retrieve host resource using this texture, we'll get a different host resource from what we get with the xbox backbuffer
		//so the data won't be linked even their xbox data point to the same address.
		DWORD oldCommon = pTexture->Common;
		DWORD newCommon;
		IDirect3DSurface* pHostSurface;
		IDirect3DTexture9* pHostTexture;
		auto xboxSurface = (xbox::X_D3DSurface*)pTexture;
		auto xboxTexture = (xbox::X_D3DTexture*)pTexture;
		if (!IsXboxResourceCached(pTexture, 0, Stage)) {
			pTexture->Common = 0x01040001;
			if (IsXboxResourceCached(pTexture, 0, Stage)) {
				//pHostTexture = (IDirect3DTexture9*)GetHostResource(pTexture, D3DUSAGE_RENDERTARGET, Stage);
				auto key = GetHostResourceKey(pTexture, Stage);
				auto& ResourceCache = GetResourceCache(key);
				auto it = ResourceCache.find(key);
				auto hostResource = it->second.pHostResource.Get();
				auto xboxResourceType = GetXboxD3DResourceType(pTexture);
				// Determine if the associated host resource is a render target already, if so, do nothing
				HRESULT hRet = STATUS_INVALID_PARAMETER; // Default to an error condition, so we can use D3D_OK to check for success
				D3DSURFACE_DESC surfaceDesc;
				hRet = ((IDirect3DTexture*)hostResource)->GetLevelDesc(0, &surfaceDesc);
				if (SUCCEEDED(hRet)) {
					// If this resource is already created as a render target on the host, simply return
					if (surfaceDesc.Usage & (D3DUSAGE_RENDERTARGET | D3DUSAGE_DEPTHSTENCIL)) {
						//restore original common
						pTexture->Common = oldCommon;
						//resource_info_t newResource;
						//ResourceCache[key] = newResource;
						//set the host resource of render target to our texture variants.
						if (xboxResourceType == xbox::X_D3DRTYPE_TEXTURE){
						    SetHostResource(pTexture, hostResource, Stage, 0);
						    SetHostResourcePCFormat(pTexture, surfaceDesc.Format, Stage);
						}
						else {
							EmuLog(LOG_LEVEL::WARNING, "xboxResourceType isn't texture in CxbxrImpl_SetTexture!");
						}
						return;
					}
				}
				else {
					EmuLog(LOG_LEVEL::WARNING, "Failed getting texture/surface desc in CxbxrImpl_SetTexture!");
				}
			}else{
				auto xboxResourceType = GetXboxD3DResourceType(pTexture);
				pTexture->Common = 0x01050001;
				if (IsXboxResourceCached(pTexture, 0, Stage)) {
					//pHostTexture = (IDirect3DTexture9*)GetHostResource(pTexture, D3DUSAGE_RENDERTARGET, Stage);
					auto key = GetHostResourceKey(pTexture, Stage);
					auto& ResourceCache = GetResourceCache(key);
					auto it = ResourceCache.find(key);
					auto hostResource = it->second.pHostResource.Get();
					// Determine if the associated host resource is a render target already, if so, do nothing
					HRESULT hRet = STATUS_INVALID_PARAMETER; // Default to an error condition, so we can use D3D_OK to check for success
					D3DSURFACE_DESC HostSurfaceDesc;
					hRet = ((IDirect3DSurface*)hostResource)->GetDesc(&HostSurfaceDesc);
					// to previous resource management behavior
					if (SUCCEEDED(hRet)) {
						// If this resource is already created as a render target on the host, simply return
						if (HostSurfaceDesc.Usage & (D3DUSAGE_RENDERTARGET|D3DUSAGE_DEPTHSTENCIL)) {
							//restore original common
							IDirect3DBaseTexture* pHostTexture;
							pHostTexture=CxbxConvertXboxSurfaceToHostTexture(pTexture);
							pTexture->Common = oldCommon;
							if (xboxResourceType == xbox::X_D3DRTYPE_TEXTURE) {
								SetHostResource(pTexture, pHostTexture, Stage, 0);
								SetHostResourcePCFormat(pTexture, HostSurfaceDesc.Format, Stage);
							}
							else {
								EmuLog(LOG_LEVEL::WARNING, "xboxResourceType isn't texture in CxbxrImpl_SetTexture!");
							}
							return;
						}
					}
					else {
						EmuLog(LOG_LEVEL::WARNING, "Failed getting texture/surface desc in CxbxrImpl_SetTexture!");
					}
				}
			}
		}
#endif
	}
}

// ******************************************************************
// * patch: D3DDevice_SetTexture
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetTexture)
(
	dword_xt           Stage,
	X_D3DBaseTexture* pTexture
	)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Stage)
		LOG_FUNC_ARG(pTexture)
		LOG_FUNC_END;

	// Call the Xbox implementation of this function, to properly handle reference counting for us
	//CxbxrImpl_Resource_AddRef(pTexture);
	XB_TRMP(D3DDevice_SetTexture)(Stage, pTexture);
	//*(UINT64*)& PBTokenArray[4] = pTexture->Data;
	//PBTokenArray[5] = (DWORD)pTexture->Format;
	// init pushbuffer related pointers
	if (g_pXbox_pPush == nullptr) {
		CxbxrImpl_SetTexture(Stage, pTexture);
	}
	else {
		DWORD* pPush_local = (DWORD*)*g_pXbox_pPush;         //pointer to current pushbuffer
		DWORD* pPush_limit = (DWORD*)*g_pXbox_pPushLimit;    //pointer to the end of current pushbuffer
		if ((unsigned int)pPush_local + 64 >= (unsigned int)pPush_limit)//check if we still have enough space
			pPush_local = (DWORD*)CxbxrImpl_MakeSpace(); //make new pushbuffer space and get the pointer to it.
		pPush_local[3] = (DWORD)0;// pTexture;

		if (pTexture != nullptr) {
			// we should better erase the pTexture if the key already existed in the map.
			// but this would introduce a data confliction if the pgraph is accessing the same key which we're trying to erase.
			// either a lock should be implemented here with g_TextureCache, or we simply keep the old key without updating it.
			pPush_local[3] = (DWORD)&pPush_local[4];// pTexture;
			*(X_D3DSurface*)pPush_local[3] = *(X_D3DSurface*)pTexture;
			if (GetXboxCommonResourceType(pTexture->Common) != X_D3DCOMMON_TYPE_SURFACE)
				((X_D3DSurface*)pPush_local[3])->Parent = nullptr;

			UINT64 key = ((UINT64)(pTexture->Format) << 32) | pTexture->Data;
			auto it = g_TextureCache.find(key);
			if (it != g_TextureCache.end()) {
				//release ref. count since we add ref. count in HLE patch.
				//CxbxrImpl_Resource_Release(pTexture);
				//g_TextureCache.erase(it);
				if (GetXboxCommonResourceType(pTexture) != X_D3DCOMMON_TYPE_SURFACE)
					((X_D3DSurface*)pPush_local[3])->Parent = nullptr;
			}
			//g_TextureCache.insert(std::pair<UINT64, xbox::X_D3DSurface>(key, pTexture));
		}
		pPush_local[0] = HLE_API_PUSHBFFER_COMMAND_64;
		// process xbox D3D API enum and arguments and push them to pushbuffer for pgraph to handle later.
		pPush_local[1] = (DWORD)X_D3DDevice_SetTexture;
		//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
		pPush_local[2] = (DWORD)Stage;
		//set pushbuffer pointer to the new beginning
		// always reserve 1 command DWORD, 1 API enum, and 14 argmenet DWORDs.
		*(DWORD**)g_pXbox_pPush += 0x10;
	}
}

void CxbxrImpl_SwitchTexture(
	xbox::dword_xt Method,
	xbox::dword_xt Data,
	xbox::dword_xt Format
    )
{
	DWORD Stage = -1;
	Stage = ((Method & 0x01FFC) - 0x01b00) / 0x40;
	// Update data and format separately, instead of via GetDataFromXboxResource()

	if (Stage >= 0 && Stage <= 3) {
		// Switch Texture updates the data pointer of an active texture using pushbuffer commands
		if (g_pXbox_SetTexture[Stage] == xbox::zeroptr) {
			LOG_TEST_CASE("D3DDevice_SwitchTexture without an active texture");
		}
		else {
			//LOG_TEST_CASE("Using CxbxActiveTextureCopies");
			// See https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/1159
			// Test-case : Arena Football
			// Test-case : Call of Duty 2: Big Red One
			// Test-case : Crimson Skies
			// Test-case : Freedom Fighters - see https://www.youtube.com/watch?v=_NDCoLY8V3I
			// Test-case : Freestyle MetalX
			// Test-case : GENMA ONIMUSHA
			// Test-case : Gun
			// Test-case : Harry Potter : Quidditch World Cup
			// Test-case : King Arthur
			// Test-case : Madden NFL 2002
			// Test-case : Madden NFL 2005
			// Test-case : Madden NFL 07
			// Test-case : Need For Speed Most Wanted
			// Test-case : Need For Speed Underground
			// Test-case : PocketBike Racer
			// Test-case : Project Gotham Racing 2
			// Test-case : Richard Burns Rally
			// Test-case : Spider - Man 2

			CxbxActiveTextureCopies[Stage].Common = g_pXbox_SetTexture[Stage]->Common;
			CxbxActiveTextureCopies[Stage].Data = Data;
			CxbxActiveTextureCopies[Stage].Format = Format;
			CxbxActiveTextureCopies[Stage].Lock = 0;
			CxbxActiveTextureCopies[Stage].Size = g_pXbox_SetTexture[Stage]->Size;
			if (GetXboxCommonResourceType(g_pXbox_SetTexture[Stage]->Common) != X_D3DCOMMON_TYPE_SURFACE)
				CxbxActiveTextureCopies[Stage].Parent = nullptr;

			// Use the above modified copy, instead of altering the active Xbox texture
			g_Xbox_SetTexture[Stage] = CxbxActiveTextureCopies[Stage];
			g_pXbox_SetTexture[Stage] =(xbox::X_D3DBaseTexture*) &g_Xbox_SetTexture[Stage];
			UINT64 key = ((UINT64)(g_pXbox_SetTexture[Stage]->Format) << 32) | g_pXbox_SetTexture[Stage]->Data;
			auto it = g_TextureCache.find(key);
			if (it != g_TextureCache.end()) {
				//release ref. count since we add ref. count in HLE patch.
				//CxbxrImpl_Resource_Release(it->second);
				if ((it->second.Common & CXBX_D3DCOMMON_IDENTIFYING_MASK) != (g_pXbox_SetTexture[Stage]->Common & CXBX_D3DCOMMON_IDENTIFYING_MASK)) {
					g_TextureCache.erase(key);
					g_TextureCache.insert(std::pair<UINT64, xbox::X_D3DSurface >(key, CxbxActiveTextureCopies[Stage]));
				}
			}else{
				g_TextureCache.insert(std::pair<UINT64, xbox::X_D3DSurface >(key, CxbxActiveTextureCopies[Stage]));
			}
			// Note : Since g_pXbox_SetTexture and CxbxActiveTextureCopies are host-managed,
			// Xbox code should never alter these members (so : no reference counting, etc).
			// As long as that's guaranteed, this is a safe way to emulate SwitchTexture.
			// (GetHostResourceKey also avoids using any Xbox texture resource memory address.)
		}
	}
}
// ******************************************************************
// * patch: D3DDevice_SwitchTexture
// ******************************************************************
xbox::void_xt __fastcall xbox::EMUPATCH(D3DDevice_SwitchTexture)
(
    dword_xt           Method,
    dword_xt           Data,
    dword_xt           Format
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Method)
		LOG_FUNC_ARG(Data)
		LOG_FUNC_ARG(Format)
		LOG_FUNC_END;
	
		/*
				__asm {
			push ecx
			mov ecx, Method
			push edx
			mov edx,Data
		}
*/
		//__fastcall XB_TRMP(D3DDevice_SwitchTexture) is a  __fastcall which put first arg in ecx, 2nd arg in edx, args starting from 3rd arg are put in stack
	XB_TRMP(D3DDevice_SwitchTexture)(Method, Data, Format);
/*
		__asm {
			pop edx
			pop ecx
		}
	}
*/
	/*
	switch (Method) { // Detect which of the 4 (X_D3DTS_STAGECOUNT) texture stages is given by the (NV2A) Method argument
	// This code contains D3DPUSH_ENCODE(NV2A_TX_OFFSET(v), 2) = 2 DWORD's, shifted left PUSH_COUNT_SHIFT (18) left
	case 0x00081b00: Stage = 0; break;
	case 0x00081b40: Stage = 1; break;
	case 0x00081b80: Stage = 2; break;
	case 0x00081bc0: Stage = 3; break;
	default:
		LOG_TEST_CASE("D3DDevice_SwitchTexture Unknown Method");
        EmuLog(LOG_LEVEL::WARNING, "Unknown Method (0x%.08X)", Method);
	}
	*/
	// init pushbuffer related pointers
		DWORD* pPush_local = (DWORD*)*g_pXbox_pPush;         //pointer to current pushbuffer
		DWORD* pPush_limit = (DWORD*)*g_pXbox_pPushLimit;    //pointer to the end of current pushbuffer
		if ((unsigned int)pPush_local + 64 >= (unsigned int)pPush_limit)//check if we still have enough space
			pPush_local = (DWORD*)CxbxrImpl_MakeSpace(); //make new pushbuffer space and get the pointer to it.

		// process xbox D3D API enum and arguments and push them to pushbuffer for pgraph to handle later.
		pPush_local[0] = HLE_API_PUSHBFFER_COMMAND_64 ;
		pPush_local[1] = X_D3DAPI_ENUM::X_D3DDevice_SwitchTexture;//enum of this patched API
		pPush_local[2] = (DWORD)Method; //total 14 DWORD space for arguments.
		pPush_local[3] = (DWORD)Data;
		pPush_local[4] = (DWORD)Format;

		//set pushbuffer pointer to the new beginning
		// always reserve 1 command DWORD, 1 API enum, and 14 argmenet DWORDs.
		*(DWORD**)g_pXbox_pPush += 0x10;
}

// ******************************************************************
// * patch: D3DDevice_ApplyStateBlock
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_ApplyStateBlock)
(
	xbox::dword_xt Token
	)
{
	LOG_FUNC_ONE_ARG(Token);

	xbox::hresult_xt result = XB_TRMP(D3DDevice_ApplyStateBlock)(Token);

	EmuKickOffWait();

	return result;

}

// ******************************************************************
// * patch: D3DDevice_Begin
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_Begin)
(
    X_D3DPRIMITIVETYPE     PrimitiveType
)
{
	LOG_FUNC_ONE_ARG(PrimitiveType);
	
	if (is_pushbuffer_recording()) {
		XB_TRMP(D3DDevice_Begin)(PrimitiveType);
	}
	else {

		assert((xbox::X_D3DPRIMITIVETYPE)PrimitiveType != xbox::X_D3DPT_INVALID);
		CxbxrImpl_Begin(PrimitiveType);
		/*
		// init pushbuffer related pointers
		DWORD* pPush_local = (DWORD*)*g_pXbox_pPush;         //pointer to current pushbuffer
		DWORD* pPush_limit = (DWORD*)*g_pXbox_pPushLimit;    //pointer to the end of current pushbuffer
		if ((unsigned int)pPush_local + 64 >= (unsigned int)pPush_limit)//check if we still have enough space
			pPush_local = (DWORD*)CxbxrImpl_MakeSpace(); //make new pushbuffer space and get the pointer to it.

		// process xbox D3D API enum and arguments and push them to pushbuffer for pgraph to handle later.
		pPush_local[0] = HLE_API_PUSHBFFER_COMMAND_64 ;
		pPush_local[1] = X_D3DAPI_ENUM::X_D3DDevice_Begin;//enum of this patched API
		pPush_local[2] = (DWORD)PrimitiveType; //total 14 DWORD space for arguments.

		//set pushbuffer pointer to the new beginning
		// always reserve 1 command DWORD, 1 API enum, and 14 argmenet DWORDs.
		*(DWORD**)g_pXbox_pPush += 0x10;
		*/
	}
}

// ******************************************************************
// * patch: D3DDevice_SetVertexData2f
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetVertexData2f)
(
    int_xt     Register,
    float_xt   a,
    float_xt   b
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Register)
		LOG_FUNC_ARG(a)
		LOG_FUNC_ARG(b)
		LOG_FUNC_END;
	
	if (is_pushbuffer_recording()) {
		XB_TRMP(D3DDevice_SetVertexData2f)(Register, a, b);
	}
	else {
	    CxbxrImpl_SetVertexData4f(Register, a, b, 0.0f, 1.0f);
		/*
		// init pushbuffer related pointers
		DWORD* pPush_local = (DWORD*)*g_pXbox_pPush;         //pointer to current pushbuffer
		DWORD* pPush_limit = (DWORD*)*g_pXbox_pPushLimit;    //pointer to the end of current pushbuffer
		if ((unsigned int)pPush_local + 64 >= (unsigned int)pPush_limit)//check if we still have enough space
			pPush_local = (DWORD*)CxbxrImpl_MakeSpace(); //make new pushbuffer space and get the pointer to it.

		// process xbox D3D API enum and arguments and push them to pushbuffer for pgraph to handle later.
		pPush_local[0] = HLE_API_PUSHBFFER_COMMAND_64 ;
		pPush_local[1] = X_D3DAPI_ENUM::X_D3DDevice_SetVertexData4f;//enum of this patched API
		pPush_local[2] = (DWORD)Register; //total 14 DWORD space for arguments.
		pPush_local[3] = FtoDW(a);
		pPush_local[4] = FtoDW(b);
		pPush_local[5] = FtoDW(0.0f);
		pPush_local[6] = FtoDW(1.0f);

		//set pushbuffer pointer to the new beginning
		// always reserve 1 command DWORD, 1 API enum, and 14 argmenet DWORDs.
		*(DWORD**)g_pXbox_pPush += 0x10;
		*/
	}
}

//static inline DWORD FtoDW(FLOAT f) { return *((DWORD*)&f); }
//static inline FLOAT DWtoF(DWORD f) { return *((FLOAT*)&f); }

// ******************************************************************
// * patch: D3DDevice_SetVertexData2s
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetVertexData2s)
(
    int_xt   Register,
    short_xt a,
    short_xt b
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Register)
		LOG_FUNC_ARG(a)
		LOG_FUNC_ARG(b)
		LOG_FUNC_END;
	
	if (is_pushbuffer_recording()) {
		XB_TRMP(D3DDevice_SetVertexData2s)(Register, a, b);
	}
	else {
		// Test case: Halo
		// Note : XQEMU verified that the int16_t arguments
		// must be mapped to floats in the range [-32768.0, 32767.0]
		// (See https://github.com/xqemu/xqemu/pull/176)
		const float fa = static_cast<float>(a);
		const float fb = static_cast<float>(b);

		CxbxrImpl_SetVertexData4f(Register, a, b, 0.0f, 1.0f);
		/*
		// init pushbuffer related pointers
		DWORD* pPush_local = (DWORD*)*g_pXbox_pPush;         //pointer to current pushbuffer
		DWORD* pPush_limit = (DWORD*)*g_pXbox_pPushLimit;    //pointer to the end of current pushbuffer
		if ((unsigned int)pPush_local + 64 >= (unsigned int)pPush_limit)//check if we still have enough space
			pPush_local = (DWORD*)CxbxrImpl_MakeSpace(); //make new pushbuffer space and get the pointer to it.

		// process xbox D3D API enum and arguments and push them to pushbuffer for pgraph to handle later.
		pPush_local[0] = HLE_API_PUSHBFFER_COMMAND_64 ;
		pPush_local[1] = X_D3DAPI_ENUM::X_D3DDevice_SetVertexData4f;//enum of this patched API
		pPush_local[2] = (DWORD)Register; //total 14 DWORD space for arguments.
		pPush_local[3] = FtoDW(fa);
		pPush_local[4] = FtoDW(fb);
		pPush_local[5] = FtoDW(0.0f);
		pPush_local[6] = FtoDW(1.0f);

		//set pushbuffer pointer to the new beginning
		// always reserve 1 command DWORD, 1 API enum, and 14 argmenet DWORDs.
		*(DWORD**)g_pXbox_pPush += 0x10;
		*/
	}
}

extern uint32_t HLE_read_NV2A_pgraph_register(const int reg); // Declared in PushBuffer.cpp

extern NV2ADevice* g_NV2A;

// Overload for logging
static void D3DDevice_SetVertexData4f_16
(
    xbox::int_xt     Register,
    xbox::float_xt   a,
    xbox::float_xt   b,
    xbox::float_xt   c,
    xbox::float_xt   d
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Register)
		LOG_FUNC_ARG(a)
		LOG_FUNC_ARG(b)
		LOG_FUNC_ARG(c)
		LOG_FUNC_ARG(d)
		LOG_FUNC_END;
	/*
	// init pushbuffer related pointers
	DWORD* pPush_local = (DWORD*)*g_pXbox_pPush;         //pointer to current pushbuffer
	DWORD* pPush_limit = (DWORD*)*g_pXbox_pPushLimit;    //pointer to the end of current pushbuffer
	if ((unsigned int)pPush_local + 64 >= (unsigned int)pPush_limit)//check if we still have enough space
		pPush_local = (DWORD*)CxbxrImpl_MakeSpace(); //make new pushbuffer space and get the pointer to it.

	// process xbox D3D API enum and arguments and push them to pushbuffer for pgraph to handle later.
	pPush_local[0] = HLE_API_PUSHBFFER_COMMAND_64 ;
	pPush_local[1] = X_D3DAPI_ENUM::X_D3DDevice_SetVertexData4f;//enum of this patched API
	pPush_local[2] = (DWORD)Register; //total 14 DWORD space for arguments.
	pPush_local[3] = FtoDW(a);
	pPush_local[4] = FtoDW(b);
	pPush_local[5] = FtoDW(c);
	pPush_local[6] = FtoDW(d);

	//set pushbuffer pointer to the new beginning
	// always reserve 1 command DWORD, 1 API enum, and 14 argmenet DWORDs.
	*(DWORD**)g_pXbox_pPush += 0x10;
	*/
}

// ******************************************************************
// * patch: D3DDevice_SetVertexData4f_16
// ******************************************************************
// This is an LTCG specific version of SetVertexData4f where the first param is passed in EDI
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetVertexData4f_16)
(
	float_xt   a,
	float_xt   b,
	float_xt   c,
	float_xt   d
)
{
	int_xt Register;

	__asm {
		LTCG_PROLOGUE
		mov  Register, edi
	}
	
	if (is_pushbuffer_recording()) {
		XB_TRMP(D3DDevice_SetVertexData4f_16)(a, b, c, d);
	}
	else {
		// Log
		D3DDevice_SetVertexData4f_16(Register, a, b, c, d);

		CxbxrImpl_SetVertexData4f(Register, a, b, c, d);
	}
	_asm {
		LTCG_EPILOGUE
		ret  10h
	}
}

// ******************************************************************
// * patch: D3DDevice_SetVertexData4f
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetVertexData4f)
(
    int_xt     Register,
    float_xt   a,
    float_xt   b,
    float_xt   c,
    float_xt   d
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Register)
		LOG_FUNC_ARG(a)
		LOG_FUNC_ARG(b)
		LOG_FUNC_ARG(c)
		LOG_FUNC_ARG(d)
		LOG_FUNC_END;
	
	if (is_pushbuffer_recording()) {
		XB_TRMP(D3DDevice_SetVertexData4f)(Register, a, b, c, d);
	}
	else {
		CxbxrImpl_SetVertexData4f(Register, a, b, c, d);
		/*
		// init pushbuffer related pointers
		DWORD* pPush_local = (DWORD*)*g_pXbox_pPush;         //pointer to current pushbuffer
		DWORD* pPush_limit = (DWORD*)*g_pXbox_pPushLimit;    //pointer to the end of current pushbuffer
		if ((unsigned int)pPush_local + 64 >= (unsigned int)pPush_limit)//check if we still have enough space
			pPush_local = (DWORD*)CxbxrImpl_MakeSpace(); //make new pushbuffer space and get the pointer to it.

		// process xbox D3D API enum and arguments and push them to pushbuffer for pgraph to handle later.
		pPush_local[0] = HLE_API_PUSHBFFER_COMMAND_64 ;
		pPush_local[1] = X_D3DAPI_ENUM::X_D3DDevice_SetVertexData4f;//enum of this patched API
		pPush_local[2] = (DWORD)Register; //total 14 DWORD space for arguments.
		pPush_local[3] = FtoDW(a);
		pPush_local[4] = FtoDW(b);
		pPush_local[5] = FtoDW(c);
		pPush_local[6] = FtoDW(d);

		//set pushbuffer pointer to the new beginning
		// always reserve 1 command DWORD, 1 API enum, and 14 argmenet DWORDs.
		*(DWORD**)g_pXbox_pPush += 0x10;
		*/
	}
}

// ******************************************************************
// * patch: D3DDevice_SetVertexData4ub
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetVertexData4ub)
(
	int_xt	Register,
	byte_xt	a,
	byte_xt	b,
	byte_xt	c,
	byte_xt	d
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Register)
		LOG_FUNC_ARG(a)
		LOG_FUNC_ARG(b)
		LOG_FUNC_ARG(c)
		LOG_FUNC_ARG(d)
		LOG_FUNC_END;
	
	if (is_pushbuffer_recording()) {
		XB_TRMP(D3DDevice_SetVertexData4ub)(Register, a, b, c, d);
	}
	else {
		const float fa = a / 255.0f;
		const float fb = b / 255.0f;
		const float fc = c / 255.0f;
		const float fd = d / 255.0f;

		CxbxrImpl_SetVertexData4f(Register, fa, fb, fc, fd);
		/*
		// init pushbuffer related pointers
		DWORD* pPush_local = (DWORD*)*g_pXbox_pPush;         //pointer to current pushbuffer
		DWORD* pPush_limit = (DWORD*)*g_pXbox_pPushLimit;    //pointer to the end of current pushbuffer
		if ((unsigned int)pPush_local + 64 >= (unsigned int)pPush_limit)//check if we still have enough space
			pPush_local = (DWORD*)CxbxrImpl_MakeSpace(); //make new pushbuffer space and get the pointer to it.

		// process xbox D3D API enum and arguments and push them to pushbuffer for pgraph to handle later.
		pPush_local[0] = HLE_API_PUSHBFFER_COMMAND_64 ;
		pPush_local[1] = X_D3DAPI_ENUM::X_D3DDevice_SetVertexData4f;//enum of this patched API
		pPush_local[2] = (DWORD)Register; //total 14 DWORD space for arguments.
		pPush_local[3] = FtoDW(fa);
		pPush_local[4] = FtoDW(fb);
		pPush_local[5] = FtoDW(fc);
		pPush_local[6] = FtoDW(fd);

		//set pushbuffer pointer to the new beginning
		// always reserve 1 command DWORD, 1 API enum, and 14 argmenet DWORDs.
		*(DWORD**)g_pXbox_pPush += 0x10;
		*/
	}
}

// ******************************************************************
// * patch: D3DDevice_SetVertexData4s
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetVertexData4s)
(
	int_xt	 Register,
	short_xt a,
	short_xt b,
	short_xt c,
	short_xt d
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Register)
		LOG_FUNC_ARG(a)
		LOG_FUNC_ARG(b)
		LOG_FUNC_ARG(c)
		LOG_FUNC_ARG(d)
		LOG_FUNC_END;
	
	if (is_pushbuffer_recording()) {
		XB_TRMP(D3DDevice_SetVertexData4s)(Register, a, b, c, d);
	}
	else {
		// Test case: Halo
		// Note : XQEMU verified that the int16_t arguments
		// must be mapped to floats in the range [-32768.0, 32767.0]
		// (See https://github.com/xqemu/xqemu/pull/176)
		const float fa = static_cast<float>(a);
		const float fb = static_cast<float>(b);
		const float fc = static_cast<float>(c);
		const float fd = static_cast<float>(d);

		CxbxrImpl_SetVertexData4f(Register, fa, fb, fc, fd);
		/*
		// init pushbuffer related pointers
		DWORD* pPush_local = (DWORD*)*g_pXbox_pPush;         //pointer to current pushbuffer
		DWORD* pPush_limit = (DWORD*)*g_pXbox_pPushLimit;    //pointer to the end of current pushbuffer
		if ((unsigned int)pPush_local + 64 >= (unsigned int)pPush_limit)//check if we still have enough space
			pPush_local = (DWORD*)CxbxrImpl_MakeSpace(); //make new pushbuffer space and get the pointer to it.

		// process xbox D3D API enum and arguments and push them to pushbuffer for pgraph to handle later.
		pPush_local[0] = HLE_API_PUSHBFFER_COMMAND_64 ;
		pPush_local[1] = X_D3DAPI_ENUM::X_D3DDevice_SetVertexData4f;//enum of this patched API
		pPush_local[2] = (DWORD)Register; //total 14 DWORD space for arguments.
		pPush_local[3] = FtoDW(fa);
		pPush_local[4] = FtoDW(fb);
		pPush_local[5] = FtoDW(fc);
		pPush_local[6] = FtoDW(fd);

		//set pushbuffer pointer to the new beginning
		// always reserve 1 command DWORD, 1 API enum, and 14 argmenet DWORDs.
		*(DWORD**)g_pXbox_pPush += 0x10;
		*/
	}
}

// ******************************************************************
// * patch: D3DDevice_SetVertexDataColor
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetVertexDataColor)
(
    int_xt      Register,
    D3DCOLOR    Color
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Register)
		LOG_FUNC_ARG(Color)
		LOG_FUNC_END;
	
	if (is_pushbuffer_recording()) {
		XB_TRMP(D3DDevice_SetVertexDataColor)(Register, Color);
	}
	else {
		const D3DXCOLOR XColor = Color;

		CxbxrImpl_SetVertexData4f(Register, XColor.r, XColor.g, XColor.b, XColor.a);
		/*
		// init pushbuffer related pointers
		DWORD* pPush_local = (DWORD*)*g_pXbox_pPush;         //pointer to current pushbuffer
		DWORD* pPush_limit = (DWORD*)*g_pXbox_pPushLimit;    //pointer to the end of current pushbuffer
		if ((unsigned int)pPush_local + 64 >= (unsigned int)pPush_limit)//check if we still have enough space
			pPush_local = (DWORD*)CxbxrImpl_MakeSpace(); //make new pushbuffer space and get the pointer to it.

		// process xbox D3D API enum and arguments and push them to pushbuffer for pgraph to handle later.
		pPush_local[0] = HLE_API_PUSHBFFER_COMMAND_64 ;
		pPush_local[1] = X_D3DAPI_ENUM::X_D3DDevice_SetVertexData4f;//enum of this patched API
		pPush_local[2] = (DWORD)Register; //total 14 DWORD space for arguments.
		pPush_local[3] = FtoDW(XColor.r);
		pPush_local[4] = FtoDW(XColor.g);
		pPush_local[5] = FtoDW(XColor.b);
		pPush_local[6] = FtoDW(XColor.a);

		//set pushbuffer pointer to the new beginning
		// always reserve 1 command DWORD, 1 API enum, and 14 argmenet DWORDs.
		*(DWORD**)g_pXbox_pPush += 0x10;
		*/
	}
}

// ******************************************************************
// * patch: D3DDevice_End
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_End)()
{
	LOG_FUNC();
	
	if (is_pushbuffer_recording()) {
		XB_TRMP(D3DDevice_End)();
	}
	else {

		bool WaitForPGRAPH;
		WaitForPGRAPH = true;
		//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
		PBTokenArray[2] = (DWORD)&WaitForPGRAPH;// (DWORD)PrimitiveType;

		//give the correct token enum here, and it's done.
		Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_End, 1, PBTokenArray);//argCount, not necessary, default to 14

		EmuKickOff();

		while (WaitForPGRAPH)
			;

		CxbxrImpl_End(); //we unpatched D3DDevice_End, but found that there are memory corruptions introduced by multi entrance. so we have to come out a workaround.
	}
	hresult_xt result = 0;// XB_TRMP(D3DDevice_End)();

	return result;
	
}

// ******************************************************************
// * patch: D3DDevice_RunPushBuffer
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_RunPushBuffer)
(
    X_D3DPushBuffer       *pPushBuffer,
    X_D3DFixup            *pFixup
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pPushBuffer)
		LOG_FUNC_ARG(pFixup)
		LOG_FUNC_END;
	// this patch is only to notify the pgraph handler we're starting to run a recorded pushbuffer.
	// pPush_local[2]!=0 for start running, pPush_local[2]==0 for end running.
	bool WaitForPgraph = true;
	if ((pPushBuffer->Common & 0x80000000) == 0)
		WaitForPgraph = false;
	//pPushBuffer->Common |= 0x80000000;
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)pPushBuffer;
	PBTokenArray[3] = (DWORD)pFixup;
	PBTokenArray[4] = (DWORD)&WaitForPgraph;
	//give the correct token enum here, and it's done.
	//Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_RunPushBuffer, 2);//argCount, not necessary, default to 14

	//EmuKickOff();
	//while (WaitForPgraph)
	//	;

	//if (is_pushbuffer_recording()) {
	XB_TRMP(D3DDevice_RunPushBuffer)(pPushBuffer, pFixup);
	//}
	//EmuExecutePushBuffer(pPushBuffer, pFixup);
	/*
	// notify pgraph handler we've done running pushbuffer.
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = 0;
	PBTokenArray[3] = (DWORD) & WaitForPgraph;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_RunPushBuffer, 2);//argCount, not necessary, default to 14
	EmuKickOff();
	while (WaitForPgraph)
		;
	*/
	return;
}
void CxbxrImpl_Clear
(
	xbox::dword_xt           Count,
	CONST D3DRECT* pRects,
	xbox::dword_xt           Flags,
	D3DCOLOR        Color,
	float           Z,
	xbox::dword_xt           Stencil
	)
{


	DWORD HostFlags = 0;
	
	// Clear requires the Xbox viewport to be applied
	// CxbxUpdateNativeD3DResources();

    // make adjustments to parameters to make sense with windows d3d
    {
		if (Flags & xbox::X_D3DCLEAR_TARGET) {
			// TODO: D3DCLEAR_TARGET_A, *R, *G, *B don't exist on windows
			if ((Flags & xbox::X_D3DCLEAR_TARGET) != xbox::X_D3DCLEAR_TARGET)
				EmuLog(LOG_LEVEL::WARNING, "Unsupported : Partial D3DCLEAR_TARGET flag(s) for D3DDevice_Clear : 0x%.08X", Flags & xbox::X_D3DCLEAR_TARGET);
		
			HostFlags |= D3DCLEAR_TARGET;
		}

        // Do not needlessly clear Z Buffer
		if (Flags & xbox::X_D3DCLEAR_ZBUFFER) {
			if (g_bHasDepth)
				HostFlags |= D3DCLEAR_ZBUFFER;
			else
				EmuLog(LOG_LEVEL::WARNING, "Unsupported : D3DCLEAR_ZBUFFER flag for D3DDevice_Clear without ZBuffer");
		}

		// Only clear depth buffer and stencil if present
		//
		// Avoids following DirectX Debug Runtime error report
		//    [424] Direct3D8: (ERROR) :Invalid flag D3DCLEAR_ZBUFFER: no zbuffer is associated with device. Clear failed. 
		if (Flags & xbox::X_D3DCLEAR_STENCIL) {
			if (g_bHasStencil)
				HostFlags |= D3DCLEAR_STENCIL;
			else
				EmuLog(LOG_LEVEL::WARNING, "Unsupported : D3DCLEAR_STENCIL flag for D3DDevice_Clear without ZBuffer");
		}

        if(Flags & ~(xbox::X_D3DCLEAR_TARGET | xbox::X_D3DCLEAR_ZBUFFER | xbox::X_D3DCLEAR_STENCIL))
            EmuLog(LOG_LEVEL::WARNING, "Unsupported Flag(s) for D3DDevice_Clear : 0x%.08X", Flags & ~(xbox::X_D3DCLEAR_TARGET | xbox::X_D3DCLEAR_ZBUFFER | xbox::X_D3DCLEAR_STENCIL));
    }

    HRESULT hRet;

    if (pRects != nullptr) {
        // Scale the fill based on our scale factor and MSAA scale
		float aaX, aaY;
		GetMultiSampleScaleRaw(aaX, aaY);
		float Xscale = aaX * g_RenderUpscaleFactor;
		float Yscale = aaY * g_RenderUpscaleFactor;

        std::vector<D3DRECT> rects(Count);
        for (DWORD i = 0; i < Count; i++) {
            rects[i].x1 = static_cast<LONG>(pRects[i].x1 * Xscale);
            rects[i].x2 = static_cast<LONG>(pRects[i].x2 * Xscale);
            rects[i].y1 = static_cast<LONG>(pRects[i].y1 * Yscale);
            rects[i].y2 = static_cast<LONG>(pRects[i].y2 * Yscale);
		}
        hRet = g_pD3DDevice->Clear(Count, rects.data(), HostFlags, Color, Z, Stencil);
    } else {
		hRet = g_pD3DDevice->Clear(Count, pRects, HostFlags, Color, Z, Stencil);
    }

	//DEBUG_D3DRESULT(hRet, "g_pD3DDevice->Clear");
}

// ******************************************************************
// * patch: D3DDevice_Clear
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_Clear)
(
	dword_xt           Count,
	CONST D3DRECT* pRects,
	dword_xt           Flags,
	D3DCOLOR        Color,
	float           Z,
	dword_xt           Stencil
	)
{

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Count)
		LOG_FUNC_ARG(pRects)
		LOG_FUNC_ARG(Flags)
		LOG_FUNC_ARG(Color)
		LOG_FUNC_ARG(Z)
		LOG_FUNC_ARG(Stencil)
		LOG_FUNC_END;

#if !USEPGRAPH_Clear
	if (is_pushbuffer_recording()) {
		XB_TRMP(D3DDevice_Clear)(Count, pRects, Flags, Color, Z, Stencil);
	}
	CxbxrImpl_Clear(  Count, pRects, Flags, Color, Z, Stencil);
#else
	if (g_pXbox_pPush == nullptr) {
		CxbxrImpl_Clear(Count, pRects, Flags, Color, Z, Stencil);
	}
	else {
		// init pushbuffer related pointers
		DWORD* pPush_local = (DWORD*)*g_pXbox_pPush;         //pointer to current pushbuffer
		DWORD* pPush_limit = (DWORD*)*g_pXbox_pPushLimit;    //pointer to the end of current pushbuffer
		if ((unsigned int)pPush_local + 64 >= (unsigned int)pPush_limit)//check if we still have enough space
			pPush_local = (DWORD*)CxbxrImpl_MakeSpace(); //make new pushbuffer space and get the pointer to it.

		// process xbox D3D API enum and arguments and push them to pushbuffer for pgraph to handle later.
		pPush_local[0] = HLE_API_PUSHBFFER_COMMAND_64;
		pPush_local[1] = X_D3DAPI_ENUM::X_D3DDevice_Clear;//enum of this patched API
		pPush_local[2] = (DWORD)Count; //total 14 DWORD space for arguments.
		pPush_local[3] = (DWORD)pRects;
		pPush_local[4] = (DWORD)Flags;
		pPush_local[5] = (DWORD)Color;
		pPush_local[6] = FtoDW(Z);
		pPush_local[7] = (DWORD)Stencil;

		//set pushbuffer pointer to the new beginning
		// always reserve 1 command DWORD, 1 API enum, and 14 argmenet DWORDs.
		*(DWORD**)g_pXbox_pPush += 0x10;
	}
#endif
}
// ******************************************************************
// * patch: D3DDevice_CopyRects
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_CopyRects)
(
	X_D3DSurface* pSourceSurface,
	CONST RECT* pSourceRectsArray,
	uint_xt                cRects,
	X_D3DSurface* pDestinationSurface,
	CONST POINT* pDestPointsArray
	)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pSourceSurface);
	LOG_FUNC_ARG(pSourceRectsArray);
	LOG_FUNC_ARG(cRects);
	LOG_FUNC_ARG(pDestinationSurface);
	LOG_FUNC_ARG(pDestPointsArray);
	LOG_FUNC_END;
	//trampoline draw call if we're recording pushbuffer
	if (g_pXbox_BeginPush_Buffer != nullptr) {
		XB_TRMP(D3DDevice_CopyRects)(pSourceSurface, pSourceRectsArray, cRects, pDestinationSurface, pDestPointsArray);
	}else{
		bool WaitForPGRAPH;
		WaitForPGRAPH = true;
		//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
		PBTokenArray[2] = (DWORD)&WaitForPGRAPH;// (DWORD)PrimitiveType;

		//give the correct token enum here, and it's done.
		Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_CopyRects, 1);//argCount, not necessary, default to 14
		//kickoff pushbuffer handler
		EmuKickOff();
		//wait till our flag been reset
		while (WaitForPGRAPH)
			;
		//now do our job
		CxbxrImpl_CopyRects(pSourceSurface, pSourceRectsArray, cRects, pDestinationSurface, pDestPointsArray);
	}
}
void WINAPI CxbxrImpl_CopyRects
(
	xbox::X_D3DSurface * pSourceSurface,
	CONST RECT * pSourceRectsArray,
	xbox::uint_xt                cRects,
	xbox::X_D3DSurface * pDestinationSurface,
	CONST POINT * pDestPointsArray
	)
{
	xbox::X_D3DFORMAT src_X_Format = GetXboxPixelContainerFormat(pSourceSurface);
	xbox::X_D3DFORMAT dst_X_Format = GetXboxPixelContainerFormat(pDestinationSurface);
	extern BOOL EmuXBFormatIsBumpMap(xbox::X_D3DFORMAT Format);
	bool bSourceIsBumpmap = EmuXBFormatIsBumpMap(src_X_Format);
	bool bDestinationIsBumpmap = EmuXBFormatIsBumpMap(dst_X_Format);
	D3DSURFACE_DESC hostSourceDesc, hostDestDesc;
	auto pHostSourceSurface = GetHostSurface(pSourceSurface);
	pHostSourceSurface->GetDesc(&hostSourceDesc);
	auto pHostDestSurface = GetHostSurface(pDestinationSurface);
	pHostDestSurface->GetDesc(&hostDestDesc);
	HRESULT hRet;
	//is we have one is bumpmap and one is not, deal with it to make both the same.
	if (bSourceIsBumpmap) {
		if (hostSourceDesc.Format == D3DFMT_A8R8G8B8) {
			if (hostDestDesc.Format != D3DFMT_A8R8G8B8) {
				//GetHostResource(); CreateHostResource();
				auto key = GetHostResourceKey(pDestinationSurface);
				auto& ResourceCache = GetResourceCache(key);
				auto it = ResourceCache.find(key);
				//if source not found, just create a host texture with ARGB
				if (it != ResourceCache.end()) {
					if (it->second.pHostResource != nullptr) {
						D3DFORMAT defaultPCFormat = EmuXB2PC_D3DFormat(dst_X_Format);
						//if(it->second.PCFormat!= D3DFMT_A8R8G8B8){
						//defaultPCFormat is UV bumpmap format, if the host resource is uv bumpmap format, we erase it from cache and force regenerate it with ARGB
						//if (it->second.PCFormat == defaultPCFormat) {
							//FreeHostResource(key);
							//erase the host resource empty key
							ResourceCache.erase(key);
						//}
					}
					else {
						//erase the host resource empty key
						;//ResourceCache.erase(key);
					}
					EmuLog(LOG_LEVEL::WARNING, "GetHostResource: Resource not registered or does not have a host counterpart!");
				}
				xbox::X_D3DBaseTexture* pDestinationTexture;
				if (pDestinationSurface->Parent != nullptr) {
					pDestinationTexture = pDestinationSurface->Parent;
				}
				else {
					pDestinationTexture = (xbox::X_D3DBaseTexture*)pDestinationSurface;
					pDestinationTexture->Common &= 0xFFFEFFFF;
				}
				key = GetHostResourceKey(pDestinationTexture);
				it = ResourceCache.find(key);
				if (it != ResourceCache.end()) {
					if (it->second.pHostResource != nullptr) {
						D3DFORMAT defaultPCFormat = EmuXB2PC_D3DFormat(dst_X_Format);
						//if(it->second.PCFormat!= D3DFMT_A8R8G8B8){
						//defaultPCFormat is UV bumpmap format, if the host resource is uv bumpmap format, we erase it from cache and force regenerate it with ARGB
						//if (it->second.PCFormat == defaultPCFormat) {
							//FreeHostResource(key);
							//erase the host resource empty key
							ResourceCache.erase(key);
						//}
					}
					else {
						//erase the host resource empty key
						;//ResourceCache.erase(key);
					}
					EmuLog(LOG_LEVEL::WARNING, "GetHostResource: Resource not registered or does not have a host counterpart!");
				}
				IDirect3DTexture* pHostDestTexture;
				extern bool g_bForceHostARGBConversion;
				if (EmuXBFormatCanBeConvertedToARGB(dst_X_Format))
					g_bForceHostARGBConversion = true;
				HRESULT hRet = g_pD3DDevice->CreateTexture(hostDestDesc.Width, hostDestDesc.Height,
					1, // Levels
					D3DUSAGE_DYNAMIC, // Usage always as render target D3DUSAGE_DYNAMIC
					hostSourceDesc.Format,
					D3DPOOL_DEFAULT, // D3DPOOL_DEFAULT D3DPOOL_SYSTEMMEM
					&pHostDestTexture,
					nullptr // pSharedHandle
				);
				//HRESULT hRet = pHostDestSurface->GetContainer(IID_PPV_ARGS(&pHostDestTexture));
				//pHostDestTexture =(IDirect3DTexture*) GetHostBaseTexture(pDestinationTexture);
				SetHostTexture(pDestinationTexture, pHostDestTexture, -1);
				hRet = pHostDestTexture->GetSurfaceLevel(0, &pHostDestSurface);
				pHostDestSurface->GetDesc(&hostDestDesc);
				SetHostSurface(pDestinationSurface, pHostDestSurface, -1);
				//SetHostSurface(pDestinationSurface, pHostDestSurface,-1);
				g_bForceHostARGBConversion = false;
			}
		}
	}

	// We skip the trampoline to prevent unnecessary work
    // As our surfaces remain on the GPU, calling the trampoline would just
    // result in a memcpy from an empty Xbox surface to another empty Xbox Surface
    
    if (pHostSourceSurface == nullptr || pHostDestSurface == nullptr) {
        // Test Case: DOA2 attempts to copy from an index buffer resource type
        // TODO: What should we do here?
        LOG_TEST_CASE("D3DDevice-CopyRects: Failed to fetch host surfaces");
        return;
    }

    
    // If the source is a render-target and the destination is not, we need force it to be re-created as one
    // This is because StrechRects cannot copy from a Render-Target to a Non-Render Target
    // Test Case: Crash Bandicoot: Wrath of Cortex attemps to copy the render-target to a texture
    // This fixes an issue on the pause screen where the screenshot of the current scene was not displayed correctly
    if ((hostSourceDesc.Usage & D3DUSAGE_RENDERTARGET) != 0 && (hostDestDesc.Usage & D3DUSAGE_RENDERTARGET) == 0) {
        pHostDestSurface = GetHostSurface(pDestinationSurface, D3DUSAGE_RENDERTARGET);
        pHostDestSurface->GetDesc(&hostDestDesc);
    }

    // If no rectangles were given, default to 1 (entire surface)
    if (cRects == 0) {
        cRects = 1;
    }

    // Get Xbox surface dimensions
    // Host resources may be scaled so we'll account for that later
    auto xboxSourceWidth = GetPixelContainerWidth(pSourceSurface);
    auto xboxSourceHeight = GetPixelContainerHeight(pSourceSurface);
    auto xboxDestWidth = GetPixelContainerWidth(pDestinationSurface);
    auto xboxDestHeight = GetPixelContainerHeight(pDestinationSurface);

    for (UINT i = 0; i < cRects; i++) {
        RECT SourceRect, DestRect;

        if (pSourceRectsArray != nullptr) {
            SourceRect = pSourceRectsArray[i];
        } else {
            SourceRect.left = 0;
            SourceRect.right = xboxSourceWidth;
            SourceRect.top = 0;
            SourceRect.bottom = xboxSourceHeight;
        }

        if (pDestPointsArray != nullptr) {
            DestRect.left = pDestPointsArray[i].x;
            DestRect.right = DestRect.left + (SourceRect.right - SourceRect.left);
            DestRect.top = pDestPointsArray[i].y;
            DestRect.bottom = DestRect.top + (SourceRect.bottom - SourceRect.top);
        } else if (pSourceRectsArray) {
            DestRect = SourceRect;
        } else {
            DestRect.left = 0;
            DestRect.right = xboxDestWidth;
            DestRect.top = 0;
            DestRect.bottom = xboxDestHeight;
        }

        // Scale the source and destination rects
        auto sourceScaleX = (uint32_t)hostSourceDesc.Width / xboxSourceWidth;
        auto sourceScaleY = (uint32_t)hostSourceDesc.Height / xboxSourceHeight;

        SourceRect.left *= sourceScaleX;
        SourceRect.right *= sourceScaleX;
        SourceRect.top *= sourceScaleY;
        SourceRect.bottom *= sourceScaleY;

        auto destScaleX = (uint32_t) hostDestDesc.Width / xboxDestWidth;
        auto destScaleY = (uint32_t) hostDestDesc.Height / xboxDestHeight;

        DestRect.left *= destScaleX;
        DestRect.right *= destScaleX;
        DestRect.top *= destScaleY;
        DestRect.bottom *= destScaleY;

        hRet = g_pD3DDevice->StretchRect(pHostSourceSurface, &SourceRect, pHostDestSurface, &DestRect, D3DTEXF_LINEAR);
        if (FAILED(hRet)) {
			// Fallback for cases which StretchRect cannot handle (such as copying from texture to texture)
			hRet = D3DXLoadSurfaceFromSurface(pHostDestSurface, nullptr, &DestRect, pHostSourceSurface, nullptr, &SourceRect, D3DTEXF_LINEAR, 0);
			if (FAILED(hRet)) {
				LOG_TEST_CASE("D3DDevice_CopyRects: Failed to copy surface");
				D3DLOCKED_RECT HostSrcLockedRect, HostDstLockedRect;
				hRet = pHostSourceSurface->LockRect(&HostSrcLockedRect, NULL, D3DLOCK_NOOVERWRITE);
				if (hRet == D3D_OK) {
					hRet = pHostDestSurface->LockRect(&HostDstLockedRect, NULL, D3DLOCK_NOOVERWRITE);
					if (hRet == D3D_OK) {
						if (HostSrcLockedRect.Pitch == HostDstLockedRect.Pitch && hostDestDesc.Format == hostSourceDesc.Format)
						    memcpy(HostDstLockedRect.pBits, HostSrcLockedRect.pBits, HostSrcLockedRect.Pitch* hostSourceDesc.Height);
					}
				}
			}
        }
    }
}

#define CXBX_SWAP_PRESENT_FORWARD (256 + xbox::X_D3DSWAP_FINISH + xbox::X_D3DSWAP_COPY) // = CxbxPresentForwardMarker + D3DSWAP_FINISH + D3DSWAP_COPY

xbox::void_xt WINAPI CxbxrImpl_Present
(
	xbox::dword_xt Flags
	)
{
	CxbxrImpl_Swap(CXBX_SWAP_PRESENT_FORWARD); // Xbox present ignores
}

// ******************************************************************
// * patch: D3DDevice_Present
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_Present)
(
    CONST RECT* pSourceRect,
    CONST RECT* pDestRect,
    PVOID       pDummy1,
    PVOID       pDummy2
)
{
	// LOG_FORWARD("D3DDevice_Swap");
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pSourceRect)
		LOG_FUNC_ARG(pDestRect)
		LOG_FUNC_ARG(pDummy1)
		LOG_FUNC_ARG(pDummy2)
		LOG_FUNC_END;
#if !USEPGRAPH_Present

	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)pSourceRect;
	PBTokenArray[3] = (DWORD)pDestRect;
	PBTokenArray[4] = (DWORD)pDummy1;
	PBTokenArray[5] = (DWORD)pDummy1;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_Present, 4);//argCount, not necessary, default to 14
#else
	//template for syncing HLE apis with pgraf using waiting lock
	bool WaitForPGRAPH;
	WaitForPGRAPH = true;
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)&WaitForPGRAPH;// (DWORD)PrimitiveType;

	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_Present, 1, PBTokenArray);//argCount, not necessary, default to 14

	EmuKickOff();

	while (WaitForPGRAPH)
		; //this line is must have
	CxbxrImpl_Swap(CXBX_SWAP_PRESENT_FORWARD);  // Xbox present ignores
#endif
}

std::chrono::steady_clock::time_point frameStartTime;

// LTCG specific swap function...
// This uses a custom calling convention where parameter is passed in EAX
__declspec(naked) xbox::dword_xt WINAPI xbox::EMUPATCH(D3DDevice_Swap_0)
(
)
{
    dword_xt Flags;
	dword_xt result;
    __asm {
        LTCG_PROLOGUE
        mov  Flags, eax
    }

    result = EMUPATCH(D3DDevice_Swap)(Flags);

    __asm {
		mov  eax, result
        LTCG_EPILOGUE
        ret
    }
}

 void CxbxrGetXboxBackBuffer(int BackBuffer, xbox::X_D3DSurface** ppXboxFBuffer)
{
	 xbox::X_D3DSurface* pXboxBackBuffer;

	 if (XB_TRMP(D3DDevice_GetBackBuffer) != nullptr) {
		 XB_TRMP(D3DDevice_GetBackBuffer)(BackBuffer, D3DBACKBUFFER_TYPE_MONO, ppXboxFBuffer);
	 }
	 else if (XB_TRMP(D3DDevice_GetBackBuffer2) != nullptr) {
		 *ppXboxFBuffer = XB_TRMP(D3DDevice_GetBackBuffer2)(BackBuffer);
	 }
	 else if (XB_TRMP(D3DDevice_GetBackBuffer_8)) {
		 __asm {
			 push eax
			 mov  eax, BackBuffer
		 }
		     XB_TRMP(D3DDevice_GetBackBuffer_8)(D3DBACKBUFFER_TYPE_MONO, ppXboxFBuffer);
		 __asm {
		     pop eax
		 }
		 *ppXboxFBuffer = pXboxBackBuffer;
	 }
	 else if(XB_TRMP(D3DDevice_GetBackBuffer2_0__LTCG_eax1)) {
		 __asm {
			 mov  eax, BackBuffer
			 call XB_TRMP(D3DDevice_GetBackBuffer2_0__LTCG_eax1)
			 mov pXboxBackBuffer, eax
		 }
		 *ppXboxFBuffer = pXboxBackBuffer;
	 }

	 // Now pXboxBackbuffer points to the requested Xbox backbuffer
	 if (*ppXboxFBuffer == nullptr) {
		 CxbxrAbort("D3DDevice_GetBackBuffer2: Could not get Xbox backbuffer");
	 }
}

void CxbxrUpdateHostXboxBackBufferSurface(xbox::X_D3DSurface *pXboxSurface)
{
	LOG_INIT

		IDirect3DSurface* pHostXboxSurface = GetHostSurface(g_pXbox_BackBufferSurface, D3DUSAGE_RENDERTARGET);
	IDirect3DSurface* pTmpHostSurface = nullptr;
	IDirect3DTexture* pTmpHostTexture = nullptr;
	D3DSURFACE_DESC HostXboxSurfaceDesc;
	HRESULT hRet = pHostXboxSurface->GetDesc(&HostXboxSurfaceDesc);
	//create temp texture and surface to load xbox surface data
	hRet = g_pD3DDevice->CreateTexture(HostXboxSurfaceDesc.Width, HostXboxSurfaceDesc.Height,
		1, // Levels
		D3DUSAGE_DYNAMIC, // Usage always as render target
		HostXboxSurfaceDesc.Format,
		D3DPOOL_SYSTEMMEM, // D3DPOOL_DEFAULT
		&pTmpHostTexture,
		nullptr // pSharedHandle
	);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateTexture");
	if (hRet == D3D_OK) {
		hRet = pTmpHostTexture->GetSurfaceLevel(0, &pTmpHostSurface);
		DEBUG_D3DRESULT(hRet, "pNewHostTexture->pNewHostSurface");
	}
	D3DLOCKED_RECT LockedRect = {};
	if (hRet == D3D_OK) {
		hRet = pTmpHostSurface->LockRect(&LockedRect,NULL, D3DLOCK_NOSYSLOCK);
		DEBUG_D3DRESULT(hRet, "pNewHostTexture->pNewHostSurface");
	}
	if (hRet == D3D_OK) {
		BYTE* src = (BYTE*)GetDataFromXboxResource(pXboxSurface);
		BYTE* dst = (BYTE*)LockedRect.pBits;
		memcpy(dst, src, HostXboxSurfaceDesc.Height * LockedRect.Pitch);

		pTmpHostSurface->UnlockRect();
		hRet = D3DXLoadSurfaceFromSurface(pHostXboxSurface, nullptr, nullptr, pTmpHostSurface, nullptr, nullptr, D3DTEXF_POINT, 0);
	}

	if (pTmpHostSurface != nullptr)pTmpHostSurface -> Release();
	if (pTmpHostTexture != nullptr)pTmpHostTexture->Release();

}
xbox::X_D3DSurface* pXbox_RenderTargetNew;
xbox::X_D3DSurface Xbox_RenderTargetNew;
IDirect3DSurface* pXbox_RenderTargetHostSurfaceNew;

DWORD CxbxrImpl_Swap
(
	xbox::dword_xt Flags
)
{
	// TODO: Ensure this flag is always the same across library versions
	if (Flags != 0 && Flags != CXBX_SWAP_PRESENT_FORWARD)
		LOG_TEST_CASE("D3DDevice_Swap: Flags != 0");
	// Handle swap flags
	// We don't maintain a swap chain, and draw everything to backbuffer 0
	// so just hack around the swap flags for now...
	// xdk sample BackbufferScale uses D3DSWAP_COPY when BackBufferScale was altered, and uses D3DSWAP_FINISH when ready to blit to front buffer
	static float prevBackBufferScaleX, prevBackBufferScaleY;
	g_LastD3DSwap = (xbox::X_D3DSWAP)Flags;

	HRESULT hRet;
	// Write Xbox constants
	auto pg = &(g_NV2A->GetDeviceState()->pgraph);
	//reset BackbufferScale before any possible early return of swap(),
	//if xbox backbuffer was locked before swap, we have to update xbox backbuffer surface to it's host suface
	if (g_bXbox_BackBufferSurfaceLocked) {
		CxbxrUpdateHostXboxBackBufferSurface(g_pXbox_BackBufferSurface);
		g_bXbox_BackBufferSurfaceLocked = false;
	}

	// Early exit if we're not ready to present
	// Test Case: Antialias sample, BackBufferScale sample
	// Which use D3DSWAP_COPY to render UI directly to the frontbuffer
	// If we present before the UI is drawn, it will flicker
	if (Flags != xbox::X_D3DSWAP_DEFAULT && !(Flags & xbox::X_D3DSWAP_FINISH)) {

		if ((Flags == xbox::X_D3DSWAP_COPY)&& (g_Xbox_BackbufferScaleX != 1.0|| g_Xbox_BackbufferScaleY != 1.0)) {
			LOG_TEST_CASE("X_D3DSWAP_COPY");
			auto pRenderTargetNewPrevKey = GetHostResourceKey(pXbox_RenderTargetNew);
			Xbox_RenderTargetNew = *g_pXbox_RenderTarget;
			pXbox_RenderTargetNew = &Xbox_RenderTargetNew;
			DWORD commonMask = 0x01000000;
			DWORD common = g_pXbox_RenderTarget->Common & commonMask;
			if(common!=0)
			    Xbox_RenderTargetNew.Common &= ~commonMask;
			else
			    Xbox_RenderTargetNew.Common |= commonMask;

			auto pXboxRenderTargetSurfaceNew= GetHostSurface(pXbox_RenderTargetNew, D3DUSAGE_RENDERTARGET);
			if (pXbox_RenderTargetHostSurfaceNew != pXboxRenderTargetSurfaceNew) {
				if (pXbox_RenderTargetHostSurfaceNew != nullptr)
					FreeHostResource(pRenderTargetNewPrevKey);
				pXbox_RenderTargetHostSurfaceNew = pXboxRenderTargetSurfaceNew;
			}
			auto pXboxRenderTargetSurface = GetHostSurface(g_pXbox_RenderTarget, D3DUSAGE_RENDERTARGET);
			IDirect3DSurface* pExistingHostRenderTarget = nullptr;
			hRet = g_pD3DDevice->GetRenderTarget(0, &pExistingHostRenderTarget);
			assert(hRet == D3D_OK);

			RECT scr{};
			scr.top = (LONG)0;
			scr.left = (LONG)0;
			scr.right = (LONG)((pg->KelvinPrimitive.SetSurfaceClipHorizontal & 0xFFFF0000) >> 16);//was (LONG)(width + 0.5f);   //
			scr.bottom = (LONG)((pg->KelvinPrimitive.SetSurfaceClipVertical & 0xFFFF0000) >> 16);//was (LONG)(height + 0.5f); //

			// stretch original render target rect(smaller) to full destination render target.
			hRet = g_pD3DDevice->StretchRect(
				/* pSourceSurface = */ pExistingHostRenderTarget,
				/* pSourceRect = */ &scr,
				/* pDestSurface = */ pXboxRenderTargetSurfaceNew,
				/* pDestRect = */ nullptr,
				/* Filter = */ D3DTEXF_LINEAR
			);
			assert(hRet == D3D_OK);
			// using StretchRect to update full surfaces
			hRet = g_pD3DDevice->StretchRect(
				/* pSourceSurface = */ pXboxRenderTargetSurfaceNew,
				/* pSourceRect = */ NULL,
				/* pDestSurface = */ pExistingHostRenderTarget,
				/* pDestRect = */ nullptr,
				/* Filter = */ D3DTEXF_LINEAR
			);
			assert(hRet == D3D_OK);
		}
		DWORD flag = xbox::X_D3DSWAP_BYPASSCOPY;
		g_Xbox_BackbufferScaleX = 1.0;
		g_Xbox_BackbufferScaleY = 1.0;
		return g_Xbox_SwapData.Swap;
	}

	g_Xbox_BackbufferScaleX = 1.0;
	g_Xbox_BackbufferScaleY = 1.0;
	// Fetch the host backbuffer
	IDirect3DSurface *pCurrentHostBackBuffer = nullptr;
	hRet = g_pD3DDevice->GetBackBuffer(
		0, // iSwapChain
		0, D3DBACKBUFFER_TYPE_MONO, &pCurrentHostBackBuffer);

	//DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetBackBuffer - Unable to get backbuffer surface!");
	if (hRet == D3D_OK) {
		assert(pCurrentHostBackBuffer != nullptr);

        // Clear the backbuffer surface, this prevents artifacts when switching aspect-ratio
        // Test-case: Dashboard
        IDirect3DSurface* pExistingRenderTarget = nullptr;
        hRet = g_pD3DDevice->GetRenderTarget(0, &pExistingRenderTarget);
        if (hRet == D3D_OK) {
            g_pD3DDevice->SetRenderTarget(0, pCurrentHostBackBuffer);
            g_pD3DDevice->Clear(
                /*Count=*/0,
                /*pRects=*/nullptr,
                D3DCLEAR_TARGET | (g_bHasDepth ? D3DCLEAR_ZBUFFER : 0) | (g_bHasStencil ? D3DCLEAR_STENCIL : 0),
                /*Color=*/0xFF000000, // TODO : Use constant for this
                /*Z=*/g_bHasDepth ? 1.0f : 0.0f,
                /*Stencil=*/0);
            g_pD3DDevice->SetRenderTarget(0, pExistingRenderTarget);
            pExistingRenderTarget->Release();
        }
        
        // TODO: Implement a hot-key to change the filter?
        // Note: LoadSurfaceFilter Must be D3DTEXF_NONE, D3DTEXF_POINT or D3DTEXF_LINEAR
        // Before StretchRects we used D3DX_FILTER_POINT here, but that gave jagged edges in Dashboard.
        // Dxbx note : D3DX_FILTER_LINEAR gives a smoother image, but 'bleeds' across borders
        // LoadOverlayFilter must be a D3DX filter DWORD value
        const D3DTEXTUREFILTERTYPE LoadSurfaceFilter = D3DTEXF_LINEAR;
        const DWORD LoadOverlayFilter = D3DX_DEFAULT;

        // Use backbuffer width/height since that may differ from the Window size
        const auto width = g_XBVideo.bMaintainAspect ? g_AspectRatioScaleWidth * g_AspectRatioScale : g_HostBackBufferDesc.Width;
        const auto height = g_XBVideo.bMaintainAspect ? g_AspectRatioScaleHeight * g_AspectRatioScale : g_HostBackBufferDesc.Height;

		auto pXboxBackBufferHostSurface = GetHostSurface(g_pXbox_BackBufferSurface, D3DUSAGE_RENDERTARGET);
		if (pXboxBackBufferHostSurface) {
            // Calculate the centered rectangle
            RECT dest{};
            dest.top = (LONG)((g_HostBackBufferDesc.Height - height) / 2);
            dest.left = (LONG)((g_HostBackBufferDesc.Width - width) / 2);
            dest.right = (LONG)(dest.left + width);
            dest.bottom = (LONG)(dest.top + height);

            // Blit Xbox BackBuffer to host BackBuffer
            hRet = g_pD3DDevice->StretchRect(
                /* pSourceSurface = */ pXboxBackBufferHostSurface,
				/* pSourceRect = */ nullptr,
                /* pDestSurface = */ pCurrentHostBackBuffer,
                /* pDestRect = */ &dest,
                /* Filter = */ LoadSurfaceFilter
            );
		
			if (hRet != D3D_OK) {
				EmuLog(LOG_LEVEL::WARNING, "Couldn't blit Xbox BackBuffer to host BackBuffer : %X", hRet);
			}
		}

		// Is there an overlay to be presented too?
		if (g_OverlayProxy.Surface.Common) {
			xbox::X_D3DFORMAT X_Format = GetXboxPixelContainerFormat(&g_OverlayProxy.Surface);
			if (X_Format != xbox::X_D3DFMT_YUY2) {
				LOG_TEST_CASE("Xbox overlay surface isn't using X_D3DFMT_YUY2");
			}

			// Interpret the Xbox overlay data (depending the color space conversion render state)
			// as either YUV or RGB format (note that either one must be a 3 bytes per pixel format)
			D3DFORMAT PCFormat;
			// TODO : Before reading from pgraph, flush all pending push-buffer commands
			NV2AState* d = g_NV2A->GetDeviceState();
			PGRAPHState* pg = &d->pgraph;

			//switch (GET_MASK(HLE_read_NV2A_pgraph_register(NV_PGRAPH_CONTROL_0), NV_PGRAPH_CONTROL_0_CSCONVERT)) {
			//switch (GET_MASK(HLE_read_NV2A_pgraph_register(NV097_SET_CONTROL0/4), NV097_SET_CONTROL0_COLOR_SPACE_CONVERT)) {
			switch (GET_MASK(pg->KelvinPrimitive.SetControl0, NV097_SET_CONTROL0_COLOR_SPACE_CONVERT)) {
			case 0:  // = pass-through
				PCFormat = D3DFMT_YUY2;
				break;
			case 1: // = CRYCB_TO_RGB
				PCFormat = D3DFMT_YUY2; // Test-case : Turok (intro movie)
				break;
			case 2: // = SCRYSCB_TO_RGB
				LOG_TEST_CASE("SCRYSCB_TO_RGB");
				PCFormat = D3DFMT_YUY2;
				break;
			default:
				LOG_TEST_CASE("Unrecognized NV_PGRAPH_CONTROL_0_CSCONVERT");
				PCFormat = D3DFMT_YUY2;
				break;
			}

			// Blit Xbox overlay to host backbuffer
			uint8_t *pOverlayData = (uint8_t*)GetDataFromXboxResource(&g_OverlayProxy.Surface);
			UINT OverlayWidth, OverlayHeight, OverlayDepth, OverlayRowPitch, OverlaySlicePitch;
			CxbxGetPixelContainerMeasures(
				&g_OverlayProxy.Surface,
				0, // dwMipMapLevel
				&OverlayWidth, &OverlayHeight, &OverlayDepth, &OverlayRowPitch, &OverlaySlicePitch);

            RECT EmuSourRect = { 0 };
            RECT EmuDestRect = { 0 };

			if (g_OverlayProxy.SrcRect.right > 0) {
				EmuSourRect = g_OverlayProxy.SrcRect;
			}
			else {
				SetRect(&EmuSourRect, 0, 0, OverlayWidth, OverlayHeight);
			}

			if (g_OverlayProxy.DstRect.right > 0) {
				// If there's a destination rectangle given, copy that into our local variable :
				EmuDestRect = g_OverlayProxy.DstRect;

                // Make sure to scale the values based on the difference between the Xbox and Host backbuffer
                // We can't use the scale factor here because we are blitting directly to the host backbuffer
                // NOT an Xbox surface!
                DWORD XboxBackBufferWidth = GetPixelContainerWidth(g_pXbox_BackBufferSurface);
                DWORD XboxBackBufferHeight = GetPixelContainerHeight(g_pXbox_BackBufferSurface);

				// We also need to account for any MSAA which may have enlarged the Xbox Backbuffer
				float xScale, yScale;
				GetMultiSampleScaleRaw(xScale, yScale);
				
                xScale = (float)width / ((float)XboxBackBufferWidth / xScale);
                yScale = (float)height / ((float)XboxBackBufferHeight / yScale);

                // Scale the destination co-ordinates by the correct scale factor
                EmuDestRect.top = (LONG)(EmuDestRect.top * yScale);
                EmuDestRect.left = (LONG)(EmuDestRect.left * xScale);
                EmuDestRect.bottom = (LONG)(EmuDestRect.bottom * yScale);
                EmuDestRect.right = (LONG)(EmuDestRect.right * xScale);

                // Finally, adjust to correct on-screen position (
                EmuDestRect.top += (LONG)((g_HostBackBufferDesc.Height - height) / 2);
                EmuDestRect.left += (LONG)((g_HostBackBufferDesc.Width - width) / 2);
                EmuDestRect.right += (LONG)((g_HostBackBufferDesc.Width - width) / 2);
                EmuDestRect.bottom += (LONG)((g_HostBackBufferDesc.Height - height) / 2);
			} else {
                // Calculate the centered rectangle
                EmuDestRect.top = (LONG)((g_HostBackBufferDesc.Height - height) / 2);
                EmuDestRect.left = (LONG)((g_HostBackBufferDesc.Width - width) / 2);
                EmuDestRect.right = (LONG)(EmuDestRect.left + width);
                EmuDestRect.bottom = (LONG)(EmuDestRect.top + height);
			}

			// load the YUY2 into the backbuffer

			// Limit the width and height of the output to the backbuffer dimensions.
			// This will (hopefully) prevent exceptions in Blinx - The Time Sweeper
			// (see https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/285)
			{
				// Use our (bounded) copy when bounds exceed :
				if (EmuDestRect.right > (LONG)g_HostBackBufferDesc.Width) {
					EmuDestRect.right = (LONG)g_HostBackBufferDesc.Width;
				}

				if (EmuDestRect.bottom > (LONG)g_HostBackBufferDesc.Height) {
					EmuDestRect.bottom = (LONG)g_HostBackBufferDesc.Height;
				}
			}

            // Create a temporary surface to hold the overlay
            // This is faster than loading directly into the backbuffer because it offloads scaling to the GPU
            // Without this, upscaling tanks the frame-rate!
            IDirect3DSurface* pTemporaryOverlaySurface;
            HRESULT hRet = g_pD3DDevice->CreateOffscreenPlainSurface(
                OverlayWidth,
                OverlayHeight,
                D3DFMT_A8R8G8B8,
                D3DPOOL_DEFAULT,
                &pTemporaryOverlaySurface,
                nullptr
            );

            if (FAILED(hRet)) {
                EmuLog(LOG_LEVEL::WARNING, "Couldn't create temporary overlay surface : %X", hRet);
            } else {
                RECT doNotScaleRect = { 0, 0, (LONG)OverlayWidth, (LONG)OverlayHeight };

                // Use D3DXLoadSurfaceFromMemory() to do conversion, we don't stretch at this moment in time
                // avoiding the need for YUY2toARGB() (might become relevant when porting to D3D9 or OpenGL)
                // see https://msdn.microsoft.com/en-us/library/windows/desktop/bb172902(v=vs.85).aspx
                hRet = D3DXLoadSurfaceFromMemory(
                    /* pDestSurface = */ pTemporaryOverlaySurface,
                    /* pDestPalette = */ nullptr,
                    /* pDestRect = */ &doNotScaleRect,
                    /* pSrcMemory = */ pOverlayData, // Source buffer
                    /* SrcFormat = */ PCFormat,
                    /* SrcPitch = */ OverlayRowPitch,
                    /* pSrcPalette = */ nullptr,
                    /* pSrcRect = */ &doNotScaleRect, // This parameter cannot be NULL
                    /* Filter = */ LoadOverlayFilter,
                    /* ColorKey = */ g_OverlayProxy.EnableColorKey ? g_OverlayProxy.ColorKey : 0);

                //DEBUG_D3DRESULT(hRet, "D3DXLoadSurfaceFromMemory - UpdateOverlay could not convert buffer!\n");
                if (hRet != D3D_OK) {
                    EmuLog(LOG_LEVEL::WARNING, "Couldn't load Xbox overlay to host surface : %X", hRet);
                } else {
                    hRet = g_pD3DDevice->StretchRect(
                        /* pSourceSurface = */ pTemporaryOverlaySurface,
                        /* pSourceRect = */ &EmuSourRect,
                        /* pDestSurface = */ pCurrentHostBackBuffer,
                        /* pDestRect = */ &EmuDestRect,
                        /* Filter = */ LoadSurfaceFilter
                    );

                    if (hRet != D3D_OK) {
                        EmuLog(LOG_LEVEL::WARNING, "Couldn't load Xbox overlay to host back buffer : %X", hRet);
                    }
                }

                pTemporaryOverlaySurface->Release();
            }
		}

		// Render ImGui
		static std::function<void(ImGuiUI*, IDirect3DSurface*)> internal_render = &CxbxImGui_RenderD3D9;
		g_renderbase->Render(internal_render, pCurrentHostBackBuffer);

		pCurrentHostBackBuffer->Release();
	}

	g_pD3DDevice->EndScene();
#if 0	
	IDirect3DSwapChain9* pSwapChain;
	hRet = g_pD3DDevice->GetSwapChain(
		/*[in]  UINT  iSwapChain*/ 0,
		/*[in]  IDirect3DSwapChain9 * *ppSwapChain*/&pSwapChain
	);

	
	pSwapChain->Present(0, 0, 0, 0, D3DPRESENT_LINEAR_CONTENT);
#endif	

	hRet = g_pD3DDevice->Present(0, 0, 0, 0);
	//DEBUG_D3DRESULT(hRet, "g_pD3DDevice->Present");

	hRet = g_pD3DDevice->BeginScene();

    // RenderStates need reapplying each frame, but can be re-used between draw calls
    // This forces them to be reset
    XboxRenderStates.SetDirty();
	NV2ARenderStates.SetDirty();

    // Check if we need to enable our frame-limiter
    xbox::dword_xt presentationInverval = g_Xbox_PresentationInterval_Override > 0 ? g_Xbox_PresentationInterval_Override : g_Xbox_PresentationInterval_Default;
    if ((presentationInverval != D3DPRESENT_INTERVAL_IMMEDIATE) && !g_bHack_UnlockFramerate) {
        // If the last frame completed faster than the Xbox target swap rate, wait for it

        auto targetRefreshRate = 60.0f; // TODO: Read from Xbox Display Mode

        // Determine how many 'frames' worth of time we need to wait for
        // This allows games that require a locked framerate (eg JSRF) to function correctly
        // While allowing titles with an unlocked frame-rate to not be limited
        auto multiplier = 1.0f;
        switch (presentationInverval) {
            case D3DPRESENT_INTERVAL_ONE:
            case 0x80000001: // D3DPRESENT_INTERVAL_ONE_OR_IMMEDIATE:
                multiplier = 1.0f;
                break;
            case D3DPRESENT_INTERVAL_TWO:
            case 0x80000002: // D3DPRESENT_INTERVAL_TWO_OR_IMMEDIATE:
                multiplier = 2.0f;
                break;
            case D3DPRESENT_INTERVAL_THREE:
                multiplier = 3.0f;
                break;
            case D3DPRESENT_INTERVAL_FOUR:
                multiplier = 4.0f;
                break;
        }

        // Wait until it's time for the next frame
        auto frameMs = (1000.0 / targetRefreshRate) * multiplier;
        auto targetDuration = std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<double, std::milli>(frameMs));
        auto targetTimestamp = frameStartTime + targetDuration;
        SleepPrecise(targetTimestamp);
    }

    frameStartTime = std::chrono::steady_clock::now();

	g_renderbase->UpdateFPSCounter();

	if (Flags == CXBX_SWAP_PRESENT_FORWARD) // Only do this when forwarded from Present
	{
		// TODO: print the primitives per frame with ImGui

		// TODO : Check if this should be done at Swap-not-Present-time too :
		// not really accurate because you definately dont always present on every vblank
		g_Xbox_VBlankData.Swap = g_Xbox_VBlankData.VBlank;

		if (g_Xbox_VBlankData.VBlank == g_VBLastSwap + 1)
			g_Xbox_VBlankData.Flags = 1; // D3DVBLANK_SWAPDONE
		else
		{
			g_Xbox_VBlankData.Flags = 2; // D3DVBLANK_SWAPMISSED
			g_Xbox_SwapData.MissedVBlanks++;
		}
	}

	// Handle Swap Callback function
	{
		g_Xbox_SwapData.Swap++;

		if(g_pXbox_SwapCallback != xbox::zeroptr) 
		{
				
			g_pXbox_SwapCallback(&g_Xbox_SwapData);
				
		}
	}

	DWORD result;
	if (Flags == CXBX_SWAP_PRESENT_FORWARD) // Only do this when forwarded from Present
		result = D3D_OK; // Present always returns success
	else
		result = g_Xbox_SwapData.Swap; // Swap returns number of swaps

	//before we exit swap(), we clear the dirty render target cache. which means we only cache dirty render target within the same frame.
	//todo: shall we cache the dirty render targets for multi frame?
	g_RenderTargetCache.clear();

    return result;
}
// ******************************************************************
// * patch: D3DDevice_Swap
// ******************************************************************
xbox::dword_xt WINAPI xbox::EMUPATCH(D3DDevice_Swap)
(
	dword_xt Flags
	)
{
	LOG_FUNC_ONE_ARG(Flags);
	hresult_xt hret = S_OK;
#if !USEPGRAPH_Swap
	// init pushbuffer related pointers
	DWORD* pPush_local = (DWORD*)*g_pXbox_pPush;         //pointer to current pushbuffer
	DWORD* pPush_limit = (DWORD*)*g_pXbox_pPushLimit;    //pointer to the end of current pushbuffer
	if ((unsigned int)pPush_local + 64 >= (unsigned int)pPush_limit)//check if we still have enough space
		pPush_local = (DWORD*)CxbxrImpl_MakeSpace(); //make new pushbuffer space and get the pointer to it.

	// process xbox D3D API enum and arguments and push them to pushbuffer for pgraph to handle later.
	pPush_local[0] = HLE_API_PUSHBFFER_COMMAND_64 ;
	pPush_local[1] = X_D3DAPI_ENUM::X_D3DDevice_Swap;//enum of this patched API
	pPush_local[2] = (DWORD)Flags; //total 14 DWORD space for arguments.

	//set pushbuffer pointer to the new beginning
	// always reserve 1 command DWORD, 1 API enum, and 14 argmenet DWORDs.
	*(DWORD**)g_pXbox_pPush += 0x10;
#else
	//template for syncing HLE apis with pgraf using waiting lock
	bool WaitForPGRAPH;
	WaitForPGRAPH = true;
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)&WaitForPGRAPH;// (DWORD)PrimitiveType;

	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_Swap, 1, PBTokenArray);//argCount, not necessary, default to 14

	EmuKickOff();

	while (WaitForPGRAPH)
		; //this line is must have
	hret=CxbxrImpl_Swap(Flags);
#endif
	if (XB_TRMP(D3DDevice_SetBackBufferScale))
		XB_TRMP(D3DDevice_SetBackBufferScale)(1.0, 1.0);
	else
		LOG_TEST_CASE("D3DDevice_SetBackBufferScale not available");

	return hret;

}
bool IsSupportedFormat(xbox::X_D3DFORMAT X_Format, xbox::X_D3DRESOURCETYPE XboxResourceType, DWORD D3DUsage) {
	// TODO : Nuance the following, because the Direct3D 8 docs states
	// CheckDeviceFormat is needed when D3DUSAGE_RENDERTARGET or
	// D3DUSAGE_DYNAMNIC is specified.
	// Otherwise, lookup resource type and accompanying 'SupportedFormat' array
	bool *pbSupportedFormats = g_bSupportsFormatTexture;

	switch (XboxResourceType) {
		case xbox::X_D3DRTYPE_SURFACE: {
			if (D3DUsage & D3DUSAGE_RENDERTARGET) {
				pbSupportedFormats = g_bSupportsFormatSurfaceRenderTarget;
			} else if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
				pbSupportedFormats = g_bSupportsFormatSurfaceDepthStencil;
			} else {
				pbSupportedFormats = g_bSupportsFormatSurface;
			}
			break;
		}
		case xbox::X_D3DRTYPE_VOLUME: {
			pbSupportedFormats = g_bSupportsFormatTexture; // TODO : Complete
			break;
		}
		case xbox::X_D3DRTYPE_TEXTURE: {
			if (D3DUsage & D3DUSAGE_RENDERTARGET) {
				pbSupportedFormats = g_bSupportsFormatTextureRenderTarget;
			} else if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
				pbSupportedFormats = g_bSupportsFormatTextureDepthStencil;
			} else {
				pbSupportedFormats = g_bSupportsFormatTexture;
			}
			break;
		}
		case xbox::X_D3DRTYPE_VOLUMETEXTURE: {
			pbSupportedFormats = g_bSupportsFormatVolumeTexture; // TODO : Complete
			break;
		}
		case xbox::X_D3DRTYPE_CUBETEXTURE: {
			pbSupportedFormats = g_bSupportsFormatCubeTexture; // TODO : Complete
			break;
		}
	} // switch XboxResourceType

	return pbSupportedFormats[X_Format];
}

// Was patch: IDirect3DResource8_Register
void CreateHostResource(xbox::X_D3DResource *pResource, DWORD D3DUsage, int iTextureStage, DWORD dwSize)
{
	if (pResource == xbox::zeroptr)
		return;

	// Determine the resource type name
	const char *ResourceTypeName;
	xbox::X_D3DRESOURCETYPE XboxResourceType = GetXboxD3DResourceType(pResource);

	switch (XboxResourceType) {
	case xbox::X_D3DRTYPE_NONE: ResourceTypeName = "None"; break;
	case xbox::X_D3DRTYPE_SURFACE: ResourceTypeName = "Surface"; break;
	case xbox::X_D3DRTYPE_VOLUME: ResourceTypeName = "Volume"; break;
	case xbox::X_D3DRTYPE_TEXTURE: ResourceTypeName = "Texture"; break;
	case xbox::X_D3DRTYPE_VOLUMETEXTURE: ResourceTypeName = "VolumeTexture"; break;
	case xbox::X_D3DRTYPE_CUBETEXTURE: ResourceTypeName = "CubeTexture"; break;
	case xbox::X_D3DRTYPE_VERTEXBUFFER: ResourceTypeName = "VertexBuffer"; break;
	case xbox::X_D3DRTYPE_INDEXBUFFER: ResourceTypeName = "IndexBuffer"; break;
	case xbox::X_D3DRTYPE_PUSHBUFFER: ResourceTypeName = "PushBuffer"; break;
	case xbox::X_D3DRTYPE_PALETTE: ResourceTypeName = "Palette"; break;
	case xbox::X_D3DRTYPE_FIXUP: ResourceTypeName = "Fixup"; break;
	default:
		EmuLog(LOG_LEVEL::WARNING, "CreateHostResource :-> Unrecognized Xbox Resource Type 0x%.08X", XboxResourceType);
		return;
	}

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pResource)
		LOG_FUNC_ARG(iTextureStage)
		LOG_FUNC_ARG(dwSize)
		LOG_FUNC_ARG(ResourceTypeName)
		LOG_FUNC_END;

	// Retrieve and test the xbox resource buffer address
	VAddr VirtualAddr = (VAddr)GetDataFromXboxResource(pResource);
	if ((VirtualAddr & ~PHYSICAL_MAP_BASE) == 0) {
		// TODO: Fix or handle this situation..?
		LOG_TEST_CASE("CreateHostResource : VirtualAddr == 0");
		// This is probably an unallocated resource, mapped into contiguous memory (0x80000000 OR 0xF0000000)
		EmuLog(LOG_LEVEL::WARNING, "CreateHostResource :-> %s carries no data - skipping conversion", ResourceTypeName);
		return;
	}

    switch (XboxResourceType) {
	case xbox::X_D3DRTYPE_NONE: {
		break;
	}

	case xbox::X_D3DRTYPE_SURFACE: {
		xbox::X_D3DSurface *pXboxSurface = (xbox::X_D3DSurface *)pResource;
		xbox::X_D3DBaseTexture *pParentXboxTexture = (pXboxSurface->Parent) ? (xbox::X_D3DBaseTexture*)pXboxSurface->Parent : xbox::zeroptr;

		// Don't init the Parent if the Surface and Surface Parent formats differ
		// Happens in some Outrun 2006 SetRenderTarget calls
		if (pParentXboxTexture && (pXboxSurface->Format == pParentXboxTexture->Format)) {
            // For surfaces with a parent texture, map these to a host texture first
			// TODO : Investigate how it's possible (and how we could fix) the case when
			// the following call to GetHostBaseTexture would reject non-texture resources,
			// which would seem to trigger a "CreateCubeTexture Failed!" regression.
			IDirect3DBaseTexture *pParentHostBaseTexture = GetHostBaseTexture(pParentXboxTexture, D3DUsage, iTextureStage);
			//this is for debug only, try to catch why pParentHostBaseTexture == nullptr when pParentXboxTexture != nullptr
			if(pParentHostBaseTexture==nullptr)
				pParentHostBaseTexture = GetHostBaseTexture(pParentXboxTexture, D3DUsage, iTextureStage);
            ComPtr<IDirect3DSurface> pNewHostSurface;
			switch (pParentHostBaseTexture->GetType()) {
			case D3DRTYPE_VOLUMETEXTURE: {
                LOG_TEST_CASE("Using child surface of VolumeTexture");
				// TODO
				break;
			}
			case D3DRTYPE_CUBETEXTURE: {
                // test-case : Burnout
                auto pParentHostTexture = (IDirect3DCubeTexture*)pParentHostBaseTexture;

                D3DCUBEMAP_FACES CubeMapFace = D3DCUBEMAP_FACE_POSITIVE_X;
                UINT SurfaceLevel = 0;
                GetSurfaceFaceAndLevelWithinTexture(pXboxSurface, pParentXboxTexture, SurfaceLevel, CubeMapFace);

                HRESULT hRet = pParentHostTexture->GetCubeMapSurface(CubeMapFace, SurfaceLevel, pNewHostSurface.GetAddressOf());

                DEBUG_D3DRESULT(hRet, "pHostParentTexture->GetCubeMapSurface");
                if (hRet == D3D_OK) {
                    SetHostSurface(pXboxSurface, pNewHostSurface.Get(), iTextureStage);
                    EmuLog(LOG_LEVEL::DEBUG, "CreateHostResource : Successfully created CubeTexture surface level (Face: %u, Level: %u, pResource: 0x%.08X, pNewHostSurface: 0x%.08X)",
                        CubeMapFace, SurfaceLevel, pResource, pNewHostSurface.Get());
                    return;
                }

				break;
			}
			case D3DRTYPE_TEXTURE: {
                IDirect3DTexture* pParentHostTexture = (IDirect3DTexture*)pParentHostBaseTexture;

                UINT SurfaceLevel = 0;
                GetSurfaceFaceAndLevelWithinTexture(pXboxSurface, pParentXboxTexture, SurfaceLevel);
				HRESULT hRet = pParentHostTexture->GetSurfaceLevel(SurfaceLevel, pNewHostSurface.GetAddressOf());

				DEBUG_D3DRESULT(hRet, "pHostParentTexture->GetSurfaceLevel");
				if (hRet == D3D_OK) {
					SetHostSurface(pXboxSurface, pNewHostSurface.Get(), iTextureStage);
                    EmuLog(LOG_LEVEL::DEBUG, "CreateHostResource : Successfully created Texture surface level (Level: %u, pResource: 0x%.08X, pNewHostSurface: 0x%.08X)",
						SurfaceLevel, pResource, pNewHostSurface.Get());
					return;
				}
				break;
			}
			}

			EmuLog(LOG_LEVEL::WARNING, "Failed getting host surface level - falling through to regular surface creation");
		}
		// fall through
	}
	case xbox::X_D3DRTYPE_VOLUME: {
		// Note : Use and check for null, since X_D3DRTYPE_SURFACE might fall through here (by design) 
		xbox::X_D3DVolume *pXboxVolume = (XboxResourceType == xbox::X_D3DRTYPE_VOLUME) ? (xbox::X_D3DVolume *)pResource : xbox::zeroptr;
		xbox::X_D3DVolumeTexture *pParentXboxVolumeTexture = (pXboxVolume) ? (xbox::X_D3DVolumeTexture *)pXboxVolume->Parent : xbox::zeroptr;
		if (pParentXboxVolumeTexture) {
			// For volumes with a parent volume texture, map these to a host volume texture first
			IDirect3DVolumeTexture *pParentHostVolumeTexture = GetHostVolumeTexture(pParentXboxVolumeTexture, iTextureStage);
			UINT VolumeLevel = 0; // TODO : Derive actual level based on pXboxVolume->Data delta to pParentXboxVolumeTexture->Data
			ComPtr<IDirect3DVolume> pNewHostVolume;
			HRESULT hRet = pParentHostVolumeTexture->GetVolumeLevel(VolumeLevel, pNewHostVolume.GetAddressOf());
			DEBUG_D3DRESULT(hRet, "pParentHostVolumeTexture->GetVolumeLevel");
			if (hRet == D3D_OK) {
				SetHostVolume(pXboxVolume, pNewHostVolume.Get(), iTextureStage);
				EmuLog(LOG_LEVEL::DEBUG, "CreateHostResource : Successfully created volume level (%u, 0x%.08X, 0x%.08X)",
					VolumeLevel, pResource, pNewHostVolume.Get());
				return;
			}

			EmuLog(LOG_LEVEL::WARNING, "Failed getting host volume level - falling through to regular volume creation");
		}
		// fall through
	}
	case xbox::X_D3DRTYPE_TEXTURE:
	case xbox::X_D3DRTYPE_VOLUMETEXTURE:
	case xbox::X_D3DRTYPE_CUBETEXTURE: {
		xbox::X_D3DPixelContainer *pPixelContainer = (xbox::X_D3DPixelContainer*)pResource;
		xbox::X_D3DFORMAT X_Format = GetXboxPixelContainerFormat(pPixelContainer);
		D3DPOOL D3DPool = D3DPOOL_DEFAULT; // TODO : Nuance D3DPOOL where/when needed

		if (EmuXBFormatIsDepthBuffer(X_Format)) {
			D3DUsage |= D3DUSAGE_DEPTHSTENCIL;
		}
		else if (pPixelContainer == g_pXbox_RenderTarget) {
			if (EmuXBFormatIsRenderTarget(X_Format))
				D3DUsage |= D3DUSAGE_RENDERTARGET;
			else
				EmuLog(LOG_LEVEL::WARNING, "Updating RenderTarget %s with an incompatible format!", ResourceTypeName);
		}

		// Determine the format we'll be using on host D3D
		D3DFORMAT PCFormat;
		bool bConvertToARGB = false;

		if (EmuXBFormatRequiresConversionToARGB(X_Format)) {
			bConvertToARGB = true;
			PCFormat = D3DFMT_A8R8G8B8;

			// Unset D3DUSAGE_DEPTHSTENCIL: It's not possible for ARGB textures to be depth stencils
			// Fixes CreateTexture error in Virtua Cop 3 (Chihiro)
			D3DUsage &= ~D3DUSAGE_DEPTHSTENCIL;
		}
		else {
			// Does host CheckDeviceFormat() succeed on this format?
			if (IsSupportedFormat(X_Format, XboxResourceType, D3DUsage)) {
				// Then use matching host format
				PCFormat = EmuXB2PC_D3DFormat(X_Format);
			}
			else {
				if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
					// If it was a depth stencil, fall back to a known supported depth format
					EmuLog(LOG_LEVEL::WARNING, "Xbox %s Format %x will be converted to D3DFMT_D24S8", ResourceTypeName, X_Format);
					PCFormat = D3DFMT_D24S8;
				} else if (EmuXBFormatCanBeConvertedToARGB(X_Format)) {
					EmuLog(LOG_LEVEL::WARNING, "Xbox %s Format %x will be converted to ARGB", ResourceTypeName, X_Format);
					bConvertToARGB = true;
					PCFormat = D3DFMT_A8R8G8B8;
				} else {
					// Otherwise, use a best matching format
					/*CxbxrAbort*/EmuLog(LOG_LEVEL::WARNING, "Encountered a completely incompatible %s format!", ResourceTypeName);
					PCFormat = EmuXB2PC_D3DFormat(X_Format);
				}
			}
		}
		
		// Update D3DPool accordingly to the active D3DUsage flags
		if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
			D3DPool = D3DPOOL_DEFAULT;
		}
		if (D3DUsage & D3DUSAGE_RENDERTARGET) {
			D3DPool = D3DPOOL_DEFAULT;
		}
		if (D3DUsage & D3DUSAGE_DYNAMIC) {
			D3DPool = D3DPOOL_DEFAULT;
		}

		// Interpret Width/Height/BPP
		bool bCubemap = pPixelContainer->Format & X_D3DFORMAT_CUBEMAP;
		bool bSwizzled = EmuXBFormatIsSwizzled(X_Format);
		bool bCompressed = EmuXBFormatIsCompressed(X_Format);
		UINT dwBPP = EmuXBFormatBytesPerPixel(X_Format);
		UINT dwMipMapLevels = CxbxGetPixelContainerMipMapLevels(pPixelContainer);
		UINT xboxWidth, xboxHeight, dwDepth, dwRowPitch, dwSlicePitch;

		// Interpret Width/Height/BPP
		CxbxGetPixelContainerMeasures(pPixelContainer, 0, &xboxWidth, &xboxHeight, &dwDepth, &dwRowPitch, &dwSlicePitch);

		// Host width and height dimensions
		UINT hostWidth = xboxWidth;
		UINT hostHeight = xboxHeight;
		// Upscale rendertargets and depth surfaces
		if (D3DUsage & (X_D3DUSAGE_DEPTHSTENCIL | X_D3DUSAGE_RENDERTARGET)) {
			hostWidth *= g_RenderUpscaleFactor;
			hostHeight *= g_RenderUpscaleFactor;
		}

		// Each mip-map level is 1/2 the size of the previous level
		// D3D9 forbids creation of a texture with more mip-map levels than it is divisible
		// EG: A 256x256 texture cannot have more than 8 levels, since that would create a texture smaller than 1x1
		// Because of this, we need to cap dwMipMapLevels when required
		if (dwMipMapLevels > 0) {
			// Calculate how many mip-map levels it takes to get to a texture of 1 pixels in either dimension
			UINT highestMipMapLevel = 1;
			UINT width = xboxWidth; UINT height = xboxHeight;
			while (width > 1 || height > 1) {
				width /= 2;
				height /= 2;
				highestMipMapLevel++;
			}

			// If the desired mip-map level was higher than the maximum possible, cap it
			// Test case: Shin Megami Tensei: Nine
			if (dwMipMapLevels > highestMipMapLevel) {
				LOG_TEST_CASE("Too many mip-map levels");
				dwMipMapLevels = highestMipMapLevel;
			}
		}

		if (dwDepth != 1) {
			LOG_TEST_CASE("CreateHostResource : Depth != 1");
		}

		// One of these will be created : each also has an intermediate copy to allow UpdateTexture to work
        // This means we don't need to lock the GPU resource anymore, so we can use D3DPOOL_DEFAULT to allow Stretch/CopyRects to work!
		ComPtr<IDirect3DSurface> pNewHostSurface; // for X_D3DRTYPE_SURFACE
		ComPtr<IDirect3DVolume> pNewHostVolume; // for X_D3DRTYPE_VOLUME
		ComPtr<IDirect3DTexture> pNewHostTexture; // for X_D3DRTYPE_TEXTURE
		ComPtr<IDirect3DTexture> pIntermediateHostTexture;
		ComPtr<IDirect3DVolumeTexture> pNewHostVolumeTexture; // for X_D3DRTYPE_VOLUMETEXTURE
		ComPtr<IDirect3DVolumeTexture> pIntermediateHostVolumeTexture;
		ComPtr<IDirect3DCubeTexture> pNewHostCubeTexture; // for X_D3DRTYPE_CUBETEXTURE
		ComPtr<IDirect3DCubeTexture> pIntermediateHostCubeTexture;

		HRESULT hRet;

		//store the PCFormat of created host resource for the ease of bumpmap/none bumpmap signed/unsigned format conversion.
		SetHostResourcePCFormat(pResource, PCFormat);

		// Create the surface/volume/(volume/cube/)texture
		switch (XboxResourceType) {
		case xbox::X_D3DRTYPE_SURFACE: {
			if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
				//PCFormat = (D3DFORMAT)MAKEFOURCC('I', 'N', 'T', 'Z');//
				
				hRet = g_pD3DDevice->CreateDepthStencilSurface(hostWidth, hostHeight, PCFormat,
					g_EmuCDPD.HostPresentationParameters.MultiSampleType,
					0, // MultisampleQuality
					false, // Discard
					pNewHostSurface.GetAddressOf(),
					nullptr // pSharedHandle
				);
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateDepthStencilSurface");
				
				/*
				// Note : This handles both (D3DUsage & D3DUSAGE_RENDERTARGET) and otherwise alike
				hRet = g_pD3DDevice->CreateTexture(hostWidth, hostHeight,
					1, // Levels
					D3DUSAGE_DEPTHSTENCIL, // Usage always as render target
					PCFormat,
					D3DPool, // D3DPOOL_DEFAULT
					pNewHostTexture.GetAddressOf(),
					nullptr // pSharedHandle
				);
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateTexture");

				if (hRet == D3D_OK) {
					HRESULT hRet2 = pNewHostTexture->GetSurfaceLevel(0, pNewHostSurface.GetAddressOf());
					DEBUG_D3DRESULT(hRet2, "pNewHostTexture->pNewHostSurface");
					pNewHostTexture.Reset();
				}
				*/
			}
			else {
				// Note : This handles both (D3DUsage & D3DUSAGE_RENDERTARGET) and otherwise alike
				hRet = g_pD3DDevice->CreateTexture(hostWidth, hostHeight,
					1, // Levels
					D3DUSAGE_RENDERTARGET, // Usage always as render target
					PCFormat,
					D3DPool, // D3DPOOL_DEFAULT
					pNewHostTexture.GetAddressOf(),
					nullptr // pSharedHandle
				);
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateTexture");

				if (hRet == D3D_OK) {
					HRESULT hRet2 = pNewHostTexture->GetSurfaceLevel(0, pNewHostSurface.GetAddressOf());
					DEBUG_D3DRESULT(hRet2, "pNewHostTexture->pNewHostSurface");
					pNewHostTexture.Reset();
                }
			}

			// First fail, retry with a fallback format
			// If this succeeds, the surface may not render correctly, but it won't crash
			if (hRet != D3D_OK) {
				if (D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
					EmuLog(LOG_LEVEL::WARNING, "CreateDepthStencilSurface Failed\n\nError: %s\nDesc: %s",
						DXGetErrorString(hRet), DXGetErrorDescription(hRet));
				}
				else {
					EmuLog(LOG_LEVEL::WARNING, "CreateTexture Failed\n\nError: %s\nDesc: %s",
						DXGetErrorString(hRet), DXGetErrorDescription(hRet));
				}

				EmuLog(LOG_LEVEL::WARNING, "Trying Fallback");
				hRet = g_pD3DDevice->CreateOffscreenPlainSurface(hostWidth, hostHeight, PCFormat, D3DPool, pNewHostSurface.GetAddressOf(), nullptr);
			}

			// If the fallback failed, show an error and exit execution.
			if (hRet != D3D_OK) {
				// We cannot safely continue in this state.
				CxbxrAbort("CreateImageSurface Failed!\n\nError: %s\nDesc: %s",
					DXGetErrorString(hRet), DXGetErrorDescription(hRet));
			}

			SetHostSurface(pResource, pNewHostSurface.Get(), iTextureStage);
			EmuLog(LOG_LEVEL::DEBUG, "CreateHostResource : Successfully created %s (0x%.08X, 0x%.08X)",
				ResourceTypeName, pResource, pNewHostSurface.Get());
			EmuLog(LOG_LEVEL::DEBUG, "CreateHostResource : Width : %d, Height : %d, Format : %d",
				hostWidth, hostHeight, PCFormat);
			break;
		}

		case xbox::X_D3DRTYPE_VOLUME: {
			LOG_UNIMPLEMENTED();
			// Note : Host D3D can only(?) retrieve a volume like this :
			// hRet = pNewHostVolumeTexture->GetVolumeLevel(level, &pNewHostVolume);
			// So, we need to do this differently - we need to step up to the containing VolumeTexture,
			// and retrieve and convert all of it's GetVolumeLevel() slices.
			pNewHostVolume = nullptr;
			// SetHostVolume(pResource, pNewHostVolume, iTextureStage);
			// EmuLog(LOG_LEVEL::DEBUG, "CreateHostResource : Successfully created %s (0x%.08X, 0x%.08X)",
			//	ResourceTypeName, pResource, pNewHostVolume);
			break;
		}

		case xbox::X_D3DRTYPE_TEXTURE: {
			hRet = g_pD3DDevice->CreateTexture(hostWidth, hostHeight, dwMipMapLevels,
				D3DUsage, PCFormat, D3DPool, pNewHostTexture.GetAddressOf(),
				nullptr
			);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateTexture");

			// If the above failed, we might be able to use an ARGB texture instead
			if ((hRet != D3D_OK) && (PCFormat != D3DFMT_A8R8G8B8) && EmuXBFormatCanBeConvertedToARGB(X_Format)) {
				hRet = g_pD3DDevice->CreateTexture(hostWidth, hostHeight, dwMipMapLevels,
					D3DUsage, D3DFMT_A8R8G8B8, D3DPool, pNewHostTexture.GetAddressOf(),
					nullptr
				);
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateTexture(D3DFMT_A8R8G8B8)");

				if (hRet == D3D_OK) {
					// Okay, now this works, make sure the texture gets converted
					bConvertToARGB = true;
					PCFormat = D3DFMT_A8R8G8B8;
				}
			}

            // Now create our intermediate texture for UpdateTexture, but not for render targets or depth stencils
            if (hRet == D3D_OK && (D3DUsage & D3DUSAGE_RENDERTARGET) == 0 && (D3DUsage & D3DUSAGE_DEPTHSTENCIL) == 0) {
                hRet = g_pD3DDevice->CreateTexture(hostWidth, hostHeight, dwMipMapLevels,
                    0, PCFormat, D3DPOOL_SYSTEMMEM, pIntermediateHostTexture.GetAddressOf(),
                    nullptr
                );
            }

			/*if(FAILED(hRet))
			{
				hRet = g_pD3DDevice->CreateTexture
				(
					dwWidth, dwHeight, dwMipMapLevels, D3DUsage, PCFormat,
					D3DPOOL_SYSTEMMEM, &pNewHostTexture,
					nullptr
					);
				DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateTexture(D3DPOOL_SYSTEMMEM)");
			}*/


			if (hRet != D3D_OK) {
				CxbxrAbort("CreateTexture Failed!\n\n"
					"Error: 0x%X\nFormat: %d\nDimensions: %dx%d", hRet, PCFormat, hostWidth, hostHeight);
			}

			SetHostTexture(pResource, pNewHostTexture.Get(), iTextureStage);
			EmuLog(LOG_LEVEL::DEBUG, "CreateHostResource : Successfully created %s (0x%.08X, 0x%.08X)",
				ResourceTypeName, pResource, pNewHostTexture.Get());
			break;
		}

		case xbox::X_D3DRTYPE_VOLUMETEXTURE: {
			hRet = g_pD3DDevice->CreateVolumeTexture(hostWidth, hostHeight, dwDepth,
				dwMipMapLevels, D3DUsage, PCFormat, D3DPool, pNewHostVolumeTexture.GetAddressOf(),
				nullptr
			);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateVolumeTexture");

            // Now create our intermediate texture for UpdateTexture, but not for render targets or depth stencils
            if (hRet == D3D_OK && (D3DUsage & D3DUSAGE_RENDERTARGET) == 0 && (D3DUsage & D3DUSAGE_DEPTHSTENCIL) == 0) {
                hRet = g_pD3DDevice->CreateVolumeTexture(hostWidth, hostHeight, dwDepth,
                    dwMipMapLevels, 0, PCFormat, D3DPOOL_SYSTEMMEM, pIntermediateHostVolumeTexture.GetAddressOf(),
                    nullptr
                );
            }

			if (hRet != D3D_OK) {
				CxbxrAbort("CreateVolumeTexture Failed!\n\nError: %s\nDesc: %s",
					DXGetErrorString(hRet), DXGetErrorDescription(hRet));
			}

			SetHostVolumeTexture(pResource, pNewHostVolumeTexture.Get(), iTextureStage);
			EmuLog(LOG_LEVEL::DEBUG, "CreateHostResource : Successfully created %s (0x%.08X, 0x%.08X)",
				ResourceTypeName, pResource, pNewHostVolumeTexture.Get());
			break;
		}

		case xbox::X_D3DRTYPE_CUBETEXTURE: {
			EmuLog(LOG_LEVEL::DEBUG, "CreateCubeTexture(%d, %d, 0, %d, D3DPOOL_DEFAULT)", hostWidth,
				dwMipMapLevels, PCFormat);

			hRet = g_pD3DDevice->CreateCubeTexture(hostWidth, dwMipMapLevels, D3DUsage,
				PCFormat, D3DPool, pNewHostCubeTexture.GetAddressOf(),
				nullptr
			);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateCubeTexture");

            // Now create our intermediate texture for UpdateTexture, but not for render targets or depth stencils
            if (hRet == D3D_OK && (D3DUsage & D3DUSAGE_RENDERTARGET) == 0 && (D3DUsage & D3DUSAGE_DEPTHSTENCIL) == 0) {
                hRet = g_pD3DDevice->CreateCubeTexture(hostWidth, dwMipMapLevels, 0,
                    PCFormat, D3DPOOL_SYSTEMMEM, pIntermediateHostCubeTexture.GetAddressOf(),
                    nullptr
                );
            }

			if (hRet != D3D_OK) {
				CxbxrAbort("CreateCubeTexture Failed!\n\nError: \nDesc: "/*,
					DXGetErrorString(hRet), DXGetErrorDescription(hRet)*/);
			}

			SetHostCubeTexture(pResource, pNewHostCubeTexture.Get(), iTextureStage);
			// TODO : Cube face surfaces can be used as a render-target,
			// so we need to associate host surfaces to each surface of this cube texture
            // However, we can't do it here: On Xbox, a new Surface is created on every call to
            // GetCubeMapSurface, so it needs to be done at surface conversion time by looking up
            // the parent CubeTexture
			EmuLog(LOG_LEVEL::DEBUG, "CreateHostResource : Successfully created %s (0x%.08X, 0x%.08X)",
				ResourceTypeName, pResource, pNewHostCubeTexture.Get());
			break;
		}
		} // switch XboxResourceType

        // If this resource is a render target or depth stencil, don't attempt to lock/copy it as it won't work anyway
        // In this case, we simply return
        if (D3DUsage & D3DUSAGE_RENDERTARGET || D3DUsage & D3DUSAGE_DEPTHSTENCIL) {
            return;
        }

		DWORD dwCubeFaceOffset = 0;
		D3DCUBEMAP_FACES last_face = (bCubemap) ? D3DCUBEMAP_FACE_NEGATIVE_Z : D3DCUBEMAP_FACE_POSITIVE_X;

		// Block size only applies to compressed DXT formats
		// DXT1 block size is 8 bytes
		// Other Xbox DXT formats are 16 bytes
		DWORD blockSize = 0;
		if (bCompressed) {
			blockSize = X_Format == xbox::X_D3DFMT_DXT1 ? 8 : 16;
		}

		DWORD actualSlicePitch = dwSlicePitch;
		for (int face = D3DCUBEMAP_FACE_POSITIVE_X; face <= last_face; face++) {
			// As we iterate through mipmap levels, we'll adjust the source resource offset
			DWORD dwMipOffset = 0;
			DWORD pxMipWidth = xboxWidth; // the current mip width in pixels
			DWORD pxMipHeight = xboxHeight; // the current mip height in pixels
			DWORD pxMipDepth = dwDepth; // the current mip depth in pixels
			DWORD dwMipRowPitch = dwRowPitch; // bytes to the next row of pixels (or DXT blocks)

			for (unsigned int mipmap_level = 0; mipmap_level < dwMipMapLevels; mipmap_level++) {

				// Calculate size of this mipmap level
				DWORD numRows = pxMipHeight;

				if (bCompressed) {
					// Each row contains a 4x4 pixel DXT blocks, instead of single pixels
					// So divide by 4 to get the number of rows
					numRows = (numRows + 3) / 4;

					if (dwDepth > 1) {
						LOG_TEST_CASE("Unsupported compressed volume texture");
					}
				}

				DWORD mip2dSize = dwMipRowPitch * numRows; // the size of one layer of the mip slice
				DWORD mipSlicePitch = mip2dSize * pxMipDepth; // the total size of the mip slice (depth is only > 1 for volume textures)

				// Lock the host resource
				D3DLOCKED_RECT LockedRect = {};
				D3DLOCKED_BOX LockedBox = {};
				DWORD D3DLockFlags = D3DLOCK_NOSYSLOCK;

				switch (XboxResourceType) {
				case xbox::X_D3DRTYPE_SURFACE:
					hRet = pNewHostSurface->LockRect(&LockedRect, nullptr, D3DLockFlags);
					break;
				case xbox::X_D3DRTYPE_VOLUME:
					hRet = pNewHostVolume->LockBox(&LockedBox, nullptr, D3DLockFlags);
					break;
				case xbox::X_D3DRTYPE_TEXTURE:
					hRet = pIntermediateHostTexture->LockRect(mipmap_level, &LockedRect, nullptr, D3DLockFlags);
					break;
				case xbox::X_D3DRTYPE_VOLUMETEXTURE:
					hRet = pIntermediateHostVolumeTexture->LockBox(mipmap_level, &LockedBox, nullptr, D3DLockFlags);
					break;
				case xbox::X_D3DRTYPE_CUBETEXTURE:
					hRet = pIntermediateHostCubeTexture->LockRect((D3DCUBEMAP_FACES)face, mipmap_level, &LockedRect, nullptr, D3DLockFlags);
					break;
				default:
					assert(false);
				} // switch XboxResourceType

				if (hRet != D3D_OK) {
					EmuLog(LOG_LEVEL::WARNING, "Locking host %s failed!", ResourceTypeName);
					continue; // This often happens on depth-stencil surfaces - let's ignore their copies for now
				}

				// Determine destination buffer attributes
				uint8_t *pDst;
				DWORD dwDstRowPitch;
				DWORD dwDstSlicePitch;

				switch (XboxResourceType) {
				case xbox::X_D3DRTYPE_VOLUME:
				case xbox::X_D3DRTYPE_VOLUMETEXTURE:
					pDst = (uint8_t *)LockedBox.pBits;
					dwDstRowPitch = LockedBox.RowPitch;
					dwDstSlicePitch = LockedBox.SlicePitch;
					break;
				default:
					pDst = (uint8_t *)LockedRect.pBits;
					dwDstRowPitch = LockedRect.Pitch;
					dwDstSlicePitch = 0;
				}

				uint8_t *pSrc = (uint8_t *)VirtualAddr + dwCubeFaceOffset + dwMipOffset;

				// If this is the final mip of the first cube face, set the cube face size
				if (face == D3DCUBEMAP_FACE_POSITIVE_X && mipmap_level >= dwMipMapLevels - 1) {
					actualSlicePitch = ROUND_UP(((UINT)pSrc + mipSlicePitch) - (UINT)VirtualAddr, X_D3DTEXTURE_CUBEFACE_ALIGNMENT);
				}
				
				// Copy texture data to the host resource
				if (bConvertToARGB) {
					EmuLog(LOG_LEVEL::DEBUG, "Unsupported texture format, expanding to D3DFMT_A8R8G8B8");

					// In case where there is a palettized texture without a palette attached,
					// fill it with zeroes for now. This might not be correct, but it prevents a crash.
					// Test case: DRIV3R
					void* data;
					int size;
					data = g_pXbox_Palette_Data[iTextureStage];
					size = g_Xbox_Palette_Size[iTextureStage];
					if (is_pgraph_using_NV2A_Kelvin()) {
						data = g_pNV2A_Palette_Data[iTextureStage];
						size = g_NV2A_Palette_Size[iTextureStage];
					}
					bool missingPalette = X_Format == xbox::X_D3DFMT_P8 && data/*g_pXbox_Palette_Data[iTextureStage]*/ == nullptr;
					if (missingPalette) {
						LOG_TEST_CASE("Palettized texture bound without a palette");

						memset(pDst, 0, dwDstRowPitch * pxMipHeight);
					}
					else {
						// Try to convert to ARGB
						if (!ConvertD3DTextureToARGBBuffer(
							X_Format,
							pSrc, pxMipWidth, pxMipHeight, dwMipRowPitch, mip2dSize,
							pDst, dwDstRowPitch, dwDstSlicePitch,
							pxMipDepth,//used pxMipDepth here because in 3D mip map the 3rd dimension also shrinked to 1/2 at each mip level.
							data/*g_pXbox_Palette_Data[iTextureStage]*/)) {
							CxbxrAbort("Unhandled conversion!");
						}
					}
				}else if (X_Format == xbox::X_D3DFMT_L6V5U5 || X_Format == xbox::X_D3DFMT_LIN_L6V5U5
					      || X_Format == xbox::X_D3DFMT_V16U16 || X_Format == xbox::X_D3DFMT_LIN_V16U16) {
					// Try to convert to ARGB
					bool bIsV16U16 = (X_Format == xbox::X_D3DFMT_V16U16 || X_Format == xbox::X_D3DFMT_LIN_V16U16) ? true : false;
					extern bool ConvertL6V5U5ToX8L8V8U8Buffer(
						xbox::X_D3DFORMAT X_Format,
						uint8_t * pSrc,
						int SrcWidth, int SrcHeight, int SrcRowPitch, int SrcSlicePitch,
						uint8_t * pDst, int DstRowPitch, int DstSlicePitch,
						unsigned int uiDepth,
						bool bIsV16U16
					);
					if (!ConvertL6V5U5ToX8L8V8U8Buffer(
						X_Format,
						pSrc, pxMipWidth, pxMipHeight, dwMipRowPitch, mip2dSize,
						pDst, dwDstRowPitch, dwDstSlicePitch,
						pxMipDepth,//used pxMipDepth here because in 3D mip map the 3rd dimension also shrinked to 1/2 at each mip level.
						bIsV16U16/*g_pXbox_Palette_Data[iTextureStage]*/)) {
						CxbxrAbort("Unhandled conversion, failed in converting X_D3DFMT_L6V5U5 or X_D3DFMT_LIN_L6V5U5 to D3DFMT_X8L8V8U8!");
					}
				}else if (bSwizzled) {
					// Unswizzle the texture data into the host texture
					EmuUnswizzleBox(
						pSrc, pxMipWidth, pxMipHeight, pxMipDepth,
						dwBPP, 
						pDst, dwDstRowPitch, dwDstSlicePitch
					);
				}
				else if (bCompressed) {
					memcpy(pDst, pSrc, mip2dSize);
				}
				else {
					if (dwDstRowPitch == dwMipRowPitch) {
						// Source and destination layout match - simple copy
						memcpy(pDst, pSrc, mip2dSize);
					}
					else {
						// Copy accounting for different row pitch
						for (DWORD v = 0; v < pxMipHeight; v++) {
							memcpy(pDst, pSrc, pxMipWidth * dwBPP);
							pDst += dwDstRowPitch;
							pSrc += dwMipRowPitch;
						}
					}
				}

				// Unlock the host resource
				switch (XboxResourceType) {
				case xbox::X_D3DRTYPE_SURFACE:
					hRet = pNewHostSurface->UnlockRect();
					break;
				case xbox::X_D3DRTYPE_VOLUME:
					hRet = pNewHostVolume->UnlockBox();
					break;
				case xbox::X_D3DRTYPE_TEXTURE:
					hRet = pIntermediateHostTexture->UnlockRect(mipmap_level);
					break;
				case xbox::X_D3DRTYPE_VOLUMETEXTURE:
					hRet = pIntermediateHostVolumeTexture->UnlockBox(mipmap_level);
					break;
				case xbox::X_D3DRTYPE_CUBETEXTURE:
					hRet = pIntermediateHostCubeTexture->UnlockRect((D3DCUBEMAP_FACES)face, mipmap_level);
					break;
				default:
					assert(false);
				}

				if (hRet != D3D_OK) {
					EmuLog(LOG_LEVEL::WARNING, "Unlocking host %s failed!", ResourceTypeName);
				}

				// Calculate the next mipmap level dimensions
				dwMipOffset += mipSlicePitch;
				if (pxMipWidth > 1) {
					pxMipWidth /= 2;

					// Update the row pitch
					dwMipRowPitch /= 2;

					// The pitch can't be less than a block
					if (dwMipRowPitch < blockSize) {
						dwMipRowPitch = blockSize;
					}
				}

				if (pxMipHeight > 1) {
					pxMipHeight /= 2;
				}

				if (pxMipDepth > 1) {
					pxMipDepth /= 2;//this is for 3D volumeTexture mip-map, it shrinked down to 1/2 in 3 dimensions. this variable should be used.
				}
			} // for mipmap levels

			dwCubeFaceOffset += actualSlicePitch;
		} // for cube faces


        // Copy from the intermediate resource to the final host resource
        // This is necessary because CopyRects/StretchRects only works on resources in the DEFAULT pool
        // But resources in this pool are not lockable: We must use UpdateSurface/UpdateTexture instead!
        switch (XboxResourceType) {
        case xbox::X_D3DRTYPE_SURFACE:
        case xbox::X_D3DRTYPE_VOLUME:
            // We didn't use a copy for Surfaces or Volumes
            break;
        case xbox::X_D3DRTYPE_TEXTURE:
            g_pD3DDevice->UpdateTexture(pIntermediateHostTexture.Get(), pNewHostTexture.Get());
            break;
        case xbox::X_D3DRTYPE_VOLUMETEXTURE:
            g_pD3DDevice->UpdateTexture(pIntermediateHostVolumeTexture.Get(), pNewHostVolumeTexture.Get());
            break;
        case xbox::X_D3DRTYPE_CUBETEXTURE:
            g_pD3DDevice->UpdateTexture(pIntermediateHostCubeTexture.Get(), pNewHostCubeTexture.Get());
            break;
        default:
            assert(false);
        }

        if (hRet != D3D_OK) {
            EmuLog(LOG_LEVEL::WARNING, "Updating host %s failed!", ResourceTypeName);
        }

		// Debug resource dumping
//#define _DEBUG_DUMP_TEXTURE_REGISTER "D:\\"
#ifdef _DEBUG_DUMP_TEXTURE_REGISTER
		bool bDumpConvertedTextures = true;  // TODO : Make this a runtime changeable setting
		if (bDumpConvertedTextures) {
			char szFilePath[MAX_PATH];

			switch (XboxResourceType) {
			case xbox::X_D3DRTYPE_SURFACE: {
				static int dwDumpSurface = 0;
				sprintf(szFilePath, _DEBUG_DUMP_TEXTURE_REGISTER "%.03d-Surface%.03d.dds", X_Format, dwDumpSurface++);
				D3DXSaveSurfaceToFileA(szFilePath, D3DXIFF_DDS, pNewHostSurface, nullptr, nullptr);
				break;
			}
			case xbox::X_D3DRTYPE_VOLUME: {
				// TODO
				break;
			}
			case xbox::X_D3DRTYPE_TEXTURE: {
				static int dwDumpTexure = 0;
				sprintf(szFilePath, _DEBUG_DUMP_TEXTURE_REGISTER "%.03d-Texture%.03d.dds", X_Format, dwDumpTexure++);
				D3DXSaveTextureToFileA(szFilePath, D3DXIFF_DDS, pNewHostTexture, nullptr);
				break;
			}
			case xbox::X_D3DRTYPE_VOLUMETEXTURE: {
				// TODO
				break;
			}
			case xbox::X_D3DRTYPE_CUBETEXTURE: {
				static int dwDumpCubeTexture = 0;
				for (unsigned int face = D3DCUBEMAP_FACE_POSITIVE_X; face <= D3DCUBEMAP_FACE_NEGATIVE_Z; face++) {
					IDirect3DSurface *pSurface;
					if (D3D_OK == pNewHostCubeTexture->GetCubeMapSurface((D3DCUBEMAP_FACES)face, 0, &pSurface)) {
						sprintf(szFilePath, _DEBUG_DUMP_TEXTURE_REGISTER "%.03d-CubeTexure%.03d-%d.dds", X_Format, dwDumpCubeTexture, face);
						D3DXSaveSurfaceToFileA(szFilePath, D3DXIFF_DDS, pSurface, nullptr, nullptr);
						pSurface->Release();
					}
				}
				dwDumpCubeTexture++;
				break;
			}
			} // switch XboxResourceType
		}
#endif

		break;
	}

	// case X_D3DRTYPE_VERTEXBUFFER: { break; } // TODO

	// case X_D3DRTYPE_INDEXBUFFER: { break; } // TODO

	case xbox::X_D3DRTYPE_PUSHBUFFER: {
		xbox::X_D3DPushBuffer *pPushBuffer = (xbox::X_D3DPushBuffer*)pResource;

		// create push buffer
		dwSize = g_VMManager.QuerySize(VirtualAddr);
		if (dwSize == 0) {
			// TODO: once this is known to be working, remove the warning
			EmuLog(LOG_LEVEL::WARNING, "Push buffer allocation size unknown");
			pPushBuffer->Lock = X_D3DRESOURCE_LOCK_FLAG_NOSIZE;
			break;
		}

		EmuLog(LOG_LEVEL::DEBUG, "CreateHostResource : Successfully created %S (0x%.08X, 0x%.08X, 0x%.08X)", ResourceTypeName, pResource->Data, pPushBuffer->Size, pPushBuffer->AllocationSize);
		break;
	}

	//case X_D3DRTYPE_PALETTE: { break;	}

	case xbox::X_D3DRTYPE_FIXUP: {
		xbox::X_D3DFixup *pFixup = (xbox::X_D3DFixup*)pResource;

		EmuLog(LOG_LEVEL::WARNING, "X_D3DRTYPE_FIXUP is not yet supported\n"
			"0x%.08X (pFixup->Common) \n"
			"0x%.08X (pFixup->Data)   \n"
			"0x%.08X (pFixup->Lock)   \n"
			"0x%.08X (pFixup->Run)    \n"
			"0x%.08X (pFixup->Next)   \n"
			"0x%.08X (pFixup->Size)   \n", pFixup->Common, pFixup->Data, pFixup->Lock, pFixup->Run, pFixup->Next, pFixup->Size);
		break;
	}
    } // switch XboxResourceType
}

D3DXVECTOR4 toVector(D3DCOLOR color) {
	D3DXVECTOR4 v;
	// ARGB to XYZW
	v.w = (color >> 24 & 0xFF) / 255.f;
	v.x = (color >> 16 & 0xFF) / 255.f;
	v.y = (color >> 8 & 0xFF) / 255.f;
	v.z = (color >> 0 & 0xFF) / 255.f;
	return v;
}

D3DCOLOR FromVector(D3DXVECTOR4 v) {
	D3DCOLOR color;
	// XYZW to ARGB 
	color=((DWORD)(v.w* 255.f)&0xFF)<< 24 ;
	color |= ((DWORD)(v.x * 255.f) & 0xFF) << 16;
	color |= ((DWORD)(v.y * 255.f) & 0xFF) << 8;
	color |= ((DWORD)(v.z * 255.f) & 0xFF) ;
	return color;
}
D3DCOLOR FromVector(D3DCOLORVALUE v) {
	D3DCOLOR color;
	// XYZW to ARGB 
	color = ((DWORD)(v.a * 255.f) & 0xFF) << 24;
	color |= ((DWORD)(v.r * 255.f) & 0xFF) << 16;
	color |= ((DWORD)(v.g * 255.f) & 0xFF) << 8;
	color |= ((DWORD)(v.b * 255.f) & 0xFF);
	return color;
}


D3DXVECTOR4 toVector(D3DCOLORVALUE val) {
	return D3DXVECTOR4(val.r, val.g, val.b, val.a);
}

extern bool is_pgraph_DirectModelView(void);
extern bool is_pgraph_using_NV2A_Kelvin();// tmp glue
extern xbox::X_D3DLIGHT8* CxbxrGetLight8Ptr(int lightNum);
D3DMATRIX g_xbox_transform_ModelView;
D3DMATRIX g_xbox_transform_InverseModelView;
D3DMATRIX g_xbox_transform_Composite;
D3DMATRIX g_xbox_DirectModelView_View;
D3DMATRIX g_xbox_DirectModelView_InverseView;
D3DMATRIX g_xbox_DirectModelView_World;
D3DMATRIX g_xbox_DirectModelView_Projection;
D3DMATRIX g_xbox_DirectModelView_InverseWorldViewTransposed;
D3DMATRIX g_xbox_transform_Projection;
D3DMATRIX g_xbox_transform_ViewportTransform;
D3DMATRIX g_xbox_transform_ProjectionViewportTransform;

extern inline bool is_pgraph_using_NV2A_Kelvin(void);
static D3DMATRIX * g_xbox_ProjectionViewportTransform = nullptr;

void UpdateFixedFunctionShaderLight(int d3dLightIndex, Light* pShaderLight, D3DXVECTOR4* pLightAmbient) {
	if (d3dLightIndex == -1) {
		pShaderLight->Type = 0; // Disable the light
		return;
	}

	auto d3dLight = &d3d8LightState.Lights[d3dLightIndex];
	D3DXMATRIX viewTransform;
	// use lights composed from kelvin.
	if (is_pgraph_using_NV2A_Kelvin()) {
		d3dLight=CxbxrGetLight8Ptr(d3dLightIndex);
		// g_xbox_DirectModelView_View was set to )d3d8TransformState.Transforms[xbox::X_D3DTS_VIEW]
		viewTransform = g_xbox_DirectModelView_View;
		//viewTransform = (D3DXMATRIX)d3d8TransformState.Transforms[xbox::X_D3DTS_VIEW];
	}else {
		//d3dLight = &d3d8LightState.Lights[d3dLightIndex];
		viewTransform = (D3DXMATRIX)d3d8TransformState.Transforms[xbox::X_D3DTS_VIEW];
	}
	// TODO remove D3DX usage
	// Pre-transform light position to viewspace
	D3DXVECTOR4 positionV;
	D3DXVec3Transform(&positionV, (D3DXVECTOR3*)&d3dLight->Position, &viewTransform);
	pShaderLight->PositionV = (D3DXVECTOR3)positionV;

	// Pre-transform light direction to viewspace and normalize
	D3DXVECTOR4 directionV;
	D3DXMATRIX viewTransform3x3;
	D3DXMatrixIdentity(&viewTransform3x3);
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			viewTransform3x3.m[x][y] = viewTransform.m[x][y];
		}
	}

	D3DXVec3Transform(&directionV, (D3DXVECTOR3*)&d3dLight->Direction, &viewTransform3x3);
	D3DXVec3Normalize((D3DXVECTOR3*)&pShaderLight->DirectionVN, (D3DXVECTOR3*)&directionV);

	bool SpecularEnable;
	if (is_pgraph_using_NV2A_Kelvin()) {
		SpecularEnable = NV2ARenderStates.GetXboxRenderState(xbox::X_D3DRS_SPECULARENABLE) != FALSE;
	}
	else {
		SpecularEnable = XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_SPECULARENABLE) != FALSE;
	}
	// Map D3D light to state struct
	pShaderLight->Type = (float)((int)d3dLight->Type);
	pShaderLight->Diffuse = toVector(d3dLight->Diffuse);
	pShaderLight->Specular = SpecularEnable ? toVector(d3dLight->Specular) : toVector(0);
	pShaderLight->Range = d3dLight->Range;
	pShaderLight->Falloff = d3dLight->Falloff;
	pShaderLight->Attenuation.x = d3dLight->Attenuation0;
	pShaderLight->Attenuation.y = d3dLight->Attenuation1;
	pShaderLight->Attenuation.z = d3dLight->Attenuation2;

	pLightAmbient->x += d3dLight->Ambient.r;
	pLightAmbient->y += d3dLight->Ambient.g;
	pLightAmbient->z += d3dLight->Ambient.b;

	auto cosHalfPhi = cos(d3dLight->Phi / 2);
	pShaderLight->CosHalfPhi = cosHalfPhi;
	pShaderLight->SpotIntensityDivisor = cos(d3dLight->Theta / 2) - cos(d3dLight->Phi / 2);
}
extern D3DMATRIX * pgraph_get_ModelViewMatrix(unsigned index);
extern D3DMATRIX * pgraph_get_InverseModelViewMatrix(unsigned index);
extern D3DMATRIX* pgraph_get_TextureTransformMatrix(unsigned index);
void UpdateFixedFunctionVertexShaderState()//(NV2ASTATE *d)
{
	extern xbox::X_VERTEXATTRIBUTEFORMAT* GetXboxVertexAttributeFormat(); // TMP glue
	using namespace xbox;
	NV2AState* dev = g_NV2A->GetDeviceState();
	PGRAPHState* pg = &(dev->pgraph);
	// Vertex blending
	// Prepare vertex blending mode variables used in transforms, below
	auto VertexBlend = XboxRenderStates.GetXboxRenderState(X_D3DRS_VERTEXBLEND);
	if (is_pgraph_using_NV2A_Kelvin())
		VertexBlend = NV2ARenderStates.GetXboxRenderState(X_D3DRS_VERTEXBLEND);
	// Xbox and host D3DVERTEXBLENDFLAGS :
	//     D3DVBF_DISABLE           = 0 : 1 matrix,   0 weights => final weight 1
	//     D3DVBF_1WEIGHTS          = 1 : 2 matrices, 1 weights => final weight calculated
	//     D3DVBF_2WEIGHTS          = 3 : 3 matrices, 2 weights => final weight calculated
	//     D3DVBF_3WEIGHTS          = 5 : 4 matrices, 3 weights => final weight calculated
	// Xbox X_D3DVERTEXBLENDFLAGS :
	//   X_D3DVBF_2WEIGHTS2MATRICES = 2 : 2 matrices, 2 weights
	//   X_D3DVBF_3WEIGHTS3MATRICES = 4 : 3 matrices, 3 weights
	//   X_D3DVBF_4WEIGHTS4MATRICES = 6 : 4 matrices, 4 weights
	//
	if (VertexBlend > xbox::X_D3DVBF_4WEIGHTS4MATRICES) LOG_TEST_CASE("X_D3DRS_VERTEXBLEND out of range");
	// Calculate the number of matrices, by adding the LSB to turn (0,1,3,5) and (0,2,4,6) into (0,2,4,6); Then divide by 2 to get (0,1,2,3), and add 1 to get 1, 2, 3 or 4 matrices :
	auto NrBlendMatrices = ((VertexBlend + (VertexBlend & 1)) / 2) + 1;
	// Looking at the above values, 0 or the LSB of VertexBlend signals that the final weight needs to be calculated from all previous weigths (deducting them all from an initial 1) :
	auto CalcLastBlendWeight = (VertexBlend == xbox::X_D3DVBF_DISABLE) || (VertexBlend & 1);
	// Copy the resulting values over to shader state :
	ffShaderState.Modes.VertexBlend_NrOfMatrices = (float)NrBlendMatrices;
	ffShaderState.Modes.VertexBlend_CalcLastWeight = (float)CalcLastBlendWeight;

	// Texture state: TextureTransformFlagsCount and TextureTransformFlagsProjected must be updated prior to TextureTransform
    // use NV2ATextureStates when we're in pgraph handling
	if (is_pgraph_using_NV2A_Kelvin()) {
		for (int i = 0; i < xbox::X_D3DTS_STAGECOUNT; i++) {
			auto transformFlags = NV2ATextureStates.Get(i, X_D3DTSS_TEXTURETRANSFORMFLAGS);
			ffShaderState.TextureStates[i].TextureTransformFlagsCount = (float)(transformFlags & ~D3DTTFF_PROJECTED);
			ffShaderState.TextureStates[i].TextureTransformFlagsProjected = (float)(transformFlags & D3DTTFF_PROJECTED);

			auto texCoordIndex = NV2ATextureStates.Get(i, X_D3DTSS_TEXCOORDINDEX);
			ffShaderState.TextureStates[i].TexCoordIndex = (float)(texCoordIndex & 0x7); // 8 coords
			ffShaderState.TextureStates[i].TexCoordIndexGen = (float)(texCoordIndex >> 16); // D3DTSS_TCI flags
		}
	}
	else {
		for (int i = 0; i < xbox::X_D3DTS_STAGECOUNT; i++) {
			auto transformFlags = XboxTextureStates.Get(i, X_D3DTSS_TEXTURETRANSFORMFLAGS);
			ffShaderState.TextureStates[i].TextureTransformFlagsCount = (float)(transformFlags & ~D3DTTFF_PROJECTED);
			ffShaderState.TextureStates[i].TextureTransformFlagsProjected = (float)(transformFlags & D3DTTFF_PROJECTED);

			auto texCoordIndex = XboxTextureStates.Get(i, X_D3DTSS_TEXCOORDINDEX);
			ffShaderState.TextureStates[i].TexCoordIndex = (float)(texCoordIndex & 0x7); // 8 coords
			ffShaderState.TextureStates[i].TexCoordIndexGen = (float)(texCoordIndex >> 16); // D3DTSS_TCI flags
		}
	}

	// Transforms
	// Transpose row major to column major for HLSL
	// use NV2ATextureStates when we're in pgraph handling
	if (is_pgraph_using_NV2A_Kelvin()) {
		//NV2A always uses modelView matrix, and the matrix in Kelvin was transposed already.
		extern D3DMATRIX g_NV2A_DirectModelView_Projection;
		extern D3DMATRIX g_NV2A_DirectModelView_View;
		// pgraph calculation of projection matrix is very close to the matrix set with D3DDevice_SetTransform()
		D3DXMatrixTranspose((D3DXMATRIX*)&ffShaderState.Transforms.Projection, (D3DXMATRIX*)&g_NV2A_DirectModelView_Projection);
		// todo:we can't calculate the View matrix via NV2A content. We can set it as unity matrix and set world martix the same as modelView matrix. but there is no world matrix in ffShaderState.Transforms.
		// test result showed using identity matrix or the view matrix set with D3DDevice_SetTransform() output the same rendering. testcsae: BumpEarth sample.
		// D3DXMatrixTranspose((D3DXMATRIX*)&ffShaderState.Transforms.View, (D3DXMATRIX*)&g_NV2A_DirectModelView_View);
		D3DXMatrixTranspose((D3DXMATRIX*)&ffShaderState.Transforms.View, (D3DXMATRIX*)&d3d8TransformState.Transforms[X_D3DTS_VIEW]);
//		float temp =  pg->vsh_constants[0][0];
		for (unsigned i = 0; i < ffShaderState.Modes.VertexBlend_NrOfMatrices; i++) {
			// FIXME! stick with g_xbox_transform_ModelView[0] and g_xbox_DirectModelView_InverseWorldViewTransposed[0] when we're not in skinning mode. 
			// when RenderState[X_D3DRS_VERTEXBLEND]==0, we're not in skinning mode, use modelview matrix 0 only. else use corresponded matrix
			unsigned int count = (NV2ARenderStates.GetXboxRenderState(X_D3DRS_VERTEXBLEND) == 0) ? 0 : i;
			// transposed xbox modelview transform == kelvin modelview transform
			ffShaderState.Transforms.WorldView[i] = *pgraph_get_ModelViewMatrix(count);
			// xbox InverseWorldView transform == Kelvin InverseModelView transform
			D3DXMatrixTranspose((D3DXMATRIX*)&ffShaderState.Transforms.WorldViewInverseTranspose[i] , (D3DXMATRIX*)pgraph_get_InverseModelViewMatrix(count));
		}
		for (unsigned i = 0; i < 4; i++) { // TODO : Would it help to limit this to just the active texture channels?
			//texture transform matrix in Kelvin was transposed already.
			//todo: the texture transform matrix in Kelvin was altered per different in/out counts and whether there are projected output or not. here we simply copy the hardware content to ffShaderState.Transforms.Texture
			// later we should check the fixed function vertex shader codes to see how it uses these matrix.
			// test case: BumpEarth sample
			// using pgraph_get_TextureTransformMatrix(i) without transpose or d3d8TransformState.Transforms[X_D3DTS_TEXTURE0 + i] with transpose seems to have the same result.
			// todo: PixelShader and ModifyPixelShader sample only works with xbox transform matrices. have to figure out how to use NV2A matrices.
			memcpy(&ffShaderState.Transforms.Texture[i], pgraph_get_TextureTransformMatrix(i), sizeof(float) * 16);
			//D3DXMatrixTranspose((D3DXMATRIX*)&ffShaderState.Transforms.Texture[i], (D3DXMATRIX*)&d3d8TransformState.Transforms[X_D3DTS_TEXTURE0 + i]);
		}
	}
	else {
		// check if we're in DirectModelView transform mode.
		if (is_pgraph_DirectModelView()) {
			D3DXMatrixTranspose((D3DXMATRIX*)&ffShaderState.Transforms.Projection, (D3DXMATRIX*)&g_xbox_DirectModelView_Projection);
			D3DXMatrixTranspose((D3DXMATRIX*)&ffShaderState.Transforms.View, (D3DXMATRIX*)&d3d8TransformState.Transforms[X_D3DTS_VIEW]);
			for (unsigned i = 0; i < ffShaderState.Modes.VertexBlend_NrOfMatrices; i++) {
				// FIXME! stick with g_xbox_transform_ModelView[0] and g_xbox_DirectModelView_InverseWorldViewTransposed[0] when we're not in skinning mode. 
				// when RenderState[X_D3DRS_VERTEXBLEND]==0, we're not in skinning mode, use modelview matrix 0 only. else use corresponded matrix
				unsigned int count = (XboxRenderStates.GetXboxRenderState(X_D3DRS_VERTEXBLEND) == 0) ? 0 : i;
				// was D3DXMatrixTranspose((D3DXMATRIX*)&ffShaderState.Transforms.WorldView[i], (D3DXMATRIX*)&g_xbox_transform_ModelView);
				// transposed xbox modelview transform == kelvin modelview transform
				ffShaderState.Transforms.WorldView[i] = *pgraph_get_ModelViewMatrix(count);
				// was D3DXMatrixTranspose((D3DXMATRIX*)&ffShaderState.Transforms.WorldViewInverseTranspose[i], (D3DXMATRIX*)&g_xbox_DirectModelView_InverseWorldViewTransposed);
				// xbox InverseWorldView transform == Kelvin InverseModelView transform
				D3DXMatrixTranspose((D3DXMATRIX*)&ffShaderState.Transforms.WorldViewInverseTranspose[i], (D3DXMATRIX*)pgraph_get_InverseModelViewMatrix(count));
			}

			for (unsigned i = 0; i < 4; i++) { // TODO : Would it help to limit this to just the active texture channels?
				//texture transform matrix in Kelvin was transposed already.
				//todo: the texture transform matrix in Kelvin was altered per different in/out counts and whether there are projected output or not. here we simply copy the hardware content to ffShaderState.Transforms.Texture
				// later we should check the fixed function vertex shader codes to see how it uses these matrix.
				memcpy(&ffShaderState.Transforms.Texture[i], pgraph_get_TextureTransformMatrix(i), sizeof(float) * 16);
				//D3DXMatrixTranspose((D3DXMATRIX*)&ffShaderState.Transforms.Texture[i], (D3DXMATRIX*)&d3d8TransformState.Transforms[X_D3DTS_TEXTURE0 + i]);
			}
		}
		else {
			D3DXMatrixTranspose((D3DXMATRIX*)&ffShaderState.Transforms.Projection, (D3DXMATRIX*)&d3d8TransformState.Transforms[X_D3DTS_PROJECTION]);
			D3DXMatrixTranspose((D3DXMATRIX*)&ffShaderState.Transforms.View, (D3DXMATRIX*)&d3d8TransformState.Transforms[X_D3DTS_VIEW]);

			for (unsigned i = 0; i < ffShaderState.Modes.VertexBlend_NrOfMatrices; i++) {
				D3DXMatrixTranspose((D3DXMATRIX*)&ffShaderState.Transforms.WorldView[i], (D3DXMATRIX*)d3d8TransformState.GetWorldView(i));
				D3DXMatrixTranspose((D3DXMATRIX*)&ffShaderState.Transforms.WorldViewInverseTranspose[i], (D3DXMATRIX*)d3d8TransformState.GetWorldViewInverseTranspose(i));
			}

			for (unsigned i = 0; i < 4; i++) { // TODO : Would it help to limit this to just the active texture channels?
				D3DXMatrixTranspose((D3DXMATRIX*)&ffShaderState.Transforms.Texture[i], (D3DXMATRIX*)&d3d8TransformState.Transforms[X_D3DTS_TEXTURE0 + i]);
			}
		}
	}

	// Lighting, pgraph CxbxrLazySetLights()
	// Point sprites aren't lit - 'each point is always rendered with constant colors.'
	// https://docs.microsoft.com/en-us/windows/win32/direct3d9/point-sprites
	bool PointSpriteEnable, LightingEnable, ColorVertex, PointScaleEnable, fogEnable,bRangeFogEnable;
	float pointSize, pointSize_Min, pointSize_Max, pointScale_A, pointScale_B, pointScale_C, renderTargetHeight;
	DWORD fogTableMode, density, fogEnd, fogStart;

	extern xbox::X_D3DMATERIAL8 NV2A_SceneMateirals[2];
	// use NV2ARenderStates when we're in pgraph handling
	if (is_pgraph_using_NV2A_Kelvin()) {
		PointSpriteEnable = NV2ARenderStates.GetXboxRenderState(X_D3DRS_POINTSPRITEENABLE);
		LightingEnable = NV2ARenderStates.GetXboxRenderState(X_D3DRS_LIGHTING);
		ffShaderState.Modes.Lighting = LightingEnable && !PointSpriteEnable;
		ffShaderState.Modes.TwoSidedLighting = (float)NV2ARenderStates.GetXboxRenderState(X_D3DRS_TWOSIDEDLIGHTING);
		ffShaderState.Modes.LocalViewer = (float)NV2ARenderStates.GetXboxRenderState(X_D3DRS_LOCALVIEWER);

		// Material sources, pgraph update these renderstates in CxbxrLazySetLights()
		//ColorVertex = NV2ARenderStates.GetXboxRenderState(X_D3DRS_COLORVERTEX) != FALSE;
		ColorVertex=pg->KelvinPrimitive.SetColorMaterial != 0;
		ffShaderState.Modes.AmbientMaterialSource = NV2ARenderStates.GetXboxRenderState(X_D3DRS_AMBIENTMATERIALSOURCE);//(float)(ColorVertex ? NV2ARenderStates.GetXboxRenderState(X_D3DRS_AMBIENTMATERIALSOURCE) : D3DMCS_MATERIAL);
		ffShaderState.Modes.DiffuseMaterialSource = NV2ARenderStates.GetXboxRenderState(X_D3DRS_DIFFUSEMATERIALSOURCE);//(float)(ColorVertex ? NV2ARenderStates.GetXboxRenderState(X_D3DRS_DIFFUSEMATERIALSOURCE) : D3DMCS_MATERIAL);
		ffShaderState.Modes.SpecularMaterialSource = NV2ARenderStates.GetXboxRenderState(X_D3DRS_SPECULARMATERIALSOURCE);//(float)(ColorVertex ? NV2ARenderStates.GetXboxRenderState(X_D3DRS_SPECULARMATERIALSOURCE) : D3DMCS_MATERIAL);
		ffShaderState.Modes.EmissiveMaterialSource = NV2ARenderStates.GetXboxRenderState(X_D3DRS_EMISSIVEMATERIALSOURCE);//(float)(ColorVertex ? NV2ARenderStates.GetXboxRenderState(X_D3DRS_EMISSIVEMATERIALSOURCE) : D3DMCS_MATERIAL);
		ffShaderState.Modes.BackAmbientMaterialSource = NV2ARenderStates.GetXboxRenderState(X_D3DRS_BACKAMBIENTMATERIALSOURCE);//(float)(ColorVertex ? NV2ARenderStates.GetXboxRenderState(X_D3DRS_BACKAMBIENTMATERIALSOURCE) : D3DMCS_MATERIAL);
		ffShaderState.Modes.BackDiffuseMaterialSource = NV2ARenderStates.GetXboxRenderState(X_D3DRS_BACKDIFFUSEMATERIALSOURCE);//(float)(ColorVertex ? NV2ARenderStates.GetXboxRenderState(X_D3DRS_BACKDIFFUSEMATERIALSOURCE) : D3DMCS_MATERIAL);
		ffShaderState.Modes.BackSpecularMaterialSource = NV2ARenderStates.GetXboxRenderState(X_D3DRS_BACKSPECULARMATERIALSOURCE);//(float)(ColorVertex ? NV2ARenderStates.GetXboxRenderState(X_D3DRS_BACKSPECULARMATERIALSOURCE) : D3DMCS_MATERIAL);
		ffShaderState.Modes.BackEmissiveMaterialSource = NV2ARenderStates.GetXboxRenderState(X_D3DRS_BACKEMISSIVEMATERIALSOURCE);//(float)(ColorVertex ? NV2ARenderStates.GetXboxRenderState(X_D3DRS_BACKEMISSIVEMATERIALSOURCE) : D3DMCS_MATERIAL);
		// Scene materials, HLE uses SetMaterial()/SetBackMaterial(), pgraph update materials in CxbxrLazySetLights()

		memcpy(&ffShaderState.Materials[0], &NV2A_SceneMateirals[0], sizeof(NV2A_SceneMateirals[0]));
		memcpy(&ffShaderState.Materials[1], &NV2A_SceneMateirals[1], sizeof(NV2A_SceneMateirals[1]));

		// Point sprites; Fetch required variables. pgraph CxbxrLazySetPointParams()
		pointSize = NV2ARenderStates.GetXboxRenderStateAsFloat(X_D3DRS_POINTSIZE);
		pointSize_Min = NV2ARenderStates.GetXboxRenderStateAsFloat(X_D3DRS_POINTSIZE_MIN);
		pointSize_Max = NV2ARenderStates.GetXboxRenderStateAsFloat(X_D3DRS_POINTSIZE_MAX);
		PointScaleEnable = NV2ARenderStates.GetXboxRenderState(X_D3DRS_POINTSCALEENABLE);
		pointScale_A = NV2ARenderStates.GetXboxRenderStateAsFloat(X_D3DRS_POINTSCALE_A);
		pointScale_B = NV2ARenderStates.GetXboxRenderStateAsFloat(X_D3DRS_POINTSCALE_B);
		pointScale_C = NV2ARenderStates.GetXboxRenderStateAsFloat(X_D3DRS_POINTSCALE_C);
		renderTargetHeight = (float)GetPixelContainerHeight(g_pXbox_RenderTarget);
		// Make sure to disable point scaling when point sprites are not enabled
		PointScaleEnable &= PointSpriteEnable;
		// Set variables in shader state
		ffShaderState.PointSprite.PointSize = PointSpriteEnable ? pointSize : 1.0f;
		ffShaderState.PointSprite.PointSize_Min = PointSpriteEnable ? pointSize_Min : 1.0f;
		ffShaderState.PointSprite.PointSize_Max = PointSpriteEnable ? pointSize_Max : 1.0f;
		ffShaderState.PointSprite.PointScaleABC.x = PointScaleEnable ? pointScale_A : 1.0f;
		ffShaderState.PointSprite.PointScaleABC.y = PointScaleEnable ? pointScale_B : 0.0f;
		ffShaderState.PointSprite.PointScaleABC.z = PointScaleEnable ? pointScale_C : 0.0f;
		ffShaderState.PointSprite.XboxRenderTargetHeight = PointScaleEnable ? renderTargetHeight : 1.0f;
		ffShaderState.PointSprite.RenderUpscaleFactor = g_RenderUpscaleFactor;

		// Fog
		// Determine how the fog depth is transformed into the fog factor
		fogEnable = NV2ARenderStates.GetXboxRenderState(X_D3DRS_FOGENABLE);
		fogTableMode = NV2ARenderStates.GetXboxRenderState(X_D3DRS_FOGTABLEMODE);
		bRangeFogEnable = NV2ARenderStates.GetXboxRenderState(X_D3DRS_RANGEFOGENABLE);
		density = NV2ARenderStates.GetXboxRenderState(X_D3DRS_FOGDENSITY);
		fogStart = NV2ARenderStates.GetXboxRenderState(X_D3DRS_FOGSTART);
		fogEnd = NV2ARenderStates.GetXboxRenderState(X_D3DRS_FOGEND);
	}else{
		PointSpriteEnable = XboxRenderStates.GetXboxRenderState(X_D3DRS_POINTSPRITEENABLE);
		LightingEnable = XboxRenderStates.GetXboxRenderState(X_D3DRS_LIGHTING);
		ffShaderState.Modes.Lighting = LightingEnable && !PointSpriteEnable;
		ffShaderState.Modes.TwoSidedLighting = (float)XboxRenderStates.GetXboxRenderState(X_D3DRS_TWOSIDEDLIGHTING);
		ffShaderState.Modes.LocalViewer = (float)XboxRenderStates.GetXboxRenderState(X_D3DRS_LOCALVIEWER);


		// Material sources, pgraph update these renderstates in CxbxrLazySetLights()
		ColorVertex = XboxRenderStates.GetXboxRenderState(X_D3DRS_COLORVERTEX) != FALSE;
		ffShaderState.Modes.AmbientMaterialSource = (float)(ColorVertex ? XboxRenderStates.GetXboxRenderState(X_D3DRS_AMBIENTMATERIALSOURCE) : D3DMCS_MATERIAL);
		ffShaderState.Modes.DiffuseMaterialSource = (float)(ColorVertex ? XboxRenderStates.GetXboxRenderState(X_D3DRS_DIFFUSEMATERIALSOURCE) : D3DMCS_MATERIAL);
		ffShaderState.Modes.SpecularMaterialSource = (float)(ColorVertex ? XboxRenderStates.GetXboxRenderState(X_D3DRS_SPECULARMATERIALSOURCE) : D3DMCS_MATERIAL);
		ffShaderState.Modes.EmissiveMaterialSource = (float)(ColorVertex ? XboxRenderStates.GetXboxRenderState(X_D3DRS_EMISSIVEMATERIALSOURCE) : D3DMCS_MATERIAL);
		ffShaderState.Modes.BackAmbientMaterialSource = (float)(ColorVertex ? XboxRenderStates.GetXboxRenderState(X_D3DRS_BACKAMBIENTMATERIALSOURCE) : D3DMCS_MATERIAL);
		ffShaderState.Modes.BackDiffuseMaterialSource = (float)(ColorVertex ? XboxRenderStates.GetXboxRenderState(X_D3DRS_BACKDIFFUSEMATERIALSOURCE) : D3DMCS_MATERIAL);
		ffShaderState.Modes.BackSpecularMaterialSource = (float)(ColorVertex ? XboxRenderStates.GetXboxRenderState(X_D3DRS_BACKSPECULARMATERIALSOURCE) : D3DMCS_MATERIAL);
		ffShaderState.Modes.BackEmissiveMaterialSource = (float)(ColorVertex ? XboxRenderStates.GetXboxRenderState(X_D3DRS_BACKEMISSIVEMATERIALSOURCE) : D3DMCS_MATERIAL);

		// Point sprites; Fetch required variables. pgraph CxbxrLazySetPointParams()
		pointSize = XboxRenderStates.GetXboxRenderStateAsFloat(X_D3DRS_POINTSIZE);
		pointSize_Min = XboxRenderStates.GetXboxRenderStateAsFloat(X_D3DRS_POINTSIZE_MIN);
		pointSize_Max = XboxRenderStates.GetXboxRenderStateAsFloat(X_D3DRS_POINTSIZE_MAX);
		PointScaleEnable = XboxRenderStates.GetXboxRenderState(X_D3DRS_POINTSCALEENABLE);
		pointScale_A = XboxRenderStates.GetXboxRenderStateAsFloat(X_D3DRS_POINTSCALE_A);
		pointScale_B = XboxRenderStates.GetXboxRenderStateAsFloat(X_D3DRS_POINTSCALE_B);
		pointScale_C = XboxRenderStates.GetXboxRenderStateAsFloat(X_D3DRS_POINTSCALE_C);
		renderTargetHeight = (float)GetPixelContainerHeight(g_pXbox_RenderTarget);
		// Make sure to disable point scaling when point sprites are not enabled
		PointScaleEnable &= PointSpriteEnable;
		// Set variables in shader state
		ffShaderState.PointSprite.PointSize = PointSpriteEnable ? pointSize : 1.0f;
		ffShaderState.PointSprite.PointSize_Min = PointSpriteEnable ? pointSize_Min : 1.0f;
		ffShaderState.PointSprite.PointSize_Max = PointSpriteEnable ? pointSize_Max : 1.0f;
		ffShaderState.PointSprite.PointScaleABC.x = PointScaleEnable ? pointScale_A : 1.0f;
		ffShaderState.PointSprite.PointScaleABC.y = PointScaleEnable ? pointScale_B : 0.0f;
		ffShaderState.PointSprite.PointScaleABC.z = PointScaleEnable ? pointScale_C : 0.0f;
		ffShaderState.PointSprite.XboxRenderTargetHeight = PointScaleEnable ? renderTargetHeight : 1.0f;
		ffShaderState.PointSprite.RenderUpscaleFactor = g_RenderUpscaleFactor;

		// Fog
		// Determine how the fog depth is transformed into the fog factor
		fogEnable = XboxRenderStates.GetXboxRenderState(X_D3DRS_FOGENABLE);
		fogTableMode = XboxRenderStates.GetXboxRenderState(X_D3DRS_FOGTABLEMODE);
		bRangeFogEnable = XboxRenderStates.GetXboxRenderState(X_D3DRS_RANGEFOGENABLE);
		density = XboxRenderStates.GetXboxRenderState(X_D3DRS_FOGDENSITY);
		fogStart = XboxRenderStates.GetXboxRenderState(X_D3DRS_FOGSTART);
		fogEnd = XboxRenderStates.GetXboxRenderState(X_D3DRS_FOGEND);
	}
	ffShaderState.Fog.Enable = fogEnable;
	// FIXME remove when fixed function PS is implemented
	// Note if we are using the fixed function pixel shader
	// We only want to produce the fog depth value in the VS, not the fog factor
	ffShaderState.Fog.TableMode = !g_UseFixedFunctionPixelShader ? D3DFOG_NONE : fogTableMode;

	// Determine how fog depth is calculated
	if (fogEnable && fogTableMode != D3DFOG_NONE) {
		auto proj = &ffShaderState.Transforms.Projection;

		if (bRangeFogEnable) {
			LOG_TEST_CASE("Using RANGE fog");
			ffShaderState.Fog.DepthMode = FixedFunctionVertexShader::FOG_DEPTH_RANGE;
		}
		else if (proj->_14 == 0 &&
			proj->_24 == 0 &&
			proj->_34 == 0 &&
			proj->_44 == 1) {
			LOG_TEST_CASE("Using Z fog");
			ffShaderState.Fog.DepthMode = FixedFunctionVertexShader::FOG_DEPTH_Z;
		}
		else {
			// Test case:
			// Fog sample
			// JSRF (non-compliant projection matrix)
			ffShaderState.Fog.DepthMode = FixedFunctionVertexShader::FOG_DEPTH_W;
		}
		
		ffShaderState.Fog.Density = *reinterpret_cast<float*>(&density);
		ffShaderState.Fog.Start = *reinterpret_cast<float*>(&fogStart);
		ffShaderState.Fog.End = *reinterpret_cast<float*>(&fogEnd);
	}
	else {
		ffShaderState.Fog.DepthMode = FixedFunctionVertexShader::FOG_DEPTH_NONE;
	}


	// Read current TexCoord component counts
	xbox::X_VERTEXATTRIBUTEFORMAT* pXboxVertexAttributeFormat = GetXboxVertexAttributeFormat();
	// Note : There seem to be other ways to access this, but we can use only this one;
	// This, because CxbxGetVertexDeclaration() can't be used, since it doesn't track VertexAttributes
	// (plus, it contains the overhead of shader lookup).
	// Another, GetXboxVertexShader(), can't be used, because it doesn't honor vertex attribute overrides
	// like those that apply for g_InlineVertexBuffer_DeclarationOverride and active SetVertexShaderInput.
	// Also, the xbox::X_D3DVertexShader.Dimensionality[] field contains somewhat strange values.
	for (int i = 0; i < xbox::X_D3DTS_STAGECOUNT; i++) {
		auto vertexDataFormat = pXboxVertexAttributeFormat->Slots[xbox::X_D3DVSDE_TEXCOORD0 + i].Format;
		ffShaderState.TexCoordComponentCount[i] = (float)GetXboxVertexDataComponentCount(vertexDataFormat);
	}

	// Update lights, pgraph CxbxrLazySetLights()
	auto LightAmbient = D3DXVECTOR4(0.f, 0.f, 0.f, 0.f);
	// we use lights composed from kelvin SetLight[] directly when we're handling pushbuffer.
	extern DWORD CxbxrNV2ALight8EnableMask(int lightNum);
	extern D3DCOLORVALUE NV2A_SceneAmbient[2];
	D3DXVECTOR4 Ambient, BackAmbient;
	if (is_pgraph_using_NV2A_Kelvin()) {
		//todo: check if accumulate active light ambient and add it to scene ambient is necessary or not.
		for (size_t i = 0; i < 8; i++) {
			if (CxbxrNV2ALight8EnableMask(i) != 0) {
				//accumulate LightAmbient with each active light
				UpdateFixedFunctionShaderLight(i, &ffShaderState.Lights[i], &LightAmbient);
			}
			else {
				//this simply set the light.type to 0 in order to disable it in shader.
				UpdateFixedFunctionShaderLight(-1, &ffShaderState.Lights[i], &LightAmbient);
			}
		}
        // X_D3DRS_AMBIENT and X_D3DRS_BACKAMBIENT renderstates are updated by pgraph in CxbxrLazySetLights() already. but here we directly use the D3DCOLORVAL NV2A_SceneAmbient[] preventing conversion loss.
		Ambient = D3DXVECTOR4(NV2A_SceneAmbient[0].r, NV2A_SceneAmbient[0].g, NV2A_SceneAmbient[0].b, NV2A_SceneAmbient[0].a);//toVector(NV2ARenderStates.GetXboxRenderState(X_D3DRS_AMBIENT));
		BackAmbient = D3DXVECTOR4(NV2A_SceneAmbient[1].r, NV2A_SceneAmbient[1].g, NV2A_SceneAmbient[1].b, NV2A_SceneAmbient[1].a);//toVector(NV2ARenderStates.GetXboxRenderState(X_D3DRS_BACKAMBIENT));
		// Misc flags
		ffShaderState.Modes.NormalizeNormals = (float)NV2ARenderStates.GetXboxRenderState(X_D3DRS_NORMALIZENORMALS);

	}
	else {
		for (size_t i = 0; i < ffShaderState.Lights.size(); i++) {
			UpdateFixedFunctionShaderLight(d3d8LightState.EnabledLights[i], &ffShaderState.Lights[i], &LightAmbient);
		}
		Ambient = toVector(XboxRenderStates.GetXboxRenderState(X_D3DRS_AMBIENT));
		BackAmbient = toVector(XboxRenderStates.GetXboxRenderState(X_D3DRS_BACKAMBIENT));
		// Misc flags
		ffShaderState.Modes.NormalizeNormals = (float)XboxRenderStates.GetXboxRenderState(X_D3DRS_NORMALIZENORMALS);

	}


	ffShaderState.TotalLightsAmbient.Front = (D3DXVECTOR3)(LightAmbient + Ambient);
	ffShaderState.TotalLightsAmbient.Back = (D3DXVECTOR3)(LightAmbient + BackAmbient);

	
	// Write fixed function state to shader constants
	const int slotSize = 16;
	const int fixedFunctionStateSize = (sizeof(FixedFunctionVertexShaderState) + slotSize - 1) / slotSize;
	auto hRet = g_pD3DDevice->SetVertexShaderConstantF(0, (float*)&ffShaderState, fixedFunctionStateSize);

	if (FAILED(hRet)) {
		CxbxrAbort("Failed to write fixed-function HLSL state");
	}
}

// ******************************************************************
// * patch: D3DDevice_EnableOverlay
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_EnableOverlay)
(
    bool_xt Enable
)
{
	LOG_FUNC_ONE_ARG(Enable);
	
	// The Xbox D3DDevice_EnableOverlay call merely resets the active
	// NV2A overlay state, it doesn't actually enable or disable anything.
	// Thus, we should just reset our overlay state here too. A title will
	// show new overlay data via D3DDevice_UpdateOverlay (see below).
	g_OverlayProxy = {};
}

// ******************************************************************
// * patch: D3DDevice_UpdateOverlay
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_UpdateOverlay)
(
	X_D3DSurface *pSurface,
	CONST RECT   *SrcRect,
	CONST RECT   *DstRect,
	bool_xt          EnableColorKey,
	D3DCOLOR      ColorKey
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pSurface)
		LOG_FUNC_ARG(SrcRect)
		LOG_FUNC_ARG(DstRect)
		LOG_FUNC_ARG(EnableColorKey)
		LOG_FUNC_ARG(ColorKey)
		LOG_FUNC_END;

	// Reset and remember the overlay arguments, so our D3DDevice_Swap patch
	// can correctly show this overlay surface data.
	g_OverlayProxy = {};
	if (pSurface) {
		g_OverlayProxy.Surface = *pSurface;
		if (SrcRect)
			g_OverlayProxy.SrcRect = *SrcRect;

		if (DstRect)
			g_OverlayProxy.DstRect = *DstRect;

		g_OverlayProxy.EnableColorKey = EnableColorKey;
		g_OverlayProxy.ColorKey = ColorKey;
		// Update overlay if present was not called since the last call to
		// EmuD3DDevice_UpdateOverlay.
		if (g_OverlaySwap != g_Xbox_SwapData.Swap - 1) {
			EMUPATCH(D3DDevice_Swap)(CXBX_SWAP_PRESENT_FORWARD);
		}

		g_OverlaySwap = g_Xbox_SwapData.Swap;
	}
}

// ******************************************************************
// * patch: D3DDevice_GetOverlayUpdateStatus
// ******************************************************************
xbox::bool_xt WINAPI xbox::EMUPATCH(D3DDevice_GetOverlayUpdateStatus)()
{
	LOG_FUNC();    

	LOG_UNIMPLEMENTED();

    // TODO: Actually check for update status
    return TRUE;
}
void CxbxrImpl_BlockUntilVerticalBlank()
{
	std::unique_lock<std::mutex> lk(g_VBConditionMutex);
	g_VBConditionVariable.wait(lk);
}
// ******************************************************************
// * patch: D3DDevice_BlockUntilVerticalBlank
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_BlockUntilVerticalBlank)()
{
	LOG_FUNC();

	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
#if USEPGRAPH_BlockUntilVerticalBlank
	//give the correct token enum here, and it's done.
	//Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_BlockUntilVerticalBlank, 0, PBTokenArray);//argCount 14
#else
	CxbxrImpl_BlockUntilVerticalBlank();
#endif
}

// ******************************************************************
// * patch: D3DDevice_SetVerticalBlankCallback
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetVerticalBlankCallback)
(
    X_D3DVBLANKCALLBACK pCallback
)
{
	LOG_FUNC_ONE_ARG(pCallback);

    g_pXbox_VerticalBlankCallback = pCallback;    
}

void WINAPI CxbxrImpl_SetRenderState_Simple
(
	xbox::dword_xt Method,
	xbox::dword_xt Value
	)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Method)
		LOG_FUNC_ARG(Value)
		LOG_FUNC_END;

	// Fetch the RenderState conversion info for the given input
	int XboxRenderStateIndex = -1;
	for (int i = xbox::X_D3DRS_FIRST; i <= xbox::X_D3DRS_LAST; i++) {
		if (GetDxbxRenderStateInfo(i).M == PUSH_METHOD(Method)) {
			XboxRenderStateIndex = i;
			break;
		}
	}

	// If we could not map it, log and return
	if (XboxRenderStateIndex == -1) {
		EmuLog(LOG_LEVEL::WARNING, "RenderState_Simple(0x%.08X (%s), 0x%.08X) could not be found in RenderState table", Method, GetDxbxRenderStateInfo(XboxRenderStateIndex).S, Value);
		return;
	}

	EmuLog(LOG_LEVEL::DEBUG, "RenderState_Simple: %s = 0x%08X", GetDxbxRenderStateInfo(XboxRenderStateIndex).S, Value);

    XboxRenderStates.SetXboxRenderState(XboxRenderStateIndex, Value);
}

// ******************************************************************
// * patch: D3DDevice_SetRenderState_Simple
// ******************************************************************
xbox::void_xt __fastcall xbox::EMUPATCH(D3DDevice_SetRenderState_Simple)
(
    dword_xt Method,
    dword_xt Value
)
{

    XB_TRMP(D3DDevice_SetRenderState_Simple)(Method, Value);

	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)Method;
	PBTokenArray[3] = (DWORD)Value;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetRenderState_Simple, 2);//argCount, not necessary, default to 14
}

void CxbxrImpl_GetProjectionViewportMatrix(D3DMATRIX *pProjectionViewportTransform)
{
	// get the transform matrix pointer in xbox d3d and store it in g_xbox_transform_matrix if it's not set yet
	if (g_xbox_ProjectionViewportTransform == nullptr) {
		byte * pProjectionViewportTransform = nullptr;
		auto it = g_SymbolAddresses.find("D3D_UpdateProjectionViewportTransform");
		if (it != g_SymbolAddresses.end()) {
			pProjectionViewportTransform = (byte *)it->second;
			if ((*(byte*)(pProjectionViewportTransform + 0x126)) == 0x8D) {
				g_xbox_ProjectionViewportTransform = (D3DMATRIX *)((*(DWORD*)(pProjectionViewportTransform + 0x128)) + *g_pXbox_D3DDevice);
			}
			else {
				g_xbox_ProjectionViewportTransform = (D3DMATRIX *)((*(DWORD*)(pProjectionViewportTransform + 0x127)) + *g_pXbox_D3DDevice);
			}
		}
	}
	// 
	if (g_xbox_ProjectionViewportTransform != nullptr) {
		// store the ProjectionViewportTransform matrix
		*pProjectionViewportTransform = *g_xbox_ProjectionViewportTransform;
	}


}

void CxbxrImpl_GetTransform
(
	xbox::X_D3DTRANSFORMSTATETYPE State,
	D3DMATRIX *pMatrix
)
{
	// get the transform matrix
	if (g_xbox_transform_matrix != nullptr && pMatrix!=nullptr) {
		*pMatrix = *(g_xbox_transform_matrix + State);
	}
}
// sets xbox d3d transform matrix directly without applying any d3d transforms.
void CxbxrImpl_SetTransformFast
(
	xbox::X_D3DTRANSFORMSTATETYPE State,
	CONST D3DMATRIX *pMatrix
)
{
	// set xbox d3d transform matrix
	if (g_xbox_transform_matrix != nullptr && pMatrix != nullptr) {
		// store the transform matrix
		*(g_xbox_transform_matrix + State) = *pMatrix;
	}
	else {
		CxbxrAbort("g_xbox_transform_matrix is nullptr in CxbxrImpl_SetTransformFast!");
	}
}

void CxbxrImpl_SetTransform
(
    xbox::X_D3DTRANSFORMSTATETYPE State,
    CONST D3DMATRIX *pMatrix
)
{
    LOG_INIT
	//update xbox d3d transform matrix array fast.
	// no need to update xbox transform here since we already xtrmp to xbox's SetTransform().
	//CxbxrImpl_SetTransformFast(State, pMatrix);

	d3d8TransformState.SetTransform(State, pMatrix);

	auto d3d9State = EmuXB2PC_D3DTS(State);

    HRESULT hRet = g_pD3DDevice->SetTransform(d3d9State, pMatrix);
    DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetTransform");    
}

// MultiplyTransform should call SetTransform, we'd like to know if it didn't
// Test case: 25 to Life
static thread_local uint32_t setTransformCount = 0;

// LTCG specific D3DDevice_SetTransform function...
// This uses a custom calling convention where parameter is passed in EAX, EDX

// Naked functions must not contain objects that would require unwinding
// so we cheat a bit by stashing the function body in a separate function
static void D3DDevice_SetTransform_0__LTCG_eax1_edx2
(
	xbox::X_D3DTRANSFORMSTATETYPE State,
    CONST D3DMATRIX *pMatrix
)
{
    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(State)
        LOG_FUNC_ARG(pMatrix)
        LOG_FUNC_END;

    setTransformCount++;
	if (g_pXbox_pPush == nullptr) {
		CxbxrImpl_SetTransform(State, pMatrix);
	}
	else {

		// use special template for allocating 0x20 DWORDS, ie. 128 bytes in pushbuffer because we're storing the whole transform matrix into the pushbuffer and point pMatrix to it.
		// init pushbuffer related pointers
		DWORD* pPush_local = (DWORD*)*g_pXbox_pPush;         //pointer to current pushbuffer
		DWORD* pPush_limit = (DWORD*)*g_pXbox_pPushLimit;    //pointer to the end of current pushbuffer
		if ((unsigned int)pPush_local + 128 >= (unsigned int)pPush_limit)//check if we still have enough space
			pPush_local = (DWORD*)CxbxrImpl_MakeSpace(); //make new pushbuffer space and get the pointer to it.
		// using special pgraph method to allocate 0x1F method count of DWORDs, ie. 0x7C bytes not including the command dword itself.
		pPush_local[0] = HLE_API_PUSHBFFER_COMMAND_128;
		// process xbox D3D API enum and arguments and push them to pushbuffer for pgraph to handle later.
		pPush_local[1] = (DWORD)X_D3DAPI_ENUM::X_D3DDevice_SetTransform_0__LTCG_eax1_edx2;
		// always copy the whole buffer
		pPush_local[2] = (DWORD)State;
		//point the pMatrix for CxbxrImpl_SetTransform() to the matrix in pPush_local[4]
		pPush_local[3] = (DWORD)&pPush_local[4];
		// store the transform matrix in pushbuffer so we can leave it along.
		// if we pass the pMatrix from the input argument directly, when pushbuffer reaches the X_D3DDevice_SetTransform handler the content of pMatrix could be modified by code in xbox side already.
		*(D3DMATRIX*)&pPush_local[4] = *pMatrix;

		//set pushbuffer pointer to the new beginning
		// always reserve 1 command DWORD, 1 API enum, and 14 argmenet DWORDs. here we add 16 more DWORDs because a matrix takes 16 DWORDs.
		*(DWORD**)g_pXbox_pPush += 0x20;
	}
}

__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetTransform_0__LTCG_eax1_edx2)
(
)
{
	xbox::X_D3DTRANSFORMSTATETYPE State;
    CONST D3DMATRIX *pMatrix;
    __asm {
        LTCG_PROLOGUE
        mov  State, eax
        mov  pMatrix, edx
		call XB_TRMP(D3DDevice_SetTransform_0__LTCG_eax1_edx2)
    }

	D3DDevice_SetTransform_0__LTCG_eax1_edx2(State, pMatrix);

    __asm {
        LTCG_EPILOGUE
        ret
    }
}

// ******************************************************************
// * patch: D3DDevice_SetTransform
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetTransform)
(
	xbox::X_D3DTRANSFORMSTATETYPE State,
    CONST D3DMATRIX      *pMatrix
)
{
    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(State)
        LOG_FUNC_ARG(pMatrix)
        LOG_FUNC_END;

    setTransformCount++;

    // Trampoline to guest code to remove the need for a GetTransform patch
    XB_TRMP(D3DDevice_SetTransform)(State, pMatrix);
	
	if (g_pXbox_pPush == nullptr) {
		CxbxrImpl_SetTransform(State, pMatrix);
	}
	else {
		// use special template for allocating 0x20 DWORDS, ie. 128 bytes in pushbuffer because we're storing the whole transform matrix into the pushbuffer and point pMatrix to it.
		// init pushbuffer related pointers
		DWORD* pPush_local = (DWORD*)*g_pXbox_pPush;         //pointer to current pushbuffer
		DWORD* pPush_limit = (DWORD*)*g_pXbox_pPushLimit;    //pointer to the end of current pushbuffer
		if ((unsigned int)pPush_local + 128 >= (unsigned int)pPush_limit)//check if we still have enough space
			pPush_local = (DWORD*)CxbxrImpl_MakeSpace(); //make new pushbuffer space and get the pointer to it.
		// using special pgraph method to allocate 0x1F method count of DWORDs, ie. 0x7C bytes not including the command dword itself.
		pPush_local[0] = HLE_API_PUSHBFFER_COMMAND_128;
		// process xbox D3D API enum and arguments and push them to pushbuffer for pgraph to handle later.
		pPush_local[1] = (DWORD)X_D3DAPI_ENUM::X_D3DDevice_SetTransform;
		// always copy the whole buffer
		pPush_local[2] = (DWORD)State;
		//point the pMatrix for CxbxrImpl_SetTransform() to the matrix in pPush_local[4]
		pPush_local[3] = (DWORD)&pPush_local[4];
		// store the transform matrix in pushbuffer so we can leave it along.
		// if we pass the pMatrix from the input argument directly, when pushbuffer reaches the X_D3DDevice_SetTransform handler the content of pMatrix could be modified by code in xbox side already.
		*(D3DMATRIX*)&pPush_local[4] = *pMatrix;

		//set pushbuffer pointer to the new beginning
		// always reserve 1 command DWORD, 1 API enum, and 14 argmenet DWORDs. here we add 16 more DWORDs because a matrix takes 16 DWORDs.
		*(DWORD**)g_pXbox_pPush += 0x20;
	}
}

// ******************************************************************
// * patch: D3DDevice_MultiplyTransform
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_MultiplyTransform)
(
    xbox::X_D3DTRANSFORMSTATETYPE State,
    CONST D3DMATRIX      *pMatrix
)
{
    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(State)
        LOG_FUNC_ARG(pMatrix)
        LOG_FUNC_END;

	setTransformCount = 0;

    // Trampoline to guest code, which we expect to call SetTransform
	// If we find a case where the trampoline doesn't call SetTransform
	// (or we can't detect the call) we will need to implement this
    XB_TRMP(D3DDevice_MultiplyTransform)(State, pMatrix);

	if (setTransformCount == 0) {
		LOG_TEST_CASE("MultiplyTransform did not appear to call SetTransform");
	}

}

/*
GetMasks2

  Purpose: produces the coordinate masks for manuvering through swizzled textures

  Parameters:
	IN Width: the width of the texture to be swizzled
	IN Height: the height of the texture to be swizzled
	OUT pMaskU: the mask for the u-coodinate
	OUT pMaskV: the mask for the v-coordinate

  Notes: This is a much faster algorithm for getting the masks than the
	more generic algorithm used in the Swizzler class defined in xgraphics.h.
	This algorithm works only for 2d textures. Swizzler's works for 2d and 3d.


Quick-Swizzling algorithm:

  The 2d swizzling/unswizzling code grabs a rectangular block of optimized size from the texture,
  rearranges that, moves it to the destination, then repeats with the next block.

  swizzling,                  8-bit or 16-bit
  linear:                     swizzled:
  00 01 02 03 04 05 06 07
  10 11 12 13 14 15 16 17
  20 21 22 23 24 25 26 27     00 01 10 11 02 03 12 13 20 21 30 31 22 23 32 33 \
  30 31 32 33 34 35 36 37 =>  04 05 14 15 06 07 16 17 24 25 34 35 26 27 36 37 \
  40 41 42 43 44 45 46 47     40 41 50 51 42 43 52 53 60 61 70 71 62 63 72 73 \
  50 51 52 53 54 55 56 57     44 45 54 55 46 47 56 57 64 65 74 75 66 67 76 77
  60 61 62 63 64 65 66 67
  70 71 72 73 74 75 76 77

  swizzling,                  32-bit
  linear:                     swizzled:
  00 01 02 03 04 05 06 07
  10 11 12 13 14 15 16 17 =>  00 01 10 11 02 03 12 13 20 21 30 31 22 23 32 33 \
  20 21 22 23 24 25 26 27     04 05 14 15 06 07 16 17 24 25 34 35 26 27 36 37
  30 31 32 33 34 35 36 37

  unswizzling, 8-bit or 16-bit
  swizzled:                                             linear:
  00 01 10 11 02 03 12 13 20 21 30 31 22 21 32 31 \     00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f
  04 05 14 15 06 07 16 17 24 25 34 35 26 27 36 37... => 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f
  08 09 18 19 0a 0b 1a 1b 28 29 38 39 2a 2b 3a 3b \     20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f
  0c 0d 1c 1d 0e 0f 1e 1f 2c 2d 3c 3d 2e 2f 3e 3f       30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f

  unswizzling, 32-bit
  swizzled:	                                            linear:
														00 01 02 03 04 05 06 07
  00 01 10 11 02 03 12 13 20 21 30 31 22 23 32 33 \  => 10 11 12 13 14 15 16 17
  04 05 14 15 06 07 16 17 24 25 34 35 26 27 36 37       20 21 22 23 24 25 26 27
														30 31 32 33 34 35 36 37

  The algorithm moves through the linear texture left->right, top->bottom,
  which means the swizzled array gets 64 or 128-byte blocks written to it in a
  seemingly random order.

  AddValU and AddValV are set to move to the next block. (-32), (-64), (-128),
  and (-256) were precalculated using the following algorithm (using
  Swizzler class methods):

	After finishing with one block, the U coordinate must be incrimented to
	the next block. Since all blocks are 4 texels wide, the value to add will
	be constant. Swizzing (4) for use with the U coordinate results in 16.
	(This assumes that the texture is at least 4 texels wide and 2 texels tall).
	Then, 16 is plugged into the AddU formula from the Swizzler class:

	  m_u = (m_u - ((-num) & m_MaskU)) & m_MaskU

	AddValU is set equal to ((-num) & m_MaskU). AddValV is calculated in a
	similar manner.

  This algorithm only works with textures with width and height >= block_size.
  This means that special-cases must occur when the is smaller. XBSwizzleRect
  and XBUnswizzleRect take care of this. Textures with width < 4, or height < 2
  actually look exactly the same in memory when they are swizzled as when they
  are deswizzled. memcpy() is used in that condition. For sizes between (2,4) and
  (blocksize_x, blocksize_y), the texture is swizzled in 2x1 blocks.
*/


typedef struct _XGPOINT3D
{
	DWORD u;
	DWORD v;
	DWORD w;
}
XGPOINT3D;

typedef DWORD SWIZNUM;

class Swizzler
{
public:

	// Dimensions of the texture
	DWORD m_Width;
	DWORD m_Height;
	DWORD m_Depth;

	// Internal mask for each coordinate
	DWORD m_MaskU;
	DWORD m_MaskV;
	DWORD m_MaskW;

	// Swizzled texture coordinates
	DWORD m_u;
	DWORD m_v;
	DWORD m_w;

	Swizzler() : m_Width(0), m_Height(0), m_Depth(0),
		m_MaskU(0), m_MaskV(0), m_MaskW(0),
		m_u(0), m_v(0), m_w(0)
	{ }

	// Initializes the swizzler
	Swizzler(
		DWORD width,
		DWORD height,
		DWORD depth
	)
	{
		Init(width, height, depth);
	}

	void Init(
		DWORD width,
		DWORD height,
		DWORD depth
	)
	{
		m_Width = width;
		m_Height = height;
		m_Depth = depth;
		m_MaskU = 0;
		m_MaskV = 0;
		m_MaskW = 0;
		m_u = 0;
		m_v = 0;
		m_w = 0;

		DWORD i = 1;
		DWORD j = 1;
		DWORD k;

		do
		{
			k = 0;
			if (i < width)
			{
				m_MaskU |= j;
				k = (j <<= 1);
			}

			if (i < height)
			{
				m_MaskV |= j;
				k = (j <<= 1);
			}

			if (i < depth)
			{
				m_MaskW |= j;
				k = (j <<= 1);
			}

			i <<= 1;
		} while (k);
	}

	// Swizzles a texture coordinate
	SWIZNUM SwizzleU(
		DWORD num
	)
	{
		SWIZNUM r = 0;

		for (DWORD i = 1; i <= m_MaskU; i <<= 1)
		{
			if (m_MaskU & i)
			{
				r |= (num & i);
			}
			else
			{
				num <<= 1;
			}
		}

		return r;
	}

	SWIZNUM SwizzleV(
		DWORD num
	)
	{
		SWIZNUM r = 0;

		for (DWORD i = 1; i <= m_MaskV; i <<= 1)
		{
			if (m_MaskV & i)
			{
				r |= (num & i);
			}
			else
			{
				num <<= 1;
			}
		}

		return r;
	}

	SWIZNUM SwizzleW(
		DWORD num
	)
	{
		SWIZNUM r = 0;

		for (DWORD i = 1; i <= m_MaskW; i <<= 1)
		{
			if (m_MaskW & i)
			{
				r |= (num & i);
			}
			else
			{
				num <<= 1;
			}
		}

		return r;
	}

	SWIZNUM Swizzle(
		DWORD u,
		DWORD v,
		DWORD w
	)
	{
		return SwizzleU(u) | SwizzleV(v) | SwizzleW(w);
	}

	// Unswizzles a texture coordinate
	DWORD UnswizzleU(
		SWIZNUM num
	)
	{
		DWORD r = 0;

		for (DWORD i = 1, j = 1; i; i <<= 1)
		{
			if (m_MaskU & i)
			{
				r |= (num & j);
				j <<= 1;
			}
			else
			{
				num >>= 1;
			}
		}

		return r;
	}

	DWORD UnswizzleV(
		SWIZNUM num
	)
	{
		DWORD r = 0;

		for (DWORD i = 1, j = 1; i; i <<= 1)
		{
			if (m_MaskV & i)
			{
				r |= (num & j);
				j <<= 1;
			}
			else
			{
				num >>= 1;
			}
		}

		return r;
	}

	DWORD UnswizzleW(
		SWIZNUM num
	)
	{
		DWORD r = 0;

		for (DWORD i = 1, j = 1; i; i <<= 1)
		{
			if (m_MaskW & i)
			{
				r |= (num & j);
				j <<= 1;
			}
			else
			{
				num >>= 1;
			}
		}

		return r;
	}

	// Sets a texture coordinate
	__forceinline SWIZNUM SetU(SWIZNUM num) { return m_u = num /* & m_MaskU */; }
	__forceinline SWIZNUM SetV(SWIZNUM num) { return m_v = num /* & m_MaskV */; }
	__forceinline SWIZNUM SetW(SWIZNUM num) { return m_w = num /* & m_MaskW */; }

	// Adds a value to a texture coordinate
	__forceinline SWIZNUM AddU(SWIZNUM num) { return m_u = (m_u - ((0 - num) & m_MaskU)) & m_MaskU; }
	__forceinline SWIZNUM AddV(SWIZNUM num) { return m_v = (m_v - ((0 - num) & m_MaskV)) & m_MaskV; }
	__forceinline SWIZNUM AddW(SWIZNUM num) { return m_w = (m_w - ((0 - num) & m_MaskW)) & m_MaskW; }

	// Subtracts a value from a texture coordinate
	__forceinline SWIZNUM SubU(SWIZNUM num) { return m_u = (m_u - num /* & m_MaskU */) & m_MaskU; }
	__forceinline SWIZNUM SubV(SWIZNUM num) { return m_v = (m_v - num /* & m_MaskV */) & m_MaskV; }
	__forceinline SWIZNUM SubW(SWIZNUM num) { return m_w = (m_w - num /* & m_MaskW */) & m_MaskW; }

	// Increments a texture coordinate
	__forceinline SWIZNUM IncU() { return m_u = (m_u - m_MaskU) & m_MaskU; }
	__forceinline SWIZNUM IncV() { return m_v = (m_v - m_MaskV) & m_MaskV; }
	__forceinline SWIZNUM IncW() { return m_w = (m_w - m_MaskW) & m_MaskW; }

	// Decrements a texture coordinate
	__forceinline SWIZNUM DecU() { return m_u = (m_u - 1) & m_MaskU; }
	__forceinline SWIZNUM DecV() { return m_v = (m_v - 1) & m_MaskV; }
	__forceinline SWIZNUM DecW() { return m_w = (m_w - 1) & m_MaskW; }

	// Gets the current swizzled address for a 2D or 3D texture
	__forceinline SWIZNUM Get2D() { return m_u | m_v; }
	__forceinline SWIZNUM Get3D() { return m_u | m_v | m_w; }
};

//----------------------------------------------------------------------------
// Swizzle a box from a buffer into a larger texture.  The destination box 
// must be completely contained within the destination texture (no clipping).
//
extern "C"
VOID WINAPI EmuSwizzleBox(
	LPCVOID     pSource,      // The buffer that contains the source rectangle
	DWORD       RowPitch,     // Byte offset from the left edge of one row to
	                          // the left edge of the next row
	DWORD       SlicePitch,   // Byte offset from the top-left of one slice to
	                          // the top-left of the next deepest slice
	CONST D3DBOX * pBox,      // The box within the buffer to copy.
	LPVOID      pDest,        // The destination texture.
	DWORD       Width,        // The width of the entire destination texture.
	DWORD       Height,       // The height of the entire destination texture.
	DWORD       Depth,        // The depth of the entire destination texture.
	CONST XGPOINT3D * pPoint, // Where to put the rectangle in the texture.
	DWORD       BytesPerPixel
)
{
	//PERF: swizzling/unswizzling when Width==1 || Height==1 || Depth==1 is the same as 2d 
	//PERF: if 2 of the following are true: Width==2, Height==1, Depth==1: use memcpy

	//PERF: optimal swiz/unswiz of 32-bit should be done with 8x2x2. (requires 8x4x4 area, aligned to 8x2x2)
	//PERF: optimal 16-bit unswiz is 8x2x2 movups (requires 8x4x4 area, aligned to 8x2x2)
	//PERF: optimal 16-bit swiz is 8x2x2 movntps (requires 8x4x4 area, aligned to 8x2x2)
	//PERF: optimal 8-bit swiz is 4x2x2 movq 

	DWORD RWidth;
	DWORD RHeight;
	DWORD RDepth;
	DWORD UOffset;
	DWORD VOffset;
	DWORD WOffset;
	if (!pBox && !pPoint && !RowPitch && !SlicePitch)
	{
		RWidth = Width;
		RHeight = Height;
		RDepth = Depth;
		UOffset = 0;
		VOffset = 0;
		WOffset = 0;
		RowPitch = Width * BytesPerPixel;
		SlicePitch = RowPitch * Height;
	}
	else
	{
		pSource = (void*)((BYTE*)pSource + pBox->Left + pBox->Top * RowPitch + pBox->Front * SlicePitch);
		RWidth = pBox->Right - pBox->Left;
		RHeight = pBox->Bottom - pBox->Top;
		RDepth = pBox->Back - pBox->Front;
		UOffset = pPoint->u;
		VOffset = pPoint->v;
		WOffset = pPoint->w;
	}
	unsigned int u, v, w;
	Swizzler swiz(Width, Height, Depth);
	DWORD SwizU = swiz.SwizzleU(UOffset);
	DWORD SwizV = swiz.SwizzleV(VOffset);
	DWORD SwizW = swiz.SwizzleW(WOffset);

	swiz.SetW(SwizW);
	if (BytesPerPixel == 4)
	{
		DWORD* pSrc = (DWORD*)pSource;
		DWORD* pDst = (DWORD*)pDest;
		DWORD RowPitchAdjust = RowPitch - RWidth * BytesPerPixel;
		DWORD SlicePitchAdjust = SlicePitch - RHeight * RowPitch;
		for (w = RDepth; w--;)
		{
			swiz.SetV(SwizV);
			for (v = RHeight; v--;)
			{
				swiz.SetU(SwizU);
				for (u = RWidth; u--;)
				{
					pDst[swiz.Get3D()] = *(pSrc++);
					swiz.IncU();
				}
				pSrc = (DWORD*)((BYTE*)pSrc + RowPitchAdjust);
				swiz.IncV();
			}
			pSrc = (DWORD*)((BYTE*)pSrc + SlicePitchAdjust);
			swiz.IncW();
		}
	}
	else if (BytesPerPixel == 2)
	{
		WORD* pSrc = (WORD*)pSource;
		WORD* pDst = (WORD*)pDest;
		DWORD RowPitchAdjust = RowPitch - RWidth * BytesPerPixel;
		DWORD SlicePitchAdjust = SlicePitch - RHeight * RowPitch;
		for (w = RDepth; w--;)
		{
			swiz.SetV(SwizV);
			for (v = RHeight; v--;)
			{
				swiz.SetU(SwizU);
				for (u = RWidth; u--;)
				{
					pDst[swiz.Get3D()] = *(pSrc++);
					swiz.IncU();
				}
				pSrc = (WORD*)((BYTE*)pSrc + RowPitchAdjust);
				swiz.IncV();
			}
			pSrc = (WORD*)((BYTE*)pSrc + SlicePitchAdjust);
			swiz.IncW();
		}
	}
	else
	{
		BYTE* pSrc = (BYTE*)pSource;
		BYTE* pDst = (BYTE*)pDest;
		DWORD RowPitchAdjust = RowPitch - RWidth * BytesPerPixel;
		DWORD SlicePitchAdjust = SlicePitch - RHeight * RowPitch;
		for (w = RDepth; w--;)
		{
			swiz.SetV(SwizV);
			for (v = RHeight; v--;)
			{
				swiz.SetU(SwizU);
				for (u = RWidth; u--;)
				{
					pDst[swiz.Get3D()] = *(pSrc++);
					swiz.IncU();
				}
				pSrc = pSrc + RowPitchAdjust;
				swiz.IncV();
			}
			pSrc = pSrc + SlicePitchAdjust;
			swiz.IncW();
		}
	}
}

//----------------------------------------------------------------------------
// Swizzle a subrectangle from a buffer into a larger texture.  The 
// destination rectangle must be completely contained within the destination 
// texture (no clipping).
//
// If pRect is NULL, pPoint is NULL and Pitch == 0, this routine will
// assume that the source buffer is exactly the same size as the destination
// texture and will swizzle the whole thing.  This routine will run
// considerably faster in that case.
//
extern "C"
VOID WINAPI EmuSwizzleRect(
	LPCVOID pSource,      // The buffer that contains the source rectangle
	DWORD   Pitch,        // The pitch of the buffer that contains the source
	LPCRECT pRect,        // The rectangle within the buffer to copy.
	LPVOID  pDest,        // The destination texture.
	DWORD   Width,        // The width of the entire destination texture.
	DWORD   Height,       // The height of the entire destination texture.
	CONST LPPOINT pPoint, // Where to put the rectangle in the texture.
	DWORD   BytesPerPixel
)
{
	//if SSE instructions are not supported, XGSwizzleBox with depth==1 does the same thing as 
	  //XGSwizzleRect, but doesn't use pentium-3-specific instructions
	D3DBOX Box, * pBox = &Box;
	XGPOINT3D Point3, * pPoint3 = &Point3;
	if (!pRect) {
		pBox = NULL;
	}
	else {
		Box.Left = pRect->left;
		Box.Right = pRect->right;
		Box.Top = pRect->top;
		Box.Bottom = pRect->bottom;
		Box.Front = 0;
		Box.Back = 1;
	}
	if (!pPoint) {
		pPoint3 = NULL;
	}
	else {
		Point3.u = pPoint->x;
		Point3.v = pPoint->y;
		Point3.w = 0;
	}
	EmuSwizzleBox(pSource, Pitch, Pitch * Width, pBox, pDest, Width, Height, 1, pPoint3, BytesPerPixel);
	return;
}


//finds a surface within a pixel container with specified cube face type and surface level.
//reversed from DisplacementMap sample.
void FindSurfaceWithinTexture(
	xbox::X_D3DPixelContainer* pPixels,
	D3DCUBEMAP_FACES FaceType,
	UINT Level,
	BYTE** ppbData,
	DWORD* pRowPitch,
	DWORD* pSlicePitch,
	DWORD* pFormat,
	DWORD* pSize
)
{
	BYTE* pbData = (BYTE*)pPixels->Data;
	xbox::X_D3DFORMAT X_Format=GetXboxPixelContainerFormat(pPixels->Format);
	DWORD TexelSize = EmuXBFormatBitsPerPixel(X_Format);
	DWORD RowPitch, SlicePitch;
	DWORD Size = pPixels->Size;
	if (!Size)
	{
		DWORD TextureLogWidth = (pPixels->Format & X_D3DFORMAT_USIZE_MASK) >> X_D3DFORMAT_USIZE_SHIFT;
		DWORD TextureLogHeight = (pPixels->Format & X_D3DFORMAT_VSIZE_MASK) >> X_D3DFORMAT_VSIZE_SHIFT;
		DWORD TextureLogDepth = (pPixels->Format & X_D3DFORMAT_PSIZE_MASK) >> X_D3DFORMAT_PSIZE_SHIFT;
		DWORD LogMin = EmuXBFormatIsCompressed(X_Format) ? 2 : 0;
		if (FaceType != D3DCUBEMAP_FACE_POSITIVE_X)
		{
			DWORD cLevels = (pPixels->Format & X_D3DFORMAT_MIPMAP_MASK) >> X_D3DFORMAT_MIPMAP_SHIFT;
			DWORD LogWidth = TextureLogWidth;
			DWORD LogHeight = TextureLogHeight;
			DWORD LogDepth = TextureLogDepth;
			DWORD FaceSize = 0;
			for (; cLevels; cLevels--)
			{
				FaceSize += (1 << (MAX(LogWidth, LogMin) + MAX(LogHeight, LogMin))) * TexelSize / 8;
				if (LogWidth > 0)
				{
					LogWidth--;
				}
				if (LogHeight > 0)
				{
					LogHeight--;
				}
				if (LogDepth > 0)
				{
					LogDepth--;
				}
			}
			FaceSize = (FaceSize + X_D3DTEXTURE_CUBEFACE_ALIGNMENT - 1) & ~(X_D3DTEXTURE_CUBEFACE_ALIGNMENT - 1);
			pbData += FaceSize * FaceType;
		}
		if (Level)
		{
			DWORD cLevels = Level;
			DWORD LogSize;
			for (; cLevels; cLevels--)
			{
				LogSize = MAX(TextureLogWidth, LogMin) + MAX(TextureLogHeight, LogMin) + TextureLogDepth;
				pbData += (1 << LogSize) * TexelSize / 8;
				if (TextureLogWidth > 0)
				{
					TextureLogWidth--;
				}
				if (TextureLogHeight > 0)
				{
					TextureLogHeight--;
				}
				if (TextureLogDepth > 0)
				{
					TextureLogDepth--;
				}
			}
		}
		DWORD Format = pPixels->Format & ~(X_D3DFORMAT_USIZE_MASK | X_D3DFORMAT_VSIZE_MASK | X_D3DFORMAT_PSIZE_MASK);
		Format |= TextureLogWidth << X_D3DFORMAT_USIZE_SHIFT;
		Format |= TextureLogHeight << X_D3DFORMAT_VSIZE_SHIFT;
		Format |= TextureLogDepth << X_D3DFORMAT_PSIZE_SHIFT;
		TextureLogWidth = MAX(TextureLogWidth, LogMin);
		TextureLogHeight = MAX(TextureLogHeight, LogMin);
		switch (X_Format)
		{
		case xbox::X_D3DFMT_DXT1://0x0C
			RowPitch = (1 << TextureLogWidth) * 2;
			break;
		case xbox::X_D3DFMT_DXT2://0x0E
		case xbox::X_D3DFMT_DXT4://0x0F
			RowPitch = (1 << TextureLogWidth) * 4;
			break;
		default:
			RowPitch = (1 << TextureLogWidth) * TexelSize / 8;
		}
		SlicePitch = (1 << (TextureLogWidth + TextureLogHeight)) * TexelSize / 8;
		*pFormat = Format;
		*pSize = pPixels->Size;
	}
	else
	{
		DWORD TextureWidth = (Size & X_D3DSIZE_WIDTH_MASK) + 1;
		DWORD TextureHeight = ((Size & X_D3DSIZE_HEIGHT_MASK) >> X_D3DSIZE_HEIGHT_SHIFT) + 1;
		DWORD Pitch = (((Size & X_D3DSIZE_PITCH_MASK) >> X_D3DSIZE_PITCH_SHIFT) + 1) * X_D3DTEXTURE_PITCH_ALIGNMENT;
		RowPitch = Pitch;
		SlicePitch = RowPitch * TextureHeight;
		*pFormat = pPixels->Format;
		*pSize = pPixels->Size;
	}
	*ppbData = pbData;
	*pRowPitch = RowPitch;
	*pSlicePitch = SlicePitch;
}
typedef enum _ComponentEncoding {
	NoCmpnts = 0, // Format doesn't contain any component (ARGB/QWVU)
	A1R5G5B5,
	X1R5G5B5, // NOTE : A=255
	A4R4G4B4,
	__R5G6B5, // NOTE : A=255
	A8R8G8B8,
	X8R8G8B8, // NOTE : A=255
	____R8B8, // NOTE : A takes R, G takes B
	____G8B8, // NOTE : A takes G, R takes B
	______A8, // TEST : R=G=B= 255
	__R6G5B5, // NOTE : A=255
	R5G5B5A1,
	R4G4B4A4,
	A8B8G8R8,
	B8G8R8A8,
	R8G8B8A8,
	______L8, // NOTE : A=255, R=G=B= L
	_____AL8, // NOTE : A=R=G=B= L
	_____L16, // NOTE : Actually G8B8, with A=R=255
	____A8L8, // NOTE : R=G=B= L
	____DXT1,
	____DXT3,
	____DXT5,
	______P8,
	____YUY2,
	____UYVY,
	__G16B16, // NOTE : A takes G, R takes B
	__R8G8B8,
}ComponentEncoding;
extern DWORD EmuPCFormatBytesPerPixel(D3DFORMAT pc);
//extern enum ComponentEncoding;
extern ComponentEncoding EmuPCFormatComponentEncoding(D3DFORMAT pc);
extern ComponentEncoding EmuXBFormatComponentEncoding(xbox::X_D3DFORMAT Format);
extern BOOL EmuXBFormatIsBumpMap(xbox::X_D3DFORMAT Format);

//find corresponding host surface, lock it and transfer data from host surface to xbox surface. swizzle the data if xbox surface format is swizzle format
void LoadSurfaceDataFromHost(xbox::X_D3DSurface* pXboxSurface)
{
	//check if the pPixelContainer  is in our resource cache or not
	auto key = GetHostResourceKey(pXboxSurface); // Note : iTextureStage is unknown here!
	auto& ResourceCache = GetResourceCache(key);
	auto it = ResourceCache.find(key);
	//return without any update when there is no host resource been created with this surface.
	if (it == ResourceCache.end())
		return;
	//from here, we're sure our xbox surface has an corresponding host surface exists.
	DWORD D3DUsage = D3DUSAGE_DYNAMIC;//only D3DUSAGE_DYNAMIC is lockable; 
	IDirect3DSurface* pHostSourceSurface;
	pHostSourceSurface = (IDirect3DSurface*)it->second.pHostResource.Get();
	bool g_IsRenderTexture = false;
	D3DSURFACE_DESC HostSurfaceDesc;
	D3DLOCKED_RECT HostLockedRect;
	HRESULT hRet;

	hRet = pHostSourceSurface->GetDesc(&HostSurfaceDesc);
	if (hRet != D3D_OK) {
		EmuLog(LOG_LEVEL::WARNING, "Could not get host surface desc in LoadSurfaceDataFromHost");
	}

	if (HostSurfaceDesc.Usage == D3DUSAGE_RENDERTARGET)
		g_IsRenderTexture = true;

	//xbox::X_D3DBaseTexture OffScreenTexture, * pOffScreenTexture;
	//pOffScreenTexture = &OffScreenTexture;
	//OffScreenTexture = *(xbox::X_D3DBaseTexture*)pXboxSurface;

	IDirect3DTexture9* pHostOffScreenTexture;
	IDirect3DSurface9* pHostOffScreenSurface;
	//g_IsRenderTexture = true;
	bool bOffScreenSurfaceNeedRelease = false;
	DWORD hostFlag = D3DLOCK_READONLY;
	if (g_IsRenderTexture)
	{
		hRet = g_pD3DDevice->CreateTexture(
			HostSurfaceDesc.Width,
			HostSurfaceDesc.Height,
			1,
			D3DUSAGE_DYNAMIC,
			HostSurfaceDesc.Format,
			D3DPOOL_SYSTEMMEM,
			&pHostOffScreenTexture,
			NULL
		);
		if (hRet != D3D_OK) {
			EmuLog(LOG_LEVEL::WARNING, "Failed in pHostOffScreenTexture->GetSurfaceLevel(0, &pHostOffScreenSurface); in LoadSurfaceDataFromHost");
		}
		bOffScreenSurfaceNeedRelease = true;
		if (hRet == D3D_OK)hRet = pHostOffScreenTexture->GetSurfaceLevel(0, &pHostOffScreenSurface);
		if (hRet != D3D_OK) {
			EmuLog(LOG_LEVEL::WARNING, "Failed in pHostOffScreenTexture->GetSurfaceLevel(0, &pHostOffScreenSurface); in LoadSurfaceDataFromHost");
		}

		if (hRet == D3D_OK)hRet = g_pD3DDevice->GetRenderTargetData(pHostSourceSurface, pHostOffScreenSurface);
		if (hRet != D3D_OK) {
			EmuLog(LOG_LEVEL::WARNING, "Failed in GetRenderTargetData in LoadSurfaceDataFromHost");
		}
		D3DSURFACE_DESC HostOffScreenSurfaceDesc;
		if (hRet == D3D_OK)pHostOffScreenSurface->GetDesc(&HostOffScreenSurfaceDesc);

		assert(HostOffScreenSurfaceDesc.Width == HostSurfaceDesc.Width);
		assert(HostOffScreenSurfaceDesc.Height == HostSurfaceDesc.Height);
		hostFlag = D3DLOCK_READONLY;
		if (hRet == D3D_OK)hRet = pHostOffScreenSurface->LockRect(&HostLockedRect, NULL, hostFlag);
		if (hRet != D3D_OK) {
			EmuLog(LOG_LEVEL::WARNING, "Could not lock Host Surface for Xbox texture in LoadSurfaceDataFromHost");
		}
	}
	//not render target texture, process it as regular texture, LockRect then transfer to xbox data with conversion.
	//if the texture is not lockable, then copy it to a lockable surface with StretchRect() then transfer to xbox with conversion.
	else {
		pHostOffScreenSurface = pHostSourceSurface;
		hRet = pHostOffScreenSurface->LockRect(&HostLockedRect, NULL, hostFlag);

		if (hRet == D3D_OK) {
			;
		}
		else {
			EmuLog(LOG_LEVEL::WARNING, "Failed in pHostOffScreenSurface->LockRect(pLockedRect, NULL, hostFlag); in LoadSurfaceDataFromHost");

			//hRet = g_pD3DDevice->CreateTexture(textureDesc.Width, textureDesc.Height,1, textureDesc.Usage, textureDesc.Format, D3DPOOL_DEFAULT, &pTexture,(HANDLE*)nullptr);
			D3DUsage = D3DUSAGE_DYNAMIC;// textureDesc.Usage;// 
			hRet = g_pD3DDevice->CreateTexture(
				HostSurfaceDesc.Width,
				HostSurfaceDesc.Height,
				1,
				D3DUSAGE_DYNAMIC,
				HostSurfaceDesc.Format,
				D3DPOOL_DEFAULT,
				&pHostOffScreenTexture,
				NULL
			);
			if (hRet != D3D_OK) {
				EmuLog(LOG_LEVEL::WARNING, "Failed in pHostOffScreenTexture->GetSurfaceLevel(0, &pHostOffScreenSurface); in LoadSurfaceDataFromHost");
			}
			bOffScreenSurfaceNeedRelease = true;

			hRet = pHostOffScreenTexture->GetSurfaceLevel(0, &pHostOffScreenSurface);
			if (hRet != D3D_OK) {
				EmuLog(LOG_LEVEL::WARNING, "Failed in pHostOffScreenTexture->GetSurfaceLevel(0, &pHostOffScreenSurface); in LoadSurfaceDataFromHost");
			}
			// Copy Surface to Surface

			hRet = g_pD3DDevice->StretchRect(
				pHostSourceSurface,
				NULL,
				pHostOffScreenSurface,
				NULL,
				D3DTEXF_NONE
			);
			if (hRet != D3D_OK) {
				EmuLog(LOG_LEVEL::WARNING, "Failed in g_pD3DDevice->StretchRect() in LoadSurfaceDataFromHost");
			}

			hostFlag = D3DLOCK_READONLY;
			hRet = pHostOffScreenSurface->LockRect(&HostLockedRect, NULL, hostFlag);
			if (hRet != D3D_OK) {
				EmuLog(LOG_LEVEL::WARNING, "Failed in pHostOffScreenSurface->LockRect(pLockedRect, NULL, hostFlag); in LoadSurfaceDataFromHost");
			}
		}
	}

	//process host surface data transfer to xbox data with data conversion.
	xbox::X_D3DFORMAT X_Format = GetXboxPixelContainerFormat(pXboxSurface);
	// Interpret Width/Height/BPP
	bool bXBSwizzled = EmuXBFormatIsSwizzled(X_Format);
	bool bCompressed = EmuXBFormatIsCompressed(X_Format);
	UINT XB_BPP = EmuXBFormatBytesPerPixel(X_Format);
	UINT MipMapLevels = CxbxGetPixelContainerMipMapLevels(pXboxSurface);
	UINT XBWidth, XBHeight, XBDepth, XBRowPitch, XBSlicePitch;
	ComponentEncoding XBComponentEncoding = EmuXBFormatComponentEncoding(X_Format);
	// Interpret Width/Height/BPP
	CxbxGetPixelContainerMeasures(pXboxSurface, 0, &XBWidth, &XBHeight, &XBDepth, &XBRowPitch, &XBSlicePitch);

	// Host width and height dimensions
	UINT PCWidth = HostSurfaceDesc.Width;
	UINT PCHeight = HostSurfaceDesc.Height;
	UINT PCPitch = HostLockedRect.Pitch;
	// instead of using width/height from xbox surface directly, getting host width/height from the host surface desc
	// these two dimensions should be the same, shall we check if they might be different?
	assert(XBWidth == PCWidth);
	assert(XBHeight == PCHeight);
	D3DFORMAT PCFormat = HostSurfaceDesc.Format;

	UINT PC_BPP = EmuPCFormatBytesPerPixel(PCFormat);
	ComponentEncoding PCComponentEncoding= EmuPCFormatComponentEncoding(PCFormat);

	//bool bConvertToARGB = false;
	//xbox::X_D3DRESOURCETYPE XboxResourceType = GetXboxD3DResourceType(pXboxSurface);

	//D3DUsage = HostSurfaceDesc.Usage;
	byte* hostPtr = (byte*)HostLockedRect.pBits;
	byte* xboxPtr = (byte*)GetDataFromXboxResource(pXboxSurface);// (byte*)pPixelContainer->Data;

	// check host format and xbox format, convert format if necessary.
	// todo:test if comparing BPP is enough
	if (PC_BPP == XB_BPP ){//&& PCComponentEncoding == XBComponentEncoding) {
		//same format, no format conversion.
		;
	}
	else {
		//different format, need format conversion.
		//hack: assuming formats are the same, so put an assert() here just in case there are different formats.
		assert(0);
	}


	byte* SwizzleBuffer = nullptr;
	// check xbox format is swizzle format or not. swizzle xbox surface data when xbox format is swizzle format.
	//bXBSwizzled = false;
	if (bXBSwizzled) {
		//xbox is swizzle format, let's swizzle the data.
		hostPtr = (byte*)HostLockedRect.pBits;
		xboxPtr = (byte*)GetDataFromXboxResource(pXboxSurface);// (byte*)pPixelContainer->Data;
		if (PCPitch != XBRowPitch) {
			//allocate an temp buffer to adjust host data row pitch to be the same as xbox row pitch so EmuSwizzleRect can work.
			SwizzleBuffer = (byte*)malloc(XBSlicePitch);
			xboxPtr = SwizzleBuffer;
			for (int i = 0; i < XBHeight; i++) {
				memcpy(xboxPtr, hostPtr, XBRowPitch);
				xboxPtr += XBRowPitch;
				hostPtr += PCPitch;
			}
			hostPtr = SwizzleBuffer;
			xboxPtr = (byte*)GetDataFromXboxResource(pXboxSurface);
		}
		//
		EmuSwizzleRect(
			hostPtr,        // The buffer that contains the source rectangle
			0,//XBRowPitch,     // The pitch of the buffer that contains the source
			nullptr,        // The rectangle within the buffer to copy.
			xboxPtr,        // The destination texture.
			XBWidth,        // The width of the entire destination texture.
			XBHeight,       // The height of the entire destination texture.
			nullptr,        // Where to put the rectangle in the texture.
			XB_BPP
		);
	}
	else {
		//xbox is not swizzle format, just copy the data from host to xbox.
		if (HostLockedRect.Pitch == XBRowPitch)
			memcpy((void*)xboxPtr, hostPtr, HostLockedRect.Pitch * XBHeight);
		else {
			DWORD minPitch = MIN(HostLockedRect.Pitch, XBRowPitch);
			for (int i = 0; i < XBHeight; i++) {
				memcpy(xboxPtr, hostPtr, minPitch);
				xboxPtr += XBRowPitch;
				hostPtr += HostLockedRect.Pitch;
			}
		}
	}
	if (SwizzleBuffer != nullptr)
		free(SwizzleBuffer);
	//unlock surface before we leave
	hRet = pHostOffScreenSurface->UnlockRect();
	//release the created pHostOffScreenTexture and pHostOffScreenSurface
	if (bOffScreenSurfaceNeedRelease) {
		pHostOffScreenSurface->Release();
		if (hRet != D3D_OK) {
			EmuLog(LOG_LEVEL::WARNING, "Failed in pHostOffScreenSurface->Release() in LoadSurfaceDataFromHost");
		}
		pHostOffScreenTexture->Release();
		if (hRet != D3D_OK) {
			EmuLog(LOG_LEVEL::WARNING, "Failed in pHostOffScreenTexture->Release() in LoadSurfaceDataFromHost");
		}
	}
	//update xbox data hash and related time variables to prevent unnecessary host surface update.
	auto& resourceInfo = ResourceCache[key];	// Implicitely inserts a new entry if not already existing
	resourceInfo.hash = ComputeHash(resourceInfo.pXboxData, resourceInfo.szXboxDataSize);
	resourceInfo.hashLifeTime = 1ms;
	resourceInfo.lastUpdate = std::chrono::steady_clock::now();
	resourceInfo.nextHashTime = resourceInfo.lastUpdate + resourceInfo.hashLifeTime;
	resourceInfo.forceRehash = false;
}

void GetXboxSurfaceAndTransferFromHostSurface
(
	xbox::X_D3DPixelContainer* pPixelContainer,
	D3DCUBEMAP_FACES           FaceType,
	xbox::uint_xt              Level
)
{
	//first we find the surface in the pixel container
	BYTE* pbData;
	DWORD RowPitch;
	DWORD SlicePitch;
	DWORD Format;
	DWORD Size;
	FindSurfaceWithinTexture(pPixelContainer, FaceType,	Level, &pbData,	&RowPitch, &SlicePitch,	&Format, &Size);

	xbox::X_D3DSurface XboxSurface = {0};
	XboxSurface.Data = (DWORD)pbData;
	XboxSurface.Format = Format;
	XboxSurface.Size = Size;
	XboxSurface.Common = pPixelContainer->Common & 0x010FFFFF;
	XboxSurface.Common |= 0x00010000;
	xbox::X_D3DSurface* pXboxSurface = &XboxSurface;
	LoadSurfaceDataFromHost(pXboxSurface);
}

void WINAPI CxbxrImpl_Lock2DSurface
(
	xbox::X_D3DPixelContainer* pPixelContainer,
	D3DCUBEMAP_FACES           FaceType,
	xbox::uint_xt              Level,
	D3DLOCKED_RECT*            pLockedRect,
	RECT*                      pRect,
	xbox::dword_xt             Flags
)
{
	GetXboxSurfaceAndTransferFromHostSurface(pPixelContainer, FaceType, Level);

	//only force rehash xbox data when there is no X_D3DLOCK_READONLY flag. because read only lock doesn't write data to xbox data.
	if ((Flags & X_D3DLOCK_READONLY) == 0)
	    ForceResourceRehash(pPixelContainer);
}

// ******************************************************************
// * patch: Lock2DSurface
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(Lock2DSurface)
(
	X_D3DPixelContainer *pPixelContainer,
	D3DCUBEMAP_FACES     FaceType,
	uint_xt                 Level,
	D3DLOCKED_RECT      *pLockedRect,
	RECT                *pRect,
	dword_xt                Flags
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pPixelContainer)
		LOG_FUNC_ARG(FaceType)
		LOG_FUNC_ARG(Level)
		LOG_FUNC_ARG(pLockedRect)
		LOG_FUNC_ARG(pRect)
		LOG_FUNC_ARG(Flags)
		LOG_FUNC_END;


	// Pass through to the Xbox implementation of this function
	XB_TRMP(Lock2DSurface)(pPixelContainer, FaceType, Level, pLockedRect, pRect, Flags);

	if (pPixelContainer == nullptr)
		return;
   
	//template for syncing HLE apis with pgraf using waiting lock
	bool WaitForPGRAPH;
	WaitForPGRAPH = true;
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)&WaitForPGRAPH;// (DWORD)PrimitiveType;

	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_Lock2DSurface, 1, PBTokenArray);//argCount, not necessary, default to 14

	EmuKickOff();

	while (WaitForPGRAPH)
		; //this line is must have

	// Mark the resource as modified
	CxbxrImpl_Lock2DSurface(pPixelContainer, FaceType, Level, pLockedRect, pRect, Flags);
}

void WINAPI CxbxrImpl_Lock3DSurface
(
	xbox::X_D3DPixelContainer* pPixelContainer,
	xbox::uint_xt              Level,
	D3DLOCKED_BOX*             pLockedVolume,
	D3DBOX*                    pBox,
	xbox::dword_xt			   Flags
)
{
	GetXboxSurfaceAndTransferFromHostSurface(pPixelContainer, D3DCUBEMAP_FACE_POSITIVE_X, Level);
	//only force rehash xbox data when there is no X_D3DLOCK_READONLY flag. because read only lock doesn't write data to xbox data.
	if ((Flags & X_D3DLOCK_READONLY) == 0)
		ForceResourceRehash(pPixelContainer);
}

// ******************************************************************
// * patch: Lock3DSurface
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(Lock3DSurface)
(
	X_D3DPixelContainer *pPixelContainer,
	uint_xt				Level,
	D3DLOCKED_BOX		*pLockedVolume,
	D3DBOX				*pBox,
	dword_xt				Flags
	)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pPixelContainer)
		LOG_FUNC_ARG(Level)
		LOG_FUNC_ARG(pLockedVolume)
		LOG_FUNC_ARG(pBox)
		LOG_FUNC_ARG(Flags)
		LOG_FUNC_END;

	// Pass through to the Xbox implementation of this function
	HRESULT hRet = XB_TRMP(Lock3DSurface)(pPixelContainer, Level, pLockedVolume, pBox, Flags);

	// Mark the resource as modified
		//template for syncing HLE apis with pgraf using waiting lock
	bool WaitForPGRAPH;
	WaitForPGRAPH = true;
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)&WaitForPGRAPH;// (DWORD)PrimitiveType;

	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_Lock3DSurface, 1, PBTokenArray);//argCount, not necessary, default to 14

	EmuKickOff();

	while (WaitForPGRAPH)
		; //this line is must have
	CxbxrImpl_Lock3DSurface(pPixelContainer, Level, pLockedVolume, pBox, Flags);
	return hRet;
}

CONST DWORD g_TilePitches[] =
{
	0x0200	, //	D3DTILE_PITCH_0200      
	0x0300	, //	D3DTILE_PITCH_0300
	0x0400	, //	D3DTILE_PITCH_0400
	0x0500	, //	D3DTILE_PITCH_0500
	0x0600	, //	D3DTILE_PITCH_0600
	0x0700	, //	D3DTILE_PITCH_0700
	0x0800	, //	D3DTILE_PITCH_0800
	0x0A00	, //	D3DTILE_PITCH_0A00
	0x0C00	, //	D3DTILE_PITCH_0C00
	0x0E00	, //	D3DTILE_PITCH_0E00
	0x1000	, //	D3DTILE_PITCH_1000
	0x1400	, //	D3DTILE_PITCH_1400
	0x1800	, //	D3DTILE_PITCH_1800
	0x1C00	, //	D3DTILE_PITCH_1C00
	0x2000	, //	D3DTILE_PITCH_2000
	0x2800	, //	D3DTILE_PITCH_2800
	0x3000	, //	D3DTILE_PITCH_3000
	0x3800	, //	D3DTILE_PITCH_3800
	0x4000	, //	D3DTILE_PITCH_4000
	0x5000	, //	D3DTILE_PITCH_5000
	0x6000	, //	D3DTILE_PITCH_6000
	0x7000	, //	D3DTILE_PITCH_7000
	0x8000	, //	D3DTILE_PITCH_8000
	0xA000	, //	D3DTILE_PITCH_A000
	0xC000	, //	D3DTILE_PITCH_C000
	0xE000	, //	D3DTILE_PITCH_E000
};
//using fixed size array to cache the textures.
//todo: check if 32 is enough or not.
xbox::X_D3DSurface g_XGSetSurface[32] = {0};
xbox::X_D3DSurface *g_pXGSetSurface[32] = { 0 };

// currently this function is unpatched, because we use GetSurfaceLevel() to cache.
xbox::hresult_xt WINAPI EMUPATCH(XGSetTextureHeader)
(
	UINT Width,
	UINT Height,
	UINT  Levels,
	DWORD Usage,
	xbox::X_D3DFORMAT Format,
	D3DPOOL Pool,
	xbox::X_D3DTexture* pTexture,
	UINT Data,
	UINT Pitch
)
{
	HRESULT hRet = XB_TRMP(XGSetTextureHeader)(
		Width,
		Height,
		Levels,
		Usage,
		Format,
		Pool,
		pTexture,
		Data,
		Pitch);

	return hRet;
}

#define X_D3DTILE_FLAGS_ZBUFFER       0x00000001
#define X_D3DTILE_FLAGS_ZCOMPRESS     0x80000000
#define X_D3DTILE_FLAGS_Z32BITS       0x04000000
#define X_D3DTILE_FLAGS_Z16BITS       0x00000000
#define X_D3DTILE_FLAGS_ZBUFFER_FLAGS (X_D3DTILE_FLAGS_ZBUFFER | X_D3DTILE_FLAGS_ZCOMPRESS | X_D3DTILE_FLAGS_Z32BITS | X_D3DTILE_FLAGS_Z16BITS)

xbox::X_D3DTILE* g_pTile[8] = { 0 };
xbox::X_D3DTILE g_Tile[8] = { 0 };
xbox::X_D3DSurface g_TileSurface[8] = { 0 };

// ******************************************************************
// * patch: IDirect3DResource8_Register
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DResource_Register)
(
	xbox::X_D3DResource* pThis,
	xbox::PVOID          pBase
)
{
	XB_TRMP(D3DResource_Register)(pThis, pBase);
	//if it's a surface resource, cache it.
	if((pThis->Common &0x000F0000)==0x00050000){
		UINT32 key = (UINT32)pBase & 0x0FFFFFFF;
		g_BufferSurfaceCache[key]=*(xbox::X_D3DSurface*)pThis;
	}
}

// ******************************************************************
// * patch: CreateStandAloneSurface
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CreateStandAloneSurface)
(
	DWORD Width,
	DWORD Height,
	xbox::X_D3DFORMAT XFormat,
	xbox::X_D3DSurface** ppSurface
)
{
	XB_TRMP(CreateStandAloneSurface)(Width, Height, XFormat, ppSurface);
	xbox::X_D3DSurface* pSurface = *ppSurface;
	//cache the surface if Data!=0
	if (pSurface->Data != 0) {
		UINT32 key = pSurface->Data;
		g_BufferSurfaceCache[key] = *pSurface;
	}
	return S_OK;
}

// ******************************************************************
// * patch: XGSetSurfaceHeader
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(XGSetSurfaceHeader)
(
	UINT Width,
	UINT Height,
	xbox::X_D3DFORMAT XFormat,
	xbox::X_D3DSurface* pSurface,
	UINT Data,
	UINT Pitch
)
{
	XB_TRMP(XGSetSurfaceHeader)(Width, Height, XFormat, pSurface, Data, Pitch);
	//cache the surface if Data!=0
	if (pSurface->Data != 0) {
		UINT32 key = pSurface->Data;
		g_BufferSurfaceCache[key] = *pSurface;
	}
	return S_OK;
}

// ******************************************************************
// * patch: IDirect3DTexture8_GetSurfaceLevel
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DTexture_GetSurfaceLevel)
(
	xbox::X_D3DTexture*  pThis,
	xbox::uint_xt        Level,
	xbox::X_D3DSurface** ppSurface
)
{
	XB_TRMP(D3DTexture_GetSurfaceLevel)(pThis, Level, ppSurface);
	xbox::X_D3DSurface* pSurface = *ppSurface;
	//cache the surface if Data!=0 and Level ==0
	if (Level==0 && pSurface->Data != 0) {
		UINT32 key = pSurface->Data;
		g_BufferSurfaceCache[key] = *pSurface;
	}
	return S_OK;
}


// ******************************************************************
// * patch: IDirect3DTexture8_GetSurfaceLevel2
// ******************************************************************
xbox::X_D3DSurface* WINAPI xbox::EMUPATCH(D3DTexture_GetSurfaceLevel2)
(
	X_D3DTexture* pThis,
	uint_xt            Level
)
{
	xbox::X_D3DSurface* pSurface;
	pSurface=XB_TRMP(D3DTexture_GetSurfaceLevel2)(pThis, Level);

	//cache the surface if Data!=0 and Level ==0
	if (Level == 0 && pSurface->Data != 0) {
		UINT32 key = pSurface->Data;
		g_BufferSurfaceCache[key] = *pSurface;
	}
	return pSurface;
}

void CxbxrImpl_SetTile
(
	xbox::dword_xt Index,
	CONST xbox::X_D3DTILE* pTile
)
{
	if (Index < 2)
		return;
	if (pTile != nullptr) {
		if (pTile->pMemory != nullptr) {
			//if (Index > 1 ) {
			g_Tile[Index] = *pTile;
			//g_pTile[Index] = &g_Tile[Index];
			UINT32 key = (DWORD)pTile->pMemory & 0x0FFFFFFF;
			auto it = g_BufferSurfaceCache.find(key);
			if (it != g_BufferSurfaceCache.end()) {
				g_TileSurface[Index] = it->second;
				if ((pTile->Flags & X_D3DTILE_FLAGS_ZBUFFER_FLAGS) != 0) {
					//set depth stencil buffer
					EmuSetDepthStencil(&g_TileSurface[Index]);
				}
				else {
					//set back buffer
					EmuSetRenderTarget(&g_TileSurface[Index]);
				}
			}
			else {
				EmuLog(LOG_LEVEL::WARNING, "D3DDevice_SetTile: Tile.pMemory not found in g_BufferSurfaceCache!");
			}
		}
	}
	else {
		if (g_Tile[Index].pMemory != nullptr) {
			if ((g_Tile[Index].Flags & X_D3DTILE_FLAGS_ZBUFFER_FLAGS) != 0) {
				//set depth stencil buffer
				EmuSetDepthStencil(&g_TileSurface[1]);
			}
			else {
				//set back buffer
				EmuSetRenderTarget(&g_TileSurface[0]);
			}
		}
		g_Tile[Index].pMemory = nullptr;
	}
}

xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_SetTile)
(
	xbox::dword_xt Index,
	CONST xbox::X_D3DTILE* pTile
)
{
	// index 0 and 1 are default back buffer and depth stencil. it's not supposed to be set by titles.
	if (Index < 2)
		return S_OK;
	// do not trampoline. we don't need xbox to setup tile.
	//HRESULT hRet = XB_TRMP(D3DDevice_SetTile)(Index, pTile);
	bool WaitForPGRAPH;
	WaitForPGRAPH = true;
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)Index;
	PBTokenArray[3] = (DWORD)pTile;
	PBTokenArray[4] = (DWORD)&WaitForPGRAPH;// (DWORD)PrimitiveType;

	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetTile, 3);//argCount, not necessary, default to 14

	EmuKickOff();

	while (WaitForPGRAPH)
		;
	CxbxrImpl_SetTile(Index, pTile);
	return S_OK;
}

xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_GetTile)
(
	xbox::dword_xt Index,
	xbox::X_D3DTILE* pTile
)
{
	//do not trampoline.
	//HRESULT hRet= XB_TRMP(D3DDevice_GetTile)(Index, pTile);
	*pTile = g_Tile[Index];
	return S_OK;
}

// ******************************************************************
// * patch: D3DDevice_GetMaterial
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(XGSetVertexBufferHeader)
(
	UINT Length,
	DWORD Usage,
	DWORD FVF,
	D3DPOOL Pool,
	xbox::X_D3DVertexBuffer* pBuffer,
	UINT Data
)
{
	//trampoline
	HRESULT hRet = XB_TRMP(XGSetVertexBufferHeader)(Length, Usage, FVF, Pool, pBuffer, Data);
	return hRet;
}

// Overload for logging
static void D3DDevice_SetStreamSource_0__LTCG_eax_StreamNumber_edi_pStreamData_ebx_Stride
(
    xbox::uint_xt            StreamNumber,
    xbox::X_D3DVertexBuffer *pStreamData,
    xbox::uint_xt            Stride
)
{
    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(StreamNumber)
        LOG_FUNC_ARG(pStreamData)
        LOG_FUNC_ARG(Stride)
        LOG_FUNC_END;
}

// LTCG specific D3DDevice_SetStreamSource function...
// This uses a custom calling convention where parameters are passed in EAX, EDI, EBX
// Test-case: Juiced
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetStreamSource_0__LTCG_eax_StreamNumber_edi_pStreamData_ebx_Stride)()
{
    uint_xt StreamNumber;
    X_D3DVertexBuffer *pStreamData;
    uint_xt Stride;
    __asm {
        LTCG_PROLOGUE
        mov  StreamNumber, eax
        mov  pStreamData, edi
        mov  Stride, ebx
    }
    // Log
    D3DDevice_SetStreamSource_0__LTCG_eax_StreamNumber_edi_pStreamData_ebx_Stride(StreamNumber, pStreamData, Stride);

    //CxbxrImpl_SetStreamSource(StreamNumber, pStreamData, Stride);

	//EMUPATCH(D3DDevice_SetShaderConstantMode)(Mode);
	//CxbxrImpl_SetShaderConstantMode(Mode);
	//1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)StreamNumber;
	PBTokenArray[3] = (DWORD)pStreamData;
	PBTokenArray[4] = (DWORD)Stride;
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetStreamSource, 3);

    __asm {
        mov  eax, StreamNumber
        mov  edi, pStreamData
        mov  ebx, Stride
        call XB_TRMP(D3DDevice_SetStreamSource_0__LTCG_eax_StreamNumber_edi_pStreamData_ebx_Stride)

        LTCG_EPILOGUE
        ret
    }
}

// Overload for logging
static void D3DDevice_SetStreamSource_4
(
    xbox::uint_xt            StreamNumber,
    xbox::X_D3DVertexBuffer *pStreamData,
    xbox::uint_xt            Stride
)
{
    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(StreamNumber)
        LOG_FUNC_ARG(pStreamData)
        LOG_FUNC_ARG(Stride)
        LOG_FUNC_END;
}

// LTCG specific D3DDevice_SetStreamSource function...
// This uses a custom calling convention where parameter is passed in EBX, EAX
// Test-case: Ninja Gaiden
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetStreamSource_4)
(
    uint_xt                Stride
)
{
    uint_xt StreamNumber;
    X_D3DVertexBuffer *pStreamData;
    __asm {
        LTCG_PROLOGUE
        mov  pStreamData, ebx
        mov  StreamNumber, eax
    }
    // Log
    D3DDevice_SetStreamSource_4(StreamNumber, pStreamData, Stride);

    //CxbxrImpl_SetStreamSource(StreamNumber, pStreamData, Stride);

	//EMUPATCH(D3DDevice_SetShaderConstantMode)(Mode);
	//CxbxrImpl_SetShaderConstantMode(Mode);
	//1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)StreamNumber;
	PBTokenArray[3] = (DWORD)pStreamData;
	PBTokenArray[4] = (DWORD)Stride;
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetStreamSource, 3);

    // Forward to Xbox implementation
    // This should stop us having to patch GetStreamSource!
    __asm {
        push Stride
        mov  ebx, pStreamData
        mov  eax, StreamNumber
        call XB_TRMP(D3DDevice_SetStreamSource_4)

        LTCG_EPILOGUE
        ret  4
    }
}

// Overload for logging
static void D3DDevice_SetStreamSource_8
(
    xbox::uint_xt            StreamNumber,
    xbox::X_D3DVertexBuffer *pStreamData,
    xbox::uint_xt            Stride
)
{
    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(StreamNumber)
        LOG_FUNC_ARG(pStreamData)
        LOG_FUNC_ARG(Stride)
        LOG_FUNC_END;
}

// This uses a custom calling convention where parameter is passed in EAX
// Test-case: Superman - The Man Of Steel
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetStreamSource_8)
(
    X_D3DVertexBuffer  *pStreamData,
    uint_xt             Stride
)
{
    uint_xt StreamNumber;
    __asm {
        LTCG_PROLOGUE
        mov  StreamNumber, eax
    }
    // Log
    D3DDevice_SetStreamSource_8(StreamNumber, pStreamData, Stride);

    //CxbxrImpl_SetStreamSource(StreamNumber, pStreamData, Stride);

	//EMUPATCH(D3DDevice_SetShaderConstantMode)(Mode);
	//CxbxrImpl_SetShaderConstantMode(Mode);
	//1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)StreamNumber;
	PBTokenArray[3] = (DWORD)pStreamData;
	PBTokenArray[4] = (DWORD)Stride;
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetStreamSource, 3);


    // Forward to Xbox implementation
    // This should stop us having to patch GetStreamSource!
    __asm {
        push Stride
        push pStreamData
        mov  eax, StreamNumber
        call XB_TRMP(D3DDevice_SetStreamSource_8)

        LTCG_EPILOGUE
        ret  8
    }
}

// This uses a custom calling convention where StreamNumber parameter is passed in EDX
// Test-case: NASCAR Heat 2002
xbox::void_xt __fastcall xbox::EMUPATCH(D3DDevice_SetStreamSource_8__LTCG_edx_StreamNumber)
(
    void*,
    uint_xt                StreamNumber,
    X_D3DVertexBuffer  *pStreamData,
    uint_xt                Stride
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(StreamNumber)
		LOG_FUNC_ARG(pStreamData)
		LOG_FUNC_ARG(Stride)
		LOG_FUNC_END;


	// Forward to Xbox implementation
	// This should stop us having to patch GetStreamSource!
	XB_TRMP(D3DDevice_SetStreamSource_8__LTCG_edx_StreamNumber)(nullptr, StreamNumber, pStreamData, Stride);

	//CxbxrImpl_SetStreamSource(StreamNumber, pStreamData, Stride);

	//EMUPATCH(D3DDevice_SetShaderConstantMode)(Mode);
	//CxbxrImpl_SetShaderConstantMode(Mode);
	//1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)StreamNumber;
	PBTokenArray[3] = (DWORD)pStreamData;
	PBTokenArray[4] = (DWORD)Stride;
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetStreamSource, 3);

}

// ******************************************************************
// * patch: D3DDevice_SetStreamSource
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetStreamSource)
(
    uint_xt                StreamNumber,
    X_D3DVertexBuffer  *pStreamData,
    uint_xt                Stride
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(StreamNumber)
		LOG_FUNC_ARG(pStreamData)
		LOG_FUNC_ARG(Stride)
		LOG_FUNC_END;

	
	// Forward to Xbox implementation
	// This should stop us having to patch GetStreamSource!
	XB_TRMP(D3DDevice_SetStreamSource)(StreamNumber, pStreamData, Stride);

	//CxbxrImpl_SetStreamSource(StreamNumber, pStreamData, Stride);

	//EMUPATCH(D3DDevice_SetShaderConstantMode)(Mode);
	//CxbxrImpl_SetShaderConstantMode(Mode);
	//1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)StreamNumber;
	PBTokenArray[3] = (DWORD)pStreamData;
	PBTokenArray[4] = (DWORD)Stride;
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetStreamSource, 3);
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShader
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetVertexShader)
(
    dword_xt Handle
)
{
	LOG_FUNC_ONE_ARG(Handle);

	// This trampoline leads to calling D3DDevice_LoadVertexShader and D3DDevice_SelectVertexShader
	// Please raise the alarm if this is ever not the case
	XB_TRMP(D3DDevice_SetVertexShader)(Handle);

	//init g_pXbox_pPush and g_pXbox_pPushLimit since SetVertexSahder is always called inside CreateDevice().
	if (g_pXbox_pPush == nullptr) {
		g_pXbox_pPush = (xbox::dword_xt**)*g_pXbox_D3DDevice;
		g_pXbox_pPushLimit = g_pXbox_pPush + 1;
	}
	else {
		CxbxrImpl_SetVertexShader(Handle);
	}
#if 0
#if !USEPGRAPH_SetVertexShader

	CxbxrImpl_SetVertexShader(Handle);
#else	
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)Handle;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetVertexShader, 1);//argCount, not necessary, default to 14
#endif
#endif
}

// Overload for logging
static void D3DDevice_SetVertexShader_0
(
    xbox::dword_xt Handle
)
{
	LOG_FUNC_ONE_ARG(Handle);
}

// This uses a custom calling convention where Handle is passed in EBX
// Test-case: NASCAR Heat 2002
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetVertexShader_0)()
{
	dword_xt Handle;
	__asm {
		LTCG_PROLOGUE
		mov  Handle, ebx
	}

	// Log
	D3DDevice_SetVertexShader_0(Handle);

	// This trampoline leads to calling D3DDevice_LoadVertexShader and D3DDevice_SelectVertexShader
	// Please raise the alarm if this is ever not the case
	__asm {
		mov  ebx, Handle
		call XB_TRMP(D3DDevice_SetVertexShader_0)
	}

	//init g_pXbox_pPush and g_pXbox_pPushLimit since SetVertexSahder is always called inside CreateDevice().
	if (g_pXbox_pPush == nullptr) {
		g_pXbox_pPush = (xbox::dword_xt**)*g_pXbox_D3DDevice;
		g_pXbox_pPushLimit = g_pXbox_pPush + 1;
	}
	else {
		CxbxrImpl_SetVertexShader(Handle);
	}
#if 0
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)Handle;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetVertexShader, 1);//argCount, not necessary, default to 14
#endif
#if !USEPGRAPH_SetVertexShader
	//CxbxrImpl_SetVertexShader(Handle);
#endif

	__asm {
		LTCG_EPILOGUE
		ret
	}
}

// TODO : Move to own file
constexpr unsigned int IndicesPerPage = PAGE_SIZE / sizeof(INDEX16);
constexpr unsigned int InputQuadsPerPage = ((IndicesPerPage) / VERTICES_PER_TRIANGLE) / TRIANGLES_PER_QUAD;// was ((IndicesPerPage * VERTICES_PER_QUAD) / VERTICES_PER_TRIANGLE) / TRIANGLES_PER_QUAD;

// TODO : Move to own file
// Called by CxbxDrawPrimitiveUP (indirectly by D3DDevice_DrawVerticesUP,
// EmuExecutePushBufferRaw and CxbxrImpl_End) when PrimitiveType == X_D3DPT_QUADLIST.
// Emulated by calling g_pD3DDevice->DrawIndexedPrimitiveUP with index data that maps
// quads to triangles. This function creates the index buffer that is needed for this;
// For every quad that must be drawn, we generate indices for two triangles.
// Note, that the resulting index data can be re-used for later comparable draw calls
// and only needs to grow when  current length doesn't suffices for a larger draw.
INDEX16 *CxbxAssureQuadListIndexData(UINT NrOfQuadIndices)
{
	NrOfQuadIndices = (NrOfQuadIndices * 3 /2) ;//we're passing in the xbox vertex count, which minus 2 becomes quad count, multiply 3 becomes triangle list index count
	if (g_QuadListToTriangleIndexData_Size < NrOfQuadIndices)
	{
		g_QuadListToTriangleIndexData_Size = RoundUp(NrOfQuadIndices, InputQuadsPerPage);
		UINT NrOfTriangleIndices = NrOfQuadIndices;// was QuadToTriangleVertexCount(g_QuadListToTriangleIndexData_Size);
		if (g_pQuadListToTriangleIndexData != nullptr) {
			free(g_pQuadListToTriangleIndexData);
		}

		g_pQuadListToTriangleIndexData = (INDEX16 *)malloc(g_QuadListToTriangleIndexData_Size * sizeof(INDEX16));
		CxbxConvertQuadListToTriangleListIndices(nullptr, g_QuadListToTriangleIndexData_Size, g_pQuadListToTriangleIndexData);
	}

	return g_pQuadListToTriangleIndexData;
}

// this is for indexed buffer convertion.
INDEX16* CxbxConvertQuadListIndexData(CxbxDrawContext& DrawContext)
{
	int NrOfQuadIndices = DrawContext.dwVertexCount;
	NrOfQuadIndices = (NrOfQuadIndices * 3)/2;//we're passing in the xbox indexed count, which devided by 4 becomes quad count, multiply 2 becomes triangle count, then multiply 3 becomes triangle list index count.
	if (g_QuadListToTriangleIndexData_Size < NrOfQuadIndices)
	{
		g_QuadListToTriangleIndexData_Size = RoundUp(NrOfQuadIndices, InputQuadsPerPage);
		UINT NrOfTriangleIndices = NrOfQuadIndices;// was QuadToTriangleVertexCount(g_QuadListToTriangleIndexData_Size);
		if (g_pQuadListToTriangleIndexData != nullptr) {
			free(g_pQuadListToTriangleIndexData);
		}

		g_pQuadListToTriangleIndexData = (INDEX16*)malloc(g_QuadListToTriangleIndexData_Size * sizeof(INDEX16));
		CxbxConvertQuadListToTriangleListIndices(DrawContext.pXboxIndexData, g_QuadListToTriangleIndexData_Size, g_pQuadListToTriangleIndexData);
	}

	return g_pQuadListToTriangleIndexData;
}

// TODO : Move to own file
// Called by CxbxDrawPrimitiveUP (indirectly by D3DDevice_DrawVerticesUP,
// EmuExecutePushBufferRaw and CxbxrImpl_End) when PrimitiveType == X_D3DPT_QUADLIST.
// Emulated by calling g_pD3DDevice->DrawIndexedPrimitiveUP with index data that maps
// quads to triangles. This function creates the index buffer that is needed for this;
// For every quad that must be drawn, we generate indices for two triangles.
// Note, that the resulting index data can be re-used for later comparable draw calls
// and only needs to grow when  current length doesn't suffices for a larger draw.

INDEX16 *CxbxAssureQuadStripIndexData(UINT NrOfQuadIndices)
{
	//we're passing in the xbox vertex count, which minus 2 then deviced by 2 becomes quad count, then multiply 2 becomes triangle list count, then multiply 3 becomes triangle list index count, add 6 more index space for CxbxConvertQuadStripToTriangleListIndices()
	NrOfQuadIndices = (NrOfQuadIndices - 2) * 3 + 6;
	if (g_QuadStripToTriangleIndexData_Size < NrOfQuadIndices)
	{

		g_QuadListToTriangleIndexData_Size = RoundUp(NrOfQuadIndices, InputQuadsPerPage);
		UINT NrOfTriangleIndices = NrOfQuadIndices;// was QuadToTriangleVertexCount(g_QuadListToTriangleIndexData_Size);
		if (g_pQuadStripToTriangleIndexData != nullptr) {
			free(g_pQuadStripToTriangleIndexData);
		}

		g_pQuadStripToTriangleIndexData = (INDEX16 *)malloc(g_QuadListToTriangleIndexData_Size * sizeof(INDEX16));
		CxbxConvertQuadStripToTriangleListIndices(nullptr, g_QuadListToTriangleIndexData_Size, g_pQuadStripToTriangleIndexData);
	}

	return g_pQuadStripToTriangleIndexData;
}

INDEX16* CxbxConvertQuadStripIndexData(CxbxDrawContext& DrawContext)
{
	//we're passing in the xbox vertex count, which minus 2 then deviced by 2 becomes quad count, then multiply 2 becomes triangle list count, then multiply 3 becomes triangle list index count, add 6 more index space for CxbxConvertQuadStripToTriangleListIndices()
	int NrOfQuadIndices = DrawContext.dwVertexCount;
	NrOfQuadIndices = (NrOfQuadIndices - 2) * 3 + 6;
	if (g_QuadStripToTriangleIndexData_Size < NrOfQuadIndices)
	{

		g_QuadListToTriangleIndexData_Size = RoundUp(NrOfQuadIndices, InputQuadsPerPage);
		UINT NrOfTriangleIndices = NrOfQuadIndices;// was QuadToTriangleVertexCount(g_QuadListToTriangleIndexData_Size);
		if (g_pQuadStripToTriangleIndexData != nullptr) {
			free(g_pQuadStripToTriangleIndexData);
		}

		g_pQuadStripToTriangleIndexData = (INDEX16*)malloc(g_QuadListToTriangleIndexData_Size * sizeof(INDEX16));
		CxbxConvertQuadStripToTriangleListIndices(DrawContext.pXboxIndexData, g_QuadListToTriangleIndexData_Size, g_pQuadStripToTriangleIndexData);
	}

	return g_pQuadStripToTriangleIndexData;
}
// TODO : Move to own file
// Makes a D3D IndexBuffer active that contains quadlist-to-trianglelist indices.
// Uses CxbxAssureQuadListIndexData to populate the index buffer with.
// Note, that the resulting index buffer can be re-used for later comparable draw calls
// and only needs to grow when current length doesn't sufficesw for a larger draw.
void CxbxAssureQuadListD3DIndexBuffer(UINT NrOfQuadIndices)
{
	LOG_INIT // Allows use of DEBUG_D3DRESULT

	HRESULT hRet;

	if (g_QuadToTriangleHostIndexBuffer_Size < NrOfQuadIndices)
	{
		// Round the number of indices up so we'll allocate whole pages
		g_QuadToTriangleHostIndexBuffer_Size = RoundUp(NrOfQuadIndices, InputQuadsPerPage);
		UINT NrOfTriangleIndices = QuadListToTriangleListVertexCount(g_QuadToTriangleHostIndexBuffer_Size); // 4 > 6

		// Create a new native index buffer of the above determined size :
		if (g_pQuadToTriangleHostIndexBuffer != nullptr) {
			g_pQuadToTriangleHostIndexBuffer->Release(); // test-case : XDK PointSprites
			g_pQuadToTriangleHostIndexBuffer = nullptr;
		}

		// Create a new native index buffer of the above determined size :
		g_pQuadToTriangleHostIndexBuffer = CxbxCreateIndexBuffer(NrOfTriangleIndices);
		if (g_pQuadToTriangleHostIndexBuffer == nullptr)
			CxbxrAbort("CxbxAssureQuadListD3DIndexBuffer : IndexBuffer Create Failed!");

		// Put quadlist-to-triangle-list index mappings into this buffer :
		INDEX16* pHostIndexBufferData = nullptr;
		hRet = g_pQuadToTriangleHostIndexBuffer->Lock(0, /*entire SizeToLock=*/0, (D3DLockData **)&pHostIndexBufferData, D3DLOCK_DISCARD);
		DEBUG_D3DRESULT(hRet, "g_pQuadToTriangleHostIndexBuffer->Lock");
		if (pHostIndexBufferData == nullptr)
			CxbxrAbort("CxbxAssureQuadListD3DIndexBuffer : Could not lock index buffer!");

		memcpy(pHostIndexBufferData, CxbxAssureQuadListIndexData(NrOfQuadIndices), NrOfTriangleIndices * sizeof(INDEX16));

		g_pQuadToTriangleHostIndexBuffer->Unlock();
	}

	// Activate the new native index buffer :
	hRet = g_pD3DDevice->SetIndices(g_pQuadToTriangleHostIndexBuffer);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetIndices");

	if (FAILED(hRet))
		CxbxrAbort("CxbxAssureQuadListD3DIndexBuffer : SetIndices Failed!"); // +DxbxD3DErrorString(hRet));
}

// TODO : Move to own file
// Calls SetIndices with a separate index-buffer, that's populated with the supplied indices.
void CxbxDrawIndexedClosingLine(INDEX16 LowIndex, INDEX16 HighIndex)
{
	LOG_INIT // Allows use of DEBUG_D3DRESULT

	HRESULT hRet;

	if (g_pClosingLineLoopHostIndexBuffer == nullptr) {
		g_pClosingLineLoopHostIndexBuffer = CxbxCreateIndexBuffer(VERTICES_PER_LINE);
		if (g_pClosingLineLoopHostIndexBuffer == nullptr)
			CxbxrAbort("Unable to create g_pClosingLineLoopHostIndexBuffer for D3DPT_LINELOOP emulation");
	}

	INDEX16 *pCxbxClosingLineLoopIndexBufferData = nullptr;
	hRet = g_pClosingLineLoopHostIndexBuffer->Lock(0, /*entire SizeToLock=*/0, (D3DLockData **)&pCxbxClosingLineLoopIndexBufferData, D3DLOCK_DISCARD);
	DEBUG_D3DRESULT(hRet, "g_pClosingLineLoopHostIndexBuffer->Lock");

	// Set the indices for the two VERTICES_PER_LINE :
	pCxbxClosingLineLoopIndexBufferData[0] = LowIndex;
	pCxbxClosingLineLoopIndexBufferData[1] = HighIndex;

	hRet = g_pClosingLineLoopHostIndexBuffer->Unlock();
	DEBUG_D3DRESULT(hRet, "g_pClosingLineLoopHostIndexBuffer->Unlock");

	hRet = g_pD3DDevice->SetIndices(g_pClosingLineLoopHostIndexBuffer);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetIndices");

	hRet = g_pD3DDevice->DrawIndexedPrimitive(
		/*PrimitiveType=*/D3DPT_LINELIST,
		/*BaseVertexIndex=*/0, // Note : Callers must apply BaseVertexIndex to the LowIndex and HighIndex argument values
		/*MinVertexIndex=*/LowIndex,
		/*NumVertices=*/VERTICES_PER_LINE,
		/*startIndex=*/0,
		/*primCount=*/1
	);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitive(CxbxDrawIndexedClosingLine)");

	g_dwPrimPerFrame++;
}

// TODO : Move to own file
void CxbxDrawIndexedClosingLineUP(INDEX16 LowIndex, INDEX16 HighIndex, void *pHostVertexStreamZeroData, UINT uiHostVertexStreamZeroStride)
{
	LOG_INIT // Allows use of DEBUG_D3DRESULT

#if 0
	// Since we can use pHostVertexStreamZeroData here, we can close the line simpler than
	// via CxbxDrawIndexedClosingLine, by drawing two indices via DrawIndexedPrimitiveUP.
	// (This is simpler because we use just indices and don't need to copy the vertices.)
	INDEX16 CxbxClosingLineIndices[2] = { LowIndex, HighIndex };

	HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitiveUP(
		/*PrimitiveType=*/D3DPT_LINELIST,
		/*MinVertexIndex=*/LowIndex,
		/*NumVertices=*/(HighIndex - LowIndex) + 1,
		/*PrimitiveCount=*/1,
		/*pIndexData=*/CxbxClosingLineIndices,
		/*IndexDataFormat=*/D3DFMT_INDEX16,
		pHostVertexStreamZeroData,
		uiHostVertexStreamZeroStride
	);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitiveUP(CxbxDrawIndexedClosingLineUP)");
#else // TODO : If NumVertices is high, performance might suffer - drawing a copy of the two vertices could be faster
	// Since we can use pHostVertexStreamZeroData here, we can close the line simpler than
	// via CxbxDrawIndexedClosingLine, by drawing two vertices via DrawPrimitiveUP.
	// (This is simpler because we just copy the vertices, and don't need a separate index buffer.)
	uint8_t VertexData[512]; assert(512 >= 2 * uiHostVertexStreamZeroStride);
	uint8_t *FirstVertex = (uint8_t *)pHostVertexStreamZeroData + (LowIndex * uiHostVertexStreamZeroStride);
	uint8_t *SecondVertex = (uint8_t *)pHostVertexStreamZeroData + (HighIndex * uiHostVertexStreamZeroStride);

	memcpy(VertexData, FirstVertex, uiHostVertexStreamZeroStride);
	memcpy(VertexData + uiHostVertexStreamZeroStride, SecondVertex, uiHostVertexStreamZeroStride);

	HRESULT hRet = g_pD3DDevice->DrawPrimitiveUP(
		/*PrimitiveType=*/D3DPT_LINELIST,
		/*PrimitiveCount=*/1,
		/*pVertexStreamZeroData=*/VertexData,
		uiHostVertexStreamZeroStride
	);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawPrimitiveUP(CxbxDrawIndexedClosingLineUP)");
#endif

	g_dwPrimPerFrame++;
}

// Requires assigned pXboxIndexData
// Called by D3DDevice_DrawIndexedVertices and EmuExecutePushBufferRaw (twice)
void CxbxDrawIndexed(CxbxDrawContext& DrawContext)
{
	LOG_INIT // Allows use of DEBUG_D3DRESULT

	//assert(DrawContext.dwStartVertex == 0);
	assert(DrawContext.pXboxIndexData != nullptr);
	assert(DrawContext.dwVertexCount > 0); // TODO : If this fails, make responsible callers do an early-exit
	convertIndexBuffer convertIndexBuffer=passthrough;
	if (DrawContext.XboxPrimitiveType == xbox::X_D3DPT_QUADLIST) {
		convertIndexBuffer = convertQuadList;
	}
	/* // quad strip doesn't require and convertion. only convert the primitive type from quad strip to triangle strip.
	else if (DrawContext.XboxPrimitiveType == xbox::X_D3DPT_QUADSTRIP){
		convertIndexBuffer = convertQuadStrip;
    }
	*/
	ConvertedIndexBuffer& CacheEntry = CxbxUpdateActiveIndexBuffer(DrawContext.pXboxIndexData, DrawContext.dwVertexCount, convertIndexBuffer);
	// Set LowIndex and HighIndex *before* VerticesInBuffer gets derived
	DrawContext.LowIndex = CacheEntry.LowIndex;
	DrawContext.HighIndex = CacheEntry.HighIndex;
	// Note : CxbxUpdateActiveIndexBuffer calls SetIndices


	VertexBufferConverter.Apply(&DrawContext); // Sets dwHostPrimitiveCount
	
	INT BaseVertexIndex = DrawContext.dwBaseVertexIndex;
	UINT primCount = DrawContext.dwHostPrimitiveCount;

	// See https://docs.microsoft.com/en-us/windows/win32/direct3d9/rendering-from-vertex-and-index-buffers
	// for an explanation on the function of the BaseVertexIndex, MinVertexIndex, NumVertices and StartIndex arguments.
	HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitive(
		/* PrimitiveType = */EmuXB2PC_D3DPrimitiveType(DrawContext.XboxPrimitiveType),
		/* BaseVertexIndex, = */-DrawContext.LowIndex, // Base vertex index and LowIndex has been accounted for in the stream/vertex buffer conversion, now we need to "un-offset" the index buffer
		/* MinVertexIndex = */DrawContext.LowIndex,
		/* NumVertices = */(DrawContext.HighIndex - DrawContext.LowIndex) + 1,
		/* startIndex = DrawContext.dwStartVertex = */0,
		primCount);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitive");

	g_dwPrimPerFrame += primCount;
	if (DrawContext.XboxPrimitiveType == xbox::X_D3DPT_LINELOOP) {
		// Close line-loops using a final single line, drawn from the end to the start vertex
		if (BaseVertexIndex == 0) {
			LOG_TEST_CASE("X_D3DPT_LINELOOP");
		} else {
			LOG_TEST_CASE("X_D3DPT_LINELOOP (BaseVertexIndex > 0)");
			// Note : This is to find test-cases for the BaseVertexIndex addition below:
		}
		// Read the end and start index from the supplied index data
		INDEX16 LowIndex = BaseVertexIndex + DrawContext.pXboxIndexData[0];
		INDEX16 HighIndex = BaseVertexIndex + DrawContext.pXboxIndexData[DrawContext.dwHostPrimitiveCount];
		// If needed, swap so highest index is higher than lowest (duh)
		if (HighIndex < LowIndex) {
			std::swap(HighIndex, LowIndex);
		}

		// Draw the closing line using a helper function (which will SetIndices)
		CxbxDrawIndexedClosingLine(LowIndex, HighIndex);
		// NOTE : We don't restore the previously active index buffer
	}
}
// TODO : Move to own file
// Drawing function specifically for rendering Xbox draw calls supplying a 'User Pointer'.
// Called by D3DDevice_DrawVerticesUP, EmuExecutePushBufferRaw and CxbxrImpl_End
void CxbxDrawIndexedPrimitiveUP(CxbxDrawContext& DrawContext)
{
	LOG_INIT // Allows use of DEBUG_D3DRESULT

		assert(DrawContext.dwStartVertex == 0);
	assert(DrawContext.pXboxVertexStreamZeroData != xbox::zeroptr);
	assert(DrawContext.uiXboxVertexStreamZeroStride > 0);
	assert(DrawContext.dwBaseVertexIndex == 0); // No IndexBase under Draw*UP
	// in DrawIndexedPrimitiveUP(), the XboxIndexBuffer isn't nullptr, so LowIndex and HighIndex will be updated per the conten of XboxIndexBuffer.
	// the XboxZeroStream isn't nullptr and will not be adjusted via LowIndex.
	VertexBufferConverter.Apply(&DrawContext);
	//convert dwHostPrimitiveCount here. the quad to d3d9 triangle is considered already.
	assert(DrawContext.XboxPrimitiveType != xbox::X_D3DPT_POLYGON);
	//DrawContext.dwHostPrimitiveCount is set by VertexBufferConverter.Apply()
	//DrawContext.dwHostPrimitiveCount = EmuXB2PC_D3DPrimitiveCount(DrawContext.dwVertexCount,DrawContext.XboxPrimitiveType);
	if (DrawContext.XboxPrimitiveType == xbox::X_D3DPT_QUADLIST) {
		// LOG_TEST_CASE("X_D3DPT_QUADLIST"); // test-case : X-Marbles and XDK Sample PlayField
		// Draw quadlists using a single 'quad-to-triangle mapping' index buffer :
		INDEX16* pIndexData = CxbxConvertQuadListIndexData(DrawContext);
		//DrawContext.dwVertexCount = DrawContext.dwHostPrimitiveCount * 3;
		//DrawContext.NumVerticesToUse = DrawContext.dwVertexCount;
		// Convert quad vertex-count to triangle vertex count :
		//UINT PrimitiveCount = DrawContext.dwHostPrimitiveCount * TRIANGLES_PER_QUAD;

		// Draw indexed triangles instead of quads
		HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitiveUP(
			/*PrimitiveType=*/D3DPT_TRIANGLELIST,
			/*MinVertexIndex=*/DrawContext.LowIndex, // should be min. index since the source is indexed buffer
			/*NumVertices=*/DrawContext.HighIndex - DrawContext.LowIndex + 1,//NumVerticesToUse, // this is the actual vertex range within vertex buffer, starting from base vertex index + MinVertexIndex to base vertex index + MaxVertexIndex.
			DrawContext.dwHostPrimitiveCount,
			pIndexData,
			/*IndexDataFormat=*/D3DFMT_INDEX16,
			DrawContext.pHostVertexStreamZeroData,
			DrawContext.uiHostVertexStreamZeroStride
		);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitieUP(X_D3DPT_QUADLIST)");

		g_dwPrimPerFrame += DrawContext.dwHostPrimitiveCount;
	}
#if 0
	else if (DrawContext.XboxPrimitiveType == xbox::X_D3DPT_QUADSTRIP) {
		// LOG_TEST_CASE("X_D3DPT_QUADSTRIP"); // test-case : GUN
		// Draw quadstrips using a single 'quadstrip-to-triangle mapping' index buffer :
		INDEX16* pIndexData = CxbxConvertQuadStripIndexData(DrawContext);
		//DrawContext.dwVertexCount = DrawContext.dwHostPrimitiveCount * 3;
		//DrawContext.NumVerticesToUse = DrawContext.dwVertexCount;
		// Convert quad vertex-count to triangle vertex count :
		//UINT PrimitiveCount = DrawContext.dwHostPrimitiveCount * TRIANGLES_PER_QUAD;
    */
		// Draw indexed triangles instead of quads
		HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitiveUP(
			/*PrimitiveType=*/D3DPT_TRIANGLELIST,
			/*MinVertexIndex=*/DrawContext.LowIndex, // should be min. index since the source is indexed buffer
			/*NumVertices=*/DrawContext.HighIndex - DrawContext.LowIndex + 1,//NumVerticesToUse,
			DrawContext.dwHostPrimitiveCount,
			pIndexData,
			/*IndexDataFormat=*/D3DFMT_INDEX16,
			DrawContext.pHostVertexStreamZeroData,
			DrawContext.uiHostVertexStreamZeroStride
		);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitieUP(X_D3DPT_QUADSTRIP)");

		g_dwPrimPerFrame += DrawContext.dwHostPrimitiveCount;

	}
#endif    
	else {
		// Primitives other than X_D3DPT_QUADLIST can be drawn using one DrawPrimitiveUP call :
		//?? for xbox::X_D3DPT_LINELOOP, we convert it to d3d9 line strip, but that will require an additional repeated vertex of vertex 0 to be appended in the last.
		//and also update the DrawContext.dwVertexCount and DrawContext.dwHostPrimitiveCount. this needs further verification.


		// Draw indexed triangles instead of quads
		HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitiveUP(
			/*PrimitiveType=*/EmuXB2PC_D3DPrimitiveType(DrawContext.XboxPrimitiveType),
			/*MinVertexIndex=*/DrawContext.LowIndex, // should be min. index since the source is indexed buffer
			/*NumVertices=*/DrawContext.HighIndex- DrawContext.LowIndex +1 ,//NumVerticesToUse,
			DrawContext.dwHostPrimitiveCount,
			DrawContext.pXboxIndexData,
			/*IndexDataFormat=*/D3DFMT_INDEX16,
			DrawContext.pHostVertexStreamZeroData,
			DrawContext.uiHostVertexStreamZeroStride
		);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitieUP()");

		g_dwPrimPerFrame += DrawContext.dwHostPrimitiveCount;
		if (DrawContext.XboxPrimitiveType == xbox::X_D3DPT_LINELOOP) {
			// test-case : XDK samples reaching this case : DebugKeyboard, Gamepad, Tiling, ShadowBuffer
			// Close line-loops using a final single line, drawn from the end to the start vertex :
			CxbxDrawIndexedClosingLineUP(
				(INDEX16)0, // LowIndex
				(INDEX16)DrawContext.dwHostPrimitiveCount, // HighIndex,
				DrawContext.pHostVertexStreamZeroData,
				DrawContext.uiHostVertexStreamZeroStride
			);
			INDEX16 closeLineIndex[2];
			closeLineIndex[0] = DrawContext.pXboxIndexData[0];
			closeLineIndex[1] = DrawContext.pXboxIndexData[DrawContext.dwVertexCount -1];
			HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitiveUP(
				/*PrimitiveType=*/EmuXB2PC_D3DPrimitiveType(xbox::X_D3DPT_LINELIST),
				/*MinVertexIndex=*/0, // min. index is 0.
				/*NumVertices=*/2,
				/*host primitive count*/1,
				/*index buffer*/closeLineIndex,
				/*IndexDataFormat=*/D3DFMT_INDEX16,
				DrawContext.pHostVertexStreamZeroData,
				DrawContext.uiHostVertexStreamZeroStride
			);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitieUP()");
		}

	}
}
// TODO : Move to own file
// Drawing function specifically for rendering Xbox draw calls supplying a 'User Pointer'.
// Called by D3DDevice_DrawVerticesUP, EmuExecutePushBufferRaw and CxbxrImpl_End
void CxbxDrawPrimitiveUP(CxbxDrawContext &DrawContext)
{
	LOG_INIT // Allows use of DEBUG_D3DRESULT

	assert(DrawContext.dwStartVertex == 0);
	assert(DrawContext.pXboxVertexStreamZeroData != xbox::zeroptr);
	assert(DrawContext.uiXboxVertexStreamZeroStride > 0);
	assert(DrawContext.dwBaseVertexIndex == 0); // No IndexBase under Draw*UP

	VertexBufferConverter.Apply(&DrawContext);
	//convert dwHostPrimitiveCount here. the quad to d3d9 triangle is considered already.
	assert(DrawContext.XboxPrimitiveType != xbox::X_D3DPT_POLYGON);
	//DrawContext.dwHostPrimitiveCount is set by VertexBufferConverter.Apply()
	//DrawContext.dwHostPrimitiveCount = EmuXB2PC_D3DPrimitiveCount(DrawContext.dwVertexCount,DrawContext.XboxPrimitiveType);
	if (DrawContext.XboxPrimitiveType == xbox::X_D3DPT_QUADLIST) {
		// LOG_TEST_CASE("X_D3DPT_QUADLIST"); // test-case : X-Marbles and XDK Sample PlayField
		// Draw quadlists using a single 'quad-to-triangle mapping' index buffer :
		INDEX16 *pIndexData = CxbxAssureQuadListIndexData(DrawContext.dwVertexCount);
		//DrawContext.dwVertexCount = DrawContext.dwHostPrimitiveCount * 3;
		//DrawContext.NumVerticesToUse = DrawContext.dwVertexCount;
		// Convert quad vertex-count to triangle vertex count :
		//UINT PrimitiveCount = DrawContext.dwHostPrimitiveCount * TRIANGLES_PER_QUAD;

		// Draw indexed triangles instead of quads
		HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitiveUP(
			/*PrimitiveType=*/D3DPT_TRIANGLELIST,
			/*MinVertexIndex=*/0, // Always 0 for converted quadlist data
			/*NumVertices=*/DrawContext.dwVertexCount,
			DrawContext.dwHostPrimitiveCount,
			pIndexData,
			/*IndexDataFormat=*/D3DFMT_INDEX16,
			DrawContext.pHostVertexStreamZeroData,
			DrawContext.uiHostVertexStreamZeroStride
		);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitieUP(X_D3DPT_QUADLIST)");

		g_dwPrimPerFrame += DrawContext.dwHostPrimitiveCount;
	}
#if 0
	else if (DrawContext.XboxPrimitiveType == xbox::X_D3DPT_QUADSTRIP) {
		// LOG_TEST_CASE("X_D3DPT_QUADSTRIP"); // test-case : GUN
		// NV2A uses OpenGL quad strip which uses the same vertex order as d3d triangle strip.
		// ref. http://edeleastar.github.io/opengl-programming/topic06/pdf/1.Polygons.pdf
		// https://learn.microsoft.com/en-us/windows/win32/direct3d9/triangle-strips
		// so we simply convert quad strip to triangle strip and use the same vertex count.
		// since this code section is identical to the default draw call, we could simply discard this section and use the default draw call. 

		HRESULT hRet = g_pD3DDevice->DrawPrimitiveUP(
			EmuXB2PC_D3DPrimitiveType(DrawContext.XboxPrimitiveType),
			DrawContext.dwHostPrimitiveCount,
			DrawContext.pHostVertexStreamZeroData,
			DrawContext.uiHostVertexStreamZeroStride
		);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitieUP(X_D3DPT_QUADLIST)");

		g_dwPrimPerFrame += DrawContext.dwHostPrimitiveCount;

	}
#endif
	else {
		// Primitives other than X_D3DPT_QUADLIST can be drawn using one DrawPrimitiveUP call :
		//?? for xbox::X_D3DPT_LINELOOP, we convert it to d3d9 line strip, but that will require an additional repeated vertex of vertex 0 to be appended in the last.
		//and also update the DrawContext.dwVertexCount and DrawContext.dwHostPrimitiveCount. this needs further verification.

		HRESULT hRet = g_pD3DDevice->DrawPrimitiveUP(
			EmuXB2PC_D3DPrimitiveType(DrawContext.XboxPrimitiveType),
			DrawContext.dwHostPrimitiveCount,
			DrawContext.pHostVertexStreamZeroData,
			DrawContext.uiHostVertexStreamZeroStride
		);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawPrimitiveUP");

		g_dwPrimPerFrame += DrawContext.dwHostPrimitiveCount;
		if (DrawContext.XboxPrimitiveType == xbox::X_D3DPT_LINELOOP) {
			// test-case : XDK samples reaching this case : DebugKeyboard, Gamepad, Tiling, ShadowBuffer
			// Close line-loops using a final single line, drawn from the end to the start vertex :
			CxbxDrawIndexedClosingLineUP(
				(INDEX16)0, // LowIndex
				(INDEX16)DrawContext.dwHostPrimitiveCount, // HighIndex,
				DrawContext.pHostVertexStreamZeroData,
				DrawContext.uiHostVertexStreamZeroStride
			);
		}
	}
}
// TODO : Move to own file
// Drawing function specifically for rendering Xbox draw calls supplying a 'User Pointer'.
// Called by D3DDevice_DrawVerticesUP, EmuExecutePushBufferRaw and CxbxrImpl_End
void CxbxDrawPrimitive(CxbxDrawContext& DrawContext)
{
	LOG_INIT // Allows use of DEBUG_D3DRESULT

		//assert(DrawContext.dwStartVertex == 0);
	//assert(DrawContext.pXboxVertexStreamZeroData != xbox::zeroptr);
	assert(DrawContext.uiXboxVertexStreamZeroStride > 0);
	assert(DrawContext.dwBaseVertexIndex == 0); // No IndexBase under Draw*UP
	// the StartVertex was taking account in Apply() which each vertex buffer's offset was adjust to the starting offset of StartingVertex.
	// also sets DrawContext.dwHostPrimitiveCount=EmuXB2PC_D3DPrimitiveCount()
	VertexBufferConverter.Apply(&DrawContext);
	//convert dwHostPrimitiveCount here. the quad to d3d9 triangle is considered already.
	assert(DrawContext.XboxPrimitiveType != xbox::X_D3DPT_POLYGON);
	//DrawContext.dwHostPrimitiveCount is set by VertexBufferConverter.Apply()
	//DrawContext.dwHostPrimitiveCount = EmuXB2PC_D3DPrimitiveCount(DrawContext.dwVertexCount,DrawContext.XboxPrimitiveType);
	if (DrawContext.XboxPrimitiveType == xbox::X_D3DPT_QUADLIST) {
		// LOG_TEST_CASE("X_D3DPT_QUADLIST"); // test-case : X-Marbles and XDK Sample PlayField
		// Draw quadlists using a single 'quad-to-triangle mapping' index buffer :
		INDEX16* pIndexData = CxbxAssureQuadListIndexData(DrawContext.dwVertexCount);
		// VertexBufferConverter.Apply(&DrawContext) already convert xbox quad list to HostPrimitive triangle list primitive count.
		//DrawContext.dwVertexCount = DrawContext.dwHostPrimitiveCount * 3;
		//DrawContext.NumVerticesToUse = DrawContext.dwVertexCount;
		//we're only using this function call to setup host index buffer.
		ConvertedIndexBuffer& CacheEntry = CxbxUpdateActiveIndexBuffer(pIndexData, DrawContext.dwHostPrimitiveCount * 3, convertIndexBuffer::passthrough);

		// Note : CxbxUpdateActiveIndexBuffer calls SetIndices

		// Set LowIndex and HighIndex *before* VerticesInBuffer gets derived
		DrawContext.LowIndex = CacheEntry.LowIndex;
		DrawContext.HighIndex = CacheEntry.HighIndex;

		// Convert quad vertex-count to triangle vertex count :
		//UINT PrimitiveCount = DrawContext.dwHostPrimitiveCount * TRIANGLES_PER_QUAD;

		// Draw indexed triangles instead of quads
		HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitive(
			/*PrimitiveType=*/D3DPT_TRIANGLELIST,
			/* BaseVertexIndex, = */0, // Base vertex index has been accounted for in the stream conversion, now we need to "un-offset" the index buffer
			/* MinVertexIndex = */0,
			/* NumVertices = */DrawContext.dwVertexCount,
			/* startIndex = DrawContext.dwStartVertex = */0,
			DrawContext.dwHostPrimitiveCount);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitieUP(X_D3DPT_QUADLIST)");

		g_dwPrimPerFrame += DrawContext.dwHostPrimitiveCount;
	}
#if 0
	else if (DrawContext.XboxPrimitiveType == xbox::X_D3DPT_QUADSTRIP) {
		// LOG_TEST_CASE("X_D3DPT_QUADSTRIP"); // test-case : GUN
		// Draw quadstrips using a single 'quadstrip-to-triangle mapping' index buffer :
		INDEX16* pIndexData = CxbxAssureQuadStripIndexData(DrawContext.dwVertexCount);
		//DrawContext.dwVertexCount = DrawContext.dwHostPrimitiveCount * 3;
		//DrawContext.NumVerticesToUse = DrawContext.dwVertexCount;

		//we're only using this function call to setup host index buffer.
		ConvertedIndexBuffer& CacheEntry = CxbxUpdateActiveIndexBuffer(pIndexData, DrawContext.dwHostPrimitiveCount * 3, convertIndexBuffer::passthrough);
		// Note : CxbxUpdateActiveIndexBuffer calls SetIndices

        // Set LowIndex and HighIndex *before* VerticesInBuffer gets derived
		DrawContext.LowIndex = CacheEntry.LowIndex;
		DrawContext.HighIndex = CacheEntry.HighIndex;

		// Convert quad vertex-count to triangle vertex count :
		//UINT PrimitiveCount = DrawContext.dwHostPrimitiveCount * TRIANGLES_PER_QUAD;

		// Draw indexed triangles instead of quads
		HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitive(
			/*PrimitiveType=*/D3DPT_TRIANGLELIST,
			/* BaseVertexIndex, = */0, // Base vertex index has been accounted for in the stream conversion, now we need to "un-offset" the index buffer
			/* MinVertexIndex = */0,
			/* NumVertices = */DrawContext.dwVertexCount,
			/* startIndex = DrawContext.dwStartVertex = */0,
			DrawContext.dwHostPrimitiveCount);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitieUP(X_D3DPT_QUADLIST)");

		g_dwPrimPerFrame += DrawContext.dwHostPrimitiveCount;

	}
#endif
	else {
		// Primitives other than X_D3DPT_QUADLIST can be drawn using one DrawPrimitiveUP call :
		//?? for xbox::X_D3DPT_LINELOOP, we convert it to d3d9 line strip, but that will require an additional repeated vertex of vertex 0 to be appended in the last.
		//and also update the DrawContext.dwVertexCount and DrawContext.dwHostPrimitiveCount. this needs further verification.

		HRESULT hRet = g_pD3DDevice->DrawPrimitive(
			EmuXB2PC_D3DPrimitiveType(DrawContext.XboxPrimitiveType),
			/*StartVertex*/0,
			/*PrimitiveCount*/DrawContext.dwHostPrimitiveCount
		);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawPrimitiveUP");

		g_dwPrimPerFrame += DrawContext.dwHostPrimitiveCount;
		if (DrawContext.XboxPrimitiveType == xbox::X_D3DPT_LINELOOP) {
			// test-case : XDK samples reaching this case : DebugKeyboard, Gamepad, Tiling, ShadowBuffer
			// Close line-loops using a final single line, drawn from the end to the start vertex :
			INDEX16 pIndexData[2] = {0,DrawContext.dwVertexCount};
			//we're only using this function call to setup host index buffer.
			ConvertedIndexBuffer& CacheEntry = CxbxUpdateActiveIndexBuffer(pIndexData, DrawContext.dwVertexCount, convertIndexBuffer::passthrough);

			HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitive(
				/* PrimitiveType = */EmuXB2PC_D3DPrimitiveType(DrawContext.XboxPrimitiveType),
				/* BaseVertexIndex, = */0, // Base vertex index has been accounted for in the stream conversion, now we need to "un-offset" the index buffer
				/* MinVertexIndex = */0,
				/* NumVertices = */2,
				/* startIndex = DrawContext.dwStartVertex = */DrawContext.dwStartVertex,
				1);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitive");
		}
	}
}
IDirect3DBaseTexture* CxbxConvertXboxSurfaceToHostTexture(xbox::X_D3DBaseTexture* pBaseTexture)
{
	LOG_INIT; // Allows use of DEBUG_D3DRESULT

	IDirect3DSurface* pHostSurface = GetHostSurface(pBaseTexture);
	if (!pHostSurface) {
        LOG_TEST_CASE("Failed to get host surface");
		return nullptr;
	}

	IDirect3DBaseTexture* pNewHostBaseTexture = nullptr;
	auto hRet = pHostSurface->GetContainer(IID_PPV_ARGS(&pNewHostBaseTexture));
    DEBUG_D3DRESULT(hRet, "pHostSurface->GetContainer");

	if (FAILED(hRet)) {
		LOG_TEST_CASE("Failed to get Texture from Surface");
		return nullptr;
	}

	return pNewHostBaseTexture;
}
extern DWORD NV2A_stateFlags;
extern xbox::X_D3DBaseTexture * g_pNV2A_SetTexture[xbox::X_D3DTS_STAGECOUNT];
void CxbxUpdateHostTextures()
{
	LOG_INIT; // Allows use of DEBUG_D3DRESULT
	DWORD stageProgram = XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_PSTEXTUREMODES);
	// use texture stage textures from NV2A if we're in pushbuffer replay mode
	// if ((NV2A_stateFlags & X_STATE_RUNPUSHBUFFERWASCALLED) != 0)
	// using textures composed from Kelvin
	if (is_pgraph_using_NV2A_Kelvin()) {
		stageProgram = NV2ARenderStates.GetXboxRenderState(xbox::X_D3DRS_PSTEXTUREMODES);
	}
	DWORD stageMode[5];
	for (int stage = 0; stage < xbox::X_D3DTS_STAGECOUNT; stage++)
		stageMode[stage] = (stageProgram >> (stage * 5)) & 0x1f;
	//this is to generalized the checking code for bumpmap texture which will index the stage mode of next stage.
	stageMode[4] = NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_PROGRAM_NONE;
	// Set the host texture for each stage
	for (int stage = 0; stage < xbox::X_D3DTS_STAGECOUNT; stage++) {
		auto pXboxBaseTexture = g_pXbox_SetTexture[stage];
		if (is_pgraph_using_NV2A_Kelvin()) {
			pXboxBaseTexture = g_pNV2A_SetTexture[stage];
			//stageProgram = NV2ARenderStates.GetXboxRenderState(xbox::X_D3DRS_PSTEXTUREMODES);
		}

		IDirect3DBaseTexture* pHostBaseTexture = nullptr;
		bool bNeedRelease = false;
		
		// for UV texture format, xbox has alias formats which are not UV format. in most cases xbox uses UV format as UV format and this happened in bumpenv texture stage.
		// so cxbxr converts xbox UV texture to d3d9 UV texture by default.
		// but for certain cases, xbox uses alias format and the texture in alias format is not used in bumpenv stage, in this case we need to force convert the UV format texture to ARGB texture
		// test case: DynamicGamma sample.
		// check the xbox format is an UV format or not, if UV format, then check if we're bumpenv stage or not. if an UV format is set in none bumpenv stage, recreate the host texture in ARGB
		extern bool g_bForceHostARGBConversion;
		extern BOOL EmuXBFormatIsBumpMap(xbox::X_D3DFORMAT Format);
#if 1
		//if (stage < 3)
		if(pXboxBaseTexture!=nullptr && stageMode[stage]!=NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_PROGRAM_NONE){
			xbox::X_D3DFORMAT X_Format = GetXboxPixelContainerFormat(pXboxBaseTexture);
			//bumpmap is not so frequently used, let's check it first.
			if (EmuXBFormatIsBumpMap(X_Format)){
				//if a bumpmap is used in none bumpenv stage, let's convert it to ARGB
				
				//if (   (stageMode[stage+1] != NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_BUMPENVMAP)
				//	&& (stageMode[stage + 1] != NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_BUMPENVMAP))
				DWORD colorSign = NV2ATextureStates.Get(stage, xbox::X_D3DTSS_COLORSIGN);
				if ( colorSign== 0
					//&& (stageMode[stage ] != NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_BUMPENVMAP_LUMINANCE)
					&& (stageMode[stage + 1] != NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_BUMPENVMAP_LUMINANCE)
					&&  stageMode[stage + 1] != NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_BUMPENVMAP)
					//&& (stageMode[stage+1] != NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_BUMPENVMAP_LUMINANCE))
				{
					//get xbox format and related PC format
					D3DFORMAT defaultPCFormat = EmuXB2PC_D3DFormat(X_Format);
					//if both xbox format and PC format are UV format, then we need to create the host texture with ARGB conversion in none bumpenv stage
				
					//GetHostResource(); CreateHostResource();
					auto key = GetHostResourceKey(pXboxBaseTexture, stage);
					auto& ResourceCache = GetResourceCache(key);
					auto it = ResourceCache.find(key);
					//if source not found, just create a host texture with ARGB
					if (it != ResourceCache.end()) {
						if (it->second.pHostResource!=nullptr) {
							//if(it->second.PCFormat!= D3DFMT_A8R8G8B8){
                            //defaultPCFormat is UV bumpmap format, if the host resource is uv bumpmap format, we erase it from cache and force regenerate it with ARGB
							if (it->second.PCFormat == defaultPCFormat) {
								//FreeHostResource(key);
								//erase the host resource empty key
								ResourceCache.erase(key);
							}
						}
						else {
							//erase the host resource empty key
							;//ResourceCache.erase(key);
						}
						EmuLog(LOG_LEVEL::WARNING, "GetHostResource: Resource not registered or does not have a host counterpart!");
					}
					if (EmuXBFormatCanBeConvertedToARGB(X_Format))
							g_bForceHostARGBConversion = true;
				}
#if 1           //we have to force the texture format to be set to bumpenv format
				else {
					//get xbox format and related PC format
					D3DFORMAT PCFormat = EmuXB2PC_D3DFormat(X_Format);
					//if both xbox format and PC format are UV format, then we need to create the host texture with ARGB conversion in none bumpenv stage

					//GetHostResource(); CreateHostResource();
					auto key = GetHostResourceKey(pXboxBaseTexture, stage);
					auto& ResourceCache = GetResourceCache(key);
					auto it = ResourceCache.find(key);
					//if source not found, just create a host texture with ARGB
					if (it != ResourceCache.end()) {
						if (it->second.pHostResource != nullptr) {
							//todo: in CreateHostResource, we could store the PCFormat of the created host resource in the resource cache by creating a new member in _resource_info_t, this would speed up the speed here.
							/*
							IDirect3DTexture* pCurrnetStageHostTexture = (IDirect3DTexture*)it->second.pHostResource.Get();
							IDirect3DSurface* pCurrentStageHostSurface;
							HRESULT hRet = pCurrnetStageHostTexture->GetSurfaceLevel(0, &pCurrentStageHostSurface);
							D3DSURFACE_DESC surfaceDesc;
							hRet = pCurrentStageHostSurface->GetDesc(&surfaceDesc);
							//if the host resource was converted to D3DFMT_A8R8G8B8, then we have to disgard the old host resource and force recreate bumpmap
							if (surfaceDesc.Format == D3DFMT_A8R8G8B8) {
							*/
							if (it->second.PCFormat == D3DFMT_A8R8G8B8){
								ResourceCache.erase(key);
							}
						}
						EmuLog(LOG_LEVEL::WARNING, "GetHostResource: Resource not registered or does not have a host counterpart!");
					}
				}
#endif
			}
		}
#endif
		if (pXboxBaseTexture != xbox::zeroptr && stageMode[stage] != NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_PROGRAM_NONE) {
			DWORD XboxResourceType = GetXboxCommonResourceType(pXboxBaseTexture);
			switch (XboxResourceType) {
			case X_D3DCOMMON_TYPE_TEXTURE:
				pHostBaseTexture = GetHostBaseTexture(pXboxBaseTexture, /*D3DUsage=*/0, stage);
				break;
			case X_D3DCOMMON_TYPE_SURFACE:
				// Surfaces can be set in the texture stages, instead of textures
				LOG_TEST_CASE("ActiveTexture set to a surface (non-texture) resource"); // Test cases : Burnout, Outrun 2006
				// We must wrap the surface before using it as a texture
				pHostBaseTexture = CxbxConvertXboxSurfaceToHostTexture(pXboxBaseTexture);
				// Release this texture (after SetTexture) when we succeeded in creating it :
				bNeedRelease = pHostBaseTexture != nullptr;
				break;
			default:
				LOG_TEST_CASE("ActiveTexture set to an unhandled resource type!");
				break;
			}
		}
		// reset the force ARGB xbox to host resource conversion 
		g_bForceHostARGBConversion = false;

		HRESULT hRet = g_pD3DDevice->SetTexture(stage, pHostBaseTexture);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetTexture");
		if (bNeedRelease) {
			pHostBaseTexture->Release();
		}
	}
}

void CxbxUpdateHostTextureScaling()
{
	//extern NV2ADevice* g_NV2A;
	NV2AState *d = g_NV2A->GetDeviceState();
	PGRAPHState *pg = &d->pgraph;
	// Xbox works with "Linear" and "Swizzled" texture formats
	// Linear formats are not addressed with normalized coordinates (similar to https://www.khronos.org/opengl/wiki/Rectangle_Texture?)
	// We want to use normalized coordinates in our shaders, so need to be able to scale the coordinates back
	// Note texcoords aren't only used for texture lookups
	// TODO store scaling per texture instead of per stage, and scale during lookup in the pixel shader

	// Each texture stage has one texture coordinate set associated with it
	// We'll store scale factors for each texture coordinate set
	std::array<std::array<float, 4>, xbox::X_D3DTS_STAGECOUNT> texcoordScales;
	texcoordScales.fill({ 1, 1, 1, 1 });
	extern VertexShaderMode g_NV2A_VertexShaderMode;//tmp glue
	for (int stage = 0; stage < xbox::X_D3DTS_STAGECOUNT; stage++) {
		auto pXboxBaseTexture = g_pXbox_SetTexture[stage];
		VertexShaderMode  VSHMode=g_Xbox_VertexShaderMode;
		// use texture stage textures from NV2A if we're in pushbuffer replay mode
		//if ((NV2A_stateFlags & X_STATE_RUNPUSHBUFFERWASCALLED) != 0)
		if (is_pgraph_using_NV2A_Kelvin()){
			pXboxBaseTexture = g_pNV2A_SetTexture[stage];
			VSHMode = g_NV2A_VertexShaderMode;
		}
		// No texture, no scaling to do
		if (pXboxBaseTexture == xbox::zeroptr) {
			continue;
		}

		// Texcoord index. Just the texture stage unless fixed function mode
		int texCoordIndex = stage;
		if (VSHMode == VertexShaderMode::FixedFunction) {
			// Get TEXCOORDINDEX for the current texture stage's state
			// Stores both the texture stage index and information for generating coordinates
			// See D3DTSS_TEXCOORDINDEX
			DWORD texCoordIndexState;
			if(is_pgraph_using_NV2A_Kelvin())
				texCoordIndexState = NV2ATextureStates.Get(stage, xbox::X_D3DTSS_TEXCOORDINDEX);
			else
				texCoordIndexState = XboxTextureStates.Get(stage, xbox::X_D3DTSS_TEXCOORDINDEX);
			// If coordinates are generated, we don't have to worry about the coordinates coming from the title
			bool isGenerated = texCoordIndexState >= X_D3DTSS_TCI_CAMERASPACENORMAL;
			if (isGenerated) {
				continue;
			}

			// Determine the texture coordinate addressing this texture stage
			texCoordIndex = (texCoordIndexState & 0x3); // 0 - 3
			// NV2A doesn't preserve X_D3DTSS_TEXCOORDINDEX, instead xbox api maps the texcoord index directly when setting up vertex attr. offset.
			// we already revers the texcoord index via vertex attr. offset mapping when we compose vertex format and stream info from NV2A.
		}

		auto texCoordScale = &texcoordScales[texCoordIndex];

		// Check for active linear textures.
		xbox::X_D3DFORMAT XboxFormat = GetXboxPixelContainerFormat(pXboxBaseTexture);
		if (EmuXBFormatIsLinear(XboxFormat)) {
			// Test-case : This is often hit by the help screen in XDK samples.
			// Set scaling factor for this texture, which will be applied to
			// all texture-coordinates in CxbxVertexShaderTemplate.hlsl
			// Note : Linear textures are two-dimensional at most (right?)
			float width, height;
			// revised the compare condition because when using textures composed from Kelvin, only texture.data & texture.format will be the same
			if (((xbox::X_D3DSurface*)pXboxBaseTexture)->Data == g_pXbox_BackBufferSurface->Data
				&& ((xbox::X_D3DSurface*)pXboxBaseTexture)->Format == g_pXbox_BackBufferSurface->Format) {
				// Account for MSAA
				// Test case: Max Payne 2 (bullet time)
				GetBackBufferPixelDimensions(width, height);
			}
			else {
				width = (float)GetPixelContainerWidth(pXboxBaseTexture);
				height = (float)GetPixelContainerHeight(pXboxBaseTexture);
			}

			*texCoordScale = {
				width,
				height,
				1.0f, // TODO should this be mip levels for volume textures?
				1.0f
			};
		}

		// When a depth buffer is used as a texture
		// We do 'Native Shadow Mapping'
		// https://aras-p.info/texts/D3D9GPUHacks.html
		// The z texture coordinate component holds a depth value, which needs to be normalized
		// TODO implement handling for
		// - X_D3DRS_SHADOWFUNC
		// - X_D3DRS_POLYGONOFFSETZSLOPESCALE
		// - X_D3DRS_POLYGONOFFSETZOFFSET
		if (EmuXBFormatIsDepthBuffer(XboxFormat)) {
			(*texCoordScale)[2] = (float)GetZScaleForPixelContainer(pXboxBaseTexture);
		}
	}
	// Pass above determined texture scaling factors to our HLSL shader.
	// Note : CxbxVertexShaderTemplate.hlsl applies texture scaling on
	// output registers oT0 to oT3. It may be needed to move the scaling
	// and apply it on input registers instead. In that case, we'd have to
	// figure out which registers are used to pass texture-coordinates into
	// the shader and allow scaling on any of the input registers (so we'd
	// need to allow scaling on all 16 attributes, instead of just the four
	// textures like we do right now).
	g_pD3DDevice->SetVertexShaderConstantF(CXBX_D3DVS_TEXTURES_SCALE_BASE, (float*)texcoordScales.data(), CXBX_D3DVS_TEXTURES_SCALE_SIZE);
}

void CxbxUpdateDirtyVertexShaderConstants(const float* constants, bool* dirty) {
	// Reduce the number of calls to D3D9 by updating contiguous "batches"
	// of dirty states
	int batchStartIndex = -1; // -1 means we aren't in a batch

	for (int i = 0; i < X_D3DVS_CONSTREG_COUNT; i++) {
		if (batchStartIndex == -1 && dirty[i]) {
			batchStartIndex = i; // Start a batch
		}
		else if (batchStartIndex != -1 && !dirty[i]) {
			// Finish the batch
			int count = i - batchStartIndex;
			g_pD3DDevice->SetVertexShaderConstantF(batchStartIndex, &constants[batchStartIndex * 4], count);
			batchStartIndex = -1;
		}

		// Constant is no longer dirty
		dirty[i] = false;
	}

	// Send the final batch
	if (batchStartIndex != -1) {
		int count = X_D3DVS_CONSTREG_COUNT - batchStartIndex + 1;
		g_pD3DDevice->SetVertexShaderConstantF(batchStartIndex, &constants[batchStartIndex * 4], count);
	}
}

extern float* HLE_get_NV2A_vertex_constant_float4_ptr(unsigned const_index); // TMP glue
// TODO : Once we're able to flush the NV2A push buffer
// remove our patches on D3DDevice_SetVertexShaderConstant (and CxbxrImpl_SetVertexShaderConstant)
void CxbxUpdateHostVertexShaderConstants()
{
	// For Xbox vertex shader programs, the Xbox vertex shader constants
	// are mirrored on the host.
	// Otherwise, the same set of constants is used for the fixed function vertex shader
	// implementation instead

	// Track which constants are currently written
	// So we can skip updates
	static bool isXboxConstants = false;
	extern VertexShaderMode g_NV2A_VertexShaderMode;//tmp glue
	VertexShaderMode  VSHMode = g_Xbox_VertexShaderMode;
	if (is_pgraph_using_NV2A_Kelvin()) {
		VSHMode = g_NV2A_VertexShaderMode;
	}
	if (VSHMode == VertexShaderMode::FixedFunction && g_UseFixedFunctionVertexShader) {
		// Write host FF shader state
		// TODO dirty tracking like for Xbox constants?
		UpdateFixedFunctionVertexShaderState();
		isXboxConstants = false;
	}
	else {
		// Write Xbox constants
		auto pg = &(g_NV2A->GetDeviceState()->pgraph);
		auto constant_floats = (float*)pg->vsh_constants;

		if (isXboxConstants) {
			// Only need to overwrite what's changed
			CxbxUpdateDirtyVertexShaderConstants(constant_floats, pg->vsh_constants_dirty);
		}
		else {
			// We need to update everything
			g_pD3DDevice->SetVertexShaderConstantF(0, constant_floats, X_D3DVS_CONSTREG_COUNT);
		}

		// We've written the Xbox constants
		isXboxConstants = true;

		// FIXME our viewport constants don't match Xbox values
		// If we write them to pgraph constants, like we do with constants set by the title,
		// the Xbox could overwrite them (at any time?) and we get flickering geometry.
		// For now, set our viewport constants directly in the call below,
		// overwriting whatever was in pgraph
		// Test case:
		// Xbox dashboard (during initial fade from black)
		// Need for Speed: Hot Pursuit 2 (car select)
		// 
		CxbxUpdateHostViewPortOffsetAndScaleConstants();

		// fixed mode vertex shader already has fog state constats, these constants below should be for user program/pass through only.
		// Placed this here until we find a better place
		uint32_t fogTableMode;
		float fogDensity, fogStart, fogEnd;
		DWORD density, start, end;
		if (is_pgraph_using_NV2A_Kelvin()) {
			fogTableMode = NV2ARenderStates.GetXboxRenderState(xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FOGTABLEMODE);
			fogDensity = NV2ARenderStates.GetXboxRenderStateAsFloat(xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FOGDENSITY);
			fogStart = NV2ARenderStates.GetXboxRenderStateAsFloat(xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FOGSTART);
			fogEnd = NV2ARenderStates.GetXboxRenderStateAsFloat(xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FOGEND);
		}
		else {
			fogTableMode = XboxRenderStates.GetXboxRenderState(xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FOGTABLEMODE);
			fogDensity = XboxRenderStates.GetXboxRenderStateAsFloat(xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FOGDENSITY);
			fogStart = XboxRenderStates.GetXboxRenderStateAsFloat(xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FOGSTART);
			fogEnd = XboxRenderStates.GetXboxRenderStateAsFloat(xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FOGEND);
		}
		float fogStuff[4] = { (float)fogTableMode, fogDensity, fogStart, fogEnd };
		g_pD3DDevice->SetVertexShaderConstantF(CXBX_D3DVS_CONSTREG_FOGINFO, fogStuff, 1);

	}
}

void CxbxUpdateHostViewport() {
	// We don't have a fixed function shader so we rely on D3D9 fixed function mode
	// So we have to set a viewport based on the current Xbox viewport
	// Antialiasing mode affects the viewport offset and scale
	float aaScaleX, aaScaleY;
	float aaOffsetX, aaOffsetY;
	//GetMultiSampleScaleRaw() return scales include backbuffer scale
	GetMultiSampleScaleRaw(aaScaleX, aaScaleY);
	GetMultiSampleOffset(aaOffsetX, aaOffsetY);
/*
	if (is_pgraph_using_NV2A_Kelvin()) {
		CxbxrGetSuperSampleScaleXY(aaScaleX, aaScaleY);
		if (aaScaleX <= 0)aaScaleX = 1.0;
		if (aaScaleY <= 0)aaScaleY = 1.0;
	}
*/
	DWORD HostRenderTarget_Width, HostRenderTarget_Height;
	if (!GetHostRenderTargetDimensions(&HostRenderTarget_Width, &HostRenderTarget_Height)) {
		LOG_TEST_CASE("Could not get rendertarget dimensions while setting the viewport");
	}

	float Xscale = aaScaleX * g_RenderUpscaleFactor;
	float Yscale = aaScaleY * g_RenderUpscaleFactor;
	extern VertexShaderMode g_NV2A_VertexShaderMode;//tmp glue
	VertexShaderMode  VSHMode = g_Xbox_VertexShaderMode;
	if (is_pgraph_using_NV2A_Kelvin()) {
		VSHMode = g_NV2A_VertexShaderMode;
	}
	if (VSHMode == VertexShaderMode::FixedFunction) {
		// Set viewport
		D3DVIEWPORT hostViewport = g_Xbox_Viewport;
		// use viewport composed from kelvin when we're in pgraph draw calls.
		if(is_pgraph_using_NV2A_Kelvin())
		    CxbxrGetViewport(hostViewport);
		hostViewport.X *= Xscale;
		hostViewport.Y *= Yscale;
		hostViewport.Width *= Xscale;
		hostViewport.Height *= Yscale;
		g_pD3DDevice->SetViewport(&hostViewport);

		// Reset scissor rect
		RECT viewportRect;
		viewportRect.left = 0;
		viewportRect.top = 0;
		viewportRect.right = HostRenderTarget_Width;
		viewportRect.bottom = HostRenderTarget_Height;
		g_pD3DDevice->SetScissorRect(&viewportRect);
	}
	else {
		// Set default viewport over the whole screen
		// And let the vertex shader take care of vertex placement
		// So we can handle shaders that don't use the Xbox viewport constants and don't align
		// with the currently set viewport
		// Test case: ???

		D3DVIEWPORT hostViewport;
		hostViewport.X = 0;
		hostViewport.Y = 0;
		hostViewport.Width = HostRenderTarget_Width;
		hostViewport.Height = HostRenderTarget_Height;
		hostViewport.MinZ = 0.0f;
		hostViewport.MaxZ = 1.0f;

		g_pD3DDevice->SetViewport(&hostViewport);

		// We still need to clip to the viewport
		// Scissor to viewport
		g_pD3DDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
		RECT viewportRect;
		viewportRect.left = g_Xbox_Viewport.X * Xscale;
		viewportRect.top = g_Xbox_Viewport.Y * Yscale;
		viewportRect.right = viewportRect.left + (g_Xbox_Viewport.Width * Xscale);
		viewportRect.bottom = viewportRect.top + (g_Xbox_Viewport.Height * Yscale);
		g_pD3DDevice->SetScissorRect(&viewportRect);
	}
}

extern void CxbxUpdateHostVertexDeclaration(); // TMP glue
extern void CxbxUpdateHostVertexShader(); // TMP glue
extern void pgraph_use_NV2A_Kelvin(void);
extern bool g_NV2AVertexShader_dirty;

void CxbxUpdateNativeD3DResources()
{
	extern bool g_VertexShader_dirty; // tmp glue

	//if xbox backbuffer was locked before swap, we have to update xbox backbuffer surface to it's host suface
	if (g_bXbox_BackBufferSurfaceLocked) {
		CxbxrUpdateHostXboxBackBufferSurface(g_pXbox_BackBufferSurface);
		//clear the flag so we won't do the data transfer multiple time with unchanged data.
		g_bXbox_BackBufferSurfaceLocked = false;
	}

	// Before we start, make sure our resource cache stays limited in size
	PrunePaletizedTexturesCache(); // TODO : Could we move this to Swap instead?

	CxbxUpdateHostVertexDeclaration();
	// setup vertex shader input if there are any changes
	if (is_pgraph_using_NV2A_Kelvin()) {
		if (g_NV2AVertexShader_dirty == true) {
			CxbxUpdateHostVertexShader();
			// reset dirty flag
			g_NV2AVertexShader_dirty = false;
		}
	}
	else {
		if (g_VertexShader_dirty == true) {
			CxbxUpdateHostVertexShader();
			// reset dirty flag
			g_VertexShader_dirty = false;
		}
	}
	

	CxbxUpdateHostVertexShaderConstants();

	//todo: use viewport composed from Kelvin.
	CxbxUpdateHostViewport();

	// NOTE: Order is important here
    // Some Texture States depend on RenderState values (Point Sprites)
    // And some Pixel Shaders depend on Texture State values (BumpEnvMat, etc)
	CxbxUpdateHostTextures();
	CxbxUpdateHostTextureScaling();
	if (is_pgraph_using_NV2A_Kelvin()) {
		NV2ARenderStates.Apply();
		NV2ATextureStates.Apply();
	}
	else {
		XboxRenderStates.Apply();
		XboxTextureStates.Apply();
	}

    // If Pixel Shaders are not disabled, process them
    if (!g_DisablePixelShaders) {
        DxbxUpdateActivePixelShader();
    }


/* TODO : Port these :
	DxbxUpdateDeferredStates(); // BeginPush sample shows us that this must come *after* texture update!
	DxbxUpdateActiveVertexBufferStreams();
	DxbxUpdateActiveRenderTarget();
*/
}

// This function should be called in thight idle-wait loops.
// It's purpose is to lower CPU cost in such a way that the
// caller will still repond quickly, without actually waiting
// or giving up it's time-slice.
// See https://docs.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-yieldprocessor
// and https://software.intel.com/en-us/cpp-compiler-developer-guide-and-reference-pause-intrinsic
inline void CxbxCPUIdleWait() // TODO : Apply wherever applicable
{
	YieldProcessor();
}

// This function indicates whether Cxbx can flush host GPU commands.
bool CxbxCanFlushHostGPU()
{
	return (g_pHostQueryWaitForIdle != nullptr);
}

// Wait until host GPU finished processing it's command queue
bool CxbxFlushHostGPU()
{
	// The following can only work when host GPU queries are available
	if (!CxbxCanFlushHostGPU()) {
		// If we can't query host GPU, return failure
		return false;
	}

	// See https://docs.microsoft.com/en-us/windows/win32/direct3d9/queries
	// Add an end marker to the command buffer queue.
	// This, so that the next GetData will always have at least one
	// final query event to flush out, after which GPU will be done.
	g_pHostQueryWaitForIdle->Issue(D3DISSUE_END);

	// Empty the command buffer and wait until host GPU is idle.
	while (S_FALSE == g_pHostQueryWaitForIdle->GetData(nullptr, 0, D3DGETDATA_FLUSH))
		CxbxCPUIdleWait();

	// Signal caller that host GPU has been flushed
	return true;
}

// This function mimicks NV2A software callback events.
// Normally, these would be handled by actual push-buffer
// command handling at the point where they where inserted.
// Since our HLE mostly circumvents the NV2A pushbuffer,
// this function has to be called after 'pushing' functions.
void CxbxHandleXboxCallbacks()
{
	// The following can only work when host GPU queries are available
	if (g_pHostQueryCallbackEvent != nullptr) {
		// Query whether host GPU encountered a callback event already
		if (S_FALSE == g_pHostQueryCallbackEvent->GetData(nullptr, 0, 0)) {
			// If not, don't handle callbacks
			return;
		}
	}

	// Process inserted callbacks
	while (!g_Xbox_CallbackQueue.empty()) {
		// Fetch a callback from the FIFO callback queue
		s_Xbox_Callback XboxCallback = g_Xbox_CallbackQueue.front();
		g_Xbox_CallbackQueue.pop();

		// Differentiate between write and read callbacks
		if (XboxCallback.Type == xbox::X_D3DCALLBACK_WRITE) {
			// Write callbacks should wait until GPU is idle
			if (!CxbxFlushHostGPU()) {
				// Host GPU can't be flushed. In the old behaviour, we made the callback anyway
				// TODO : Should we keep doing that?
			}
		} else {
			assert(XboxCallback.Type == xbox::X_D3DCALLBACK_READ);
			// Should we mimick Read callback old behaviour?
			if (g_bHack_DisableHostGPUQueries) {
				// Note : Previously, we only processed Write, and ignored Read callbacks
				continue;
			} else {
				// New behaviour does place Read callbacks too
			}
		}

		// Make the callback
		XboxCallback.pCallback(XboxCallback.Context);
	}
}

// On Xbox, this function inserts push-buffer commands that
// will trigger the software handler to perform the callback
// when the GPU processes these commands.
// The type X_D3DCALLBACK_WRITE callbacks are prefixed with an
// wait-for-idle command, but otherwise they're identical.
// (Software handlers are triggered on NV2A via NV097_NO_OPERATION) 
void CxbxrImpl_InsertCallback
(
	xbox::X_D3DCALLBACKTYPE	Type,
	xbox::X_D3DCALLBACK		pCallback,
	xbox::dword_xt				Context
)
{
	if (Type > xbox::X_D3DCALLBACK_WRITE) {
		LOG_TEST_CASE("Illegal callback type!");
		return;
	}

	if (pCallback == xbox::zeroptr) {
		LOG_TEST_CASE("pCallback == xbox::zeroptr!");
		return;
	}

	//now this is called by pgraph method handler, we can call the callback routine directly.
	pCallback(Context);

	/*
	// Should we mimick old behaviour?
	if (g_bHack_DisableHostGPUQueries) {
		// Mimick old behaviour, in which only the final callback event
		// was remembered, by emptying the callback queue entirely :
		while (!g_Xbox_CallbackQueue.empty()) {
			g_Xbox_CallbackQueue.pop();
		}
	}

	// Push this callback's arguments into the callback queue :
	s_Xbox_Callback XboxCallback = { pCallback, Type, Context };
	g_Xbox_CallbackQueue.push(XboxCallback); // g_Xbox_CallbackQueue.emplace(pCallback, Type, Context); doesn't compile?

	// Does host supports GPU queries?
	if (g_pHostQueryCallbackEvent != nullptr) {
		// Insert a callback event on host GPU,
		// which will be handled by CxbxHandleXboxCallback
		g_pHostQueryCallbackEvent->Issue(D3DISSUE_END);
	}
	*/

}
xbox::void_xt CxbxrImpl_SetPixelShader(xbox::dword_xt Handle)
{
    
    // Copy the Pixel Shader data to our RenderState handler (this includes values for pixel shader constants)
    // This mirrors the fact that unpatched SetPixelShader does the same thing!
    // This shouldn't be necessary anymore, but shaders still break if we don't do this
	// This breakage might be caused by our push-buffer processing could be "trailing behind" what our patches do;
	// By writing to render state during this patch, we avoid missing out on updates that push buffer commands would perform.
	// However, any updates that occur mid-way can overwrite what we store here, and still cause problems!
	// The only viable solution for that would be to draw entirely based on push-buffer handling (which might require removing possibly all D3D patches!)
    
	if (is_pgraph_using_NV2A_Kelvin()) {
		if (pNV2A_PixelShader != nullptr) {
			// TODO : If D3DDevice_SetPixelShader() in XDKs don't overwrite the X_D3DRS_PS_RESERVED slot with PSDef.PSTextureModes,
			// store it here and restore after memcpy, or alternatively, perform two separate memcpy's (the halves before, and after the reserved slot).
			memcpy(NV2ARenderStates.GetPixelShaderRenderStatePointer(), pNV2A_PixelShader->pPSDef, sizeof(xbox::X_D3DPIXELSHADERDEF) - 3 * sizeof(DWORD));
			// Copy the PSDef.PSTextureModes field to it's dedicated slot, which lies outside the range of PixelShader render state slots
			// Note : This seems to be what XDK's do as well. Needs verification.

			NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_PSTEXTUREMODES, pNV2A_PixelShader->pPSDef->PSTextureModes);
		}
	}
	else {
		// Cache the active shader handle
		g_pXbox_PixelShader = (xbox::X_PixelShader*)Handle;

		if (g_pXbox_PixelShader != nullptr) {
			// TODO : If D3DDevice_SetPixelShader() in XDKs don't overwrite the X_D3DRS_PS_RESERVED slot with PSDef.PSTextureModes,
			// store it here and restore after memcpy, or alternatively, perform two separate memcpy's (the halves before, and after the reserved slot).
			memcpy(XboxRenderStates.GetPixelShaderRenderStatePointer(), g_pXbox_PixelShader->pPSDef, sizeof(xbox::X_D3DPIXELSHADERDEF) - 3 * sizeof(DWORD));
			// Copy the PSDef.PSTextureModes field to it's dedicated slot, which lies outside the range of PixelShader render state slots
			// Note : This seems to be what XDK's do as well. Needs verification.

			XboxRenderStates.SetXboxRenderState(xbox::X_D3DRS_PSTEXTUREMODES, g_pXbox_PixelShader->pPSDef->PSTextureModes);
		}
	}
}

// Overload for logging
static void D3DDevice_SetPixelShader_0__LTCG_eax_handle
(
    xbox::dword_xt      Handle
)
{
    LOG_FUNC_ONE_ARG(Handle);
}

// LTCG specific D3DDevice_SetPixelShader function...
// This uses a custom calling convention where parameter is passed in EAX
// Test-case: Metal Wolf Chaos
// Test-case: Lord of the Rings: The Third Age
// Test-case: Midtown Madness 3
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetPixelShader_0__LTCG_eax_handle)()
{
    dword_xt Handle;
    __asm {
        LTCG_PROLOGUE
        mov  Handle, eax
    }

    // Log
    D3DDevice_SetPixelShader_0__LTCG_eax_handle(Handle);

    __asm {
        mov  eax, Handle
        call XB_TRMP(D3DDevice_SetPixelShader_0__LTCG_eax_handle)
    }

    //CxbxrImpl_SetPixelShader(Handle);
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)Handle;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetPixelShader_0__LTCG_eax_handle, 1, PBTokenArray);//argCount 14

    __asm {
        LTCG_EPILOGUE
        ret
    }
}

// ******************************************************************
// * patch: D3DDevice_SetPixelShader
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetPixelShader)
(
	dword_xt           Handle
)
{
	LOG_FUNC_ONE_ARG(Handle);

	// Call the Xbox function to make sure D3D structures get set
	XB_TRMP(D3DDevice_SetPixelShader)(Handle);

	//CxbxrImpl_SetPixelShader(Handle);

	//CxbxrImpl_SetPixelShader(Handle);
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)Handle;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetPixelShader, 1, PBTokenArray);//argCount 14
}

extern void D3D_draw_state_update(NV2AState* d);

void WINAPI CxbxrImpl_DrawVertices
(
	xbox::X_D3DPRIMITIVETYPE PrimitiveType,
	xbox::uint_xt            StartVertex,
	xbox::uint_xt            VertexCount
)
{
	if (!IsValidXboxVertexCount(PrimitiveType, VertexCount)) {
		LOG_TEST_CASE("Invalid VertexCount");
		return;
	}

	// TODO : Call unpatched CDevice_SetStateVB(0);
	// map pgraph status to D3D
	NV2AState* d = g_NV2A->GetDeviceState();
	// this shall update all NV2A texture/transform/vertex shader/pixel shader
	// D3D_draw_state_update(d);

	CxbxDrawContext DrawContext = {};

	DrawContext.XboxPrimitiveType = PrimitiveType;
	DrawContext.dwVertexCount = VertexCount;
	DrawContext.dwStartVertex = StartVertex;

	VertexBufferConverter.Apply(&DrawContext);
	if (DrawContext.XboxPrimitiveType == xbox::X_D3DPT_QUADLIST) {
		//if (StartVertex == 0) {
			//LOG_TEST_CASE("X_D3DPT_QUADLIST (StartVertex == 0)"); // disabled, hit too often
			// test-case : ?X-Marbles
			// test-case XDK Samples : AlphaFog, AntiAlias, BackBufferScale, BeginPush, Cartoon, TrueTypeFont (?maybe PlayField?)
		//}
		//else {
			LOG_TEST_CASE("X_D3DPT_QUADLIST (StartVertex > 0)");
			// https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/1156
			// test-case : All - Star Baseball '03
			// test-case : Army Men Major Malfunction
			// test-case : Big Mutha Truckers
			// test-case : BLiNX: the time sweeper
			// test-case : Blood Wake
			// test-case : Call of Duty: Finest Hour
			// test-case : Flight academy
			// test-case : FIFA World Cup 2002
			// test-case : GENMA ONIMUSHA 
			// test-case : Halo - Combat Evolved
			// test-case : Harry Potter and the Sorcerer's Stone
			// test-case : Heroes of the Pacific
			// test-case : Hummer Badlands
			// test-case : Knights Of The Temple 2
			// test-case : LakeMasters Bass fishing
			// test-case : MetalDungeon
			// test-case : NFL Fever 2003 Demo - main menu
			// test-case : Night Caster 2
			// test-case : Pinball Hall of Fame
			// test-case : Robotech : Battlecry
			// test-case : SpiderMan 2
			// test-case : Splinter Cell Demo
			// test-case : Stubbs the Zombie
			// test-case : Tony Hawk's Pro Skater 2X (main menu entries)
			// test-case : Worms 3D Special Edition
			// test-case : XDK sample Lensflare (4, for 10 flare-out quads that use a linear texture; rendered incorrectly: https://youtu.be/idwlxHl9nAA?t=439)
			DrawContext.dwStartVertex = StartVertex; // Breakpoint location for testing.
		//}

		// Draw quadlists using a single 'quad-to-triangle mapping' index buffer :
		// Assure & activate that special index buffer :
		CxbxAssureQuadListD3DIndexBuffer(/*NrOfQuadIndices=*/DrawContext.dwVertexCount);
		// Convert quad vertex count to triangle vertex count :
		// todo: this could be wrong, the NumVertices is for actual vertex span in the stream, not the triangle list index counts. we should use DrawContext.dwVertexCount directly instead.
		UINT NumVertices = QuadListToTriangleListVertexCount(DrawContext.dwVertexCount);
		// the dwHostPrimitiveCount is coverted to triangle list already :
		UINT primCount = DrawContext.dwHostPrimitiveCount;
		// See https://docs.microsoft.com/en-us/windows/win32/direct3d9/rendering-from-vertex-and-index-buffers
		// for an explanation on the function of the BaseVertexIndex, MinVertexIndex, NumVertices and StartIndex arguments.
		// Emulate drawing quads by drawing each quad with two indexed triangles :
		HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitive(
			/*PrimitiveType=*/D3DPT_TRIANGLELIST,
			/*BaseVertexIndex=*/0, // Base vertex index has been accounted for in the stream conversion
			/*MinVertexIndex=*/0,
			NumVertices,
			/*startIndex=*/0,
			primCount
		);
		//DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitive(X_D3DPT_QUADLIST)");

		g_dwPrimPerFrame += primCount;
	}
	else {
		// if (StartVertex > 0) LOG_TEST_CASE("StartVertex > 0 (non-quad)"); // Verified test case : XDK Sample (PlayField)
		HRESULT hRet = g_pD3DDevice->DrawPrimitive(
			EmuXB2PC_D3DPrimitiveType(DrawContext.XboxPrimitiveType),
			/*StartVertex=*/0, // Start vertex has been accounted for in the stream conversion
			DrawContext.dwHostPrimitiveCount
		);
		//DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawPrimitive");

		g_dwPrimPerFrame += DrawContext.dwHostPrimitiveCount;
		if (DrawContext.XboxPrimitiveType == xbox::X_D3DPT_LINELOOP) {
			// Close line-loops using a final single line, drawn from the end to the start vertex
			LOG_TEST_CASE("X_D3DPT_LINELOOP"); // TODO : Text-cases needed

			assert(DrawContext.dwBaseVertexIndex == 0); // if this fails, it needs to be added to LowIndex and HighIndex :
			INDEX16 LowIndex = 0;
			INDEX16 HighIndex = (INDEX16)(DrawContext.dwHostPrimitiveCount);
			// Draw the closing line using a helper function (which will SetIndices)
			CxbxDrawIndexedClosingLine(LowIndex, HighIndex);
			// NOTE : We don't restore the previously active index buffer
		}
	}

	CxbxHandleXboxCallbacks();
}

void D3DDevice_DrawVertices
(
	xbox::X_D3DPRIMITIVETYPE PrimitiveType,
	xbox::uint_xt            StartVertex,
	xbox::uint_xt            VertexCount
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(PrimitiveType)
		LOG_FUNC_ARG(StartVertex)
		LOG_FUNC_ARG(VertexCount)
		LOG_FUNC_END;
	CxbxrImpl_DrawVertices(PrimitiveType, StartVertex, VertexCount);
}

// ******************************************************************
// * patch: D3DDevice_DrawVertices_4
// LTCG specific D3DDevice_DrawVertices function...
// This uses a custom calling convention where parameter is passed in ECX, EAX and Stack
// Test Case: Conker
// ******************************************************************
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_DrawVertices_4__LTCG_ecx2_eax3)
(
    X_D3DPRIMITIVETYPE PrimitiveType
)
{
    uint_xt VertexCount;
    uint_xt StartVertex;
    __asm {
        LTCG_PROLOGUE
        mov  VertexCount, eax
        mov  StartVertex, ecx
    }
	//trampoline draw call if we're recording pushbuffer
	if (g_pXbox_BeginPush_Buffer != nullptr){
		XB_TRMP(D3DDevice_DrawVertices_4__LTCG_ecx2_eax3)(PrimitiveType);
	}else{
		bool WaitForPGRAPH;
		WaitForPGRAPH = true;
		//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
		PBTokenArray[2] = (DWORD)&WaitForPGRAPH;// (DWORD)PrimitiveType;
		PBTokenArray[3] = (DWORD)StartVertex;
		PBTokenArray[4] = (DWORD)VertexCount;

		//give the correct token enum here, and it's done.
		Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_DrawVertices_4__LTCG_ecx2_eax3, 3);//argCount, not necessary, default to 14

		EmuKickOff();

		while (WaitForPGRAPH)
			;
		// Dxbx Note : In DrawVertices and DrawIndexedVertices, PrimitiveType may not be D3DPT_POLYGON
		// Move original implementation code to CxbxrImpl_DrawVertices(PrimitiveType, StartVertex, VertexCount); for duplicate usage with D3DDevice_DrawVertices_4
		// CxbxrImpl_DrawVertices(PrimitiveType, StartVertex, VertexCount);
		D3DDevice_DrawVertices(PrimitiveType, StartVertex, VertexCount);
	}
    __asm {
        LTCG_EPILOGUE
        ret  4
    }
}

// ******************************************************************
// * patch: D3DDevice_DrawVertices_8__LTCG_eax3
// LTCG specific D3DDevice_DrawVertices function...
// ******************************************************************
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_DrawVertices_8__LTCG_eax3)
(
    X_D3DPRIMITIVETYPE PrimitiveType,
    uint_xt            StartVertex
)
{
    uint_xt VertexCount;
    __asm {
        LTCG_PROLOGUE
        mov  VertexCount, eax
    }
	//trampoline draw call if we're recording pushbuffer
	if (g_pXbox_BeginPush_Buffer != nullptr) {
		XB_TRMP(D3DDevice_DrawVertices_8__LTCG_eax3)(PrimitiveType, StartVertex);
	}
	else {
		bool WaitForPGRAPH;
		WaitForPGRAPH = true;
		//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
		PBTokenArray[2] = (DWORD)&WaitForPGRAPH;// (DWORD)PrimitiveType;
		PBTokenArray[3] = (DWORD)StartVertex;
		PBTokenArray[4] = (DWORD)VertexCount;

		//give the correct token enum here, and it's done.
		Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_DrawVertices_8__LTCG_eax3, 3);//argCount, not necessary, default to 14

		EmuKickOff();

		while (WaitForPGRAPH)
			;
		// Dxbx Note : In DrawVertices and DrawIndexedVertices, PrimitiveType may not be D3DPT_POLYGON
		// Move original implementation code to CxbxrImpl_DrawVertices(PrimitiveType, StartVertex, VertexCount); for duplicate usage with D3DDevice_DrawVertices_4
		// CxbxrImpl_DrawVertices(PrimitiveType, StartVertex, VertexCount);
		D3DDevice_DrawVertices(PrimitiveType, StartVertex, VertexCount);
	}
    __asm {
        LTCG_EPILOGUE
        ret  4
    }
}

// ******************************************************************
// * patch: D3DDevice_DrawVertices
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_DrawVertices)
(
    X_D3DPRIMITIVETYPE PrimitiveType,
    uint_xt            StartVertex,
    uint_xt            VertexCount
)
{
	
	// Dxbx Note : In DrawVertices and DrawIndexedVertices, PrimitiveType may not be D3DPT_POLYGON

	if (g_pXbox_BeginPush_Buffer != nullptr){
	    XB_TRMP(D3DDevice_DrawVertices)(PrimitiveType, StartVertex, VertexCount);
	}else{
		bool WaitForPGRAPH;
		WaitForPGRAPH = true;
		//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
		PBTokenArray[2] = (DWORD)&WaitForPGRAPH;// (DWORD)PrimitiveType;
		PBTokenArray[3] = (DWORD)StartVertex;
		PBTokenArray[4] = (DWORD)VertexCount;

		//give the correct token enum here, and it's done.
		Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_DrawVertices, 3);//argCount, not necessary, default to 14

		EmuKickOff();

		while (WaitForPGRAPH)
			;
		// Dxbx Note : In DrawVertices and DrawIndexedVertices, PrimitiveType may not be D3DPT_POLYGON
		// Move original implementation code to CxbxrImpl_DrawVertices(PrimitiveType, StartVertex, VertexCount); for duplicate usage with D3DDevice_DrawVertices_4
		// CxbxrImpl_DrawVertices(PrimitiveType, StartVertex, VertexCount);
		D3DDevice_DrawVertices(PrimitiveType, StartVertex, VertexCount);
	}
}


void WINAPI CxbxrImpl_DrawVerticesUP
(
	xbox::X_D3DPRIMITIVETYPE  PrimitiveType,
	xbox::uint_xt             VertexCount,
	CONST xbox::PVOID         pVertexStreamZeroData,
	xbox::uint_xt             VertexStreamZeroStride
)
{
	if (!IsValidXboxVertexCount(PrimitiveType, VertexCount)) {
		LOG_TEST_CASE("Invalid VertexCount");
		return;
	}
	// TODO : Call unpatched CDevice_SetStateUP();
    //we update the resources in pgraph handler
	//CxbxUpdateNativeD3DResources();

	CxbxDrawContext DrawContext = {};

	DrawContext.XboxPrimitiveType = PrimitiveType;
	DrawContext.dwVertexCount = VertexCount;
	DrawContext.pXboxVertexStreamZeroData = pVertexStreamZeroData;
	DrawContext.uiXboxVertexStreamZeroStride = VertexStreamZeroStride;

	CxbxDrawPrimitiveUP(DrawContext);

	CxbxHandleXboxCallbacks();
}

void D3DDevice_DrawVerticesUP
(
	xbox::X_D3DPRIMITIVETYPE  PrimitiveType,
	xbox::uint_xt             VertexCount,
	CONST xbox::PVOID         pVertexStreamZeroData,
	xbox::uint_xt             VertexStreamZeroStride
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(PrimitiveType)
		LOG_FUNC_ARG(VertexCount)
		LOG_FUNC_ARG(pVertexStreamZeroData)
		LOG_FUNC_ARG(VertexStreamZeroStride)
		LOG_FUNC_END;
#if USEPGRAPH_DrawVerticesUP
#endif
}
extern bool g_nv2a_use_Kelvin;
// ******************************************************************
// * patch: D3DDevice_DrawVerticesUP
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_DrawVerticesUP)
(
	X_D3DPRIMITIVETYPE  PrimitiveType,
	uint_xt             VertexCount,
	CONST PVOID         pVertexStreamZeroData,
	uint_xt             VertexStreamZeroStride
	)
{
	//trampoline draw call if we're recording pushbuffer
	if (g_pXbox_BeginPush_Buffer != nullptr) {
		XB_TRMP(D3DDevice_DrawVerticesUP)(PrimitiveType, VertexCount, pVertexStreamZeroData, VertexStreamZeroStride);
	}
	else {
		bool WaitForPGRAPH;
		WaitForPGRAPH = true;
		//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
		PBTokenArray[2] = (DWORD)&WaitForPGRAPH;// (DWORD)PrimitiveType;
		PBTokenArray[3] = (DWORD)VertexCount;
		PBTokenArray[4] = (DWORD)pVertexStreamZeroData;
		PBTokenArray[5] = (DWORD)VertexStreamZeroStride;

		//give the correct token enum here, and it's done.
		Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_DrawVerticesUP, 4);//argCount, not necessary, default to 14

		EmuKickOff();

		while (WaitForPGRAPH)
			;
		//pgraph_use_NV2A_Kelvin();
			//g_nv2a_use_Kelvin = true;
		CxbxrImpl_DrawVerticesUP(PrimitiveType, VertexCount, pVertexStreamZeroData, VertexStreamZeroStride);
	}
}


// LTCG specific D3DDevice_DrawVerticesUP function...
// This uses a custom calling convention where pVertexStreamZeroData is passed in EBX
// Test-case: NASCAR Heat 20002
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_DrawVerticesUP_12__LTCG_ebx3)
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    uint_xt             VertexCount,
    uint_xt             VertexStreamZeroStride
)
{
    PVOID pVertexStreamZeroData;
    __asm {
        LTCG_PROLOGUE
        mov  pVertexStreamZeroData, ebx
    }
	//trampoline draw call if we're recording pushbuffer
	if (g_pXbox_BeginPush_Buffer != nullptr) {
		XB_TRMP(D3DDevice_DrawVerticesUP_12__LTCG_ebx3)(PrimitiveType, VertexCount, VertexStreamZeroStride);
	}
	else {
		bool WaitForPGRAPH;
		WaitForPGRAPH = true;
		//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
		PBTokenArray[2] = (DWORD)&WaitForPGRAPH;// (DWORD)PrimitiveType;
		PBTokenArray[3] = (DWORD)VertexCount;
		PBTokenArray[4] = (DWORD)pVertexStreamZeroData;
		PBTokenArray[5] = (DWORD)VertexStreamZeroStride;

		//give the correct token enum here, and it's done.
		Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_DrawVerticesUP_12__LTCG_ebx3, 4);//argCount, not necessary, default to 14

		EmuKickOff();

		while (WaitForPGRAPH)
			;
		// We can't call emupatch() here because the target function calls trampoline which might not be available in guest code.
		//EMUPATCH(D3DDevice_DrawVerticesUP)(PrimitiveType, VertexCount, pVertexStreamZeroData, VertexStreamZeroStride);
		//CxbxrImpl_DrawVerticesUP(PrimitiveType, VertexCount, pVertexStreamZeroData, VertexStreamZeroStride);
		D3DDevice_DrawVerticesUP(PrimitiveType, VertexCount, pVertexStreamZeroData, VertexStreamZeroStride);
	}
    __asm {
        LTCG_EPILOGUE
        ret  0Ch
    }
}
void WINAPI CxbxrImpl_DrawIndexedVertices
(
	xbox::X_D3DPRIMITIVETYPE  PrimitiveType,
	xbox::uint_xt             VertexCount,
	CONST PWORD         pIndexData
)
{
	if (!IsValidXboxVertexCount(PrimitiveType, VertexCount)) {
		LOG_TEST_CASE("Invalid VertexCount");
		return;
	}

	// TODO : Call unpatched CDevice_SetStateVB[_8](g_Xbox_BaseVertexIndex);
	// we update the resources in pgraph handler
	//CxbxUpdateNativeD3DResources();

	CxbxDrawContext DrawContext = {};

	DrawContext.XboxPrimitiveType = PrimitiveType;
	DrawContext.dwVertexCount = VertexCount;
	DrawContext.dwBaseVertexIndex = g_Xbox_BaseVertexIndex; // Multiplied by vertex stride and added to the vertex buffer start
	DrawContext.pXboxIndexData = pIndexData; // Used to derive VerticesInBuffer

	// Test case JSRF draws all geometry through this function (only sparks are drawn via another method)
	// using X_D3DPT_TRIANGLELIST and X_D3DPT_TRIANGLESTRIP PrimitiveType
	CxbxDrawIndexed(DrawContext);

	CxbxHandleXboxCallbacks();
}
// ******************************************************************
// * patch: D3DDevice_DrawIndexedVertices
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_DrawIndexedVertices)
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    uint_xt             VertexCount,
    CONST PWORD         pIndexData
)
{
	// Test-cases : XDK samples (Cartoon, Gamepad)
	// Note : In gamepad.xbe, the gamepad is drawn by D3DDevice_DrawIndexedVertices
	// Dxbx Note : In DrawVertices and DrawIndexedVertices, PrimitiveType may not be D3DPT_POLYGON

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(PrimitiveType)
		LOG_FUNC_ARG(VertexCount)
		LOG_FUNC_ARG(pIndexData)
		LOG_FUNC_END;
	//trampoline draw call if we're recording pushbuffer
	if (g_pXbox_BeginPush_Buffer != nullptr) {
		XB_TRMP(D3DDevice_DrawIndexedVertices)(PrimitiveType, VertexCount, pIndexData);
	}
	else {
		bool WaitForPGRAPH;
		WaitForPGRAPH = true;
		//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
		PBTokenArray[2] = (DWORD)&WaitForPGRAPH;// (DWORD)PrimitiveType;
		PBTokenArray[3] = (DWORD)VertexCount;
		PBTokenArray[4] = (DWORD)pIndexData;

		//give the correct token enum here, and it's done.
		Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_DrawIndexedVertices, 3);//argCount, not necessary, default to 14

		EmuKickOff();

		while (WaitForPGRAPH)
			;

		CxbxrImpl_DrawIndexedVertices(PrimitiveType, VertexCount, pIndexData);
	}
}

void WINAPI CxbxrImpl_DrawIndexedVerticesUP
(
	xbox::X_D3DPRIMITIVETYPE  PrimitiveType,
	xbox::uint_xt                VertexCount,
	CONST PVOID         pIndexData,
	CONST PVOID         pVertexStreamZeroData,
	xbox::uint_xt                VertexStreamZeroStride
)
{
	if (!IsValidXboxVertexCount(PrimitiveType, VertexCount)) {
		LOG_TEST_CASE("Invalid VertexCount");
		return;
	}

	// TODO : Call unpatched CDevice_SetStateUP();
	EmuKickOffWait();
	CxbxUpdateNativeD3DResources();

	CxbxDrawContext DrawContext = {};
	INDEX16* pXboxIndexData = (INDEX16*)pIndexData;

	DrawContext.XboxPrimitiveType = PrimitiveType;
	DrawContext.dwVertexCount = VertexCount;
	DrawContext.pXboxIndexData = pXboxIndexData; // Used to derive VerticesInBuffer
	// Note : D3DDevice_DrawIndexedVerticesUP does NOT use g_Xbox_BaseVertexIndex, so keep DrawContext.dwBaseVertexIndex at 0!
	DrawContext.pXboxVertexStreamZeroData = pVertexStreamZeroData;
	DrawContext.uiXboxVertexStreamZeroStride = VertexStreamZeroStride;

	// Determine LowIndex and HighIndex *before* VerticesInBuffer gets derived
	WalkIndexBuffer(DrawContext.LowIndex, DrawContext.HighIndex, pXboxIndexData, VertexCount);

	VertexBufferConverter.Apply(&DrawContext);

	INDEX16* pHostIndexData;
	UINT PrimitiveCount = DrawContext.dwHostPrimitiveCount;

	bool bConvertQuadListToTriangleList = (DrawContext.XboxPrimitiveType == xbox::X_D3DPT_QUADLIST);
	if (bConvertQuadListToTriangleList) {
		LOG_TEST_CASE("X_D3DPT_QUADLIST");
		// Test-case : Buffy: The Vampire Slayer
		// Test-case : XDK samples : FastLoad, BackBufferScale, DisplacementMap, Donuts3D, VolumeLight, PersistDisplay, PolynomialTextureMaps, SwapCallback, Tiling, VolumeFog, DebugKeyboard, Gamepad
		// Convert draw arguments from quads to triangles :
		pHostIndexData = CxbxCreateQuadListToTriangleListIndexData(pXboxIndexData, VertexCount);

		// Note, that LowIndex and HighIndex won't change due to this quad-to-triangle conversion,
		// so it's less work to WalkIndexBuffer over the input instead of the converted index buffer.
	}
	else {
		// LOG_TEST_CASE("DrawIndexedPrimitiveUP"); // Test-case : Burnout, Namco Museum 50th Anniversary
		pHostIndexData = pXboxIndexData;
	}

	HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitiveUP(
		/*PrimitiveType=*/EmuXB2PC_D3DPrimitiveType(DrawContext.XboxPrimitiveType),
		/*MinVertexIndex=*/DrawContext.LowIndex,
		/*NumVertexIndices=*/(DrawContext.HighIndex - DrawContext.LowIndex) + 1,
		PrimitiveCount,
		pHostIndexData,
		/*IndexDataFormat=*/D3DFMT_INDEX16,
		DrawContext.pHostVertexStreamZeroData,
		DrawContext.uiHostVertexStreamZeroStride
	);
	//DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitiveUP");

	if (bConvertQuadListToTriangleList) {
		CxbxReleaseQuadListToTriangleListIndexData(pHostIndexData);
	}

	g_dwPrimPerFrame += PrimitiveCount;
	if (DrawContext.XboxPrimitiveType == xbox::X_D3DPT_LINELOOP) {
		// Close line-loops using a final single line, drawn from the end to the start vertex
		LOG_TEST_CASE("X_D3DPT_LINELOOP"); // TODO : Which titles reach this test-case?
		// Read the end and start index from the supplied index data
		INDEX16 LowIndex = pXboxIndexData[0];
		INDEX16 HighIndex = pXboxIndexData[DrawContext.dwHostPrimitiveCount];
		// If needed, swap so highest index is higher than lowest (duh)
		if (HighIndex < LowIndex) {
			std::swap(HighIndex, LowIndex);
		}

		// Close line-loops using a final single line, drawn from the end to the start vertex :
		CxbxDrawIndexedClosingLineUP(
			LowIndex,
			HighIndex,
			DrawContext.pHostVertexStreamZeroData,
			DrawContext.uiHostVertexStreamZeroStride
		);
	}

	CxbxHandleXboxCallbacks();
}
// ******************************************************************
// * patch: D3DDevice_DrawIndexedVerticesUP
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_DrawIndexedVerticesUP)
(
    X_D3DPRIMITIVETYPE  PrimitiveType,
    uint_xt                VertexCount,
    CONST PVOID         pIndexData,
    CONST PVOID         pVertexStreamZeroData,
    uint_xt                VertexStreamZeroStride
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(PrimitiveType)
		LOG_FUNC_ARG(VertexCount)
		LOG_FUNC_ARG(pIndexData)
		LOG_FUNC_ARG(pVertexStreamZeroData)
		LOG_FUNC_ARG(VertexStreamZeroStride)
		LOG_FUNC_END;
	//trampoline draw call if we're recording pushbuffer
	if (g_pXbox_BeginPush_Buffer != nullptr) {
		XB_TRMP(D3DDevice_DrawIndexedVerticesUP)(PrimitiveType, VertexCount, pIndexData, pVertexStreamZeroData, VertexStreamZeroStride);
	}
	else {
		bool WaitForPGRAPH;
		WaitForPGRAPH = true;
		//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
		PBTokenArray[2] = (DWORD)&WaitForPGRAPH;// (DWORD)PrimitiveType;
		PBTokenArray[3] = (DWORD)VertexCount;
		PBTokenArray[4] = (DWORD)pIndexData;

		//give the correct token enum here, and it's done.
		Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_DrawIndexedVerticesUP, 3);//argCount, not necessary, default to 14

		EmuKickOff();

		while (WaitForPGRAPH)
			;

		CxbxrImpl_DrawIndexedVerticesUP(PrimitiveType, VertexCount, pIndexData, pVertexStreamZeroData, VertexStreamZeroStride);
	}
}

xbox::hresult_xt WINAPI CxbxrImpl_SetLight
(
	xbox::dword_xt            Index,
	CONST xbox::X_D3DLIGHT8* pLight
	)
{
	d3d8LightState.Lights[Index] = *pLight;

	//HRESULT hRet = g_pD3DDevice->SetLight(Index, pLight);
	return S_OK;//hRet;
}

// ******************************************************************
// * patch: D3DDevice_SetLight
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_SetLight)
(
    dword_xt            Index,
    CONST X_D3DLIGHT8 *pLight
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Index)
		LOG_FUNC_ARG(pLight)
		LOG_FUNC_END;

	XB_TRMP(D3DDevice_SetLight)(Index, pLight);

	// special case: passing in whole *pLight to pushbuffer to prevent content of *pLight being altered before we reach CxbxrImpl_SetLight()
	// both xbox api and HLE patch copy *pLight and preserve the variable.
	// init pushbuffer related pointers
	DWORD* pPush_local = (DWORD*)*g_pXbox_pPush;         //pointer to current pushbuffer
	DWORD* pPush_limit = (DWORD*)*g_pXbox_pPushLimit;    //pointer to the end of current pushbuffer
	if ((unsigned int)pPush_local + 128 >= (unsigned int)pPush_limit)//check if we still have enough space
		pPush_local = (DWORD*)CxbxrImpl_MakeSpace(); //make new pushbuffer space and get the pointer to it.
	pPush_local[0] = HLE_API_PUSHBFFER_COMMAND_128;
	// process xbox D3D API enum and arguments and push them to pushbuffer for pgraph to handle later.
	pPush_local[1] = (DWORD)X_D3DAPI_ENUM::X_D3DDevice_SetLight;
	
	pPush_local[2] = (DWORD)Index;
	pPush_local[3] = (DWORD)&pPush_local[4];
	*(X_D3DLIGHT8*)pPush_local[3] = *pLight;
	//set pushbuffer pointer to the new beginning
	// always reserve 1 command DWORD, 1 API enum, and 14 argmenet DWORDs.
	*(DWORD**)g_pXbox_pPush += 0x20;

	//HRESULT hRet = CxbxrImpl_SetLight(Index, pLight);

	//DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetLight");    

	return S_OK;//hRet;
}
xbox::hresult_xt WINAPI CxbxrImpl_SetMaterial
(
	CONST xbox::X_D3DMATERIAL8* pMaterial
)
{
	ffShaderState.Materials[0].Ambient = toVector(pMaterial->Ambient);
	ffShaderState.Materials[0].Diffuse = toVector(pMaterial->Diffuse);
	ffShaderState.Materials[0].Specular = toVector(pMaterial->Specular);
	ffShaderState.Materials[0].Emissive = toVector(pMaterial->Emissive);
	ffShaderState.Materials[0].Power = pMaterial->Power;

	HRESULT hRet = g_pD3DDevice->SetMaterial(pMaterial);
	return hRet;
}
// ******************************************************************
// * patch: D3DDevice_SetMaterial
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetMaterial)
(
    CONST X_D3DMATERIAL8 *pMaterial
)
{
	LOG_FUNC_ONE_ARG(pMaterial);
	XB_TRMP(D3DDevice_SetMaterial)(pMaterial);
	//HRESULT hRet = CxbxrImpl_SetMaterial(pMaterial);
	HRESULT hRet = S_OK;

	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)pMaterial;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetMaterial, 1, PBTokenArray);//argCount 14
	
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetMaterial");
}
xbox::hresult_xt WINAPI CxbxrImpl_LightEnable
(
	xbox::dword_xt            Index,
	xbox::bool_xt             bEnable
	)
{

	d3d8LightState.EnableLight(Index, bEnable);

	HRESULT hRet = g_pD3DDevice->LightEnable(Index, bEnable);
	//DEBUG_D3DRESULT(hRet, "g_pD3DDevice->LightEnable");
	return hRet;
}
// ******************************************************************
// * patch: D3DDevice_LightEnable
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_LightEnable)
(
    dword_xt            Index,
    bool_xt             bEnable
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Index)
		LOG_FUNC_ARG(bEnable)
		LOG_FUNC_END;

	XB_TRMP(D3DDevice_LightEnable)(Index, bEnable);

	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)Index;
	PBTokenArray[3] = (DWORD)bEnable;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_LightEnable, 2);//argCount, not necessary, default to 14

	//HRESULT hRet = CxbxrImpl_LightEnable(Index, bEnable);
	//

	return S_OK;//hRet;
}

// SetRenderTarget can call CommonSetRenderTarget, nested call detection is required
// Test case: Midtown Madness 3
static thread_local uint32_t setRenderTargetCount = 0;
//create cache key of xbox texture which contents the render target surface. and get host container resource of host surface set to that xbox render target surface,
//then set host container resource to xbox texture key. this is in case any render target (including render target and depth buffer) being used as texture input in texture stage.
//by creating the texture in cache and setup the correct host container resource, the data will be linked.
void CxbxrImpl_SetRenderTargetTexture(xbox::X_D3DSurface* pXborSurface, IDirect3DSurface* pHostSurface, D3DSURFACE_DESC HostSurfaceDesc)
{
	LOG_INIT;
	if (pXborSurface != nullptr) {
		//if there is not parent texture of xbox surface, then we create a dummy texture for it and put it into cache.
		if(pXborSurface->Parent==nullptr){
			IDirect3DTexture9* pHostTexture;
			xbox::X_D3DTexture XboxTexture, * pXboxTexture;
			XboxTexture = *(xbox::X_D3DTexture*)pXborSurface;
			pXboxTexture = &XboxTexture;
			//set the common to resource type texture.
			XboxTexture.Common = (pXborSurface->Common & 0xFFF8FFFF) | 0x00040000;
			//insert the dummy texture key and texture resource to texture cache for D3DDevice_SetTexture()
			UINT64 key = ((UINT64)(pXboxTexture->Format) << 32) | pXboxTexture->Data;
			auto it = g_TextureCache.find(key);
			g_TextureCache[key] = (xbox::X_D3DSurface)XboxTexture;
			g_TextureCache[key].Parent = nullptr;
			//pHostTexture = CxbxConvertXboxSurfaceToHostTexture(pXboxTexture);
			auto hRet = pHostSurface->GetContainer(IID_PPV_ARGS(&pHostTexture));
			DEBUG_D3DRESULT(hRet, "CxbxrImpl_SetRenderTargetTexture: pHostSurface->GetContainer");
			if (FAILED(hRet)) {
				LOG_TEST_CASE("CxbxrImpl_SetRenderTargetTexture: Failed to get Texture from Surface");
			}else{
				auto key = GetHostResourceKey(pXboxTexture);
				auto& ResourceCache = GetResourceCache(key);
				auto it = ResourceCache.find(key);
				//insert the key to get a new item in resource cache without host resource and PCFormt set to it.
				//ResourceCache.insert(key);
				auto& resourceInfo = ResourceCache[key];	// Implicitely inserts a new entry if not already existing
				resourceInfo.pHostResource = pHostTexture;
				resourceInfo.dwXboxResourceType = GetXboxCommonResourceType(pXboxTexture);
				resourceInfo.pXboxData = GetDataFromXboxResource(pXboxTexture);
				resourceInfo.szXboxDataSize = GetXboxResourceSize(pXboxTexture);
				resourceInfo.hash = ComputeHash(resourceInfo.pXboxData, resourceInfo.szXboxDataSize);
				resourceInfo.hashLifeTime = 1ms;
				resourceInfo.lastUpdate = std::chrono::steady_clock::now();
				resourceInfo.nextHashTime = resourceInfo.lastUpdate + resourceInfo.hashLifeTime;
				resourceInfo.forceRehash = false;
				resourceInfo.PCFormat = HostSurfaceDesc.Format;

				SetHostResource(pXborSurface, pHostSurface);
			}
		}
		// there is parent texture, check if parent texture is pure texure, if so, put parent texture into cache and set host resource to it.
        else {
			IDirect3DTexture9* pHostTexture;
			xbox::X_D3DTexture* pXboxTexture;
			pXboxTexture = (xbox::X_D3DTexture*)pXborSurface->Parent;
			bool bCubemap = pXboxTexture->Format & X_D3DFORMAT_CUBEMAP;
			if ((pXboxTexture->Common & X_D3DCOMMON_TYPE_MASK) == X_D3DCOMMON_TYPE_TEXTURE
				&& !bCubemap) {
				//insert the dummy texture key and texture resource to texture cache for D3DDevice_SetTexture()
				UINT64 key = ((UINT64)(pXboxTexture->Format) << 32) | pXboxTexture->Data;
				auto it = g_TextureCache.find(key);
				g_TextureCache[key] = *(xbox::X_D3DSurface*)pXboxTexture;
				g_TextureCache[key].Parent = nullptr;
				//pHostTexture = CxbxConvertXboxSurfaceToHostTexture(pXboxTexture);
				auto hRet = pHostSurface->GetContainer(IID_PPV_ARGS(&pHostTexture));
				DEBUG_D3DRESULT(hRet, "CxbxrImpl_SetRenderTargetTexture: pHostSurface->GetContainer");
				if (FAILED(hRet)) {
					LOG_TEST_CASE("CxbxrImpl_SetRenderTargetTexture: Failed to get Texture from Surface");
				}
				else {
					auto key = GetHostResourceKey(pXboxTexture);
					auto& ResourceCache = GetResourceCache(key);
					auto it = ResourceCache.find(key);
					//insert the key to get a new item in resource cache without host resource and PCFormt set to it.
					//ResourceCache.insert(key);
					auto& resourceInfo = ResourceCache[key];	// Implicitely inserts a new entry if not already existing
					resourceInfo.pHostResource = pHostTexture;
					resourceInfo.dwXboxResourceType = GetXboxCommonResourceType(pXboxTexture);
					resourceInfo.pXboxData = GetDataFromXboxResource(pXboxTexture);
					resourceInfo.szXboxDataSize = GetXboxResourceSize(pXboxTexture);
					resourceInfo.hash = ComputeHash(resourceInfo.pXboxData, resourceInfo.szXboxDataSize);
					resourceInfo.hashLifeTime = 1ms;
					resourceInfo.lastUpdate = std::chrono::steady_clock::now();
					resourceInfo.nextHashTime = resourceInfo.lastUpdate + resourceInfo.hashLifeTime;
					resourceInfo.forceRehash = false;
					resourceInfo.PCFormat = HostSurfaceDesc.Format;
				}
			}
		}
	}
}

void EmuSetDefaultTile(int index, xbox::X_D3DSurface* pSurface)
{
	UINT32 key = pSurface->Data & 0x0FFFFFFF;
	g_BufferSurfaceCache[key] = *pSurface;
	xbox::X_D3DFORMAT X_Format = GetXboxPixelContainerFormat((xbox::X_D3DPixelContainer*)pSurface);
	//bool bDepthBuffer = EmuXBFormatIsDepthBuffer(X_Format);
	//bool bSwizzled = EmuXBFormatIsSwizzled(X_Format);
	bool bCompressed = EmuXBFormatIsCompressed(X_Format);
	UINT dwBPP = EmuXBFormatBytesPerPixel(X_Format);
	//UINT dwMipMapLevels = CxbxGetPixelContainerMipMapLevels(pSurface);
	UINT xboxWidth, xboxHeight, dwDepth, dwRowPitch, dwSlicePitch;
	//D3DFORMAT PCFormat = EmuXB2PC_D3DFormat((xbox::X_D3DFORMAT)X_Format);
	// Interpret Width/Height/BPP
	CxbxGetPixelContainerMeasures(pSurface, 0, &xboxWidth, &xboxHeight, &dwDepth, &dwRowPitch, &dwSlicePitch);

	if (index == 0) {
		g_TileSurface[0] = *pSurface;
		g_Tile[0].Flags = 0;
		g_Tile[0].Pitch = dwRowPitch;
		g_Tile[0].pMemory = (PVOID)(pSurface->Data | 0x80000000);
		g_Tile[0].Size = dwSlicePitch;
		g_Tile[0].ZOffset = 0;
		g_Tile[0].ZStartTag =0 ;
	}
	else {
		g_TileSurface[1] = *pSurface;
		g_Tile[1].Flags = 0x00000001;
		if (bCompressed)
			g_Tile[1].Flags |= 0x80000000;
		if (dwBPP==4)
			g_Tile[1].Flags |= 0x04000000;
		g_Tile[1].Pitch = dwRowPitch;
		g_Tile[1].pMemory = (PVOID)(pSurface->Data | 0x80000000);
		g_Tile[1].Size = dwSlicePitch;
		g_Tile[1].ZOffset = 0;
		g_Tile[1].ZStartTag = 0;
	}
}


void EmuSetRenderTarget
(
	xbox::X_D3DSurface* pRenderTarget
)
{
	LOG_INIT;
	IDirect3DSurface* pHostRenderTarget = nullptr;
	// whenever SetRenderTarget was called, we insert the previous render target surface to render target cache. every surface in the cache is dirty.
	// when we compose vertex stream in NV2A, we check whether the vertex stream is withing the data range of these dirty render targets. if a match was found, we transfer data from host to xbox for the matched render target and remove it from cache.
	if (g_pXbox_RenderTarget != nullptr) {
		UINT64 key = ((UINT64)(g_pXbox_RenderTarget->Format) << 32) | g_pXbox_RenderTarget->Data;
		g_RenderTargetCache[key] = *g_pXbox_RenderTarget;
	}

	// In Xbox titles, CreateDevice calls SetRenderTarget for the back buffer
	// We can use this to determine the Xbox backbuffer surface for later use!
	if (g_pXbox_BackBufferSurface == xbox::zeroptr && pRenderTarget != nullptr) {
		g_Xbox_BackBufferSurface = *pRenderTarget;
		g_pXbox_BackBufferSurface = pRenderTarget;
		//set default tile 0 for default back buffer
		EmuSetDefaultTile(0, pRenderTarget);
		// TODO : Some titles might render to another backbuffer later on,
		// if that happens, we might need to skip the first one or two calls?
	}

	// The current render target is only replaced if it's passed in here non-null
	if (pRenderTarget != xbox::zeroptr) {
		g_Xbox_RenderTarget = *pRenderTarget;
		g_pXbox_RenderTarget = pRenderTarget;
	}
	else {
		// If non is given, use the current Xbox render target
		pRenderTarget = g_pXbox_RenderTarget;
		// If there's no Xbox render target yet, fallback to the Xbox back buffer
		if (pRenderTarget == xbox::zeroptr) {
			LOG_TEST_CASE("SetRenderTarget fallback to backbuffer");
			pRenderTarget = g_pXbox_BackBufferSurface;
		}
	}

	// Set default viewport now we've updated the rendertarget
	// Note the Xbox does this, but before _our_ SetRenderTarget sets up the render target
	// Test case: Dashboard
	static xbox::X_D3DVIEWPORT8 defaultViewport;
	defaultViewport.X = 0;
	defaultViewport.Y = 0;
	defaultViewport.Width = INT_MAX;
	defaultViewport.Height = INT_MAX;
	defaultViewport.MinZ = 0;
	defaultViewport.MaxZ = 1;
	CxbxrImpl_SetViewport(&defaultViewport);

	pHostRenderTarget = GetHostSurface(pRenderTarget, D3DUSAGE_RENDERTARGET);
	if (pHostRenderTarget != nullptr) {
		// Get current host render target dimensions
		D3DSURFACE_DESC HostRenderTarget_Desc;
		pHostRenderTarget->GetDesc(&HostRenderTarget_Desc);
		// create resource cache for containers of both xbox and host surfaces and link two containers together.
		CxbxrImpl_SetRenderTargetTexture(pRenderTarget, pHostRenderTarget, HostRenderTarget_Desc);
	}

	HRESULT hRet;

	// Mimick Direct3D 8 SetRenderTarget by only setting render target if non-null
	if (pHostRenderTarget) {
		hRet = g_pD3DDevice->SetRenderTarget(/*RenderTargetIndex=*/0, pHostRenderTarget);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetRenderTarget");
		if (FAILED(hRet)) {
			// If Direct3D 9 SetRenderTarget failed, skip setting depth stencil
			return;
		}
	}

	// Validate that our host render target is still the correct size
	DWORD HostRenderTarget_Width, HostRenderTarget_Height;
	if (GetHostRenderTargetDimensions(&HostRenderTarget_Width, &HostRenderTarget_Height, pHostRenderTarget)) {
		DWORD XboxRenderTarget_Width = GetPixelContainerWidth(g_pXbox_RenderTarget);
		DWORD XboxRenderTarget_Height = GetPixelContainerHeight(g_pXbox_RenderTarget);
		ValidateRenderTargetDimensions(HostRenderTarget_Width, HostRenderTarget_Height, XboxRenderTarget_Width, XboxRenderTarget_Height);
	}
}

void EmuSetDepthStencil
(
    xbox::X_D3DSurface    *pNewZStencil
)
{
	LOG_INIT;
	//IDirect3DSurface *pHostDepthStencil = nullptr;
	IDirect3DSurface* pHostDepthStencilSurface = nullptr;
	IDirect3DTexture* pHostDepthStencilTexture = nullptr;

	// In Xbox titles, CreateDevice calls SetRenderTarget (our caller) for the depth stencil
	// We can use this to determine the Xbox depth stencil surface for later use!
    if (g_pXbox_DefaultDepthStencilSurface == xbox::zeroptr && pNewZStencil != nullptr) {
		g_Xbox_DefaultDepthStencilSurface = *pNewZStencil;
		g_pXbox_DefaultDepthStencilSurface = pNewZStencil;
		//set default tile 1 for default depth stencil
		EmuSetDefaultTile(1, pNewZStencil);
		// TODO : Some titles might set another depth stencil later on,
		// if that happens, we might need to skip the first one or two calls?
    }

	// The currenct depth stencil is always replaced by whats passed in here (even a null)
	
	if (pNewZStencil != nullptr) {
		g_Xbox_DepthStencil = *pNewZStencil;
		g_pXbox_DepthStencil = pNewZStencil;
	}
	else {
		g_pXbox_DepthStencil = nullptr;
	}

	g_ZScale = GetZScaleForPixelContainer(g_pXbox_DepthStencil); // TODO : Discern between Xbox and host and do this in UpdateDepthStencilFlags?

	HRESULT hRet;

	if (g_pXbox_DepthStencil != nullptr) {
		UINT64 key = ((UINT64)(g_pXbox_DepthStencil->Format) << 32) | g_pXbox_DepthStencil->Data;
		auto it = g_DepthBufferCache.find(key);
		if (it != g_DepthBufferCache.end()) {
			//pHostDepthStencil = g_DepthBufferCache[key].pHostDepthStencil;
			pHostDepthStencilSurface = g_DepthBufferCache[key].pHostDepthStencilTextureSurface;
		}else{
			xbox::X_D3DFORMAT X_Format=GetXboxPixelContainerFormat((xbox::X_D3DPixelContainer*)g_pXbox_DepthStencil);
			bool bDepthBuffer = EmuXBFormatIsDepthBuffer(X_Format);
			bool bSwizzled = EmuXBFormatIsSwizzled(X_Format);
			bool bCompressed = EmuXBFormatIsCompressed(X_Format);
			UINT dwBPP = EmuXBFormatBytesPerPixel(X_Format);
			UINT dwMipMapLevels = CxbxGetPixelContainerMipMapLevels(g_pXbox_DepthStencil);
			UINT xboxWidth, xboxHeight, dwDepth, dwRowPitch, dwSlicePitch;
			D3DFORMAT PCFormat = EmuXB2PC_D3DFormat((xbox::X_D3DFORMAT)X_Format);

			// Interpret Width/Height/BPP
			CxbxGetPixelContainerMeasures(g_pXbox_DepthStencil, 0, &xboxWidth, &xboxHeight, &dwDepth, &dwRowPitch, &dwSlicePitch);

			//CreateHostResource(g_pXbox_DepthStencil, D3DUSAGE_DEPTHSTENCIL);
			//hack: forcing PCFormat to be the host format we specified
			PCFormat = (D3DFORMAT)MAKEFOURCC('I', 'N', 'T', 'Z');
			//PCFormat = (D3DFORMAT)MAKEFOURCC('R', '3', '2', 'F');
			//PCFormat = D3DFMT_D16_LOCKABLE;//D3DFMT_D32_LOCKABLE, D3DFMT_D16_LOCKABLE, D3DFMT_D32F_LOCKABLE, D3DFMT_S8_LOCKABLE

			hRet = g_pD3DDevice->CreateTexture(xboxWidth, xboxHeight,
				1, // Levels
				D3DUSAGE_DEPTHSTENCIL, // D3DUSAGE_DYNAMIC
				PCFormat,
				D3DPOOL_DEFAULT, // D3DPOOL_DEFAULT D3DPOOL_SYSTEMMEM
				&pHostDepthStencilTexture,
				nullptr // pSharedHandle
			);

			if (hRet == S_OK) {
				hRet = pHostDepthStencilTexture->GetSurfaceLevel(0, &pHostDepthStencilSurface);
			}

			/*
			hRet = g_pD3DDevice->CreateDepthStencilSurface(xboxWidth, xboxHeight, PCFormat,
				g_EmuCDPD.HostPresentationParameters.MultiSampleType,
				0, // MultisampleQuality
				false, // Discard
				&pHostDepthStencil,
				nullptr // pSharedHandle
			);
			*/
			if (hRet == S_OK) {
				UINT64 key = ((UINT64)(g_pXbox_DepthStencil->Format) << 32) | g_pXbox_DepthStencil->Data;
				//g_DepthBufferCache[key].pHostDepthStencil = pHostDepthStencil;
				g_DepthBufferCache[key].XboxDepthBufferSurface = *g_pXbox_DepthStencil;
				g_DepthBufferCache[key].pHostDepthStencilTextureSurface = pHostDepthStencilSurface;
				g_DepthBufferCache[key].pHostDepthStencilTexture = pHostDepthStencilTexture;
			}
			
			if (pHostDepthStencilSurface != nullptr) {
				// Get current host render target dimensions
				D3DSURFACE_DESC HostDepthStencilSurface_Desc;
				hRet = pHostDepthStencilSurface->GetDesc(&HostDepthStencilSurface_Desc);
				CxbxrImpl_SetRenderTargetTexture(g_pXbox_DepthStencil, pHostDepthStencilSurface, HostDepthStencilSurface_Desc);
			}
		}
	}

	hRet = g_pD3DDevice->SetDepthStencilSurface(pHostDepthStencilSurface);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetDepthStencilSurface");

	if (SUCCEEDED(hRet)) {
		// Once we're sure the host depth-stencil is activated...
		UpdateDepthStencilFlags(pHostDepthStencilSurface);
	}
}

void CxbxrImpl_SetRenderTarget
(
	xbox::X_D3DSurface* pRenderTarget,
	xbox::X_D3DSurface* pNewZStencil
)
{
	LOG_INIT;
	NestedPatchCounter call(setRenderTargetCount);
	EmuSetRenderTarget(pRenderTarget);
	EmuSetDepthStencil(pNewZStencil);
}

ULONG CxbxrImpl_Resource_AddRef
(
	xbox::X_D3DResource* pResource
)
{
	if (pResource == nullptr)
		return 0L;
	// If the resource being referenced is a surface then we also need
	// to increment the refcount on its parent.  We only do this once.
	if ((pResource->Common & X_D3DCOMMON_REFCOUNT_MASK) == 0)
	{
		if ((pResource->Common & X_D3DCOMMON_TYPE_MASK) == X_D3DCOMMON_TYPE_SURFACE)
		{
			xbox::X_D3DBaseTexture* pParent = ((xbox::X_D3DBaseTexture**)pResource)[5];

			if (pParent)
			{
				CxbxrImpl_Resource_AddRef(pParent);
			}
		}
	}

	return ++pResource->Common & X_D3DCOMMON_REFCOUNT_MASK;
}

ULONG CxbxrImpl_Resource_Release
(
	xbox::X_D3DResource* pResource
)
{

    if (pResource == nullptr)
		return 0L;
	if ((pResource->Common & X_D3DCOMMON_REFCOUNT_MASK) == 1)
	{
		// If this is the last external release from a surface then we need
		// to release its external reference on the parent.
		//
		if ((pResource->Common & X_D3DCOMMON_TYPE_MASK) == X_D3DCOMMON_TYPE_SURFACE)
		{
			xbox::X_D3DBaseTexture* pParent = ((xbox::X_D3DBaseTexture**)pResource)[5];

			if (pParent)
			{
				CxbxrImpl_Resource_Release(pParent);
			}
		}

		// Only destroy this if the internal refcount is zero.
		if ((pResource->Common & X_D3DCOMMON_INTREFCOUNT_MASK) == 0)
		{
			// Release the host copy (if it exists!)
			FreeHostResource(GetHostResourceKey(pResource));

			return 0;
		}
	}

	return --pResource->Common & X_D3DCOMMON_REFCOUNT_MASK;
}

void CxbxrImpl_ReleaseRenderTarget(xbox::X_D3DSurface* pTarget, xbox::X_D3DSurface* pZbuffer)
{
	
	if (pTarget)
		CxbxrImpl_Resource_Release(pTarget);
	if (pZbuffer)
		CxbxrImpl_Resource_Release(pZbuffer);
}

static bool bRenderTargetInit = false;
// ******************************************************************
// * patch: D3DDevice_SetRenderTarget
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetRenderTarget)
(
    X_D3DSurface    *pRenderTarget,
    X_D3DSurface    *pNewZStencil
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pRenderTarget)
		LOG_FUNC_ARG(pNewZStencil)
		LOG_FUNC_END;
	//move to CxbxrImpl_SetRenderTarget()
	//NestedPatchCounter call(setRenderTargetCount);

	XB_TRMP(D3DDevice_SetRenderTarget)(pRenderTarget, pNewZStencil);
	if (!bRenderTargetInit) {
		bRenderTargetInit = true;
		CxbxrImpl_SetRenderTarget(pRenderTarget, pNewZStencil);
	}
	else {
		// init pushbuffer related pointers
		DWORD* pPush_local = (DWORD*)*g_pXbox_pPush;         //pointer to current pushbuffer
		DWORD* pPush_limit = (DWORD*)*g_pXbox_pPushLimit;    //pointer to the end of current pushbuffer
		if ((unsigned int)pPush_local + 64 >= (unsigned int)pPush_limit)//check if we still have enough space
			pPush_local = (DWORD*)CxbxrImpl_MakeSpace(); //make new pushbuffer space and get the pointer to it.

		// process xbox D3D API enum and arguments and push them to pushbuffer for pgraph to handle later.
		pPush_local[0] = HLE_API_PUSHBFFER_COMMAND_64;
		pPush_local[1] = (DWORD)X_D3DDevice_SetRenderTarget;//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
		if (pRenderTarget) {
			pPush_local[2] = (DWORD)pRenderTarget;
			CxbxrImpl_Resource_AddRef(pRenderTarget);
		}
		else {
			pPush_local[2] = 0;
		}

		if (pNewZStencil) {
			pPush_local[3] = (DWORD)pNewZStencil;
			CxbxrImpl_Resource_AddRef(pNewZStencil);
		}
		else {
			pPush_local[3] = 0;
		}
		//set pushbuffer pointer to the new beginning, HLE_API_PUSHBFFER_COMMAND_128 requires 0x20 dwords offset to pPush.
		*(DWORD**)g_pXbox_pPush += 0x10;
	}
}

// LTCG specific D3DDevice_SetRenderTarget function...
// Passes pRenderTarget in ecx and pNewZStencil in eax
static void D3DDevice_SetRenderTarget_0
(
    xbox::X_D3DSurface    *pRenderTarget,
    xbox::X_D3DSurface    *pNewZStencil
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pRenderTarget)
		LOG_FUNC_ARG(pNewZStencil)
		LOG_FUNC_END;


	//
}

__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetRenderTarget_0)
(
)
{
	X_D3DSurface *pRenderTarget;
    X_D3DSurface *pNewZStencil;
	__asm {
		LTCG_PROLOGUE
		mov  pRenderTarget, ecx
		mov  pNewZStencil, eax
	}
	//this must be called before we trampolin to avoid HLE D3D_CommonSetRenderTarget() being called twice
	//move to CxbxrImpl_SetRenderTarget()
	//NestedPatchCounter call(setRenderTargetCount);
	// logging only
	D3DDevice_SetRenderTarget_0(pRenderTarget, pNewZStencil);
	//CxbxrImpl_SetRenderTarget(pRenderTarget, pNewZStencil);
	if (!bRenderTargetInit) {
		bRenderTargetInit = true;
		CxbxrImpl_SetRenderTarget(pRenderTarget, pNewZStencil);
	}
	else {
		//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
		PBTokenArray[2] = (DWORD)pRenderTarget;
		PBTokenArray[3] = (DWORD)pNewZStencil;
		//give the correct token enum here, and it's done.
		Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetRenderTarget_0, 2, PBTokenArray);//argCount 14
		// add reference to the surfaces to prevent them being released before we access them in pgraph.
		if (pRenderTarget)
			CxbxrImpl_Resource_AddRef(pRenderTarget);
		if (pNewZStencil)
			CxbxrImpl_Resource_AddRef(pNewZStencil);
	}

	__asm {
		mov  ecx, pRenderTarget
		mov  eax, pNewZStencil
		call XB_TRMP(D3DDevice_SetRenderTarget_0)
		LTCG_EPILOGUE
		ret
	}
}
void WINAPI CxbxrImpl_D3D_CommonSetRenderTarget
(
	xbox::X_D3DSurface* pRenderTarget,
	xbox::X_D3DSurface* pNewZStencil,
	void* unknown
)
{
	NestedPatchCounter call(setRenderTargetCount);

	if (call.GetLevel() == 0) {
		CxbxrImpl_SetRenderTarget(pRenderTarget, pNewZStencil);
	}
}
// ******************************************************************
// * patch: D3D_CommonSetRenderTarget
// ******************************************************************
// This is an internal function, but some LTCG games inline SetRenderTarget and call it directly
// Test-case: Midtown Madness 3
xbox::void_xt WINAPI xbox::EMUPATCH(D3D_CommonSetRenderTarget)
(
    X_D3DSurface    *pRenderTarget,
    X_D3DSurface    *pNewZStencil,
    void            *unknown
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pRenderTarget)
		LOG_FUNC_ARG(pNewZStencil)
		LOG_FUNC_ARG(unknown)
		LOG_FUNC_END;
	//move to CxbxrImpl_D3D_CommonSetRenderTarget()
	//NestedPatchCounter call(setRenderTargetCount);

	//CxbxrImpl_D3D_CommonSetRenderTarget(pRenderTarget,pNewZStencil,unknown);
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)pRenderTarget;
	PBTokenArray[3] = (DWORD)pNewZStencil;
	PBTokenArray[4] = (DWORD)unknown;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3D_CommonSetRenderTarget, 3, PBTokenArray);//argCount 14
	// add reference to the surfaces to prevent them being released before we access them in pgraph.
	if (pRenderTarget)
		CxbxrImpl_Resource_AddRef(pRenderTarget);
	if (pNewZStencil)
		CxbxrImpl_Resource_AddRef(pNewZStencil);
    //for variants of SetRenderTarget(), we need to setup HLE to pgraph token first in order to make sure the execution ordre of NestedPatchCounter call(setRenderTargetCount);
	XB_TRMP(D3D_CommonSetRenderTarget)(pRenderTarget, pNewZStencil, unknown);

}
//NV097_SET_TEXTURE_PALETTE(Stage) //pPalette->Data | (pPalette->Common >> D3DPALETTE_COMMON_PALETTESET_SHIFT) & D3DPALETTE_COMMON_PALETTESET_MASK
void CxbxrImpl_SetPalette
(
    xbox::dword_xt      Stage,
    xbox::X_D3DPalette *pPalette
)
{
	if (Stage >= xbox::X_D3DTS_STAGECOUNT) {
		LOG_TEST_CASE("Stage out of bounds");
	} else {
		// Note : Actual update of paletized textures (X_D3DFMT_P8) happens in CxbxUpdateHostTextures!
		g_pXbox_Palette_Data[Stage] = GetDataFromXboxResource(pPalette);
		g_Xbox_Palette_Size[Stage] = pPalette ? XboxD3DPaletteSizeToBytes(GetXboxPaletteSize(pPalette)) : 0;
	}
}

// Overload for logging
static void D3DDevice_SetPalette_4
(
	xbox::dword_xt      Stage,
	xbox::X_D3DPalette *pPalette
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Stage)
		LOG_FUNC_ARG(pPalette)
		LOG_FUNC_END;
}

// LTCG specific D3DDevice_SetPalette function...
// This uses a custom calling convention where Stage parameter is passed in EAX
// Test-case: Ninja Gaiden
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetPalette_4)
(
	X_D3DPalette *pPalette
)
{
	dword_xt Stage;
	__asm {
		LTCG_PROLOGUE
		mov  Stage, eax
	}

	// Log
	D3DDevice_SetPalette_4(Stage, pPalette);

	// Call the Xbox implementation of this function, to properly handle reference counting for us
	__asm {
		push pPalette
		mov  eax, Stage
		call XB_TRMP(D3DDevice_SetPalette_4)
	}

	//CxbxrImpl_SetPalette(Stage, pPalette);

	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)Stage;
	PBTokenArray[3] = (DWORD)pPalette;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetPalette_4, 2, PBTokenArray);//argCount 14

	__asm {
		LTCG_EPILOGUE
		ret  4
	}
}

// ******************************************************************
// * patch: D3DDevice_SetPalette
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetPalette)
(
	dword_xt      Stage,
	X_D3DPalette *pPalette
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Stage)
		LOG_FUNC_ARG(pPalette)
		LOG_FUNC_END;

	// Call the Xbox implementation of this function, to properly handle reference counting for us
	XB_TRMP(D3DDevice_SetPalette)(Stage, pPalette);

	//CxbxrImpl_SetPalette(Stage, pPalette);

	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)Stage;
	PBTokenArray[3] = (DWORD)pPalette;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetPalette, 2, PBTokenArray);//argCount 14
}

// LTCG specific D3DDevice_SetFlickerFilter function...
// This uses a custom calling convention where parameter is passed in ESI
// Test-case: Metal Wolf Chaos
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetFlickerFilter_0)
(
)
{
	dword_xt Filter;
	__asm {
		LTCG_PROLOGUE
		mov  Filter, esi
		// always trampoline with HLE patches
		call XB_TRMP(D3DDevice_SetFlickerFilter_0)
		//EMUPATCH(D3DDevice_SetFlickerFilter)(Filter);
	}

	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)Filter;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetFlickerFilter, 1);//argCount, not necessary, default to 14

	__asm {
		LTCG_EPILOGUE
		ret
	}
}

// ******************************************************************
// * patch: D3DDevice_SetFlickerFilter
// ******************************************************************
// test case: XDK sample BumpEarth
void WINAPI xbox::EMUPATCH(D3DDevice_SetFlickerFilter)
(
    dword_xt         Filter
)
{
	LOG_FUNC_ONE_ARG(Filter);
	// always trampoline with HLE patches
	XB_TRMP(D3DDevice_SetFlickerFilter)(Filter);

	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)Filter;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetFlickerFilter, 1);//argCount, not necessary, default to 14

	LOG_IGNORED();
}

// ******************************************************************
// * patch: D3DDevice_SetSoftDisplayFilter
// ******************************************************************
void WINAPI xbox::EMUPATCH(D3DDevice_SetSoftDisplayFilter)
(
    bool_xt Enable
)
{
	LOG_FUNC_ONE_ARG(Enable);
	XB_TRMP(D3DDevice_SetSoftDisplayFilter)(Enable);
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)Enable;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetSoftDisplayFilter, 1);//argCount, not necessary, default to 14
	LOG_IGNORED();
}

// Overload for logging
static void D3DDevice_DeleteVertexShader_0
(
	xbox::dword_xt Handle
)
{
	LOG_FUNC_ONE_ARG(Handle);
}

// LTCG specific D3DDevice_DeleteVertexShader function...
// This uses a custom calling convention where parameter is passed in EAX
// Test-case: Midtown Madness 3
__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_DeleteVertexShader_0)
(
)
{
	dword_xt Handle;
	__asm {
		LTCG_PROLOGUE
		mov  Handle, eax
	}

	// Log
	D3DDevice_DeleteVertexShader_0(Handle);

	CxbxrImpl_DeleteVertexShader(Handle);

		// When deleting, call trampoline *after* our implementation,
		// so that we can still access it's fields before it gets deleted!
		__asm {
			mov  eax, Handle
			call XB_TRMP(D3DDevice_DeleteVertexShader_0)

			LTCG_EPILOGUE
		ret
	}
}

// ******************************************************************
// * patch: D3DDevice_DeleteVertexShader
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_DeleteVertexShader)
(
	dword_xt Handle
)
{
	LOG_FUNC_ONE_ARG(Handle);

	CxbxrImpl_DeleteVertexShader(Handle);

	// When deleting, call trampoline *after* our implementation,
	// so that we can still access it's fields before it gets deleted!
	XB_TRMP(D3DDevice_DeleteVertexShader)(Handle);
}



// ******************************************************************
// * patch: D3DDevice_GetShaderConstantMode
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_GetShaderConstantMode)
(
    dword_xt *pMode
)
{
	LOG_FUNC_ONE_ARG(pMode);
        
    if(pMode)
    {
        *pMode = g_Xbox_VertexShaderConstantMode;
    }
}

// ******************************************************************
// * patch: D3DDevice_GetVertexShader
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_GetVertexShader)
(
    dword_xt *pHandle
)
{
	LOG_FUNC_ONE_ARG(pHandle);

    if(pHandle)
    {
        (*pHandle) = g_Xbox_VertexShader_Handle;
    }
}

// ******************************************************************
// * patch: D3DDevice_GetVertexShaderConstant
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_GetVertexShaderConstant)
(
    int_xt   Register,
    void  *pConstantData,
    dword_xt ConstantCount
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Register)
		LOG_FUNC_ARG(pConstantData)
		LOG_FUNC_ARG(ConstantCount)
		LOG_FUNC_END;

	// Xbox vertex shader constants range from -96 to 95
	// The host does not support negative, so we adjust to 0..191
	Register += X_D3DSCM_CORRECTION;
	/*
	HRESULT hRet = g_pD3DDevice->GetVertexShaderConstantF
    (
        Register,
        (float*)pConstantData, // TODO : Validate this work correctly under D3D9
        ConstantCount
    );
	*/
	// read Vertex Shader constants from nv2a
	//extern float* HLE_get_NV2A_vertex_constant_float4_ptr(unsigned const_index); // TMP glue
	float* constant_floats = HLE_get_NV2A_vertex_constant_float4_ptr(Register);
	memcpy(pConstantData, constant_floats, ConstantCount * sizeof(float) * 4);


	//DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetVertexShaderConstant");
}

// ******************************************************************
// * patch: D3DDevice_SetVertexShaderInput
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetVertexShaderInput)
(
    dword_xt              Handle,
    uint_xt               StreamCount,
    X_STREAMINPUT     *pStreamInputs
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Handle)
		LOG_FUNC_ARG(StreamCount)
		LOG_FUNC_ARG(pStreamInputs)
		LOG_FUNC_END;

	// Call trampoline
	XB_TRMP(D3DDevice_SetVertexShaderInput)(Handle, StreamCount, pStreamInputs);

	// When this API is in effect, VertexBuffers as set by Xbox SetStreamSource are disregarded,
	// instead, the pStreamInputs[].VertexBuffer streams are used.

	// If Handle is NULL, all VertexShader input state is cleared (after which the VertexBuffers as set by SetStreamSource are used once again).

	// Otherwise, Handle is the address of an Xbox VertexShader struct, or-ed with 1 (X_D3DFVF_RESERVED0)
	// The given pStreamInputs are stored in a global array, and the NV2A is programmed to read
	// each vertex attribute (as defined in the given VertexShader.VertexAttribute.Slots[]) to read
	// the attribute data from the pStreamInputs[slot].VertexBuffer + pStreamInputs[slot].Offset + VertexShader.VertexAttribute.Slots[slot].Offset

	/* LOG_TEST_CASE("SetVertexShaderInput");
	/* Test-cases :
		PushBuffer XDK sample
		Halo 2-3ebe4439.ini:D3DDevice_SetVertexShaderInput = 0x3f7440
		Kung Fu Chaos-d9ab292c.ini:D3DDevice_SetVertexShaderInput = 0x2bc0e0
		NBA LIVE 2005-71d4eeb1.ini:D3DDevice_SetVertexShaderInput = 0x5cf810
		NBA LIVE 2005-71d4eeb1.ini:D3DDevice_SetVertexShaderInputDirect = 0x5ceba0
		Prince of Persia WW-4ccf7369.ini:D3DDevice_SetVertexShaderInput = 0x494830
		Prince of Persia WW-4ccf7369.ini:D3DDevice_SetVertexShaderInputDirect = 0x494280
		Spyro A Hero's Tail-b18e00e5.ini:D3DDevice_SetVertexShaderInput = 0x286cf0
		Spyro A Hero's Tail-b18e00e5.ini:D3DDevice_SetVertexShaderInputDirect = 0x286760
	*/

	//CxbxrImpl_SetVertexShaderInput(Handle, StreamCount, pStreamInputs);

	 //fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)Handle;
	PBTokenArray[3] = (DWORD)StreamCount;
	PBTokenArray[4] = (DWORD)pStreamInputs;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetVertexShaderInput, 3);//argCount, not necessary, default to 14
}

extern xbox::dword_xt* GetCxbxVertexShaderSlotPtr(const DWORD SlotIndexAddress); // tmp glue

void WINAPI CxbxrImpl_RunVertexStateShader(
	xbox::dword_xt Address,
	CONST xbox::float_xt* pData)
{
	NV2AState* dev = g_NV2A->GetDeviceState();
	PGRAPHState* pg = &(dev->pgraph);

	Nv2aVshProgram program = {}; // Note: This nulls program.steps
	// TODO : Retain program globally and perform nv2a_vsh_parse_program only when
	//        the address-range we're about to emulate was modified since last parse.
	// TODO : As a suggestion for this, parse all NV2A_MAX_TRANSFORM_PROGRAM_LENGTH slots,
	//        and here just point program.steps to global vsh_program_steps[Address].
	Nv2aVshParseResult result = nv2a_vsh_parse_program(
		&program, // Note : program.steps will be malloc'ed
		GetCxbxVertexShaderSlotPtr(Address), // TODO : At some point, use pg->program_data[Address] here instead
		NV2A_MAX_TRANSFORM_PROGRAM_LENGTH - Address);
	if (result != NV2AVPR_SUCCESS) {
		LOG_TEST_CASE("nv2a_vsh_parse_program failed");
		// TODO : Dump Nv2aVshParseResult as string and program for debugging purposes
		return;
	}

	Nv2aVshCPUXVSSExecutionState state_linkage;
	Nv2aVshExecutionState state = nv2a_vsh_emu_initialize_xss_execution_state(
		&state_linkage, (float*)pg->vsh_constants); // Note : This wil memset(state_linkage, 0)
	if (pData != nullptr)
		//if pData != nullptr, then it contains v0.xyzw, we shall copy the binary content directly.
		memcpy(state_linkage.input_regs, pData, sizeof(state_linkage.input_regs));

	nv2a_vsh_emu_execute_track_context_writes(&state, &program, pg->vsh_constants_dirty);
	// Note: Above emulation's primary purpose is to update pg->vsh_constants and pg->vsh_constants_dirty
	// therefor, nothing else needs to be done here, other than to cleanup

	nv2a_vsh_program_destroy(&program); // Note: program.steps will be free'ed

}

// ******************************************************************
// * patch: D3DDevice_RunVertexStateShader
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_RunVertexStateShader)
(
    dword_xt Address,
    CONST float_xt *pData
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Address)
		LOG_FUNC_ARG(pData)
		LOG_FUNC_END;

	// If pData is assigned, pData[0..3] is pushed towards nv2a transform data registers
	// then sends the nv2a a command to launch the vertex shader function located at Address
	if (Address >= NV2A_MAX_TRANSFORM_PROGRAM_LENGTH) {
		LOG_TEST_CASE("Address out of bounds");
		return;
	}
	// make sure pushbuffer is processed.
	// CxbxrImpl_RunVertexStateShader(Address,pData);

	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)Address;
	if (pData != nullptr) {
		PBTokenArray[3] = (DWORD)&PBTokenArray[4];
		memcpy(&PBTokenArray[4], pData, sizeof(float) * 4);
	}
	else {
		PBTokenArray[3]= (DWORD)nullptr;
	}
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_RunVertexStateShader, 6, PBTokenArray);//argCount 14
}

// ******************************************************************
// * patch: D3DDevice_LoadVertexShaderProgram
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_LoadVertexShaderProgram)
(
	CONST dword_xt *pFunction,
	dword_xt        Address
	)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pFunction)
		LOG_FUNC_ARG(Address)
		LOG_FUNC_END;
	if (is_pushbuffer_recording()) {
		XB_TRMP(D3DDevice_LoadVertexShaderProgram)(pFunction, Address);
	}
#if !USEPGRAPH_LoadVertexShaderProgram
	CxbxrImpl_LoadVertexShaderProgram((DWORD *)pFunction, Address);
#else
	// init pushbuffer related pointers
	DWORD* pPush_local = (DWORD*)*g_pXbox_pPush;         //pointer to current pushbuffer
	DWORD* pPush_limit = (DWORD*)*g_pXbox_pPushLimit;    //pointer to the end of current pushbuffer
	if ((unsigned int)pPush_local + 64 >= (unsigned int)pPush_limit)//check if we still have enough space
		pPush_local = (DWORD*)CxbxrImpl_MakeSpace(); //make new pushbuffer space and get the pointer to it.

	// process xbox D3D API enum and arguments and push them to pushbuffer for pgraph to handle later.
	pPush_local[0] = HLE_API_PUSHBFFER_COMMAND_64 ;
	pPush_local[1] = X_D3DAPI_ENUM::X_D3DDevice_LoadVertexShaderProgram;//enum of this patched API
	pPush_local[2] = (DWORD)pFunction; //total 14 DWORD space for arguments.
	pPush_local[3] = (DWORD)Address;

	//set pushbuffer pointer to the new beginning
	// always reserve 1 command DWORD, 1 API enum, and 14 argmenet DWORDs.
	*(DWORD**)g_pXbox_pPush += 0x10;
#endif
}

void WINAPI CxbxrImpl_SetDepthClipPlanes
(
	xbox::float_xt Near,
	xbox::float_xt Far,
	xbox::dword_xt Flags
)
{
	HRESULT hRet = D3D_OK;

	switch (Flags) // Member of X_D3DSET_DEPTH_CLIP_PLANES_FLAGS enum
	{
	case xbox::X_D3DSDCP_SET_VERTEXPROGRAM_PLANES:
	{
		// Sets the depth-clipping planes used whenever vertex shader programs are active
		// TODO

		// pDevice->fNear = Near
		// pDevice->fFar  = Far
	}
	break;

	case xbox::X_D3DSDCP_SET_FIXEDFUNCTION_PLANES:
	{
		// Sets the depth-clipping planes used whenever the fixed-function pipeline is in use. 
		// TODO

		// pDevice->fNear = Near
		// pDevice->fFar  = Far
	}
	break;
	//Near and Far values passed in will be ignored in this case
	case xbox::X_D3DSDCP_USE_DEFAULT_VERTEXPROGRAM_PLANES:
	{
		// Causes Direct3D to disregard the depth-clipping planes set when using X_D3DSDCP_SET_VERTEXPROGRAM_PLANE. 
		// Direct3D will resume using its own internally calculated clip planes when vertex shader programs are active. 
		// TODO
	}
	break;
	//Near and Far values passed in will be ignored in this case
	case xbox::X_D3DSDCP_USE_DEFAULT_FIXEDFUNCTION_PLANES:
	{
		// Causes Direct3D to disregard the depth-clipping planes set when using X_D3DSDCP_SET_FIXEDFUNCTION_PLANES. 
		// Direct3D will resume using its own internally calculated clip planes when the fixed-function pipeline is active.
		// TODO
	}
	break;

	default:
		EmuLog(LOG_LEVEL::WARNING, "Unknown SetDepthClipPlanes Flags provided");
	}

	// TODO
}
// ******************************************************************
// * patch: D3DDevice_SetDepthClipPlanes
// ******************************************************************
// test cases: Tak 2_ The Staff Of Dreams
xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_SetDepthClipPlanes)
(
    float_xt Near,
    float_xt Far,
    dword_xt Flags
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Near)
		LOG_FUNC_ARG(Far)
		LOG_FUNC_ARG(Flags)
		LOG_FUNC_END;

    HRESULT hRet = D3D_OK;

	//CxbxrImpl_SetDepthClipPlanes(Near, Far, Flags);

	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = FtoDW(Near);
	PBTokenArray[3] = FtoDW(Far),
	PBTokenArray[4] = (DWORD)Flags;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetDepthClipPlanes, 3, PBTokenArray);//argCount 14

	return hRet;
}

// ******************************************************************
// * patch: D3DDevice_InsertFence
// ******************************************************************
xbox::dword_xt WINAPI xbox::EMUPATCH(D3DDevice_InsertFence)()
{
	LOG_FUNC();

    // TODO: Actually implement this
    dword_xt dwRet = 0x8000BEEF;

	LOG_UNIMPLEMENTED();

    return dwRet;
}

// ******************************************************************
// * patch: D3DDevice_IsFencePending
// ******************************************************************
xbox::bool_xt WINAPI xbox::EMUPATCH(D3DDevice_IsFencePending)
(
    dword_xt Fence
)
{
	LOG_FUNC_ONE_ARG(Fence);

	// TODO: Implement
	LOG_UNIMPLEMENTED();

	return FALSE;
}

// ******************************************************************
// * patch: D3DDevice_BlockOnFence
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_BlockOnFence)
(
    dword_xt Fence
)
{
	LOG_FUNC_ONE_ARG(Fence);

    // TODO: Implement
	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * patch: IDirect3DResource8_BlockUntilNotBusy
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DResource_BlockUntilNotBusy)
(
    X_D3DResource *pThis
)
{
	LOG_FUNC_ONE_ARG(pThis);

    // TODO: Implement
	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * patch: D3DDevice_SetScreenSpaceOffset
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetScreenSpaceOffset)
(
    float_xt x,
    float_xt y
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(x)
		LOG_FUNC_ARG(y)
		LOG_FUNC_END;
	XB_TRMP(D3DDevice_SetScreenSpaceOffset)(x, y);
	//CxbxrImpl_SetScreenSpaceOffset(x, y);

	// init pushbuffer related pointers
	DWORD* pPush_local = (DWORD*)*g_pXbox_pPush;         //pointer to current pushbuffer
	DWORD* pPush_limit = (DWORD*)*g_pXbox_pPushLimit;    //pointer to the end of current pushbuffer
	if ((unsigned int)pPush_local + 64 >= (unsigned int)pPush_limit)//check if we still have enough space
		pPush_local = (DWORD*)CxbxrImpl_MakeSpace(); //make new pushbuffer space and get the pointer to it.

	// process xbox D3D API enum and arguments and push them to pushbuffer for pgraph to handle later.
	pPush_local[0] = HLE_API_PUSHBFFER_COMMAND_64 ;
	pPush_local[1] = X_D3DAPI_ENUM::X_D3DDevice_SetScreenSpaceOffset;//enum of this patched API
	pPush_local[2] = FtoDW(x); //total 14 DWORD space for arguments.
	pPush_local[3] = FtoDW(y);

	//set pushbuffer pointer to the new beginning
	// always reserve 1 command DWORD, 1 API enum, and 14 argmenet DWORDs.
	*(DWORD**)g_pXbox_pPush += 0x10;
}

// ******************************************************************
// * patch: D3DDevice_InsertCallback
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_InsertCallback)
(
	X_D3DCALLBACKTYPE	Type,
	X_D3DCALLBACK		pCallback,
	dword_xt				Context
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Type)
		LOG_FUNC_ARG(pCallback)
		LOG_FUNC_ARG(Context)
		LOG_FUNC_END;

	CxbxrImpl_InsertCallback(Type, pCallback, Context);

	LOG_INCOMPLETE();
}

extern void pgraph_notuse_NV2A_Kelvin(void);
extern void pgraph_use_NV2A_Kelvin(void);
unsigned int MaxRectPatchVertexCount = 0;
std::map<uint32_t, D3DRECTPATCH_INFO> g_RectPatchInfoCache;
std::map<uint32_t, D3DTRIPATCH_INFO>  g_TriPatchInfoCache;
// global flag true: using DrawRectPatch. false: converting patches to triangle stripes.
bool g_bUseDrawPatch = false;
INDEX16 IndexedPatchVertex[0x250];
static inline int GetIndex(int x, int y, int stride) { return x + y * stride; }
xbox::hresult_xt WINAPI CxbxrImpl_DrawRectPatch
(
	xbox::uint_xt					Handle,
	CONST xbox::float_xt* pNumSegs,
	CONST D3DRECTPATCH_INFO* pRectPatchInfo
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Handle)
		LOG_FUNC_ARG(pNumSegs)
		LOG_FUNC_ARG(pRectPatchInfo)
		LOG_FUNC_END;

	pgraph_use_NV2A_Kelvin();
	NV2AState* d = g_NV2A->GetDeviceState();
	D3D_draw_state_update(d);
	
	//setup DrawContext in order to call VertexBufferConverter.Apply() to setup stream source and vertex buffer
	CxbxDrawContext DrawContext = {};
	D3DRECTPATCH_INFO tmpRectPatchInfo;
	if (pRectPatchInfo != nullptr) {
		g_RectPatchInfoCache.insert(std::pair<uint32_t, D3DRECTPATCH_INFO>(Handle, *pRectPatchInfo));
	}
	else {
		//d3d9 seems unhappy when pRectPatchInfo==nullptr
		auto it = g_RectPatchInfoCache.find(Handle);
		tmpRectPatchInfo = it->second;
		pRectPatchInfo = &tmpRectPatchInfo;
	}

	DrawContext.XboxPrimitiveType = xbox::X_D3DPRIMITIVETYPE::X_D3DPT_TRIANGLESTRIP;
	/*

	illustration of a 4X3 RectPatch
  StartVertexOffsetWidth :2
	  |<----->|
	 V0  V1   V2  V3  V4           ---
			--------------       | StartVertexOffsetHeight:1
	 V5  V6 | V7  V8  V9 |   ---   ---
							 /|\
	V10 V11 |V12 V13 V14 |    |
							  | Height:4
	V15 V16 |V17 V18 V19 |    |
							 \|/
	V20 V21 |V22 V23 V24 |   ---
			--------------
			  |<----->|  Width:3

	 |<-------------->|  Stride:5

	 start vertex index V7 = StartVertexOffsetWidth + StartVertexOffsetHeight * Stride; 2 + 1*5 =7

	 for any given vertex in position x, y, assuming origin is on top left corner, x positive to right, y positive to down.
	 the index of given vertex is x + y * Stride

	 give vertex V7, (x,y)=(2,1), index = 2 + 1* 5 =7

	 vertex index in upper row is index(V7) - Stride = 7 - 5 = 2
	 vertex index in lower roW is index(V7) + stride = 7 + 5 = 12
	 vertex index in right is     index(V7) + 1 = 8
	 vertex index in left is      index(V7) - 1 = 6

	 we're going to create a index buffer to conver the 4X3 RectPatch into 3 triangle strips.

	 V5  V6  V7   V8   V9
			  |\  |\  |
			  | \ | \ |
			  |  \|  \|
	V10 V11  V12 V13 V14
			  |\  |\  |
			  | \ | \ |
			  |  \|  \|
	V15 V16  V17 V18 V19
			  |\  |\  |
			  | \ | \ |
			  |  \|  \|
	V20 V21  V22 V23 V24

	testcase: Patch sample uses 4X4 RectPatch
	testcase: True Crime Streets of LA ?

	*/

	// g_bUseDrawPatch is global flag which we can dynamically switch between using DrawRectPatch or converting patches to triangle stripes.
	HRESULT hRet = S_OK;
	if (!g_bUseDrawPatch) {
		int index = 0;
		int stride = pRectPatchInfo->Stride;
		int originY = pRectPatchInfo->StartVertexOffsetHeight;
		int originX = pRectPatchInfo->StartVertexOffsetWidth;
		int y;
		int StartVertexIndex = pRectPatchInfo->StartVertexOffsetHeight * pRectPatchInfo->Stride + pRectPatchInfo->StartVertexOffsetWidth;
		for (int deltaY = 1; deltaY < (pRectPatchInfo->Height); deltaY++) {
			y = originY + deltaY;
			int x;
			for (int deltaX = 0; deltaX < (pRectPatchInfo->Width); deltaX++) {
				x = originX + deltaX;
				/*
						  (x,y-1)(x+1,y-1)
					 V5  V6  V7    V8   V9
							  |\   |\   |
							  | \  | \  |
							  |  \ |  \ |
							  |   \|   \|
					V10 V11  V12  V13  V14
							(x,y) (x+1,y)
				1st loop: V12,V7,
				2nd loop: V13,V8,
				3rd loop: V14,V9
				*/
				IndexedPatchVertex[index++] = GetIndex(x, y, stride);
				IndexedPatchVertex[index++] = GetIndex(x, y - 1, stride);
			}
		}
		DrawContext.dwVertexCount = index;
		DrawContext.pXboxIndexData = IndexedPatchVertex;
		DrawContext.dwStartVertex = 0;
		CxbxDrawIndexed(DrawContext);
	}
	else {
		DrawContext.dwVertexCount = (pRectPatchInfo->StartVertexOffsetHeight + pRectPatchInfo->Height) * pRectPatchInfo->Stride;
		//keep max. vertex count so we can use same vertex count in DrawContext which will result in using the same vertex buffer cache.
		if (MaxRectPatchVertexCount < DrawContext.dwVertexCount)MaxRectPatchVertexCount = DrawContext.dwVertexCount;
		DrawContext.dwVertexCount = MaxRectPatchVertexCount;
		VertexBufferConverter.Apply(&DrawContext);
		hRet = g_pD3DDevice->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawRectPatch");
	}
	pgraph_notuse_NV2A_Kelvin();
	return hRet;
}

// ******************************************************************
// * patch: D3DDevice_DrawRectPatch
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_DrawRectPatch)
(
	uint_xt					Handle,
	CONST float_xt				*pNumSegs,
	CONST D3DRECTPATCH_INFO *pRectPatchInfo
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Handle)
		LOG_FUNC_ARG(pNumSegs)
		LOG_FUNC_ARG(pRectPatchInfo)
		LOG_FUNC_END;

	bool WaitForPGRAPH;
	WaitForPGRAPH = true;
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)&WaitForPGRAPH;// (DWORD)pPresentationParameters;

	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_DrawRectPatch, 1, PBTokenArray);//argCount, not necessary, default to 14

	EmuKickOff();

	while (WaitForPGRAPH)
		;

	HRESULT hRet = CxbxrImpl_DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
	return hRet;
}

// ******************************************************************
// * patch: D3DDevice_DrawTriPatch
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_DrawTriPatch)
(
	uint_xt					Handle,
	CONST float_xt				*pNumSegs,
	CONST D3DTRIPATCH_INFO* pTriPatchInfo
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Handle)
		LOG_FUNC_ARG(pNumSegs)
		LOG_FUNC_ARG(pTriPatchInfo)
		LOG_FUNC_END;

	bool WaitForPGRAPH;
	WaitForPGRAPH = true;
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)&WaitForPGRAPH;// (DWORD)pPresentationParameters;

	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_DrawTriPatch, 1, PBTokenArray);//argCount, not necessary, default to 14

	EmuKickOff();

	while (WaitForPGRAPH)
		;
	//resource update call is called via pgraph token handler
	//CxbxUpdateNativeD3DResources();

	HRESULT hRet = g_pD3DDevice->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawTriPatch");

	return hRet;
}

#pragma warning(disable:4244)
// ******************************************************************
// * patch: D3DDevice_GetProjectionViewportMatrix
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_GetProjectionViewportMatrix)
(
	D3DXMATRIX *pProjectionViewport
)
{
	LOG_FUNC_ONE_ARG(pProjectionViewport);

	// blueshogun96 1/25/10
	// It's been almost 3 years, but I think this is a better 
	// implementation.  Still probably not right, but better
	// then before.

	HRESULT hRet;
	D3DXMATRIX Out, mtxProjection, mtxViewport;
	D3DVIEWPORT Viewport;

	// Get current viewport
	hRet = g_pD3DDevice->GetViewport(&Viewport);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetViewport - Unable to get viewport!");

	// Get current projection matrix
	hRet = g_pD3DDevice->GetTransform(D3DTS_PROJECTION, &mtxProjection);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetTransform - Unable to get projection matrix!");

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
}
#pragma warning(default:4244)

// ******************************************************************
// * patch: CDevice_SetStateVB (D3D::CDevice::SetStateVB)
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(CDevice_SetStateVB)(ulong_xt Unknown1)
{
	addr_xt _this;
	__asm mov _this, ecx;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(_this)
		LOG_FUNC_ARG(Unknown1)
		LOG_FUNC_END;

	// TODO: Anything?
//	__asm int 3;

	LOG_UNIMPLEMENTED();
}

xbox::void_xt WINAPI xbox::EMUPATCH(CDevice_SetStateVB_8)(addr_xt _this, ulong_xt Unknown1)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(_this)
		LOG_FUNC_ARG(Unknown1)
		LOG_FUNC_END;

	// TODO: Anything?
//	__asm int 3;

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * patch: CDevice_SetStateUP (D3D::CDevice::SetStateUP)
// ******************************************************************
xbox::void_xt CxbxrImpl_CDevice_SetStateUP(xbox::addr_xt _this)
{
	LOG_UNIMPLEMENTED();

	// TODO: Anything?
	//__asm int 3;
}

xbox::void_xt WINAPI xbox::EMUPATCH(CDevice_SetStateUP)()
{
	addr_xt _this;
	__asm mov _this, ecx;

	LOG_FUNC_ONE_ARG(_this);

	CxbxrImpl_CDevice_SetStateUP(_this);
}

xbox::void_xt WINAPI xbox::EMUPATCH(CDevice_SetStateUP_4)(xbox::addr_xt _this)
{
	LOG_FUNC_ONE_ARG(_this);

	CxbxrImpl_CDevice_SetStateUP(_this);
}
static void CDevice_SetStateUP_0__LTCG_esi1(xbox::addr_xt _this)
{
	LOG_FUNC_ONE_ARG(_this);
}

__declspec(naked) xbox::void_xt WINAPI xbox::EMUPATCH(CDevice_SetStateUP_0__LTCG_esi1)()
{
	addr_xt _this;
	__asm {
		LTCG_PROLOGUE
		mov  _this, esi
	}

	// Log
	CDevice_SetStateUP_0__LTCG_esi1(_this);

	CxbxrImpl_CDevice_SetStateUP(_this);

	__asm {
		LTCG_EPILOGUE
		ret
	}
}

// ******************************************************************
// * patch: D3DDevice_SetStipple
// ******************************************************************
void WINAPI xbox::EMUPATCH(D3DDevice_SetStipple)( dword_xt* pPattern )
{
	LOG_FUNC_ONE_ARG(pPattern);

	// We need an OpenGL port... badly

	LOG_IGNORED();
}

// ******************************************************************
// * patch: D3DDevice_SetSwapCallback
// ******************************************************************
void WINAPI xbox::EMUPATCH(D3DDevice_SetSwapCallback)
(
	X_D3DSWAPCALLBACK		pCallback
)
{
	LOG_FUNC_ONE_ARG(pCallback);

    g_pXbox_SwapCallback = pCallback;
}

xbox::X_D3DBaseTexture xboxPersistTexture = { 0 };
//xbox::X_D3DSurface xboxPersistSurface = { 0 };
extern xbox::PVOID xbox::AvSavedDataAddress;
extern BOOL EmuXBFormatIsBumpMap(xbox::X_D3DFORMAT Format);
extern bool g_bForceHostARGBConversion;
xbox::X_D3DSurface* CxbxrImpl_GetAvSavedDataSurface()
{
	LOG_INIT
	//we can't do anything is AvSavedDataAddress is not set yet.
	if (xbox::AvSavedDataAddress == nullptr)
		return nullptr;
	//now we copy the host surface data in the buffer to the surface AvSetSavedDataAddress pointing to so it can persist after rebooting.
	xbox::X_D3DSurface* pPersistSurface = (xbox::X_D3DSurface*)xbox::AvSavedDataAddress;
	if (xboxPersistTexture.Data != NULL)
		return (xbox::X_D3DSurface*)xbox::AvSavedDataAddress;
	IDirect3DSurface* pHostPersistSurface;
	pHostPersistSurface = GetHostSurface(pPersistSurface, D3DUSAGE_RENDERTARGET);
	//copy AVSavedAddress common and modify it then set to our own persist texture/surface
	xboxPersistTexture = *(xbox::X_D3DBaseTexture*)xbox::AvSavedDataAddress;
	xboxPersistTexture.Common &= 0xFFFEFFFF;

	// Interpret Width/Height/BPP
	xbox::X_D3DPixelContainer* pPixelContainer = (xbox::X_D3DPixelContainer*)pPersistSurface;
	xbox::X_D3DFORMAT X_Format = GetXboxPixelContainerFormat(pPixelContainer);
	D3DFORMAT PCFormat = EmuXB2PC_D3DFormat(X_Format);
	if (EmuXBFormatIsBumpMap(X_Format))PCFormat = D3DFMT_A8R8G8B8;
	bool bCubemap = pPixelContainer->Format & X_D3DFORMAT_CUBEMAP;
	bool bSwizzled = EmuXBFormatIsSwizzled(X_Format);
	bool bCompressed = EmuXBFormatIsCompressed(X_Format);
	UINT dwBPP = EmuXBFormatBytesPerPixel(X_Format);
	UINT dwMipMapLevels = CxbxGetPixelContainerMipMapLevels(pPixelContainer);
	UINT xboxWidth, xboxHeight, dwDepth, dwRowPitch, dwSlicePitch;
	// Interpret Width/Height/BPP
	CxbxGetPixelContainerMeasures(pPixelContainer, 0, &xboxWidth, &xboxHeight, &dwDepth, &dwRowPitch, &dwSlicePitch);
	// Host width and height dimensions
	UINT hostWidth = xboxWidth;
	UINT hostHeight = xboxHeight;
	D3DSURFACE_DESC HostAVSavedSurdaceDesc;
	pHostPersistSurface->GetDesc(&HostAVSavedSurdaceDesc);
	IDirect3DTexture* pTmpHostTexture;
	IDirect3DSurface* pTmpHostSurface;
	HRESULT hRet = g_pD3DDevice->CreateTexture(HostAVSavedSurdaceDesc.Width, HostAVSavedSurdaceDesc.Height,
		1, // Levels
		D3DUSAGE_DYNAMIC, // Usage always as render target D3DUSAGE_DYNAMIC
		HostAVSavedSurdaceDesc.Format,
		D3DPOOL_SYSTEMMEM, // D3DPOOL_DEFAULT D3DPOOL_SYSTEMMEM
		&pTmpHostTexture,
		nullptr // pSharedHandle
	);
	DEBUG_D3DRESULT(hRet, "D3DDevice_GetPersistedSurface::CreateTexture");
	D3DLOCKED_RECT TmpLockedRect;
	if (hRet == D3D_OK) {
		hRet = pTmpHostTexture->GetSurfaceLevel(0, &pTmpHostSurface);
		DEBUG_D3DRESULT(hRet, "D3DDevice_GetPersistedSurface::pNewHostSurface");
		hRet = pTmpHostSurface->LockRect(&TmpLockedRect, NULL, D3DLOCK_NOSYSLOCK);
		if (hRet != D3D_OK) {
			EmuLog(LOG_LEVEL::WARNING, "Could not lock Host Surface for Xbox texture in D3DDevice_GetPersistedSurface");
			return nullptr;
		}
		else {
			if ((PCFormat == HostAVSavedSurdaceDesc.Format)
				&& (dwSlicePitch == TmpLockedRect.Pitch * HostAVSavedSurdaceDesc.Height)) {
				BYTE* dst = (BYTE*)TmpLockedRect.pBits;
				BYTE* src = (BYTE*)GetDataFromXboxResource(pPersistSurface);
				memcpy(dst, src, TmpLockedRect.Pitch * HostAVSavedSurdaceDesc.Height);
			}
		}
		hRet = pTmpHostSurface->UnlockRect();
	}
	IDirect3DTexture* pNewHostPersistTexture;
	IDirect3DSurface* pNewHostPersistSurface;
	hRet = g_pD3DDevice->CreateTexture(HostAVSavedSurdaceDesc.Width, HostAVSavedSurdaceDesc.Height,
		1, // Levels
		D3DUSAGE_DYNAMIC, // Usage always as render target D3DUSAGE_DYNAMIC
		HostAVSavedSurdaceDesc.Format,
		D3DPOOL_DEFAULT, // D3DPOOL_DEFAULT D3DPOOL_SYSTEMMEM
		&pNewHostPersistTexture,
		nullptr // pSharedHandle
	);
	DEBUG_D3DRESULT(hRet, "D3DDevice_GetPersistedSurface::CreateTexture");
	D3DLOCKED_RECT HostLockedRect;
	if (hRet == D3D_OK) {
		hRet = pNewHostPersistTexture->GetSurfaceLevel(0, &pNewHostPersistSurface);
		DEBUG_D3DRESULT(hRet, "D3DDevice_GetPersistedSurface::pNewHostSurface");
		if (hRet != D3D_OK) {
			EmuLog(LOG_LEVEL::WARNING, "Could not lock Host Surface for Xbox texture in D3DDevice_GetPersistedSurface");
			return nullptr;
		}
		else {
			//CreateHostResource()
			hRet = g_pD3DDevice->UpdateTexture(pTmpHostTexture, pNewHostPersistTexture);
			if(!FAILED(hRet)){
				SetHostResourceForcedXboxData(&xboxPersistTexture, pNewHostPersistTexture, -1);
				SetHostResourcePCFormat(&xboxPersistTexture, HostAVSavedSurdaceDesc.Format);
				SetHostResourcehashLifeTime(&xboxPersistTexture, 1000ms);
				SetHostResourceForcedXboxData(pPersistSurface, pNewHostPersistSurface, -1);
				SetHostResourcePCFormat(pPersistSurface, HostAVSavedSurdaceDesc.Format);
				SetHostResourcehashLifeTime(pPersistSurface, 1000ms);
				SetHostResourcePCFormat((xbox::X_D3DResource*)xbox::AvSavedDataAddress, HostAVSavedSurdaceDesc.Format);
				pHostPersistSurface->Release();
				//pNewHostPersistTexture->Release();
				//pNewHostPersistSurface->Release();
				return pPersistSurface;
			}
		}
	}

	if(pHostPersistSurface)
		pHostPersistSurface->Release();
	return nullptr;//show warning message.
}

// ******************************************************************
// * patch: D3DDevice_GetPersistedSurface
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_GetPersistedSurface)(xbox::X_D3DSurface** ppSurface)
{
	//template for syncing HLE apis with pgraf using waiting lock
	bool WaitForPGRAPH;
	WaitForPGRAPH = true;
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)&WaitForPGRAPH;// (DWORD)PrimitiveType;

	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_GetPersistedSurface, 1, PBTokenArray);//argCount, not necessary, default to 14

	EmuKickOff();

	while (WaitForPGRAPH)
		; //this line is must have
	*ppSurface = CxbxrImpl_GetAvSavedDataSurface();// CxbxrImpl_GetPersistedSurface();
}
// ******************************************************************
// * patch: D3DDevice_GetPersistedSurface2
// ******************************************************************
xbox::X_D3DSurface* WINAPI xbox::EMUPATCH(D3DDevice_GetPersistedSurface2)()
{
	//template for syncing HLE apis with pgraf using waiting lock
	bool WaitForPGRAPH;
	WaitForPGRAPH = true;
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)&WaitForPGRAPH;// (DWORD)PrimitiveType;

	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_GetPersistedSurface, 1, PBTokenArray);//argCount, not necessary, default to 14

	EmuKickOff();

	while (WaitForPGRAPH)
		; //this line is must have
	return CxbxrImpl_GetAvSavedDataSurface();// CxbxrImpl_GetPersistedSurface();
}

void CxbxrImpl_PersistDisplay()
{
	LOG_INIT

	if (xbox::AvSavedDataAddress == nullptr) {
		return;
	}
	//free previously stored persist surface data if it exists
	if (xboxPersistTexture.Data != NULL) {
//		free((void*)xboxPersistTexture.Data);
		xboxPersistTexture.Data = NULL;
	}
	IDirect3DSurface* pXboxBackBufferHostSurface;
	D3DSURFACE_DESC pXboxBackBufferHostSurdaceDesc;
	//retrive xbox back buffer surface, if backbuffer surface doesn't exist, use xbox render target surface instead.
	xbox::X_D3DSurface xboxPersistSurface = g_pXbox_BackBufferSurface != nullptr ? *g_pXbox_BackBufferSurface : *g_pXbox_RenderTarget;
	pXboxBackBufferHostSurface = GetHostSurface(&xboxPersistSurface);
	pXboxBackBufferHostSurface->GetDesc(&pXboxBackBufferHostSurdaceDesc);
	//alter common flags of xbox persist texture and surface.
	xboxPersistSurface.Common &= 0x00FFFFFF;
	xboxPersistTexture = (xbox::X_D3DBaseTexture)xboxPersistSurface;
	xboxPersistTexture.Common &= 0x00FEFFFF;
	IDirect3DSurface* pXboxPersistSurfaceHostSurface;
	IDirect3DTexture* pXboxPersistTextureHostTexture;
	HRESULT hRet = g_pD3DDevice->CreateTexture(pXboxBackBufferHostSurdaceDesc.Width, pXboxBackBufferHostSurdaceDesc.Height,
		1, // Levels
		D3DUSAGE_DYNAMIC, // Usage always as render target
		pXboxBackBufferHostSurdaceDesc.Format,
		D3DPOOL_SYSTEMMEM, // D3DPOOL_DEFAULT
		&pXboxPersistTextureHostTexture,
		nullptr // pSharedHandle
	);
	DEBUG_D3DRESULT(hRet, "D3DDevice_PersistDisplay::CreateTexture");

	if (hRet == D3D_OK) {
		hRet = pXboxPersistTextureHostTexture->GetSurfaceLevel(0, &pXboxPersistSurfaceHostSurface);
		DEBUG_D3DRESULT(hRet, "D3DDevice_PersistDisplay::pNewHostSurface");
	}
	hRet = g_pD3DDevice->GetRenderTargetData(pXboxBackBufferHostSurface, pXboxPersistSurfaceHostSurface);
	if (hRet != D3D_OK) {
		EmuLog(LOG_LEVEL::WARNING, "Failed in GetRenderTargetData in CxbxrImpl_PersistDisplay");
	}

	D3DLOCKED_RECT HostLockedRect;
	hRet = pXboxPersistSurfaceHostSurface->LockRect(&HostLockedRect, NULL, D3DLOCK_READONLY);
	if (hRet != D3D_OK) {
		EmuLog(LOG_LEVEL::WARNING, "Could not lock Host Surface for Xbox texture in CxbxrImpl_PersistDisplay");
	}
	//now we copy the host surface data in the buffer to the surface AvSetSavedDataAddress pointing to so it can persist after rebooting.
	xbox::X_D3DSurface* pPersistSurface = (xbox::X_D3DSurface*)xbox::AvSavedDataAddress;

	BYTE* dst, * src;
	dst = (BYTE*)GetDataFromXboxResource(pPersistSurface);
	src = (BYTE*)((DWORD)HostLockedRect.pBits );
	memcpy(dst, src, HostLockedRect.Pitch * pXboxBackBufferHostSurdaceDesc.Height);
	pXboxPersistSurfaceHostSurface->UnlockRect();
}
// ******************************************************************
// * patch: D3DDevice_PersistDisplay
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_PersistDisplay)()
{
	LOG_INIT

		// TODO: This function simply saves a copy of the display to a surface and persists it in contiguous memory
		// This function, if ever required, can be implemented as the following
		// 1. Check for an existing persisted surface via AvGetSavedDataAddress, free it if necessary
		// 2. Create an Xbox format surface with the same size and format as active display
		// 3. Copy the host framebuffer to the xbox surface, converting format if necessary
		// 4. Set the display mode via AvSetDisplayMode to the same format as the persisted surface,
		//    passing the ->Data pointer of the xbox surface as the framebuffer pointer.
		// 5. Use MmPersistContigousMemory to persist the surface data across reboot
		// 6. Call AvSetSavedDataAddress, passing the xbox surface data pointer

	// Call the native Xbox function so that AvSetSavedDataAddress is called and the VMManager can know its correct address
	HRESULT hRet= XB_TRMP(D3DDevice_PersistDisplay)();

	//template for syncing HLE apis with pgraph using waiting lock
	bool WaitForPGRAPH;
	WaitForPGRAPH = true;
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)&WaitForPGRAPH;// (DWORD)PrimitiveType;

	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_PersistDisplay, 1, PBTokenArray);//argCount, not necessary, default to 14

	EmuKickOff();

	while (WaitForPGRAPH)
		; //this line is must have
	CxbxrImpl_PersistDisplay();
	return  hRet;
}

// ******************************************************************
// * patch: D3DDevice_PrimeVertexCache
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_PrimeVertexCache)
(
	uint_xt  VertexCount,
	WORD *pIndexData
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(VertexCount)
		LOG_FUNC_ARG(pIndexData)
		LOG_FUNC_END;

	// TODO: Implement
	// no need to implement this in HLE or trampoline to xbox. this function's purpose is to pre-fill GPU's vertex cache before actual draw call in order to speed up draw calls.
	// simply skip this function and handle the draw calls followed up is enough. trampoline is ok since pgraph is capable of handling this situation.
	LOG_UNIMPLEMENTED();
}

extern void pgraph_use_Transform();
extern void pgraph_use_DirectModelView();

void WINAPI CxbxrImpl_SetModelView_Calc
(
	CONST D3DMATRIX *pModelView,
	CONST D3DMATRIX *pInverseModelView,
	CONST D3DMATRIX *pComposite
)
{
	// we can't calculate matProjection without pModelView
	if ((pModelView == nullptr )) {
		// cancel DirectModelView transform mode. 
		pgraph_use_Transform();
		return;
	}

	g_xbox_transform_ModelView = *pModelView;
	if (pComposite != nullptr)g_xbox_transform_Composite = *pComposite;
	if(pInverseModelView!=nullptr)g_xbox_transform_InverseModelView = *pInverseModelView;
	// matModelView=matWorld*matView, we have no idea of these two Matrix. so we use unit matrix for view matrix, and matModelView matrix for matWorld
	D3DMATRIX matUnit;
	memset(&matUnit._11, 0, sizeof(matUnit));
	matUnit._11 = 1.0;
	matUnit._22 = 1.0;
	matUnit._33 = 1.0;
	matUnit._44 = 1.0;

	//CxbxrImpl_SetTransform(xbox::X_D3DTS_WORLD, pModelView);
	//CxbxrImpl_SetTransform(xbox::X_D3DTS_VIEW, &matUnit);
	g_xbox_DirectModelView_View= matUnit;
	g_xbox_DirectModelView_World= *pModelView; 
	// use g_xbox_transform_ViewportTransform
	//D3DXMATRIX matViewportTransform;
	// backup xbox d3d porjection matrix
	CxbxrImpl_GetTransform(xbox::X_D3DTS_PROJECTION, &g_xbox_transform_Projection);
	// set projection to unit matrix, using trampoline directly, so we can get viewport transform matrix.
	CxbxrImpl_SetTransformFast(xbox::X_D3DTS_PROJECTION, &matUnit);
	// update ProjectionViewportTransform matrix
	if (XB_TRMP(D3D_UpdateProjectionViewportTransform))
	{
		XB_TRMP(D3D_UpdateProjectionViewportTransform)();
	}
	else {
		// D3D_UpdateProjectionViewportTransform should be found always 
		assert(0);
	}
	// read back ProjectionViewportTransform matrix, it should be ViewportTrasform matrix only.
	CxbxrImpl_GetProjectionViewportMatrix(&g_xbox_transform_ViewportTransform);

	// restore xbox d3d porjection matrix
	CxbxrImpl_SetTransformFast(xbox::X_D3DTS_PROJECTION, &g_xbox_transform_Projection);
	// use g_xbox_transform_InverseModelView
	//D3DXMATRIX matInverseModelViewNew;
	// get matInverseModelViewNew, the input pInverseModelView might not present, so we always recalculate it.
	D3DXMatrixInverse((D3DXMATRIX*)&g_xbox_transform_InverseModelView, NULL, (D3DXMATRIX*)pModelView);
	
	// update g_xbox_DirectModelView_InverseWorldViewTransposed for use in FF mode vertex shader constant update routine
	D3DXMatrixTranspose((D3DXMATRIX*)&g_xbox_DirectModelView_InverseWorldViewTransposed, (D3DXMATRIX*)&g_xbox_transform_InverseModelView);

	if ((pComposite == nullptr)) {
		// use xbox d3d projection if we don't have pComposite
		CxbxrImpl_GetTransform(xbox::X_D3DTS_PROJECTION, &g_xbox_DirectModelView_Projection);
		return;
	}

	// matComposite = matModelView * matProjectionViewportTransform
	// matProjectionViewportTransform = matPjojection * matViewportTransform
	// if we set matProjection to matUnit, then CDevice_GetProjectionViewportMatrix() will return matViewportTransform
	D3DXMATRIX matViewportTransform;
	// set projection to unit matrix, so we can get viewport transform matrix.
	//CxbxrImpl_SetTransform(xbox::X_D3DTS_PROJECTION, &matUnit);
	// zeroout matViewportTransform
	//memset(&matViewportTransform._11, 0, sizeof(matViewportTransform));
	if(XB_TRMP(D3DDevice_GetProjectionViewportMatrix)){
	    XB_TRMP(D3DDevice_GetProjectionViewportMatrix)(&g_xbox_transform_ViewportTransform);
	}
	else {
		// can't calculate projection matrix, use xbox d3d projection matrix
		CxbxrImpl_GetTransform(xbox::X_D3DTS_PROJECTION, &g_xbox_DirectModelView_Projection);
		return;
	}

	D3DXMATRIX matInverseViewportTransform;
	// get matInverseViewportTransform
	D3DXMatrixInverse(&matInverseViewportTransform, NULL, (D3DXMATRIX*)&g_xbox_transform_ViewportTransform);
	// if we're not in skinning mode, composite matrix includes modelview matrix
	if (NV2ARenderStates.GetXboxRenderState(xbox::X_D3DRS_VERTEXBLEND) == 0){
		//D3DXMATRIX matProjection;
		// matPjojection = inverseModelView*matModelView*matProjection*matViewportTransform * matInverseViewportTransform = matProjection*matViewportTransform * matInverseViewportTransform = matProjection
		//g_xbox_DirectModelView_Projection = (D3DXMATRIX)g_xbox_transform_InverseModelView * (*pComposite)* matInverseViewportTransform;
		D3DXMATRIX matTmp;
		D3DXMatrixMultiply(&matTmp, (D3DXMATRIX*)&g_xbox_transform_InverseModelView, (D3DXMATRIX*)pComposite);
		D3DXMatrixMultiply((D3DXMATRIX*)&g_xbox_DirectModelView_Projection, &matTmp, &matInverseViewportTransform);
	}
	// in skinning mode, composite matrix is only projectionviewport transform. 
	else {
		D3DXMatrixMultiply((D3DXMATRIX*)&g_xbox_DirectModelView_Projection, (D3DXMATRIX*)pComposite, &matInverseViewportTransform);
	}

}

void WINAPI CxbxrImpl_SetModelView
(
	CONST D3DMATRIX* pModelView,
	CONST D3DMATRIX* pInverseModelView,
	CONST D3DMATRIX* pComposite
)
{
	extern void CxbxrImpl_LazySetTransform(NV2AState * d);
	NV2AState* d = g_NV2A->GetDeviceState();
	//PGRAPHState* pg = &d->pgraph;
	// only calls d3d implememtation when pushbuffer is not recording.
	if (g_pXbox_BeginPush_Buffer == nullptr) {
		// CxbxrImpl_SetModleView_Cale() uses trampoline, which is not feasible for pgraph token handler, we call pgraph transform setup routine CxbxrImpl_LazySetTransform(NV2AState* d) instaed.
		//CxbxrImpl_SetModelView_Calc(pModelView, pInverseModelView, pComposite);
		CxbxrImpl_LazySetTransform(d);
		// set DirectModelView/Transform flag.
		if (pModelView != nullptr) {
			pgraph_use_DirectModelView();
		}
		else {
			pgraph_use_Transform();
		}
	}
}
// ******************************************************************
// * patch: D3DDevice_SetModelView
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetModelView)
(
	CONST D3DMATRIX *pModelView, 
	CONST D3DMATRIX *pInverseModelView, 
	CONST D3DMATRIX *pComposite
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pModelView)
		LOG_FUNC_ARG(pInverseModelView)
		LOG_FUNC_ARG(pComposite)
		LOG_FUNC_END;
	// Trampoline to pushbuffer.
	XB_TRMP(D3DDevice_SetModelView)(pModelView, pInverseModelView, pComposite);

	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)pModelView;
	PBTokenArray[3] = (DWORD)pInverseModelView;
	PBTokenArray[4] = (DWORD)pComposite;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetModelView, 3, PBTokenArray);//argCount 14

	LOG_TEST_CASE("SetModelView");
}

// ******************************************************************
// * patch: D3DDevice_FlushVertexCache
// ******************************************************************
void WINAPI xbox::EMUPATCH(D3DDevice_FlushVertexCache)()
{
	LOG_FUNC();

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * patch: D3DDevice_GetModelView
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(D3DDevice_GetModelView)(D3DXMATRIX* pModelView)
{
	LOG_FUNC_ONE_ARG(pModelView);

	D3DXMATRIX mtxWorld, mtxView;

	// I hope this is right
	g_pD3DDevice->GetTransform( D3DTS_WORLD, &mtxWorld );
	g_pD3DDevice->GetTransform( D3DTS_VIEW, &mtxView );

	*pModelView = mtxWorld * mtxView;
	//*pModelView = *d3d8TransformState.GetWorldView(0);
	return D3D_OK;
}


DWORD PushBuffer[64 * 1024 / sizeof(DWORD)];

// ******************************************************************
// * patch: D3D_SetCommonDebugRegisters
// ******************************************************************
void WINAPI xbox::EMUPATCH(D3D_SetCommonDebugRegisters)()
{
	LOG_FUNC();

	// NOTE: I added this because I was too lazy to deal with emulating certain render
	// states that use it.  

	LOG_UNIMPLEMENTED();

}

// ******************************************************************
// * patch: D3DDevice_IsBusy
// ******************************************************************
xbox::bool_xt WINAPI xbox::EMUPATCH(D3DDevice_IsBusy)()
{
		LOG_FUNC();

	// NOTE: This function returns FALSE when the NV2A FIFO is empty/complete, or NV_PGRAPH_STATUS = 0
	// Otherwise, it returns true.

	return FALSE;
}

// ******************************************************************
// * patch: D3D_BlockOnTime
// ******************************************************************
void WINAPI xbox::EMUPATCH(D3D_BlockOnTime)( dword_xt Unknown1, int Unknown2 )
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Unknown1)
		LOG_FUNC_ARG(Unknown2)
		LOG_FUNC_END;

	// NOTE: This function is not meant to be emulated.  Just use it to find out
	// the function that is calling it, and emulate that instead!!!  If necessary,
	// create an XRef...

	//__asm int 3;

	LOG_UNIMPLEMENTED();
}

// LTCG specific D3D_BlockOnTime function
// This uses a custom calling convention where parameter is passed in EAX
// Test case: Burnout 3
__declspec(naked) void WINAPI xbox::EMUPATCH(D3D_BlockOnTime_4)( dword_xt Unknown1 )
{
	int Unknown2;
	__asm {
		LTCG_PROLOGUE
		mov  Unknown2, eax
	}

	// LOG_FORWARD requires unwinding, so carry on without it
	EMUPATCH(D3D_BlockOnTime)(Unknown1, Unknown2);

	__asm {
		LTCG_EPILOGUE
		ret  4
	}
}

void WINAPI CxbxrImpl_DestroyResource(xbox::X_D3DResource* pResource)
{
	if(pResource!=nullptr)
	    FreeHostResource(GetHostResourceKey(pResource));
}
// ******************************************************************
// * patch: D3D_DestroyResource
// ******************************************************************
void WINAPI xbox::EMUPATCH(D3D_DestroyResource)(X_D3DResource* pResource)
{
    LOG_FUNC_ONE_ARG(pResource);

    // Release the host copy (if it exists!)
    FreeHostResource(GetHostResourceKey(pResource));
	//do not sync D3D_DestroyResource() with pgraph, might introduce memory corruption
#if 0
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)pResource;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3D_DestroyResource, 1);//argCount, not necessary, default to 14
#endif
	// Call the Xbox version of DestroyResource
    XB_TRMP(D3D_DestroyResource)(pResource);
}

// ******************************************************************
// * patch: D3D_DestroyResource_LTCG
// ******************************************************************
static void D3D_DestroyResource__LTCG(xbox::X_D3DResource* pResource)
{
	LOG_FUNC_ONE_ARG(pResource);
}

__declspec(naked) void WINAPI xbox::EMUPATCH(D3D_DestroyResource__LTCG)()
{
    X_D3DResource* pResource;
    __asm {
        LTCG_PROLOGUE
        mov  pResource, edi
    }

    // Log
    D3D_DestroyResource__LTCG(pResource);

    // Release the host copy (if it exists!)
    FreeHostResource(GetHostResourceKey(pResource));
	//do not sync D3D_DestroyResource() with pgraph, might introduce memory corruption
#if 0
	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)pResource;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3D_DestroyResource__LTCG, 1);//argCount, not necessary, default to 14
#endif
    // Call the Xbox version of DestroyResource
    __asm {
        mov  edi, pResource
        call XB_TRMP(D3D_DestroyResource__LTCG)

        LTCG_EPILOGUE
        ret
    }
}


// ******************************************************************
// * patch: D3DDevice_SetRenderTargetFast
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_SetRenderTargetFast)
(
    X_D3DSurface	*pRenderTarget,
    X_D3DSurface	*pNewZStencil,
    dword_xt			Flags
)
{
	LOG_FORWARD("D3DDevice_SetRenderTarget");

	// Redirect to the standard version.
	
	//EMUPATCH(D3DDevice_SetRenderTarget)(pRenderTarget, pNewZStencil);
	//NestedPatchCounter call(setRenderTargetCount);

	//fill in the args first. 1st arg goes to PBTokenArray[2], float args need FtoDW(arg)
	PBTokenArray[2] = (DWORD)pRenderTarget;
	PBTokenArray[3] = (DWORD)pNewZStencil;
	//give the correct token enum here, and it's done.
	Cxbxr_PushHLESyncToken(X_D3DAPI_ENUM::X_D3DDevice_SetRenderTarget, 2, PBTokenArray);//argCount 14
	// add reference to the surfaces to prevent them being released before we access them in pgraph.
	if (pRenderTarget)
		CxbxrImpl_Resource_AddRef(pRenderTarget);
	if (pNewZStencil)
		CxbxrImpl_Resource_AddRef(pNewZStencil);

	XB_TRMP(D3DDevice_SetRenderTargetFast)(pRenderTarget, pNewZStencil,Flags);

	//CxbxrImpl_SetRenderTarget(pRenderTarget, pNewZStencil);
}

// ******************************************************************
// * patch: D3D::LazySetPointParams
// ******************************************************************
void WINAPI xbox::EMUPATCH(D3D_LazySetPointParams)
(
	void* Device
)
{
	LOG_FUNC_ONE_ARG(Device);

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * patch: D3DDevice_GetMaterial
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(D3DDevice_GetMaterial)
(
	X_D3DMATERIAL8* pMaterial
)
{
	LOG_FUNC_ONE_ARG(pMaterial);

	HRESULT hRet = D3D_OK;

	if (pMaterial)
	{
		hRet = g_pD3DDevice->GetMaterial(pMaterial);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->GetMaterial");
	}

	if(FAILED(hRet))
    {
		EmuLog(LOG_LEVEL::WARNING, "We're lying about getting a material!");
        hRet = D3D_OK;
    }
}
