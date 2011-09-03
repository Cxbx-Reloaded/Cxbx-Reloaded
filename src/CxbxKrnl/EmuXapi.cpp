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
#define _XBOXKRNL_DEFEXTRN_

#undef FIELD_OFFSET     // prevent macro redefinition warnings
#include <windows.h>
//#include <xinput.h>

#include "CxbxUtil.h"
#include "CxbxKrnl.h"
#include "Emu.h"
#include "EmuFS.h"
#include "EmuAlloc.h"

// XInputSetState status waiters
extern XInputSetStateStatus g_pXInputSetStateStatus[XINPUT_SETSTATE_SLOTS] = {0};

// XInputOpen handles
extern HANDLE g_hInputHandle[XINPUT_HANDLE_SLOTS] = {0};

// Xbe section list
extern SectionList* g_pSectionList;
// Number of sections
extern int g_NumSections;

bool g_bXLaunchNewImageCalled = false;
bool g_bXInputOpenCalled = false;


// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace NtDll
{
    #include "EmuNtDll.h"
};

#include "EmuXTL.h"


XTL::POLLING_PARAMETERS_HANDLE g_pph;
XTL::XINPUT_POLLING_PARAMETERS g_pp;

// Saved launch data
XTL::LAUNCH_DATA g_SavedLaunchData;

// ******************************************************************
// * func: EmuXapiApplyKernelPatches
// ******************************************************************
VOID WINAPI XTL::EmuXapiApplyKernelPatches()
{
    #ifdef _DEBUG_TRACE
    EmuSwapFS();   // Win2k/XP FS
    DbgPrintf("EmuXapi (0x%X): EmuXapiApplyKernelPatches()\n", GetCurrentThreadId());
    EmuSwapFS();   // XBox FS
    #endif

    // we dont really feel like patching, now do we?

    return;
}

// ******************************************************************
// * func: EmuXFormatUtilityDrive
// ******************************************************************
BOOL WINAPI XTL::EmuXFormatUtilityDrive()
{
    #ifdef _DEBUG_TRACE
    EmuSwapFS();   // Win2k/XP FS
    DbgPrintf("EmuXapi (0x%X): EmuXFormatUtilityDrive()\n", GetCurrentThreadId());
    EmuSwapFS();   // XBox FS
    #endif

    // TODO: yeah... we'll format... riiiiight

    return TRUE;
}

// ******************************************************************
// * func: EmuGetTimeZoneInformation
// ******************************************************************
DWORD WINAPI XTL::EmuGetTimeZoneInformation
(
    OUT LPTIME_ZONE_INFORMATION lpTimeZoneInformation
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuGetTimeZoneInformation\n"
           "(\n"
           "   lpTimeZoneInformation : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), lpTimeZoneInformation);

    DWORD dwRet = GetTimeZoneInformation(lpTimeZoneInformation);

    EmuSwapFS();   // XBox FS

    return dwRet;
}

// ******************************************************************
// * func: EmuRtlCreateHeap
// ******************************************************************
PVOID WINAPI XTL::EmuRtlCreateHeap
(
    IN ULONG   Flags,
    IN PVOID   Base OPTIONAL,
    IN ULONG   Reserve OPTIONAL,
    IN ULONG   Commit,
    IN PVOID   Lock OPTIONAL,
    IN PVOID   RtlHeapParams OPTIONAL
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuRtlCreateHeap\n"
           "(\n"
           "   Flags               : 0x%.08X\n"
           "   Base                : 0x%.08X\n"
           "   Reserve             : 0x%.08X\n"
           "   Commit              : 0x%.08X\n"
           "   Lock                : 0x%.08X\n"
           "   RtlHeapParams       : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Flags, Base, Reserve, Commit, Lock, RtlHeapParams);

    NtDll::RTL_HEAP_DEFINITION RtlHeapDefinition;

    ZeroMemory(&RtlHeapDefinition, sizeof(RtlHeapDefinition));

    RtlHeapDefinition.Length = sizeof(RtlHeapDefinition);

    PVOID pRet = NtDll::RtlCreateHeap(Flags, Base, Reserve, Commit, Lock, &RtlHeapDefinition);

    EmuSwapFS();   // XBox FS

    return pRet;
}

