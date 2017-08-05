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
// *  Copyright(c) 2012 espes
// *  Copyright(c) 2015 Jannik Vogel
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

#include <distorm.h> // For uint32_t
#include <string> // For std::string

#include "CxbxKrnl.h"
#include "device.h"
#include "Emu.h"
#include "EmuNV2A.h"
#include "nv2a_int.h" // from https://github.com/espes/xqemu/tree/xbox/hw/xbox

#include <gl\glew.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <cassert>
//#include <gl\glut.h>

#define NV_PMC_ADDR      0x00000000
#define NV_PMC_SIZE                 0x001000
#define NV_PBUS_ADDR     0x00001000
#define NV_PBUS_SIZE                0x001000
#define NV_PFIFO_ADDR    0x00002000
#define NV_PFIFO_SIZE               0x002000
#define NV_PRMA_ADDR     0x00007000
#define NV_PRMA_SIZE                0x001000
#define NV_PVIDEO_ADDR   0x00008000
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
#define NV_PRAMIN_ADDR   0x00700000
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

typedef struct GraphicsContext {
	bool channel_3d;
	unsigned int subchannel;
} GraphicsContext;


struct {
	uint32_t pending_interrupts;
	uint32_t enabled_interrupts;

	uint32_t context_table;
	uint32_t context_address;

	unsigned int trapped_method;
	unsigned int trapped_subchannel;
	unsigned int trapped_channel_id;
	uint32_t trapped_data[2];
	uint32_t notify_source;

	bool fifo_access;

	unsigned int channel_id;
	bool channel_valid;
	GraphicsContext context[NV2A_NUM_CHANNELS];


	uint32_t regs[NV_PGRAPH_SIZE / sizeof(uint32_t)]; // TODO : union
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
		EmuWarning("NV2A update_irq() : Raise IRQ Not Implemented");
	}
	else {
		// TODO: Cancel IRQ
		EmuWarning("NV2A update_irq() : Cancel IRQ Not Implemented");
	}
}


#define DEBUG_START(DEV) \
const char *DebugNV_##DEV##(xbaddr addr) \
{ \
	switch (addr) {
#define DEBUG_CASE(a) \
		case a: return #a;
#define DEBUG_CASE_EX(a, c) \
		case a: return #a##c;
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
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_0, ":VENDOR_ID");
	DEBUG_CASE(NV_PBUS_PCI_NV_1);
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_2, ":REVISION_ID");
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_3, ":LATENCY_TIMER");
	DEBUG_CASE(NV_PBUS_PCI_NV_4);
	DEBUG_CASE(NV_PBUS_PCI_NV_5);
	DEBUG_CASE(NV_PBUS_PCI_NV_6);
	DEBUG_CASE(NV_PBUS_PCI_NV_7);
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_11, ":SUBSYSTEM");
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_12, ":ROM_BASE");
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_13, ":CAP_PTR");
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_14, ":RESERVED");
	DEBUG_CASE(NV_PBUS_PCI_NV_15);
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_16, ":SUBSYSTEM");
	DEBUG_CASE(NV_PBUS_PCI_NV_17);
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_18, ":AGP_STATUS");
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_19, ":AGP_COMMAND");
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_20, ":ROM_SHADOW");
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_21, ":VGA");
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_22, ":SCRATCH");
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_23, ":DT_TIMEOUT");
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_24, ":PME");
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_25, ":POWER_STATE");
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_26, ":RESERVED");
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
DEBUG_END(PFIFO)

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
	DEBUG_CASE(NV_PFB_CFG0)
	DEBUG_CASE(NV_PFB_CSTATUS)
	DEBUG_CASE(NV_PFB_REFCTRL)
	DEBUG_CASE(NV_PFB_NVM) // 	NV_PFB_NVM_MODE_DISABLE 
	DEBUG_CASE(NV_PFB_PIN)
	DEBUG_CASE(NV_PFB_PAD)
	DEBUG_CASE(NV_PFB_TIMING0)
	DEBUG_CASE(NV_PFB_TIMING1)
	DEBUG_CASE(NV_PFB_TIMING2)
	DEBUG_CASE(NV_PFB_TILE)
	DEBUG_CASE(NV_PFB_TLIMIT)
	DEBUG_CASE(NV_PFB_TSIZE)
	DEBUG_CASE(NV_PFB_TSTATUS)
	DEBUG_CASE(NV_PFB_MRS)
	DEBUG_CASE(NV_PFB_EMRS)
	DEBUG_CASE(NV_PFB_MRS_EXT)
	DEBUG_CASE(NV_PFB_EMRS_EXT)
	DEBUG_CASE(NV_PFB_REF)
	DEBUG_CASE(NV_PFB_PRE)
	DEBUG_CASE(NV_PFB_ZCOMP)
	DEBUG_CASE(NV_PFB_ARB_PREDIVIDER)
	DEBUG_CASE(NV_PFB_ARB_TIMEOUT)
	DEBUG_CASE(NV_PFB_ARB_XFER_REM)
	DEBUG_CASE(NV_PFB_ARB_DIFF_BANK)
	DEBUG_CASE(NV_PFB_CLOSE_PAGE0)
	DEBUG_CASE(NV_PFB_CLOSE_PAGE1)
	DEBUG_CASE(NV_PFB_CLOSE_PAGE2)
	DEBUG_CASE(NV_PFB_BPARB)
	DEBUG_CASE(NV_PFB_CMDQ0)
	DEBUG_CASE(NV_PFB_CMDQ1)
	DEBUG_CASE(NV_PFB_ILL_INSTR)
	DEBUG_CASE(NV_PFB_RT)
	DEBUG_CASE(NV_PFB_AUTOCLOSE)
	DEBUG_CASE(NV_PFB_WBC)
	DEBUG_CASE(NV_PFB_CMDQ_PRT)
	DEBUG_CASE(NV_PFB_CPU_RRQ)
	DEBUG_CASE(NV_PFB_BYPASS);
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
	DEBUG_CASE(NV_PGRAPH_TEXADDRESS0_ADDRV);
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




