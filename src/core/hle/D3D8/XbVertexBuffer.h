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
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef XBVERTEXBUFFER_H
#define XBVERTEXBUFFER_H

#include <unordered_map>
#include <list>
#include <array>

#include "Cxbx.h"

#include "core\hle\D3D8\XbVertexShader.h"
#include "common\util\hasher.h" // For ComputeHash

typedef struct _CxbxDrawContext
{
    IN     xbox::X_D3DPRIMITIVETYPE    XboxPrimitiveType;
    IN     DWORD                 dwVertexCount;
    IN     DWORD                 dwStartVertex; // Only D3DDevice_DrawVertices sets this (potentially higher than default 0)
	IN	   PWORD				 pXboxIndexData; // Set by D3DDevice_DrawIndexedVertices, D3DDevice_DrawIndexedVerticesUP and HLE_draw_inline_elements
	IN	   DWORD				 dwBaseVertexIndex; // Set to g_Xbox_BaseVertexIndex in D3DDevice_DrawIndexedVertices
	IN	   INDEX16               LowIndex, HighIndex; // Set when pXboxIndexData is set
	IN	   UINT 				 NumVerticesToUse; // Set by CxbxVertexBufferConverter::Apply
    // Data if Draw...UP call
    IN PVOID                     pXboxVertexStreamZeroData;
    IN UINT                      uiXboxVertexStreamZeroStride;
	// Values to be used on host
	OUT PVOID                    pHostVertexStreamZeroData;
	OUT UINT                     uiHostVertexStreamZeroStride;
    OUT DWORD                    dwHostPrimitiveCount; // Set by CxbxVertexBufferConverter::Apply
}
CxbxDrawContext;

class CxbxPatchedStream
{
public:
    CxbxPatchedStream() = default;
    ~CxbxPatchedStream();
    void Clear();
    void Activate(CxbxDrawContext *pDrawContext, UINT HostStreamNumber) const;
    bool                    isValid = false;
    xbox::X_D3DPRIMITIVETYPE XboxPrimitiveType = xbox::X_D3DPT_NONE;
    PVOID                   pCachedXboxVertexData = xbox::zeroptr;
    UINT                    uiCachedXboxVertexDataSize = 0;
    uint64_t                uiVertexDataHash = 0;
    uint64_t                uiVertexStreamInformationHash = 0;
    UINT                    uiCachedXboxVertexStride = 0;
    UINT                    uiCachedHostVertexStride = 0;
    bool                    bCacheIsStreamZeroDrawUP = false;
    void                   *pCachedHostVertexStreamZeroData = nullptr;
    bool                    bCachedHostVertexStreamZeroDataIsAllocated = false;
    IDirect3DVertexBuffer  *pCachedHostVertexBuffer = nullptr;
};

class CxbxVertexBufferConverter
{
    public:
        CxbxVertexBufferConverter() = default;
        void Apply(CxbxDrawContext *pPatchDesc);
        void DrawCacheStats();
    private:
        struct StreamKey
        {
            uint64_t dataKey;
            uint64_t streamInfoKey;

            bool operator==(const StreamKey& rhs) const {
                return this->dataKey == rhs.dataKey && this->streamInfoKey == rhs.streamInfoKey;
            }
        };

        struct StreamKeyHash
        {
            std::size_t operator()(const StreamKey& k) const {
                return static_cast<std::size_t>(ComputeHash(&k, sizeof(k)));
            }
        };

        // Stack tracking
        ULONG m_TotalCacheHits = 0;
        ULONG m_TotalLookupSuccesses = 0;
        ULONG m_VertexStreamHashMisses = 0;
        ULONG m_DataNotInCacheMisses = 0;

        const UINT m_MaxCacheSize = 10000;                                        // Maximum number of entries in the cache
        const UINT m_CacheElasticity = 200;                                      // Cache is allowed to grow this much more than maximum before being purged to maximum
        std::unordered_map<StreamKey, std::list<CxbxPatchedStream>::iterator, StreamKeyHash> m_PatchedStreams; // Stores references to patched streams for fast lookup
        std::list<CxbxPatchedStream> m_PatchedStreamUsageList;             // Linked list of vertex streams, least recently used is last in the list
        CxbxPatchedStream& GetPatchedStream(uint64_t dataKey, uint64_t streamInfoKey); // Fetches (or inserts) a patched stream associated with the given key

        // Returns the number of streams of a patch
        UINT GetNbrStreams(CxbxDrawContext *pPatchDesc) const;

        // Patches the types of the stream
        void ConvertStream(CxbxDrawContext *pPatchDesc, CxbxVertexDeclaration* pCxbxVertexDeclaration, UINT uiStream);
};

extern CxbxVertexBufferConverter VertexBufferConverter;

// Inline vertex buffer emulation
extern xbox::X_D3DPRIMITIVETYPE      g_InlineVertexBuffer_PrimitiveType;

typedef struct _D3DIVB
{
    std::array<D3DXVECTOR4, 16> Slots;

} D3DIVB;

extern std::vector<D3DIVB> g_InlineVertexBuffer_Table;
extern UINT g_InlineVertexBuffer_TableLength;
extern UINT g_InlineVertexBuffer_TableOffset;

extern void EmuUpdateActiveTexture();

extern void CxbxSetVertexAttribute(int Register, FLOAT a, FLOAT b, FLOAT c, FLOAT d);

extern void CxbxImpl_Begin(xbox::X_D3DPRIMITIVETYPE PrimitiveType);
extern void CxbxImpl_End();
extern void CxbxImpl_SetStreamSource(UINT StreamNumber, xbox::X_D3DVertexBuffer* pStreamData, UINT Stride);
extern void CxbxImpl_SetVertexData4f(int Register, FLOAT a, FLOAT b, FLOAT c, FLOAT d);

extern DWORD g_dwPrimPerFrame;

#endif
