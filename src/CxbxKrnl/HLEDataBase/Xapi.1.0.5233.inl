// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->Xapi.1.0.5233.cpp
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
// * XInitDevices
// ******************************************************************
// * NOTE: We are actually intercepting USBD_Init, because
// *       XInitDevices Simply redirects to that function
// ******************************************************************
OOVPA_NO_XREF(XInitDevices_1_0_5233, 7)

        { 0x0E, 0x75 },
        { 0x20, 0xBE },
        { 0x2E, 0xC0 },
        { 0x3E, 0xEC },
        { 0x50, 0x0F },
        { 0x5F, 0xE8 },
        { 0x70, 0x5F },
OOVPA_END;

// ******************************************************************
// * XInputGetCapabilities
// ******************************************************************
OOVPA_NO_XREF(XInputGetCapabilities_1_0_5233, 8)

        { 0x1D, 0x85 },
        { 0x3C, 0x8A },
        { 0x5B, 0x00 },
        { 0x85, 0x45 },
        { 0xA0, 0xD4 },
        { 0xBB, 0x02 },
        { 0xDA, 0x8D },
        { 0xF9, 0x46 },
OOVPA_END;

// ******************************************************************
// * XGetDeviceChanges
// ******************************************************************
OOVPA_NO_XREF(XGetDeviceChanges_1_0_5233, 7)

        { 0x0C, 0x75 },
        { 0x1A, 0x53 },
        { 0x28, 0xF7 },
        { 0x36, 0x56 },
        { 0x44, 0x89 },
        { 0x52, 0xC8 },
        { 0x60, 0x5F },
OOVPA_END;

// ******************************************************************
// * XInputSetState
// ******************************************************************
OOVPA_NO_XREF(XInputSetState_1_0_5233, 7)

        { 0x05, 0x81 },
        { 0x0C, 0xF6 },
        { 0x13, 0x57 },
        { 0x1A, 0x08 },
        { 0x21, 0x8B },
        { 0x28, 0x88 },
        { 0x30, 0xC2 },
OOVPA_END;

// ******************************************************************
// * XAPI_1_0_5233
// ******************************************************************
OOVPATable XAPI_1_0_5233[] = {

    // GetTimeZoneInformation (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(GetTimeZoneInformation_1_0_3911, XTL::EmuGetTimeZoneInformation),
	// SetThreadPriority (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(SetThreadPriority_1_0_3911, XTL::EmuSetThreadPriority),
	// SetThreadPriorityBoost (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(SetThreadPriorityBoost_1_0_4627, XTL::EmuSetThreadPriorityBoost),
	// GetThreadPriority (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(GetThreadPriority_1_0_4627, XTL::EmuGetThreadPriority),
    // XRegisterThreadNotifyRoutine (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(XRegisterThreadNotifyRoutine_1_0_3911, XTL::EmuXRegisterThreadNotifyRoutine),
    // XMountUtilityDrive (* unchanged since 4432 *)
	OOVPA_TABLE_PATCH(XMountUtilityDrive_1_0_4432, XTL::EmuXMountUtilityDrive),
    // XInitDevices
	OOVPA_TABLE_PATCH(XInitDevices_1_0_5233, XTL::EmuXInitDevices),
    // XGetDevices (* unchanged since 1.0.4134 *)
	OOVPA_TABLE_PATCH(XGetDevices_1_0_4134, XTL::EmuXGetDevices),
    // XGetDeviceChanges
	OOVPA_TABLE_PATCH(XGetDeviceChanges_1_0_5233, XTL::EmuXGetDeviceChanges),
    // XInputOpen (* unchanged since 1.0.4361 *)
	OOVPA_TABLE_PATCH(XInputOpen_1_0_4361, XTL::EmuXInputOpen),
    // XInputGetCapabilities
	OOVPA_TABLE_PATCH(XInputGetCapabilities_1_0_5233, XTL::EmuXInputGetCapabilities),
    // XID_fCloseDevice (* unchanged since 1.0.4928 *)
	OOVPA_TABLE_XREF(XID_fCloseDevice_1_0_4928),
    // XInputClose (* unchanged since 1.0.4928 *)
	OOVPA_TABLE_PATCH(XInputClose_1_0_4928, XTL::EmuXInputClose),
    // XInputGetState (* unchanged since 1.0.4928 *)
	OOVPA_TABLE_PATCH(XInputGetState_1_0_4928, XTL::EmuXInputGetState),
    // XInputSetState
	OOVPA_TABLE_PATCH(XInputSetState_1_0_5233, XTL::EmuXInputSetState),
	// QueueUserAPC (* unchanged since 3911 *)
	OOVPA_TABLE_PATCH(QueueUserAPC_1_0_3911, XTL::EmuQueueUserAPC),
};

// ******************************************************************
// * XAPI_1_0_5233_SIZE
// ******************************************************************
uint32 XAPI_1_0_5233_SIZE = sizeof(XAPI_1_0_5233);
