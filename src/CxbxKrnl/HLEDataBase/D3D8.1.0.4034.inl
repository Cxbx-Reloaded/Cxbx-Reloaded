// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->D3D8.1.0.4034.cpp
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
OOVPA_NO_XREF(CMiniport_InitHardware, 4034, 24)
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
		{ 0x1F, 0xD8 },
OOVPA_END;

// ******************************************************************
// * CMiniport::CreateCtxDmaObject
// ******************************************************************
OOVPA_NO_XREF(CMiniport_CreateCtxDmaObject, 4034, 32) // Also for 4361, 4627, 5344, 5558, 5659, 5788, 5849, 5933
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

// ******************************************************************
// * D3DDevice_GetGammaRamp
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetGammaRamp, 4034, 7)
        { 0x05, 0x8B },
        { 0x0A, 0x00 },
        { 0x10, 0xE1 },
        { 0x16, 0x0C },
        { 0x1C, 0x00 },
        { 0x22, 0x00 },
        { 0x28, 0x04 },
OOVPA_END;

#if 0 // Moved to 3911
// ******************************************************************
// * D3DDevice_GetCreationParameters
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetCreationParameters, 4034, 7)

        { 0x06, 0x8B },
        { 0x0E, 0x8B },
        { 0x16, 0x4E },
        { 0x1E, 0x83 },
        { 0x26, 0x00 },
        { 0x2E, 0x5E },
        { 0x36, 0xC9 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_GetViewport
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetViewport, 4034, 10)

        // D3DDevice_GetViewport+0x05 : push esi; push edi
        { 0x05, 0x56 }, // (Offset,Value)-Pair #1
        { 0x06, 0x57 }, // (Offset,Value)-Pair #2

        // D3DDevice_GetViewport+0x0B : lea esi, [eax+0x0B50]
        { 0x0B, 0x8D }, // (Offset,Value)-Pair #3
        { 0x0C, 0xB0 }, // (Offset,Value)-Pair #4
        { 0x0D, 0x50 }, // (Offset,Value)-Pair #5
        { 0x0E, 0x0B }, // (Offset,Value)-Pair #6

        // D3DDevice_GetViewport+0x11 : mov ecx, 6
        { 0x11, 0xB9 }, // (Offset,Value)-Pair #7
        { 0x12, 0x06 }, // (Offset,Value)-Pair #8

        // D3DDevice_GetViewport+0x1A : retn 0x04
        { 0x1A, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x1B, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetViewport
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetViewport, 4034, 10)

        // D3DDevice_SetViewport+0x00 : sub esp, 0x08
        { 0x00, 0x83 }, // (Offset,Value)-Pair #1
        { 0x01, 0xEC }, // (Offset,Value)-Pair #2
        { 0x02, 0x08 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetViewport+0x2D : mov ebx, [esp+0x14]
        { 0x2D, 0x8B }, // (Offset,Value)-Pair #4
        { 0x2E, 0x5C }, // (Offset,Value)-Pair #5
        { 0x2F, 0x24 }, // (Offset,Value)-Pair #6
        { 0x30, 0x14 }, // (Offset,Value)-Pair #7

        // D3DDevice_SetViewport+0x53 : jnz +0x12
        { 0x53, 0x75 }, // (Offset,Value)-Pair #8
        { 0x54, 0x12 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetViewport+0x9B : inc edx
        { 0x9B, 0x42 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTransform
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTransform, 4034, 10)

        // D3DDevice_SetTransform+0x44 : fld dword ptr [edx+0x28]
        { 0x44, 0xD9 }, // (Offset,Value)-Pair #1
        { 0x45, 0x42 }, // (Offset,Value)-Pair #2
        { 0x46, 0x28 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetTransform+0x51 : test ah, 0x44
        { 0x51, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x52, 0xC4 }, // (Offset,Value)-Pair #5
        { 0x53, 0x44 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetTransform+0x5F : fnstsw ax
        { 0x5F, 0xDF }, // (Offset,Value)-Pair #7
        { 0x60, 0xE0 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetTransform+0x84 : fdivp st(1), st
        { 0x84, 0xDE }, // (Offset,Value)-Pair #9
        { 0x85, 0xF9 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_CullMode
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_CullMode, 4034, 2+14,

    XREF_SETRENDERSTATE_CULLMODE,
	XRefTwo)

		XREF_ENTRY(0x03, XREF_D3DDEVICE), // Derived
		XREF_ENTRY(0x2B, XREF_D3DRS_CULLMODE), // Derived

        // D3DDevice_SetRenderState_CullMode+0x00 : push esi
        { 0x00, 0x56 }, // (Offset,Value)-Pair #1

        // D3DDevice_SetRenderState_CullMode+0x19 : mov dword ptr [eax], 0x40308
        { 0x19, 0xC7 }, // (Offset,Value)-Pair #2
        { 0x1B, 0x08 }, // (Offset,Value)-Pair #3
        { 0x1C, 0x03 }, // (Offset,Value)-Pair #4
        { 0x1D, 0x04 }, // (Offset,Value)-Pair #5

        // D3DDevice_SetRenderState_CullMode+0x24 : add eax, 8
        { 0x24, 0x83 }, // (Offset,Value)-Pair #6
        { 0x25, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x26, 0x08 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetRenderState_CullMode+0x30 : retn 4
        { 0x30, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x31, 0x04 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetRenderState_CullMode+0x53 : add edx, 0x404
        { 0x53, 0x81 }, // (Offset,Value)-Pair #11
        { 0x54, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x55, 0x04 }, // (Offset,Value)-Pair #13
        { 0x56, 0x04 }, // (Offset,Value)-Pair #14
OOVPA_END;

// ******************************************************************
// * D3DDevice_BeginVisibilityTest
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginVisibilityTest, 4034, 7)

        { 0x07, 0x8B },
        { 0x0A, 0x46 },
        { 0x13, 0xC7 },
        { 0x16, 0x17 },
        { 0x1C, 0x00 },
        { 0x22, 0x48 },
        { 0x28, 0x06 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_EndVisibilityTest
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_EndVisibilityTest, 4034, 7)

        { 0x0B, 0x8B },
        { 0x16, 0x5E },
        { 0x22, 0x07 },
        { 0x2E, 0x00 },
        { 0x3A, 0x81 },
        { 0x46, 0x89 },
        { 0x55, 0x5F },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CopyRects
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CopyRects, 4034, 8)

        { 0x1E, 0x57 },
        { 0x3E, 0xF6 },
        { 0x5E, 0xEB },
        { 0x7E, 0x00 },
        { 0x9E, 0x10 },
        { 0xBE, 0x24 },
        { 0xDE, 0x22 },
        { 0xFE, 0x14 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DeletePatch
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DeletePatch, 4034, 7)

        { 0x05, 0x0D },
        { 0x0C, 0xA1 },
        { 0x13, 0x34 },
        { 0x1A, 0x4E },
        { 0x21, 0x00 },
        { 0x2A, 0x56 },
        { 0x30, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawIndexedVertices
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawIndexedVertices, 4034, 11)

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
// * D3DDevice_SetStreamSource
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetStreamSource, 4034, 14)

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
// * D3DDevice_SetTextureState_BorderColor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTextureState_BorderColor, 4034, 7)

        { 0x07, 0x8B },
        { 0x0E, 0xE8 },
        { 0x16, 0x08 },
        { 0x1E, 0x24 },
        { 0x26, 0x24 },
        { 0x2E, 0x89 },
        { 0x39, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_ColorKeyColor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTextureState_ColorKeyColor, 4034, 7)

        { 0x07, 0x8B },
        { 0x0E, 0xE8 },
        { 0x16, 0x08 },
        { 0x1E, 0x89 },
        { 0x26, 0x04 },
        { 0x2E, 0x07 },
        { 0x36, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawVertices
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawVertices, 4034, 12)

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
// * D3DDevice_LoadVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_LoadVertexShader, 4034, 10)

        // D3DDevice_LoadVertexShader+0x07 : mov al, [ebx+0x08]
        { 0x07, 0x8A }, // (Offset,Value)-Pair #1
        { 0x08, 0x43 }, // (Offset,Value)-Pair #2
        { 0x09, 0x08 }, // (Offset,Value)-Pair #3

        // D3DDevice_LoadVertexShader+0x32 : mov dword ptr [eax], 0x00041E9C
        { 0x32, 0xC7 }, // (Offset,Value)-Pair #4
        { 0x33, 0x00 }, // (Offset,Value)-Pair #5
        { 0x34, 0x9C }, // (Offset,Value)-Pair #6
        { 0x35, 0x1E }, // (Offset,Value)-Pair #7
        { 0x36, 0x04 }, // (Offset,Value)-Pair #8

        // D3DDevice_LoadVertexShader+0x4E : mov [ebx], ebx
        { 0x4E, 0x89 }, // (Offset,Value)-Pair #9
        { 0x4F, 0x13 }, // (Offset,Value)-Pair #10
OOVPA_END;

#if 0 // obsolete, replaced by generic 3911 version
// ******************************************************************
// * D3DDevice_DrawRectPatch
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawRectPatch, 4034, 8)

        { 0x1E, 0x6C },
        { 0x3E, 0x3C },
        { 0x5E, 0x54 },
        { 0x7F, 0x89 },
        { 0x9E, 0x00 },
        { 0xBE, 0xD9 },
        { 0xDE, 0xD9 },
        { 0xFE, 0x86 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_SetIndices
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetIndices, 4034, 11)

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

        // D3DDevice_SetIndices+0x76 : mov [esi+0x0504], edi
        { 0x76, 0x89 }, // (Offset,Value)-Pair #8
        { 0x77, 0xBE }, // (Offset,Value)-Pair #9
        { 0x78, 0x04 }, // (Offset,Value)-Pair #10
        { 0x79, 0x05 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_EdgeAntiAlias
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_EdgeAntiAlias, 4034, 7)

        { 0x07, 0x8B },
        { 0x0C, 0x72 },
        { 0x13, 0x8B },
        { 0x1A, 0x03 },
        { 0x21, 0x48 },
        { 0x28, 0x89 },
        { 0x2F, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_FillMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_FillMode, 4034, 7)

        { 0x07, 0x8B },
        { 0x13, 0x8B },
        { 0x19, 0x8B },
        { 0x22, 0x4C },
        { 0x2B, 0x8C },
        { 0x34, 0x08 },
        { 0x40, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_BackFillMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_BackFillMode, 4034, 8)

        { 0x00, 0x8B },
        { 0x0B, 0xA3 },
        { 0x14, 0x04 },
        { 0x23, 0x8B },
        { 0x2A, 0xC9 },
        { 0x35, 0xC7 },
        { 0x40, 0x08 },
        { 0x4C, 0x5F },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_FogColor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_FogColor, 4034, 7)

        { 0x08, 0x06 },
        { 0x14, 0x8B },
        { 0x1C, 0x0E },
        { 0x26, 0xE7 },
        { 0x30, 0x00 },
        { 0x3A, 0x89 },
        { 0x44, 0x0D },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_FrontFace
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_FrontFace, 4034, 6)

        { 0x07, 0x8B },
        { 0x0E, 0xE8 },
        { 0x16, 0x08 },
        { 0x1E, 0x48 },
        { 0x2A, 0x89 },
        { 0x30, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_LogicOp
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_LogicOp, 4034, 7)

        { 0x09, 0x3B },
        { 0x14, 0x4C },
        { 0x1F, 0x04 },
        { 0x2A, 0x0D },
        { 0x35, 0xBC },
        { 0x40, 0x89 },
        { 0x4E, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleAntiAlias
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleAntiAlias, 4034, 7)

        { 0x0B, 0x8B },
        { 0x17, 0x8B },
        { 0x23, 0x8B },
        { 0x2C, 0x8B },
        { 0x38, 0xC1 },
        { 0x40, 0x7C },
        { 0x4B, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMask
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleMask, 4034, 7)

        { 0x0B, 0x8B },
        { 0x17, 0x8B },
        { 0x23, 0x8B },
        { 0x2C, 0x8B },
        { 0x38, 0x83 },
        { 0x40, 0x7C },
        { 0x4B, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_NormalizeNormals
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_NormalizeNormals, 4034, 7)

        { 0x07, 0x8B },
        { 0x0E, 0xE8 },
        { 0x16, 0x08 },
        { 0x1E, 0x48 },
        { 0x26, 0x0D },
        { 0x2E, 0x00 },
        { 0x36, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_ShadowFunc
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_ShadowFunc, 4034, 7)

        { 0x07, 0x8B },
        { 0x0C, 0x72 },
        { 0x13, 0x8B },
        { 0x1A, 0x1E },
        { 0x21, 0xFF },
        { 0x28, 0x08 },
        { 0x31, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_Simple
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_Simple, 4034, 13)

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
// * D3DDevice_SetRenderState_StencilEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilEnable, 4034, 7)

        { 0x13, 0x8B },
        { 0x20, 0x02 },
        { 0x34, 0x85 },
        { 0x42, 0x75 },
        { 0x53, 0x89 },
        { 0x64, 0x00 },
        { 0x75, 0x83 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_StencilFail
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilFail, 4034, 7)

        { 0x0C, 0x72 },
        { 0x1A, 0xC9 },
        { 0x2A, 0x85 },
        { 0x36, 0x74 },
        { 0x44, 0x83 },
        { 0x52, 0x24 },
        { 0x60, 0x10 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_YuvEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_YuvEnable, 4034, 7)

        { 0x04, 0x56 },
        { 0x0B, 0xA3 },
        { 0x10, 0x8B },
        { 0x16, 0x05 },
        { 0x1C, 0x8B },
        { 0x25, 0x89 },
        { 0x28, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_TwoSidedLighting
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_TwoSidedLighting, 4034, 7)

        { 0x0B, 0x8B },
        { 0x18, 0x15 },
        { 0x25, 0x46 },
        { 0x35, 0x8B },
        { 0x43, 0x75 },
        { 0x4C, 0x00 },
        { 0x59, 0x3D },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_VertexBlend
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_VertexBlend, 4034, 7)

        { 0x07, 0xCA },
        { 0x13, 0x89 },
        { 0x19, 0x8B },
        { 0x25, 0x8B },
        { 0x2B, 0x28 },
        { 0x34, 0x08 },
        { 0x3D, 0x5E },
