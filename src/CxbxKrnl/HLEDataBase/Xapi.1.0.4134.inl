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
// * QueryPerformanceCounter
// ******************************************************************
SOOVPA<12> QueryPerformanceCounter_1_0_4134 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // QueryPerformanceCounter+0x04 : rdtsc
        { 0x04, 0x0F }, // (Offset,Value)-Pair #1
        { 0x05, 0x31 }, // (Offset,Value)-Pair #2

        // QueryPerformanceCounter+0x06 : mov [ecx], eax
        { 0x06, 0x89 }, // (Offset,Value)-Pair #3
        { 0x07, 0x01 }, // (Offset,Value)-Pair #4

        // QueryPerformanceCounter+0x08 : mov [ecx+4], eax
        { 0x08, 0x89 }, // (Offset,Value)-Pair #5
        { 0x09, 0x51 }, // (Offset,Value)-Pair #6
        { 0x0A, 0x04 }, // (Offset,Value)-Pair #7

        // QueryPerformanceCounter+0x0B : xor eax, eax
        { 0x0B, 0x33 }, // (Offset,Value)-Pair #8
        { 0x0C, 0xC0 }, // (Offset,Value)-Pair #9

        // QueryPerformanceCounter+0x0D : inc eax
        { 0x0D, 0x40 }, // (Offset,Value)-Pair #10

        // QueryPerformanceCounter+0x0E : retn 0x04
        { 0x0E, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x0F, 0x04 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * QueryPerformanceFrequency
// ******************************************************************
SOOVPA<15> QueryPerformanceFrequency_1_0_4134 =
{
    0,  // Large == 0
    15, // Count == 15

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // QueryPerformanceFrequency+0x04 : and dword ptr [eax+4], 0
        { 0x04, 0x83 }, // (Offset,Value)-Pair #1
        { 0x05, 0x60 }, // (Offset,Value)-Pair #2
        { 0x06, 0x04 }, // (Offset,Value)-Pair #3
        { 0x07, 0x00 }, // (Offset,Value)-Pair #4

        // QueryPerformanceFrequency+0x06 : mov dword ptr [eax], 0x2BB5C755
        { 0x08, 0xC7 }, // (Offset,Value)-Pair #5
        { 0x09, 0x00 }, // (Offset,Value)-Pair #6
        { 0x0A, 0x55 }, // (Offset,Value)-Pair #7
        { 0x0B, 0xC7 }, // (Offset,Value)-Pair #8
        { 0x0C, 0xB5 }, // (Offset,Value)-Pair #9
        { 0x0D, 0x2B }, // (Offset,Value)-Pair #10

        // QueryPerformanceFrequency+0x0E : xor eax, eax
        { 0x0E, 0x33 }, // (Offset,Value)-Pair #11
        { 0x0F, 0xC0 }, // (Offset,Value)-Pair #12

        // QueryPerformanceFrequency+0x10 : inc eax
        { 0x10, 0x40 }, // (Offset,Value)-Pair #13

        // QueryPerformanceFrequency+0x11 : retn 0x04
        { 0x11, 0xC2 }, // (Offset,Value)-Pair #14
        { 0x12, 0x04 }, // (Offset,Value)-Pair #15
    }
};

// ******************************************************************
// * XInputPoll
// ******************************************************************
SOOVPA<10> XInputPoll_1_0_4134 =
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
// * XGetSectionHandleA
// ******************************************************************
SOOVPA<13> XGetSectionHandleA_1_0_4134 = 
{
	0,	// Large == 0
	13,	// Count == 0

	-1,	// XRef not saved
	0,	// XRef not used

	{
		// XGetSectionaHandleA+0x03 : sub esp, 0x10
		{ 0x03, 0x83 },
		{ 0x04, 0xEC },
		{ 0x05, 0x10 },

		// XGetSectionaHandleA+0x24 : imul esi, 0x38
		{ 0x24, 0x6B },
		{ 0x25, 0xF6 },
		{ 0x26, 0x38 },

		// XGetSectionaHandleA+0x4F : push 0x490
		{ 0x4F, 0x68 },
		{ 0x50, 0x90 },
		{ 0x51, 0x04 },
		{ 0x52, 0x00 },
		{ 0x53, 0x00 },

		// XGetSectionaHandleA+0x60 : retn 4
		{ 0x60, 0xC2 },
		{ 0x61, 0x04 }
	}
};

// ******************************************************************
// * XLoadSectionByHandle
// ******************************************************************
SOOVPA<11> XLoadSectionByHandle_1_0_4134 = 
{
	0,	// Large == 0
	11,	// Count == 11

	-1,	// XRef not saved
	0,	// XRef not used

	{
		// XLoadSectionByHandle+0x01 : mov esi, [esp+4+arg_0]
		{ 0x01, 0x8B },
		{ 0x02, 0x74 },
		{ 0x03, 0x24 },
		{ 0x04, 0x08 },

		// XLoadSectionByHandle+0x0C : test eax, eax
		{ 0x0C, 0x85 },
		{ 0x0D, 0xC0 },

		// XLoadSectionByHandle+0x1A : mov eax, [esi+4]
		{ 0x1A, 0x8B },
		{ 0x1B, 0x46 },
		{ 0x1C, 0x04 },

		// XLoadSectionByHandle+0x1E : retn 4
		{ 0x1E, 0xC2 },
		{ 0x1F, 0x04 }
	}
};


