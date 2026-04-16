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

#ifdef CXBX_USE_D3D11

#define LOG_PREFIX CXBXR_MODULE::D3D8

#include "Backend_D3D11.h"
#include "RenderGlobals.h" // g_pD3DDevice, g_pD3DDeviceContext
#include "core\kernel\init\CxbxKrnl.h" // LOG_INIT, EmuLog
#include "core\hle\D3D8\XbD3D8Logging.h" // DEBUG_D3DRESULT
#include "core\hle\D3D8\Rendering\Shader.h" // D3DCompile (via d3dcompiler.h)
#include "core\hle\D3D8\XbConvert.h" // EmuXB2PC_D3D11PrimitiveTopology

#include <cstring> // memcpy
#include <wrl/client.h>
using namespace Microsoft::WRL;

// ******************************************************************
// * D3D11 device globals — definitions
// ******************************************************************
IDXGISwapChain                     *g_pSwapChain   = nullptr;
ID3D11DeviceContext                *g_pD3DDeviceContext = nullptr;

// D3D11 render target and depth stencil views for the back buffer
ID3D11RenderTargetView             *g_pD3DBackBufferView = nullptr;
ID3D11DepthStencilView             *g_pD3DDepthStencilView = nullptr;
// D3D11 currently bound render target view (backbuffer or offscreen)
ID3D11RenderTargetView             *g_pD3DCurrentRTV = nullptr;
// D3D11 depth/stencil buffer texture
ID3D11Texture2D                    *g_pD3DDepthStencilBuffer = nullptr;
// D3D11 back buffer texture (used as fallback for dimension queries)
IDirect3DSurface                   *g_pD3DBackBufferSurface = nullptr;
// D3D11 current host render target surface (used for dimension queries)
IDirect3DSurface                   *g_pD3DCurrentHostRenderTarget = nullptr;

// ******************************************************************
// * D3D11 state descriptors — definitions
// ******************************************************************
D3D11_RASTERIZER_DESC    g_D3D11RasterizerDesc = {};
D3D11_DEPTH_STENCIL_DESC g_D3D11DepthStencilDesc = {};
D3D11_BLEND_DESC         g_D3D11BlendDesc = {};

// Dirty flags
bool  g_bD3D11RasterizerStateDirty = true;
bool  g_bD3D11DepthStencilStateDirty = true;
bool  g_bD3D11BlendStateDirty = true;

// Additional OMSet* parameters
UINT  g_D3D11StencilRef = 0;
FLOAT g_D3D11BlendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
UINT  g_D3D11SampleMask = 0xFFFFFFFF;

// ******************************************************************
// * D3D11 state objects (internal — only used by ApplyDirtyStates)
// ******************************************************************
static ID3D11RasterizerState   *g_pD3DRasterizerState = nullptr;
static ID3D11DepthStencilState *g_pD3DDepthStencilState = nullptr;
static ID3D11BlendState        *g_pD3DBlendState = nullptr;

// ******************************************************************
// * Vertex shader constant buffer
// ******************************************************************
       ID3D11Buffer *g_pD3D11VSConstantBuffer = nullptr;
static float         g_D3D11VSConstants[CXBX_D3D11_VS_CB_COUNT][4] = {};
static bool          g_bD3D11VSConstantsDirty = true;

// ******************************************************************
// * Pixel shader constant buffer
// ******************************************************************
       ID3D11Buffer *g_pD3D11PSConstantBuffer = nullptr;
static float         g_D3D11PSConstants[CXBX_D3D11_PS_CB_COUNT][4] = {};
static bool          g_bD3D11PSConstantsDirty = true;

// ******************************************************************
// * Blit shader resources (StretchRect replacement)
// ******************************************************************
static ID3D11VertexShader  *g_pD3D11BlitVS = nullptr;
static ID3D11PixelShader   *g_pD3D11BlitPS = nullptr;
static ID3D11SamplerState  *g_pD3D11BlitSamplerLinear = nullptr;
static ID3D11SamplerState  *g_pD3D11BlitSamplerPoint = nullptr;

// ******************************************************************
// * Shader constant functions
// ******************************************************************
void CxbxSetVertexShaderConstantF(UINT startRegister, const float* pConstantData, UINT Vector4fCount)
{
	if (!g_pD3D11VSConstantBuffer || !pConstantData || Vector4fCount == 0)
		return;

	UINT endRegister = startRegister + Vector4fCount;
	if (endRegister > CXBX_D3D11_VS_CB_COUNT)
		endRegister = CXBX_D3D11_VS_CB_COUNT;

	for (UINT i = startRegister; i < endRegister; i++) {
		memcpy(g_D3D11VSConstants[i], pConstantData + (i - startRegister) * 4, sizeof(float) * 4);
	}
	g_bD3D11VSConstantsDirty = true;
}

