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
// *  (c) 2019 ergo720
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


static constexpr std::array<std::array<const char *, XBOX_CTRL_NUM_BUTTONS>, 2> button_xbox_ctrl_default = { {
	{ "Pad N", "Pad S", "Pad W", "Pad E", "Start", "Back", "Thumb L", "Thumb R", "Button A", "Button B", "Button X", "Button Y", "Shoulder R", "Shoulder L", "Trigger L",
	"Trigger R", "Left X+", "Left X-", "Left Y+", "Left Y-", "Right X+", "Right X-", "Right Y+", "Right Y-", "LeftRight" },
	{ "UP", "DOWN", "LEFT", "RIGHT", "RETURN", "SPACE", "B", "M", "S", "D", "W", "E", "C", "X", "Q", "R", "H", "F", "T", "G", "L", "J", "I", "K", "" }
} };

static DukeInputWindow *g_InputWindow = nullptr;

void DukeInputWindow::Initialize(HWND hwnd, int port_num, int dev_type)
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

	// Set window icon
	SetClassLong(m_hwnd_window, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_CXBX)));

	// Set window title
	std::string title;
	switch (m_dev_type)
	{
	case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE): {
		title += "Xbox Controller Duke at port ";
	}
	break;

	case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_S): {
		title += "Xbox Controller S at port ";
	}
	break;

	}
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

	// Load rumble binding
	char rumble[30];
	m_DeviceConfig->FindButtonByIndex(m_max_num_buttons - 1)->GetText(rumble, sizeof(rumble));
	m_rumble = rumble;

	// Install the subclass for the profile combobox
	SetWindowSubclass(GetWindow(m_hwnd_profile_list, GW_CHILD), ProfileNameSubclassProc, 0, 0);
}

void DukeInputWindow::InitRumble(HWND hwnd)
{
	m_hwnd_rumble = hwnd;
	m_hwnd_rumble_list = GetDlgItem(m_hwnd_rumble, IDC_RUMBLE_LIST);
	SendMessage(m_hwnd_rumble_list, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(""));
	auto dev = g_InputDeviceManager.FindDevice(m_host_dev);
	if (dev != nullptr) {
		auto outputs = dev->GetOutputs();
		for (const auto out : outputs) {
			SendMessage(m_hwnd_rumble_list, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(out->GetName().c_str()));
		}
	}
	SendMessage(m_hwnd_rumble_list, CB_SETCURSEL, SendMessage(m_hwnd_rumble_list, CB_FINDSTRINGEXACT, 1,
		reinterpret_cast<LPARAM>(m_rumble.c_str())), 0);
}

void DukeInputWindow::UpdateRumble(int command)
{
	switch (command)
	{
	case RUMBLE_SET: {
		char rumble[30];
		SendMessage(m_hwnd_rumble_list, WM_GETTEXT, sizeof(rumble), reinterpret_cast<LPARAM>(rumble));
		m_rumble = rumble;
	}
	break;

	case RUMBLE_UPDATE: {
		m_DeviceConfig->FindButtonByIndex(m_max_num_buttons - 1)->UpdateText(m_rumble.c_str());
	}
	break;

	case RUMBLE_TEST: {
		DetectOutput(OUTPUT_TIMEOUT);
	}
	break;

	}
}

void DukeInputWindow::BindDefault()
{
	int api = EnableDefaultButton();
	if (api != -1) {
		m_DeviceConfig->BindDefault<XBOX_CTRL_NUM_BUTTONS>(button_xbox_ctrl_default[api]);
		m_bHasChanges = true;
	}
}

int DukeInputWindow::EnableDefaultButton()
{
	if (std::strncmp(m_host_dev.c_str(), "XInput", std::strlen("XInput")) == 0) {
		EnableWindow(m_hwnd_default, TRUE);
		return XINPUT_DEFAULT;
	}
	else if (std::strncmp(m_host_dev.c_str(), "DInput", std::strlen("DInput")) == 0) {
		EnableWindow(m_hwnd_default, TRUE);
		return DINPUT_DEFAULT;
	}
	else {
		EnableWindow(m_hwnd_default, FALSE);
		return -1;
	}
}

void DukeInputWindow::ClearBindings()
{
	m_DeviceConfig->ClearButtons();
	m_rumble = std::string();
	m_bHasChanges = true;
}

void DukeInputWindow::UpdateProfile(const std::string &name, int command)
{
	switch (command)
	{
	case PROFILE_LOAD: {
		LoadProfile(name);
	}
	break;

	case PROFILE_SAVE: {
		SaveProfile(name);
	}
	break;

	case PROFILE_DELETE: {
		DeleteProfile(name);
	}
	break;

	case RUMBLE_CLEAR: {
		m_rumble = std::string();
	}
	break;

	case BUTTON_CLEAR: {
		m_bHasChanges = true;
	}
	break;

	}
}

