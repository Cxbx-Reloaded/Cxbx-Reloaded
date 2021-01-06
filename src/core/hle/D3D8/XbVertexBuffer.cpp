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
// *  (c) 2002-2004 Aaron Robinson <caustik@caustik.com>
// *                Kingofc <kingofc@freenet.de>
// *
// *  All rights reserved
// *
// ******************************************************************
#define LOG_PREFIX CXBXR_MODULE::VTXB

#include <unordered_map>
#include "core\kernel\memory-manager\VMManager.h"
#include "common\util\hasher.h"
#include "core\kernel\support\Emu.h"
#include "core\hle\D3D8\Direct3D9\Direct3D9.h" // For g_pD3DDevice
#include "core\hle\D3D8\Direct3D9\WalkIndexBuffer.h" // for WalkIndexBuffer
#include "core\hle\D3D8\ResourceTracker.h"
#include "core\hle\D3D8\XbPushBuffer.h" // For CxbxDrawPrimitiveUP
#include "core\hle\D3D8\XbVertexBuffer.h"
#include "core\hle\D3D8\XbConvert.h"

#include <ctime>
#include <chrono>
#include <algorithm>

#define MAX_STREAM_NOT_USED_TIME (2 * CLOCKS_PER_SEC) // TODO: Trim the not used time

// Inline vertex buffer emulation
xbox::X_D3DPRIMITIVETYPE      g_InlineVertexBuffer_PrimitiveType = xbox::X_D3DPT_INVALID;
uint32_t                      g_InlineVertexBuffer_WrittenRegisters = 0; // A bitmask, indicating which registers have been set in g_InlineVertexBuffer_Table
xbox::X_VERTEXATTRIBUTEFORMAT g_InlineVertexBuffer_AttributeFormat = {};
bool                          g_InlineVertexBuffer_DeclarationOverride = false;
std::vector<D3DIVB>           g_InlineVertexBuffer_Table;
UINT                          g_InlineVertexBuffer_TableLength = 0;
UINT                          g_InlineVertexBuffer_TableOffset = 0;
FLOAT                        *g_InlineVertexBuffer_pData = nullptr;
UINT                          g_InlineVertexBuffer_DataSize = 0;

// Copy of active Xbox D3D Vertex Streams (and strides), set by [D3DDevice|CxbxImpl]_SetStreamSource*
xbox::X_STREAMINPUT g_Xbox_SetStreamSource[X_VSH_MAX_STREAMS] = { 0 }; // Note : .Offset member is never set (so always 0)

extern float *HLE_get_NV2A_vertex_attribute_value_pointer(unsigned VertexSlot); // Declared in PushBuffer.cpp

void *GetDataFromXboxResource(xbox::X_D3DResource *pXboxResource);
bool GetHostRenderTargetDimensions(DWORD* pHostWidth, DWORD* pHostHeight, IDirect3DSurface* pHostRenderTarget = nullptr);
uint32_t GetPixelContainerWidth(xbox::X_D3DPixelContainer* pPixelContainer);
uint32_t GetPixelContainerHeight(xbox::X_D3DPixelContainer* pPixelContainer);

struct _D3DIVB &_D3DIVB::operator=(const struct _D3DIVB &Val)
{
	Position.x = Val.Position.x;
	Position.y = Val.Position.y;
	Position.z = Val.Position.z;
	Rhw = Val.Rhw;
	std::copy(std::begin(Val.Blend), std::end(Val.Blend), std::begin(Blend));
	Normal.x = Val.Normal.x;
	Normal.y = Val.Normal.y;
	Normal.z = Val.Normal.z;
	Diffuse = Val.Diffuse;
	Specular = Val.Specular;
	Fog = Val.Fog;
	PointSize = Val.PointSize;
	BackDiffuse = Val.BackDiffuse;
	BackSpecular = Val.BackSpecular;
	std::copy(std::begin(Val.TexCoord), std::end(Val.TexCoord), std::begin(TexCoord));
	std::copy(std::begin(Val.Reg13Up), std::end(Val.Reg13Up), std::begin(Reg13Up));

	return *this;
}

void CxbxPatchedStream::Activate(CxbxDrawContext *pDrawContext, UINT HostStreamNumber) const
{
	//LOG_INIT // Allows use of DEBUG_D3DRESULT

	// Use the cached stream values on the host
	if (bCacheIsStreamZeroDrawUP) {
		// Set the UserPointer variables in the drawing context
		pDrawContext->pHostVertexStreamZeroData = pCachedHostVertexStreamZeroData;
		pDrawContext->uiHostVertexStreamZeroStride = uiCachedHostVertexStride;
	}
	else {
		HRESULT hRet = g_pD3DDevice->SetStreamSource(
			HostStreamNumber,
			pCachedHostVertexBuffer, 
			0, // OffsetInBytes
			uiCachedHostVertexStride);
		//DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetStreamSource");
		if (FAILED(hRet)) {
			CxbxKrnlCleanup("Failed to set the type patched buffer as the new stream source!\n");
			// TODO : test-case : XDK Cartoon hits the above case when the vertex cache size is 0.
		}
	}
}

CxbxPatchedStream::CxbxPatchedStream()
{
    isValid = false;
}

void CxbxPatchedStream::Clear()
{
    if (bCachedHostVertexStreamZeroDataIsAllocated) {
        free(pCachedHostVertexStreamZeroData);
        bCachedHostVertexStreamZeroDataIsAllocated = false;
    }

    pCachedHostVertexStreamZeroData = nullptr;

    if (pCachedHostVertexBuffer != nullptr) {
        pCachedHostVertexBuffer->Release();
        pCachedHostVertexBuffer = nullptr;
    }
}

CxbxPatchedStream::~CxbxPatchedStream()
{
	Clear();
}

CxbxVertexBufferConverter::CxbxVertexBufferConverter()
{
    m_uiNbrStreams = 0;
    m_pCxbxVertexDeclaration = nullptr;
}

// TODO: CountActiveD3DStreams must be removed once we can rely on CxbxGetVertexDeclaration always being set
int CountActiveD3DStreams()
{
	int StreamCount = 0;
	for (int XboxStreamNumber = 0; XboxStreamNumber < X_VSH_MAX_STREAMS; XboxStreamNumber++) {
		if (GetXboxVertexStreamInput(XboxStreamNumber).VertexBuffer != xbox::zeroptr) {
			StreamCount++;
		}
	}

	return StreamCount;
}

