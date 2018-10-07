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
// *   src->devices->video->EmuNV2A_PCRTC.cpp
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
// *  https://github.com/xqemu/xqemu/tree/master/hw/xbox/nv2a/nv2a_pcrtc.c
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

DEVICE_READ32(PCRTC)
{
	DEVICE_READ32_SWITCH() {

	case NV_PCRTC_INTR_0:
		result = d->pcrtc.pending_interrupts;
		break;
	case NV_PCRTC_INTR_EN_0:
		result = d->pcrtc.enabled_interrupts;
		break;
	case NV_PCRTC_START:
		result = d->pcrtc.start;
		break;
	default: 
		result = 0;
		//DEVICE_READ32_REG(pcrtc); // Was : DEBUG_READ32_UNHANDLED(PCRTC);
		break;
	}

	DEVICE_READ32_END(PCRTC);
}

DEVICE_WRITE32(PCRTC)
{
	switch (addr) {

	case NV_PCRTC_INTR_0:
		d->pcrtc.pending_interrupts &= ~value;
		update_irq(d);
		break;
	case NV_PCRTC_INTR_EN_0:
		d->pcrtc.enabled_interrupts = value;
		update_irq(d);
		break;
	case NV_PCRTC_START:
        value &= 0x07FFFFFF;
        // assert(val < memory_region_size(d->vram));
		d->pcrtc.start = value;

        NV2A_DPRINTF("PCRTC_START - %x %x %x %x\n",
                d->vram_ptr[value+64], d->vram_ptr[value+64+1],
                d->vram_ptr[value+64+2], d->vram_ptr[value+64+3]);
		break;

	default: 
		DEVICE_WRITE32_REG(pcrtc); // Was : DEBUG_WRITE32_UNHANDLED(PCRTC);
		break;
	}

	DEVICE_WRITE32_END(PCRTC);
}
