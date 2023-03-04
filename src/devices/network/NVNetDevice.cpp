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
// *  Copyright (c) 2013 espes
// *  Copyright (c) 2015 Matt Borgerson
// *  (c) 2017 Luke Usher <luke.usher@outlook.com>
// * 
// * This file is based on code from the XQEMU Project
// * https://github.com/xqemu/xqemu/blob/xbox/hw/xbox/nvnet.c
// *
// *  All rights reserved
// *
// ******************************************************************

#define LOG_PREFIX CXBXR_MODULE::NET


#include <core\kernel\exports\xboxkrnl.h> // For PKINTERRUPT, etc.
#include <WinSock2.h> 
#include "core\kernel\init\CxbxKrnl.h"
#include "core\kernel\support\Emu.h"
#include "core\kernel\exports\EmuKrnl.h"
#include "EmuShared.h"
#include "devices\Xbox.h"
#include "NVNetDevice.h"
#include <Iphlpapi.h>
#include <pcap.h>
#include <exception>

#define IOPORT_SIZE 0x8
#define MMIO_SIZE   0x400

#define NV_TX_LASTPACKET      (1<<0)
#define NV_TX_RETRYERROR      (1<<3)
#define NV_TX_LASTPACKET1     (1<<8)
#define NV_TX_DEFERRED        (1<<10)
#define NV_TX_CARRIERLOST     (1<<11)
#define NV_TX_LATECOLLISION   (1<<12)
#define NV_TX_UNDERFLOW       (1<<13)
#define NV_TX_ERROR           (1<<14)
#define NV_TX_VALID           (1<<15)
#define NV_RX_DESCRIPTORVALID (1<<0)
#define NV_RX_MISSEDFRAME     (1<<1)
#define NV_RX_SUBSTRACT1      (1<<3)
#define NV_RX_BIT4            (1<<4)
#define NV_RX_ERROR1          (1<<7)
#define NV_RX_ERROR2          (1<<8)
#define NV_RX_ERROR3          (1<<9)
#define NV_RX_ERROR4          (1<<10)
#define NV_RX_CRCERR          (1<<11)
#define NV_RX_OVERFLOW        (1<<12)
#define NV_RX_FRAMINGERR      (1<<13)
#define NV_RX_ERROR           (1<<14)
#define NV_RX_AVAIL           (1<<15)

/* Miscelaneous hardware related defines: */
#define NV_PCI_REGSZ          0x270

/* various timeout delays: all in usec */
#define NV_TXRX_RESET_DELAY   4
#define NV_TXSTOP_DELAY1      10
#define NV_TXSTOP_DELAY1MAX   500000
#define NV_TXSTOP_DELAY2      100
#define NV_RXSTOP_DELAY1      10
#define NV_RXSTOP_DELAY1MAX   500000
#define NV_RXSTOP_DELAY2      100
#define NV_SETUP5_DELAY       5
#define NV_SETUP5_DELAYMAX    50000
#define NV_POWERUP_DELAY      5
#define NV_POWERUP_DELAYMAX   5000
#define NV_MIIBUSY_DELAY      50
#define NV_MIIPHY_DELAY       10
#define NV_MIIPHY_DELAYMAX    10000
#define NV_WAKEUPPATTERNS     5
#define NV_WAKEUPMASKENTRIES  4

/* General driver defaults */
#define NV_WATCHDOG_TIMEO     (2*HZ)
#define DEFAULT_MTU           1500

#define RX_RING               4
#define TX_RING               2
/* limited to 1 packet until we understand NV_TX_LASTPACKET */
#define TX_LIMIT_STOP         10
#define TX_LIMIT_START        5

/* rx/tx mac addr + type + vlan + align + slack*/
#define RX_NIC_BUFSIZE        (DEFAULT_MTU + 64)
/* even more slack */
#define RX_ALLOC_BUFSIZE      (DEFAULT_MTU + 128)

#define OOM_REFILL            (1+HZ/20)
#define POLL_WAIT             (1+HZ/100)

#define MII_READ      (-1)
#define MII_PHYSID1   0x02    /* PHYS ID 1                   */
#define MII_PHYSID2   0x03    /* PHYS ID 2                   */
#define MII_BMCR      0x00    /* Basic mode control register */
#define MII_BMSR      0x01    /* Basic mode status register  */
#define MII_ADVERTISE 0x04    /* Advertisement control reg   */
#define MII_LPA       0x05    /* Link partner ability reg    */

