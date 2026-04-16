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
#include "EmuD3D8_common.h"

XboxRenderStateConverter XboxRenderStates;
XboxTextureStateConverter XboxTextureStates;

D3D8LightState d3d8LightState = D3D8LightState();
D3D8TransformState d3d8TransformState = D3D8TransformState();
FixedFunctionVertexShaderState ffShaderState = {}; // TODO find a home for this and associated code

// Allow use of time duration literals (making 16ms, etc possible)
using namespace std::literals::chrono_literals;

// Global(s)
HWND                                g_hEmuWindow   = NULL; // rendering window
bool                                g_bClipCursor  = false; // indicates that the mouse cursor should be confined inside the rendering window
IDirect3DDevice                    *g_pD3DDevice   = nullptr; // Direct3D Device

// Static Variable(s)
static bool                         g_bSupportsFormatSurface[xbox::X_D3DFMT_LAST + 1]; // Does device support surface format?
static bool                         g_bSupportsFormatSurfaceRenderTarget[xbox::X_D3DFMT_LAST + 1]; // Does device support surface format?
static bool                         g_bSupportsFormatSurfaceDepthStencil[xbox::X_D3DFMT_LAST + 1]; // Does device support surface format?
static bool                         g_bSupportsFormatTexture[xbox::X_D3DFMT_LAST + 1]; // Does device support texture format?
static bool                         g_bSupportsFormatTextureRenderTarget[xbox::X_D3DFMT_LAST + 1]; // Does device support texture format?
static bool                         g_bSupportsFormatTextureDepthStencil[xbox::X_D3DFMT_LAST + 1]; // Does device support texture format?
static bool                         g_bSupportsFormatVolumeTexture[xbox::X_D3DFMT_LAST + 1]; // Does device support surface format?
static bool                         g_bSupportsFormatCubeTexture[xbox::X_D3DFMT_LAST + 1]; // Does device support surface format?
static HBRUSH                       g_hBgBrush = NULL; // Background Brush
static bool                         g_bIsFauxFullscreen = false;
static DWORD						g_OverlaySwap = 0; // Set in D3DDevice_UpdateOverlay
static int                          g_iWireframe = 0; // wireframe toggle
static bool                         g_bHack_UnlockFramerate = false; // ignore the xbox presentation interval
static bool                         g_bHasDepth = false;    // Does device have a Depth Buffer?
       float                        g_ZScale = 1.0;
static bool                         g_bHasStencil = false;  // Does device have a Stencil Buffer?
static DWORD						g_dwPrimPerFrame = 0;	// Number of primitives within one frame
       bool                         g_bUsePassthroughHLSL = true;
static float                        g_AspectRatioScale = 1.0f;
static UINT                         g_AspectRatioScaleWidth = 0;
static UINT                         g_AspectRatioScaleHeight = 0;
static D3DSurfaceDesc               g_HostBackBufferDesc;
static Settings::s_video            g_XBVideo;

// D3D based variables
static IDirect3DIndexBuffer        *g_pClosingLineLoopHostIndexBuffer = nullptr;
static IDirect3DIndexBuffer        *g_pQuadToTriangleHostIndexBuffer = nullptr;

static bool                         g_bEnableHostQueryVisibilityTest = true;
static std::stack<IDirect3DQuery*>  g_HostQueryVisibilityTests;
static std::map<int, IDirect3DQuery*> g_HostVisibilityTestMap;

// cached Direct3D state variable(s)
static size_t                       g_QuadToTriangleHostIndexBuffer_Size = 0; // = NrOfQuadIndices
static INDEX16                     *g_pQuadToTriangleIndexData = nullptr;
static size_t                       g_QuadToTriangleIndexData_Size = 0; // = NrOfQuadIndices

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

static DWORD                        g_VBLastSwap = 0;

static xbox::dword_xt                   g_Xbox_PresentationInterval_Default = D3DPRESENT_INTERVAL_IMMEDIATE;
       xbox::dword_xt                   g_Xbox_PresentationInterval_Override = 0;
static xbox::X_D3DSWAPDATA			g_Xbox_SwapData = {0}; // current swap information
static xbox::X_D3DSWAPCALLBACK		g_pXbox_SwapCallback = xbox::zeroptr;	// Swap/Present callback routine
static xbox::X_D3DVBLANKDATA			g_Xbox_VBlankData = {0}; // current vertical blank information

       xbox::X_D3DSurface           *g_pXbox_BackBufferSurface = xbox::zeroptr;
