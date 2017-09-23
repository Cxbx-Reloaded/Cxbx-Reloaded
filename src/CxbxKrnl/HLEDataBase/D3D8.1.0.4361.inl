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
// * CMiniport::InitHardware
// ******************************************************************
OOVPA_NO_XREF(CMiniport_InitHardware, 4361, 24)
		{ 0x00, 0x55 },
		{ 0x01, 0x8B },
		{ 0x02, 0xEC },
		{ 0x03, 0x83 },
		{ 0x04, 0xEC },
		{ 0x05, 0x10 },
		{ 0x06, 0x53 },
		{ 0x07, 0x56 },
		{ 0x08, 0x8B },
		{ 0x09, 0xF1 },
		{ 0x0A, 0x56 },
		{ 0x0B, 0x68 },

		{ 0x10, 0x8D },
		{ 0x11, 0x86 },
		{ 0x12, 0x84 },
		{ 0x13, 0x00 },
		{ 0x14, 0x00 },
		{ 0x15, 0x00 },
		{ 0x16, 0x50 },
		{ 0x17, 0xFF },
		{ 0x18, 0x15 },

		{ 0x1D, 0x80 },
		{ 0x1E, 0xA6 },
		{ 0x1F, 0xDC },
OOVPA_END;
#if 0 // Moved to 4034
// ******************************************************************
// * CMiniport::CreateCtxDmaObject
// ******************************************************************
OOVPA_NO_XREF(CMiniport_CreateCtxDmaObject, 4361, 32) // Also for 4627, 5344, 5558, 5659, 5788, 5849, 5933
		{ 0x00, 0x55 },
		{ 0x01, 0x8B },
		{ 0x02, 0xEC },
		{ 0x03, 0x51 },
		{ 0x04, 0x51 },
		{ 0x05, 0x53 },
		{ 0x06, 0x56 },
		{ 0x07, 0x57 },
		{ 0x08, 0x33 },
		{ 0x09, 0xC0 },
		{ 0x0A, 0x50 },
		{ 0x0B, 0x89 },
		{ 0x0C, 0x45 },
		{ 0x0D, 0xF8 },
		{ 0x0E, 0x89 },
		{ 0x0F, 0x45 },
		{ 0x10, 0xFC },
		{ 0x11, 0x8D },
		{ 0x12, 0x45 },
		{ 0x13, 0xFC },
		{ 0x14, 0x50 },
		{ 0x15, 0x8D },
		{ 0x16, 0x45 },
		{ 0x17, 0xF8 },
		{ 0x18, 0x50 },
		{ 0x19, 0xFF },
		{ 0x1A, 0x75 },
		{ 0x1B, 0x10 },
		{ 0x1C, 0x8B },
		{ 0x1D, 0xD1 },
		{ 0x1E, 0x8B },
		{ 0x1F, 0x3A },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * Direct3D_CreateDevice
// ******************************************************************
OOVPA_NO_XREF(Direct3D_CreateDevice, 4361, 8)
        // Direct3D_CreateDevice+0x07 : jnz +0x0A
        { 0x07, 0x75 },
        { 0x08, 0x0A },

        // Direct3D_CreateDevice+0x86 : repe stosd
        { 0x86, 0xF3 },
        { 0x87, 0xAB },

        // Direct3D_CreateDevice+0x89 : mov eax, esi
        { 0x89, 0x8B },
        { 0x8A, 0xC6 },

        // Direct3D_CreateDevice+0xA0 : retn 0x18
        { 0xA0, 0xC2 },
        { 0xA1, 0x18 },
OOVPA_END;
#endif
#if 0 // Moved to 4134
// ******************************************************************
// * MakeRequestedSpace
// ******************************************************************
OOVPA_XREF(MakeRequestedSpace, 4361, 28, // Also for 4627

	XREF_D3D_MakeRequestedSpace,
	XRefZero)

		{ 0x00, 0x83 },
		{ 0x01, 0xEC },
		{ 0x02, 0x08 },
		{ 0x03, 0x56 },
		{ 0x04, 0x8B },
		{ 0x05, 0x35 },

		{ 0x0A, 0xF6 },
		{ 0x0B, 0x46 },
		{ 0x0C, 0x08 },
		{ 0x0D, 0x04 },
		{ 0x0E, 0x8B },
		{ 0x0F, 0x0E },
		{ 0x10, 0x57 },
		{ 0x11, 0x74 },
		{ 0x12, 0x26 },
		{ 0x13, 0x8B },
		{ 0x14, 0x86 },
		{ 0x15, 0x50 },
		{ 0x16, 0x03 },
		{ 0x17, 0x00 },
		{ 0x18, 0x00 },
		{ 0x19, 0x8B },
		{ 0x1A, 0x78 },
		{ 0x1B, 0x04 },
		{ 0x1C, 0x8B },
		{ 0x1D, 0x96 },
		{ 0x1E, 0x54 },
		{ 0x1F, 0x03 },
