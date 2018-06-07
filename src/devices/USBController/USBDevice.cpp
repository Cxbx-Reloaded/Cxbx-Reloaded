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
// *   Cxbx->devices->USBController->USBDevice.cpp
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

#include "USBDevice.h"
#include "OHCI.h"
#include "CxbxKrnl\EmuKrnl.h"  // For EmuWarning

#define USB_ENDPOINT_XFER_CONTROL   0
#define USB_ENDPOINT_XFER_ISOC      1
#define USB_ENDPOINT_XFER_BULK      2
#define USB_ENDPOINT_XFER_INT       3
#define USB_ENDPOINT_XFER_INVALID   255


void USBDevice::Init(unsigned int address)
{
	PCIBarRegister r;

	// Register Memory bar :
	r.Raw.type = PCI_BAR_TYPE_MEMORY;
	r.Memory.address = address >> 4;
	RegisterBAR(0, USB_SIZE, r.value);

	// Taken from https://github.com/docbrown/vxb/wiki/Xbox-Hardware-Information
	m_DeviceId = 0x01C2;
	m_VendorId = PCI_VENDOR_ID_NVIDIA;

	if (address == USB0_BASE) {
		m_HostController = new OHCI(1, this);
		return;
	}

	m_HostController = new OHCI(9, this);
}

uint32_t USBDevice::MMIORead(int barIndex, uint32_t addr, unsigned size)
{
	// barIndex must be zero since we created the USB devices with a zero index in Init()
	assert(barIndex == 0);

	// read the register of the corresponding HC
	return m_HostController->OHCI_ReadRegister(addr);
}

void USBDevice::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size)
{
	// barIndex must be zero since we created the USB devices with a zero index in Init()
	assert(barIndex == 0);

	// write the register of the corresponding HC
	m_HostController->OHCI_WriteRegister(addr, value);
}

void USBDevice::USB_RegisterPort(USBPort* Port, int Index, int SpeedMask)
{
	Port->PortIndex = Index;
	Port->SpeedMask = SpeedMask;
	Port->HubCount = 0;
	std::snprintf(Port->Path, sizeof(Port->Path), "%d", Index + 1);
}

void USBDevice::USB_DeviceEPstopped(XboxDevice* Dev, USBEndpoint* EP)
{
	// This seems to be a nop in XQEMU since it doesn't assign the EP_Stopped function (it's nullptr)
	USBDeviceClass* klass = USB_DEVICE_GET_CLASS(Dev);
	if (klass->EP_Stopped) {
		klass->EP_Stopped(Dev, EP);
	}
}

void USBDevice::USB_PortReset(USBPort* Port)
{
	XboxDevice* dev = Port->Dev;

	assert(dev != nullptr);
	USB_Detach(Port);
	USB_Attach(Port);
	USB_DeviceReset(dev);
}

void USBDevice::USB_Detach(USBPort* Port)
{
	XboxDevice* dev = Port->Dev;

	assert(dev != nullptr);
	assert(dev->State != USB_STATE_NOTATTACHED);
	OHCI_Detach(Port);
	dev->State = USB_STATE_NOTATTACHED;
}

void USBDevice::USB_Attach(USBPort* Port)
{
	XboxDevice *dev = Port->Dev;

	assert(dev != nullptr);
	assert(dev->Attached);
	assert(dev->State == USB_STATE_NOTATTACHED);
	OHCI_Attach(Port);
	dev->State = USB_STATE_ATTACHED;
	usb_device_handle_attach(dev);
}

void USBDevice::USB_DeviceReset(XboxDevice* dev)
{
	if (dev == nullptr || !dev->Attached) {
		return;
	}

	dev->RemoteWakeup = 0;
	dev->Addr = 0;
	dev->State = USB_STATE_DEFAULT;
	usb_device_handle_reset(dev);
}

XboxDevice* USBDevice::USB_FindDevice(USBPort* Port, uint8_t Addr)
{
	XboxDevice* dev = Port->Dev;

	if (dev == nullptr || !dev->Attached || dev->State != USB_STATE_DEFAULT) {
		return nullptr;
	}
	if (dev->Addr == Addr) {
		return dev;
	}

	return USB_DeviceFindDevice(dev, Addr);
}

XboxDevice* USBDevice::USB_DeviceFindDevice(XboxDevice* Dev, uint8_t Addr)
{
	USBDeviceClass *klass = USB_DEVICE_GET_CLASS(Dev);
	if (klass->find_device) {
		return klass->find_device(Dev, Addr); // TODO: usb_hub_find_device
	}

	return nullptr;
}

