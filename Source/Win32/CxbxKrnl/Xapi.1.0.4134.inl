// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->Xapi.1.0.4134.cpp
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
// * SetThreadPriority
// ******************************************************************
SOOVPA<10> SetThreadPriority_1_0_4134 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // SetThreadPriority+0x0D : push [ebp+0x08]
        { 0x0D, 0xFF }, // (Offset,Value)-Pair #1
        { 0x0E, 0x75 }, // (Offset,Value)-Pair #2
        { 0x0F, 0x08 }, // (Offset,Value)-Pair #3

        // SetThreadPriority+0x18 : jl +0x2C
        { 0x18, 0x7C }, // (Offset,Value)-Pair #4
        { 0x19, 0x2C }, // (Offset,Value)-Pair #5

        // SetThreadPriority+0x22 : push 0x10
        { 0x22, 0x6A }, // (Offset,Value)-Pair #6
        { 0x23, 0x10 }, // (Offset,Value)-Pair #7

        // SetThreadPriority+0x26 : cmp eax, 0xFFFFFFF1
        { 0x26, 0x83 }, // (Offset,Value)-Pair #8
        { 0x27, 0xF8 }, // (Offset,Value)-Pair #9
        { 0x28, 0xF1 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * XMountUtilityDrive
// ******************************************************************
SOOVPA<10> XMountUtilityDrive_1_0_4134 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // XMountUtilityDrive+0x03 : sub esp, 0x0114
        { 0x03, 0x81 }, // (Offset,Value)-Pair #1
        { 0x04, 0xEC }, // (Offset,Value)-Pair #2
        { 0x05, 0x14 }, // (Offset,Value)-Pair #3
        { 0x06, 0x01 }, // (Offset,Value)-Pair #4

        // XMountUtilityDrive+0x45 : push 0x0104
        { 0x45, 0x68 }, // (Offset,Value)-Pair #5
        { 0x46, 0x04 }, // (Offset,Value)-Pair #6
        { 0x47, 0x01 }, // (Offset,Value)-Pair #7

        // XMountUtilityDrive+0xAA : lea eax, [ebp-0x10]
        { 0xAA, 0x8D }, // (Offset,Value)-Pair #8
        { 0xAB, 0x45 }, // (Offset,Value)-Pair #9
        { 0xAC, 0xF0 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * XAPI_1_0_4134
// ******************************************************************
OOVPATable XAPI_1_0_4134[] =
{
    //* Too High Level
    // XapiInitProcess (* unchanged since 3911 *)
    {
        (OOVPA*)&XapiInitProcess_1_0_3911,

        XTL::EmuXapiInitProcess,

        #ifdef _DEBUG_TRACE
        "EmuXapiInitProcess" 
        #endif
    },
    //*/
    // SetThreadPriority
    {
        (OOVPA*)&SetThreadPriority_1_0_4134,

        XTL::EmuSetThreadPriority,

        #ifdef _DEBUG_TRACE
        "EmuSetThreadPriority"
        #endif
    },
    // XMountUtilityDrive
    {
        (OOVPA*)&XMountUtilityDrive_1_0_4134,

        XTL::EmuXMountUtilityDrive,

        #ifdef _DEBUG_TRACE
        "EmuXMountUtilityDrive" 
        #endif
    }
};

// ******************************************************************
// * XAPI_1_0_4134_SIZE
// ******************************************************************
uint32 XAPI_1_0_4134_SIZE = sizeof(XAPI_1_0_4134);
