// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->XG.1.0.3911.inl
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
// * XGIsSwizzledFormat
// ******************************************************************
OOVPA_NO_XREF(XGIsSwizzledFormat, 3911, 7)

        { 0x07, 0x7F },
        { 0x10, 0x7C },
        { 0x19, 0x0B },
        { 0x22, 0xF8 },
        { 0x2B, 0x83 },
        { 0x34, 0x0A },
        { 0x3D, 0x7F },
OOVPA_END;

// ******************************************************************
// * XGSwizzleRect
// ******************************************************************
OOVPA_NO_XREF(XGSwizzleRect, 3911, 8)

        { 0x1E, 0x03 },
        { 0x3E, 0x89 },
        { 0x5E, 0x83 },
        { 0x7E, 0x6C },
        { 0x9E, 0xFF },
        { 0xBE, 0xFF },
        { 0xDE, 0x89 },
        { 0xFE, 0x89 },
OOVPA_END;

// ******************************************************************
// * XGSwizzleBox
// ******************************************************************
OOVPA_NO_XREF(XGSwizzleBox, 3911, 8)

        { 0x1E, 0x75 },
        { 0x3E, 0x4D },
        { 0x5E, 0x48 },
        { 0x7E, 0x04 },
        { 0x9E, 0xD8 },
        { 0xC0, 0x83 },
        { 0xDE, 0xAF },
        { 0xFE, 0x45 },
OOVPA_END;

// ******************************************************************
// * XGUnswizzleRect
// ******************************************************************
OOVPA_NO_XREF(XGUnswizzleRect, 3911, 8)

        { 0x1E, 0x03 },
        { 0x3E, 0x00 },
        { 0x5E, 0xD2 },
        { 0x7E, 0x75 },
        { 0x9E, 0x70 },
        { 0xC1, 0xE9 },
        { 0xDE, 0x89 },
        { 0xFE, 0x60 },
OOVPA_END;

// ******************************************************************
// * XGWriteSurfaceOrTextureToXPR
// ******************************************************************
OOVPA_NO_XREF(XGWriteSurfaceOrTextureToXPR, 3911, 12)

        { 0x0C, 0x08 },
        { 0x1E, 0x3D },

        { 0x21, 0x05 },
        { 0x22, 0x00 },
        { 0x23, 0x74 },
        { 0x24, 0x0A },
        { 0x25, 0xB8 },
        { 0x26, 0x05 },
        { 0x27, 0x40 },
        { 0x28, 0x00 },

        { 0x2E, 0x00 },
        { 0x47, 0x03 },

/*
        { 0x1E, 0x3D },
        { 0x3E, 0xE0 },
        { 0x5E, 0x75 },
        { 0x7E, 0x33 },
        { 0x9E, 0xC2 },
        { 0xAE, 0x4D },
        { 0xBE, 0xF0 },*/
OOVPA_END;

// ******************************************************************
// * XGSetTextureHeader
// ******************************************************************
OOVPA_NO_XREF(XGSetTextureHeader, 3911, 7)

        { 0x04, 0x75 },
        { 0x0A, 0x00 },
        { 0x10, 0xFF },
        { 0x16, 0xFF },
        { 0x1C, 0x75 },
        { 0x26, 0x5D },
        { 0x28, 0x24 },
OOVPA_END;

// ******************************************************************
// * XGUnswizzleBox
// ******************************************************************
OOVPA_NO_XREF(XGUnswizzleBox, 3911, 8)

        { 0x1E, 0x26 },
        { 0x3E, 0x55 },
        { 0x5E, 0x58 },
        { 0x7E, 0x89 },
        { 0xA0, 0xFF },
        { 0xBE, 0x2C },
        { 0xDE, 0x24 },
        { 0xFE, 0x20 },
OOVPA_END;

// ******************************************************************
// * XGCompressRect
// ******************************************************************
//Generic OOVPA as of 3911 and newer.
OOVPA_NO_XREF(XGCompressRect, 3911, 12)

        { 0x00, 0x55 },
        { 0x01, 0x8D },

        { 0x14, 0x83 },
        { 0x15, 0x7D },
        { 0x16, 0x6C },
        { 0x17, 0x00 },
        { 0x18, 0x56 },
        { 0x19, 0x57 },
        { 0x1A, 0xC7 },
        { 0x1B, 0x45 },

        { 0xC0, 0x7E },
        { 0xC1, 0x01 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * XGCompressRect
// ******************************************************************
OOVPA_NO_XREF(XGCompressRect, 3911, 8)

        { 0x01, 0x8D },
        { 0x1E, 0x00 },
        { 0x32, 0x0F },
        { 0x5E, 0x85 },
        { 0x80, 0x85 },
        { 0x9E, 0x07 },
        { 0xBE, 0x80 },
        { 0xDA, 0xAF },
OOVPA_END;
#endif
// ******************************************************************
// * XGSetIndexBufferHeader
// ******************************************************************
OOVPA_NO_XREF(XGSetIndexBufferHeader, 3911, 13)

        { 0x01, 0x44 },
        { 0x04, 0x8B },

        { 0x06, 0x24 },
        { 0x07, 0x18 },
        { 0x08, 0xC7 },
        { 0x09, 0x00 },
        { 0x0A, 0x01 },
        { 0x0B, 0x00 },
        { 0x0C, 0x01 },
        { 0x0D, 0x00 },

        { 0x10, 0x04 },
        { 0x11, 0xC2 },
        { 0x12, 0x18 },
OOVPA_END;

// ******************************************************************
// * XGSetVertexBufferHeader
// ******************************************************************
OOVPA_NO_XREF(XGSetVertexBufferHeader, 3911, 13)

        { 0x01, 0x44 },
        { 0x04, 0x8B },

        { 0x07, 0x18 },
        { 0x08, 0xC7 },
        { 0x09, 0x00 },
        { 0x0A, 0x01 },
        { 0x0B, 0x00 },
        { 0x0C, 0x00 },
        { 0x0D, 0x00 },
        { 0x0E, 0x89 },

        { 0x10, 0x04 },
        { 0x11, 0xC2 },
        { 0x12, 0x18 },
OOVPA_END;
