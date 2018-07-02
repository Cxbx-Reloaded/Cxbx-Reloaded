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
// *   Cxbx->devices->usb->Hub.cpp
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

#include "OHCI.h"
#include "Hub.h"
#include "..\CxbxKrnl\EmuKrnl.h" // For EmuWarning

#define NUM_PORTS 8

#define PORT_STAT_CONNECTION   0x0001
#define PORT_STAT_ENABLE       0x0002
#define PORT_STAT_SUSPEND      0x0004
#define PORT_STAT_OVERCURRENT  0x0008
#define PORT_STAT_RESET        0x0010
#define PORT_STAT_POWER        0x0100
#define PORT_STAT_LOW_SPEED    0x0200

#define PORT_STAT_C_CONNECTION	0x0001
#define PORT_STAT_C_ENABLE      0x0002
#define PORT_STAT_C_SUSPEND     0x0004
#define PORT_STAT_C_OVERCURRENT	0x0008
#define PORT_STAT_C_RESET       0x0010


// To avoid including Xbox.h
extern USBDevice* g_USB0;
extern USBDevice* g_USB1;

// This array is used to translate an xbox player to the corresponding usb port
// The port associations are taken from XQEMU
int PlayerToUsbArray[] = {
	-1,
	3,
	4,
	1,
	2,
};

struct USBHubPort {
	USBPort port;          // downstream port status
	uint16_t wPortStatus;  // Port Status Field, in accordance with the standard
	uint16_t wPortChange;  // Port Change Field, in accordance with the standard
};

struct USBHubState {
	XboxDeviceState dev;         // hub device status
	USBEndpoint* intr;           // interrupt endpoint of the hub
	USBHubPort ports[NUM_PORTS]; // downstream ports of the hub
};

USBDescIface::USBDescIface(bool bDefault)
{
	std::memset(this, 0, sizeof(USBDescIface));
	if (bDefault) {
		descs = new USBDescOther();
		eps = new USBDescEndpoint();
		bInterfaceNumber = 0;
		bNumEndpoints = 1;
		bInterfaceClass = USB_CLASS_HUB;
		eps->bEndpointAddress = USB_DIR_IN | 0x01;
		eps->bmAttributes = USB_ENDPOINT_XFER_INT;
		eps->wMaxPacketSize = 1 + (NUM_PORTS + 7) / 8;
		eps->bInterval = 0xFF;
	}
}

USBDescIface::~USBDescIface()
{
	delete descs; // always one struct of this?
	if (bNumEndpoints != 1) {
		delete[] eps;
		return;
	}
	delete eps;
}

static const USBDescIface desc_iface_hub(true);

USBDescDevice::USBDescDevice(bool bDefault)
{
	std::memset(this, 0, sizeof(USBDescDevice));
	if (bDefault) {
		USBDescConfig* pUSBDescConfig = new USBDescConfig();
		bcdUSB = 0x0110;
		bDeviceClass = USB_CLASS_HUB;
		bMaxPacketSize0 = 8;
		bNumConfigurations = 1;
		pUSBDescConfig->bNumInterfaces = 1;
		pUSBDescConfig->bConfigurationValue = 1;
		pUSBDescConfig->bmAttributes = 0xE0;
		pUSBDescConfig->nif = 1;
		pUSBDescConfig->ifs = &desc_iface_hub;
		confs = pUSBDescConfig;
	}
}

USBDescDevice::~USBDescDevice()
{
	if (bNumConfigurations != 1) {
		delete[] confs;
		return;
	}
	delete confs;
}

static const USBDescDevice desc_device_hub(true);

static USBDescStrings desc_strings = { nullptr, "Cxbx-Reloaded", "Cxbx-Reloaded USB Hub", "314159" };

USBDesc::USBDesc(bool bDefault)
{
	std::memset(this, 0, sizeof(USBDesc));
	if (bDefault) {
		id.idVendor = 0x0409;
		id.idProduct = 0x55AA;
		id.bcdDevice = 0x0101;
		id.iManufacturer = STR_MANUFACTURER;
		id.iProduct = STR_PRODUCT;
		id.iSerialNumber = STR_SERIALNUMBER;
		full = &desc_device_hub;
		str = desc_strings;
	}
}

static const USBDesc desc_hub(true);

