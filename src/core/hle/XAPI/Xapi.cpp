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

#define LOG_PREFIX CXBXR_MODULE::XAPI



#include <core\kernel\exports\xboxkrnl.h>
#include "common\input\SdlJoystick.h"
#include "common\input\InputManager.h"
#include <Shlwapi.h>
#include "core\kernel\init\CxbxKrnl.h"
#include "Logging.h"
#include "core\kernel\support\Emu.h"
#include "core\kernel\exports\EmuKrnl.h" // For DefaultLaunchDataPage
#include "core\kernel\support\EmuFile.h"
#include "EmuShared.h"
#include "core\hle\Intercept.hpp"
#include "Windef.h"
#include <vector>
#include "core\hle\XAPI\Xapi.h"


xbox::PXPP_DEVICE_TYPE g_DeviceType_Gamepad = nullptr;
xbox::PXPP_DEVICE_TYPE g_DeviceType_SBC = nullptr;

// Flag is unset after initialize devices is done by simulate LLE USB thread.
std::atomic<bool> g_bIsDevicesInitializing = true;
std::atomic<bool> g_bIsDevicesEmulating = false;
static CXBX_XINPUT_IN_STATE InState[4];

// Global bridge for xbox controller to host, 4 elements for 4 ports.
CXBX_CONTROLLER_HOST_BRIDGE g_XboxControllerHostBridge[4] = {
	{ NULL, PORT_INVALID, XBOX_INPUT_DEVICE::DEVICE_INVALID, &InState[0], false, false, false, { 0, 0, 0, 0, 0 } },
	{ NULL, PORT_INVALID, XBOX_INPUT_DEVICE::DEVICE_INVALID, &InState[1], false, false, false, { 0, 0, 0, 0, 0 } },
	{ NULL, PORT_INVALID, XBOX_INPUT_DEVICE::DEVICE_INVALID, &InState[2], false, false, false, { 0, 0, 0, 0, 0 } },
	{ NULL, PORT_INVALID, XBOX_INPUT_DEVICE::DEVICE_INVALID, &InState[3], false, false, false, { 0, 0, 0, 0, 0 } },
};


bool operator==(xbox::PXPP_DEVICE_TYPE XppType, XBOX_INPUT_DEVICE XidType)
{
	switch (XidType)
	{
	case XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE:
	case XBOX_INPUT_DEVICE::MS_CONTROLLER_S: {
		if (XppType == g_DeviceType_Gamepad) {
			return true;
		}
	}
	break;

	case XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER: {
		if (XppType == g_DeviceType_SBC) {
			return true;
		}
	}
	break;

	case XBOX_INPUT_DEVICE::LIGHT_GUN:
	case XBOX_INPUT_DEVICE::STEERING_WHEEL:
	case XBOX_INPUT_DEVICE::MEMORY_UNIT:
	case XBOX_INPUT_DEVICE::IR_DONGLE:
	default:
		break;
	}

	return false;
}

bool operator!=(xbox::PXPP_DEVICE_TYPE XppType, XBOX_INPUT_DEVICE XidType)
{
	return !(XppType == XidType);
}

