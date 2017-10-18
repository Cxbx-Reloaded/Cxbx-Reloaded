// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->D3D8.1.0.5344.cpp
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

#if 0 // Moved to 5028
// ******************************************************************
// * Direct3D_CreateDevice
// ******************************************************************
OOVPA_NO_XREF(Direct3D_CreateDevice, 5344, 20) // ONLY for 5344, 5558 and later use generic 3911 version
		{ 0x00, 0x55 },
		{ 0x01, 0x8B },
		{ 0x02, 0xEC },
		{ 0x03, 0xA1 },

		{ 0x08, 0x85 },
		{ 0x09, 0xC0 },
		{ 0x0A, 0x75 },
		{ 0x0B, 0x0A },
		{ 0x0C, 0xC7 },
		{ 0x0D, 0x05 },

		{ 0x12, 0x00 },
		{ 0x13, 0x00 },
		{ 0x14, 0x08 },
		{ 0x15, 0x00 },
		{ 0x16, 0xA1 },

		{ 0x1B, 0x85 },
		{ 0x1C, 0xC0 },
		{ 0x1D, 0x75 },
		{ 0x1E, 0x0A },
		{ 0x1F, 0xC7 },
OOVPA_END;
#endif
#if 0 // Moved to 5028
// ******************************************************************
// * D3D_MakeRequestedSpace
// ******************************************************************
OOVPA_XREF(D3D_MakeRequestedSpace, 5344, 28,

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
		{ 0x15, 0x4C },
		{ 0x16, 0x03 },
		{ 0x17, 0x00 },
		{ 0x18, 0x00 },
		{ 0x19, 0x8B },
		{ 0x1A, 0x78 },
		{ 0x1B, 0x04 },
		{ 0x1C, 0x8B },
		{ 0x1D, 0x96 },
		{ 0x1E, 0x50 },
		{ 0x1F, 0x03 },
OOVPA_END;
#endif
// ******************************************************************
// * D3DDevice_SetRenderState_Dxt1NoiseEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_Dxt1NoiseEnable, 5344, 10)

        // D3DDevice_SetRenderState_Dxt1NoiseEnable+0D : mov eax, [edi+XXXXh]
        { 0x0D, 0x8B },
        { 0x0E, 0x87 },

        // D3DDevice_SetRenderState_Dxt1NoiseEnable+1D : and dl, 3Ch
        { 0x1D, 0x80 },
        { 0x1E, 0xE2 },
        { 0x1F, 0x3C },

        // D3DDevice_SetRenderState_Dxt1NoiseEnable+20 : xor esi, esi
        { 0x20, 0x33 },
        { 0x21, 0xF6 },

        // D3DDevice_SetRenderState_Dxt1NoiseEnable+22 : cmp dl, 20h
        { 0x22, 0x80 },
        { 0x23, 0xFA },
        { 0x24, 0x20 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetLight
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetLight, 5344, 15)

        // D3DDevice_SetLight+0x1f : add REG, 0x10
        { 0x1F, 0x83 },
        { 0x21, 0x10 },

        // D3DDevice_SetLight+0x22 : and REG, 0xfffffff0
        { 0x22, 0x83 },
        { 0x24, 0xf0 },

        // D3DDevice_SetLight+0x2c : push 0x24800000
        { 0x2C, 0x68 },
        { 0x2D, 0x00 },
        { 0x2E, 0x00 },
        { 0x2F, 0x80 },
        { 0x30, 0x24 },

        // D3DDevice_SetLight+0x65 : jz +8
        { 0x65, 0x74 },
        { 0x66, 0x08 },

        // D3DDevice_SetLight+0xe3 : jz +8
        { 0xE3, 0x74 },
        { 0xE4, 0x08 },

        // D3DDevice_SetLight+0xfe : shr ???, 2
        { 0xFE, 0xC1 },
        { 0x100, 0x02 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 5344 version
// ******************************************************************
// * D3DDevice_SetLight
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetLight, 5344, 8)

        { 0x1E, 0x00 },
        { 0x3E, 0x89 },
        { 0x5E, 0x00 },
        { 0x7E, 0x03 },
        { 0x9E, 0xC8 },
        { 0xBE, 0xC9 },
        { 0xDE, 0x03 },
        { 0xFE, 0xC1 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleRenderTargetMode
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, 5344, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0B, XREF_D3DRS_MULTISAMPLERENDERTARGETMODE ),

        { 0x03, 0x04 },
        { 0x0F, 0x8B },
        { 0x15, 0x3B },
        { 0x1C, 0x07 }, // **
        { 0x1D, 0x33 }, // **
        { 0x1F, 0xE8 }, // **
        { 0x25, 0x04 }, // **
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_TwoSidedLighting
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_TwoSidedLighting, 5344, 13)

        // D3DDevice_SetRenderState_TwoSidedLighting+0x07 : mov eax, [esi]
        { 0x07, 0x8B },
        { 0x08, 0x06 },

        // D3DDevice_SetRenderState_TwoSidedLighting+0x09 : cmp eax, [esi+4]
        { 0x09, 0x3B },
        { 0x0A, 0x46 },
        { 0x0B, 0x04 },

        // ***
        // D3DDevice_SetRenderState_TwoSidedLighting+0x18 : mov dword ptr [eax], 417C4h
        { 0x18, 0xC7 },
        { 0x19, 0x00 },
        { 0x1A, 0xC4 },
        { 0x1B, 0x17 },
        { 0x1C, 0x04 },
        { 0x1D, 0x00 },

        // D3DDevice_SetRenderState_TwoSidedLighting+0x7D : retn 0x04
        { 0x7D, 0xC2 },
        { 0x7E, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetScreenSpaceOffset
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetScreenSpaceOffset, 5344, 7)

        { 0x08, 0x44 },
        { 0x12, 0xCE },
        { 0x1F, 0xD8 },
        { 0x26, 0x0A },
        { 0x30, 0x3B },
        { 0x3A, 0x8B },
        { 0x44, 0x06 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetViewport
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetViewport, 5344, 7)

        { 0x05, 0x56 },
        { 0x06, 0x57 },
        { 0x0A, 0x0C },
        { 0x0E, 0x0A },
        { 0x12, 0x06 },
        { 0x16, 0xF3 },
        { 0x1A, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetBackBuffer2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetBackBuffer2, 5344, 7)

        { 0x07, 0x8B },
        { 0x10, 0x01 },
        { 0x19, 0x15 },
        { 0x22, 0x8B },
        { 0x2B, 0xC0 },
        { 0x34, 0x15 },
        { 0x3D, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_LightEnable
// ******************************************************************
OOVPA_XREF(D3DDevice_LightEnable, 5344, 1+24,

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

	// { 0x11, 0x90 }, // A4 // A8
	// { 0x12, 0x03 }, // 07
	{ 0x13, 0x00 },

	{ 0x14, 0x00 },
	{ 0x15, 0x57 },
	{ 0x16, 0x73 },
	{ 0x17, 0x13 },
	{ 0x18, 0x8B },

	{ 0x19, 0x8E },
	// { 0x1A, 0x8C }, // A0 // A4
	// { 0x1B, 0x03 }, // 07
	{ 0x1C, 0x00 },
	{ 0x1D, 0x00 },
	{ 0x1E, 0x8D },
	{ 0x1F, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetLightEnable
// ******************************************************************
OOVPA_XREF(D3DDevice_GetLightEnable, 5344, 1+26,

	XRefNoSaveIndex,
	XRefOne)

        XREF_ENTRY( 0x06, XREF_D3DDEVICE),

        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        { 0x0A, 0x8B },
        { 0x0B, 0x91 },
        //{ 0x0C, 0x8C }, 5344 0x8C03 vs 5558 0xA007
        //{ 0x0D, 0x03 },
        { 0x0E, 0x00 },
        { 0x0F, 0x00 },
        { 0x10, 0x8D },
        { 0x11, 0x04 },
        { 0x12, 0xC0 },
        { 0x13, 0xC1 },
        { 0x14, 0xE0 },
        { 0x15, 0x04 },
        { 0x16, 0x56 },
        { 0x17, 0x8B },
        { 0x18, 0xB4 },
        { 0x19, 0x02 },
        { 0x1A, 0x8C },
        { 0x1B, 0x00 },
        { 0x1C, 0x00 },
        { 0x1D, 0x00 },
        { 0x1E, 0x8B },
        { 0x1F, 0x54 },
/*
        { 0x3E, 0xC7 },
        { 0x5E, 0xC7 },
        { 0x7E, 0x00 },
        { 0x9E, 0x85 },
        { 0xBE, 0x01 },
        { 0xDE, 0x00 },
        { 0xFE, 0x83 },
*/
OOVPA_END;

#if 0 // No longer used, replaced by generic 4134 version
// ******************************************************************
// * D3DDevice_SetSoftDisplayFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetSoftDisplayFilter, 5344, 12)

        // D3DDevice_SetSoftDisplayFilter+0x25 : mov edx, [eax+0x17F8]
        { 0x25, 0x8B },
        { 0x26, 0x90 },
        { 0x27, 0xF8 },
        { 0x28, 0x17 },

        // D3DDevice_SetSoftDisplayFilter+0x2B : push 0; push esi; push 0x0E; push edx
        { 0x2B, 0x6A },
        { 0x2C, 0x00 },
        { 0x2D, 0x56 },
        { 0x2E, 0x6A },
        { 0x2F, 0x0E },
        { 0x30, 0x52 },

        // D3DDevice_SetSoftDisplayFilter+0x4E : retn 0x04
        { 0x4E, 0xC2 },
        { 0x4F, 0x04 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_SetMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetMaterial, 5344, 11)

        { 0x04, 0x08 },

        { 0x0C, 0x81 },
        { 0x0D, 0xC7 },
        { 0x0E, 0xE0 },
        { 0x0F, 0x0A },
        { 0x10, 0x00 },

        { 0x16, 0x00 },
        { 0x1F, 0x81 },
        { 0x22, 0x90 },
        { 0x2C, 0x5E },
        { 0x2E, 0x04 },
OOVPA_END;

#if 0 // No longer used, replaced by generic another 5344 version
// ******************************************************************
// * D3DDevice_SetMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetMaterial, 5344, 9)

        { 0x04, 0x08 },
        { 0x0C, 0x81 },
        { 0x0E, 0xE0 },
        { 0x10, 0x00 },
        { 0x16, 0x00 },
        { 0x1F, 0x81 },
        { 0x22, 0x90 },
        { 0x2C, 0x5E },
        { 0x2E, 0x04 },
OOVPA_END;
#endif
// ******************************************************************
// * Get2DSurfaceDesc
// ******************************************************************
OOVPA_NO_XREF(Get2DSurfaceDesc, 5344, 10)

        // Get2DSurfaceDesc+0x2B : movzx edx, byte ptr [edi+0x0D]
        { 0x2B, 0x0F },
        { 0x2C, 0xB6 },
        { 0x2D, 0x57 },
        { 0x2E, 0x0D },

        // Get2DSurfaceDesc+0x52 : mov edx, [eax+0x15F4]
        { 0x52, 0x8B },
        { 0x53, 0x90 },
        { 0x54, 0xF4 },
        { 0x55, 0x15 },

        // Get2DSurfaceDesc+0xAE : retn 0x0C
        { 0xAE, 0xC2 },
        { 0xAF, 0x0C },
OOVPA_END;

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3DDevice_SetFlickerFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetFlickerFilter, 5344, 12)

        // D3DDevice_SetFlickerFilter+0x1C : mov eax, [eax+0x17C8]
        { 0x1C, 0x8B },
        { 0x1D, 0x80 },
        { 0x1E, 0xF8 },
        { 0x1F, 0x17 },

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
// * D3DDevice_SetViewport
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetViewport, 5344, 8)

        { 0x1E, 0x87 },
        { 0x3E, 0xC0 },
        { 0x5E, 0x49 },
        { 0x7E, 0xD6 },
        { 0x9E, 0xE2 },
        { 0xBE, 0xC1 },
        { 0xDE, 0xC9 },
        { 0xFE, 0x14 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTransform
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTransform, 5344, 15)

        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },
        { 0x04, 0x8B },
        { 0x05, 0x54 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },
        { 0x08, 0x53 },
        { 0x09, 0x8B },
        { 0x0A, 0x1D },

        { 0x0F, 0x56 },
        { 0x1F, 0xF2 },

        { 0x2F, 0x0B },
        { 0x35, 0x35 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 5344 version
// ******************************************************************
// * D3DDevice_SetTransform
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTransform, 5344, 9)

        { 0x01, 0x44 },
        { 0x1E, 0x8B },
        { 0x3E, 0x00 },
        { 0x5E, 0xC4 },
        { 0x7E, 0x62 },
        { 0x9E, 0x00 },
        { 0xBE, 0x42 },
        { 0xDE, 0x42 },
        { 0xFE, 0xCB },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_SetScissors
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetScissors, 5344, 8)

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
// * D3DDevice_SetVerticalBlankCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVerticalBlankCallback, 5344, 7)

        { 0x01, 0x44 },
        { 0x04, 0x8B },
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0x88 },
        { 0x0D, 0x19 },
        { 0x10, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetSwapCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetSwapCallback, 5344, 7)

        { 0x01, 0x44 },
        { 0x04, 0x8B },
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0x84 },
        { 0x0D, 0x19 },
        { 0x10, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderTargetFast
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderTargetFast, 5344, 8)

        { 0x1D, 0xC9 },
        { 0x3C, 0x89 },
        { 0x5F, 0x8B },
        { 0x7A, 0x04 },
        { 0x99, 0x04 },
        { 0xB8, 0x02 },
        { 0xD7, 0x0C },
        { 0xF6, 0xC5 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SelectVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SelectVertexShader, 5344, 8)

        { 0x0E, 0x49 },
        { 0x10, 0x8D },
        { 0x14, 0x86 },
        { 0x18, 0x00 },
        { 0x1C, 0x03 },
        { 0x36, 0x4F },
        { 0x38, 0x83 },
        { 0x3C, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderTarget
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderTarget, 5344, 9)

        { 0x00, 0x83 },
        { 0x08, 0x56 },
        { 0x09, 0x33 },
        { 0x0A, 0xF6 },
        { 0x0B, 0x3B },
        { 0x0C, 0xEE },
        { 0x2A, 0xE4 },
        { 0x30, 0x74 },
        { 0x34, 0x14 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPalette
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPalette, 5344, 12)

        // D3DDevice_SetPalette+0x0D : mov eax, [esi+edi*4+0B78h]
        { 0x0D, 0x8B },
        { 0x0E, 0x84 },
        { 0x0F, 0xBE },
        { 0x10, 0x78 },
        { 0x11, 0x0B },

        // D3DDevice_SetPalette+0x51 : add ebp, 0x80000
        { 0x51, 0x81 },
        { 0x52, 0xC5 },
        { 0x53, 0x00 },
        { 0x54, 0x00 },
        { 0x55, 0x08 },

        // D3DDevice_SetPalette+0x8A : retn 0x08
        { 0x8A, 0xC2 },
        { 0x8B, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_IsBusy
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_IsBusy, 5344, 12)

        { 0x05, 0x8B },
        { 0x09, 0x00 },
        { 0x0A, 0x00 },
        { 0x0B, 0x8B },
        { 0x0C, 0x88 },
        { 0x0D, 0x40 },
        { 0x0E, 0x32 },
        { 0x0F, 0x00 },
        { 0x10, 0x00 },
        { 0x14, 0x32 },
        { 0x18, 0x06 },
        { 0x1C, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateVertexBuffer2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateVertexBuffer2, 5344, 9)

        { 0x08, 0xE8 },
        { 0x0E, 0xF0 },
        { 0x10, 0xF6 },
        { 0x14, 0x44 },
        { 0x18, 0x04 },
        { 0x1C, 0x6A },
        { 0x2C, 0x85 },
        { 0x30, 0x68 },
        { 0x34, 0x24 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreatePalette2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreatePalette2, 5344, 8)

        { 0x08, 0xE8 },
        { 0x10, 0xFF },
        { 0x11, 0x75 },
        { 0x12, 0x04 },
        { 0x13, 0x5F },
        { 0x14, 0xC2 },
        { 0x38, 0x85 },
        { 0x3C, 0x68 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawVerticesUP
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawVerticesUP, 5344, 16)

        { 0x03, 0x83 },
        { 0x04, 0xEC },
        { 0x05, 0x14 },

        { 0x25, 0x8B },
        { 0x26, 0x4D },
        { 0x27, 0x08 },
        { 0x28, 0x8B },
        { 0x29, 0x5D },
        { 0x2A, 0x10 },
        { 0x2B, 0x8B },
        { 0x2C, 0x75 },
        { 0x2D, 0x14 },
        { 0x2E, 0xC7 },
        { 0x2F, 0x00 },

        { 0x50, 0xC3 },
        { 0x51, 0x8B },
