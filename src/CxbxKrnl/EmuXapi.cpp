// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
#include "Logging.h"
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

// Fiber function list
typedef struct _XFIBER
{
	LPFIBER_START_ROUTINE pfnRoutine;
	LPVOID				  pParam;
}XFIBER;

XFIBER g_Fibers[256];
// Number of fiber routines queued
int	   g_FiberCount = 0;


// ******************************************************************
// * func: EmuXFormatUtilityDrive
// ******************************************************************
BOOL WINAPI XTL::EMUPATCH(XFormatUtilityDrive)()
{
	LOG_FUNC();

    // TODO: yeah... we'll format... riiiiight

	RETURN(TRUE);
}

// ******************************************************************
// * func: EmuGetTimeZoneInformation
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(GetTimeZoneInformation)
(
    OUT LPTIME_ZONE_INFORMATION lpTimeZoneInformation
)
{
	LOG_FUNC_ONE_ARG_OUT(lpTimeZoneInformation);

    DWORD dwRet = GetTimeZoneInformation(lpTimeZoneInformation);

	RETURN(dwRet);
}

// ******************************************************************
// * func: EmuXMountUtilityDrive
// ******************************************************************
BOOL WINAPI XTL::EMUPATCH(XMountUtilityDrive)
(
    BOOL    fFormatClean
)
{
	LOG_FUNC_ONE_ARG(fFormatClean);

	CxbxMountUtilityDrive(fFormatClean == TRUE);

	RETURN(TRUE);
}

// ******************************************************************
// * func: EmuXInitDevices
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(XInitDevices)
(
    DWORD					dwPreallocTypeCount,
	PXDEVICE_PREALLOC_TYPE	PreallocTypes
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(dwPreallocTypeCount)
		LOG_FUNC_ARG(PreallocTypes)
		LOG_FUNC_END;

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
}

// ******************************************************************
// * func: EmuXGetDevices
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XGetDevices)
(
    PXPP_DEVICE_TYPE DeviceType
)
{
	LOG_FUNC_ONE_ARG(DeviceType);

    DWORD ret = 0;

    if(DeviceType->Reserved[0] == 0 && DeviceType->Reserved[1] == 0 && DeviceType->Reserved[2] == 0)
        ret = (1 << 0);    // Return 1 Controller
    else
        EmuWarning("Unknown DeviceType (0x%.08X, 0x%.08X, 0x%.08X)", DeviceType->Reserved[0], DeviceType->Reserved[1], DeviceType->Reserved[2]);

	RETURN(ret);
}

// ******************************************************************
// * func: EmuXGetDeviceChanges
// ******************************************************************
BOOL WINAPI XTL::EMUPATCH(XGetDeviceChanges)
(
    PXPP_DEVICE_TYPE DeviceType,
    PDWORD           pdwInsertions,
    PDWORD           pdwRemovals
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(DeviceType)
		LOG_FUNC_ARG(pdwInsertions)
		LOG_FUNC_ARG(pdwRemovals)
		LOG_FUNC_END;

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
			EmuWarning("Unknown DeviceType (0x%.08X, 0x%.08X, 0x%.08X)", DeviceType->Reserved[0], DeviceType->Reserved[1], DeviceType->Reserved[2]);
		}
    }
    else
    {
        *pdwInsertions = (1<<0); //0;
        *pdwRemovals   = 0;
    }

	RETURN(TRUE); // TODO : RETURN(bRet);
}

// ******************************************************************
// * func: EmuXInputOpen
// ******************************************************************
HANDLE WINAPI XTL::EMUPATCH(XInputOpen)
(
    IN PXPP_DEVICE_TYPE             DeviceType,
    IN DWORD                        dwPort,
    IN DWORD                        dwSlot,
    IN PXINPUT_POLLING_PARAMETERS   pPollingParameters OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(DeviceType)
		LOG_FUNC_ARG(dwPort)
		LOG_FUNC_ARG(dwSlot)
		LOG_FUNC_ARG(pPollingParameters)
		LOG_FUNC_END;

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

	RETURN((HANDLE)pph);
}

// ******************************************************************
// * func: EmuXInputClose
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(XInputClose)
(
    IN HANDLE hDevice
)
{
	LOG_FUNC_ONE_ARG(hDevice);

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
}

