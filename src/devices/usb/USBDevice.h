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
// *   Cxbx->devices->usb->USBDevice.h
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

#ifndef USBDEVICE_H_
#define USBDEVICE_H_

#include "..\PCIDevice.h"
#include "UsbCommon.h"


// Forward declare OHCI class for USBDevice class
class OHCI;

class USBDevice : public PCIDevice {
	public:
		// constructor
		USBDevice() {}
		// destructor
		~USBDevice() {}
	
		// PCI Device functions
		void Init(unsigned int address);
		void Reset() {}
	
		uint32_t IORead(int barIndex, uint32_t port, unsigned size) {}
		void IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size) {}
		uint32_t MMIORead(int barIndex, uint32_t addr, unsigned size);
		void MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size);


		// USBDevice-specific functions/variables
		// pointer to the host controller this device refers to
		OHCI* m_HostController = nullptr;

		// register a port with the HC
		void USB_RegisterPort(USBPort* Port, int Index, int SpeedMask);
		//
		void USB_DeviceEPstopped(XboxDevice* Dev, USBEndpoint* Ep);
		// reset a usb port
		void USB_PortReset(USBPort* Port);
		// a device is attched
		void USB_Attach(USBPort* Port);
		// a device is detached
		void USB_Detach(USBPort* Port);
		// a device downstream from the device attached to the port (attached through a hub) is detached
		void ChildDetach(USBPort* Port, XboxDevice* Child);
		// TODO
		void Wakeup(USBPort* Port);
		// TODO
		void Complete(USBPort* Port, USBPacket *P);
		// reset a device
		void USB_DeviceReset(XboxDevice* Dev);
		// find the usb device with the supplied address
		XboxDevice* USB_FindDevice(USBPort* Port, uint8_t Addr);
		// ergo720: can probably be removed by calling directly usb_hub_find_device
		XboxDevice* USB_DeviceFindDevice(XboxDevice* Dev, uint8_t Addr);
		// find the requested endpoint in the supplied device
		USBEndpoint* USB_GetEP(XboxDevice* Dev, int Pid, int Ep);
		// setup a packet for transfer
		void USB_PacketSetup(USBPacket* p, int Pid, USBEndpoint* Ep, unsigned int Stream,
			uint64_t Id, bool ShortNotOK, bool IntReq);
		// check if the state of the packet is queued or async
		bool USB_IsPacketInflight(USBPacket* p);
		// append the user buffer to the packet
		void USB_PacketAddBuffer(USBPacket* p, void* ptr, size_t len);
		// transfer and process the packet
		void USB_HandlePacket(XboxDevice* dev, USBPacket* p);
		// check if the packet has the expected state and assert if not
		void USB_PacketCheckState(USBPacket* p, USBPacketState expected);
		// process the packet
		void USB_ProcessOne(USBPacket* p);
		//
		void USB_DoParameter(XboxDevice* s, USBPacket* p);
		// process a setup token
		void USB_DoTokenSetup(XboxDevice* s, USBPacket* p);
		// process an input token
		void DoTokenIn(XboxDevice* s, USBPacket* p);
		// process an output token
		void DoTokenOut(XboxDevice* s, USBPacket* p);
		// copy the packet data to the buffer pointed to by ptr
		void USB_PacketCopy(USBPacket* p, void* ptr, size_t bytes);
		// queue a packet to an endpoint
		void USB_QueueOne(USBPacket* p);
		//
		void USB_DeviceHandleControl(XboxDevice* dev, USBPacket* p, int request, int value, int index, int length, uint8_t* data);
		//
		void USB_DeviceHandleData(XboxDevice* dev, USBPacket* p);
		//
		void USB_DeviceFlushEPqueue(XboxDevice* dev, USBEndpoint* ep);
		//
		void USB_DeviceCancelPacket(XboxDevice* dev, USBPacket* p);
		// Cancel an active packet.  The packed must have been deferred by
		// returning USB_RET_ASYNC from handle_packet, and not yet completed
		void USB_CancelPacket(USBPacket* p);
};

#endif
