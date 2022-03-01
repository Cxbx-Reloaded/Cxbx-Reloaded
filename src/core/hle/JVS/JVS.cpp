// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
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
// *  (c) 2019 Luke Usher <luke.usher@outlook.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#define _XBOXKRNL_DEFEXTRN_

#define LOG_PREFIX CXBXR_MODULE::JVS

#undef FIELD_OFFSET     // prevent macro redefinition warnings

#include "EmuShared.h"
#include "common\Logging.h"
#include "common\FilePaths.hpp"
#include "core\kernel\init\CxbxKrnl.h"
#include "core\kernel\support\Emu.h"
#include "core\hle\JVS\JVS.h"
#include "core\hle\Intercept.hpp"
#include "devices\chihiro\JvsIo.h"
#include "devices\Xbox.h"
#include <thread>

#pragma warning(disable:4244) // Silence mio compiler warnings
#include <mio/mmap.hpp>
#pragma warning(default:4244)

// Global variables used to store JVS related firmware/eeproms
mio::mmap_sink g_BaseBoardQcFirmware;		// QC Microcontroller firmware
mio::mmap_sink g_BaseBoardScFirmware;		// SC Microcontroller firmware
mio::mmap_sink g_BaseBoardEeprom;			// Config EEPROM
mio::mmap_sink g_BaseBoardBackupMemory;		// Backup Memory (high-scores, etc)

typedef struct _baseboard_state_t {
	// Switch 1:	Horizontal Display, On = Vertical Display
	// Switch 2-3:	D3D Resolution Configuraton
	// Switch 4:	0 = Hardware Vertex Processing, 1 = Software Vertex processing (Causes D3D to fail).. Horizontal frequency?
	// Switch 5:	Unknown
	// Switch 6-8:	Connected AV Pack flag
	bool DipSwitch[8];
	bool TestButton;
	bool ServiceButton;
	uint8_t JvsSense;

	void Reset()
	{
		// TODO: Make this configurable
		DipSwitch[0] = false;
		DipSwitch[1] = false;
		DipSwitch[2] = true;
		DipSwitch[3] = true;
		DipSwitch[4] = false;
		DipSwitch[5] = true;
		DipSwitch[6] = true;
		DipSwitch[7] = true;
		TestButton = false;
		ServiceButton = false;
		JvsSense = 0;
	}

	uint8_t GetAvPack()
	{
		uint8_t avpack = 0;

		// Dip Switches 6,7,8 combine to form the Av Pack ID
		// TODO: Verify the order, these might need to be reversed
		avpack &= ~((DipSwitch[5] ? 1 : 0) << 2);
		avpack &= ~((DipSwitch[6] ? 1 : 0) << 1);
		avpack &= ~ (DipSwitch[7] ? 1 : 0);

		return avpack;
	}

	uint8_t GetPINSA()
	{
		uint8_t PINSA = 0b11101111; // 1 = Off, 0 = On

		// Dip Switches 1-3 are set on PINSA bits 0-2
		PINSA &= ~ (DipSwitch[0] ? 1 : 0);
		PINSA &= ~((DipSwitch[1] ? 1 : 0) << 1);
		PINSA &= ~((DipSwitch[2] ? 1 : 0) << 2);
		
		// Bit 3 is currently unknown, so we don't modify that bit

		// Dip Switches 4,5 are set on bits 4,5
		PINSA &= ~((DipSwitch[3] ? 1 : 0) << 4);
		PINSA &= ~((DipSwitch[4] ? 1 : 0) << 5);

		// Bit 6 = Test, Bit 7 = Service
		PINSA &= ~((TestButton ? 1 : 0) << 6);
		PINSA &= ~((ServiceButton ? 1 : 0) << 7);

		return PINSA;
	}

	uint8_t GetPINSB()
	{
		// PINSB bits 0-1 represent the JVS Sense line
		return JvsSense;
	}

} baseboard_state_t;

