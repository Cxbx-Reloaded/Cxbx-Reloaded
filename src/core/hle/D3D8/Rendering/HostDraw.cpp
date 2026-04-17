// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
#include "EmuD3D8_common.h"
#include "IndexBufferConvert.h"
#include "Backend_D3D11.h"

class ConvertedIndexBuffer {
public:
	uint64_t Hash = 0;
	DWORD IndexCount = 0;
	IDirect3DIndexBuffer* pHostIndexBuffer = nullptr;
	INDEX16 LowIndex = 0;
	INDEX16 HighIndex = 0;

	~ConvertedIndexBuffer()
	{
		if (pHostIndexBuffer != nullptr) {
			pHostIndexBuffer->Release();
		}
	}
};

	std::unordered_map<uint32_t, ConvertedIndexBuffer> g_IndexBufferCache;

void CxbxRemoveIndexBuffer(PWORD pData)
{
	// HACK: Never Free
}

IDirect3DIndexBuffer* CxbxCreateIndexBuffer(unsigned IndexCount)
{
	LOG_INIT; // Allows use of DEBUG_D3DRESULT

	IDirect3DIndexBuffer* Result = nullptr;

	// Create a new native index buffer of the requested size :
	UINT uiIndexBufferSize = IndexCount * sizeof(INDEX16);
#ifdef CXBX_USE_D3D11
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = uiIndexBufferSize;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	HRESULT hRet = g_pD3DDevice->CreateBuffer(&bufferDesc, nullptr, &Result);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateBuffer (index buffer)");
#else
	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb147168(v=vs.85).aspx
	// "Managing Resources (Direct3D 9)"
	// suggests "for resources which change with high frequency" [...]
	// "D3DPOOL_DEFAULT along with D3DUSAGE_DYNAMIC should be used."
	const D3DPOOL D3DPool = D3DPOOL_DEFAULT;
	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb172625(v=vs.85).aspx
	// "Buffers created with D3DPOOL_DEFAULT that do not specify D3DUSAGE_WRITEONLY may suffer a severe performance penalty."
	const DWORD D3DUsage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY; // Was D3DUSAGE_WRITEONLY

	HRESULT hRet = g_pD3DDevice->CreateIndexBuffer(
		uiIndexBufferSize, // Size of the index buffer, in bytes.
		D3DUsage,
		/*Format=*/EMUFMT_INDEX16,
		D3DPool,
		&Result,
		nullptr // pSharedHandle
	);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->CreateIndexBuffer");
#endif
	if (FAILED(hRet)) {
		assert(Result == nullptr);
	}

	return Result;
}

