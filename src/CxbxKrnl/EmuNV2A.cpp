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

#ifdef _MSC_VER                         // Check if MS Visual C compiler
#  pragma comment(lib, "opengl32.lib")  // Compiler-specific directive to avoid manually configuration
//#  pragma comment(lib, "glu32.lib")     // Link libraries
#  pragma comment(lib, "glew32.lib")
#endif

#include <Zydis.hpp>

#include "CxbxKrnl.h"
#include "Emu.h"
#include "EmuNV2A.h"
#include "nv2a_int.h" // from https://github.com/espes/xqemu/tree/xbox/hw/xbox

#include <gl\glew.h>
#include <gl\GL.h>
#include <gl\GLU.h>
//#include <gl\glut.h>

#define NV_PMC_ADDR      0x00000000
#define NV_PMC_SIZE                 0x001000
#define NV_PBUS_ADDR     0x00001000
#define NV_PBUS_SIZE                0x001000
#define NV_PFIFO_ADDR    0x00002000
#define NV_PFIFO_SIZE               0x002000
#define NV_PRMA_ADDR     0x00007000
#define NV_PRMA_SIZE                0x001000
#define NV_VIDEO_ADDR    0x00008000
#define NV_PVIDEO_SIZE              0x001000
#define NV_PTIMER_ADDR   0x00009000
#define NV_PTIMER_SIZE              0x001000
#define NV_PCOUNTER_ADDR 0x0000A000
#define NV_PCOUNTER_SIZE            0x001000
#define NV_PVPE_ADDR     0x0000B000
#define NV_PVPE_SIZE                0x001000
#define NV_PTV_ADDR      0x0000D000
#define NV_PTV_SIZE                 0x001000
#define NV_PRMFB_ADDR    0x000A0000
#define NV_PRMFB_SIZE               0x020000
#define NV_PRMVIO_ADDR   0x000C0000
#define NV_PRMVIO_SIZE              0x001000
#define NV_PFB_ADDR      0x00100000
#define NV_PFB_SIZE                 0x001000
#define NV_PSTRAPS_ADDR  0x00101000
#define NV_PSTRAPS_SIZE             0x001000
#define NV_PGRAPH_ADDR   0x00400000
#define NV_PGRAPH_SIZE              0x002000
#define NV_PCRTC_ADDR    0x00600000
#define NV_PCRTC_SIZE               0x001000
#define NV_PRMCIO_ADDR   0x00601000
#define NV_PRMCIO_SIZE              0x001000
#define NV_PRAMDAC_ADDR  0x00680000
#define NV_PRAMDAC_SIZE             0x001000
#define NV_PRMDIO_ADDR   0x00681000
#define NV_PRMDIO_SIZE              0x001000
#define NV_PRAMIN_ADDR   0x00710000
#define NV_PRAMIN_SIZE              0x100000
#define NV_USER_ADDR     0x00800000
#define NV_USER_SIZE                0x800000

struct {
	uint32_t pending_interrupts;
	uint32_t enabled_interrupts;
	uint32_t regs[NV_PMC_SIZE / sizeof(uint32_t)]; // TODO : union
} pmc;

struct {
	uint32_t pending_interrupts;
	uint32_t enabled_interrupts;
	//TODO:
	// QemuThread puller_thread;
	// Cache1State cache1;
	uint32_t regs[NV_PFIFO_SIZE / sizeof(uint32_t)]; // TODO : union
} pfifo;

struct {
	uint32_t regs[NV_PVIDEO_SIZE / sizeof(uint32_t)]; // TODO : union
} pvideo;

struct {
	uint32_t pending_interrupts;
	uint32_t enabled_interrupts;
	uint32_t numerator;
	uint32_t denominator;
	uint32_t alarm_time;
	uint32_t regs[NV_PTIMER_SIZE / sizeof(uint32_t)]; // TODO : union
} ptimer;

struct {
	uint32_t regs[NV_PFB_SIZE / sizeof(uint32_t)]; // TODO : union
} pfb;

struct {
	uint32_t pending_interrupts;
	uint32_t enabled_interrupts;
	uint32_t start;
	uint32_t regs[NV_PCRTC_SIZE / sizeof(uint32_t)]; // TODO : union
} pcrtc;

