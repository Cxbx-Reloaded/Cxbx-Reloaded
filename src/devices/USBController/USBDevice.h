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
// *   Cxbx->devices->USBController->USBDevice.h
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
#ifndef USBDEVICE_H_
#define USBDEVICE_H_

#include "..\PCIDevice.h"
#include <assert.h>

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

// This is a linux struct for vectored I/O. See readv() and writev()
typedef struct _IoVec
{
	void* Iov_Base;
	size_t Iov_Len;
}
IoVec;

typedef struct _IOVector
{
	IoVec* IoVec;
	int IoVecNumber;      // TODO
	int AllocNumber;      // TODO
	size_t Size;
}
IOVector;

typedef struct _USBEndpoint
{
	uint8_t nr;
	uint8_t pid;
	uint8_t type;
	uint8_t ifnum;
	int max_packet_size;
	bool pipeline;
	bool halted;
	USBDev *dev;
	//QTAILQ_HEAD(, USBPacket) queue;
}
USBEndpoint;

// definition of a USB device
typedef struct _USBDev
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
	uint8_t Addr; // device address
	char product_desc[32];
	int auto_attach;
	int Attached; // device is attached

	int32_t State; // current state of device
	uint8_t setup_buf[8];
	uint8_t data_buf[4096];
	int32_t RemoteWakeup; // wakeup flag
	int32_t setup_state;
	int32_t setup_len;
	int32_t setup_index;

	USBEndpoint EP_ctl;
	USBEndpoint EP_in[USB_MAX_ENDPOINTS];
	USBEndpoint EP_out[USB_MAX_ENDPOINTS];

	//QLIST_HEAD(, USBDescString) strings;
	const USBDesc *usb_desc; // Overrides class usb_desc if not NULL
	const USBDescDevice *device;

	int configuration;
	int ninterfaces;
	int altsetting[USB_MAX_INTERFACES];
	const USBDescConfig *config;
	const USBDescIface  *ifaces[USB_MAX_INTERFACES];
}
USBDev;

typedef struct USBDeviceClass
{
	DeviceClass parent_class;

	int(*init)(USBDev *dev);

	// Walk (enabled) downstream ports, check for a matching device.
	// Only hubs implement this.
	USBDev *(*find_device)(USBDev *dev, uint8_t addr);

	// Called when a packet is canceled.
	void(*cancel_packet)(USBDev *dev, USBPacket *p);

	// Called when device is destroyed.
	void(*handle_destroy)(USBDev *dev);

	// Attach the device
	void(*handle_attach)(USBDev *dev);

	// Reset the device
	void(*handle_reset)(USBDev *dev);

	// Process control request.
	// Called from handle_packet().
	// Status gets stored in p->status, and if p->status == USB_RET_SUCCESS
	// then the number of bytes transferred is stored in p->actual_length
	void(*handle_control)(USBDev *dev, USBPacket *p, int request, int value,
		int index, int length, uint8_t *data);

	// Process data transfers (both BULK and ISOC).
	// Called from handle_packet().
	// Status gets stored in p->status, and if p->status == USB_RET_SUCCESS
	// then the number of bytes transferred is stored in p->actual_length
	void(*handle_data)(USBDev *dev, USBPacket *p);

	void(*set_interface)(USBDev *dev, int Interface,
		int alt_old, int alt_new);

	// Called when the hcd is done queuing packets for an endpoint, only
	// necessary for devices which can return USB_RET_ADD_TO_QUEUE.
	void(*flush_ep_queue)(USBDev *dev, USBEndpoint *ep);

	// Called by the hcd to let the device know the queue for an endpoint
	// has been unlinked / stopped. Optional may be NULL.
	void(*EP_Stopped)(USBDev* Dev, USBEndpoint* EP);

	const char *product_desc;
	const USBDesc *usb_desc;
}
USBDeviceClass;

// Structure used to hold information about an active USB packet
typedef struct _USBPacket
{
	// Data fields for use by the driver
	int Pid;
	uint64_t Id;
	USBEndpoint* Endpoint;
	unsigned int stream;
	IOVector IoVec;               // used to perform vectored I/O
	uint64_t parameter;           // control transfers
	bool short_not_ok;
	bool int_req;
	int status;                   // USB_RET_* status code
	int actual_length;            // Number of bytes actually transferred
	// Internal use by the USB layer
	USBPacketState state;
	USBCombinedPacket *combined;
	//QTAILQ_ENTRY(USBPacket) queue;
	//QTAILQ_ENTRY(USBPacket) combined_entry;
}
USBPacket;

// Struct describing the status of a usb port
typedef struct _USBPort {
	USBDev* Dev;     // usb device (if present)
	int SpeedMask;   // usb speeds supported
	int HubCount;    // number of hubs attached
	char Path[16];   // the number of the port
	int PortIndex;   // internal port index
	//QTAILQ_ENTRY(USBPort) next;
}
USBPort;


class USBDevice : public PCIDevice {
	public:
		// constructor
		USBDevice() {}
		// destructor
		~USBDevice() {}
	
		// PCI Device functions
		void Init(unsigned int address);
		void Reset() {}
	
		uint32_t IORead(int barIndex, uint32_t port, unsigned size) {}
		void IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size) {}
		uint32_t MMIORead(int barIndex, uint32_t addr, unsigned size);
		void MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size);


		// pointers to the two USB host controllers available on the Xbox
		OHCI* m_pHostController1 = nullptr;
		OHCI* m_pHostController2 = nullptr;
};

#endif
