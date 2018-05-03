// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->D3D8.1.0.4531.inl
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
// * D3DDevice_UpdateOverlay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_UpdateOverlay, 4531, 11) // Up to 5120

        { 0x03, 0xA1 },
        { 0x1A, 0x04 },
        { 0x1F, 0x7C },

        { 0x6F, 0x8B },
        { 0x70, 0x4F },
        { 0x71, 0x04 },
        { 0x72, 0x8B },
        { 0x73, 0xD0 },
        { 0x74, 0x8B },
        { 0x75, 0x44 },
        { 0x76, 0x24 },
OOVPA_END;

// ******************************************************************
// * D3D::CDevice::KickOff
// ******************************************************************
OOVPA_XREF(D3DDevice_KickOff, 4531, 13,

    XREF_D3D_CDevice_KickOff,
    XRefZero)

        // D3D::CDevice::KickOff+0x0B : mov edx, [ecx+0x35C]
        { 0x0B, 0x8B },
        { 0x0C, 0x91 },
        { 0x0D, 0x5C },
        { 0x0E, 0x03 },
        { 0x0F, 0x00 },
        { 0x10, 0x00 },

        // D3D::CDevice::KickOff+0x1F : test ch, 0x20
        { 0x1F, 0xF6 },
        { 0x20, 0xC5 },
        { 0x21, 0x20 },

        // D3D::CDevice::KickOff+0x2F : sfence
        { 0x2F, 0x0F },
        { 0x30, 0xAE },
        { 0x31, 0xF8 },

        // D3D::CDevice::KickOff+0xE2 : retn
        { 0xE2, 0xC3 }
OOVPA_END;

// ******************************************************************
// * D3DPalette_Lock
// ******************************************************************
OOVPA_NO_XREF(D3DPalette_Lock, 4531, 17)

        { 0x00, 0xF6 },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x0C },
        { 0x04, 0xA0 },
        { 0x05, 0x56 },
        { 0x06, 0x8B },
        { 0x07, 0x74 },
        { 0x08, 0x24 },
        { 0x09, 0x08 },
        { 0x0A, 0x75 },
        { 0x0B, 0x06 },
        { 0x0C, 0x56 },
        { 0x0D, 0xE8 },

        { 0x12, 0x8B },
        { 0x21, 0xC2 },
        { 0x22, 0x0C },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BeginPush
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginPush, 4531, 12)

		{ 0x00, 0x56 },
		{ 0x07, 0x6A },

		{ 0x10, 0x8B },
		{ 0x11, 0x44 },
		{ 0x12, 0x24 },
		{ 0x13, 0x08 },
		{ 0x14, 0x40 },
		{ 0x15, 0x50 },
		{ 0x16, 0x56 },
		{ 0x17, 0xE8 },
		{ 0x1D, 0xC2 },
		{ 0x1E, 0x04 },
OOVPA_END;