struct {
	uint32_t core_clock_coeff;
	uint64_t core_clock_freq;
	uint32_t memory_clock_coeff;
	uint32_t video_clock_coeff;
	uint32_t regs[NV_PRAMDAC_SIZE / sizeof(uint32_t)]; // TODO : union
} pramdac;

struct {
	uint32_t regs[NV_PRAMIN_SIZE / sizeof(uint32_t)]; // TODO : union
} pramin;

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
		EmuWarning("EmuNV2A update_irq() : Raise IRQ Not Implemented");
	}
	else {
		// TODO: Cancel IRQ
		EmuWarning("EmuNV2A update_irq() : Cancel IRQ Not Implemented");
	}
}

#define DEBUG_START(DEV) const char *DebugNV_##DEV##(uint32_t addr) { switch (addr) {
#define CASE(a, c) case a: return #a##c;
#define DEBUG_END(DEV) default: return "Unknown " #DEV " Address"; } }

DEBUG_START(PMC)
	CASE(NV_PMC_BOOT_0);
	CASE(NV_PMC_INTR_0);
	CASE(NV_PMC_INTR_EN_0);
	CASE(NV_PMC_ENABLE);
DEBUG_END(PMC)

DEBUG_START(PBUS)
	CASE(NV_PBUS_PCI_NV_0, "_VENDOR_ID");
	CASE(NV_PBUS_PCI_NV_1);
	CASE(NV_PBUS_PCI_NV_2, "_REVISION_ID");
	CASE(NV_PBUS_PCI_NV_3, "_LATENCY_TIMER");
	CASE(NV_PBUS_PCI_NV_4);
	CASE(NV_PBUS_PCI_NV_5);
	CASE(NV_PBUS_PCI_NV_6);
	CASE(NV_PBUS_PCI_NV_7);
	CASE(NV_PBUS_PCI_NV_11, "_SUBSYSTEM");
	CASE(NV_PBUS_PCI_NV_12, "_ROM_BASE");
	CASE(NV_PBUS_PCI_NV_13, "_CAP_PTR");
	CASE(NV_PBUS_PCI_NV_14, "_RESERVED");
	CASE(NV_PBUS_PCI_NV_15);
	CASE(NV_PBUS_PCI_NV_16, "_SUBSYSTEM");
	CASE(NV_PBUS_PCI_NV_17);
	CASE(NV_PBUS_PCI_NV_18, "_AGP_STATUS");
	CASE(NV_PBUS_PCI_NV_19, "_AGP_COMMAND");
	CASE(NV_PBUS_PCI_NV_20, "_ROM_SHADOW");
	CASE(NV_PBUS_PCI_NV_21, "_VGA");
	CASE(NV_PBUS_PCI_NV_22, "_SCRATCH");
	CASE(NV_PBUS_PCI_NV_23, "_DT_TIMEOUT");
	CASE(NV_PBUS_PCI_NV_24, "_PME");
	CASE(NV_PBUS_PCI_NV_25, "_POWER_STATE");
	CASE(NV_PBUS_PCI_NV_26, "_RESERVED");
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
	CASE(NV_PFB_CFG0)
	CASE(NV_PFB_CSTATUS)
	CASE(NV_PFB_REFCTRL)
	CASE(NV_PFB_NVM) // 	NV_PFB_NVM_MODE_DISABLE 
	CASE(NV_PFB_PIN)
	CASE(NV_PFB_PAD)
	CASE(NV_PFB_TIMING0)
	CASE(NV_PFB_TIMING1)
	CASE(NV_PFB_TIMING2)
	CASE(NV_PFB_TILE)
	CASE(NV_PFB_TLIMIT)
	CASE(NV_PFB_TSIZE)
	CASE(NV_PFB_TSTATUS)
	CASE(NV_PFB_MRS)
	CASE(NV_PFB_EMRS)
	CASE(NV_PFB_MRS_EXT)
	CASE(NV_PFB_EMRS_EXT)
	CASE(NV_PFB_REF)
	CASE(NV_PFB_PRE)
	CASE(NV_PFB_ZCOMP)
	CASE(NV_PFB_ARB_PREDIVIDER)
	CASE(NV_PFB_ARB_TIMEOUT)
	CASE(NV_PFB_ARB_XFER_REM)
	CASE(NV_PFB_ARB_DIFF_BANK)
	CASE(NV_PFB_CLOSE_PAGE0)
	CASE(NV_PFB_CLOSE_PAGE1)
	CASE(NV_PFB_CLOSE_PAGE2)
	CASE(NV_PFB_BPARB)
	CASE(NV_PFB_CMDQ0)
	CASE(NV_PFB_CMDQ1)
	CASE(NV_PFB_ILL_INSTR)
	CASE(NV_PFB_RT)
	CASE(NV_PFB_AUTOCLOSE)
	CASE(NV_PFB_WBC)
	CASE(NV_PFB_CMDQ_PRT)
	CASE(NV_PFB_CPU_RRQ)
	CASE(NV_PFB_BYPASS);
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