UINT CxbxVertexBufferConverter::GetNbrStreams(CxbxDrawContext *pDrawContext)
{
	// Draw..Up always have one stream
	if (pDrawContext->pXboxVertexStreamZeroData != xbox::zeroptr) {
		return 1;
	}

	CxbxVertexDeclaration *pDecl = CxbxGetVertexDeclaration();
	if (pDecl) {
		return pDecl->NumberOfVertexStreams;
    } 
	
	// TODO: This code and CountActiveD3DStreams must be removed once we can rely on CxbxGetVertexDeclaration always being set
	if (g_Xbox_VertexShader_Handle) {
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

CxbxPatchedStream& CxbxVertexBufferConverter::GetPatchedStream(uint64_t key)
{
    // First, attempt to fetch an existing patched stream
    auto it = m_PatchedStreams.find(key);
    if (it != m_PatchedStreams.end()) {
        m_PatchedStreamUsageList.splice(m_PatchedStreamUsageList.begin(), m_PatchedStreamUsageList, it->second);
        return *it->second;
    }

    // We didn't find an existing patched stream, so we must insert one and get a reference to it
    m_PatchedStreamUsageList.push_front({});
    CxbxPatchedStream& stream = m_PatchedStreamUsageList.front();

    // Insert a reference iterator into the fast lookup map
    m_PatchedStreams[key] = m_PatchedStreamUsageList.begin();

    // If the cache has exceeded it's upper bound, discard the oldest entries in the cache
    if (m_PatchedStreams.size() > (m_MaxCacheSize + m_CacheElasticity)) {
        while (m_PatchedStreams.size() > m_MaxCacheSize) {
            m_PatchedStreams.erase(m_PatchedStreamUsageList.back().uiVertexDataHash);
            m_PatchedStreamUsageList.pop_back();
        }
    }
    
    return stream;
}

void CxbxVertexBufferConverter::PrintStats()
{
    printf("Vertex Buffer Cache Status: \n");
    printf("- Cache Size: %d\n", m_PatchedStreams.size());
    printf("- Hits: %d\n", m_TotalCacheHits);
    printf("- Misses: %d\n", m_TotalCacheMisses);
}

void CxbxVertexBufferConverter::ConvertStream
(
	CxbxDrawContext *pDrawContext,
    UINT             uiStream
)
{
	extern D3DCAPS g_D3DCaps;
		//X_D3DBaseTexture *pLinearBaseTexture[xbox::X_D3DTS_STAGECOUNT];

	CxbxVertexShaderStreamInfo *pVertexShaderStreamInfo = nullptr;
	UINT XboxStreamNumber = uiStream;
	if (m_pCxbxVertexDeclaration != nullptr) {
		if (uiStream > m_pCxbxVertexDeclaration->NumberOfVertexStreams) {
			LOG_TEST_CASE("uiStream > NumberOfVertexStreams");
			return;
		}

		pVertexShaderStreamInfo = &(m_pCxbxVertexDeclaration->VertexStreams[uiStream]);
		XboxStreamNumber = pVertexShaderStreamInfo->XboxStreamIndex;
	}

	bool bNeedVertexPatching = (pVertexShaderStreamInfo != nullptr && pVertexShaderStreamInfo->NeedPatch);
	bool bNeedStreamCopy = bNeedVertexPatching;

	UINT HostStreamNumber = XboxStreamNumber; // Use Xbox stream index on host
	uint8_t *pXboxVertexData = xbox::zeroptr;
	UINT uiXboxVertexStride = 0;
	UINT uiHostVertexStride = 0;
	uint8_t *pHostVertexData = nullptr;
	IDirect3DVertexBuffer *pNewHostVertexBuffer = nullptr;

    if (pDrawContext->pXboxVertexStreamZeroData != xbox::zeroptr) {
		// There should only be one stream (stream zero) in this case
		if (XboxStreamNumber != 0) {
			CxbxKrnlCleanup("Trying to patch a Draw..UP with more than stream zero!");
		}

		pXboxVertexData = (uint8_t *)pDrawContext->pXboxVertexStreamZeroData;
		uiXboxVertexStride = pDrawContext->uiXboxVertexStreamZeroStride;
		uiHostVertexStride = (bNeedVertexPatching) ? pVertexShaderStreamInfo->HostVertexStride : uiXboxVertexStride;
	} else {
		xbox::X_STREAMINPUT& XboxStreamInput = GetXboxVertexStreamInput(XboxStreamNumber);
		xbox::X_D3DVertexBuffer *pXboxVertexBuffer = XboxStreamInput.VertexBuffer;
        pXboxVertexData = (uint8_t*)GetDataFromXboxResource(pXboxVertexBuffer);
		if (pXboxVertexData == xbox::zeroptr) {
			HRESULT hRet = g_pD3DDevice->SetStreamSource(
				HostStreamNumber,
				nullptr, 
				0, // OffsetInBytes
				0);
//			DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetStreamSource");
			if (FAILED(hRet)) {
				EmuLog(LOG_LEVEL::WARNING, "g_pD3DDevice->SetStreamSource(HostStreamNumber, nullptr, 0)");
			}

			return;
		}

		pXboxVertexData += XboxStreamInput.Offset;
		uiXboxVertexStride = XboxStreamInput.Stride;
		// Dxbx note : Don't overwrite pDrawContext.dwVertexCount with uiVertexCount, because an indexed draw
		// can (and will) use less vertices than the supplied nr of indexes. Thix fixes
		// the missing parts in the CompressedVertices sample (in Vertex shader mode).

		uiHostVertexStride = (bNeedVertexPatching) ? pVertexShaderStreamInfo->HostVertexStride : uiXboxVertexStride;

		// Copy stream for patching and caching.
		bNeedStreamCopy = true;
    }

    // FAST PATH: If this draw is a zerostream based draw, and does not require patching, we can use it directly
    // No need to hash or patch at all in this case!
    if (pDrawContext->pXboxVertexStreamZeroData != xbox::zeroptr && !bNeedStreamCopy) {
        pHostVertexData = pXboxVertexData;

        CxbxPatchedStream stream;
        stream.isValid = true;
        stream.XboxPrimitiveType = pDrawContext->XboxPrimitiveType;
        stream.uiCachedHostVertexStride = uiHostVertexStride;
        stream.bCacheIsStreamZeroDrawUP = true;
        stream.pCachedHostVertexStreamZeroData = pHostVertexData;
        stream.Activate(pDrawContext, HostStreamNumber);
        return;
    }

    // Now we have enough information to hash the existing resource and find it in our cache!
    // To avoid hashing and converting unused vertices, identify the "interesting" region
    // basing on the index/starting vertex data 
    if (pDrawContext->pXboxIndexData != nullptr) {
        pXboxVertexData += (pDrawContext->dwBaseVertexIndex + pDrawContext->LowIndex) * uiXboxVertexStride;
    } else {
        pXboxVertexData += pDrawContext->dwStartVertex * uiXboxVertexStride;
    }

    const UINT uiVertexCount = pDrawContext->NumVerticesToUse;
    const DWORD dwHostVertexDataSize = uiVertexCount * uiHostVertexStride;
    const DWORD xboxVertexDataSize = uiVertexCount * uiXboxVertexStride;
    uint64_t vertexDataHash = ComputeHash(pXboxVertexData, xboxVertexDataSize);
    uint64_t pVertexShaderSteamInfoHash = 0;

    if (pVertexShaderStreamInfo != nullptr) {
        pVertexShaderSteamInfoHash = ComputeHash(pVertexShaderStreamInfo, sizeof(CxbxVertexShaderStreamInfo));
    }

    // Lookup implicity inserts a new entry if not exists, so this always works
    CxbxPatchedStream& patchedStream = GetPatchedStream(vertexDataHash);

    // We check a few fields of the patched stream to protect against hash collisions (rare)
    // but also to protect against games using the exact same vertex data for different vertex formats (Test Case: Burnout)
    if (patchedStream.isValid && // Check that we found a cached stream
        patchedStream.uiVertexStreamInformationHash == pVertexShaderSteamInfoHash && // Check that the vertex conversion is valid
        patchedStream.uiCachedHostVertexStride == patchedStream.uiCachedHostVertexStride && // Make sure the host stride didn't change
        patchedStream.uiCachedXboxVertexStride == uiXboxVertexStride && // Make sure the Xbox Stride didn't change
        patchedStream.uiCachedXboxVertexDataSize == xboxVertexDataSize ) { // Make sure the Xbox Data Size also didn't change
        m_TotalCacheHits++;
        patchedStream.Activate(pDrawContext, HostStreamNumber);
        return;
    }

    m_TotalCacheMisses++;

    // If execution reaches here, the cached vertex buffer was not valid and we must reconvert the data
    // Free the existing buffers
	patchedStream.Clear();
    assert(pHostVertexData == nullptr);
	assert(pNewHostVertexBuffer == nullptr);

	// If dwHostVertexDataSize is zero, the allocation/creation will fail
	// This can be caused by a stride of 0, and 'other' invalid configurations
	// Test Case :SSX series of games
	if (dwHostVertexDataSize == 0) {
		LOG_TEST_CASE("Attempted to use a 0 sized vertex stream");
		return;
	}

    // Allocate new buffers
    if (pDrawContext->pXboxVertexStreamZeroData != xbox::zeroptr) {
        pHostVertexData = (uint8_t*)malloc(dwHostVertexDataSize);

        if (pHostVertexData == nullptr) {
            CxbxKrnlCleanup("Couldn't allocate the new stream zero buffer");
        }
    } else {
        HRESULT hRet = g_pD3DDevice->CreateVertexBuffer(
            dwHostVertexDataSize,
            D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
            0,
            D3DPOOL_DEFAULT,
            &pNewHostVertexBuffer,
            nullptr
        );

        if (FAILED(hRet)) {
            CxbxKrnlCleanup("Failed to create vertex buffer");
        }
    }

    // If we need to lock a host vertex buffer, do so now
    if (pHostVertexData == nullptr && pNewHostVertexBuffer != nullptr) {
        if (FAILED(pNewHostVertexBuffer->Lock(0, 0, (D3DLockData **)&pHostVertexData, D3DLOCK_DISCARD))) {
            CxbxKrnlCleanup("Couldn't lock vertex buffer");
        }
    }
	
	if (bNeedVertexPatching) {
	    // assert(bNeedStreamCopy || "bNeedVertexPatching implies bNeedStreamCopy (but copies via conversions");
		for (uint32_t uiVertex = 0; uiVertex < uiVertexCount; uiVertex++) {
			uint8_t *pXboxVertexAsByte = &pXboxVertexData[uiVertex * uiXboxVertexStride];
			uint8_t *pHostVertexAsByte = &pHostVertexData[uiVertex * uiHostVertexStride];
			for (UINT uiElement = 0; uiElement < pVertexShaderStreamInfo->NumberOfVertexElements; uiElement++) {
				FLOAT *pXboxVertexAsFloat = (FLOAT*)pXboxVertexAsByte;
				SHORT *pXboxVertexAsShort = (SHORT*)pXboxVertexAsByte;
				const int XboxElementByteSize = pVertexShaderStreamInfo->VertexElements[uiElement].XboxByteSize;
				FLOAT *pHostVertexAsFloat = (FLOAT*)pHostVertexAsByte;
				SHORT *pHostVertexAsShort = (SHORT*)pHostVertexAsByte;
				// Dxbx note : The following code handles only the D3DVSDT enums that need conversion;
				// All other cases are catched by the memcpy in the default-block.
				switch (pVertexShaderStreamInfo->VertexElements[uiElement].XboxType) {
				case xbox::X_D3DVSDT_NORMSHORT1: { // 0x11:
					// Test-cases : Halo - Combat Evolved
					if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT2N) {
						// Make it SHORT2N
						pHostVertexAsShort[0] = pXboxVertexAsShort[0];
						pHostVertexAsShort[1] = 0;
					} else {
						// Make it FLOAT1
						pHostVertexAsFloat[0] = NormShortToFloat(pXboxVertexAsShort[0]);
						//pHostVertexAsFloat[1] = 0.0f; // Would be needed for FLOAT2
					}
					break;
				}
				case xbox::X_D3DVSDT_NORMSHORT2: { // 0x21:
					// Test-cases : Baldur's Gate: Dark Alliance 2, F1 2002, Gun, Halo - Combat Evolved, Scrapland 
					if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT2N) {
						// No need for patching when D3D9 supports D3DDECLTYPE_SHORT2N
						// TODO : goto default; // ??
						//memcpy(pHostVertexAsByte, pXboxVertexAsByte, XboxElementByteSize);
						// Make it SHORT2N
						pHostVertexAsShort[0] = pXboxVertexAsShort[0];
						pHostVertexAsShort[1] = pXboxVertexAsShort[1];
					} else {
						// Make it FLOAT2
						pHostVertexAsFloat[0] = NormShortToFloat(pXboxVertexAsShort[0]);
						pHostVertexAsFloat[1] = NormShortToFloat(pXboxVertexAsShort[1]);
					}
					break;
				}
				case xbox::X_D3DVSDT_NORMSHORT3: { // 0x31:
					// Test-cases : Cel Damage, Constantine, Destroy All Humans!
					if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT4N) {
						// Make it SHORT4N
						pHostVertexAsShort[0] = pXboxVertexAsShort[0];
						pHostVertexAsShort[1] = pXboxVertexAsShort[1];
						pHostVertexAsShort[2] = pXboxVertexAsShort[2];
						pHostVertexAsShort[3] = 32767; // TODO : verify
					} else {
						// Make it FLOAT3
						pHostVertexAsFloat[0] = NormShortToFloat(pXboxVertexAsShort[0]);
						pHostVertexAsFloat[1] = NormShortToFloat(pXboxVertexAsShort[1]);
						pHostVertexAsFloat[2] = NormShortToFloat(pXboxVertexAsShort[2]);
					}
					break;
				}
				case xbox::X_D3DVSDT_NORMSHORT4: { // 0x41:
					// Test-cases : Judge Dredd: Dredd vs Death, NHL Hitz 2002, Silent Hill 2, Sneakers, Tony Hawk Pro Skater 4
					if (g_D3DCaps.DeclTypes & D3DDTCAPS_SHORT4N) {
						// No need for patching when D3D9 supports D3DDECLTYPE_SHORT4N
						// TODO : goto default; // ??
						//memcpy(pHostVertexAsByte, pXboxVertexAsByte, XboxElementByteSize);
						// Make it SHORT4N
						pHostVertexAsShort[0] = pXboxVertexAsShort[0];
						pHostVertexAsShort[1] = pXboxVertexAsShort[1];
						pHostVertexAsShort[2] = pXboxVertexAsShort[2];
						pHostVertexAsShort[3] = pXboxVertexAsShort[3];
					} else {
						// Make it FLOAT4
						pHostVertexAsFloat[0] = NormShortToFloat(pXboxVertexAsShort[0]);
						pHostVertexAsFloat[1] = NormShortToFloat(pXboxVertexAsShort[1]);
						pHostVertexAsFloat[2] = NormShortToFloat(pXboxVertexAsShort[2]);
						pHostVertexAsFloat[3] = NormShortToFloat(pXboxVertexAsShort[3]);
					}
					break;
				}
				case xbox::X_D3DVSDT_NORMPACKED3: { // 0x16:
					// Test-cases : Dashboard
					// Make it FLOAT3
					union {
                        int32_t value;
						struct {
							int x : 11;
							int y : 11;
							int z : 10;
						};
					} NormPacked3;

					NormPacked3.value = ((int32_t*)pXboxVertexAsByte)[0];

					pHostVertexAsFloat[0] = PackedIntToFloat(NormPacked3.x, 1023.0f, 1024.f);
					pHostVertexAsFloat[1] = PackedIntToFloat(NormPacked3.y, 1023.0f, 1024.f);
					pHostVertexAsFloat[2] = PackedIntToFloat(NormPacked3.z, 511.0f, 512.f);
					break;
				}
				case xbox::X_D3DVSDT_SHORT1: { // 0x15:
					// Make it SHORT2 and set the second short to 0
					pHostVertexAsShort[0] = pXboxVertexAsShort[0];
					pHostVertexAsShort[1] = 0;
					break;
				}
				case xbox::X_D3DVSDT_SHORT3: { // 0x35:
					// Test-cases : Turok
					// Make it a SHORT4 and set the fourth short to 1
					pHostVertexAsShort[0] = pXboxVertexAsShort[0];
					pHostVertexAsShort[1] = pXboxVertexAsShort[1];
					pHostVertexAsShort[2] = pXboxVertexAsShort[2];
					pHostVertexAsShort[3] = 1; // Turok verified (character disappears when this is 32767)
					break;
				}
				case xbox::X_D3DVSDT_PBYTE1: { // 0x14:
					if (g_D3DCaps.DeclTypes & D3DDTCAPS_UBYTE4N) {
						// Make it UBYTE4N
						pHostVertexAsByte[0] = pXboxVertexAsByte[0];
						pHostVertexAsByte[1] = 0;
						pHostVertexAsByte[2] = 0;
						pHostVertexAsByte[3] = 255; // TODO : Verify
					} else {
						// Make it FLOAT1
						pHostVertexAsFloat[0] = ByteToFloat(pXboxVertexAsByte[0]);
					}
					break;
				}
				case xbox::X_D3DVSDT_PBYTE2: { // 0x24:
					if (g_D3DCaps.DeclTypes & D3DDTCAPS_UBYTE4N) {
						// Make it UBYTE4N
						pHostVertexAsByte[0] = pXboxVertexAsByte[0];
						pHostVertexAsByte[1] = pXboxVertexAsByte[1];
						pHostVertexAsByte[2] = 0;
						pHostVertexAsByte[3] = 255; // TODO : Verify
					} else {
						// Make it FLOAT2
						pHostVertexAsFloat[0] = ByteToFloat(pXboxVertexAsByte[0]);
						pHostVertexAsFloat[1] = ByteToFloat(pXboxVertexAsByte[1]);
					}
					break;
				}
				case xbox::X_D3DVSDT_PBYTE3: { // 0x34:
					// Test-cases : Turok
					if (g_D3DCaps.DeclTypes & D3DDTCAPS_UBYTE4N) {
						// Make it UBYTE4N
						pHostVertexAsByte[0] = pXboxVertexAsByte[0];
						pHostVertexAsByte[1] = pXboxVertexAsByte[1];
						pHostVertexAsByte[2] = pXboxVertexAsByte[2];
						pHostVertexAsByte[3] = 255; // TODO : Verify
					} else {
						// Make it FLOAT3
						pHostVertexAsFloat[0] = ByteToFloat(pXboxVertexAsByte[0]);
						pHostVertexAsFloat[1] = ByteToFloat(pXboxVertexAsByte[1]);
						pHostVertexAsFloat[2] = ByteToFloat(pXboxVertexAsByte[2]);
					}
					break;
				}
				case xbox::X_D3DVSDT_PBYTE4: { // 0x44:
					// Test-case : Jet Set Radio Future
					if (g_D3DCaps.DeclTypes & D3DDTCAPS_UBYTE4N) {
						// No need for patching when D3D9 supports D3DDECLTYPE_UBYTE4N
						// TODO : goto default; // ??
						//memcpy(pHostVertexAsByte, pXboxVertexAsByte, XboxElementByteSize);
						// Make it UBYTE4N
						pHostVertexAsByte[0] = pXboxVertexAsByte[0];
						pHostVertexAsByte[1] = pXboxVertexAsByte[1];
						pHostVertexAsByte[2] = pXboxVertexAsByte[2];
						pHostVertexAsByte[3] = pXboxVertexAsByte[3];
					} else {
						// Make it FLOAT4
						pHostVertexAsFloat[0] = ByteToFloat(pXboxVertexAsByte[0]);
						pHostVertexAsFloat[1] = ByteToFloat(pXboxVertexAsByte[1]);
						pHostVertexAsFloat[2] = ByteToFloat(pXboxVertexAsByte[2]);
						pHostVertexAsFloat[3] = ByteToFloat(pXboxVertexAsByte[3]);
					}
					break;
				}
				case xbox::X_D3DVSDT_FLOAT2H: { // 0x72:
					// Make it FLOAT4 and set the third float to 0.0
					pHostVertexAsFloat[0] = pXboxVertexAsFloat[0];
					pHostVertexAsFloat[1] = pXboxVertexAsFloat[1];
					pHostVertexAsFloat[2] = 0.0f;
					pHostVertexAsFloat[3] = pXboxVertexAsFloat[2];
					break;
				}
				case xbox::X_D3DVSDT_NONE: { // 0x02:
					// Test-case : WWE RAW2
					// Test-case : PetitCopter 
					LOG_TEST_CASE("X_D3DVSDT_NONE");
					// No host element data (but Xbox size can be above zero, when used for X_D3DVSD_MASK_SKIP*
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

    patchedStream.isValid = true;
    patchedStream.XboxPrimitiveType = pDrawContext->XboxPrimitiveType;
    patchedStream.pCachedXboxVertexData = pXboxVertexData;
    patchedStream.uiCachedXboxVertexDataSize = xboxVertexDataSize;
    patchedStream.uiVertexDataHash = vertexDataHash;
    patchedStream.uiVertexStreamInformationHash = pVertexShaderSteamInfoHash;
    patchedStream.uiCachedXboxVertexStride = uiXboxVertexStride;
    patchedStream.uiCachedHostVertexStride = uiHostVertexStride;
    patchedStream.bCacheIsStreamZeroDrawUP = (pDrawContext->pXboxVertexStreamZeroData != xbox::zeroptr);
    if (patchedStream.bCacheIsStreamZeroDrawUP) {
        patchedStream.pCachedHostVertexStreamZeroData = pHostVertexData;
        patchedStream.bCachedHostVertexStreamZeroDataIsAllocated = bNeedStreamCopy;
    } else {
        // assert(pNewHostVertexBuffer != nullptr);
        pNewHostVertexBuffer->Unlock();
        patchedStream.pCachedHostVertexBuffer = pNewHostVertexBuffer;
    }

	patchedStream.Activate(pDrawContext, HostStreamNumber);
}

void CxbxVertexBufferConverter::Apply(CxbxDrawContext *pDrawContext)
{
	if ((pDrawContext->XboxPrimitiveType < xbox::X_D3DPT_POINTLIST) || (pDrawContext->XboxPrimitiveType > xbox::X_D3DPT_POLYGON))
		CxbxKrnlCleanup("Unknown primitive type: 0x%.02X\n", pDrawContext->XboxPrimitiveType);

	m_pCxbxVertexDeclaration = CxbxGetVertexDeclaration();

	// When this is an indexed draw, take the index buffer into account
	if (pDrawContext->pXboxIndexData) {
		// Is the highest index in this buffer not set yet?
		if (pDrawContext->HighIndex == 0) {
			// TODO : Instead of calling WalkIndexBuffer here, set LowIndex and HighIndex
			// in all callers that end up here (since they might be able to avoid the call)
			LOG_TEST_CASE("HighIndex == 0"); // TODO : If this is never hit, replace entire block by assert(pDrawContext->HighIndex > 0);
			WalkIndexBuffer(pDrawContext->LowIndex, pDrawContext->HighIndex, pDrawContext->pXboxIndexData, pDrawContext->dwVertexCount);
		}
		// Convert the range of indices into a count
		pDrawContext->NumVerticesToUse = pDrawContext->HighIndex - pDrawContext->LowIndex + 1;
	}
	else {
		// If we are drawing from an offset, we know that the vertex count must have
		// 'offset' vertices before the first drawn vertices
		pDrawContext->NumVerticesToUse = pDrawContext->dwVertexCount;
	}

    // Get the number of streams
    m_uiNbrStreams = GetNbrStreams(pDrawContext);
	if (m_uiNbrStreams > X_VSH_MAX_STREAMS) {
		LOG_TEST_CASE("m_uiNbrStreams count > max number of streams");
		m_uiNbrStreams = X_VSH_MAX_STREAMS;
	}

    for(UINT i = 0; i < m_uiNbrStreams; i++) {
		ConvertStream(pDrawContext, i);
    }

	if (pDrawContext->XboxPrimitiveType == xbox::X_D3DPT_QUADSTRIP) {
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
		pDrawContext->dwHostPrimitiveCount = ConvertXboxVertexCountToPrimitiveCount(xbox::X_D3DPT_TRIANGLESTRIP, pDrawContext->dwVertexCount);
	} else {
		pDrawContext->dwHostPrimitiveCount = ConvertXboxVertexCountToPrimitiveCount(pDrawContext->XboxPrimitiveType, pDrawContext->dwVertexCount);
	}

	if (pDrawContext->XboxPrimitiveType == xbox::X_D3DPT_POLYGON) {
		// Convex polygon is the same as a triangle fan.
		// No need to set : pDrawContext->XboxPrimitiveType = X_D3DPT_TRIANGLEFAN;
		// Test-case : Panzer Dragoon ORTA (when entering in-game)
		LOG_TEST_CASE("X_D3DPT_POLYGON");
	}
}

void CxbxSetVertexAttribute(int Register, FLOAT a, FLOAT b, FLOAT c, FLOAT d)
{
	if (Register < 0) {
		LOG_TEST_CASE("Register < 0");
		return;
	}
	if (Register >= 16) {
		LOG_TEST_CASE("Register >= 16");
		return;
	}

	// Write these values to the NV2A registers, so that we read them back when needed
	float* attribute_floats = HLE_get_NV2A_vertex_attribute_value_pointer(Register);
	attribute_floats[0] = a;
	attribute_floats[1] = b;
	attribute_floats[2] = c;
	attribute_floats[3] = d;

	// Also, write the given register value to a matching host vertex shader constant
	// This allows us to implement Xbox functionality where SetVertexData4f can be used to specify attributes
	// not present in the vertex declaration.
	// We use range 193 and up to store these values, as Xbox shaders stop at c192!
	g_pD3DDevice->SetVertexShaderConstantF(CXBX_D3DVS_CONSTREG_VREGDEFAULTS_BASE + Register, attribute_floats, 1);
}

DWORD Float4ToDWORD(float* floats)
{
	// Inverse of D3DDevice_SetVertexDataColor
	uint8_t a = uint8_t(floats[0] * 255.0f);
	uint8_t b = uint8_t(floats[1] * 255.0f);
	uint8_t c = uint8_t(floats[2] * 255.0f);
	uint8_t d = uint8_t(floats[3] * 255.0f);
	uint32_t value = a + (b << 8) + (c << 16) + (d << 24);
	return value;
}

void CxbxImpl_Begin(xbox::X_D3DPRIMITIVETYPE PrimitiveType)
{
	g_InlineVertexBuffer_PrimitiveType = PrimitiveType;
	g_InlineVertexBuffer_TableOffset = 0;
	g_InlineVertexBuffer_WrittenRegisters = 0;
}

void CxbxImpl_End()
{
	using namespace xbox;

#ifndef FIELD_OFFSET
#define FIELD_OFFSET(type, field)    ((LONG)(LONG_PTR)&(((type *)0)->field))
#endif
	static const LONG OffsetPerRegister[X_VSH_MAX_ATTRIBUTES] = {
		/*X_D3DVSDE_POSITION     =  0:*/FIELD_OFFSET(D3DIVB, Position),
		/*X_D3DVSDE_BLENDWEIGHT  =  1:*/FIELD_OFFSET(D3DIVB, Blend),
		/*X_D3DVSDE_NORMAL       =  2:*/FIELD_OFFSET(D3DIVB, Normal),
		/*X_D3DVSDE_DIFFUSE      =  3:*/FIELD_OFFSET(D3DIVB, Diffuse),
		/*X_D3DVSDE_SPECULAR     =  4:*/FIELD_OFFSET(D3DIVB, Specular),
		/*X_D3DVSDE_FOG          =  5:*/FIELD_OFFSET(D3DIVB, Fog),
		/*X_D3DVSDE_POINTSIZE    =  6:*/FIELD_OFFSET(D3DIVB, PointSize),
		/*X_D3DVSDE_BACKDIFFUSE  =  7:*/FIELD_OFFSET(D3DIVB, BackDiffuse),
		/*X_D3DVSDE_BACKSPECULAR =  8:*/FIELD_OFFSET(D3DIVB, BackSpecular),
		/*X_D3DVSDE_TEXCOORD0    =  9:*/FIELD_OFFSET(D3DIVB, TexCoord[0]),
		/*X_D3DVSDE_TEXCOORD1    = 10:*/FIELD_OFFSET(D3DIVB, TexCoord[1]),
		/*X_D3DVSDE_TEXCOORD2    = 11:*/FIELD_OFFSET(D3DIVB, TexCoord[2]),
		/*X_D3DVSDE_TEXCOORD3    = 12:*/FIELD_OFFSET(D3DIVB, TexCoord[3]),
		/*                         13:*/FIELD_OFFSET(D3DIVB, Reg13Up[0]),
		/*                         14:*/FIELD_OFFSET(D3DIVB, Reg13Up[1]),
		/*                         15:*/FIELD_OFFSET(D3DIVB, Reg13Up[2])
	};
	static const LONG FormatPerRegister[X_VSH_MAX_ATTRIBUTES] = {
		/*X_D3DVSDE_POSITION     =  0:*/X_D3DVSDT_FLOAT4,
		/*X_D3DVSDE_BLENDWEIGHT  =  1:*/X_D3DVSDT_FLOAT4,
		/*X_D3DVSDE_NORMAL       =  2:*/X_D3DVSDT_FLOAT3,
		/*X_D3DVSDE_DIFFUSE      =  3:*/X_D3DVSDT_D3DCOLOR,
		/*X_D3DVSDE_SPECULAR     =  4:*/X_D3DVSDT_D3DCOLOR,
		/*X_D3DVSDE_FOG          =  5:*/X_D3DVSDT_FLOAT1,
		/*X_D3DVSDE_POINTSIZE    =  6:*/X_D3DVSDT_FLOAT1,
		/*X_D3DVSDE_BACKDIFFUSE  =  7:*/X_D3DVSDT_D3DCOLOR,
		/*X_D3DVSDE_BACKSPECULAR =  8:*/X_D3DVSDT_D3DCOLOR,
		/*X_D3DVSDE_TEXCOORD0    =  9:*/X_D3DVSDT_FLOAT4,
		/*X_D3DVSDE_TEXCOORD1    = 10:*/X_D3DVSDT_FLOAT4,
		/*X_D3DVSDE_TEXCOORD2    = 11:*/X_D3DVSDT_FLOAT4,
		/*X_D3DVSDE_TEXCOORD3    = 12:*/X_D3DVSDT_FLOAT4,
		/*                         13:*/X_D3DVSDT_FLOAT4,
		/*                         14:*/X_D3DVSDT_FLOAT4,
		/*                         15:*/X_D3DVSDT_FLOAT4
	};
	static const DWORD SizePerRegister[X_VSH_MAX_ATTRIBUTES] = {
		/*X_D3DVSDE_POSITION     =  0:*/sizeof(float) * 4,
		/*X_D3DVSDE_BLENDWEIGHT  =  1:*/sizeof(float) * 4,
		/*X_D3DVSDE_NORMAL       =  2:*/sizeof(float) * 3,
		/*X_D3DVSDE_DIFFUSE      =  3:*/sizeof(DWORD),
		/*X_D3DVSDE_SPECULAR     =  4:*/sizeof(DWORD),
		/*X_D3DVSDE_FOG          =  5:*/sizeof(float) * 1,
		/*X_D3DVSDE_POINTSIZE    =  6:*/sizeof(float) * 1,
		/*X_D3DVSDE_BACKDIFFUSE  =  7:*/sizeof(DWORD),
		/*X_D3DVSDE_BACKSPECULAR =  8:*/sizeof(DWORD),
		/*X_D3DVSDE_TEXCOORD0    =  9:*/sizeof(float) * 4,
		/*X_D3DVSDE_TEXCOORD1    = 10:*/sizeof(float) * 4,
		/*X_D3DVSDE_TEXCOORD2    = 11:*/sizeof(float) * 4,
		/*X_D3DVSDE_TEXCOORD3    = 12:*/sizeof(float) * 4,
		/*                         13:*/sizeof(float) * 4,
		/*                         14:*/sizeof(float) * 4,
		/*                         15:*/sizeof(float) * 4
	};

	if (g_InlineVertexBuffer_TableOffset <= 0) {
		return;
	}

	// Compose an Xbox vertex attribute format according to the registers that have been written to :
	UINT uiStride = 0;
	g_InlineVertexBuffer_AttributeFormat = {};
	for (int reg = 0; reg < X_VSH_MAX_ATTRIBUTES; reg++) {
		if (g_InlineVertexBuffer_WrittenRegisters & (1 << reg)) {
			g_InlineVertexBuffer_AttributeFormat.Slots[reg].Format = FormatPerRegister[reg];
			g_InlineVertexBuffer_AttributeFormat.Slots[reg].Offset = uiStride;
			uiStride += SizePerRegister[reg];
		} else {
			g_InlineVertexBuffer_AttributeFormat.Slots[reg].Format = X_D3DVSDT_NONE;
		}
	}

	// Make sure the output buffer is big enough 
	UINT NeededSize = g_InlineVertexBuffer_TableOffset * uiStride;
	if (g_InlineVertexBuffer_DataSize < NeededSize) {
		g_InlineVertexBuffer_DataSize = NeededSize;
		if (g_InlineVertexBuffer_pData != nullptr) {
			free(g_InlineVertexBuffer_pData);
		}

		g_InlineVertexBuffer_pData = (FLOAT*)malloc(g_InlineVertexBuffer_DataSize);
	}

	// For each register that ever got written, copy the data over from
	// g_InlineVertexBuffer_Table to pVertexBufferData, but adjacent
	// to eachother. This, so that host accepts this as a vertex declaration.
	// Note, that if we figure out how to draw using vertex buffers that
	// contain gaps, the following copy is no longer needed, and we could
	// draw straight from g_InlineVertexBuffer_Table instead!
	uint8_t* pVertexBufferData = (uint8_t*)g_InlineVertexBuffer_pData;
	for (unsigned int v = 0; v < g_InlineVertexBuffer_TableOffset; v++) {
		auto vertex_ptr = (uint8_t*)&(g_InlineVertexBuffer_Table[v]);
		for (unsigned int reg = 0; reg < X_VSH_MAX_ATTRIBUTES; reg++) {
			if (g_InlineVertexBuffer_WrittenRegisters & (1 << reg)) {
				auto source = vertex_ptr + OffsetPerRegister[reg];
				auto size = SizePerRegister[reg];
				// Note, that g_InlineVertexBuffer_AttributeFormat is declared with
				// data types (in g_InlineVertexBuffer_Table ) that are host-compatible,
				// so we can do a straight copy here, there's no conversion needed :
				memcpy(pVertexBufferData, source, size);
				pVertexBufferData += size;
			}
		}
	}

	// Arrange for g_InlineVertexBuffer_AttributeFormat to be returned in CxbxGetVertexDeclaration,
	// so that our above composed declaration will be used for the next draw :
	g_InlineVertexBuffer_DeclarationOverride = true;
	// Note, that g_Xbox_VertexShaderMode should be left untouched,
	// because except for the declaration override, the Xbox shader (either FVF
	// or a program, or even passthrough shaders) should still be in effect!

	CxbxUpdateNativeD3DResources();

	CxbxDrawContext DrawContext = {};

	DrawContext.XboxPrimitiveType = g_InlineVertexBuffer_PrimitiveType;
	DrawContext.dwVertexCount = g_InlineVertexBuffer_TableOffset;
	DrawContext.pXboxVertexStreamZeroData = g_InlineVertexBuffer_pData;
	DrawContext.uiXboxVertexStreamZeroStride = uiStride;

	CxbxDrawPrimitiveUP(DrawContext);

	// Now that we've drawn, stop our override in CxbxGetVertexDeclaration :
	g_InlineVertexBuffer_DeclarationOverride = false;

	// TODO: Should technically clean this up at some point..but on XP doesnt matter much
	//	g_VMManager.Deallocate((VAddr)g_InlineVertexBuffer_pData);
	//	g_VMManager.Deallocate((VAddr)g_InlineVertexBuffer_Table);
}

void CxbxImpl_SetVertexData4f(int Register, FLOAT a, FLOAT b, FLOAT c, FLOAT d)
{
	using namespace xbox;

	HRESULT hRet = D3D_OK;

	// Always update our attribute storage with the most recently set register value
	CxbxSetVertexAttribute(Register, a, b, c, d);

	// Grow g_InlineVertexBuffer_Table to contain at least current, and a potentially next vertex
	if (g_InlineVertexBuffer_TableLength <= g_InlineVertexBuffer_TableOffset + 1) {
		UINT InlineVertexBuffer_TableLength_Original = g_InlineVertexBuffer_TableLength;
		if (g_InlineVertexBuffer_TableLength == 0) {
			g_InlineVertexBuffer_TableLength = PAGE_SIZE / sizeof(D3DIVB);
		} else {
			g_InlineVertexBuffer_TableLength *= 2;
		}

		for (unsigned i = 0; i < (g_InlineVertexBuffer_TableLength - InlineVertexBuffer_TableLength_Original); ++i) {
			g_InlineVertexBuffer_Table.emplace_back();
		}

		EmuLog(LOG_LEVEL::DEBUG, "Expanded g_InlineVertexBuffer_Table to %u entries", g_InlineVertexBuffer_TableLength);

		// Sanity check: ensure that g_InlineVertexBuffer_Table is not growing indefinetly. This can happen if D3DDevice_Begin and D3DDevice_End
		// are not patched, since they both reset g_InlineVertexBuffer_TableOffset back to zero, thus preventing further growth
		if (g_InlineVertexBuffer_TableLength > 50000) {
			LOG_TEST_CASE("g_InlineVertexBuffer_TableLength > 50000! This probably means that g_InlineVertexBuffer_Table is growing indefinitely.");
		}
	}

	// Is this the initial call after D3DDevice_Begin() ?
	if (g_InlineVertexBuffer_WrittenRegisters == 0) {
		// Read starting values for all inline vertex attributes from HLE NV2A pgraph (converting them to required types) :
		g_InlineVertexBuffer_Table[0].Position = D3DXVECTOR3(HLE_get_NV2A_vertex_attribute_value_pointer(X_D3DVSDE_POSITION));
		g_InlineVertexBuffer_Table[0].Rhw = HLE_get_NV2A_vertex_attribute_value_pointer(X_D3DVSDE_POSITION)[3];
		g_InlineVertexBuffer_Table[0].Blend[0] = HLE_get_NV2A_vertex_attribute_value_pointer(X_D3DVSDE_BLENDWEIGHT)[0];
		g_InlineVertexBuffer_Table[0].Blend[1] = HLE_get_NV2A_vertex_attribute_value_pointer(X_D3DVSDE_BLENDWEIGHT)[1];
		g_InlineVertexBuffer_Table[0].Blend[2] = HLE_get_NV2A_vertex_attribute_value_pointer(X_D3DVSDE_BLENDWEIGHT)[2];
		g_InlineVertexBuffer_Table[0].Blend[3] = HLE_get_NV2A_vertex_attribute_value_pointer(X_D3DVSDE_BLENDWEIGHT)[3];
		g_InlineVertexBuffer_Table[0].Normal = D3DXVECTOR3(HLE_get_NV2A_vertex_attribute_value_pointer(X_D3DVSDE_NORMAL));
		g_InlineVertexBuffer_Table[0].Diffuse = Float4ToDWORD(HLE_get_NV2A_vertex_attribute_value_pointer(X_D3DVSDE_DIFFUSE));
		g_InlineVertexBuffer_Table[0].Specular = Float4ToDWORD(HLE_get_NV2A_vertex_attribute_value_pointer(X_D3DVSDE_SPECULAR));
		g_InlineVertexBuffer_Table[0].Fog = HLE_get_NV2A_vertex_attribute_value_pointer(X_D3DVSDE_FOG)[0];
		g_InlineVertexBuffer_Table[0].PointSize = HLE_get_NV2A_vertex_attribute_value_pointer(X_D3DVSDE_POINTSIZE)[0];
		g_InlineVertexBuffer_Table[0].BackDiffuse = Float4ToDWORD(HLE_get_NV2A_vertex_attribute_value_pointer(X_D3DVSDE_BACKDIFFUSE));
		g_InlineVertexBuffer_Table[0].BackSpecular = Float4ToDWORD(HLE_get_NV2A_vertex_attribute_value_pointer(X_D3DVSDE_BACKSPECULAR));
		g_InlineVertexBuffer_Table[0].TexCoord[0] = D3DXVECTOR4(HLE_get_NV2A_vertex_attribute_value_pointer(X_D3DVSDE_TEXCOORD0));
		g_InlineVertexBuffer_Table[0].TexCoord[1] = D3DXVECTOR4(HLE_get_NV2A_vertex_attribute_value_pointer(X_D3DVSDE_TEXCOORD1));
		g_InlineVertexBuffer_Table[0].TexCoord[2] = D3DXVECTOR4(HLE_get_NV2A_vertex_attribute_value_pointer(X_D3DVSDE_TEXCOORD2));
		g_InlineVertexBuffer_Table[0].TexCoord[3] = D3DXVECTOR4(HLE_get_NV2A_vertex_attribute_value_pointer(X_D3DVSDE_TEXCOORD3));
		g_InlineVertexBuffer_Table[0].Reg13Up[0] = D3DXVECTOR4(HLE_get_NV2A_vertex_attribute_value_pointer(13));
		g_InlineVertexBuffer_Table[0].Reg13Up[1] = D3DXVECTOR4(HLE_get_NV2A_vertex_attribute_value_pointer(14));
		g_InlineVertexBuffer_Table[0].Reg13Up[2] = D3DXVECTOR4(HLE_get_NV2A_vertex_attribute_value_pointer(15));
		// Note : Because all members are assigned an initial value, there's no need for a clearing constructor for _D3DIVB!
	}

	if (Register == X_D3DVSDE_VERTEX)
		g_InlineVertexBuffer_WrittenRegisters |= (1 << X_D3DVSDE_POSITION);
	else if (Register < 16)
		g_InlineVertexBuffer_WrittenRegisters |= (1 << Register);

	unsigned o = g_InlineVertexBuffer_TableOffset;

	switch (Register)
	{
	case X_D3DVSDE_VERTEX:
	case X_D3DVSDE_POSITION:
	{
		// Note : Setting position signals completion of a vertex
		g_InlineVertexBuffer_Table[o].Position.x = a;
		g_InlineVertexBuffer_Table[o].Position.y = b;
		g_InlineVertexBuffer_Table[o].Position.z = c;
		g_InlineVertexBuffer_Table[o].Rhw = d;
		// Start a new vertex
		g_InlineVertexBuffer_TableOffset++;
		// Copy all attributes of the prior vertex to the new one, to simulate persistent attribute values
		g_InlineVertexBuffer_Table[g_InlineVertexBuffer_TableOffset] = g_InlineVertexBuffer_Table[o];
		break;
	}

	case X_D3DVSDE_BLENDWEIGHT:
	{
		g_InlineVertexBuffer_Table[o].Blend[0] = a;
		g_InlineVertexBuffer_Table[o].Blend[1] = b;
		g_InlineVertexBuffer_Table[o].Blend[2] = c;
		g_InlineVertexBuffer_Table[o].Blend[3] = d;
		break;
	}

	case X_D3DVSDE_NORMAL:
	{
		g_InlineVertexBuffer_Table[o].Normal.x = a;
		g_InlineVertexBuffer_Table[o].Normal.y = b;
		g_InlineVertexBuffer_Table[o].Normal.z = c;
		break;
	}

	case X_D3DVSDE_DIFFUSE:
	{
		g_InlineVertexBuffer_Table[o].Diffuse = D3DCOLOR_COLORVALUE(a, b, c, d);
		break;
	}

	case X_D3DVSDE_SPECULAR:
	{
		g_InlineVertexBuffer_Table[o].Specular = D3DCOLOR_COLORVALUE(a, b, c, d);
		break;
	}

	case X_D3DVSDE_FOG: // Xbox extension
	{
		g_InlineVertexBuffer_Table[o].Fog = a; // TODO : What about the other (b, c and d) arguments?
		break;
	}

	case X_D3DVSDE_POINTSIZE:
	{
		g_InlineVertexBuffer_Table[o].PointSize = a; // TODO : What about the other (b, c and d) arguments?
		break;
	}

	case X_D3DVSDE_BACKDIFFUSE: // Xbox extension
	{
		g_InlineVertexBuffer_Table[o].BackDiffuse = D3DCOLOR_COLORVALUE(a, b, c, d);
		break;
	}

	case X_D3DVSDE_BACKSPECULAR: // Xbox extension
	{
		g_InlineVertexBuffer_Table[o].BackSpecular = D3DCOLOR_COLORVALUE(a, b, c, d);
		break;
	}

	case X_D3DVSDE_TEXCOORD0:
	case X_D3DVSDE_TEXCOORD1:
	case X_D3DVSDE_TEXCOORD2:
	case X_D3DVSDE_TEXCOORD3:
	{
		unsigned i = Register - X_D3DVSDE_TEXCOORD0;
		g_InlineVertexBuffer_Table[o].TexCoord[i].x = a;
		g_InlineVertexBuffer_Table[o].TexCoord[i].y = b;
		g_InlineVertexBuffer_Table[o].TexCoord[i].z = c;
		g_InlineVertexBuffer_Table[o].TexCoord[i].w = d;
		break;
	}

	case 13:
	case 14:
	case 15:
	{
		unsigned i = Register - 13;
		g_InlineVertexBuffer_Table[o].Reg13Up[i].x = a;
		g_InlineVertexBuffer_Table[o].Reg13Up[i].y = b;
		g_InlineVertexBuffer_Table[o].Reg13Up[i].z = c;
		g_InlineVertexBuffer_Table[o].Reg13Up[i].w = d;
		break;
	}

	default:
		EmuLog(LOG_LEVEL::WARNING, "Unknown IVB Register : %d", Register);
	}
}

void CxbxImpl_SetStreamSource(UINT StreamNumber, xbox::X_D3DVertexBuffer* pStreamData, UINT Stride)
{
	if (pStreamData != xbox::zeroptr && Stride == 0) {
		LOG_TEST_CASE("CxbxImpl_SetStreamSource : Stream assigned, and stride set to 0 (might be okay)");
	}

	assert(StreamNumber < X_VSH_MAX_STREAMS);

	g_Xbox_SetStreamSource[StreamNumber].VertexBuffer = pStreamData;
	g_Xbox_SetStreamSource[StreamNumber].Stride = Stride;
}
