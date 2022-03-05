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
// *  (c) 2021 ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

#define LOG_PREFIX CXBXR_MODULE::LIBUSB

#include "LibusbDevice.h"
#include "InputManager.h"
#include "core\kernel\support\Emu.h"
#include "core\hle\XAPI\Xapi.h"

// Sanity check: ensure that our libusb version is high enough for libusb_get_device_descriptor to succeed and to pass nullptr to libusb_interrupt_transfer
static_assert(LIBUSB_API_VERSION >= 0x01000105);


namespace Libusb
{
	int InitStatus = NOT_INIT;

	// These come from here https://github.com/xboxdrv/xboxdrv/blob/ac6ebb1228962220482ea03743cadbe18754246c/src/xpad_device.cpp#L29
	static constexpr uint16_t SupportedDevices_VidPid[][2] = { // vid, pid
		0x0d2f, 0x0002,
		0x045e, 0x0202,
		0x045e, 0x0285,
		0x045e, 0x0287,
		0x045e, 0x0289,
		0x046d, 0xca84,
		0x046d, 0xca88,
		0x05fd, 0x1007,
		0x05fd, 0x107a,
		0x0738, 0x4516,
		0x0738, 0x4522,
		0x0738, 0x4526,
		0x0738, 0x4536,
		0x0738, 0x4556,
		0x0c12, 0x8802,
		0x0c12, 0x8810,
		0x0c12, 0x9902,
		0x0e4c, 0x1097,
		0x0e4c, 0x2390,
		0x0e6f, 0x0003,
		0x0e6f, 0x0005,
		0x0e6f, 0x0006,
		0x0f30, 0x0202,
		0x0f30, 0x8888,
		0x102c, 0xff0c,
		0x044f, 0x0f07,
		0x0e8f, 0x3008,
	};

	static constexpr const char *SupportedDevices_Name[] = {
		"Andamiro Pump It Up pad",
		"Microsoft X-Box pad v1 (US)",
		"Microsoft X-Box pad (Japan)",
		"Microsoft Xbox Controller S",
		"Microsoft X-Box pad v2 (US)",
		"Logitech Xbox Cordless Controller",
		"Logitech Compact Controller for Xbox",
		"Mad Catz Controller (unverified)",
		"InterAct 'PowerPad Pro' X-Box pad (Germany)",
		"Mad Catz Control Pad",
		"Mad Catz LumiCON",
		"Mad Catz Control Pad Pro",
		"Mad Catz MicroCON",
		"Mad Catz Lynx Wireless Controller",
		"Zeroplus Xbox Controller",
		"Zeroplus Xbox Controller",
		"HAMA VibraX - *FAULTY HARDWARE*",
		"Radica Gamester Controller",
		"Radica Games Jtech Controller",
		"Logic3 Freebird wireless Controller",
		"Eclipse wireless Controller",
		"Edge wireless Controller",
		"Joytech Advanced Controller",
		"BigBen XBMiniPad Controller",
		"Joytech Wireless Advanced Controller",
		"Thrustmaster, Inc. Controller",
		"Generic xbox control (dealextreme)",
	};

	static_assert(ARRAY_SIZE(SupportedDevices_VidPid) == ARRAY_SIZE(SupportedDevices_Name));

	void Init(std::mutex &Mtx)
	{
		std::unique_lock<std::mutex> lck(Mtx);

		// We only use a single libusb session per cxbxr process, so we do not need to use a libusb context
		if (libusb_init(nullptr) != 0) {
			EmuLog(LOG_LEVEL::ERROR2, "Failed to initialize Libusb!");
			InitStatus = INIT_ERROR;
			return;
		}

		InitStatus = INIT_SUCCESS;
	}

	void DeInit()
	{
		InitStatus = NOT_INIT;
		libusb_exit(nullptr);
	}

	void PopulateDevices()
	{
		// NOTE: the libusb docs say that the list is always appended with a NULL element at the end
		libusb_device **List;
		ssize_t DevicesConnected = libusb_get_device_list(nullptr, &List) - 1;
		if (DevicesConnected < 0) {
			EmuLog(LOG_LEVEL::ERROR2, "Failed to enumerate devices. The error was: %s", libusb_strerror(DevicesConnected));
			return;
		}

		for (ssize_t i = 0; i < DevicesConnected; ++i) {
			libusb_device *LibusbDev = List[i];
			libusb_device_descriptor Desc;
			libusb_get_device_descriptor(LibusbDev, &Desc); // always succeeds when LIBUSB_API_VERSION >= 0x01000102
			auto Device = std::make_shared<LibusbDevice>(&Desc, LibusbDev);
			if (Device->IsLibusb()) {
				g_InputDeviceManager.AddDevice(std::move(Device));
			}
		}

		libusb_free_device_list(List, 1);
	}

