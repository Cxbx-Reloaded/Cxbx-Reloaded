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
// *  https://github.com/xqemu/xqemu/blob/master/hw/xbox/nv2a/nv2a_pramdac.c
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

DEVICE_READ32(PRAMDAC)
{
	DEVICE_READ32_SWITCH() {

	case NV_PRAMDAC_NVPLL_COEFF:
		result = d->pramdac.core_clock_coeff;
		break;
	case NV_PRAMDAC_MPLL_COEFF:
		result = d->pramdac.memory_clock_coeff;
		break;
	case NV_PRAMDAC_VPLL_COEFF:
		result = d->pramdac.video_clock_coeff;
		break;
	case NV_PRAMDAC_PLL_TEST_COUNTER:
		/* emulated PLLs locked instantly? */
		result = NV_PRAMDAC_PLL_TEST_COUNTER_VPLL2_LOCK
			| NV_PRAMDAC_PLL_TEST_COUNTER_NVPLL_LOCK
			| NV_PRAMDAC_PLL_TEST_COUNTER_MPLL_LOCK
			| NV_PRAMDAC_PLL_TEST_COUNTER_VPLL_LOCK;
		break;

	default: 
		//DEVICE_READ32_REG(pramdac); // Was : DEBUG_READ32_UNHANDLED(PRAMDAC);
		break;
	}

    /* Surprisingly, QEMU doesn't handle unaligned access for you properly */
    // result >>= 32 - 8 * size - 8 * (addr & 3);
	
	DEVICE_READ32_END(PRAMDAC);
}

DEVICE_WRITE32(PRAMDAC)
{
	switch (addr) {

	uint32_t m, n, p;
	case NV_PRAMDAC_NVPLL_COEFF:
		d->pramdac.core_clock_coeff = value;

		m = value & NV_PRAMDAC_NVPLL_COEFF_MDIV;
		n = (value & NV_PRAMDAC_NVPLL_COEFF_NDIV) >> 8;
		p = (value & NV_PRAMDAC_NVPLL_COEFF_PDIV) >> 16;

		if (m == 0) {
			d->pramdac.core_clock_freq = 0;
		} else {
			d->pramdac.core_clock_freq = (NV2A_CRYSTAL_FREQ * n)
				/ (1 << p) / m;
			d->ptimer_period = ((uint64_t(d->ptimer.alarm_time >> 5) * SCALE_S_IN_US) / d->pramdac.core_clock_freq);
		}

		break;
	case NV_PRAMDAC_MPLL_COEFF:
		d->pramdac.memory_clock_coeff = value;
		break;
	case NV_PRAMDAC_VPLL_COEFF:
		d->pramdac.video_clock_coeff = value;
		break;

	default: 
		DEVICE_WRITE32_REG(pramdac); // Was : DEBUG_WRITE32_UNHANDLED(PRAMDAC);
		break;
	}

	DEVICE_WRITE32_END(PRAMDAC);
}
