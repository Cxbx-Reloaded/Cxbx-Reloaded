// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuX.cpp
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
#define CXBXKRNL_INTERNAL
#define _XBOXKRNL_LOCAL_

#include "Cxbx.h"
#include "EmuX.h"

// ******************************************************************
// * ntdll wrapped in namespace to avoid collisions
// ******************************************************************
namespace xntdll
{
    #include "xntdll.h"
};

#include <windows.h>
#include <stdio.h>

// ******************************************************************
// * static functions
// ******************************************************************
static void EmuXInstallWrappers(void (*Entry)());

// ******************************************************************
// * func: EmuXInit
// ******************************************************************
CXBXKRNL_API void NTAPI EmuXInit(DebugMode DebugConsole, char *DebugFilename, uint08 *XBEHeader, uint32 XBEHeaderSize, void (*Entry)())
{
    // ******************************************************************
    // * debug console allocation (if configured)
    // ******************************************************************
    if(DebugConsole == DM_CONSOLE)
    {
        if(AllocConsole())
        {
            freopen("CONOUT$", "wt", stdout);

            printf("CxbxKrnl [0x%.08X]: Debug console allocated.\n", GetCurrentThreadId());
        }
    }
    else if(DebugConsole == DM_FILE)
    {
        FreeConsole();

        freopen(DebugFilename, "wt", stdout);

        printf("CxbxKrnl [0x%.08X]: Debug console allocated.\n", GetCurrentThreadId());
    }

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    {
        printf("CxbxKrnl [0x%.08X]: EmuXInit\n"
               "          (\n"
               "             DebugConsole        : 0x%.08X\n"
               "             DebugFilename       : \"%s\"\n"
               "             XBEHeader           : 0x%.08X\n"
               "             XBEHeaderSize       : 0x%.08X\n"
               "             Entry               : 0x%.08X\n"
               "          );\n",
               GetCurrentThreadId(), DebugConsole, DebugFilename, XBEHeader, XBEHeaderSize, Entry);
    }

    // ******************************************************************
    // * Locate functions and install wrapper vectors
    // ******************************************************************
    {
        EmuXInstallWrappers(Entry);
    }

    // ******************************************************************
    // * Load the necessary pieces of XBEHeader
    // ******************************************************************
    {
        uint32 old_protection = 0;

        VirtualProtect((void*)0x00010000, 0x1000, PAGE_READWRITE, &old_protection);

        // we sure hope we aren't corrupting anything necessary for an .exe to survive :]
        uint32 dwSizeofHeaders   = *(uint32*)&XBEHeader[0x0108];
        uint32 dwCertificateAddr = *(uint32*)&XBEHeader[0x0118];
        uint32 dwInitFlags       = *(uint32*)&XBEHeader[0x0124];
        uint32 dwPeHeapReserve   = *(uint32*)&XBEHeader[0x0134];
        uint32 dwPeHeapCommit    = *(uint32*)&XBEHeader[0x0138];

        *(uint32 *)0x00010108 = dwSizeofHeaders;
        *(uint32 *)0x00010118 = dwCertificateAddr;
        *(uint32 *)0x00010124 = dwInitFlags;
        *(uint32 *)0x00010134 = dwPeHeapReserve;
        *(uint32 *)0x00010138 = dwPeHeapCommit;

        memcpy((void*)dwCertificateAddr, &XBEHeader[dwCertificateAddr - 0x00010000], sizeof(Xbe::Certificate));
    }

    // ******************************************************************
    // * Initialize FS system
    // ******************************************************************
    {
        EmuXInitFS();
    }

    // ******************************************************************
    // * Initialize FS:* structure
    // ******************************************************************
    {
        EmuXGenerateFS();
    }

    printf("CxbxKrnl [0x%.08X]: Initial thread starting.\n", GetCurrentThreadId());

    EmuXSwapFS();   // XBox FS

    // This must be enabled or the debugger may crash (sigh)
    //_asm _emit 0xF1

    Entry();

    EmuXSwapFS();   // Win2k/XP FS

    printf("CxbxKrnl [0x%.08X]: Initial thread ended.\n", GetCurrentThreadId());

    fflush(stdout);

    return;
}

// ******************************************************************
// * func: EmuXDummy
// ******************************************************************
CXBXKRNL_API void NTAPI EmuXDummy()
{
    EmuXSwapFS();   // Win2k/XP FS

    MessageBox(NULL, "EmuXDummy()", "CxbxKrnl", MB_OK);

    EmuXSwapFS();   // XBox FS
}

// ******************************************************************
// * func: EmuXPanic
// ******************************************************************
CXBXKRNL_API void NTAPI EmuXPanic()
{
    EmuXSwapFS();   // Win2k/XP FS

    printf("CxbxKrnl [0x%.08X]: EmuXPanic()\n", GetCurrentThreadId());

    MessageBox(NULL, "Kernel Panic! Process will now terminate.", "CxbxKrnl", MB_OK | MB_ICONEXCLAMATION);

    EmuXSwapFS();   // XBox FS
}

// ******************************************************************
// * func: EmuXInstallWrappers
// ******************************************************************
void EmuXInstallWrappers(void (*Entry)())
{
    // ******************************************************************
    // * debug trace
    // ******************************************************************
    {
        printf("CxbxKrnl [0x%.08X]: EmuXInstallWrappers()\n"
               "          (\n"
               "             Entry               : 0x%.08X\n"
               "          );\n",
            GetCurrentThreadId(), Entry);
    }

    // ******************************************************************
    // * install CreateThread vector
    // ******************************************************************
    {
        // ******************************************************************
        // * CreateThread is easily located using an offset from the standard
        // * xbe entry point
        // ******************************************************************
        uint32  RelCallAddr         = (uint32)Entry + 0x54;
        uint32  RelRealCreateThread = *(uint32*)(RelCallAddr + 1);
        uint08 *RealCreateThread    = (uint08*)(RelCallAddr + RelRealCreateThread + 5);

        printf("RealCreateThread : %.08X\n", RealCreateThread);
        fflush(stdout);

        *(uint08*)&RealCreateThread[0] = 0xE9;
        *(uint32*)&RealCreateThread[1] = (uint32)xboxkrnl::EmuXCreateThread - (uint32)RealCreateThread - 5;
    }

    // ******************************************************************
    // * install CloseHandle vector
    // ******************************************************************
    {
        // ******************************************************************
        // * CloseHandle is easily located using an offset from the standard
        // * xbe entry point
        // ******************************************************************
        uint32  RelCallAddr         = (uint32)Entry + 0x6A;
        uint32  RelRealCloseHandle  = *(uint32*)(RelCallAddr + 1);
        uint08 *RealCloseHandle     = (uint08*)(RelCallAddr + RelRealCloseHandle + 5);

        *(uint08*)&RealCloseHandle[0] = 0xE9;
        *(uint32*)&RealCloseHandle[1] = (uint32)xboxkrnl::EmuXCloseHandle - (uint32)RealCloseHandle - 5;
    }
}