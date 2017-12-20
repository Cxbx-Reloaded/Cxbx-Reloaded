// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->D3D8.1.0.5233.inl
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
// * D3DDevice_UpdateOverlay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_UpdateOverlay, 5233, 12) // Up to 5849

        { 0x00, 0x83 },
        { 0x13, 0x89 },
        { 0x14, 0x88 },
        { 0x74, 0x4F },

        { 0x80, 0xE1 },
        { 0x81, 0xFE },
        { 0x82, 0x0F },
        { 0x83, 0xAF },
        { 0x84, 0xCA },
        { 0x85, 0x25 },
        { 0x86, 0xFF },
        { 0x87, 0xFF },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BlockUntilVerticalBlank
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BlockUntilVerticalBlank, 5233, 11)

        // D3DDevice_BlockUntilVerticalBlank+0x05 : push 0; push 0; push 1
        { 0x05, 0x6A },
        { 0x06, 0x00 },
        { 0x07, 0x6A },
        { 0x08, 0x00 },
        { 0x09, 0x6A },
        { 0x0A, 0x01 },

        // D3DDevice_BlockUntilVerticalBlank+0x17 : add eax, 0x195C
        { 0x17, 0x05 },
        { 0x18, 0x5C },
        { 0x19, 0x19 },

        // D3DDevice_BlockUntilVerticalBlank+0x1D : call [KrnlImport]
        { 0x1D, 0xFF },

        // D3DDevice_BlockUntilVerticalBlank+0x23 : retn
        { 0x23, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleMode, 5233, 13)

        // D3DDevice_SetRenderState_MultiSampleMode+0x04 : mov ecx, ds:dword_XXXX
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        // D3DDevice_SetRenderState_MultiSampleMode+0x0F : mov edx, [ecx+XXXXh]
        { 0x0F, 0x8B },
        { 0x10, 0x91 },

        // D3DDevice_SetRenderState_MultiSampleMode+0x15 : cmp edx, [ecx+XXXXh]
        { 0x15, 0x3B },
        { 0x16, 0x91 },

        // D3DDevice_SetRenderState_MultiSampleMode+0x1B : jnz short
        { 0x1B, 0x75 },
        { 0x1C, 0x07 },

        // D3DDevice_SetRenderState_MultiSampleMode+0x1D : xor edx, edx
        { 0x1D, 0x33 },
        { 0x1E, 0xD2 },

        // D3DDevice_SetRenderState_MultiSampleMode+0x24 : retn 4
        { 0x24, 0xC2 },
        { 0x25, 0x04 },
        { 0x26, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVerticalBlankCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVerticalBlankCallback, 5233, 12)

        // D3DDevice_SetVerticalBlankCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // D3DDevice_SetVerticalBlankCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        // D3DDevice_SetVerticalBlankCallback+0x0A : mov [ecx+0x1958], eax
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0x58 }, // 54 vs 58
        { 0x0D, 0x19 },

        // D3DDevice_SetVerticalBlankCallback+0x10 : retn 0x04
        { 0x10, 0xC2 },
        { 0x11, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetSwapCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetSwapCallback, 5233, 12)

        // D3DDevice_SetSwapCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // D3DDevice_SetSwapCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        // D3DDevice_SetSwapCallback+0x0A : mov [ecx+0x1954], eax
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0x54 }, // 54 vs 58
        { 0x0D, 0x19 },

        // D3DDevice_SetSwapCallback+0x10 : retn 0x04
        { 0x10, 0xC2 },
        { 0x11, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderTargetFast
// ******************************************************************
#ifndef WIP_LessVertexPatching
OOVPA_NO_XREF(D3DDevice_SetRenderTargetFast, 5233, 27)
#else
OOVPA_XREF(D3DDevice_SetRenderTargetFast, 5233, 1+27,
	
	XRefNoSaveIndex,
	XRefOne)

		XREF_ENTRY( 0x15, XREF_OFFSET_D3DDEVICE_M_RENDERTARGET ), // Derived
#endif
		// D3DDevice_SetRenderTargetFast+0x00 : push ebp
		{ 0x00, 0x55 },

		// D3DDevice_SetRenderTargetFast+0x01 : mov ebp, [esp+8]
		{ 0x01, 0x8B },
		{ 0x02, 0x6C },
		{ 0x03, 0x24 },
		{ 0x04, 0x08 },

		// D3DDevice_SetRenderTargetFast+0x05 : push esi
		{ 0x05, 0x56 },

		// D3DDevice_SetRenderTargetFast+0x06 : push edi
		{ 0x06, 0x57 },

		// D3DDevice_SetRenderTargetFast+0x07 : mov edi, [ds:{addr}]
		{ 0x07, 0x8B },
		{ 0x08, 0x3D },

		// D3DDevice_SetRenderTargetFast+0x0D : push ebp
		{ 0x0D, 0x55 },

		// D3DDevice_SetRenderTargetFast+0x0E : call {addr}
		{ 0x0E, 0xE8 },

		// D3DDevice_SetRenderTargetFast+0x13 : mov eax, [edi+{offset}]
		{ 0x13, 0x8B },
		{ 0x14, 0x87 },
	
		// D3DDevice_SetRenderTargetFast+0x19 : mov ecx, [eax+0x14]
		{ 0x19, 0x8B },
		{ 0x1A, 0x48 },
		{ 0x1B, 0x14 },
	
		// D3DDevice_SetRenderTargetFast+0x1C : test ecx, ecx
		{ 0x1C, 0x85 },
		{ 0x1D, 0xC9 },

		// D3DDevice_SetRenderTargetFast+0x1E : jz + 0x02
		{ 0x1E, 0x74 },
		{ 0x1F, 0x02 },
	
        { 0x3C, 0x89 },
        { 0x5F, 0x8B },
        { 0x7A, 0x04 },
        { 0x99, 0x04 },
        { 0xB8, 0x02 },
        { 0xD7, 0x0C },
        { 0xF6, 0xC5 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetVisibilityTestResult
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVisibilityTestResult, 5233, 7)

        { 0x0E, 0x08 },
        { 0x10, 0xFF },
        { 0x2A, 0x16 },
        { 0x3C, 0x76 },
        { 0x3F, 0x0C },
        { 0x51, 0x0A },
        { 0x5F, 0x0C },
OOVPA_END;
