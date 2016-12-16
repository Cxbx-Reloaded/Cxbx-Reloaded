// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->Xapi.1.0.4432.cpp
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
SOOVPA<9> RtlFreeHeap_1_0_4432 =
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    XRefNotUsed,

    {
        // RtlFreeHeap+0x1F : test ecx, ecx
        { 0x1F, 0x85 }, // (Offset,Value)-Pair #1
        { 0x20, 0xC9 }, // (Offset,Value)-Pair #2

        // RtlFreeHeap+0x23 : mov al, 1
        { 0x23, 0xB0 }, // (Offset,Value)-Pair #3
        { 0x24, 0x01 }, // (Offset,Value)-Pair #4

        // RtlFreeHeap+0x4B : test byte ptr [edi+5], 8
        { 0x4B, 0xF6 }, // (Offset,Value)-Pair #5
        { 0x4C, 0x47 }, // (Offset,Value)-Pair #6
        { 0x4D, 0x05 }, // (Offset,Value)-Pair #7

        // RtlFreeHeap+0x6E : cmp ecx, 0x80
        { 0x6E, 0x81 }, // (Offset,Value)-Pair #8
        { 0x6F, 0xF9 }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * XMountUtilityDrive
// ******************************************************************
SOOVPA<8> XMountUtilityDrive_1_0_4432 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

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

// ******************************************************************
// * XAPI_1_0_4432
// ******************************************************************
OOVPATable XAPI_1_0_4432[] =
{

    // XMountUtilityDrive
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
	// XID_fCloseDevice
	{
		(OOVPA*)&XID_fCloseDevice_1_0_4361, 0,

		#ifdef _DEBUG_TRACE
		"XID_fCloseDevice (XREF)"
		#endif
	},
	// XInputClose (* unchanged since 1.0.3911 *)
	{
		(OOVPA*)&XInputClose_1_0_3911,

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
    // XInputSetState (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&XInputSetState_1_0_4361,

        XTL::EmuXInputSetState,

        #ifdef _DEBUG_TRACE
        "EmuXInputSetState"
        #endif
    },
    /*
    // XapiThreadStartup (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&XapiThreadStartup_1_0_4361,

        XTL::EmuXapiThreadStartup,

        #ifdef _DEBUG_TRACE
        "EmuXapiThreadStartup"
        #endif
    },
    //*/
    //*/
	// GetExitCodeThread (* unchanged since 3911 *)
    {
        (OOVPA*)&GetExitCodeThread_1_0_3911,

        XTL::EmuGetExitCodeThread,

        #ifdef _DEBUG_TRACE
        "EmuGetExitCodeThread"
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
	// QueueUserAPC (* unchanged since 3911 *)
	{ 
		(OOVPA*)&QueueUserAPC_1_0_3911,

		XTL::EmuQueueUserAPC,
			
		#ifdef _DEBUG_TRACE
		"EmuQueueUserAPC"
		#endif
	},
	// GetFileAttributesA (* unchanged since 4134 *)
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
// * XAPI_1_0_4432_SIZE
// ******************************************************************
uint32 XAPI_1_0_4432_SIZE = sizeof(XAPI_1_0_4432);