#define READ32_START(DEV) uint32_t EmuNV2A_##DEV##_Read32(uint32_t addr) { uint32_t result = 0; switch (addr) {
#define READ32_UNHANDLED(DEV) default: EmuWarning("EmuX86_Read32 NV2A_" #DEV "(0x%08X) = 0x%08X [Unhandled, %s]", addr, result, DebugNV_##DEV##(addr)); return result;
#define READ32_END(DEV) } EmuWarning("EmuX86_Read32 NV2A_" #DEV "(0x%08X) = 0x%08X [Handled, %s]", addr, result, DebugNV_##DEV##(addr)); return result; }

#define WRITE32_START(DEV) void EmuNV2A_##DEV##_Write32(uint32_t addr, uint32_t value) { switch (addr) {
#define WRITE32_UNHANDLED(DEV) default: EmuWarning("EmuX86_Write32 NV2A_" #DEV "(0x%08X, 0x%08X) [Unhandled, %s]", addr, value, DebugNV_##DEV##(addr)); return;
#define WRITE32_END(DEV) } EmuWarning("EmuX86_Write32 NV2A_" #DEV "(0x%08X, 0x%08X) [Handled, %s]", addr, value, DebugNV_##DEV##(addr)); }  


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
	case NV_PFIFO_RAMHT:
		result = 0x03000100; // = NV_PFIFO_RAMHT_SIZE_4K | NV_PFIFO_RAMHT_BASE_ADDRESS(NumberOfPaddingBytes >> 12) | NV_PFIFO_RAMHT_SEARCH_128
	case NV_PFIFO_RAMFC:
		result = 0x00890110; // = ? | NV_PFIFO_RAMFC_SIZE_2K | ?
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
	case NV_PFB_CFG0:
		result = 3; // = NV_PFB_CFG0_PART_4
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
	default:
		result = pramin.regs[addr];
READ32_END(PRAMIN)

WRITE32_START(PRAMIN)
	default:
		pramin.regs[addr] = value;
WRITE32_END(PRAMIN)


READ32_START(USER)
	READ32_UNHANDLED(USER)
READ32_END(USER)

WRITE32_START(USER)
	WRITE32_UNHANDLED(USER)
WRITE32_END(USER)

typedef struct NV2ABlockInfo {
		uint32_t offset;
		uint32_t size;
		uint32_t(*read)(uint32_t addr);
		void(*write)(uint32_t addr, uint32_t value);
} NV2ABlockInfo;