void CxbxD3D11FlushVertexShaderConstants()
{
	if (!g_pD3D11VSConstantBuffer || !g_bD3D11VSConstantsDirty)
		return;

	D3D11_MAPPED_SUBRESOURCE mapped = {};
	if (SUCCEEDED(g_pD3DDeviceContext->Map(g_pD3D11VSConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
		memcpy(mapped.pData, g_D3D11VSConstants, sizeof(g_D3D11VSConstants));
		g_pD3DDeviceContext->Unmap(g_pD3D11VSConstantBuffer, 0);
	}
	g_bD3D11VSConstantsDirty = false;
}

void CxbxSetPixelShaderConstantF(UINT startRegister, const float* pConstantData, UINT Vector4fCount)
{
	if (!g_pD3D11PSConstantBuffer || !pConstantData || Vector4fCount == 0)
		return;

	UINT endRegister = startRegister + Vector4fCount;
	if (endRegister > CXBX_D3D11_PS_CB_COUNT)
		endRegister = CXBX_D3D11_PS_CB_COUNT;

	for (UINT i = startRegister; i < endRegister; i++) {
		memcpy(g_D3D11PSConstants[i], pConstantData + (i - startRegister) * 4, sizeof(float) * 4);
	}
	g_bD3D11PSConstantsDirty = true;
}

void CxbxGetVertexShaderConstants(UINT startRegister, float* pConstantData, UINT Vector4fCount)
{
	if (!pConstantData || Vector4fCount == 0)
		return;

	UINT endRegister = startRegister + Vector4fCount;
	if (endRegister > CXBX_D3D11_VS_CB_COUNT)
		endRegister = CXBX_D3D11_VS_CB_COUNT;

	for (UINT i = startRegister; i < endRegister; i++) {
		memcpy(pConstantData + (i - startRegister) * 4, g_D3D11VSConstants[i], sizeof(float) * 4);
	}
}

void CxbxD3D11FlushPixelShaderConstants()
{
	if (!g_pD3D11PSConstantBuffer || !g_bD3D11PSConstantsDirty)
		return;

	D3D11_MAPPED_SUBRESOURCE mapped = {};
	if (SUCCEEDED(g_pD3DDeviceContext->Map(g_pD3D11PSConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
		memcpy(mapped.pData, g_D3D11PSConstants, sizeof(g_D3D11PSConstants));
		g_pD3DDeviceContext->Unmap(g_pD3D11PSConstantBuffer, 0);
	}
	g_bD3D11PSConstantsDirty = false;
}

// ******************************************************************
// * Blit shader init
// ******************************************************************
void CxbxD3D11InitBlit()
{
	LOG_INIT;

	// Full-screen triangle vertex shader (no vertex buffer needed)
	static const char* blitVS =
		"void main(uint id : SV_VertexID, out float4 pos : SV_Position, out float2 uv : TEXCOORD0) {\n"
		"    uv = float2((id << 1) & 2, id & 2);\n"
		"    pos = float4(uv * float2(2, -2) + float2(-1, 1), 0, 1);\n"
		"}\n";

	// Simple texture sampling pixel shader
	static const char* blitPS =
		"Texture2D tex : register(t0);\n"
		"SamplerState samp : register(s0);\n"
		"float4 main(float4 pos : SV_Position, float2 uv : TEXCOORD0) : SV_Target {\n"
		"    return tex.Sample(samp, uv);\n"
		"}\n";

	// Compile vertex shader
	ID3DBlob* pVSBlob = nullptr;
	ID3DBlob* pErrors = nullptr;
	HRESULT hr = D3DCompile(blitVS, strlen(blitVS), "CxbxBlitVS", nullptr, nullptr, "main", "vs_5_0", 0, 0, &pVSBlob, &pErrors);
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to compile blit VS: %s", pErrors ? (char*)pErrors->GetBufferPointer() : "unknown");
		if (pErrors) pErrors->Release();
		return;
	}
	hr = g_pD3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_pD3D11BlitVS);
	pVSBlob->Release();
	if (pErrors) pErrors->Release();
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to create blit VS");
		return;
	}

	// Compile pixel shader
	pErrors = nullptr;
	ID3DBlob* pPSBlob = nullptr;
	hr = D3DCompile(blitPS, strlen(blitPS), "CxbxBlitPS", nullptr, nullptr, "main", "ps_5_0", 0, 0, &pPSBlob, &pErrors);
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to compile blit PS: %s", pErrors ? (char*)pErrors->GetBufferPointer() : "unknown");
		if (pErrors) pErrors->Release();
		return;
	}
	hr = g_pD3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_pD3D11BlitPS);
	pPSBlob->Release();
	if (pErrors) pErrors->Release();
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to create blit PS");
		return;
	}

	// Create linear sampler
	D3D11_SAMPLER_DESC sd = {};
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.MaxLOD = D3D11_FLOAT32_MAX;
	hr = g_pD3DDevice->CreateSamplerState(&sd, &g_pD3D11BlitSamplerLinear);
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to create linear sampler");
	}

	// Create point sampler
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	hr = g_pD3DDevice->CreateSamplerState(&sd, &g_pD3D11BlitSamplerPoint);
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to create point sampler");
	}
}