OOVPA_END;
#endif
// ******************************************************************
// * D3DDevice_SetVerticalBlankCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVerticalBlankCallback, 4361, 12)

        // D3DDevice_SetVerticalBlankCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // D3DDevice_SetVerticalBlankCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        // D3DDevice_SetVerticalBlankCallback+0x0A : mov [ecx+0x242C], eax
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0x30 }, // 2C vs 30
        { 0x0D, 0x24 },

        // D3DDevice_SetVerticalBlankCallback+0x10 : retn 0x04
        { 0x10, 0xC2 },
        { 0x11, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetSwapCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetSwapCallback, 4361, 12)

        // D3DDevice_SetSwapCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // D3DDevice_SetSwapCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        // D3DDevice_SetSwapCallback+0x0A : mov [ecx+0x242C], eax
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0x2C }, // 2C vs 30
        { 0x0D, 0x24 },

        // D3DDevice_SetSwapCallback+0x10 : retn 0x04
        { 0x10, 0xC2 },
        { 0x11, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3D_GetAdapterDisplayMode
// ******************************************************************
OOVPA_NO_XREF(D3D_GetAdapterDisplayMode, 4361, 13)

        // D3D_GetAdapterDisplayMode+0x08 : mov eax, 0x8876086C
        { 0x08, 0xB8 },
        { 0x09, 0x6C },
        { 0x0A, 0x08 },
        { 0x0B, 0x76 },
        { 0x0C, 0x88 },

        // D3D_GetAdapterDisplayMode+0x18 : jnz +0x17
        { 0x18, 0x75 },
        { 0x19, 0x17 },

        // D3D_GetAdapterDisplayMode+0x31 : mov ecx, [edx+0x2080]
        { 0x31, 0x8B },
        { 0x32, 0x8A },
        { 0x33, 0x80 },
        { 0x34, 0x20 },

        // D3D_GetAdapterDisplayMode+0xBD : retn 0x08
        { 0xBD, 0xC2 },
        { 0xBE, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_AddRef
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_AddRef, 4361, 10)

        // D3DDevice_AddRef+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3DDevice_AddRef+0x05 : mov ecx, [eax+0x0440]
        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x40 },
        { 0x08, 0x04 },

        // D3DDevice_AddRef+0x0B : inc ecx
        { 0x0B, 0x41 },

        // D3DDevice_AddRef+0x05 : mov [eax+0x0440], ecx
        { 0x0C, 0x89 },
        { 0x0D, 0x88 },
        { 0x0E, 0x40 },
        { 0x0F, 0x04 },
OOVPA_END;
#if 0 // Moved to 4134
// ******************************************************************
// * D3D_ClearStateBlockFlags
// ******************************************************************
OOVPA_XREF(D3D_ClearStateBlockFlags, 4361, 9,

    XREF_CLEARSTATEBLOCKFLAGS,
    XRefZero)

        // D3D_ClearStateBlockFlags+0x0A : movzx ecx, 0x82
        { 0x0A, 0xB9 },
        { 0x0B, 0x82 },

        // D3D_ClearStateBlockFlags+0x16 : mov ecx, [edx+0x0390]
        { 0x16, 0x8B },
        { 0x17, 0x8A },
        { 0x18, 0x90 },
        { 0x19, 0x03 },

        // D3D_ClearStateBlockFlags+0x2F : add ecx, 0x90
        { 0x2F, 0x81 },
        { 0x30, 0xC1 },
        { 0x31, 0x90 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3D_RecordStateBlock
// ******************************************************************
OOVPA_XREF(D3D_RecordStateBlock, 4361, 10,

    XREF_RECORDSTATEBLOCK,
    XRefZero)

        // D3D_RecordStateBlock+0x0F : mov eax, [edi+0x0390]
        { 0x0F, 0x8B },
        { 0x10, 0x87 },
        { 0x11, 0x90 },
        { 0x12, 0x03 },

        // D3D_RecordStateBlock+0x95 : add ebx, ecx
        { 0x95, 0x03 },
        { 0x96, 0xD9 },

        // D3D_RecordStateBlock+0xD5 : mov dword ptr [esi+0x0C], 1
        { 0xD5, 0xC7 },
        { 0xD6, 0x46 },
        { 0xD7, 0x0C },
        { 0xD8, 0x01 },
OOVPA_END;
#endif
#if 0 // Moved to 4134
// ******************************************************************
// * D3DDevice_BeginStateBlock
// ******************************************************************
OOVPA_XREF(D3DDevice_BeginStateBlock, 4361, 6,

    XRefNoSaveIndex,
    XRefOne)

        // D3DDevice_BeginStateBlock+0x0F : call [ClearStateBlockFlags]
        XREF_ENTRY( 0x0A, XREF_CLEARSTATEBLOCKFLAGS ),

        // D3DDevice_BeginStateBlock+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3DDevice_BeginStateBlock+0x05 : mov [eax+8], 0x20
        { 0x05, 0x83 },
        { 0x06, 0x48 },
        { 0x07, 0x08 },
        { 0x08, 0x20 },
OOVPA_END;
#endif
#if 0 // Moved to 4134
// ******************************************************************
// * D3DDevice_CaptureStateBlock
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CaptureStateBlock, 4361, 9)

        // D3DDevice_CaptureStateBlock+0x36 : mov eax, [edi+eax*4+0x0A78]
        { 0x36, 0x8B },
        { 0x37, 0x84 },
        { 0x38, 0x87 },
        { 0x39, 0x78 },
        { 0x3A, 0x0A },

        // D3DDevice_CaptureStateBlock+0xA9 : cmp dword ptr [esi+0x0C], 0
        { 0xA9, 0x83 },
        { 0xAA, 0x7E },
        { 0xAB, 0x0C },
        { 0xAC, 0x00 },
