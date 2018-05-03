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
#define _XBOXKRNL_DEFEXTRN_

#define LOG_PREFIX "XAPI"

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
#include <vector>

// XInputSetState status waiters
extern XInputSetStateStatus g_pXInputSetStateStatus[XINPUT_SETSTATE_SLOTS] = {0};

// XInputOpen handles
extern HANDLE g_hInputHandle[XINPUT_HANDLE_SLOTS] = {0};

bool g_bXInputOpenCalled = false;

XTL::PXPP_DEVICE_TYPE gDeviceType_Gamepad = nullptr;

#include "EmuXTL.h"


XTL::POLLING_PARAMETERS_HANDLE g_pph[4];
XTL::XINPUT_POLLING_PARAMETERS g_pp[4];
DWORD total_xinput_gamepad = 0;

void SetupXboxDeviceTypes()
{
	// If we don't yet have the offset to gDeviceType_Gamepad, work it out!
	if (gDeviceType_Gamepad == nullptr) {
		// First, attempt to find GetTypeInformation
		auto typeInformation = g_SymbolAddresses.find("GetTypeInformation");
		if (typeInformation != g_SymbolAddresses.end() && typeInformation->second != xbnull) {
			printf("Deriving XDEVICE_TYPE_GAMEPAD from DeviceTable (via GetTypeInformation)\n");
			// Read the offset values of the device table structure from GetTypeInformation
			xbaddr deviceTableStartOffset = *(uint32_t*)((uint32_t)typeInformation->second + 0x01);
			xbaddr deviceTableEndOffset = *(uint32_t*)((uint32_t)typeInformation->second + 0x09);

			// Calculate the number of device entires in the table
			size_t deviceTableEntryCount = (deviceTableEndOffset - deviceTableStartOffset) / sizeof(uint32_t);

			printf("DeviceTableStart: 0x%08X\n", deviceTableStartOffset);
			printf("DeviceTableEnd: 0x%08X\n", deviceTableEndOffset);
			printf("DeviceTable Entires: %u\n", deviceTableEntryCount);

			// Sanity check: Where all these device offsets within Xbox memory
			if ((deviceTableStartOffset >= g_SystemMaxMemory) || (deviceTableEndOffset >= g_SystemMaxMemory)) {
				CxbxKrnlCleanup("XAPI DeviceTable Location is outside of Xbox Memory range");
			}

			// Iterate through the table until we find gamepad
			XTL::PXID_TYPE_INFORMATION* deviceTable = (XTL::PXID_TYPE_INFORMATION*)(deviceTableStartOffset);
			for (unsigned int i = 0; i < deviceTableEntryCount; i++) {
				// Skip empty table entries
				if (deviceTable[i] == nullptr) {
					continue;
				}

				printf("----------------------------------------\n");
				printf("DeviceTable[%u]->ucType = %d\n", i, deviceTable[i]->ucType);
				printf("DeviceTable[%u]->XppType = 0x%08X (", i, deviceTable[i]->XppType);

				switch (deviceTable[i]->ucType) {
				case 1:
					gDeviceType_Gamepad = deviceTable[i]->XppType;
					printf("XDEVICE_TYPE_GAMEPAD)\n");
					break;
				default:
					printf("Unknown)\n");
					break;
				}
			}
		} else {
			// XDKs without GetTypeInformation have the GamePad address hardcoded in XInputOpen
			// Only the earliest XDKs use this code path, and the offset never changed between them
			// so this works well for us.
			void* XInputOpenAddr = (void*)g_SymbolAddresses["XInputOpen"];
			if (XInputOpenAddr != nullptr) {
				printf("XAPI: Deriving XDEVICE_TYPE_GAMEPAD from XInputOpen (0x%08X)\n", XInputOpenAddr);
				gDeviceType_Gamepad = *(XTL::PXPP_DEVICE_TYPE*)((uint32_t)XInputOpenAddr + 0x0B);
			}
		}

		if (gDeviceType_Gamepad == nullptr) {
			EmuWarning("XAPI: XDEVICE_TYPE_GAMEPAD was not found");
			return;
		}

		printf("XAPI: XDEVICE_TYPE_GAMEPAD Found at 0x%08X\n", gDeviceType_Gamepad);
	}
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
		LOG_FUNC_ARG((DWORD)PreallocTypes)
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
	
	if (g_XInputEnabled)
	{
		//query the total connected xinput gamepad.
		total_xinput_gamepad = XInputGamepad_Connected();
	}
	else 
	{
		//using keyboard, we set the gamd pad count to 1
		total_xinput_gamepad = 1;
	}
	
}

