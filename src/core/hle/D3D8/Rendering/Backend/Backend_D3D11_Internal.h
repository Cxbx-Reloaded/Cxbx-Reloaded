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

// Internal shared state for Backend_D3D11 split files.
// Not for use outside the Backend/ folder.
#ifndef BACKEND_D3D11_INTERNAL_H
#define BACKEND_D3D11_INTERNAL_H

#ifdef CXBX_USE_D3D11

#define LOG_PREFIX CXBXR_MODULE::D3D8

#include "Backend_D3D11.h"
#include "../RenderGlobals.h"
#include "core\kernel\init\CxbxKrnl.h"
#include "core\hle\D3D8\XbD3D8Logging.h"
#include "core\hle\D3D8\XbConvert.h"
#include "core\hle\D3D8\XbVertexShader.h"
#include "../TextureStates.h"
#include "core\hle\D3D8\Rendering\Shaders\Shader.h"

#include <cstring>
#include <unordered_map>
#include <utility>
#include <vector>
#include <wrl/client.h>
using namespace Microsoft::WRL;

// ******************************************************************
// * D3D11 state objects (shared across split files)
// ******************************************************************
extern ComPtr<ID3D11RasterizerState>   g_pD3DRasterizerState;
extern ComPtr<ID3D11DepthStencilState> g_pD3DDepthStencilState;
extern ComPtr<ID3D11BlendState>        g_pD3DBlendState;

// ******************************************************************
// * Constant buffer shadow arrays
// ******************************************************************
extern float g_D3D11VSConstants[CXBX_D3D11_VS_CB_COUNT][4];
extern bool  g_bD3D11VSConstantsDirty;
extern float g_D3D11PSConstants[CXBX_D3D11_PS_CB_COUNT][4];
extern bool  g_bD3D11PSConstantsDirty;

// ******************************************************************
// * Blit shader resources
// ******************************************************************
extern ID3D11VertexShader  *g_pD3D11BlitVS;
extern ID3D11PixelShader   *g_pD3D11BlitPS;
extern ID3D11SamplerState  *g_pD3D11BlitSamplerLinear;
extern ID3D11SamplerState  *g_pD3D11BlitSamplerPoint;

// ******************************************************************
// * Geometry shader resources
// ******************************************************************
extern ID3D11GeometryShader *g_pD3D11PointSpriteGS;
extern ID3D11Buffer         *g_pD3D11GSConstantBuffer;
extern bool                  g_bPointSpriteEnabled;
extern ID3D11GeometryShader *g_pD3D11ThickLineGS;
extern float                 g_fLineWidth;

// ******************************************************************
// * Compute shader resources — unswizzle
// ******************************************************************
extern ID3D11ComputeShader       *g_pD3D11UnswizzleCS;
extern ID3D11Buffer              *g_pD3D11UnswizzleCB;
extern ID3D11Buffer              *g_pD3D11UnswizzleStagingBuf;
extern UINT                       g_UnswizzleStagingBufSize;
extern ID3D11ShaderResourceView  *g_pD3D11UnswizzleSRV;

// ******************************************************************
// * Compute shader resources — index convert
// ******************************************************************
extern ID3D11ComputeShader       *g_pD3D11IndexConvertCS;
extern ID3D11Buffer              *g_pD3D11IndexConvertCB;
extern ID3D11Buffer              *g_pD3D11IndexConvertInputBuf;
extern UINT                       g_IndexConvertInputBufSize;
extern ID3D11ShaderResourceView  *g_pD3D11IndexConvertInputSRV;
extern ID3D11Buffer              *g_pD3D11IndexConvertOutputBuf;
extern UINT                       g_IndexConvertOutputBufSize;
extern ID3D11UnorderedAccessView *g_pD3D11IndexConvertOutputUAV;

// ******************************************************************
// * Compute shader resources — palette expand
// ******************************************************************
extern ID3D11ComputeShader       *g_pD3D11PaletteExpandCS;
extern ID3D11Buffer              *g_pD3D11PaletteExpandCB;
extern ID3D11Buffer              *g_pD3D11PaletteBuf;
extern ID3D11ShaderResourceView  *g_pD3D11PaletteSRV;

// ******************************************************************
// * Compute shader resources — format conversion
// ******************************************************************
extern ID3D11ComputeShader       *g_pD3D11FormatConvertCS;
extern ID3D11Buffer              *g_pD3D11FormatConvertCB;

// ******************************************************************
// * Compute shader resources — vertex convert
// ******************************************************************
extern ID3D11ComputeShader       *g_pD3D11VertexConvertCS;
extern ID3D11Buffer              *g_pD3D11VertexConvertCB;
extern ID3D11Buffer              *g_pD3D11VertexConvertSrcBuf;
extern UINT                       g_VertexConvertSrcBufSize;
extern ID3D11ShaderResourceView  *g_pD3D11VertexConvertSrcSRV;

// ******************************************************************
// * Internal helper functions (shared across split files)
// ******************************************************************

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

HRESULT CxbxD3D11CreateConstantBuffer(UINT byteWidth, bool bDynamic, ID3D11Buffer** ppBuffer);
HRESULT CxbxD3D11CreateRawBuffer(UINT byteWidth, bool bDynamic, UINT extraBindFlags, ID3D11Buffer** ppBuffer);
HRESULT CxbxD3D11CreateRawBufferSRV(ID3D11Buffer* pBuffer, UINT byteWidth, ID3D11ShaderResourceView** ppSRV);
HRESULT CxbxD3D11CreateTypedBufferUAV(ID3D11Buffer* pBuffer, UINT byteWidth, ID3D11UnorderedAccessView** ppUAV);
void CxbxD3D11EnsureRawStagingBuffer(UINT requiredSize, ID3D11Buffer** ppBuffer, UINT* pCurrentSize, ID3D11ShaderResourceView** ppSRV, const char* debugName);
void CxbxD3D11DispatchCS(ID3D11ComputeShader* pShader, ID3D11Buffer* pCB, UINT numSRVs, ID3D11ShaderResourceView* const* ppSRVs, ID3D11UnorderedAccessView* pUAV, UINT groupsX, UINT groupsY, UINT groupsZ);
HRESULT CxbxD3D11UpdateDynamicBuffer(ID3D11Buffer* pBuffer, const void* pData, size_t dataSize);

// RTV cache helpers
using RTVCacheKey = std::pair<IDirect3DSurface*, UINT>;
struct RTVCacheKeyHash {
	size_t operator()(const RTVCacheKey& k) const {
		return std::hash<IDirect3DSurface*>()(k.first) ^ (static_cast<size_t>(k.second) << 16);
	}
};
void ClearRTVCache();
extern std::unordered_map<RTVCacheKey, ID3D11RenderTargetView*, RTVCacheKeyHash> g_RTVCache;

// Scene begin/end (defined in HostDevice.cpp or HostRender.cpp)
extern void CxbxBeginScene();
extern void CxbxEndScene();

#endif // CXBX_USE_D3D11
#endif // BACKEND_D3D11_INTERNAL_H
