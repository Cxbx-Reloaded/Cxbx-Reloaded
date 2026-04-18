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
// * Initialize with valid D3D11 defaults so that the first
// * CreateXxxState() call (triggered by the dirty flags below)
// * succeeds even if the game hasn't set every render state yet.
// ******************************************************************
D3D11_RASTERIZER_DESC    g_D3D11RasterizerDesc = {
	/* FillMode              */ D3D11_FILL_SOLID,
	/* CullMode              */ D3D11_CULL_BACK,
	/* FrontCounterClockwise */ FALSE,
	/* DepthBias             */ 0,
	/* DepthBiasClamp        */ 0.0f,
	/* SlopeScaledDepthBias  */ 0.0f,
	/* DepthClipEnable       */ TRUE,
	/* ScissorEnable         */ FALSE,
	/* MultisampleEnable     */ FALSE,
	/* AntialiasedLineEnable */ FALSE,
};
D3D11_DEPTH_STENCIL_DESC g_D3D11DepthStencilDesc = {
	/* DepthEnable           */ TRUE,
	/* DepthWriteMask        */ D3D11_DEPTH_WRITE_MASK_ALL,
	/* DepthFunc             */ D3D11_COMPARISON_LESS,
	/* StencilEnable         */ FALSE,
	/* StencilReadMask       */ D3D11_DEFAULT_STENCIL_READ_MASK,
	/* StencilWriteMask      */ D3D11_DEFAULT_STENCIL_WRITE_MASK,
	/* FrontFace             */ { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS },
	/* BackFace              */ { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS },
};
D3D11_BLEND_DESC         g_D3D11BlendDesc = {
	/* AlphaToCoverageEnable  */ FALSE,
	/* IndependentBlendEnable */ FALSE,
	/* RenderTarget[0]        */ {
		/* BlendEnable           */ FALSE,
		/* SrcBlend              */ D3D11_BLEND_ONE,
		/* DestBlend             */ D3D11_BLEND_ZERO,
		/* BlendOp               */ D3D11_BLEND_OP_ADD,
		/* SrcBlendAlpha         */ D3D11_BLEND_ONE,
		/* DestBlendAlpha        */ D3D11_BLEND_ZERO,
		/* BlendOpAlpha          */ D3D11_BLEND_OP_ADD,
		/* RenderTargetWriteMask */ D3D11_COLOR_WRITE_ENABLE_ALL,
	},
};

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
ComPtr<ID3D11RasterizerState>   g_pD3DRasterizerState;
ComPtr<ID3D11DepthStencilState> g_pD3DDepthStencilState;
ComPtr<ID3D11BlendState>        g_pD3DBlendState;

// ******************************************************************
// * Vertex shader constant buffer
// ******************************************************************
   	   ID3D11Buffer *g_pD3D11VSConstantBuffer = nullptr;
float         g_D3D11VSConstants[CXBX_D3D11_VS_CB_COUNT][4] = {};
bool          g_bD3D11VSConstantsDirty = true;

// ******************************************************************
// * Pixel shader constant buffer
// ******************************************************************
   	   ID3D11Buffer *g_pD3D11PSConstantBuffer = nullptr;
float         g_D3D11PSConstants[CXBX_D3D11_PS_CB_COUNT][4] = {};
bool          g_bD3D11PSConstantsDirty = true;

// ******************************************************************
// * Blit shader resources (StretchRect replacement)
// ******************************************************************
ID3D11VertexShader  *g_pD3D11BlitVS = nullptr;
ID3D11PixelShader   *g_pD3D11BlitPS = nullptr;
ID3D11SamplerState  *g_pD3D11BlitSamplerLinear = nullptr;
ID3D11SamplerState  *g_pD3D11BlitSamplerPoint = nullptr;

// ******************************************************************
// * Point sprite geometry shader resources
// ******************************************************************
ID3D11GeometryShader *g_pD3D11PointSpriteGS = nullptr;
ID3D11Buffer         *g_pD3D11GSConstantBuffer = nullptr;
bool                  g_bPointSpriteEnabled = false;

// ******************************************************************
// * Thick line geometry shader resources
// ******************************************************************
ID3D11GeometryShader *g_pD3D11ThickLineGS = nullptr;
float                 g_fLineWidth = 1.0f;

// ******************************************************************
// * Compute shader unswizzle resources
// ******************************************************************
ID3D11ComputeShader  *g_pD3D11UnswizzleCS = nullptr;
ID3D11Buffer         *g_pD3D11UnswizzleCB = nullptr; // constant buffer: maskX, maskY, width, bpp
ID3D11Buffer         *g_pD3D11UnswizzleStagingBuf = nullptr; // reusable ByteAddressBuffer for upload
UINT                  g_UnswizzleStagingBufSize = 0;
ID3D11ShaderResourceView *g_pD3D11UnswizzleSRV = nullptr; // SRV for staging buffer

// ******************************************************************
// * Compute shader index buffer conversion resources
// ******************************************************************
ID3D11ComputeShader       *g_pD3D11IndexConvertCS = nullptr;
ID3D11Buffer              *g_pD3D11IndexConvertCB = nullptr; // constant buffer: vertexCount, mode, isIndexed, pad
ID3D11Buffer              *g_pD3D11IndexConvertInputBuf = nullptr; // ByteAddressBuffer for source indices
UINT                       g_IndexConvertInputBufSize = 0;
ID3D11ShaderResourceView  *g_pD3D11IndexConvertInputSRV = nullptr;
ID3D11Buffer              *g_pD3D11IndexConvertOutputBuf = nullptr; // output buffer (INDEX_BUFFER + UAV)
UINT                       g_IndexConvertOutputBufSize = 0;
ID3D11UnorderedAccessView *g_pD3D11IndexConvertOutputUAV = nullptr;

// ******************************************************************
// * Compute shader palette texture expansion resources
// ******************************************************************
ID3D11ComputeShader       *g_pD3D11PaletteExpandCS = nullptr;
ID3D11Buffer              *g_pD3D11PaletteExpandCB = nullptr; // constant buffer: maskX, maskY, width, pad
ID3D11Buffer              *g_pD3D11PaletteBuf = nullptr; // 256-entry palette upload buffer
ID3D11ShaderResourceView  *g_pD3D11PaletteSRV = nullptr;

// ******************************************************************
// * Compute shader format conversion resources
// ******************************************************************
ID3D11ComputeShader       *g_pD3D11FormatConvertCS = nullptr;
ID3D11Buffer              *g_pD3D11FormatConvertCB = nullptr; // constant buffer: maskX, maskY, width, height, bpp, fmtType, swizzled, pad

// ******************************************************************
// * Compute shader vertex format conversion resources
// ******************************************************************
ID3D11ComputeShader       *g_pD3D11VertexConvertCS = nullptr;
ID3D11Buffer              *g_pD3D11VertexConvertCB = nullptr; // constant buffer: header + 16 element descriptors
ID3D11Buffer              *g_pD3D11VertexConvertSrcBuf = nullptr; // staging ByteAddressBuffer for source vertices
UINT                       g_VertexConvertSrcBufSize = 0;
ID3D11ShaderResourceView  *g_pD3D11VertexConvertSrcSRV = nullptr;

