// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->D3D8LTCG.1.0.3911.inl
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
//85C9750AC705 ...C21000
OOVPA_NO_XREF(Direct3D_CreateDevice_16, 2024, 9)

        { 0x00, 0x8B },

        { 0x06, 0x85 },
        { 0x07, 0xC9 },
        { 0x08, 0x75 },
        { 0x09, 0x0A },
        { 0x0A, 0xC7 },
        { 0x0B, 0x05 },

        { 0x28, 0x56 },
        { 0x29, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_CullMode
// ******************************************************************
//C7000803040075
OOVPA_XREF(D3DDevice_SetRenderState_CullMode, 1045, 2+8,

    XREF_D3DDevice_SetRenderState_CullMode,
    XRefTwo)

        XREF_ENTRY( 0x03, XREF_D3DDEVICE ), // Derived
        XREF_ENTRY( 0x2D, XREF_D3DRS_CULLMODE ), // Derived

        { 0x00, 0x56 },

        { 0x1B, 0xC7 },
        { 0x1C, 0x00 },
        { 0x1D, 0x08 },
        { 0x1E, 0x03 },
        { 0x1F, 0x04 },
        { 0x20, 0x00 },
        { 0x21, 0x75 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_BumpEnv
// ******************************************************************
//000085C08BDF75 ...C20800
OOVPA_NO_XREF(D3DDevice_SetTextureState_BumpEnv_8, 2024, 8)

        { 0x00, 0x53 },

        { 0x14, 0x00 },
        { 0x15, 0x00 },
        { 0x16, 0x85 },
        { 0x17, 0xC0 },
        { 0x18, 0x8B },
        { 0x19, 0xDF },
        { 0x1A, 0x75 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_ZEnable
// ******************************************************************
//C9C7000C03040089480483
OOVPA_NO_XREF(D3DDevice_SetRenderState_ZEnable, 1024, 13)

        { 0x00, 0x56 },
        { 0x01, 0x8B },

        { 0x2F, 0xC9 },
        { 0x30, 0xC7 },
        { 0x31, 0x00 },
        { 0x32, 0x0C },
        { 0x33, 0x03 },
        { 0x34, 0x04 },
        { 0x35, 0x00 },
        { 0x36, 0x89 },
        { 0x37, 0x48 },
        { 0x38, 0x04 },
        { 0x39, 0x83 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_TextureFactor
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_TextureFactor, 1036, 1+10,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x03, XREF_D3DDEVICE ), // Derived

        { 0x00, 0x56 },
        { 0x07, 0x8B },
        { 0x08, 0x86 },
        //{ 0x09, 0x84 },
        //{ 0x0A, 0x07 },
        { 0x0B, 0x00 },
        { 0x0C, 0x00 },
        { 0x0D, 0x85 },
        { 0x0E, 0xC0 },
        { 0x0F, 0x75 },

        { 0x11, 0x8B },
        { 0x12, 0x06 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_YuvEnable
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_YuvEnable, 1024, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0C, XREF_D3DRS_YUVENABLE ),

        { 0x01, 0x44 },
        { 0x04, 0x56 },
        { 0x05, 0x8B },
        { 0x06, 0x35 },
        { 0x0B, 0xA3 },
        { 0x11, 0x06 },
        { 0x15, 0x72 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_OcclusionCullEnable
// ******************************************************************
//568BD0E8
OOVPA_XREF(D3DDevice_SetRenderState_OcclusionCullEnable, 1024, 1+2,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0C, XREF_D3DRS_OCCLUSIONCULLENABLE ),

        { 0x00, 0x8B },
        { 0x06, 0x35 },

        //{ 0x26, 0x56 },
        //{ 0x27, 0x8B },
        //{ 0x28, 0xD0 },
        //{ 0x29, 0xE8 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_TexCoordIndex
// ******************************************************************
//538BD9C1E107 ...C3
OOVPA_XREF(D3DDevice_SetTextureState_TexCoordIndex_0, 2039, 1+10,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x08, XREF_D3DTSS_TEXCOORDINDEX ), // Derived

        { 0x00, 0x53 },
        { 0x01, 0x8B },
        { 0x02, 0xD9 },
        { 0x03, 0xC1 },
        { 0x04, 0xE1 },
        { 0x05, 0x07 },

        { 0x13, 0x25 },
        { 0x14, 0x00 },
        { 0x15, 0x00 },
        { 0x16, 0xFF },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_BorderColor
// ******************************************************************
//C1..0681..241B040089 ...C3
OOVPA_NO_XREF(D3DDevice_SetTextureState_BorderColor_0, 2024, 12)

        { 0x00, 0x56 },
        { 0x01, 0x8B },

        { 0x1C, 0xC1 },
        { 0x1D, 0xE1 },
        { 0x1E, 0x06 },
        { 0x1F, 0x81 },
        { 0x20, 0xC1 },
        { 0x21, 0x24 },
        { 0x22, 0x1B },
        { 0x23, 0x04 },
        { 0x24, 0x00 },
        { 0x25, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_ColorKeyColor
// ******************************************************************
//83C008C1E707890689 ...C3
OOVPA_NO_XREF(D3DDevice_SetTextureState_ColorKeyColor_0, 2024, 10)

        { 0x01, 0x8B },

        { 0x26, 0x83 },
        { 0x27, 0xC0 },
        { 0x28, 0x08 },
        { 0x29, 0xC1 },
        { 0x2A, 0xE7 },
        { 0x2B, 0x07 },
        { 0x2C, 0x89 },
        { 0x2D, 0x06 },
        { 0x2E, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_ZBias
// ******************************************************************
//24..8BF07D06D805
OOVPA_NO_XREF(D3DDevice_SetRenderState_ZBias, 1048, 9)

        // XREF_ENTRY( 0x6A, XREF_D3DRS_ZBIAS ),

        { 0x00, 0x51 },
        { 0x01, 0x53 },

        { 0x17, 0x24 },
        //{ 0x18, 0x10 },
        { 0x19, 0x8B },
        { 0x1A, 0xF0 },
        { 0x1B, 0x7D },
        { 0x1C, 0x06 },
        { 0x1D, 0xD8 },
        { 0x1E, 0x05 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_LineWidth
// ******************************************************************
//44240C57D88E
OOVPA_NO_XREF(D3DDevice_SetRenderState_LineWidth, 1024, 7)


      // XREF_ENTRY( 0x5B, XREF_D3DRS_LINEWIDTH ),

        { 0x02, 0x5C },

        { 0x11, 0x44 },
        { 0x12, 0x24 },
        { 0x13, 0x0C },
        { 0x14, 0x57 },
        { 0x15, 0xD8 },
        { 0x16, 0x8E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_Dxt1NoiseEnable
// ******************************************************************
//0FB6480D8A91
OOVPA_NO_XREF(D3DDevice_SetRenderState_Dxt1NoiseEnable, 1036, 7)

        // XREF_ENTRY( 0x6F, XREF_D3DRS_DXT1NOISEENABLE ),

        { 0x00, 0x53 },

        { 0x12, 0x0F },
        { 0x13, 0xB6 },
        { 0x14, 0x48 },
        { 0x15, 0x0D },
        { 0x16, 0x8A },
        { 0x17, 0x91 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_StencilCullEnable
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_StencilCullEnable, 1024, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0C, XREF_D3DRS_STENCILCULLENABLE ),

        { 0x01, 0x44 },
        { 0x04, 0x56 },
        { 0x05, 0x8B },
        { 0x06, 0x35 },
        { 0x0B, 0xA3 },
        { 0x11, 0x06 },
        { 0x15, 0x72 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_RopZCmpAlwaysRead
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_RopZCmpAlwaysRead, 1024, 1+8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x05, XREF_D3DRS_ROPZCMPALWAYSREAD ),

        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        { 0x09, 0xE8 },

        { 0x0E, 0xC2 },
        { 0x0F, 0x04 },
        { 0x10, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_RopZRead
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_RopZRead, 1024, 1+8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x05, XREF_D3DRS_ROPZREAD ),

        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        { 0x09, 0xE8 },

        { 0x0E, 0xC2 },
        { 0x0F, 0x04 },
        { 0x10, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_DoNotCullUncompressed
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_DoNotCullUncompressed, 1024, 1+8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x05, XREF_D3DRS_DONOTCULLUNCOMPRESSED ),

        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        { 0x09, 0xE8 },

        { 0x0E, 0xC2 },
        { 0x0F, 0x04 },
        { 0x10, 0x00 },
OOVPA_END;

//******************************************************************
//* D3DDevice_SetRenderState_BackFillMode
//******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_BackFillMode, 1024, 2+5,

    XRefNoSaveIndex,
    XRefTwo)

        XREF_ENTRY( 0x06, XREF_D3DRS_FILLMODE ),
        XREF_ENTRY( 0x0B, XREF_D3DRS_BACKFILLMODE ),

        { 0x01, 0x44 },
        { 0x0F, 0x89 },
        { 0x10, 0x4C },
        { 0x11, 0x24 },
        { 0x12, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_EdgeAntiAlias
// ******************************************************************
//0020030800894804
OOVPA_NO_XREF(D3DDevice_SetRenderState_EdgeAntiAlias, 1024, 10)

        { 0x01, 0x8B },
        { 0x12, 0x4C },

        { 0x1A, 0x00 },
        { 0x1B, 0x20 },
        { 0x1C, 0x03 },
        { 0x1D, 0x08 },
        { 0x1E, 0x00 },
        { 0x1F, 0x89 },
        { 0x20, 0x48 },
        { 0x21, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_FillMode
// ******************************************************************
//C98B4C24087502
OOVPA_NO_XREF(D3DDevice_SetRenderState_FillMode, 1024, 9)

        { 0x00, 0x56 },
        { 0x08, 0x06 },

        { 0x22, 0xC9 },
        { 0x23, 0x8B },
        { 0x24, 0x4C },
        { 0x25, 0x24 },
        { 0x26, 0x08 },
        { 0x27, 0x75 },
        { 0x28, 0x02 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_FogColor
// ******************************************************************
//54240E8BF981
OOVPA_NO_XREF(D3DDevice_SetRenderState_FogColor, 1024, 7)

        { 0x00, 0x56 },

        { 0x1C, 0x54 },
        { 0x1D, 0x24 },
        { 0x1E, 0x0E },
        { 0x1F, 0x8B },
        { 0x20, 0xF9 },
        { 0x21, 0x81 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleAntiAlias
// ******************************************************************
//C1E11083E20F
OOVPA_XREF(D3DDevice_SetRenderState_MultiSampleAntiAlias, 1024, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0C, XREF_D3DRS_MULTISAMPLEANTIALIAS ),

        { 0x00, 0x8B },

        { 0x36, 0xC1 },
        { 0x37, 0xE1 },
        { 0x38, 0x10 },
        { 0x39, 0x83 },
        { 0x3A, 0xE2 },
        { 0x3B, 0x0F },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMask
// ******************************************************************
//83E10FC1E210
OOVPA_XREF(D3DDevice_SetRenderState_MultiSampleMask, 1024, 1+8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0C, XREF_D3DRS_MULTISAMPLEMASK ),

        { 0x00, 0x8B },
        { 0x01, 0x44 },

        { 0x36, 0x83 },
        { 0x37, 0xE1 },
        { 0x38, 0x0F },
        { 0x39, 0xC1 },
        { 0x3A, 0xE2 },
        { 0x3B, 0x10 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_LogicOp
// ******************************************************************
//18C700BC1704
OOVPA_NO_XREF(D3DDevice_SetRenderState_LogicOp, 1024, 7)

        // XREF_ENTRY( 0x2D, XREF_D3DRS_LOGICOP ),

        { 0x01, 0x8B },

        { 0x1C, 0x18 },
        { 0x1D, 0xC7 },
        { 0x1E, 0x00 },
        { 0x1F, 0xBC },
        { 0x20, 0x17 },
        { 0x21, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_NormalizeNormals
// ******************************************************************
//A4030400894804
OOVPA_NO_XREF(D3DDevice_SetRenderState_NormalizeNormals, 1024, 8)

        { 0x00, 0x56 },

        { 0x1B, 0xA4 },
        { 0x1C, 0x03 },
        { 0x1D, 0x04 },
        { 0x1E, 0x00 },
        { 0x1F, 0x89 },
        { 0x20, 0x48 },
        { 0x21, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_StencilFail
// ******************************************************************
//7003040089
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilFail, 1024, 7)

        { 0x01, 0x8B },
        { 0x1B, 0x33 },

        { 0x59, 0x70 },
        { 0x5A, 0x03 },
        { 0x5B, 0x04 },
        { 0x5C, 0x00 },
        { 0x5D, 0x89 },
OOVPA_END;

//******************************************************************
//* D3DDevice_SetRenderState_TwoSidedLighting
//******************************************************************
//834808208B
OOVPA_XREF(D3DDevice_SetRenderState_TwoSidedLighting, 1024, 1+6,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_D3DRS_TWOSIDEDLIGHTING ),

        { 0x00, 0xA1 },

        { 0x05, 0x83 },
        { 0x06, 0x48 },
        { 0x07, 0x08 },
        { 0x08, 0x20 },
        { 0x09, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_FrontFace
// ******************************************************************
//00A00304008948
OOVPA_NO_XREF(D3DDevice_SetRenderState_FrontFace, 1024, 8)

        { 0x01, 0x8B },

        { 0x1A, 0x00 },
        { 0x1B, 0xA0 },
        { 0x1C, 0x03 },
        { 0x1D, 0x04 },
        { 0x1E, 0x00 },
        { 0x1F, 0x89 },
        { 0x20, 0x48 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_ShadowFunc
// ******************************************************************
//6C1E04008D91
OOVPA_NO_XREF(D3DDevice_SetRenderState_ShadowFunc, 1024, 8)

        { 0x00, 0x56 },
        { 0x07, 0x8B },

        { 0x1B, 0x6C },
        { 0x1C, 0x1E },
        { 0x1D, 0x04 },
        { 0x1E, 0x00 },
        { 0x1F, 0x8D },
        { 0x20, 0x91 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_StencilEnable
// ******************************************************************
//018b54240885d2c700
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilEnable, 1024, 10)

        { 0x02, 0x35 },

        { 0x48, 0x01 },
        { 0x49, 0x8B },
        { 0x4A, 0x54 },
        { 0x4B, 0x24 },
        { 0x4C, 0x08 },
        { 0x4D, 0x85 },
        { 0x4E, 0xD2 },
        { 0x4F, 0xC7 },
        { 0x50, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateVertexShader
// ******************************************************************
//241833f685ed
OOVPA_NO_XREF(D3DDevice_CreateVertexShader, 1024, 8)

        { 0x04, 0x6C },
        { 0x0C, 0x0F },
        { 0x1A, 0x24 },
        { 0x1B, 0x18 },
        { 0x1C, 0x33 },
        { 0x1D, 0xF6 },
        { 0x1E, 0x85 },
        { 0x1F, 0xED },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetShaderConstantMode
// ******************************************************************
//A810538B1D ...C3
OOVPA_NO_XREF(D3DDevice_SetShaderConstantMode_0, 2048, 11)

        { 0x00, 0xA8 },
        { 0x01, 0x10 },
        { 0x02, 0x53 },
        { 0x03, 0x8B },
        { 0x04, 0x1D },

        { 0x10, 0x00 },
        { 0x11, 0x02 },
        { 0x12, 0x00 },
        { 0x13, 0x00 },
        { 0x14, 0xEB },
        { 0x15, 0x06 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Clear
// ******************************************************************
//FFFDFFFF8944
OOVPA_NO_XREF(D3DDevice_Clear, 1036, 8)

        { 0x00, 0x83 },
        { 0x01, 0xEC },

        { 0x42, 0xFF },
        { 0x43, 0xFD },
        { 0x44, 0xFF },
        { 0x45, 0xFF },
        { 0x46, 0x89 },
        { 0x47, 0x44 },
OOVPA_END;

// ******************************************************************
// * D3DResource_Release
// ******************************************************************
//578B7C24088B078BC881E1FFFF0000
OOVPA_NO_XREF(D3DResource_Release, 1036, 15)

        { 0x00, 0x57 },
        { 0x01, 0x8B },
        { 0x02, 0x7C },
        { 0x03, 0x24 },
        { 0x04, 0x08 },
        { 0x05, 0x8B },
        { 0x06, 0x07 },
        { 0x07, 0x8B },
        { 0x08, 0xC8 },
        { 0x09, 0x81 },
        { 0x0A, 0xE1 },
        { 0x0B, 0xFF },
        { 0x0C, 0xFF },
        { 0x0D, 0x00 },
        { 0x0E, 0x00 },
OOVPA_END;

//******************************************************************
//* D3DDevice_SetTile
//******************************************************************
//C744242000000000C744241C ...C3
OOVPA_NO_XREF(D3DDevice_SetTile_0, 2024, 14)

        { 0x00, 0x8B },
        { 0x01, 0x15 },

        { 0x2C, 0xC7 },
        { 0x2D, 0x44 },
        { 0x2E, 0x24 },
        { 0x2F, 0x20 },
        { 0x30, 0x00 },
        { 0x31, 0x00 },
        { 0x32, 0x00 },
        { 0x33, 0x00 },
        { 0x34, 0xC7 },
        { 0x35, 0x44 },
        { 0x36, 0x24 },
        { 0x37, 0x1C },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Present
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Present, 1024, 17)

        { 0x00, 0x83 },
        { 0x01, 0xEC },
        { 0x02, 0x08 },
        { 0x03, 0x56 },
        { 0x04, 0x8B },
        { 0x05, 0x35 },

        { 0x0B, 0x6A },
        { 0x0C, 0x02 },
        { 0x0D, 0xE8 },
        { 0x12, 0x8B },
        { 0x13, 0x86 },
        { 0x16, 0x00 },
        { 0x17, 0x00 },
        { 0x18, 0x8B },
        { 0x19, 0x8E },
        { 0x1C, 0x00 },
        { 0x1D, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawIndexedVerticesUP
// ******************************************************************
//C700FC1704008B57
OOVPA_NO_XREF(D3DDevice_DrawIndexedVerticesUP, 1024, 10)

        { 0x01, 0x8B },
        { 0x05, 0x14 },

        { 0x2F, 0xC7 },
        { 0x30, 0x00 },
        { 0x31, 0xFC },
        { 0x32, 0x17 },
        { 0x33, 0x04 },
        { 0x34, 0x00 },
        { 0x35, 0x8B },
        { 0x36, 0x57 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShader
// ******************************************************************
//F6C30155568B35 ...C3
OOVPA_NO_XREF(D3DDevice_SetVertexShader_0, 2024, 14)

        { 0x00, 0xF6 },
        { 0x01, 0xC3 },
        { 0x02, 0x01 },
        { 0x03, 0x55 },
        { 0x04, 0x56 },
        { 0x05, 0x8B },
        { 0x06, 0x35 },

        { 0x15, 0x74 },
        { 0x16, 0x05 },
        { 0x17, 0x8D },
        { 0x18, 0x7B },
        { 0x19, 0xFF },
        { 0x1A, 0xEB },
        { 0x1B, 0x0E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetScissors
// ******************************************************************
//75448B..000B00008B..080B00008B..040B0000
OOVPA_NO_XREF(D3DDevice_SetScissors, 1024, 19)

        { 0x00, 0x83 },
        { 0x01, 0xEC },

        { 0x13, 0x75 },
        { 0x14, 0x44 },
        { 0x15, 0x8B },
        { 0x17, 0x00 },
        { 0x18, 0x0B },
        { 0x19, 0x00 },
        { 0x1A, 0x00 },
        { 0x1B, 0x8B },
        { 0x1D, 0x08 },
        { 0x1E, 0x0B },
        { 0x1F, 0x00 },
        { 0x20, 0x00 },
        { 0x21, 0x8B },
        { 0x23, 0x04 },
        { 0x24, 0x0B },
        { 0x25, 0x00 },
        { 0x26, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTransform
// ******************************************************************
//8D7822C1E70603FBB910000000 ...C3
OOVPA_NO_XREF(D3DDevice_SetTransform_0, 2024, 15)

        { 0x00, 0x53 },
        { 0x01, 0x8B },

        { 0x09, 0x8D },
        { 0x0A, 0x78 },
        { 0x0B, 0x22 },
        { 0x0C, 0xC1 },
        { 0x0D, 0xE7 },
        { 0x0E, 0x06 },
        { 0x0F, 0x03 },
        { 0x10, 0xFB },
        { 0x11, 0xB9 },
        { 0x12, 0x10 },
        { 0x13, 0x00 },
        { 0x14, 0x00 },
        { 0x15, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShaderConstant
// ******************************************************************
//83C160C1E202A810 ...C3
OOVPA_NO_XREF(D3DDevice_SetVertexShaderConstant_8, 2024, 10)

        { 0x00, 0x55 },
        { 0x01, 0x8B },

        { 0x13, 0x83 },
        { 0x14, 0xC1 },
        { 0x15, 0x60 },
        { 0x16, 0xC1 },
        { 0x17, 0xE2 },
        { 0x18, 0x02 },
        { 0x19, 0xA8 },
        { 0x1A, 0x10 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawVerticesUP
// ******************************************************************
//894804C700FC1704008B ...C20C00
OOVPA_NO_XREF(D3DDevice_DrawVerticesUP_12, 2024, 12)

        { 0x00, 0x55 },
        { 0x01, 0x8B },

        { 0x2B, 0x89 },
        { 0x2C, 0x48 },
        { 0x2D, 0x04 },
        { 0x2E, 0xC7 },
        { 0x2F, 0x00 },
        { 0x30, 0xFC },
        { 0x31, 0x17 },
        { 0x32, 0x04 },
        { 0x33, 0x00 },
        { 0x34, 0x8B },
OOVPA_END;

//******************************************************************
//* D3DDevice_LoadVertexShader
//******************************************************************
//C7009C1E04008B ...C3
OOVPA_NO_XREF(D3DDevice_LoadVertexShader_0, 2024, 9)

        { 0x00, 0x53 },
        { 0x01, 0x8B },

        { 0x3F, 0xC7 },
        { 0x40, 0x00 },
        { 0x41, 0x9C },
        { 0x42, 0x1E },
        { 0x43, 0x04 },
        { 0x44, 0x00 },
        { 0x45, 0x8B },
OOVPA_END;

//******************************************************************
//* D3DDevice_SelectVertexShader
//******************************************************************
//C740040600000083 ...C20400
OOVPA_NO_XREF(D3DDevice_SelectVertexShader_4, 2024, 10)

        { 0x00, 0x85 },
        { 0x03, 0x8B },

        { 0x46, 0xC7 },
        { 0x47, 0x40 },
        { 0x48, 0x04 },
        { 0x49, 0x06 },
        { 0x4A, 0x00 },
        { 0x4B, 0x00 },
        { 0x4C, 0x00 },
        { 0x4D, 0x83 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderTarget
// ******************************************************************
//0CC1E91483E10FBD01000000
OOVPA_NO_XREF(D3DDevice_SetRenderTarget, 1024, 14)

        { 0x00, 0x83 },
        { 0x01, 0xEC },

        { 0x24, 0x0C },
        { 0x25, 0xC1 },
        { 0x26, 0xE9 },
        { 0x27, 0x14 },
        { 0x28, 0x83 },
        { 0x29, 0xE1 },
        { 0x2A, 0x0F },
        { 0x2B, 0xBD },
        { 0x2C, 0x01 },
        { 0x2D, 0x00 },
        { 0x2E, 0x00 },
        { 0x2F, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetViewport
// ******************************************************************
//EB06894424088BF8
OOVPA_NO_XREF(D3DDevice_SetViewport, 1024, 10)

        { 0x00, 0x83 },
        { 0x01, 0xEC },

        { 0x25, 0xEB },
        { 0x26, 0x06 },
        { 0x27, 0x89 },
        { 0x28, 0x44 },
        { 0x29, 0x24 },
        { 0x2A, 0x08 },
        { 0x2B, 0x8B },
        { 0x2C, 0xF8 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTexture
// ******************************************************************
//81C10000F8FF89 ...C20400
OOVPA_NO_XREF(D3DDevice_SetTexture_4, 2024, 9)

        { 0x00, 0x83 },
        { 0x01, 0xEC },

        { 0x27, 0x81 },
        { 0x28, 0xC1 },
        { 0x29, 0x00 },
        { 0x2A, 0x00 },
        { 0x2B, 0xF8 },
        { 0x2C, 0xFF },
        { 0x2D, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3D_BlockOnResource
// ******************************************************************
//00007800750C85 ...C3
OOVPA_NO_XREF(D3D_BlockOnResource_0, 2024, 11)

        { 0x00, 0x8B },
        { 0x01, 0x15 },

        { 0x28, 0xF7 },
        { 0x29, 0xC1 },
        { 0x2A, 0x00 },
        { 0x2B, 0x00 },
        { 0x2C, 0x78 },
        { 0x2D, 0x00 },
        { 0x2E, 0x75 },
        { 0x2F, 0x0C },
        { 0x30, 0x85 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetStreamSource
// ******************************************************************
//7406810300000800
OOVPA_XREF(D3DDevice_SetStreamSource, 1039, 1+11,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x47, XREF_G_STREAM ), // Derived

        { 0x00, 0x53 },
        { 0x01, 0x8B },
        { 0x02, 0x1D },

        { 0x38, 0x81 },
        { 0x39, 0x45 },
        { 0x3A, 0x00 },
        { 0x3B, 0x00 },
        { 0x3C, 0x00 },
        { 0x3D, 0x08 },
        { 0x3E, 0x00 },
        { 0x3F, 0x8D },
OOVPA_END;
