// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->XG.1.0.5849.cpp
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
// * XFONT_OpenBitmapFontFromMemory
// ******************************************************************
OOVPA_NO_XREF(XFONT_OpenBitmapFontFromMemory_1_0_5849, 8)

        { 0x0B, 0x75 },
        { 0x1A, 0x8B },
        { 0x28, 0x8B },
        { 0x32, 0x08 },
        { 0x3F, 0x8B },
        { 0x4C, 0x8B },
        { 0x59, 0x45 },
        { 0x66, 0x0C },
OOVPA_END;

// ******************************************************************
// * XG_1_0_5849
// ******************************************************************
OOVPATable XG_1_0_5849[] = {

    // XGIsSwizzledFormat (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(XGIsSwizzledFormat_1_0_4361, XTL::EmuXGIsSwizzledFormat),
    // XGSwizzleRect (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(XGSwizzleRect_1_0_3911, XTL::EmuXGSwizzleRect),
    // XGSwizzleBox (* unchanged since 4627 *) (* UNTESTED *)
	OOVPA_TABLE_PATCH(XGSwizzleBox_1_0_4627, XTL::EmuXGSwizzleBox),
	// XGUnswizzleRect (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(XGUnswizzleRect_1_0_3911, XTL::EmuXGUnswizzleRect),
	// XFONT_OpenBitmapFontFromMemory
    /*
	OOVPA_TABLE_PATCH(XFONT_OpenBitmapFontFromMemory_1_0_5849, XTL::EmuXFONT_OpenBitmapFontFromMemory),
	*/
};

// ******************************************************************
// * XG_1_0_5849_SIZE
// ******************************************************************
uint32 XG_1_0_5849_SIZE = sizeof(XG_1_0_5849);
