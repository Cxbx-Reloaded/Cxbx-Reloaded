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
// *  (c) 2018 Luke Usher <luke.usher@outlook.com>
// *
// *  All rights reserved
// *
// ******************************************************************

#ifndef _APU_H_
#define _APU_H_

#include "../PCIDevice.h"
class APUDevice : public PCIDevice {
public:
	using PCIDevice::PCIDevice;

	// PCI Functions
	void Init();
	void Reset();

	uint32_t IORead(int barIndex, uint32_t addr, unsigned size = sizeof(uint8_t));
	void IOWrite(int barIndex, uint32_t addr, uint32_t data, unsigned size = sizeof(uint8_t));

	uint32_t MMIORead(int barIndex, uint32_t addr, unsigned size);
	void MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size);
private:
	uint32_t GPRead(uint32_t addr, unsigned size);
	void GPWrite(uint32_t addr, uint32_t value, unsigned size);
	uint32_t EPRead(uint32_t addr, unsigned size);
	void EPWrite(uint32_t addr, uint32_t value, unsigned size);
	uint32_t VPRead(uint32_t addr, unsigned size);
	void VPWrite(uint32_t addr, uint32_t value, unsigned size);
};

#endif
