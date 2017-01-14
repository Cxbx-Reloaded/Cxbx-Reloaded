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
// * IDirect3D8_CreateDevice
// ******************************************************************
OOVPA_NO_XREF(IDirect3D8_CreateDevice_1_0_4361, 8)
        // IDirect3D8_CreateDevice+0x07 : jnz +0x0A
        { 0x07, 0x75 }, // (Offset,Value)-Pair #1
        { 0x08, 0x0A }, // (Offset,Value)-Pair #2

        // IDirect3D8_CreateDevice+0x86 : repe stosd
        { 0x86, 0xF3 }, // (Offset,Value)-Pair #3
        { 0x87, 0xAB }, // (Offset,Value)-Pair #4

        // IDirect3D8_CreateDevice+0x89 : mov eax, esi
        { 0x89, 0x8B }, // (Offset,Value)-Pair #5
        { 0x8A, 0xC6 }, // (Offset,Value)-Pair #6

        // IDirect3D8_CreateDevice+0xA0 : retn 0x18
        { 0xA0, 0xC2 }, // (Offset,Value)-Pair #7
        { 0xA1, 0x18 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetVerticalBlankCallback
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetVerticalBlankCallback_1_0_4361, 6)

	{ 0x01, 0x44 },
	{ 0x03, 0x04 },
	{ 0x05, 0x0D },
	{ 0x0C, 0x30 },
	{ 0x0D, 0x24 },
	{ 0x11, 0x04 },
OOVPA_END;

