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

#ifndef HUB_H_
#define HUB_H_

#include "UsbDevice.h"


struct USBHubState; // forward declare


/* Class which implements a usb hub */
class Hub
{
	public:
		// usb device this hub is attached to
		USBDevice* m_UsbDev = nullptr;

		// initialize this hub
		int Init(int port);
		// start hub destruction
		void HubDestroy();
		
		
	private:
		// hub state
		USBHubState* m_HubState = nullptr;
		// hub class functions
		USBDeviceClass* m_pPeripheralFuncStruct = nullptr;

		// initialize various member variables/functions
		XboxDeviceState* ClassInitFn();
		// see USBDeviceClass for comments about these functions
		int UsbHub_Initfn(XboxDeviceState* dev);
		XboxDeviceState* UsbHub_FindDevice(XboxDeviceState* dev, uint8_t addr);
		void UsbHub_HandleReset();
		void UsbHub_HandleControl(XboxDeviceState* dev, USBPacket* p,
               int request, int value, int index, int length, uint8_t* data);
		void UsbHub_HandleData(XboxDeviceState* dev, USBPacket* p);
		void UsbHub_HandleDestroy();
		// see USBPortOps struct for info
		void UsbHub_Attach(USBPort* port1);
		void UsbHub_Detach(USBPort* port1);
		void UsbHub_ChildDetach(XboxDeviceState* child);
		void UsbHub_Wakeup(USBPort* port1);
		void UsbHub_Complete(USBPort* port, USBPacket* packet);
		// reserve a usb port for this hub
		int UsbHubClaimPort(XboxDeviceState* dev, int port);
		// free the usb port used by this hub
		void UsbHubReleasePort(XboxDeviceState* dev);
		// retieve the name of the feature of the usb request
		std::string GetFeatureName(int feature);
		// destroy hub resources
		void HubCleanUp();
};

extern Hub* g_HubObjArray[4];

#endif
