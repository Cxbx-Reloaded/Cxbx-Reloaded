// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->D3D8.1.0.5558.cpp
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
// * D3D_CDevice_LazySetStateVB
// ******************************************************************
OOVPA_XREF(D3D_CDevice_LazySetStateVB_1_0_5558, 12,

    XREF_D3DCD_LAZYSETSTATEVB,
    XRefZero)

        { 0x00, 0x83 }, // 1
        { 0x01, 0xEC }, // 2
        { 0x02, 0x0C }, // 3

        { 0x34, 0xF6 }, // 4
        { 0x35, 0xC3 }, // 5
        { 0x36, 0x40 }, // 6

        { 0x57, 0x83 }, // 7
        { 0x58, 0xE5 }, // 8
        { 0x59, 0x10 }, // 9

        { 0x79, 0x17 }, // 10
        { 0x7A, 0x40 }, // 11
        { 0x7B, 0x00 }, // 12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_BeginPush
// ******************************************************************
OOVPA_XREF(IDirect3DDevice8_BeginPush_1_0_5558, 10,

    XRefNoSaveIndex,
    XRefOne)

        { 0x09, XREF_D3DCD_LAZYSETSTATEVB }, // 1

        { 0x00, 0x8B }, // 2
        { 0x01, 0x0D }, // 3

        { 0x06, 0x6A }, // 4
        { 0x07, 0x00 }, // 5

        { 0x11, 0x40 }, // 6

        { 0x12, 0x89 }, // 7
        { 0x13, 0x44 }, // 8
        { 0x14, 0x24 }, // 9
        { 0x15, 0x04 }, // 10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SelectVertexShader
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SelectVertexShader_1_0_5558, 7)

        { 0x12, 0x07 },
        { 0x26, 0x00 },
        { 0x3A, 0x0D },
        { 0x4F, 0x8B },
        { 0x62, 0xE1 },
        { 0x76, 0x3B },
        { 0x8A, 0x89 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_CreateVertexShader
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_CreateVertexShader_1_0_5558, 12)

        // IDirect3DDevice8_CreateVertexShader+0x00 : push ecx; push ebx; push ebp
        { 0x00, 0x51 }, // (Offset,Value)-Pair #1
        { 0x01, 0x53 }, // (Offset,Value)-Pair #2
        { 0x02, 0x55 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_CreateVertexShader+0x0A : jz +0x10
        { 0x0A, 0x74 }, // (Offset,Value)-Pair #4
        { 0x0B, 0x10 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_CreateVertexShader+0x43 : shl eax, 0x02
        { 0x43, 0xC1 }, // (Offset,Value)-Pair #6
        { 0x44, 0xE0 }, // (Offset,Value)-Pair #7
        { 0x45, 0x02 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_CreateVertexShader+0x73 : mov eax, 0x8007000E
        { 0x65, 0x07 }, // (Offset,Value)-Pair #11
        { 0x66, 0x80 }, // (Offset,Value)-Pair #12

        // IDirect3DDevice8_CreateVertexShader+0x69 : retn 0x10
        { 0x69, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x6A, 0x10 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetVertexShaderConstant1
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetVertexShaderConstant1_1_0_5558, 11)

        // IDirect3DDevice8_SetVertexShaderConstant1+0x05 : add eax, 0x1C
        /*{ 0x05, 0x83 }, // (Offset,Value)-Pair #1
        { 0x06, 0xC0 }, // (Offset,Value)-Pair #2
        { 0x07, 0x1C }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetVertexShaderConstant1+0x0E : jnb +0x2E
        { 0x0E, 0x73 }, // (Offset,Value)-Pair #4
        { 0x0F, 0x2E }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetVertexShaderConstant1+0x28 : mov [eax-0x10], ecx
        { 0x28, 0x89 }, // (Offset,Value)-Pair #6
        { 0x29, 0x48 }, // (Offset,Value)-Pair #7
        { 0x2A, 0xF0 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetVertexShaderConstant1+0x3D : retn
        { 0x3D, 0xC3 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetVertexShaderConstant1+0x47 : jmp +0xB7
        { 0x47, 0xEB }, // (Offset,Value)-Pair #10
        { 0x48, 0xB7 }, // (Offset,Value)-Pair #11
		*/

		// IDirect3DDevice8_SetVertexShaderConstant1+0x06 : add eax, 0x1C
        { 0x06, 0x83 }, // (Offset,Value)-Pair #1
        { 0x07, 0xC0 }, // (Offset,Value)-Pair #2
        { 0x08, 0x1C }, // (Offset,Value)-Pair #3

		{ 0x16, 0xC7 },
		{ 0x17, 0x40 },
		{ 0x18, 0xE4 },
		{ 0x19, 0xA4 },
		{ 0x1A, 0x1E },
		{ 0x1B, 0x04 },
		{ 0x1C, 0x00 },

		// IDirect3DDevice8_SetVertexShaderConstant1+0x53 : retn
        { 0x53, 0xC3 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetVertexShaderConstant1Fast
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetVertexShaderConstant1Fast_1_0_5558, 11)

        // IDirect3DDevice8_SetVertexShaderConstant1Fast+0x05 : add eax, 0x1C
        { 0x05, 0x83 }, // (Offset,Value)-Pair #1
        { 0x06, 0xC0 }, // (Offset,Value)-Pair #2
        { 0x07, 0x1C }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetVertexShaderConstant1Fast+0x0E : jnb +0x2E
        { 0x0E, 0x73 }, // (Offset,Value)-Pair #4
        { 0x0F, 0x2E }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetVertexShaderConstant1Fast+0x28 : mov [eax-0x10], ecx
        { 0x28, 0x89 }, // (Offset,Value)-Pair #6
        { 0x29, 0x48 }, // (Offset,Value)-Pair #7
        { 0x2A, 0xF0 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetVertexShaderConstant1Fast+0x3D : retn
        { 0x3D, 0xC3 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetVertexShaderConstant1Fast+0x47 : jmp +0xB7
        { 0x47, 0xEB }, // (Offset,Value)-Pair #10
        { 0x48, 0xB7 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetViewport
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetViewport_1_0_5558, 8)

        { 0x1E, 0x86 },
        { 0x3E, 0x1B },
        { 0x5E, 0x8B },
        { 0x7E, 0x6D },
        { 0x9E, 0x81 },
        { 0xBE, 0x0C },
        { 0xDE, 0x75 },
        { 0xFE, 0x85 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_CreateVertexBuffer2
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_CreateVertexBuffer2_1_0_5558, 7)

        { 0x08, 0xE8 },
        { 0x12, 0x1E },
        { 0x1C, 0x50 },
        { 0x26, 0x68 },
        { 0x31, 0x33 },
        { 0x3A, 0xFF },
        { 0x44, 0x01 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_UpdateOverlay
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_UpdateOverlay_1_0_5558, 11)

        // IDirect3DDevice8_UpdateOverlay+0x13 : mov [eax+0x1C0C], ecx
        { 0x13, 0x89 }, // (Offset,Value)-Pair #1
        { 0x14, 0x88 }, // (Offset,Value)-Pair #2
        { 0x15, 0x0C }, // (Offset,Value)-Pair #3
        { 0x16, 0x1C }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_UpdateOverlay+0x85 : and eax, 0x0FFFFFFF
        { 0x85, 0x25 }, // (Offset,Value)-Pair #5
        { 0x86, 0xFF }, // (Offset,Value)-Pair #6
        { 0x89, 0x0F }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_UpdateOverlay+0x9D : mov [esi+0x8920], ecx
        { 0x9D, 0x89 }, // (Offset,Value)-Pair #8
        { 0x9E, 0x8E }, // (Offset,Value)-Pair #9
        { 0x9F, 0x20 }, // (Offset,Value)-Pair #10
        { 0xA0, 0x89 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_GetOverlayUpdateStatus
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_GetOverlayUpdateStatus_1_0_5558, 10)

        // IDirect3DDevice8_GetOverlayUpdateStatus+0x05 : mov ecx, [eax+0x24xx]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #1
        { 0x06, 0x88 }, // (Offset,Value)-Pair #2
     // { 0x07, 0x00 }, // The offset from EAX has been seen as 2400 (5558) and 2410 (5659)
        { 0x08, 0x24 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_GetOverlayUpdateStatus+0x0C : mov esi, [eax+0x1Dxx]
        { 0x0C, 0x8B }, // (Offset,Value)-Pair #4
        { 0x0D, 0xB0 }, // (Offset,Value)-Pair #5
     // { 0x0E, 0xD8 }, // // The offset from EAX has been seen as 0x1DD8 (5558) and 0x1DE8 (5659)
        { 0x0F, 0x1D }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_GetOverlayUpdateStatus+0x16 : setnz dl
        { 0x16, 0x0F }, // (Offset,Value)-Pair #7
        { 0x17, 0x95 }, // (Offset,Value)-Pair #8
        { 0x18, 0xC2 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_GetOverlayUpdateStatus+0x1C : retn
        { 0x1C, 0xC3 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_CreatePalette2
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_CreatePalette2_1_0_5558, 7)

        { 0x0D, 0x8B },
        { 0x16, 0x00 },
        { 0x23, 0x68 },
        { 0x2E, 0x85 },
        { 0x3D, 0x5E },
        { 0x46, 0x1E },
        { 0x52, 0x89 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_CreateTexture2
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_CreateTexture2_1_0_5558, 8)

        { 0x14, 0x4C },
        { 0x2A, 0x8B },
        { 0x42, 0x8B },
        { 0x56, 0x80 },
        { 0x70, 0x85 },
        { 0x82, 0x5E },
        { 0x98, 0x89 },
        { 0xAE, 0x1C },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_End
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_End_1_0_5558, 7)

        { 0x07, 0x8B },
        { 0x13, 0xC7 },
        { 0x19, 0xC7 },
        { 0x22, 0x08 },
        { 0x2B, 0xE1 },
        { 0x34, 0x4E },
        { 0x40, 0xC3 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetLight
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetLight_1_0_5558, 8)

        { 0x1E, 0x00 },
        { 0x3E, 0x89 },
        { 0x5E, 0x00 },
        { 0x7E, 0x07 },
        { 0x9E, 0xC8 },
        { 0xBE, 0xC9 },
        { 0xDE, 0x07 },
        { 0xFE, 0xC1 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetMaterial
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetMaterial_1_0_5558, 9)

        { 0x04, 0x08 },
        { 0x0C, 0x81 },
        { 0x0E, 0xF0 },
        { 0x10, 0x00 },
        { 0x16, 0x00 },
        { 0x1F, 0x81 },
        { 0x22, 0x90 },
        { 0x2C, 0x5E },
        { 0x2E, 0x04 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_LightEnable
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_LightEnable_1_0_5558, 8)

        { 0x1E, 0x8D },
        { 0x3E, 0xC7 },
        { 0x5E, 0xC7 },
        { 0x7E, 0x00 },
        { 0x9E, 0x85 },
        { 0xBE, 0x01 },
        { 0xDE, 0x00 },
        { 0xFE, 0x83 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetVertexShader
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetVertexShader_1_0_5558, 8)

        { 0x1E, 0x89 },
        { 0x3E, 0x5F },
        { 0x61, 0x3B },
        { 0x81, 0x00 },
        { 0x9E, 0x00 },
        { 0xBE, 0x8B },
        { 0xDE, 0x04 },
        { 0xFE, 0xC1 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetPalette
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetPalette_1_0_5558, 12)

        // IDirect3DDevice8_SetPalette+0x0D : mov eax, [edi+esi*4+0x0A88]
        { 0x0D, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0E, 0x84 }, // (Offset,Value)-Pair #2
        { 0x0F, 0xBE }, // (Offset,Value)-Pair #3
        { 0x10, 0x88 }, // (Offset,Value)-Pair #4
        { 0x11, 0x0F }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetPalette+0x76 : add esi, 0x41B20
        { 0x76, 0x81 }, // (Offset,Value)-Pair #6
        { 0x77, 0xC7 }, // (Offset,Value)-Pair #7
        { 0x78, 0x20 }, // (Offset,Value)-Pair #8
        { 0x79, 0x1B }, // (Offset,Value)-Pair #9
        { 0x7A, 0x04 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetPalette+0x8A : retn 0x08
        { 0x8A, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x8B, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_BlockUntilVerticalBlank
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_BlockUntilVerticalBlank_1_0_5558, 7)

        { 0x05, 0x6A },
        { 0x08, 0x00 },
        { 0x0D, 0xB0 },
        { 0x12, 0x00 },
        { 0x17, 0x05 },
        { 0x1C, 0x50 },
        { 0x23, 0xC3 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_DrawIndexedVerticesUP
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_DrawIndexedVerticesUP_1_0_5558, 10)

        // IDirect3DDevice8_DrawIndexedVerticesUP+0x4A : or edx, 0x800
        { 0x4A, 0x81 }, // (Offset,Value)-Pair #1
        { 0x4B, 0xCA }, // (Offset,Value)-Pair #2
        { 0x4C, 0x00 }, // (Offset,Value)-Pair #3
        { 0x4D, 0x08 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_DrawIndexedVerticesUP+0x5C : lea edx, [edi+0xBxx]
        { 0x5C, 0x8D }, // (Offset,Value)-Pair #5
        { 0x5D, 0x96 }, // (Offset,Value)-Pair #6
     // { 0x5E, 0xC4 }, // The offset from EDI has been seen as BC4 (5558) and BD4 (5659)
        { 0x5F, 0x0B }, // (Offset,Value)-Pair #7
        { 0x60, 0x00 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_DrawIndexedVerticesUP+0x6A : mov eax, 0x10
        { 0x6A, 0xB8 }, // (Offset,Value)-Pair #9
        { 0x6B, 0x10 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetTransform
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetTransform_1_0_5558, 8)

        { 0x1D, 0xB9 },
        { 0x3E, 0x0F },
        { 0x5E, 0xD9 },
        { 0x7A, 0x00 },
        { 0x99, 0x83 },
        { 0xB8, 0xE0 },
        { 0xD7, 0xD9 },
        { 0xF6, 0xF6 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_GetBackBuffer2
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_GetBackBuffer2_1_0_5558, 12)

        // IDirect3DDevice8_GetBackBuffer2+0x04 : cmp eax, 0xFFFFFFFF
        { 0x04, 0x83 }, // (Offset,Value)-Pair #1
        { 0x05, 0xF8 }, // (Offset,Value)-Pair #2
        { 0x06, 0xFF }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_GetBackBuffer2+0x0D : jnz +0x19
        { 0x0D, 0x75 }, // (Offset,Value)-Pair #4
        { 0x0E, 0x19 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_GetBackBuffer2+0x15 : mov esi, [ecx+eax*4+0x1A04]
        { 0x15, 0x8B }, // (Offset,Value)-Pair #6
        { 0x16, 0xB4 }, // (Offset,Value)-Pair #7
        { 0x17, 0x81 }, // (Offset,Value)-Pair #8
        { 0x18, 0x04 }, // (Offset,Value)-Pair #9
        { 0x19, 0x1A }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_GetBackBuffer2+0x40 : retn 0x04
        { 0x40, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x41, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_GetBackBuffer2B
// ******************************************************************
// NOTE: XIII has a different version than the others...
OOVPA_NO_XREF(IDirect3DDevice8_GetBackBuffer2B_1_0_5558, 12)

        // IDirect3DDevice8_GetBackBuffer2+0x04 : cmp eax, 0xFFFFFFFF
        { 0x04, 0x83 }, // (Offset,Value)-Pair #1
        { 0x05, 0xF8 }, // (Offset,Value)-Pair #2
        { 0x06, 0xFF }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_GetBackBuffer2+0x0D : jnz +0x19
        { 0x0D, 0x75 }, // (Offset,Value)-Pair #4
        { 0x0E, 0x19 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_GetBackBuffer2+0x15 : mov esi, [ecx+eax*4+0x1A14]
        { 0x15, 0x8B }, // (Offset,Value)-Pair #6
        { 0x16, 0xB4 }, // (Offset,Value)-Pair #7
        { 0x17, 0x81 }, // (Offset,Value)-Pair #8
        { 0x18, 0x14 }, // (Offset,Value)-Pair #9
        { 0x19, 0x1A }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_GetBackBuffer2+0x40 : retn 0x04
        { 0x40, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x41, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_GetBackBuffer2
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_GetBackBuffer2_1_0_5455, 12)

        // IDirect3DDevice8_GetBackBuffer2+0x04 : cmp eax, 0xFFFFFFFF
        { 0x04, 0x83 }, // (Offset,Value)-Pair #1
        { 0x05, 0xF8 }, // (Offset,Value)-Pair #2
        { 0x06, 0xFF }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_GetBackBuffer2+0x0D : jnz +0x19
        { 0x0D, 0x75 }, // (Offset,Value)-Pair #4
        { 0x0E, 0x19 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_GetBackBuffer2+0x15 : mov esi, [ecx+eax*4+0x15F4]
        { 0x15, 0x8B }, // (Offset,Value)-Pair #6
        { 0x16, 0xB4 }, // (Offset,Value)-Pair #7
        { 0x17, 0x81 }, // (Offset,Value)-Pair #8
        { 0x18, 0xF4 }, // (Offset,Value)-Pair #9
        { 0x19, 0x15 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_GetBackBuffer2+0x40 : retn 0x04
        { 0x40, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x41, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3D8_CheckDeviceMultiSampleType
// ******************************************************************
OOVPA_NO_XREF(IDirect3D8_CheckDeviceMultiSampleType_1_0_5558, 7)

        { 0x0E, 0x00 },
        { 0x1E, 0x54 },
        { 0x2E, 0xC9 },
        { 0x3E, 0x8B },
        { 0x4E, 0x08 },
        { 0x5E, 0x72 },
        { 0x6E, 0x03 },
OOVPA_END;

// ******************************************************************
// * IDirect3D8_SetPushBufferSize
// ******************************************************************
OOVPA_NO_XREF(IDirect3D8_SetPushBufferSize_1_0_5558, 7)

        { 0x01, 0x44 },
        { 0x04, 0x8B },
        { 0x07, 0x08 },
        { 0x0D, 0x89 },
        { 0x0E, 0x0D },
        { 0x13, 0xC2 },
        { 0x14, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3D::SetFence
// ******************************************************************
OOVPA_XREF(D3D_SetFence_1_0_5558, 14,

    XREF_D3D_SETFENCE,
    XRefZero)

        { 0x14, 0x8B },
        { 0x15, 0x7E },
        { 0x16, 0x2C },
        { 0x23, 0x8A },
        { 0x24, 0x54 },
        { 0x25, 0x24 },
        { 0x26, 0x0C },
        { 0x66, 0x89 },
        { 0x67, 0x44 },
        { 0x68, 0xCE },
        { 0x69, 0x64 },
        { 0x91, 0xC7 },
        { 0x92, 0x46 },
        { 0x93, 0x3C },
OOVPA_END;

// ******************************************************************
// * D3D::BlockOnTime
// ******************************************************************
OOVPA_XREF(D3D_BlockOnTime_1_0_5558, 6,

    XREF_D3D_BLOCKONTIME,
    XRefZero)

        { 0x09, 0x30 },
        { 0x27, 0x07 },
        { 0x7E, 0x25 },
        { 0x9F, 0x20 },
        { 0xD3, 0x56 },
        { 0xE9, 0x57 },
OOVPA_END;

// ******************************************************************
// * D3D::BlockOnResource
// ******************************************************************
OOVPA_XREF(D3D_BlockOnResource_1_0_5558, 16,

    XREF_D3D_BlockOnResource,
    XRefZero)

        { 0x09, 0x8B }, // mov eax, [esp+arg_0]
        { 0x0A, 0x44 },
        { 0x0B, 0x24 },
        { 0x0C, 0x04 },
        { 0x0F, 0x81 }, // and ecx, 0x00070000
        { 0x10, 0xE1 },
        { 0x11, 0x00 },
        { 0x12, 0x00 },
        { 0x13, 0x07 },
        { 0x14, 0x00 },
        { 0x46, 0x8B }, // mov esi, [eax+8]
        { 0x47, 0x70 },
        { 0x48, 0x08 },
        { 0x75, 0xC2 }, // retn 4
        { 0x76, 0x04 },
        { 0x77, 0x00 },

OOVPA_END;

// ******************************************************************
// * IDirect3DResource8_BlockUntilNotBusy
// ******************************************************************
OOVPA_XREF(IDirect3DResource8_BlockUntilNotBusy_1_0_5558, 2,

    XRefNoSaveIndex,
    XRefOne)

        { 0x01, XREF_D3D_BlockOnResource },

        { 0x00, 0xE9 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetScissors
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetScissors_1_0_5558, 8)

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
// * IDirect3DDevice8_SetScissors
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetScissors_1_0_5455, 8)

        { 0x1D, 0x44 },
        { 0x3C, 0x8B },
        { 0x5D, 0xD9 },
        { 0x7D, 0xD8 },
        { 0x99, 0x0A },
        { 0xB8, 0xE8 },
        { 0xD7, 0x24 },
        { 0xF6, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_AddRef
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_AddRef_1_0_5558, 10)

        // IDirect3DDevice8_AddRef+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_AddRef+0x05 : mov ecx, [eax+0x04FC]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x88 }, // (Offset,Value)-Pair #3
        { 0x07, 0x34 }, // (Offset,Value)-Pair #4
        { 0x08, 0x09 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_AddRef+0x0B : inc ecx
        { 0x0B, 0x41 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_AddRef+0x0C : mov [eax+0x04FC], ecx
        { 0x0C, 0x89 }, // (Offset,Value)-Pair #7
        { 0x0D, 0x88 }, // (Offset,Value)-Pair #8
        { 0x0E, 0x34 }, // (Offset,Value)-Pair #9
        { 0x0F, 0x09 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_CreatePixelShader
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_CreatePixelShader_1_0_5558, 11)

        // IDirect3DDevice8_CreatePixelShader+0x05 : push 0xFC
        { 0x05, 0x68 }, // (Offset,Value)-Pair #1
        { 0x06, 0xFC }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_CreatePixelShader+0x10 : mov eax, 0x8007000E
        { 0x13, 0xB8 }, // (Offset,Value)-Pair #3
        { 0x14, 0x0E }, // (Offset,Value)-Pair #4
        { 0x15, 0x00 }, // (Offset,Value)-Pair #5
        { 0x16, 0x07 }, // (Offset,Value)-Pair #6
        { 0x17, 0x80 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_CreatePixelShader+0x31 : mov ecx, 0x3C
        { 0x34, 0xB9 }, // (Offset,Value)-Pair #8
        { 0x35, 0x3C }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_CreatePixelShader+0x42 : retn 0x08
        { 0x45, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x46, 0x08 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_DrawVerticesUP
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_DrawVerticesUP_1_0_5558, 10)

        // IDirect3DDevice8_DrawVerticesUP+0x03 : sub esp, 0x14
        { 0x03, 0x83 }, // (Offset,Value)-Pair #1
        { 0x04, 0xEC }, // (Offset,Value)-Pair #2
        { 0x05, 0x14 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_DrawVerticesUP+0x37 : mov edx, [edi+0xC44]
        { 0x37, 0x8B }, // (Offset,Value)-Pair #4
        { 0x38, 0x97 }, // (Offset,Value)-Pair #5
        { 0x39, 0x44 }, // (Offset,Value)-Pair #6
        { 0x3A, 0x0C }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_DrawVerticesUP+0xC4 : cmp esi, 0x80
        { 0xC4, 0x81 }, // (Offset,Value)-Pair #8
        { 0xC5, 0xFE }, // (Offset,Value)-Pair #9
        { 0xC6, 0x80 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_CreateIndexBuffer
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_CreateIndexBuffer_1_0_5558, 10)

        { 0x00, 0x50 },
        { 0x06, 0x8B },
        { 0x07, 0x4C },
        { 0x09, 0x04 },
        { 0x0A, 0x33 },
        { 0x0E, 0x0F },
        { 0x12, 0x01 },
        { 0x16, 0x0E },
        { 0x1A, 0x8B },
        { 0x1E, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateIndexBuffer2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateIndexBuffer2_1_0_5558, 7)

        { 0x05, 0x00 },
        { 0x0C, 0x50 },
        { 0x13, 0xC0 },
        { 0x1A, 0xC9 },
        { 0x21, 0x04 },
        { 0x28, 0xC7 },
        { 0x2F, 0x48 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVerticalBlankCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVerticalBlankCallback_1_0_5558, 7)

        { 0x01, 0x44 },
        { 0x04, 0x8B },
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0xA8 },
        { 0x0D, 0x1D },
        { 0x10, 0xC2 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetVerticalBlankCallback
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetVerticalBlankCallback_1_0_5455, 12)

        // IDirect3DDevice8_SetVerticalBlankCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x04 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetVerticalBlankCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #5
        { 0x05, 0x0D }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetVerticalBlankCallback+0x0A : mov [ecx+0x1998], eax
        { 0x0A, 0x89 }, // (Offset,Value)-Pair #7
        { 0x0B, 0x81 }, // (Offset,Value)-Pair #8
        { 0x0C, 0x98 }, // (Offset,Value)-Pair #9
        { 0x0D, 0x19 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetVerticalBlankCallback+0x10 : retn 0x04
        { 0x10, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x11, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetSwapCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetSwapCallback_1_0_5558, 7)

        { 0x01, 0x44 },
        { 0x04, 0x8B },
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0xA4 },
        { 0x0D, 0x1D },
        { 0x10, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_TextureFactor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_TextureFactor_1_0_5558, 7)

        { 0x09, 0x80 },
        { 0x14, 0x46 },
        { 0x1F, 0x24 },
        { 0x2A, 0xBA },
        { 0x35, 0x4A },
        { 0x40, 0x5E },
        { 0x4D, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetViewport
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetViewport_1_0_5558, 7)

        { 0x05, 0x8B },
        { 0x0C, 0x00 },
        { 0x13, 0x50 },
        { 0x1A, 0x08 },
        { 0x21, 0x89 },
        { 0x28, 0x51 },
        { 0x2F, 0x14 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Reset
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Reset_1_0_5558, 8)

        { 0x13, 0x00 },
        { 0x28, 0x74 },
        { 0x40, 0x5F },
        { 0x55, 0x8B },
        { 0x67, 0x50 },
        { 0x7C, 0x00 },
        { 0x91, 0x3F },
        { 0xA6, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetGammaRamp
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetGammaRamp_1_0_5558, 7)

        { 0x0F, 0x68 },
        { 0x20, 0x34 },
        { 0x31, 0xB4 },
        { 0x42, 0x08 },
        { 0x53, 0xF3 },
        { 0x64, 0xCA },
        { 0x75, 0x07 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_GetProjectionViewportMatrix
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_GetProjectionViewportMatrix_1_0_5455, 7)

        { 0x02, 0x35 },
        { 0x07, 0x57 },
        { 0x0A, 0x24 },
        { 0x0E, 0x60 },
        { 0x12, 0xB9 },
        { 0x16, 0x00 },
        { 0x1A, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetModelView
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetModelView_1_0_5558, 7)

        { 0x05, 0x57 },
        { 0x0A, 0x85 },
        { 0x10, 0xB0 },
        { 0x16, 0x10 },
        { 0x1C, 0x5E },
        { 0x22, 0xC1 },
        { 0x28, 0xE0 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetBackMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetBackMaterial_1_0_5558, 9)

        { 0x04, 0x08 },
        { 0x0C, 0x81 },
        { 0x0E, 0x34 },
        { 0x10, 0x00 },
        { 0x16, 0x00 },
        { 0x1F, 0x81 },
        { 0x22, 0x90 },
        { 0x2C, 0x5E },
        { 0x2E, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_TwoSidedLighting
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_TwoSidedLighting_1_0_5558, 8)

        { 0x0E, 0x05 },
        { 0x1E, 0x89 },
        { 0x30, 0x00 },
        { 0x40, 0x89 },
        { 0x52, 0x8B },
        { 0x60, 0x75 },
        { 0x6E, 0x48 },
        { 0x7E, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BeginStateBlock
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginStateBlock_1_0_5558, 6)

        { 0x00, 0xA1 },
        { 0x05, 0x83 },
        { 0x06, 0x48 },
        { 0x07, 0x08 },
        { 0x08, 0x20 },
        { 0x09, 0xE9 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_EndStateBlock
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_EndStateBlock_1_0_5558, 6)

        { 0x00, 0xA1 },
        { 0x05, 0x83 },
        { 0x06, 0x60 },
        { 0x07, 0x08 },
        { 0x08, 0xDF },
        { 0x09, 0xE9 },
OOVPA_END;

// ******************************************************************
// * Get2DSurfaceDesc
// ******************************************************************
OOVPA_NO_XREF(Get2DSurfaceDesc_1_0_5558, 10)

        // Get2DSurfaceDesc+0x2B : movzx edx, byte ptr [edi+0x0D]
        { 0x2B, 0x0F }, // (Offset,Value)-Pair #1
        { 0x2C, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x2D, 0x57 }, // (Offset,Value)-Pair #3
        { 0x2E, 0x0D }, // (Offset,Value)-Pair #4

        // Get2DSurfaceDesc+0x52 : mov edx, [eax+0x1A04]
        { 0x52, 0x8B }, // (Offset,Value)-Pair #5
        { 0x53, 0x90 }, // (Offset,Value)-Pair #6
        { 0x54, 0x04 }, // (Offset,Value)-Pair #7
        { 0x55, 0x1A }, // (Offset,Value)-Pair #8

        // Get2DSurfaceDesc+0xAE : retn 0x0C
        { 0xAE, 0xC2 }, // (Offset,Value)-Pair #9
        { 0xAF, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3D::MakeRequestedSpace
// ******************************************************************
OOVPA_NO_XREF(D3D_MakeRequestedSpace_1_0_5558, 12)

		// D3D::MakeRequestedSpace+0x08: test byte ptr [esi+8], 4
		{ 0x08, 0xF6 },
		{ 0x09, 0x46 },
		{ 0x0A, 0x08 },
		{ 0x0B, 0x04 },
		// D3D::MakeRequestedSpace+0x32: retn 8
		{ 0x32, 0xC2 },
		{ 0x33, 0x08 },
		// D3D::MakeRequestedSpace+0x57: add ebp, 0x4000
		{ 0x57, 0x81 },
		{ 0x58, 0xC5 },
		{ 0x59, 0x00 },
		{ 0x5A, 0x40 },
		{ 0x5B, 0x00 },
		{ 0x5C, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_MakeSpace
// ******************************************************************
// TODO: This may or may not need to save/use an XRef to work...
OOVPA_NO_XREF(D3DDevice_MakeSpace_1_0_5558, 7)

        { 0x00, 0xA1 },
        { 0x05, 0x50 },
        { 0x06, 0xD1 },
        { 0x07, 0xE8 },
        { 0x08, 0x50 },
        { 0x09, 0xE8 },
        { 0x0E, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderTarget@8
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderTarget_1_0_5558, 8)

        { 0x1E, 0x44 },
        { 0x3E, 0x2C },
        { 0x5E, 0x8B },
        { 0x7E, 0xDE },
        { 0xA0, 0x89 },
        { 0xBE, 0x09 },
        { 0xDE, 0x8B },
        { 0xFE, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3DTexture_LockRect
// ******************************************************************
OOVPA_NO_XREF(D3DTexture_LockRect_1_0_5558, 8)

        { 0x03, 0x14 },
        { 0x08, 0x8B },
        { 0x0D, 0x8B },
        { 0x12, 0x8B },
        { 0x17, 0x50 },
		{ 0x1B, 0xE8 },
        { 0x20, 0xC2 },
        { 0x21, 0x14 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPixelShaderConstant
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPixelShaderConstant_1_0_5558, 7)

        { 0x1E, 0x85 },
        { 0x3E, 0x8B },
        { 0x5E, 0xC0 },
        { 0x7E, 0x0F },
        { 0x9F, 0x8B },
        { 0xBE, 0x00 },
        { 0xDE, 0xC0 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetFlickerFilter
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetFlickerFilter_1_0_5455, 12)

        // IDirect3DDevice8_SetFlickerFilter+0x1C : mov eax, [eax+0x1808]
        { 0x1C, 0x8B }, // (Offset,Value)-Pair #1
        { 0x1D, 0x80 }, // (Offset,Value)-Pair #2
        { 0x1E, 0x08 }, // (Offset,Value)-Pair #3
        { 0x1F, 0x18 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetFlickerFilter+0x22 : push 0; push esi; push 0x0B; push eax
        { 0x22, 0x6A }, // (Offset,Value)-Pair #5
        { 0x23, 0x00 }, // (Offset,Value)-Pair #6
        { 0x24, 0x56 }, // (Offset,Value)-Pair #7
        { 0x25, 0x6A }, // (Offset,Value)-Pair #8
        { 0x26, 0x0B }, // (Offset,Value)-Pair #9
        { 0x27, 0x50 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetFlickerFilter+0x3F : retn 0x04
        { 0x3F, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x40, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_BlockUntilVerticalBlank
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_BlockUntilVerticalBlank_1_0_5455, 11)

        // IDirect3DDevice8_BlockUntilVerticalBlank+0x05 : push 0; push 0; push 1
        { 0x05, 0x6A }, // (Offset,Value)-Pair #1
        { 0x06, 0x00 }, // (Offset,Value)-Pair #2
        { 0x07, 0x6A }, // (Offset,Value)-Pair #3
        { 0x08, 0x00 }, // (Offset,Value)-Pair #4
        { 0x09, 0x6A }, // (Offset,Value)-Pair #5
        { 0x0A, 0x01 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_BlockUntilVerticalBlank+0x17 : add eax, 0x199C
        { 0x17, 0x05 }, // (Offset,Value)-Pair #7
        { 0x18, 0x9C }, // (Offset,Value)-Pair #8
        { 0x19, 0x19 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_BlockUntilVerticalBlank+0x1D : call [KrnlImport]
        { 0x1D, 0xFF }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_BlockUntilVerticalBlank+0x23 : retn
        { 0x23, 0xC3 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetScreenSpaceOffset
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetScreenSpaceOffset_1_0_5455, 8)

        // IDirect3DDevice8_SetScreenSpaceOffset+0x13 : fstp [esi+0x0AD8]
        { 0x13, 0xD9 }, // (Offset,Value)-Pair #1
        { 0x14, 0x9E }, // (Offset,Value)-Pair #2
        { 0x15, 0xD8 }, // (Offset,Value)-Pair #3
        { 0x16, 0x0A }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetScreenSpaceOffset+0x33 : jb +0x05
        { 0x33, 0x72 }, // (Offset,Value)-Pair #5
        { 0x34, 0x05 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetScreenSpaceOffset+0x46 : retn 0x08
        { 0x46, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x47, 0x08 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetScreenSpaceOffset
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetScreenSpaceOffset_1_0_5558, 8)

        // IDirect3DDevice8_SetScreenSpaceOffset+0x13 : fstp [esi+0x0EE8]
        { 0x13, 0xD9 }, // (Offset,Value)-Pair #1
        { 0x14, 0x9E }, // (Offset,Value)-Pair #2
        { 0x15, 0xE8 }, // (Offset,Value)-Pair #3
        { 0x16, 0x0E }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetScreenSpaceOffset+0x33 : jb +0x05
        { 0x33, 0x72 }, // (Offset,Value)-Pair #5
        { 0x34, 0x05 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetScreenSpaceOffset+0x46 : retn 0x08
        { 0x46, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x47, 0x08 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * D3DDevice_InsertCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_InsertCallback_1_0_5558, 7)

        { 0x0C, 0x72 },
        { 0x1A, 0x10 },
        { 0x28, 0x85 },
        { 0x36, 0xC7 },
        { 0x44, 0x0C },
        { 0x52, 0x00 },
        { 0x60, 0x00 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_LineWidth
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetRenderState_LineWidth_1_0_5455, 10)

        // IDirect3DDevice8_SetRenderState_LineWidth+0x15 : fmul dword ptr [esi+0x54C]
        { 0x15, 0xD8 }, // (Offset,Value)-Pair #1
        { 0x16, 0x8E }, // (Offset,Value)-Pair #2
        { 0x17, 0x4C }, // (Offset,Value)-Pair #3
        { 0x18, 0x05 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_LineWidth+0x32 : cmp edi, 0x1FF
        { 0x32, 0x81 }, // (Offset,Value)-Pair #5
        { 0x33, 0xFF }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_LineWidth+0x3A : mov edi, 0x1FF
        { 0x3A, 0xBF }, // (Offset,Value)-Pair #7
        { 0x3B, 0xFF }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetRenderState_LineWidth+0x62 : retn 0x04
        { 0x62, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x63, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_FlushVertexCache
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_FlushVertexCache_1_0_5558, 10)

        { 0x00, 0x56 },
        { 0x07, 0x8B },
        { 0x08, 0x06 },
        { 0x0D, 0x05 },
        { 0x13, 0xC7 },
        { 0x15, 0x10 },
        { 0x17, 0x04 },
        { 0x1C, 0x00 },
        { 0x21, 0xC0 },
        { 0x26, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_LogicOp
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_LogicOp_1_0_5558, 7)

        { 0x09, 0x3B },
        { 0x14, 0x4C },
        { 0x1F, 0x04 },
        { 0x2A, 0x0D },
        { 0x35, 0xBC },
        { 0x40, 0x89 },
        { 0x4E, 0x5E },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_LightEnable
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_LightEnable_1_0_5455, 13)

        // IDirect3DDevice8_LightEnable+0x0F : cmp ebx, [esi+0x0390]
        { 0x0F, 0x3B }, // (Offset,Value)-Pair #1
        { 0x10, 0x9E }, // (Offset,Value)-Pair #2
        { 0x11, 0x90 }, // (Offset,Value)-Pair #3
        { 0x12, 0x03 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_LightEnable+0x6B : test eax, eax
        { 0x6B, 0x85 }, // (Offset,Value)-Pair #5
        { 0x6C, 0xC0 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_LightEnable+0x9A : cmp ecx, 0xFFFFFFFF
        { 0x9A, 0x83 }, // (Offset,Value)-Pair #7
        { 0x9B, 0xF9 }, // (Offset,Value)-Pair #8
        { 0x9C, 0xFF }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_LightEnable+0xB9 : jnz +0xF7
        { 0xB9, 0x75 }, // (Offset,Value)-Pair #10
        { 0xBA, 0xF7 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_LightEnable+0xFB : retn 0x08
        { 0xFB, 0xC2 }, // (Offset,Value)-Pair #12
        { 0xFC, 0x08 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetModelView
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetModelView_1_0_5558, 7)

        { 0x15, 0xFF },
        { 0x2D, 0x81 },
        { 0x43, 0x3B },
        { 0x5A, 0xC1 },
        { 0x71, 0x53 },
        { 0x8A, 0x8B },
        { 0x9F, 0x30 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetIndices
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetIndices_1_0_5558, 7)

        { 0x0F, 0x10 },
        { 0x20, 0xC7 },
        { 0x31, 0xC0 },
        { 0x42, 0x78 },
        { 0x53, 0xBE },
        { 0x64, 0x89 },
        { 0x75, 0x10 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetIndices
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetIndices_1_0_5455, 7)

        { 0x0F, 0xBE },
        { 0x20, 0xBE },
        { 0x31, 0xFF },
        { 0x42, 0x14 },
        { 0x53, 0x00 },
        { 0x64, 0x06 },
        { 0x75, 0xDD },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetBackBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetBackBuffer_1_0_5558, 6)

        { 0x01, 0x44 },
        { 0x04, 0x50 },
        { 0x0A, 0x8B },
        { 0x0B, 0x4C },
        { 0x0D, 0x0C },
        { 0x10, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_PersistDisplay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_PersistDisplay_1_0_5558, 7)

        { 0x1E, 0x15 },
        { 0x3E, 0x89 },
        { 0x5E, 0x89 },
        { 0x7E, 0x42 },
        { 0x9E, 0x50 },
        { 0xBE, 0x00 },
        { 0xDE, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3DCubeTexture_GetCubeMapSurface
// ******************************************************************
OOVPA_NO_XREF(D3DCubeTexture_GetCubeMapSurface_1_0_5558, 7)

        { 0x04, 0x8B },
        { 0x0A, 0x24 },
        { 0x14, 0x8B },
        { 0x16, 0x24 },
        { 0x1C, 0x0F },
        { 0x22, 0x81 },
        { 0x28, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexDataColor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexDataColor_1_0_5558, 7)

        { 0x08, 0x06 },
        { 0x14, 0x8B },
        { 0x1C, 0x19 },
        { 0x26, 0xB6 },
        { 0x30, 0x00 },
        { 0x3A, 0xFF },
        { 0x44, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData4f
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData4f_1_0_5558, 7)

        { 0x0A, 0x46 },
        { 0x16, 0x08 },
        { 0x22, 0x09 },
        { 0x2E, 0x24 },
        { 0x3A, 0x24 },
        { 0x46, 0x8B },
        { 0x52, 0x14 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData2f
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData2f_1_0_5558, 8)

        { 0x07, 0x8B },
        { 0x0C, 0x72 },
        { 0x13, 0x8B },
        { 0x1A, 0x80 },
        { 0x21, 0x0C },
        { 0x28, 0x89 },
        { 0x2F, 0xC0 },
        { 0x36, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_LoadVertexShaderProgram
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_LoadVertexShaderProgram_1_0_5558, 7)

        { 0x0B, 0x2D },
        { 0x18, 0x10 },
        { 0x25, 0xE0 },
        { 0x32, 0x5F },
        { 0x3F, 0x24 },
        { 0x4C, 0x83 },
        { 0x5C, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPixelShaderProgram
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPixelShaderProgram_1_0_5558, 7)

        { 0x06, 0xA1 },
        { 0x0E, 0x88 },
        { 0x16, 0x00 },
        { 0x1E, 0x00 },
        { 0x26, 0x09 },
        { 0x32, 0xC7 },
        { 0x36, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleMode_1_0_5558, 9)

        { 0x03, 0x04 },
        { 0x0A, 0xA3 },
        { 0x0F, 0x8B },
        { 0x12, 0x19 },
        { 0x17, 0x04 },
        { 0x1B, 0x75 },
        { 0x1C, 0x07 },
        { 0x24, 0xC2 },
        { 0x26, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleMode_1_0_5455, 9)

        { 0x03, 0x04 },
        { 0x0A, 0xA3 },
        { 0x0F, 0x8B },
        { 0x12, 0x15 },
        { 0x17, 0xF4 },
        { 0x1B, 0x75 },
        { 0x1C, 0x07 },
        { 0x24, 0xC2 },
        { 0x26, 0x00 },
OOVPA_END;


// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleRenderTargetMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleRenderTargetMode_1_0_5455, 9)

        { 0x03, 0x04 },
        { 0x0A, 0xA3 },
        { 0x0F, 0x8B },
        { 0x12, 0x15 },
        { 0x17, 0xF4 },
        { 0x1B, 0x74 },
        { 0x1C, 0x07 },
        { 0x24, 0xC2 },
        { 0x26, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleRenderTargetMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleRenderTargetMode_1_0_5558, 9)

        { 0x03, 0x04 },
        { 0x0A, 0xA3 },
        { 0x0F, 0x8B },
        { 0x12, 0x19 },
        { 0x17, 0x04 },
        { 0x1B, 0x74 },
        { 0x1C, 0x07 },
        { 0x24, 0xC2 },
        { 0x26, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_Dxt1NoiseEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_Dxt1NoiseEnable_1_0_5558, 8)

        { 0x0D, 0x8B },
        { 0x1D, 0x80 },
        { 0x2B, 0x08 },
        { 0x3A, 0x01 },
        { 0x49, 0xC7 },
        { 0x58, 0x05 },
        { 0x67, 0xC0 },
        { 0x76, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_IsFencePending
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_IsFencePending_1_0_5558, 7)

        { 0x05, 0x8B },
        { 0x06, 0x48 },
        { 0x0A, 0x2C },
        { 0x0E, 0xD0 },
        { 0x12, 0x44 },
        { 0x16, 0xC2 },
        { 0x1A, 0xD8 },
OOVPA_END;

// ******************************************************************
// * D3D8_1_0_5558
// ******************************************************************
OOVPATable D3D8_1_0_5558[] = {

    // IDirect3D8::CreateDevice (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirect3D8_CreateDevice_1_0_4627, XTL::EmuIDirect3D8_CreateDevice),
	// IDirect3DDevice8::GetDisplayFieldStatus (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetDisplayFieldStatus_1_0_5233, XTL::EmuIDirect3DDevice8_GetDisplayFieldStatus),
    // D3D::CDevice::LazySetStateVB (XREF)
	OOVPA_TABLE_XREF(D3D_CDevice_LazySetStateVB_1_0_5558),
	// IDirect3DDevice8::BeginPush
	OOVPA_TABLE_PATCH(IDirect3DDevice8_BeginPush_1_0_5558, XTL::EmuIDirect3DDevice8_BeginPush),
	// IDirect3DDevice8::EndPush (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_EndPush_1_0_4627, XTL::EmuIDirect3DDevice8_EndPush),
    // D3DDevice_RunVertexStateShader (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(X_D3DDevice_RunVertexStateShader_1_0_4627, XTL::EmuIDirect3DDevice8_RunVertexStateShader),
    // D3DDevice_GetVertexShaderSize (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(X_D3DDevice_GetVertexShaderSize_1_0_3925, XTL::EmuIDirect3DDevice8_GetVertexShaderSize),
	// IDirect3DDevice8::BeginStateBlock
	OOVPA_TABLE_PATCH(IDirect3DDevice8_BeginStateBlock_1_0_4361, XTL::EmuIDirect3DDevice8_BeginStateBlock),
	// IDirect3DDevice8::EndStateBlock
	OOVPA_TABLE_PATCH(IDirect3DDevice8_EndStateBlock_1_0_4361, XTL::EmuIDirect3DDevice8_EndStateBlock),
	// IDirect3DDevice8::LoadVertexShader (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_LoadVertexShader_1_0_5233, XTL::EmuIDirect3DDevice8_LoadVertexShader),
	// IDirect3DDevice8::SelectVertexShader
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SelectVertexShader_1_0_5558, XTL::EmuIDirect3DDevice8_SelectVertexShader),
	// IDirect3DDevice8::CreateVertexShader
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateVertexShader_1_0_5558, XTL::EmuIDirect3DDevice8_CreateVertexShader),
	// IDirect3DDevice8::SetVertexShaderConstant1
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVertexShaderConstant1_1_0_5558, XTL::EmuIDirect3DDevice8_SetVertexShaderConstant1),
	// IDirect3DDevice8::SetVertexShaderConstant1Fast
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVertexShaderConstant1Fast_1_0_5558, XTL::EmuIDirect3DDevice8_SetVertexShaderConstant1),
	// IDirect3DDevice8::SetVertexShaderConstantNotInline (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVertexShaderConstantNotInline_1_0_5233, XTL::EmuIDirect3DDevice8_SetVertexShaderConstantNotInline),
	// IDirect3DDevice8::CreateVertexBuffer2
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateVertexBuffer2_1_0_5558, XTL::EmuIDirect3DDevice8_CreateVertexBuffer2),
	// IDirect3DDevice8::UpdateOverlay
	OOVPA_TABLE_PATCH(IDirect3DDevice8_UpdateOverlay_1_0_5558, XTL::EmuIDirect3DDevice8_UpdateOverlay),
	// IDirect3DDevice8::GetOverlayUpdateStatus
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetOverlayUpdateStatus_1_0_5558, XTL::EmuIDirect3DDevice8_GetOverlayUpdateStatus),
	// IDirect3DDevice8::Clear (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_Clear_1_0_5233, XTL::EmuIDirect3DDevice8_Clear),
	// IDirect3DDevice8::CreatePalette2
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreatePalette2_1_0_5558, XTL::EmuIDirect3DDevice8_CreatePalette2),
	// IDirect3DDevice8::SetPalette
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetPalette_1_0_5558, XTL::EmuIDirect3DDevice8_SetPalette),
	// IDirect3DDevice8::Swap (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_Swap_1_0_5233, XTL::EmuIDirect3DDevice8_Swap),
	// IDirect3DDevice8::BlockUntilVerticalBlank
	OOVPA_TABLE_PATCH(IDirect3DDevice8_BlockUntilVerticalBlank_1_0_5558, XTL::EmuIDirect3DDevice8_BlockUntilVerticalBlank),
	// IDirect3DDevice8::DrawIndexedVertices (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_DrawIndexedVertices_1_0_5233, XTL::EmuIDirect3DDevice8_DrawIndexedVertices),
	// IDirect3DDevice8::DrawIndexedVerticesUP
	OOVPA_TABLE_PATCH(IDirect3DDevice8_DrawIndexedVerticesUP_1_0_5558, XTL::EmuIDirect3DDevice8_DrawIndexedVerticesUP),
	// IDirect3DDevice8::SetPixelShader (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetPixelShader_1_0_5233, XTL::EmuIDirect3DDevice8_SetPixelShader),
	// IDirect3DDevice8::CreateTexture2
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateTexture2_1_0_5558, XTL::EmuIDirect3DDevice8_CreateTexture2),
	// IDirect3DDevice8::GetDisplayMode (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetDisplayMode_1_0_4627, XTL::EmuIDirect3DDevice8_GetDisplayMode),
	// IDirect3DDevice8::SetVertexData2f (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVertexData2f_1_0_4627, XTL::EmuIDirect3DDevice8_SetVertexData2f),
	// IDirect3DDevice8::End
	OOVPA_TABLE_PATCH(IDirect3DDevice8_End_1_0_5558, XTL::EmuIDirect3DDevice8_End),
	// IDirect3DDevice8::SetTextureState_TexCoordIndex (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_4627, XTL::EmuIDirect3DDevice8_SetTextureState_TexCoordIndex),
	// IDirect3DDevice8::SetTextureState_BumpEnv (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTextureState_BumpEnv_1_0_4361, XTL::EmuIDirect3DDevice8_SetTextureState_BumpEnv),
	// IDirect3DDevice8::SetTextureState_BorderColor (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTextureState_BorderColor_1_0_4361, XTL::EmuIDirect3DDevice8_SetTextureState_BorderColor),
	// IDirect3DDevice8::SetRenderState_CullMode (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_CullMode_1_0_5233, XTL::EmuIDirect3DDevice8_SetRenderState_CullMode),
	// IDirect3DDevice8::SetRenderState_NormalizeNormals (* unchanged since 4432 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_NormalizeNormals_1_0_4432, XTL::EmuIDirect3DDevice8_SetRenderState_NormalizeNormals),
	// IDirect3DDevice8::SetRenderState_ZBias (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_ZBias_1_0_3925, XTL::EmuIDirect3DDevice8_SetRenderState_ZBias),
	// IDirect3DDevice8::SetRenderState_EdgeAntiAlias (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_EdgeAntiAlias_1_0_4361, XTL::EmuIDirect3DDevice8_SetRenderState_EdgeAntiAlias),
	// IDirect3DDevice8::SetRenderState_FillMode (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_FillMode_1_0_4361, XTL::EmuIDirect3DDevice8_SetRenderState_FillMode),
	// IDirect3DDevice8::SetRenderState_FogColor (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_FogColor_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_FogColor),
	// IDirect3DDevice8::SetRenderState_Simple (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_Simple_1_0_4361, XTL::EmuIDirect3DDevice8_SetRenderState_Simple),
	// IDirect3DDevice8::SetRenderState_ZEnable (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_ZEnable_1_0_5233, XTL::EmuIDirect3DDevice8_SetRenderState_ZEnable),
	// IDirect3DDevice8::SetRenderState_StencilEnable (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_StencilEnable_1_0_4361, XTL::EmuIDirect3DDevice8_SetRenderState_StencilEnable),
	// IDirect3DDevice8::SetRenderState_StencilFail (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_StencilFail_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_StencilFail),
	// IDirect3DDevice8::SetLight
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetLight_1_0_5558, XTL::EmuIDirect3DDevice8_SetLight),
	// IDirect3DDevice8::SetMaterial
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetMaterial_1_0_5558, XTL::EmuIDirect3DDevice8_SetMaterial),
	// IDirect3DDevice8::LightEnable
	OOVPA_TABLE_PATCH(IDirect3DDevice8_LightEnable_1_0_5558, XTL::EmuIDirect3DDevice8_LightEnable),
	// IDirect3DVertexBuffer8::Lock2 (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirect3DVertexBuffer8_Lock2_1_0_4627, XTL::EmuIDirect3DVertexBuffer8_Lock2),
	// IDirect3DDevice8::SetRenderState_OcclusionCullEnable (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_OcclusionCullEnable_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_OcclusionCullEnable),
	// IDirect3DDevice8::SetRenderState_MultiSampleAntiAlias (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias_1_0_4627, XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias),
	// IDirect3DDevice8::SetRenderState_ShadowFunc (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_ShadowFunc_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_ShadowFunc),
	// IDirect3DDevice8::SetRenderState_YuvEnable (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_YuvEnable_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_YuvEnable),
	// IDirect3DDevice8::SetVertexShader
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVertexShader_1_0_5558, XTL::EmuIDirect3DDevice8_SetVertexShader),
	// IDirect3DDevice8::SetIndices
	OOVPA_TABLE_PATCH(D3DDevice_SetIndices_1_0_5558, XTL::EmuIDirect3DDevice8_SetIndices),
	 // IDirect3DDevice8::SetIndices
	OOVPA_TABLE_PATCH(D3DDevice_SetIndices_1_0_5455, XTL::EmuIDirect3DDevice8_SetIndices),
	// IDirect3DDevice8::SetTexture (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTexture_1_0_5233, XTL::EmuIDirect3DDevice8_SetTexture),
	// IDirect3DDevice8::SwitchTexture (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SwitchTexture_1_0_4361, XTL::EmuIDirect3DDevice8_SwitchTexture),
	// IDirect3DDevice8::Begin (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_Begin_1_0_5233, XTL::EmuIDirect3DDevice8_Begin),
	// IDirect3DResource8::IsBusy (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DResource8_IsBusy_1_0_4361, XTL::EmuIDirect3DResource8_IsBusy),
	// IDirect3DDevice8::BeginVisibilityTest (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_BeginVisibilityTest_1_0_4627, XTL::EmuIDirect3DDevice8_BeginVisibilityTest),
	// IDirect3DDevice8::SetTransform
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTransform_1_0_5558, XTL::EmuIDirect3DDevice8_SetTransform),
	// IDirect3DDevice8::SetStreamSource (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetStreamSource_1_0_4361, XTL::EmuIDirect3DDevice8_SetStreamSource),
	// IDirect3DDevice8::SetViewport
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetViewport_1_0_5558, XTL::EmuIDirect3DDevice8_SetViewport),
	// IDirect3DDevice8::GetBackBuffer2
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetBackBuffer2_1_0_5558, XTL::EmuIDirect3DDevice8_GetBackBuffer2),
	// IDirect3DDevice8::GetBackBuffer2B
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetBackBuffer2B_1_0_5558, XTL::EmuIDirect3DDevice8_GetBackBuffer2),
	// IDirect3DDevice8::GetBackBuffer2
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetBackBuffer2_1_0_5455, XTL::EmuIDirect3DDevice8_GetBackBuffer2),
	// IDirect3DDevice8::SetShaderConstantMode (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetShaderConstantMode_1_0_5233, XTL::EmuIDirect3DDevice8_SetShaderConstantMode),
	// IDirect3DResource8::Register (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DResource8_Register_1_0_3925, XTL::EmuIDirect3DResource8_Register),
	// IDirect3DResource8::AddRef (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DResource8_AddRef_1_0_3925, XTL::EmuIDirect3DResource8_AddRef),
	// IDirect3DResource8::Release (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DResource8_Release_1_0_3925, XTL::EmuIDirect3DResource8_Release),
	// IDirect3DSurface8::GetDesc (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DSurface8_GetDesc_1_0_4361, XTL::EmuIDirect3DSurface8_GetDesc),
	// IDirect3DPalette8::Lock2 (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(IDirect3DPalette8_Lock2_1_0_5233, XTL::EmuIDirect3DPalette8_Lock2),
	// IDirect3DTexture8::LockRect
	OOVPA_TABLE_PATCH(D3DTexture_LockRect_1_0_5558, XTL::EmuIDirect3DTexture8_LockRect),
	// IDirect3DSurface8::LockRect (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DSurface8_LockRect_1_0_3925, XTL::EmuIDirect3DSurface8_LockRect),
	// IDirect3DBaseTexture8::GetLevelCount (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DBaseTexture8_GetLevelCount_1_0_4361, XTL::EmuIDirect3DBaseTexture8_GetLevelCount),
	// IDirect3DTexture8::GetSurfaceLevel2 (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirect3DTexture8_GetSurfaceLevel2_1_0_4627, XTL::EmuIDirect3DTexture8_GetSurfaceLevel2),
	// IDirect3DDevice8::GetRenderTarget2 (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetRenderTarget2_1_0_5233, XTL::EmuIDirect3DDevice8_GetRenderTarget2),
	// IDirect3D8::CheckDeviceMultiSampleType
	OOVPA_TABLE_PATCH(IDirect3D8_CheckDeviceMultiSampleType_1_0_5558, XTL::EmuIDirect3D8_CheckDeviceMultiSampleType),
	// IDirect3D8::GetDeviceCaps (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(IDirect3D8_GetDeviceCaps_1_0_5233, XTL::EmuIDirect3D8_GetDeviceCaps),
	// IDirect3D8::SetPushBufferSize
	OOVPA_TABLE_PATCH(IDirect3D8_SetPushBufferSize_1_0_5558, XTL::EmuIDirect3D8_SetPushBufferSize),
	// IDirect3DDevice8::DrawVertices (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_DrawVertices_1_0_5233, XTL::EmuIDirect3DDevice8_DrawVertices),
    // D3D::SetFence (XREF)
	OOVPA_TABLE_XREF(D3D_SetFence_1_0_5558),
	// IDirect3DDevice8::InsertFence (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_InsertFence_1_0_5233, XTL::EmuIDirect3DDevice8_InsertFence),
    // D3D::BlockOnTime (XREF)
	OOVPA_TABLE_XREF(D3D_BlockOnTime_1_0_5558),
	// IDirect3DDevice8::BlockOnFence (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_BlockOnFence_1_0_5233, XTL::EmuIDirect3DDevice8_BlockOnFence),
    // D3D::BlockOnResource (XREF)
	OOVPA_TABLE_XREF(D3D_BlockOnResource_1_0_5558),
	// IDirect3DResource8::BlockUntilNotBusy
	OOVPA_TABLE_PATCH(IDirect3DResource8_BlockUntilNotBusy_1_0_5558, XTL::EmuIDirect3DResource8_BlockUntilNotBusy),
	// IDirect3DVertexBuffer8::GetDesc (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(IDirect3DVertexBuffer8_GetDesc_1_0_5233, XTL::EmuIDirect3DVertexBuffer8_GetDesc),
	// IDirect3DDevice8::CopyRects (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CopyRects_1_0_5233, XTL::EmuIDirect3DDevice8_CopyRects),
	// IDirect3DDevice8::SetScissors
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetScissors_1_0_5558, XTL::EmuIDirect3DDevice8_SetScissors),
	// IDirect3DDevice8::SetScissors
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetScissors_1_0_5455, XTL::EmuIDirect3DDevice8_SetScissors),
	// IDirect3DDevice8::GetDepthStencilSurface2 (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetDepthStencilSurface2_1_0_5233, XTL::EmuIDirect3DDevice8_GetDepthStencilSurface2),
	// IDirect3DDevice8::AddRef
	OOVPA_TABLE_PATCH(IDirect3DDevice8_AddRef_1_0_5558, XTL::EmuIDirect3DDevice8_AddRef),
	// IDirect3DDevice8::CreatePixelShader
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreatePixelShader_1_0_5558, XTL::EmuIDirect3DDevice8_CreatePixelShader),
	// IDirect3DDevice8::SetVertexShaderConstant4
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVertexShaderConstant4_1_0_4627, XTL::EmuIDirect3DDevice8_SetVertexShaderConstant4),
	// IDirect3DDevice8::DrawVerticesUP
	OOVPA_TABLE_PATCH(IDirect3DDevice8_DrawVerticesUP_1_0_5558, XTL::EmuIDirect3DDevice8_DrawVerticesUP),
	// IDirect3DDevice8::GetTransform (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetTransform_1_0_4361, XTL::EmuIDirect3DDevice8_GetTransform),
	// IDirect3DDevice8::CreateIndexBuffer2
	OOVPA_TABLE_PATCH(D3DDevice_CreateIndexBuffer2_1_0_5558, XTL::EmuIDirect3DDevice8_CreateIndexBuffer2),
	// IDirect3DDevice8::CreateIndexBuffer
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateIndexBuffer_1_0_5558, XTL::EmuIDirect3DDevice8_CreateIndexBuffer),
	// D3DDevice_SetVerticalBlankCallback
	OOVPA_TABLE_PATCH(D3DDevice_SetVerticalBlankCallback_1_0_5558, XTL::EmuIDirect3DDevice8_SetVerticalBlankCallback),
	// D3DDevice_SetVerticalBlankCallback
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVerticalBlankCallback_1_0_5455, XTL::EmuIDirect3DDevice8_SetVerticalBlankCallback),
	// D3DDevice_SetSwapCallback
	OOVPA_TABLE_PATCH(D3DDevice_SetSwapCallback_1_0_5558, XTL::EmuIDirect3DDevice8_SetSwapCallback),
	// IDirect3DDevice8::GetGammaRamp (* unchanged since 4034 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetGammaRamp_1_0_4034, XTL::EmuIDirect3DDevice8_GetGammaRamp),
	// IDirect3DDevice8::SetRenderState_TextureFactor
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_TextureFactor_1_0_5558, XTL::EmuIDirect3DDevice8_SetRenderState_TextureFactor),
	// IDirect3DDevice8::SetRenderState_VertexBlend (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_VertexBlend_1_0_4361, XTL::EmuIDirect3DDevice8_SetRenderState_VertexBlend),
	// IDirect3D8::KickOffAndWaitForIdle (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(IDirect3D8_KickOffAndWaitForIdle_1_0_5233, XTL::EmuIDirect3D8_KickOffAndWaitForIdle),
	// IDirect3DDevice8::Reset (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_Reset_1_0_4134, XTL::EmuIDirect3DDevice8_Reset),
	// D3DDevice_SetRenderState_SampleAlpha (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_SampleAlpha_1_0_5233, XTL::EmuIDirect3DDevice8_SetRenderState_SampleAlpha),
	// IDirect3DDevice8::SetGammaRamp (* unchanged since 4928 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetGammaRamp_1_0_4928, XTL::EmuIDirect3DDevice8_SetGammaRamp),
	// IDirect3DDevice8::GetViewport
	OOVPA_TABLE_PATCH(D3DDevice_GetViewport_1_0_5558, XTL::EmuIDirect3DDevice8_GetViewport),
	// IDirect3DDevice8_GetProjectionViewportMatrix
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetProjectionViewportMatrix_1_0_5455, XTL::EmuIDirect3DDevice8_GetProjectionViewportMatrix),
	// IDirect3DDevice8::GetModelView
	OOVPA_TABLE_PATCH(D3DDevice_GetModelView_1_0_5558, XTL::EmuIDirect3DDevice8_GetModelView),
	// IDirect3DDevice8::SetBackMaterial
	OOVPA_TABLE_PATCH(D3DDevice_SetBackMaterial_1_0_5558, XTL::EmuIDirect3DDevice8_SetBackMaterial),
	// IDirect3DDevice8::SetRenderState_TwoSidedLighting
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_TwoSidedLighting_1_0_5558, XTL::EmuIDirect3DDevice8_SetTextureState_TwoSidedLighting),
	// IDirect3DDevice8::BeginStateBlock
	OOVPA_TABLE_PATCH(D3DDevice_BeginStateBlock_1_0_5558, XTL::EmuIDirect3DDevice8_BeginStateBlock),
	// IDirect3DDevice8::EndStateBlock
	OOVPA_TABLE_PATCH(D3DDevice_EndStateBlock_1_0_5558, XTL::EmuIDirect3DDevice8_EndStateBlock),
	// IDirect3DDevice8_GetDeviceCaps (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetDeviceCaps_1_0_3925, XTL::EmuIDirect3DDevice8_GetDeviceCaps),
	// IDirect3DDevice8::SetRenderState_FrontFace (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_FrontFace_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_FrontFace),
	// Get2DSurfaceDesc
	OOVPA_TABLE_PATCH(Get2DSurfaceDesc_1_0_5558, XTL::EmuGet2DSurfaceDesc),
	// D3D::MakeRequestedSpace
	OOVPA_TABLE_PATCH(D3D_MakeRequestedSpace_1_0_5558, XTL::EmuD3D_MakeRequestedSpace),
	// D3DDevice_MakeSpace
	OOVPA_TABLE_PATCH(D3DDevice_MakeSpace_1_0_5558, XTL::EmuD3DDevice_MakeSpace),
	// IDirect3DDevice8::SetRenderTarget
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderTarget_1_0_5558, XTL::EmuIDirect3DDevice8_SetRenderTarget),
	// IDirect3DDevice8::SetPixelShaderConstant
	OOVPA_TABLE_PATCH(D3DDevice_SetPixelShaderConstant_1_0_5558, XTL::EmuIDirect3DDevice8_SetPixelShaderConstant),
	// Lock2DSurface (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(Lock2DSurface_1_0_3925, XTL::EmuLock2DSurface),
	// IDirect3DDevice8::SetFlickerFilter
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetFlickerFilter_1_0_5455, XTL::EmuIDirect3DDevice8_SetFlickerFilter),
	// IDirect3DDevice8::BlockUntilVerticalBlank
	OOVPA_TABLE_PATCH(IDirect3DDevice8_BlockUntilVerticalBlank_1_0_5455, XTL::EmuIDirect3DDevice8_BlockUntilVerticalBlank),
	// IDirect3DDevice8::SetScreenSpaceOffset
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetScreenSpaceOffset_1_0_5455, XTL::EmuIDirect3DDevice8_SetScreenSpaceOffset),
	// IDirect3DDevice8::SetScreenSpaceOffset
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetScreenSpaceOffset_1_0_5558, XTL::EmuIDirect3DDevice8_SetScreenSpaceOffset),
	// IDirect3DDevice8_InsertCallback
	OOVPA_TABLE_PATCH(D3DDevice_InsertCallback_1_0_5558, XTL::EmuIDirect3DDevice8_InsertCallback),
	// IDirect3DDevice8::SetRenderState_LineWidth
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_LineWidth_1_0_5455, XTL::EmuIDirect3DDevice8_SetRenderState_LineWidth),
	// D3DDevice_FlushVertexCache
	OOVPA_TABLE_PATCH(D3DDevice_FlushVertexCache_1_0_5558, XTL::EmuIDirect3DDevice8_FlushVertexCache),
	// IDirect3DDevice8::EnableOverlay (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_EnableOverlay_1_0_4361, XTL::EmuIDirect3DDevice8_EnableOverlay),
	// D3DDevice_SetRenderState_LogicOp
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_LogicOp_1_0_5558, XTL::EmuIDirect3DDevice8_SetRenderState_LogicOp),
	// IDirect3DDevice8::LightEnable
	OOVPA_TABLE_PATCH(IDirect3DDevice8_LightEnable_1_0_5455, XTL::EmuIDirect3DDevice8_LightEnable),
	// D3DDevice_SetModelView
	OOVPA_TABLE_PATCH(D3DDevice_SetModelView_1_0_5558, XTL::EmuIDirect3DDevice8_SetModelView),
	// D3DDevice_GetBackBuffer
	OOVPA_TABLE_PATCH(D3DDevice_GetBackBuffer_1_0_5558, XTL::EmuIDirect3DDevice8_GetBackBuffer),
	// D3DDevice_PersistDisplay
	OOVPA_TABLE_PATCH(D3DDevice_PersistDisplay_1_0_5558, XTL::EmuIDirect3DDevice8_PersistDisplay),
	// D3DCubeTexture_GetCubeMapSurface
	OOVPA_TABLE_PATCH(D3DCubeTexture_GetCubeMapSurface_1_0_5558, XTL::EmuIDirect3DCubeTexture8_GetCubeMapSurface),
	// IDirect3DDevice8::GetCreationParameters (* unchanged since 4034 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetCreationParameters_1_0_4034, XTL::EmuIDirect3DDevice8_GetCreationParameters),
	// IDirect3DDevice8::SetVertexDataColor
	OOVPA_TABLE_PATCH(D3DDevice_SetVertexDataColor_1_0_5558, XTL::EmuIDirect3DDevice8_SetVertexDataColor),
	// IDirect3DDevice8::SetVertexData4f
	OOVPA_TABLE_PATCH(D3DDevice_SetVertexData4f_1_0_5558, XTL::EmuIDirect3DDevice8_SetVertexData4f),
	// IDirect3DDevice8::SetVertexData2f
	OOVPA_TABLE_PATCH(D3DDevice_SetVertexData2f_1_0_5558, XTL::EmuIDirect3DDevice8_SetVertexData2f),
	// IDirect3DDevice8::ApplyStateBlock (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(D3DDevice_ApplyStateBlock_1_0_4627, XTL::EmuIDirect3DDevice8_ApplyStateBlock),
	// IDirect3DDevice8::SetRenderState_RopZCmpAlwaysRead (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead),
	// IDirect3DCubeTexture8::GetCubeMapSurface2 (* Unchanged since 4627 *)
	OOVPA_TABLE_PATCH(D3DCubeTexture_GetCubeMapSurface2_1_0_4627, XTL::EmuIDirect3DCubeTexture8_GetCubeMapSurface2),
	// IDirect3DDevice8::SetRenderState_RopZRead (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_RopZRead_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_RopZRead),
	// IDirect3DDevice8::SetRenderState_DoNotCullUncompressed (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_DoNotCullUncompressed_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_DoNotCullUncompressed),
	// D3DDevice_LoadVertexShaderProgram
	OOVPA_TABLE_PATCH(D3DDevice_LoadVertexShaderProgram_1_0_5558, XTL::EmuIDirect3DDevice8_LoadVertexShaderProgram),
	// D3DDevice_SetPixelShaderProgram
	OOVPA_TABLE_PATCH(D3DDevice_SetPixelShaderProgram_1_0_5558, XTL::EmuIDirect3DDevice8_SetPixelShaderProgram),
	// IDirect3DDevice8::SetRenderState_BackFillMode (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_BackFillMode_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_BackFillMode),
	// IDirect3DDevice8::SetRenderState_TextureFactor (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_TextureFactor_1_0_5233, XTL::EmuIDirect3DDevice8_SetRenderState_TextureFactor),
	// D3DDevice_SetRenderState_MultiSampleMask (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_MultiSampleMask_1_0_5233, XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleMask),
	// IDirect3DDevice8::SetRenderState_MultiSampleMode
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_MultiSampleMode_1_0_5558, XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleMode),
	// IDirect3DDevice8::SetRenderState_MultiSampleMode
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_MultiSampleMode_1_0_5455, XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleMode),
	// IDirect3DDevice8::SetRenderState_MultiSampleRenderTargetMode
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_MultiSampleRenderTargetMode_1_0_5558, XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode),
	// IDirect3DDevice8::SetRenderState_MultiSampleRenderTargetMode
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_MultiSampleRenderTargetMode_1_0_5455, XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode),
	// IDirect3DDevice8::SetRenderState_Dxt1NoiseEnable
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_Dxt1NoiseEnable_1_0_5558, XTL::EmuIDirect3DDevice8_SetRenderState_Dxt1NoiseEnable),
	// IDirect3DDevice8::SetRenderState_StencilCullEnable (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_StencilCullEnable_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_StencilCullEnable),
	// IDirect3DDevice8::SetTextureState_ColorKeyColor (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTextureState_ColorKeyColor_1_0_4134, XTL::EmuIDirect3DDevice8_SetTextureState_ColorKeyColor),
	// IDirect3DDevice8::IsFencePending
	OOVPA_TABLE_PATCH(D3DDevice_IsFencePending_1_0_5558, XTL::EmuIDirect3DDevice8_IsFencePending),
	// IDirect3DDevice8::SetRenderTargetFast (* unchanged since 5344 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderTargetFast_1_0_5344, XTL::EmuIDirect3DDevice8_SetRenderTargetFast),
};

// ******************************************************************
// * D3D8_1_0_5558_SIZE
// ******************************************************************
uint32 D3D8_1_0_5558_SIZE = sizeof(D3D8_1_0_5558);
