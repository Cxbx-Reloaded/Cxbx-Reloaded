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

#define USB_SPEED_LOW   0
#define USB_SPEED_FULL  1


typedef enum _USB_SPEED
{
	USB_SPEED_MASK_LOW =  1 << 0,
	USB_SPEED_MASK_FULL = 1 << 1,
}
USB_SPEED;


OHCI::OHCI(int Irq)
{
	m_IrqNum = Irq;

	for (int i = 0; i < 2; i++) {
		USB_RegisterPort(&m_Registers.RhPort[i].UsbPort, i, USB_SPEED_MASK_LOW | USB_SPEED_MASK_FULL);
	}
	OHCI_PacketInit(&m_UsbPacket);

	m_UsbFrameTime = 1000000ULL; // 1 ms
	m_TicksPerUsbTick = 1000000000ULL / USB_HZ; // 83

	// Do a hardware reset
	OHCI_StateReset();
}

void OHCI::OHCI_FrameBoundaryWrapper(void* pVoid)
{
	static_cast<OHCI*>(pVoid)->OHCI_FrameBoundaryWorker();
}

void OHCI::OHCI_FrameBoundaryWorker()
{
	OHCI_HCCA hcca;

	if (OHCI_ReadHCCA(m_Registers.HcHCCA, &hcca)) {
		EmuWarning("Ohci: HCCA read error at physical address 0x%X", m_Registers.HcHCCA);
		OHCI_FatalError();
		return;
	}

	// Process all the lists at the end of the frame
	if (m_Registers.HcControl & OHCI_CTL_PLE) {
		int n = m_Registers.HcFmNumber & 0x1f;
		ohci_service_ed_list(ohci, le32_to_cpu(hcca.intr[n]), 0);
	}

	// Cancel all pending packets if either of the lists has been disabled
	if (ohci->old_ctl & (~ohci->ctl) & (OHCI_CTL_BLE | OHCI_CTL_CLE)) {
		if (ohci->async_td) {
			usb_cancel_packet(&ohci->usb_packet);
			ohci->async_td = 0;
		}
		OHCI_StopEndpoints();
	}
	ohci->old_ctl = ohci->ctl;
	ohci_process_lists(ohci, 0);

	// Stop if UnrecoverableError happened or OHCI_SOF will crash
	if (m_Registers.HcInterruptStatus & OHCI_INTR_UE) {
		return;
	}

	// From the standard: "This bit is loaded from the FrameIntervalToggle field of
	// HcFmInterval whenever FrameRemaining reaches 0."
	m_Registers.HcFmRemaining = (m_Registers.HcFmRemaining & ~OHCI_FMR_FRT) | (m_Registers.HcFmInterval & OHCI_FMI_FIT);

	// Increment frame number
	m_Registers.HcFmNumber = (m_Registers.HcFmNumber + 1) & 0xFFFF; // prevent overflow
	hcca.HccaFrameNumber = m_Registers.HcFmNumber; // dropped big -> little endian conversion from XQEMU

	if (m_DoneCount == 0 && !(m_Registers.HcInterruptStatus & OHCI_INTR_WD)) {
		if (!m_Registers.HcDoneHead) {
			// From the standard: "This is set to zero whenever HC writes the content of this
			// register to HCCA. It also sets the WritebackDoneHead of HcInterruptStatus."
			CxbxKrnlCleanup("Ohci: HcDoneHead is zero but WritebackDoneHead interrupt is not set!\n");
		}

		if (m_Registers.HcInterrupt & m_Registers.HcInterruptStatus) {
			// From the standard: "The least significant bit of this entry is set to 1 to indicate whether an
			// unmasked HcInterruptStatus was set when HccaDoneHead was written." It's tecnically incorrect to
			// do this to HcDoneHead instead of HccaDoneHead however it doesn't matter since HcDoneHead is
			// zeroed below
			m_Registers.HcDoneHead |= 1;
		}

		hcca.HccaDoneHead = m_Registers.HcDoneHead; // dropped big -> little endian conversion from XQEMU
		m_Registers.HcDoneHead = 0;
		m_DoneCount = 7;
		OHCI_SetInterrupt(OHCI_INTR_WD);
	}

	if (m_DoneCount != 7 && m_DoneCount != 0) {
		// decrease DelayInterrupt counter
		m_DoneCount--;
	}

	// Do SOF stuff here
	OHCI_SOF();

	// Writeback HCCA
	if (OHCI_WriteHCCA(m_Registers.HcHCCA, &hcca)) {
		EmuWarning("Ohci: HCCA write error at physical address 0x%X", m_Registers.HcHCCA);
		OHCI_FatalError();
	}
}

