// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->D3D8.1.0.4432.cpp
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

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * Direct3D_CreateDevice
// ******************************************************************
OOVPA_NO_XREF(Direct3D_CreateDevice, 4432, 8)

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
// ******************************************************************
// * D3DDevice_GetRenderTarget
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetRenderTarget, 4432, 11)

        // D3DDevice_GetRenderTarget+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3DDevice_GetRenderTarget+0x05 : mov eax, [eax + 0x2070]
        { 0x05, 0x8B },
        { 0x06, 0x80 },
        { 0x07, 0x70 },
        { 0x08, 0x20 },

        // D3DDevice_GetRenderTarget+0x15 : push eax
        { 0x15, 0x50 },

        // D3DDevice_GetRenderTarget+0x13 : jz + 0x06
        { 0x13, 0x74 },
        { 0x14, 0x06 },

        // D3DDevice_GetRenderTarget+0x16 : call [addr]
        { 0x16, 0xE8 },

        // D3DDevice_GetRenderTarget+0x1D : retn 0x04
        { 0x1D, 0xC2 },
        { 0x1E, 0x04 },
OOVPA_END;
#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3DDevice_GetDepthStencilSurface
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetDepthStencilSurface, 4432, 11)

        // D3DDevice_GetDepthStencilSurface+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3DDevice_GetDepthStencilSurface+0x05 : mov eax, [eax + 0x2074]
        { 0x05, 0x8B },
        { 0x06, 0x80 },
        { 0x07, 0x74 },
        { 0x08, 0x20 },

        // D3DDevice_GetDepthStencilSurface+0x15 : push eax
        { 0x15, 0x50 },

        // D3DDevice_GetDepthStencilSurface+0x13 : jnz +0x0B
        { 0x13, 0x74 },
        { 0x14, 0x0B },

        // D3DDevice_GetDepthStencilSurface+0x16 : call [addr]
        { 0x16, 0xE8 },

        // D3DDevice_GetDepthStencilSurface+0x25 : retn 0x04
        { 0x25, 0xC2 },
        { 0x26, 0x04 },