baseboard_state_t ChihiroBaseBoardState = {};
DWORD* g_pPINSA = nullptr; // Qc PINSA Register: Contains Filter Board DIP Switches + Test/Service buttons
DWORD* g_pPINSB = nullptr; // Qc PINSB Register: Contains JVS Sense Pin state 

bool JVS_LoadFile(std::string path, mio::mmap_sink& data)
{
	FILE* fp = fopen(path.c_str(), "rb");

	if (fp == nullptr) {
		return false;
	}

	std::error_code error;
	data = mio::make_mmap_sink(path, error);

	if (error) {
		return false;
	}

	return true;
}

void JvsInputThread()
{
	g_AffinityPolicy->SetAffinityOther(GetCurrentThread());

	while (true) {
		// This thread is responsible for reading the emulated Baseboard state
		// and setting the correct internal variables
		ChihiroBaseBoardState.TestButton = GetAsyncKeyState(VK_F1);
		ChihiroBaseBoardState.ServiceButton = GetAsyncKeyState(VK_F2);

		// Call into the Jvs I/O board update function
		g_pJvsIo->Update();

		if (g_pPINSA != nullptr) {
			*g_pPINSA = ChihiroBaseBoardState.GetPINSA();
		}

		if (g_pPINSB != nullptr) {
			*g_pPINSB = ChihiroBaseBoardState.GetPINSB();
		}
	}
}


void JVS_Init()
{
	// Init Jvs IO board
	g_pJvsIo = new JvsIo(&ChihiroBaseBoardState.JvsSense);

	std::string romPath = g_DataFilePath + std::string("\\EmuDisk\\Chihiro");
	std::string baseBoardQcFirmwarePath = "ic10_g24lc64.bin";
	std::string baseBoardScFirmwarePath = "pc20_g24lc64.bin";
	std::string baseBoardEepromPath = "ic11_24lc024.bin";
	std::string baseBoardBackupRamPath = "backup_ram.bin";

	if (!JVS_LoadFile((romPath + "\\" + baseBoardQcFirmwarePath).c_str(), g_BaseBoardQcFirmware)) {
		("Failed to load base board firmware: %s", baseBoardQcFirmwarePath.c_str());
	}

	if (!JVS_LoadFile((romPath + "\\" + baseBoardScFirmwarePath).c_str(), g_BaseBoardScFirmware)) {
		CxbxrAbort("Failed to load base board qc firmware: %s", baseBoardScFirmwarePath.c_str());
	}

	if (!JVS_LoadFile((romPath + "\\" + baseBoardEepromPath).c_str(), g_BaseBoardEeprom)) {
		CxbxrAbort("Failed to load base board EEPROM: %s", baseBoardEepromPath.c_str());
	}

	// backup ram is a special case, we can create it automatically if it doesn't exist
	if (!std::filesystem::exists(romPath + "\\" + baseBoardBackupRamPath)) {
		FILE *fp = fopen((romPath + "\\" + baseBoardBackupRamPath).c_str(), "w");
		if (fp == nullptr) {
			CxbxrAbort("Could not create Backup File: %s", baseBoardBackupRamPath.c_str());
		}

		// Create 128kb empty file for backup ram
		fseek(fp, (128 * 1024) - 1, SEEK_SET);
		fputc('\0', fp);
		fclose(fp);
	}

	if (!JVS_LoadFile((romPath + "\\" + baseBoardBackupRamPath).c_str(), g_BaseBoardBackupMemory)) {
		CxbxrAbort("Failed to load base board BACKUP RAM: %s", baseBoardBackupRamPath.c_str());
	}

	// Determine which version of JVS_SendCommand this title is using and derive the offset
	// TODO: Extract this into a function and also locate PINSB
	static int JvsSendCommandVersion = -1;
	g_pPINSA = nullptr;
	g_pPINSB = nullptr;

	auto JvsSendCommandOffset1 = (uintptr_t)GetXboxSymbolPointer("JVS_SendCommand");
	auto JvsSendCommandOffset2 = (uintptr_t)GetXboxSymbolPointer("JVS_SendCommand2");
	auto JvsSendCommandOffset3 = (uintptr_t)GetXboxSymbolPointer("JVS_SendCommand3");

	if (JvsSendCommandOffset1) {
		JvsSendCommandVersion = 1;
		g_pPINSA = *(DWORD**)(JvsSendCommandOffset1 + 0x2A0);
		g_pPINSB = (DWORD*)((DWORD)g_pPINSA - 8);
	}

	if (JvsSendCommandOffset2) {
		JvsSendCommandVersion = 2;
		g_pPINSA = *(DWORD**)(JvsSendCommandOffset2 + 0x312);
		g_pPINSB = (DWORD*)((DWORD)g_pPINSA - 8);
	}

	if (JvsSendCommandOffset3) {
		JvsSendCommandVersion = 3;
		g_pPINSA = *(DWORD**)(JvsSendCommandOffset3 + 0x307);
		g_pPINSB = (DWORD*)((DWORD)g_pPINSA - 8);

		if ((DWORD)g_pPINSA > XBE_MAX_VA) { 
			// This was invalid, we must have the other varient of SendCommand3 (SEGABOOT)
			g_pPINSA = *(DWORD**)(JvsSendCommandOffset3 + 0x302);
			g_pPINSB = (DWORD*)((DWORD)g_pPINSA - 8);
		}
	}

	// Set state to a sane initial default
	ChihiroBaseBoardState.Reset();

	// Auto-Patch Chihiro Region Flag to match the desired game
	uint8_t &region = (uint8_t &)g_BaseBoardQcFirmware[0x1F00];
	auto regionFlags = g_MediaBoard->GetBootId().regionFlags;

	// The region of the system can be converted to a game region flag by doing 1 << region
	// This gives a bitmask that can be ANDed with the BootID region flags to check the games support
	if ((regionFlags & (1 << region)) == 0) {
		// The region was not compatible, so we need to patch the region flag
		// This avoids "Error 05: This game is not acceptable by main board."
		// We use USA,EXPORT,JAPAN to make sure mutiple-language games default to English first
		if (regionFlags & MB_CHIHIRO_REGION_FLAG_USA) {
			region = 2;
		}
		else if (regionFlags & MB_CHIHIRO_REGION_FLAG_EXPORT) {
			region = 3;
		}
		else if (regionFlags & MB_CHIHIRO_REGION_FLAG_JAPAN) {
			region = 1;
		}
	}

	// Spawn the Chihiro/JVS Input Thread
	std::thread(JvsInputThread).detach();
}

