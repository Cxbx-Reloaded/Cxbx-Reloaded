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

// Acknowledgment: QEMU ohci subsystem as used in XQEMU (GPLv2)
// https://xqemu.com/

/*
* QEMU USB OHCI Emulation
* Copyright (c) 2004 Gianni Tedesco
* Copyright (c) 2006 CodeSourcery
* Copyright (c) 2006 Openedhand Ltd.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, see <https://www.gnu.org/licenses/>.
*/


#define LOG_PREFIX CXBXR_MODULE::OHCI


#include <core\kernel\exports\xboxkrnl.h>
#include "OHCI.h"
#include "core\kernel\exports\EmuKrnl.h"  // For HalSystemInterrupt
#include "common\util\CxbxUtil.h"
#include "Logging.h"

static const char* OHCI_RegNames[] = {
	"HcRevision",
	"HcControl",
	"HcCommandStatus",
	"HcInterruptStatus",
	"HcInterruptEnable",
	"HcInterruptDisable",
	"HcHCCA",
	"HcPeriodCurrentED",
	"HcControlHeadED",
	"HcControlCurrentED",
	"HcBulkHeadED",
	"HcBulkCurrentED",
	"HcDoneHead",
	"HcFmInterval",
	"HcFmRemaining",
	"HcFmNumber",
	"HcPeriodicStart",
	"HcLSThreshold",
	"HcRhDescriptorA",
	"HcRhDescriptorB",
	"HcRhStatus",
	"HcRhPortStatus[0]",
	"HcRhPortStatus[1]",
	"HcRhPortStatus[2]",
	"HcRhPortStatus[3]"
};

/* Define these two if you want to dump usb packets and OHCI registers */
//#define DEBUG_ISOCH
//#define DEBUG_PACKET
//#define DEBUG_OHCI_REG

#ifdef DEBUG_OHCI_REG
#define DUMP_REG_R(reg_val) EmuLog(LOG_LEVEL::DEBUG, "%s, R, reg_val: 0x%X", OHCI_RegNames[Addr >> 2], reg_val)
#define DUMP_REG_W(reg_val, val) EmuLog(LOG_LEVEL::DEBUG, "%s, W, reg_val: 0x%X, val: 0x%X", OHCI_RegNames[Addr >> 2], reg_val, val)
#define DUMP_REG_RO(reg_val, val) EmuLog(LOG_LEVEL::DEBUG, "%s, RO, reg_val: 0x%X, val: 0x%X", OHCI_RegNames[Addr >> 2], reg_val, val) 
#else
#define DUMP_REG_R(reg_val)
#define DUMP_REG_W(reg_val, val) 
#define DUMP_REG_RO(reg_val, val)
#endif

/* These macros are used to access the bits of the various registers */
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
// HcFmInterval
#define OHCI_FMI_FI                         0x00003FFF       // FrameInterval
#define OHCI_FMI_FIT                        0x80000000       // FrameIntervalToggle
// HcFmRemaining
#define OHCI_FMR_FR                         0x00003FFF       // FrameRemaining
#define OHCI_FMR_FRT                        0x80000000       // FrameRemainingToggle
// LSThreshold
#define OHCI_LS_THRESH                      0x628            // LSThreshold
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

/* Bitfields for the first word of an ED */
#define OHCI_ED_FA_SHIFT  0
#define OHCI_ED_FA_MASK   (0x7F<<OHCI_ED_FA_SHIFT)    // FunctionAddress
#define OHCI_ED_EN_SHIFT  7
#define OHCI_ED_EN_MASK   (0xF<<OHCI_ED_EN_SHIFT)     // EndpointNumber
#define OHCI_ED_D_SHIFT   11
#define OHCI_ED_D_MASK    (3<<OHCI_ED_D_SHIFT)        // Direction
#define OHCI_ED_S         (1<<13)                     // Speed
#define OHCI_ED_K         (1<<14)                     // sKip
#define OHCI_ED_F         (1<<15)                     // Format
#define OHCI_ED_MPS_SHIFT 16
#define OHCI_ED_MPS_MASK  (0x7FF<<OHCI_ED_MPS_SHIFT)  // MaximumPacketSize

/* Flags in the HeadP field of an ED */
#define OHCI_ED_H         1                           // Halted
#define OHCI_ED_C         2                           // toggleCarry

/* Bitfields for the first word of a TD */
#define OHCI_TD_R         (1<<18)                     // bufferRounding
#define OHCI_TD_DP_SHIFT  19
#define OHCI_TD_DP_MASK   (3<<OHCI_TD_DP_SHIFT)       // Direction-Pid
#define OHCI_TD_DI_SHIFT  21
#define OHCI_TD_DI_MASK   (7<<OHCI_TD_DI_SHIFT)       // DelayInterrupt
#define OHCI_TD_T0        (1<<24)
#define OHCI_TD_T1        (1<<25)                     // DataToggle (T0 and T1)
#define OHCI_TD_EC_SHIFT  26
#define OHCI_TD_EC_MASK   (3<<OHCI_TD_EC_SHIFT)       // ErrorCount
#define OHCI_TD_CC_SHIFT  28
#define OHCI_TD_CC_MASK   (0xF<<OHCI_TD_CC_SHIFT)     // ConditionCode

/* Bitfields for the first word of an Isochronous Transfer Desciptor.  */
/* CC & DI - same as in the General Transfer Desciptor */
#define OHCI_TD_SF_SHIFT  0
#define OHCI_TD_SF_MASK   (0xFFFF<<OHCI_TD_SF_SHIFT)
#define OHCI_TD_FC_SHIFT  24
#define OHCI_TD_FC_MASK   (7<<OHCI_TD_FC_SHIFT)

/* Isochronous Transfer Desciptor - Offset / PacketStatusWord */
#define OHCI_TD_PSW_CC_SHIFT 12
#define OHCI_TD_PSW_CC_MASK  (0xF<<OHCI_TD_PSW_CC_SHIFT)
#define OHCI_TD_PSW_SIZE_SHIFT 0
#define OHCI_TD_PSW_SIZE_MASK  (0xFFF<<OHCI_TD_PSW_SIZE_SHIFT)

/* Mask the four least significant bits in an ED address */
#define OHCI_DPTR_MASK    0xFFFFFFF0