// Index conversion mode constants
#define CXBX_INDEX_CONVERT_QUAD_CW  0
#define CXBX_INDEX_CONVERT_QUAD_CCW 1
#define CXBX_INDEX_CONVERT_FAN      2

// Vertex format conversion type constants
#define CXBX_VTXCONV_COPY       0
#define CXBX_VTXCONV_NORMSHORT3 1
#define CXBX_VTXCONV_NORMPACKED3 2
#define CXBX_VTXCONV_SHORT3     3
#define CXBX_VTXCONV_PBYTE3     4
#define CXBX_VTXCONV_FLOAT2H    5
#define CXBX_VTXCONV_D3DCOLOR   6
#define CXBX_VTXCONV_NONE       7

// ******************************************************************
// * D3D11 buffer/view creation helpers
// ******************************************************************

// Create a constant buffer. If bDynamic, uses DYNAMIC + CPU_ACCESS_WRITE
// (for Map/Unmap); otherwise DEFAULT (for UpdateSubresource).
HRESULT CxbxD3D11CreateConstantBuffer(UINT byteWidth, bool bDynamic, ID3D11Buffer** ppBuffer)
{
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = byteWidth;
	desc.Usage = bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = bDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	return g_pD3DDevice->CreateBuffer(&desc, nullptr, ppBuffer);
}

// Create a ByteAddressBuffer (BUFFER_ALLOW_RAW_VIEWS) with optional CPU write access.
// If bDynamic, uses DYNAMIC + CPU_ACCESS_WRITE; otherwise DEFAULT.
HRESULT CxbxD3D11CreateRawBuffer(UINT byteWidth, bool bDynamic, UINT extraBindFlags, ID3D11Buffer** ppBuffer)
{
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = byteWidth;
	desc.Usage = bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | extraBindFlags;
	desc.CPUAccessFlags = bDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	return g_pD3DDevice->CreateBuffer(&desc, nullptr, ppBuffer);
}

// Create a R32_TYPELESS raw SRV (for ByteAddressBuffer access in shaders).
HRESULT CxbxD3D11CreateRawBufferSRV(ID3D11Buffer* pBuffer, UINT byteWidth, ID3D11ShaderResourceView** ppSRV)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
	desc.BufferEx.NumElements = byteWidth / 4;
	return g_pD3DDevice->CreateShaderResourceView(pBuffer, &desc, ppSRV);
}

// Create a R32_UINT typed UAV for a buffer.
HRESULT CxbxD3D11CreateTypedBufferUAV(ID3D11Buffer* pBuffer, UINT byteWidth, ID3D11UnorderedAccessView** ppUAV)
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
	desc.Format = DXGI_FORMAT_R32_UINT;
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = byteWidth / 4;
	return g_pD3DDevice->CreateUnorderedAccessView(pBuffer, &desc, ppUAV);
}

// Ensure a dynamic raw staging buffer + SRV pair are at least requiredSize bytes.
// Rounds up to 4KB. Releases and re-creates if too small.
void CxbxD3D11EnsureRawStagingBuffer(
	UINT requiredSize,
	ID3D11Buffer** ppBuffer, UINT* pCurrentSize,
	ID3D11ShaderResourceView** ppSRV,
	const char* debugName)
{
	requiredSize = (requiredSize + 4095) & ~4095u;
	if (*ppBuffer && *pCurrentSize >= requiredSize)
		return;

	if (*ppSRV) { (*ppSRV)->Release(); *ppSRV = nullptr; }
	if (*ppBuffer) { (*ppBuffer)->Release(); *ppBuffer = nullptr; }

	HRESULT hr = CxbxD3D11CreateRawBuffer(requiredSize, /*bDynamic=*/true, 0, ppBuffer);
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "%s: Failed to create buffer (%u bytes)", debugName, requiredSize);
		return;
	}
	*pCurrentSize = requiredSize;

	hr = CxbxD3D11CreateRawBufferSRV(*ppBuffer, requiredSize, ppSRV);
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "%s: Failed to create SRV", debugName);
	}
}

// Bind CS pipeline state, dispatch, and unbind resources to avoid hazards.
void CxbxD3D11DispatchCS(
	ID3D11ComputeShader* pShader,
	ID3D11Buffer* pCB,
	UINT numSRVs,
	ID3D11ShaderResourceView* const* ppSRVs,
	ID3D11UnorderedAccessView* pUAV,
	UINT groupsX, UINT groupsY, UINT groupsZ)
{
	g_pD3DDeviceContext->CSSetShader(pShader, nullptr, 0);
	g_pD3DDeviceContext->CSSetConstantBuffers(0, 1, &pCB);
	if (numSRVs > 0)
		g_pD3DDeviceContext->CSSetShaderResources(0, numSRVs, ppSRVs);
	g_pD3DDeviceContext->CSSetUnorderedAccessViews(0, 1, &pUAV, nullptr);
	g_pD3DDeviceContext->Dispatch(groupsX, groupsY, groupsZ);

	// Unbind to avoid resource hazards
	ID3D11ShaderResourceView* nullSRVs[2] = { nullptr, nullptr };
	ID3D11UnorderedAccessView* pNullUAV = nullptr;
	UINT unbindCount = numSRVs > 0 ? numSRVs : 1;
	g_pD3DDeviceContext->CSSetShaderResources(0, unbindCount, nullSRVs);
	g_pD3DDeviceContext->CSSetUnorderedAccessViews(0, 1, &pNullUAV, nullptr);
	g_pD3DDeviceContext->CSSetShader(nullptr, nullptr, 0);
}

// Map a DYNAMIC buffer with WRITE_DISCARD, memcpy data, and Unmap.
HRESULT CxbxD3D11UpdateDynamicBuffer(ID3D11Buffer* pBuffer, const void* pData, size_t dataSize)
{
	D3D11_MAPPED_SUBRESOURCE mapped = {};
	HRESULT hr = g_pD3DDeviceContext->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (FAILED(hr))
		return hr;
	memcpy(mapped.pData, pData, dataSize);
	g_pD3DDeviceContext->Unmap(pBuffer, 0);
	return S_OK;
}

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

	CxbxD3D11UpdateDynamicBuffer(g_pD3D11VSConstantBuffer, g_D3D11VSConstants, sizeof(g_D3D11VSConstants));
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

	CxbxD3D11UpdateDynamicBuffer(g_pD3D11PSConstantBuffer, g_D3D11PSConstants, sizeof(g_D3D11PSConstants));
	g_bD3D11PSConstantsDirty = false;
}

