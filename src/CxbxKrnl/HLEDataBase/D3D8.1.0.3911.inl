// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->D3D8.1.0.3911.cpp
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
OOVPA_NO_XREF(Direct3D_CreateDevice, 3911, 18) // Also for 4361, 4627, 5558, 5659, 5788, 5849, 5933 (NOT 5344!)

        { 0x00, 0xA1 },

        { 0x05, 0x85 },
        { 0x06, 0xC0 },
        { 0x07, 0x75 },
        { 0x08, 0x0A },
        { 0x09, 0xC7 },
        { 0x0A, 0x05 },

        { 0x0F, 0x00 },
        { 0x10, 0x00 },
        { 0x11, 0x08 },
        { 0x12, 0x00 },
        { 0x13, 0xA1 },

        { 0x18, 0x85 },
        { 0x19, 0xC0 },
        { 0x1A, 0x75 },
        { 0x1B, 0x0A },
        { 0x1C, 0xC7 },
        { 0x1D, 0x05 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetTexture, named with 2 suffix to match EMUPATCH(D3DDevice_GetTexture2)
// ******************************************************************
#if 0 // Derived XREF_OFFSET_D3DDEVICE_M_TEXTURES is not Implemented yet.
OOVPA_XREF(D3DDevice_GetTexture2, 3911, 1 + 21, // Also for 4361

    XRefNoSaveIndex,
    XRefOne)

    XREF_ENTRY( 0x0E, XREF_OFFSET_D3DDEVICE_M_TEXTURES ), // derived
#endif

OOVPA_NO_XREF(D3DDevice_GetTexture2, 3911, 21) // Also for 4361

        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        { 0x0A, 0x56 },
        { 0x0B, 0x8D },
        { 0x0C, 0xB4 },
        { 0x0D, 0x81 },

        { 0x10, 0x00 },
        { 0x11, 0x00 },
        { 0x12, 0x8B },
        { 0x13, 0x06 },
        { 0x14, 0x85 },
        { 0x15, 0xC0 },
        { 0x16, 0x74 },

        { 0x18, 0x50 },
        { 0x19, 0xE8 },

        { 0x1E, 0x8B },
        { 0x1F, 0x16 },
OOVPA_END;


// ******************************************************************
// * D3DDevice_SetTexture
// ******************************************************************
#if 0 // Derived XREF_OFFSET_D3DDEVICE_M_TEXTURES is not Implemented yet.
OOVPA_XREF(D3DDevice_SetTexture, 3911, 1+28,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x11, XREF_OFFSET_D3DDEVICE_M_TEXTURES ), // derived
#endif
OOVPA_NO_XREF(D3DDevice_SetTexture, 3911, 28)

        { 0x00, 0x51 },
        { 0x01, 0x53 },
        { 0x02, 0x8B },
        { 0x03, 0x5C },
        { 0x04, 0x24 },
        { 0x05, 0x0C },
        { 0x06, 0x55 },
        { 0x07, 0x57 },
        { 0x08, 0x8B },
        { 0x09, 0x3D },

        { 0x0E, 0x8B },
        { 0x0F, 0x84 },
        { 0x10, 0x9F },
        { 0x11, 0xA0 },
        { 0x12, 0x0B },
        { 0x13, 0x00 },
        { 0x14, 0x00 },
        { 0x15, 0x85 },
        { 0x16, 0xC0 },
        { 0x17, 0x89 },
        { 0x18, 0x44 },
        { 0x19, 0x24 },
        { 0x1A, 0x0C },
        { 0x1B, 0x74 },
        { 0x1C, 0x20 },
        { 0x1D, 0x8B },
        { 0x1E, 0x4F },
        { 0x1F, 0x1C },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawRectPatch
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawRectPatch, 3911, 27) // Also verified for 4361, 4627, 5344, 5558, 5659, 5788, 5849, 5933
        { 0x00, 0x83 },
        { 0x01, 0xEC },
        { 0x02, 0x38 },
        { 0x03, 0xA1 },

        { 0x08, 0x8B },
        { 0x09, 0x48 },
        // { 0x0A, 0x0C }, // 0x08 for 4361 and up
        { 0x0B, 0x53 },
        { 0x0C, 0x55 },
        { 0x0D, 0x56 },
        { 0x0E, 0x57 },
        { 0x0F, 0x8B },
        { 0x10, 0x7C },
        { 0x11, 0x24 },
        { 0x12, 0x50 },
        { 0x13, 0x33 },
        { 0x14, 0xED },
        { 0x15, 0x81 },
        { 0x16, 0xC9 },
        { 0x17, 0x00 },
        { 0x18, 0x08 },
        { 0x19, 0x00 },
        { 0x1A, 0x00 },
        { 0x1B, 0x85 },
        { 0x1C, 0xFF },
        { 0x1D, 0x89 },
        { 0x1E, 0x6C },
        { 0x1F, 0x24 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawTriPatch
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawTriPatch, 3911, 27) // Also verified for 4361, 4627, 5344, 5558, 5659, 5788, 5849, 5933

        { 0x00, 0x83 },
        { 0x01, 0xEC },
        { 0x02, 0x14 },
        { 0x03, 0xA1 },

        { 0x08, 0x8B },
        { 0x09, 0x48 },
        // { 0x0A, 0x0C }, // 0x08 for 4361 and up
        { 0x0B, 0x53 },
        { 0x0C, 0x55 },
        { 0x0D, 0x56 },
        { 0x0E, 0x57 },
        { 0x0F, 0x8B },
        { 0x10, 0x7C },
        { 0x11, 0x24 },
        { 0x12, 0x2C },
        { 0x13, 0x81 },
        { 0x14, 0xC9 },
        { 0x15, 0x00 },
        { 0x16, 0x08 },
        { 0x17, 0x00 },
        { 0x18, 0x00 },
        { 0x19, 0x33 },
        { 0x1A, 0xDB },
        { 0x1B, 0x85 },
        { 0x1C, 0xFF },
        { 0x1D, 0x89 },
        { 0x1E, 0x44 },
        { 0x1F, 0x24 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateVertexShader
// ******************************************************************
// Based on Dxbx patterns for 3911, 4361, 4627, 5344, 5558, 5659, 5788, 5849, 5933
OOVPA_NO_XREF(D3DDevice_CreateVertexShader, 3925, 38)
        // IDirect3DDevice8_CreateVertexShader+0x00 : push ecx; push ebx; push ebp
        { 0x00, 0x51 },
        { 0x01, 0x53 },
        { 0x02, 0x55 },

        { 0x03, 0x8B },
        { 0x04, 0x6C },
        { 0x05, 0x24 },
        { 0x06, 0x14 },
        { 0x07, 0x85 },
        { 0x08, 0xED },
        { 0x09, 0x56 },

        // IDirect3DDevice8_CreateVertexShader+0x0A : jz +0x10
        { 0x0A, 0x74 },
        { 0x0B, 0x10 },

        { 0x0C, 0x0F },
        { 0x0D, 0xB7 },
        { 0x0E, 0x45 },
        { 0x0F, 0x00 },
        { 0x10, 0x0F },
        { 0x11, 0xB7 },
        { 0x12, 0x4D },
        { 0x13, 0x02 },
        { 0x14, 0x89 },
        { 0x15, 0x44 },
        { 0x16, 0x24 },
        { 0x17, 0x0C },
        { 0x18, 0x89 },
        { 0x19, 0x4C },
        { 0x1A, 0x24 },
        { 0x1B, 0x18 },
        { 0x1C, 0x33 },
        { 0x1D, 0xF6 },
        { 0x1E, 0x85 },
        { 0x1F, 0xED },

        // D3DDevice_CreateVertexShader+0x43 : shl eax, 0x02
        { 0x43, 0xC1 },
        { 0x44, 0xE0 },
        { 0x45, 0x02 },

        { 0x46, 0x8D },
        { 0x47, 0x1C },
        { 0x48, 0x30 }, 
/*
    OOVPA_NO_XREF(D3DDevice_CreateVertexShader, 5558)

        // D3DDevice_CreateVertexShader+0x73 : mov eax, 0x8007000E
        { 0x65, 0x07 },
        { 0x66, 0x80 },

        // D3DDevice_CreateVertexShader+0x69 : retn 0x10
        { 0x69, 0xC2 },
        { 0x6A, 0x10 },
*/
/*
    OOVPA_NO_XREF(D3DDevice_CreateVertexShader, 5788)

        { 0x3E, 0xE8 },
        { 0x5E, 0x75 },
        { 0x7E, 0x04 },
        { 0x9E, 0x24 },
        { 0xBE, 0x24 },
        { 0xDE, 0x83 },
        { 0xFE, 0xC7 },
*/
/*
    OOVPA_NO_XREF(D3DDevice_CreateVertexShader, 5849)

        { 0x3E, 0xE8 },
        { 0x5E, 0x75 },
        { 0x7E, 0x04 },
        { 0x9E, 0x24 },
        { 0xBE, 0x24 },
        { 0xDE, 0x83 },
        { 0xFE, 0xC7 },
*/
OOVPA_END;


// ******************************************************************
// * D3DDevice_Release
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Release, 3911, 12) // Also for 4034, 4361, 4627, 5344, 5558, 5788, 5849, 5933

        { 0x00, 0x57 },
        { 0x08, 0x87 },
        { 0x0B, 0x00 },
        { 0x0C, 0x00 },
        { 0x0D, 0x83 },
        { 0x0E, 0xF8 },
        { 0x0F, 0x01 },
        { 0x10, 0x75 },
        { 0x11, 0x1C },
        { 0x12, 0x8B },
        { 0x13, 0xCF },
        { 0x14, 0xE8 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetDepthStencilSurface
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetDepthStencilSurface, 3911, 18)

        { 0x00, 0xA1 },

        { 0x05, 0x8B },
        { 0x06, 0x80 },

        { 0x09, 0x00 },
        { 0x0A, 0x00 },
        { 0x0B, 0x85 },
        { 0x0C, 0xC0 },
        { 0x0D, 0x8B },
        { 0x0E, 0x4C },
        { 0x0F, 0x24 },
        { 0x10, 0x04 },
        { 0x11, 0x89 },
        { 0x12, 0x01 },
        { 0x13, 0x74 },
        { 0x14, 0x0B },
        { 0x15, 0x50 },
        { 0x16, 0xE8 },

        { 0x1E, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetGammaRamp
// ******************************************************************
// Generic OOVPA as of 3911 and newer.
OOVPA_NO_XREF(D3DDevice_GetGammaRamp, 3911, 13)

        { 0x05, 0x8B },

        { 0x0A, 0x00 },
        { 0x0B, 0x56 },
        { 0x0C, 0x8D },
        { 0x0D, 0x0C },
        { 0x0E, 0x49 },
        { 0x0F, 0xC1 },
        { 0x10, 0xE1 },
        { 0x11, 0x08 },

        { 0x16, 0x0C },
        { 0x1C, 0x00 },
        { 0x22, 0x00 },
        { 0x27, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3D_SetPushBufferSize
// ******************************************************************
// Generic OOVPA as of 3911 and newer.
OOVPA_NO_XREF(D3D_SetPushBufferSize, 3911, 13)

        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },
        { 0x04, 0x8B },
        { 0x05, 0x4C },
        { 0x06, 0x24 },
        { 0x07, 0x08 },
        { 0x08, 0xA3 },
        { 0x0D, 0x89 },
        { 0x0E, 0x0D },
        { 0x13, 0xC2 },
        { 0x14, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_LightEnable
// ******************************************************************
OOVPA_XREF(D3DDevice_LightEnable, 3911, 1 + 24,// valid upto at least 4627, next known difference is from 5028 onwards

    XRefNoSaveIndex,
    XRefOne)

    XREF_ENTRY(0x0B, XREF_D3DDEVICE),
    //XREF_ENTRY(0x67, XREF_D3DDEVICE_SETLIGHT),

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

        { 0x0F, 0x3B },
        { 0x10, 0x9E },
        // { 0x11, 0x84 }, // 94
        // { 0x12, 0x04 }, // 03
        { 0x13, 0x00 },

        { 0x14, 0x00 },
        { 0x15, 0x73 },
        { 0x16, 0x13 },
        { 0x17, 0x8B },
        { 0x18, 0x8E },

        // { 0x19, 0x80 }, // 90
        // { 0x1A, 0x04 }, // 03
        { 0x1B, 0x00 },
        { 0x1C, 0x00 },
        { 0x1D, 0x8D },
        { 0x1E, 0x04 },
        { 0x1F, 0xDB },
/*
        { 0x36, 0x00 },
        { 0x52, 0x00 },
        { 0x6E, 0x75 },
        { 0x8A, 0x88 },
        { 0xA6, 0x75 },
        { 0xC2, 0x00 },
*/
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetLightEnable
// ******************************************************************
OOVPA_XREF(D3DDevice_GetLightEnable, 3911, 1+24, // valid upto at least 5233, next known difference is from 5344 onwards

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x05, XREF_D3DDEVICE),

        { 0x00, 0x8B },
        { 0x01, 0x4C },
        { 0x02, 0x24 },
        { 0x03, 0x04 },
    
        { 0x04, 0xA1 },
        { 0x09, 0x8B },
        { 0x0A, 0x90 },
        // 0x0B, 0x80 }, // 0x8004 vs 4134 0x9003
        // 0x0C, 0x04 },

        { 0x0D, 0x00 },
        { 0x0E, 0x00 },
        { 0x0F, 0x8B },
        { 0x10, 0x80 },
        // 0x11, 0x88 }, // 0x8804 vs 4134 0x9803

        // 0x12, 0x04 },
        { 0x13, 0x00 },
        { 0x14, 0x00 },
        { 0x15, 0x8D },
        { 0x16, 0x0C },

        { 0x17, 0xC9 },
        { 0x18, 0xC1 },
        { 0x19, 0xE1 },
        { 0x1A, 0x04 },
        { 0x1B, 0x03 },

        { 0x1C, 0xCA },
        { 0x1D, 0x85 },
        { 0x1E, 0xC0 },
        { 0x1F, 0x74 },
OOVPA_END;

// ******************************************************************
// * D3D_ClearStateBlockFlags
// ******************************************************************
OOVPA_XREF(D3D_ClearStateBlockFlags, 3911, 13,

    XREF_D3D_ClearStateBlockFlags,
    XRefZero)

        { 0x01, 0x15 },
        { 0x07, 0x57 },
        { 0x0F, 0xBF },
        { 0x14, 0xF3 },
        { 0x15, 0xAB },
        { 0x16, 0x8B },
        { 0x17, 0x8A },
//      { 0x18, 0x84 },
//      { 0x19, 0x04 },
        { 0x1A, 0x00 },
        { 0x1B, 0x00 },
        { 0x1C, 0x33 },
        { 0x1D, 0xF6 },
        { 0x1E, 0x8D },
        { 0x1F, 0x82 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BeginStateBlock
// ******************************************************************
OOVPA_XREF(D3DDevice_BeginStateBlock, 3911, 7,

    XRefNoSaveIndex,
    XRefOne)

        // D3DDevice_BeginStateBlock+0x0F : call [ClearStateBlockFlags]
        XREF_ENTRY( 0x0A, XREF_D3D_ClearStateBlockFlags ),

        // D3DDevice_BeginStateBlock+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3DDevice_BeginStateBlock+0x05 : mov [eax+C], 0x20
        { 0x05, 0x83 },
        { 0x06, 0x48 },
        { 0x07, 0x0C },//0C VS 08
        { 0x08, 0x20 },
        { 0x09, 0xE9 },
OOVPA_END;

// ******************************************************************
// * D3D_RecordStateBlock
// ******************************************************************
OOVPA_XREF(D3D_RecordStateBlock, 3911, 19,

    XREF_D3D_RecordStateBlock,
    XRefZero)

        { 0x00, 0x55 },
        { 0x01, 0x8B },
        { 0x02, 0xEC },
        { 0x03, 0x83 },
        { 0x04, 0xEC },
        { 0x05, 0x18 },
        { 0x06, 0x53 },
        { 0x07, 0x56 },

        { 0x0F, 0x8B },
        { 0x1F, 0x89 },

        { 0x37, 0x89 },
        { 0x38, 0x45 },
        { 0x39, 0xEC },
        { 0x3A, 0x8A },
        { 0x3B, 0x02 },
        { 0x3C, 0x42 },
        { 0x3D, 0x84 },
        { 0x3E, 0xC0 },
        { 0x5C, 0xE8 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_EndStateBlock
// ******************************************************************
OOVPA_XREF(D3DDevice_EndStateBlock, 3911, 6,

    XRefNoSaveIndex,
    XRefOne)

        // D3DDevice_EndStateBlock+0x0F : call [ClearStateBlockFlags]
        XREF_ENTRY( 0x0A, XREF_D3D_RecordStateBlock ),

        // D3DDevice_EndStateBlock+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3DDevice_EndStateBlock+0x05 : and [eax+0C], 0xFFFFFFDF
        { 0x05, 0x83 },
        { 0x06, 0x60 },
        { 0x07, 0x0C },
        { 0x08, 0xDF },
OOVPA_END;

// ******************************************************************
// * CMiniport::InitHardware
// ******************************************************************
OOVPA_NO_XREF(CMiniport_InitHardware, 3911, 24)
        { 0x00, 0x55 },
        { 0x01, 0x8B },
        { 0x02, 0xEC },
        { 0x03, 0x83 },
        { 0x04, 0xEC },
        { 0x05, 0x0C },
        { 0x06, 0x53 },
        { 0x07, 0x56 },
        { 0x08, 0x8B },
        { 0x09, 0xF1 },
        { 0x0A, 0x56 },
        { 0x0B, 0x68 },

        { 0x10, 0x8D },
        { 0x11, 0x86 },
        { 0x12, 0x88 },
        { 0x13, 0x00 },
        { 0x14, 0x00 },
        { 0x15, 0x00 },
        { 0x16, 0x50 },
        { 0x17, 0xFF },
        { 0x18, 0x15 },

        { 0x1D, 0x80 },
        { 0x1E, 0xA6 },
        { 0x1F, 0xF8 },
OOVPA_END;

// ******************************************************************
// * CMiniport::CreateCtxDmaObject
// ******************************************************************
OOVPA_NO_XREF(CMiniport_CreateCtxDmaObject, 3911, 32)
        { 0x00, 0x55 },
        { 0x01, 0x8B },
        { 0x02, 0xEC },
        { 0x03, 0x51 },
        { 0x04, 0x51 },
        { 0x05, 0x53 },
        { 0x06, 0x56 },
        { 0x07, 0x57 },
        { 0x08, 0x33 },
        { 0x09, 0xF6 },
        { 0x0A, 0x56 },
        { 0x0B, 0x8D },
        { 0x0C, 0x45 },
        { 0x0D, 0xFC },
        { 0x0E, 0x50 },
        { 0x0F, 0x8D },
        { 0x10, 0x45 },
        { 0x11, 0xF8 },
        { 0x12, 0x50 },
        { 0x13, 0xFF },
        { 0x14, 0x75 },
        { 0x15, 0x10 },
        { 0x16, 0x8B },
        { 0x17, 0xD1 },
        { 0x18, 0x8B },
        { 0x19, 0x3A },
        { 0x1A, 0x89 },
        { 0x1B, 0x75 },
        { 0x1C, 0xF8 },
        { 0x1D, 0x89 },
        { 0x1E, 0x75 },
        { 0x1F, 0xFC },
OOVPA_END;

// ******************************************************************
// * D3D_CMiniport_GetDisplayCapabilities
// ******************************************************************
OOVPA_NO_XREF(D3D_CMiniport_GetDisplayCapabilities, 3911, 15)

        // D3D_CMiniport_GetDisplayCapabilities+0x00 : xor eax, eax
        { 0x00, 0x33 },
        { 0x01, 0xC0 },
        // D3D_CMiniport_GetDisplayCapabilities+0x02 : cmp ds:_D3D__AvInfo, eax
        { 0x02, 0x39 },
        { 0x03, 0x05 },
        // D3D_CMiniport_GetDisplayCapabilities+0x08 : jne
        { 0x08, 0x75 },
        { 0x09, 0x0f },
        // D3D_CMiniport_GetDisplayCapabilities+0x0A : push #
        { 0x0A, 0x68 },
        // D3D_CMiniport_GetDisplayCapabilities+0x0F : push eax
        { 0x0F, 0x50 },
        // D3D_CMiniport_GetDisplayCapabilities+0x10 : push 6
        { 0x10, 0x6A },
        { 0x11, 0x06 },
        // D3D_CMiniport_GetDisplayCapabilities+0x12 : push eax
        { 0x12, 0x50 },
        // D3D_CMiniport_GetDisplayCapabilities+0x13 : call ds:AvSendTVEncoderOption
        { 0x13, 0xFF },
        { 0x14, 0x15 },
        // D3D_CMiniport_GetDisplayCapabilities+0x19 : mov    eax, #
        { 0x19, 0xA1 },
        // D3D_CMiniport_GetDisplayCapabilities+0x1E : retn
        { 0x1E, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DBaseTexture_GetLevelCount
// ******************************************************************
/* Accuracy is not gauranteed.
   It might be a ?GetMipmapLevelCount@PixelJar@D3D@@YGKPAUD3DPixelContainer@@@Z
   that has an identical signature... */
OOVPA_NO_XREF(D3DBaseTexture_GetLevelCount, 3911, 13)

        // D3DBaseTexture_GetLevelCount+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // D3DBaseTexture_GetLevelCount+0x04 : movzx eax, [eax+0x0E]
        { 0x04, 0x0F },
        { 0x05, 0xB6 },
        { 0x06, 0x40 },
        { 0x07, 0x0E },

        // D3DBaseTexture_GetLevelCount+0x08 : and eax, 0x0F
        { 0x08, 0x83 },
        { 0x09, 0xE0 },
        { 0x0A, 0x0F },

        // D3DBaseTexture_GetLevelCount+0x0B : retn 0x04
        { 0x0B, 0xC2 },
        { 0x0C, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DCubeTexture_GetCubeMapSurface
// ******************************************************************
OOVPA_NO_XREF(D3DCubeTexture_GetCubeMapSurface, 3911, 11)

        { 0x01, 0xEC },
        { 0x18, 0x54 },
        { 0x19, 0x24 },
        { 0x1A, 0x24 },
        { 0x1B, 0x8D },
        { 0x1C, 0x44 },
        { 0x1D, 0x24 },
        { 0x1E, 0x14 },
        { 0x1F, 0x50 },
        { 0x32, 0x4C },
        { 0x45, 0x51 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetCreationParameters
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetCreationParameters, 3911, 7)

        { 0x06, 0x8B },
        { 0x0E, 0x8B },
        { 0x16, 0x4E },
        { 0x1E, 0x83 },
        { 0x26, 0x00 },
        { 0x2E, 0x5E },
        { 0x36, 0xC9 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetScissors
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetScissors, 3911, 15)

        { 0x00, 0x8B },
        { 0x01, 0x4C },
        { 0x02, 0x24 },
        { 0x03, 0x04 },
        { 0x04, 0x85 },
        { 0x05, 0xC9 },
        { 0x06, 0xA1 },

        { 0x11, 0x00 },
        { 0x18, 0x08 },
        { 0x19, 0x85 },
        { 0x1A, 0xC9 },
        { 0x1B, 0x74 },
        { 0x1C, 0x08 },
        { 0x1D, 0x8B },
        { 0x1E, 0x90 },
//        { 0x1F, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3D::LazySetPointParams
// ******************************************************************
OOVPA_NO_XREF(D3D_LazySetPointParams, 3911, 12)

        { 0x0A, 0x57 },
        { 0x0B, 0x08 },
        { 0x0C, 0x83 },
        { 0x0D, 0xE2 },
        { 0x0E, 0xFE },
        { 0x0F, 0x57 },
        { 0x10, 0x89 },
        { 0x11, 0x57 },
        { 0x12, 0x08 },
        { 0x13, 0xE8 },
        { 0x37, 0x89 },
        { 0x6E, 0x15 },
OOVPA_END;

// ******************************************************************
// * Lock3DSurface
// ******************************************************************
OOVPA_NO_XREF(Lock3DSurface, 3911, 11)

        // D3D::PixelJar::Lock3DSurface + 0x08: test bl, 0x20
        { 0x08, 0xF6 },
        { 0x09, 0xC3 },
        { 0x0A, 0x20 },

        // D3D::PixelJar::Lock3DSurface + 0x40: test bl, 0x40
        { 0x40, 0xF6 },
        { 0x41, 0xC3 },
        { 0x42, 0x40 },

        // D3D::PixelJar::Lock3DSurface + 0x6D: and edx, 0x3C
        { 0x6D, 0x83 },
        { 0x6E, 0xE2 },
        { 0x6F, 0x3C },

        // D3D::PixelJar::Lock3DSurface + 0x99: ret 0x14
        { 0x99, 0xC2 },
        { 0x9A, 0x14 },
OOVPA_END;

// ******************************************************************
// * D3D::CDevice::SetStateUP
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetStateUP, 3911, 16)

        { 0x01, 0xEC },
        { 0x0A, 0xE8 },

        { 0x10, 0x46 },
        { 0x11, 0x08 },
        { 0x12, 0xF6 },
        { 0x13, 0xC4 },
        { 0x14, 0x01 },
        { 0x15, 0x0F },
        { 0x16, 0x84 },
        { 0x17, 0x79 },

        { 0x56, 0x25 },
        { 0x9C, 0x00 },
        { 0x9D, 0x72 },
        { 0x9E, 0xB1 },
        { 0x9F, 0x56 },

        { 0xA5, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3D::CDevice::SetStateVB
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetStateVB, 3911, 16)

        { 0x00, 0x51 },
        { 0x08, 0xE8 },

        { 0x10, 0xA9 },
        { 0x11, 0x00 },
        { 0x12, 0x00 },
        { 0x13, 0x00 },
        { 0x14, 0x40 },
        { 0x15, 0x0F },
        { 0x16, 0x85 },
        { 0x17, 0xCD },

        { 0x56, 0x25 },
        { 0x8C, 0x00 },
        { 0x8D, 0x00 },
        { 0x8E, 0x81 },
        { 0x8F, 0xC1 },

        { 0xA5, 0x8B },
OOVPA_END;

// ******************************************************************
// * Direct3D_CheckDeviceMultiSampleType
// ******************************************************************
OOVPA_NO_XREF(Direct3D_CheckDeviceMultiSampleType, 3911, 7)

        { 0x0E, 0x00 },
        { 0x1E, 0x54 },
        { 0x2E, 0xC9 },
        { 0x3E, 0x8B },
        { 0x4E, 0x08 },
        { 0x5E, 0x72 },
        { 0x6E, 0x03 },
OOVPA_END;


// ******************************************************************
// * D3DDevice_GetOverlayUpdateStatus
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetOverlayUpdateStatus, 3911, 15) // Also for 4134, 4361, 4627, 5344, 5558, 5788, 5849

        { 0x00, 0xA1 },

        { 0x05, 0x8B },
        { 0x06, 0x88 },
//      { 0x07, 0x88 },
//      { 0x08, 0x2A },
        { 0x09, 0x00 },

//      { 0x0E, 0x5C },
//      { 0x0F, 0x24 },
        { 0x10, 0x00 },
        { 0x11, 0x00 },
        { 0x12, 0x33 },
        { 0x13, 0xD2 },
        { 0x14, 0x3B },
        { 0x15, 0xCE },
        { 0x16, 0x0F },
        { 0x17, 0x95 },
        { 0x18, 0xC2 },
        { 0x19, 0x5E },

        { 0x1C, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3D_CheckDeviceFormat
// ******************************************************************
OOVPA_NO_XREF(D3D_CheckDeviceFormat, 3911, 7)

        { 0x0E, 0x18 },
        { 0x1E, 0x00 },
        { 0x2E, 0x42 },
        { 0x41, 0x40 },
        { 0x4E, 0x74 },
        { 0x5E, 0x08 },
        { 0x6E, 0x3C },
OOVPA_END;
// ******************************************************************
// * D3DDevice_EnableOverlay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_EnableOverlay, 3911, 24)

        { 0x00, 0x8B },

        { 0x09, 0x8B },
        { 0x0A, 0x80 },
        { 0x0B, 0x04 },
        { 0x0C, 0x04 },
        { 0x0D, 0x00 },
        { 0x0E, 0x00 },
        { 0x0F, 0x33 },
        { 0x10, 0xC9 },
        { 0x11, 0x3B },
        { 0x12, 0xD1 },
        { 0x13, 0x74 },
        { 0x14, 0x0A },
        { 0x15, 0x39 },
        { 0x16, 0x88 },
        { 0x17, 0x00 },
        { 0x18, 0x87 },
        { 0x19, 0x00 },
        { 0x1A, 0x00 },
        { 0x1B, 0x75 },
        { 0x1C, 0xF8 },
        { 0x1D, 0xEB },
        { 0x1E, 0x0A },
        { 0x1F, 0xC7 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_UpdateOverlay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_UpdateOverlay, 3911, 11)

        { 0x02, 0x08 },
        { 0x1F, 0x7C },
        { 0x6F, 0x8B },

        { 0x80, 0x8B },
        { 0x81, 0x54 },
        { 0x82, 0x24 },
        { 0x83, 0x20 },
        { 0x84, 0x8B },
        { 0x85, 0x3F },
        { 0x86, 0x83 },
        { 0x87, 0xE1 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_TexCoordIndex
// ******************************************************************
OOVPA_XREF(D3DDevice_SetTextureState_TexCoordIndex, 3911, 1+11,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY(0x11, XREF_D3DTSS_TEXCOORDINDEX), // Derived

        // D3DDevice_SetTextureState_TexCoordIndex+0x0B : shl eax, 0x07
        { 0x0B, 0xC1 },
        { 0x0C, 0xE0 },
        { 0x0D, 0x07 },

        // D3DDevice_SetTextureState_TexCoordIndex+0x24 : cmp eax, 0x20000
        { 0x24, 0x81 },
        { 0x25, 0xF9 },
        { 0x28, 0x02 },

        // D3DDevice_SetTextureState_TexCoordIndex+0x3B : mov edi, 0x8511
        { 0x3B, 0xBF },
        { 0x3C, 0x11 },
        { 0x3D, 0x85 },

        // D3DDevice_SetTextureState_TexCoordIndex+0x97 : shl eax, cl
        { 0x97, 0xD3 },
        { 0x98, 0xE0 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_CullMode
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_CullMode, 3911, 2+15,

    XRefNoSaveIndex,
    XRefTwo)

        XREF_ENTRY(0x03, XREF_D3DDEVICE), // Derived
        XREF_ENTRY(0x25, XREF_D3DRS_CULLMODE), // Derived

        // D3DDevice_SetRenderState_CullMode+0x42 : mov dword ptr [eax+8], 0x4039C
        { 0x42, 0xC7 },
        { 0x43, 0x40 },
        { 0x44, 0x08 },
        { 0x45, 0x9C },
        { 0x46, 0x03 },
        { 0x47, 0x04 },

        // D3DDevice_SetRenderState_CullMode+0x49 : add eax, 0x10
        { 0x49, 0x83 },
        { 0x4A, 0xC0 },
        { 0x4B, 0x10 },

        // D3DDevice_SetRenderState_CullMode+0x5F : retn 4
        { 0x5F, 0xC2 },
        { 0x60, 0x04 },

        // D3DDevice_SetRenderState_CullMode+0x4D : add edx, 0x404
        { 0x4D, 0x81 },
        { 0x4E, 0xC2 },
        { 0x4F, 0x04 },
        { 0x50, 0x04 },
OOVPA_END;