#define OHCI_BM(val, field) \
  (((val) & OHCI_##field##_MASK) >> OHCI_##field##_SHIFT)

#define OHCI_SET_BM(val, field, newval) do { \
    val &= ~OHCI_##field##_MASK; \
    val |= ((newval) << OHCI_##field##_SHIFT) & OHCI_##field##_MASK; \
    } while(0)

/* Indicates the direction of data flow as specified by the TD */
#define OHCI_TD_DIR_SETUP     0x0 // to endpoint
#define OHCI_TD_DIR_OUT       0x1 // to endpoint
#define OHCI_TD_DIR_IN        0x2 // from endpoint
#define OHCI_TD_DIR_RESERVED  0x3

#define OHCI_CC_NOERROR             0x0
#define OHCI_CC_CRC                 0x1
#define OHCI_CC_BITSTUFFING         0x2
#define OHCI_CC_DATATOGGLEMISMATCH  0x3
#define OHCI_CC_STALL               0x4
#define OHCI_CC_DEVICENOTRESPONDING 0x5
#define OHCI_CC_PIDCHECKFAILURE     0x6
#define OHCI_CC_UNDEXPETEDPID       0x7
#define OHCI_CC_DATAOVERRUN         0x8
#define OHCI_CC_DATAUNDERRUN        0x9
#define OHCI_CC_BUFFEROVERRUN       0xC
#define OHCI_CC_BUFFERUNDERRUN      0xD

#define USB_HZ 12000000

#define USUB(a, b) ((int16_t)((uint16_t)(a) - (uint16_t)(b)))

#define OHCI_PAGE_MASK    0xFFFFF000
#define OHCI_OFFSET_MASK  0xFFF


OHCI::OHCI(USBDevice* UsbObj)
{
	int offset = 0;
	USBPortOps* ops;

	m_UsbDevice = UsbObj;
	ops = new USBPortOps();
	{
		using namespace std::placeholders;

		ops->attach       = std::bind(&OHCI::OHCI_Attach, this, _1);
		ops->detach       = std::bind(&OHCI::OHCI_Detach, this, _1);
		ops->child_detach = std::bind(&OHCI::OHCI_ChildDetach, this, _1);
		ops->wakeup       = std::bind(&OHCI::OHCI_Wakeup, this, _1);
		ops->complete     = std::bind(&OHCI::OHCI_AsyncCompletePacket, this, _1, _2);
	}

	for (int i = 0; i < 4; i++) {
		m_UsbDevice->USB_RegisterPort(&m_Registers.RhPort[i].UsbPort, i, USB_SPEED_MASK_LOW | USB_SPEED_MASK_FULL, ops);
	}
	OHCI_PacketInit(&m_UsbPacket);

	m_UsbFrameTime = 1000000ULL; // 1 ms expressed in ns
	m_TicksPerUsbTick = 1000000000ULL / USB_HZ; // 83

	// Do a hardware reset
	OHCI_StateReset();
}

void OHCI::OHCI_FrameBoundaryWorker()
{
	OHCI_HCCA hcca;

	m_FrameTimeMutex.lock();

	if (OHCI_ReadHCCA(m_Registers.HcHCCA, &hcca)) {
		EmuLog(LOG_LEVEL::WARNING, "HCCA read error at physical address 0x%X", m_Registers.HcHCCA);
		OHCI_FatalError();
		m_FrameTimeMutex.unlock();
		return;
	}

	// Process all the lists at the end of the frame
	if (m_Registers.HcControl & OHCI_CTL_PLE) {
		// From the OHCI standard: "The head pointer used for a particular frame is determined by using the last 5 bits of the
		// Frame Counter as an offset into the interrupt array within the HCCA."
		int n = m_Registers.HcFmNumber & 0x1F;
		OHCI_ServiceEDlist(hcca.HccaInterrruptTable[n], 0); // dropped little -> big endian conversion from QEMU
	}

	// Cancel all pending packets if either of the lists has been disabled
	if (m_OldHcControl & (~m_Registers.HcControl) & (OHCI_CTL_BLE | OHCI_CTL_CLE)) {
		if (m_AsyncTD) {
			m_UsbDevice->USB_CancelPacket(&m_UsbPacket);
			m_AsyncTD = xbox::zero;
		}
		OHCI_StopEndpoints();
	}
	m_OldHcControl = m_Registers.HcControl;
	OHCI_ProcessLists(0);

	// Stop if UnrecoverableError happened or OHCI_SOF will crash
	if (m_Registers.HcInterruptStatus & OHCI_INTR_UE) {
		m_FrameTimeMutex.unlock();
		return;
	}

	// From the OHCI standard: "This bit is loaded from the FrameIntervalToggle field of
	// HcFmInterval whenever FrameRemaining reaches 0."
	m_Registers.HcFmRemaining = (m_Registers.HcFmInterval & OHCI_FMI_FIT) == 0 ?
		m_Registers.HcFmRemaining & ~OHCI_FMR_FRT : m_Registers.HcFmRemaining | OHCI_FMR_FRT;

	// Increment frame number
	m_Registers.HcFmNumber = (m_Registers.HcFmNumber + 1) & 0xFFFF; // prevent overflow
	hcca.HccaFrameNumber = m_Registers.HcFmNumber; // dropped big -> little endian conversion from QEMU

	if (m_DoneCount == 0 && !(m_Registers.HcInterruptStatus & OHCI_INTR_WD)) {
		if (!m_Registers.HcDoneHead) {
			// From the OHCI standard: "This is set to zero whenever HC writes the content of this
			// register to HCCA. It also sets the WritebackDoneHead of HcInterruptStatus."
			CxbxrAbort("HcDoneHead is zero but WritebackDoneHead interrupt is not set!\n");
		}

		if (m_Registers.HcInterrupt & m_Registers.HcInterruptStatus) {
			// From the OHCI standard: "The least significant bit of this entry is set to 1 to indicate whether an
			// unmasked HcInterruptStatus was set when HccaDoneHead was written." It's tecnically incorrect to
			// do this to HcDoneHead instead of HccaDoneHead however it doesn't matter since HcDoneHead is
			// zeroed below
			m_Registers.HcDoneHead |= 1;
		}

		hcca.HccaDoneHead = m_Registers.HcDoneHead; // dropped big -> little endian conversion from QEMU
		m_Registers.HcDoneHead = 0;
		m_DoneCount = 7;
		OHCI_SetInterrupt(OHCI_INTR_WD);
	}

	if (m_DoneCount != 7 && m_DoneCount != 0) {
		// decrease Done Queue counter
		m_DoneCount--;
	}

	// Do SOF stuff here
	OHCI_SOF();

	// Writeback HCCA
	if (OHCI_WriteHCCA(m_Registers.HcHCCA, &hcca)) {
		EmuLog(LOG_LEVEL::ERROR2, "HCCA write error at physical address 0x%X", m_Registers.HcHCCA);
		OHCI_FatalError();
	}

	m_FrameTimeMutex.unlock();
}

void OHCI::OHCI_FatalError()
{
	// According to the standard, an OHCI will stop operating, and set itself into error state
	// (which can be queried by MMIO). Instead of calling directly CxbxrAbort, we let the
	// HCD know the problem so that it can try to solve it

	OHCI_SetInterrupt(OHCI_INTR_UE);
	OHCI_BusStop();
	EmuLog(LOG_LEVEL::ERROR2, "An unrecoverable error has occoured!\n");
}

bool OHCI::OHCI_ReadHCCA(xbox::addr_xt Paddr, OHCI_HCCA* Hcca)
{
	return Memory_R(reinterpret_cast<void*>(Paddr), Hcca, sizeof(OHCI_HCCA));
}

bool OHCI::OHCI_WriteHCCA(xbox::addr_xt Paddr, OHCI_HCCA* Hcca)
{
	// We need to calculate the offset of the HccaFrameNumber member to avoid overwriting HccaInterrruptTable
	size_t OffsetOfFrameNumber = offsetof(OHCI_HCCA, HccaFrameNumber);
	return Memory_W(reinterpret_cast<void*>(Paddr + OffsetOfFrameNumber), reinterpret_cast<uint8_t*>(Hcca) + OffsetOfFrameNumber, 8);
}

bool OHCI::OHCI_ReadED(xbox::addr_xt Paddr, OHCI_ED* Ed)
{
	return GetDwords(Paddr, reinterpret_cast<uint32_t*>(Ed), sizeof(*Ed) >> 2); // ED is 16 bytes large
}

bool OHCI::OHCI_WriteED(xbox::addr_xt Paddr, OHCI_ED* Ed)
{
	// According to the standard, only the HeadP field is writable by the HC, so we'll write just that
	size_t OffsetOfHeadP = offsetof(OHCI_ED, HeadP);
	return WriteDwords(Paddr + OffsetOfHeadP, reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(Ed) + OffsetOfHeadP), 1);
}

bool OHCI::OHCI_ReadTD(xbox::addr_xt Paddr, OHCI_TD* Td)
{
	return GetDwords(Paddr, reinterpret_cast<uint32_t*>(Td), sizeof(*Td) >> 2); // TD is 16 bytes large
}

bool OHCI::OHCI_WriteTD(xbox::addr_xt Paddr, OHCI_TD* Td)
{
	return WriteDwords(Paddr, reinterpret_cast<uint32_t*>(Td), sizeof(*Td) >> 2);
}

bool OHCI::OHCI_ReadIsoTD(xbox::addr_xt Paddr, OHCI_ISO_TD* td)
{
	return GetDwords(Paddr, reinterpret_cast<uint32_t*>(td), 4) || GetWords(Paddr + 16, td->Offset, 8);
}

bool OHCI::OHCI_WriteIsoTD(xbox::addr_xt Paddr, OHCI_ISO_TD* td)
{
	return WriteDwords(Paddr, reinterpret_cast<uint32_t*>(td), 4) || WriteWords(Paddr + 16, td->Offset, 8);
}

bool OHCI::OHCI_CopyTDBuffer(OHCI_TD* Td, uint8_t* Buffer, int Length, bool bIsWrite)
{
	uint32_t ptr;
	int n;

	// Figure out if we are crossing a 4K page boundary
	ptr = Td->CurrentBufferPointer;
	n = 0x1000 - (ptr & 0xFFF);
	if (n > Length) {
		n = Length;
	}

	if (Memory_RW(reinterpret_cast<void*>(ptr), Buffer, n, bIsWrite)) {
		return true; // error
	}
	if (n == Length) {
		return false; // no bytes left to copy
	}

	// From the OHCI standard: "If during the data transfer the buffer address contained in the HCfs working copy of
	// CurrentBufferPointer crosses a 4K boundary, the upper 20 bits of BufferEnd are copied to the
	// working value of CurrentBufferPointer causing the next buffer address to be the 0th byte in the
	// same 4K page that contains the last byte of the buffer."
	ptr = Td->BufferEnd & ~0xFFFu;
	Buffer += n;
	if (Memory_RW(reinterpret_cast<void*>(ptr), Buffer, Length - n, bIsWrite)) {
		return true; // error
	}
	return false;
}

bool OHCI::OHCI_CopyIsoTDBuffer(uint32_t start_addr, uint32_t end_addr, uint8_t* Buffer, int Length, bool bIsWrite)
{
	uint32_t ptr;
	int n;

	ptr = start_addr;
	n = 0x1000 - (ptr & 0xFFF);
	if (n > Length) {
		n = Length;
	}

	if (Memory_RW(reinterpret_cast<void*>(ptr), Buffer, n, bIsWrite)) {
		return true; // error
	}
	if (n == Length) {
		return false; // no bytes left to copy
	}
	ptr = end_addr & ~0xFFFu;
	Buffer += n;
	if (Memory_RW(reinterpret_cast<void*>(ptr), Buffer, Length - n, bIsWrite)) {
		return true; // error
	}
	return false;
}

int OHCI::OHCI_ServiceEDlist(xbox::addr_xt Head, int Completion)
{
	OHCI_ED ed;
	xbox::addr_xt next_ed;
	xbox::addr_xt current;
	int active;

	active = 0;

	if (Head == xbox::zero) {
		// no ED here, nothing to do
		return 0;
	}

	for (current = Head; current; current = next_ed) {
		if (OHCI_ReadED(current, &ed)) {
			EmuLog(LOG_LEVEL::ERROR2, "ED read error at physical address 0x%X", current);
			OHCI_FatalError();
			return 0;
		}

		// From the OHCI standard "An Endpoint Descriptor (ED) is a 16-byte, memory resident structure that must be aligned to a
		// 16-byte boundary."
		next_ed = ed.NextED & OHCI_DPTR_MASK;

		if ((ed.HeadP & OHCI_ED_H) || (ed.Flags & OHCI_ED_K)) { // halted or skip
			// Cancel pending packets for ED that have been paused
			xbox::addr_xt addr = ed.HeadP & OHCI_DPTR_MASK;
			if (m_AsyncTD && addr == m_AsyncTD) {
				m_UsbDevice->USB_CancelPacket(&m_UsbPacket);
				m_AsyncTD = xbox::zero;
				m_UsbDevice->USB_DeviceEPstopped(m_UsbPacket.Endpoint->Dev, m_UsbPacket.Endpoint);
			}
			continue;
		}

		while ((ed.HeadP & OHCI_DPTR_MASK) != ed.TailP) { // a TD is available to be processed
#ifdef DEBUG_PACKET
			printf("ED @ 0x%.8x fa=%u en=%u d=%u s=%u k=%u f=%u mps=%u "
				"h=%u c=%u\n  head=0x%.8x tailp=0x%.8x next=0x%.8x\n", current,
				OHCI_BM(ed.Flags, ED_FA), OHCI_BM(ed.Flags, ED_EN),
				OHCI_BM(ed.Flags, ED_D), (ed.Flags & OHCI_ED_S) != 0,
				(ed.Flags & OHCI_ED_K) != 0, (ed.Flags & OHCI_ED_F) != 0,
				OHCI_BM(ed.Flags, ED_MPS), (ed.HeadP & OHCI_ED_H) != 0,
				(ed.HeadP & OHCI_ED_C) != 0, ed.HeadP & OHCI_DPTR_MASK,
				ed.TailP & OHCI_DPTR_MASK, ed.NextED & OHCI_DPTR_MASK);
#endif
			active = 1;

			if ((ed.Flags & OHCI_ED_F) == 0) {
				// Handle control, interrupt or bulk endpoints
				if (OHCI_ServiceTD(&ed)) {
					break;
				}
			}
			else {
				// Handle isochronous endpoints
				if (OHCI_ServiceIsoTD(&ed, Completion)) {
					break;
				}
			}
		}

		// Writeback ED
		if (OHCI_WriteED(current, &ed)) {
			OHCI_FatalError();
			return 0;
		}
	}

	return active;
}

int OHCI::OHCI_ServiceTD(OHCI_ED* Ed)
{
	int direction;
	size_t length = 0, packetlen = 0;
#ifdef DEBUG_PACKET
	const char *str = nullptr;
#endif
	int pid;
	int ret;
	int i;
	XboxDeviceState* dev;
	USBEndpoint* ep;
	OHCI_TD td;
	xbox::addr_xt addr;
	int flag_r;
	int completion;

	addr = Ed->HeadP & OHCI_DPTR_MASK;
	// See if this TD has already been submitted to the device
	completion = (addr == m_AsyncTD);
	if (completion && !m_AsyncComplete) {
		EmuLog(LOG_LEVEL::DEBUG, "Skipping async TD");
		return 1;
	}
	if (OHCI_ReadTD(addr, &td)) {
		EmuLog(LOG_LEVEL::ERROR2, "TD read error at physical address 0x%X", addr);
		OHCI_FatalError();
		return 0;
	}

	// From the OHCI standard: "This 2-bit field indicates the direction of data flow and the PID
	// to be used for the token. This field is only relevant to the HC if the D field in the ED
	// was set to 00b or 11b indicating that the PID determination is deferred to the TD."
	direction = OHCI_BM(Ed->Flags, ED_D);
	switch (direction) {
		case OHCI_TD_DIR_OUT:
		case OHCI_TD_DIR_IN:
			// Same value
			break;
		default:
			direction = OHCI_BM(td.Flags, TD_DP);
	}

	// Info: Each USB transaction consists of a
	// 1. Token Packet, (Header defining what it expects to follow).
	// 2. Optional Data Packet, (Containing the payload).
	// 3. Status Packet, (Used to acknowledge transactions and to provide a means of error correction).

	// There are three types of token packets:
	// In - Informs the USB device that the host wishes to read information.
	// Out - Informs the USB device that the host wishes to send information.
	// Setup - Used to begin control transfers.

	switch (direction) {
		case OHCI_TD_DIR_IN:
#ifdef DEBUG_PACKET
			str = "in";
#endif
			pid = USB_TOKEN_IN;
			break;
		case OHCI_TD_DIR_OUT:
#ifdef DEBUG_PACKET
			str = "out";
#endif
			pid = USB_TOKEN_OUT;
			break;
		case OHCI_TD_DIR_SETUP:
#ifdef DEBUG_PACKET
			str = "setup";
#endif
			pid = USB_TOKEN_SETUP;
			break;
		default:
			EmuLog(LOG_LEVEL::WARNING, "bad direction");
			return 1;
	}

	// Check if this TD has a buffer of user data to transfer
	if (td.CurrentBufferPointer && td.BufferEnd) {
		if ((td.CurrentBufferPointer & 0xFFFFF000) != (td.BufferEnd & 0xFFFFF000)) {
			// the buffer crosses a 4K page boundary
			length = (td.BufferEnd & 0xFFF) + 0x1001 - (td.CurrentBufferPointer & 0xFFF);
		}
		else {
			// the buffer is within a single page
			length = (td.BufferEnd - td.CurrentBufferPointer) + 1;
		}

		packetlen = length;
		if (length && direction != OHCI_TD_DIR_IN) {
			// The endpoint may not allow us to transfer it all now
			packetlen = (Ed->Flags & OHCI_ED_MPS_MASK) >> OHCI_ED_MPS_SHIFT;
			if (packetlen > length) {
				packetlen = length;
			}
			if (!completion) {
				if (OHCI_CopyTDBuffer(&td, m_UsbBuffer, packetlen, false)) {
					OHCI_FatalError();
				}
			}
		}
	}

	flag_r = (td.Flags & OHCI_TD_R) != 0;
#ifdef DEBUG_PACKET
	printf(" TD @ 0x%.8X %lld of %lld bytes %s r=%d cbp=0x%.8X be=0x%.8X\n",
		addr, (int64_t)packetlen, (int64_t)length, str, flag_r, td.CurrentBufferPointer, td.BufferEnd);

	if (packetlen > 0 && direction != OHCI_TD_DIR_IN) {
		printf("  data:");
		for (i = 0; i < packetlen; i++) {
			printf(" %.2x", m_UsbBuffer[i]);
		}
		printf("\n");
	}
#endif
	if (completion) {
		m_AsyncTD = 0;
		m_AsyncComplete = 0;
	}
	else {
		if (m_AsyncTD) {
			// From QEMU: "??? The hardware should allow one active packet per endpoint.
			// We only allow one active packet per controller. This should be sufficient
			// as long as devices respond in a timely manner."
			EmuLog(LOG_LEVEL::DEBUG, "too many pending packets");
			return 1;
		}
		dev = OHCI_FindDevice(OHCI_BM(Ed->Flags, ED_FA));
		ep = m_UsbDevice->USB_GetEP(dev, pid, OHCI_BM(Ed->Flags, ED_EN));
		m_UsbDevice->USB_PacketSetup(&m_UsbPacket, pid, ep, addr, !flag_r, OHCI_BM(td.Flags, TD_DI) == 0);
		m_UsbDevice->USB_PacketAddBuffer(&m_UsbPacket, m_UsbBuffer, packetlen);
		m_UsbDevice->USB_HandlePacket(dev, &m_UsbPacket);
#ifdef DEBUG_PACKET
		printf("status=%d\n", m_UsbPacket.Status);
#endif
		if (m_UsbPacket.Status == USB_RET_ASYNC) {
			m_UsbDevice->USB_DeviceFlushEPqueue(dev, ep);
			m_AsyncTD = addr;
			return 1;
		}
	}
	if (m_UsbPacket.Status == USB_RET_SUCCESS) {
		ret = m_UsbPacket.ActualLength;
	}
	else {
		ret = m_UsbPacket.Status;
	}

	if (ret >= 0) {
		if (direction == OHCI_TD_DIR_IN) {
			if (OHCI_CopyTDBuffer(&td, m_UsbBuffer, ret, true)) {
				OHCI_FatalError();
			}
#ifdef DEBUG_PACKET
			printf("  data:");
			for (i = 0; i < ret; i++) {
				printf(" %.2X", m_UsbBuffer[i]);
			}
			printf("\n");
#endif
		}
		else {
			ret = packetlen;
		}
	}

	if (ret >= 0) {
		if ((td.CurrentBufferPointer & 0xFFF) + ret > 0xFFF) {
			td.CurrentBufferPointer = (td.BufferEnd & ~0xFFF) + ((td.CurrentBufferPointer + ret) & 0xFFF);
		}
		else {
			td.CurrentBufferPointer += ret;
		}
	}

	// Writeback
	if (ret == packetlen || (direction == OHCI_TD_DIR_IN && ret >= 0 && flag_r)) {
		// Transmission succeeded
		if (ret == length) {
			td.CurrentBufferPointer = 0;
		}
		td.Flags |= OHCI_TD_T1;
		td.Flags ^= OHCI_TD_T0;
		OHCI_SET_BM(td.Flags, TD_CC, OHCI_CC_NOERROR);
		OHCI_SET_BM(td.Flags, TD_EC, 0);

		if ((direction != OHCI_TD_DIR_IN) && (ret != length)) {
			// Partial packet transfer: TD not ready to retire yet
			goto exit_no_retire;
		}

		// Setting ED_C is part of the TD retirement process
		Ed->HeadP &= ~OHCI_ED_C;
		if (td.Flags & OHCI_TD_T0)
			Ed->HeadP |= OHCI_ED_C;
	}
	else {
		if (ret >= 0) {
			EmuLog(LOG_LEVEL::DEBUG, "Underrun");
			OHCI_SET_BM(td.Flags, TD_CC, OHCI_CC_DATAUNDERRUN);
		}
		else {
			switch (ret) {
				case USB_RET_IOERROR:
				case USB_RET_NODEV:
					EmuLog(LOG_LEVEL::DEBUG, "Received DEV ERROR");
					OHCI_SET_BM(td.Flags, TD_CC, OHCI_CC_DEVICENOTRESPONDING);
					break;
				case USB_RET_NAK:
					EmuLog(LOG_LEVEL::DEBUG, "Received NAK");
					return 1;
				case USB_RET_STALL:
					EmuLog(LOG_LEVEL::DEBUG, "Received STALL");
					OHCI_SET_BM(td.Flags, TD_CC, OHCI_CC_STALL);
					break;
				case USB_RET_BABBLE:
					EmuLog(LOG_LEVEL::DEBUG, "Received BABBLE");
					OHCI_SET_BM(td.Flags, TD_CC, OHCI_CC_DATAOVERRUN);
					break;
				default:
					EmuLog(LOG_LEVEL::DEBUG, "Bad device response %d", ret);
					OHCI_SET_BM(td.Flags, TD_CC, OHCI_CC_UNDEXPETEDPID);
					OHCI_SET_BM(td.Flags, TD_EC, 3);
			}
		}
		Ed->HeadP |= OHCI_ED_H;
	}

	// Retire this TD
	Ed->HeadP &= ~OHCI_DPTR_MASK;
	Ed->HeadP |= td.NextTD & OHCI_DPTR_MASK;
	td.NextTD = m_Registers.HcDoneHead;
	m_Registers.HcDoneHead = addr;
	i = OHCI_BM(td.Flags, TD_DI);
	if (i < m_DoneCount) {
		m_DoneCount = i;
	}	
	if (OHCI_BM(td.Flags, TD_CC) != OHCI_CC_NOERROR) {
		m_DoneCount = 0;
	}

exit_no_retire:
	if (OHCI_WriteTD(addr, &td)) {
		OHCI_FatalError();
		return 1;
	}
	return OHCI_BM(td.Flags, TD_CC) != OHCI_CC_NOERROR;
}

XboxDeviceState* OHCI::OHCI_FindDevice(uint8_t Addr)
{
	XboxDeviceState* dev;
	int i;

	for (i = 0; i < 4; i++) {
		if ((m_Registers.RhPort[i].HcRhPortStatus & OHCI_PORT_PES) == 0) {
			continue; // port is disabled
		}
		dev = m_UsbDevice->USB_FindDevice(&m_Registers.RhPort[i].UsbPort, Addr);
		if (dev != nullptr) {
			return dev; // return found device
		}
	}

	return nullptr;
}

void OHCI::OHCI_StateReset()
{
	// The usb state can be USB_Suspend if it is a software reset, and USB_Reset if it is a hardware
	// reset or cold boot

	OHCI_BusStop();
	m_OldHcControl = 0;

	// Reset all registers
	m_Registers.HcRevision = 0x10;
	m_Registers.HcControl = 0;
	m_Registers.HcCommandStatus = 0;
	m_Registers.HcInterruptStatus = 0;
	m_Registers.HcInterrupt = OHCI_INTR_MIE; // enable interrupts

	m_Registers.HcHCCA = 0;
	m_Registers.HcPeriodCurrentED = 0;
	m_Registers.HcControlHeadED = m_Registers.HcControlCurrentED = 0;
	m_Registers.HcBulkHeadED = m_Registers.HcBulkCurrentED = 0;
	m_Registers.HcDoneHead = 0;

	m_Registers.HcFmInterval = 0;
	m_Registers.HcFmInterval |= (0x2778 << 16); // TBD according to the standard, using what QEMU sets (FSLargestDataPacket)
	m_Registers.HcFmInterval |= 0x2EDF; // bit-time of a frame. 1 frame = 1 ms (FrameInterval)
	m_Registers.HcFmRemaining = 0;
	m_Registers.HcFmNumber = 0;
	m_Registers.HcPeriodicStart = 0;
	m_Registers.HcLSThreshold = OHCI_LS_THRESH;

	m_Registers.HcRhDescriptorA = OHCI_RHA_NOCP | OHCI_RHA_NPS | 4; // The xbox lacks the hw to switch off the power on the ports and has 4 ports per HC
	m_Registers.HcRhDescriptorB = 0; // The attached devices are removable and use PowerSwitchingMode to control the power on the ports
	m_Registers.HcRhStatus = 0;

	m_DoneCount = 7;

	for (int i = 0; i < 4; i++)
	{
		OHCIPort* Port = &m_Registers.RhPort[i];
		Port->HcRhPortStatus = 0;
		if (Port->UsbPort.Dev && Port->UsbPort.Dev->Attached) {
			m_UsbDevice->USB_PortReset(&Port->UsbPort);
		}
	}
	if (m_AsyncTD) {
		m_UsbDevice->USB_CancelPacket(&m_UsbPacket);
		m_AsyncTD = xbox::zero;
	}

	OHCI_StopEndpoints();

	EmuLog(LOG_LEVEL::DEBUG, "Reset event.");
}

void OHCI::OHCI_BusStart()
{
	// Create the EOF timer.
	m_pEOFtimer = true;

	EmuLog(LOG_LEVEL::DEBUG, "Operational event");

	// SOF event
	OHCI_SOF();
}

void OHCI::OHCI_BusStop()
{
	m_pEOFtimer = false;
}

void OHCI::OHCI_SOF()
{
	// set current SOF time
	m_SOFtime = get_now();

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
			EmuLog(LOG_LEVEL::DEBUG, "Suspend event");
			break;

		case Resume:
			EmuLog(LOG_LEVEL::DEBUG, "Resume event");
			break;

		case Reset:
			OHCI_StateReset();
			break;

		default:
			EmuLog(LOG_LEVEL::WARNING, "Unknown USB mode!");
	}
}

