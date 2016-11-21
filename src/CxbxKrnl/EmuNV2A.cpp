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
	}
	else {
		pmc.pending_interrupts &= ~NV_PMC_INTR_0_PFIFO;
	}
	if (pcrtc.pending_interrupts & pcrtc.enabled_interrupts) {
		pmc.pending_interrupts |= NV_PMC_INTR_0_PCRTC;
	}
	else {
		pmc.pending_interrupts &= ~NV_PMC_INTR_0_PCRTC;
	}
	/* TODO PGRAPH */
	/*
	if (pgraph.pending_interrupts & pgraph.enabled_interrupts) {
		pmc.pending_interrupts |= NV_PMC_INTR_0_PGRAPH;
	}
	else {
		pmc.pending_interrupts &= ~NV_PMC_INTR_0_PGRAPH;
	} */
	if (pmc.pending_interrupts && pmc.enabled_interrupts) {
		// TODO Raise IRQ
		EmuWarning("EmuNV2A: update_irq : Raise IRQ Not Implemented");
	}
	else {
		// TODO: Cancel IRQ
		EmuWarning("EmuNV2A: update_irq : Cancel IRQ Not Implemented");
	}
}

#define DEBUG_START(DEV) const char *DebugNV_##DEV##(uint32_t addr) { switch (addr) {
#define CASE(a) case a: return #a
#define DEBUG_END(DEV) default: return "Unknown " #DEV " Address"; } }

DEBUG_START(PMC)
	CASE(NV_PMC_BOOT_0);
	CASE(NV_PMC_INTR_0);
	CASE(NV_PMC_INTR_EN_0);
	CASE(NV_PMC_ENABLE);
DEBUG_END(PMC)

DEBUG_START(PBUS)
	CASE(NV_PBUS_PCI_NV_0);
	CASE(NV_PBUS_PCI_NV_1);
	CASE(NV_PBUS_PCI_NV_2);
DEBUG_END(PBUS)

DEBUG_START(PFIFO)
	CASE(NV_PFIFO_INTR_0);
	CASE(NV_PFIFO_INTR_EN_0);
	CASE(NV_PFIFO_RAMHT);
	CASE(NV_PFIFO_RAMFC);
	CASE(NV_PFIFO_RAMRO);
	CASE(NV_PFIFO_RUNOUT_STATUS);
	CASE(NV_PFIFO_MODE);
	CASE(NV_PFIFO_DMA);
DEBUG_END(PFIFO)

DEBUG_START(PFIFO_CACHE)
	CASE(NV_PFIFO_CACHE1_PUSH0);
	CASE(NV_PFIFO_CACHE1_PUSH1);
	CASE(NV_PFIFO_CACHE1_STATUS);
	CASE(NV_PFIFO_CACHE1_DMA_PUSH);
	CASE(NV_PFIFO_CACHE1_DMA_FETCH);
	CASE(NV_PFIFO_CACHE1_DMA_STATE);
	CASE(NV_PFIFO_CACHE1_DMA_INSTANCE);
	CASE(NV_PFIFO_CACHE1_DMA_PUT);
	CASE(NV_PFIFO_CACHE1_DMA_GET);
	CASE(NV_PFIFO_CACHE1_DMA_SUBROUTINE);
	CASE(NV_PFIFO_CACHE1_PULL0);
	CASE(NV_PFIFO_CACHE1_ENGINE);
	CASE(NV_PFIFO_CACHE1_DMA_DCOUNT);
	CASE(NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW);
	CASE(NV_PFIFO_CACHE1_DMA_RSVD_SHADOW);
	CASE(NV_PFIFO_CACHE1_DMA_DATA_SHADOW);
DEBUG_END(PFIFO_CACHE)

DEBUG_START(PRMA)
DEBUG_END(PRMA)

DEBUG_START(PVIDEO)
	CASE(NV_PVIDEO_INTR);
	CASE(NV_PVIDEO_INTR_EN);
	CASE(NV_PVIDEO_BUFFER);
	CASE(NV_PVIDEO_STOP);
	CASE(NV_PVIDEO_BASE);
	CASE(NV_PVIDEO_LIMIT);
	CASE(NV_PVIDEO_LUMINANCE);
	CASE(NV_PVIDEO_CHROMINANCE);
	CASE(NV_PVIDEO_OFFSET);
	CASE(NV_PVIDEO_SIZE_IN);
	CASE(NV_PVIDEO_POINT_IN);
	CASE(NV_PVIDEO_DS_DX);
	CASE(NV_PVIDEO_DT_DY);
	CASE(NV_PVIDEO_POINT_OUT);
	CASE(NV_PVIDEO_SIZE_OUT);
	CASE(NV_PVIDEO_FORMAT);
