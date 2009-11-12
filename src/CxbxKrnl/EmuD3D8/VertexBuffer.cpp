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
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

#include "CxbxKrnl/Emu.h"
#include "CxbxKrnl/EmuAlloc.h"
#include "CxbxKrnl/EmuXTL.h"
#include "CxbxKrnl/ResourceTracker.h"

#include <ctime>

#define VERTEX_BUFFER_CACHE_SIZE 64
#define MAX_STREAM_NOT_USED_TIME (2 * CLOCKS_PER_SEC) // TODO: Trim the not used time

// inline vertex buffer emulation
XTL::DWORD                  *XTL::g_pIVBVertexBuffer = 0;
XTL::X_D3DPRIMITIVETYPE      XTL::g_IVBPrimitiveType = XTL::X_D3DPT_INVALID;
UINT                         XTL::g_IVBTblOffs = 0;
struct XTL::_D3DIVB         *XTL::g_IVBTable = 0;
extern DWORD                 XTL::g_IVBFVF = 0;

static unsigned int crctab[256];

static void CRC32Init(void)
{
    static boolean bFirstTime = true;
    int i, j;

    unsigned int crc;

    if(!bFirstTime)
    {
        return;
    }
    for(i = 0; i < 256; i++)
    {
        crc = i << 24;
        for(j = 0; j < 8; j++)
        {
            if(crc & 0x80000000)
                crc = (crc << 1) ^ 0x04c11db7;
            else
                crc = crc << 1;
        }
        crctab[i] = crc;
    }
    bFirstTime = false;
}

static unsigned int CRC32(unsigned char *data, int len)
{
    unsigned int        result;
    int                 i;;

    if(len < 4) abort();

    result = *data++ << 24;
    result |= *data++ << 16;
    result |= *data++ << 8;
    result |= *data++;
    result = ~ result;
    len -=4;

    for(i=0; i<len; i++)
    {
        result = (result << 8 | *data++) ^ crctab[result >> 24];
    }

    return ~result;
}

XTL::VertexPatcher::VertexPatcher()
{
    this->m_uiNbrStreams = 0;
    ZeroMemory(this->m_pStreams, sizeof(PATCHEDSTREAM) * MAX_NBR_STREAMS);
    this->m_bPatched = false;
    this->m_bAllocatedStreamZeroData = false;
    this->m_pNewVertexStreamZeroData = NULL;
    this->m_pDynamicPatch = NULL;
    CRC32Init();
}

XTL::VertexPatcher::~VertexPatcher()
{
}

void XTL::VertexPatcher::DumpCache(void)
{
    printf("--- Dumping streams cache ---\n");
    RTNode *pNode = g_PatchedStreamsCache.getHead();
    while(pNode)
    {
        CACHEDSTREAM *pCachedStream = (CACHEDSTREAM *)pNode->pResource;
        if(pCachedStream)
        {
            // TODO: Write nicer dump presentation
            printf("Key: 0x%.08X Cache Hits: %d IsUP: %s OrigStride: %d NewStride: %d CRCCount: %d CRCFreq: %d Lengh: %d CRC32: 0x%.08X\n",
                   pNode->uiKey, pCachedStream->uiCacheHit, pCachedStream->bIsUP ? "YES" : "NO",
                   pCachedStream->Stream.uiOrigStride, pCachedStream->Stream.uiNewStride,
                   pCachedStream->uiCount, pCachedStream->uiCheckFrequency,
                   pCachedStream->uiLength, pCachedStream->uiCRC32);
        }

        pNode = pNode->pNext;
    }
}

void XTL::VertexPatcher::CacheStream(VertexPatchDesc *pPatchDesc,
                                     UINT             uiStream)
{
    UINT                       uiStride;
    IDirect3DVertexBuffer8    *pOrigVertexBuffer;
    XTL::D3DVERTEXBUFFER_DESC  Desc;
    void                      *pCalculateData;
    uint32                     uiKey;
    UINT                       uiLength;
    CACHEDSTREAM              *pCachedStream = (CACHEDSTREAM *)CxbxMalloc(sizeof(CACHEDSTREAM));

    ZeroMemory(pCachedStream, sizeof(CACHEDSTREAM));

    // Check if the cache is full, if so, throw away the least used stream
    if(g_PatchedStreamsCache.get_count() > VERTEX_BUFFER_CACHE_SIZE)
    {
        uint32 uiKey = 0;
        uint32 uiMinHit = 0xFFFFFFFF;

        RTNode *pNode = g_PatchedStreamsCache.getHead();
        while(pNode)
        {
            if(pNode->pResource)
            {
                // First, check if there is an "expired" stream in the cache (not recently used)
                if(((CACHEDSTREAM *)pNode->pResource)->lLastUsed < (clock() + MAX_STREAM_NOT_USED_TIME))
                {
                    printf("!!!Found an old stream, %2.2f\n", ((FLOAT)((clock() + MAX_STREAM_NOT_USED_TIME) - ((CACHEDSTREAM *)pNode->pResource)->lLastUsed)) / (FLOAT)CLOCKS_PER_SEC);
                    uiKey = pNode->uiKey;
                    break;
                }
                // Find the least used cached stream
                if((uint32)((CACHEDSTREAM *)pNode->pResource)->uiCacheHit < uiMinHit)
                {
                    uiMinHit = ((CACHEDSTREAM *)pNode->pResource)->uiCacheHit;
                    uiKey = pNode->uiKey;
                }
            }
            pNode = pNode->pNext;
        }
        if(uiKey != 0)
        {
            printf("!!!Removing stream\n\n");
            FreeCachedStream((void*)uiKey);
        }
    }

    // Start the actual stream caching
    if(!pPatchDesc->pVertexStreamZeroData)
    {
        pOrigVertexBuffer = m_pStreams[uiStream].pOriginalStream;
        pOrigVertexBuffer->AddRef();
        m_pStreams[uiStream].pPatchedStream->AddRef();
        if(FAILED(pOrigVertexBuffer->GetDesc(&Desc)))
        {
            CxbxKrnlCleanup("Could not retrieve original buffer size");
        }
        if(FAILED(pOrigVertexBuffer->Lock(0, 0, (uint08**)&pCalculateData, 0)))
        {
            CxbxKrnlCleanup("Couldn't lock the original buffer");
        }

        uiLength = Desc.Size;
        pCachedStream->bIsUP = false;
        uiKey = (uint32)pOrigVertexBuffer;
    }
    else
    {
        // There should only be one stream (stream zero) in this case
        if(uiStream != 0)
        {
            CxbxKrnlCleanup("Trying to patch a Draw..UP with more than stream zero!");
        }
        uiStride  = pPatchDesc->uiVertexStreamZeroStride;
        pCalculateData = (uint08 *)pPatchDesc->pVertexStreamZeroData;
        // TODO: This is sometimes the number of indices, which isn't too good
        uiLength = pPatchDesc->dwVertexCount * pPatchDesc->uiVertexStreamZeroStride;
        pCachedStream->bIsUP = true;
        pCachedStream->pStreamUP = pCalculateData;
        uiKey = (uint32)pCalculateData;
    }

    UINT uiChecksum = CRC32((unsigned char *)pCalculateData, uiLength);
    if(!pPatchDesc->pVertexStreamZeroData)
    {
        pOrigVertexBuffer->Unlock();
    }

    pCachedStream->uiCRC32 = uiChecksum;
    pCachedStream->Stream = m_pStreams[uiStream];
    pCachedStream->uiCheckFrequency = 1; // Start with checking every 1th Draw..
    pCachedStream->uiCount = 0;
    pCachedStream->uiLength = uiLength;
    pCachedStream->uiCacheHit = 0;
    pCachedStream->dwPrimitiveCount = pPatchDesc->dwPrimitiveCount;
    pCachedStream->lLastUsed = clock();
    g_PatchedStreamsCache.insert(uiKey, pCachedStream);
}