OOVPA_END;
#if 0 // Moved to 3911
// ******************************************************************
// * D3D_CheckDeviceFormat
// ******************************************************************
OOVPA_NO_XREF(D3D_CheckDeviceFormat, 4034, 7)

        { 0x0E, 0x18 },
        { 0x1E, 0x00 },
        { 0x2E, 0x42 },
        { 0x41, 0x40 },
        { 0x4E, 0x74 },
        { 0x5E, 0x08 },
        { 0x6E, 0x3C },
OOVPA_END;
#endif
// ******************************************************************
// * Get2DSurfaceDesc
// ******************************************************************
/*OOVPA_NO_XREF(Get2DSurfaceDesc, 4034, 10)

        // Get2DSurfaceDesc+0x2B : movzx edx, byte ptr [edi+0x0D]
        { 0x2B, 0x0F }, // (Offset,Value)-Pair #1
        { 0x2C, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x2D, 0x57 }, // (Offset,Value)-Pair #3
        { 0x2E, 0x0D }, // (Offset,Value)-Pair #4

        // Get2DSurfaceDesc+0x62 : mov eax, [eax+0x21DC]
        { 0x56, 0x8B }, // (Offset,Value)-Pair #5
        { 0x57, 0x80 }, // (Offset,Value)-Pair #6
        { 0x58, 0xDC }, // (Offset,Value)-Pair #7
        { 0x59, 0x21 }, // (Offset,Value)-Pair #8

        // Get2DSurfaceDesc+0xAE : retn 0x0C
        { 0xAE, 0xC2 }, // (Offset,Value)-Pair #9
        { 0xAF, 0x0C }, // (Offset,Value)-Pair #10
    }
OOVPA_END;
*/

