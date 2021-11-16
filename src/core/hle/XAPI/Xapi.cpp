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
#include "common\input\LibusbDevice.h" // include this after Shlwapi.h or else it causes an error
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
#include <charconv>


xbox::PXPP_DEVICE_TYPE g_DeviceType_Gamepad = nullptr;
xbox::PXPP_DEVICE_TYPE g_DeviceType_SBC = nullptr;
xbox::PXPP_DEVICE_TYPE g_DeviceType_MU = nullptr;

// Flag is unset after initialize devices is done by simulate LLE USB thread.
std::atomic<bool> g_bIsDevicesInitializing = true;
std::atomic<bool> g_bIsDevicesEmulating = false;

// Protects access to xpp types
std::atomic<bool> g_bXppGuard = false;

// Allocate enough memory for the max number of devices we can support simultaneously
// 4 duke / S / sbc / arcade joystick / lightgun (mutually exclusive) + 8 memory units
DeviceState g_devs[MAX_DEVS];

xbox::ulong_xt g_Mounted_MUs = 0;
xbox::char_xt g_AltLett_MU = 0;
xbox::ulong_xt *g_XapiMountedMUs = &g_Mounted_MUs;
xbox::char_xt *g_XapiAltLett_MU = &g_AltLett_MU;
std::recursive_mutex g_MuLock;

// Declare trampolines
#define XB_TRAMPOLINES(XB_MACRO)                                                                                                                                     \
    XB_MACRO(xbox::dword_xt,    WINAPI, XUnmountAlternateTitleA,  (xbox::char_xt)                                                                                );  \
    XB_MACRO(xbox::ntstatus_xt, WINAPI, XapiMapLetterToDirectory, (xbox::PSTRING, xbox::PSTRING, const xbox::PCHAR, xbox::bool_xt, xbox::PCWSTR, xbox::bool_xt)  );  \

XB_TRAMPOLINES(XB_trampoline_declare);

void LookupTrampolinesXAPI()
{
	XB_TRAMPOLINES(XB_trampoline_lookup);
}

#undef XB_TRAMPOLINES


static inline xbox::char_xt MuPort2Lett(xbox::dword_xt port, xbox::dword_xt slot)
{
	return 'F' + (XBOX_CTRL_NUM_SLOTS * port) + slot;
}

static inline int MuPort2Idx(xbox::dword_xt port, xbox::dword_xt slot)
{
	return (port << 1) + slot;
}

static inline bool MuIsMounted(xbox::char_xt lett)
{
	return *g_XapiMountedMUs & (1 << (lett - 'F'));
}

static inline void MuSetMounted(xbox::char_xt lett)
{
	*g_XapiMountedMUs |= (1 << (lett - 'F'));
}

static inline void MuClearMounted(xbox::char_xt lett)
{
	*g_XapiMountedMUs &= ~(1 << (lett - 'F'));
}

bool operator==(xbox::PXPP_DEVICE_TYPE XppType, XBOX_INPUT_DEVICE XidType)
{
	switch (XidType)
	{
	case XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE:
	case XBOX_INPUT_DEVICE::MS_CONTROLLER_S:
	case XBOX_INPUT_DEVICE::LIGHTGUN:
	case XBOX_INPUT_DEVICE::ARCADE_STICK:
	case XBOX_INPUT_DEVICE::HW_XBOX_CONTROLLER: {
		if (XppType == g_DeviceType_Gamepad) {
			return true;
		}
	}
	break;

	case XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER:
	case XBOX_INPUT_DEVICE::HW_STEEL_BATTALION_CONTROLLER: {
		if (XppType == g_DeviceType_SBC) {
			return true;
		}
	}
	break;

	case XBOX_INPUT_DEVICE::MEMORY_UNIT: {
		if (XppType == g_DeviceType_MU) {
			return true;
		}
	}
	break;

	case XBOX_INPUT_DEVICE::STEERING_WHEEL:
	case XBOX_INPUT_DEVICE::IR_DONGLE:
	default:
		break;
	}

	return false;
}

