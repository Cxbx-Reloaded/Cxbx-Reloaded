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

#include "CxbxUtil.h"
#include "CxbxKrnl.h"
#include "Emu.h"
#include "EmuFS.h"
#include "EmuAlloc.h"

// XInputSetState status waiters
extern XInputSetStateStatus g_pXInputSetStateStatus[XINPUT_SETSTATE_SLOTS] = {0};

// XInputOpen handles
extern HANDLE g_hInputHandle[XINPUT_HANDLE_SLOTS] = {0};

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace NtDll
{
    #include "EmuNtDll.h"
};

#include "EmuXTL.h"

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

//* ended up not fixing anything in panzer dragoon!
// ******************************************************************
// * func: EmuFindFirstFileA
// ******************************************************************
HANDLE WINAPI XTL::EmuFindFirstFileA
(
    IN  LPCSTR              lpFileName,
    OUT LPWIN32_FIND_DATA   lpFindFileData
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuFindFirstFileA\n"
           "(\n"
           "   lpFileName          : 0x%.08X (%s)\n"
           "   lpFindFileData      : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), lpFileName, lpFileName, lpFindFileData);

    //
    // TODO: this code is replicated in NtCreateFile. make this a function
    //

    //
    // TODO: replace full directories with their shorthand (D:\, etc)
    //

    char *szBuffer = (char*)lpFileName;
    char *szRoot = g_strCurDrive;

    //printf("bef : %s\n", lpFileName);

    if(szBuffer != NULL)
    {
        // trim this off
        if(szBuffer[0] == '\\' && szBuffer[1] == '?' && szBuffer[2] == '?' && szBuffer[3] == '\\')
        {
            szBuffer += 4;
        }

        // D:\ should map to current directory
	    if( (szBuffer[0] == 'D' || szBuffer[0] == 'd') && szBuffer[1] == ':' && szBuffer[2] == '\\')
	    {
		    szBuffer += 3;
	    }
	    else if( (szBuffer[0] == 'T' || szBuffer[0] == 't') && szBuffer[1] == ':' && szBuffer[2] == '\\')
	    {
		    szBuffer += 3;

            szRoot = g_strTDrive;
	    }
	    else if( (szBuffer[0] == 'U' || szBuffer[0] == 'u') && szBuffer[1] == ':' && szBuffer[2] == '\\')
	    {
		    szBuffer += 3;

            szRoot = g_strUDrive;
	    }
	    else if( (szBuffer[0] == 'Z' || szBuffer[0] == 'z') && szBuffer[1] == ':' && szBuffer[2] == '\\')
	    {
		    szBuffer += 3;

            szRoot = g_strZDrive;
	    }
    }

    //printf("af1 : %s\n", szRoot);
    //printf("af2 : %s\n", szBuffer);

    //char szOldDir[MAX_PATH];
    
    //GetCurrentDirectory(MAX_PATH, szOldDir);

    SetCurrentDirectory(szRoot);

    HANDLE hRet = FindFirstFile(szBuffer, lpFindFileData);
    
    if(!FAILED(hRet))
    {
        do
        {
            BOOL bRet = FindNextFile(hRet, lpFindFileData);

            if(!bRet) { hRet = INVALID_HANDLE_VALUE; break; }

            if( (strcmp(lpFindFileData->cFileName, ".") != 0) && (strcmp(lpFindFileData->cFileName, "..") != 0) )
                break;
        }
        while(true);
    }

    //SetCurrentDirectory(szOldDir);

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuFindNextFileA
// ******************************************************************
BOOL WINAPI XTL::EmuFindNextFileA
(
    IN  HANDLE              hFindFile,
    OUT LPWIN32_FIND_DATA   lpFindFileData
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuFindNextFileA\n"
           "(\n"
           "   hFindFile           : 0x%.08X\n"
           "   lpFindFileData      : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), hFindFile, lpFindFileData);

    //
    // TODO: replace full directories with their shorthand (D:\, etc)
    //

    BOOL bRet;

    do
    {
        bRet = FindNextFile(hFindFile, lpFindFileData);

        if(!bRet) { break; }

        if( (strcmp(lpFindFileData->cFileName, ".") != 0) && (strcmp(lpFindFileData->cFileName, "..") != 0) )
            break;
    }
    while(true);

    //printf("Found : %s\n", lpFindFileData->cFileName);

    EmuSwapFS();   // XBox FS

    return bRet;
}
//*/

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

    offs = (BYTE)(RoundUp((uint32)pRet, 0x20) - (uint32)pRet);

    if(offs == 0)
    {
        offs = 0x20;
    }

    pRet = (PVOID)((uint32)pRet + offs);

    *(BYTE*)((uint32)pRet - 1) = offs;

    DbgPrintf("pRet : 0x%.08X\n", pRet);

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

    if(lpMem != NULL)
    {
        BYTE offs = *(BYTE*)((uint32)lpMem - 1);

        lpMem = (PVOID)((uint32)lpMem - offs);
    }

    PVOID pRet = CxbxRtlRealloc(hHeap, dwFlags, lpMem, dwBytes + 0x20);

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
    DWORD   Unknown1,
    PVOID   Unknown2
)
{
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuXInitDevices\n"
           "(\n"
           "   Unknown1            : 0x%.08X\n"
           "   Unknown2            : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), Unknown1, Unknown2);

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
    BOOL bFirst = TRUE;

    // Return 1 Controller Inserted initially, then no changes forever
    if(bFirst)
    {
        *pdwInsertions = (1<<0);
        *pdwRemovals   = 0;
        bRet = TRUE;
    }
    else
    {
        *pdwInsertions = 0;
        *pdwRemovals   = 0;
    }

    EmuSwapFS();   // XBox FS

    return bRet;
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
            pph = new POLLING_PARAMETERS_HANDLE();

            if(pPollingParameters != NULL)
            {
                pph->pPollingParameters = new XINPUT_POLLING_PARAMETERS();

                memcpy(&pph->pPollingParameters, pPollingParameters, sizeof(XINPUT_POLLING_PARAMETERS));
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
                    pph->pPollingParameters = new XINPUT_POLLING_PARAMETERS();
                }

                memcpy(&pph->pPollingParameters, pPollingParameters, sizeof(XINPUT_POLLING_PARAMETERS));
            }
            else
            {
                if(pph->pPollingParameters != 0)
                {
                    delete pph->pPollingParameters;

                    pph->pPollingParameters = 0;
                }
            }
        }

        pph->dwPort = dwPort;
    }

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
            if(dwPort == 0)
            {
                EmuDInputPoll(pState);
                ret = ERROR_SUCCESS;
            }
        }
    }

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
        if(g_pfnThreadNotification != NULL)
            CxbxKrnlCleanup("Multiple thread notification routines installed (caustik can fix this!)");

        g_pfnThreadNotification = pThreadNotification->pfnNotifyRoutine;
    }
    else
    {
        if(g_pfnThreadNotification != NULL)
            g_pfnThreadNotification = NULL;
    }

    EmuSwapFS();   // XBox FS
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
