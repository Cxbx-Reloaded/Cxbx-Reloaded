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

// ******************************************************************
// * 32 Input Devices Maximum
// ******************************************************************
#define MAX_INPUT_DEVICES 0x20

// ******************************************************************
// * InputMapping Flags
// ******************************************************************
#define INPUT_MAPPING_JOYSTICK      (1 << 0)
#define INPUT_MAPPING_KEYBOARD      (1 << 1)
#define INPUT_MAPPING_MOUSE         (1 << 2)
#define INPUT_MAPPING_AXIS_POSITIVE (1 << 3)
#define INPUT_MAPPING_AXIS_NEGATIVE (1 << 4)
#define INPUT_MAPPING_MOUSE_CLICK   (1 << 5)

// ******************************************************************
// * Input mapping information
// ******************************************************************
struct InputMapping
{
    InputMapping() : dwDevice(-1), dwInfo(-1), dwFlags(0) {}

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
        // * Constructor
        // ******************************************************************
        InputConfig();

        // ******************************************************************
        // * Deconstructor
        // ******************************************************************
       ~InputConfig() {}

        // ******************************************************************
        // * Mapping Accessors
        // ******************************************************************
        void MapLThumbX(const char *DeviceName, int dwInfo, int dwFlags) { Map(m_LThumbX, DeviceName, dwInfo, dwFlags); }
        void MapLThumbY(const char *DeviceName, int dwInfo, int dwFlags) { Map(m_LThumbY, DeviceName, dwInfo, dwFlags); }
        void MapRThumbX(const char *DeviceName, int dwInfo, int dwFlags) { Map(m_RThumbX, DeviceName, dwInfo, dwFlags); }
        void MapRThumbY(const char *DeviceName, int dwInfo, int dwFlags) { Map(m_RThumbY, DeviceName, dwInfo, dwFlags); }
        void MapX(const char *DeviceName, int dwInfo, int dwFlags) { Map(m_X, DeviceName, dwInfo, dwFlags); }
        void MapY(const char *DeviceName, int dwInfo, int dwFlags) { Map(m_Y, DeviceName, dwInfo, dwFlags); }
        void MapA(const char *DeviceName, int dwInfo, int dwFlags) { Map(m_A, DeviceName, dwInfo, dwFlags); }
        void MapB(const char *DeviceName, int dwInfo, int dwFlags) { Map(m_B, DeviceName, dwInfo, dwFlags); }
        void MapWhite(const char *DeviceName, int dwInfo, int dwFlags) { Map(m_White, DeviceName, dwInfo, dwFlags); }
        void MapBlack(const char *DeviceName, int dwInfo, int dwFlags) { Map(m_Black, DeviceName, dwInfo, dwFlags); }
        void MapLTrigger(const char *DeviceName, int dwInfo, int dwFlags) { Map(m_LTrigger, DeviceName, dwInfo, dwFlags); }
        void MapRTrigger(const char *DeviceName, int dwInfo, int dwFlags) { Map(m_RTrigger, DeviceName, dwInfo, dwFlags); }
        void MapDPadUp(const char *DeviceName, int dwInfo, int dwFlags) { Map(m_DPadUp, DeviceName, dwInfo, dwFlags); }
        void MapDPadDown(const char *DeviceName, int dwInfo, int dwFlags) { Map(m_DPadDown, DeviceName, dwInfo, dwFlags); }
        void MapDPadLeft(const char *DeviceName, int dwInfo, int dwFlags) { Map(m_DPadLeft, DeviceName, dwInfo, dwFlags); }
        void MapDPadRight(const char *DeviceName, int dwInfo, int dwFlags) { Map(m_DPadRight, DeviceName, dwInfo, dwFlags); }
        void MapBack(const char *DeviceName, int dwInfo, int dwFlags) { Map(m_Back, DeviceName, dwInfo, dwFlags); }
        void MapStart(const char *DeviceName, int dwInfo, int dwFlags) { Map(m_Start, DeviceName, dwInfo, dwFlags); }
        void MapLThumb(const char *DeviceName, int dwInfo, int dwFlags) { Map(m_LThumb, DeviceName, dwInfo, dwFlags); }
        void MapRThumb(const char *DeviceName, int dwInfo, int dwFlags) { Map(m_RThumb, DeviceName, dwInfo, dwFlags); }

    private:
        // ******************************************************************
        // * Map a given input control mapping
        // ******************************************************************
        void Map(InputMapping &IM, const char *DeviceName, int dwInfo, int dwFlags);

        // ******************************************************************
        // * Find the look-up value for a DeviceName (creates if needed)
        // ******************************************************************
        int Insert(const char *DeviceName);

        // ******************************************************************
        // * Devices used by one or more input mappings
        // ******************************************************************
        char m_DeviceName[260][MAX_INPUT_DEVICES];

        // ******************************************************************
        // * Analog Axis
        // ******************************************************************
        InputMapping m_LThumbX, m_LThumbY;
        InputMapping m_RThumbX, m_RThumbY;

        // ******************************************************************
        // * Analog Buttons
        // ******************************************************************
        InputMapping m_X, m_Y;
        InputMapping m_A, m_B;
        InputMapping m_White, m_Black;
        InputMapping m_LTrigger, m_RTrigger;

        // ******************************************************************
        // * Digital Buttons
        // ******************************************************************
        InputMapping m_DPadUp, m_DPadDown, m_DPadLeft, m_DPadRight;
        InputMapping m_Back, m_Start;
        InputMapping m_LThumb, m_RThumb;
};

#endif
