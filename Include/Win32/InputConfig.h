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
// * 32 input devices maximum
// ******************************************************************
#define MAX_INPUT_DEVICES 0x20

// ******************************************************************
// * Input mapping information
// ******************************************************************
struct InputMapping
{
    InputMapping() : dwDevice(-1), dwType(-1) {}

    int dwDevice;  // lookup into m_Devices
    int dwType;    // used to locate the data inside GetDeviceState's return data
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
       ~InputConfig();

        // ******************************************************************
        // * Mapping Accessors
        // ******************************************************************
        void MapLThumbX(const char *DeviceName, int dwType) { Map(m_LThumbX, DeviceName, dwType); }
        void MapLThumbY(const char *DeviceName, int dwType) { Map(m_LThumbY, DeviceName, dwType); }
        void MapRThumbX(const char *DeviceName, int dwType) { Map(m_RThumbX, DeviceName, dwType); }
        void MapRThumbY(const char *DeviceName, int dwType) { Map(m_RThumbY, DeviceName, dwType); }
        void MapX(const char *DeviceName, int dwType) { Map(m_X, DeviceName, dwType); }
        void MapY(const char *DeviceName, int dwType) { Map(m_Y, DeviceName, dwType); }
        void MapA(const char *DeviceName, int dwType) { Map(m_A, DeviceName, dwType); }
        void MapB(const char *DeviceName, int dwType) { Map(m_B, DeviceName, dwType); }
        void MapWhite(const char *DeviceName, int dwType) { Map(m_White, DeviceName, dwType); }
        void MapBlack(const char *DeviceName, int dwType) { Map(m_Black, DeviceName, dwType); }
        void MapLTrigger(const char *DeviceName, int dwType) { Map(m_LTrigger, DeviceName, dwType); }
        void MapRTrigger(const char *DeviceName, int dwType) { Map(m_RTrigger, DeviceName, dwType); }
        void MapDPadUp(const char *DeviceName, int dwType) { Map(m_DPadUp, DeviceName, dwType); }
        void MapDPadDown(const char *DeviceName, int dwType) { Map(m_DPadDown, DeviceName, dwType); }
        void MapDPadLeft(const char *DeviceName, int dwType) { Map(m_DPadLeft, DeviceName, dwType); }
        void MapDPadRight(const char *DeviceName, int dwType) { Map(m_DPadRight, DeviceName, dwType); }
        void MapBack(const char *DeviceName, int dwType) { Map(m_Back, DeviceName, dwType); }
        void MapStart(const char *DeviceName, int dwType) { Map(m_Start, DeviceName, dwType); }
        void MapLThumb(const char *DeviceName, int dwType) { Map(m_LThumb, DeviceName, dwType); }
        void MapRThumb(const char *DeviceName, int dwType) { Map(m_RThumb, DeviceName, dwType); }

    private:
        // ******************************************************************
        // * Map a given input control mapping
        // ******************************************************************
        void Map(InputMapping &IM, const char *DeviceName, int dwType);

        // ******************************************************************
        // * Find the look-up value for a DeviceName (creates if needed)
        // ******************************************************************
        int Insert(const char *DeviceName);

        // ******************************************************************
        // * Devices used by one or more input mappings
        // ******************************************************************
        char *m_DeviceName[MAX_INPUT_DEVICES];

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