void UpdateXppState(DeviceState *dev, XBOX_INPUT_DEVICE type, std::string_view port)
{
	xbox::PXPP_DEVICE_TYPE xpp = nullptr;
	switch (type)
	{
	case XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE:
	case XBOX_INPUT_DEVICE::MS_CONTROLLER_S:
	case XBOX_INPUT_DEVICE::LIGHTGUN:
	case XBOX_INPUT_DEVICE::ARCADE_STICK:
	case XBOX_INPUT_DEVICE::HW_XBOX_CONTROLLER:
		xpp = g_DeviceType_Gamepad;
		break;

	case XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER:
	case XBOX_INPUT_DEVICE::HW_STEEL_BATTALION_CONTROLLER:
		xpp = g_DeviceType_SBC;
		break;

	case XBOX_INPUT_DEVICE::MEMORY_UNIT:
		xpp = g_DeviceType_MU;
		break;

	default:
		assert(0);
	}

	if (xpp == nullptr) {
		// This will happen with xbes that act like launchers, and don't link against the xinput libraries, which results in all the global
		// xpp types being nullptr. Test case: Innocent Tears
		return;
	}

	int port_num, slot;
	PortStr2Int(port, &port_num, &slot);
	xbox::ulong_xt port_mask = 1 << port_num;
	xbox::ulong_xt slot_mask = 0;

	// Guard against the unfortunate case where XGetDevices or XGetDeviceChanges have already checked for g_bIsDevicesInitializing
	// and g_bIsDevicesEmulating and a thread switch happens to this function
	while (g_bXppGuard) {}

	if (xpp == g_DeviceType_MU) {
		assert(slot != PORT_INVALID);
		if (slot == 1) {
			slot_mask = 16;
		}
	}

	if (xpp == dev->type && !dev->bPendingRemoval) {
		xpp->CurrentConnected |= (port_mask << slot_mask);
	}
	else {
		xpp->CurrentConnected &= ~(port_mask << slot_mask);
	}

	xpp->ChangeConnected = xpp->PreviousConnected ^ xpp->CurrentConnected;
}