#define DEBUG_READ32(DEV)            DbgPrintf("EmuX86 Read32 NV2A " #DEV "(0x%08X) = 0x%08X [Handled, %s]\n", addr, result, DebugNV_##DEV##(addr))
#define DEBUG_READ32_UNHANDLED(DEV)  { DbgPrintf("EmuX86 Read32 NV2A " #DEV "(0x%08X) = 0x%08X [Unhandled, %s]\n", addr, result, DebugNV_##DEV##(addr)); return result; }

#define DEBUG_WRITE32(DEV)           DbgPrintf("EmuX86 Write32 NV2A " #DEV "(0x%08X, 0x%08X) [Handled, %s]\n", addr, value, DebugNV_##DEV##(addr))
#define DEBUG_WRITE32_UNHANDLED(DEV) { DbgPrintf("EmuX86 Write32 NV2A " #DEV "(0x%08X, 0x%08X) [Unhandled, %s]\n", addr, value, DebugNV_##DEV##(addr)); return; }

#define DEVICE_READ32(DEV) uint32_t EmuNV2A_##DEV##_Read32(xbaddr addr)
#define DEVICE_READ32_SWITCH() uint32_t result = 0; switch (addr) 
#define DEVICE_READ32_REG(dev) result = dev.regs[addr / sizeof(uint32_t)]
#define DEVICE_READ32_END(DEV) DEBUG_READ32(DEV); return result

#define DEVICE_WRITE32(DEV) void EmuNV2A_##DEV##_Write32(xbaddr addr, uint32_t value)
#define DEVICE_WRITE32_REG(dev) dev.regs[addr / sizeof(uint32_t)] = value
#define DEVICE_WRITE32_END(DEV) DEBUG_WRITE32(DEV)


DWORD __index;
#define GET_MASK(v, mask) {											  \
		(bool)(((value) & (mask)) >> (_BitScanForward(&__index, mask) - 1)) \
	}

#define SET_MASK(v, mask, val) {                                     \
        const unsigned int __val = val;                              \
        const unsigned int __mask =  mask;                           \
        (v) &= ~(__mask);                                            \
        (v) |= ((__val) << (_BitScanForward(&__index, __mask)-1)) & (__mask);              \
    }

