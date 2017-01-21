// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->Xapi.1.0.4034.cpp
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

/*

// ******************************************************************
// * XInputGetCapabilities
// ******************************************************************
OOVPA_NO_XREF(XInputGetCapabilities, 4361, 14)

        // XInputGetCapabilities+0x03 : sub esp, 0x48
        { 0x03, 0x83 }, // (Offset,Value)-Pair #1
        { 0x04, 0xEC }, // (Offset,Value)-Pair #2
        { 0x05, 0x48 }, // (Offset,Value)-Pair #3

        // XInputGetCapabilities+0x24 : test byte ptr [esi+4], 2
        { 0x24, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x25, 0x46 }, // (Offset,Value)-Pair #5
        { 0x26, 0x04 }, // (Offset,Value)-Pair #6
        { 0x27, 0x02 }, // (Offset,Value)-Pair #7

        // XInputGetCapabilities+0x3A : stosb
        { 0x3A, 0xAA }, // (Offset,Value)-Pair #8

        // XInputGetCapabilities+0x7D : mov [ebp+var_48], 0x30
        { 0x7D, 0xC6 }, // (Offset,Value)-Pair #9
        { 0x7E, 0x45 }, // (Offset,Value)-Pair #10
        { 0x7F, 0xB8 }, // (Offset,Value)-Pair #11
        { 0x80, 0x30 }, // (Offset,Value)-Pair #12

        // XInputGetCapabilities+0xEE : cmp [eax], ebx
        { 0xEE, 0x39 }, // (Offset,Value)-Pair #13
        { 0xEF, 0x18 }, // (Offset,Value)-Pair #14
OOVPA_END;

// ******************************************************************
// * XInputGetState
// ******************************************************************
OOVPA_NO_XREF(XInputGetState, 4361, 12)

        // XInputGetState+0x0E : mov ecx, [edx+0x0A3]
        { 0x0E, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0F, 0x8A }, // (Offset,Value)-Pair #2
        { 0x10, 0xA3 }, // (Offset,Value)-Pair #3

        // XInputGetState+0x1D : push 0x57
        { 0x1D, 0x6A }, // (Offset,Value)-Pair #4
        { 0x1E, 0x57 }, // (Offset,Value)-Pair #5

        // XInputGetState+0x20 : jmp +0x3E
        { 0x20, 0xEB }, // (Offset,Value)-Pair #6
        { 0x21, 0x3E }, // (Offset,Value)-Pair #7

        // XInputGetState+0x2E : mov ebx, 0x048F
        { 0x2E, 0xBB }, // (Offset,Value)-Pair #8
        { 0x2F, 0x8F }, // (Offset,Value)-Pair #9
        { 0x30, 0x04 }, // (Offset,Value)-Pair #10

        // XInputGetState+0x6C : retn 8
        { 0x6C, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x6D, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * CreateThread
// ******************************************************************
OOVPA_NO_XREF(CreateThread, 4361, 8)

        // CreateThread+0x0A : mov eax, ds:10130h
        { 0x0A, 0xA1 }, // (Offset,Value)-Pair #1
        { 0x0B, 0x30 }, // (Offset,Value)-Pair #2
        { 0x0C, 0x01 }, // (Offset,Value)-Pair #3

        // CreateThread+0x1C : and ecx, 0xFFFFFF01
        { 0x1C, 0x81 }, // (Offset,Value)-Pair #4
        { 0x1D, 0xE1 }, // (Offset,Value)-Pair #5
        { 0x1E, 0x01 }, // (Offset,Value)-Pair #6

        // CreateThread+0x6B : retn 0x18
        { 0x6B, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x6C, 0x18 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * CloseHandle
// ******************************************************************
OOVPA_NO_XREF(CloseHandle, 4361, 10)

        // CloseHandle+0x00 : push [esp+4]
        { 0x00, 0xFF }, // (Offset,Value)-Pair #1
        { 0x01, 0x74 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x04 }, // (Offset,Value)-Pair #4

        // CloseHandle+0x04 : call ds:[addr]
        { 0x04, 0xFF }, // (Offset,Value)-Pair #5
        { 0x05, 0x15 }, // (Offset,Value)-Pair #6

        // CloseHandle+0x11 : jmp +0x08
        { 0x11, 0xEB }, // (Offset,Value)-Pair #7
        { 0x12, 0x08 }, // (Offset,Value)-Pair #8

        // CloseHandle+0x1B : retn 4
        { 0x1B, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x1C, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * XapiInitProcess
// ******************************************************************
OOVPA_NO_XREF(XapiInitProcess, 4361, 7)

        // XapiInitProcess+0x00 : sub esp, 30h
        { 0x05, 0x30 }, // (Offset,Value)-Pair #1

        // XapiInitProcess+0x0F : push 0x0C
        { 0x0F, 0x6A }, // (Offset,Value)-Pair #2
        { 0x10, 0x0C }, // (Offset,Value)-Pair #3

        // XapiInitProcess+0x17 : repe stosd
        { 0x17, 0xF3 }, // (Offset,Value)-Pair #4
        { 0x18, 0xAB }, // (Offset,Value)-Pair #5

        // XapiInitProcess+0x42 : jnz +0x0B
        { 0x42, 0x75 }, // (Offset,Value)-Pair #6
        { 0x43, 0x0B }, // (Offset,Value)-Pair #7
OOVPA_END;
*/

