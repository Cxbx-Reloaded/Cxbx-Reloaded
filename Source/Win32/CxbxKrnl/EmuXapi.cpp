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
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_LOCAL_

#undef FIELD_OFFSET     // prevent macro redefinition warnings
#include <windows.h>

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace xapi
{
    #include "EmuXapi.h"
};

#include "Emu.h"
#include "EmuFS.h"
#include "EmuD3D8.h"
#include "EmuDInput.h"

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

    EmuGenerateFS(g_pTLS);

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXapi (0x%.08X): EmuCreateThreadProxy\n"
               "(\n"
               "   lpParameter         : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), lpParameter);

    }
    #endif

    EmuSwapFS();   // XBox FS

    DWORD ret = ilpStartAddress(ilpParam);

    EmuSwapFS();   // Win2k/XP FS

    return ret;
}

// ******************************************************************
// * func: EmuXInitDevices
// ******************************************************************
VOID WINAPI xapi::EmuXInitDevices
(
    DWORD   Unknown1,
    PVOID   Unknown2
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXapi (0x%.08X): EmuXInitDevices\n"
               "(\n"
               "   Unknown1            : 0x%.08X\n"
               "   Unknown2            : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Unknown1, Unknown2);
    }
    #endif

    // TODO: For now, we don't need to init devices, we merely intercept
    //       this function to prevent hardware access code from executing

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuXGetDevices
// ******************************************************************
DWORD WINAPI xapi::EmuXGetDevices
(
    PXPP_DEVICE_TYPE DeviceType
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXapi (0x%.08X): EmuXGetDevices\n"
               "(\n"
               "   DeviceType          : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), DeviceType);
    }
    #endif

    EmuSwapFS();   // XBox FS

    // TODO: Temporarily just return 1 Controller, even if the user didn't ask about Controllers
    return (1 << 0);
}

// ******************************************************************
// * func: EmuXInputOpen
// ******************************************************************
HANDLE WINAPI xapi::EmuXInputOpen
(
    IN PXPP_DEVICE_TYPE             DeviceType,
    IN DWORD                        dwPort,
    IN DWORD                        dwSlot,
    IN PXINPUT_POLLING_PARAMETERS   pPollingParameters OPTIONAL
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXapi (0x%.08X): EmuXInputOpen\n"
               "(\n"
               "   DeviceType          : 0x%.08X\n"
               "   dwPort              : 0x%.08X\n"
               "   dwSlot              : 0x%.08X\n"
               "   pPollingParameters  : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), DeviceType, dwPort, dwSlot, pPollingParameters);
    }
    #endif

    HANDLE ret = NULL;

    // TODO: We simply return dwPort+1 to represent the input device (lame!)
    if(dwPort <= 3)
        ret = (HANDLE)(dwPort+1);

    EmuSwapFS();   // XBox FS

    return ret;
}

// ******************************************************************
// * func: EmuXInputGetCapabilities
// ******************************************************************
DWORD WINAPI xapi::EmuXInputGetCapabilities
(
    IN  HANDLE               hDevice,
    OUT PXINPUT_CAPABILITIES pCapabilities
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXapi (0x%.08X): EmuXInputGetCapabilities\n"
               "(\n"
               "   hDevice             : 0x%.08X\n"
               "   pCapabilities       : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), hDevice, pCapabilities);
    }
    #endif

    DWORD ret = ERROR_INVALID_HANDLE;

    // TODO: For now, the only valid handles are Controller 1 through 4,
    //       and they are always normal Controllers
    if((int)hDevice >= 1 && (int)hDevice <= 4)
    {
        pCapabilities->SubType = XINPUT_DEVSUBTYPE_GC_GAMEPAD;

        ZeroMemory(&pCapabilities->In.Gamepad, sizeof(pCapabilities->In.Gamepad));

        ret = ERROR_SUCCESS;
    }

    EmuSwapFS();   // XBox FS

    return ret;
}