OOVPA_END;

#if 0 // No longer used, replaced by generic 5344 version
// ******************************************************************
// * D3DDevice_DrawVerticesUP
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawVerticesUP, 5344, 10)

        // D3DDevice_DrawVerticesUP+0x03 : sub esp, 0x14
        { 0x03, 0x83 },
        { 0x04, 0xEC },
        { 0x05, 0x14 },

        // D3DDevice_DrawVerticesUP+0x37 : mov edx, [edi+834h]
        { 0x37, 0x8B },
        { 0x38, 0x97 },
        { 0x39, 0x34 },
        { 0x3A, 0x08 },

        // D3DDevice_DrawVerticesUP+0xC4 : cmp esi, 0x80
        { 0xC4, 0x81 },
        { 0xC5, 0xFE },
        { 0xC6, 0x80 },
OOVPA_END;
#endif
#if 0 // Used 4361
// ******************************************************************
// * D3DDevice_SetVertexDataColor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexDataColor, 5344, 8)

        { 0x08, 0x06 },
        { 0x0C, 0x57 },
        { 0x0F, 0xE8 },
        { 0x15, 0x4C },
        { 0x19, 0x14 },
        { 0x1A, 0x8D },
        { 0x1B, 0x40 },
        { 0x1C, 0x19 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_End
// ******************************************************************
//Generic OOVPA as of 5344 and newer.
OOVPA_NO_XREF(D3DDevice_End, 5344, 14)

        { 0x08, 0x06 },
        { 0x0A, 0x46 },
        { 0x0E, 0xE8 },
        { 0x15, 0xFC },
        { 0x17, 0x04 },

        { 0x34, 0x4E },
        { 0x35, 0x08 },
        { 0x36, 0x5E },
        { 0x37, 0x74 },
        { 0x38, 0x07 },
        { 0x39, 0x6A },
        { 0x3A, 0x01 },
        { 0x3B, 0xE8 },

        { 0x40, 0xC3 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 5344 version
// ******************************************************************
// * D3DDevice_End
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_End, 5344, 9)

        { 0x08, 0x06 },
        { 0x0A, 0x46 },
        { 0x0E, 0xE8 },
        { 0x15, 0xFC },
        { 0x17, 0x04 },
        { 0x19, 0xC7 },
        { 0x1A, 0x40 },
        { 0x1B, 0x04 },
        { 0x40, 0xC3 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_CreatePixelShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreatePixelShader, 5344, 11)

        // D3DDevice_CreatePixelShader+0x05 : push 0xFC
        { 0x05, 0x68 },
        { 0x06, 0xFC },

        // D3DDevice_CreatePixelShader+0x10 : mov eax, 0x8007000E
        { 0x13, 0xB8 },
        { 0x14, 0x0E },
        { 0x15, 0x00 },
        { 0x16, 0x07 },
        { 0x17, 0x80 },

        // D3DDevice_CreatePixelShader+0x31 : mov ecx, 0x3C
        { 0x34, 0xB9 },
        { 0x35, 0x3C },

        // D3DDevice_CreatePixelShader+0x42 : retn 0x08
        { 0x45, 0xC2 },
        { 0x46, 0x08 },
OOVPA_END;
#if 0 // Moved to 5233
// ******************************************************************
// * D3DDevice_SetPixelShaderConstant
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPixelShaderConstant, 5344, 9)

        { 0x1A, 0x0C },
        { 0x1B, 0x8B },
        { 0x1C, 0x45 },
        { 0x1D, 0x10 },
        { 0x1E, 0x85 },
        { 0x5B, 0xF3 },
        { 0x60, 0xE0 },
        { 0x64, 0xC0 },
        { 0x68, 0x2D },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3DDevice_GetOverlayUpdateStatus
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetOverlayUpdateStatus, 5344, 12)

        // D3DDevice_GetOverlayUpdateStatus+0x05 : mov ecx, [eax+1FE0h]
        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0xE0 },
        { 0x08, 0x1F },

        // D3DDevice_GetOverlayUpdateStatus+0x0C : mov esi, [eax+19B8h]
        { 0x0C, 0x8B },
        { 0x0D, 0xB0 },
        { 0x0E, 0xB8 },
        { 0x0F, 0x19 },

        // D3DDevice_GetOverlayUpdateStatus+0x16 : setnz dl
        { 0x16, 0x0F },
        { 0x17, 0x95 },
        { 0x18, 0xC2 },

        // D3DDevice_GetOverlayUpdateStatus+0x1C : retn
        { 0x1C, 0xC3 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 5233 version
// ******************************************************************
// * D3DDevice_UpdateOverlay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_UpdateOverlay, 5344, 10)

        { 0x0A, 0xB8 },
        { 0x13, 0x89 },
        { 0x14, 0x88 },
        { 0x15, 0xF0 },
        { 0x16, 0x17 },
        { 0x74, 0x4F },
        { 0x79, 0x44 },
        { 0x7D, 0x40 },
        { 0x80, 0xE1 },
        { 0x84, 0xCA },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * IDirectD3DDevice8_Release
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Release, 5344, 8)

        { 0x07, 0x8B },
        { 0x0C, 0x00 },
        { 0x13, 0xCF },
        { 0x1A, 0x1C },
        { 0x21, 0x05 },
        { 0x28, 0x00 },
        { 0x2F, 0x89 },
        { 0x36, 0xC3 },
