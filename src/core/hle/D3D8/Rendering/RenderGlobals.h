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
#ifndef RENDERGLOBALS_H
#define RENDERGLOBALS_H

#include "core\hle\XAPI\Xapi.h" // For EMUPATCH

// NOTE: this is necessary or else d3dx9mesh.h fails to compile because of undefined VOID macros
#ifndef VOID
#define VOID void
#endif

#include "core\hle\D3D8\XbD3D8Types.h"
#include "common\Settings.hpp" // For Settings::s_video
#include "common\util\hasher.h" // For ComputeHash

#include <chrono>
#include <queue>
#include <stack>
#include <map>
#include <unordered_map>
#include <wrl/client.h>

#define DIRECTDRAW_VERSION 0x0700
#include <ddraw.h>

extern IDirect3DDevice *g_pD3DDevice;

// Xbox state globals (defined in RenderGlobals.cpp)
class XboxRenderStateConverter;
class XboxTextureStateConverter;
class D3D8LightState;
class D3D8TransformState;
extern XboxRenderStateConverter  XboxRenderStates;
extern XboxTextureStateConverter XboxTextureStates;
extern D3D8LightState            d3d8LightState;
extern D3D8TransformState        d3d8TransformState;
extern xbox::X_D3DVIEWPORT8      g_Xbox_Viewport;
extern float                     g_Xbox_BackbufferScaleX;
extern float                     g_Xbox_BackbufferScaleY;
extern xbox::X_D3DSWAP           g_LastD3DSwap;

// Shared type definitions (moved from RenderGlobals.cpp for cross-file visibility)
typedef struct {
	xbox::X_D3DCALLBACK             pCallback;
	xbox::X_D3DCALLBACKTYPE         Type;
	xbox::dword_xt                  Context;
} s_Xbox_Callback;

#define CXBX_D3DCOMMON_IDENTIFYING_MASK (X_D3DCOMMON_TYPE_MASK | X_D3DCOMMON_D3DCREATED)

typedef struct resource_key_hash {
	DWORD Common;
	DWORD Data;
	union {
		struct {
			xbox::addr_xt ResourceAddr;
		};
		struct {
			DWORD Format;
			DWORD Size;
			uint64_t PaletteHash;
		};
	};

	bool operator==(const struct resource_key_hash& other) const
	{
		return (Common == other.Common)
			&& (Data == other.Data)
			&& (Format == other.Format)
			&& (Size == other.Size)
			&& (PaletteHash == other.PaletteHash);
	}

	size_t operator()(const struct resource_key_hash& value) const
	{
		return (size_t)ComputeHash((void*)&value, sizeof(value));
	}
} resource_key_t;

struct OverlayProxy {
	xbox::X_D3DSurface Surface;
	RECT SrcRect;
	RECT DstRect;
	BOOL EnableColorKey;
	D3DCOLOR ColorKey;
};

// information passed to the create device proxy thread
struct EmuD3D8CreateDeviceProxyData
{
	_9_11(xbox::uint_xt, IDXGIAdapter*)  Adapter;
	_9_11(D3DDEVTYPE, D3D_DRIVER_TYPE)   DeviceType;
#ifdef CXBX_USE_D3D11
	struct {
		UINT BackBufferWidth;
		UINT BackBufferHeight;
		UINT FullScreen_RefreshRateInHz;
		BOOL Windowed;
	} HostPresentationParameters;
#else
	D3DPRESENT_PARAMETERS            HostPresentationParameters;
#endif
};

// LTCG prologue/epilogue macros for __declspec(naked) patches
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

constexpr size_t INDEX_BUFFER_CACHE_SIZE = 10000;
constexpr UINT WM_CXBXR_RUN_ON_MESSAGE_THREAD = WM_USER+0;

