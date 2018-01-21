// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   src->CxbxKrnl->SMCDevice.cpp
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

/* prevent name collisions */
namespace xboxkrnl
{
#include <xboxkrnl/xboxkrnl.h> // For xbox.h:AV_PACK_HDTV
};

#include "CxbxKrnl\CxbxKrnl.h"
#include "CxbxKrnl\EmuShared.h"

#include "SMCDevice.h" // For SMCDevice
#include "LED.h"

void SetLEDSequence(LED::Sequence aLEDSequence)
{
	// See http://xboxdevwiki.net/PIC#The_LED
	DbgPrintf("SMC : SetLEDSequence : %u\n", (byte)aLEDSequence);

	bool bLedHasChanged = true;

	if (aLEDSequence == LED::GREEN) // Automatic solid green color
	{
		int LedSequence[4] = { XBOX_LED_COLOUR_GREEN, XBOX_LED_COLOUR_GREEN, XBOX_LED_COLOUR_GREEN, XBOX_LED_COLOUR_GREEN };

		g_EmuShared->SetLedStatus(&bLedHasChanged);
		g_EmuShared->SetLedSequence(LedSequence);
	}
	else // Draw the color represented by the sequence obtained
	{
		int LedSequence[4] = { XBOX_LED_COLOUR_OFF, XBOX_LED_COLOUR_OFF, XBOX_LED_COLOUR_OFF, XBOX_LED_COLOUR_OFF };

		LedSequence[0] = ((aLEDSequence >> 3) & 1) | ((aLEDSequence >> 6) & 2);
		LedSequence[1] = ((aLEDSequence >> 2) & 1) | ((aLEDSequence >> 5) & 2);
		LedSequence[2] = ((aLEDSequence >> 1) & 1) | ((aLEDSequence >> 4) & 2);
		LedSequence[3] = (aLEDSequence & 1) | ((aLEDSequence >> 3) & 2);

		g_EmuShared->SetLedStatus(&bLedHasChanged);
		g_EmuShared->SetLedSequence(LedSequence);
	}
}

/* SMCDevice */

SMCDevice::SMCDevice(HardwareModel hardwareModel)
{
	m_HardwareModel = hardwareModel;
}

void SMCDevice::Init()
{
	m_PICVersionStringIndex = 0;
	buffer[SMC_COMMAND_AV_PACK] = AV_PACK_HDTV; // see http://xboxdevwiki.net/PIC#The_AV_Pack
	buffer[SMC_COMMAND_LED_SEQUENCE] = LED::GREEN;
	buffer[SMC_COMMAND_SCRATCH] = 0; // http://xboxdevwiki.net/PIC#Scratch_register_values
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
		// See http://xboxdevwiki.net/PIC#PIC_version_string
		switch (m_HardwareModel) {
		case Revision1_0: buffer[0] = "P01"[m_PICVersionStringIndex]; break;
		case Revision1_1: buffer[0] = "P05"[m_PICVersionStringIndex]; break;
		case DebugKit: buffer[0] = "DXB"[m_PICVersionStringIndex]; break;
		// default: UNREACHABLE(hardwareModel);
		}

		m_PICVersionStringIndex = (m_PICVersionStringIndex + 1) % 3;
		break;
	//0x03	tray state
	//#define SMC_COMMAND_AV_PACK 0x04	// A / V Pack state
	//0x09	CPU temperature(�C)
	//0x0A	board temperature(�C)
	case 0x0F: // reads scratch register written with 0x0E
		return buffer[0x0E];
	//0x10	current fan speed(0~50)
	//0x11	interrupt reason
	//0x18	reading this reg locks up xbox in "overheated" state
	//#define SMC_COMMAND_SCRATCH 0x1B	// scratch register for the original kernel
	case SMC_COMMAND_CHALLENGE_1C: // random number for boot challenge
	case SMC_COMMAND_CHALLENGE_1D: // random number for boot challenge
	case SMC_COMMAND_CHALLENGE_1E: // random number for boot challenge
	case SMC_COMMAND_CHALLENGE_1F: // random number for boot challenge
		if (m_HardwareModel == DebugKit)
			// See http://xboxdevwiki.net/PIC#PIC_Challenge_.28regs_0x1C.7E0x21.29
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
		// See http://xboxdevwiki.net/PIC#Reset_and_Power_Off
		switch (value) {
		case SMC_RESET_ASSERT_RESET: return; //TODO
		case SMC_RESET_ASSERT_POWERCYCLE: return; //TODO
		case SMC_RESET_ASSERT_SHUTDOWN: CxbxKrnlShutDown(); return; // Power off, terminating the emulation
		}
	//0x05	power fan mode(0 = automatic; 1 = custom speed from reg 0x06)
	//0x06	power fan speed(0..~50)
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
		// ergo720: if WriteWord is true the Xbox still sets the LED correctly but it errors with ntstatus
		// STATUS_IO_DEVICE_ERROR, however WriteWord is not accessible from here
		// The LED flashing sequence is stored in the buffer of the SMCDevice class, so there's nothing to do here
		break;
	//0x0C	tray eject(0 = eject; 1 = load)
	//0x0E	another scratch register ? seems like an error code.
	//0x19	reset on eject(0 = enable; 1 = disable)
	//0x1A	interrupt enable(write 0x01 to enable; can't disable once enabled)
	//0x1B	scratch register for the original kernel
	//0x20	response to PIC challenge(written first)
	//0x21	response to PIC challenge(written second)
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
