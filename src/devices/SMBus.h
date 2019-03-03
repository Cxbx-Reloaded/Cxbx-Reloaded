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

#ifndef _SMBUS_H_
#define _SMBUS_H_

#include "PCIDevice.h"
#include "SMDevice.h"

#include <map>

#define SMB_ADDR_OFFSET			0xE0
#define SMB_IOSIZE				16
#define SMB_IRQ					11

#define SMB_GLOBAL_STATUS       0x0
#define SMB_GLOBAL_ENABLE       0x2
#define SMB_HOST_ADDRESS        0x4
#define SMB_HOST_DATA           0x6
#define SMB_HOST_COMMAND        0x8
#define SMB_HOST_BLOCK_DATA     0x9
#define SMB_HAS_DATA            0xA
#define SMB_HAS_DEVICE_ADDRESS  0xC
#define SMB_HAS_HOST_ADDRESS    0xE
#define SMB_SNOOP_ADDRESS       0xF

/* AMD756 constants */
#define AMD756_QUICK        0x00
#define AMD756_BYTE         0x01
#define AMD756_BYTE_DATA    0x02
#define AMD756_WORD_DATA    0x03
#define AMD756_PROCESS_CALL 0x04
#define AMD756_BLOCK_DATA   0x05

// SMB_GLOBAL_STATUS flags
#define GS_ABRT_STS (1 << 0)
#define GS_COL_STS  (1 << 1)
#define GS_PRERR_STS    (1 << 2)
#define GS_HST_STS  (1 << 3)
#define GS_HCYC_STS (1 << 4)
#define GS_TO_STS   (1 << 5)
#define GS_SMB_STS  (1 << 11)

#define GS_CLEAR_STS    (GS_ABRT_STS | GS_COL_STS | GS_PRERR_STS |  GS_HCYC_STS | GS_TO_STS )

#define GE_CYC_TYPE_MASK    (7)
#define GE_HOST_STC     (1 << 3)

#define GE_HCYC_EN      (1 << 4)
#define GE_ABORT        (1 << 5)

class SMBus : public PCIDevice {
	public:
		using PCIDevice::PCIDevice;

		// PCI Functions
		void Init();
		void Reset();

		uint32_t IORead(int barIndex, uint32_t addr, unsigned size = sizeof(uint8_t));
		void IOWrite(int barIndex, uint32_t addr, uint32_t data, unsigned size = sizeof(uint8_t));

		uint32_t MMIORead(int barIndex, uint32_t addr, unsigned size);
		void MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size);

		// Misc
		void ConnectDevice(uint8_t addr, SMDevice *device);

	private:
		uint8_t m_Status;
		uint8_t m_Control;
		uint8_t m_Command;
		uint8_t m_Address;
		uint8_t m_Data0;
		uint8_t m_Data1;
		uint8_t m_Data[32];
		uint8_t m_Index;

		void ExecuteTransaction();

		std::map<uint8_t, SMDevice*> m_Devices;
};

#endif
