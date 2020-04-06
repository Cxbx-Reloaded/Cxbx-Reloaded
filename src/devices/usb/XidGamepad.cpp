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

// Acknowledgment: XQEMU xid emulation (GPLv2)
// https://xqemu.com/

/*
* QEMU USB XID Devices
*
* Copyright (c) 2013 espes
* Copyright (c) 2017 Jannik Vogel
* Copyright (c) 2018 Matt Borgerson
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, see <https://www.gnu.org/licenses/>.
*/


#define LOG_PREFIX CXBXR_MODULE::XIDCTRL

// prevent name collisions
namespace xboxkrnl
{
	#include <xboxkrnl/xboxkrnl.h> // For PKINTERRUPT, etc.
};

#include <cstring>
#include "XidGamepad.h"
#include "USBDevice.h"
#include "common\input\InputManager.h"
#include "common\input\SdlJoystick.h"
#include "OHCI.h"
#include "core\kernel\exports\EmuKrnl.h"
#include "Logging.h"

#define USB_CLASS_XID  0x58
#define USB_DT_XID     0x42

#define HID_GET_REPORT       0x01
#define HID_SET_REPORT       0x09
#define XID_GET_CAPABILITIES 0x01


// To avoid including Xbox.h
extern USBDevice* g_USB0;

XidGamepad* g_XidControllerObjArray[4];

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

	XIDGamepadReport in_state;                      // Get_Report struct
	XIDGamepadReport in_state_capabilities;         // Get_Capabilities struct (in)
	XIDGamepadOutputReport out_state;               // Set_Report struct
	XIDGamepadOutputReport out_state_capabilities;  // Get_Capabilities struct (out)
};

static const USBDescEndpoint desc_endp_xbox_gamepad[2] = {
	{
		USB_DIR_IN | 0x02,       // bEndpointAddress;
		USB_ENDPOINT_XFER_INT,   // bmAttributes;
		0x20,                    // wMaxPacketSize;
		4,                       // bInterval;
		0,                       // bRefresh;
		0,                       // bSynchAddress
		0,                       // is_audio
		nullptr                  // extra
	},
	{
		USB_DIR_OUT | 0x02,
		USB_ENDPOINT_XFER_INT,
		0x20,
		4,
		0,
		0,
		0,
		nullptr
	}
};

static const USBDescIface desc_iface_xbox_gamepad = {
	0,                // bInterfaceNumber;
	0,                // bAlternateSetting;
	2,                // bNumEndpoints;
	USB_CLASS_XID,    // bInterfaceClass;
	0x42,             // bInterfaceSubClass
	0x00,             // bInterfaceProtocol
	0,                // iInterface
	0,                // ndesc
	nullptr,          // descs
	desc_endp_xbox_gamepad
};

static const USBDescConfig desc_config_xbox_gamepad = {
	1,     // bNumInterfaces
	1,     // bConfigurationValue
	0,     // iConfiguration
	0x80,  // bmAttributes
	50,    // bMaxPower
	1,     // nif
	&desc_iface_xbox_gamepad
};

static const USBDescDevice desc_device_xbox_gamepad = {
	0x0110,   // bcdUSB
	0,        // bDeviceClass
	0,        // bDeviceSubClass
	0,        // bDeviceProtocol
	0x40,     // bMaxPacketSize0
	1,        // bNumConfigurations
	&desc_config_xbox_gamepad
};

static const USBDesc desc_xbox_gamepad = {
	{
		0x045E,            // idVendor
		0x0202,            // idProduct
		0x0100,            // bcdDevice
		STR_MANUFACTURER,  // iManufacturer
		STR_PRODUCT,       // iProduct
		STR_SERIALNUMBER   // iSerialNumber
	},
	&desc_device_xbox_gamepad
};