OOVPA_END;
#endif
#if 1 //same as 3925
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
#endif

// ******************************************************************
// * D3DDevice_ApplyStateBlock
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_ApplyStateBlock, 4361, 9)

        // D3DDevice_ApplyStateBlock+0x0E : lea esi, [edi+0x3C]
        { 0x0E, 0x8D },
        { 0x0F, 0x77 },
        { 0x10, 0x3C },

        // D3DDevice_ApplyStateBlock+0x34 : cmp [edi+8], ebp
        { 0x34, 0x39 },
        { 0x35, 0x6F },
        { 0x36, 0x08 },

        // D3DDevice_ApplyStateBlock+0x9E : sub eax, 0x60
        { 0x9E, 0x83 },
        { 0x9F, 0xE8 },
        { 0xA0, 0x60 },
OOVPA_END;
#if 0 // Moved to 4134
// ******************************************************************
// * D3DDevice_EndStateBlock
// ******************************************************************
OOVPA_XREF(D3DDevice_EndStateBlock, 4361, 6,

    XRefNoSaveIndex,
    XRefOne)

        // D3DDevice_EndStateBlock+0x0F : call [ClearStateBlockFlags]
        XREF_ENTRY( 0x0A, XREF_RECORDSTATEBLOCK ),

        // D3DDevice_EndStateBlock+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3DDevice_EndStateBlock+0x05 : and [eax+8], 0xFFFFFFDF
        { 0x05, 0x83 },
        { 0x06, 0x60 },
        { 0x07, 0x08 },
        { 0x08, 0xDF },
OOVPA_END;
#endif
// ******************************************************************
// * D3DDevice_GetRenderTarget
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetRenderTarget, 4361, 9)

        // D3DDevice_GetRenderTarget+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3DDevice_GetRenderTarget+0x05 : mov eax, [eax + 0x2070]
        { 0x05, 0x8B },
        { 0x06, 0x80 },
        { 0x07, 0x70 },
        { 0x08, 0x20 },

        // D3DDevice_GetRenderTarget+0x11 : mov [ecx], eax
        { 0x11, 0x89 },
        { 0x12, 0x01 },

        // D3DDevice_GetRenderTarget+0x1D : retn 0x04
        { 0x1D, 0xC2 },
        { 0x1E, 0x04 },
OOVPA_END;
#if 0 // Moved to 4134
// ******************************************************************
// * D3DDevice_GetViewport
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetViewport, 4361, 10)

        // D3DDevice_GetViewport+0x05 : push esi; push edi
        { 0x05, 0x56 },
        { 0x06, 0x57 },

        // D3DDevice_GetViewport+0x0B : lea esi, [eax+0x9D0]
        { 0x0B, 0x8D },
        { 0x0C, 0xB0 },
        { 0x0D, 0xD0 },
        { 0x0E, 0x09 },

        // D3DDevice_GetViewport+0x11 : mov ecx, 6
        { 0x11, 0xB9 },
        { 0x12, 0x06 },

        // D3DDevice_GetViewport+0x1A : retn 0x04
        { 0x1A, 0xC2 },
        { 0x1B, 0x04 },