#define BMSR_ANEGCOMPLETE 0x0020 /* Auto-negotiation complete   */
#define BMSR_BIT2         0x0004 /* Unknown... */

/* Link partner ability register. */
#define LPA_SLCT     0x001f  /* Same as advertise selector  */
#define LPA_10HALF   0x0020  /* Can do 10mbps half-duplex   */
#define LPA_10FULL   0x0040  /* Can do 10mbps full-duplex   */
#define LPA_100HALF  0x0080  /* Can do 100mbps half-duplex  */
#define LPA_100FULL  0x0100  /* Can do 100mbps full-duplex  */
#define LPA_100BASE4 0x0200  /* Can do 100mbps 4k packets   */
#define LPA_RESV     0x1c00  /* Unused...                   */
#define LPA_RFAULT   0x2000  /* Link partner faulted        */
#define LPA_LPACK    0x4000  /* Link partner acked us       */
#define LPA_NPAGE    0x8000  /* Next page bit               */

/*******************************************************************************
* Primary State Structure
******************************************************************************/

typedef struct NvNetState {
	uint8_t      regs[MMIO_SIZE / 4];
	uint32_t     phy_regs[6];
	uint8_t      tx_ring_index;
	uint8_t      tx_ring_size;
	uint8_t      rx_ring_index;
	uint8_t      rx_ring_size;
	uint8_t      txrx_dma_buf[RX_ALLOC_BUFSIZE];
	FILE         *packet_dump_file;
	char         *packet_dump_path;
} NvNetState_t;

NvNetState_t NvNetState;

struct RingDesc {
	uint32_t packet_buffer;
	uint16_t length;
	uint16_t flags;
};

const char* EmuNVNet_GetRegisterName(xbox::addr_xt addr)
{
	switch (addr) {
	case NvRegIrqStatus:             return "NvRegIrqStatus";
	case NvRegIrqMask:               return "NvRegIrqMask";
	case NvRegUnknownSetupReg6:      return "NvRegUnknownSetupReg6";
	case NvRegPollingInterval:       return "NvRegPollingInterval";
	case NvRegMisc1:                 return "NvRegMisc1";
	case NvRegTransmitterControl:    return "NvRegTransmitterControl";
	case NvRegTransmitterStatus:     return "NvRegTransmitterStatus";
	case NvRegPacketFilterFlags:     return "NvRegPacketFilterFlags";
	case NvRegOffloadConfig:         return "NvRegOffloadConfig";
	case NvRegReceiverControl:       return "NvRegReceiverControl";
	case NvRegReceiverStatus:        return "NvRegReceiverStatus";
	case NvRegRandomSeed:            return "NvRegRandomSeed";
	case NvRegUnknownSetupReg1:      return "NvRegUnknownSetupReg1";
	case NvRegUnknownSetupReg2:      return "NvRegUnknownSetupReg2";
	case NvRegMacAddrA:              return "NvRegMacAddrA";
	case NvRegMacAddrB:              return "NvRegMacAddrB";
	case NvRegMulticastAddrA:        return "NvRegMulticastAddrA";
	case NvRegMulticastAddrB:        return "NvRegMulticastAddrB";
	case NvRegMulticastMaskA:        return "NvRegMulticastMaskA";
	case NvRegMulticastMaskB:        return "NvRegMulticastMaskB";
	case NvRegTxRingPhysAddr:        return "NvRegTxRingPhysAddr";
	case NvRegRxRingPhysAddr:        return "NvRegRxRingPhysAddr";
	case NvRegRingSizes:             return "NvRegRingSizes";
	case NvRegUnknownTransmitterReg: return "NvRegUnknownTransmitterReg";
	case NvRegLinkSpeed:             return "NvRegLinkSpeed";
	case NvRegUnknownSetupReg5:      return "NvRegUnknownSetupReg5";
	case NvRegUnknownSetupReg3:      return "NvRegUnknownSetupReg3";
	case NvRegUnknownSetupReg8:      return "NvRegUnknownSetupReg8";
	case NvRegUnknownSetupReg7:      return "NvRegUnknownSetupReg7";
	case NvRegTxRxControl:           return "NvRegTxRxControl";
	case NvRegMIIStatus:             return "NvRegMIIStatus";
	case NvRegUnknownSetupReg4:      return "NvRegUnknownSetupReg4";
	case NvRegAdapterControl:        return "NvRegAdapterControl";
	case NvRegMIISpeed:              return "NvRegMIISpeed";
	case NvRegMIIControl:            return "NvRegMIIControl";
	case NvRegMIIData:               return "NvRegMIIData";
	case NvRegWakeUpFlags:           return "NvRegWakeUpFlags";
	case NvRegPatternCRC:            return "NvRegPatternCRC";
	case NvRegPatternMask:           return "NvRegPatternMask";
	case NvRegPowerCap:              return "NvRegPowerCap";
	case NvRegPowerState:            return "NvRegPowerState";
	default:                         return "Unknown";
	}
}

