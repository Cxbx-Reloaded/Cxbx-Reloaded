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

#ifndef CXBX_USE_D3D11

#define LOG_PREFIX CXBXR_MODULE::D3D8

#include "Backend_D3D9.h"
#include "RenderGlobals.h" // g_pD3DDevice
#include "core\kernel\init\CxbxKrnl.h" // LOG_INIT, EmuLog, CxbxrAbort
#include "core\hle\D3D8\XbD3D8Logging.h" // DEBUG_D3DRESULT
#include "core\hle\D3D8\XbConvert.h" // EmuXB2PC_D3DPrimitiveType
#include "core\hle\D3D8\XbVertexShader.h" // CxbxVertexDeclaration

// ******************************************************************
// * D3D9 globals — definitions
// ******************************************************************
IDirect3D9Ex *g_pDirect3D = nullptr;
D3DCAPS       g_D3DCaps = {};

// ******************************************************************
// * Shader constant functions (D3D9: thin wrappers around device calls)
// ******************************************************************
void CxbxSetVertexShaderConstantF(UINT startRegister, const float* pConstantData, UINT Vector4fCount)
{
	if (g_pD3DDevice && pConstantData && Vector4fCount > 0) {
		g_pD3DDevice->SetVertexShaderConstantF(startRegister, pConstantData, Vector4fCount);
	}
}

void CxbxSetPixelShaderConstantF(UINT startRegister, const float* pConstantData, UINT Vector4fCount)
{
	if (g_pD3DDevice && pConstantData && Vector4fCount > 0) {
		g_pD3DDevice->SetPixelShaderConstantF(startRegister, pConstantData, Vector4fCount);
	}
}

void CxbxGetVertexShaderConstants(UINT startRegister, float* pConstantData, UINT Vector4fCount)
{
	if (g_pD3DDevice && pConstantData && Vector4fCount > 0) {
		g_pD3DDevice->GetVertexShaderConstantF(startRegister, pConstantData, Vector4fCount);
	}
}

// ******************************************************************
// * Rendering helpers (D3D9 implementations)
// ******************************************************************
HRESULT CxbxSetRenderTarget(IDirect3DSurface* pHostRenderTarget)
{
	LOG_INIT;
	HRESULT hRet = g_pD3DDevice->SetRenderTarget(/*RenderTargetIndex=*/0, pHostRenderTarget);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetRenderTarget");
	return hRet;
}

void CxbxD3DClear(DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
	LOG_INIT;
	HRESULT hRet = g_pD3DDevice->Clear(Count, pRects, Flags, Color, Z, Stencil);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->Clear");
}

void CxbxSetViewport(D3DVIEWPORT *pHostViewport)
{
	g_pD3DDevice->SetViewport(pHostViewport);
}

void CxbxSetScissorRect(CONST RECT *pHostViewportRect)
{
	g_pD3DDevice->SetScissorRect(pHostViewportRect);
}

void CxbxSetIndices(IDirect3DIndexBuffer* pHostIndexBuffer)
{
	LOG_INIT;
	HRESULT hRet = g_pD3DDevice->SetIndices(pHostIndexBuffer);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetIndices");
	if (FAILED(hRet))
		CxbxrAbort("CxbxSetIndices: SetIndices Failed!");
}

INDEX16* CxbxLockIndexBuffer(IDirect3DIndexBuffer* pHostIndexBuffer)
{
	LOG_INIT;
	INDEX16* pData = nullptr;
	HRESULT hRet = pHostIndexBuffer->Lock(0, /*entire SizeToLock=*/0, (D3DLockData**)&pData, D3DLOCK_DISCARD);
	DEBUG_D3DRESULT(hRet, "CxbxLockIndexBuffer: Lock");
	return pData;
}

void CxbxUnlockIndexBuffer(IDirect3DIndexBuffer* pHostIndexBuffer)
{
	LOG_INIT;
	HRESULT hRet = pHostIndexBuffer->Unlock();
	DEBUG_D3DRESULT(hRet, "CxbxUnlockIndexBuffer: Unlock");
}

HRESULT CxbxDrawIndexedPrimitive(xbox::X_D3DPRIMITIVETYPE XboxPrimitiveType, UINT IndexCount, INT BaseVertexIndex, UINT StartIndex, UINT MinIndex, UINT NumVertices, UINT PrimCount)
{
	return g_pD3DDevice->DrawIndexedPrimitive(
		EmuXB2PC_D3DPrimitiveType(XboxPrimitiveType),
		BaseVertexIndex,
		MinIndex,
		NumVertices,
		StartIndex,
		PrimCount);
}

HRESULT CxbxDrawPrimitive(xbox::X_D3DPRIMITIVETYPE XboxPrimitiveType, UINT VertexCount, UINT StartVertex, UINT PrimCount)
{
	return g_pD3DDevice->DrawPrimitive(
		EmuXB2PC_D3DPrimitiveType(XboxPrimitiveType),
		StartVertex,
		PrimCount);
}