bool ConstructHleInputDevice(int Type, int Port)
{
	g_bIsDevicesEmulating = true;
	bool ret;

	switch (Type)
	{
	case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE): {
		g_XboxControllerHostBridge[Port].XboxPort = Port;
		g_XboxControllerHostBridge[Port].XboxType = XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE;
		g_XboxControllerHostBridge[Port].bPendingRemoval = false;
		g_XboxControllerHostBridge[Port].bSignaled = false;
		g_XboxControllerHostBridge[Port].bIoInProgress = false;
		g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucType = XINPUT_DEVTYPE_GAMEPAD;
		g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucSubType = XINPUT_DEVSUBTYPE_GC_GAMEPAD;
		g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucInputStateSize = sizeof(XpadInput);
		g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucFeedbackSize = sizeof(XpadOutput);
		g_XboxControllerHostBridge[Port].XboxDeviceInfo.dwPacketNumber = 0;
		ret = true;
	}
	break;

	case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_S): {
		g_XboxControllerHostBridge[Port].XboxPort = Port;
		g_XboxControllerHostBridge[Port].XboxType = XBOX_INPUT_DEVICE::MS_CONTROLLER_S;
		g_XboxControllerHostBridge[Port].bPendingRemoval = false;
		g_XboxControllerHostBridge[Port].bSignaled = false;
		g_XboxControllerHostBridge[Port].bIoInProgress = false;
		g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucType = XINPUT_DEVTYPE_GAMEPAD;
		g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucSubType = XINPUT_DEVSUBTYPE_GC_GAMEPAD_ALT;
		g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucInputStateSize = sizeof(XpadInput);
		g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucFeedbackSize = sizeof(XpadOutput);
		g_XboxControllerHostBridge[Port].XboxDeviceInfo.dwPacketNumber = 0;
		ret = true;
    }
	break;

	case to_underlying(XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER): {
		g_XboxControllerHostBridge[Port].XboxPort = Port;
		g_XboxControllerHostBridge[Port].XboxType = XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER;
		g_XboxControllerHostBridge[Port].InState->SBC.ucGearLever = 8;
		g_XboxControllerHostBridge[Port].InState->SBC.sAimingX = static_cast<uint8_t>(0x7F);
		g_XboxControllerHostBridge[Port].InState->SBC.sAimingY = static_cast<uint8_t>(0x7F);
		g_XboxControllerHostBridge[Port].bPendingRemoval = false;
		g_XboxControllerHostBridge[Port].bSignaled = false;
		g_XboxControllerHostBridge[Port].bIoInProgress = false;
		g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucType = XINPUT_DEVTYPE_STEELBATTALION;
		g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucSubType = XINPUT_DEVSUBTYPE_GC_GAMEPAD_ALT;
		g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucInputStateSize = sizeof(SBCInput);
		g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucFeedbackSize = sizeof(SBCOutput);
		g_XboxControllerHostBridge[Port].XboxDeviceInfo.dwPacketNumber = 0;
		ret = true;
	}
	break;

	case to_underlying(XBOX_INPUT_DEVICE::LIGHT_GUN):
	case to_underlying(XBOX_INPUT_DEVICE::STEERING_WHEEL):
	case to_underlying(XBOX_INPUT_DEVICE::MEMORY_UNIT):
	case to_underlying(XBOX_INPUT_DEVICE::IR_DONGLE):
		EmuLog(LOG_LEVEL::INFO, "%s: device %s is not yet supported", __func__, GetInputDeviceName(Type).c_str());
		ret = false;
		break;

	default:
		EmuLog(LOG_LEVEL::WARNING, "Attempted to attach an unknown device type (type was %d)", Type);
		ret = false;
	}

	g_bIsDevicesEmulating = false;
	return ret;
}

void DestructHleInputDevice(int Port)
{
	g_bIsDevicesEmulating = true;

	g_XboxControllerHostBridge[Port].XboxType = XBOX_INPUT_DEVICE::DEVICE_INVALID;
	g_XboxControllerHostBridge[Port].XboxPort = PORT_INVALID;
	while (g_XboxControllerHostBridge[Port].bIoInProgress) {}
	g_XboxControllerHostBridge[Port].bPendingRemoval = false;
	g_XboxControllerHostBridge[Port].bSignaled = false;
	g_XboxControllerHostBridge[Port].bIoInProgress = false;
	g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucType = 0;
	g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucSubType = 0;
	g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucInputStateSize = 0;
	g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucFeedbackSize = 0;
	g_XboxControllerHostBridge[Port].XboxDeviceInfo.dwPacketNumber = 0;
	std::memset(&InState[Port], 0, sizeof(CXBX_XINPUT_IN_STATE));

	g_bIsDevicesEmulating = false;
}

