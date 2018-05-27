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
	uint8_t addr;
	char product_desc[32];
	int auto_attach;
	int attached;

	int32_t state;
	uint8_t setup_buf[8];
	uint8_t data_buf[4096];
	int32_t remote_wakeup;
	int32_t setup_state;
	int32_t setup_len;
	int32_t setup_index;

	USBEndpoint ep_ctl;
	USBEndpoint ep_in[USB_MAX_ENDPOINTS];
	USBEndpoint ep_out[USB_MAX_ENDPOINTS];

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

typedef struct _USBEndpoint
{
	uint8_t nr;
	uint8_t pid;
	uint8_t type;
	uint8_t ifnum;
	int max_packet_size;
	bool pipeline;
	bool halted;
	USBDevice *dev;
	//QTAILQ_HEAD(, USBPacket) queue;
}
USBEndpoint;

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
	USBDev *Dev;
	int SpeedMask;   // usb speeds supported
	int HubCount;    // number of hubs attached
	char Path[16];   // the number of the port
	OHCI* Opaque;    // OHCI* to let USBPort access it
	int PortIndex;   // internal port index, may be used with the Opaque
	//QTAILQ_ENTRY(USBPort) next;
	// a device is attched
	void Attach(USBPort* port);
	// a device is detached
	void Detach(USBPort* port);
	// a device downstream from the device attached to the port (attached through a hub) is detached
	void ChildDetach(USBPort* port, USBDev* child);
	// TODO
	void Wakeup(USBPort* port);
	// TODO
	void Complete(USBPort* port, USBPacket *p);
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

		// USB specific functions
		// register a port with the HC
		void USB_RegisterPort(USBPort* Port, OHCI* Obj, int Index, int SpeedMask);
};

#endif