// Shared global variables (defined in RenderGlobals.cpp)
struct FixedFunctionVertexShaderState;
extern FixedFunctionVertexShaderState ffShaderState;
extern HWND                          g_hEmuWindow;
extern bool                          g_bClipCursor;
extern bool                          g_bSupportsFormatSurface[xbox::X_D3DFMT_LAST + 1];
extern bool                          g_bSupportsFormatSurfaceRenderTarget[xbox::X_D3DFMT_LAST + 1];
extern bool                          g_bSupportsFormatSurfaceDepthStencil[xbox::X_D3DFMT_LAST + 1];
extern bool                          g_bSupportsFormatTexture[xbox::X_D3DFMT_LAST + 1];
extern bool                          g_bSupportsFormatTextureRenderTarget[xbox::X_D3DFMT_LAST + 1];
extern bool                          g_bSupportsFormatTextureDepthStencil[xbox::X_D3DFMT_LAST + 1];
extern bool                          g_bSupportsFormatVolumeTexture[xbox::X_D3DFMT_LAST + 1];
extern bool                          g_bSupportsFormatCubeTexture[xbox::X_D3DFMT_LAST + 1];
extern bool                          g_bHack_UnlockFramerate;
extern bool                          g_bHasDepth;
extern float                         g_ZScale;
extern bool                          g_bHasStencil;
extern DWORD                         g_dwPrimPerFrame;
extern float                         g_AspectRatioScale;
extern UINT                          g_AspectRatioScaleWidth;
extern UINT                          g_AspectRatioScaleHeight;
extern D3DSurfaceDesc                g_HostBackBufferDesc;
extern Settings::s_video             g_XBVideo;
extern bool                          g_bEnableHostQueryVisibilityTest;
extern OverlayProxy                  g_OverlayProxy;
extern bool                          g_bHack_DisableHostGPUQueries;
extern IDirect3DQuery               *g_pHostQueryWaitForIdle;
extern IDirect3DQuery               *g_pHostQueryCallbackEvent;
extern int                           g_RenderUpscaleFactor;
extern xbox::dword_xt                g_Xbox_PresentationInterval_Default;
extern xbox::dword_xt                g_Xbox_PresentationInterval_Override;
extern xbox::X_D3DSWAPDATA           g_Xbox_SwapData;
extern xbox::X_D3DSWAPCALLBACK       g_pXbox_SwapCallback;
extern xbox::X_D3DVBLANKDATA         g_Xbox_VBlankData;
extern xbox::X_D3DSurface           *g_pXbox_BackBufferSurface;
extern xbox::X_D3DSurface           *g_pXbox_DefaultDepthStencilSurface;
extern xbox::X_D3DSurface           *g_pXbox_RenderTarget;
extern xbox::X_D3DSurface           *g_pXbox_DepthStencil;
extern xbox::X_D3DMULTISAMPLE_TYPE   g_Xbox_MultiSampleType;
extern xbox::X_VERTEXSHADERCONSTANTMODE g_Xbox_VertexShaderConstantMode;
extern xbox::dword_xt                g_Xbox_BaseVertexIndex;
extern xbox::PVOID                   g_pXbox_Palette_Data[xbox::X_D3DTS_STAGECOUNT];
extern unsigned                      g_Xbox_Palette_Size[xbox::X_D3DTS_STAGECOUNT];
extern EmuD3D8CreateDeviceProxyData  g_EmuCDPD;

void LookupTrampolinesD3D();

// initialize render window
extern void CxbxInitWindow();

void CxbxUpdateNativeD3DResources();

// Unified shader constant helpers (dispatch to D3D9 or D3D11 internally)
void CxbxSetVertexShaderConstantF(UINT startRegister, const float* pConstantData, UINT Vector4fCount);
void CxbxSetPixelShaderConstantF(UINT startRegister, const float* pConstantData, UINT Vector4fCount);