// ******************************************************************
// * func: EmuRtlAllocateHeap
// ******************************************************************
PVOID WINAPI XTL::EmuRtlAllocateHeap
(
    IN HANDLE hHeap,
    IN DWORD  dwFlags,
    IN SIZE_T dwBytes
)
{
    EmuSwapFS();   // Win2k/XP FS

    //* too much debug output
    DbgPrintf("EmuXapi (0x%X): EmuRtlAllocateHeap\n"
           "(\n"
           "   hHeap               : 0x%.08X\n"
           "   dwFlags             : 0x%.08X\n"
           "   dwBytes             : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), hHeap, dwFlags, dwBytes);
    //*/

    BYTE offs;

    PVOID pRet = CxbxRtlAlloc(hHeap, dwFlags, dwBytes+0x20);

	if( pRet )
	{
		offs = (BYTE)(RoundUp((uint32)pRet, 0x20) - (uint32)pRet);

		if(offs == 0)
		{
			offs = 0x20;
		}

		pRet = (PVOID)((uint32)pRet + offs);

		*(BYTE*)((uint32)pRet - 1) = offs;

		DbgPrintf("pRet : 0x%.08X\n", pRet);
	}

	// JSRF test
//	if( !pRet ) __asm int 3;

    EmuSwapFS();   // XBox FS

    return pRet;
}

// ******************************************************************
// * func: EmuRtlFreeHeap
// ******************************************************************
BOOL WINAPI XTL::EmuRtlFreeHeap
(
    IN HANDLE hHeap,
    IN DWORD  dwFlags,
    IN PVOID  lpMem
)
{
    EmuSwapFS();   // Win2k/XP FS

    //* too much debug output
    DbgPrintf("EmuXapi (0x%X): EmuRtlFreeHeap\n"
           "(\n"
           "   hHeap               : 0x%.08X\n"
           "   dwFlags             : 0x%.08X\n"
           "   lpMem               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), hHeap, dwFlags, lpMem);
    //*/

    if(lpMem != NULL)
    {
        BYTE offs = *(BYTE*)((uint32)lpMem - 1);

        lpMem = (PVOID)((uint32)lpMem - offs);
    }

    BOOL bRet = CxbxRtlFree(hHeap, dwFlags, lpMem);

    EmuSwapFS();   // XBox FS

    return bRet;
}

// ******************************************************************
// * func: EmuRtlReAllocateHeap
// ******************************************************************
PVOID WINAPI XTL::EmuRtlReAllocateHeap
(
    IN HANDLE hHeap,
    IN DWORD  dwFlags,
    IN PVOID  lpMem,
    IN SIZE_T dwBytes
)
{
    EmuSwapFS();   // Win2k/XP FS

    //* too much debug output
    DbgPrintf("EmuXapi (0x%X): EmuRtlReAllocateHeap\n"
           "(\n"
           "   hHeap               : 0x%.08X\n"
           "   dwFlags             : 0x%.08X\n"
           "   lpMem               : 0x%.08X\n"
           "   dwBytes             : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), hHeap, dwFlags, lpMem, dwBytes);
   //*/

    BYTE offs;

    if(lpMem != NULL)
    {
        offs = *(BYTE*)((uint32)lpMem - 1);

        lpMem = (PVOID)((uint32)lpMem - offs);
    }

    PVOID pRet = CxbxRtlRealloc(hHeap, dwFlags, lpMem, dwBytes + 0x20);

    if(lpMem != NULL)
    {
        pRet = (PVOID)((uint32)pRet + offs);
    }

    DbgPrintf("pRet : 0x%.08X\n", pRet);

    EmuSwapFS();   // XBox FS

    return pRet;
}

// ******************************************************************
// * func: EmuRtlSizeHeap
// ******************************************************************
SIZE_T WINAPI XTL::EmuRtlSizeHeap
(
    IN HANDLE hHeap,
    IN DWORD  dwFlags,
    IN PVOID  lpMem
)
{
    EmuSwapFS();   // Win2k/XP FS

    //* too much debug output
    DbgPrintf("EmuXapi (0x%X): EmuRtlSizeHeap\n"
           "(\n"
           "   hHeap               : 0x%.08X\n"
           "   dwFlags             : 0x%.08X\n"
           "   lpMem               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), hHeap, dwFlags, lpMem);
    //*/

    if(lpMem != NULL)
    {
        BYTE offs = *(BYTE*)((uint32)lpMem - 1);

        lpMem = (PVOID)((uint32)lpMem - offs);
    }

    SIZE_T ret = CxbxRtlSizeHeap(hHeap, dwFlags, lpMem) - 0x20;

    EmuSwapFS();   // XBox FS

    return ret;
}

// ******************************************************************
// * func: EmuQueryPerformanceCounter
// ******************************************************************
BOOL WINAPI XTL::EmuQueryPerformanceCounter
(
    PLARGE_INTEGER lpPerformanceCount
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuQueryPerformanceCounter\n"
           "(\n"
           "   lpPerformanceCount  : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), lpPerformanceCount);

    BOOL bRet = QueryPerformanceCounter(lpPerformanceCount);

    // debug - 4x speed
    //lpPerformanceCount->QuadPart *= 4;

    EmuSwapFS();   // XBox FS

    return bRet;
}

// ******************************************************************
// * func: EmuQueryPerformanceFrequency
// ******************************************************************
BOOL WINAPI XTL::EmuQueryPerformanceFrequency
(
    PLARGE_INTEGER lpFrequency
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuQueryPerformanceFrequency\n"
           "(\n"
           "   lpFrequency         : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), lpFrequency);

    BOOL bRet = QueryPerformanceFrequency(lpFrequency);

    EmuSwapFS();   // XBox FS

    return bRet;
}

// ******************************************************************
// * func: EmuXMountUtilityDrive
// ******************************************************************
BOOL WINAPI XTL::EmuXMountUtilityDrive
(
    BOOL    fFormatClean
)
{
    #ifdef _DEBUG_TRACE
    {
        EmuSwapFS();   // Win2k/XP FS
        DbgPrintf("EmuXapi (0x%X): EmuXMountUtilityDrive\n"
               "(\n"
               "   fFormatClean        : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), fFormatClean);
        EmuSwapFS();   // XBox FS
    }
    #endif

    return TRUE;
}

// ******************************************************************
// * func: EmuXInitDevices
// ******************************************************************
VOID WINAPI XTL::EmuXInitDevices
(
    DWORD					dwPreallocTypeCount,
	PXDEVICE_PREALLOC_TYPE	PreallocTypes
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuXInitDevices\n"
           "(\n"
           "   dwPreallocTypeCount : 0x%.08X\n"
           "   PreallocTypes       : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), dwPreallocTypeCount, PreallocTypes);

	/*for( DWORD i = 0; i < dwPreallocTypeCount; i++ )
	{
		printf( "PreallocTypes[%d]: Device = 0x%.08X, 0x%.08X, 0x%.08X\n\tCount %d\n", i,
			PreallocTypes[i].DeviceType->Reserved[0],
			PreallocTypes[i].DeviceType->Reserved[1],
			PreallocTypes[i].DeviceType->Reserved[2], PreallocTypes[i].dwPreallocCount );
	}*/

    int v;

    for(v=0;v<XINPUT_SETSTATE_SLOTS;v++)
    {
        g_pXInputSetStateStatus[v].hDevice = 0;
        g_pXInputSetStateStatus[v].dwLatency = 0;
        g_pXInputSetStateStatus[v].pFeedback = 0;
    }

    for(v=0;v<XINPUT_HANDLE_SLOTS;v++)
    {
        g_hInputHandle[v] = 0;
    }

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuXGetDevices
// ******************************************************************
DWORD WINAPI XTL::EmuXGetDevices
(
    PXPP_DEVICE_TYPE DeviceType
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuXGetDevices\n"
           "(\n"
           "   DeviceType          : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), DeviceType);

    DWORD ret = 0;

    if(DeviceType->Reserved[0] == 0 && DeviceType->Reserved[1] == 0 && DeviceType->Reserved[2] == 0 && DeviceType->Reserved[3] == 0)
        ret = (1 << 0);    // Return 1 Controller
    else
        EmuWarning("Unknown DeviceType (0x%.08X, 0x%.08X, 0x%.08X, 0x%.08X)\n", DeviceType->Reserved[0], DeviceType->Reserved[1], DeviceType->Reserved[2], DeviceType->Reserved[3]);

    EmuSwapFS();   // XBox FS

    return ret;
}

// ******************************************************************
// * func: EmuXGetDeviceChanges
// ******************************************************************
BOOL WINAPI XTL::EmuXGetDeviceChanges
(
    PXPP_DEVICE_TYPE DeviceType,
    PDWORD           pdwInsertions,
    PDWORD           pdwRemovals
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuXGetDeviceChanges\n"
           "(\n"
           "   DeviceType          : 0x%.08X\n"
           "   pdwInsertions       : 0x%.08X\n"
           "   pdwRemovals         : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), DeviceType, pdwInsertions, pdwRemovals);

    BOOL bRet = FALSE;
    static BOOL bFirst = TRUE;

    // Return 1 Controller Inserted initially, then no changes forever
    if(bFirst)
    {
        if(DeviceType->Reserved[0] == 0 && DeviceType->Reserved[1] == 0 && DeviceType->Reserved[2] == 0 && DeviceType->Reserved[3] == 0)
		{
			*pdwInsertions = (1<<0);
			*pdwRemovals   = 0;
			bRet = TRUE;
			bFirst = FALSE;
		}
		else
		{
			// TODO: What if it's not a controller?
			EmuWarning("Unknown DeviceType (0x%.08X, 0x%.08X, 0x%.08X, 0x%.08X)\n", DeviceType->Reserved[0], DeviceType->Reserved[1], DeviceType->Reserved[2], DeviceType->Reserved[3]);
		}
    }
    else
    {
        *pdwInsertions = (1<<0); //0;
        *pdwRemovals   = 0;
    }

    EmuSwapFS();   // XBox FS

    return TRUE; //bRet;
}

// ******************************************************************
// * func: EmuXInputOpen
// ******************************************************************
HANDLE WINAPI XTL::EmuXInputOpen
(
    IN PXPP_DEVICE_TYPE             DeviceType,
    IN DWORD                        dwPort,
    IN DWORD                        dwSlot,
    IN PXINPUT_POLLING_PARAMETERS   pPollingParameters OPTIONAL
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuXInputOpen\n"
           "(\n"
           "   DeviceType          : 0x%.08X\n"
           "   dwPort              : 0x%.08X\n"
           "   dwSlot              : 0x%.08X\n"
           "   pPollingParameters  : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), DeviceType, dwPort, dwSlot, pPollingParameters);

    POLLING_PARAMETERS_HANDLE *pph = 0;

    if(dwPort >= 0 && (dwPort <= 3))
    {
        if(g_hInputHandle[dwPort] == 0)
        {
            pph = (POLLING_PARAMETERS_HANDLE*) &g_pph;	// new POLLING_PARAMETERS_HANDLE();

            if(pPollingParameters != NULL)
            {
                pph->pPollingParameters = (XINPUT_POLLING_PARAMETERS*) &g_pp; // new XINPUT_POLLING_PARAMETERS();

                memcpy(pph->pPollingParameters, pPollingParameters, sizeof(XINPUT_POLLING_PARAMETERS));
            }
            else
            {
                pph->pPollingParameters = NULL;
            }

            g_hInputHandle[dwPort] = pph;
        }
        else
        {
            pph = (POLLING_PARAMETERS_HANDLE*)g_hInputHandle[dwPort];

            if(pPollingParameters != 0)
            {
                if(pph->pPollingParameters == 0)
                {
                    pph->pPollingParameters = (XINPUT_POLLING_PARAMETERS*) &g_pp; // new XINPUT_POLLING_PARAMETERS();
                }

                memcpy(pph->pPollingParameters, pPollingParameters, sizeof(XINPUT_POLLING_PARAMETERS));
            }
            else
            {
                if(pph->pPollingParameters != 0)
                {
                    //delete pph->pPollingParameters;

                    pph->pPollingParameters = 0;
                }
            }
        }

        pph->dwPort = dwPort;
    }

	g_bXInputOpenCalled = true;

    EmuSwapFS();   // XBox FS

    return (HANDLE)pph;
}

// ******************************************************************
// * func: EmuXInputClose
// ******************************************************************
VOID WINAPI XTL::EmuXInputClose
(
    IN HANDLE hDevice
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuXInputClose\n"
           "(\n"
           "   hDevice             : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), hDevice);

    POLLING_PARAMETERS_HANDLE *pph = (POLLING_PARAMETERS_HANDLE*)hDevice;

    /* no longer necessary
    if(pph != NULL)
    {
        int v;

        for(v=0;v<XINPUT_SETSTATE_SLOTS;v++)
        {
            if(g_pXInputSetStateStatus[v].hDevice == hDevice)
            {
                // remove from slot
                g_pXInputSetStateStatus[v].hDevice = NULL;
                g_pXInputSetStateStatus[v].pFeedback = NULL;
                g_pXInputSetStateStatus[v].dwLatency = 0;
            }
        }

        if(pph->pPollingParameters != NULL)
        {
            delete pph->pPollingParameters;
        }

        delete pph;
    }
    //*/

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuXInputPoll
// ******************************************************************
DWORD WINAPI XTL::EmuXInputPoll
(
    IN HANDLE hDevice
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuXInputPoll\n"
           "(\n"
           "   hDevice             : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), hDevice);

    POLLING_PARAMETERS_HANDLE *pph = (POLLING_PARAMETERS_HANDLE*)hDevice;

    //
    // Poll input
    //

    {
        int v;

        for(v=0;v<XINPUT_SETSTATE_SLOTS;v++)
        {
            HANDLE hDevice = g_pXInputSetStateStatus[v].hDevice;

            if(hDevice == 0)
                continue;

            g_pXInputSetStateStatus[v].dwLatency = 0;

            XTL::PXINPUT_FEEDBACK pFeedback = (XTL::PXINPUT_FEEDBACK)g_pXInputSetStateStatus[v].pFeedback;

            if(pFeedback == 0)
                continue;

            //
            // Only update slot if it has not already been updated
            //

            if(pFeedback->Header.dwStatus != ERROR_SUCCESS)
            {
                if(pFeedback->Header.hEvent != 0)
                {
                    SetEvent(pFeedback->Header.hEvent);
                }

                pFeedback->Header.dwStatus = ERROR_SUCCESS;
            }
        }
    }

    EmuSwapFS();   // XBox FS

    return ERROR_SUCCESS;
}

// ******************************************************************
// * func: EmuXInputGetCapabilities
// ******************************************************************
DWORD WINAPI XTL::EmuXInputGetCapabilities
(
    IN  HANDLE               hDevice,
    OUT PXINPUT_CAPABILITIES pCapabilities
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuXInputGetCapabilities\n"
           "(\n"
           "   hDevice             : 0x%.08X\n"
           "   pCapabilities       : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), hDevice, pCapabilities);

    DWORD ret = ERROR_INVALID_HANDLE;

    POLLING_PARAMETERS_HANDLE *pph = (POLLING_PARAMETERS_HANDLE*)hDevice;

    if(pph != NULL)
    {
        DWORD dwPort = pph->dwPort;

        if((dwPort >= 0) && (dwPort <= 3))
        {
            pCapabilities->SubType = XINPUT_DEVSUBTYPE_GC_GAMEPAD;

            ZeroMemory(&pCapabilities->In.Gamepad, sizeof(pCapabilities->In.Gamepad));

            ret = ERROR_SUCCESS;
        }
    }

    EmuSwapFS();   // XBox FS

    return ret;
}

// ******************************************************************
// * func: EmuInputGetState
// ******************************************************************
DWORD WINAPI XTL::EmuXInputGetState
(
    IN  HANDLE         hDevice,
    OUT PXINPUT_STATE  pState
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuXInputGetState\n"
           "(\n"
           "   hDevice             : 0x%.08X\n"
           "   pState              : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), hDevice, pState);

    DWORD ret = ERROR_INVALID_HANDLE;

    POLLING_PARAMETERS_HANDLE *pph = (POLLING_PARAMETERS_HANDLE*)hDevice;

    if(pph != NULL)
    {
        if(pph->pPollingParameters != NULL)
        {
            if(pph->pPollingParameters->fAutoPoll == FALSE)
            {
                //
                // TODO: uh..
                //

                EmuWarning("EmuXInputGetState : fAutoPoll == FALSE");
            }
        }

        DWORD dwPort = pph->dwPort;

        if((dwPort >= 0) && (dwPort <= 3))
        {
			DbgPrintf( "EmuXInputGetState(): dwPort = %d\n", dwPort );

            if(dwPort == 0)
            {
                EmuDInputPoll(pState);
		//		EmuXInputPCPoll(pState);
                ret = ERROR_SUCCESS;
            }
        }
    }
	else
		EmuWarning( "EmuXInputGetState(): pph == NULL!" );

    EmuSwapFS();   // XBox FS

    return ret;
}

// ******************************************************************
// * func: EmuInputGetState
// ******************************************************************
DWORD WINAPI XTL::EmuXInputSetState
(
    IN     HANDLE           hDevice,
    IN OUT PXINPUT_FEEDBACK pFeedback
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuXInputSetState\n"
           "(\n"
           "   hDevice             : 0x%.08X\n"
           "   pFeedback           : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), hDevice, pFeedback);

    DWORD ret = ERROR_IO_PENDING;

    POLLING_PARAMETERS_HANDLE *pph = (POLLING_PARAMETERS_HANDLE*)hDevice;

    if(pph != NULL)
    {
        int v;

        //
        // Check if this device is already being polled
        //

        bool found = false;

        for(v=0;v<XINPUT_SETSTATE_SLOTS;v++)
        {
            if(g_pXInputSetStateStatus[v].hDevice == hDevice)
            {
                found = true;

                if(pFeedback->Header.dwStatus == ERROR_SUCCESS)
                {
                    ret = ERROR_SUCCESS;

                    // remove from slot
                    g_pXInputSetStateStatus[v].hDevice = NULL;
                    g_pXInputSetStateStatus[v].pFeedback = NULL;
                    g_pXInputSetStateStatus[v].dwLatency = 0;
                }
            }
        }

        //
        // If device was not already slotted, queue it
        //

        if(!found)
        {
            for(v=0;v<XINPUT_SETSTATE_SLOTS;v++)
            {
                if(g_pXInputSetStateStatus[v].hDevice == 0)
                {
                    g_pXInputSetStateStatus[v].hDevice = hDevice;
                    g_pXInputSetStateStatus[v].dwLatency = 0;
                    g_pXInputSetStateStatus[v].pFeedback = pFeedback;

                    pFeedback->Header.dwStatus = ERROR_IO_PENDING;

                    break;
                }
            }

            if(v == XINPUT_SETSTATE_SLOTS)
            {
                CxbxKrnlCleanup("Ran out of XInputSetStateStatus slots!");
            }
        }
    }

    EmuSwapFS();   // XBox FS

    return ret;
}

// ******************************************************************
// * func: EmuCreateMutex
// ******************************************************************
HANDLE WINAPI XTL::EmuCreateMutex
(
    LPSECURITY_ATTRIBUTES   lpMutexAttributes,
    BOOL                    bInitialOwner,
    LPCSTR                  lpName
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuCreateMutex\n"
           "(\n"
           "   lpMutexAttributes   : 0x%.08X\n"
           "   bInitialOwner       : 0x%.08X\n"
           "   lpName              : 0x%.08X (%s)\n"
           ");\n",
           GetCurrentThreadId(), lpMutexAttributes, bInitialOwner, lpName, lpName);

    HANDLE hRet = CreateMutex((SECURITY_ATTRIBUTES *)lpMutexAttributes, bInitialOwner, lpName);

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuCloseHandle
// ******************************************************************
BOOL WINAPI XTL::EmuCloseHandle
(
    HANDLE hObject
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuCloseHandle\n"
           "(\n"
           "   hObject             : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), hObject);

    BOOL bRet = CloseHandle(hObject);

    EmuSwapFS();   // XBox FS

    return bRet;
}

// ******************************************************************
// * func: EmuSetThreadPriorityBoost
// ******************************************************************
BOOL WINAPI XTL::EmuSetThreadPriorityBoost
(
    HANDLE  hThread,
    BOOL    DisablePriorityBoost
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuSetThreadPriorityBoost\n"
           "(\n"
           "   hThread             : 0x%.08X\n"
           "   DisablePriorityBoost: 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), hThread, DisablePriorityBoost);

    BOOL bRet = SetThreadPriorityBoost(hThread, DisablePriorityBoost);

    if(bRet == FALSE)
        EmuWarning("SetThreadPriorityBoost Failed!");

    EmuSwapFS();   // XBox FS

    return bRet;
}

// ******************************************************************
// * func: EmuSetThreadPriority
// ******************************************************************
BOOL WINAPI XTL::EmuSetThreadPriority
(
    HANDLE  hThread,
    int     nPriority
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuSetThreadPriority\n"
           "(\n"
           "   hThread             : 0x%.08X\n"
           "   nPriority           : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), hThread, nPriority);

    BOOL bRet = TRUE;//SetThreadPriority(hThread, nPriority);

    if(bRet == FALSE)
        EmuWarning("SetThreadPriority Failed!");

    // HACK!
    //Sleep(10);

    EmuSwapFS();   // XBox FS

    return bRet;
}


// ******************************************************************
// * func: EmuGetThreadPriority
// ******************************************************************
int WINAPI XTL::EmuGetThreadPriority
(
    HANDLE  hThread
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuGetThreadPriority\n"
           "(\n"
           "   hThread             : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), hThread);

    int iRet = GetThreadPriority(hThread);

    if(iRet == THREAD_PRIORITY_ERROR_RETURN)
        EmuWarning("GetThreadPriority Failed!");

    EmuSwapFS();   // XBox FS

    return iRet;
}

// ******************************************************************
// * func: EmuGetExitCodeThread
// ******************************************************************
BOOL WINAPI XTL::EmuGetExitCodeThread
(
    HANDLE  hThread,
    LPDWORD lpExitCode
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuGetExitCodeThread\n"
           "(\n"
           "   hThread             : 0x%.08X\n"
           "   lpExitCode          : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), hThread, lpExitCode);

    BOOL bRet = GetExitCodeThread(hThread, lpExitCode);

    EmuSwapFS();   // XBox FS

    return bRet;
}

// ******************************************************************
// * func: EmuXapiInitProcess
// ******************************************************************
VOID WINAPI XTL::EmuXapiInitProcess()
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuXapiInitProcess();\n", GetCurrentThreadId());

    // call RtlCreateHeap
    {
        RTL_HEAP_PARAMETERS HeapParameters;

        ZeroMemory(&HeapParameters, sizeof(HeapParameters));

        HeapParameters.Length = sizeof(HeapParameters);

        EmuSwapFS();   // XBox FS

        uint32 dwPeHeapReserve = CxbxKrnl_XbeHeader->dwPeHeapReserve;
        uint32 dwPeHeapCommit  = CxbxKrnl_XbeHeader->dwPeHeapCommit;

        PVOID dwResult = 0;

        #define HEAP_GROWABLE 0x00000002

        *XTL::EmuXapiProcessHeap = XTL::g_pRtlCreateHeap(HEAP_GROWABLE, 0, dwPeHeapReserve, dwPeHeapCommit, NULL, &HeapParameters);
    }

    return;
}

// ******************************************************************
// * data: EmuXapiProcessHeap
// ******************************************************************
PVOID* XTL::EmuXapiProcessHeap;

// ******************************************************************
// * func: g_pRtlCreateHeap
// ******************************************************************
XTL::pfRtlCreateHeap XTL::g_pRtlCreateHeap;

// ******************************************************************
// * func: EmuXapiThreadStartup
// ******************************************************************
VOID WINAPI XTL::EmuXapiThreadStartup
(
    DWORD dwDummy1,
    DWORD dwDummy2
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuXapiThreadStartup\n"
           "(\n"
           "   dwDummy1            : 0x%.08X\n"
           "   dwDummy2            : 0x%.08X\n"
           ");\n",
            GetCurrentThreadId(), dwDummy1, dwDummy2);

    EmuSwapFS();   // XBox FS

    typedef int (__stdcall *pfDummyFunc)(DWORD dwDummy);

    pfDummyFunc func = (pfDummyFunc)dwDummy1;

    func(dwDummy2);

    // TODO: Call thread notify routines ?

    /*
    __asm
    {
        push dwDummy2
        call dwDummy1
    }
    */

    //_asm int 3;

    return;
}

/* Too High Level!
// ******************************************************************
// * func: XapiSetupPerTitleDriveLetters
// ******************************************************************
XTL::NTSTATUS CDECL XTL::XapiSetupPerTitleDriveLetters(DWORD dwTitleId, LPCWSTR wszTitleName)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): XapiSetupPerTitleDriveLetters\n"
           "(\n"
           "   dwTitleId           : 0x%.08X\n"
           "   wszTitleName        : 0x%.08X\n"
           ");\n",
            GetCurrentThreadId(), dwTitleId, wszTitleName);

    NTSTATUS ret = STATUS_SUCCESS;

    EmuSwapFS();   // XBox FS

    return ret;
}
*/
// ******************************************************************
// * func: EmuXapiBootDash
// ******************************************************************
VOID WINAPI XTL::EmuXapiBootDash(DWORD UnknownA, DWORD UnknownB, DWORD UnknownC)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuXapiBootDash\n"
           "(\n"
           "   UnknownA            : 0x%.08X\n"
           "   UnknownB            : 0x%.08X\n"
           "   UnknownC            : 0x%.08X\n"
           ");\n",
            GetCurrentThreadId(), UnknownA, UnknownB, UnknownC);

    CxbxKrnlCleanup("Emulation Terminated (XapiBootDash)");

    EmuSwapFS();   // XBox FS

    return;
}

// ******************************************************************
// * func: EmuXRegisterThreadNotifyRoutine
// ******************************************************************
VOID WINAPI XTL::EmuXRegisterThreadNotifyRoutine
(
    PXTHREAD_NOTIFICATION   pThreadNotification,
    BOOL                    fRegister
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuXRegisterThreadNotifyRoutine\n"
           "(\n"
           "   pThreadNotification : 0x%.08X (0x%.08X)\n"
           "   fRegister           : 0x%.08X\n"
           ");\n",
            GetCurrentThreadId(), pThreadNotification, pThreadNotification->pfnNotifyRoutine, fRegister);

    if(fRegister)
    {
		// I honestly don't expect this to happen, but if it does...
        if(g_iThreadNotificationCount >= 16)
			CxbxKrnlCleanup("Too many thread notification routines installed\n"
							"If you're reading this message than tell blueshogun you saw it!!!");

		// Find an empty spot in the thread notification array
		for(int i = 0; i < 16; i++)
		{
			// If we find one, then add it to the array, and break the loop so
			// that we don't accidently register the same routine twice!
			if(g_pfnThreadNotification[i] == NULL)
			{
				g_pfnThreadNotification[i] = pThreadNotification->pfnNotifyRoutine;				
				g_iThreadNotificationCount++;
				break;
			}
		}
    }
    else
    {
		// Go through each routine and nullify the routine passed in.
        for(int i = 0; i < 16; i++)
		{
			if(pThreadNotification->pfnNotifyRoutine == g_pfnThreadNotification[i])
			{
				g_pfnThreadNotification[i] = NULL;
				g_iThreadNotificationCount--;
				break;
			}
		}
    }

    EmuSwapFS();   // XBox FS
}

// ******************************************************************
// * func: EmuCreateFiber
// ******************************************************************
LPVOID WINAPI XTL::EmuCreateFiber
(
	DWORD					dwStackSize,
	LPFIBER_START_ROUTINE	lpStartRoutine,
	LPVOID					lpParameter
)
{
	EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuCreateFiber\n"
           "(\n"
		   "   dwStackSize         : 0x%.08X\n"
           "   lpStartRoutine      : 0x%.08X\n"
           "   lpParameter         : 0x%.08X\n"
           ");\n",
            GetCurrentThreadId(), dwStackSize, lpStartRoutine, lpParameter);

	LPVOID pFiber = CreateFiber( dwStackSize, lpStartRoutine, lpParameter );
	if( !pFiber )
		EmuWarning( "CreateFiber failed!" );

	EmuSwapFS();	// Xbox FS

	return pFiber;
}

// ******************************************************************
// * func: EmuDeleteFiber
// ******************************************************************
VOID WINAPI XTL::EmuDeleteFiber
(
	LPVOID					lpFiber
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuDeleteFiber\n"
			"(\n"
			"	lpFiber            : 0x%.08X\n"
			");\n",
			GetCurrentThreadId(), lpFiber );

	DeleteFiber( lpFiber );

	EmuSwapFS();	// Xbox FS
}

