// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->Xapi.1.0.5028.cpp
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
// * XMountAlternateTitleA
// ******************************************************************
OOVPA_NO_XREF(XMountAlternateTitleA, 5028, 14)

        { 0x04, 0xEC },

        { 0x0F, 0x01 },
        { 0x10, 0x00 },
        { 0x11, 0x53 },
        { 0x12, 0x8A },
        { 0x13, 0x19 },
        { 0x14, 0x88 },

        { 0x82, 0x83 },
        { 0x83, 0xC4 },
        { 0x84, 0x0C },
        { 0x85, 0x8D },
        { 0x86, 0x85 },
        { 0x91, 0x15 },

        { 0x96, 0x8D },
OOVPA_END;

// ******************************************************************
// * XapiInitProcess
// ******************************************************************
OOVPA_NO_XREF(XapiInitProcess, 5028, 12)

        { 0x00, 0x55 },
        { 0x01, 0x8B },

        { 0x0E, 0xF6 },
        { 0x0F, 0x68 },

        { 0x22, 0x6A },
        { 0x23, 0x0C },
        { 0x24, 0x59 },
        { 0x25, 0x33 },
        { 0x26, 0xC0 },
        { 0x27, 0x8D },
        { 0x28, 0x7D },
        { 0x29, 0xCC },
OOVPA_END;

// ******************************************************************
// * XAPI_5028
// ******************************************************************
OOVPATable XAPI_5028[] = {

	REGISTER_OOVPA(XFormatUtilityDrive, 4242, PATCH),
	REGISTER_OOVPA(SetThreadPriorityBoost, 3911, PATCH),
	REGISTER_OOVPA(SetThreadPriority, 3911, PATCH),
	REGISTER_OOVPA(GetThreadPriority, 3911, PATCH),
	REGISTER_OOVPA(GetTypeInformation, 4134, XREF),
	REGISTER_OOVPA(XRegisterThreadNotifyRoutine, 3911, PATCH),
	REGISTER_OOVPA(XMountUtilityDrive, 4432, PATCH),
	REGISTER_OOVPA(XInitDevices, 3911, PATCH),
	REGISTER_OOVPA(XGetDevices, 3911, PATCH),
	REGISTER_OOVPA(XInputOpen, 4242, PATCH),
	REGISTER_OOVPA(XInputPoll, 3911, PATCH),
	REGISTER_OOVPA(XID_fCloseDevice, 4831, XREF),
	REGISTER_OOVPA(XInputClose, 3911, PATCH),
	REGISTER_OOVPA(XInputGetState, 4831, PATCH),
	REGISTER_OOVPA(XInputGetCapabilities, 4831, PATCH),
	REGISTER_OOVPA(XInputSetState, 4831, PATCH),
	REGISTER_OOVPA(XGetDeviceChanges, 3911, PATCH),
	// REGISTER_OOVPA(XapiThreadStartup, 3911, PATCH), // obsolete?
	// REGISTER_OOVPA(XCalculateSignatureBeginEx, 4627, PATCH), // +s, not necessary?
	// REGISTER_OOVPA(XCalculateSignatureBegin, 4627, PATCH),
	// REGISTER_OOVPA(XCalculateSignatureUpdate, 4627, PATCH),
	// REGISTER_OOVPA(XCalculateSignatureEnd, 4627, PATCH), // s+
	REGISTER_OOVPA(CreateFiber, 3911, DISABLED),
	REGISTER_OOVPA(DeleteFiber, 3911, DISABLED),
	REGISTER_OOVPA(SwitchToFiber, 3911, DISABLED),
	REGISTER_OOVPA(ConvertThreadToFiber, 3911, DISABLED),
	REGISTER_OOVPA(GetTimeZoneInformation, 3911, DISABLED),
	REGISTER_OOVPA(GetExitCodeThread, 3911, PATCH),
	REGISTER_OOVPA(GetOverlappedResult, 3911, PATCH),
	REGISTER_OOVPA(XLaunchNewImageA, 4721, PATCH),
	REGISTER_OOVPA(XGetLaunchInfo, 3911, DISABLED),
	REGISTER_OOVPA(XSetProcessQuantumLength, 4134, PATCH),
	REGISTER_OOVPA(SignalObjectAndWait, 3911, PATCH),
	REGISTER_OOVPA(timeSetEvent, 3911, PATCH),
	REGISTER_OOVPA(RaiseException, 3911, PATCH),
	REGISTER_OOVPA(QueueUserAPC, 3911, PATCH),
	REGISTER_OOVPA(XMountAlternateTitleA, 5028, PATCH),
	REGISTER_OOVPA(XUnmountAlternateTitleA, 3911, PATCH),
	// REGISTER_OOVPA(XInputGetDeviceDescription, 4831, PATCH), // Was NOT XInputGetDeviceDescription
	REGISTER_OOVPA(OutputDebugStringA, 3911, PATCH),
	// REGISTER_OOVPA(MoveFileA, 3911, PATCH),
	REGISTER_OOVPA(XMountMUA, 4242, PATCH),
	REGISTER_OOVPA(XGetDeviceEnumerationStatus, 4831, PATCH),

	// ******************************************************************
	// Provisional registration functions in XDK 5028
	// TODO: Need test cases
	// ******************************************************************
	REGISTER_OOVPA(timeKillEvent, 3911, PATCH),
	REGISTER_OOVPA(XMountMURootA, 4242, PATCH),
	// ******************************************************************
};

// ******************************************************************
// * XAPI_5028_SIZE
// ******************************************************************
uint32 XAPI_5028_SIZE = sizeof(XAPI_5028);
