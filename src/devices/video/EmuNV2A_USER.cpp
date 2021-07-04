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
// *  https://github.com/xqemu/xqemu/blob/master/hw/xbox/nv2a/nv2a_user.c
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

/* USER - PFIFO MMIO and DMA submission area */
DEVICE_READ32(USER)
{
	unsigned int channel_id = addr >> 16;
	assert(channel_id < NV2A_NUM_CHANNELS);

	qemu_mutex_lock(&d->pfifo.pfifo_lock);

	uint32_t channel_modes = d->pfifo.regs[NV_PFIFO_MODE/4];

	uint32_t result = 0;
	if (channel_modes & (1 << channel_id)) {
		/* DMA Mode */

		unsigned int cur_channel_id =
			GET_MASK(d->pfifo.regs[NV_PFIFO_CACHE1_PUSH1/4],
				NV_PFIFO_CACHE1_PUSH1_CHID);

		if (channel_id == cur_channel_id) {
			switch(addr & 0xFFFF) { // Was DEVICE_READ32_SWITCH()
				case NV_USER_DMA_PUT:
					result = d->pfifo.regs[NV_PFIFO_CACHE1_DMA_PUT/4];
					break;
				case NV_USER_DMA_GET:
					result = d->pfifo.regs[NV_PFIFO_CACHE1_DMA_GET/4];
					break;
				case NV_USER_REF:
					result = d->pfifo.regs[NV_PFIFO_CACHE1_REF/4];
					break;
				default:
					DEBUG_READ32_UNHANDLED(USER);
					break;
			}
		} else {
			/* ramfc */
			assert(false);
		}
	} else {
		/* PIO Mode */
		assert(false);
	}

	qemu_mutex_unlock(&d->pfifo.pfifo_lock);

	DEVICE_READ32_END(USER);
}

DEVICE_WRITE32(USER)
{
	unsigned int channel_id = addr >> 16;
	assert(channel_id < NV2A_NUM_CHANNELS);

	qemu_mutex_lock(&d->pfifo.pfifo_lock);

	uint32_t channel_modes = d->pfifo.regs[NV_PFIFO_MODE/4];
	if (channel_modes & (1 << channel_id)) {
		/* DMA Mode */
		unsigned int cur_channel_id =
			GET_MASK(d->pfifo.regs[NV_PFIFO_CACHE1_PUSH1/4],
				NV_PFIFO_CACHE1_PUSH1_CHID);

		if (channel_id == cur_channel_id) {
			switch (addr & 0xFFFF) {
			case NV_USER_DMA_PUT:
				d->pfifo.regs[NV_PFIFO_CACHE1_DMA_PUT/4] = value;
				break;
			case NV_USER_DMA_GET:
				d->pfifo.regs[NV_PFIFO_CACHE1_DMA_GET/4] = value;
				break;
			case NV_USER_REF:
				d->pfifo.regs[NV_PFIFO_CACHE1_REF/4] = value;
				break;
			default:
				assert(false);
				break;
			}

            // kick pfifo
            qemu_cond_broadcast(&d->pfifo.pusher_cond);
            //qemu_cond_broadcast(&d->pfifo.puller_cond);
		} else {
			/* ramfc */
			assert(false);
		}
	} else {
		/* PIO Mode */
		assert(false);
	}

    qemu_mutex_unlock(&d->pfifo.pfifo_lock);

	DEVICE_WRITE32_END(USER);
}