// Rendering helpers (implemented per-backend in Backend_D3D9.cpp / Backend_D3D11.cpp)
HRESULT CxbxSetRenderTarget(IDirect3DSurface* pHostRenderTarget);
void    CxbxD3DClear(DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil);
void    CxbxSetViewport(D3DVIEWPORT *pHostViewport);
void    CxbxSetScissorRect(CONST RECT *pHostViewportRect);
void    CxbxSetIndices(IDirect3DIndexBuffer* pHostIndexBuffer);
INDEX16* CxbxLockIndexBuffer(IDirect3DIndexBuffer* pHostIndexBuffer);
void    CxbxUnlockIndexBuffer(IDirect3DIndexBuffer* pHostIndexBuffer);
HRESULT CxbxDrawIndexedPrimitive(xbox::X_D3DPRIMITIVETYPE XboxPrimitiveType, UINT IndexCount, INT BaseVertexIndex, UINT StartIndex, UINT MinIndex, UINT NumVertices, UINT PrimCount);
HRESULT CxbxDrawPrimitive(xbox::X_D3DPRIMITIVETYPE XboxPrimitiveType, UINT VertexCount, UINT StartVertex, UINT PrimCount);
HRESULT CxbxBltSurface(IDirect3DSurface* pSrc, const RECT* pSrcRect, IDirect3DSurface* pDst, const RECT* pDstRect, D3DTEXTUREFILTERTYPE Filter);
HRESULT CxbxPresent();
void    CxbxSetDepthStencilSurface(IDirect3DSurface* pHostDepthStencil);
IDirect3DSurface* CxbxGetCurrentRenderTarget(); // Returns current RT (caller owns ref in D3D9, not in D3D11)
HRESULT CxbxGetBackBuffer(IDirect3DSurface** ppBackBuffer); // Returns back buffer (caller owns ref)
HRESULT CxbxSetStreamSource(UINT HostStreamNumber, IDirect3DVertexBuffer* pHostVertexBuffer, UINT VertexStride);
HRESULT CxbxCreateVertexBuffer(UINT Length, IDirect3DVertexBuffer** ppVertexBuffer);
void*   CxbxLockVertexBuffer(IDirect3DVertexBuffer* pVertexBuffer); // Returns mapped pointer, or nullptr on failure
void    CxbxUnlockVertexBuffer(IDirect3DVertexBuffer* pVertexBuffer);
HRESULT CxbxCreatePixelShader(const void* pFunction, SIZE_T FunctionSize, IDirect3DPixelShader** ppShader);
void    CxbxRawSetPixelShader(IDirect3DPixelShader* pPixelShader);
HRESULT CxbxSetVertexShader(IDirect3DVertexShader* pHostVertexShader);
IDirect3DVertexDeclaration* CxbxCreateHostVertexDeclaration(D3DVERTEXELEMENT *pDeclaration);
struct _CxbxVertexDeclaration;
void    CxbxSetHostVertexDeclaration(struct _CxbxVertexDeclaration* pCxbxVertexDeclaration);
void    CxbxSetFogColor(uint32_t fog_color);
void    CxbxGetBumpEnvMatrix(int stage, DWORD value[4]);
void    CxbxGetBumpEnvLuminance(int stage, DWORD value[2]);

#ifdef CXBX_USE_D3D11
// Reusable grow-to-fit dynamic buffer for D3D11.
// Avoids per-draw CreateBuffer/Release overhead for temporary vertex/index buffers.
struct CxbxDynBuffer {
	ID3D11Buffer *pBuffer = nullptr;
	UINT          capacity = 0;   // Current buffer size in bytes
	UINT          bindFlags = 0;  // D3D11_BIND_VERTEX_BUFFER or D3D11_BIND_INDEX_BUFFER

	// Ensure the buffer is at least 'size' bytes, then map-discard and memcpy data in.
	// Returns the (still-bound) buffer, or nullptr on failure.
	ID3D11Buffer *Update(const void *pData, UINT size);
	void Release();
	~CxbxDynBuffer() { Release(); }
};
#endif

// BeginScene/EndScene: no-ops in D3D11
inline void CxbxBeginScene()
{
#ifndef CXBX_USE_D3D11
	g_pD3DDevice->BeginScene();
#endif
}

inline void CxbxEndScene()
{
#ifndef CXBX_USE_D3D11
	g_pD3DDevice->EndScene();
#endif
}

// D3D11 device context (declared in Backend_D3D11.h, needed by query helpers below)
#ifdef CXBX_USE_D3D11
extern ID3D11DeviceContext *g_pD3DDeviceContext;
#endif

