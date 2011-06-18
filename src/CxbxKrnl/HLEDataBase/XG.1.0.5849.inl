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
// * XGSwizzleBox
// ******************************************************************
SOOVPA<10> XGSwizzleBox_1_0_5849 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // XGSwizzleBox+0x60 : sub ecx, [eax]
        { 0x60, 0x2B }, // (Offset,Value)-Pair #1
        { 0x61, 0x08 }, // (Offset,Value)-Pair #2

        // XGSwizzleBox+0x8E : lea ecx, [ebp+0xD8]
        { 0x8E, 0x8D }, // (Offset,Value)-Pair #3
        { 0x8F, 0x4D }, // (Offset,Value)-Pair #4
        { 0x90, 0xD8 }, // (Offset,Value)-Pair #5

        // XGSwizzleBox+0xD5 : shl eax, 2
        { 0xD5, 0xC1 }, // (Offset,Value)-Pair #6
        { 0xD6, 0xE0 }, // (Offset,Value)-Pair #7
        { 0xD7, 0x02 }, // (Offset,Value)-Pair #8

        // XGSwizzleBox+0xE2 : test ebx, ebx
        { 0xE2, 0x85 }, // (Offset,Value)-Pair #9
        { 0xE3, 0xDB }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * XFONT_OpenBitmapFontFromMemory
// ******************************************************************
SOOVPA<8> XFONT_OpenBitmapFontFromMemory_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x0B, 0x75 },
        { 0x1A, 0x8B },
        { 0x28, 0x8B },
        { 0x32, 0x08 },
        { 0x3F, 0x8B },
        { 0x4C, 0x8B },
        { 0x59, 0x45 },
        { 0x66, 0x0C },
    }
};

// ******************************************************************
// * XG_1_0_5849
// ******************************************************************
OOVPATable XG_1_0_5849[] =
{
    // XGIsSwizzledFormat (* unchanged since 4361 *)
    {
        (OOVPA*)&XGIsSwizzledFormat_1_0_4361,

        XTL::EmuXGIsSwizzledFormat,

        #ifdef _DEBUG_TRACE
        "XGIsSwizzledFormat"
        #endif
    },
    // XGSwizzleRect (* unchanged since 5558 *)
    {
        (OOVPA*)&XGSwizzleRect_1_0_5558,

        XTL::EmuXGSwizzleRect,

        #ifdef _DEBUG_TRACE
        "EmuXGSwizzleRect"
        #endif
    },
	// XGUnswizzleRect (* unchanged since 5558 *)
    {
        (OOVPA*)&XGUnswizzleRect_1_0_5558,

        XTL::EmuXGUnswizzleRect,

        #ifdef _DEBUG_TRACE
        "EmuXGUnswizzleRect"
        #endif
    },
	// XFONT_OpenBitmapFontFromMemory
    /*{
        (OOVPA*)&XFONT_OpenBitmapFontFromMemory_1_0_5849,

        XTL::EmuXFONT_OpenBitmapFontFromMemory,

        #ifdef _DEBUG_TRACE
        "EmuXFONT_OpenBitmapFontFromMemory"
        #endif
    },*/
};

// ******************************************************************
// * XG_1_0_5849_SIZE
// ******************************************************************
uint32 XG_1_0_5849_SIZE = sizeof(XG_1_0_5849);