ConvertedIndexBuffer& CxbxUpdateActiveIndexBuffer
(
	INDEX16* pXboxIndexData,
	unsigned XboxIndexCount,
	bool bConvertQuadListToTriangleList,
	bool bConvertTriFanToTriangleList = false
)
{
	LOG_INIT; // Allows use of DEBUG_D3DRESULT

	uint32_t LookupKey = (uint32_t)pXboxIndexData;
	unsigned RequiredIndexCount = XboxIndexCount;

	if (bConvertQuadListToTriangleList) {
		LOG_TEST_CASE("bConvertQuadListToTriangleList");
		RequiredIndexCount = QuadToTriangleVertexCount(XboxIndexCount);
		// For now, indicate the quad-to-triangles case using the otherwise
		// (due to alignment) always-zero least significant bit :
		LookupKey |= 1;
	}
	else if (bConvertTriFanToTriangleList) {
		RequiredIndexCount = FanToTriangleVertexCount(XboxIndexCount);
		LookupKey |= 1;
	}

	// Poor-mans eviction policy : when exceeding treshold, clear entire cache :
	if (g_IndexBufferCache.size() > INDEX_BUFFER_CACHE_SIZE) {
		g_IndexBufferCache.clear(); // Note : ConvertedIndexBuffer destructor will release any assigned pHostIndexBuffer
	}

	// Create a reference to the active buffer
	ConvertedIndexBuffer& CacheEntry = g_IndexBufferCache[LookupKey];

	// If the current index buffer size is too small, free it, so it'll get re-created
	bool bNeedRepopulation = CacheEntry.IndexCount < RequiredIndexCount;
	if (bNeedRepopulation) {
		if (CacheEntry.pHostIndexBuffer != nullptr)
			CacheEntry.pHostIndexBuffer->Release();

		CacheEntry = {};
	}

	// If we need to create an index buffer, do so.
	if (CacheEntry.pHostIndexBuffer == nullptr) {
		CacheEntry.pHostIndexBuffer = CxbxCreateIndexBuffer(RequiredIndexCount);
		if (!CacheEntry.pHostIndexBuffer)
			CxbxrAbort("CxbxUpdateActiveIndexBuffer: IndexBuffer Create Failed!");
	}

	// TODO : Speeds this up, perhaps by hashing less often, and/or by
	// doing two hashes : a small subset regularly, all data less frequently.
	uint64_t uiHash = ComputeHash(pXboxIndexData, XboxIndexCount * sizeof(INDEX16));

	// If the data needs updating, do so
	bNeedRepopulation |= (uiHash != CacheEntry.Hash);
	if (bNeedRepopulation)	{
		// Update the Index Count and the hash
		CacheEntry.IndexCount = RequiredIndexCount;
		CacheEntry.Hash = uiHash;

		// Update the host index buffer
		INDEX16* pHostIndexBufferData = CxbxLockIndexBuffer(CacheEntry.pHostIndexBuffer);
		if (pHostIndexBufferData == nullptr) {
			CxbxrAbort("CxbxUpdateActiveIndexBuffer: Could not lock index buffer!");
		}

		// Determine highest and lowest index in use :
		WalkIndexBuffer(CacheEntry.LowIndex, CacheEntry.HighIndex, pXboxIndexData, XboxIndexCount);
		if (bConvertQuadListToTriangleList) {
			// Note, that LowIndex and HighIndex won't change due to any quad-to-triangle conversion,
			// so it's less work to WalkIndexBuffer over the input instead of the converted index buffer.
			EmuLog(LOG_LEVEL::DEBUG, "CxbxUpdateActiveIndexBuffer: Converting quads to %d triangle indices (EMUFMT_INDEX16)", RequiredIndexCount);
			CxbxConvertQuadListToTriangleListIndices(pXboxIndexData, RequiredIndexCount, pHostIndexBufferData);
		} else if (bConvertTriFanToTriangleList) {
			EmuLog(LOG_LEVEL::DEBUG, "CxbxUpdateActiveIndexBuffer: Converting fan to %d triangle indices (EMUFMT_INDEX16)", RequiredIndexCount);
			CxbxConvertTriFanToTriangleListIndices(pXboxIndexData, XboxIndexCount, pHostIndexBufferData);
		} else {
			EmuLog(LOG_LEVEL::DEBUG, "CxbxUpdateActiveIndexBuffer: Copying %d indices (EMUFMT_INDEX16)", XboxIndexCount);
			memcpy(pHostIndexBufferData, pXboxIndexData, XboxIndexCount * sizeof(INDEX16));
		}

		CxbxUnlockIndexBuffer(CacheEntry.pHostIndexBuffer);
	}

	// Activate the new native index buffer :
	CxbxSetIndices(CacheEntry.pHostIndexBuffer);

	return CacheEntry;
}

void Direct3D_CreateDevice_Start
(
	const xbox::X_D3DPRESENT_PARAMETERS     *pPresentationParameters
)
{
   	CxbxVertexShaderSetFlags();

   	if (!XboxRenderStates.Init()) {
   	   	CxbxrAbort("Failed to init XboxRenderStates");
   	}

   	if (!XboxTextureStates.Init(&XboxRenderStates)) {
   	   	CxbxrAbort("Failed to init XboxTextureStates");
   	}

	SetXboxMultiSampleType(pPresentationParameters->MultiSampleType);

	// create default device *before* calling Xbox Direct3D_CreateDevice trampoline
	// to avoid hitting EMUPATCH'es that need a valid g_pD3DDevice

	if (g_pD3DDevice != nullptr) { // Check to make sure device is null, otherwise no need to create it
		return;
	}

	CreateDefaultD3D9Device(pPresentationParameters);
}