// ******************************************************************
// * IDirect3D8_GetAdapterModeCount
// ******************************************************************
OOVPA_NO_XREF(IDirect3D8_GetAdapterModeCount_1_0_4361, 12)

        // IDirect3D8_GetAdapterModeCount+0x25 : add edx, 0xFF
        { 0x25, 0x81 }, // (Offset,Value)-Pair #1
        { 0x26, 0xE2 }, // (Offset,Value)-Pair #2
        { 0x27, 0xFF }, // (Offset,Value)-Pair #3

        // IDirect3D8_GetAdapterModeCount+0x47 : cmp edi, 0x04
        { 0x47, 0x83 }, // (Offset,Value)-Pair #4
        { 0x48, 0xFF }, // (Offset,Value)-Pair #5
        { 0x49, 0x04 }, // (Offset,Value)-Pair #6

        // IDirect3D8_GetAdapterModeCount+0x95 : shl eax, 0x02
        { 0x95, 0xC1 }, // (Offset,Value)-Pair #7
        { 0x96, 0xE0 }, // (Offset,Value)-Pair #8
        { 0x97, 0x02 }, // (Offset,Value)-Pair #9

        // IDirect3D8_GetAdapterModeCount+0x99 : add esp, 0x08
        { 0x99, 0x83 }, // (Offset,Value)-Pair #10
        { 0x9A, 0xC4 }, // (Offset,Value)-Pair #11
        { 0x9B, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3D8_GetAdapterDisplayMode
// ******************************************************************
OOVPA_NO_XREF(IDirect3D8_GetAdapterDisplayMode_1_0_4361, 13)

        // IDirect3D8_GetAdapterDisplayMode+0x08 : mov eax, 0x8876086C
        { 0x08, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x09, 0x6C }, // (Offset,Value)-Pair #2
        { 0x0A, 0x08 }, // (Offset,Value)-Pair #3
        { 0x0B, 0x76 }, // (Offset,Value)-Pair #4
        { 0x0C, 0x88 }, // (Offset,Value)-Pair #5

        // IDirect3D8_GetAdapterDisplayMode+0x18 : jnz +0x17
        { 0x18, 0x75 }, // (Offset,Value)-Pair #6
        { 0x19, 0x17 }, // (Offset,Value)-Pair #7

        // IDirect3D8_GetAdapterDisplayMode+0x31 : mov ecx, [edx+0x2080]
        { 0x31, 0x8B }, // (Offset,Value)-Pair #8
        { 0x32, 0x8A }, // (Offset,Value)-Pair #9
        { 0x33, 0x80 }, // (Offset,Value)-Pair #10
        { 0x34, 0x20 }, // (Offset,Value)-Pair #11

        // IDirect3D8_GetAdapterDisplayMode+0xBD : retn 0x08
        { 0xBD, 0xC2 }, // (Offset,Value)-Pair #12
        { 0xBE, 0x08 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * IDirect3D8_EnumAdapterModes
// ******************************************************************
OOVPA_NO_XREF(IDirect3D8_EnumAdapterModes_1_0_4361, 10)

        // IDirect3D8_EnumAdapterModes+0x00 : sub esp, 0x08
        { 0x00, 0x83 }, // (Offset,Value)-Pair #1
        { 0x01, 0xEC }, // (Offset,Value)-Pair #2
        { 0x02, 0x08 }, // (Offset,Value)-Pair #3

        // IDirect3D8_EnumAdapterModes+0xD1 : and edx, 0x0A
        { 0xD1, 0x83 }, // (Offset,Value)-Pair #4
        { 0xD2, 0xE2 }, // (Offset,Value)-Pair #5
        { 0xD3, 0x0A }, // (Offset,Value)-Pair #6

        // IDirect3D8_EnumAdapterModes+0xF5 : mov dword ptr[edi+0x10], 0x1E
        { 0xF5, 0xC7 }, // (Offset,Value)-Pair #7
        { 0xF6, 0x47 }, // (Offset,Value)-Pair #8
        { 0xF7, 0x10 }, // (Offset,Value)-Pair #9
        { 0xF8, 0x1E }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_AddRef
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_AddRef_1_0_4361, 10)

        // IDirect3DDevice8_AddRef+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_AddRef+0x05 : mov ecx, [eax+0x0440]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x88 }, // (Offset,Value)-Pair #3
        { 0x07, 0x40 }, // (Offset,Value)-Pair #4
        { 0x08, 0x04 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_AddRef+0x0B : inc ecx
        { 0x0B, 0x41 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_AddRef+0x05 : mov [eax+0x0440], ecx
        { 0x0C, 0x89 }, // (Offset,Value)-Pair #7
        { 0x0D, 0x88 }, // (Offset,Value)-Pair #8
        { 0x0E, 0x40 }, // (Offset,Value)-Pair #9
        { 0x0F, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3D_ClearStateBlockFlags
// ******************************************************************
OOVPA_XREF(IDirect3D_ClearStateBlockFlags_1_0_4361, 9,

    XREF_CLEARSTATEBLOCKFLAGS,
    XRefZero)

        // IDirect3D_ClearStateBlockFlags+0x0A : movzx ecx, 0x82
        { 0x0A, 0xB9 }, // (Offset,Value)-Pair #1
        { 0x0B, 0x82 }, // (Offset,Value)-Pair #2

        // IDirect3D_ClearStateBlockFlags+0x16 : mov ecx, [edx+0x0390]
        { 0x16, 0x8B }, // (Offset,Value)-Pair #3
        { 0x17, 0x8A }, // (Offset,Value)-Pair #4
        { 0x18, 0x90 }, // (Offset,Value)-Pair #5
        { 0x19, 0x03 }, // (Offset,Value)-Pair #6

        // IDirect3D_ClearStateBlockFlags+0x2F : add ecx, 0x90
        { 0x2F, 0x81 }, // (Offset,Value)-Pair #7
        { 0x30, 0xC1 }, // (Offset,Value)-Pair #8
        { 0x31, 0x90 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * IDirect3D_RecordStateBlock
// ******************************************************************
OOVPA_XREF(IDirect3D_RecordStateBlock_1_0_4361, 10,

    XREF_RECORDSTATEBLOCK,
    XRefZero)

        // IDirect3D_RecordStateBlock+0x0F : mov eax, [edi+0x0390]
        { 0x0F, 0x8B }, // (Offset,Value)-Pair #1
        { 0x10, 0x87 }, // (Offset,Value)-Pair #2
        { 0x11, 0x90 }, // (Offset,Value)-Pair #3
        { 0x12, 0x03 }, // (Offset,Value)-Pair #4

        // IDirect3D_RecordStateBlock+0x95 : add ebx, ecx
        { 0x95, 0x03 }, // (Offset,Value)-Pair #5
        { 0x96, 0xD9 }, // (Offset,Value)-Pair #6

        // IDirect3D_RecordStateBlock+0xD5 : mov dword ptr [esi+0x0C], 1
        { 0xD5, 0xC7 }, // (Offset,Value)-Pair #7
        { 0xD6, 0x46 }, // (Offset,Value)-Pair #8
        { 0xD7, 0x0C }, // (Offset,Value)-Pair #9
        { 0xD8, 0x01 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_BeginStateBlock
// ******************************************************************
OOVPA_XREF(IDirect3DDevice8_BeginStateBlock_1_0_4361, 6,

    XRefNoSaveIndex,
    XRefOne)

        // IDirect3DDevice8_BeginStateBlock+0x0F : call [ClearStateBlockFlags]
        { 0x0A, XREF_CLEARSTATEBLOCKFLAGS }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_BeginStateBlock+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_BeginStateBlock+0x05 : mov [eax+8], 0x20
        { 0x05, 0x83 }, // (Offset,Value)-Pair #3
        { 0x06, 0x48 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5
        { 0x08, 0x20 }, // (Offset,Value)-Pair #6
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_CaptureStateBlock
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_CaptureStateBlock_1_0_4361, 9)

        // IDirect3DDevice8_CaptureStateBlock+0x36 : mov eax, [edi+eax*4+0x0A78]
        { 0x36, 0x8B }, // (Offset,Value)-Pair #1
        { 0x37, 0x84 }, // (Offset,Value)-Pair #2
        { 0x38, 0x87 }, // (Offset,Value)-Pair #3
        { 0x39, 0x78 }, // (Offset,Value)-Pair #4
        { 0x3A, 0x0A }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_CaptureStateBlock+0xA9 : cmp dword ptr [esi+0x0C], 0
        { 0xA9, 0x83 }, // (Offset,Value)-Pair #6
        { 0xAA, 0x7E }, // (Offset,Value)-Pair #7
        { 0xAB, 0x0C }, // (Offset,Value)-Pair #8
        { 0xAC, 0x00 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3DDevice_DeleteStateBlock
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DeleteStateBlock_1_0_4361, 7)

	{ 0x11, 0x76 },
	{ 0x24, 0x3B },
	{ 0x37, 0xE8 },
	{ 0x4A, 0x50 },
	{ 0x5D, 0x74 },
	{ 0x70, 0x06 },
	{ 0x83, 0xEB },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_ApplyStateBlock
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_ApplyStateBlock_1_0_4361, 9)

        // IDirect3DDevice8_ApplyStateBlock+0x0E : lea esi, [edi+0x3C]
        { 0x0E, 0x8D }, // (Offset,Value)-Pair #1
        { 0x0F, 0x77 }, // (Offset,Value)-Pair #2
        { 0x10, 0x3C }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_ApplyStateBlock+0x34 : cmp [edi+8], ebp
        { 0x34, 0x39 }, // (Offset,Value)-Pair #4
        { 0x35, 0x6F }, // (Offset,Value)-Pair #5
        { 0x36, 0x08 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_ApplyStateBlock+0x9E : sub eax, 0x60
        { 0x9E, 0x83 }, // (Offset,Value)-Pair #7
        { 0x9F, 0xE8 }, // (Offset,Value)-Pair #8
        { 0xA0, 0x60 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_EndStateBlock
// ******************************************************************
OOVPA_XREF(IDirect3DDevice8_EndStateBlock_1_0_4361, 6,

    XRefNoSaveIndex,
    XRefOne)

        // IDirect3DDevice8_EndStateBlock+0x0F : call [ClearStateBlockFlags]
        { 0x0A, XREF_RECORDSTATEBLOCK }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_EndStateBlock+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_EndStateBlock+0x05 : and [eax+8], 0xFFFFFFDF
        { 0x05, 0x83 }, // (Offset,Value)-Pair #3
        { 0x06, 0x60 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5
        { 0x08, 0xDF }, // (Offset,Value)-Pair #6
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_GetRenderTarget
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_GetRenderTarget_1_0_4361, 9)

        // IDirect3DDevice8_GetRenderTarget+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_GetRenderTarget+0x05 : mov eax, [eax + 0x2070]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x80 }, // (Offset,Value)-Pair #3
        { 0x07, 0x70 }, // (Offset,Value)-Pair #4
        { 0x08, 0x20 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_GetRenderTarget+0x11 : mov [ecx], eax
        { 0x11, 0x89 }, // (Offset,Value)-Pair #6
        { 0x12, 0x01 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_GetRenderTarget+0x1D : retn 0x04
        { 0x1D, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x1E, 0x04 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_GetViewport
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_GetViewport_1_0_4361, 10)

        // IDirect3DDevice8_GetViewport+0x05 : push esi; push edi
        { 0x05, 0x56 }, // (Offset,Value)-Pair #1
        { 0x06, 0x57 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_GetViewport+0x0B : lea esi, [eax+0x9D0]
        { 0x0B, 0x8D }, // (Offset,Value)-Pair #3
        { 0x0C, 0xB0 }, // (Offset,Value)-Pair #4
        { 0x0D, 0xD0 }, // (Offset,Value)-Pair #5
        { 0x0E, 0x09 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_GetViewport+0x11 : mov ecx, 6
        { 0x11, 0xB9 }, // (Offset,Value)-Pair #7
        { 0x12, 0x06 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_GetViewport+0x1A : retn 0x04
        { 0x1A, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x1B, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetTextureState_BorderColor
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetTextureState_BorderColor_1_0_4361, 15)

        // IDirect3DDevice8_SetTextureState_BorderColor+0x0C : jb +0x05
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #1
        { 0x0D, 0x05 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetTextureState_BorderColor+0x19 : shl edx, 6
        { 0x19, 0xC1 }, // (Offset,Value)-Pair #3
        { 0x1A, 0xE2 }, // (Offset,Value)-Pair #4
        { 0x1B, 0x06 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetTextureState_BorderColor+0x2B : add eax, 8; mov [esi], eax; shl ecx, 7
        { 0x2B, 0x83 }, // (Offset,Value)-Pair #6
        { 0x2C, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x2D, 0x08 }, // (Offset,Value)-Pair #8
        { 0x2E, 0x89 }, // (Offset,Value)-Pair #9
        { 0x2F, 0x06 }, // (Offset,Value)-Pair #10
        { 0x30, 0xC1 }, // (Offset,Value)-Pair #11
        { 0x31, 0xE1 }, // (Offset,Value)-Pair #12
        { 0x32, 0x07 }, // (Offset,Value)-Pair #13

        // IDirect3DDevice8_SetTextureState_BorderColor+0x3A : retn 0x08
        { 0x3A, 0xC2 }, // (Offset,Value)-Pair #14
        { 0x3B, 0x08 }, // (Offset,Value)-Pair #15
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SwitchTexture
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SwitchTexture_1_0_4361, 10)

        // IDirect3DDevice8_SwitchTexture+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_SwitchTexture+0x05 : add eax, 0x0C
        { 0x05, 0x83 }, // (Offset,Value)-Pair #2
        { 0x06, 0xC0 }, // (Offset,Value)-Pair #3
        { 0x07, 0x0C }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SwitchTexture+0x0E : jnb +0x15
        { 0x0E, 0x73 }, // (Offset,Value)-Pair #5
        { 0x0F, 0x15 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SwitchTexture+0x22 : retn 0x04
        { 0x22, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x23, 0x04 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SwitchTexture+0x2E : jmp +0xD0
        { 0x2E, 0xEB }, // (Offset,Value)-Pair #9
        { 0x2F, 0xD0 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_Swap
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_Swap_1_0_4361, 11)

        // IDirect3DDevice8_Swap+0x10 : mov ebx, 5
        { 0x10, 0xBB }, // (Offset,Value)-Pair #1
        { 0x11, 0x05 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_Swap+0x1D : test bl, 3
        { 0x1D, 0xF6 }, // (Offset,Value)-Pair #3
        { 0x1E, 0xC3 }, // (Offset,Value)-Pair #4
        { 0x1F, 0x03 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_Swap+0x46 : inc dword ptr [esi+0x2AC4]
        { 0x46, 0xFF }, // (Offset,Value)-Pair #6
        { 0x47, 0x86 }, // (Offset,Value)-Pair #7
        { 0x48, 0xC4 }, // (Offset,Value)-Pair #8
        { 0x49, 0x2A }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_Swap+0xAE : retn 4
        { 0xAE, 0xC2 }, // (Offset,Value)-Pair #10
        { 0xAF, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_EnableOverlay
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_EnableOverlay_1_0_4361, 11)

        // IDirect3DDevice8_EnableOverlay+0x0B : mov ecx, [eax+0x8700]
        { 0x0B, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0C, 0x88 }, // (Offset,Value)-Pair #2
        { 0x0E, 0x87 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_EnableOverlay+0x15 : jz +0x0A
        { 0x15, 0x74 }, // (Offset,Value)-Pair #4
        { 0x16, 0x0A }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_EnableOverlay+0x5A : mov [eax+0x8918], ecx
        { 0x5A, 0x89 }, // (Offset,Value)-Pair #6
        { 0x5B, 0x88 }, // (Offset,Value)-Pair #7
        { 0x5C, 0x18 }, // (Offset,Value)-Pair #8
        { 0x5D, 0x89 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_EnableOverlay+0x60 : retn 0x04
        { 0x60, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x61, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_UpdateOverlay
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_UpdateOverlay_1_0_4361, 11)

        // IDirect3DDevice8_UpdateOverlay+0x0F : mov [eax+0x2A90], ecx
        { 0x0F, 0x89 }, // (Offset,Value)-Pair #1
        { 0x10, 0x88 }, // (Offset,Value)-Pair #2
        { 0x11, 0x90 }, // (Offset,Value)-Pair #3
        { 0x12, 0x2A }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_UpdateOverlay+0x86 : and ecx, 0xFFFFFFFE
        { 0x86, 0x83 }, // (Offset,Value)-Pair #5
        { 0x87, 0xE1 }, // (Offset,Value)-Pair #6
        { 0x88, 0xFE }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_UpdateOverlay+0xA2 : mov [esi+0x8920], ecx
        { 0xA2, 0x89 }, // (Offset,Value)-Pair #8
        { 0xA3, 0x8E }, // (Offset,Value)-Pair #9
        { 0xA4, 0x20 }, // (Offset,Value)-Pair #10
        { 0xA5, 0x89 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_BlockUntilVerticalBlank
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_BlockUntilVerticalBlank_1_0_4361, 11)

        // IDirect3DDevice8_BlockUntilVerticalBlank+0x05 : push 0; push 0; push 1
        { 0x05, 0x6A }, // (Offset,Value)-Pair #1
        { 0x06, 0x00 }, // (Offset,Value)-Pair #2
        { 0x07, 0x6A }, // (Offset,Value)-Pair #3
        { 0x08, 0x00 }, // (Offset,Value)-Pair #4
        { 0x09, 0x6A }, // (Offset,Value)-Pair #5
        { 0x0A, 0x01 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_BlockUntilVerticalBlank+0x17 : add eax, 0x2434
        { 0x17, 0x05 }, // (Offset,Value)-Pair #7
        { 0x18, 0x34 }, // (Offset,Value)-Pair #8
        { 0x19, 0x24 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_BlockUntilVerticalBlank+0x1D : call [KrnlImport]
        { 0x1D, 0xFF }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_BlockUntilVerticalBlank+0x23 : retn
        { 0x23, 0xC3 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetTextureState_TexCoordIndex
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_4361, 10)

        // IDirect3DDevice8_SetTextureState_TexCoordIndex+0x0D : shl eax, 0x07
        { 0x0D, 0xC1 }, // (Offset,Value)-Pair #1
        { 0x0E, 0xE0 }, // (Offset,Value)-Pair #2
        { 0x0F, 0x07 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetTextureState_TexCoordIndex+0x24 : cmp eax, ecx
        { 0x24, 0x3B }, // (Offset,Value)-Pair #4
        { 0x25, 0xC1 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetTextureState_TexCoordIndex+0x6B : mov esi, 0x2400
        { 0x6B, 0xBE }, // (Offset,Value)-Pair #6
        { 0x6D, 0x24 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetTextureState_TexCoordIndex+0xB3 : shl edx, 0x04
        { 0xB3, 0xC1 }, // (Offset,Value)-Pair #8
        { 0xB4, 0xE2 }, // (Offset,Value)-Pair #9
        { 0xB5, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DResource8_Release
// ******************************************************************
OOVPA_NO_XREF(IDirect3DResource8_Release_1_0_4361, 13)

        // IDirect3DResource8_Release+0x09 : and ecx, 0xFFFF
        { 0x09, 0x81 }, // (Offset,Value)-Pair #1
        { 0x0A, 0xE1 }, // (Offset,Value)-Pair #2
        { 0x0B, 0xFF }, // (Offset,Value)-Pair #3
        { 0x0C, 0xFF }, // (Offset,Value)-Pair #4

        // IDirect3DResource8_Release+0x0F : cmp ecx, 1
        { 0x0F, 0x83 }, // (Offset,Value)-Pair #5
        { 0x10, 0xF9 }, // (Offset,Value)-Pair #6
        { 0x11, 0x01 }, // (Offset,Value)-Pair #7

        // IDirect3DResource8_Release+0x12 : jnz +0x2E
        { 0x12, 0x75 }, // (Offset,Value)-Pair #8
        { 0x13, 0x2E }, // (Offset,Value)-Pair #9

        // IDirect3DResource8_Release+0x2F : test eax, 0x780000
        { 0x2F, 0xA9 }, // (Offset,Value)-Pair #10
        { 0x32, 0x78 }, // (Offset,Value)-Pair #11

        // IDirect3DResource8_Release+0x4B : retn 0x04
        { 0x4B, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x4C, 0x04 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * IDirect3DResource8_IsBusy
// ******************************************************************
OOVPA_NO_XREF(IDirect3DResource8_IsBusy_1_0_4361, 11)

        // IDirect3DResource8_IsBusy+0x24 : test eax, 0x780000
        { 0x24, 0xA9 }, // (Offset,Value)-Pair #1
        { 0x25, 0x00 }, // (Offset,Value)-Pair #2
        { 0x26, 0x00 }, // (Offset,Value)-Pair #3
        { 0x27, 0x78 }, // (Offset,Value)-Pair #4

        // IDirect3DResource8_IsBusy+0x35 : jnz +0x41
        { 0x35, 0x75 }, // (Offset,Value)-Pair #5
        { 0x36, 0x41 }, // (Offset,Value)-Pair #6

        // IDirect3DResource8_IsBusy+0x4E : mov eax, [ecx+0x14]
        { 0x4E, 0x8B }, // (Offset,Value)-Pair #7
        { 0x4F, 0x41 }, // (Offset,Value)-Pair #8
        { 0x50, 0x14 }, // (Offset,Value)-Pair #9

        // IDirect3DResource8_IsBusy+0x76 : jnb +0x09
        { 0x76, 0x73 }, // (Offset,Value)-Pair #10
        { 0x77, 0x09 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3DBaseTexture8_GetLevelCount
// ******************************************************************
OOVPA_NO_XREF(IDirect3DBaseTexture8_GetLevelCount_1_0_4361, 13)

        // IDirect3DBaseTexture8_GetLevelCount+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x04 }, // (Offset,Value)-Pair #4

        // IDirect3DBaseTexture8_GetLevelCount+0x04 : movzx eax, [eax+0x0E]
        { 0x04, 0x0F }, // (Offset,Value)-Pair #5
        { 0x05, 0xB6 }, // (Offset,Value)-Pair #6
        { 0x06, 0x40 }, // (Offset,Value)-Pair #7
        { 0x07, 0x0E }, // (Offset,Value)-Pair #8

        // IDirect3DBaseTexture8_GetLevelCount+0x08 : and eax, 0x0F
        { 0x08, 0x83 }, // (Offset,Value)-Pair #9
        { 0x09, 0xE0 }, // (Offset,Value)-Pair #10
        { 0x0A, 0x0F }, // (Offset,Value)-Pair #11

        // IDirect3DBaseTexture8_GetLevelCount+0x0B : retn 0x04
        { 0x0B, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x0C, 0x04 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetPalette
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetPalette_1_0_4361, 12)

        // IDirect3DDevice8_SetPalette+0x0D : mov eax, [edi+esi*4+0x0A88]
        { 0x0D, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0E, 0x84 }, // (Offset,Value)-Pair #2
        { 0x0F, 0xBE }, // (Offset,Value)-Pair #3
        { 0x10, 0x88 }, // (Offset,Value)-Pair #4
        { 0x11, 0x0A }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetPalette+0x76 : add edi, 0x41B20
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
// * IDirect3DDevice8_SetShaderConstantMode
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetShaderConstantMode_1_0_4361, 12)

        // IDirect3DDevice8_SetShaderConstantMode+0x26 : mov [ebx+0x20D8], eax
        { 0x26, 0x89 }, // (Offset,Value)-Pair #1
        { 0x27, 0x83 }, // (Offset,Value)-Pair #2
        { 0x28, 0x18 }, // (Offset,Value)-Pair #3
        { 0x29, 0x20 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetShaderConstantMode+0x50 : mov dword ptr [eax+0x04], 0x3C
        { 0x50, 0xC7 }, // (Offset,Value)-Pair #5
        { 0x51, 0x40 }, // (Offset,Value)-Pair #6
        { 0x52, 0x04 }, // (Offset,Value)-Pair #7
        { 0x53, 0x3C }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetShaderConstantMode+0xE7 : add esi, 0x0124
        { 0xE7, 0x81 }, // (Offset,Value)-Pair #9
        { 0xE8, 0xC6 }, // (Offset,Value)-Pair #10
        { 0xE9, 0x24 }, // (Offset,Value)-Pair #11
        { 0xEA, 0x01 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetFlickerFilter
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetFlickerFilter_1_0_4361, 11)

        // IDirect3DDevice8_SetFlickerFilter+0x1C : mov eax, [eax+0x2268]
        { 0x1D, 0x80 }, // (Offset,Value)-Pair #2
        { 0x1E, 0x7C }, // (Offset,Value)-Pair #3
        { 0x1F, 0x22 }, // (Offset,Value)-Pair #4

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
// * IDirect3DDevice8_SetFlickerFilter
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetSoftDisplayFilter_1_0_4361, 12)

        // IDirect3DDevice8_SetSoftDisplayFilter+0x25 : mov edx, [eax+0x2268]
        { 0x25, 0x8B }, // (Offset,Value)-Pair #1
        { 0x26, 0x90 }, // (Offset,Value)-Pair #2
        { 0x27, 0x68 }, // (Offset,Value)-Pair #3
        { 0x28, 0x22 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetSoftDisplayFilter+0x2B : push 0; push esi; push 0x0E; push edx
        { 0x2B, 0x6A }, // (Offset,Value)-Pair #5
        { 0x2C, 0x00 }, // (Offset,Value)-Pair #6
        { 0x2D, 0x56 }, // (Offset,Value)-Pair #7
        { 0x2E, 0x6A }, // (Offset,Value)-Pair #8
        { 0x2F, 0x0E }, // (Offset,Value)-Pair #9
        { 0x30, 0x52 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetSoftDisplayFilter+0x4E : retn 0x04
        { 0x4E, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x4F, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_PrimeVertexCache
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_PrimeVertexCache_1_0_4361, 7)

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
OOVPA_NO_XREF(D3DDevice_BeginPush_1_0_4361, 7)

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
OOVPA_NO_XREF(D3DDevice_EndPush_1_0_4361, 8)

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
OOVPA_NO_XREF(D3DDevice_Begin_1_0_4361, 7)

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
OOVPA_NO_XREF(D3DDevice_End_1_0_4361, 7)

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
OOVPA_NO_XREF(D3DDevice_SetVertexData2f_1_0_4361, 8)

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
OOVPA_NO_XREF(D3DDevice_SetVertexData4f_1_0_4361, 7)

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
OOVPA_NO_XREF(D3DDevice_SetVertexData4ub_1_0_4361, 7)

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
OOVPA_NO_XREF(D3DDevice_Release_1_0_4361, 8)

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
OOVPA_NO_XREF(D3DDevice_BeginPushBuffer_1_0_4361, 7)

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
OOVPA_NO_XREF(D3DDevice_EndPushBuffer_1_0_4361, 7)

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
OOVPA_NO_XREF(D3DDevice_GetPushBufferOffset_1_0_4361, 7)

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
OOVPA_NO_XREF(D3DDevice_RunPushBuffer_1_0_4361, 8)

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
OOVPA_NO_XREF(D3DDevice_CreateCubeTexture_1_0_4361, 8)

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
OOVPA_NO_XREF(D3DCubeTexture_GetCubeMapSurface_1_0_4361, 7)

        { 0x09, 0x44 },
        { 0x14, 0x24 },
        { 0x1F, 0x50 },
        { 0x2A, 0x50 },
        { 0x35, 0x8B },
        { 0x40, 0x24 },
        { 0x4B, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3D8_1_0_4361
// ******************************************************************
OOVPATable D3D8_1_0_4361[] = {

    // IDirect3D8::CreateDevice
	OOVPA_TABLE_PATCH(IDirect3D8_CreateDevice_1_0_4361, XTL::EmuIDirect3D8_CreateDevice),
	// IDirect3D8::CheckDeviceFormat (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3D8_CheckDeviceFormat_1_0_4134, XTL::EmuIDirect3D8_CheckDeviceFormat),
	// IDirect3DDevice8::LoadVertexShader (* unchanged since 4034 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_LoadVertexShader_1_0_4034, XTL::EmuIDirect3DDevice8_LoadVertexShader),
	// IDirect3DDevice8::SelectVertexShader (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SelectVertexShader_1_0_4134, XTL::EmuIDirect3DDevice8_SelectVertexShader),
	// IDirect3DDevice8::SetRenderTarget (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderTarget_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderTarget),
	// IDirect3DDevice8::AddRef
	OOVPA_TABLE_PATCH(IDirect3DDevice8_AddRef_1_0_4361, XTL::EmuIDirect3DDevice8_AddRef),
    // IDirect3D::ClearStateBlockFlags
	OOVPA_TABLE_XREF(IDirect3D_ClearStateBlockFlags_1_0_4361),
	// IDirect3DDevice8::SetGammaRamp (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetGammaRamp_1_0_3925, XTL::EmuIDirect3DDevice8_SetGammaRamp),
    // IDirect3D::RecordStateBlock
	OOVPA_TABLE_XREF(IDirect3D_RecordStateBlock_1_0_4361),
	// IDirect3DDevice8::BeginStateBlock
	OOVPA_TABLE_PATCH(IDirect3DDevice8_BeginStateBlock_1_0_4361, XTL::EmuIDirect3DDevice8_BeginStateBlock),
	// IDirect3DDevice8::CaptureStateBlock
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CaptureStateBlock_1_0_4361, XTL::EmuIDirect3DDevice8_CaptureStateBlock),
	// D3DDevice_DeleteStateBlock
	OOVPA_TABLE_PATCH(D3DDevice_DeleteStateBlock_1_0_4361, XTL::EmuIDirect3DDevice8_DeleteStateBlock),
	// IDirect3DDevice8::ApplyStateBlock
	OOVPA_TABLE_PATCH(IDirect3DDevice8_ApplyStateBlock_1_0_4361, XTL::EmuIDirect3DDevice8_ApplyStateBlock),
	// IDirect3DDevice8::EndStateBlock
	OOVPA_TABLE_PATCH(IDirect3DDevice8_EndStateBlock_1_0_4361, XTL::EmuIDirect3DDevice8_EndStateBlock),
	// IDirect3DDevice8::CopyRects (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CopyRects_1_0_4134, XTL::EmuIDirect3DDevice8_CopyRects),
	// IDirect3DDevice8::CreateImageSurface (* unchanged since 4034 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateImageSurface_1_0_4034, XTL::EmuIDirect3DDevice8_CreateImageSurface),
	// IDirect3DDevice8::GetBackBuffer (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetBackBuffer_1_0_4134, XTL::EmuIDirect3DDevice8_GetBackBuffer),
	// IDirect3DDevice8::GetRenderTarget
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetRenderTarget_1_0_4361, XTL::EmuIDirect3DDevice8_GetRenderTarget),
	// IDirect3DDevice8::GetDepthStencilSurface (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetDepthStencilSurface_1_0_4134, XTL::EmuIDirect3DDevice8_GetDepthStencilSurface),
	// IDirect3DDevice8::CreateVertexShader (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateVertexShader_1_0_3925, XTL::EmuIDirect3DDevice8_CreateVertexShader),
	// IDirect3DDevice8::SetVertexShaderConstant (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVertexShaderConstant_1_0_4134, XTL::EmuIDirect3DDevice8_SetVertexShaderConstant),
	// IDirect3DDevice8::CreatePixelShader (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreatePixelShader_1_0_3925, XTL::EmuIDirect3DDevice8_CreatePixelShader),
	// IDirect3DDevice8::SetPixelShader (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetPixelShader_1_0_4134, XTL::EmuIDirect3DDevice8_SetPixelShader),
	// IDirect3DDevice8::SetViewport (* unchanged since 4034 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetViewport_1_0_4034, XTL::EmuIDirect3DDevice8_SetViewport),
	// IDirect3DDevice8::GetViewport
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetViewport_1_0_4361, XTL::EmuIDirect3DDevice8_GetViewport),
	// IDirect3DDevice8::SetTextureState_BumpEnv (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTextureState_BumpEnv_1_0_4134, XTL::EmuIDirect3DDevice8_SetTextureState_BumpEnv),
	// IDirect3DDevice8::SetTextureState_BorderColor
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTextureState_BorderColor_1_0_4361, XTL::EmuIDirect3DDevice8_SetTextureState_BorderColor),
	// IDirect3DDevice8::CreateTexture (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateTexture_1_0_3925, XTL::EmuIDirect3DDevice8_CreateTexture),
	// IDirect3DDevice8::SetIndices (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetIndices_1_0_4134, XTL::EmuIDirect3DDevice8_SetIndices),
	// IDirect3DDevice8::SetTexture (* unchanged since 4034 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTexture_1_0_4034, XTL::EmuIDirect3DDevice8_SetTexture),
	// IDirect3DDevice8::SwitchTexture
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SwitchTexture_1_0_4361, XTL::EmuIDirect3DDevice8_SwitchTexture),
	// IDirect3DDevice8::GetDisplayMode (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetDisplayMode_1_0_4134, XTL::EmuIDirect3DDevice8_GetDisplayMode),
	// IDirect3DDevice8::Clear (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_Clear_1_0_4134, XTL::EmuIDirect3DDevice8_Clear),
	// IDirect3DDevice8::Swap
	OOVPA_TABLE_PATCH(IDirect3DDevice8_Swap_1_0_4361, XTL::EmuIDirect3DDevice8_Swap),
	// IDirect3DDevice8::CreateVertexBuffer (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateVertexBuffer_1_0_3925, XTL::EmuIDirect3DDevice8_CreateVertexBuffer),
	// IDirect3DDevice8::EnableOverlay
	OOVPA_TABLE_PATCH(IDirect3DDevice8_EnableOverlay_1_0_4361, XTL::EmuIDirect3DDevice8_EnableOverlay),
	// IDirect3DDevice8::UpdateOverlay
	OOVPA_TABLE_PATCH(IDirect3DDevice8_UpdateOverlay_1_0_4361, XTL::EmuIDirect3DDevice8_UpdateOverlay),
	// IDirect3DDevice8::BlockUntilVerticalBlank
	OOVPA_TABLE_PATCH(IDirect3DDevice8_BlockUntilVerticalBlank_1_0_4361, XTL::EmuIDirect3DDevice8_BlockUntilVerticalBlank),
	// IDirect3DDevice8::SetTextureState_TexCoordIndex
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_4361, XTL::EmuIDirect3DDevice8_SetTextureState_TexCoordIndex),
	// IDirect3DDevice8::SetRenderState_VertexBlend (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_VertexBlend_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_VertexBlend),
	// IDirect3DDevice8::SetRenderState_CullMode (* unchanged since 4034 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_CullMode_1_0_4034, XTL::EmuIDirect3DDevice8_SetRenderState_CullMode),
	// IDirect3DDevice8::SetRenderState_TextureFactor (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_TextureFactor_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_TextureFactor),
	// IDirect3DDevice8::SetRenderState_EdgeAntiAlias (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_EdgeAntiAlias_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_EdgeAntiAlias),
	// IDirect3DDevice8::SetRenderState_FillMode (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_FillMode_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_FillMode),
	// IDirect3DDevice8::SetRenderState_FogColor (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_FogColor_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_FogColor),
	// IDirect3DDevice8::SetRenderState_Simple (* unchanged since 4034 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_Simple_1_0_4034, XTL::EmuIDirect3DDevice8_SetRenderState_Simple),
	// IDirect3DDevice8::SetRenderState_ZEnable (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_ZEnable_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_ZEnable),
	// IDirect3DDevice8::SetRenderState_StencilEnable (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_StencilEnable_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_StencilEnable),
	// IDirect3DDevice8::SetRenderState_MultiSampleAntiAlias (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias),
	// IDirect3DDevice8::SetTransform (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTransform_1_0_4134, XTL::EmuIDirect3DDevice8_SetTransform),
	// IDirect3DDevice8::GetTransform (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetTransform_1_0_4134, XTL::EmuIDirect3DDevice8_GetTransform),
	// IDirect3DDevice8::SetStreamSource (* unchanged since 4034 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetStreamSource_1_0_4034, XTL::EmuIDirect3DDevice8_SetStreamSource),
	// IDirect3DDevice8::SetVertexShader (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVertexShader_1_0_4134, XTL::EmuIDirect3DDevice8_SetVertexShader),
	// IDirect3DDevice8::DrawVertices (* unchanged since 4034 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_DrawVertices_1_0_4034, XTL::EmuIDirect3DDevice8_DrawVertices),
	// IDirect3DDevice8::DrawVerticesUP (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_DrawVerticesUP_1_0_4134, XTL::EmuIDirect3DDevice8_DrawVerticesUP),
	// IDirect3DDevice8::DrawIndexedVertices (* unchanged since 4034 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_DrawIndexedVertices_1_0_4034, XTL::EmuIDirect3DDevice8_DrawIndexedVertices),
	// IDirect3DDevice8::SetLight (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetLight_1_0_4134, XTL::EmuIDirect3DDevice8_SetLight),
	// IDirect3DDevice8::SetMaterial (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetMaterial_1_0_4134, XTL::EmuIDirect3DDevice8_SetMaterial),
	// IDirect3DDevice8::LightEnable (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_LightEnable_1_0_4134, XTL::EmuIDirect3DDevice8_LightEnable),
	// IDirect3DVertexBuffer8::Lock (* unchanged since 4034 *)
	OOVPA_TABLE_PATCH(IDirect3DVertexBuffer8_Lock_1_0_4034, XTL::EmuIDirect3DVertexBuffer8_Lock),
	// IDirect3DResource8::Register (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DResource8_Register_1_0_3925, XTL::EmuIDirect3DResource8_Register),
	// IDirect3DResource8::Release
	OOVPA_TABLE_PATCH(IDirect3DResource8_Release_1_0_4361, XTL::EmuIDirect3DResource8_Release),
	// IDirect3DResource8::IsBusy
	OOVPA_TABLE_PATCH(IDirect3DResource8_IsBusy_1_0_4361, XTL::EmuIDirect3DResource8_IsBusy),
	// Get2DSurfacDesc (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(Get2DSurfaceDesc_1_0_4134, XTL::EmuGet2DSurfaceDesc),
	// IDirect3DSurface8::GetDesc (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DSurface8_GetDesc_1_0_4134, XTL::EmuIDirect3DSurface8_GetDesc),
	// IDirect3DSurface8::LockRect (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DSurface8_LockRect_1_0_3925, XTL::EmuIDirect3DSurface8_LockRect),
	// IDirect3DBaseTexture8::GetLevelCount
	OOVPA_TABLE_PATCH(IDirect3DBaseTexture8_GetLevelCount_1_0_4361, XTL::EmuIDirect3DBaseTexture8_GetLevelCount),
	// IDirect3DTexture8::LockRect (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DTexture8_LockRect_1_0_3925, XTL::EmuIDirect3DTexture8_LockRect),
	// IDirect3DTexture8::GetSurfaceLevel (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DTexture8_GetSurfaceLevel_1_0_3925, XTL::EmuIDirect3DTexture8_GetSurfaceLevel),
	// IDirect3DDevice8_SetRenderState_StencilFail (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_StencilFail_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_StencilFail),
	// IDirect3DDevice8_SetRenderState_NormalizeNormals (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_NormalizeNormals_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_NormalizeNormals),
	// IDirect3DDevice8_GetDeviceCaps (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetDeviceCaps_1_0_3925, XTL::EmuIDirect3DDevice8_GetDeviceCaps),
	// IDirect3DDevice8::CreatePalette (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreatePalette_1_0_3925, XTL::EmuIDirect3DDevice8_CreatePalette),
	// IDirect3DDevice8::CreateIndexBuffer
	// TODO: This needs to be verified on 4361, not just 4242!
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateIndexBuffer_1_0_3925, XTL::EmuIDirect3DDevice8_CreateIndexBuffer),
	// IDirect3DDevice8::SetRenderState_ZBias
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_ZBias_1_0_3925, XTL::EmuIDirect3DDevice8_SetRenderState_ZBias),
	// IDirect3DResource8::AddRef
	OOVPA_TABLE_PATCH(IDirect3DResource8_AddRef_1_0_3925, XTL::EmuIDirect3DResource8_AddRef),
	// IDirect3DResource8::Release
	OOVPA_TABLE_PATCH(IDirect3DResource8_Release_1_0_3925, XTL::EmuIDirect3DResource8_Release),
	// IDirect3DPalette8::Lock (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DPalette8_Lock_1_0_3925, XTL::EmuIDirect3DPalette8_Lock),
	// IDirect3DCubeTexture8::LockRect
	// TODO: This needs to be verified on 4361, not just 4242!
	OOVPA_TABLE_PATCH(IDirect3DCubeTexture8_LockRect_1_0_3925, XTL::EmuIDirect3DCubeTexture8_LockRect),
	// IDirect3DDevice8::SetPalette
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetPalette_1_0_4361, XTL::EmuIDirect3DDevice8_SetPalette),
	// IDirect3DDevice8::Reset (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_Reset_1_0_4134, XTL::EmuIDirect3DDevice8_Reset),
	// IDirect3D8::KickOffAndWaitForIdle (* unchanged since 4034 *)
	OOVPA_TABLE_PATCH(IDirect3D8_KickOffAndWaitForIdle_1_0_4034, XTL::EmuIDirect3D8_KickOffAndWaitForIdle),
	// IDirect3DDevice8::SetTextureState_TwoSidedLighting (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTextureState_TwoSidedLighting_1_0_4134, XTL::EmuIDirect3DDevice8_SetTextureState_TwoSidedLighting),
	// IDirect3DDevice8::SetRenderState_BackFillMode (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_BackFillMode_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_BackFillMode),
	// IDirect3DDevice8::SetTextureState_ColorKeyColor (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTextureState_ColorKeyColor_1_0_4134, XTL::EmuIDirect3DDevice8_SetTextureState_ColorKeyColor),
	// IDirect3DDevice8::SetRenderState_FrontFace (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_FrontFace_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_FrontFace),
	// IDirect3DDevice8::SetRenderState_LogicOp (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_LogicOp_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_LogicOp),
	// IDirect3DDevice8::SetRenderState_StencilFail (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_StencilFail_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_StencilFail),
	// IDirect3DDevice8::SetRenderState_OcclusionCullEnable (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_OcclusionCullEnable_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_OcclusionCullEnable),
	// IDirect3DDevice8::SetRenderState_StencilCullEnable (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_StencilCullEnable_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_StencilCullEnable),
	// IDirect3DDevice8::SetRenderState_RopZCmpAlwaysRead (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead_1_0_3925, XTL::EmuIDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead),
	// IDirect3DDevice8::SetRenderState_RopZRead (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_RopZRead_1_0_3925, XTL::EmuIDirect3DDevice8_SetRenderState_RopZRead),
	// IDirect3DDevice8::SetRenderState_DoNotCullUncompressed (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_DoNotCullUncompressed_1_0_3925, XTL::EmuIDirect3DDevice8_SetRenderState_DoNotCullUncompressed),
	// IDirect3DDevice8::SetRenderState_Dxt1NoiseEnable (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_Dxt1NoiseEnable),
	 // IDirect3DDevice8::SetRenderState_MultiSampleMask (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_MultiSampleMask_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleMask),
	 // IDirect3DDevice8::SetRenderState_MultiSampleMode (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_MultiSampleMode_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleMode),
	 // IDirect3DDevice8::SetRenderState_MultiSampleRenderTargetMode (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode),
	// IDirect3DDevice8::SetRenderState_ShadowFunc (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_ShadowFunc_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_ShadowFunc),
	// IDirect3DDevice8::SetRenderState_YuvEnable (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_YuvEnable_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_YuvEnable),
	// IDirect3DDevice8::DrawIndexedVerticesUP (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_DrawIndexedVerticesUP_1_0_4134, XTL::EmuIDirect3DDevice8_DrawIndexedVerticesUP),
	// IDirect3DDevice8::SetPixelShaderConstant (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetPixelShaderConstant_1_0_4134, XTL::EmuIDirect3DDevice8_SetPixelShaderConstant),
	// IDirect3DDevice8::DeletePixelShader (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(D3DDevice_DeletePixelShader_1_0_3925, XTL::EmuIDirect3DDevice8_DeletePixelShader),
	// IDirect3DDevice8::SetRenderState_CullModeB (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_CullModeB_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_CullMode),
	// D3DDevice_DeleteVertexShader (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(X_D3DDevice_DeleteVertexShader_1_0_3925, XTL::EmuIDirect3DDevice8_DeleteVertexShader),
	// IDirect3DDevice8_SetScreenSpaceOffset (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetScreenSpaceOffset_1_0_4134, XTL::EmuIDirect3DDevice8_SetScreenSpaceOffset),
	// D3D::SetFence (XREF) (* unchanged since 4134 *)
	OOVPA_TABLE_XREF(D3D_SetFence_1_0_4134),
	// IDirect3DDevice8::SetRenderState_LineWidth (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_LineWidth_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_LineWidth),
	// IDirect3DDevice8::SetSwapCallback (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetSwapCallback_1_0_4134, XTL::EmuIDirect3DDevice8_SetSwapCallback),
	// IDirect3DDevice8::GetTile (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetTile_1_0_4134, XTL::EmuIDirect3DDevice8_GetTile),
	// IDirect3DDevice8::SetTile (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTile_1_0_4134, XTL::EmuIDirect3DDevice8_SetTileNoWait),
	// IDirect3DDevice8::IsBusy (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(D3DDevice_IsBusy_1_0_4134, XTL::EmuIDirect3DDevice8_IsBusy),
	// IDirect3DDevice8::GetGammaRamp (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetGammaRamp_1_0_3925, XTL::EmuIDirect3DDevice8_GetGammaRamp),
	// IDirect3DDevice8::GetCreationParameters (* unchanged since 4034 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetCreationParameters_1_0_4034, XTL::EmuIDirect3DDevice8_GetCreationParameters),
	// IDirect3DDevice8::GetVisibilityTestResult (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetVisibilityTestResult_1_0_3925, XTL::EmuIDirect3DDevice8_GetVisibilityTestResult),
	// IDirect3DDevice8::SetShaderConstantMode
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetShaderConstantMode_1_0_4361, XTL::EmuIDirect3DDevice8_SetShaderConstantMode),
	// IDirect3DDevice8::SetFlickerFilter
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetFlickerFilter_1_0_4361, XTL::EmuIDirect3DDevice8_SetFlickerFilter),
	// IDirect3DDevice8::SetSoftDisplayFilter
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetSoftDisplayFilter_1_0_4361, XTL::EmuIDirect3DDevice8_SetSoftDisplayFilter),
	// D3DDevice_PrimeVertexCache
	OOVPA_TABLE_PATCH(D3DDevice_PrimeVertexCache_1_0_4361, XTL::EmuIDirect3DDevice8_PrimeVertexCache),
	// D3DDevice_BeginPush
	OOVPA_TABLE_PATCH(D3DDevice_BeginPush_1_0_4361, XTL::EmuIDirect3DDevice8_BeginPush),
	// D3DDevice_EndPush
	OOVPA_TABLE_PATCH(D3DDevice_EndPush_1_0_4361, XTL::EmuIDirect3DDevice8_EndPush),
	// IDirect3DDevice8::Begin
	OOVPA_TABLE_PATCH(D3DDevice_Begin_1_0_4361, XTL::EmuIDirect3DDevice8_Begin),
	// IDirect3DDevice8::End
	OOVPA_TABLE_PATCH(D3DDevice_End_1_0_4361, XTL::EmuIDirect3DDevice8_End),
	// IDirect3DDevice8::SetVerticalBlankCallback
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVerticalBlankCallback_1_0_4361, XTL::EmuIDirect3DDevice8_SetVerticalBlankCallback),
	// IDirect3DDevice8::SetVertexData2f
	OOVPA_TABLE_PATCH(D3DDevice_SetVertexData2f_1_0_4361, XTL::EmuIDirect3DDevice8_SetVertexData2f),
	// IDirect3DDevice8::SetVertexData4f
	OOVPA_TABLE_PATCH(D3DDevice_SetVertexData4f_1_0_4361, XTL::EmuIDirect3DDevice8_SetVertexData4f),
	// IDirect3DDevice8::SetVertexData4ub
	OOVPA_TABLE_PATCH(D3DDevice_SetVertexData4ub_1_0_4361, XTL::EmuIDirect3DDevice8_SetVertexData4ub),
	// D3DDevice_GetVertexShaderSize (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(X_D3DDevice_GetVertexShaderSize_1_0_3925, XTL::EmuIDirect3DDevice8_GetVertexShaderSize),
	// IDirect3DDevice8::BlockOnFence (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(D3DDevice_BlockOnFence_1_0_3925, XTL::EmuIDirect3DDevice8_BlockOnFence),
	// Direct3D_GetAdapterIdentifier (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(Direct3D_GetAdapterIdentifier_1_0_3925, XTL::EmuIDirect3D8_GetAdapterIdentifier),
	// IDirect3DDevice8::Release
	OOVPA_TABLE_PATCH(D3DDevice_Release_1_0_4361, XTL::EmuIDirect3DDevice8_Release),
	// IDirect3DDevice8::BeginPushBuffer
	OOVPA_TABLE_PATCH(D3DDevice_BeginPushBuffer_1_0_4361, XTL::EmuIDirect3DDevice8_BeginPushBuffer),
	// IDirect3DDevice8::EndPushBuffer
	OOVPA_TABLE_PATCH(D3DDevice_EndPushBuffer_1_0_4361, XTL::EmuIDirect3DDevice8_EndPushBuffer),
	// IDirect3DDevice8::RunPushBuffer
	OOVPA_TABLE_PATCH(D3DDevice_RunPushBuffer_1_0_4361, XTL::EmuIDirect3DDevice8_RunPushBuffer),
	// IDirect3DDevice8::GetPushBufferOffset
	OOVPA_TABLE_PATCH(D3DDevice_GetPushBufferOffset_1_0_4361, XTL::EmuIDirect3DDevice8_GetPushBufferOffset),
	// IDirect3DDevice8::CreateCubeTexture
	OOVPA_TABLE_PATCH(D3DDevice_CreateCubeTexture_1_0_4361, XTL::EmuIDirect3DDevice8_CreateCubeTexture),
	// IDirect3DCubeTexture8::GetCubeMapSurface
	OOVPA_TABLE_PATCH(D3DCubeTexture_GetCubeMapSurface_1_0_4361, XTL::EmuIDirect3DCubeTexture8_GetCubeMapSurface),
	// Lock2DSurface (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(Lock2DSurface_1_0_3925, XTL::EmuLock2DSurface),
};

// ******************************************************************
// * D3D8_1_0_4361_SIZE
// ******************************************************************
uint32 D3D8_1_0_4361_SIZE = sizeof(D3D8_1_0_4361);
