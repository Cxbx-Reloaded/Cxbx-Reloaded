// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuD3D8PushBuffer.cpp
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
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_LOCAL_

// prevent name collisions
namespace xboxkrnl
{
    #include <xboxkrnl/xboxkrnl.h>
};

#include "Emu.h"
#include "EmuFS.h"
#include "EmuShared.h"

// prevent name collisions
namespace XTL
{
    #include "EmuXTL.h"
};

extern XTL::LPDIRECT3DDEVICE8 g_pD3DDevice8;  // Direct3D8 Device

// pushbuffer execution emulation
void XTL::EmuExecutePushBuffer
(
    X_D3DPushBuffer       *pPushBuffer,
    PVOID                  pFixup
)
{
    DWORD *pdwPushData = (DWORD*)pPushBuffer->Data;

    DWORD dwIndices = 0, dwBytes = 0;
    PVOID pIndexData = 0;

    D3DPRIMITIVETYPE    PCPrimitiveType = (D3DPRIMITIVETYPE)-1;
    X_D3DPRIMITIVETYPE  XBPrimitiveType = -1;

    while((DWORD)pdwPushData < ((DWORD)pPushBuffer->Data + pPushBuffer->Size))
    {
        // Interpret GPU Instruction
        switch(*pdwPushData++)
        {
            // NVPB_DrawVertices
            case 0x000417FC:
            {
                if(*pdwPushData == 0)
                {
                    pdwPushData++;

                    // Null instruction
                    if( (*pdwPushData) == 0x00000000)
                    {
                        pdwPushData++;
                    }
                    // Skip Bytes
                    else if( (*pdwPushData & 0x40000100) == 0x40000100)
                    {
                        DWORD dwSkipBytes = (*pdwPushData & 0x1FFF0000) >> 16;

                        // advance to argument
                        pdwPushData++;

                        // argument doesnt matter
                        pdwPushData++;

                        // skip over given data
                        pdwPushData += dwSkipBytes/4;
                    }
                }
                else
                {
                    XBPrimitiveType = *pdwPushData++;
                    PCPrimitiveType = EmuPrimitiveType(XBPrimitiveType);

                    if( (*pdwPushData & 0x40001800) != 0x40001800)
                    {
                        EmuCleanup("Error in PushBuffer interpretter");
                    }

                    // Parse Index Data
                    while( (*pdwPushData & 0x40001800) == 0x40001800)
                    {
                        dwBytes = ((*pdwPushData & 0x1FFF0000) >> 16);

                        if(PCPrimitiveType == D3DPT_TRIANGLESTRIP)
                            dwIndices = dwBytes/2;

                        // argument doesnt matter
                        pdwPushData++;

                        // assign index data buffer
                        pIndexData = (PVOID)pdwPushData;

                        // advance past index data
                        pdwPushData += dwBytes/4;

                        LPDIRECT3DINDEXBUFFER8 pIndexBuffer=0;

                        HRESULT hRet = g_pD3DDevice8->CreateIndexBuffer(dwBytes, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pIndexBuffer);

                        if(FAILED(hRet))
                            EmuCleanup("Unable to create index buffer for PushBuffer emulation\n");

                        // copy index data
                        {
                            WORD *pData=0;

                            pIndexBuffer->Lock(0, dwBytes, (UCHAR**)&pData, NULL);

                            memcpy(pData, pIndexData, dwBytes);

                            pIndexBuffer->Unlock();
                        }

                        // render indexed vertices
                        {
                            g_pD3DDevice8->SetIndices(pIndexBuffer, 0);

                            g_pD3DDevice8->DrawIndexedPrimitive
                            (
                                PCPrimitiveType, 0, dwIndices, 0, EmuD3DVertex2PrimitiveCount(XBPrimitiveType, dwIndices)
                            );
                        }

                        // cleanup
                        pIndexBuffer->Release();
                    
                    }
                }
            }
            break;

            // NVPB_Unknown
            case 0x00041808:
            {
                WORD wUnknown = (WORD)*pdwPushData++;

//                EmuCleanup("GPU Instruction Mask 0x00041808 Unhandled (D3DDevice_DrawIndexedVertices)");
            }
            break;

            // NVPB_Unknown
            default:
            {
                EmuCleanup("GPU Instruction Mask 0x%.08X Unhandled", *(pdwPushData-1));
            }
            break;
        }
    }
}