void DukeInputWindow::DetectOutput(int ms)
{
	if (m_rumble == std::string()) {
		return;
	}
	auto dev = g_InputDeviceManager.FindDevice(m_host_dev);
	if (dev != nullptr) {
		// Don't block the message processing loop
		std::thread([this, dev, ms]() {
			EnableWindow(m_hwnd_rumble, FALSE);
			SendMessage(m_hwnd_rumble_list, WM_SETTEXT, 0, reinterpret_cast<LPARAM>("..."));
			auto outputs = dev->GetOutputs();
			for (const auto out : outputs) {
				if (out->GetName() == m_rumble) {
					out->SetState(1.0, 1.0);
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(ms));
			for (const auto out : outputs) {
				if (out->GetName() == m_rumble) {
					out->SetState(0.0, 0.0);
				}
			}
			SendMessage(m_hwnd_rumble_list, WM_SETTEXT, 0, reinterpret_cast<LPARAM>("Test"));
			EnableWindow(m_hwnd_rumble, TRUE);
			}).detach();
	}
}

static INT_PTR CALLBACK DlgRumbleConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT_PTR CALLBACK DlgXidControllerConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
		assert(dev_type == to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE) ||
			dev_type == to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_S));

		g_InputWindow = new DukeInputWindow;
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

		case IDC_SET_X:
		case IDC_SET_Y:
		case IDC_SET_A:
		case IDC_SET_B:
		case IDC_SET_WHITE:
		case IDC_SET_BLACK:
		case IDC_SET_LTRIGGER:
		case IDC_SET_RTRIGGER:
		case IDC_SET_LTHUMB:
		case IDC_SET_RTHUMB:
		case IDC_SET_START:
		case IDC_SET_BACK:
		case IDC_SET_DPAD_LEFT:
		case IDC_SET_DPAD_RIGHT:
		case IDC_SET_DPAD_UP:
		case IDC_SET_DPAD_DOWN:
		case IDC_SET_LEFT_POSY:
		case IDC_SET_LEFT_NEGX:
		case IDC_SET_LEFT_NEGY:
		case IDC_SET_LEFT_POSX:
		case IDC_SET_RIGHT_POSY:
		case IDC_SET_RIGHT_NEGY:
		case IDC_SET_RIGHT_NEGX:
		case IDC_SET_RIGHT_POSX: {
			if (HIWORD(wParam) == BN_CLICKED) {
				g_InputWindow->BindButton(LOWORD(wParam));
			}
		}
		break;

		case IDC_SET_MOTOR: {
			if (HIWORD(wParam) == BN_CLICKED) {
				// Show rumble dialog box
				DialogBox(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_RUMBLE_CFG), hWndDlg, DlgRumbleConfigProc);
				g_InputWindow->UpdateRumble(RUMBLE_UPDATE);
			}
		}
		break;
		}
	}
	break;
	}
	return FALSE;
}

INT_PTR CALLBACK DlgRumbleConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		g_InputWindow->InitRumble(hWndDlg);
	}
	break;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_RUMBLE_LIST: {
			g_InputWindow->UpdateRumble(RUMBLE_SET);
		}
		break;

		case IDC_RUMBLE_TEST: {
			g_InputWindow->UpdateRumble(RUMBLE_TEST);
		}
		break;
		}
	}
	break;

	case WM_CLOSE:
	{
		EndDialog(hWndDlg, wParam);
	}
	break;
	}
	return FALSE;
}

LRESULT CALLBACK ProfileNameSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (uMsg)
	{
	// Remove the window subclass when this window is destroyed
	case WM_NCDESTROY: {
		RemoveWindowSubclass(hWnd, ProfileNameSubclassProc, uIdSubclass);
	}
	break;

	// Override the default system behaviour and process WM_CHAR messages ourselves
	case WM_GETDLGCODE: {
		if (lParam) {
			LPMSG lpmsg = reinterpret_cast<LPMSG>(lParam);
			if (lpmsg->message == WM_CHAR) {
				return DLGC_WANTCHARS;
			}
		}
	}
	break;

	case WM_CHAR:
	{
		// Make sure that we only allow printable ascii characters and some special keys to delete characters
		if (!((wParam >= ' ' && wParam <= '~')
			|| wParam == VK_CANCEL
			|| wParam == VK_CLEAR
			|| wParam == VK_DELETE
			|| wParam == VK_BACK))
		{
			return FALSE;
		}
	}
	break;

	// Don't allow pasting operations, they can be used to bypass the filtering done in WM_CHAR
	case WM_PASTE:
		return FALSE;
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}