// ******************************************************************
// * func: EmuXInputPoll
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XInputPoll)
(
    IN HANDLE hDevice
)
{
	LOG_FUNC_ONE_ARG(hDevice);

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

	RETURN(ERROR_SUCCESS);
}

// ******************************************************************
// * func: EmuXInputGetCapabilities
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XInputGetCapabilities)
(
    IN  HANDLE               hDevice,
    OUT PXINPUT_CAPABILITIES pCapabilities
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hDevice)
		LOG_FUNC_ARG_OUT(pCapabilities)
		LOG_FUNC_END;

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

	RETURN(ret);
}

// ******************************************************************
// * func: EmuInputGetState
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XInputGetState)
(
    IN  HANDLE         hDevice,
    OUT PXINPUT_STATE  pState
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hDevice)
		LOG_FUNC_ARG_OUT(pState)
		LOG_FUNC_END;

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

	RETURN(ret);
}

// ******************************************************************
// * func: EmuInputSetState
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XInputSetState)
(
    IN     HANDLE           hDevice,
    IN OUT PXINPUT_FEEDBACK pFeedback
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hDevice)
		LOG_FUNC_ARG(pFeedback)
		LOG_FUNC_END;

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

	RETURN(ret);
}


// ******************************************************************
// * func: EmuSetThreadPriorityBoost
// ******************************************************************
BOOL WINAPI XTL::EMUPATCH(SetThreadPriorityBoost)
(
    HANDLE  hThread,
    BOOL    DisablePriorityBoost
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hThread)
		LOG_FUNC_ARG(DisablePriorityBoost)
		LOG_FUNC_END;

    BOOL bRet = SetThreadPriorityBoost(hThread, DisablePriorityBoost);

    if(bRet == FALSE)
        EmuWarning("SetThreadPriorityBoost Failed!");

	RETURN(bRet);
}

// ******************************************************************
// * func: EmuSetThreadPriority
// ******************************************************************
BOOL WINAPI XTL::EMUPATCH(SetThreadPriority)
(
    HANDLE  hThread,
    int     nPriority
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hThread)
		LOG_FUNC_ARG(nPriority)
		LOG_FUNC_END;

    BOOL bRet = SetThreadPriority(hThread, nPriority);

    if(bRet == FALSE)
        EmuWarning("SetThreadPriority Failed!");

	RETURN(bRet);
}


// ******************************************************************
// * func: EmuGetThreadPriority
// ******************************************************************
int WINAPI XTL::EMUPATCH(GetThreadPriority)
(
    HANDLE  hThread
)
{
	LOG_FUNC_ONE_ARG(hThread);

    int iRet = GetThreadPriority(hThread);

    if(iRet == THREAD_PRIORITY_ERROR_RETURN)
        EmuWarning("GetThreadPriority Failed!");

	RETURN(iRet);
}

// ******************************************************************
// * func: EmuGetExitCodeThread
// ******************************************************************
BOOL WINAPI XTL::EMUPATCH(GetExitCodeThread)
(
    HANDLE  hThread,
    LPDWORD lpExitCode
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hThread)
		LOG_FUNC_ARG(lpExitCode)
		LOG_FUNC_END;

    BOOL bRet = GetExitCodeThread(hThread, lpExitCode);

	RETURN(bRet);
}

// ******************************************************************
// * func: EmuXapiThreadStartup
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(XapiThreadStartup)
(
    DWORD dwDummy1,
    DWORD dwDummy2
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(dwDummy1)
		LOG_FUNC_ARG(dwDummy2)
		LOG_FUNC_END;

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
}

// ******************************************************************
// * func: EmuXRegisterThreadNotifyRoutine
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(XRegisterThreadNotifyRoutine)
(
    PXTHREAD_NOTIFICATION   pThreadNotification,
    BOOL                    fRegister
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThreadNotification)
		LOG_FUNC_ARG(fRegister)
		LOG_FUNC_END;

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
// * func: EmuCreateFiber
// ******************************************************************
LPVOID WINAPI XTL::EMUPATCH(CreateFiber)
(
	DWORD					dwStackSize,
	LPFIBER_START_ROUTINE	lpStartRoutine,
	LPVOID					lpParameter
)
{
    DbgPrintf("EmuXapi: EmuCreateFiber\n"
           "(\n"
		   "   dwStackSize         : 0x%.08X\n"
           "   lpStartRoutine      : 0x%.08X\n"
           "   lpParameter         : 0x%.08X\n"
           ");\n",
            dwStackSize, lpStartRoutine, lpParameter);

	LPVOID pFiber = CreateFiber( dwStackSize, lpStartRoutine, lpParameter );
	if( !pFiber )
		EmuWarning( "CreateFiber failed!" );
	else
		DbgPrintf("CreateFiber returned 0x%X\n", pFiber);

	// Add to list of queued fiber routines
	g_Fibers[g_FiberCount].pfnRoutine = lpStartRoutine;
	if( lpParameter ) g_Fibers[g_FiberCount].pParam = lpParameter;

	g_FiberCount++;

	return pFiber;
}