DEVICE_READ32(PMC)
{
	DEVICE_READ32_SWITCH() {
	case NV_PMC_BOOT_0:	// chipset and stepping: NV2A, A02, Rev 0
		result = 0x02A000A2;
		break;
	case NV_PMC_INTR_0:
		result = pmc.pending_interrupts;
		break;
	case NV_PMC_INTR_EN_0:
		result = pmc.enabled_interrupts;
		break;
	default:
		DEVICE_READ32_REG(pmc); // Was : DEBUG_READ32_UNHANDLED(PMC);
		break;
	}

	DEVICE_READ32_END(PMC);
}

DEVICE_WRITE32(PMC)
{
	switch(addr) {
	case NV_PMC_INTR_0:
		pmc.pending_interrupts &= ~value;
		update_irq();
		break;
	case NV_PMC_INTR_EN_0:
		pmc.enabled_interrupts = value;
		update_irq();
		break;

	default: 
		DEVICE_WRITE32_REG(pmc); // Was : DEBUG_WRITE32_UNHANDLED(PMC);
		break;
	}

	DEVICE_WRITE32_END(PMC);
}


DEVICE_READ32(PBUS)
{
	DEVICE_READ32_SWITCH() {
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
		DEBUG_READ32_UNHANDLED(PBUS); // TODO : DEVICE_READ32_REG(pbus);
		break;
	}

	DEVICE_READ32_END(PBUS);
}

DEVICE_WRITE32(PBUS)
{
	switch(addr) {
	case NV_PBUS_PCI_NV_1:
		// TODO : Handle write on NV_PBUS_PCI_NV_1 with  1 (NV_PBUS_PCI_NV_1_IO_SPACE_ENABLED) + 4 (NV_PBUS_PCI_NV_1_BUS_MASTER_ENABLED)
		break;
	default: 
		DEBUG_WRITE32_UNHANDLED(PBUS); // TODO : DEVICE_WRITE32_REG(pbus);
		break;
	}

	DEVICE_WRITE32_END(PBUS);
}


DEVICE_READ32(PFIFO)
{
	DEVICE_READ32_SWITCH() {
	case NV_PFIFO_RAMHT:
		result = 0x03000100; // = NV_PFIFO_RAMHT_SIZE_4K | NV_PFIFO_RAMHT_BASE_ADDRESS(NumberOfPaddingBytes >> 12) | NV_PFIFO_RAMHT_SEARCH_128
		break;
	case NV_PFIFO_RAMFC:
		result = 0x00890110; // = ? | NV_PFIFO_RAMFC_SIZE_2K | ?
		break;
	case NV_PFIFO_INTR_0:
		result = pfifo.pending_interrupts;
		break;
	case NV_PFIFO_INTR_EN_0:
		result = pfifo.enabled_interrupts;
		break;
	case NV_PFIFO_RUNOUT_STATUS:
		result = NV_PFIFO_RUNOUT_STATUS_LOW_MARK; /* low mark empty */
		break;
	default:
		DEVICE_READ32_REG(pfifo); // Was : DEBUG_READ32_UNHANDLED(PFIFO);
		break;
	}

	DEVICE_READ32_END(PFIFO);
}

DEVICE_WRITE32(PFIFO)
{
	switch(addr) {
	case NV_PFIFO_INTR_0:
		pfifo.pending_interrupts &= ~value;
		update_irq();
		break;
	case NV_PFIFO_INTR_EN_0:
		pfifo.enabled_interrupts = value;
		update_irq();
		break;
	default: 
		DEVICE_WRITE32_REG(pfifo); // Was : DEBUG_WRITE32_UNHANDLED(PFIFO);
		break;
	}

	DEVICE_WRITE32_END(PFIFO);
}


DEVICE_READ32(PRMA)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEBUG_READ32_UNHANDLED(PRMA); // TODO : DEVICE_READ32_REG(prma);
		break;
	}

	DEVICE_READ32_END(PRMA);
}

DEVICE_WRITE32(PRMA)
{
	switch(addr) {
	default: 
		DEBUG_WRITE32_UNHANDLED(PRMA); // TODO : DEVICE_WRITE32_REG(prma);
		break;
	}

	DEVICE_WRITE32_END(PRMA);
}


