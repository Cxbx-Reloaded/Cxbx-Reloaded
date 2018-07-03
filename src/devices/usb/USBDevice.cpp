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
// *   Cxbx->devices->usb->USBDevice.cpp
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
		m_PciPath = "pci.0:02.0";
		return;
	}

	m_HostController = new OHCI(9, this);
	m_PciPath = "pci.0:03.0";
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

void USBDevice::USB_RegisterPort(USBPort* Port, int Index, int SpeedMask, USBPortOps* Ops)
{
	Port->PortIndex = Index;
	Port->SpeedMask = SpeedMask;
	Port->Operations = Ops;
	USB_PortLocation(Port, nullptr, Index + 1);
}

void USBDevice::USB_PortReset(USBPort* Port)
{
	XboxDeviceState* dev = Port->Dev;

	assert(dev != nullptr);
	USB_Detach(Port);
	USB_Attach(Port);
	USB_DeviceReset(dev);
}

void USBDevice::USB_Attach(USBPort* Port)
{
	XboxDeviceState* dev = Port->Dev;

	assert(dev != nullptr);
	assert(dev->Attached);
	assert(dev->State == USB_STATE_NOTATTACHED);
	Port->Operations->attach(Port);
	dev->State = USB_STATE_ATTACHED;
	USB_DeviceHandleAttach(dev);
}

void USBDevice::USB_Detach(USBPort* Port)
{
	XboxDeviceState* dev = Port->Dev;

	assert(dev != nullptr);
	assert(dev->State != USB_STATE_NOTATTACHED);
	m_HostController->OHCI_Detach(Port);
	dev->State = USB_STATE_NOTATTACHED;
}

void USBDevice::USB_Wakeup(USBEndpoint* ep)
{
	XboxDeviceState* dev = ep->Dev;

	if (dev->RemoteWakeup && dev->Port && dev->Port->Operations->wakeup) {
		dev->Port->Operations->wakeup(dev->Port);
	}
}

void USBDevice::USB_DeviceReset(XboxDeviceState* dev)
{
	if (dev == nullptr || !dev->Attached) {
		return;
	}

	dev->RemoteWakeup = 0;
	dev->Addr = 0;
	dev->State = USB_STATE_DEFAULT;
	usb_device_handle_reset(dev);
}

XboxDeviceState* USBDevice::USB_FindDevice(USBPort* Port, uint8_t Addr)
{
	XboxDeviceState* dev = Port->Dev;

	if (dev == nullptr || !dev->Attached || dev->State != USB_STATE_DEFAULT) {
		return nullptr;
	}
	if (dev->Addr == Addr) {
		return dev;
	}

	return USB_DeviceFindDevice(dev, Addr);
}

