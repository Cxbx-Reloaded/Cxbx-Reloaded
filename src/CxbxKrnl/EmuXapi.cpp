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
#include "EmuKrnl.h" // For DefaultLaunchDataPage
#include "EmuFile.h"
#include "EmuFS.h"
#include "EmuShared.h"
#include "HLEIntercept.h"

// XInputSetState status waiters
extern XInputSetStateStatus g_pXInputSetStateStatus[XINPUT_SETSTATE_SLOTS] = {0};

// XInputOpen handles
extern HANDLE g_hInputHandle[XINPUT_HANDLE_SLOTS] = {0};

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
// * patch: XFormatUtilityDrive
// ******************************************************************
BOOL WINAPI XTL::EMUPATCH(XFormatUtilityDrive)()
{
	FUNC_EXPORTS

	LOG_FUNC();

    // TODO: yeah... we'll format... riiiiight

	RETURN(TRUE);
}

// ******************************************************************
// * patch: XMountUtilityDrive
// ******************************************************************
BOOL WINAPI XTL::EMUPATCH(XMountUtilityDrive)
(
    BOOL    fFormatClean
)
{
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(fFormatClean);

	CxbxMountUtilityDrive(fFormatClean == TRUE);

	RETURN(TRUE);
}

// ******************************************************************
// * patch: XInitDevices
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(XInitDevices)
(
    DWORD					dwPreallocTypeCount,
	PXDEVICE_PREALLOC_TYPE	PreallocTypes
)
{
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(dwPreallocTypeCount)
		LOG_FUNC_ARG(PreallocTypes)
		LOG_FUNC_END;

    for(int v=0;v<XINPUT_SETSTATE_SLOTS;v++)
    {
        g_pXInputSetStateStatus[v].hDevice = 0;
        g_pXInputSetStateStatus[v].dwLatency = 0;
        g_pXInputSetStateStatus[v].pFeedback = 0;
    }

    for(int v=0;v<XINPUT_HANDLE_SLOTS;v++)
    {
        g_hInputHandle[v] = 0;
    }
}

// ******************************************************************
// * patch: XGetDevices
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XGetDevices)
(
    PXPP_DEVICE_TYPE DeviceType
)
{
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(DeviceType);

	if (DeviceType->CurrentConnected == 0) {
		DeviceType->CurrentConnected = 1;
	}
		
    DWORD ret = DeviceType->CurrentConnected;
	DeviceType->ChangeConnected = 0;
	DeviceType->PreviousConnected = DeviceType->CurrentConnected;

	RETURN(ret);
}

// ******************************************************************
// * patch: XGetDeviceChanges
// ******************************************************************
BOOL WINAPI XTL::EMUPATCH(XGetDeviceChanges)
(
    PXPP_DEVICE_TYPE DeviceType,
    PDWORD           pdwInsertions,
    PDWORD           pdwRemovals
)
{
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(DeviceType)
		LOG_FUNC_ARG(pdwInsertions)
		LOG_FUNC_ARG(pdwRemovals)
	LOG_FUNC_END;

	BOOL ret = FALSE;
	
	// JSRF Hack: Always return no device changes
	// Without this, JSRF hard crashes sometime after calling this function
	// I HATE game specific hacks, but I've wasted three weeks trying to solve this already
	// TitleID 0x49470018 = JSRF NTSC-U
	// TitleID 0x5345000A = JSRF PAL, NTSC-J
	// TitleID 0x53450016 = JSRF NTSC-J (Demo)
	// ~Luke Usher
	Xbe::Certificate *pCertificate = (Xbe::Certificate*)CxbxKrnl_XbeHeader->dwCertificateAddr;
	if (pCertificate->dwTitleId == 0x49470018 || pCertificate->dwTitleId == 0x5345000A || pCertificate->dwTitleId == 0x53450016) {
		RETURN(ret);
	}

	// If we have no connected devices, report one insertion
	if (DeviceType->CurrentConnected == 0) {
		*pdwInsertions = 1;
		ret = TRUE;
	} else	{
		// Otherwise, report no changes
		*pdwInsertions = 0;
	}

	*pdwRemovals = 0;  

/*
    if(!DeviceType->ChangeConnected)
    {
        *pdwInsertions = 0;
        *pdwRemovals = 0;
    }
    else
    {
        *pdwInsertions = (DeviceType->CurrentConnected & ~DeviceType->PreviousConnected);
        *pdwRemovals = (DeviceType->PreviousConnected & ~DeviceType->CurrentConnected);
        ULONG RemoveInsert = DeviceType->ChangeConnected &
            DeviceType->CurrentConnected &
            DeviceType->PreviousConnected;
        *pdwRemovals |= RemoveInsert;
        *pdwInsertions |= RemoveInsert;
        DeviceType->ChangeConnected = 0;
        DeviceType->PreviousConnected = DeviceType->CurrentConnected;
        ret = (*pdwInsertions | *pdwRemovals) ? TRUE : FALSE;
    }
*/
	RETURN(ret);
}