static const NV2ABlockInfo regions[] = {{
		NV_PMC_ADDR, // = 0x000000
		NV_PMC_SIZE, // = 0x001000
		EmuNV2A_PMC_Read32,
		EmuNV2A_PMC_Write32,
	}, {
		NV_PBUS_ADDR, // = 0x001000
		NV_PBUS_SIZE, // = 0x001000
		EmuNV2A_PBUS_Read32,
		EmuNV2A_PBUS_Write32,
	}, {
		NV_PFIFO_ADDR, // = 0x002000
		NV_PFIFO_SIZE, // = 0x002000
		EmuNV2A_PFIFO_Read32,
		EmuNV2A_PFIFO_Write32,
	}, {
		NV_PRMA_ADDR, // = 0x007000
		NV_PRMA_SIZE, // = 0x001000
		EmuNV2A_PRMA_Read32,
		EmuNV2A_PRMA_Write32,
	}, {
		NV_VIDEO_ADDR, // = 0x008000
		NV_PVIDEO_SIZE, // = 0x001000
		EmuNV2A_PVIDEO_Read32,
		EmuNV2A_PVIDEO_Write32,
	}, {
		NV_PTIMER_ADDR, // = 0x009000
		NV_PTIMER_SIZE, // = 0x001000
		EmuNV2A_PTIMER_Read32,
		EmuNV2A_PTIMER_Write32,
	}, {
		NV_PCOUNTER_ADDR, // = 0x00a000
		NV_PCOUNTER_SIZE, // = 0x001000
		EmuNV2A_PCOUNTER_Read32,
		EmuNV2A_PCOUNTER_Write32,
	}, {
		NV_PVPE_ADDR, // = 0x00b000
		NV_PVPE_SIZE, // = 0x001000
		EmuNV2A_PVPE_Read32,
		EmuNV2A_PVPE_Write32,
	},	{
		NV_PTV_ADDR, // = 0x00d000
		NV_PTV_SIZE, // = 0x001000
		EmuNV2A_PTV_Read32,
		EmuNV2A_PTV_Write32,
	}, {
		NV_PRMFB_ADDR, // = 0x0a0000
		NV_PRMFB_SIZE, // = 0x020000
		EmuNV2A_PRMFB_Read32,
		EmuNV2A_PRMFB_Write32,
	}, {
		NV_PRMVIO_ADDR, // = 0x0c0000
		NV_PRMVIO_SIZE, // = 0x001000
		EmuNV2A_PRMVIO_Read32,
		EmuNV2A_PRMVIO_Write32,
	},{
		NV_PFB_ADDR, // = 0x100000
		NV_PFB_SIZE, // = 0x001000
		EmuNV2A_PFB_Read32,
		EmuNV2A_PFB_Write32,
	}, {
		NV_PSTRAPS_ADDR, // = 0x101000
		NV_PSTRAPS_SIZE, // = 0x001000
		EmuNV2A_PSTRAPS_Read32,
		EmuNV2A_PSTRAPS_Write32,
	}, {
		NV_PGRAPH_ADDR, // = 0x400000
		NV_PGRAPH_SIZE, // = 0x002000
		EmuNV2A_PGRAPH_Read32,
		EmuNV2A_PGRAPH_Write32,
	}, {
		NV_PCRTC_ADDR, // = 0x600000
		NV_PCRTC_SIZE, // = 0x001000
		EmuNV2A_PCRTC_Read32,
		EmuNV2A_PCRTC_Write32,
	}, {
		NV_PRMCIO_ADDR, // = 0x601000
		NV_PRMCIO_SIZE, // = 0x001000
		EmuNV2A_PRMCIO_Read32,
		EmuNV2A_PRMCIO_Write32,
	}, {
		NV_PRAMDAC_ADDR, // = 0x680000
		NV_PRAMDAC_SIZE, // = 0x001000
		EmuNV2A_PRAMDAC_Read32,
		EmuNV2A_PRAMDAC_Write32,
	}, {
		NV_PRMDIO_ADDR, // = 0x681000
		NV_PRMDIO_SIZE, // = 0x001000
		EmuNV2A_PRMDIO_Read32,
		EmuNV2A_PRMDIO_Write32,
	}, {
		NV_PRAMIN_ADDR, // = 0x710000
		NV_PRAMIN_SIZE, // = 0x100000
		EmuNV2A_PRAMIN_Read32,
		EmuNV2A_PRAMIN_Write32,
	},{
		NV_USER_ADDR, // = 0x800000,
		NV_USER_SIZE, // = 0x800000,
		EmuNV2A_USER_Read32,
		EmuNV2A_USER_Write32,
	}, {
		0xFFFFFFFF,
		0,
		nullptr,
		nullptr,
	},
};

const NV2ABlockInfo* EmuNV2A_Block(uint32_t addr) 
{
	// Find the block in the block table
	const NV2ABlockInfo* block = &regions[0];
	int i = 0;

	while (block->read != nullptr) {
		if (addr >= block->offset && addr < block->offset + block->size) {
			return block;
		}

		block = &regions[++i];
	}

	return nullptr;
}

uint32_t EmuNV2A_Read32(uint32_t addr)
{
	const NV2ABlockInfo* block = EmuNV2A_Block(addr);

	if (block != nullptr) {
		return block->read(addr - block->offset);
	}

	EmuWarning("EmuNV2A_Read32: Unhandled Read Address %08X", addr);
	return 0;
}