USBEndpoint* USBDevice::USB_GetEP(XboxDeviceState* Dev, int Pid, int Ep)
{
	USBEndpoint* eps;

	if (Dev == nullptr) {
		return nullptr;
	}
	eps = (Pid == USB_TOKEN_IN) ? Dev->EP_in : Dev->EP_out;
	if (Ep == 0) {
		return &Dev->EP_ctl; // EndpointNumber zero represents the default control endpoint
	}
	assert(Pid == USB_TOKEN_IN || Pid == USB_TOKEN_OUT);
	assert(Ep > 0 && Ep <= USB_MAX_ENDPOINTS);

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

void USBDevice::USB_HandlePacket(XboxDeviceState* dev, USBPacket* p)
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
	XboxDeviceState* dev = p->Endpoint->Dev;

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

void USBDevice::USB_DoParameter(XboxDeviceState* s, USBPacket* p)
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

void USBDevice::USB_DoTokenSetup(XboxDeviceState* s, USBPacket* p)
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

void USBDevice::DoTokenIn(XboxDeviceState* s, USBPacket* p)
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

void USBDevice::DoTokenOut(XboxDeviceState* s, USBPacket* p)
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

int USBDevice::USB_DeviceInit(XboxDeviceState* dev)
{
	USBDeviceClass* klass = dev->klass;
	if (klass->init) {
		return klass->init(dev);
	}

	return 0;
}

XboxDeviceState* USBDevice::USB_DeviceFindDevice(XboxDeviceState* dev, uint8_t Addr)
{
	USBDeviceClass* klass = dev->klass;
	if (klass->find_device) {
		return klass->find_device(dev, Addr);
	}

	return nullptr;
}

void USBDevice::USB_DeviceCancelPacket(XboxDeviceState* dev, USBPacket* p)
{
	USBDeviceClass* klass = dev->klass;
	if (klass->cancel_packet) {
		klass->cancel_packet(dev, p);
	}
}

void USBDevice::USB_DeviceHandleDestroy(XboxDeviceState* dev)
{
	USBDeviceClass* klass = dev->klass;
	if (klass->handle_destroy) {
		klass->handle_destroy(dev);
	}
}

void USBDevice::USB_DeviceHandleAttach(XboxDeviceState* dev)
{
	USBDeviceClass* klass = dev->klass;
	if (klass->handle_attach) {
		klass->handle_attach(dev);
	}
}

void USBDevice::USB_DeviceHandleReset(XboxDeviceState* dev)
{
	USBDeviceClass* klass = dev->klass;
	if (klass->handle_reset) {
		klass->handle_reset(dev);
	}
}

void USBDevice::USB_DeviceHandleControl(XboxDeviceState* dev, USBPacket* p, int request, int value, int index, int length, uint8_t* data)
{
	USBDeviceClass* klass = dev->klass;
	if (klass->handle_control) {
		klass->handle_control(dev, p, request, value, index, length, data);
	}
}

void USBDevice::USB_DeviceHandleData(XboxDeviceState* dev, USBPacket* p)
{
	USBDeviceClass* klass = dev->klass;
	if (klass->handle_data) {
		klass->handle_data(dev, p);
	}
}

void USBDevice::USB_DeviceSetInterface(XboxDeviceState* dev, int iface, int alt_old, int alt_new)
{
	USBDeviceClass* klass = dev->klass;
	if (klass->set_interface) {
		klass->set_interface(dev, iface, alt_old, alt_new);
	}
}

void USBDevice::USB_DeviceFlushEPqueue(XboxDeviceState* dev, USBEndpoint* ep)
{
	USBDeviceClass *klass = dev->klass;
	if (klass->flush_ep_queue) {
		klass->flush_ep_queue(dev, ep);
	}
}

void USBDevice::USB_DeviceEPstopped(XboxDeviceState* dev, USBEndpoint* EP)
{
	USBDeviceClass* klass = dev->klass;
	if (klass->ep_stopped) {
		klass->ep_stopped(dev, EP);
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

void USBDevice::USB_EPsetType(XboxDeviceState* dev, int pid, int ep, uint8_t type)
{
	USBEndpoint* uep = USB_GetEP(dev, pid, ep);
	uep->Type = type;
}

uint8_t USBDevice::USB_EPsetIfnum(XboxDeviceState* dev, int pid, int ep, uint8_t ifnum)
{
	USBEndpoint* uep = USB_GetEP(dev, pid, ep);
	uep->IfNum = ifnum;
}

void USBDevice::USB_EPsetMaxPacketSize(XboxDeviceState* dev, int pid, int ep, uint16_t raw)
{
	USBEndpoint* uep = USB_GetEP(dev, pid, ep);

	// Dropped from XQEMU the calculation max_packet_size = size * microframes since that's only true
	// for high speed (usb 2.0) devices

	uep->MaxPacketSize = raw & 0x7FF;
}

void USBDevice::USB_PortLocation(USBPort* downstream, USBPort* upstream, int portnr)
{
	if (upstream) {
		std::snprintf(downstream->Path, sizeof(downstream->Path), "%s.%d",
			upstream->Path, portnr);
		downstream->HubCount = upstream->HubCount + 1;
	}
	else {
		std::snprintf(downstream->Path, sizeof(downstream->Path), "%d", portnr);
		downstream->HubCount = 0;
	}
}

void USBDevice::USB_DeviceAttach(XboxDeviceState* dev)
{
	USBPort* port = dev->Port;

	assert(port != nullptr);
	assert(!dev->Attached);

	dev->Attached++;
	USB_Attach(port);
}

void USBDevice::UsbEpInit(XboxDeviceState* dev)
{
	UsbEpReset(dev);
	QTAILQ_INIT(&dev->EP_ctl.Queue);
	for (int ep = 0; ep < USB_MAX_ENDPOINTS; ep++) {
		QTAILQ_INIT(&dev->EP_in[ep].Queue);
		QTAILQ_INIT(&dev->EP_out[ep].Queue);
	}
}

void USBDevice::UsbEpReset(XboxDeviceState* dev)
{
	dev->EP_ctl.Num = 0;
	dev->EP_ctl.Type = USB_ENDPOINT_XFER_CONTROL;
	dev->EP_ctl.IfNum = 0;
	dev->EP_ctl.MaxPacketSize = 64;
	dev->EP_ctl.Dev = dev;
	dev->EP_ctl.Pipeline = false;
	for (int ep = 0; ep < USB_MAX_ENDPOINTS; ep++) {
		dev->EP_in[ep].Num = ep + 1;
		dev->EP_out[ep].Num = ep + 1;
		dev->EP_in[ep].pid = USB_TOKEN_IN;
		dev->EP_out[ep].pid = USB_TOKEN_OUT;
		dev->EP_in[ep].Type = USB_ENDPOINT_XFER_INVALID;
		dev->EP_out[ep].Type = USB_ENDPOINT_XFER_INVALID;
		dev->EP_in[ep].IfNum = USB_INTERFACE_INVALID;
		dev->EP_out[ep].IfNum = USB_INTERFACE_INVALID;
		dev->EP_in[ep].MaxPacketSize = 0;
		dev->EP_out[ep].MaxPacketSize = 0;
		dev->EP_in[ep].Dev = dev;
		dev->EP_out[ep].Dev = dev;
		dev->EP_in[ep].Pipeline = false;
		dev->EP_out[ep].Pipeline = false;
	}
}

/*
* From XQEMU:
* This function creates a serial number for a usb device.
* The serial number should:
*   (a) Be unique within the emulator.
*   (b) Be constant, so you don't get a new one each
*       time the guest is started.
* So we are using the physical location to generate a serial number
* from it.  It has three pieces:  First a fixed, device-specific
* prefix.  Second the device path of the host controller (which is
* the pci address in most cases).  Third the physical port path.
* Results in serial numbers like this: "314159-0000:00:1d.7-3".
*/
void USBDevice::CreateSerial(XboxDeviceState* dev)
{
	const USBDesc* desc = GetUsbDeviceDesc(dev);
	int index = desc->id.iSerialNumber;
	USBDescString* s;
	char serial[64];
	char* path;
	int dst;

	assert(index != 0 && desc->str[index] != NULL);
	dst = std::snprintf(serial, sizeof(serial), "%s", desc->str[index]);
	dst += std::snprintf(serial + dst, sizeof(serial) - dst, "-%s", m_PciPath);
	dst += std::snprintf(serial + dst, sizeof(serial) - dst, "-%s", dev->Port->Path);

	QLIST_FOREACH(s, &dev->Strings, next) {
		if (s->index == index) {
			break;
		}
	}

	if (s == nullptr) {
		s = new USBDescString;
		s->index = index;
		QLIST_INSERT_HEAD(&dev->Strings, s, next);
	}

	s->str = serial;
}

const USBDesc* USBDevice::GetUsbDeviceDesc(XboxDeviceState* dev)
{
	USBDeviceClass* klass = dev->klass;
	if (dev->UsbDesc) {
		return dev->UsbDesc;
	}
	return klass->usb_desc;
}

void USBDevice::UsbDescInit(XboxDeviceState* dev)
{
	const USBDesc* desc = GetUsbDeviceDesc(dev);

	assert(desc != NULL);
	dev->Speed = USB_SPEED_FULL;
	dev->SpeedMask = 0;
	if (desc->full) {
		dev->SpeedMask |= USB_SPEED_MASK_FULL;
	}
	UsbDescSetDefaults(dev);
}

void USBDevice::UsbDescSetDefaults(XboxDeviceState* dev)
{
	const USBDesc *desc = GetUsbDeviceDesc(dev);

	assert(desc != NULL);
	switch (dev->Speed) {
	case USB_SPEED_LOW:
	case USB_SPEED_FULL: {
		dev->Device = desc->full;
		break;
	}
	default:
		EmuWarning("Unknown speed parameter %d set in %s", dev->ProductDesc.c_str());
	}
	UsbDescSetConfig(dev, 0);
}

int USBDevice::UsbDescSetConfig(XboxDeviceState* dev, int value)
{
	int i;

	if (value == 0) { // default configuration
		dev->Configuration = 0;
		dev->NumInterfaces = 0;
		dev->Config = nullptr;
	}
	else {
		for (i = 0; i < dev->Device->bNumConfigurations; i++) { // select the configuration specified
			if (dev->Device->confs[i].bConfigurationValue == value) {
				dev->Configuration = value;
				dev->NumInterfaces = dev->Device->confs[i].bNumInterfaces;
				dev->Config = dev->Device->confs + i;
				assert(dev->NumInterfaces <= USB_MAX_INTERFACES);
			}
		}
		if (i < dev->Device->bNumConfigurations) {
			return -1;
		}
	}

	for (i = 0; i < dev->NumInterfaces; i++) { // setup all interfaces for the selected configuration
		UsbDescSetInterface(dev, i, 0);
	}
	for (; i < USB_MAX_INTERFACES; i++) { // null the remaining interfaces
		dev->AltSetting[i] = 0;
		dev->Ifaces[i] = nullptr;
	}

	return 0;
}

int USBDevice::UsbDescSetInterface(XboxDeviceState* dev, int index, int value)
{
	const USBDescIface* iface;
	int old;

	iface = UsbDescFindInterface(dev, index, value);
	if (iface == nullptr) {
		return -1;
	}

	old = dev->AltSetting[index];
	dev->AltSetting[index] = value;
	dev->Ifaces[index] = iface;
	UsbDescEpInit(dev);

	if (old != value) {
		USB_DeviceSetInterface(dev, index, old, value);
	}
	return 0;
}

const USBDescIface* USBDevice::UsbDescFindInterface(XboxDeviceState* dev, int nif, int alt)
{
	const USBDescIface* iface;
	int i;

	if (!dev->Config) { // no configuration descriptor here, nothing to search
		return nullptr;
	}
	for (i = 0; i < dev->Config->nif; i++) { // find the desired interface
		iface = &dev->Config->ifs[i];
		if (iface->bInterfaceNumber == nif &&
			iface->bAlternateSetting == alt) {
			return iface;
		}
	}
	return nullptr; // not found
}

void USBDevice::UsbDescEpInit(XboxDeviceState* dev)
{
	const USBDescIface *iface;
	int i, e, pid, ep;

	UsbEpInit(dev); // reset endpoints (because we changed descriptors in use?)
	for (i = 0; i < dev->NumInterfaces; i++) {
		iface = dev->Ifaces[i];
		if (iface == nullptr) {
			continue;
		}
		for (e = 0; e < iface->bNumEndpoints; e++) {
			// From the standard:
			// "bEndpointAddress:
			// Bit 3...0: The endpoint number
			// Bit 6...4: Reserved, reset to zero
			// Bit 7: Direction -> 0 = OUT endpoint, 1 = IN endpoint
			// bmAttributes:
			// Bit 1..0: Transfer Type
			// 00 = Control, 01 = Isochronous, 10 = Bulk, 11 = Interrupt. All other bits are reserved"
			pid = (iface->eps[e].bEndpointAddress & USB_DIR_IN) ? USB_TOKEN_IN : USB_TOKEN_OUT;
			ep = iface->eps[e].bEndpointAddress & 0xF;
			USB_EPsetType(dev, pid, ep, iface->eps[e].bmAttributes & 0x03);
			USB_EPsetIfnum(dev, pid, ep, iface->bInterfaceNumber);
			USB_EPsetMaxPacketSize(dev, pid, ep, iface->eps[e].wMaxPacketSize);
		}
	}
}