void SetupXboxDeviceTypes()
{
	// If we don't yet have the offset to gDeviceType_Gamepad, work it out!
	if (g_DeviceType_Gamepad == nullptr) {
		// First, attempt to find GetTypeInformation
		auto typeInformation = g_SymbolAddresses.find("GetTypeInformation");
		if (typeInformation != g_SymbolAddresses.end() && typeInformation->second != xbox::zero) {
			EmuLog(LOG_LEVEL::INFO, "Deriving XDEVICE_TYPE_GAMEPAD from DeviceTable (via GetTypeInformation)");
			// Read the offset values of the device table structure from GetTypeInformation
			xbox::addr_xt deviceTableStartOffset = *(uint32_t*)((uint32_t)typeInformation->second + 0x01);
			xbox::addr_xt deviceTableEndOffset = *(uint32_t*)((uint32_t)typeInformation->second + 0x09);

			// Calculate the number of device entires in the table
			size_t deviceTableEntryCount = (deviceTableEndOffset - deviceTableStartOffset) / sizeof(uint32_t);

			EmuLog(LOG_LEVEL::INFO, "DeviceTableStart: 0x%08X", deviceTableStartOffset);
			EmuLog(LOG_LEVEL::INFO, "DeviceTableEnd: 0x%08X", deviceTableEndOffset);
			EmuLog(LOG_LEVEL::INFO, "DeviceTable Entires: %u", deviceTableEntryCount);

			// Sanity check: Where all these device offsets within Xbox memory
			if ((deviceTableStartOffset >= g_SystemMaxMemory) || (deviceTableEndOffset >= g_SystemMaxMemory)) {
				CxbxKrnlCleanup("DeviceTable Location is outside of Xbox Memory range");
			}

			// Iterate through the table until we find gamepad
			xbox::PXID_TYPE_INFORMATION* deviceTable = (xbox::PXID_TYPE_INFORMATION*)(deviceTableStartOffset);
			for (unsigned int i = 0; i < deviceTableEntryCount; i++) {
				// Skip empty table entries
				if (deviceTable[i] == nullptr) {
					continue;
				}

				EmuLog(LOG_LEVEL::INFO, "----------------------------------------");
				EmuLog(LOG_LEVEL::INFO, "DeviceTable[%u]->ucType = %d", i, deviceTable[i]->ucType);

				switch (deviceTable[i]->ucType) {
				case XINPUT_DEVTYPE_GAMEPAD:
					g_DeviceType_Gamepad = deviceTable[i]->XppType;
					EmuLog(LOG_LEVEL::INFO, "DeviceTable[%u]->XppType = 0x%08X (XDEVICE_TYPE_GAMEPAD)", i, (uintptr_t)g_DeviceType_Gamepad);
					break;

				case XINPUT_DEVTYPE_STEELBATTALION:
					g_DeviceType_SBC = deviceTable[i]->XppType;
					EmuLog(LOG_LEVEL::INFO, "DeviceTable[%u]->XppType = 0x%08X (XDEVICE_TYPE_STEELBATTALION)", i, (uintptr_t)g_DeviceType_SBC);
					break;

				default:
					EmuLog(LOG_LEVEL::WARNING, "DeviceTable[%u]->XppType = 0x%08X (Unknown device type)", i, (uintptr_t)deviceTable[i]->XppType);
					continue;
				}
			}
		} else {
			// XDKs without GetTypeInformation have the GamePad address hardcoded in XInputOpen
			// Only the earliest XDKs use this code path, and the offset never changed between them
			// so this works well for us.
			void* XInputOpenAddr = (void*)g_SymbolAddresses["XInputOpen"];
			if (XInputOpenAddr != nullptr) {
				EmuLog(LOG_LEVEL::INFO, "Deriving XDEVICE_TYPE_GAMEPAD from XInputOpen (0x%08X)", (uintptr_t)XInputOpenAddr);
				g_DeviceType_Gamepad = *(xbox::PXPP_DEVICE_TYPE*)((uint32_t)XInputOpenAddr + 0x0B);
			}
		}

		if (g_DeviceType_Gamepad == nullptr) {
			EmuLog(LOG_LEVEL::WARNING, "XDEVICE_TYPE_GAMEPAD was not found");
			return;
		}

		EmuLog(LOG_LEVEL::INFO, "XDEVICE_TYPE_GAMEPAD Found at 0x%08X", (uintptr_t)g_DeviceType_Gamepad);
	}
}

// ******************************************************************
// * patch: XInitDevices
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(XInitDevices)
(
    dword_xt					dwPreallocTypeCount,
	PXDEVICE_PREALLOC_TYPE	PreallocTypes
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(dwPreallocTypeCount)
		LOG_FUNC_ARG((DWORD)PreallocTypes)
		LOG_FUNC_END;

	// TODO: May need to apply this delay update to LLE USB if modern hardware initialization is faster than og xbox hardware.
	// Start initialize delay timer base on og xbox hardware's external thread process to finish initialize devices.
	// Test cases: Lego Star Wars - Without delay timer will cause not to call the input polling even after devices are open.
	//             Panzer Dragoon ORTA - After called and within 500 milliseconds will cause unstable crash for the emulator.
	std::thread([]() {
		// Set initialize state.
		g_bIsDevicesInitializing = true;
		// Initialize devices take about less than half a second usually.
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		// Clear initialize state for allow application to see device changes.
		g_bIsDevicesInitializing = false;
	}).detach();
}