void EmuNV2A_Write32(uint32_t addr, uint32_t value)
{
	const NV2ABlockInfo* block = EmuNV2A_Block(addr);

	if (block != nullptr) {
		block->write(addr - block->offset, value);
		return;
	}

	EmuWarning("EmuNV2A_Write32: Unhandled Write Address %08X (value %08X)", addr, value);
	return;
}

//
// OPENGL
//

// 
#define X_D3DTS_STAGECOUNT 4

HDC g_EmuWindowsDC = 0;
GLuint VertexProgramIDs[4] = { 0, 0, 0, 0 };
uint ActiveVertexProgramID = 0;
GLuint TextureIDs[X_D3DTS_STAGECOUNT]= { 0, 0, 0, 0 };

// Vertex shader header, mapping Xbox1 registers to the ARB syntax (original version by KingOfC).
// Note about the use of 'conventional' attributes in here: Since we prefer to use only one shader
// for both immediate and deferred mode rendering, we alias all attributes to conventional inputs
// as much as possible. Only when there's no conventional attribute available, we use generic attributes.
// So in the following header, we use conventional attributes first, and generic attributes for the
// rest of the vertex attribute slots. This makes it possible to support immediate and deferred mode
// rendering with the same shader, and the use of the OpenGL fixed-function pipeline without a shader.
std::string DxbxVertexShaderHeader =
    "!!ARBvp1.0\n"
    "TEMP R0,R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12;\n"
    "ADDRESS A0;\n"
#ifdef DXBX_OPENGL_CONVENTIONAL
    "ATTRIB v0 = vertex.position;\n" // Was: vertex.attrib[0] (See "conventional" note above)
    "ATTRIB v1 = vertex.%s;\n" // Note : We replace this with "weight" or "attrib[1]" depending GL_ARB_vertex_blend
    "ATTRIB v2 = vertex.normal;\n" // Was: vertex.attrib[2]
    "ATTRIB v3 = vertex.color.primary;\n" // Was: vertex.attrib[3]
    "ATTRIB v4 = vertex.color.secondary;\n" // Was: vertex.attrib[4]
    "ATTRIB v5 = vertex.fogcoord;\n" // Was: vertex.attrib[5]
    "ATTRIB v6 = vertex.attrib[6];\n"
    "ATTRIB v7 = vertex.attrib[7];\n"
    "ATTRIB v8 = vertex.texcoord[0];\n" // Was: vertex.attrib[8]
    "ATTRIB v9 = vertex.texcoord[1];\n" // Was: vertex.attrib[9]
    "ATTRIB v10 = vertex.texcoord[2];\n" // Was: vertex.attrib[10]
    "ATTRIB v11 = vertex.texcoord[3];\n" // Was: vertex.attrib[11]
#else
    "ATTRIB v0 = vertex.attrib[0];\n"
    "ATTRIB v1 = vertex.attrib[1];\n"
    "ATTRIB v2 = vertex.attrib[2];\n"
    "ATTRIB v3 = vertex.attrib[3];\n"
    "ATTRIB v4 = vertex.attrib[4];\n"
    "ATTRIB v5 = vertex.attrib[5];\n"
    "ATTRIB v6 = vertex.attrib[6];\n"
    "ATTRIB v7 = vertex.attrib[7];\n"
    "ATTRIB v8 = vertex.attrib[8];\n"
    "ATTRIB v9 = vertex.attrib[9];\n"
    "ATTRIB v10 = vertex.attrib[10];\n"
    "ATTRIB v11 = vertex.attrib[11];\n"
#endif
    "ATTRIB v12 = vertex.attrib[12];\n"
    "ATTRIB v13 = vertex.attrib[13];\n"
    "ATTRIB v14 = vertex.attrib[14];\n"
    "ATTRIB v15 = vertex.attrib[15];\n"
    "OUTPUT oPos = result.position;\n"
    "OUTPUT oD0 = result.color.front.primary;\n"
    "OUTPUT oD1 = result.color.front.secondary;\n"
    "OUTPUT oB0 = result.color.back.primary;\n"
    "OUTPUT oB1 = result.color.back.secondary;\n"
    "OUTPUT oPts = result.pointsize;\n"
    "OUTPUT oFog = result.fogcoord;\n"
    "OUTPUT oT0 = result.texcoord[0];\n"
    "OUTPUT oT1 = result.texcoord[1];\n"
    "OUTPUT oT2 = result.texcoord[2];\n"
    "OUTPUT oT3 = result.texcoord[3];\n"
    "PARAM c[] = { program.env[0..191] };\n" // All constants in 1 array declaration (requires NV_gpu_program4?)
    "PARAM mvp[4] = { state.matrix.mvp };\n";