void OHCI::OHCI_FatalError()
{
	// According to the standard, an OHCI will stop operating, and set itself into error state
	// (which can be queried by MMIO). Instead of calling directly CxbxKrnlCleanup, we let the
	// HCD know the problem so it can try to solve it

	OHCI_SetInterrupt(OHCI_INTR_UE);
	OHCI_BusStop();
}

bool OHCI::OHCI_ReadHCCA(xbaddr Paddr, OHCI_HCCA* Hcca)
{
	// ergo720: I disassembled various xbe's of my games and discovered that the shared memory between
	// HCD and HC is allocated with MmAllocateContiguousMemory which means we can access it from
	// the contiguous region. Hopefully XDK revisions didn't alter this...

	if (Paddr != xbnull) {
		std::memcpy(Hcca, reinterpret_cast<void*>(Paddr + CONTIGUOUS_MEMORY_BASE), sizeof(OHCI_HCCA));
		return false;
	}

	return true; // error
}

bool OHCI::OHCI_WriteHCCA(xbaddr Paddr, OHCI_HCCA* Hcca)
{
	if (Paddr != xbnull) {
		// We need to calculate the offset of the HccaFrameNumber member to avoid overwriting HccaInterrruptTable
		size_t OffsetOfFrameNumber = offsetof(OHCI_HCCA, HccaFrameNumber);

		std::memcpy(reinterpret_cast<void*>(Paddr + OffsetOfFrameNumber + CONTIGUOUS_MEMORY_BASE),
			reinterpret_cast<uint8_t*>(Hcca) + OffsetOfFrameNumber, 8);
		return false;
	}

	return true; // error
}

void OHCI::OHCI_StateReset()
{
	// The usb state can be USB_Suspend if it is a software reset, and USB_Reset if it is a hardware
	// reset or cold boot

	OHCI_BusStop();

	// Reset all registers
	// Remark: the standard says that RemoteWakeupConnected bit should be set during POST, cleared during hw reset
	// and ignored during a sw reset. However, VBox sets it on hw reset and XQEMU clears it. Considering that the Xbox
	// doesn't do POST, I will clear it.
	m_Registers.HcRevision = 0x10;
	m_Registers.HcControl = 0;
	m_Registers.HcControl &= ~OHCI_CTL_HCFS;
	m_Registers.HcControl |= Reset;
	m_Registers.HcCommandStatus = 0;
	m_Registers.HcInterruptStatus = 0;
	m_Registers.HcInterrupt = OHCI_INTR_MIE; // enable interrupts

	m_Registers.HcHCCA = 0;
	m_Registers.HcPeriodCurrentED = 0;
	m_Registers.HcControlHeadED = m_Registers.HcControlCurrentED = 0;
	m_Registers.HcBulkHeadED = m_Registers.HcBulkCurrentED = 0;
	m_Registers.HcDoneHead = 0;

	m_Registers.HcFmInterval = 0;
	m_Registers.HcFmInterval |= (0x2778 << 16); // TBD according to the standard, using what XQEMU sets (FSLargestDataPacket)
	m_Registers.HcFmInterval |= 0x2EDF; // bit-time of a frame. 1 frame = 1 ms (FrameInterval)
	m_Registers.HcFmRemaining = 0;
	m_Registers.HcFmNumber = 0;
	m_Registers.HcPeriodicStart = 0;

	m_Registers.HcRhDescriptorA = OHCI_RHA_NPS | 2; // The xbox lacks the hw to switch off the power on the ports and has 2 ports per HC
	m_Registers.HcRhDescriptorB = 0; // The attached devices are removable and use PowerSwitchingMode to control the power on the ports
	for (int i = 0; i < 2; i++)
	{
		OHCIPort* Port = &m_Registers.RhPort[i];
		Port->HcRhPortStatus = 0;
		if (Port->UsbPort.Dev && Port->UsbPort.Dev->Attached) {
			USB_PortReset(&Port->UsbPort);
		}
	}
	m_DoneCount = 7;

	OHCI_StopEndpoints();

	DbgPrintf("Ohci: Reset mode event.\n");
}

