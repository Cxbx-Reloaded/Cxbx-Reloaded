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

#include <core\kernel\exports\xboxkrnl.h>

#include <windows.h>
#include <thread>
#include <vector>
#include <mutex>
#include <array>
#include "Timer.h"
#include "common\util\CxbxUtil.h"
#include "core\kernel\support\EmuFS.h"
#include "core\kernel\exports\EmuKrnlPs.hpp"
#include "core\kernel\exports\EmuKrnl.h"
#include "devices\Xbox.h"
#include "devices\usb\OHCI.h"
#include "core\hle\DSOUND\DirectSound\DirectSoundGlobal.hpp"


static uint64_t last_qpc; // last time when QPC was called
static uint64_t last_time; // starting time point until the next periodic event is due
static uint64_t exec_time; // total execution time in us since the emulation started
static uint64_t pit_last; // last time when the pit time was updated
static uint64_t pit_last_qpc; // last QPC time of the pit
// The frequency of the high resolution clock of the host, and the start time
int64_t HostQPCFrequency, HostQPCStartTime;


void timer_init()
{
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER *>(&HostQPCFrequency));
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER *>(&last_qpc));
	pit_last_qpc = last_qpc;
	pit_last = get_now();

	// Synchronize xbox system time with host time
	LARGE_INTEGER HostSystemTime;
	GetSystemTimeAsFileTime((LPFILETIME)&HostSystemTime);
	xbox::KeSystemTime.High2Time = HostSystemTime.u.HighPart;
	xbox::KeSystemTime.LowPart = HostSystemTime.u.LowPart;
	xbox::KeSystemTime.High1Time = HostSystemTime.u.HighPart;
}

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

// NOTE: the pit device is not implemented right now, so we put this here
static uint64_t pit_next(uint64_t now)
{
	constexpr uint64_t pit_period = 1000;
	uint64_t next = pit_last + pit_period;

	if (now >= next) {
		xbox::KiClockIsr((now - pit_last - pit_period) / 1000);
		pit_last = get_now();
		return pit_period;
	}

	return pit_last + pit_period - now; // time remaining until next clock interrupt
}

static void update_non_periodic_events()
{
	// update dsound
	dsound_worker();

	// check nvnet
	NVNetRecv();

	// check for hw interrupts, but skip the gpu interrupt since that is serviced in vblank_next
	for (int i = 0; i < 3; i++) {
		// If the interrupt is pending and connected, process it
		if (g_bEnableAllInterrupts && HalSystemInterrupts[i].IsPending() && EmuInterruptList[i] && EmuInterruptList[i]->Connected) {
			HalSystemInterrupts[i].Trigger(EmuInterruptList[i]);
		}
	}
	for (int i = 4; i < MAX_BUS_INTERRUPT_LEVEL; i++) {
		// If the interrupt is pending and connected, process it
		if (g_bEnableAllInterrupts && HalSystemInterrupts[i].IsPending() && EmuInterruptList[i] && EmuInterruptList[i]->Connected) {
			HalSystemInterrupts[i].Trigger(EmuInterruptList[i]);
		}
	}
}

uint64_t get_now()
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	uint64_t elapsed_us = now.QuadPart - last_qpc;
	last_qpc = now.QuadPart;
	elapsed_us *= 1000000;
	elapsed_us /= HostQPCFrequency;
	exec_time += elapsed_us;
	return exec_time;
}

static uint64_t get_next(uint64_t now)
{
	std::array<uint64_t, 4> next;
	next[0] = g_NV2A->vblank_next(now);
	next[1] = pit_next(now);
	next[2] = g_USB0->m_HostController->OHCI_next(now);
	next[3] = dsound_next(now);
	return *std::min_element(next.begin(), next.end());
}

xbox::void_xt NTAPI system_events(xbox::PVOID arg)
{
	// Testing shows that, if this thread has the same priority of the other xbox threads, it can take tens, even hundreds of ms to complete a single loop.
	// So we increase its priority to above normal, so that it completes a loop roughly every 3.1ms
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);

	while (true) {
		const uint64_t nearest_next = get_next(get_now());
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER *>(&last_time));

		while (true) {
			update_non_periodic_events();
			LARGE_INTEGER now;
			QueryPerformanceCounter(&now);
			uint64_t elapsed_us = static_cast<uint64_t>(now.QuadPart) - last_time;
			elapsed_us *= 1000000;
			elapsed_us /= HostQPCFrequency;
			if (elapsed_us >= nearest_next) {
				break;
			}
			std::this_thread::yield();
			_mm_pause();
		}
	}
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