DEVICE_READ32(PVIDEO)
{
	DEVICE_READ32_SWITCH() {

	case NV_PVIDEO_STOP:
		result = 0;
		break;
	default:
		DEVICE_READ32_REG(pvideo);
		break;
	}

	DEVICE_READ32_END(PVIDEO);
}

DEVICE_WRITE32(PVIDEO)
{
	switch (addr) {
	default:
		DEVICE_WRITE32_REG(pvideo);
		break;
	}

	DEVICE_WRITE32_END(PVIDEO);
}

DEVICE_READ32(PCOUNTER)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEBUG_READ32_UNHANDLED(PCOUNTER); // TODO : DEVICE_READ32_REG(pcounter);
		break;
	}

	DEVICE_READ32_END(PCOUNTER);
}

DEVICE_WRITE32(PCOUNTER)
{
	switch (addr) {
	default:
		DEBUG_WRITE32_UNHANDLED(PCOUNTER); // TODO : DEVICE_WRITE32_REG(pcounter);
		break;
	}

	DEVICE_WRITE32_END(PCOUNTER);
}


DEVICE_READ32(PTIMER)
{
	DEVICE_READ32_SWITCH() {
	case NV_PTIMER_INTR_0:
		result = ptimer.pending_interrupts;
		break;
	case NV_PTIMER_INTR_EN_0:
		result = ptimer.enabled_interrupts;
		break;
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
		DEVICE_READ32_REG(ptimer); // Was : DEBUG_READ32_UNHANDLED(PTIMER);
		break;
	}

	DEVICE_READ32_END(PTIMER);
}


DEVICE_WRITE32(PTIMER)
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
		DEVICE_WRITE32_REG(ptimer); // Was : DEBUG_WRITE32_UNHANDLED(PTIMER);
		break;
	}

	DEVICE_WRITE32_END(PTIMER);
}


DEVICE_READ32(PVPE)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEBUG_READ32_UNHANDLED(PVPE); // TODO : DEVICE_READ32_REG(pvpe);
		break;
	}

	DEVICE_READ32_END(PVPE);
}


DEVICE_WRITE32(PVPE)
{
	switch (addr) {
	default:
		DEBUG_WRITE32_UNHANDLED(PVPE); // TODO : DEVICE_WRITE32_REG(pvpe);
		break;
	}

	DEVICE_WRITE32_END(PVPE);
}


DEVICE_READ32(PTV)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEBUG_READ32_UNHANDLED(PTV); // TODO : DEVICE_READ32_REG(ptv);
		break;
	}

	DEVICE_READ32_END(PTV);
}

DEVICE_WRITE32(PTV)
{
	switch (addr) {
	default:
		DEBUG_WRITE32_UNHANDLED(PTV); // TODO : DEVICE_WRITE32_REG(ptv);
		break;
	}

	DEVICE_WRITE32_END(PTV);
}


DEVICE_READ32(PRMFB)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEBUG_READ32_UNHANDLED(PRMFB); // TODO : DEVICE_READ32_REG(prmfb);
		break;
	}

	DEVICE_READ32_END(PRMFB);
}

DEVICE_WRITE32(PRMFB)
{
	switch (addr) {
	default:
		DEBUG_WRITE32_UNHANDLED(PRMFB); // TODO : DEVICE_WRITE32_REG(prmfb);
		break;
	}

	DEVICE_WRITE32_END(PRMFB);
}


DEVICE_READ32(PRMVIO)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEBUG_READ32_UNHANDLED(PRMVIO); // TODO : DEVICE_READ32_REG(prmvio);
		break;
	}

	DEVICE_READ32_END(PRMVIO);
}

DEVICE_WRITE32(PRMVIO)
{
	switch (addr) {
	default:
		DEBUG_WRITE32_UNHANDLED(PRMVIO); // TODO : DEVICE_WRITE32_REG(prmvio);
		break;
	}

	DEVICE_WRITE32_END(PRMVIO);
}