USBEndpoint* USBDevice::USB_GetEP(XboxDevice* Dev, int Pid, int Ep)
{
	USBEndpoint* eps;

	if (Dev == nullptr) {
		return nullptr;
	}
	eps = (Pid == USB_TOKEN_IN) ? Dev->EP_in : Dev->EP_out;
	if (Ep == 0) {
		return &Dev->EP_ctl; // EndpointNumber zero represents the default control endpoint
	}
	assert(pid == USB_TOKEN_IN || pid == USB_TOKEN_OUT);
	assert(ep > 0 && ep <= USB_MAX_ENDPOINTS);

	return eps + Ep - 1;
}

void USBDevice::USB_PacketSetup(USBPacket* p, int Pid, USBEndpoint* Ep, unsigned int Stream,
	uint64_t Id, bool ShortNotOK, bool IntReq)
{
	assert(!USB_IsPacketInflight(p));
	assert(p->IoVec.IoVecStruct != nullptr);
	p->Id = Id;
	p->Pid = Pid;
	p->Endpoint = Ep;
	p->Stream = Stream;
	p->Status = USB_RET_SUCCESS;
	p->ActualLength = 0;
	p->Parameter = 0;
	p->ShortNotOK = ShortNotOK;
	p->IntReq = IntReq;
	p->Combined = nullptr;
	IoVecReset(&p->IoVec);
	p->State = USB_PACKET_SETUP;
}

bool USBDevice::USB_IsPacketInflight(USBPacket* p)
{
	return (p->State == USB_PACKET_QUEUED || p->State == USB_PACKET_ASYNC);
}

void USBDevice::USB_PacketAddBuffer(USBPacket* p, void* ptr, size_t len)
{
	IoVecAdd(&p->IoVec, ptr, len);
}

void USBDevice::USB_HandlePacket(XboxDevice* dev, USBPacket* p)
{
	if (dev == nullptr) {
		p->Status = USB_RET_NODEV;
		return;
	}
	assert(dev == p->Endpoint->Dev);
	assert(dev->State == USB_STATE_DEFAULT);
	USB_PacketCheckState(p, USB_PACKET_SETUP);
	assert(p->Endpoint != nullptr);

	// Submitting a new packet clears halt
	if (p->Endpoint->Halted) {
		assert(QTAILQ_EMPTY(&p->Endpoint->Queue));
		p->Endpoint->Halted = false;
	}

	if (QTAILQ_EMPTY(&p->Endpoint->Queue) || p->Endpoint->Pipeline || p->Stream) {
		USB_ProcessOne(p);
		if (p->Status == USB_RET_ASYNC) {
			// hcd drivers cannot handle async for isoc
			assert(p->Endpoint->Type != USB_ENDPOINT_XFER_ISOC);
			// using async for interrupt packets breaks migration
			assert(p->Endpoint->Type != USB_ENDPOINT_XFER_INT ||
				(dev->flags & (1 << USB_DEV_FLAG_IS_HOST)));
			p->State = USB_PACKET_ASYNC;
			QTAILQ_INSERT_TAIL(&p->Endpoint->Queue, p, Queue);
		}
		else if (p->Status == USB_RET_ADD_TO_QUEUE) {
			usb_queue_one(p);
		}
		else {
			// When pipelining is enabled usb-devices must always return async,
			// otherwise packets can complete out of order!
			assert(p->stream || !p->Endpoint->pipeline ||
				QTAILQ_EMPTY(&p->Endpoint->Queue));
			if (p->Status != USB_RET_NAK) {
				p->State = USB_PACKET_COMPLETE;
			}
		}
	}
	else {
		usb_queue_one(p);
	}
}

void USBDevice::USB_PacketCheckState(USBPacket* p, USBPacketState expected)
{
	if (p->State == expected) {
		return;
	}

	EmuWarning("Usb: packet state check failed!");
	assert(0);
}

void USBDevice::USB_ProcessOne(USBPacket* p)
{
	XboxDevice* dev = p->Endpoint->Dev;

	// Handlers expect status to be initialized to USB_RET_SUCCESS, but it
	// can be USB_RET_NAK here from a previous usb_process_one() call,
	// or USB_RET_ASYNC from going through usb_queue_one().
	p->Status = USB_RET_SUCCESS;

	if (p->Endpoint->Num == 0) {
		// Info: All devices must support endpoint zero. This is the endpoint which receives all of the devices control 
		// and status requests during enumeration and throughout the duration while the device is operational on the bus
		if (p->Parameter) {
			do_parameter(dev, p);
			return;
		}
		switch (p->Pid) {
			case USB_TOKEN_SETUP:
				do_token_setup(dev, p);
				break;
			case USB_TOKEN_IN:
				do_token_in(dev, p);
				break;
			case USB_TOKEN_OUT:
				do_token_out(dev, p);
				break;
			default:
				p->Status = USB_RET_STALL;
		}
	}
	else {
		// data pipe
		usb_device_handle_data(dev, p);
	}
}
