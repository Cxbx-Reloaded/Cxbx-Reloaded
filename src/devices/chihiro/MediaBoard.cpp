// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *   src->devices->chihiro->MediaBoard.cpp
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

#include "MediaBoard.h"
#include <cstdio>
#include <string>

#define _XBOXKRNL_DEFEXTRN_
#define LOG_PREFIX CXBXR_MODULE::JVS // TODO: XBAM


#include <core\kernel\exports\xboxkrnl.h>
#include "core\kernel\init\\CxbxKrnl.h"
#include "core\kernel\exports\EmuKrnl.h" // for HalSystemInterrupts

chihiro_bootid &MediaBoard::GetBootId()
{
    return BootID;
}

void MediaBoard::SetMountPath(std::string path)
{
    m_MountPath = path;

    // Load Boot.id from file
    FILE* bootidFile = fopen((path+"/boot.id").c_str(), "rb");
    if (bootidFile == nullptr) {
        CxbxrAbort("Could not open Chihiro boot.id");
    }
    fread(&BootID, 1, sizeof(chihiro_bootid), bootidFile);
    fclose(bootidFile);
}

uint32_t MediaBoard::LpcRead(uint32_t addr, int size)
{
    uint32_t result = 0;
    const char* desc = nullptr;

    switch (addr) {
    case 0x40E0: result = 1;          desc = "MB_ALIVE";           break;
    case 0x401E: result = 0x0317;     desc = "FW_VERSION";         break;
    case 0x4020: result = 0x00A0;     desc = "XBAM[0]";            break;
    case 0x4022: result = 0x4258;     desc = "XBAM[1] 'XB'";       break;
    case 0x4024: result = 0x4D41;     desc = "XBAM[2] 'MA'";       break;
    case 0x40F4: result = 0x0003;     desc = "UNK_40F4";           break;
    case 0x4026: result = temp_0x4026; desc = "HANDSHAKE";         break;
    case 0x40F0: result = 0x0000;     desc = "BOARD_TYPE(Type-1)"; break;
    case 0x4084: result = 0;          desc = "UNK_4084";           break;
    default:
        EmuLog(LOG_LEVEL::WARNING, "LpcRead: Unknown addr 0x%04X size=%d", addr, size);
        return 0;
    }

    //EmuLog(LOG_LEVEL::DEBUG, "LpcRead  [0x%04X] %-22s -> 0x%04X", addr, desc, result);
    return result;
}

void MediaBoard::LpcWrite(uint32_t addr, uint32_t value, int size)
{
    EmuLog(LOG_LEVEL::DEBUG, "LpcWrite [0x%04X] = 0x%08X", addr, value);

    if (addr == 0x40E1) {
        EmuLog(LOG_LEVEL::DEBUG, "LpcWrite [0x40E1] deassert IRQ10");
        HalSystemInterrupts[10].Assert(false);
        return;
    }

    if (addr == 0x4026) {
        temp_0x4026 = value;
        return;
    }

    EmuLog(LOG_LEVEL::WARNING, "LpcWrite: Unknown addr 0x%04X = 0x%08X", addr, value);
}

void MediaBoard::ComRead(uint32_t offset, void* buffer, uint32_t length)
{
    if (offset == 0x00D00000) {
        EmuLog(LOG_LEVEL::DEBUG, "ComRead  [0xD00000] length=%u (ignored)", length);
        return;
    }

    if (offset == 0x005FFCE9) {
        EmuLog(LOG_LEVEL::DEBUG, "ComRead  [0x5FFCE9] -> 0x10000 (DMA size/flag)");
        memset(buffer, 0, length);
        *(uint32_t*)buffer = 0x10000;
        return;
    }

    if (offset == 0x800000 || offset == 0x800200) {
        memcpy(buffer, buffer_800000, length);
        return;
    }

    if (offset == 0x900000 || offset == 0x900200) {
        memcpy(buffer, buffer_900000, length);
        return;
    }

    EmuLog(LOG_LEVEL::WARNING, "ComRead: Unknown offset 0x%08X length=%u", offset, length);
    memset(buffer, 0, length);
}