const char* EmuNVNet_GetMiiRegisterName(uint8_t reg)
{
	switch (reg) {
	case MII_PHYSID1:   return "MII_PHYSID1";
	case MII_PHYSID2:   return "MII_PHYSID2";
	case MII_BMCR:      return "MII_BMCR";
	case MII_BMSR:      return "MII_BMSR";
	case MII_ADVERTISE: return "MII_ADVERTISE";
	case MII_LPA:       return "MII_LPA";
	default:            return "Unknown";
	}
}

uint32_t EmuNVNet_GetRegister(xbox::addr_xt addr, unsigned int size)
{
	switch (size) {
	case sizeof(uint32_t) :
		return ((uint32_t *)NvNetState.regs)[addr >> 2];
	case sizeof(uint16_t) :
		return ((uint16_t *)NvNetState.regs)[addr >> 1];
	case sizeof(uint8_t) :
		return NvNetState.regs[addr];
	}

	return 0;
}

void EmuNVNet_SetRegister(xbox::addr_xt addr, uint32_t value, unsigned int size)
{
	switch (size) {
	case sizeof(uint32_t):
		((uint32_t *)NvNetState.regs)[addr >> 2] = value;
		break;
	case sizeof(uint16_t):
		((uint16_t *)NvNetState.regs)[addr >> 1] = (uint16_t)value;
		break;
	case sizeof(uint8_t):
		NvNetState.regs[addr] = (uint8_t)value;
		break;
	}
}

void EmuNVNet_UpdateIRQ()
{
	if (EmuNVNet_GetRegister(NvRegIrqMask, 4) &&
		EmuNVNet_GetRegister(NvRegIrqStatus, 4)) {
		EmuLog(LOG_LEVEL::DEBUG, "Asserting IRQ");
		HalSystemInterrupts[4].Assert(true);
	} else {
		HalSystemInterrupts[4].Assert(false);
	}
}

int EmuNVNet_MiiReadWrite(uint64_t val)
{
	uint32_t mii_ctl;
	int write, retval, phy_addr, reg;
	
	retval = 0;
	mii_ctl = EmuNVNet_GetRegister(NvRegMIIControl, 4);

	phy_addr = (mii_ctl >> NVREG_MIICTL_ADDRSHIFT) & 0x1f;
	reg = mii_ctl & ((1 << NVREG_MIICTL_ADDRSHIFT) - 1);
	write = mii_ctl & NVREG_MIICTL_WRITE;

	EmuLog(LOG_LEVEL::DEBUG, "nvnet mii %s: phy 0x%x %s [0x%x]", write ? "write" : "read", phy_addr, EmuNVNet_GetMiiRegisterName(reg), reg);

	if (phy_addr != 1) {
		return -1;
	}

	if (write) {
		return retval;
	}

	switch (reg) {
	case MII_BMSR:
		/* Phy initialization code waits for BIT2 to be set.. If not set,
		* software may report controller as not running */
		retval = BMSR_ANEGCOMPLETE | BMSR_BIT2;
		break;
	case MII_ADVERTISE:
		/* Fall through... */
	case MII_LPA:
		retval = LPA_10HALF | LPA_10FULL;
		retval |= LPA_100HALF | LPA_100FULL | LPA_100BASE4;
		break;
	default:
		break;
	}

	return retval;
}

