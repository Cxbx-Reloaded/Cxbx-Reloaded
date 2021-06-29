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

#define LOG_PREFIX CXBXR_MODULE::RINP

#include "RawDevice.h"
#include "Logging.h"
#include <array>

// NOTE: we don't implement host input devices controlled by rawinput, we only use the api to detect device changes for xinput

namespace RawInput
{
    int InitStatus = NOT_INIT;
    bool IgnoreHotplug = true;

	void Init(std::mutex &Mtx, bool is_gui, HWND hwnd)
	{
        std::unique_lock<std::mutex> lck(Mtx);

        if (is_gui) {
            // We don't need to monitor xinput device changes from the gui, because there we have the refresh button to detect changes
            InitStatus = INIT_SUCCESS;
            return;
        }

        std::array<RAWINPUTDEVICE, 3> devices;
        // joystick devices
        devices[0].usUsagePage = 0x01;
        devices[0].usUsage = 0x04;
        devices[0].dwFlags = RIDEV_DEVNOTIFY | RIDEV_INPUTSINK;
        devices[0].hwndTarget = hwnd;
        // gamepad devices
        devices[1].usUsagePage = 0x01;
        devices[1].usUsage = 0x05;
        devices[1].dwFlags = RIDEV_DEVNOTIFY | RIDEV_INPUTSINK;
        devices[1].hwndTarget = hwnd;
        // multi axis controller devices
        devices[2].usUsagePage = 0x01;
        devices[2].usUsage = 0x08;
        devices[2].dwFlags = RIDEV_DEVNOTIFY | RIDEV_INPUTSINK;
        devices[2].hwndTarget = hwnd;

        if (!RegisterRawInputDevices(devices.data(), static_cast<UINT>(devices.size()),
            static_cast<UINT>(sizeof(decltype(devices)::value_type))))
        {
            EmuLog(LOG_LEVEL::ERROR2, "RegisterRawInputDevices failed: %i", GetLastError());
            InitStatus = INIT_ERROR;
            return;
        }

        InitStatus = INIT_SUCCESS;
	}

    void DeInit()
    {
        InitStatus = NOT_INIT;
        IgnoreHotplug = true;
    }
}