OOVPA_END;
#endif
#if 0 // Moved to 5233
// ******************************************************************
// * D3DDevice_GetVisibilityTestResult
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVisibilityTestResult, 5344, 7)

        { 0x0E, 0x08 },
        { 0x10, 0xFF },
        { 0x2A, 0x16 },
        { 0x3C, 0x76 },
        { 0x3F, 0x0C },
        { 0x51, 0x0A },
        { 0x5F, 0x0C },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4831 version
// ******************************************************************
// * D3DDevice_PersistDisplay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_PersistDisplay, 5344, 7)

        { 0x1E, 0x15 },
        { 0x3A, 0xAB },
        { 0x3B, 0xF8 },
        { 0x3C, 0x17 },
        { 0x3D, 0x00 },
        { 0x4E, 0xF5 },
        { 0x6E, 0x01 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_DeleteVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DeleteVertexShader, 5344, 6)

        { 0x02, 0x24 },
        { 0x06, 0xFF },
        { 0x0A, 0x08 },
        { 0x0E, 0x00 },

        // D3DDevice_DeleteVertexShader+0x18 : retn 4
        { 0x18, 0xC2 },
        { 0x19, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawIndexedVerticesUP
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawIndexedVerticesUP, 5344, 9)

        { 0x05, 0x14 },
        { 0x06, 0x53 },
        { 0x07, 0x56 },
        { 0x08, 0x8B },
        { 0x09, 0x35 },
        { 0x2A, 0xFC },
        { 0x3A, 0x89 },
        { 0x4A, 0x81 },
        { 0x5A, 0x56 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DeletePixelShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DeletePixelShader, 5344, 7)

        { 0x02, 0x24 },
        { 0x06, 0x75 },
        { 0x0A, 0x04 },
        { 0x0E, 0x0B },
        { 0x12, 0x80 },
        { 0x1A, 0xC2 },
        { 0x1B, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetMaterial, 5344, 8)

        { 0x05, 0x56 },
        { 0x06, 0x57 },
        { 0x0A, 0x0C },
        { 0x0D, 0xE0 },
        { 0x0E, 0x0A },
        { 0x12, 0x11 },
        { 0x16, 0xF3 },
        { 0x1A, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DPalette_Lock
// ******************************************************************
OOVPA_NO_XREF(D3DPalette_Lock, 5344, 8)

        { 0x04, 0x8B },
        { 0x0A, 0xE8 },
        { 0x13, 0x89 },
        { 0x14, 0x02 },
        { 0x15, 0x33 },
        { 0x16, 0xC0 },
        { 0x17, 0xC2 },
        { 0x18, 0x0C },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetBackMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetBackMaterial, 5344, 8)

        { 0x05, 0x56 },
        { 0x06, 0x57 },
        { 0x0A, 0x0C },
        { 0x0D, 0x24 },
        { 0x0E, 0x0B },
        { 0x12, 0x11 },
        { 0x16, 0xF3 },
        { 0x1A, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetBackMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetBackMaterial, 5344, 11)

        { 0x04, 0x08 },

        { 0x0C, 0x81 },
        { 0x0D, 0xC7 },
        { 0x0E, 0x24 },
        { 0x0F, 0x0B },
        { 0x10, 0x00 },

        { 0x16, 0x00 },
        { 0x1F, 0x81 },
        { 0x22, 0x90 },
        { 0x2C, 0x5E },
        { 0x2E, 0x04 },
