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
#include "gui/resource/ResCxbx.h"
#include "input\InputWindow.h"
#include "gui\DlgInputConfig.h"
#include "common/Logging.h"


static SbcInputWindow *g_InputWindow = nullptr;

void SbcInputWindow::Initialize(HWND hwnd, int port_num, int dev_type)
{
	// Save window/device specific variables
	m_hwnd_window = hwnd;
	m_hwnd_device_list = GetDlgItem(m_hwnd_window, IDC_DEVICE_LIST);
	m_hwnd_profile_list = GetDlgItem(m_hwnd_window, IDC_PROFILE_NAME);
	m_dev_type = dev_type;
	m_max_num_buttons = dev_num_buttons[dev_type];
	m_port_num = port_num;
	m_bHasChanges = false;
	m_bIsBinding = false;

	// Set window icon
	SetClassLong(m_hwnd_window, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_CXBX)));

	// Set window title
	std::string title("Steel Battalion Controller at port ");
	SendMessage(m_hwnd_window, WM_SETTEXT, 0,
		reinterpret_cast<LPARAM>((title + std::to_string(PORT_INC(m_port_num))).c_str()));

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

void SbcInputWindow::ClearBindings()
{
	m_DeviceConfig->ClearButtons();
	m_bHasChanges = true;
}

int SbcInputWindow::EnableDefaultButton()
{
	// The SBC window does not have a default button, so we return a dummy value here
	return -1;
}

INT_PTR CALLBACK DlgSBControllerConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
		assert(dev_type == to_underlying(XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER));

		g_InputWindow = new SbcInputWindow;
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

		case IDC_BTN_MAIN_WEAPON:
		case IDC_BTN_SUB_WEAPON:
		case IDC_BTN_LOCK_ON:
		case IDC_BTN_EJECT:
		case IDC_BTN_COCKPIT_HATCH:
		case IDC_BTN_IGNITION:
		case IDC_BTN_START:
		case IDC_BTN_OPEN_CLOSE:
		case IDC_BTN_MAP_ZOOM_IN_OUT:
		case IDC_BTN_MODE_SELECT:
		case IDC_BTN_SUB_MONITOR_MODE_SELECT:
		case IDC_BTN_ZOOM_IN:
		case IDC_BTN_ZOOM_OUT:
		case IDC_BTN_FSS:
		case IDC_BTN_MANIPULATOR:
		case IDC_BTN_LINE_COLOR_CHANGE:
		case IDC_BTN_WASHING:
		case IDC_BTN_EXTINGUISHER:
		case IDC_BTN_CHAFF:
		case IDC_BTN_TANK_DETACH:
		case IDC_BTN_OVERRIDE:
		case IDC_BTN_NIGHT_SCOPE:
		case IDC_BTN_FUNC1:
		case IDC_BTN_FUNC2:
		case IDC_BTN_FUNC3:
		case IDC_BTN_MAIN_WEAPON_CONTROL:
		case IDC_BTN_SUB_WEAPON_CONTROL:
		case IDC_BTN_MAGAZINE_CHANGE:
		case IDC_BTN_COM1:
		case IDC_BTN_COM2:
		case IDC_BTN_COM3:
		case IDC_BTN_COM4:
		case IDC_BTN_COM5:
		case IDC_BTN_SIGHT_CHANGE:
		case IDC_FILT_CONTROL_SYSTEM:
		case IDC_OXYGEN_SUPPLY_SYSTEM:
		case IDC_FUEL_FLOW_RATE:
		case IDC_BUFFER_MATERIAL:
		case IDC_VT_LOCATION_MEASUREMENT:
		case IDC_AIMING_POSX:
		case IDC_AIMING_NEGX:
		case IDC_AIMING_POSY:
		case IDC_AIMING_NEGY:
		case IDC_LEVER_LEFT:
		case IDC_LEVER_RIGHT:
		case IDC_SIGHT_CHANGE_POSX:
		case IDC_SIGHT_CHANGE_NEGX:
		case IDC_SIGHT_CHANGE_POSY:
		case IDC_SIGHT_CHANGE_NEGY:
		case IDC_BTN_LEFT_PEDAL:
		case IDC_BTN_MIDDLE_PEDAL:
		case IDC_BTN_RIGHT_PEDAL:
		case IDC_RADIO_TD_UP:
		case IDC_RADIO_TD_DOWN:
		case IDC_GEAR_UP:
		case IDC_GEAR_DOWN: {
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