// ******************************************************************
// * func: EmuLoadSectionA
// ******************************************************************
LPVOID WINAPI XTL::EmuXLoadSectionA
(
	LPCSTR					pSectionName
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuXLoadSectionA\n"
			"(\n"
			"   pSectionName       : \"%s\"\n"
			");\n",
			GetCurrentThreadId(), pSectionName, pSectionName );

	// TODO: Search this .xbe for the section it wants to load.
	// If we find it, return the address of it.
	LPVOID pRet = NULL;

	// Xbox Dashboard (3944)
	if(strcmp("XIPS", pSectionName)==0)
		pRet = (void*) 0x13B500;

	// Blade II NTSC
	else if(!strcmp(pSectionName, "DSPImage"))
		pRet = (void*) 0x41F900;

	// Zapper (NTSC)
//	else if(!strcmp(pSectionName, "sig"))
//		pRet = (void*) 0x41F900;

	else
		__asm int 3;

	EmuSwapFS();	// Xbox FS

	return pRet;
}

// ******************************************************************
// * func: EmuXFreeSectionA
// ******************************************************************
BOOL WINAPI XTL::EmuXFreeSectionA
(
	LPCSTR					pSectionName
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuXFreeSectionA\n"
			"(\n"
			"   pSectionName       : \"%s\"\n"
			");\n",
			GetCurrentThreadId(), pSectionName, pSectionName );

	// TODO: Implement (if necessary)?