OOVPA_END;

#if 0 // No longer used, replaced by another 5344 version
// ******************************************************************
// * D3DDevice_SetBackMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetBackMaterial, 5344, 8)

        { 0x04, 0x08 },
        { 0x0C, 0x81 },
        { 0x0E, 0x24 },
        { 0x10, 0x00 },
        { 0x16, 0x00 },
        { 0x1F, 0x81 },
        { 0x2C, 0x5E },
        { 0x2E, 0x04 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_GetProjectionViewportMatrix
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetProjectionViewportMatrix, 5344, 16) // Up to 5455

        { 0x02, 0x35 },

        { 0x07, 0x57 },

        { 0x0E, 0x60 }, // 4361 0x7004
        { 0x0F, 0x05 },
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

#if 0 // No longer used, replaced by generic 5344 version
// ******************************************************************
// * D3DDevice_GetProjectionViewportMatrix
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetProjectionViewportMatrix, 5344, 7)

        { 0x02, 0x35 },
        { 0x07, 0x57 },
        { 0x0A, 0x24 },
        { 0x0E, 0x60 },
        { 0x12, 0xB9 },
        { 0x16, 0x00 },
        { 0x1A, 0x5E },
OOVPA_END;
#endif
// ******************************************************************
// * D3DDevice_GetTile
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetTile, 5344, 10)

        { 0x05, 0x0D },
        { 0x0A, 0x56 },
        { 0x0B, 0x57 },
        { 0x0C, 0x8B },
        { 0x0D, 0x7C },
        { 0x0E, 0x24 },
        { 0x0F, 0x10 },
        { 0x10, 0x8D },
        { 0x11, 0x04 },
        { 0x1F, 0xF3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateIndexBuffer2
// ******************************************************************
OOVPA_XREF(D3DDevice_CreateIndexBuffer2, 5344, 7,

    XREF_D3DDevice_CreateIndexBuffer2,
    XRefZero)

        { 0x05, 0x00 },
        { 0x0C, 0x50 },
        { 0x13, 0xC0 },
        { 0x1A, 0xC9 },
        { 0x21, 0x04 },
        { 0x28, 0xC7 },
        { 0x2F, 0x48 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTexture
// ******************************************************************
#if 0 // Derived XREF_OFFSET_D3DDEVICE_M_TEXTURES is not Implemented yet.
OOVPA_XREF(D3DDevice_SetTexture, 5344, 1+26, // Also for 5558, 5659, 5788, 5849, 5933

    XRefNoSaveIndex,
    XRefOne)

		XREF_ENTRY( 0x13, XREF_OFFSET_D3DDEVICE_M_TEXTURES ), // derived
#endif
OOVPA_NO_XREF(D3DDevice_SetTexture, 5344, 26)

		{ 0x00, 0x83 },
		{ 0x01, 0xEC },
		{ 0x02, 0x08 },
		{ 0x03, 0x53 },
		{ 0x04, 0x55 },
		{ 0x05, 0x8B },
		{ 0x06, 0x2D },

		{ 0x0B, 0x56 },
		{ 0x0C, 0x8B },
		{ 0x0D, 0x74 },
		{ 0x0E, 0x24 },
		{ 0x0F, 0x18 },
		{ 0x10, 0x8B },
		{ 0x11, 0x84 },
		{ 0x12, 0xB5 },

		{ 0x15, 0x00 },
		{ 0x16, 0x00 },
		{ 0x17, 0x85 },
		{ 0x18, 0xC0 },
		{ 0x19, 0x89 },
		{ 0x1A, 0x6C },
		{ 0x1B, 0x24 },
		{ 0x1C, 0x0C },
		{ 0x1D, 0x89 },
		{ 0x1E, 0x44 },
		{ 0x1F, 0x24 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BlockUntilVerticalBlank
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BlockUntilVerticalBlank, 5344, 11)

        // D3DDevice_BlockUntilVerticalBlank+0x05 : push 0; push 0; push 1
        { 0x05, 0x6A },
        { 0x06, 0x00 },
        { 0x07, 0x6A },
        { 0x08, 0x00 },
        { 0x09, 0x6A },
        { 0x0A, 0x01 },

        // D3DDevice_BlockUntilVerticalBlank+0x17 : add eax, 0x198C
        { 0x17, 0x05 },
        { 0x18, 0x8C },
        { 0x19, 0x19 },

        // D3DDevice_BlockUntilVerticalBlank+0x1D : call [KrnlImport]
        { 0x1D, 0xFF },

        // D3DDevice_BlockUntilVerticalBlank+0x23 : retn
        { 0x23, 0xC3 },
OOVPA_END;

#if 0 // No longer used, replaced by another 5344 version
// ******************************************************************
// * D3DDevice_BlockUntilVerticalBlank
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BlockUntilVerticalBlank, 5344, 13)

        { 0x00, 0xA1 },
        { 0x05, 0x6A },
        { 0x0F, 0x00 },
        { 0x10, 0x00 },
        { 0x11, 0x00 },
        { 0x12, 0x00 },
        { 0x13, 0x00 },
        { 0x14, 0x00 },
        { 0x15, 0x6A },
        { 0x16, 0x06 },
        { 0x17, 0x05 },
        { 0x1E, 0x15 },
        { 0x23, 0xC3 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_BeginPush
// ******************************************************************
OOVPA_XREF(D3DDevice_BeginPush, 5344, 10,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x09, XREF_D3DDevice_LazySetStateVB ),

        { 0x00, 0x8B },
        { 0x01, 0x0D },

        { 0x06, 0x6A },
        { 0x07, 0x00 },

        { 0x11, 0x40 },

        { 0x12, 0x89 },
        { 0x13, 0x44 },
        { 0x14, 0x24 },
        { 0x15, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_AddRef
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_AddRef, 5344, 10)

        // D3DDevice_AddRef+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3DDevice_AddRef+0x05 : mov ecx, [eax+0x0520]
        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x20 },
        { 0x08, 0x05 },

        // D3DDevice_AddRef+0x0B : inc ecx
        { 0x0B, 0x41 },

        // D3DDevice_AddRef+0x0C : mov [eax+0x0520], ecx
        { 0x0C, 0x89 },
        { 0x0D, 0x88 },
        { 0x0E, 0x20 },
        { 0x0F, 0x05 },
OOVPA_END;

#if 0 // No longer used, replaced by another 4134 version
// ******************************************************************
// * D3DDevice_AddRef
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_AddRef, 5344, 10)

        { 0x00, 0xA1 },

        { 0x05, 0x8B },
        { 0x09, 0x00 },
        { 0x0D, 0x88 },

        { 0x0F, 0x05 },
        { 0x10, 0x00 },
        { 0x11, 0x00 },
        { 0x12, 0x8B },
        { 0x13, 0xC1 },
        { 0x14, 0xC3 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_GetShaderConstantMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetShaderConstantMode, 5344, 16)

        { 0x00, 0xA1 },

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x08 },
        { 0x08, 0x15 },
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
// * Rollback support signature(s)
// ******************************************************************
#define D3DDevice_GetTransform_5344 D3DDevice_GetTransform_3925

