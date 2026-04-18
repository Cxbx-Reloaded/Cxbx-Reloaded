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

#include "Backend_D3D11_Internal.h"

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
   	   	case xbox::X_D3DRS_ZBIAS: {
   	   	   	// Value arrives as a float-encoded DWORD (converted by ApplyComplexRenderState)
   	   	   	// D3D11 DepthBias is an integer scaled by the depth buffer's minimum representable value
   	   	   	// For D24: DepthBias * (1 / 2^24). Convert the D3D9 float bias to D3D11 integer bias.
   	   	   	float fBias; std::memcpy(&fBias, &Value, sizeof(fBias));
   	   	   	g_D3D11RasterizerDesc.DepthBias = static_cast<INT>(fBias * (float)(1 << 24));
   	   	   	g_D3D11RasterizerDesc.DepthBiasClamp = 0.0f;
   	   	   	g_D3D11RasterizerDesc.SlopeScaledDepthBias = 0.0f;
   	   	   	g_bD3D11RasterizerStateDirty = true;
   	   	} break;
   	   	case xbox::X_D3DRS_EDGEANTIALIAS:
   	   	   	g_D3D11RasterizerDesc.AntialiasedLineEnable = (Value != 0) ? TRUE : FALSE;
   	   	   	g_bD3D11RasterizerStateDirty = true;
   	   	   	break;
   	   	case xbox::X_D3DRS_MULTISAMPLEANTIALIAS:
   	   	   	g_D3D11RasterizerDesc.MultisampleEnable = (Value != 0) ? TRUE : FALSE;
   	   	   	g_bD3D11RasterizerStateDirty = true;
   	   	   	break;
   	   	case xbox::X_D3DRS_FRONTFACE:
   	   	   	// Xbox FRONTFACE: 0x900 = CW, 0x901 = CCW (NV2A values)
   	   	   	g_D3D11RasterizerDesc.FrontCounterClockwise = (Value != 0x900) ? TRUE : FALSE;
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
   	   	// ---- Point sprite state ----
   	   	case xbox::X_D3DRS_POINTSPRITEENABLE:
   	   	   	g_bPointSpriteEnabled = (Value != 0);
   	   	   	break;
   	   	// ---- Line width (Xbox extension, float-encoded DWORD) ----
   	   	case xbox::X_D3DRS_LINEWIDTH:
   	   	   	float fVal; std::memcpy(&fVal, &Value, sizeof(fVal)); g_fLineWidth = fVal;
   	   	   	if (g_fLineWidth < 1.0f) g_fLineWidth = 1.0f;
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
		HRESULT hr = g_pD3DDevice->CreateRasterizerState(&g_D3D11RasterizerDesc, g_pD3DRasterizerState.ReleaseAndGetAddressOf());
		DEBUG_D3DRESULT(hr, "g_pD3DDevice->CreateRasterizerState");
		if (SUCCEEDED(hr)) {
			g_pD3DDeviceContext->RSSetState(g_pD3DRasterizerState.Get());
		}
		g_bD3D11RasterizerStateDirty = false;
	}

	if (g_bD3D11DepthStencilStateDirty) {
		HRESULT hr = g_pD3DDevice->CreateDepthStencilState(&g_D3D11DepthStencilDesc, g_pD3DDepthStencilState.ReleaseAndGetAddressOf());
		DEBUG_D3DRESULT(hr, "g_pD3DDevice->CreateDepthStencilState");
		if (SUCCEEDED(hr)) {
			g_pD3DDeviceContext->OMSetDepthStencilState(g_pD3DDepthStencilState.Get(), g_D3D11StencilRef);
		}
		g_bD3D11DepthStencilStateDirty = false;
	}

	if (g_bD3D11BlendStateDirty) {
		// Diagnostic: log blend state (first 5 occurrences only)
		{
			static int s_blendDiag = 0;
			if (s_blendDiag < 5) {
				s_blendDiag++;
				auto& rt = g_D3D11BlendDesc.RenderTarget[0];
				EmuLog(LOG_LEVEL::INFO, "Blend diag [%d]: Enable=%d Src=%d Dst=%d Op=%d SrcA=%d DstA=%d OpA=%d WriteMask=0x%X",
					s_blendDiag, rt.BlendEnable, rt.SrcBlend, rt.DestBlend, rt.BlendOp,
					rt.SrcBlendAlpha, rt.DestBlendAlpha, rt.BlendOpAlpha, rt.RenderTargetWriteMask);
			}
		}
		HRESULT hr = g_pD3DDevice->CreateBlendState(&g_D3D11BlendDesc, g_pD3DBlendState.ReleaseAndGetAddressOf());
		DEBUG_D3DRESULT(hr, "g_pD3DDevice->CreateBlendState");
		if (SUCCEEDED(hr)) {
			g_pD3DDeviceContext->OMSetBlendState(g_pD3DBlendState.Get(), g_D3D11BlendFactor, g_D3D11SampleMask);
		}
		g_bD3D11BlendStateDirty = false;
	}

	CxbxD3D11FlushVertexShaderConstants();
	CxbxD3D11FlushPixelShaderConstants();

	// Update GS constant buffer (shared by point sprite and thick line GS)
	// xy = inverse viewport dimensions, z = line width, w = unused
	{
		D3D11_VIEWPORT vp = {};
		UINT numVP = 1;
		g_pD3DDeviceContext->RSGetViewports(&numVP, &vp);
		if (vp.Width > 0 && vp.Height > 0 && g_pD3D11GSConstantBuffer) {
			float gsConstants[4] = { 1.0f / vp.Width, 1.0f / vp.Height, g_fLineWidth, 0.0f };
			CxbxD3D11UpdateDynamicBuffer(g_pD3D11GSConstantBuffer, gsConstants, sizeof(gsConstants));
			g_pD3DDeviceContext->GSSetConstantBuffers(0, 1, &g_pD3D11GSConstantBuffer);
		}
	}

	// Bind or unbind the point sprite geometry shader
	// (Thick line GS is bound at draw time since it depends on primitive type)
	if (g_bPointSpriteEnabled && g_pD3D11PointSpriteGS) {
		g_pD3DDeviceContext->GSSetShader(g_pD3D11PointSpriteGS, nullptr, 0);
	} else {
		g_pD3DDeviceContext->GSSetShader(nullptr, nullptr, 0);
	}
}

