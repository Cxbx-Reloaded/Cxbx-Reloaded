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
// *  (c) 2018 ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

#pragma once

#include "SMDevice.h"

// ergo702: annoyingly, I must override all the pure functions defined in SMDevice even if don't need most of them. If I don't, then
// the compiler will complain about that ADM1032Device is an abstract class since it has pure functions within. Also, ADM1032Device
// must derive from SMDevice because ConnectDevice expects a pointer to an object of that class...

class ADM1032Device : public SMDevice
{
	public:
		void Init();
		void Reset() {};

		void QuickCommand(bool read) {};
		uint8_t ReceiveByte() { return 0; };
		uint8_t ReadByte(uint8_t command);
		uint16_t ReadWord(uint8_t command) { return 0; };
		int ReadBlock(uint8_t command, uint8_t *data) { return 0; };
	
		void SendByte(uint8_t data) {};
		void WriteByte(uint8_t command, uint8_t value) {};
		void WriteWord(uint8_t command, uint16_t value) {};
		void WriteBlock(uint8_t command, uint8_t* data, int length) {};

	private:
		uint8_t m_CPUTemperature;
		uint8_t m_MBTemperature;
};

extern ADM1032Device* g_ADM1032;
