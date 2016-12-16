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
SOOVPA<14> XInputGetCapabilities_1_0_4361 =
{
    0,  // Large == 0
    14, // Count == 14

    XRefNotSaved,
    XRefNotUsed,

    {
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
    }
};

// ******************************************************************
// * XInputGetState
// ******************************************************************
SOOVPA<12> XInputGetState_1_0_4361 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
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
    }
};

// ******************************************************************
// * CreateThread
// ******************************************************************
SOOVPA<8> CreateThread_1_0_4361 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
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
        { 0x6C, 0x18 }  // (Offset,Value)-Pair #8
    }
};

// ******************************************************************
// * CloseHandle
// ******************************************************************
SOOVPA<10> CloseHandle_1_0_4361 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
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
        { 0x1C, 0x04 }  // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * XapiInitProcess
// ******************************************************************
SOOVPA<7> XapiInitProcess_1_0_4361 =
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
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
    }
};
*/

// ******************************************************************
// * XAPI_1_0_4034
// ******************************************************************
OOVPATable XAPI_1_0_4034[] =
{
    // XInitDevices (* unchanged since 3911 *)
    {
        (OOVPA*)&XInitDevices_1_0_3911,

        XTL::EmuXInitDevices,

        #ifdef _DEBUG_TRACE
        "EmuXInitDevices"
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
/* These functions havent been rev'd yet (may be same as new/old)
    // XInputOpen
    {
        (OOVPA*)&XInputOpen_1_0_4034,

        XTL::EmuXInputOpen,

        #ifdef _DEBUG_TRACE
        "EmuXInputOpen"
        #endif
    },
    // XInputGetCapabilities
    {
        (OOVPA*)&XInputGetCapabilities_1_0_4361,

        XTL::EmuXInputGetCapabilities,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetCapabilities"
        #endif
    },
    // XInputGetState
    {
        (OOVPA*)&XInputGetState_1_0_4361,

        XTL::EmuXInputGetState,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetState"
        #endif
    },
    */
    /* Too High Level
    // CreateThread (* unchanged since 1.0.3911 *)
    {
        (OOVPA*)&CreateThread_1_0_3911,

        XTL::EmuCreateThread,

        #ifdef _DEBUG_TRACE
        "EmuCreateThread"
        #endif
    },
    */
    /*
    // CloseHandle (*???*)
    {
        (OOVPA*)&CloseHandle_1_0_(???),

        XTL::EmuCloseHandle,

        #ifdef _DEBUG_TRACE
        "EmuCloseHandle"
        #endif
    },
    */
    //*/
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
    // SetThreadPriority (* unchanged since 3911 *)
    {
        (OOVPA*)&SetThreadPriority_1_0_3911,

        XTL::EmuSetThreadPriority,

        #ifdef _DEBUG_TRACE
        "EmuSetThreadPriority"
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
	// XInputSetState (* unchanged since 3911 *)
    {
        (OOVPA*)&XInputSetState_1_0_3911,

        XTL::EmuXInputSetState,

        #ifdef _DEBUG_TRACE
        "EmuXInputSetState"
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
	// XInputOpen (* unchanged since 3911 *)
    {
        (OOVPA*)&XInputOpen_1_0_3911,

        XTL::EmuXInputOpen,

        #ifdef _DEBUG_TRACE
        "EmuXInputOpen"
        #endif
    },
	// XInputGetState (* unchanged since 3911 *)
    {
        (OOVPA*)&XInputGetState_1_0_3911,

        XTL::EmuXInputGetState,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetState"
        #endif
    },
};

// ******************************************************************
// * XAPI_1_0_4034_SIZE
// ******************************************************************
uint32 XAPI_1_0_4034_SIZE = sizeof(XAPI_1_0_4034);