// ******************************************************************
// * Blit implementation
// ******************************************************************
HRESULT CxbxD3D11Blt(
	ID3D11Texture2D* pSrc, const RECT* pSrcRect,
	ID3D11Texture2D* pDst, const RECT* pDstRect,
	D3DTEXTUREFILTERTYPE Filter)
{
	D3D11_TEXTURE2D_DESC srcDesc, dstDesc;
	pSrc->GetDesc(&srcDesc);
	pDst->GetDesc(&dstDesc);

	// Determine source region
	UINT srcX = pSrcRect ? pSrcRect->left : 0;
	UINT srcY = pSrcRect ? pSrcRect->top : 0;
	UINT srcW = pSrcRect ? (pSrcRect->right - pSrcRect->left) : srcDesc.Width;
	UINT srcH = pSrcRect ? (pSrcRect->bottom - pSrcRect->top) : srcDesc.Height;

	// Determine dest region
	UINT dstX = pDstRect ? pDstRect->left : 0;
	UINT dstY = pDstRect ? pDstRect->top : 0;
	UINT dstW = pDstRect ? (pDstRect->right - pDstRect->left) : dstDesc.Width;
	UINT dstH = pDstRect ? (pDstRect->bottom - pDstRect->top) : dstDesc.Height;

	// Fast path: same-size, same-format copy
	if (srcW == dstW && srcH == dstH && srcDesc.Format == dstDesc.Format) {
		D3D11_BOX srcBox = { srcX, srcY, 0, srcX + srcW, srcY + srcH, 1 };
		g_pD3DDeviceContext->CopySubresourceRegion(pDst, 0, dstX, dstY, 0, pSrc, 0, &srcBox);
		return S_OK;
	}

	// Scaled blit path
	if (!g_pD3D11BlitVS || !g_pD3D11BlitPS) {
		g_pD3DDeviceContext->CopyResource(pDst, pSrc);
		return S_OK;
	}

	// Create temporary SRV for source
	ID3D11ShaderResourceView* pSRV = nullptr;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = srcDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	HRESULT hr = g_pD3DDevice->CreateShaderResourceView(pSrc, &srvDesc, &pSRV);
	if (FAILED(hr)) return hr;

	// Create temporary RTV for destination
	ID3D11RenderTargetView* pRTV = nullptr;
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = dstDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	hr = g_pD3DDevice->CreateRenderTargetView(pDst, &rtvDesc, &pRTV);
	if (FAILED(hr)) {
		pSRV->Release();
		return hr;
	}

	// Save current pipeline state
	ID3D11RenderTargetView* pOldRTV = nullptr;
	ID3D11DepthStencilView* pOldDSV = nullptr;
	g_pD3DDeviceContext->OMGetRenderTargets(1, &pOldRTV, &pOldDSV);
	D3D11_VIEWPORT oldVP;
	UINT numVP = 1;
	g_pD3DDeviceContext->RSGetViewports(&numVP, &oldVP);

	// Set blit pipeline state
	D3D11_VIEWPORT vp = { (FLOAT)dstX, (FLOAT)dstY, (FLOAT)dstW, (FLOAT)dstH, 0.0f, 1.0f };
	g_pD3DDeviceContext->RSSetViewports(1, &vp);
	g_pD3DDeviceContext->OMSetRenderTargets(1, &pRTV, nullptr);
	g_pD3DDeviceContext->VSSetShader(g_pD3D11BlitVS, nullptr, 0);
	g_pD3DDeviceContext->PSSetShader(g_pD3D11BlitPS, nullptr, 0);
	g_pD3DDeviceContext->PSSetShaderResources(0, 1, &pSRV);
	ID3D11SamplerState* pSampler = (Filter == D3DTEXF_LINEAR) ? g_pD3D11BlitSamplerLinear : g_pD3D11BlitSamplerPoint;
	g_pD3DDeviceContext->PSSetSamplers(0, 1, &pSampler);
	g_pD3DDeviceContext->IASetInputLayout(nullptr);
	g_pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw full-screen triangle
	g_pD3DDeviceContext->Draw(3, 0);

	// Restore previous state
	g_pD3DDeviceContext->OMSetRenderTargets(1, &pOldRTV, pOldDSV);
	g_pD3DDeviceContext->RSSetViewports(1, &oldVP);
	if (pOldRTV) pOldRTV->Release();
	if (pOldDSV) pOldDSV->Release();

	// Unbind source SRV to avoid hazard
	ID3D11ShaderResourceView* pNullSRV = nullptr;
	g_pD3DDeviceContext->PSSetShaderResources(0, 1, &pNullSRV);

	pRTV->Release();
	pSRV->Release();

	return S_OK;
}