//	CxbxKrnlCleanup( "XFreeSectionA is not implemented" );

	EmuSwapFS();	// Xbox FS

	return TRUE;
}

// ******************************************************************
// * func: EmuXGetSectionHandleA
// ******************************************************************
HANDLE WINAPI XTL::EmuXGetSectionHandleA
(
	LPCSTR					pSectionName
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuXGetSectionHandleA\n"
			"(\n"
			"   pSectionName       : \"%s\"\n"
			");\n",
			GetCurrentThreadId(), pSectionName, pSectionName );

	void* pRet = NULL;

	// TODO: Implement (if necessary)?
//	CxbxKrnlCleanup( "XGetSectionHandleA is not implemented" );

	// TODO: Save the name and address of each section contained in 
	// this .xbe instead of adding this stuff by hand because the section
	// address can change from one game region to the next, and some games
	// will use the same engine and section name, so accuracy is not
	// guarunteed.

	// Metal Gear Solid II (NTSC)
	if( !strcmp( pSectionName, "Rev24b" ) )
	{
		pRet = (void*) 0xCE3E60;
	}

	// Metal Slug 3 (NTSC)
	else if(!strcmp(pSectionName, "newpal"))
	{
		pRet = (void*) 0x2C79E0;
	}
	else if(!strcmp(pSectionName, "msg_font"))
	{
		pRet = (void*) 0x2CABC0;
	}
	else if(!strcmp(pSectionName, "se_all"))
	{
		pRet = (void*) 0x2CBD20;
	}
	else if(!strcmp(pSectionName, ".XTLID"))
	{
//		__asm int 3;
		pRet = (void*) 0x91B0A0;
	}

	// Zapper (NTSC)
	else if(!strcmp(pSectionName, "SYSFONT"))
	{
		pRet = (void*) 0x2CEC80;
	}

	// Conflict: Desert Storm (NTSC)
	else if(!strcmp(pSectionName, "FONTIMG"))
	{
		pRet = (void*) 0x364580;
	}
	else if(!strcmp(pSectionName, "FONTT0"))
	{
		pRet = (void*) 0x37E620;
	}
	else if(!strcmp(pSectionName, "FONTT1"))
	{
		pRet = (void*) 0x38E6A0;
	}

	// Taz Wanted (NTSC)
	/*else if(!strcmp(pSectionName, "sig" ))
	{
		pRet = (void*) 0x*/

	// Red Star (Unknown Region)
	/*else if(!strcmp(pSectionName, "BINK32"))
	{
		pRet = (void*) 0x00366F60;
	}*/

	else
	{
//		__asm int 3;
	}

	EmuSwapFS();	// Xbox FS

	return (LPVOID) pRet;
}

