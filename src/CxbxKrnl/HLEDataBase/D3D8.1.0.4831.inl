// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->D3D8.1.0.4831.inl
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
// *  (c) 2017 jarupxx
// *
// *  All rights reserved
// *
// ******************************************************************

// ******************************************************************
// * D3DDevice_CreateTexture2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateTexture2, 4831, 12)

        { 0x00, 0x56 },
        { 0x01, 0x57 },

        { 0x10, 0x0F },
        { 0x11, 0x94 },
        { 0x12, 0xC2 },
        { 0x13, 0x8D },
        { 0x14, 0x4C },
        { 0x15, 0x24 },
        { 0x16, 0x28 },
        { 0x17, 0x51 },

        { 0x51, 0x24 },
        { 0x52, 0xF7 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 4831 version
// ******************************************************************
// * D3DDevice_CreateTexture2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateTexture2, 4831, 12)

        // D3DDevice_CreateTexture2+0x02 : lea eax, [esp+0x20]
        { 0x04, 0x8D },
        { 0x07, 0x20 },

        // D3DDevice_CreateTexture2+0x0E : setz dl
        { 0x10, 0x0F },
        { 0x11, 0x94 },
        { 0x12, 0xC2 },

        // D3DDevice_CreateTexture2+0x2A : push 0x00
        { 0x2E, 0x6A },
        { 0x2F, 0x00 },

        // D3DDevice_CreateTexture2+0x53 : call [abs]
        { 0x57, 0xE8 },

        // D3DDevice_CreateTexture2+0x78 : jnz +0x0D
        { 0x7C, 0x75 },
        { 0x7D, 0x0D },

        // D3DDevice_CreateTexture2+0xAE : retn 0x1C
        { 0x88, 0xC2 },
        { 0x89, 0x1C },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_SetTexture
// ******************************************************************
#ifndef WIP_LessVertexPatching
OOVPA_NO_XREF(D3DDevice_SetTexture, 4831, 15)
#else
OOVPA_XREF(D3DDevice_SetTexture, 4831, 1+15,

    XRefNoSaveIndex,
    XRefOne)

		XREF_ENTRY( 0x13, XREF_OFFSET_D3DDEVICE_M_TEXTURES ), // Derived
#endif
#if 0 // PatrickvL preparations // 1+31,
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
		{ 0x0B, 0x3D }, // TODO : See if this should be value 0x56?

		{ 0x10, 0x8B },
		{ 0x11, 0x84 },
		{ 0x12, 0xB7 },
		// { 0x13, 0x38 }, // disabled. part of an offset
		// { 0x14, 0x0B },
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
#else
		{ 0x0B, 0x56 },

        { 0x21, 0x74 },
        { 0x22, 0x20 },

        { 0x33, 0xF7 },
        { 0x34, 0xC1 },
        { 0x36, 0xFF },

        { 0x39, 0x89 },
        { 0x3A, 0x10 },

        { 0x50, 0x75 },
        { 0x51, 0x47 },
#endif
		{ 0x52, 0x8B },
        { 0x53, 0x45 },
        { 0x54, 0x00 },
        { 0x55, 0x3B },
        { 0x56, 0x45 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_PersistDisplay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_PersistDisplay, 4831, 13) // Up to 5344

        { 0x00, 0x83 },
        { 0x01, 0xEC },
        { 0x1E, 0x15 },

        { 0x28, 0x00 },
        { 0x29, 0x85 },
        { 0x2A, 0xC0 },
        { 0x2B, 0x75 },
        { 0x2C, 0x0A },
        { 0x2D, 0xB8 },
        { 0x2E, 0x05 },
        { 0x2F, 0x40 },
        { 0x30, 0x00 },

        { 0x36, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPixelShaderConstant
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPixelShaderConstant, 4831, 13)

        // D3DDevice_SetPixelShaderConstant+0x1E : test eax, eax
        { 0x1E, 0x85 },
        { 0x1F, 0xC0 },

        // D3DDevice_SetPixelShaderConstant+0x32 : mov [ebp+0x10], eax
        { 0x32, 0x89 },
        { 0x33, 0x45 },
        { 0x34, 0x10 },

        // D3DDevice_SetPixelShaderConstant+0x84 : or eax, ebx
        { 0x84, 0x0B },
        { 0x85, 0xC3 },

        // D3DDevice_SetPixelShaderConstant+0xBD : mov dword ptr [eax], 0x406A0
        { 0xBD, 0xC7 },
        { 0xBE, 0x00 },
        { 0xBF, 0x60 },
        { 0xC0, 0x0A },
        { 0xC1, 0x04 },
        { 0xC2, 0x00 },
OOVPA_END;
