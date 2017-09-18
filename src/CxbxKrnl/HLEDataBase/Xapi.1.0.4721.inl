// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->Xapi.1.0.4721.cpp
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
// * XLaunchNewImageA
// ******************************************************************
OOVPA_NO_XREF(XLaunchNewImageA, 4721, 15)

        { 0x03, 0x81 },
        { 0x04, 0xEC },
        { 0x05, 0x84 },
        { 0x06, 0x03 },
        { 0x07, 0x00 },
        { 0x08, 0x00 },

        { 0x33, 0x3C },
        { 0x34, 0x44 },

        { 0xA0, 0x3B },
        { 0xA1, 0xC3 },
        { 0xA2, 0x7C },
        { 0xA3, 0x5A },
        { 0xA4, 0x57 },
        { 0xA5, 0x8D },

        { 0xC1, 0x15 },
OOVPA_END;

// ******************************************************************
// * XAPI_4721
// ******************************************************************
OOVPATable XAPI_4721[] = {

	// REGISTER_OOVPA(RtlCreateHeap, 3911, PATCH), // obsolete, (* unchanged since 1.0.4361 *) (* OR FARTHER *)
	// REGISTER_OOVPA(RtlAllocateHeap, 3911, PATCH), // obsolete (* unchanged since 1.0.4361 *) (* OR FARTHER *)
	// REGISTER_OOVPA(RtlReAllocateHeap, 4627, PATCH), // obsolete 
	// REGISTER_OOVPA(RtlFreeHeap, 4627, PATCH), // obsolete 
	// REGISTER_OOVPA(RtlSizeHeap, 4627, PATCH), // obsolete 
	// REGISTER_OOVPA(RtlDestroyHeap, 4627, PATCH), // obsolete 
	REGISTER_OOVPA(GetTypeInformation, 4134, XREF),
	REGISTER_OOVPA(XMountUtilityDrive, 4432, PATCH),
	REGISTER_OOVPA(XInitDevices, 3911, PATCH),
	REGISTER_OOVPA(XGetDevices, 3911, PATCH),
	REGISTER_OOVPA(XInputOpen, 4242, PATCH),
	REGISTER_OOVPA(XInputGetCapabilities, 4242, PATCH),
	REGISTER_OOVPA(XInputGetState, 4242, PATCH),
	REGISTER_OOVPA(XInputSetState, 4242, PATCH),
	REGISTER_OOVPA(XID_fCloseDevice, 4242, XREF),
	REGISTER_OOVPA(XInputClose, 3911, PATCH),
	REGISTER_OOVPA(XGetDeviceChanges, 3911, PATCH),
	// REGISTER_OOVPA(XapiThreadStartup, 4361, PATCH), // obsolete 
	// REGISTER_OOVPA(XapiInitProcess, 4361, PATCH), // obsolete, Too High Level
	// REGISTER_OOVPA(XapiBootDash, 3911, PATCH), // obsolete 
	REGISTER_OOVPA(CreateFiber, 3911, DISABLED),
	REGISTER_OOVPA(DeleteFiber, 3911, DISABLED),
	REGISTER_OOVPA(SwitchToFiber, 3911, DISABLED),
	REGISTER_OOVPA(ConvertThreadToFiber, 3911, DISABLED),
	REGISTER_OOVPA(OutputDebugStringA, 3911, PATCH),
	REGISTER_OOVPA(GetExitCodeThread, 3911, PATCH),
	REGISTER_OOVPA(SetThreadPriority, 3911, PATCH),
	REGISTER_OOVPA(XRegisterThreadNotifyRoutine, 3911, PATCH),
	REGISTER_OOVPA(SetThreadPriorityBoost, 3911, PATCH),
	REGISTER_OOVPA(GetThreadPriority, 3911, PATCH),
	REGISTER_OOVPA(SignalObjectAndWait, 3911, PATCH),
	REGISTER_OOVPA(QueueUserAPC, 3911, PATCH),
	REGISTER_OOVPA(XMountAlternateTitleA, 3911, PATCH),
	REGISTER_OOVPA(XUnmountAlternateTitleA, 3911, PATCH),
	REGISTER_OOVPA(XMountMUA, 4242, PATCH),
	REGISTER_OOVPA(XLaunchNewImageA, 4721, PATCH),
	REGISTER_OOVPA(XMountMURootA, 4242, PATCH),
	REGISTER_OOVPA(XInputPoll, 3911, PATCH),
	REGISTER_OOVPA(timeSetEvent, 3911, PATCH),
	REGISTER_OOVPA(timeKillEvent, 3911, PATCH),
	REGISTER_OOVPA(XFormatUtilityDrive, 4242, PATCH),
	REGISTER_OOVPA(GetOverlappedResult, 3911, PATCH),
	REGISTER_OOVPA(XSetProcessQuantumLength, 4134, PATCH),
	REGISTER_OOVPA(RaiseException, 3911, PATCH),
};

// ******************************************************************
// * XAPI_4721_SIZE
// ******************************************************************
uint32 XAPI_4721_SIZE = sizeof(XAPI_4721);