// ******************************************************************
// * D3DDevice_SetRenderState_ZEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_ZEnable, 4034, 13)

        // D3DDevice_SetRenderState_ZEnable+0x0C : jb +0x05
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #1
        { 0x0D, 0x05 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_ZEnable+0x1B : mov edx, [esi+0x2074h]
        { 0x1B, 0x8B }, // (Offset,Value)-Pair #3
        { 0x1C, 0x96 }, // (Offset,Value)-Pair #4
        { 0x1D, 0xF4 }, // (Offset,Value)-Pair #5
        { 0x1E, 0x21 }, // (Offset,Value)-Pair #6

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

#if 0 // Used 3911
// ******************************************************************
// * D3DDevice_LightEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_LightEnable, 4034, 13)

        // D3DDevice_LightEnable+0x0F : cmp ebx, [esi+0x050C]
        { 0x0F, 0x3B }, // (Offset,Value)-Pair #1
        { 0x10, 0x9E }, // (Offset,Value)-Pair #2
        { 0x11, 0x0C }, // (Offset,Value)-Pair #3
        { 0x12, 0x05 }, // (Offset,Value)-Pair #4

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
#endif

// ******************************************************************
// * D3DDevice_SetRenderState_TextureFactor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_TextureFactor, 4034, 9)

        // D3DDevice_SetRenderState_TextureFactor+0x07 : mov eax, [esi+0x04E8]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x86 }, // (Offset,Value)-Pair #2
        { 0x09, 0xE8 }, // (Offset,Value)-Pair #3
        { 0x0A, 0x04 }, // (Offset,Value)-Pair #4

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
// * Get2DSurfaceDesc
// ******************************************************************
OOVPA_NO_XREF(Get2DSurfaceDesc, 4034, 10)

        // Get2DSurfaceDesc+0x2B : movzx edx, byte ptr [edi+0x0D]
        { 0x2B, 0x0F }, // (Offset,Value)-Pair #1
        { 0x2C, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x2D, 0x57 }, // (Offset,Value)-Pair #3
        { 0x2E, 0x0D }, // (Offset,Value)-Pair #4

        // Get2DSurfaceDesc+0x52 : mov edx, [eax+0x21FC]
        { 0x52, 0x8B }, // (Offset,Value)-Pair #5
        { 0x53, 0x90 }, // (Offset,Value)-Pair #6
        { 0x54, 0xFC }, // (Offset,Value)-Pair #7
        { 0x55, 0x21 }, // (Offset,Value)-Pair #8

        // Get2DSurfaceDesc+0xAE : retn 0x0C
        { 0xAE, 0xC2 }, // (Offset,Value)-Pair #9
        { 0xAF, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_Clear
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Clear, 4034, 9)
        // D3DDevice_Clear+0x0000 : sub esp, 38h
        { 0x0000, 0x83 }, // (Offset,Value)-Pair #1
        { 0x0001, 0xEC }, // (Offset,Value)-Pair #2
        { 0x0002, 0x38 }, // (Offset,Value)-Pair #3

        // D3DDevice_Clear+0x000B : mov ebx, [ebp+0x21F4]
        { 0x000B, 0x8B }, // (Offset,Value)-Pair #4
        { 0x000C, 0x9D }, // (Offset,Value)-Pair #5
        { 0x000D, 0xF4 }, // (Offset,Value)-Pair #6
        { 0x000E, 0x21 }, // (Offset,Value)-Pair #7

        // D3DDevice_Clear+0x0399 : retn 0x18
        { 0x0399, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x039A, 0x18 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetLight
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetLight, 4034, 12)

        // D3DDevice_SetLight+0x11 : mov edi, [ebp+0x50C]
        { 0x11, 0x8B }, // (Offset,Value)-Pair #1
        { 0x12, 0xBD }, // (Offset,Value)-Pair #2
        { 0x13, 0x0C }, // (Offset,Value)-Pair #3
        { 0x14, 0x05 }, // (Offset,Value)-Pair #4

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
// * D3DVertexBuffer_Lock
// ******************************************************************
OOVPA_NO_XREF(D3DVertexBuffer_Lock, 4034, 11)

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
// * D3DDevice_SetTexture
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTexture, 4034, 10) // TODO : Alias 3911 instead ?

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
// * D3DDevice_SetPalette
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPalette, 4034, 12)

        // D3DDevice_SetPalette+0x0D : mov eax, [edi+esi*4+0x0C08]
        { 0x0D, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0E, 0x84 }, // (Offset,Value)-Pair #2
        { 0x0F, 0xBE }, // (Offset,Value)-Pair #3
        { 0x10, 0x08 }, // (Offset,Value)-Pair #4
        { 0x11, 0x0C }, // (Offset,Value)-Pair #5

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
// * D3DDevice_SetTextureState_TexCoordIndex
// ******************************************************************
OOVPA_XREF(D3DDevice_SetTextureState_TexCoordIndex, 4034, 1+10,
	
	XRefNoSaveIndex,
	XRefOne)

		XREF_ENTRY(0x11, XREF_D3DTSS_TEXCOORDINDEX), // Derived - TODO : Verify 0x11 (could be 0x18 or 0x19)

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

        // D3DDevice_SetTextureState_TexCoordIndex+0xB4 : shl edx, 0x04
        { 0xB4, 0xC1 }, // (Offset,Value)-Pair #8
        { 0xB5, 0xE2 }, // (Offset,Value)-Pair #9
        { 0xB6, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetMaterial, 4034, 12)

        // D3DDevice_SetMaterial+0x0C : add edi, 0x0B70
        { 0x0C, 0x81 }, // (Offset,Value)-Pair #1
        { 0x0D, 0xC7 }, // (Offset,Value)-Pair #2
        { 0x0E, 0x70 }, // (Offset,Value)-Pair #3
        { 0x0F, 0x0B }, // (Offset,Value)-Pair #4

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
// * D3DDevice_SetVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShader, 4034, 14)

        // D3DDevice_SetVertexShader+0x06 : test bl, 1
        { 0x06, 0xF6 }, // (Offset,Value)-Pair #1
        { 0x07, 0xC3 }, // (Offset,Value)-Pair #2
        { 0x08, 0x01 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetVertexShader+0x11 : mov eax, [esi+0x04F8]
        { 0x11, 0x8B }, // (Offset,Value)-Pair #4
        { 0x12, 0x86 }, // (Offset,Value)-Pair #5
        { 0x13, 0xF8 }, // (Offset,Value)-Pair #6
        { 0x14, 0x04 }, // (Offset,Value)-Pair #7

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
// * D3DDevice_Swap
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Swap, 4034, 11)

        // D3DDevice_Swap+0x10 : mov ebx, 5
        { 0x10, 0xBB }, // (Offset,Value)-Pair #1
        { 0x11, 0x05 }, // (Offset,Value)-Pair #2

        // D3DDevice_Swap+0x1D : test bl, 3
        { 0x1D, 0xF6 }, // (Offset,Value)-Pair #3
        { 0x1E, 0xC3 }, // (Offset,Value)-Pair #4
        { 0x1F, 0x03 }, // (Offset,Value)-Pair #5

        // D3DDevice_Swap+0x30 : mov [esi+0x2C38], eax
        { 0x30, 0x89 }, // (Offset,Value)-Pair #6
        { 0x31, 0x86 }, // (Offset,Value)-Pair #7
        { 0x32, 0x38 }, // (Offset,Value)-Pair #8
        { 0x33, 0x2C }, // (Offset,Value)-Pair #9

        // D3DDevice_Swap+0xB0 : retn 4
        { 0xB0, 0xC2 }, // (Offset,Value)-Pair #10
        { 0xB1, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3D_KickOffAndWaitForIdle
// ******************************************************************
OOVPA_NO_XREF(D3D_KickOffAndWaitForIdle, 4034, 9)

        // D3D_KickOffAndWaitForIdle+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // D3D_KickOffAndWaitForIdle+0x05 : mov ecx, [eax+0x30]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x48 }, // (Offset,Value)-Pair #3
        { 0x07, 0x30 }, // (Offset,Value)-Pair #4

        // D3D_KickOffAndWaitForIdle+0x08 : push 0
        { 0x08, 0x6A }, // (Offset,Value)-Pair #5
        { 0x09, 0x00 }, // (Offset,Value)-Pair #6

        // D3D_KickOffAndWaitForIdle+0x0A : push ecx
        { 0x0A, 0x51 }, // (Offset,Value)-Pair #7

        // D3D_KickOffAndWaitForIdle+0x0B : call [addr]
        { 0x0B, 0xE8 }, // (Offset,Value)-Pair #8

        // D3D_KickOffAndWaitForIdle+0x10 : retn
        { 0x10, 0xC3 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateImageSurface
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateImageSurface, 4034, 9)

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
// * D3DDevice_GetBackBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetBackBuffer, 4034, 12)

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

        // D3DDevice_GetBackBuffer+0x1D : mov eax, [ecx+eax*4+0x21FC]
        { 0x1D, 0x8B }, // (Offset,Value)-Pair #8
        { 0x1E, 0x84 }, // (Offset,Value)-Pair #9
        { 0x1F, 0x81 }, // (Offset,Value)-Pair #10
        { 0x20, 0xFC }, // (Offset,Value)-Pair #11
        { 0x21, 0x21 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShaderConstant
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShaderConstant, 4034, 10)

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
// * D3DDevice_GetTransform
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetTransform, 4039, 12)

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
// * D3DDevice_SelectVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SelectVertexShader, 4039, 9)

        { 0x04, 0x85 },
        { 0x05, 0xC0 },
        { 0x06, 0x56 },
        { 0x07, 0x8B },
        { 0x3B, 0x35 },
        { 0x3C, 0x00 },
        { 0x3D, 0x94 },
        { 0x3E, 0x1E },
        { 0x3F, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BlockUntilVerticalBlank
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BlockUntilVerticalBlank, 4039, 9)

        { 0x05, 0x6A },
        { 0x06, 0x00 },
        { 0x07, 0x6A },
        { 0x08, 0x00 },
        { 0x09, 0x6A },
        { 0x0A, 0x01 },
        { 0x10, 0x00 },
        { 0x17, 0x05 },
        { 0x1E, 0x15 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderTarget
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderTarget, 4039, 9)

        { 0x06, 0x35 },
        { 0x12, 0x3B },
        { 0x13, 0xFB },
        { 0x14, 0x75 },
        { 0x15, 0x0C },
        { 0x16, 0x8B },
        { 0x1A, 0x00 },
        { 0x1F, 0x4C },
        { 0x32, 0x74 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPixelShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPixelShader, 4039, 9)

        { 0x04, 0x85 },
        { 0x05, 0xC0 },
        { 0x06, 0x53 },
        { 0x07, 0x8B },
        { 0x08, 0x1D },
        { 0x09, 0xA0 },
        { 0x18, 0x00 },
        { 0x1D, 0x0D },
        { 0x3F, 0xA3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Begin
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Begin, 4039, 13)

        { 0x07, 0xE8 },

        { 0x18, 0x8B },
        { 0x19, 0x4C },
        { 0x1A, 0x24 },
        { 0x1B, 0x08 },
        { 0x1C, 0xC7 },
        { 0x1D, 0x00 },
        { 0x1E, 0xFC },
        { 0x1F, 0x17 },
        { 0x20, 0x04 },
        { 0x21, 0x00 },

        { 0x32, 0xC2 },
        { 0x33, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetShaderConstantMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetShaderConstantMode, 4039, 9)

        { 0x08, 0x1D },
        { 0x1A, 0x81 },
        { 0x1B, 0xE1 },
        { 0x1C, 0xFF },
        { 0x1D, 0xFD },
        { 0x1E, 0xFF },
        { 0x1F, 0xFF },
        { 0x38, 0x00 },
        { 0x43, 0xE8 },