int Hub::Init(int pport)
{
	ClassInitFn();
	UsbEpInit();
    int rc = UsbClaimPort(pport);
    if (rc != 0) {
        return rc;
    }
    rc = m_UsbDev->USB_DeviceInit(m_pDeviceStruct);
    if (rc != 0) {
        UsbReleasePort(m_pDeviceStruct);
        return rc;
    }
    rc = usb_device_attach(dev);
    if (rc != 0) {
        usb_qdev_exit(qdev);
        return rc;
    }
}

Hub::~Hub()
{
	HubCleanUp();
}

void Hub::HubCleanUp()
{
	delete m_pPeripheralFuncStruct;
	delete m_pDeviceStruct;
	m_pPeripheralFuncStruct = nullptr;
	m_pDeviceStruct = nullptr;
}

void Hub::ClassInitFn()
{
	m_pPeripheralFuncStruct = new USBDeviceClass();
	m_HubState = new USBHubState();
	m_pDeviceStruct = &m_HubState->dev;
	
	m_pDeviceStruct->ProductDesc = "Cxbx-Reloaded USB Hub";
	QLIST_INIT(&m_pDeviceStruct->Strings);
	m_pDeviceStruct->klass = m_pPeripheralFuncStruct;

	{
		using namespace std::placeholders;
		
		m_pPeripheralFuncStruct->init           = std::bind(&Hub::UsbHub_Initfn, this, _1);
		m_pPeripheralFuncStruct->find_device    = std::bind(&Hub::UsbHub_FindDevice, this, _1, _2);
		m_pPeripheralFuncStruct->handle_reset   = std::bind(&Hub::UsbHub_HandleReset, this, _1);
		m_pPeripheralFuncStruct->handle_control = std::bind(&Hub::UsbHub_HandleControl, this, _1, _2, _3, _4, _5, _6, _7);
		m_pPeripheralFuncStruct->handle_data    = std::bind(&Hub::UsbHub_HandleData, this, _1, _2);
		m_pPeripheralFuncStruct->handle_destroy = std::bind(&Hub::UsbHub_HandleDestroy, this, _1);
		m_pPeripheralFuncStruct->product_desc   = m_pDeviceStruct->ProductDesc.c_str();
		m_pPeripheralFuncStruct->usb_desc       = &desc_hub;
	}
}

void Hub::UsbEpInit()
{
    UsbEpReset();
    QTAILQ_INIT(&m_pDeviceStruct->EP_ctl.Queue);
    for (int ep = 0; ep < USB_MAX_ENDPOINTS; ep++) {
        QTAILQ_INIT(&m_pDeviceStruct->EP_in[ep].Queue);
        QTAILQ_INIT(&m_pDeviceStruct->EP_out[ep].Queue);
    }
}

void Hub::UsbEpReset()
{
    m_pDeviceStruct->EP_ctl.Num = 0;
    m_pDeviceStruct->EP_ctl.Type = USB_ENDPOINT_XFER_CONTROL;
    m_pDeviceStruct->EP_ctl.IfNum = 0;
    m_pDeviceStruct->EP_ctl.MaxPacketSize = 64;
    m_pDeviceStruct->EP_ctl.Dev = m_pDeviceStruct;
    m_pDeviceStruct->EP_ctl.Pipeline = false;
    for (int ep = 0; ep < USB_MAX_ENDPOINTS; ep++) {
        m_pDeviceStruct->EP_in[ep].Num = ep + 1;
        m_pDeviceStruct->EP_out[ep].Num = ep + 1;
        m_pDeviceStruct->EP_in[ep].pid = USB_TOKEN_IN;
        m_pDeviceStruct->EP_out[ep].pid = USB_TOKEN_OUT;
        m_pDeviceStruct->EP_in[ep].Type = USB_ENDPOINT_XFER_INVALID;
        m_pDeviceStruct->EP_out[ep].Type = USB_ENDPOINT_XFER_INVALID;
        m_pDeviceStruct->EP_in[ep].IfNum = USB_INTERFACE_INVALID;
        m_pDeviceStruct->EP_out[ep].IfNum = USB_INTERFACE_INVALID;
        m_pDeviceStruct->EP_in[ep].MaxPacketSize = 0;
        m_pDeviceStruct->EP_out[ep].MaxPacketSize = 0;
        m_pDeviceStruct->EP_in[ep].Dev = m_pDeviceStruct;
        m_pDeviceStruct->EP_out[ep].Dev = m_pDeviceStruct;
        m_pDeviceStruct->EP_in[ep].Pipeline = false;
        m_pDeviceStruct->EP_out[ep].Pipeline = false;
    }
}

