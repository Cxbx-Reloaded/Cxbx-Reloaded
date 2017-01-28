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
OOVPA_NO_XREF(XGetDevices, 4134, 14)

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
OOVPA_NO_XREF(XInputOpen, 4134, 12)

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
OOVPA_NO_XREF(XInputGetState, 4134, 12)

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
OOVPA_NO_XREF(XMountUtilityDrive, 4134, 10)

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
// * XInputPoll
// ******************************************************************
OOVPA_NO_XREF(XInputPoll, 4134, 10)

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
// * XMountMUA
// ******************************************************************
OOVPA_NO_XREF(XMountMUA, 4134, 8)

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
// * timeSetEvent
// ******************************************************************
OOVPA_NO_XREF(timeSetEvent, 4134, 7)

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
OOVPA_NO_XREF(timeKillEvent, 4134, 8)

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
// * XAPI_4134
// ******************************************************************
OOVPATable XAPI_4134[] = {

    // XInitDevices (* unchanged since 3911 *)
	OOVPA_TABLE_ENTRY(XInitDevices, 3911), //*/
    // SetThreadPriority (* unchanged since 3911 *)
	OOVPA_TABLE_ENTRY(SetThreadPriority, 3911),
	// GetExitCodeThread (* unchanged since 3911 *)
	OOVPA_TABLE_ENTRY(GetExitCodeThread, 3911),
    // XGetDevices
	OOVPA_TABLE_ENTRY(XGetDevices, 4134),
    // XInputOpen
	OOVPA_TABLE_ENTRY(XInputOpen, 4134),
    // XInputGetState
	OOVPA_TABLE_ENTRY(XInputGetState, 4134),
    // XInputSetState (* unchanged since 3911 *)
	OOVPA_TABLE_ENTRY(XInputSetState, 3911),
    // XMountUtilityDrive
	OOVPA_TABLE_ENTRY(XMountUtilityDrive, 4134),
	// CreateFiber (* unchanged since 3911 *)
	OOVPA_TABLE_ENTRY(CreateFiber, 3911),
	// DeleteFiber (* unchanged since 3911 *)
	OOVPA_TABLE_ENTRY(DeleteFiber, 3911),
	// GetTimeZoneInformation (* unchanged since 3911 *)
	OOVPA_TABLE_ENTRY(GetTimeZoneInformation, 3911),
    // XRegisterThreadNotifyRoutine (* unchanged since 3911 *)
	OOVPA_TABLE_ENTRY(XRegisterThreadNotifyRoutine, 3911),
	// XGetDeviceChanges (* unchanged since 3911 *)
	OOVPA_TABLE_ENTRY(XGetDeviceChanges, 3911),
	// XID_fCloseDevice (* unchanged since 3911 *)
	OOVPA_TABLE_XREF(XID_fCloseDevice, 3911),
    // XInputClose (* unchanged since 3911 *)
	OOVPA_TABLE_ENTRY(XInputClose, 3911),
    // XInputGetCapabilities (* unchanged since 3911 *)
	OOVPA_TABLE_ENTRY(XInputGetCapabilities, 3911),
	// GetThreadPriority (* unchanged since 3911 *)
	OOVPA_TABLE_ENTRY(GetThreadPriority, 3911),
	// XInputPoll
	OOVPA_TABLE_ENTRY(XInputPoll, 4134),
	// SetThreadPriorityBoost (* unchanged since 3911 *)
	OOVPA_TABLE_ENTRY(SetThreadPriorityBoost, 3911),
	// SignalObjectAndWait (* unchanged since 3911 *)
	OOVPA_TABLE_ENTRY(SignalObjectAndWait, 3911),
	// QueueUserAPC (* unchanged since 3911 *)
	OOVPA_TABLE_ENTRY(QueueUserAPC, 3911),
	// XMountMUA
	OOVPA_TABLE_ENTRY(XMountMUA, 4134),
	// timeSetEvent
	OOVPA_TABLE_ENTRY(timeSetEvent, 4134),
	// timeKillEvent
	OOVPA_TABLE_ENTRY(timeKillEvent, 4134),
	// XLaunchNewImageA
	OOVPA_TABLE_PATCH(XLaunchNewImageA, 3911, XLaunchNewImage),
};

// ******************************************************************
// * XAPI_4134_SIZE
// ******************************************************************
uint32 XAPI_4134_SIZE = sizeof(XAPI_4134);
