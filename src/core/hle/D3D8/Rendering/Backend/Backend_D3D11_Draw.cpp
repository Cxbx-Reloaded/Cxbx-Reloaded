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

	// Diagnostic: log clear color (first 3 occurrences only)
	{
		static int s_clearDiag = 0;
		if (s_clearDiag < 3) {
			s_clearDiag++;
			EmuLog(LOG_LEVEL::INFO, "Clear diag [%d]: Color=0x%08X -> RGBA(%f,%f,%f,%f) Flags=0x%X",
				s_clearDiag, Color, clearColor[0], clearColor[1], clearColor[2], clearColor[3], Flags);
		}
	}

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

// Filter D3D11 input layout elements to only include semantics present in
// the shader's input signature (ISGN/ISG1 chunk in DXBC bytecode).
// D3D11 returns E_INVALIDARG from CreateInputLayout when an element references
// a semantic the vertex shader doesn't declare (e.g. the HLSL compiler optimized
// away an unused TEXCOORD slot).  The NV2A has 16 attribute slots that all map
// to TEXCOORD0-15, but a given shader may only use a subset.
//
// Additionally, DXVK (unlike native D3D11) requires ALL non-builtin shader
// inputs to have a corresponding input layout element.  Native D3D11 defaults
// missing inputs to (0,0,0,1), but DXVK returns E_INVALIDARG.  To handle this,
// we add dummy elements (slot 0, offset 0) for ISGN entries that have no
// matching vertex buffer element.  The dummy reads overlap real vertex data,
// but the shader ignores these values (they're either optimized away or
// overridden by vRegisterDefaultValues via the lerp in init_v).
std::vector<D3D11_INPUT_ELEMENT_DESC> FilterInputElementsByShaderSignature(
	const D3D11_INPUT_ELEMENT_DESC* pElements, UINT elementCount,
	const void* bytecode, size_t bytecodeSize)
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> result;

	// Parse DXBC header to find the ISGN (or ISG1) chunk
	auto data = static_cast<const uint8_t*>(bytecode);
	if (bytecodeSize < 32 || memcmp(data, "DXBC", 4) != 0) {
		// Not valid DXBC — return all elements unfiltered
		result.assign(pElements, pElements + elementCount);
		return result;
	}

	uint32_t chunkCount = *reinterpret_cast<const uint32_t*>(data + 28);
	if (32 + chunkCount * 4 > bytecodeSize) {
		result.assign(pElements, pElements + elementCount);
		return result;
	}

	const uint8_t* isgnData = nullptr;
	uint32_t isgnSize = 0;
	bool isISG1 = false;
	for (uint32_t i = 0; i < chunkCount; i++) {
		uint32_t offset = *reinterpret_cast<const uint32_t*>(data + 32 + i * 4);
		if (offset + 8 > bytecodeSize)
			continue;
		uint32_t fourCC = *reinterpret_cast<const uint32_t*>(data + offset);
		// ISGN = 0x4E475349 ("ISGN"), ISG1 = 0x31475349 ("ISG1")
		if (fourCC == 0x4E475349 || fourCC == 0x31475349) {
			isgnSize = *reinterpret_cast<const uint32_t*>(data + offset + 4);
			isgnData = data + offset + 8;
			isISG1 = (fourCC == 0x31475349);
			break;
		}
	}

	if (isgnData == nullptr || isgnSize < 8) {
		EmuLog(LOG_LEVEL::WARNING, "FilterInputElements: No ISGN chunk found in bytecode (%zu bytes)", bytecodeSize);
		result.assign(pElements, pElements + elementCount);
		return result;
	}

	uint32_t sigElementCount = *reinterpret_cast<const uint32_t*>(isgnData);
	// ISG1 uses 32 bytes per element (extra Stream field), ISGN uses 24
	uint32_t elemStride = isISG1 ? 32 : 24;
	// ISG1 element layout: Stream(4), NameOffset(4), SemanticIndex(4), SystemValue(4), ...
	// ISGN element layout: NameOffset(4), SemanticIndex(4), SystemValue(4), ...
	uint32_t nameFieldOff   = isISG1 ? 4 : 0;
	uint32_t semIdxFieldOff = isISG1 ? 8 : 4;
	uint32_t sysvalFieldOff = isISG1 ? 12 : 8;

	if (8 + sigElementCount * elemStride > isgnSize) {
		EmuLog(LOG_LEVEL::WARNING, "FilterInputElements: ISGN size mismatch (count=%u stride=%u isgnSize=%u)", sigElementCount, elemStride, isgnSize);
		result.assign(pElements, pElements + elementCount);
		return result;
	}

	// Log ISGN contents for debugging
	EmuLog(LOG_LEVEL::DEBUG, "FilterInputElements: ISGN has %u elements (stride=%u, isgnSize=%u, bytecodeSize=%zu, ISG1=%d)",
		sigElementCount, elemStride, isgnSize, bytecodeSize, isISG1);
	for (uint32_t s = 0; s < sigElementCount; s++) {
		const uint8_t* sigElem = isgnData + 8 + s * elemStride;
		uint32_t nameOffset = *reinterpret_cast<const uint32_t*>(sigElem + nameFieldOff);
		uint32_t semIndex   = *reinterpret_cast<const uint32_t*>(sigElem + semIdxFieldOff);
		const char* sigName = (nameOffset < isgnSize) ? reinterpret_cast<const char*>(isgnData + nameOffset) : "(invalid)";
		EmuLog(LOG_LEVEL::DEBUG, "  ISGN[%u]: %s/%u", s, sigName, semIndex);
	}

	// Track which ISGN entries are matched by input layout elements
	std::vector<bool> isgnMatched(sigElementCount, false);

	// For each input layout element, check if the shader has a matching input
	for (UINT e = 0; e < elementCount; e++) {
		bool found = false;
		for (uint32_t s = 0; s < sigElementCount; s++) {
			const uint8_t* sigElem = isgnData + 8 + s * elemStride;
			uint32_t nameOffset = *reinterpret_cast<const uint32_t*>(sigElem + nameFieldOff);
			uint32_t semIndex   = *reinterpret_cast<const uint32_t*>(sigElem + semIdxFieldOff);
			if (nameOffset >= isgnSize)
				continue;
			const char* sigName = reinterpret_cast<const char*>(isgnData + nameOffset);
			if (semIndex == pElements[e].SemanticIndex
				&& pElements[e].SemanticName != nullptr
				&& _stricmp(sigName, pElements[e].SemanticName) == 0) {
				found = true;
				isgnMatched[s] = true;
				break;
			}
		}
		if (found) {
			result.push_back(pElements[e]);
		}
	}

	// DXVK compatibility: add dummy elements for unmatched ISGN entries.
	// DXVK (unlike native D3D11) requires every non-builtin shader input to
	// have a corresponding input layout element, returning E_INVALIDARG otherwise.
	// Dummy elements read from slot 0 / offset 0 with a small format; the shader
	// either doesn't use the value or overrides it via vRegisterDefaultValues.
	uint32_t dummyCount = 0;
	for (uint32_t s = 0; s < sigElementCount; s++) {
		if (isgnMatched[s]) continue;
		const uint8_t* sigElem = isgnData + 8 + s * elemStride;
		uint32_t nameOffset = *reinterpret_cast<const uint32_t*>(sigElem + nameFieldOff);
		uint32_t semIndex   = *reinterpret_cast<const uint32_t*>(sigElem + semIdxFieldOff);
		uint32_t sysval     = *reinterpret_cast<const uint32_t*>(sigElem + sysvalFieldOff);
		// Skip built-in system-value inputs (SV_VertexID, SV_InstanceID, etc.)
		if (sysval != 0) continue;
		if (nameOffset >= isgnSize) continue;
		const char* sigName = reinterpret_cast<const char*>(isgnData + nameOffset);

		D3D11_INPUT_ELEMENT_DESC dummy = {};
		dummy.SemanticName = sigName; // Points into bytecode, valid for CreateInputLayout lifetime
		dummy.SemanticIndex = semIndex;
		dummy.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 4 bytes, universally supported, 4-byte aligned
		dummy.InputSlot = 0;
		dummy.AlignedByteOffset = 0;
		dummy.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		dummy.InstanceDataStepRate = 0;
		result.push_back(dummy);
		dummyCount++;
	}

	EmuLog(LOG_LEVEL::DEBUG, "FilterInputElements: %u of %u elements matched ISGN, %u dummy elements added for DXVK compat",
		(unsigned)(result.size() - dummyCount), elementCount, dummyCount);

	return result;
}

