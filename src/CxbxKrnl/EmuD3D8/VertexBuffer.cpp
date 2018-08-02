// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuD3D8->VertexBuffer.cpp
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
// *  (c) 2002-2004 Aaron Robinson <caustik@caustik.com>
// *                Kingofc <kingofc@freenet.de>
// *
// *  All rights reserved
// *
// ******************************************************************
#define _XBOXKRNL_DEFEXTRN_

#include "CxbxKrnl/VMManager.h"
#include "CxbxKrnl/xxhash32.h" // For XXHash32::hash()
#include "CxbxKrnl/Emu.h"
#include "CxbxKrnl/EmuXTL.h"
#include "CxbxKrnl/ResourceTracker.h"

// TODO: Find somewhere to put this that doesn't conflict with XTL::
extern void EmuUpdateActiveTextureStages();

#include <ctime>
#include <unordered_map>
#include <chrono>
#include <algorithm>

#define HASH_SEED 0

#define MAX_STREAM_NOT_USED_TIME (2 * CLOCKS_PER_SEC) // TODO: Trim the not used time

// Inline vertex buffer emulation
extern XTL::X_D3DPRIMITIVETYPE XTL::g_InlineVertexBuffer_PrimitiveType = XTL::X_D3DPT_INVALID;
extern DWORD                   XTL::g_InlineVertexBuffer_FVF = 0;
extern struct XTL::_D3DIVB    *XTL::g_InlineVertexBuffer_Table = nullptr;
extern UINT                    XTL::g_InlineVertexBuffer_TableLength = 0;
extern UINT                    XTL::g_InlineVertexBuffer_TableOffset = 0;

FLOAT *g_InlineVertexBuffer_pData = nullptr;
UINT   g_InlineVertexBuffer_DataSize = 0;

extern XTL::X_D3DVertexBuffer      *g_pVertexBuffer = NULL;

extern DWORD				XTL::g_dwPrimPerFrame = 0;
extern XTL::X_D3DVertexBuffer*g_D3DStreams[16];
extern UINT g_D3DStreamStrides[16];
void *GetDataFromXboxResource(XTL::X_D3DResource *pXboxResource);

typedef struct {
	XTL::IDirect3DVertexBuffer* pHostVertexBuffer;
	size_t uiSize;
	std::chrono::time_point<std::chrono::high_resolution_clock> lastUsed;
} cached_vertex_buffer_object;

std::unordered_map<DWORD, cached_vertex_buffer_object> g_HostVertexBuffers;

// This caches Vertex Buffer Objects, but not the containing data
// This prevents unnecessary allocation and releasing of Vertex Buffers when
// we can use an existing just fine. This gives a (slight) performance boost
// Returns true if the existing vertex buffer was trashed/made invalid
bool GetCachedVertexBufferObject(DWORD pXboxDataPtr, DWORD size, XTL::IDirect3DVertexBuffer** pVertexBuffer)
{
	// TODO: If the vertex buffer object cache becomes too large, 
	// free the least recently used vertex buffers

	auto it = g_HostVertexBuffers.find(pXboxDataPtr);
	if (it == g_HostVertexBuffers.end()) {
		// Create new vertex buffer and return
		cached_vertex_buffer_object newBuffer;
		newBuffer.uiSize = size;
		newBuffer.lastUsed = std::chrono::high_resolution_clock::now();

		HRESULT hRet = g_pD3DDevice->CreateVertexBuffer(
			size,
			D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
			0,
			XTL::D3DPOOL_DEFAULT,
			&newBuffer.pHostVertexBuffer,
			nullptr
		);
		if (FAILED(hRet)) {
			CxbxKrnlCleanup("Failed to create vertex buffer");
		}
		
		g_HostVertexBuffers[pXboxDataPtr] = newBuffer;

		*pVertexBuffer = newBuffer.pHostVertexBuffer;
		return false;
	}

	auto buffer = &it->second;
	buffer->lastUsed = std::chrono::high_resolution_clock::now();

	// Return the existing vertex buffer, if possible
	if (size <= buffer->uiSize) {
		*pVertexBuffer = buffer->pHostVertexBuffer;
		return false;
	}

	// If execution reached here, we need to release and re-create the vertex buffer..
	buffer->pHostVertexBuffer->Release();
	buffer->uiSize = size;
	HRESULT hRet = g_pD3DDevice->CreateVertexBuffer(
		size,
		D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
		0,
		XTL::D3DPOOL_DEFAULT,
		&buffer->pHostVertexBuffer,
		nullptr
	);
	if (FAILED(hRet)) {
		CxbxKrnlCleanup("Failed to create vertex buffer");
	}

	*pVertexBuffer = buffer->pHostVertexBuffer;
	return true;
}

void ActivatePatchedStream
(
	XTL::CxbxDrawContext *pDrawContext,
	UINT uiStream,
	XTL::CxbxPatchedStream *pPatchedStream,
	bool bRelease
)
{
	//LOG_INIT // Allows use of DEBUG_D3DRESULT

	// Use the cached stream values on the host
	if (pPatchedStream->bCacheIsStreamZeroDrawUP) {
		// Set the UserPointer variables in the drawing context
		pDrawContext->pHostVertexStreamZeroData = pPatchedStream->pCachedHostVertexStreamZeroData;
		pDrawContext->uiHostVertexStreamZeroStride = pPatchedStream->uiCachedHostVertexStride;
	}
	else {
		HRESULT hRet = g_pD3DDevice->SetStreamSource(
			uiStream, 
			pPatchedStream->pCachedHostVertexBuffer, 
			0, // OffsetInBytes
			pPatchedStream->uiCachedHostVertexStride);
		//DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetStreamSource");
		if (FAILED(hRet)) {
			CxbxKrnlCleanup("Failed to set the type patched buffer as the new stream source!\n");
			// TODO : Cartoon hits the above case when the vertex cache size is 0.
		}

		// TODO : The following doesn't fix that - find our why and fix it for real
		if (bRelease) {
			// Always release to prevent leaks when it wasn't read from cache:
			pPatchedStream->pCachedHostVertexBuffer->Release();
			// NOTE : Even this doesn't prevent Cartoon breaking : g_pD3DDevice->ResourceManagerDiscardBytes(0);
		}
	}
}