// ******************************************************************
// * XAPI_4034
// ******************************************************************
OOVPATable XAPI_4034[] = {

    // XInitDevices (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(XInitDevices, 3911, XInitDevices),
    // XGetDevices (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(XGetDevices, 3911, XGetDevices),
/* These functions havent been rev'd yet (may be same as new/old)
    // XInputOpen
	OOVPA_TABLE_PATCH(XInputOpen, 4034, XInputOpen),
    // XInputGetCapabilities
	OOVPA_TABLE_PATCH(XInputGetCapabilities, 4361, XInputGetCapabilities),
    // XInputGetState
	OOVPA_TABLE_PATCH(XInputGetState, 4361, XInputGetState), */
    /* Too High Level
    // CreateThread (* unchanged since 1.0.3911 *)
	OOVPA_TABLE_PATCH(CreateThread, 3911, CreateThread), */
    /*
    // CloseHandle (*???*)
	OOVPA_TABLE_PATCH(CloseHandle, (???), CloseHandle), */
    //*/
	// CreateFiber (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(CreateFiber, 3911, CreateFiber),
	// DeleteFiber (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(DeleteFiber, 3911, DeleteFiber),
	// GetTimeZoneInformation (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(GetTimeZoneInformation, 3911, GetTimeZoneInformation),
	// SetThreadPriority (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(SetThreadPriority, 3911, SetThreadPriority),
	// SignalObjectAndWait (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(SignalObjectAndWait, 3911, SignalObjectAndWait),
	// QueueUserAPC (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(QueueUserAPC, 3911, QueueUserAPC),
	// XInputSetState (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(XInputSetState, 3911, XInputSetState),

	// XRegisterThreadNotifyRoutine (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(XRegisterThreadNotifyRoutine, 3911, XRegisterThreadNotifyRoutine),
	// XGetDeviceChanges (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(XGetDeviceChanges, 3911, XGetDeviceChanges),
	// XID_fCloseDevice (* unchanged since 3911 *)
	OOVPA_TABLE_XREF(XID_fCloseDevice, 3911),
    // XInputClose (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(XInputClose, 3911, XInputClose),
    // XInputGetCapabilities (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(XInputGetCapabilities, 3911, XInputGetCapabilities),
	// GetThreadPriority (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(GetThreadPriority, 3911, GetThreadPriority),
	// XInputOpen (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(XInputOpen, 3911, XInputOpen),
	// XInputGetState (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(XInputGetState, 3911, XInputGetState),
};

// ******************************************************************
// * XAPI_4034_SIZE
// ******************************************************************
uint32 XAPI_4034_SIZE = sizeof(XAPI_4034);