void OHCI::OHCI_PacketInit(USBPacket* packet)
{
	IOVector* vec = &packet->IoVec;
	vec->IoVecStruct = new IoVec;
	vec->IoVecNumber = 0;
	vec->AllocNumber = 1;
	vec->Size = 0;
}

uint32_t OHCI::OHCI_ReadRegister(xbox::addr_xt Addr)
{
	uint32_t ret = 0xFFFFFFFF;

	if (Addr & 3) {
		// The standard allows only aligned reads to the registers
		EmuLog(LOG_LEVEL::DEBUG, "Unaligned read. Ignoring.");
		return ret;
	}
	else {
		switch (Addr >> 2) // read the register
		{
			case 0: // HcRevision
				ret = m_Registers.HcRevision;
				DUMP_REG_R(ret);
				break;

			case 1: // HcControl
				ret = m_Registers.HcControl;
				DUMP_REG_R(ret);
				break;

			case 2: // HcCommandStatus
				ret = m_Registers.HcCommandStatus;
				DUMP_REG_R(ret);
				break;

			case 3: // HcInterruptStatus
				ret = m_Registers.HcInterruptStatus;
				DUMP_REG_R(ret);
				break;

			case 4: // HcInterruptEnable
			case 5: // HcInterruptDisable
				ret = m_Registers.HcInterrupt;
				DUMP_REG_R(ret);
				break;

			case 6: // HcHCCA
				ret = m_Registers.HcHCCA;
				DUMP_REG_R(ret);
				break;

			case 7: // HcPeriodCurrentED
				ret = m_Registers.HcPeriodCurrentED;
				DUMP_REG_R(ret);
				break;

			case 8: // HcControlHeadED
				ret = m_Registers.HcControlHeadED;
				DUMP_REG_R(ret);
				break;

			case 9: // HcControlCurrentED
				ret = m_Registers.HcControlCurrentED;
				DUMP_REG_R(ret);
				break;

			case 10: // HcBulkHeadED
				ret = m_Registers.HcBulkHeadED;
				DUMP_REG_R(ret);
				break;

			case 11: // HcBulkCurrentED
				ret = m_Registers.HcBulkCurrentED;
				DUMP_REG_R(ret);
				break;

			case 12: // HcDoneHead
				ret = m_Registers.HcDoneHead;
				DUMP_REG_R(ret);
				break;

			case 13: // HcFmInterval
				ret = m_Registers.HcFmInterval;
				DUMP_REG_R(ret);
				break;

			case 14: // HcFmRemaining
				ret = OHCI_GetFrameRemaining();
				DUMP_REG_R(ret);
				break;

			case 15: // HcFmNumber
				ret = m_Registers.HcFmNumber;
				DUMP_REG_R(ret);
				break;

			case 16: // HcPeriodicStart
				ret = m_Registers.HcPeriodicStart;
				DUMP_REG_R(ret);
				break;

			case 17: // HcLSThreshold
				ret = m_Registers.HcLSThreshold;
				DUMP_REG_R(ret);
				break;

			case 18: // HcRhDescriptorA
				ret = m_Registers.HcRhDescriptorA;
				DUMP_REG_R(ret);
				break;

			case 19: // HcRhDescriptorB
				ret = m_Registers.HcRhDescriptorB;
				DUMP_REG_R(ret);
				break;

			case 20: // HcRhStatus
				ret = m_Registers.HcRhStatus;
				DUMP_REG_R(ret);
				break;

			// Always report that the port power is on since the Xbox cannot switch off the electrical current to it
			case 21: // RhPort 0
				ret = m_Registers.RhPort[0].HcRhPortStatus | OHCI_PORT_PPS;
				DUMP_REG_R(ret);
				break;

			case 22: // RhPort 1
				ret = m_Registers.RhPort[1].HcRhPortStatus | OHCI_PORT_PPS;
				DUMP_REG_R(ret);
				break;

			case 23: // RhPort 2
				ret = m_Registers.RhPort[2].HcRhPortStatus | OHCI_PORT_PPS;
				DUMP_REG_R(ret);
				break;

			case 24: // RhPort 3
				ret = m_Registers.RhPort[3].HcRhPortStatus | OHCI_PORT_PPS;
				DUMP_REG_R(ret);
				break;

			default:
				EmuLog(LOG_LEVEL::WARNING, "Read register operation with bad offset %u. Ignoring.", Addr >> 2);
		}
		return ret;
	}
}