void SetupPixelFormat(HDC DC)
{
	const PIXELFORMATDESCRIPTOR pfd = {
		 /* .nSize = */ sizeof(PIXELFORMATDESCRIPTOR), // size
		 /* .nVersion = */ 1,   // version
		 /* .dwFlags = */ PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER, // support double-buffering
		 /* .iPixelType = */ PFD_TYPE_RGBA, // color type
		 /* .cColorBits = */ 32,   // preferred color depth
		 /* .cRedBits = */ 0,
		 /* .cRedShift = */ 0, // color bits (ignored)
		 /* .cGreenBits = */ 0,
		 /* .cGreenShift = */ 0,
		 /* .cBlueBits = */ 0,
		 /* .cBlueShift = */ 0,
		 /* .cAlphaBits = */ 0,
		 /* .cAlphaShift = */ 0,   // no alpha buffer
		 /* .cAccumBits = */ 0,
		 /* .cAccumRedBits = */ 0,    // no accumulation buffer,
		 /* .cAccumGreenBits = */ 0,      // accum bits (ignored)
		 /* .cAccumBlueBits = */ 0,
		 /* .cAccumAlphaBits = */ 0,
		 /* .cDepthBits = */ 16,   // depth buffer
		 /* .cStencilBits = */ 0,   // no stencil buffer
		 /* .cAuxBuffers = */ 0,   // no auxiliary buffers
		 /* .iLayerType= */ PFD_MAIN_PLANE,   // main layer
		 /* .bReserved = */ 0,
		 /* .dwLayerMask = */ 0,
		 /* .dwVisibleMask = */ 0,
		 /* .dwDamageMask = */ 0                    // no layer, visible, damage masks
	};

	int PixelFormat = ChoosePixelFormat(DC, &pfd);
	if (PixelFormat == 0)
		return;

	if (SetPixelFormat(DC, PixelFormat, &pfd) != TRUE)
		return;
}

// From https://github.com/inolen/redream/blob/master/src/video/gl_backend.c
static int rb_compile_shader(const char *source, GLenum shader_type, GLuint *shader)
{
	size_t sourceLength = strlen(source);

	*shader = glCreateShader(shader_type);
	glShaderSource(*shader, 1, (const GLchar **)&source,
		(const GLint *)&sourceLength);
	glCompileShader(*shader);

	GLint compiled;
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &compiled);

	if (!compiled) {
//		rb_print_shader_log(*shader);
		glDeleteShader(*shader);
		return 0;
	}

	return 1;
}

void DxbxCompileShader(std::string Shader)
{
	int GLErrorPos;

//	if (MayLog(lfUnit))
//		DbgPrintf("  NV2A: New vertex program :\n" + Shader);

/*
glProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, Shader.size(), Shader.c_str());

	// errors are catched
	glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &GLErrorPos);
	*/
	GLuint shader;

	GLErrorPos = rb_compile_shader(Shader.c_str(), GL_VERTEX_SHADER, &shader); // TODO : GL_VERTEX_SHADER_ARB ??
	/*
	if (GLErrorPos > 0) 
	{
		Shader.insert(GLErrorPos, "{ERROR}");
		EmuWarning("Program error at position %d:", GLErrorPos);
		EmuWarning((char*)glGetString(GL_PROGRAM_ERROR_STRING_ARB));
		EmuWarning(Shader.c_str());
	}
	*/
}
void InitOpenGLContext()
{
	HGLRC RC;
	std::string szCode;

	//glutInit();
	{ // rb_init_context();
	/* link in gl functions at runtime */
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (err != GLEW_OK) {
			//LOG_WARNING("GLEW initialization failed: %s", glewGetErrorString(err));
			return;
		}
	}
	g_EmuWindowsDC = GetDC(g_hEmuWindow); // Actually, you can use any windowed control here
	SetupPixelFormat(g_EmuWindowsDC);

	RC = wglCreateContext(g_EmuWindowsDC); // makes OpenGL window out of DC
	wglMakeCurrent(g_EmuWindowsDC, RC);   // makes OpenGL window active
	//ReadImplementationProperties(); // Determine a set of booleans indicating which OpenGL extensions are available
	//ReadExtensions(); // Assign all OpenGL extension API's (DON'T call them if the extension is not available!)

	// Initialize the viewport :
	//Viewport.X = 0;
	//Viewport.Y = 0;
	//Viewport.Width = g_EmuCDPD.pPresentationParameters.BackBufferWidth;
	//Viewport.Height = g_EmuCDPD.pPresentationParameters.BackBufferHeight;
	//Viewport.MinZ = -1.0;
	//Viewport.MaxZ = 1.0;

	//DxbxUpdateTransformProjection();
	//DxbxUpdateViewport();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Switch to left-handed coordinate space (as per http://www.opengl.org/resources/faq/technical/transformations.htm) :
	//  glScalef(1.0, 1.0, -1.0);

	// Set some defaults :
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL); // Nearer Z coordinates cover further Z

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glAlphaFunc(GL_GEQUAL, 0.5);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // GL_LINE for wireframe