OOVPA_END;
#endif
// ******************************************************************
// * D3DDevice_SetTextureState_BorderColor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTextureState_BorderColor, 4361, 15)

        // D3DDevice_SetTextureState_BorderColor+0x0C : jb +0x05
        { 0x0C, 0x72 },
        { 0x0D, 0x05 },

        // D3DDevice_SetTextureState_BorderColor+0x19 : shl edx, 6
        { 0x19, 0xC1 },
        { 0x1A, 0xE2 },
        { 0x1B, 0x06 },

        // D3DDevice_SetTextureState_BorderColor+0x2B : add eax, 8; mov [esi], eax; shl ecx, 7
        { 0x2B, 0x83 },
        { 0x2C, 0xC0 },
        { 0x2D, 0x08 },
        { 0x2E, 0x89 },
        { 0x2F, 0x06 },
        { 0x30, 0xC1 },
        { 0x31, 0xE1 },
        { 0x32, 0x07 },

        // D3DDevice_SetTextureState_BorderColor+0x3A : retn 0x08
        { 0x3A, 0xC2 },
        { 0x3B, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SwitchTexture
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SwitchTexture, 4361, 10)

        // D3DDevice_SwitchTexture+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3DDevice_SwitchTexture+0x05 : add eax, 0x0C
        { 0x05, 0x83 },
        { 0x06, 0xC0 },
        { 0x07, 0x0C },

        // D3DDevice_SwitchTexture+0x0E : jnb +0x15
        { 0x0E, 0x73 },
        { 0x0F, 0x15 },

        // D3DDevice_SwitchTexture+0x22 : retn 0x04
        { 0x22, 0xC2 },
        { 0x23, 0x04 },

        // D3DDevice_SwitchTexture+0x2E : jmp +0xD0
        { 0x2E, 0xEB },
        { 0x2F, 0xD0 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Swap
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Swap, 4361, 11)

        // D3DDevice_Swap+0x10 : mov ebx, 5
        { 0x10, 0xBB },
        { 0x11, 0x05 },

        // D3DDevice_Swap+0x1D : test bl, 3
        { 0x1D, 0xF6 },
        { 0x1E, 0xC3 },
        { 0x1F, 0x03 },

        // D3DDevice_Swap+0x46 : inc dword ptr [esi+0x2AC4]
        { 0x46, 0xFF },
        { 0x47, 0x86 },
        { 0x48, 0xC4 },
        { 0x49, 0x2A },

        // D3DDevice_Swap+0xAE : retn 4
        { 0xAE, 0xC2 },
        { 0xAF, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_UpdateOverlay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_UpdateOverlay, 4361, 11)

        // D3DDevice_UpdateOverlay+0x0F : mov [eax+0x2A90], ecx
        { 0x0F, 0x89 },
        { 0x10, 0x88 },
        { 0x11, 0x90 },
        { 0x12, 0x2A },

        // D3DDevice_UpdateOverlay+0x86 : and ecx, 0xFFFFFFFE
        { 0x86, 0x83 },
        { 0x87, 0xE1 },
        { 0x88, 0xFE },

        // D3DDevice_UpdateOverlay+0xA2 : mov [esi+0x8920], ecx
        { 0xA2, 0x89 },
        { 0xA3, 0x8E },
        { 0xA4, 0x20 },
        { 0xA5, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BlockUntilVerticalBlank
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BlockUntilVerticalBlank, 4361, 11)

        // D3DDevice_BlockUntilVerticalBlank+0x05 : push 0; push 0; push 1
        { 0x05, 0x6A },
        { 0x06, 0x00 },
        { 0x07, 0x6A },
        { 0x08, 0x00 },
        { 0x09, 0x6A },
        { 0x0A, 0x01 },

        // D3DDevice_BlockUntilVerticalBlank+0x17 : add eax, 0x2434
        { 0x17, 0x05 },
        { 0x18, 0x34 },
        { 0x19, 0x24 },

        // D3DDevice_BlockUntilVerticalBlank+0x1D : call [KrnlImport]
        { 0x1D, 0xFF },

        // D3DDevice_BlockUntilVerticalBlank+0x23 : retn
        { 0x23, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_TexCoordIndex
// ******************************************************************
OOVPA_XREF(D3DDevice_SetTextureState_TexCoordIndex, 4242, 1+10,

	XRefNoSaveIndex,
	XRefOne)

		XREF_ENTRY(0x19, XREF_D3DTSS_TEXCOORDINDEX), // Derived

        // D3DDevice_SetTextureState_TexCoordIndex+0x0D : shl eax, 0x07
        { 0x0D, 0xC1 },
        { 0x0E, 0xE0 },
        { 0x0F, 0x07 },

        // D3DDevice_SetTextureState_TexCoordIndex+0x24 : cmp eax, ecx
        { 0x24, 0x3B },
        { 0x25, 0xC1 },

        // D3DDevice_SetTextureState_TexCoordIndex+0x6B : mov esi, 0x2400
        { 0x6B, 0xBE },
        { 0x6D, 0x24 },

        // D3DDevice_SetTextureState_TexCoordIndex+0xB3 : shl edx, 0x04
        { 0xB3, 0xC1 },
        { 0xB4, 0xE2 },
        { 0xB5, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DResource_Release
// ******************************************************************
OOVPA_NO_XREF(D3DResource_Release, 4361, 13)

        // D3DResource_Release+0x09 : and ecx, 0xFFFF
        { 0x09, 0x81 },
        { 0x0A, 0xE1 },
        { 0x0B, 0xFF },
        { 0x0C, 0xFF },

        // D3DResource_Release+0x0F : cmp ecx, 1
        { 0x0F, 0x83 },
        { 0x10, 0xF9 },
        { 0x11, 0x01 },

        // D3DResource_Release+0x12 : jnz +0x2E
        { 0x12, 0x75 },
        { 0x13, 0x2E },

        // D3DResource_Release+0x2F : test eax, 0x780000
        { 0x2F, 0xA9 },
        { 0x32, 0x78 },

        // D3DResource_Release+0x4B : retn 0x04
        { 0x4B, 0xC2 },
        { 0x4C, 0x04 },
OOVPA_END;
#if 0 // Moved to 4039
// ******************************************************************
// * D3DResource_IsBusy
// ******************************************************************
OOVPA_NO_XREF(D3DResource_IsBusy, 4361, 11)

        // D3DResource_IsBusy+0x24 : test eax, 0x780000
        { 0x24, 0xA9 },
        { 0x25, 0x00 },
        { 0x26, 0x00 },
        { 0x27, 0x78 },

        // D3DResource_IsBusy+0x35 : jnz +0x41
        { 0x35, 0x75 },
        { 0x36, 0x41 },

        // D3DResource_IsBusy+0x4E : mov eax, [ecx+0x14]
        { 0x4E, 0x8B },
        { 0x4F, 0x41 },
        { 0x50, 0x14 },

        // D3DResource_IsBusy+0x76 : jnb +0x09
        { 0x76, 0x73 },
        { 0x77, 0x09 },
OOVPA_END;
#endif
#if 0 // Moved to 3911
// ******************************************************************
// * D3DBaseTexture_GetLevelCount
// ******************************************************************
OOVPA_NO_XREF(D3DBaseTexture_GetLevelCount, 4361, 13)

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
#endif

// ******************************************************************
// * D3DDevice_SetShaderConstantMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetShaderConstantMode, 4361, 12)

        // D3DDevice_SetShaderConstantMode+0x26 : mov [ebx+0x20D8], eax
        { 0x26, 0x89 },
        { 0x27, 0x83 },
        { 0x28, 0x18 },
        { 0x29, 0x20 },

        // D3DDevice_SetShaderConstantMode+0x50 : mov dword ptr [eax+0x04], 0x3C
        { 0x50, 0xC7 },
        { 0x51, 0x40 },
        { 0x52, 0x04 },
        { 0x53, 0x3C },

        // D3DDevice_SetShaderConstantMode+0xE7 : add esi, 0x0124
        { 0xE7, 0x81 },
        { 0xE8, 0xC6 },
        { 0xE9, 0x24 },
        { 0xEA, 0x01 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3DDevice_SetFlickerFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetFlickerFilter, 4361, 11)

        // D3DDevice_SetFlickerFilter+0x1C : mov eax, [eax+0x2268]
        { 0x1D, 0x80 },
        { 0x1E, 0x7C },
        { 0x1F, 0x22 },

        // D3DDevice_SetFlickerFilter+0x22 : push 0; push esi; push 0x0B; push eax
        { 0x22, 0x6A },
        { 0x23, 0x00 },
        { 0x24, 0x56 },
        { 0x25, 0x6A },
        { 0x26, 0x0B },
        { 0x27, 0x50 },

        // D3DDevice_SetFlickerFilter+0x3F : retn 0x04
        { 0x3F, 0xC2 },
        { 0x40, 0x04 },
OOVPA_END;
#endif

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

#if 0 // Moved to 4134
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
#endif
#if 0 // Moved to 4134
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
#endif
#if 0 // No longer used, replaced by generic 4039 version
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
#endif
#if 0 // Moved to 4134
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
#endif
#if 0 // No longer used, replaced by generic 3911 version
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
#endif
#if 0 // No longer used, replaced by generic 4134 version
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
#endif
#if 0 // Moved to 4134
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
#endif
#if 0 // Moved to 3925
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
#endif
#if 0 // Moved to 4134
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
#endif
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
#if 0 // Moved to 4134
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
#endif
// ******************************************************************
// * D3DDevice_SetScissors
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetScissors, 4361, 10)

        // D3DDevice_SetScissors+0x0E : mov ebx, [esp+28h+arg_0]
        { 0x04, 0x8B },
        { 0x05, 0x5C },
        { 0x06, 0x24 },
        { 0x07, 0x2C },

        // D3DDevice_SetScissors+0x0E : test ebx, ebx
        { 0x08, 0x85 },
        { 0x09, 0xDB },

        // D3DDevice_SetScissors+0x0E : mov edx, [ebp+9D8h]
        { 0x1B, 0x8B },
        { 0x1C, 0x95 },
        { 0x1D, 0xD8 },
        { 0x1E, 0x09 },
OOVPA_END;


// ******************************************************************
// * D3DDevice_SetPixelShaderProgram
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPixelShaderProgram, 4361, 10)

	        { 0x00, 0x8B },
        { 0x01, 0x54 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },
        { 0x04, 0x85 },
        { 0x05, 0xD2 },
        { 0x29, 0x89 },
        { 0x2A, 0x4C },
        { 0x2B, 0x24 },
        { 0x2C, 0x04 },
