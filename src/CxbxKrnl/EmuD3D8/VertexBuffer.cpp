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
			&newBuffer.pHostVertexBuffer
#ifdef CXBX_USE_D3D9
			, nullptr
#endif
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
		&buffer->pHostVertexBuffer
#ifdef CXBX_USE_D3D9
		, nullptr
#endif
	);
	if (FAILED(hRet)) {
		CxbxKrnlCleanup("Failed to create vertex buffer");
	}

	*pVertexBuffer = buffer->pHostVertexBuffer;
	return true;
}

XTL::VertexPatcher::VertexPatcher()
{
    this->m_uiNbrStreams = 0;
    ZeroMemory(this->m_pStreams, sizeof(PATCHEDSTREAM) * MAX_NBR_STREAMS);
    this->m_bPatched = false;
    this->m_bAllocatedStreamZeroData = false;
    this->m_pNewVertexStreamZeroData = NULL;
    this->m_pDynamicPatch = NULL;
}

XTL::VertexPatcher::~VertexPatcher()
{
}

size_t GetVertexBufferSize(DWORD dwVertexCount, DWORD dwStride, PWORD pIndexData, DWORD dwOffset, DWORD dwIndexBase)
{	
	// If we are drawing from an offset, we know that the vertex count must have offset vertices
	// before the first drawn vertices
	dwVertexCount += dwOffset;

	// If this is not an indexed draw, the size is simply VertexCount * Stride
	if (pIndexData == nullptr) {
		return dwVertexCount * dwStride;
	}

	// We are an indexed draw, so we have to parse the index buffer
	// The highest index we see can be used to determine the vertex buffer size
	DWORD highestVertexIndex = 0;
	for (DWORD i = 0; i < dwVertexCount; i++) {
		if (pIndexData[i] > highestVertexIndex) {
			highestVertexIndex = pIndexData[i];
		}
	}

	return (highestVertexIndex + dwIndexBase + 1) * dwStride;		
}

int CountActiveD3DStreams()
{
	int lastStreamIndex = -1;
	for (int i = 0; i < 16; i++) {
		if (g_D3DStreams[i] != nullptr) {
			lastStreamIndex = i;
		}
	}

	return lastStreamIndex + 1;
}

UINT XTL::VertexPatcher::GetNbrStreams(VertexPatchDesc *pPatchDesc)
{
	// Draw..Up always have one stream
	if (pPatchDesc->pXboxVertexStreamZeroData != nullptr) {
		return 1;
	}

    if(VshHandleIsVertexShader(pPatchDesc->hVertexShader)) {
        VERTEX_DYNAMIC_PATCH *pDynamicPatch = VshGetVertexDynamicPatch(pPatchDesc->hVertexShader);
		if (pDynamicPatch) {
			return pDynamicPatch->NbrStreams;
		}

		return CountActiveD3DStreams();
    } 
	
	if (pPatchDesc->hVertexShader) {
		return CountActiveD3DStreams();
    }

    return 0;
}

