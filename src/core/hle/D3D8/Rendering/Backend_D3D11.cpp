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
#include "core\hle\D3D8\Rendering\Shader.h" // EmuCompileShader
#include "core\hle\D3D8\XbConvert.h" // EmuXB2PC_D3D11PrimitiveTopology
#include "core\hle\D3D8\XbVertexShader.h" // CxbxVertexDeclaration, CxbxGetActiveVertexShaderBytecode
#include "TextureStates.h" // XboxTextureStateConverter

#include <cstring> // memcpy
#include <unordered_map>
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
static ComPtr<ID3D11RasterizerState>   g_pD3DRasterizerState;
static ComPtr<ID3D11DepthStencilState> g_pD3DDepthStencilState;
static ComPtr<ID3D11BlendState>        g_pD3DBlendState;

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
// * Point sprite geometry shader resources
// ******************************************************************
static ID3D11GeometryShader *g_pD3D11PointSpriteGS = nullptr;
static ID3D11Buffer         *g_pD3D11GSConstantBuffer = nullptr;
static bool                  g_bPointSpriteEnabled = false;

// ******************************************************************
// * Thick line geometry shader resources
// ******************************************************************
static ID3D11GeometryShader *g_pD3D11ThickLineGS = nullptr;
static float                 g_fLineWidth = 1.0f;

// ******************************************************************
// * Compute shader unswizzle resources
// ******************************************************************
static ID3D11ComputeShader  *g_pD3D11UnswizzleCS = nullptr;
static ID3D11Buffer         *g_pD3D11UnswizzleCB = nullptr; // constant buffer: maskX, maskY, width, bpp
static ID3D11Buffer         *g_pD3D11UnswizzleStagingBuf = nullptr; // reusable ByteAddressBuffer for upload
static UINT                  g_UnswizzleStagingBufSize = 0;
static ID3D11ShaderResourceView *g_pD3D11UnswizzleSRV = nullptr; // SRV for staging buffer

// ******************************************************************
// * Compute shader index buffer conversion resources
// ******************************************************************
static ID3D11ComputeShader       *g_pD3D11IndexConvertCS = nullptr;
static ID3D11Buffer              *g_pD3D11IndexConvertCB = nullptr; // constant buffer: vertexCount, mode, isIndexed, pad
static ID3D11Buffer              *g_pD3D11IndexConvertInputBuf = nullptr; // ByteAddressBuffer for source indices
static UINT                       g_IndexConvertInputBufSize = 0;
static ID3D11ShaderResourceView  *g_pD3D11IndexConvertInputSRV = nullptr;
static ID3D11Buffer              *g_pD3D11IndexConvertOutputBuf = nullptr; // output buffer (INDEX_BUFFER + UAV)
static UINT                       g_IndexConvertOutputBufSize = 0;
static ID3D11UnorderedAccessView *g_pD3D11IndexConvertOutputUAV = nullptr;

// ******************************************************************
// * Compute shader palette texture expansion resources
// ******************************************************************
static ID3D11ComputeShader       *g_pD3D11PaletteExpandCS = nullptr;
static ID3D11Buffer              *g_pD3D11PaletteExpandCB = nullptr; // constant buffer: maskX, maskY, width, pad
static ID3D11Buffer              *g_pD3D11PaletteBuf = nullptr; // 256-entry palette upload buffer
static ID3D11ShaderResourceView  *g_pD3D11PaletteSRV = nullptr;

// ******************************************************************
// * Compute shader vertex format conversion resources
// ******************************************************************
static ID3D11ComputeShader       *g_pD3D11VertexConvertCS = nullptr;
static ID3D11Buffer              *g_pD3D11VertexConvertCB = nullptr; // constant buffer: header + 16 element descriptors
static ID3D11Buffer              *g_pD3D11VertexConvertSrcBuf = nullptr; // staging ByteAddressBuffer for source vertices
static UINT                       g_VertexConvertSrcBufSize = 0;
static ID3D11ShaderResourceView  *g_pD3D11VertexConvertSrcSRV = nullptr;

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
static HRESULT CxbxD3D11CreateRawBuffer(UINT byteWidth, bool bDynamic, UINT extraBindFlags, ID3D11Buffer** ppBuffer)
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
static HRESULT CxbxD3D11CreateRawBufferSRV(ID3D11Buffer* pBuffer, UINT byteWidth, ID3D11ShaderResourceView** ppSRV)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
	desc.BufferEx.NumElements = byteWidth / 4;
	return g_pD3DDevice->CreateShaderResourceView(pBuffer, &desc, ppSRV);
}

