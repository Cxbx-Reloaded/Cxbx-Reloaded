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
OOVPA_NO_XREF(XMountUtilityDrive, 4432, 8)

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
// * XAPI_4432
// ******************************************************************
OOVPATable XAPI_4432[] = {
	REGISTER_OOVPA(XMountUtilityDrive, 4432, PATCH),
	REGISTER_OOVPA(XInitDevices, 3911, PATCH),
	REGISTER_OOVPA(XGetDevices, 4134, PATCH),
	REGISTER_OOVPA(XInputOpen, 4361, PATCH),
	REGISTER_OOVPA(XID_fCloseDevice, 4361, XREF),
	REGISTER_OOVPA(XInputClose, 3911, PATCH),
	REGISTER_OOVPA(XInputGetCapabilities, 4361, PATCH),
	REGISTER_OOVPA(XInputGetState, 4361, PATCH),
	REGISTER_OOVPA(XInputSetState, 4361, PATCH),
    // REGISTER_OOVPA(XapiThreadStartup, 4361, PATCH),
	REGISTER_OOVPA(GetExitCodeThread, 3911, PATCH),
	REGISTER_OOVPA(GetTimeZoneInformation, 3911, DISABLED),
	REGISTER_OOVPA(SetThreadPriority, 3911, PATCH),
	REGISTER_OOVPA(SignalObjectAndWait, 3911, PATCH),
	REGISTER_OOVPA(CreateFiber, 3911, PATCH),
	REGISTER_OOVPA(DeleteFiber, 3911, PATCH),
	REGISTER_OOVPA(SwitchToFiber, 3911, PATCH),
	REGISTER_OOVPA(ConvertThreadToFiber, 3911, PATCH),
	REGISTER_OOVPA(QueueUserAPC, 3911, PATCH),
	REGISTER_OOVPA(timeSetEvent, 4134, PATCH),
	REGISTER_OOVPA(timeKillEvent, 4134, PATCH),
	REGISTER_OOVPA(XLaunchNewImage, 3911, ALIAS, XLaunchNewImageA),
};

// ******************************************************************
// * XAPI_4432_SIZE
// ******************************************************************
uint32 XAPI_4432_SIZE = sizeof(XAPI_4432);