void Direct3D_CreateDevice_End
(
	const xbox::X_D3DPRESENT_PARAMETERS     *pPresentationParameters
)
{
#if 0 // Unused :
   	// Set g_Xbox_D3DDevice to point to the Xbox D3D Device
   	auto it = g_SymbolAddresses.find("D3D_g_pDevice");
   	if (it != g_SymbolAddresses.end()) {
   	   	g_Xbox_D3DDevice = (DWORD*)it->second;
   	}
   	else {
   	   	EmuLog(LOG_LEVEL::ERROR2, "D3D_g_pDevice was not found!");
   	}
#endif

   	UpdateHostBackBufferDesc();
   	SetAspectRatioScale(pPresentationParameters);

   	// If the Xbox version of CreateDevice didn't call SetRenderTarget, we must derive the default backbuffer ourselves
   	// This works because CreateDevice always sets the current render target to the Xbox Backbuffer
   	// In later XDKs, it does this inline rather than by calling D3DDevice_SetRenderTarget
   	// meaning our patch doesn't always get called in these cases.
   	// We fix the situation by calling the Xbox GetRenderTarget function, which immediately after CreateDevice
   	// WILL always return the Backbuffer!
   	// Test Case: Shin Megami Tensei: Nine
   	if (g_pXbox_BackBufferSurface == xbox::zeroptr && g_pXbox_DefaultDepthStencilSurface == xbox::zeroptr) {
   	   	// First, log the test case
   	   	LOG_TEST_CASE("Xbox CreateDevice did not call SetRenderTarget");
   	}

   	if (g_pXbox_BackBufferSurface == xbox::zeroptr) {
   	   	if (XB_TRMP(D3DDevice_GetRenderTarget)) {
   	   	   	XB_TRMP(D3DDevice_GetRenderTarget)(&g_pXbox_BackBufferSurface);
   	   	}
   	   	else if (XB_TRMP(D3DDevice_GetRenderTarget2)) {
   	   	   	g_pXbox_BackBufferSurface = XB_TRMP(D3DDevice_GetRenderTarget2)();
   	   	}

   	   	// At this point, g_pXbox_BackBufferSurface should now point to a valid render target
   	   	// if it still doesn't, we cannot continue without crashing at draw time
   	   	if (g_pXbox_BackBufferSurface == xbox::zeroptr) {
   	   	   	CxbxrAbort("Unable to determine default Xbox backbuffer");
   	   	}

   	   	// Set the backbuffer as the initial rendertarget
   	   	CxbxImpl_SetRenderTarget(g_pXbox_BackBufferSurface, xbox::zeroptr);
   	}

   	// Now do the same, but for the default depth stencil surface
   	if (g_pXbox_DefaultDepthStencilSurface == xbox::zeroptr) {
   	   	if (XB_TRMP(D3DDevice_GetDepthStencilSurface)) {
   	   	   	XB_TRMP(D3DDevice_GetDepthStencilSurface)(&g_pXbox_DefaultDepthStencilSurface);
   	   	}
   	   	else if (XB_TRMP(D3DDevice_GetDepthStencilSurface2)) {
   	   	   	g_pXbox_DefaultDepthStencilSurface = XB_TRMP(D3DDevice_GetDepthStencilSurface2)();
   	   	}

   	   	// At this point, g_pXbox_DefaultDepthStencilSurface should now point to a valid depth stencil
   	   	// If it doesn't, just log and carry on: Unlike RenderTarget, this situation is not fatal
   	   	if (g_pXbox_DefaultDepthStencilSurface == xbox::zeroptr) {
   	   	   	LOG_TEST_CASE("Unable to determine default Xbox depth stencil");
   	   	} else {
   	   	   	// Update only the depth stencil
   	   	   	CxbxImpl_SetRenderTarget(xbox::zeroptr, g_pXbox_DefaultDepthStencilSurface);
   	   	}
   	}
}