OOVPA_END;
#endif
// ******************************************************************
// * D3DDevice_SetPixelShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPixelShader, 4432, 10)

        // D3DDevice_SetPixelShader+0x22 : or ecx, 0x4800
        { 0x22, 0x81 },
        { 0x23, 0xC9 },
        { 0x24, 0x00 },
        { 0x25, 0x48 },

        // D3DDevice_SetPixelShader+0x3A : or eax, 0x2000
        { 0x3A, 0x0D },
        { 0x3C, 0x20 },

        // D3DDevice_SetPixelShader+0x78 : retn 0x04
        { 0x78, 0xC2 },
        { 0x79, 0x04 },

        // D3DDevice_SetPixelShader+0xE3 : rep movsd
        { 0xE3, 0xF3 },
        { 0xE4, 0xA5 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_ZEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_ZEnable, 4432, 13)

        // D3DDevice_SetRenderState_ZEnable+0x0C : jb +0x05
        { 0x0C, 0x72 },
        { 0x0D, 0x05 },

        // D3DDevice_SetRenderState_ZEnable+0x1B : mov ecx, [esi+0x2074]
        { 0x1B, 0x8B },
        { 0x1C, 0x8E },
        { 0x1D, 0x74 },
        { 0x1E, 0x20 },

        // D3DDevice_SetRenderState_ZEnable+0x2E : mov dword ptr [eax], 0x4030C
        { 0x2E, 0xC7 },
        { 0x2F, 0x00 },
        { 0x30, 0x0C },
        { 0x31, 0x03 },
        { 0x32, 0x04 },

        // D3DDevice_SetRenderState_ZEnable+0x8C : retn 0x04
        { 0x8C, 0xC2 },
        { 0x8D, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleAntiAlias
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleAntiAlias, 4432, 9)

        // D3DDevice_SetRenderState_MultiSampleAntiAlias+0x04 : push esi
        { 0x04, 0x56 },

        // D3DDevice_SetRenderState_MultiSampleAntiAlias+0x12 : call [abs]
        { 0x12, 0xE8 },

        // D3DDevice_SetRenderState_MultiSampleAntiAlias+0x38 : shl ecx, 0x10
        { 0x38, 0xC1 },
        { 0x39, 0xE1 },
        { 0x3A, 0x10 },

        // D3DDevice_SetRenderState_MultiSampleAntiAlias+0x44 : or ecx, edx
        { 0x44, 0x0B },
        { 0x45, 0xCA },

        // D3DDevice_SetRenderState_MultiSampleAntiAlias+0x4F : retn 0x04
        { 0x4F, 0xC2 },
        { 0x50, 0x04 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_SetRenderState_LineWidth
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_LineWidth, 4432, 10)

        // D3DDevice_SetRenderState_LineWidth+0x15 : fmul dword ptr [esi+0x460]
        { 0x15, 0xD8 },
        { 0x16, 0x8E },
        { 0x17, 0x60 },
        { 0x18, 0x04 },

        // D3DDevice_SetRenderState_LineWidth+0x32 : cmp edi, 0x1FF
        { 0x32, 0x81 },
        { 0x33, 0xFF },

        // D3DDevice_SetRenderState_LineWidth+0x3A : mov edi, 0x1FF
        { 0x3A, 0xBF },
        { 0x3B, 0xFF },

        // D3DDevice_SetRenderState_LineWidth+0x62 : retn 0x04
        { 0x62, 0xC2 },
        { 0x63, 0x04 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_SetPalette
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPalette, 4432, 12)

        // D3DDevice_SetPalette+0x0D : mov eax, [edi+esi*4+0x0A88]
        { 0x0D, 0x8B },
        { 0x0E, 0x84 },
        { 0x0F, 0xBE },
        { 0x10, 0x88 },
        { 0x11, 0x0A },

        // D3DDevice_SetPalette+0x76 : add esi, 0x41B20
        { 0x76, 0x81 },
        { 0x77, 0xC7 },
        { 0x78, 0x20 },
        { 0x79, 0x1B },
        { 0x7A, 0x04 },

        // D3DDevice_SetPalette+0x8A : retn 0x08
        { 0x8A, 0xC2 },
        { 0x8B, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Swap
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Swap, 4432, 11)

        // D3DDevice_Swap+0x10 : mov ebx, 5
        { 0x10, 0xBB },
        { 0x11, 0x05 },

        // D3DDevice_Swap+0x1D : test bl, 3
        { 0x1D, 0xF6 },
        { 0x1E, 0xC3 },
        { 0x1F, 0x03 },

        // D3DDevice_Swap+0x46 : inc dword ptr [esi+0x2AD8]
        { 0x46, 0xFF },
        { 0x47, 0x86 },
        { 0x48, 0xD8 },
        { 0x49, 0x2A },

        // D3DDevice_Swap+0xAE : retn 4
        { 0xAE, 0xC2 },
        { 0xAF, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Swap
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Swap, 4531, 11)

        // D3DDevice_Swap+0x10 : mov ebx, 5
        { 0x10, 0xBB },
        { 0x11, 0x05 },

        // D3DDevice_Swap+0x1D : test bl, 3
        { 0x1D, 0xF6 },
        { 0x1E, 0xC3 },
        { 0x1F, 0x03 },

        // D3DDevice_Swap+0x46 : inc dword ptr [esi+0x2AD8]
        { 0x46, 0xFF },
        { 0x47, 0x86 },
        { 0x48, 0xD8 },
        { 0x49, 0x2A },

        // D3DDevice_Swap+0xAE : retn 4
        { 0xB9, 0xC2 },
        { 0xBA, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_UpdateOverlay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_UpdateOverlay, 4432, 11)

        // D3DDevice_UpdateOverlay+0x0F : mov [eax+0x2AA4], ecx
        { 0x0F, 0x89 },
        { 0x10, 0x88 },
        { 0x11, 0xA4 },
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
OOVPA_NO_XREF(D3DDevice_BlockUntilVerticalBlank, 4432, 11)

        // D3DDevice_BlockUntilVerticalBlank+0x05 : push 0; push 0; push 1
        { 0x05, 0x6A },
        { 0x06, 0x00 },
        { 0x07, 0x6A },
        { 0x08, 0x00 },
        { 0x09, 0x6A },
        { 0x0A, 0x01 },

        // D3DDevice_BlockUntilVerticalBlank+0x17 : add eax, 0x2448
        { 0x17, 0x05 },
        { 0x18, 0x48 },
        { 0x19, 0x24 },

        // D3DDevice_BlockUntilVerticalBlank+0x1D : call [KrnlImport]
        { 0x1D, 0xFF },

        // D3DDevice_BlockUntilVerticalBlank+0x23 : retn
        { 0x23, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetSwapCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetSwapCallback, 4432, 12)

        // D3DDevice_SetSwapCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // D3DDevice_SetSwapCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        // D3DDevice_SetSwapCallback+0x0A : mov [ecx+0x2444], eax
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0x40 }, // 40 vs 44
        { 0x0D, 0x24 },

        // D3DDevice_SetSwapCallback+0x10 : retn 0x04
        { 0x10, 0xC2 },
        { 0x11, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVerticalBlankCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVerticalBlankCallback, 4432, 12)

        // D3DDevice_SetVerticalBlankCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // D3DDevice_SetVerticalBlankCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        // D3DDevice_SetVerticalBlankCallback+0x0A : mov [ecx+0x2444], eax
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0x44 }, // 40 vs 44
        { 0x0D, 0x24 },

        // D3DDevice_SetVerticalBlankCallback+0x10 : retn 0x04
        { 0x10, 0xC2 },
        { 0x11, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_BackFillMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_BackFillMode, 4531, 9)

        { 0x04, 0x56 },
	{ 0x10, 0x8B },
	{ 0x11, 0x06 },
	{ 0x12, 0x3B },
	{ 0x13, 0x46 },
	{ 0x14, 0x04 },
	{ 0x41, 0x83 },
	{ 0x42, 0xC0 },
	{ 0x43, 0x0C },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BeginPush
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginPush, 4432, 6)

        { 0x09, 0x8B },
        { 0x0A, 0xCE },
        { 0x10, 0x8B },
        { 0x11, 0x44 },
        { 0x17, 0xE8 },
        { 0x1D, 0xC2 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3DDevice_Release
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Release, 4432, 11)

	{ 0x07, 0x8B },
	{ 0x08, 0x87 },
	{ 0x09, 0x40 },
	{ 0x0A, 0x04 },
	{ 0x0B, 0x00 },
	{ 0x0C, 0x00 },
	{ 0x1B, 0xB9 },
	{ 0x1C, 0xC0 },
	{ 0x1D, 0x0A },
	{ 0x1E, 0x00 },
	{ 0x1F, 0x00 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DTexture_GetSurfaceLevel
// ******************************************************************
OOVPA_NO_XREF(D3DTexture_GetSurfaceLevel, 4432, 14)

        // D3DTexture_GetSurfaceLevel+0x00 : sub esp, 0x0C
        { 0x00, 0x83 },
        { 0x01, 0xEC },
        { 0x02, 0x0C },

        // D3DTexture_GetSurfaceLevel+0x04 : mov esi, [esp+0x14]
        { 0x04, 0x8B },
        { 0x07, 0x14 },

        // D3DTexture_GetSurfaceLevel+0x04 : lea eax, [esp+0x14]
        { 0x08, 0x8D },
        { 0x0B, 0x14 },

        // D3DTexture_GetSurfaceLevel+0x17 : mov edx, [esp+0x24]
        { 0x17, 0x8B },
        { 0x1A, 0x24 },

        // D3DTexture_GetSurfaceLevel+0x1B : lea eax, [esp+0x18]
        { 0x1B, 0x8D },
        { 0x1E, 0x18 },

        // D3DTexture_GetSurfaceLevel+0x29 : call [abs]
        { 0x29, 0xE8 },

        // D3DTexture_GetSurfaceLevel+0x4C : retn 0x0C
        { 0x4C, 0xC2 },
        { 0x4D, 0x0C },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetDisplayMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetDisplayMode, 4432, 12)

        // D3DDevice_GetDisplayMode+0x01 : mov esi, ds:[*]
        { 0x01, 0x8B },
        { 0x02, 0x35 },

        // D3DDevice_GetDisplayMode+0x18 : shr ecx, 0x14
        { 0x18, 0xC1 },
        { 0x19, 0xE9 },
        { 0x1A, 0x14 },

        // D3DDevice_GetDisplayMode+0x50 : shr eax, 0x0C
        { 0x50, 0xC1 },
        { 0x51, 0xE8 },
        { 0x52, 0x0C },

        // D3DDevice_GetDisplayMode+0x6B : and eax, 0x0A
        { 0x6B, 0x83 },
        { 0x6C, 0xE0 },

        // D3DDevice_GetDisplayMode+0x92 : retn 4
        { 0x92, 0xC2 },
        { 0x93, 0x04 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3DDevice_SetFlickerFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetFlickerFilter, 4432, 11)

        // D3DDevice_SetFlickerFilter+0x1C : mov eax, [eax+0x227C]
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
#if 0 // No longer used, replaced by generic 4134 version
// ******************************************************************
// * D3DDevice_SetFlickerFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetSoftDisplayFilter, 4432, 12)

        // D3DDevice_SetSoftDisplayFilter+0x25 : mov edx, [eax+0x227C]
        { 0x25, 0x8B },
        { 0x26, 0x90 },
        { 0x27, 0x7C },
        { 0x28, 0x22 },

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
// * D3DVertexBuffer_Lock
// ******************************************************************
OOVPA_NO_XREF(D3DVertexBuffer_Lock, 4531, 11)

        // D3DVertexBuffer_Lock+0x01 : mov bl, [esp+0x18]
        { 0x01, 0x8A },
        { 0x02, 0x5C },
        { 0x03, 0x24 },
        { 0x04, 0x18 },

        // D3DVertexBuffer_Lock+0x09 : jnz +0x24
        { 0x09, 0x75 },
        { 0x0A, 0x24 },

        // D3DVertexBuffer_Lock+0x2F : test bl, 0xA0
        { 0x2F, 0xF6 },
        { 0x30, 0xC3 },
        { 0x31, 0xA0 },

        // D3DVertexBuffer_Lock+0x50 : retn 0x14
        { 0x54, 0xC2 },
        { 0x55, 0x14 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetProjectionViewportMatrix
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetProjectionViewportMatrix, 4432, 7)

        { 0x02, 0x35 },
        { 0x07, 0x57 },
        { 0x0A, 0x24 },
        { 0x0E, 0x70 },
        { 0x12, 0xB9 },
        { 0x16, 0x00 },
        { 0x1A, 0x5E },
