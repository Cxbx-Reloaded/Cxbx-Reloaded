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

#define USB_DIR_OUT         0
#define USB_DIR_IN          0x80

#define SETUP_STATE_IDLE    0
#define SETUP_STATE_SETUP   1
#define SETUP_STATE_DATA    2
#define SETUP_STATE_ACK     3
#define SETUP_STATE_PARAM   4


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
	m_HostController->OHCI_Detach(Port);
	dev->State = USB_STATE_NOTATTACHED;
}

void USBDevice::USB_Attach(USBPort* Port)
{
	XboxDevice *dev = Port->Dev;

	assert(dev != nullptr);
	assert(dev->Attached);
	assert(dev->State == USB_STATE_NOTATTACHED);
	m_HostController->OHCI_Attach(Port);
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
			USB_QueueOne(p);
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
		USB_QueueOne(p);
	}
}

void USBDevice::USB_QueueOne(USBPacket* p)
{
	p->State = USB_PACKET_QUEUED;
	QTAILQ_INSERT_TAIL(&p->Endpoint->Queue, p, Queue);
	p->Status = USB_RET_ASYNC;
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
			USB_DoParameter(dev, p);
			return;
		}
		switch (p->Pid) {
			case USB_TOKEN_SETUP:
				USB_DoTokenSetup(dev, p);
				break;
			case USB_TOKEN_IN:
				DoTokenIn(dev, p);
				break;
			case USB_TOKEN_OUT:
				DoTokenOut(dev, p);
				break;
			default:
				p->Status = USB_RET_STALL;
		}
	}
	else {
		// data pipe
		USB_DeviceHandleData(dev, p);
	}
}

void USBDevice::USB_DoParameter(XboxDevice* s, USBPacket* p)
{
	int i, request, value, index;

	for (i = 0; i < 8; i++) {
		s->SetupBuffer[i] = p->Parameter >> (i * 8);
	}

	s->SetupState = SETUP_STATE_PARAM;
	s->SetupLength = (s->SetupBuffer[7] << 8) | s->SetupBuffer[6];
	s->SetupIndex = 0;

	request = (s->SetupBuffer[0] << 8) | s->SetupBuffer[1];
	value = (s->SetupBuffer[3] << 8) | s->SetupBuffer[2];
	index = (s->SetupBuffer[5] << 8) | s->SetupBuffer[4];

	if (s->SetupLength > sizeof(s->data_buf)) {
		DbgPrintf("Usb: ctrl buffer too small (%d > %zu)\n", s->SetupLength, sizeof(s->data_buf));
		p->Status = USB_RET_STALL;
		return;
	}

	if (p->Pid == USB_TOKEN_OUT) {
		USB_PacketCopy(p, s->data_buf, s->SetupLength);
	}

	USB_DeviceHandleControl(s, p, request, value, index, s->SetupLength, s->data_buf);
	if (p->Status == USB_RET_ASYNC) {
		return;
	}

	if (p->ActualLength < s->SetupLength) {
		s->SetupLength = p->ActualLength;
	}
	if (p->Pid == USB_TOKEN_IN) {
		p->ActualLength = 0;
		USB_PacketCopy(p, s->data_buf, s->SetupLength);
	}
}

void USBDevice::USB_DoTokenSetup(XboxDevice* s, USBPacket* p)
{
	int request, value, index;

	// From the standard "Every Setup packet has eight bytes."
	if (p->IoVec.Size != 8) {
		p->Status = USB_RET_STALL;
		return;
	}

	// Info: name, offset, size, info (sizes are in bytes)
	// bmRequestType, 1, 1, determines the direction of the request, type of request and designated recipient
	// bRequest, 1, 1, determines the request being made
	// wValue, 2, 2, it is used to pass a parameter to the device, specific to the request
	// wIndex, 4, 2, often used in requests to specify an endpoint or an interface
	// wLength, 6, 2, number of bytes to transfer if there is a data phase
	// The wValue and wIndex fields allow parameters to be passed with the request

	USB_PacketCopy(p, s->SetupBuffer, p->IoVec.Size);
	p->ActualLength = 0;
	s->SetupLength = (s->SetupBuffer[7] << 8) | s->SetupBuffer[6];
	s->SetupIndex = 0;

	request = (s->SetupBuffer[0] << 8) | s->SetupBuffer[1];
	value = (s->SetupBuffer[3] << 8) | s->SetupBuffer[2];
	index = (s->SetupBuffer[5] << 8) | s->SetupBuffer[4];

	if (s->SetupBuffer[0] & USB_DIR_IN) {
		USB_DeviceHandleControl(s, p, request, value, index, s->SetupLength, s->data_buf);
		if (p->Status == USB_RET_ASYNC) {
			s->SetupState = SETUP_STATE_SETUP;
		}
		if (p->Status != USB_RET_SUCCESS) {
			return;
		}

		if (p->ActualLength < s->SetupLength) {
			s->SetupLength = p->ActualLength;
		}
		s->SetupState = SETUP_STATE_DATA;
	}
	else {
		if (s->SetupLength > sizeof(s->data_buf)) {
			DbgPrintf("Usb: ctrl buffer too small (%d > %zu)\n", s->SetupLength, sizeof(s->data_buf));
			p->Status = USB_RET_STALL;
			return;
		}
		if (s->SetupLength == 0) {
			s->SetupState = SETUP_STATE_ACK;
		}
		else {
			s->SetupState = SETUP_STATE_DATA;
		}
	}

	p->ActualLength = 8;
}

