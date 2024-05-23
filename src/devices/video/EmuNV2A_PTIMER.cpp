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
// *  https://github.com/xqemu/xqemu/blob/master/hw/xbox/nv2a/nv2a_ptimer.c
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

#include "common\util\CxbxUtil.h"


/* PTIMER - time measurement and time-based alarms */
static uint64_t ptimer_get_clock(NV2AState *d)
{
	return Muldiv64(Muldiv64(get_now(),
					(uint32_t)d->pramdac.core_clock_freq, // TODO : Research how this can be updated to accept uint64_t
					SCALE_S_IN_US), // Was CLOCKS_PER_SEC
				d->ptimer.denominator,
				d->ptimer.numerator);
}

DEVICE_READ32(PTIMER)
{
	DEVICE_READ32_SWITCH() {
	case NV_PTIMER_INTR_0:
		result = d->ptimer.pending_interrupts;
		break;
	case NV_PTIMER_INTR_EN_0:
		result = d->ptimer.enabled_interrupts;
		break;
	case NV_PTIMER_NUMERATOR:
		result = d->ptimer.numerator;
		break;
	case NV_PTIMER_DENOMINATOR:
		result = d->ptimer.denominator;
		break;
	case NV_PTIMER_TIME_0:
		result = (ptimer_get_clock(d) & 0x7ffffff) << 5;
		break;
	case NV_PTIMER_TIME_1:
		result = (ptimer_get_clock(d) >> 27) & 0x1fffffff;
		break;
	default: 
		result = 0;
		//DEVICE_READ32_REG(ptimer); // Was : DEBUG_READ32_UNHANDLED(PTIMER);
		break;
	}

	DEVICE_READ32_END(PTIMER);
}


DEVICE_WRITE32(PTIMER)
{
	switch (addr) {

	case NV_PTIMER_INTR_0:
		d->ptimer.pending_interrupts &= ~value;
		update_irq(d);
		break;
	case NV_PTIMER_INTR_EN_0:
		d->ptimer.enabled_interrupts = value;
		if (d->ptimer.enabled_interrupts & NV_PTIMER_INTR_EN_0_ALARM) {
			d->ptimer_last = get_now();
			d->ptimer_active = true;
		}
		else if ((d->ptimer.enabled_interrupts & NV_PTIMER_INTR_EN_0_ALARM) == 0) {
			d->ptimer_active = false;
		}
		update_irq(d);
		break;
	case NV_PTIMER_DENOMINATOR:
		d->ptimer.denominator = value;
		break;
	case NV_PTIMER_NUMERATOR:
		d->ptimer.numerator = value;
		break;
	case NV_PTIMER_ALARM_0:
		d->ptimer.alarm_time = value;
		d->ptimer_period = ((uint64_t(d->ptimer.alarm_time >> 5) * SCALE_S_IN_US) / d->pramdac.core_clock_freq);
		break;
	default: 
		//DEVICE_WRITE32_REG(ptimer); // Was : DEBUG_WRITE32_UNHANDLED(PTIMER);
		break;
	}

	DEVICE_WRITE32_END(PTIMER);
}
