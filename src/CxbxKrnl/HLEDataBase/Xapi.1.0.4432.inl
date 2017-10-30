// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->Xapi.1.0.4432.inl
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
OOVPA_NO_XREF(XMountUtilityDrive, 4432, 27)

        { 0x00, 0x55 },
        { 0x01, 0x8B },
        { 0x02, 0xEC },
        { 0x03, 0x81 },
        { 0x04, 0xEC },
        { 0x05, 0x14 },
        { 0x06, 0x01 },
        { 0x07, 0x00 },
        { 0x08, 0x00 },
        { 0x09, 0x53 },
        { 0x0A, 0x56 },
        { 0x0B, 0x57 },
        { 0x0C, 0x8D },
        { 0x0D, 0x45 },
        { 0x0E, 0xF4 },
        { 0x0F, 0x50 },
        { 0x10, 0x8D },
        { 0x11, 0x45 },
        { 0x12, 0xFC },
        { 0x13, 0x50 },
        { 0x14, 0xFF },
        { 0x15, 0x75 },
        { 0x16, 0x08 },
        { 0x17, 0xE8 },

        { 0x51, 0x68 },
        { 0x52, 0x04 },
        { 0x53, 0x01 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 4432 version
// ******************************************************************
// * XMountUtilityDrive
// ******************************************************************
OOVPA_NO_XREF(XMountUtilityDrive, 4432, 8)

        // XapiUnknownBad1+0x51 : push 0x0104
        { 0x51, 0x68 },
        { 0x52, 0x04 },
        { 0x53, 0x01 },

        // XapiUnknownBad1+0xB3 : jnz +0x1A
        { 0xB3, 0x75 },
        { 0xB4, 0x1A },

        // XapiUnknownBad1+0xE8 : setnl al
        { 0xE8, 0x0F },
        { 0xE9, 0x9D },
        { 0xEA, 0xC0 },

        // TODO: Finish signature, toss out function call basically
OOVPA_END;
#endif