// ******************************************************************
// * func: EmuDeleteFiber
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(DeleteFiber)
(
	LPVOID					lpFiber
)
{

	DbgPrintf("EmuXapi: EmuDeleteFiber\n"
			"(\n"
			"	lpFiber            : 0x%.08X\n"
			");\n",
			lpFiber );

	DeleteFiber( lpFiber );

}

// ******************************************************************
// * func: EmuSwitchToFiber
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(SwitchToFiber)
(
	LPVOID lpFiber 
)
{

	DbgPrintf("EmuXapi: EmuSwitchToFiber\n"
			"(\n"
			"	lpFiber            : 0x%.08X\n"
			");\n",
			lpFiber );

//	SwitchToFiber( lpFiber );	// <- Hangs/crashes...

	// Execute fiber routines
	for( int i = 0; i < g_FiberCount; i++ )
	{
		if( g_Fibers[i].pfnRoutine )
			g_Fibers[i].pfnRoutine(g_Fibers[i].pParam);
	
	}

	g_FiberCount = 0;

	DbgPrintf( "Finished executing fibers!\n" );

}

// ******************************************************************
// * func: EmuConvertThreadToFiber
// ******************************************************************
LPVOID WINAPI XTL::EMUPATCH(ConvertThreadToFiber)
(
	LPVOID lpParameter
)
{
	DbgPrintf("EmuXapi: EmuConvertThreadToFiber\n"
			"(\n"
			"	lpParameter        : 0x%.08X\n"
			");\n",
			lpParameter );

	LPVOID pRet = ConvertThreadToFiber( lpParameter );
	
	DbgPrintf( "EmuConvertThreadToFiber returned 0x%X\n", pRet );


	return pRet;
}

// ******************************************************************
// * func: EmuXapiFiberStartup
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(XapiFiberStartup)(DWORD dwDummy)
{

	DbgPrintf("EmuXapi: EmuXapiFiberStarup()\n"
			"(\n"
			"	dwDummy            : 0x%.08X\n"
			");\n",
			dwDummy);


	typedef void (__stdcall *pfDummyFunc)(DWORD dwDummy);
	pfDummyFunc func = (pfDummyFunc)dwDummy;

	void* TlsIndex = (void*) CxbxKrnl_TLS->dwTLSIndexAddr;

	__asm 
	{
		mov     eax, TlsIndex
		mov     ecx, fs:4
		mov     eax, [ecx+eax*4]
		mov     eax, [eax+8]
		push    dword ptr [eax]
		call    func
	}

}

// ******************************************************************
// * func: EmuQueueUserAPC
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(QueueUserAPC)
(
	PAPCFUNC	pfnAPC,
	HANDLE		hThread,
	DWORD		dwData
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pfnAPC)
		LOG_FUNC_ARG(hThread)
		LOG_FUNC_ARG(dwData)
		LOG_FUNC_END;

	DWORD dwRet = 0;

	// If necessary, we can just continue to emulate NtQueueApcThread (0xCE).
	// I added this because NtQueueApcThread fails in Metal Slug 3.

	HANDLE hApcThread = NULL;
	if(!DuplicateHandle(g_CurrentProcessHandle, hThread, g_CurrentProcessHandle, &hApcThread, THREAD_SET_CONTEXT,FALSE,0))
		EmuWarning("DuplicateHandle failed!");

	dwRet = QueueUserAPC(pfnAPC, hApcThread, dwData);
	if(!dwRet)
		EmuWarning("QueueUserAPC failed!");

	RETURN(dwRet);
}

