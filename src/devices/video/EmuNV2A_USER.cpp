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
// *   src->devices->video->EmuNV2A_USER.cpp
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
// *  nv2a.cpp is heavily based on code from XQEMU
// *  Copyright(c) 2012 espes
// *  Copyright(c) 2015 Jannik Vogel
// *  https://github.com/espes/xqemu/blob/xbox/hw/xbox/nv2a.c
// *  (c) 2017-2018 Luke Usher <luke.usher@outlook.com>
// *  (c) 2018 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************

/* USER - PFIFO MMIO and DMA submission area */
DEVICE_READ32(USER)
{
	unsigned int channel_id = addr >> 16;
	assert(channel_id < NV2A_NUM_CHANNELS);

	ChannelControl *control = &d->user.channel_control[channel_id];

	uint32_t channel_modes = d->pfifo.regs[NV_PFIFO_MODE];

	/* PIO Mode */
	if (!channel_modes & (1 << channel_id)) {
		assert(false);
	}

	/* DMA Mode */
	addr &= 0xFFFF;
	DEVICE_READ32_SWITCH() {
		case NV_USER_DMA_PUT:
			result = control->dma_put;
			break;
		case NV_USER_DMA_GET:
			result = control->dma_get;
			break;
		case NV_USER_REF:
			result = control->ref;
			break;
		default:
			DEBUG_READ32_UNHANDLED(USER);
			break;
	}

	DEVICE_READ32_END(USER);
}

DEVICE_WRITE32(USER)
{
	unsigned int channel_id = addr >> 16;
	assert(channel_id < NV2A_NUM_CHANNELS);

	ChannelControl *control = &d->user.channel_control[channel_id];

	uint32_t channel_modes = d->pfifo.regs[NV_PFIFO_MODE];
	if (channel_modes & (1 << channel_id)) {
		/* DMA Mode */
		switch (addr & 0xFFFF) {
		case NV_USER_DMA_PUT:
			control->dma_put = value;

			if (d->pfifo.cache1.push_enabled) {
				pfifo_run_pusher(d);
			}
			break;
		case NV_USER_DMA_GET:
			control->dma_get = value;
			break;
		case NV_USER_REF:
			control->ref = value;
			break;
		default:
			DEBUG_WRITE32_UNHANDLED(USER);
			break;
		}
	} else {
		/* PIO Mode */
		assert(false);
	}

	DEVICE_WRITE32_END(USER);
}