void ConstructHleInputDevice(DeviceState *dev, DeviceState *upstream, int type, std::string_view port)
{
	g_bIsDevicesEmulating = true;

	if (g_bIsChihiro) {
		// Don't emulate XID devices during Chihiro Emulation
		g_bIsDevicesEmulating = false;
		return;
	}

	// Set up common device state
	int port_num, slot;
	PortStr2Int(port, &port_num, &slot);
	dev->upstream = nullptr;
	dev->port = port;
	dev->port_idx = port_num;
	dev->bPendingRemoval = false;
	dev->bSignaled = false;
	dev->info.dwPacketNumber = 0;
	dev->slots[SLOT_TOP] = dev->slots[SLOT_BOTTOM] = nullptr;

	// NOTE: initialize bAutoPollDefault to its default state, which varies depending on the device type
	switch (type)
	{
	case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE):
	case to_underlying(XBOX_INPUT_DEVICE::HW_XBOX_CONTROLLER):
		dev->type = XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE;
		dev->info.bAutoPollDefault = true;
		dev->info.ucType = XINPUT_DEVTYPE_GAMEPAD;
		dev->info.ucSubType = XINPUT_DEVSUBTYPE_GC_GAMEPAD;
		dev->info.ucInputStateSize = sizeof(XpadInput);
		dev->info.ucFeedbackSize = sizeof(XpadOutput);
		if (type == to_underlying(XBOX_INPUT_DEVICE::HW_XBOX_CONTROLLER)) {
			dev->type = XBOX_INPUT_DEVICE::HW_XBOX_CONTROLLER;
			char dev_name[50];
			g_EmuShared->GetInputDevNameSettings(dev_name, port_num);
			if (auto Device = g_InputDeviceManager.FindDevice(std::string(dev_name))) {
				dev->info.ucType = dynamic_cast<Libusb::LibusbDevice *>(Device.get())->GetUcType();
				dev->info.ucSubType = dynamic_cast<Libusb::LibusbDevice *>(Device.get())->GetUcSubType();
			}
		}
		break;

	case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_S):
		dev->type = XBOX_INPUT_DEVICE::MS_CONTROLLER_S;
		dev->info.bAutoPollDefault = true;
		dev->info.ucType = XINPUT_DEVTYPE_GAMEPAD;
		dev->info.ucSubType = XINPUT_DEVSUBTYPE_GC_GAMEPAD_ALT;
		dev->info.ucInputStateSize = sizeof(XpadInput);
		dev->info.ucFeedbackSize = sizeof(XpadOutput);
		break;

	case to_underlying(XBOX_INPUT_DEVICE::LIGHTGUN):
		dev->type = XBOX_INPUT_DEVICE::LIGHTGUN;
		dev->info.bAutoPollDefault = true;
		dev->info.ucType = XINPUT_DEVTYPE_GAMEPAD;
		dev->info.ucSubType = XINPUT_DEVSUBTYPE_GC_LIGHTGUN;
		dev->info.ucInputStateSize = sizeof(XpadInput);
		dev->info.ucFeedbackSize = sizeof(XpadOutput);
		dev->info.ligthgun.offset_x = dev->info.ligthgun.offset_y = 0;
		dev->info.ligthgun.offset_upp_x = dev->info.ligthgun.offset_upp_x = 0;
		dev->info.ligthgun.last_in_state = dev->info.ligthgun.turbo_delay = 0;
		dev->info.ligthgun.turbo = dev->info.ligthgun.last_turbo = 0;
		g_EmuShared->GetLightgunLaser(&dev->info.ligthgun.laser);
		break;

	case to_underlying(XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER):
	case to_underlying(XBOX_INPUT_DEVICE::HW_STEEL_BATTALION_CONTROLLER):
		dev->type = XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER;
		dev->info.buff.sbc.InBuffer.ucGearLever = 8;
		dev->info.buff.sbc.InBuffer.sAimingX = static_cast<uint8_t>(0x7F);
		dev->info.buff.sbc.InBuffer.sAimingY = static_cast<uint8_t>(0x7F);
		dev->info.bAutoPollDefault = true;
		dev->info.ucType = XINPUT_DEVTYPE_STEELBATTALION;
		dev->info.ucSubType = XINPUT_DEVSUBTYPE_GC_GAMEPAD;
		dev->info.ucInputStateSize = sizeof(SBCInput);
		dev->info.ucFeedbackSize = sizeof(SBCOutput);
		dev->info.sbc.last_in_state = 0;
		if (type == to_underlying(XBOX_INPUT_DEVICE::HW_STEEL_BATTALION_CONTROLLER)) {
			dev->type = XBOX_INPUT_DEVICE::HW_STEEL_BATTALION_CONTROLLER;
			char dev_name[50];
			g_EmuShared->GetInputDevNameSettings(dev_name, port_num);
			if (auto Device = g_InputDeviceManager.FindDevice(std::string(dev_name))) {
				dev->info.ucType = dynamic_cast<Libusb::LibusbDevice *>(Device.get())->GetUcType();
				dev->info.ucSubType = dynamic_cast<Libusb::LibusbDevice *>(Device.get())->GetUcSubType();
			}
		}
		break;

	case to_underlying(XBOX_INPUT_DEVICE::ARCADE_STICK):
		dev->type = XBOX_INPUT_DEVICE::ARCADE_STICK;
		dev->info.bAutoPollDefault = true;
		dev->info.ucType = XINPUT_DEVTYPE_GAMEPAD;
		dev->info.ucSubType = XINPUT_DEVSUBTYPE_GC_ARCADE_STICK;
		dev->info.ucInputStateSize = sizeof(XpadInput);
		dev->info.ucFeedbackSize = sizeof(XpadOutput);
		break;

	case to_underlying(XBOX_INPUT_DEVICE::MEMORY_UNIT):
		assert(upstream != nullptr);
		dev->upstream = upstream;
		dev->type = XBOX_INPUT_DEVICE::MEMORY_UNIT;
		dev->port_idx = PORT_INVALID;
		assert(slot != PORT_INVALID);
		dev->upstream->slots[slot] = dev;
		break;

	default:
		assert(0);
	}

	UpdateXppState(dev, static_cast<XBOX_INPUT_DEVICE>(type), port);

	g_bIsDevicesEmulating = false;

	EmuLogEx(CXBXR_MODULE::INPSYS, LOG_LEVEL::INFO, "Attached device %s to port %s", GetInputDeviceName(type).c_str(), PortUserFormat(port).c_str());
}

