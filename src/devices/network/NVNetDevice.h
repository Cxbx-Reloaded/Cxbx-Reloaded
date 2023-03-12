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
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *  (c) 2017 Luke Usher <luke.usher@outlook.com>
// *  All rights reserved
// *
// ******************************************************************
#pragma once

#include <string>

#include "../PCIDevice.h" // For PCIDevice

// NVNET Register Definitions
// Taken from XQEMU
enum {
	NvRegIrqStatus = 0x000,
#       define NVREG_IRQSTAT_BIT1     0x002
#       define NVREG_IRQSTAT_BIT4     0x010
#       define NVREG_IRQSTAT_MIIEVENT 0x040
#       define NVREG_IRQSTAT_MASK     0x1ff
	NvRegIrqMask = 0x004,
#       define NVREG_IRQ_RX           0x0002
#       define NVREG_IRQ_RX_NOBUF     0x0004
#       define NVREG_IRQ_TX_ERR       0x0008
#       define NVREG_IRQ_TX2          0x0010
#       define NVREG_IRQ_TIMER        0x0020
#       define NVREG_IRQ_LINK         0x0040
#       define NVREG_IRQ_TX1          0x0100
#       define NVREG_IRQMASK_WANTED_1 0x005f
#       define NVREG_IRQMASK_WANTED_2 0x0147
#       define NVREG_IRQ_UNKNOWN      (~(NVREG_IRQ_RX|NVREG_IRQ_RX_NOBUF|\
    NVREG_IRQ_TX_ERR|NVREG_IRQ_TX2|NVREG_IRQ_TIMER|NVREG_IRQ_LINK|\
    NVREG_IRQ_TX1))
	NvRegUnknownSetupReg6 = 0x008,
#       define NVREG_UNKSETUP6_VAL 3
	/*
	* NVREG_POLL_DEFAULT is the interval length of the timer source on the nic
	* NVREG_POLL_DEFAULT=97 would result in an interval length of 1 ms
	*/
	NvRegPollingInterval = 0x00c,
#       define NVREG_POLL_DEFAULT 970
	NvRegMisc1 = 0x080,
#       define NVREG_MISC1_HD    0x02
#       define NVREG_MISC1_FORCE 0x3b0f3c
	NvRegTransmitterControl = 0x084,
#       define NVREG_XMITCTL_START 0x01
	NvRegTransmitterStatus = 0x088,
#       define NVREG_XMITSTAT_BUSY 0x01
	NvRegPacketFilterFlags = 0x8c,
#       define NVREG_PFF_ALWAYS  0x7F0008
#       define NVREG_PFF_PROMISC 0x80
#       define NVREG_PFF_MYADDR  0x20
	NvRegOffloadConfig = 0x90,
#       define NVREG_OFFLOAD_HOMEPHY 0x601
#       define NVREG_OFFLOAD_NORMAL  0x5ee
	NvRegReceiverControl = 0x094,
#       define NVREG_RCVCTL_START 0x01
	NvRegReceiverStatus = 0x98,
#       define NVREG_RCVSTAT_BUSY  0x01
	NvRegRandomSeed = 0x9c,
#       define NVREG_RNDSEED_MASK  0x00ff
#       define NVREG_RNDSEED_FORCE 0x7f00
	NvRegUnknownSetupReg1 = 0xA0,
#       define NVREG_UNKSETUP1_VAL 0x16070f
	NvRegUnknownSetupReg2 = 0xA4,
#       define NVREG_UNKSETUP2_VAL 0x16
	NvRegMacAddrA = 0xA8,
	NvRegMacAddrB = 0xAC,
	NvRegMulticastAddrA = 0xB0,
#       define NVREG_MCASTADDRA_FORCE  0x01
	NvRegMulticastAddrB = 0xB4,
	NvRegMulticastMaskA = 0xB8,
	NvRegMulticastMaskB = 0xBC,
	NvRegTxRingPhysAddr = 0x100,
	NvRegRxRingPhysAddr = 0x104,
	NvRegRingSizes = 0x108,
#       define NVREG_RINGSZ_TXSHIFT 0
#       define NVREG_RINGSZ_RXSHIFT 16
	NvRegUnknownTransmitterReg = 0x10c,
	NvRegLinkSpeed = 0x110,
#       define NVREG_LINKSPEED_FORCE 0x10000
#       define NVREG_LINKSPEED_10    10
#       define NVREG_LINKSPEED_100   100
#       define NVREG_LINKSPEED_1000  1000
	NvRegUnknownSetupReg5 = 0x130,
#       define NVREG_UNKSETUP5_BIT31 (1<<31)
	NvRegUnknownSetupReg3 = 0x134,
#       define NVREG_UNKSETUP3_VAL1 0x200010
	NvRegUnknownSetupReg8 = 0x13C,
#       define NVREG_UNKSETUP8_VAL1 0x300010
	NvRegUnknownSetupReg7 = 0x140,
#       define NVREG_UNKSETUP7_VAL 0x300010
	NvRegTxRxControl = 0x144,
