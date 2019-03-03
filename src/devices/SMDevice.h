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
// *  (c) 2018 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************

#ifndef _SMDEVICE_H_
#define _SMDEVICE_H_

#include <cstdint>

class SMDevice {
	public:
		virtual void Init() = 0;
		virtual void Reset() = 0;

		virtual void QuickCommand(bool read) = 0;
		virtual uint8_t ReceiveByte() = 0;
		virtual uint8_t ReadByte(uint8_t command) = 0;
		virtual uint16_t ReadWord(uint8_t command) = 0;
		virtual int ReadBlock(uint8_t command, uint8_t *data) = 0;

		virtual void SendByte(uint8_t data) = 0;
		virtual void WriteByte(uint8_t command, uint8_t value) = 0;
		virtual void WriteWord(uint8_t command, uint16_t value) = 0;
		virtual void WriteBlock(uint8_t command, uint8_t* data, int length) = 0;
};

#endif