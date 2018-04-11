// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->D3D8LTCG.1.0.4627.inl
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
// *  (c) 2017 jarupxx
// *
// *  All rights reserved
// *
// ******************************************************************

// ******************************************************************
// * Direct3D_CreateDevice
// ******************************************************************
//85C9750AC705 ...C20400
OOVPA_NO_XREF(Direct3D_CreateDevice_4, 2060, 7)

        { 0x00, 0x56 },

        { 0x0A, 0x85 },
        { 0x0B, 0xC9 },
        { 0x0C, 0x75 },
        { 0x0D, 0x0A },
        { 0x0E, 0xC7 },
        { 0x0F, 0x05 },
OOVPA_END;

// ******************************************************************
// * D3DTexture_GetLevelDesc
// ******************************************************************
// * NOTE: We are actually intercepting Get2DSurfaceDesc, because
// *       GetLevelDesc Simply redirects to that function
// ******************************************************************
//7909C74608010000 ...C20400
OOVPA_NO_XREF(Get2DSurfaceDesc_4, 2048, 10)

        { 0x00, 0x0F },
        { 0x01, 0xB6 },

        { 0x2F, 0x79 },
        { 0x30, 0x09 },
        { 0x31, 0xC7 },
        { 0x32, 0x46 },
        { 0x33, 0x08 },
        { 0x34, 0x01 },
        { 0x35, 0x00 },
        { 0x36, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_ZBias
// ******************************************************************
//24..8BF07D06D805
OOVPA_NO_XREF(D3DDevice_SetRenderState_ZBias, 1060, 9)

        // XREF_ENTRY( 0x6A, XREF_D3DRS_ZBIAS ),

        { 0x00, 0x51 },
        { 0x01, 0x53 },

        { 0x18, 0x24 },
        //{ 0x19, 0x18 },
        { 0x1A, 0x8B },
        { 0x1B, 0xF0 },
        { 0x1C, 0x7D },
        { 0x1D, 0x06 },
        { 0x1E, 0xD8 },
        { 0x1F, 0x05 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_Dxt1NoiseEnable
// ******************************************************************
//0FB6480D8A91
OOVPA_NO_XREF(D3DDevice_SetRenderState_Dxt1NoiseEnable, 1024, 7)

        // XREF_ENTRY( 0x6F, XREF_D3DRS_DXT1NOISEENABLE ),

        { 0x00, 0x53 },

        { 0x13, 0x0F },
        { 0x14, 0xB6 },
        { 0x15, 0x48 },
        { 0x16, 0x0D },
        { 0x17, 0x8A },
        { 0x18, 0x91 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleAntiAlias
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_MultiSampleAntiAlias, 1060, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0D, XREF_D3DRS_MULTISAMPLEANTIALIAS ),

        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },
        { 0x04, 0x56 },
        { 0x05, 0x8B },
        { 0x06, 0x35 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMask
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_MultiSampleMask, 1060, 1+6,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0C, XREF_D3DRS_MULTISAMPLEMASK ),

        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },
        { 0x04, 0x57 },
        { 0x05, 0x8B },
        { 0x06, 0x3D },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMask
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_MultiSampleMask, 1072, 1+6,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0B, XREF_D3DRS_MULTISAMPLEMASK ),

        { 0x01, 0x44 },
        { 0x0F, 0xC1 },
        { 0x10, 0xE0 },
        { 0x11, 0x10 },
        { 0x12, 0x56 },
        { 0x13, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_SampleAlpha
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_SampleAlpha, 1024, 1+4,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0C, XREF_D3DRS_SAMPLEALPHA ),

        { 0x00, 0x8B },
        { 0x04, 0x57 },
        { 0x05, 0x8B },
        { 0x06, 0x3D },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_SampleAlpha
// ******************************************************************
//C1E610578B3D
OOVPA_NO_XREF(D3DDevice_SetRenderState_SampleAlpha, 1036, 8)

        { 0x00, 0x8B },
        { 0x01, 0x44 },

        { 0x0B, 0xC1 },
        { 0x0C, 0xE6 },
        { 0x0D, 0x10 },
        { 0x0E, 0x57 },
        { 0x0F, 0x8B },
        { 0x10, 0x3D },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_BorderColor