// Called by D3DDevice_DrawIndexedVertices and EmuExecutePushBufferRaw (twice)
void CxbxDrawIndexed(CxbxDrawContext &DrawContext)
{
	LOG_INIT // Allows use of DEBUG_D3DRESULT

	assert(DrawContext.dwStartVertex == 0);
	assert(DrawContext.pXboxIndexData != nullptr);
	assert(DrawContext.dwVertexCount > 0); // TODO : If this fails, make responsible callers do an early-exit

	bool bConvertQuadListToTriangleList = (DrawContext.XboxPrimitiveType == xbox::X_D3DPT_QUADLIST);
#ifdef CXBX_USE_D3D11
	bool bConvertTriFanToTriangleList = (DrawContext.XboxPrimitiveType == xbox::X_D3DPT_TRIANGLEFAN
		|| DrawContext.XboxPrimitiveType == xbox::X_D3DPT_POLYGON);

	// GPU index conversion path: dispatch CS to convert quad/fan indices to triangle list
	if (bConvertQuadListToTriangleList || bConvertTriFanToTriangleList) {
		// WalkIndexBuffer is still needed on CPU for LowIndex/HighIndex (vertex buffer range)
		INDEX16 LowIndex = 0, HighIndex = 0;
		WalkIndexBuffer(LowIndex, HighIndex, DrawContext.pXboxIndexData, DrawContext.dwVertexCount);
		DrawContext.LowIndex = LowIndex;
		DrawContext.HighIndex = HighIndex;

		UINT OutputIndexCount = bConvertQuadListToTriangleList ?
			QuadToTriangleVertexCount(DrawContext.dwVertexCount) :
			FanToTriangleVertexCount(DrawContext.dwVertexCount);

		int mode = bConvertQuadListToTriangleList ?
			(CxbxGetClockWiseWindingOrder() ? CXBX_INDEX_CONVERT_QUAD_CW : CXBX_INDEX_CONVERT_QUAD_CCW) :
			CXBX_INDEX_CONVERT_FAN;

		if (CxbxD3D11ConvertIndexBufferGPU(DrawContext.pXboxIndexData, DrawContext.dwVertexCount, OutputIndexCount, mode)) {
			VertexBufferConverter.Apply(&DrawContext);

			UINT primCount = bConvertQuadListToTriangleList ?
				DrawContext.dwHostPrimitiveCount * TRIANGLES_PER_QUAD :
				((DrawContext.dwVertexCount >= 3) ? DrawContext.dwVertexCount - 2 : 0);

			HRESULT hRet = CxbxDrawIndexedPrimitive(
				DrawContext.XboxPrimitiveType,
				/*IndexCount=*/OutputIndexCount,
				/*BaseVertexIndex=*/-(INT)LowIndex,
				/*StartIndex=*/0,
				/*MinIndex=*/LowIndex,
				/*NumVertices=*/(HighIndex - LowIndex) + 1,
				primCount);
			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitive(CS converted)");

			g_dwPrimPerFrame += primCount;
			return;
		}
		// CS failed — fall through to CPU conversion path
	}
#else
	bool bConvertTriFanToTriangleList = false;
#endif
	ConvertedIndexBuffer& CacheEntry = CxbxUpdateActiveIndexBuffer(DrawContext.pXboxIndexData, DrawContext.dwVertexCount, bConvertQuadListToTriangleList, bConvertTriFanToTriangleList);
	// Note : CxbxUpdateActiveIndexBuffer calls SetIndices

	// Set LowIndex and HighIndex *before* VerticesInBuffer gets derived
	DrawContext.LowIndex = CacheEntry.LowIndex;
	DrawContext.HighIndex = CacheEntry.HighIndex;

	VertexBufferConverter.Apply(&DrawContext); // Sets dwHostPrimitiveCount

	INT BaseVertexIndex = DrawContext.dwBaseVertexIndex;
	UINT primCount = DrawContext.dwHostPrimitiveCount;
	bool bConvertedPrimitive = bConvertQuadListToTriangleList || bConvertTriFanToTriangleList;
	if (bConvertQuadListToTriangleList) {
		if (DrawContext.dwVertexCount == 4)
			LOG_TEST_CASE("X_D3DPT_QUADLIST (single quad)"); // breakpoint location
		else
			LOG_TEST_CASE("X_D3DPT_QUADLIST");

		if (BaseVertexIndex > 0)
			LOG_TEST_CASE("X_D3DPT_QUADLIST (BaseVertexIndex > 0)");

		// Convert draw arguments from quads to triangles :
		// Note : BaseVertexIndex must NOT be mapped through QuadToTriangleVertexCount(BaseVertexIndex)!
		primCount *= TRIANGLES_PER_QUAD;
	}
	else if (bConvertTriFanToTriangleList) {
		primCount = (DrawContext.dwVertexCount >= 3) ? DrawContext.dwVertexCount - 2 : 0;
	}

	UINT indexCount = bConvertedPrimitive ? primCount * 3 : DrawContext.dwVertexCount;

	// See https://docs.microsoft.com/en-us/windows/win32/direct3d9/rendering-from-vertex-and-index-buffers
	// for an explanation on the function of the BaseVertexIndex, MinVertexIndex, NumVertices and StartIndex arguments.
	HRESULT hRet = CxbxDrawIndexedPrimitive(
		DrawContext.XboxPrimitiveType,
		/*IndexCount=*/indexCount,
		/*BaseVertexIndex=*/-(INT)CacheEntry.LowIndex, // Base vertex index has been accounted for in the stream conversion, now we need to "un-offset" the index buffer
		/*StartIndex=*/0,
		/*MinIndex=*/CacheEntry.LowIndex,
		/*NumVertices=*/(CacheEntry.HighIndex - CacheEntry.LowIndex) + 1,
		primCount);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitive");

	g_dwPrimPerFrame += primCount;
	if (DrawContext.XboxPrimitiveType == xbox::X_D3DPT_LINELOOP) {
		// Close line-loops using a final single line, drawn from the end to the start vertex
		if (BaseVertexIndex == 0) {
			LOG_TEST_CASE("X_D3DPT_LINELOOP");
		} else {
			LOG_TEST_CASE("X_D3DPT_LINELOOP (BaseVertexIndex > 0)");
			// Note : This is to find test-cases for the BaseVertexIndex addition below:
		}
		// Read the end and start index from the supplied index data
		INDEX16 LowIndex = BaseVertexIndex + DrawContext.pXboxIndexData[0];
		INDEX16 HighIndex = BaseVertexIndex + DrawContext.pXboxIndexData[DrawContext.dwHostPrimitiveCount];
		// If needed, swap so highest index is higher than lowest (duh)
		if (HighIndex < LowIndex) {
			std::swap(HighIndex, LowIndex);
		}

		// Draw the closing line using a helper function (which will SetIndices)
		CxbxDrawIndexedClosingLine(LowIndex, HighIndex);
		// NOTE : We don't restore the previously active index buffer
	}
}