void OHCI::OHCI_WriteRegister(xbox::addr_xt Addr, uint32_t Value)
{
	if (Addr & 3) {
		// The standard allows only aligned writes to the registers
		EmuLog(LOG_LEVEL::DEBUG, "Unaligned write. Ignoring.");
		return;
	}
	else {
		switch (Addr >> 2)
		{
			case 0: // HcRevision
				// This register is read-only
				DUMP_REG_RO(m_Registers.HcRevision, Value);
				break;

			case 1: // HcControl
				OHCI_ChangeState(Value);
				DUMP_REG_W(m_Registers.HcControl, Value);
				break;

			case 2: // HcCommandStatus
			{
				// SOC is read-only
				Value &= ~OHCI_STATUS_SOC;

				// From the OHCI standard: "The Host Controller must ensure that bits written as 1 become set
				// in the register while bits written as 0 remain unchanged in the register."
				m_Registers.HcCommandStatus |= Value;

				if (m_Registers.HcCommandStatus & OHCI_STATUS_HCR) {
					// Do a hardware reset
					OHCI_StateReset();
				}
				DUMP_REG_W(m_Registers.HcCommandStatus, Value);
			}
			break;

			case 3: // HcInterruptStatus
				m_Registers.HcInterruptStatus &= ~Value;
				OHCI_UpdateInterrupt();
				DUMP_REG_W(m_Registers.HcInterruptStatus, Value);
				break;

			case 4: // HcInterruptEnable
				m_Registers.HcInterrupt |= Value;
				OHCI_UpdateInterrupt();
				DUMP_REG_W(m_Registers.HcInterrupt, Value);
				break;

			case 5: // HcInterruptDisable
				m_Registers.HcInterrupt &= ~Value;
				OHCI_UpdateInterrupt();
				DUMP_REG_W(m_Registers.HcInterrupt, Value);
				break;

			case 6: // HcHCCA
				// The standard says the minimum alignment is 256 bytes and so bits 0 through 7 are always zero
				m_Registers.HcHCCA = Value & OHCI_HCCA_MASK;
				DUMP_REG_W(m_Registers.HcHCCA, Value);
				break;

			case 7: // HcPeriodCurrentED
				// This register is read-only
				DUMP_REG_RO(m_Registers.HcPeriodCurrentED, Value);
				break;

			case 8: // HcControlHeadED
				m_Registers.HcControlHeadED = Value & OHCI_DPTR_MASK;
				DUMP_REG_W(m_Registers.HcControlHeadED, Value);
				break;

			case 9: // HcControlCurrentED
				m_Registers.HcControlCurrentED = Value & OHCI_DPTR_MASK;
				DUMP_REG_W(m_Registers.HcControlCurrentED, Value);
				break;

			case 10: // HcBulkHeadED
				m_Registers.HcBulkHeadED = Value & OHCI_DPTR_MASK;
				DUMP_REG_W(m_Registers.HcBulkHeadED, Value);
				break;

			case 11: // HcBulkCurrentED
				m_Registers.HcBulkCurrentED = Value & OHCI_DPTR_MASK;
				DUMP_REG_W(m_Registers.HcBulkCurrentED, Value);
				break;

			case 12: // HcDoneHead
				// This register is read-only
				DUMP_REG_RO(m_Registers.HcDoneHead, Value);
				break;

			case 13: // HcFmInterval
			{
				if ((Value & OHCI_FMI_FI) != (m_Registers.HcFmInterval & OHCI_FMI_FI)) {
					EmuLog(LOG_LEVEL::DEBUG, "Changing frame interval duration. New value is %u", Value & OHCI_FMI_FI);
				}
				m_Registers.HcFmInterval = Value & ~0xC000;
				DUMP_REG_W(m_Registers.HcFmInterval, Value);
			}
			break;

			case 14: // HcFmRemaining
				// This register is read-only
				DUMP_REG_RO(m_Registers.HcFmRemaining, Value);
				break;

			case 15: // HcFmNumber
				// This register is read-only
				DUMP_REG_RO(m_Registers.HcFmNumber, Value);
				break;

			case 16: // HcPeriodicStart
				m_Registers.HcPeriodicStart = Value & 0x3FFF;
				DUMP_REG_W(m_Registers.HcPeriodicStart, Value);
				break;

			case 17: // HcLSThreshold
				m_Registers.HcLSThreshold = Value & 0xFFF;
				DUMP_REG_W(m_Registers.HcLSThreshold, Value);
				break;

			case 18: // HcRhDescriptorA
				m_Registers.HcRhDescriptorA &= ~OHCI_RHA_RW_MASK;
				m_Registers.HcRhDescriptorA |= Value & OHCI_RHA_RW_MASK; // ??
				DUMP_REG_W(m_Registers.HcRhDescriptorA, Value);
				break;

			case 19: // HcRhDescriptorB
				// Don't do anything, the attached devices are all removable and PowerSwitchingMode is always 0
				break;

			case 20: // HcRhStatus
				OHCI_SetHubStatus(Value);
				DUMP_REG_W(m_Registers.HcRhStatus, Value);
				break;

			case 21: // RhPort 0
				OHCI_PortSetStatus(0, Value);
				DUMP_REG_W(m_Registers.RhPort[0].HcRhPortStatus, Value);
				break;

			case 22: // RhPort 1
				OHCI_PortSetStatus(1, Value);
				DUMP_REG_W(m_Registers.RhPort[1].HcRhPortStatus, Value);
				break;

			case 23: // RhPort 2
				OHCI_PortSetStatus(2, Value);
				DUMP_REG_W(m_Registers.RhPort[2].HcRhPortStatus, Value);
				break;

			case 24: // RhPort 3
				OHCI_PortSetStatus(3, Value);
				DUMP_REG_W(m_Registers.RhPort[3].HcRhPortStatus, Value);
				break;

			default:
				EmuLog(LOG_LEVEL::WARNING, "Write register operation with bad offset %u. Ignoring.", Addr >> 2);
		}
	}
}

