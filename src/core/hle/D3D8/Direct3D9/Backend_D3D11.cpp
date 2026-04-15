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
#include "Direct3D9.h" // g_pD3DDevice, g_pD3DDeviceContext
#include "core\kernel\init\CxbxKrnl.h" // LOG_INIT, EmuLog
#include "core\hle\D3D8\XbD3D8Logging.h" // DEBUG_D3DRESULT
#include "core\hle\D3D8\Direct3D9\Shader.h" // D3DCompile (via d3dcompiler.h)

#include <cstring> // memcpy

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

#endif // CXBX_USE_D3D11
