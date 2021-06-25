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
// *  https://github.com/xqemu/xqemu/blob/master/hw/xbox/nv2a/nv2a_pvideo.c
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

static void pvideo_vga_invalidate(NV2AState *d)
{
    int y1 = GET_MASK(d->pvideo.regs[NV_PVIDEO_POINT_OUT(0)/4],
                      NV_PVIDEO_POINT_OUT_Y);
    int y2 = y1 + GET_MASK(d->pvideo.regs[NV_PVIDEO_SIZE_OUT(0)/4],
                           NV_PVIDEO_SIZE_OUT_HEIGHT);
    NV2A_DPRINTF("pvideo_vga_invalidate %d %d\n", y1, y2);
	// TODO : vga_invalidate_scanlines(&d->vga, y1, y2);

	// TODO: Remove this when the AMD crash is solved in vblank_thread
	if (d->enable_overlay) // to avoid flickering, only update when there's an overlay
		NV2ADevice::UpdateHostDisplay(d);
}

DEVICE_READ32(PVIDEO)
{
	DEVICE_READ32_SWITCH() {

	case NV_PVIDEO_INTR:
		result = d->pvideo.pending_interrupts;
		break;
	case NV_PVIDEO_INTR_EN:
		result = d->pvideo.enabled_interrupts;
		break;
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
	case NV_PVIDEO_INTR:
		d->pvideo.pending_interrupts &= ~value;
		update_irq(d);
//		qemu_cond_broadcast(&d->pvideo.interrupt_cond);
		break;
	case NV_PVIDEO_INTR_EN:
		d->pvideo.enabled_interrupts = value;
		update_irq(d);
		break;
	case NV_PVIDEO_BUFFER:
		d->pvideo.regs[NV_PVIDEO_BUFFER/4] = value;
		d->enable_overlay = (value != 0);
		pvideo_vga_invalidate(d);
		break;
	case NV_PVIDEO_STOP:
		d->pvideo.regs[NV_PVIDEO_BUFFER/4] = 0;
		d->enable_overlay = false;
		pvideo_vga_invalidate(d);
		break;
	default:
		DEVICE_WRITE32_REG(pvideo);
		break;
	}

	DEVICE_WRITE32_END(PVIDEO);
}