DEVICE_READ32(PFB)
{
	DEVICE_READ32_SWITCH() {
	case NV_PFB_CFG0:
		result = 3; // = NV_PFB_CFG0_PART_4
		break;
	case NV_PFB_CSTATUS:
		result = CONTIGUOUS_MEMORY_SIZE;
		break;
	case NV_PFB_WBC:
		result = 0; // = !NV_PFB_WBC_FLUSH
		break;
	default:
		DEVICE_READ32_REG(pfb);
		break;
	}

	DEVICE_READ32_END(PFB);
}

DEVICE_WRITE32(PFB)
{
	switch (addr) {
	default:
		DEVICE_WRITE32_REG(pfb);
		break;
	}

	DEVICE_WRITE32_END(PFB);
}


DEVICE_READ32(PSTRAPS)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEBUG_READ32_UNHANDLED(PSTRAPS);
		break;
	}

	DEVICE_READ32_END(PSTRAPS);
}

DEVICE_WRITE32(PSTRAPS)
{
	switch (addr) {
	default:
		DEBUG_WRITE32_UNHANDLED(PSTRAPS);
		break;
	}

	DEVICE_WRITE32_END(PSTRAPS);
}


DEVICE_READ32(PGRAPH)
{
	DEVICE_READ32_SWITCH() {
	case NV_PGRAPH_INTR:
		result = pgraph.pending_interrupts;
		break;
	case NV_PGRAPH_INTR_EN:
		result = pgraph.enabled_interrupts;
		break;
	case NV_PGRAPH_NSOURCE:
		result = pgraph.notify_source;
		break;
	case NV_PGRAPH_CTX_USER:
		SET_MASK(result, NV_PGRAPH_CTX_USER_CHANNEL_3D,
			pgraph.context[pgraph.channel_id].channel_3d);
		SET_MASK(result, NV_PGRAPH_CTX_USER_CHANNEL_3D_VALID, 1);
		SET_MASK(result, NV_PGRAPH_CTX_USER_SUBCH,
			pgraph.context[pgraph.channel_id].subchannel << 13);
		SET_MASK(result, NV_PGRAPH_CTX_USER_CHID, pgraph.channel_id);
		break;
	case NV_PGRAPH_TRAPPED_ADDR:
		SET_MASK(result, NV_PGRAPH_TRAPPED_ADDR_CHID, pgraph.trapped_channel_id);
		SET_MASK(result, NV_PGRAPH_TRAPPED_ADDR_SUBCH, pgraph.trapped_subchannel);
		SET_MASK(result, NV_PGRAPH_TRAPPED_ADDR_MTHD, pgraph.trapped_method);
		break;
	case NV_PGRAPH_TRAPPED_DATA_LOW:
		result = pgraph.trapped_data[0];
		break;
	case NV_PGRAPH_FIFO:
		SET_MASK(result, NV_PGRAPH_FIFO_ACCESS, pgraph.fifo_access);
		break;
	case NV_PGRAPH_CHANNEL_CTX_TABLE:
		result = pgraph.context_table >> 4;
		break;
	case NV_PGRAPH_CHANNEL_CTX_POINTER:
		result = pgraph.context_address >> 4;
		break;
	default:
		DEVICE_READ32_REG(pgraph); // Was : DEBUG_READ32_UNHANDLED(PGRAPH);
	}

	DEVICE_READ32_END(PGRAPH);
}	

DEVICE_WRITE32(PGRAPH)
{
	switch (addr) {
	case NV_PGRAPH_INTR:
		pgraph.pending_interrupts &= ~value;
		break;
	case NV_PGRAPH_INTR_EN:
		pgraph.enabled_interrupts = value;
		break;
	case NV_PGRAPH_CTX_CONTROL:
		pgraph.channel_valid = (value & NV_PGRAPH_CTX_CONTROL_CHID);
		break;
	case NV_PGRAPH_FIFO:
		pgraph.fifo_access = GET_MASK(value, NV_PGRAPH_FIFO_ACCESS);
		break;
	case NV_PGRAPH_CHANNEL_CTX_TABLE:
		pgraph.context_table =
			(value & NV_PGRAPH_CHANNEL_CTX_TABLE_INST) << 4;
		break;
	case NV_PGRAPH_CHANNEL_CTX_POINTER:
		pgraph.context_address =
			(value & NV_PGRAPH_CHANNEL_CTX_POINTER_INST) << 4;
		break;
	default: 
		DEVICE_WRITE32_REG(pgraph); // Was : DEBUG_WRITE32_UNHANDLED(PGRAPH);
		break;
	}

	DEVICE_WRITE32_END(PGRAPH);
}