void OHCI::OHCI_BusStart()
{
	// Create the EOF timer. Let's try a factor of 50 (1 virtual ms -> 50 real ms)
	m_pEOFtimer = Timer_Create(OHCI_FrameBoundaryWrapper, this, 50);

	DbgPrintf("Ohci: Operational mode event\n");

	// SOF event
	OHCI_SOF();
}

void OHCI::OHCI_BusStop()
{
	if (m_pEOFtimer) {
		// Delete existing EOF timer
		Timer_Exit(m_pEOFtimer);
	}
	m_pEOFtimer = nullptr;
}

void OHCI::OHCI_SOF()
{
	m_SOFtime = GetTime_NS(m_pEOFtimer); // set current SOF time
	Timer_Start(m_pEOFtimer, m_SOFtime + m_UsbFrameTime); // make timer expire at SOF + 1 virtual ms from now
	OHCI_SetInterrupt(OHCI_INTR_SF);
}

void OHCI::OHCI_ChangeState(uint32_t Value)
{
	uint32_t OldState = m_Registers.HcControl & OHCI_CTL_HCFS;
	m_Registers.HcControl = Value;
	uint32_t NewState = m_Registers.HcControl & OHCI_CTL_HCFS;

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
			DbgPrintf("Ohci: Suspend mode event\n");
			break;

		case Resume:
			DbgPrintf("Ohci: Resume mode event\n");
			break;

		case Reset:
			OHCI_StateReset();
			break;

		default:
			EmuWarning("Ohci: Unknown USB mode!");
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
		DbgPrintf("Ohci: Unaligned read. Ignoring.");
		return ret;
	}
	else {
		switch (Addr >> 2) // read the register
		{
			case 0: // HcRevision
				ret = m_Registers.HcRevision;
				break;

			case 1: // HcControl
				ret = m_Registers.HcControl;
				break;

			case 2: // HcCommandStatus
				ret = m_Registers.HcCommandStatus;
				break;

			case 3: // HcInterruptStatus
				ret = m_Registers.HcInterruptStatus;
				break;

			case 4: // HcInterruptEnable
			case 5: // HcInterruptDisable
				ret = m_Registers.HcInterrupt;
				break;

			case 6: // HcHCCA
				ret = m_Registers.HcHCCA;
				break;

			case 7: // HcPeriodCurrentED
				ret = m_Registers.HcPeriodCurrentED;
				break;

			case 8: // HcControlHeadED
				ret = m_Registers.HcControlHeadED;
				break;

			case 9: // HcControlCurrentED
				ret = m_Registers.HcControlCurrentED;
				break;

			case 10: // HcBulkHeadED
				ret = m_Registers.HcBulkHeadED;
				break;

			case 11: // HcBulkCurrentED
				ret = m_Registers.HcBulkCurrentED;
				break;

			case 12: // HcDoneHead
				ret = m_Registers.HcDoneHead;
				break;

			case 13: // HcFmInterval
				ret = m_Registers.HcFmInterval;
				break;

			case 14: // HcFmRemaining
				ret = OHCI_GetFrameRemaining();
				break;

			case 15: // HcFmNumber
				ret = m_Registers.HcFmNumber;
				break;

			case 16: // HcPeriodicStart
				ret = m_Registers.HcPeriodicStart;
				break;

			case 17: // HcLSThreshold
				ret = m_Registers.HcLSThreshold;
				break;

			case 18: // HcRhDescriptorA
				ret = m_Registers.HcRhDescriptorA;
				break;

			case 19: // HcRhDescriptorB
				ret = m_Registers.HcRhDescriptorB;
				break;

			case 20: // HcRhStatus
				ret = m_Registers.HcRhStatus;
				break;

			// Always report that the port power is on since the Xbox cannot switch off the electrical current to it
			case 21: // RhPort 0
				ret = m_Registers.RhPort[0].HcRhPortStatus | OHCI_PORT_PPS;
				break;

			case 22: // RhPort 1
				ret = m_Registers.RhPort[1].HcRhPortStatus | OHCI_PORT_PPS;
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
		DbgPrintf("Ohci: Unaligned write. Ignoring.");
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
				m_Registers.HcCommandStatus |= Value;

				if (m_Registers.HcCommandStatus & OHCI_STATUS_HCR) {
					// Do a hardware reset
					OHCI_StateReset();
				}
			}
			break;

			case 3: // HcInterruptStatus
				m_Registers.HcInterruptStatus &= ~Value;
				OHCI_UpdateInterrupt();
				break;

			case 4: // HcInterruptEnable
				m_Registers.HcInterrupt |= Value;
				OHCI_UpdateInterrupt();
				break;

			case 5: // HcInterruptDisable
				m_Registers.HcInterrupt &= ~Value;
				OHCI_UpdateInterrupt();
				break;

			case 6: // HcHCCA
				// The standard says the minimum alignment is 256 bytes and so bits 0 through 7 are always zero
				m_Registers.HcHCCA = Value & OHCI_HCCA_MASK;
				break;

			case 7: // HcPeriodCurrentED
				// This register is read-only
				break;

			case 8: // HcControlHeadED
				m_Registers.HcControlHeadED = Value & OHCI_EDPTR_MASK;
				break;

			case 9: // HcControlCurrentED
				m_Registers.HcControlCurrentED = Value & OHCI_EDPTR_MASK;
				break;

			case 10: // HcBulkHeadED
				m_Registers.HcBulkHeadED = Value & OHCI_EDPTR_MASK;
				break;

			case 11: // HcBulkCurrentED
				m_Registers.HcBulkCurrentED = Value & OHCI_EDPTR_MASK;
				break;

			case 12: // HcDoneHead
				// This register is read-only
				break;

			case 13: // HcFmInterval
			{
				if ((Value & OHCI_FMI_FIT) != (m_Registers.HcFmInterval & OHCI_FMI_FIT)) {
					DbgPrintf("Ohci: Changing frame interval duration. New value is %u\n", Value & OHCI_FMI_FI);
				}
				m_Registers.HcFmInterval = Value & ~0xC000;
			}
			break;

			case 14: // HcFmRemaining
				// This register is read-only
				break;

			case 15: // HcFmNumber
				// This register is read-only
				break;

			case 16: // HcPeriodicStart
				m_Registers.HcPeriodicStart = Value & 0x3FFF;
				break;

			case 17: // HcLSThreshold
				m_Registers.HcLSThreshold = Value & 0xFFF;
				break;

			case 18: // HcRhDescriptorA
				m_Registers.HcRhDescriptorA &= ~OHCI_RHA_RW_MASK;
				m_Registers.HcRhDescriptorA |= Value & OHCI_RHA_RW_MASK; // ??
				break;

			case 19: // HcRhDescriptorB
				// Don't do anything, the attached devices are all removable and PowerSwitchingMode is always 0
				break;

			case 20: // HcRhStatus
				OHCI_SetHubStatus(Value);
				break;

			case 21: // RhPort 0
				OHCI_PortSetStatus(0, Value);
				break;

			case 22: // RhPort 1
				OHCI_PortSetStatus(1, Value);
				break;

			default:
				EmuWarning("Ohci: Write register operation with bad offset %u. Ignoring.", Addr >> 2);
		}
	}
}

