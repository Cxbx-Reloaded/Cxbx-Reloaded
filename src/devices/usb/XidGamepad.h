// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
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
		int Init(int port);
		// destroy gamepad resources
		void XpadCleanUp();


	private:
		// usb device this gamepad is attached to through the hub
		USBDevice* m_UsbDev = nullptr;
		// gamepad state
		USBXIDState* m_XidState = nullptr;
		// gamepad class functions
		USBDeviceClass* m_pPeripheralFuncStruct = nullptr;
		// xbox port this gamepad is attached to
		int m_Port = 0;

		// initialize various member variables/functions
		XboxDeviceState* ClassInitFn();
		// reserve a usb port for this gamepad
		int UsbXidClaimPort(XboxDeviceState* dev, int port);
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
		// this should update the vibration strenght of the real controller this gamepad represents.
		// It doesn't do anything at the moment
		void UpdateForceFeedback();
};

extern XidGamepad* g_XidControllerObjArray[4];

#endif
