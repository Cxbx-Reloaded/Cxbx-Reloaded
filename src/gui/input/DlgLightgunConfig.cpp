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
#define AUTORELOAD_DELAY 5000

#include "Windows.h"
#include "gui\resource\ResCxbx.h"
#include "input\InputWindow.h"
#include "gui\DlgInputConfig.h"
#include "common\Logging.h"


static constexpr std::array<std::array<const char *, LIGHTGUN_NUM_BUTTONS>, 2> button_lightgun_default = { {
	{ "Pad N", "Pad S", "Pad W", "Pad E", "Start", "Back", "Button A", "Button B", "Button X", "Button Y", "Left X+", "Left X-",
	"Left Y+", "Left Y-", "Shoulder L", "Shoulder R", "Thumb R" },
	{ "UP", "DOWN", "LEFT", "RIGHT", "RETURN", "SPACE", "Click 0", "Click 1", "W", "E", "Cursor X+", "Cursor X-", "Cursor Y+", "Cursor Y-", "S", "D", "C"}
} };

static LightgunInputWindow *g_InputWindow = nullptr;

LightgunInputWindow::~LightgunInputWindow()
{
	g_Settings->m_input_port[m_port_num].SlotType[SLOT_TOP] = to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID);
	g_Settings->m_input_port[m_port_num].SlotType[SLOT_BOTTOM] = to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID);
}

void LightgunInputWindow::Initialize(HWND hwnd, int port_num, int dev_type)
{
	// Save window/device specific variables
	m_hwnd_window = hwnd;
	m_hwnd_device_list = GetDlgItem(m_hwnd_window, IDC_DEVICE_LIST);
	m_hwnd_profile_list = GetDlgItem(m_hwnd_window, IDC_PROFILE_NAME);
	m_hwnd_default = GetDlgItem(m_hwnd_window, IDC_DEFAULT);
	m_dev_type = dev_type;
	m_max_num_buttons = dev_num_buttons[dev_type];
	m_port_num = port_num;
	m_bHasChanges = false;
	m_bIsBinding = false;
	m_num_devices = 0;

	// Set window icon
	SetClassLong(m_hwnd_window, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_CXBX)));

	// Set window title
	std::string title = (GetInputDeviceName(m_dev_type) + " at port ");
	SendMessage(m_hwnd_window, WM_SETTEXT, 0,
		reinterpret_cast<LPARAM>((title + PortUserFormat(std::to_string(m_port_num))).c_str()));

	// Set the maximum profile name lenght the user can enter in the profile combobox
	SendMessage(m_hwnd_profile_list, CB_LIMITTEXT, 49, 0);

	// construct emu device
	m_DeviceConfig = new EmuDevice(m_dev_type, m_hwnd_window, this);

	// Enumerate devices
	UpdateDeviceList();

	// Load currently saved profile for this port/device type
	LoadDefaultProfile();

	// Load currently selected host device
	UpdateCurrentDevice();

	// Install the subclass for the profile combobox
	SetWindowSubclass(GetWindow(m_hwnd_profile_list, GW_CHILD), ProfileNameSubclassProc, 0, 0);
}

void LightgunInputWindow::ClearBindings()
{
	m_DeviceConfig->ClearButtons();
	m_bHasChanges = true;
}

void LightgunInputWindow::BindDefault()
{
	int api = EnableDefaultButton();
	if (api != -1) {
		m_DeviceConfig->BindDefault<LIGHTGUN_NUM_BUTTONS>(button_lightgun_default[api]);
		m_bHasChanges = true;
	}
}

void LightgunInputWindow::BindButton(int ControlID)
{
	InputWindow::BindButton(ControlID);
	SwapMoCursorAxis(m_DeviceConfig->FindButtonById(ControlID));
}

INT_PTR CALLBACK DlgLightgunConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
		assert(dev_type == to_underlying(XBOX_INPUT_DEVICE::LIGHTGUN));

		g_InputWindow = new LightgunInputWindow;
		g_InputWindow->Initialize(hWndDlg, port_num, dev_type);
	}
	break;

	case WM_CLOSE:
	{
		if (g_InputWindow->IsProfileSaved()) {
			delete g_InputWindow;
			g_InputWindow = nullptr;
			EndDialog(hWndDlg, wParam);
		}
	}
	break;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_DEVICE_LIST: {
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				g_InputWindow->UpdateCurrentDevice();
			}
		}
		break;

		case IDC_PROFILE_NAME: {
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				char name[50];
				HWND hwnd = GetDlgItem(hWndDlg, IDC_PROFILE_NAME);
				LRESULT str_idx = SendMessage(hwnd, CB_GETCURSEL, 0, 0);
				if (str_idx != CB_ERR) {
					SendMessage(hwnd, CB_GETLBTEXT, str_idx, reinterpret_cast<LPARAM>(name));
					g_InputWindow->UpdateProfile(std::string(name), PROFILE_LOAD);
				}
			}
		}
		break;

		case IDC_PROFILE_SAVE:
		case IDC_PROFILE_DELETE: {
			if (HIWORD(wParam) == BN_CLICKED) {
				char name[50];
				SendMessage(GetDlgItem(hWndDlg, IDC_PROFILE_NAME), WM_GETTEXT,
					sizeof(name), reinterpret_cast<LPARAM>(name));
				g_InputWindow->UpdateProfile(std::string(name), (LOWORD(wParam) == IDC_PROFILE_SAVE) ? PROFILE_SAVE : PROFILE_DELETE);
			}
		}
		break;

		case IDC_DEFAULT: {
			if (HIWORD(wParam) == BN_CLICKED) {
				g_InputWindow->BindDefault();
			}
		}
		break;

		case IDC_CLEAR: {
			if (HIWORD(wParam) == BN_CLICKED) {
				if (PopupQuestionEx(hWndDlg, LOG_LEVEL::WARNING, PopupButtons::YesNo, PopupReturn::No, "Are you sure you want to remove all button bindings?") == PopupReturn::Yes) {
					g_InputWindow->ClearBindings();
				}
			}
		}
		break;

		case IDC_REFRESH_DEVICES: {
			if (HIWORD(wParam) == BN_CLICKED) {
				g_InputWindow->UpdateDeviceList();
			}
		}
		break;

		case IDC_LG_STICK_UP:
		case IDC_LG_STICK_DOWN:
		case IDC_LG_STICK_LEFT:
		case IDC_LG_STICK_RIGHT:
		case IDC_LG_START:
		case IDC_LG_SEBA:
		case IDC_LG_TRIGGER:
		case IDC_LG_GRIP:
		case IDC_LG_A:
		case IDC_LG_B:
		case IDC_LG_AIM_POSX:
		case IDC_LG_AIM_NEGX:
		case IDC_LG_AIM_POSY:
		case IDC_LG_AIM_NEGY:
		case IDC_TURBO_LEFT:
		case IDC_TURBO_RIGHT:
		case IDC_LASER: {
			if (HIWORD(wParam) == BN_CLICKED) {
				g_InputWindow->BindButton(LOWORD(wParam));
			}
		}
		break;
		}
	}
	break;
	}

	return FALSE;
}