void XTL::VertexPatcher::FreeCachedStream(void *pStream)
{
    g_PatchedStreamsCache.Lock();
    CACHEDSTREAM *pCachedStream = (CACHEDSTREAM *)g_PatchedStreamsCache.get(pStream);
    if(pCachedStream)
    {
        if(pCachedStream->bIsUP && pCachedStream->pStreamUP)
        {
            CxbxFree(pCachedStream->pStreamUP);
        }
        if(pCachedStream->Stream.pOriginalStream)
        {
            pCachedStream->Stream.pOriginalStream->Release();
        }
        if(pCachedStream->Stream.pPatchedStream)
        {
            pCachedStream->Stream.pPatchedStream->Release();
        }
        CxbxFree(pCachedStream);
    }
    g_PatchedStreamsCache.Unlock();
    g_PatchedStreamsCache.remove(pStream);
}

bool XTL::VertexPatcher::ApplyCachedStream(VertexPatchDesc *pPatchDesc,
                                           UINT             uiStream)
{
    UINT                       uiStride;
    IDirect3DVertexBuffer8    *pOrigVertexBuffer;
    XTL::D3DVERTEXBUFFER_DESC  Desc;
    void                      *pCalculateData;
    UINT                       uiLength;
    bool                       bApplied = false;
    uint32                     uiKey;
    //CACHEDSTREAM              *pCachedStream = (CACHEDSTREAM *)CxbxMalloc(sizeof(CACHEDSTREAM));

    if(!pPatchDesc->pVertexStreamZeroData)
    {
        g_pD3DDevice8->GetStreamSource(uiStream, &pOrigVertexBuffer, &uiStride);
        if(FAILED(pOrigVertexBuffer->GetDesc(&Desc)))
        {
            CxbxKrnlCleanup("Could not retrieve original buffer size");
        }
        uiLength = Desc.Size;
        uiKey = (uint32)pOrigVertexBuffer;
        //pCachedStream->bIsUP = false;
    }
    else
    {
        // There should only be one stream (stream zero) in this case
        if(uiStream != 0)
        {
            CxbxKrnlCleanup("Trying to find a cached Draw..UP with more than stream zero!");
        }
        uiStride  = pPatchDesc->uiVertexStreamZeroStride;
        pCalculateData = (uint08 *)pPatchDesc->pVertexStreamZeroData;
        // TODO: This is sometimes the number of indices, which isn't too good
        uiLength = pPatchDesc->dwVertexCount * pPatchDesc->uiVertexStreamZeroStride;
        uiKey = (uint32)pCalculateData;
        //pCachedStream->bIsUP = true;
        //pCachedStream->pStreamUP = pCalculateData;
    }
    g_PatchedStreamsCache.Lock();

    CACHEDSTREAM *pCachedStream = (CACHEDSTREAM *)g_PatchedStreamsCache.get(uiKey);
    if(pCachedStream)
    {
        pCachedStream->lLastUsed = clock();
        pCachedStream->uiCacheHit++;
        bool bMismatch = false;
        if(pCachedStream->uiCount == (pCachedStream->uiCheckFrequency - 1))
        {
            if(!pPatchDesc->pVertexStreamZeroData)
            {
                if(FAILED(pOrigVertexBuffer->Lock(0, 0, (uint08**)&pCalculateData, 0)))
                {
                    CxbxKrnlCleanup("Couldn't lock the original buffer");
                }
            }
            // Use the cached stream length (which is a must for the UP stream)
            uint32 Checksum = CRC32((uint08*)pCalculateData, pCachedStream->uiLength);
            if(Checksum == pCachedStream->uiCRC32)
            {
                // Take a while longer to check
                if(pCachedStream->uiCheckFrequency < 32*1024)
                {
                    pCachedStream->uiCheckFrequency *= 2;
                }
                pCachedStream->uiCount = 0;
            }
            else
            {
                // TODO: Do something about this
                if(pCachedStream->bIsUP)
                {
                    FreeCachedStream(pCachedStream->pStreamUP);
                }
                else
                {
                    FreeCachedStream(pCachedStream->Stream.pOriginalStream);
                }
                pCachedStream = NULL;
                bMismatch = true;
            }
            if(!pPatchDesc->pVertexStreamZeroData)
            {
                pOrigVertexBuffer->Unlock();
            }
        }
        else
        {
            pCachedStream->uiCount++;
        }
        if(!bMismatch)
        {
            if(!pCachedStream->bIsUP)
            {
                m_pStreams[uiStream].pOriginalStream = pOrigVertexBuffer;
                m_pStreams[uiStream].uiOrigStride = uiStride;
                g_pD3DDevice8->SetStreamSource(uiStream, pCachedStream->Stream.pPatchedStream, pCachedStream->Stream.uiNewStride);
                pCachedStream->Stream.pPatchedStream->AddRef();
                pCachedStream->Stream.pOriginalStream->AddRef();
                m_pStreams[uiStream].pPatchedStream = pCachedStream->Stream.pPatchedStream;
                m_pStreams[uiStream].uiNewStride = pCachedStream->Stream.uiNewStride;
            }
            else
            {
                pPatchDesc->pVertexStreamZeroData = pCachedStream->pStreamUP;
                pPatchDesc->uiVertexStreamZeroStride = pCachedStream->Stream.uiNewStride;
            }
            if(pCachedStream->dwPrimitiveCount)
            {
                // The primitives were patched, draw with the correct number of primimtives from the cache
                pPatchDesc->dwPrimitiveCount = pCachedStream->dwPrimitiveCount;
            }
            bApplied = true;
            m_bPatched = true;
        }
    }
    g_PatchedStreamsCache.Unlock();

    if(!pPatchDesc->pVertexStreamZeroData)
    {
        pOrigVertexBuffer->Release();
    }

    return bApplied;
}

UINT XTL::VertexPatcher::GetNbrStreams(VertexPatchDesc *pPatchDesc)
{
    if(VshHandleIsVertexShader(g_CurrentVertexShader))
    {
        VERTEX_DYNAMIC_PATCH *pDynamicPatch = VshGetVertexDynamicPatch(g_CurrentVertexShader);
        if(pDynamicPatch)
        {
            return pDynamicPatch->NbrStreams;
        }
        else
        {
            return 1; // Could be more, but it doesn't matter as we're not going to patch the types
        }
    }
    else if(g_CurrentVertexShader)
    {
        return 1;
    }
    return 0;
}

