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

#undef FIELD_OFFSET     // prevent macro redefinition warnings
/* prevent name collisions */
namespace xboxkrnl
{
	#include <xboxkrnl\xboxkrnl.h>
};

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
#include "vsbc\CxbxVSBC.h"
#include "Windef.h"
#include <vector>
#include "core\hle\XAPI\Xapi.h"
#include "core\hle\XAPI\XapiCxbxr.h"

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
XTL::PXPP_DEVICE_TYPE g_DeviceType_Gamepad = nullptr;

// Flag is unset after initialize devices is done by simulate LLE USB thread.
std::atomic<bool> g_bIsDevicesInitializing = true;
std::atomic<bool> g_bIsDevicesEmulating = false;

//global bridge for xbox controller to host, 4 elements for 4 ports.
CXBX_CONTROLLER_HOST_BRIDGE g_XboxControllerHostBridge[4] = {
	{ NULL, PORT_INVALID, XBOX_INPUT_DEVICE::DEVICE_INVALID, nullptr, false, false, false, { 0, 0, 0, 0, 0 } },
	{ NULL, PORT_INVALID, XBOX_INPUT_DEVICE::DEVICE_INVALID, nullptr, false, false, false, { 0, 0, 0, 0, 0 } },
	{ NULL, PORT_INVALID, XBOX_INPUT_DEVICE::DEVICE_INVALID, nullptr, false, false, false, { 0, 0, 0, 0, 0 } },
	{ NULL, PORT_INVALID, XBOX_INPUT_DEVICE::DEVICE_INVALID, nullptr, false, false, false, { 0, 0, 0, 0, 0 } },
};


bool operator==(XTL::PXPP_DEVICE_TYPE XppType, XBOX_INPUT_DEVICE XidType)
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

	case XBOX_INPUT_DEVICE::LIGHT_GUN:
	case XBOX_INPUT_DEVICE::STEERING_WHEEL:
	case XBOX_INPUT_DEVICE::MEMORY_UNIT:
	case XBOX_INPUT_DEVICE::IR_DONGLE:
	case XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER:
	default:
		break;
	}

	return false;
}

bool operator!=(XTL::PXPP_DEVICE_TYPE XppType, XBOX_INPUT_DEVICE XidType)
{
	return !(XppType == XidType);
}

bool ConstructHleInputDevice(int Type, int Port)
{
	g_bIsDevicesEmulating = true;
	bool ret = true;

	switch (Type)
	{
	case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE): {
		g_XboxControllerHostBridge[Port].XboxPort = Port;
		g_XboxControllerHostBridge[Port].XboxType = XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE;
		g_XboxControllerHostBridge[Port].InState = new XpadInput();
		g_XboxControllerHostBridge[Port].bPendingRemoval = false;
		g_XboxControllerHostBridge[Port].bSignaled = false;
		g_XboxControllerHostBridge[Port].bIoInProgress = false;
		g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucType = XINPUT_DEVTYPE_GAMEPAD;
		g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucSubType = XINPUT_DEVSUBTYPE_GC_GAMEPAD;
		g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucInputStateSize = sizeof(XpadInput);
		g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucFeedbackSize = sizeof(XpadOutput);
		g_XboxControllerHostBridge[Port].XboxDeviceInfo.dwPacketNumber = 0;
	}
	break;
    case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_S): {
        g_XboxControllerHostBridge[Port].XboxPort = Port;
        g_XboxControllerHostBridge[Port].XboxType = XBOX_INPUT_DEVICE::MS_CONTROLLER_S;
        g_XboxControllerHostBridge[Port].InState = new XpadInput();
        g_XboxControllerHostBridge[Port].bPendingRemoval = false;
        g_XboxControllerHostBridge[Port].bSignaled = false;
        g_XboxControllerHostBridge[Port].bIoInProgress = false;
        g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucType = XINPUT_DEVTYPE_GAMEPAD;
        g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucSubType = XINPUT_DEVSUBTYPE_GC_GAMEPAD_ALT;
        g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucInputStateSize = sizeof(XpadInput);
        g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucFeedbackSize = sizeof(XpadOutput);
        g_XboxControllerHostBridge[Port].XboxDeviceInfo.dwPacketNumber = 0;
    }
	break;
	case to_underlying(XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER): {
		// TODO
	}
	break;

	case to_underlying(XBOX_INPUT_DEVICE::LIGHT_GUN):
	case to_underlying(XBOX_INPUT_DEVICE::STEERING_WHEEL):
	case to_underlying(XBOX_INPUT_DEVICE::MEMORY_UNIT):
	case to_underlying(XBOX_INPUT_DEVICE::IR_DONGLE): {
		EmuLog(LOG_LEVEL::INFO, "%s: device %s is not yet supported", __func__, GetInputDeviceName(Type).c_str());
		ret = false;
	}
	break;

	default:
		EmuLog(LOG_LEVEL::WARNING, "Attempted to attach an unknown device type (type was %d)", Type);
		ret = false;
		break;
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
	delete g_XboxControllerHostBridge[Port].InState;
	g_XboxControllerHostBridge[Port].bPendingRemoval = false;
	g_XboxControllerHostBridge[Port].bSignaled = false;
	g_XboxControllerHostBridge[Port].bIoInProgress = false;
	g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucType = 0;
	g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucSubType = 0;
	g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucInputStateSize = 0;
	g_XboxControllerHostBridge[Port].XboxDeviceInfo.ucFeedbackSize = 0;
	g_XboxControllerHostBridge[Port].XboxDeviceInfo.dwPacketNumber = 0;

	g_bIsDevicesEmulating = false;
}

