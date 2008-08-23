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

// NOTE: This function actually came in with 4928
// ******************************************************************
// * XapiApplyKernelPatches
// ******************************************************************
SOOVPA<7> XapiApplyKernelPatches_1_0_4928 =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x17, 0x81 },
        { 0x2A, 0x81 },
        { 0x4B, 0x05 },
        { 0x5F, 0x1B },
        { 0x7C, 0xFF },
        { 0x88, 0x6A },
        { 0x92, 0xC7 },
    }
};

// NOTE: This function actually came in with 4928
// ******************************************************************
// * XapiInitProcess
// ******************************************************************
SOOVPA<7> XapiInitProcess_1_0_4928 =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        { 0x22, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x23, 0x45 }, // (Offset,Value)-Pair #2
        { 0x24, 0xCC }, // (Offset,Value)-Pair #3
        { 0x25, 0x30 }, // (Offset,Value)-Pair #4
        { 0x4A, 0xA1 }, // (Offset,Value)-Pair #5
        { 0x4B, 0x18 }, // (Offset,Value)-Pair #6
        { 0x4C, 0x01 }, // (Offset,Value)-Pair #7
    }
};

// ******************************************************************
// * XFormatUtilityDrive
// ******************************************************************
SOOVPA<7> XFormatUtilityDrive_1_0_4627 =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x10, 0x50 },
        { 0x23, 0xFF },
        { 0x34, 0xC0 },
        { 0x46, 0x45 },
        { 0x58, 0xFF },
        { 0x6D, 0x33 },
        { 0x7C, 0x40 },
    }
};

// ******************************************************************
// * FindFirstFileA
// ******************************************************************
SOOVPA<8> FindFirstFileA_1_0_4627 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x1E, 0x89 },
        { 0x3E, 0x74 },
        { 0x5E, 0x45 },
        { 0x7E, 0x75 },
        { 0x9E, 0x8D },
        { 0xBE, 0xFE },
        { 0xE1, 0x56 },
        { 0xFE, 0x57 },
    }
};

// ******************************************************************
// * FindNextFileA
// ******************************************************************
SOOVPA<7> FindNextFileA_1_0_4627 =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x09, 0x56 },
        { 0x14, 0x00 },
        { 0x1F, 0x50 },
        { 0x2C, 0x3B },
        { 0x37, 0x33 },
        { 0x40, 0xB0 },
        { 0x4B, 0xC0 },
    }
};

// ******************************************************************
// * SetThreadPriorityBoost
// ******************************************************************
SOOVPA<10> SetThreadPriorityBoost_1_0_4627 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // SetThreadPriorityBoost+0x0D : push [ebp+0x08]
        { 0x0D, 0xFF }, // (Offset,Value)-Pair #1
        { 0x0E, 0x75 }, // (Offset,Value)-Pair #2
        { 0x0F, 0x08 }, // (Offset,Value)-Pair #3

        // SetThreadPriorityBoost+0x18 : jl +0x20
        { 0x18, 0x7C }, // (Offset,Value)-Pair #4
        { 0x19, 0x20 }, // (Offset,Value)-Pair #5

        // SetThreadPriorityBoost+0x1F : setnz al
        { 0x1F, 0x0F }, // (Offset,Value)-Pair #6
        { 0x20, 0x95 }, // (Offset,Value)-Pair #7
        { 0x21, 0xC0 }, // (Offset,Value)-Pair #8

        // SetThreadPriorityBoost+0x2C : mov ecx, [ebp+0x08]
        { 0x2C, 0x8B }, // (Offset,Value)-Pair #9
        { 0x2D, 0x4D }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * GetThreadPriority
