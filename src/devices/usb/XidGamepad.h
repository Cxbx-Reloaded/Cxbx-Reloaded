// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->devices->usb->XidGamepad.h
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
// *  (c) 2018 ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

#ifndef XIDGAMEPAD_H_
#define XIDGAMEPAD_H_

#include "Hub.h"


struct USBXIDState; // forward declare


/* Class which implements an xbox gamepad */
class XidGamepad
{
	public:
		// initialize this peripheral
		int Init(int pport);
		// destroy gamepad resources
		void XidCleanUp();


	private:
		// usb device this gamepad is attached to through the hub
		USBDevice* m_UsbDev = nullptr;
		// gamepad state
		USBXIDState* m_XidState = nullptr;
		// gamepad class functions
		USBDeviceClass* m_pPeripheralFuncStruct = nullptr;

		// initialize various member variables/functions
		XboxDeviceState* ClassInitFn();
		// reserve a usb port for this gamepad
		int UsbXidClaimPort(XboxDeviceState* dev, int pport);
		// free the usb port used by this gamepad
		void UsbXidReleasePort(XboxDeviceState* dev);
		// see USBDeviceClass for comments about these functions
		int UsbXid_Initfn(XboxDeviceState* dev);
		void UsbXid_HandleDestroy();
		void UsbXid_Attach(XboxDeviceState* dev);
		void UsbXid_HandleReset();
		void UsbXid_HandleControl(XboxDeviceState* dev, USBPacket* p,
			int request, int value, int index, int length, uint8_t* data);
		void UsbXid_HandleData(XboxDeviceState* dev, USBPacket* p);
};

#endif