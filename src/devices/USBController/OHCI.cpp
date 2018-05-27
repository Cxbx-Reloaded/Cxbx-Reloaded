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
// *   Cxbx->devices->USBController->OHCI.cpp
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

#include "OHCI.h"
#include "CxbxKrnl\EmuKrnl.h"  // For HalSystemInterrupt

#define USB_HZ 12000000


typedef enum _USB_SPEED
{
	USB_SPEED_MASK_LOW =  1 << 0,
	USB_SPEED_MASK_FULL = 1 << 1,
}
USB_SPEED;

// global pointers to the two USB host controllers available on the Xbox
OHCI* g_pHostController1 = nullptr;
OHCI* g_pHostController2 = nullptr;


OHCI::OHCI(USBDevice* UsbObj, int Irq)
{
	UsbInstance = UsbObj;
	Irq_n = Irq;

	for (int i = 0; i < 2; i++) {
		UsbInstance->USB_RegisterPort(&Registers.RhPort[i].Port, this, i, USB_SPEED_MASK_LOW | USB_SPEED_MASK_FULL);
	}
	OHCI_PacketInit(&UsbPacket);

	UsbFrameTime = 1000000ULL; // 1 ms
	TicksPerUsbTick = 1000000000ULL / USB_HZ; // 83

	// Do a hardware reset
	OHCI_StateReset();
}

void OHCI::OHCI_FrameBoundaryWrapper(void* pVoid)
{
	static_cast<OHCI*>(pVoid)->OHCI_FrameBoundaryWorker();
}

void OHCI::OHCI_StateReset()
{
	// The usb state can be USB_Suspend if it is a software reset, and USB_Reset if it is a hardware
	// reset or cold boot

	// TODO: stop all the list processing here

	// Reset all registers
	// Remark: the standard says that RemoteWakeupConnected bit should be set during POST, cleared during hw reset
	// and ignored during a sw reset. However, VBox sets it on hw reset and XQEMU clears it. Considering that the Xbox
	// doesn't do POST, I will clear it.
	Registers.HcRevision = 0x10;
	Registers.HcControl = 0;
	Registers.HcControl &= ~OHCI_CTL_HCFS;
	Registers.HcControl |= Reset;
	Registers.HcCommandStatus = 0;
	Registers.HcInterruptStatus = 0;
	Registers.HcInterrupt = OHCI_INTR_MIE; // enable interrupts

	Registers.HcHCCA = 0;
	Registers.HcPeriodCurrentED = 0;
	Registers.HcControlHeadED = Registers.HcControlCurrentED = 0;
	Registers.HcBulkHeadED = Registers.HcBulkCurrentED = 0;
	Registers.HcDoneHead = 0;

	Registers.HcFmInterval = 0;
	Registers.HcFmInterval |= (0x2778 << 16); // TBD according to the standard, using what XQEMU sets (FSLargestDataPacket)
	Registers.HcFmInterval |= 0x2EDF; // bit-time of a frame. 1 frame = 1 ms (FrameInterval)
	Registers.HcFmRemaining = 0;
	Registers.HcFmNumber = 0;
	Registers.HcPeriodicStart = 0;

	Registers.HcRhDescriptorA = OHCI_RHA_NPS | 2; // The xbox lacks the hw to switch off the power on the ports and has 2 ports per HC
	Registers.HcRhDescriptorB = 0; // The attached devices are removable and use PowerSwitchingMode to control the power on the ports
	for (int i = 0; i < 2; i++)
	{
		OHCIPort* Port = &Registers.RhPort[i];
		Port->HcRhPortStatus = 0;
		//if (Port->port.device && Port->port.device->attached) {
			//usb_port_reset(&Port->port);
		//}
	}

	DbgPrintf("Ohci: Reset event.\n");
}

void OHCI::OHCI_BusStart()
{
	// Create the end-of-frame timer. Let's try a factor of 50 (1 virtual ms -> 50 real ms)
	pEOFtimer = Timer_Create(OHCI_FrameBoundaryWrapper, this, 50);

	DbgPrintf("Ohci: Operational event\n");

	// SOF event
	OHCI_SOF();
}

