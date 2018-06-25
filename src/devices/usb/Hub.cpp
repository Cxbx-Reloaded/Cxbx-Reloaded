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

#include "Hub.h"


USBDescIface::USBDescIface(bool bDefault)
{
	std::memset(this, 0, sizeof(USBDescIface));
	if (bDefault) {
		bInterfaceNumber = 0;
		bNumEndpoints = 1;
		bInterfaceClass = USB_CLASS_HUB;
		eps = new USBDescEndpoint();
		eps->bEndpointAddress = USB_DIR_IN | 0x01;
		eps->bmAttributes = USB_ENDPOINT_XFER_INT;
		eps->wMaxPacketSize = 1 + (NUM_PORTS + 7) / 8;
		eps->bInterval = 0xFF;
	}
}

USBDescIface::~USBDescIface() { delete eps; }

static const USBDescIface desc_iface_hub(true);

USBDescDevice::USBDescDevice()
{
	std::memset(this, 0, sizeof(USBDescDevice));
	bcdUSB = 0x0110;
	bDeviceClass = USB_CLASS_HUB;
	bMaxPacketSize0 = 8;
	bNumConfigurations = 1;
	confs->bNumInterfaces = 1;
	confs->bConfigurationValue = 1;
	confs->bmAttributes = 0xE0;
	confs->nif = 1;
	confs->ifs = &desc_iface_hub;
}

static const USBDescStrings desc_strings = {
    [STR_MANUFACTURER] = "Cxbx-Reloaded",
    [STR_PRODUCT]      = "Cxbx-Reloaded USB Hub",
    [STR_SERIALNUMBER] = "314159",
};

static const USBDesc desc_hub = {
    id.idVendor           = 0x0409,
    id.idProduct          = 0x55AA,
    id.bcdDevice          = 0x0101,
    id.iManufacturer      = STR_MANUFACTURER,
    id.iProduct           = STR_PRODUCT,
    id.iSerialNumber      = STR_SERIALNUMBER,
    full                  = &desc_device_hub,
	str                   = desc_strings,
};

void Hub::Init()
{
	ClassInitFn();
	UsbEpInit();
    rc = usb_claim_port(dev);
    if (rc != 0) {
        return rc;
    }
    rc = usb_device_init(dev);
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

void Hub::ClassInitFn()
{
	//DeviceClass *dc = DEVICE_CLASS(klass);
	m_pPeripheralFuncStruct = new USBDeviceClass;
	m_pDeviceStruct = new XboxDevice;
	
	m_pDeviceStruct->ProductDesc = "Cxbx-Reloaded USB Hub";
	QLIST_INIT(&m_pDeviceStruct->Strings);

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
	
    //set_bit(DEVICE_CATEGORY_BRIDGE, dc->categories);
    //dc->fw_name = "hub";
    //dc->vmsd = &vmstate_usb_hub;
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
    int ep;

    dev->EP_ctl.Num = 0;
    dev->EP_ctl.Type = USB_ENDPOINT_XFER_CONTROL;
    dev->EP_ctl.ifnum = 0;
    dev->EP_ctl.max_packet_size = 64;
    dev->EP_ctl.dev = dev;
    dev->EP_ctl.pipeline = false;
    for (ep = 0; ep < USB_MAX_ENDPOINTS; ep++) {
        dev->ep_in[ep].Num = ep + 1;
        dev->ep_out[ep].Num = ep + 1;
        dev->ep_in[ep].pid = USB_TOKEN_IN;
        dev->ep_out[ep].pid = USB_TOKEN_OUT;
        dev->ep_in[ep].Type = USB_ENDPOINT_XFER_INVALID;
        dev->ep_out[ep].Type = USB_ENDPOINT_XFER_INVALID;
        dev->ep_in[ep].ifnum = USB_INTERFACE_INVALID;
        dev->ep_out[ep].ifnum = USB_INTERFACE_INVALID;
        dev->ep_in[ep].max_packet_size = 0;
        dev->ep_out[ep].max_packet_size = 0;
        dev->ep_in[ep].dev = dev;
        dev->ep_out[ep].dev = dev;
        dev->ep_in[ep].pipeline = false;
        dev->ep_out[ep].pipeline = false;
    }
}