bool XTL::VertexPatcher::PatchStream(VertexPatchDesc *pPatchDesc,
                                     UINT             uiStream)
{
    // FVF buffers doesn't have Xbox extensions, but texture coordinates may
    // need normalization if used with linear textures.
    if(VshHandleIsFVF(pPatchDesc->hVertexShader))
    {
        if(pPatchDesc->hVertexShader & D3DFVF_TEXCOUNT_MASK)
        {
            return NormalizeTexCoords(pPatchDesc, uiStream);
        }
        else
        {
            return false;
        }
    }

    if(!m_pDynamicPatch || m_pDynamicPatch->NbrStreams <= uiStream || !m_pDynamicPatch->pStreamPatches[uiStream].NeedPatch)
    {
        return false;
    }

    // Do some groovy patchin'
    
    XTL::X_D3DVertexBuffer    *pOrigVertexBuffer;
    IDirect3DVertexBuffer     *pNewVertexBuffer;
    uint08                    *pOrigData;
    uint08                    *pNewData;
	UINT                       uiVertexCount;
    UINT                       uiStride;
	UINT					   uiLength;
    PATCHEDSTREAM             *pStream = &m_pStreams[uiStream];
    STREAM_DYNAMIC_PATCH      *pStreamPatch = &m_pDynamicPatch->pStreamPatches[uiStream];
    DWORD dwNewSize;

    if(!pPatchDesc->pXboxVertexStreamZeroData)
    {
		pOrigVertexBuffer = g_D3DStreams[uiStream];
		uiStride = g_D3DStreamStrides[uiStream];
		uiLength = pPatchDesc->uiSize;

        // Set a new (exact) vertex count
		uiVertexCount = uiLength / uiStride;

		// Dxbx addition : Don't update pPatchDesc.dwVertexCount because an indexed draw
		// can (and will) use less vertices than the supplied nr of indexes. Thix fixes
		// the missing parts in the CompressedVertices sample (in Vertex shader mode).
		pStreamPatch->ConvertedStride = std::max((uint32_t)pStreamPatch->ConvertedStride, (uint32_t)uiStride); // ??
		dwNewSize = uiVertexCount * pStreamPatch->ConvertedStride;

        pOrigData = (uint08*)GetDataFromXboxResource(pOrigVertexBuffer);
		GetCachedVertexBufferObject(pOrigVertexBuffer->Data, dwNewSize, &pNewVertexBuffer);

        if(FAILED(pNewVertexBuffer->Lock(0, 0, &pNewData, D3DLOCK_DISCARD)))
        {
            CxbxKrnlCleanup("Couldn't lock the new buffer");
        }
        if(!pStream->pOriginalStream)
        {
            // The stream was not previously patched, we'll need this when restoring
            pStream->pOriginalStream = pOrigVertexBuffer;
        }
    }
    else
    {
        // There should only be one stream (stream zero) in this case
        if(uiStream != 0)
        {
            CxbxKrnlCleanup("Trying to patch a Draw..UP with more than stream zero!");
        }
        uiStride  = pPatchDesc->uiXboxVertexStreamZeroStride;
		pStreamPatch->ConvertedStride = std::max((uint32_t)pStreamPatch->ConvertedStride, (uint32_t)uiStride); // ??
		pOrigData = (uint08 *)pPatchDesc->pXboxVertexStreamZeroData;
		uiLength = pPatchDesc->uiSize;
		uiVertexCount = uiLength / uiStride;
		dwNewSize = uiVertexCount * pStreamPatch->ConvertedStride;
        pNewVertexBuffer = NULL;
        pNewData = (uint08*)g_VMManager.Allocate(dwNewSize);
        if(!pNewData)
        {
            CxbxKrnlCleanup("Couldn't allocate the new stream zero buffer");
        }
    }

	for (uint32 uiVertex = 0; uiVertex < uiVertexCount; uiVertex++)
	{
		uint08 *pOrigVertex = &pOrigData[uiVertex * uiStride];
		uint08 *pNewDataPos = &pNewData[uiVertex * pStreamPatch->ConvertedStride];
		for (UINT uiType = 0; uiType < pStreamPatch->NbrTypes; uiType++)
		{
			// Dxbx note : The following code handles only the D3DVSDT enums that need conversion;
			// All other cases are catched by the memcpy in the default-block.
			switch (pStreamPatch->pTypes[uiType])
			{
			case X_D3DVSDT_NORMPACKED3: { // 0x16: // Make it FLOAT3
				// Hit by Dashboard
				int32 iPacked = ((int32 *)pOrigVertex)[0];
				// Cxbx note : to make each component signed, two need to be shifted towards the sign-bit first :
				((FLOAT *)pNewDataPos)[0] = ((FLOAT)((iPacked << 21) >> 21)) / 1023.0f;
				((FLOAT *)pNewDataPos)[1] = ((FLOAT)((iPacked << 10) >> 21)) / 1023.0f;
				((FLOAT *)pNewDataPos)[2] = ((FLOAT)((iPacked      ) >> 22)) / 511.0f;
				pOrigVertex += 1 * sizeof(int32);
				break;
			}
			case X_D3DVSDT_SHORT1: { // 0x15: // Make it SHORT2 and set the second short to 0
				((SHORT *)pNewDataPos)[0] = ((SHORT*)pOrigVertex)[0];
				((SHORT *)pNewDataPos)[1] = 0x00;
				pOrigVertex += 1 * sizeof(SHORT);
				break;
			}
			case X_D3DVSDT_SHORT3: { // 0x35: // Make it a SHORT4 and set the fourth short to 1
				// Hit by Turok
				memcpy(pNewDataPos, pOrigVertex, 3 * sizeof(SHORT));
				((SHORT *)pNewDataPos)[3] = 0x01;
				pOrigVertex += 3 * sizeof(SHORT);
				break;
			}
			case X_D3DVSDT_PBYTE1: { // 0x14:  // Make it FLOAT1
				((FLOAT *)pNewDataPos)[0] = ((FLOAT)((BYTE*)pOrigVertex)[0]) / 255.0f;
				pOrigVertex += 1 * sizeof(BYTE);
				break;
			}
			case X_D3DVSDT_PBYTE2: { // 0x24:  // Make it FLOAT2
				((FLOAT *)pNewDataPos)[0] = ((FLOAT)((BYTE*)pOrigVertex)[0]) / 255.0f;
				((FLOAT *)pNewDataPos)[1] = ((FLOAT)((BYTE*)pOrigVertex)[1]) / 255.0f;
				pOrigVertex += 2 * sizeof(BYTE);
				break;
			}
			case X_D3DVSDT_PBYTE3: { // 0x34: // Make it FLOAT3
				// Hit by Turok
				((FLOAT *)pNewDataPos)[0] = ((FLOAT)((BYTE*)pOrigVertex)[0]) / 255.0f;
				((FLOAT *)pNewDataPos)[1] = ((FLOAT)((BYTE*)pOrigVertex)[1]) / 255.0f;
				((FLOAT *)pNewDataPos)[2] = ((FLOAT)((BYTE*)pOrigVertex)[2]) / 255.0f;
				pOrigVertex += 3 * sizeof(BYTE);
				break;
			}
			case X_D3DVSDT_PBYTE4: { // 0x44: // Make it FLOAT4
				// Hit by Jet Set Radio Future
				((FLOAT *)pNewDataPos)[0] = ((FLOAT)((BYTE*)pOrigVertex)[0]) / 255.0f;
				((FLOAT *)pNewDataPos)[1] = ((FLOAT)((BYTE*)pOrigVertex)[1]) / 255.0f;
				((FLOAT *)pNewDataPos)[2] = ((FLOAT)((BYTE*)pOrigVertex)[2]) / 255.0f;
				((FLOAT *)pNewDataPos)[3] = ((FLOAT)((BYTE*)pOrigVertex)[3]) / 255.0f;
				pOrigVertex += 4 * sizeof(BYTE);
				break;
			}
			case X_D3DVSDT_NORMSHORT1: { // 0x11: // Make it FLOAT1
				// UNTESTED - Need test-case!
				((FLOAT *)pNewDataPos)[0] = ((FLOAT)((SHORT*)pOrigVertex)[0]) / 32767.0f;
				//((FLOAT *)pNewDataPos)[1] = 0.0f; // Would be needed for FLOAT2
				pOrigVertex += 1 * sizeof(SHORT);
				break;
			}
#if !CXBX_USE_D3D9 // No need for patching in D3D9
			case X_D3DVSDT_NORMSHORT2: { // 0x21: // Make it FLOAT2
				// UNTESTED - Need test-case!
				((FLOAT *)pNewDataPos)[0] = ((FLOAT)((SHORT*)pOrigVertex)[0]) / 32767.0f;
				((FLOAT *)pNewDataPos)[1] = ((FLOAT)((SHORT*)pOrigVertex)[1]) / 32767.0f;
				pOrigVertex += 2 * sizeof(SHORT);
				break;
			}
#endif
			case X_D3DVSDT_NORMSHORT3: { // 0x31: // Make it FLOAT3
				// UNTESTED - Need test-case!
				((FLOAT *)pNewDataPos)[0] = ((FLOAT)((SHORT*)pOrigVertex)[0]) / 32767.0f;
				((FLOAT *)pNewDataPos)[1] = ((FLOAT)((SHORT*)pOrigVertex)[1]) / 32767.0f;
				((FLOAT *)pNewDataPos)[2] = ((FLOAT)((SHORT*)pOrigVertex)[2]) / 32767.0f;
				pOrigVertex += 3 * sizeof(SHORT);
				break;
			}
#if !CXBX_USE_D3D9 // No need for patching in D3D9
			case X_D3DVSDT_NORMSHORT4: { // 0x41: // Make it FLOAT4
				// UNTESTED - Need test-case!
				((FLOAT *)pNewDataPos)[0] = ((FLOAT)((SHORT*)pOrigVertex)[0]) / 32767.0f;
				((FLOAT *)pNewDataPos)[1] = ((FLOAT)((SHORT*)pOrigVertex)[1]) / 32767.0f;
				((FLOAT *)pNewDataPos)[2] = ((FLOAT)((SHORT*)pOrigVertex)[2]) / 32767.0f;
				((FLOAT *)pNewDataPos)[3] = ((FLOAT)((SHORT*)pOrigVertex)[3]) / 32767.0f;
				pOrigVertex += 4 * sizeof(SHORT);
				break;
			}
#endif
			case X_D3DVSDT_FLOAT2H: { // 0x72: // Make it FLOAT4 and set the third float to 0.0
				((FLOAT *)pNewDataPos)[0] = ((FLOAT*)pOrigVertex)[0];
				((FLOAT *)pNewDataPos)[1] = ((FLOAT*)pOrigVertex)[1];
				((FLOAT *)pNewDataPos)[2] = 0.0f;
				((FLOAT *)pNewDataPos)[3] = ((FLOAT*)pOrigVertex)[2];
				pOrigVertex += 3 * sizeof(FLOAT);
				break;
			}
			/*TODO
			case X_D3DVSDT_NONE: { // 0x02:
				printf("D3DVSDT_NONE / xbox ext. nsp /");
				dwNewDataType = 0xFF;
				break;
			}
			*/
			default: {
				// Generic 'conversion' - just make a copy :
				memcpy(pNewDataPos, pOrigVertex, pStreamPatch->pSizes[uiType]);
				pOrigVertex += pStreamPatch->pSizes[uiType];
				break;
			}
			} // switch

			// Increment the new pointer :
			pNewDataPos += pStreamPatch->pSizes[uiType];
		}
	}

    if(!pPatchDesc->pXboxVertexStreamZeroData)
    {
        //if(pNewVertexBuffer != nullptr) // Dxbx addition
			pNewVertexBuffer->Unlock();

        HRESULT hr = g_pD3DDevice->SetStreamSource(
			uiStream, 
			pNewVertexBuffer, 
#ifdef CXBX_USE_D3D9
			0, // OffsetInBytes
#endif
			pStreamPatch->ConvertedStride);
		if (FAILED(hr))
        {
            CxbxKrnlCleanup("Failed to set the type patched buffer as the new stream source!\n");
        }

        pStream->pPatchedStream = pNewVertexBuffer;
    }
    else
    {
        pPatchDesc->pXboxVertexStreamZeroData = pNewData;
        pPatchDesc->uiXboxVertexStreamZeroStride = pStreamPatch->ConvertedStride;
        if(!m_bAllocatedStreamZeroData)
        {
            // The stream was not previously patched. We'll need this when restoring
            m_bAllocatedStreamZeroData = true;
            m_pNewVertexStreamZeroData = pNewData;
        }
    }

    pStream->uiOrigStride = uiStride;
    pStream->uiNewStride = pStreamPatch->ConvertedStride;
    m_bPatched = true;

    return true;
}

