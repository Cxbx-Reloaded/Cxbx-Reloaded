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
namespace xntdll
{
    #include "xntdll.h"
};

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
// * Loaded at run-time to avoid linker conflicts
// ******************************************************************
static HMODULE hNtDll = GetModuleHandle("ntdll");

xntdll::FPTR_RtlCreateHeap                  NT_RtlCreateHeap                = (xntdll::FPTR_RtlCreateHeap)GetProcAddress(hNtDll, "RtlCreateHeap");
xntdll::FPTR_RtlAllocateHeap                NT_RtlAllocateHeap              = (xntdll::FPTR_RtlAllocateHeap)GetProcAddress(hNtDll, "RtlAllocateHeap");
xntdll::FPTR_RtlFreeHeap                    NT_RtlFreeHeap                  = (xntdll::FPTR_RtlFreeHeap)GetProcAddress(hNtDll, "RtlFreeHeap");

// ******************************************************************
// * func: EmuRtlCreateHeap
// ******************************************************************
PVOID WINAPI xapi::EmuRtlCreateHeap
(
    IN ULONG   Flags,
    IN PVOID   Base OPTIONAL,
    IN ULONG   Reserve OPTIONAL,
    IN ULONG   Commit,
    IN BOOLEAN Lock OPTIONAL,
    IN PVOID   RtlHeapParams OPTIONAL
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXapi (0x%X): EmuRtlCreateHeap\n"
               "(\n"
               "   Flags               : 0x%.08X\n"
               "   Base                : 0x%.08X\n"
               "   Reserve             : 0x%.08X\n"
               "   Commit              : 0x%.08X\n"
               "   Lock                : 0x%.08X\n"
               "   RtlHeapParams       : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Flags, Base, Reserve, Commit, Lock, RtlHeapParams);
    }
    #endif

    xntdll::RTL_HEAP_DEFINITION RtlHeapDefinition;

    ZeroMemory(&RtlHeapDefinition, sizeof(RtlHeapDefinition));

    RtlHeapDefinition.Length = sizeof(RtlHeapDefinition);

    PVOID pRet = NT_RtlCreateHeap(Flags, Base, Reserve, Commit, Lock, &RtlHeapDefinition);

    EmuSwapFS();   // XBox FS

    return pRet;
}

// ******************************************************************
// * func: EmuRtlAllocateHeap
// ******************************************************************
PVOID WINAPI xapi::EmuRtlAllocateHeap
(
    IN HANDLE hHeap,
    IN DWORD  dwFlags,
    IN SIZE_T dwBytes
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXapi (0x%X): EmuRtlAllocateHeap\n"
               "(\n"
               "   hHeap               : 0x%.08X\n"
               "   dwFlags             : 0x%.08X\n"
               "   dwBytes             : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), hHeap, dwFlags, dwBytes);
    }
    #endif

    PVOID pRet = NT_RtlAllocateHeap(hHeap, dwFlags, dwBytes);

    EmuSwapFS();   // XBox FS

    return pRet;
}

// ******************************************************************
// * func: EmuRtlFreeHeap
// ******************************************************************
BOOL WINAPI xapi::EmuRtlFreeHeap
(
    IN HANDLE hHeap,
    IN DWORD  dwFlags,
    IN PVOID  lpMem
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXapi (0x%X): EmuRtlFreeHeap\n"
               "(\n"
               "   hHeap               : 0x%.08X\n"
               "   dwFlags             : 0x%.08X\n"
               "   lpMem               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), hHeap, dwFlags, lpMem);
    }
    #endif

    BOOL bRet = NT_RtlFreeHeap(hHeap, dwFlags, lpMem);

    EmuSwapFS();   // XBox FS

    return bRet;
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
        printf("EmuXapi (0x%X): EmuXInitDevices\n"
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
        printf("EmuXapi (0x%X): EmuXGetDevices\n"
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
        printf("EmuXapi (0x%X): EmuXInputOpen\n"
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
        printf("EmuXapi (0x%X): EmuXInputGetCapabilities\n"
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
        printf("EmuXapi (0x%X): XInputGetState\n"
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
        printf("EmuXapi (0x%X): EmuCloseHandle\n"
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
        printf("EmuXapi (0x%X): EmuXapiInitProcess();\n", GetCurrentThreadId());
    }
    #endif

    // ******************************************************************
	// * Call RtlCreateHeap
    // ******************************************************************
	{
        RTL_HEAP_PARAMETERS HeapParameters;

		ZeroMemory(&HeapParameters, sizeof(HeapParameters));

        HeapParameters.Length = sizeof(HeapParameters);

		EmuSwapFS();   // XBox FS

		uint32 pHeapParameters = (uint32)&HeapParameters;
		uint32 dwPeHeapReserve = g_pXbeHeader->dwPeHeapReserve;
		uint32 dwPeHeapCommit  = g_pXbeHeader->dwPeHeapCommit;

        PVOID dwResult = 0;

        #define HEAP_GROWABLE                 0x00000002

        __asm
		{
            xor ecx, ecx

			push pHeapParameters
			push ecx
			push dwPeHeapCommit
			push dwPeHeapReserve
			push ecx
			push HEAP_GROWABLE
			call g_pRtlCreateHeap
			mov dwResult, eax
		}

        *xapi::EmuXapiProcessHeap = dwResult;
	}

    return;
}

// ******************************************************************
// * data: EmuXapiProcessHeap
// ******************************************************************
PVOID* xapi::EmuXapiProcessHeap;

// ******************************************************************
// * func: g_pRtlCreateHeap
// ******************************************************************
xapi::pfRtlCreateHeap xapi::g_pRtlCreateHeap;

// ******************************************************************
// * func: EmuXapiThreadStartup
// ******************************************************************
VOID WINAPI xapi::EmuXapiThreadStartup
(
    DWORD dwDummy1,
    DWORD dwDummy2
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXapi (0x%X): EmuXapiThreadStartup\n"
               "(\n"
               "   dwDummy1            : 0x%.08X\n"
               "   dwDummy2            : 0x%.08X\n"
               ");\n",
                GetCurrentThreadId(), dwDummy1, dwDummy2);
    }
    #endif

    EmuSwapFS();   // XBox FS

	// TODO: Call thread notify routines ?

    __asm
    {
        push dwDummy2
        call dwDummy1
    }

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
        printf("EmuXapi (0x%X): XapiSetupPerTitleDriveLetters\n"
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
        printf("EmuXapi (0x%X): EmuXapiBootDash\n"
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