OOVPA_END;
#if 0 // Moved to 4134
// ******************************************************************
// * D3DDevice_GetVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVertexShader, 4361, 7)

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x84 },
        { 0x0A, 0x00 },
        { 0x0D, 0x24 },
        { 0x10, 0x0A },
        { 0x13, 0x00 },
OOVPA_END;
#endif
// ******************************************************************
// * D3DDevice_SetVertexDataColor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexDataColor, 4361, 7)

        { 0x08, 0x06 },
        { 0x14, 0x8B },
        { 0x1C, 0x19 },
        { 0x26, 0xB6 },
        { 0x30, 0x00 },
        { 0x3A, 0xFF },
        { 0x44, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShaderInput
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShaderInput, 4361, 8)

        { 0x1E, 0x83 },
        { 0x3E, 0x10 },
        { 0x5E, 0x00 },
        { 0x7E, 0x24 },
        { 0x9E, 0x89 },
        { 0xBE, 0x81 },
        { 0xDE, 0xC6 },
        { 0xFE, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData2s
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData2s, 4361, 8)

        { 0x08, 0x06 },
        { 0x0E, 0xE8 },
        { 0x16, 0x08 },
        { 0x17, 0x8D },
        { 0x18, 0x14 },
        { 0x19, 0x8D },
        { 0x1A, 0x00 },
        { 0x1F, 0xBF },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData4s
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData4s, 4361, 9)

        { 0x08, 0x06 },
        { 0x0E, 0xE8 },
        { 0x16, 0x08 },
        { 0x17, 0x8D },
        { 0x18, 0x14 },
        { 0x19, 0xCD },
        { 0x1A, 0x80 },
        { 0x1B, 0x19 },
        { 0x1F, 0xBF },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BeginVisibilityTest@0
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginVisibilityTest, 4361, 7) 
        { 0x07, 0x8B },
        { 0x0A, 0x46 },
        { 0x13, 0xC7 },
        { 0x16, 0x17 },
        { 0x1C, 0x00 },
        { 0x22, 0x48 },
        { 0x28, 0x06 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_EndVisibilityTest@4
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_EndVisibilityTest, 4361, 7)
        { 0x0B, 0x8B },
        { 0x16, 0x5E },
        { 0x22, 0x07 },
        { 0x2E, 0x00 },
        { 0x3A, 0x81 },
        { 0x46, 0x89 },
        { 0x55, 0x5F },
