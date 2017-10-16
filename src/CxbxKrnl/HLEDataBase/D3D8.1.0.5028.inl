// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->D3D8.1.0.5028.cpp
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
// * Direct3D_CreateDevice
// ******************************************************************
OOVPA_NO_XREF(Direct3D_CreateDevice, 5028, 20) // Also for 5120, 5233, 5344 (5455 and later use generic 3911 version)

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

// ******************************************************************
// * D3DDevice_SetRenderState_TextureFactor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_TextureFactor, 5028, 9)

        // D3DDevice_SetRenderState_TextureFactor+0x07 : mov eax, [esi+0x036C]
        { 0x07, 0x8B },
        { 0x08, 0x86 },
        { 0x09, 0x6C },
        { 0x0A, 0x03 },

        // D3DDevice_SetRenderState_TextureFactor+0x2F : nop
        { 0x2F, 0x90 },

        // D3DDevice_SetRenderState_TextureFactor+0x41 : retn 0x04
        { 0x41, 0xC2 },
        { 0x42, 0x04 },

        // D3DDevice_SetRenderState_TextureFactor+0x4E : retn 0x04
        { 0x4E, 0xC2 },
        { 0x4F, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_Dxt1NoiseEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_Dxt1NoiseEnable, 5028, 7)

        { 0x0F, 0xB4 },
        { 0x10, 0x15 },
        { 0x16, 0x0D },
        { 0x1F, 0x3C },
        { 0x34, 0x36 },
        { 0x58, 0x05 },
        { 0x75, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_ZEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_ZEnable, 5028, 8)

        { 0x13, 0x8B },
        { 0x22, 0xC9 },
        { 0x34, 0x89 },
        { 0x46, 0x0C },
        { 0x5A, 0x74 },
        { 0x6D, 0x8B },
        { 0x7C, 0xCE },
        { 0x8E, 0x00 },
OOVPA_END;
#if 0 // Moved to 4627
// ******************************************************************
// * D3DDevice_SetRenderState_SampleAlpha
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_SampleAlpha, 5028, 7)

        { 0x0B, 0xC1 },
        { 0x15, 0x0B },
        { 0x1C, 0x8B },
        { 0x28, 0x85 },
        { 0x30, 0x07 },
        { 0x3B, 0xC7 },
        { 0x44, 0x83 },