// This is called to emulate async for both XGetDevices and XGetDeviceChanges
void UpdateConnectedDeviceState(xbox::PXPP_DEVICE_TYPE DeviceType) {

	// Do not process the queries until initialize delay and device emulating are complete.
	if (g_bIsDevicesInitializing || g_bIsDevicesEmulating){
		return;
	}

	int Port, PortMask;
	for (Port = PORT_1, PortMask = 1; Port <= PORT_4; Port++, PortMask <<= 1) {
		if (DeviceType == g_XboxControllerHostBridge[Port].XboxType && !g_XboxControllerHostBridge[Port].bPendingRemoval) {
			DeviceType->CurrentConnected |= PortMask;
		}
		else {
			DeviceType->CurrentConnected &= ~PortMask;
		}

		if (static_cast<uint8_t>(g_XboxControllerHostBridge[Port].bPendingRemoval) &
			~(static_cast<uint8_t>(g_XboxControllerHostBridge[Port].bSignaled))) {
			g_XboxControllerHostBridge[Port].bSignaled = true;
			SDL_Event DeviceRemoveEvent;
			SDL_memset(&DeviceRemoveEvent, 0, sizeof(SDL_Event));
			DeviceRemoveEvent.type = Sdl::DeviceRemoveAck_t;
			DeviceRemoveEvent.user.data1 = new int(Port);
			SDL_PushEvent(&DeviceRemoveEvent);
		}
	}
	DeviceType->ChangeConnected = DeviceType->PreviousConnected ^ DeviceType->CurrentConnected;
}

// ******************************************************************
// * patch: XGetDevices
// * Note: This could be unpatched however,
// * XInitDevices is required to be unpatched first.
// * This in turn requires USB LLE to be implemented, or USBD_Init 
// * patched with a stub, so this patch is still enabled for now
// ******************************************************************
xbox::dword_xt WINAPI xbox::EMUPATCH(XGetDevices)
(
    PXPP_DEVICE_TYPE DeviceType
)
{
	LOG_FUNC_ONE_ARG(DeviceType);

	UpdateConnectedDeviceState(DeviceType);

	UCHAR oldIrql = xbox::KeRaiseIrqlToDpcLevel();

	dword_xt ret = DeviceType->CurrentConnected;

	DeviceType->ChangeConnected = 0;
	DeviceType->PreviousConnected = DeviceType->CurrentConnected;

	xbox::KfLowerIrql(oldIrql);

	RETURN(ret);
}

// ******************************************************************
// * patch: XGetDeviceChanges
// * Note: This could be unpatched however,
// * XInitDevices is required to be unpatched first.
// * This in turn requires USB LLE to be implemented, or USBD_Init 
// * patched with a stub, so this patch is still enabled for now
// ******************************************************************
xbox::bool_xt WINAPI xbox::EMUPATCH(XGetDeviceChanges)
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

	UpdateConnectedDeviceState(DeviceType);

	BOOL ret = FALSE;


    if(!DeviceType->ChangeConnected)
    {
        *pdwInsertions = 0;
        *pdwRemovals = 0;
    }
    else
    {
        UCHAR oldIrql = xbox::KeRaiseIrqlToDpcLevel();

		// Insertions and removals
        *pdwInsertions = (DeviceType->CurrentConnected & ~DeviceType->PreviousConnected);
        *pdwRemovals = (DeviceType->PreviousConnected & ~DeviceType->CurrentConnected);

		// Detect devices that were removed and then immediately inserted again
        ULONG RemoveInsert = DeviceType->ChangeConnected &
            DeviceType->CurrentConnected &
            DeviceType->PreviousConnected;
        *pdwRemovals |= RemoveInsert;
        *pdwInsertions |= RemoveInsert;

        DeviceType->ChangeConnected = 0;
        DeviceType->PreviousConnected = DeviceType->CurrentConnected;
        ret = (*pdwInsertions | *pdwRemovals) ? TRUE : FALSE;

		xbox::KfLowerIrql(oldIrql);
    }

	RETURN(ret);
}

