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

#include "UsbCommon.h"


/* same as Linux kernel root hubs */
typedef enum ft{
    STR_MANUFACTURER = 1,
    STR_PRODUCT,
    STR_SERIALNUMBER,
};


/* Class which implements a usb hub */
class Hub final : public UsbPeripheral
{
	public:
		// initialize this peripheral
		void Init() override;
		
		
	private:
		// initialize various member variables/functions
		void ClassInitFn();
		// see USBDeviceClass for comments about these functions
		int UsbHub_Initfn(XboxDevice* dev);
		XboxDevice* UsbHub_FindDevice(XboxDevice* dev, uint8_t addr);
		void UsbHub_HandleReset(XboxDevice* dev);
		void UsbHub_HandleControl(XboxDevice* dev, USBPacket* p,
               int request, int value, int index, int length, uint8_t* data);
		void UsbHub_HandleData(XboxDevice* dev, USBPacket* p);
		void UsbHub_HandleDestroy(XboxDevice* dev);
		// initialize the endpoints of this peripheral
		void UsbEpInit();
		//
		void UsbEpReset();
};