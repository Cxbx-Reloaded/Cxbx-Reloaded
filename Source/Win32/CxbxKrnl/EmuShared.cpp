// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuShared.cpp
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
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_LOCAL_

#include "Cxbx.h"
#include "Emu.h"

using namespace win32;

// ******************************************************************
// * shared memory
// ******************************************************************
CXBXKRNL_API EmuShared *g_EmuShared = NULL;

// ******************************************************************
// * map object
// ******************************************************************
static HANDLE g_hMapObject = NULL;

// ******************************************************************
// * func: EmuSharedInit
// ******************************************************************
CXBXKRNL_API void EmuSharedInit()
{
    // we should only initialize once
    if(g_hMapObject != NULL)
        return;

    bool init = true;

    g_hMapObject = CreateFileMapping
    ( 
        INVALID_HANDLE_VALUE,   // Paging file
        NULL,                   // default security attributes
        PAGE_READWRITE,         // read/write access
        0,                      // size: high 32 bits
        sizeof(EmuShared),      // size: low 32 bits
        "EmuShared"             // name of map object
    );

    if(g_hMapObject == NULL)
        return; // TODO: Fail gracefully

    if(GetLastError() == ERROR_ALREADY_EXISTS)
        init = false;

    g_EmuShared = (EmuShared*)MapViewOfFile
    (
        g_hMapObject,   // object to map view of
        FILE_MAP_WRITE, // read/write access
        0,              // high offset:  map from
        0,              // low offset:   beginning
        0               // default: map entire file
    );

    if(g_EmuShared == NULL) 
        return; // TODO: Fail gracefully

    if(init)
    {
        // initialization of shared data
        g_EmuShared->dwRandomData = 0;
    }

}

// ******************************************************************
// * func: EmuSharedCleanup
// ******************************************************************
CXBXKRNL_API void EmuSharedCleanup()
{
    UnmapViewOfFile(g_EmuShared);

    CloseHandle(g_hMapObject);
}