// ******************************************************************
// * func: EmuInputGetState
// ******************************************************************
DWORD WINAPI xapi::EmuXInputGetState
(
    IN  HANDLE         hDevice,
    OUT PXINPUT_STATE  pState
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXapi (0x%.08X): XInputGetState\n"
               "(\n"
               "   hDevice             : 0x%.08X\n"
               "   pState              : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), hDevice, pState);
    }
    #endif

    DWORD ret = ERROR_INVALID_HANDLE;

    // TODO: For now, the only valid handles are Controller 1 through 4,
    //       and they are always normal Controllers
    if((int)hDevice >= 1 && (int)hDevice <= 4)
    {
        if((int)hDevice == 1)
            EmuDInputPoll(pState);

        ret = ERROR_SUCCESS;
    }

    EmuSwapFS();   // XBox FS

    return ret;
}

// ******************************************************************
// * func: EmuCreateThread
// ******************************************************************
HANDLE WINAPI xapi::EmuCreateThread
(
    LPSECURITY_ATTRIBUTES   lpThreadAttributes,
    DWORD                   dwStackSize,
    LPTHREAD_START_ROUTINE  lpStartAddress,
    LPVOID                  lpParameter,
    DWORD                   dwCreationFlags,
    LPDWORD                 lpThreadId
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXapi (0x%.08X): EmuCreateThread\n"
               "(\n"
               "   lpThreadAttributes  : 0x%.08X\n"
               "   dwStackSize         : 0x%.08X\n"
               "   lpStartAddress      : 0x%.08X\n"
               "   lpParameter         : 0x%.08X\n"
               "   dwCreationFlags     : 0x%.08X\n"
               "   lpThreadId          : 0x%.08X\n"
               ");\n",
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

    EmuSwapFS();   // XBox FS

    return RetHandle;
}

// ******************************************************************
// * func: EmuCloseHandle
// ******************************************************************
BOOL WINAPI xapi::EmuCloseHandle
(
    HANDLE hObject
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXapi (0x%.08X): EmuCloseHandle\n"
               "(\n"
               "   hObject             : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), hObject);
    }
    #endif

    BOOL Ret = CloseHandle(hObject);

    EmuSwapFS();   // XBox FS

    return Ret;
}

// ******************************************************************
// * func: EmuXapiInitProcess
// ******************************************************************
VOID WINAPI xapi::EmuXapiInitProcess()
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXapi (0x%.08X): EmuXapiInitProcess();\n", GetCurrentThreadId());
    }
    #endif

    // TODO: Process initialization (if necessary)
	// TODO: Somehow initialize floating point

    EmuSwapFS();   // XBox FS

    return;
}
/* Too High Level!
// ******************************************************************
// * func: XapiSetupPerTitleDriveLetters
// ******************************************************************
xapi::NTSTATUS CDECL xapi::XapiSetupPerTitleDriveLetters(DWORD dwTitleId, LPCWSTR wszTitleName)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXapi (0x%.08X): XapiSetupPerTitleDriveLetters\n"
               "(\n"
               "   dwTitleId           : 0x%.08X\n"
               "   wszTitleName        : 0x%.08X\n"
               ");\n",
                GetCurrentThreadId(), dwTitleId, wszTitleName);
    }
    #endif

    NTSTATUS ret = STATUS_SUCCESS;

    EmuSwapFS();   // XBox FS

    return ret;
}
*/
// ******************************************************************
// * func: EmuXapiBootDash
// ******************************************************************
VOID WINAPI xapi::EmuXapiBootDash(DWORD UnknownA, DWORD UnknownB, DWORD UnknownC)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXapi (0x%.08X): EmuXapiBootDash\n"
               "(\n"
               "   UnknownA            : 0x%.08X\n"
               "   UnknownB            : 0x%.08X\n"
               "   UnknownC            : 0x%.08X\n"
               ");\n",
                GetCurrentThreadId(), UnknownA, UnknownB, UnknownC);
    }
    #endif

    EmuCleanup("EmuXapiBootDash");

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: Emu__rtinit
// ******************************************************************
VOID xapi::Emu__rtinit()
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXapi (0x%.08X): Emu__rtinit();\n", GetCurrentThreadId());
    }
    #endif

    EmuSwapFS();   // XBox FS
}

// ******************************************************************
// * func: Emu__cinit
// ******************************************************************
VOID xapi::Emu__cinit()
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXapi (0x%.08X): Emu__cinit();\n", GetCurrentThreadId());
    }
    #endif

    EmuSwapFS();   // XBox FS
}