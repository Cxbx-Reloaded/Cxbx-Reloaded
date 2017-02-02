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

    // SetThreadPriority (* unchanged since 3911 *)
    OOVPA_TABLE_ENTRY(SetThreadPriority, 3911),
    // XMountUtilityDrive (* unchanged since 4432 *)
    OOVPA_TABLE_ENTRY(XMountUtilityDrive, 4432),
    // XInitDevices (* unchanged since 5233 *)
    OOVPA_TABLE_ENTRY(XInitDevices, 5233),
    // XGetDevices (* unchanged since 1.0.4134 *)
    OOVPA_TABLE_ENTRY(XGetDevices, 4134),
    // XInputOpen (* unchanged since 1.0.4361 *)
    OOVPA_TABLE_ENTRY(XInputOpen, 4361),
    // XInputGetCapabilities (* unchanged since 5558 *)
    OOVPA_TABLE_ENTRY(XInputGetCapabilities, 5558),
    // XInputGetState (* unchanged since 5558 *)
    OOVPA_TABLE_ENTRY(XInputGetState, 5558),
    // XGetDeviceChanges
    OOVPA_TABLE_ENTRY(XGetDeviceChanges, 5788),
    /* obsolete?
    // XapiThreadStartup (* unchanged since 1.0.4361 *)
    OOVPA_TABLE_ENTRY(XapiThreadStartup, 4361), //*/
    // XGetLaunchInfo (* unchanged since 5558 *)
    OOVPA_TABLE_ENTRY(XGetLaunchInfo, 5558),
    // QueueUserAPC (* unchanged since 3911 *)
    OOVPA_TABLE_ENTRY(QueueUserAPC, 3911),
    // XRegisterThreadNotifyRoutine (* unchanged since 3911 *)
    OOVPA_TABLE_ENTRY(XRegisterThreadNotifyRoutine, 3911),
    // GetThreadPriority
    OOVPA_TABLE_ENTRY(GetThreadPriority, 5788),
    // SetThreadPriorityBoost
    OOVPA_TABLE_ENTRY(SetThreadPriorityBoost, 5788),
    // GetThreadPriorityBoost
    /*{
    OOVPA_TABLE_ENTRY(GetThreadPriorityBoost, 5788),*/
    // XMountMUA (* unchanged since 5558 *)
    OOVPA_TABLE_ENTRY(XMountMUA, 5558),
    // GetTimeZoneInformation (* unchanged since 3911 *)
    OOVPA_TABLE_ENTRY(GetTimeZoneInformation, 3911),
    // RaiseException
    OOVPA_TABLE_ENTRY(RaiseException, 5788),
    // XLaunchNewImageA (* unchanged since 5558 *)
    OOVPA_TABLE_PATCH(XLaunchNewImageA, 5558, XLaunchNewImage),
    // XInputSetState (* unchanged since 5233 *)
    OOVPA_TABLE_ENTRY(XInputSetState, 5233),
    // XGetDeviceEnumerationStatus
    OOVPA_TABLE_ENTRY(XGetDeviceEnumerationStatus, 5788),
    // SwitchToThread
    /*
    OOVPA_TABLE_ENTRY(SwitchToThread, 5788),
    */
    // XFormatUtilityDrive (* unchanged since 4627 *)
    OOVPA_TABLE_ENTRY(XFormatUtilityDrive, 4627),
    // CreateFiber
    OOVPA_TABLE_ENTRY(CreateFiber, 5558),
    // DeleteFiber
    OOVPA_TABLE_ENTRY(DeleteFiber, 5558),
    // SwitchToFiber
    OOVPA_TABLE_ENTRY(SwitchToFiber, 5558),
    // ConvertThreadToFiber
    OOVPA_TABLE_ENTRY(ConvertThreadToFiber, 5558),
    // XID_fCloseDevice (* unchanged since 5558 *)
    OOVPA_TABLE_XREF(XID_fCloseDevice, 5558),
    // XInputClose (* unchanged since 5558 *)
    OOVPA_TABLE_ENTRY(XInputClose, 5558),
};

// ******************************************************************
// * XAPI_5788_SIZE
// ******************************************************************
uint32 XAPI_5788_SIZE = sizeof(XAPI_5788);