// ******************************************************************
// * func: EmuXLoadSectionByHandle
// ******************************************************************
LPVOID WINAPI XTL::EmuXLoadSectionByHandle
(
	HANDLE					hSection
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuXLoadSectionByHandle\n"
			"(\n"
			"   hSection           : 0x%.08X\n"
			");\n",
			GetCurrentThreadId(), hSection );

	// The handle should contain the address of this section by the hack
	// used in EmuXGetSectionHandleA.

	EmuSwapFS();	// Xbox FS

	return (LPVOID) hSection;
}

// ******************************************************************
// * func: EmuXFreeSectionByHandle
// ******************************************************************
BOOL WINAPI XTL::EmuXFreeSectionByHandle
(
	HANDLE					hSection
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuXFreeSectionByHandle\n"
			"(\n"
			"   hSection           : 0x%.08X\n"
			");\n",
			GetCurrentThreadId(), hSection );

	// TODO: Implement (if necessary)?
//	CxbxKrnlCleanup( "XLoadSectionByHandle is not implemented" );

	EmuSwapFS();	// Xbox FS

	return TRUE;
}

// ******************************************************************
// * func: EmuXGetSectionSize
// ******************************************************************
DWORD WINAPI XTL::EmuXGetSectionSize
(
	HANDLE hSection                       
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuXGetSectionSize\n"
			"(\n"
			"   hSection           : 0x%.08X\n"
			");\n",
			GetCurrentThreadId(), hSection );

	DWORD dwSize = 0;

	// Metal Slug 3 (NTSC)
	if(hSection == (HANDLE) 0x26C000)	// newpal
		dwSize = 0x31DA;
	else if(hSection == (HANDLE) 0x270000)	// msg_fong
		dwSize = 0x115F;
	else if(hSection == (HANDLE) 0x272000)	// se_all
		dwSize = 0x64F37E;
	else if(hSection == (HANDLE) 0x8C5000)	// .XTLID
		dwSize = 0x480;

	EmuSwapFS();

	return dwSize;
}


// ******************************************************************
// * func: EmuRtlDestroyHeap
// ******************************************************************
PVOID WINAPI XTL::EmuRtlDestroyHeap
(
    IN HANDLE HeapHandle
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuRtlDestroyHeap\n"
			"(\n"
			"   HeapHandle         : 0x%.08X\n"
			");\n",
			GetCurrentThreadId(), HeapHandle );

	PVOID pRet = NtDll::RtlDestroyHeap( HeapHandle );

	EmuSwapFS();	// Xbox FS

	return pRet;
}

// ******************************************************************
// * func: EmuQueueUserAPC
// ******************************************************************
DWORD WINAPI XTL::EmuQueueUserAPC
(
	PAPCFUNC	pfnAPC,
	HANDLE		hThread,
	DWORD		dwData
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuQueueUserAPC\n"
			"(\n"
			"   pfnAPC           : 0x%.08X\n"
			"   hThread          : 0x%.08X\n"
			"   dwData           : 0x%.08X\n"
			");\n",
			GetCurrentThreadId(), pfnAPC, hThread, dwData);

	DWORD dwRet = 0;

	// If necessary, we can just continue to emulate NtQueueApcThread (0xCE).
	// I added this because NtQueueApcThread fails in Metal Slug 3.

	HANDLE hApcThread = NULL;
	if(!DuplicateHandle(GetCurrentProcess(),hThread,GetCurrentProcess(),&hApcThread,THREAD_SET_CONTEXT,FALSE,0))
		EmuWarning("DuplicateHandle failed!");

	dwRet = QueueUserAPC(pfnAPC, hApcThread, dwData);
	if(!dwRet)
		EmuWarning("QueueUserAPC failed!");

	EmuSwapFS();	// Xbox FS

	return dwRet;
}

// ******************************************************************
// * func: EmuGetOverlappedResult
// ******************************************************************
BOOL WINAPI XTL::EmuGetOverlappedResult
(
	HANDLE			hFile,
	LPOVERLAPPED	lpOverlapped,
	LPDWORD			lpNumberOfBytesTransferred,
	BOOL			bWait
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuGetOverlappedResult\n"
			"(\n"
			"   hFile            : 0x%.08X\n"
			"   lpOverlapped     : 0x%.08X\n"
			"   lpNumberOfBytesTransformed : 0x%.08X\n"
			"   bWait            : 0x%.08X\n"
			");\n",
			GetCurrentThreadId(), hFile, lpOverlapped, lpNumberOfBytesTransferred, bWait);

	BOOL bRet = GetOverlappedResult( hFile, lpOverlapped, lpNumberOfBytesTransferred, bWait );

//	if(bWait)
//		bRet = TRUE; // Sucker...

	EmuSwapFS();	// Xbox FS

	return bRet;
}

// ******************************************************************
// * func: EmuXLaunchNewImage
// ******************************************************************
DWORD WINAPI XTL::EmuXLaunchNewImage
(
	LPCSTR			lpTitlePath,
	PLAUNCH_DATA	pLaunchData
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuXLaunchNewImage\n"
			"(\n"
			"   lpTitlePath      : 0x%.08X (%s)\n"
			"   pLaunchData      : 0x%.08X\n"
			");\n",
			GetCurrentThreadId(), lpTitlePath, lpTitlePath, pLaunchData);

	// If this function succeeds, it doesn't get a chance to return anything.
	DWORD dwRet = ERROR_GEN_FAILURE;

	// If no path is specified, then the xbe is rebooting to dashboard
	if(!lpTitlePath)
		CxbxKrnlCleanup("The xbe is rebooting (XLaunchNewImage)");

	// Ignore any other attempts to execute other .xbe files (for now).
	EmuWarning("Not executing the xbe!");
//	CxbxKrnlCleanup("XLaunchNewImage(): Attempting to launch %s", lpTitlePath);

	// Save the launch data
	if(pLaunchData != NULL)
	{
		CopyMemory(&g_SavedLaunchData, pLaunchData, sizeof(LAUNCH_DATA));

		// Save the launch data parameters to disk for later.
		DbgPrintf("Saving launch data as CxbxLaunchData.bin...\n");

		FILE* fp = fopen("CxbxLaunchData.bin", "wb");

		fseek(fp, 0, SEEK_SET);
		fwrite(pLaunchData, sizeof( LAUNCH_DATA ), 1, fp);
		fclose(fp);
	}

	g_bXLaunchNewImageCalled = true;

	// Hey, let's try executing the .exe instead of the .xbe!
	/*{
		char* szExePath = (char*) lpTitlePath;
		int len = strlen( lpTitlePath );

		strcpy( szExePath, lpTitlePath );
		szExePath[len-3] = 'e';
		szExePath[len-2] = 'x';
		szExePath[len-1] = 'e';
		
		if( szExePath[0] == 'D' && szExePath[1] == ':' && szExePath[2] == '\\' )
			szExePath += 3;

		DbgPrintf( "Attempting to execute %s instead of the equivelant .xbe\n", szExePath );

		if((int)ShellExecute(NULL, "open", szExePath, NULL, ".\\", SW_SHOWDEFAULT) <= 32)
		{
			EmuWarning( "Could not launch %s", szExePath );
		}
		else
		{
			CxbxKrnlCleanup( "New emulation session has begun.\nTODO: Use a more graceful method..." );
		}
	}*/

	// Temporary Hack (Unreal): Jump back to the entry point