// Query helpers: dispatch Issue/GetData calls to D3D9 or D3D11 patterns
inline void CxbxQueryIssueBegin(IDirect3DQuery* pQuery)
{
#ifdef CXBX_USE_D3D11
	g_pD3DDeviceContext->Begin(pQuery);
#else
	pQuery->Issue(D3DISSUE_BEGIN);
#endif
}

inline void CxbxQueryIssueEnd(IDirect3DQuery* pQuery)
{
#ifdef CXBX_USE_D3D11
	g_pD3DDeviceContext->End(pQuery);
#else
	pQuery->Issue(D3DISSUE_END);
#endif
}

inline HRESULT CxbxQueryGetData(IDirect3DQuery* pQuery, void* pData, DWORD dwSize, DWORD dwGetDataFlags)
{
#ifdef CXBX_USE_D3D11
	return g_pD3DDeviceContext->GetData(pQuery, pData, dwSize, dwGetDataFlags);
#else
	return pQuery->GetData(pData, dwSize, dwGetDataFlags);
#endif
}

void CxbxImpl_SetRenderTarget(xbox::X_D3DSurface* pRenderTarget, xbox::X_D3DSurface* pNewZStencil);
void CxbxImpl_SetViewport(xbox::X_D3DVIEWPORT8* pViewport);

// Xbox resource type helpers (used in HostResource.cpp, HostSync.cpp, etc.)
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

// initialize direct3d
extern void EmuD3DInit();

// cleanup direct3d
extern void EmuD3DCleanup();

extern xbox::dword_xt g_Xbox_VertexShader_Handle;

extern xbox::X_PixelShader *g_pXbox_PixelShader;

extern EMUFORMAT g_HostTextureFormats[xbox::X_D3DTS_STAGECOUNT];

extern xbox::X_D3DBaseTexture *g_pXbox_SetTexture[xbox::X_D3DTS_STAGECOUNT];


// Cross-file function declarations (defined in various Host*.cpp files)

// Swap present forward marker (used in EmuPatches_Surface.cpp, EmuPatches_Misc.cpp)
#define CXBX_SWAP_PRESENT_FORWARD (256 + X_D3DSWAP_FINISH + X_D3DSWAP_COPY) // = CxbxPresentForwardMarker + D3DSWAP_FINISH + D3DSWAP_COPY

// Resource cache types (defined in HostResource.cpp)
constexpr DWORD D3DUSAGE_INVALID = 0xFFFFFFFF;

typedef struct _resource_info_t {
	Microsoft::WRL::ComPtr<IDirect3DResource> pHostResource;
	EMUFORMAT HostFormat = EMUFMT_UNKNOWN;
	DWORD HostUsage = D3DUSAGE_INVALID;
	DWORD dwXboxResourceType = 0;
	void* pXboxData = xbox::zeroptr;
	size_t szXboxDataSize = 0;
	uint64_t hash = 0;
	bool forceRehash = false;
	std::chrono::time_point<std::chrono::steady_clock> nextHashTime;
	std::chrono::milliseconds hashLifeTime = std::chrono::milliseconds{1};
	std::chrono::time_point<std::chrono::steady_clock> lastUpdate;
} resource_info_t;

typedef std::unordered_map<resource_key_t, resource_info_t, resource_key_hash> resource_cache_t;

// HostDevice.cpp
void UpdateHostBackBufferDesc();
void SetAspectRatioScale(const xbox::X_D3DPRESENT_PARAMETERS* pPresentationParameters);
DWORD WINAPI EmuRenderWindow(LPVOID lpParam);
void GetMultiSampleOffset(float& xOffset, float& yOffset);
void GetMultiSampleScaleRaw(float& xScale, float& yScale);
void GetScreenScaleFactors(float& scaleX, float& scaleY);
void GetRenderTargetBaseDimensions(float& x, float& y);
void GetBackBufferPixelDimensions(float& x, float& y);
void UpdateDepthStencilFlags(IDirect3DSurface *pDepthStencilSurface);
void SetupPresentationParameters(const xbox::X_D3DPRESENT_PARAMETERS *pXboxPresentationParameters);
void DetermineSupportedD3DFormats();

// HostDevice.cpp
void SetXboxMultiSampleType(xbox::X_D3DMULTISAMPLE_TYPE value);

