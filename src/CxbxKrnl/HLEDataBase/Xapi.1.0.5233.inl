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

    // GetTimeZoneInformation (* unchanged since 3911 *)
    OOVPA_TABLE_ENTRY(GetTimeZoneInformation, 3911),
    // SetThreadPriority (* unchanged since 3911 *)
    OOVPA_TABLE_ENTRY(SetThreadPriority, 3911),
    // SetThreadPriorityBoost (* unchanged since 4627 *)
    OOVPA_TABLE_ENTRY(SetThreadPriorityBoost, 4627),
    // GetThreadPriority (* unchanged since 4627 *)
    OOVPA_TABLE_ENTRY(GetThreadPriority, 4627),
    // XRegisterThreadNotifyRoutine (* unchanged since 3911 *)
    OOVPA_TABLE_ENTRY(XRegisterThreadNotifyRoutine, 3911),
    // XMountUtilityDrive (* unchanged since 4432 *)
    OOVPA_TABLE_ENTRY(XMountUtilityDrive, 4432),
    // XInitDevices
    OOVPA_TABLE_ENTRY(XInitDevices, 5233),
    // XGetDevices (* unchanged since 1.0.4134 *)
    OOVPA_TABLE_ENTRY(XGetDevices, 4134),
    // XGetDeviceChanges
    OOVPA_TABLE_ENTRY(XGetDeviceChanges, 5233),
    // XInputOpen (* unchanged since 1.0.4361 *)
    OOVPA_TABLE_ENTRY(XInputOpen, 4361),
    // XInputGetCapabilities
    OOVPA_TABLE_ENTRY(XInputGetCapabilities, 5233),
    // XID_fCloseDevice (* unchanged since 1.0.4928 *)
    OOVPA_TABLE_XREF(XID_fCloseDevice, 4928),
    // XInputClose (* unchanged since 1.0.4928 *)
    OOVPA_TABLE_ENTRY(XInputClose, 4928),
    // XInputGetState (* unchanged since 1.0.4928 *)
    OOVPA_TABLE_ENTRY(XInputGetState, 4928),
    // XInputSetState
    OOVPA_TABLE_ENTRY(XInputSetState, 5233),
    // QueueUserAPC (* unchanged since 3911 *)
    OOVPA_TABLE_ENTRY(QueueUserAPC, 3911),
};

// ******************************************************************
// * XAPI_5233_SIZE
// ******************************************************************
uint32 XAPI_5233_SIZE = sizeof(XAPI_5233);
