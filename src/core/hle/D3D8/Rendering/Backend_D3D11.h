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
// *  All rights reserved
// *
// ******************************************************************
#ifndef BACKEND_D3D11_H
#define BACKEND_D3D11_H

#ifdef CXBX_USE_D3D11

#include "core\hle\D3D8\XbD3D8Types.h"

// ******************************************************************
// * D3D11 device globals
// ******************************************************************
extern IDXGISwapChain              *g_pSwapChain;
extern ID3D11DeviceContext         *g_pD3DDeviceContext;
extern ID3D11RenderTargetView      *g_pD3DBackBufferView;
extern ID3D11DepthStencilView      *g_pD3DDepthStencilView;
extern ID3D11RenderTargetView      *g_pD3DCurrentRTV;
extern ID3D11Texture2D             *g_pD3DDepthStencilBuffer;
extern IDirect3DSurface            *g_pD3DBackBufferSurface;
extern IDirect3DSurface            *g_pD3DCurrentHostRenderTarget;

// ******************************************************************
// * Constant buffer sizing
// ******************************************************************
static const UINT CXBX_D3D11_VS_CB_SLOT = 0;
static const UINT CXBX_D3D11_VS_CB_COUNT = 256;
static const UINT CXBX_D3D11_PS_CB_SLOT = 0;
static const UINT CXBX_D3D11_PS_CB_COUNT = 32;

// Constant buffers (created in RenderGlobals.cpp device init, used by Backend_D3D11.cpp)
extern ID3D11Buffer *g_pD3D11VSConstantBuffer;
extern ID3D11Buffer *g_pD3D11PSConstantBuffer;

// ******************************************************************
// * D3D11 state descriptors (modified by RenderStates.cpp, applied by CxbxD3D11ApplyDirtyStates)
// ******************************************************************
extern D3D11_RASTERIZER_DESC    g_D3D11RasterizerDesc;
extern D3D11_DEPTH_STENCIL_DESC g_D3D11DepthStencilDesc;
extern D3D11_BLEND_DESC         g_D3D11BlendDesc;

// Dirty flags to trigger D3D11 state object recreation
extern bool  g_bD3D11RasterizerStateDirty;
extern bool  g_bD3D11DepthStencilStateDirty;
extern bool  g_bD3D11BlendStateDirty;

// Additional state parameters passed to OMSet* calls
extern UINT  g_D3D11StencilRef;
extern FLOAT g_D3D11BlendFactor[4];
extern UINT  g_D3D11SampleMask;

// ******************************************************************
// * D3D11 backend functions
// ******************************************************************

// Compile blit shaders and create sampler states (called once at device init)
void CxbxD3D11InitBlit();

// Release all D3D11 backend resources (blit shaders, samplers, constant buffers, state objects)
void CxbxD3D11ReleaseBackendResources();

// Unified D3D11 render state mapping — maps a PC-converted render state
// to D3D11 state descriptor members and sets appropriate dirty flags.
// Called from ApplySimpleRenderState and ApplyComplexRenderState.
void CxbxD3D11SetRenderState(uint32_t State, uint32_t Value);

// Recreate D3D11 state objects that have been marked dirty, and flush constant buffers
void CxbxD3D11ApplyDirtyStates();

// Flush vertex/pixel shader constant buffers to GPU if dirty
void CxbxD3D11FlushVertexShaderConstants();
void CxbxD3D11FlushPixelShaderConstants();

// Read back vertex shader constants from the shadow buffer
void CxbxGetVertexShaderConstants(UINT startRegister, float* pConstantData, UINT Vector4fCount);

// D3D11 blit: copy source texture region to dest texture region with optional filtering
// Fast path for same-size copies, shader-based path for scaled copies
HRESULT CxbxD3D11Blt(
	ID3D11Texture2D* pSrc, const RECT* pSrcRect,
	ID3D11Texture2D* pDst, const RECT* pDstRect,
	D3DTEXTUREFILTERTYPE Filter);

#endif // CXBX_USE_D3D11

#endif // BACKEND_D3D11_H
