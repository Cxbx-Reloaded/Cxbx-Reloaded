// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->D3D8.1.0.4361.inl
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
// * D3DDevice_SetTexture
// ******************************************************************
#ifndef WIP_LessVertexPatching
OOVPA_NO_XREF(D3DDevice_SetTexture, 4361, 28)
#else
OOVPA_XREF(D3DDevice_SetTexture, 4361, 1+26,

    XRefNoSaveIndex,
    XRefOne)

		XREF_ENTRY( 0x13, XREF_OFFSET_D3DDEVICE_M_TEXTURES ), // Derived
#endif
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
#ifndef WIP_LessVertexPatching
		{ 0x13, 0x78 }, // disabled. part of an offset
		{ 0x14, 0x0A },
#endif
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
