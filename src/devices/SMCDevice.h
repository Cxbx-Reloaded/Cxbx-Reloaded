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
// *   src->CxbxKrnl->SMCDevice.h
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
#pragma once

#include "SMDevice.h"

// This https://upload.wikimedia.org/wikipedia/commons/9/94/Xbox-Motherboard-FR.jpg shows :
// PIC16LC63A-04/SO
// (M) 0123857
//
// Producer : http://www.microchip.com/wwwproducts/en/en010145
// Datasheet : http://ww1.microchip.com/downloads/en/DeviceDoc/30605D.pdf

// NOTE : Instead of calling this device by it's real name ("PIC16LC63A-04/SO"),
// we've decided to call this device "SMC", since we don't implement
// the low-level functionality of this PIC, but only the minimum set of
// high-level commands that are sufficient for the Xbox.

// Reading:

// From https://web.archive.org/web/20100617022549/http://www.xbox-linux.org/wiki/PIC :
// Command	Description
#define SMC_COMMAND_VERSION 0x01	// PIC version string
//0x03	tray state
#define SMC_COMMAND_AV_PACK 0x04	// A / V Pack state
#define SMC_COMMAND_CPU_TEMP 0x09 // CPU temperature (°C)
#define SMC_COMMAND_MOTHERBOARD_TEMP 0x0A // motherboard temperature (°C)
//0x0F	reads scratch register written with 0x0E
#define SMC_COMMAND_POWER_FAN_READBACK 0x10 // Current power fan speed (0-50)
//0x11	interrupt reason
//0x18	reading this reg locks up xbox in "overheated" state
#define SMC_COMMAND_SCRATCH 0x1B	// scratch register for the original kernel
#define SMC_COMMAND_CHALLENGE_1C 0x1C	// random number for boot challenge
#define SMC_COMMAND_CHALLENGE_1D 0x1D	// random number for boot challenge
#define SMC_COMMAND_CHALLENGE_1E 0x1E	// random number for boot challenge
#define SMC_COMMAND_CHALLENGE_1F 0x1F	// random number for boot challenge
//
// Writing:
//
//Command	Description
//0x01	PIC version string counter reset
#define SMC_COMMAND_RESET 0x02	 //0x02	reset and power off control
#define SMC_COMMAND_POWER_FAN_MODE 0x05 // power fan mode(0 = automatic; 1 = custom speed from reg 0x06)
#define SMC_COMMAND_POWER_FAN_REGISTER 0x06 // Set custom power fan speed (0-50)
#define SMC_COMMAND_LED_MODE 0x07	// LED mode(0 = automatic; 1 = custom sequence from reg 0x08)
#define SMC_COMMAND_LED_SEQUENCE 0x08	// LED flashing sequence
//0x0C	tray eject(0 = eject; 1 = load)
//0x0E	another scratch register ? seems like an error code.
//0x19	reset on eject(0 = enable; 1 = disable)
//0x1A	interrupt enable(write 0x01 to enable; can't disable once enabled)
#define SMC_COMMAND_SCRATCH 0x1B	//0x1B	scratch register for the original kernel
//0x20	response to PIC challenge(written first)
//0x21	response to PIC challenge(written second)

//
// Register values for SMC_COMMAND_RESET
//
#define SMC_RESET_ASSERT_RESET			0x01
#define SMC_RESET_ASSERT_POWERCYCLE		0x40
#define SMC_RESET_ASSERT_SHUTDOWN		0x80

//
// Register values for SMC_COMMAND_SCRATCH
//
#define SMC_SCRATCH_TRAY_EJECT_PENDING	0x01
#define SMC_SCRATCH_DISPLAY_FATAL_ERROR	0x02
#define SMC_SCRATCH_SHORT_ANIMATION		0x04
#define SMC_SCRATCH_DASHBOARD_BOOT		0x08

typedef enum {
	// http://xboxdevwiki.net/System_Management_Controller
	P01,
	P2L,
	D01, // Seen in a debug kit 
	D05, // Seen in a earlier model chihiro
} SCMRevision;

class SMCDevice : public SMDevice {
public:
	// constructor
	SMCDevice(SCMRevision revision);

	// SMDevice functions
	void Init();
	void Reset();

	void QuickCommand(bool read);
	uint8_t ReceiveByte();
	uint8_t ReadByte(uint8_t command);
	uint16_t ReadWord(uint8_t command);
	int ReadBlock(uint8_t command, uint8_t *data);

	void SendByte(uint8_t data);
	void WriteByte(uint8_t command, uint8_t value);
	void WriteWord(uint8_t command, uint16_t value);
	void WriteBlock(uint8_t command, uint8_t* data, int length);

private:
	SCMRevision m_revision;
	int m_PICVersionStringIndex = 0;
	uint8_t buffer[256] = {};
};