OOVPA_END;
#endif
// ******************************************************************
// * D3DDevice_CreateTexture2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateTexture2, 5028, 11) // Also for 5233, 5344, 5558, 5659, 5788, 5849

        { 0x00, 0x56 },
        { 0x18, 0x33 },
        { 0x19, 0xC9 },
        { 0x1A, 0x83 },
        { 0x1B, 0xF8 },
        { 0x1C, 0x05 },
        { 0x1D, 0x8B },
        { 0x1E, 0x44 },
        { 0x1F, 0x24 },
        { 0x42, 0x8B },
        { 0x52, 0xF7 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BlockUntilVerticalBlank
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BlockUntilVerticalBlank, 5028, 7)

        { 0x05, 0x6A },
        { 0x12, 0x00 },
        { 0x15, 0x6A },
        { 0x16, 0x06 },
        { 0x17, 0x05 },
        { 0x1C, 0x50 },
        { 0x23, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetBackBuffer2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetBackBuffer2, 5028, 12)

        // D3DDevice_GetBackBuffer2+0x04 : cmp eax, 0xFFFFFFFF
        { 0x04, 0x83 },
        { 0x05, 0xF8 },
        { 0x06, 0xFF },

        // D3DDevice_GetBackBuffer+0x0D : jnz +0x19
        { 0x0D, 0x75 },
        { 0x0E, 0x19 },

        // D3DDevice_GetBackBuffer+0x15 : mov esi, [ecx+eax*4+0x15C0]
        { 0x15, 0x8B },
        { 0x16, 0xB4 },
        { 0x17, 0x81 },
        { 0x18, 0xC0 },
        { 0x19, 0x15 },

        // D3DDevice_GetBackBuffer+0x40 : retn 0x04
        { 0x40, 0xC2 },
        { 0x41, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetRenderTarget2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetRenderTarget2, 5028, 6)

        { 0x05, 0x56 },
        { 0x06, 0x8B },
        { 0x0A, 0x00 },
        { 0x0E, 0x74 },
        { 0x16, 0x8B },
        { 0x17, 0xC6 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetDepthStencilSurface2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetDepthStencilSurface2, 5028, 7)

        { 0x05, 0x56 },
        { 0x06, 0x8B },
        { 0x0A, 0x00 },
        { 0x0E, 0x75 },
        { 0x12, 0x5E },
        { 0x1A, 0x8B },
        { 0x1B, 0xC6 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetViewport
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetViewport, 5028, 9)

        // D3DDevice_SetViewport+0x0A : mov eax, [esi+0x15B4]
        { 0x0A, 0x8B },
        { 0x0B, 0x86 },
        { 0x0C, 0xB4 },
        { 0x0D, 0x15 },

        // D3DDevice_SetViewport+0x63 : shl edi, cl
        { 0x63, 0xD3 },
        { 0x64, 0xE7 },

        // D3DDevice_SetViewport+0xDF : and ecx, 0x0F
        { 0xDF, 0x83 },
        { 0xE0, 0xE1 },
        { 0xE1, 0x0F },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetLight
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetLight, 5028, 12)

        // D3DDevice_SetLight+0x11 : mov edi, [ebp+0x390]
        { 0x11, 0x8B },
        { 0x12, 0xBD },
        { 0x13, 0x90 },
        { 0x14, 0x03 },

        // D3DDevice_SetLight+0x30 : shl eax, 0x04
        { 0x30, 0xC1 },
        { 0x31, 0xE0 },
        { 0x32, 0x04 },

        // D3DDevice_SetLight+0xBD : rep stosb
        { 0xBD, 0xF3 },
        { 0xBE, 0xAB },

        // D3DDevice_SetLight+0xFD : mov ecx, [ebx+0x68]
        { 0xFD, 0x8B },
        { 0xFE, 0x4B },
        { 0xFF, 0x68 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_LightEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_LightEnable, 5028, 13)

        // D3DDevice_LightEnable+0x0F : cmp ebx, [esi+0x0390]
        { 0x0F, 0x3B },
        { 0x10, 0x9E },
        { 0x11, 0x90 },
        { 0x12, 0x03 },

        // D3DDevice_LightEnable+0x6B : test eax, eax
        { 0x6B, 0x85 },
        { 0x6C, 0xC0 },

        // D3DDevice_LightEnable+0x9E : mov eax, [eax+0x8C]
        { 0x9E, 0x8B },
        { 0x9F, 0x80 },
        { 0xA0, 0x8C },

        // D3DDevice_LightEnable+0xBC : jz +0x12
        { 0xBC, 0x74 },
        { 0xBD, 0x12 },

        // D3DDevice_LightEnable+0xE1 : retn 0x08
        { 0xE1, 0xC2 },
        { 0xE2, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetIndices
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetIndices, 5028, 7)

        { 0x0F, 0x10 },
        { 0x20, 0xC7 },
        { 0x31, 0xC0 },
        { 0x42, 0x78 },
        { 0x53, 0xBE },
        { 0x64, 0x89 },
        { 0x75, 0x10 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTile
// ******************************************************************
// Accurately this one ?SetTileNoWait@D3D@@YGXKPBU_D3DTILE@@@Z
OOVPA_NO_XREF(D3DDevice_SetTile, 5028, 11)

        // D3DDevice_SetTile+0x06 : sub esp, 0x18
        { 0x06, 0x83 },
        { 0x07, 0xEC },
        { 0x08, 0x18 },

        // D3DDevice_SetTile+0x15 : cmp [esi+4], eax
        { 0x15, 0x39 },
        { 0x16, 0x46 },
        { 0x17, 0x04 },

        // D3DDevice_SetTile+0x3D : lea edi, [edx+ecx*8+0xXXXX]
        { 0x3D, 0x8D },
        { 0x3E, 0xBC },
        { 0x3F, 0xCA },
        { 0x42, 0x00 },
        { 0x43, 0x00 },
OOVPA_END;


// ******************************************************************
// * D3DDevice_GetTile
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetTile, 5028, 13)

        { 0x05, 0x0D },

        { 0x0A, 0x56 },
        { 0x0B, 0x57 },

        { 0x18, 0x00 },
        { 0x19, 0x00 },
        { 0x1A, 0xB9 },
        { 0x1B, 0x06 },
        { 0x1C, 0x00 },
        { 0x1D, 0x00 },
        { 0x1E, 0x00 },
        { 0x1F, 0xF3 },

        { 0x23, 0xC2 },
        { 0x24, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Clear
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Clear, 5028, 8)
        { 0x1D, 0x8A },
        { 0x3C, 0x8B },
        { 0x5B, 0x00 },
        { 0x7A, 0xFD },
        { 0x99, 0x25 },
        { 0xB8, 0x00 },
        { 0xD7, 0x75 },
        { 0xF6, 0xFF },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawVertices
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawVertices, 5028, 7)

        { 0x12, 0x8B },
        { 0x26, 0x8B },
        { 0x37, 0x00 },
        { 0x4A, 0x27 },
        { 0x5D, 0x89 },
        { 0x70, 0x75 },
        { 0x83, 0x40 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawIndexedVertices
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawIndexedVertices, 5028, 8)

        { 0x1E, 0x5E },
        { 0x3E, 0x17 },
        { 0x5E, 0x00 },
        { 0x7E, 0x8B },
        { 0x9E, 0x89 },
        { 0xBE, 0x2B },
        { 0xDE, 0x00 },
        { 0xFE, 0x04 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 4134 version
// ******************************************************************
// * D3DDevice_SetVertexData2f
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData2f, 5028, 8)

        { 0x07, 0x8B },
        { 0x0C, 0x72 },
        { 0x13, 0x8B },
        { 0x1A, 0x80 },
        { 0x21, 0x0C },
        { 0x28, 0x89 },
        { 0x2F, 0xC0 },
        { 0x36, 0x00 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_LoadVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_LoadVertexShader, 5028, 7)

        { 0x09, 0x45 },
        { 0x14, 0x75 },
        { 0x21, 0x8B },
        { 0x2D, 0x8B },
        { 0x35, 0x04 },
        { 0x40, 0x00 },
        { 0x4B, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetShaderConstantMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetShaderConstantMode, 5028, 8)

        { 0x1D, 0xFD },
        { 0x3C, 0x8B },
        { 0x5B, 0x0B },
        { 0x7A, 0x56 },
        { 0x99, 0xC0 },
        { 0xB8, 0x00 },
        { 0xD7, 0x89 },
        { 0xF6, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShader, 5028, 14)

        // D3DDevice_SetVertexShader+0x06 : test bl, 1
        { 0x06, 0xF6 },
        { 0x07, 0xC3 },
        { 0x08, 0x01 },

        // D3DDevice_SetVertexShader+0x11 : mov eax, [esi+0x037C]
        { 0x11, 0x8B },
        { 0x12, 0x86 },
        { 0x13, 0x7C },
        { 0x14, 0x03 },

        // D3DDevice_SetVertexShader+0x90 : retn 0x04
        { 0x90, 0xC2 },
        { 0x91, 0x04 },

        // D3DDevice_SetVertexShader+0xB0 : mov dword ptr [eax], 0x4194C
        { 0xB0, 0xC7 },
        { 0xB1, 0x00 },
        { 0xB2, 0x4C },
        { 0xB3, 0x19 },
        { 0xB4, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Swap
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Swap, 5028, 7)

        { 0x1B, 0xE8 },
        { 0x38, 0xC3 },
        { 0x55, 0xC0 },
        { 0x72, 0x3F },
        { 0x8F, 0x8E },
        { 0xAC, 0xC6 },
        { 0xC9, 0xC0 },
