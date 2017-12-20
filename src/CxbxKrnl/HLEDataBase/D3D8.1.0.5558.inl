// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->D3D8.1.0.5558.inl
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

// ******************************************************************
// * D3DDevice_LazySetStateVB
// ******************************************************************
OOVPA_XREF(D3DDevice_LazySetStateVB, 5558, 12,

    XREF_D3DDevice_LazySetStateVB,
    XRefZero)

        { 0x00, 0x83 },
        { 0x01, 0xEC },
        { 0x02, 0x0C },

        { 0x34, 0xF6 },
        { 0x35, 0xC3 },
        { 0x36, 0x40 },

        { 0x57, 0x83 },
        { 0x58, 0xE5 },
        { 0x59, 0x10 },

        { 0x79, 0x17 },
        { 0x7A, 0x40 },
        { 0x7B, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetMaterial, 5558, 11)

        { 0x04, 0x08 },

        { 0x0C, 0x81 },
        { 0x0D, 0xC7 },
        { 0x0E, 0xF0 },
        { 0x0F, 0x0E },
        { 0x10, 0x00 },

        { 0x16, 0x00 },
        { 0x1F, 0x81 },
        { 0x22, 0x90 },
        { 0x2C, 0x5E },
        { 0x2E, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetMaterial, 5558, 8)

        { 0x05, 0x56 },
        { 0x06, 0x57 },
        { 0x0A, 0x0C },

        { 0x0D, 0xF0 },
        { 0x0E, 0x0E },

        { 0x12, 0x11 },
        { 0x16, 0xF3 },
        { 0x1A, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BlockUntilVerticalBlank
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BlockUntilVerticalBlank, 5558, 11)

        // D3DDevice_BlockUntilVerticalBlank+0x05 : push 0; push 0; push 1
        { 0x05, 0x6A },
        { 0x06, 0x00 },
        { 0x07, 0x6A },
        { 0x08, 0x00 },
        { 0x09, 0x6A },
        { 0x0A, 0x01 },

        // D3DDevice_BlockUntilVerticalBlank+0x17 : add eax, 0x1DAC
        { 0x17, 0x05 },
        { 0x18, 0xAC },
        { 0x19, 0x1D },

        // D3DDevice_BlockUntilVerticalBlank+0x1D : call [KrnlImport]
        { 0x1D, 0xFF },

        // D3DDevice_BlockUntilVerticalBlank+0x23 : retn
        { 0x23, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTransform
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTransform, 5558, 12)

        { 0x04, 0x8B },
        { 0x05, 0x54 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },
        { 0x08, 0x53 },
        { 0x09, 0x8B },
        { 0x0A, 0x1D },
        { 0x0F, 0x56 },

        { 0x2C, 0x8B },
        { 0x2D, 0x0C },

        { 0x9C, 0x00 },
        { 0x9D, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3D::SetFence
// ******************************************************************
OOVPA_XREF(D3D_SetFence, 5558, 9,

    XREF_D3D_SetFence,
    XRefZero)

        { 0x07, 0x8B },
        { 0x09, 0x3B },
        { 0x0A, 0x46 },
        { 0x0B, 0x04 },
        { 0x0F, 0xE8 },
        { 0x14, 0x8B },
        { 0x15, 0x7E },
        { 0x16, 0x2C },

        { 0x39, 0x0C },
OOVPA_END;

// ******************************************************************
// * D3D::BlockOnTime
// ******************************************************************
OOVPA_XREF(D3D_BlockOnTime, 5558, 12,

    XREF_D3D_BlockOnTime,
    XRefZero)

        { 0x02, 0x35 },

        { 0x08, 0x46 },
        { 0x1F, 0x83 },

        { 0x52, 0x8B },
        { 0x53, 0x4D },
        { 0x54, 0x04 },
        { 0x55, 0x3B },
        { 0x56, 0xCA },
        { 0x57, 0x77 },
        { 0x58, 0x03 },
        { 0x59, 0x03 },
        { 0x5A, 0x4E },
/*
        { 0x09, 0x30 },
        { 0x27, 0x07 },
        { 0x7E, 0x25 },
        { 0x9F, 0x20 },
        { 0xD3, 0x56 },
        { 0xE9, 0x57 },*/
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetScissors
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetScissors, 5558, 8)

        { 0x1D, 0x44 },
        { 0x3C, 0x8B },
        { 0x5D, 0xD9 },
        { 0x7D, 0xD8 },
        { 0x99, 0x0E },
        { 0xB8, 0xE8 },
        { 0xD7, 0x24 },
        { 0xF6, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_AddRef
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_AddRef, 5558, 10)

        // D3DDevice_AddRef+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3DDevice_AddRef+0x05 : mov ecx, [eax+0x0934]
        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x34 },
        { 0x08, 0x09 },

        // D3DDevice_AddRef+0x0B : inc ecx
        { 0x0B, 0x41 },

        // D3DDevice_AddRef+0x0C : mov [eax+0x0934], ecx
        { 0x0C, 0x89 },
        { 0x0D, 0x88 },
        { 0x0E, 0x34 },
        { 0x0F, 0x09 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVerticalBlankCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVerticalBlankCallback, 5558, 7)

        { 0x01, 0x44 },
        { 0x04, 0x8B },
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0xA8 },
        { 0x0D, 0x1D },
        { 0x10, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetSwapCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetSwapCallback, 5558, 7)

        { 0x01, 0x44 },
        { 0x04, 0x8B },
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0xA4 },
        { 0x0D, 0x1D },
        { 0x10, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Reset
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Reset, 5558, 13)

        { 0x00, 0x53 },
        { 0x27, 0x8B },
        { 0x28, 0x74 },
        { 0x29, 0x24 },
        { 0x2A, 0x14 },
        { 0x2B, 0x56 },
        { 0x2C, 0x8B },
        { 0x2D, 0xCF },
        { 0x2E, 0xE8 },
        { 0x8B, 0x6A },
        { 0x9A, 0xE8 },
        { 0x9F, 0x33 },
        { 0xA6, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetGammaRamp
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetGammaRamp, 5558, 7)

        { 0x0F, 0x68 },
        { 0x20, 0x34 },
        { 0x31, 0xB4 },
        { 0x42, 0x08 },
        { 0x53, 0xF3 },
        { 0x64, 0xCA },
        { 0x75, 0x07 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetBackMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetBackMaterial, 5558, 11)

        { 0x04, 0x08 },

        { 0x0C, 0x81 },
        { 0x0D, 0xC7 },
        { 0x0E, 0x34 },
        { 0x0F, 0x0F },
        { 0x10, 0x00 },

        { 0x16, 0x00 },
        { 0x1F, 0x81 },
        { 0x22, 0x90 },
        { 0x2C, 0x5E },
        { 0x2E, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetBackMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetBackMaterial, 5558, 8)

        { 0x05, 0x56 },
        { 0x06, 0x57 },
        { 0x0A, 0x0C },

        { 0x0D, 0x34 },
        { 0x0E, 0x0F },

        { 0x12, 0x11 },
        { 0x16, 0xF3 },
        { 0x1A, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3D::MakeRequestedSpace
// ******************************************************************
OOVPA_XREF(D3D_MakeRequestedSpace, 5558, 26, // Also for 5659

	XREF_D3D_MakeRequestedSpace,
	XRefZero)

		{ 0x00, 0x51 },
		{ 0x01, 0x56 },
		{ 0x02, 0x8B },
		{ 0x03, 0x35 },

		{ 0x08, 0xF6 },
		{ 0x09, 0x46 },
		{ 0x0A, 0x08 },
		{ 0x0B, 0x04 },
		{ 0x0C, 0x57 },
		{ 0x0D, 0x74 },
		{ 0x0E, 0x26 },
		{ 0x0F, 0x8B },
		{ 0x10, 0x86 },
	//	{ 0x11, 0x60 }, // 0x64 for 5558.4
		{ 0x12, 0x07 },
		{ 0x13, 0x00 },
		{ 0x14, 0x00 },
		{ 0x15, 0x8B },
		{ 0x16, 0x78 },
		{ 0x17, 0x04 },
		{ 0x18, 0x8B },
		{ 0x19, 0x0E },
		{ 0x1A, 0x8B },
		{ 0x1B, 0x96 },
	//	{ 0x1C, 0x64 }, // 0x68 for 5558.4
		{ 0x1D, 0x07 },
		{ 0x1E, 0x00 },
		{ 0x1F, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_PersistDisplay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_PersistDisplay, 5558, 12)

        { 0x00, 0x83 },
        { 0x1B, 0x6A },

        { 0x28, 0x00 },
        { 0x29, 0x85 },
        { 0x2A, 0xC0 },
        { 0x2B, 0x75 },
        { 0x2C, 0x0A },
        { 0x2D, 0xB8 },
        { 0x2E, 0x05 },
        { 0x2F, 0x40 },

        { 0x49, 0x85 },
        { 0x6B, 0xE1 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetViewportOffsetAndScale
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetViewportOffsetAndScale, 5558, 8)

        { 0x1E, 0xD8 },
        { 0x40, 0xD8 },
        { 0x5E, 0xC9 },
        { 0x8C, 0x0E },
        { 0x9D, 0x0E },
        { 0xBE, 0xD8 },
        { 0xDD, 0x14 },
        { 0xFE, 0xC1 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetProjectionViewportMatrix
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetProjectionViewportMatrix, 5558, 15)

        { 0x02, 0x35 },
        { 0x07, 0x57 },
        { 0x0F, 0x09 },
        { 0x10, 0x00 },
        { 0x11, 0x00 },
        { 0x12, 0xB9 },
        { 0x13, 0x10 },
        { 0x14, 0x00 },
        { 0x15, 0x00 },
        { 0x16, 0x00 },
        { 0x17, 0xF3 },
        { 0x18, 0xA5 },
        { 0x19, 0x5F },
        { 0x1A, 0x5E },
        { 0x1B, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_RunPushBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_RunPushBuffer, 5558, 6)

        { 0x0E, 0xED },
        { 0x42, 0x18 },
        { 0x62, 0x04 },
        { 0x8D, 0x1C },
        { 0xAF, 0x0F },
        { 0xF6, 0x41 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetShaderConstantMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetShaderConstantMode, 5558, 16)

        { 0x00, 0xA1 },

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x18 },
        { 0x08, 0x19 },
        { 0x09, 0x00 },
        { 0x0A, 0x00 },
        { 0x0B, 0x8B },
        { 0x0C, 0x54 },
        { 0x0D, 0x24 },
        { 0x0E, 0x04 },
        { 0x0F, 0x89 },
        { 0x10, 0x0A },
        { 0x11, 0xC2 },
        { 0x12, 0x04 },
        { 0x13, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetPixelShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetPixelShader, 5558, 16)

        { 0x00, 0xA1 },

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x80 },
        { 0x08, 0x07 },
        { 0x09, 0x00 },
        { 0x0A, 0x00 },
        { 0x0B, 0x8B },
        { 0x0C, 0x54 },
        { 0x0D, 0x24 },
        { 0x0E, 0x04 },
        { 0x0F, 0x89 },
        { 0x10, 0x0A },
        { 0x11, 0xC2 },
        { 0x12, 0x04 },
        { 0x13, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVertexShader, 5558, 16)

        { 0x00, 0xA1 },

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x94 },
        { 0x08, 0x07 },
        { 0x09, 0x00 },
        { 0x0A, 0x00 },
        { 0x0B, 0x8B },
        { 0x0C, 0x54 },
        { 0x0D, 0x24 },
        { 0x0E, 0x04 },
        { 0x0F, 0x89 },
        { 0x10, 0x0A },
        { 0x11, 0xC2 },
        { 0x12, 0x04 },
        { 0x13, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetTexture, named with 2 suffix to match EMUPATCH(D3DDevice_GetTexture2)
// ******************************************************************
#ifndef WIP_LessVertexPatching
OOVPA_NO_XREF(D3DDevice_GetTexture2, 5558, 23) // Up to 5659
#else
OOVPA_XREF(D3DDevice_GetTexture2, 5558, 1+23, // Up to 5659

	XRefNoSaveIndex,
	XRefOne)

		XREF_ENTRY( 0x0E, XREF_OFFSET_D3DDEVICE_M_TEXTURES ), // Derived
#endif
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
		{ 0x0E, 0x78 }, // GetTexture2 780F vs GetPalette2 880F
		{ 0x0F, 0x0F },

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
		{ 0x1F, 0x06 },
//        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * Rollback support signature(s)
// ******************************************************************
#define Direct3D_CreateDevice_5558 Direct3D_CreateDevice_3911
#define D3DDevice_GetTransform_5558 D3DDevice_GetTransform_4039
#define D3DDevice_SetVertexShaderConstantNotInline_5558 D3DDevice_SetVertexShaderConstantNotInline_4627
