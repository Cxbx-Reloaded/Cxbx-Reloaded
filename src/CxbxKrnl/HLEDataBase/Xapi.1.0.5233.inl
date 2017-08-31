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
OOVPA_NO_XREF(XInitDevices, 5233, 7)

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
OOVPA_NO_XREF(XInputGetCapabilities, 5233, 8)

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
OOVPA_NO_XREF(XGetDeviceChanges, 5233, 7)

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
OOVPA_NO_XREF(XInputSetState, 5233, 7)

        { 0x05, 0x81 },
        { 0x0C, 0xF6 },
        { 0x13, 0x57 },
        { 0x1A, 0x08 },
        { 0x21, 0x8B },
        { 0x28, 0x88 },
        { 0x30, 0xC2 },
OOVPA_END;

// ******************************************************************
// * XAPI_5233
// ******************************************************************
OOVPATable XAPI_5233[] = {

	REGISTER_OOVPA(GetTimeZoneInformation, 3911, DISABLED),
	REGISTER_OOVPA(SetThreadPriority, 3911, PATCH),
	REGISTER_OOVPA(SetThreadPriorityBoost, 3911, PATCH),
	REGISTER_OOVPA(GetThreadPriority, 3911, PATCH),
	REGISTER_OOVPA(XRegisterThreadNotifyRoutine, 3911, PATCH),
	REGISTER_OOVPA(XMountUtilityDrive, 4432, PATCH),
	REGISTER_OOVPA(XInitDevices, 5233, PATCH),
	REGISTER_OOVPA(XGetDevices, 3911, PATCH),
	REGISTER_OOVPA(XGetDeviceChanges, 5233, PATCH),
	REGISTER_OOVPA(XInputOpen, 4361, PATCH),
	REGISTER_OOVPA(XInputGetCapabilities, 5233, PATCH),
	REGISTER_OOVPA(XID_fCloseDevice, 4928, XREF),
	REGISTER_OOVPA(XInputClose, 3911, PATCH),
	REGISTER_OOVPA(XInputGetState, 4928, PATCH),
	REGISTER_OOVPA(XInputSetState, 5233, PATCH),
	REGISTER_OOVPA(QueueUserAPC, 3911, PATCH),
	REGISTER_OOVPA(CreateFiber, 3911, DISABLED),
	REGISTER_OOVPA(DeleteFiber, 3911, DISABLED),
	REGISTER_OOVPA(SwitchToFiber, 3911, DISABLED),
	REGISTER_OOVPA(ConvertThreadToFiber, 3911, DISABLED),
	REGISTER_OOVPA(OutputDebugStringA, 3911, PATCH),
	REGISTER_OOVPA(GetExitCodeThread, 3911, PATCH),
	REGISTER_OOVPA(SignalObjectAndWait, 3911, PATCH),
	REGISTER_OOVPA(XMountAlternateTitleA, 5028, PATCH),
	REGISTER_OOVPA(XUnmountAlternateTitleA, 3911, PATCH),
	REGISTER_OOVPA(XMountMUA, 4361, PATCH),
	REGISTER_OOVPA(XLaunchNewImageA, 4721, PATCH),
	REGISTER_OOVPA(XInputPoll, 3911, PATCH),
	REGISTER_OOVPA(XFormatUtilityDrive, 4361, PATCH),
	REGISTER_OOVPA(GetOverlappedResult, 3911, PATCH),
	REGISTER_OOVPA(XSetProcessQuantumLength, 4134, PATCH),
	REGISTER_OOVPA(RaiseException, 3911, PATCH),
	REGISTER_OOVPA(XGetDeviceEnumerationStatus, 4831, PATCH),

	// ******************************************************************
	// Provisional registration functions in XDK 5233
	// TODO: Need test cases
	// ******************************************************************
	REGISTER_OOVPA(XMountMURootA, 4361, PATCH),
	REGISTER_OOVPA(timeSetEvent, 3911, PATCH),
	REGISTER_OOVPA(timeKillEvent, 3911, PATCH),
	// ******************************************************************
};

// ******************************************************************
// * XAPI_5233_SIZE
// ******************************************************************
uint32 XAPI_5233_SIZE = sizeof(XAPI_5233);