// ******************************************************************
// * Unified D3D11 render state mapping
// * Called from ApplySimpleRenderState and ApplyComplexRenderState
// * after Xbox→PC value conversion. Updates D3D11 state descriptors
// * and sets dirty flags for deferred state object recreation.
// ******************************************************************
void CxbxD3D11SetRenderState(uint32_t State, uint32_t Value)
{
    switch (State) {
        // ---- Rasterizer state ----
        case xbox::X_D3DRS_FILLMODE:
            switch (Value) {
                case D3DFILL_SOLID:     g_D3D11RasterizerDesc.FillMode = D3D11_FILL_SOLID; break;
                case D3DFILL_WIREFRAME: g_D3D11RasterizerDesc.FillMode = D3D11_FILL_WIREFRAME; break;
                default: break;
            }
            g_bD3D11RasterizerStateDirty = true;
            break;
        case xbox::X_D3DRS_CULLMODE:
            switch (Value) {
                case D3DCULL_NONE: g_D3D11RasterizerDesc.CullMode = D3D11_CULL_NONE; break;
                case D3DCULL_CW:   g_D3D11RasterizerDesc.CullMode = D3D11_CULL_FRONT; break;
                case D3DCULL_CCW:  g_D3D11RasterizerDesc.CullMode = D3D11_CULL_BACK; break;
                default: break;
            }
            g_bD3D11RasterizerStateDirty = true;
            break;
        case xbox::X_D3DRS_ZBIAS:
            g_D3D11RasterizerDesc.DepthBias = (INT)Value;
            g_bD3D11RasterizerStateDirty = true;
            break;
        case xbox::X_D3DRS_EDGEANTIALIAS:
            g_D3D11RasterizerDesc.AntialiasedLineEnable = (Value != 0) ? TRUE : FALSE;
            g_bD3D11RasterizerStateDirty = true;
            break;
        case xbox::X_D3DRS_MULTISAMPLEANTIALIAS:
            g_D3D11RasterizerDesc.MultisampleEnable = (Value != 0) ? TRUE : FALSE;
            g_bD3D11RasterizerStateDirty = true;
            break;

        // ---- Depth/stencil state ----
        case xbox::X_D3DRS_ZENABLE:
            g_D3D11DepthStencilDesc.DepthEnable = (Value != 0) ? TRUE : FALSE;
            g_bD3D11DepthStencilStateDirty = true;
            break;
        case xbox::X_D3DRS_ZWRITEENABLE:
            g_D3D11DepthStencilDesc.DepthWriteMask = Value ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
            g_bD3D11DepthStencilStateDirty = true;
            break;
        case xbox::X_D3DRS_ZFUNC:
            g_D3D11DepthStencilDesc.DepthFunc = (D3D11_COMPARISON_FUNC)Value; // D3D9 and D3D11 comparison func values match
            g_bD3D11DepthStencilStateDirty = true;
            break;
        case xbox::X_D3DRS_STENCILENABLE:
            g_D3D11DepthStencilDesc.StencilEnable = (Value != 0) ? TRUE : FALSE;
            g_bD3D11DepthStencilStateDirty = true;
            break;
        case xbox::X_D3DRS_STENCILFAIL:
            g_D3D11DepthStencilDesc.FrontFace.StencilFailOp = (D3D11_STENCIL_OP)Value;
            g_D3D11DepthStencilDesc.BackFace.StencilFailOp = (D3D11_STENCIL_OP)Value;
            g_bD3D11DepthStencilStateDirty = true;
            break;
        case xbox::X_D3DRS_STENCILZFAIL:
            g_D3D11DepthStencilDesc.FrontFace.StencilDepthFailOp = (D3D11_STENCIL_OP)Value;
            g_D3D11DepthStencilDesc.BackFace.StencilDepthFailOp = (D3D11_STENCIL_OP)Value;
            g_bD3D11DepthStencilStateDirty = true;
            break;
        case xbox::X_D3DRS_STENCILPASS:
            g_D3D11DepthStencilDesc.FrontFace.StencilPassOp = (D3D11_STENCIL_OP)Value;
            g_D3D11DepthStencilDesc.BackFace.StencilPassOp = (D3D11_STENCIL_OP)Value;
            g_bD3D11DepthStencilStateDirty = true;
            break;
        case xbox::X_D3DRS_STENCILFUNC:
            g_D3D11DepthStencilDesc.FrontFace.StencilFunc = (D3D11_COMPARISON_FUNC)Value;
            g_D3D11DepthStencilDesc.BackFace.StencilFunc = (D3D11_COMPARISON_FUNC)Value;
            g_bD3D11DepthStencilStateDirty = true;
            break;
        case xbox::X_D3DRS_STENCILREF:
            g_D3D11StencilRef = Value;
            g_bD3D11DepthStencilStateDirty = true; // re-apply to pass new ref
            break;
        case xbox::X_D3DRS_STENCILMASK:
            g_D3D11DepthStencilDesc.StencilReadMask = (UINT8)Value;
            g_bD3D11DepthStencilStateDirty = true;
            break;
        case xbox::X_D3DRS_STENCILWRITEMASK:
            g_D3D11DepthStencilDesc.StencilWriteMask = (UINT8)Value;
            g_bD3D11DepthStencilStateDirty = true;
            break;

        // ---- Blend state ----
        case xbox::X_D3DRS_ALPHABLENDENABLE:
            g_D3D11BlendDesc.RenderTarget[0].BlendEnable = (Value != 0) ? TRUE : FALSE;
            g_bD3D11BlendStateDirty = true;
            break;
        case xbox::X_D3DRS_SRCBLEND:
            g_D3D11BlendDesc.RenderTarget[0].SrcBlend = (D3D11_BLEND)Value;
            g_D3D11BlendDesc.RenderTarget[0].SrcBlendAlpha = (D3D11_BLEND)Value;
            g_bD3D11BlendStateDirty = true;
            break;
        case xbox::X_D3DRS_DESTBLEND:
            g_D3D11BlendDesc.RenderTarget[0].DestBlend = (D3D11_BLEND)Value;
            g_D3D11BlendDesc.RenderTarget[0].DestBlendAlpha = (D3D11_BLEND)Value;
            g_bD3D11BlendStateDirty = true;
            break;
        case xbox::X_D3DRS_BLENDOP:
            g_D3D11BlendDesc.RenderTarget[0].BlendOp = (D3D11_BLEND_OP)Value;
            g_D3D11BlendDesc.RenderTarget[0].BlendOpAlpha = (D3D11_BLEND_OP)Value;
            g_bD3D11BlendStateDirty = true;
            break;
        case xbox::X_D3DRS_BLENDCOLOR: {
            // Convert ARGB DWORD to float4 blend factor
            g_D3D11BlendFactor[0] = ((Value >> 16) & 0xFF) / 255.0f; // R
            g_D3D11BlendFactor[1] = ((Value >> 8) & 0xFF) / 255.0f;  // G
            g_D3D11BlendFactor[2] = (Value & 0xFF) / 255.0f;         // B
            g_D3D11BlendFactor[3] = ((Value >> 24) & 0xFF) / 255.0f;  // A
            g_bD3D11BlendStateDirty = true; // re-apply to pass new factor
            break;
        }
        case xbox::X_D3DRS_COLORWRITEENABLE:
            g_D3D11BlendDesc.RenderTarget[0].RenderTargetWriteMask = (UINT8)Value;
            g_bD3D11BlendStateDirty = true;
            break;
        case xbox::X_D3DRS_MULTISAMPLEMASK:
            g_D3D11SampleMask = Value;
            g_bD3D11BlendStateDirty = true; // re-apply to pass new sample mask
            break;

        // ---- States handled as shader constants ----
        case xbox::X_D3DRS_ALPHATESTENABLE:
        case xbox::X_D3DRS_ALPHAREF:
        case xbox::X_D3DRS_ALPHAFUNC:
        case xbox::X_D3DRS_SHADEMODE:
        case xbox::X_D3DRS_DITHERENABLE:
        case xbox::X_D3DRS_FOGCOLOR:
        case xbox::X_D3DRS_NORMALIZENORMALS:
        case xbox::X_D3DRS_TEXTUREFACTOR:
            // These are read from xbox render state by the fixed function shader
            break;
        default:
            break;
    }
}