OOVPA_END;
#if 0 // Moved to 4039
// ******************************************************************
// * D3DDevice_SetModelView
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetModelView, 4432, 7)

        { 0x15, 0x25 },
        { 0x2F, 0x81 },
        { 0x43, 0x43 },
        { 0x5A, 0x04 },
        { 0x72, 0x8D },
        { 0x88, 0x00 },
        { 0x9F, 0x75 },
OOVPA_END;
#endif
// ******************************************************************
// * D3DDevice_GetDisplayFieldStatus
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetDisplayFieldStatus, 4432, 7)

		{ 0x0B, 0x8B },
		{ 0x0C, 0x4C },
		{ 0x0D, 0x24 },
		{ 0x0E, 0x04 },
		{ 0x1E, 0x56 },
		{ 0x25, 0x33 },
		{ 0x26, 0xD2 },
OOVPA_END;

// ******************************************************************
// * D3D8_4432
// ******************************************************************
OOVPATable D3D8_4432[]  {

	REGISTER_OOVPA(CMiniport_InitHardware, 4361, PATCH),
	REGISTER_OOVPA(CMiniport_CreateCtxDmaObject, 4034, PATCH),
	REGISTER_OOVPA(D3D_CMiniport_GetDisplayCapabilities, 3911, PATCH),
	
	REGISTER_OOVPA(Direct3D_CreateDevice, 3911, PATCH),
	REGISTER_OOVPA(D3D_CheckDeviceFormat, 4134, DISABLED),
	REGISTER_OOVPA(MakeRequestedSpace, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderTarget, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_AddRef, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_GetBackBuffer, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_Reset, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_GetRenderTarget, 4432, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDepthStencilSurface, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVertexShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShader, 4432, PATCH),
	REGISTER_OOVPA(D3D_KickOffAndWaitForIdle, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetViewport, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTexture, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_EnableOverlay, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BumpEnv, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TwoSidedLighting, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_BackFillMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BorderColor, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_ColorKeyColor, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_TexCoordIndex, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_VertexBlend, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_CullMode, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FrontFace, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LogicOp, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_NormalizeNormals, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TextureFactor, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZBias, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetCreationParameters, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginVisibilityTest, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_EndVisibilityTest, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVisibilityTestResult, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_EdgeAntiAlias, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FillMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FogColor, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Simple, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LineWidth, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilFail, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_OcclusionCullEnable, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilCullEnable, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZCmpAlwaysRead, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZRead, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_DoNotCullUncompressed, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Dxt1NoiseEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZEnable, 4432, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleAntiAlias, 4432, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMask, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ShadowFunc, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SwitchTexture, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_YuvEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTransform, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTransform, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_Clear, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_CreatePalette, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPalette, 4432, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateTexture, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_Swap, 4432, PATCH),
	REGISTER_OOVPA(D3DDevice_Swap, 4531, PATCH),
	REGISTER_OOVPA(D3DDevice_UpdateOverlay, 4432, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVerticalBlankCallback, 4432, PATCH),
	REGISTER_OOVPA(D3DDevice_SetSwapCallback, 4432, PATCH),
	REGISTER_OOVPA(D3DDevice_BlockUntilVerticalBlank, 4432, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawIndexedVerticesUP, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetStreamSource, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShader, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawVerticesUP, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetLight, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetLightEnable, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetLight, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_LightEnable, 3911, PATCH),
	REGISTER_OOVPA(D3DResource_Register, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_AddRef, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_Release, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_IsBusy, 4039, PATCH),
	REGISTER_OOVPA(D3DTexture_GetSurfaceLevel, 4432, PATCH),
	REGISTER_OOVPA(D3DPalette_Lock, 3925, PATCH),
	REGISTER_OOVPA(D3DSurface_LockRect, 3925, PATCH),
	REGISTER_OOVPA(D3DTexture_LockRect, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_Release, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetMaterial, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetMaterial, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetBackMaterial, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetBackMaterial, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDeviceCaps, 3925, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetRenderState_BackFillMode, 4531, PATCH),
	REGISTER_OOVPA(D3DSurface_GetDesc, 3925, PATCH),
	REGISTER_OOVPA(Get2DSurfaceDesc, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShaderConstant, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_DeletePixelShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteVertexShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScreenSpaceOffset, 4039, PATCH),
	REGISTER_OOVPA(D3D_SetFence, 4134, XREF),
	REGISTER_OOVPA(D3DDevice_GetGammaRamp, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDisplayMode, 4432, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVertexBuffer, 3925, DISABLED),
	REGISTER_OOVPA(D3DVertexBuffer_Lock, 4034, PATCH),
	REGISTER_OOVPA(D3DVertexBuffer_Lock, 4531, PATCH),
	REGISTER_OOVPA(D3DDevice_SetShaderConstantMode, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_CreatePixelShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetViewport, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawVertices, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetFlickerFilter, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetSoftDisplayFilter, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetGammaRamp, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawIndexedVertices, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_Begin, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_End, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData2s, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData2f, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4f, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4ub, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetIndices, 4134, DISABLED),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderSize, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginStateBlock, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_EndStateBlock, 4134, PATCH),
	REGISTER_OOVPA(Lock3DSurface, 3911, PATCH),
	REGISTER_OOVPA(Lock2DSurface, 3925, PATCH),
	REGISTER_OOVPA(D3DBaseTexture_GetLevelCount, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetProjectionViewportMatrix, 4432, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderInput, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetModelView, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginPush, 4432, PATCH),
	REGISTER_OOVPA(D3DDevice_EndPush, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScissors, 4361, PATCH),
	REGISTER_OOVPA(D3D_SetPushBufferSize, 3925, DISABLED),
	REGISTER_OOVPA(D3D_BlockOnResource, 4034, XREF),
	REGISTER_OOVPA(D3DResource_BlockUntilNotBusy, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginPushBuffer, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_EndPushBuffer, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_RunPushBuffer, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_GetPushBufferOffset, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetScissors, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetBackBufferScale, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDisplayFieldStatus, 4432, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexDataColor, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVolumeTexture, 3925, PATCH),
	REGISTER_OOVPA(D3DVolumeTexture_LockBox, 3925, PATCH),// Just calls Lock3DSurface
	REGISTER_OOVPA(D3DDevice_PersistDisplay, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4s, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShaderProgram, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_FlushVertexCache, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_PrimeVertexCache, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_LoadVertexShaderProgram, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetModelView, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetShaderConstantMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShader, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderConstant, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderInput, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_RunVertexStateShader, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderType, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderDeclaration, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderFunction, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetPixelShader, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_IsFencePending, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SelectVertexShaderDirect, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderInputDirect, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_IsBusy, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_LoadVertexShader, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SelectVertexShader, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateCubeTexture, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_PSTextureModes, 4034, PATCH),
	REGISTER_OOVPA(D3DCubeTexture_LockRect, 3925, PATCH),// Just calls Lock2DSurface
	REGISTER_OOVPA(D3DCubeTexture_GetCubeMapSurface, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTile, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_ApplyStateBlock, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteStateBlock, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_KickOff, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_InsertCallback, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_InsertFence, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateImageSurface, 4034, PATCH),
	REGISTER_OOVPA(D3D_BlockOnTime, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_BlockOnFence, 4134, PATCH),
	REGISTER_OOVPA(D3D_ClearStateBlockFlags, 3911, XREF),
	REGISTER_OOVPA(D3DDevice_SetTile, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_CopyRects, 4134, PATCH),
	REGISTER_OOVPA(D3D_RecordStateBlock, 3911, XREF),
	REGISTER_OOVPA(D3D_LazySetPointParams, 4039, PATCH),

	// ******************************************************************
	// Provisional registration functions in XDK 4432
	// TODO: Need test cases
	// ******************************************************************
	REGISTER_OOVPA(D3DDevice_DrawRectPatch, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateStateBlock, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CaptureStateBlock, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetOverlayUpdateStatus, 3911, PATCH),
	// ******************************************************************
};

// ******************************************************************
// * D3D8_4432_SIZE
// ******************************************************************
uint32 D3D8_4432_SIZE = sizeof(D3D8_4432);