void ReleasePatchedStream(XTL::CxbxPatchedStream *pPatchedStream)
{
	if (pPatchedStream->bCachedHostVertexStreamZeroDataIsAllocated) {
		free(pPatchedStream->pCachedHostVertexStreamZeroData);
		pPatchedStream->bCachedHostVertexStreamZeroDataIsAllocated = false;
	}
	pPatchedStream->pCachedHostVertexStreamZeroData = nullptr;
	pPatchedStream->pCachedHostVertexBuffer = nullptr; // g_HostVertexBuffers owns these nowadays
}

XTL::CxbxVertexBufferConverter::CxbxVertexBufferConverter()
{
    this->m_uiNbrStreams = 0;
    ZeroMemory(this->m_PatchedStreams, sizeof(CxbxPatchedStream) * MAX_NBR_STREAMS);
    this->m_bAllocatedStreamZeroData = false;
    this->m_pNewVertexStreamZeroData = NULL;
    this->m_pVertexShaderInfo = NULL;
}

XTL::CxbxVertexBufferConverter::~CxbxVertexBufferConverter()
{
	for (int i = 0; i < MAX_NBR_STREAMS; i++) {
		ReleasePatchedStream(&m_PatchedStreams[i]);
	}
}

size_t GetVerticesInBuffer(DWORD dwOffset, DWORD dwVertexCount, PWORD pIndexData, DWORD dwIndexBase)
{	
	// If we are drawing from an offset, we know that the vertex count must have offset vertices
	// before the first drawn vertices
	dwVertexCount += dwOffset;
	if (pIndexData == nullptr) {
		return dwVertexCount;
	}

	// We are an indexed draw, so we have to parse the index buffer
	// The highest index we see can be used to determine the vertex buffer size
	DWORD highestVertexIndex = 0;
	for (DWORD i = 0; i < dwVertexCount; i++) {
		if (highestVertexIndex < pIndexData[i]) {
			highestVertexIndex = pIndexData[i];
		}
	}

	return dwIndexBase + highestVertexIndex + 1;
}

int CountActiveD3DStreams()
{
	int lastStreamIndex = 0;
	for (int i = 0; i < 16; i++) {
		if (g_D3DStreams[i] != nullptr) {
			lastStreamIndex = i + 1;
		}
	}

	return lastStreamIndex;
}

XTL::CxbxVertexShaderInfo *GetCxbxVertexShaderInfo(DWORD Handle); // forward

UINT XTL::CxbxVertexBufferConverter::GetNbrStreams(CxbxDrawContext *pDrawContext)
{
	// Draw..Up always have one stream
	if (pDrawContext->pXboxVertexStreamZeroData != nullptr) {
		return 1;
	}

    if(VshHandleIsVertexShader(pDrawContext->hVertexShader)) {
        CxbxVertexShaderInfo *pVertexShaderInfo = GetCxbxVertexShaderInfo(pDrawContext->hVertexShader);
		if (pVertexShaderInfo) {
			return pVertexShaderInfo->NumberOfVertexStreams;
		}

		return CountActiveD3DStreams();
    } 
	
	if (pDrawContext->hVertexShader) {
		return CountActiveD3DStreams();
    }

    return 0;
}

inline FLOAT PackedIntToFloat(const int value, const FLOAT PosFactor, const FLOAT NegFactor)
{
	if (value >= 0) {
		return ((FLOAT)value) / PosFactor;
	}
	else {
		return ((FLOAT)value) / NegFactor;
	}
}

inline FLOAT NormShortToFloat(const SHORT value)
{
	return PackedIntToFloat((int)value, 32767.0f, 32768.0f);
}

inline FLOAT ByteToFloat(const BYTE value)
{
	return ((FLOAT)value) / 255.0f;
}