// ******************************************************************
// * Apply dirty states
// ******************************************************************
void CxbxD3D11ApplyDirtyStates()
{
	LOG_INIT;

	if (g_bD3D11RasterizerStateDirty) {
		if (g_pD3DRasterizerState) { g_pD3DRasterizerState->Release(); g_pD3DRasterizerState = nullptr; }
		HRESULT hr = g_pD3DDevice->CreateRasterizerState(&g_D3D11RasterizerDesc, &g_pD3DRasterizerState);
		DEBUG_D3DRESULT(hr, "g_pD3DDevice->CreateRasterizerState");
		if (SUCCEEDED(hr)) {
			g_pD3DDeviceContext->RSSetState(g_pD3DRasterizerState);
		}
		g_bD3D11RasterizerStateDirty = false;
	}

	if (g_bD3D11DepthStencilStateDirty) {
		if (g_pD3DDepthStencilState) { g_pD3DDepthStencilState->Release(); g_pD3DDepthStencilState = nullptr; }
		HRESULT hr = g_pD3DDevice->CreateDepthStencilState(&g_D3D11DepthStencilDesc, &g_pD3DDepthStencilState);
		DEBUG_D3DRESULT(hr, "g_pD3DDevice->CreateDepthStencilState");
		if (SUCCEEDED(hr)) {
			g_pD3DDeviceContext->OMSetDepthStencilState(g_pD3DDepthStencilState, g_D3D11StencilRef);
		}
		g_bD3D11DepthStencilStateDirty = false;
	}

	if (g_bD3D11BlendStateDirty) {
		if (g_pD3DBlendState) { g_pD3DBlendState->Release(); g_pD3DBlendState = nullptr; }
		HRESULT hr = g_pD3DDevice->CreateBlendState(&g_D3D11BlendDesc, &g_pD3DBlendState);
		DEBUG_D3DRESULT(hr, "g_pD3DDevice->CreateBlendState");
		if (SUCCEEDED(hr)) {
			g_pD3DDeviceContext->OMSetBlendState(g_pD3DBlendState, g_D3D11BlendFactor, g_D3D11SampleMask);
		}
		g_bD3D11BlendStateDirty = false;
	}

	CxbxD3D11FlushVertexShaderConstants();
	CxbxD3D11FlushPixelShaderConstants();
}

