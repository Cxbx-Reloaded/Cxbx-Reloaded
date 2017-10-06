// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;	 .,;;  ;;;'';;'   `;;;	 .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o	   oP"``"Yo	 _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDatabase->XOnline.OOVPA.inl
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

#ifndef XONLINES_OOVPA_INL
#define XONLINES_OOVPA_INL

#include "../OOVPA.h"

#include "HLEDataBase/XOnline.1.0.4361.inl"
#include "HLEDataBase/XOnline.1.0.4627.inl"
#include "HLEDataBase/XOnline.1.0.5028.inl"
#include "HLEDataBase/XOnline.1.0.5233.inl"
#include "HLEDataBase/XOnline.1.0.5344.inl"
#include "HLEDataBase/XOnline.1.0.5558.inl"
#include "HLEDataBase/XOnline.1.0.5788.inl"
#include "HLEDataBase/XOnline.1.0.5849.inl"

// ******************************************************************
// * XONLINES_OOVPA
// ******************************************************************
OOVPATable XONLINES_OOVPAV2[] = {

	REGISTER_OOVPAS(XnInit, XREF, 4361, 4627, 5788),
	REGISTER_OOVPAS(WSAStartup, PATCH, 4361, 5558),
	REGISTER_OOVPAS(XNetStartup, PATCH, 4361, 5233),
	REGISTER_OOVPAS(XNetGetEthernetLinkStatus, PATCH, 4361, 4627),
	REGISTER_OOVPAS(bind, PATCH, 3911, 4627),
	REGISTER_OOVPAS(connect, PATCH, 3911, 5233),
	REGISTER_OOVPAS(ioctlsocket, PATCH, 3911, 4627),
	REGISTER_OOVPAS(listen, PATCH, 3911, 4627),
	REGISTER_OOVPAS(recv, PATCH, 3911),
	REGISTER_OOVPAS(send, PATCH, 3911),
	REGISTER_OOVPAS(socket, PATCH, 3911, 4627, 5558),
	REGISTER_OOVPAS(CXo_XOnlineLogon, XREF, 4361, 4627, 5028, 5233, 5558, 5849),
	REGISTER_OOVPAS(XOnlineLogon, PATCH, 4361),
	REGISTER_OOVPAS(XoUpdateLaunchNewImageInternal, XREF, 4627, 5788),
};

// ******************************************************************
// * XONLINES_OOVPA_SIZE
// ******************************************************************
uint32 XONLINES_OOVPA_SIZEV2 = sizeof(XONLINES_OOVPAV2);

#endif