OOVPA_END;

// ******************************************************************
// * D3D::BlockOnResource
// ******************************************************************
OOVPA_XREF(D3D_BlockOnResource, 4034, 12,

    XREF_D3D_BlockOnResource,
    XRefZero)

        { 0x00, 0xA1 },
        { 0x05, 0x85 },
        { 0x18, 0x00 },
        { 0x19, 0x05 },
        { 0x1A, 0x00 },
        { 0x1B, 0x56 },
        { 0x1C, 0x75 },
        { 0x1D, 0x28 },
        { 0x1E, 0x8B },
        { 0x1F, 0x70 },
        { 0x40, 0x5E },
        { 0x4A, 0xE8 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVerticalBlankCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVerticalBlankCallback, 4039, 12)

        // D3DDevice_SetVerticalBlankCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x04 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetVerticalBlankCallback+0x0A : mov [ecx+0x25AC], eax
        { 0x0A, 0x89 }, // (Offset,Value)-Pair #5
        { 0x0B, 0x81 }, // (Offset,Value)-Pair #6
        { 0x0C, 0xAC }, // (Offset,Value)-Pair #7
        { 0x0D, 0x25 }, // (Offset,Value)-Pair #8
        { 0x0E, 0x00 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetVerticalBlankCallback+0x10 : retn 0x04
        { 0x10, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x11, 0x04 }, // (Offset,Value)-Pair #11
        { 0x12, 0x00 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData2f
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData2f, 4039, 14)

        { 0x00, 0x56 },
        { 0x01, 0x8B },
        { 0x02, 0x35 },
        { 0x07, 0x8B },
        { 0x18, 0x14 },
        { 0x19, 0xCD },
        { 0x1A, 0x80 },
        { 0x1B, 0x18 },
        { 0x1C, 0x08 },
        { 0x1D, 0x00 },
        { 0x1E, 0x8B },
        { 0x1F, 0x4C },
        { 0x34, 0xC2 },
        { 0x35, 0x0C },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData4f
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData4f, 4039, 11)

        { 0x00, 0x56 },
        { 0x07, 0x8B },
        { 0x18, 0xF9 },
        { 0x19, 0xFF },
        { 0x1A, 0x75 },
        { 0x1B, 0x07 },
        { 0x1C, 0xB9 },
        { 0x1D, 0x18 },
        { 0x1E, 0x15 },
        { 0x1F, 0x00 },
        { 0x56, 0xC2 },