uint32_t EmuNVNet_Read(xbox::addr_xt addr, int size)
{
	EmuLog(LOG_LEVEL::DEBUG, "Read%d: %s (0x%.8X)", size * 8, EmuNVNet_GetRegisterName(addr), addr);

	switch (addr) {
		case NvRegMIIData:
			return EmuNVNet_MiiReadWrite(MII_READ);
		case NvRegMIIControl:
			return EmuNVNet_GetRegister(addr, size) & ~NVREG_MIICTL_INUSE;
		case NvRegMIIStatus:
			return 0;
	}

	return EmuNVNet_GetRegister(addr,size);
}

void EmuNVNet_DMAPacketFromGuest()
{
	struct RingDesc desc;
	bool is_last_packet;
	bool packet_sent = false;

	NvNetState_t* s = &NvNetState;

	for (int i = 0; i < s->tx_ring_size; i++) {
		/* Read ring descriptor */
		s->tx_ring_index %= s->tx_ring_size;
		xbox::addr_xt tx_ring_addr = EmuNVNet_GetRegister(NvRegTxRingPhysAddr, 4);
		tx_ring_addr += s->tx_ring_index * sizeof(desc);

		memcpy(&desc, (void*)(tx_ring_addr | CONTIGUOUS_MEMORY_BASE), sizeof(desc));

		EmuLog(LOG_LEVEL::DEBUG, "Looking at ring desc %d (%llx): "
		                         "\n   Buffer: 0x%x "
		                         "\n   Length: 0x%x "
		                         "\n   Flags:  0x%x ",
		                         s->tx_ring_index, tx_ring_addr, desc.packet_buffer, desc.length, desc.flags);

		s->tx_ring_index += 1;

		if (!(desc.flags & NV_TX_VALID)) {
			continue;
		}

		/* Transfer packet from guest memory */
		EmuLog(LOG_LEVEL::DEBUG, "Sending packet...");

		memcpy(s->txrx_dma_buf, (void*)(desc.packet_buffer | CONTIGUOUS_MEMORY_BASE), desc.length + 1);
		g_NVNet->PCAPSend(s->txrx_dma_buf, desc.length + 1);

		packet_sent = true;

		/* Update descriptor */
		is_last_packet = desc.flags & NV_TX_LASTPACKET;
		desc.flags &= ~(NV_TX_VALID | NV_TX_RETRYERROR | NV_TX_DEFERRED | NV_TX_CARRIERLOST | NV_TX_LATECOLLISION | NV_TX_UNDERFLOW | NV_TX_ERROR);
		desc.length = desc.length + 5;

		memcpy((void*)(tx_ring_addr | CONTIGUOUS_MEMORY_BASE), &desc, sizeof(desc));

		if (is_last_packet) {
			EmuLog(LOG_LEVEL::DEBUG, "  -- Last packet");
			break;
		}
	}

	if (packet_sent) {
		/* Trigger interrupt */
		EmuLog(LOG_LEVEL::DEBUG, "Triggering interrupt");
		EmuNVNet_SetRegister(NvRegIrqStatus, NVREG_IRQSTAT_BIT4, 4);
		EmuNVNet_UpdateIRQ();
	}
}

bool EmuNVNet_DMAPacketToGuest(void* packet, size_t size)
{
	struct RingDesc desc;

	NvNetState_t* s = &NvNetState;

	for (int i = 0; i < s->rx_ring_size; i++) {
		/* Read current ring descriptor */
		s->rx_ring_index %= s->rx_ring_size;
		xbox::addr_xt rx_ring_addr = EmuNVNet_GetRegister(NvRegRxRingPhysAddr, 4);
		rx_ring_addr += s->rx_ring_index * sizeof(desc);
		
		memcpy(&desc, (void*)(rx_ring_addr | CONTIGUOUS_MEMORY_BASE), sizeof(desc));

        EmuLog(LOG_LEVEL::DEBUG, "Looking at ring descriptor %d (0x%llx): "
                                 "\n   Buffer: 0x%x "
                                 "\n   Length: 0x%x "
                                 "\n   Flags:  0x%x ",
                                 s->rx_ring_index, rx_ring_addr, desc.packet_buffer, desc.length, desc.flags);

		s->rx_ring_index += 1;

		if (!(desc.flags & NV_RX_AVAIL) || !(desc.length >= size)) {
			continue;
		}

		/* Transfer packet from device to memory */
		EmuLog(LOG_LEVEL::DEBUG, "Transferring packet, size 0x%zx, to memory at 0x%x", size, desc.packet_buffer);
		memcpy((void*)(desc.packet_buffer | CONTIGUOUS_MEMORY_BASE), packet, size);

		/* Update descriptor indicating the packet is waiting */
		desc.length = (uint16_t)size;
		desc.flags = NV_RX_BIT4 | NV_RX_DESCRIPTORVALID;
		memcpy((void*)(rx_ring_addr | CONTIGUOUS_MEMORY_BASE), &desc, sizeof(desc));
        EmuLog(LOG_LEVEL::DEBUG, "Updated ring descriptor: "
                                 "\n   Length: 0x%x "
                                 "\n   Flags:  0x%x ",
                                 desc.flags, desc.length);

		/* Trigger interrupt */
		EmuLog(LOG_LEVEL::DEBUG, "Triggering interrupt");
		EmuNVNet_SetRegister(NvRegIrqStatus, NVREG_IRQSTAT_BIT1, 4);
		EmuNVNet_UpdateIRQ();
		return true;
	}

	/* Could not find free buffer, or packet too large. */
	EmuLog(LOG_LEVEL::DEBUG, "Could not find free buffer!");
	return false;
}