void OHCI::OHCI_UpdateInterrupt()
{
	if ((m_Registers.HcInterrupt & OHCI_INTR_MIE) && (m_Registers.HcInterruptStatus & m_Registers.HcInterrupt)) {
		HalSystemInterrupts[m_IrqNum].Assert(true);
	}
	else { HalSystemInterrupts[m_IrqNum].Assert(false); }
}

void OHCI::OHCI_SetInterrupt(uint32_t Value)
{
	m_Registers.HcInterruptStatus |= Value;
	OHCI_UpdateInterrupt();
}

uint32_t OHCI::OHCI_GetFrameRemaining()
{
	uint16_t frame;
	uint64_t ticks;

	if ((m_Registers.HcControl & OHCI_CTL_HCFS) != Operational) {
		return m_Registers.HcFmRemaining & OHCI_FMR_FRT;
	}

	// Being in USB operational state guarantees that m_pEOFtimer and m_SOFtime were set already
	ticks = GetTime_NS(m_pEOFtimer) - m_SOFtime;

	// Avoid Muldiv64 if possible
	if (ticks >= m_UsbFrameTime) {
		return m_Registers.HcFmRemaining & OHCI_FMR_FRT;
	}

	ticks = Muldiv64(1, ticks, m_TicksPerUsbTick);
	frame = static_cast<uint16_t>((m_Registers.HcFmInterval & OHCI_FMI_FI) - ticks);

	return (m_Registers.HcFmRemaining & OHCI_FMR_FRT) | frame;
}

