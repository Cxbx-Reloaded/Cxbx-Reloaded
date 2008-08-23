// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuD3D->State.cpp
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

#include "CxbxKrnl/Emu.h"
#include "CxbxKrnl/EmuXTL.h"

// deferred state lookup tables
DWORD *XTL::EmuD3DDeferredRenderState;
DWORD *XTL::EmuD3DDeferredTextureState;

// ******************************************************************
// * func: EmuUpdateDeferredStates
// ******************************************************************
void XTL::EmuUpdateDeferredStates()
{
    using namespace XTL;

    // Certain D3DRS values need to be checked on each Draw[Indexed]Vertices
    if(EmuD3DDeferredRenderState != 0)
    {
        if(XTL::EmuD3DDeferredRenderState[0] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_FOGENABLE, XTL::EmuD3DDeferredRenderState[0]);

        if(XTL::EmuD3DDeferredRenderState[1] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_FOGTABLEMODE, XTL::EmuD3DDeferredRenderState[1]);

        if(XTL::EmuD3DDeferredRenderState[2] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_FOGSTART, XTL::EmuD3DDeferredRenderState[2]);

        if(XTL::EmuD3DDeferredRenderState[3] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_FOGEND, XTL::EmuD3DDeferredRenderState[3]);

        if(XTL::EmuD3DDeferredRenderState[6] != X_D3DRS_UNK)
        {
            ::DWORD dwConv = 0;

            dwConv |= (XTL::EmuD3DDeferredRenderState[6] & 0x00000010) ? D3DWRAP_U : 0;
            dwConv |= (XTL::EmuD3DDeferredRenderState[6] & 0x00001000) ? D3DWRAP_V : 0;
            dwConv |= (XTL::EmuD3DDeferredRenderState[6] & 0x00100000) ? D3DWRAP_W : 0;

            g_pD3DDevice8->SetRenderState(D3DRS_WRAP0, dwConv);
        }

        if(XTL::EmuD3DDeferredRenderState[10] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_LIGHTING, XTL::EmuD3DDeferredRenderState[10]);

        if(XTL::EmuD3DDeferredRenderState[11] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_SPECULARENABLE, XTL::EmuD3DDeferredRenderState[11]);

        if(XTL::EmuD3DDeferredRenderState[13] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_COLORVERTEX, XTL::EmuD3DDeferredRenderState[13]);

        if(XTL::EmuD3DDeferredRenderState[20] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, XTL::EmuD3DDeferredRenderState[20]);

        if(XTL::EmuD3DDeferredRenderState[23] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_AMBIENT, XTL::EmuD3DDeferredRenderState[23]);

        if(XTL::EmuD3DDeferredRenderState[24] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_POINTSIZE, XTL::EmuD3DDeferredRenderState[24]);

        if(XTL::EmuD3DDeferredRenderState[25] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_POINTSIZE_MIN, XTL::EmuD3DDeferredRenderState[25]);

        if(XTL::EmuD3DDeferredRenderState[26] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_POINTSPRITEENABLE, XTL::EmuD3DDeferredRenderState[26]);

        if(XTL::EmuD3DDeferredRenderState[27] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_POINTSCALEENABLE, XTL::EmuD3DDeferredRenderState[27]);

        if(XTL::EmuD3DDeferredRenderState[28] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_POINTSCALE_A, XTL::EmuD3DDeferredRenderState[28]);

        if(XTL::EmuD3DDeferredRenderState[29] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_POINTSCALE_B, XTL::EmuD3DDeferredRenderState[29]);

        if(XTL::EmuD3DDeferredRenderState[30] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_POINTSCALE_C, XTL::EmuD3DDeferredRenderState[30]);

        if(XTL::EmuD3DDeferredRenderState[31] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_POINTSIZE_MAX, XTL::EmuD3DDeferredRenderState[31]);

        if(XTL::EmuD3DDeferredRenderState[33] != X_D3DRS_UNK)
            g_pD3DDevice8->SetRenderState(D3DRS_PATCHSEGMENTS, XTL::EmuD3DDeferredRenderState[33]);

        /** To check for unhandled RenderStates
        for(int v=0;v<117-82;v++)
        {
            if(XTL::EmuD3DDeferredRenderState[v] != X_D3DRS_UNK)
            {
                if(v != 0  && v != 1  && v != 2 && v != 3 && v != 6  && v != 10 && v != 11 && v != 13
                && v != 20 && v != 23 && v != 24 && v != 25 && v != 26 && v != 27 && v != 28 && v != 29
                && v != 30 && v != 31 && v != 33)
                    EmuWarning("Unhandled RenderState Change @ %d (%d)", v, v + 82);
            }
        }
        //**/
    }

    // Certain D3DTS values need to be checked on each Draw[Indexed]Vertices
    if(EmuD3DDeferredTextureState != 0)
    {
        for(int v=0;v<4;v++)
        {
            ::DWORD *pCur = &EmuD3DDeferredTextureState[v*32];

            if(pCur[0] != X_D3DTSS_UNK)
            {
                if(pCur[0] == 5)
                    CxbxKrnlCleanup("ClampToEdge is unsupported (temporarily)");

                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_ADDRESSU, pCur[0]);
            }

            if(pCur[1] != X_D3DTSS_UNK)
            {
                if(pCur[1] == 5)
                    CxbxKrnlCleanup("ClampToEdge is unsupported (temporarily)");

                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_ADDRESSV, pCur[1]);
            }

            if(pCur[2] != X_D3DTSS_UNK)
            {
                if(pCur[2] == 5)
                    CxbxKrnlCleanup("ClampToEdge is unsupported (temporarily)");

                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_ADDRESSW, pCur[2]);
            }

            if(pCur[3] != X_D3DTSS_UNK)
            {
                if(pCur[3] == 4)
                    CxbxKrnlCleanup("QuinCunx is unsupported (temporarily)");

                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_MAGFILTER, pCur[3]);
            }

            if(pCur[4] != X_D3DTSS_UNK)
            {
                if(pCur[4] == 4)
                    CxbxKrnlCleanup("QuinCunx is unsupported (temporarily)");

                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_MINFILTER, pCur[4]);
            }

            if(pCur[5] != X_D3DTSS_UNK)
            {
                if(pCur[5] == 4)
                    CxbxKrnlCleanup("QuinCunx is unsupported (temporarily)");

                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_MIPFILTER, pCur[5]);
            }

            if(pCur[6] != X_D3DTSS_UNK)
                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_MIPMAPLODBIAS, pCur[6]);

            if(pCur[7] != X_D3DTSS_UNK)
                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_MAXMIPLEVEL, pCur[7]);

            if(pCur[8] != X_D3DTSS_UNK)
                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_MAXANISOTROPY, pCur[8]);

            // TODO: Use a lookup table, this is not always a 1:1 map
            if(pCur[12] != X_D3DTSS_UNK)
            {
                if(pCur[12] > 12 && !(pCur[12] >= 17 && pCur[12] <= 21))
                    CxbxKrnlCleanup("(Temporarily) Unsupported D3DTSS_COLOROP Value (%d)", pCur[12]);

                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_COLOROP, pCur[12]);
            }

            if(pCur[13] != X_D3DTSS_UNK)
                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_COLORARG0, pCur[13]);

            if(pCur[14] != X_D3DTSS_UNK)
                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_COLORARG1, pCur[14]);

            if(pCur[15] != X_D3DTSS_UNK)
                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_COLORARG2, pCur[15]);

            // TODO: Use a lookup table, this is not always a 1:1 map (same as D3DTSS_COLOROP)
            if(pCur[16] != X_D3DTSS_UNK)
            {
                if(pCur[16] > 12)
                    CxbxKrnlCleanup("(Temporarily) Unsupported D3DTSS_ALPHAOP Value (%d)", pCur[16]);

                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_ALPHAOP, pCur[16]);
            }

            if(pCur[17] != X_D3DTSS_UNK)
                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_ALPHAARG0, pCur[17]);

            if(pCur[18] != X_D3DTSS_UNK)
                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_ALPHAARG1, pCur[18]);

            if(pCur[19] != X_D3DTSS_UNK)
                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_ALPHAARG2, pCur[19]);

            if(pCur[20] != X_D3DTSS_UNK)
                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_RESULTARG, pCur[20]);

            if(pCur[21] != X_D3DTSS_UNK)
                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_TEXTURETRANSFORMFLAGS, pCur[21]);

            if(pCur[29] != X_D3DTSS_UNK)
                g_pD3DDevice8->SetTextureStageState(v, D3DTSS_BORDERCOLOR, pCur[29]);

            /** To check for unhandled texture stage state changes
            for(int r=0;r<32;r++)
            {
                static const int unchecked[] =
                {
                    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 29, 30, 31
                };

                if(pCur[r] != X_D3DTSS_UNK)
                {
                    bool pass = true;

                    for(int q=0;q<sizeof(unchecked)/sizeof(int);q++)
                    {
                        if(r == unchecked[q])
                        {
                            pass = false;
                            break;
                        }
                    }

                    if(pass)
                        EmuWarning("Unhandled TextureState Change @ %d->%d", v, r);
                }
            }
            //**/
        }

        // if point sprites are enabled, copy stage 3 over to 0
        if(EmuD3DDeferredRenderState[26] == TRUE)
        {
            // pCur = Texture Stage 3 States
            ::DWORD *pCur = &EmuD3DDeferredTextureState[2*32];

            IDirect3DBaseTexture8 *pTexture;

            // set the point sprites texture
            g_pD3DDevice8->GetTexture(3, &pTexture);
            g_pD3DDevice8->SetTexture(0, pTexture);

            // disable all other stages
            g_pD3DDevice8->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
            g_pD3DDevice8->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

            // in that case we have to copy over the stage by hand
            for(int v=0;v<30;v++)
            {
                if(pCur[v] != X_D3DTSS_UNK)
                {
                    ::DWORD dwValue;

                    g_pD3DDevice8->GetTextureStageState(3, (D3DTEXTURESTAGESTATETYPE)v, &dwValue);
                    g_pD3DDevice8->SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE)v, dwValue);
                }
            }
        }
    }

    if(g_bFakePixelShaderLoaded)
    {
        g_pD3DDevice8->SetRenderState(D3DRS_FOGENABLE, FALSE);

        // programmable pipeline
        //*
        for(int v=0;v<4;v++)
        {
            g_pD3DDevice8->SetTextureStageState(v, D3DTSS_COLOROP, D3DTOP_DISABLE);
            g_pD3DDevice8->SetTextureStageState(v, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
        }
        //*/

        // fixed pipeline
        /*
        for(int v=0;v<4;v++)
        {
            g_pD3DDevice8->SetTextureStageState(v, D3DTSS_COLOROP,   D3DTOP_MODULATE);
            g_pD3DDevice8->SetTextureStageState(v, D3DTSS_COLORARG1, D3DTA_TEXTURE);
            g_pD3DDevice8->SetTextureStageState(v, D3DTSS_COLORARG2, D3DTA_CURRENT);

            g_pD3DDevice8->SetTextureStageState(v, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
            g_pD3DDevice8->SetTextureStageState(v, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
            g_pD3DDevice8->SetTextureStageState(v, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
        }

        g_pD3DDevice8->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
        g_pD3DDevice8->SetRenderState(D3DRS_LIGHTING,TRUE);
        g_pD3DDevice8->SetRenderState(D3DRS_AMBIENT, 0xFFFFFFFF);
        //*/
    }
}