void USBDevice::DoTokenIn(XboxDevice* s, USBPacket* p)
{
	int request, value, index;

	assert(p->ep->nr == 0);

	request = (s->SetupBuffer[0] << 8) | s->SetupBuffer[1];
	value = (s->SetupBuffer[3] << 8) | s->SetupBuffer[2];
	index = (s->SetupBuffer[5] << 8) | s->SetupBuffer[4];

	switch (s->SetupState) {
		case SETUP_STATE_ACK:
			if (!(s->SetupBuffer[0] & USB_DIR_IN)) {
				USB_DeviceHandleControl(s, p, request, value, index, s->SetupLength, s->data_buf);
				if (p->Status == USB_RET_ASYNC) {
					return;
				}
				s->SetupState = SETUP_STATE_IDLE;
				p->ActualLength = 0;
			}
			break;

		case SETUP_STATE_DATA:
			if (s->SetupBuffer[0] & USB_DIR_IN) {
				int len = s->SetupLength - s->SetupIndex;
				if (len > p->IoVec.Size) {
					len = p->IoVec.Size;
				}
				USB_PacketCopy(p, s->data_buf + s->SetupIndex, len);
				s->SetupIndex += len;
				if (s->SetupIndex >= s->SetupLength) {
					s->SetupState = SETUP_STATE_ACK;
				}
				return;
			}
			s->SetupState = SETUP_STATE_IDLE;
			p->Status = USB_RET_STALL;
			break;

		default:
			p->Status = USB_RET_STALL;
	}
}

void USBDevice::DoTokenOut(XboxDevice* s, USBPacket* p)
{
	assert(p->ep->nr == 0);

	switch (s->SetupState) {
		case SETUP_STATE_ACK:
			if (s->SetupBuffer[0] & USB_DIR_IN) {
				s->SetupState = SETUP_STATE_IDLE;
				/* transfer OK */
			}
			else {
				/* ignore additional output */
			}
			break;

		case SETUP_STATE_DATA:
			if (!(s->SetupBuffer[0] & USB_DIR_IN)) {
				int len = s->SetupLength - s->SetupIndex;
				if (len > p->IoVec.Size) {
					len = p->IoVec.Size;
				}
				USB_PacketCopy(p, s->data_buf + s->SetupIndex, len);
				s->SetupIndex += len;
				if (s->SetupIndex >= s->SetupLength) {
					s->SetupState = SETUP_STATE_ACK;
				}
				return;
			}
			s->SetupState = SETUP_STATE_IDLE;
			p->Status = USB_RET_STALL;
			break;

		default:
			p->Status = USB_RET_STALL;
	}
}

void USBDevice::USB_PacketCopy(USBPacket* p, void* ptr, size_t bytes)
{
	IOVector* iov = p->Combined ? &p->Combined->IoVec : &p->IoVec;

	assert(p->ActualLength >= 0);
	assert(p->ActualLength + bytes <= iov->Size);
	switch (p->Pid) {
		case USB_TOKEN_SETUP:
		case USB_TOKEN_OUT:
			IoVecTobuffer(iov->IoVecStruct, iov->IoVecNumber, p->ActualLength, ptr, bytes);
			break;
		case USB_TOKEN_IN:
			IoVecFromBuffer(iov->IoVecStruct, iov->IoVecNumber, p->ActualLength, ptr, bytes);
			break;
		default:
			CxbxKrnlCleanup("Usb: %s has an invalid pid: %x\n", __func__, p->Pid);
	}
	p->ActualLength += bytes;
}

void USBDevice::USB_DeviceHandleControl(XboxDevice* dev, USBPacket* p, int request, int value, int index, int length, uint8_t* data)
{
	USBDeviceClass *klass = USB_DEVICE_GET_CLASS(dev);
	if (klass->handle_control) {
		klass->handle_control(dev, p, request, value, index, length, data); // TODO: usb_hub_handle_control
	}
}

void USBDevice::USB_DeviceHandleData(XboxDevice* dev, USBPacket* p)
{
	USBDeviceClass *klass = USB_DEVICE_GET_CLASS(dev);
	if (klass->handle_data) {
		klass->handle_data(dev, p); // TODO: usb_hub_handle_data
	}
}

void USBDevice::USB_DeviceFlushEPqueue(XboxDevice* dev, USBEndpoint* ep)
{
	USBDeviceClass *klass = USB_DEVICE_GET_CLASS(dev);
	if (klass->flush_ep_queue) {
		klass->flush_ep_queue(dev, ep); // TODO: it's nullptr in XQEMU...
	}
}

void USBDevice::USB_DeviceCancelPacket(XboxDevice* dev, USBPacket* p)
{
	USBDeviceClass *klass = USB_DEVICE_GET_CLASS(dev);
	if (klass->cancel_packet) {
		klass->cancel_packet(dev, p); // TODO: it's nullptr in XQEMU...
	}
}

void USBDevice::USB_CancelPacket(USBPacket* p)
{
	bool callback = (p->State == USB_PACKET_ASYNC);
	assert(USB_IsPacketInflight(p));
	p->State = USB_PACKET_CANCELED;
	QTAILQ_REMOVE(&p->Endpoint->Queue, p, Queue);
	if (callback) {
		USB_DeviceCancelPacket(p->Endpoint->Dev, p);
	}
}
