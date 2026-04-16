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

// ******************************************************************
// * Triangle fan to triangle list conversion
// ******************************************************************

// Convert a triangle fan (N vertices) to triangle list indices.
// Fan vertex 0 is the hub; for each i in [1..N-2], triangle = (0, i, i+1).
// When pFanIndexData != null, reads original fan indices; otherwise generates sequential.
// Output: (N-2)*3 triangle indices written to pTriangleIndexData.
void CxbxConvertTriFanToTriangleListIndices(
	INDEX16* pFanIndexData,
	unsigned uNrOfFanVertices,
	INDEX16* pTriangleIndexData)
{
	assert(uNrOfFanVertices >= 3);
	assert(pTriangleIndexData);

	unsigned out = 0;
	for (unsigned i = 1; i + 1 < uNrOfFanVertices; i++) {
		pTriangleIndexData[out++] = pFanIndexData ? pFanIndexData[0]     : 0;
		pTriangleIndexData[out++] = pFanIndexData ? pFanIndexData[i]     : (INDEX16)i;
		pTriangleIndexData[out++] = pFanIndexData ? pFanIndexData[i + 1] : (INDEX16)(i + 1);
	}
}

UINT FanToTriangleVertexCount(UINT NrOfFanVertices)
{
	return (NrOfFanVertices >= 3) ? (NrOfFanVertices - 2) * VERTICES_PER_TRIANGLE : 0;
}

INDEX16* CxbxCreateTriFanToTriangleListIndexData(INDEX16* pFanIndexData, unsigned FanVertexCount)
{
	UINT NrOfTriangleIndices = FanToTriangleVertexCount(FanVertexCount);
	INDEX16* pBuffer = (INDEX16*)malloc(NrOfTriangleIndices * sizeof(INDEX16));
	CxbxConvertTriFanToTriangleListIndices(pFanIndexData, FanVertexCount, pBuffer);
	return pBuffer;
}

// ******************************************************************
// * Quad list to triangle list conversion
// ******************************************************************

bool bUseClockWiseWindingOrder = true; // TODO : Should this be fetched from X_D3DRS_FRONTFACE (or X_D3DRS_CULLMODE)?

UINT QuadToTriangleVertexCount(UINT NrOfQuadVertices)
{
	return (NrOfQuadVertices * VERTICES_PER_TRIANGLE * TRIANGLES_PER_QUAD) / VERTICES_PER_QUAD;
}

// This function convertes quad to triangle indices.
// When pXboxQuadIndexData is set, original quad indices are read from this buffer
// (this use-case is for when an indexed quad draw is to be emulated).
// When pXboxQuadIndexData is null, quad-emulating indices are generated
// (this use-case is for when a non-indexed quad draw is to be emulated).
// The number of indices to generate is specified through uNrOfTriangleIndices.
// Resulting triangle indices are written to pTriangleIndexData, which must
// be pre-allocated to fit the output data.
// (Note, this function is marked 'constexpr' to allow the compiler to optimize
// the case when pXboxQuadIndexData is null)
void CxbxConvertQuadListToTriangleListIndices(
	INDEX16* pXboxQuadIndexData,
	unsigned uNrOfTriangleIndices,
	INDEX16* pTriangleIndexData)
{
	assert(uNrOfTriangleIndices > 0);
	assert(pTriangleIndexData);

	unsigned i = 0;
	unsigned j = 0;
	while (i + (VERTICES_PER_TRIANGLE * TRIANGLES_PER_QUAD) <= uNrOfTriangleIndices) {
		if (bUseClockWiseWindingOrder) {
			// ABCD becomes ABD+BCD (split along the B-D diagonal, matching NV2A hardware)
			pTriangleIndexData[i + 0] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 0] : j + 0; // A
			pTriangleIndexData[i + 1] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 1] : j + 1; // B
			pTriangleIndexData[i + 2] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 3] : j + 3; // D
			i += VERTICES_PER_TRIANGLE;
			pTriangleIndexData[i + 0] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 1] : j + 1; // B
			pTriangleIndexData[i + 1] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 2] : j + 2; // C
			pTriangleIndexData[i + 2] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 3] : j + 3; // D
			i += VERTICES_PER_TRIANGLE;
		} else {
			// ABCD becomes ADB+BDC (split along the B-D diagonal, matching NV2A hardware)
			pTriangleIndexData[i + 0] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 0] : j + 0; // A
			pTriangleIndexData[i + 1] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 3] : j + 3; // D
			pTriangleIndexData[i + 2] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 1] : j + 1; // B
			i += VERTICES_PER_TRIANGLE;
			pTriangleIndexData[i + 0] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 1] : j + 1; // B
			pTriangleIndexData[i + 1] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 3] : j + 3; // D
			pTriangleIndexData[i + 2] = pXboxQuadIndexData ? pXboxQuadIndexData[j + 2] : j + 2; // C
			i += VERTICES_PER_TRIANGLE;
		}

		// Next quad, please :
		j += VERTICES_PER_QUAD;
	}
}

