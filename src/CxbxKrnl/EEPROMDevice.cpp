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
// *   src->CxbxKrnl->EEPROMDevice.cpp
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

#include "EEPROMDevice.h" // For EEPROMDevice

void EEPROMDevice::Init()
{
	// TODO
}

void EEPROMDevice::Reset()
{
	// TODO
}

void EEPROMDevice::QuickCommand(bool read)
{
	// TODO
}

uint8_t EEPROMDevice::ReceiveByte()
{
	return 0; // TODO
}

uint8_t EEPROMDevice::ReadByte(uint8_t command)
{
	return *(m_pEEPROM + command);
}

uint16_t EEPROMDevice::ReadWord(uint8_t command)
{
	return *((uint16_t*)(m_pEEPROM + command));
}

int EEPROMDevice::ReadBlock(uint8_t command, uint8_t *data)
{
	return 0; // TODO
}

void EEPROMDevice::SendByte(uint8_t data)
{
	// TODO
}

void EEPROMDevice::WriteByte(uint8_t command, uint8_t value)
{
	*((uint16_t*)(m_pEEPROM + command)) = value;
}

void EEPROMDevice::WriteWord(uint8_t command, uint16_t value)
{
	*((uint16_t*)(m_pEEPROM + command)) = value;
}

void EEPROMDevice::WriteBlock(uint8_t command, uint8_t* data, int length)
{
	std::memcpy(m_pEEPROM + command, data, length);
}
