// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->XG.1.0.4361.cpp
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
SOOVPA<12> XGIsSwizzledFormat_1_0_4361 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // RtlCreateHeap+0x04 : cmp eax, 0x2D
        { 0x04, 0x83 }, // (Offset,Value)-Pair #1
        { 0x05, 0xF8 }, // (Offset,Value)-Pair #2
        { 0x06, 0x2D }, // (Offset,Value)-Pair #3

        // RtlCreateHeap+0x0C : jge +0x31
        { 0x0C, 0x7D }, // (Offset,Value)-Pair #4
        { 0x0D, 0x31 }, // (Offset,Value)-Pair #5

        // RtlCreateHeap+0x17 : cmp eax, 0x0B
        { 0x17, 0x83 }, // (Offset,Value)-Pair #6
        { 0x18, 0xF8 }, // (Offset,Value)-Pair #7
        { 0x19, 0x0B }, // (Offset,Value)-Pair #8

        // RtlCreateHeap+0x1A : jz +0x23
        { 0x1A, 0x74 }, // (Offset,Value)-Pair #9
        { 0x1B, 0x23 }, // (Offset,Value)-Pair #10

        // RtlCreateHeap+0x42 : jmp +0xE4
        { 0x42, 0xEB }, // (Offset,Value)-Pair #11
        { 0x43, 0xE4 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * XG_1_0_4361
// ******************************************************************
OOVPATable XG_1_0_4361[] =
{
    // XGIsSwizzledFormat
    {
        (OOVPA*)&XGIsSwizzledFormat_1_0_4361,

        xg::EmuXGIsSwizzledFormat,

        #ifdef _DEBUG_TRACE
        "XGIsSwizzledFormat"
        #endif
    },
};

// ******************************************************************
// * XG_1_0_4361_SIZE
// ******************************************************************
uint32 XG_1_0_4361_SIZE = sizeof(XG_1_0_4361);