// RTV cache: maps texture pointer to its render target view, avoiding
// redundant CreateRenderTargetView calls for the same texture.
std::unordered_map<IDirect3DSurface*, ID3D11RenderTargetView*> g_RTVCache;

void ClearRTVCache()
{
	for (auto &pair : g_RTVCache) {
		if (pair.second) pair.second->Release();
	}
	g_RTVCache.clear();
}

// ******************************************************************
// * Thick line GS bind/unbind helpers
// ******************************************************************
static bool CxbxIsLinePrimitive(xbox::X_D3DPRIMITIVETYPE type)
{
	return type == xbox::X_D3DPT_LINELIST
	   	|| type == xbox::X_D3DPT_LINESTRIP
	   	|| type == xbox::X_D3DPT_LINELOOP;
}

void CxbxBindThickLineGS(xbox::X_D3DPRIMITIVETYPE type)
{
	if (g_fLineWidth > 1.0f && CxbxIsLinePrimitive(type) && g_pD3D11ThickLineGS) {
		g_pD3DDeviceContext->GSSetShader(g_pD3D11ThickLineGS, nullptr, 0);
	}
}

void CxbxUnbindThickLineGS(xbox::X_D3DPRIMITIVETYPE type)
{
	if (g_fLineWidth > 1.0f && CxbxIsLinePrimitive(type) && g_pD3D11ThickLineGS) {
		// Restore point sprite GS or null
		if (g_bPointSpriteEnabled && g_pD3D11PointSpriteGS) {
			g_pD3DDeviceContext->GSSetShader(g_pD3D11PointSpriteGS, nullptr, 0);
		} else {
			g_pD3DDeviceContext->GSSetShader(nullptr, nullptr, 0);
		}
	}
}

#endif // CXBX_USE_D3D11
