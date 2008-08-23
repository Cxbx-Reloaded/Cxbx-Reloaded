// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuD3D8->VertexBuffer.h
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
#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include "Cxbx.h"
//#include <time.h> // Conflict with io.h

#define MAX_NBR_STREAMS 16

typedef struct _VertexPatchDesc
{
    IN     X_D3DPRIMITIVETYPE    PrimitiveType;
    IN     DWORD                 dwVertexCount;
    IN     DWORD                 dwPrimitiveCount;
    IN     DWORD                 dwOffset;
    // Data if Draw...UP call
    IN OUT PVOID                 pVertexStreamZeroData;
    IN     UINT                  uiVertexStreamZeroStride;
    // The current vertex shader, used to identify the streams
    IN     DWORD                 hVertexShader;
}
VertexPatchDesc;

typedef struct _PATCHEDSTREAM
{
    IDirect3DVertexBuffer8 *pOriginalStream;
    IDirect3DVertexBuffer8 *pPatchedStream;
    UINT                    uiOrigStride;
    UINT                    uiNewStride;
    bool                    bUsedCached;
} PATCHEDSTREAM;

typedef struct _CACHEDSTREAM
{
    uint32         uiCRC32;
    uint32         uiCheckFrequency;
    uint32         uiCacheHit;
    bool           bIsUP;
    PATCHEDSTREAM  Stream;
    void          *pStreamUP;           // Draw..UP (instead of pOriginalStream)
    uint32         uiLength;            // The length of the stream
    uint32         uiCount;             // CRC32 check count
    uint32         dwPrimitiveCount;
    long           lLastUsed;           // For cache removal purposes
} CACHEDSTREAM;

class VertexPatcher
{
    public:
        VertexPatcher();
       ~VertexPatcher();

        bool Apply(VertexPatchDesc *pPatchDesc);
        bool Restore();

        // Dumps the cache to the console
        static void DumpCache(void);

    private:

        UINT m_uiNbrStreams;
        PATCHEDSTREAM m_pStreams[MAX_NBR_STREAMS];

        PVOID m_pNewVertexStreamZeroData;

        bool m_bPatched;
        bool m_bAllocatedStreamZeroData;

        VERTEX_DYNAMIC_PATCH *m_pDynamicPatch;

        // Returns the number of streams of a patch
        UINT GetNbrStreams(VertexPatchDesc *pPatchDesc);

        // Caches a patched stream
        void CacheStream(VertexPatchDesc *pPatchDesc,
                         UINT             uiStream);

        // Frees a cached, patched stream
        void FreeCachedStream(void *pStream);

        // Tries to apply a previously patched stream from the cache
        bool ApplyCachedStream(VertexPatchDesc *pPatchDesc,
                               UINT             uiStream);

        // Patches the types of the stream
        bool PatchStream(VertexPatchDesc *pPatchDesc, UINT uiStream);

        // Patches the primitive of the stream
        bool PatchPrimitive(VertexPatchDesc *pPatchDesc, UINT uiStream);
};

// inline vertex buffer emulation
extern DWORD                  *g_pIVBVertexBuffer;
extern X_D3DPRIMITIVETYPE      g_IVBPrimitiveType;
extern DWORD                   g_IVBFVF;

extern struct _D3DIVB
{
    XTL::D3DXVECTOR3 Position;   // Position
    FLOAT            Rhw;        // Rhw
    XTL::DWORD       dwSpecular; // Specular
    XTL::DWORD       dwDiffuse;  // Diffuse
    XTL::D3DXVECTOR3 Normal;     // Normal
    XTL::D3DXVECTOR2 TexCoord1;  // TexCoord1
    XTL::D3DXVECTOR2 TexCoord2;  // TexCoord2
    XTL::D3DXVECTOR2 TexCoord3;  // TexCoord3
    XTL::D3DXVECTOR2 TexCoord4;  // TexCoord4
}
*g_IVBTable;

extern UINT g_IVBTblOffs;

extern VOID EmuFlushIVB();

extern VOID EmuUpdateActiveTexture();

#endif