DEVICE_READ32(PCRTC)
{
	DEVICE_READ32_SWITCH() {

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
		DEVICE_READ32_REG(pcrtc); // Was : DEBUG_READ32_UNHANDLED(PCRTC);
		break;
	}

	DEVICE_READ32_END(PCRTC);
}

DEVICE_WRITE32(PCRTC)
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
	case NV_PCRTC_START:
		pcrtc.start = value &= 0x07FFFFFF;
		break;

	default: 
		DEVICE_WRITE32_REG(pcrtc); // Was : DEBUG_WRITE32_UNHANDLED(PCRTC);
		break;
	}

	DEVICE_WRITE32_END(PCRTC);
}


DEVICE_READ32(PRMCIO)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEBUG_READ32_UNHANDLED(PRMCIO);
		break;
	}

	DEVICE_READ32_END(PRMCIO);
}

DEVICE_WRITE32(PRMCIO)
{
	switch (addr) {
	default:
		DEBUG_WRITE32_UNHANDLED(PRMCIO); // TODO : DEVICE_WRITE32_REG(prmcio);
		break;
	}

	DEVICE_WRITE32_END(PRMCIO);
}


DEVICE_READ32(PRAMDAC)
{
	DEVICE_READ32_SWITCH() {

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
		DEVICE_READ32_REG(pramdac); // Was : DEBUG_READ32_UNHANDLED(PRAMDAC);
		break;
	}

	DEVICE_READ32_END(PRAMDAC);
}

DEVICE_WRITE32(PRAMDAC)
{
	switch (addr) {

	uint32_t m, n, p;
	case NV_PRAMDAC_NVPLL_COEFF:
		pramdac.core_clock_coeff = value;

		m = value & NV_PRAMDAC_NVPLL_COEFF_MDIV;
		n = (value & NV_PRAMDAC_NVPLL_COEFF_NDIV) >> 8;
		p = (value & NV_PRAMDAC_NVPLL_COEFF_PDIV) >> 16;

		if (m == 0) {
			pramdac.core_clock_freq = 0;
		}
		else {
			pramdac.core_clock_freq = (NV2A_CRYSTAL_FREQ * n)
				/ (1 << p) / m;
		}

		break;
	case NV_PRAMDAC_MPLL_COEFF:
		pramdac.memory_clock_coeff = value;
		break;
	case NV_PRAMDAC_VPLL_COEFF:
		pramdac.video_clock_coeff = value;
		break;

	default: 
		DEVICE_WRITE32_REG(pramdac); // Was : DEBUG_WRITE32_UNHANDLED(PRAMDAC);
		break;
	}

	DEVICE_WRITE32_END(PRAMDAC);
}


DEVICE_READ32(PRMDIO)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEBUG_READ32_UNHANDLED(PRMDIO);
		break;
	}

	DEVICE_READ32_END(PRMDIO);
}

DEVICE_WRITE32(PRMDIO)
{
	switch (addr) {
	default:
		DEBUG_WRITE32_UNHANDLED(PRMDIO);
		break;
	}

	DEVICE_WRITE32_END(PRMDIO);
}


DEVICE_READ32(PRAMIN)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEVICE_READ32_REG(pramin);
		break;
	}

	DEVICE_READ32_END(PRAMIN);
}

DEVICE_WRITE32(PRAMIN)
{
	switch (addr) {
	default:
		DEVICE_WRITE32_REG(pramin);
		break;
	}

	DEVICE_WRITE32_END(PRAMIN);
}


DEVICE_READ32(USER)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEBUG_READ32_UNHANDLED(USER);
		break;
	}

	DEVICE_READ32_END(USER);
}

DEVICE_WRITE32(USER)
{
	switch (addr) {
	default:
		DEBUG_WRITE32_UNHANDLED(USER);
		break;
	}

	DEVICE_WRITE32_END(USER);
}


