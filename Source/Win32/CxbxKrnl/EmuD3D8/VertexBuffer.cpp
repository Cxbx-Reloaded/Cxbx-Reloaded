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

// inline vertex buffer emulation
XTL::DWORD                  *XTL::g_pIVBVertexBuffer = 0;
XTL::X_D3DPRIMITIVETYPE      XTL::g_IVBPrimitiveType = 0;
UINT                         XTL::g_IVBTblOffs = 0;
struct XTL::_D3DIVB         *XTL::g_IVBTable = 0;

XTL::VertexPatcher::VertexPatcher()
{
    this->pOrigVertexBuffer8 = 0;
    this->pPatchedVertexBuffer8 = 0;
    this->uiStride = 0;
    this->bPatched = false;
    this->bAllocatedStreamZeroData = false;
}

XTL::VertexPatcher::~VertexPatcher()
{
}

bool XTL::VertexPatcher::Apply(VertexPatchDesc *pPatchDesc)
{
    // only quad and listloop are currently supported
    if((pPatchDesc->PrimitiveType != 8) && (pPatchDesc->PrimitiveType != 3))
        return false;

    uiStride = 0;

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
        g_pD3DDevice8->GetStreamSource(0, &pOrigVertexBuffer8, &uiStride);

        // Quad
        if(pPatchDesc->PrimitiveType == 8)
        {
            pPatchDesc->dwPrimitiveCount *= 2;

            // This is a list of sqares/rectangles, so we convert it to a list of triangles
            dwOriginalSize  = pPatchDesc->dwPrimitiveCount*uiStride*2;
            dwNewSize       = pPatchDesc->dwPrimitiveCount*uiStride*3;
        }
        // LineLoop
        else if(pPatchDesc->PrimitiveType == 3)
        {
            pPatchDesc->dwPrimitiveCount += 1;

            // We will add exactly one more line
            dwOriginalSize  = pPatchDesc->dwPrimitiveCount*uiStride;
            dwNewSize       = pPatchDesc->dwPrimitiveCount*uiStride + uiStride;
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

        g_pD3DDevice8->CreateVertexBuffer(dwNewSizeWR, 0, 0, XTL::D3DPOOL_MANAGED, &pPatchedVertexBuffer8);

        if(pOrigVertexBuffer8 != 0)
            pOrigVertexBuffer8->Lock(0, 0, &pOrigVertexData, 0);

        if(pPatchedVertexBuffer8 != 0)
            pPatchedVertexBuffer8->Lock(0, 0, &pPatchedVertexData, 0);
    }
    else
    {
        uiStride = pPatchDesc->uiVertexStreamZeroStride;

        if(pPatchDesc->PrimitiveType == 8)      // Quad
        {
            pPatchDesc->dwPrimitiveCount *= 2;

            // This is a list of sqares/rectangles, so we convert it to a list of triangles
            dwOriginalSize  = pPatchDesc->dwPrimitiveCount*uiStride*2;
            dwNewSize       = pPatchDesc->dwPrimitiveCount*uiStride*3;
        }
        else if(pPatchDesc->PrimitiveType == 3) // LineLoop
        {
            pPatchDesc->dwPrimitiveCount += 1;

            // We will add exactly one more line
            dwOriginalSize  = pPatchDesc->dwPrimitiveCount*uiStride;
            dwNewSize       = pPatchDesc->dwPrimitiveCount*uiStride + uiStride;
        }

        dwOriginalSizeWR = dwOriginalSize;
        dwNewSizeWR = dwNewSize;

        pNewVertexStreamZeroData = (uint08*)CxbxMalloc(dwNewSizeWR);
        this->bAllocatedStreamZeroData = true;

        pPatchedVertexData = (uint08*)pNewVertexStreamZeroData;
        pOrigVertexData = (uint08*)pPatchDesc->pVertexStreamZeroData;

        pPatchDesc->pVertexStreamZeroData = pPatchedVertexData;
    }

    DWORD dwVertexShader = NULL;

    g_pD3DDevice8->GetVertexShader(&dwVertexShader);

    // Copy the nonmodified data
    memcpy(pPatchedVertexData, pOrigVertexData, pPatchDesc->dwOffset);
    memcpy(&pPatchedVertexData[pPatchDesc->dwOffset+dwNewSize], 
           &pOrigVertexData[pPatchDesc->dwOffset+dwOriginalSize], 
           dwOriginalSizeWR - pPatchDesc->dwOffset - dwOriginalSize);

    // Quad
    if(pPatchDesc->PrimitiveType == 8)
    {
        uint08 *pPatch1 = &pPatchedVertexData[pPatchDesc->dwOffset+0*uiStride];
        uint08 *pPatch2 = &pPatchedVertexData[pPatchDesc->dwOffset+3*uiStride];
        uint08 *pPatch3 = &pPatchedVertexData[pPatchDesc->dwOffset+4*uiStride];
        uint08 *pPatch4 = &pPatchedVertexData[pPatchDesc->dwOffset+5*uiStride];

        uint08 *pOrig1 = &pOrigVertexData[pPatchDesc->dwOffset+0*uiStride];
        uint08 *pOrig2 = &pOrigVertexData[pPatchDesc->dwOffset+2*uiStride];
        uint08 *pOrig3 = &pOrigVertexData[pPatchDesc->dwOffset+3*uiStride];

        for(uint32 i=0;i<pPatchDesc->dwPrimitiveCount/2;i++)
        {
            memcpy(pPatch1, pOrig1, uiStride*3); // Vertex 0,1,2 := Vertex 0,1,2
            memcpy(pPatch2, pOrig2, uiStride);   // Vertex 3     := Vertex 2
            memcpy(pPatch3, pOrig3, uiStride);   // Vertex 4     := Vertex 3
            memcpy(pPatch4, pOrig1, uiStride);   // Vertex 5     := Vertex 0

            pPatch1 += uiStride*6;
            pPatch2 += uiStride*6;
            pPatch3 += uiStride*6;
            pPatch4 += uiStride*6;

            pOrig1 += uiStride*4;
            pOrig2 += uiStride*4;
            pOrig3 += uiStride*4;

            if(dwVertexShader & D3DFVF_XYZRHW)
            {
                for(int z=0;z<6;z++)
                {
                    if(((FLOAT*)&pPatchedVertexData[pPatchDesc->dwOffset+i*uiStride*6+z*uiStride])[2] == 0.0f)
                        ((FLOAT*)&pPatchedVertexData[pPatchDesc->dwOffset+i*uiStride*6+z*uiStride])[2] = 1.0f;
                    if(((FLOAT*)&pPatchedVertexData[pPatchDesc->dwOffset+i*uiStride*6+z*uiStride])[3] == 0.0f)
                        ((FLOAT*)&pPatchedVertexData[pPatchDesc->dwOffset+i*uiStride*6+z*uiStride])[3] = 1.0f;
                }
            }
        }
    }
    // LineLoop
    else if(pPatchDesc->PrimitiveType == 3)
    {
        memcpy(&pPatchedVertexData[pPatchDesc->dwOffset], &pOrigVertexData[pPatchDesc->dwOffset], dwOriginalSize);
        memcpy(&pPatchedVertexData[pPatchDesc->dwOffset + dwOriginalSize], &pOrigVertexData[pPatchDesc->dwOffset], uiStride);
    }

    if(pPatchDesc->pVertexStreamZeroData == 0)
    {
        pOrigVertexBuffer8->Unlock();
        pPatchedVertexBuffer8->Unlock();

        g_pD3DDevice8->SetStreamSource(0, pPatchedVertexBuffer8, uiStride);
    }

    this->bPatched = true;

    return true;
}