OOVPA_END;

// ******************************************************************
// * D3D::SetFence
// ******************************************************************
OOVPA_XREF(D3D_SetFence, 5028, 7,

    XREF_D3D_SetFence,
    XRefZero)

        { 0x0E, 0x05 },
        { 0x18, 0xC9 },
        { 0x3E, 0x3F },
        { 0x5E, 0x28 },
        { 0x86, 0x5D },
        { 0x98, 0xE8 },
        { 0xA2, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3D::BlockOnTime
// ******************************************************************
OOVPA_XREF(D3D_BlockOnTime, 5028, 6,

    XREF_D3D_BlockOnTime,
    XRefZero)

        { 0x09, 0x30 },
        { 0x27, 0x07 },
        { 0x55, 0x7E },
        { 0x7B, 0x58 },
        { 0xE3, 0x80 },
        { 0xF5, 0x2C },
OOVPA_END;

// ******************************************************************
// * D3D_KickOffAndWaitForIdle
// ******************************************************************
OOVPA_NO_XREF(D3D_KickOffAndWaitForIdle, 5028, 9)

        // D3D_KickOffAndWaitForIdle+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3D_KickOffAndWaitForIdle+0x05 : mov ecx, [eax+0x2C]
        { 0x05, 0x8B },
        { 0x06, 0x48 },
        { 0x07, 0x2C },

        // D3D_KickOffAndWaitForIdle+0x08 : push 2
        { 0x08, 0x6A },
        { 0x09, 0x02 },

        // D3D_KickOffAndWaitForIdle+0x0A : push ecx
        { 0x0A, 0x51 },

        // D3D_KickOffAndWaitForIdle+0x0B : call [addr]
        { 0x0B, 0xE8 },

        // D3D_KickOffAndWaitForIdle+0x10 : retn
        { 0x10, 0xC3 },
OOVPA_END;

// ******************************************************************
// * Get2DSurfaceDesc
// ******************************************************************
OOVPA_NO_XREF(Get2DSurfaceDesc, 5028, 10)

        // Get2DSurfaceDesc+0x2B : movzx edx, byte ptr [edi+0x0D]
        { 0x2B, 0x0F },
        { 0x2C, 0xB6 },
        { 0x2D, 0x57 },
        { 0x2E, 0x0D },

        // Get2DSurfaceDesc+0x52 : mov edx, [eax+0x15C0]
        { 0x52, 0x8B },
        { 0x53, 0x90 },
        { 0x54, 0xC0 },
        { 0x55, 0x15 },

        // Get2DSurfaceDesc+0xAE : retn 0x0C
        { 0xAE, 0xC2 },
        { 0xAF, 0x0C },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShaderConstantNotInline
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShaderConstantNotInline, 5028, 10)

        { 0x00, 0x55 },
        { 0x0B, 0x10 },

        { 0x10, 0x75 },
        { 0x11, 0x15 },
        { 0x12, 0x56 },
        { 0x13, 0x57 },
        { 0x14, 0x8B },

        { 0x1A, 0xC7 },
        { 0x31, 0xC2 },
        { 0x32, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_AddRef
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_AddRef, 5028, 10)

        // D3DDevice_AddRef+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3DDevice_AddRef+0x05 : mov ecx, [eax+0x04FC]
        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0xFC },
        { 0x08, 0x04 },

        // D3DDevice_AddRef+0x0B : inc ecx
        { 0x0B, 0x41 },

        // D3DDevice_AddRef+0x0C : mov [eax+0x04FC], ecx
        { 0x0C, 0x89 },
        { 0x0D, 0x88 },
        { 0x0E, 0xFC },
        { 0x0F, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_InsertCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_InsertCallback, 5028, 7)

        { 0x0C, 0x72 },
        { 0x1A, 0x10 },
        { 0x28, 0x85 },
        { 0x36, 0xC7 },
        { 0x44, 0x0C },
        { 0x52, 0x00 },
        { 0x60, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVertexShader, 5028, 7)

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x80 },
        { 0x0A, 0x00 },
        { 0x0D, 0x24 },
        { 0x10, 0x0A },
        { 0x13, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetVertexShaderConstant
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVertexShaderConstant, 5028, 8)

        { 0x07, 0x24 },
        { 0x08, 0x08 },
        { 0x09, 0xC1 },
        { 0x0A, 0xE1 },
        { 0x0B, 0x04 },
        { 0x10, 0x7C },
        { 0x14, 0xE6 },
        { 0x18, 0x02 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetPixelShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetPixelShader, 5028, 7)

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x6C },
        { 0x0A, 0x00 },
        { 0x0D, 0x24 },
        { 0x10, 0x0A },
        { 0x13, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3D::CommonSetRenderTarget
// ******************************************************************
OOVPA_XREF(D3D_CommonSetRenderTarget, 5028, 12,

    XREF_D3D_CommonSetRenderTarget,
    XRefZero)

        { 0x02, 0x1C },
        { 0x1A, 0x15 },

        { 0x3D, 0x28 },
        { 0x3E, 0x05 },
        { 0x3F, 0x00 },
        { 0x40, 0x00 },
        { 0x41, 0x89 },
        { 0x42, 0x4C },
        { 0x43, 0x24 },
        { 0x44, 0x24 },

        { 0x7E, 0xD8 },
        { 0x7F, 0x05 },
OOVPA_END;
#if 0 // No longer used, replaced by generic 5028 version
// ******************************************************************
// * D3D_CommonSetRenderTarget
// ******************************************************************
OOVPA_XREF(D3D_CommonSetRenderTarget, 5028, 10,

    XREF_D3D_CommonSetRenderTarget,
    XRefZero)

        { 0x06, 0x24 },
        { 0x07, 0x24 },
        { 0x08, 0x55 },
        { 0x09, 0x56 },
        { 0x0A, 0x8B },
        { 0x35, 0x8B },
        { 0x3C, 0x96 },
        { 0x47, 0xC4 },
        { 0x57, 0x10 },
        { 0x67, 0x00 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4831 version
// ******************************************************************
// * D3DDevice_PersistDisplay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_PersistDisplay, 5028, 10)

        { 0x01, 0xEC },
        { 0x02, 0x08 },
        { 0x03, 0x53 },
        { 0x04, 0x8B },
        { 0x05, 0x1D },
        { 0x11, 0xC0 },
        { 0x12, 0x74 },
        { 0x13, 0x0F },
        { 0x23, 0x8B },
        { 0x24, 0x83 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_LazySetStateVB
// ******************************************************************
OOVPA_XREF(D3DDevice_LazySetStateVB, 5028, 12,

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
// * D3DDevice_BeginPush
// ******************************************************************
OOVPA_XREF(D3DDevice_BeginPush, 5028, 10,

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

#if 0
// ******************************************************************
// * D3DDevice_BeginPush
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginPush, 5028, 6)

        { 0x06, 0x6A },
        { 0x07, 0x00 },
        { 0x0D, 0x8B },
        { 0x0E, 0x44 },
        { 0x12, 0x89 },
        { 0x16, 0xE9 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_End
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_End, 5028, 12)

        // D3DDevice_End+0x13 : mov dword ptr [eax], 0x417FC
        { 0x13, 0xC7 },
        { 0x14, 0x00 },
        { 0x15, 0xFC },
        { 0x16, 0x17 },
        { 0x17, 0x04 },
        { 0x18, 0x00 },

        // D3DDevice_End+0x28 : test ah, 0x10
        { 0x28, 0xF6 },
        { 0x29, 0xC4 },
        { 0x2A, 0x10 },

        // D3DDevice_End+0x2B : jz +0x07
        { 0x2B, 0x74 },
        { 0x2C, 0x07 },

        // D3DDevice_End+0x3C : retn
        { 0x3C, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_IsFencePending
// ******************************************************************
OOVPA_XREF(D3DDevice_IsFencePending, 5028, 6,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x01, XREF_D3DDEVICE ),

        { 0x07, 0x30 },
        { 0x0A, 0x2C },
        { 0x10, 0xD1 },
        { 0x17, 0x1B },
        { 0x1C, 0x04 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 4531 version
// ******************************************************************
// * D3DDevice_UpdateOverlay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_UpdateOverlay, 5028, 8)

        { 0x0F, 0x89 },
        { 0x10, 0x88 },
        { 0x11, 0xA8 },
        { 0x12, 0x1F },
        { 0x70, 0x4F },
        { 0x78, 0x8B },
        { 0x80, 0xCA },
        { 0x88, 0x8B },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3DDevice_GetOverlayUpdateStatus
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetOverlayUpdateStatus, 5028, 12)

        // D3DDevice_GetOverlayUpdateStatus+0x05 : mov ecx, [eax+0x2BA8]
        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0xA8 },
        { 0x08, 0x1F },

        // D3DDevice_GetOverlayUpdateStatus+0x0C : mov esi, [eax+0x2580]
        { 0x0C, 0x8B },
        { 0x0D, 0xB0 },
        { 0x0E, 0x80 },
        { 0x0F, 0x19 },

        // D3DDevice_GetOverlayUpdateStatus+0x16 : setnz dl
        { 0x16, 0x0F },
        { 0x17, 0x95 },
        { 0x18, 0xC2 },

        // D3DDevice_GetOverlayUpdateStatus+0x1C : retn
        { 0x1C, 0xC3 },
OOVPA_END;
#endif
// ******************************************************************
// * D3D::CDevice::KickOff
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_KickOff, 5028, 8)

        { 0x07, 0x08 },
        { 0x17, 0xC4 },
        { 0x18, 0x20 },
        { 0x19, 0x75 },
        { 0x1A, 0x63 },
        { 0x1B, 0xA1 },
        { 0x6D, 0x85 },
        { 0x7E, 0xBA },
OOVPA_END;

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3DDevice_GetLightEnable
// ******************************************************************
OOVPA_XREF(D3DDevice_GetLightEnable, 5028, 1+26,

	XRefNoSaveIndex,
	XRefOne)

        XREF_ENTRY( 0x05, XREF_D3DDEVICE),

		{ 0x00, 0x8B },
		{ 0x01, 0x4C },
		{ 0x02, 0x24 },
		{ 0x03, 0x04 },
	
		{ 0x04, 0xA1 },
		{ 0x09, 0x8B },
		{ 0x0A, 0x90 },
		{ 0x0B, 0x80 },
		{ 0x0C, 0x04 },

		{ 0x0D, 0x00 },
		{ 0x0E, 0x00 },
		{ 0x0F, 0x8B },
		{ 0x10, 0x80 },
	//	{ 0x11, 0x88 }, // 88 vs 94

	//	{ 0x12, 0x04 }, // 04 vs 03
		{ 0x13, 0x00 },
		{ 0x14, 0x00 },
		{ 0x15, 0x8D },
		{ 0x16, 0x0C },

		{ 0x17, 0xC9 },
		{ 0x18, 0xC1 },
		{ 0x19, 0xE1 },
		{ 0x1A, 0x04 },
		{ 0x1B, 0x03 },

		{ 0x1C, 0xCA },
		{ 0x1D, 0x85 },
		{ 0x1E, 0xC0 },
		{ 0x1F, 0x74 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_BeginStateBig
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginStateBig, 5028, 15)

        { 0x01, 0x0D },
        { 0x07, 0x01 },
        { 0x10, 0x8D },

        { 0x16, 0x02 },
        { 0x17, 0x00 },
        { 0x18, 0x00 },
        { 0x19, 0x3B },
        { 0x1A, 0xD1 },
        { 0x1B, 0x72 },
        { 0x1C, 0x23 },
        { 0x1D, 0xA1 },

        { 0x22, 0x8B },
        { 0x2B, 0x00 },
        { 0x34, 0xCA },
        { 0x40, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_IsBusy
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_IsBusy, 5028, 12)

        { 0x00, 0xA1 },
        { 0x06, 0x80 },

        { 0x13, 0x44 },
        { 0x14, 0x32 },
        { 0x15, 0x00 },
        { 0x16, 0x00 },
        { 0x17, 0x74 },
        { 0x18, 0x06 },
        { 0x19, 0xB8 },
        { 0x1A, 0x01 },
        { 0x1B, 0x00 },

        { 0x1F, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetSwapCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetSwapCallback, 5028, 12)

        // D3DDevice_SetSwapCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // D3DDevice_SetSwapCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        // D3DDevice_SetSwapCallback+0x0A : mov [ecx+0x194C], eax
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0x4C }, // 4C vs 50
        { 0x0D, 0x19 },

        // D3DDevice_SetSwapCallback+0x10 : retn 0x04
        { 0x10, 0xC2 },
        { 0x11, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVerticalBlankCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVerticalBlankCallback, 5028, 12)

        // D3DDevice_SetVerticalBlankCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // D3DDevice_SetVerticalBlankCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        // D3DDevice_SetVerticalBlankCallback+0x0A : mov [ecx+0x1950], eax
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0x50 }, // 4C vs 50
        { 0x0D, 0x19 },

        // D3DDevice_SetVerticalBlankCallback+0x10 : retn 0x04
        { 0x10, 0xC2 },
        { 0x11, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawIndexedVerticesUP
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawIndexedVerticesUP, 5028, 17) // Also for 5120, 5233

        { 0x00, 0x55 },
        { 0x01, 0x8B },
        { 0x02, 0xEC },
        { 0x03, 0x83 },
        { 0x04, 0xEC },
        { 0x05, 0x14 },

        { 0x23, 0x8B },
        { 0x3B, 0x86 },

        { 0x53, 0x0C },
        { 0x54, 0x83 },
        { 0x55, 0xF9 },
        { 0x56, 0x10 },
        { 0x57, 0x89 },
        { 0x58, 0x56 },
        { 0x59, 0x08 },
        { 0x5A, 0x8D },
        { 0x5B, 0x96 },
OOVPA_END;

// ******************************************************************
// * D3D_MakeRequestedSpace
// ******************************************************************
OOVPA_XREF(D3D_MakeRequestedSpace, 5028, 28, // Also for 5344

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
#if 0 // Moved to 5120
// ******************************************************************
// * D3DDevice_RunPushBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_RunPushBuffer, 5120, 13)

        { 0x0B, 0x57 },
        { 0x10, 0xE8 },

        { 0x15, 0x8B },
        { 0x1F, 0x01 },

        { 0x8D, 0x8B },
        { 0x8E, 0x4E },
        { 0x8F, 0x30 },
        { 0x90, 0x8B },
        { 0x91, 0x11 },
        { 0x92, 0x8B },
        { 0x93, 0x4E },
        { 0x94, 0x2C },
        { 0x95, 0x8B },
OOVPA_END;
#endif
// ******************************************************************
// * Rollback support signature(s)
// ******************************************************************
#define D3DDevice_Begin_5028 D3DDevice_Begin_4361

// ******************************************************************
// * D3D8_5028
// ******************************************************************
// No longer supported XDK 5120 by HLEDB_v1
OOVPATable D3D8_5028[] = {

	REGISTER_OOVPA(CMiniport_InitHardware, 4627, XREF),
	REGISTER_OOVPA(CMiniport_CreateCtxDmaObject, 4034, XREF),
	REGISTER_OOVPA(D3D_CMiniport_GetDisplayCapabilities, 3911, XREF),
	REGISTER_OOVPA(D3D_MakeRequestedSpace, 5028, PATCH),

	REGISTER_OOVPA(Direct3D_CreateDevice, 5028, PATCH),
	REGISTER_OOVPA(D3D_CheckDeviceFormat, 4134, DISABLED),
	REGISTER_OOVPA(D3DDevice_BeginVisibilityTest, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetCreationParameters, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_EndVisibilityTest, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVisibilityTestResult, 3911, PATCH),
	REGISTER_OOVPA(D3D_KickOffAndWaitForIdle, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_LazySetStateVB, 5028, XREF),
	REGISTER_OOVPA(D3DDevice_BeginPush, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_EndPush, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_CopyRects, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetGammaRamp, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetBackBuffer2, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetViewport, 5028, PATCH),
	REGISTER_OOVPA(D3D_CommonSetRenderTarget, 5028, XREF),
	REGISTER_OOVPA(D3DDevice_SetRenderTarget, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_AddRef, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetGammaRamp, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetShaderConstantMode, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_GetRenderTarget2, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDepthStencilSurface2, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTile, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTile, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateIndexBuffer2, 4627, DISABLED),
	REGISTER_OOVPA(D3DDevice_CreateVertexShader, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShaderConstant, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShaderConstant, 4831, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant1, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant1, 4627, ALIAS, D3DDevice_SetVertexShaderConstant1Fast),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant4, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstantNotInline, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstantNotInline, 4627, ALIAS, D3DDevice_SetVertexShaderConstantNotInlineFast),
	REGISTER_OOVPA(D3DDevice_DeletePixelShader, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_CreatePixelShader, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShader, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateTexture2, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetIndices, 5028, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetTexture, 4831, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDisplayMode, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_Begin, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginStateBig, 5028, XREF), 
	REGISTER_OOVPA(D3DDevice_SetVertexData2f, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_End, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_Clear, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_Swap, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_EnableOverlay, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVertexBuffer2, 4627, DISABLED),
	REGISTER_OOVPA(D3DDevice_UpdateOverlay, 4531, PATCH),
	REGISTER_OOVPA(D3DDevice_GetOverlayUpdateStatus, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_BlockUntilVerticalBlank, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVerticalBlankCallback, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_GetViewport, 4627, PATCH),
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
	REGISTER_OOVPA(D3DDevice_SetRenderState_BackFillMode, 4531, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FogColor, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Dxt1NoiseEnable, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Simple, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZEnable, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LineWidth, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_OcclusionCullEnable, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleAntiAlias, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ShadowFunc, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SwitchTexture, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_YuvEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTransform, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTransform, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_GetStreamSource2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetStreamSource, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawVerticesUP, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShader, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawVertices, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_GetLight, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetLightEnable, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetLight, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawIndexedVertices, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_LightEnable, 5028, PATCH),
	REGISTER_OOVPA(D3DVertexBuffer_Lock2, 4627, PATCH),
	REGISTER_OOVPA(D3DResource_Register, 3911, PATCH),
	REGISTER_OOVPA(D3DResource_AddRef, 3911, PATCH),
	REGISTER_OOVPA(D3DResource_Release, 3911, PATCH),
	REGISTER_OOVPA(D3DResource_IsBusy, 4039, PATCH),
	REGISTER_OOVPA(D3DResource_GetType, 4034, DISABLED), // I forget why I had this in here...
	REGISTER_OOVPA(Get2DSurfaceDesc, 5028, PATCH),
	REGISTER_OOVPA(D3DSurface_GetDesc, 3911, PATCH),
	REGISTER_OOVPA(D3DSurface_LockRect, 3911, PATCH),
	REGISTER_OOVPA(D3DBaseTexture_GetLevelCount, 3911, PATCH),
	REGISTER_OOVPA(D3DTexture_GetSurfaceLevel2, 4627, PATCH),
	REGISTER_OOVPA(D3DTexture_LockRect, 3911, PATCH),
	REGISTER_OOVPA(D3DVolumeTexture_LockBox, 3911, PATCH),	// Just calls Lock3DSurface
	REGISTER_OOVPA(D3DDevice_GetViewportOffsetAndScale, 4432, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteVertexShader, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SelectVertexShaderDirect, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_GetShaderConstantMode, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShader, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderConstant, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderInputDirect, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderInput, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_RunVertexStateShader, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_LoadVertexShader, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_LoadVertexShaderProgram, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SelectVertexShader, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderSize, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderType, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderDeclaration, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderFunction, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderInput, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_Release, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilFail, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_Reset, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScissors, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPalette, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetBackBufferScale, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShaderProgram, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDisplayFieldStatus, 4432, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScreenSpaceOffset, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_InsertCallback, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_GetProjectionViewportMatrix, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDeviceCaps, 3911, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMask, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LogicOp, 4627, PATCH),
	REGISTER_OOVPA(D3D_SetFence, 5028, XREF),
	REGISTER_OOVPA(D3DDevice_InsertFence, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_IsFencePending, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_KickOff, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TwoSidedLighting, 4134, PATCH), // Beware of the typo...
	REGISTER_OOVPA(D3DDevice_GetTexture2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetStateVB, 4627, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetStateUP, 4627, DISABLED),
	REGISTER_OOVPA(D3DDevice_DrawIndexedVerticesUP, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetStipple, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZCmpAlwaysRead, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZRead, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_DoNotCullUncompressed, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_PersistDisplay, 4831, PATCH),
	REGISTER_OOVPA(D3DDevice_SetSwapCallback, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FrontFace, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMode, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilCullEnable, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4ub, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4f, 4039, PATCH),
	REGISTER_OOVPA(D3D_BlockOnTime, 5028, XREF),
	REGISTER_OOVPA(D3DDevice_BlockOnFence, 3911, PATCH),
	REGISTER_OOVPA(Lock3DSurface, 3911, PATCH),
	REGISTER_OOVPA(Lock2DSurface, 3911, PATCH),
	REGISTER_OOVPA(D3DCubeTexture_GetCubeMapSurface2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetSoftDisplayFilter, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetFlickerFilter, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetPixelShader, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_CreatePalette2, 4627, PATCH),
	REGISTER_OOVPA(D3DPalette_Lock2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetModelView, 4627, PATCH),
	REGISTER_OOVPA(D3D_LazySetPointParams, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetDepthClipPlanes, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_KickPushBuffer, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetMaterial, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetMaterial, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetBackMaterial, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetBackMaterial, 4627, PATCH),
	REGISTER_OOVPA(D3D_SetPushBufferSize, 3911, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetRenderState_SampleAlpha, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_PSTextureModes, 4034, PATCH),
	REGISTER_OOVPA(D3D_GetDeviceCaps, 3911, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetVertexDataColor, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_GetModelView, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_FlushVertexCache, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData2s, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4s, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_PrimeVertexCache, 4627, PATCH),
	REGISTER_OOVPA(D3DVertexBuffer_GetDesc, 3911, DISABLED),
	REGISTER_OOVPA(D3D_BlockOnResource, 4034, XREF),
	REGISTER_OOVPA(D3DResource_BlockUntilNotBusy, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetScissors, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_IsBusy, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant, 4627, PATCH),
	REGISTER_OOVPA(D3DCubeTexture_LockRect, 3911, PATCH),

	// ******************************************************************
	// Provisional registration functions in XDK 5028
	// TODO: Need test cases
	// ******************************************************************
	REGISTER_OOVPA(D3DDevice_CreateVolumeTexture, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateCubeTexture, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawRectPatch, 3911, PATCH),
	REGISTER_OOVPA(D3D_ClearStateBlockFlags, 3911, XREF),
	REGISTER_OOVPA(D3DDevice_BeginStateBlock, 4134, PATCH),
	REGISTER_OOVPA(D3D_RecordStateBlock, 3911, XREF),
	REGISTER_OOVPA(D3DDevice_EndStateBlock, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_ApplyStateBlock, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateStateBlock, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_CaptureStateBlock, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteStateBlock, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginPushBuffer, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_EndPushBuffer, 4134, PATCH),
	// REGISTER_OOVPA(D3DDevice_RunPushBuffer, 5120, PATCH),
	REGISTER_OOVPA(D3DDevice_GetPushBufferOffset, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateImageSurface, 4627, PATCH),
	// ******************************************************************
};

// ******************************************************************
// * D3D8_5028_SIZE
// ******************************************************************
uint32 D3D8_5028_SIZE = sizeof(D3D8_5028);
