// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->Xapi.1.0.4361.cpp
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
// * XInputOpen
// ******************************************************************
OOVPA_NO_XREF(XInputOpen, 4361, 12)

        // XInputOpen+0x14 : push 0x57
        { 0x14, 0x6A }, // (Offset,Value)-Pair #1
        { 0x15, 0x57 }, // (Offset,Value)-Pair #2

        // XInputOpen+0x1D : jmp +0x33
        { 0x1D, 0xEB }, // (Offset,Value)-Pair #3
        { 0x1E, 0x33 }, // (Offset,Value)-Pair #4

        // XInputOpen+0x33 : add edx, 0x10
        { 0x33, 0x83 }, // (Offset,Value)-Pair #5
        { 0x34, 0xC2 }, // (Offset,Value)-Pair #6
        { 0x35, 0x10 }, // (Offset,Value)-Pair #7

        // XInputOpen+0x47 : jmp +0x06
        { 0x47, 0x75 }, // (Offset,Value)-Pair #8
        { 0x48, 0x06 }, // (Offset,Value)-Pair #9

        // XInputOpen+0x52 : leave
        { 0x52, 0xC9 }, // (Offset,Value)-Pair #10

        // XInputOpen+0x53 : retn 0x10
        { 0x53, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x54, 0x10 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * XID_fCloseDevice
// ******************************************************************
OOVPA_XREF(XID_fCloseDevice, 4361, 11,

    XREF_FCLOSEDEVICE,
    XRefZero)

        // XID_fCloseDevice+0x1B : mov eax, [esi+0x00A3]
        { 0x1B, 0x8B }, // (Offset,Value)-Pair #1
        { 0x1C, 0x86 }, // (Offset,Value)-Pair #2
        { 0x1D, 0xA3 }, // (Offset,Value)-Pair #3

        // XID_fCloseDevice+0x91 : mov [esi+0x00A7], eax
        { 0x91, 0x89 }, // (Offset,Value)-Pair #4
        { 0x92, 0x86 }, // (Offset,Value)-Pair #5
        { 0x93, 0xA7 }, // (Offset,Value)-Pair #6

        // XID_fCloseDevice+0x91 : mov [esi+0x00A7], eax    // FIXME: Same instruction repeated.
        { 0x91, 0x89 }, // (Offset,Value)-Pair #7
        { 0x92, 0x86 }, // (Offset,Value)-Pair #8
        { 0x93, 0xA7 }, // (Offset,Value)-Pair #9

        // XID_fCloseDevice+0x9F : leave; retn
        { 0x9F, 0xC9 }, // (Offset,Value)-Pair #10
        { 0xA0, 0xC3 }, // (Offset,Value)-Pair #11
OOVPA_END;

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
// * XInputSetState
// ******************************************************************
OOVPA_NO_XREF(XInputSetState, 4361, 12)

        // XInputSetState+0x04 : lea eax, [ecx+0x0A3]
        { 0x04, 0x8D }, // (Offset,Value)-Pair #1
        { 0x05, 0x81 }, // (Offset,Value)-Pair #2
        { 0x06, 0xA3 }, // (Offset,Value)-Pair #3

        // XInputSetState+0x15 : push 0x57
        { 0x15, 0x6A }, // (Offset,Value)-Pair #4
        { 0x16, 0x57 }, // (Offset,Value)-Pair #5

        // XInputSetState+0x18 : jmp +0x19
        { 0x18, 0xEB }, // (Offset,Value)-Pair #6
        { 0x19, 0x19 }, // (Offset,Value)-Pair #7

        // XInputSetState+0x2B : mov [edx+0x41], al
        { 0x2B, 0x88 }, // (Offset,Value)-Pair #8
        { 0x2C, 0x42 }, // (Offset,Value)-Pair #9
        { 0x2D, 0x41 }, // (Offset,Value)-Pair #10

        // XInputSetState+0x33 : retn 0x08
        { 0x33, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x34, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * XInputSetState
// ******************************************************************
OOVPA_NO_XREF(XInputSetState, 4928, 12)

        // XInputSetState+0x04 : lea eax, [ecx+0x0A3]
        { 0x04, 0x8D }, // (Offset,Value)-Pair #1
        { 0x05, 0x81 }, // (Offset,Value)-Pair #2
        { 0x06, 0xA3 }, // (Offset,Value)-Pair #3

        // XInputSetState+0x12 : push 0x57
        { 0x12, 0x6A }, // (Offset,Value)-Pair #4
        { 0x13, 0x57 }, // (Offset,Value)-Pair #5

        // XInputSetState+0x15 : jmp +0x19
        { 0x15, 0xEB }, // (Offset,Value)-Pair #6
        { 0x16, 0x19 }, // (Offset,Value)-Pair #7

        // XInputSetState+0x28 : mov [edx+0x41], al
        { 0x28, 0x88 }, // (Offset,Value)-Pair #8
        { 0x29, 0x42 }, // (Offset,Value)-Pair #9
        { 0x2A, 0x41 }, // (Offset,Value)-Pair #10

        // XInputSetState+0x30 : retn 0x08
        { 0x30, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x31, 0x08 }, // (Offset,Value)-Pair #12
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

        // XapiInitProcess+0x03 : sub esp, 30h
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

// ******************************************************************
// * XapiThreadStartup
// ******************************************************************
OOVPA_NO_XREF(XapiThreadStartup, 4361, 10)

        // XapiThreadStartup+0x00 : push 0x18
        { 0x00, 0x6A }, // (Offset,Value)-Pair #1
        { 0x01, 0x18 }, // (Offset,Value)-Pair #2

        // XapiThreadStartup+0x10 : mov eax, fs:[0x28]
        { 0x10, 0x64 }, // (Offset,Value)-Pair #3
        { 0x11, 0xA1 }, // (Offset,Value)-Pair #4
        { 0x12, 0x28 }, // (Offset,Value)-Pair #5

        // XapiThreadStartup+0x3F : repe movsd
        { 0x3F, 0xF3 }, // (Offset,Value)-Pair #6
        { 0x40, 0xA5 }, // (Offset,Value)-Pair #7

        // XapiThreadStartup+0x7C : jmp +0x0C
        { 0x7C, 0xEB }, // (Offset,Value)-Pair #8
        { 0x7D, 0x0C }, // (Offset,Value)-Pair #9

        // XapiThreadStartup+0x86 : retn
        { 0x86, 0xC3 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * XapiSetupPerTitleDriveLetters
// ******************************************************************
OOVPA_NO_XREF(XapiSetupPerTitleDriveLetters, 4361, 10)

        // XapiSetupPerTitleDriveLetters+0x09 : lea eax, [ebp-0x0C]
        { 0x09, 0x8D }, // (Offset,Value)-Pair #1
        { 0x0A, 0x45 }, // (Offset,Value)-Pair #2
        { 0x0B, 0xF4 }, // (Offset,Value)-Pair #3

        // XapiSetupPerTitleDriveLetters+0x17 : edd esp, 0x0C
        { 0x17, 0x83 }, // (Offset,Value)-Pair #4
        { 0x18, 0xC4 }, // (Offset,Value)-Pair #5
        { 0x19, 0x0C }, // (Offset,Value)-Pair #6

        // XapiSetupPerTitleDriveLetters+0x35 : jl 0x1A
        { 0x35, 0x7C }, // (Offset,Value)-Pair #7
        { 0x36, 0x1A }, // (Offset,Value)-Pair #8

        // XapiSetupPerTitleDriveLetters+0x52 : retn 0x08
        { 0x52, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x53, 0x08 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * XMountMUA
// ******************************************************************
OOVPA_NO_XREF(XMountMUA, 4361, 8)

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
// * XAPI_4361
// ******************************************************************
OOVPATable XAPI_4361[] = {

	OOVPA_TABLE_ENTRY(XInitDevices, 3911),
	OOVPA_TABLE_ENTRY(XGetDevices, 4134),
	OOVPA_TABLE_ENTRY(XGetDeviceChanges, 3911),
	OOVPA_TABLE_ENTRY(XInputOpen, 4361),
	OOVPA_TABLE_XREF(XID_fCloseDevice, 4361),
	OOVPA_TABLE_ENTRY(XInputClose, 3911),
	OOVPA_TABLE_ENTRY(XInputGetCapabilities, 4361),
	OOVPA_TABLE_ENTRY(XInputGetState, 4361),
	OOVPA_TABLE_ENTRY(XInputSetState, 4361),
    // OOVPA_TABLE_ENTRY(XapiThreadStartup, 4361),
	OOVPA_TABLE_ENTRY(XMountUtilityDrive, 4134), // TODO: This needs to be verified on 4361, not just 4242!
	OOVPA_TABLE_ENTRY(CreateFiber, 3911),
	OOVPA_TABLE_ENTRY(DeleteFiber, 3911),
	OOVPA_TABLE_ENTRY(GetTimeZoneInformation, 3911),
	OOVPA_TABLE_ENTRY(SetThreadPriority, 3911),
	OOVPA_TABLE_ENTRY(GetExitCodeThread, 3911),
	OOVPA_TABLE_ENTRY(SetThreadPriorityBoost, 3911),
	OOVPA_TABLE_ENTRY(SignalObjectAndWait, 3911),
	OOVPA_TABLE_ENTRY(QueueUserAPC, 3911),
	OOVPA_TABLE_ENTRY(XMountMUA, 4361),
};

// ******************************************************************
// * XAPI_4361_SIZE
// ******************************************************************
uint32 XAPI_4361_SIZE = sizeof(XAPI_4361);
