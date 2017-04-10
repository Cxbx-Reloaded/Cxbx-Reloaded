// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->Xapi.1.0.5788.cpp
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
// * XapiInitProcess
// ******************************************************************
OOVPA_NO_XREF(XapiInitProcess, 5788, 8)

        { 0x22, 0x6A },
        { 0x3E, 0x01 },
        { 0x5E, 0x75 },
        { 0x7E, 0x5E },
        { 0x9E, 0x7D },
        { 0xBE, 0x51 },
        { 0xDE, 0x00 },
        { 0xFE, 0x53 },
OOVPA_END;

// ******************************************************************
// * XGetDeviceChanges
// ******************************************************************
OOVPA_NO_XREF(XGetDeviceChanges, 5788, 7)

        { 0x0C, 0x75 },
        { 0x1A, 0x53 },
        { 0x28, 0xF7 },
        { 0x36, 0x56 },
        { 0x44, 0x89 },
        { 0x52, 0xC8 },
        { 0x60, 0x5F },
OOVPA_END;

// ******************************************************************
// * GetThreadPriority
// ******************************************************************
OOVPA_NO_XREF(GetThreadPriority, 5788, 7)

        { 0x0D, 0xFF },
        { 0x16, 0x85 },
        { 0x1F, 0x15 },
        { 0x2A, 0x04 },
        { 0x35, 0xF1 },
        { 0x40, 0x8B },
        { 0x4B, 0xB8 },
OOVPA_END;

// ******************************************************************
// * SetThreadPriorityBoost
// ******************************************************************
OOVPA_NO_XREF(SetThreadPriorityBoost, 5788, 7)

        { 0x07, 0xFF },
        { 0x10, 0xFF },
        { 0x19, 0x20 },
        { 0x22, 0x50 },
        { 0x2C, 0x8B },
        { 0x35, 0x33 },
        { 0x40, 0x33 },
OOVPA_END;

// ******************************************************************
// * XGetSectionSize
// ******************************************************************
OOVPA_NO_XREF(XGetSectionSize, 5788, 5)

        { 0x00, 0x8B },
        { 0x02, 0x24 },
        { 0x04, 0x8B },
        { 0x06, 0x08 },
        { 0x08, 0x04 },
OOVPA_END;

// ******************************************************************
// * RaiseException
// ******************************************************************
OOVPA_NO_XREF(RaiseException, 5788, 7)

        { 0x09, 0x83 },
        { 0x14, 0x8B },
        { 0x1F, 0xC7 },
        { 0x2A, 0x10 },
        { 0x35, 0x89 },
        { 0x40, 0x5F },
        { 0x4B, 0xFF },
OOVPA_END;

// ******************************************************************
// * XGetDeviceEnumerationStatus
// ******************************************************************
OOVPA_NO_XREF(XGetDeviceEnumerationStatus, 5788, 7)

        { 0x04, 0x15 },
        { 0x0A, 0x35 },
        { 0x10, 0x09 },
        { 0x17, 0x00 },
        { 0x1C, 0x46 },
        { 0x25, 0x8B },
        { 0x28, 0xC3 },
OOVPA_END;

// ******************************************************************
// * SwitchToThread
// ******************************************************************
OOVPA_NO_XREF(SwitchToThread, 5788, 6)

        { 0x01, 0x15 },
        { 0x06, 0x33 },
        { 0x07, 0xC9 },
        { 0x0A, 0x00 },
        { 0x0D, 0x0F },
        { 0x10, 0x8B },
OOVPA_END;

// ******************************************************************
// * XAPI_5788
// ******************************************************************
OOVPATable XAPI_5788[] = {

	REGISTER_OOVPA(SetThreadPriority, 3911, PATCH),
	REGISTER_OOVPA(XMountUtilityDrive, 4432, PATCH),
	REGISTER_OOVPA(XInitDevices, 5233, PATCH),
	REGISTER_OOVPA(XGetDevices, 4134, PATCH),
	REGISTER_OOVPA(XInputOpen, 4361, PATCH),
	REGISTER_OOVPA(XInputGetCapabilities, 5558, PATCH),
	REGISTER_OOVPA(XInputGetState, 5558, PATCH),
	REGISTER_OOVPA(XGetDeviceChanges, 5788, DISABLED), // Was PATCH
    // REGISTER_OOVPA(XapiThreadStartup, 4361, PATCH), // obsolete?
	REGISTER_OOVPA(XGetLaunchInfo, 5558, PATCH),
	REGISTER_OOVPA(QueueUserAPC, 3911, PATCH),
	REGISTER_OOVPA(XRegisterThreadNotifyRoutine, 3911, PATCH),
	REGISTER_OOVPA(GetThreadPriority, 5788, PATCH),
	REGISTER_OOVPA(SetThreadPriorityBoost, 5788, PATCH),
	// REGISTER_OOVPA(GetThreadPriorityBoost, 5788, PATCH),
	REGISTER_OOVPA(XMountMUA, 5558, PATCH),
	REGISTER_OOVPA(GetTimeZoneInformation, 3911, DISABLED),
	REGISTER_OOVPA(RaiseException, 5788, PATCH),
	REGISTER_OOVPA(XLaunchNewImage, 5558, ALIAS, XLaunchNewImageA),
	REGISTER_OOVPA(XInputSetState, 5233, PATCH),
	REGISTER_OOVPA(XGetDeviceEnumerationStatus, 5788, PATCH),
	// REGISTER_OOVPA(SwitchToThread, 5788, PATCH),
	REGISTER_OOVPA(XFormatUtilityDrive, 4627, PATCH),
	REGISTER_OOVPA(CreateFiber, 3911, DISABLED),
	REGISTER_OOVPA(DeleteFiber, 3911, DISABLED),
	REGISTER_OOVPA(SwitchToFiber, 3911, DISABLED),
	REGISTER_OOVPA(ConvertThreadToFiber, 3911, DISABLED),
	REGISTER_OOVPA(XID_fCloseDevice, 5558, XREF),
	REGISTER_OOVPA(XInputClose, 5558, PATCH),
};

// ******************************************************************
// * XAPI_5788_SIZE
// ******************************************************************
uint32 XAPI_5788_SIZE = sizeof(XAPI_5788);