static xbox::X_D3DSurface           *g_pXbox_DefaultDepthStencilSurface = xbox::zeroptr;
       xbox::X_D3DSurface           *g_pXbox_RenderTarget = xbox::zeroptr;
static xbox::X_D3DSurface           *g_pXbox_DepthStencil = xbox::zeroptr;
       xbox::X_D3DMULTISAMPLE_TYPE   g_Xbox_MultiSampleType = xbox::X_D3DMULTISAMPLE_NONE;

       xbox::X_VERTEXSHADERCONSTANTMODE g_Xbox_VertexShaderConstantMode = X_D3DSCM_192CONSTANTS; // Set by D3DDevice_SetShaderConstantMode, TODO : Move to XbVertexShader.cpp
static xbox::dword_xt                   g_Xbox_BaseVertexIndex = 0; // Set by D3DDevice_SetIndices, read by D3DDevice_DrawIndexedVertices : a value that's effectively added to every vertex index (as stored in an index buffer) by multiplying this by vertex stride and added to the vertex buffer start (see BaseVertexIndex in CxbxDrawIndexed)
static xbox::dword_xt                  *g_pXbox_BeginPush_Buffer = xbox::zeroptr; // primary push buffer

       xbox::X_PixelShader*			g_pXbox_PixelShader = xbox::zeroptr;
static xbox::PVOID                   g_pXbox_Palette_Data[xbox::X_D3DTS_STAGECOUNT] = { xbox::zeroptr, xbox::zeroptr, xbox::zeroptr, xbox::zeroptr }; // cached palette pointer
static unsigned                     g_Xbox_Palette_Size[xbox::X_D3DTS_STAGECOUNT] = { 0 }; // cached palette size


             EMUFORMAT               g_HostTextureFormats[xbox::X_D3DTS_STAGECOUNT]; // Updated by CxbxUpdateHostTextures(), read by CxbxCalcColorSign
       xbox::X_D3DBaseTexture       *g_pXbox_SetTexture[xbox::X_D3DTS_STAGECOUNT] = {0,0,0,0}; // Set by our D3DDevice_SetTexture and D3DDevice_SwitchTexture patches
static xbox::X_D3DBaseTexture        CxbxActiveTextureCopies[xbox::X_D3DTS_STAGECOUNT] = {}; // Set by D3DDevice_SwitchTexture. Cached active texture

xbox::X_D3DVIEWPORT8 g_Xbox_Viewport = { 0 };
float g_Xbox_BackbufferScaleX = 1;
float g_Xbox_BackbufferScaleY = 1;
xbox::X_D3DSWAP g_LastD3DSwap = (xbox::X_D3DSWAP) -1;


static constexpr size_t INDEX_BUFFER_CACHE_SIZE = 10000;

static void							UpdateCurrentMSpFAndFPS(); // Used for benchmarking/fps count
static void							CxbxImpl_SetRenderTarget(xbox::X_D3DSurface *pRenderTarget, xbox::X_D3DSurface *pNewZStencil);

#define CXBX_D3DCOMMON_IDENTIFYING_MASK (X_D3DCOMMON_TYPE_MASK | X_D3DCOMMON_D3DCREATED)


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
    _9_11(xbox::uint_xt, IDXGIAdapter*)  Adapter;
    _9_11(D3DDEVTYPE, D3D_DRIVER_TYPE)   DeviceType;
	// Set by EmuCreateDeviceProxy()
#ifdef CXBX_USE_D3D11
	// D3D11 only needs the common presentation fields (DXGI handles the rest)
	struct {
		UINT BackBufferWidth;
		UINT BackBufferHeight;
		UINT FullScreen_RefreshRateInHz;
		BOOL Windowed;
	} HostPresentationParameters;
#else
	D3DPRESENT_PARAMETERS            HostPresentationParameters;
#endif
}
g_EmuCDPD;