/*
	{ 0x0A, 0x46 },
	{ 0x16, 0x08 },
	{ 0x22, 0x09 },
	{ 0x2E, 0x24 },
	{ 0x3A, 0x24 },
	{ 0x46, 0x8B },
	{ 0x52, 0x14 },*/
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexDataColor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexDataColor, 4039, 13)

        { 0x02, 0x35 },

        { 0x14, 0x8B },
        { 0x15, 0x4C },
        { 0x16, 0x24 },
        { 0x17, 0x0C },
        { 0x18, 0x8D },
        { 0x19, 0x14 },
        { 0x1A, 0x8D },
        { 0x1B, 0x40 },
        { 0x1C, 0x19 },
        { 0x1D, 0x04 },
        { 0x1E, 0x00 },
        { 0x25, 0x0F },
OOVPA_END;

// ******************************************************************
// * D3DDevice_End
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_End, 4039, 7)
	{ 0x07, 0x8B },
	{ 0x0E, 0xE8 },
	{ 0x16, 0x17 },
	{ 0x1E, 0x00 },
	{ 0x26, 0x46 },
	{ 0x2E, 0x01 },
	{ 0x36, 0x08 },
OOVPA_END;


// ******************************************************************
// * D3DDevice_SetFlickerFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetFlickerFilter, 4034, 12)

        // D3DDevice_SetFlickerFilter+0x0A : mov edx, [ecx+0x23E8]
        { 0x0A, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0B, 0x91 }, // (Offset,Value)-Pair #2
        { 0x0C, 0xE8 }, // (Offset,Value)-Pair #3
        { 0x0D, 0x23 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetFlickerFilter+0x10 : push 0; push eax; push 0x0B; push edx
        { 0x10, 0x6A }, // (Offset,Value)-Pair #5
        { 0x11, 0x00 }, // (Offset,Value)-Pair #6
        { 0x12, 0x50 }, // (Offset,Value)-Pair #7
        { 0x13, 0x6A }, // (Offset,Value)-Pair #8
        { 0x14, 0x0B }, // (Offset,Value)-Pair #9
        { 0x15, 0x52 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetFlickerFilter+0x1C : retn 0x08
        { 0x1C, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x1D, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetSoftDisplayFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetSoftDisplayFilter, 4034, 12)

        // D3DDevice_SetSoftDisplayFilter+0x0A : mov edx, [ecx+0x23E8]
        { 0x0A, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0B, 0x91 }, // (Offset,Value)-Pair #2
        { 0x0C, 0xE8 }, // (Offset,Value)-Pair #3
        { 0x0D, 0x23 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetSoftDisplayFilter+0x10 : push 0; push eax; push 0x0E; push edx
        { 0x10, 0x6A }, // (Offset,Value)-Pair #5
        { 0x11, 0x00 }, // (Offset,Value)-Pair #6
        { 0x12, 0x50 }, // (Offset,Value)-Pair #7
        { 0x13, 0x6A }, // (Offset,Value)-Pair #8
        { 0x14, 0x0E }, // (Offset,Value)-Pair #9
        { 0x15, 0x52 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetSoftDisplayFilter+0x1C : retn 0x08
        { 0x1C, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x1D, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_PSTextureModes
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_PSTextureModes, 4034, 10)
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
// * D3DDevice_SetRenderState_Dxt1NoiseEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_Dxt1NoiseEnable, 4034, 11)

        { 0x01, 0x8B },

        { 0x10, 0x00 },
        { 0x11, 0x00 },
        { 0x12, 0x0F },
        { 0x13, 0xB6 },
        { 0x14, 0x48 },
        { 0x15, 0x0D },
        { 0x16, 0x8A },
        { 0x17, 0x91 },

        { 0x49, 0xC7 },
        { 0x76, 0x1D },
OOVPA_END;

// ******************************************************************
// * D3DResource_IsBusy
// ******************************************************************
OOVPA_NO_XREF(D3DResource_IsBusy, 4039, 11)

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
// * D3DDevice_SetBackBufferScale
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetBackBufferScale, 4039, 16)

        { 0x09, 0x86 },

        { 0x10, 0xC1 },
        { 0x11, 0xE9 },
        { 0x12, 0x04 },
        { 0x13, 0x83 },
        { 0x14, 0xE1 },
        { 0x15, 0x0F },
        { 0x16, 0x85 },
        { 0x17, 0xC9 },

        { 0x28, 0xD8 },
        { 0x29, 0x4C },
        { 0x2A, 0x24 },
        { 0x2B, 0x0C },
        { 0x2C, 0x83 },
        { 0x2D, 0xE0 },
        { 0x2E, 0x0F },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleMode, 4034, 12)

        // D3DDevice_SetRenderState_MultiSampleMode+0x0F : mov ecx, [eax+0x21F0]
        { 0x0F, 0x8B }, // (Offset,Value)-Pair #1
        { 0x10, 0x88 }, // (Offset,Value)-Pair #2
        { 0x11, 0xF0 }, // (Offset,Value)-Pair #3
        { 0x12, 0x21 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_MultiSampleMode+0x15 : cmp ecx, [eax+0x21FC]
        { 0x15, 0x3B }, // (Offset,Value)-Pair #5
        { 0x16, 0x88 }, // (Offset,Value)-Pair #6
        { 0x17, 0xFC }, // (Offset,Value)-Pair #7
        { 0x18, 0x21 }, // (Offset,Value)-Pair #8

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
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, 4034, 12)

        // D3DDevice_SetRenderState_MultiSampleRenderTargetMode+0x0F : mov ecx, [eax+0x21F0]
        { 0x0F, 0x8B }, // (Offset,Value)-Pair #1
        { 0x10, 0x88 }, // (Offset,Value)-Pair #2
        { 0x11, 0xF0 }, // (Offset,Value)-Pair #3
        { 0x12, 0x21 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_MultiSampleRenderTargetMode+0x15 : cmp ecx, [eax+0x21FC]
        { 0x15, 0x3B }, // (Offset,Value)-Pair #5
        { 0x16, 0x88 }, // (Offset,Value)-Pair #6
        { 0x17, 0xFC }, // (Offset,Value)-Pair #7
        { 0x18, 0x21 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetRenderState_MultiSampleRenderTargetMode+0x1B : jz +0x0D
        { 0x1B, 0x74 }, // (Offset,Value)-Pair #9
        { 0x1C, 0x0D }, // (Offset,Value)-Pair #10

        // D3DDevice_SetRenderState_MultiSampleRenderTargetMode+0x2A : retn 0x04
        { 0x2A, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x2B, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_IsBusy
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_IsBusy, 4034, 7)

        { 0x05, 0x8B },
        { 0x0C, 0x90 },
        { 0x13, 0x44 },
        { 0x1A, 0xFF },
        { 0x21, 0x01 },
        { 0x28, 0x00 },
        { 0x2F, 0xD2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetRenderTarget
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetRenderTarget, 4039, 11)

        // D3DDevice_GetRenderTarget+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // D3DDevice_GetRenderTarget+0x05 : mov eax, [eax + 0x21F0]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x80 }, // (Offset,Value)-Pair #3
        { 0x07, 0xF0 }, // (Offset,Value)-Pair #4
        { 0x08, 0x21 }, // (Offset,Value)-Pair #5
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
// * D3DDevice_GetDisplayFieldStatus
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetDisplayFieldStatus, 4039, 12)

        { 0x00, 0xA1 },

        { 0x09, 0x00 },
        { 0x0A, 0x00 },
        { 0x0B, 0x8B },
        { 0x0C, 0x4C },
        { 0x0D, 0x24 },
        { 0x0E, 0x04 },
        { 0x0F, 0x89 },
        { 0x10, 0x51 },
        { 0x11, 0x04 },
        { 0x12, 0xF7 },
        { 0x1C, 0x74 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_PersistDisplay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_PersistDisplay, 4039, 17)

        { 0x01, 0xEC },
        { 0x02, 0x08 },
        { 0x03, 0x57 },
        { 0x04, 0x8B },
        { 0x05, 0x3D },

        { 0x11, 0xC0 },
        { 0x12, 0x89 },
        { 0x13, 0x44 },
        { 0x14, 0x24 },
        { 0x15, 0x04 },
        { 0x16, 0x74 },
        { 0x17, 0x0F },
        { 0x18, 0x50 },
        { 0x19, 0xFF },
        { 0x1A, 0x15 },

        { 0x1F, 0x6A },

        { 0x3F, 0xAF },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_LineWidth
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_LineWidth, 4034, 14)

        { 0x0C, 0x89 },

        { 0x30, 0x8B },
        { 0x46, 0xE8 },

        { 0x4B, 0xC7 },
        { 0x53, 0x04 },
        { 0x54, 0x83 },
        { 0x55, 0xC0 },
        { 0x56, 0x08 },
        { 0x57, 0x5F },
        { 0x58, 0x89 },
        { 0x59, 0x06 },
        { 0x5A, 0x5E },
        { 0x5B, 0x89 },
        { 0x5C, 0x1D },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_OcclusionCullEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_OcclusionCullEnable, 4034, 12)

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
OOVPA_XREF(D3DDevice_SetRenderState_StencilCullEnable, 4034, 13,

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
// * D3DResource_GetType
// ******************************************************************
OOVPA_NO_XREF(D3DResource_GetType, 4034, 7)

        { 0x10, 0x77 },
        { 0x22, 0x83 },
        { 0x34, 0x00 },
        { 0x46, 0x74 },
        { 0x58, 0x00 },
        { 0x6A, 0x74 },
        { 0x7C, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawVerticesUP
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawVerticesUP, 4039, 13)

        { 0x00, 0x55 },
        { 0x01, 0x8B },
        { 0x02, 0xEC },
        { 0x03, 0x83 },
        { 0x04, 0xEC },
        { 0x05, 0x14 },
        { 0x06, 0x57 },
        { 0x07, 0x8B },
        { 0x08, 0x3D },

        { 0x24, 0x4D },
        { 0x30, 0x97 },

        { 0x62, 0x00 },
        { 0x85, 0xB7 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawIndexedVerticesUP
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawIndexedVerticesUP, 4039, 15)

        { 0x00, 0x55 },
        { 0x01, 0x8B },
        { 0x02, 0xEC },
        { 0x03, 0x83 },
        { 0x04, 0xEC },
        { 0x05, 0x14 },
        { 0x06, 0x53 },
        { 0x07, 0x56 },
        { 0x08, 0x57 },
        { 0x09, 0x8B },
        { 0x0A, 0x3D },

        { 0x25, 0x8B },
        { 0x3D, 0x87 },

        { 0x48, 0x00 },
        { 0x5C, 0x8D },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetDisplayMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetDisplayMode, 4039, 13)

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
// * D3DDevice_SetTextureState_BumpEnv
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTextureState_BumpEnv, 4039, 12)

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
// * D3D::CDevice::KickOff
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_KickOff, 4039, 16)

        { 0x00, 0x51 },
        { 0x0E, 0x04 },
        { 0x0F, 0x74 },
        { 0x10, 0x08 },
        { 0x11, 0x8B },
        { 0x12, 0x89 },
        { 0x1B, 0xA1 },

        { 0x80, 0x08 },
        { 0x81, 0x00 },
        { 0x82, 0x20 },
        { 0x83, 0x00 },
        { 0x84, 0x00 },
        { 0x85, 0x59 },
        { 0x86, 0xC3 },
        { 0x87, 0xB8 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetScissors
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetScissors, 4039, 10)

        // D3DDevice_SetScissors+0x0E : mov ebx, [esp+28h+arg_0]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #1
        { 0x05, 0x5C }, // (Offset,Value)-Pair #2
        { 0x06, 0x24 }, // (Offset,Value)-Pair #3
        { 0x07, 0x2C }, // (Offset,Value)-Pair #4

        // D3DDevice_SetScissors+0x0E : test ebx, ebx
        { 0x08, 0x85 }, // (Offset,Value)-Pair #5
        { 0x09, 0xDB }, // (Offset,Value)-Pair #6

        { 0x0A, 0x55 }, // (Offset,Value)-Pair #7
        { 0x0B, 0x8B }, // (Offset,Value)-Pair #8

        // D3DDevice_SetScissors+0x0E : mov edx, [ebp+XXXh]
        { 0x1B, 0x8B }, // (Offset,Value)-Pair #9
        { 0x1C, 0x95 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPixelShaderConstant
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPixelShaderConstant, 4039, 10)

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
// * D3D::SetFence
// ******************************************************************
OOVPA_XREF(D3D_SetFence, 4039, 16,

    XREF_D3D_SETFENCE,
    XRefZero)

        { 0x02, 0x35 },

        { 0x07, 0x8B },
        { 0x0D, 0x72 },

        { 0x1E, 0xC9 },
        { 0x1F, 0x89 },
        { 0x20, 0x48 },
        { 0x21, 0x0C },
        { 0x22, 0x89 },
        { 0x23, 0x48 },
        { 0x24, 0x14 },
        { 0x25, 0xC7 },

        { 0x2E, 0xBA },
        { 0x2F, 0x90 },
        { 0x30, 0x1D },
        { 0x31, 0x04 },
        { 0x32, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_RunPushBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_RunPushBuffer, 4039, 8)

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
// * D3DDevice_Reset
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Reset, 4039, 10)

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
// * D3D::BlockOnTime
// ******************************************************************
OOVPA_XREF(D3D_BlockOnTime, 4039, 10,

    XREF_D3D_BLOCKONTIME,
    XRefZero)

        { 0x07, 0x3D },

        { 0x18, 0x8B },
        { 0x19, 0xC8 },
        { 0x1A, 0x2B },
        { 0x1B, 0xCE },
        { 0x1C, 0x3B },
        { 0x1D, 0xCA },
        { 0x1E, 0x0F },
        { 0x1F, 0x83 },

        { 0x41, 0x3D },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BlockOnFence
