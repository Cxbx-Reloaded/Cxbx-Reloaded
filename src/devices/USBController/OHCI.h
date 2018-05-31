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
// OHCI: Open Host Controller Interface; the standard used on the xbox to comunicate with the usb devices
// HC: Host Controller; the hardware which interfaces with the usb device and the usb driver
// HCD: Host Controller Driver; software which talks to the HC, it's linked in the xbe
// SOF: start of frame; the beginning of a USB-defined frame
// EOF: end of frame; the end of a USB-defined frame
// TD: transfer descriptor; a memory structure used by the HC to transfer a block of data to/from a device endpoint


// These macros are used to access the bits of the various registers
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
// HcCommandStatus
#define OHCI_STATUS_HCR                     (1<<0)           // HostControllerReset
#define OHCI_STATUS_CLF                     (1<<1)           // ControlListFilled
#define OHCI_STATUS_BLF                     (1<<2)           // BulkListFilled
#define OHCI_STATUS_OCR                     (1<<3)           // OwnershipChangeRequest
#define OHCI_STATUS_SOC                     ((1<<6)|(1<<7))  // SchedulingOverrunCount
// HcInterruptStatus
#define OHCI_INTR_SO                        (1<<0)           // SchedulingOverrun
#define OHCI_INTR_WD                        (1<<1)           // WritebackDoneHead
#define OHCI_INTR_SF                        (1<<2)           // StartofFrame
#define OHCI_INTR_RD                        (1<<3)           // ResumeDetected
#define OHCI_INTR_UE                        (1<<4)           // UnrecoverableError
#define OHCI_INTR_FNO                       (1<<5)           // FrameNumberOverflow
#define OHCI_INTR_RHSC                      (1<<6)           // RootHubStatusChange
#define OHCI_INTR_OC                        (1<<30)          // OwnershipChange
// HcInterruptEnable, HcInterruptDisable
#define OHCI_INTR_MIE                       (1<<31)          // MasterInterruptEnable
// HcHCCA
#define OHCI_HCCA_MASK                      0xFFFFFF00       // HCCA mask
// HcControlHeadED
#define OHCI_EDPTR_MASK                     0xFFFFFFF0       // endpoint descriptor mask
// HcFmInterval
#define OHCI_FMI_FI                         0x00003FFF       // FrameInterval
#define OHCI_FMI_FIT                        0x80000000       // FrameIntervalToggle
// HcFmRemaining
#define OHCI_FMR_FR                         0x00003FFF       // FrameRemaining
#define OHCI_FMR_FRT                        0x80000000       // FrameRemainingToggle
// HcRhDescriptorA
#define OHCI_RHA_RW_MASK                    0x00000000       // Mask of supported features
#define OHCI_RHA_PSM                        (1<<8)           // PowerSwitchingMode
#define OHCI_RHA_NPS                        (1<<9)           // NoPowerSwitching
#define OHCI_RHA_DT                         (1<<10)          // DeviceType
#define OHCI_RHA_OCPM                       (1<<11)          // OverCurrentProtectionMode
#define OHCI_RHA_NOCP                       (1<<12)          // NoOverCurrentProtection
// HcRhStatus
#define OHCI_RHS_LPS                        (1<<0)           // LocalPowerStatus
#define OHCI_RHS_OCI                        (1<<1)           // OverCurrentIndicator
#define OHCI_RHS_DRWE                       (1<<15)          // DeviceRemoteWakeupEnable
#define OHCI_RHS_LPSC                       (1<<16)          // LocalPowerStatusChange
#define OHCI_RHS_OCIC                       (1<<17)          // OverCurrentIndicatorChange
#define OHCI_RHS_CRWE                       (1<<31)          // ClearRemoteWakeupEnable
// HcRhPortStatus
#define OHCI_PORT_CCS                       (1<<0)           // CurrentConnectStatus
#define OHCI_PORT_PES                       (1<<1)           // PortEnableStatus
#define OHCI_PORT_PSS                       (1<<2)           // PortSuspendStatus
#define OHCI_PORT_POCI                      (1<<3)           // PortOverCurrentIndicator
#define OHCI_PORT_PRS                       (1<<4)           // PortResetStatus
#define OHCI_PORT_PPS                       (1<<8)           // PortPowerStatus
#define OHCI_PORT_LSDA                      (1<<9)           // LowSpeedDeviceAttached
#define OHCI_PORT_CSC                       (1<<16)          // ConnectStatusChange
#define OHCI_PORT_PESC                      (1<<17)          // PortEnableStatusChange
#define OHCI_PORT_PSSC                      (1<<18)          // PortSuspendStatusChange
#define OHCI_PORT_OCIC                      (1<<19)          // PortOverCurrentIndicatorChange
#define OHCI_PORT_PRSC                      (1<<20)          // PortResetStatusChange
#define OHCI_PORT_WTC                       (OHCI_PORT_CSC|OHCI_PORT_PESC|OHCI_PORT_PSSC \
                                                          |OHCI_PORT_OCIC|OHCI_PORT_PRSC)

// enum indicating the current HC state
typedef enum _OHCI_State
{
	Reset = 0x00,
	Resume = 0x40,
	Operational = 0x80,
	Suspend = 0xC0,
}
OHCI_State;

// Host Controller Communications Area
typedef struct _OHCI_HCCA
{
	uint32_t HccaInterrruptTable[32];
	uint16_t HccaFrameNumber, HccaPad1;
	uint32_t HccaDoneHead;
}
OHCI_HCCA;

// Small struct used to hold the HcRhPortStatus register and the usb port status
typedef struct _OHCIPort
{
	USBPort UsbPort;
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


// OHCI class representing the state of the HC
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
		// usb packet
		USBPacket m_UsbPacket;
		// ergo720: I believe it's the value of HcControl in the last frame
		uint32_t old_ctl;
		// irq number
		int m_IrqNum;
		// ergo720: I think it's the DelayInterrupt flag in a TD
		// -> num of frames to wait before generating an interrupt for this TD
		int m_DoneCount;

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
		void OHCI_SOF();
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
		bool OHCI_ReadHCCA(uint32_t Paddr, OHCI_HCCA* Hcca);
		// write the HCCA structure in memory
		bool OHCI_WriteHCCA(uint32_t Paddr, OHCI_HCCA* Hcca);

		// register a port with the HC
		void USB_RegisterPort(USBPort* Port, int Index, int SpeedMask);
		//
		void USB_DeviceEPstopped(USBDev* Dev, USBEndpoint* EP);
		// reset a usb port
		void USB_PortReset(USBPort* Port);
		// a device is attched
		void USB_Attach(USBPort* port);
		// a device is detached
		void USB_Detach(USBPort* port);
		// a device downstream from the device attached to the port (attached through a hub) is detached
		void ChildDetach(USBPort* port, USBDev* child);
		// TODO
		void Wakeup(USBPort* port);
		// TODO
		void Complete(USBPort* port, USBPacket *p);
		// reset a device
		void USB_DeviceReset(USBDev* dev);
};

#endif
