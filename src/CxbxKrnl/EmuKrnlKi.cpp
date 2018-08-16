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
// *   Cxbx->src->CxbxKrnl->EmuKrnlKi.cpp
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
// *  (c) 2016 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#define _XBOXKRNL_DEFEXTRN_

#define LOG_PREFIX CXBXR_MODULE::KI

// prevent name collisions
namespace xboxkrnl
{
#include <xboxkrnl/xboxkrnl.h> // For KeBugCheck, etc.
};

#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnlLogging.h"

//#include "EmuKrnl.h" // For InitializeListHead(), etc.

xboxkrnl::BOOLEAN KiInsertTimerTable(
	IN xboxkrnl::LARGE_INTEGER Interval,
	xboxkrnl::ULONGLONG,
	IN xboxkrnl::PKTIMER Timer
)
{
	// TODO
	return TRUE;
}

xboxkrnl::BOOLEAN KiInsertTreeTimer(
	IN xboxkrnl::PKTIMER Timer,
	IN xboxkrnl::LARGE_INTEGER Interval
)
{
	// Is the given time absolute (indicated by a positive number)?
	if (Interval.u.HighPart >= 0) {
		// Convert absolute time to a time relative to the system time :
		xboxkrnl::LARGE_INTEGER SystemTime;
		xboxkrnl::KeQuerySystemTime(&SystemTime);
		Interval.QuadPart = SystemTime.QuadPart - Interval.QuadPart;
		if (Interval.u.HighPart >= 0) {
			// If the relative time is already passed, return without inserting :
			Timer->Header.Inserted = FALSE;
			Timer->Header.SignalState = TRUE;
			return FALSE;
		}

		Timer->Header.Absolute = TRUE;
	}
	else
		// Negative intervals are relative, not absolute :
		Timer->Header.Absolute = FALSE;

	if (Timer->Period == 0)
		Timer->Header.SignalState = FALSE;

	Timer->Header.Inserted = TRUE;
	return KiInsertTimerTable(Interval, xboxkrnl::KeQueryInterruptTime(), Timer);
}