bool TitleIsJSRF()
{
	static bool detected = false;
	static bool result = false;

	// Prevent running the check every time this function is called
	if (detected) {
		return result;
	}

	// Array of known JSRF title IDs, must be 0 terminated
	DWORD titleIds[] = {
		0x49470018, // JSRF NTSC-U
		0x5345000A, // JSRF PAL, NTSC-J
		0x53450016, // JSRF NTSC - J(Demo)
		0
	};

	DWORD* pTitleId = &titleIds[0];
	while (*pTitleId != 0) {
		if (g_pCertificate->dwTitleId == *pTitleId) {
			result = true;
			break;
		}

		pTitleId++;
	}

	// We didn't find a known JSRF title id, fallback to checking the title
	// This isn't 100% effective, but could work for some versions of JSRF
	// Because of this, we log a message to say that the title_id should be added
	if (!result) {
		char tAsciiTitle[40] = "Unknown";
		setlocale(LC_ALL, "English");
		wcstombs(tAsciiTitle, g_pCertificate->wszTitleName, sizeof(tAsciiTitle));

		if (_strnicmp(tAsciiTitle, "Jet Set Radio", 13) == 0) {
			CxbxPopupMessage("Detected JSRF by name, not title ID, please report that [%08X] should be added to the list", g_pCertificate->dwTitleId);
			result = true;
		}
	}

	if (result) {
		EmuWarning("Applying JSRF Hack");
	}

	detected = true;
	return result;
}

// ******************************************************************
// * patch: XGetDevices
// * Note: This could be unpatched however,
// * XInitDevices is required to be unpatched first.
// * This in turn requires USB LLE to be implemented, or USBD_Init 
// * patched with a stub, so this patch is still enabled for now
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XGetDevices)
(
    PXPP_DEVICE_TYPE DeviceType
)
{
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(DeviceType);

	UCHAR oldIrql = xboxkrnl::KeRaiseIrqlToDpcLevel();

	DWORD ret = DeviceType->CurrentConnected;
	DeviceType->ChangeConnected = 0;
	DeviceType->PreviousConnected = DeviceType->CurrentConnected;

	// If this is for getting  gamepad devices, and no gamepad was previously detected, connect one
	if (DeviceType == gDeviceType_Gamepad && DeviceType->CurrentConnected == 0) {
		for (int i = 0; i < total_xinput_gamepad; i++)
		{
			DeviceType->CurrentConnected |= 1<<i;
		}
		DeviceType->ChangeConnected = DeviceType->CurrentConnected ;
		ret = DeviceType->CurrentConnected;
	}

	// JSRF Hack: Don't set the ChangeConnected flag. Without this, JSRF hard crashes 
	// TODO: Why is this still needed? 
	if (TitleIsJSRF()) {
		DeviceType->ChangeConnected = 0;
	}

	xboxkrnl::KfLowerIrql(oldIrql);

	RETURN(ret);
}