// Called from EMUPATCH(D3DDevice_DrawIndexedVerticesUP) when PrimitiveType == X_D3DPT_QUADLIST.
// This API receives the number of vertices to draw (VertexCount), the index data that references
// vertices and a single stream of vertex data. The number of vertices to draw indicates the number
// of indices that are going to be fetched. The vertex data is referenced up to the highest index
// number present in the index data.
// To emulate drawing indexed quads, g_pD3DDevice->DrawIndexedPrimitiveUP is called on host,
// whereby the quad indices are converted to triangle indices. This implies for every four
// quad indices, we have to generate (two times three is) six triangle indices. (Note, that
// vertex data undergoes it's own Xbox-to-host conversion, independent from these indices.)
INDEX16* CxbxCreateQuadListToTriangleListIndexData(INDEX16* pXboxQuadIndexData, unsigned QuadVertexCount)
{
	UINT NrOfTriangleIndices = QuadToTriangleVertexCount(QuadVertexCount);
	INDEX16* pQuadToTriangleIndexBuffer = (INDEX16*)malloc(NrOfTriangleIndices * sizeof(INDEX16));
	CxbxConvertQuadListToTriangleListIndices(pXboxQuadIndexData, NrOfTriangleIndices, pQuadToTriangleIndexBuffer);
	return pQuadToTriangleIndexBuffer;
}

// ******************************************************************
// * Quad list index data cache
// ******************************************************************

static INDEX16 *g_pQuadToTriangleIndexData = nullptr;
static size_t g_QuadToTriangleIndexData_Size = 0; // = NrOfQuadIndices

constexpr unsigned int IndicesPerPage = PAGE_SIZE / sizeof(INDEX16);
constexpr unsigned int InputQuadsPerPage = ((IndicesPerPage * VERTICES_PER_QUAD) / VERTICES_PER_TRIANGLE) / TRIANGLES_PER_QUAD;

// Called by CxbxDrawPrimitiveUP (indirectly by D3DDevice_DrawVerticesUP,
// EmuExecutePushBufferRaw and CxbxImpl_End) when PrimitiveType == X_D3DPT_QUADLIST.
// Emulated by calling g_pD3DDevice->DrawIndexedPrimitiveUP with index data that maps
// quads to triangles. This function creates the index buffer that is needed for this;
// For every quad that must be drawn, we generate indices for two triangles.
// Note, that the resulting index data can be re-used for later comparable draw calls
// and only needs to grow when  current length doesn't suffices for a larger draw.
INDEX16 *CxbxAssureQuadListIndexData(UINT NrOfQuadIndices)
{
	if (g_QuadToTriangleIndexData_Size < NrOfQuadIndices)
	{
		g_QuadToTriangleIndexData_Size = RoundUp(NrOfQuadIndices, InputQuadsPerPage);
		UINT NrOfTriangleIndices = QuadToTriangleVertexCount(g_QuadToTriangleIndexData_Size);
		if (g_pQuadToTriangleIndexData != nullptr) {
			free(g_pQuadToTriangleIndexData);
		}

		g_pQuadToTriangleIndexData = (INDEX16 *)malloc(NrOfTriangleIndices * sizeof(INDEX16));
		CxbxConvertQuadListToTriangleListIndices(nullptr, NrOfTriangleIndices, g_pQuadToTriangleIndexData);
	}

	return g_pQuadToTriangleIndexData;
}

