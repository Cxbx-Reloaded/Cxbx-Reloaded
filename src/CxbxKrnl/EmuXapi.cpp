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
/* prevent name collisions */
namespace xboxkrnl
{
	#include <xboxkrnl/xboxkrnl.h>
};

#include <Shlwapi.h>
#include "CxbxKrnl.h"
#include "Emu.h"
#include "EmuFile.h"
#include "EmuFS.h"
#include "EmuShared.h"
#include "HLEIntercept.h"
#include "Exe.h"

// XInputSetState status waiters
extern XInputSetStateStatus g_pXInputSetStateStatus[XINPUT_SETSTATE_SLOTS] = {0};

// XInputOpen handles
extern HANDLE g_hInputHandle[XINPUT_HANDLE_SLOTS] = {0};

bool g_bXLaunchNewImageCalled = false;
bool g_bXInputOpenCalled = false;

bool CxbxMountUtilityDrive(bool formatClean);

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
// * func: EmuXFormatUtilityDrive
// ******************************************************************
BOOL WINAPI XTL::EmuXFormatUtilityDrive()
{
    #ifdef _DEBUG_TRACE
    
    DbgPrintf("EmuXapi (0x%X): EmuXFormatUtilityDrive()\n", GetCurrentThreadId());
    
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
    

    DbgPrintf("EmuXapi (0x%X): EmuGetTimeZoneInformation\n"
           "(\n"
           "   lpTimeZoneInformation : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), lpTimeZoneInformation);

    DWORD dwRet = GetTimeZoneInformation(lpTimeZoneInformation);

    

    return dwRet;
}

// ******************************************************************
// * func: EmuQueryPerformanceCounter
// ******************************************************************
BOOL WINAPI XTL::EmuQueryPerformanceCounter
(
    PLARGE_INTEGER lpPerformanceCount
)
{
    

    DbgPrintf("EmuXapi (0x%X): EmuQueryPerformanceCounter\n"
           "(\n"
           "   lpPerformanceCount  : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), lpPerformanceCount);

    BOOL bRet = QueryPerformanceCounter(lpPerformanceCount);

    // debug - 4x speed
    //lpPerformanceCount->QuadPart *= 4;

    

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
    

    DbgPrintf("EmuXapi (0x%X): EmuQueryPerformanceFrequency\n"
           "(\n"
           "   lpFrequency         : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), lpFrequency);

    BOOL bRet = QueryPerformanceFrequency(lpFrequency);

    

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
        
        DbgPrintf("EmuXapi (0x%X): EmuXMountUtilityDrive\n"
               "(\n"
               "   fFormatClean        : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), fFormatClean);
        
    }
    #endif

	CxbxMountUtilityDrive(fFormatClean);

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
    

    DbgPrintf("EmuXapi (0x%X): EmuXGetDevices\n"
           "(\n"
           "   DeviceType          : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), DeviceType);

    DWORD ret = 0;

    if(DeviceType->Reserved[0] == 0 && DeviceType->Reserved[1] == 0 && DeviceType->Reserved[2] == 0)
        ret = (1 << 0);    // Return 1 Controller
    else
        EmuWarning("Unknown DeviceType (0x%.08X, 0x%.08X, 0x%.08X)\n", DeviceType->Reserved[0], DeviceType->Reserved[1], DeviceType->Reserved[2]);

    

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
        if(DeviceType->Reserved[0] == 0 && DeviceType->Reserved[1] == 0 && DeviceType->Reserved[2] == 0)
		{
			*pdwInsertions = (1<<0);
			*pdwRemovals   = 0;
			bRet = TRUE;
			bFirst = FALSE;
		}
		else
		{
			// TODO: What if it's not a controller?
			EmuWarning("Unknown DeviceType (0x%.08X, 0x%.08X, 0x%.08X)\n", DeviceType->Reserved[0], DeviceType->Reserved[1], DeviceType->Reserved[2]);
		}
    }
    else
    {
        *pdwInsertions = (1<<0); //0;
        *pdwRemovals   = 0;
    }

    

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

    

    return ret;
}

// ******************************************************************
// * func: EmuInputSetState
// ******************************************************************
DWORD WINAPI XTL::EmuXInputSetState
(
    IN     HANDLE           hDevice,
    IN OUT PXINPUT_FEEDBACK pFeedback
)
{
    

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

    

    return ret;
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
    

    DbgPrintf("EmuXapi (0x%X): EmuSetThreadPriorityBoost\n"
           "(\n"
           "   hThread             : 0x%.08X\n"
           "   DisablePriorityBoost: 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), hThread, DisablePriorityBoost);

    BOOL bRet = SetThreadPriorityBoost(hThread, DisablePriorityBoost);

    if(bRet == FALSE)
        EmuWarning("SetThreadPriorityBoost Failed!");

    

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
    

    DbgPrintf("EmuXapi (0x%X): EmuSetThreadPriority\n"
           "(\n"
           "   hThread             : 0x%.08X\n"
           "   nPriority           : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), hThread, nPriority);

    BOOL bRet = SetThreadPriority(hThread, nPriority);

    if(bRet == FALSE)
        EmuWarning("SetThreadPriority Failed!");

    

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
    

    DbgPrintf("EmuXapi (0x%X): EmuGetThreadPriority\n"
           "(\n"
           "   hThread             : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), hThread);

    int iRet = GetThreadPriority(hThread);

    if(iRet == THREAD_PRIORITY_ERROR_RETURN)
        EmuWarning("GetThreadPriority Failed!");

    

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
    

    DbgPrintf("EmuXapi (0x%X): EmuGetExitCodeThread\n"
           "(\n"
           "   hThread             : 0x%.08X\n"
           "   lpExitCode          : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), hThread, lpExitCode);

    BOOL bRet = GetExitCodeThread(hThread, lpExitCode);

    

    return bRet;
}

// ******************************************************************
// * func: EmuXapiThreadStartup
// ******************************************************************
VOID WINAPI XTL::EmuXapiThreadStartup
(
    DWORD dwDummy1,
    DWORD dwDummy2
)
{
    

    DbgPrintf("EmuXapi (0x%X): EmuXapiThreadStartup\n"
           "(\n"
           "   dwDummy1            : 0x%.08X\n"
           "   dwDummy2            : 0x%.08X\n"
           ");\n",
            GetCurrentThreadId(), dwDummy1, dwDummy2);

    

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

// ******************************************************************
// * func: EmuXapiBootDash
// ******************************************************************
VOID WINAPI XTL::EmuXapiBootDash(DWORD dwReason, DWORD dwParameter1, DWORD dwParameter2)
{
    

    DbgPrintf("EmuXapi (0x%X): EmuXapiBootDash\n"
           "(\n"
           "   dwReason            : 0x%.08X\n"
           "   dwParameter1        : 0x%.08X\n"
           "   dwParameter2        : 0x%.08X\n"
           ");\n",
            GetCurrentThreadId(), dwReason, dwParameter1, dwParameter2);

    CxbxKrnlCleanup("Emulation Terminated (XapiBootDash)");

    

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
		

	DbgPrintf("EmuXapi (0x%X): EmuXLaunchNewImage\n"
			"(\n"
			"   lpTitlePath      : 0x%.08X (%s)\n"
			"   pLaunchData      : 0x%.08X\n"
			");\n",
			GetCurrentThreadId(), lpTitlePath, lpTitlePath, pLaunchData);

	// If this function succeeds, it doesn't get a chance to return anything.
	DWORD dwRet = ERROR_GEN_FAILURE;

	// If no path is specified, then the xbe is rebooting to dashboard
	if (!lpTitlePath) {
		char szDashboardPath[MAX_PATH];
		EmuNtSymbolicLinkObject* symbolicLinkObject = FindNtSymbolicLinkObjectByDevice(DeviceHarddisk0Partition2);
		sprintf(szDashboardPath, "%s\\xboxdash.xbe", symbolicLinkObject->NativePath.c_str());
		
		if (PathFileExists(szDashboardPath)) {
			MessageBox(CxbxKrnl_hEmuParent, "The title is rebooting to dashboard", "Cxbx", 0);
			char szXboxDashboardPath[MAX_PATH];
			sprintf(szXboxDashboardPath, "%c:\\xboxdash.xbe", symbolicLinkObject->DriveLetter);
			EmuXLaunchNewImage(szXboxDashboardPath, pLaunchData);
		}
			
		CxbxKrnlCleanup("The xbe rebooted to Dashboard and xboxdash.xbe could not be found");
	}
		
	char szExeFileName[MAX_PATH];
	GetModuleFileName(GetModuleHandle(NULL), szExeFileName, MAX_PATH);

	// Convert Xbox XBE Path to Windows Path
	char szXbePath[MAX_PATH];

	EmuNtSymbolicLinkObject* symbolicLink = FindNtSymbolicLinkObjectByVolumeLetter(lpTitlePath[0]);
	snprintf(szXbePath, MAX_PATH, "%s%s", symbolicLink->NativePath.c_str(), &lpTitlePath[2]);

	// Determine Working Directory
	char szWorkingDirectoy[MAX_PATH];
	strncpy_s(szWorkingDirectoy, szXbePath, MAX_PATH);
	PathRemoveFileSpec(szWorkingDirectoy);

	// Save the launch data
	if (pLaunchData != NULL)
	{
		CopyMemory(&g_SavedLaunchData, pLaunchData, sizeof(LAUNCH_DATA));

		// Save the launch data parameters to disk for later.
		char szLaunchDataPath[MAX_PATH];
		snprintf(szLaunchDataPath, MAX_PATH, "%s\\CxbxLaunchData.bin", szWorkingDirectoy);

		DbgPrintf("Saving launch data to %s\n", szLaunchDataPath);

		FILE* fp = fopen(szLaunchDataPath, "wb");
		fseek(fp, 0, SEEK_SET);
		fwrite(pLaunchData, sizeof(LAUNCH_DATA), 1, fp);
		fclose(fp);
	}

	g_bXLaunchNewImageCalled = true;

	// Launch the new Xbe	
	char szArgsBuffer[4096];
	snprintf(szArgsBuffer, 4096, "/load \"%s\" %u %d \"%s\"", szXbePath, CxbxKrnl_hEmuParent, CxbxKrnl_DebugMode, CxbxKrnl_DebugFileName);

	if ((int)ShellExecute(NULL, "open", szExeFileName, szArgsBuffer, szWorkingDirectoy, SW_SHOWDEFAULT) <= 32)
	{
		CxbxKrnlCleanup("Could not launch %s", lpTitlePath);
	}

	ExitProcess(EXIT_SUCCESS);
	

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
		

	DbgPrintf("EmuXapi (0x%X): EmuXGetLaunchInfo\n"
			"(\n"
			"   pdwLaunchDataType : 0x%.08X\n"
			"   pLaunchData       : 0x%.08X\n"
			");\n",
			GetCurrentThreadId(), pdwLaunchDataType, pLaunchData);
	
	// The title was launched by turning on the Xbox console with the title disc already in the DVD drive
	DWORD dwRet = ERROR_NOT_FOUND;

	// Has XLaunchNewImage been called since we've started this round?
	if(g_bXLaunchNewImageCalled)
	{
		// The title was launched by a call to XLaunchNewImage
		// A title can pass data only to itself, not another title
		//
		// Other options include LDT_FROM_DASHBOARD, LDT_FROM_DEBUGGER_CMDLINE and LDT_FROM_UPDATE
		//
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
		// The title was launched by a call to XLaunchNewImage
		// A title can pass data only to itself, not another title
		//
		// Other options include LDT_FROM_DASHBOARD, LDT_FROM_DEBUGGER_CMDLINE and LDT_FROM_UPDATE
		//
		*pdwLaunchDataType = LDT_TITLE; 

		// Read in the contents.
		fseek(fp, 0, SEEK_SET);
		fread(&g_SavedLaunchData, sizeof(LAUNCH_DATA), 1, fp);
		memcpy(pLaunchData, &g_SavedLaunchData, sizeof(LAUNCH_DATA));
		fclose(fp);

		// Delete the file once we're done.
		DeleteFile("CxbxLaunchData.bin");

		dwRet = ERROR_SUCCESS;
	}

		

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
		

	DbgPrintf("EmuXapi (0x%X): EmuXSetProcessQuantumLength\n"
			"(\n"
			"   dwMilliseconds    : 0x%.08X\n"
			");\n",
			GetCurrentThreadId(), dwMilliseconds);

	// TODO: Implement?
	EmuWarning("XSetProcessQuantumLength is being ignored!");

		
}
	
// ******************************************************************
// * func: EmuXGetFileCacheSize
// ******************************************************************
DWORD WINAPI XTL::EmuXGetFileCacheSize()
{
		

	DbgPrintf("EmuXapi (0x%X): EmuXGetFileCacheSize()\n", GetCurrentThreadId());

	// Return the default cache size for now.
	// TODO: Save the file cache size if/when set.
	DWORD dwRet = 64 * 1024;

	

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
		

	DbgPrintf("EmuXapi (0x%X): EmuSignalObjectAndWait\n"
			"(\n"
			"   hObjectToSignal   : 0x%.08X\n"
			"   hObjectToWaitOn   : 0x%.08X\n"
			"   dwMilliseconds    : 0x%.08X\n"
			"   bAlertable        : 0x%.08X\n"
			");\n",
			GetCurrentThreadId(), hObjectToSignal, hObjectToWaitOn, dwMilliseconds, bAlertable);

	DWORD dwRet = SignalObjectAndWait( hObjectToSignal, hObjectToWaitOn, dwMilliseconds, bAlertable ); 

		

	return dwRet;
}

// ******************************************************************
// * func: EmuPulseEvent
// ******************************************************************
BOOL WINAPI XTL::EmuPulseEvent( HANDLE hEvent )
{
		

	DbgPrintf("EmuXapi (0x%X): EmuPulseEvent\n"
			"(\n"
			"   hEvent            : 0x%.08X\n"
			");\n",
			GetCurrentThreadId(), hEvent);

	// TODO: This function might be a bit too high level.  If it is,
	// feel free to implement NtPulseEvent in EmuKrnl.cpp

	BOOL bRet = PulseEvent( hEvent );

		

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
		

	DbgPrintf("EmuXapi (0x%X): EmuReleaseSemaphore\n"
			"(\n"
			"   uTimerID          : 0x%.08X\n"
			");\n",
			GetCurrentThreadId(), uTimerID);

	MMRESULT Ret = timeKillEvent( uTimerID );

		

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

		
}

// ******************************************************************
// * func: EmuGetFileAttributesA
// ******************************************************************
DWORD WINAPI XTL::EmuGetFileAttributesA
(
	LPCSTR			lpFileName    // name of file or directory
)
{
		

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
		EmuWarning("GetFileAttributes(\"%s\") failed!", szBuffer);

	

	return dwRet;
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
		

	DbgPrintf("EmuXapi (0x%X): EmuXMountMUA\n"
			"(\n"
			"   dwPort            : 0x%.08X\n"
			"   dwSlot            : 0x%.08X\n"
			"   pchDrive          : 0x%.08X (%s)\n"
			");\n", 
			GetCurrentThreadId(), dwPort, dwSlot, pchDrive, pchDrive);

	// TODO: Actually allow memory card emulation? This might make transferring
	// game saves a bit easier if the memory card directory was configurable. =]

		

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
		

	DbgPrintf("EmuXapi (0x%X): EmuXMountAlternateTitle\n"
			"(\n"
			"   lpRootPath         : 0x%.08X (%s)\n"
			"   dwAltTitleId       : 0x%.08X\n"
			"   pchDrive           : 0x%.08X (%s)\n"
			");\n",
			GetCurrentThreadId(), lpRootPath, lpRootPath, dwAltTitleId, pchDrive, pchDrive);

	// TODO: Anything?

		

	return ERROR_SUCCESS;
}

// ******************************************************************
// * func: EmuXUnmountAlternateTitle
// ******************************************************************
DWORD WINAPI XTL::EmuXUnmountAlternateTitle(CHAR chDrive)
{
		

	DbgPrintf("EmuXapi (0x%X): EmuXUnmountAlternativeTitle\n"
			"(\n"
			"   chDrive           : 0x%.08X (%c)\n"
			");\n",
			GetCurrentThreadId(), chDrive, chDrive);

	

	return ERROR_SUCCESS;
}

// ******************************************************************
// * func: EmuXGetDeviceEnumerationStatus
// ******************************************************************
DWORD WINAPI XTL::EmuXGetDeviceEnumerationStatus()
{
		

	DbgPrintf("EmuXapi (0x%X): EmuXGetDeviceEnumerationStatus()\n", GetCurrentThreadId());

		

	return XDEVICE_ENUMERATION_IDLE;
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
		

	DbgPrintf("EmuXapi (0x%X): EmuXInputGetDeviceDescription\n"
           "(\n"
           "   hDevice             : 0x%.08X\n"
		   "   pDescription        : 0x%.08X\n"
           ");\n",
            GetCurrentThreadId(), hDevice, pDescription);

	// TODO: Lightgun support?

		

	return ERROR_NOT_SUPPORTED; // ERROR_DEVICE_NOT_CONNECTED;
}

// ******************************************************************
// * func: EmuXAutoPowerDownResetTimer
// ******************************************************************
int WINAPI XTL::EmuXAutoPowerDownResetTimer()
{
		

	DbgPrintf("EmuXapi (0x%X): EmuXAutoPowerDownResetTimer()\n",
            GetCurrentThreadId());

	// Meh, that's what the 'X' is for! =]

		

	return TRUE;
}

// ******************************************************************
// * func: EmuXMountMURootA
// ******************************************************************
DWORD WINAPI XTL::EmuXMountMURootA
(
	DWORD dwPort,                  
	DWORD dwSlot,                  
	PCHAR pchDrive               
)
{
	
		

	DbgPrintf("EmuXapi (0x%X): EmuXMountMURootA\n"
			"(\n"
			"   dwPort            : 0x%.08X\n"
			"   dwSlot            : 0x%.08X\n"
			"   pchDrive          : 0x%.08X (%s)\n"
			");\n", 
			GetCurrentThreadId(), dwPort, dwSlot, pchDrive, pchDrive);

	// TODO: The params are probably wrong...

		

	return ERROR_SUCCESS;
}
