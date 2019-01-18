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

// Acknowledgment: XQEMU (GPLv2)
// https://xqemu.com/

#define _XBOXKRNL_DEFEXTRN_

#define LOG_PREFIX CXBXR_MODULE::USB

// prevent name collisions
namespace xboxkrnl
{
	#include <xboxkrnl/xboxkrnl.h> // For PKINTERRUPT, etc.
};

#include "USBDevice.h"
#include "OHCI.h"
#include "core\kernel\exports\EmuKrnl.h"  // For EmuLog
#include "common\util\CxbxUtil.h"
#include "Logging.h"

#define SETUP_STATE_IDLE    0
#define SETUP_STATE_SETUP   1
#define SETUP_STATE_DATA    2
#define SETUP_STATE_ACK     3


void USBDevice::Init()
{
	PCIBarRegister r;

	// Register Memory bar :
	r.Raw.type = PCI_BAR_TYPE_MEMORY;
	r.Memory.address = USB0_BASE >> 4;
	RegisterBAR(0, USB_SIZE, r.value);

	// Taken from https://github.com/docbrown/vxb/wiki/Xbox-Hardware-Information
	m_DeviceId = 0x01C2;
	m_VendorId = PCI_VENDOR_ID_NVIDIA;

	m_HostController = new OHCI(this);
	m_PciPath = "pci.0:02.0";
}

uint32_t USBDevice::MMIORead(int barIndex, uint32_t addr, unsigned size)
{
	// barIndex must be zero since we created the USB devices with a zero index in Init()
	assert(barIndex == 0);

	// read the register of the HC
	return m_HostController->OHCI_ReadRegister(addr);
}

void USBDevice::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size)
{
	// barIndex must be zero since we created the USB devices with a zero index in Init()
	assert(barIndex == 0);

	// write the register of the HC
	m_HostController->OHCI_WriteRegister(addr, value);
}

void USBDevice::USB_RegisterPort(USBPort* Port, int Index, int SpeedMask, USBPortOps* Ops)
{
	Port->PortIndex = Index;
	Port->SpeedMask = SpeedMask;
	Port->Operations = Ops;
	Port->Dev = nullptr;
	USB_PortLocation(Port, nullptr, Index + 1);
	m_FreePorts.push_back(Port);
}