bool XTL::VertexPatcher::PatchStream(VertexPatchDesc *pPatchDesc,
                                     UINT             uiStream)
{
    // FVF buffers doesn't have Xbox extensions, but texture coordinates may
    // need normalization if used with linear textures.
    if(!VshHandleIsVertexShader(pPatchDesc->hVertexShader))
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

    if(!m_pDynamicPatch || !m_pDynamicPatch->pStreamPatches[uiStream].NeedPatch)
    {
        return false;
    }

    // Do some groovy patchin'
    
    IDirect3DVertexBuffer8    *pOrigVertexBuffer;
    IDirect3DVertexBuffer8    *pNewVertexBuffer;
    uint08                    *pOrigData;
    uint08                    *pNewData;
    UINT                       uiStride;
    XTL::D3DVERTEXBUFFER_DESC  Desc;
    PATCHEDSTREAM             *pStream = &m_pStreams[uiStream];
    STREAM_DYNAMIC_PATCH      *pStreamPatch = &m_pDynamicPatch->pStreamPatches[uiStream];
    DWORD dwNewSize;

    if(!pPatchDesc->pVertexStreamZeroData)
    {
        g_pD3DDevice8->GetStreamSource(uiStream, &pOrigVertexBuffer, &uiStride);
        if(FAILED(pOrigVertexBuffer->GetDesc(&Desc)))
        {
            CxbxKrnlCleanup("Could not retrieve original buffer size");
        }
        // Set a new (exact) vertex count
        pPatchDesc->dwVertexCount = Desc.Size / uiStride;
        dwNewSize = pPatchDesc->dwVertexCount * pStreamPatch->ConvertedStride;

        if(FAILED(pOrigVertexBuffer->Lock(0, 0, &pOrigData, 0)))
        {
            CxbxKrnlCleanup("Couldn't lock the original buffer");
        }
        g_pD3DDevice8->CreateVertexBuffer(dwNewSize, 0, 0, XTL::D3DPOOL_MANAGED, &pNewVertexBuffer);
        if(FAILED(pNewVertexBuffer->Lock(0, 0, &pNewData, 0)))
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
        uiStride  = pPatchDesc->uiVertexStreamZeroStride;
        pOrigData = (uint08 *)pPatchDesc->pVertexStreamZeroData;
        // TODO: This is sometimes the number of indices, which isn't too good
        dwNewSize = pPatchDesc->dwVertexCount * pStreamPatch->ConvertedStride;
        pNewVertexBuffer = NULL;
        pNewData = (uint08*)CxbxMalloc(dwNewSize);
        if(!pNewData)
        {
            CxbxKrnlCleanup("Couldn't allocate the new stream zero buffer");
        }
    }

    for(uint32 uiVertex = 0; uiVertex < pPatchDesc->dwVertexCount; uiVertex++)
    {
        DWORD dwPosOrig = 0;
        DWORD dwPosNew = 0;
        for(UINT uiType = 0; uiType < pStreamPatch->NbrTypes; uiType++)
        {
            switch(pStreamPatch->pTypes[uiType])
            {
                case 0x12: // FLOAT1
                    memcpy(&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew],
                           &pOrigData[uiVertex * uiStride + dwPosOrig],
                           sizeof(FLOAT));
                    dwPosOrig += sizeof(FLOAT);
                    dwPosNew  += sizeof(FLOAT);
                    break;
                case 0x22: // FLOAT2
                    memcpy(&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew],
                           &pOrigData[uiVertex * uiStride + dwPosOrig],
                           2 * sizeof(FLOAT));
                    dwPosOrig += 2 * sizeof(FLOAT);
                    dwPosNew  += 2 * sizeof(FLOAT);
                    break;
                case 0x32: // FLOAT3
                    memcpy(&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew],
                           &pOrigData[uiVertex * uiStride + dwPosOrig],
                           3 * sizeof(FLOAT));
                    dwPosOrig += 3 * sizeof(FLOAT);
                    dwPosNew  += 3 * sizeof(FLOAT);
                    break;
                case 0x42: // FLOAT4
                    memcpy(&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew],
                           &pOrigData[uiVertex * uiStride + dwPosOrig],
                           4 * sizeof(FLOAT));
                    dwPosOrig += 4 * sizeof(FLOAT);
                    dwPosNew  += 4 * sizeof(FLOAT);
                    break;
                case 0x40: // D3DCOLOR
                    memcpy(&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew],
                           &pOrigData[uiVertex * uiStride + dwPosOrig],
                           sizeof(XTL::D3DCOLOR));
                    dwPosOrig += sizeof(XTL::D3DCOLOR);
                    dwPosNew  += sizeof(XTL::D3DCOLOR);
                    break;
                case 0x16: //NORMPACKED3
                    {
                        DWORD dwPacked = ((DWORD *)&pOrigData[uiVertex * uiStride + dwPosOrig])[0];

                        ((FLOAT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew])[0] = ((FLOAT)(dwPacked & 0x7ff)) / 1023.0f;
                        ((FLOAT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew])[1] = ((FLOAT)((dwPacked >> 11) & 0x7ff)) / 1023.0f;
                        ((FLOAT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew])[2] = ((FLOAT)((dwPacked >> 22) & 0x3ff)) / 511.0f;

                        dwPosOrig += sizeof(DWORD);
                        dwPosNew  += 3 * sizeof(FLOAT);
                    }
                    break;
                case 0x15: // SHORT1
                    // Make it a SHORT2
                    (*((SHORT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew + 0 * sizeof(SHORT)])) = *(SHORT*)&pOrigData[uiVertex * uiStride + dwPosOrig];
                    (*((SHORT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew + 1 * sizeof(SHORT)])) = 0x00;

                    dwPosOrig += 1 * sizeof(SHORT);
                    dwPosNew  += 2 * sizeof(SHORT);

                    break;
                case 0x25: // SHORT2
                    memcpy(&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew],
                           &pOrigData[uiVertex * uiStride+dwPosOrig],
                           2 * sizeof(SHORT));
                    dwPosOrig += 2 * sizeof(SHORT);
                    dwPosNew  += 2 * sizeof(SHORT);
                    break;
                case 0x35: // SHORT3
                    memcpy(&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew],
                           &pOrigData[uiVertex * uiStride + dwPosOrig],
                           3 * sizeof(SHORT));
                    // Make it a SHORT4 and set the last short to 1
                    (*((SHORT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew + 3 * sizeof(SHORT)])) = 0x01;

                    dwPosOrig += 3 * sizeof(SHORT);
                    dwPosNew  += 4 * sizeof(SHORT);

                    break;
                case 0x45: // SHORT4
                    memcpy(&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew],
                           &pOrigData[uiVertex * uiStride + dwPosOrig],
                           4 * sizeof(SHORT));
                    dwPosOrig += 4 * sizeof(SHORT);
                    dwPosNew  += 4 * sizeof(SHORT);
                    break;
                case 0x14: // PBYTE1
                    ((FLOAT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew])[0] = ((FLOAT)((BYTE*)&pOrigData[uiVertex * uiStride + dwPosOrig])[0]) / 255.0f;

                    dwPosOrig += 1 * sizeof(BYTE);
                    dwPosNew  += 1 * sizeof(FLOAT);

                    break;
                case 0x24: // PBYTE2
                    ((FLOAT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew])[0] = ((FLOAT)((BYTE*)&pOrigData[uiVertex * uiStride + dwPosOrig])[0]) / 255.0f;
                    ((FLOAT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew])[1] = ((FLOAT)((BYTE*)&pOrigData[uiVertex * uiStride + dwPosOrig])[1]) / 255.0f;

                    dwPosOrig += 2 * sizeof(BYTE);
                    dwPosNew  += 2 * sizeof(FLOAT);

                    break;
                case 0x34: // PBYTE3
                    ((FLOAT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew])[0] = ((FLOAT)((BYTE*)&pOrigData[uiVertex * uiStride + dwPosOrig])[0]) / 255.0f;
                    ((FLOAT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew])[1] = ((FLOAT)((BYTE*)&pOrigData[uiVertex * uiStride + dwPosOrig])[1]) / 255.0f;
                    ((FLOAT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew])[2] = ((FLOAT)((BYTE*)&pOrigData[uiVertex * uiStride + dwPosOrig])[2]) / 255.0f;

                    dwPosOrig += 3 * sizeof(BYTE);
                    dwPosNew  += 3 * sizeof(FLOAT);

                    break;
                case 0x44: // PBYTE4
                    ((FLOAT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew])[0] = ((FLOAT)((BYTE*)&pOrigData[uiVertex * uiStride + dwPosOrig])[0]) / 255.0f;
                    ((FLOAT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew])[1] = ((FLOAT)((BYTE*)&pOrigData[uiVertex * uiStride + dwPosOrig])[1]) / 255.0f;
                    ((FLOAT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew])[2] = ((FLOAT)((BYTE*)&pOrigData[uiVertex * uiStride + dwPosOrig])[2]) / 255.0f;
                    ((FLOAT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew])[3] = ((FLOAT)((BYTE*)&pOrigData[uiVertex * uiStride + dwPosOrig])[3]) / 255.0f;

                    dwPosOrig += 4 * sizeof(BYTE);
                    dwPosNew  += 4 * sizeof(FLOAT);

                    break;
                case 0x11: // NORMSHORT1
                    ((FLOAT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew])[0] = ((FLOAT)((SHORT*)&pOrigData[uiVertex * uiStride + dwPosOrig])[0]) / 32767.0f;

                    dwPosOrig += 1 * sizeof(SHORT);
                    dwPosNew  += 1 * sizeof(FLOAT);
                    break;
                case 0x21: // NORMSHORT2
                    ((FLOAT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew])[0] = ((FLOAT)((SHORT*)&pOrigData[uiVertex * uiStride + dwPosOrig])[0]) / 32767.0f;
                    ((FLOAT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew])[1] = ((FLOAT)((SHORT*)&pOrigData[uiVertex * uiStride + dwPosOrig])[1]) / 32767.0f;

                    dwPosOrig += 2 * sizeof(SHORT);
                    dwPosNew  += 2 * sizeof(FLOAT);
                    break;
                case 0x31: // NORMSHORT3
                    ((FLOAT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew])[0] = ((FLOAT)((SHORT*)&pOrigData[uiVertex * uiStride + dwPosOrig])[0]) / 32767.0f;
                    ((FLOAT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew])[1] = ((FLOAT)((SHORT*)&pOrigData[uiVertex * uiStride + dwPosOrig])[1]) / 32767.0f;
                    ((FLOAT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew])[2] = ((FLOAT)((SHORT*)&pOrigData[uiVertex * uiStride + dwPosOrig])[2]) / 32767.0f;

                    dwPosOrig += 3 * sizeof(SHORT);
                    dwPosNew  += 3 * sizeof(FLOAT);
                    break;
                case 0x41: // NORMSHORT4
                    ((FLOAT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew])[0] = ((FLOAT)((SHORT*)&pOrigData[uiVertex * uiStride + dwPosOrig])[0]) / 32767.0f;
                    ((FLOAT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew])[1] = ((FLOAT)((SHORT*)&pOrigData[uiVertex * uiStride + dwPosOrig])[1]) / 32767.0f;
                    ((FLOAT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew])[2] = ((FLOAT)((SHORT*)&pOrigData[uiVertex * uiStride + dwPosOrig])[2]) / 32767.0f;
                    ((FLOAT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew])[3] = ((FLOAT)((SHORT*)&pOrigData[uiVertex * uiStride + dwPosOrig])[3]) / 32767.0f;

                    dwPosOrig += 4 * sizeof(SHORT);
                    dwPosNew  += 4 * sizeof(FLOAT);
                    break;
                case 0x72: // FLOAT2H
                    ((FLOAT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew])[0] = ((FLOAT*)&pOrigData[uiVertex * uiStride + dwPosOrig])[0];
                    ((FLOAT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew])[1] = ((FLOAT*)&pOrigData[uiVertex * uiStride + dwPosOrig])[1];
                    ((FLOAT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew])[2] = 0.0f;
                    ((FLOAT *)&pNewData[uiVertex * pStreamPatch->ConvertedStride + dwPosNew])[3] = ((FLOAT*)&pOrigData[uiVertex * uiStride + dwPosOrig])[2];

                /*TODO
                case 0x02:
                    printf("D3DVSDT_NONE / xbox ext. nsp /");
                    dwNewDataType = 0xFF;
                    break;
                */
                default:
                    CxbxKrnlCleanup("Unhandled stream type: 0x%.02X\n", pStreamPatch->pTypes[uiType]);
                    break;
            }
        }
    }
    if(!pPatchDesc->pVertexStreamZeroData)
    {
        pNewVertexBuffer->Unlock();
        pOrigVertexBuffer->Unlock();

        if(FAILED(g_pD3DDevice8->SetStreamSource(uiStream, pNewVertexBuffer, pStreamPatch->ConvertedStride)))
        {
            CxbxKrnlCleanup("Failed to set the type patched buffer as the new stream source!\n");
        }
        if(pStream->pPatchedStream)
        {
            // The stream was already primitive patched, release the previous vertex buffer to avoid memory leaks
            pStream->pPatchedStream->Release();
        }
        pStream->pPatchedStream = pNewVertexBuffer;
    }
    else
    {
        pPatchDesc->pVertexStreamZeroData = pNewData;
        pPatchDesc->uiVertexStreamZeroStride = pStreamPatch->ConvertedStride;
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
    bool bHasLinearTex = false, bTexIsLinear[4];
    X_D3DPixelContainer *pLinearPixelContainer[4];

    for(uint08 i = 0; i < 4; i++)
    {
        X_D3DPixelContainer *pPixelContainer = (X_D3DPixelContainer*)EmuD3DActiveTexture[i];
        if(pPixelContainer && EmuXBFormatIsLinear(((X_D3DFORMAT)pPixelContainer->Format & X_D3DFORMAT_FORMAT_MASK) >> X_D3DFORMAT_FORMAT_SHIFT))
        {
            bHasLinearTex = bTexIsLinear[i] = true;
            pLinearPixelContainer[i] = pPixelContainer;
        }
        else
        {
            bTexIsLinear[i] = false;
        }
    }

    if(!bHasLinearTex)
        return false;

    IDirect3DVertexBuffer8 *pOrigVertexBuffer;
    IDirect3DVertexBuffer8 *pNewVertexBuffer;
    PATCHEDSTREAM *pStream;
    uint08 *pData, *pUVData;
    uint uiStride, uiVertexCount;

    if(pPatchDesc->pVertexStreamZeroData)
    {
        // In-place patching of inline buffer.
        pNewVertexBuffer = 0;
        pData = (uint08 *)pPatchDesc->pVertexStreamZeroData;
        uiStride = pPatchDesc->uiVertexStreamZeroStride;
        uiVertexCount = pPatchDesc->dwVertexCount;
    }
    else
    {
        // Copy stream for patching and caching.
        g_pD3DDevice8->GetStreamSource(uiStream, &pOrigVertexBuffer, &uiStride);
        XTL::D3DVERTEXBUFFER_DESC Desc;
        if(FAILED(pOrigVertexBuffer->GetDesc(&Desc)))
        {
            CxbxKrnlCleanup("Could not retrieve original FVF buffer size.");
        }
        uiVertexCount = Desc.Size / uiStride;
        
        uint08 *pOrigData;
        if(FAILED(pOrigVertexBuffer->Lock(0, 0, &pOrigData, 0)))
        {
            CxbxKrnlCleanup("Couldn't lock original FVF buffer.");
        }
        g_pD3DDevice8->CreateVertexBuffer(Desc.Size, 0, 0, XTL::D3DPOOL_MANAGED, &pNewVertexBuffer);
        if(FAILED(pNewVertexBuffer->Lock(0, 0, &pData, 0)))
        {
            CxbxKrnlCleanup("Couldn't lock new FVF buffer.");
        }
        memcpy(pData, pOrigData, Desc.Size);
        pOrigVertexBuffer->Unlock();

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
                ((FLOAT*)pUVData)[0] /= ( pLinearPixelContainer[0]->Size & X_D3DSIZE_WIDTH_MASK) + 1;
                ((FLOAT*)pUVData)[1] /= ((pLinearPixelContainer[0]->Size & X_D3DSIZE_HEIGHT_MASK) >> X_D3DSIZE_HEIGHT_SHIFT) + 1;
            }
            pUVData += sizeof(FLOAT) * 2;
        }

        if(dwTexN >= 2)
        {
            if(bTexIsLinear[1])
            {
                ((FLOAT*)pUVData)[0] /= ( pLinearPixelContainer[1]->Size & X_D3DSIZE_WIDTH_MASK) + 1;
                ((FLOAT*)pUVData)[1] /= ((pLinearPixelContainer[1]->Size & X_D3DSIZE_HEIGHT_MASK) >> X_D3DSIZE_HEIGHT_SHIFT) + 1;
            }
            pUVData += sizeof(FLOAT) * 2;
        }

        if(dwTexN >= 3)
        {
            if(bTexIsLinear[2])
            {
                ((FLOAT*)pUVData)[0] /= ( pLinearPixelContainer[2]->Size & X_D3DSIZE_WIDTH_MASK) + 1;
                ((FLOAT*)pUVData)[1] /= ((pLinearPixelContainer[2]->Size & X_D3DSIZE_HEIGHT_MASK) >> X_D3DSIZE_HEIGHT_SHIFT) + 1;
            }
            pUVData += sizeof(FLOAT) * 2;
        }

        if((dwTexN >= 4) && bTexIsLinear[3])
        {
            ((FLOAT*)pUVData)[0] /= ( pLinearPixelContainer[3]->Size & X_D3DSIZE_WIDTH_MASK) + 1;
            ((FLOAT*)pUVData)[1] /= ((pLinearPixelContainer[3]->Size & X_D3DSIZE_HEIGHT_MASK) >> X_D3DSIZE_HEIGHT_SHIFT) + 1;
        }
    }

    if(pNewVertexBuffer)
    {
        pNewVertexBuffer->Unlock();

        if(FAILED(g_pD3DDevice8->SetStreamSource(uiStream, pNewVertexBuffer, uiStride)))
        {
            CxbxKrnlCleanup("Failed to set the texcoord patched FVF buffer as the new stream source.");
        }
        if(pStream->pPatchedStream)
        {
            pStream->pPatchedStream->Release();
        }

        pStream->pPatchedStream = pNewVertexBuffer;
        pStream->uiOrigStride = uiStride;
        pStream->uiNewStride = uiStride;
        m_bPatched = true;
    }

    return m_bPatched;
}

