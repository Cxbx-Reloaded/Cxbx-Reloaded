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

#ifndef USBDEVICE_H_
#define USBDEVICE_H_

#include "..\PCIDevice.h"
#include "UsbCommon.h"
#include <vector>


// Forward declare OHCI class for m_HostController pointer
class OHCI;

/* Helper class which provides various functionality to both OHCI and usb device classes */
class USBDevice : public PCIDevice {
	public:
		// constructor
		USBDevice() {}
		// destructor
		~USBDevice() {}

		// PCI Device functions
		void Init();
		void Reset() {}

		uint32_t IORead(int barIndex, uint32_t port, unsigned size) { return 0; }
		void IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size) {}
		uint32_t MMIORead(int barIndex, uint32_t addr, unsigned size);
		void MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size);


		// USBDevice-specific functions/variables
		// pointer to the host controller this device refers to
		OHCI* m_HostController;
		// PCI path of this usb device
		const char* m_PciPath;
		// free usb ports on this device (hubs included)
		std::vector<USBPort*> m_FreePorts;

		// register a port with the HC
		void USB_RegisterPort(USBPort* Port, int Index, int SpeedMask, USBPortOps* Ops);
		// free a port with the HC
		void USB_UnregisterPort(USBPort* Port);
		// reset a usb port
		void USB_PortReset(USBPort* Port);
		// update device status during an attach
		void USB_DeviceAttach(XboxDeviceState* dev);
		// update device status during an detach
		void USB_DeviceDetach(XboxDeviceState* dev);
		// update port status when a device is attached
		void USB_Attach(USBPort* Port);
		// update port status when a device is detached
		void USB_Detach(USBPort* Port);
		// update port status when a device is detached
		void USB_Wakeup(USBEndpoint* ep);
		// reset a device
		void USB_DeviceReset(XboxDeviceState* Dev);
		// find the device connected to the supplied port and address
		XboxDeviceState* USB_FindDevice(USBPort* Port, uint8_t Addr);
		// find the requested endpoint in the supplied device
		USBEndpoint* USB_GetEP(XboxDeviceState* Dev, int Pid, int Ep);
		// setup a packet for transfer
		void USB_PacketSetup(USBPacket* p, int Pid, USBEndpoint* Ep, uint64_t Id,
			bool ShortNotOK, bool IntReq);
		// check if the state of the packet is queued or async
		bool USB_IsPacketInflight(USBPacket* p);
		// append the user buffer to the packet
		void USB_PacketAddBuffer(USBPacket* p, void* ptr, size_t len);
		// transfer and process the packet
		void USB_HandlePacket(XboxDeviceState* dev, USBPacket* p);
		// check if the packet has the expected state and assert if not
		void USB_PacketCheckState(USBPacket* p, USBPacketState expected);
		// process the packet
		void USB_ProcessOne(USBPacket* p);
		// process a setup token
		void USB_DoTokenSetup(XboxDeviceState* s, USBPacket* p);
		// process an input token
		void DoTokenIn(XboxDeviceState* s, USBPacket* p);
		// process an output token
		void DoTokenOut(XboxDeviceState* s, USBPacket* p);
		// copy the packet data to the buffer pointed to by ptr
		void USB_PacketCopy(USBPacket* p, void* ptr, size_t bytes);
		// Cancel an active packet.  The packed must have been deferred by
		// returning USB_RET_ASYNC from handle_packet, and not yet completed
		void USB_CancelPacket(USBPacket* p);
		// queue a packet to an endpoint
		void USB_QueueOne(USBPacket* p);
		// call usb class init function
		int USB_DeviceInit(XboxDeviceState* dev);
		// call usb class find_device function
		XboxDeviceState* USB_DeviceFindDevice(XboxDeviceState* Dev, uint8_t Addr);
		// call usb class cancel_packet function
		void USB_DeviceCancelPacket(XboxDeviceState* dev, USBPacket* p);
		// call usb class handle_destroy function
		void USB_DeviceHandleDestroy(XboxDeviceState* dev);
		// call usb class handle_attach function
		void USB_DeviceHandleAttach(XboxDeviceState* dev);
		// call usb class handle_reset function
		void USB_DeviceHandleReset(XboxDeviceState* dev);
		// call usb class handle_control function
		void USB_DeviceHandleControl(XboxDeviceState* dev, USBPacket* p, int request, int value, int index, int length, uint8_t* data);
		// call usb class handle_data function
		void USB_DeviceHandleData(XboxDeviceState* dev, USBPacket* p);
		// call usb class set_interface function
		void USB_DeviceSetInterface(XboxDeviceState* dev, int iface, int alt_old, int alt_new);
		// call usb class flush_ep_queue function
		void USB_DeviceFlushEPqueue(XboxDeviceState* dev, USBEndpoint* ep);
		// call usb class ep_stopped function
		void USB_DeviceEPstopped(XboxDeviceState* Dev, USBEndpoint* Ep);
		// set the type of the endpoint
		void USB_EPsetType(XboxDeviceState* dev, int pid, int ep, uint8_t type);
		// set the interface number of the endpoint
		void USB_EPsetIfnum(XboxDeviceState* dev, int pid, int ep, uint8_t ifnum);
		// set the maximum packet size parameter of the endpoint
		void USB_EPsetMaxPacketSize(XboxDeviceState* dev, int pid, int ep, uint16_t raw);
		// assign port numbers (also for hubs)
		void USB_PortLocation(USBPort* downstream, USBPort* upstream, int portnr);
		// initialize the endpoints of this peripheral
		void USB_EpInit(XboxDeviceState* dev);
		// reset all endpoints of this peripheral
		void USB_EpReset(XboxDeviceState* dev);
		// create a serial number for the device
		void USB_CreateSerial(XboxDeviceState* dev, std::string&& str);
		// start descriptors initialization
		void USBDesc_Init(XboxDeviceState* dev);
		// get device descriptor
		const USBDesc* USBDesc_GetUsbDeviceDesc(XboxDeviceState* dev);
		// set the descriptors to use for this device
		void USBDesc_SetDefaults(XboxDeviceState* dev);
		// set the configuration to use
		int USBDesc_SetConfig(XboxDeviceState* dev, int value);
		// set the interface to use
		int USBDesc_SetInterface(XboxDeviceState* dev, int index, int value);
		// find the interface to use
		const USBDescIface* USBDesc_FindInterface(XboxDeviceState* dev, int nif, int alt);
		// setup endpoints and their descriptors
		void USBDesc_EpInit(XboxDeviceState* dev);
		// handle standard control request
		int USBDesc_HandleControl(XboxDeviceState *dev, USBPacket *p,
			int request, int value, int index, int length, uint8_t *data);
		// handle standard GetDescriptor() request
		int USBDesc_HandleStandardGetDescriptor(XboxDeviceState* dev, USBPacket* p,
			int value, uint8_t* dest, size_t len);
		// return the binary rapresentation of a device descriptor
		int USB_ReadDeviceDesc(const USBDescID* id, const USBDescDevice* dev, uint8_t* dest, size_t len);
		// return the binary rapresentation of configuration descriptors
		int USB_ReadConfigurationDesc(const USBDescConfig* conf, int flags, uint8_t* dest, size_t len);
		// return the binary rapresentation of interface descriptors
		int USB_ReadInterfaceDesc(const USBDescIface* iface, int flags, uint8_t* dest, size_t len);
		// return the binary rapresentation of class-specific descriptors
		size_t USB_ReadOtherDesc(const USBDescOther* desc, uint8_t* dest, size_t len);
		// return the binary rapresentation of endpoint descriptors
		int USB_ReadEndpointDesc(const USBDescEndpoint* ep, int flags, uint8_t* dest, size_t len);
		// return the binary rapresentation of string descriptors
		int USB_ReadStringDesc(XboxDeviceState* dev, int index, uint8_t* dest, size_t len);
		// set a string in the string descriptor with the supplied index
		void USBDesc_SetString(XboxDeviceState* dev, int index, std::string&& str);
		// get a string in the string descriptor with the supplied index
		const char* USBDesc_GetString(XboxDeviceState* dev, int index);
};

#endif
