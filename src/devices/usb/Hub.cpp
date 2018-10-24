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

// Acknowledgment: XQEMU (GPLv2)
// https://xqemu.com/

#define _XBOXKRNL_DEFEXTRN_

#define LOG_PREFIX CXBXR_MODULE::HUB

// prevent name collisions
namespace xboxkrnl
{
	#include <xboxkrnl/xboxkrnl.h> // For PKINTERRUPT, etc.
};

#include "OHCI.h"
#include "Hub.h"
#include "CxbxKrnl\EmuKrnl.h"  // For EmuLog
#include "Logging.h"

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

#define ClearHubFeature		(0x2000 | USB_REQ_CLEAR_FEATURE)
#define ClearPortFeature	(0x2300 | USB_REQ_CLEAR_FEATURE)
#define GetHubDescriptor	(0xA000 | USB_REQ_GET_DESCRIPTOR)
#define GetHubStatus		(0xA000 | USB_REQ_GET_STATUS)
#define GetPortStatus		(0xA300 | USB_REQ_GET_STATUS)
#define SetHubFeature		(0x2000 | USB_REQ_SET_FEATURE)
#define SetPortFeature		(0x2300 | USB_REQ_SET_FEATURE)

#define PORT_CONNECTION	    0
#define PORT_ENABLE	        1
#define PORT_SUSPEND        2
#define PORT_OVERCURRENT    3
#define PORT_RESET          4
#define PORT_POWER          8
#define PORT_LOWSPEED       9
#define PORT_C_CONNECTION   16
#define PORT_C_ENABLE       17
#define PORT_C_SUSPEND      18
#define PORT_C_OVERCURRENT	19
#define PORT_C_RESET		20


// To avoid including Xbox.h
extern USBDevice* g_USB0;

Hub* g_HubObjArray[4] = { nullptr };


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

static const USBDescEndpoint desc_endp_hub = {
	USB_DIR_IN | 0x01,        // bEndpointAddress;
	USB_ENDPOINT_XFER_INT,    // bmAttributes;
	1 + (NUM_PORTS + 7) / 8,  // wMaxPacketSize;
	0xFF,                     // bInterval;
	0,                        // bRefresh;
	0,                        // bSynchAddress
	0,                        // is_audio
	nullptr                   // extra
};

static const USBDescIface desc_iface_hub = {
	0,                // bInterfaceNumber;
	0,                // bAlternateSetting;
	1,                // bNumEndpoints;
	USB_CLASS_HUB,    // bInterfaceClass;
	0,                // bInterfaceSubClass
	0,                // bInterfaceProtocol
	0,                // iInterface
	0,                // ndesc
	nullptr,          // descs
	&desc_endp_hub
};

static const USBDescConfig desc_config_hub = {
	1,     // bNumInterfaces
	1,     // bConfigurationValue
	0,     // iConfiguration
	0xE0,  // bmAttributes
	0,     // bMaxPower
	1,     // nif
	&desc_iface_hub
};

static const USBDescDevice desc_device_hub = {
	0x0110,         // bcdUSB
	USB_CLASS_HUB,  // bDeviceClass
	0,              // bDeviceSubClass
	0,              // bDeviceProtocol
	8,              // bMaxPacketSize0
	1,              // bNumConfigurations
	&desc_config_hub
};

static const USBDesc desc_hub = {
	{
		0x0409,            // idVendor
		0x55AA,            // idProduct
		0x0101,            // bcdDevice
		STR_MANUFACTURER,  // iManufacturer
		STR_PRODUCT,       // iProduct
		STR_SERIALNUMBER   // iSerialNumber
	},
	&desc_device_hub
};