// Create a R32_UINT typed UAV for a buffer.
static HRESULT CxbxD3D11CreateTypedBufferUAV(ID3D11Buffer* pBuffer, UINT byteWidth, ID3D11UnorderedAccessView** ppUAV)
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
static void CxbxD3D11EnsureRawStagingBuffer(
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
static void CxbxD3D11DispatchCS(
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
static HRESULT CxbxD3D11UpdateDynamicBuffer(ID3D11Buffer* pBuffer, const void* pData, size_t dataSize)
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
	HRESULT hr = EmuCompileShader(std::string(blitVS), "vs_4_0", &pVSBlob, "CxbxBlitVS");
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
	hr = EmuCompileShader(std::string(blitPS), "ps_4_0", &pPSBlob, "CxbxBlitPS");
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
	static const char* pointSpriteGS =
		"struct GS_INPUT {\n"
		"    float4 oPos : POSITION;\n"
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
	hr = EmuCompileShader(std::string(pointSpriteGS), "gs_4_0", &pGSBlob, "CxbxPointSpriteGS");
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
	static const char* thickLineGS =
		"struct GS_INPUT {\n"
		"    float4 oPos : POSITION;\n"
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
	hr = EmuCompileShader(std::string(thickLineGS), "gs_4_0", &pGSBlob, "CxbxThickLineGS");
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
	static const char* unswizzleCS =
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
	hr = EmuCompileShader(std::string(unswizzleCS), "cs_5_0", &pCSBlob, "CxbxUnswizzleCS");
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

	// Set blit pipeline state
	D3D11_VIEWPORT vp = { (FLOAT)dstX, (FLOAT)dstY, (FLOAT)dstW, (FLOAT)dstH, 0.0f, 1.0f };
	g_pD3DDeviceContext->RSSetViewports(1, &vp);
	g_pD3DDeviceContext->OMSetRenderTargets(1, &pRTV, nullptr);
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
static std::unordered_map<IDirect3DSurface*, ID3D11RenderTargetView*> g_RTVCache;

static void ClearRTVCache()
{
	for (auto &pair : g_RTVCache) {
		if (pair.second) pair.second->Release();
	}
	g_RTVCache.clear();
}

// ******************************************************************
// * GPU unswizzle via compute shader
// ******************************************************************
static void CxbxEnsureUnswizzleStagingBuffer(UINT requiredSize)
{
	CxbxD3D11EnsureRawStagingBuffer(requiredSize,
		&g_pD3D11UnswizzleStagingBuf, &g_UnswizzleStagingBufSize,
		&g_pD3D11UnswizzleSRV, "CxbxEnsureUnswizzleStagingBuffer");
}

bool CxbxD3D11UnswizzleTexture(
	ID3D11Texture2D* pTexture,
	const void* pSwizzledSrc,
	UINT width,
	UINT height,
	UINT bpp,
	DXGI_FORMAT format)
{
	if (!g_pD3D11UnswizzleCS || !g_pD3D11UnswizzleCB)
		return false;

	// Only 2D textures with bpp 1, 2, or 4
	if (bpp != 1 && bpp != 2 && bpp != 4)
		return false;

	UINT dataSize = width * height * bpp;
	// Round up to DWORD alignment for ByteAddressBuffer
	UINT bufferSize = (dataSize + 3) & ~3u;

	// Ensure staging buffer is large enough
	CxbxEnsureUnswizzleStagingBuffer(bufferSize);
	if (!g_pD3D11UnswizzleStagingBuf || !g_pD3D11UnswizzleSRV)
		return false;

	// Upload swizzled source data to the staging buffer
	HRESULT hr = CxbxD3D11UpdateDynamicBuffer(g_pD3D11UnswizzleStagingBuf, pSwizzledSrc, dataSize);
	if (FAILED(hr))
		return false;

	// Compute Morton masks (same algorithm as EmuUnswizzleBox)
	UINT maskX = 0, maskY = 0;
	for (UINT i = 1, j = 1; (i < width) || (i < height); i <<= 1) {
		if (i < width)  { maskX |= j; j <<= 1; }
		if (i < height) { maskY |= j; j <<= 1; }
	}

	// Update constant buffer
	UINT cbData[4] = { maskX, maskY, width, bpp };
	hr = CxbxD3D11UpdateDynamicBuffer(g_pD3D11UnswizzleCB, cbData, sizeof(cbData));
	if (FAILED(hr))
		return false;

	// Create a temporary UAV for the destination texture
	// Map the format to a uint-typed format for RWTexture2D<uint>
	DXGI_FORMAT uavFormat;
	switch (bpp) {
	case 4:  uavFormat = DXGI_FORMAT_R32_UINT; break;
	case 2:  uavFormat = DXGI_FORMAT_R16_UINT; break;
	case 1:  uavFormat = DXGI_FORMAT_R8_UINT;  break;
	default: return false;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = uavFormat;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;
	ID3D11UnorderedAccessView* pUAV = nullptr;
	hr = g_pD3DDevice->CreateUnorderedAccessView(pTexture, &uavDesc, &pUAV);
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11UnswizzleTexture: Failed to create UAV (format=%u)", uavFormat);
		return false;
	}

	// Dispatch 8x8 thread groups covering the texture dimensions
	UINT groupsX = (width + 7) / 8;
	UINT groupsY = (height + 7) / 8;
	CxbxD3D11DispatchCS(g_pD3D11UnswizzleCS, g_pD3D11UnswizzleCB,
		1, &g_pD3D11UnswizzleSRV, pUAV, groupsX, groupsY, 1);

	pUAV->Release();
	return true;
}

// ******************************************************************
// * GPU palette texture expansion
// ******************************************************************

bool CxbxD3D11ExpandPaletteTexture(
	ID3D11Texture2D* pTexture,
	const void* pSwizzledP8Src,
	UINT width,
	UINT height,
	const void* pPaletteData)
{
	if (!g_pD3D11PaletteExpandCS || !g_pD3D11PaletteExpandCB || !g_pD3D11PaletteBuf || !g_pD3D11PaletteSRV)
		return false;

	UINT dataSize = width * height; // 1 byte per pixel
	UINT bufferSize = (dataSize + 3) & ~3u;

	// Reuse the unswizzle staging buffer for P8 source data
	CxbxEnsureUnswizzleStagingBuffer(bufferSize);
	if (!g_pD3D11UnswizzleStagingBuf || !g_pD3D11UnswizzleSRV)
		return false;

	// Upload swizzled P8 source data
	HRESULT hr = CxbxD3D11UpdateDynamicBuffer(g_pD3D11UnswizzleStagingBuf, pSwizzledP8Src, dataSize);
	if (FAILED(hr))
		return false;

	// Upload palette data (256 entries * 4 bytes = 1024 bytes)
	hr = CxbxD3D11UpdateDynamicBuffer(g_pD3D11PaletteBuf, pPaletteData, 1024);
	if (FAILED(hr))
		return false;

	// Compute Morton masks
	UINT maskX = 0, maskY = 0;
	for (UINT i = 1, j = 1; (i < width) || (i < height); i <<= 1) {
		if (i < width)  { maskX |= j; j <<= 1; }
		if (i < height) { maskY |= j; j <<= 1; }
	}

	// Update constant buffer
	UINT cbData[4] = { maskX, maskY, width, height };
	hr = CxbxD3D11UpdateDynamicBuffer(g_pD3D11PaletteExpandCB, cbData, sizeof(cbData));
	if (FAILED(hr))
		return false;

	// Create UAV for destination texture (R8G8B8A8_UNORM → R32_UINT UAV)
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_R32_UINT;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;
	ID3D11UnorderedAccessView* pUAV = nullptr;
	hr = g_pD3DDevice->CreateUnorderedAccessView(pTexture, &uavDesc, &pUAV);
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11ExpandPaletteTexture: Failed to create UAV (hr=0x%08X)", hr);
		return false;
	}

	// Dispatch 8x8 thread groups: t0=P8 source, t1=palette, u0=destination
	ID3D11ShaderResourceView* srvs[2] = { g_pD3D11UnswizzleSRV, g_pD3D11PaletteSRV };
	UINT groupsX = (width + 7) / 8;
	UINT groupsY = (height + 7) / 8;
	CxbxD3D11DispatchCS(g_pD3D11PaletteExpandCS, g_pD3D11PaletteExpandCB,
		2, srvs, pUAV, groupsX, groupsY, 1);

	pUAV->Release();
	return true;
}

// ******************************************************************
// * GPU vertex format conversion
// ******************************************************************

static void CxbxEnsureVertexConvertSrcBuffer(UINT requiredSize)
{
	CxbxD3D11EnsureRawStagingBuffer(requiredSize,
		&g_pD3D11VertexConvertSrcBuf, &g_VertexConvertSrcBufSize,
		&g_pD3D11VertexConvertSrcSRV, "CxbxEnsureVertexConvertSrcBuffer");
}

bool CxbxD3D11ConvertVertexBufferGPU(
	const uint8_t* pSrcVertexData,
	UINT srcDataSize,
	UINT vertexCount,
	UINT srcStride,
	UINT dstStride,
	UINT numElements,
	const UINT* pElementDescriptors, // 4 UINTs per element: srcOffset, dstOffset, convType, copyDwords
	UINT dstBufferSize,
	IDirect3DVertexBuffer** ppOutputVB)
{
	if (!g_pD3D11VertexConvertCS || !g_pD3D11VertexConvertCB || vertexCount == 0)
		return false;

	// Require dst stride to be a multiple of 4 (for RWBuffer<uint> writes)
	if ((dstStride & 3) != 0)
		return false;

	UINT bufferSize = (srcDataSize + 3) & ~3u;

	// Upload source vertex data
	CxbxEnsureVertexConvertSrcBuffer(bufferSize);
	if (!g_pD3D11VertexConvertSrcBuf || !g_pD3D11VertexConvertSrcSRV)
		return false;

	HRESULT hr = CxbxD3D11UpdateDynamicBuffer(g_pD3D11VertexConvertSrcBuf, pSrcVertexData, srcDataSize);
	if (FAILED(hr))
		return false;

	// Update constant buffer: header (4 uints) + elements (numElements * 4 uints)
	D3D11_MAPPED_SUBRESOURCE mapped = {};
	hr = g_pD3DDeviceContext->Map(g_pD3D11VertexConvertCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (FAILED(hr))
		return false;
	UINT* pCB = (UINT*)mapped.pData;
	pCB[0] = vertexCount;
	pCB[1] = srcStride;
	pCB[2] = dstStride;
	pCB[3] = numElements;
	memcpy(&pCB[4], pElementDescriptors, numElements * 4 * sizeof(UINT));
	// Zero unused element slots
	if (numElements < 16)
		memset(&pCB[4 + numElements * 4], 0, (16 - numElements) * 4 * sizeof(UINT));
	g_pD3DDeviceContext->Unmap(g_pD3D11VertexConvertCB, 0);

	// Create output vertex buffer (DEFAULT + VERTEX_BUFFER + UAV)
	D3D11_BUFFER_DESC outDesc = {};
	outDesc.ByteWidth = dstBufferSize;
	outDesc.Usage = D3D11_USAGE_DEFAULT;
	outDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
	ID3D11Buffer* pOutputBuf = nullptr;
	hr = g_pD3DDevice->CreateBuffer(&outDesc, nullptr, &pOutputBuf);
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11ConvertVertexBufferGPU: Failed to create output VB (hr=0x%08X)", hr);
		return false;
	}

	ID3D11UnorderedAccessView* pUAV = nullptr;
	hr = CxbxD3D11CreateTypedBufferUAV(pOutputBuf, dstBufferSize, &pUAV);
	if (FAILED(hr)) {
		EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11ConvertVertexBufferGPU: Failed to create UAV");
		pOutputBuf->Release();
		return false;
	}

	// Dispatch CS
	UINT numGroups = (vertexCount + 63) / 64;
	CxbxD3D11DispatchCS(g_pD3D11VertexConvertCS, g_pD3D11VertexConvertCB,
		1, &g_pD3D11VertexConvertSrcSRV, pUAV, numGroups, 1, 1);

	pUAV->Release();
	*ppOutputVB = pOutputBuf;
	return true;
}

// ******************************************************************
// * GPU index buffer conversion helpers
// ******************************************************************

static bool CxbxEnsureIndexConvertInputBuffer(UINT requiredSize)
{
	if (g_pD3D11IndexConvertInputBuf && g_IndexConvertInputBufSize >= requiredSize)
		return true;

	// Release old resources
	if (g_pD3D11IndexConvertInputSRV) { g_pD3D11IndexConvertInputSRV->Release(); g_pD3D11IndexConvertInputSRV = nullptr; }
	if (g_pD3D11IndexConvertInputBuf) { g_pD3D11IndexConvertInputBuf->Release(); g_pD3D11IndexConvertInputBuf = nullptr; }

	UINT newSize = (requiredSize + 4095) & ~4095u;
	if (newSize < 4) newSize = 4;

	HRESULT hr = CxbxD3D11CreateRawBuffer(newSize, /*bDynamic=*/false, 0, &g_pD3D11IndexConvertInputBuf);
	if (FAILED(hr)) return false;

	hr = CxbxD3D11CreateRawBufferSRV(g_pD3D11IndexConvertInputBuf, newSize, &g_pD3D11IndexConvertInputSRV);
	if (FAILED(hr)) {
		g_pD3D11IndexConvertInputBuf->Release();
		g_pD3D11IndexConvertInputBuf = nullptr;
		return false;
	}

	g_IndexConvertInputBufSize = newSize;
	return true;
}

static bool CxbxEnsureIndexConvertOutputBuffer(UINT requiredByteSize)
{
	if (g_pD3D11IndexConvertOutputBuf && g_IndexConvertOutputBufSize >= requiredByteSize)
		return true;

	// Release old resources
	if (g_pD3D11IndexConvertOutputUAV) { g_pD3D11IndexConvertOutputUAV->Release(); g_pD3D11IndexConvertOutputUAV = nullptr; }
	if (g_pD3D11IndexConvertOutputBuf) { g_pD3D11IndexConvertOutputBuf->Release(); g_pD3D11IndexConvertOutputBuf = nullptr; }

	UINT newSize = (requiredByteSize + 4095) & ~4095u;
	if (newSize < 4) newSize = 4;

	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = newSize;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;

	HRESULT hr = g_pD3DDevice->CreateBuffer(&desc, nullptr, &g_pD3D11IndexConvertOutputBuf);
	if (FAILED(hr)) return false;

	hr = CxbxD3D11CreateTypedBufferUAV(g_pD3D11IndexConvertOutputBuf, newSize, &g_pD3D11IndexConvertOutputUAV);
	if (FAILED(hr)) {
		g_pD3D11IndexConvertOutputBuf->Release();
		g_pD3D11IndexConvertOutputBuf = nullptr;
		return false;
	}

	g_IndexConvertOutputBufSize = newSize;
	return true;
}

bool CxbxD3D11ConvertIndexBufferGPU(
	const INDEX16* pSourceIndices,
	UINT sourceVertexCount,
	UINT outputIndexCount,
	int conversionMode)
{
	if (!g_pD3D11IndexConvertCS || !g_pD3D11IndexConvertCB || outputIndexCount == 0)
		return false;

	// 1. Upload source indices to input staging buffer (only if indexed)
	if (pSourceIndices != nullptr) {
		UINT inputByteSize = sourceVertexCount * sizeof(INDEX16);
		if (!CxbxEnsureIndexConvertInputBuffer(inputByteSize))
			return false;

		D3D11_BOX box = { 0, 0, 0, inputByteSize, 1, 1 };
		g_pD3DDeviceContext->UpdateSubresource(g_pD3D11IndexConvertInputBuf, 0, &box, pSourceIndices, inputByteSize, 0);
	}

	// 2. Ensure output buffer is large enough
	// Output: ceil(outputIndexCount / 2) uint32 elements (packed 16-bit pairs)
	UINT outputUintCount = (outputIndexCount + 1) / 2;
	UINT outputByteSize = outputUintCount * sizeof(UINT);
	if (!CxbxEnsureIndexConvertOutputBuffer(outputByteSize))
		return false;

	// 3. Update constant buffer
	struct { UINT VertexCount, ConversionMode, IsIndexed, Pad; } cbData = {
		sourceVertexCount,
		(UINT)conversionMode,
		pSourceIndices != nullptr ? 1u : 0u,
		0
	};
	g_pD3DDeviceContext->UpdateSubresource(g_pD3D11IndexConvertCB, 0, nullptr, &cbData, sizeof(cbData), 0);

	// 4. Bind resources and dispatch
	UINT numSRVs = (pSourceIndices != nullptr) ? 1 : 0;
	UINT numWorkItems;
	if (conversionMode <= CXBX_INDEX_CONVERT_QUAD_CCW) {
		numWorkItems = sourceVertexCount / 4; // one thread per quad
	} else {
		UINT numTris = (sourceVertexCount >= 3) ? (sourceVertexCount - 2) : 0;
		numWorkItems = (numTris + 1) / 2; // one thread per pair of triangles
	}
	UINT numGroups = (numWorkItems + 63) / 64;
	CxbxD3D11DispatchCS(g_pD3D11IndexConvertCS, g_pD3D11IndexConvertCB,
		numSRVs, &g_pD3D11IndexConvertInputSRV, g_pD3D11IndexConvertOutputUAV,
		numGroups, 1, 1);

	// 5. Bind output as index buffer (R16_UINT — the CS packed 16-bit indices into 32-bit writes)
	g_pD3DDeviceContext->IASetIndexBuffer(g_pD3D11IndexConvertOutputBuf, DXGI_FORMAT_R16_UINT, 0);

	return true;
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
	ClearRTVCache();
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

		// Check RTV cache first
		auto it = g_RTVCache.find(pHostRenderTarget);
		if (it != g_RTVCache.end()) {
			if (g_pD3DCurrentRTV != nullptr && g_pD3DCurrentRTV != g_pD3DBackBufferView) {
				// Don't release — it's in the cache
			}
			g_pD3DCurrentRTV = it->second;
			g_pD3DDeviceContext->OMSetRenderTargets(1, &g_pD3DCurrentRTV, g_pD3DDepthStencilView);
			hRet = S_OK;
		} else {
			D3D11_TEXTURE2D_DESC textureDesc = {};
			pHostRenderTarget->GetDesc(&textureDesc);

			D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
			renderTargetViewDesc.Format = textureDesc.Format;
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			renderTargetViewDesc.Texture2D.MipSlice = 0;

			ID3D11RenderTargetView* renderTargetView = nullptr;
			hRet = g_pD3DDevice->CreateRenderTargetView((ID3D11Resource*)pHostRenderTarget, &renderTargetViewDesc, &renderTargetView);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateRenderTargetView");

			if (SUCCEEDED(hRet)) {
				g_RTVCache[pHostRenderTarget] = renderTargetView;
				if (g_pD3DCurrentRTV != nullptr && g_pD3DCurrentRTV != g_pD3DBackBufferView) {
					// Don't release — it's in the cache
				}
				g_pD3DCurrentRTV = renderTargetView;
				g_pD3DDeviceContext->OMSetRenderTargets(1, &renderTargetView, g_pD3DDepthStencilView);
			}
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
	CxbxBindThickLineGS(XboxPrimitiveType);
	g_pD3DDeviceContext->IASetPrimitiveTopology(EmuXB2PC_D3D11PrimitiveTopology(XboxPrimitiveType));
	g_pD3DDeviceContext->DrawIndexed(IndexCount, StartIndex, BaseVertexIndex);
	CxbxUnbindThickLineGS(XboxPrimitiveType);
	return S_OK;
}

HRESULT CxbxDrawPrimitive(xbox::X_D3DPRIMITIVETYPE XboxPrimitiveType, UINT VertexCount, UINT StartVertex, UINT PrimCount)
{
	CxbxBindThickLineGS(XboxPrimitiveType);
	g_pD3DDeviceContext->IASetPrimitiveTopology(EmuXB2PC_D3D11PrimitiveTopology(XboxPrimitiveType));
	g_pD3DDeviceContext->Draw(VertexCount, StartVertex);
	CxbxUnbindThickLineGS(XboxPrimitiveType);
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
	if (FAILED(CxbxD3D11UpdateDynamicBuffer(pBuffer, pData, size)))
		return nullptr;
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

void CxbxD3D11SetVertexDeclaration(CxbxVertexDeclaration* pCxbxVertexDeclaration)
{
	// Lazily create the input layout when we have elements but no layout yet
	if (pCxbxVertexDeclaration != nullptr && pCxbxVertexDeclaration->pHostVertexDeclaration == nullptr
		&& pCxbxVertexDeclaration->pD3D11InputElements != nullptr && pCxbxVertexDeclaration->D3D11InputElementCount > 0) {
		ID3DBlob* pBytecode = CxbxGetActiveVertexShaderBytecode();
		if (pBytecode != nullptr) {
			HRESULT hRet = g_pD3DDevice->CreateInputLayout(
				pCxbxVertexDeclaration->pD3D11InputElements,
				pCxbxVertexDeclaration->D3D11InputElementCount,
				pBytecode->GetBufferPointer(),
				pBytecode->GetBufferSize(),
				&pCxbxVertexDeclaration->pHostVertexDeclaration
			);
			if (FAILED(hRet)) {
				EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11SetVertexDeclaration: CreateInputLayout failed (0x%08X) elements=%u bytecodeSize=%zu",
					hRet,
					pCxbxVertexDeclaration->D3D11InputElementCount,
					pBytecode->GetBufferSize());
				for (UINT i = 0; i < pCxbxVertexDeclaration->D3D11InputElementCount; i++) {
					auto& e = pCxbxVertexDeclaration->pD3D11InputElements[i];
					EmuLog(LOG_LEVEL::WARNING, "  [%u] Semantic=%s/%u Fmt=%u Slot=%u Offset=%u Class=%u Step=%u",
						i, e.SemanticName ? e.SemanticName : "(null)", e.SemanticIndex,
						e.Format, e.InputSlot, e.AlignedByteOffset,
						e.InputSlotClass, e.InstanceDataStepRate);
				}
			}
		}
	}

	g_pD3DDeviceContext->IASetInputLayout(
		pCxbxVertexDeclaration != nullptr ? pCxbxVertexDeclaration->pHostVertexDeclaration : nullptr);
}

// ******************************************************************
// * Dual-backend wrappers — D3D11 implementations
// ******************************************************************

HRESULT CxbxSetVertexShader(IDirect3DVertexShader* pHostVertexShader)
{
	g_pD3DDeviceContext->VSSetShader(pHostVertexShader, nullptr, 0);
	return S_OK;
}

IDirect3DVertexDeclaration* CxbxCreateHostVertexDeclaration(D3DVERTEXELEMENT *pDeclaration)
{
	// For D3D11, we cannot create an input layout without compiled shader bytecode.
	// Return nullptr here; the actual ID3D11InputLayout will be created lazily
	// in CxbxSetHostVertexDeclaration when both elements and a compiled shader are available.
	(void)pDeclaration;
	return nullptr;
}

void CxbxSetHostVertexDeclaration(CxbxVertexDeclaration* pCxbxVertexDeclaration)
{
	CxbxD3D11SetVertexDeclaration(pCxbxVertexDeclaration);
}

void CxbxSetFogColor(uint32_t fog_color)
{
	// D3D11: Set fog color in PS constant buffer at PSH_XBOX_CONSTANT_FOG (register 18)
	D3DXCOLOR fogColorFloat(fog_color); // ARGB → correct R,G,B,A
	CxbxSetPixelShaderConstantF(/*PSH_XBOX_CONSTANT_FOG=*/18, (const float*)&fogColorFloat, 1);
}

void CxbxGetBumpEnvMatrix(int stage, DWORD value[4])
{
	value[0] = XboxTextureStates.Get(stage, xbox::X_D3DTSS_BUMPENVMAT00);
	value[1] = XboxTextureStates.Get(stage, xbox::X_D3DTSS_BUMPENVMAT01);
	value[2] = XboxTextureStates.Get(stage, xbox::X_D3DTSS_BUMPENVMAT10);
	value[3] = XboxTextureStates.Get(stage, xbox::X_D3DTSS_BUMPENVMAT11);
}

void CxbxGetBumpEnvLuminance(int stage, DWORD value[2])
{
	value[0] = XboxTextureStates.Get(stage, xbox::X_D3DTSS_BUMPENVLSCALE);
	value[1] = XboxTextureStates.Get(stage, xbox::X_D3DTSS_BUMPENVLOFFSET);
}

#endif // CXBX_USE_D3D11
