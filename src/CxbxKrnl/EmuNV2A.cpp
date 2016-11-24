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
#include "device.h"
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

struct {
	uint32_t pending_interrupts;
	uint32_t enabled_interrupts;
} pgraph;


static void update_irq()
{
	/* PFIFO */
	if (pfifo.pending_interrupts & pfifo.enabled_interrupts) {
		pmc.pending_interrupts |= NV_PMC_INTR_0_PFIFO;
	}
	else {
		pmc.pending_interrupts &= ~NV_PMC_INTR_0_PFIFO;
	}
	/* PCRTC */
	if (pcrtc.pending_interrupts & pcrtc.enabled_interrupts) {
		pmc.pending_interrupts |= NV_PMC_INTR_0_PCRTC;
	}
	else {
		pmc.pending_interrupts &= ~NV_PMC_INTR_0_PCRTC;
	}
	/* PGRAPH */
	if (pgraph.pending_interrupts & pgraph.enabled_interrupts) {
		pmc.pending_interrupts |= NV_PMC_INTR_0_PGRAPH;
	}
	else {
		pmc.pending_interrupts &= ~NV_PMC_INTR_0_PGRAPH;

	}

	/* TODO : PBUS * /
	if (pbus.pending_interrupts & pbus.enabled_interrupts) {
		pmc.pending_interrupts |= NV_PMC_INTR_0_PBUS;
	}
	else {
		pmc.pending_interrupts &= ~NV_PMC_INTR_0_PBUS;
	} */

	/* TODO : SOFTWARE * /
	if (user.pending_interrupts & user.enabled_interrupts) {
		pmc.pending_interrupts |= NV_PMC_INTR_0_SOFTWARE;
	}
	else {
		pmc.pending_interrupts &= ~NV_PMC_INTR_0_SOFTWARE;
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


#define DEBUG_START(DEV) \
const char *DebugNV_##DEV##(uint32_t addr) \
{ \
	switch (addr) {
#define DEBUG_CASE(a) \
		case a: return #a
#define DEBUG_END(DEV) \
		default: \
			return "Unknown " #DEV " Address"; \
	} \
}

DEBUG_START(PMC)
	DEBUG_CASE(NV_PMC_BOOT_0);
	DEBUG_CASE(NV_PMC_INTR_0);
	DEBUG_CASE(NV_PMC_INTR_EN_0);
	DEBUG_CASE(NV_PMC_ENABLE);
DEBUG_END(PMC)

DEBUG_START(PBUS)
	DEBUG_CASE(NV_PBUS_PCI_NV_0);
	DEBUG_CASE(NV_PBUS_PCI_NV_1);
	DEBUG_CASE(NV_PBUS_PCI_NV_2);
DEBUG_END(PBUS)

DEBUG_START(PFIFO)
	DEBUG_CASE(NV_PFIFO_INTR_0);
	DEBUG_CASE(NV_PFIFO_INTR_EN_0);
	DEBUG_CASE(NV_PFIFO_RAMHT);
	DEBUG_CASE(NV_PFIFO_RAMFC);
	DEBUG_CASE(NV_PFIFO_RAMRO);
	DEBUG_CASE(NV_PFIFO_RUNOUT_STATUS);
	DEBUG_CASE(NV_PFIFO_MODE);
	DEBUG_CASE(NV_PFIFO_DMA);
DEBUG_END(PFIFO)

DEBUG_START(PFIFO_CACHE)
	DEBUG_CASE(NV_PFIFO_CACHE1_PUSH0);
	DEBUG_CASE(NV_PFIFO_CACHE1_PUSH1);
	DEBUG_CASE(NV_PFIFO_CACHE1_STATUS);
	DEBUG_CASE(NV_PFIFO_CACHE1_DMA_PUSH);
	DEBUG_CASE(NV_PFIFO_CACHE1_DMA_FETCH);
	DEBUG_CASE(NV_PFIFO_CACHE1_DMA_STATE);
	DEBUG_CASE(NV_PFIFO_CACHE1_DMA_INSTANCE);
	DEBUG_CASE(NV_PFIFO_CACHE1_DMA_PUT);
	DEBUG_CASE(NV_PFIFO_CACHE1_DMA_GET);
	DEBUG_CASE(NV_PFIFO_CACHE1_DMA_SUBROUTINE);
	DEBUG_CASE(NV_PFIFO_CACHE1_PULL0);
	DEBUG_CASE(NV_PFIFO_CACHE1_ENGINE);
	DEBUG_CASE(NV_PFIFO_CACHE1_DMA_DCOUNT);
	DEBUG_CASE(NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW);
	DEBUG_CASE(NV_PFIFO_CACHE1_DMA_RSVD_SHADOW);
	DEBUG_CASE(NV_PFIFO_CACHE1_DMA_DATA_SHADOW);
DEBUG_END(PFIFO_CACHE)

DEBUG_START(PRMA)
DEBUG_END(PRMA)

DEBUG_START(PVIDEO)
	DEBUG_CASE(NV_PVIDEO_INTR);
	DEBUG_CASE(NV_PVIDEO_INTR_EN);
	DEBUG_CASE(NV_PVIDEO_BUFFER);
	DEBUG_CASE(NV_PVIDEO_STOP);
	DEBUG_CASE(NV_PVIDEO_BASE);
	DEBUG_CASE(NV_PVIDEO_LIMIT);
	DEBUG_CASE(NV_PVIDEO_LUMINANCE);
	DEBUG_CASE(NV_PVIDEO_CHROMINANCE);
	DEBUG_CASE(NV_PVIDEO_OFFSET);
	DEBUG_CASE(NV_PVIDEO_SIZE_IN);
	DEBUG_CASE(NV_PVIDEO_POINT_IN);
	DEBUG_CASE(NV_PVIDEO_DS_DX);
	DEBUG_CASE(NV_PVIDEO_DT_DY);
	DEBUG_CASE(NV_PVIDEO_POINT_OUT);
	DEBUG_CASE(NV_PVIDEO_SIZE_OUT);
	DEBUG_CASE(NV_PVIDEO_FORMAT);
DEBUG_END(PVIDEO)

DEBUG_START(PTIMER)
	DEBUG_CASE(NV_PTIMER_INTR_0);
	DEBUG_CASE(NV_PTIMER_INTR_EN_0);
	DEBUG_CASE(NV_PTIMER_NUMERATOR);
	DEBUG_CASE(NV_PTIMER_DENOMINATOR);
	DEBUG_CASE(NV_PTIMER_TIME_0);
	DEBUG_CASE(NV_PTIMER_TIME_1);
	DEBUG_CASE(NV_PTIMER_ALARM_0);

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

	DEBUG_CASE(NV_PFB_CFG0);
	DEBUG_CASE(NV_PFB_CSTATUS);
	DEBUG_CASE(NV_PFB_WBC);
DEBUG_END(PFB)

DEBUG_START(PSTRAPS)
DEBUG_END(PSTRAPS)

DEBUG_START(PGRAPH)
	DEBUG_CASE(NV_PGRAPH_INTR);
	DEBUG_CASE(NV_PGRAPH_NSOURCE);
	DEBUG_CASE(NV_PGRAPH_INTR_EN);
	DEBUG_CASE(NV_PGRAPH_CTX_CONTROL);
	DEBUG_CASE(NV_PGRAPH_CTX_USER);
	DEBUG_CASE(NV_PGRAPH_CTX_SWITCH1);
	DEBUG_CASE(NV_PGRAPH_TRAPPED_ADDR);
	DEBUG_CASE(NV_PGRAPH_TRAPPED_DATA_LOW);
	DEBUG_CASE(NV_PGRAPH_SURFACE);
	DEBUG_CASE(NV_PGRAPH_INCREMENT);
	DEBUG_CASE(NV_PGRAPH_FIFO);
	DEBUG_CASE(NV_PGRAPH_CHANNEL_CTX_TABLE);
	DEBUG_CASE(NV_PGRAPH_CHANNEL_CTX_POINTER);
	DEBUG_CASE(NV_PGRAPH_CHANNEL_CTX_TRIGGER);
	DEBUG_CASE(NV_PGRAPH_CSV0_D);
	DEBUG_CASE(NV_PGRAPH_CSV0_C);
	DEBUG_CASE(NV_PGRAPH_CSV1_B);
	DEBUG_CASE(NV_PGRAPH_CSV1_A);
	DEBUG_CASE(NV_PGRAPH_CHEOPS_OFFSET);
	DEBUG_CASE(NV_PGRAPH_BLEND);
	DEBUG_CASE(NV_PGRAPH_BLENDCOLOR);
	DEBUG_CASE(NV_PGRAPH_BORDERCOLOR0);
	DEBUG_CASE(NV_PGRAPH_BORDERCOLOR1);
	DEBUG_CASE(NV_PGRAPH_BORDERCOLOR2);
	DEBUG_CASE(NV_PGRAPH_BORDERCOLOR3);
	DEBUG_CASE(NV_PGRAPH_BUMPOFFSET1);
	DEBUG_CASE(NV_PGRAPH_BUMPSCALE1);
	DEBUG_CASE(NV_PGRAPH_CLEARRECTX);
	DEBUG_CASE(NV_PGRAPH_CLEARRECTY);
	DEBUG_CASE(NV_PGRAPH_COLORCLEARVALUE);
	DEBUG_CASE(NV_PGRAPH_COMBINEFACTOR0);
	DEBUG_CASE(NV_PGRAPH_COMBINEFACTOR1);
	DEBUG_CASE(NV_PGRAPH_COMBINEALPHAI0);
	DEBUG_CASE(NV_PGRAPH_COMBINEALPHAO0);
	DEBUG_CASE(NV_PGRAPH_COMBINECOLORI0);
	DEBUG_CASE(NV_PGRAPH_COMBINECOLORO0);
	DEBUG_CASE(NV_PGRAPH_COMBINECTL);
	DEBUG_CASE(NV_PGRAPH_COMBINESPECFOG0);
	DEBUG_CASE(NV_PGRAPH_COMBINESPECFOG1);
	DEBUG_CASE(NV_PGRAPH_CONTROL_0);
	DEBUG_CASE(NV_PGRAPH_CONTROL_2);
	DEBUG_CASE(NV_PGRAPH_CONTROL_3);
	DEBUG_CASE(NV_PGRAPH_FOGCOLOR);
	DEBUG_CASE(NV_PGRAPH_FOGPARAM0);
	DEBUG_CASE(NV_PGRAPH_FOGPARAM1);
	DEBUG_CASE(NV_PGRAPH_SETUPRASTER);
	DEBUG_CASE(NV_PGRAPH_SHADERCLIPMODE);
	DEBUG_CASE(NV_PGRAPH_SHADERCTL);
	DEBUG_CASE(NV_PGRAPH_SHADERPROG);
	DEBUG_CASE(NV_PGRAPH_SHADOWZSLOPETHRESHOLD);
	DEBUG_CASE(NV_PGRAPH_SPECFOGFACTOR0);
	DEBUG_CASE(NV_PGRAPH_SPECFOGFACTOR1);
	DEBUG_CASE(NV_PGRAPH_TEXADDRESS0);
	DEBUG_CASE(NV_PGRAPH_TEXADDRESS1);
	DEBUG_CASE(NV_PGRAPH_TEXADDRESS2);
	DEBUG_CASE(NV_PGRAPH_TEXADDRESS3);
	DEBUG_CASE(NV_PGRAPH_TEXCTL0_0);
	DEBUG_CASE(NV_PGRAPH_TEXCTL0_1);
	DEBUG_CASE(NV_PGRAPH_TEXCTL0_2);
	DEBUG_CASE(NV_PGRAPH_TEXCTL0_3);
	DEBUG_CASE(NV_PGRAPH_TEXCTL1_0);
	DEBUG_CASE(NV_PGRAPH_TEXCTL1_1);
	DEBUG_CASE(NV_PGRAPH_TEXCTL1_2);
	DEBUG_CASE(NV_PGRAPH_TEXCTL1_3);
	DEBUG_CASE(NV_PGRAPH_TEXCTL2_0);
	DEBUG_CASE(NV_PGRAPH_TEXCTL2_1);
	DEBUG_CASE(NV_PGRAPH_TEXFILTER0);
	DEBUG_CASE(NV_PGRAPH_TEXFILTER1);
	DEBUG_CASE(NV_PGRAPH_TEXFILTER2);
	DEBUG_CASE(NV_PGRAPH_TEXFILTER3);
	DEBUG_CASE(NV_PGRAPH_TEXFMT0);
	DEBUG_CASE(NV_PGRAPH_TEXFMT1);
	DEBUG_CASE(NV_PGRAPH_TEXFMT2);
	DEBUG_CASE(NV_PGRAPH_TEXFMT3);
	DEBUG_CASE(NV_PGRAPH_TEXIMAGERECT0);
	DEBUG_CASE(NV_PGRAPH_TEXIMAGERECT1);
	DEBUG_CASE(NV_PGRAPH_TEXIMAGERECT2);
	DEBUG_CASE(NV_PGRAPH_TEXIMAGERECT3);
	DEBUG_CASE(NV_PGRAPH_TEXOFFSET0);
	DEBUG_CASE(NV_PGRAPH_TEXOFFSET1);
	DEBUG_CASE(NV_PGRAPH_TEXOFFSET2);
	DEBUG_CASE(NV_PGRAPH_TEXOFFSET3);
	DEBUG_CASE(NV_PGRAPH_TEXPALETTE0);
	DEBUG_CASE(NV_PGRAPH_TEXPALETTE1);
	DEBUG_CASE(NV_PGRAPH_TEXPALETTE2);
	DEBUG_CASE(NV_PGRAPH_TEXPALETTE3);
	DEBUG_CASE(NV_PGRAPH_ZSTENCILCLEARVALUE);
	DEBUG_CASE(NV_PGRAPH_ZCLIPMIN);
	DEBUG_CASE(NV_PGRAPH_ZOFFSETBIAS);
	DEBUG_CASE(NV_PGRAPH_ZOFFSETFACTOR);
	DEBUG_CASE(NV_PGRAPH_EYEVEC0);
	DEBUG_CASE(NV_PGRAPH_EYEVEC1);
	DEBUG_CASE(NV_PGRAPH_EYEVEC2);
	DEBUG_CASE(NV_PGRAPH_ZCLIPMAX);
DEBUG_END(PGRAPH)

DEBUG_START(PCRTC)
	DEBUG_CASE(NV_PCRTC_INTR_0);
	DEBUG_CASE(NV_PCRTC_INTR_EN_0);
	DEBUG_CASE(NV_PCRTC_START);
	DEBUG_CASE(NV_PCRTC_CONFIG);

DEBUG_END(PCRTC)

DEBUG_START(PRMCIO)
DEBUG_END(PRMCIO)

DEBUG_START(PRAMDAC)
	DEBUG_CASE(NV_PRAMDAC_NVPLL_COEFF);
	DEBUG_CASE(NV_PRAMDAC_MPLL_COEFF);
	DEBUG_CASE(NV_PRAMDAC_VPLL_COEFF);
	DEBUG_CASE(NV_PRAMDAC_PLL_TEST_COUNTER);

DEBUG_END(PRAMDAC)

DEBUG_START(PRMDIO)
DEBUG_END(PRMDIO)

DEBUG_START(PRAMIN)
DEBUG_END(PRAMIN)

DEBUG_START(USER)

	DEBUG_CASE(NV_USER_DMA_PUT);
	DEBUG_CASE(NV_USER_DMA_GET);
	DEBUG_CASE(NV_USER_REF);

DEBUG_END(USER)


#define DEBUG_READ32(DEV) EmuWarning("EmuNV2A_" #DEV "_Read32($%08X) // %s = $%08X", addr, DebugNV_##DEV##(addr), result)
#define DEBUG_WRITE32(DEV) EmuWarning("EmuNV2A_" #DEV "_Write32($%08X, $%08X) // %s ", addr, value, DebugNV_##DEV##(addr))
#define DEBUG_READ32_UNHANDLED(DEV) EmuWarning("EmuNV2A_" #DEV "_Read32 Unhandled")
#define DEBUG_WRITE32_UNHANDLED(DEV) EmuWarning("EmuNV2A_" #DEV "_Write32 Unhandled")

#define DEVICE_READ32(DEV) uint32_t EmuNV2A_##DEV##_Read32(uint32_t addr)
#define DEVICE_READ32_SWITCH(addr) uint32_t result = 0; switch (addr) 
#define DEVICE_READ32_END(DEV) DEBUG_READ32(DEV); return result

#define DEVICE_WRITE32(DEV) void EmuNV2A_##DEV##_Write32(uint32_t addr, uint32_t value)
#define DEVICE_WRITE32_SWITCH(DEV, addr) DEBUG_WRITE32(DEV); switch (addr)


DEVICE_READ32(PMC)
{
	DEVICE_READ32_SWITCH(addr) {

	case NV_PMC_BOOT_0:	// chipset and stepping: NV2A, A02, Rev 0
		result = 0x02A000A2;
		break;
	case NV_PMC_INTR_0:
		result = pmc.pending_interrupts;
		break;
	case NV_PMC_INTR_EN_0:
		result = pmc.enabled_interrupts;
		break;

	case 0x0000020C: // What's this address? What does the xbe expect to read here? The Kernel base address perhaps?
		result = NV20_REG_BASE_KERNEL;
		break;
	default:
		DEBUG_READ32_UNHANDLED(PMC);
	}

	DEVICE_READ32_END(PMC);
}

DEVICE_WRITE32(PMC)
{
	DEVICE_WRITE32_SWITCH(PMC, addr) {

	case NV_PMC_INTR_0:
		pmc.pending_interrupts &= ~value;
		update_irq();
		break;
	case NV_PMC_INTR_EN_0:
		pmc.enabled_interrupts = value;
		update_irq();
		break;

	default: 
		DEBUG_WRITE32_UNHANDLED(PMC);
	}
}


DEVICE_READ32(PBUS)
{
	DEVICE_READ32_SWITCH(addr) {
	case NV_PBUS_PCI_NV_0:
		result = 0x10de;	// PCI_VENDOR_ID_NVIDIA	(?where to return PCI_DEVICE_ID_NVIDIA_NV2A = 0x01b7)

		break;
	case NV_PBUS_PCI_NV_1:
		result = 1; // NV_PBUS_PCI_NV_1_IO_SPACE_ENABLED
		break;
	case NV_PBUS_PCI_NV_2:
		result = (0x02 << 24) | 161; // PCI_CLASS_DISPLAY_3D (0x02) Rev 161 (0xA1) 
		break;

	default: 
		DEBUG_READ32_UNHANDLED(PBUS);
	}

	DEVICE_READ32_END(PBUS);
}

DEVICE_WRITE32(PBUS)
{
	DEVICE_WRITE32_SWITCH(PBUS, addr) {
	// TODO : Handle write on NV_PBUS_PCI_NV_1 with  1 (NV_PBUS_PCI_NV_1_IO_SPACE_ENABLED) + 4 (NV_PBUS_PCI_NV_1_BUS_MASTER_ENABLED)
	default: 
		DEBUG_WRITE32_UNHANDLED(PBUS);
	}
}


DEVICE_READ32(PFIFO)
{
	DEVICE_READ32_SWITCH(addr) {
	default: 
		DEBUG_READ32_UNHANDLED(PFIFO);
	}

	DEVICE_READ32_END(PFIFO);
}

DEVICE_WRITE32(PFIFO)
{
	DEVICE_WRITE32_SWITCH(PFIFO, addr) {
	default: 
		DEBUG_WRITE32_UNHANDLED(PFIFO);
	}
}


DEVICE_READ32(PFIFO_CACHE)
{
	DEVICE_READ32_SWITCH(addr) {
	default: 
		DEBUG_READ32_UNHANDLED(PFIFO_CACHE);
	}

	DEVICE_READ32_END(PFIFO_CACHE);
}

DEVICE_WRITE32(PFIFO_CACHE)
{
	DEVICE_WRITE32_SWITCH(PFIFO_CACHE, addr) {
	default: 
		DEBUG_WRITE32_UNHANDLED(PFIFO_CACHE);
	}
}


DEVICE_READ32(PRMA)
{
	DEVICE_READ32_SWITCH(addr) {
	default: 
		DEBUG_READ32_UNHANDLED(PRMA);
	}

	DEVICE_READ32_END(PRMA);
}

DEVICE_WRITE32(PRMA)
{
	DEVICE_WRITE32_SWITCH(PRMA, addr) {
	default: 
		DEBUG_WRITE32_UNHANDLED(PRMA);
	}
}


DEVICE_READ32(PVIDEO)
{
	DEVICE_READ32_SWITCH(addr) {

	case NV_PVIDEO_STOP:
		result = 0;
		break;
	default:
		result = pvideo.regs[addr];
	}

	DEVICE_READ32_END(PVIDEO);
}

DEVICE_WRITE32(PVIDEO)
{
	DEVICE_WRITE32_SWITCH(PVIDEO, addr) {
	default:
		pvideo.regs[addr] = value;
	}
}

DEVICE_READ32(PCOUNTER)
{
	DEVICE_READ32_SWITCH(addr) {
	default: 
		DEBUG_READ32_UNHANDLED(PCOUNTER);
	}

	DEVICE_READ32_END(PCOUNTER);
}

DEVICE_WRITE32(PCOUNTER)
{
	DEVICE_WRITE32_SWITCH(PCOUNTER, addr) {
	default: 
		DEBUG_WRITE32_UNHANDLED(PCOUNTER);
	}
}


DEVICE_READ32(PTIMER)
{
	DEVICE_READ32_SWITCH(addr) {

	case NV_PTIMER_DENOMINATOR:
		result = ptimer.denominator;
		break;
	case NV_PTIMER_NUMERATOR:
		result = ptimer.numerator;
		break;
	case NV_PTIMER_ALARM_0:
		result = ptimer.alarm_time;
		break;
	default: 
		DEBUG_READ32_UNHANDLED(PTIMER);
	}

	DEVICE_READ32_END(PTIMER);
}


DEVICE_WRITE32(PTIMER)
{
	DEVICE_WRITE32_SWITCH(PTIMER, addr) {

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
		DEBUG_WRITE32_UNHANDLED(PTIMER);
	}
}


DEVICE_READ32(PVPE)
{
	DEVICE_READ32_SWITCH(addr) {
	default: 
		DEBUG_READ32_UNHANDLED(PVPE);
	}

	DEVICE_READ32_END(PVPE);
}


DEVICE_WRITE32(PVPE)
{
	DEVICE_WRITE32_SWITCH(PVPE, addr) {
	default: 
		DEBUG_WRITE32_UNHANDLED(PVPE);
	}
}


DEVICE_READ32(PTV)
{
	DEVICE_READ32_SWITCH(addr) {
	default: 
		DEBUG_READ32_UNHANDLED(PTV);
	}

	DEVICE_READ32_END(PTV);
}

DEVICE_WRITE32(PTV)
{
	DEVICE_WRITE32_SWITCH(PTV, addr) {
	default: 
		DEBUG_WRITE32_UNHANDLED(PTV);
	}
}


DEVICE_READ32(PRMFB)
{
	DEVICE_READ32_SWITCH(addr) {
	default: 
		DEBUG_READ32_UNHANDLED(PRMFB);
	}

	DEVICE_READ32_END(PRMFB);
}

DEVICE_WRITE32(PRMFB)
{
	DEVICE_WRITE32_SWITCH(PRMFB, addr) {
	default: 
		DEBUG_WRITE32_UNHANDLED(PRMFB);
	}
}


DEVICE_READ32(PRMVIO)
{
	DEVICE_READ32_SWITCH(addr) {
	default: 
		DEBUG_READ32_UNHANDLED(PRMVIO);
	}

	DEVICE_READ32_END(PRMVIO);
}

DEVICE_WRITE32(PRMVIO)
{
	DEVICE_WRITE32_SWITCH(PRMVIO, addr) {
	default: 
		DEBUG_WRITE32_UNHANDLED(PRMVIO);
	}
}


DEVICE_READ32(PFB)
{
	DEVICE_READ32_SWITCH(addr) {
	default:
		result = pfb.regs[addr];
	}

	DEVICE_READ32_END(PFB);
}

DEVICE_WRITE32(PFB)
{
	DEVICE_WRITE32_SWITCH(PFB, addr) {
	default:
		pfb.regs[addr] = value;
	}
}


DEVICE_READ32(PSTRAPS)
{
	DEVICE_READ32_SWITCH(addr) {
	default: 
		DEBUG_READ32_UNHANDLED(PSTRAPS);
	}

	DEVICE_READ32_END(PSTRAPS);
}

DEVICE_WRITE32(PSTRAPS)
{
	DEVICE_WRITE32_SWITCH(PSTRAPS, addr) {
	default: 
		DEBUG_WRITE32_UNHANDLED(PSTRAPS);
	}
}


DEVICE_READ32(PGRAPH)
{
	DEVICE_READ32_SWITCH(addr) {
	default: 
		DEBUG_READ32_UNHANDLED(PGRAPH);
	}

	DEVICE_READ32_END(PGRAPH);
}

DEVICE_WRITE32(PGRAPH)
{
	DEVICE_WRITE32_SWITCH(PGRAPH, addr) {
	case NV_PGRAPH_INTR:
		pgraph.pending_interrupts &= ~value;
		update_irq();
		break;
	case NV_PGRAPH_INTR_EN:
		pgraph.enabled_interrupts = value;
		update_irq();
		break;
	default: 
		DEBUG_WRITE32_UNHANDLED(PGRAPH);
	}
}


DEVICE_READ32(PCRTC)
{
	DEVICE_READ32_SWITCH(addr) {

	case NV_PCRTC_INTR_0:
		result = pcrtc.pending_interrupts;
		break;
	case NV_PCRTC_INTR_EN_0:
		result = pcrtc.enabled_interrupts;
		break;
	case NV_PCRTC_START:
		result = pcrtc.start;
		break;
	default: 
		DEBUG_READ32_UNHANDLED(PCRTC);
	}

	DEVICE_READ32_END(PCRTC);
}

DEVICE_WRITE32(PCRTC)
{
	DEVICE_WRITE32_SWITCH(PCRTC, addr) {

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

	default: 
		DEBUG_WRITE32_UNHANDLED(PCRTC);
	}
}


DEVICE_READ32(PRMCIO)
{
	DEVICE_READ32_SWITCH(addr) {
	default: 
		DEBUG_READ32_UNHANDLED(PRMCIO);
	}

	DEVICE_READ32_END(PRMCIO);
}

DEVICE_WRITE32(PRMCIO)
{
	DEVICE_WRITE32_SWITCH(PRMCIO, addr) {
	default: 
		DEBUG_WRITE32_UNHANDLED(PRMCIO);
	}
}


DEVICE_READ32(PRAMDAC)
{
	DEVICE_READ32_SWITCH(addr) {

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

	default: 
		DEBUG_READ32_UNHANDLED(PRAMDAC);
	}

	DEVICE_READ32_END(PRAMDAC);
}

DEVICE_WRITE32(PRAMDAC)
{
	DEVICE_WRITE32_SWITCH(PRAMDAC, addr) {

	case NV_PRAMDAC_NVPLL_COEFF:
		pramdac.core_clock_coeff = value;
		break;
	case NV_PRAMDAC_MPLL_COEFF:
		pramdac.memory_clock_coeff = value;
		break;
	case NV_PRAMDAC_VPLL_COEFF:
		pramdac.video_clock_coeff = value;
		break;

	default: 
		DEBUG_WRITE32_UNHANDLED(PRAMDAC);
	}
}


DEVICE_READ32(PRMDIO)
{
	DEVICE_READ32_SWITCH(addr) {
	default: 
		DEBUG_READ32_UNHANDLED(PRMDIO);
	}

	DEVICE_READ32_END(PRMDIO);
}

DEVICE_WRITE32(PRMDIO)
{
	DEVICE_WRITE32_SWITCH(PRMDIO, addr) {
	default: 
		DEBUG_WRITE32_UNHANDLED(PRMDIO);
	}
}


DEVICE_READ32(PRAMIN)
{
	DEVICE_READ32_SWITCH(addr) {
	default: 
		DEBUG_READ32_UNHANDLED(PRAMIN);
	}

	DEVICE_READ32_END(PRAMIN);
}

DEVICE_WRITE32(PRAMIN)
{
	DEVICE_WRITE32_SWITCH(PRAMIN, addr) {
	default: 
		DEBUG_WRITE32_UNHANDLED(PRAMIN);
	}
}


DEVICE_READ32(USER)
{
	DEVICE_READ32_SWITCH(addr) {
	default: 
		DEBUG_READ32_UNHANDLED(USER);
	}

	DEVICE_READ32_END(USER);
}

DEVICE_WRITE32(USER)
{
	DEVICE_WRITE32_SWITCH(USER, addr) {
	default: 
		DEBUG_WRITE32_UNHANDLED(USER);
	}
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