// ******************************************************************
// * func: EmuGetOverlappedResult
// ******************************************************************
BOOL WINAPI XTL::EMUPATCH(GetOverlappedResult)
(
	HANDLE			hFile,
	LPOVERLAPPED	lpOverlapped,
	LPDWORD			lpNumberOfBytesTransferred,
	BOOL			bWait
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hFile)
		LOG_FUNC_ARG(lpOverlapped)
		LOG_FUNC_ARG(lpNumberOfBytesTransferred)
		LOG_FUNC_ARG(bWait)
		LOG_FUNC_END;

	BOOL bRet = GetOverlappedResult( hFile, lpOverlapped, lpNumberOfBytesTransferred, bWait );

//	if(bWait)
//		bRet = TRUE; // Sucker...

	RETURN(bRet);
}

// ******************************************************************
// * func: EmuXLaunchNewImage
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XLaunchNewImage)
(
	LPCSTR			lpTitlePath,
	PLAUNCH_DATA	pLaunchData
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(lpTitlePath)
		LOG_FUNC_ARG(pLaunchData)
		LOG_FUNC_END;

	// If this function succeeds, it doesn't get a chance to return anything.
	DWORD dwRet = ERROR_GEN_FAILURE;

	// If no path is specified, then the xbe is rebooting to dashboard
	if (!lpTitlePath) {
		char szDashboardPath[MAX_PATH] = { 0 };
		XboxDevice* rootDevice = CxbxDeviceByDevicePath(DeviceHarddisk0Partition2);
		if(rootDevice != nullptr)
			sprintf(szDashboardPath, "%s\\xboxdash.xbe", rootDevice->HostDevicePath.c_str());
		
		if (PathFileExists(szDashboardPath))
		{
			MessageBox(CxbxKrnl_hEmuParent, "The title is rebooting to dashboard", "Cxbx-Reloaded", 0);
			EMUPATCH(XLaunchNewImage)("C:\\xboxdash.xbe", pLaunchData);
		}
			
		CxbxKrnlCleanup("The xbe rebooted to Dashboard and xboxdash.xbe could not be found");
	}
		
	char szExeFileName[MAX_PATH];
	GetModuleFileName(GetModuleHandle(NULL), szExeFileName, MAX_PATH);

	// Convert Xbox XBE Path to Windows Path
	char szXbePath[MAX_PATH];

	EmuNtSymbolicLinkObject* symbolicLink = FindNtSymbolicLinkObjectByDriveLetter(lpTitlePath[0]);
	snprintf(szXbePath, MAX_PATH, "%s%s", symbolicLink->HostSymbolicLinkPath.c_str(), &lpTitlePath[2]);

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

	RETURN(dwRet);
}

// ******************************************************************
// * func: EmuXGetLaunchInfo
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XGetLaunchInfo)
(
	PDWORD			pdwLaunchDataType,
	PLAUNCH_DATA	pLaunchData
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pdwLaunchDataType)
		LOG_FUNC_ARG(pLaunchData)
		LOG_FUNC_END;

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

	RETURN(dwRet);
}

// ******************************************************************
// * func: EmuXSetProcessQuantumLength
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(XSetProcessQuantumLength)
(
    DWORD dwMilliseconds
)
{
	LOG_FUNC_ONE_ARG(dwMilliseconds);

	// TODO: Implement?
	LOG_IGNORED();
}
	
// ******************************************************************
// * func: EmuSignalObjectAndWait
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(SignalObjectAndWait)
(
	HANDLE	hObjectToSignal,
	HANDLE	hObjectToWaitOn,
	DWORD	dwMilliseconds,
	BOOL	bAlertable
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hObjectToSignal)
		LOG_FUNC_ARG(hObjectToWaitOn)
		LOG_FUNC_ARG(dwMilliseconds)
		LOG_FUNC_ARG(bAlertable)
		LOG_FUNC_END;

	DWORD dwRet = SignalObjectAndWait( hObjectToSignal, hObjectToWaitOn, dwMilliseconds, bAlertable ); 

	RETURN(dwRet);
}

// ******************************************************************
// * func: timeSetEvent
// ******************************************************************
MMRESULT WINAPI XTL::EMUPATCH(timeSetEvent)
(
	UINT			uDelay,
	UINT			uResolution,
	LPTIMECALLBACK	fptc,
	DWORD			dwUser,
	UINT			fuEvent
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(uDelay)
		LOG_FUNC_ARG(uResolution)
		LOG_FUNC_ARG(fptc)
		LOG_FUNC_ARG(dwUser)
		LOG_FUNC_ARG(fuEvent)
		LOG_FUNC_END;

	MMRESULT Ret = timeSetEvent( uDelay, uResolution, fptc, (DWORD_PTR) dwUser, fuEvent );

	RETURN(Ret);
}

