// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->XG.1.0.4627.cpp
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

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * XGSwizzleBox
// ******************************************************************
OOVPA_NO_XREF(XGSwizzleBox, 4627, 10)

        // XGSwizzleBox+0x60 : sub ecx, [eax]
        { 0x60, 0x2B },
        { 0x61, 0x08 },

        // XGSwizzleBox+0x8E : lea ecx, [ebp+0xD8]
        { 0x8E, 0x8D },
        { 0x8F, 0x4D },
        { 0x90, 0xD8 },

        // XGSwizzleBox+0xD5 : shl eax, 2
        { 0xD5, 0xC1 },
        { 0xD6, 0xE0 },
        { 0xD7, 0x02 },

        // XGSwizzleBox+0xE2 : test ebx, ebx
        { 0xE2, 0x85 },
        { 0xE3, 0xDB },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * XGWriteSurfaceOrTextureToXPR
// ******************************************************************
OOVPA_NO_XREF(XGWriteSurfaceOrTextureToXPR, 4627, 7)

        { 0x1E, 0x3D },
        { 0x3E, 0xE0 },
        { 0x5E, 0x75 },
        { 0x7E, 0x33 },
        { 0x9E, 0xC2 },
        { 0xBE, 0xF0 },
        { 0xE2, 0x8B },
OOVPA_END;
#endif
// ******************************************************************
// * XG_4627
// ******************************************************************
OOVPATable XG_4627[] = {

	REGISTER_OOVPA(XGIsSwizzledFormat, 3911, PATCH),
	// REGISTER_OOVPA(XGSwizzleRect, 3911, DISABLED), // TODO : Uncomment
	// REGISTER_OOVPA(XGUnswizzleRect, 3911, DISABLED), // TODO : Uncomment
	REGISTER_OOVPA(XGSwizzleBox, 3911, PATCH),
	REGISTER_OOVPA(XGWriteSurfaceOrTextureToXPR, 3911, PATCH),
	REGISTER_OOVPA(XGSetTextureHeader, 3911, PATCH),
	REGISTER_OOVPA(XGSetVertexBufferHeader, 4361, XREF),
	REGISTER_OOVPA(XGSetIndexBufferHeader, 4361, XREF),
	REGISTER_OOVPA(XGCompressRect, 3911, XREF),
	REGISTER_OOVPA(XGUnswizzleBox, 3911, DISABLED),
	REGISTER_OOVPA(XFONT_OpenBitmapFontFromMemory, 4361, XREF),
};

// ******************************************************************
// * XG_4627_SIZE
// ******************************************************************
uint32 XG_4627_SIZE = sizeof(XG_4627);