void XTL::CxbxVertexBufferConverter::ConvertStream
(
	CxbxDrawContext *pDrawContext,
    UINT             uiStream
)
{
	extern XTL::D3DCAPS g_D3DCaps;

	bool bVshHandleIsFVF = VshHandleIsFVF(pDrawContext->hVertexShader);
	DWORD XboxFVF = bVshHandleIsFVF ? pDrawContext->hVertexShader : 0;
	// Texture normalization can only be set for FVF shaders
	bool bNeedTextureNormalization = false;
	struct { int NrTexCoords; bool bTexIsLinear; int Width; int Height; int Depth; } pActivePixelContainer[X_D3DTS_STAGECOUNT] = { 0 };

	if (bVshHandleIsFVF) {
		DWORD dwTexN = (XboxFVF & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
		if (dwTexN > X_D3DTS_STAGECOUNT) {
			LOG_TEST_CASE("FVF,dwTexN > X_D3DTS_STAGECOUNT");
		}

		// Check for active linear textures.
		//X_D3DBaseTexture *pLinearBaseTexture[X_D3DTS_STAGECOUNT];
		for (uint i = 0; i < X_D3DTS_STAGECOUNT; i++) {
			// Only normalize coordinates used by the FVF shader :
			if (i + 1 <= dwTexN) {
				pActivePixelContainer[i].NrTexCoords = DxbxFVF_GetNumberOfTextureCoordinates(XboxFVF, i);
				// TODO : Use GetXboxBaseTexture()
				X_D3DBaseTexture *pXboxBaseTexture = EmuD3DActiveTexture[i];
				if (pXboxBaseTexture != xbnullptr) {
					// TODO : Use GetXboxPixelContainerFormat
					XTL::X_D3DFORMAT XBFormat = (XTL::X_D3DFORMAT)((pXboxBaseTexture->Format & X_D3DFORMAT_FORMAT_MASK) >> X_D3DFORMAT_FORMAT_SHIFT);
					if (EmuXBFormatIsLinear(XBFormat)) {
						// This is often hit by the help screen in XDK samples.
						bNeedTextureNormalization = true;
						// Remember linearity, width and height :
						pActivePixelContainer[i].bTexIsLinear = true;
						// TODO : Use DecodeD3DSize or GetPixelContainerWidth + GetPixelContainerHeight
						pActivePixelContainer[i].Width = (pXboxBaseTexture->Size & X_D3DSIZE_WIDTH_MASK) + 1;
						pActivePixelContainer[i].Height = ((pXboxBaseTexture->Size & X_D3DSIZE_HEIGHT_MASK) >> X_D3DSIZE_HEIGHT_SHIFT) + 1;
						// TODO : Support 3D textures
					}
				}
			}
		}
	}

	CxbxVertexShaderStreamInfo *pVertexShaderStreamInfo = nullptr;
	if (m_pVertexShaderInfo != nullptr) {
		pVertexShaderStreamInfo = &(m_pVertexShaderInfo->VertexStreams[uiStream]);
	}

	bool bNeedVertexPatching = (pVertexShaderStreamInfo != nullptr && pVertexShaderStreamInfo->NeedPatch);
	bool bNeedRHWReset = bVshHandleIsFVF && ((XboxFVF & D3DFVF_POSITION_MASK) == D3DFVF_XYZRHW);
	bool bNeedStreamCopy = bNeedTextureNormalization || bNeedVertexPatching || bNeedRHWReset;

	uint08 *pXboxVertexData;
	UINT uiXboxVertexStride;
	UINT uiVertexCount;
	UINT uiHostVertexStride;
	DWORD dwHostVertexDataSize;
	uint08 *pHostVertexData;
	IDirect3DVertexBuffer *pNewHostVertexBuffer = nullptr;

    if (pDrawContext->pXboxVertexStreamZeroData != xbnullptr) {
		// There should only be one stream (stream zero) in this case
		if (uiStream != 0) {
			CxbxKrnlCleanup("Trying to patch a Draw..UP with more than stream zero!");
		}

		pXboxVertexData = (uint08 *)pDrawContext->pXboxVertexStreamZeroData;
		uiXboxVertexStride = pDrawContext->uiXboxVertexStreamZeroStride;
		uiVertexCount = pDrawContext->VerticesInBuffer; 
		uiHostVertexStride = (bNeedVertexPatching) ? pVertexShaderStreamInfo->HostVertexStride : uiXboxVertexStride;
		dwHostVertexDataSize = uiVertexCount * uiHostVertexStride;
		if (bNeedStreamCopy) {
			pHostVertexData = (uint08*)malloc(dwHostVertexDataSize);
			if (pHostVertexData == nullptr) {
				CxbxKrnlCleanup("Couldn't allocate the new stream zero buffer");
			}
		}
		else {
			pHostVertexData = pXboxVertexData;
		}
	}
	else {
		XTL::X_D3DVertexBuffer *pXboxVertexBuffer = g_D3DStreams[uiStream];
        pXboxVertexData = (uint08*)GetDataFromXboxResource(pXboxVertexBuffer);
		if (pXboxVertexData == NULL) {
			HRESULT hRet = g_pD3DDevice->SetStreamSource(
				uiStream, 
				nullptr, 
				0, // OffsetInBytes
				0);
//			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetStreamSource");
			if (FAILED(hRet)) {
				EmuWarning("g_pD3DDevice->SetStreamSource(uiStream, nullptr, 0)");
			}

			return;
		}

		uiXboxVertexStride = g_D3DStreamStrides[uiStream];
        // Set a new (exact) vertex count
		uiVertexCount = pDrawContext->VerticesInBuffer;
		// Dxbx note : Don't overwrite pDrawContext.dwVertexCount with uiVertexCount, because an indexed draw
		// can (and will) use less vertices than the supplied nr of indexes. Thix fixes
		// the missing parts in the CompressedVertices sample (in Vertex shader mode).

		uiHostVertexStride = (bNeedVertexPatching) ? pVertexShaderStreamInfo->HostVertexStride : uiXboxVertexStride;
		dwHostVertexDataSize = uiVertexCount * uiHostVertexStride;
		GetCachedVertexBufferObject(pXboxVertexBuffer->Data, dwHostVertexDataSize, &pNewHostVertexBuffer);

        if (FAILED(pNewHostVertexBuffer->Lock(0, 0, (D3DLockData **)&pHostVertexData, D3DLOCK_DISCARD))) {
            CxbxKrnlCleanup("Couldn't lock the new buffer");
        }

		// Copy stream for patching and caching.
		bNeedStreamCopy = true;
    }
	
	if (bNeedVertexPatching) {
	    // assert(bNeedStreamCopy || "bNeedVertexPatching implies bNeedStreamCopy (but copies via conversions");
		for (uint32 uiVertex = 0; uiVertex < uiVertexCount; uiVertex++) {
			uint08 *pXboxVertexAsByte = &pXboxVertexData[uiVertex * uiXboxVertexStride];
			uint08 *pHostVertexAsByte = &pHostVertexData[uiVertex * uiHostVertexStride];
			for (UINT uiElement = 0; uiElement < pVertexShaderStreamInfo->NumberOfVertexElements; uiElement++) {
				FLOAT *pXboxVertexAsFloat = (FLOAT*)pXboxVertexAsByte;
				SHORT *pXboxVertexAsShort = (SHORT*)pXboxVertexAsByte;
				int XboxElementByteSize = pVertexShaderStreamInfo->VertexElements[uiElement].HostByteSize;
				FLOAT *pHostVertexAsFloat = (FLOAT*)pHostVertexAsByte;
				SHORT *pHostVertexAsShort = (SHORT*)pHostVertexAsByte;
				// Dxbx note : The following code handles only the D3DVSDT enums that need conversion;
				// All other cases are catched by the memcpy in the default-block.
				switch (pVertexShaderStreamInfo->VertexElements[uiElement].XboxType) {
				case X_D3DVSDT_NORMSHORT1: { // 0x11:
					// Test-cases : Halo - Combat Evolved
					XboxElementByteSize = 1 * sizeof(SHORT);
					if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT2N) {
						// Make it SHORT2N
						pHostVertexAsShort[0] = pXboxVertexAsShort[0];
						pHostVertexAsShort[1] = 0;
					}
					else
					{
						// Make it FLOAT1
						pHostVertexAsFloat[0] = NormShortToFloat(pXboxVertexAsShort[0]);
						//pHostVertexAsFloat[1] = 0.0f; // Would be needed for FLOAT2
					}
					break;
				}
				case X_D3DVSDT_NORMSHORT2: { // 0x21:
					// Test-cases : Baldur's Gate: Dark Alliance 2, F1 2002, Gun, Halo - Combat Evolved, Scrapland 
					XboxElementByteSize = 2 * sizeof(SHORT);
					if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT2N) {
						// No need for patching when D3D9 supports D3DDECLTYPE_SHORT2N
						// TODO : goto default; // ??
						//assert(XboxElementByteSize == 2 * sizeof(SHORT));
						//memcpy(pHostVertexAsByte, pXboxVertexAsByte, XboxElementByteSize);
						// Make it SHORT2N
						pHostVertexAsShort[0] = pXboxVertexAsShort[0];
						pHostVertexAsShort[1] = pXboxVertexAsShort[1];
					}
					else
					{
						// Make it FLOAT2
						pHostVertexAsFloat[0] = NormShortToFloat(pXboxVertexAsShort[0]);
						pHostVertexAsFloat[1] = NormShortToFloat(pXboxVertexAsShort[1]);
					}
					break;
				}
				case X_D3DVSDT_NORMSHORT3: { // 0x31:
					// Test-cases : Cel Damage, Constantine, Destroy All Humans!
					XboxElementByteSize = 3 * sizeof(SHORT);
					if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT4N) {
						// Make it SHORT4N
						pHostVertexAsShort[0] = pXboxVertexAsShort[0];
						pHostVertexAsShort[1] = pXboxVertexAsShort[1];
						pHostVertexAsShort[2] = pXboxVertexAsShort[2];
						pHostVertexAsShort[3] = 32767; // TODO : verify
					}
					else
					{
						// Make it FLOAT3
						pHostVertexAsFloat[0] = NormShortToFloat(pXboxVertexAsShort[0]);
						pHostVertexAsFloat[1] = NormShortToFloat(pXboxVertexAsShort[1]);
						pHostVertexAsFloat[2] = NormShortToFloat(pXboxVertexAsShort[2]);
					}
					break;
				}
				case X_D3DVSDT_NORMSHORT4: { // 0x41:
					// Test-cases : Judge Dredd: Dredd vs Death, NHL Hitz 2002, Silent Hill 2, Sneakers, Tony Hawk Pro Skater 4
					XboxElementByteSize = 4 * sizeof(SHORT);
					if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT4N) {
						// No need for patching when D3D9 supports D3DDECLTYPE_SHORT4N
						// TODO : goto default; // ??
						//assert(XboxElementByteSize == 4 * sizeof(SHORT));
						//memcpy(pHostVertexAsByte, pXboxVertexAsByte, XboxElementByteSize);
						// Make it SHORT4N
						pHostVertexAsShort[0] = pXboxVertexAsShort[0];
						pHostVertexAsShort[1] = pXboxVertexAsShort[1];
						pHostVertexAsShort[2] = pXboxVertexAsShort[2];
						pHostVertexAsShort[3] = pXboxVertexAsShort[3];
					}
					else
					{
						// Make it FLOAT4
						pHostVertexAsFloat[0] = NormShortToFloat(pXboxVertexAsShort[0]);
						pHostVertexAsFloat[1] = NormShortToFloat(pXboxVertexAsShort[1]);
						pHostVertexAsFloat[2] = NormShortToFloat(pXboxVertexAsShort[2]);
						pHostVertexAsFloat[3] = NormShortToFloat(pXboxVertexAsShort[3]);
					}
					break;
				}
				case X_D3DVSDT_NORMPACKED3: { // 0x16:
					// Test-cases : Dashboard
					XboxElementByteSize = 1 * sizeof(int32);
					// Make it FLOAT3
					union {
						int32 value;
						struct {
							int x : 11;
							int y : 11;
							int z : 10;
						};
					} NormPacked3;

					NormPacked3.value = ((int32 *)pXboxVertexAsByte)[0];

					pHostVertexAsFloat[0] = PackedIntToFloat(NormPacked3.x, 1023.0f, 1024.f);
					pHostVertexAsFloat[1] = PackedIntToFloat(NormPacked3.y, 1023.0f, 1024.f);
					pHostVertexAsFloat[2] = PackedIntToFloat(NormPacked3.z, 511.0f, 512.f);
					break;
				}
				case X_D3DVSDT_SHORT1: { // 0x15:
					XboxElementByteSize = 1 * sizeof(SHORT);
					// Make it SHORT2 and set the second short to 0
					pHostVertexAsShort[0] = pXboxVertexAsShort[0];
					pHostVertexAsShort[1] = 0;
					break;
				}
				case X_D3DVSDT_SHORT3: { // 0x35:
					// Test-cases : Turok
					XboxElementByteSize = 3 * sizeof(SHORT);
					// Make it a SHORT4 and set the fourth short to 1
					pHostVertexAsShort[0] = pXboxVertexAsShort[0];
					pHostVertexAsShort[1] = pXboxVertexAsShort[1];
					pHostVertexAsShort[2] = pXboxVertexAsShort[2];
					pHostVertexAsShort[3] = 1; // Turok verified (character disappears when this is 32767)
					break;
				}
				case X_D3DVSDT_PBYTE1: { // 0x14:
					XboxElementByteSize = 1 * sizeof(BYTE);
					if (g_D3DCaps.DeclTypes & D3DDTCAPS_UBYTE4N) {
						// Make it UBYTE4N
						pHostVertexAsByte[0] = pXboxVertexAsByte[0];
						pHostVertexAsByte[1] = 0;
						pHostVertexAsByte[2] = 0;
						pHostVertexAsByte[3] = 255; // TODO : Verify
					}
					else
					{
						// Make it FLOAT1
						pHostVertexAsFloat[0] = ByteToFloat(pXboxVertexAsByte[0]);
					}
					break;
				}
				case X_D3DVSDT_PBYTE2: { // 0x24:
					XboxElementByteSize = 2 * sizeof(BYTE);
					if (g_D3DCaps.DeclTypes & D3DDTCAPS_UBYTE4N) {
						// Make it UBYTE4N
						pHostVertexAsByte[0] = pXboxVertexAsByte[0];
						pHostVertexAsByte[1] = pXboxVertexAsByte[1];
						pHostVertexAsByte[2] = 0;
						pHostVertexAsByte[3] = 255; // TODO : Verify
					}
					else
					{
						// Make it FLOAT2
						pHostVertexAsFloat[0] = ByteToFloat(pXboxVertexAsByte[0]);
						pHostVertexAsFloat[1] = ByteToFloat(pXboxVertexAsByte[1]);
					}
					break;
				}
				case X_D3DVSDT_PBYTE3: { // 0x34:
					// Test-cases : Turok
					XboxElementByteSize = 3 * sizeof(BYTE);
					if (g_D3DCaps.DeclTypes & D3DDTCAPS_UBYTE4N) {
						// Make it UBYTE4N
						pHostVertexAsByte[0] = pXboxVertexAsByte[0];
						pHostVertexAsByte[1] = pXboxVertexAsByte[1];
						pHostVertexAsByte[2] = pXboxVertexAsByte[2];
						pHostVertexAsByte[3] = 255; // TODO : Verify
					}
					else
					{
						// Make it FLOAT3
						pHostVertexAsFloat[0] = ByteToFloat(pXboxVertexAsByte[0]);
						pHostVertexAsFloat[1] = ByteToFloat(pXboxVertexAsByte[1]);
						pHostVertexAsFloat[2] = ByteToFloat(pXboxVertexAsByte[2]);
					}
					break;
				}
				case X_D3DVSDT_PBYTE4: { // 0x44:
					// Test-case : Jet Set Radio Future
					XboxElementByteSize = 4 * sizeof(BYTE);
					if (g_D3DCaps.DeclTypes & D3DDTCAPS_UBYTE4N) {
						// No need for patching when D3D9 supports D3DDECLTYPE_UBYTE4N
						// TODO : goto default; // ??
						//assert(XboxElementByteSize == 4 * sizeof(BYTE));
						//memcpy(pHostVertexAsByte, pXboxVertexAsByte, XboxElementByteSize);
						// Make it UBYTE4N
						pHostVertexAsByte[0] = pXboxVertexAsByte[0];
						pHostVertexAsByte[1] = pXboxVertexAsByte[1];
						pHostVertexAsByte[2] = pXboxVertexAsByte[2];
						pHostVertexAsByte[3] = pXboxVertexAsByte[3];
					}
					else
					{
						// Make it FLOAT4
						pHostVertexAsFloat[0] = ByteToFloat(pXboxVertexAsByte[0]);
						pHostVertexAsFloat[1] = ByteToFloat(pXboxVertexAsByte[1]);
						pHostVertexAsFloat[2] = ByteToFloat(pXboxVertexAsByte[2]);
						pHostVertexAsFloat[3] = ByteToFloat(pXboxVertexAsByte[3]);
					}
					break;
				}
				case X_D3DVSDT_FLOAT2H: { // 0x72:
					XboxElementByteSize = 3 * sizeof(FLOAT);
					// Make it FLOAT4 and set the third float to 0.0
					pHostVertexAsFloat[0] = pXboxVertexAsFloat[0];
					pHostVertexAsFloat[1] = pXboxVertexAsFloat[1];
					pHostVertexAsFloat[2] = 0.0f;
					pHostVertexAsFloat[3] = pXboxVertexAsFloat[2];
					break;
				}
				case X_D3DVSDT_NONE: { // 0x02: // Skip it
					// Test-case : WWE RAW2
					LOG_TEST_CASE("X_D3DVSDT_NONE");
					break;
				}
				default: {
					// Generic 'conversion' - just make a copy :
					memcpy(pHostVertexAsByte, pXboxVertexAsByte, XboxElementByteSize);
					break;
				}
				} // switch

				// Increment the Xbox pointer :
				pXboxVertexAsByte += XboxElementByteSize;
				// Increment the host pointer :
				pHostVertexAsByte += pVertexShaderStreamInfo->VertexElements[uiElement].HostByteSize;
			} // for NumberOfVertexElements
		} // for uiVertexCount
    }
    else {
		if (bNeedStreamCopy) {
			memcpy(pHostVertexData, pXboxVertexData, dwHostVertexDataSize);
		}
	}

	// Xbox FVF shaders are identical to host Direct3D 8.1, however
	// texture coordinates may need normalization if used with linear textures.
	if (bNeedTextureNormalization || bNeedRHWReset) {
		// assert(bVshHandleIsFVF);

		UINT uiTextureCoordinatesByteOffsetInVertex = 0;

		// Locate texture coordinate offset in vertex structure.
		if (bNeedTextureNormalization) {
			uiTextureCoordinatesByteOffsetInVertex = XTL::DxbxFVFToVertexSizeInBytes(XboxFVF, /*bIncludeTextures=*/false);
			if (bNeedVertexPatching) {
				LOG_TEST_CASE("Potential xbox vs host texture-offset difference! (bNeedVertexPatching within bNeedTextureNormalization)");
			}
			// As long as vertices aren't resized / patched up until the texture coordinates,
			// the uiTextureCoordinatesByteOffsetInVertex on host will match Xbox 
		}

		for (uint32 uiVertex = 0; uiVertex < uiVertexCount; uiVertex++) {
			FLOAT *pVertexDataAsFloat = (FLOAT*)(&pHostVertexData[uiVertex * uiHostVertexStride]);

			// Handle pre-transformed vertices (which bypass the vertex shader pipeline)
			if (bNeedRHWReset) {
#if 0
				// Check Z. TODO : Why reset Z from 0.0 to 1.0 ? (Maybe fog-related?)
				if (pVertexDataAsFloat[2] == 0.0f) {
					// LOG_TEST_CASE("D3DFVF_XYZRHW (Z)"); // Test-case : Many XDK Samples (AlphaFog, PointSprites)
					pVertexDataAsFloat[2] = 1.0f;
				}
#endif
#if 1
				// Check RHW. TODO : Why reset from 0.0 to 1.0 ? (Maybe 1.0 indicates that the vertices are not to be transformed)
				if (pVertexDataAsFloat[3] == 0.0f) {
					// LOG_TEST_CASE("D3DFVF_XYZRHW (RHW)"); // Test-case : Many XDK Samples (AlphaFog, PointSprites)
					pVertexDataAsFloat[3] = 1.0f;
				}
#endif
			}

			// Normalize texture coordinates in FVF stream if needed
			if (uiTextureCoordinatesByteOffsetInVertex > 0) { // implies bNeedTextureNormalization (using one is more efficient than both)
				FLOAT *pVertexUVData = (FLOAT*)((uintptr_t)pVertexDataAsFloat + uiTextureCoordinatesByteOffsetInVertex);
				for (uint i = 0; i < X_D3DTS_STAGECOUNT; i++) {
					if (pActivePixelContainer[i].bTexIsLinear) {
						switch (pActivePixelContainer[i].NrTexCoords) {
						case 0:
							LOG_TEST_CASE("Normalize 0D?");
							break;
						case 1:
							LOG_TEST_CASE("Normalize 1D");
							pVertexUVData[0] /= pActivePixelContainer[i].Width;
							break;
						case 2:
							pVertexUVData[0] /= pActivePixelContainer[i].Width;
							pVertexUVData[1] /= pActivePixelContainer[i].Height;
							break;
						case 3:
							LOG_TEST_CASE("Normalize 3D");
							// Test case : HeatShimmer
							pVertexUVData[0] /= pActivePixelContainer[i].Width;
							pVertexUVData[1] /= pActivePixelContainer[i].Height;
							pVertexUVData[2] /= pActivePixelContainer[i].Depth;
							break;
						default:
							LOG_TEST_CASE("Normalize ?D");
							break;
						}
					}

					pVertexUVData += pActivePixelContainer[i].NrTexCoords;
				}
			}
		}
	}

	CxbxPatchedStream *pPatchedStream = &m_PatchedStreams[uiStream];

