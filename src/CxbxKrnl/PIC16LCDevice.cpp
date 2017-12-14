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
// *   src->CxbxKrnl->PIC16LCDevice.cpp
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

#include <cstring> // For memcpy

#include "PIC16LCDevice.h" // For PIC16LCDevice

void SetLEDSequence(uint8_t LEDSequence)
{
	// TODO : Move to best suited location & implement
	// See http://xboxdevwiki.net/PIC#The_LED
}

// See http://xboxdevwiki.net/PIC#PIC_version_string
const char *PICVersion_Retail_1_0 = "P01";
const char *PICVersion_Retail_1_1 = "P05";
const char *PICVersion_Debug_Kit = "DXB";

void PIC16LCDevice::Init()
{
	m_PICVersion = (char*)PICVersion_Retail_1_1; // TODO : Configurable selection

	buffer[SMC_COMMAND_VERSION] = m_PICVersion[2];
	buffer[SMC_COMMAND_AV_PACK] = 2; // VGA, see http://xboxdevwiki.net/PIC#The_AV_Pack
	buffer[SMC_COMMAND_LED_SEQUENCE] = 0x0F; // solid green
	buffer[SMC_COMMAND_SCRATCH] = 0; // http://xboxdevwiki.net/PIC#Scratch_register_values
}

void PIC16LCDevice::Reset()
{
	// TODO
}

void PIC16LCDevice::QuickCommand(bool read)
{
	// TODO
}

uint8_t PIC16LCDevice::ReceiveByte()
{
	return 0; // TODO
}

uint8_t PIC16LCDevice::ReadByte(uint8_t command)
{
	switch (command) {
	case SMC_COMMAND_VERSION: // 0x01 PIC version string
		// See https://github.com/bji/libmame/blob/master/src/mame/drivers/chihiro.c#L977
		if (buffer[0] == m_PICVersion[0])
			buffer[0] = m_PICVersion[1];
		else
			if (buffer[0] == m_PICVersion[1])
				buffer[0] = m_PICVersion[2];
			else
				buffer[0] = m_PICVersion[0];
		break;
	//0x03	tray state
	//#define SMC_COMMAND_AV_PACK 0x04	// A / V Pack state
	//0x09	CPU temperature(°C)
	//0x0A	board temperature(°C)
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
		if (m_PICVersion == PICVersion_Debug_Kit)
			// See http://xboxdevwiki.net/PIC#PIC_Challenge_.28regs_0x1C.7E0x21.29
			return 0;

		break;
	// case SMC_COMMAND_...: TODO
	}

	return buffer[command];
}

uint16_t PIC16LCDevice::ReadWord(uint8_t command)
{
	return 0; // TODO
}

int PIC16LCDevice::ReadBlock(uint8_t command, uint8_t *data)
{
	return 0; // TODO
}

void PIC16LCDevice::SendByte(uint8_t data)
{
	// TODO
}

void PIC16LCDevice::WriteByte(uint8_t command, uint8_t value)
{
	switch (command) {
	case SMC_COMMAND_VERSION: // 0x01 PIC version string counter reset
		if (value == 0) // Note : MAME Xbox/Chihiro driver doesn't check for zero
			buffer[0] = m_PICVersion[2];
		return;
	//0x02	reset and power off control
	//0x05	power fan mode(0 = automatic; 1 = custom speed from reg 0x06)
	//0x06	power fan speed(0..~50)
	case SMC_COMMAND_LED_MODE: // 0x07 LED mode(0 = automatic; 1 = custom sequence from reg 0x08)
		switch (value) {
		case 0: SetLEDSequence(0x0F); return; // Automatic; Solid green?
		case 1: SetLEDSequence(buffer[SMC_COMMAND_LED_SEQUENCE]); // Custom sequence
		}
		return;
	// #define SMC_COMMAND_LED_SEQUENCE 0x08	// LED flashing sequence
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

void PIC16LCDevice::WriteWord(uint8_t command, uint16_t value)
{
	// TODO : Is this needed and/or acceptable?
	WriteByte(command, value >> 8);
	WriteByte(command + 1, value & 0xFF);
}

void PIC16LCDevice::WriteBlock(uint8_t command, uint8_t* data, int length)
{
	// TODO
}
