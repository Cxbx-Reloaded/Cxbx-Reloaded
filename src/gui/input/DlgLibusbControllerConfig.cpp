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

#define LOG_PREFIX CXBXR_MODULE::GUI

#include "Windows.h"
#include "gui\resource\ResCxbx.h"
#include "input\InputWindow.h"
#include "common\input\LibusbDevice.h"
#include <chrono>


static LibusbInputWindow *g_InputWindow = nullptr;

LibusbInputWindow::~LibusbInputWindow()
{
	g_Settings->m_input_port[m_port_num].DeviceName = m_host_dev;
	g_Settings->m_input_port[m_port_num].ProfileName = "";
	g_Settings->m_input_port[m_port_num].SlotType[SLOT_TOP] = to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID);
	g_Settings->m_input_port[m_port_num].SlotType[SLOT_BOTTOM] = to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID);
}

void LibusbInputWindow::Initialize(HWND hwnd, int port_num, int dev_type)
{
	// Save window/device specific variables
	m_hwnd_window = hwnd;
	m_hwnd_device_list = GetDlgItem(m_hwnd_window, IDC_LIBUSB_LIST);
	m_hwnd_device_test = GetDlgItem(m_hwnd_window, IDC_LIBUSB_TEST);
	m_dev_type = dev_type;
	m_port_num = port_num;
	m_bIsBinding = false;
	m_num_devices = 0;
	m_DeviceConfig = nullptr;

	// Set window icon
	SetClassLong(m_hwnd_window, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_CXBX)));

	// Set window title
	std::string title = (GetInputDeviceName(m_dev_type) + " at port ");
	SendMessage(m_hwnd_window, WM_SETTEXT, 0,
		reinterpret_cast<LPARAM>((title + PortUserFormat(std::to_string(m_port_num))).c_str()));

	// Enumerate devices
	UpdateDeviceList();
}

void LibusbInputWindow::ClearBindings()
{
	// There are no profiles for libusb devices, so this is a nop
}

int LibusbInputWindow::EnableDefaultButton()
{
	// The libusb window does not have a default button, so we return a dummy value here
	return -1;
}

void LibusbInputWindow::UpdateDeviceList()
{
	g_InputDeviceManager.RefreshDevices();

	// Populate device list
	for (int i = 0; i < m_num_devices; i++) {
		SendMessage(m_hwnd_device_list, CB_DELETESTRING, 0, 0);
	}

	// Add only libusb devices and only those that match the type we are configuring
	std::vector<std::string> dev_list = g_InputDeviceManager.GetDeviceList([this](const auto &Device) {
		if (Device->IsLibusb()) {
			if (to_underlying(dynamic_cast<const Libusb::LibusbDevice *>(Device)->GetLibusbType()) == m_dev_type) {
				return true;
			}
		}
		return false;
		});
	for (const auto &str : dev_list) {
		SendMessage(m_hwnd_device_list, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(str.c_str()));
		++m_num_devices;
	}
	if (m_num_devices) {
		SendMessage(m_hwnd_device_list, CB_SETCURSEL, 0, 0);
	}

	UpdateCurrentDevice();
}

void LibusbInputWindow::TestInput()
{
	using namespace std::chrono;

	// Check if binding thread is still active
	if (m_bIsBinding) {
		return;
	}

	auto dev = g_InputDeviceManager.FindDevice(m_host_dev);
	if (dev != nullptr) {
		m_bIsBinding = true;

		// Don't block the message processing loop
		std::thread([this, dev]() {
			auto LibusbDev = dynamic_cast<Libusb::LibusbDevice *>(dev.get());
			EnableWindow(m_hwnd_window, FALSE);
			SendMessage(m_hwnd_device_test, WM_SETTEXT, 0, reinterpret_cast<LPARAM>("..."));
			auto now = system_clock::now();
			auto timeout = now + milliseconds(INPUT_TIMEOUT);
			InputBuff buffer[2];
			std::memset(buffer, 0, 2 * sizeof(InputBuff));
			bool detect = false;
			while (now <= timeout) {
				LibusbDev->ExecuteIo(buffer, DIRECTION_IN);
				if (std::memcmp(reinterpret_cast<uint8_t *>(buffer) + XID_PACKET_HEADER,
					reinterpret_cast<uint8_t *>(&buffer[1]) + XID_PACKET_HEADER,
					sizeof(InputBuff) - XID_PACKET_HEADER)) {
					detect = true;
					break;
				}
				std::this_thread::sleep_for(milliseconds(10));
				now += milliseconds(10);
			}
			if (detect) {
				SendMessage(m_hwnd_device_test, WM_SETTEXT, 0, reinterpret_cast<LPARAM>("Ok!"));
			}
			else {
				SendMessage(m_hwnd_device_test, WM_SETTEXT, 0, reinterpret_cast<LPARAM>("Fail!"));
			}
			std::this_thread::sleep_for(milliseconds(2000));
			SendMessage(m_hwnd_device_test, WM_SETTEXT, 0, reinterpret_cast<LPARAM>("Test"));
			EnableWindow(m_hwnd_window, TRUE);
			m_bIsBinding = false;
			}).detach();
	}
}

INT_PTR CALLBACK DlgLibUsbControllerConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		int port_num = lParam & 0xFF;
		int dev_type = (lParam & 0xFF00) >> 8;

		// Ensure that port_num is a valid xbox port
		assert(port_num >= PORT_1 && port_num <= PORT_4);

		// Ensure that the controller type is valid
		assert(dev_type == to_underlying(XBOX_INPUT_DEVICE::HW_XBOX_CONTROLLER) ||
			dev_type == to_underlying(XBOX_INPUT_DEVICE::HW_STEEL_BATTALION_CONTROLLER));

		g_InputWindow = new LibusbInputWindow;
		g_InputWindow->Initialize(hWndDlg, port_num, dev_type);
	}
	break;

	case WM_CLOSE:
	{
		delete g_InputWindow;
		g_InputWindow = nullptr;
		EndDialog(hWndDlg, wParam);
	}
	break;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_LIBUSB_LIST:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				g_InputWindow->UpdateCurrentDevice();
			}
			break;

		case IDC_REFRESH_DEVICES:
			if (HIWORD(wParam) == BN_CLICKED) {
				g_InputWindow->UpdateDeviceList();
			}
			break;

		case IDC_LIBUSB_TEST:
			if (HIWORD(wParam) == BN_CLICKED) {
				g_InputWindow->TestInput();
			}
			break;
		}
	}
	break;
	}

	return FALSE;
}