void SetupXboxDeviceTypes()
{
	// If we don't yet have the offset to gDeviceType_Gamepad, work it out!
	if (g_DeviceType_Gamepad == nullptr) {
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
				printf("DeviceTable[%u]->XppType = 0x%08X (", i, (uintptr_t)deviceTable[i]->XppType);

                switch (deviceTable[i]->ucType) {
                case XINPUT_DEVTYPE_GAMEPAD:
                    g_DeviceType_Gamepad = deviceTable[i]->XppType;
                    printf("XDEVICE_TYPE_GAMEPAD)\n");
                    break;
                case XINPUT_DEVTYPE_STEELBATALION:
                    printf("XDEVICE_TYPE_STEELBATALION)\n");
                    break;
				default:
					printf("Unknown device type)\n");
					continue;
				}
			}
		} else {
			// XDKs without GetTypeInformation have the GamePad address hardcoded in XInputOpen
			// Only the earliest XDKs use this code path, and the offset never changed between them
			// so this works well for us.
			void* XInputOpenAddr = (void*)g_SymbolAddresses["XInputOpen"];
			if (XInputOpenAddr != nullptr) {
				printf("XAPI: Deriving XDEVICE_TYPE_GAMEPAD from XInputOpen (0x%08X)\n", (uintptr_t)XInputOpenAddr);
				g_DeviceType_Gamepad = *(XTL::PXPP_DEVICE_TYPE*)((uint32_t)XInputOpenAddr + 0x0B);
			}
		}

		if (g_DeviceType_Gamepad == nullptr) {
			EmuLog(LOG_LEVEL::WARNING, "XDEVICE_TYPE_GAMEPAD was not found");
			return;
		}

		printf("XAPI: XDEVICE_TYPE_GAMEPAD Found at 0x%08X\n", (uintptr_t)g_DeviceType_Gamepad);
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
void UpdateConnectedDeviceState(XTL::PXPP_DEVICE_TYPE DeviceType) {

	// Do not process the queries until initialize delay and device emulating are complete.
	if (g_bIsDevicesInitializing || g_bIsDevicesEmulating){
		return;
	}

	int Port, PortMask;
	for (Port = PORT_1, PortMask = 1; Port <= PORT_4; Port++, PortMask <<= 1) {
		if (DeviceType == g_XboxControllerHostBridge[Port].XboxType) {
			if (!g_XboxControllerHostBridge[Port].bPendingRemoval) {
				DeviceType->CurrentConnected |= PortMask;
			}
			else {
				if (!g_XboxControllerHostBridge[Port].bSignaled) {
					g_XboxControllerHostBridge[Port].bSignaled = true;
					SDL_Event DeviceRemoveEvent;
					SDL_memset(&DeviceRemoveEvent, 0, sizeof(SDL_Event));
					DeviceRemoveEvent.type = Sdl::DeviceRemoveAck_t;
					DeviceRemoveEvent.user.data1 = new int(Port);
					SDL_PushEvent(&DeviceRemoveEvent);
				}
				DeviceType->CurrentConnected &= ~PortMask;
			}
		}
		else {
			DeviceType->CurrentConnected &= ~PortMask;
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
DWORD WINAPI XTL::EMUPATCH(XGetDevices)
(
    PXPP_DEVICE_TYPE DeviceType
)
{
	LOG_FUNC_ONE_ARG(DeviceType);

	UpdateConnectedDeviceState(DeviceType);

	UCHAR oldIrql = xboxkrnl::KeRaiseIrqlToDpcLevel();

	DWORD ret = DeviceType->CurrentConnected;

	DeviceType->ChangeConnected = 0;
	DeviceType->PreviousConnected = DeviceType->CurrentConnected;

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

	UpdateConnectedDeviceState(DeviceType);

	BOOL ret = FALSE;


    if(!DeviceType->ChangeConnected)
    {
        *pdwInsertions = 0;
        *pdwRemovals = 0;
    }
    else
    {
        UCHAR oldIrql = xboxkrnl::KeRaiseIrqlToDpcLevel();

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
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(DeviceType)
		LOG_FUNC_ARG(dwPort)
		LOG_FUNC_ARG(dwSlot)
		LOG_FUNC_ARG(pPollingParameters)
		LOG_FUNC_END;

    if (dwPort >= PORT_1 && dwPort <= PORT_4) {
        if (DeviceType == g_XboxControllerHostBridge[dwPort].XboxType) {
#if 0 // TODO
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
#endif
			g_XboxControllerHostBridge[dwPort].hXboxDevice = &g_XboxControllerHostBridge[dwPort];
			RETURN(g_XboxControllerHostBridge[dwPort].hXboxDevice);
        }
        
    }
    
	RETURN(NULL);
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

	PCXBX_CONTROLLER_HOST_BRIDGE Device = (PCXBX_CONTROLLER_HOST_BRIDGE)hDevice;
	int Port = Device->XboxPort;
	if (g_XboxControllerHostBridge[Port].hXboxDevice == hDevice) {
		Device->hXboxDevice = NULL;
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

	// Nothing to do

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
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hDevice)
		LOG_FUNC_ARG_OUT(pCapabilities)
		LOG_FUNC_END;

    DWORD ret = ERROR_DEVICE_NOT_CONNECTED;
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
void EmuSBCGetState(XTL::PX_SBC_GAMEPAD pSBCGamepad, XTL::PXINPUT_GAMEPAD pXIGamepad, XTL::PXINPUT_GAMEPAD pDIGamepad)
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
    pSBCGamepad->wButtons[0] |= (XboxSBCGamepad.wButtons[0] & (CXBX_SBC_GAMEPAD_W0_COCKPITHATCH | CXBX_SBC_GAMEPAD_W0_IGNITION));

    pSBCGamepad->wButtons[2] |= (XboxSBCGamepad.wButtons[2]&( CXBX_SBC_GAMEPAD_W2_TOGGLEFILTERCONTROL
                                                            | CXBX_SBC_GAMEPAD_W2_TOGGLEOXYGENSUPPLY
                                                            | CXBX_SBC_GAMEPAD_W2_TOGGLEFUELFLOWRATE
                                                            | CXBX_SBC_GAMEPAD_W2_TOGGLEBUFFREMATERIAL
                                                            | CXBX_SBC_GAMEPAD_W2_TOGGLEVTLOCATION));


    // Analog Sticks
    pSBCGamepad->sAimingX = pXIGamepad->sThumbRX;
    pSBCGamepad->sAimingY = pXIGamepad->sThumbRY;
    pSBCGamepad->sRotationLever = 0;//(pXIGamepad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) ? 255 : 0;
    pSBCGamepad->sSightChangeX = pXIGamepad->sThumbLX;
    pSBCGamepad->sSightChangeY = pXIGamepad->sThumbLY;
    pSBCGamepad->wLeftPedal = ((SHORT)(pXIGamepad->bAnalogButtons[CXBX_XINPUT_GAMEPAD_LEFT_TRIGGER]))<<8;
    pSBCGamepad->wMiddlePedal=0;// = (pXIGamepad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) ? 255 : 0;
    pSBCGamepad->wRightPedal = (SHORT)(pXIGamepad->bAnalogButtons[CXBX_XINPUT_GAMEPAD_RIGHT_TRIGGER])<<8;
    pSBCGamepad->ucTunerDial=0;//low nibble

    
    // Digital Buttons
    if (pXIGamepad->bAnalogButtons [CXBX_XINPUT_GAMEPAD_A]>0) {
        pSBCGamepad->wButtons[0] |= CXBX_SBC_GAMEPAD_W0_RIGHTJOYMAINWEAPON;
    }
    else {
        pSBCGamepad->wButtons[0] &= ~CXBX_SBC_GAMEPAD_W0_RIGHTJOYMAINWEAPON;
    }
    if (pXIGamepad->bAnalogButtons[CXBX_XINPUT_GAMEPAD_B]>0) {
        pSBCGamepad->wButtons[0] |= CXBX_SBC_GAMEPAD_W0_RIGHTJOYFIRE;
    }
    else {
        pSBCGamepad->wButtons[0] &= ~CXBX_SBC_GAMEPAD_W0_RIGHTJOYFIRE;
    }
    if (pXIGamepad->bAnalogButtons[CXBX_XINPUT_GAMEPAD_X]>0) {
        pSBCGamepad->wButtons[0] |= CXBX_SBC_GAMEPAD_W0_RIGHTJOYLOCKON;
    }
    else {
        pSBCGamepad->wButtons[0] &= ~CXBX_SBC_GAMEPAD_W0_RIGHTJOYLOCKON;
    }
    if (pXIGamepad->bAnalogButtons[CXBX_XINPUT_GAMEPAD_Y]>0) {
        pSBCGamepad->wButtons[1] |= CXBX_SBC_GAMEPAD_W1_WEAPONCONMAGAZINE;
    }
    else {
        pSBCGamepad->wButtons[1] &= ~CXBX_SBC_GAMEPAD_W1_WEAPONCONMAGAZINE;
    }

    //GearLever 1~5 for gear 1~5, 7~13 for gear R,N,1~5, 15 for gear R. we use the continues range from 7~13
    if (pXIGamepad->bAnalogButtons[CXBX_XINPUT_GAMEPAD_WHITE]>0) {//Left Shouder, Gear Down
        if (pSBCGamepad->ucGearLever >7) {
            pSBCGamepad->ucGearLever-- ;
        }
    }

    if (pXIGamepad->bAnalogButtons[CXBX_XINPUT_GAMEPAD_BLACK]>0) {//Right Shouder, Gear Up
        if (pSBCGamepad->ucGearLever < 13) {
            pSBCGamepad->ucGearLever ++;
        }
    }
    //OLD_XINPUT
    /* //not used, don't duplicate the handling for same setting of pXIGamepad's members, later one will over write privous one.
    if (pXIGamepad->wButtons & CXBX_XINPUT_GAMEPAD_START) {
        pSBCGamepad->wButtons[0] |= CXBX_SBC_GAMEPAD_W0_START;
    }
    else {
        pSBCGamepad->wButtons[0] &= ~CXBX_SBC_GAMEPAD_W0_START;
    }
    */
    if (pXIGamepad->wButtons & CXBX_XINPUT_GAMEPAD_LEFT_THUMB) {//Center Sight Change
        pSBCGamepad->wButtons[2] |= CXBX_SBC_GAMEPAD_W2_LEFTJOYSIGHTCHANGE;
    }
    else {
        pSBCGamepad->wButtons[2] &= ~CXBX_SBC_GAMEPAD_W2_LEFTJOYSIGHTCHANGE;
    }
    //OLD_XINPUT
    /* //not used
    if (pXIGamepad->wButtons & CXBX_XINPUT_GAMEPAD_RIGHT_THUMB) {
        pSBCGamepad->wButtons |= CXBX_XINPUT_GAMEPAD_RIGHT_THUMB;
    }
    else {
        pSBCGamepad->wButtons &= ~CXBX_XINPUT_GAMEPAD_RIGHT_THUMB;
    }
    */


    //additional input from 2nd input, default using directinput
    if (pDIGamepad->bAnalogButtons[CXBX_XINPUT_GAMEPAD_A]>0) {
        pSBCGamepad->wButtons[0] |= CXBX_SBC_GAMEPAD_W0_START;
    }
    else {
        pSBCGamepad->wButtons[0] &= ~CXBX_SBC_GAMEPAD_W0_START;
    }
    // Iginition is Toggle Switch
    if (pDIGamepad->bAnalogButtons[CXBX_XINPUT_GAMEPAD_B]>0) {
        if (pSBCGamepad->wButtons[0] & CXBX_SBC_GAMEPAD_W0_IGNITION) {
            pSBCGamepad->wButtons[0] &= ~CXBX_SBC_GAMEPAD_W0_IGNITION;
        }
        else {
            pSBCGamepad->wButtons[0] |= CXBX_SBC_GAMEPAD_W0_IGNITION;
        }
    }

    if (pDIGamepad->bAnalogButtons[CXBX_XINPUT_GAMEPAD_X]>0) {
        pSBCGamepad->wButtons[0] |= CXBX_SBC_GAMEPAD_W0_EJECT;
    }
    else {
        pSBCGamepad->wButtons[0] &= ~CXBX_SBC_GAMEPAD_W0_EJECT;
    }
    // CockpitHatch is Toggle Switch
    if (pDIGamepad->bAnalogButtons[CXBX_XINPUT_GAMEPAD_Y]>0) {
        if (pSBCGamepad->wButtons[0] & CXBX_SBC_GAMEPAD_W0_COCKPITHATCH) {
            pSBCGamepad->wButtons[0] &= ~CXBX_SBC_GAMEPAD_W0_COCKPITHATCH;
        }
        else {
            pSBCGamepad->wButtons[0] |= CXBX_SBC_GAMEPAD_W0_COCKPITHATCH;
        }
    }

    if (pDIGamepad->wButtons & CXBX_XINPUT_GAMEPAD_BACK) {//Toggle Switch ToggleFilterControl
        if (pSBCGamepad->wButtons[2] & CXBX_SBC_GAMEPAD_W2_TOGGLEFILTERCONTROL) {
            pSBCGamepad->wButtons[2] &= ~CXBX_SBC_GAMEPAD_W2_TOGGLEFILTERCONTROL;
        }
        else {
            pSBCGamepad->wButtons[2] |= CXBX_SBC_GAMEPAD_W2_TOGGLEFILTERCONTROL;
        }
    }

    if (pDIGamepad->wButtons & CXBX_XINPUT_GAMEPAD_DPAD_UP) {//Toggle Switch ToggleOxygenSupply
        if (pSBCGamepad->wButtons[2] & CXBX_SBC_GAMEPAD_W2_TOGGLEOXYGENSUPPLY) {
            pSBCGamepad->wButtons[2] &= ~CXBX_SBC_GAMEPAD_W2_TOGGLEOXYGENSUPPLY;
        }
        else {
            pSBCGamepad->wButtons[2] |= CXBX_SBC_GAMEPAD_W2_TOGGLEOXYGENSUPPLY;
        }
    }

    if (pDIGamepad->wButtons & CXBX_XINPUT_GAMEPAD_DPAD_DOWN) {//Toggle Switch ToggleBuffreMaterial
        if (pSBCGamepad->wButtons[2] & CXBX_SBC_GAMEPAD_W2_TOGGLEBUFFREMATERIAL) {
            pSBCGamepad->wButtons[2] &= ~CXBX_SBC_GAMEPAD_W2_TOGGLEBUFFREMATERIAL;
        }
        else {
            pSBCGamepad->wButtons[2] |= CXBX_SBC_GAMEPAD_W2_TOGGLEBUFFREMATERIAL;
        }
    }

    if (pDIGamepad->wButtons & CXBX_XINPUT_GAMEPAD_DPAD_LEFT) {//Toggle Switch ToggleVTLocation
        if (pSBCGamepad->wButtons[2] & CXBX_SBC_GAMEPAD_W2_TOGGLEVTLOCATION) {
            pSBCGamepad->wButtons[2] &= ~CXBX_SBC_GAMEPAD_W2_TOGGLEVTLOCATION;
        }
        else {
            pSBCGamepad->wButtons[2] |= CXBX_SBC_GAMEPAD_W2_TOGGLEVTLOCATION;
        }
    }

    if (pDIGamepad->wButtons & CXBX_XINPUT_GAMEPAD_DPAD_RIGHT) {//Toggle Switch ToggleFuelFlowRate
        if (pSBCGamepad->wButtons[2] & CXBX_SBC_GAMEPAD_W2_TOGGLEFUELFLOWRATE) {
            pSBCGamepad->wButtons[2] &= ~CXBX_SBC_GAMEPAD_W2_TOGGLEFUELFLOWRATE;
        }
        else {
            pSBCGamepad->wButtons[2] |= CXBX_SBC_GAMEPAD_W2_TOGGLEFUELFLOWRATE;
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
    OUT PXINPUT_STATE  pState
)
{


	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hDevice)
		LOG_FUNC_ARG_OUT(pState)
		LOG_FUNC_END;

    DWORD ret = ERROR_DEVICE_NOT_CONNECTED;
    PCXBX_CONTROLLER_HOST_BRIDGE Device = (PCXBX_CONTROLLER_HOST_BRIDGE)hDevice;
    int Port = Device->XboxPort;
    if (g_XboxControllerHostBridge[Port].hXboxDevice == hDevice) {
        if (!g_XboxControllerHostBridge[Port].bPendingRemoval) {
            g_XboxControllerHostBridge[Port].bIoInProgress = true;
            if (g_InputDeviceManager.UpdateXboxPortInput(Port, g_XboxControllerHostBridge[Port].InState, DIRECTION_IN, to_underlying(g_XboxControllerHostBridge[Port].XboxType))) {
                pState->dwPacketNumber = g_XboxControllerHostBridge[Port].XboxDeviceInfo.dwPacketNumber++;
            }
            memcpy((void*)&pState->Gamepad, g_XboxControllerHostBridge[Port].InState, sizeof(pState->Gamepad));
            g_XboxControllerHostBridge[Port].bIoInProgress = false;
            ret = ERROR_SUCCESS;
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
            ObReferenceObjectByHandle(pFeedback->Header.hEvent, &xboxkrnl::ExEventObjectType, (PVOID*)&pFeedback->Header.IoCompletedEvent) == ERROR_SUCCESS) {
            KeSetEvent((xboxkrnl::PKEVENT)pFeedback->Header.IoCompletedEvent, NULL, FALSE);
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
				PopupInfo(nullptr, "The title is rebooting to dashboard");
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

	DWORD ret = (g_bIsDevicesInitializing || g_bIsDevicesEmulating) ? XDEVICE_ENUMERATION_BUSY : XDEVICE_ENUMERATION_IDLE;

	RETURN(ret);
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
