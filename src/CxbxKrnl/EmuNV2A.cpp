// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuNV2A.cpp
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
// *  (c) 2016 Luke Usher <luke.usher@outlook.com>
// * 
// *  EmuNV2A.cpp is heavily based on code from XQEMU
// *  (c) XQEMU Team
// *  https://github.com/espes/xqemu/blob/xbox/hw/xbox/nv2a.c
// * 
// *  All rights reserved
// *
// ******************************************************************
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

#include <Zydis.hpp>

#include "CxbxKrnl.h"
#include "Emu.h"
#include "EmuNV2A.h"
#include "nv2a_int.h" // from https://github.com/espes/xqemu/tree/xbox/hw/xbox

struct {
	uint32_t pending_interrupts;
	uint32_t enabled_interrupts;
} pmc;
struct {
	uint32_t pending_interrupts;
	uint32_t enabled_interrupts;
	//TODO:
	// QemuThread puller_thread;
	// Cache1State cache1;
	uint32_t regs[0x2000];
} pfifo;
struct {
	uint32_t regs[0x1000];
} pvideo;
struct {
	uint32_t pending_interrupts;
	uint32_t enabled_interrupts;
	uint32_t numerator;
	uint32_t denominator;
	uint32_t alarm_time;
} ptimer;

struct {
	uint32_t regs[0x1000];
} pfb;
struct {
	uint32_t pending_interrupts;
	uint32_t enabled_interrupts;
	uint32_t start;
} pcrtc;
struct {
	uint32_t core_clock_coeff;
	uint64_t core_clock_freq;
	uint32_t memory_clock_coeff;
	uint32_t video_clock_coeff;
} pramdac;

static void update_irq()
{
	/* PFIFO */
	if (pfifo.pending_interrupts & pfifo.enabled_interrupts) {
		pmc.pending_interrupts |= NV_PMC_INTR_0_PFIFO;
	}	else {
		pmc.pending_interrupts &= ~NV_PMC_INTR_0_PFIFO;
	}
	if (pcrtc.pending_interrupts & pcrtc.enabled_interrupts) {
		pmc.pending_interrupts |= NV_PMC_INTR_0_PCRTC;
	}	else {
		pmc.pending_interrupts &= ~NV_PMC_INTR_0_PCRTC;
	}
	/* TODO PGRAPH */
	/*
	if (pgraph.pending_interrupts & pgraph.enabled_interrupts) {
		pmc.pending_interrupts |= NV_PMC_INTR_0_PGRAPH;
	}	else {
		pmc.pending_interrupts &= ~NV_PMC_INTR_0_PGRAPH;
	} */
	if (pmc.pending_interrupts && pmc.enabled_interrupts) {
		// TODO Raise IRQ
		EmuWarning("EmuNV2A: Raise IRQ Not Implemented");
	}	else {
		// TODO: Cancel IRQ
		EmuWarning("EmuNV2A: Cancel IRQ Not Implemented");
	}
}

uint32_t EmuNV2A_PMC_Read32(uint32_t addr)
{
	switch (addr) {
	case NV_PMC_BOOT_0:	// chipset and stepping: NV2A, A02, Rev 0		return 0x02A000A2;
	case NV_PMC_INTR_0:		return pmc.pending_interrupts;
	case NV_PMC_INTR_EN_0:
		return pmc.enabled_interrupts;
	default:
		EmuWarning("EmuNV2A_PMC_Read32: Unknown Read Address %08X", addr);
	}
	
	return 0;
}

uint32_t EmuNV2A_PBUS_Read32(uint32_t addr)
{
	switch (addr) {
	case NV_PBUS_PCI_NV_0:
		return 0x10de;	// PCI_VENDOR_ID_NVIDIA	
	case NV_PBUS_PCI_NV_2:
		return (0x02 << 24) | 161; // PCI_CLASS_DISPLAY_3D (0x02) Rev 161 (0xA1) 
	default:
		EmuWarning("EmuNV2A_PBUS_Read32: Unknown Read Address %08X", addr);
	}

	return 0;
}

uint32_t EmuNV2A_PFIFO_Read32(uint32_t addr)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PFIFO_Read32: Unknown Read Address %08X", addr);
	}
	
	return 0;
}