#       define NVREG_TXRXCTL_KICK  0x0001
#       define NVREG_TXRXCTL_BIT1  0x0002
#       define NVREG_TXRXCTL_BIT2  0x0004
#       define NVREG_TXRXCTL_IDLE  0x0008
#       define NVREG_TXRXCTL_RESET 0x0010
	NvRegMIIStatus = 0x180,
#       define NVREG_MIISTAT_ERROR      0x0001
#       define NVREG_MIISTAT_LINKCHANGE 0x0008
#       define NVREG_MIISTAT_MASK       0x000f
#       define NVREG_MIISTAT_MASK2      0x000f
	NvRegUnknownSetupReg4 = 0x184,
#       define NVREG_UNKSETUP4_VAL 8
	NvRegAdapterControl = 0x188,
#       define NVREG_ADAPTCTL_START    0x02
#       define NVREG_ADAPTCTL_LINKUP   0x04
#       define NVREG_ADAPTCTL_PHYVALID 0x4000
#       define NVREG_ADAPTCTL_RUNNING  0x100000
#       define NVREG_ADAPTCTL_PHYSHIFT 24
	NvRegMIISpeed = 0x18c,
#       define NVREG_MIISPEED_BIT8 (1<<8)
#       define NVREG_MIIDELAY  5
	NvRegMIIControl = 0x190,
#       define NVREG_MIICTL_INUSE 0x10000
#       define NVREG_MIICTL_WRITE 0x08000
#       define NVREG_MIICTL_ADDRSHIFT  5
	NvRegMIIData = 0x194,
	NvRegWakeUpFlags = 0x200,
#       define NVREG_WAKEUPFLAGS_VAL               0x7770
#       define NVREG_WAKEUPFLAGS_BUSYSHIFT         24
#       define NVREG_WAKEUPFLAGS_ENABLESHIFT       16
#       define NVREG_WAKEUPFLAGS_D3SHIFT           12
#       define NVREG_WAKEUPFLAGS_D2SHIFT           8
#       define NVREG_WAKEUPFLAGS_D1SHIFT           4
#       define NVREG_WAKEUPFLAGS_D0SHIFT           0
#       define NVREG_WAKEUPFLAGS_ACCEPT_MAGPAT     0x01
#       define NVREG_WAKEUPFLAGS_ACCEPT_WAKEUPPAT  0x02
#       define NVREG_WAKEUPFLAGS_ACCEPT_LINKCHANGE 0x04
	NvRegPatternCRC = 0x204,
	NvRegPatternMask = 0x208,
	NvRegPowerCap = 0x268,
#       define NVREG_POWERCAP_D3SUPP (1<<30)
#       define NVREG_POWERCAP_D2SUPP (1<<26)
#       define NVREG_POWERCAP_D1SUPP (1<<25)
	NvRegPowerState = 0x26c,
#       define NVREG_POWERSTATE_POWEREDUP 0x8000
#       define NVREG_POWERSTATE_VALID     0x0100
#       define NVREG_POWERSTATE_MASK      0x0003
#       define NVREG_POWERSTATE_D0        0x0000
#       define NVREG_POWERSTATE_D1        0x0001
#       define NVREG_POWERSTATE_D2        0x0002
#       define NVREG_POWERSTATE_D3        0x0003
};

typedef struct _ip_address
{
	uint8_t bytes[4];
} ip_address;

typedef struct _mac_address
{
	uint8_t bytes[6];
} mac_address;

typedef struct _ethernet_header
{
	mac_address dst;
	mac_address src;
	uint16_t protocol;
} ethernet_header;

typedef struct _arp_packet
{
	uint16_t    hw_type;
	uint16_t    protocol;
	uint8_t     h_addr_len;
	uint8_t     p_addr_len;
	uint16_t    operation;
	mac_address h_src;
	ip_address  p_src;
	mac_address h_dst;
	ip_address  p_dst;
} arp_packet;

typedef struct _ip_header {
	uint8_t     ver_hlen;    /* version << 4 | header length >> 2 */
	uint8_t     type;        /* type of service */
	uint16_t    len;         /* total length */
	uint16_t    id;          /* identification */
	uint16_t    offset;      /* fragment offset field */
	uint8_t     ttl;         /* time to live */
	uint8_t     proto;       /* protocol */
	uint16_t    hdr_csum;    /* checksum */
	ip_address  src;         /* source and dest address */
	ip_address  dst;
} ip_header;

class NVNetDevice : public PCIDevice {
public:
	// PCI Device functions
	void Init();
	void Reset();
	uint32_t IORead(int barIndex, uint32_t port, unsigned size);
	void IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size);
	uint32_t MMIORead(int barIndex, uint32_t addr, unsigned size);
	void MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size);

	__declspec(noinline) bool PCAPSend(void* packet, size_t length);
	size_t PCAPReceive(void* packet, size_t max_length);
private:
	bool PCAPInit();
	bool GetMacAddress(std::string adapterName, void* pMAC);

	bool m_PCAPRunning = false;

	void* m_AdapterHandle = nullptr;
	std::string m_HostAdapterName;
	mac_address m_HostMacAddress;
	mac_address m_GuestMacAddress = { 0x00, 0x50, 0xF2, 0x00, 0x00, 0x34 };
	mac_address m_BroadcastMacAddress = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
};
