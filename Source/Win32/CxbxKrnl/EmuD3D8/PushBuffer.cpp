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
#include "EmuXTL.h"
#include "ResourceTracker.h"

bool XTL::g_bStepPush = false;
bool XTL::g_bSkipPush = false;
bool XTL::g_bBrkPush  = false;

bool g_bPBSkipPusher = false;

// pushbuffer execution emulation
void XTL::EmuExecutePushBuffer
(
    X_D3DPushBuffer       *pPushBuffer,
    X_D3DFixup            *pFixup
)
{
    if(g_bSkipPush)
        return;

    if(pFixup != NULL)
        EmuCleanup("PushBuffer has fixups\n");

    DWORD *pdwPushData = (DWORD*)pPushBuffer->Data;

    D3DPRIMITIVETYPE    PCPrimitiveType = (D3DPRIMITIVETYPE)-1;
    X_D3DPRIMITIVETYPE  XBPrimitiveType = -1;

    // TODO: This technically should be enabled
    XTL::EmuUpdateDeferredStates();

//	g_pD3DDevice8->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0, NULL);
//	g_pD3DDevice8->SetRenderState(D3DRS_ZENABLE, FALSE);
//	g_pD3DDevice8->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    #ifdef _DEBUG_TRACK_PB
    bool bShowPB = false;

    g_PBTrackTotal.insert(pdwPushData);

    if(g_PBTrackShowOnce.exists(pdwPushData))
    {
        g_PBTrackShowOnce.remove(pdwPushData);

        printf("\n");
        printf("\n");
        printf("  PushBuffer@0x%.08X...\n", pdwPushData);
        printf("\n");

        bShowPB = true;
    }
    #endif

    // NOTE: I believe 0x1808 is actually a seperate command, but I need to verify this.

    while(true)
    {
        DWORD dwCount = (*pdwPushData >> 18);
        DWORD dwMethod = (*pdwPushData & 0x3FFFF);

        // Interpret GPU Instruction
        if(dwMethod == 0x000017FC) // NVPB_SetBeginEnd
        {
            pdwPushData++;

            #ifdef _DEBUG_TRACK_PB
            if(bShowPB)
            {
                printf("  NVPB_SetBeginEnd(");
            }
            #endif

            if(*pdwPushData == 0)
            {
                #ifdef _DEBUG_TRACK_PB
                if(bShowPB)
                {
                    printf("DONE)\n");
                }
                #endif
                break;  // done?
            }
            else
            {
                #ifdef _DEBUG_TRACK_PB
                if(bShowPB)
                {
                    printf("PrimitiveType := %d)\n", *pdwPushData);
                }
                #endif

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

            #ifdef _DEBUG_TRACK_PB
            if(bShowPB)
            {
                printf("  NVPB_InlineArray(0x%.08X)...\n", pIndexData);
                printf("\n");
                printf("  Index Array Data...\n");

                WORD *pwVal = (WORD*)pIndexData;

                for(uint s=0;s<dwCount/2;s++)
                {
                    if(s%8 == 0) printf("\n  ");

                    printf("  %.04X", *pwVal++);
                }

                printf("\n");

                XTL::IDirect3DVertexBuffer8 *pActiveVB = NULL;

                D3DVERTEXBUFFER_DESC VBDesc;

                BYTE *pVBData = 0;
                UINT  uiStride;

                // retrieve stream data
                g_pD3DDevice8->GetStreamSource(0, &pActiveVB, &uiStride);

                // retrieve stream desc
                pActiveVB->GetDesc(&VBDesc);

                // unlock just in case
                pActiveVB->Unlock();

                // grab ptr
                pActiveVB->Lock(0, 0, &pVBData, D3DLOCK_READONLY);

                // print out stream data
                {
                    printf("\n");
                    printf("  Vertex Stream Data (0x%.08X)...\n", pActiveVB);
                    printf("\n");
                    printf("  Format : %d\n", VBDesc.Format);
                    printf("  Size   : %d bytes\n", VBDesc.Size);
                    printf("  FVF    : 0x%.08X\n", VBDesc.FVF);
                    printf("\n");
                }

                // release ptr
                pActiveVB->Unlock();
            }
            #endif

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

                    #ifdef _DEBUG_TRACK_PB
                    if(!g_PBTrackDisable.exists((PVOID)pPushBuffer->Data))
                    {
                    #endif

                    if(!g_bPBSkipPusher)
                    {
                        if(IsValidCurrentShader())
                        {
                            //g_pD3DDevice8->SetVertexShader(D3DFVF_XYZ | D3DFVF_TEX0);

                            /* these don't help the turok lighting problems :[
                            g_pD3DDevice8->SetRenderState(D3DRS_FOGENABLE, FALSE);
                            g_pD3DDevice8->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
                            g_pD3DDevice8->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
                            g_pD3DDevice8->SetRenderState(D3DRS_ZVISIBLE, FALSE);
                            g_pD3DDevice8->SetRenderState(D3DRS_RANGEFOGENABLE, FALSE);
                            g_pD3DDevice8->SetRenderState(D3DRS_AMBIENT, RGB(255,125,125));
                            g_pD3DDevice8->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);

                            D3DMATERIAL8 mtrl;

                            ZeroMemory( &mtrl, sizeof(D3DMATERIAL8) );

                            mtrl.Diffuse.r = mtrl.Ambient.r = mtrl.Specular.r = 1.0f;
                            mtrl.Diffuse.g = mtrl.Ambient.g = mtrl.Specular.r = 0.0f;
                            mtrl.Diffuse.b = mtrl.Ambient.b = mtrl.Specular.r = 0.0f;
                            mtrl.Diffuse.a = mtrl.Ambient.a = mtrl.Specular.r = 1.0f;

                            g_pD3DDevice8->SetMaterial(&mtrl);
                            //*/

                            g_pD3DDevice8->DrawIndexedPrimitive
                            (
                                PCPrimitiveType, 0, dwCount*2, 0, EmuD3DVertex2PrimitiveCount(XBPrimitiveType, dwCount*2)
                            );
                        }
                    }

                    #ifdef _DEBUG_TRACK_PB
                    }
                    #endif

                    g_pD3DDevice8->SetIndices(0, 0);
                }

                pIndexBuffer->Release();
            }
        }

        pdwPushData++;
    }

    #ifdef _DEBUG_TRACK_PB
    if(bShowPB)
    {
        printf("\n");
        printf("CxbxDbg> ");
        fflush(stdout);
    }
    #endif

    if(g_bStepPush)
    {
        g_pD3DDevice8->Present(0,0,0,0);
        Sleep(500);
    }
}