void DestructHleInputDevice(DeviceState *dev)
{
	g_bIsDevicesEmulating = true;

	// Clear common device state
	XBOX_INPUT_DEVICE type = dev->type;
	std::string port = dev->port;
	dev->port_idx = PORT_INVALID;
	dev->type = XBOX_INPUT_DEVICE::DEVICE_INVALID;
	dev->port = std::to_string(PORT_INVALID);
	dev->bPendingRemoval = false;
	dev->bSignaled = false;
	dev->slots[SLOT_TOP] = dev->slots[SLOT_BOTTOM] = nullptr;

	switch (type)
	{
	case XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE:
	case XBOX_INPUT_DEVICE::MS_CONTROLLER_S:
	case XBOX_INPUT_DEVICE::LIGHTGUN:
		dev->info.bAutoPollDefault = false;
		dev->info.ucType = 0;
		dev->info.ucSubType = 0;
		dev->info.ucInputStateSize = 0;
		dev->info.ucFeedbackSize = 0;
		dev->info.dwPacketNumber = 0;
		std::memset(&dev->info.buff.ctrl.InBuffer, 0, sizeof(XpadInput));
		break;

	case XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER:
		dev->info.bAutoPollDefault = false;
		dev->info.ucType = 0;
		dev->info.ucSubType = 0;
		dev->info.ucInputStateSize = 0;
		dev->info.ucFeedbackSize = 0;
		dev->info.dwPacketNumber = 0;
		std::memset(&dev->info.buff.sbc.InBuffer, 0, sizeof(SBCInput));
		break;

	case XBOX_INPUT_DEVICE::ARCADE_STICK:
		dev->info.bAutoPollDefault = false;
		dev->info.ucType = 0;
		dev->info.ucSubType = 0;
		dev->info.ucInputStateSize = 0;
		dev->info.ucFeedbackSize = 0;
		dev->info.dwPacketNumber = 0;
		std::memset(&dev->info.buff.ctrl.InBuffer, 0, sizeof(XpadInput));
		break;

	case XBOX_INPUT_DEVICE::MEMORY_UNIT: {
		assert(dev->upstream != nullptr);
		int port_num, slot;
		PortStr2Int(port, &port_num, &slot);
		assert(slot != PORT_INVALID);
		dev->upstream->slots[slot] = nullptr;
	}
	break;

	default:
		assert(0);
	}

	UpdateXppState(dev, type, port);
	dev->upstream = nullptr;

	g_bIsDevicesEmulating = false;

	EmuLogEx(CXBXR_MODULE::INPSYS, LOG_LEVEL::INFO, "Detached device %s from port %s", GetInputDeviceName(to_underlying(type)).c_str(), PortUserFormat(port).c_str());
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
				CxbxrKrnlAbort("DeviceTable Location is outside of Xbox Memory range");
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

		EmuLog(LOG_LEVEL::INFO, "XDEVICE_TYPE_GAMEPAD found at 0x%08X", (uintptr_t)g_DeviceType_Gamepad);
	}

	if (xbox::addr_xt mu_xpp_type = g_SymbolAddresses["g_DeviceType_MU"]) {
		g_DeviceType_MU = reinterpret_cast<xbox::PXPP_DEVICE_TYPE>(mu_xpp_type);
		EmuLog(LOG_LEVEL::INFO, "XDEVICE_TYPE_MEMORY_UNIT found at 0x%08X", reinterpret_cast<uintptr_t>(g_DeviceType_MU));
	}
	else {
		EmuLog(LOG_LEVEL::INFO, "XDEVICE_TYPE_MEMORY_UNIT was not found by XbSymbolDatabase");
	}

	if (xbox::addr_xt xapi_mounted_mu = g_SymbolAddresses["g_XapiMountedMUs"]) {
		g_XapiMountedMUs = reinterpret_cast<xbox::ulong_xt *>(xapi_mounted_mu);
		EmuLog(LOG_LEVEL::INFO, "XapiMountedMUs found at 0x%08X", reinterpret_cast<uintptr_t>(g_XapiMountedMUs));

		g_XapiAltLett_MU = reinterpret_cast<xbox::char_xt *>(g_XapiMountedMUs - 1);
		EmuLog(LOG_LEVEL::INFO, "XapiAltLett_MU found at 0x%08X", reinterpret_cast<uintptr_t>(g_XapiAltLett_MU));
	}
	else {
		EmuLog(LOG_LEVEL::INFO, "XapiMountedMUs was not found by XbSymbolDatabase");
	}
}