bool XTL::VertexPatcher::NormalizeTexCoords(VertexPatchDesc *pPatchDesc, UINT uiStream)
{
    // Check for active linear textures.
	bool bHasLinearTex = false, bTexIsLinear[4] = { false };
	X_D3DBaseTexture *pLinearBaseTexture[4];

    for(uint08 i = 0; i < 4; i++)
    {
		X_D3DBaseTexture *pBaseTexture = EmuD3DActiveTexture[i];
		if (pBaseTexture)
		{ 
			XTL::X_D3DFORMAT XBFormat = (XTL::X_D3DFORMAT)((pBaseTexture->Format & X_D3DFORMAT_FORMAT_MASK) >> X_D3DFORMAT_FORMAT_SHIFT);
			if (EmuXBFormatIsLinear(XBFormat))
			{
				bHasLinearTex = bTexIsLinear[i] = true;
				pLinearBaseTexture[i] = pBaseTexture;
			}
        }
    }

    if(!bHasLinearTex)
        return false;

    XTL::X_D3DVertexBuffer *pOrigVertexBuffer;
    IDirect3DVertexBuffer *pNewVertexBuffer;
    PATCHEDSTREAM *pStream;
    uint08 *pData, *pUVData;
    uint uiStride, uiVertexCount;

    if(pPatchDesc->pXboxVertexStreamZeroData)
    {
        // In-place patching of inline buffer.
        pNewVertexBuffer = 0;
        pData = (uint08 *)pPatchDesc->pXboxVertexStreamZeroData;
        uiStride = pPatchDesc->uiXboxVertexStreamZeroStride;
		DWORD uiLength = pPatchDesc->uiSize;
        uiVertexCount = uiLength / uiStride;
    }
    else
    {
        // Copy stream for patching and caching.
		pOrigVertexBuffer = g_D3DStreams[uiStream];
		uiStride = g_D3DStreamStrides[uiStream];
		UINT uiLength = pPatchDesc->uiSize;

		uiVertexCount = uiLength / uiStride;

		uint08 *pOrigData = (uint08*)GetDataFromXboxResource(pOrigVertexBuffer);

		GetCachedVertexBufferObject(pOrigVertexBuffer->Data, uiLength, &pNewVertexBuffer);
        if(FAILED(pNewVertexBuffer->Lock(0, 0, &pData, D3DLOCK_DISCARD)))
        {
            CxbxKrnlCleanup("Couldn't lock new FVF buffer.");
        }
        memcpy(pData, pOrigData, uiLength);

        pStream = &m_pStreams[uiStream];
        if(!pStream->pOriginalStream)
        {
            pStream->pOriginalStream = pOrigVertexBuffer;
        }
    }

    // Locate texture coordinate offset in vertex structure.
    uint uiOffset = 0;
    if(pPatchDesc->hVertexShader & D3DFVF_XYZRHW)
        uiOffset += (sizeof(FLOAT) * 4);
    else
    {
        if(pPatchDesc->hVertexShader & D3DFVF_XYZ)
            uiOffset += (sizeof(FLOAT) * 3 );
        else if(pPatchDesc->hVertexShader & D3DFVF_XYZB1)
            uiOffset += (sizeof(FLOAT) *4 );
        else if(pPatchDesc->hVertexShader & D3DFVF_XYZB2)
            uiOffset += (sizeof(FLOAT) * 5);
        else if(pPatchDesc->hVertexShader & D3DFVF_XYZB3)
            uiOffset += (sizeof(FLOAT) * 6);
        else if(pPatchDesc->hVertexShader & D3DFVF_XYZB4)
            uiOffset += (sizeof(FLOAT) * 7);

        if(pPatchDesc->hVertexShader & D3DFVF_NORMAL)
            uiOffset += (sizeof(FLOAT) * 3);
    }

    if(pPatchDesc->hVertexShader & D3DFVF_DIFFUSE)
        uiOffset += sizeof(DWORD);
    if(pPatchDesc->hVertexShader & D3DFVF_SPECULAR)
        uiOffset += sizeof(DWORD);

    DWORD dwTexN = (pPatchDesc->hVertexShader & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;

    // Normalize texture coordinates.
    for(uint32 uiVertex = 0; uiVertex < uiVertexCount; uiVertex++)
    {
        pUVData = pData + (uiVertex * uiStride) + uiOffset;

        if(dwTexN >= 1)
        {
            if(bTexIsLinear[0])
            {
                ((FLOAT*)pUVData)[0] /= ( pLinearBaseTexture[0]->Size & X_D3DSIZE_WIDTH_MASK) + 1;
                ((FLOAT*)pUVData)[1] /= ((pLinearBaseTexture[0]->Size & X_D3DSIZE_HEIGHT_MASK) >> X_D3DSIZE_HEIGHT_SHIFT) + 1;
            }
            pUVData += sizeof(FLOAT) * 2;
        }

        if(dwTexN >= 2)
        {
            if(bTexIsLinear[1])
            {
                ((FLOAT*)pUVData)[0] /= ( pLinearBaseTexture[1]->Size & X_D3DSIZE_WIDTH_MASK) + 1;
                ((FLOAT*)pUVData)[1] /= ((pLinearBaseTexture[1]->Size & X_D3DSIZE_HEIGHT_MASK) >> X_D3DSIZE_HEIGHT_SHIFT) + 1;
            }
            pUVData += sizeof(FLOAT) * 2;
        }

        if(dwTexN >= 3)
        {
            if(bTexIsLinear[2])
            {
                ((FLOAT*)pUVData)[0] /= ( pLinearBaseTexture[2]->Size & X_D3DSIZE_WIDTH_MASK) + 1;
                ((FLOAT*)pUVData)[1] /= ((pLinearBaseTexture[2]->Size & X_D3DSIZE_HEIGHT_MASK) >> X_D3DSIZE_HEIGHT_SHIFT) + 1;
            }
            pUVData += sizeof(FLOAT) * 2;
        }

        if((dwTexN >= 4) && bTexIsLinear[3])
        {
            ((FLOAT*)pUVData)[0] /= ( pLinearBaseTexture[3]->Size & X_D3DSIZE_WIDTH_MASK) + 1;
            ((FLOAT*)pUVData)[1] /= ((pLinearBaseTexture[3]->Size & X_D3DSIZE_HEIGHT_MASK) >> X_D3DSIZE_HEIGHT_SHIFT) + 1;
        }
    }

    if(pNewVertexBuffer)
    {
        pNewVertexBuffer->Unlock();

        HRESULT hr = g_pD3DDevice->SetStreamSource(
			uiStream, 
			pNewVertexBuffer, 
#ifdef CXBX_USE_D3D9
			0, // OffsetInBytes
#endif
			uiStride);
		if (FAILED(hr))
        {
            CxbxKrnlCleanup("Failed to set the texcoord patched FVF buffer as the new stream source.");
        }

        pStream->pPatchedStream = pNewVertexBuffer;
        pStream->uiOrigStride = uiStride;
        pStream->uiNewStride = uiStride;
        m_bPatched = true;
    }

    return m_bPatched;
}

bool XTL::VertexPatcher::Apply(VertexPatchDesc *pPatchDesc, bool *pbFatalError)
{
	CxbxDrawContext *pDrawContext = pPatchDesc; // Temporary alias to help porting over code from PatrickvL WIP_LessVertexPatching
	if ((pDrawContext->XboxPrimitiveType < X_D3DPT_POINTLIST) || (pDrawContext->XboxPrimitiveType > X_D3DPT_POLYGON))
		CxbxKrnlCleanup("Unknown primitive type: 0x%.02X\n", pDrawContext->XboxPrimitiveType);

    bool Patched = false;
    // Get the number of streams
    m_uiNbrStreams = GetNbrStreams(pPatchDesc);

    if(VshHandleIsVertexShader(pPatchDesc->hVertexShader))
    {
        m_pDynamicPatch = &((VERTEX_SHADER *)VshHandleGetVertexShader(pPatchDesc->hVertexShader)->Handle)->VertexDynamicPatch;
    }

    for(UINT uiStream = 0; uiStream < m_uiNbrStreams; uiStream++)
    {
        bool LocalPatched = false;
		pPatchDesc->uiSize = GetVertexBufferSize(
			pPatchDesc->dwVertexCount, 
			pPatchDesc->pXboxVertexStreamZeroData == nullptr ? g_D3DStreamStrides[uiStream] : pPatchDesc->uiXboxVertexStreamZeroStride,
			pPatchDesc->pIndexData, 
			pPatchDesc->dwStartVertex,
			pPatchDesc->dwIndexBase
		);
	
		// TODO: Check for cached vertex buffer, and use it if possible
		
        LocalPatched |= PatchStream(pPatchDesc, uiStream);
        Patched |= LocalPatched;

		if (!Patched && pPatchDesc->pXboxVertexStreamZeroData == nullptr) {
			// Fetch or Create the host Vertex Buffer
			XTL::IDirect3DVertexBuffer* pHostVertexBuffer;
			GetCachedVertexBufferObject(g_D3DStreams[uiStream]->Data, pPatchDesc->uiSize, &pHostVertexBuffer);

			// Copy xbox data to the host vertex buffer
			BYTE* pVertexDataData;
			if (FAILED(pHostVertexBuffer->Lock(0, 0, &pVertexDataData, D3DLOCK_DISCARD))) {
				CxbxKrnlCleanup("Couldn't lock Vertex Buffer");
			}

			memcpy(pVertexDataData, GetDataFromXboxResource(g_D3DStreams[uiStream]), pPatchDesc->uiSize);
			pHostVertexBuffer->Unlock();
			
			// Set the buffer as a stream source
			g_pD3DDevice->SetStreamSource(
				uiStream, 
				pHostVertexBuffer, 
#ifdef CXBX_USE_D3D9
				0, // OffsetInBytes
#endif
				g_D3DStreamStrides[uiStream]);
		}

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
		LOG_TEST_CASE("X_D3DPT_POLYGON");
	}

    return Patched;
}

VOID XTL::EmuFlushIVB()
{
    XTL::EmuUpdateDeferredStates();
	EmuUpdateActiveTextureStages();

    // Parse IVB table with current FVF shader if possible.
    bool bFVF = VshHandleIsFVF(g_CurrentXboxVertexShaderHandle);
    DWORD dwCurFVF = (bFVF) ? g_CurrentXboxVertexShaderHandle : g_InlineVertexBuffer_FVF;

    DbgPrintf("g_InlineVertexBuffer_TableOffset := %d\n", g_InlineVertexBuffer_TableOffset);

	// Check the given FVF
	switch (dwCurFVF & D3DFVF_POSITION_MASK) {
	case 0: // No position ?
		if (bFVF) {
			EmuWarning("EmuFlushIVB(): g_CurrentVertexShader isn't a valid FVF - using D3DFVF_XYZRHW instead!");
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
		TexSize[i] = DxbxFVF_GetTextureSize(dwCurFVF, i);
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

    DWORD *pdwVB = (DWORD*)g_InlineVertexBuffer_pData;
	for(uint v=0;v<g_InlineVertexBuffer_TableOffset;v++)
    {
		switch (dwPos) {
        case D3DFVF_XYZ:
            *(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Position.x;
            *(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Position.y;
            *(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Position.z;
            DbgPrintf("IVB Position := {%f, %f, %f}\n", g_InlineVertexBuffer_Table[v].Position.x, g_InlineVertexBuffer_Table[v].Position.y, g_InlineVertexBuffer_Table[v].Position.z);
			break;
		case D3DFVF_XYZRHW:
            *(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Position.x;
            *(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Position.y;
            *(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Position.z;
            *(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Rhw;
            DbgPrintf("IVB Position := {%f, %f, %f, %f, %f}\n", g_InlineVertexBuffer_Table[v].Position.x, g_InlineVertexBuffer_Table[v].Position.y, g_InlineVertexBuffer_Table[v].Position.z, g_InlineVertexBuffer_Table[v].Position.z, g_InlineVertexBuffer_Table[v].Rhw);
			break;
		case D3DFVF_XYZB1:
            *(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Position.x;
            *(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Position.y;
            *(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Position.z;
			*(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Blend[0];
			DbgPrintf("IVB Position := {%f, %f, %f, %f}\n", g_InlineVertexBuffer_Table[v].Position.x, g_InlineVertexBuffer_Table[v].Position.y, g_InlineVertexBuffer_Table[v].Position.z, g_InlineVertexBuffer_Table[v].Blend[0]);
			break;
		case D3DFVF_XYZB2:
			*(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Position.x;
			*(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Position.y;
			*(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Position.z;
			*(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Blend[0];
			*(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Blend[1];
			DbgPrintf("IVB Position := {%f, %f, %f, %f, %f}\n", g_InlineVertexBuffer_Table[v].Position.x, g_InlineVertexBuffer_Table[v].Position.y, g_InlineVertexBuffer_Table[v].Position.z, g_InlineVertexBuffer_Table[v].Blend[0], g_InlineVertexBuffer_Table[v].Blend[1]);
			break;
		case D3DFVF_XYZB3:
			*(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Position.x;
			*(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Position.y;
			*(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Position.z;
			*(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Blend[0];
			*(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Blend[1];
			*(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Blend[2];
			DbgPrintf("IVB Position := {%f, %f, %f, %f, %f, %f}\n", g_InlineVertexBuffer_Table[v].Position.x, g_InlineVertexBuffer_Table[v].Position.y, g_InlineVertexBuffer_Table[v].Position.z, g_InlineVertexBuffer_Table[v].Blend[0], g_InlineVertexBuffer_Table[v].Blend[1], g_InlineVertexBuffer_Table[v].Blend[2]);
			break;
		case D3DFVF_XYZB4:
			*(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Position.x;
			*(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Position.y;
			*(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Position.z;
			*(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Blend[0];
			*(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Blend[1];
			*(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Blend[2];
			*(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Blend[3];
			DbgPrintf("IVB Position := {%f, %f, %f, %f, %f, %f, %f}\n", g_InlineVertexBuffer_Table[v].Position.x, g_InlineVertexBuffer_Table[v].Position.y, g_InlineVertexBuffer_Table[v].Position.z, g_InlineVertexBuffer_Table[v].Blend[0], g_InlineVertexBuffer_Table[v].Blend[1], g_InlineVertexBuffer_Table[v].Blend[2], g_InlineVertexBuffer_Table[v].Blend[3]);
			break;
		default:
			CxbxKrnlCleanup("Unsupported Position Mask (FVF := 0x%.08X dwPos := 0x%.08X)", dwCurFVF, dwPos);
			break;
		}

		if (dwCurFVF & D3DFVF_NORMAL) {
			*(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Normal.x;
			*(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Normal.y;
			*(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].Normal.z;
			DbgPrintf("IVB Normal := {%f, %f, %f}\n", g_InlineVertexBuffer_Table[v].Normal.x, g_InlineVertexBuffer_Table[v].Normal.y, g_InlineVertexBuffer_Table[v].Normal.z);
        }

        if (dwCurFVF & D3DFVF_DIFFUSE) {
            *(DWORD*)pdwVB++ = g_InlineVertexBuffer_Table[v].Diffuse;
            DbgPrintf("IVB Diffuse := 0x%.08X\n", g_InlineVertexBuffer_Table[v].Diffuse);
        }

		if (dwCurFVF & D3DFVF_SPECULAR) {
			*(DWORD*)pdwVB++ = g_InlineVertexBuffer_Table[v].Specular;
			DbgPrintf("IVB Specular := 0x%.08X\n", g_InlineVertexBuffer_Table[v].Specular);
		}

		for (uint i = 0; i < dwTexN; i++) {
            *(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].TexCoord[i].x;
			if (TexSize[i] >= 2) {
				*(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].TexCoord[i].y;
				if (TexSize[i] >= 3) {
					*(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].TexCoord[i].z;
					if (TexSize[i] >= 4) {
						*(FLOAT*)pdwVB++ = g_InlineVertexBuffer_Table[v].TexCoord[i].w;
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
			uint VertexBufferUsage = (uintptr_t)pdwVB - (uintptr_t)g_InlineVertexBuffer_pData;
			if (VertexBufferUsage != uiStride) {
				CxbxKrnlCleanup("EmuFlushIVB uses wrong stride!");
			}
		}
	}

	VertexPatchDesc VPDesc = {};
    VPDesc.XboxPrimitiveType = g_InlineVertexBuffer_PrimitiveType;
    VPDesc.dwVertexCount = g_InlineVertexBuffer_TableOffset;
    VPDesc.pXboxVertexStreamZeroData = g_InlineVertexBuffer_pData;
    VPDesc.uiXboxVertexStreamZeroStride = uiStride;
    VPDesc.hVertexShader = g_CurrentXboxVertexShaderHandle;

    VertexPatcher VertPatch;

    bool bPatched = VertPatch.Apply(&VPDesc, NULL);

    if(bFVF)
    {
#ifdef CXBX_USE_D3D9
        g_pD3DDevice->SetVertexShader(nullptr);
        g_pD3DDevice->SetFVF(dwCurFVF);
#else
        g_pD3DDevice->SetVertexShader(dwCurFVF);
#endif
    }

    g_pD3DDevice->DrawPrimitiveUP(
		EmuXB2PC_D3DPrimitiveType(VPDesc.XboxPrimitiveType),
        VPDesc.dwHostPrimitiveCount,
        VPDesc.pXboxVertexStreamZeroData,
        VPDesc.uiXboxVertexStreamZeroStride);

	g_dwPrimPerFrame += VPDesc.dwHostPrimitiveCount;

    if(bFVF)
    {
#ifdef CXBX_USE_D3D9
		g_pD3DDevice->SetVertexShader(nullptr);
		g_pD3DDevice->SetFVF(g_CurrentXboxVertexShaderHandle);
#else
        g_pD3DDevice->SetVertexShader(g_CurrentXboxVertexShaderHandle);
#endif
    }

    g_InlineVertexBuffer_TableOffset = 0;

    return;
}
