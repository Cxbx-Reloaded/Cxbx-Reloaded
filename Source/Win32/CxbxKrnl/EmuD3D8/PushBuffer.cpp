// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuD3D->PushBuffer.cpp
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
#define _XBOXKRNL_DEFEXTRN_

#include "Emu.h"

// prevent name collisions
namespace XTL
{
    #include "EmuXTL.h"
};

extern XTL::LPDIRECT3DDEVICE8 g_pD3DDevice8;  // Direct3D8 Device

bool XTL::g_bStepPush = FALSE;
bool XTL::g_bSkipPush = FALSE;
bool XTL::g_bBrkPush  = FALSE;

// pushbuffer execution emulation
void XTL::EmuExecutePushBuffer
(
    X_D3DPushBuffer       *pPushBuffer,
    PVOID                  pFixup
)
{
    if(g_bSkipPush)
        return;

    DWORD *pdwPushData = (DWORD*)pPushBuffer->Data;

    D3DPRIMITIVETYPE    PCPrimitiveType = (D3DPRIMITIVETYPE)-1;
    X_D3DPRIMITIVETYPE  XBPrimitiveType = -1;

    // TODO: This technically should be enabled
    //XTL::EmuUpdateDeferredStates();

//	g_pD3DDevice8->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0, NULL);
//	g_pD3DDevice8->SetRenderState(D3DRS_ZENABLE, FALSE);
//	g_pD3DDevice8->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    // NOTE: I believe 0x1808 is actually a seperate command, but I need to verify this.

    while(true)
    {
        DWORD dwCount = (*pdwPushData >> 18);
        DWORD dwMethod = (*pdwPushData & 0x3FFFF);

        // Interpret GPU Instruction
        if(dwMethod == 0x000017FC) // NVPB_SetBeginEnd
        {
            pdwPushData++;

            if(*pdwPushData == 0)
                break;  // done?
            else
            {
                XBPrimitiveType = *pdwPushData;
                PCPrimitiveType = EmuPrimitiveType(XBPrimitiveType);
            }
        }
        else // NVPB_InlineArray
        {
            PVOID pIndexData = 0;
            BOOL bInc = *pdwPushData & 0x40000000;

            BOOL bFix = (dwMethod != 0x00001818);

            if(bFix)
            {
                if(bInc)
                    dwCount = ((*pdwPushData - (0x40000000 | 0x00001818)) >> 18);

                dwMethod = 0x1818;
            }

            pdwPushData++;

            pIndexData = pdwPushData;

            pdwPushData += dwCount - (bInc ? 0 : 1);

            // perform rendering
            {
                LPDIRECT3DINDEXBUFFER8 pIndexBuffer=0;

                HRESULT hRet = g_pD3DDevice8->CreateIndexBuffer(dwCount*4, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pIndexBuffer);

                if(FAILED(hRet))
                    EmuCleanup("Unable to create index buffer for PushBuffer emulation\n");

                // copy index data
                {
                    WORD *pData=0;

                    pIndexBuffer->Lock(0, dwCount*4, (UCHAR**)&pData, NULL);

                    memcpy(pData, pIndexData, dwCount*4);

                    pIndexBuffer->Unlock();
                }

                // render indexed vertices
                {
                    g_pD3DDevice8->SetIndices(pIndexBuffer, 0);

                    g_pD3DDevice8->DrawIndexedPrimitive
                    (
                        PCPrimitiveType, 0, dwCount*2, 0, EmuD3DVertex2PrimitiveCount(XBPrimitiveType, dwCount*2)
                    );
                }

                // cleanup
                pIndexBuffer->Release();
            }
        }

        pdwPushData++;
    }

    if(g_bStepPush)
    {
        g_pD3DDevice8->Present(0,0,0,0);
        Sleep(500);
    }
}
