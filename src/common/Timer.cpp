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
#include "core\kernel\support\EmuFS.h"
#include "core/kernel/exports/EmuKrnlPs.hpp"
#ifdef __linux__
#include <time.h>
#endif

// More precise sleep, but with increased CPU usage
void SleepPrecise(std::chrono::steady_clock::time_point targetTime)
{
	using namespace std::chrono;
	// If we don't need to wait, return right away

	// TODO use waitable timers?
	// TODO fetch the timer resolution to determine the sleep threshold?
	// TODO adaptive wait? https://blat-blatnik.github.io/computerBear/making-accurate-sleep-function/

	// Try to sleep for as much of the wait as we can
	// to save CPU usage / power
	// We expect sleep to overshoot, so give ourselves some extra time
	// Note currently we ask Windows to give us 1ms timer resolution
	constexpr auto sleepThreshold = 2ms; // Minimum remaining time before we attempt to use sleep

	auto sleepFor = (targetTime - sleepThreshold) - steady_clock::now();
	auto sleepMs = duration_cast<milliseconds>(sleepFor).count();

	// Sleep if required
	if (sleepMs >= 0) {
		Sleep((DWORD)sleepMs);
	}

	// Spin wait
	while (steady_clock::now() < targetTime) {
		;
	}
}

// Virtual clocks will probably become useful once LLE CPU is implemented, but for now we don't need them.
// See the QEMUClockType QEMU_CLOCK_VIRTUAL of XQEMU for more info.
#define CLOCK_REALTIME 0
//#define CLOCK_VIRTUALTIME  1


// Vector storing all the timers created
static std::vector<TimerObject*> TimerList;
// The frequency of the high resolution clock of the host, and the start time
int64_t HostQPCFrequency, HostQPCStartTime;
// Lock to acquire when accessing TimerList
std::mutex TimerMtx;


// Returns the current time of the timer
uint64_t GetTime_NS(TimerObject* Timer)
{
#ifdef _WIN32
	uint64_t Ret = Timer_GetScaledPerformanceCounter(SCALE_S_IN_NS);
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

void Timer_Shutdown()
{
	unsigned i, iXboxThreads = 0;
	TimerMtx.lock();

	for (i = 0; i < TimerList.size(); i++) {
		TimerObject* Timer = TimerList[i];
		// We only need to terminate host threads.
		if (!Timer->IsXboxTimer) {
			Timer_Exit(Timer);
		}
		// If the thread is xbox, we need to increment for while statement check
		else {
			iXboxThreads++;
		}
	}

	// Only perform wait for host threads, otherwise xbox threads are
	// already handled within xbox kernel for shutdown process. See CxbxrKrnlSuspendThreads function.
	int counter = 0;
	while (iXboxThreads != TimerList.size()) {
		if (counter >= 8) {
			break;
		}
		TimerMtx.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		TimerMtx.lock();
		counter++;
	}
	TimerList.clear();
	TimerMtx.unlock();
}

// Thread that runs the timer
void NTAPI ClockThread(void *TimerArg)
{
	TimerObject *Timer = static_cast<TimerObject *>(TimerArg);
	if (!Timer->Name.empty()) {
		CxbxSetThreadName(Timer->Name.c_str());
	}
	if (!Timer->IsXboxTimer) {
		g_AffinityPolicy->SetAffinityOther();
	}

	uint64_t NewExpireTime = GetNextExpireTime(Timer);

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
TimerObject* Timer_Create(TimerCB Callback, void* Arg, std::string Name, bool IsXboxTimer)
{
	std::lock_guard<std::mutex>lock(TimerMtx);
	TimerObject* pTimer = new TimerObject;
	pTimer->Type = CLOCK_REALTIME;
	pTimer->Callback = Callback;
	pTimer->ExpireTime_MS.store(0);
	pTimer->Exit.store(false);
	pTimer->Opaque = Arg;
	pTimer->Name = Name.empty() ? "Unnamed thread" : std::move(Name);
	pTimer->IsXboxTimer = IsXboxTimer;
	TimerList.emplace_back(pTimer);

	return pTimer;
}

// Starts the timer
// Expire_MS must be expressed in NS
void Timer_Start(TimerObject* Timer, uint64_t Expire_MS)
{
	Timer->ExpireTime_MS.store(Expire_MS);
	if (Timer->IsXboxTimer) {
		xbox::HANDLE hThread;
		CxbxrCreateThread(&hThread, xbox::zeroptr, ClockThread, Timer, FALSE);
	}
	else {
		std::thread(ClockThread, Timer).detach();
	}
}

// Retrives the frequency of the high resolution clock of the host
void Timer_Init()
{
#ifdef _WIN32
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&HostQPCFrequency));
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&HostQPCStartTime));
#elif __linux__
	ClockFrequency = 0;
#else
#error "Unsupported OS"
#endif
}

int64_t Timer_GetScaledPerformanceCounter(int64_t Period)
{
	LARGE_INTEGER currentQPC;
	QueryPerformanceCounter(&currentQPC);

	// Scale frequency with overflow avoidance, like in std::chrono
	// https://github.com/microsoft/STL/blob/6d2f8b0ed88ea6cba26cc2151f47f678442c1663/stl/inc/chrono#L703
	const int64_t currentTime = currentQPC.QuadPart - HostQPCStartTime;
	const int64_t whole = (currentTime / HostQPCFrequency) * Period;
	const int64_t part  = (currentTime % HostQPCFrequency) * Period / HostQPCFrequency;

	return whole + part;
}