bool XTL::VertexPatcher::PatchPrimitive(VertexPatchDesc *pPatchDesc,
                                        UINT             uiStream)
{
    PATCHEDSTREAM *pStream = &m_pStreams[uiStream];

    if((pPatchDesc->PrimitiveType) < 1 || (pPatchDesc->PrimitiveType >= X_D3DPT_MAX))
    {
        CxbxKrnlCleanup("Unknown primitive type: 0x%.02X\n", pPatchDesc->PrimitiveType);
    }

    // Unsupported primitives that don't need deep patching.
    switch(pPatchDesc->PrimitiveType)
    {
        // Quad strip is just like a triangle strip, but requires two
        // vertices per primitive.
        case X_D3DPT_QUADSTRIP:
            pPatchDesc->dwVertexCount -= pPatchDesc->dwVertexCount % 2;
            pPatchDesc->PrimitiveType = X_D3DPT_TRIANGLESTRIP;
            break;

        // Convex polygon is the same as a triangle fan.
        case X_D3DPT_POLYGON:
            pPatchDesc->PrimitiveType = X_D3DPT_TRIANGLEFAN;
            break;
    }

    pPatchDesc->dwPrimitiveCount = EmuD3DVertex2PrimitiveCount(pPatchDesc->PrimitiveType, pPatchDesc->dwVertexCount);

    // Skip primitives that don't need further patching.
    switch(pPatchDesc->PrimitiveType)
    {
        case X_D3DPT_QUADLIST:
        case X_D3DPT_LINELOOP:
            break;

        default:
            return false;
    }

    if(pPatchDesc->pVertexStreamZeroData && uiStream > 0)
    {
        CxbxKrnlCleanup("Draw..UP call with more than one stream!\n");
    }

    pStream->uiOrigStride = 0;

    // sizes of our part in the vertex buffer
    DWORD dwOriginalSize    = 0;
    DWORD dwNewSize         = 0;

    // sizes with the rest of the buffer
    DWORD dwOriginalSizeWR  = 0;
    DWORD dwNewSizeWR       = 0;

    // vertex data arrays
    BYTE *pOrigVertexData = 0;
    BYTE *pPatchedVertexData = 0;

    if(pPatchDesc->pVertexStreamZeroData == 0)
    {
        g_pD3DDevice8->GetStreamSource(0, &pStream->pOriginalStream, &pStream->uiOrigStride);
        pStream->uiNewStride = pStream->uiOrigStride; // The stride is still the same
    }
    else
    {
        pStream->uiOrigStride = pPatchDesc->uiVertexStreamZeroStride;
    }

    // Quad list
    if(pPatchDesc->PrimitiveType == X_D3DPT_QUADLIST)
    {
        pPatchDesc->dwPrimitiveCount *= 2;

        // This is a list of sqares/rectangles, so we convert it to a list of triangles
        dwOriginalSize  = pPatchDesc->dwPrimitiveCount * pStream->uiOrigStride * 2;
        dwNewSize       = pPatchDesc->dwPrimitiveCount * pStream->uiOrigStride * 3;
    }
    // Line loop
    else if(pPatchDesc->PrimitiveType == X_D3DPT_LINELOOP)
    {
        pPatchDesc->dwPrimitiveCount += 1;

        // We will add exactly one more line
        dwOriginalSize  = pPatchDesc->dwPrimitiveCount * pStream->uiOrigStride;
        dwNewSize       = pPatchDesc->dwPrimitiveCount * pStream->uiOrigStride + pStream->uiOrigStride;
    }

    if(pPatchDesc->pVertexStreamZeroData == 0)
    {
        // Retrieve the original buffer size
        {
            XTL::D3DVERTEXBUFFER_DESC Desc;

            if(FAILED(pStream->pOriginalStream->GetDesc(&Desc)))
            {
                CxbxKrnlCleanup("Could not retrieve buffer size");
            }

            // Here we save the full buffer size
            dwOriginalSizeWR = Desc.Size;

            // So we can now calculate the size of the rest (dwOriginalSizeWR - dwOriginalSize) and
            // add it to our new calculated size of the patched buffer
            dwNewSizeWR = dwNewSize + dwOriginalSizeWR - dwOriginalSize;
        }

        g_pD3DDevice8->CreateVertexBuffer(dwNewSizeWR, 0, 0, XTL::D3DPOOL_MANAGED, &pStream->pPatchedStream);

        if(pStream->pOriginalStream != 0)
        {
            pStream->pOriginalStream->Lock(0, 0, &pOrigVertexData, 0);
        }

        if(pStream->pPatchedStream != 0)
        {
            pStream->pPatchedStream->Lock(0, 0, &pPatchedVertexData, 0);
        }
    }
    else
    {
        dwOriginalSizeWR = dwOriginalSize;
        dwNewSizeWR = dwNewSize;

        m_pNewVertexStreamZeroData = (uint08*)CxbxMalloc(dwNewSizeWR);
        m_bAllocatedStreamZeroData = true;

        pPatchedVertexData = (uint08*)m_pNewVertexStreamZeroData;
        pOrigVertexData = (uint08*)pPatchDesc->pVertexStreamZeroData;

        pPatchDesc->pVertexStreamZeroData = pPatchedVertexData;
    }

    // Copy the nonmodified data
    memcpy(pPatchedVertexData, pOrigVertexData, pPatchDesc->dwOffset);
    memcpy(&pPatchedVertexData[pPatchDesc->dwOffset+dwNewSize],
           &pOrigVertexData[pPatchDesc->dwOffset+dwOriginalSize],
           dwOriginalSizeWR - pPatchDesc->dwOffset - dwOriginalSize);

    // Quad list
    if(pPatchDesc->PrimitiveType == X_D3DPT_QUADLIST)
    {
        uint08 *pPatch1 = &pPatchedVertexData[pPatchDesc->dwOffset     * pStream->uiOrigStride];
        uint08 *pPatch2 = &pPatchedVertexData[pPatchDesc->dwOffset + 3 * pStream->uiOrigStride];
        uint08 *pPatch3 = &pPatchedVertexData[pPatchDesc->dwOffset + 4 * pStream->uiOrigStride];
        uint08 *pPatch4 = &pPatchedVertexData[pPatchDesc->dwOffset + 5 * pStream->uiOrigStride];

        uint08 *pOrig1 = &pOrigVertexData[pPatchDesc->dwOffset     * pStream->uiOrigStride];
        uint08 *pOrig2 = &pOrigVertexData[pPatchDesc->dwOffset + 2 * pStream->uiOrigStride];
        uint08 *pOrig3 = &pOrigVertexData[pPatchDesc->dwOffset + 3 * pStream->uiOrigStride];

        for(uint32 i = 0;i < pPatchDesc->dwPrimitiveCount/2;i++)
        {
            memcpy(pPatch1, pOrig1, pStream->uiOrigStride * 3); // Vertex 0,1,2 := Vertex 0,1,2
            memcpy(pPatch2, pOrig2, pStream->uiOrigStride);     // Vertex 3     := Vertex 2
            memcpy(pPatch3, pOrig3, pStream->uiOrigStride);     // Vertex 4     := Vertex 3
            memcpy(pPatch4, pOrig1, pStream->uiOrigStride);     // Vertex 5     := Vertex 0

            pPatch1 += pStream->uiOrigStride * 6;
            pPatch2 += pStream->uiOrigStride * 6;
            pPatch3 += pStream->uiOrigStride * 6;
            pPatch4 += pStream->uiOrigStride * 6;

            pOrig1 += pStream->uiOrigStride * 4;
            pOrig2 += pStream->uiOrigStride * 4;
            pOrig3 += pStream->uiOrigStride * 4;

            if(pPatchDesc->hVertexShader & D3DFVF_XYZRHW)
            {
                for(int z = 0; z < 6; z++)
                {
                    if(((FLOAT*)&pPatchedVertexData[pPatchDesc->dwOffset + i * pStream->uiOrigStride * 6 + z * pStream->uiOrigStride])[2] == 0.0f)
                        ((FLOAT*)&pPatchedVertexData[pPatchDesc->dwOffset + i * pStream->uiOrigStride * 6 + z * pStream->uiOrigStride])[2] = 1.0f;
                    if(((FLOAT*)&pPatchedVertexData[pPatchDesc->dwOffset + i * pStream->uiOrigStride * 6 + z * pStream->uiOrigStride])[3] == 0.0f)
                        ((FLOAT*)&pPatchedVertexData[pPatchDesc->dwOffset + i * pStream->uiOrigStride * 6 + z * pStream->uiOrigStride])[3] = 1.0f;
                }
            }
        }
    }
    // Line loop
    else if(pPatchDesc->PrimitiveType == X_D3DPT_LINELOOP)
    {
        memcpy(&pPatchedVertexData[pPatchDesc->dwOffset], &pOrigVertexData[pPatchDesc->dwOffset], dwOriginalSize);
        memcpy(&pPatchedVertexData[pPatchDesc->dwOffset + dwOriginalSize], &pOrigVertexData[pPatchDesc->dwOffset], pStream->uiOrigStride);
    }

    if(pPatchDesc->pVertexStreamZeroData == 0)
    {
        pStream->pOriginalStream->Unlock();
        pStream->pPatchedStream->Unlock();

        g_pD3DDevice8->SetStreamSource(0, pStream->pPatchedStream, pStream->uiOrigStride);
    }

    m_bPatched = true;

    return true;
}

