// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
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

#define LOG_PREFIX CXBXR_MODULE::XAPI

#undef FIELD_OFFSET     // prevent macro redefinition warnings
/* prevent name collisions */
namespace xboxkrnl
{
	#include <xboxkrnl\xboxkrnl.h>
};

#include <Shlwapi.h>
#include "core\kernel\init\CxbxKrnl.h"
#include "Logging.h"
#include "core\kernel\support\Emu.h"
#include "core\kernel\exports\EmuKrnl.h" // For DefaultLaunchDataPage
#include "core\kernel\support\EmuFile.h"
#include "core\kernel\support\EmuFS.h"
#include "core\kernel\support\EmuXTL.h"
#include "EmuShared.h"
#include "..\common\win32\XBPortMapping.h"
#include "core\hle\Intercept.hpp"
#include "vsbc\CxbxVSBC.h"
#include "Windef.h"
#include <vector>

// XInputSetState status waiters
extern XInputSetStateStatus g_pXInputSetStateStatus[XINPUT_SETSTATE_SLOTS] = {0};

// XInputOpen handles
extern HANDLE g_hInputHandle[XINPUT_HANDLE_SLOTS] = {0};

bool g_bXInputOpenCalled = false;
bool g_bCxbxVSBCLoaded = false;
HINSTANCE g_module;
typedef int (FAR WINAPI *PFARPROC1)(int);
typedef int (FAR WINAPI *PFARPROC2)(UCHAR*);
typedef int (NEAR WINAPI *PNEARPROC1)(int);
typedef int (NEAR WINAPI *PNEARPROC2)(UCHAR*);
typedef int (WINAPI *PPROC)();

PFARPROC2 fnCxbxVSBCSetState;
PFARPROC2 fnCxbxVSBCGetState;
PFARPROC1 fnCxbxVSBCOpen;
//typedef DWORD(*fnCxbxVSBCOpen)(HWND);
//typedef DWORD(*fnCxbxVSBCSetState)(UCHAR *);
//typedef DWORD(*fnCxbxVSBCGetState)(UCHAR *);
XTL::PXPP_DEVICE_TYPE gDeviceType_Gamepad = nullptr;


XTL::X_POLLING_PARAMETERS_HANDLE g_pph[4];
XTL::X_XINPUT_POLLING_PARAMETERS g_pp[4];
//for host connected gamepad
DWORD total_xinput_gamepad = 0;
//global bridge for xbox controller to host, 4 elements for 4 ports.
XTL::X_CONTROLLER_HOST_BRIDGE g_XboxControllerHostBridge[4] = {};
//global xbox xinput device info from interpreting device table.
std::vector<XTL::X_XINPUT_DEVICE_INFO> g_XboxInputDeviceInfo;



//look for xbox Device info from global info vector, and return the found index. return -1 for not found.
int FindDeviceInfoIndexByXboxType(UCHAR ucType)
{
    size_t i;
    for (i = 0; i < g_XboxInputDeviceInfo.size(); i++) {
        if (g_XboxInputDeviceInfo[i].ucType == ucType) {
            return i;
        }
    }
    return -1;
}

//look for xbox Device info from global info vector, and return the found index. return -1 for not found.
int FindDeviceInfoIndexByDeviceType(XTL::PXPP_DEVICE_TYPE DeviceType)
{
    size_t i;
    for (i = 0; i < g_XboxInputDeviceInfo.size(); i++) {
        if (g_XboxInputDeviceInfo[i].DeviceType == DeviceType) {
            return i;
        }
    }
    return -1;
}

