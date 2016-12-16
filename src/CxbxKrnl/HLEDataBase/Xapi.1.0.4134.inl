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

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
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
// * RtlDestroyHeap
// ******************************************************************
SOOVPA<8> RtlDestroyHeap_1_0_4134 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

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
// * XMountMUA
// ******************************************************************
SOOVPA<8> XMountMUA_1_0_4134 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x22, 0x8A },
        { 0x3E, 0x89 },
        { 0x5E, 0x5C },
        { 0x7E, 0xF4 },
        { 0x9E, 0x50 },
        { 0xBE, 0xF8 },
        { 0xDE, 0x8D },
        { 0xFE, 0x09 },
    }
};

// ******************************************************************
// * GetFileAttributesA
// ******************************************************************
SOOVPA<7> GetFileAttributesA_1_0_4134 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x08, 0x08 },
        { 0x13, 0x8D },
        { 0x1C, 0x50 },
        { 0x26, 0xFF },
        { 0x30, 0x15 },
        { 0x3A, 0x45 },
        { 0x44, 0x83 },
    }
};

// ******************************************************************
// * timeSetEvent
// ******************************************************************
SOOVPA<7> timeSetEvent_1_0_4134 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x1E, 0x8D },
        { 0x3E, 0x89 },
        { 0x5E, 0x15 },
        { 0x7E, 0x3F },
        { 0x9E, 0x03 },
        { 0xBE, 0x32 },
        { 0xDE, 0x89 },
    }
};

// ******************************************************************
// * timeKillEvent
// ******************************************************************
SOOVPA<8> timeKillEvent_1_0_4134 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x0E, 0x8B },
        { 0x1A, 0xC2 },
        { 0x28, 0x8D },
        { 0x36, 0x56 },
        { 0x44, 0x00 },
        { 0x52, 0x00 },
        { 0x60, 0x5E },
        { 0x6E, 0x00 },
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
	// GetTimeZoneInformation (* unchanged since 3911 *)
    {
        (OOVPA*)&GetTimeZoneInformation_1_0_3911,

        XTL::EmuGetTimeZoneInformation,

        #ifdef _DEBUG_TRACE
        "GetTimeZoneInformation"
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
	// XMountMUA
    {
        (OOVPA*)&XMountMUA_1_0_4134,
        XTL::EmuXMountMUA,

        #ifdef _DEBUG_TRACE
        "EmuXMountMUA"
        #endif
    },
	// GetFileAttributesA
    {
        (OOVPA*)&GetFileAttributesA_1_0_4134,
        XTL::EmuGetFileAttributesA,

        #ifdef _DEBUG_TRACE
        "EmuGetFileAttributesA"
        #endif
    },
	// timeSetEvent
    {
        (OOVPA*)&timeSetEvent_1_0_4134,
        XTL::EmutimeSetEvent,

        #ifdef _DEBUG_TRACE
        "EmutimeSetEvent"
        #endif
    },
    // timeKillEvent
    {
        (OOVPA*)&timeKillEvent_1_0_4134,
        XTL::EmutimeKillEvent,

        #ifdef _DEBUG_TRACE
        "EmutimeKillEvent"
        #endif
    },
	// XLaunchNewImageA
	{
		(OOVPA*)&XLaunchNewImageA_1_0_3911,
		XTL::EmuXLaunchNewImage,

		#ifdef _DEBUG_TRACE
		"EmuXLaunchNewImageA"
		#endif
	},
};

// ******************************************************************
// * XAPI_1_0_4134_SIZE
// ******************************************************************
uint32 XAPI_1_0_4134_SIZE = sizeof(XAPI_1_0_4134);