// Declare trampolines
#define XB_TRAMPOLINES(XB_MACRO)                                                                                                                                                                            \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_CreateVertexShader,                       (CONST xbox::dword_xt*, CONST xbox::dword_xt*, xbox::dword_xt*, xbox::dword_xt)                       );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_DeleteVertexShader,                       (xbox::dword_xt)                                                                                      );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_DeleteVertexShader_0__LTCG_eax1,          ()                                                                                                    );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_GetBackBuffer,                            (xbox::int_xt, D3DBACKBUFFER_TYPE, xbox::X_D3DSurface**)                                              );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_GetBackBuffer_8__LTCG_eax1,               (D3DBACKBUFFER_TYPE, xbox::X_D3DSurface**)                                                            );  \
    XB_MACRO(xbox::X_D3DSurface*, WINAPI,     D3DDevice_GetBackBuffer2,                           (xbox::int_xt)                                                                                        );  \
    XB_MACRO(xbox::X_D3DSurface*, WINAPI,     D3DDevice_GetBackBuffer2_0__LTCG_eax1,              ()                                                                                                    );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_GetDepthStencilSurface,                   (xbox::X_D3DSurface**)                                                                                );  \
    XB_MACRO(xbox::X_D3DSurface*, WINAPI,     D3DDevice_GetDepthStencilSurface2,                  (xbox::void_xt)                                                                                       );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_GetDisplayMode,                           (xbox::X_D3DDISPLAYMODE*)                                                                             );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_GetRenderTarget,                          (xbox::X_D3DSurface**)                                                                                );  \
    XB_MACRO(xbox::X_D3DSurface*, WINAPI,     D3DDevice_GetRenderTarget2,                         (xbox::void_xt)                                                                                       );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_LightEnable,                              (xbox::dword_xt, xbox::bool_xt)                                                                       );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_LightEnable_4__LTCG_eax1,                 (xbox::bool_xt)                                                                                       );  \
  /*XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_LoadVertexShader,                         (xbox::dword_xt, xbox::dword_xt)                                                                      );*/\
  /*XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_LoadVertexShaderProgram,                  (CONST xbox::dword_xt*, xbox::dword_xt)                                                               );*/\
  /*XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_LoadVertexShader_4__LTCG_eax1,            (xbox::dword_xt)                                                                                      );*/\
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_PersistDisplay,                           (xbox::void_xt)                                                                                       );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_Reset,                                    (xbox::X_D3DPRESENT_PARAMETERS*)                                                                      );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_Reset_0__LTCG_edi1,                       ()                                                                                                    );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_Reset_0__LTCG_ebx1,                       ()                                                                                                    );  \
  /*XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SelectVertexShader,                       (xbox::dword_xt, xbox::dword_xt)                                                                      );*/\
  /*XB_MACRO(xbox::void_xt,       __stdcall,  D3DDevice_SelectVertexShader_0__LTCG_eax1_ebx2,     ()                                                                                                    );*/\
  /*XB_MACRO(xbox::void_xt,       __stdcall,  D3DDevice_SelectVertexShader_4__LTCG_eax1,          (xbox::dword_xt)                                                                                      );*/\
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetBackMaterial,                          (CONST xbox::X_D3DMATERIAL8*)                                                                         );  \
  /*XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetGammaRamp,                             (xbox::dword_xt, CONST X_D3DGAMMARAMP*)                                                               );*/\
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetIndices,                               (xbox::X_D3DIndexBuffer*, xbox::uint_xt)                                                              );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetIndices_4__LTCG_ebx1,                  (xbox::uint_xt)                                                                                       );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_SetLight,                                 (xbox::dword_xt, CONST xbox::X_D3DLIGHT8*)                                                            );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetMaterial,                              (CONST xbox::X_D3DMATERIAL8*)                                                                         );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetPixelShader,                           (xbox::dword_xt)                                                                                      );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetPixelShader_0__LTCG_eax1,              ()                                                                                                    );  \
    XB_MACRO(xbox::void_xt,       __fastcall, D3DDevice_SetRenderState_Simple,                    (xbox::dword_xt, xbox::dword_xt)                                                                      );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetRenderTarget,                          (xbox::X_D3DSurface*, xbox::X_D3DSurface*)                                                            );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetRenderTarget_0__LTCG_ecx1_eax2,        ()                                                                                                    );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetStreamSource,                          (xbox::uint_xt, xbox::X_D3DVertexBuffer*, xbox::uint_xt)                                              );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetStreamSource_0__LTCG_eax1_edi2_ebx3,   ()                                                                                                    );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetStreamSource_4__LTCG_eax1_ebx2,        (xbox::uint_xt)                                                                                       );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetStreamSource_8__LTCG_eax1,             (xbox::X_D3DVertexBuffer*, xbox::uint_xt)                                                             );  \
    XB_MACRO(xbox::void_xt,       __fastcall, D3DDevice_SetStreamSource_8__LTCG_edx1,             (void*, xbox::uint_xt, xbox::X_D3DVertexBuffer*, xbox::uint_xt)                                       );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetTexture,                               (xbox::dword_xt, xbox::X_D3DBaseTexture*)                                                             );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetTexture_4__LTCG_eax2,                  (xbox::dword_xt)                                                                                      );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetTexture_4__LTCG_eax1,                  (xbox::X_D3DBaseTexture*)                                                                             );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetPalette,                               (xbox::dword_xt, xbox::X_D3DPalette*)                                                                 );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetPalette_4__LTCG_eax1,                  (xbox::X_D3DPalette*)                                                                                 );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetVertexShader,                          (xbox::dword_xt)                                                                                      );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetVertexShader_0__LTCG_ebx1,             ()                                                                                                    );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetVertexShaderInput,                     (xbox::dword_xt, xbox::uint_xt, xbox::X_STREAMINPUT*)                                                 );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetViewport,                              (CONST xbox::X_D3DVIEWPORT8*)                                                                         );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetTransform,                             (xbox::X_D3DTRANSFORMSTATETYPE, CONST xbox::X_D3DMATRIX*)                                             );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetTransform_0__LTCG_eax1_edx2,           ()                                                                                                    );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_MultiplyTransform,                        (xbox::X_D3DTRANSFORMSTATETYPE, CONST xbox::X_D3DMATRIX*)                                             );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_MultiplyTransform_0__LTCG_ebx1_eax2,      ()                                                                                                    );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3D_DestroyResource,                                (xbox::X_D3DResource*)                                                                                );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3D_DestroyResource_0__LTCG_edi1,                   (xbox::void_xt)                                                                                       );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     Direct3D_CreateDevice,                              (xbox::uint_xt, D3DDEVTYPE, HWND, xbox::dword_xt, xbox::X_D3DPRESENT_PARAMETERS*, xbox::X_D3DDevice**));  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     Direct3D_CreateDevice_16__LTCG_eax4_ebx6,           (xbox::uint_xt, D3DDEVTYPE, HWND, xbox::X_D3DPRESENT_PARAMETERS*)                                     );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     Direct3D_CreateDevice_16__LTCG_eax4_ecx6,           (xbox::uint_xt, D3DDEVTYPE, HWND, xbox::X_D3DPRESENT_PARAMETERS*)                                     );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     Direct3D_CreateDevice_4__LTCG_eax1_ecx3,            (xbox::X_D3DPRESENT_PARAMETERS*)                                                                      );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     Lock2DSurface,                                      (xbox::X_D3DPixelContainer*, xbox::X_D3DCUBEMAP_FACES, xbox::uint_xt, xbox::X_D3DLOCKED_RECT*, RECT*, xbox::dword_xt) );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     Lock2DSurface_16__LTCG_esi4_eax5,                   (xbox::X_D3DPixelContainer*, xbox::X_D3DCUBEMAP_FACES, xbox::uint_xt, xbox::dword_xt)                         );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     Lock3DSurface,                                      (xbox::X_D3DPixelContainer*, xbox::uint_xt, xbox::X_D3DLOCKED_BOX*, xbox::X_D3DBOX*, xbox::dword_xt)                  );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     Lock3DSurface_16__LTCG_eax4,                        (xbox::X_D3DPixelContainer*, xbox::uint_xt, xbox::X_D3DLOCKED_BOX*, xbox::dword_xt)                           );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3D_CommonSetRenderTarget,                          (xbox::X_D3DSurface*, xbox::X_D3DSurface*, void*)                                                     );  \

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
#ifndef CXBX_USE_D3D11 // DDERR_* constants require ddraw.h (not available in D3D11 mode)
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
#endif // !CXBX_USE_D3D11
	}

	return nullptr;
}

static constexpr UINT WM_CXBXR_RUN_ON_MESSAGE_THREAD = WM_USER+0;

// A helper function to run any code on a window message thread
// Used for those D3D9 function which *must* be ran on this particular thread
