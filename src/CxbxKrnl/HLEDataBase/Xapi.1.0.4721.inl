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
    // OOVPA_TABLE_ENTRY(RtlCreateHeap, 3911), // obsolete, (* unchanged since 1.0.4361 *) (* OR FARTHER *)
    // OOVPA_TABLE_ENTRY(RtlAllocateHeap, 3911), // obsolete (* unchanged since 1.0.4361 *) (* OR FARTHER *)
    // OOVPA_TABLE_ENTRY(RtlReAllocateHeap, 4627), // obsolete 
    // OOVPA_TABLE_ENTRY(RtlFreeHeap, 4627), // obsolete 
    // OOVPA_TABLE_ENTRY(RtlSizeHeap, 4627), // obsolete 
	// OOVPA_TABLE_ENTRY(RtlDestroyHeap, 4627), // obsolete 
	OOVPA_TABLE_ENTRY(XMountUtilityDrive, 4432),
	OOVPA_TABLE_ENTRY(XInitDevices, 3911),
	OOVPA_TABLE_ENTRY(XGetDevices, 4134),
	OOVPA_TABLE_ENTRY(XInputOpen, 4361),
	OOVPA_TABLE_ENTRY(XInputGetCapabilities, 4361),
	OOVPA_TABLE_ENTRY(XInputGetState, 4361),
	OOVPA_TABLE_ENTRY(XInputSetState, 4361),
	OOVPA_TABLE_XREF(XID_fCloseDevice, 4361),
	OOVPA_TABLE_ENTRY(XInputClose, 3911),
	// OOVPA_TABLE_ENTRY(XInputClose, 4361),
	// OOVPA_TABLE_ENTRY(XapiThreadStartup, 4361), // obsolete 
	// OOVPA_TABLE_ENTRY(XapiInitProcess, 4361), // obsolete, Too High Level
    // OOVPA_TABLE_ENTRY(XapiBootDash, 3911), // obsolete 
};

// ******************************************************************
// * XAPI_4721_SIZE
// ******************************************************************
uint32 XAPI_4721_SIZE = sizeof(XAPI_4721);