void EmuNVNet_Write(xbox::addr_xt addr, uint32_t value, int size)
{
	switch (addr) {
	case NvRegRingSizes:
		EmuNVNet_SetRegister(addr, value, size);
		NvNetState.rx_ring_size = ((value >> NVREG_RINGSZ_RXSHIFT) & 0xffff) + 1;
		NvNetState.tx_ring_size = ((value >> NVREG_RINGSZ_TXSHIFT) & 0xffff) + 1;
		break;
	case NvRegMIIData:
		EmuNVNet_MiiReadWrite(value);
		break;
	case NvRegTxRxControl:
		if (value == NVREG_TXRXCTL_KICK) {
			EmuLog(LOG_LEVEL::DEBUG, "NvRegTxRxControl = NVREG_TXRXCTL_KICK!");
			EmuNVNet_DMAPacketFromGuest();
		}

		if (value & NVREG_TXRXCTL_BIT2) {
			EmuNVNet_SetRegister(NvRegTxRxControl, NVREG_TXRXCTL_IDLE, 4);
			break;
		}

		if (value & NVREG_TXRXCTL_BIT1) {
			EmuNVNet_SetRegister(NvRegIrqStatus, 0, 4);
			break;
		} else if (value == 0) {
			uint32_t temp = EmuNVNet_GetRegister(NvRegUnknownSetupReg3, 4);
			if (temp == NVREG_UNKSETUP3_VAL1) {
				/* forcedeth waits for this bit to be set... */
				EmuNVNet_SetRegister(NvRegUnknownSetupReg5,
					NVREG_UNKSETUP5_BIT31, 4);
				break;
			}
		}
		EmuNVNet_SetRegister(NvRegTxRxControl, value, size);
		break;
	case NvRegIrqMask:
		EmuNVNet_SetRegister(addr, value, size);
		EmuNVNet_UpdateIRQ();
		break;
	case NvRegIrqStatus:
		EmuNVNet_SetRegister(addr, EmuNVNet_GetRegister(addr, size) & ~value, size);
		EmuNVNet_UpdateIRQ();
		break;
	default:
		EmuNVNet_SetRegister(addr, value, size);
		break;
	}

	EmuLog(LOG_LEVEL::DEBUG, "Write%d: %s (0x%.8X) = 0x%.8X", size * 8, EmuNVNet_GetRegisterName(addr), addr, value);
}

void NVNetRecv()
{
	static std::unique_ptr<uint8_t[]> packet(new uint8_t[65536]);
	int size = g_NVNet->PCAPReceive(packet.get(), 65536);
	if (size > 0) {
		EmuNVNet_DMAPacketToGuest(packet.get(), size);
	}
}

/* NVNetDevice */

// PCI Device functions