uint32_t EmuNV2A_PFIFO_CACHE_Read32(uint32_t addr)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PFIFO_CACHE_Read32: Unknown Read Address %08X", addr);
	}
	
	return 0;
}

uint32_t EmuNV2A_PRMA_Read32(uint32_t addr)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PRMA_Read32: Unknown Read Address %08X", addr);
	}
	
	return 0;
}

uint32_t EmuNV2A_PVIDEO_Read32(uint32_t addr)
{
	switch (addr) {
	case NV_PVIDEO_STOP:
		return 0;
	default:
		return pvideo.regs[addr];
	}
	
	return 0;
}

uint32_t EmuNV2A_PCOUNTER_Read32(uint32_t addr)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PCOUNTER_Read32: Unknown Read Address %08X", addr);
	}
	
	return 0;
}

uint32_t EmuNV2A_PTIMER_Read32(uint32_t addr)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PTIMER_Read32: Unknown Read Address %08X", addr);
	}
	
	return 0;
}

uint32_t EmuNV2A_PVPE_Read32(uint32_t addr)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PVPE_Read32: Unknown Read Address %08X", addr);
	}
	
	return 0;
}

uint32_t EmuNV2A_PTV_Read32(uint32_t addr)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PTV_Read32: Unknown Read Address %08X", addr);
	}
	
	return 0;
}

uint32_t EmuNV2A_PRMFB_Read32(uint32_t addr)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PRMFB_Read32: Unknown Read Address %08X", addr);
	}
	
	return 0;
}

uint32_t EmuNV2A_PRMVIO_Read32(uint32_t addr)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PRMVIO_Read32: Unknown Read Address %08X", addr);
	}
	
	return 0;
}

uint32_t EmuNV2A_PFB_Read32(uint32_t addr)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PFB_Read32: Unknown Read Address %08X", addr);
	}
	
	return 0;
}

uint32_t EmuNV2A_PSTRAPS_Read32(uint32_t addr)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PSTRAPS_Read32: Unknown Read Address %08X", addr);
	}
	
	return 0;
}

uint32_t EmuNV2A_PGRAPH_Read32(uint32_t addr)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PGRAPH_Read32: Unknown Read Address %08X", addr);
	}
	
	return 0;
}

uint32_t EmuNV2A_PCRTC_Read32(uint32_t addr)
{
	switch (addr) {
	case NV_PCRTC_INTR_0:
		return pcrtc.pending_interrupts;
	case NV_PCRTC_INTR_EN_0:
		return pcrtc.enabled_interrupts;
	case NV_PCRTC_START:
		return pcrtc.start;
	default:
		EmuWarning("EmuNV2A_PCRTC_Read32: Unknown Read Address %08X", addr);
	}
	
	return 0;
}

uint32_t EmuNV2A_PRMCIO_Read32(uint32_t addr)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PRMCIO_Read32: Unknown Read Address %08X", addr);
	}
	
	return 0;
}

uint32_t EmuNV2A_PRAMDAC_Read32(uint32_t addr)
{
	switch (addr & 3) {
		case NV_PRAMDAC_NVPLL_COEFF:
			return pramdac.core_clock_coeff;
			break;
		case NV_PRAMDAC_MPLL_COEFF:
			return pramdac.memory_clock_coeff;
			break;
		case NV_PRAMDAC_VPLL_COEFF:
			return pramdac.video_clock_coeff;
			break;
		case NV_PRAMDAC_PLL_TEST_COUNTER:
			/* emulated PLLs locked instantly? */
			return NV_PRAMDAC_PLL_TEST_COUNTER_VPLL2_LOCK
				| NV_PRAMDAC_PLL_TEST_COUNTER_NVPLL_LOCK
				| NV_PRAMDAC_PLL_TEST_COUNTER_MPLL_LOCK
				| NV_PRAMDAC_PLL_TEST_COUNTER_VPLL_LOCK;
			break;
		default:
			EmuWarning("EmuNV2A_PRAMDAC_Read32: Unknown Read Address %08X", addr);
	}

	return 0;
}

uint32_t EmuNV2A_PRMDIO_Read32(uint32_t addr)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PRMDIO_Read32: Unknown Read Address %08X", addr);
	}
	
	return 0;
}

