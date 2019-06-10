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
// *  (c) 2017 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#define _XBOXKRNL_DEFEXTRN_

#define LOG_PREFIX CXBXR_MODULE::SMC

/* prevent name collisions */
namespace xboxkrnl
{
#include <xboxkrnl/xboxkrnl.h> // For xbox.h:AV_PACK_HDTV
};

#include "core\kernel\init\CxbxKrnl.h"
#include "EmuShared.h"

#include "SMCDevice.h" // For SMCDevice
#include "ADM1032Device.h" // For ADM1032Device
#include "LED.h"

void SetLEDSequence(LED::Sequence aLEDSequence)
{
	// See https://xboxdevwiki.net/PIC#The_LED
	EmuLog(LOG_LEVEL::DEBUG, "SetLEDSequence : %u", (byte)aLEDSequence);

    union {
        UINT  LedSequenceBlock;
        UCHAR LedSequence[4] = { XBOX_LED_COLOUR_OFF, XBOX_LED_COLOUR_OFF, XBOX_LED_COLOUR_OFF, XBOX_LED_COLOUR_OFF };
    };

	LedSequence[0] = ((aLEDSequence >> 6) & 2) | ((aLEDSequence >> 3) & 1);
	LedSequence[1] = ((aLEDSequence >> 5) & 2) | ((aLEDSequence >> 2) & 1);
	LedSequence[2] = ((aLEDSequence >> 4) & 2) | ((aLEDSequence >> 1) & 1);
	LedSequence[3] = ((aLEDSequence >> 3) & 2) | ((aLEDSequence >> 0) & 1);

	ipc_send_gui_update(IPC_UPDATE_GUI::XBOX_LED_COLOUR, LedSequenceBlock);
}

/* SMCDevice */

SMCDevice::SMCDevice(SCMRevision revision, uint8_t av_pack)
{
	m_revision = revision;
	buffer[SMC_COMMAND_AV_PACK] = av_pack;
}

void SMCDevice::Init()
{
	m_PICVersionStringIndex = 0;
	buffer[SMC_COMMAND_LED_SEQUENCE] = LED::GREEN;
	buffer[SMC_COMMAND_SCRATCH] = 0; // https://xboxdevwiki.net/PIC#Scratch_register_values
}

void SMCDevice::Reset()
{
	// TODO
}

void SMCDevice::QuickCommand(bool read)
{
	// TODO
}

uint8_t SMCDevice::ReceiveByte()
{
	return 0; // TODO
}

uint8_t SMCDevice::ReadByte(uint8_t command)
{
	switch (command) {
	case SMC_COMMAND_VERSION: // 0x01 PIC version string
		// See https://xboxdevwiki.net/PIC#PIC_version_string
		switch (m_revision) {
		case SCMRevision::P01: buffer[1] = "P01"[m_PICVersionStringIndex]; break;
		case SCMRevision::P2L: buffer[1] = "P05"[m_PICVersionStringIndex]; break; // ??
		case SCMRevision::D01: buffer[1] = "DXB"[m_PICVersionStringIndex]; break;
		case SCMRevision::D05: buffer[1] = "D05"[m_PICVersionStringIndex]; break; // ??
		// default: UNREACHABLE(m_revision);
		}

		m_PICVersionStringIndex = (m_PICVersionStringIndex + 1) % 3;
		break;
	//case 0x03: // tray state
	//case SMC_COMMAND_AV_PACK: // 0x04	// A / V Pack state
	case SMC_COMMAND_CPU_TEMP: // 0x09 // CPU temperature (°C)
		return g_ADM1032->ReadByte(0x1);
	case SMC_COMMAND_MOTHERBOARD_TEMP: // 0x0A // motherboard temperature (°C)
		return g_ADM1032->ReadByte(0x0);
	case 0x0F: // reads scratch register written with 0x0E
		return buffer[0x0E];
	//case SMC_COMMAND_POWER_FAN_READBACK: // 0x10 // Current power fan speed (0-50)
	//case 0x11: // interrupt reason
	//case 0x18: // reading this reg locks up xbox in "overheated" state
	//case SMC_COMMAND_SCRATCH: // 0x1B	// scratch register for the original kernel
	case SMC_COMMAND_CHALLENGE_1C: // random number for boot challenge
	case SMC_COMMAND_CHALLENGE_1D: // random number for boot challenge
	case SMC_COMMAND_CHALLENGE_1E: // random number for boot challenge
	case SMC_COMMAND_CHALLENGE_1F: // random number for boot challenge
		if (m_revision == SCMRevision::D01)
			// See https://xboxdevwiki.net/PIC#PIC_Challenge_.28regs_0x1C.7E0x21.29
			return 0;

		break;
	// case SMC_COMMAND_...: TODO
	}

	return buffer[command];
}

