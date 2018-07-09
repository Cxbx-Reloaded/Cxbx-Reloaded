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
// *   Cxbx->devices->usb->XidGamepad.cpp
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

#include "XidGamepad.h"
#include "USBDevice.h"

#define LOG_STR_GAMEPAD "Gamepad"

#define USB_CLASS_XID  0x58
#define USB_DT_XID     0x42

#define HID_GET_REPORT       0x01
#define HID_SET_REPORT       0x09
#define XID_GET_CAPABILITIES 0x01


#pragma pack(1)

/* Class-specific xid descriptor */
struct XIDDesc {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t bcdXid;
	uint8_t bType;
	uint8_t bSubType;
	uint8_t bMaxInputReportSize;
	uint8_t bMaxOutputReportSize;
	uint16_t wAlternateProductIds[4];
	XIDDesc();
};

/* Struct used by the Get_Report request -> button's state */
struct XIDGamepadReport {
	uint8_t bReportId;
	uint8_t bLength;
	uint16_t wButtons;         // all non-analog buttons
	uint8_t bAnalogButtons[8]; // X, Y, A, B, white, black, left/right trigger
	int16_t sThumbLX;          // analog stick, left X
	int16_t sThumbLY;          // analog stick, left Y
	int16_t sThumbRX;          // analog stick, right X
	int16_t sThumbRY;          // analog stick, right Y
};

/* Struct used by the Set_Report request -> vibration strenght */
struct XIDGamepadOutputReport {
	uint8_t report_id; // From XQEMU: FIXME: is this correct?
	uint8_t length;
	uint16_t left_actuator_strength;  // strenght of left vibration motor
	uint16_t right_actuator_strength; // strenght of right vibration motor
};

#pragma pack()

struct USBXIDState {
	XboxDeviceState dev;                // gamepad device status
	USBEndpoint* intr;                  // interrupt endpoint of the gamepad

	const XIDDesc* xid_desc;            // xid-specific descriptor

	//QEMUPutKbdEntry *kbd_entry;
	bool in_dirty;                      // indicates a change in the button's state
	XIDGamepadReport in_state;          // Get_Report struct
	XIDGamepadOutputReport out_state;   // Ser_Report struct
};

USBDescIface::USBDescIface()
{
	std::memset(this, 0, sizeof(USBDescIface));
	eps = new USBDescEndpoint[2]();
	bInterfaceNumber = 0;
	bNumEndpoints = 2;
	bInterfaceClass = USB_CLASS_XID;
	bInterfaceSubClass = 0x42;
	bInterfaceProtocol = 0x00;
	eps->bEndpointAddress = USB_DIR_IN | 0x02;
	eps->bmAttributes = USB_ENDPOINT_XFER_INT;
	eps->wMaxPacketSize = 0x20;
	eps->bInterval = 4;
	eps++;
	eps->bEndpointAddress = USB_DIR_OUT | 0x02;
	eps->bmAttributes = USB_ENDPOINT_XFER_INT;
	eps->wMaxPacketSize = 0x20;
	eps->bInterval = 4;
}

USBDescIface::~USBDescIface()
{
	delete[] eps;
}

static const USBDescIface desc_iface_xbox_gamepad;

USBDescDevice::USBDescDevice()
{
	std::memset(this, 0, sizeof(USBDescDevice));
	USBDescConfig* pUSBDescConfig = new USBDescConfig();
	bcdUSB = 0x0110;
	bMaxPacketSize0 = 0x40;
	bNumConfigurations = 1;
	pUSBDescConfig->bNumInterfaces = 1;
	pUSBDescConfig->bConfigurationValue = 1;
	pUSBDescConfig->bmAttributes = 0x80;
	pUSBDescConfig->bMaxPower = 50;
	pUSBDescConfig->nif = 1;
	pUSBDescConfig->ifs = &desc_iface_xbox_gamepad;
	confs = pUSBDescConfig;
}

USBDescDevice::~USBDescDevice()
{
	delete confs;
}

static const USBDescDevice desc_device_xbox_gamepad;

USBDesc::USBDesc()
{
	std::memset(this, 0, sizeof(USBDesc));
	id.idVendor = 0x045E;
	id.idProduct = 0x0202;
	id.bcdDevice = 0x0100;
	full = &desc_device_xbox_gamepad;
}

static const USBDesc desc_xbox_gamepad;

XIDDesc::XIDDesc()
{
	bLength = 0x10;
	bDescriptorType = USB_DT_XID;
	bcdXid = 1;
	bType = 1;
	bSubType = 1;
	bMaxInputReportSize = 0x20;
	bMaxOutputReportSize = 0x6;
	wAlternateProductIds[0] = -1;
	wAlternateProductIds[1] = -1;
	wAlternateProductIds[2] = -1;
	wAlternateProductIds[3] = -1;
}

static const XIDDesc desc_xid_xbox_gamepad;