// ******************************************************************
// * patch: XInputOpen
// ******************************************************************
xbox::HANDLE WINAPI xbox::EMUPATCH(XInputOpen)
(
    IN PXPP_DEVICE_TYPE             DeviceType,
    IN dword_xt                        dwPort,
    IN dword_xt                        dwSlot,
    IN PXINPUT_POLLING_PARAMETERS   pPollingParameters OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(DeviceType)
		LOG_FUNC_ARG(dwPort)
		LOG_FUNC_ARG(dwSlot)
		LOG_FUNC_ARG(pPollingParameters)
		LOG_FUNC_END;

    if (dwPort >= PORT_1 && dwPort <= PORT_4) {
        if (DeviceType == g_XboxControllerHostBridge[dwPort].XboxType) {
			g_XboxControllerHostBridge[dwPort].hXboxDevice = &g_XboxControllerHostBridge[dwPort];
			RETURN(g_XboxControllerHostBridge[dwPort].hXboxDevice);
        }
        
    }
    
	RETURN(NULL);
}

// ******************************************************************
// * patch: XInputClose
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(XInputClose)
(
    IN HANDLE hDevice
)
{
	LOG_FUNC_ONE_ARG(hDevice);

	PCXBX_CONTROLLER_HOST_BRIDGE Device = (PCXBX_CONTROLLER_HOST_BRIDGE)hDevice;
	int Port = Device->XboxPort;
	if (g_XboxControllerHostBridge[Port].hXboxDevice == hDevice) {
		Device->hXboxDevice = NULL;
	}
}

// ******************************************************************
// * patch: XInputPoll
// ******************************************************************
xbox::dword_xt WINAPI xbox::EMUPATCH(XInputPoll)
(
    IN HANDLE hDevice
)
{
	LOG_FUNC_ONE_ARG(hDevice);

	// Nothing to do

	RETURN(ERROR_SUCCESS);
}

// ******************************************************************
// * patch: XInputGetCapabilities
// ******************************************************************
xbox::dword_xt WINAPI xbox::EMUPATCH(XInputGetCapabilities)
(
    IN  HANDLE               hDevice,
    OUT PXINPUT_CAPABILITIES pCapabilities
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hDevice)
		LOG_FUNC_ARG_OUT(pCapabilities)
		LOG_FUNC_END;

    dword_xt ret = ERROR_DEVICE_NOT_CONNECTED;
	PCXBX_CONTROLLER_HOST_BRIDGE Device = (PCXBX_CONTROLLER_HOST_BRIDGE)hDevice;
	int Port = Device->XboxPort;
    if (g_XboxControllerHostBridge[Port].hXboxDevice == hDevice && !g_XboxControllerHostBridge[Port].bPendingRemoval) {
        pCapabilities->SubType = g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucSubType;
        UCHAR* pCap = (UCHAR*)(&pCapabilities->In);
        memset(pCap, 0xFF, g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucInputStateSize + g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucFeedbackSize);
		ret = ERROR_SUCCESS;
    }
    
	RETURN(ret);
}

// ******************************************************************
// * patch: XInputGetState
// ******************************************************************
xbox::dword_xt WINAPI xbox::EMUPATCH(XInputGetState)
(
    IN  HANDLE         hDevice,
    OUT PXINPUT_STATE  pState
)
{


	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hDevice)
		LOG_FUNC_ARG_OUT(pState)
		LOG_FUNC_END;

    dword_xt ret = ERROR_DEVICE_NOT_CONNECTED;
    PCXBX_CONTROLLER_HOST_BRIDGE Device = (PCXBX_CONTROLLER_HOST_BRIDGE)hDevice;
    int Port = Device->XboxPort;
    if (g_XboxControllerHostBridge[Port].hXboxDevice == hDevice) {
        if (!g_XboxControllerHostBridge[Port].bPendingRemoval) {
            g_XboxControllerHostBridge[Port].bIoInProgress = true;
            if (g_InputDeviceManager.UpdateXboxPortInput(Port, g_XboxControllerHostBridge[Port].InState, DIRECTION_IN, to_underlying(g_XboxControllerHostBridge[Port].XboxType))) {
                pState->dwPacketNumber = g_XboxControllerHostBridge[Port].XboxDeviceInfo.dwPacketNumber++;
            }
            memcpy((void*)&pState->Gamepad, g_XboxControllerHostBridge[Port].InState, g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucInputStateSize);
            g_XboxControllerHostBridge[Port].bIoInProgress = false;
            ret = ERROR_SUCCESS;
        }
    }
    
	RETURN(ret);
}

