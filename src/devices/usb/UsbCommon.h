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
// *   Cxbx->devices->usb->UsbCommon.h
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

#ifndef USBCOMMON_H_
#define USBCOMMON_H_

#include "CxbxCommon.h"
#include "..\devices\video\queue.h"
#include <functional>

#define USB_MAX_ENDPOINTS  15
#define USB_MAX_INTERFACES 16

#define USB_STATE_NOTATTACHED 0
#define USB_STATE_ATTACHED    1
#define USB_STATE_DEFAULT     2

#define USB_CLASS_HUB         9

#define USB_ENDPOINT_XFER_CONTROL   0
#define USB_ENDPOINT_XFER_ISOC      1
#define USB_ENDPOINT_XFER_BULK      2
#define USB_ENDPOINT_XFER_INT       3
#define USB_ENDPOINT_XFER_INVALID   255

#define USB_INTERFACE_INVALID       255

#define USB_DIR_OUT         0
#define USB_DIR_IN          0x80

#define USB_TOKEN_SETUP 0x2D
#define USB_TOKEN_IN    0x69 // device -> host
#define USB_TOKEN_OUT   0xE1 // host -> device

#define USB_SPEED_LOW   0
#define USB_SPEED_FULL  1


typedef enum _USB_SPEED
{
	USB_SPEED_MASK_LOW = 1 << 0,
	USB_SPEED_MASK_FULL = 1 << 1,
}
USB_SPEED;

typedef enum USBPacketState {
	USB_PACKET_UNDEFINED = 0,
	USB_PACKET_SETUP,
	USB_PACKET_QUEUED,
	USB_PACKET_ASYNC,
	USB_PACKET_COMPLETE,
	USB_PACKET_CANCELED,
}
USBPacketState;

struct USBPacket;
struct XboxDeviceState;
typedef const char* USBDescStrings[256];

/* String descriptor */
struct USBDescString {
	uint8_t index;      // index of this string descriptor
	std::string str;    // the string of this string descriptor
	QLIST_ENTRY(USBDescString) next;
};

struct USBDescOther {
    uint8_t                   length;
    const uint8_t*            data;
};

/* Endpoint descriptor */
struct USBDescEndpoint {
    uint8_t                   bEndpointAddress; // the address of the endpoint on the USB device described by this descriptor
    uint8_t                   bmAttributes;     // this field describes the endpoint's attributes when it is configured using the bConfigurationValue
    uint16_t                  wMaxPacketSize;   // maximum packet size this endpoint is capable of sending or receiving when this configuration is selected
    uint8_t                   bInterval;        // interval for polling endpoint for data transfers, expressed in milliseconds.
    uint8_t                   bRefresh;         // for audio devices only: the rate at which synchronization feedback is provided
    uint8_t                   bSynchAddress;    // for audio devices only: the address of the synchronization endpoint

    uint8_t                   is_audio; /* has bRefresh + bSynchAddress */
    uint8_t*                  extra;

	// Dropped from XQEMU the parameters bMaxBurst, bmAttributes_super and wBytesPerInterval because those are only defined for
	// superspeed (usb 3.0) devices in the superspeed endpoint companion
};

/* Interface descriptor */
struct USBDescIface {
    uint8_t                   bInterfaceNumber;   // number of interface
    uint8_t                   bAlternateSetting;  // value used to select the alternate setting for the interface identified by bInterfaceNumber
    uint8_t                   bNumEndpoints;      // number of endpoints used by this interface (excluding endpoint zero)
    uint8_t                   bInterfaceClass;    // class code (assigned by the USB)
    uint8_t                   bInterfaceSubClass; // subclass code (assigned by the USB)
    uint8_t                   bInterfaceProtocol; // protocol code (assigned by the USB)
    uint8_t                   iInterface;         // index of string descriptor describing this interface

    uint8_t                   ndesc;
    USBDescOther*             descs;
    USBDescEndpoint*          eps;                // endpoints supported by this interface
	USBDescIface(bool bDefault);
	~USBDescIface();
};

/*
* ergo720: I removed the Interface Association Descriptor (IAD) since, at the time of this writing (2018), the xboxdevwiki documents that all
* known xid devices don't use them and also, according to the corresponding standard, IAD applies to usb revision 2.0 while the xbox uses
* usb revision 1.1 so it shouldn't support them either. If this turns out to be incorrect, then IAD support will have to be added
*/