DWORD WINAPI xbox::EMUPATCH(JVS_SendCommand)
(
	DWORD a1,
	DWORD Command,
	DWORD a3,
	DWORD Length,
	DWORD a5,
	DWORD a6,
	DWORD a7,
	DWORD a8
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(a1)
		LOG_FUNC_ARG(Command)
		LOG_FUNC_ARG(a3)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(a5)
		LOG_FUNC_ARG(a6)
		LOG_FUNC_ARG(a7)
		LOG_FUNC_ARG(a8)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(0);
}

DWORD WINAPI xbox::EMUPATCH(JvsBACKUP_Read)
(
	DWORD Offset,
	DWORD Length,
	PUCHAR Buffer,
	DWORD a4
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Offset)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(Buffer)
		LOG_FUNC_ARG(a4)
		LOG_FUNC_END

	memcpy((void*)Buffer, &g_BaseBoardBackupMemory[Offset], Length);

	RETURN(0);
}

DWORD WINAPI xbox::EMUPATCH(JvsBACKUP_Write)
(
	DWORD Offset,
	DWORD Length,
	PUCHAR Buffer,
	DWORD a4
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Offset)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(Buffer)
		LOG_FUNC_ARG(a4)
		LOG_FUNC_END

	memcpy(&g_BaseBoardBackupMemory[Offset], (void*)Buffer, Length);

	RETURN(0);
}