bool XTL::VertexPatcher::Apply(VertexPatchDesc *pPatchDesc)
{
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

        if(ApplyCachedStream(pPatchDesc, uiStream))
        {
            m_pStreams[uiStream].bUsedCached = true;
            continue;
        }

        LocalPatched |= PatchPrimitive(pPatchDesc, uiStream);
        LocalPatched |= PatchStream(pPatchDesc, uiStream);
        if(LocalPatched && !pPatchDesc->pVertexStreamZeroData)
        {
            // Insert the patched stream in the cache
            CacheStream(pPatchDesc, uiStream);
            m_pStreams[uiStream].bUsedCached = true;
        }
        Patched |= LocalPatched;
    }

    return Patched;
}

bool XTL::VertexPatcher::Restore()
{
    if(!this->m_bPatched)
        return false;

    for(UINT uiStream = 0; uiStream < m_uiNbrStreams; uiStream++)
    {
        if(m_pStreams[uiStream].pOriginalStream != NULL && m_pStreams[uiStream].pPatchedStream != NULL)
        {
            g_pD3DDevice8->SetStreamSource(0, m_pStreams[uiStream].pOriginalStream, m_pStreams[uiStream].uiOrigStride);
        }

        if(m_pStreams[uiStream].pOriginalStream != NULL)
        {
            UINT a = m_pStreams[uiStream].pOriginalStream->Release();
        }

        if(m_pStreams[uiStream].pPatchedStream != NULL)
        {
            UINT b = m_pStreams[uiStream].pPatchedStream->Release();
        }

        if(!m_pStreams[uiStream].bUsedCached)
        {

            if(this->m_bAllocatedStreamZeroData)
            {
                CxbxFree(m_pNewVertexStreamZeroData);
            }
        }
        else
        {
            m_pStreams[uiStream].bUsedCached = false;
        }

    }

    return true;
}