//	uint32* start = (uint32*) 0x21C13B;

	EmuSwapFS();	// Xbox FS

	/*__asm
	{
		mov esi, 0;
		mov edi, 0;
		mov esp, 0;
		mov ebp, 0;
		jmp start;
	}*/

	return dwRet;
}

// ******************************************************************
// * func: EmuXGetLaunchInfo
// ******************************************************************
DWORD WINAPI XTL::EmuXGetLaunchInfo
(
	PDWORD			pdwLaunchDataType,
	PLAUNCH_DATA	pLaunchData
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuXGetLaunchInfo\n"
			"(\n"
			"   pdwLaunchDataType : 0x%.08X\n"
			"   pLaunchData       : 0x%.08X\n"
			");\n",
			GetCurrentThreadId(), pdwLaunchDataType, pLaunchData);
	
	DWORD dwRet = E_FAIL;

	// Has XLaunchNewImage been called since we've started this round?
	if(g_bXLaunchNewImageCalled)
	{
		// I don't think we'll be emulating any other xbox apps
    	// other than games anytime soon...
    	*pdwLaunchDataType = LDT_TITLE; 

		// Copy saved launch data
		CopyMemory(pLaunchData, &g_SavedLaunchData, sizeof(LAUNCH_DATA));

		dwRet = ERROR_SUCCESS;
	}

	FILE* fp = NULL;

	// Does CxbxLaunchData.bin exist?
	if(!g_bXLaunchNewImageCalled)
		fp = fopen("CxbxLaunchData.bin", "rb");

	// If it does exist, load it.
	if(fp)
	{
		// Data from Xbox game
		*pdwLaunchDataType = LDT_TITLE; 

		// Read in the contents.
		fseek(fp, 0, SEEK_SET);
		fread(&g_SavedLaunchData, sizeof(LAUNCH_DATA), 1, fp);
		memcpy(pLaunchData, &g_SavedLaunchData, sizeof(LAUNCH_DATA));
		fclose(fp);

		// Delete the file once we're done.
		DeleteFile("CxbxLaunchData.bin");

		// HACK: Initialize XInput from restart
		/*if(g_bXInputOpenCalled)
		{
			EmuSwapFS();
			XTL::EmuXInputOpen( NULL, 0, 0, NULL );
			EmuSwapFS();
		}*/

		dwRet = ERROR_SUCCESS;
	}

	EmuSwapFS();	// Xbox FS

	return dwRet;
}

// ******************************************************************
// * func: EmuXSetProcessQuantumLength
// ******************************************************************
VOID WINAPI XTL::EmuXSetProcessQuantumLength
(
    DWORD dwMilliseconds
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuXSetProcessQuantumLength\n"
			"(\n"
			"   dwMilliseconds    : 0x%.08X\n"
			");\n",
			GetCurrentThreadId(), dwMilliseconds);

	// TODO: Implement?
	EmuWarning("XSetProcessQuantumLength is being ignored!");

	EmuSwapFS();	// Xbox FS
}
	
// ******************************************************************
// * func: EmuXGetFileCacheSize
// ******************************************************************
DWORD WINAPI XTL::EmuXGetFileCacheSize()
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuXGetFileCacheSize()\n", GetCurrentThreadId());

	// Return the default cache size for now.
	// TODO: Save the file cache size if/when set.
	DWORD dwRet = 64 * 1024;

	EmuSwapFS();

	return dwRet;
}

// ******************************************************************
// * func: EmuSignalObjectAndWait
// ******************************************************************
DWORD WINAPI XTL::EmuSignalObjectAndWait
(
	HANDLE	hObjectToSignal,
	HANDLE	hObjectToWaitOn,
	DWORD	dwMilliseconds,
	BOOL	bAlertable
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuSignalObjectAndWait\n"
			"(\n"
			"   hObjectToSignal   : 0x%.08X\n"
			"   hObjectToWaitOn   : 0x%.08X\n"
			"   dwMilliseconds    : 0x%.08X\n"
			"   bAlertable        : 0x%.08X\n"
			");\n",
			GetCurrentThreadId(), hObjectToSignal, hObjectToWaitOn, dwMilliseconds, bAlertable);

	DWORD dwRet = SignalObjectAndWait( hObjectToSignal, hObjectToWaitOn, dwMilliseconds, bAlertable ); 

	EmuSwapFS();	// Xbox FS

	return dwRet;
}

// ******************************************************************
// * func: EmuPulseEvent
// ******************************************************************
BOOL WINAPI XTL::EmuPulseEvent( HANDLE hEvent )
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuPulseEvent\n"
			"(\n"
			"   hEvent            : 0x%.08X\n"
			");\n",
			GetCurrentThreadId(), hEvent);

	// TODO: This function might be a bit too high level.  If it is,
	// feel free to implement NtPulseEvent in EmuKrnl.cpp

	BOOL bRet = PulseEvent( hEvent );

	EmuSwapFS();	// Xbox FS

	return bRet;
}

// ******************************************************************
// * func: EmuCreateSemaphore
// ******************************************************************
HANDLE WINAPI XTL::EmuCreateSemaphore
(
	LPVOID	lpSemaphoreAttributes, 
	LONG	lInitialCount,
	LONG	lMaximumCount,
	LPSTR	lpName
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuCreateSemaphore\n"
			"(\n"
			"   lpSemaphoreAttributes : 0x%.08X\n"
			"   lInitialCount         : 0x%.08X\n"
			"   lMaximumCount         : 0x%.08X\n"
			"   lpName                : 0x%.08X (%s)\n"
			");\n",
			GetCurrentThreadId(), lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName);

	if(lpSemaphoreAttributes)
		EmuWarning( "lpSemaphoreAttributes != NULL" );

	HANDLE hRet = CreateSemaphoreA( NULL, lInitialCount, lMaximumCount, lpName );
	if( !hRet )
		EmuWarning( "CreateSemaphore failed!" );

	EmuSwapFS();	// Xbox FS

	return hRet;
}

// ******************************************************************
// * func: EmuReleaseSemaphore
// ******************************************************************
BOOL WINAPI XTL::EmuReleaseSemaphore
(
	HANDLE	hSemaphore,
	LONG	lReleaseCount,
	LPLONG	lpPreviousCount
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuReleaseSemaphore\n"
			"(\n"
			"   hSemaphore        : 0x%.08X\n"
			"   lReleaseCount     : 0x%.08X\n"
			"   lpPreviousCount   : 0x%.08X\n"
			");\n",
			GetCurrentThreadId(), hSemaphore, lReleaseCount, lpPreviousCount);

	BOOL bRet = ReleaseSemaphore( hSemaphore, lReleaseCount, lpPreviousCount );
	if( !bRet )
		EmuWarning( "ReleaseSemaphore failed!" );

	EmuSwapFS();	// Xbox FS

	return bRet;
}

// ******************************************************************
// * func: timeSetEvent
// ******************************************************************
MMRESULT WINAPI XTL::EmutimeSetEvent
(
	UINT			uDelay,
	UINT			uResolution,
	LPTIMECALLBACK	fptc,
	DWORD			dwUser,
	UINT			fuEvent
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmutimeSetEvent\n"
			"(\n"
			"   uDelay            : 0x%.08X\n"
			"   uResolution       : 0x%.08X\n"
			"   fptc              : 0x%.08X\n"
			"   dwUser            : 0x%.08X\n"
			"   fuEvent           : 0x%.08X\n"
			");\n",
			GetCurrentThreadId(), uDelay, uResolution, fptc, dwUser, fuEvent);

	MMRESULT Ret = timeSetEvent( uDelay, uResolution, fptc, (DWORD_PTR) dwUser, fuEvent );

	EmuSwapFS();	// Xbox FS

	return Ret;
}

// ******************************************************************
// * func: timeKillEvent
// ******************************************************************
MMRESULT WINAPI XTL::EmutimeKillEvent
(
	UINT uTimerID  
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuReleaseSemaphore\n"
			"(\n"
			"   uTimerID          : 0x%.08X\n"
			");\n",
			GetCurrentThreadId(), uTimerID);

	MMRESULT Ret = timeKillEvent( uTimerID );

	EmuSwapFS();	// Xbox FS

	return Ret;
}