// ******************************************************************
// * patch: XInputOpen
// ******************************************************************
HANDLE WINAPI XTL::EMUPATCH(XInputOpen)
(
    IN PXPP_DEVICE_TYPE             DeviceType,
    IN DWORD                        dwPort,
    IN DWORD                        dwSlot,
    IN PXINPUT_POLLING_PARAMETERS   pPollingParameters OPTIONAL
)
{
	FUNC_EXPORTS

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
// * patch: XInputClose
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(XInputClose)
(
    IN HANDLE hDevice
)
{
	FUNC_EXPORTS

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
// * patch: XInputPoll
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XInputPoll)
(
    IN HANDLE hDevice
)
{
	FUNC_EXPORTS

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
// * patch: XInputGetCapabilities
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XInputGetCapabilities)
(
    IN  HANDLE               hDevice,
    OUT PXINPUT_CAPABILITIES pCapabilities
)
{
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hDevice)
		LOG_FUNC_ARG_OUT(pCapabilities)
		LOG_FUNC_END;

    DWORD ret = ERROR_DEVICE_NOT_CONNECTED;

    POLLING_PARAMETERS_HANDLE *pph = (POLLING_PARAMETERS_HANDLE*)hDevice;

    if(pph != NULL)
    {
        DWORD dwPort = pph->dwPort;

        if(dwPort == 0)
        {
            pCapabilities->SubType = XINPUT_DEVSUBTYPE_GC_GAMEPAD;
			ZeroMemory(&pCapabilities->In.Gamepad, sizeof(pCapabilities->In.Gamepad));
            ret = ERROR_SUCCESS;
        }
    }

	RETURN(ret);
}

// ******************************************************************
// * patch: InputGetState
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XInputGetState)
(
    IN  HANDLE         hDevice,
    OUT PXINPUT_STATE  pState
)
{
	FUNC_EXPORTS

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
				if (g_XInputEnabled) {
					EmuXInputPCPoll(pState);
				} else {
					EmuDInputPoll(pState);
				}
				
                ret = ERROR_SUCCESS;
            }
        }
    }
	else
		EmuWarning( "EmuXInputGetState(): pph == NULL!" );

	RETURN(ret);
}