uint32_t EmuNV2A_PRAMIN_Read32(uint32_t addr)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PRAMIN_Read32: Unknown Read Address %08X", addr);
	}
	
	return 0;
}

uint32_t EmuNV2A_USER_Read32(uint32_t addr)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_USER_Read32: Unknown Read Address %08X", addr);
	}
	
	return 0;
}

uint32_t EmuNV2A_Read32(uint32_t addr)
{
	switch ((addr >> 12) & 31) {
	case NV_PMC          :  /* card master control */
		return EmuNV2A_PMC_Read32(addr & 0x0FFF);
	case NV_PBUS         :  /* bus control */
		return EmuNV2A_PBUS_Read32(addr & 0x0FFF);
	case NV_PFIFO        :  /* MMIO and DMA FIFO submission to PGRAPH and VPE */
		return EmuNV2A_PFIFO_Read32(addr & 0x0FFF);
	case NV_PFIFO_CACHE  :
		return EmuNV2A_PFIFO_CACHE_Read32(addr & 0x0FFF);
	case NV_PRMA         :  /* access to BAR0/BAR1 from real mode */
		return EmuNV2A_PRMA_Read32(addr & 0x0FFF);
	case NV_PVIDEO       :  /* video overlay */
		return EmuNV2A_PVIDEO_Read32(addr & 0x0FFF);
	case NV_PTIMER       :  /* time measurement and time-based alarms */
		return EmuNV2A_PTIMER_Read32(addr & 0x0FFF);
	case NV_PCOUNTER     :  /* performance monitoring counters */
		return EmuNV2A_PCOUNTER_Read32(addr & 0x0FFF);
	case NV_PVPE         :  /* MPEG2 decoding engine */
		return EmuNV2A_PVPE_Read32(addr & 0x0FFF);
	case NV_PTV          :  /* TV encoder */
		return EmuNV2A_PTV_Read32(addr & 0x0FFF);
	case NV_PRMFB        :  /* aliases VGA memory window */
		return EmuNV2A_PRMFB_Read32(addr & 0x0FFF);
	case NV_PRMVIO       :  /* aliases VGA sequencer and graphics controller registers */
		return EmuNV2A_PRMVIO_Read32(addr & 0x0FFF);
	case NV_PFB          :  /* memory interface */
		return EmuNV2A_PFB_Read32(addr & 0x0FFF);
	case NV_PSTRAPS      :  /* straps readout / override */
		return EmuNV2A_PSTRAPS_Read32(addr & 0x0FFF);
	case NV_PGRAPH       :  /* accelerated 2d/3d drawing engine */
		return EmuNV2A_PGRAPH_Read32(addr & 0x0FFF);
	case NV_PCRTC        :  /* more CRTC controls */
		return EmuNV2A_PCRTC_Read32(addr & 0x0FFF);
	case NV_PRMCIO       :  /* aliases VGA CRTC and attribute controller registers */
		return EmuNV2A_PRMCIO_Read32(addr & 0x0FFF);
    case NV_PRAMDAC      :  /* RAMDAC, cursor, and PLL control */
		return EmuNV2A_PRAMDAC_Read32(addr & 0x0FFF);
	case NV_PRMDIO       :  /* aliases VGA palette registers */
		return EmuNV2A_PRMDIO_Read32(addr & 0x0FFF);
	case NV_PRAMIN       :  /* RAMIN access */
		return EmuNV2A_PRAMIN_Read32(addr & 0x0FFF);
	case NV_USER         :  /* PFIFO MMIO and DMA submission area */
		return EmuNV2A_USER_Read32(addr & 0x0FFF);
	default:
		EmuWarning("EmuNV2A_Read32: Unknown Read Address %08X", addr);
	}
	return 0;
}

void EmuNV2A_PMC_Write32(uint32_t addr, uint32_t value)
{
	switch (addr) {
	case NV_PMC_INTR_0:		pmc.pending_interrupts &= ~value;
		update_irq();
		break;
	case NV_PMC_INTR_EN_0:
		pmc.enabled_interrupts = value;
		update_irq();
		break;
	default:
		EmuWarning("EmuNV2A_PMC_Write32: Unknown Write Address %08X (value %08X)", addr, value);
	}
}