void USBDevice::USB_UnregisterPort(USBPort* Port)
{
	auto it = std::find(m_FreePorts.begin(), m_FreePorts.end(), Port);
	assert(it != m_FreePorts.end());
	m_FreePorts.erase(it);
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
	Port->Operations->detach(Port);
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
	USB_DeviceHandleReset(dev);
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

void USBDevice::USB_PacketSetup(USBPacket* p, int Pid, USBEndpoint* Ep, uint64_t Id,
	bool ShortNotOK, bool IntReq)
{
	assert(!USB_IsPacketInflight(p));
	assert(p->IoVec.IoVecStruct != nullptr);
	p->Id = Id;
	p->Pid = Pid;
	p->Endpoint = Ep;
	p->Status = USB_RET_SUCCESS;
	p->ActualLength = 0;
	p->ShortNotOK = ShortNotOK;
	p->IntReq = IntReq;
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

	if (QTAILQ_EMPTY(&p->Endpoint->Queue)) {
		USB_ProcessOne(p);
		if (p->Status == USB_RET_ASYNC) {
			// hcd drivers cannot handle async for isoc
			assert(p->Endpoint->Type != USB_ENDPOINT_XFER_ISOC);
			// using async for interrupt packets breaks migration
			assert(p->Endpoint->Type != USB_ENDPOINT_XFER_INT);
			p->State = USB_PACKET_ASYNC;
			QTAILQ_INSERT_TAIL(&p->Endpoint->Queue, p, Queue);
		}
		else if (p->Status == USB_RET_ADD_TO_QUEUE) {
			USB_QueueOne(p);
		}
		else {
			// When pipelining is enabled usb-devices must always return async,
			// otherwise packets can complete out of order!
			assert(QTAILQ_EMPTY(&p->Endpoint->Queue));
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

	EmuLog(LOG_LEVEL::WARNING, "Packet state check failed!");
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
		switch (p->Pid) {
			case USB_TOKEN_SETUP: {
				USB_DoTokenSetup(dev, p);
				break;
			}

			case USB_TOKEN_IN: {
				DoTokenIn(dev, p);
				break;
			}

			case USB_TOKEN_OUT: {
				DoTokenOut(dev, p);
				break;
			}

			default:
				p->Status = USB_RET_STALL;
		}
	}
	else {
		// data pipe
		USB_DeviceHandleData(dev, p);
	}
}

void USBDevice::USB_DoTokenSetup(XboxDeviceState* s, USBPacket* p)
{
	int request, value, index;

	// From the USB 1.1 standard "Every Setup packet has eight bytes."
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
		USB_DeviceHandleControl(s, p, request, value, index, s->SetupLength, s->DataBuffer);
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
		if (s->SetupLength > sizeof(s->DataBuffer)) {
			DBG_PRINTF("CTRL buffer too small (%d > %zu)\n", s->SetupLength, sizeof(s->DataBuffer));
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

	assert(p->Endpoint->Num == 0);

	request = (s->SetupBuffer[0] << 8) | s->SetupBuffer[1];
	value = (s->SetupBuffer[3] << 8) | s->SetupBuffer[2];
	index = (s->SetupBuffer[5] << 8) | s->SetupBuffer[4];

	switch (s->SetupState) {
		case SETUP_STATE_ACK:
			if (!(s->SetupBuffer[0] & USB_DIR_IN)) {
				USB_DeviceHandleControl(s, p, request, value, index, s->SetupLength, s->DataBuffer);
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
				USB_PacketCopy(p, s->DataBuffer + s->SetupIndex, len);
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
	assert(p->Endpoint->Num == 0);

	switch (s->SetupState) {
		case SETUP_STATE_ACK:
			if (s->SetupBuffer[0] & USB_DIR_IN) {
				s->SetupState = SETUP_STATE_IDLE;
				// transfer OK
			}
			else {
				// ignore additional output
			}
			break;

		case SETUP_STATE_DATA:
			if (!(s->SetupBuffer[0] & USB_DIR_IN)) {
				int len = s->SetupLength - s->SetupIndex;
				if (len > p->IoVec.Size) {
					len = p->IoVec.Size;
				}
				USB_PacketCopy(p, s->DataBuffer + s->SetupIndex, len);
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
	IOVector* iov = &p->IoVec;

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
			CxbxKrnlCleanup("%s has an invalid pid: %x\n", __func__, p->Pid);
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
		klass->handle_destroy();
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

void USBDevice::USB_EPsetIfnum(XboxDeviceState* dev, int pid, int ep, uint8_t ifnum)
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
		downstream->Path = upstream->Path + '.' + std::to_string(portnr);
	}
	else {
		downstream->Path = std::to_string(portnr);
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

void USBDevice::USB_DeviceDetach(XboxDeviceState* dev)
{
	USBPort* port = dev->Port;

	assert(port != nullptr);
	assert(dev->Attached);

	USB_Detach(port);
	dev->Attached--;
}

void USBDevice::USB_EpInit(XboxDeviceState* dev)
{
	USB_EpReset(dev);
	QTAILQ_INIT(&dev->EP_ctl.Queue);
	for (int ep = 0; ep < USB_MAX_ENDPOINTS; ep++) {
		QTAILQ_INIT(&dev->EP_in[ep].Queue);
		QTAILQ_INIT(&dev->EP_out[ep].Queue);
	}
}

void USBDevice::USB_EpReset(XboxDeviceState* dev)
{
	dev->EP_ctl.Num = 0;
	dev->EP_ctl.Type = USB_ENDPOINT_XFER_CONTROL;
	dev->EP_ctl.IfNum = 0;
	dev->EP_ctl.MaxPacketSize = 64;
	dev->EP_ctl.Dev = dev;
	for (int ep = 0; ep < USB_MAX_ENDPOINTS; ep++) {
		dev->EP_in[ep].Num = ep + 1;
		dev->EP_out[ep].Num = ep + 1;
		dev->EP_in[ep].Type = USB_ENDPOINT_XFER_INVALID;
		dev->EP_out[ep].Type = USB_ENDPOINT_XFER_INVALID;
		dev->EP_in[ep].IfNum = USB_INTERFACE_INVALID;
		dev->EP_out[ep].IfNum = USB_INTERFACE_INVALID;
		dev->EP_in[ep].MaxPacketSize = 0;
		dev->EP_out[ep].MaxPacketSize = 0;
		dev->EP_in[ep].Dev = dev;
		dev->EP_out[ep].Dev = dev;
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
void USBDevice::USB_CreateSerial(XboxDeviceState* dev, std::string&& str)
{
	const USBDesc* desc = USBDesc_GetUsbDeviceDesc(dev);
	int index = desc->id.iSerialNumber;

	assert(index != 0 && str.empty() == false);
	str += '-';
	str += m_PciPath;
	str += ('-' + dev->Port->Path);

	USBDesc_SetString(dev, index, std::move(str));
}

const USBDesc* USBDevice::USBDesc_GetUsbDeviceDesc(XboxDeviceState* dev)
{
	USBDeviceClass* klass = dev->klass;
	if (dev->UsbDesc) {
		return dev->UsbDesc;
	}
	return klass->usb_desc;
}

void USBDevice::USBDesc_Init(XboxDeviceState* dev)
{
	const USBDesc* desc = USBDesc_GetUsbDeviceDesc(dev);

	assert(desc != nullptr);
	dev->Speed = USB_SPEED_FULL;
	dev->SpeedMask = 0;
	if (desc->full) {
		dev->SpeedMask |= USB_SPEED_MASK_FULL;
	}
	USBDesc_SetDefaults(dev);
}

void USBDevice::USBDesc_SetDefaults(XboxDeviceState* dev)
{
	const USBDesc *desc = USBDesc_GetUsbDeviceDesc(dev);

	assert(desc != nullptr);
	switch (dev->Speed) {
	case USB_SPEED_LOW:
	case USB_SPEED_FULL: {
		dev->Device = desc->full;
		break;
	}
	default:
		EmuLog(LOG_LEVEL::WARNING, "Unknown speed parameter %d set in %s", dev->Speed, dev->ProductDesc.c_str());
	}
	USBDesc_SetConfig(dev, 0);
}

int USBDevice::USBDesc_SetConfig(XboxDeviceState* dev, int value)
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
		USBDesc_SetInterface(dev, i, 0);
	}
	for (; i < USB_MAX_INTERFACES; i++) { // null the remaining interfaces
		dev->AltSetting[i] = 0;
		dev->Ifaces[i] = nullptr;
	}

	return 0;
}

int USBDevice::USBDesc_SetInterface(XboxDeviceState* dev, int index, int value)
{
	const USBDescIface* iface;
	int old;

	iface = USBDesc_FindInterface(dev, index, value);
	if (iface == nullptr) {
		return -1;
	}

	old = dev->AltSetting[index];
	dev->AltSetting[index] = value;
	dev->Ifaces[index] = iface;
	USBDesc_EpInit(dev);

	if (old != value) {
		USB_DeviceSetInterface(dev, index, old, value);
	}
	return 0;
}

const USBDescIface* USBDevice::USBDesc_FindInterface(XboxDeviceState* dev, int nif, int alt)
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

void USBDevice::USBDesc_EpInit(XboxDeviceState* dev)
{
	const USBDescIface *iface;
	int i, e, pid, ep;

	USB_EpInit(dev); // reset endpoints
	for (i = 0; i < dev->NumInterfaces; i++) {
		iface = dev->Ifaces[i];
		if (iface == nullptr) {
			continue;
		}
		for (e = 0; e < iface->bNumEndpoints; e++) {
			// From the USB 1.1 standard:
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

int USBDevice::USBDesc_HandleControl(XboxDeviceState* dev, USBPacket *p,
	int request, int value, int index, int length, uint8_t* data)
{
	const USBDesc* desc = USBDesc_GetUsbDeviceDesc(dev);
	int ret = -1;

	assert(desc != nullptr);
	switch (request) {
		case DeviceOutRequest | USB_REQ_SET_ADDRESS: {
			// From the USB 1.1 standard: "This request sets the device address for all future device accesses.
			// The wValue field specifies the device address to use for all subsequent accesses"
			dev->Addr = value;
			DBG_PRINTF("Address 0x%X set for device %s\n", dev->Addr, dev->ProductDesc.c_str());
			ret = 0;
			break;
		}

		case DeviceRequest | USB_REQ_GET_DESCRIPTOR: {
			// From the USB 1.1 standard: "This request returns the specified descriptor if the descriptor exists.
			// The wValue field specifies the descriptor type in the high byte and the descriptor index in the low byte.
			// The wIndex field specifies the Language ID for string descriptors or is reset to zero for other descriptors"
			ret = USBDesc_HandleStandardGetDescriptor(dev, p, value, data, length);
			break;
		}

		case DeviceRequest | USB_REQ_GET_CONFIGURATION: {
			// From the USB 1.1 standard: "This request returns the current device configuration value.
			// If the returned value is zero, the device is not configured"
			data[0] = dev->Config ? dev->Config->bConfigurationValue : 0;
			p->ActualLength = 1;
			ret = 0;
			break;
		}

		case DeviceOutRequest | USB_REQ_SET_CONFIGURATION: {
			// From the USB 1.1 standard: "This request sets the device configuration. The lower byte of the wValue field specifies the desired configuration.
			// This configuration value must be zero or match a configuration value from a configuration descriptor"
			ret = USBDesc_SetConfig(dev, value);
			DBG_PRINTF("Received standard SetConfiguration() request for device at address 0x%X. Configuration selected is %d and returned %d\n",
				dev->Addr, value, ret);
			break;
		}

		case DeviceRequest | USB_REQ_GET_STATUS: {
			// From the USB 1.1 standard: "This request returns the status for the specified recipient. The Recipient bits of the bmRequestType field
			// specify the desired recipient. The data returned is the current status of the specified recipient."
			// From XQEMU:
			/* Default state: Device behavior when this request is received while
			*                the device is in the Default state is not specified.
			* We return the same value that a configured device would return if
			* it used the first configuration. */
			const USBDescConfig* config = dev->Config ? dev->Config : &dev->Device->confs[0];
			data[0] = 0;
			if (config->bmAttributes & 0x40) {
				data[0] |= 1 << USB_DEVICE_SELF_POWERED;
			}
			if (dev->RemoteWakeup) {
				data[0] |= 1 << USB_DEVICE_REMOTE_WAKEUP;
			}
			data[1] = 0x00;
			p->ActualLength = 2;
			ret = 0;
			break;
		}

		case DeviceOutRequest | USB_REQ_CLEAR_FEATURE: {
			// From the USB 1.1 standard: "This request is used to clear or disable a specific feature.
			// Feature selector values in wValue must be appropriate to the recipient"
			if (value == USB_DEVICE_REMOTE_WAKEUP) {
				dev->RemoteWakeup = 0;
				ret = 0;
			}
			DBG_PRINTF("Received standard ClearFeature() request for device at address 0x%X. Feature selected is %d and returned %d\n",
				dev->Addr, value, ret);
			break;
		}

		case DeviceOutRequest | USB_REQ_SET_FEATURE: {
			// From the USB 1.1 standard: "This request is used to set or enable a specific feature.
			// Feature selector values in wValue must be appropriate to the recipient"
			if (value == USB_DEVICE_REMOTE_WAKEUP) {
				dev->RemoteWakeup = 1;
				ret = 0;
			}
			DBG_PRINTF("Received standard SetFeature() request for device at address 0x%X. Feature selected is %d and returned %d\n",
				dev->Addr, value, ret);
			break;
		}

		case InterfaceRequest | USB_REQ_GET_INTERFACE: {
			// From the USB 1.1 standard: "This request returns the selected alternate setting for the specified interface.
			// wValue = Zero; wIndex = Interface"
			if (index < 0 || index >= dev->NumInterfaces) {
				break;
			}
			data[0] = dev->AltSetting[index];
			p->ActualLength = 1;
			ret = 0;
			break;
		}

		case InterfaceOutRequest | USB_REQ_SET_INTERFACE: {
			// From the USB 1.1 standard: "This request allows the host to select an alternate setting for the specified interface"
			// wValue = Alternative Setting; wIndex = Interface
			ret = USBDesc_SetInterface(dev, index, value);
			DBG_PRINTF("Received standard SetInterface() request for device at address 0x%X. Interface selected is %d, Alternative Setting \
is %d and returned %d\n", dev->Addr, index, value, ret);
			break;
		}

		default:
			break;
	}
	return ret;
}

int USBDevice::USBDesc_HandleStandardGetDescriptor(XboxDeviceState* dev, USBPacket* p,
	int value, uint8_t* dest, size_t len)
{
	const USBDesc* desc = USBDesc_GetUsbDeviceDesc(dev);
	uint8_t buf[256];
	uint8_t type = value >> 8;    // recover descriptor type from wValue
	uint8_t index = value & 0xFF; // recover descriptor index from wValue
	int ret = -1;
	int flags = 0;

	// Dropped from XQEMU bcdUSB check for usb 3.0 devices

	// From the USB 1.1 standard: "The standard request to a device supports three types of descriptors: DEVICE, CONFIGURATION, and STRING."

	switch (type) {
		case USB_DT_DEVICE: {
			ret = USB_ReadDeviceDesc(&desc->id, dev->Device, buf, sizeof(buf));
			DBG_PRINTF("Read operation of device descriptor of device 0x%X returns %d\n", dev->Addr, ret);
			break;
		}

		case USB_DT_CONFIG: {
			if (index < dev->Device->bNumConfigurations) {
				ret = USB_ReadConfigurationDesc(dev->Device->confs + index, flags, buf, sizeof(buf));
			}
			DBG_PRINTF("Read operation of configuration descriptor %d of device 0x%X returns %d\n", index, dev->Addr, ret);
			break;
		}

		case USB_DT_STRING: {
			ret = USB_ReadStringDesc(dev, index, buf, sizeof(buf));
			DBG_PRINTF("Read operation of string descriptor %d of device 0x%X returns %d\n", index, dev->Addr, ret);
			break;
		}

		// Dropped from XQEMU descriptor types USB_DT_DEVICE_QUALIFIER (6), USB_DT_OTHER_SPEED_CONFIG (7) -> usb 2.0 only and reserved in usb 3.0,
		// USB_DT_BOS (15) and USB_DT_DEBUG (10) -> usb 3.0 only

		default:
			EmuLog(LOG_LEVEL::WARNING, "%s has a device address %d of unknown type %d (len %zd)", __func__, dev->Addr, type, len);
			break;
	}

	if (ret > 0) {
		if (ret > len) {
			ret = len;
		}
		std::memcpy(dest, buf, ret);
		p->ActualLength = ret;
		ret = 0;
	}
	return ret;
}

int USBDevice::USB_ReadDeviceDesc(const USBDescID* id, const USBDescDevice* dev, uint8_t* dest, size_t len)
{
	uint8_t bLength = 0x12; // a device descriptor is 18 bytes large
	USBDescriptor* d = reinterpret_cast<USBDescriptor*>(dest);

	if (len < bLength) {
		return -1;
	}

	d->bLength = bLength;
	d->bDescriptorType = USB_DT_DEVICE;

	d->u.device.bcdUSB_lo = GET_WORD_LOW(dev->bcdUSB);
	d->u.device.bcdUSB_hi = GET_WORD_HIGH(dev->bcdUSB);
	d->u.device.bDeviceClass = dev->bDeviceClass;
	d->u.device.bDeviceSubClass = dev->bDeviceSubClass;
	d->u.device.bDeviceProtocol = dev->bDeviceProtocol;
	d->u.device.bMaxPacketSize0 = dev->bMaxPacketSize0;

	d->u.device.idVendor_lo = GET_WORD_LOW(id->idVendor);
	d->u.device.idVendor_hi = GET_WORD_HIGH(id->idVendor);
	d->u.device.idProduct_lo = GET_WORD_LOW(id->idProduct);
	d->u.device.idProduct_hi = GET_WORD_HIGH(id->idProduct);
	d->u.device.bcdDevice_lo = GET_WORD_LOW(id->bcdDevice);
	d->u.device.bcdDevice_hi = GET_WORD_HIGH(id->bcdDevice);
	d->u.device.iManufacturer = id->iManufacturer;
	d->u.device.iProduct = id->iProduct;
	d->u.device.iSerialNumber = id->iSerialNumber;

	d->u.device.bNumConfigurations = dev->bNumConfigurations;

	return bLength;
}

int USBDevice::USB_ReadConfigurationDesc(const USBDescConfig* conf, int flags, uint8_t* dest, size_t len)
{
	uint8_t  bLength = 0x09; // a configuration descriptor is 9 bytes large
	uint16_t wTotalLength = 0;
	USBDescriptor* d = reinterpret_cast<USBDescriptor*>(dest);
	int i, rc;

	if (len < bLength) {
		return -1;
	}

	// From the USB 1.1 standard: "A request for a configuration descriptor returns the configuration descriptor, all interface
	// descriptors, and endpoint descriptors for all of the interfaces in a single request."

	d->bLength = bLength;
	d->bDescriptorType = USB_DT_CONFIG;

	d->u.config.bNumInterfaces = conf->bNumInterfaces;
	d->u.config.bConfigurationValue = conf->bConfigurationValue;
	d->u.config.iConfiguration = conf->iConfiguration;
	d->u.config.bmAttributes = conf->bmAttributes;
	d->u.config.bMaxPower = conf->bMaxPower;
	wTotalLength += bLength;

	for (i = 0; i < conf->nif; i++) {
		rc = USB_ReadInterfaceDesc(conf->ifs + i, flags, dest + wTotalLength, len - wTotalLength);
		if (rc < 0) {
			return rc;
		}
		wTotalLength += rc;
	}

	d->u.config.wTotalLength_lo = GET_WORD_LOW(wTotalLength);
	d->u.config.wTotalLength_hi = GET_WORD_HIGH(wTotalLength);
	return wTotalLength;
}

int USBDevice::USB_ReadInterfaceDesc(const USBDescIface* iface, int flags, uint8_t* dest, size_t len)
{
	uint8_t bLength = 0x09; // an interface descriptor is 9 bytes large
	int i, rc, pos = 0;
	USBDescriptor* d = reinterpret_cast<USBDescriptor*>(dest);

	if (len < bLength) {
		return -1;
	}

	// From the USB 1.1 standard: "The first interface descriptor follows the configuration descriptor.
	// The endpoint descriptors for the first interface follow the first interface descriptor.
	// If there are additional interfaces, their interface descriptor and endpoint descriptors
	// follow the first interface’s endpoint descriptors. Class-specific and/or vendor-specific
	// descriptors follow the standard descriptors they extend or modify."

	d->bLength = bLength;
	d->bDescriptorType = USB_DT_INTERFACE;

	d->u.iface.bInterfaceNumber = iface->bInterfaceNumber;
	d->u.iface.bAlternateSetting = iface->bAlternateSetting;
	d->u.iface.bNumEndpoints = iface->bNumEndpoints;
	d->u.iface.bInterfaceClass = iface->bInterfaceClass;
	d->u.iface.bInterfaceSubClass = iface->bInterfaceSubClass;
	d->u.iface.bInterfaceProtocol = iface->bInterfaceProtocol;
	d->u.iface.iInterface = iface->iInterface;
	pos += bLength;

	for (i = 0; i < iface->ndesc; i++) {
		rc = USB_ReadOtherDesc(iface->descs + i, dest + pos, len - pos);
		if (rc < 0) {
			return rc;
		}
		pos += rc;
	}

	for (i = 0; i < iface->bNumEndpoints; i++) {
		rc = USB_ReadEndpointDesc(iface->eps + i, flags, dest + pos, len - pos);
		if (rc < 0) {
			return rc;
		}
		pos += rc;
	}

	return pos;
}

size_t USBDevice::USB_ReadOtherDesc(const USBDescOther* desc, uint8_t* dest, size_t len)
{
	size_t bLength = desc->length ? desc->length : desc->data[0];

	if (len < bLength) {
		return -1;
	}

	std::memcpy(dest, desc->data, bLength);
	return bLength;
}

int USBDevice::USB_ReadEndpointDesc(const USBDescEndpoint* ep, int flags, uint8_t* dest, size_t len)
{
	size_t bLength = ep->is_audio ? 0x09 : 0x07; // an endpoint descriptor is 7 bytes large (or 9 if it is an audio device)
	size_t extralen = ep->extra ? ep->extra[0] : 0;
	USBDescriptor* d = reinterpret_cast<USBDescriptor*>(dest);

	if (len < bLength + extralen) {
		return -1;
	}

	d->bLength = static_cast<uint8_t>(bLength);
	d->bDescriptorType = USB_DT_ENDPOINT;

	d->u.endpoint.bEndpointAddress = ep->bEndpointAddress;
	d->u.endpoint.bmAttributes = ep->bmAttributes;
	d->u.endpoint.wMaxPacketSize_lo = GET_WORD_LOW(ep->wMaxPacketSize);
	d->u.endpoint.wMaxPacketSize_hi = GET_WORD_HIGH(ep->wMaxPacketSize);
	d->u.endpoint.bInterval = ep->bInterval;
	if (ep->is_audio) {
		d->u.endpoint.bRefresh = ep->bRefresh;
		d->u.endpoint.bSynchAddress = ep->bSynchAddress;
	}

	// Dropped from XQEMU the reading of the SuperSpeed Endpoint Companion descriptors since those are usb 3.0 specific

	if (ep->extra) {
		std::memcpy(dest + bLength, ep->extra, extralen);
	}

	return bLength + extralen;
}

int USBDevice::USB_ReadStringDesc(XboxDeviceState* dev, int index, uint8_t* dest, size_t len)
{
	size_t bLength, i;
	unsigned int pos;
	const char* str;

	if (len < 4) {
		return -1;
	}

	// From the USB 1.1 standard: "String index zero for all languages returns a string descriptor
	// that contains an array of two-byte LANGID codes supported by the device"

	if (index == 0) {
		/* language ids */
		dest[0] = 4;
		dest[1] = USB_DT_STRING;
		dest[2] = 0x09;
		dest[3] = 0x04; // we only support English (United States)
		return 4;
	}

	str = USBDesc_GetString(dev, index);
	if (str == nullptr) {
		return 0;
	}

	// From the USB 1.1 standard: "The UNICODE string descriptor is not NULL-terminated. The string length is
	// computed by subtracting two from the value of the first byte of the descriptor"

	bLength = std::strlen(str) * 2 + 2;
	dest[0] = bLength;
	dest[1] = USB_DT_STRING;
	i = 0; pos = 2;
	while (pos + 1 < bLength && pos + 1 < len) {
		dest[pos++] = str[i++];
		dest[pos++] = 0;
	}
	return pos;
}

void USBDevice::USBDesc_SetString(XboxDeviceState* dev, int index, std::string&& str)
{
	USBDescString* s;

	QLIST_FOREACH(s, &dev->Strings, next) {
		if (s->index == index) {
			break;
		}
	}

	if (s == nullptr) {
		s = new USBDescString();
		s->index = index;
		QLIST_INSERT_HEAD(&dev->Strings, s, next);
	}

	s->str = str;
}

const char* USBDevice::USBDesc_GetString(XboxDeviceState* dev, int index)
{
	USBDescString* s;

	QLIST_FOREACH(s, &dev->Strings, next) {
		if (s->index == index) {
			return s->str.c_str();
		}
	}

	return nullptr;
}

