// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuShared.h
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
#ifndef EMUSHARED_H
#define EMUSHARED_H

#include "Mutex.h"
#include "InputConfig.h"

// ******************************************************************
// * cxbxkrnl exports, others import
// ******************************************************************
#ifndef _CXBXKRNL_INTERNAL
#define CXBXKRNL_API DECLSPEC_IMPORT
#else
#define CXBXKRNL_API DECLSPEC_EXPORT
#endif

// ******************************************************************
// * func: EmuSharedInit
// ******************************************************************
CXBXKRNL_API void EmuSharedInit();

// ******************************************************************
// * func: EmuSharedCleanup
// ******************************************************************
CXBXKRNL_API void EmuSharedCleanup();

// ******************************************************************
// * EmuShared : Shared memory
// ******************************************************************
extern CXBXKRNL_API class EmuShared : public Mutex
{
    public:
        // ******************************************************************
        // * UpdateInputConfiguration
        // ******************************************************************
        void UpdateInputConfiguration(InputConfig *x_InputConfig)
        {
            g_EmuShared->Lock();

            memcpy(&m_InputConfig, x_InputConfig, sizeof(InputConfig));

            m_dwChangeID++;

            g_EmuShared->Unlock();
        }

        // ******************************************************************
        // * Check the current Change ID
        // ******************************************************************
        uint32 GetChangeID() { return m_dwChangeID; }

    private:
        // ******************************************************************
        // * Shared configuration
        // ******************************************************************
        InputConfig m_InputConfig;

        // ******************************************************************
        // * Used to see if there has been a change
        // ******************************************************************
        uint32 m_dwChangeID;
}
*g_EmuShared;

#endif