// HostDraw.cpp
void Direct3D_CreateDevice_Start(const xbox::X_D3DPRESENT_PARAMETERS* pPresentationParameters);
void Direct3D_CreateDevice_End(const xbox::X_D3DPRESENT_PARAMETERS* pPresentationParameters);
IDirect3DIndexBuffer* CxbxCreateIndexBuffer(unsigned IndexCount);
void CreateDefaultD3D9Device(const xbox::X_D3DPRESENT_PARAMETERS* pPresentationParameters);
IDirect3DBaseTexture* CxbxConvertXboxSurfaceToHostTexture(xbox::X_D3DBaseTexture* pBaseTexture);

// HostResource.cpp
xbox::X_D3DRESOURCETYPE GetXboxD3DResourceType(const xbox::X_D3DResource* pXboxResource);
void* GetDataFromXboxResource(xbox::X_D3DResource* pXboxResource);
resource_key_t GetHostResourceKey(xbox::X_D3DResource* pXboxResource, int iTextureStage = -1);
void SetHostResource(xbox::X_D3DResource* pXboxResource, IDirect3DResource* pHostResource, int iTextureStage = -1, DWORD D3DUsage = 0xFFFFFFFFu, EMUFORMAT PCFormat = EMUFMT_UNKNOWN);
void FreeHostResource(resource_key_t key);
IDirect3DSurface* GetHostSurface(xbox::X_D3DResource* pXboxResource, DWORD D3DUsage = 0);
IDirect3DBaseTexture* GetHostBaseTexture(xbox::X_D3DResource* pXboxResource, DWORD D3DUsage = 0, int iTextureStage = 0);
IDirect3DVolumeTexture* GetHostVolumeTexture(xbox::X_D3DResource* pXboxResource, int iTextureStage = 0);
void ClearAllResourceCaches();
uint32_t GetPixelContainerWidth(xbox::X_D3DPixelContainer *pPixelContainer);
uint32_t GetPixelContainerHeight(xbox::X_D3DPixelContainer *pPixelContainer);
unsigned int CxbxGetPixelContainerMipMapLevels(xbox::X_D3DPixelContainer *pPixelContainer);
void GetSurfaceFaceAndLevelWithinTexture(xbox::X_D3DSurface* pSurface, xbox::X_D3DBaseTexture* pTexture, UINT& Level, _9_11(D3DCUBEMAP_FACES, int)& Face);
void GetSurfaceFaceAndLevelWithinTexture(xbox::X_D3DSurface* pSurface, xbox::X_D3DBaseTexture* pBaseTexture, UINT& Level);
resource_cache_t& GetResourceCache(resource_key_t& key);
void PrunePaletizedTexturesCache();
void CxbxGetPixelContainerMeasures(xbox::X_D3DPixelContainer *pPixelContainer, DWORD dwMipMapLevel, UINT *pWidth, UINT *pHeight, UINT *pDepth, UINT *pRowPitch, UINT *pSlicePitch);
void CreateHostResource(xbox::X_D3DResource *pResource, DWORD D3DUsage, int iTextureStage, DWORD dwSize);
void ForceResourceRehash(xbox::X_D3DResource* pXboxResource);
int XboxD3DPaletteSizeToBytes(const xbox::X_D3DPALETTESIZE Size);
xbox::X_D3DPALETTESIZE GetXboxPaletteSize(const xbox::X_D3DPalette *pPalette);

// Inline resource setters
inline void SetHostSurface(xbox::X_D3DResource* pXboxResource, IDirect3DSurface* pHostSurface, int iTextureStage = -1)
{
	SetHostResource(pXboxResource, (IDirect3DResource*)pHostSurface, iTextureStage);
}

inline void SetHostVolume(xbox::X_D3DResource* pXboxResource, IDirect3DVolume* pHostVolume, int iTextureStage = -1)
{
	SetHostResource(pXboxResource, (IDirect3DResource*)pHostVolume, iTextureStage);
}

inline void SetHostTexture(xbox::X_D3DResource* pXboxResource, IDirect3DTexture* pHostTexture, int iTextureStage = -1)
{
	SetHostResource(pXboxResource, (IDirect3DResource*)pHostTexture, iTextureStage);
}