void MediaBoard::ComWrite(uint32_t offset, void* buffer, uint32_t length)
{
    if (offset == 0x005FFCE9) {
        EmuLog(LOG_LEVEL::DEBUG, "ComWrite [0x5FFCE9] assert IRQ10");
        HalSystemInterrupts[10].Assert(true);
        return;
    }

    // Network-related command channels
    if (offset == 0x800000) {
        EmuLog(LOG_LEVEL::DEBUG, "ComWrite [0x800000] net-cmd write length=%u", length);
        memcpy(buffer_800000, buffer, length);
        HalSystemInterrupts[10].Assert(true);
        return;
    }
    if (offset == 0x800200) {
        // Input: string of an IP address — respond with a fixed local IP
        EmuLog(LOG_LEVEL::DEBUG, "ComWrite [0x800200] net IP resolve request: \"%.*s\" -> 10.0.0.1", length, (char*)buffer);
        uint8_t* outputBuffer = (uint8_t*)buffer_800000;
        *(uint32_t*)outputBuffer = 167772161; // 10.0.0.1
        HalSystemInterrupts[10].Assert(true);
        return;
    }

    if (offset == 0x900000) {
        EmuLog(LOG_LEVEL::DEBUG, "ComWrite [0x900000] sys-cmd write length=%u", length);
        memcpy(buffer_900000, buffer, length);
        HalSystemInterrupts[10].Assert(true);
        return;
    }

    if (offset == 0x900200) {
        uint8_t tempBuffer[512];
        memcpy(tempBuffer, buffer, length);

        uint8_t* inputBuffer  = (uint8_t*)tempBuffer;
        uint8_t* outputBuffer = (uint8_t*)buffer_900000;

        uint16_t seq     = *(uint16_t*)&inputBuffer[0];
        uint32_t command = *(uint16_t*)&inputBuffer[2];
        if (command == 0)
            return;

        EmuLog(LOG_LEVEL::DEBUG, "ComWrite [0x900200] cmd=0x%04X seq=0x%04X length=%u", command, seq, length);

        bool success = true;
        switch (command) {
        case MB_CMD_DIMM_SIZE: { // 0x0001
            uint32_t sz = 1024 * ONE_MB;
            *(uint32_t*)&outputBuffer[4] = sz;
            EmuLog(LOG_LEVEL::DEBUG, "  MB_CMD_DIMM_SIZE -> %u MB", sz / ONE_MB);
            break;
        }
        case MB_CMD_STATUS: { // 0x0100
            *(uint32_t*)&outputBuffer[4] = MB_STATUS_READY;
            *(uint32_t*)&outputBuffer[8] = 100;
            EmuLog(LOG_LEVEL::DEBUG, "  MB_CMD_STATUS -> READY (100%%)");
            break;
        }
        case MB_CMD_FIRMWARE_VERSION: { // 0x0101
            *(uint32_t*)&outputBuffer[4] = 0x0317;
            EmuLog(LOG_LEVEL::DEBUG, "  MB_CMD_FIRMWARE_VERSION -> 0x0317");
            break;
        }
        case MB_CMD_SYSTEM_TYPE: { // 0x0102
            *(uint32_t*)&outputBuffer[4] = MB_SYSTEM_TYPE_DEVELOPER | MB_SYSTEM_TYPE_GDROM;
            EmuLog(LOG_LEVEL::DEBUG, "  MB_CMD_SYSTEM_TYPE -> DEVELOPER|GDROM");
            break;
        }
        case MB_CMD_SERIAL_NUMBER: { // 0x0103
            memcpy(&outputBuffer[4], "A89E-25A47354512", 16);
            EmuLog(LOG_LEVEL::DEBUG, "  MB_CMD_SERIAL_NUMBER -> A89E-25A47354512");
            break;
        }
        case 0x0104: {
            *(uint32_t*)&outputBuffer[4] = 0;
            EmuLog(LOG_LEVEL::DEBUG, "  CMD_0104 -> 0");
            break;
        }
        case MB_CMD_HARDWARE_TEST: { // 0x0301
            uint32_t testType       = *(uint32_t*)&inputBuffer[4];
            xbox::addr_xt resultPtr = *(uint32_t*)&inputBuffer[8];
            *(uint32_t*)&outputBuffer[4] = testType;
            EmuLog(LOG_LEVEL::DEBUG, "  MB_CMD_HARDWARE_TEST type=0x%X resultPtr=0x%08X -> TEST OK", testType, (uint32_t)resultPtr);
            memcpy((void*)resultPtr, "TEST OK", 8);
        } break;
        case 0x0204: {
            *(uint32_t*)&outputBuffer[4] = 0;
            EmuLog(LOG_LEVEL::DEBUG, "  CMD_0204 -> 0");
            break;
        }
        case 0x0415: {
            *(uint32_t*)&outputBuffer[4] = 167772161; // 10.0.0.1
            EmuLog(LOG_LEVEL::DEBUG, "  CMD_0415 (net IP?) -> 10.0.0.1");
            break;
        }
        case 0x0601: {
            *(uint32_t*)&outputBuffer[4] = 0;
            EmuLog(LOG_LEVEL::DEBUG, "  CMD_0601 (net init?) -> 0");
            break;
        }
        case 0x0602: {
            // This triggers 0x0605
            *(uint32_t*)&outputBuffer[4] = 0xFFFF;
            EmuLog(LOG_LEVEL::DEBUG, "  CMD_0602 -> 0xFFFF");
            break;
        }
        case 0x0605: {
            *(uint32_t*)&outputBuffer[4] = 0;
            EmuLog(LOG_LEVEL::DEBUG, "  CMD_0605 -> 0");
            break;
        }
        case 0x0606: {
            *(uint32_t*)&outputBuffer[4] = 0;
            EmuLog(LOG_LEVEL::DEBUG, "  CMD_0606 (net?) -> 0");
            break;
        }
        case 0x0607: {
            *(uint32_t*)&outputBuffer[4] = 0;
            *(uint32_t*)&outputBuffer[8] = 0;
            EmuLog(LOG_LEVEL::DEBUG, "  CMD_0607 -> 0,0");
            break;
        }
        case 0x0608: {
            *(uint32_t*)&outputBuffer[4] = 167772161; // 10.0.0.1
            EmuLog(LOG_LEVEL::DEBUG, "  CMD_0608 (net IP?) -> 10.0.0.1");
            break;
        }
        default: {
            EmuLog(LOG_LEVEL::WARNING, "  Unhandled MediaBoard command 0x%04X", command);
            break;
        }
        }

        // Echo sequence number; OR command with ACK bit
        *(uint16_t*)outputBuffer = *(uint16_t*)inputBuffer;
        if (success)
            *(uint16_t*)&outputBuffer[2] = (uint16_t)(command | 0x8000);
        else
            *(uint16_t*)&outputBuffer[2] = 0x8000;

        HalSystemInterrupts[10].Assert(true);
        return;
    }

    EmuLog(LOG_LEVEL::WARNING, "ComWrite: Unknown offset 0x%08X length=%u", offset, length);
}