void EmuNV2A_PBUS_Write32(uint32_t addr, uint32_t value)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PBUS_Write32: Unknown Write Address %08X (value %08X)", addr, value);
	}
}

void EmuNV2A_PFIFO_Write32(uint32_t addr, uint32_t value)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PFIFO_Write32: Unknown Write Address %08X (value %08X)", addr, value);
	}
}

void EmuNV2A_PFIFO_CACHE_Write32(uint32_t addr, uint32_t value)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PFIFO_CACHE_Write32: Unknown Write Address %08X (value %08X)", addr, value);
	}

}

void EmuNV2A_PRMA_Write32(uint32_t addr, uint32_t value)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PRMA_Write32: Unknown Write Address %08X (value %08X)", addr, value);
	}
}

void EmuNV2A_PVIDEO_Write32(uint32_t addr, uint32_t value)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PVIDEO_Write32: Unknown Write Address %08X (value %08X)", addr, value);
	}
}

void EmuNV2A_PCOUNTER_Write32(uint32_t addr, uint32_t value)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PCOUNTER_Write32: Unknown Write Address %08X (value %08X)", addr, value);
	}
}

void EmuNV2A_PTIMER_Write32(uint32_t addr, uint32_t value)
{
	switch (addr) {
	case NV_PTIMER_INTR_0:
		ptimer.pending_interrupts &= ~value;
		update_irq();
		break;
	case NV_PTIMER_INTR_EN_0:
		ptimer.enabled_interrupts = value;
		update_irq();
		break;
	case NV_PTIMER_DENOMINATOR:
		ptimer.denominator = value;
		break;
	case NV_PTIMER_NUMERATOR:
		ptimer.numerator = value;
		break;
	case NV_PTIMER_ALARM_0:
		ptimer.alarm_time = value;
		break;
	default:
		EmuWarning("EmuNV2A_PTIMER_Write32: Unknown Write Address %08X (value %08X)", addr, value);
	}
}

void EmuNV2A_PVPE_Write32(uint32_t addr, uint32_t value)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PVPE_Write32: Unknown Write Address %08X (value %08X)", addr, value);
	}
}

void EmuNV2A_PTV_Write32(uint32_t addr, uint32_t value)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PTV_Write32: Unknown Write Address %08X (value %08X)", addr, value);
	}
}

void EmuNV2A_PRMFB_Write32(uint32_t addr, uint32_t value)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PRMFB_Write32: Unknown Write Address %08X (value %08X)", addr, value);
	}
}

void EmuNV2A_PRMVIO_Write32(uint32_t addr, uint32_t value)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PRMVIO_Write32: Unknown Write Address %08X (value %08X)", addr, value);
	}
}

void EmuNV2A_PFB_Write32(uint32_t addr, uint32_t value)
{
	switch (addr) {
		default:
			pfb.regs[addr] = value;
	}
}

void EmuNV2A_PSTRAPS_Write32(uint32_t addr, uint32_t value)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PSTRAPS_Write32: Unknown Write Address %08X (value %08X)", addr, value);
	}
}

void EmuNV2A_PGRAPH_Write32(uint32_t addr, uint32_t value)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PGRAPH_Write32: Unknown Write Address %08X (value %08X)", addr, value);
	}
}

void EmuNV2A_PCRTC_Write32(uint32_t addr, uint32_t value)
{
	switch (addr) {
	case NV_PCRTC_INTR_0:
		pcrtc.pending_interrupts &= ~value;
		update_irq();
		break;
	case NV_PCRTC_INTR_EN_0:
		pcrtc.enabled_interrupts = value;
		update_irq();
		break;
	case NV_PCRTC_START:		pcrtc.start = value &= 0x07FFFFFF;
		break;
	default:
		EmuWarning("EmuNV2A_PCRTC_Write32: Unknown Write Address %08X (value %08X)", addr, value);
	}
}

void EmuNV2A_PRMCIO_Write32(uint32_t addr, uint32_t value)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PRMCIO_Write32: Unknown Write Address %08X (value %08X)", addr, value);
	}
}

void EmuNV2A_PRAMDAC_Write32(uint32_t addr, uint32_t value)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PRAMDAC_Write32: Unknown Write Address %08X (value %08X)", addr, value);
	}
}