DWORD WINAPI xbox::EMUPATCH(JvsEEPROM_Read)
(
	DWORD Offset,
	DWORD Length,
	PUCHAR Buffer,
	DWORD a4
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Offset)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG_OUT(Buffer)
		LOG_FUNC_ARG(a4)
		LOG_FUNC_END

	memcpy((void*)Buffer, &g_BaseBoardEeprom[Offset], Length);

	RETURN(0);
}

DWORD WINAPI xbox::EMUPATCH(JvsEEPROM_Write)
(
	DWORD Offset,
	DWORD Length,
	PUCHAR Buffer,
	DWORD a4
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Offset)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG_OUT(Buffer)
		LOG_FUNC_ARG(a4)
		LOG_FUNC_END

	memcpy(&g_BaseBoardEeprom[Offset], (void*)Buffer, Length);

	std::error_code error;
	g_BaseBoardEeprom.sync(error);

	if (error) {
		EmuLog(LOG_LEVEL::WARNING, "Couldn't sync EEPROM to disk");
	}

	RETURN(0);
}

DWORD WINAPI xbox::EMUPATCH(JvsFirmwareDownload)
(
	DWORD Offset,
	DWORD Length,
	PUCHAR Buffer,
	DWORD a4
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Offset)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG_OUT(Buffer)
		LOG_FUNC_ARG(a4)
		LOG_FUNC_END

	memcpy((void*)Buffer, &g_BaseBoardQcFirmware[Offset], Length);

	RETURN(0);
}


DWORD WINAPI xbox::EMUPATCH(JvsFirmwareUpload)
(
	DWORD Offset,
	DWORD Length,
	PUCHAR Buffer,
	DWORD a4
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Offset)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(Buffer)
		LOG_FUNC_ARG(a4)
		LOG_FUNC_END

	memcpy(&g_BaseBoardQcFirmware[Offset], (void*)Buffer, Length);

	RETURN(0);
}

DWORD WINAPI xbox::EMUPATCH(JvsNodeReceivePacket)
(
	PUCHAR Buffer,
	PDWORD Length,
	DWORD a3
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(Buffer)
		LOG_FUNC_ARG_OUT(Length)
		LOG_FUNC_ARG(a3)
		LOG_FUNC_END

	// Receive the packet from the connected IO board
	uint8_t DeviceId = g_pJvsIo->GetDeviceId();

	// TODO : "Number of packets received" below might imply multiple packets might need receiving here...
	uint16_t payloadSize = (uint16_t)g_pJvsIo->ReceivePacket(&Buffer[6]);
	if (payloadSize > 0) {
		Buffer[0] = 0; // Empty header byte, ignored
		Buffer[1] = 1; // Number of packets received
		Buffer[2] = DeviceId;
		Buffer[3] = 0; // Unused

		*Length = payloadSize + 6;

		// Write the payload size header field
		*((uint16_t*)&Buffer[4]) = payloadSize; // Packet Length (bytes 4-5)
		// TODO : Prevent little/big endian issues here by explicitly setting Buffer[4] and Buffer[5]
	}
		
	RETURN(0);
}

DWORD WINAPI xbox::EMUPATCH(JvsNodeSendPacket)
(
	PUCHAR Buffer,
	DWORD Length,
	DWORD a3
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Buffer)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(a3)
		LOG_FUNC_END

	// Buffer contains two opening bytes, '00' and 'XX', where XX is the number of JVS packets to send
	// Each JVS packet is prepended with a '00' byte, the rest of the packet is as-per the JVS I/O standard.

	// Ignore Buffer[0] (should be 0x00)
	unsigned packetCount = Buffer[1];
	uint8_t* packetPtr = &Buffer[2]; // First JVS packet starts at offset 2;

	for (unsigned i = 0; i < packetCount; i++) {
		// Skip the separator byte (should be 0x00)
		packetPtr++;

		// Send the packet to the connected I/O board
		size_t bytes = g_pJvsIo->SendPacket(packetPtr);

		// Set packetPtr to the next packet
		packetPtr += bytes;
	}

	RETURN(0);
}

// Binary Coded Decimal to Decimal conversion
uint8_t BcdToUint8(uint8_t value)
{
	return value - 6 * (value >> 4);
}

