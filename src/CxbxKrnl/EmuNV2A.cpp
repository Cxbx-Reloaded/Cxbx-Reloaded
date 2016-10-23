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

uint32_t EmuNV2A_PMC_Read32(uint32_t addr)
{
	switch (addr) {
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
		return 0x0; // NV_PBUS_PCI_NV_2_REVISION_ID ??
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
	default:
		EmuWarning("EmuNV2A_PVIDEO_Read32: Unknown Read Address %08X", addr);
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
	switch (addr) {
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
		EmuWarning("EmuNV2A_PFB_Write32: Unknown Write Address %08X (value %08X)", addr, value);
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
	case NV_PBUS:  /* bus control */
		EmuNV2A_PBUS_Write32(addr & 0x0FFF, value);
	case NV_PFIFO:  /* MMIO and DMA FIFO submission to PGRAPH and VPE */
		EmuNV2A_PFIFO_Write32(addr & 0x0FFF, value);
	case NV_PFIFO_CACHE:
		EmuNV2A_PFIFO_CACHE_Write32(addr & 0x0FFF, value);
	case NV_PRMA:  /* access to BAR0/BAR1 from real mode */
		EmuNV2A_PRMA_Write32(addr & 0x0FFF, value);
	case NV_PVIDEO:  /* video overlay */
		EmuNV2A_PVIDEO_Write32(addr & 0x0FFF, value);
	case NV_PTIMER:  /* time measurement and time-based alarms */
		EmuNV2A_PTIMER_Write32(addr & 0x0FFF, value);
	case NV_PCOUNTER:  /* performance monitoring counters */
		EmuNV2A_PCOUNTER_Write32(addr & 0x0FFF, value);
	case NV_PVPE:  /* MPEG2 decoding engine */
		EmuNV2A_PVPE_Write32(addr & 0x0FFF, value);
	case NV_PTV:  /* TV encoder */
		EmuNV2A_PTV_Write32(addr & 0x0FFF, value);
	case NV_PRMFB:  /* aliases VGA memory window */
		EmuNV2A_PRMFB_Write32(addr & 0x0FFF, value);
	case NV_PRMVIO:  /* aliases VGA sequencer and graphics controller registers */
		EmuNV2A_PRMVIO_Write32(addr & 0x0FFF, value);
	case NV_PFB:  /* memory interface */
		EmuNV2A_PFB_Write32(addr & 0x0FFF, value);
	case NV_PSTRAPS:  /* straps readout / override */
		EmuNV2A_PSTRAPS_Write32(addr & 0x0FFF, value);
	case NV_PGRAPH:  /* accelerated 2d/3d drawing engine */
		EmuNV2A_PGRAPH_Write32(addr & 0x0FFF, value);
	case NV_PCRTC:  /* more CRTC controls */
		EmuNV2A_PCRTC_Write32(addr & 0x0FFF, value);
	case NV_PRMCIO:  /* aliases VGA CRTC and attribute controller registers */
		EmuNV2A_PRMCIO_Write32(addr & 0x0FFF, value);
	case NV_PRAMDAC:  /* RAMDAC, cursor, and PLL control */
		EmuNV2A_PRAMDAC_Write32(addr & 0x0FFF, value);
	case NV_PRMDIO:  /* aliases VGA palette registers */
		EmuNV2A_PRMDIO_Write32(addr & 0x0FFF, value);
	case NV_PRAMIN:  /* RAMIN access */
		EmuNV2A_PRAMIN_Write32(addr & 0x0FFF, value);
	case NV_USER:  /* PFIFO MMIO and DMA submission area */
		EmuNV2A_USER_Write32(addr & 0x0FFF, value);
	default:
		EmuWarning("EmuNV2A_Write32: Unknown Write Address %08X (value %08X)", addr, value);
	}
}