// TODO : Move to own file
// Drawing function specifically for rendering Xbox draw calls supplying a 'User Pointer'.
// Called by D3DDevice_DrawVerticesUP, EmuExecutePushBufferRaw and CxbxImpl_End
void CxbxDrawPrimitiveUP(CxbxDrawContext &DrawContext)
{
	LOG_INIT // Allows use of DEBUG_D3DRESULT

	assert(DrawContext.dwStartVertex == 0);
	assert(DrawContext.pXboxVertexStreamZeroData != xbox::zeroptr);
	assert(DrawContext.uiXboxVertexStreamZeroStride > 0);
	assert(DrawContext.dwBaseVertexIndex == 0); // No IndexBase under Draw*UP

	VertexBufferConverter.Apply(&DrawContext);

#ifdef CXBX_USE_D3D11
	// D3D11 has no DrawPrimitiveUP - we need to create a temporary vertex buffer
	UINT vertexDataSize = DrawContext.dwVertexCount * DrawContext.uiHostVertexStreamZeroStride;

	// Reusable dynamic vertex buffer for UP draws
	static CxbxDynBuffer s_UpVB = { nullptr, 0, D3D11_BIND_VERTEX_BUFFER };
	ID3D11Buffer* pVB = s_UpVB.Update(DrawContext.pHostVertexStreamZeroData, vertexDataSize);

	if (pVB != nullptr) {
		UINT stride = DrawContext.uiHostVertexStreamZeroStride;
		UINT offset = 0;
		g_pD3DDeviceContext->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);

		if (DrawContext.XboxPrimitiveType == xbox::X_D3DPT_QUADLIST) {
			// Draw quadlists using a single 'quad-to-triangle mapping' index buffer :
			UINT PrimitiveCount = DrawContext.dwHostPrimitiveCount * TRIANGLES_PER_QUAD;
			UINT IndexCount = PrimitiveCount * 3;
			int mode = CxbxGetClockWiseWindingOrder() ? CXBX_INDEX_CONVERT_QUAD_CW : CXBX_INDEX_CONVERT_QUAD_CCW;

			if (!CxbxD3D11ConvertIndexBufferGPU(nullptr, DrawContext.dwVertexCount, IndexCount, mode)) {
				// Fallback: CPU conversion
				INDEX16 *pIndexData = CxbxAssureQuadListIndexData(DrawContext.dwVertexCount);
				static CxbxDynBuffer s_UpIB = { nullptr, 0, D3D11_BIND_INDEX_BUFFER };
				ID3D11Buffer* pIB = s_UpIB.Update(pIndexData, IndexCount * sizeof(INDEX16));
				if (!pIB) { g_dwPrimPerFrame += PrimitiveCount; return; }
				g_pD3DDeviceContext->IASetIndexBuffer(pIB, DXGI_FORMAT_R16_UINT, 0);
			}
			g_pD3DDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			g_pD3DDeviceContext->DrawIndexed(IndexCount, 0, 0);

			g_dwPrimPerFrame += PrimitiveCount;
		} else if (DrawContext.XboxPrimitiveType == xbox::X_D3DPT_TRIANGLEFAN
			|| DrawContext.XboxPrimitiveType == xbox::X_D3DPT_POLYGON) {
			// D3D11 doesn't support triangle fan topology — convert to indexed triangle list
			UINT NrOfTriangleIndices = FanToTriangleVertexCount(DrawContext.dwVertexCount);
			if (NrOfTriangleIndices > 0) {
				UINT PrimitiveCount = DrawContext.dwVertexCount - 2;

				if (!CxbxD3D11ConvertIndexBufferGPU(nullptr, DrawContext.dwVertexCount, NrOfTriangleIndices, CXBX_INDEX_CONVERT_FAN)) {
					// Fallback: CPU conversion
					INDEX16* pFanIndices = CxbxCreateTriFanToTriangleListIndexData(nullptr, DrawContext.dwVertexCount);
					static CxbxDynBuffer s_FanUpIB = { nullptr, 0, D3D11_BIND_INDEX_BUFFER };
					ID3D11Buffer* pIB = s_FanUpIB.Update(pFanIndices, NrOfTriangleIndices * sizeof(INDEX16));
					free(pFanIndices);
					if (!pIB) { g_dwPrimPerFrame += PrimitiveCount; return; }
					g_pD3DDeviceContext->IASetIndexBuffer(pIB, DXGI_FORMAT_R16_UINT, 0);
				}
				g_pD3DDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				g_pD3DDeviceContext->DrawIndexed(NrOfTriangleIndices, 0, 0);

				g_dwPrimPerFrame += PrimitiveCount;
			}
		} else {
			CxbxBindThickLineGS(DrawContext.XboxPrimitiveType);
			g_pD3DDeviceContext->IASetPrimitiveTopology(EmuXB2PC_D3D11PrimitiveTopology(DrawContext.XboxPrimitiveType));
			g_pD3DDeviceContext->Draw(DrawContext.dwVertexCount, 0);
			CxbxUnbindThickLineGS(DrawContext.XboxPrimitiveType);

			g_dwPrimPerFrame += DrawContext.dwHostPrimitiveCount;
			if (DrawContext.XboxPrimitiveType == xbox::X_D3DPT_LINELOOP) {
				CxbxDrawIndexedClosingLineUP(
					(INDEX16)0,
					(INDEX16)DrawContext.dwHostPrimitiveCount,
					DrawContext.pHostVertexStreamZeroData,
					DrawContext.uiHostVertexStreamZeroStride
				);
			}
		}
	}
