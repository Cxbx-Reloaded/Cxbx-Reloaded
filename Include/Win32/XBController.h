// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->Cxbx->XBController.h
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
#ifndef XBCONTROLLER_H
#define XBCONTROLLER_H

#include "Cxbx.h"
#include "Error.h"
#include "Mutex.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace xapi
{
    #include "EmuXapi.h"
};

// ******************************************************************
// * Xbox Controller Object IDs
// ******************************************************************
enum XBCtrlObject
{
    // ******************************************************************
    // * Analog Axis
    // ******************************************************************
    XBCTRL_OBJECT_LTHUMBPOSX = 0,
    XBCTRL_OBJECT_LTHUMBNEGX,
    XBCTRL_OBJECT_LTHUMBPOSY,
    XBCTRL_OBJECT_LTHUMBNEGY,
    XBCTRL_OBJECT_RTHUMBPOSX,
    XBCTRL_OBJECT_RTHUMBNEGX,
    XBCTRL_OBJECT_RTHUMBPOSY,
    XBCTRL_OBJECT_RTHUMBNEGY,
    // ******************************************************************
    // * Analog Buttons
    // ******************************************************************
    XBCTRL_OBJECT_X,
    XBCTRL_OBJECT_Y,
    XBCTRL_OBJECT_A,
    XBCTRL_OBJECT_B,
    XBCTRL_OBJECT_WHITE,
    XBCTRL_OBJECT_BLACK,
    XBCTRL_OBJECT_LTRIGGER,
    XBCTRL_OBJECT_RTRIGGER,
    // ******************************************************************
    // * Digital Buttons
    // ******************************************************************
    XBCTRL_OBJECT_DPADUP,
    XBCTRL_OBJECT_DPADDOWN,
    XBCTRL_OBJECT_DPADLEFT,
    XBCTRL_OBJECT_DPADRIGHT,
    XBCTRL_OBJECT_BACK,
    XBCTRL_OBJECT_START,
    XBCTRL_OBJECT_LTHUMB,
    XBCTRL_OBJECT_RTHUMB,
    // ******************************************************************
    // * Total number of components
    // ******************************************************************
    XBCTRL_OBJECT_COUNT
};

// ******************************************************************
// * Maximum number of devices allowed
// ******************************************************************
#define XBCTRL_MAX_DEVICES XBCTRL_OBJECT_COUNT

// ******************************************************************
// * Xbox Controller Object Config
// ******************************************************************
struct XBCtrlObjectCfg
{
    int dwDevice;   // offset into m_Devices
    int dwInfo;     // extended information, depending on dwFlags
    int dwFlags;    // flags explaining the data format
};

// ******************************************************************
// * class: XBController
// ******************************************************************
class XBController : public Error, Mutex
{
    public:
        // ******************************************************************
        // * Initialization
        // ******************************************************************
        XBController();
       ~XBController() {};

        // ******************************************************************
        // * Registry Load/Save
        // ******************************************************************
        void Load(const char *szRegistryKey);
        void Save(const char *szRegistryKey);

        // ******************************************************************
        // * Configuration
        // ******************************************************************
        void ConfigBegin(HWND hwnd, XBCtrlObject object);
        bool ConfigPoll(char *szStatus);  // true if polling detected a change
        void ConfigEnd();

        // ******************************************************************
        // * Listening
        // ******************************************************************
        void ListeningBegin(HWND hwnd);
        void ListeningPoll(xapi::XINPUT_STATE &Controller);
        void ListeningEnd();

        // ******************************************************************
        // * Input Device Name Lookup Table
        // ******************************************************************
        static const char *m_DeviceNameLookup[XBCTRL_OBJECT_COUNT];

    private:
        // ******************************************************************
        // * Object Mapping
        // ******************************************************************
        void Map(XBCtrlObject object, const char *szDeviceName, int dwInfo, int dwFlags);

        // ******************************************************************
        // * Find the look-up value for a DeviceName (creating if needed)
        // ******************************************************************
        int Insert(const char *szDeviceName);

        // ******************************************************************
        // * Controller and Objects Enumeration
        // ******************************************************************
        BOOL EnumGameCtrlCallback(LPCDIDEVICEINSTANCE lpddi);
        BOOL EnumObjectsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi);

        // ******************************************************************
        // * Wrapper Function for Enumeration
        // ******************************************************************
        friend BOOL CALLBACK WrapEnumGameCtrlCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);
        friend BOOL CALLBACK WrapEnumObjectsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef);

        // ******************************************************************
        // * Device Names
        // ******************************************************************
        char m_DeviceName[XBCTRL_MAX_DEVICES][260];

        // ******************************************************************
        // * Object Configuration
        // ******************************************************************
        XBCtrlObjectCfg m_ObjectConfig[XBCTRL_OBJECT_COUNT];

        // ******************************************************************
        // * DirectInput
        // ******************************************************************
        LPDIRECTINPUT8 m_pDirectInput8;

        // ******************************************************************
        // * DirectInput Devices
        // ******************************************************************
        struct InputDevice
        {
            LPDIRECTINPUTDEVICE8 m_Device;
            DWORD                m_Flags;
        }
        m_InputDevice[XBCTRL_MAX_DEVICES];

        // ******************************************************************
        // * Config State Variables
        // ******************************************************************
        LONG LastMouse_lX, LastMouse_lY, LastMouse_lZ;
        XBCtrlObject CurConfigObject;

        // ******************************************************************
        // * Etc State Variables
        // ******************************************************************
        int m_dwInputDeviceCount;
        int m_dwCurObject;

};

// ******************************************************************
// * Device Flags
// ******************************************************************
#define DEVICE_FLAG_JOYSTICK      (1 << 0)
#define DEVICE_FLAG_KEYBOARD      (1 << 1)
#define DEVICE_FLAG_MOUSE         (1 << 2)
#define DEVICE_FLAG_AXIS          (1 << 3)
#define DEVICE_FLAG_POSITIVE      (1 << 4)
#define DEVICE_FLAG_NEGATIVE      (1 << 5)
#define DEVICE_FLAG_MOUSE_CLICK   (1 << 6)
#define DEVICE_FLAG_MOUSE_LX      (1 << 7)
#define DEVICE_FLAG_MOUSE_LY      (1 << 8)
#define DEVICE_FLAG_MOUSE_LZ      (1 << 9)

// ******************************************************************
// * Detection Sensitivity
// ******************************************************************
#define DETECT_SENSITIVITY_JOYSTICK 25000
#define DETECT_SENSITIVITY_BUTTON   0
#define DETECT_SENSITIVITY_MOUSE    10
#define DETECT_SENSITIVITY_POV      50000

#endif
