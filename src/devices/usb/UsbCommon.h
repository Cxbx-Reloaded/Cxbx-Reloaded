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

#include "Cxbx.h"
#include "..\devices\video\queue.h"
#include <functional>

#define USB_MAX_ENDPOINTS  15
#define USB_MAX_INTERFACES 16

#define USB_STATE_NOTATTACHED 0
#define USB_STATE_ATTACHED    1
#define USB_STATE_DEFAULT     2


typedef enum USBPacketState {
	USB_PACKET_UNDEFINED = 0,
	USB_PACKET_SETUP,
	USB_PACKET_QUEUED,
	USB_PACKET_ASYNC,
	USB_PACKET_COMPLETE,
	USB_PACKET_CANCELED,
}
USBPacketState;

typedef struct _USBPacket USBPacket;
typedef struct _XboxDevice XboxDevice;
typedef const char* USBDescStrings[256];

typedef struct _USBDescOther {
    uint8_t                   length;
    const uint8_t             *data;
}
USBDescOther;

typedef struct _USBDescEndpoint {
    uint8_t                   bEndpointAddress;
    uint8_t                   bmAttributes;
    uint16_t                  wMaxPacketSize;
    uint8_t                   bInterval;
    uint8_t                   bRefresh;
    uint8_t                   bSynchAddress;

    uint8_t                   is_audio; /* has bRefresh + bSynchAddress */
    uint8_t                   *extra;

    /* superspeed endpoint companion */
    uint8_t                   bMaxBurst;
    uint8_t                   bmAttributes_super;
    uint16_t                  wBytesPerInterval;
}
USBDescEndpoint;

struct USBDescIface {
    uint8_t                   bInterfaceNumber;
    uint8_t                   bAlternateSetting;
    uint8_t                   bNumEndpoints;
    uint8_t                   bInterfaceClass;
    uint8_t                   bInterfaceSubClass;
    uint8_t                   bInterfaceProtocol;
    uint8_t                   iInterface;

    uint8_t                   ndesc;
    USBDescOther*             descs;
    USBDescEndpoint*          eps;
	USBDescIface(bool bDefault);
	~USBDescIface();
};

/* conceptually an Interface Association Descriptor, and releated interfaces */
typedef struct _USBDescIfaceAssoc {
    uint8_t                   bFirstInterface;
    uint8_t                   bInterfaceCount;
    uint8_t                   bFunctionClass;
    uint8_t                   bFunctionSubClass;
    uint8_t                   bFunctionProtocol;
    uint8_t                   iFunction;

    uint8_t                   nif;
    const USBDescIface*       ifs;
}
USBDescIfaceAssoc;

typedef struct _USBDescConfig {
    uint8_t                   bNumInterfaces;
    uint8_t                   bConfigurationValue;
    uint8_t                   iConfiguration;
    uint8_t                   bmAttributes;
    uint8_t                   bMaxPower;

    /* grouped interfaces */
    uint8_t                   nif_groups;
    const USBDescIfaceAssoc*  if_groups;

    /* "normal" interfaces */
    uint8_t                   nif;
    const USBDescIface*       ifs;
}
USBDescConfig;

struct USBDescDevice {
    uint16_t                  bcdUSB;
    uint8_t                   bDeviceClass;
    uint8_t                   bDeviceSubClass;
    uint8_t                   bDeviceProtocol;
    uint8_t                   bMaxPacketSize0;
    uint8_t                   bNumConfigurations;
    const USBDescConfig*      confs;
	USBDescDevice();
};

typedef struct _USBDescID {
    uint16_t                  idVendor;
    uint16_t                  idProduct;
    uint16_t                  bcdDevice;
    uint8_t                   iManufacturer;
    uint8_t                   iProduct;
    uint8_t                   iSerialNumber;
}
USBDescID;

typedef struct _USBDesc {
    USBDescID                 id;
    const USBDescDevice*      full;
    const char* const*        str;
}
USBDesc;