/* Configuration descriptor */
struct USBDescConfig {
    uint8_t                   bNumInterfaces;      // number of interfaces supported by this configuration
    uint8_t                   bConfigurationValue; // value to use as an argument to the SetConfiguration() request to select this configuration
    uint8_t                   iConfiguration;      // index of string descriptor describing this configuration
    uint8_t                   bmAttributes;        // configuration characteristics
    uint8_t                   bMaxPower;           // maximum power consumption of the USB device in this configuration expressed in 2mA units
    uint8_t                   nif;                 // number of interfaces (again)
    const USBDescIface*       ifs;                 // interfaces supported by this configuration
};

/* Device descriptor part 1 */
struct USBDescDevice {
    uint16_t                  bcdUSB;             // USB Specification Release Number in Binary-Coded Decimal (i.e., 2.10 is 210H)
    uint8_t                   bDeviceClass;       // class code (assigned by the USB)
    uint8_t                   bDeviceSubClass;    // subclass code (assigned by the USB)
    uint8_t                   bDeviceProtocol;    // protocol code (assigned by the USB)
    uint8_t                   bMaxPacketSize0;    // maximum packet size for endpoint zero (only 8, 16, 32, or 64 are valid)
    uint8_t                   bNumConfigurations; // number of possible configurations
    const USBDescConfig*      confs;              // configurations supported by this device
	USBDescDevice(bool bDefault);
	~USBDescDevice();
};

/* Device descriptor part 2 */
struct USBDescID {
    uint16_t                  idVendor;      // vendor ID (assigned by the USB)
    uint16_t                  idProduct;     // product ID (assigned by the manufacturer)
    uint16_t                  bcdDevice;     // device release number in binary-coded decimal
    uint8_t                   iManufacturer; // index of string descriptor describing manufacturer
    uint8_t                   iProduct;      // index of string descriptor describing product
    uint8_t                   iSerialNumber; // index of string descriptor describing the device’s serial number
};

/* Global USB Descriptor struct */
struct USBDesc {
    USBDescID                 id;   // id-specific info of the device descriptor
    const USBDescDevice*      full; // remaining fields of the device descriptor
    const char* const*        str;
	USBDesc(bool bDefault);
};

/* USB endpoint */
struct USBEndpoint {
	uint8_t Num;                      // endpoint number
	uint8_t pid;
	uint8_t Type;                     // the type of this endpoint
	uint8_t IfNum;                    // interface number this endpoint belongs to
	int MaxPacketSize;                // maximum packet size supported by this endpoint
	bool Pipeline;
	bool Halted;                      // indicates that the endpoint is halted
	XboxDeviceState* Dev;             // device this endpoint belongs to
	QTAILQ_HEAD(, USBPacket) Queue;   // queue of packets to this endpoint
};

/* definition of an Xbox usb device */
struct XboxDeviceState {
	USBPort* Port;                         // usb port struct of this device
	int PortPath;                          // port index to which this device is attached to
	char* Serial;						   
	uint32_t flags;						   
	USBDeviceClass* klass;                 // usb class struct of this device
										   
	int Speed;                             // actual speed of the connected device
	int SpeedMask;                         // supported speeds, not in info because it may be variable (hostdevs)
	uint8_t Addr;                          // device function address
	std::string ProductDesc;               // the friendly name of this device
	int Attached;                          // device is attached

	int32_t State;                         // current state of device
	uint8_t SetupBuffer[8];                // holds the IoVec structs copied (control transfers only?)
	uint8_t data_buf[4096];
	int32_t RemoteWakeup;                  // wakeup flag
	int32_t SetupState;                    // result of a setup tken processing operation
	int32_t SetupLength;                   // number of bytes to transfer as specified by a setup token
	int32_t SetupIndex;                    // index of the parameter in a setup token?

	USBEndpoint EP_ctl;                    // endpoints for SETUP tokens
	USBEndpoint EP_in[USB_MAX_ENDPOINTS];  // endpoints for OUT tokens
	USBEndpoint EP_out[USB_MAX_ENDPOINTS]; // endpoints for IN tokens

	QLIST_HEAD(, USBDescString) Strings;   // strings of the string descriptor
	const USBDesc* UsbDesc;                // Overrides class usb_desc if not nullptr
	const USBDescDevice* Device;           // device descriptor part 1