// ******************************************************************
// * patch: XInputSetState
// ******************************************************************
xbox::dword_xt WINAPI xbox::EMUPATCH(XInputSetState)
(
    IN     HANDLE           hDevice,
    IN OUT PXINPUT_FEEDBACK pFeedback
)
{


	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hDevice)
		LOG_FUNC_ARG(pFeedback)
		LOG_FUNC_END;

    PCXBX_CONTROLLER_HOST_BRIDGE Device = (PCXBX_CONTROLLER_HOST_BRIDGE)hDevice;
    int Port = Device->XboxPort;
    if (g_XboxControllerHostBridge[Port].hXboxDevice == hDevice && !g_XboxControllerHostBridge[Port].bPendingRemoval) {
        pFeedback->Header.dwStatus = ERROR_IO_PENDING;
        g_InputDeviceManager.UpdateXboxPortInput(Port, (void*)&pFeedback->Rumble, DIRECTION_OUT, to_underlying(g_XboxControllerHostBridge[Port].XboxType));
        pFeedback->Header.dwStatus = ERROR_SUCCESS;
        if (pFeedback->Header.hEvent != NULL &&
            ObReferenceObjectByHandle(pFeedback->Header.hEvent, &xbox::ExEventObjectType, (PVOID*)&pFeedback->Header.IoCompletedEvent) == ERROR_SUCCESS) {
            KeSetEvent((xbox::PKEVENT)pFeedback->Header.IoCompletedEvent, NULL, FALSE);
        }
    }
    else {
        pFeedback->Header.dwStatus = ERROR_DEVICE_NOT_CONNECTED;
    }

	RETURN(pFeedback->Header.dwStatus);
}


// ******************************************************************
// * patch: SetThreadPriorityBoost
// ******************************************************************
xbox::bool_xt WINAPI xbox::EMUPATCH(SetThreadPriorityBoost)
(
    HANDLE  hThread,
    bool_xt    DisablePriorityBoost
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
xbox::bool_xt WINAPI xbox::EMUPATCH(SetThreadPriority)
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
int WINAPI xbox::EMUPATCH(GetThreadPriority)
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
xbox::bool_xt WINAPI xbox::EMUPATCH(GetExitCodeThread)
(
    HANDLE  hThread,
    LPDWORD lpExitCode
)
{


	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hThread)
		LOG_FUNC_ARG(lpExitCode)
		LOG_FUNC_END;

    BOOL bRet = GetExitCodeThread(hThread, (::LPDWORD)lpExitCode);

	RETURN(bRet);
}