// ******************************************************************
// * Release all backend resources (called from device release lambda)
// ******************************************************************
void CxbxD3D11ReleaseBackendResources()
{
	if (g_pD3DBlendState) { g_pD3DBlendState->Release(); g_pD3DBlendState = nullptr; }
	if (g_pD3DDepthStencilState) { g_pD3DDepthStencilState->Release(); g_pD3DDepthStencilState = nullptr; }
	if (g_pD3DRasterizerState) { g_pD3DRasterizerState->Release(); g_pD3DRasterizerState = nullptr; }
	if (g_pD3D11VSConstantBuffer) { g_pD3D11VSConstantBuffer->Release(); g_pD3D11VSConstantBuffer = nullptr; }
	if (g_pD3D11PSConstantBuffer) { g_pD3D11PSConstantBuffer->Release(); g_pD3D11PSConstantBuffer = nullptr; }
	if (g_pD3D11BlitVS) { g_pD3D11BlitVS->Release(); g_pD3D11BlitVS = nullptr; }
	if (g_pD3D11BlitPS) { g_pD3D11BlitPS->Release(); g_pD3D11BlitPS = nullptr; }
	if (g_pD3D11BlitSamplerLinear) { g_pD3D11BlitSamplerLinear->Release(); g_pD3D11BlitSamplerLinear = nullptr; }
	if (g_pD3D11BlitSamplerPoint) { g_pD3D11BlitSamplerPoint->Release(); g_pD3D11BlitSamplerPoint = nullptr; }
}

// ******************************************************************
// * Rendering helpers (D3D11 implementations)
// ******************************************************************
HRESULT CxbxSetRenderTarget(IDirect3DSurface* pHostRenderTarget)
{
	LOG_INIT;
	HRESULT hRet;
	if (pHostRenderTarget == nullptr) {
		g_pD3DCurrentHostRenderTarget = g_pD3DBackBufferSurface;
		if (g_pD3DCurrentRTV != nullptr && g_pD3DCurrentRTV != g_pD3DBackBufferView) {
			g_pD3DCurrentRTV->Release();
		}
		g_pD3DCurrentRTV = g_pD3DBackBufferView;
		g_pD3DDeviceContext->OMSetRenderTargets(1, &g_pD3DBackBufferView, g_pD3DDepthStencilView);
		hRet = S_OK;
	} else {
		g_pD3DCurrentHostRenderTarget = pHostRenderTarget;
		D3D11_TEXTURE2D_DESC textureDesc = {};
		pHostRenderTarget->GetDesc(&textureDesc);

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		ComPtr<ID3D11RenderTargetView> renderTargetView;
		hRet = g_pD3DDevice->CreateRenderTargetView((ID3D11Resource*)pHostRenderTarget, &renderTargetViewDesc, renderTargetView.GetAddressOf());
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateRenderTargetView");

		if (SUCCEEDED(hRet)) {
			if (g_pD3DCurrentRTV != nullptr && g_pD3DCurrentRTV != g_pD3DBackBufferView) {
				g_pD3DCurrentRTV->Release();
			}
			g_pD3DCurrentRTV = renderTargetView.Get();
			g_pD3DCurrentRTV->AddRef();
			g_pD3DDeviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), g_pD3DDepthStencilView);
		}
	}
	return hRet;
}

