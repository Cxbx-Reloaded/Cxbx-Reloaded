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
// * D3DDevice_LazySetStateVB
// ******************************************************************
OOVPA_XREF(D3DDevice_LazySetStateVB, 5558, 12,

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
// * D3DDevice_BeginPush
// ******************************************************************
OOVPA_XREF(D3DDevice_BeginPush, 5558, 10,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x09, XREF_D3DCD_LAZYSETSTATEVB ), // 1

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
// * D3DDevice_SelectVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SelectVertexShader, 5558, 7)

        { 0x10, 0x8E },
        { 0x26, 0x00 },
        { 0x3A, 0x0D },
        { 0x4F, 0x8B },
        { 0x62, 0xE1 },
        { 0x76, 0x3B },
        { 0x8A, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShaderConstant1
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShaderConstant1, 5558, 11)

        // D3DDevice_SetVertexShaderConstant1+0x05 : add eax, 0x1C
        /*
        { 0x05, 0x83 }, // (Offset,Value)-Pair #1
        { 0x06, 0xC0 }, // (Offset,Value)-Pair #2
        { 0x07, 0x1C }, // (Offset,Value)-Pair #3

        // D3DDevice_SetVertexShaderConstant1+0x0E : jnb +0x2E
        { 0x0E, 0x73 }, // (Offset,Value)-Pair #4
        { 0x0F, 0x2E }, // (Offset,Value)-Pair #5

        // D3DDevice_SetVertexShaderConstant1+0x28 : mov [eax-0x10], ecx
        { 0x28, 0x89 }, // (Offset,Value)-Pair #6
        { 0x29, 0x48 }, // (Offset,Value)-Pair #7
        { 0x2A, 0xF0 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetVertexShaderConstant1+0x3D : retn
        { 0x3D, 0xC3 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetVertexShaderConstant1+0x47 : jmp +0xB7
        { 0x47, 0xEB }, // (Offset,Value)-Pair #10
        { 0x48, 0xB7 }, // (Offset,Value)-Pair #11
	*/

	// D3DDevice_SetVertexShaderConstant1+0x06 : add eax, 0x1C
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

	// D3DDevice_SetVertexShaderConstant1+0x53 : retn
        { 0x53, 0xC3 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShaderConstant1Fast
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShaderConstant1Fast, 5558, 11)

        // D3DDevice_SetVertexShaderConstant1Fast+0x05 : add eax, 0x1C
        { 0x05, 0x83 }, // (Offset,Value)-Pair #1
        { 0x06, 0xC0 }, // (Offset,Value)-Pair #2
        { 0x07, 0x1C }, // (Offset,Value)-Pair #3

        // D3DDevice_SetVertexShaderConstant1Fast+0x0E : jnb +0x2E
        { 0x0E, 0x73 }, // (Offset,Value)-Pair #4
        { 0x0F, 0x2E }, // (Offset,Value)-Pair #5

        // D3DDevice_SetVertexShaderConstant1Fast+0x28 : mov [eax-0x10], ecx
        { 0x28, 0x89 }, // (Offset,Value)-Pair #6
        { 0x29, 0x48 }, // (Offset,Value)-Pair #7
        { 0x2A, 0xF0 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetVertexShaderConstant1Fast+0x3D : retn
        { 0x3D, 0xC3 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetVertexShaderConstant1Fast+0x47 : jmp +0xB7
        { 0x47, 0xEB }, // (Offset,Value)-Pair #10
        { 0x48, 0xB7 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetViewport
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetViewport, 5558, 8)

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
// * D3DDevice_CreateVertexBuffer2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateVertexBuffer2, 5558, 7)

        { 0x08, 0xE8 },
        { 0x12, 0x1E },
        { 0x1C, 0x50 },
        { 0x26, 0x68 },
        { 0x31, 0x33 },
        { 0x3A, 0xFF },
        { 0x44, 0x01 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_UpdateOverlay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_UpdateOverlay, 5558, 11)

        // D3DDevice_UpdateOverlay+0x13 : mov [eax+0xXXXX], ecx
        { 0x13, 0x89 }, // (Offset,Value)-Pair #1
        { 0x14, 0x88 }, // (Offset,Value)-Pair #2
        { 0x17, 0x00 }, // (Offset,Value)-Pair #3
        { 0x18, 0x00 }, // (Offset,Value)-Pair #4

        // D3DDevice_UpdateOverlay+0x85 : and eax, 0x0FFFFFFF
        { 0x85, 0x25 }, // (Offset,Value)-Pair #5
        { 0x86, 0xFF }, // (Offset,Value)-Pair #6
        { 0x89, 0x0F }, // (Offset,Value)-Pair #7

        // D3DDevice_UpdateOverlay+0x9D : mov [esi+0x8920], ecx
        { 0x9D, 0x89 }, // (Offset,Value)-Pair #8
        { 0x9E, 0x8E }, // (Offset,Value)-Pair #9
        { 0x9F, 0x20 }, // (Offset,Value)-Pair #10
        { 0xA0, 0x89 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetOverlayUpdateStatus
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetOverlayUpdateStatus, 5558, 12)

        // D3DDevice_GetOverlayUpdateStatus+0x05 : mov ecx, [eax+0xXXXX]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #1
        { 0x06, 0x88 }, // (Offset,Value)-Pair #2
        { 0x09, 0x00 }, // (Offset,Value)-Pair #3
        { 0x0A, 0x00 }, // (Offset,Value)-Pair #4

        // D3DDevice_GetOverlayUpdateStatus+0x0C : mov esi, [eax+0xXXXX]
        { 0x0C, 0x8B }, // (Offset,Value)-Pair #5
        { 0x0D, 0xB0 }, // (Offset,Value)-Pair #6
        { 0x10, 0x00 }, // (Offset,Value)-Pair #7
        { 0x11, 0x00 }, // (Offset,Value)-Pair #8

        // D3DDevice_GetOverlayUpdateStatus+0x16 : setnz dl
        { 0x16, 0x0F }, // (Offset,Value)-Pair #9
        { 0x17, 0x95 }, // (Offset,Value)-Pair #10
        { 0x18, 0xC2 }, // (Offset,Value)-Pair #11

        // D3DDevice_GetOverlayUpdateStatus+0x1C : retn
        { 0x1C, 0xC3 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreatePalette2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreatePalette2, 5558, 7)

        { 0x0D, 0x8B },
        { 0x16, 0x00 },
        { 0x23, 0x68 },
        { 0x2E, 0x85 },
        { 0x3D, 0x5E },
        { 0x46, 0x1E },
        { 0x52, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateTexture2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateTexture2, 5558, 8)

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
// * D3DDevice_End
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_End, 5558, 7)

        { 0x07, 0x8B },
        { 0x13, 0xC7 },
        { 0x19, 0xC7 },
        { 0x22, 0x08 },
        { 0x2B, 0xE1 },
        { 0x34, 0x4E },
        { 0x40, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetLight
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetLight, 5558, 15)

        // D3DDevice_SetLight+0x1f : add REG, 0x10
        { 0x1f, 0x83 },
        { 0x21, 0x10 },

        // D3DDevice_SetLight+0x22 : and REG, 0xfffffff0
        { 0x22, 0x83 },
        { 0x24, 0xf0 },

        // D3DDevice_SetLight+0x2c : push 0x24800000
        { 0x2c, 0x68 },
        { 0x2d, 0x00 },
        { 0x2e, 0x00 },
        { 0x2f, 0x80 },
        { 0x30, 0x24 },

        // D3DDevice_SetLight+0x65 : jz +8
        { 0x65, 0x74 },
        { 0x66, 0x08 },

        // D3DDevice_SetLight+0xe3 : jz +8
        { 0xe3, 0x74 },
        { 0xe4, 0x08 },

        // D3DDevice_SetLight+0xfe : shr ???, 2
        { 0xfe, 0xc1 },
        { 0x100, 0x02 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetMaterial, 5558, 9)

        { 0x04, 0x08 },
        { 0x0C, 0x81 },
        { 0x0D, 0xC7 },
        { 0x10, 0x00 },
        { 0x16, 0x00 },
        { 0x1F, 0x81 },
        { 0x22, 0x90 },
        { 0x2C, 0x5E },
        { 0x2E, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShader, 5558, 8)

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
// * D3DDevice_SetPalette
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPalette, 5558, 12)

        // D3DDevice_SetPalette+0x0D : mov eax, [edi+esi*4+0x0A88]
        { 0x0D, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0E, 0x84 }, // (Offset,Value)-Pair #2
        { 0x0F, 0xBE }, // (Offset,Value)-Pair #3
        { 0x10, 0x88 }, // (Offset,Value)-Pair #4
        { 0x11, 0x0F }, // (Offset,Value)-Pair #5

        // D3DDevice_SetPalette+0x76 : add esi, 0x41B20
        { 0x76, 0x81 }, // (Offset,Value)-Pair #6
        { 0x77, 0xC7 }, // (Offset,Value)-Pair #7
        { 0x78, 0x20 }, // (Offset,Value)-Pair #8
        { 0x79, 0x1B }, // (Offset,Value)-Pair #9
        { 0x7A, 0x04 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetPalette+0x8A : retn 0x08
        { 0x8A, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x8B, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_BlockUntilVerticalBlank
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BlockUntilVerticalBlank, 5558, 7)

        { 0x05, 0x6A },
        { 0x08, 0x00 },
        { 0x0D, 0xB0 },
        { 0x12, 0x00 },
        { 0x17, 0x05 },
        { 0x1C, 0x50 },
        { 0x23, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawIndexedVerticesUP
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawIndexedVerticesUP, 5558, 10)

        // D3DDevice_DrawIndexedVerticesUP+0x4A : or edx, 0x800
        { 0x4A, 0x81 }, // (Offset,Value)-Pair #1
        { 0x4B, 0xCA }, // (Offset,Value)-Pair #2
        { 0x4C, 0x00 }, // (Offset,Value)-Pair #3
        { 0x4D, 0x08 }, // (Offset,Value)-Pair #4

        // D3DDevice_DrawIndexedVerticesUP+0x5C : lea edx, [edi+0xBxx]
        { 0x5C, 0x8D }, // (Offset,Value)-Pair #5
        { 0x5D, 0x96 }, // (Offset,Value)-Pair #6
     // { 0x5E, 0xC4 }, // The offset from EDI has been seen as BC4 (5558) and BD4 (5659)
        { 0x5F, 0x0B }, // (Offset,Value)-Pair #7
        { 0x60, 0x00 }, // (Offset,Value)-Pair #8

        // D3DDevice_DrawIndexedVerticesUP+0x6A : mov eax, 0x10
        { 0x6A, 0xB8 }, // (Offset,Value)-Pair #9
        { 0x6B, 0x10 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTransform
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTransform, 5558, 10)

        { 0x04, 0x8B },
        { 0x05, 0x54 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },
        { 0x08, 0x53 },
        { 0x09, 0x8B },
        { 0x0A, 0x1D },
        { 0x0F, 0x56 },
        { 0x9C, 0x00 },
        { 0x9D, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetBackBuffer2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetBackBuffer2, 5558, 12)

        // D3DDevice_GetBackBuffer2+0x04 : cmp eax, 0xFFFFFFFF
        { 0x04, 0x83 }, // (Offset,Value)-Pair #1
        { 0x05, 0xF8 }, // (Offset,Value)-Pair #2
        { 0x06, 0xFF }, // (Offset,Value)-Pair #3

        // D3DDevice_GetBackBuffer2+0x0D : mov ecx, ds:dword_XXXXXX
        { 0x07, 0x8B }, // (Offset,Value)-Pair #4
        { 0x08, 0x0D }, // (Offset,Value)-Pair #5

        // D3DDevice_GetBackBuffer2+0x15 : mov esi, [ecx+eax*4+0xXXXX]
        { 0x15, 0x8B }, // (Offset,Value)-Pair #6
        { 0x16, 0xB4 }, // (Offset,Value)-Pair #7
        { 0x17, 0x81 }, // (Offset,Value)-Pair #8
        { 0x1A, 0x00 }, // (Offset,Value)-Pair #9
        { 0x1B, 0x00 }, // (Offset,Value)-Pair #10

        // D3DDevice_GetBackBuffer2+0x40 : retn 0x04
        { 0x40, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x41, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetBackBuffer2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetBackBuffer2, 5455, 12)

        // D3DDevice_GetBackBuffer2+0x04 : cmp eax, 0xFFFFFFFF
        { 0x04, 0x83 }, // (Offset,Value)-Pair #1
        { 0x05, 0xF8 }, // (Offset,Value)-Pair #2
        { 0x06, 0xFF }, // (Offset,Value)-Pair #3

        // D3DDevice_GetBackBuffer2+0x0D : jnz +0x19
        { 0x0D, 0x75 }, // (Offset,Value)-Pair #4
        { 0x0E, 0x19 }, // (Offset,Value)-Pair #5

        // D3DDevice_GetBackBuffer2+0x15 : mov esi, [ecx+eax*4+0x15F4]
        { 0x15, 0x8B }, // (Offset,Value)-Pair #6
        { 0x16, 0xB4 }, // (Offset,Value)-Pair #7
        { 0x17, 0x81 }, // (Offset,Value)-Pair #8
        { 0x18, 0xF4 }, // (Offset,Value)-Pair #9
        { 0x19, 0x15 }, // (Offset,Value)-Pair #10

        // D3DDevice_GetBackBuffer2+0x40 : retn 0x04
        { 0x40, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x41, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * Direct3D_CheckDeviceMultiSampleType
// ******************************************************************
OOVPA_NO_XREF(Direct3D_CheckDeviceMultiSampleType, 5558, 7)

        { 0x0E, 0x00 },
        { 0x1E, 0x54 },
        { 0x2E, 0xC9 },
        { 0x3E, 0x8B },
        { 0x4E, 0x08 },
        { 0x5E, 0x72 },
        { 0x6E, 0x03 },
OOVPA_END;

// ******************************************************************
// * D3D::SetFence
// ******************************************************************
OOVPA_XREF(D3D_SetFence, 5558, 8,

    XREF_D3D_SETFENCE,
    XRefZero)

        { 0x07, 0x8B },
        { 0x09, 0x3B },
        { 0x0A, 0x46 },
        { 0x0B, 0x04 },
        { 0x0F, 0xE8 },
        { 0x14, 0x8B },
        { 0x15, 0x7E },
        { 0x16, 0x2C },
OOVPA_END;

// ******************************************************************
// * D3D::BlockOnTime
// ******************************************************************
OOVPA_XREF(D3D_BlockOnTime, 5558, 6,

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
OOVPA_XREF(D3D_BlockOnResource, 5558, 16,

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
// * D3DResource_BlockUntilNotBusy
// ******************************************************************
#define D3DResource_BlockUntilNotBusy_5558 D3DResource_BlockUntilNotBusy_3925

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
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // D3DDevice_AddRef+0x05 : mov ecx, [eax+0x04FC]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x88 }, // (Offset,Value)-Pair #3
        { 0x07, 0x34 }, // (Offset,Value)-Pair #4
        { 0x08, 0x09 }, // (Offset,Value)-Pair #5

        // D3DDevice_AddRef+0x0B : inc ecx
        { 0x0B, 0x41 }, // (Offset,Value)-Pair #6

        // D3DDevice_AddRef+0x0C : mov [eax+0x04FC], ecx
        { 0x0C, 0x89 }, // (Offset,Value)-Pair #7
        { 0x0D, 0x88 }, // (Offset,Value)-Pair #8
        { 0x0E, 0x34 }, // (Offset,Value)-Pair #9
        { 0x0F, 0x09 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreatePixelShader
// ******************************************************************
#define D3DDevice_CreatePixelShader_5558 D3DDevice_CreatePixelShader_5344

// ******************************************************************
// * D3DDevice_DrawVerticesUP
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawVerticesUP, 5558, 9)

        // D3DDevice_DrawVerticesUP+0x03 : sub esp, 0x14
        { 0x03, 0x83 }, // (Offset,Value)-Pair #1
        { 0x04, 0xEC }, // (Offset,Value)-Pair #2
        { 0x05, 0x14 }, // (Offset,Value)-Pair #3

        // D3DDevice_DrawVerticesUP+0x40 : mov [edi], eax
        { 0x40, 0x89 }, // (Offset,Value)-Pair #4
        { 0x41, 0x07 }, // (Offset,Value)-Pair #5

        // D3DDevice_DrawVerticesUP+0x42 : or dword ptr [edi+8], 800h
        { 0x42, 0x81 }, // (Offset,Value)-Pair #6

        // D3DDevice_DrawVerticesUP+0xC4 : cmp esi, 0x80
        { 0xC4, 0x81 }, // (Offset,Value)-Pair #7
        { 0xC5, 0xFE }, // (Offset,Value)-Pair #8
        { 0xC6, 0x80 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateIndexBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateIndexBuffer, 5558, 10)

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
#define D3DDevice_CreateIndexBuffer2_5558 D3DDevice_CreateIndexBuffer2_5344

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
// * D3DDevice_SetVerticalBlankCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVerticalBlankCallback, 5455, 12)

        // D3DDevice_SetVerticalBlankCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x04 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetVerticalBlankCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #5
        { 0x05, 0x0D }, // (Offset,Value)-Pair #6

        // D3DDevice_SetVerticalBlankCallback+0x0A : mov [ecx+0x1998], eax
        { 0x0A, 0x89 }, // (Offset,Value)-Pair #7
        { 0x0B, 0x81 }, // (Offset,Value)-Pair #8
        { 0x0C, 0x98 }, // (Offset,Value)-Pair #9
        { 0x0D, 0x19 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetVerticalBlankCallback+0x10 : retn 0x04
        { 0x10, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x11, 0x04 }, // (Offset,Value)-Pair #12
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
// * D3DDevice_SetRenderState_TextureFactor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_TextureFactor, 5558, 7)

        { 0x08, 0x86 },
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
OOVPA_NO_XREF(D3DDevice_GetViewport, 5558, 7)

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
OOVPA_NO_XREF(D3DDevice_Reset, 5558, 8)

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
// * D3DDevice_GetModelView
// ******************************************************************
#define D3DDevice_GetModelView_5558 D3DDevice_GetModelView_4134

// ******************************************************************
// * D3DDevice_SetBackMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetBackMaterial, 5558, 9)

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
OOVPA_NO_XREF(D3DDevice_SetRenderState_TwoSidedLighting, 5558, 8)

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
// * Get2DSurfaceDesc
// ******************************************************************
OOVPA_NO_XREF(Get2DSurfaceDesc, 5558, 10)

        // Get2DSurfaceDesc+0x2B : movzx edx, byte ptr [edi+0x0D]
        { 0x2B, 0x0F }, // (Offset,Value)-Pair #1
        { 0x2C, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x2D, 0x57 }, // (Offset,Value)-Pair #3
        { 0x2E, 0x0D }, // (Offset,Value)-Pair #4

        // Get2DSurfaceDesc+0x52 : mov edx, [eax+0xXXXX]
        { 0x52, 0x8B }, // (Offset,Value)-Pair #5
        { 0x53, 0x90 }, // (Offset,Value)-Pair #6
        { 0x56, 0x00 }, // (Offset,Value)-Pair #7
        { 0x57, 0x00 }, // (Offset,Value)-Pair #8

        // Get2DSurfaceDesc+0xAE : retn 0x0C
        { 0xAE, 0xC2 }, // (Offset,Value)-Pair #9
        { 0xAF, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3D::MakeRequestedSpace
// ******************************************************************
OOVPA_NO_XREF(D3D_MakeRequestedSpace, 5558, 12)

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
OOVPA_NO_XREF(D3DDevice_MakeSpace, 5558, 7)

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
OOVPA_NO_XREF(D3DDevice_SetRenderTarget, 5558, 8)

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
OOVPA_NO_XREF(D3DTexture_LockRect, 5558, 8)

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
OOVPA_NO_XREF(D3DDevice_SetPixelShaderConstant, 5558, 7)

        { 0x1E, 0x85 },
        { 0x3E, 0x8B },
        { 0x5E, 0xC0 },
        { 0x7E, 0x0F },
        { 0x9F, 0x8B },
        { 0xBE, 0x00 },
        { 0xDE, 0xC0 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetFlickerFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetFlickerFilter, 5455, 12)

        // D3DDevice_SetFlickerFilter+0x1A : jz, 0x22
        { 0x1A, 0x74 }, // (Offset,Value)-Pair #1
        { 0x1B, 0x22 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetFlickerFilter+0x1C : mov eax, [eax+0xXXXX]
        { 0x1C, 0x8B }, // (Offset,Value)-Pair #3
        { 0x1D, 0x80 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetFlickerFilter+0x22 : push 0; push esi; push 0x0B; push eax
        { 0x22, 0x6A }, // (Offset,Value)-Pair #5
        { 0x23, 0x00 }, // (Offset,Value)-Pair #6
        { 0x24, 0x56 }, // (Offset,Value)-Pair #7
        { 0x25, 0x6A }, // (Offset,Value)-Pair #8
        { 0x26, 0x0B }, // (Offset,Value)-Pair #9
        { 0x27, 0x50 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetFlickerFilter+0x3F : retn 0x04
        { 0x3F, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x40, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_BlockUntilVerticalBlank
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BlockUntilVerticalBlank, 5455, 11)

        // D3DDevice_BlockUntilVerticalBlank+0x05 : push 0; push 0; push 1
        { 0x05, 0x6A }, // (Offset,Value)-Pair #1
        { 0x06, 0x00 }, // (Offset,Value)-Pair #2
        { 0x07, 0x6A }, // (Offset,Value)-Pair #3
        { 0x08, 0x00 }, // (Offset,Value)-Pair #4
        { 0x09, 0x6A }, // (Offset,Value)-Pair #5
        { 0x0A, 0x01 }, // (Offset,Value)-Pair #6

        // D3DDevice_BlockUntilVerticalBlank+0x17 : add eax, 0x199C
        { 0x17, 0x05 }, // (Offset,Value)-Pair #7
        { 0x18, 0x9C }, // (Offset,Value)-Pair #8
        { 0x19, 0x19 }, // (Offset,Value)-Pair #9

        // D3DDevice_BlockUntilVerticalBlank+0x1D : call [KrnlImport]
        { 0x1D, 0xFF }, // (Offset,Value)-Pair #10

        // D3DDevice_BlockUntilVerticalBlank+0x23 : retn
        { 0x23, 0xC3 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetScreenSpaceOffset
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetScreenSpaceOffset, 5455, 8)

        // D3DDevice_SetScreenSpaceOffset+0x13 : fstp [esi+0x0AD8]
        { 0x13, 0xD9 }, // (Offset,Value)-Pair #1
        { 0x14, 0x9E }, // (Offset,Value)-Pair #2
        { 0x15, 0xD8 }, // (Offset,Value)-Pair #3
        { 0x16, 0x0A }, // (Offset,Value)-Pair #4

        // D3DDevice_SetScreenSpaceOffset+0x33 : jb +0x05
        { 0x33, 0x72 }, // (Offset,Value)-Pair #5
        { 0x34, 0x05 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetScreenSpaceOffset+0x46 : retn 0x08
        { 0x46, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x47, 0x08 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetScreenSpaceOffset
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetScreenSpaceOffset, 5558, 8)

        // D3DDevice_SetScreenSpaceOffset+0x04 : push    esi
        { 0x04, 0x56 }, // (Offset,Value)-Pair #1

        // D3DDevice_SetScreenSpaceOffset+0x13 : fstp [esi+0x0EE8]
        { 0x13, 0xD9 }, // (Offset,Value)-Pair #2
        { 0x14, 0x9E }, // (Offset,Value)-Pair #3
        { 0x16, 0x0E }, // (Offset,Value)-Pair #4

        // D3DDevice_SetScreenSpaceOffset+0x33 : jb +0x05
        { 0x33, 0x72 }, // (Offset,Value)-Pair #5
        { 0x34, 0x05 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetScreenSpaceOffset+0x46 : retn 0x08
        { 0x46, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x47, 0x08 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * D3DDevice_InsertCallback
// ******************************************************************
#define D3DDevice_InsertCallback_5558 D3DDevice_InsertCallback_5028

// ******************************************************************
// * D3DDevice_SetRenderState_LineWidth
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_LineWidth, 5455, 10)

        // D3DDevice_SetRenderState_LineWidth+0x15 : fmul dword ptr [esi+0x54C]
        { 0x15, 0xD8 }, // (Offset,Value)-Pair #1
        { 0x16, 0x8E }, // (Offset,Value)-Pair #2
        { 0x17, 0x4C }, // (Offset,Value)-Pair #3
        { 0x18, 0x05 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_LineWidth+0x32 : cmp edi, 0x1FF
        { 0x32, 0x81 }, // (Offset,Value)-Pair #5
        { 0x33, 0xFF }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_LineWidth+0x3A : mov edi, 0x1FF
        { 0x3A, 0xBF }, // (Offset,Value)-Pair #7
        { 0x3B, 0xFF }, // (Offset,Value)-Pair #8

        // D3DDevice_SetRenderState_LineWidth+0x62 : retn 0x04
        { 0x62, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x63, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;


// ******************************************************************
// * D3DDevice_SetRenderState_LineWidth
// ******************************************************************
#define D3DDevice_SetRenderState_LineWidth_5558 D3DDevice_SetRenderState_LineWidth_4627

// ******************************************************************
// * D3DDevice_FlushVertexCache
// ******************************************************************
#define D3DDevice_FlushVertexCache_5558 D3DDevice_FlushVertexCache_4134


// ******************************************************************
// * D3DDevice_LightEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_LightEnable, 5455, 13)

        // D3DDevice_LightEnable+0x0F : cmp ebx, [esi+0x0390]
        { 0x0F, 0x3B }, // (Offset,Value)-Pair #1
        { 0x10, 0x9E }, // (Offset,Value)-Pair #2
        { 0x11, 0x90 }, // (Offset,Value)-Pair #3
        { 0x12, 0x03 }, // (Offset,Value)-Pair #4

        // D3DDevice_LightEnable+0x6B : test eax, eax
        { 0x6B, 0x85 }, // (Offset,Value)-Pair #5
        { 0x6C, 0xC0 }, // (Offset,Value)-Pair #6

        // D3DDevice_LightEnable+0x9A : cmp ecx, 0xFFFFFFFF
        { 0x9A, 0x83 }, // (Offset,Value)-Pair #7
        { 0x9B, 0xF9 }, // (Offset,Value)-Pair #8
        { 0x9C, 0xFF }, // (Offset,Value)-Pair #9

        // D3DDevice_LightEnable+0xB9 : jnz +0xF7
        { 0xB9, 0x75 }, // (Offset,Value)-Pair #10
        { 0xBA, 0xF7 }, // (Offset,Value)-Pair #11

        // D3DDevice_LightEnable+0xFB : retn 0x08
        { 0xFB, 0xC2 }, // (Offset,Value)-Pair #12
        { 0xFC, 0x08 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetIndices
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetIndices, 5558, 7)

        { 0x0F, 0x10 },
        { 0x20, 0xC7 },
        { 0x31, 0xC0 },
        { 0x42, 0x78 },
        { 0x53, 0xBE },
        { 0x64, 0x89 },
        { 0x75, 0x10 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetBackBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetBackBuffer, 5558, 6)

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
OOVPA_NO_XREF(D3DDevice_PersistDisplay, 5558, 7)

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
OOVPA_NO_XREF(D3DCubeTexture_GetCubeMapSurface, 5558, 7)

        { 0x04, 0x8B },
        { 0x0A, 0x24 },
        { 0x14, 0x8B },
        { 0x16, 0x24 },
        { 0x1C, 0x0F },
        { 0x22, 0x81 },
        { 0x28, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData4f
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData4f, 5558, 7)

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
OOVPA_NO_XREF(D3DDevice_SetVertexData2f, 5558, 8)

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
OOVPA_NO_XREF(D3DDevice_LoadVertexShaderProgram, 5558, 7)

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
OOVPA_NO_XREF(D3DDevice_SetPixelShaderProgram, 5558, 7)

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
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleMode, 5558, 9)

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
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleMode, 5455, 9)

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
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, 5455, 9)

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
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, 5558, 9)

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
OOVPA_NO_XREF(D3DDevice_SetRenderState_Dxt1NoiseEnable, 5558, 8)

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
OOVPA_NO_XREF(D3DDevice_IsFencePending, 5558, 7)

        { 0x05, 0x8B },
        { 0x06, 0x48 },
        { 0x0A, 0x2C },
        { 0x0E, 0xD0 },
        { 0x12, 0x44 },
        { 0x16, 0xC2 },
        { 0x1A, 0xD8 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetSoftDisplayFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetSoftDisplayFilter, 5558, 12)

        // D3DDevice_SetSoftDisplayFilter+0x23 : jz, 0x28
        { 0x23, 0x74 }, // (Offset,Value)-Pair #1
        { 0x24, 0x28 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetSoftDisplayFilter+0x25 : mov edx, [eax+0xXXXX]
        { 0x25, 0x8B }, // (Offset,Value)-Pair #3
        { 0x26, 0x90 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetSoftDisplayFilter+0x2B : push 0; push esi; push 0x0E; push edx
        { 0x2B, 0x6A }, // (Offset,Value)-Pair #5
        { 0x2C, 0x00 }, // (Offset,Value)-Pair #6
        { 0x2D, 0x56 }, // (Offset,Value)-Pair #7
        { 0x2E, 0x6A }, // (Offset,Value)-Pair #8
        { 0x2F, 0x0E }, // (Offset,Value)-Pair #9
        { 0x30, 0x52 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetSoftDisplayFilter+0x4E : retn 0x04
        { 0x4E, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x4F, 0x04 }, // (Offset,Value)-Pair #12
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
// * D3DDevice_SetTile
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTile, 5558, 10)

        { 0x00, 0x83 },
        { 0x01, 0xEC },
        { 0x02, 0x1C },
        { 0x18, 0x48 },
        { 0x1F, 0x70 },
        { 0x97, 0x0C },
        { 0x98, 0x40 },
        { 0x99, 0x8B },
        { 0x9A, 0x94 },
        { 0x9B, 0xCB },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CaptureStateBlock
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CaptureStateBlock, 5455, 8)
	{ 0x1E, 0x3C },
	{ 0x3E, 0xC0 },
	{ 0x5E, 0x7E },
	{ 0x7E, 0x00 },
	{ 0x9E, 0x74 },
	{ 0xC0, 0x8B },
	{ 0xDE, 0x83 },
	{ 0xFE, 0x45 },
OOVPA_END;


// ******************************************************************
// * D3D8_5558
// ******************************************************************
OOVPATable D3D8_5558[] = {

	REGISTER_OOVPA(Direct3D_CreateDevice, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDisplayFieldStatus, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_LazySetStateVB, 5558, XREF),
	REGISTER_OOVPA(D3DDevice_BeginPush, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_EndPush, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_RunVertexStateShader, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderSize, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginStateBlock, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_EndStateBlock, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_LoadVertexShader, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SelectVertexShader, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVertexShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant1, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant1, 5558, ALIAS, D3DDevice_SetVertexShaderConstant1Fast),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstantNotInline, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVertexBuffer2, 5558, DISABLED),
	REGISTER_OOVPA(D3DDevice_UpdateOverlay, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_GetOverlayUpdateStatus, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_Clear, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_CreatePalette2, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPalette, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_Swap, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_BlockUntilVerticalBlank, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawIndexedVertices, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawIndexedVerticesUP, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShader, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateTexture2, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDisplayMode, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_End, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_TexCoordIndex, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BumpEnv, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BorderColor, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_CullMode, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_NormalizeNormals, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZBias, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_EdgeAntiAlias, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FillMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FogColor, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Simple, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZEnable, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilFail, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetLight, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetLight, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetMaterial, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_LightEnable, 5344, PATCH),
	REGISTER_OOVPA(D3DVertexBuffer_Lock2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_OcclusionCullEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleAntiAlias, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ShadowFunc, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_YuvEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShader, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetIndices, 5558, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetTexture, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SwitchTexture, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_Begin, 4361, PATCH),
	REGISTER_OOVPA(D3DResource_IsBusy, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginVisibilityTest, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTransform, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetStreamSource, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetViewport, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_GetBackBuffer2, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_GetBackBuffer2, 5455, PATCH),
	REGISTER_OOVPA(D3DDevice_SetShaderConstantMode, 5028, PATCH),
	REGISTER_OOVPA(D3DResource_Register, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_AddRef, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_Release, 3925, PATCH),
	REGISTER_OOVPA(D3DSurface_GetDesc, 3925, PATCH),
	REGISTER_OOVPA(D3DPalette_Lock2, 4627, PATCH),
	REGISTER_OOVPA(D3DTexture_LockRect, 5558, PATCH),
	REGISTER_OOVPA(D3DSurface_LockRect, 3925, PATCH),
	REGISTER_OOVPA(D3DBaseTexture_GetLevelCount, 4361, PATCH),
	REGISTER_OOVPA(D3DTexture_GetSurfaceLevel2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetRenderTarget2, 5028, PATCH),
	REGISTER_OOVPA(Direct3D_CheckDeviceMultiSampleType, 5558, DISABLED),
	REGISTER_OOVPA(D3D_GetDeviceCaps, 3925, DISABLED),
	REGISTER_OOVPA(D3D_SetPushBufferSize, 3925, DISABLED),
	REGISTER_OOVPA(D3DDevice_DrawVertices, 5028, PATCH),
	REGISTER_OOVPA(D3D_SetFence, 5558, XREF),
	REGISTER_OOVPA(D3DDevice_InsertFence, 3925, PATCH),
	REGISTER_OOVPA(D3D_BlockOnTime, 5558, XREF),
	REGISTER_OOVPA(D3DDevice_BlockOnFence, 5233, PATCH),
	REGISTER_OOVPA(D3D_BlockOnResource, 5558, XREF),
	REGISTER_OOVPA(D3DResource_BlockUntilNotBusy, 5558, PATCH),
	REGISTER_OOVPA(D3DVertexBuffer_GetDesc, 5233, DISABLED),
	REGISTER_OOVPA(D3DDevice_CopyRects, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScissors, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScissors, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDepthStencilSurface2, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_AddRef, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_CreatePixelShader, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant4, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawVerticesUP, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTransform, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTransform, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateIndexBuffer2, 5558, DISABLED),
	REGISTER_OOVPA(D3DDevice_CreateIndexBuffer, 5558, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetVerticalBlankCallback, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVerticalBlankCallback, 5455, PATCH),
	REGISTER_OOVPA(D3DDevice_SetSwapCallback, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_GetGammaRamp, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TextureFactor, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_VertexBlend, 4134, PATCH),
	REGISTER_OOVPA(D3D_KickOffAndWaitForIdle, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_Reset, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_SampleAlpha, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetGammaRamp, 4928, PATCH),
	REGISTER_OOVPA(D3DDevice_GetViewport, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_GetProjectionViewportMatrix, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_GetModelView, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetBackMaterial, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TwoSidedLighting, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginStateBlock, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_EndStateBlock, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDeviceCaps, 3925, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FrontFace, 4134, PATCH),
	REGISTER_OOVPA(Get2DSurfaceDesc, 5558, PATCH),
	REGISTER_OOVPA(D3D_MakeRequestedSpace, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_MakeSpace, 5558, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetRenderTarget, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShaderConstant, 5558, PATCH),
	REGISTER_OOVPA(Lock2DSurface, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetFlickerFilter, 5455, PATCH),
	REGISTER_OOVPA(D3DDevice_BlockUntilVerticalBlank, 5455, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScreenSpaceOffset, 5455, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScreenSpaceOffset, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_InsertCallback, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LineWidth, 5455, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LineWidth, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_FlushVertexCache, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_EnableOverlay, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LogicOp, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_LightEnable, 5455, PATCH),
	REGISTER_OOVPA(D3DDevice_SetModelView, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetBackBuffer, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_PersistDisplay, 5558, PATCH),
	REGISTER_OOVPA(D3DCubeTexture_GetCubeMapSurface, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_GetCreationParameters, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexDataColor, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4f, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData2f, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_ApplyStateBlock, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_CaptureStateBlock, 5455, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZCmpAlwaysRead, 3925, PATCH),
	REGISTER_OOVPA(D3DCubeTexture_GetCubeMapSurface2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZRead, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_DoNotCullUncompressed, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_LoadVertexShaderProgram, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShaderProgram, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_BackFillMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TextureFactor, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMask, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMode, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMode, 5455, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, 5455, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Dxt1NoiseEnable, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilCullEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_ColorKeyColor, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_IsFencePending, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderTargetFast, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_SetSoftDisplayFilter, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTile, 5558, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateStateBlock, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteStateBlock, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_DeletePixelShader, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteVertexShader, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_RunPushBuffer, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginPushBuffer, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_EndPushBuffer, 4627, PATCH),
};

// ******************************************************************
// * D3D8_5558_SIZE
// ******************************************************************
uint32 D3D8_5558_SIZE = sizeof(D3D8_5558);
