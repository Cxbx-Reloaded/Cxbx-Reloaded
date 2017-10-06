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

// TODO: Known XNET OOVPA issue list
// * Verified each OOVPA with 3911, 4039, 4361, 4627.
// * XnInit is for only XNETS library. XNET library is different one.
// * Following OOVPA revision are not verified
//   * listen (4039)

#ifndef XNET_OOVPA_INL
#define XNET_OOVPA_INL

#include "../OOVPA.h"

#include "HLEDataBase/XNet.1.0.3911.inl"
#include "HLEDataBase/XNet.1.0.4361.inl"
#include "HLEDataBase/XNet.1.0.4627.inl"

// ******************************************************************
// * XNET_OOVPA
// ******************************************************************
OOVPATable XNET_OOVPAV2[] = {

	REGISTER_OOVPAS(XnInit, XREF, 3911, 4361/*, 4627*/), // 3911 is only XNETS library, XNET library is different OOVPA.
	REGISTER_OOVPAS(WSAStartup, PATCH, 3911, 4361/*, 4627*/),
	REGISTER_OOVPAS(XNetStartup, PATCH, 3911, 4361/*, 4627*/),
	REGISTER_OOVPAS(XNetGetEthernetLinkStatus, PATCH, 3911, 4361),
	REGISTER_OOVPAS(bind, PATCH, 3911, 4627),
	REGISTER_OOVPAS(connect, PATCH, 3911, 4627),
	REGISTER_OOVPAS(ioctlsocket, PATCH, 3911, 4627),
	REGISTER_OOVPAS(listen, PATCH, 3911, 4627),
	REGISTER_OOVPAS(recv, PATCH, 3911, 4627),
	REGISTER_OOVPAS(send, PATCH, 3911, 4627),
	REGISTER_OOVPAS(socket, PATCH, 3911, 4627),
};

// ******************************************************************
// * XNET_OOVPA_SIZE
// ******************************************************************
uint32 XNET_OOVPA_SIZEV2 = sizeof(XNET_OOVPAV2);

#endif