//init XboxControllerHostBridge's content, also put interpreted data from device table to it.
//this is called in the end of SetupXboxDeviceTypes(), later we'll move this code to accept user configuration.
void InitXboxControllerHostBridge(void)
{
    //load host type and port configuration from settings.
    Settings::s_controller_port ControllerPortMap;
    g_EmuShared->GetControllerPortSettings(&ControllerPortMap);
    XBPortMappingSet(ControllerPortMap);
    total_xinput_gamepad = XTL::XInputGamepad_Connected();
    
    int port;
    for (port = 0; port < 4; port++) {
        g_XboxControllerHostBridge[port].dwHostType = GetXboxPortMapHostType(port);
        g_XboxControllerHostBridge[port].dwHostPort = GetXboxPortMapHostPort(port);
        g_XboxControllerHostBridge[port].XboxDeviceInfo.ucType = X_XINPUT_DEVTYPE_GAMEPAD;
        switch (GetXboxPortMapHostType(port)) {
        case X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT:
            //disconnect to host if the host port of xinput exceeds the total xinput controller connected to host.
            if (g_XboxControllerHostBridge[port].dwHostPort >= total_xinput_gamepad) {
                g_XboxControllerHostBridge[port].dwHostType = X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_NOTCONNECT;
                printf("InitXboxControllerHostBridge: Host XInput port greater then total xinput controller connected. disconnect xbox port from host!\n");
            }
            break;
        case X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_DINPUT:
            break;
        case X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_VIRTUAL_SBC:
            g_XboxControllerHostBridge[port].XboxDeviceInfo.ucType = X_XINPUT_DEVTYPE_STEELBATALION;
            break;
        default:
            break;
        }
        g_XboxControllerHostBridge[port].dwXboxPort = port;
        //xbox device handle set to 0 before being open.
        g_XboxControllerHostBridge[port].hXboxDevice = 0;
        int index;
        //find corresponding XboxDeviceInfo
        index=FindDeviceInfoIndexByXboxType(g_XboxControllerHostBridge[port].XboxDeviceInfo.ucType);
        if (index == -1) {
            printf("XboxControllerHostBridge XboxDeviceInfo.ucType: %d not found in global XboxInputDeviceInfo vector!\n", g_XboxControllerHostBridge[port].XboxDeviceInfo.ucType);
        }
        //copy XboxDeviceInfo from the global vector.
        g_XboxControllerHostBridge[port].XboxDeviceInfo.ucSubType = g_XboxInputDeviceInfo[index].ucSubType;
        g_XboxControllerHostBridge[port].XboxDeviceInfo.DeviceType = g_XboxInputDeviceInfo[index].DeviceType;
        g_XboxControllerHostBridge[port].XboxDeviceInfo.ucInputStateSize = g_XboxInputDeviceInfo[index].ucInputStateSize;
        g_XboxControllerHostBridge[port].XboxDeviceInfo.ucFeedbackSize = g_XboxInputDeviceInfo[index].ucFeedbackSize;
        //these two members are not used yet.
        g_XboxControllerHostBridge[port].pXboxState = 0;
        g_XboxControllerHostBridge[port].pXboxFeedbackHeader = 0;
    }
}
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
				CxbxKrnlCleanup("DeviceTable Location is outside of Xbox Memory range");
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

                XTL::X_XINPUT_DEVICE_INFO CurrentInfo = {};
                CurrentInfo.ucType = deviceTable[i]->ucType;
                CurrentInfo.DeviceType = deviceTable[i]->XppType;
                if (deviceTable[i]->pInputStateDesc!=0) {
                    CurrentInfo.ucInputStateSize = deviceTable[i]->pInputStateDesc->ucSize;
                }
                if(deviceTable[i]->pFeedbackDesc!=0){
                    CurrentInfo.ucFeedbackSize = deviceTable[i]->pFeedbackDesc->ucSize;
                }

                switch (deviceTable[i]->ucType) {
				case X_XINPUT_DEVTYPE_GAMEPAD:
					gDeviceType_Gamepad = deviceTable[i]->XppType;
					CurrentInfo.ucSubType = X_XINPUT_DEVSUBTYPE_GC_GAMEPAD;
					printf("XDEVICE_TYPE_GAMEPAD)\n");
					break;
                case X_XINPUT_DEVTYPE_STEELBATALION:
                    printf("XDEVICE_TYPE_STEELBATALION)\n");
					CurrentInfo.ucSubType = X_XINPUT_DEVSUBTYPE_GC_GAMEPAD_ALT;
                    break;
				default:
					printf("Unknown device type)\n");
					continue;
				}

				//store the DeviceInfo in global vector.
				g_XboxInputDeviceInfo.push_back(CurrentInfo);
			}
		} else {
			// XDKs without GetTypeInformation have the GamePad address hardcoded in XInputOpen
			// Only the earliest XDKs use this code path, and the offset never changed between them
			// so this works well for us.
			void* XInputOpenAddr = (void*)g_SymbolAddresses["XInputOpen"];
			if (XInputOpenAddr != nullptr) {
				printf("XAPI: Deriving XDEVICE_TYPE_GAMEPAD from XInputOpen (0x%08X)\n", XInputOpenAddr);
				gDeviceType_Gamepad = *(XTL::PXPP_DEVICE_TYPE*)((uint32_t)XInputOpenAddr + 0x0B);

                //only have one GAMEPAD device type. setup global DeviceInfo vector accordingly.
                XTL::X_XINPUT_DEVICE_INFO CurrentInfo = {};
                CurrentInfo.ucType = X_XINPUT_DEVTYPE_GAMEPAD;
                CurrentInfo.ucSubType = X_XINPUT_DEVSUBTYPE_GC_GAMEPAD;
                CurrentInfo.DeviceType = gDeviceType_Gamepad;
                CurrentInfo.ucInputStateSize = sizeof(XTL::X_XINPUT_GAMEPAD);
                CurrentInfo.ucFeedbackSize = sizeof(XTL::X_XINPUT_RUMBLE);
                //store the DeviceInfo in global vector.
                g_XboxInputDeviceInfo.push_back(CurrentInfo);
			}
		}

		if (gDeviceType_Gamepad == nullptr) {
			EmuLog(LOG_LEVEL::WARNING, "XDEVICE_TYPE_GAMEPAD was not found");
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


	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(dwPreallocTypeCount)
		LOG_FUNC_ARG((DWORD)PreallocTypes)
		LOG_FUNC_END;

/*    for(int v=0;v<XINPUT_SETSTATE_SLOTS;v++)
    {
        g_pXInputSetStateStatus[v].hDevice = 0;
        g_pXInputSetStateStatus[v].dwLatency = 0;
        g_pXInputSetStateStatus[v].pFeedback = 0;
    }

    for(int v=0;v<XINPUT_HANDLE_SLOTS;v++)
    {
        g_hInputHandle[v] = 0;
    }
*/

	InitXboxControllerHostBridge();
}