// Class-specific hub descriptor. Remember to update DeviceRemovable and PortPwrCtrlMask if you change NUM_PORTS since their values depend on
// the number of downstream ports available on the hub! Also note that this descriptor cannot be put in the descs member of the interface descriptor
// because then this descriptor will be retrieved during a standard GetDescriptor request instead of the hub-specific GetHubDescriptor request
static const uint8_t HubDescriptor[] =
{
	0x0A,			//  u8   bDescLength; 10 bytes
	0x29,			//  u8   bDescriptorType; Hub-descriptor
	NUM_PORTS,		//  u8   bNbrPorts; 0x08
	0x0a,			//  u16  wHubCharacteristics; (individual port over-current protection, no power switching)
	0x00,
	0x01,			//  u8   bPwrOn2pwrGood; 2 ms
	0x00,			//  u8   bHubContrCurrent; 0 mA
	0x00,           //  u16  DeviceRemovable; all devices are removable
	0x00,
	0xFF,           //  u8  PortPwrCtrlMask; all 1's for compatibility reasons
};

int Hub::Init(int port)
{
	if (port > 4 || port < 1) { return -1; };

	XboxDeviceState* dev = ClassInitFn();
    int rc = UsbHubClaimPort(dev, port);
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

XboxDeviceState* Hub::ClassInitFn()
{
	m_pPeripheralFuncStruct = new USBDeviceClass();
	m_HubState = new USBHubState();
	XboxDeviceState* dev = &m_HubState->dev;

	dev->ProductDesc = "Cxbx-Reloaded USB Hub";
	QLIST_INIT(&dev->Strings);
	dev->klass = m_pPeripheralFuncStruct;

	{
		using namespace std::placeholders;

		m_pPeripheralFuncStruct->init           = std::bind(&Hub::UsbHub_Initfn, this, _1);
		m_pPeripheralFuncStruct->find_device    = std::bind(&Hub::UsbHub_FindDevice, this, _1, _2);
		m_pPeripheralFuncStruct->handle_reset   = std::bind(&Hub::UsbHub_HandleReset, this);
		m_pPeripheralFuncStruct->handle_control = std::bind(&Hub::UsbHub_HandleControl, this, _1, _2, _3, _4, _5, _6, _7);
		m_pPeripheralFuncStruct->handle_data    = std::bind(&Hub::UsbHub_HandleData, this, _1, _2);
		m_pPeripheralFuncStruct->handle_destroy = std::bind(&Hub::UsbHub_HandleDestroy, this);
		m_pPeripheralFuncStruct->product_desc   = dev->ProductDesc.c_str();
		m_pPeripheralFuncStruct->usb_desc       = &desc_hub;
	}

	return dev;
}

int Hub::UsbHubClaimPort(XboxDeviceState* dev, int port)
{
	int i;
	std::vector<USBPort*>::iterator it;

	assert(dev->Port == nullptr);

	m_UsbDev = g_USB0;
	it = m_UsbDev->m_FreePorts.end();
	i = 0;

	m_UsbDev->m_HostController->m_FrameTimeMutex.lock();

	for (auto usb_port : m_UsbDev->m_FreePorts) {
		if (usb_port->Path == std::to_string(port)) {
			it = m_UsbDev->m_FreePorts.begin() + i;
			break;
		}
		i++;
	}
	if (it == m_UsbDev->m_FreePorts.end()) {
		EmuLog(LOG_LEVEL::WARNING, "Port requested %d not found (in use?)", port);
		return -1;
	}
	dev->Port = *it;
	(*it)->Dev = dev;
	m_UsbDev->m_FreePorts.erase(it);

	return 0;
}

void Hub::UsbHubReleasePort(XboxDeviceState* dev)
{
	USBPort* port = dev->Port;

	assert(port != nullptr);

	port->Dev = nullptr;
	dev->Port = nullptr;
}

int Hub::UsbHub_Initfn(XboxDeviceState* dev)
{
	USBHubPort* port;
	USBPortOps* ops;
	int i;

	m_UsbDev->USB_CreateSerial(dev, std::string("314159"));
	m_UsbDev->USBDesc_SetString(dev, STR_MANUFACTURER, std::string("Cxbx-Reloaded"));
	m_UsbDev->USBDesc_SetString(dev, STR_PRODUCT, std::string("Cxbx-Reloaded USB Hub"));
	m_UsbDev->USBDesc_Init(dev);
	m_HubState->intr = m_UsbDev->USB_GetEP(dev, USB_TOKEN_IN, 1);

	ops = new USBPortOps();
	{
		using namespace std::placeholders;

		ops->attach       = std::bind(&Hub::UsbHub_Attach, this, _1);
		ops->detach       = std::bind(&Hub::UsbHub_Detach, this, _1);
		ops->child_detach = std::bind(&Hub::UsbHub_ChildDetach, this, _1);
		ops->wakeup       = std::bind(&Hub::UsbHub_Wakeup, this, _1);
		ops->complete     = std::bind(&Hub::UsbHub_Complete, this, _1, _2);
	}

	for (i = 0; i < NUM_PORTS; i++) {
		port = &m_HubState->ports[i];
		m_UsbDev->USB_RegisterPort(&port->port, i, USB_SPEED_MASK_LOW | USB_SPEED_MASK_FULL, ops);
		m_UsbDev->USB_PortLocation(&port->port, dev->Port, i + 1);
	}
	UsbHub_HandleReset();
	return 0;
}

XboxDeviceState* Hub::UsbHub_FindDevice(XboxDeviceState* dev, uint8_t addr)
{
	USBHubPort* port;
	XboxDeviceState* downstream;

	for (int i = 0; i < NUM_PORTS; i++) {
		port = &m_HubState->ports[i];
		if (!(port->wPortStatus & PORT_STAT_ENABLE)) {
			continue;
		}
		downstream = m_UsbDev->USB_FindDevice(&port->port, addr);
		if (downstream != nullptr) {
			return downstream;
		}
	}
	return nullptr;
}

void Hub::UsbHub_HandleReset()
{
	USBHubPort* port;

	for (int i = 0; i < NUM_PORTS; i++) {
		port = m_HubState->ports + i;
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

void Hub::UsbHub_HandleControl(XboxDeviceState* dev, USBPacket* p,
	int request, int value, int index, int length, uint8_t* data)
{
	int ret;

	ret = m_UsbDev->USBDesc_HandleControl(dev, p, request, value, index, length, data);
	if (ret >= 0) {
		return;
	}

	switch (request) {
		case EndpointOutRequest | USB_REQ_CLEAR_FEATURE: {
			// clear ep halt and bEndpointAddress of hub is 0x81
			if (value == 0 && index != 0x81) {
				goto fail;
			}
			break;
		}

		case GetHubStatus: {
			// From the standard: "This request returns the current hub status and the states that have changed since the previous acknowledgment.
			// The first word of data contains wHubStatus. The second word of data contains wHubChange"
			// We always report that the local power supply is good and that currently there is no over-power condition
			data[0] = 0;
			data[1] = 0;
			data[2] = 0;
			data[3] = 0;
			p->ActualLength = 4;
			break;
		}

		case GetPortStatus: {
			// From the standard: "This request returns the current port status and the current value of the port status change bits.
			// The first word of data contains wPortStatus. The second word of data contains wPortChange"
			unsigned int n = index - 1;
			USBHubPort* port;
			if (n >= NUM_PORTS) {
				goto fail;
			}
			port = &m_HubState->ports[n];
			DBG_PRINTF("%s GetPortStatus -> Address 0x%X, wIndex %d, wPortStatus %d, wPortChange %d\n",
				__func__, m_HubState->dev.Addr, index, port->wPortStatus, port->wPortChange);
			data[0] = port->wPortStatus;
			data[1] = port->wPortStatus >> 8;
			data[2] = port->wPortChange;
			data[3] = port->wPortChange >> 8;
			p->ActualLength = 4;
			break;
		}

		case SetHubFeature:
		case ClearHubFeature: {
			if (value != 0 && value != 1) {
				goto fail;
			}
			break;
		}

		case SetPortFeature: {
			// From the standard: "This request sets a value reported in the port status. Features that can be set with this request are PORT_RESET,
			// PORT_SUSPEND and PORT_POWER; others features are not required to be set by this request"
			unsigned int n = index - 1;
			USBHubPort* port;
			XboxDeviceState* dev;

			DBG_PRINTF("%s SetPortFeature -> Address 0x%X, wIndex %d, Feature %s\n",
				__func__, m_HubState->dev.Addr, index, GetFeatureName(value));

			if (n >= NUM_PORTS) {
				goto fail;
			}
			port = &m_HubState->ports[n];
			dev = port->port.Dev;
			switch (value) {
				case PORT_SUSPEND: {
					port->wPortStatus |= PORT_STAT_SUSPEND;
					break;
				}

				case PORT_RESET: {
					if (dev && dev->Attached) {
						m_UsbDev->USB_DeviceReset(dev);
						port->wPortChange |= PORT_STAT_C_RESET;
						port->wPortStatus |= PORT_STAT_ENABLE;
						m_UsbDev->USB_Wakeup(m_HubState->intr);
					}
					break;
				}

				case PORT_POWER:
					break;

				default:
					goto fail;
			}
			break;
		}

		case ClearPortFeature: {
			// From the standard: "This request resets a value reported in the port status"
			unsigned int n = index - 1;
			USBHubPort *port;

			DBG_PRINTF("%s ClearPortFeature -> Address 0x%X, wIndex %d, Feature %s\n",
				__func__, m_HubState->dev.Addr, index, GetFeatureName(value));

			if (n >= NUM_PORTS) {
				goto fail;
			}
			port = &m_HubState->ports[n];
			switch (value) {
				case PORT_ENABLE: {
					port->wPortStatus &= ~PORT_STAT_ENABLE;
					break;
				}

				case PORT_C_ENABLE: {
					port->wPortChange &= ~PORT_STAT_C_ENABLE;
					break;
				}

				case PORT_SUSPEND: {
					port->wPortStatus &= ~PORT_STAT_SUSPEND;
					break;
				}

				case PORT_C_SUSPEND: {
					port->wPortChange &= ~PORT_STAT_C_SUSPEND;
					break;
				}

				case PORT_C_CONNECTION: {
					port->wPortChange &= ~PORT_STAT_C_CONNECTION;
					break;
				}

				case PORT_C_OVERCURRENT: {
					port->wPortChange &= ~PORT_STAT_C_OVERCURRENT;
					break;
				}

				case PORT_C_RESET: {
					port->wPortChange &= ~PORT_STAT_C_RESET;
					break;
				}

				default:
					goto fail;
			}
			break;
		}

		case GetHubDescriptor: {
			std::memcpy(data, HubDescriptor, sizeof(HubDescriptor));
			p->ActualLength = sizeof(HubDescriptor);
			break;
		}

		default:
		fail:
			p->Status = USB_RET_STALL;
			break;
	}
}

void Hub::UsbHub_HandleData(XboxDeviceState* dev, USBPacket* p)
{
	switch (p->Pid) {
		case USB_TOKEN_IN: {
			if (p->Endpoint->Num == 1) {
				USBHubPort* port;
				unsigned int status;
				uint8_t buf[4];
				int i, n;
				status = 0;
				for (i = 0; i < NUM_PORTS; i++) {
					port = &m_HubState->ports[i];
					if (port->wPortChange) {
						status |= (1 << (i + 1));
					}
				}
				if (status != 0) {
					n = (NUM_PORTS + 1 + 7) / 8;
					if (p->IoVec.Size == 1) { // FreeBSD workaround
						n = 1;
					}
					else if (n > p->IoVec.Size) {
						p->Status = USB_RET_BABBLE;
						return;
					}
					DBG_PRINTF("%s Address 0x%X, Status %d\n", __func__, m_HubState->dev.Addr, status);
					for (i = 0; i < n; i++) {
						buf[i] = status >> (8 * i);
					}
					m_UsbDev->USB_PacketCopy(p, buf, n);
				}
				else {
					p->Status = USB_RET_NAK; // usb11 11.13.1
				}
			}
			else {
				goto fail;
			}
			break;
		}

		case USB_TOKEN_OUT:
		default:
		fail:
			p->Status = USB_RET_STALL;
			break;
	}
}

void Hub::UsbHub_HandleDestroy()
{
	// Inform upstream that the hub is detached and gone
	m_UsbDev->USB_DeviceDetach(&m_HubState->dev);
	m_UsbDev->m_FreePorts.push_back(m_HubState->dev.Port);

	for (int i = 0; i < NUM_PORTS; i++) {
		if (m_HubState->ports[i].port.Dev) {
			// Also destroy attached downstream device
			m_HubState->ports[i].port.Dev->klass->handle_destroy();
		}
		else {
			m_UsbDev->USB_UnregisterPort(&m_HubState->ports[i].port);
		}
	}
	UsbHubReleasePort(&m_HubState->dev);
	HubCleanUp();
}

void Hub::UsbHub_Attach(USBPort* port1)
{
	USBHubPort* port = &m_HubState->ports[port1->PortIndex];

	port->wPortStatus |= PORT_STAT_CONNECTION;
	port->wPortChange |= PORT_STAT_C_CONNECTION;
	if (port->port.Dev->Speed == USB_SPEED_LOW) {
		port->wPortStatus |= PORT_STAT_LOW_SPEED;
	}
	else {
		port->wPortStatus &= ~PORT_STAT_LOW_SPEED;
	}
	m_UsbDev->USB_Wakeup(m_HubState->intr);
}

void Hub::UsbHub_Detach(USBPort* port1)
{
	USBHubPort* port = &m_HubState->ports[port1->PortIndex];

	m_UsbDev->USB_Wakeup(m_HubState->intr);

	// Let upstream know the device on this port is gone
	m_HubState->dev.Port->Operations->child_detach(port1->Dev);

	port->wPortStatus &= ~PORT_STAT_CONNECTION;
	port->wPortChange |= PORT_STAT_C_CONNECTION;
	if (port->wPortStatus & PORT_STAT_ENABLE) {
		port->wPortStatus &= ~PORT_STAT_ENABLE;
		port->wPortChange |= PORT_STAT_C_ENABLE;
	}
	m_UsbDev->USB_Wakeup(m_HubState->intr);
}

void Hub::UsbHub_ChildDetach(XboxDeviceState* child)
{
	// Pass along to upstream
	m_HubState->dev.Port->Operations->child_detach(child);
}

void Hub::UsbHub_Wakeup(USBPort* port1)
{
	USBHubPort* port = &m_HubState->ports[port1->PortIndex];

	if (port->wPortStatus & PORT_STAT_SUSPEND) {
		port->wPortChange |= PORT_STAT_C_SUSPEND;
		m_UsbDev->USB_Wakeup(m_HubState->intr);
	}
}

void Hub::UsbHub_Complete(USBPort* port, USBPacket* packet)
{
	// Just pass it along to upstream
	m_HubState->dev.Port->Operations->complete(m_HubState->dev.Port, packet);
}

std::string Hub::GetFeatureName(int feature)
{
	std::string str;

	switch (feature) {
		case PORT_CONNECTION: {
			str = "connection";
			break;
		}

		case PORT_ENABLE: {
			str = "enable";
			break;
		}

		case PORT_SUSPEND: {
			str = "suspend";
			break;
		}

		case PORT_OVERCURRENT: {
			str = "overcurrent";
			break;
		}

		case PORT_RESET: {
			str = "reset";
			break;
		}

		case PORT_POWER: {
			str = "power";
			break;
		}

		case PORT_LOWSPEED: {
			str = "lowspeed";
			break;
		}

		case PORT_C_CONNECTION: {
			str = "change_connection";
			break;
		}

		case PORT_C_ENABLE: {
			str = "change_enable";
			break;
		}

		case PORT_C_SUSPEND: {
			str = "change_suspend";
			break;
		}

		case PORT_C_OVERCURRENT: {
			str = "change_overcurrent";
			break;
		}

		case PORT_C_RESET: {
			str = "change_reset";
			break;
		}

		default:
			str = "?";
			break;
	}

	return str;
}

void Hub::HubCleanUp()
{
	delete m_pPeripheralFuncStruct;
	delete m_HubState->ports[0].port.Operations;
	delete m_HubState;
	m_pPeripheralFuncStruct = nullptr;
	m_HubState = nullptr;
}

void Hub::HubDestroy()
{
	m_UsbDev->m_HostController->m_FrameTimeMutex.lock();
	m_pPeripheralFuncStruct->handle_destroy();
	m_UsbDev->m_HostController->m_FrameTimeMutex.unlock();
}