// ******************************************************************
// * func: EmuRaiseException
// ******************************************************************
VOID WINAPI XTL::EmuRaiseException
(
	DWORD			dwExceptionCode,       // exception code
	DWORD			dwExceptionFlags,      // continuable exception flag
	DWORD			nNumberOfArguments,    // number of arguments
	CONST ULONG_PTR *lpArguments		   // array of arguments
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuRaiseException\n"
			"(\n"
			"   dwExceptionCode   : 0x%.08X\n"
			"   dwExceptionFlags  : 0x%.08X\n"
			"   nNumberOfArguments: 0x%.08X\n"
			"   lpArguments       : 0x%.08X\n"
			");\n",
			GetCurrentThreadId(), dwExceptionCode, dwExceptionFlags, nNumberOfArguments, lpArguments);

	// TODO: Implement or not?
//	RaiseException(dwExceptionCode, dwExceptionFlags, nNumberOfArguments, (*(ULONG_PTR**) &lpArguments));

	EmuSwapFS();	// Xbox FS
}

// ******************************************************************
// * func: EmuGetFileAttributesA
// ******************************************************************
DWORD WINAPI XTL::EmuGetFileAttributesA
(
	LPCSTR			lpFileName    // name of file or directory
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuGetFileAttributesA\n"
			"(\n"
			"   lpFileName        : (%s)\n"
			");\n", 
			GetCurrentThreadId(), lpFileName);

	// Dues Ex...

	// Shave off the D:\ and default to the current directory.
	// TODO: Other directories (i.e. Utility)?

	char* szBuffer = (char*) lpFileName;

	if((szBuffer[0] == 'D' || szBuffer[0] == 'd') && szBuffer[1] == ':' || szBuffer[2] == '\\')
	{
		szBuffer += 3;

		 DbgPrintf("EmuXapi (0x%X): GetFileAttributesA Corrected path...\n", GetCurrentThreadId());
         DbgPrintf("  Org:\"%s\"\n", lpFileName);
         DbgPrintf("  New:\"$XbePath\\%s\"\n", szBuffer);
    }

	DWORD dwRet = GetFileAttributesA(szBuffer);
	if(FAILED(dwRet))
		EmuWarning("GetFileAttributes failed!");

	EmuSwapFS();

	return dwRet;
}

// ******************************************************************
// func: EmuVirtualProtect
// ******************************************************************
BOOL WINAPI XTL::EmuVirtualProtect
(
	LPVOID	lpAddress,       // region of committed pages
	SIZE_T	dwSize,          // size of the region
	DWORD	flNewProtect,    // desired access protection
	PDWORD	lpflOldProtect   // old protection
)
{
	EmuSwapFS();

	DbgPrintf("EmuXapi (0x%X): EmuVirtualProtect\n"
			"(\n"
			"   lpAddress         : 0x%.08X\n"
			"   dwSize            : 0x%.08X\n"
			"   flNewProtect      : 0x%.08X\n"
			"   lpflOldProtect    : 0x%.08X\n"
			");\n", 
			GetCurrentThreadId(), lpAddress, dwSize, flNewProtect, lpflOldProtect);

	DWORD dwRet = VirtualProtect(lpAddress, dwSize, flNewProtect, lpflOldProtect);

	EmuSwapFS();

	return dwRet;
}

// ******************************************************************
// func: EmulstrcmpiW
// ******************************************************************
int WINAPI XTL::EmulstrcmpiW
(
	LPCWSTR lpString1,
	LPCWSTR lpString2
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmulstrcmpiW\n"
			"(\n"
			"   lpString1         : 0x%.08X (%S)\n"
			"   lpString2         : 0x%.08X (%S)\n"
			");\n", 
			GetCurrentThreadId(), lpString1, lpString1, lpString2, lpString2);

	int Ret = lstrcmpiW(lpString1, lpString2);

	EmuSwapFS();	// Xbox FS

	return Ret;
}

// ******************************************************************
// func: XMountMUA
// ******************************************************************
DWORD WINAPI XTL::EmuXMountMUA
(
	DWORD dwPort,                  
	DWORD dwSlot,                  
	PCHAR pchDrive               
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuXMountMUA\n"
			"(\n"
			"   dwPort            : 0x%.08X\n"
			"   dwSlot            : 0x%.08X\n"
			"   pchDrive          : 0x%.08X (%s)\n"
			");\n", 
			GetCurrentThreadId(), dwPort, dwSlot, pchDrive, pchDrive);

	// TODO: Actually allow memory card emulation? This might make transferring
	// game saves a bit easier if the memory card directory was configurable. =]

	EmuSwapFS();	// Xbox FS

	return E_FAIL;
}

// ******************************************************************
// func: EmuCreateWaitableTimer
// ******************************************************************
HANDLE WINAPI XTL::EmuCreateWaitableTimerA
(
	LPVOID					lpTimerAttributes, // SD
	BOOL					bManualReset,      // reset type
	LPCSTR					lpTimerName        // object name
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuCreateWaitableTimerA\n"
			"(\n"
			"   lpTimerAttributes : 0x%.08X\n"
			"   bManualReset      : 0x%.08X\n"
			"   lpTimerName       : 0x%.08X (%s)\n"
			");\n", 
			GetCurrentThreadId(), lpTimerAttributes, bManualReset, lpTimerName, lpTimerName);

	// For Xbox titles, this param should always be NULL.
	if(lpTimerAttributes)
		EmuWarning("lpTimerAttributes != NULL");

	HANDLE hRet = CreateWaitableTimerA( NULL, bManualReset, lpTimerName );

	EmuSwapFS();	// Xbox FS

	return hRet;
}

// ******************************************************************
// func: EmuSetWaitableTimer
// ******************************************************************
BOOL WINAPI XTL::EmuSetWaitableTimer
(
	HANDLE				hTimer,                     // handle to timer
	const LARGE_INTEGER *pDueTime,					// timer due time
	LONG				lPeriod,                    // timer interval
	PTIMERAPCROUTINE	pfnCompletionRoutine,		// completion routine
	LPVOID				lpArgToCompletionRoutine,   // completion routine parameter
	BOOL				fResume                     // resume state
)
{
	
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuSetWaitableTimer\n"
			"(\n"
			"   hTimer            : 0x%.08X\n"
			"   pDueTime          : 0x%.08X\n"
			"   lPeriod           : 0x%.08X\n"
			"   pfnCompletionRoutine : 0x%.08X\n"
			"   lpArgToCompletionRoutine : 0x%.08X\n"
			"   fResume           : 0x%.08X\n"
			");\n", 
			GetCurrentThreadId(), hTimer, pDueTime, lPeriod, pfnCompletionRoutine,
				lpArgToCompletionRoutine, fResume);

	BOOL Ret = SetWaitableTimer( hTimer, pDueTime, lPeriod, pfnCompletionRoutine,
							lpArgToCompletionRoutine, fResume );
	if(!Ret)
		EmuWarning("SetWaitableTimer failed!");

	EmuSwapFS();	// Xbox FS

	return Ret;
}

// ******************************************************************
// * func: EmuXMountAlternateTitle
// ******************************************************************
DWORD WINAPI XTL::EmuXMountAlternateTitle
(
	LPCSTR		lpRootPath,               
	DWORD		dwAltTitleId,               
	PCHAR		pchDrive               
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuXMountAlternateTitle\n"
			"(\n"
			"   lpRootPath         : 0x%.08X (%s)\n"
			"   dwAltTitleId       : 0x%.08X\n"
			"   pchDrive           : 0x%.08X (%s)\n"
			");\n",
			GetCurrentThreadId(), lpRootPath, lpRootPath, dwAltTitleId, pchDrive, pchDrive);

	// TODO: Anything?

	EmuSwapFS();	// Xbox FS

	return ERROR_SUCCESS;
}

// ******************************************************************
// * func: EmuXUnmountAlternateTitle
// ******************************************************************
DWORD WINAPI XTL::EmuXUnmountAlternateTitle(CHAR chDrive)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuXUnmountAlternativeTitle\n"
			"(\n"
			"   chDrive           : 0x%.08X (%c)\n"
			");\n",
			GetCurrentThreadId(), chDrive, chDrive);

	EmuSwapFS();

	return ERROR_SUCCESS;
}

// ******************************************************************
// * func: EmuMoveFileA
// ******************************************************************
BOOL WINAPI XTL::EmuMoveFileA
(
    LPCSTR lpExistingFileName,
    LPCSTR lpNewFileName
)
{
	EmuSwapFS();	// Win2k/XP FS

	printf("EmuXapi (0x%X): EmuMoveFileA\n"
			"(\n"
			"   lpExistingFileName : 0x%.08X (%s)\n"
			"   lpNewFileName      : 0x%.08X (%s)\n"
			");\n",
			GetCurrentThreadId(), lpExistingFileName, lpExistingFileName,
							lpNewFileName, lpNewFileName);

	// For Panzer.
	BOOL bRet = MoveFileA( lpExistingFileName, lpNewFileName );


	EmuSwapFS();	// Xbox FS

	return bRet;
}