DEBUG_END(PVIDEO)

DEBUG_START(PTIMER)
	CASE(NV_PTIMER_INTR_0);
	CASE(NV_PTIMER_INTR_EN_0);
	CASE(NV_PTIMER_NUMERATOR);
	CASE(NV_PTIMER_DENOMINATOR);
	CASE(NV_PTIMER_TIME_0);
	CASE(NV_PTIMER_TIME_1);
	CASE(NV_PTIMER_ALARM_0);
DEBUG_END(PTIMER)

DEBUG_START(PCOUNTER)
DEBUG_END(PCOUNTER)

DEBUG_START(PVPE)
DEBUG_END(PVPE)

DEBUG_START(PTV)
DEBUG_END(PTV)

DEBUG_START(PRMFB)
DEBUG_END(PRMFB)

DEBUG_START(PRMVIO)
DEBUG_END(PRMVIO)

DEBUG_START(PFB)
	CASE(NV_PFB_CFG0);
	CASE(NV_PFB_CSTATUS);
	CASE(NV_PFB_WBC);
DEBUG_END(PFB)

DEBUG_START(PSTRAPS)
DEBUG_END(PSTRAPS)

DEBUG_START(PGRAPH)
	CASE(NV_PGRAPH_INTR);
	CASE(NV_PGRAPH_NSOURCE);
	CASE(NV_PGRAPH_INTR_EN);
	CASE(NV_PGRAPH_CTX_CONTROL);
	CASE(NV_PGRAPH_CTX_USER);
	CASE(NV_PGRAPH_CTX_SWITCH1);
	CASE(NV_PGRAPH_TRAPPED_ADDR);
	CASE(NV_PGRAPH_TRAPPED_DATA_LOW);
	CASE(NV_PGRAPH_SURFACE);
	CASE(NV_PGRAPH_INCREMENT);
	CASE(NV_PGRAPH_FIFO);
	CASE(NV_PGRAPH_CHANNEL_CTX_TABLE);
	CASE(NV_PGRAPH_CHANNEL_CTX_POINTER);
	CASE(NV_PGRAPH_CHANNEL_CTX_TRIGGER);
	CASE(NV_PGRAPH_CSV0_D);
	CASE(NV_PGRAPH_CSV0_C);
	CASE(NV_PGRAPH_CSV1_B);
	CASE(NV_PGRAPH_CSV1_A);
	CASE(NV_PGRAPH_CHEOPS_OFFSET);
	CASE(NV_PGRAPH_BLEND);
	CASE(NV_PGRAPH_BLENDCOLOR);
	CASE(NV_PGRAPH_BORDERCOLOR0);
	CASE(NV_PGRAPH_BORDERCOLOR1);
	CASE(NV_PGRAPH_BORDERCOLOR2);
	CASE(NV_PGRAPH_BORDERCOLOR3);
	CASE(NV_PGRAPH_BUMPOFFSET1);
	CASE(NV_PGRAPH_BUMPSCALE1);
	CASE(NV_PGRAPH_CLEARRECTX);
	CASE(NV_PGRAPH_CLEARRECTY);
	CASE(NV_PGRAPH_COLORCLEARVALUE);
	CASE(NV_PGRAPH_COMBINEFACTOR0);
	CASE(NV_PGRAPH_COMBINEFACTOR1);
	CASE(NV_PGRAPH_COMBINEALPHAI0);
	CASE(NV_PGRAPH_COMBINEALPHAO0);
	CASE(NV_PGRAPH_COMBINECOLORI0);
	CASE(NV_PGRAPH_COMBINECOLORO0);
	CASE(NV_PGRAPH_COMBINECTL);
	CASE(NV_PGRAPH_COMBINESPECFOG0);
	CASE(NV_PGRAPH_COMBINESPECFOG1);
	CASE(NV_PGRAPH_CONTROL_0);
	CASE(NV_PGRAPH_CONTROL_2);
	CASE(NV_PGRAPH_CONTROL_3);
	CASE(NV_PGRAPH_FOGCOLOR);
	CASE(NV_PGRAPH_FOGPARAM0);
	CASE(NV_PGRAPH_FOGPARAM1);
	CASE(NV_PGRAPH_SETUPRASTER);
	CASE(NV_PGRAPH_SHADERCLIPMODE);
	CASE(NV_PGRAPH_SHADERCTL);
	CASE(NV_PGRAPH_SHADERPROG);
	CASE(NV_PGRAPH_SHADOWZSLOPETHRESHOLD);
	CASE(NV_PGRAPH_SPECFOGFACTOR0);
	CASE(NV_PGRAPH_SPECFOGFACTOR1);
	CASE(NV_PGRAPH_TEXADDRESS0);
	CASE(NV_PGRAPH_TEXADDRESS1);
	CASE(NV_PGRAPH_TEXADDRESS2);
	CASE(NV_PGRAPH_TEXADDRESS3);
	CASE(NV_PGRAPH_TEXCTL0_0);
	CASE(NV_PGRAPH_TEXCTL0_1);
	CASE(NV_PGRAPH_TEXCTL0_2);
	CASE(NV_PGRAPH_TEXCTL0_3);
	CASE(NV_PGRAPH_TEXCTL1_0);
	CASE(NV_PGRAPH_TEXCTL1_1);
	CASE(NV_PGRAPH_TEXCTL1_2);
	CASE(NV_PGRAPH_TEXCTL1_3);
	CASE(NV_PGRAPH_TEXCTL2_0);
	CASE(NV_PGRAPH_TEXCTL2_1);
	CASE(NV_PGRAPH_TEXFILTER0);
	CASE(NV_PGRAPH_TEXFILTER1);
	CASE(NV_PGRAPH_TEXFILTER2);
	CASE(NV_PGRAPH_TEXFILTER3);
	CASE(NV_PGRAPH_TEXFMT0);
	CASE(NV_PGRAPH_TEXFMT1);
	CASE(NV_PGRAPH_TEXFMT2);
	CASE(NV_PGRAPH_TEXFMT3);
	CASE(NV_PGRAPH_TEXIMAGERECT0);
	CASE(NV_PGRAPH_TEXIMAGERECT1);
	CASE(NV_PGRAPH_TEXIMAGERECT2);
	CASE(NV_PGRAPH_TEXIMAGERECT3);
	CASE(NV_PGRAPH_TEXOFFSET0);
	CASE(NV_PGRAPH_TEXOFFSET1);
	CASE(NV_PGRAPH_TEXOFFSET2);
	CASE(NV_PGRAPH_TEXOFFSET3);
	CASE(NV_PGRAPH_TEXPALETTE0);
	CASE(NV_PGRAPH_TEXPALETTE1);
	CASE(NV_PGRAPH_TEXPALETTE2);
	CASE(NV_PGRAPH_TEXPALETTE3);
	CASE(NV_PGRAPH_ZSTENCILCLEARVALUE);
	CASE(NV_PGRAPH_ZCLIPMIN);
	CASE(NV_PGRAPH_ZOFFSETBIAS);
	CASE(NV_PGRAPH_ZOFFSETFACTOR);
	CASE(NV_PGRAPH_EYEVEC0);
	CASE(NV_PGRAPH_EYEVEC1);
	CASE(NV_PGRAPH_EYEVEC2);
	CASE(NV_PGRAPH_ZCLIPMAX);