void CxbxReleaseQuadListToTriangleListIndexData(void* pHostIndexData)
{
	free(pHostIndexData);
}

// ******************************************************************
// * Quad-to-triangle D3D index buffer management
// ******************************************************************

static IDirect3DIndexBuffer *g_pQuadToTriangleHostIndexBuffer = nullptr;
static size_t g_QuadToTriangleHostIndexBuffer_Size = 0; // = NrOfQuadIndices

// Makes a D3D IndexBuffer active that contains quadlist-to-trianglelist indices.
// Uses CxbxAssureQuadListIndexData to populate the index buffer with.
// Note, that the resulting index buffer can be re-used for later comparable draw calls
// and only needs to grow when current length doesn't suffice for a larger draw.
void CxbxAssureQuadListD3DIndexBuffer(UINT NrOfQuadIndices)
{
	LOG_INIT // Allows use of DEBUG_D3DRESULT

	if (g_QuadToTriangleHostIndexBuffer_Size < NrOfQuadIndices)
	{
		// Round the number of indices up so we'll allocate whole pages
		g_QuadToTriangleHostIndexBuffer_Size = RoundUp(NrOfQuadIndices, InputQuadsPerPage);
		UINT NrOfTriangleIndices = QuadToTriangleVertexCount(g_QuadToTriangleHostIndexBuffer_Size); // 4 > 6

		// Create a new native index buffer of the above determined size :
		if (g_pQuadToTriangleHostIndexBuffer != nullptr) {
			g_pQuadToTriangleHostIndexBuffer->Release(); // test-case : XDK PointSprites
			g_pQuadToTriangleHostIndexBuffer = nullptr;
		}

		// Create a new native index buffer of the above determined size :
		g_pQuadToTriangleHostIndexBuffer = CxbxCreateIndexBuffer(NrOfTriangleIndices);
		if (g_pQuadToTriangleHostIndexBuffer == nullptr)
			CxbxrAbort("CxbxAssureQuadListD3DIndexBuffer : IndexBuffer Create Failed!");

		// Put quadlist-to-triangle-list index mappings into this buffer :
		INDEX16* pHostIndexBufferData = CxbxLockIndexBuffer(g_pQuadToTriangleHostIndexBuffer);
		if (pHostIndexBufferData == nullptr)
			CxbxrAbort("CxbxAssureQuadListD3DIndexBuffer : Could not lock index buffer!");

		memcpy(pHostIndexBufferData, CxbxAssureQuadListIndexData(NrOfQuadIndices), NrOfTriangleIndices * sizeof(INDEX16));

		CxbxUnlockIndexBuffer(g_pQuadToTriangleHostIndexBuffer);
	}

	// Activate the new native index buffer :
	CxbxSetIndices(g_pQuadToTriangleHostIndexBuffer);
}

// ******************************************************************
// * Line loop closing-line helpers
// ******************************************************************

static IDirect3DIndexBuffer *g_pClosingLineLoopHostIndexBuffer = nullptr;

// Calls SetIndices with a separate index-buffer, that's populated with the supplied indices.
void CxbxDrawIndexedClosingLine(INDEX16 LowIndex, INDEX16 HighIndex)
{
	LOG_INIT // Allows use of DEBUG_D3DRESULT

	HRESULT hRet;

	if (g_pClosingLineLoopHostIndexBuffer == nullptr) {
		g_pClosingLineLoopHostIndexBuffer = CxbxCreateIndexBuffer(VERTICES_PER_LINE);
		if (g_pClosingLineLoopHostIndexBuffer == nullptr)
			CxbxrAbort("Unable to create g_pClosingLineLoopHostIndexBuffer for D3DPT_LINELOOP emulation");
	}

	INDEX16 *pCxbxClosingLineLoopIndexBufferData = CxbxLockIndexBuffer(g_pClosingLineLoopHostIndexBuffer);

	// Set the indices for the two VERTICES_PER_LINE :
	pCxbxClosingLineLoopIndexBufferData[0] = LowIndex;
	pCxbxClosingLineLoopIndexBufferData[1] = HighIndex;

	CxbxUnlockIndexBuffer(g_pClosingLineLoopHostIndexBuffer);

	CxbxSetIndices(g_pClosingLineLoopHostIndexBuffer);

	hRet = CxbxDrawIndexedPrimitive(
		/*XboxPrimitiveType=*/(xbox::X_D3DPRIMITIVETYPE)xbox::X_D3DPT_LINELIST,
		/*IndexCount=*/VERTICES_PER_LINE,
		/*BaseVertexIndex=*/0, // Note : Callers must apply BaseVertexIndex to the LowIndex and HighIndex argument values
		/*StartIndex=*/0,
		/*MinIndex=*/LowIndex,
		/*NumVertices=*/VERTICES_PER_LINE,
		/*PrimCount=*/1
	);
	DEBUG_D3DRESULT(hRet, "DrawIndexedPrimitive(CxbxDrawIndexedClosingLine)");

	g_dwPrimPerFrame++;
}

