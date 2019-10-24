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

#ifdef _WIN32
#include <windows.h>
#endif
#include <thread>
#include <vector>
#include <mutex>
#include "Timer.h"
#include "common\util\CxbxUtil.h"
#include "core\kernel\init\CxbxKrnl.h"
#ifdef __linux__
#include <time.h>
#endif


// Virtual clocks will probably become useful once LLE CPU is implemented, but for now we don't need them.
// See the QEMUClockType QEMU_CLOCK_VIRTUAL of XQEMU for more info.
#define CLOCK_REALTIME 0
//#define CLOCK_VIRTUALTIME  1


// Vector storing all the timers created
static std::vector<TimerObject*> TimerList;
// The frequency of the high resolution clock of the host
uint64_t HostClockFrequency;
// Lock to acquire when accessing TimerList
std::mutex TimerMtx;


// Returns the current time of the timer
inline uint64_t GetTime_NS(TimerObject* Timer)
{
#ifdef _WIN32
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	uint64_t Ret = Muldiv64(li.QuadPart, SCALE_S_IN_NS, (uint32_t)HostClockFrequency);
#elif __linux__
	static struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	uint64_t Ret = Muldiv64(ts.tv_sec, SCALE_S_IN_NS, 1) + ts.tv_nsec;
#else
#error "Unsupported OS"
#endif
	return Ret;
}

// Calculates the next expire time of the timer
static inline uint64_t GetNextExpireTime(TimerObject* Timer)
{
	return GetTime_NS(Timer) + Timer->ExpireTime_MS.load();
}

// Deallocates the memory of the timer
void Timer_Destroy(TimerObject* Timer)
{
	unsigned int index, i;
	std::lock_guard<std::mutex>lock(TimerMtx);
	
	index = TimerList.size();
	for (i = 0; i < index; i++) {
		if (Timer == TimerList[i]) {
			index = i;
		}
	}

	assert(index != TimerList.size());
	delete Timer;
	TimerList.erase(TimerList.begin() + index);
}

// Thread that runs the timer
void ClockThread(TimerObject* Timer)
{
	uint64_t NewExpireTime;

	if (!Timer->Name.empty()) {
		CxbxSetThreadName(Timer->Name.c_str());
	}
	if (Timer->CpuAffinity != nullptr) {
		InitXboxThread(*Timer->CpuAffinity);
	}
	NewExpireTime = GetNextExpireTime(Timer);

	while (true) {
		if (GetTime_NS(Timer) > NewExpireTime) {
			if (Timer->Exit.load()) {
				Timer_Destroy(Timer);
				return;
			}
			Timer->Callback(Timer->Opaque);
			NewExpireTime = GetNextExpireTime(Timer);
		}
		Sleep(1); // prevent burning the cpu
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
TimerObject* Timer_Create(TimerCB Callback, void* Arg, std::string Name, unsigned long* Affinity)
{
	std::lock_guard<std::mutex>lock(TimerMtx);
	TimerObject* pTimer = new TimerObject;
	pTimer->Type = CLOCK_REALTIME;
	pTimer->Callback = Callback;
	pTimer->ExpireTime_MS.store(0);
	pTimer->Exit.store(false);
	pTimer->Opaque = Arg;
	Name.empty() ? pTimer->Name = "Unnamed thread" : pTimer->Name = Name;
	pTimer->CpuAffinity = Affinity;
	TimerList.emplace_back(pTimer);

	return pTimer;
}

// Starts the timer
// Expire_MS must be expressed in NS
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
	HostClockFrequency = freq.QuadPart;
#elif __linux__
	ClockFrequency = 0;
#else
#error "Unsupported OS"
#endif
}