DEBUG_END(PGRAPH)

DEBUG_START(PCRTC)
	CASE(NV_PCRTC_INTR_0);
	CASE(NV_PCRTC_INTR_EN_0);
	CASE(NV_PCRTC_START);
	CASE(NV_PCRTC_CONFIG);
DEBUG_END(PCRTC)

DEBUG_START(PRMCIO)
DEBUG_END(PRMCIO)

DEBUG_START(PRAMDAC)
	CASE(NV_PRAMDAC_NVPLL_COEFF);
	CASE(NV_PRAMDAC_MPLL_COEFF);
	CASE(NV_PRAMDAC_VPLL_COEFF);
	CASE(NV_PRAMDAC_PLL_TEST_COUNTER);
DEBUG_END(PRAMDAC)

DEBUG_START(PRMDIO)
DEBUG_END(PRMDIO)

DEBUG_START(PRAMIN)
DEBUG_END(PRAMIN)

DEBUG_START(USER)
	CASE(NV_USER_DMA_PUT);
	CASE(NV_USER_DMA_GET);
	CASE(NV_USER_REF);
DEBUG_END(USER)


#define DEBUG_READ32(DEV) EmuWarning("EmuNV2A_" #DEV "_Read32($%08X) // %s = $%08X", addr, DebugNV_##DEV##(addr), result)
#define DEBUG_WRITE32(DEV) EmuWarning("EmuNV2A_" #DEV "_Write32($%08X, $%08X) // %s ", addr, value, DebugNV_##DEV##(addr))

#define READ32_START(DEV) uint32_t EmuNV2A_##DEV##_Read32(uint32_t addr) { uint32_t result = 0; switch (addr) {
#define READ32_UNHANDLED(DEV) default: EmuWarning("EmuNV2A_" #DEV "_Read32 Unhandled");
#define READ32_END(DEV) DEBUG_READ32(DEV); } return result; }