void OHCI::OHCI_StopEndpoints()
{
	USBDev* dev;
	int i, j;

	for (i = 0; i < 2; i++) {
		dev = m_Registers.RhPort[i].UsbPort.Dev;
		if (dev && dev->Attached) {
			USB_DeviceEPstopped(dev, &dev->EP_ctl);
			for (j = 0; j < USB_MAX_ENDPOINTS; j++) {
				USB_DeviceEPstopped(dev, &dev->EP_in[j]);
				USB_DeviceEPstopped(dev, &dev->EP_out[j]);
			}
		}
	}
}

void OHCI::OHCI_SetHubStatus(uint32_t Value)
{
	uint32_t old_state;

	old_state = m_Registers.HcRhStatus;

	// write 1 to clear OCIC
	if (Value & OHCI_RHS_OCIC) {
		m_Registers.HcRhStatus &= ~OHCI_RHS_OCIC;
	}

	if (Value & OHCI_RHS_LPS) {
		int i;

		for (i = 0; i < 2; i++) {
			OHCI_PortPower(i, 0);
		}	
		DbgPrintf("Ohci: powered down all ports\n");
	}

	if (Value & OHCI_RHS_LPSC) {
		int i;

		for (i = 0; i < 2; i++) {
			OHCI_PortPower(i, 1);
		}	
		DbgPrintf("Ohci: powered up all ports\n");
	}

	if (Value & OHCI_RHS_DRWE) {
		m_Registers.HcRhStatus |= OHCI_RHS_DRWE;
	}

	if (Value & OHCI_RHS_CRWE) {
		m_Registers.HcRhStatus &= ~OHCI_RHS_DRWE;
	}

	if (old_state != m_Registers.HcRhStatus) {
		OHCI_SetInterrupt(OHCI_INTR_RHSC);
	}	
}

void OHCI::OHCI_PortPower(int i, int p)
{
	if (p) {
		m_Registers.RhPort[i].HcRhPortStatus |= OHCI_PORT_PPS;
	}
	else {
		m_Registers.RhPort[i].HcRhPortStatus &= ~(OHCI_PORT_PPS |
			OHCI_PORT_CCS |
			OHCI_PORT_PSS |
			OHCI_PORT_PRS);
	}
}

void OHCI::OHCI_PortSetStatus(int PortNum, uint32_t Value)
{
	uint32_t old_state;
	OHCIPort* port;

	port = &m_Registers.RhPort[PortNum];
	old_state = port->HcRhPortStatus;

	// Write to clear CSC, PESC, PSSC, OCIC, PRSC
	if (Value & OHCI_PORT_WTC) {
		port->HcRhPortStatus &= ~(Value & OHCI_PORT_WTC);
	}

	if (Value & OHCI_PORT_CCS) {
		port->HcRhPortStatus &= ~OHCI_PORT_PES;
	}

	OHCI_PortSetIfConnected(PortNum, Value & OHCI_PORT_PES);

	if (OHCI_PortSetIfConnected(PortNum, Value & OHCI_PORT_PSS)) {
		DbgPrintf("Ohci: port %d: SUSPEND\n", PortNum);
	}

	if (OHCI_PortSetIfConnected(PortNum, Value & OHCI_PORT_PRS)) {
		DbgPrintf("Ohci: port %d: RESET\n", PortNum);
		USB_DeviceReset(port->UsbPort.Dev);
		port->HcRhPortStatus &= ~OHCI_PORT_PRS;
		// ??? Should this also set OHCI_PORT_PESC
		port->HcRhPortStatus |= OHCI_PORT_PES | OHCI_PORT_PRSC;
	}

	// Invert order here to ensure in ambiguous case, device is powered up...
	if (Value & OHCI_PORT_LSDA) {
		OHCI_PortPower(PortNum, 0);
	}

	if (Value & OHCI_PORT_PPS) {
		OHCI_PortPower(PortNum, 1);
	}	

	if (old_state != port->HcRhPortStatus) {
		OHCI_SetInterrupt(OHCI_INTR_RHSC);
	}
}