uint64_t OHCI::OHCI_next(uint64_t now)
{
	if (m_pEOFtimer) {
		constexpr uint64_t ohci_period = 1000;
		uint64_t next = m_SOFtime + ohci_period;

		if (now >= next) {
			OHCI_FrameBoundaryWorker();
			return ohci_period;
		}

		return m_SOFtime + ohci_period - now; // time remaining until EOF
	}

	return -1;
}

void OHCI::OHCI_UpdateInterrupt()
{
	if ((m_Registers.HcInterrupt & OHCI_INTR_MIE) && (m_Registers.HcInterruptStatus & m_Registers.HcInterrupt)) {
		HalSystemInterrupts[1].Assert(false);
		HalSystemInterrupts[1].Assert(true);
	}
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
	ticks = get_now() - m_SOFtime;

	// Avoid Muldiv64 if possible
	if (ticks >= m_UsbFrameTime) {
		return m_Registers.HcFmRemaining & OHCI_FMR_FRT;
	}

	ticks = Muldiv64(1, (uint32_t)ticks, (uint32_t)m_TicksPerUsbTick);
	frame = static_cast<uint16_t>((m_Registers.HcFmInterval & OHCI_FMI_FI) - ticks);

	return (m_Registers.HcFmRemaining & OHCI_FMR_FRT) | frame;
}

