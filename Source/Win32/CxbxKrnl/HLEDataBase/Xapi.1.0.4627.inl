// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->Xapi.1.0.4627.cpp
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
// * RtlFreeHeap
// ******************************************************************
SOOVPA<9> RtlFreeHeap_1_0_4627 =
{
    0,  // Large == 0
    9,  // Count == 9

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // RtlFreeHeap+0x1F : test edi, edi
        { 0x1F, 0x85 }, // (Offset,Value)-Pair #1
        { 0x20, 0xFF }, // (Offset,Value)-Pair #2

        // RtlFreeHeap+0x23 : mov al, 1
        { 0x23, 0xB0 }, // (Offset,Value)-Pair #3
        { 0x24, 0x01 }, // (Offset,Value)-Pair #4

        // RtlFreeHeap+0x35 : mov eax, fs:[0x20]
        { 0x35, 0x64 }, // (Offset,Value)-Pair #5
        { 0x36, 0xA1 }, // (Offset,Value)-Pair #6
        { 0x37, 0x20 }, // (Offset,Value)-Pair #7

        // RtlFreeHeap+0x49 : push 0x0B
        { 0x49, 0x6A }, // (Offset,Value)-Pair #8
        { 0x4A, 0x0B }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * RtlSizeHeap
// ******************************************************************
SOOVPA<11> RtlSizeHeap_1_0_4627 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // RtlSizeHeap+0x04 : mov al, [ecx-0x0B]
        { 0x04, 0x8A }, // (Offset,Value)-Pair #1
        { 0x05, 0x41 }, // (Offset,Value)-Pair #2
        { 0x06, 0xF5 }, // (Offset,Value)-Pair #3

        // RtlSizeHeap+0x07 : test al, 1
        { 0x07, 0xA8 }, // (Offset,Value)-Pair #4
        { 0x08, 0x01 }, // (Offset,Value)-Pair #5

        // RtlSizeHeap+0x14 : movzx edx, word ptr [ecx-0x10]
        { 0x14, 0x0F }, // (Offset,Value)-Pair #6
        { 0x15, 0xB7 }, // (Offset,Value)-Pair #7
        { 0x16, 0x51 }, // (Offset,Value)-Pair #8
        { 0x17, 0xF0 }, // (Offset,Value)-Pair #9

        // RtlSizeHeap+0x2A : sub eax, ecx
        { 0x2A, 0x2B }, // (Offset,Value)-Pair #10
        { 0x2B, 0xC1 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * XMountUtilityDrive
// ******************************************************************
SOOVPA<8> XMountUtilityDrive_1_0_4627 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // XapiUnknownBad1+0x51 : push 0x0104
        { 0x51, 0x68 }, // (Offset,Value)-Pair #1
        { 0x52, 0x04 }, // (Offset,Value)-Pair #2
        { 0x53, 0x01 }, // (Offset,Value)-Pair #3

        // XapiUnknownBad1+0xB3 : jnz +0x1A
        { 0xB3, 0x75 }, // (Offset,Value)-Pair #4
        { 0xB4, 0x1A }, // (Offset,Value)-Pair #5

        // XapiUnknownBad1+0xE8 : setnl al
        { 0xE8, 0x0F }, // (Offset,Value)-Pair #6
        { 0xE9, 0x9D }, // (Offset,Value)-Pair #7
        { 0xEA, 0xC0 }, // (Offset,Value)-Pair #8

        // TODO: Finish signature, toss out function call basically
    }
};