int XidGamepad::Init(int pport)
{
	if (pport > 4 || pport < 1) { return -1; };

	XboxDeviceState* dev = ClassInitFn();
	int rc = UsbXidClaimPort(dev, pport);
	if (rc != 0) {
		return rc;
	}
	m_UsbDev->USB_EpInit(dev);
	m_UsbDev->USB_DeviceInit(dev);
	m_UsbDev->USB_DeviceAttach(dev);

	return 0;
}

XboxDeviceState* XidGamepad::ClassInitFn()
{
	m_pPeripheralFuncStruct = new USBDeviceClass();
	m_XidState = new USBXIDState();
	XboxDeviceState* dev = &m_XidState->dev;

	dev->ProductDesc = "Microsoft Xbox Controller";
	QLIST_INIT(&dev->Strings);
	dev->klass = m_pPeripheralFuncStruct;

	{
		using namespace std::placeholders;

		m_pPeripheralFuncStruct->init           = std::bind(&XidGamepad::UsbXid_Initfn, this, _1);
		m_pPeripheralFuncStruct->handle_reset   = std::bind(&XidGamepad::UsbXid_HandleReset, this);
		m_pPeripheralFuncStruct->handle_control = std::bind(&XidGamepad::UsbXid_HandleControl, this, _1, _2, _3, _4, _5, _6, _7);
		m_pPeripheralFuncStruct->handle_data    = std::bind(&XidGamepad::UsbXid_HandleData, this, _1, _2);
		m_pPeripheralFuncStruct->handle_destroy = std::bind(&XidGamepad::UsbXid_HandleDestroy, this);
		m_pPeripheralFuncStruct->handle_attach  = std::bind(&XidGamepad::UsbXid_Attach, this, _1);
		m_pPeripheralFuncStruct->product_desc   = dev->ProductDesc.c_str();
		m_pPeripheralFuncStruct->usb_desc       = &desc_xbox_gamepad;
	}

	return dev;
}

int XidGamepad::UsbXidClaimPort(XboxDeviceState* dev, int pport)
{
	int i;
	int usb_port;
	int port_offset;
	std::vector<USBPort*>::iterator it;

	assert(dev->Port == nullptr);

	usb_port = PlayerToUsbArray[pport];
	for (int j = 0; j < 4; j++) {
		if (g_HubObjArray[j]) {
			i = 0;
			for (auto port : g_HubObjArray[j]->m_UsbDev->m_FreePorts) {
				if (strcmp(port->Path, (std::to_string(usb_port) + ".2").c_str()) == 0) {
					m_UsbDev = g_HubObjArray[j]->m_UsbDev;
					break;
				}
				i++;
			}
		}
	}
	if (m_UsbDev == nullptr) {
		EmuWarning("Port requested %d.%d not found (in use?)", usb_port, 2);
		return -1;
	}
	it = m_UsbDev->m_FreePorts.begin() + i;
	dev->Port = *it;
	(*it)->Dev = dev;
	m_UsbDev->m_FreePorts.erase(it);

	return 0;
}

void XidGamepad::UsbXidReleasePort(XboxDeviceState* dev)
{
	USBPort* port = dev->Port;

	assert(port != nullptr);

	port->Dev = nullptr;
	dev->Port = nullptr;
}

int XidGamepad::UsbXid_Initfn(XboxDeviceState* dev)
{
	m_UsbDev->USBDesc_Init(dev);
	m_XidState->intr = m_UsbDev->USB_GetEP(dev, USB_TOKEN_IN, 2);

	m_XidState->in_state.bLength = sizeof(m_XidState->in_state);
	m_XidState->out_state.length = sizeof(m_XidState->out_state);
	//m_XidState->kbd_entry = qemu_add_kbd_event_handler(xbox_gamepad_keyboard_event, m_XidState); TODO
	m_XidState->xid_desc = &desc_xid_xbox_gamepad;

	return 0;
}

void XidGamepad::UsbXid_HandleDestroy()
{
	UsbXidReleasePort(&m_XidState->dev);
	XidCleanUp();
}

void XidGamepad::UsbXid_Attach(XboxDeviceState* dev)
{
	if ((dev->Port->SpeedMask & USB_SPEED_MASK_FULL)) {
		dev->Speed = USB_SPEED_FULL;
	}
	else {
		return;
	}
	m_UsbDev->USBDesc_SetDefaults(dev);
}

void XidGamepad::UsbXid_HandleReset()
{
	DbgPrintf("%s reset event\n", LOG_STR_GAMEPAD);
}