typedef struct NV2ABlockInfo {
		uint32_t offset;
		uint32_t size;
		uint32_t(*read)(xbaddr addr);
		void(*write)(xbaddr addr, uint32_t value);
} NV2ABlockInfo;

static const NV2ABlockInfo regions[] = {{
		/* card master control */
		NV_PMC_ADDR, // = 0x000000
		NV_PMC_SIZE, // = 0x001000
		EmuNV2A_PMC_Read32,
		EmuNV2A_PMC_Write32,
	}, {
		/* bus control */
		NV_PBUS_ADDR, // = 0x001000
		NV_PBUS_SIZE, // = 0x001000
		EmuNV2A_PBUS_Read32,
		EmuNV2A_PBUS_Write32,
	}, {
		/* MMIO and DMA FIFO submission to PGRAPH and VPE */
		NV_PFIFO_ADDR, // = 0x002000
		NV_PFIFO_SIZE, // = 0x002000
		EmuNV2A_PFIFO_Read32,
		EmuNV2A_PFIFO_Write32,
	}, {
		/* access to BAR0/BAR1 from real mode */
		NV_PRMA_ADDR, // = 0x007000
		NV_PRMA_SIZE, // = 0x001000
		EmuNV2A_PRMA_Read32,
		EmuNV2A_PRMA_Write32,
	}, {
		/* video overlay */
		NV_PVIDEO_ADDR, // = 0x008000
		NV_PVIDEO_SIZE, // = 0x001000
		EmuNV2A_PVIDEO_Read32,
		EmuNV2A_PVIDEO_Write32,
	}, {
		/* time measurement and time-based alarms */
		NV_PTIMER_ADDR, // = 0x009000
		NV_PTIMER_SIZE, // = 0x001000
		EmuNV2A_PTIMER_Read32,
		EmuNV2A_PTIMER_Write32,
	}, {
		/* performance monitoring counters */
		NV_PCOUNTER_ADDR, // = 0x00a000
		NV_PCOUNTER_SIZE, // = 0x001000
		EmuNV2A_PCOUNTER_Read32,
		EmuNV2A_PCOUNTER_Write32,
	}, {
		/* MPEG2 decoding engine */
		NV_PVPE_ADDR, // = 0x00b000
		NV_PVPE_SIZE, // = 0x001000
		EmuNV2A_PVPE_Read32,
		EmuNV2A_PVPE_Write32,
	},	{
		/* TV encoder */
		NV_PTV_ADDR, // = 0x00d000
		NV_PTV_SIZE, // = 0x001000
		EmuNV2A_PTV_Read32,
		EmuNV2A_PTV_Write32,
	}, {
		/* aliases VGA memory window */
		NV_PRMFB_ADDR, // = 0x0a0000
		NV_PRMFB_SIZE, // = 0x020000
		EmuNV2A_PRMFB_Read32,
		EmuNV2A_PRMFB_Write32,
	}, {
		/* aliases VGA sequencer and graphics controller registers */
		NV_PRMVIO_ADDR, // = 0x0c0000
		NV_PRMVIO_SIZE, // = 0x001000
		EmuNV2A_PRMVIO_Read32,
		EmuNV2A_PRMVIO_Write32,
	},{
		/* memory interface */
		NV_PFB_ADDR, // = 0x100000
		NV_PFB_SIZE, // = 0x001000
		EmuNV2A_PFB_Read32,
		EmuNV2A_PFB_Write32,
	}, {
		/* straps readout / override */
		NV_PSTRAPS_ADDR, // = 0x101000
		NV_PSTRAPS_SIZE, // = 0x001000
		EmuNV2A_PSTRAPS_Read32,
		EmuNV2A_PSTRAPS_Write32,
	}, {
		/* accelerated 2d/3d drawing engine */
		NV_PGRAPH_ADDR, // = 0x400000
		NV_PGRAPH_SIZE, // = 0x002000
		EmuNV2A_PGRAPH_Read32,
		EmuNV2A_PGRAPH_Write32,
	}, {
		/* more CRTC controls */
		NV_PCRTC_ADDR, // = 0x600000
		NV_PCRTC_SIZE, // = 0x001000
		EmuNV2A_PCRTC_Read32,
		EmuNV2A_PCRTC_Write32,
	}, {
		/* aliases VGA CRTC and attribute controller registers */
		NV_PRMCIO_ADDR, // = 0x601000
		NV_PRMCIO_SIZE, // = 0x001000
		EmuNV2A_PRMCIO_Read32,
		EmuNV2A_PRMCIO_Write32,
	}, {
		/* RAMDAC, cursor, and PLL control */
		NV_PRAMDAC_ADDR, // = 0x680000
		NV_PRAMDAC_SIZE, // = 0x001000
		EmuNV2A_PRAMDAC_Read32,
		EmuNV2A_PRAMDAC_Write32,
	}, {
		/* aliases VGA palette registers */
		NV_PRMDIO_ADDR, // = 0x681000
		NV_PRMDIO_SIZE, // = 0x001000
		EmuNV2A_PRMDIO_Read32,
		EmuNV2A_PRMDIO_Write32,
	}, {
		/* RAMIN access */
		NV_PRAMIN_ADDR, // = 0x700000
		NV_PRAMIN_SIZE, // = 0x100000
		EmuNV2A_PRAMIN_Read32,
		EmuNV2A_PRAMIN_Write32,
	},{
		/* PFIFO MMIO and DMA submission area */
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

const NV2ABlockInfo* EmuNV2A_Block(xbaddr addr) 
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

uint32_t EmuNV2A_Read(xbaddr addr, int size)
{
	const NV2ABlockInfo* block = EmuNV2A_Block(addr);

	if (block != nullptr) {
		switch (size) {
			case 8:
				return block->read(addr - block->offset) & 0xFF;
			case 16:
				return block->read(addr - block->offset) & 0xFFFF;
			case 32:
				return block->read(addr - block->offset);
			default:
				EmuWarning("EmuNV2A_Read: Invalid read size: %d", size);
				return 0;
		}
	}

	EmuWarning("EmuNV2A_Read%d: Unhandled Read Address %08X", size, addr);
	return 0;
}

void EmuNV2A_Write(xbaddr addr, uint32_t value, int size)
{
	const NV2ABlockInfo* block = EmuNV2A_Block(addr);

	if (block != nullptr) {
		int shift = 0;
		xbaddr aligned_addr = 0;
		uint32_t aligned_value = 0;
		uint32_t mask = 0;
		switch (size) {
			case 8:
				shift = (addr & 3) * 8;
				aligned_addr = addr & ~3;
				aligned_value = block->read(aligned_addr - block->offset);
				mask = 0xFF << shift;

				// TODO : Must the second byte be written to the next DWORD?		
				block->write(aligned_addr - block->offset, (aligned_value & ~mask) | (value << shift));
				return;
			case 16:
				assert((addr & 1) == 0);
				
				shift = (addr & 2) * 16;
				aligned_addr = addr & ~3;
				aligned_value = block->read(addr - block->offset);
				 mask = 0xFFFF << shift;
				
				// TODO : Must the second byte be written to the next DWORD?		
				block->write(aligned_addr - block->offset, (aligned_value & ~mask) | (value << shift));
				return;
			case 32:
				block->write(addr - block->offset, value);
				return;
			default:
				EmuWarning("EmuNV2A_Read: Invalid read size: %d", size);
				return;
		}
	}

	EmuWarning("EmuNV2A_Write%d: Unhandled Write Address %08X (value %08X)", size, addr, value);
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


	//glutInit();
	{ // rb_init_context();
	  /* link in gl functions at runtime */
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (err != GLEW_OK) {
			EmuWarning("GLEW initialization failed: %s", glewGetErrorString(err));
			return;
		}
	}

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

	// TODO: EmuNV2A_Init
	pcrtc.start = 0;

	pramdac.core_clock_coeff = 0x00011c01; /* 189MHz...? */
	pramdac.core_clock_freq = 189000000;
	pramdac.memory_clock_coeff = 0;
	pramdac.video_clock_coeff = 0x0003C20D; /* 25182Khz...? */
}