template<bool IsXInputPoll>
xbox::dword_xt CxbxImpl_XInputHandler(xbox::HANDLE hDevice, xbox::PXINPUT_STATE pState)
{
	xbox::dword_xt status = ERROR_DEVICE_NOT_CONNECTED;
	DeviceState *dev = static_cast<DeviceState *>(hDevice);
	int port = dev->port_idx;

	if ((g_devs[port].info.hHandle == hDevice) && !g_devs[port].bPendingRemoval) {
		if (g_devs[port].info.bAutoPoll != IsXInputPoll) {
			if (g_InputDeviceManager.UpdateXboxPortInput(port, &g_devs[port].info.buff, DIRECTION_IN, to_underlying(g_devs[port].type))) {
				g_devs[port].info.dwPacketNumber++;
			}
		}

		if constexpr (!IsXInputPoll) {
			std::memcpy((void *)&pState->Gamepad, reinterpret_cast<uint8_t *>(&g_devs[port].info.buff) + XID_PACKET_HEADER, g_devs[port].info.ucInputStateSize);
			pState->dwPacketNumber = g_devs[port].info.dwPacketNumber;
		}

		status = ERROR_SUCCESS;
	}

	return status;
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

	g_bXppGuard = true;

	if (g_bIsDevicesInitializing || g_bIsDevicesEmulating) {
		g_bXppGuard = false;
		RETURN(DeviceType->CurrentConnected);
	}

	for (unsigned i = 0; i < 12; ++i) {
		if (static_cast<uint8_t>(g_devs[i].bPendingRemoval) &
			~(static_cast<uint8_t>(g_devs[i].bSignaled))) {
			g_devs[i].bSignaled = true;
			SDL_Event DeviceRemoveEvent;
			SDL_memset(&DeviceRemoveEvent, 0, sizeof(SDL_Event));
			DeviceRemoveEvent.type = Sdl::DeviceRemoveAck_t;
			DeviceRemoveEvent.user.data1 = new std::string(g_devs[i].port);
			SDL_PushEvent(&DeviceRemoveEvent);
		}
	}

	UCHAR oldIrql = xbox::KeRaiseIrqlToDpcLevel();

	dword_xt ret = DeviceType->CurrentConnected;

	DeviceType->ChangeConnected = 0;
	DeviceType->PreviousConnected = DeviceType->CurrentConnected;

	xbox::KfLowerIrql(oldIrql);

	g_bXppGuard = false;

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

	g_bXppGuard = true;
	if (g_bIsDevicesInitializing || g_bIsDevicesEmulating) {
		*pdwInsertions = 0;
		*pdwRemovals = 0;
		g_bXppGuard = false;
		RETURN(FALSE);
	}

	for (unsigned i = 0; i < 12; ++i) {
		if (static_cast<uint8_t>(g_devs[i].bPendingRemoval) &
			~(static_cast<uint8_t>(g_devs[i].bSignaled))) {
			g_devs[i].bSignaled = true;
			SDL_Event DeviceRemoveEvent;
			SDL_memset(&DeviceRemoveEvent, 0, sizeof(SDL_Event));
			DeviceRemoveEvent.type = Sdl::DeviceRemoveAck_t;
			DeviceRemoveEvent.user.data1 = new std::string(g_devs[i].port);
			SDL_PushEvent(&DeviceRemoveEvent);
		}
	}

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
	g_bXppGuard = false;

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

	if (DeviceType == g_DeviceType_MU) {
		// MUs cannot be opened with XInputOpen
		RETURN(NULL);
	}

    if (dwPort >= PORT_1 && dwPort <= PORT_4) {
        if (DeviceType == g_devs[dwPort].type) {
			g_devs[dwPort].info.bAutoPoll = pPollingParameters != xbox::zeroptr ?
				pPollingParameters->fAutoPoll : g_devs[dwPort].info.bAutoPollDefault;
			g_devs[dwPort].info.hHandle = &g_devs[dwPort];
			RETURN(g_devs[dwPort].info.hHandle);
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

	DeviceState *dev = static_cast<DeviceState *>(hDevice);
	if (g_devs[dev->port_idx].info.hHandle == hDevice) {
		dev->info.hHandle = NULL;
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

	// Test cases: Oddworld: Stranger's Wrath, Jet Set Radio Future, Splinter Cell 2, Panzer Dragoon Orta and 1
	dword_xt ret = CxbxImpl_XInputHandler<true>(hDevice, nullptr);

	RETURN(ret);
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
	DeviceState *dev = static_cast<DeviceState *>(hDevice);
	int port = dev->port_idx;
    if (g_devs[port].info.hHandle == hDevice && !g_devs[port].bPendingRemoval) {
        pCapabilities->SubType = g_devs[port].info.ucSubType;
        UCHAR* pCap = (UCHAR*)(&pCapabilities->In);
        std::memset(pCap, 0xFF, g_devs[port].info.ucInputStateSize + g_devs[port].info.ucFeedbackSize);
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

	dword_xt ret = CxbxImpl_XInputHandler<false>(hDevice, pState);
    
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

	DeviceState *dev = static_cast<DeviceState *>(hDevice);
	int port = dev->port_idx;
    if (g_devs[port].info.hHandle == hDevice && !g_devs[port].bPendingRemoval) {
        pFeedback->Header.dwStatus = ERROR_IO_PENDING;
        g_InputDeviceManager.UpdateXboxPortInput(port, (void*)&pFeedback->Header.bReportId, DIRECTION_OUT, to_underlying(g_devs[port].type));
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
	HANDLE                      hDevice,
	PXINPUT_DEVICE_DESCRIPTION	pDescription
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hDevice)
		LOG_FUNC_ARG(pDescription)
		LOG_FUNC_END;

	dword_xt ret;
	int port = static_cast<DeviceState *>(hDevice)->port_idx;
	if (g_devs[port].info.hHandle == hDevice && !g_devs[port].bPendingRemoval) {
		if (g_devs[port].type == XBOX_INPUT_DEVICE::LIGHTGUN) {
			// These values are those reported in the device descriptor for the EMS TopGun II documented in the xboxdevwiki
			pDescription->wVendorID = 0x0b9a;
			pDescription->wProductID = 0x016b;
			pDescription->wVersion = 0x457;
			ret = ERROR_SUCCESS;
		}
		else {
			// NOTE: Phantasy star online also calls this on the keyboard device
			ret = ERROR_NOT_SUPPORTED;
		}
	}
	else {
		ret = ERROR_DEVICE_NOT_CONNECTED;
	}

	RETURN(ret);
}

// ******************************************************************
// * patch: XInputSetLightgunCalibration
// ******************************************************************
xbox::dword_xt WINAPI xbox::EMUPATCH(XInputSetLightgunCalibration)
(
	HANDLE hDevice,
	PXINPUT_LIGHTGUN_CALIBRATION_OFFSETS pCalibrationOffsets
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hDevice)
		LOG_FUNC_ARG(pCalibrationOffsets)
		LOG_FUNC_END;

	dword_xt ret;
	int port = static_cast<DeviceState *>(hDevice)->port_idx;
	if (g_devs[port].info.hHandle == hDevice && !g_devs[port].bPendingRemoval) {
		if (g_devs[port].type == XBOX_INPUT_DEVICE::LIGHTGUN) {
			g_devs[port].info.ligthgun.offset_x = pCalibrationOffsets->wCenterX;
			g_devs[port].info.ligthgun.offset_y = pCalibrationOffsets->wCenterY;
			g_devs[port].info.ligthgun.offset_upp_x = pCalibrationOffsets->wUpperLeftX;
			g_devs[port].info.ligthgun.offset_upp_y = pCalibrationOffsets->wUpperLeftY;
			ret = ERROR_SUCCESS;
		}
		else {
			ret = ERROR_NOT_SUPPORTED;
		}
	}
	else {
		ret = ERROR_DEVICE_NOT_CONNECTED;
	}

	RETURN(ret);
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
			CxbxrKrnlAbort("Too many thread notification routines installed\n");

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
			char szDashboardPath[xbox::max_path] = { 0 };
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
				CxbxrKrnlAbort("The xbe rebooted to Dashboard and xboxdash.xbe could not be found");
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

	std::lock_guard lock(g_MuLock);

	char lett = MuPort2Lett(dwPort, dwSlot);
	if (MuIsMounted(lett)) {
		if (pchDrive != zeroptr) {
			*pchDrive = lett;
		}
		RETURN(ERROR_ALREADY_ASSIGNED);
	}

	char title_id_buff[9];
	std::sprintf(title_id_buff, "%08lx", CxbxKrnl_Xbe->m_Certificate.dwTitleId);
	std::string mu_path_str(DrivePrefix + lett + ":");
	std::string mu_dev_str(DeviceMU + std::to_string(MuPort2Idx(dwPort, dwSlot)));
	ANSI_STRING mu_dev, mu_path;
	RtlInitAnsiString(&mu_path, mu_path_str.data());
	RtlInitAnsiString(&mu_dev, mu_dev_str.data());
	mu_dev_str += '\\';

	ntstatus_xt status = XB_TRMP(XapiMapLetterToDirectory)(&mu_path, &mu_dev, title_id_buff, 1,
		reinterpret_cast<const char16_t *>(CxbxKrnl_Xbe->m_Certificate.wsTitleName), 0);

	if (!nt_success(status)) {
		RETURN(RtlNtStatusToDosError(status));
	}

	MuSetMounted(lett);
	if (pchDrive != zeroptr) {
		*pchDrive = lett;
	}

	RETURN(ERROR_SUCCESS);
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

	std::lock_guard lock(g_MuLock);

	char_xt lett = MuPort2Lett(dwPort, dwSlot);
	if (MuIsMounted(lett)) {
		if (pchDrive != zeroptr) {
			*pchDrive = lett;
		}
		RETURN(ERROR_ALREADY_ASSIGNED);
	}

	std::string mu_path_str(DrivePrefix + lett + ":");
	std::string mu_dev_str(DeviceMU + std::to_string(MuPort2Idx(dwPort, dwSlot)));
	ANSI_STRING mu_dev, mu_path;
	RtlInitAnsiString(&mu_path, mu_path_str.data());
	RtlInitAnsiString(&mu_dev, mu_dev_str.data());
	ntstatus_xt status = IoCreateSymbolicLink(&mu_path, &mu_dev);

	if (!nt_success(status)) {
		if (pchDrive != zeroptr) {
			*pchDrive = 0;
		}
		RETURN(RtlNtStatusToDosError(status));
	}

	MuSetMounted(lett);
	if (pchDrive != zeroptr) {
		*pchDrive = lett;
	}

	RETURN(ERROR_SUCCESS);
}

// ******************************************************************
// * patch: XUnmountMU
// ******************************************************************
xbox::dword_xt WINAPI xbox::EMUPATCH(XUnmountMU)
(
	dword_xt dwPort,
	dword_xt dwSlot
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(dwPort)
		LOG_FUNC_ARG(dwSlot)
	LOG_FUNC_END;

	std::lock_guard lock(g_MuLock);

	char_xt lett = MuPort2Lett(dwPort, dwSlot);
	if (!MuIsMounted(lett)) {
		RETURN(ERROR_INVALID_DRIVE);
	}

	if (*g_XapiAltLett_MU == lett) {
		XB_TRMP(XUnmountAlternateTitleA)('X');
	}

	std::string mu_path_str(DrivePrefix + lett + ":");
	ANSI_STRING mu_path;
	RtlInitAnsiString(&mu_path, mu_path_str.data());
	ntstatus_xt status = IoDeleteSymbolicLink(&mu_path);

	if (!nt_success(status)) {
		RETURN(RtlNtStatusToDosError(status));
	}

	MuClearMounted(lett);
	g_io_mu_metadata->flush(static_cast<wchar_t>(lett));

	RETURN(ERROR_SUCCESS);
}

// ******************************************************************
// * patch: XReadMUMetaData
// ******************************************************************
xbox::dword_xt WINAPI xbox::EMUPATCH(XReadMUMetaData)
(
	IN dword_xt dwPort,
	IN dword_xt dwSlot,
	IN LPVOID lpBuffer,
	IN dword_xt dwByteOffset,
	IN dword_xt dwNumberOfBytesToRead
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(dwPort)
		LOG_FUNC_ARG(dwSlot)
		LOG_FUNC_ARG(lpBuffer)
		LOG_FUNC_ARG(dwByteOffset)
		LOG_FUNC_ARG(dwNumberOfBytesToRead)
		LOG_FUNC_END;

	// NOTE: in reality, this function should actually use IoSynchronousFsdRequest to read the metadata of the MU. Unfortunately,
	// that requires kernel support for device objects, which when this was implemented, was non-existent. So, we instead cheat
	// and use NtFsControlFile to perform the same action.

	std::lock_guard lock(g_MuLock);

	bool unmount = false;
	char_xt lett = MuPort2Lett(dwPort, dwSlot);
	if (!MuIsMounted(lett)) {
		unmount = true;
		dword_xt ret = EMUPATCH(XMountMURootA(dwPort, dwSlot, &lett));
		if (ret != ERROR_SUCCESS) {
			RETURN(ret);
		}
	}

	OBJECT_ATTRIBUTES obj;
	std::string mu_path_str(DrivePrefix + lett + ":");
	ANSI_STRING mu_path;
	RtlInitAnsiString(&mu_path, mu_path_str.data());
	XB_InitializeObjectAttributes(&obj, &mu_path, obj_case_insensitive, ObDosDevicesDirectory());

	HANDLE handle;
	IO_STATUS_BLOCK io_status_block;
	ntstatus_xt status = NtOpenFile(&handle,
		SYNCHRONIZE | GENERIC_READ,
		&obj,
		&io_status_block,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		FILE_SYNCHRONOUS_IO_ALERT);

	if (nt_success(status)) {
		fatx_volume_metadata volume;
		volume.offset = dwByteOffset;
		volume.length = dwNumberOfBytesToRead;
		volume.buffer = new char[dwNumberOfBytesToRead];

		status = NtFsControlFile(handle,
			zeroptr,
			zeroptr,
			zeroptr,
			&io_status_block,
			fsctl_read_fatx_metadata,
			&volume,
			sizeof(volume),
			zeroptr,
			0);

		if (nt_success(status)) {
			std::memcpy(lpBuffer, volume.buffer, dwNumberOfBytesToRead);
			status = status_success;
		}
		else {
			status = status_unrecognized_volume;
		}

		delete[] volume.buffer;
		NtClose(handle);
	}
	else {
		status = status_unrecognized_volume;
	}

	if (unmount) {
		EMUPATCH(XUnmountMU(dwPort, dwSlot));
	}

	RETURN(RtlNtStatusToDosError(status));
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