#if 0 // new
	pPatchedStream->pCachedXboxVertexData = pXboxVertexData; // TODO : For hashing & caching purposes
#endif
	pPatchedStream->uiCachedXboxVertexStride = uiXboxVertexStride;
#if 0 // new
	pPatchedStream->uiCachedXboxVertexDataSize = uiVertexCount * uiXboxVertexStride; // TODO : For hashing & caching purposes
#endif
	pPatchedStream->uiCachedHostVertexStride = uiHostVertexStride;
	pPatchedStream->bCacheIsStreamZeroDrawUP = (pDrawContext->pXboxVertexStreamZeroData != NULL);
	if (pPatchedStream->bCacheIsStreamZeroDrawUP) {
		pPatchedStream->pCachedHostVertexStreamZeroData = pHostVertexData;
		pPatchedStream->bCachedHostVertexStreamZeroDataIsAllocated = bNeedStreamCopy;
	}
	else {
		// assert(pNewHostVertexBuffer != nullptr);

		pNewHostVertexBuffer->Unlock();
		pPatchedStream->pCachedHostVertexBuffer = pNewHostVertexBuffer;
	}

	ActivatePatchedStream(pDrawContext, uiStream, pPatchedStream, 
		/*Release=*/!bNeedStreamCopy); // Release when it won't get cached
}

