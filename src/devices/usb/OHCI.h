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

#ifndef OHCI_H_
#define OHCI_H_

#include <mutex>
#include "USBDevice.h"
#include "Timer.h"


// Abbreviations used:
// OHCI: Open Host Controller Interface; the standard used on the Xbox to comunicate with the usb devices
// HC: Host Controller; the hardware which interfaces with the usb device and the usb driver
// HCD: Host Controller Driver; software which talks to the HC, it's linked in the xbe
// SOF: start of frame; the beginning of a USB-defined frame
// EOF: end of frame; the end of a USB-defined frame
// ED: endpoint descriptor; a memory structure used by the HC to communicate with an endpoint
// TD: transfer descriptor; a memory structure used by the HC to transfer a block of data to/from a device endpoint
// HCCA: Host Controller Communications Area; shared memory between the HC and HCD


/* endpoint descriptor */
struct OHCI_ED {
	uint32_t Flags;
	uint32_t TailP;
	uint32_t HeadP;
	uint32_t NextED;
};

/* general transfer descriptor */
struct OHCI_TD {
	uint32_t Flags;
	uint32_t CurrentBufferPointer;
	uint32_t NextTD;
	uint32_t BufferEnd;
};

/* Isochronous transfer descriptor */
struct OHCI_ISO_TD {
	uint32_t Flags;
	uint32_t BufferPage0;
	uint32_t NextTD;
	uint32_t BufferEnd;
	uint16_t Offset[8];
};

/* enum indicating the current HC state */
typedef enum _OHCI_State
{
	Reset = 0x00,
	Resume = 0x40,
	Operational = 0x80,
	Suspend = 0xC0,
}
OHCI_State;

/* Host Controller Communications Area */
struct OHCI_HCCA
{
	uint32_t HccaInterrruptTable[32];
	uint16_t HccaFrameNumber, HccaPad1;
	uint32_t HccaDoneHead;
};

/* Small struct used to hold the HcRhPortStatus register and the usb port status */
struct OHCIPort
{
	USBPort UsbPort;
	uint32_t HcRhPortStatus;
};

/* All these registers are well documented in the OHCI standard */
struct OHCI_Registers
{
	// Control and Status partition
	uint32_t HcRevision;
	uint32_t HcControl;
	uint32_t HcCommandStatus;
	uint32_t HcInterruptStatus;
	// HcInterruptEnable/Disable are the same so we can merge them together
	uint32_t HcInterrupt;

	// Memory Pointer partition
	uint32_t HcHCCA;
	uint32_t HcPeriodCurrentED;
	uint32_t HcControlHeadED;
	uint32_t HcControlCurrentED;
	uint32_t HcBulkHeadED;
	uint32_t HcBulkCurrentED;
	uint32_t HcDoneHead;

	// Frame Counter partition
	uint32_t HcFmInterval;
	uint32_t HcFmRemaining;
	uint32_t HcFmNumber;
	uint32_t HcPeriodicStart;
	uint32_t HcLSThreshold;

	// Root Hub partition
	uint32_t HcRhDescriptorA;
	uint32_t HcRhDescriptorB;
	uint32_t HcRhStatus;
	OHCIPort RhPort[4]; // 4 ports per HC
};


/* OHCI class representing the state of the HC */
class OHCI
{
	public:
		// Indicates that the timer thread is accessing the OHCI object. Necessary because the input thread from the
		// InputDeviceManager will access us when it needs to create or destroy a device
		std::mutex m_FrameTimeMutex;

		// constructor
		OHCI(USBDevice* UsbObj);
		// destructor
		~OHCI() {}
		// read a register
		uint32_t OHCI_ReadRegister(xbox::addr_xt Addr);
		// write a register
		void OHCI_WriteRegister(xbox::addr_xt Addr, uint32_t Value);
		// calculates when the next EOF is due
		uint64_t OHCI_next(uint64_t now);
		// EOF callback function
		void OHCI_FrameBoundaryWorker();