OOVPA_END;


// ******************************************************************
// * D3DDevice_SetTexture
// ******************************************************************
#if 0 // Derived XREF_OFFSET_D3DDEVICE_M_TEXTURES is not Implemented yet.
OOVPA_XREF(D3DDevice_SetTexture, 4361, 1+28,

    XRefNoSaveIndex,
    XRefOne)

		XREF_ENTRY( 0x13, XREF_OFFSET_D3DDEVICE_M_TEXTURES ), // derived
#endif

OOVPA_NO_XREF(D3DDevice_SetTexture, 4361, 28)

		{ 0x00, 0x83 },
		{ 0x01, 0xEC },
		{ 0x02, 0x08 },
		{ 0x03, 0x53 },
		{ 0x04, 0x56 },
		{ 0x05, 0x8B },
		{ 0x06, 0x74 },
		{ 0x07, 0x24 },
		{ 0x08, 0x14 },
		{ 0x09, 0x57 },
		{ 0x0A, 0x8B },
		{ 0x0B, 0x3D },

		{ 0x10, 0x8B },
		{ 0x11, 0x84 },
		{ 0x12, 0xB7 },
		{ 0x13, 0x78 },
		{ 0x14, 0x0A },
		{ 0x15, 0x00 },
		{ 0x16, 0x00 },
		{ 0x17, 0x85 },
		{ 0x18, 0xC0 },
		{ 0x19, 0x89 },
		{ 0x1A, 0x7C },
		{ 0x1B, 0x24 },
		{ 0x1C, 0x0C },
		{ 0x1D, 0x89 },
		{ 0x1E, 0x44 },
		{ 0x1F, 0x24 },
