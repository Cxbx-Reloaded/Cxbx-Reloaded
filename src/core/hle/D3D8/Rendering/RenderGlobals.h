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

// ImGui rendering (still has internal ifdefs for ImGui_Impl backend)
class ImGuiUI;
void CxbxImGui_RenderD3D(ImGuiUI* m_imgui, IDirect3DSurface* renderTarget);

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


// EMUPATCH declarations split into topic-specific headers:
#include "EmuPatches_Device.h"
#include "EmuPatches_Draw.h"
#include "EmuPatches_Misc.h"
#include "EmuPatches_Shader.h"
#include "EmuPatches_State.h"
#include "EmuPatches_Surface.h"
#include "EmuPatches_Unused.h"

#endif // RENDERGLOBALS_H
