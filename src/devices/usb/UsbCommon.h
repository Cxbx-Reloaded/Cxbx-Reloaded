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

// Acknowledgment: some of the functions present are from XQEMU (GPLv2)
// https://xqemu.com/

#ifndef USBCOMMON_H_
#define USBCOMMON_H_

#include "common\util\CxbxUtil.h"
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

#define USB_DEVICE_SELF_POWERED		0
#define USB_DEVICE_REMOTE_WAKEUP	1

#define USB_TYPE_MASK			(0x03 << 5)
#define USB_TYPE_STANDARD		(0x00 << 5)
#define USB_TYPE_CLASS			(0x01 << 5)
#define USB_TYPE_VENDOR			(0x02 << 5)
#define USB_TYPE_RESERVED		(0x03 << 5)

#define USB_RECIP_MASK			0x1F
#define USB_RECIP_DEVICE		0x00
#define USB_RECIP_INTERFACE		0x01
#define USB_RECIP_ENDPOINT		0x02
#define USB_RECIP_OTHER			0x03

#define DeviceRequest ((USB_DIR_IN|USB_TYPE_STANDARD|USB_RECIP_DEVICE)<<8)
#define DeviceOutRequest ((USB_DIR_OUT|USB_TYPE_STANDARD|USB_RECIP_DEVICE)<<8)
#define VendorDeviceRequest ((USB_DIR_IN|USB_TYPE_VENDOR|USB_RECIP_DEVICE)<<8)
#define VendorDeviceOutRequest ((USB_DIR_OUT|USB_TYPE_VENDOR|USB_RECIP_DEVICE)<<8)

#define InterfaceRequest \
        ((USB_DIR_IN|USB_TYPE_STANDARD|USB_RECIP_INTERFACE)<<8)
#define InterfaceOutRequest \
        ((USB_DIR_OUT|USB_TYPE_STANDARD|USB_RECIP_INTERFACE)<<8)
#define ClassInterfaceRequest \
        ((USB_DIR_IN|USB_TYPE_CLASS|USB_RECIP_INTERFACE)<<8)
#define ClassInterfaceOutRequest \
        ((USB_DIR_OUT|USB_TYPE_CLASS|USB_RECIP_INTERFACE)<<8)
#define VendorInterfaceRequest \
        ((USB_DIR_IN|USB_TYPE_VENDOR|USB_RECIP_INTERFACE)<<8)
#define VendorInterfaceOutRequest \
        ((USB_DIR_OUT|USB_TYPE_VENDOR|USB_RECIP_INTERFACE)<<8)

#define EndpointRequest ((USB_DIR_IN|USB_TYPE_STANDARD|USB_RECIP_ENDPOINT)<<8)
#define EndpointOutRequest \
        ((USB_DIR_OUT|USB_TYPE_STANDARD|USB_RECIP_ENDPOINT)<<8)

#define USB_REQ_GET_STATUS			0x00
#define USB_REQ_CLEAR_FEATURE		0x01
#define USB_REQ_SET_FEATURE			0x03
#define USB_REQ_SET_ADDRESS			0x05
#define USB_REQ_GET_DESCRIPTOR		0x06
#define USB_REQ_SET_DESCRIPTOR		0x07
#define USB_REQ_GET_CONFIGURATION	0x08
#define USB_REQ_SET_CONFIGURATION	0x09
#define USB_REQ_GET_INTERFACE		0x0A
#define USB_REQ_SET_INTERFACE		0x0B
#define USB_REQ_SYNCH_FRAME			0x0C

#define USB_DT_DEVICE			0x01
#define USB_DT_CONFIG			0x02
#define USB_DT_STRING			0x03
#define USB_DT_INTERFACE		0x04
#define USB_DT_ENDPOINT			0x05

#define USB_RET_SUCCESS           (0)
#define USB_RET_NODEV             (-1)
#define USB_RET_NAK               (-2)
#define USB_RET_STALL             (-3)
#define USB_RET_BABBLE            (-4)
#define USB_RET_IOERROR           (-5)
#define USB_RET_ASYNC             (-6)
#define USB_RET_ADD_TO_QUEUE      (-7)
#define USB_RET_REMOVE_FROM_QUEUE (-8)


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

/* same as Linux kernel root hubs */
typedef enum {
	STR_MANUFACTURER = 1,
	STR_PRODUCT,
	STR_SERIALNUMBER,
}
STRING_DESC_INDEX;

// Forward declarations
struct USBPacket;
struct XboxDeviceState;
struct USBPortOps;

/* String descriptor */
struct USBDescString {
	uint8_t index;      // index of this string descriptor
	std::string str;    // the string of this string descriptor
	QLIST_ENTRY(USBDescString) next;
};

// Device-specific class descriptors, if any. No idea if some Xbox devices use this but, if not, this can be removed
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

    uint8_t                   is_audio;         // has bRefresh + bSynchAddress
    uint8_t*                  extra;            // class-specific descriptors (if any) associated with this endpoint

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

    uint8_t                   ndesc;              // number of device-specific class descriptors (if any)
    USBDescOther*             descs;              // pointer to the extra class descriptors
    const USBDescEndpoint*    eps;                // endpoints supported by this interface
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
};

#pragma pack(1)