static const XIDDesc desc_xid_xbox_gamepad = {
	0x10,        // bLength
	USB_DT_XID,  // bDescriptorType
	0x100,       // bcdXid
	1,           // bType
	1,           // bSubType
	20,          // bMaxInputReportSize
	6,           // bMaxOutputReportSize
	{ 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF }  // wAlternateProductIds
};

int XidGamepad::Init(int port)
{
	if (port > 4 || port < 1) { return -1; }

	XboxDeviceState* dev = ClassInitFn();
	int rc = UsbXidClaimPort(dev, port);
	if (rc != 0) {
		m_UsbDev->m_HostController->m_FrameTimeMutex.unlock();
		return rc;
	}
	m_UsbDev->USB_EpInit(dev);
	m_UsbDev->USB_DeviceInit(dev);
	m_UsbDev->USB_DeviceAttach(dev);

	m_UsbDev->m_HostController->m_FrameTimeMutex.unlock();

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

int XidGamepad::UsbXidClaimPort(XboxDeviceState* dev, int port)
{
	int i;
	std::vector<USBPort*>::iterator it;

	assert(dev->Port == nullptr);

	m_UsbDev = g_USB0;
	it = m_UsbDev->m_FreePorts.end();
	i = 0;

	m_UsbDev->m_HostController->m_FrameTimeMutex.lock();

	for (auto usb_port : m_UsbDev->m_FreePorts) {
		if (usb_port->Path == (std::to_string(port) + ".2")) {
			it = m_UsbDev->m_FreePorts.begin() + i;
			break;
		}
		i++;
	}
	if (it == m_UsbDev->m_FreePorts.end()) {
		EmuLog(LOG_LEVEL::WARNING, "Port requested %d.2 not found (in use?)", port);
		return -1;
	}

	m_Port = port;
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
	m_UsbDev->USB_CreateSerial(dev, std::string("1"));
	m_UsbDev->USBDesc_SetString(dev, STR_MANUFACTURER, std::string("Cxbx-Reloaded"));
	m_UsbDev->USBDesc_SetString(dev, STR_PRODUCT, std::string("Microsoft Gamepad"));
	m_UsbDev->USBDesc_Init(dev);
	m_XidState->intr = m_UsbDev->USB_GetEP(dev, USB_TOKEN_IN, 2);

	m_XidState->in_state.bLength = sizeof(m_XidState->in_state);
	m_XidState->in_state.bReportId = 0;
	m_XidState->out_state.length = sizeof(m_XidState->out_state);
	m_XidState->out_state.report_id = 0;

	std::memset(&m_XidState->in_state_capabilities, 0xFF, sizeof(m_XidState->in_state_capabilities));
	m_XidState->in_state_capabilities.bLength = sizeof(m_XidState->in_state_capabilities);
	m_XidState->in_state_capabilities.bReportId = 0;

	std::memset(&m_XidState->out_state_capabilities, 0xFF, sizeof(m_XidState->out_state_capabilities));
	m_XidState->out_state_capabilities.length = sizeof(m_XidState->out_state_capabilities);
	m_XidState->out_state_capabilities.report_id = 0;

	m_XidState->xid_desc = &desc_xid_xbox_gamepad;

	return 0;
}

void XidGamepad::UsbXid_HandleDestroy()
{
	UsbXidReleasePort(&m_XidState->dev);
	XpadCleanUp();
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
	EmuLog(LOG_LEVEL::DEBUG, "Reset event");
}

void XidGamepad::UsbXid_HandleControl(XboxDeviceState* dev, USBPacket* p,
	int request, int value, int index, int length, uint8_t* data)
{
	int ret = m_UsbDev->USBDesc_HandleControl(dev, p, request, value, index, length, data);
	if (ret >= 0) {
		EmuLog(LOG_LEVEL::DEBUG, "Handled by USBDesc_HandleControl, ret is %d", ret);
		return;
	}

	switch (request) {
		// HID-specific requests
		case ClassInterfaceRequest | HID_GET_REPORT: {
			// From the HID standard: "The Get_Report request allows the host to receive a report via the Control pipe.
			// The wValue field specifies the Report Type in the high byte and the Report ID in the low byte. Set Report ID
			// to 0 (zero) if Report IDs are not used. 01 = input, 02 = output, 03 = feature, 04-FF = reserved"
			EmuLog(LOG_LEVEL::DEBUG, "GET_REPORT xpad request 0x%X", value);
			// JayFoxRox's analysis: "This 0x0100 case is for input.
			// This is the case where the Xbox wants to read input data from the controller.
			// Confirmed with a real Duke controller :
			// If the buffer provided by the Xbox is too small, the controller will cut the transfer when the buffer is full (actual_length is patched).
			// If the buffer is too large the controller will STALL instead.
			// If the buffer has the correct length the full input data is transferred."
			if (value == 0x0100) {
				if (length <= m_XidState->in_state.bLength) {
#if 0 // Reenable this when LLE USB actually works
					SDL2Devices* controller = g_InputDeviceManager->FindDeviceFromXboxPort(m_Port);
					if (controller != nullptr) {
						controller->ReadButtonState(&m_XidState->in_state.wButtons, m_XidState->in_state.bAnalogButtons,
							&m_XidState->in_state.sThumbLX, &m_XidState->in_state.sThumbLY, &m_XidState->in_state.sThumbRX,
							&m_XidState->in_state.sThumbRY);
					}
					else {
						// ergo720: this shouldn't really happen. If it does, it either means that m_Port is wrong or there's a bug
						// in the InputDeviceManager
						p->Status = USB_RET_STALL;
						assert(0);
					}
#endif
					std::memcpy(data, &m_XidState->in_state, m_XidState->in_state.bLength);
					p->ActualLength = length;
				}
				else {
					p->Status = USB_RET_STALL;
				}
			}
			else {
				p->Status = USB_RET_STALL;
			}
			break;
		}

		case ClassInterfaceOutRequest | HID_SET_REPORT: {
			// From the HID standard: "The Set_Report request allows the host to send a report to the device, possibly
			// setting the state of input, output, or feature controls. The meaning of the request fields for the Set_Report
			// request is the same as for the Get_Report request, however the data direction is reversed and the Report
			// Data is sent from host to device."
			EmuLog(LOG_LEVEL::DEBUG, "SET_REPORT xpad request 0x%X", value);
			// JayFoxRox's analysis: "The 0x0200 case below is for output.
			// This is the case where the Xbox wants to write rumble data to the controller.
			// To my knowledge :
			// If the buffer provided by the Xbox is too small the transfer will STALL.
			// If the buffer is too large the transfer will STALL.
			// If the buffer has the correct length the full output data is transferred."
			if (value == 0x0200) {
				if (length == m_XidState->out_state.length) {
					// Read length, then the entire packet
					std::memcpy(&m_XidState->out_state, data, sizeof(m_XidState->out_state));
					/* FIXME: This should also be a STALL */
					assert(m_XidState->out_state.length == sizeof(m_XidState->out_state));

					p->ActualLength = length;
				}
				else {
					p->Status = USB_RET_STALL;
				}
				UpdateForceFeedback();
			}
			else {
				p->Status = USB_RET_STALL;
				assert(0);
			}
			break;
		}

		// XID-specific requests
		case VendorInterfaceRequest | USB_REQ_GET_DESCRIPTOR: {
			EmuLog(LOG_LEVEL::DEBUG, "GET_DESCRIPTOR xpad request 0x%x", value);
			if (value == 0x4200) {
				assert(m_XidState->xid_desc->bLength <= length);
				std::memcpy(data, m_XidState->xid_desc, m_XidState->xid_desc->bLength);
				p->ActualLength = m_XidState->xid_desc->bLength;
			}
			else {
				p->Status = USB_RET_STALL;
				assert(0);
			}
			break;
		}

		case VendorInterfaceRequest | XID_GET_CAPABILITIES: {
			EmuLog(LOG_LEVEL::DEBUG, "XID_GET_CAPABILITIES xpad request 0x%x", value);
			if (value == 0x0100) {
				if (length > m_XidState->in_state_capabilities.bLength) {
					length = m_XidState->in_state_capabilities.bLength;
				}
				std::memcpy(data, &m_XidState->in_state_capabilities, length);
				p->ActualLength = length;
			}
			else if (value == 0x0200) {
				if (length > m_XidState->out_state_capabilities.length) {
					length = m_XidState->out_state_capabilities.length;
				}
				std::memcpy(data, &m_XidState->out_state_capabilities, length);
				p->ActualLength = length;
			}
			else {
				p->Status = USB_RET_STALL;
				assert(0);
			}
			break;
		}

		case ((USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_DEVICE) << 8) | USB_REQ_GET_DESCRIPTOR: {
			/* FIXME: ! */
			EmuLog(LOG_LEVEL::DEBUG, "Unknown xpad request 0x%X: value = 0x%X", request, value);
			std::memset(data, 0x00, length);
			//FIXME: Intended for the hub: usbd_get_hub_descriptor, UT_READ_CLASS?!
			p->Status = USB_RET_STALL;
			//assert(false);
			break;
		}

		case ((USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_ENDPOINT) << 8) | USB_REQ_CLEAR_FEATURE: {
			/* FIXME: ! */
			EmuLog(LOG_LEVEL::DEBUG, "Unknown xpad request 0x%X: value = 0x%X", request, value);
			std::memset(data, 0x00, length);
			p->Status = USB_RET_STALL;
			break;
		}

		default:
			EmuLog(LOG_LEVEL::DEBUG, "USB stalled on request 0x%X value 0x%X", request, value);
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
#if 0 // Reenable this when LLE USB actually works
			SDL2Devices* controller = g_InputDeviceManager->FindDeviceFromXboxPort(m_Port);
			if (controller != nullptr) {
				bool ret;
				ret = controller->ReadButtonState(&m_XidState->in_state.wButtons, m_XidState->in_state.bAnalogButtons,
					&m_XidState->in_state.sThumbLX, &m_XidState->in_state.sThumbLY, &m_XidState->in_state.sThumbRX,
					&m_XidState->in_state.sThumbRY);
				if (ret) {
					m_UsbDev->USB_PacketCopy(p, &m_XidState->in_state, m_XidState->in_state.bLength);
				}
				else {
#endif
					p->Status = USB_RET_NAK;
				}
#if 0 // Reenable this when LLE USB actually works
			}
			else {
				p->Status = USB_RET_STALL;
				assert(0);
			}
		}
		else {
			assert(0);
		}
#endif
		break;
	}

	case USB_TOKEN_OUT: {
		if (p->Endpoint->Num == 2) {
			m_UsbDev->USB_PacketCopy(p, &m_XidState->out_state, m_XidState->out_state.length);
			UpdateForceFeedback();
		}
		else {
			assert(0);
		}
		break;
	}

	default:
		p->Status = USB_RET_STALL;
		assert(0);
		break;
	}
}

void XidGamepad::XpadCleanUp()
{
	delete m_pPeripheralFuncStruct;
	delete m_XidState;
	m_pPeripheralFuncStruct = nullptr;
	m_XidState = nullptr;
}

void XidGamepad::UpdateForceFeedback()
{
	// JayFoxRox's remarks: "Xbox -> XID packets were not tested
	// The handling of output packets / force feedback was not checked."
	// For the above reason we don't implement vibration support for now since the current
	// implementation is untested and could potentially contain errors

	/* FIXME: Check actuator endianess */
	EmuLog(LOG_LEVEL::DEBUG, "Set rumble power to left: 0x%X and right: 0x%X",
		m_XidState->out_state.left_actuator_strength,
		m_XidState->out_state.right_actuator_strength);
}