// s+
/* not necessary?
// ******************************************************************
// * XCalculateSignatureBeginEx
// ******************************************************************
SOOVPA<8> XCalculateSignatureBeginEx_1_0_4627 =
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_XAPIXCALCULATESIGNATUREBEGINEX, // XRef Is Saved
    0,                                   // XRef Not Used

    {
        // XCalculateSignatureBeginEx+0x09 : push 7Ch
        { 0x09, 0x6A },
        { 0x0A, 0x7C },

        // XCalculateSignatureBeginEx+0x2D : lea eax, [esi+5Ch]
        { 0x2D, 0x8D },
        { 0x2E, 0x46 },
        { 0x2F, 0x5C },

        // XCalculateSignatureBeginEx+0xE8 : cmp edx, 40h ; '@'
        { 0x43, 0x83 },
        { 0x44, 0xFA },
        { 0x45, 0x40 },
    }
};

// ******************************************************************
// * XCalculateSignatureBegin
// ******************************************************************
SOOVPA<12> XCalculateSignatureBegin_1_0_4627 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    1,  // XRef Is Used

    {
        // XCalculateSignatureBegin+0x0C : call [XCalculateSignatureBeginEx]
        { 0x0D, XREF_XAPIXCALCULATESIGNATUREBEGINEX },

        // XCalculateSignatureBegin+0x00 : mov eax, ds:10118h
        { 0x00, 0xA1 },
        { 0x01, 0x18 },
        { 0x02, 0x01 },
        { 0x03, 0x01 },
        { 0x04, 0x00 },

        // XCalculateSignatureBegin+0x08 : push [esp+4+arg_0]
        { 0x08, 0xFF },
        { 0x09, 0x74 },
        { 0x0A, 0x24 },
        { 0x0B, 0x08 },

        // XCalculateSignatureBegin+0x11 : retn 4
        { 0x12, 0x04 },
        { 0x13, 0x00 },
    }
};

// ******************************************************************
// * XCalculateSignatureUpdate
// ******************************************************************
SOOVPA<9> XCalculateSignatureUpdate_1_0_4627 =
{
    0,  // Large == 0
    9,  // Count == 9

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // XCalculateSignatureUpdate+0x04 : mov eax, [esp+4+arg_0]
        { 0x04, 0x8B },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },

        // XCalculateSignatureUpdate+0x0C : add eax, 8
        { 0x0C, 0x83 },
        { 0x0D, 0xC0 },
        { 0x0E, 0x08 },

        // XCalculateSignatureUpdate+0x11 : xor eax, eax
        { 0x15, 0x33 },
        { 0x16, 0xC0 },
    }
};

// ******************************************************************
// * XCalculateSignatureEnd
// ******************************************************************
SOOVPA<8> XCalculateSignatureEnd_1_0_4627 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // XCalculateSignatureEnd+0x03 : sub esp, 14h
        { 0x03, 0x83 },
        { 0x04, 0xEC },
        { 0x05, 0x14 },

        // XCalculateSignatureEnd+0x22 : push 10h
        { 0x22, 0x6A },
        { 0x23, 0x10 },

        // XCalculateSignatureEnd+0x37 : lea eax, [ebp+var_14]
        { 0x37, 0x8D },
        { 0x38, 0x45 },
        { 0x39, 0xEC },
    }
};
//*/
// +s

