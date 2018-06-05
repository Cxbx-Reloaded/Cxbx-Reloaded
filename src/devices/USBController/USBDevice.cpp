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
		m_pHostController1 = new OHCI(1, this);
		return;
	}

	m_pHostController2 = new OHCI(9, this);
}

uint32_t USBDevice::MMIORead(int barIndex, uint32_t addr, unsigned size)
{
	// barIndex must be zero since we created the USB devices with a zero index in Init()
	assert(barIndex == 0);

	// Figure out the correct OHCI object and read the register
	if (addr < USB1_BASE) {
		// USB0 queried
		return m_pHostController1->OHCI_ReadRegister(addr);
	}

	// USB1 queried
	return m_pHostController2->OHCI_ReadRegister(addr);
}

void USBDevice::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size)
{
	// barIndex must be zero since we created the USB devices with a zero index in Init()
	assert(barIndex == 0);

	// Figure out the correct OHCI object and write the value to the register
	if (addr < USB1_BASE) {
		// USB0 queried
		m_pHostController1->OHCI_WriteRegister(addr, value);
		return;
	}

	// USB1 queried
	m_pHostController2->OHCI_WriteRegister(addr, value);
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
	assert(!usb_packet_is_inflight(p));
	assert(p->iov.iov != NULL);
	p->Id = Id;
	p->Pid = Pid;
	p->Endpoint = Ep;
	p->Stream = Stream;
	p->Status = USB_RET_SUCCESS;
	p->ActualLength = 0;
	p->Parameter = 0;
	p->ShortNotOK = ShortNotOK;
	p->IntReq = IntReq;
	p->Combined = NULL;
	qemu_iovec_reset(&p->iov);
	usb_packet_set_state(p, USB_PACKET_SETUP);
}