#else
	if (DrawContext.XboxPrimitiveType == xbox::X_D3DPT_QUADLIST) {
		// LOG_TEST_CASE("X_D3DPT_QUADLIST"); // test-case : X-Marbles and XDK Sample PlayField
		// Draw quadlists using a single 'quad-to-triangle mapping' index buffer :
		INDEX16 *pIndexData = CxbxAssureQuadListIndexData(DrawContext.dwVertexCount);
		// Convert quad vertex-count to triangle vertex count :
		UINT PrimitiveCount = DrawContext.dwHostPrimitiveCount * TRIANGLES_PER_QUAD;

		// Draw indexed triangles instead of quads
		HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitiveUP(
			/*PrimitiveType=*/D3DPT_TRIANGLELIST,
			/*MinVertexIndex=*/0, // Always 0 for converted quadlist data
			/*NumVertices=*/DrawContext.dwVertexCount,
			PrimitiveCount,
			pIndexData,
			/*IndexDataFormat=*/EMUFMT_INDEX16,
			DrawContext.pHostVertexStreamZeroData,
			DrawContext.uiHostVertexStreamZeroStride
		);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitieUP(X_D3DPT_QUADLIST)");

		g_dwPrimPerFrame += PrimitiveCount;
	}
	else {
		// Primitives other than X_D3DPT_QUADLIST can be drawn using one DrawPrimitiveUP call :
		HRESULT hRet = g_pD3DDevice->DrawPrimitiveUP(
			EmuXB2PC_D3DPrimitiveType(DrawContext.XboxPrimitiveType),
			DrawContext.dwHostPrimitiveCount,
			DrawContext.pHostVertexStreamZeroData,
			DrawContext.uiHostVertexStreamZeroStride
		);
		DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawPrimitiveUP");

		g_dwPrimPerFrame += DrawContext.dwHostPrimitiveCount;
		if (DrawContext.XboxPrimitiveType == xbox::X_D3DPT_LINELOOP) {
			// test-case : XDK samples reaching this case : DebugKeyboard, Gamepad, Tiling, ShadowBuffer
			// Close line-loops using a final single line, drawn from the end to the start vertex :
			CxbxDrawIndexedClosingLineUP(
				(INDEX16)0, // LowIndex
				(INDEX16)DrawContext.dwHostPrimitiveCount, // HighIndex,
				DrawContext.pHostVertexStreamZeroData,
				DrawContext.uiHostVertexStreamZeroStride
			);
		}
	}
#endif
}

IDirect3DBaseTexture* CxbxConvertXboxSurfaceToHostTexture(xbox::X_D3DBaseTexture* pBaseTexture)
{
	LOG_INIT; // Allows use of DEBUG_D3DRESULT

	IDirect3DSurface* pHostSurface = GetHostSurface(pBaseTexture);
	if (!pHostSurface) {
   	   	LOG_TEST_CASE("Failed to get host surface");
		return nullptr;
	}

#ifdef CXBX_USE_D3D11
	// For D3D11, the surface IS already the texture (ID3D11Texture2D)
	// Just add a reference and return it
	pHostSurface->AddRef();
	return pHostSurface;
#else
	IDirect3DBaseTexture* pNewHostBaseTexture = nullptr;
	auto hRet = pHostSurface->GetContainer(IID_PPV_ARGS(&pNewHostBaseTexture)); // TODO : pNewHostBaseTexture.GetAddressOf() ?
   	DEBUG_D3DRESULT(hRet, "pHostSurface->GetContainer");

	if (FAILED(hRet)) {
		LOG_TEST_CASE("Failed to get Texture from Surface");
		return nullptr;
	}

	return pNewHostBaseTexture;
#endif
}

