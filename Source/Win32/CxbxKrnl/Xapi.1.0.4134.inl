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
// * GetExitCodeThread
// ******************************************************************
SOOVPA<11> GetExitCodeThread_1_0_4134 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // GetExitCodeThread+0x03 : lea eax, [ebp+0x08]
        { 0x03, 0x8D }, // (Offset,Value)-Pair #1
        { 0x04, 0x45 }, // (Offset,Value)-Pair #2
        { 0x05, 0x08 }, // (Offset,Value)-Pair #3

        // GetExitCodeThread+0x18 : mov ecx, dword ptr [ebp+0x08]
        { 0x1A, 0x8B }, // (Offset,Value)-Pair #4
        { 0x1B, 0x4D }, // (Offset,Value)-Pair #5
        { 0x1C, 0x08 }, // (Offset,Value)-Pair #6

        // GetExitCodeThread+0x2B : mov eax, 0x0103
        { 0x2B, 0xB8 }, // (Offset,Value)-Pair #7
        { 0x2C, 0x03 }, // (Offset,Value)-Pair #8
        { 0x2D, 0x01 }, // (Offset,Value)-Pair #9

        // GetExitCodeThread+0x49 : retn 0x08
        { 0x49, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x4A, 0x08 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * XGetDevices
// ******************************************************************
SOOVPA<14> XGetDevices_1_0_4134 =
{
    0,  // Large == 0
    14, // Count == 14

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // XGetDevices+0x07 : mov edx, [esp+arg_0]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x54 }, // (Offset,Value)-Pair #2
        { 0x09, 0x24 }, // (Offset,Value)-Pair #3
        { 0x0A, 0x08 }, // (Offset,Value)-Pair #4

        // XGetDevices+0x0D : and [edx+4], 0
        { 0x0D, 0x83 }, // (Offset,Value)-Pair #5
        { 0x0E, 0x62 }, // (Offset,Value)-Pair #6
        { 0x0F, 0x04 }, // (Offset,Value)-Pair #7
        { 0x10, 0x00 }, // (Offset,Value)-Pair #8

        // XGetDevices+0x11 : mov cl, al
        { 0x11, 0x8A }, // (Offset,Value)-Pair #9
        { 0x12, 0xC8 }, // (Offset,Value)-Pair #10

        // XGetDevices+0x16 : call KfLowerIrql
        { 0x16, 0xFF }, // (Offset,Value)-Pair #11
        { 0x17, 0x15 }, // (Offset,Value)-Pair #12

        // XGetDevices+0x1F : retn 4
        { 0x1F, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x20, 0x04 }, // (Offset,Value)-Pair #14
    }
};

