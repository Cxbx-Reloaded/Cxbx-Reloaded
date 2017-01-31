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
// * XAPI_4721
// ******************************************************************
OOVPATable XAPI_4721[] = {
    /*
    // RtlCreateHeap (* unchanged since 1.0.4361 *) (* OR FARTHER *)
    obsolete OOVPA_TABLE_ENTRY(RtlCreateHeap, 3911),
    // RtlAllocateHeap (* unchanged since 1.0.4361 *) (* OR FARTHER *)
    obsolete OOVPA_TABLE_ENTRY(RtlAllocateHeap, 3911),
    // RtlReAllocateHeap
    obsolete OOVPA_TABLE_ENTRY(RtlReAllocateHeap, 4627),
    // RtlFreeHeap
    obsolete OOVPA_TABLE_ENTRY(RtlFreeHeap, 4627),
    // RtlSizeHeap
    obsolete OOVPA_TABLE_ENTRY(RtlSizeHeap, 4627),
    // RtlDestroyHeap
    obsolete OOVPA_TABLE_ENTRY(RtlDestroyHeap, 4627),
    */
    // XMountUtilityDrive
    OOVPA_TABLE_ENTRY(XMountUtilityDrive, 4432),
    // XInitDevices (* unchanged since 1.0.3911 *)
    OOVPA_TABLE_ENTRY(XInitDevices, 3911),
    // XGetDevices (* unchanged since 1.0.4134 *)
    OOVPA_TABLE_ENTRY(XGetDevices, 4134),
    // XInputOpen (* unchanged since 1.0.4361 *)
    OOVPA_TABLE_ENTRY(XInputOpen, 4361),
    // XInputGetCapabilities (* unchanged since 1.0.4361 *)
    OOVPA_TABLE_ENTRY(XInputGetCapabilities, 4361),
    // XInputGetState (* unchanged since 1.0.4361 *)
    OOVPA_TABLE_ENTRY(XInputGetState, 4361),
    // XInputSetState (* unchanged since 1.0.4361 *)
    OOVPA_TABLE_ENTRY(XInputSetState, 4361),
    // XID_fCloseDevice (* unchanged since 1.0.4361 *)
    OOVPA_TABLE_XREF(XID_fCloseDevice, 4361),
    // XInputClose (* unchanged since 1.0.3911 *)
    OOVPA_TABLE_ENTRY(XInputClose, 3911),
    // XInputClose (* unchanged since 1.0.4361 *)
    // OOVPA_TABLE_ENTRY(XInputClose, 4361),
    // XapiThreadStartup (* unchanged since 1.0.4361 *)
    // obsolete OOVPA_TABLE_ENTRY(XapiThreadStartup, 4361),
    //* Too High Level
    // XapiInitProcess (* unchanged since 1.0.4361 *)
    // obsolete OOVPA_TABLE_ENTRY(XapiInitProcess, 4361),
    //*/
    // XapiBootToDash (* unchanged since 1.0.3911 *)
    /* obsolete OOVPA_TABLE_ENTRY(XapiBootDash, 3911),*/
};

// ******************************************************************
// * XAPI_4721_SIZE
// ******************************************************************
uint32 XAPI_4721_SIZE = sizeof(XAPI_4721);
