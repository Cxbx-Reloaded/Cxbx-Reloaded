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
        Mutex() { m_Mutex = 0; }

        // ******************************************************************
        // * Deconstructor
        // ******************************************************************
        ~Mutex() {  }

        // ******************************************************************
        // * Lock critical section
        // ******************************************************************
        void Lock() { while(InterlockedCompareExchange(&m_Mutex, 1, 0) == 1) Sleep(0); }

        // ******************************************************************
        // * Unlock critical section
        // ******************************************************************
        void Unlock() { if(m_Mutex == 1)InterlockedDecrement(&m_Mutex); }

    private:
        // ******************************************************************
        // * Critical section
        // ******************************************************************
        int32 m_Mutex;
};

#endif
