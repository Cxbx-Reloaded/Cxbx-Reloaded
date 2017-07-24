// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
#define _XBOXKRNL_DEFEXTRN_

#include "CxbxKrnl/CxbxKrnl.h"
#include "CxbxKrnl/Emu.h"
#include "CxbxKrnl/EmuShared.h"

#undef FIELD_OFFSET     // prevent macro redefinition warnings
#include <windows.h>
#include <cstdio>

// ******************************************************************
// * exported globals
// ******************************************************************
EmuShared *g_EmuShared = nullptr;

// ******************************************************************
// * static/global
// ******************************************************************
HANDLE hMapObject = NULL;

// ******************************************************************
// * func: EmuShared::EmuSharedInit
// ******************************************************************
void EmuShared::Init()
{
    // ******************************************************************
    // * Ensure initialization only occurs once
    // ******************************************************************
    bool bRequireConstruction = true;

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
            CxbxKrnlCleanup("Could not map shared memory!");

        if(GetLastError() == ERROR_ALREADY_EXISTS)
            bRequireConstruction = false;
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

        if(g_EmuShared == nullptr)
            CxbxKrnlCleanup("Could not map view of shared memory!");
    }

    // ******************************************************************
    // * Executed only on first initialization of shared memory
    // ******************************************************************
    if(bRequireConstruction)
        g_EmuShared->EmuShared::EmuShared();

    g_EmuShared->m_RefCount++;
}

// ******************************************************************
// * func: EmuSharedCleanup
// ******************************************************************
void EmuShared::Cleanup()
{
	if (g_EmuShared != nullptr) {
		if (--(g_EmuShared->m_RefCount) <= 0)
			g_EmuShared->EmuShared::~EmuShared();

		UnmapViewOfFile(g_EmuShared);
		g_EmuShared = nullptr;
	}
}

// ******************************************************************
// * Constructor
// ******************************************************************
EmuShared::EmuShared()
{
	Load();
}

// ******************************************************************
// * Deconstructor
// ******************************************************************
EmuShared::~EmuShared()
{
	Save();
}

void EmuShared::Load()
{
	m_XBController.Load("Software\\Cxbx-Reloaded\\XBController");
    m_XBVideo.Load("Software\\Cxbx-Reloaded\\XBVideo");
    m_XBAudio.Load("Software\\Cxbx-Reloaded\\XBAudio");
}

void EmuShared::Save()
{
	m_XBController.Save("Software\\Cxbx-Reloaded\\XBController");
    m_XBVideo.Save("Software\\Cxbx-Reloaded\\XBVideo");
    m_XBAudio.Save("Software\\Cxbx-Reloaded\\XBAudio");
}