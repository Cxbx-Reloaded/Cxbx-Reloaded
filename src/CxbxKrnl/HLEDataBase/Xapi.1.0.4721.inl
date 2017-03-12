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
    // REGISTER_OOVPA(RtlCreateHeap, 3911, PATCH), // obsolete, (* unchanged since 1.0.4361 *) (* OR FARTHER *)
    // REGISTER_OOVPA(RtlAllocateHeap, 3911, PATCH), // obsolete (* unchanged since 1.0.4361 *) (* OR FARTHER *)
    // REGISTER_OOVPA(RtlReAllocateHeap, 4627, PATCH), // obsolete 
    // REGISTER_OOVPA(RtlFreeHeap, 4627, PATCH), // obsolete 
    // REGISTER_OOVPA(RtlSizeHeap, 4627, PATCH), // obsolete 
	// REGISTER_OOVPA(RtlDestroyHeap, 4627, PATCH), // obsolete 
	REGISTER_OOVPA(XMountUtilityDrive, 4432, PATCH),
	REGISTER_OOVPA(XInitDevices, 3911, PATCH),
	REGISTER_OOVPA(XGetDevices, 4134, PATCH),
	REGISTER_OOVPA(XInputOpen, 4361, PATCH),
	REGISTER_OOVPA(XInputGetCapabilities, 4361, PATCH),
	REGISTER_OOVPA(XInputGetState, 4361, PATCH),
	REGISTER_OOVPA(XInputSetState, 4361, PATCH),
	REGISTER_OOVPA(XID_fCloseDevice, 4361, XREF),
	REGISTER_OOVPA(XInputClose, 3911, PATCH),
	// REGISTER_OOVPA(XInputClose, 4361, PATCH),
	// REGISTER_OOVPA(XapiThreadStartup, 4361, PATCH), // obsolete 
	// REGISTER_OOVPA(XapiInitProcess, 4361, PATCH), // obsolete, Too High Level
    // REGISTER_OOVPA(XapiBootDash, 3911, PATCH), // obsolete 
	REGISTER_OOVPA(CreateFiber, 3911, DISABLED),
	REGISTER_OOVPA(DeleteFiber, 3911, DISABLED),
	REGISTER_OOVPA(SwitchToFiber, 3911, DISABLED),
	REGISTER_OOVPA(ConvertThreadToFiber, 3911, DISABLED),
};

// ******************************************************************
// * XAPI_4721_SIZE
// ******************************************************************
uint32 XAPI_4721_SIZE = sizeof(XAPI_4721);
