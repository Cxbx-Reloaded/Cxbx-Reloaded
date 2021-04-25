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
// *  (c) 2017 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************

#include "Xbox.h" // For HardwareModel
#include "common\xbe\Xbe.h"  // Without this HLEIntercept complains about some undefined xbe variables
#include "core\hle\Intercept.hpp"
#include "EmuShared.h"

PCIBus* g_PCIBus;
SMBus* g_SMBus;
MCPXDevice* g_MCPX;
SMCDevice* g_SMC;
EEPROMDevice* g_EEPROM;
NVNetDevice* g_NVNet;
NV2ADevice* g_NV2A;
ADM1032Device* g_ADM1032;
USBDevice* g_USB0;
MediaBoard* g_MediaBoard;

MCPXRevision MCPXRevisionFromHardwareModel(HardwareModel hardwareModel)
{
	switch (hardwareModel) {
	case Revision1_0:
	case Revision1_1:
	case Revision1_2:
	case Revision1_3:
	case Revision1_4:
	case Revision1_5:
	case Revision1_6:
		return MCPXRevision::MCPX_X3;
	case DebugKit:
		// EmuLog(LOG_LEVEL::WARNING, "Guessing MCPXVersion");
		return MCPXRevision::MCPX_X2;
	default:
		// UNREACHABLE(hardwareModel);
		return MCPXRevision::MCPX_X3;
	}
}

SCMRevision SCMRevisionFromHardwareModel(HardwareModel hardwareModel)
{
	switch (hardwareModel) {
	case Revision1_0:
		return SCMRevision::P01; // Our SCM returns PIC version string "P01"
	case Revision1_1:
	case Revision1_2:
	case Revision1_3:
	case Revision1_4:
	case Revision1_5:
	case Revision1_6:
		// EmuLog(LOG_LEVEL::WARNING, "Guessing SCMRevision");
		return SCMRevision::P2L; // Assumption; Our SCM returns PIC version string "P05"
	case DebugKit:
		return SCMRevision::D01; // Our SCM returns PIC version string "DXB"
	default:
		// UNREACHABLE(hardwareModel);
		return SCMRevision::P2L;
	}
}

TVEncoder TVEncoderFromHardwareModel(HardwareModel hardwareModel)
{
	switch (hardwareModel) {
	case Revision1_0:
	case Revision1_1:
	case Revision1_2:
	case Revision1_3:
		return TVEncoder::Conexant;
	case Revision1_4:
		return TVEncoder::Focus;
	case Revision1_5:
		return TVEncoder::Focus; // Assumption
	case Revision1_6:
		return TVEncoder::XCalibur;
	case DebugKit:
		// LukeUsher : My debug kit and at least most of them (maybe all?)
		// are equivalent to v1.0 and have Conexant encoders.
		return TVEncoder::Conexant;
	default: 
		// UNREACHABLE(hardwareModel);
		return TVEncoder::Focus;
	}
}