#define WRITE32_START(DEV) void EmuNV2A_##DEV##_Write32(uint32_t addr, uint32_t value) { DEBUG_WRITE32(DEV); switch (addr) {
#define WRITE32_UNHANDLED(DEV) default: EmuWarning("EmuNV2A_" #DEV "_Write32 Unhandled");
#define WRITE32_END(DEV) } }  


READ32_START(PMC)
	case NV_PMC_BOOT_0:	// chipset and stepping: NV2A, A02, Rev 0
		result = 0x02A000A2;
		break;
	case NV_PMC_INTR_0:
		result = pmc.pending_interrupts;
		break;
	case NV_PMC_INTR_EN_0:
		result = pmc.enabled_interrupts;
		break;
	READ32_UNHANDLED(PMC)
READ32_END(PMC)

WRITE32_START(PMC)
	case NV_PMC_INTR_0:
		pmc.pending_interrupts &= ~value;
		update_irq();
		break;
	case NV_PMC_INTR_EN_0:
		pmc.enabled_interrupts = value;
		update_irq();
		break;
	WRITE32_UNHANDLED(PMC)
WRITE32_END(PMC)


READ32_START(PBUS)
	case NV_PBUS_PCI_NV_0:
		result = 0x10de;	// PCI_VENDOR_ID_NVIDIA	
		break;
	case NV_PBUS_PCI_NV_1:
		result = 1; // NV_PBUS_PCI_NV_1_IO_SPACE_ENABLED
		break;
	case NV_PBUS_PCI_NV_2:
		result = (0x02 << 24) | 161; // PCI_CLASS_DISPLAY_3D (0x02) Rev 161 (0xA1) 
		break;
	READ32_UNHANDLED(PBUS)
READ32_END(PBUS)

WRITE32_START(PBUS)
	WRITE32_UNHANDLED(PBUS)
WRITE32_END(PBUS)


READ32_START(PFIFO)
	READ32_UNHANDLED(PFIFO)
READ32_END(PFIFO)

WRITE32_START(PFIFO)
	WRITE32_UNHANDLED(PFIFO)
WRITE32_END(PFIFO)


READ32_START(PFIFO_CACHE)
	READ32_UNHANDLED(PFIFO_CACHE)
READ32_END(PFIFO_CACHE)

WRITE32_START(PFIFO_CACHE)
	WRITE32_UNHANDLED(PFIFO_CACHE)
WRITE32_END(PFIFO_CACHE)


READ32_START(PRMA)
	READ32_UNHANDLED(PRMA)
READ32_END(PRMA)

WRITE32_START(PRMA)
	WRITE32_UNHANDLED(PRMA)
WRITE32_END(PRMA)


READ32_START(PVIDEO)
	case NV_PVIDEO_STOP:
		result = 0;
		break;
	default:
		result = pvideo.regs[addr];
READ32_END(PVIDEO)

WRITE32_START(PVIDEO)
	default:
		pvideo.regs[addr] = value;
WRITE32_END(PVIDEO)

READ32_START(PCOUNTER)
	READ32_UNHANDLED(PCOUNTER)
READ32_END(PCOUNTER)

WRITE32_START(PCOUNTER)
	WRITE32_UNHANDLED(PCOUNTER)
WRITE32_END(PCOUNTER)


READ32_START(PTIMER)
	case NV_PTIMER_DENOMINATOR:
		result = ptimer.denominator;
		break;
	case NV_PTIMER_NUMERATOR:
		result = ptimer.numerator;
		break;
	case NV_PTIMER_ALARM_0:
		result = ptimer.alarm_time;
		break;
	READ32_UNHANDLED(PTIMER)
READ32_END(PTIMER)

WRITE32_START(PTIMER)
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
WRITE32_END(PTIMER)


READ32_START(PVPE)
	READ32_UNHANDLED(PVPE)
READ32_END(PVPE)

WRITE32_START(PVPE)
	WRITE32_UNHANDLED(PVPE)
WRITE32_END(PVPE)


READ32_START(PTV)
	READ32_UNHANDLED(PTV)
READ32_END(PTV)

WRITE32_START(PTV)
	WRITE32_UNHANDLED(PTV)
WRITE32_END(PTV)


READ32_START(PRMFB)
	READ32_UNHANDLED(PRMFB)
READ32_END(PRMFB)

WRITE32_START(PRMFB)
	WRITE32_UNHANDLED(PRMFB)
WRITE32_END(PRMFB)


