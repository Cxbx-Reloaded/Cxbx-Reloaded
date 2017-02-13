// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->D3D8.1.0.4361.cpp
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
// * D3D_CreateDevice
// ******************************************************************
OOVPA_NO_XREF(D3D_CreateDevice, 4361, 8)
        // D3D_CreateDevice+0x07 : jnz +0x0A
        { 0x07, 0x75 }, // (Offset,Value)-Pair #1
        { 0x08, 0x0A }, // (Offset,Value)-Pair #2

        // D3D_CreateDevice+0x86 : repe stosd
        { 0x86, 0xF3 }, // (Offset,Value)-Pair #3
        { 0x87, 0xAB }, // (Offset,Value)-Pair #4

        // D3D_CreateDevice+0x89 : mov eax, esi
        { 0x89, 0x8B }, // (Offset,Value)-Pair #5
        { 0x8A, 0xC6 }, // (Offset,Value)-Pair #6

        // D3D_CreateDevice+0xA0 : retn 0x18
        { 0xA0, 0xC2 }, // (Offset,Value)-Pair #7
        { 0xA1, 0x18 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * D3D_CheckDeviceFormat
// ******************************************************************
OOVPA_NO_XREF(D3D_CheckDeviceFormat, 4361, 8)

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
// * D3DDevice_SetVerticalBlankCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVerticalBlankCallback, 4361, 6)

	{ 0x01, 0x44 },
	{ 0x03, 0x04 },
	{ 0x05, 0x0D },
	{ 0x0C, 0x30 },
	{ 0x0D, 0x24 },
	{ 0x11, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3D_GetAdapterModeCount
// ******************************************************************
OOVPA_NO_XREF(D3D_GetAdapterModeCount, 4361, 12)

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
// * D3D_GetAdapterDisplayMode
// ******************************************************************
OOVPA_NO_XREF(D3D_GetAdapterDisplayMode, 4361, 13)

        // D3D_GetAdapterDisplayMode+0x08 : mov eax, 0x8876086C
        { 0x08, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x09, 0x6C }, // (Offset,Value)-Pair #2
        { 0x0A, 0x08 }, // (Offset,Value)-Pair #3
        { 0x0B, 0x76 }, // (Offset,Value)-Pair #4
        { 0x0C, 0x88 }, // (Offset,Value)-Pair #5

        // D3D_GetAdapterDisplayMode+0x18 : jnz +0x17
        { 0x18, 0x75 }, // (Offset,Value)-Pair #6
        { 0x19, 0x17 }, // (Offset,Value)-Pair #7

        // D3D_GetAdapterDisplayMode+0x31 : mov ecx, [edx+0x2080]
        { 0x31, 0x8B }, // (Offset,Value)-Pair #8
        { 0x32, 0x8A }, // (Offset,Value)-Pair #9
        { 0x33, 0x80 }, // (Offset,Value)-Pair #10
        { 0x34, 0x20 }, // (Offset,Value)-Pair #11

        // D3D_GetAdapterDisplayMode+0xBD : retn 0x08
        { 0xBD, 0xC2 }, // (Offset,Value)-Pair #12
        { 0xBE, 0x08 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3D_EnumAdapterModes
// ******************************************************************
OOVPA_NO_XREF(D3D_EnumAdapterModes, 4361, 10)

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
// * D3DDevice_SelectVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SelectVertexShader, 4361, 11)

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
// * D3DDevice_AddRef
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_AddRef, 4361, 10)

        // D3DDevice_AddRef+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // D3DDevice_AddRef+0x05 : mov ecx, [eax+0x0440]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x88 }, // (Offset,Value)-Pair #3
        { 0x07, 0x40 }, // (Offset,Value)-Pair #4
        { 0x08, 0x04 }, // (Offset,Value)-Pair #5

        // D3DDevice_AddRef+0x0B : inc ecx
        { 0x0B, 0x41 }, // (Offset,Value)-Pair #6

        // D3DDevice_AddRef+0x05 : mov [eax+0x0440], ecx
        { 0x0C, 0x89 }, // (Offset,Value)-Pair #7
        { 0x0D, 0x88 }, // (Offset,Value)-Pair #8
        { 0x0E, 0x40 }, // (Offset,Value)-Pair #9
        { 0x0F, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3D_ClearStateBlockFlags
// ******************************************************************
OOVPA_XREF(D3D_ClearStateBlockFlags, 4361, 9,

    XREF_CLEARSTATEBLOCKFLAGS,
    XRefZero)

        // D3D_ClearStateBlockFlags+0x0A : movzx ecx, 0x82
        { 0x0A, 0xB9 }, // (Offset,Value)-Pair #1
        { 0x0B, 0x82 }, // (Offset,Value)-Pair #2

        // D3D_ClearStateBlockFlags+0x16 : mov ecx, [edx+0x0390]
        { 0x16, 0x8B }, // (Offset,Value)-Pair #3
        { 0x17, 0x8A }, // (Offset,Value)-Pair #4
        { 0x18, 0x90 }, // (Offset,Value)-Pair #5
        { 0x19, 0x03 }, // (Offset,Value)-Pair #6

        // D3D_ClearStateBlockFlags+0x2F : add ecx, 0x90
        { 0x2F, 0x81 }, // (Offset,Value)-Pair #7
        { 0x30, 0xC1 }, // (Offset,Value)-Pair #8
        { 0x31, 0x90 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3D_RecordStateBlock
// ******************************************************************
OOVPA_XREF(D3D_RecordStateBlock, 4361, 10,

    XREF_RECORDSTATEBLOCK,
    XRefZero)

        // D3D_RecordStateBlock+0x0F : mov eax, [edi+0x0390]
        { 0x0F, 0x8B }, // (Offset,Value)-Pair #1
        { 0x10, 0x87 }, // (Offset,Value)-Pair #2
        { 0x11, 0x90 }, // (Offset,Value)-Pair #3
        { 0x12, 0x03 }, // (Offset,Value)-Pair #4

        // D3D_RecordStateBlock+0x95 : add ebx, ecx
        { 0x95, 0x03 }, // (Offset,Value)-Pair #5
        { 0x96, 0xD9 }, // (Offset,Value)-Pair #6

        // D3D_RecordStateBlock+0xD5 : mov dword ptr [esi+0x0C], 1
        { 0xD5, 0xC7 }, // (Offset,Value)-Pair #7
        { 0xD6, 0x46 }, // (Offset,Value)-Pair #8
        { 0xD7, 0x0C }, // (Offset,Value)-Pair #9
        { 0xD8, 0x01 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetGammaRamp
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetGammaRamp, 4361, 9)

        // D3DDevice_SetGammaRamp+0x11 : and eax, 1; push ebx
        { 0x11, 0x83 }, // (Offset,Value)-Pair #1
        { 0x12, 0xE0 }, // (Offset,Value)-Pair #2
        { 0x13, 0x01 }, // (Offset,Value)-Pair #3
        { 0x14, 0x53 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetGammaRamp+0x2F : rep movsd
        { 0x2F, 0xF3 }, // (Offset,Value)-Pair #5
        { 0x30, 0xA5 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetGammaRamp+0x3E : push ebx; mov ecx, edx
        { 0x3E, 0x53 }, // (Offset,Value)-Pair #7
        { 0x3F, 0x8B }, // (Offset,Value)-Pair #8
        { 0x40, 0xCA }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3DDevice_BeginStateBlock
// ******************************************************************
OOVPA_XREF(D3DDevice_BeginStateBlock, 4361, 6,

    XRefNoSaveIndex,
    XRefOne)

        // D3DDevice_BeginStateBlock+0x0F : call [ClearStateBlockFlags]
        XREF_ENTRY( 0x0A, XREF_CLEARSTATEBLOCKFLAGS ), // (Offset,Value)-Pair #1

        // D3DDevice_BeginStateBlock+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #2

        // D3DDevice_BeginStateBlock+0x05 : mov [eax+8], 0x20
        { 0x05, 0x83 }, // (Offset,Value)-Pair #3
        { 0x06, 0x48 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5
        { 0x08, 0x20 }, // (Offset,Value)-Pair #6
OOVPA_END;

// ******************************************************************
// * D3DDevice_CaptureStateBlock
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CaptureStateBlock, 4361, 9)

        // D3DDevice_CaptureStateBlock+0x36 : mov eax, [edi+eax*4+0x0A78]
        { 0x36, 0x8B }, // (Offset,Value)-Pair #1
        { 0x37, 0x84 }, // (Offset,Value)-Pair #2
        { 0x38, 0x87 }, // (Offset,Value)-Pair #3
        { 0x39, 0x78 }, // (Offset,Value)-Pair #4
        { 0x3A, 0x0A }, // (Offset,Value)-Pair #5

        // D3DDevice_CaptureStateBlock+0xA9 : cmp dword ptr [esi+0x0C], 0
        { 0xA9, 0x83 }, // (Offset,Value)-Pair #6
        { 0xAA, 0x7E }, // (Offset,Value)-Pair #7
        { 0xAB, 0x0C }, // (Offset,Value)-Pair #8
        { 0xAC, 0x00 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3DDevice_DeleteStateBlock
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DeleteStateBlock, 4361, 7)

	{ 0x11, 0x76 },
	{ 0x24, 0x3B },
	{ 0x37, 0xE8 },
	{ 0x4A, 0x50 },
	{ 0x5D, 0x74 },
	{ 0x70, 0x06 },
	{ 0x83, 0xEB },
OOVPA_END;

// ******************************************************************
// * D3DDevice_ApplyStateBlock
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_ApplyStateBlock, 4361, 9)

        // D3DDevice_ApplyStateBlock+0x0E : lea esi, [edi+0x3C]
        { 0x0E, 0x8D }, // (Offset,Value)-Pair #1
        { 0x0F, 0x77 }, // (Offset,Value)-Pair #2
        { 0x10, 0x3C }, // (Offset,Value)-Pair #3

        // D3DDevice_ApplyStateBlock+0x34 : cmp [edi+8], ebp
        { 0x34, 0x39 }, // (Offset,Value)-Pair #4
        { 0x35, 0x6F }, // (Offset,Value)-Pair #5
        { 0x36, 0x08 }, // (Offset,Value)-Pair #6

        // D3DDevice_ApplyStateBlock+0x9E : sub eax, 0x60
        { 0x9E, 0x83 }, // (Offset,Value)-Pair #7
        { 0x9F, 0xE8 }, // (Offset,Value)-Pair #8
        { 0xA0, 0x60 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3DDevice_EndStateBlock
// ******************************************************************
OOVPA_XREF(D3DDevice_EndStateBlock, 4361, 6,

    XRefNoSaveIndex,
    XRefOne)

        // D3DDevice_EndStateBlock+0x0F : call [ClearStateBlockFlags]
        XREF_ENTRY( 0x0A, XREF_RECORDSTATEBLOCK ), // (Offset,Value)-Pair #1

        // D3DDevice_EndStateBlock+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #2

        // D3DDevice_EndStateBlock+0x05 : and [eax+8], 0xFFFFFFDF
        { 0x05, 0x83 }, // (Offset,Value)-Pair #3
        { 0x06, 0x60 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5
        { 0x08, 0xDF }, // (Offset,Value)-Pair #6
OOVPA_END;

// ******************************************************************
// * D3DDevice_CopyRects
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CopyRects, 4361, 10)

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
OOVPA_NO_XREF(D3DDevice_CreateImageSurface, 4361, 9)

        // D3DDevice_CreateImageSurface+0x2A : push 0x18
        { 0x2A, 0x6A }, // (Offset,Value)-Pair #1
        { 0x2B, 0x18 }, // (Offset,Value)-Pair #2

        // D3DDevice_CreateImageSurface+0x2C : push 0x40
        { 0x2C, 0x6A }, // (Offset,Value)-Pair #3
        { 0x2D, 0x40 }, // (Offset,Value)-Pair #4

        // D3DDevice_CreateImageSurface+0x3B : push 0x0404
        { 0x3B, 0x68 }, // (Offset,Value)-Pair #5
        { 0x3C, 0x04 }, // (Offset,Value)-Pair #6
        { 0x3D, 0x04 }, // (Offset,Value)-Pair #7

        // D3DDevice_CreateImageSurface+0x61 : retn 0x10
        { 0x61, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x62, 0x10 }, // (Offset,Value)-Pair #9
OOVPA_END;


// ******************************************************************
// * D3DDevice_GetRenderTarget
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetRenderTarget, 4361, 9)

        // D3DDevice_GetRenderTarget+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // D3DDevice_GetRenderTarget+0x05 : mov eax, [eax + 0x2070]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x80 }, // (Offset,Value)-Pair #3
        { 0x07, 0x70 }, // (Offset,Value)-Pair #4
        { 0x08, 0x20 }, // (Offset,Value)-Pair #5

        // D3DDevice_GetRenderTarget+0x11 : mov [ecx], eax
        { 0x11, 0x89 }, // (Offset,Value)-Pair #6
        { 0x12, 0x01 }, // (Offset,Value)-Pair #7

        // D3DDevice_GetRenderTarget+0x1D : retn 0x04
        { 0x1D, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x1E, 0x04 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShaderConstant
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShaderConstant, 4361, 10)

        // D3DDevice_SetVertexShaderConstant+0x11 : mov cl, [ebx+0x08]
        { 0x11, 0x8A }, // (Offset,Value)-Pair #1
        { 0x12, 0x4B }, // (Offset,Value)-Pair #2
        { 0x13, 0x08 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetVertexShaderConstant+0x1A : test cl, 0x10
        { 0x1A, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x1B, 0xC1 }, // (Offset,Value)-Pair #5
        { 0x1C, 0x10 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetVertexShaderConstant+0x60 : jle +0x19
        { 0x60, 0x7E }, // (Offset,Value)-Pair #7
        { 0x61, 0x19 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetVertexShaderConstant+0x98 : retn 0x0C
        { 0x98, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x99, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPixelShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPixelShader, 4361, 10)

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
// * D3DDevice_GetViewport
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetViewport, 4361, 10)

        // D3DDevice_GetViewport+0x05 : push esi; push edi
        { 0x05, 0x56 }, // (Offset,Value)-Pair #1
        { 0x06, 0x57 }, // (Offset,Value)-Pair #2

        // D3DDevice_GetViewport+0x0B : lea esi, [eax+0x9D0]
        { 0x0B, 0x8D }, // (Offset,Value)-Pair #3
        { 0x0C, 0xB0 }, // (Offset,Value)-Pair #4
        { 0x0D, 0xD0 }, // (Offset,Value)-Pair #5
        { 0x0E, 0x09 }, // (Offset,Value)-Pair #6

        // D3DDevice_GetViewport+0x11 : mov ecx, 6
        { 0x11, 0xB9 }, // (Offset,Value)-Pair #7
        { 0x12, 0x06 }, // (Offset,Value)-Pair #8

        // D3DDevice_GetViewport+0x1A : retn 0x04
        { 0x1A, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x1B, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_BumpEnv
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTextureState_BumpEnv, 4361, 12)

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
// * D3DDevice_SetTextureState_BorderColor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTextureState_BorderColor, 4361, 15)

        // D3DDevice_SetTextureState_BorderColor+0x0C : jb +0x05
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #1
        { 0x0D, 0x05 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetTextureState_BorderColor+0x19 : shl edx, 6
        { 0x19, 0xC1 }, // (Offset,Value)-Pair #3
        { 0x1A, 0xE2 }, // (Offset,Value)-Pair #4
        { 0x1B, 0x06 }, // (Offset,Value)-Pair #5

        // D3DDevice_SetTextureState_BorderColor+0x2B : add eax, 8; mov [esi], eax; shl ecx, 7
        { 0x2B, 0x83 }, // (Offset,Value)-Pair #6
        { 0x2C, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x2D, 0x08 }, // (Offset,Value)-Pair #8
        { 0x2E, 0x89 }, // (Offset,Value)-Pair #9
        { 0x2F, 0x06 }, // (Offset,Value)-Pair #10
        { 0x30, 0xC1 }, // (Offset,Value)-Pair #11
        { 0x31, 0xE1 }, // (Offset,Value)-Pair #12
        { 0x32, 0x07 }, // (Offset,Value)-Pair #13

        // D3DDevice_SetTextureState_BorderColor+0x3A : retn 0x08
        { 0x3A, 0xC2 }, // (Offset,Value)-Pair #14
        { 0x3B, 0x08 }, // (Offset,Value)-Pair #15
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetIndices
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetIndices, 4361, 11)

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
// * D3DDevice_SetTexture
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTexture, 4361, 10)

        // D3DDevice_SetTexture+0x09 : push edi
        { 0x09, 0x57 }, // (Offset,Value)-Pair #1

        // D3DDevice_SetTexture+0x21 : jz +0x20
        { 0x21, 0x74 }, // (Offset,Value)-Pair #2
        { 0x22, 0x20 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetTexture+0x33 : test ecx, 0x78FFFF
        { 0x33, 0xF7 }, // (Offset,Value)-Pair #4
        { 0x34, 0xC1 }, // (Offset,Value)-Pair #5
        { 0x37, 0x78 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetTexture+0x39 : mov [eax], edx
        { 0x39, 0x89 }, // (Offset,Value)-Pair #7
        { 0x3A, 0x10 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetTexture+0xC9 : jz +0x4D
        { 0xC9, 0x74 }, // (Offset,Value)-Pair #9
        { 0xCA, 0x4D }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SwitchTexture
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SwitchTexture, 4361, 10)

        // D3DDevice_SwitchTexture+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // D3DDevice_SwitchTexture+0x05 : add eax, 0x0C
        { 0x05, 0x83 }, // (Offset,Value)-Pair #2
        { 0x06, 0xC0 }, // (Offset,Value)-Pair #3
        { 0x07, 0x0C }, // (Offset,Value)-Pair #4

        // D3DDevice_SwitchTexture+0x0E : jnb +0x15
        { 0x0E, 0x73 }, // (Offset,Value)-Pair #5
        { 0x0F, 0x15 }, // (Offset,Value)-Pair #6

        // D3DDevice_SwitchTexture+0x22 : retn 0x04
        { 0x22, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x23, 0x04 }, // (Offset,Value)-Pair #8

        // D3DDevice_SwitchTexture+0x2E : jmp +0xD0
        { 0x2E, 0xEB }, // (Offset,Value)-Pair #9
        { 0x2F, 0xD0 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetDisplayMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetDisplayMode, 4361, 13)

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
// * D3DDevice_Swap
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Swap, 4361, 11)

        // D3DDevice_Swap+0x10 : mov ebx, 5
        { 0x10, 0xBB }, // (Offset,Value)-Pair #1
        { 0x11, 0x05 }, // (Offset,Value)-Pair #2

        // D3DDevice_Swap+0x1D : test bl, 3
        { 0x1D, 0xF6 }, // (Offset,Value)-Pair #3
        { 0x1E, 0xC3 }, // (Offset,Value)-Pair #4
        { 0x1F, 0x03 }, // (Offset,Value)-Pair #5

        // D3DDevice_Swap+0x46 : inc dword ptr [esi+0x2AC4]
        { 0x46, 0xFF }, // (Offset,Value)-Pair #6
        { 0x47, 0x86 }, // (Offset,Value)-Pair #7
        { 0x48, 0xC4 }, // (Offset,Value)-Pair #8
        { 0x49, 0x2A }, // (Offset,Value)-Pair #9

        // D3DDevice_Swap+0xAE : retn 4
        { 0xAE, 0xC2 }, // (Offset,Value)-Pair #10
        { 0xAF, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_EnableOverlay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_EnableOverlay, 4361, 11)

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
OOVPA_NO_XREF(D3DDevice_UpdateOverlay, 4361, 11)

        // D3DDevice_UpdateOverlay+0x0F : mov [eax+0x2A90], ecx
        { 0x0F, 0x89 }, // (Offset,Value)-Pair #1
        { 0x10, 0x88 }, // (Offset,Value)-Pair #2
        { 0x11, 0x90 }, // (Offset,Value)-Pair #3
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
// * D3DDevice_BlockUntilVerticalBlank
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BlockUntilVerticalBlank, 4361, 11)

        // D3DDevice_BlockUntilVerticalBlank+0x05 : push 0; push 0; push 1
        { 0x05, 0x6A }, // (Offset,Value)-Pair #1
        { 0x06, 0x00 }, // (Offset,Value)-Pair #2
        { 0x07, 0x6A }, // (Offset,Value)-Pair #3
        { 0x08, 0x00 }, // (Offset,Value)-Pair #4
        { 0x09, 0x6A }, // (Offset,Value)-Pair #5
        { 0x0A, 0x01 }, // (Offset,Value)-Pair #6

        // D3DDevice_BlockUntilVerticalBlank+0x17 : add eax, 0x2434
        { 0x17, 0x05 }, // (Offset,Value)-Pair #7
        { 0x18, 0x34 }, // (Offset,Value)-Pair #8
        { 0x19, 0x24 }, // (Offset,Value)-Pair #9

        // D3DDevice_BlockUntilVerticalBlank+0x1D : call [KrnlImport]
        { 0x1D, 0xFF }, // (Offset,Value)-Pair #10

        // D3DDevice_BlockUntilVerticalBlank+0x23 : retn
        { 0x23, 0xC3 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_TexCoordIndex
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTextureState_TexCoordIndex, 4361, 10)

        // D3DDevice_SetTextureState_TexCoordIndex+0x0D : shl eax, 0x07
        { 0x0D, 0xC1 }, // (Offset,Value)-Pair #1
        { 0x0E, 0xE0 }, // (Offset,Value)-Pair #2
        { 0x0F, 0x07 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetTextureState_TexCoordIndex+0x24 : cmp eax, ecx
        { 0x24, 0x3B }, // (Offset,Value)-Pair #4
        { 0x25, 0xC1 }, // (Offset,Value)-Pair #5

        // D3DDevice_SetTextureState_TexCoordIndex+0x6B : mov esi, 0x2400
        { 0x6B, 0xBE }, // (Offset,Value)-Pair #6
        { 0x6D, 0x24 }, // (Offset,Value)-Pair #7

        // D3DDevice_SetTextureState_TexCoordIndex+0xB3 : shl edx, 0x04
        { 0xB3, 0xC1 }, // (Offset,Value)-Pair #8
        { 0xB4, 0xE2 }, // (Offset,Value)-Pair #9
        { 0xB5, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_VertexBlend
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_VertexBlend, 4361, 11)

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
OOVPA_NO_XREF(D3DDevice_SetRenderState_TextureFactor, 4361, 9)

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
OOVPA_NO_XREF(D3DDevice_SetRenderState_EdgeAntiAlias, 4361, 10)

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
// * D3DDevice_SetRenderState_FillMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_FillMode, 4361, 11)

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
// * D3DDevice_SetRenderState_Simple
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_Simple, 4361, 13)

        // D3DDevice_SetRenderState_Simple+0x05 : add eax, 0x08
        { 0x05, 0x83 }, // (Offset,Value)-Pair #1
        { 0x06, 0xC0 }, // (Offset,Value)-Pair #2
        { 0x07, 0x08 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetRenderState_Simple+0x0E : jnb +0x0C
        { 0x0E, 0x73 }, // (Offset,Value)-Pair #4
        { 0x0F, 0x0C }, // (Offset,Value)-Pair #5

        // D3DDevice_SetRenderState_Simple+0x15 : mov [eax-8], ecx
        { 0x15, 0x89 }, // (Offset,Value)-Pair #6
        { 0x16, 0x48 }, // (Offset,Value)-Pair #7
        { 0x17, 0xF8 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetRenderState_Simple+0x18 : mov [eax-4], ecx
        { 0x18, 0x89 }, // (Offset,Value)-Pair #9
        { 0x19, 0x50 }, // (Offset,Value)-Pair #10
        { 0x1A, 0xFC }, // (Offset,Value)-Pair #11

        // D3DDevice_SetRenderState_Simple+0x25 : jmp +0xD9
        { 0x25, 0xEB }, // (Offset,Value)-Pair #12
        { 0x26, 0xD9 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_ZEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_ZEnable, 4361, 13)

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
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilEnable, 4361, 11)

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
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleAntiAlias, 4361, 12)

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
OOVPA_NO_XREF(D3DDevice_GetTransform, 4361, 12)

        // D3DDevice_GetTransform+0x0A : shl ecx, 0x06
        { 0x0A, 0xC1 }, // (Offset,Value)-Pair #1
        { 0x0B, 0xE1 }, // (Offset,Value)-Pair #2
        { 0x0C, 0x06 }, // (Offset,Value)-Pair #3

        // D3DDevice_GetTransform+0x12 : lea esi, [ecx+eax+*numb*]
        { 0x12, 0x8D }, // (Offset,Value)-Pair #4
        { 0x13, 0xB4 }, // (Offset,Value)-Pair #5
        { 0x14, 0x01 }, // (Offset,Value)-Pair #6

        // D3DDevice_GetTransform+0x19 : mov ecx, 0x10
        { 0x19, 0xB9 }, // (Offset,Value)-Pair #7
        { 0x1A, 0x10 }, // (Offset,Value)-Pair #8

        // D3DDevice_GetTransform+0x20 : pop edi, pop esi
        { 0x20, 0x5F }, // (Offset,Value)-Pair #9
        { 0x21, 0x5E }, // (Offset,Value)-Pair #10

        // D3DDevice_GetTransform+0x22 : retn 0x08
        { 0x22, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x23, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetStreamSource
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetStreamSource, 4361, 14)

        // D3DDevice_SetStreamSource+0x08 : mov edi, [esp+8+0x04]
        { 0x08, 0x8B }, // (Offset,Value)-Pair #1
        { 0x09, 0x7C }, // (Offset,Value)-Pair #2
        { 0x0A, 0x24 }, // (Offset,Value)-Pair #3
        { 0x0B, 0x10 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetStreamSource+0x28 : jz +0x26
        { 0x28, 0x74 }, // (Offset,Value)-Pair #5
        { 0x29, 0x26 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetStreamSource+0x70 : or ecx, 0x40
        { 0x70, 0x83 }, // (Offset,Value)-Pair #7
        { 0x71, 0xC9 }, // (Offset,Value)-Pair #8
        { 0x72, 0x40 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetStreamSource+0x7E : or ecx, 0x70
        { 0x7E, 0x83 }, // (Offset,Value)-Pair #10
        { 0x7F, 0xC9 }, // (Offset,Value)-Pair #11
        { 0x80, 0x70 }, // (Offset,Value)-Pair #12

        // D3DDevice_SetStreamSource+0x89 : retn 0x0C
        { 0x89, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x8A, 0x0C }, // (Offset,Value)-Pair #14
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShader, 4361, 14)

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
// * D3DDevice_DrawVertices
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawVertices, 4361, 12)

        // D3DDevice_DrawVertices+0x12 : mov edi, [esp+0x0C+0x0C]
        { 0x12, 0x8B }, // (Offset,Value)-Pair #1
        { 0x13, 0x7C }, // (Offset,Value)-Pair #2
        { 0x14, 0x24 }, // (Offset,Value)-Pair #3
        { 0x15, 0x18 }, // (Offset,Value)-Pair #4

        // D3DDevice_DrawVertices+0x16 : lea esi, [edi-1]
        { 0x16, 0x8D }, // (Offset,Value)-Pair #5
        { 0x17, 0x77 }, // (Offset,Value)-Pair #6
        { 0x18, 0xFF }, // (Offset,Value)-Pair #7

        // D3DDevice_DrawVertices+0x7E : shl edi, 0x18
        { 0x7E, 0xC1 }, // (Offset,Value)-Pair #8
        { 0x7F, 0xE7 }, // (Offset,Value)-Pair #9
        { 0x80, 0x18 }, // (Offset,Value)-Pair #10

        // D3DDevice_DrawVertices+0x9C : retn 0x0C
        { 0x9C, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x9D, 0x0C }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawVerticesUP
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawVerticesUP, 4361, 10)

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
// * D3DDevice_DrawIndexedVertices
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawIndexedVertices, 4361, 11)

        // D3DDevice_DrawIndexedVertices+0x0E : mov eax, [esi+0x1C]
        { 0x0E, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0F, 0x46 }, // (Offset,Value)-Pair #2
        { 0x10, 0x1C }, // (Offset,Value)-Pair #3

        // D3DDevice_DrawIndexedVertices+0x20 : push 0x0209
        { 0x20, 0x68 }, // (Offset,Value)-Pair #4
        { 0x21, 0x09 }, // (Offset,Value)-Pair #5
        { 0x22, 0x02 }, // (Offset,Value)-Pair #6

        // D3DDevice_DrawIndexedVertices+0x64 : dec eax
        { 0x64, 0x48 }, // (Offset,Value)-Pair #7

        // D3DDevice_DrawIndexedVertices+0xF1 : prefetchnta byte ptr [esi+0x3C]
        { 0xF1, 0x0F }, // (Offset,Value)-Pair #8
        { 0xF2, 0x18 }, // (Offset,Value)-Pair #9
        { 0xF3, 0x46 }, // (Offset,Value)-Pair #10
        { 0xF4, 0x3C }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetLight
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetLight, 4361, 12)

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
OOVPA_NO_XREF(D3DDevice_SetMaterial, 4361, 12)

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
OOVPA_NO_XREF(D3DDevice_LightEnable, 4361, 13)

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
// * D3DVertexBuffer_Lock
// ******************************************************************
OOVPA_NO_XREF(D3DVertexBuffer_Lock, 4361, 11)

        // D3DVertexBuffer_Lock+0x01 : mov bl, [esp+0x18]
        { 0x01, 0x8A }, // (Offset,Value)-Pair #1
        { 0x02, 0x5C }, // (Offset,Value)-Pair #2
        { 0x03, 0x24 }, // (Offset,Value)-Pair #3
        { 0x04, 0x18 }, // (Offset,Value)-Pair #4

        // D3DVertexBuffer_Lock+0x09 : jnz +0x24
        { 0x09, 0x75 }, // (Offset,Value)-Pair #5
        { 0x0A, 0x24 }, // (Offset,Value)-Pair #6

        // D3DVertexBuffer_Lock+0x2F : test bl, 0xA0
        { 0x2F, 0xF6 }, // (Offset,Value)-Pair #7
        { 0x30, 0xC3 }, // (Offset,Value)-Pair #8
        { 0x31, 0xA0 }, // (Offset,Value)-Pair #9

        // D3DVertexBuffer_Lock+0x50 : retn 0x14
        { 0x50, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x51, 0x14 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DResource_Release
// ******************************************************************
OOVPA_NO_XREF(D3DResource_Release, 4361, 13)

        // D3DResource_Release+0x09 : and ecx, 0xFFFF
        { 0x09, 0x81 }, // (Offset,Value)-Pair #1
        { 0x0A, 0xE1 }, // (Offset,Value)-Pair #2
        { 0x0B, 0xFF }, // (Offset,Value)-Pair #3
        { 0x0C, 0xFF }, // (Offset,Value)-Pair #4

        // D3DResource_Release+0x0F : cmp ecx, 1
        { 0x0F, 0x83 }, // (Offset,Value)-Pair #5
        { 0x10, 0xF9 }, // (Offset,Value)-Pair #6
        { 0x11, 0x01 }, // (Offset,Value)-Pair #7

        // D3DResource_Release+0x12 : jnz +0x2E
        { 0x12, 0x75 }, // (Offset,Value)-Pair #8
        { 0x13, 0x2E }, // (Offset,Value)-Pair #9

        // D3DResource_Release+0x2F : test eax, 0x780000
        { 0x2F, 0xA9 }, // (Offset,Value)-Pair #10
        { 0x32, 0x78 }, // (Offset,Value)-Pair #11

        // D3DResource_Release+0x4B : retn 0x04
        { 0x4B, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x4C, 0x04 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DResource_IsBusy
// ******************************************************************
OOVPA_NO_XREF(D3DResource_IsBusy, 4361, 11)

        // D3DResource_IsBusy+0x24 : test eax, 0x780000
        { 0x24, 0xA9 }, // (Offset,Value)-Pair #1
        { 0x25, 0x00 }, // (Offset,Value)-Pair #2
        { 0x26, 0x00 }, // (Offset,Value)-Pair #3
        { 0x27, 0x78 }, // (Offset,Value)-Pair #4

        // D3DResource_IsBusy+0x35 : jnz +0x41
        { 0x35, 0x75 }, // (Offset,Value)-Pair #5
        { 0x36, 0x41 }, // (Offset,Value)-Pair #6

        // D3DResource_IsBusy+0x4E : mov eax, [ecx+0x14]
        { 0x4E, 0x8B }, // (Offset,Value)-Pair #7
        { 0x4F, 0x41 }, // (Offset,Value)-Pair #8
        { 0x50, 0x14 }, // (Offset,Value)-Pair #9

        // D3DResource_IsBusy+0x76 : jnb +0x09
        { 0x76, 0x73 }, // (Offset,Value)-Pair #10
        { 0x77, 0x09 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DSurface_GetDesc
// ******************************************************************
OOVPA_NO_XREF(D3DSurface_GetDesc, 4361, 15)

        // D3DSurface_GetDesc+0x00 : mov eax, [esp+0x08]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x08 }, // (Offset,Value)-Pair #4

        // D3DSurface_GetDesc+0x04 : mov ecx, [esp+0x04]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #5
        { 0x05, 0x4C }, // (Offset,Value)-Pair #6
        { 0x06, 0x24 }, // (Offset,Value)-Pair #7
        { 0x07, 0x04 }, // (Offset,Value)-Pair #8

        // D3DSurface_GetDesc+0x08 : push eax
        { 0x08, 0x50 }, // (Offset,Value)-Pair #9

        // D3DSurface_GetDesc+0x09 : push 0
        { 0x09, 0x6A }, // (Offset,Value)-Pair #10
        { 0x0A, 0x00 }, // (Offset,Value)-Pair #11

        // D3DSurface_GetDesc+0x0B : push ecx
        { 0x0B, 0x51 }, // (Offset,Value)-Pair #12

        // D3DSurface_GetDesc+0x0C : call [abs]
        { 0x0C, 0xE8 }, // (Offset,Value)-Pair #13

        // D3DSurface_GetDesc+0x11 : retn 0x08
        { 0x11, 0xC2 }, // (Offset,Value)-Pair #14
        { 0x12, 0x08 }, // (Offset,Value)-Pair #15
OOVPA_END;

// ******************************************************************
// * D3DBaseTexture_GetLevelCount
// ******************************************************************
OOVPA_NO_XREF(D3DBaseTexture_GetLevelCount, 4361, 13)

        // D3DBaseTexture_GetLevelCount+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x04 }, // (Offset,Value)-Pair #4

        // D3DBaseTexture_GetLevelCount+0x04 : movzx eax, [eax+0x0E]
        { 0x04, 0x0F }, // (Offset,Value)-Pair #5
        { 0x05, 0xB6 }, // (Offset,Value)-Pair #6
        { 0x06, 0x40 }, // (Offset,Value)-Pair #7
        { 0x07, 0x0E }, // (Offset,Value)-Pair #8

        // D3DBaseTexture_GetLevelCount+0x08 : and eax, 0x0F
        { 0x08, 0x83 }, // (Offset,Value)-Pair #9
        { 0x09, 0xE0 }, // (Offset,Value)-Pair #10
        { 0x0A, 0x0F }, // (Offset,Value)-Pair #11

        // D3DBaseTexture_GetLevelCount+0x0B : retn 0x04
        { 0x0B, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x0C, 0x04 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * Get2DSurfaceDesc
// ******************************************************************
OOVPA_NO_XREF(Get2DSurfaceDesc, 4361, 10)

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
// * D3DDevice_SetRenderState_StencilFail
// ******************************************************************
/* Blueshogun96 8/29/08: This is the same as 4432.  Copied and pasted.  Lazy, I know... */
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilFail, 4361, 12)

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
// * D3DDevice_SetRenderState_NormalizeNormals
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_NormalizeNormals, 4361, 8)

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
// * D3DDevice_SetPalette
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPalette, 4361, 12)

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
// * D3DDevice_SetShaderConstantMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetShaderConstantMode, 4361, 12)

        // D3DDevice_SetShaderConstantMode+0x26 : mov [ebx+0x20D8], eax
        { 0x26, 0x89 }, // (Offset,Value)-Pair #1
        { 0x27, 0x83 }, // (Offset,Value)-Pair #2
        { 0x28, 0x18 }, // (Offset,Value)-Pair #3
        { 0x29, 0x20 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetShaderConstantMode+0x50 : mov dword ptr [eax+0x04], 0x3C
        { 0x50, 0xC7 }, // (Offset,Value)-Pair #5
        { 0x51, 0x40 }, // (Offset,Value)-Pair #6
        { 0x52, 0x04 }, // (Offset,Value)-Pair #7
        { 0x53, 0x3C }, // (Offset,Value)-Pair #8

        // D3DDevice_SetShaderConstantMode+0xE7 : add esi, 0x0124
        { 0xE7, 0x81 }, // (Offset,Value)-Pair #9
        { 0xE8, 0xC6 }, // (Offset,Value)-Pair #10
        { 0xE9, 0x24 }, // (Offset,Value)-Pair #11
        { 0xEA, 0x01 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetFlickerFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetFlickerFilter, 4361, 11)

        // D3DDevice_SetFlickerFilter+0x1C : mov eax, [eax+0x2268]
        { 0x1D, 0x80 }, // (Offset,Value)-Pair #2
        { 0x1E, 0x7C }, // (Offset,Value)-Pair #3
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
// * D3DDevice_SetFlickerFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetSoftDisplayFilter, 4361, 12)

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
// * D3DDevice_PrimeVertexCache
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_PrimeVertexCache, 4361, 7)

        { 0x0E, 0xE8 },
        { 0x1E, 0xEE },
        { 0x2E, 0xC1 },
        { 0x3E, 0x24 },
        { 0x4E, 0x8B },
        { 0x5E, 0x04 },
        { 0x6E, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BeginPush
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginPush, 4361, 7)

        { 0x07, 0x6A },
        { 0x08, 0x00 },
        { 0x10, 0x8B },
        { 0x12, 0x24 },
        { 0x17, 0xE8 },
        { 0x1C, 0x8B },
        { 0x21, 0x01 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_EndPush
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_EndPush, 4361, 8)

        { 0x00, 0x8B },
        { 0x02, 0x24 },
        { 0x04, 0x8B },
        { 0x0A, 0x89 },
        { 0x0B, 0x01 },
        { 0x0C, 0xC2 },
        { 0x0D, 0x04 },
        { 0x0E, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Begin
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Begin, 4361, 7)

        { 0x07, 0xE8 },
        { 0x0C, 0x8B },
        { 0x13, 0xE8 },
        { 0x1A, 0x24 },
        { 0x21, 0x00 },
        { 0x28, 0x89 },
        { 0x2F, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_End
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_End, 4361, 7)

        { 0x07, 0x8B },
        { 0x0E, 0xE8 },
        { 0x16, 0x17 },
        { 0x1E, 0x00 },
        { 0x26, 0x46 },
        { 0x2E, 0x01 },
        { 0x36, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData2f
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData2f, 4361, 8)

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
OOVPA_NO_XREF(D3DDevice_SetVertexData4f, 4361, 7)

        { 0x0A, 0x46 },
        { 0x16, 0x08 },
        { 0x22, 0x09 },
        { 0x2E, 0x24 },
        { 0x3A, 0x24 },
        { 0x46, 0x8B },
        { 0x52, 0x14 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData4ub
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData4ub, 4361, 7)

        { 0x08, 0x06 },
        { 0x13, 0x8B },
        { 0x1C, 0x04 },
        { 0x26, 0x0F },
        { 0x30, 0x24 },
        { 0x3A, 0x24 },
        { 0x44, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Release
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Release, 4361, 8)

        { 0x07, 0x8B },
        { 0x0C, 0x00 },
        { 0x13, 0xCF },
        { 0x1A, 0xC0 },
        { 0x25, 0xA3 },
        { 0x2A, 0xF3 },
        { 0x2F, 0x89 },
        { 0x36, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BeginPushBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginPushBuffer, 4361, 7)

        { 0x0B, 0xCE },
        { 0x1B, 0x89 },
        { 0x25, 0x00 },
        { 0x32, 0x06 },
        { 0x3F, 0x03 },
        { 0x4C, 0x04 },
        { 0x59, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_EndPushBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_EndPushBuffer, 4361, 7)

        { 0x11, 0x8D },
        { 0x24, 0x8B },
        { 0x37, 0x58 },
        { 0x4A, 0xFF },
        { 0x5D, 0xF7 },
        { 0x70, 0x03 },
        { 0x83, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetPushBufferOffset
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetPushBufferOffset, 4361, 7)

        { 0x14, 0xB8 },
        { 0x2A, 0x8B },
        { 0x40, 0x47 },
        { 0x56, 0x89 },
        { 0x6C, 0xAB },
        { 0x82, 0x04 },
        { 0x98, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_RunPushBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_RunPushBuffer, 4361, 8)

        { 0x1E, 0x07 },
        { 0x3E, 0x00 },
        { 0x5E, 0x46 },
        { 0x7E, 0x24 },
        { 0x9E, 0x18 },
        { 0xBE, 0x74 },
        { 0xE2, 0x8B },
        { 0xFE, 0x24 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateCubeTexture
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateCubeTexture, 4361, 8)

        { 0x03, 0x18 },
        { 0x08, 0x8B },
        { 0x0D, 0x8B },
        { 0x12, 0x00 },
        { 0x17, 0x50 },
        { 0x1C, 0x6A },
        { 0x25, 0xC2 },
        { 0x26, 0x18 },
OOVPA_END;

// ******************************************************************
// * D3DCubeTexture_GetCubeMapSurface
// ******************************************************************
OOVPA_NO_XREF(D3DCubeTexture_GetCubeMapSurface, 4361, 7)

        { 0x09, 0x44 },
        { 0x14, 0x24 },
        { 0x1F, 0x50 },
        { 0x2A, 0x50 },
        { 0x35, 0x8B },
        { 0x40, 0x24 },
        { 0x4B, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3D8_4361
// ******************************************************************
OOVPATable D3D8_4361[] = {

	REGISTER_OOVPA(D3D_CreateDevice, 4361, PATCH),
	REGISTER_OOVPA(D3D_CheckDeviceFormat, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_LoadVertexShader, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SelectVertexShader, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderTarget, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_AddRef, 4361, PATCH),
	REGISTER_OOVPA(D3D_ClearStateBlockFlags, 4361, XREF),
	REGISTER_OOVPA(D3DDevice_SetGammaRamp, 4361, PATCH),
	REGISTER_OOVPA(D3D_RecordStateBlock, 4361, XREF),
	REGISTER_OOVPA(D3DDevice_BeginStateBlock, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_CaptureStateBlock, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteStateBlock, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_ApplyStateBlock, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_EndStateBlock, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_CopyRects, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateImageSurface, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_GetBackBuffer, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetRenderTarget, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDepthStencilSurface, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVertexShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_CreatePixelShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShader, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShaderProgram, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetViewport, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_GetViewport, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BumpEnv, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BorderColor, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateTexture, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetIndices, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTexture, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SwitchTexture, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDisplayMode, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_Clear, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_Swap, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVertexBuffer, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_EnableOverlay, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_UpdateOverlay, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_BlockUntilVerticalBlank, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_TexCoordIndex, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_VertexBlend, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_CullMode, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TextureFactor, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_EdgeAntiAlias, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FillMode, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FogColor, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Simple, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZEnable, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilEnable, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleAntiAlias, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTransform, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTransform, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetStreamSource, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShader, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawVertices, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawVerticesUP, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawIndexedVertices, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetLight, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetMaterial, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_LightEnable, 4361, PATCH),
	REGISTER_OOVPA(D3DVertexBuffer_Lock, 4361, PATCH),
	REGISTER_OOVPA(D3DResource_Register, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_Release, 4361, PATCH),
	REGISTER_OOVPA(D3DResource_IsBusy, 4361, PATCH),
	REGISTER_OOVPA(Get2DSurfaceDesc, 4361, PATCH),
	REGISTER_OOVPA(D3DSurface_GetDesc, 4361, PATCH),
	REGISTER_OOVPA(D3DSurface_LockRect, 3925, PATCH),
	REGISTER_OOVPA(D3DBaseTexture_GetLevelCount, 4361, PATCH),
	REGISTER_OOVPA(D3DTexture_LockRect, 3925, PATCH),
	REGISTER_OOVPA(D3DTexture_GetSurfaceLevel, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilFail, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_NormalizeNormals, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDeviceCaps, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreatePalette, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateIndexBuffer, 3925, PATCH), // TODO: This needs to be verified on 4361, not just 4242!
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZBias, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_AddRef, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_Release, 3925, PATCH),
	REGISTER_OOVPA(D3DPalette_Lock, 3925, PATCH),
	REGISTER_OOVPA(D3DCubeTexture_LockRect, 3925, PATCH), // TODO: This needs to be verified on 4361, not just 4242!
	REGISTER_OOVPA(D3DDevice_SetPalette, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_Reset, 4134, PATCH),
	REGISTER_OOVPA(D3D_KickOffAndWaitForIdle, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TwoSidedLighting, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_BackFillMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_ColorKeyColor, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FrontFace, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LogicOp, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilFail, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_OcclusionCullEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilCullEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZCmpAlwaysRead, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZRead, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_DoNotCullUncompressed, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Dxt1NoiseEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMask, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ShadowFunc, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_YuvEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawIndexedVerticesUP, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShaderConstant, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_DeletePixelShader, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_CullMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteVertexShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScreenSpaceOffset, 4134, PATCH),
	REGISTER_OOVPA(D3D_SetFence, 4134, XREF),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LineWidth, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetSwapCallback, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTile, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTileNoWait, 4134, ALIAS, D3DDevice_SetTile),
	REGISTER_OOVPA(D3DDevice_IsBusy, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetGammaRamp, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_GetCreationParameters, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVisibilityTestResult, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetShaderConstantMode, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetFlickerFilter, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetSoftDisplayFilter, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_PrimeVertexCache, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginPush, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_EndPush, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_Begin, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_End, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVerticalBlankCallback, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData2f, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4f, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4ub, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderSize, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_BlockOnFence, 4134, PATCH),
	REGISTER_OOVPA(D3D_GetAdapterIdentifier, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_Release, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginPushBuffer, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_EndPushBuffer, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_RunPushBuffer, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_GetPushBufferOffset, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateCubeTexture, 4361, PATCH),
	REGISTER_OOVPA(D3DCubeTexture_GetCubeMapSurface, 4361, PATCH),
	REGISTER_OOVPA(Lock2DSurface, 3925, PATCH),
};

// ******************************************************************
// * D3D8_4361_SIZE
// ******************************************************************
uint32 D3D8_4361_SIZE = sizeof(D3D8_4361);
