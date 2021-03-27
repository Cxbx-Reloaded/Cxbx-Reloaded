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
// *  (c) 2018      ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

#ifndef TIMER_H
#define TIMER_H

#include <atomic>
#include <mutex>

#define SCALE_S_IN_NS  1000000000
#define SCALE_MS_IN_NS 1000000
#define SCALE_US_IN_NS 1000
#define SCALE_NS_IN_NS 1

#define SCALE_S_IN_US  1000000
#define SCALE_MS_IN_US 1000
#define SCALE_US_IN_US 1

/* typedef of the timer object and the callback function */
typedef void(*TimerCB)(void*);
typedef struct _TimerObject
{
	int Type;                            // timer type
	std::atomic_uint64_t ExpireTime_MS;  // when the timer expires (ms)
	std::atomic_bool Exit;               // indicates that the timer should be destroyed
	TimerCB Callback;                    // function to call when the timer expires
	void* Opaque;                        // opaque argument to pass to the callback
	std::string Name;                    // the name of the timer thread (if any)
	bool IsXboxTimer;                    // indicates that the timer should run on the Xbox CPU
}
TimerObject;

extern uint64_t HostClockFrequency;

/* Timer exported functions */
TimerObject* Timer_Create(TimerCB Callback, void* Arg, std::string Name, bool IsXboxTimer);
void Timer_Start(TimerObject* Timer, uint64_t Expire_MS);
void Timer_Exit(TimerObject* Timer);
void Timer_ChangeExpireTime(TimerObject* Timer, uint64_t Expire_ms);
uint64_t GetTime_NS(TimerObject* Timer);
void Timer_Init();

// A stateful replacement for QueryPerformanceCounter, ticking at an arbitrary frequency
// Thread-safe and designed to avoid overflows at all cost
class ScaledPerformanceCounter
{
public:
	ScaledPerformanceCounter() = default;
	void Reset(uint32_t frequency);
	uint64_t Tick();

private:
	std::mutex m_mutex;

	uint64_t m_frequencyFactor;
	int64_t m_lastQPC;

	uint64_t m_currentCount;
	uint64_t m_currentRemainder;
};

#endif
