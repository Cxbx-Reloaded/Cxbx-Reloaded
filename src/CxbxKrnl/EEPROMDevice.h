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
// *   src->CxbxKrnl->EEPROMDevice.h
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

#define SMBUS_EEPROM_ADDRESS 0xA8 // = Write; Read = 0xA9

class EEPROMDevice : public SMDevice {
public:
	// constructor
	EEPROMDevice(uint8_t* pEEPROM) : m_pEEPROM(pEEPROM) {};

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
	uint8_t* m_pEEPROM;
};

extern EEPROMDevice* g_EEPROM;
