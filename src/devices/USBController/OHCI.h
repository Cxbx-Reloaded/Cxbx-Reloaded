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

#include <stdint.h>
#include "Cxbx.h"
#include "USBDevice.h"
#include "..\CxbxKrnl\Timer.h"


// Abbreviations used:
// OHCI: Open Host Controller Interface; the standard used on the xbox to comunicate with the usb devices
// HC: Host Controller; the hardware which interfaces with the usb device and the usb driver
// HCD: Host Controller Driver; software which talks to the HC, it's linked in the xbe


// These macros are used to access the bits in the various registers
// HcControl
#define OHCI_CTL_CBSR                       ((1<<0)|(1<<1))  // ControlBulkServiceRatio
#define OHCI_CTL_PLE                        (1<<2)           // PeriodicListEnable
#define OHCI_CTL_IE                         (1<<3)           // IsochronousEnable
#define OHCI_CTL_CLE                        (1<<4)           // ControlListEnable
#define OHCI_CTL_BLE                        (1<<5)           // BulkListEnable
#define OHCI_CTL_HCFS                       ((1<<6)|(1<<7))  // HostControllerFunctionalState
#define OHCI_CTL_IR                         (1<<8)           // InterruptRouting
#define OHCI_CTL_RWC                        (1<<9)           // RemoteWakeupConnected
#define OHCI_CTL_RWE                        (1<<10)          // RemoteWakeupEnable
// HcInterruptEnable, HcInterruptDisable
#define OHCI_INTR_MASTER_INTERRUPT_ENABLED  (1<<31)          // MasterInterruptEnable
// HcRhDescriptorA
#define OHCI_RHA_PSM                        (1<<8)           // PowerSwitchingMode
#define OHCI_RHA_NPS                        (1<<9)           // NoPowerSwitching
#define OHCI_RHA_DT                         (1<<10)          // DeviceType
#define OHCI_RHA_OCPM                       (1<<11)          // OverCurrentProtectionMode
#define OHCI_RHA_NOCP                       (1<<12)          // NoOverCurrentProtection


// enum indicating the current HC state
typedef enum _OHCI_State
{
	Reset = 0x00,
	Resume = 0x40,
	Operational = 0x80,
	Suspend = 0xC0,
}
OHCI_State;

// Small struct used to hold the HcRhPortStatus register and the usb port status
typedef struct _OHCIPort
{
	USBPort Port;
	uint32_t HcRhPortStatus;
}
OHCIPort;

// All these registers are well documented in the OHCI standard
typedef struct _OHCI_Registers
{
	// Control and Status partition
	uint32_t HcRevision;
	uint32_t HcControl;
	uint32_t HcCommandStatus;
	uint32_t HcInterruptStatus;
	uint32_t HcInterruptEnable;
	uint32_t HcInterruptDisable;

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
	OHCIPort RhPort[2]; // 2 ports per HC, for a total of 4 USB ports
}
OHCI_Registers;


/* OHCI class representing the state of the HC */
class OHCI
{
	public:
		// constructor
		OHCI(USBDevice* UsbObj);
		// destructor
		~OHCI() {}
		// read a register
		uint32_t OHCI_ReadRegister(xbaddr Addr);
		// write a register
		void OHCI_WriteRegister(xbaddr Addr, uint32_t Value);
		// switch the HC to the reset state
		void OHCI_StateReset();


	private:
		// all the registers available on the OHCI standard
		OHCI_Registers Registers;
		// end-of-frame timer
		TimerObject* pEndOfFrameTimer;
		// the duration of a usb frame
		uint64_t UsbFrameTime;
		// ticks per usb tick
		uint64_t TicksPerUsbTick;
		// the usb device instance of this HC
		USBDevice* UsbInstance;
		// usb packet
		USBPacket UsbPacket;

		// end-of-frame callback wrapper
		static void OHCI_FrameBoundaryWrapper(void* pVoid);
		// end-of-frame callback function
		void OHCI_FrameBoundaryWorker();
		// initialize packet struct
		void USB_PacketInit(USBPacket* packet);
};

extern OHCI* g_pHostController1;
extern OHCI* g_pHostController2;

#endif
