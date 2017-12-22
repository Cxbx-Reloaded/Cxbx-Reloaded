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
// *   src->CxbxKrnl->Xbox.cpp
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
#include "Xbox.h"

#include "PCIBus.h" // For PCIBus
#include "SMBus.h" // For SMBus
#include "SMCDevice.h" // For SMCDevice
#include "EEPROMDevice.h" // For EEPROMDevice
#include "EmuNVNet.h" // For NVNetDevice

PCIBus* g_PCIBus;
SMBus* g_SMBus;
SMCDevice* g_SMC;
EEPROMDevice* g_EEPROM;
NVNetDevice* g_NVNet;

#define SMBUS_TV_ENCODER_ID_CONEXANT 0x8A // = Write; Read = 08B
#define SMBUS_TV_ENCODER_ID_FOCUS 0xD4 // = Write; Read = 0D5

void InitXboxHardware()
{
	g_PCIBus = new PCIBus();
	g_SMBus = new SMBus();
	g_SMC = new SMCDevice(Revision1_1); // TODO : Make configurable
	g_EEPROM = new EEPROMDevice();
	g_NVNet = new NVNetDevice();

	g_SMBus->ConnectDevice(SMBUS_SMC_SLAVE_ADDRESS, g_SMC);
	g_SMBus->ConnectDevice(SMBUS_EEPROM_ADDRESS, g_EEPROM);

	g_PCIBus->ConnectDevice(PCI_DEVID(0, PCI_DEVFN(1, 1)), g_SMBus);
	g_PCIBus->ConnectDevice(PCI_DEVID(0, PCI_DEVFN(4, 0)), g_NVNet);

	// TODO : Handle other SMBUS Addresses, like PIC_ADDRESS, XCALIBUR_ADDRESS
	// Resources : http://pablot.com/misc/fancontroller.cpp
	// https://github.com/JayFoxRox/Chihiro-Launcher/blob/master/hook.h
	// https://github.com/docbrown/vxb/wiki/Xbox-Hardware-Information
	// https://web.archive.org/web/20100617022549/http://www.xbox-linux.org/wiki/PIC
}
