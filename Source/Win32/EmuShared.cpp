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

#include "Emu.h"
#include "EmuShared.h"

#undef FIELD_OFFSET     // prevent macro redefinition warnings
#include <windows.h>
#include <stdio.h>

// ******************************************************************
// * exported globals
// ******************************************************************
CXBXKRNL_API EmuShared *g_EmuShared = NULL;
CXBXKRNL_API int        g_EmuSharedRefCount = 0;

// ******************************************************************
// * static/global
// ******************************************************************
HANDLE hMapObject = NULL;

// ******************************************************************
// * func: EmuShared::EmuSharedInit
// ******************************************************************
CXBXKRNL_API void EmuShared::Init()
{
    // ******************************************************************
    // * Ensure initialization only occurs once
    // ******************************************************************
    bool init = true;

    // ******************************************************************
    // * Prevent multiple initializations
    // ******************************************************************
    if(hMapObject != NULL)
        return;

    // ******************************************************************
    // * Create the shared memory "file"
    // ******************************************************************
    {
        hMapObject = CreateFileMapping
        ( 
            INVALID_HANDLE_VALUE,   // Paging file
            NULL,                   // default security attributes
            PAGE_READWRITE,         // read/write access
            0,                      // size: high 32 bits
            sizeof(EmuShared),      // size: low 32 bits
            "Local\\EmuShared"      // name of map object
        );

        if(hMapObject == NULL)
			EmuCleanup("Could not map shared memory!");

        if(GetLastError() == ERROR_ALREADY_EXISTS)
            init = false;
    }

    // ******************************************************************
    // * Memory map this file
    // ******************************************************************
    {
        g_EmuShared = (EmuShared*)MapViewOfFile
        (
            hMapObject,     // object to map view of
            FILE_MAP_WRITE, // read/write access
            0,              // high offset:  map from
            0,              // low offset:   beginning
            0               // default: map entire file
        );

        if(g_EmuShared == NULL) 
			EmuCleanup("Could not map view of shared memory!");
    }

    // ******************************************************************
    // * Executed only on first initialization of shared memory
    // ******************************************************************
    if(init)
        g_EmuShared->EmuShared::EmuShared();

    g_EmuSharedRefCount++;
}

// ******************************************************************
// * func: EmuSharedCleanup
// ******************************************************************
CXBXKRNL_API void EmuShared::Cleanup()
{
    g_EmuSharedRefCount--;

    if(g_EmuSharedRefCount == 0)
        g_EmuShared->EmuShared::~EmuShared();

    UnmapViewOfFile(g_EmuShared);
}

// ******************************************************************
// * Constructor
// ******************************************************************
CXBXKRNL_API EmuShared::EmuShared()
{
    m_XBController.Load("Software\\Cxbx\\XBController");
    m_XBVideo.Load("Software\\Cxbx\\XBVideo");
}

// ******************************************************************
// * Deconstructor
// ******************************************************************
CXBXKRNL_API EmuShared::~EmuShared()
{
    m_XBController.Save("Software\\Cxbx\\XBController");
    m_XBVideo.Save("Software\\Cxbx\\XBVideo");
}