// ******************************************************************
// * patch: XapiThreadStartup
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(XapiThreadStartup)
(
    dword_xt dwDummy1,
    dword_xt dwDummy2
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(dwDummy1)
		LOG_FUNC_ARG(dwDummy2)
		LOG_FUNC_END;

	typedef int (__stdcall *pfDummyFunc)(dword_xt dwDummy);

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
xbox::void_xt WINAPI xbox::EMUPATCH(XRegisterThreadNotifyRoutine)
(
    PXTHREAD_NOTIFICATION   pThreadNotification,
    bool_xt                    fRegister
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
				g_pfnThreadNotification[i] = (PVOID)pThreadNotification->pfnNotifyRoutine;				
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
	LPFIBER_START_ROUTINE pfStartRoutine = (LPFIBER_START_ROUTINE)context->lpStartRoutine;
	pfStartRoutine(context->lpParameter);
}

// ******************************************************************
// * patch: CreateFiber
// ******************************************************************
xbox::LPVOID WINAPI xbox::EMUPATCH(CreateFiber)
(
	dword_xt					dwStackSize,
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
xbox::void_xt WINAPI xbox::EMUPATCH(DeleteFiber)
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
xbox::void_xt WINAPI xbox::EMUPATCH(SwitchToFiber)
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
xbox::LPVOID WINAPI xbox::EMUPATCH(ConvertThreadToFiber)
(
	LPVOID lpParameter
)
{

	LOG_FUNC_ONE_ARG(lpParameter);
		
	LPVOID pRet = ConvertThreadToFiber(lpParameter);
	
	RETURN(pRet);
}

// ******************************************************************
// * patch: QueueUserAPC
// ******************************************************************
xbox::dword_xt WINAPI xbox::EMUPATCH(QueueUserAPC)
(
	PAPCFUNC	pfnAPC,
	HANDLE		hThread,
	dword_xt		dwData
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_TYPE(PVOID, pfnAPC)
		LOG_FUNC_ARG(hThread)
		LOG_FUNC_ARG(dwData)
		LOG_FUNC_END;

	dword_xt dwRet = 0;

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
xbox::bool_xt WINAPI xbox::EMUPATCH(GetOverlappedResult)
(
	HANDLE			hFile,
	LPOVERLAPPED	lpOverlapped,
	LPDWORD			lpNumberOfBytesTransferred,
	bool_xt			bWait
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
xbox::dword_xt WINAPI xbox::EMUPATCH(XLaunchNewImageA)
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
		if (xbox::LaunchDataPage == xbox::zeroptr)
		{
			PVOID LaunchDataVAddr = xbox::MmAllocateContiguousMemory(sizeof(xbox::LAUNCH_DATA_PAGE));
			if (!LaunchDataVAddr)
			{
				RETURN(xbox::status_no_memory);
			}
			xbox::LaunchDataPage = (xbox::LAUNCH_DATA_PAGE*)LaunchDataVAddr;
		}

		xbox::LaunchDataPage->Header.dwTitleId = g_pCertificate->dwTitleId;
		xbox::LaunchDataPage->Header.dwFlags = 0; // TODO : What to put in here?
		xbox::LaunchDataPage->Header.dwLaunchDataType = LDT_TITLE;

		xbox::MmPersistContiguousMemory((PVOID)xbox::LaunchDataPage, PAGE_SIZE, TRUE);

		if (pLaunchData != xbox::zeroptr)
			// Save the launch data
			memcpy(&(xbox::LaunchDataPage->LaunchData[0]), pLaunchData, sizeof(LAUNCH_DATA));

		if (lpTitlePath == xbox::zeroptr)
		{
			// If no path is specified, then the xbe is rebooting to dashboard
			char szDashboardPath[MAX_PATH] = { 0 };
			XboxDevice* rootDevice = CxbxDeviceByDevicePath(DeviceHarddisk0Partition2);
			if (rootDevice != nullptr)
				sprintf(szDashboardPath, "%s\\xboxdash.xbe", rootDevice->HostDevicePath.c_str());

			if (PathFileExists(szDashboardPath))
			{
				PopupInfo(nullptr, "The title is rebooting to dashboard");
				lpTitlePath = "C:\\xboxdash.xbe";
				xbox::LaunchDataPage->Header.dwLaunchDataType = LDT_FROM_DASHBOARD;
				// Other options include LDT_NONE, LDT_FROM_DEBUGGER_CMDLINE and LDT_FROM_UPDATE
			}
			else
				CxbxKrnlCleanup("The xbe rebooted to Dashboard and xboxdash.xbe could not be found");
		}

		strncpy(&(xbox::LaunchDataPage->Header.szLaunchPath[0]), lpTitlePath, 520);
	}

	// Note : While this patch exists, HalReturnToFirmware() calls
	// MmPersistContiguousMemory on LaunchDataPage. When this
	// patch on XLaunchNewImageA is removed, remove the call to
	// MmPersistContiguousMemory from HalReturnToFirmware() too!!

	xbox::HalReturnToFirmware(xbox::ReturnFirmwareQuickReboot);

	// If this function succeeds, it doesn't get a chance to return anything.
	RETURN(ERROR_GEN_FAILURE);
}

#if 0 // patch disabled
// ******************************************************************
// * patch: XGetLaunchInfo
// ******************************************************************
xbox::dword_xt WINAPI xbox::EMUPATCH(XGetLaunchInfo)
(
	PDWORD			pdwLaunchDataType,
	PLAUNCH_DATA	pLaunchData
)
{


	// TODO : This patch can be removed once we're sure all XAPI library
	// functions indirectly reference our xbox::LaunchDataPage variable.
	// For this, we need a test-case that hits this function, and run that
	// with and without this patch enabled. Behavior should be identical.
	// When this is verified, this patch can be removed.
	LOG_TEST_CASE("Unpatching test needed");

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pdwLaunchDataType)
		LOG_FUNC_ARG(pLaunchData)
		LOG_FUNC_END;

	dword_xt ret = ERROR_NOT_FOUND;

	if (xbox::LaunchDataPage != NULL)
	{
		// Note : Here, CxbxRestoreLaunchDataPage() was already called,
		// which has loaded LaunchDataPage from a binary file (if present).

		// A title can pass data only to itself, not another title (unless started from the dashboard, of course) :
		if (   (xbox::LaunchDataPage->Header.dwTitleId == g_pCertificate->dwTitleId)
			|| (xbox::LaunchDataPage->Header.dwLaunchDataType == LDT_FROM_DASHBOARD)
			|| (xbox::LaunchDataPage->Header.dwLaunchDataType == LDT_FROM_DEBUGGER_CMDLINE))
		{
			*pdwLaunchDataType = xbox::LaunchDataPage->Header.dwLaunchDataType;
			memcpy(pLaunchData, &(xbox::LaunchDataPage->LaunchData[0]), sizeof(LAUNCH_DATA));

			// Now that LaunchDataPage is retrieved by the emulated software, free it :
			MmFreeContiguousMemory(xbox::LaunchDataPage);
			xbox::LaunchDataPage = NULL;

			ret = ERROR_SUCCESS;
		}
	}

	RETURN(ret);
}
#endif

// ******************************************************************
// * patch: XSetProcessQuantumLength
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(XSetProcessQuantumLength)
(
    dword_xt dwMilliseconds
)
{

	LOG_FUNC_ONE_ARG(dwMilliseconds);

	// TODO: Implement?
	LOG_IGNORED();
}
	
// ******************************************************************
// * patch: SignalObjectAndWait
// ******************************************************************
xbox::dword_xt WINAPI xbox::EMUPATCH(SignalObjectAndWait)
(
	HANDLE	hObjectToSignal,
	HANDLE	hObjectToWaitOn,
	dword_xt	dwMilliseconds,
	bool_xt	bAlertable
)
{

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hObjectToSignal)
		LOG_FUNC_ARG(hObjectToWaitOn)
		LOG_FUNC_ARG(dwMilliseconds)
		LOG_FUNC_ARG(bAlertable)
		LOG_FUNC_END;

	dword_xt dwRet = SignalObjectAndWait( hObjectToSignal, hObjectToWaitOn, dwMilliseconds, bAlertable );

	RETURN(dwRet);
}