void XTL::CxbxVertexBufferConverter::Apply(CxbxDrawContext *pDrawContext)
{
	if ((pDrawContext->XboxPrimitiveType < X_D3DPT_POINTLIST) || (pDrawContext->XboxPrimitiveType > X_D3DPT_POLYGON))
		CxbxKrnlCleanup("Unknown primitive type: 0x%.02X\n", pDrawContext->XboxPrimitiveType);

    if (VshHandleIsVertexShader(pDrawContext->hVertexShader)) {
        m_pVertexShaderInfo = &(MapXboxVertexShaderHandleToCxbxVertexShader(pDrawContext->hVertexShader)->VertexShaderInfo);
    }

	pDrawContext->VerticesInBuffer = GetVerticesInBuffer(
		pDrawContext->dwStartVertex,
		pDrawContext->dwVertexCount,
		pDrawContext->pIndexData, 
		pDrawContext->dwIndexBase
	);

    // Get the number of streams
    m_uiNbrStreams = GetNbrStreams(pDrawContext);

    for(UINT uiStream = 0; uiStream < m_uiNbrStreams; uiStream++) {
		// TODO: Check for cached vertex buffer, and use it if possible

		ConvertStream(pDrawContext, uiStream);

		// TODO: Cache Vertex Buffer Data
    }

	if (pDrawContext->XboxPrimitiveType == X_D3DPT_QUADSTRIP) {
		// Quad strip is just like a triangle strip, but requires two vertices per primitive.
		// A quadstrip starts with 4 vertices and adds 2 vertices per additional quad.
		// This is much like a trianglestrip, which starts with 3 vertices and adds
		// 1 vertex per additional triangle, so we use that instead. The planar nature
		// of the quads 'survives' through this change. There's a catch though :
		// In a trianglestrip, every 2nd triangle has an opposing winding order,
		// which would cause backface culling - but this seems to be intelligently
		// handled by d3d :
		// Test-case : XDK Samples (FocusBlur, MotionBlur, Trees, PaintEffect, PlayField)
		// No need to set : pDrawContext->XboxPrimitiveType = X_D3DPT_TRIANGLESTRIP;
		pDrawContext->dwHostPrimitiveCount = EmuD3DVertex2PrimitiveCount(X_D3DPT_TRIANGLESTRIP, pDrawContext->dwVertexCount);
	} else {
		pDrawContext->dwHostPrimitiveCount = EmuD3DVertex2PrimitiveCount(pDrawContext->XboxPrimitiveType, pDrawContext->dwVertexCount);
	}

	if (pDrawContext->XboxPrimitiveType == X_D3DPT_POLYGON) {
		// Convex polygon is the same as a triangle fan.
		// No need to set : pDrawContext->XboxPrimitiveType = X_D3DPT_TRIANGLEFAN;
		// Test-case : Panzer Dragoon ORTA (when entering in-game)
		LOG_TEST_CASE("X_D3DPT_POLYGON");
	}
}