// ******************************************************************
OOVPA_XREF(D3DDevice_BlockOnFence, 4039, 8,

    XRefNoSaveIndex,
    XRefOne)

	XREF_ENTRY( 0x08, XREF_D3D_BLOCKONTIME ),

        { 0x00, 0x8B },
        { 0x02, 0x24 },
        { 0x04, 0x6A },
        { 0x06, 0x50 },
        { 0x0C, 0xC2 },
        { 0x0D, 0x04 },
        { 0x0E, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTile
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTile, 4039, 12)

        { 0x05, 0x1D },

        { 0x17, 0x46 },
        { 0x18, 0x04 },
        { 0x19, 0x74 },
        { 0x1A, 0x69 },
        { 0x1B, 0xB9 },
        { 0x1C, 0x06 },
        { 0x1D, 0x00 },
        { 0x1E, 0x00 },
        { 0x1D, 0x00 },

        { 0x7B, 0x5F },
        { 0x8A, 0x76 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVertexShader, 4039, 9)

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0xFC },
        { 0x08, 0x04 },
        { 0x09, 0x00 },
        { 0x0A, 0x00 },
        { 0x0D, 0x24 },
        { 0x10, 0x0A },
        { 0x13, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetPixelShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetPixelShader, 4039, 9)

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0xE8 },
        { 0x08, 0x04 },
        { 0x09, 0x00 },
        { 0x0A, 0x00 },
        { 0x0D, 0x24 },
        { 0x10, 0x0A },
        { 0x13, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetVertexShaderConstant
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVertexShaderConstant, 4039, 7)

        { 0x05, 0x15 },
        { 0x0C, 0x24 },
        { 0x13, 0xE0 },
        { 0x1A, 0x8D },
        { 0x21, 0x8B },
        { 0x28, 0x8B },
        { 0x2F, 0x5F },