void OHCI::OHCI_StopEndpoints()
{
	XboxDeviceState* dev;
	int i, j;

	for (i = 0; i < 4; i++) {
		dev = m_Registers.RhPort[i].UsbPort.Dev;
		if (dev && dev->Attached) {
			m_UsbDevice->USB_DeviceEPstopped(dev, &dev->EP_ctl);
			for (j = 0; j < USB_MAX_ENDPOINTS; j++) {
				m_UsbDevice->USB_DeviceEPstopped(dev, &dev->EP_in[j]);
				m_UsbDevice->USB_DeviceEPstopped(dev, &dev->EP_out[j]);
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

		for (i = 0; i < 4; i++) {
			OHCI_PortPower(i, 0);
		}	
		EmuLog(LOG_LEVEL::DEBUG, "powered down all ports");
	}

	if (Value & OHCI_RHS_LPSC) {
		int i;

		for (i = 0; i < 4; i++) {
			OHCI_PortPower(i, 1);
		}	
		EmuLog(LOG_LEVEL::DEBUG, "powered up all ports");
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
		EmuLog(LOG_LEVEL::DEBUG, "port %d: SUSPEND", PortNum);
	}

	if (OHCI_PortSetIfConnected(PortNum, Value & OHCI_PORT_PRS)) {
		EmuLog(LOG_LEVEL::DEBUG, "port %d: RESET", PortNum);
		m_UsbDevice->USB_DeviceReset(port->UsbPort.Dev);
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
			// from QEMU: TODO, CSC is a wakeup event
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

	OHCI_AsyncCancelDevice(Port->Dev);

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

	EmuLog(LOG_LEVEL::DEBUG, "Detached port %d", Port->PortIndex);

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
	if (port->UsbPort.Dev->Speed == USB_SPEED_LOW) {
		port->HcRhPortStatus |= OHCI_PORT_LSDA;
	}
	else {
		port->HcRhPortStatus &= ~OHCI_PORT_LSDA;
	}

	// notify of remote-wakeup
	if ((m_Registers.HcControl & OHCI_CTL_HCFS) == Suspend) {
		OHCI_SetInterrupt(OHCI_INTR_RD);
	}

	EmuLog(LOG_LEVEL::DEBUG, "Attached port %d", Port->PortIndex);

	if (old_state != port->HcRhPortStatus) {
		OHCI_SetInterrupt(OHCI_INTR_RHSC);
	}
}

void OHCI::OHCI_ChildDetach(XboxDeviceState* child)
{
	OHCI_AsyncCancelDevice(child);
}

void OHCI::OHCI_Wakeup(USBPort* port1)
{
	OHCIPort* port = &m_Registers.RhPort[port1->PortIndex];
	uint32_t intr = 0;
	if (port->HcRhPortStatus & OHCI_PORT_PSS) {
		EmuLog(LOG_LEVEL::DEBUG, "port %d: wakeup", port1->PortIndex);
		port->HcRhPortStatus |= OHCI_PORT_PSSC;
		port->HcRhPortStatus &= ~OHCI_PORT_PSS;
		intr = OHCI_INTR_RHSC;
	}
	// Note that the controller can be suspended even if this port is not
	if ((m_Registers.HcControl & OHCI_CTL_HCFS) == Suspend) {
		EmuLog(LOG_LEVEL::DEBUG, "remote-wakeup: SUSPEND->RESUME");
		// From the OHCI standard: "The only interrupts possible in the USBSUSPEND state are ResumeDetected (the
		// Host Controller will have changed the HostControllerFunctionalState to the USBRESUME state)
		// and OwnershipChange."
		m_Registers.HcControl &= ~OHCI_CTL_HCFS;
		m_Registers.HcControl |= Resume;
		intr = OHCI_INTR_RD;
	}
	OHCI_SetInterrupt(intr);
}

void OHCI::OHCI_AsyncCompletePacket(USBPort* port, USBPacket* packet)
{
#ifdef DEBUG_PACKET
	printf("Async packet complete\n");
#endif
	m_AsyncComplete = 1;
	OHCI_ProcessLists(1);
}

void OHCI::OHCI_AsyncCancelDevice(XboxDeviceState* dev)
{
	if (m_AsyncTD &&
		m_UsbDevice->USB_IsPacketInflight(&m_UsbPacket) &&
		m_UsbPacket.Endpoint->Dev == dev) {
		m_UsbDevice->USB_CancelPacket(&m_UsbPacket);
		m_AsyncTD = xbox::zero;
	}
}

void OHCI::OHCI_ProcessLists(int completion)
{
	// Only process the control list if it is enabled (HcControl) and has available TD's (HcCommandStatus)
	if ((m_Registers.HcControl & OHCI_CTL_CLE) && (m_Registers.HcCommandStatus & OHCI_STATUS_CLF)) {
		if (m_Registers.HcControlCurrentED && m_Registers.HcControlCurrentED != m_Registers.HcControlHeadED) {
			EmuLog(LOG_LEVEL::DEBUG, "head 0x%X, current 0x%X", m_Registers.HcControlHeadED, m_Registers.HcControlCurrentED);
		}
		if (!OHCI_ServiceEDlist(m_Registers.HcControlHeadED, completion)) {
			m_Registers.HcControlCurrentED = 0;
			m_Registers.HcCommandStatus &= ~OHCI_STATUS_CLF;
		}
	}

	// Only process the bulk list if it is enabled (HcControl) and has available TD's (HcCommandStatus)
	if ((m_Registers.HcControl & OHCI_CTL_BLE) && (m_Registers.HcCommandStatus & OHCI_STATUS_BLF)) {
		if (!OHCI_ServiceEDlist(m_Registers.HcBulkHeadED, completion)) {
			m_Registers.HcBulkCurrentED = 0;
			m_Registers.HcCommandStatus &= ~OHCI_STATUS_BLF;
		}
	}
}

int OHCI::OHCI_ServiceIsoTD(OHCI_ED* ed, int completion)
{
	int dir;
	int len = 0;
#ifdef DEBUG_ISOCH
	const char* str = nullptr;
#endif
	int pid;
	int ret;
	int i;
	XboxDeviceState* dev;
	USBEndpoint* ep;
	OHCI_ISO_TD iso_td;
	uint32_t addr;
	uint16_t starting_frame;
	int16_t relative_frame_number;
	int frame_count;
	uint32_t start_offset, next_offset, end_offset = 0;
	uint32_t start_addr, end_addr;

	addr = ed->HeadP & OHCI_DPTR_MASK;

	if (OHCI_ReadIsoTD(addr, &iso_td)) {
		EmuLog(LOG_LEVEL::ERROR2, "ISO_TD read error at physical address 0x%X", addr);
		OHCI_FatalError();
		return 0;
	}

	starting_frame = OHCI_BM(iso_td.Flags, TD_SF);
	frame_count = OHCI_BM(iso_td.Flags, TD_FC);
	// From the OHCI standard: "The Host Controller does an unsigned subtraction of StartingFrame from the 16 bits of
	// HcFmNumber to arrive at a signed value for a relative frame number (frame R)."
	relative_frame_number = USUB(m_Registers.HcFmNumber & 0xFFFF, starting_frame);

#ifdef DEBUG_ISOCH
	printf("--- ISO_TD ED head 0x%.8X tailp 0x%.8X\n"
		"0x%.8X 0x%.8X 0x%.8X 0x%.8X\n"
		"0x%.8X 0x%.8X 0x%.8X 0x%.8X\n"
		"0x%.8X 0x%.8X 0x%.8X 0x%.8X\n"
		"frame_number 0x%.8X starting_frame 0x%.8X\n"
		"frame_count  0x%.8X relative %d\n"
		"di 0x%.8X cc 0x%.8X\n",
		ed->HeadP & OHCI_DPTR_MASK, ed->TailP & OHCI_DPTR_MASK,
		iso_td.Flags, iso_td.BufferPage0, iso_td.NextTD, iso_td.BufferEnd,
		iso_td.Offset[0], iso_td.Offset[1], iso_td.Offset[2], iso_td.Offset[3],
		iso_td.Offset[4], iso_td.Offset[5], iso_td.Offset[6], iso_td.Offset[7],
		m_Registers.HcFmNumber, starting_frame,
		frame_count, relative_frame_number,
		OHCI_BM(iso_td.Flags, TD_DI), OHCI_BM(iso_td.Flags, TD_CC));
#endif

	if (relative_frame_number < 0) {
		// From the OHCI standard: "If the relative frame number is negative, then the current frame is earlier than the 0th frame
		// of the Isochronous TD and the Host Controller advances to the next ED."
		EmuLog(LOG_LEVEL::DEBUG, "ISO_TD R=%d < 0", relative_frame_number);
		return 1;
	}
	else if (relative_frame_number > frame_count) {
		// From the OHCI standard: "If the relative frame number is greater than
		// FrameCount, then the Isochronous TD has expired and a error condition exists."
		EmuLog(LOG_LEVEL::DEBUG, "ISO_TD R=%d > FC=%d", relative_frame_number, frame_count);
		OHCI_SET_BM(iso_td.Flags, TD_CC, OHCI_CC_DATAOVERRUN);
		ed->HeadP &= ~OHCI_DPTR_MASK;
		ed->HeadP |= (iso_td.NextTD & OHCI_DPTR_MASK);
		iso_td.NextTD = m_Registers.HcDoneHead;
		m_Registers.HcDoneHead = addr;
		i = OHCI_BM(iso_td.Flags, TD_DI);
		if (i < m_DoneCount) {
			m_DoneCount = i;
		}
		if (OHCI_WriteIsoTD(addr, &iso_td)) {
			OHCI_FatalError();
			return 1;
		}
		return 0;
	}

	// From the OHCI standard: "If the relative frame number is between 0 and FrameCount, then the Host Controller issues
	// a token to the endpoint and attempts a data transfer using the buffer described by the Isochronous TD."	

	dir = OHCI_BM(ed->Flags, ED_D);
	switch (dir) {
		case OHCI_TD_DIR_IN:
#ifdef DEBUG_ISOCH
			str = "in";
#endif
			pid = USB_TOKEN_IN;
			break;
		case OHCI_TD_DIR_OUT:
#ifdef DEBUG_ISOCH
			str = "out";
#endif
			pid = USB_TOKEN_OUT;
			break;
		case OHCI_TD_DIR_SETUP:
#ifdef DEBUG_ISOCH
			str = "setup";
#endif
			pid = USB_TOKEN_SETUP;
			break;
		default:
			EmuLog(LOG_LEVEL::WARNING, "Bad direction %d", dir);
			return 1;
	}

	if (!iso_td.BufferPage0 || !iso_td.BufferEnd) {
		EmuLog(LOG_LEVEL::DEBUG, "ISO_TD bp 0x%.8X be 0x%.8X", iso_td.BufferPage0, iso_td.BufferEnd);
		return 1;
	}

	start_offset = iso_td.Offset[relative_frame_number];
	next_offset = iso_td.Offset[relative_frame_number + 1];

	// From the OHCI standard: "If the Host Controller supports checking of the Offsets, if either Offset[R] or Offset[R+1] does
	// not have a ConditionCode of NOT ACCESSED or if the Offset[R + 1] is not greater than or equal to Offset[R], then
	// an Unrecoverable Error is indicated."
	// ergo720: I have a doubt here: according to the standard, the error condition is set if ConditionCode (bits 12-15 of
	// Offset[R(+1)] is not 111x (= NOT ACCESSED), however the check below is only triggered if the bits are all zeros
	// (= NO ERROR). So, if, for example, these bits are 1100 (= BUFFER OVERRUN), the check won't be triggered when actually
	// it should be

	if (!(OHCI_BM(start_offset, TD_PSW_CC) & 0xE) ||
		((relative_frame_number < frame_count) &&
			!(OHCI_BM(next_offset, TD_PSW_CC) & 0xE))) {
		EmuLog(LOG_LEVEL::DEBUG, "ISO_TD cc != not accessed 0x%.8x 0x%.8x", start_offset, next_offset);
		return 1;
	}

	if ((relative_frame_number < frame_count) && (start_offset > next_offset)) {
		printf("ISO_TD start_offset=0x%.8x > next_offset=0x%.8x\n", start_offset, next_offset);
		return 1;
	}

	// From the OHCI standard: "Bit 12 of offset R then selects the upper 20 bits of the physical address
	// as either BufferPage0 when bit 12 = 0 or the upper 20 bits of BufferEnd when bit 12 = 1."
		
	if ((start_offset & 0x1000) == 0) {
		start_addr = (iso_td.BufferPage0 & OHCI_PAGE_MASK) |
			(start_offset & OHCI_OFFSET_MASK);
	}
	else {
		start_addr = (iso_td.BufferEnd & OHCI_PAGE_MASK) |
			(start_offset & OHCI_OFFSET_MASK);
	}

	// From the OHCI standard: "If the data packet is not the last in an Isochronous TD (R not equal to FrameCount),
	// then the ending address of the buffer is found by using Offset[R + 1] - 1. This value is then used to create a
	// physical address in the same manner as the Offset[R] was used to create the starting physical address."	

	if (relative_frame_number < frame_count) {
		end_offset = next_offset - 1;
		if ((end_offset & 0x1000) == 0) {
			end_addr = (iso_td.BufferPage0 & OHCI_PAGE_MASK) |
				(end_offset & OHCI_OFFSET_MASK);
		}
		else {
			end_addr = (iso_td.BufferEnd & OHCI_PAGE_MASK) |
				(end_offset & OHCI_OFFSET_MASK);
		}
	}
	else {
		// From the OHCI standard: "If, however, the data packet is the last in an Isochronous TD(R = FrameCount),
		// then the value of BufferEnd is the address of the last byte in the buffer."	
		end_addr = iso_td.BufferEnd;
	}

	if ((start_addr & OHCI_PAGE_MASK) != (end_addr & OHCI_PAGE_MASK)) {
		len = (end_addr & OHCI_OFFSET_MASK) + 0x1001
			- (start_addr & OHCI_OFFSET_MASK);
	}
	else {
		len = end_addr - start_addr + 1;
	}

	if (len && dir != OHCI_TD_DIR_IN) {
		if (OHCI_CopyIsoTDBuffer(start_addr, end_addr, m_UsbBuffer, len, false)) {
			OHCI_FatalError();
			return 1;
		}
	}

	if (!completion) {
		bool int_req = relative_frame_number == frame_count && OHCI_BM(iso_td.Flags, TD_DI) == 0;
		dev = OHCI_FindDevice(OHCI_BM(ed->Flags, ED_FA));
		ep = m_UsbDevice->USB_GetEP(dev, pid, OHCI_BM(ed->Flags, ED_EN));
		m_UsbDevice->USB_PacketSetup(&m_UsbPacket, pid, ep, addr, false, int_req);
		m_UsbDevice->USB_PacketAddBuffer(&m_UsbPacket, m_UsbBuffer, len);
		m_UsbDevice->USB_HandlePacket(dev, &m_UsbPacket);
		if (m_UsbPacket.Status == USB_RET_ASYNC) {
			m_UsbDevice->USB_DeviceFlushEPqueue(dev, ep);
			return 1;
		}
	}
	if (m_UsbPacket.Status == USB_RET_SUCCESS) {
		ret = m_UsbPacket.ActualLength;
	}
	else {
		ret = m_UsbPacket.Status;
	}

#ifdef DEBUG_ISOCH
	printf("so 0x%.8X eo 0x%.8X\nsa 0x%.8X ea 0x%.8X\ndir %s len %zu ret %d\n",
		start_offset, end_offset, start_addr, end_addr, str, len, ret);
#endif

	// From the OHCI standard: "After each data packet transfer, the Rth Offset is replaced with a value that indicates the status of
	// the data packet transfer.The upper 4 bits of the value are the ConditionCode for the transfer and the lower 12 bits
	// represent the size of the transfer.Together, these two fields constitute the Packet Status Word(PacketStatusWord)."

	// Writeback
	if (dir == OHCI_TD_DIR_IN && ret >= 0 && ret <= len) {
		// IN transfer succeeded
		if (OHCI_CopyIsoTDBuffer(start_addr, end_addr, m_UsbBuffer, ret, true)) {
			OHCI_FatalError();
			return 1;
		}
		OHCI_SET_BM(iso_td.Offset[relative_frame_number], TD_PSW_CC, OHCI_CC_NOERROR);
		OHCI_SET_BM(iso_td.Offset[relative_frame_number], TD_PSW_SIZE, ret);
	}
	else if (dir == OHCI_TD_DIR_OUT && ret == len) {
		// OUT transfer succeeded
		OHCI_SET_BM(iso_td.Offset[relative_frame_number], TD_PSW_CC, OHCI_CC_NOERROR);
		OHCI_SET_BM(iso_td.Offset[relative_frame_number], TD_PSW_SIZE, 0);
	}
	else {
		// Handle the error condition
		if (ret > static_cast<ptrdiff_t>(len)) { // Sequence Error
			EmuLog(LOG_LEVEL::DEBUG, "DataOverrun %d > %zu", ret, len);
			OHCI_SET_BM(iso_td.Offset[relative_frame_number], TD_PSW_CC, OHCI_CC_DATAOVERRUN);
			OHCI_SET_BM(iso_td.Offset[relative_frame_number], TD_PSW_SIZE, len);
		}
		else if (ret >= 0) { // Sequence Error
			EmuLog(LOG_LEVEL::DEBUG, "DataUnderrun %d", ret);
			OHCI_SET_BM(iso_td.Offset[relative_frame_number], TD_PSW_CC, OHCI_CC_DATAUNDERRUN);
		}
		else {
			switch (ret) {
				case USB_RET_IOERROR: // Transmission Errors
				case USB_RET_NODEV:
					OHCI_SET_BM(iso_td.Offset[relative_frame_number], TD_PSW_CC, OHCI_CC_DEVICENOTRESPONDING);
					OHCI_SET_BM(iso_td.Offset[relative_frame_number], TD_PSW_SIZE, 0);
					break;
				case USB_RET_NAK: // NAK and STALL
				case USB_RET_STALL:
					EmuLog(LOG_LEVEL::DEBUG, "got NAK/STALL %d", ret);
					OHCI_SET_BM(iso_td.Offset[relative_frame_number], TD_PSW_CC, OHCI_CC_STALL);
					OHCI_SET_BM(iso_td.Offset[relative_frame_number], TD_PSW_SIZE, 0);
					break;
				default: // Unknown Error
					EmuLog(LOG_LEVEL::DEBUG, "Bad device response %d", ret);
					OHCI_SET_BM(iso_td.Offset[relative_frame_number], TD_PSW_CC, OHCI_CC_UNDEXPETEDPID);
					break;
			}
		}
	}

	if (relative_frame_number == frame_count) {
		// Last data packet of ISO TD - retire the TD to the Done Queue
		OHCI_SET_BM(iso_td.Flags, TD_CC, OHCI_CC_NOERROR);
		ed->HeadP &= ~OHCI_DPTR_MASK;
		ed->HeadP |= (iso_td.NextTD & OHCI_DPTR_MASK);
		iso_td.NextTD = m_Registers.HcDoneHead;
		m_Registers.HcDoneHead = addr;
		i = OHCI_BM(iso_td.Flags, TD_DI);
		if (i < m_DoneCount) {
			m_DoneCount = i;
		}
	}
	if (OHCI_WriteIsoTD(addr, &iso_td)) {
		OHCI_FatalError();
	}
	return 1;
}
