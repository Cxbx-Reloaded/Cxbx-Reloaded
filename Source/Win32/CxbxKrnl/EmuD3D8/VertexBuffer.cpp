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

// prevent name collisions
namespace XTL
{
    #include "EmuXTL.h"
};

extern XTL::LPDIRECT3DDEVICE8 g_pD3DDevice8;  // Direct3D8 Device

// fixup xbox extensions to be compatible with PC direct3d
UINT XTL::EmuFixupVerticesA
(
    DWORD                           PrimitiveType,
    UINT                           &PrimitiveCount,
    XTL::IDirect3DVertexBuffer8   *&pOrigVertexBuffer8,
    XTL::IDirect3DVertexBuffer8   *&pHackVertexBuffer8,
    UINT                            dwOffset,
    PVOID                           pVertexStreamZeroData,
    UINT                            uiVertexStreamZeroStride, 
    PVOID                          *ppNewVertexStreamZeroData
)
{
    // only quad and listloop are supported right now
    if(PrimitiveType != 8 && PrimitiveType != 3)
        return -1;

    // stride of this stream source
    UINT uiStride = 0;

    // sizes of our part in the vertex buffer
    DWORD dwOriginalSize    = 0;
    DWORD dwNewSize         = 0;

    // sizes with the rest of the buffer
    DWORD dwOriginalSizeWR  = 0;
    DWORD dwNewSizeWR       = 0;

    // vertex data arrays
    BYTE *pOrigVertexData = 0;
    BYTE *pHackVertexData = 0;

    if(pVertexStreamZeroData == 0)
    {
        g_pD3DDevice8->GetStreamSource(0, &pOrigVertexBuffer8, &uiStride);

        if(PrimitiveType == 8)      // Quad
        {
            PrimitiveCount *= 2;

            // This is a list of sqares/rectangles, so we convert it to a list of triangles
            dwOriginalSize  = PrimitiveCount*uiStride*2;
            dwNewSize       = PrimitiveCount*uiStride*3;
        }
        else if(PrimitiveType == 3) // LineLoop
        {
            PrimitiveCount += 1;

            // We will add exactly one more line
            dwOriginalSize  = PrimitiveCount*uiStride;
            dwNewSize       = PrimitiveCount*uiStride + uiStride;
        }

        // Retrieve the original buffer size
        {
            XTL::D3DVERTEXBUFFER_DESC Desc;

            if(FAILED(pOrigVertexBuffer8->GetDesc(&Desc))) 
                EmuCleanup("Could not retrieve buffer size");

            // Here we save the full buffer size
            dwOriginalSizeWR = Desc.Size;

            // So we can now calculate the size of the rest (dwOriginalSizeWR - dwOriginalSize) and
            // add it to our new calculated size of the patched buffer
            dwNewSizeWR = dwNewSize + dwOriginalSizeWR - dwOriginalSize;
        }

        g_pD3DDevice8->CreateVertexBuffer(dwNewSizeWR, 0, 0, XTL::D3DPOOL_MANAGED, &pHackVertexBuffer8);

        if(pOrigVertexBuffer8 != 0)
            pOrigVertexBuffer8->Lock(0, 0, &pOrigVertexData, 0);

        if(pHackVertexBuffer8 != 0)
            pHackVertexBuffer8->Lock(0, 0, &pHackVertexData, 0);
    }
    else
    {
        uiStride = uiVertexStreamZeroStride;

        if(PrimitiveType == 8)      // Quad
        {
            PrimitiveCount *= 2;

            // This is a list of sqares/rectangles, so we convert it to a list of triangles
            dwOriginalSize  = PrimitiveCount*uiStride*2;
            dwNewSize       = PrimitiveCount*uiStride*3;
        }
        else if(PrimitiveType == 3) // LineLoop
        {
            PrimitiveCount += 1;

            // We will add exactly one more line
            dwOriginalSize  = PrimitiveCount*uiStride;
            dwNewSize       = PrimitiveCount*uiStride + uiStride;
        }

        dwOriginalSizeWR = dwOriginalSize;
        dwNewSizeWR = dwNewSize;

        pHackVertexData = (uint08*)CxbxMalloc(dwNewSizeWR);
        pOrigVertexData = (uint08*)pVertexStreamZeroData;

        *ppNewVertexStreamZeroData = pHackVertexData;
    }

    DWORD dwVertexShader = NULL;

    g_pD3DDevice8->GetVertexShader(&dwVertexShader);

    // Copy the nonmodified data
    memcpy(pHackVertexData, pOrigVertexData, dwOffset);
    memcpy(&pHackVertexData[dwOffset+dwNewSize], &pOrigVertexData[dwOffset+dwOriginalSize], dwOriginalSizeWR-dwOffset-dwOriginalSize);

    if(PrimitiveType == 8)      // Quad
    {
        uint08 *pHack1 = &pHackVertexData[dwOffset+0*uiStride];
        uint08 *pHack2 = &pHackVertexData[dwOffset+3*uiStride];
        uint08 *pHack3 = &pHackVertexData[dwOffset+4*uiStride];
        uint08 *pHack4 = &pHackVertexData[dwOffset+5*uiStride];

        uint08 *pOrig1 = &pOrigVertexData[dwOffset+0*uiStride];
        uint08 *pOrig2 = &pOrigVertexData[dwOffset+2*uiStride];
        uint08 *pOrig3 = &pOrigVertexData[dwOffset+3*uiStride];

        for(uint32 i=0;i<PrimitiveCount/2;i++)
        {
            memcpy(pHack1, pOrig1, uiStride*3); // Vertex 0,1,2 := Vertex 0,1,2
            memcpy(pHack2, pOrig2, uiStride);   // Vertex 3     := Vertex 2
            memcpy(pHack3, pOrig3, uiStride);   // Vertex 4     := Vertex 3
            memcpy(pHack4, pOrig1, uiStride);   // Vertex 5     := Vertex 0

            pHack1 += uiStride*6;
            pHack2 += uiStride*6;
            pHack3 += uiStride*6;
            pHack4 += uiStride*6;

            pOrig1 += uiStride*4;
            pOrig2 += uiStride*4;
            pOrig3 += uiStride*4;

            if(dwVertexShader & D3DFVF_XYZRHW)
            {
                for(int z=0;z<6;z++)
                {
                    if(((FLOAT*)&pHackVertexData[dwOffset+i*uiStride*6+z*uiStride])[2] == 0.0f)
                        ((FLOAT*)&pHackVertexData[dwOffset+i*uiStride*6+z*uiStride])[2] = 1.0f;
                    if(((FLOAT*)&pHackVertexData[dwOffset+i*uiStride*6+z*uiStride])[3] == 0.0f)
                        ((FLOAT*)&pHackVertexData[dwOffset+i*uiStride*6+z*uiStride])[3] = 1.0f;
                }
            }
        }
    }
    else if(PrimitiveType == 3)
    {
        memcpy(&pHackVertexData[dwOffset], &pOrigVertexData[dwOffset], dwOriginalSize);
        memcpy(&pHackVertexData[dwOffset + dwOriginalSize], &pOrigVertexData[dwOffset], uiStride);
    }

    if(pVertexStreamZeroData == 0)
    {
        pOrigVertexBuffer8->Unlock();
        pHackVertexBuffer8->Unlock();

        g_pD3DDevice8->SetStreamSource(0, pHackVertexBuffer8, uiStride);
    }

    return uiStride;
}

// fixup xbox extensions to be compatible with PC direct3d
VOID XTL::EmuFixupVerticesB
(
    UINT                            nStride,
    XTL::IDirect3DVertexBuffer8   *&pOrigVertexBuffer8,
    XTL::IDirect3DVertexBuffer8   *&pHackVertexBuffer8
)
{
    if(pOrigVertexBuffer8 != 0 && pHackVertexBuffer8 != 0)
        g_pD3DDevice8->SetStreamSource(0, pOrigVertexBuffer8, nStride);

    if(pOrigVertexBuffer8 != 0)
        pOrigVertexBuffer8->Release();

    if(pHackVertexBuffer8 != 0)
        pHackVertexBuffer8->Release();
}