// ******************************************************************
//088BD1C1E20681C2241B
OOVPA_NO_XREF(D3DDevice_SetTextureState_BorderColor, 1024, 11)

        { 0x00, 0x56 },

        { 0x1F, 0x08 },
        { 0x20, 0x8B },
        { 0x21, 0xD1 },
        { 0x22, 0xC1 },
        { 0x23, 0xE2 },
        { 0x24, 0x06 },
        { 0x25, 0x81 },
        { 0x26, 0xC2 },
        { 0x27, 0x24 },
        { 0x28, 0x1B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_BumpEnv
// ******************************************************************
//8d5e01f6c303
OOVPA_NO_XREF(D3DDevice_SetTextureState_BumpEnv, 1024, 7)

        { 0x02, 0x56 },

        { 0x1A, 0x8D },
        { 0x1B, 0x5E },
        { 0x1C, 0x01 },
        { 0x1D, 0xF6 },
        { 0x1E, 0xC3 },
        { 0x1F, 0x03 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_TexCoordIndex
// ******************************************************************
//c1e0078998
OOVPA_XREF(D3DDevice_SetTextureState_TexCoordIndex, 1958, 1+10,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_D3DTSS_TEXCOORDINDEX ), // Derived

        { 0x00, 0x53 },
        { 0x08, 0x35 },

        { 0x14, 0xC1 },
        { 0x15, 0xE0 },
        { 0x16, 0x07 },
        { 0x17, 0x89 },
        { 0x18, 0x98 },

        { 0x1D, 0x8B },
        { 0x1E, 0x06 },
        { 0x1F, 0x8B },
OOVPA_END;

//******************************************************************
//* D3DDevice_SetRenderTarget
//******************************************************************
//8B4424088B4C240468
OOVPA_NO_XREF(D3DDevice_SetRenderTarget, 1060, 15)

        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x08 },
        { 0x04, 0x8B },
        { 0x05, 0x4C },
        { 0x06, 0x24 },
        { 0x07, 0x04 },
        { 0x08, 0x68 },

        { 0x0D, 0x50 },
        { 0x0E, 0x51 },
        { 0x0F, 0xE8 },

        { 0x14, 0xC2 },
        { 0x15, 0x08 },
        { 0x16, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderTargetFast
// ******************************************************************
//0F85..0100008B0D
OOVPA_XREF(D3DDevice_SetRenderTargetFast, 1036, 1+11,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x4F, XREF_D3DRS_MULTISAMPLEMODE ),

        { 0x00, 0x83 },
        { 0x01, 0xEC },
        { 0x02, 0x20 },

        { 0x47, 0x0F },
        { 0x48, 0x85 },
        //{ 0x49, 0x4B },
        { 0x4A, 0x01 },
        { 0x4B, 0x00 },
        { 0x4C, 0x00 },
        { 0x4D, 0x8B },
        { 0x4E, 0x0D },

        { 0x53, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetScissors
// ******************************************************************
//C700B402040089
OOVPA_NO_XREF(D3DDevice_SetScissors, 1036, 8)

        { 0x00, 0x83 },
        { 0x01, 0xEC },

        { 0x88, 0xC7 },
        { 0x89, 0x00 },
        { 0x8A, 0xB4 },
        { 0x8B, 0x02 },
        { 0x8C, 0x04 },
        { 0x8D, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTexture
// ******************************************************************
//81C10000F8FF89
OOVPA_NO_XREF(D3DDevice_SetTexture, 1024, 9)

        { 0x00, 0x83 },
        { 0x01, 0xEC },

        { 0x28, 0x81 },
        { 0x29, 0xC1 },
        { 0x2A, 0x00 },
        { 0x2B, 0x00 },
        { 0x2C, 0xF8 },
        { 0x2D, 0xFF },
        { 0x2E, 0x89 },
OOVPA_END;

//******************************************************************
//* D3DDevice_CopyRects
//******************************************************************
//81EC940100008B842498
OOVPA_NO_XREF(D3DDevice_CopyRects, 1036, 10)

        { 0x00, 0x81 },
        { 0x01, 0xEC },
        { 0x02, 0x94 },
        { 0x03, 0x01 },
        { 0x04, 0x00 },
        { 0x05, 0x00 },
        { 0x06, 0x8B },
        { 0x07, 0x84 },
        { 0x08, 0x24 },
        { 0x09, 0x98 },
OOVPA_END;

//******************************************************************
//* D3DDevice_BeginVisibilityTest
//******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginVisibilityTest, 1024, 8)

        { 0x08, 0x06 },
        { 0x1C, 0xC7 },
        { 0x1D, 0x00 },
        { 0x1E, 0xC8 },
        { 0x1F, 0x17 },
        { 0x20, 0x08 },
        { 0x21, 0x00 },
        { 0x22, 0xB9 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateTexture2
// ******************************************************************
//F744241C0000010074
OOVPA_NO_XREF(D3DDevice_CreateTexture2, 1048, 11)

        { 0x00, 0x56 },
        { 0x01, 0x57 },

        { 0x3D, 0xF7 },
        { 0x3E, 0x44 },
        { 0x3F, 0x24 },
        { 0x40, 0x1C },
        { 0x41, 0x00 },
        { 0x42, 0x00 },
        { 0x43, 0x01 },
        { 0x44, 0x00 },
        { 0x45, 0x74 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DeleteStateBlock
// ******************************************************************
//535556578B7C241433ED33DB
OOVPA_NO_XREF(D3DDevice_DeleteStateBlock, 1024, 12)

        { 0x00, 0x53 },
        { 0x01, 0x55 },
        { 0x02, 0x56 },
        { 0x03, 0x57 },
        { 0x04, 0x8B },
        { 0x05, 0x7C },
        { 0x06, 0x24 },
        { 0x07, 0x14 },
        { 0x08, 0x33 },
        { 0x09, 0xED },
        { 0x0A, 0x33 },
        { 0x0B, 0xDB },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawIndexedVerticesUP
// ******************************************************************
//8B4D148B560883
OOVPA_NO_XREF(D3DDevice_DrawIndexedVerticesUP, 1036, 9)

        { 0x00, 0x55 },
        { 0x0F, 0xF8 },

        { 0x36, 0x8B },
        { 0x37, 0x4D },
        { 0x38, 0x14 },
        { 0x39, 0x8B },
        { 0x3A, 0x56 },
        { 0x3B, 0x08 },
        { 0x3C, 0x83 },
OOVPA_END;

//******************************************************************
//* D3DDevice_DrawVerticesUP
//******************************************************************
//8B8E7C0700008B
OOVPA_NO_XREF(D3DDevice_DrawVerticesUP, 1036, 9)

        { 0x00, 0x55 },
        { 0x0F, 0xEC },

        { 0x36, 0x8B },
        { 0x37, 0x8E },
        { 0x38, 0x7C },
        { 0x39, 0x07 },
        { 0x3A, 0x00 },
        { 0x3B, 0x00 },
        { 0x3C, 0x8B },
OOVPA_END;

//******************************************************************
//* D3DDevice_EndVisibilityTest
//******************************************************************
OOVPA_NO_XREF(D3DDevice_EndVisibilityTest, 1024, 7)

        { 0x01, 0x44 },
        { 0x10, 0xB8 },
        { 0x11, 0x0E },
        { 0x12, 0x00 },
        { 0x13, 0x07 },
        { 0x14, 0x80 },
        { 0x15, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetBackBuffer2
// ******************************************************************
//7507B801000000EB07F7
OOVPA_NO_XREF(D3DDevice_GetBackBuffer2, 1036, 14)

        { 0x00, 0x8B },
        { 0x01, 0x44 },

        { 0x0D, 0x75 },
        { 0x0E, 0x07 },
        { 0x0F, 0xB8 },
        { 0x10, 0x01 },
        { 0x11, 0x00 },
        { 0x12, 0x00 },
        { 0x13, 0x00 },
        { 0x14, 0xEB },
        { 0x15, 0x07 },
        { 0x16, 0xF7 },

        { 0x4C, 0xC2 },
        { 0x4D, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetBackBuffer2
// ******************************************************************
//7507B801000000EB07F7 ...C3
OOVPA_NO_XREF(D3DDevice_GetBackBuffer2_0, 2024, 13)

        { 0x00, 0x83 },
        { 0x01, 0xF8 },

        { 0x09, 0x75 },
        { 0x0A, 0x07 },
        { 0x0B, 0xB8 },
        { 0x0C, 0x01 },
        { 0x0D, 0x00 },
        { 0x0E, 0x00 },
        { 0x0F, 0x00 },
        { 0x10, 0xEB },
        { 0x11, 0x07 },
        { 0x12, 0xF7 },

        { 0x48, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetDepthStencilSurface2
// ******************************************************************
//85F6750433C05EC3
OOVPA_NO_XREF(D3DDevice_GetDepthStencilSurface2, 1048, 10)

        { 0x00, 0xA1 },

        { 0x0B, 0x00 },
        { 0x0C, 0x85 },
        { 0x0D, 0xF6 },
        { 0x0E, 0x75 },
        { 0x0F, 0x04 },
        { 0x10, 0x33 },
        { 0x11, 0xC0 },
        { 0x12, 0x5E },
        { 0x13, 0xC3 },
OOVPA_END;

//******************************************************************
//* D3DDevice_GetDisplayMode
//******************************************************************
OOVPA_NO_XREF(D3DDevice_GetDisplayMode, 1024, 9)
//8B90181A0000
        { 0x00, 0xA1 },

        { 0x05, 0x8B },
        { 0x06, 0x90 },
        //{ 0x07, 0x18 },
        //{ 0x08, 0x1A },
        { 0x09, 0x00 },
        { 0x0A, 0x00 },
        { 0x0D, 0x10 },
        { 0x0E, 0x85 },
        { 0x12, 0x8B },
        { 0x13, 0x4A },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetTransform
// ******************************************************************
//56C1E106578B7C24108DB4
OOVPA_NO_XREF(D3DDevice_GetTransform, 1024, 12)

        { 0x00, 0x8B },

        { 0x09, 0x56 },
        { 0x0A, 0xC1 },
        { 0x0B, 0xE1 },
        { 0x0C, 0x06 },
        { 0x0D, 0x57 },
        { 0x0E, 0x8B },
        { 0x0F, 0x7C },
        { 0x10, 0x24 },
        { 0x11, 0x10 },
        { 0x12, 0x8D },
        { 0x13, 0xB4 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_IsBusy
// ******************************************************************
//8B88BC2300008B90
OOVPA_NO_XREF(D3DDevice_IsBusy, 1024, 9)

        { 0x00, 0xA1 },
        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0xBC },
        { 0x08, 0x23 },
        { 0x09, 0x00 },
        { 0x0A, 0x00 },
        { 0x0B, 0x8B },
        { 0x0C, 0x90 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_LightEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_LightEnable, 1024, 11)

        { 0x00, 0x83 },
        { 0x01, 0xEC },
        { 0x02, 0x68 },
        { 0x03, 0x53 },
        { 0x04, 0x8B },
        { 0x05, 0x5C },
        { 0x06, 0x24 },
        { 0x07, 0x70 },
        { 0x08, 0x56 },
        { 0x09, 0x8B },
        { 0x0A, 0x35 },
OOVPA_END;

//******************************************************************
//* D3DDevice_LoadVertexShaderProgram
//******************************************************************
//8B542404530FB75A0255
OOVPA_NO_XREF(D3DDevice_LoadVertexShaderProgram, 1036, 10)

        { 0x00, 0x8B },
        { 0x01, 0x54 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },
        { 0x04, 0x53 },
        { 0x05, 0x0F },
        { 0x06, 0xB7 },
        { 0x07, 0x5A },
        { 0x08, 0x02 },
        { 0x09, 0x55 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_PersistDisplay
// ******************************************************************
//85C0740F50FF15 ...C3
OOVPA_NO_XREF(D3DDevice_PersistDisplay_0, 2048, 9)

        { 0x00, 0x53 },
        { 0x01, 0x8B },

        { 0x0D, 0x85 },
        { 0x0E, 0xC0 },
        { 0x0F, 0x74 },
        { 0x10, 0x0F },
        { 0x11, 0x50 },
        { 0x12, 0xFF },
        { 0x13, 0x15 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Reset
// ******************************************************************
//803F6A006A036A006A00E8
OOVPA_NO_XREF(D3DDevice_Reset, 1024, 10)

        { 0x00, 0x53 },
        { 0x01, 0x8B },

        { 0x99, 0x6A },
        { 0x9A, 0x00 },
        { 0x9B, 0x68 },
        { 0x9C, 0x00 },
        { 0x9D, 0x00 },
        { 0x9E, 0x80 },
        { 0x9F, 0x3F },
        { 0xA0, 0x6A },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Reset
// ******************************************************************
//803F6A006A036A006A00E8 ...C3
OOVPA_NO_XREF(D3DDevice_Reset_0, 2024, 10)

        { 0x00, 0x53 },
        { 0x01, 0x8B },

        { 0x92, 0x6A },
        { 0x93, 0x00 },
        { 0x94, 0x68 },
        { 0x95, 0x00 },
        { 0x96, 0x00 },
        { 0x97, 0x80 },
        { 0x98, 0x3F },
        { 0x99, 0x6A },
OOVPA_END;

// ******************************************************************
// * D3DDevice_RunVertexStateShader
// ******************************************************************
//C740EC801E1000D9 ...C20400
OOVPA_NO_XREF(D3DDevice_RunVertexStateShader_4, 2048, 9)

        { 0x00, 0x57 },

        { 0x2E, 0xC7 },
        { 0x2F, 0x40 },
        { 0x30, 0xEC },
        { 0x31, 0x80 },
        { 0x32, 0x1E },
        { 0x33, 0x10 },
        { 0x34, 0x00 },
        { 0x35, 0xD9 },
OOVPA_END;

//******************************************************************
//* D3DDevice_SetGammaRamp
//******************************************************************
OOVPA_NO_XREF(D3DDevice_SetGammaRamp, 1024, 8)

        { 0x01, 0x15 },
        { 0x06, 0x53 },
        { 0x07, 0x8B },
        { 0x08, 0x9A },
        //{ 0x09, 0x78 },
        //{ 0x0A, 0x24 },
        { 0x0B, 0x00 },
        { 0x0C, 0x00 },
        { 0x0D, 0x8D },
        { 0x0E, 0x82 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPixelShader
// ******************************************************************
//C740040000210083C008
OOVPA_NO_XREF(D3DDevice_SetPixelShader, 1036, 12)

        { 0x00, 0x51 },
        { 0x01, 0x53 },

        { 0x76, 0xC7 },
        { 0x77, 0x40 },
        { 0x78, 0x04 },
        { 0x79, 0x00 },
        { 0x7A, 0x00 },
        { 0x7B, 0x21 },
        { 0x7C, 0x00 },
        { 0x7D, 0x83 },
        { 0x7E, 0xC0 },
        { 0x7F, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateVertexShader
// ******************************************************************
//C740040000210083C008 ...C3
OOVPA_NO_XREF(D3DDevice_SetPixelShader_0, 2024, 12)

        { 0x00, 0x51 },
        { 0x01, 0x85 },

        { 0x72, 0xC7 },
        { 0x73, 0x40 },
        { 0x74, 0x04 },
        { 0x75, 0x00 },
        { 0x76, 0x00 },
        { 0x77, 0x21 },
        { 0x78, 0x00 },
        { 0x79, 0x83 },
        { 0x7A, 0xC0 },
        { 0x7B, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTransform
// ******************************************************************
//8B4424048B542408538B1D
OOVPA_NO_XREF(D3DDevice_SetTransform, 1024, 11)

        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },
        { 0x04, 0x8B },
        { 0x05, 0x54 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },
        { 0x08, 0x53 },
        { 0x09, 0x8B },
        { 0x0A, 0x1D },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData2s
// ******************************************************************
//8D....00190400
OOVPA_NO_XREF(D3DDevice_SetVertexData2s, 1036, 7)

        { 0x00, 0x56 },
        { 0x01, 0x8B },

        { 0x20, 0x8D },
        //{ 0x21, 0x14 },
        //{ 0x22, 0x8D },
        { 0x23, 0x00 },
        { 0x24, 0x19 },
        { 0x25, 0x04 },
        { 0x26, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData4f
// ******************************************************************
//B918150000EB09 ...C21000
OOVPA_NO_XREF(D3DDevice_SetVertexData4f_16, 2036, 9)

        { 0x00, 0x56 },
        { 0x01, 0x8B },

        { 0x21, 0xB9 },
        { 0x22, 0x18 },
        { 0x23, 0x15 },
        { 0x24, 0x00 },
        { 0x25, 0x00 },
        { 0x26, 0xEB },
        { 0x27, 0x09 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData4s
// ******************************************************************
//801908000FBF
OOVPA_NO_XREF(D3DDevice_SetVertexData4s, 1036, 7)

        { 0x00, 0x56 },

        { 0x23, 0x80 },
        { 0x24, 0x19 },
        { 0x25, 0x08 },
        { 0x26, 0x00 },
        { 0x27, 0x0F },
        { 0x28, 0xBF },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData4ub
// ******************************************************************
//8D....4019040033
OOVPA_NO_XREF(D3DDevice_SetVertexData4ub, 1036, 7)

        { 0x00, 0x56 },
        { 0x20, 0x8D },

        { 0x23, 0x40 },
        { 0x24, 0x19 },
        { 0x25, 0x04 },
        { 0x26, 0x00 },
        { 0x27, 0x33 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Swap
// ******************************************************************
//7505BB050000008B
OOVPA_NO_XREF(D3DDevice_Swap, 1024, 9)

        { 0x01, 0x8B },

        { 0x28, 0x75 },
        { 0x29, 0x05 },
        { 0x2A, 0xBB },
        { 0x2B, 0x05 },
        { 0x2C, 0x00 },
        { 0x2D, 0x00 },
        { 0x2E, 0x00 },
        { 0x2F, 0x8B },
OOVPA_END;

//******************************************************************
//* D3DPalette_Lock2
//******************************************************************
//F6442408A0568B74
OOVPA_NO_XREF(D3DPalette_Lock2, 1024, 8)

        { 0x00, 0xF6 },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x08 },
        { 0x04, 0xA0 },
        { 0x05, 0x56 },
        { 0x06, 0x8B },
        { 0x07, 0x74 },
OOVPA_END;

// ******************************************************************
// * D3DTexture_LockRect
// ******************************************************************
//8B4424148B4C24088B542404568B
OOVPA_NO_XREF(D3DTexture_LockRect, 1024, 14)

        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x14 },
        { 0x04, 0x8B },
        { 0x05, 0x4C },
        { 0x06, 0x24 },
        { 0x07, 0x08 },
        { 0x08, 0x8B },
        { 0x09, 0x54 },
        { 0x0A, 0x24 },
        { 0x0B, 0x04 },
        { 0x0C, 0x56 },
        { 0x0D, 0x8B },
OOVPA_END;

// Verteks 11/03/2009
// blueshogun96 7/17/2010
// ******************************************************************
// * D3DVertexBuffer_Lock2
// ******************************************************************
//0CF6C3105675
OOVPA_NO_XREF(D3DVertexBuffer_Lock2, 1024, 8)

        { 0x01, 0x8A },
        { 0x04, 0x0C },
        { 0x05, 0xF6 },
        { 0x06, 0xC3 },
        { 0x07, 0x10 },
        { 0x08, 0x56 },
        { 0x09, 0x75 },
        { 0x11, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3D_BlockOnResource
// ******************************************************************
//F7C20000780075 ...C3
OOVPA_NO_XREF(D3D_BlockOnResource_0, 2048, 9)

        { 0x01, 0x8B },
        { 0x02, 0x35 },

        { 0x24, 0xF7 },
        { 0x25, 0xC2 },
        { 0x26, 0x00 },
        { 0x27, 0x00 },
        { 0x28, 0x78 },
        { 0x29, 0x00 },
        { 0x2A, 0x75 },
OOVPA_END;

// ******************************************************************
// * D3D_BlockOnTime
// ******************************************************************
//6A006A006A016A0656
OOVPA_XREF(D3D_BlockOnTime, 1036, 11,

    XREF_D3D_BlockOnTime,
    XRefZero)

        { 0x01, 0x8B },
        { 0x02, 0x5C },

        { 0x110, 0x6A },
        { 0x111, 0x00 },
        { 0x112, 0x6A },
        { 0x113, 0x00 },
        { 0x114, 0x6A },
        { 0x115, 0x01 },
        { 0x116, 0x6A },
        { 0x117, 0x06 },
        { 0x118, 0x56 },
OOVPA_END;

// ******************************************************************
// * D3D_CommonSetRenderTarget
// ******************************************************************
//00750C8B87B42100
#ifndef WIP_LessVertexPatching
OOVPA_XREF(D3D_CommonSetRenderTarget, 1024, 10,

    XREF_D3D_CommonSetRenderTarget,
    XRefZero)
#else
OOVPA_XREF(D3D_CommonSetRenderTarget, 1024, 1+10,

    XREF_D3D_CommonSetRenderTarget,
    XRefOne)

        XREF_ENTRY( 0x18, XREF_OFFSET_D3DDEVICE_M_RENDERTARGET ), // Derived
#endif

        { 0x00, 0x83 },
        { 0x01, 0xEC },

        { 0x13, 0x00 },
        { 0x14, 0x75 },
        { 0x15, 0x0C },
        { 0x16, 0x8B },
        { 0x17, 0x87 },
        { 0x18, 0xB4 },
        { 0x19, 0x21 },
        { 0x1A, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3D_KickOffAndWaitForIdle
// ******************************************************************
//8B4C24088B442404E8
OOVPA_NO_XREF(D3D_KickOffAndWaitForIdle2, 1024, 12)

        { 0x00, 0xA1 },

        { 0x10, 0x8B },
        { 0x11, 0x4C },
        { 0x12, 0x24 },
        { 0x13, 0x08 },
        { 0x14, 0x8B },
        { 0x15, 0x44 },
        { 0x16, 0x24 },
        { 0x17, 0x04 },
        { 0x18, 0xE8 },

        { 0x1D, 0xC2 },
        { 0x1E, 0x08 },
OOVPA_END;

//******************************************************************
//* D3DDevice_UpdateOverlay
//******************************************************************
//7707B800001000EB
OOVPA_NO_XREF(D3DDevice_UpdateOverlay, 1036, 9)

        { 0x02, 0x08 },

        { 0xCA, 0x77 },
        { 0xCB, 0x07 },
        { 0xCC, 0xB8 },
        { 0xCD, 0x00 },
        { 0xCE, 0x00 },
        { 0xCF, 0x10 },
        { 0xD0, 0x00 },
        { 0xD1, 0xEB },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderStateNotInline
// ******************************************************************
//C381FE880000007D1D8B0D ...C3
OOVPA_NO_XREF(D3DDevice_SetRenderStateNotInline_0, 2048, 13)

        { 0x00, 0x83 },
        { 0x01, 0xFE },

        { 0x1A, 0xC3 },
        { 0x1B, 0x81 },
        { 0x1C, 0xFE },
        { 0x1D, 0x88 },
        { 0x1E, 0x00 },
        { 0x1F, 0x00 },
        { 0x10, 0x00 },
        { 0x21, 0x7D },
        { 0x22, 0x1D },
        { 0x23, 0x8B },
        { 0x24, 0x0D },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderStateNotInline
// ******************************************************************
//578B7C24108BD7E8
OOVPA_NO_XREF(D3DDevice_SetRenderStateNotInline, 1024, 9)

        { 0x00, 0x56 },

        { 0x11, 0x57 },
        { 0x12, 0x8B },
        { 0x13, 0x7C },
        { 0x14, 0x24 },
        { 0x15, 0x10 },
        { 0x16, 0x8B },
        { 0x17, 0xD7 },
        { 0x18, 0xE8 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureStageStateNotInline
// ******************************************************************
//1BC981E1F1BFFFFF81C1
OOVPA_NO_XREF(D3DDevice_SetTextureStageStateNotInline, 1024, 11)

        { 0x00, 0x8B },

        { 0x46, 0x1B },
        { 0x47, 0xC9 },
        { 0x48, 0x81 },
        { 0x49, 0xE1 },
        { 0x4A, 0xF1 },
        { 0x4B, 0xBF },
        { 0x4C, 0xFF },
        { 0x4D, 0xFF },
        { 0x4E, 0x81 },
        { 0x4F, 0xC1 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureStageStateNotInline
// ******************************************************************
//1BC025F1BFFFFF050F48
OOVPA_NO_XREF(D3DDevice_SetTextureStageStateNotInline_0, 2024, 11)

        { 0x00, 0x83 },

        { 0x36, 0x1B },
        { 0x37, 0xC0 },
        { 0x38, 0x25 },
        { 0x39, 0xF1 },
        { 0x3A, 0xBF },
        { 0x3B, 0xFF },
        { 0x3C, 0xFF },
        { 0x3D, 0x05 },
        { 0x3E, 0x0F },
        { 0x3F, 0x48 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BeginPush
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginPush, 1024, 18)

        { 0x00, 0x56 },
        { 0x01, 0x8B },

        { 0x0F, 0x8B },
        { 0x10, 0x06 },
        { 0x11, 0x8B },
        { 0x12, 0x76 },
        { 0x13, 0x04 },
        { 0x14, 0x8B },
        { 0x15, 0x4C },
        { 0x16, 0x24 },
        { 0x17, 0x08 },
        { 0x18, 0x81 },
        { 0x19, 0xC6 },
        { 0x1A, 0x00 },
        { 0x1B, 0x02 },
        { 0x1C, 0x00 },
        { 0x1D, 0x00 },
        { 0x1E, 0x8D },
OOVPA_END;
