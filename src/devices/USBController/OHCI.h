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
// *   Cxbx->devices->USBController->OHCI.h
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

#include "Cxbx.h"
#include "USBDevice.h"
#include "..\CxbxKrnl\Timer.h"


// Abbreviations used:
// OHCI: Open Host Controller Interface; the standard used on the Xbox to comunicate with the usb devices
// HC: Host Controller; the hardware which interfaces with the usb device and the usb driver
// HCD: Host Controller Driver; software which talks to the HC, it's linked in the xbe
// SOF: start of frame; the beginning of a USB-defined frame
// EOF: end of frame; the end of a USB-defined frame
// ED: endpoint descriptor; a memory structure used by the HC to communicate with an endpoint
// TD: transfer descriptor; a memory structure used by the HC to transfer a block of data to/from a device endpoint


/* endpoint descriptor */
typedef struct _OHCI_ED {
	uint32_t Flags;
	uint32_t TailP;
	uint32_t HeadP;
	uint32_t NextED;
}
OHCI_ED;

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
typedef struct _OHCI_HCCA
{
	uint32_t HccaInterrruptTable[32];
	uint16_t HccaFrameNumber, HccaPad1;
	uint32_t HccaDoneHead;
}
OHCI_HCCA;

/* Small struct used to hold the HcRhPortStatus register and the usb port status */
typedef struct _OHCIPort
{
	USBPort UsbPort;
	uint32_t HcRhPortStatus;
}
OHCIPort;

/* All these registers are well documented in the OHCI standard */
typedef struct _OHCI_Registers
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
	// ergo720: I have some doubts here. Both XQEMU and OpenXbox set 4 ports per HC, for a total of 8 usb ports.
	// Could it be becasue each gamepad can host 2 memory units?
	OHCIPort RhPort[2]; // 2 ports per HC, for a total of 4 USB ports
}
OHCI_Registers;


/* OHCI class representing the state of the HC */
class OHCI
{
	public:
		// constructor
		OHCI(int Irqn);
		// destructor
		~OHCI() {}
		// read a register
		uint32_t OHCI_ReadRegister(xbaddr Addr);
		// write a register
		void OHCI_WriteRegister(xbaddr Addr, uint32_t Value);


	private:
		// all the registers available on the OHCI standard
		OHCI_Registers m_Registers;
		// end-of-frame timer
		TimerObject* m_pEOFtimer;
		// time at which a SOF was sent
		uint64_t m_SOFtime;
		// the duration of a usb frame
		uint64_t m_UsbFrameTime;
		// ticks per usb tick
		uint64_t m_TicksPerUsbTick;
		// pending usb packet to process
		USBPacket m_UsbPacket;
		// ergo720: I believe it's the value of HcControl in the last frame
		uint32_t old_ctl;
		// irq number
		int m_IrqNum;
		// ergo720: I think it's the DelayInterrupt flag in a TD
		// -> num of frames to wait before generating an interrupt for this TD
		int m_DoneCount;
		// the address of the pending TD
		xbaddr AsyncTD;

		// EOF callback wrapper
		static void OHCI_FrameBoundaryWrapper(void* pVoid);
		// EOF callback function
		void OHCI_FrameBoundaryWorker();
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
		// generate a SOF event, and start a timer for EOF
		void OHCI_SOF(bool bCreate);
		// change interrupt status
		void OHCI_UpdateInterrupt();
		// fire an interrupt
		void OHCI_SetInterrupt(uint32_t Value);
		// calculate frame time remaining
		uint32_t OHCI_GetFrameRemaining();
		//
		void OHCI_StopEndpoints();
		// update ohci registers during a device attach
		void OHCI_Attach(USBPort* Port);
		// update ohci registers during a device detach
		void OHCI_Detach(USBPort* Port);
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
		bool OHCI_ReadHCCA(xbaddr Paddr, OHCI_HCCA* Hcca);
		// write the HCCA structure in memory
		bool OHCI_WriteHCCA(xbaddr Paddr, OHCI_HCCA* Hcca);
		// read an ED in memory
		bool OHCI_ReadED(xbaddr Paddr, OHCI_ED* Ed);
		// write an ED in memory
		bool OHCI_WriteED(xbaddr Paddr, OHCI_ED* Ed);
		// read an array of DWORDs in memory
		bool OHCI_GetDwords(xbaddr Paddr, uint32_t* Buffer, int Number);
		// write an array of DWORDs in memory
		bool OHCI_WriteDwords(xbaddr Paddr, uint32_t* Buffer, int Number);
		//
		int OHCI_ServiceEDlist(xbaddr Head, int Completion);

		// register a port with the HC
		void USB_RegisterPort(USBPort* Port, int Index, int SpeedMask);
		//
		void USB_DeviceEPstopped(USBDev* Dev, USBEndpoint* Ep);
		// reset a usb port
		void USB_PortReset(USBPort* Port);
		// a device is attched
		void USB_Attach(USBPort* Port);
		// a device is detached
		void USB_Detach(USBPort* Port);
		// a device downstream from the device attached to the port (attached through a hub) is detached
		void ChildDetach(USBPort* Port, USBDev* Child);
		// TODO
		void Wakeup(USBPort* Port);
		// TODO
		void Complete(USBPort* Port, USBPacket *P);
		// reset a device
		void USB_DeviceReset(USBDev* Dev);
};

#endif
