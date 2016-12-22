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
OOVPA_NO_XREF(XGetDevices_1_0_4134, 14)

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
OOVPA_END;

// ******************************************************************
// * XInputOpen
// ******************************************************************
OOVPA_NO_XREF(XInputOpen_1_0_4134, 12)

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
OOVPA_END;

// ******************************************************************
// * XInputGetState
// ******************************************************************
OOVPA_NO_XREF(XInputGetState_1_0_4134, 12)

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
OOVPA_END;

// ******************************************************************
// * XMountUtilityDrive
// ******************************************************************
OOVPA_NO_XREF(XMountUtilityDrive_1_0_4134, 10)

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
OOVPA_END;

// ******************************************************************
// * QueryPerformanceCounter
// ******************************************************************
OOVPA_NO_XREF(QueryPerformanceCounter_1_0_4134, 12)

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
OOVPA_END;

// ******************************************************************
// * QueryPerformanceFrequency
// ******************************************************************
OOVPA_NO_XREF(QueryPerformanceFrequency_1_0_4134, 15)

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
OOVPA_END;

// ******************************************************************
// * XInputPoll
// ******************************************************************
OOVPA_NO_XREF(XInputPoll_1_0_4134, 10)

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
OOVPA_END;

// ******************************************************************
// * RtlDestroyHeap
// ******************************************************************
OOVPA_NO_XREF(RtlDestroyHeap_1_0_4134, 8)

        { 0x10, 0x56 },
        { 0x22, 0x75 },
        { 0x34, 0xE0 },
        { 0x46, 0x67 },
        { 0x58, 0x75 },
        { 0x6A, 0xE0 },
        { 0x7C, 0x06 },
        { 0x8E, 0x00 },
OOVPA_END;

// ******************************************************************
// * XMountMUA
// ******************************************************************
OOVPA_NO_XREF(XMountMUA_1_0_4134, 8)

        { 0x22, 0x8A },
        { 0x3E, 0x89 },
        { 0x5E, 0x5C },
        { 0x7E, 0xF4 },
        { 0x9E, 0x50 },
        { 0xBE, 0xF8 },
        { 0xDE, 0x8D },
        { 0xFE, 0x09 },
OOVPA_END;

// ******************************************************************
// * GetFileAttributesA
// ******************************************************************
OOVPA_NO_XREF(GetFileAttributesA_1_0_4134, 7)

        { 0x08, 0x08 },
        { 0x13, 0x8D },
        { 0x1C, 0x50 },
        { 0x26, 0xFF },
        { 0x30, 0x15 },
        { 0x3A, 0x45 },
        { 0x44, 0x83 },
OOVPA_END;

// ******************************************************************
// * timeSetEvent
// ******************************************************************
OOVPA_NO_XREF(timeSetEvent_1_0_4134, 7)

        { 0x1E, 0x8D },
        { 0x3E, 0x89 },
        { 0x5E, 0x15 },
        { 0x7E, 0x3F },
        { 0x9E, 0x03 },
        { 0xBE, 0x32 },
        { 0xDE, 0x89 },
OOVPA_END;

// ******************************************************************
// * timeKillEvent
// ******************************************************************
OOVPA_NO_XREF(timeKillEvent_1_0_4134, 8)

        { 0x0E, 0x8B },
        { 0x1A, 0xC2 },
        { 0x28, 0x8D },
        { 0x36, 0x56 },
        { 0x44, 0x00 },
        { 0x52, 0x00 },
        { 0x60, 0x5E },
        { 0x6E, 0x00 },
OOVPA_END;

// ******************************************************************
// * XAPI_1_0_4134
// ******************************************************************
OOVPATable XAPI_1_0_4134[] = {

    // XInitDevices (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(XInitDevices_1_0_3911, XTL::EmuXInitDevices), //*/
    // SetThreadPriority (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(SetThreadPriority_1_0_3911, XTL::EmuSetThreadPriority),
	// GetExitCodeThread (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(GetExitCodeThread_1_0_3911, XTL::EmuGetExitCodeThread),
    // XGetDevices
	OOVPA_TABLE_PATCH(XGetDevices_1_0_4134, XTL::EmuXGetDevices),
    // XInputOpen
	OOVPA_TABLE_PATCH(XInputOpen_1_0_4134, XTL::EmuXInputOpen),
    // XInputGetState
	OOVPA_TABLE_PATCH(XInputGetState_1_0_4134, XTL::EmuXInputGetState),
    // XInputSetState (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(XInputSetState_1_0_3911, XTL::EmuXInputSetState),
    // XMountUtilityDrive
	OOVPA_TABLE_PATCH(XMountUtilityDrive_1_0_4134, XTL::EmuXMountUtilityDrive),
	// CreateFiber (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(CreateFiber_1_0_3911, XTL::EmuCreateFiber),
	// DeleteFiber (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(DeleteFiber_1_0_3911, XTL::EmuDeleteFiber),
	// GetTimeZoneInformation (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(GetTimeZoneInformation_1_0_3911, XTL::EmuGetTimeZoneInformation),
    // XRegisterThreadNotifyRoutine (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(XRegisterThreadNotifyRoutine_1_0_3911, XTL::EmuXRegisterThreadNotifyRoutine),
	// XGetDeviceChanges (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(XGetDeviceChanges_1_0_3911, XTL::EmuXGetDeviceChanges),
	// XID_fCloseDevice (* unchanged since 3911 *)
	OOVPA_TABLE_XREF(XID_fCloseDevice_1_0_3911),
    // XInputClose (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(XInputClose_1_0_3911, XTL::EmuXInputClose),
    // XInputGetCapabilities (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(XInputGetCapabilities_1_0_3911, XTL::EmuXInputGetCapabilities),
	// GetThreadPriority (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(GetThreadPriority_1_0_3911, XTL::EmuGetThreadPriority),
	// QueryPerformanceCounter
	OOVPA_TABLE_PATCH(QueryPerformanceCounter_1_0_4134, XTL::EmuQueryPerformanceCounter),
	// QueryPerformanceFrequency
	OOVPA_TABLE_PATCH(QueryPerformanceFrequency_1_0_4134, XTL::EmuQueryPerformanceFrequency),
	// XInputPoll
	OOVPA_TABLE_PATCH(XInputPoll_1_0_4134, XTL::EmuXInputPoll),
	// SetThreadPriorityBoost (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(SetThreadPriorityBoost_1_0_3911, XTL::EmuSetThreadPriorityBoost),
	// SignalObjectAndWait (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(SignalObjectAndWait_1_0_3911, XTL::EmuSignalObjectAndWait),
	// QueueUserAPC (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(QueueUserAPC_1_0_3911, XTL::EmuQueueUserAPC),
	// XMountMUA
	OOVPA_TABLE_PATCH(XMountMUA_1_0_4134, XTL::EmuXMountMUA),
	// GetFileAttributesA
	OOVPA_TABLE_PATCH(GetFileAttributesA_1_0_4134, XTL::EmuGetFileAttributesA),
	// timeSetEvent
	OOVPA_TABLE_PATCH(timeSetEvent_1_0_4134, XTL::EmutimeSetEvent),
	// timeKillEvent
	OOVPA_TABLE_PATCH(timeKillEvent_1_0_4134, XTL::EmutimeKillEvent),
	// XLaunchNewImageA
	OOVPA_TABLE_PATCH(XLaunchNewImageA_1_0_3911, XTL::EmuXLaunchNewImage),
};

// ******************************************************************
// * XAPI_1_0_4134_SIZE
// ******************************************************************
uint32 XAPI_1_0_4134_SIZE = sizeof(XAPI_1_0_4134);