void OHCI::OHCI_BusStop()
{
	if (pEOFtimer) {
		// Delete existing EOF timer
		Timer_Exit(pEOFtimer);
	}
	pEOFtimer = nullptr;
}

void OHCI::OHCI_SOF()
{
	SOFtime = GetTime_NS(pEOFtimer); // set current SOF time
	Timer_Start(pEOFtimer, SOFtime + UsbFrameTime); // make timer expire at SOF + 1 virtual ms from now
	OHCI_SetInterrupt(OHCI_INTR_SF);
}

void OHCI::OHCI_ChangeState(uint32_t Value)
{
	uint32_t OldState = Registers.HcControl & OHCI_CTL_HCFS;
	Registers.HcControl = Value;
	uint32_t NewState = Registers.HcControl & OHCI_CTL_HCFS;

	// no state change
	if (OldState == NewState) {
		return;
	}

	switch (NewState)
	{
		case Operational:
			OHCI_BusStart();
			break;

		case Suspend:
			OHCI_BusStop();
			DbgPrintf("Ohci: Suspend event\n");
			break;

		case Resume:
			DbgPrintf("Ohci: Resume event\n");
			break;

		case Reset:
			OHCI_StateReset();
			break;

		default:
			EmuWarning("Ohci: Unknown USB state mode!");
	}
}

void OHCI::OHCI_PacketInit(USBPacket* packet)
{
	IOVector* vec = &packet->IoVec;
	vec->IoVec = new IoVec;
	vec->IoVecNumber = 0;
	vec->AllocNumber = 1;
	vec->Size = 0;
}

uint32_t OHCI::OHCI_ReadRegister(xbaddr Addr)
{
	uint32_t ret = 0xFFFFFFFF;

	if (Addr & 3) {
		// The standard allows only aligned reads to the registers
		EmuWarning("Ohci: Unaligned read. Ignoring.");
		return ret;
	}
	else {
		switch (Addr >> 2) // read the register
		{
			case 0: // HcRevision
				ret = Registers.HcRevision;
				break;

			case 1: // HcControl
				ret = Registers.HcControl;
				break;

			case 2: // HcCommandStatus
				ret = Registers.HcCommandStatus;
				break;

			case 3: // HcInterruptStatus
				ret = Registers.HcInterruptStatus;
				break;

			case 4: // HcInterruptEnable
			case 5: // HcInterruptDisable
				ret = Registers.HcInterrupt;
				break;

			case 6: // HcHCCA
				ret = Registers.HcHCCA;
				break;

			case 7: // HcPeriodCurrentED
				ret = Registers.HcPeriodCurrentED;
				break;

			case 8: // HcControlHeadED
				ret = Registers.HcControlHeadED;
				break;

			case 9: // HcControlCurrentED
				ret = Registers.HcControlCurrentED;
				break;

			case 10: // HcBulkHeadED
				ret = Registers.HcBulkHeadED;
				break;

			case 11: // HcBulkCurrentED
				ret = Registers.HcBulkCurrentED;
				break;

			case 12: // HcDoneHead
				ret = Registers.HcDoneHead;
				break;

			case 13: // HcFmInterval
				ret = Registers.HcFmInterval;
				break;

			case 14: // HcFmRemaining
				// TODO
				break;

			case 15: // HcFmNumber
				ret = Registers.HcFmNumber;
				break;

			case 16: // HcPeriodicStart
				ret = Registers.HcPeriodicStart;
				break;

			case 17: // HcLSThreshold
				ret = Registers.HcLSThreshold;
				break;

			case 18: // HcRhDescriptorA
				ret = Registers.HcRhDescriptorA;
				break;

			case 19: // HcRhDescriptorB
				ret = Registers.HcRhDescriptorB;
				break;

			case 20: // HcRhStatus
				ret = Registers.HcRhStatus;
				break;

			// Always report that the port power is on since the Xbox cannot switch off the electrical current to it
			case 21: // RhPort 0
				ret = Registers.RhPort[0].HcRhPortStatus | OHCI_PORT_PPS;
				break;

			case 22: // RhPort 1
				ret = Registers.RhPort[1].HcRhPortStatus | OHCI_PORT_PPS;
				break;

			default:
				EmuWarning("Ohci: Read register operation with bad offset %u. Ignoring.", Addr >> 2);
		}
		return ret;
	}
}