void CxbxD3D11SetVertexDeclaration(CxbxVertexDeclaration* pCxbxVertexDeclaration)
{
	// Lazily create the input layout when we have elements but no layout yet
	if (pCxbxVertexDeclaration != nullptr && pCxbxVertexDeclaration->pHostVertexDeclaration == nullptr
		&& pCxbxVertexDeclaration->pD3D11InputElements != nullptr && pCxbxVertexDeclaration->D3D11InputElementCount > 0) {
		ID3DBlob* pBytecode = CxbxGetActiveVertexShaderBytecode();
		if (pBytecode == nullptr) {
			// No fallback available either; try the fixed-function bytecode
			pBytecode = CxbxGetFixedFunctionVertexShaderBytecode();
		}
		if (pBytecode != nullptr) {
			// Filter input elements to only those the shader actually declares,
			// since D3D11 rejects CreateInputLayout when an element references a
			// semantic not present in the shader's input signature.
			auto filtered = FilterInputElementsByShaderSignature(
				pCxbxVertexDeclaration->pD3D11InputElements,
				pCxbxVertexDeclaration->D3D11InputElementCount,
				pBytecode->GetBufferPointer(),
				pBytecode->GetBufferSize());

			HRESULT hRet = E_FAIL;
			if (!filtered.empty()) {
				EmuLog(LOG_LEVEL::DEBUG, "CxbxD3D11SetVertexDeclaration: Trying CreateInputLayout with %u filtered elements, bytecodeSize=%zu",
					(unsigned)filtered.size(), pBytecode->GetBufferSize());
				for (UINT i = 0; i < (UINT)filtered.size(); i++) {
					auto& e = filtered[i];
					EmuLog(LOG_LEVEL::DEBUG, "  filtered[%u] Semantic=%s/%u Fmt=%u Slot=%u Offset=%u",
						i, e.SemanticName ? e.SemanticName : "(null)", e.SemanticIndex,
						e.Format, e.InputSlot, e.AlignedByteOffset);
				}
				hRet = g_pD3DDevice->CreateInputLayout(
					filtered.data(),
					(UINT)filtered.size(),
					pBytecode->GetBufferPointer(),
					pBytecode->GetBufferSize(),
					&pCxbxVertexDeclaration->pHostVertexDeclaration
				);
				if (FAILED(hRet)) {
					EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11SetVertexDeclaration: Primary CreateInputLayout failed (0x%08X)", hRet);
				}
			} else {
				EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11SetVertexDeclaration: Filter produced 0 elements from %u originals", pCxbxVertexDeclaration->D3D11InputElementCount);
			}
			// If layout creation still failed, retry with the FixedFunction
			// shader bytecode which declares all 16 TEXCOORD inputs.
			if (FAILED(hRet)) {
				ID3DBlob* pFallback = CxbxGetFixedFunctionVertexShaderBytecode();
				if (pFallback != nullptr && pFallback != pBytecode) {
					auto fbFiltered = FilterInputElementsByShaderSignature(
						pCxbxVertexDeclaration->pD3D11InputElements,
						pCxbxVertexDeclaration->D3D11InputElementCount,
						pFallback->GetBufferPointer(),
						pFallback->GetBufferSize());
					if (!fbFiltered.empty()) {
						EmuLog(LOG_LEVEL::DEBUG, "CxbxD3D11SetVertexDeclaration: Fallback with %u elements, bytecodeSize=%zu",
							(unsigned)fbFiltered.size(), pFallback->GetBufferSize());
						hRet = g_pD3DDevice->CreateInputLayout(
							fbFiltered.data(),
							(UINT)fbFiltered.size(),
							pFallback->GetBufferPointer(),
							pFallback->GetBufferSize(),
							&pCxbxVertexDeclaration->pHostVertexDeclaration
						);
						if (FAILED(hRet)) {
							EmuLog(LOG_LEVEL::WARNING, "CxbxD3D11SetVertexDeclaration: Fallback CreateInputLayout also failed (0x%08X)", hRet);
						}
					}
				}
			}
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