// ******************************************************************
// * Blit shader init
// ******************************************************************
void CxbxD3D11InitBlit()
{
	LOG_INIT;

	// Full-screen triangle vertex shader (no vertex buffer needed)
	static const std::string blitVS =
		"void main(uint id : SV_VertexID, out float4 pos : SV_Position, out float2 uv : TEXCOORD0) {\n"
		"    uv = float2((id << 1) & 2, id & 2);\n"
		"    pos = float4(uv * float2(2, -2) + float2(-1, 1), 0, 1);\n"
		"}\n";

	// Simple texture sampling pixel shader
	static const std::string blitPS =
		"Texture2D tex : register(t0);\n"
		"SamplerState samp : register(s0);\n"
		"float4 main(float4 pos : SV_Position, float2 uv : TEXCOORD0) : SV_Target {\n"
		"    return tex.Sample(samp, uv);\n"
		"}\n";

	// Compile vertex shader
	ID3DBlob* pVSBlob = nullptr;
	HRESULT hr = EmuCompileShader(blitVS, "vs_5_0", &pVSBlob, "CxbxBlitVS");
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to compile blit VS");
		return;
	}
	hr = g_pD3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_pD3D11BlitVS);
	pVSBlob->Release();
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to create blit VS");
		return;
	}

	// Compile pixel shader
	ID3DBlob* pPSBlob = nullptr;
	hr = EmuCompileShader(blitPS, "ps_5_0", &pPSBlob, "CxbxBlitPS");
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to compile blit PS");
		return;
	}
	hr = g_pD3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_pD3D11BlitPS);
	pPSBlob->Release();
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

	// ************************************************************
	// Point sprite geometry shader
	// ************************************************************
	static const std::string pointSpriteGS =
		"struct GS_INPUT {\n"
		"    float4 oPos : SV_Position;\n"
		"    float4 oD0  : COLOR0;\n"
		"    float4 oD1  : COLOR1;\n"
		"    float  oFog : FOG;\n"
		"    float  oPts : PSIZE;\n"
		"    float4 oB0  : TEXCOORD4;\n"
		"    float4 oB1  : TEXCOORD5;\n"
		"    float4 oT0  : TEXCOORD0;\n"
		"    float4 oT1  : TEXCOORD1;\n"
		"    float4 oT2  : TEXCOORD2;\n"
		"    float4 oT3  : TEXCOORD3;\n"
		"};\n"
		"struct GS_OUTPUT {\n"
		"    float4 oPos : SV_Position;\n"
		"    float4 oD0  : COLOR0;\n"
		"    float4 oD1  : COLOR1;\n"
		"    float  oFog : FOG;\n"
		"    float  oPts : PSIZE;\n"
		"    float4 oB0  : TEXCOORD4;\n"
		"    float4 oB1  : TEXCOORD5;\n"
		"    float4 oT0  : TEXCOORD0;\n"
		"    float4 oT1  : TEXCOORD1;\n"
		"    float4 oT2  : TEXCOORD2;\n"
		"    float4 oT3  : TEXCOORD3;\n"
		"};\n"
		"cbuffer GSConstants : register(b0) { float4 gsViewportInv; };\n"
		"[maxvertexcount(4)]\n"
		"void main(point GS_INPUT input[1], inout TriangleStream<GS_OUTPUT> stream) {\n"
		"    GS_OUTPUT o;\n"
		"    o.oD0  = input[0].oD0;\n"
		"    o.oD1  = input[0].oD1;\n"
		"    o.oFog = input[0].oFog;\n"
		"    o.oPts = input[0].oPts;\n"
		"    o.oB0  = input[0].oB0;\n"
		"    o.oB1  = input[0].oB1;\n"
		"    o.oT1  = input[0].oT1;\n"
		"    o.oT2  = input[0].oT2;\n"
		"    o.oT3  = input[0].oT3;\n"
		"    float ptSize = max(input[0].oPts, 1.0);\n"
		"    float4 pos = input[0].oPos;\n"
		"    float halfW = ptSize * gsViewportInv.x * pos.w;\n"
		"    float halfH = ptSize * gsViewportInv.y * pos.w;\n"
		"    o.oPos = pos + float4(-halfW, -halfH, 0, 0);\n"
		"    o.oT0 = float4(0, 0, 0, 1);\n"
		"    stream.Append(o);\n"
		"    o.oPos = pos + float4(+halfW, -halfH, 0, 0);\n"
		"    o.oT0 = float4(1, 0, 0, 1);\n"
		"    stream.Append(o);\n"
		"    o.oPos = pos + float4(-halfW, +halfH, 0, 0);\n"
		"    o.oT0 = float4(0, 1, 0, 1);\n"
		"    stream.Append(o);\n"
		"    o.oPos = pos + float4(+halfW, +halfH, 0, 0);\n"
		"    o.oT0 = float4(1, 1, 0, 1);\n"
		"    stream.Append(o);\n"
		"    stream.RestartStrip();\n"
		"}\n";

	ID3DBlob* pGSBlob = nullptr;
	hr = EmuCompileShader(pointSpriteGS, "gs_5_0", &pGSBlob, "CxbxPointSpriteGS");
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to compile point sprite GS");
	} else {
		hr = g_pD3DDevice->CreateGeometryShader(pGSBlob->GetBufferPointer(), pGSBlob->GetBufferSize(), nullptr, &g_pD3D11PointSpriteGS);
		pGSBlob->Release();
		if (FAILED(hr)) {
			EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to create point sprite GS");
		}
	}

	// ************************************************************
	// Thick line geometry shader
	// ************************************************************
	static const std::string thickLineGS =
		"struct GS_INPUT {\n"
		"    float4 oPos : SV_Position;\n"
		"    float4 oD0  : COLOR0;\n"
		"    float4 oD1  : COLOR1;\n"
		"    float  oFog : FOG;\n"
		"    float  oPts : PSIZE;\n"
		"    float4 oB0  : TEXCOORD4;\n"
		"    float4 oB1  : TEXCOORD5;\n"
		"    float4 oT0  : TEXCOORD0;\n"
		"    float4 oT1  : TEXCOORD1;\n"
		"    float4 oT2  : TEXCOORD2;\n"
		"    float4 oT3  : TEXCOORD3;\n"
		"};\n"
		"struct GS_OUTPUT {\n"
		"    float4 oPos : SV_Position;\n"
		"    float4 oD0  : COLOR0;\n"
		"    float4 oD1  : COLOR1;\n"
		"    float  oFog : FOG;\n"
		"    float  oPts : PSIZE;\n"
		"    float4 oB0  : TEXCOORD4;\n"
		"    float4 oB1  : TEXCOORD5;\n"
		"    float4 oT0  : TEXCOORD0;\n"
		"    float4 oT1  : TEXCOORD1;\n"
		"    float4 oT2  : TEXCOORD2;\n"
		"    float4 oT3  : TEXCOORD3;\n"
		"};\n"
		"cbuffer GSConstants : register(b0) { float4 gsParams; };\n" // xy = invViewport, z = lineWidth
		"[maxvertexcount(4)]\n"
		"void main(line GS_INPUT input[2], inout TriangleStream<GS_OUTPUT> stream) {\n"
		"    float4 p0 = input[0].oPos;\n"
		"    float4 p1 = input[1].oPos;\n"
		"    // Convert to NDC\n"
		"    float2 ndc0 = p0.xy / p0.w;\n"
		"    float2 ndc1 = p1.xy / p1.w;\n"
		"    // Line direction in NDC, then to screen pixels\n"
		"    float2 screenDir = (ndc1 - ndc0) / (gsParams.xy * 2.0);\n"
		"    float len = length(screenDir);\n"
		"    float2 dir = (len > 0.0001) ? screenDir / len : float2(1, 0);\n"
		"    // Perpendicular in screen space, then back to clip space\n"
		"    float2 perp = float2(-dir.y, dir.x);\n"
		"    float halfWidth = gsParams.z * 0.5;\n"
		"    float2 offset0 = perp * halfWidth * gsParams.xy * 2.0 * p0.w;\n"
		"    float2 offset1 = perp * halfWidth * gsParams.xy * 2.0 * p1.w;\n"
		"    GS_OUTPUT o;\n"
		"    // Vertex 0 — start, left side\n"
		"    o.oD0  = input[0].oD0; o.oD1  = input[0].oD1;\n"
		"    o.oFog = input[0].oFog; o.oPts = input[0].oPts;\n"
		"    o.oB0  = input[0].oB0; o.oB1  = input[0].oB1;\n"
		"    o.oT0  = input[0].oT0; o.oT1  = input[0].oT1;\n"
		"    o.oT2  = input[0].oT2; o.oT3  = input[0].oT3;\n"
		"    o.oPos = p0 + float4(-offset0, 0, 0);\n"
		"    stream.Append(o);\n"
		"    // Vertex 1 — start, right side\n"
		"    o.oPos = p0 + float4(+offset0, 0, 0);\n"
		"    stream.Append(o);\n"
		"    // Vertex 2 — end, left side (interpolate from input[1])\n"
		"    o.oD0  = input[1].oD0; o.oD1  = input[1].oD1;\n"
		"    o.oFog = input[1].oFog; o.oPts = input[1].oPts;\n"
		"    o.oB0  = input[1].oB0; o.oB1  = input[1].oB1;\n"
		"    o.oT0  = input[1].oT0; o.oT1  = input[1].oT1;\n"
		"    o.oT2  = input[1].oT2; o.oT3  = input[1].oT3;\n"
		"    o.oPos = p1 + float4(-offset1, 0, 0);\n"
		"    stream.Append(o);\n"
		"    // Vertex 3 — end, right side\n"
		"    o.oPos = p1 + float4(+offset1, 0, 0);\n"
		"    stream.Append(o);\n"
		"    stream.RestartStrip();\n"
		"}\n";

	pGSBlob = nullptr;
	hr = EmuCompileShader(thickLineGS, "gs_5_0", &pGSBlob, "CxbxThickLineGS");
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to compile thick line GS");
	} else {
		hr = g_pD3DDevice->CreateGeometryShader(pGSBlob->GetBufferPointer(), pGSBlob->GetBufferSize(), nullptr, &g_pD3D11ThickLineGS);
		pGSBlob->Release();
		if (FAILED(hr)) {
			EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to create thick line GS");
		}
	}

	// Create GS constant buffer (1 float4: inverse viewport dimensions + line width)
	hr = CxbxD3D11CreateConstantBuffer(16, true, &g_pD3D11GSConstantBuffer);
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to create GS constant buffer");
	}

	// ************************************************************
	// Compute shader for texture unswizzle
	// ************************************************************
	static const std::string unswizzleCS =
		"ByteAddressBuffer g_SrcBuffer : register(t0);\n"
		"RWTexture2D<uint> g_DstTexture : register(u0);\n"
		"cbuffer UnswizzleConstants : register(b0) {\n"
		"    uint maskX; uint maskY; uint texWidth; uint bpp;\n"
		"};\n"
		"uint MortonIndex(uint x, uint y) {\n"
		"    uint mx = maskX; uint my = maskY;\n"
		"    uint result = 0;\n"
		"    uint xBit = 1, yBit = 1, outBit = 1;\n"
		"    uint totalMask = mx | my;\n"
		"    [unroll(20)]\n"
		"    for (uint i = 0; i < 20; i++) {\n"
		"        if ((totalMask & outBit) == 0) break;\n"
		"        if (mx & outBit) { if (x & xBit) result |= outBit; xBit <<= 1; }\n"
		"        if (my & outBit) { if (y & yBit) result |= outBit; yBit <<= 1; }\n"
		"        outBit <<= 1;\n"
		"    }\n"
		"    return result;\n"
		"}\n"
		"[numthreads(8, 8, 1)]\n"
		"void main(uint3 dtid : SV_DispatchThreadID) {\n"
		"    uint x = dtid.x; uint y = dtid.y;\n"
		"    if (x >= texWidth) return;\n"
		"    uint mortonIdx = MortonIndex(x, y);\n"
		"    uint srcByteOffset = mortonIdx * bpp;\n"
		"    uint value;\n"
		"    if (bpp == 4) {\n"
		"        value = g_SrcBuffer.Load(srcByteOffset);\n"
		"    } else if (bpp == 2) {\n"
		"        uint dwordAddr = srcByteOffset & ~3u;\n"
		"        uint shift = (srcByteOffset & 2u) * 8u;\n"
		"        value = (g_SrcBuffer.Load(dwordAddr) >> shift) & 0xFFFF;\n"
		"    } else {\n"
		"        uint dwordAddr = srcByteOffset & ~3u;\n"
		"        uint shift = (srcByteOffset & 3u) * 8u;\n"
		"        value = (g_SrcBuffer.Load(dwordAddr) >> shift) & 0xFF;\n"
		"    }\n"
		"    g_DstTexture[uint2(x, y)] = value;\n"
		"}\n";

	ID3DBlob* pCSBlob = nullptr;
	hr = EmuCompileShader(unswizzleCS, "cs_5_0", &pCSBlob, "CxbxUnswizzleCS");
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to compile unswizzle CS");
	} else {
		hr = g_pD3DDevice->CreateComputeShader(pCSBlob->GetBufferPointer(), pCSBlob->GetBufferSize(), nullptr, &g_pD3D11UnswizzleCS);
		pCSBlob->Release();
		if (FAILED(hr)) {
			EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to create unswizzle CS");
		}
	}

	// Create unswizzle constant buffer (4 uints: maskX, maskY, width, bpp)
	hr = CxbxD3D11CreateConstantBuffer(16, true, &g_pD3D11UnswizzleCB);
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to create unswizzle CB");
	}

	// ---------------------------------------------------------------
	// Index buffer conversion compute shader (cs_5_0)
	// Converts quad-list and triangle-fan indices to triangle-list on GPU.
	// ---------------------------------------------------------------
	const char* indexConvertCS =
		"cbuffer IndexConvertCB : register(b0) {\n"
		"    uint g_VertexCount;\n"
		"    uint g_ConversionMode;\n"
		"    uint g_IsIndexed;\n"
		"    uint g_Pad;\n"
		"};\n"
		"ByteAddressBuffer InputIndices : register(t0);\n"
		"RWBuffer<uint> OutputIndices : register(u0);\n"
		"uint ReadIndex16(uint idx) {\n"
		"    if (g_IsIndexed == 0) return idx;\n"
		"    uint byteOff = idx * 2;\n"
		"    uint dwordOff = byteOff & ~3u;\n"
		"    uint raw = InputIndices.Load(dwordOff);\n"
		"    return (byteOff & 2u) ? (raw >> 16) : (raw & 0xFFFF);\n"
		"}\n"
		"uint Pack16(uint lo, uint hi) { return (lo & 0xFFFF) | (hi << 16); }\n"
		"[numthreads(64, 1, 1)]\n"
		"void main(uint3 DTid : SV_DispatchThreadID) {\n"
		"    uint tid = DTid.x;\n"
		"    if (g_ConversionMode <= 1) {\n"
		"        uint numQuads = g_VertexCount / 4;\n"
		"        if (tid >= numQuads) return;\n"
		"        uint s = tid * 4;\n"
		"        uint A = ReadIndex16(s), B = ReadIndex16(s+1), C = ReadIndex16(s+2), D = ReadIndex16(s+3);\n"
		"        uint d = tid * 3;\n"
		"        if (g_ConversionMode == 0) {\n"
		"            OutputIndices[d+0] = Pack16(A, B);\n"
		"            OutputIndices[d+1] = Pack16(D, B);\n"
		"            OutputIndices[d+2] = Pack16(C, D);\n"
		"        } else {\n"
		"            OutputIndices[d+0] = Pack16(A, D);\n"
		"            OutputIndices[d+1] = Pack16(B, B);\n"
		"            OutputIndices[d+2] = Pack16(D, C);\n"
		"        }\n"
		"    } else {\n"
		"        uint numTris = (g_VertexCount >= 3) ? (g_VertexCount - 2) : 0;\n"
		"        uint triIdx = tid * 2;\n"
		"        if (triIdx >= numTris) return;\n"
		"        uint hub = ReadIndex16(0);\n"
		"        uint i1 = ReadIndex16(triIdx + 1), i2 = ReadIndex16(triIdx + 2);\n"
		"        uint d = tid * 3;\n"
		"        if (triIdx + 1 < numTris) {\n"
		"            uint i3 = ReadIndex16(triIdx + 3);\n"
		"            OutputIndices[d+0] = Pack16(hub, i1);\n"
		"            OutputIndices[d+1] = Pack16(i2, hub);\n"
		"            OutputIndices[d+2] = Pack16(i2, i3);\n"
		"        } else {\n"
		"            OutputIndices[d+0] = Pack16(hub, i1);\n"
		"            OutputIndices[d+1] = Pack16(i2, 0);\n"
		"        }\n"
		"    }\n"
		"}\n";

	ID3DBlob* pIdxCSBlob = nullptr;
	hr = EmuCompileShader(std::string(indexConvertCS), "cs_5_0", &pIdxCSBlob, "CxbxIndexConvertCS");
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to compile index convert CS");
	} else {
		hr = g_pD3DDevice->CreateComputeShader(pIdxCSBlob->GetBufferPointer(), pIdxCSBlob->GetBufferSize(), nullptr, &g_pD3D11IndexConvertCS);
		pIdxCSBlob->Release();
		if (FAILED(hr)) {
			EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to create index convert CS");
		}
	}

	// Create index convert constant buffer (4 uints: vertexCount, mode, isIndexed, pad)
	hr = CxbxD3D11CreateConstantBuffer(16, false, &g_pD3D11IndexConvertCB);
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to create index convert CB");
	}

	// ---------------------------------------------------------------
	// Palette texture expansion compute shader (cs_5_0)
	// Combines unswizzle + P8 palette lookup in a single dispatch.
	// ---------------------------------------------------------------
	const char* paletteExpandCS =
		"ByteAddressBuffer g_SrcBuffer : register(t0);\n"
		"ByteAddressBuffer g_Palette : register(t1);\n"
		"RWTexture2D<uint> g_DstTexture : register(u0);\n"
		"cbuffer PaletteExpandCB : register(b0) {\n"
		"    uint maskX; uint maskY; uint texWidth; uint texHeight;\n"
		"};\n"
		"uint MortonIndex(uint x, uint y) {\n"
		"    uint mx = maskX; uint my = maskY;\n"
		"    uint result = 0;\n"
		"    uint xBit = 1, yBit = 1, outBit = 1;\n"
		"    uint totalMask = mx | my;\n"
		"    [unroll(20)]\n"
		"    for (uint i = 0; i < 20; i++) {\n"
		"        if ((totalMask & outBit) == 0) break;\n"
		"        if (mx & outBit) { if (x & xBit) result |= outBit; xBit <<= 1; }\n"
		"        if (my & outBit) { if (y & yBit) result |= outBit; yBit <<= 1; }\n"
		"        outBit <<= 1;\n"
		"    }\n"
		"    return result;\n"
		"}\n"
		"[numthreads(8, 8, 1)]\n"
		"void main(uint3 dtid : SV_DispatchThreadID) {\n"
		"    uint x = dtid.x; uint y = dtid.y;\n"
		"    if (x >= texWidth || y >= texHeight) return;\n"
		"    uint mortonIdx = MortonIndex(x, y);\n"
		"    // Read P8 index byte from swizzled source\n"
		"    uint dwordAddr = mortonIdx & ~3u;\n"
		"    uint shift = (mortonIdx & 3u) * 8u;\n"
		"    uint palIdx = (g_SrcBuffer.Load(dwordAddr) >> shift) & 0xFF;\n"
		"    // Look up ARGB palette entry (stored as D3DCOLOR = 0xAARRGGBB little-endian = [B,G,R,A] bytes)\n"
		"    uint argb = g_Palette.Load(palIdx * 4);\n"
		"    // Swap R and B for R8G8B8A8_UNORM output: BGRA bytes -> RGBA bytes\n"
		"    uint rgba = ((argb & 0x00FF0000u) >> 16) | (argb & 0x0000FF00u) | ((argb & 0x000000FFu) << 16) | (argb & 0xFF000000u);\n"
		"    g_DstTexture[uint2(x, y)] = rgba;\n"
		"}\n";

	ID3DBlob* pPalCSBlob = nullptr;
	hr = EmuCompileShader(std::string(paletteExpandCS), "cs_5_0", &pPalCSBlob, "CxbxPaletteExpandCS");
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to compile palette expand CS");
	} else {
		hr = g_pD3DDevice->CreateComputeShader(pPalCSBlob->GetBufferPointer(), pPalCSBlob->GetBufferSize(), nullptr, &g_pD3D11PaletteExpandCS);
		pPalCSBlob->Release();
		if (FAILED(hr)) {
			EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to create palette expand CS");
		}
	}

	// Create palette expand constant buffer (4 uints: maskX, maskY, width, pad)
	hr = CxbxD3D11CreateConstantBuffer(16, true, &g_pD3D11PaletteExpandCB);
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to create palette expand CB");
	}

	// Create palette data buffer (256 entries * 4 bytes = 1024 bytes)
	hr = CxbxD3D11CreateRawBuffer(1024, true, 0, &g_pD3D11PaletteBuf);
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to create palette buffer");
	} else {
		hr = CxbxD3D11CreateRawBufferSRV(g_pD3D11PaletteBuf, 1024, &g_pD3D11PaletteSRV);
		if (FAILED(hr)) {
			EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to create palette SRV");
		}
	}

	// ---------------------------------------------------------------
	// Texture format conversion compute shader (cs_5_0)
	// Combines unswizzle (optional) + format decode → RGBA output.
	// Replaces CPU ConvertD3DTextureToARGBBuffer for most formats.
	// ---------------------------------------------------------------
	static const std::string formatConvertCS =
		"ByteAddressBuffer g_SrcBuffer : register(t0);\n"
		"RWTexture2D<uint> g_DstTexture : register(u0);\n"
		"cbuffer FormatConvertCB : register(b0) {\n"
		"    uint maskX; uint maskY; uint texWidth; uint texHeight;\n"
		"    uint bpp; uint fmtType; uint swizzled; uint srcRowPitch;\n"
		"};\n"
		"\n"
		"uint MortonIndex(uint x, uint y) {\n"
		"    uint mx = maskX; uint my = maskY;\n"
		"    uint result = 0;\n"
		"    uint xBit = 1, yBit = 1, outBit = 1;\n"
		"    uint totalMask = mx | my;\n"
		"    [unroll(20)]\n"
		"    for (uint i = 0; i < 20; i++) {\n"
		"        if ((totalMask & outBit) == 0) break;\n"
		"        if (mx & outBit) { if (x & xBit) result |= outBit; xBit <<= 1; }\n"
		"        if (my & outBit) { if (y & yBit) result |= outBit; yBit <<= 1; }\n"
		"        outBit <<= 1;\n"
		"    }\n"
		"    return result;\n"
		"}\n"
		"\n"
		"uint LoadSrcByte(uint byteOffset) {\n"
		"    uint dw = g_SrcBuffer.Load(byteOffset & ~3u);\n"
		"    return (dw >> ((byteOffset & 3u) * 8u)) & 0xFF;\n"
		"}\n"
		"uint LoadSrc16(uint byteOffset) {\n"
		"    uint dw = g_SrcBuffer.Load(byteOffset & ~3u);\n"
		"    return (dw >> ((byteOffset & 2u) * 8u)) & 0xFFFF;\n"
		"}\n"
		"uint LoadSrc32(uint byteOffset) {\n"
		"    return g_SrcBuffer.Load(byteOffset);\n"
		"}\n"
		"\n"
		"// Sign-extend a 5-bit two's complement value to float [-1,+1]\n"
		"float SignExtend5(uint v) {\n"
		"    int s = int(v); if (s >= 16) s -= 32;\n"
		"    return float(s) / 15.0;\n"
		"}\n"
		"// Sign-extend an 8-bit two's complement value to float [-1,+1]\n"
		"float SignExtend8(uint v) {\n"
		"    int s = int(v); if (s >= 128) s -= 256;\n"
		"    return float(s) / 127.0;\n"
		"}\n"
		"// Unsigned 5-bit expand [0..31] -> 8-bit [0..255]\n"
		"uint Expand5to8(uint v) { return (v << 3) | (v >> 2); }\n"
		"// Unsigned 6-bit expand [0..63] -> 8-bit [0..255]\n"
		"uint Expand6to8(uint v) { return (v << 2) | (v >> 4); }\n"
		"// Unsigned 4-bit expand [0..15] -> 8-bit [0..255]\n"
		"uint Expand4to8(uint v) { return (v << 4) | v; }\n"
		"// Signed 5-bit bias to unsigned 8-bit (signed -> unsigned range)\n"
		"uint SignedExpand5to8(uint v) {\n"
		"    int s = int(v); if (s >= 16) s -= 32;\n"
		"    // Map [-16..+15] to [0..255]: add 16 then scale\n"
		"    return uint(clamp((s + 16) * 255 / 31, 0, 255));\n"
		"}\n"
		"\n"
		"// Pack R,G,B,A (0-255 each) into a uint for R8G8B8A8_UNORM UAV\n"
		"uint PackRGBA(uint r, uint g, uint b, uint a) {\n"
		"    return (r & 0xFF) | ((g & 0xFF) << 8) | ((b & 0xFF) << 16) | ((a & 0xFF) << 24);\n"
		"}\n"
		"\n"
		"#define FMT_L6V5U5   1\n"
		"#define FMT_R6G5B5   2\n"
		"#define FMT_V8U8     3\n"
		"#define FMT_G8B8     4\n"
		"#define FMT_R5G5B5A1 5\n"
		"#define FMT_R4G4B4A4 6\n"
		"#define FMT_A8       7\n"
		"#define FMT_YUY2     8\n"
		"#define FMT_UYVY     9\n"
		"\n"
		"uint ConvertTexel16(uint raw, uint fmt) {\n"
		"    if (fmt == FMT_L6V5U5) {\n"
		"        // L6V5U5: bits [4:0]=U(signed), [9:5]=V(signed), [15:10]=L(unsigned)\n"
		"        // Bias U,V from signed to unsigned range for UNORM output\n"
		"        // Colorsign in the pixel shader converts back to signed at sampling time\n"
		"        uint u8 = SignedExpand5to8(raw & 0x1Fu);\n"
		"        uint v8 = SignedExpand5to8((raw >> 5u) & 0x1Fu);\n"
		"        uint l8 = Expand6to8((raw >> 10u) & 0x3Fu);\n"
		"        return PackRGBA(u8, v8, l8, 255);\n"
		"    }\n"
		"    if (fmt == FMT_R6G5B5) {\n"
		"        // R6G5B5: bits [4:0]=B(5), [9:5]=G(5), [15:10]=R(6)\n"
		"        uint b8 = Expand5to8(raw & 0x1Fu);\n"
		"        uint g8 = Expand5to8((raw >> 5u) & 0x1Fu);\n"
		"        uint r8 = Expand6to8((raw >> 10u) & 0x3Fu);\n"
		"        return PackRGBA(r8, g8, b8, 255);\n"
		"    }\n"
		"    if (fmt == FMT_V8U8) {\n"
		"        // V8U8 (G8B8 alias): byte[0]=U(signed), byte[1]=V(signed)\n"
		"        // Bias from signed to unsigned for UNORM; colorsign converts back\n"
		"        uint u8 = ((raw & 0xFFu) + 128u) & 0xFFu;\n"
		"        uint v8 = (((raw >> 8u) & 0xFFu) + 128u) & 0xFFu;\n"
		"        return PackRGBA(u8, v8, 0, 255);\n"
		"    }\n"
		"    if (fmt == FMT_G8B8) {\n"
		"        // G8B8 (unsigned): byte[0]=B, byte[1]=G\n"
		"        uint b8 = raw & 0xFFu;\n"
		"        uint g8 = (raw >> 8u) & 0xFFu;\n"
		"        return PackRGBA(b8, g8, b8, g8);\n"
		"    }\n"
		"    if (fmt == FMT_R5G5B5A1) {\n"
		"        // R5G5B5A1: bit[0]=A(1), [5:1]=B(5), [10:6]=G(5), [15:11]=R(5)\n"
		"        uint a8 = (raw & 1u) ? 255 : 0;\n"
		"        uint b8 = Expand5to8((raw >> 1u) & 0x1Fu);\n"
		"        uint g8 = Expand5to8((raw >> 6u) & 0x1Fu);\n"
		"        uint r8 = Expand5to8((raw >> 11u) & 0x1Fu);\n"
		"        return PackRGBA(r8, g8, b8, a8);\n"
		"    }\n"
		"    if (fmt == FMT_R4G4B4A4) {\n"
		"        // R4G4B4A4: [3:0]=A(4), [7:4]=B(4), [11:8]=G(4), [15:12]=R(4)\n"
		"        uint a8 = Expand4to8(raw & 0xFu);\n"
		"        uint b8 = Expand4to8((raw >> 4u) & 0xFu);\n"
		"        uint g8 = Expand4to8((raw >> 8u) & 0xFu);\n"
		"        uint r8 = Expand4to8((raw >> 12u) & 0xFu);\n"
		"        return PackRGBA(r8, g8, b8, a8);\n"
		"    }\n"
		"    return PackRGBA(255, 0, 255, 255); // Magenta = unhandled\n"
		"}\n"
		"\n"
		"uint ConvertTexel8(uint raw, uint fmt) {\n"
		"    if (fmt == FMT_A8) {\n"
		"        // A8: Xbox RGB=1, alpha=raw\n"
		"        return PackRGBA(255, 255, 255, raw & 0xFFu);\n"
		"    }\n"
		"    return PackRGBA(255, 0, 255, 255); // Magenta\n"
		"}\n"
		"\n"
		"[numthreads(8, 8, 1)]\n"
		"void main(uint3 dtid : SV_DispatchThreadID) {\n"
		"    uint x = dtid.x; uint y = dtid.y;\n"
		"    if (x >= texWidth || y >= texHeight) return;\n"
		"    uint srcByteOffset;\n"
		"    if (swizzled != 0) {\n"
		"        srcByteOffset = MortonIndex(x, y) * bpp;\n"
		"    } else {\n"
		"        srcByteOffset = y * srcRowPitch + x * bpp;\n"
		"    }\n"
		"    uint rgba;\n"
		"    if (bpp == 2) {\n"
		"        uint raw16 = LoadSrc16(srcByteOffset);\n"
		"        rgba = ConvertTexel16(raw16, fmtType);\n"
		"    } else if (bpp == 1) {\n"
		"        uint raw8 = LoadSrcByte(srcByteOffset);\n"
		"        rgba = ConvertTexel8(raw8, fmtType);\n"
		"    } else if (bpp == 4) {\n"
		"        // 32-bit formats: channel reorder only\n"
		"        rgba = LoadSrc32(srcByteOffset);\n"
		"    } else {\n"
		"        rgba = PackRGBA(255, 0, 255, 255);\n"
		"    }\n"
		"    g_DstTexture[uint2(x, y)] = rgba;\n"
		"}\n";

	ID3DBlob* pFmtCSBlob = nullptr;
	hr = EmuCompileShader(formatConvertCS, "cs_5_0", &pFmtCSBlob, "CxbxFormatConvertCS");
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to compile format convert CS");
	} else {
		hr = g_pD3DDevice->CreateComputeShader(pFmtCSBlob->GetBufferPointer(), pFmtCSBlob->GetBufferSize(), nullptr, &g_pD3D11FormatConvertCS);
		pFmtCSBlob->Release();
		if (FAILED(hr)) {
			EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to create format convert CS");
		}
	}

	// Create format convert constant buffer (8 uints)
	hr = CxbxD3D11CreateConstantBuffer(32, true, &g_pD3D11FormatConvertCB);
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to create format convert CB");
	}

	// ---------------------------------------------------------------
	// Vertex format conversion compute shader (cs_5_0)
	// Converts Xbox vertex formats to D3D11-compatible layouts on GPU.
	// ---------------------------------------------------------------
	const char* vertexConvertCS =
		"ByteAddressBuffer g_SrcBuffer : register(t0);\n"
		"RWBuffer<uint> g_DstBuffer : register(u0);\n"
		"cbuffer VertexConvertCB : register(b0) {\n"
		"    uint g_VertexCount; uint g_SrcStride; uint g_DstStride; uint g_NumElements;\n"
		"    uint4 g_Elements[16];\n" // x=srcOffset, y=dstOffset, z=convType, w=copyDwords
		"};\n"
		"#define CONV_COPY        0\n"
		"#define CONV_NORMSHORT3  1\n"
		"#define CONV_NORMPACKED3 2\n"
		"#define CONV_SHORT3      3\n"
		"#define CONV_PBYTE3      4\n"
		"#define CONV_FLOAT2H     5\n"
		"#define CONV_D3DCOLOR    6\n"
		"#define CONV_NONE        7\n"
		"uint ReadU32(uint byteOff) {\n"
		"    uint a = byteOff & ~3u; uint s = (byteOff & 3u) * 8u;\n"
		"    if (s == 0) return g_SrcBuffer.Load(a);\n"
		"    return (g_SrcBuffer.Load(a) >> s) | (g_SrcBuffer.Load(a + 4) << (32 - s));\n"
		"}\n"
		"uint ReadU16(uint byteOff) {\n"
		"    uint a = byteOff & ~3u; uint s = (byteOff & 3u) * 8u;\n"
		"    uint d = g_SrcBuffer.Load(a);\n"
		"    if (s <= 16) return (d >> s) & 0xFFFF;\n"
		"    return ((d >> s) | (g_SrcBuffer.Load(a + 4) << (32 - s))) & 0xFFFF;\n"
		"}\n"
		"void WriteU32(uint dstByteOff, uint value) { g_DstBuffer[dstByteOff >> 2] = value; }\n"
		"float PackedIntToFloat(int v, float pm, float nm) {\n"
		"    return (v >= 0) ? (float(v) / pm) : (float(v) / nm);\n"
		"}\n"
		"[numthreads(64, 1, 1)]\n"
		"void main(uint3 dtid : SV_DispatchThreadID) {\n"
		"    uint vi = dtid.x;\n"
		"    if (vi >= g_VertexCount) return;\n"
		"    uint sb = vi * g_SrcStride;\n"
		"    uint db = vi * g_DstStride;\n"
		"    for (uint e = 0; e < g_NumElements; e++) {\n"
		"        uint so = sb + g_Elements[e].x;\n"
		"        uint do_ = db + g_Elements[e].y;\n"
		"        uint ct = g_Elements[e].z;\n"
		"        uint cw = g_Elements[e].w;\n"
		"        if (ct == CONV_COPY) {\n"
		"            for (uint d = 0; d < cw; d++) WriteU32(do_ + d * 4, ReadU32(so + d * 4));\n"
		"        } else if (ct == CONV_NORMSHORT3) {\n"
		"            WriteU32(do_, ReadU32(so));\n"
		"            WriteU32(do_ + 4, ReadU16(so + 4) | 0x7FFF0000u);\n"
		"        } else if (ct == CONV_NORMPACKED3) {\n"
		"            int p = asint(ReadU32(so));\n"
		"            int x = (p << 21) >> 21; int y = (p << 10) >> 21; int z = p >> 22;\n"
		"            WriteU32(do_, asuint(PackedIntToFloat(x, 1023.0, 1024.0)));\n"
		"            WriteU32(do_ + 4, asuint(PackedIntToFloat(y, 1023.0, 1024.0)));\n"
		"            WriteU32(do_ + 8, asuint(PackedIntToFloat(z, 511.0, 512.0)));\n"
		"        } else if (ct == CONV_SHORT3) {\n"
		"            WriteU32(do_, ReadU32(so));\n"
		"            WriteU32(do_ + 4, ReadU16(so + 4) | (1u << 16));\n"
		"        } else if (ct == CONV_PBYTE3) {\n"
		"            WriteU32(do_, (ReadU32(so) & 0x00FFFFFFu) | 0xFF000000u);\n"
		"        } else if (ct == CONV_FLOAT2H) {\n"
		"            WriteU32(do_, ReadU32(so));\n"
		"            WriteU32(do_ + 4, ReadU32(so + 4));\n"
		"            WriteU32(do_ + 8, 0);\n"
		"            WriteU32(do_ + 12, ReadU32(so + 8));\n"
		"        } else if (ct == CONV_D3DCOLOR) {\n"
		"            uint c = ReadU32(so);\n"
		"            WriteU32(do_, ((c >> 16) & 0xFFu) | (c & 0xFF00FF00u) | ((c & 0xFFu) << 16));\n"
		"        }\n" // CONV_NONE: skip
		"    }\n"
		"}\n";

	ID3DBlob* pVtxCSBlob = nullptr;
	hr = EmuCompileShader(std::string(vertexConvertCS), "cs_5_0", &pVtxCSBlob, "CxbxVertexConvertCS");
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to compile vertex convert CS");
	} else {
		hr = g_pD3DDevice->CreateComputeShader(pVtxCSBlob->GetBufferPointer(), pVtxCSBlob->GetBufferSize(), nullptr, &g_pD3D11VertexConvertCS);
		pVtxCSBlob->Release();
		if (FAILED(hr)) {
			EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to create vertex convert CS");
		}
	}

	// Create vertex convert constant buffer (header 16 bytes + 16 elements * 16 bytes = 272 bytes)
	hr = CxbxD3D11CreateConstantBuffer(272, true, &g_pD3D11VertexConvertCB);
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11InitBlit: Failed to create vertex convert CB");
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
	// Save blend state so the game's blend settings don't leak into the blit
	ComPtr<ID3D11BlendState> pOldBlendState;
	FLOAT oldBlendFactor[4];
	UINT oldSampleMask;
	g_pD3DDeviceContext->OMGetBlendState(&pOldBlendState, oldBlendFactor, &oldSampleMask);

	// Set blit pipeline state
	D3D11_VIEWPORT vp = { (FLOAT)dstX, (FLOAT)dstY, (FLOAT)dstW, (FLOAT)dstH, 0.0f, 1.0f };
	g_pD3DDeviceContext->RSSetViewports(1, &vp);
	g_pD3DDeviceContext->OMSetRenderTargets(1, &pRTV, nullptr);
	g_pD3DDeviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF); // Default blend: no blending, all channels written
	g_pD3DDeviceContext->VSSetShader(g_pD3D11BlitVS, nullptr, 0);
	g_pD3DDeviceContext->PSSetShader(g_pD3D11BlitPS, nullptr, 0);
	g_pD3DDeviceContext->GSSetShader(nullptr, nullptr, 0); // Ensure point sprite GS doesn't interfere
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
	g_pD3DDeviceContext->OMSetBlendState(pOldBlendState.Get(), oldBlendFactor, oldSampleMask);
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
// * Release all backend resources (called from device release lambda)
// ******************************************************************
void CxbxD3D11ReleaseBackendResources()
{
	g_pD3DBlendState.Reset();
	g_pD3DDepthStencilState.Reset();
	g_pD3DRasterizerState.Reset();
	if (g_pD3D11VSConstantBuffer) { g_pD3D11VSConstantBuffer->Release(); g_pD3D11VSConstantBuffer = nullptr; }
	if (g_pD3D11PSConstantBuffer) { g_pD3D11PSConstantBuffer->Release(); g_pD3D11PSConstantBuffer = nullptr; }
	if (g_pD3D11BlitVS) { g_pD3D11BlitVS->Release(); g_pD3D11BlitVS = nullptr; }
	if (g_pD3D11BlitPS) { g_pD3D11BlitPS->Release(); g_pD3D11BlitPS = nullptr; }
	if (g_pD3D11BlitSamplerLinear) { g_pD3D11BlitSamplerLinear->Release(); g_pD3D11BlitSamplerLinear = nullptr; }
	if (g_pD3D11BlitSamplerPoint) { g_pD3D11BlitSamplerPoint->Release(); g_pD3D11BlitSamplerPoint = nullptr; }
	if (g_pD3D11PointSpriteGS) { g_pD3D11PointSpriteGS->Release(); g_pD3D11PointSpriteGS = nullptr; }
	if (g_pD3D11ThickLineGS) { g_pD3D11ThickLineGS->Release(); g_pD3D11ThickLineGS = nullptr; }
	if (g_pD3D11GSConstantBuffer) { g_pD3D11GSConstantBuffer->Release(); g_pD3D11GSConstantBuffer = nullptr; }
	if (g_pD3D11UnswizzleCS) { g_pD3D11UnswizzleCS->Release(); g_pD3D11UnswizzleCS = nullptr; }
	if (g_pD3D11UnswizzleCB) { g_pD3D11UnswizzleCB->Release(); g_pD3D11UnswizzleCB = nullptr; }
	if (g_pD3D11UnswizzleSRV) { g_pD3D11UnswizzleSRV->Release(); g_pD3D11UnswizzleSRV = nullptr; }
	if (g_pD3D11UnswizzleStagingBuf) { g_pD3D11UnswizzleStagingBuf->Release(); g_pD3D11UnswizzleStagingBuf = nullptr; }
	g_UnswizzleStagingBufSize = 0;
	if (g_pD3D11IndexConvertCS) { g_pD3D11IndexConvertCS->Release(); g_pD3D11IndexConvertCS = nullptr; }
	if (g_pD3D11IndexConvertCB) { g_pD3D11IndexConvertCB->Release(); g_pD3D11IndexConvertCB = nullptr; }
	if (g_pD3D11IndexConvertInputSRV) { g_pD3D11IndexConvertInputSRV->Release(); g_pD3D11IndexConvertInputSRV = nullptr; }
	if (g_pD3D11IndexConvertInputBuf) { g_pD3D11IndexConvertInputBuf->Release(); g_pD3D11IndexConvertInputBuf = nullptr; }
	g_IndexConvertInputBufSize = 0;
	if (g_pD3D11IndexConvertOutputUAV) { g_pD3D11IndexConvertOutputUAV->Release(); g_pD3D11IndexConvertOutputUAV = nullptr; }
	if (g_pD3D11IndexConvertOutputBuf) { g_pD3D11IndexConvertOutputBuf->Release(); g_pD3D11IndexConvertOutputBuf = nullptr; }
	g_IndexConvertOutputBufSize = 0;
	if (g_pD3D11FormatConvertCS) { g_pD3D11FormatConvertCS->Release(); g_pD3D11FormatConvertCS = nullptr; }
	if (g_pD3D11FormatConvertCB) { g_pD3D11FormatConvertCB->Release(); g_pD3D11FormatConvertCB = nullptr; }
	ClearRTVCache();
}

#endif // CXBX_USE_D3D11
