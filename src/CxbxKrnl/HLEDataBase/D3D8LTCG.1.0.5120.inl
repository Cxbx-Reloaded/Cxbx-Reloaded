// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->D3D8LTCG.1.0.5120.inl
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
// * Direct3D_CreateDevice
// ******************************************************************
//00000800A1
OOVPA_NO_XREF(Direct3D_CreateDevice, 1036, 7)

        { 0x00, 0x55 },
        { 0x09, 0xC0 },

        { 0x12, 0x00 },
        { 0x13, 0x00 },
        { 0x14, 0x08 },
        { 0x15, 0x00 },
        { 0x16, 0xA1 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Reset
// ******************************************************************
//803F6A006A036A006A00E8
OOVPA_NO_XREF(D3DDevice_Reset, 1036, 11)

        { 0x00, 0x53 },
        { 0x01, 0x8B },

        { 0xA6, 0x6A },
        { 0xA7, 0x00 },
        { 0xA8, 0x6A },
        { 0xA9, 0x03 },
        { 0xAA, 0x6A },
        { 0xAB, 0x00 },
        { 0xAC, 0x6A },
        { 0xAD, 0x00 },
        { 0xAE, 0xE8 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShaderInput
// ******************************************************************
//8B0F81C10000F8FF8BC1A9
OOVPA_NO_XREF(D3DDevice_SetVertexShaderInput, 1024, 19)

        { 0x00, 0x53 },
        { 0x01, 0x8B },
        { 0x02, 0x5C },
        { 0x03, 0x24 },
        { 0x04, 0x08 },
        { 0x05, 0x56 },
        { 0x06, 0x8B },
        { 0x07, 0x35 },

        { 0x25, 0x8B },
        { 0x26, 0x0F },
        { 0x27, 0x81 },
        { 0x28, 0xC1 },
        { 0x29, 0x00 },
        { 0x2A, 0x00 },
        { 0x2B, 0xF8 },
        { 0x2C, 0xFF },
        { 0x2D, 0x8B },
        { 0x2E, 0xC1 },
        { 0x2F, 0xA9 },
OOVPA_END;