// ******************************************************************
// * patch: timeSetEvent
// ******************************************************************
MMRESULT WINAPI xbox::EMUPATCH(timeSetEvent)
(
	uint_xt			uDelay,
	uint_xt			uResolution,
	LPTIMECALLBACK	fptc,
	dword_xt			dwUser,
	uint_xt			fuEvent
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
MMRESULT WINAPI xbox::EMUPATCH(timeKillEvent)
(
	uint_xt uTimerID  
)
{


	LOG_FUNC_ONE_ARG(uTimerID);

	MMRESULT Ret = timeKillEvent( uTimerID );

	RETURN(Ret);
}

// ******************************************************************
// * patch: RaiseException
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(RaiseException)
(
	dword_xt			dwExceptionCode,       // exception code
	dword_xt			dwExceptionFlags,      // continuable exception flag
	dword_xt			nNumberOfArguments,    // number of arguments
	CONST ulong_ptr_xt *lpArguments		   // array of arguments
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
xbox::dword_xt WINAPI xbox::EMUPATCH(XMountMUA)
(
	dword_xt dwPort,                  
	dword_xt dwSlot,                  
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
xbox::dword_xt WINAPI xbox::EMUPATCH(XGetDeviceEnumerationStatus)()
{


	LOG_FUNC();

	dword_xt ret = (g_bIsDevicesInitializing || g_bIsDevicesEmulating) ? XDEVICE_ENUMERATION_BUSY : XDEVICE_ENUMERATION_IDLE;

	RETURN(ret);
}

// ******************************************************************
// * patch: XInputGetDeviceDescription
// ******************************************************************
xbox::dword_xt WINAPI xbox::EMUPATCH(XInputGetDeviceDescription)
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
xbox::dword_xt WINAPI xbox::EMUPATCH(XMountMURootA)
(
	dword_xt dwPort,                  
	dword_xt dwSlot,                  
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
xbox::void_xt WINAPI xbox::EMUPATCH(OutputDebugStringA)
(
	IN LPCSTR lpOutputString
)
{

	LOG_FUNC_ONE_ARG(lpOutputString);
	printf("OutputDebugStringA: %s\n", lpOutputString);
}
