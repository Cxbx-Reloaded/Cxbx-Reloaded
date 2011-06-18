// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->XG.1.0.4034.inl
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

#if 0
// ******************************************************************
// * XGIsSwizzledFormat
// ******************************************************************
SOOVPA<7> XGIsSwizzledFormat_1_0_3911 = 
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x07, 0x7F },
        { 0x10, 0x7C },
        { 0x19, 0x0B },
        { 0x22, 0xF8 },
        { 0x2B, 0x83 },
        { 0x34, 0x0A },
        { 0x3D, 0x7F },
    }
};

// ******************************************************************
// * XGSwizzleRect
// ******************************************************************
SOOVPA<8> XGSwizzleRect_1_0_3911 = 
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x1E, 0x03 },
        { 0x3E, 0x89 },
        { 0x5E, 0x83 },
        { 0x7E, 0x6C },
        { 0x9E, 0xFF },
        { 0xBE, 0xFF },
        { 0xDE, 0x89 },
        { 0xFE, 0x89 },
    }
};

// ******************************************************************
// * XGSwizzleBox
// ******************************************************************
SOOVPA<8> XGSwizzleBox_1_0_3911 = 
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x1E, 0x75 },
        { 0x3E, 0x4D },
        { 0x5E, 0x48 },
        { 0x7E, 0x04 },
        { 0x9E, 0xD8 },
        { 0xC0, 0x83 },
        { 0xDE, 0xAF },
        { 0xFE, 0x45 },
    }
};

// ******************************************************************
// * XGUnswizzleRect
// ******************************************************************
SOOVPA<8> XGUnswizzleRect_1_0_3911 = 
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x1E, 0x03 },
        { 0x3E, 0x00 },
        { 0x5E, 0xD2 },
        { 0x7E, 0x75 },
        { 0x9E, 0x70 },
        { 0xC1, 0xE9 },
        { 0xDE, 0x89 },
        { 0xFE, 0x60 },
    }
};

// ******************************************************************
// * XGWriteSurfaceOrTextureToXPR
// ******************************************************************
SOOVPA<7> XGWriteSurfaceOrTextureToXPR_1_0_3911 = 
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x1E, 0x3D },
        { 0x3E, 0xE0 },
        { 0x5E, 0x75 },
        { 0x7E, 0x33 },
        { 0x9E, 0xC2 },
        { 0xBE, 0xF0 },
        { 0xDE, 0xFC },
    }
};
#endif

// ******************************************************************
// * XG_1_0_4034
// ******************************************************************
OOVPATable XG_1_0_4034[] =
{
    // XGIsSwizzledFormat
    /*{
        (OOVPA*)&XGIsSwizzledFormat_1_0_3911,

        XTL::EmuXGIsSwizzledFormat,

        #ifdef _DEBUG_TRACE
        "XGIsSwizzledFormat"
        #endif
    },*/
    // XGSwizzleRect (* unchanged since 3911 *)
    {
        (OOVPA*)&XGSwizzleRect_1_0_3911,

        XTL::EmuXGSwizzleRect,

        #ifdef _DEBUG_TRACE
        "EmuXGSwizzleRect"
        #endif
    },
	// XGUnswizzleRect
    /*{
        (OOVPA*)&XGUnswizzleRect_1_0_3911,

        XTL::EmuXGUnswizzleRect,

        #ifdef _DEBUG_TRACE
        "EmuXGUnswizzleRect"
        #endif
    },*/
    // XGSwizzleBox (* unchanged since 3911 *)
    {
        (OOVPA*)&XGSwizzleBox_1_0_3911,

        XTL::EmuXGSwizzleBox,

        #ifdef _DEBUG_TRACE
        "EmuXGSwizzleBox"
        #endif
    },
	// XGWriteSurfaceOrTextureToXPR
    /*{
        (OOVPA*)&XGWriteSurfaceOrTextureToXPR_1_0_3911,

        XTL::EmuXGWriteSurfaceOrTextureToXPR,

        #ifdef _DEBUG_TRACE
        "EmuXGWriteSurfaceOrTextureToXPR"
        #endif
    },*/
};

// ******************************************************************
// * XG_1_0_3911_SIZE
// ******************************************************************
uint32 XG_1_0_4034_SIZE = sizeof(XG_1_0_4034);