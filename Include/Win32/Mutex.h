// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->Mutex.h
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
#ifndef MUTEX_H
#define MUTEX_H

#include <windows.h>

// ******************************************************************
// * Mutex object (intended to be inherited from)
// ******************************************************************
class Mutex
{
    public:
        // ******************************************************************
        // * Constructor
        // ******************************************************************
        Mutex();

        // ******************************************************************
        // * Lock critical section
        // ******************************************************************
        void Lock();

        // ******************************************************************
        // * Unlock critical section
        // ******************************************************************
        void Unlock();

    private:
        // ******************************************************************
        // * Critical section
        // ******************************************************************
        LONG m_MutexLock;      // Mutex lock
        LONG m_OwnerProcess;   // Current owner process (or zero)
        LONG m_OwnerThread;    // Current owner thread
        LONG m_LockCount;      // Lock count within this thread
};

#endif
