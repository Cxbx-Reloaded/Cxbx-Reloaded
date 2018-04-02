// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->D3D8LTCG.1.0.5659.inl
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
// * D3DDevice_SetPalette
// ******************************************************************
//85C05774258B4E2C8948088BBC
OOVPA_NO_XREF(D3DDevice_SetPalette, 1024, 15)

        { 0x00, 0x53 },
        { 0x01, 0x8B },

        { 0x13, 0x85 },
        { 0x14, 0xC0 },
        { 0x15, 0x57 },
        { 0x16, 0x74 },
        { 0x17, 0x25 },
        { 0x18, 0x8B },
        { 0x19, 0x4E },
        { 0x1A, 0x2C },
        { 0x1B, 0x89 },
        { 0x1C, 0x48 },
        { 0x1D, 0x08 },
        { 0x1E, 0x8B },
        { 0x1F, 0xBC },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPalette
// ******************************************************************
//85C05774258B4E2C8948088BBC ...C20400
OOVPA_NO_XREF(D3DDevice_SetPalette_4, 2024, 14)

        { 0x00, 0x53 },

        { 0x16, 0x85 },
        { 0x17, 0xC0 },
        { 0x18, 0x57 },
        { 0x19, 0x74 },
        { 0x1A, 0x25 },
        { 0x1B, 0x8B },
        { 0x1C, 0x4E },
        { 0x1D, 0x2C },
        { 0x1E, 0x89 },
        { 0x1F, 0x48 },
        { 0x20, 0x08 },
        { 0x21, 0x8B },
        { 0x22, 0xBC },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPixelShader
// ******************************************************************
//C740040000210083C008
OOVPA_NO_XREF(D3DDevice_SetPixelShader, 1024, 12)

        { 0x00, 0x51 },
        { 0x01, 0x53 },

        { 0x77, 0xC7 },
        { 0x78, 0x40 },
        { 0x79, 0x04 },
        { 0x7A, 0x00 },
        { 0x7B, 0x00 },
        { 0x7C, 0x21 },
        { 0x7D, 0x00 },
        { 0x7E, 0x83 },
        { 0x7F, 0xC0 },
        { 0x80, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateVertexShader
// ******************************************************************
//C740040000210083C008 ...C3
OOVPA_NO_XREF(D3DDevice_SetPixelShader_0, 2036, 12)

        { 0x00, 0x51 },
        { 0x01, 0x85 },

        { 0x73, 0xC7 },
        { 0x74, 0x40 },
        { 0x75, 0x04 },
        { 0x76, 0x00 },
        { 0x77, 0x00 },
        { 0x78, 0x21 },
        { 0x79, 0x00 },
        { 0x7A, 0x83 },
        { 0x7B, 0xC0 },
        { 0x7C, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPixelShaderConstant
// ******************************************************************
//840700008B49 ...C20400
OOVPA_NO_XREF(D3DDevice_SetPixelShaderConstant_4, 2024, 7)

        { 0x04, 0xEC },

        { 0x13, 0x84 },
        { 0x14, 0x07 },
        { 0x15, 0x00 },
        { 0x16, 0x00 },
        { 0x17, 0x8B },
        { 0x18, 0x49 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPixelShaderProgram
// ******************************************************************
//C780280900
OOVPA_NO_XREF(D3DDevice_SetPixelShaderProgram, 1024, 8)

        { 0x06, 0xA1 },
        { 0x0B, 0x74 },

        { 0x19, 0xC7 },
        { 0x1A, 0x80 },
        { 0x1B, 0x28 },
        { 0x1C, 0x09 },
        { 0x1D, 0x00 },
        { 0x1E, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_CullMode
// ******************************************************************
//C7000803040075
OOVPA_XREF(D3DDevice_SetRenderState_CullMode, 1053, 2+8,

    XREF_D3DDevice_SetRenderState_CullMode,
    XRefTwo)

        XREF_ENTRY( 0x03, XREF_D3DDEVICE ), // Derived
        XREF_ENTRY( 0x35, XREF_D3DRS_CULLMODE ), // Derived

        { 0x00, 0x56 },

        { 0x23, 0xC7 },
        { 0x24, 0x00 },
        { 0x25, 0x08 },
        { 0x26, 0x03 },
        { 0x27, 0x04 },
        { 0x28, 0x00 },
        { 0x29, 0x75 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_EdgeAntiAlias
// ******************************************************************
//0020030800894804
OOVPA_NO_XREF(D3DDevice_SetRenderState_EdgeAntiAlias, 1060, 10)

        { 0x01, 0x8B },
        { 0x1E, 0x4C },

        { 0x22, 0x00 },
        { 0x23, 0x20 },
        { 0x24, 0x03 },
        { 0x25, 0x08 },
        { 0x26, 0x00 },
        { 0x27, 0x89 },
        { 0x28, 0x48 },
        { 0x29, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_FillMode
// ******************************************************************
//C98B4C24087502
OOVPA_NO_XREF(D3DDevice_SetRenderState_FillMode, 1060, 9)

        { 0x00, 0x56 },
        { 0x08, 0x06 },

        { 0x2A, 0xC9 },
        { 0x2B, 0x8B },
        { 0x2C, 0x4C },
        { 0x2D, 0x24 },
        { 0x2E, 0x08 },
        { 0x2F, 0x75 },
        { 0x30, 0x02 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_FogColor
// ******************************************************************
//54240E8BF981
OOVPA_NO_XREF(D3DDevice_SetRenderState_FogColor, 1060, 7)

        { 0x00, 0x56 },

        { 0x24, 0x54 },
        { 0x25, 0x24 },
        { 0x26, 0x0E },
        { 0x27, 0x8B },
        { 0x28, 0xF9 },
        { 0x29, 0x81 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_FrontFace
// ******************************************************************
//00a0A0304008948
OOVPA_NO_XREF(D3DDevice_SetRenderState_FrontFace, 1060, 8)

        { 0x01, 0x8B },

        { 0x22, 0x00 },
        { 0x23, 0xA0 },
        { 0x24, 0x03 },
        { 0x25, 0x04 },
        { 0x26, 0x00 },
        { 0x27, 0x89 },
        { 0x28, 0x48 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_LogicOp
// ******************************************************************
//18C700BC1704
OOVPA_NO_XREF(D3DDevice_SetRenderState_LogicOp, 1060, 7)

        // XREF_ENTRY( 0x34, XREF_D3DRS_LOGICOP ),

        { 0x01, 0x8B },

        { 0x24, 0x18 },
        { 0x25, 0xC7 },
        { 0x26, 0x00 },
        { 0x27, 0xBC },
        { 0x28, 0x17 },
        { 0x29, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_NormalizeNormals
// ******************************************************************
//A4030400894804
OOVPA_NO_XREF(D3DDevice_SetRenderState_NormalizeNormals, 1060, 8)

        { 0x00, 0x56 },

        { 0x23, 0xA4 },
        { 0x24, 0x03 },
        { 0x25, 0x04 },
        { 0x26, 0x00 },
        { 0x27, 0x89 },
        { 0x28, 0x48 },
        { 0x29, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_ShadowFunc
// ******************************************************************
//6C1E04008D91
OOVPA_NO_XREF(D3DDevice_SetRenderState_ShadowFunc, 1060, 8)

        { 0x00, 0x56 },
        { 0x07, 0x8B },

        { 0x23, 0x6C },
        { 0x24, 0x1E },
        { 0x25, 0x04 },
        { 0x26, 0x00 },
        { 0x27, 0x8D },
        { 0x28, 0x91 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_StencilEnable
// ******************************************************************
//018b54240885d2c700
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilEnable, 1060, 10)

        { 0x02, 0x35 },

        { 0x50, 0x01 },
        { 0x51, 0x8B },
        { 0x52, 0x54 },
        { 0x53, 0x24 },
        { 0x54, 0x08 },
        { 0x55, 0x85 },
        { 0x56, 0xD2 },
        { 0x57, 0xC7 },
        { 0x58, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_StencilFail
// ******************************************************************
//7003040089
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilFail, 1060, 7)

        { 0x01, 0x8B },
        { 0x23, 0x33 },

        { 0x61, 0x70 },
        { 0x62, 0x03 },
        { 0x63, 0x04 },
        { 0x64, 0x00 },
        { 0x65, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_ZEnable
// ******************************************************************
//C9C7000C0304008948048B
OOVPA_NO_XREF(D3DDevice_SetRenderState_ZEnable, 1060, 13)

        { 0x00, 0x56 },
        { 0x01, 0x8B },

        { 0x37, 0xC9 },
        { 0x38, 0xC7 },
        { 0x39, 0x00 },
        { 0x3A, 0x0C },
        { 0x3B, 0x03 },
        { 0x3C, 0x04 },
        { 0x3D, 0x00 },
        { 0x3E, 0x89 },
        { 0x3F, 0x48 },
        { 0x40, 0x04 },
        { 0x41, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_BorderColor
// ******************************************************************
//088BD1C1E20681C2241B
OOVPA_NO_XREF(D3DDevice_SetTextureState_BorderColor, 1048, 11)

        { 0x00, 0x56 },

        { 0x20, 0x08 },
        { 0x21, 0x8B },
        { 0x22, 0xD1 },
        { 0x23, 0xC1 },
        { 0x24, 0xE2 },
        { 0x25, 0x06 },
        { 0x26, 0x81 },
        { 0x27, 0xC2 },
        { 0x28, 0x24 },
        { 0x29, 0x1B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_BorderColor
// ******************************************************************
//891089580483 ...C3
OOVPA_NO_XREF(D3DDevice_SetTextureState_BorderColor_0, 2048, 7)

        { 0x01, 0x57 },

        { 0x2D, 0x89 },
        { 0x2E, 0x10 },
        { 0x2F, 0x89 },
        { 0x30, 0x58 },
        { 0x31, 0x04 },
        { 0x32, 0x83 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_ColorKeyColor
// ******************************************************************
//E00A040089
OOVPA_NO_XREF(D3DDevice_SetTextureState_ColorKeyColor, 1024, 8)

        { 0x01, 0x8B },
        { 0x18, 0xE8 },
        { 0x1E, 0x4C },

        { 0x24, 0xE0 },
        { 0x25, 0x0A },
        { 0x26, 0x04 },
        { 0x27, 0x00 },
        { 0x28, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_ColorKeyColor
// ******************************************************************
//E00A040089 ...C3
OOVPA_NO_XREF(D3DDevice_SetTextureState_ColorKeyColor_0, 2048, 8)

        { 0x01, 0x57 },
        { 0x0B, 0x8B },
        { 0x1D, 0xE8 },

        { 0x25, 0xE0 },
        { 0x26, 0x0A },
        { 0x27, 0x04 },
        { 0x28, 0x00 },
        { 0x29, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderTargetFast
// ******************************************************************
//A900007800578B
OOVPA_NO_XREF(D3DDevice_SetRenderTargetFast, 1024, 9)

        { 0x00, 0x53 },
        { 0x01, 0x8B },

        { 0x07, 0xA9 },
        { 0x08, 0x00 },
        { 0x09, 0x00 },
        { 0x0A, 0x78 },
        { 0x0B, 0x00 },
        { 0x0C, 0x57 },
        { 0x0D, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3D_BlockOnTime
// ******************************************************************
//C747101001040089
OOVPA_XREF(D3D_BlockOnTime, 1024, 10,

    XREF_D3D_BlockOnTime,
    XRefZero)

        { 0x00, 0x56 },
        { 0x01, 0x8B },

        { 0xB4, 0xC7 },
        { 0xB5, 0x47 },
        { 0xB6, 0x10 },
        { 0xB7, 0x10 },
        { 0xB8, 0x01 },
        { 0xB9, 0x04 },
        { 0xBA, 0x00 },
        { 0xBB, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3D_BlockOnTime
// ******************************************************************
//C747101001040089 ...C20400
OOVPA_XREF(D3D_BlockOnTime_4, 2048, 9,

    XREF_D3D_BlockOnTime,
    XRefZero)

        { 0x00, 0x56 },

        { 0xB8, 0xC7 },
        { 0xB9, 0x47 },
        { 0xBA, 0x10 },
        { 0xBB, 0x10 },
        { 0xBC, 0x01 },
        { 0xBD, 0x04 },
        { 0xBE, 0x00 },
        { 0xBF, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3D_MakeRequestedSpace
// ******************************************************************
//81C5004000003BE9 ...C20400
OOVPA_NO_XREF(D3D_MakeRequestedSpace, 2048, 10)

        { 0x00, 0x83 },
        { 0x01, 0xEC },

        { 0x5A, 0x81 },
        { 0x5B, 0xC5 },
        { 0x5C, 0x00 },
        { 0x5D, 0x40 },
        { 0x5E, 0x00 },
        { 0x5F, 0x00 },
        { 0x60, 0x3B },
        { 0x61, 0xE9 },
OOVPA_END;

// ******************************************************************
// * D3D_SetFence
// ******************************************************************
//C70010A304008BCF
OOVPA_XREF(D3D_SetFence, 1048, 10,

    XREF_D3D_SetFence,
    XRefZero)

        { 0x00, 0x56 },
        { 0x01, 0x8B },

        { 0x21, 0xC7 },
        { 0x22, 0x00 },
        { 0x23, 0x10 },
        { 0x24, 0xA3 },
        { 0x25, 0x04 },
        { 0x26, 0x00 },
        { 0x27, 0x8B },
        { 0x28, 0xCF },
OOVPA_END;

// ******************************************************************
// * D3D_SetFence
// ******************************************************************
//C70010A304008B
OOVPA_XREF(D3D_SetFence, 1060, 9,

    XREF_D3D_SetFence,
    XRefZero)

        { 0x00, 0x53 },
        { 0x01, 0x8A },

        { 0x25, 0xC7 },
        { 0x26, 0x00 },
        { 0x27, 0x10 },
        { 0x28, 0xA3 },
        { 0x29, 0x04 },
        { 0x2A, 0x00 },
        { 0x2B, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawIndexedVerticesUP
// ******************************************************************
//558BEC83EC1453568B35 ...C700FC17040089
OOVPA_NO_XREF(D3DDevice_DrawIndexedVerticesUP, 1060, 17)

        { 0x00, 0x55 },
        { 0x01, 0x8B },
        { 0x02, 0xEC },
        { 0x03, 0x83 },
        { 0x04, 0xEC },
        { 0x05, 0x14 },
        { 0x06, 0x53 },
        { 0x07, 0x56 },
        { 0x08, 0x8B },
        { 0x09, 0x35 },

        { 0x30, 0xC7 },
        { 0x31, 0x00 },
        { 0x32, 0xFC },
        { 0x33, 0x17 },
        { 0x34, 0x04 },
        { 0x35, 0x00 },
        { 0x36, 0x89 },
OOVPA_END;