/* not necessary?
// ******************************************************************
// * func: EmuXCalculateSignatureBegin
// ******************************************************************
HANDLE WINAPI XTL::EmuXCalculateSignatureBegin
(
    DWORD dwFlags
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): XCalculateSignatureBegin\n"
           "(\n"
           "   dwFlags             : 0x%.08X\n"
           ");\n",
            GetCurrentThreadId(), dwFlags);

    EmuSwapFS();   // XBox FS

    // return a fake handle value for now
    return (PVOID)0xAAAAAAAA;
}

// ******************************************************************
// * func: EmuXCalculateSignatureBeginEx
// ******************************************************************
HANDLE WINAPI XTL::EmuXCalculateSignatureBeginEx
(
    DWORD dwFlags,
    DWORD dwAltTitleId
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): XCalculateSignatureBeginEx\n"
           "(\n"
           "   dwFlags             : 0x%.08X\n",
           "   dwAltTitleId        : 0x%.08X\n",
           ");\n",
            GetCurrentThreadId(), dwFlags, dwAltTitleId);

    EmuSwapFS();   // XBox FS

    // return a fake handle value for now
    return (PVOID)0xAAAAAAAA;
}

// ******************************************************************
// * func: EmuXCalculateSignatureUpdate
// ******************************************************************
DWORD WINAPI XTL::EmuXCalculateSignatureUpdate
(
  HANDLE        hCalcSig,
  const BYTE    *pbData,
  ULONG         cbData
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): XCalculateSignatureUpdate\n"
           "(\n"
           "   hCalcSig            : 0x%.08X\n",
           "   pbData              : 0x%.08X\n",
           "   cbData              : 0x%.08X\n",
           ");\n",
            GetCurrentThreadId(), hCalcSig, pbData, cbData);

    EmuSwapFS();   // XBox FS

    return ERROR_SUCCESS;
}

// ******************************************************************
// * func: EmuXCalculateSignatureEnd
// ******************************************************************
DWORD WINAPI XTL::EmuXCalculateSignatureEnd
(
  HANDLE                hCalcSig,
  PXCALCSIG_SIGNATURE   pSignature
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): XCalculateSignatureEnd\n"
           "(\n"
           "   hCalcSig            : 0x%.08X\n",
           "   pSignature          : 0x%.08X\n",
           ");\n",
            GetCurrentThreadId(), hCalcSig, pSignature);

    EmuSwapFS();   // XBox FS

    return ERROR_SUCCESS;
}
//*/

/* Yeah, I know these are too high level, but I'm just testing to see 
   if adding these will fix some problems with multi-threaded games hanging
   in WinXP (i.e. Panzer and Azurik). */

// ******************************************************************
// * func: EmuCreateThread
// ******************************************************************
HANDLE WINAPI XTL::EmuCreateThread
(
    LPVOID				    lpThreadAttributes,
    DWORD                   dwStackSize,
    LPTHREAD_START_ROUTINE  lpStartAddress,
    LPVOID                  lpParameter,
    DWORD                   dwCreationFlags,
    LPDWORD                 lpThreadId
)
{
	EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuCreateThread\n"
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

	HANDLE hRet = CreateThread( (PSECURITY_ATTRIBUTES) lpThreadAttributes, dwStackSize, lpStartAddress,
						lpParameter, dwCreationFlags, lpThreadId );

    EmuSwapFS();   // XBox FS

	return hRet;
}

// ******************************************************************
// * func: EmuCreateMutex
// ******************************************************************
HANDLE WINAPI EmuCreateMutex
(
    LPSECURITY_ATTRIBUTES   lpMutexAttributes,
    BOOL                    bInitialOwner,
    LPCSTR                  lpName
)
{
	// TODO: Later (if needed)
	return NULL;
}

// ******************************************************************
// * func: EmuCloseHandle
// ******************************************************************
//BOOL WINAPI XTL::EmuCloseHandle
//(
//    HANDLE hObject
//)
//{
//	EmuSwapFS();   // Win2k/XP FS
//
//    DbgPrintf("EmuXapi (0x%X): EmuCloseHandle\n"
//           "(\n"
//           "   hObject             : 0x%.08X\n",
//           ");\n",
//            GetCurrentThreadId(), hObject);
//
//	BOOL bRet = CloseHandle( hObject );
//
//	EmuSwapFS();	// Xbox FS
//
//	return bRet;
//}

// ******************************************************************
// * func: EmuExitThread
// ******************************************************************
VOID WINAPI XTL::EmuExitThread
(
	DWORD dwExitCode  
)
{
	DbgPrintf("EmuXapi (0x%X): EmuExitThread\n"
           "(\n"
           "   dwExitCode           : 0x%.08X\n"
           ");\n",
            GetCurrentThreadId(), dwExitCode);

	ExitThread( dwExitCode );

	EmuSwapFS();	// Xbox FS
}

// ******************************************************************
// * func: ResumeThread
// ******************************************************************
DWORD WINAPI XTL::EmuResumeThread
(
	HANDLE hThread 
)
{
	EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuResumeThread\n"
           "(\n"
           "   hThread             : 0x%.08X\n"
           ");\n",
            GetCurrentThreadId(), hThread);

	DWORD dwRet = ResumeThread( hThread );

	EmuSwapFS();	// Xbox FS

	return dwRet;
}

// ******************************************************************
// * func: SuspendThread
// ******************************************************************
DWORD WINAPI XTL::EmuSuspendThread
(
	HANDLE hThread 
)
{
	EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuSuspendThread\n"
           "(\n"
           "   hThread             : 0x%.08X\n"
           ");\n",
            GetCurrentThreadId(), hThread);

	DWORD dwRet = SuspendThread( hThread );

	EmuSwapFS();	// Xbox FS

	return dwRet;
}

// ******************************************************************
// * func: EmuVirtualAlloc
// ******************************************************************
LPVOID WINAPI XTL::EmuVirtualAlloc
(
	LPVOID lpAddress,        
	SIZE_T dwSize,           
	DWORD flAllocationType,  
	DWORD flProtect          
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuVirtualAlloc\n"
           "(\n"
           "   lpAddress           : 0x%.08X\n"
		   "   dwSize              : 0x%.08X\n"
		   "   flAllocationType    : 0x%.08X\n"
		   "   flProtect           : 0x%.08X\n"
           ");\n",
            GetCurrentThreadId(), lpAddress, dwSize, flAllocationType, flProtect);

	LPVOID ret = VirtualAllocEx( GetCurrentProcess(), lpAddress, dwSize, flAllocationType, flProtect );

	EmuSwapFS();	// Xbox FS

	return ret;
}

// ******************************************************************
// * func: EmuVirtualAlloc
// ******************************************************************
BOOL WINAPI XTL::EmuVirtualFree
(
	LPVOID lpAddress,   
	SIZE_T dwSize,      
	DWORD dwFreeType    
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuVirtualFree\n"
           "(\n"
           "   lpAddress           : 0x%.08X\n"
		   "   dwSize              : 0x%.08X\n"
		   "   dwFreeType          : 0x%.08X\n"
           ");\n",
            GetCurrentThreadId(), lpAddress, dwSize, dwFreeType);

	BOOL bRet = VirtualFreeEx( GetCurrentProcess(), lpAddress, dwSize, dwFreeType );

	EmuSwapFS();	// Xbox FS

	return bRet;
}

// ******************************************************************
// * func: EmuXGetDeviceEnumerationStatus
// ******************************************************************
DWORD WINAPI XTL::EmuXGetDeviceEnumerationStatus()
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuXGetDeviceEnumerationStatus()\n", GetCurrentThreadId());

	EmuSwapFS();	// Xbox FS

	return XDEVICE_ENUMERATION_IDLE;
}

// ******************************************************************
// * func: EmuSwitchToThread
// ******************************************************************
BOOL WINAPI XTL::EmuSwitchToThread()
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuSwitchToThread()\n", GetCurrentThreadId());

	BOOL bRet = SwitchToThread();

	EmuSwapFS();	// Xbox FS

	return bRet;
}

// ******************************************************************
// * func: EmuXInputGetDeviceDescription
// ******************************************************************
DWORD WINAPI XTL::EmuXInputGetDeviceDescription
(
    HANDLE	hDevice,
    PVOID	pDescription
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuXInputGetDeviceDescription\n"
           "(\n"
           "   hDevice             : 0x%.08X\n"
		   "   pDescription        : 0x%.08X\n"
           ");\n",
            GetCurrentThreadId(), hDevice, pDescription);

	// TODO: Lightgun support?

	EmuSwapFS();	// Xbox FS

	return ERROR_NOT_SUPPORTED; // ERROR_DEVICE_NOT_CONNECTED;
}