OOVPA_END;

// ******************************************************************
// * D3DDevice_IsFencePending
// ******************************************************************
OOVPA_XREF(D3DDevice_IsFencePending, 4039, 6,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x01, XREF_D3DDEVICE ),

        { 0x07, 0x34 },
        { 0x0A, 0x30 },
        { 0x10, 0xD1 },
        { 0x17, 0x1B },
        { 0x1C, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetScreenSpaceOffset
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetScreenSpaceOffset, 4039, 7)

	{ 0x06, 0x56 },
	{ 0x07, 0xD8 },
	{ 0x08, 0x44 },
	{ 0x09, 0x24 },
	{ 0x0A, 0x08 },
	{ 0x2E, 0x8B },
	{ 0x2F, 0x06 },
OOVPA_END;

// ******************************************************************
// * D3D::LazySetPointParams
// ******************************************************************
OOVPA_NO_XREF(D3D_LazySetPointParams, 4039, 7)
	{ 0x00, 0x83 },
	{ 0x01, 0xEC },
	{ 0x02, 0x14 },
	{ 0x4E, 0xE0 },
	{ 0x73, 0xF6 },
	{ 0x74, 0xC4 },
	{ 0x75, 0x41 },
OOVPA_END;

// ******************************************************************
// * D3D8_4034
// ******************************************************************
OOVPATable D3D8_4034[] = {

	REGISTER_OOVPA(CMiniport_InitHardware, 4034, PATCH),
	REGISTER_OOVPA(CMiniport_CreateCtxDmaObject, 4034, PATCH),
	REGISTER_OOVPA(D3D_CMiniport_GetDisplayCapabilities, 3911, PATCH),
	REGISTER_OOVPA(Direct3D_CreateDevice, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_Clear, 4034, PATCH),
	REGISTER_OOVPA(D3DResource_Register, 3925, PATCH),

	// ******************************************************************
	// Tested on version 4039, The Simpsons - Road Rage (PAL) 4039 etc
	// TODO: This needs to be verified on 4034
	// ******************************************************************
	REGISTER_OOVPA(D3DDevice_GetTransform, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SelectVertexShader, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_BlockUntilVerticalBlank, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderTarget, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShader, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_Begin, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetShaderConstantMode, 4039, PATCH),

	REGISTER_OOVPA(D3DResource_IsBusy, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_End, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexDataColor, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData2f, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4f, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVerticalBlankCallback, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetBackBufferScale, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_GetRenderTarget, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDisplayFieldStatus, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_PersistDisplay, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawVerticesUP, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawIndexedVerticesUP, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDisplayMode, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BumpEnv, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_DeletePixelShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_KickOff, 4039, PATCH),
	REGISTER_OOVPA(D3D_SetFence, 4039, XREF),
	REGISTER_OOVPA(D3DDevice_RunPushBuffer, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_Reset, 4039, PATCH),
	REGISTER_OOVPA(D3D_BlockOnTime, 4039, XREF),
	REGISTER_OOVPA(D3DDevice_BlockOnFence, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScissors, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShaderConstant, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTile, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShader, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_GetPixelShader, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderConstant, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_IsFencePending, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScreenSpaceOffset, 4039, PATCH),
	REGISTER_OOVPA(D3D_LazySetPointParams, 4039, PATCH),
	// ******************************************************************

	REGISTER_OOVPA(D3DDevice_GetScissors, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_OcclusionCullEnable, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilCullEnable, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_PSTextureModes, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Dxt1NoiseEnable, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDepthStencilSurface, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetFlickerFilter, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetSoftDisplayFilter, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_Release, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_GetGammaRamp, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_GetCreationParameters, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetViewport, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetViewport, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTransform, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_CreatePixelShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderSize, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_CullMode, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateCubeTexture, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDeviceCaps, 3925, DISABLED),
	REGISTER_OOVPA(D3DDevice_CreatePalette, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateTexture, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVolumeTexture, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateIndexBuffer, 3925, DISABLED),
	REGISTER_OOVPA(D3DDevice_CreateVertexShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZBias, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVertexBuffer, 3925, DISABLED),
	REGISTER_OOVPA(D3DResource_Release, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_AddRef, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_GetType, 4034, PATCH),
	REGISTER_OOVPA(D3D_AllocContiguousMemory, 3925, DISABLED), // Just calls MmAllocateContiguousMemory. Was PATCH
	REGISTER_OOVPA(D3DPalette_Lock, 3925, PATCH),
	REGISTER_OOVPA(D3DTexture_LockRect, 3925, PATCH),
	REGISTER_OOVPA(D3DVolumeTexture_LockBox, 3925, PATCH),
	REGISTER_OOVPA(D3DCubeTexture_LockRect, 3925, PATCH),
	REGISTER_OOVPA(D3DTexture_GetSurfaceLevel, 3925, PATCH),
	REGISTER_OOVPA(D3DSurface_LockRect, 3925, PATCH),
	REGISTER_OOVPA(D3DBaseTexture_GetLevelCount, 3911, PATCH),
	REGISTER_OOVPA(D3DCubeTexture_GetCubeMapSurface, 3911, PATCH),
	REGISTER_OOVPA(Lock3DSurface, 3911, PATCH),
	REGISTER_OOVPA(Lock2DSurface, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetGammaRamp, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawVertices, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawIndexedVertices, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginVisibilityTest, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_EndVisibilityTest, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_CopyRects, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_DeletePatch, 4034, DISABLED), // (TODO)
	REGISTER_OOVPA(D3DDevice_SetStreamSource, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BorderColor, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_ColorKeyColor, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_LoadVertexShader, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVisibilityTestResult, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteVertexShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawRectPatch, 3911, PATCH), // TODO: Unused?
	REGISTER_OOVPA(D3DDevice_SetIndices, 4034, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetRenderState_EdgeAntiAlias, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FillMode, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_BackFillMode, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FogColor, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FrontFace, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LogicOp, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleAntiAlias, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMask, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_NormalizeNormals, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ShadowFunc, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Simple, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilEnable, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilFail, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_YuvEnable, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TwoSidedLighting, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_VertexBlend, 4034, PATCH),
	REGISTER_OOVPA(D3D_CheckDeviceFormat, 3911, DISABLED),
	REGISTER_OOVPA(D3D_SetPushBufferSize, 3925, DISABLED),
	REGISTER_OOVPA(Get2DSurfaceDesc, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZEnable, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_LightEnable, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TextureFactor, 4034, PATCH),
	REGISTER_OOVPA(D3D_GetAdapterIdentifier, 3925, DISABLED),
	REGISTER_OOVPA(D3DSurface_GetDesc, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetLight, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetLightEnable, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetLight, 4034, PATCH),
	REGISTER_OOVPA(D3DVertexBuffer_Lock, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTexture, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPalette, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_TexCoordIndex, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetMaterial, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShader, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_Swap, 4034, PATCH),
	REGISTER_OOVPA(D3D_KickOffAndWaitForIdle, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateImageSurface, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_GetBackBuffer, 4034, PATCH),
	REGISTER_OOVPA(D3D_BlockOnResource, 4034, XREF),
	REGISTER_OOVPA(D3DResource_BlockUntilNotBusy, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMode, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_IsBusy, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LineWidth, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZCmpAlwaysRead, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZRead, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_DoNotCullUncompressed, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_InsertFence, 3925, PATCH),

	// ******************************************************************
	// Provisional registration functions in XDK 4034
	// TODO: Need test cases
	// ******************************************************************
	REGISTER_OOVPA(D3DDevice_DrawTriPatch, 3911, DISABLED),
	REGISTER_OOVPA(D3DDevice_Present, 3925, PATCH),
	REGISTER_OOVPA(D3D_RecordStateBlock, 3911, XREF),
//	REGISTER_OOVPA(D3DDevice_EndStateBlock, 4134, PATCH),
	REGISTER_OOVPA(D3D_ClearStateBlockFlags, 3911, XREF),
	REGISTER_OOVPA(D3DDevice_BeginStateBlock, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_ApplyStateBlock, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CaptureStateBlock, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteStateBlock, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateStateBlock, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShaderProgram, 3925, PATCH),
//	REGISTER_OOVPA(D3DDevice_SetVertexShaderInput, 4134, PATCH),
//	REGISTER_OOVPA(D3DDevice_PrimeVertexCache, 4134, PATCH),
//	REGISTER_OOVPA(D3DDevice_InsertCallback, 4434, PATCH),
	REGISTER_OOVPA(D3DDevice_SwitchTexture, 3925, PATCH),
//	REGISTER_OOVPA(D3DDevice_GetModelView, 4134, PATCH),
//	REGISTER_OOVPA(D3DDevice_BeginPushBuffer, 4134, PATCH),
//	REGISTER_OOVPA(D3DDevice_EndPushBuffer, 4134, PATCH),
//	REGISTER_OOVPA(D3DDevice_LoadVertexShaderProgram, 4134, PATCH),
//	REGISTER_OOVPA(D3DDevice_GetShaderConstantMode, 4134, PATCH),
//	REGISTER_OOVPA(D3DDevice_GetVertexShaderInput, 4134, PATCH),
//	REGISTER_OOVPA(D3DDevice_RunVertexStateShader, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderType, 3925, PATCH),
//	REGISTER_OOVPA(D3DDevice_GetVertexShaderDeclaration, 4134, PATCH),
//	REGISTER_OOVPA(D3DDevice_GetVertexShaderFunction, 4134, PATCH),
//	REGISTER_OOVPA(MakeRequestedSpace, 4134, PATCH),
//	REGISTER_OOVPA(D3DDevice_EnableOverlay, 4134, PATCH),
//	REGISTER_OOVPA(D3DDevice_UpdateOverlay, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetPushBufferOffset, 3925, PATCH),
//	REGISTER_OOVPA(D3DDevice_SetSwapCallback, 4134, PATCH),
//	REGISTER_OOVPA(D3DDevice_SetModelView, 4134, PATCH),
//	REGISTER_OOVPA(D3DDevice_SetVertexData4s, 4134, PATCH),
//	REGISTER_OOVPA(D3DDevice_GetProjectionViewportMatrix, 4134, PATCH),
//	REGISTER_OOVPA(D3DDevice_AddRef, 4361, PATCH),
//	REGISTER_OOVPA(D3DDevice_SetBackMaterial, 4134, PATCH),
//	REGISTER_OOVPA(D3DDevice_SetVertexData2s, 4134, PATCH),
//	REGISTER_OOVPA(D3DDevice_SetVertexData4ub, 4134, PATCH),
//	REGISTER_OOVPA(D3DDevice_GetTile, 4134, PATCH),
//	REGISTER_OOVPA(D3DDevice_FlushVertexCache, 4134, PATCH),
	// ******************************************************************
};

// ******************************************************************
// * D3D8_4034_SIZE
// ******************************************************************
uint32 D3D8_4034_SIZE = sizeof(D3D8_4034);
