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
// * XAPI_1_0_4721
// ******************************************************************
OOVPATable XAPI_1_0_4721[] = {
	/*
    // RtlCreateHeap (* unchanged since 1.0.4361 *) (* OR FARTHER *)
    obsolete OOVPA_TABLE_PATCH(RtlCreateHeap_1_0_3911, XTL::EmuRtlCreateHeap),
    // RtlAllocateHeap (* unchanged since 1.0.4361 *) (* OR FARTHER *)
    obsolete OOVPA_TABLE_PATCH(RtlAllocateHeap_1_0_3911, XTL::EmuRtlAllocateHeap),
	// RtlReAllocateHeap
    obsolete OOVPA_TABLE_PATCH(RtlReAllocateHeap_1_0_4627, XTL::EmuRtlReAllocateHeap),
    // RtlFreeHeap
    obsolete OOVPA_TABLE_PATCH(RtlFreeHeap_1_0_4627, XTL::EmuRtlFreeHeap),
    // RtlSizeHeap
    obsolete OOVPA_TABLE_PATCH(RtlSizeHeap_1_0_4627, XTL::EmuRtlSizeHeap),
	// RtlDestroyHeap
	obsolete OOVPA_TABLE_PATCH(RtlDestroyHeap_1_0_4627, XTL::EmuRtlDestroyHeap),
	*/
    // XMountUtilityDrive
	OOVPA_TABLE_PATCH(XMountUtilityDrive_1_0_4432, XTL::EmuXMountUtilityDrive),
    // XInitDevices (* unchanged since 1.0.3911 *)
	OOVPA_TABLE_PATCH(XInitDevices_1_0_3911, XTL::EmuXInitDevices),
    // XGetDevices (* unchanged since 1.0.4134 *)
	OOVPA_TABLE_PATCH(XGetDevices_1_0_4134, XTL::EmuXGetDevices),
    // XInputOpen (* unchanged since 1.0.4361 *)
	OOVPA_TABLE_PATCH(XInputOpen_1_0_4361, XTL::EmuXInputOpen),
    // XInputGetCapabilities (* unchanged since 1.0.4361 *)
	OOVPA_TABLE_PATCH(XInputGetCapabilities_1_0_4361, XTL::EmuXInputGetCapabilities),
    // XInputGetState (* unchanged since 1.0.4361 *)
	OOVPA_TABLE_PATCH(XInputGetState_1_0_4361, XTL::EmuXInputGetState),
    // XInputSetState (* unchanged since 1.0.4361 *)
	OOVPA_TABLE_PATCH(XInputSetState_1_0_4361, XTL::EmuXInputSetState),
	// XID_fCloseDevice (* unchanged since 1.0.4361 *)
	OOVPA_TABLE_XREF(XID_fCloseDevice_1_0_4361),
	// XInputClose (* unchanged since 1.0.3911 *)
	OOVPA_TABLE_PATCH(XInputClose_1_0_3911, XTL::EmuXInputClose),
	// XInputClose (* unchanged since 1.0.4361 *)
	// OOVPA_TABLE_PATCH(XInputClose_1_0_4361, XTL::EmuXInputClose),
    // XapiThreadStartup (* unchanged since 1.0.4361 *)
	// obsolete OOVPA_TABLE_PATCH(XapiThreadStartup_1_0_4361, XTL::EmuXapiThreadStartup),
    //* Too High Level
    // XapiInitProcess (* unchanged since 1.0.4361 *)
	// obsolete OOVPA_TABLE_PATCH(XapiInitProcess_1_0_4361, XTL::EmuXapiInitProcess),
    //*/
    // XapiBootToDash (* unchanged since 1.0.3911 *)
    /* obsolete OOVPA_TABLE_PATCH(XapiBootDash_1_0_3911, XTL::EmuXapiBootDash),*/
};

// ******************************************************************
// * XAPI_1_0_4721_SIZE
// ******************************************************************
uint32 XAPI_1_0_4721_SIZE = sizeof(XAPI_1_0_4721);
