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

#include "Emu.h"
#include "EmuAlloc.h"
#include "EmuXTL.h"
#include "ResourceTracker.h"
#include <time.h>

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
            EmuCleanup("Could not retrieve original buffer size");
        }
        if(FAILED(pOrigVertexBuffer->Lock(0, 0, (uint08**)&pCalculateData, 0)))
        {
            EmuCleanup("Couldn't lock the original buffer");
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
            EmuCleanup("Trying to patch a Draw..UP with more than stream zero!");
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
            EmuCleanup("Could not retrieve original buffer size");
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
            EmuCleanup("Trying to find a cached Draw..UP with more than stream zero!");
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
                    EmuCleanup("Couldn't lock the original buffer");
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
    PATCHEDSTREAM *pStream = &m_pStreams[uiStream];
    if(!m_pDynamicPatch)
    {
        return false;
    }

    if(!VshHandleIsVertexShader(pPatchDesc->hVertexShader))
    {
        // No need to patch FVF types, there are no xbox extensions
        return false;
    }

    if(!m_pDynamicPatch->pStreamPatches[uiStream].NeedPatch)
    {
        return false;
    }
    // Do some groovey patchin'
    IDirect3DVertexBuffer8    *pOrigVertexBuffer;
    IDirect3DVertexBuffer8    *pNewVertexBuffer;
    uint08                    *pOrigData;
    uint08                    *pNewData;
    UINT                       uiStride;
    XTL::D3DVERTEXBUFFER_DESC  Desc;
    STREAM_DYNAMIC_PATCH      *pStreamPatch = &m_pDynamicPatch->pStreamPatches[uiStream];
    DWORD dwNewSize;

    if(!pPatchDesc->pVertexStreamZeroData)
    {
        g_pD3DDevice8->GetStreamSource(uiStream, &pOrigVertexBuffer, &uiStride);
        if(FAILED(pOrigVertexBuffer->GetDesc(&Desc)))
        {
            EmuCleanup("Could not retrieve original buffer size");
        }
        // Set a new (exact) vertex count
        pPatchDesc->dwVertexCount = Desc.Size / uiStride;
        dwNewSize = pPatchDesc->dwVertexCount * pStreamPatch->ConvertedStride;

        if(FAILED(pOrigVertexBuffer->Lock(0, 0, &pOrigData, 0)))
        {
            EmuCleanup("Couldn't lock the original buffer");
        }
        g_pD3DDevice8->CreateVertexBuffer(dwNewSize, 0, 0, XTL::D3DPOOL_MANAGED, &pNewVertexBuffer);
        if(FAILED(pNewVertexBuffer->Lock(0, 0, &pNewData, 0)))
        {
            EmuCleanup("Couldn't lock the new buffer");
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
            EmuCleanup("Trying to patch a Draw..UP with more than stream zero!");
        }
        uiStride  = pPatchDesc->uiVertexStreamZeroStride;
        pOrigData = (uint08 *)pPatchDesc->pVertexStreamZeroData;
        // TODO: This is sometimes the number of indices, which isn't too good
        dwNewSize = pPatchDesc->dwVertexCount * pStreamPatch->ConvertedStride;
        pNewVertexBuffer = NULL;
        pNewData = (uint08*)CxbxMalloc(dwNewSize);
        if(!pNewData)
        {
            EmuCleanup("Couldn't allocate the new stream zero buffer");
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
                    EmuCleanup("Unhandled stream type: 0x%.02X\n", pStreamPatch->pTypes[uiType]);
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
            EmuCleanup("Failed to set the type patched buffer as the new stream source!\n");
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

bool XTL::VertexPatcher::PatchPrimitive(VertexPatchDesc *pPatchDesc,
                                        UINT             uiStream)
{
    PATCHEDSTREAM *pStream = &m_pStreams[uiStream];
    // only quad and listloop are currently supported
    if((pPatchDesc->PrimitiveType != X_D3DPT_QUADLIST) && (pPatchDesc->PrimitiveType != X_D3DPT_LINELOOP))
        return false;

    if(pPatchDesc->pVertexStreamZeroData && uiStream > 0)
    {
        EmuCleanup("Draw..UP call with more than one stream!\n");
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

        if(pPatchDesc->PrimitiveType == X_D3DPT_QUADLIST)
        {
            pPatchDesc->dwPrimitiveCount *= 2;

            // This is a list of sqares/rectangles, so we convert it to a list of triangles
            dwOriginalSize  = pPatchDesc->dwPrimitiveCount * pStream->uiOrigStride * 2;
            dwNewSize       = pPatchDesc->dwPrimitiveCount * pStream->uiOrigStride * 3;
        }
        // LineLoop
        else if(pPatchDesc->PrimitiveType == X_D3DPT_LINELOOP)
        {
            pPatchDesc->dwPrimitiveCount += 1;

            // We will add exactly one more line
            dwOriginalSize  = pPatchDesc->dwPrimitiveCount * pStream->uiOrigStride;
            dwNewSize       = pPatchDesc->dwPrimitiveCount * pStream->uiOrigStride + pStream->uiOrigStride;
        }

        // Retrieve the original buffer size
        {
            XTL::D3DVERTEXBUFFER_DESC Desc;

            if(FAILED(pStream->pOriginalStream->GetDesc(&Desc)))
            {
                EmuCleanup("Could not retrieve buffer size");
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
        pStream->uiOrigStride = pPatchDesc->uiVertexStreamZeroStride;

        if(pPatchDesc->PrimitiveType == X_D3DPT_QUADLIST)
        {
            pPatchDesc->dwPrimitiveCount *= 2;

            // This is a list of sqares/rectangles, so we convert it to a list of triangles
            dwOriginalSize  = pPatchDesc->dwPrimitiveCount * pStream->uiOrigStride * 2;
            dwNewSize       = pPatchDesc->dwPrimitiveCount * pStream->uiOrigStride * 3;
        }
        else if(pPatchDesc->PrimitiveType == X_D3DPT_LINELOOP) // LineLoop
        {
            pPatchDesc->dwPrimitiveCount += 1;

            // We will add exactly one more line
            dwOriginalSize  = pPatchDesc->dwPrimitiveCount * pStream->uiOrigStride;
            dwNewSize       = pPatchDesc->dwPrimitiveCount * pStream->uiOrigStride + pStream->uiOrigStride;
        }

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

    // Quad
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
    // LineLoop
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
    if((g_IVBPrimitiveType == X_D3DPT_QUADLIST) && (g_IVBTblOffs == 4))
    {
        XTL::EmuUpdateDeferredStates();

        DWORD *pdwVB = (DWORD*)g_IVBTable;

        UINT uiStride = 0;

        for(int v=0;v<4;v++)
        {
            DWORD dwPos = g_IVBFVF & D3DFVF_POSITION_MASK;

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
            else
            {
                EmuCleanup("Unsupported Position Mask (FVF := 0x%.08X)", g_IVBFVF);
            }

            if(g_IVBFVF & D3DFVF_DIFFUSE)
            {
                *(DWORD*)pdwVB++ = g_IVBTable[v].dwDiffuse;

                if(v == 0)
                {
                    uiStride += sizeof(DWORD);
                }

                DbgPrintf("IVB Diffuse := 0x%.08X\n", g_IVBTable[v].dwDiffuse);
            }

            DWORD dwTexN = (g_IVBFVF & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;

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
        }

        g_pD3DDevice8->SetVertexShader(g_IVBFVF);
        g_pD3DDevice8->SetPixelShader(NULL);

        // patch buffer
        UINT PrimitiveCount = EmuD3DVertex2PrimitiveCount(g_IVBPrimitiveType, 4);

        VertexPatchDesc VPDesc;

        VPDesc.dwVertexCount = 4;
        VPDesc.PrimitiveType = g_IVBPrimitiveType;
        VPDesc.dwPrimitiveCount = PrimitiveCount;
        VPDesc.dwOffset = 0;
        VPDesc.pVertexStreamZeroData = g_IVBTable;
        VPDesc.uiVertexStreamZeroStride = uiStride;
        // TODO: Set the current shader and let the patcher handle it..
        VPDesc.hVertexShader = g_IVBFVF;

        VertexPatcher VertPatch;

        bool bPatched = VertPatch.Apply(&VPDesc);

        g_pD3DDevice8->DrawPrimitiveUP(D3DPT_TRIANGLELIST, VPDesc.dwPrimitiveCount, VPDesc.pVertexStreamZeroData, VPDesc.uiVertexStreamZeroStride);

        VertPatch.Restore();

        // ignore
        g_IVBTblOffs = 0;
    }

    return;
}