OOVPA_END;

// ******************************************************************
// * D3D::BlockOnTime
// ******************************************************************
OOVPA_XREF(D3D_BlockOnTime, 4361, 13,

    XREF_D3D_BLOCKONTIME,
    XRefZero)

        { 0x07, 0x3D },

        { 0x17, 0xD1 },
        { 0x18, 0x8B },
        { 0x19, 0xC8 },
        { 0x1A, 0x2B },
        { 0x1B, 0xCE },
        { 0x1C, 0x3B },
        { 0x1D, 0xCA },
        { 0x1E, 0x0F },
        { 0x1F, 0x83 },

        { 0x2F, 0x53 },

        { 0x38, 0x8B },
        { 0x44, 0xE8 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SelectVertexShaderDirect
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SelectVertexShaderDirect, 4361, 13)

        { 0x00, 0x56 },
        { 0x05, 0x85 },
        { 0x0C, 0x00 },

        { 0x14, 0xF3 },
        { 0x15, 0xA5 },
        { 0x16, 0x5F },
        { 0x17, 0x5E },
        { 0x18, 0xB9 },

        { 0x1D, 0x83 },
        { 0x1E, 0xC9 },
        { 0x1F, 0x01 },

        { 0x29, 0x5E },
        { 0x32, 0xE9 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShaderInputDirect
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShaderInputDirect, 4361, 13)

        { 0x00, 0x56 },
        { 0x05, 0x85 },
        { 0x0C, 0x00 },

        { 0x14, 0xF3 },
        { 0x15, 0xA5 },
        { 0x16, 0x5F },
        { 0x17, 0x5E },
        { 0x18, 0xBA },

        { 0x1D, 0x83 },
        { 0x1E, 0xCA },
        { 0x1F, 0x01 },

        { 0x29, 0x5E },
        { 0x32, 0xE9 },
OOVPA_END;