bool TitleRequiresInputHack()
{
	static bool detected = false;
	static bool result = false;

	// Prevent running the check every time this function is called
	if (detected) {
		return result;
	}

	// Array of known games that require the gamepad hack
	// TODO: Figure out WHY this is needed and fix the root cause
	// Perhaps LLE USB/OHCI is required?
	DWORD titleIds[] = {
		0x49470018, // JSRF NTSC-U
		0x5345000A, // JSRF PAL, NTSC-J
		0x53450016, // JSRF NTSC - J(Demo)
		0x5345000b, // GunValkyre NTSC-U
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

	if (result) {
		EmuLog(LOG_LEVEL::WARNING, "Applying Smilebit Input Hack");
	}

	detected = true;
	return result;
}

bool TitleIsLegoSW()
{
	static bool detected = false;
	static bool result = false;

	// Prevent running the check every time this function is called
	if (detected) {
		return result;
	}

	// Array of known Lego Star Wars title IDs, must be 0 terminated
	DWORD titleIds[] = {
		0x4553001D, // v1.01 - PAL; v1.02 - NTSC
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
		
	if (result) {
		EmuLog(LOG_LEVEL::WARNING, "Applying Lego Star Wars Hack");
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


	LOG_FUNC_ONE_ARG(DeviceType);

	UCHAR oldIrql = xboxkrnl::KeRaiseIrqlToDpcLevel();

	DWORD ret = DeviceType->CurrentConnected;
	DeviceType->ChangeConnected = 0;
	DeviceType->PreviousConnected = DeviceType->CurrentConnected;

    int index = FindDeviceInfoIndexByDeviceType(DeviceType);
    int port;
    if (DeviceType->CurrentConnected == 0) {
        for (port = 0; port < 4; port++) {
            //if the host controller is connected and the xbox DeviceType matches. set the CurrentConnected flag.
            if (g_XboxControllerHostBridge[port].XboxDeviceInfo.DeviceType == DeviceType && g_XboxControllerHostBridge[port].dwHostType>0) {
                DeviceType->CurrentConnected |= 1 << port;
            }
        }
    }
    //the ChangeConnected flag must be set here together with the CurrentConnected flag.
    DeviceType->ChangeConnected = DeviceType->CurrentConnected;

    // JSRF Hack: Don't set the ChangeConnected flag. Without this, JSRF hard crashes 
	// TODO: Why is this still needed? 
	if (DeviceType == gDeviceType_Gamepad && TitleRequiresInputHack()) {
		DeviceType->ChangeConnected = 0;
	}
    ret = DeviceType->CurrentConnected;

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


	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(DeviceType)
		LOG_FUNC_ARG(pdwInsertions)
		LOG_FUNC_ARG(pdwRemovals)
	LOG_FUNC_END;

	BOOL ret = FALSE;
    
    // If this device type was not previously detected, connect one (or more)
    // some titles call XGetDevices first, and the CurrentConnected and ChangeConnected flags are set there.
    // note that certain titles such as Otogi need the ChangeConnected to be set to 1 always to enable the input. 
    int port;
    if (DeviceType->CurrentConnected == 0) {
        for (port = 0; port < 4; port++) {
            //if the host controller is connected and the xbox DeviceType matches. set the CurrentConnected flag.
            if (g_XboxControllerHostBridge[port].XboxDeviceInfo.DeviceType == DeviceType && g_XboxControllerHostBridge[port].dwHostType>0) {
                DeviceType->CurrentConnected |= 1 << port;
            }
        }
        DeviceType->ChangeConnected = DeviceType->CurrentConnected;
    }

    // JSRF Hack: Don't set the ChangeConnected flag. Without this, JSRF hard crashes
	if (TitleRequiresInputHack()) {
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

    // Lego SW Hack: Require XGetDeviceChanges to return changes all the time, but no removal, only insertions.
    // Without this, Lego SW will not response to controller's input.
	if (TitleIsLegoSW()) {
		*pdwRemovals = 0;
		*pdwInsertions = DeviceType->CurrentConnected;
		ret = TRUE;
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
    IN PX_XINPUT_POLLING_PARAMETERS   pPollingParameters OPTIONAL
)
{


	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(DeviceType)
		LOG_FUNC_ARG(dwPort)
		LOG_FUNC_ARG(dwSlot)
		LOG_FUNC_ARG(pPollingParameters)
		LOG_FUNC_END;

    X_POLLING_PARAMETERS_HANDLE *pph = 0;
    //OLD_XINPUT
    //rever back to return handle  for port 0~3, this is for multi controller support.
/*    if(dwPort >= 0 && (dwPort <= total_xinput_gamepad))
    {
        if(g_hInputHandle[dwPort] == 0)
        {
            pph = (X_POLLING_PARAMETERS_HANDLE*) &g_pph[dwPort];	// new XB_POLLING_PARAMETERS_HANDLE();

            if(pPollingParameters != NULL)
            {
                pph->pPollingParameters = (X_XINPUT_POLLING_PARAMETERS*) &g_pp[dwPort]; // new XINPUT_POLLING_PARAMETERS();

                memcpy(pph->pPollingParameters, pPollingParameters, sizeof(X_XINPUT_POLLING_PARAMETERS));
            }
            else
            {
                pph->pPollingParameters = NULL;
            }

            g_hInputHandle[dwPort] = pph;
        }
        else
        {
            pph = (X_POLLING_PARAMETERS_HANDLE*)g_hInputHandle[dwPort];

            if(pPollingParameters != 0)
            {
                if(pph->pPollingParameters == 0)
                {
                    pph->pPollingParameters = (X_XINPUT_POLLING_PARAMETERS*) &g_pp[dwPort]; // new XINPUT_POLLING_PARAMETERS();
                }

                memcpy(pph->pPollingParameters, pPollingParameters, sizeof(X_XINPUT_POLLING_PARAMETERS));
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
*/
	g_bXInputOpenCalled = true;

	//RETURN((HANDLE)pph);
    //code above are not used at all, in future we might remove them.
    if (dwPort >= 0 && dwPort < 4) {
        //check if the bridged xbox controller at this port matches the DeviceType, if matches, setup the device handle and return it.
        if (g_XboxControllerHostBridge[dwPort].XboxDeviceInfo.DeviceType == DeviceType && g_XboxControllerHostBridge[dwPort].dwHostType!=0) {
            //create the dialog for virtual SteelBatallion controller feedback status.
            if(g_XboxControllerHostBridge[dwPort].dwHostType==X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_VIRTUAL_SBC){
                //if DLL not loaded yet, load it.
                if (g_bCxbxVSBCLoaded != true) {

                    g_module = LoadLibrary(TEXT("CxbxVSBC.dll"));
                    if (g_module != 0) {
                        g_bCxbxVSBCLoaded = true;
                    }
                }
                if(g_module!=0&& fnCxbxVSBCOpen==0){
                    fnCxbxVSBCSetState = (PFARPROC2)GetProcAddress(g_module, "VSBCSetState");
                    fnCxbxVSBCGetState = (PFARPROC2)GetProcAddress(g_module, "VSBCGetState");
                    fnCxbxVSBCOpen = (PFARPROC1)GetProcAddress(g_module, "VSBCOpen");
                }
                
                if (fnCxbxVSBCOpen == 0) {
                    printf("EmuXapi: EmuXInputOpen: GetPRocAddress VSBCOpen failed!\n");
                }
                else {
                    (*fnCxbxVSBCOpen)(X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_VIRTUAL_SBC);
                }
                //DWORD dwVXBCOpenResult = CxbxVSBC::MyCxbxVSBC::VSBCOpen(X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_VIRTUAL_SBC);

            }
            g_XboxControllerHostBridge[dwPort].hXboxDevice = &g_XboxControllerHostBridge[dwPort];
            return g_XboxControllerHostBridge[dwPort].hXboxDevice;
        }
        
    }
    
    return 0;
}

// ******************************************************************
// * patch: XInputClose
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(XInputClose)
(
    IN HANDLE hDevice
)
{


	LOG_FUNC_ONE_ARG(hDevice);

    X_POLLING_PARAMETERS_HANDLE *pph = (X_POLLING_PARAMETERS_HANDLE*)hDevice;
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
        //OLD_XINPUT
        /*
        if(pph->pPollingParameters != NULL)
        {
            delete pph->pPollingParameters;
        }
		
        delete pph;
		*/
    }

    //above code is not used at all, in future we might remove them.
    //reset hXboxDevice handle if it matches the hDevice
    int port;
    for(port=0;port<4;port++){
        if (g_XboxControllerHostBridge[dwPort].hXboxDevice == hDevice) {
            g_XboxControllerHostBridge[dwPort].hXboxDevice=0;
            break;
        }
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


	LOG_FUNC_ONE_ARG(hDevice);

    //OLD_XINPUT
/*    X_POLLING_PARAMETERS_HANDLE *pph = (X_POLLING_PARAMETERS_HANDLE*)hDevice;

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

            XTL::PX_XINPUT_FEEDBACK pFeedback = (XTL::PX_XINPUT_FEEDBACK)g_pXInputSetStateStatus[v].pFeedback;

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
*/
    int port;
    for (port = 0; port<4; port++) {
        if (g_XboxControllerHostBridge[port].hXboxDevice == hDevice) {
            if (g_XboxControllerHostBridge[port].pXboxFeedbackHeader != 0) {
                if (g_XboxControllerHostBridge[port].pXboxFeedbackHeader->dwStatus != ERROR_SUCCESS) {
                    if (g_XboxControllerHostBridge[port].pXboxFeedbackHeader->hEvent != 0)
                    {
                        SetEvent(g_XboxControllerHostBridge[port].pXboxFeedbackHeader->hEvent);
                    }
                    g_XboxControllerHostBridge[port].pXboxFeedbackHeader->dwStatus = ERROR_SUCCESS;
                    break;
                }
            }
            else {
                break;
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
    OUT PX_XINPUT_CAPABILITIES pCapabilities
)
{


	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hDevice)
		LOG_FUNC_ARG_OUT(pCapabilities)
		LOG_FUNC_END;

    DWORD ret = ERROR_DEVICE_NOT_CONNECTED;

    X_POLLING_PARAMETERS_HANDLE *pph = (X_POLLING_PARAMETERS_HANDLE*)hDevice;
    //OLD_XINPUT
/*    if(pph != NULL)
    {
        DWORD dwPort = pph->dwPort;
		//return gamepad capabilities for port 0~3.
        if(dwPort >= 0 && dwPort<=total_xinput_gamepad)
        {
            pCapabilities->SubType = X_XINPUT_DEVSUBTYPE_GC_GAMEPAD;
			pCapabilities->In.Gamepad = {};
			pCapabilities->Out.Rumble = {};

            ret = ERROR_SUCCESS;
        }
    }
*/
    //above code is not used any more, could be removed.
    //find XboxControllerHostBridge per hDevice, and fill the Capabilities Structure per Device Info
    int port;
    for (port = 0; port < 4; port++) {
        if (g_XboxControllerHostBridge[port].hXboxDevice == hDevice) {
            pCapabilities->SubType = g_XboxControllerHostBridge[port].XboxDeviceInfo.ucSubType;
            //ready to set the In and Out structure in pCapabilities, shall set all bit to 1 for enabling the capabilities.
            UCHAR * pCapa = (UCHAR *)(&pCapabilities->In);
            
            memset( pCapa,
                    0xFF,
                    g_XboxControllerHostBridge[port].XboxDeviceInfo.ucInputStateSize + g_XboxControllerHostBridge[port].XboxDeviceInfo.ucFeedbackSize);

			ret = ERROR_SUCCESS;
            break;
        }
    }
    
	RETURN(ret);
}

//variable names correlated to X_SBC_FEEDBACK, mapped to each nibble accordingly.
char * XboxSBCFeedbackNames[] = {
    "EmergencyEject",
    "CockpitHatch",
    "Ignition",
    "Start",
    "OpenClose",
    "MapZoomInOut",
    "ModeSelect",
    "SubMonitorModeSelect",
    "MainMonitorZoomIn",
    "MainMonitorZoomOut",
    "ForecastShootingSystem",
    "Manipulator",
    "LineColorChange",
    "Washing",
    "Extinguisher",
    "Chaff",
    "TankDetach",
    "Override",
    "NightScope",
    "F1",
    "F2",
    "F3",
    "MainWeaponControl",
    "SubWeaponControl",
    "MagazineChange",
    "Comm1",
    "Comm2",
    "Comm3",
    "Comm4",
    "Comm5",
    "Unknown",
    "GearR",
    "GearN",
    "Gear1",
    "Gear2",
    "Gear3",
    "Gear4",
    "Gear5"
};

//keep last SBC_GAMEPAD status, for DIP switch and GearLever
XTL::X_SBC_GAMEPAD XboxSBCGamepad = {};

//virtual SteelBatalion controller GetState, using port 0 from XInput and DirectInput to emulate virtual controller.
void EmuSBCGetState(XTL::PX_SBC_GAMEPAD pSBCGamepad, XTL::PX_XINPUT_GAMEPAD pXIGamepad, XTL::PX_XINPUT_GAMEPAD pDIGamepad)
{
    // Now convert those values to SteelBatalion Gamepad

    //restore certain variables such as GerLever and Toggle Switches.

    //restore the kept ucGearLever
    pSBCGamepad->ucGearLever = XboxSBCGamepad.ucGearLever;
    //we use continues range 7~13, 8 for gear N.
    if (pSBCGamepad->ucGearLever < 7 || pSBCGamepad->ucGearLever>13) {
        pSBCGamepad->ucGearLever = 8;
    }

    //restore Toggle Switches.
    pSBCGamepad->wButtons[0] |= (XboxSBCGamepad.wButtons[0] & (X_SBC_GAMEPAD_W0_COCKPITHATCH | X_SBC_GAMEPAD_W0_IGNITION));

    pSBCGamepad->wButtons[2] |= (XboxSBCGamepad.wButtons[2]&( X_SBC_GAMEPAD_W2_TOGGLEFILTERCONTROL
                                                            | X_SBC_GAMEPAD_W2_TOGGLEOXYGENSUPPLY
                                                            | X_SBC_GAMEPAD_W2_TOGGLEFUELFLOWRATE
                                                            | X_SBC_GAMEPAD_W2_TOGGLEBUFFREMATERIAL
                                                            | X_SBC_GAMEPAD_W2_TOGGLEVTLOCATION));


    // Analog Sticks
    pSBCGamepad->sAimingX = pXIGamepad->sThumbRX;
    pSBCGamepad->sAimingY = pXIGamepad->sThumbRY;
    pSBCGamepad->sRotationLever = 0;//(pXIGamepad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) ? 255 : 0;
    pSBCGamepad->sSightChangeX = pXIGamepad->sThumbLX;
    pSBCGamepad->sSightChangeY = pXIGamepad->sThumbLY;
    pSBCGamepad->wLeftPedal = ((SHORT)(pXIGamepad->bAnalogButtons[X_XINPUT_GAMEPAD_LEFT_TRIGGER]))<<8;
    pSBCGamepad->wMiddlePedal=0;// = (pXIGamepad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) ? 255 : 0;
    pSBCGamepad->wRightPedal = (SHORT)(pXIGamepad->bAnalogButtons[X_XINPUT_GAMEPAD_RIGHT_TRIGGER])<<8;
    pSBCGamepad->ucTunerDial=0;//low nibble

    
    // Digital Buttons
    if (pXIGamepad->bAnalogButtons [X_XINPUT_GAMEPAD_A]>0) {
        pSBCGamepad->wButtons[0] |= X_SBC_GAMEPAD_W0_RIGHTJOYMAINWEAPON;
    }
    else {
        pSBCGamepad->wButtons[0] &= ~X_SBC_GAMEPAD_W0_RIGHTJOYMAINWEAPON;
    }
    if (pXIGamepad->bAnalogButtons[X_XINPUT_GAMEPAD_B]>0) {
        pSBCGamepad->wButtons[0] |= X_SBC_GAMEPAD_W0_RIGHTJOYFIRE;
    }
    else {
        pSBCGamepad->wButtons[0] &= ~X_SBC_GAMEPAD_W0_RIGHTJOYFIRE;
    }
    if (pXIGamepad->bAnalogButtons[X_XINPUT_GAMEPAD_X]>0) {
        pSBCGamepad->wButtons[0] |= X_SBC_GAMEPAD_W0_RIGHTJOYLOCKON;
    }
    else {
        pSBCGamepad->wButtons[0] &= ~X_SBC_GAMEPAD_W0_RIGHTJOYLOCKON;
    }
    if (pXIGamepad->bAnalogButtons[X_XINPUT_GAMEPAD_Y]>0) {
        pSBCGamepad->wButtons[1] |= X_SBC_GAMEPAD_W1_WEAPONCONMAGAZINE;
    }
    else {
        pSBCGamepad->wButtons[1] &= ~X_SBC_GAMEPAD_W1_WEAPONCONMAGAZINE;
    }

    //GearLever 1~5 for gear 1~5, 7~13 for gear R,N,1~5, 15 for gear R. we use the continues range from 7~13
    if (pXIGamepad->bAnalogButtons[X_XINPUT_GAMEPAD_WHITE]>0) {//Left Shouder, Gear Down
        if (pSBCGamepad->ucGearLever >7) {
            pSBCGamepad->ucGearLever-- ;
        }
    }

    if (pXIGamepad->bAnalogButtons[X_XINPUT_GAMEPAD_BLACK]>0) {//Right Shouder, Gear Up
        if (pSBCGamepad->ucGearLever < 13) {
            pSBCGamepad->ucGearLever ++;
        }
    }
    //OLD_XINPUT
    /* //not used, don't duplicate the handling for same setting of pXIGamepad's members, later one will over write privous one.
    if (pXIGamepad->wButtons & X_XINPUT_GAMEPAD_START) {
        pSBCGamepad->wButtons[0] |= X_SBC_GAMEPAD_W0_START;
    }
    else {
        pSBCGamepad->wButtons[0] &= ~X_SBC_GAMEPAD_W0_START;
    }
    */
    if (pXIGamepad->wButtons & X_XINPUT_GAMEPAD_LEFT_THUMB) {//Center Sight Change
        pSBCGamepad->wButtons[2] |= X_SBC_GAMEPAD_W2_LEFTJOYSIGHTCHANGE;
    }
    else {
        pSBCGamepad->wButtons[2] &= ~X_SBC_GAMEPAD_W2_LEFTJOYSIGHTCHANGE;
    }
    //OLD_XINPUT
    /* //not used
    if (pXIGamepad->wButtons & X_XINPUT_GAMEPAD_RIGHT_THUMB) {
        pSBCGamepad->wButtons |= X_XINPUT_GAMEPAD_RIGHT_THUMB;
    }
    else {
        pSBCGamepad->wButtons &= ~X_XINPUT_GAMEPAD_RIGHT_THUMB;
    }
    */


    //additional input from 2nd input, default using directinput
    if (pDIGamepad->bAnalogButtons[X_XINPUT_GAMEPAD_A]>0) {
        pSBCGamepad->wButtons[0] |= X_SBC_GAMEPAD_W0_START;
    }
    else {
        pSBCGamepad->wButtons[0] &= ~X_SBC_GAMEPAD_W0_START;
    }
    // Iginition is Toggle Switch
    if (pDIGamepad->bAnalogButtons[X_XINPUT_GAMEPAD_B]>0) {
        if (pSBCGamepad->wButtons[0] & X_SBC_GAMEPAD_W0_IGNITION) {
            pSBCGamepad->wButtons[0] &= ~X_SBC_GAMEPAD_W0_IGNITION;
        }
        else {
            pSBCGamepad->wButtons[0] |= X_SBC_GAMEPAD_W0_IGNITION;
        }
    }

    if (pDIGamepad->bAnalogButtons[X_XINPUT_GAMEPAD_X]>0) {
        pSBCGamepad->wButtons[0] |= X_SBC_GAMEPAD_W0_EJECT;
    }
    else {
        pSBCGamepad->wButtons[0] &= ~X_SBC_GAMEPAD_W0_EJECT;
    }
    // CockpitHatch is Toggle Switch
    if (pDIGamepad->bAnalogButtons[X_XINPUT_GAMEPAD_Y]>0) {
        if (pSBCGamepad->wButtons[0] & X_SBC_GAMEPAD_W0_COCKPITHATCH) {
            pSBCGamepad->wButtons[0] &= ~X_SBC_GAMEPAD_W0_COCKPITHATCH;
        }
        else {
            pSBCGamepad->wButtons[0] |= X_SBC_GAMEPAD_W0_COCKPITHATCH;
        }
    }

    if (pDIGamepad->wButtons & X_XINPUT_GAMEPAD_BACK) {//Toggle Switch ToggleFilterControl
        if (pSBCGamepad->wButtons[2] & X_SBC_GAMEPAD_W2_TOGGLEFILTERCONTROL) {
            pSBCGamepad->wButtons[2] &= ~X_SBC_GAMEPAD_W2_TOGGLEFILTERCONTROL;
        }
        else {
            pSBCGamepad->wButtons[2] |= X_SBC_GAMEPAD_W2_TOGGLEFILTERCONTROL;
        }
    }

    if (pDIGamepad->wButtons & X_XINPUT_GAMEPAD_DPAD_UP) {//Toggle Switch ToggleOxygenSupply
        if (pSBCGamepad->wButtons[2] & X_SBC_GAMEPAD_W2_TOGGLEOXYGENSUPPLY) {
            pSBCGamepad->wButtons[2] &= ~X_SBC_GAMEPAD_W2_TOGGLEOXYGENSUPPLY;
        }
        else {
            pSBCGamepad->wButtons[2] |= X_SBC_GAMEPAD_W2_TOGGLEOXYGENSUPPLY;
        }
    }

    if (pDIGamepad->wButtons & X_XINPUT_GAMEPAD_DPAD_DOWN) {//Toggle Switch ToggleBuffreMaterial
        if (pSBCGamepad->wButtons[2] & X_SBC_GAMEPAD_W2_TOGGLEBUFFREMATERIAL) {
            pSBCGamepad->wButtons[2] &= ~X_SBC_GAMEPAD_W2_TOGGLEBUFFREMATERIAL;
        }
        else {
            pSBCGamepad->wButtons[2] |= X_SBC_GAMEPAD_W2_TOGGLEBUFFREMATERIAL;
        }
    }

    if (pDIGamepad->wButtons & X_XINPUT_GAMEPAD_DPAD_LEFT) {//Toggle Switch ToggleVTLocation
        if (pSBCGamepad->wButtons[2] & X_SBC_GAMEPAD_W2_TOGGLEVTLOCATION) {
            pSBCGamepad->wButtons[2] &= ~X_SBC_GAMEPAD_W2_TOGGLEVTLOCATION;
        }
        else {
            pSBCGamepad->wButtons[2] |= X_SBC_GAMEPAD_W2_TOGGLEVTLOCATION;
        }
    }

    if (pDIGamepad->wButtons & X_XINPUT_GAMEPAD_DPAD_RIGHT) {//Toggle Switch ToggleFuelFlowRate
        if (pSBCGamepad->wButtons[2] & X_SBC_GAMEPAD_W2_TOGGLEFUELFLOWRATE) {
            pSBCGamepad->wButtons[2] &= ~X_SBC_GAMEPAD_W2_TOGGLEFUELFLOWRATE;
        }
        else {
            pSBCGamepad->wButtons[2] |= X_SBC_GAMEPAD_W2_TOGGLEFUELFLOWRATE;
        }
    }
    //reserve the SBCGamepad to keep the status of GearLever and Toggole Switches.
    XboxSBCGamepad = *pSBCGamepad;
}
// ******************************************************************
// * patch: XInputGetState
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XInputGetState)
(
    IN  HANDLE         hDevice,
    OUT PX_XINPUT_STATE  pState
)
{


	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hDevice)
		LOG_FUNC_ARG_OUT(pState)
		LOG_FUNC_END;

    DWORD ret = ERROR_INVALID_HANDLE;
    //OLD_XINPUT
    /*
    X_POLLING_PARAMETERS_HANDLE *pph = (X_POLLING_PARAMETERS_HANDLE*)hDevice;

    if(pph != NULL)
    {
        if(pph->pPollingParameters != NULL)
        {
            if(pph->pPollingParameters->fAutoPoll == FALSE)
            {
                //
                // TODO: uh..
                //

                EmuLog(LOG_LEVEL::WARNING, "EmuXInputGetState : fAutoPoll == FALSE");
            }
        }

        DWORD dwPort = pph->dwPort;

        if((dwPort >= 0) && (dwPort <= total_xinput_gamepad))
        {
			DBG_PRINTF("EmuXInputGetState(): dwPort = %d\n", dwPort );

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
		EmuLog(LOG_LEVEL::WARNING, "EmuXInputGetState(): pph == NULL!");
    */
    //above code is not used at all, in future we might remove them.
    //get input state if hXboxDevice matches hDevice
    int port;
    for (port = 0; port<4; port++) {
        if (g_XboxControllerHostBridge[port].hXboxDevice == hDevice) {
            
            //for xinput, we query the state corresponds to port.
            switch (g_XboxControllerHostBridge[port].dwHostType) {
            case X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_XINPUT://using XInput
                EmuXInputPCPoll(g_XboxControllerHostBridge[port].dwHostPort, pState);
                ret = ERROR_SUCCESS;
                break;
            case X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_DINPUT://using directinput
                EmuDInputPoll(pState);
                ret = ERROR_SUCCESS;
                break;
            case X_XONTROLLER_HOST_BRIDGE_HOSTTYPE_VIRTUAL_SBC://using virtual SteelBatalion Controller
                //printf("SBC get state!\n");
                XTL::X_XINPUT_STATE  InputState0, InputState1;
                InputState0 = {};
                InputState1 = {};
                EmuXInputPCPoll(0, &InputState0);
                EmuDInputPoll(&InputState1);
                pState->dwPacketNumber = InputState0.dwPacketNumber;
                EmuSBCGetState(XTL::PX_SBC_GAMEPAD(&pState->Gamepad), &InputState0.Gamepad, &InputState1.Gamepad);
                break;
            default:
                break;
            }

            break;
        }
    }
    
	RETURN(ret);
}

// ******************************************************************
// * patch: XInputSetState
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XInputSetState)
(
    IN     HANDLE           hDevice,
    IN OUT PX_XINPUT_FEEDBACK pFeedback
)
{


	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hDevice)
		LOG_FUNC_ARG(pFeedback)
		LOG_FUNC_END;

    DWORD ret = ERROR_IO_PENDING;
    //OLD_XINPUT
/*
    X_POLLING_PARAMETERS_HANDLE *pph = (X_POLLING_PARAMETERS_HANDLE*)hDevice;

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
                    //If the device was succesfully added to polling before, recycle the request
                    g_pXInputSetStateStatus[v].pFeedback = pFeedback;
                    pFeedback->Header.dwStatus = ERROR_IO_PENDING;
                }
                else {
                    //Ignore this request as another one is already pending
                    ret = ERROR_SUCCESS;
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
    */

    //above code is not used at all, in future we might remove them.
    //reset hXboxDevice handle if it matches the hDevice
    int port;
    for (port = 0; port<4; port++) {
        if (g_XboxControllerHostBridge[port].hXboxDevice == hDevice) {
            //if (g_XboxControllerHostBridge[port].pXboxFeedbackHeader == 0) {
                g_XboxControllerHostBridge[port].pXboxFeedbackHeader = &pFeedback->Header;
                g_XboxControllerHostBridge[port].dwLatency = 0;
                pFeedback->Header.dwStatus = ERROR_IO_PENDING;
                ret = ERROR_IO_PENDING;
                //set XInput State if host type is Xinput.
                switch (g_XboxControllerHostBridge[port].dwHostType) {
                case 1://using XInput
                    XTL::EmuXInputSetState(port, pFeedback);
                    break;
                case 2://using directinput
                    break;
                case 0x80://using virtual SteelBatalion Controller
                    //printf("SBC setstate!\n");
                    //EmuXSBCSetState((UCHAR *)&pFeedback->Rumble);
                    //UpdateVirtualSBCFeedbackDlg((UCHAR *)&pFeedback->Rumble);
                    //DWORD dwVXBCSetStateResult;
                    if (g_module != 0 && fnCxbxVSBCSetState == 0) {
                        fnCxbxVSBCSetState = (PFARPROC2)GetProcAddress(g_module, "VSBCSetState");
                        //fnCxbxVSBCGetState = (PFARPROC2)GetProcAddress(g_module, "VSBCGetState");
                        //fnCxbxVSBCOpen = (PFARPROC1)GetProcAddress(g_module, "VSBCOpen");
                    }
                    if (fnCxbxVSBCSetState == 0) {
                        printf("EmuXapi: EmuXInputSetState: GetPRocAddress VSBCSetState failed!\n");
                    }
                    else {
                        (*fnCxbxVSBCSetState)((UCHAR *)&pFeedback->Rumble);
                    }

                    
                    //dwVXBCSetStateResult = CxbxVSBC::MyCxbxVSBC::VSBCSetState((UCHAR *)&pFeedback->Rumble);
                    break;
                default:
                    break;
                }
                break;
            //}
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


	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hThread)
		LOG_FUNC_ARG(DisablePriorityBoost)
		LOG_FUNC_END;

    BOOL bRet = SetThreadPriorityBoost(hThread, DisablePriorityBoost);

    if(bRet == FALSE)
        EmuLog(LOG_LEVEL::WARNING, "SetThreadPriorityBoost Failed!");

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


	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hThread)
		LOG_FUNC_ARG(nPriority)
		LOG_FUNC_END;

    BOOL bRet = SetThreadPriority(hThread, nPriority);

    if(bRet == FALSE)
        EmuLog(LOG_LEVEL::WARNING, "SetThreadPriority Failed!");

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


	LOG_FUNC_ONE_ARG(hThread);

    int iRet = GetThreadPriority(hThread);

    if(iRet == THREAD_PRIORITY_ERROR_RETURN)
        EmuLog(LOG_LEVEL::WARNING, "GetThreadPriority Failed!");

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
		EmuLog(LOG_LEVEL::WARNING, "Problem with ExceptionFilter");
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

	LOG_FUNC_ONE_ARG(lpParameter);
		
	LPVOID pRet = ConvertThreadToFiber(lpParameter);
	
	RETURN(pRet);
}

// ******************************************************************
// * patch: QueryPerformanceCounter
// ******************************************************************
ULONGLONG CxbxGetPerformanceCounter(bool acpi); // implemented in EmuKrnlKe.cpp
BOOL WINAPI XTL::EMUPATCH(QueryPerformanceCounter)
(
	LARGE_INTEGER * lpPerformanceCount
)
{
	// NOTE: QueryPerformanceCounter runs from the tsc via RdTsc (733mhz)
	// However, KeQueryPerformanceCounter runs at 3.375Mhz, so we can't use that here
	lpPerformanceCount->QuadPart = CxbxGetPerformanceCounter(false);
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
		EmuLog(LOG_LEVEL::WARNING, "DuplicateHandle failed!");

	dwRet = QueueUserAPC(pfnAPC, hApcThread, dwData);
	if(!dwRet)
		EmuLog(LOG_LEVEL::WARNING, "QueueUserAPC failed!");

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
				(void)CxbxMessageBox("The title is rebooting to dashboard", MB_OK, CxbxKrnl_hEmuParent);
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
// * patch: XGetDeviceEnumerationStatus
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(XGetDeviceEnumerationStatus)()
{


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

	LOG_FUNC_ONE_ARG(lpOutputString);
	printf("OutputDebugStringA: %s\n", lpOutputString);
}