void XidGamepad::UsbXid_HandleControl(XboxDeviceState* dev, USBPacket* p,
	int request, int value, int index, int length, uint8_t* data)
{
	int ret = m_UsbDev->USBDesc_HandleControl(dev, p, request, value, index, length, data);
	if (ret >= 0) {
		DbgPrintf("%s handled by USBDesc_HandleControl, ret is %d\n", LOG_STR_GAMEPAD, ret);
		return;
	}

	switch (request) {
		// HID-specific requests
		case ClassInterfaceRequest | HID_GET_REPORT: {
			// From the HID standard: "The Get_Report request allows the host to receive a report via the Control pipe.
			// The wValue field specifies the Report Type in the high byte and the Report ID in the low byte. Set Report ID
			// to 0 (zero) if Report IDs are not used. 01 = input, 02 = output, 03 = feature, 04-FF = reserved"
			DbgPrintf("%s GET_REPORT 0x%X\n", LOG_STR_GAMEPAD, value);
			if (value == 0x100) {
				assert(m_XidState->in_state.bLength <= length);
				// m_XidState->in_state.bReportId++; /* FIXME: I'm not sure if bReportId is just a counter */
				std::memcpy(data, &m_XidState->in_state, m_XidState->in_state.bLength);
				p->ActualLength = m_XidState->in_state.bLength;
			}
			else {
				assert(0);
			}
			break;
		}

		case ClassInterfaceOutRequest | HID_SET_REPORT: {
			// From the HID standard: "The Set_Report request allows the host to send a report to the device, possibly
			// setting the state of input, output, or feature controls. The meaning of the request fields for the Set_Report
			// request is the same as for the Get_Report request, however the data direction is reversed and the Report
			// Data is sent from host to device."
			DbgPrintf("%s SET_REPORT 0x%X\n", LOG_STR_GAMEPAD, value);
			if (value == 0x200) {
				// Read length, then the entire packet
				std::memcpy(&m_XidState->out_state, data, sizeof(m_XidState->out_state));
				assert(m_XidState->out_state.length == sizeof(m_XidState->out_state));
				assert(m_XidState->out_state.length <= length);
				//FIXME: Check actuator endianess
				DbgPrintf("Set rumble power to left: 0x%X and right: 0x%X\n",
					m_XidState->out_state.left_actuator_strength,
					m_XidState->out_state.right_actuator_strength);
				p->ActualLength = m_XidState->out_state.length;
			}
			else {
				assert(0);
			}
			break;
		}

		// XID-specific requests
		case VendorInterfaceRequest | USB_REQ_GET_DESCRIPTOR: {
			DbgPrintf("%s GET_DESCRIPTOR 0x%x\n", LOG_STR_GAMEPAD, value);
			if (value == 0x4200) {
				assert(m_XidState->xid_desc->bLength <= length);
				std::memcpy(data, m_XidState->xid_desc, m_XidState->xid_desc->bLength);
				p->ActualLength = m_XidState->xid_desc->bLength;
			}
			else {
				assert(0);
			}
			break;
		}

		case VendorInterfaceRequest | XID_GET_CAPABILITIES: {
			DbgPrintf("%s XID_GET_CAPABILITIES 0x%x\n", LOG_STR_GAMEPAD, value);
			/* FIXME: ! */
			p->Status = USB_RET_STALL;
			//assert(false);
			break;
		}

		case ((USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_DEVICE) << 8) | USB_REQ_GET_DESCRIPTOR: {
			/* FIXME: ! */
			DbgPrintf("%s unknown xpad request 0x%X: value = 0x%X\n", LOG_STR_GAMEPAD, request, value);
			std::memset(data, 0x00, length);
			//FIXME: Intended for the hub: usbd_get_hub_descriptor, UT_READ_CLASS?!
			p->Status = USB_RET_STALL;
			//assert(false);
			break;
		}

		case ((USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_ENDPOINT) << 8) | USB_REQ_CLEAR_FEATURE: {
			/* FIXME: ! */
			DbgPrintf("%s unknown xpad request 0x%X: value = 0x%X\n", LOG_STR_GAMEPAD, request, value);
			std::memset(data, 0x00, length);
			p->Status = USB_RET_STALL;
			break;
		}

		default:
			DbgPrintf("%s USB stalled on request 0x%X value 0x%X\n", LOG_STR_GAMEPAD, request, value);
			p->Status = USB_RET_STALL;
			assert(0);
			break;
	}
}

void XidGamepad::UsbXid_HandleData(XboxDeviceState* dev, USBPacket* p)
{
	switch (p->Pid) {
	case USB_TOKEN_IN: {
		if (p->Endpoint->Num == 2) {
			if (m_XidState->in_dirty) {
				m_UsbDev->USB_PacketCopy(p, &m_XidState->in_state, m_XidState->in_state.bLength);
				m_XidState->in_dirty = false;
			}
			else {
				p->Status = USB_RET_NAK;
			}
		}
		else {
			assert(0);
		}
		break;
	}

	case USB_TOKEN_OUT: {
		p->Status = USB_RET_STALL;
		break;
	}

	default:
		p->Status = USB_RET_STALL;
		assert(0);
		break;
	}
}

void XidGamepad::XidCleanUp()
{
	delete m_pPeripheralFuncStruct;
	delete m_XidState;
	m_pPeripheralFuncStruct = nullptr;
	m_XidState = nullptr;
}
