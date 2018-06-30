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

#define NUM_PORTS 8


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
	USBPort port;          // downstream ports of the hub
	uint16_t wPortStatus;
	uint16_t wPortChange;
};

struct USBHubState {
	XboxDeviceState dev;
	USBEndpoint* intr;
	USBHubPort ports[NUM_PORTS];
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
	delete descs;
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

USBDescDevice::~USBDescDevice() { delete confs; }

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
        usb_release_port(dev);
        return rc;
    }
    if (dev->auto_attach) {
        rc = usb_device_attach(dev);
        if (rc != 0) {
            usb_qdev_exit(qdev);
            return rc;
        }
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
	USBPort* port;
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

int Hub::UsbHub_Initfn(XboxDeviceState* dev)
{
	USBHubState* s = container_of(dev, USBHubState, dev);
	USBHubPort *port;
	int i;

	if (dev->Port->HubCount == 5) {
		DbgPrintf("Hub: chain too deep");
		return -1;
	}

	usb_desc_create_serial(dev);
	usb_desc_init(dev);
	s->intr = usb_ep_get(dev, USB_TOKEN_IN, 1);
	for (i = 0; i < NUM_PORTS; i++) {
		port = &s->ports[i];
		usb_register_port(usb_bus_from_device(dev),
			&port->port, s, i, &usb_hub_port_ops,
			USB_SPEED_MASK_LOW | USB_SPEED_MASK_FULL);
		usb_port_location(&port->port, dev->port, i + 1);
	}
	usb_hub_handle_reset(dev);
	return 0;
}

/*
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
	DeviceState *hcd = dev->qdev.parent_bus->parent;
	const USBDesc* desc = GetUsbDeviceDesc(dev);
	int index = desc->id.iSerialNumber;
	char serial[64];
	char* path;
	int dst;

	if (dev->serial) {
		/* 'serial' usb bus property has priority if present */
		usb_desc_set_string(dev, index, dev->serial);
		return;
	}

	assert(index != 0 && desc->str[index] != NULL);
	dst = snprintf(serial, sizeof(serial), "%s", desc->str[index]);
	path = qdev_get_dev_path(hcd);
	if (path) {
		dst += snprintf(serial + dst, sizeof(serial) - dst, "-%s", path);
	}
	dst += snprintf(serial + dst, sizeof(serial) - dst, "-%s", dev->port->path);
	usb_desc_set_string(dev, index, serial);
}

const USBDesc* Hub::GetUsbDeviceDesc(XboxDeviceState* dev)
{
	USBDeviceClass* klass = dev->klass;
	if (dev->usb_desc) {
		return dev->usb_desc;
	}
	return klass->usb_desc;
}

void usb_desc_set_string(USBDevice *dev, uint8_t index, const char *str)
{
	USBDescString *s;

	QLIST_FOREACH(s, &dev->strings, next) {
		if (s->index == index) {
			break;
		}
	}
	if (s == NULL) {
		s = g_malloc0(sizeof(*s));
		s->index = index;
		QLIST_INSERT_HEAD(&dev->strings, s, next);
	}
	g_free(s->str);
	s->str = g_strdup(str);
}