void CxbxD3DClear(DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
	LOG_INIT;
	FLOAT clearColor[4];
	clearColor[0] = ((Color >> 16) & 0xFF) / 255.0f;
	clearColor[1] = ((Color >>  8) & 0xFF) / 255.0f;
	clearColor[2] = ((Color >>  0) & 0xFF) / 255.0f;
	clearColor[3] = ((Color >> 24) & 0xFF) / 255.0f;

	if ((Flags & D3DCLEAR_TARGET) && g_pD3DCurrentRTV != nullptr) {
		if (Count > 0 && pRects != nullptr) {
			ComPtr<ID3D11DeviceContext1> context1;
			if (SUCCEEDED(g_pD3DDeviceContext->QueryInterface(IID_PPV_ARGS(context1.GetAddressOf())))) {
				context1->ClearView(g_pD3DCurrentRTV, clearColor, (const D3D11_RECT*)pRects, Count);
			}
		} else {
			g_pD3DDeviceContext->ClearRenderTargetView(g_pD3DCurrentRTV, clearColor);
		}
	}

	if (g_pD3DDepthStencilView != nullptr) {
		UINT clearFlags = 0;
		if (Flags & D3DCLEAR_ZBUFFER) clearFlags |= D3D11_CLEAR_DEPTH;
		if (Flags & D3DCLEAR_STENCIL) clearFlags |= D3D11_CLEAR_STENCIL;
		if (clearFlags != 0) {
			g_pD3DDeviceContext->ClearDepthStencilView(g_pD3DDepthStencilView, clearFlags, Z, (UINT8)Stencil);
		}
	}
}

void CxbxSetViewport(D3DVIEWPORT *pHostViewport)
{
	g_pD3DDeviceContext->RSSetViewports(1, pHostViewport);
}

void CxbxSetScissorRect(CONST RECT *pHostViewportRect)
{
	g_pD3DDeviceContext->RSSetScissorRects(1, pHostViewportRect);
}