uint16_t SMCDevice::ReadWord(uint8_t command)
{
	return 0; // TODO
}

int SMCDevice::ReadBlock(uint8_t command, uint8_t *data)
{
	return 0; // TODO
}

void SMCDevice::SendByte(uint8_t data)
{
	// TODO
}

void SMCDevice::WriteByte(uint8_t command, uint8_t value)
{
	switch (command) {
	case SMC_COMMAND_VERSION: // 0x01 PIC version string counter reset
		if (value == 0) // Note : MAME Xbox/Chihiro driver doesn't check for zero
			m_PICVersionStringIndex = 0;
		return;
	case SMC_COMMAND_RESET: //0x02	reset and power off control
		// See https://xboxdevwiki.net/PIC#Reset_and_Power_Off
		switch (value) {
		case SMC_RESET_ASSERT_RESET: return; // TODO
		case SMC_RESET_ASSERT_POWERCYCLE: return; // TODO
		case SMC_RESET_ASSERT_SHUTDOWN: CxbxKrnlShutDown(); return; // Power off, terminating the emulation
		}
	//case SMC_COMMAND_POWER_FAN_MODE: // 0x05 // power fan mode(0 = automatic; 1 = custom speed from reg 0x06)
	//case SMC_COMMAND_POWER_FAN_REGISTER: // 0x06 // Set custom power fan speed (0-50)
	case SMC_COMMAND_LED_MODE: // 0x07 LED mode(0 = automatic; 1 = custom sequence from reg 0x08)
		switch (value) {
		case 0: SetLEDSequence(LED::GREEN); return; // Automatic LED management: we set it to solid green
		case 1: SetLEDSequence((LED::Sequence)buffer[SMC_COMMAND_LED_SEQUENCE]); return; // Custom sequence
		default:
		// Notes from https://github.com/ergo720/Cxbx-Reloaded/blob/LED/src/CxbxKrnl/EmuKrnlHal.cpp#L572
		//
		// HalWriteSMBusValue(0x20, 0x08, false, x) and then HalWriteSMBusValue(0x20, 0x07, false, y > 1)
		// will cause the led to be solid green, while the next pair of 
		// HalWriteSMBusValue with arbitrary y will cause the led to assume the color of the sequence x
		// and afterwards this will repeat with whatever y; ntstatus is always 0
		//
		// TODO : Implement the above, SMB_GLOBAL_STATUS should probably get the GS_PRERR_STS flag. But how?
		return;
		}
	case SMC_COMMAND_LED_SEQUENCE: // 0x08 LED flashing sequence
		// The LED flashing sequence is stored in the buffer of the SMCDevice class, so there's nothing to do here
		break;
	//case 0x0C: // tray eject(0 = eject; 1 = load)
	//case 0x0E: // another scratch register ? seems like an error code.
	//case 0x19: // reset on eject(0 = enable; 1 = disable)
	//case 0x1A: // interrupt enable(write 0x01 to enable; can't disable once enabled)
	case SMC_COMMAND_SCRATCH: //0x1B	scratch register for the original kernel
		// See https://xboxdevwiki.net/PIC#Scratch_register_values
		switch (value) {
		case SMC_SCRATCH_TRAY_EJECT_PENDING: return; // TODO
		case SMC_SCRATCH_DISPLAY_FATAL_ERROR:
		{
			int FatalFlag;
			g_EmuShared->GetBootFlags(&FatalFlag);
			FatalFlag |= BOOT_FATAL_ERROR;
			g_EmuShared->SetBootFlags(&FatalFlag);
			break;
		}
		case SMC_SCRATCH_SHORT_ANIMATION: return; // TODO
		case SMC_SCRATCH_DASHBOARD_BOOT: return;  // TODO
		}
		break;
	//case 0x20: // response to PIC challenge(written first)
	//case 0x21: // response to PIC challenge(written second)
	}

	buffer[command] = value;
}

void SMCDevice::WriteWord(uint8_t command, uint16_t value)
{
	// Notes from https://github.com/ergo720/Cxbx-Reloaded/blob/LED/src/CxbxKrnl/EmuKrnlHal.cpp#L554 :
	//
	// [..] WriteWord [..] still sets the LED correctly but it errors
	// with ntstatus STATUS_IO_DEVICE_ERROR and afterwards the LED cannot be set 
	// any longer until restarting the console
	//
	// TODO : Implement the above, SMB_GLOBAL_STATUS should probably a flag (like GS_ABRT_STS or GS_PRERR_STS). But how?
}

void SMCDevice::WriteBlock(uint8_t command, uint8_t* data, int length)
{
	// TODO
}

