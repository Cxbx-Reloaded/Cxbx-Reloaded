// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuXapi.cpp
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
#include "Cxbx.h"
#include "CxbxKrnl.h"

#include <stdio.h>

// ******************************************************************
// * win32 wrapped in namespace to avoid collisions
// ******************************************************************
namespace win32
{
    #undef FIELD_OFFSET     // prevent macro redefinition warnings
    #include <windows.h>
}

using namespace win32;

// ******************************************************************
// * EmuCreateThreadProxyParam
// ******************************************************************
typedef struct _EmuCreateThreadProxyParam
{
    LPVOID                 lpParameter;
    LPTHREAD_START_ROUTINE lpStartAddress;
}
EmuCreateThreadProxyParam;

// ******************************************************************
// * func: EmuCreateThreadProxy
// ******************************************************************
static DWORD WINAPI EmuCreateThreadProxy
(
    LPVOID lpParameter
)
{
    EmuCreateThreadProxyParam *iEmuCreateThreadProxyParam = (EmuCreateThreadProxyParam*)lpParameter;

    LPTHREAD_START_ROUTINE  ilpStartAddress = iEmuCreateThreadProxyParam->lpStartAddress;
    LPVOID                  ilpParam        = iEmuCreateThreadProxyParam->lpParameter;

    delete iEmuCreateThreadProxyParam;

    EmuXGenerateFS();

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("CxbxKrnl [0x%.08X]: EmuCreateThreadProxy\n"
               "          (\n"
               "             lpParameter         : 0x%.08X\n"
               "          );\n",
               GetCurrentThreadId(), lpParameter);

    }
    #endif

    EmuXSwapFS();   // XBox FS

    return ilpStartAddress(ilpParam);
}

// ******************************************************************
// * func: EmuCreateThread
// ******************************************************************
HANDLE WINAPI xboxkrnl::EmuCreateThread
(
    LPSECURITY_ATTRIBUTES   lpThreadAttributes,
    DWORD                   dwStackSize,
    LPTHREAD_START_ROUTINE  lpStartAddress,
    LPVOID                  lpParameter,
    DWORD                   dwCreationFlags,
    LPDWORD                 lpThreadId
)
{
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("CxbxKrnl [0x%.08X]: EmuCreateThread\n"
               "          (\n"
               "             lpThreadAttributes  : 0x%.08X\n"
               "             dwStackSize         : 0x%.08X\n"
               "             lpStartAddress      : 0x%.08X\n"
               "             lpParameter         : 0x%.08X\n"
               "             dwCreationFlags     : 0x%.08X\n"
               "             lpThreadId          : 0x%.08X\n"
               "          );\n",
               GetCurrentThreadId(), lpThreadAttributes, dwStackSize, lpStartAddress,
               lpParameter, dwCreationFlags, lpThreadId);
    }
    #endif

    EmuCreateThreadProxyParam *iEmuCreateThreadProxyParam = new EmuCreateThreadProxyParam();

    iEmuCreateThreadProxyParam->lpParameter    = lpParameter;
    iEmuCreateThreadProxyParam->lpStartAddress = lpStartAddress;

    HANDLE RetHandle = CreateThread
    (
       NULL,
       dwStackSize,
       EmuCreateThreadProxy,
       iEmuCreateThreadProxyParam,
       dwCreationFlags,
       lpThreadId
    );

    EmuXSwapFS();   // XBox FS

    return RetHandle;
}

// ******************************************************************
// * func: EmuCloseHandle
// ******************************************************************
BOOL WINAPI xboxkrnl::EmuCloseHandle
(
    HANDLE hObject
)
{
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("CxbxKrnl [0x%.08X]: EmuCloseHandle\n"
               "          (\n"
               "             hObject             : 0x%.08X\n"
               "          );\n",
               GetCurrentThreadId(), hObject);
    }
    #endif

    BOOL Ret = CloseHandle(hObject);

    EmuXSwapFS();   // XBox FS

    return Ret;
}