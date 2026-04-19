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
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#include "Mutex.h"
#include <intrin.h> // YieldProcessor / _mm_pause

// Spin this many times with YieldProcessor before falling back to SwitchToThread.
// ~200 iterations costs < 1 µs on modern CPUs, which is cheaper than a context switch
// for locks held for a short period (the typical case in Cxbx's emulation mutex use).
static constexpr int kSpinCount = 200;

// Helper: spin briefly, then yield the time-slice once before the caller retries.
static inline void MutexBackoff()
{
    for (int i = 0; i < kSpinCount; ++i) {
        YieldProcessor(); // issues PAUSE on x86 — hints the CPU this is a spin-wait
    }
    SwitchToThread(); // let the OS run other ready threads; avoids burning an entire ms
}

// ******************************************************************
// * Constructor
// ******************************************************************
Mutex::Mutex()
{
    InterlockedExchange(&m_MutexLock, 0);
    InterlockedExchange(&m_OwnerProcess, 0);
    InterlockedExchange(&m_OwnerThread, 0);
    InterlockedExchange(&m_LockCount, 0);
}

// ******************************************************************
// * Lock
// ******************************************************************
void Mutex::Lock()
{
    LONG _CurrentProcessId = (LONG) GetCurrentProcessId();
    LONG _CurrentThreadId = (LONG) GetCurrentThreadId();
    while(true)
    {
        // Grab the lock, letting us look at the variables
#if defined(_MSC_VER) && (_MSC_VER < 1300)  // We are not using VC++.NET
        while(InterlockedCompareExchange((LPVOID*)&m_MutexLock, (LPVOID)1, (LPVOID)0))
#else
        while(InterlockedCompareExchange((LPLONG)&m_MutexLock, (LONG)1, (LONG)0))
#endif
            MutexBackoff();

        // Are we the the new owner?
        if (!m_OwnerProcess)
        {
            // Take ownership
            InterlockedExchange(&m_OwnerProcess, _CurrentProcessId);
            InterlockedExchange(&m_OwnerThread, _CurrentThreadId);
            InterlockedExchange(&m_LockCount, 1);

            // Unlock the mutex itself
            InterlockedExchange(&m_MutexLock, 0);

            return;
        }

        // If a different process owns this mutex right now, unlock
        // the mutex lock and wait.  The reading need not be
        // interlocked.
        if ((m_OwnerProcess != _CurrentProcessId) ||
            (m_OwnerThread  != _CurrentThreadId))
        {
            // Unlock the mutex itself
            InterlockedExchange(&m_MutexLock, 0);

            // Spin-wait then yield before retrying; avoids the 1 ms floor of Sleep(1).
            MutexBackoff();
            continue;
        }

        // The mutex was already locked, but by us.  Just increment
        // the lock count and unlock the mutex itself.
        InterlockedIncrement(&m_LockCount);
        InterlockedExchange(&m_MutexLock, 0);

        return;
    }
}

// ******************************************************************
// * Unlock
// ******************************************************************
void Mutex::Unlock()
{
    // Grab the lock, letting us look at the variables
#if defined(_MSC_VER) && (_MSC_VER < 1300)  // We are not using VC++.NET
    while(InterlockedCompareExchange((LPVOID*)&m_MutexLock, (LPVOID)1, (LPVOID)0))
#else
    while (InterlockedCompareExchange((LPLONG)&m_MutexLock, (LONG)1, (LONG)0))
#endif
        MutexBackoff();

    // Decrement the lock count
    if (!InterlockedDecrement(&m_LockCount))
    {
        // Mark the mutex as now unused
        InterlockedExchange(&m_OwnerProcess, 0);
        InterlockedExchange(&m_OwnerThread, 0);
    }

    // Unlock the mutex itself
    InterlockedExchange(&m_MutexLock, 0);
}