VOID XTL::EmuFlushIVB()
{
	CxbxUpdateNativeD3DResources();

    // Parse IVB table with current FVF shader if possible.
    bool bFVF = VshHandleIsFVF(g_CurrentXboxVertexShaderHandle);
    DWORD dwCurFVF = (bFVF) ? g_CurrentXboxVertexShaderHandle : g_InlineVertexBuffer_FVF;

    DbgPrintf("g_InlineVertexBuffer_TableOffset := %d\n", g_InlineVertexBuffer_TableOffset);

	// Check the given FVF
	switch (dwCurFVF & D3DFVF_POSITION_MASK) {
	case 0: // No position ?
		if (bFVF) {
			EmuWarning("EmuFlushIVB(): g_CurrentXboxVertexShaderHandle isn't a valid FVF - using D3DFVF_XYZRHW instead!");
			dwCurFVF |= D3DFVF_XYZRHW;
		}
		else {
			EmuWarning("EmuFlushIVB(): using g_InlineVertexBuffer_FVF instead of current FVF!");
			dwCurFVF = g_InlineVertexBuffer_FVF;
		}
		break;
	case D3DFVF_XYZRHW:
		// D3DFVF_NORMAL isn't allowed in combination with D3DFVF_XYZRHW 
		if (dwCurFVF & D3DFVF_NORMAL) {
			EmuWarning("EmuFlushIVB(): Normal encountered while D3DFVF_XYZRHW is given - switching back to D3DFVF_XYZ!");
			dwCurFVF &= ~D3DFVF_POSITION_MASK;
			dwCurFVF |= D3DFVF_XYZ;
		}
		break;
	}

	DWORD dwPos = dwCurFVF & D3DFVF_POSITION_MASK;
	DWORD dwTexN = (dwCurFVF & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
	size_t TexSize[X_D3DTS_STAGECOUNT]; // Xbox supports up to 4 textures (TEXTURE_STAGES)

	for (uint i = 0; i < dwTexN; i++) {
		TexSize[i] = DxbxFVF_GetNumberOfTextureCoordinates(dwCurFVF, i);
	}

	// Use a tooling function to determine the vertex stride :
	UINT uiStride = DxbxFVFToVertexSizeInBytes(dwCurFVF, /*bIncludeTextures=*/true);
	// Make sure the output buffer is big enough 
	UINT NeededSize = g_InlineVertexBuffer_TableOffset * uiStride;
	if (g_InlineVertexBuffer_DataSize < NeededSize) {
		g_InlineVertexBuffer_DataSize = NeededSize;
		if (g_InlineVertexBuffer_pData != nullptr) {
			free(g_InlineVertexBuffer_pData);
		}

		g_InlineVertexBuffer_pData = (FLOAT*)malloc(g_InlineVertexBuffer_DataSize);
	}

	FLOAT *pVertexBufferData = g_InlineVertexBuffer_pData;
	for(uint v=0;v<g_InlineVertexBuffer_TableOffset;v++) {
        *pVertexBufferData++ = g_InlineVertexBuffer_Table[v].Position.x;
        *pVertexBufferData++ = g_InlineVertexBuffer_Table[v].Position.y;
        *pVertexBufferData++ = g_InlineVertexBuffer_Table[v].Position.z;
		if (dwPos == D3DFVF_XYZRHW) {
            *pVertexBufferData++ = g_InlineVertexBuffer_Table[v].Rhw;
            DbgPrintf("IVB Position := {%f, %f, %f, %f}\n", g_InlineVertexBuffer_Table[v].Position.x, g_InlineVertexBuffer_Table[v].Position.y, g_InlineVertexBuffer_Table[v].Position.z, g_InlineVertexBuffer_Table[v].Rhw);
		}
		else { // XYZRHW cannot be combined with NORMAL, but the other XYZ formats can :
			switch (dwPos) {
			case D3DFVF_XYZ:
				DbgPrintf("IVB Position := {%f, %f, %f}\n", g_InlineVertexBuffer_Table[v].Position.x, g_InlineVertexBuffer_Table[v].Position.y, g_InlineVertexBuffer_Table[v].Position.z);
				break;
			case D3DFVF_XYZB1:
				*pVertexBufferData++ = g_InlineVertexBuffer_Table[v].Blend[0];
				DbgPrintf("IVB Position := {%f, %f, %f, %f}\n", g_InlineVertexBuffer_Table[v].Position.x, g_InlineVertexBuffer_Table[v].Position.y, g_InlineVertexBuffer_Table[v].Position.z, g_InlineVertexBuffer_Table[v].Blend[0]);
				break;
			case D3DFVF_XYZB2:
				*pVertexBufferData++ = g_InlineVertexBuffer_Table[v].Blend[0];
				*pVertexBufferData++ = g_InlineVertexBuffer_Table[v].Blend[1];
				DbgPrintf("IVB Position := {%f, %f, %f, %f, %f}\n", g_InlineVertexBuffer_Table[v].Position.x, g_InlineVertexBuffer_Table[v].Position.y, g_InlineVertexBuffer_Table[v].Position.z, g_InlineVertexBuffer_Table[v].Blend[0], g_InlineVertexBuffer_Table[v].Blend[1]);
				break;
			case D3DFVF_XYZB3:
				*pVertexBufferData++ = g_InlineVertexBuffer_Table[v].Blend[0];
				*pVertexBufferData++ = g_InlineVertexBuffer_Table[v].Blend[1];
				*pVertexBufferData++ = g_InlineVertexBuffer_Table[v].Blend[2];
				DbgPrintf("IVB Position := {%f, %f, %f, %f, %f, %f}\n", g_InlineVertexBuffer_Table[v].Position.x, g_InlineVertexBuffer_Table[v].Position.y, g_InlineVertexBuffer_Table[v].Position.z, g_InlineVertexBuffer_Table[v].Blend[0], g_InlineVertexBuffer_Table[v].Blend[1], g_InlineVertexBuffer_Table[v].Blend[2]);
				break;
			case D3DFVF_XYZB4:
				*pVertexBufferData++ = g_InlineVertexBuffer_Table[v].Blend[0];
				*pVertexBufferData++ = g_InlineVertexBuffer_Table[v].Blend[1];
				*pVertexBufferData++ = g_InlineVertexBuffer_Table[v].Blend[2];
				*pVertexBufferData++ = g_InlineVertexBuffer_Table[v].Blend[3];
				DbgPrintf("IVB Position := {%f, %f, %f, %f, %f, %f, %f}\n", g_InlineVertexBuffer_Table[v].Position.x, g_InlineVertexBuffer_Table[v].Position.y, g_InlineVertexBuffer_Table[v].Position.z, g_InlineVertexBuffer_Table[v].Blend[0], g_InlineVertexBuffer_Table[v].Blend[1], g_InlineVertexBuffer_Table[v].Blend[2], g_InlineVertexBuffer_Table[v].Blend[3]);
				break;
			default:
				CxbxKrnlCleanup("Unsupported Position Mask (FVF := 0x%.08X dwPos := 0x%.08X)", dwCurFVF, dwPos);
				break;
			}

			if (dwCurFVF & D3DFVF_NORMAL) {
				*pVertexBufferData++ = g_InlineVertexBuffer_Table[v].Normal.x;
				*pVertexBufferData++ = g_InlineVertexBuffer_Table[v].Normal.y;
				*pVertexBufferData++ = g_InlineVertexBuffer_Table[v].Normal.z;
				DbgPrintf("IVB Normal := {%f, %f, %f}\n", g_InlineVertexBuffer_Table[v].Normal.x, g_InlineVertexBuffer_Table[v].Normal.y, g_InlineVertexBuffer_Table[v].Normal.z);
			}
		}

#if 0 // TODO : Was this supported on Xbox from some point in time (pun intended)?
		if (dwCurFVF & D3DFVF_PSIZE) {
			*(DWORD*)pVertexBufferData++ = g_InlineVertexBuffer_Table[v].PointSize;
			DbgPrintf("IVB PointSize := 0x%.08X\n", g_InlineVertexBuffer_Table[v].PointSize);
		}
#endif

        if (dwCurFVF & D3DFVF_DIFFUSE) {
            *(DWORD*)pVertexBufferData++ = g_InlineVertexBuffer_Table[v].Diffuse;
            DbgPrintf("IVB Diffuse := 0x%.08X\n", g_InlineVertexBuffer_Table[v].Diffuse);
        }

		if (dwCurFVF & D3DFVF_SPECULAR) {
			*(DWORD*)pVertexBufferData++ = g_InlineVertexBuffer_Table[v].Specular;
			DbgPrintf("IVB Specular := 0x%.08X\n", g_InlineVertexBuffer_Table[v].Specular);
		}

		for (uint i = 0; i < dwTexN; i++) {
            *pVertexBufferData++ = g_InlineVertexBuffer_Table[v].TexCoord[i].x;
			if (TexSize[i] >= 2) {
				*pVertexBufferData++ = g_InlineVertexBuffer_Table[v].TexCoord[i].y;
				if (TexSize[i] >= 3) {
					*pVertexBufferData++ = g_InlineVertexBuffer_Table[v].TexCoord[i].z;
					if (TexSize[i] >= 4) {
						*pVertexBufferData++ = g_InlineVertexBuffer_Table[v].TexCoord[i].w;
					}
				}
			}

			if (g_bPrintfOn) {
				switch (TexSize[i]) {
				case 1: DbgPrintf("IVB TexCoord%d := {%f}\n", i + 1, g_InlineVertexBuffer_Table[v].TexCoord[i].x); break;
				case 2: DbgPrintf("IVB TexCoord%d := {%f, %f}\n", i + 1, g_InlineVertexBuffer_Table[v].TexCoord[i].x, g_InlineVertexBuffer_Table[v].TexCoord[i].y); break;
				case 3: DbgPrintf("IVB TexCoord%d := {%f, %f, %f}\n", i + 1, g_InlineVertexBuffer_Table[v].TexCoord[i].x, g_InlineVertexBuffer_Table[v].TexCoord[i].y, g_InlineVertexBuffer_Table[v].TexCoord[i].z); break;
				case 4: DbgPrintf("IVB TexCoord%d := {%f, %f, %f, %f}\n", i + 1, g_InlineVertexBuffer_Table[v].TexCoord[i].x, g_InlineVertexBuffer_Table[v].TexCoord[i].y, g_InlineVertexBuffer_Table[v].TexCoord[i].z, g_InlineVertexBuffer_Table[v].TexCoord[i].w); break;
				}
			}
        }

		if (v == 0) {
			uint VertexBufferUsage = (uintptr_t)pVertexBufferData - (uintptr_t)g_InlineVertexBuffer_pData;
			if (VertexBufferUsage != uiStride) {
				CxbxKrnlCleanup("EmuFlushIVB uses wrong stride!");
			}
		}
	}

	CxbxDrawContext DrawContext = {};

	DrawContext.XboxPrimitiveType = g_InlineVertexBuffer_PrimitiveType;
	DrawContext.dwVertexCount = g_InlineVertexBuffer_TableOffset;
	DrawContext.pXboxVertexStreamZeroData = g_InlineVertexBuffer_pData;
	DrawContext.uiXboxVertexStreamZeroStride = uiStride;
	DrawContext.hVertexShader = g_CurrentXboxVertexShaderHandle;

	HRESULT hRet;

	if (bFVF) {
		g_pD3DDevice->SetVertexShader(nullptr);
		hRet = g_pD3DDevice->SetFVF(dwCurFVF);
		//DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexShader");
	}

	CxbxDrawPrimitiveUP(DrawContext);
	if (bFVF) {
		hRet = g_pD3DDevice->SetFVF(g_CurrentXboxVertexShaderHandle);
		//DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetVertexShader");
	}
    g_InlineVertexBuffer_TableOffset = 0; // Might not be needed (also cleared in D3DDevice_Begin)
}