/*
	// TODO : The following code only works on cards that support the
	// vertex program extensions (NVidia cards mainly); So for ATI we
	// have to come up with another solution !!!
	glGenProgramsARB(4, &VertexProgramIDs[0]);
*/

#ifdef DXBX_OPENGL_CONVENTIONAL
	if (GL_ARB_vertex_blend)
		DxbxVertexShaderHeader = sprintf(DxbxVertexShaderHeader, "weight");
	else
		DxbxVertexShaderHeader = sprintf(DxbxVertexShaderHeader, "attrib[1]");
#endif

	// Precompiled shader for the fixed function pipeline :
	szCode = DxbxVertexShaderHeader +
		"# This part adjusts the vertex position by the super-sampling scale & offset :\n"
		"MOV R0, v0;\n"
		"RCP R0.w, R0.w;\n"
		"MUL R0, R0, c[0];\n" // c[-96] in D3D speak - applies SuperSampleScale
								// Note : Use R12 instead of oPos because this is not yet the final assignment :
		"ADD R12, R0, c[1];\n" // c[-95] in D3D speak - applies SuperSampleOffset

		"# This part just reads all other components and passes them to the output :\n"
		"MOV oD0, v3;\n"
		"MOV oD1, v4;\n"
		"MOV oFog, v4.w;\n" // specular fog
							  //    "MOV oFog, v0.z;\n" // z fog
							  //    "RCP oFog, v0.w;\n" // w fog
		"MOV oPts, v1.x;\n"
		"MOV oB0, v7;\n"
		"MOV oB1, v8;\n"
		"MOV oT0, v9;\n"
		"MOV oT1, v10;\n"
		"MOV oT2, v11;\n"
		"MOV oT3, v12;\n"

		"# This part applies the screen-space transform (not present when '#pragma screenspace' was used) :\n"
		"MUL R12.xyz, R12, c[58];\n" // c[-38] in D3D speak - see EmuNV2A_ViewportScale,
		"RCP R1.x, R12.w;\n" // Originally RCC, but that"s not supported in ARBvp1.0 (use "MIN R1, R1, 0" and "MAX R1, R1, 1"?)
		"MAD R12.xyz, R12, R1.x, c[59];\n" // c[-37] in D3D speak - see EmuNV2A_ViewportOffset

		"# Dxbx addition : Transform the vertex to clip coordinates :\n"
		"DP4 R0.x, mvp[0], R12;\n"
		"DP4 R0.y, mvp[1], R12;\n"
		"DP4 R0.z, mvp[2], R12;\n"
		"DP4 R0.w, mvp[3], R12;\n"
		"MOV R12, R0;\n"

		"# Apply Z coord mapping\n"
		"ADD R12.z, R12.z, R12.z;\n"
		"ADD R12.z, R12.z, -R12.w;\n"

		"# Here""s the final assignment to oPos :\n"
		"MOV oPos, R12;\n"
		"END\n"; // TODO : Check if newline is required?

//	glBindProgramARB(GL_VERTEX_PROGRAM_ARB, VertexProgramIDs[0]);
	DxbxCompileShader(szCode);
}