// ******************************************************************
SOOVPA<10> GetThreadPriority_1_0_4627 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // GetThreadPriority+0x0D : push [ebp+0x08]
        { 0x0D, 0xFF }, // (Offset,Value)-Pair #1
        { 0x0E, 0x75 }, // (Offset,Value)-Pair #2
        { 0x0F, 0x08 }, // (Offset,Value)-Pair #3

        // GetThreadPriority+0x18 : jl +0x2B
        { 0x18, 0x7C }, // (Offset,Value)-Pair #4
        { 0x19, 0x2B }, // (Offset,Value)-Pair #5

        // GetThreadPriority+0x2F : cmp esi, 0xFFFFFFF0
        { 0x2F, 0x83 }, // (Offset,Value)-Pair #6
        { 0x30, 0xFE }, // (Offset,Value)-Pair #7
        { 0x31, 0xF0 }, // (Offset,Value)-Pair #8

        // GetThreadPriority+0x37 : mov ecx, [ebp+0x08]
        { 0x37, 0x8B }, // (Offset,Value)-Pair #9
        { 0x38, 0x4D }, // (Offset,Value)-Pair #10
    }
};

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
// * RtlReAllocateHeap
// ******************************************************************
SOOVPA<10> RtlReAllocateHeap_1_0_4627 =
{
    0,  // Large == 0
    10,  // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // RtlReAllocateHeap+0x1C : test esi, esi
        { 0x1C, 0x85 }, // (Offset,Value)-Pair #1
        { 0x1D, 0xF6 }, // (Offset,Value)-Pair #2

        // RtlReAllocateHeap+0x26 : or [ebp+0x0C], ecx
        { 0x26, 0x09 }, // (Offset,Value)-Pair #3
        { 0x27, 0x4D }, // (Offset,Value)-Pair #4
        { 0x28, 0x0C }, // (Offset,Value)-Pair #5

        // RtlReAllocateHeap+0x61 : jz +0x06; add ecx, 0x10
        { 0x61, 0x74 }, // (Offset,Value)-Pair #6
        { 0x62, 0x06 }, // (Offset,Value)-Pair #7
        { 0x63, 0x83 }, // (Offset,Value)-Pair #8
        { 0x64, 0xC1 }, // (Offset,Value)-Pair #9
        { 0x65, 0x10 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * XID_fCloseDevice
// ******************************************************************
SOOVPA<11> XID_fCloseDevice_1_0_4928 =
{
    0,  // Large == 0
    11, // Count == 11

    XREF_FCLOSEDEVICE,  // XRef Is  Saved
    0,                  // XRef Not Used

    {
        { 0x13, 0x8B }, // (Offset,Value)-Pair #1
        { 0x14, 0x86 }, // (Offset,Value)-Pair #2
        { 0x15, 0xA3 }, // (Offset,Value)-Pair #3

        { 0x4C, 0x89 }, // (Offset,Value)-Pair #4
        { 0x4D, 0x86 }, // (Offset,Value)-Pair #5
        { 0x4E, 0x9E }, // (Offset,Value)-Pair #6

        { 0x87, 0x89 }, // (Offset,Value)-Pair #7
        { 0x88, 0x86 }, // (Offset,Value)-Pair #8
        { 0x89, 0xA7 }, // (Offset,Value)-Pair #9

        { 0x95, 0xC9 }, // (Offset,Value)-Pair #10
        { 0x96, 0xC3 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * XInputClose
// ******************************************************************
SOOVPA<8> XInputClose_1_0_4928 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // XRef Not Saved
    1,  // XRef Is Used

    {
        // XInputClose+0x05 : call [fCloseDevice]
        { 0x05, XREF_FCLOSEDEVICE },  // (Offset,Value)-Pair #1

        // XInputClose+0x00 : mov ecx, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #2
        { 0x01, 0x4C }, // (Offset,Value)-Pair #3
        { 0x02, 0x24 }, // (Offset,Value)-Pair #4
        { 0x03, 0x04 }, // (Offset,Value)-Pair #5

        // XInputClose+0x04 : call [fCloseDevice]
        { 0x04, 0xE8 }, // (Offset,Value)-Pair #6

        // XInputClose+0x09 : retn 0x04
        { 0x09, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x0A, 0x04 }, // (Offset,Value)-Pair #8
    }
};

// ******************************************************************
// * XInputPoll
// ******************************************************************
SOOVPA<10> XInputPoll_1_0_4928 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Is Used

    {
        { 0x16, 0xF6 }, // (Offset,Value)-Pair #1
        { 0x17, 0x41 }, // (Offset,Value)-Pair #2
        { 0x18, 0x04 }, // (Offset,Value)-Pair #3
        { 0x19, 0x02 }, // (Offset,Value)-Pair #4

        { 0x25, 0x39 }, // (Offset,Value)-Pair #5
        { 0x26, 0x70 }, // (Offset,Value)-Pair #6
        { 0x27, 0x04 }, // (Offset,Value)-Pair #7

        { 0x3A, 0x83 }, // (Offset,Value)-Pair #8
        { 0x3B, 0xC0 }, // (Offset,Value)-Pair #9
        { 0x3C, 0x52 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * XInputGetState
// ******************************************************************
SOOVPA<12> XInputGetState_1_0_4928 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        { 0x0E, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0F, 0x8A }, // (Offset,Value)-Pair #2
        { 0x10, 0xA3 }, // (Offset,Value)-Pair #3

        { 0x1A, 0x6A }, // (Offset,Value)-Pair #4
        { 0x1B, 0x57 }, // (Offset,Value)-Pair #5

        { 0x1D, 0xEB }, // (Offset,Value)-Pair #6
        { 0x1E, 0x3E }, // (Offset,Value)-Pair #7

        { 0x3A, 0x8B }, // (Offset,Value)-Pair #8
        { 0x3B, 0x8A }, // (Offset,Value)-Pair #9
        { 0x3C, 0xA3 }, // (Offset,Value)-Pair #10

        { 0x69, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x6A, 0x08 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * XInputGetCapabilities
// ******************************************************************
SOOVPA<12> XInputGetCapabilities_1_0_4928 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        { 0x0B, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0C, 0x1D }, // (Offset,Value)-Pair #2
        { 0x0D, 0xAC }, // (Offset,Value)-Pair #3

        { 0x25, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x26, 0x46 }, // (Offset,Value)-Pair #5
        { 0x27, 0x04 }, // (Offset,Value)-Pair #6
        { 0x28, 0x02 }, // (Offset,Value)-Pair #7

        { 0x39, 0xF3 }, // (Offset,Value)-Pair #8
        { 0x3A, 0xAB }, // (Offset,Value)-Pair #9

        { 0x90, 0xBF }, // (Offset,Value)-Pair #10
        { 0x91, 0xE8 }, // (Offset,Value)-Pair #11
        { 0x92, 0x7B }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * XAPI_1_0_4627
// ******************************************************************
OOVPATable XAPI_1_0_4627[] =
{
    // XapiApplyKernelPatches
    {
        (OOVPA*)&XapiApplyKernelPatches_1_0_4928,

        XTL::EmuXapiApplyKernelPatches,

        #ifdef _DEBUG_TRACE
        "EmuXapiApplyKernelPatches"
        #endif
    },
    // XapiInitProcess
    {
        (OOVPA*)&XapiInitProcess_1_0_4928,

        XTL::EmuXapiInitProcess,

        #ifdef _DEBUG_TRACE
        "EmuXapiInitProcess"
        #endif
    },
    // XFormatUtilityDrive
    {
        (OOVPA*)&XFormatUtilityDrive_1_0_4627,

        XTL::EmuXFormatUtilityDrive,

        #ifdef _DEBUG_TRACE
        "EmuXFormatUtilityDrive"
        #endif
    },
    /* ended up not fixing anything in panzer dragoon!
    // FindFirstFileA
    {
        (OOVPA*)&FindFirstFileA_1_0_4627,

        XTL::EmuFindFirstFileA,

        #ifdef _DEBUG_TRACE
        "EmuFindFirstFileA"
        #endif
    },
    // FindNextFileA
    {
        (OOVPA*)&FindNextFileA_1_0_4627,

        XTL::EmuFindNextFileA,

        #ifdef _DEBUG_TRACE
        "EmuFindNextFileA"
        #endif
    },
    //*/
    // SetThreadPriorityBoost
    {
        (OOVPA*)&SetThreadPriorityBoost_1_0_4627,

        XTL::EmuSetThreadPriorityBoost,

        #ifdef _DEBUG_TRACE
        "EmuSetThreadPriorityBoost"
        #endif
    },
    // SetThreadPriority (* unchanged since 3911 *)
    {
        (OOVPA*)&SetThreadPriority_1_0_3911,

        XTL::EmuSetThreadPriority,

        #ifdef _DEBUG_TRACE
        "EmuSetThreadPriority"
        #endif
    },
    // GetThreadPriority
    {
        (OOVPA*)&GetThreadPriority_1_0_4627,

        XTL::EmuGetThreadPriority,

        #ifdef _DEBUG_TRACE
        "EmuGetThreadPriority"
        #endif
    },
    // XRegisterThreadNotifyRoutine (* unchanged since 3911 *)
    {
        (OOVPA*)&XRegisterThreadNotifyRoutine_1_0_3911,

        XTL::EmuXRegisterThreadNotifyRoutine,

        #ifdef _DEBUG_TRACE
        "EmuXRegisterThreadNotifyRoutine"
        #endif
    },
    // RtlCreateHeap (* unchanged since 1.0.3911 *)
    {
        (OOVPA*)&RtlCreateHeap_1_0_3911,

        XTL::EmuRtlCreateHeap,

        #ifdef _DEBUG_TRACE
        "EmuRtlCreateHeap"
        #endif
    },
    // RtlAllocateHeap (* unchanged since 1.0.3911 *)
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
    // RtlReAllocateHeap
    {
        (OOVPA*)&RtlReAllocateHeap_1_0_4627,

        XTL::EmuRtlReAllocateHeap,

        #ifdef _DEBUG_TRACE
        "EmuRtlReAllocateHeap"
        #endif
    },
    // RtlSizeHeap (* unchanged since 4432 *)
    {
        (OOVPA*)&RtlSizeHeap_1_0_4432,

        XTL::EmuRtlSizeHeap,

        #ifdef _DEBUG_TRACE
        "EmuRtlSizeHeap"
        #endif
    },
    // QueryPerformanceCounter (* unchanged since 4361 *)
    {
        (OOVPA*)&QueryPerformanceCounter_1_0_4361,

        XTL::EmuQueryPerformanceCounter,

        #ifdef _DEBUG_TRACE
        "EmuQueryPerformanceCounter"
        #endif
    },
    // QueryPerformanceFrequency (* unchanged since 4361 *)
    {
        (OOVPA*)&QueryPerformanceFrequency_1_0_4361,

        XTL::EmuQueryPerformanceFrequency,

        #ifdef _DEBUG_TRACE
        "EmuQueryPerformanceFrequency"
        #endif
    },
    // XMountUtilityDrive (* unchanged since 4432 *)
    {
        (OOVPA*)&XMountUtilityDrive_1_0_4432,

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
    // XInputPoll
    {
        (OOVPA*)&XInputPoll_1_0_4928,

        XTL::EmuXInputPoll,

        #ifdef _DEBUG_TRACE
        "EmuXInputPoll"
        #endif
    },
    // XID_fCloseDevice
    {
        (OOVPA*)&XID_fCloseDevice_1_0_4928, 0,

        #ifdef _DEBUG_TRACE
        "XID_fCloseDevice (XREF)"
        #endif
    },
    // XInputClose
    {
        (OOVPA*)&XInputClose_1_0_4928,

        XTL::EmuXInputClose,

        #ifdef _DEBUG_TRACE
        "EmuXInputClose"
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
    // XInputGetState
    {
        (OOVPA*)&XInputGetState_1_0_4928,

        XTL::EmuXInputGetState,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetState"
        #endif
    },
    // XInputGetCapabilities
    {
        (OOVPA*)&XInputGetCapabilities_1_0_4928,

        XTL::EmuXInputGetCapabilities,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetCapabilities"
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
    /* obsolete?
    // XapiThreadStartup (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&XapiThreadStartup_1_0_4361,

        XTL::EmuXapiThreadStartup,

        #ifdef _DEBUG_TRACE
        "EmuXapiThreadStartup"
        #endif
    },
    //*/
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