int OHCI::OHCI_PortSetIfConnected(int i, uint32_t Value)
{
	int ret = 1;

	// writing a 0 has no effect
	if (Value == 0) {
		return 0;
	}

	// If CurrentConnectStatus is cleared we set ConnectStatusChange
	if (!(m_Registers.RhPort[i].HcRhPortStatus & OHCI_PORT_CCS)) {
		m_Registers.RhPort[i].HcRhPortStatus |= OHCI_PORT_CSC;
		if (m_Registers.HcRhStatus & OHCI_RHS_DRWE) {
			// TODO: CSC is a wakeup event
		}
		return 0;
	}

	if (m_Registers.RhPort[i].HcRhPortStatus & Value) {
		ret = 0;
	}	

	// set the bit
	m_Registers.RhPort[i].HcRhPortStatus |= Value;

	return ret;
}

void OHCI::OHCI_Detach(USBPort* Port)
{
	OHCIPort* port = &m_Registers.RhPort[Port->PortIndex];
	uint32_t old_state = port->HcRhPortStatus;

	ohci_async_cancel_device(Port->Dev);

	// set connect status
	if (port->HcRhPortStatus & OHCI_PORT_CCS) {
		port->HcRhPortStatus &= ~OHCI_PORT_CCS;
		port->HcRhPortStatus |= OHCI_PORT_CSC;
	}

	// disable port
	if (port->HcRhPortStatus & OHCI_PORT_PES) {
		port->HcRhPortStatus &= ~OHCI_PORT_PES;
		port->HcRhPortStatus |= OHCI_PORT_PESC;
	}

	DbgPrintf("Ohci: Detached port %d\n", Port->PortIndex);

	if (old_state != port->HcRhPortStatus) {
		OHCI_SetInterrupt(OHCI_INTR_RHSC);
	}
}

void OHCI::OHCI_Attach(USBPort* Port)
{
	OHCIPort* port = &m_Registers.RhPort[Port->PortIndex];
	uint32_t old_state = port->HcRhPortStatus;

	// set connect status
	port->HcRhPortStatus |= OHCI_PORT_CCS | OHCI_PORT_CSC;

	// update speed
	if (port->UsbPort.Dev->speed == USB_SPEED_LOW) {
		port->HcRhPortStatus |= OHCI_PORT_LSDA;
	}
	else {
		port->HcRhPortStatus &= ~OHCI_PORT_LSDA;
	}

	// notify of remote-wakeup
	if ((m_Registers.HcControl & OHCI_CTL_HCFS) == Suspend) {
		OHCI_SetInterrupt(OHCI_INTR_RD);
	}

	DbgPrintf("Ohci: Attached port %d\n", Port->PortIndex);

	if (old_state != port->HcRhPortStatus) {
		OHCI_SetInterrupt(OHCI_INTR_RHSC);
	}
}

void OHCI::USB_RegisterPort(USBPort* Port, int Index, int SpeedMask)
{
	Port->PortIndex = Index;
	Port->SpeedMask = SpeedMask;
	Port->HubCount = 0;
	std::snprintf(Port->Path, sizeof(Port->Path), "%d", Index + 1);
}

void OHCI::USB_DeviceEPstopped(USBDev* Dev, USBEndpoint* EP)
{
	// This seems to be a nop in XQEMU since it doesn't assign the EP_Stopped function (it's nullptr)
	USBDeviceClass* klass = USB_DEVICE_GET_CLASS(Dev);
	if (klass->EP_Stopped) {
		klass->EP_Stopped(Dev, EP);
	}
}

void OHCI::USB_PortReset(USBPort* Port)
{
	USBDev* dev = Port->Dev;

	assert(dev != nullptr);
	USB_Detach(Port);
	USB_Attach(Port);
	USB_DeviceReset(dev);
}

void OHCI::USB_Detach(USBPort* Port)
{
	USBDev* dev = Port->Dev;

	assert(dev != nullptr);
	assert(dev->State != USB_STATE_NOTATTACHED);
	OHCI_Detach(Port);
	dev->State = USB_STATE_NOTATTACHED;
}

void OHCI::USB_Attach(USBPort* Port)
{
	USBDev *dev = Port->Dev;

	assert(dev != nullptr);
	assert(dev->Attached);
	assert(dev->State == USB_STATE_NOTATTACHED);
	OHCI_Attach(Port);
	dev->State = USB_STATE_ATTACHED;
	usb_device_handle_attach(dev);
}

void OHCI::USB_DeviceReset(USBDev* dev)
{
	if (dev == nullptr || !dev->Attached) {
		return;
	}

	dev->RemoteWakeup = 0;
	dev->Addr = 0;
	dev->State = USB_STATE_DEFAULT;
	usb_device_handle_reset(dev);
}