// ******************************************************************
// * XInputOpen
// ******************************************************************
SOOVPA<12> XInputOpen_1_0_4134 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // XInputOpen+0x13 : jmp +0x18
        { 0x13, 0xEB }, // (Offset,Value)-Pair #1
        { 0x14, 0x18 }, // (Offset,Value)-Pair #2

        // XInputOpen+0x4A : add edx, 0x10
        { 0x4A, 0x83 }, // (Offset,Value)-Pair #3
        { 0x4B, 0xC2 }, // (Offset,Value)-Pair #4
        { 0x4C, 0x10 }, // (Offset,Value)-Pair #5

        // XInputOpen+0x5B : jnz +0x06
        { 0x5B, 0x75 }, // (Offset,Value)-Pair #6
        { 0x5C, 0x06 }, // (Offset,Value)-Pair #7

        // XInputOpen+0x68 : push 0x57
        { 0x68, 0x6A }, // (Offset,Value)-Pair #8
        { 0x69, 0x57 }, // (Offset,Value)-Pair #9

        // XInputOpen+0x71 : leave
        { 0x71, 0xC9 }, // (Offset,Value)-Pair #10

        // XInputOpen+0x72 : retn 0x10
        { 0x72, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x73, 0x10 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * XInputGetState
// ******************************************************************
SOOVPA<12> XInputGetState_1_0_4134 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // XInputGetState+0x0E : cmp byte ptr [edx+0xA3], 1
        { 0x0E, 0x80 }, // (Offset,Value)-Pair #1
        { 0x0F, 0xBA }, // (Offset,Value)-Pair #2
        { 0x10, 0xA3 }, // (Offset,Value)-Pair #3

        // XInputGetState+0x17 : push 0x57
        { 0x17, 0x6A }, // (Offset,Value)-Pair #4
        { 0x18, 0x57 }, // (Offset,Value)-Pair #5

        // XInputGetState+0x1A : jmp +0x46
        { 0x1A, 0xEB }, // (Offset,Value)-Pair #6
        { 0x1B, 0x46 }, // (Offset,Value)-Pair #7

        // XInputGetState+0x28 : mov ebx, 0x048F
        { 0x28, 0xBB }, // (Offset,Value)-Pair #8
        { 0x29, 0x8F }, // (Offset,Value)-Pair #9
        { 0x2A, 0x04 }, // (Offset,Value)-Pair #10

        // XInputGetState+0x6E : retn 8
        { 0x6E, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x6F, 0x08 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * XInputSetState
// ******************************************************************
SOOVPA<12> XInputSetState_1_0_4134 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // XInputSetState+0x04 : lea eax, [ecx+0x0A3]
        { 0x04, 0x8D }, // (Offset,Value)-Pair #1
        { 0x05, 0x81 }, // (Offset,Value)-Pair #2
        { 0x06, 0xA3 }, // (Offset,Value)-Pair #3

        // XInputSetState+0x0F : push 0x57
        { 0x0F, 0x6A }, // (Offset,Value)-Pair #4
        { 0x10, 0x57 }, // (Offset,Value)-Pair #5

        // XInputSetState+0x12 : jmp +0x21
        { 0x12, 0xEB }, // (Offset,Value)-Pair #6
        { 0x13, 0x21 }, // (Offset,Value)-Pair #7

        // XInputSetState+0x2D : mov [edx+0x41], al
        { 0x2D, 0x88 }, // (Offset,Value)-Pair #8
        { 0x2E, 0x42 }, // (Offset,Value)-Pair #9
        { 0x2F, 0x41 }, // (Offset,Value)-Pair #10

        // XInputSetState+0x35 : retn 0x08
        { 0x35, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x36, 0x08 }, // (Offset,Value)-Pair #12
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
    // XInitDevices (* unchanged since 3911 *)
    {
        (OOVPA*)&XInitDevices_1_0_3911,

        XTL::EmuXInitDevices,

        #ifdef _DEBUG_TRACE
        "EmuXInitDevices"
        #endif
    },
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
    // SetThreadPriority (* unchanged since 3911 *)
    {
        (OOVPA*)&SetThreadPriority_1_0_3911,

        XTL::EmuSetThreadPriority,

        #ifdef _DEBUG_TRACE
        "EmuSetThreadPriority"
        #endif
    },
    // GetExitCodeThread
    {
        (OOVPA*)&GetExitCodeThread_1_0_4134,

        XTL::EmuGetExitCodeThread,

        #ifdef _DEBUG_TRACE
        "EmuGetExitCodeThread"
        #endif
    },
    // XGetDevices
    {
        (OOVPA*)&XGetDevices_1_0_4134,

        XTL::EmuXGetDevices,

        #ifdef _DEBUG_TRACE
        "EmuXGetDevices"
        #endif
    },
    // XInputOpen
    {
        (OOVPA*)&XInputOpen_1_0_4134,

        XTL::EmuXInputOpen,

        #ifdef _DEBUG_TRACE
        "EmuXInputOpen"
        #endif
    },
    // XInputGetState
    {
        (OOVPA*)&XInputGetState_1_0_4134,

        XTL::EmuXInputGetState,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetState"
        #endif
    },
    // XInputSetState
    {
        (OOVPA*)&XInputSetState_1_0_4134,

        XTL::EmuXInputSetState,

        #ifdef _DEBUG_TRACE
        "EmuXInputSetState"
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
