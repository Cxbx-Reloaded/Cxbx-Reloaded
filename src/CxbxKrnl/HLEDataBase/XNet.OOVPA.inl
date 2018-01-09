// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;	 .,;;  ;;;'';;'   `;;;	 .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o	   oP"``"Yo	 _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDatabase->XNet.OOVPA.inl
// *
// *  This file is part of the Cxbx project.
// *
// *  Cxbx and Cxbe are free software; you can redistribute them
// *  and/or modify them under the terms of the GNU General Public
// *  License as published by the Free Software Foundation; either
// *  version 2 of the license	or (at your option) any later version.
// *
// *  This program is distributed in the hope that it will be useful,
// *  but WITHOUT ANY WARRANTY; without even the implied warranty of
// *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// *  GNU General Public License for more details.
// *
// *  You should have recieved a copy of the GNU General Public License
// *  along with this program; see the file COPYING.
// *  If not	write to the Free Software Foundation	Inc.,
// *  59 Temple Place - Suite 330	Bostom	MA 02111-1307	USA.
// *
// *  (c) 2017 jarupxx
// *
// *  All rights reserved
// *
// ******************************************************************

// Titles which did compiled with full library version
//   [LibV] Title Name                        | Verify  |  Comments
//-------------------------------------------------------------------
// * [3911] Halo                              |  100%   | Contain full library.
// * [4039] Tony Hawk's Pro Skater 3          |  100%   | have 10/11 library.
// * [4361] MotoGP                            |  100%   | Contain full library.
// * [4721] TimeSplitters 2                   |  100%   | have 8/11 library.
// * [5344] Stake                             |  100%   | have 10/11 XNETN library.

// TODO: Known XNET OOVPA issue list
// * XnInit 3911 is for only XNETS library. XNET library is different one.
// * Verification needed: Function Name ( Revision )
//   * connect (4721)
//   * listen (4039, 4721)
//   * send (4721)
//   * XNetGetEthernetLinkStatus (5344)

#ifndef XNET_OOVPA_INL
#define XNET_OOVPA_INL

#include "../OOVPA.h"

#include "XNet.1.0.3911.inl"
#include "XNet.1.0.4361.inl"
#include "XNet.1.0.4627.inl"
#include "XNet.1.0.5120.inl"
#include "XNet.1.0.5455.inl"

// ******************************************************************
// * XNET_OOVPA
// ******************************************************************
OOVPATable XNET_OOVPAV2[] = {

	REGISTER_OOVPAS(XnInit, XREF, 3911, 4361), // 3911 is only XNETS library, XNET library is different OOVPA.
	REGISTER_OOVPAS(WSAStartup, PATCH, 3911, 4361),
	REGISTER_OOVPAS(XNetStartup, PATCH, 3911, 4361),
	REGISTER_OOVPAS(XNetGetEthernetLinkStatus, PATCH, 3911, 4627),
	REGISTER_OOVPAS(bind, PATCH, 3911, 4627),
	REGISTER_OOVPAS(connect, PATCH, 3911, 5120),
	REGISTER_OOVPAS(ioctlsocket, PATCH, 3911, 4627),
	REGISTER_OOVPAS(listen, PATCH, 3911, 4627),
	REGISTER_OOVPAS(recv, PATCH, 3911),
	REGISTER_OOVPAS(send, PATCH, 3911),
	REGISTER_OOVPAS(socket, PATCH, 3911, 4627, 5455),
};

// ******************************************************************
// * XNET_OOVPA_COUNTV2
// ******************************************************************
const uint32 XNET_OOVPA_COUNTV2 = OOVPA_TABLE_COUNT(XNET_OOVPAV2);

#endif
