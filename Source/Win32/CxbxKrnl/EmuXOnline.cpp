// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuXOnline.cpp
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

#undef FIELD_OFFSET     // prevent macro redefinition warnings
#include <windows.h>

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace xntdll
{
    #include "xntdll.h"
};

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace xonline
{
    #include "xonline.h"
};

#include "Emu.h"
#include "EmuFS.h"
#include "EmuD3D8.h"
#include "EmuDInput.h"

// ******************************************************************
// * func: EmuWSAStartup
// ******************************************************************
int WINAPI xonline::EmuWSAStartup
(
    WORD        wVersionRequested,
    LPVOID      lpWSAData   // todo: use correct type
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXapi (0x%X): EmuWSAStartup\n"
               "(\n"
               "   wVersionRequested   : 0x%.08X\n"
               "   lpWSAData           : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), wVersionRequested, lpWSAData);
    }
    #endif

    EmuSwapFS();   // XBox FS

    // Fake Successfull...hehehe...sucker...hehehehehe
    return 0;
}

// ******************************************************************
// * func: EmuXNetStartup
// ******************************************************************
INT WINAPI xonline::EmuXNetStartup
(
    const PVOID pDummy
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXapi (0x%X): EmuXNetStartup\n"
               "(\n"
               "   pDummy              : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pDummy);
    }
    #endif

    EmuSwapFS();   // XBox FS

    // Fake Successfull...hehehe...sucker...hehehehehe
    return 0;
}