void NVNetDevice::Init()
{
	PCIBarRegister r;

	// Register Memory bar :
	r.Raw.type = PCI_BAR_TYPE_MEMORY;
	r.Memory.address = NVNET_BASE >> 4;
	RegisterBAR(0, NVNET_SIZE, r.value);

	// Register IO bar :
	r.Raw.type = PCI_BAR_TYPE_IO;
	r.IO.address = 0xE000;
	RegisterBAR(1, 8, r.value);

	m_DeviceId = 0x01C3;
	m_VendorId = PCI_VENDOR_ID_NVIDIA;

	memset(NvNetState.regs, 0, sizeof(NvNetState.regs));
	NvNetState.rx_ring_index = 0;
	NvNetState.rx_ring_size = 0;
	NvNetState.tx_ring_index = 0;
	NvNetState.tx_ring_size = 0;

	// Fetch Host Network Device
	Settings::s_network networkSettings;
	g_EmuShared->GetNetworkSettings(&networkSettings);
	m_HostAdapterName = networkSettings.adapter_name;

	// Get Mac Address
	if (!GetMacAddress(m_HostAdapterName, m_HostMacAddress.bytes)) {
		EmuLog(LOG_LEVEL::WARNING, "Failed to initialize network adapter.");
		return;
	};

	PCAPInit();
}

void NVNetDevice::Reset()
{
}

bool NVNetDevice::GetMacAddress(std::string adapterName, void* pMAC)
{
	// AdapterInfo is too large to be allocated on the stack, and will cause a crash in debug builds when _chkstk detects it
	auto adapterInfo = new IP_ADAPTER_INFO[128];
	PIP_ADAPTER_INFO pAdapterInfo = (PIP_ADAPTER_INFO)adapterInfo;

	ULONG dwBufferLength = sizeof(IP_ADAPTER_INFO) * 128;

	DWORD dwStatus = GetAdaptersInfo(pAdapterInfo, &dwBufferLength);
	if (dwStatus != ERROR_SUCCESS) {
		delete[] adapterInfo;
		return false;
	}

	// Find the specified adapter
	do {
		if (strcmp(pAdapterInfo->AdapterName, adapterName.c_str()) == 0) {
			memcpy(pMAC, pAdapterInfo->Address, 6);
			delete[] adapterInfo;
			return true;
		}

		pAdapterInfo = pAdapterInfo->Next;

	} while (pAdapterInfo);

	delete[] adapterInfo;
	return false;
}

uint32_t NVNetDevice::IORead(int barIndex, uint32_t port, unsigned size)
{
	if (barIndex != 1) {
		return 0;
	}

	return 0;
}

void NVNetDevice::IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size)
{
	if (barIndex != 1) {
		return;
	}

}

uint32_t NVNetDevice::MMIORead(int barIndex, uint32_t addr, unsigned size)
{
	if (barIndex != 0) {
		return 0;
	}

	return EmuNVNet_Read(addr, size); // For now, forward
}

void NVNetDevice::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size)
{
	if (barIndex != 0) {
		return;
	}

	EmuNVNet_Write(addr, value, size); // For now, forward

	// Cache guest MAC address for packet filter
	if (addr == NvRegMacAddrA) {
		m_GuestMacAddress.bytes[0] = NvNetState.regs[NvRegMacAddrA + 0];
		m_GuestMacAddress.bytes[1] = NvNetState.regs[NvRegMacAddrA + 1];
		m_GuestMacAddress.bytes[2] = NvNetState.regs[NvRegMacAddrA + 2];
		m_GuestMacAddress.bytes[3] = NvNetState.regs[NvRegMacAddrA + 3];
	}
	else if (addr == NvRegMacAddrB) {
		m_GuestMacAddress.bytes[4] = NvNetState.regs[NvRegMacAddrB + 0];
		m_GuestMacAddress.bytes[5] = NvNetState.regs[NvRegMacAddrB + 1];
	}
}

bool NVNetDevice::PCAPInit()
{
	char errorBuffer[PCAP_ERRBUF_SIZE];

	// Open the desired network adapter
#ifdef _MSC_VER // TODO: Implement loaded pcap driver detection for cross-platform support or make a requirement for non-Windows platform.
	__try {
#endif
		char buffer[MAX_PATH];
		snprintf(buffer, MAX_PATH, "\\Device\\NPF_%s", m_HostAdapterName.c_str());
		m_AdapterHandle = pcap_open_live(buffer,
			65536,	// Capture entire packet
			1,		// Use promiscuous mode
			1,		// Read Timeout
			errorBuffer
		);
#ifdef _MSC_VER
	} __except(EXCEPTION_EXECUTE_HANDLER) {
		m_AdapterHandle = nullptr;
		snprintf(errorBuffer, PCAP_ERRBUF_SIZE, "Could not initialize pcap");
	}
#endif

	if (m_AdapterHandle == nullptr) {
		EmuLog(LOG_LEVEL::WARNING, "Unable to open Network Adapter:\n%s\nNetworking will be disabled", errorBuffer);
		return false;
	}

	if (pcap_setnonblock((pcap_t*)m_AdapterHandle, 1, errorBuffer) == -1) {
		EmuLog(LOG_LEVEL::WARNING, "PCAP: Failed to set non-blocking mode");
	}

	m_PCAPRunning = true;
	return true;
}

