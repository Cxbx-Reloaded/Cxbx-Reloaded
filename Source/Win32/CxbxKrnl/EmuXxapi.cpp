// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuXxapi.cpp
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
        printf("EmuXxapi (0x%.08X): EmuXCreateThreadProxy\n"
               "(\n"
               "   lpParameter         : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), lpParameter);

    }
    #endif

    EmuXSwapFS();   // XBox FS

    return ilpStartAddress(ilpParam);
}

// ******************************************************************
// * func: EmuXXInitDevices
// ******************************************************************
VOID WINAPI xboxkrnl::EmuXXInitDevices
(
    DWORD   Unknown1,
    PVOID   Unknown2
)
{
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXxapi (0x%.08X): EmuXXInitDevices\n"
               "(\n"
               "   Unknown1            : 0x%.08X\n"
               "   Unknown2            : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Unknown1, Unknown2);
    }
    #endif

    // TODO: For now, we don't need to init devices, we merely intercept
    //       this function to prevent hardware access code from executing

    EmuXSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuXXGetDevices
// ******************************************************************
DWORD WINAPI xboxkrnl::EmuXXGetDevices
(
    PXPP_DEVICE_TYPE DeviceType
)
{
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXxapi (0x%.08X): EmuXXGetDevices\n"
               "(\n"
               "   DeviceType          : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), DeviceType);
    }
    #endif

    EmuXSwapFS();   // XBox FS

    // TODO: Temporarily just return 1 controller, even if the user didn't ask about controllers
    return (1 << 0);
}

// ******************************************************************
// * func: EmuXXInputOpen
// ******************************************************************
HANDLE WINAPI xboxkrnl::EmuXXInputOpen
(
    IN PXPP_DEVICE_TYPE             DeviceType,
    IN DWORD                        dwPort,
    IN DWORD                        dwSlot,
    IN PXINPUT_POLLING_PARAMETERS   pPollingParameters OPTIONAL
)
{
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXxapi (0x%.08X): EmuXXInputOpen\n"
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

    EmuXSwapFS();   // XBox FS

    return ret;
}

// ******************************************************************
// * func: EmuXXInputGetCapabilities
// ******************************************************************
DWORD WINAPI xboxkrnl::EmuXXInputGetCapabilities
(
    IN  HANDLE               hDevice,
    OUT PXINPUT_CAPABILITIES pCapabilities
)
{
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXxapi (0x%.08X): EmuXXInputGetCapabilities\n"
               "(\n"
               "   hDevice             : 0x%.08X\n"
               "   pCapabilities       : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), hDevice, pCapabilities);
    }
    #endif

    DWORD ret = ERROR_INVALID_HANDLE;

    // TODO: For now, the only valid handles are controllers 1 through 4,
    //       and they are always normal controllers
    if((int)hDevice >= 1 && (int)hDevice <= 4)
    {
        pCapabilities->SubType = XINPUT_DEVSUBTYPE_GC_GAMEPAD;

        ZeroMemory(&pCapabilities->In.Gamepad, sizeof(pCapabilities->In.Gamepad));

        ret = ERROR_SUCCESS;
    }

    EmuXSwapFS();   // XBox FS

    return ret;
}

// ******************************************************************
// * func: EmuXInputGetState
// ******************************************************************
DWORD WINAPI xboxkrnl::EmuXXInputGetState
(
    IN  HANDLE         hDevice,
    OUT PXINPUT_STATE  pState
)
{
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXxapi (0x%.08X): XInputGetState\n"
               "(\n"
               "   hDevice             : 0x%.08X\n"
               "   pState              : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), hDevice, pState);
    }
    #endif

    DWORD ret = ERROR_INVALID_HANDLE;

    // TODO: For now, the only valid handles are controllers 1 through 4,
    //       and they are always normal controllers
    if((int)hDevice >= 1 && (int)hDevice <= 4)
    {
        ZeroMemory(&pState->Gamepad, sizeof(pState->Gamepad));

        pState->Gamepad.sThumbLX = 10000;

        ret = ERROR_SUCCESS;
    }

    EmuXSwapFS();   // XBox FS

    return ret;
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
        printf("EmuXxapi (0x%.08X): EmuXCreateThread\n"
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
        printf("EmuXxapi (0x%.08X): EmuXCloseHandle\n"
               "(\n"
               "   hObject             : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), hObject);
    }
    #endif

    BOOL Ret = CloseHandle(hObject);

    EmuXSwapFS();   // XBox FS

    return Ret;
}

// ******************************************************************
// * func: EmuXapiInitProcess
// ******************************************************************
VOID WINAPI xboxkrnl::EmuXapiInitProcess()
{
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXxapi (0x%.08X): EmuXapiInitProcess();\n", GetCurrentThreadId());
    }
    #endif

    // TODO: Process initialization (if necessary)
	// TODO: Somehow initialize floating point

    EmuXSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuXapiBootDash
// ******************************************************************
VOID WINAPI xboxkrnl::EmuXapiBootDash(uint32 UnknownA, uint32 UnknownB, uint32 UnknownC)
{
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXxapi (0x%.08X): EmuXapiBootDash\n"
               "(\n"
               "   UnknownA            : 0x%.08X\n"
               "   UnknownB            : 0x%.08X\n"
               "   UnknownC            : 0x%.08X\n"
               ");\n",
                GetCurrentThreadId(), UnknownA, UnknownB, UnknownC);
    }
    #endif

    // TODO: Cleanly "Boot Dash" :-)
    exit(0);

    EmuXSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuX__rtinit
// ******************************************************************
VOID xboxkrnl::EmuX__rtinit()
{
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXxapi (0x%.08X): EmuX__rtinit();\n", GetCurrentThreadId());
    }
    #endif

    EmuXSwapFS();   // XBox FS
}

// ******************************************************************
// * func: EmuX__cinit
// ******************************************************************
VOID xboxkrnl::EmuX__cinit()
{
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXxapi (0x%.08X): EmuX__cinit();\n", GetCurrentThreadId());
    }
    #endif

    EmuXSwapFS();   // XBox FS
}