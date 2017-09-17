// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->Xapi.1.0.5344.cpp
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
// * XInputGetDeviceDescription
// ******************************************************************
OOVPA_NO_XREF(XInputGetDeviceDescription, 5344, 15)

        { 0x04, 0xEC },
        { 0x0B, 0x15 },

        { 0x13, 0x45 },
        { 0x14, 0x08 },
        { 0x15, 0x8B },
        { 0x16, 0x30 },
        { 0x17, 0x3B },
        { 0x18, 0xF3 },
        { 0x19, 0x88 },
        { 0x1A, 0x4D },
        { 0x1B, 0xFF },
        { 0x1C, 0x0F },
        { 0x1D, 0x84 },

        { 0x30, 0x45 },
        { 0x31, 0xF8 },
OOVPA_END;

// ******************************************************************
// * XLaunchNewImageA
// ******************************************************************
OOVPA_NO_XREF(XLaunchNewImageA, 5344, 11)

        { 0x00, 0xB8 },

        { 0x18, 0x75 },
        { 0x19, 0x19 },
        { 0x1A, 0x83 },
        { 0x1B, 0xC1 },
        { 0x1C, 0x04 },
        { 0x1D, 0x3B },
        { 0x1E, 0xC8 },
        { 0x1F, 0x72 },

        { 0x30, 0xC2 },
        { 0x41, 0xEE },
OOVPA_END;

// ******************************************************************
// * XAPI_5344
// ******************************************************************
OOVPATable XAPI_5344[] = {

	REGISTER_OOVPA(GetTimeZoneInformation, 3911, DISABLED),
	REGISTER_OOVPA(SetThreadPriority, 3911, PATCH),
	REGISTER_OOVPA(SetThreadPriorityBoost, 3911, PATCH),
	REGISTER_OOVPA(GetThreadPriority, 3911, PATCH),
	REGISTER_OOVPA(GetTypeInformation, 4134, XREF),
	REGISTER_OOVPA(XRegisterThreadNotifyRoutine, 3911, PATCH),
	REGISTER_OOVPA(XMountUtilityDrive, 4432, PATCH),
	REGISTER_OOVPA(XInitDevices, 5233, PATCH),
	REGISTER_OOVPA(XGetDevices, 3911, PATCH),
	REGISTER_OOVPA(XGetDeviceChanges, 5233, PATCH),
	REGISTER_OOVPA(XInputOpen, 4242, PATCH),
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
	REGISTER_OOVPA(XLaunchNewImageA, 5344, PATCH),
	REGISTER_OOVPA(XMountMURootA, 4361, PATCH),
	REGISTER_OOVPA(XInputPoll, 3911, PATCH),
	REGISTER_OOVPA(timeSetEvent, 3911, PATCH),
	REGISTER_OOVPA(timeKillEvent, 3911, PATCH),
	REGISTER_OOVPA(XFormatUtilityDrive, 4361, PATCH),
	REGISTER_OOVPA(GetOverlappedResult, 3911, PATCH),
	REGISTER_OOVPA(XSetProcessQuantumLength, 4134, PATCH),
	REGISTER_OOVPA(RaiseException, 3911, PATCH),
	REGISTER_OOVPA(XInputGetDeviceDescription, 5344, PATCH),
	REGISTER_OOVPA(XGetDeviceEnumerationStatus, 4831, PATCH),
};

// ******************************************************************
// * XAPI_5344_SIZE
// ******************************************************************
uint32 XAPI_5344_SIZE = sizeof(XAPI_5344);
