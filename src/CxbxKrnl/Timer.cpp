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
// *   Cxbx->CxbxKrnl->Timer.cpp
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

#ifdef _WIN32
#include <windows.h>
#endif
#include <thread>
#include <vector>
#include "Timer.h"
#include "CxbxCommon.h"
#ifdef __linux__
#include <time.h>
#endif


#define CLOCK_REALTIME 0
#define CLOCK_VIRTUALTIME  1
#define SCALE_S  1000000000ULL
#define SCALE_MS 1000000ULL
#define SCALE_US 1000ULL
#define SCALE_NS 1ULL


// Vector storing all the timers created
static std::vector<TimerObject*> TimerList;
// The frequency of the high resolution clock of the host
static uint64_t ClockFrequency;


// Returns the current time of the timer
inline uint64_t GetTime_NS(TimerObject* Timer)
{
#ifdef _WIN32
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	uint64_t Ret = Muldiv64(li.QuadPart, SCALE_S, ClockFrequency);
#elif __linux__
	static struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	uint64_t Ret = Muldiv64(ts.tv_sec, SCALE_S, 1) + ts.tv_nsec;
#else
#error "Unsupported OS"
#endif
	return Timer->Type == CLOCK_REALTIME ? Ret : Ret / Timer->SlowdownFactor;
}

// Calculates the next expire time of the timer
static inline uint64_t GetNextExpireTime(TimerObject* Timer)
{
	return GetTime_NS(Timer) + Timer->ExpireTime_MS.load() * SCALE_MS;
}

// Deallocates the memory of the timer
void Timer_Destroy(TimerObject* Timer)
{
	unsigned int index;
	for (unsigned int i = 0; i < TimerList.size(); i++) {
		if (Timer == TimerList[i]) {
			index = i;
		}
	}

	delete Timer;
	TimerList.erase(TimerList.begin() + index);
}

// Thread that runs the timer
void ClockThread(TimerObject* Timer)
{
	uint64_t NewExpireTime = GetNextExpireTime(Timer);

	while (true) {
		if (GetTime_NS(Timer) > NewExpireTime) {
			if (Timer->Exit.load()) {
				Timer_Destroy(Timer);
				return;
			}
			Timer->Callback(Timer->Opaque);
			if (Timer->Exit.load()) {
				Timer_Destroy(Timer);
				return;
			}
			NewExpireTime = GetNextExpireTime(Timer);
		}
	}
}

// Changes the expire time of a timer
void Timer_ChangeExpireTime(TimerObject* Timer, uint64_t Expire_ms)
{
	Timer->ExpireTime_MS.store(Expire_ms);
}

// Destroys the timer
void Timer_Exit(TimerObject* Timer)
{
	Timer->Exit.store(true);
}

// Allocates the memory for the timer object
TimerObject* Timer_Create(pTimerCB Callback, void* Arg, unsigned int Factor)
{
	TimerObject* pTimer = new TimerObject;
	pTimer->Type = Factor <= 1 ? CLOCK_REALTIME : CLOCK_VIRTUALTIME;
	pTimer->Callback = Callback;
	pTimer->ExpireTime_MS.store(0);
	pTimer->Exit.store(false);
	pTimer->Opaque = Arg;
	pTimer->SlowdownFactor = Factor < 1 ? 1 : Factor;
	TimerList.emplace_back(pTimer);

	return pTimer;
}

// Starts the timer
void Timer_Start(TimerObject* Timer, uint64_t Expire_MS)
{
	Timer->ExpireTime_MS.store(Expire_MS);
	std::thread(ClockThread, Timer).detach();
}

// Retrives the frequency of the high resolution clock of the host
void Timer_Init()
{
#ifdef _WIN32
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	ClockFrequency = freq.QuadPart;
#elif __linux__
	ClockFrequency = 0;
#else
#error "Unsupported OS"
#endif
}