	void GetDeviceChanges()
	{
		g_InputDeviceManager.RemoveDevice([](const auto &Device) {
			return Device->IsLibusb();
			});
		PopulateDevices();
	}

	LibusbDevice::LibusbDevice(libusb_device_descriptor *Desc, libusb_device *Dev)
	{
		m_Type = XBOX_INPUT_DEVICE::DEVICE_INVALID;

		// The SBC's VID and PID are taken from https://xboxdevwiki.net/Xbox_Input_Devices#Steel_Battalion_Controller
		if ((Desc->idVendor == 0x0a7b) && (Desc->idProduct == 0xd000)) {
			m_Type = XBOX_INPUT_DEVICE::HW_STEEL_BATTALION_CONTROLLER;
			m_UcType = XINPUT_DEVTYPE_STEELBATTALION;
			m_UcSubType = XINPUT_DEVSUBTYPE_GC_GAMEPAD;
			m_Name = "Steel battalion controller";
			m_BufferInSize = sizeof(XidSBCInput);
			m_BufferOutSize = sizeof(XidSBCOutput);
			assert(Desc->bcdUSB == 0x110); // must be a usb 1.1 device
		}
		else {
			for (size_t i = 0; i < ARRAY_SIZE(SupportedDevices_VidPid); ++i) {
				if ((Desc->idVendor = SupportedDevices_VidPid[i][0]) && (Desc->idProduct == SupportedDevices_VidPid[i][1])) {
					m_Type = XBOX_INPUT_DEVICE::HW_XBOX_CONTROLLER;
					m_UcType = XINPUT_DEVTYPE_GAMEPAD;
					m_UcSubType = XINPUT_DEVSUBTYPE_GC_GAMEPAD;
					m_Name = SupportedDevices_Name[i];
					m_BufferInSize = sizeof(XidGamepadInput);
					m_BufferOutSize = sizeof(XidGamepadOutput);
					assert(Desc->bcdUSB == 0x110); // must be a usb 1.1 device
					break;
				}
			}
		}

		if (m_Type == XBOX_INPUT_DEVICE::DEVICE_INVALID) { return; }

		// Duke, S and SBC have 1 configuration, 1 interface and 2 endpoints (input and output) and use the default alternate setting zero.
		// The code below assumes that third-party controllers follow suit.
		if (libusb_open(Dev, &m_hDev) == 0) {
			libusb_config_descriptor *ConfDesc;
			if (libusb_get_active_config_descriptor(Dev, &ConfDesc) == 0) {
				if (ConfDesc->bNumInterfaces == 1) {
					auto Iface = ConfDesc->interface[0];
					if (Iface.num_altsetting == 1) {
						auto Setting = Iface.altsetting[0];
						m_IfaceNum = Setting.bInterfaceNumber;
						if (Setting.bNumEndpoints >= 1) {
							m_HasEndpointOut = false;
							for (uint8_t i = 0; i < Setting.bNumEndpoints; ++i) {
								auto Endpoint = Setting.endpoint[i];
								if (Endpoint.bEndpointAddress & 0x80) {
									m_EndpointIn = Endpoint.bEndpointAddress;
									m_IntervalIn = Endpoint.bInterval;
								}
								else {
									// third-party controllers that don't support rumble probably won't have an out endpoint
									m_EndpointOut = Endpoint.bEndpointAddress;
									m_IntervalOut = Endpoint.bInterval;
									m_HasEndpointOut = true;
								}
							}
							EmuLog(LOG_LEVEL::INFO, "Out endpoint %s", m_HasEndpointOut ? "present" : "not present");
							if (int err = libusb_claim_interface(m_hDev, m_IfaceNum) != 0) {
								EmuLog(LOG_LEVEL::INFO, "Rejected device %s because libusb could not claim its interface. The error was: %s",
									m_Name.c_str(), libusb_strerror(err));
								m_Type = XBOX_INPUT_DEVICE::DEVICE_INVALID;
							}
							else {
								// Grab the xid descriptor so that we can report real type/subtype values back to the title when it calls XInputGetCapabilities
								XidDesc XidDesc;
								if (libusb_control_transfer(m_hDev, 0xC1, 6, 0x4200, m_IfaceNum, reinterpret_cast<uint8_t *>(&XidDesc), sizeof(XidDesc), m_IntervalIn)
									== sizeof(XidDesc)) { // submit a GET_DESCRIPTOR request

									// Dump the xid descriptor to the log
									EmuLog(LOG_LEVEL::INFO, "Xid descriptor dump:\nbLength: %#010X\nbDescriptorType: %#010X\nbcdXid: %#010X\nbType: %#010X\n"
										"bSubType: %#010X\nbMaxInputReportSize: %#010X\nbMaxOutputReportSize: %#010X\nwAlternateProductIds[0]: %#010X\n"
										"wAlternateProductIds[1]: %#010X\nwAlternateProductIds[2]: %#010X\nwAlternateProductIds[3]: %#010X\n",
										XidDesc.bLength, XidDesc.bDescriptorType, XidDesc.bcdXid, XidDesc.bType, XidDesc.bSubType, XidDesc.bMaxInputReportSize, XidDesc.bMaxOutputReportSize,
										XidDesc.wAlternateProductIds[0], XidDesc.wAlternateProductIds[1], XidDesc.wAlternateProductIds[2], XidDesc.wAlternateProductIds[3]);

									if (XidDesc.bDescriptorType == 0x42) {
										m_UcType = XidDesc.bType;
										m_UcSubType = XidDesc.bSubType;
									}
									else {
										EmuLog(LOG_LEVEL::INFO, "The xid descriptor for device %s reported an unexpected descriptor type, assuming a default subtype", m_Name);
									}
								}
								else {
									EmuLog(LOG_LEVEL::INFO, "Could not retrieve the xid descriptor for device %s, assuming a default subtype", m_Name);
								}
							}
						}
						else {
							EmuLog(LOG_LEVEL::INFO, "Rejected device %s because of unexpected number of endpoints, bNumEndpoints: %d",
								m_Name.c_str(), Setting.bNumEndpoints);
							m_Type = XBOX_INPUT_DEVICE::DEVICE_INVALID;
						}
					}
					else {
						EmuLog(LOG_LEVEL::INFO, "Rejected device %s because of unexpected number of alternative settings, num_altsetting: %d",
							m_Name.c_str(), Iface.num_altsetting);
						m_Type = XBOX_INPUT_DEVICE::DEVICE_INVALID;
					}
				}
				else {
					EmuLog(LOG_LEVEL::INFO, "Rejected device %s because of unexpected number of interfaces, bNumInterfaces: %d",
						m_Name.c_str(), ConfDesc->bNumInterfaces);
					m_Type = XBOX_INPUT_DEVICE::DEVICE_INVALID;
				}
				libusb_free_config_descriptor(ConfDesc);
			}
		}

		if (m_Type == XBOX_INPUT_DEVICE::DEVICE_INVALID) { libusb_close(m_hDev); }
	}

