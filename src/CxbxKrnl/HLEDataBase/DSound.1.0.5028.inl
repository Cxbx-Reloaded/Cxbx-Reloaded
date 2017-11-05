// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->DSound.1.0.5028.inl
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
// * XFileCreateMediaObjectEx
// ******************************************************************
OOVPA_NO_XREF(XFileCreateMediaObjectEx, 5028, 8)

        { 0x03, 0x56 },
        { 0x22, 0x1B },
        { 0x89, 0x1B },
        { 0xA9, 0x85 },
        { 0xAA, 0xF6 },
        { 0xAB, 0x7C },
        { 0xAC, 0x0C },
        { 0xAD, 0x57 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_Flush
// ******************************************************************
// Generic OOVPA as of 5028 and newer
OOVPA_XREF(CDirectSoundStream_Flush, 5028, 1+8,

    XRefNoSaveIndex,
    XRefOne)

        //CDirectSoundStream_Flush+0x31 : call [CMcpxStream_Flush]
        XREF_ENTRY( 0x32, XREF_CMcpxStream_Flush ),

        { 0x00, 0x56 },

        { 0x28, 0x8B },
        { 0x2B, 0x08 },

        { 0x2C, 0x8B },
        { 0x2E, 0x24 },

        { 0x31, 0xE8 },

        //CDirectSoundStream_Flush+0x48 : ret 4
        { 0x48, 0xC2 },
        { 0x49, 0x04 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_FlushEx
// ******************************************************************
// Generic OOVPA as of 5028 and newer
OOVPA_XREF(CDirectSoundStream_FlushEx, 5028, 16,

    XREF_CDirectSoundStream_FlushEx,
    XRefZero)

        { 0x00, 0x55 },

        { 0x24, 0xB8 },
        { 0x25, 0x05 },
        { 0x26, 0x40 },
        { 0x27, 0x00 },
        { 0x28, 0x80 },

        { 0x29, 0xEB },
        { 0x2A, 0x3A },
        { 0x2B, 0x83 },
        { 0x2C, 0x7D },

        { 0x36, 0x74 },
        { 0x37, 0x12 },

        { 0x3E, 0xFF },
        { 0x40, 0x0C },

        { 0x67, 0xC2 },
        { 0x68, 0x10 },
OOVPA_END;
