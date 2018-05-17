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
// *   Cxbx->CxbxKrnl->Timer.h
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
// *  (c) 2018      ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

#ifndef TIMER_H
#define TIMER_H

#include <atomic>

/* typedef of the timer object and the callback function*/
typedef void(*pTimerCB)(void*);
typedef struct _TimerObject
{
	int Type;                            // timer type (virtual or real)
	std::atomic_uint64_t ExpireTime_MS;  // when the timer expires (ms)
	std::atomic_bool Exit;               // indicates that the timer should be destroyed
	pTimerCB Callback;                   // function to call when the timer expires
	void* Opaque;                        // opaque argument to pass to the callback
	unsigned int SlowdownFactor;         // how much the time is slowed down (virtual clocks only)
}
TimerObject;


/* Timer exported functions */
TimerObject* Timer_Create(pTimerCB Callback, void* Arg, unsigned int Factor);
void Timer_Start(TimerObject* Timer, uint64_t Expire_MS);
void Timer_Exit(TimerObject* Timer);
void Timer_ChangeExpireTime(TimerObject* Timer, uint64_t Expire_ms);
void Timer_Init();

#endif
