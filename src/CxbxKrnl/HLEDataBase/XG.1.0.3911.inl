// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->XG.1.0.3911.inl
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

/* Leave unpatched
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
*/

// ******************************************************************
// * XGWriteSurfaceOrTextureToXPR
// ******************************************************************
OOVPA_NO_XREF(XGWriteSurfaceOrTextureToXPR, 3911, 7)

        { 0x1E, 0x3D },
        { 0x3E, 0xE0 },
        { 0x5E, 0x75 },
        { 0x7E, 0x33 },
        { 0x9E, 0xC2 },
        { 0xBE, 0xF0 },
        { 0xDE, 0xFC },
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
// * XG_3911
// ******************************************************************
OOVPATable XG_3911[] = {

    // XGIsSwizzledFormat
	OOVPA_TABLE_ENTRY(XGIsSwizzledFormat, 3911),
	// XGSwizzleRect
	/* Leave unpatched
	OOVPA_TABLE_ENTRY(XGSwizzleRect, 3911),
	*/
	// XGUnswizzleRect
	/* Leave unpatched
	OOVPA_TABLE_ENTRY(XGUnswizzleRect, 3911),
    */
	// XGSwizzleBox
	OOVPA_TABLE_ENTRY(XGSwizzleBox, 3911),
	// XGWriteSurfaceOrTextureToXPR
	OOVPA_TABLE_ENTRY(XGWriteSurfaceOrTextureToXPR, 3911),
	// XGSetTextureHeader
	OOVPA_TABLE_ENTRY(XGSetTextureHeader, 3911),
};

// ******************************************************************
// * XG_3911_SIZE
// ******************************************************************
uint32 XG_3911_SIZE = sizeof(XG_3911);