	LibusbDevice::~LibusbDevice()
	{
		if (m_Type != XBOX_INPUT_DEVICE::DEVICE_INVALID) {
			libusb_release_interface(m_hDev, m_IfaceNum);
			libusb_close(m_hDev);
		}
	}

	bool LibusbDevice::UpdateInput()
	{
		// Dummy, it should never be called. It's only here to override the pure function UpdateInput in InputDevice
		assert(0);
		return false;
	}

	bool LibusbDevice::ExecuteIo(void *Buffer, int Direction)
	{
		// NOTE: a SET_REPORT control transfer to the SBC doesn't seem to work, the parameters might not be appropriate for it... So, we use
		// the interrupt pipes for everything instead
		*static_cast<uint8_t *>(Buffer) = 0; // write bReportId
		if (Direction == DIRECTION_IN) {
			*(static_cast<uint8_t *>(Buffer) + 1) = m_BufferInSize; // write bLength
			if (libusb_interrupt_transfer(m_hDev, m_EndpointIn, static_cast<uint8_t *>(Buffer), m_BufferInSize, nullptr, m_IntervalIn) != 0) {
				return false;
			}
		}
		else {
			if (m_HasEndpointOut) {
				*(static_cast<uint8_t *>(Buffer) + 1) = m_BufferOutSize; // write bLength
				if (libusb_interrupt_transfer(m_hDev, m_EndpointOut, static_cast<uint8_t *>(Buffer), m_BufferOutSize, nullptr, m_IntervalOut) != 0) {
					return false;
				}
			}
		}

		return true;
	}

	std::string LibusbDevice::GetDeviceName() const
	{
		return m_Name;
	}

	std::string LibusbDevice::GetAPI() const
	{
		return "Libusb";
	}
}