// ******************************************************************
// * XAPI_1_0_4627
// ******************************************************************
OOVPATable XAPI_1_0_4627[] =
{
    // RtlCreateHeap (* unchanged since 1.0.4361 *) (* OR FARTHER *)
    {
        (OOVPA*)&RtlCreateHeap_1_0_3911,

        XTL::EmuRtlCreateHeap,

        #ifdef _DEBUG_TRACE
        "EmuRtlCreateHeap"
        #endif
    },
    // RtlAllocateHeap (* unchanged since 1.0.4361 *) (* OR FARTHER *)
    {
        (OOVPA*)&RtlAllocateHeap_1_0_3911,

        XTL::EmuRtlAllocateHeap,

        #ifdef _DEBUG_TRACE
        "EmuRtlAllocateHeap"
        #endif
    },
    // RtlFreeHeap
    {
        (OOVPA*)&RtlFreeHeap_1_0_4627,

        XTL::EmuRtlFreeHeap,

        #ifdef _DEBUG_TRACE
        "EmuRtlFreeHeap"
        #endif
    },
    // RtlSizeHeap
    {
        (OOVPA*)&RtlSizeHeap_1_0_4627,

        XTL::EmuRtlSizeHeap,

        #ifdef _DEBUG_TRACE
        "EmuRtlSizeHeap"
        #endif
    },
    // XMountUtilityDrive
    {
        (OOVPA*)&XMountUtilityDrive_1_0_4627,

        XTL::EmuXMountUtilityDrive,

        #ifdef _DEBUG_TRACE
        "EmuXMountUtilityDrive"
        #endif
    },
    // XInitDevices (* unchanged since 1.0.3911 *)
    {
        (OOVPA*)&XInitDevices_1_0_3911,

        XTL::EmuXInitDevices,

        #ifdef _DEBUG_TRACE
        "EmuXInitDevices"
        #endif
    },
    // XGetDevices (* unchanged since 1.0.4134 *)
    {
        (OOVPA*)&XGetDevices_1_0_4134,

        XTL::EmuXGetDevices,

        #ifdef _DEBUG_TRACE
        "EmuXGetDevices" 
        #endif
    },
    // XInputOpen (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&XInputOpen_1_0_4361,

        XTL::EmuXInputOpen,

        #ifdef _DEBUG_TRACE
        "EmuXInputOpen"
        #endif
    },
    // XInputGetCapabilities (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&XInputGetCapabilities_1_0_4361,

        XTL::EmuXInputGetCapabilities,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetCapabilities"
        #endif
    },
    // XInputGetState (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&XInputGetState_1_0_4361,

        XTL::EmuXInputGetState,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetState"
        #endif
    },
    // XInputSetState (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&XInputSetState_1_0_4361,

        XTL::EmuXInputSetState,

        #ifdef _DEBUG_TRACE
        "EmuXInputSetState"
        #endif
    },
    // XapiThreadStartup (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&XapiThreadStartup_1_0_4361,

        XTL::EmuXapiThreadStartup,

        #ifdef _DEBUG_TRACE
        "EmuXapiThreadStartup"
        #endif
    },
    //* Too High Level
    // XapiInitProcess (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&XapiInitProcess_1_0_4361,

        XTL::EmuXapiInitProcess,

        #ifdef _DEBUG_TRACE
        "EmuXapiInitProcess" 
        #endif
    },
    //*/
    // XapiBootToDash (* unchanged since 1.0.3911 *)
    {
        (OOVPA*)&XapiBootDash_1_0_3911,

        XTL::EmuXapiBootDash,

        #ifdef _DEBUG_TRACE
        "EmuXapiBootDash"
        #endif
    },
    // +s
    /* not necessary?
    // XCalculateSignatureBeginEx
    {
        (OOVPA*)&XCalculateSignatureBeginEx_1_0_4627,

        XTL::EmuXCalculateSignatureBeginEx,

        #ifdef _DEBUG_TRACE
        "EmuXCalculateSignatureBeginEx (XREF&FUNC)"
        #endif
    },
    // XCalculateSignatureBegin
    {
        (OOVPA*)&XCalculateSignatureBegin_1_0_4627,

        XTL::EmuXCalculateSignatureBegin,

        #ifdef _DEBUG_TRACE
        "EmuXCalculateSignatureBegin"
        #endif
    },
    // XCalculateSignatureUpdate
    {
        (OOVPA*)&XCalculateSignatureUpdate_1_0_4627,

        XTL::EmuXCalculateSignatureUpdate,

        #ifdef _DEBUG_TRACE
        "EmuXCalculateSignatureUpdate"
        #endif
    },
    // XCalculateSignatureEnd
    {
        (OOVPA*)&XCalculateSignatureEnd_1_0_4627,

        XTL::EmuXCalculateSignatureEnd,

        #ifdef _DEBUG_TRACE
        "EmuXCalculateSignatureEnd"
        #endif
    },
    //*/
    // s+
};

// ******************************************************************
// * XAPI_1_0_4627_SIZE
// ******************************************************************
uint32 XAPI_1_0_4627_SIZE = sizeof(XAPI_1_0_4627);
