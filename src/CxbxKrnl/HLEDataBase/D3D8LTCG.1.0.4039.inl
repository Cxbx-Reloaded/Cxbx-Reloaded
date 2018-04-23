// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->D3D8LTCG.1.0.4039.inl
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
// * D3DDevice_DrawIndexedVertices
// ******************************************************************
//558BEC83EC0853568B35
OOVPA_NO_XREF(D3DDevice_DrawIndexedVertices, 1024, 10)

        { 0x00, 0x55 },
        { 0x01, 0x8B },
        { 0x02, 0xEC },
        { 0x03, 0x83 },
        { 0x04, 0xEC },
        { 0x05, 0x08 },
        { 0x06, 0x53 },
        { 0x07, 0x56 },
        { 0x08, 0x8B },
        { 0x09, 0x35 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetViewport
// ******************************************************************
//EB06894424088BF8
OOVPA_NO_XREF(D3DDevice_SetViewport, 1036, 10)

        { 0x00, 0x83 },
        { 0x01, 0xEC },

        { 0x23, 0xEB },
        { 0x24, 0x06 },
        { 0x25, 0x89 },
        { 0x26, 0x44 },
        { 0x27, 0x24 },
        { 0x28, 0x08 },
        { 0x29, 0x8B },
        { 0x2A, 0xF8 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetLight
// ******************************************************************
//83E0F0894424108D04C0
OOVPA_NO_XREF(D3DDevice_SetLight, 1048, 12)

        { 0x00, 0x83 },
        { 0x01, 0xEC },

        { 0x26, 0x83 },
        { 0x27, 0xE0 },
        { 0x28, 0xF0 },
        { 0x29, 0x89 },
        { 0x2A, 0x44 },
        { 0x2B, 0x24 },
        { 0x2C, 0x10 },
        { 0x2D, 0x8D },
        { 0x2E, 0x04 },
        { 0x2F, 0xC0 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_TexCoordIndex
// ******************************************************************
//51538B5C240C8BC6 ...C20400
OOVPA_XREF(D3DDevice_SetTextureState_TexCoordIndex_4, 2040, 1+10,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x14, XREF_D3DTSS_TEXCOORDINDEX ), // Derived

        { 0x00, 0x51 },
        { 0x01, 0x53 },
        { 0x02, 0x8B },
        { 0x03, 0x5C },
        { 0x04, 0x24 },
        { 0x05, 0x0C },
        { 0x06, 0x8B },
        { 0x07, 0xC6 },

        { 0x12, 0x89 },
        { 0x18, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_BorderColor
// ******************************************************************
//8BCEC1E10681C1241B04 ...C20400
OOVPA_NO_XREF(D3DDevice_SetTextureState_BorderColor_4, 2048, 10)

        { 0x01, 0x8B },

        { 0x23, 0x8B },
        { 0x24, 0xCE },
        { 0x25, 0xC1 },
        { 0x26, 0xE1 },
        { 0x27, 0x06 },
        { 0x28, 0x81 },
        { 0x29, 0xC1 },
        { 0x2A, 0x24 },

        { 0x44, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_CullMode
// ******************************************************************
//C7000803040075
OOVPA_XREF(D3DDevice_SetRenderState_CullMode, 1049, 2+8,

    XREF_D3DDevice_SetRenderState_CullMode,
    XRefTwo)

        XREF_ENTRY( 0x03, XREF_D3DDEVICE ), // Derived
        XREF_ENTRY( 0x31, XREF_D3DRS_CULLMODE ), // Derived

        { 0x00, 0x56 },

        { 0x1F, 0xC7 },
        { 0x20, 0x00 },
        { 0x21, 0x08 },
        { 0x22, 0x03 },
        { 0x23, 0x04 },
        { 0x24, 0x00 },
        { 0x25, 0x75 },
OOVPA_END;


// ******************************************************************
// * D3DDevice_SetRenderState_NormalizeNormals
// ******************************************************************
//A4030400894804
OOVPA_NO_XREF(D3DDevice_SetRenderState_NormalizeNormals, 1036, 8)

        { 0x00, 0x56 },

        { 0x1F, 0xA4 },
        { 0x20, 0x03 },
        { 0x21, 0x04 },
        { 0x22, 0x00 },
        { 0x23, 0x89 },
        { 0x24, 0x48 },
        { 0x25, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_ZEnable
// ******************************************************************
//C7000C0304008950
OOVPA_NO_XREF(D3DDevice_SetRenderState_ZEnable, 1036, 10)

        { 0x00, 0x56 },
        { 0x01, 0x8B },

        { 0x37, 0xC7 },
        { 0x38, 0x00 },
        { 0x39, 0x0C },
        { 0x3A, 0x03 },
        { 0x3B, 0x04 },
        { 0x3C, 0x00 },
        { 0x3D, 0x89 },
        { 0x3E, 0x50 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_StencilEnable
// ******************************************************************
//018b54240885d2c700
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilEnable, 1036, 10)

        { 0x02, 0x35 },

        { 0x4C, 0x01 },
        { 0x4D, 0x8B },
        { 0x4E, 0x54 },
        { 0x4F, 0x24 },
        { 0x50, 0x08 },
        { 0x51, 0x85 },
        { 0x52, 0xD2 },
        { 0x53, 0xC7 },
        { 0x54, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_FillMode
// ******************************************************************
//C98B4C24087502
OOVPA_NO_XREF(D3DDevice_SetRenderState_FillMode, 1036, 9)

        { 0x00, 0x56 },
        { 0x08, 0x06 },

        { 0x26, 0xC9 },
        { 0x27, 0x8B },
        { 0x28, 0x4C },
        { 0x29, 0x24 },
        { 0x2A, 0x08 },
        { 0x2B, 0x75 },
        { 0x2C, 0x02 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_FogColor
// ******************************************************************
//54240E8BF981
OOVPA_NO_XREF(D3DDevice_SetRenderState_FogColor, 1036, 7)

        { 0x00, 0x56 },

        { 0x20, 0x54 },
        { 0x21, 0x24 },
        { 0x22, 0x0E },
        { 0x23, 0x8B },
        { 0x24, 0xF9 },
        { 0x25, 0x81 },
OOVPA_END;


// ******************************************************************
// * D3DDevice_SetRenderState_ShadowFunc
// ******************************************************************
//6C1E04008D91
OOVPA_NO_XREF(D3DDevice_SetRenderState_ShadowFunc, 1036, 8)

        { 0x00, 0x56 },
        { 0x07, 0x8B },

        { 0x1F, 0x6C },
        { 0x20, 0x1E },
        { 0x21, 0x04 },
        { 0x22, 0x00 },
        { 0x23, 0x8D },
        { 0x24, 0x91 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_EdgeAntiAlias
// ******************************************************************
//0020030800894804
OOVPA_NO_XREF(D3DDevice_SetRenderState_EdgeAntiAlias, 1036, 10)

        { 0x01, 0x8B },
        { 0x1A, 0x4C },

        { 0x1E, 0x00 },
        { 0x1F, 0x20 },
        { 0x20, 0x03 },
        { 0x21, 0x08 },
        { 0x22, 0x00 },
        { 0x23, 0x89 },
        { 0x24, 0x48 },
        { 0x25, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DTexture_GetLevelDesc
// ******************************************************************
// * NOTE: We are actually intercepting Get2DSurfaceDesc, because
// *       GetLevelDesc Simply redirects to that function
// ******************************************************************
//7909C74608010000 ...C3
OOVPA_NO_XREF(Get2DSurfaceDesc_0, 2024, 10)

        { 0x00, 0x0F },
        { 0x01, 0xB6 },

        { 0x2A, 0x79 },
        { 0x2B, 0x09 },
        { 0x2C, 0xC7 },
        { 0x2D, 0x46 },
        { 0x2E, 0x08 },
        { 0x2F, 0x01 },
        { 0x30, 0x00 },
        { 0x31, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_ColorKeyColor
// ******************************************************************
//E00A040089 ...C20400
OOVPA_NO_XREF(D3DDevice_SetTextureState_ColorKeyColor_4, 2048, 7)

        { 0x00, 0x53 },
        { 0x01, 0x8B },

        { 0x26, 0xE0 },
        { 0x27, 0x0A },
        { 0x28, 0x04 },
        { 0x29, 0x00 },
        { 0x2A, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShader
// ******************************************************************
//F6C30155568B35
OOVPA_NO_XREF(D3DDevice_SetVertexShader, 1024, 12)

        { 0x00, 0x53 },
        { 0x01, 0x8B },
        { 0x02, 0x5C },
        { 0x03, 0x24 },
        { 0x04, 0x08 },
        { 0x05, 0xF6 },
        { 0x06, 0xC3 },
        { 0x07, 0x01 },
        { 0x08, 0x55 },
        { 0x09, 0x56 },
        { 0x0A, 0x8B },
        { 0x0B, 0x35 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Swap
// ******************************************************************
//7505BB050000008B
OOVPA_NO_XREF(D3DDevice_Swap, 1036, 9)

        { 0x01, 0x8B },

        { 0x0E, 0x75 },
        { 0x0F, 0x05 },
        { 0x10, 0xBB },
        { 0x11, 0x05 },
        { 0x12, 0x00 },
        { 0x13, 0x00 },
        { 0x14, 0x00 },
        { 0x15, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_IsFencePending
// ******************************************************************
//D12B4424043B
OOVPA_NO_XREF(D3DDevice_IsFencePending, 1024, 8)

        { 0x00, 0xA1 },
        { 0x05, 0x8B },

        { 0x10, 0xD1 },
        { 0x11, 0x2B },
        { 0x12, 0x44 },
        { 0x13, 0x24 },
        { 0x14, 0x04 },
        { 0x15, 0x3B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_VertexBlend
// ******************************************************************
//81CA000200003BC1
OOVPA_NO_XREF(D3DDevice_SetRenderState_VertexBlend, 1036, 10)

        { 0x00, 0x8B },
        { 0x01, 0x15 },

        { 0x12, 0x81 },
        { 0x13, 0xCA },
        { 0x14, 0x00 },
        { 0x15, 0x02 },
        { 0x16, 0x00 },
        { 0x17, 0x00 },
        { 0x18, 0x3B },
        { 0x19, 0xC1 },
OOVPA_END;

//******************************************************************
//* D3DDevice_SetRenderState_TwoSidedLighting
//******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_TwoSidedLighting, 1036, 2+6,

    XRefNoSaveIndex,
    XRefTwo)

        XREF_ENTRY( 0x18, XREF_D3DRS_FILLMODE ),
        XREF_ENTRY( 0x1D, XREF_D3DRS_TWOSIDEDLIGHTING ),

        { 0x00, 0x8B },
        { 0x06, 0x8B },
        { 0x07, 0x44 },
        { 0x08, 0x24 },
        { 0x09, 0x04 },
        { 0x25, 0xE9 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_LogicOp
// ******************************************************************
//18C700BC1704
OOVPA_NO_XREF(D3DDevice_SetRenderState_LogicOp, 1036, 7)

        // XREF_ENTRY( 0x34, XREF_D3DRS_LOGICOP ),

        { 0x01, 0x8B },

        { 0x20, 0x18 },
        { 0x21, 0xC7 },
        { 0x22, 0x00 },
        { 0x23, 0xBC },
        { 0x24, 0x17 },
        { 0x25, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleAntiAlias
// ******************************************************************
//C1E11083E20F
OOVPA_XREF(D3DDevice_SetRenderState_MultiSampleAntiAlias, 1036, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0C, XREF_D3DRS_MULTISAMPLEANTIALIAS ),

        { 0x00, 0x8B },

        { 0x3D, 0xC1 },
        { 0x3E, 0xE1 },
        { 0x3F, 0x10 },
        { 0x40, 0x83 },
        { 0x41, 0xE2 },
        { 0x42, 0x0F },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMask
// ******************************************************************
//83E10FC1E210
OOVPA_XREF(D3DDevice_SetRenderState_MultiSampleMask, 1036, 1+8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0C, XREF_D3DRS_MULTISAMPLEMASK ),

        { 0x00, 0x8B },
        { 0x01, 0x44 },

        { 0x3D, 0x83 },
        { 0x3E, 0xE1 },
        { 0x3F, 0x0F },
        { 0x40, 0xC1 },
        { 0x41, 0xE2 },
        { 0x42, 0x10 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMode
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_MultiSampleMode, 1024, 1+5,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0B, XREF_D3DRS_MULTISAMPLEMODE ),

        { 0x00, 0x8B },
        { 0x15, 0x3B },
        //{ 0x16, 0x90 },
        //{ 0x17, 0x14 },
        //{ 0x18, 0x1A },
        { 0x19, 0x00 },
        { 0x1A, 0x00 },
        { 0x1B, 0x75 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleRenderTargetMode
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, 1024, 1+5,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0B, XREF_D3DRS_MULTISAMPLERENDERTARGETMODE ),

        { 0x01, 0x4C },
        { 0x15, 0x3B },
        //{ 0x16, 0x90 },
        //{ 0x17, 0x14 },
        //{ 0x18, 0x1A },
        { 0x19, 0x00 },
        { 0x1A, 0x00 },
        { 0x1B, 0x74 },
        //{ 0x1C, 0x0D },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_StencilFail
// ******************************************************************
//7003040089
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilFail, 1036, 7)

        { 0x01, 0x8B },
        { 0x1F, 0x33 },

        { 0x5D, 0x70 },
        { 0x5E, 0x03 },
        { 0x5F, 0x04 },
        { 0x60, 0x00 },
        { 0x61, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_FrontFace
// ******************************************************************
//00A00304008948
OOVPA_NO_XREF(D3DDevice_SetRenderState_FrontFace, 1036, 8)

        { 0x01, 0x8B },

        { 0x1E, 0x00 },
        { 0x1F, 0xA0 },
        { 0x20, 0x03 },
        { 0x21, 0x04 },
        { 0x22, 0x00 },
        { 0x23, 0x89 },
        { 0x24, 0x48 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateTexture2
// ******************************************************************
//F744241C0000010074
OOVPA_NO_XREF(D3DDevice_CreateTexture2, 1036, 11)

        { 0x00, 0x53 },
        { 0x01, 0x56 },

        { 0x37, 0xF7 },
        { 0x38, 0x44 },
        { 0x39, 0x24 },
        { 0x3A, 0x1C },
        { 0x3B, 0x00 },
        { 0x3C, 0x00 },
        { 0x3D, 0x01 },
        { 0x3E, 0x00 },
        { 0x3F, 0x74 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderTarget
// ******************************************************************
//0F85..0100008B0D
OOVPA_XREF(D3DDevice_SetRenderTarget, 1036, 1+11,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x4F, XREF_D3DRS_MULTISAMPLEMODE ),

        { 0x00, 0x83 },
        { 0x01, 0xEC },
        { 0x02, 0x3C },

        { 0x47, 0x0F },
        { 0x48, 0x85 },
        //{ 0x49, 0x65 },
        { 0x4A, 0x01 },
        { 0x4B, 0x00 },
        { 0x4C, 0x00 },
        { 0x4D, 0x8B },
        { 0x4E, 0x0D },

        { 0x53, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DeleteVertexShader
// ******************************************************************
//FF08750D8B480485C9740650E8 ...C3
OOVPA_NO_XREF(D3DDevice_DeleteVertexShader_0, 2024, 12)

        { 0x00, 0x8B },
        { 0x01, 0x48 },
        { 0x02, 0xFF },
        { 0x03, 0x48 },
        { 0x04, 0x49 },
        { 0x05, 0x89 },
        { 0x06, 0x08 },
        { 0x07, 0x75 },
        { 0x08, 0x06 },
        { 0x09, 0x50 },
        { 0x0A, 0xE8 },

        { 0x0F, 0xC3 },
OOVPA_END;

//******************************************************************
//* D3DSurface_GetDesc
//******************************************************************
//578B7C241033DBE8
OOVPA_NO_XREF(D3DSurface_GetDesc, 1024, 9)

        { 0x00, 0x53 },

        { 0x06, 0x57 },
        { 0x07, 0x8B },
        { 0x08, 0x7C },
        { 0x09, 0x24 },
        { 0x0A, 0x10 },
        { 0x0B, 0x33 },
        { 0x0C, 0xDB },
        { 0x0D, 0xE8 },
OOVPA_END;

//******************************************************************
//* Lock3DSurface
//******************************************************************
//2418F6C320558B ...C21000
OOVPA_NO_XREF(Lock3DSurface_16, 2048, 8)

        { 0x01, 0x53 },
        { 0x04, 0x24 },
        { 0x05, 0x18 },
        { 0x06, 0xF6 },
        { 0x07, 0xC3 },
        { 0x08, 0x20 },
        { 0x09, 0x55 },
        { 0x0A, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTransform
// ******************************************************************
//568BC8C1E106578DBC ...C3
OOVPA_NO_XREF(D3DDevice_SetTransform_0, 2048, 12)

        { 0x00, 0x53 },
        { 0x01, 0x8B },

        { 0x07, 0x56 },
        { 0x08, 0x8B },
        { 0x09, 0xC8 },
        { 0x0A, 0xC1 },
        { 0x0B, 0xE1 },
        { 0x0C, 0x06 },
        { 0x0D, 0x57 },
        { 0x0E, 0x8D },
        { 0x0F, 0xBC },
        { 0x10, 0x19 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DeletePixelShader
// ******************************************************************
//FF08750D8B480485C9740650E8 ...C3
OOVPA_NO_XREF(D3DDevice_DeletePixelShader_0, 2024, 15)

        { 0x00, 0xFF },
        { 0x01, 0x08 },
        { 0x02, 0x75 },
        { 0x03, 0x0D },
        { 0x04, 0x8B },
        { 0x05, 0x48 },
        { 0x06, 0x04 },
        { 0x07, 0x85 },
        { 0x08, 0xC9 },
        { 0x09, 0x74 },
        { 0x0A, 0x06 },
        { 0x0B, 0x50 },
        { 0x0C, 0xE8 },

        { 0x10, 0xFF },
        { 0x11, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DResource_GetType
// ******************************************************************
//3D00000300772274 ...C3
OOVPA_NO_XREF(D3DResource_GetType_0, 2024, 11)

        { 0x00, 0x8B },
        { 0x01, 0x01 },

        { 0x07, 0x3D },
        { 0x08, 0x00 },
        { 0x09, 0x00 },
        { 0x0A, 0x03 },
        { 0x0B, 0x00 },
        { 0x0C, 0x77 },
        { 0x0D, 0x22 },
        { 0x0E, 0x74 },

        { 0x0F, 0x1A },
OOVPA_END;

// ******************************************************************
// * D3D_BlockOnResource
// ******************************************************************
//F7C20000780075 ...C3
OOVPA_NO_XREF(D3D_BlockOnResource_0, 2036, 9)

        { 0x01, 0x8B },
        { 0x02, 0x35 },

        { 0x22, 0xF7 },
        { 0x23, 0xC2 },
        { 0x24, 0x00 },
        { 0x25, 0x00 },
        { 0x26, 0x78 },
        { 0x27, 0x00 },
        { 0x28, 0x75 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Begin
// ******************************************************************
//814E08000800005EC204
OOVPA_NO_XREF(D3DDevice_Begin, 1048, 12)

        { 0x00, 0x56 },
        { 0x01, 0x8B },

        { 0x30, 0x81 },
        { 0x31, 0x4E },
        { 0x32, 0x08 },
        { 0x33, 0x00 },
        { 0x34, 0x08 },
        { 0x35, 0x00 },
        { 0x36, 0x00 },
        { 0x37, 0x5E },
        { 0x38, 0xC2 },
        { 0x39, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetBackBuffer
// ******************************************************************
//7507B801000000EB07F7 ...C20800
OOVPA_NO_XREF(D3DDevice_GetBackBuffer_8, 2048, 13)

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

        { 0x4C, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData2s
// ******************************************************************
//8D....00190400
OOVPA_NO_XREF(D3DDevice_SetVertexData2s, 1024, 7)

        { 0x00, 0x56 },
        { 0x01, 0x8B },

        { 0x1D, 0x8D },
        //{ 0x21, 0x14 },
        //{ 0x22, 0x8D },
        { 0x20, 0x00 },
        { 0x21, 0x19 },
        { 0x22, 0x04 },
        { 0x23, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData2f
// ******************************************************************
//8D....801808008B
OOVPA_NO_XREF(D3DDevice_SetVertexData2f, 1024, 7)

        { 0x01, 0x8B },
        { 0x1D, 0x8D },

        { 0x20, 0x80 },
        { 0x21, 0x18 },
        { 0x22, 0x08 },
        { 0x23, 0x00 },
        { 0x24, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData4f
// ******************************************************************
//B918150000EB09 ...C21000
OOVPA_NO_XREF(D3DDevice_SetVertexData4f_16, 2024, 9)

        { 0x00, 0x56 },
        { 0x01, 0x8B },

        { 0x1E, 0xB9 },
        { 0x1F, 0x18 },
        { 0x20, 0x15 },
        { 0x21, 0x00 },
        { 0x22, 0x00 },
        { 0x23, 0xEB },
        { 0x24, 0x09 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData4s
// ******************************************************************
//801908000FBF
OOVPA_NO_XREF(D3DDevice_SetVertexData4s, 1024, 7)

        { 0x00, 0x56 },

        { 0x20, 0x80 },
        { 0x21, 0x19 },
        { 0x22, 0x08 },
        { 0x23, 0x00 },
        { 0x24, 0x0F },
        { 0x25, 0xBF },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData4ub
// ******************************************************************
//8D....4019040033
OOVPA_NO_XREF(D3DDevice_SetVertexData4ub, 1024, 7)

        { 0x00, 0x56 },
        { 0x1D, 0x8D },

        { 0x20, 0x40 },
        { 0x21, 0x19 },
        { 0x22, 0x04 },
        { 0x23, 0x00 },
        { 0x24, 0x33 },
OOVPA_END;

// ******************************************************************
// * D3D::SetFence
// ******************************************************************
OOVPA_XREF(D3D_SetFence, 1024, 9,

    XREF_D3D_SetFence,
    XRefZero)

        { 0x00, 0x53 },

        { 0x24, 0xC9 },
        { 0x25, 0x89 },
        { 0x26, 0x48 },
        { 0x27, 0x0C },
        { 0x28, 0x89 },
        { 0x29, 0x48 },
        { 0x2A, 0x14 },
        { 0x2B, 0xC7 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_MultiplyTransform
// ******************************************************************
//558BEC83E4F081EC8800000056578BF0 ...C3
OOVPA_NO_XREF(D3DDevice_MultiplyTransform_0, 2024, 23)

        { 0x00, 0x55 },
        { 0x01, 0x8B },
        { 0x02, 0xEC },
        { 0x03, 0x83 },
        { 0x04, 0xE4 },
        { 0x05, 0xF0 },
        { 0x06, 0x81 },
        { 0x07, 0xEC },
        { 0x08, 0x88 },
        { 0x09, 0x00 },
        { 0x0A, 0x00 },
        { 0x0B, 0x00 },
        { 0x0C, 0x56 },
        { 0x0D, 0x57 },
        { 0x0E, 0x8B },
        { 0x0F, 0xF0 },
        { 0x10, 0xB9 },
        { 0x11, 0x10 },
        { 0x12, 0x00 },
        { 0x13, 0x00 },
        { 0x14, 0x00 },
        { 0x15, 0x8D },
        { 0x16, 0x7C },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetIndices
// ******************************************************************
//74108103000008008B
OOVPA_NO_XREF(D3DDevice_SetIndices, 1024, 11)

        { 0x00, 0x53 },
        { 0x01, 0x8B },

        { 0x0F, 0x74 },
        { 0x10, 0x10 },
        { 0x11, 0x81 },
        { 0x12, 0x03 },
        { 0x13, 0x00 },
        { 0x14, 0x00 },
        { 0x15, 0x08 },
        { 0x16, 0x00 },
        { 0x17, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetStreamSource
// ******************************************************************
//7406810300000800
OOVPA_XREF(D3DDevice_SetStreamSource, 1040, 1+10,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x23, XREF_G_STREAM ), // Derived

        { 0x00, 0x8B },
        { 0x01, 0x0D },

        { 0x13, 0x74 },
        { 0x14, 0x06 },
        { 0x15, 0x81 },
        { 0x16, 0x03 },
        { 0x17, 0x00 },
        { 0x18, 0x00 },
        { 0x19, 0x08 },
        { 0x1A, 0x00 },
OOVPA_END;
