// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->D3D8.1.0.4134.cpp
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
// * D3DDevice_SetTextureState_TexCoordIndex
// ******************************************************************
OOVPA_XREF(D3DDevice_SetTextureState_TexCoordIndex, 4134, 1+10,

	XRefNoSaveIndex,
	XRefOne)

		XREF_ENTRY(0x18, XREF_D3DTSS_TEXCOORDINDEX), // Derived

        // D3DDevice_SetTextureState_TexCoordIndex+0x13 : shl eax, 0x07
        { 0x13, 0xC1 }, // (Offset,Value)-Pair #1
        { 0x14, 0xE0 }, // (Offset,Value)-Pair #2
        { 0x15, 0x07 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetTextureState_TexCoordIndex+0x25 : cmp eax, ecx
        { 0x25, 0x3B }, // (Offset,Value)-Pair #4
        { 0x26, 0xC1 }, // (Offset,Value)-Pair #5

        // D3DDevice_SetTextureState_TexCoordIndex+0x6C : mov edi, 0x2400
        { 0x6C, 0xBF }, // (Offset,Value)-Pair #6
        { 0x6E, 0x24 }, // (Offset,Value)-Pair #7

        // D3DDevice_SetTextureState_TexCoordIndex+0xB3 : shl edx, 0x04
        { 0xB4, 0xC1 }, // (Offset,Value)-Pair #8
        { 0xB5, 0xE2 }, // (Offset,Value)-Pair #9
        { 0xB6, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTransform
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTransform, 4134, 10)

        // D3DDevice_SetTransform+0x16 : lea edi, [ecx+ebx+0x750]
        { 0x16, 0x8D }, // (Offset,Value)-Pair #1
        { 0x17, 0xBC }, // (Offset,Value)-Pair #2
        { 0x18, 0x19 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetTransform+0x24 : repe movsd
        { 0x24, 0xF3 }, // (Offset,Value)-Pair #4
        { 0x25, 0xA5 }, // (Offset,Value)-Pair #5

        // D3DDevice_SetTransform+0x78 : fld [edx+0x3C]
        { 0x78, 0xD9 }, // (Offset,Value)-Pair #6
        { 0x79, 0x42 }, // (Offset,Value)-Pair #7
        { 0x7A, 0x3C }, // (Offset,Value)-Pair #8

        // D3DDevice_SetTransform+0x92 : jmp +0x10
        { 0x92, 0xEB }, // (Offset,Value)-Pair #9
        { 0x93, 0x10 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3D_CheckDeviceFormat
// ******************************************************************
OOVPA_NO_XREF(D3D_CheckDeviceFormat, 4134, 8)

        // D3D_CheckDeviceFormat+0x0D : retn 0x18
        { 0x0D, 0xC2 }, // (Offset,Value)-Pair #1
        { 0x0E, 0x18 }, // (Offset,Value)-Pair #2

        // D3D_CheckDeviceFormat+0x15 : jz +0x08
        { 0x15, 0x74 }, // (Offset,Value)-Pair #3
        { 0x16, 0x08 }, // (Offset,Value)-Pair #4

        // D3D_CheckDeviceFormat+0x29 : mov cl, 0x80
        { 0x29, 0xB1 }, // (Offset,Value)-Pair #5
        { 0x2A, 0x80 }, // (Offset,Value)-Pair #6

        // D3D_CheckDeviceFormat+0x61 : retn 0x18
        { 0x61, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x62, 0x18 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * D3D_GetAdapterModeCount
// ******************************************************************
OOVPA_NO_XREF(D3D_GetAdapterModeCount, 4134, 12)

        // D3D_GetAdapterModeCount+0x25 : add edx, 0xFF
        { 0x25, 0x81 }, // (Offset,Value)-Pair #1
        { 0x26, 0xE2 }, // (Offset,Value)-Pair #2
        { 0x27, 0xFF }, // (Offset,Value)-Pair #3

        // D3D_GetAdapterModeCount+0x47 : cmp edi, 0x04
        { 0x47, 0x83 }, // (Offset,Value)-Pair #4
        { 0x48, 0xFF }, // (Offset,Value)-Pair #5
        { 0x49, 0x04 }, // (Offset,Value)-Pair #6

        // D3D_GetAdapterModeCount+0x95 : shl eax, 0x02
        { 0x95, 0xC1 }, // (Offset,Value)-Pair #7
        { 0x96, 0xE0 }, // (Offset,Value)-Pair #8
        { 0x97, 0x02 }, // (Offset,Value)-Pair #9

        // D3D_GetAdapterModeCount+0x99 : add esp, 0x08
        { 0x99, 0x83 }, // (Offset,Value)-Pair #10
        { 0x9A, 0xC4 }, // (Offset,Value)-Pair #11
        { 0x9B, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3D_EnumAdapterModes
// ******************************************************************
OOVPA_NO_XREF(D3D_EnumAdapterModes, 4134, 10)

        // D3D_EnumAdapterModes+0x00 : sub esp, 0x08
        { 0x00, 0x83 }, // (Offset,Value)-Pair #1
        { 0x01, 0xEC }, // (Offset,Value)-Pair #2
        { 0x02, 0x08 }, // (Offset,Value)-Pair #3

        // D3D_EnumAdapterModes+0xD1 : and edx, 0x0A
        { 0xD1, 0x83 }, // (Offset,Value)-Pair #4
        { 0xD2, 0xE2 }, // (Offset,Value)-Pair #5
        { 0xD3, 0x0A }, // (Offset,Value)-Pair #6

        // D3D_EnumAdapterModes+0xF5 : mov dword ptr[edi+0x10], 0x1E
        { 0xF5, 0xC7 }, // (Offset,Value)-Pair #7
        { 0xF6, 0x47 }, // (Offset,Value)-Pair #8
        { 0xF7, 0x10 }, // (Offset,Value)-Pair #9
        { 0xF8, 0x1E }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_FogColor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_FogColor, 4134, 10)

        // D3DDevice_SetRenderState_FogColor+0x07 : mov eax, [esi]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x06 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_FogColor+0x09 : cmp eax, [esi+4]
        { 0x09, 0x3B }, // (Offset,Value)-Pair #3
        { 0x0A, 0x46 }, // (Offset,Value)-Pair #4
        { 0x0B, 0x04 }, // (Offset,Value)-Pair #5

        // D3DDevice_SetRenderState_FogColor+0x25 : shl edi, 0x10
        { 0x25, 0xC1 }, // (Offset,Value)-Pair #6
        { 0x26, 0xE7 }, // (Offset,Value)-Pair #7
        { 0x27, 0x10 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetRenderState_FogColor+0x40 : mov [esi], eax
        { 0x40, 0x89 }, // (Offset,Value)-Pair #9
        { 0x41, 0x06 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_Swap
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Swap, 4134, 11)

        // D3DDevice_Swap+0x10 : mov ebx, 5
        { 0x10, 0xBB }, // (Offset,Value)-Pair #1
        { 0x11, 0x05 }, // (Offset,Value)-Pair #2

        // D3DDevice_Swap+0x1D : test bl, 3
        { 0x1D, 0xF6 }, // (Offset,Value)-Pair #3
        { 0x1E, 0xC3 }, // (Offset,Value)-Pair #4
        { 0x1F, 0x03 }, // (Offset,Value)-Pair #5

        // D3DDevice_Swap+0x46 : inc dword ptr [esi+0x2ABC]
        { 0x46, 0xFF }, // (Offset,Value)-Pair #6
        { 0x47, 0x86 }, // (Offset,Value)-Pair #7
        { 0x48, 0xBC }, // (Offset,Value)-Pair #8
        { 0x49, 0x2A }, // (Offset,Value)-Pair #9

        // D3DDevice_Swap+0xAE : retn 4
        { 0xAE, 0xC2 }, // (Offset,Value)-Pair #10
        { 0xAF, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_Clear
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Clear, 4134, 9)
        // D3DDevice_Clear+0x0000 : sub esp, 38h
        { 0x0000, 0x83 }, // (Offset,Value)-Pair #1
        { 0x0001, 0xEC }, // (Offset,Value)-Pair #2
        { 0x0002, 0x38 }, // (Offset,Value)-Pair #3

        // D3DDevice_Clear+0x000B : mov ebx, [ebp+0x2074]
        { 0x000B, 0x8B }, // (Offset,Value)-Pair #4
        { 0x000C, 0x9D }, // (Offset,Value)-Pair #5
        { 0x000D, 0x74 }, // (Offset,Value)-Pair #6
        { 0x000E, 0x20 }, // (Offset,Value)-Pair #7

        // D3DDevice_Clear+0x0399 : retn 0x18
        { 0x0399, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x039A, 0x18 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3DDevice_Begin
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Begin, 4134, 11)
        // D3DDevice_Begin : mov dword ptr [eax], 0x417FC
        { 0x1C, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x1D, 0x00 }, // (Offset,Value)-Pair #2
        { 0x1E, 0xFC }, // (Offset,Value)-Pair #3
        { 0x1F, 0x17 }, // (Offset,Value)-Pair #4
        { 0x20, 0x04 }, // (Offset,Value)-Pair #5
        { 0x21, 0x00 }, // (Offset,Value)-Pair #6

        // D3DDevice_Begin : mov     [eax+4], ecx
        { 0x22, 0x89 }, // (Offset,Value)-Pair #7
        { 0x23, 0x48 }, // (Offset,Value)-Pair #8
        { 0x24, 0x04 }, // (Offset,Value)-Pair #9

        // D3DDevice_Begin : retn 0x04
        { 0x32, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x33, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_End
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_End, 4134, 7)
	{ 0x07, 0x8B },
	{ 0x0E, 0xE8 },
	{ 0x16, 0x17 },
	{ 0x1E, 0x00 },
	{ 0x26, 0x46 },
	{ 0x2E, 0x01 },
	{ 0x36, 0x08 },
OOVPA_END;


// ******************************************************************
// * D3DDevice_GetBackBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetBackBuffer, 4134, 12)

        // D3DDevice_GetBackBuffer+0x04 : cmp eax, 0xFFFFFFFF
        { 0x04, 0x83 }, // (Offset,Value)-Pair #1
        { 0x05, 0xF8 }, // (Offset,Value)-Pair #2
        { 0x06, 0xFF }, // (Offset,Value)-Pair #3

        // D3DDevice_GetBackBuffer+0x0D : jnz +0x07
        { 0x0D, 0x75 }, // (Offset,Value)-Pair #4
        { 0x0E, 0x07 }, // (Offset,Value)-Pair #5

        // D3DDevice_GetBackBuffer+0x18 : sbb eax, eax
        { 0x18, 0x1B }, // (Offset,Value)-Pair #6
        { 0x19, 0xC0 }, // (Offset,Value)-Pair #7

        // D3DDevice_GetBackBuffer+0x1D : mov eax, [ecx+eax*4+0x207C]
        { 0x1D, 0x8B }, // (Offset,Value)-Pair #8
        { 0x1E, 0x84 }, // (Offset,Value)-Pair #9
        { 0x1F, 0x81 }, // (Offset,Value)-Pair #10
        { 0x20, 0x7C }, // (Offset,Value)-Pair #11
        { 0x21, 0x20 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetDepthStencilSurface
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetDepthStencilSurface, 4134, 9)

        // D3DDevice_GetDepthStencilSurface+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // D3DDevice_GetDepthStencilSurface+0x05 : mov eax, [eax + 0x2074]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x80 }, // (Offset,Value)-Pair #3
        { 0x07, 0x74 }, // (Offset,Value)-Pair #4
        { 0x08, 0x20 }, // (Offset,Value)-Pair #5

        // D3DDevice_GetDepthStencilSurface+0x11 : mov [ecx], eax
        { 0x11, 0x89 }, // (Offset,Value)-Pair #6
        { 0x12, 0x01 }, // (Offset,Value)-Pair #7

        // D3DDevice_GetDepthStencilSurface+0x1D : retn 0x04
        { 0x1D, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x1E, 0x04 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderTarget
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderTarget, 4134, 9)

        // D3DDevice_SetRenderTarget+0x00 : sub esp, 0x3C
        { 0x00, 0x83 }, // (Offset,Value)-Pair #1
        { 0x01, 0xEC }, // (Offset,Value)-Pair #2
        { 0x02, 0x3C }, // (Offset,Value)-Pair #3

        // D3DDevice_SetRenderTarget+014 : jnz +0x0C
        { 0x14, 0x75 }, // (Offset,Value)-Pair #4
        { 0x15, 0x0C }, // (Offset,Value)-Pair #5

        // D3DDevice_SetRenderTarget+0x44 : push ebp
        { 0x44, 0x55 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderTarget+0x79 : shr ecx, 0x14
        { 0x79, 0xC1 }, // (Offset,Value)-Pair #7
        { 0x7A, 0xE9 }, // (Offset,Value)-Pair #8
        { 0x7B, 0x14 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3DDevice_SelectVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SelectVertexShader, 4134, 11)

        // D3DDevice_SelectVertexShader+0x13 : mov [esi+0x384], eax
        { 0x13, 0x89 }, // (Offset,Value)-Pair #1
        { 0x14, 0x86 }, // (Offset,Value)-Pair #2
        { 0x15, 0x84 }, // (Offset,Value)-Pair #3
        { 0x16, 0x03 }, // (Offset,Value)-Pair #4

        // D3DDevice_SelectVertexShader+0x3B : mov dword ptr [eax], 0x00081E94
        { 0x3B, 0xC7 }, // (Offset,Value)-Pair #5
        { 0x3C, 0x00 }, // (Offset,Value)-Pair #6
        { 0x3D, 0x94 }, // (Offset,Value)-Pair #7
        { 0x3E, 0x1E }, // (Offset,Value)-Pair #8
        { 0x3F, 0x08 }, // (Offset,Value)-Pair #9

        // D3DDevice_SelectVertexShader+0x55 : mov [esi], eax
        { 0x55, 0x89 }, // (Offset,Value)-Pair #10
        { 0x56, 0x06 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_CopyRects
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CopyRects, 4134, 10)

        // D3DDevice_CopyRects+0x00 : sub esp, 0x01A0
        { 0x00, 0x81 }, // (Offset,Value)-Pair #1
        { 0x01, 0xEC }, // (Offset,Value)-Pair #2
        { 0x02, 0xA0 }, // (Offset,Value)-Pair #3
        { 0x03, 0x01 }, // (Offset,Value)-Pair #4

        // D3DDevice_CopyRects+0x6E : mov esi, [eax+0x201C]
        { 0x6E, 0x8B }, // (Offset,Value)-Pair #5
        { 0x6F, 0xB0 }, // (Offset,Value)-Pair #6
        { 0x70, 0x1C }, // (Offset,Value)-Pair #7
        { 0x71, 0x20 }, // (Offset,Value)-Pair #8

        // D3DDevice_CopyRects+0xDD : jnz +0x22
        { 0xDD, 0x75 }, // (Offset,Value)-Pair #9
        { 0xDE, 0x22 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateImageSurface
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateImageSurface, 4134, 28)
	{ 0x00, 0x83 }, 
	{ 0x01, 0xEC },
	{ 0x02, 0x08 },
	{ 0x03, 0x53 },
	{ 0x04, 0x8A },
	{ 0x05, 0x5C },
	{ 0x06, 0x24 },
	{ 0x07, 0x20 },
	{ 0x08, 0xF6 },
	{ 0x09, 0xC3 },
	{ 0x0A, 0x20 },
	{ 0x0B, 0x55 },
	{ 0x0C, 0x8B },
	{ 0x0D, 0x6C },
	{ 0x0E, 0x24 },
	{ 0x0F, 0x14 },
	{ 0x10, 0x56 },
	{ 0x11, 0x57 },
	{ 0x12, 0x75 },
	{ 0x13, 0x06 },
	{ 0x14, 0x55 },
	{ 0x15, 0xE8 },
	{ 0x1A, 0x8D },
	{ 0x1B, 0x44 },
	{ 0x1C, 0x24 },
	{ 0x1D, 0x10 },
	{ 0x1E, 0x50 },
	{ 0x1F, 0x8D },

	OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPixelShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPixelShader, 4134, 10)

        // D3DDevice_SetPixelShader+0x0E : mov esi, [ebx+0x0370]
        { 0x0E, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0F, 0xB3 }, // (Offset,Value)-Pair #2
        { 0x10, 0x70 }, // (Offset,Value)-Pair #3
        { 0x11, 0x03 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetPixelShader+0x3A : or eax, 0x2000
        { 0x3A, 0x0D }, // (Offset,Value)-Pair #5
        { 0x3C, 0x20 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetPixelShader+0x78 : retn 0x04
        { 0x78, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x79, 0x04 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetPixelShader+0xE3 : rep movsd
        { 0xE3, 0xF3 }, // (Offset,Value)-Pair #9
        { 0xE4, 0xA5 }, // (Offset,Value)-Pair #10

OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_BumpEnv
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTextureState_BumpEnv, 4134, 12)

        // D3DDevice_SetTextureState_BumpEnv+0x18 : jnz +0x03
        { 0x18, 0x75 }, // (Offset,Value)-Pair #1
        { 0x19, 0x03 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetTextureState_BumpEnv+0x1D : test bl, 3
        { 0x1D, 0xF6 }, // (Offset,Value)-Pair #3
        { 0x1E, 0xC3 }, // (Offset,Value)-Pair #4
        { 0x1F, 0x03 }, // (Offset,Value)-Pair #5

        // D3DDevice_SetTextureState_BumpEnv+0x32 : mov ecx, [esp+0x14]
        { 0x32, 0x8B }, // (Offset,Value)-Pair #6
        { 0x33, 0x4C }, // (Offset,Value)-Pair #7
        { 0x34, 0x24 }, // (Offset,Value)-Pair #8
        { 0x35, 0x18 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetTextureState_BumpEnv+0x50 : shl esi, 5
        { 0x50, 0xC1 }, // (Offset,Value)-Pair #10
        { 0x51, 0xE6 }, // (Offset,Value)-Pair #11
        { 0x52, 0x05 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetIndices
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetIndices, 4134, 11)

        // D3DDevice_SetIndices+0x0E : jz +0x10
        { 0x0E, 0x74 }, // (Offset,Value)-Pair #1
        { 0x0F, 0x10 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetIndices+0x1E : jmp +0x0A
        { 0x1E, 0xEB }, // (Offset,Value)-Pair #3
        { 0x1F, 0x0A }, // (Offset,Value)-Pair #4

        // D3DDevice_SetIndices+0x59 : mov [esi+0x1C], ecx
        { 0x59, 0x89 }, // (Offset,Value)-Pair #5
        { 0x5A, 0x4E }, // (Offset,Value)-Pair #6
        { 0x5B, 0x1C }, // (Offset,Value)-Pair #7

        // D3DDevice_SetIndices+0x76 : mov [esi+0x038C], edi
        { 0x76, 0x89 }, // (Offset,Value)-Pair #8
        { 0x77, 0xBE }, // (Offset,Value)-Pair #9
        { 0x78, 0x8C }, // (Offset,Value)-Pair #10
        { 0x79, 0x03 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_VertexBlend
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_VertexBlend, 4134, 11)

        // D3DDevice_SetRenderState_VertexBlend+0x06 : or edx, 0x200
        { 0x06, 0x81 }, // (Offset,Value)-Pair #1
        { 0x07, 0xCA }, // (Offset,Value)-Pair #2
        { 0x08, 0x00 }, // (Offset,Value)-Pair #3
        { 0x09, 0x02 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_VertexBlend+0x1E : jb +0x05
        { 0x1E, 0x72 }, // (Offset,Value)-Pair #5
        { 0x1F, 0x05 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_VertexBlend+0x2F : mov [eax+4], ecx
        { 0x2F, 0x89 }, // (Offset,Value)-Pair #7
        { 0x30, 0x48 }, // (Offset,Value)-Pair #8
        { 0x31, 0x04 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetRenderState_VertexBlend+0x3E : retn 0x04
        { 0x3E, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x3F, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_TextureFactor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_TextureFactor, 4134, 9)

        // D3DDevice_SetRenderState_TextureFactor+0x07 : mov eax, [esi+0x0370]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x86 }, // (Offset,Value)-Pair #2
        { 0x09, 0x70 }, // (Offset,Value)-Pair #3
        { 0x0A, 0x03 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_TextureFactor+0x2F : nop
        { 0x2F, 0x90 }, // (Offset,Value)-Pair #5

        // D3DDevice_SetRenderState_TextureFactor+0x41 : retn 0x04
        { 0x41, 0xC2 }, // (Offset,Value)-Pair #6
        { 0x42, 0x04 }, // (Offset,Value)-Pair #7

        // D3DDevice_SetRenderState_TextureFactor+0x4E : retn 0x04
        { 0x4E, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x4F, 0x04 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_EdgeAntiAlias
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_EdgeAntiAlias, 4134, 10)

        // D3DDevice_SetRenderState_EdgeAntiAlias+0x07 : mov eax, [esi]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x06 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_EdgeAntiAlias+0x0C : jb +0x05
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #3
        { 0x0D, 0x05 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_EdgeAntiAlias+0x13 : mov ecx, [esp+0x08]
        { 0x13, 0x8B }, // (Offset,Value)-Pair #5
        { 0x14, 0x4C }, // (Offset,Value)-Pair #6
        { 0x15, 0x24 }, // (Offset,Value)-Pair #7
        { 0x16, 0x08 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetRenderState_EdgeAntiAlias+0x2F : retn 0x04
        { 0x2F, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x30, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_ZEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_ZEnable, 4134, 13)

        // D3DDevice_SetRenderState_ZEnable+0x0C : jb +0x05
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #1
        { 0x0D, 0x05 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_ZEnable+0x1B : mov edx, [esi+0x2074h]
        { 0x1B, 0x8B }, // (Offset,Value)-Pair #3
        { 0x1C, 0x96 }, // (Offset,Value)-Pair #4
        { 0x1D, 0x74 }, // (Offset,Value)-Pair #5
        { 0x1E, 0x20 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_ZEnable+0x31 : mov dword ptr [eax], 0x4030C
        { 0x31, 0xC7 }, // (Offset,Value)-Pair #7
        { 0x32, 0x00 }, // (Offset,Value)-Pair #8
        { 0x33, 0x0C }, // (Offset,Value)-Pair #9
        { 0x34, 0x03 }, // (Offset,Value)-Pair #10
        { 0x35, 0x04 }, // (Offset,Value)-Pair #11

        // D3DDevice_SetRenderState_ZEnable+0x98 : retn 0x04
        { 0x98, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x99, 0x04 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_StencilEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilEnable, 4134, 11)

        // D3DDevice_SetRenderState_StencilEnable+0x0C : jb +0x05
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #1
        { 0x0D, 0x05 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_StencilEnable+0x2C : jz +0x19
        { 0x2C, 0x74 }, // (Offset,Value)-Pair #3
        { 0x2D, 0x19 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_StencilEnable+0x4D : mov dword ptr [eax], 0x00041D84
        { 0x4D, 0xC7 }, // (Offset,Value)-Pair #5
        { 0x4E, 0x00 }, // (Offset,Value)-Pair #6
        { 0x4F, 0x84 }, // (Offset,Value)-Pair #7
        { 0x50, 0x1D }, // (Offset,Value)-Pair #8
        { 0x51, 0x04 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetRenderState_StencilEnable+0x81 : retn 0x04
        { 0x81, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x82, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleAntiAlias
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleAntiAlias, 4134, 12)

        // D3DDevice_SetRenderState_MultiSampleAntiAlias+0x04 : push esi
        { 0x04, 0x56 }, // (Offset,Value)-Pair #1

        // D3DDevice_SetRenderState_MultiSampleAntiAlias+0x05 : mov esi, dword ptr [addr]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x35 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetRenderState_MultiSampleAntiAlias+0x0B : mov ecx, esi
        { 0x0B, 0x8B }, // (Offset,Value)-Pair #4
        { 0x0C, 0xCE }, // (Offset,Value)-Pair #5

        // D3DDevice_SetRenderState_MultiSampleAntiAlias+0x1C : jb +0x05
        { 0x1C, 0x72 }, // (Offset,Value)-Pair #6
        { 0x1D, 0x05 }, // (Offset,Value)-Pair #7

        // D3DDevice_SetRenderState_MultiSampleAntiAlias+0x38 : shl ecx, 0x10
        { 0x38, 0xC1 }, // (Offset,Value)-Pair #8
        { 0x39, 0xE1 }, // (Offset,Value)-Pair #9
        { 0x3A, 0x10 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetRenderState_MultiSampleAntiAlias+0x4F : retn 0x04
        { 0x4F, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x50, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetTransform
// ******************************************************************
#define D3DDevice_GetTransform_4134 D3DDevice_GetTransform_4039

// ******************************************************************
// * D3DDevice_SetVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShader, 4134, 14)

        // D3DDevice_SetVertexShader+0x06 : test bl, 1
        { 0x06, 0xF6 }, // (Offset,Value)-Pair #1
        { 0x07, 0xC3 }, // (Offset,Value)-Pair #2
        { 0x08, 0x01 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetVertexShader+0x11 : mov eax, [esi+0x0380]
        { 0x11, 0x8B }, // (Offset,Value)-Pair #4
        { 0x12, 0x86 }, // (Offset,Value)-Pair #5
        { 0x13, 0x80 }, // (Offset,Value)-Pair #6
        { 0x14, 0x03 }, // (Offset,Value)-Pair #7

        // D3DDevice_SetVertexShader+0x90 : retn 0x04
        { 0x90, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x91, 0x04 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetVertexShader+0xB0 : mov dword ptr [eax], 0x4194C
        { 0xB0, 0xC7 }, // (Offset,Value)-Pair #10
        { 0xB1, 0x00 }, // (Offset,Value)-Pair #11
        { 0xB2, 0x4C }, // (Offset,Value)-Pair #12
        { 0xB3, 0x19 }, // (Offset,Value)-Pair #13
        { 0xB4, 0x04 }, // (Offset,Value)-Pair #14
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawVerticesUP
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawVerticesUP, 4134, 10)

        // D3DDevice_DrawVerticesUP+0x03 : sub esp, 0x14
        { 0x03, 0x83 }, // (Offset,Value)-Pair #1
        { 0x04, 0xEC }, // (Offset,Value)-Pair #2
        { 0x05, 0x14 }, // (Offset,Value)-Pair #3

        // D3DDevice_DrawVerticesUP+0x2F : mov edx, [edi+0x744]
        { 0x2F, 0x8B }, // (Offset,Value)-Pair #4
        { 0x30, 0x97 }, // (Offset,Value)-Pair #5
        { 0x31, 0x44 }, // (Offset,Value)-Pair #6
        { 0x32, 0x07 }, // (Offset,Value)-Pair #7

        // D3DDevice_DrawVerticesUP+0xC1 : cmp esi, 0x80
        { 0xC1, 0x81 }, // (Offset,Value)-Pair #8
        { 0xC2, 0xFE }, // (Offset,Value)-Pair #9
        { 0xC3, 0x80 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetLight
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetLight, 4134, 12)

        // D3DDevice_SetLight+0x11 : mov edi, [ebp+0x394]
        { 0x11, 0x8B }, // (Offset,Value)-Pair #1
        { 0x12, 0xBD }, // (Offset,Value)-Pair #2
        { 0x13, 0x94 }, // (Offset,Value)-Pair #3
        { 0x14, 0x03 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetLight+0x30 : shl eax, 0x04
        { 0x30, 0xC1 }, // (Offset,Value)-Pair #5
        { 0x31, 0xE0 }, // (Offset,Value)-Pair #6
        { 0x32, 0x04 }, // (Offset,Value)-Pair #7

        // D3DDevice_SetLight+0xBD : rep stosb
        { 0xBD, 0xF3 }, // (Offset,Value)-Pair #8
        { 0xBE, 0xAB }, // (Offset,Value)-Pair #9

        // D3DDevice_SetLight+0xFD : mov ecx, [ebx+0x68]
        { 0xFD, 0x8B }, // (Offset,Value)-Pair #10
        { 0xFE, 0x4B }, // (Offset,Value)-Pair #11
        { 0xFF, 0x68 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetMaterial, 4134, 12)

        // D3DDevice_SetMaterial+0x0C : add edi, 0x0940
        { 0x0C, 0x81 }, // (Offset,Value)-Pair #1
        { 0x0D, 0xC7 }, // (Offset,Value)-Pair #2
        { 0x0E, 0xF0 }, // (Offset,Value)-Pair #3
        { 0x0F, 0x09 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetMaterial+0x12 : mov ecx, 0x11
        { 0x12, 0xB9 }, // (Offset,Value)-Pair #5
        { 0x13, 0x11 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetMaterial+0x1F : or ecx, 0x1000
        { 0x1F, 0x81 }, // (Offset,Value)-Pair #7
        { 0x20, 0xC9 }, // (Offset,Value)-Pair #8
        { 0x21, 0x00 }, // (Offset,Value)-Pair #9
        { 0x22, 0x10 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetMaterial+0x2D : retn 0x04
        { 0x2D, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x2E, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_LightEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_LightEnable, 4134, 13)

        // D3DDevice_LightEnable+0x0F : cmp ebx, [esi+0x0394]
        { 0x0F, 0x3B }, // (Offset,Value)-Pair #1
        { 0x10, 0x9E }, // (Offset,Value)-Pair #2
        { 0x11, 0x94 }, // (Offset,Value)-Pair #3
        { 0x12, 0x03 }, // (Offset,Value)-Pair #4

        // D3DDevice_LightEnable+0x6B : test eax, eax
        { 0x6B, 0x85 }, // (Offset,Value)-Pair #5
        { 0x6C, 0xC0 }, // (Offset,Value)-Pair #6

        // D3DDevice_LightEnable+0x9E : mov eax, [eax+0x8C]
        { 0x9E, 0x8B }, // (Offset,Value)-Pair #7
        { 0x9F, 0x80 }, // (Offset,Value)-Pair #8
        { 0xA0, 0x8C }, // (Offset,Value)-Pair #9

        // D3DDevice_LightEnable+0xBC : jz +0x12
        { 0xBC, 0x74 }, // (Offset,Value)-Pair #10
        { 0xBD, 0x12 }, // (Offset,Value)-Pair #11

        // D3DDevice_LightEnable+0xE1 : retn 0x08
        { 0xE1, 0xC2 }, // (Offset,Value)-Pair #12
        { 0xE2, 0x08 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_NormalizeNormals
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_NormalizeNormals, 4134, 8)

        // D3DDevice_SetRenderState_NormalizeNormals+0x07 : mov eax, [esi]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x06 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_NormalizeNormals+0x25 : or [addr], 0x200
        { 0x25, 0x81 }, // (Offset,Value)-Pair #3
        { 0x26, 0x0D }, // (Offset,Value)-Pair #4
        { 0x2B, 0x00 }, // (Offset,Value)-Pair #5
        { 0x2C, 0x02 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_NormalizeNormals+0x36 : retn 0x04
        { 0x36, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x37, 0x04 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * Get2DSurfaceDesc
// ******************************************************************
OOVPA_NO_XREF(Get2DSurfaceDesc, 4134, 10)

        // Get2DSurfaceDesc+0x2B : movzx edx, byte ptr [edi+0x0D]
        { 0x2B, 0x0F }, // (Offset,Value)-Pair #1
        { 0x2C, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x2D, 0x57 }, // (Offset,Value)-Pair #3
        { 0x2E, 0x0D }, // (Offset,Value)-Pair #4

        // Get2DSurfaceDesc+0x52 : mov edx, [eax+0x21C0]
        { 0x52, 0x8B }, // (Offset,Value)-Pair #5
        { 0x53, 0x90 }, // (Offset,Value)-Pair #6
        { 0x54, 0x7C }, // (Offset,Value)-Pair #7
        { 0x55, 0x20 }, // (Offset,Value)-Pair #8

        // Get2DSurfaceDesc+0xAE : retn 0x0C
        { 0xAE, 0xC2 }, // (Offset,Value)-Pair #9
        { 0xAF, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_Reset
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Reset, 4134, 10)

        // D3DDevice_Reset+0x15 : mov ecx, ebp
        { 0x15, 0x8B }, // (Offset,Value)-Pair #1
        { 0x16, 0xCD }, // (Offset,Value)-Pair #2

        // D3DDevice_Reset+0x1E : jnz +0xF5
        { 0x1E, 0x75 }, // (Offset,Value)-Pair #3
        { 0x1F, 0xF5 }, // (Offset,Value)-Pair #4

        // D3DDevice_Reset+0x27 : mov esi, [esp+0x14]
        { 0x27, 0x8B }, // (Offset,Value)-Pair #5
        { 0x28, 0x74 }, // (Offset,Value)-Pair #6
        { 0x29, 0x24 }, // (Offset,Value)-Pair #7
        { 0x2A, 0x14 }, // (Offset,Value)-Pair #8

        // D3DDevice_Reset+0x37 : jge +0x10
        { 0x37, 0x7D }, // (Offset,Value)-Pair #9
        { 0x38, 0x10 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_TwoSidedLighting
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_TwoSidedLighting, 4134, 13)

        // D3DDevice_SetRenderState_TwoSidedLighting+0x11 : or edx, 0x1000
        { 0x11, 0x81 }, // (Offset,Value)-Pair #1
        { 0x12, 0xCA }, // (Offset,Value)-Pair #2
        { 0x13, 0x00 }, // (Offset,Value)-Pair #3
        { 0x14, 0x10 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_TwoSidedLighting+0x24 : cmp eax, [esi+4]; push edi
        { 0x24, 0x3B }, // (Offset,Value)-Pair #5
        { 0x25, 0x46 }, // (Offset,Value)-Pair #6
        { 0x26, 0x04 }, // (Offset,Value)-Pair #7
        { 0x27, 0x57 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetRenderState_TwoSidedLighting+0x53 : add eax, 0x0C
        { 0x53, 0x83 }, // (Offset,Value)-Pair #9
        { 0x54, 0xC0 }, // (Offset,Value)-Pair #10
        { 0x55, 0x0C }, // (Offset,Value)-Pair #11

        // D3DDevice_SetRenderState_TwoSidedLighting+0x60 : retn 0x04
        { 0x60, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x61, 0x04 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_BackFillMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_BackFillMode, 4134, 13)

        // D3DDevice_SetRenderState_BackFillMode+0x1C : jb +0x05
        { 0x1C, 0x72 }, // (Offset,Value)-Pair #1
        { 0x1D, 0x05 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_BackFillMode+0x35 : mov dword ptr [eax], abs
        { 0x35, 0xC7 }, // (Offset,Value)-Pair #3
        { 0x36, 0x00 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_BackFillMode+0x3B : add eax, 0x0C; mov [esi], eax; mov [abs], edi
        { 0x41, 0x83 }, // (Offset,Value)-Pair #5
        { 0x42, 0xC0 }, // (Offset,Value)-Pair #6
        { 0x43, 0x0C }, // (Offset,Value)-Pair #7
        { 0x44, 0x89 }, // (Offset,Value)-Pair #8
        { 0x45, 0x06 }, // (Offset,Value)-Pair #9
        { 0x46, 0x89 }, // (Offset,Value)-Pair #10
        { 0x47, 0x3D }, // (Offset,Value)-Pair #11

        // D3DDevice_SetRenderState_BackFillMode+0x4E : retn 0x04
        { 0x4E, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x4F, 0x04 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_ColorKeyColor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTextureState_ColorKeyColor, 4134, 12)

        // D3DDevice_SetTextureState_ColorKeyColor+0x0C : jb +0x05
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #1
        { 0x0D, 0x05 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetTextureState_ColorKeyColor+0x27 : add eax, 8; mov [esi], eax; shl ecx, 7
        { 0x27, 0x83 }, // (Offset,Value)-Pair #3
        { 0x28, 0xC0 }, // (Offset,Value)-Pair #4
        { 0x29, 0x08 }, // (Offset,Value)-Pair #5
        { 0x2A, 0x89 }, // (Offset,Value)-Pair #6
        { 0x2B, 0x06 }, // (Offset,Value)-Pair #7
        { 0x2C, 0xC1 }, // (Offset,Value)-Pair #8
        { 0x2D, 0xE1 }, // (Offset,Value)-Pair #9
        { 0x2E, 0x07 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetTextureState_ColorKeyColor+0x36 : retn 0x08
        { 0x36, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x37, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_FrontFace
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_FrontFace, 4134, 11,

    XRefNoSaveIndex,
    XRefOne)

        // D3DDevice_SetRenderState_FrontFace+0x36 : jmp D3DDevice_SetRenderState_CullMode
        XREF_ENTRY( 0x36, XREF_SETRENDERSTATE_CULLMODE), // (Offset,Value)-Pair #1

        // D3DDevice_SetRenderState_FrontFace+0x09 : cmp eax, [esi+4]
        { 0x09, 0x3B }, // (Offset,Value)-Pair #2
        { 0x0A, 0x46 }, // (Offset,Value)-Pair #3
        { 0x0B, 0x04 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_FrontFace+0x13 : mov ecx, [esp+0x08]
        { 0x13, 0x8B }, // (Offset,Value)-Pair #5
        { 0x14, 0x4C }, // (Offset,Value)-Pair #6
        { 0x15, 0x24 }, // (Offset,Value)-Pair #7
        { 0x16, 0x08 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetRenderState_FrontFace+0x20 : add eax, 8
        { 0x20, 0x83 }, // (Offset,Value)-Pair #9
        { 0x21, 0xC0 }, // (Offset,Value)-Pair #10
        { 0x22, 0x08 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_LogicOp
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_LogicOp, 4134, 9)

        // D3DDevice_SetRenderState_LogicOp+0x07 : mov eax, [esi]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x06 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_LogicOp+0x1B : mov dword ptr [eax], 0x417BC
        { 0x1B, 0xC7 }, // (Offset,Value)-Pair #3
        { 0x1C, 0x00 }, // (Offset,Value)-Pair #4
        { 0x1D, 0xBC }, // (Offset,Value)-Pair #5
        { 0x1E, 0x17 }, // (Offset,Value)-Pair #6
        { 0x1F, 0x04 }, // (Offset,Value)-Pair #7

        // D3DDevice_SetRenderState_LogicOp+0x4F : retn 0x04
        { 0x4F, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x50, 0x04 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_StencilFail
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilFail, 4134, 12)

        // D3DDevice_SetRenderState_StencilFail+0x0C : jb +0x05
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #1
        { 0x0D, 0x05 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_StencilFail+0x38 : cmp [abs], 0x1E00
        { 0x38, 0x81 }, // (Offset,Value)-Pair #3
        { 0x3E, 0x00 }, // (Offset,Value)-Pair #4
        { 0x3F, 0x1E }, // (Offset,Value)-Pair #5

        // D3DDevice_SetRenderState_StencilFail+0x47 : mov dword ptr [eax], 0x41D84
        { 0x47, 0xC7 }, // (Offset,Value)-Pair #6
        { 0x48, 0x00 }, // (Offset,Value)-Pair #7
        { 0x49, 0x84 }, // (Offset,Value)-Pair #8
        { 0x4A, 0x1D }, // (Offset,Value)-Pair #9
        { 0x4B, 0x04 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetRenderState_StencilFail+0x6A : retn 0x04
        { 0x6A, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x6B, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_OcclusionCullEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_OcclusionCullEnable, 4134, 12)

        // D3DDevice_SetRenderState_OcclusionCullEnable+0x15 : jb +0x05
        { 0x15, 0x72 }, // (Offset,Value)-Pair #1
        { 0x16, 0x05 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_OcclusionCullEnable+0x41 : cmp [abs], 0x1E00
        { 0x41, 0x81 }, // (Offset,Value)-Pair #3
        { 0x47, 0x00 }, // (Offset,Value)-Pair #4
        { 0x48, 0x1E }, // (Offset,Value)-Pair #5

        // D3DDevice_SetRenderState_OcclusionCullEnable+0x50 : mov dword ptr [eax], 0x41D84
        { 0x50, 0xC7 }, // (Offset,Value)-Pair #6
        { 0x51, 0x00 }, // (Offset,Value)-Pair #7
        { 0x52, 0x84 }, // (Offset,Value)-Pair #8
        { 0x53, 0x1D }, // (Offset,Value)-Pair #9
        { 0x54, 0x04 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetRenderState_OcclusionCullEnable+0x5F : retn 0x04
        { 0x5F, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x60, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_StencilCullEnable
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_StencilCullEnable, 4134, 13,

    XRefNoSaveIndex,
    XRefOne)

        // D3DDevice_SetRenderState_StencilCullEnable+0x0C : D3D__RenderState[D3DRS_STENCILCULLENABLE]
        XREF_ENTRY( 0x0C, XREF_D3DRS_STENCILCULLENABLE ), // (Offset,Value)-Pair #1

        // D3DDevice_SetRenderState_StencilCullEnable+0x15 : jb +0x05
        { 0x15, 0x72 }, // (Offset,Value)-Pair #2
        { 0x16, 0x05 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetRenderState_StencilCullEnable+0x41 : cmp [abs], 0x1E00
        { 0x41, 0x81 }, // (Offset,Value)-Pair #4
        { 0x47, 0x00 }, // (Offset,Value)-Pair #5
        { 0x48, 0x1E }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_StencilCullEnable+0x50 : mov dword ptr [eax], 0x41D84
        { 0x50, 0xC7 }, // (Offset,Value)-Pair #7
        { 0x51, 0x00 }, // (Offset,Value)-Pair #8
        { 0x52, 0x84 }, // (Offset,Value)-Pair #9
        { 0x53, 0x1D }, // (Offset,Value)-Pair #10
        { 0x54, 0x04 }, // (Offset,Value)-Pair #11

        // D3DDevice_SetRenderState_StencilCullEnable+0x5F : retn 0x04
        { 0x5F, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x60, 0x04 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_FillMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_FillMode, 4134, 11)

        // D3DDevice_SetRenderState_FillMode+0x07 : mov eax, [esi]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x06 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_FillMode+0x21 : mov ecx, [esp+0x08]
        { 0x21, 0x8B }, // (Offset,Value)-Pair #3
        { 0x22, 0x4C }, // (Offset,Value)-Pair #4
        { 0x23, 0x24 }, // (Offset,Value)-Pair #5
        { 0x24, 0x08 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_FillMode+0x25 : jnz +0x02
        { 0x25, 0x75 }, // (Offset,Value)-Pair #7
        { 0x26, 0x02 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetRenderState_FillMode+0x35 : add eax, 0x0C
        { 0x35, 0x83 }, // (Offset,Value)-Pair #9
        { 0x36, 0xC0 }, // (Offset,Value)-Pair #10
        { 0x37, 0x0C }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_Dxt1NoiseEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_Dxt1NoiseEnable, 4134, 9)

        // D3DDevice_SetRenderState_Dxt1NoiseEnable+0x0C : mov eax, [edi+0x2070]
        { 0x0C, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0D, 0x86 }, // (Offset,Value)-Pair #2
        { 0x0E, 0x70 }, // (Offset,Value)-Pair #3
        { 0x0F, 0x20 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_Dxt1NoiseEnable+0x6E : add eax, 0x18
        { 0x6E, 0x83 }, // (Offset,Value)-Pair #5
        { 0x6F, 0xC0 }, // (Offset,Value)-Pair #6
        { 0x70, 0x18 }, // (Offset,Value)-Pair #7

        // D3DDevice_SetRenderState_Dxt1NoiseEnable+0x7C : retn 0x04
        { 0x7C, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x7D, 0x04 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMask
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleMask, 4134, 13)

        // D3DDevice_SetRenderState_MultiSampleMask+0x1C : jb +0x05
        { 0x1C, 0x72 }, // (Offset,Value)-Pair #1
        { 0x1D, 0x05 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_MultiSampleMask+0x38 : and ecx, 0x0F; shl edx, 0x10
        { 0x38, 0x83 }, // (Offset,Value)-Pair #3
        { 0x39, 0xE1 }, // (Offset,Value)-Pair #4
        { 0x3A, 0x0F }, // (Offset,Value)-Pair #5
        { 0x3B, 0xC1 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_MultiSampleMask+0x3E : mov dword ptr [eax], 0x41D7C
        { 0x3E, 0xC7 }, // (Offset,Value)-Pair #7
        { 0x3F, 0x00 }, // (Offset,Value)-Pair #8
        { 0x40, 0x7C }, // (Offset,Value)-Pair #9
        { 0x41, 0x1D }, // (Offset,Value)-Pair #10
        { 0x42, 0x04 }, // (Offset,Value)-Pair #11

        // D3DDevice_SetRenderState_MultiSampleMask+0x4F : retn 0x04
        { 0x4F, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x50, 0x04 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleMode, 4134, 12)

        // D3DDevice_SetRenderState_MultiSampleMode+0x0F : mov ecx, [eax+0x2070]
        { 0x0F, 0x8B }, // (Offset,Value)-Pair #1
        { 0x10, 0x88 }, // (Offset,Value)-Pair #2
        { 0x11, 0x70 }, // (Offset,Value)-Pair #3
        { 0x12, 0x20 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_MultiSampleMode+0x15 : cmp ecx, [eax+0x207C]
        { 0x15, 0x3B }, // (Offset,Value)-Pair #5
        { 0x16, 0x88 }, // (Offset,Value)-Pair #6
        { 0x17, 0x7C }, // (Offset,Value)-Pair #7
        { 0x18, 0x20 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetRenderState_MultiSampleMode+0x1B : jnz +0x0D
        { 0x1B, 0x75 }, // (Offset,Value)-Pair #9
        { 0x1C, 0x0D }, // (Offset,Value)-Pair #10

        // D3DDevice_SetRenderState_MultiSampleMode+0x2A : retn 0x04
        { 0x2A, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x2B, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleRenderTargetMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, 4134, 12)

        // D3DDevice_SetRenderState_MultiSampleRenderTargetMode+0x0F : mov ecx, [eax+0x2070]
        { 0x0F, 0x8B }, // (Offset,Value)-Pair #1
        { 0x10, 0x88 }, // (Offset,Value)-Pair #2
        { 0x11, 0x70 }, // (Offset,Value)-Pair #3
        { 0x12, 0x20 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_MultiSampleRenderTargetMode+0x15 : cmp ecx, [eax+0x207C]
        { 0x15, 0x3B }, // (Offset,Value)-Pair #5
        { 0x16, 0x88 }, // (Offset,Value)-Pair #6
        { 0x17, 0x7C }, // (Offset,Value)-Pair #7
        { 0x18, 0x20 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetRenderState_MultiSampleRenderTargetMode+0x1B : jz +0x0D
        { 0x1B, 0x74 }, // (Offset,Value)-Pair #9
        { 0x1C, 0x0D }, // (Offset,Value)-Pair #10

        // D3DDevice_SetRenderState_MultiSampleRenderTargetMode+0x2A : retn 0x04
        { 0x2A, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x2B, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_ShadowFunc
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_ShadowFunc, 4134, 13)

        // D3DDevice_SetRenderState_ShadowFunc+0x00 : push esi
        { 0x00, 0x56 }, // (Offset,Value)-Pair #1

        // D3DDevice_SetRenderState_ShadowFunc+0x09 : cmp eax, [esi+4]
        { 0x09, 0x3B }, // (Offset,Value)-Pair #2
        { 0x0A, 0x46 }, // (Offset,Value)-Pair #3
        { 0x0B, 0x04 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_ShadowFunc+0x0C : jb +0x05
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #5
        { 0x0D, 0x05 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_ShadowFunc+0x1D : lea edx, [ecx-0x0200]
        { 0x1D, 0x8D }, // (Offset,Value)-Pair #7
        { 0x1E, 0x91 }, // (Offset,Value)-Pair #8
        { 0x1F, 0x00 }, // (Offset,Value)-Pair #9
        { 0x20, 0xFE }, // (Offset,Value)-Pair #10

        // D3DDevice_SetRenderState_ShadowFunc+0x26 : add eax, 0x08
        { 0x26, 0x83 }, // (Offset,Value)-Pair #11
        { 0x27, 0xC0 }, // (Offset,Value)-Pair #12
        { 0x28, 0x08 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_YuvEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_YuvEnable, 4134, 13)

        // D3DDevice_SetRenderState_YuvEnable+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x04 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_YuvEnable+0x10 : mov eax, [esi]
        { 0x10, 0x8B }, // (Offset,Value)-Pair #5
        { 0x11, 0x06 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_YuvEnable+0x12 : cmp eax, [esi+4]
        { 0x12, 0x3B }, // (Offset,Value)-Pair #7
        { 0x13, 0x46 }, // (Offset,Value)-Pair #8
        { 0x14, 0x04 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetRenderState_YuvEnable+0x15 : jb +0x05
        { 0x15, 0x72 }, // (Offset,Value)-Pair #10
        { 0x16, 0x05 }, // (Offset,Value)-Pair #11

        // D3DDevice_SetRenderState_YuvEnable+0x28 : retn 0x04
        { 0x28, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x29, 0x04 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawIndexedVerticesUP
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawIndexedVerticesUP, 4134, 10)

        // D3DDevice_DrawIndexedVerticesUP+0x4A : or edx, 0x800
        { 0x4A, 0x81 }, // (Offset,Value)-Pair #1
        { 0x4B, 0xCA }, // (Offset,Value)-Pair #2
        { 0x4C, 0x00 }, // (Offset,Value)-Pair #3
        { 0x4D, 0x08 }, // (Offset,Value)-Pair #4

        // D3DDevice_DrawIndexedVerticesUP+0x5C : lea edx, [edi+0x6C4]
        { 0x5C, 0x8D }, // (Offset,Value)-Pair #5
        { 0x5D, 0x97 }, // (Offset,Value)-Pair #6
        { 0x5E, 0xC4 }, // (Offset,Value)-Pair #7
        { 0x5F, 0x06 }, // (Offset,Value)-Pair #8

        // D3DDevice_DrawIndexedVerticesUP+0x6A : mov eax, 0x10
        { 0x6A, 0xB8 }, // (Offset,Value)-Pair #9
        { 0x6B, 0x10 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPixelShaderConstant
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPixelShaderConstant, 4134, 10)

        // D3DDevice_SetPixelShaderConstant+0x17 : test eax, eax
        { 0x17, 0x85 }, // (Offset,Value)-Pair #1
        { 0x18, 0xC0 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetPixelShaderConstant+0x32 : add edi, 0x04
        { 0x32, 0x83 }, // (Offset,Value)-Pair #3
        { 0x33, 0xC7 }, // (Offset,Value)-Pair #4
        { 0x34, 0x04 }, // (Offset,Value)-Pair #5

        // D3DDevice_SetPixelShaderConstant+0x47 : jmp +0x07
        { 0x47, 0xEB }, // (Offset,Value)-Pair #6
        { 0x48, 0x07 }, // (Offset,Value)-Pair #7

        // D3DDevice_SetPixelShaderConstant+0xC7 : fstp [esp+??]
        { 0xC7, 0xD9 }, // (Offset,Value)-Pair #8
        { 0xC8, 0x1C }, // (Offset,Value)-Pair #9
        { 0xC9, 0x24 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_DeleteVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DeleteVertexShader, 4134, 6)

        { 0x02, 0x24 },
        { 0x06, 0xFF },
        { 0x0A, 0x08 },
        { 0x0E, 0x44 },
        { 0x16, 0xC2 },
        { 0x17, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetScreenSpaceOffset
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetScreenSpaceOffset, 4134, 7)

	{ 0x06, 0x56 },
	{ 0x07, 0xD8 },
	{ 0x08, 0x44 },
	{ 0x09, 0x24 },
	{ 0x0A, 0x08 },
	{ 0x2E, 0x8B },
	{ 0x2F, 0x06 },
OOVPA_END;

// ******************************************************************
// * D3D::SetFence
// ******************************************************************
OOVPA_XREF(D3D_SetFence, 4134, 12,

    XREF_D3D_SETFENCE,
    XRefZero)

	{ 0x0D, 0x72 },
	{ 0x0E, 0x0E },
	{ 0x31, 0xBA },
	{ 0x32, 0x90 },
	{ 0x33, 0x1D },
	{ 0x34, 0x04 },
	{ 0x35, 0x00 },
	{ 0x45, 0x83 },
	{ 0x46, 0xE1 },
	{ 0x47, 0x3F },
        { 0xAA, 0xC2 },
	{ 0xAB, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_LineWidth
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_LineWidth, 4134, 7)

        { 0x0C, 0x89 },
        { 0x18, 0x04 },
        { 0x28, 0xD9 },
        { 0x32, 0x81 },
        { 0x3F, 0x8B },
        { 0x4C, 0x00 },
        { 0x59, 0x06 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_PSTextureModes
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_PSTextureModes, 4134, 10)
		{ 0x0A, 0x89 },
		{ 0x0B, 0x81 },
		{ 0x0E, 0x00 },
		{ 0x0F, 0x00 },
		{ 0x10, 0x81 },
		{ 0x11, 0x0D },
		{ 0x16, 0x00 },
		{ 0x17, 0x40 },
		{ 0x1A, 0xA3 },
		{ 0x1F, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BlockUntilVerticalBlank
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BlockUntilVerticalBlank, 4134, 11)

        // D3DDevice_BlockUntilVerticalBlank+0x05 : push 0; push 0; push 1
        { 0x05, 0x6A }, // (Offset,Value)-Pair #1
        { 0x06, 0x00 }, // (Offset,Value)-Pair #2
        { 0x07, 0x6A }, // (Offset,Value)-Pair #3
        { 0x08, 0x00 }, // (Offset,Value)-Pair #4
        { 0x09, 0x6A }, // (Offset,Value)-Pair #5
        { 0x0A, 0x01 }, // (Offset,Value)-Pair #6

        // D3DDevice_BlockUntilVerticalBlank+0x17 : add eax, 0x2430
        { 0x17, 0x05 }, // (Offset,Value)-Pair #7
        { 0x18, 0x30 }, // (Offset,Value)-Pair #8
        { 0x19, 0x24 }, // (Offset,Value)-Pair #9

        // D3DDevice_BlockUntilVerticalBlank+0x1D : call [KrnlImport]
        { 0x1D, 0xFF }, // (Offset,Value)-Pair #10

        // D3DDevice_BlockUntilVerticalBlank+0x23 : retn
        { 0x23, 0xC3 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetSwapCallback
// ******************************************************************
/* This may look the same as 4361, but accuracy is not gauranteed.
   It might be a SetVerticalBlankCallback that has an identical signature... */
OOVPA_NO_XREF(D3DDevice_SetSwapCallback, 4134, 12)

        // D3DDevice_SetVerticalBlankCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x04 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetVerticalBlankCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #5
        { 0x05, 0x0D }, // (Offset,Value)-Pair #6

        // D3DDevice_SetVerticalBlankCallback+0x0A : mov [ecx+0x242C], eax
        { 0x0A, 0x89 }, // (Offset,Value)-Pair #7
        { 0x0B, 0x81 }, // (Offset,Value)-Pair #8
        { 0x0C, 0x2C }, // (Offset,Value)-Pair #9
        { 0x0D, 0x24 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetVerticalBlankCallback+0x10 : retn 0x04
        { 0x10, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x11, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetTile
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetTile, 4134, 11)

        // D3DDevice_GetTile+0x13 : lea esi, [ecx+eax*8+0x211C]
        { 0x13, 0x8D }, // (Offset,Value)-Pair #1
        { 0x14, 0xB4 }, // (Offset,Value)-Pair #2
        { 0x15, 0xC1 }, // (Offset,Value)-Pair #3
        { 0x16, 0x1C }, // (Offset,Value)-Pair #4
        { 0x17, 0x21 }, // (Offset,Value)-Pair #5

        // D3DDevice_GetTile+0x1A : mov ecx, 0x06
        { 0x1A, 0xB9 }, // (Offset,Value)-Pair #6
        { 0x1B, 0x06 }, // (Offset,Value)-Pair #7

        // D3DDevice_GetTile+0x21 : pop edi; pop esi
        { 0x21, 0x5F }, // (Offset,Value)-Pair #8
        { 0x22, 0x5E }, // (Offset,Value)-Pair #9

        // D3DDevice_GetTile+0x23 : retn 0x08
        { 0x23, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x24, 0x08 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTile
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTile, 4134, 11)

        // D3DDevice_SetTile+0x00 : sub esp, 0x18
        { 0x00, 0x83 }, // (Offset,Value)-Pair #1
        { 0x01, 0xEC }, // (Offset,Value)-Pair #2
        { 0x02, 0x18 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetTile+0x16 : cmp [esi+4], eax
        { 0x16, 0x39 }, // (Offset,Value)-Pair #4
        { 0x17, 0x46 }, // (Offset,Value)-Pair #5
        { 0x18, 0x04 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetTile+0x3D : lea edi, [edx+ecx*8+0x211C]
        { 0x3D, 0x8D }, // (Offset,Value)-Pair #7
        { 0x3E, 0xBC }, // (Offset,Value)-Pair #8
        { 0x3F, 0xCB }, // (Offset,Value)-Pair #9
        { 0x40, 0x1C }, // (Offset,Value)-Pair #10
        { 0x41, 0x21 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_IsBusy
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_IsBusy, 4134, 7)

        { 0x05, 0x8B },
        { 0x0C, 0x90 },
        { 0x13, 0x44 },
        { 0x1A, 0xFF },
        { 0x21, 0x01 },
        { 0x28, 0x00 },
        { 0x2F, 0xD2 },
OOVPA_END;

// ******************************************************************
// * D3D::CDevice::KickOff
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_KickOff, 4134, 12)

	// D3D::CDevice::KickOff+0x0D : test al, 4
	{ 0x0D, 0xA8 },
	{ 0x0E, 0x04 },
	// D3D::CDevice::KickOff+0x11 : mov ecx, [ecx+0x35C]
	{ 0x11, 0x8B },
	{ 0x12, 0x89 },
	{ 0x13, 0x5C },
	{ 0x14, 0x03 },
	{ 0x15, 0x00 },
	{ 0x16, 0x00 },
	// D3D::CDevice::KickOff+0x26 : sfence
	{ 0x26, 0x0F },
	{ 0x27, 0xAE },
	{ 0x28, 0xF8 },
	// D3D::CDevice::KickOff+0xE3 : retn
	{ 0xE3, 0xC3 }
OOVPA_END;

// ******************************************************************
// * D3DDevice_BlockOnFence
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BlockOnFence, 4134, 7)

        { 0x00, 0x8B },
        { 0x02, 0x24 },
        { 0x04, 0x6A },
        { 0x06, 0x50 },
        { 0x0C, 0xC2 },
        { 0x0D, 0x04 },
        { 0x0E, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetRenderTarget
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetRenderTarget, 4134, 11)

        // D3DDevice_GetRenderTarget+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // D3DDevice_GetRenderTarget+0x05 : mov eax, [eax + 0x2070]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x80 }, // (Offset,Value)-Pair #3
        { 0x07, 0x70 }, // (Offset,Value)-Pair #4
        { 0x08, 0x20 }, // (Offset,Value)-Pair #5
        { 0x09, 0x00 }, // (Offset,Value)-Pair #6
        { 0x0A, 0x00 }, // (Offset,Value)-Pair #7

        // D3DDevice_GetRenderTarget+0x0B : test    eax, eax
        { 0x0B, 0x85 }, // (Offset,Value)-Pair #8
        { 0x0C, 0xC0 }, // (Offset,Value)-Pair #9

        // D3DDevice_GetRenderTarget+0x1D : retn 0x04
        { 0x1D, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x1E, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_Release
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Release, 4134, 11)

        // D3DDevice_Release+0x07 : mov eax, [edi+0x043C]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x87 }, // (Offset,Value)-Pair #2
        { 0x09, 0x3C }, // (Offset,Value)-Pair #3
        { 0x0A, 0x04 }, // (Offset,Value)-Pair #4

        // D3DDevice_Release+0x0D : cmp eax, 1
        { 0x0D, 0x83 }, // (Offset,Value)-Pair #5
        { 0x0E, 0xF8 }, // (Offset,Value)-Pair #6
        { 0x0F, 0x01 }, // (Offset,Value)-Pair #7

        // D3DDevice_Release+0x1B : mov ecx, 0x0AB8
        { 0x1B, 0xB9 }, // (Offset,Value)-Pair #8
        { 0x1C, 0xB8 }, // (Offset,Value)-Pair #9
        { 0x1D, 0x0A }, // (Offset,Value)-Pair #10

        // D3DDevice_Release+0x2E : dec eax
        { 0x2E, 0x48 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetDisplayMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetDisplayMode, 4134, 13)

        // D3DDevice_GetDisplayMode+0x01 : mov esi, ds:[*]
        { 0x01, 0x8B }, // (Offset,Value)-Pair #1
        { 0x02, 0x35 }, // (Offset,Value)-Pair #2

        // D3DDevice_GetDisplayMode+0x18 : shr ecx, 0x14
        { 0x18, 0xC1 }, // (Offset,Value)-Pair #3
        { 0x19, 0xE9 }, // (Offset,Value)-Pair #4
        { 0x1A, 0x14 }, // (Offset,Value)-Pair #5

        // D3DDevice_GetDisplayMode+0x50 : shr eax, 0x0C
        { 0x50, 0xC1 }, // (Offset,Value)-Pair #6
        { 0x51, 0xE8 }, // (Offset,Value)-Pair #7
        { 0x52, 0x0C }, // (Offset,Value)-Pair #8

        // D3DDevice_GetDisplayMode+0x6B : and eax, 0x0A
        { 0x6B, 0x83 }, // (Offset,Value)-Pair #9
        { 0x6C, 0xE0 }, // (Offset,Value)-Pair #10
        { 0x6D, 0x0A }, // (Offset,Value)-Pair #11

        // D3DDevice_GetDisplayMode+0x92 : retn 0x04
        { 0x92, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x93, 0x04 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetShaderConstantMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetShaderConstantMode, 4134, 7)

        { 0x1D, 0xFD },
        { 0x3C, 0x8B },
        { 0x5B, 0x0B },
        { 0x7A, 0x56 },
        { 0x99, 0xC0 },
        { 0xB8, 0x00 },
        { 0xD7, 0xC7 },
OOVPA_END;

// ******************************************************************
// * D3D::LazySetPointParams
// ******************************************************************
OOVPA_NO_XREF(D3D_LazySetPointParams, 4134, 7)
	{ 0x00, 0x83 },
	{ 0x01, 0xEC },
	{ 0x02, 0x14 },
	{ 0x4E, 0xE0 },
	{ 0x73, 0xF6 },
	{ 0x74, 0xC4 },
	{ 0x75, 0x41 },
OOVPA_END;


// ******************************************************************
// * D3DDevice_SetVertexData2f
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData2f, 4134, 8)
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
// * D3DDevice_SetVertexData4f
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData4f, 4134, 7)
	{ 0x0A, 0x46 },
	{ 0x16, 0x08 },
	{ 0x22, 0x09 },
	{ 0x2E, 0x24 },
	{ 0x3A, 0x24 },
	{ 0x46, 0x8B },
	{ 0x52, 0x14 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetFlickerFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetFlickerFilter, 4134, 11)

        // D3DDevice_SetFlickerFilter+0x1C : mov eax, [eax+0x2268]
        { 0x1D, 0x80 }, // (Offset,Value)-Pair #2
        { 0x1E, 0x68 }, // (Offset,Value)-Pair #3
        { 0x1F, 0x22 }, // (Offset,Value)-Pair #4

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
// * D3DDevice_SetSoftDisplayFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetSoftDisplayFilter, 4134, 12)

        // D3DDevice_SetSoftDisplayFilter+0x25 : mov edx, [eax+0x2268]
        { 0x25, 0x8B }, // (Offset,Value)-Pair #1
        { 0x26, 0x90 }, // (Offset,Value)-Pair #2
        { 0x27, 0x68 }, // (Offset,Value)-Pair #3
        { 0x28, 0x22 }, // (Offset,Value)-Pair #4

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
// * D3DDevice_SetPalette
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPalette, 4134, 12)

        // D3DDevice_SetPalette+0x0D : mov eax, [edi+esi*4+0x0A88]
        { 0x0D, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0E, 0x84 }, // (Offset,Value)-Pair #2
        { 0x0F, 0xBE }, // (Offset,Value)-Pair #3
        { 0x10, 0x88 }, // (Offset,Value)-Pair #4
        { 0x11, 0x0A }, // (Offset,Value)-Pair #5

        // D3DDevice_SetPalette+0x76 : add edi, 0x41B20
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
// * D3DDevice_EnableOverlay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_EnableOverlay, 4134, 11)

        // D3DDevice_EnableOverlay+0x0B : mov ecx, [eax+0x8700]
        { 0x0B, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0C, 0x88 }, // (Offset,Value)-Pair #2
        { 0x0E, 0x87 }, // (Offset,Value)-Pair #3

        // D3DDevice_EnableOverlay+0x15 : jz +0x0A
        { 0x15, 0x74 }, // (Offset,Value)-Pair #4
        { 0x16, 0x0A }, // (Offset,Value)-Pair #5

        // D3DDevice_EnableOverlay+0x5A : mov [eax+0x8918], ecx
        { 0x5A, 0x89 }, // (Offset,Value)-Pair #6
        { 0x5B, 0x88 }, // (Offset,Value)-Pair #7
        { 0x5C, 0x18 }, // (Offset,Value)-Pair #8
        { 0x5D, 0x89 }, // (Offset,Value)-Pair #9

        // D3DDevice_EnableOverlay+0x60 : retn 0x04
        { 0x60, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x61, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_UpdateOverlay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_UpdateOverlay, 4134, 11)

        // D3DDevice_UpdateOverlay+0x0F : mov [eax+0x2A88], ecx
        { 0x0F, 0x89 }, // (Offset,Value)-Pair #1
        { 0x10, 0x88 }, // (Offset,Value)-Pair #2
        { 0x11, 0x88 }, // (Offset,Value)-Pair #3
        { 0x12, 0x2A }, // (Offset,Value)-Pair #4

        // D3DDevice_UpdateOverlay+0x86 : and ecx, 0xFFFFFFFE
        { 0x86, 0x83 }, // (Offset,Value)-Pair #5
        { 0x87, 0xE1 }, // (Offset,Value)-Pair #6
        { 0x88, 0xFE }, // (Offset,Value)-Pair #7

        // D3DDevice_UpdateOverlay+0xA2 : mov [esi+0x8920], ecx
        { 0xA2, 0x89 }, // (Offset,Value)-Pair #8
        { 0xA3, 0x8E }, // (Offset,Value)-Pair #9
        { 0xA4, 0x20 }, // (Offset,Value)-Pair #10
        { 0xA5, 0x89 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetModelView
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetModelView, 4134, 7)

        { 0x05, 0x57 },
        { 0x0A, 0x85 },
        { 0x10, 0xB0 },
        { 0x16, 0x10 },
        { 0x1C, 0x5E },
        { 0x22, 0xC1 },
        { 0x28, 0xE0 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_FlushVertexCache
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_FlushVertexCache, 4134, 10)

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
// * D3D8_4134
// ******************************************************************
OOVPATable D3D8_4134[] = {

	REGISTER_OOVPA(Direct3D_CreateDevice, 3925, PATCH),
	REGISTER_OOVPA(D3D_CheckDeviceFormat, 4134, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetTextureState_TexCoordIndex, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_CullMode, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTransform, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FogColor, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_Swap, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_Clear, 4134, PATCH),

    // ********************** BEG WARNING UNTESTED!!! *******************
	REGISTER_OOVPA(D3DDevice_GetDeviceCaps, 3925, DISABLED),
	REGISTER_OOVPA(D3DDevice_BeginVisibilityTest, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_EndVisibilityTest, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_LoadVertexShader, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVerticalBlankCallback, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetViewport, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_Begin, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData2f, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4f, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexDataColor, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_End, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_CreatePalette, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPalette, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateTexture, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVolumeTexture, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateCubeTexture, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateIndexBuffer, 3925, DISABLED),
	REGISTER_OOVPA(D3DDevice_CreateVertexShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetFlickerFilter, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetSoftDisplayFilter, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BorderColor, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_PSTextureModes, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZBias, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_Present, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVertexBuffer, 3925, DISABLED),
	REGISTER_OOVPA(D3DResource_Register, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_Release, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_AddRef, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_IsBusy, 3925, PATCH),
	REGISTER_OOVPA(D3DPalette_Lock, 3925, PATCH),
	REGISTER_OOVPA(D3DTexture_LockRect, 3925, PATCH),
	REGISTER_OOVPA(D3DVolumeTexture_LockBox, 3925, PATCH),	// Just calls Lock3DSurface
	REGISTER_OOVPA(D3DCubeTexture_LockRect, 3925, PATCH),	// Just calls Lock2DSurface
	REGISTER_OOVPA(D3DTexture_GetSurfaceLevel, 3925, PATCH),

    // ********************** END WARNING UNTESTED!!! *******************

	REGISTER_OOVPA(D3DDevice_GetBackBuffer, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDepthStencilSurface, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderTarget, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_CreatePixelShader, 3925, PATCH),
	REGISTER_OOVPA(D3DSurface_LockRect, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteVertexShader, 3925, PATCH),
	REGISTER_OOVPA(Lock2DSurface, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetGammaRamp, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SelectVertexShader, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_CopyRects, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateImageSurface, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShader, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BumpEnv, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetIndices, 4134, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetTexture, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_VertexBlend, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TextureFactor, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_EdgeAntiAlias, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Simple, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleAntiAlias, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTransform, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetStreamSource, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShader, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawVertices, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawVerticesUP, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawIndexedVertices, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_GetLight, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetLight, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetMaterial, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_LightEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DVertexBuffer_Lock, 4034, PATCH),
	REGISTER_OOVPA(Get2DSurfaceDesc, 4134, PATCH),
	REGISTER_OOVPA(D3DSurface_GetDesc, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilFail, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_NormalizeNormals, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_Reset, 4134, PATCH),
	REGISTER_OOVPA(D3D_KickOffAndWaitForIdle, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TwoSidedLighting, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_BackFillMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_ColorKeyColor, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FrontFace, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LogicOp, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_OcclusionCullEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilCullEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FillMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZCmpAlwaysRead, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZRead, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_DoNotCullUncompressed, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Dxt1NoiseEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMask, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ShadowFunc, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_YuvEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawIndexedVerticesUP, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShaderConstant, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_DeletePixelShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteVertexShader, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScreenSpaceOffset, 4134, PATCH),
	REGISTER_OOVPA(D3D_SetFence, 4134, XREF),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LineWidth, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetGammaRamp, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_BlockUntilVerticalBlank, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetSwapCallback, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTile, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTile, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_IsBusy, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderSize, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_KickOff, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_InsertFence, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_BlockOnFence, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetRenderTarget, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_Release, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDisplayMode, 4134, PATCH),
	REGISTER_OOVPA(D3D_GetAdapterIdentifier, 3925, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetShaderConstantMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetCreationParameters, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVisibilityTestResult, 3925, PATCH),
	REGISTER_OOVPA(D3D_LazySetPointParams, 4134, PATCH),
	REGISTER_OOVPA(D3D_SetPushBufferSize, 3925, DISABLED),
	REGISTER_OOVPA(D3DDevice_EnableOverlay, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_UpdateOverlay, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetModelView, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_FlushVertexCache, 4134, PATCH),
};

// ******************************************************************
// * D3D8_4134_SIZE
// ******************************************************************
uint32 D3D8_4134_SIZE = sizeof(D3D8_4134);