bool XTL::VertexPatcher::Restore()
{
    if(!this->bPatched)
        return false;

    if(pOrigVertexBuffer8 != 0 && pPatchedVertexBuffer8 != 0)
        g_pD3DDevice8->SetStreamSource(0, pOrigVertexBuffer8, uiStride);

    if(pOrigVertexBuffer8 != 0)
        pOrigVertexBuffer8->Release();

    if(pPatchedVertexBuffer8 != 0)
        pPatchedVertexBuffer8->Release();

    if(this->bAllocatedStreamZeroData)
        CxbxFree(pNewVertexStreamZeroData);

    return true;
}

VOID XTL::EmuFlushIVB()
{
    if(g_IVBPrimitiveType == 9 && g_IVBTblOffs == 4)
    {
        DWORD  dwShader = -1;
        DWORD *pdwVB = g_pIVBVertexBuffer;

        g_pD3DDevice8->GetVertexShader(&dwShader);

        UINT uiStride = 0;

        for(int v=0;v<4;v++)
        {
            DWORD dwPos = dwShader & D3DFVF_POSITION_MASK;

            if(dwPos == D3DFVF_XYZRHW)
            {
                *(FLOAT*)pdwVB++ = g_IVBTable[v].Position.x;
                *(FLOAT*)pdwVB++ = g_IVBTable[v].Position.y;
                *(FLOAT*)pdwVB++ = g_IVBTable[v].Position.z;

                uiStride += (sizeof(FLOAT)*3);

                DbgPrintf("IVB Position := {%f, %f, %f}\n", g_IVBTable[v].Position.x, g_IVBTable[v].Position.y, g_IVBTable[v].Position.z);
            }
            else
            {
                EmuCleanup("Unsupported Position Mask (FVF := 0x%.08X)", dwShader);
            }
            
            if(dwShader & D3DFVF_NORMAL)
            {
                *(FLOAT*)pdwVB++ = g_IVBTable[v].Normal.x;
                *(FLOAT*)pdwVB++ = g_IVBTable[v].Normal.y;
                *(FLOAT*)pdwVB++ = g_IVBTable[v].Normal.z;

                uiStride += sizeof(FLOAT)*3;

                DbgPrintf("IVB Normal := {%f, %f, %f}\n", g_IVBTable[v].Normal.x, g_IVBTable[v].Normal.y, g_IVBTable[v].Normal.z);
            }

            if(dwShader & D3DFVF_DIFFUSE)
            {
                *(DWORD*)pdwVB++ = g_IVBTable[v].dwDiffuse;
                
                DbgPrintf("IVB Diffuse := 0x%.08X\n", g_IVBTable[v].dwDiffuse);
            }

            if(dwShader & D3DFVF_SPECULAR)
            {
                *(DWORD*)pdwVB++ = g_IVBTable[v].dwSpecular;
                
                uiStride += sizeof(DWORD);

                DbgPrintf("IVB Specular := 0x%.08X\n", g_IVBTable[v].dwSpecular);
            }

            DWORD dwTexN = (dwShader & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;

            if(dwTexN >= 1)
            {
                *(FLOAT*)pdwVB++ = g_IVBTable[v].TexCoord1.x;
                *(FLOAT*)pdwVB++ = g_IVBTable[v].TexCoord1.y;

                uiStride += sizeof(FLOAT)*2;

                DbgPrintf("IVB TexCoord1 := {%f, %f}\n", g_IVBTable[v].TexCoord1.x, g_IVBTable[v].TexCoord1.y);
            }

            if(dwTexN >= 2)
            {
                *(FLOAT*)pdwVB++ = g_IVBTable[v].TexCoord2.x;
                *(FLOAT*)pdwVB++ = g_IVBTable[v].TexCoord2.y;

                uiStride += sizeof(FLOAT)*2;

                DbgPrintf("IVB TexCoord2 := {%f, %f}\n", g_IVBTable[v].TexCoord2.x, g_IVBTable[v].TexCoord2.y);
            }

            if(dwTexN >= 3)
            {
                *(FLOAT*)pdwVB++ = g_IVBTable[v].TexCoord3.x;
                *(FLOAT*)pdwVB++ = g_IVBTable[v].TexCoord3.y;

                uiStride += sizeof(FLOAT)*2;

                DbgPrintf("IVB TexCoord3 := {%f, %f}\n", g_IVBTable[v].TexCoord3.x, g_IVBTable[v].TexCoord3.y);
            }

            if(dwTexN >= 4)
            {
                *(FLOAT*)pdwVB++ = g_IVBTable[v].TexCoord4.x;
                *(FLOAT*)pdwVB++ = g_IVBTable[v].TexCoord4.y;

                uiStride += sizeof(FLOAT)*2;

                DbgPrintf("IVB TexCoord4 := {%f, %f}\n", g_IVBTable[v].TexCoord4.x, g_IVBTable[v].TexCoord4.y);
            }
        }

        /*
        static XTL::IDirect3DTexture8 *pTexture = 0;

        if(pTexture == 0)
        {
            g_pD3DDevice8->CreateTexture(512, 512, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &pTexture);

            IDirect3DSurface8 *pSurface = 0;;

            pTexture->GetSurfaceLevel(0, &pSurface);

            D3DXLoadSurfaceFromFileA(pSurface, NULL, NULL, "C:\\texture.bmp", NULL, D3DX_FILTER_NONE, 0, NULL);

            pSurface->Release();
        }

        g_pD3DDevice8->SetTexture(0, pTexture);
        //*/

        /*
        IDirect3DBaseTexture8 *pTexture = 0;

        g_pD3DDevice8->GetTexture(0, &pTexture);
        
        if(pTexture != NULL)
        {
            static int dwDumpTexture = 0;

            char szBuffer[255];

            sprintf(szBuffer, "C:\\Aaron\\Textures\\Texture-Active%.03d.bmp", dwDumpTexture++);

            D3DXSaveTextureToFile(szBuffer, D3DXIFF_BMP, pTexture, NULL);
        }
        //*/

        XTL::EmuUpdateDeferredStates();

        g_pD3DDevice8->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_pIVBVertexBuffer, uiStride);
        
        g_pD3DDevice8->Present(0,0,0,0);

        g_IVBTblOffs = 0;
    }

    return;
}