void CxbxSetIndices(IDirect3DIndexBuffer* pHostIndexBuffer)
{
	g_pD3DDeviceContext->IASetIndexBuffer(pHostIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
}

INDEX16* CxbxLockIndexBuffer(IDirect3DIndexBuffer* pHostIndexBuffer)
{
	LOG_INIT;
	INDEX16* pData = nullptr;
	D3D11_MAPPED_SUBRESOURCE mapped = {};
	HRESULT hRet = g_pD3DDeviceContext->Map(pHostIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	DEBUG_D3DRESULT(hRet, "CxbxLockIndexBuffer: Map");
	if (SUCCEEDED(hRet))
		pData = (INDEX16*)mapped.pData;
	return pData;
}

void CxbxUnlockIndexBuffer(IDirect3DIndexBuffer* pHostIndexBuffer)
{
	g_pD3DDeviceContext->Unmap(pHostIndexBuffer, 0);
}

HRESULT CxbxDrawIndexedPrimitive(xbox::X_D3DPRIMITIVETYPE XboxPrimitiveType, UINT IndexCount, INT BaseVertexIndex, UINT StartIndex, UINT MinIndex, UINT NumVertices, UINT PrimCount)
{
	g_pD3DDeviceContext->IASetPrimitiveTopology(EmuXB2PC_D3D11PrimitiveTopology(XboxPrimitiveType));
	g_pD3DDeviceContext->DrawIndexed(IndexCount, StartIndex, BaseVertexIndex);
	return S_OK;
}

HRESULT CxbxDrawPrimitive(xbox::X_D3DPRIMITIVETYPE XboxPrimitiveType, UINT VertexCount, UINT StartVertex, UINT PrimCount)
{
	g_pD3DDeviceContext->IASetPrimitiveTopology(EmuXB2PC_D3D11PrimitiveTopology(XboxPrimitiveType));
	g_pD3DDeviceContext->Draw(VertexCount, StartVertex);
	return S_OK;
}

HRESULT CxbxBltSurface(IDirect3DSurface* pSrc, const RECT* pSrcRect, IDirect3DSurface* pDst, const RECT* pDstRect, D3DTEXTUREFILTERTYPE Filter)
{
	return CxbxD3D11Blt(pSrc, pSrcRect, pDst, pDstRect, Filter);
}

HRESULT CxbxPresent()
{
	LOG_INIT;
	CxbxEndScene();
	HRESULT hRet = g_pSwapChain->Present(0, 0);
	DEBUG_D3DRESULT(hRet, "g_pSwapChain->Present");
	CxbxBeginScene();
	return hRet;
}

void CxbxSetDepthStencilSurface(IDirect3DSurface* pHostDepthStencil)
{
	ID3D11DepthStencilView* pDSV = nullptr;
	if (pHostDepthStencil != nullptr) {
		D3D11_TEXTURE2D_DESC texDesc = {};
		pHostDepthStencil->GetDesc(&texDesc);
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = texDesc.Format;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;
		g_pD3DDevice->CreateDepthStencilView(pHostDepthStencil, &dsvDesc, &pDSV);
	}
	if (g_pD3DDepthStencilView) { g_pD3DDepthStencilView->Release(); }
	g_pD3DDepthStencilView = pDSV;
	g_pD3DDeviceContext->OMSetRenderTargets(1, &g_pD3DCurrentRTV, g_pD3DDepthStencilView);
}

IDirect3DSurface* CxbxGetCurrentRenderTarget()
{
	return g_pD3DCurrentHostRenderTarget;
}

HRESULT CxbxGetBackBuffer(IDirect3DSurface** ppBackBuffer)
{
	return g_pSwapChain->GetBuffer(0, __uuidof(IDirect3DSurface), reinterpret_cast<void**>(ppBackBuffer));
}

HRESULT CxbxSetStreamSource(UINT HostStreamNumber, IDirect3DVertexBuffer* pHostVertexBuffer, UINT VertexStride)
{
	UINT offset = 0;
	g_pD3DDeviceContext->IASetVertexBuffers(HostStreamNumber, 1, &pHostVertexBuffer, &VertexStride, &offset);
	return S_OK;
}

HRESULT CxbxCreateVertexBuffer(UINT Length, IDirect3DVertexBuffer** ppVertexBuffer)
{
	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.ByteWidth = Length;
	bufDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	return g_pD3DDevice->CreateBuffer(&bufDesc, nullptr, ppVertexBuffer);
}

void* CxbxLockVertexBuffer(IDirect3DVertexBuffer* pVertexBuffer)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	if (FAILED(g_pD3DDeviceContext->Map(pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
		return nullptr;
	}
	return mappedResource.pData;
}

void CxbxUnlockVertexBuffer(IDirect3DVertexBuffer* pVertexBuffer)
{
	g_pD3DDeviceContext->Unmap(pVertexBuffer, 0);
}

ID3D11Buffer *CxbxDynBuffer::Update(const void *pData, UINT size)
{
	if (size == 0)
		return nullptr;

	// Grow the buffer if it's too small (or doesn't exist yet)
	if (pBuffer == nullptr || capacity < size) {
		Release();
		// Round up to next power-of-two-ish size to avoid frequent re-allocs
		UINT newCap = (size < 4096) ? 4096 : size;
		// Round up to next multiple of 4096
		newCap = (newCap + 4095) & ~4095u;

		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = newCap;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = bindFlags;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		HRESULT hr = g_pD3DDevice->CreateBuffer(&desc, nullptr, &pBuffer);
		if (FAILED(hr) || pBuffer == nullptr)
			return nullptr;
		capacity = newCap;
	}

	// Map-discard and upload
	D3D11_MAPPED_SUBRESOURCE mapped = {};
	HRESULT hr = g_pD3DDeviceContext->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (FAILED(hr))
		return nullptr;
	memcpy(mapped.pData, pData, size);
	g_pD3DDeviceContext->Unmap(pBuffer, 0);
	return pBuffer;
}

void CxbxDynBuffer::Release()
{
	if (pBuffer != nullptr) {
		pBuffer->Release();
		pBuffer = nullptr;
	}
	capacity = 0;
}

HRESULT CxbxCreatePixelShader(const void* pFunction, SIZE_T FunctionSize, IDirect3DPixelShader** ppShader)
{
	return g_pD3DDevice->CreatePixelShader(pFunction, FunctionSize, nullptr, ppShader);
}

void CxbxRawSetPixelShader(IDirect3DPixelShader* pPixelShader)
{
	g_pD3DDeviceContext->PSSetShader(pPixelShader, nullptr, 0);
}

#endif // CXBX_USE_D3D11