void EmuNV2A_PRMDIO_Write32(uint32_t addr, uint32_t value)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PRMDIO_Write32: Unknown Write Address %08X (value %08X)", addr, value);
	}
}

void EmuNV2A_PRAMIN_Write32(uint32_t addr, uint32_t value)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PRAMIN_Write32: Unknown Write Address %08X (value %08X)", addr, value);
	}
}

void EmuNV2A_USER_Write32(uint32_t addr, uint32_t value)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_USER_Write32: Unknown Write Address %08X (value %08X)", addr, value);
	}
}

void EmuNV2A_Write32(uint32_t addr, uint32_t value)
{
	switch ((addr >> 12) & 31) {
	case NV_PMC:  /* card master control */
		EmuNV2A_PMC_Write32(addr & 0x0FFF, value);
		break;
	case NV_PBUS:  /* bus control */
		EmuNV2A_PBUS_Write32(addr & 0x0FFF, value);
		break;
	case NV_PFIFO:  /* MMIO and DMA FIFO submission to PGRAPH and VPE */
		EmuNV2A_PFIFO_Write32(addr & 0x0FFF, value);
		break;
	case NV_PFIFO_CACHE:
		EmuNV2A_PFIFO_CACHE_Write32(addr & 0x0FFF, value);
		break;
	case NV_PRMA:  /* access to BAR0/BAR1 from real mode */
		EmuNV2A_PRMA_Write32(addr & 0x0FFF, value);
		break;
	case NV_PVIDEO:  /* video overlay */
		EmuNV2A_PVIDEO_Write32(addr & 0x0FFF, value);
		break;
	case NV_PTIMER:  /* time measurement and time-based alarms */
		EmuNV2A_PTIMER_Write32(addr & 0x0FFF, value);
		break;
	case NV_PCOUNTER:  /* performance monitoring counters */
		EmuNV2A_PCOUNTER_Write32(addr & 0x0FFF, value);
		break;
	case NV_PVPE:  /* MPEG2 decoding engine */
		EmuNV2A_PVPE_Write32(addr & 0x0FFF, value);
		break;
	case NV_PTV:  /* TV encoder */
		EmuNV2A_PTV_Write32(addr & 0x0FFF, value);
		break;
	case NV_PRMFB:  /* aliases VGA memory window */
		EmuNV2A_PRMFB_Write32(addr & 0x0FFF, value);
		break;
	case NV_PRMVIO:  /* aliases VGA sequencer and graphics controller registers */
		EmuNV2A_PRMVIO_Write32(addr & 0x0FFF, value);
		break;
	case NV_PFB:  /* memory interface */
		EmuNV2A_PFB_Write32(addr & 0x0FFF, value);
		break;
	case NV_PSTRAPS:  /* straps readout / override */
		EmuNV2A_PSTRAPS_Write32(addr & 0x0FFF, value);
		break;
	case NV_PGRAPH:  /* accelerated 2d/3d drawing engine */
		EmuNV2A_PGRAPH_Write32(addr & 0x0FFF, value);
		break;
	case NV_PCRTC:  /* more CRTC controls */
		EmuNV2A_PCRTC_Write32(addr & 0x0FFF, value);
		break;
	case NV_PRMCIO:  /* aliases VGA CRTC and attribute controller registers */
		EmuNV2A_PRMCIO_Write32(addr & 0x0FFF, value);
		break;
	case NV_PRAMDAC:  /* RAMDAC, cursor, and PLL control */
		EmuNV2A_PRAMDAC_Write32(addr & 0x0FFF, value);
		break;
	case NV_PRMDIO:  /* aliases VGA palette registers */
		EmuNV2A_PRMDIO_Write32(addr & 0x0FFF, value);
		break;
	case NV_PRAMIN:  /* RAMIN access */
		EmuNV2A_PRAMIN_Write32(addr & 0x0FFF, value);
		break;
	case NV_USER:  /* PFIFO MMIO and DMA submission area */
		EmuNV2A_USER_Write32(addr & 0x0FFF, value);
		break;
	default:
		EmuWarning("EmuNV2A_Write32: Unknown Write Address %08X (value %08X)", addr, value);
	}
}