/* USB endpoint */
typedef struct _USBEndpoint
{
	uint8_t Num;                      // endpoint number
	uint8_t pid;
	uint8_t Type;                     // the type of this endpoint
	uint8_t ifnum;
	int max_packet_size;
	bool Pipeline;
	bool Halted;                      // indicates that the endpoint is halted
	XboxDevice* Dev;                  // device this endpoint belongs to
	QTAILQ_HEAD(, _USBPacket) Queue;  // queue of packets to this endpoint
}
USBEndpoint;

/* definition of an Xbox usb device */
typedef struct _XboxDevice
{
	DeviceState qdev;
	USBPort *port;
	char *port_path;
	char *serial;
	void *opaque;
	uint32_t flags;

	// Actual connected speed
	int speed;
	// Supported speeds, not in info because it may be variable (hostdevs)
	int speedmask;
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

	QLIST_HEAD(, USBDescString) Strings;
	const USBDesc *usb_desc;               // Overrides class usb_desc if not NULL
	const USBDescDevice *device;

	int configuration;
	int ninterfaces;
	int altsetting[USB_MAX_INTERFACES];
	const USBDescConfig *config;
	const USBDescIface  *ifaces[USB_MAX_INTERFACES];
}
XboxDevice;

typedef struct _USBCombinedPacket {
	_USBPacket* First;
	QTAILQ_HEAD(packets_head, _USBPacket) Packets;
	IOVector IoVec;
}
USBCombinedPacket;

/* Structure used to hold information about an active USB packet */
struct _USBPacket
{
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
	QTAILQ_ENTRY(_USBPacket) Queue;
	QTAILQ_ENTRY(_USBPacket) CombinedEntry;
};

/* Struct describing the status of a usb port */
typedef struct _USBPort {
	XboxDevice* Dev;              // usb device (if present)
	int SpeedMask;                // usb speeds supported
	int HubCount;                 // number of hubs attached
	char Path[16];                // the number of the port
	int PortIndex;                // internal port index
	QTAILQ_ENTRY(_USBPort) Next;
}
USBPort;

/* Struct which stores general functions/variables regarding the peripheral */
typedef struct _USBDeviceClass
{
	std::function<int(XboxDevice* dev)> init;

	// Walk (enabled) downstream ports, check for a matching device.
	// Only hubs implement this.
	std::function<XboxDevice*(XboxDevice* dev, uint8_t addr)> find_device;

	// Called when a packet is canceled.
	std::function<void(XboxDevice* dev, USBPacket* p)> cancel_packet;

	// Called when device is destroyed.
	std::function<void(XboxDevice* dev)> handle_destroy;

	// Attach the device
	std::function<void(XboxDevice* dev)> handle_attach;

	// Reset the device
	std::function<void(XboxDevice* dev)> handle_reset;

	// Process control request.
	// Called from handle_packet().
	// Status gets stored in p->status, and if p->status == USB_RET_SUCCESS
	// then the number of bytes transferred is stored in p->actual_length
	std::function<void(XboxDevice* dev, USBPacket* p, int request, int value,
		int index, int length, uint8_t *data)> handle_control;

	// Process data transfers (both BULK and ISOC).
	// Called from handle_packet().
	// Status gets stored in p->status, and if p->status == USB_RET_SUCCESS
	// then the number of bytes transferred is stored in p->actual_length
	std::function<void(XboxDevice* dev, USBPacket* p)> handle_data;

	std::function<void(XboxDevice* dev, int Interface,
		int alt_old, int alt_new)> set_interface;

	// Called when the hcd is done queuing packets for an endpoint, only
	// necessary for devices which can return USB_RET_ADD_TO_QUEUE.
	std::function<void(XboxDevice* dev, USBEndpoint* ep)> flush_ep_queue;

	// Called by the hcd to let the device know the queue for an endpoint
	// has been unlinked / stopped. Optional may be NULL.
	std::function<void(XboxDevice* Dev, USBEndpoint* EP)> EP_Stopped;

	const char* product_desc;
	const USBDesc* usb_desc;
}
USBDeviceClass;


/* Abstract class representing a usb peripheral */
class UsbPeripheral
{
	protected:
		USBDeviceClass* m_pPeripheralFuncStruct;
		XboxDevice* m_pDeviceStruct;
		
		virtual void Init() = 0;
		
};