	int Configuration;                              // number of the selected configuration descriptor
	int NumInterfaces;                              // number of available interface descriptors
	int AltSetting[USB_MAX_INTERFACES];             // alternate setting numbers for the current interface
	const USBDescConfig* Config;                    // configuration in use
	const USBDescIface* Ifaces[USB_MAX_INTERFACES]; // interface in use
};

struct USBCombinedPacket {
	USBPacket* First;
	QTAILQ_HEAD(packets_head, USBPacket) Packets;
	IOVector IoVec;
};

/* Structure used to hold information about an active USB packet */
struct USBPacket {
	int Pid;                                 // Packet ID (used to identify the type of packet that is being sent)
	uint32_t Id;                             // Paddr of the TD for this packet 
	USBEndpoint* Endpoint;                   // endpoint this packet is transferred to
	unsigned int Stream;		             
	IOVector IoVec;                          // used to perform vectored I/O
	uint64_t Parameter;                      // control transfers
	bool ShortNotOK;                         // the bufferRounding mode of the TD for this packet
	bool IntReq;                             // whether or not to generate an interrupt for this packet (DelayInterrupt of the TD is zero)
	int Status;                              // USB_RET_* status code
	int ActualLength;                        // before copy: offset inside IoVec structs; after copy: number of bytes actually transferred
	// Internal use by the USB layer
	USBPacketState State;
	USBCombinedPacket* Combined;
	QTAILQ_ENTRY(USBPacket) Queue;
	QTAILQ_ENTRY(USBPacket) CombinedEntry;
};

struct USBPortOps {
	std::function<void(USBPort* port)> attach;
	std::function<void(USBPort* port)> detach;
	/*
	* This gets called when a device downstream from the device attached to
	* the port (attached through a hub) gets detached.
	*/
	std::function<void(XboxDeviceState* child)> child_detach;
	std::function<void(USBPort* port)> wakeup;
	/*
	* Note that port->dev will be different then the device from which
	* the packet originated when a hub is involved.
	*/
	std::function<void(USBPort* port, USBPacket* p)> complete;
};

/* Struct describing the status of a usb port */
struct USBPort {
	XboxDeviceState* Dev;         // usb device (if present)
	USBPortOps* Operations;       // functions to call when a port event happens
	int SpeedMask;                // usb speeds supported
	int HubCount;                 // number of hubs chained
	char Path[16];                // the number of the port + 1, used to create a serial number for this device
	int PortIndex;                // internal port index
};

/* Struct which stores general functions/variables regarding the peripheral */
struct USBDeviceClass {
	std::function<int(XboxDeviceState* dev)> init;

	// Walk (enabled) downstream ports, check for a matching device.
	// Only hubs implement this.
	std::function<XboxDeviceState*(XboxDeviceState* dev, uint8_t addr)> find_device;

	// Called when a packet is canceled.
	std::function<void(XboxDeviceState* dev, USBPacket* p)> cancel_packet;

	// Called when device is destroyed.
	std::function<void(XboxDeviceState* dev)> handle_destroy;

	// Attach the device
	std::function<void(XboxDeviceState* dev)> handle_attach;

	// Reset the device
	std::function<void(XboxDeviceState* dev)> handle_reset;

	// Process control request.
	// Called from handle_packet().
	// Status gets stored in p->status, and if p->status == USB_RET_SUCCESS
	// then the number of bytes transferred is stored in p->actual_length
	std::function<void(XboxDeviceState* dev, USBPacket* p, int request, int value,
		int index, int length, uint8_t *data)> handle_control;

	// Process data transfers (both BULK and ISOC).
	// Called from handle_packet().
	// Status gets stored in p->status, and if p->status == USB_RET_SUCCESS
	// then the number of bytes transferred is stored in p->actual_length
	std::function<void(XboxDeviceState* dev, USBPacket* p)> handle_data;

	std::function<void(XboxDeviceState* dev, int Interface,
		int alt_old, int alt_new)> set_interface;

	// Called when the hcd is done queuing packets for an endpoint, only
	// necessary for devices which can return USB_RET_ADD_TO_QUEUE.
	std::function<void(XboxDeviceState* dev, USBEndpoint* ep)> flush_ep_queue;

	// Called by the hcd to let the device know the queue for an endpoint
	// has been unlinked / stopped. Optional may be NULL.
	std::function<void(XboxDeviceState* Dev, USBEndpoint* EP)> ep_stopped;

	const char* product_desc;  // friendly name of the device
	const USBDesc* usb_desc;   // device descriptor
};

#endif