VOID XTL::EmuFlushIVB()
{
    XTL::EmuUpdateDeferredStates();

    DWORD *pdwVB = (DWORD*)g_IVBTable;

    UINT uiStride = 0;

    // Parse IVB table with current FVF shader if possible.
    bool bFVF = !VshHandleIsVertexShader(g_CurrentVertexShader);
    DWORD dwCurFVF;
    if(bFVF && ((g_CurrentVertexShader & D3DFVF_POSITION_MASK) != D3DFVF_XYZRHW))
    {
        dwCurFVF = g_CurrentVertexShader;
    }
    else
    {
        dwCurFVF = g_IVBFVF;
    }

    DbgPrintf("g_IVBTblOffs := %d\n", g_IVBTblOffs);

    for(uint v=0;v<g_IVBTblOffs;v++)
    {
        DWORD dwPos = dwCurFVF & D3DFVF_POSITION_MASK;

        if(dwPos == D3DFVF_XYZ)
        {
            *(FLOAT*)pdwVB++ = g_IVBTable[v].Position.x;
            *(FLOAT*)pdwVB++ = g_IVBTable[v].Position.y;
            *(FLOAT*)pdwVB++ = g_IVBTable[v].Position.z;

            if(v == 0)
            {
                uiStride += (sizeof(FLOAT)*3);
            }

            DbgPrintf("IVB Position := {%f, %f, %f}\n", g_IVBTable[v].Position.x, g_IVBTable[v].Position.y, g_IVBTable[v].Position.z);
 
        }
        else if(dwPos == D3DFVF_XYZRHW)
        {
            *(FLOAT*)pdwVB++ = g_IVBTable[v].Position.x;
            *(FLOAT*)pdwVB++ = g_IVBTable[v].Position.y;
            *(FLOAT*)pdwVB++ = g_IVBTable[v].Position.z;
            *(FLOAT*)pdwVB++ = g_IVBTable[v].Rhw;

            if(v == 0)
            {
                uiStride += (sizeof(FLOAT)*4);
            }

            DbgPrintf("IVB Position := {%f, %f, %f, %f}\n", g_IVBTable[v].Position.x, g_IVBTable[v].Position.y, g_IVBTable[v].Position.z, g_IVBTable[v].Position.z, g_IVBTable[v].Rhw);
        }

        else
        {
            CxbxKrnlCleanup("Unsupported Position Mask (FVF := 0x%.08X)", g_IVBFVF);
        }

        if(dwPos == D3DFVF_NORMAL)
        {
            *(FLOAT*)pdwVB++ = g_IVBTable[v].Normal.x;
            *(FLOAT*)pdwVB++ = g_IVBTable[v].Normal.y;
            *(FLOAT*)pdwVB++ = g_IVBTable[v].Normal.z;

            if(v == 0)
            {
                uiStride += (sizeof(FLOAT)*3);
            }

            DbgPrintf("IVB Normal := {%f, %f, %f}\n", g_IVBTable[v].Normal.x, g_IVBTable[v].Normal.y, g_IVBTable[v].Normal.z);
 
        }

        if(dwCurFVF & D3DFVF_DIFFUSE)
        {
            *(DWORD*)pdwVB++ = g_IVBTable[v].dwDiffuse;

            if(v == 0)
            {
                uiStride += sizeof(DWORD);
            }

            DbgPrintf("IVB Diffuse := 0x%.08X\n", g_IVBTable[v].dwDiffuse);
        }

        if(dwCurFVF & D3DFVF_SPECULAR)
        {
            *(DWORD*)pdwVB++ = g_IVBTable[v].dwSpecular;

            if(v == 0)
            {
                uiStride += sizeof(DWORD);
            }

            DbgPrintf("IVB Specular := 0x%.08X\n", g_IVBTable[v].dwSpecular);
        }

        DWORD dwTexN = (dwCurFVF & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;

        if(dwTexN >= 1)
        {
            *(FLOAT*)pdwVB++ = g_IVBTable[v].TexCoord1.x;
            *(FLOAT*)pdwVB++ = g_IVBTable[v].TexCoord1.y;

            if(v == 0)
            {
                uiStride += sizeof(FLOAT)*2;
            }

            DbgPrintf("IVB TexCoord1 := {%f, %f}\n", g_IVBTable[v].TexCoord1.x, g_IVBTable[v].TexCoord1.y);
        }

        if(dwTexN >= 2)
        {
            *(FLOAT*)pdwVB++ = g_IVBTable[v].TexCoord2.x;
            *(FLOAT*)pdwVB++ = g_IVBTable[v].TexCoord2.y;

            if(v == 0)
            {
                uiStride += sizeof(FLOAT)*2;
            }

            DbgPrintf("IVB TexCoord2 := {%f, %f}\n", g_IVBTable[v].TexCoord2.x, g_IVBTable[v].TexCoord2.y);
        }

        if(dwTexN >= 3)
        {
            *(FLOAT*)pdwVB++ = g_IVBTable[v].TexCoord3.x;
            *(FLOAT*)pdwVB++ = g_IVBTable[v].TexCoord3.y;

            if(v == 0)
            {
                uiStride += sizeof(FLOAT)*2;
            }

            DbgPrintf("IVB TexCoord3 := {%f, %f}\n", g_IVBTable[v].TexCoord3.x, g_IVBTable[v].TexCoord3.y);
        }

        if(dwTexN >= 4)
        {
            *(FLOAT*)pdwVB++ = g_IVBTable[v].TexCoord4.x;
            *(FLOAT*)pdwVB++ = g_IVBTable[v].TexCoord4.y;

            if(v == 0)
            {
                uiStride += sizeof(FLOAT)*2;
            }

            DbgPrintf("IVB TexCoord4 := {%f, %f}\n", g_IVBTable[v].TexCoord4.x, g_IVBTable[v].TexCoord4.y);
        }
    }

    VertexPatchDesc VPDesc;

    VPDesc.PrimitiveType = g_IVBPrimitiveType;
    VPDesc.dwVertexCount = g_IVBTblOffs;
    VPDesc.dwOffset = 0;
    VPDesc.pVertexStreamZeroData = g_IVBTable;
    VPDesc.uiVertexStreamZeroStride = uiStride;
    VPDesc.hVertexShader = g_CurrentVertexShader;

    VertexPatcher VertPatch;

    bool bPatched = VertPatch.Apply(&VPDesc);

    if(bFVF)
    {
        g_pD3DDevice8->SetVertexShader(dwCurFVF);
    }

    g_pD3DDevice8->DrawPrimitiveUP(
        EmuPrimitiveType(VPDesc.PrimitiveType),
        VPDesc.dwPrimitiveCount,
        VPDesc.pVertexStreamZeroData,
        VPDesc.uiVertexStreamZeroStride);

    if(bFVF)
    {
        g_pD3DDevice8->SetVertexShader(g_CurrentVertexShader);
    }

    VertPatch.Restore();

    g_IVBTblOffs = 0;

    return;
}

VOID XTL::EmuUpdateActiveTexture()
{
    //
    // DEBUGGING
    //
    for(int Stage=0;Stage<4;Stage++)
    {
        X_D3DResource *pTexture = EmuD3DActiveTexture[Stage];

        if(pTexture == NULL)
            continue;

        //*
        X_D3DResource       *pResource = (X_D3DResource*)pTexture;
        X_D3DPixelContainer *pPixelContainer = (X_D3DPixelContainer*)pTexture;

        X_D3DFORMAT X_Format = (X_D3DFORMAT)((pPixelContainer->Format & X_D3DFORMAT_FORMAT_MASK) >> X_D3DFORMAT_FORMAT_SHIFT);

        if(X_Format != 0xCD && (pTexture->EmuResource8->GetType() == D3DRTYPE_TEXTURE))
        {
            DWORD dwWidth, dwHeight, dwBPP, dwDepth = 1, dwPitch = 0, dwMipMapLevels = 1;
            BOOL  bSwizzled = FALSE, bCompressed = FALSE, dwCompressedSize = 0;
            BOOL  bCubemap = pPixelContainer->Format & X_D3DFORMAT_CUBEMAP;

            // Interpret Width/Height/BPP
            if(X_Format == 0x07 /* X_D3DFMT_X8R8G8B8 */ || X_Format == 0x06 /* X_D3DFMT_A8R8G8B8 */)
            {
                bSwizzled = TRUE;

                // Swizzled 32 Bit
                dwWidth  = 1 << ((pPixelContainer->Format & X_D3DFORMAT_USIZE_MASK) >> X_D3DFORMAT_USIZE_SHIFT);
                dwHeight = 1 << ((pPixelContainer->Format & X_D3DFORMAT_VSIZE_MASK) >> X_D3DFORMAT_VSIZE_SHIFT);
                dwMipMapLevels = (pPixelContainer->Format & X_D3DFORMAT_MIPMAP_MASK) >> X_D3DFORMAT_MIPMAP_SHIFT;
                dwDepth  = 1;// HACK? 1 << ((pPixelContainer->Format & X_D3DFORMAT_PSIZE_MASK) >> X_D3DFORMAT_PSIZE_SHIFT);
                dwPitch  = dwWidth*4;
                dwBPP = 4;
            }
            else if(X_Format == 0x05 /* X_D3DFMT_R5G6B5 */ || X_Format == 0x04 /* X_D3DFMT_A4R4G4B4 */
                 || X_Format == 0x1D /* X_D3DFMT_LIN_A4R4G4B4 */ || X_Format == 0x02 /* X_D3DFMT_A1R5G5B5 */
                 || X_Format == 0x28 /* X_D3DFMT_G8B8 */ || X_Format == 0x10 /* X_D3DFMT_LIN_A1R5G5B5 */)
            {
                bSwizzled = TRUE;

                // Swizzled 16 Bit
                dwWidth  = 1 << ((pPixelContainer->Format & X_D3DFORMAT_USIZE_MASK) >> X_D3DFORMAT_USIZE_SHIFT);
                dwHeight = 1 << ((pPixelContainer->Format & X_D3DFORMAT_VSIZE_MASK) >> X_D3DFORMAT_VSIZE_SHIFT);
                dwMipMapLevels = (pPixelContainer->Format & X_D3DFORMAT_MIPMAP_MASK) >> X_D3DFORMAT_MIPMAP_SHIFT;
                dwDepth  = 1;// HACK? 1 << ((pPixelContainer->Format & X_D3DFORMAT_PSIZE_MASK) >> X_D3DFORMAT_PSIZE_SHIFT);
                dwPitch  = dwWidth*2;
                dwBPP = 2;
            }
            else if(X_Format == 0x00 /* X_D3DFMT_L8 */ || X_Format == 0x0B /* X_D3DFMT_P8 */ || X_Format == 0x01 /* X_D3DFMT_AL8 */ || X_Format == 0x1A /* X_D3DFMT_A8L8 */)
            {
                bSwizzled = TRUE;

                // Swizzled 8 Bit
                dwWidth  = 1 << ((pPixelContainer->Format & X_D3DFORMAT_USIZE_MASK) >> X_D3DFORMAT_USIZE_SHIFT);
                dwHeight = 1 << ((pPixelContainer->Format & X_D3DFORMAT_VSIZE_MASK) >> X_D3DFORMAT_VSIZE_SHIFT);
                dwMipMapLevels = (pPixelContainer->Format & X_D3DFORMAT_MIPMAP_MASK) >> X_D3DFORMAT_MIPMAP_SHIFT;
                dwDepth  = 1;// HACK? 1 << ((pPixelContainer->Format & X_D3DFORMAT_PSIZE_MASK) >> X_D3DFORMAT_PSIZE_SHIFT);
                dwPitch  = dwWidth;
                dwBPP = 1;
            }
            else if(X_Format == 0x1E /* X_D3DFMT_LIN_X8R8G8B8 */ || X_Format == 0x12 /* X_D3DFORMAT_A8R8G8B8 */ 
				 || X_Format == 0x2E /* D3DFMT_LIN_D24S8 */ || X_Format == 0x3F /* X_D3DFMT_LIN_A8B8G8R8 */)
            {
                // Linear 32 Bit
                dwWidth  = (pPixelContainer->Size & X_D3DSIZE_WIDTH_MASK) + 1;
                dwHeight = ((pPixelContainer->Size & X_D3DSIZE_HEIGHT_MASK) >> X_D3DSIZE_HEIGHT_SHIFT) + 1;
                dwPitch  = (((pPixelContainer->Size & X_D3DSIZE_PITCH_MASK) >> X_D3DSIZE_PITCH_SHIFT)+1)*64;
                dwBPP = 4;
            }
            else if(X_Format == 0x11 /* D3DFMT_LIN_R5G6B5 */)
            {
                // Linear 16 Bit
                dwWidth  = (pPixelContainer->Size & X_D3DSIZE_WIDTH_MASK) + 1;
                dwHeight = ((pPixelContainer->Size & X_D3DSIZE_HEIGHT_MASK) >> X_D3DSIZE_HEIGHT_SHIFT) + 1;
                dwPitch  = (((pPixelContainer->Size & X_D3DSIZE_PITCH_MASK) >> X_D3DSIZE_PITCH_SHIFT)+1)*64;
                dwBPP = 2;
            }
            else if(X_Format == 0x0C /* D3DFMT_DXT1 */ || X_Format == 0x0E /* D3DFMT_DXT2 */ || X_Format == 0x0F /* D3DFMT_DXT3 */)
            {
                bCompressed = TRUE;

                // Compressed
                dwWidth  = 1 << ((pPixelContainer->Format & X_D3DFORMAT_USIZE_MASK) >> X_D3DFORMAT_USIZE_SHIFT);
                dwHeight = 1 << ((pPixelContainer->Format & X_D3DFORMAT_VSIZE_MASK) >> X_D3DFORMAT_VSIZE_SHIFT);
                dwDepth  = 1 << ((pPixelContainer->Format & X_D3DFORMAT_PSIZE_MASK) >> X_D3DFORMAT_PSIZE_SHIFT);
                dwMipMapLevels = (pPixelContainer->Format & X_D3DFORMAT_MIPMAP_MASK) >> X_D3DFORMAT_MIPMAP_SHIFT;

                // D3DFMT_DXT2...D3DFMT_DXT5 : 128bits per block/per 16 texels
                dwCompressedSize = dwWidth*dwHeight;

                if(X_Format == 0x0C)    // D3DFMT_DXT1 : 64bits per block/per 16 texels
                    dwCompressedSize /= 2;

                dwBPP = 1;
            }
            else if(X_Format == 0x24 /* D3DFMT_YUY2 */)
            {
                // Linear 32 Bit
                dwWidth  = (pPixelContainer->Size & X_D3DSIZE_WIDTH_MASK) + 1;
                dwHeight = ((pPixelContainer->Size & X_D3DSIZE_HEIGHT_MASK) >> X_D3DSIZE_HEIGHT_SHIFT) + 1;
                dwPitch  = (((pPixelContainer->Size & X_D3DSIZE_PITCH_MASK) >> X_D3DSIZE_PITCH_SHIFT)+1)*64;
            }
            else
            {
                CxbxKrnlCleanup("0x%.08X is not a supported format!\n", X_Format);
            }

            // as we iterate through mipmap levels, we'll adjust the source resource offset
            DWORD dwCompressedOffset = 0;

            DWORD dwMipOffs = 0;
            DWORD dwMipWidth = dwWidth;
            DWORD dwMipHeight = dwHeight;
            DWORD dwMipPitch = dwPitch;

            if(dwMipMapLevels > 6)
                dwMipMapLevels = 6;

            // iterate through the number of mipmap levels
            for(uint level=0;level<dwMipMapLevels;level++)
            {
                D3DLOCKED_RECT LockedRect;

                HRESULT hRet = pResource->EmuTexture8->LockRect(level, &LockedRect, NULL, 0);

                RECT  iRect  = {0,0,0,0};
                POINT iPoint = {0,0};

                BYTE *pSrc = (BYTE*)pTexture->Data;

                if( IsSpecialResource(pResource->Data) && (pResource->Data & X_D3DRESOURCE_DATA_FLAG_SURFACE))
                {

                }
                else
                {
                    if(bSwizzled)
                    {
                        if((DWORD)pSrc == 0x80000000)
                        {
                            // TODO: Fix or handle this situation..?
                        }
                        else
                        {
                            XTL::EmuXGUnswizzleRect
                            (
                                pSrc + dwMipOffs, dwMipWidth, dwMipHeight, dwDepth, LockedRect.pBits,
                                LockedRect.Pitch, iRect, iPoint, dwBPP
                            );
                        }
                    }
                    else if(bCompressed)
                    {
                        // NOTE: compressed size is (dwWidth/2)*(dwHeight/2)/2, so each level divides by 4

                        memcpy(LockedRect.pBits, pSrc + dwCompressedOffset, dwCompressedSize >> (level*2));

                        dwCompressedOffset += (dwCompressedSize >> (level*2));
                    }
                    else
                    {
                        BYTE *pDest = (BYTE*)LockedRect.pBits;

                        if((DWORD)LockedRect.Pitch == dwMipPitch && dwMipPitch == dwMipWidth*dwBPP)
                        {
                            memcpy(pDest, pSrc + dwMipOffs, dwMipWidth*dwMipHeight*dwBPP);
                        }
                        else
                        {
                            for(DWORD v=0;v<dwMipHeight;v++)
                            {
                                memcpy(pDest, pSrc + dwMipOffs, dwMipWidth*dwBPP);

                                pDest += LockedRect.Pitch;
                                pSrc  += dwMipPitch;
                            }
                        }
                    }
                }

                pResource->EmuTexture8->UnlockRect(level);

                dwMipOffs += dwMipWidth*dwMipHeight*dwBPP;

                dwMipWidth /= 2;
                dwMipHeight /= 2;
                dwMipPitch /= 2;
            }
        }

        g_pD3DDevice8->SetTexture(Stage, pTexture->EmuTexture8);
        //*/
    }
}