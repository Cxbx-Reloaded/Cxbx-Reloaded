// ******************************************************************
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
// *  (c) 2019 Luke Usher <luke.usher@outlook.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef JVS_H
#define JVS_H

// Used by CxbxKrnl to setup JVS roms
void JVS_Init();

#include "core\hle\XAPI\Xapi.h" // For EMUPATCH

namespace xbox {

DWORD WINAPI EMUPATCH(JVS_SendCommand)
	(
		DWORD a1,
		DWORD Command,
		DWORD a3,
		DWORD Length,
		DWORD a5,
		DWORD a6,
		DWORD a7,
		DWORD a8
		);

DWORD WINAPI EMUPATCH(JvsBACKUP_Read)
	(
		DWORD Offset,
		DWORD Length,
		PUCHAR Buffer,
		DWORD a4
		);

DWORD WINAPI EMUPATCH(JvsBACKUP_Write)
	(
		DWORD Offset,
		DWORD Length,
		PUCHAR Buffer,
		DWORD a4
		);

DWORD WINAPI EMUPATCH(JvsEEPROM_Read)
	(
		DWORD Offset,
		DWORD Length,
		PUCHAR Buffer,
		DWORD a4
		);

DWORD WINAPI EMUPATCH(JvsEEPROM_Write)
	(
		DWORD Offset,
		DWORD Length,
		PUCHAR Buffer,
		DWORD a4
		);

DWORD WINAPI EMUPATCH(JvsFirmwareDownload)
	(
		DWORD Offset,
		DWORD Length,
		PUCHAR Buffer,
		DWORD a4
		);

DWORD WINAPI EMUPATCH(JvsFirmwareUpload)
	(
		DWORD Offset,
		DWORD Length,
		PUCHAR Buffer,
		DWORD a4
		);

DWORD WINAPI EMUPATCH(JvsNodeReceivePacket)
	(
		PUCHAR Buffer,
		PDWORD Length,
		DWORD a3
		);

DWORD WINAPI EMUPATCH(JvsNodeSendPacket)
	(
		PUCHAR Buffer,
		DWORD Length,
		DWORD a3
		);

typedef struct {
	UCHAR second;
	UCHAR minute;
	UCHAR hour;
	UCHAR unused_2;

	UCHAR day;
	UCHAR month;
	UCHAR year;
	UCHAR unused_1;
} JvsRTCTime;

DWORD WINAPI EMUPATCH(JvsRTC_Read)
	(
		DWORD a1,
		DWORD a2,
		JvsRTCTime *time,
		DWORD a4
		);

DWORD WINAPI EMUPATCH(JvsRTC_Write)
	(
		DWORD a1,
		DWORD a2,
		JvsRTCTime *time,
		DWORD a4
		);

DWORD WINAPI EMUPATCH(JvsScFirmwareDownload)
	(
		DWORD Offset,
		DWORD Length,
		PUCHAR Buffer,
		DWORD a4
		);

DWORD WINAPI EMUPATCH(JvsScFirmwareUpload)
	(
		DWORD Offset,
		DWORD Length,
		PUCHAR Buffer,
		DWORD a4
		);

DWORD WINAPI EMUPATCH(JvsScReceiveMidi)
	(
		DWORD a1,
		DWORD a2,
		DWORD a3
		);

DWORD WINAPI EMUPATCH(JvsScSendMidi)
	(
		DWORD a1,
		DWORD a2,
		DWORD a3
		);

DWORD WINAPI EMUPATCH(JvsScReceiveRs323c)
	(
		PUCHAR Buffer,
		DWORD Length,
		DWORD a3
		);


DWORD WINAPI EMUPATCH(JvsScSendRs323c)
	(
		PUCHAR Buffer,
		DWORD Length,
		DWORD a3
		);
}

#endif