void PrintRawPayload(void* buffer, size_t length)
{
	uint8_t* startAddr = (uint8_t*)buffer;

	printf("----Payload----\n");
	for (uint8_t* addr = startAddr; addr < (startAddr + length); addr += 16) {
		printf("%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\t|\t%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
			addr[0x0], addr[0x1], addr[0x2], addr[0x3], addr[0x4], addr[0x5], addr[0x6], addr[0x7], 
			addr[0x8], addr[0x9], addr[0xA], addr[0xB], addr[0xC], addr[0xD3], addr[0xE], addr[0xF],
			addr[0x0], addr[0x1], addr[0x2], addr[0x3], addr[0x4], addr[0x5], addr[0x6], addr[0x7],
			addr[0x8], addr[0x9], addr[0xA], addr[0xB], addr[0xC], addr[0xD3], addr[0xE], addr[0xF]
		);
	}
}

void PrintPacket(void* buffer, size_t length)
{
	ethernet_header* header = (ethernet_header*)buffer;
	printf("----Ethernet Frame----\n");
	printf("Src: %02X:%02X:%02X:%02X:%02X:%02X\n", header->src.bytes[0], header->src.bytes[1], header->src.bytes[2], header->src.bytes[3], header->src.bytes[4], header->src.bytes[5]);
	printf("Dst: %02X:%02X:%02X:%02X:%02X:%02X\n", header->dst.bytes[0], header->dst.bytes[1], header->dst.bytes[2], header->dst.bytes[3], header->dst.bytes[4], header->dst.bytes[5]);
	printf("EtherType: %04X\n", ntohs(header->protocol));

	void* payloadPtr = (void*)((uint8_t*)buffer + sizeof(ethernet_header));
	size_t payloadLength = length - sizeof(ethernet_header);

	// TODO: If we support the EtherType, decode it, otherwise, just dump the raw payload
	//switch (ntohs(header->protocol))
	{
	//	default:
			PrintRawPayload(payloadPtr, payloadLength);
	}
}	

bool NVNetDevice::PCAPSend(void* packet, size_t length)
{
	if (!m_PCAPRunning) {
		return false;
	}

	ethernet_header* header = (ethernet_header*)packet;

	// TODO: Optional
	// PrintPacket(packet, length);

	// Forward broadcast packets direct to the host PC, as well as over the network
	if (memcmp(header->dst.bytes, m_BroadcastMacAddress.bytes, 6) == 0) {
		static char pack[65536];
		memcpy(pack, packet, length);
		ethernet_header* _header = (ethernet_header*)pack;
		_header->dst = m_HostMacAddress;
		pcap_sendpacket((pcap_t*)m_AdapterHandle, (uint8_t*)pack, length);
	}

	return pcap_sendpacket((pcap_t*)m_AdapterHandle, (uint8_t*)packet, length);
}

size_t NVNetDevice::PCAPReceive(void* packet, size_t max_length)
{
	if (!m_PCAPRunning) {
		return -1;
	}

	struct pcap_pkthdr *header;
	const uint8_t *pkt_data;

	if (int res = pcap_next_ex((pcap_t*)m_AdapterHandle, &header, &pkt_data) > 0) {
		// Only forward packets that are broadcast or specifically for Cxbx-R's MAC
		ethernet_header* e_header = (ethernet_header*)pkt_data;
		if (memcmp(e_header->dst.bytes, m_GuestMacAddress.bytes, 6) == 0 || memcmp(e_header->dst.bytes, m_BroadcastMacAddress.bytes, 6) == 0) {
			memcpy(packet, pkt_data, header->len);
			return header->len;
		}

		return -1;
	}

	return -1;
}
