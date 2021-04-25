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
	switch (addr) {
		case 0x401E: return 0x0317;     // Firmware Version Number
		case 0x4020: return 0x00A0;     // XBAM String (SEGABOOT reports Media Board is not present if these values change)
		case 0x4022: return 0x4258;     // Continued
		case 0x4024: return 0x4D41;     // Continued
		case 0x40F0: return 0x0000;     // Media Board Type (Type-1 vs Type-3), 0x0000 = Type-1, 0x0100 = Type-3
		case 0x40F4: return 0x03;       // 1GB
	}
	
	printf("MediaBoard::LpcRead: Unknown Addr %08X\n", addr);
	return 0;
}

void MediaBoard::LpcWrite(uint32_t addr, uint32_t value, int size)
{
	switch (addr) {
		case 0x40E1: HalSystemInterrupts[10].Assert(false); break;
		default:
			printf("MediaBoard::LpcWrite: Unknown Addr %08X = %08X\n", addr, value);
			break;
	}	
}

void MediaBoard::ComRead(uint32_t offset, void* buffer, uint32_t length)
{
	// Copy the current read buffer to the output
	memcpy(buffer, readBuffer, 0x20);
}

void MediaBoard::ComWrite(uint32_t offset, void* buffer, uint32_t length)
{
    if (offset == 0x900000) { // Some kind of reset?
        memcpy(readBuffer, buffer, 0x20);
        return;
    } else if (offset == 0x900200) { // Command Sector
        // Copy the written data to our internal, so we don't trash the original data
        memcpy(writeBuffer, buffer, 0x20);

        // Create accessor pointers 
        auto inputBuffer16 = (uint16_t*)writeBuffer;
        auto inputBuffer32 = (uint32_t*)writeBuffer;
        auto outputBuffer16 = (uint16_t*)readBuffer;
        auto outputBuffer32 = (uint32_t*)readBuffer;

        // If no command word was specified, do nothing
        if (inputBuffer16[0] == 0) {
            return;
        }

        // First word of output gets set to first word of the input, second word gets OR'D with ACK
        outputBuffer16[0] = inputBuffer16[0];
        outputBuffer16[1] = inputBuffer16[1] | 0x8000; // ACK?

        // Read the given Command and handle it
        uint32_t command = inputBuffer16[1];
        switch (command) {
        case MB_CMD_DIMM_SIZE:
                outputBuffer32[1] = 1024 * ONE_MB;
                break;
            case MB_CMD_STATUS:
                outputBuffer32[1] = MB_STATUS_READY;
                outputBuffer32[2] = 100;  // Load/Test Percentage (0-100)
                break;
            case MB_CMD_FIRMWARE_VERSION:
                outputBuffer32[1] = 0x0317;
                break;
            case MB_CMD_SYSTEM_TYPE:
                outputBuffer32[1] = MB_SYSTEM_TYPE_DEVELOPER | MB_SYSTEM_TYPE_GDROM;
                break;
            case MB_CMD_SERIAL_NUMBER:
                memcpy(&outputBuffer32[1], "A89E-25A47354512", 17);
                break;
            case MB_CMD_HARDWARE_TEST: {
                uint32_t testType = inputBuffer32[1];
                xbox::addr_xt resultWritePtr = inputBuffer32[2];
                outputBuffer32[1] = inputBuffer32[1];

                printf("Perform Test Type %X, place result at %08X\n", testType, resultWritePtr);

                // For now, just pretend we did the test and was successful
                // TODO: How to report percentage? Get's stuck on "CHECKING 0% but still shows "TEST OK"
                memcpy((void*)resultWritePtr, "TEST OK", 8);
            } break;
            default: printf("Unhandled MediaBoard Command: %04X\n", command);
        }

        // Clear the command bytes
        inputBuffer16[0] = 0;
        inputBuffer16[1] = 0;

        // Trigger LPC Interrupt
        HalSystemInterrupts[10].Assert(true);
        return;
    }

    printf("Unhandled MediaBoard mbcom: offset %08X\n", offset);
}