void InitXboxHardware(HardwareModel hardwareModel)
{
	// Determine which (revisions of which) components should be used for this hardware model
	MCPXRevision mcpx_revision = MCPXRevisionFromHardwareModel(hardwareModel);
	SCMRevision smc_revision = SCMRevisionFromHardwareModel(hardwareModel);
	TVEncoder tv_encoder = TVEncoderFromHardwareModel(hardwareModel);

	// Create busses
	g_PCIBus = new PCIBus();
	g_SMBus = new SMBus();

	// Create devices
	g_MCPX = new MCPXDevice(mcpx_revision);

	// TODO: For Chihiro, different games modes require different DIP switch settings
	// Chihiro FilterBoard dip-switches 6,7,8 change this value!
	g_SMC = new SMCDevice(smc_revision, g_bIsChihiro ? 0 : 1); // 0 = AV_PACK_SCART, 1 = AV_PACK_HDTV. Chihiro doesn't support HDTV!
															   // SMC uses different AV_PACK values than the Kernel
															   // See https://xboxdevwiki.net/PIC#The_AV_Pack
	g_EEPROM = new EEPROMDevice();
	g_NVNet = new NVNetDevice();
	g_NV2A = new NV2ADevice();
	g_ADM1032 = new ADM1032Device();
	if (bLLE_USB) {
		g_USB0 = new USBDevice();
	}

	if (g_bIsChihiro) {
        g_MediaBoard = new MediaBoard();
        char MediaBoardMountPath[xbox::max_path];
        g_EmuShared->GetTitleMountPath(MediaBoardMountPath);
        g_MediaBoard->SetMountPath(MediaBoardMountPath);
	}

	// Connect devices to SM bus
	g_SMBus->ConnectDevice(SMBUS_ADDRESS_SYSTEM_MICRO_CONTROLLER, g_SMC); // W 0x20 R 0x21
	g_SMBus->ConnectDevice(SMBUS_ADDRESS_EEPROM, g_EEPROM); // W 0xA8 R 0xA9

	// TODO : Other SMBus devices to connect
	//g_SMBus->ConnectDevice(SMBUS_ADDRESS_MCPX, g_MCPX); // W 0x10 R 0x11 -- TODO : Is MCPX an SMBus and/or PCI device?
	g_SMBus->ConnectDevice(SMBUS_ADDRESS_TEMPERATURE_MONITOR, g_ADM1032); // W 0x98 R 0x99
	//g_SMBus->ConnectDevice(SMBUS_ADDRESS_TV_ENCODER, g_TVEncoder); // W 0x88 R 0x89
	switch (tv_encoder) {
	case TVEncoder::Conexant:
		// g_SMBus->ConnectDevice(SMBUS_ADDRESS_TV_ENCODER_ID_CONEXANT, g_TVEncoderConexant); // W 0x8A R 0x8B
		break;
	case TVEncoder::Focus:
		// g_SMBus->ConnectDevice(SMBUS_ADDRESS_TV_ENCODER_ID_FOCUS, g_TVEncoderFocus); // W 0xD4 R 0xD5
		break;
	case TVEncoder::XCalibur:
		// g_SMBus->ConnectDevice(SMBUS_ADDRESS_TV_ENCODER_ID_XCALIBUR, g_TVEncoderXCalibur); // W 0xE0 R 0xE1
		break;
	}

	// Connect devices to PCI bus
	g_PCIBus->ConnectDevice(PCI_DEVID(0, PCI_DEVFN(1, 1)), g_SMBus);
	g_PCIBus->ConnectDevice(PCI_DEVID(0, PCI_DEVFN(4, 0)), g_NVNet);
	//g_PCIBus->ConnectDevice(PCI_DEVID(0, PCI_DEVFN(4, 1)), g_MCPX); // MCPX device ID = 0x0808 ?
	//g_PCIBus->ConnectDevice(PCI_DEVID(0, PCI_DEVFN(5, 0)), g_NVAPU);
	//g_PCIBus->ConnectDevice(PCI_DEVID(0, PCI_DEVFN(6, 0)), g_AC97);
	g_PCIBus->ConnectDevice(PCI_DEVID(1, PCI_DEVFN(0, 0)), g_NV2A);
	if (bLLE_USB) {
		// ergo720: according to some research done by LukeUsher, only Xbox Alpha Kits have a two HCs configuration. This seems to also be confirmed by the xboxdevwiki,
		// which states that it has a xircom PGPCI2(OPTI 82C861) 2 USB port PCI card -> 2 ports, not 4. Finally, I disassembled various xbe's and discovered that the number
		// of ports per HC is hardcoded as 4 in the driver instead of being detected at runtime by reading the HcRhDescriptorA register and so a game would have to be
		// recompiled to support 2 HCs, which further confirms the point. Because we are not going to emulate an Alpha Kit, we can simply ignore the USB1 device.

		g_PCIBus->ConnectDevice(PCI_DEVID(0, PCI_DEVFN(2, 0)), g_USB0);
	}

	// TODO : Handle other SMBUS Addresses, like PIC_ADDRESS, XCALIBUR_ADDRESS
	// Resources : http://pablot.com/misc/fancontroller.cpp
	// https://github.com/JayFoxRox/Chihiro-Launcher/blob/master/hook.h
	// https://github.com/docbrown/vxb/wiki/Xbox-Hardware-Information
	// https://web.archive.org/web/20100617022549/https://www.xbox-linux.org/wiki/PIC
}