// Binary representation of the descriptors
// Dropped from XQEMU usb 2.0 and 3.0 only descriptors
struct USBDescriptor {
	uint8_t                   bLength;
	uint8_t                   bDescriptorType;
	union {
		struct {
			uint8_t           bcdUSB_lo;
			uint8_t           bcdUSB_hi;
			uint8_t           bDeviceClass;
			uint8_t           bDeviceSubClass;
			uint8_t           bDeviceProtocol;
			uint8_t           bMaxPacketSize0;
			uint8_t           idVendor_lo;
			uint8_t           idVendor_hi;
			uint8_t           idProduct_lo;
			uint8_t           idProduct_hi;
			uint8_t           bcdDevice_lo;
			uint8_t           bcdDevice_hi;
			uint8_t           iManufacturer;
			uint8_t           iProduct;
			uint8_t           iSerialNumber;
			uint8_t           bNumConfigurations;
		} device; // device descriptor
		struct {
			uint8_t           wTotalLength_lo;
			uint8_t           wTotalLength_hi;
			uint8_t           bNumInterfaces;
			uint8_t           bConfigurationValue;
			uint8_t           iConfiguration;
			uint8_t           bmAttributes;
			uint8_t           bMaxPower;
		} config; // configuration descriptor
		struct {
			uint8_t           bInterfaceNumber;
			uint8_t           bAlternateSetting;
			uint8_t           bNumEndpoints;
			uint8_t           bInterfaceClass;
			uint8_t           bInterfaceSubClass;
			uint8_t           bInterfaceProtocol;
			uint8_t           iInterface;
		} iface; // interface descriptor
		struct {
			uint8_t           bEndpointAddress;
			uint8_t           bmAttributes;
			uint8_t           wMaxPacketSize_lo;
			uint8_t           wMaxPacketSize_hi;
			uint8_t           bInterval;
			uint8_t           bRefresh;        // only audio ep
			uint8_t           bSynchAddress;   // only audio ep
		} endpoint; // endpoint descriptor
	} u;
};

#pragma pack()

/* USB endpoint */
struct USBEndpoint {
	uint8_t Num;                      // endpoint number
	uint8_t Type;                     // the type of this endpoint
	uint8_t IfNum;                    // interface number this endpoint belongs to
	int MaxPacketSize;                // maximum packet size supported by this endpoint
	bool Halted;                      // indicates that the endpoint is halted
	XboxDeviceState* Dev;             // device this endpoint belongs to
	QTAILQ_HEAD(, USBPacket) Queue;   // queue of packets to this endpoint
};

/* Struct describing the status of a usb port */
struct USBPort {
	XboxDeviceState* Dev;         // usb device (if present)
	USBPortOps* Operations;       // functions to call when a port event happens
	int SpeedMask;                // usb speeds supported
	std::string Path;             // the number of the port + 1, used to create a serial number for this device
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
	std::function<void(void)> handle_destroy;

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

/* definition of an Xbox usb device */
struct XboxDeviceState {
	USBPort* Port;                         // usb port struct of this device
	int PortPath;                          // port index to which this device is attached to
	USBDeviceClass* klass;                 // usb class struct of this device
										   
	int Speed;                             // actual speed of the connected device
	int SpeedMask;                         // supported speeds, not in info because it may be variable (hostdevs)
	uint8_t Addr;                          // device function address
	std::string ProductDesc;               // the friendly name of this device
	int Attached;                          // device is attached

	int32_t State;                         // current state of device
	uint8_t SetupBuffer[8];                // setup packet buffer - 8 bytes (control transfers only)
	uint8_t DataBuffer[4096];              // buffer where to write the data requested during usb requests
	int32_t RemoteWakeup;                  // wakeup flag
	int32_t SetupState;                    // result of a control transfer processing operation
	int32_t SetupLength;                   // this field specifies the length of the data transferred during the second phase of the control transfer
	int32_t SetupIndex;                    // index of the parameter in a setup token?

	USBEndpoint EP_ctl;                    // control endpoint
	USBEndpoint EP_in[USB_MAX_ENDPOINTS];  // device endpoint (input direction)
	USBEndpoint EP_out[USB_MAX_ENDPOINTS]; // device endpoint (output direction)

	QLIST_HEAD(, USBDescString) Strings;   // strings of the string descriptors
	const USBDesc* UsbDesc;                // Overrides class usb_desc if not nullptr
	const USBDescDevice* Device;           // device descriptor part 1

	int Configuration;                              // number of the selected configuration descriptor
	int NumInterfaces;                              // number of available interface descriptors
	int AltSetting[USB_MAX_INTERFACES];             // alternate setting numbers for the current interface
	const USBDescConfig* Config;                    // configuration in use
	const USBDescIface* Ifaces[USB_MAX_INTERFACES]; // interface in use
};

/* Structure used to hold information about an active USB packet */
struct USBPacket {
	int Pid;                             // Packet ID (used to identify the type of packet that is being processed)
	uint32_t Id;                         // Paddr of the TD for this packet 
	USBEndpoint* Endpoint;               // endpoint this packet is transferred to
	IOVector IoVec;                      // used to perform vectored I/O
	bool ShortNotOK;                     // the bufferRounding mode of the TD for this packet
	bool IntReq;                         // whether or not to generate an interrupt for this packet (DelayInterrupt of the TD is zero)
	int Status;                          // USB_RET_* status code
	int ActualLength;                    // number of bytes actually written to DataBuffer
	// Internal use by the USB layer
	USBPacketState State;
	QTAILQ_ENTRY(USBPacket) Queue;
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

#endif