HRESULT CxbxBltSurface(IDirect3DSurface* pSrc, const RECT* pSrcRect, IDirect3DSurface* pDst, const RECT* pDstRect, D3DTEXTUREFILTERTYPE Filter)
{
	HRESULT hRet = g_pD3DDevice->StretchRect(pSrc, pSrcRect, pDst, pDstRect, Filter);
	if (FAILED(hRet)) {
		// Fallback for cases StretchRect cannot handle (e.g. texture to texture)
		hRet = D3DXLoadSurfaceFromSurface(pDst, nullptr, pDstRect, pSrc, nullptr, pSrcRect, (Filter == D3DTEXF_LINEAR) ? D3DX_FILTER_LINEAR : D3DX_FILTER_POINT, 0);
	}
	return hRet;
}

HRESULT CxbxPresent()
{
	LOG_INIT;
	CxbxEndScene();
	HRESULT hRet = g_pD3DDevice->Present(0, 0, 0, 0);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->Present");
	CxbxBeginScene();
	return hRet;
}

void CxbxSetDepthStencilSurface(IDirect3DSurface* pHostDepthStencil)
{
	g_pD3DDevice->SetDepthStencilSurface(pHostDepthStencil);
}

IDirect3DSurface* CxbxGetCurrentRenderTarget()
{
	IDirect3DSurface* pHostRenderTarget = nullptr;
	g_pD3DDevice->GetRenderTarget(0, &pHostRenderTarget);
	return pHostRenderTarget;
}

HRESULT CxbxGetBackBuffer(IDirect3DSurface** ppBackBuffer)
{
	return g_pD3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, ppBackBuffer);
}

HRESULT CxbxSetStreamSource(UINT HostStreamNumber, IDirect3DVertexBuffer* pHostVertexBuffer, UINT VertexStride)
{
	return g_pD3DDevice->SetStreamSource(HostStreamNumber, pHostVertexBuffer, /*OffsetInBytes=*/0, VertexStride);
}

HRESULT CxbxCreateVertexBuffer(UINT Length, IDirect3DVertexBuffer** ppVertexBuffer)
{
	return g_pD3DDevice->CreateVertexBuffer(Length, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, ppVertexBuffer, nullptr);
}

void* CxbxLockVertexBuffer(IDirect3DVertexBuffer* pVertexBuffer)
{
	void* pData = nullptr;
	if (FAILED(pVertexBuffer->Lock(0, 0, (D3DLockData**)&pData, D3DLOCK_DISCARD))) {
		return nullptr;
	}
	return pData;
}

void CxbxUnlockVertexBuffer(IDirect3DVertexBuffer* pVertexBuffer)
{
	pVertexBuffer->Unlock();
}

HRESULT CxbxCreatePixelShader(const void* pFunction, SIZE_T FunctionSize, IDirect3DPixelShader** ppShader)
{
	return g_pD3DDevice->CreatePixelShader((const DWORD*)pFunction, ppShader);
}

void CxbxRawSetPixelShader(IDirect3DPixelShader* pPixelShader)
{
	g_pD3DDevice->SetPixelShader(pPixelShader);
}

// ******************************************************************
// * Dual-backend wrappers — D3D9 implementations
// ******************************************************************

HRESULT CxbxSetVertexShader(IDirect3DVertexShader* pHostVertexShader)
{
	return g_pD3DDevice->SetVertexShader(pHostVertexShader);
}

IDirect3DVertexDeclaration* CxbxCreateHostVertexDeclaration(D3DVERTEXELEMENT *pDeclaration)
{
	LOG_INIT; // Allows use of DEBUG_D3DRESULT

	IDirect3DVertexDeclaration* pHostVertexDeclaration = nullptr;
	HRESULT hRet = g_pD3DDevice->CreateVertexDeclaration(pDeclaration, &pHostVertexDeclaration);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateVertexDeclaration");
	return pHostVertexDeclaration;
}

void CxbxSetHostVertexDeclaration(CxbxVertexDeclaration* pCxbxVertexDeclaration)
{
	g_pD3DDevice->SetVertexDeclaration(
		pCxbxVertexDeclaration != nullptr ? pCxbxVertexDeclaration->pHostVertexDeclaration : nullptr);
}

void CxbxSetFogColor(uint32_t fog_color)
{
	g_pD3DDevice->SetRenderState(D3DRS_FOGCOLOR, fog_color);
}

void CxbxGetBumpEnvMatrix(int stage, DWORD value[4])
{
	g_pD3DDevice->GetTextureStageState(stage, D3DTSS_BUMPENVMAT00, &value[0]);
	g_pD3DDevice->GetTextureStageState(stage, D3DTSS_BUMPENVMAT01, &value[1]);
	g_pD3DDevice->GetTextureStageState(stage, D3DTSS_BUMPENVMAT10, &value[2]);
	g_pD3DDevice->GetTextureStageState(stage, D3DTSS_BUMPENVMAT11, &value[3]);
}

void CxbxGetBumpEnvLuminance(int stage, DWORD value[2])
{
	g_pD3DDevice->GetTextureStageState(stage, D3DTSS_BUMPENVLSCALE,  &value[0]);
	g_pD3DDevice->GetTextureStageState(stage, D3DTSS_BUMPENVLOFFSET, &value[1]);
}

#endif // !CXBX_USE_D3D11