// ******************************************************************
// * D3D8_5344
// ******************************************************************
OOVPATable D3D8_5344[] = {

	REGISTER_OOVPA(CMiniport_InitHardware, 4627, PATCH),
	REGISTER_OOVPA(CMiniport_CreateCtxDmaObject, 4034, PATCH),

	REGISTER_OOVPA(Direct3D_CreateDevice, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDisplayFieldStatus, 5233, PATCH),
	REGISTER_OOVPA(D3D_MakeRequestedSpace, 5028, PATCH),
	REGISTER_OOVPA(D3D_CheckDeviceFormat, 4134, DISABLED),
	REGISTER_OOVPA(D3D_KickOffAndWaitForIdle, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_CopyRects, 5120, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScissors, 5344, PATCH), // Was 5233
	REGISTER_OOVPA(D3DDevice_GetGammaRamp, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetGammaRamp, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDeviceCaps, 3911, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetShaderConstantMode, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_GetRenderTarget2, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDepthStencilSurface2, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderTarget, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_AddRef, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_Reset, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTile, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateIndexBuffer2, 5344, DISABLED),
	REGISTER_OOVPA(D3DDevice_CreateVertexShader, 3911, PATCH),
	REGISTER_OOVPA(D3D_RecordStateBlock, 3911, XREF),
	REGISTER_OOVPA(D3D_ClearStateBlockFlags, 3911, XREF),
	REGISTER_OOVPA(D3DDevice_BeginStateBlock, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_EndStateBlock, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_RunVertexStateShader, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderSize, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant1, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant1, 4627, ALIAS, D3DDevice_SetVertexShaderConstant1Fast),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant4, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstantNotInline, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstantNotInline, 4627, ALIAS, D3DDevice_SetVertexShaderConstantNotInlineFast),
	REGISTER_OOVPA(D3DDevice_DeletePixelShader, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_CreatePixelShader, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShader, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateTexture2, 5028, PATCH), // Was 4627
	REGISTER_OOVPA(D3DDevice_SetIndices, 5028, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetTexture, 5344, PATCH), // Was 4928
	REGISTER_OOVPA(D3DDevice_GetDisplayMode, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData2f, 4039, PATCH), // Was 4627
	REGISTER_OOVPA(D3DDevice_Begin, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginStateBig, 5028, XREF), 
	REGISTER_OOVPA(D3DDevice_End, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_Clear, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_Swap, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_EnableOverlay, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVertexBuffer2, 5344, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetTextureState_TexCoordIndex, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BumpEnv, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BorderColor, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_ColorKeyColor, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_VertexBlend, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_CullMode, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_NormalizeNormals, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TextureFactor, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZBias, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_EdgeAntiAlias, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FillMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_BackFillMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FogColor, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Dxt1NoiseEnable, 5344, PATCH), // Was 5233
	REGISTER_OOVPA(D3DDevice_SetRenderState_Simple, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZEnable, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilFail, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LineWidth, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_OcclusionCullEnable, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilCullEnable, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZCmpAlwaysRead, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZRead, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_DoNotCullUncompressed, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleAntiAlias, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMode, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, 5344, PATCH), // Was 5233
	REGISTER_OOVPA(D3DDevice_SetRenderState_ShadowFunc, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SwitchTexture, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_YuvEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTransform, 5344, PATCH), // Was 4134
	REGISTER_OOVPA(D3DDevice_SetStreamSource, 4034, PATCH),
	REGISTER_OOVPA(D3D_GetDeviceCaps, 3911, DISABLED),
	REGISTER_OOVPA(D3DDevice_GetLight, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetLight, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_LightEnable, 5344, PATCH), // Was 5233
	REGISTER_OOVPA(D3DDevice_DrawIndexedVertices, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetMaterial, 5344, PATCH), // Was 4627
	REGISTER_OOVPA(D3DVertexBuffer_GetDesc, 3911, DISABLED),
	REGISTER_OOVPA(D3DVertexBuffer_Lock2, 4627, PATCH),
	REGISTER_OOVPA(D3DResource_Register, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetBackBuffer2, 5344, PATCH), // 5233
	REGISTER_OOVPA(D3DDevice_UpdateOverlay, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_GetOverlayUpdateStatus, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_BlockUntilVerticalBlank, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_GetViewport, 5344, PATCH), // Was 4627
	REGISTER_OOVPA(D3DDevice_SetViewport, 5344, PATCH), // Was 5233
	REGISTER_OOVPA(D3DResource_AddRef, 3911, PATCH),
	REGISTER_OOVPA(D3DResource_Release, 3911, PATCH),
	REGISTER_OOVPA(D3DResource_IsBusy, 4039, PATCH),
	REGISTER_OOVPA(D3DPalette_Lock2, 4627, PATCH),
	REGISTER_OOVPA(D3DTexture_LockRect, 3911, PATCH),
	REGISTER_OOVPA(Lock2DSurface, 3911, PATCH),
	REGISTER_OOVPA(Get2DSurfaceDesc, 5344, PATCH), // Was 5233
	REGISTER_OOVPA(D3DSurface_GetDesc, 3911, PATCH),
	REGISTER_OOVPA(D3DSurface_LockRect, 3911, PATCH),
	REGISTER_OOVPA(D3DBaseTexture_GetLevelCount, 3911, PATCH),
	REGISTER_OOVPA(D3DTexture_GetSurfaceLevel2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShader, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawVertices, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_GetViewportOffsetAndScale, 4432, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteVertexShader, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_SelectVertexShaderDirect, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_LoadVertexShader, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_Release, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScreenSpaceOffset, 5344, PATCH), // Was 5233
	REGISTER_OOVPA(D3D_SetFence, 5028, XREF),
	REGISTER_OOVPA(D3DDevice_InsertFence, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_IsFencePending, 5028, PATCH),
	REGISTER_OOVPA(D3D_BlockOnTime, 5028, XREF),
	REGISTER_OOVPA(D3DDevice_BlockOnFence, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTransform, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPalette, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_GetCreationParameters, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetFlickerFilter, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetSoftDisplayFilter, 4134, PATCH),
	REGISTER_OOVPA(D3D_CMiniport_GetDisplayCapabilities, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TwoSidedLighting, 5344, PATCH), // 4134
	REGISTER_OOVPA(D3DDevice_SetRenderState_FrontFace, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LogicOp, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_LazySetStateVB, 5028, XREF),
	REGISTER_OOVPA(D3DDevice_BeginPush, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginVisibilityTest, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_EndPush, 4134, PATCH), // Was 4627
	REGISTER_OOVPA(D3DDevice_SetVerticalBlankCallback, 5344, PATCH), // Was 5233
	REGISTER_OOVPA(D3DDevice_SetRenderState_SampleAlpha, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMask, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4ub, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4f, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetSwapCallback, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderTargetFast, 5344, PATCH),
	REGISTER_OOVPA(D3DCubeTexture_GetCubeMapSurface2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_ApplyStateBlock, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_EndVisibilityTest, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_PSTextureModes, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SelectVertexShader, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_IsBusy, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_CreatePalette2, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawVerticesUP, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexDataColor, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShaderConstant, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVisibilityTestResult, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_PersistDisplay, 4831, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawIndexedVerticesUP, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShaderProgram, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetPixelShader, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData2s, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4s, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_PrimeVertexCache, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_InsertCallback, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_GetStreamSource2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShader, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderConstant, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderInputDirect, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderInput, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderType, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderDeclaration, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderFunction, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderInput, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_GetMaterial, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_GetBackMaterial, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_SetBackMaterial, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_GetProjectionViewportMatrix, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_GetModelView, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTile, 5344, PATCH),
	REGISTER_OOVPA(D3DPalette_Lock, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTexture2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_FlushVertexCache, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetModelView, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetStipple, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_LoadVertexShaderProgram, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetBackBufferScale, 4134, PATCH),
	REGISTER_OOVPA(D3D_BlockOnResource, 4034, XREF),
	REGISTER_OOVPA(D3DResource_BlockUntilNotBusy, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginPushBuffer, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_EndPushBuffer, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_RunPushBuffer, 5120, PATCH),
	REGISTER_OOVPA(D3DDevice_GetPushBufferOffset, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawTriPatch, 3911, DISABLED),
	REGISTER_OOVPA(D3DDevice_DrawRectPatch, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetScissors, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVolumeTexture, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateCubeTexture, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant, 4627, PATCH),
	REGISTER_OOVPA(D3DVolumeTexture_LockBox, 3911, PATCH),// Just calls Lock3DSurface
	REGISTER_OOVPA(D3DCubeTexture_LockRect, 3911, PATCH),
	REGISTER_OOVPA(Lock3DSurface, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_CaptureStateBlock, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteStateBlock, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateStateBlock, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_KickOff, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateImageSurface, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetShaderConstantMode, 5344, PATCH),
	REGISTER_OOVPA(D3DDevice_SetDepthClipPlanes, 4627, PATCH),
	REGISTER_OOVPA(D3D_LazySetPointParams, 4039, PATCH),

	// ******************************************************************
	// Unused functions in XDK 5344
	// TODO: Remove
	// ******************************************************************
	REGISTER_OOVPA(D3D_CommonSetRenderTarget, 5028, XREF), // Used between 4627 to 5233
	// ******************************************************************
};

// ******************************************************************
// * D3D8_5344_SIZE
// ******************************************************************
uint32 D3D8_5344_SIZE = sizeof(D3D8_5344);
