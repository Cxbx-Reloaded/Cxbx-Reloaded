// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->InputConfig.h
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
#ifndef INPUTCONFIG_H
#define INPUTCONFIG_H

#include <string.h>

// ******************************************************************
// * Input Device Component IDs
// ******************************************************************
enum InputDeviceComponent
{
    // ******************************************************************
    // * Analog Axis
    // ******************************************************************
    INPUT_DEVICE_COMPONENT_LTHUMBX = 0,
    INPUT_DEVICE_COMPONENT_LTHUMBY,
    INPUT_DEVICE_COMPONENT_RTHUMBX,
    INPUT_DEVICE_COMPONENT_RTHUMBY,
    // ******************************************************************
    // * Analog Buttons
    // ******************************************************************
    INPUT_DEVICE_COMPONENT_X,
    INPUT_DEVICE_COMPONENT_Y,
    INPUT_DEVICE_COMPONENT_A,
    INPUT_DEVICE_COMPONENT_B,
    INPUT_DEVICE_COMPONENT_WHITE,
    INPUT_DEVICE_COMPONENT_BLACK,
    INPUT_DEVICE_COMPONENT_LTRIGGER,
    INPUT_DEVICE_COMPONENT_RTRIGGER,
    // ******************************************************************
    // * Digital Buttons
    // ******************************************************************
    INPUT_DEVICE_COMPONENT_DPADUP,
    INPUT_DEVICE_COMPONENT_DPADDOWN,
    INPUT_DEVICE_COMPONENT_DPADLEFT,
    INPUT_DEVICE_COMPONENT_DPADRIGHT,
    INPUT_DEVICE_COMPONENT_BACK,
    INPUT_DEVICE_COMPONENT_START,
    INPUT_DEVICE_COMPONENT_LTHUMB,
    INPUT_DEVICE_COMPONENT_RTHUMB,
    // ******************************************************************
    // * Total number of components
    // ******************************************************************
    INPUT_DEVICE_COMPONENT_COUNT
};

// ******************************************************************
// * Input Device Title Lookup Table
// ******************************************************************
extern const char *g_InputDeviceTitle[INPUT_DEVICE_COMPONENT_COUNT];

// ******************************************************************
// * InputMapping Flags
// ******************************************************************
#define INPUT_MAPPING_JOYSTICK      (1 << 0)
#define INPUT_MAPPING_KEYBOARD      (1 << 1)
#define INPUT_MAPPING_MOUSE         (1 << 2)
#define INPUT_MAPPING_AXIS_POSITIVE (1 << 3)
#define INPUT_MAPPING_AXIS_NEGATIVE (1 << 4)
#define INPUT_MAPPING_MOUSE_CLICK   (1 << 5)
#define INPUT_MAPPING_MOUSE_LX      (1 << 6)
#define INPUT_MAPPING_MOUSE_LY      (1 << 7)
#define INPUT_MAPPING_MOUSE_LZ      (1 << 8)

// ******************************************************************
// * Maximum number of input devices is limited by # of components
// ******************************************************************
#define MAX_INPUT_DEVICES INPUT_DEVICE_COMPONENT_COUNT

// ******************************************************************
// * Input mapping information
// ******************************************************************
struct InputMapping
{
    int dwDevice;   // Offset into m_Devices
    int dwInfo;     // Extended information, depending on dwFlags
    int dwFlags;    // Flags explaining the data format
};

// ******************************************************************
// * Input Configuration
// ******************************************************************
class InputConfig
{
    public:
        // ******************************************************************
        // * Init
        // ******************************************************************
        void Init();

        // ******************************************************************
        // * Mapping Accessors
        // ******************************************************************
        void Map(InputDeviceComponent idc, const char *DeviceName, int dwInfo, int dwFlags);
        void Get(InputDeviceComponent idc, int *dwDevice, int *dwInfo, int *dwFlags);

        // ******************************************************************
        // * To enumerate and set device names
        // ******************************************************************
        const char *GetDeviceName(int offset) { return (const char*)m_DeviceName[offset]; }
        void SetDeviceName(int offset, const char *device_name) { strncpy(m_DeviceName[offset], device_name, 259); }

    private:
        // ******************************************************************
        // * Find the look-up value for a DeviceName (creates if needed)
        // ******************************************************************
        int Insert(const char *DeviceName);

        // ******************************************************************
        // * Devices used by one or more input mappings
        // ******************************************************************
        char m_DeviceName[MAX_INPUT_DEVICES][260];

        // ******************************************************************
        // * Input Device Components
        // ******************************************************************
        InputMapping m_InputMapping[INPUT_DEVICE_COMPONENT_COUNT];
};

#endif
