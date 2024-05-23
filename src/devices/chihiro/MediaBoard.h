// ******************************************************************
// *   src->devices->chihiro->MediaBoard.h
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
// *  (c) 2019 Luke Usher
// *
// *  All rights reserved
// *
// ******************************************************************

#ifndef MEDIABOARD_H
#define MEDIABOARD_H

#include <cstdint>
#include <string>

#define MB_CMD_DIMM_SIZE                    0x0001
#define MB_CMD_STATUS                       0x0100
    #define MB_STATUS_INIT              0
    #define MB_STATUS_CHECKING_NETWORK  1
    #define MB_STATUS_SYSTEM_DISC       2
    #define MB_STATUS_TESTING           3
    #define MB_STATUS_LOADING           4
    #define MB_STATUS_READY             5
    #define MB_STATUS_ERROR             6
#define MB_CMD_FIRMWARE_VERSION             0x0101
#define MB_CMD_SYSTEM_TYPE                  0x0102
    #define MB_SYSTEM_TYPE_DEVELOPER    0x8000
    #define MB_SYSTEM_TYPE_GDROM        0x0001
#define MB_CMD_SERIAL_NUMBER                0x0103
#define MB_CMD_HARDWARE_TEST                0x0301

#define MB_CHIHIRO_REGION_FLAG_JAPAN    0x2
#define MB_CHIHIRO_REGION_FLAG_USA      0x4
#define MB_CHIHIRO_REGION_FLAG_EXPORT   0x8

typedef struct {
    char magic[4];              // 0x00 (Always BTID)
    uint32_t unknown0[3];
    uint32_t unknown1[4];
    char mediaboardType[4];     // 0x20 (XBAM for Chihiro)
    uint32_t unknown2;
    uint16_t year;              // 0x28
    uint8_t month;              // 0x2A
    uint8_t day;                // 0x2B
    uint8_t videoMode;          // 0x2C unknown bitmask, resolutions + horizontal/vertical
    uint8_t unknown3;
    uint8_t type3Compatible;    // 0x2E (Type-3 compatibile titles have this set to 1)
    uint8_t unknown4;
    char gameId[8];             // 0x30
    uint32_t regionFlags;       // 0x38
    uint32_t unknown6[9];
    char manufacturer[0x20];    // 0x60
    char gameName[0x20];        // 0x80
    char gameExecutable[0x20];  // 0xA0
    char testExecutable[0x20];  // 0xC0
    char creditTypes[8][0x20];  // 0xE0
} chihiro_bootid;

class MediaBoard
{
public:
    void SetMountPath(std::string path);

	// LPC IO handlers
	uint32_t LpcRead(uint32_t addr, int size);
	void LpcWrite(uint32_t addr, uint32_t value, int size);

	// Mbcom partition handlers
	void ComRead(uint32_t offset, void* buffer, uint32_t length);
	void ComWrite(uint32_t offset, void* buffer, uint32_t length);
    chihiro_bootid &GetBootId();
private:
	uint8_t readBuffer[512];
	uint8_t writeBuffer[512];

    std::string m_MountPath;

    chihiro_bootid BootID;
};

#endif
