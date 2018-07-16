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
// *   src->devices->video->EmuNV2A_PTIMER.cpp
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

#include "CxbxCommon.h"

/* PTIMER - time measurement and time-based alarms */
static uint64_t ptimer_get_clock(NV2AState * d)
{
	// Get time in nanoseconds
    uint64_t time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	
	return Muldiv64(time,
					uint32_t(d->pramdac.core_clock_freq * d->ptimer.numerator),
					CLOCKS_PER_SEC * d->ptimer.denominator);
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
		break;
	default: 
		//DEVICE_WRITE32_REG(ptimer); // Was : DEBUG_WRITE32_UNHANDLED(PTIMER);
		break;
	}

	DEVICE_WRITE32_END(PTIMER);
}