inline void SetHostVolumeTexture(xbox::X_D3DResource* pXboxResource, IDirect3DVolumeTexture* pHostVolumeTexture, int iTextureStage = -1)
{
	SetHostResource(pXboxResource, (IDirect3DResource*)pHostVolumeTexture, iTextureStage);
}

inline void SetHostCubeTexture(xbox::X_D3DResource* pXboxResource, IDirect3DCubeTexture* pHostCubeTexture, int iTextureStage = -1)
{
	SetHostResource(pXboxResource, (IDirect3DResource*)pHostCubeTexture, iTextureStage);
}

// HostRender.cpp
float GetZScaleForPixelContainer(xbox::X_D3DPixelContainer* pSurface);
bool GetHostRenderTargetDimensions(DWORD *pHostWidth, DWORD *pHostHeight, IDirect3DSurface* pHostRenderTarget = nullptr);
void ValidateRenderTargetDimensions(DWORD HostRenderTarget_Width, DWORD HostRenderTarget_Height, DWORD XboxRenderTarget_Width, DWORD XboxRenderTarget_Height);
void UpdateFixedFunctionVertexShaderState();
void CxbxUpdateHostViewPortOffsetAndScaleConstants();

// HostResourceCreate.cpp
D3DXVECTOR4 toVector(D3DCOLOR color);
D3DXVECTOR4 toVector(xbox::X_D3DCOLORVALUE val);

// HostSync.cpp
void CxbxHandleXboxCallbacks();
void CxbxImpl_InsertCallback(xbox::X_D3DCALLBACKTYPE Type, xbox::X_D3DCALLBACK pCallback, xbox::dword_xt Context);
xbox::void_xt CxbxImpl_SetPixelShader(xbox::dword_xt Handle);

// HostWindow.cpp
void DrawUEM(HWND hWnd);
void CxbxReleaseCursor();
void CxbxClipCursor(HWND hWnd);
void CxbxUpdateCursor(bool forceShow = false);
void RunOnWndMsgThread(const std::function<void()>& func);

const char *CxbxGetErrorDescription(HRESULT hResult);

// EmuPatches_State.cpp
xbox::X_D3DSurface* CxbxrImpl_GetBackBuffer2(xbox::int_xt BackBuffer);

// Xbox function trampolines -- defined in RenderGlobals.cpp, used across
// multiple EmuPatches_*.cpp translation units.
// XB_TRAMPOLINES lists all D3D8 trampolines; we invoke it here with an
// "extern" generator so every TU that includes this header sees the
// declarations.  The actual definitions live in RenderGlobals.cpp.
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
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_PersistDisplay,                           (xbox::void_xt)                                                                                       );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_Reset,                                    (xbox::X_D3DPRESENT_PARAMETERS*)                                                                      );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_Reset_0__LTCG_edi1,                       ()                                                                                                    );  \
    XB_MACRO(xbox::hresult_xt,    WINAPI,     D3DDevice_Reset_0__LTCG_ebx1,                       ()                                                                                                    );  \
    XB_MACRO(xbox::void_xt,       WINAPI,     D3DDevice_SetBackMaterial,                          (CONST xbox::X_D3DMATERIAL8*)                                                                         );  \
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

// Generate extern declarations for trampoline function pointers.
// Each expands to a typedef + extern variable declaration.
#define XB_trampoline_extern(ret, conv, func, arguments) \
    typedef ret(conv * XB_TRAMPOLINE_##func##_t)arguments; \
    extern XB_TRAMPOLINE_##func##_t XB_TRAMPOLINE_##func

XB_TRAMPOLINES(XB_trampoline_extern);
#undef XB_trampoline_extern


// EMUPATCH declarations split into topic-specific headers:
#include "EmuPatches_Device.h"
#include "EmuPatches_Draw.h"
#include "EmuPatches_Misc.h"
#include "EmuPatches_Shader.h"
#include "EmuPatches_State.h"
#include "EmuPatches_Surface.h"
// EmuPatches_Unused.h intentionally not included - it's a documentation-only dustbin

#endif // RENDERGLOBALS_H
