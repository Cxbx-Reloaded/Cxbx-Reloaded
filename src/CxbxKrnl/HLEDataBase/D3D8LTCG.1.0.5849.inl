// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->D3D8LTCG.1.0.5849.inl
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
// * D3DDevice_SetPixelShader
// ******************************************************************
//C70001000000C7812809
OOVPA_NO_XREF(D3DDevice_SetPixelShader, 1048, 12)

        { 0x00, 0x8B },
        { 0x01, 0x54 },

        { 0x14, 0xC7 },
        { 0x15, 0x00 },
        { 0x16, 0x01 },
        { 0x17, 0x00 },
        { 0x18, 0x00 },
        { 0x19, 0x00 },
        { 0x1A, 0xC7 },
        { 0x1B, 0x81 },
        { 0x1C, 0x28 },
        { 0x1D, 0x09 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_StencilEnable
// ******************************************************************
//85FFC700841D0400
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilEnable, 1072, 9)

        { 0x00, 0x56 },
        { 0x02, 0x35 },

        { 0x5C, 0x85 },
        { 0x5D, 0xFF },
        { 0x5E, 0xC7 },
        { 0x5F, 0x00 },
        { 0x60, 0x84 },
        { 0x61, 0x1D },
        { 0x62, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_StencilEnable
// ******************************************************************
//85FFC700841D0400
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilEnable, 1084, 9)

        { 0x00, 0x56 },
        { 0x02, 0x35 },

        { 0x5B, 0x85 },
        { 0x5C, 0xFF },
        { 0x5D, 0xC7 },
        { 0x5E, 0x00 },
        { 0x5F, 0x84 },
        { 0x60, 0x1D },
        { 0x61, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DTexture_GetLevelDesc
// ******************************************************************
// * NOTE: We are actually intercepting Get2DSurfaceDesc, because
// *       GetLevelDesc Simply redirects to that function
// ******************************************************************
//7909C74608010000
OOVPA_NO_XREF(Get2DSurfaceDesc, 1024, 10)

        { 0x00, 0x83 },
        { 0x01, 0xEC },

        { 0x38, 0x79 },
        { 0x39, 0x09 },
        { 0x3A, 0xC7 },
        { 0x3B, 0x46 },
        { 0x3C, 0x08 },
        { 0x3D, 0x01 },
        { 0x3E, 0x00 },
        { 0x3F, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DResource_Release
// ******************************************************************
//068BC881E1FFFF000083F90175
OOVPA_NO_XREF(D3DResource_Release, 1024, 15)

        { 0x00, 0x56 },

        { 0x06, 0x06 },
        { 0x07, 0x8B },
        { 0x08, 0xC8 },
        { 0x09, 0x81 },
        { 0x0A, 0xE1 },
        { 0x0B, 0xFF },
        { 0x0C, 0xFF },
        { 0x0D, 0x00 },
        { 0x0E, 0x00 },
        { 0x0F, 0x83 },
        { 0x10, 0xF9 },
        { 0x11, 0x01 },
        { 0x12, 0x75 },

        { 0x17, 0x07 },
OOVPA_END;

// ******************************************************************
// * D3D_BlockOnTime
// ******************************************************************
//C747101001040089
OOVPA_XREF(D3D_BlockOnTime, 1048, 10,

    XREF_D3D_BlockOnTime,
    XRefZero)

        { 0x00, 0x56 },
        { 0x01, 0x8B },

        { 0xF0, 0xC7 },
        { 0xF1, 0x47 },
        { 0xF2, 0x10 },
        { 0xF3, 0x10 },
        { 0xF4, 0x01 },
        { 0xF5, 0x04 },
        { 0xF6, 0x00 },
        { 0xF7, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3D_BlockOnTime
// ******************************************************************
//C747101001040089 ...C20400
OOVPA_XREF(D3D_BlockOnTime_4, 2060, 9,

    XREF_D3D_BlockOnTime,
    XRefZero)

        { 0x00, 0x56 },

        { 0xF4, 0xC7 },
        { 0xF5, 0x47 },
        { 0xF6, 0x10 },
        { 0xF7, 0x10 },
        { 0xF8, 0x01 },
        { 0xF9, 0x04 },
        { 0xFA, 0x00 },
        { 0xFB, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3D_KickOffAndWaitForIdle
// ******************************************************************
//8B4C24088B442404E8
OOVPA_NO_XREF(D3D_KickOffAndWaitForIdle2, 1036, 12)

        { 0x00, 0xA1 },

        { 0x0F, 0x8B },
        { 0x10, 0x4C },
        { 0x11, 0x24 },
        { 0x12, 0x08 },
        { 0x13, 0x8B },
        { 0x14, 0x44 },
        { 0x15, 0x24 },
        { 0x16, 0x04 },
        { 0x17, 0xE8 },

        { 0x1C, 0xC2 },
        { 0x1D, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_TwoSidedLighting
// ******************************************************************
//C700C417040089
OOVPA_NO_XREF(D3DDevice_SetRenderState_TwoSidedLighting, 1084, 8)

        { 0x01, 0x8B },

        { 0x22, 0xC7 },
        { 0x23, 0x00 },
        { 0x24, 0xC4 },
        { 0x25, 0x17 },
        { 0x26, 0x04 },
        { 0x27, 0x00 },
        { 0x28, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_ZBias
// ******************************************************************
//241485FF0F95C085
OOVPA_NO_XREF(D3DDevice_SetRenderState_ZBias, 1024, 9)

        // XREF_ENTRY( 0x6A, XREF_D3DRS_ZBIAS ),

        { 0x00, 0x51 },

        { 0x08, 0x24 },
        { 0x09, 0x14 },
        { 0x0A, 0x85 },
        { 0x0B, 0xFF },
        { 0x0C, 0x0F },
        { 0x0D, 0x95 },
        { 0x0E, 0xC0 },
        { 0x0F, 0x85 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_StencilFail
// ******************************************************************
//7003040089
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilFail, 1072, 7)

        { 0x01, 0x8B },
        { 0x2E, 0x33 },

        { 0x68, 0x70 },
        { 0x69, 0x03 },
        { 0x6A, 0x04 },
        { 0x6B, 0x00 },
        { 0x6C, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_StencilFail
// ******************************************************************
//7003040089
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilFail, 1084, 7)

        { 0x01, 0x8B },
        { 0x2D, 0x33 },

        { 0x67, 0x70 },
        { 0x68, 0x03 },
        { 0x69, 0x04 },
        { 0x6A, 0x00 },
        { 0x6B, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Release
// ******************************************************************
//8B873809000083
OOVPA_NO_XREF(D3DDevice_Release, 1024, 8)

        { 0x02, 0x3D },
        { 0x07, 0x8B },
        { 0x08, 0x87 },
        { 0x09, 0x38 },
        { 0x0A, 0x09 },
        { 0x0B, 0x00 },
        { 0x0C, 0x00 },
        { 0x0D, 0x83 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTile
// ******************************************************************
//83EC1885C05356578B3D ...C3
OOVPA_NO_XREF(D3DDevice_SetTile_0, 2072, 10)

        { 0x00, 0x83 },
        { 0x01, 0xEC },
        { 0x02, 0x18 },
        { 0x03, 0x85 },
        { 0x04, 0xC0 },
        { 0x05, 0x53 },
        { 0x06, 0x56 },
        { 0x07, 0x57 },
        { 0x08, 0x8B },
        { 0x09, 0x3D },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetFlickerFilter
// ******************************************************************
//80281C00006A0056 ...C3
OOVPA_NO_XREF(D3DDevice_SetFlickerFilter_0, 2048, 9)

        { 0x00, 0x8B },

        { 0x18, 0x80 },
        { 0x19, 0x28 },
        { 0x1A, 0x1C },
        { 0x1B, 0x00 },
        { 0x1C, 0x00 },
        { 0x1D, 0x6A },
        { 0x1E, 0x00 },
        { 0x1F, 0x56 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_LoadVertexShaderProgram
// ******************************************************************
//B75802558B2D
OOVPA_NO_XREF(D3DDevice_LoadVertexShaderProgram, 1024, 7)

        { 0x01, 0x44 },

        { 0x06, 0xB7 },
        { 0x07, 0x58 },
        { 0x08, 0x02 },
        { 0x09, 0x55 },
        { 0x0A, 0x8B },
        { 0x0B, 0x2D },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SelectVertexShader
// ******************************************************************
//00008B4904578D
OOVPA_NO_XREF(D3DDevice_SelectVertexShader, 1024, 8)

        { 0x00, 0x8B },

        { 0x13, 0x00 },
        { 0x14, 0x00 },
        { 0x15, 0x8B },
        { 0x16, 0x49 },
        { 0x17, 0x04 },
        { 0x18, 0x57 },
        { 0x19, 0x8D },
OOVPA_END;

// ******************************************************************
// * D3DDevice_LoadVertexShader
// ******************************************************************
//C7009C1E040089 ...C20400
OOVPA_NO_XREF(D3DDevice_LoadVertexShader_4, 2048, 8)

        { 0x01, 0x55 },

        { 0x2F, 0xC7 },
        { 0x30, 0x00 },
        { 0x31, 0x9C },
        { 0x32, 0x1E },
        { 0x33, 0x04 },
        { 0x34, 0x00 },
        { 0x35, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BeginVisibilityTest
// ******************************************************************
//C700C8170800B9
OOVPA_NO_XREF(D3DDevice_BeginVisibilityTest, 1048, 9)

        { 0x00, 0x56 },
        { 0x01, 0x8B },

        { 0x1D, 0xC7 },
        { 0x1E, 0x00 },
        { 0x1F, 0xC8 },
        { 0x20, 0x17 },
        { 0x21, 0x08 },
        { 0x22, 0x00 },
        { 0x23, 0xB9 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetViewportOffsetAndScale
// ******************************************************************
//8B88E00E0000DB
OOVPA_NO_XREF(D3DDevice_GetViewportOffsetAndScale, 1024, 8)

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0xE0 },
        { 0x08, 0x0E },
        { 0x09, 0x00 },
        { 0x0A, 0x00 },
        { 0x0B, 0xDB },

        { 0x19, 0x05 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData2f
// ******************************************************************
//8D....801808008B
OOVPA_NO_XREF(D3DDevice_SetVertexData2f, 1048, 7)

        { 0x01, 0x8B },
        { 0x21, 0x8D },

        { 0x24, 0x80 },
        { 0x25, 0x18 },
        { 0x26, 0x08 },
        { 0x27, 0x00 },
        { 0x28, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData2s
// ******************************************************************
//8D....00190400
OOVPA_NO_XREF(D3DDevice_SetVertexData2s, 1048, 7)

        { 0x00, 0x56 },
        { 0x01, 0x8B },

        { 0x21, 0x8D },
        //{ 0x22, 0x0C },
        //{ 0x23, 0x75 },
        { 0x24, 0x00 },
        { 0x25, 0x19 },
        { 0x26, 0x04 },
        { 0x27, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData4f
// ******************************************************************
//B918150000EB09
OOVPA_NO_XREF(D3DDevice_SetVertexData4f, 1036, 9)

        { 0x00, 0x56 },
        { 0x01, 0x8B },

        { 0x26, 0xB9 },
        { 0x27, 0x18 },
        { 0x28, 0x15 },
        { 0x29, 0x00 },
        { 0x2A, 0x00 },
        { 0x2B, 0xEB },
        { 0x2C, 0x09 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData4s
// ******************************************************************
//801908000FBF
OOVPA_NO_XREF(D3DDevice_SetVertexData4s, 1048, 7)

        { 0x00, 0x56 },

        { 0x34, 0x80 },
        { 0x35, 0x19 },
        { 0x36, 0x08 },
        { 0x37, 0x00 },
        { 0x38, 0x0F },
        { 0x39, 0xBF },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData4ub
// ******************************************************************
//8D....4019040033
OOVPA_NO_XREF(D3DDevice_SetVertexData4ub, 1048, 7)

        { 0x00, 0x56 },
        { 0x21, 0x8D },

        { 0x24, 0x40 },
        { 0x25, 0x19 },
        { 0x26, 0x04 },
        { 0x27, 0x00 },
        { 0x28, 0x33 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_End
// ******************************************************************
//108956085E74076A
OOVPA_NO_XREF(D3DDevice_End, 1024, 9)

        { 0x00, 0x56 },

        { 0x3C, 0x10 },
        { 0x3D, 0x89 },
        { 0x3E, 0x56 },
        { 0x3F, 0x08 },
        { 0x40, 0x5E },
        { 0x41, 0x74 },
        { 0x42, 0x07 },
        { 0x43, 0x6A },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BeginPushBuffer
// ******************************************************************
//008107000008008B
OOVPA_NO_XREF(D3DDevice_BeginPushBuffer, 1024, 10)

        { 0x00, 0x56 },
        { 0x01, 0x8B },

        { 0x45, 0x00 },
        { 0x46, 0x81 },
        { 0x47, 0x07 },
        { 0x48, 0x00 },
        { 0x49, 0x00 },
        { 0x4A, 0x08 },
        { 0x4B, 0x00 },
        { 0x4C, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Begin
// ******************************************************************
//814E08000800005EC204
OOVPA_NO_XREF(D3DDevice_Begin, 1036, 12)

        { 0x00, 0x56 },
        { 0x01, 0x8B },

        { 0x34, 0x81 },
        { 0x35, 0x4E },
        { 0x36, 0x08 },
        { 0x37, 0x00 },
        { 0x38, 0x08 },
        { 0x39, 0x00 },
        { 0x3A, 0x00 },
        { 0x3B, 0x5E },
        { 0x3C, 0xC2 },
        { 0x3D, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3D_MakeRequestedSpace
// ******************************************************************
//81C5004000003BE9
OOVPA_NO_XREF(D3D_MakeRequestedSpace, 1024, 10)

        { 0x00, 0x51 },
        { 0x01, 0x53 },

        { 0x57, 0x81 },
        { 0x58, 0xC5 },
        { 0x59, 0x00 },
        { 0x5A, 0x40 },
        { 0x5B, 0x00 },
        { 0x5C, 0x00 },
        { 0x5D, 0x3B },
        { 0x5E, 0xE9 },
OOVPA_END;

// ******************************************************************
// * D3D_MakeRequestedSpace
// ******************************************************************
//81C5004000003BE9
OOVPA_NO_XREF(D3D_MakeRequestedSpace, 1036, 10)

        { 0x00, 0x51 },
        { 0x01, 0x53 },

        { 0x58, 0x81 },
        { 0x59, 0xC5 },
        { 0x5A, 0x00 },
        { 0x5B, 0x40 },
        { 0x5C, 0x00 },
        { 0x5D, 0x00 },
        { 0x5E, 0x3B },
        { 0x5F, 0xE9 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreatePixelShader
// ******************************************************************
//85C07508B80E000780C2
OOVPA_NO_XREF(D3DDevice_CreatePixelShader, 1024, 12)

        { 0x00, 0x8B },
        { 0x01, 0x0D },

        { 0x22, 0x85 },
        { 0x23, 0xC0 },
        { 0x24, 0x75 },
        { 0x25, 0x08 },
        { 0x26, 0xB8 },
        { 0x27, 0x0E },
        { 0x28, 0x00 },
        { 0x29, 0x07 },
        { 0x2A, 0x80 },
        { 0x2B, 0xC2 },
OOVPA_END;
