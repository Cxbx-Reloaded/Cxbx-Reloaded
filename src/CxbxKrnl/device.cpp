// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->Device.cpp
// *
// *  This file is part of the Cxbx project.
// *
// *  Cxbx is free software; you can redistribute it
// *  and/or modify it under the terms of the GNU General Public
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
// *  (c) 2016 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************

// Some symbols, code and ideas in here are based on MAME diexec.* :
// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    diexec.c

    Device execution interfaces.

***************************************************************************/

#include "device.h"

//**************************************************************************
//  DEVICE INPUT
//**************************************************************************

//-------------------------------------------------
//  device_input - constructor
//-------------------------------------------------

device_execute_interface::device_input::device_input()
	: //m_execute(nullptr),
		m_linenum(0),
		m_stored_vector(0),
		m_curvector(0),
		m_curstate(CLEAR_LINE),
		m_qindex(0)
{
//	memset(m_queue, 0, sizeof(m_queue));
}


//-------------------------------------------------
//  start - called by interface_pre_start so we
//  can set ourselves up
//-------------------------------------------------

void device_execute_interface::device_input::start(device_execute_interface *execute, int linenum)
{
//	m_execute = execute;
	m_linenum = linenum;

	reset();
/*
	device_t &device = m_execute->device();
	device.save_item(NAME(m_stored_vector), m_linenum);
	device.save_item(NAME(m_curvector), m_linenum);
	device.save_item(NAME(m_curstate), m_linenum);
*/
}


//-------------------------------------------------
//  reset - reset our input states
//-------------------------------------------------

void device_execute_interface::device_input::reset()
{
	m_curvector = m_stored_vector = 0; // m_execute->default_irq_vector();
	m_qindex = 0;
}

//-------------------------------------------------
//  set_state_synced - enqueue an event for later
//  execution via timer
//-------------------------------------------------

void device_execute_interface::device_input::set_state_synced(int state, int vector)
{
/*
	LOG(("set_state_synced('%s',%d,%d,%02x)\n", m_execute->device().tag(), m_linenum, state, vector));

if (TEMPLOG) printf("setline(%s,%d,%d,%d)\n", m_execute->device().tag(), m_linenum, state, (vector == USE_STORED_VECTOR) ? 0 : vector);
	assert(state == ASSERT_LINE || state == HOLD_LINE || state == CLEAR_LINE || state == PULSE_LINE);
*/
	// treat PULSE_LINE as ASSERT+CLEAR
	if (state == PULSE_LINE)
	{
		// catch errors where people use PULSE_LINE for devices that don't support it
		if (m_linenum != INPUT_LINE_NMI && m_linenum != INPUT_LINE_RESET)
			;// throw emu_fatalerror("device '%s': PULSE_LINE can only be used for NMI and RESET lines\n", m_execute->device().tag());

		set_state_synced(ASSERT_LINE, vector);
		set_state_synced(CLEAR_LINE, vector);
		return;
	}

	// if we're full of events, flush the queue and log a message
	int event_index = m_qindex++;

	if (event_index >= ARRAY_LENGTH(m_queue))
	{
		m_qindex--;
//		empty_event_queue(nullptr,0);
		event_index = m_qindex++;
//		m_execute->device().logerror("Exceeded pending input line event queue on device '%s'!\n", m_execute->device().tag());
	}

	// enqueue the event
	if (event_index < ARRAY_LENGTH(m_queue))
	{
		if (vector == USE_STORED_VECTOR)
			vector = m_stored_vector;
		m_queue[event_index] = (state & 0xff) | (vector << 8);

		// if this is the first one, set the timer
		if (event_index == 0)
;//			m_execute->scheduler().synchronize(timer_expired_delegate(FUNC(device_execute_interface::device_input::empty_event_queue),this), 0, this);
	}
}

