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
#include "EmuX.h"

#include <stdio.h>

// ******************************************************************
// * win32 wrapped in namespace to avoid collisions
// ******************************************************************
namespace win32
{
    #include <windows.h>
}

using namespace win32;

// ******************************************************************
// * EmuXCreateThreadProxyParam
// ******************************************************************
typedef struct _EmuXCreateThreadProxyParam
{
    LPVOID                 lpParameter;
    LPTHREAD_START_ROUTINE lpStartAddress;
}
EmuXCreateThreadProxyParam;

// ******************************************************************
// * func: EmuXCreateThreadProxy
// ******************************************************************
static DWORD WINAPI EmuXCreateThreadProxy
(
    LPVOID lpParameter
)
{
    EmuXCreateThreadProxyParam *iEmuXCreateThreadProxyParam = (EmuXCreateThreadProxyParam*)lpParameter;

    LPTHREAD_START_ROUTINE  ilpStartAddress = iEmuXCreateThreadProxyParam->lpStartAddress;
    LPVOID                  ilpParam        = iEmuXCreateThreadProxyParam->lpParameter;

    delete iEmuXCreateThreadProxyParam;

    EmuXGenerateFS();

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("CxbxKrnl [0x%.08X]: EmuXCreateThreadProxy\n"
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
// * func: EmuXCreateThread
// ******************************************************************
HANDLE WINAPI xboxkrnl::EmuXCreateThread
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
        printf("CxbxKrnl [0x%.08X]: EmuXCreateThread\n"
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

    EmuXCreateThreadProxyParam *iEmuXCreateThreadProxyParam = new EmuXCreateThreadProxyParam();

    iEmuXCreateThreadProxyParam->lpParameter    = lpParameter;
    iEmuXCreateThreadProxyParam->lpStartAddress = lpStartAddress;

    HANDLE RetHandle = CreateThread
    (
       NULL,
       dwStackSize,
       EmuXCreateThreadProxy,
       iEmuXCreateThreadProxyParam,
       dwCreationFlags,
       lpThreadId
    );

    EmuXSwapFS();   // XBox FS

    return RetHandle;
}

// ******************************************************************
// * func: EmuXCloseHandle
// ******************************************************************
BOOL WINAPI xboxkrnl::EmuXCloseHandle
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
        printf("CxbxKrnl [0x%.08X]: EmuXCloseHandle\n"
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