// ******************************************************************
// * func: timeKillEvent
// ******************************************************************
MMRESULT WINAPI XTL::EMUPATCH(timeKillEvent)
(
	UINT uTimerID  
)
{
	LOG_FUNC_ONE_ARG(uTimerID);

	MMRESULT Ret = timeKillEvent( uTimerID );

	RETURN(Ret);
}

// ******************************************************************
// * func: EmuRaiseException
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(RaiseException)
(
	DWORD			dwExceptionCode,       // exception code
	DWORD			dwExceptionFlags,      // continuable exception flag
	DWORD			nNumberOfArguments,    // number of arguments
	CONST ULONG_PTR *lpArguments		   // array of arguments
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(dwExceptionCode)
		LOG_FUNC_ARG(dwExceptionFlags)
		LOG_FUNC_ARG(nNumberOfArguments)
		LOG_FUNC_ARG(lpArguments)
		LOG_FUNC_END;

	// TODO: Implement or not?
//	RaiseException(dwExceptionCode, dwExceptionFlags, nNumberOfArguments, (*(ULONG_PTR**) &lpArguments));

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// func: XMountMUA
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XMountMUA)
(
	DWORD dwPort,                  
	DWORD dwSlot,                  
	PCHAR pchDrive               
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(dwPort)
		LOG_FUNC_ARG(dwSlot)
		LOG_FUNC_ARG(pchDrive)
		LOG_FUNC_END;

	// TODO: Actually allow memory card emulation? This might make transferring
	// game saves a bit easier if the memory card directory was configurable. =]

	RETURN(E_FAIL);
}

// ******************************************************************
// * func: EmuXMountAlternateTitle
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XMountAlternateTitle)
(
	LPCSTR		lpRootPath,               
	DWORD		dwAltTitleId,               
	PCHAR		pchDrive               
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(lpRootPath)
		LOG_FUNC_ARG(dwAltTitleId)
		LOG_FUNC_ARG(pchDrive)
		LOG_FUNC_END;

	// TODO: Anything?
	LOG_UNIMPLEMENTED();

	RETURN(ERROR_SUCCESS);
}

// ******************************************************************
// * func: EmuXUnmountAlternateTitle
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XUnmountAlternateTitle)
(
	CHAR chDrive
)
{
	LOG_FUNC_ONE_ARG(chDrive);

	LOG_UNIMPLEMENTED();

	RETURN(ERROR_SUCCESS);
}

// ******************************************************************
// * func: EmuXGetDeviceEnumerationStatus
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XGetDeviceEnumerationStatus)()
{
	LOG_FUNC();

	LOG_UNIMPLEMENTED();

	RETURN(XDEVICE_ENUMERATION_IDLE);
}

// ******************************************************************
// * func: EmuXInputGetDeviceDescription
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XInputGetDeviceDescription)
(
    HANDLE	hDevice,
    PVOID	pDescription
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hDevice)
		LOG_FUNC_ARG(pDescription)
		LOG_FUNC_END;

	// TODO: Lightgun support?
	LOG_UNIMPLEMENTED();

	RETURN(ERROR_NOT_SUPPORTED); // ERROR_DEVICE_NOT_CONNECTED;
}

// ******************************************************************
// * func: EmuXAutoPowerDownResetTimer
// ******************************************************************
int WINAPI XTL::EMUPATCH(XAutoPowerDownResetTimer)()
{
	LOG_FUNC();

	// Meh, that's what the 'X' is for! =]
	LOG_UNIMPLEMENTED();

	RETURN(TRUE);
}

// ******************************************************************
// * func: EmuXMountMURootA
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XMountMURootA)
(
	DWORD dwPort,                  
	DWORD dwSlot,                  
	PCHAR pchDrive               
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(dwPort)
		LOG_FUNC_ARG(dwSlot)
		LOG_FUNC_ARG(pchDrive)
		LOG_FUNC_END;

	// TODO: The params are probably wrong...
	LOG_UNIMPLEMENTED();

	RETURN(ERROR_SUCCESS);
}