// ******************************************************************
// * patch: XGetDeviceChanges
// * Note: This could be unpatched however,
// * XInitDevices is required to be unpatched first.
// * This in turn requires USB LLE to be implemented, or USBD_Init 
// * patched with a stub, so this patch is still enabled for now
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

	// If this is a gamepad, and no gamepad was previously detected, connect one
	if (DeviceType == gDeviceType_Gamepad && DeviceType->CurrentConnected == 0) {
		DeviceType->CurrentConnected = 1;
		DeviceType->ChangeConnected = 1;		
	}

	// JSRF Hack: Don't set the ChangeConnected flag. Without this, JSRF hard crashes 
	// TODO: Why is this still needed? 
	if (TitleIsJSRF()) {
		DeviceType->ChangeConnected = 0;
	}

    if(!DeviceType->ChangeConnected)
    {
        *pdwInsertions = 0;
        *pdwRemovals = 0;
    }
    else
    {
		UCHAR oldIrql = xboxkrnl::KeRaiseIrqlToDpcLevel();

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

		xboxkrnl::KfLowerIrql(oldIrql);
    }

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
	//rever back to return handle  for port 0~3, this is for multi controller support.
    if(dwPort >= 0 && (dwPort <= total_xinput_gamepad))
    {
        if(g_hInputHandle[dwPort] == 0)
        {
            pph = (POLLING_PARAMETERS_HANDLE*) &g_pph[dwPort];	// new POLLING_PARAMETERS_HANDLE();

            if(pPollingParameters != NULL)
            {
                pph->pPollingParameters = (XINPUT_POLLING_PARAMETERS*) &g_pp[dwPort]; // new XINPUT_POLLING_PARAMETERS();

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
                    pph->pPollingParameters = (XINPUT_POLLING_PARAMETERS*) &g_pp[dwPort]; // new XINPUT_POLLING_PARAMETERS();
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
	DWORD dwPort = pph->dwPort;
	//NULL out the input handle corresponds to port.
	g_hInputHandle[dwPort] = 0;

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
		/*
        if(pph->pPollingParameters != NULL)
        {
            delete pph->pPollingParameters;
        }
		
        delete pph;
		*/
    }
    
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
            if ((HANDLE)g_pXInputSetStateStatus[v].hDevice == 0)
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
		//return gamepad capabilities for port 0~3.
        if(dwPort >= 0 && dwPort<=total_xinput_gamepad)
        {
            pCapabilities->SubType = XINPUT_DEVSUBTYPE_GC_GAMEPAD;
			pCapabilities->In.Gamepad = {};
			pCapabilities->Out.Rumble = {};

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

        if((dwPort >= 0) && (dwPort <= total_xinput_gamepad))
        {
			DbgPrintf("XAPI: EmuXInputGetState(): dwPort = %d\n", dwPort );

            //for xinput, we query the state corresponds to port.
				if (g_XInputEnabled) {
					EmuXInputPCPoll(dwPort,pState);
				} else {
					EmuDInputPoll(pState);
				}
				
                ret = ERROR_SUCCESS;
        }
    }
	else
		EmuWarning("EmuXInputGetState(): pph == NULL!");

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

		if (g_XInputEnabled)
		{
			XTL::EmuXInputSetState(pph->dwPort, pFeedback);
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
			CxbxKrnlCleanup("Too many thread notification routines installed\n");

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

typedef struct {
	LPFIBER_START_ROUTINE	lpStartRoutine;
	LPVOID					lpParameter;
} fiber_context_t;

void WINAPI EmuFiberStartup(fiber_context_t* context)
{
	__try
	{
		LPFIBER_START_ROUTINE pfStartRoutine = (LPFIBER_START_ROUTINE)context->lpStartRoutine;
		pfStartRoutine(context->lpParameter);
	}
	__except (EmuException(GetExceptionInformation()))
	{
		EmuWarning("Problem with ExceptionFilter");
	}
}

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

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(dwStackSize)
		LOG_FUNC_ARG((PVOID)lpStartRoutine)
		LOG_FUNC_ARG(lpParameter)
	LOG_FUNC_END;

	// Create a Fiber Context: This has to be malloced because if it goes out of scope
	// between CreateFiber and SwitchToFiber, it will cause a crash
	// WARNING: Currently this leaks memory, can be fixed by tracking fibers and freeing them in DeleteFiber
	fiber_context_t* context = (fiber_context_t*)malloc(sizeof(fiber_context_t));
	context->lpStartRoutine = lpStartRoutine;
	context->lpParameter = lpParameter;
		
	RETURN(CreateFiber(dwStackSize, (LPFIBER_START_ROUTINE)EmuFiberStartup, context));
}

// ******************************************************************
// * patch: DeleteFiber
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(DeleteFiber)
(
	LPVOID					lpFiber
)
{
	FUNC_EXPORTS
	LOG_FUNC_ONE_ARG((DWORD)DeleteFiber);

	DeleteFiber(lpFiber);
}

// ******************************************************************
// * patch: SwitchToFiber
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(SwitchToFiber)
(
	LPVOID lpFiber 
)
{
	FUNC_EXPORTS
	LOG_FUNC_ONE_ARG(lpFiber);

	SwitchToFiber(lpFiber);
}

// ******************************************************************
// * patch: ConvertThreadToFiber
// ******************************************************************
LPVOID WINAPI XTL::EMUPATCH(ConvertThreadToFiber)
(
	LPVOID lpParameter
)
{
	FUNC_EXPORTS
	LOG_FUNC_ONE_ARG(lpParameter);
		
	LPVOID pRet = ConvertThreadToFiber(lpParameter);
	
	RETURN(pRet);
}

// ******************************************************************
// * patch: QueryPerformanceCounter
// ******************************************************************
BOOL WINAPI XTL::EMUPATCH(QueryPerformanceCounter)
(
	LARGE_INTEGER * lpPerformanceCount
)
{
	FUNC_EXPORTS;
	
	lpPerformanceCount->QuadPart = xboxkrnl::KeQueryPerformanceCounter();
	return TRUE;
}

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
	//FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_TYPE(PVOID, pfnAPC)
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
	//FUNC_EXPORTS

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
		if (xboxkrnl::LaunchDataPage == xbnullptr)
		{
			PVOID LaunchDataVAddr = xboxkrnl::MmAllocateContiguousMemory(sizeof(xboxkrnl::LAUNCH_DATA_PAGE));
			if (!LaunchDataVAddr)
			{
				RETURN(STATUS_NO_MEMORY);
			}
			xboxkrnl::LaunchDataPage = (xboxkrnl::LAUNCH_DATA_PAGE*)LaunchDataVAddr;
		}

		xboxkrnl::LaunchDataPage->Header.dwTitleId = g_pCertificate->dwTitleId;
		xboxkrnl::LaunchDataPage->Header.dwFlags = 0; // TODO : What to put in here?
		xboxkrnl::LaunchDataPage->Header.dwLaunchDataType = LDT_TITLE;

		xboxkrnl::MmPersistContiguousMemory((PVOID)xboxkrnl::LaunchDataPage, PAGE_SIZE, TRUE);

		if (pLaunchData != xbnullptr)
			// Save the launch data
			memcpy(&(xboxkrnl::LaunchDataPage->LaunchData[0]), pLaunchData, sizeof(LAUNCH_DATA));

		if (lpTitlePath == xbnullptr)
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
	LOG_TEST_CASE("Unpatching test needed");

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pdwLaunchDataType)
		LOG_FUNC_ARG(pLaunchData)
		LOG_FUNC_END;

	DWORD ret = ERROR_NOT_FOUND;

	if (xboxkrnl::LaunchDataPage != NULL)
	{
		// Note : Here, CxbxRestoreLaunchDataPage() was already called,
		// which has loaded LaunchDataPage from a binary file (if present).

		// A title can pass data only to itself, not another title (unless started from the dashboard, of course) :
		if (   (xboxkrnl::LaunchDataPage->Header.dwTitleId == g_pCertificate->dwTitleId)
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
		LOG_FUNC_ARG_TYPE(PVOID, fptc)
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