// ******************************************************************
// * XFreeSectionByHandle
// ******************************************************************
SOOVPA<11> XFreeSectionByHandle_1_0_4134 = 
{
	0,	// Large == 0
	11,	// Count == 11

	-1,	// XRef not saved
	0,	// XRef not used

	{
		// XFreeSectionByHandle+0x00 : push esp
		{ 0x00, 0xFF },
		{ 0x01, 0x74 },
		{ 0x02, 0x24 },
		{ 0x03, 0x04 },

		// XFreeSectionByHandle+0x0A : test eax, eax
		{ 0x0A, 0x85 },
		{ 0x0B, 0xC0 },

		// XFreeSectionByHandle+0x18 : xor eax, eax
		{ 0x18, 0x33 },
		{ 0x19, 0xC0 },

		// XFreeSectionByHandle+0x1A : inc eax
		{ 0x1A, 0x40 },

		// XFreeSectionByHandle+0x1B : retn 4
		{ 0x1B, 0xC2 },
		{ 0x1C, 0x04 }
	}

};

// ******************************************************************
// * RtlDestroyHeap
// ******************************************************************
SOOVPA<8> RtlDestroyHeap_1_0_4134 = 
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x10, 0x56 },
        { 0x22, 0x75 },
        { 0x34, 0xE0 },
        { 0x46, 0x67 },
        { 0x58, 0x75 },
        { 0x6A, 0xE0 },
        { 0x7C, 0x06 },
        { 0x8E, 0x00 },
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
    // GetExitCodeThread (* unchanged since 3911 *)
    {
        (OOVPA*)&GetExitCodeThread_1_0_3911,

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
    // XInputSetState (* unchanged since 3911 *)
    {
        (OOVPA*)&XInputSetState_1_0_3911,

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
    },
	// CreateFiber (* unchanged since 3911 *)
	{
		(OOVPA*)&CreateFiber_1_0_3911,

		XTL::EmuCreateFiber,

		#ifdef _DEBUG_TRACE
		"EmuCreateFiber"
		#endif
	},
	// DeleteFiber (* unchanged since 3911 *)
	{
		(OOVPA*)&DeleteFiber_1_0_3911,

		XTL::EmuDeleteFiber,

		#ifdef _DEBUG_TRACE
		"EmuDeleteFiber"
		#endif
	},
	// XLoadSectionA (* unchanged since 3911 *)
	{
		(OOVPA*)&XLoadSectionA_1_0_3911, 
			
		XTL::EmuXLoadSectionA,

		#ifdef _DEBUG_TRACE
		"EmuXLoadSectionA"
		#endif
	},
	// XFreeSectionA (* unchanged since 3911 *)
	{
		(OOVPA*)&XFreeSectionA_1_0_3911, 
			
		XTL::EmuXFreeSectionA,

		#ifdef _DEBUG_TRACE
		"EmuXFreeSectionA"
		#endif
	},
	// GetTimeZoneInformation (* unchanged since 3911 *)
    {
        (OOVPA*)&GetTimeZoneInformation_1_0_3911,

        XTL::EmuGetTimeZoneInformation,

        #ifdef _DEBUG_TRACE
        "GetTimeZoneInformation"
        #endif
    },
	// RtlCreateHeap (* unchanged since 3911 *)
    {
        (OOVPA*)&RtlCreateHeap_1_0_3911,

        XTL::EmuRtlCreateHeap,

        #ifdef _DEBUG_TRACE
        "EmuRtlCreateHeap"
        #endif
    },
    // RtlAllocateHeap (* unchanged since 3911 *)
    {
        (OOVPA*)&RtlAllocateHeap_1_0_3911,

        XTL::EmuRtlAllocateHeap,

        #ifdef _DEBUG_TRACE
        "EmuRtlAllocateHeap"
        #endif
    },
    // RtlFreeHeap (* unchanged since 3911 *)
    {
        (OOVPA*)&RtlFreeHeap_1_0_3911,

        XTL::EmuRtlFreeHeap,

        #ifdef _DEBUG_TRACE
        "EmuRtlFreeHeap"
        #endif
    },
    //*
    // RtlReAllocateHeap (* unchanged since 3911 *)
    {
        (OOVPA*)&RtlReAllocateHeap_1_0_3911,

        XTL::EmuRtlReAllocateHeap,

        #ifdef _DEBUG_TRACE
        "EmuRtlReAllocateHeap"
        #endif
    },
    //*/
    //*
    // RtlSizeHeap (* unchanged since 3911 *)
    {
        (OOVPA*)&RtlSizeHeap_1_0_3911,

        XTL::EmuRtlSizeHeap,

        #ifdef _DEBUG_TRACE
        "EmuRtlSizeHeap"
        #endif
    },
	// XapiBootToDash (* unchanged since 3911 *)
    {
        (OOVPA*)&XapiBootDash_1_0_3911,

        XTL::EmuXapiBootDash,

        #ifdef _DEBUG_TRACE
        "EmuXapiBootDash"
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
	// XGetDeviceChanges (* unchanged since 3911 *)
    {
        (OOVPA*)&XGetDeviceChanges_1_0_3911,

        XTL::EmuXGetDeviceChanges,

        #ifdef _DEBUG_TRACE
        "EmuXGetDeviceChanges"
        #endif
    },
	// XID_fCloseDevice (* unchanged since 3911 *)
    {
        (OOVPA*)&XID_fCloseDevice_1_0_3911, 0,

        #ifdef _DEBUG_TRACE
        "XID_fCloseDevice (XREF)"
        #endif
    },
    // XInputClose (* unchanged since 3911 *)
    {
        (OOVPA*)&XInputClose_1_0_3911,

        XTL::EmuXInputClose,

        #ifdef _DEBUG_TRACE
        "EmuXInputClose"
        #endif
    },
    // XInputGetCapabilities (* unchanged since 3911 *)
    {
        (OOVPA*)&XInputGetCapabilities_1_0_3911,

        XTL::EmuXInputGetCapabilities,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetCapabilities"
        #endif
    },
	// GetThreadPriority (* unchanged since 3911 *)
	{
		(OOVPA*)&GetThreadPriority_1_0_3911,

		XTL::EmuGetThreadPriority,

		#ifdef _DEBUG_TRACE
		"EmuGetThreadPriority"
		#endif
	},
	// XGetDevices (* unchanged since 3911 *)
	{
		(OOVPA*)&XGetDevices_1_0_3911,

		XTL::EmuXGetDevices,

		#ifdef _DEBUG_TRACE
		"EmuXGetDevices"
		#endif
	},
	// QueryPerformanceCounter
    {
        (OOVPA*)&QueryPerformanceCounter_1_0_4134,

        XTL::EmuQueryPerformanceCounter,

        #ifdef _DEBUG_TRACE
        "EmuQueryPerformanceCounter"
        #endif
    },
    // QueryPerformanceFrequency
    {
        (OOVPA*)&QueryPerformanceFrequency_1_0_4134,

        XTL::EmuQueryPerformanceFrequency,

        #ifdef _DEBUG_TRACE
        "EmuQueryPerformanceFrequency"
        #endif
    },
	// XInputPoll
    {
        (OOVPA*)&XInputPoll_1_0_4134,

        XTL::EmuXInputPoll,

        #ifdef _DEBUG_TRACE
        "EmuXInputPoll"
        #endif
    },
	// XGetSectionHandleA
	{
		(OOVPA*)&XGetSectionHandleA_1_0_4134, 
			
		XTL::EmuXGetSectionHandleA,

		#ifdef _DEBUG_TRACE
		"EmuXGetSectionHandleA"
		#endif
	},
	// XLoadSectionByHandle
	{
		(OOVPA*)&XLoadSectionByHandle_1_0_4134, 
			
		XTL::EmuXLoadSectionByHandle,

		#ifdef _DEBUG_TRACE
		"EmuXLoadSectionByHandle"
		#endif
	},
	// XFreeSectionByHandle
	{
		(OOVPA*)&XFreeSectionByHandle_1_0_4134, 
			
		XTL::EmuXFreeSectionByHandle,

		#ifdef _DEBUG_TRACE
		"EmuXFreeSectionByHandle"
		#endif
	},
	// RtlDestroyHeap
    {
        (OOVPA*)&RtlDestroyHeap_1_0_4134,

        XTL::EmuRtlDestroyHeap,

        #ifdef _DEBUG_TRACE
        "EmuRtlDestroyHeap"
        #endif
    },
	// SetThreadPriorityBoost (* unchanged since 3911 *)
	{
		(OOVPA*)&SetThreadPriorityBoost_1_0_3911,

		XTL::EmuSetThreadPriorityBoost,

		#ifdef _DEBUG_TRACE
		"EmuSetThreadPriorityBoost"
		#endif
	},
	// SignalObjectAndWait (* unchanged since 3911 *)
	{
		(OOVPA*)&SignalObjectAndWait_1_0_3911, 
			
		XTL::EmuSignalObjectAndWait,

		#ifdef _DEBUG_TRACE
		"EmuSignalObjectAndWait"
		#endif
	},
	// QueueUserAPC (* unchanged since 3911 *)
	{ 
		(OOVPA*)&QueueUserAPC_1_0_3911,

		XTL::EmuQueueUserAPC,
			
		#ifdef _DEBUG_TRACE
		"EmuQueueUserAPC"
		#endif
	},
};

// ******************************************************************
// * XAPI_1_0_4134_SIZE
// ******************************************************************
uint32 XAPI_1_0_4134_SIZE = sizeof(XAPI_1_0_4134);
