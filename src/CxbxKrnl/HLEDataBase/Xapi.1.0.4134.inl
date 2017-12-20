// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->Xapi.1.0.4134.inl
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
// * XMountUtilityDrive
// ******************************************************************
OOVPA_NO_XREF(XMountUtilityDrive, 4134, 10)

        // XMountUtilityDrive+0x03 : sub esp, 0x0114
        { 0x03, 0x81 },
        { 0x04, 0xEC },
        { 0x05, 0x14 },
        { 0x06, 0x01 },

        // XMountUtilityDrive+0x45 : push 0x0104
        { 0x45, 0x68 },
        { 0x46, 0x04 },
        { 0x47, 0x01 },

        // XMountUtilityDrive+0xAA : lea eax, [ebp-0x10]
        { 0xAA, 0x8D },
        { 0xAB, 0x45 },
        { 0xAC, 0xF0 },
OOVPA_END;

// ******************************************************************
// * XSetProcessQuantumLength
// ******************************************************************
OOVPA_NO_XREF(XSetProcessQuantumLength, 4134, 7)

        { 0x01, 0xA1 },
        { 0x04, 0x00 },
        { 0x07, 0x4C },
        { 0x0A, 0x8B },
        { 0x0D, 0x8D },
        { 0x10, 0x89 },
        { 0x13, 0xC2 },
OOVPA_END;

// ******************************************************************
// * GetTypeInformation
// ******************************************************************
OOVPA_XREF(GetTypeInformation, 4134, 24,

    XREF_XAPI_GetTypeInformation,
    XRefZero)

		{ 0x00, 0xB8 },
		{ 0x05, 0x56 },
		{ 0x06, 0x8B },
		{ 0x07, 0xD0 },
		{ 0x08, 0xBE },
		{ 0x0D, 0x3B },
		{ 0x0E, 0xD6 },
		{ 0x0F, 0x73 },
		{ 0x10, 0x12 },
		{ 0x11, 0x8B },
		{ 0x12, 0x10 },
		{ 0x13, 0x85 },
		{ 0x14, 0xD2 },
		{ 0x15, 0x74 },
		{ 0x16, 0x05 },
		{ 0x17, 0x39 },
		{ 0x18, 0x4A },
		{ 0x19, 0x04 },
		{ 0x1A, 0x74 },
		{ 0x1B, 0x0B },
		{ 0x1C, 0x83 },
		{ 0x1D, 0xC0 },
		{ 0x1E, 0x04 },
		{ 0x1F, 0x3B }
OOVPA_END;