// ******************************************************************
// * patch: InputSetState
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XInputSetState)
(
    IN     HANDLE           hDevice,
    IN OUT PXINPUT_FEEDBACK pFeedback
)
{
	FUNC_EXPORTS

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
// * patch: SetThreadPriorityBoost
// ******************************************************************
BOOL WINAPI XTL::EMUPATCH(SetThreadPriorityBoost)
(
    HANDLE  hThread,
    BOOL    DisablePriorityBoost
)
{
	FUNC_EXPORTS

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
// * patch: SetThreadPriority
// ******************************************************************
BOOL WINAPI XTL::EMUPATCH(SetThreadPriority)
(
    HANDLE  hThread,
    int     nPriority
)
{
	FUNC_EXPORTS

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
// * patch: GetThreadPriority
// ******************************************************************
int WINAPI XTL::EMUPATCH(GetThreadPriority)
(
    HANDLE  hThread
)
{
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(hThread);

    int iRet = GetThreadPriority(hThread);

    if(iRet == THREAD_PRIORITY_ERROR_RETURN)
        EmuWarning("GetThreadPriority Failed!");

	RETURN(iRet);
}

// ******************************************************************
// * patch: GetExitCodeThread
// ******************************************************************
BOOL WINAPI XTL::EMUPATCH(GetExitCodeThread)
(
    HANDLE  hThread,
    LPDWORD lpExitCode
)
{
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hThread)
		LOG_FUNC_ARG(lpExitCode)
		LOG_FUNC_END;

    BOOL bRet = GetExitCodeThread(hThread, lpExitCode);

	RETURN(bRet);
}

// ******************************************************************
// * patch: XapiThreadStartup
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(XapiThreadStartup)
(
    DWORD dwDummy1,
    DWORD dwDummy2
)
{
	//FUNC_EXPORTS

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
// * patch: XRegisterThreadNotifyRoutine
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(XRegisterThreadNotifyRoutine)
(
    PXTHREAD_NOTIFICATION   pThreadNotification,
    BOOL                    fRegister
)
{
	//FUNC_EXPORTS

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

#if 0 // patch disabled
// ******************************************************************
// * patch: CreateFiber
// ******************************************************************
LPVOID WINAPI XTL::EMUPATCH(CreateFiber)
(
	DWORD					dwStackSize,
	LPFIBER_START_ROUTINE	lpStartRoutine,
	LPVOID					lpParameter
)
{
	FUNC_EXPORTS

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
#endif

#if 0 // patch disabled
// ******************************************************************
// * patch: DeleteFiber
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(DeleteFiber)
(
	LPVOID					lpFiber
)
{
	FUNC_EXPORTS

	DbgPrintf("EmuXapi: EmuDeleteFiber\n"
			"(\n"
			"	lpFiber            : 0x%.08X\n"
			");\n",
			lpFiber );

	DeleteFiber( lpFiber );

}
#endif

#if 0 // patch disabled
// ******************************************************************
// * patch: SwitchToFiber
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(SwitchToFiber)
(
	LPVOID lpFiber 
)
{
	FUNC_EXPORTS

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
#endif

#if 0 // patch disabled
// ******************************************************************
// * patch: ConvertThreadToFiber
// ******************************************************************
LPVOID WINAPI XTL::EMUPATCH(ConvertThreadToFiber)
(
	LPVOID lpParameter
)
{
	FUNC_EXPORTS

	DbgPrintf("EmuXapi: EmuConvertThreadToFiber\n"
			"(\n"
			"	lpParameter        : 0x%.08X\n"
			");\n",
			lpParameter );

	LPVOID pRet = ConvertThreadToFiber( lpParameter );
	
	DbgPrintf( "EmuConvertThreadToFiber returned 0x%X\n", pRet );


	return pRet;
}
#endif

#if 0 // patch disabled
// ******************************************************************
// * patch: XapiFiberStartup
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(XapiFiberStartup)(DWORD dwDummy)
{
	FUNC_EXPORTS

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
#endif

// ******************************************************************
// * patch: QueueUserAPC
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(QueueUserAPC)
(
	PAPCFUNC	pfnAPC,
	HANDLE		hThread,
	DWORD		dwData
)
{
	FUNC_EXPORTS

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

#if 0 // Handled by WaitForSingleObject
// ******************************************************************
// * patch: GetOverlappedResult
// ******************************************************************
BOOL WINAPI XTL::EMUPATCH(GetOverlappedResult)
(
	HANDLE			hFile,
	LPOVERLAPPED	lpOverlapped,
	LPDWORD			lpNumberOfBytesTransferred,
	BOOL			bWait
)
{
	//FUNC_EXPORTS

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
#endif

// ******************************************************************
// * patch: XLaunchNewImageA
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XLaunchNewImageA)
(
	LPCSTR			lpTitlePath,
	PLAUNCH_DATA	pLaunchData
)
{
	FUNC_EXPORTS

	// Note : This can be tested using "Innocent tears",
	// which relaunches different xbes between scenes;
	// One for menus, one for fmvs, etc.
	//
	// Other titles do this too (like "DOA2 Ultimate",
	// and probably "Panzer Dragoon Orta"), but these
	// titles don't come this far as-of yet.

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(lpTitlePath)
		LOG_FUNC_ARG(pLaunchData)
		LOG_FUNC_END;

	// TODO : This patch can be removed once NtOpenSymbolicLinkObject()
	// and NtQuerySymbolicLinkObject() work together correctly.
	// Also, XLaunchNewImageA() depends on XeImageHeader() and uses
	// XWriteTitleInfoAndReboot() and indirectly XWriteTitleInfoNoReboot()

	// Update the kernel's LaunchDataPage :
	{
		if (xboxkrnl::LaunchDataPage == &DefaultLaunchDataPage)
			xboxkrnl::LaunchDataPage = NULL;

		if (xboxkrnl::LaunchDataPage == NULL)
			xboxkrnl::LaunchDataPage = (xboxkrnl::LAUNCH_DATA_PAGE *)xboxkrnl::MmAllocateContiguousMemory(sizeof(xboxkrnl::LAUNCH_DATA_PAGE));

		Xbe::Certificate *pCertificate = (Xbe::Certificate*)CxbxKrnl_XbeHeader->dwCertificateAddr;
		xboxkrnl::LaunchDataPage->Header.dwTitleId = pCertificate->dwTitleId;
		xboxkrnl::LaunchDataPage->Header.dwFlags = 0; // TODO : What to put in here?
		xboxkrnl::LaunchDataPage->Header.dwLaunchDataType = LDT_TITLE;

		if (pLaunchData != NULL)
			// Save the launch data
			memcpy(&(xboxkrnl::LaunchDataPage->LaunchData[0]), pLaunchData, sizeof(LAUNCH_DATA));

		if (lpTitlePath == NULL)
		{
			// If no path is specified, then the xbe is rebooting to dashboard
			char szDashboardPath[MAX_PATH] = { 0 };
			XboxDevice* rootDevice = CxbxDeviceByDevicePath(DeviceHarddisk0Partition2);
			if (rootDevice != nullptr)
				sprintf(szDashboardPath, "%s\\xboxdash.xbe", rootDevice->HostDevicePath.c_str());

			if (PathFileExists(szDashboardPath))
			{
				MessageBox(CxbxKrnl_hEmuParent, "The title is rebooting to dashboard", "Cxbx-Reloaded", 0);
				lpTitlePath = "C:\\xboxdash.xbe";
				xboxkrnl::LaunchDataPage->Header.dwLaunchDataType = LDT_FROM_DASHBOARD;
				// Other options include LDT_NONE, LDT_FROM_DEBUGGER_CMDLINE and LDT_FROM_UPDATE
			}
			else
				CxbxKrnlCleanup("The xbe rebooted to Dashboard and xboxdash.xbe could not be found");
		}

		strncpy(&(xboxkrnl::LaunchDataPage->Header.szLaunchPath[0]), lpTitlePath, 520);
	}

	// Note : While this patch exists, HalReturnToFirmware() calls
	// MmPersistContiguousMemory on LaunchDataPage. When this
	// patch on XLaunchNewImageA is removed, remove the call to
	// MmPersistContiguousMemory from HalReturnToFirmware() too!!

	xboxkrnl::HalReturnToFirmware(xboxkrnl::ReturnFirmwareQuickReboot);

	// If this function succeeds, it doesn't get a chance to return anything.
	RETURN(ERROR_GEN_FAILURE);
}

DWORD g_XGetLaunchInfo_Status = -1;

#if 0 // patch disabled
// ******************************************************************
// * patch: XGetLaunchInfo
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XGetLaunchInfo)
(
	PDWORD			pdwLaunchDataType,
	PLAUNCH_DATA	pLaunchData
)
{
	FUNC_EXPORTS
	// TODO : This patch can be removed once we're sure all XAPI library
	// functions indirectly reference our xboxkrnl::LaunchDataPage variable.
	// For this, we need a test-case that hits this function, and run that
	// with and without this patch enabled. Behavior should be identical.
	// When this is verified, this patch can be removed.

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pdwLaunchDataType)
		LOG_FUNC_ARG(pLaunchData)
		LOG_FUNC_END;

	DWORD ret = ERROR_NOT_FOUND;

	if (xboxkrnl::LaunchDataPage != NULL)
	{
		// Note : Here, CxbxRestoreLaunchDataPage() was already called,
		// which has loaded LaunchDataPage from a binary file (if present).

		Xbe::Certificate *pCertificate = (Xbe::Certificate*)CxbxKrnl_XbeHeader->dwCertificateAddr;

		// A title can pass data only to itself, not another title (unless started from the dashboard, of course) :
		if (   (xboxkrnl::LaunchDataPage->Header.dwTitleId == pCertificate->dwTitleId)
			|| (xboxkrnl::LaunchDataPage->Header.dwLaunchDataType == LDT_FROM_DASHBOARD)
			|| (xboxkrnl::LaunchDataPage->Header.dwLaunchDataType == LDT_FROM_DEBUGGER_CMDLINE))
		{
			*pdwLaunchDataType = xboxkrnl::LaunchDataPage->Header.dwLaunchDataType;
			memcpy(pLaunchData, &(xboxkrnl::LaunchDataPage->LaunchData[0]), sizeof(LAUNCH_DATA));

			// Now that LaunchDataPage is retrieved by the emulated software, free it :
			MmFreeContiguousMemory(xboxkrnl::LaunchDataPage);
			xboxkrnl::LaunchDataPage = NULL;

			ret = ERROR_SUCCESS;
		}
	}

	RETURN(ret);
}
#endif

// ******************************************************************
// * patch: XSetProcessQuantumLength
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(XSetProcessQuantumLength)
(
    DWORD dwMilliseconds
)
{
	FUNC_EXPORTS
	LOG_FUNC_ONE_ARG(dwMilliseconds);

	// TODO: Implement?
	LOG_IGNORED();
}
	
// ******************************************************************
// * patch: SignalObjectAndWait
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(SignalObjectAndWait)
(
	HANDLE	hObjectToSignal,
	HANDLE	hObjectToWaitOn,
	DWORD	dwMilliseconds,
	BOOL	bAlertable
)
{
	FUNC_EXPORTS
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
// * patch: timeSetEvent
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
	FUNC_EXPORTS

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
// * patch: timeKillEvent
// ******************************************************************
MMRESULT WINAPI XTL::EMUPATCH(timeKillEvent)
(
	UINT uTimerID  
)
{
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(uTimerID);

	MMRESULT Ret = timeKillEvent( uTimerID );

	RETURN(Ret);
}

// ******************************************************************
// * patch: RaiseException
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(RaiseException)
(
	DWORD			dwExceptionCode,       // exception code
	DWORD			dwExceptionFlags,      // continuable exception flag
	DWORD			nNumberOfArguments,    // number of arguments
	CONST ULONG_PTR *lpArguments		   // array of arguments
)
{
	FUNC_EXPORTS

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
// patch: XMountMUA
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XMountMUA)
(
	DWORD dwPort,                  
	DWORD dwSlot,                  
	PCHAR pchDrive               
)
{
	FUNC_EXPORTS

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
// * patch: XMountAlternateTitleA
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XMountAlternateTitleA)
(
	LPCSTR		lpRootPath,               
	DWORD		dwAltTitleId,               
	PCHAR		pchDrive               
)
{
	FUNC_EXPORTS

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
// * patch: XUnmountAlternateTitleA
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XUnmountAlternateTitleA)
(
	CHAR chDrive
)
{
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(chDrive);

	LOG_UNIMPLEMENTED();

	RETURN(ERROR_SUCCESS);
}

// ******************************************************************
// * patch: XGetDeviceEnumerationStatus
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XGetDeviceEnumerationStatus)()
{
	FUNC_EXPORTS

	LOG_FUNC();

	LOG_UNIMPLEMENTED();

	RETURN(XDEVICE_ENUMERATION_IDLE);
}

// ******************************************************************
// * patch: XInputGetDeviceDescription
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XInputGetDeviceDescription)
(
    HANDLE	hDevice,
    PVOID	pDescription
)
{
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hDevice)
		LOG_FUNC_ARG(pDescription)
		LOG_FUNC_END;

	// TODO: Lightgun support?
	LOG_UNIMPLEMENTED();

	RETURN(ERROR_NOT_SUPPORTED); // ERROR_DEVICE_NOT_CONNECTED;
}

// ******************************************************************
// * patch: XMountMURootA
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XMountMURootA)
(
	DWORD dwPort,                  
	DWORD dwSlot,                  
	PCHAR pchDrive               
)
{
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(dwPort)
		LOG_FUNC_ARG(dwSlot)
		LOG_FUNC_ARG(pchDrive)
		LOG_FUNC_END;

	// TODO: The params are probably wrong...
	LOG_UNIMPLEMENTED();

	RETURN(ERROR_SUCCESS);
}

// ******************************************************************
// * patch: OutputDebugStringA
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(OutputDebugStringA)
(
	IN LPCSTR lpOutputString
)
{
	FUNC_EXPORTS
	LOG_FUNC_ONE_ARG(lpOutputString);
	printf("OutputDebugStringA: %s\n", lpOutputString);
}
