// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->XG.1.0.4134.inl
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
// * XGSetVertexBufferHeader
// ******************************************************************
//Generic OOVPA as of 4134 and newer.
OOVPA_NO_XREF(XGSetVertexBufferHeader, 4134, 15)

        { 0x01, 0x44 },
        { 0x04, 0x8B },
        { 0x07, 0x18 },
        { 0x0A, 0x08 },

        { 0x0C, 0xC7 },
        { 0x0D, 0x00 },
        { 0x0E, 0x01 },
        { 0x0F, 0x00 },
        { 0x10, 0x00 }, // XGSetVertexBufferHeader 0x00 vs XGSetIndexBufferHeader 0x01
        { 0x11, 0x00 },
        { 0x12, 0x89 },
        { 0x13, 0x48 },
        { 0x14, 0x04 },
        { 0x15, 0xC2 },
        { 0x16, 0x18 },
OOVPA_END;

// ******************************************************************
// * XGSetIndexBufferHeader
// ******************************************************************
//Generic OOVPA as of 4134 and newer.
OOVPA_NO_XREF(XGSetIndexBufferHeader, 4134, 15)

        { 0x01, 0x44 },
        { 0x04, 0x8B },
        { 0x07, 0x18 },
        { 0x0A, 0x08 },

        { 0x0C, 0xC7 },
        { 0x0D, 0x00 },
        { 0x0E, 0x01 },
        { 0x0F, 0x00 },
        { 0x10, 0x01 }, // XGSetVertexBufferHeader 0x00 vs XGSetIndexBufferHeader 0x01
        { 0x11, 0x00 },
        { 0x12, 0x89 },
        { 0x13, 0x48 },
        { 0x14, 0x04 },
        { 0x15, 0xC2 },
        { 0x16, 0x18 },
OOVPA_END;