void OHCI::OHCI_WriteRegister(xbaddr Addr, uint32_t Value)
{
	if (Addr & 3) {
		// The standard allows only aligned writes to the registers
		EmuWarning("Ohci: Unaligned write. Ignoring.");
		return;
	}
	else {
		switch (Addr >> 2)
		{
			case 0: // HcRevision
				// This register is read-only
				break;

			case 1: // HcControl
				OHCI_ChangeState(Value);
				break;

			case 2: // HcCommandStatus
			{
				// SOC is read-only
				Value &= ~OHCI_STATUS_SOC;

				// From the standard: "The Host Controller must ensure that bits written as 1 become set
				// in the register while bits written as 0 remain unchanged in the register."
				Registers.HcCommandStatus |= Value;

				if (Registers.HcCommandStatus & OHCI_STATUS_HCR) {
					// Do a hardware reset
					OHCI_StateReset();
				}
			}
			break;

			case 3: // HcInterruptStatus
				Registers.HcInterruptStatus &= ~Value;
				OHCI_UpdateInterrupt();
				break;

			case 4: // HcInterruptEnable
				Registers.HcInterrupt |= Value;
				OHCI_UpdateInterrupt();
				break;

			case 5: // HcInterruptDisable
				Registers.HcInterrupt &= ~Value;
				OHCI_UpdateInterrupt();
				break;

			case 6: // HcHCCA
				// The standard says the minimum alignment is 256 bytes and so bits 0 through 7 are always zero
				Registers.HcHCCA = Value & OHCI_HCCA_MASK;
				break;

			case 7: // HcPeriodCurrentED
				// This register is read-only
				break;

			case 8: // HcControlHeadED
				Registers.HcControlHeadED = Value & OHCI_EDPTR_MASK;
				break;

			case 9: // HcControlCurrentED
				Registers.HcControlCurrentED = Value & OHCI_EDPTR_MASK;
				break;

			case 10: // HcBulkHeadED
				Registers.HcBulkHeadED = Value & OHCI_EDPTR_MASK;
				break;

			case 11: // HcBulkCurrentED
				Registers.HcBulkCurrentED = Value & OHCI_EDPTR_MASK;
				break;

			case 12: // HcDoneHead
				// This register is read-only
				break;

			case 13: // HcFmInterval
			{
				if ((Value & OHCI_FMI_FIT) != (Registers.HcFmInterval & OHCI_FMI_FIT)) {
					DbgPrintf("Ohci: Changing frame interval duration. New value is %u\n", Value & OHCI_FMI_FI);
				}
				Registers.HcFmInterval = Value & ~0xC000;
			}
			break;

			case 14: // HcFmRemaining
				// This register is read-only
				break;

			case 15: // HcFmNumber
				// This register is read-only
				break;

			case 16: // HcPeriodicStart
				Registers.HcPeriodicStart = Value & 0x3FFF;
				break;

			case 17: // HcLSThreshold
				Registers.HcLSThreshold = Value & 0xFFF;
				break;

			case 18: // HcRhDescriptorA
				Registers.HcRhDescriptorA &= ~OHCI_RHA_RW_MASK;
				Registers.HcRhDescriptorA |= Value & OHCI_RHA_RW_MASK; // ??
				break;

			case 19: // HcRhDescriptorB
				// Don't do anything, the attached devices are all removable and PowerSwitchingMode is always 0
				break;

			case 20: // HcRhStatus
				// TODO
				break;

			case 21: // RhPort 0
				// TODO
				break;

			case 22: // RhPort 1
				// TODO
				break;

			default:
				EmuWarning("Ohci: Write register operation with bad offset %u. Ignoring.", Addr >> 2);
		}
	}
}

void OHCI::OHCI_UpdateInterrupt()
{
	if ((Registers.HcInterrupt & OHCI_INTR_MIE) && (Registers.HcInterruptStatus & Registers.HcInterrupt)) {
		HalSystemInterrupts[Irq_n].Assert(true);
	}
	else { HalSystemInterrupts[Irq_n].Assert(false); }
}

void OHCI::OHCI_SetInterrupt(uint32_t Value)
{
	Registers.HcInterruptStatus |= Value;
	OHCI_UpdateInterrupt();
}