// ******************************************************************
// * D3D8_4361
// ******************************************************************
OOVPATable D3D8_4361[] = {

	REGISTER_OOVPA(CMiniport_InitHardware, 4361, PATCH),
	REGISTER_OOVPA(CMiniport_CreateCtxDmaObject, 4034, PATCH),
	REGISTER_OOVPA(D3D_CMiniport_GetDisplayCapabilities, 3911, PATCH),
	
	REGISTER_OOVPA(Direct3D_CreateDevice, 3911, PATCH),
	REGISTER_OOVPA(D3D_CheckDeviceFormat, 4134, DISABLED),
	REGISTER_OOVPA(MakeRequestedSpace, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_LoadVertexShader, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SelectVertexShader, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderTarget, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_AddRef, 4361, PATCH),
	REGISTER_OOVPA(D3D_ClearStateBlockFlags, 3911, XREF),
	REGISTER_OOVPA(D3DDevice_SetGammaRamp, 3925, PATCH),
	REGISTER_OOVPA(D3D_RecordStateBlock, 3911, XREF),
	REGISTER_OOVPA(D3DDevice_BeginStateBlock, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_CaptureStateBlock, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteStateBlock, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_ApplyStateBlock, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_EndStateBlock, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateStateBlock, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CopyRects, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateImageSurface, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_GetBackBuffer, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetRenderTarget, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDepthStencilSurface, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVertexShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_CreatePixelShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShader, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShaderProgram, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetViewport, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_GetViewport, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BumpEnv, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BorderColor, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateTexture, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetIndices, 4134, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetTexture, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SwitchTexture, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDisplayMode, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_Clear, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_Swap, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVertexBuffer, 3925, DISABLED),
	REGISTER_OOVPA(D3DDevice_EnableOverlay, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_UpdateOverlay, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_BlockUntilVerticalBlank, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_TexCoordIndex, 4242, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_VertexBlend, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_CullMode, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TextureFactor, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_EdgeAntiAlias, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FillMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FogColor, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Simple, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleAntiAlias, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTransform, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTransform, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetStreamSource, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShader, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawVertices, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawVerticesUP, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawIndexedVertices, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_GetLight, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetLightEnable, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetLight, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetMaterial, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetMaterial, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetBackMaterial, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetBackMaterial, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_LightEnable, 3911, PATCH),
	REGISTER_OOVPA(D3DVertexBuffer_Lock, 4034, PATCH),
	REGISTER_OOVPA(D3DResource_Register, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_Release, 4361, PATCH),
	REGISTER_OOVPA(D3DResource_IsBusy, 4039, PATCH),
	REGISTER_OOVPA(Get2DSurfaceDesc, 4134, PATCH),
	REGISTER_OOVPA(D3DSurface_GetDesc, 3925, PATCH),
	REGISTER_OOVPA(D3DSurface_LockRect, 3925, PATCH),
	REGISTER_OOVPA(D3DBaseTexture_GetLevelCount, 3911, PATCH),
	REGISTER_OOVPA(D3DTexture_LockRect, 3925, PATCH),
	REGISTER_OOVPA(D3DTexture_GetSurfaceLevel, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilFail, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_NormalizeNormals, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDeviceCaps, 3925, DISABLED),
	REGISTER_OOVPA(D3DDevice_CreatePalette, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateIndexBuffer, 3925, DISABLED), // TODO: This needs to be verified on 4361, not just 4242!
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZBias, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_AddRef, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_Release, 3925, PATCH),
	REGISTER_OOVPA(D3DPalette_Lock, 3925, PATCH),
	REGISTER_OOVPA(D3DVolumeTexture_LockBox, 3925, PATCH),	// Just calls Lock3DSurface
	REGISTER_OOVPA(D3DCubeTexture_LockRect, 3925, PATCH),	// Just calls Lock2DSurface
	REGISTER_OOVPA(D3DDevice_SetPalette, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_Reset, 4039, PATCH),
	REGISTER_OOVPA(D3D_KickOffAndWaitForIdle, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TwoSidedLighting, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_BackFillMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_ColorKeyColor, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FrontFace, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LogicOp, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilFail, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_OcclusionCullEnable, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilCullEnable, 4034, PATCH),
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
	REGISTER_OOVPA(D3DDevice_SetPixelShaderConstant, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_DeletePixelShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteVertexShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScreenSpaceOffset, 4039, PATCH),
	REGISTER_OOVPA(D3D_SetFence, 4134, XREF),
	REGISTER_OOVPA(D3DDevice_InsertFence, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LineWidth, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetSwapCallback, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTile, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTile, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_IsBusy, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_GetGammaRamp, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_GetCreationParameters, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginVisibilityTest, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_EndVisibilityTest, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVisibilityTestResult, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetShaderConstantMode, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetFlickerFilter, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetSoftDisplayFilter, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_PrimeVertexCache, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginPush, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_EndPush, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_Begin, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_End, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVerticalBlankCallback, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData2f, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4f, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4ub, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderSize, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_BlockOnFence, 4134, PATCH),
	REGISTER_OOVPA(D3D_GetAdapterIdentifier, 3925, DISABLED),
	REGISTER_OOVPA(D3DDevice_Release, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginPushBuffer, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_EndPushBuffer, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_RunPushBuffer, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_GetPushBufferOffset, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateCubeTexture, 4361, PATCH),
	REGISTER_OOVPA(D3DCubeTexture_GetCubeMapSurface, 4134, PATCH),
	REGISTER_OOVPA(Lock3DSurface, 3911, PATCH),
	REGISTER_OOVPA(Lock2DSurface, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScissors, 4361, PATCH),
	REGISTER_OOVPA(D3D_SetPushBufferSize, 3925, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetRenderState_PSTextureModes, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShaderProgram, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShader, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderConstant, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderInput, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_RunVertexStateShader, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderType, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderDeclaration, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderFunction, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetPixelShader, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SelectVertexShaderDirect, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderInputDirect, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexDataColor, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderInput, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData2s, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4s, 4361, PATCH),
	REGISTER_OOVPA(D3D_BlockOnTime, 4361, PATCH),
	REGISTER_OOVPA(D3D_BlockOnResource, 4034, PATCH),
	REGISTER_OOVPA(D3DResource_BlockUntilNotBusy, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawTriPatch, 3911, DISABLED),
	REGISTER_OOVPA(D3DDevice_DrawRectPatch, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetScissors, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetBackBufferScale, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_PersistDisplay, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_GetProjectionViewportMatrix, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_KickOff, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_FlushVertexCache, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_InsertCallback, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_LoadVertexShaderProgram, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetModelView, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetShaderConstantMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_IsFencePending, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_GetOverlayUpdateStatus, 3911, PATCH),
	REGISTER_OOVPA(D3D_LazySetPointParams, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetModelView, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVolumeTexture, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDisplayFieldStatus, 4039, PATCH),
};

// ******************************************************************
// * D3D8_4361_SIZE
// ******************************************************************
uint32 D3D8_4361_SIZE = sizeof(D3D8_4361);