int Hub::UsbClaimPort(int pport)
{
	int usb_port;

	assert(m_pDeviceStruct->Port == nullptr);

	if (pport > 4 || pport < 1) { return -1; };

	usb_port = PlayerToUsbArray[pport];
	if (usb_port > 2) {
		m_UsbDev = g_USB0;
		m_UsbDev->m_HostController->OHCI_AssignUsbPortStruct(usb_port - 3, m_pDeviceStruct);
	}
	else {
		m_UsbDev = g_USB1;
		m_UsbDev->m_HostController->OHCI_AssignUsbPortStruct(usb_port - 1, m_pDeviceStruct);
	}

	return 0;
}

void Hub::UsbReleasePort(XboxDeviceState* dev)
{
	USBPort* port = dev->Port;

	assert(port != nullptr);

	port->Dev = nullptr;
	dev->Port = nullptr;
}

int Hub::UsbHub_Initfn(XboxDeviceState* dev)
{
	USBHubState* s = container_of(dev, USBHubState, dev);
	USBHubPort* port;
	int i;

	if (dev->Port->HubCount == 5) {
		DbgPrintf("Hub: chain too deep\n");
		return -1;
	}

	CreateSerial(dev);
	UsbDescInit(dev);
	s->intr = m_UsbDev->USB_GetEP(dev, USB_TOKEN_IN, 1);
	for (i = 0; i < NUM_PORTS; i++) {
		port = &s->ports[i];
		m_UsbDev->USB_RegisterPort(&port->port, i, USB_SPEED_MASK_LOW | USB_SPEED_MASK_FULL);
		m_UsbDev->USB_PortLocation(&port->port, dev->Port, i + 1);
	}
	UsbHub_HandleReset(dev);
	return 0;
}

void Hub::UsbHub_HandleReset(XboxDeviceState* dev)
{
	USBHubState* s = container_of(dev, USBHubState, dev);
	USBHubPort* port;

	for (int i = 0; i < NUM_PORTS; i++) {
		port = s->ports + i;
		port->wPortStatus = PORT_STAT_POWER;
		port->wPortChange = 0;
		if (port->port.Dev && port->port.Dev->Attached) {
			port->wPortStatus |= PORT_STAT_CONNECTION;
			port->wPortChange |= PORT_STAT_C_CONNECTION;
			if (port->port.Dev->Speed == USB_SPEED_LOW) {
				port->wPortStatus |= PORT_STAT_LOW_SPEED;
			}
		}
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
void Hub::CreateSerial(XboxDeviceState* dev)
{
	const USBDesc* desc = GetUsbDeviceDesc(dev);
	int index = desc->id.iSerialNumber;
	USBDescString* s;
	char serial[64];
	char* path;
	int dst;

	assert(index != 0 && desc->str[index] != NULL);
	dst = std::snprintf(serial, sizeof(serial), "%s", desc->str[index]);
	dst += std::snprintf(serial + dst, sizeof(serial) - dst, "-%s", m_UsbDev->m_PciPath);
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

const USBDesc* Hub::GetUsbDeviceDesc(XboxDeviceState* dev)
{
	USBDeviceClass* klass = dev->klass;
	if (dev->UsbDesc) {
		return dev->UsbDesc;
	}
	return klass->usb_desc;
}

void Hub::UsbDescInit(XboxDeviceState* dev)
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

void Hub::UsbDescSetDefaults(XboxDeviceState* dev)
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

int Hub::UsbDescSetConfig(XboxDeviceState* dev, int value)
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

int Hub::UsbDescSetInterface(XboxDeviceState* dev, int index, int value)
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
		m_UsbDev->USB_DeviceSetInterface(dev, index, old, value);
	}
	return 0;
}

const USBDescIface* Hub::UsbDescFindInterface(XboxDeviceState* dev, int nif, int alt)
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

void Hub::UsbDescEpInit(XboxDeviceState* dev)
{
	const USBDescIface *iface;
	int i, e, pid, ep;

	UsbEpInit(); // reset endpoints (because we changed descriptors in use?)
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
			m_UsbDev->USB_EPsetType(dev, pid, ep, iface->eps[e].bmAttributes & 0x03);
			m_UsbDev->USB_EPsetIfnum(dev, pid, ep, iface->bInterfaceNumber);
			m_UsbDev->USB_EPsetMaxPacketSize(dev, pid, ep, iface->eps[e].wMaxPacketSize);
		}
	}
}