uint8_t Uint8ToBcd(uint8_t value)
{
	return value + 6 * (value / 10);
}

DWORD WINAPI xbox::EMUPATCH(JvsRTC_Read)
(
	DWORD a1,
	DWORD a2,
	JvsRTCTime* pTime,
	DWORD a4
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(a1)
		LOG_FUNC_ARG(a2)
		LOG_FUNC_ARG_OUT(time)
		LOG_FUNC_ARG(a4)
		LOG_FUNC_END

	time_t hostTime;
	struct tm* hostTimeInfo;
	time(&hostTime);
	hostTimeInfo = localtime(&hostTime);

	memset(pTime, 0, sizeof(JvsRTCTime));

	pTime->day = Uint8ToBcd(hostTimeInfo->tm_mday);
	pTime->month = Uint8ToBcd(hostTimeInfo->tm_mon + 1);	// Chihiro month counter stats at 1
	pTime->year = Uint8ToBcd(hostTimeInfo->tm_year - 100);	// Chihiro starts counting from year 2000

	pTime->hour = Uint8ToBcd(hostTimeInfo->tm_hour);
	pTime->minute = Uint8ToBcd(hostTimeInfo->tm_min);
	pTime->second = Uint8ToBcd(hostTimeInfo->tm_sec);

	RETURN(0);
}

DWORD WINAPI xbox::EMUPATCH(JvsRTC_Write)
(
	DWORD a1,
	DWORD a2,
	JvsRTCTime* pTime,
	DWORD a4
	)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(a1)
		LOG_FUNC_ARG(a2)
		LOG_FUNC_ARG_OUT(time)
		LOG_FUNC_ARG(a4)
		LOG_FUNC_END

	LOG_UNIMPLEMENTED();

	RETURN(0);
}

DWORD WINAPI xbox::EMUPATCH(JvsScFirmwareDownload)
(
	DWORD Offset,
	DWORD Length,
	PUCHAR Buffer,
	DWORD a4
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Offset)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG_OUT(Buffer)
		LOG_FUNC_ARG(a4)
		LOG_FUNC_END

	memcpy((void*)Buffer, &g_BaseBoardScFirmware[Offset], Length);

	RETURN(0);
}

DWORD WINAPI xbox::EMUPATCH(JvsScFirmwareUpload)
(
	DWORD Offset,
	DWORD Length,
	PUCHAR Buffer,
	DWORD a4
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Offset)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(Buffer)
		LOG_FUNC_ARG(a4)
		LOG_FUNC_END

	memcpy(&g_BaseBoardScFirmware[Offset], (void*)Buffer, Length);

	RETURN(0);
}

DWORD WINAPI xbox::EMUPATCH(JvsScReceiveMidi)
(
	DWORD a1,
	DWORD a2,
	DWORD a3
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(a1)
		LOG_FUNC_ARG(a2)
		LOG_FUNC_ARG(a3)
		LOG_FUNC_END

	LOG_UNIMPLEMENTED();

	RETURN(0);
}

DWORD WINAPI xbox::EMUPATCH(JvsScSendMidi)
(
	DWORD a1,
	DWORD a2,
	DWORD a3
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(a1)
		LOG_FUNC_ARG(a2)
		LOG_FUNC_ARG(a3)
		LOG_FUNC_END

	LOG_UNIMPLEMENTED();

	RETURN(0);
}

DWORD WINAPI xbox::EMUPATCH(JvsScReceiveRs323c)
(
	PUCHAR Buffer,
	DWORD Length,
	DWORD a3
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Buffer)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(a3)
		LOG_FUNC_END

	LOG_UNIMPLEMENTED();

	RETURN(0);
}


DWORD WINAPI xbox::EMUPATCH(JvsScSendRs323c)
(
	PUCHAR Buffer,
	DWORD Length,
	DWORD a3
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Buffer)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(a3)
		LOG_FUNC_END

	LOG_UNIMPLEMENTED();

	RETURN(0);
}