	private:
		// pointer to g_USB0
		USBDevice* m_UsbDevice = nullptr;
		// all the registers available in the OHCI standard
		OHCI_Registers m_Registers;
		// end-of-frame timer
		bool m_pEOFtimer = false;
		// time at which a SOF was sent
		uint64_t m_SOFtime;
		// the duration of a usb frame
		uint64_t m_UsbFrameTime;
		// ticks per usb tick
		uint64_t m_TicksPerUsbTick;
		// pending usb packet to process
		USBPacket m_UsbPacket = {};
		// temporary buffer that holds the user data to transfer in a packet
		uint8_t m_UsbBuffer[8192] = {};
		// the value of HcControl in the previous frame
		uint32_t m_OldHcControl;
		// Done Queue Interrupt Counter
		int m_DoneCount;
		// the address of the pending TD
		xbox::addr_xt m_AsyncTD = xbox::zero;
		// indicates if there is a pending asynchronous packet to process
		int m_AsyncComplete = 0;

		// inform the HCD that we got a problem here...
		void OHCI_FatalError();
		// initialize packet struct
		void OHCI_PacketInit(USBPacket* packet);
		// change usb state mode
		void OHCI_ChangeState(uint32_t Value);
		// switch the HC to the reset state
		void OHCI_StateReset();
		// start sending SOF tokens across the usb bus
		void OHCI_BusStart();
		// stop sending SOF tokens across the usb bus
		void OHCI_BusStop();
		// generate a SOF event
		void OHCI_SOF();
		// change interrupt status
		void OHCI_UpdateInterrupt();
		// fire an interrupt
		void OHCI_SetInterrupt(uint32_t Value);
		// calculate frame time remaining
		uint32_t OHCI_GetFrameRemaining();
		// halt the endpoints of the device
		void OHCI_StopEndpoints();
		// set root hub status
		void OHCI_SetHubStatus(uint32_t Value);
		// update power related bits in HcRhPortStatus
		void OHCI_PortPower(int i, int p);
		// set root hub port status
		void OHCI_PortSetStatus(int PortNum, uint32_t Value);
		// set a flag in a port status register but only set it if the port is connected,
		// if not set ConnectStatusChange flag; if flag is enabled return 1
		int OHCI_PortSetIfConnected(int i, uint32_t Value);
		// read the HCCA structure in memory
		bool OHCI_ReadHCCA(xbox::addr_xt Paddr, OHCI_HCCA* Hcca);
		// write the HCCA structure in memory
		bool OHCI_WriteHCCA(xbox::addr_xt Paddr, OHCI_HCCA* Hcca);
		// read an ED in memory
		bool OHCI_ReadED(xbox::addr_xt Paddr, OHCI_ED* Ed);
		// write an ED in memory
		bool OHCI_WriteED(xbox::addr_xt Paddr, OHCI_ED* Ed);
		// read an TD in memory
		bool OHCI_ReadTD(xbox::addr_xt Paddr, OHCI_TD* Td);
		// write a TD in memory
		bool OHCI_WriteTD(xbox::addr_xt Paddr, OHCI_TD* Td);
		// read an iso TD in memory
		bool OHCI_ReadIsoTD(xbox::addr_xt Paddr, OHCI_ISO_TD* td);
		// write an iso TD in memory
		bool OHCI_WriteIsoTD(xbox::addr_xt Paddr, OHCI_ISO_TD* td);
		// read/write the user buffer pointed to by a TD from/to main memory
		bool OHCI_CopyTDBuffer(OHCI_TD* Td, uint8_t* Buffer, int Length, bool bIsWrite);
		// read/write the user buffer pointed to by a ISO TD from/to main memory
		bool OHCI_CopyIsoTDBuffer(uint32_t start_addr, uint32_t end_addr, uint8_t* Buffer, int Length, bool bIsWrite);
		// process an ED list. Returns nonzero if active TD was found
		int OHCI_ServiceEDlist(xbox::addr_xt Head, int Completion);
		// process a TD. Returns nonzero to terminate processing of this endpoint
		int OHCI_ServiceTD(OHCI_ED* Ed);
		// process an isochronous TD
		int OHCI_ServiceIsoTD(OHCI_ED* ed, int completion);
		// find the usb device with the supplied address
		XboxDeviceState* OHCI_FindDevice(uint8_t Addr);
		// cancel a packet when a device is removed
		void OHCI_AsyncCancelDevice(XboxDeviceState* dev);
		// Process Control and Bulk lists
		void OHCI_ProcessLists(int completion);
		// see USBPortOps struct for info
		void OHCI_Attach(USBPort* Port);
		void OHCI_Detach(USBPort* Port);
		void OHCI_ChildDetach(XboxDeviceState* child);
		void OHCI_Wakeup(USBPort* port1);
		void OHCI_AsyncCompletePacket(USBPort* port, USBPacket* packet);
};

#endif