void CxbxDrawIndexedClosingLineUP(INDEX16 LowIndex, INDEX16 HighIndex, void *pHostVertexStreamZeroData, UINT uiHostVertexStreamZeroStride)
{
	LOG_INIT // Allows use of DEBUG_D3DRESULT

#if 0
	// Since we can use pHostVertexStreamZeroData here, we can close the line simpler than
	// via CxbxDrawIndexedClosingLine, by drawing two indices via DrawIndexedPrimitiveUP.
	// (This is simpler because we use just indices and don't need to copy the vertices.)
	INDEX16 CxbxClosingLineIndices[2] = { LowIndex, HighIndex };

	HRESULT hRet = g_pD3DDevice->DrawIndexedPrimitiveUP(
		/*PrimitiveType=*/D3DPT_LINELIST,
		/*MinVertexIndex=*/LowIndex,
		/*NumVertices=*/(HighIndex - LowIndex) + 1,
		/*PrimitiveCount=*/1,
		/*pIndexData=*/CxbxClosingLineIndices,
		/*IndexDataFormat=*/EMUFMT_INDEX16,
		pHostVertexStreamZeroData,
		uiHostVertexStreamZeroStride
	);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawIndexedPrimitiveUP(CxbxDrawIndexedClosingLineUP)");
#else // TODO : If NumVertices is high, performance might suffer - drawing a copy of the two vertices could be faster
	// Since we can use pHostVertexStreamZeroData here, we can close the line simpler than
	// via CxbxDrawIndexedClosingLine, by drawing two vertices via DrawPrimitiveUP.
	// (This is simpler because we just copy the vertices, and don't need a separate index buffer.)
	uint8_t VertexData[512]; assert(512 >= 2 * uiHostVertexStreamZeroStride);
	uint8_t *FirstVertex = (uint8_t *)pHostVertexStreamZeroData + (LowIndex * uiHostVertexStreamZeroStride);
	uint8_t *SecondVertex = (uint8_t *)pHostVertexStreamZeroData + (HighIndex * uiHostVertexStreamZeroStride);

	memcpy(VertexData, FirstVertex, uiHostVertexStreamZeroStride);
	memcpy(VertexData + uiHostVertexStreamZeroStride, SecondVertex, uiHostVertexStreamZeroStride);

#ifdef CXBX_USE_D3D11
	static CxbxDynBuffer s_ClosingLineVB = { nullptr, 0, D3D11_BIND_VERTEX_BUFFER };
	UINT dataSize = 2 * uiHostVertexStreamZeroStride;
	ID3D11Buffer* pVB = s_ClosingLineVB.Update(VertexData, dataSize);
	if (pVB != nullptr) {
		UINT stride = uiHostVertexStreamZeroStride;
		UINT offset = 0;
		g_pD3DDeviceContext->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
		g_pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		g_pD3DDeviceContext->Draw(2, 0);
	}
#else
	HRESULT hRet = g_pD3DDevice->DrawPrimitiveUP(
		/*PrimitiveType=*/D3DPT_LINELIST,
		/*PrimitiveCount=*/1,
		/*pVertexStreamZeroData=*/VertexData,
		uiHostVertexStreamZeroStride
	);
	DEBUG_D3DRESULT(hRet, "g_pD3DDevice->DrawPrimitiveUP(CxbxDrawIndexedClosingLineUP)");
#endif
#endif

	g_dwPrimPerFrame++;
}
