// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->Mutex.cpp
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
    while(true)
    {
        // Grab the lock, letting us look at the variables
        while(InterlockedCompareExchange((LPVOID*)&m_MutexLock, (LPVOID)1, (LPVOID)0))
//        while(InterlockedCompareExchange((LPLONG)&m_MutexLock, (LONG)1, (LONG)0))
            Sleep(1);

        // Are we the the new owner?
        if (!m_OwnerProcess)
        {
            // Take ownership
            InterlockedExchange(&m_OwnerProcess, (LONG)GetCurrentProcessId());
            InterlockedExchange(&m_OwnerThread, (LONG)GetCurrentThreadId());
            InterlockedExchange(&m_LockCount, 1);

            // Unlock the mutex itself
            InterlockedExchange(&m_MutexLock, 0);

            return;
        }

        // If a different process owns this mutex right now, unlock
        // the mutex lock and wait.  The reading need not be
        // interlocked.
        if ((m_OwnerProcess != (LONG) GetCurrentProcessId()) ||
            (m_OwnerThread  != (LONG) GetCurrentThreadId()))
        {
            // Unlock the mutex itself
            InterlockedExchange(&m_MutexLock, 0);

            // Wait and try again
            Sleep(1);
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
    while(InterlockedCompareExchange((LPVOID*)&m_MutexLock, (LPVOID)1, (LPVOID)0))
//    while (InterlockedCompareExchange((LPLONG)&m_MutexLock, (LONG)1, (LONG)0))
        Sleep(1);

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