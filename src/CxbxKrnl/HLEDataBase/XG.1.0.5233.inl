// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->XG.1.0.5233.cpp
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
// * XGUnswizzleRect
// ******************************************************************
SOOVPA<8> XGUnswizzleRect_1_0_5233 =
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
// * XG_1_0_5233
// ******************************************************************
OOVPATable XG_1_0_5233[] =
{
    // XGIsSwizzledFormat (* unchanged since 4361 *)
    {
        (OOVPA*)&XGIsSwizzledFormat_1_0_4361,

        XTL::EmuXGIsSwizzledFormat,

        #ifdef _DEBUG_TRACE
        "XGIsSwizzledFormat"
        #endif
    },
    // XGSwizzleRect (* unchanged since 4361 *)
    {
        (OOVPA*)&XGSwizzleRect_1_0_4361,

        XTL::EmuXGSwizzleRect,

        #ifdef _DEBUG_TRACE
        "EmuXGSwizzleRect"
        #endif
    },
    // XGUnswizzleRect
    {
        (OOVPA*)&XGUnswizzleRect_1_0_5233,

        XTL::EmuXGUnswizzleRect,

        #ifdef _DEBUG_TRACE
        "EmuXGUnswizzleRect"
        #endif
    },
};

// ******************************************************************
// * XG_1_0_5233_SIZE
// ******************************************************************
uint32 XG_1_0_5233_SIZE = sizeof(XG_1_0_5233);
