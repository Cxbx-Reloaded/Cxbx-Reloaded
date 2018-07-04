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
// *   Cxbx->devices->usb->Hub.h
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

#include "UsbCommon.h"


/* same as Linux kernel root hubs */
typedef enum {
    STR_MANUFACTURER = 1,
    STR_PRODUCT,
    STR_SERIALNUMBER,
};


/* Class which implements a usb hub */
class Hub
{
	public:
		// initialize this peripheral
		int Init(int pport);
		// destructor
		~Hub();
		
		
	private:
		// usb device this hub is attached to
		USBDevice* m_UsbDev = nullptr;
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
		void UsbHub_HandleDestroy(XboxDeviceState* dev);
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
};

extern Hub* g_HubObjArray[4];

#endif
