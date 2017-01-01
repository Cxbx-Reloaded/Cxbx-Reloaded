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
// * XMountUtilityDrive
// ******************************************************************
OOVPA_NO_XREF(XMountUtilityDrive_1_0_4432, 8)

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
OOVPA_END;

// ******************************************************************
// * XAPI_1_0_4432
// ******************************************************************
OOVPATable XAPI_1_0_4432[] = {
    // XMountUtilityDrive
	OOVPA_TABLE_PATCH(XMountUtilityDrive_1_0_4432, XTL::EmuXMountUtilityDrive),
    // XInitDevices (* unchanged since 1.0.3911 *)
	OOVPA_TABLE_PATCH(XInitDevices_1_0_3911, XTL::EmuXInitDevices),
    // XGetDevices (* unchanged since 1.0.4134 *)
	OOVPA_TABLE_PATCH(XGetDevices_1_0_4134, XTL::EmuXGetDevices),
    // XInputOpen (* unchanged since 1.0.4361 *)
	OOVPA_TABLE_PATCH(XInputOpen_1_0_4361, XTL::EmuXInputOpen),
	// XID_fCloseDevice
	OOVPA_TABLE_XREF(XID_fCloseDevice_1_0_4361),
	// XInputClose (* unchanged since 1.0.3911 *)
	OOVPA_TABLE_PATCH(XInputClose_1_0_3911, XTL::EmuXInputClose),
    // XInputGetCapabilities (* unchanged since 1.0.4361 *)
	OOVPA_TABLE_PATCH(XInputGetCapabilities_1_0_4361, XTL::EmuXInputGetCapabilities),
    // XInputGetState (* unchanged since 1.0.4361 *)
	OOVPA_TABLE_PATCH(XInputGetState_1_0_4361, XTL::EmuXInputGetState),
    // XInputSetState (* unchanged since 1.0.4361 *)
	OOVPA_TABLE_PATCH(XInputSetState_1_0_4361, XTL::EmuXInputSetState),
    /*
    // XapiThreadStartup (* unchanged since 1.0.4361 *)
	OOVPA_TABLE_PATCH(XapiThreadStartup_1_0_4361, XTL::EmuXapiThreadStartup), //*/
    //*/
	// GetExitCodeThread (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(GetExitCodeThread_1_0_3911, XTL::EmuGetExitCodeThread),
	// GetTimeZoneInformation (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(GetTimeZoneInformation_1_0_3911, XTL::EmuGetTimeZoneInformation),
	// SetThreadPriority (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(SetThreadPriority_1_0_3911, XTL::EmuSetThreadPriority),
	// SignalObjectAndWait (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(SignalObjectAndWait_1_0_3911, XTL::EmuSignalObjectAndWait),
	// CreateFiber (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(CreateFiber_1_0_3911, XTL::EmuCreateFiber),
	// DeleteFiber (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(DeleteFiber_1_0_3911, XTL::EmuDeleteFiber),
	// QueueUserAPC (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(QueueUserAPC_1_0_3911, XTL::EmuQueueUserAPC),
	// GetFileAttributesA (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(GetFileAttributesA_1_0_4134, XTL::EmuGetFileAttributesA),
	// timeSetEvent
	OOVPA_TABLE_PATCH(timeSetEvent_1_0_4134, XTL::EmutimeSetEvent),
	// timeKillEvent
	OOVPA_TABLE_PATCH(timeKillEvent_1_0_4134, XTL::EmutimeKillEvent),
	// XLaunchNewImageA
	OOVPA_TABLE_PATCH(XLaunchNewImageA_1_0_3911, XTL::EmuXLaunchNewImage),
};

// ******************************************************************
// * XAPI_1_0_4432_SIZE
// ******************************************************************
uint32 XAPI_1_0_4432_SIZE = sizeof(XAPI_1_0_4432);
