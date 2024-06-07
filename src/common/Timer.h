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


extern int64_t HostQPCFrequency;

void timer_init();
uint64_t get_now();
int64_t Timer_GetScaledPerformanceCounter(int64_t Period);
void SleepPrecise(std::chrono::steady_clock::time_point targetTime);

#endif
