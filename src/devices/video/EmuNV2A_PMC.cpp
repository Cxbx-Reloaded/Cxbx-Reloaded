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
// *  This file is heavily based on code from XQEMU
// *  https://github.com/xqemu/xqemu/blob/master/hw/xbox/nv2a/nv2a_pmc.c
// *  Copyright (c) 2012 espes
// *  Copyright (c) 2015 Jannik Vogel
// *  Copyright (c) 2018 Matt Borgerson
// *
// *  Contributions for Cxbx-Reloaded
// *  Copyright (c) 2017-2018 Luke Usher <luke.usher@outlook.com>
// *  Copyright (c) 2018 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************

/* PMC - card master control */
DEVICE_READ32(PMC)
{
	DEVICE_READ32_SWITCH() {
	case NV_PMC_BOOT_0:	// chipset and stepping: NV2A, A02, Rev 0
		result = 0x02A000A2;
		break;
	case NV_PMC_BOOT_1:	// Selects big/little endian mode for the card
		result = 0; // When read, returns 0 if in little-endian mode, 0x01000001 if in big-endian mode.
		break;
	case NV_PMC_INTR_0: // Shows which functional units have pending IRQ
		result = d->pmc.pending_interrupts;
		break;
	case NV_PMC_INTR_EN_0: // Selects which functional units can cause IRQs
		result = d->pmc.enabled_interrupts;
		break;
	default:
		result = 0;
		//DEVICE_READ32_REG(PMC); // Was : DEBUG_READ32_UNHANDLED(PMC);
		break;
	}

	DEVICE_READ32_END(PMC);
}

DEVICE_WRITE32(PMC)
{
	switch(addr) {
	case NV_PMC_INTR_0:
        /* the bits of the interrupts to clear are wrtten */
		d->pmc.pending_interrupts &= ~value;
		update_irq(d);
		break;
	case NV_PMC_INTR_EN_0:
		d->pmc.enabled_interrupts = value;
		update_irq(d);
		break;

	default: 
		//DEVICE_WRITE32_REG(pmc); // Was : DEBUG_WRITE32_UNHANDLED(PMC);
		break;
	}

	DEVICE_WRITE32_END(PMC);
}


