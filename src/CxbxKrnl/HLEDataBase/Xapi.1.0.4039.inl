// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->Xapi.1.0.4039.inl
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

// Generic OOVPA as of 4039 and newer.
// ******************************************************************
// * XCalculateSignatureBegin
// ******************************************************************
OOVPA_NO_XREF(XCalculateSignatureBegin, 4039, 16)

        { 0x00, 0xA1 },
        { 0x01, 0x18 },
        { 0x02, 0x01 },
        { 0x03, 0x01 },
        { 0x04, 0x00 },
        { 0x05, 0xFF },
        { 0x06, 0x70 },
        { 0x07, 0x08 },
        { 0x08, 0xFF },
        { 0x09, 0x74 },
        { 0x0A, 0x24 },
        { 0x0B, 0x08 },
        { 0x0C, 0xE8 },

        { 0x11, 0xC2 },
        { 0x12, 0x04 },
        { 0x13, 0x00 },
OOVPA_END;