READ32_START(PRMVIO)
	READ32_UNHANDLED(PRMVIO)
READ32_END(PRMVIO)

WRITE32_START(PRMVIO)
	WRITE32_UNHANDLED(PRMVIO)
WRITE32_END(PRMVIO)


READ32_START(PFB)
	default:
		result = pfb.regs[addr];
READ32_END(PFB)

WRITE32_START(PFB)
	default:
		pfb.regs[addr] = value;
WRITE32_END(PFB)


READ32_START(PSTRAPS)
	READ32_UNHANDLED(PSTRAPS)
READ32_END(PSTRAPS)

WRITE32_START(PSTRAPS)
	WRITE32_UNHANDLED(PSTRAPS)
WRITE32_END(PSTRAPS)


READ32_START(PGRAPH)
	READ32_UNHANDLED(PGRAPH)
READ32_END(PGRAPH)

WRITE32_START(PGRAPH)
	WRITE32_UNHANDLED(PGRAPH)
WRITE32_END(PGRAPH)


READ32_START(PCRTC)
	case NV_PCRTC_INTR_0:
		result = pcrtc.pending_interrupts;
		break;
	case NV_PCRTC_INTR_EN_0:
		result = pcrtc.enabled_interrupts;
		break;
	case NV_PCRTC_START:
		result = pcrtc.start;
		break;
	READ32_UNHANDLED(PCRTC)
READ32_END(PCRTC)

WRITE32_START(PCRTC)
	case NV_PCRTC_INTR_0:
		pcrtc.pending_interrupts &= ~value;
		update_irq();
		break;
	case NV_PCRTC_INTR_EN_0:
		pcrtc.enabled_interrupts = value;
		update_irq();
		break;
	case NV_PCRTC_START:
		pcrtc.start = value &= 0x07FFFFFF;
		break;
	WRITE32_UNHANDLED(PCRTC)
WRITE32_END(PCRTC)


READ32_START(PRMCIO)
	READ32_UNHANDLED(PRMCIO)
READ32_END(PRMCIO)

WRITE32_START(PRMCIO)
	WRITE32_UNHANDLED(PRMCIO)
WRITE32_END(PRMCIO)


READ32_START(PRAMDAC)
	case NV_PRAMDAC_NVPLL_COEFF:
		result = pramdac.core_clock_coeff;
		break;
	case NV_PRAMDAC_MPLL_COEFF:
		result = pramdac.memory_clock_coeff;
		break;
	case NV_PRAMDAC_VPLL_COEFF:
		result = pramdac.video_clock_coeff;
		break;
	case NV_PRAMDAC_PLL_TEST_COUNTER:
		/* emulated PLLs locked instantly? */
		result = NV_PRAMDAC_PLL_TEST_COUNTER_VPLL2_LOCK
			| NV_PRAMDAC_PLL_TEST_COUNTER_NVPLL_LOCK
			| NV_PRAMDAC_PLL_TEST_COUNTER_MPLL_LOCK
			| NV_PRAMDAC_PLL_TEST_COUNTER_VPLL_LOCK;
		break;
	READ32_UNHANDLED(PRAMDAC)
READ32_END(PRAMDAC)

WRITE32_START(PRAMDAC)
	case NV_PRAMDAC_NVPLL_COEFF:
		pramdac.core_clock_coeff = value;
		break;
	case NV_PRAMDAC_MPLL_COEFF:
		pramdac.memory_clock_coeff = value;
		break;
	case NV_PRAMDAC_VPLL_COEFF:
		pramdac.video_clock_coeff = value;
		break;
	WRITE32_UNHANDLED(PRAMDAC)
WRITE32_END(PRAMDAC)


READ32_START(PRMDIO)
	READ32_UNHANDLED(PRMDIO)
READ32_END(PRMDIO)

WRITE32_START(PRMDIO)
	WRITE32_UNHANDLED(PRMDIO)
WRITE32_END(PRMDIO)


READ32_START(PRAMIN)
	READ32_UNHANDLED(PRAMIN)
READ32_END(PRAMIN)

WRITE32_START(PRAMIN)
	WRITE32_UNHANDLED(PRAMIN)
WRITE32_END(PRAMIN)


READ32_START(USER)
	READ32_UNHANDLED(USER)
READ32_END(USER)

WRITE32_START(USER)
	WRITE32_UNHANDLED(USER)
WRITE32_END(USER)


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
		EmuWarning("EmuNV2A_Read32: Unhandled Read Address %08X", addr);
	}
	return 0;
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
		EmuWarning("EmuNV2A_Write32: Unhandled Write Address %08X (value %08X)", addr, value);
	}
}
