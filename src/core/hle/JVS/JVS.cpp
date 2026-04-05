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
#include <mutex>
#include <vector>

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

		Sleep(1); // 1ms poll rate; kept for segaboot timing compatibility
	}
}

#define CHIHIRO_PATH "/EmuMediaBoard/Chihiro/"

void JVS_Init()
{
	// Init Jvs IO board
	g_pJvsIo = new JvsIo(&ChihiroBaseBoardState.JvsSense);
	g_pJvsIo->OpenLog(g_DataFilePath);

	std::string romPath = g_MediaBoardBasePath + std::string("\\Chihiro");
	std::string baseBoardQcFirmwarePath = "ic10_g24lc64.bin";
	std::string baseBoardScFirmwarePath = "pc20_g24lc64.bin";
	std::string baseBoardEepromPath = "ic11_24lc024.bin";
	std::string baseBoardBackupRamPath = "backup_ram.bin";

	if (!JVS_LoadFile((romPath + "\\" + baseBoardQcFirmwarePath).c_str(), g_BaseBoardQcFirmware)) {
		CxbxrAbort("Failed to load base board firmware: " CHIHIRO_PATH "%s", baseBoardQcFirmwarePath.c_str());
	}

	if (!JVS_LoadFile((romPath + "\\" + baseBoardScFirmwarePath).c_str(), g_BaseBoardScFirmware)) {
		CxbxrAbort("Failed to load base board qc firmware: " CHIHIRO_PATH "%s", baseBoardScFirmwarePath.c_str());
	}

	if (!JVS_LoadFile((romPath + "\\" + baseBoardEepromPath).c_str(), g_BaseBoardEeprom)) {
		CxbxrAbort("Failed to load base board EEPROM: " CHIHIRO_PATH "%s", baseBoardEepromPath.c_str());
	}

	// backup ram is a special case, we can create it automatically if it doesn't exist
	if (!std::filesystem::exists(romPath + "\\" + baseBoardBackupRamPath)) {
		FILE *fp = fopen((romPath + "\\" + baseBoardBackupRamPath).c_str(), "w");
		if (fp == nullptr) {
			CxbxrAbort("Could not create Backup File: " CHIHIRO_PATH "%s", baseBoardBackupRamPath.c_str());
		}

		// Create 128kb empty file for backup ram
		fseek(fp, (128 * 1024) - 1, SEEK_SET);
		fputc('\0', fp);
		fclose(fp);
	}

	if (!JVS_LoadFile((romPath + "\\" + baseBoardBackupRamPath).c_str(), g_BaseBoardBackupMemory)) {
		CxbxrAbort("Failed to load base board BACKUP RAM: " CHIHIRO_PATH "%s", baseBoardBackupRamPath.c_str());
	}

	// Determine which version of JVS_SendCommand this title is using and derive the offset
	// TODO: Extract this into a function and also locate PINSB
	static int JvsSendCommandVersion = -1;
	g_pPINSA = (DWORD*)GetXboxSymbolPointer("JVS_g_pPINSA");
	g_pPINSB = (DWORD*)GetXboxSymbolPointer("JVS_g_pPINSB");

	auto JvsSendCommandOffset1 = (uintptr_t)GetXboxSymbolPointer("JVS_SendCommand");
	auto JvsSendCommandOffset2 = (uintptr_t)GetXboxSymbolPointer("JVS_SendCommand2");
	auto JvsSendCommandOffset3 = (uintptr_t)GetXboxSymbolPointer("JVS_SendCommand3");

	if (JvsSendCommandOffset1) {
		JvsSendCommandVersion = 1;
	}

	if (JvsSendCommandOffset2) {
		JvsSendCommandVersion = 2;
	}

	if (JvsSendCommandOffset3) {
		JvsSendCommandVersion = 3;
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

	// === JVS watchdog suppression — "Error 11" / "Error 12" ===
	//
	// Chihiro games (confirmed: Virtua Cop 3, House of the Dead 3) contain a
	// per-node timeout watchdog in the JVS receive loop.  Two counters are
	// incremented every tick inside a do-while:
	//   v5[2]  secondary counter — fires Error 11 when >= 60 and a grace timer >= 600
	//   v5[3]  primary  counter — fires Error 11 immediately when >= 10
	//
	// Our emulated JVS doesn't replicate the exact per-node packet accounting
	// the game expects, so both counters climb until the error fires.
	//
	// Fix: NOP the two writeback instructions so neither counter can ever grow:
	//   89 50 FC  —  MOV [EAX-4], EDX  (v5[2] secondary writeback, 3 bytes)
	//   89 08     —  MOV [EAX],   ECX  (v5[3] primary  writeback, 2 bytes)
	//
	// The surrounding code forms a unique 19-byte signature (verified 1 hit in
	// every tested XBE).  We scan the whole image so this works on any title
	// that links the same JVS library version without needing game-specific
	// address constants.
	//
	// Confirmed games and pattern VA (all bytes at +14/+17 verified as 89 50 FC / 89 08):
	//   Virtua Cop 3              (vc3.xbe)         VA 0x000F50C6
	//   House of the Dead 3       (hod3xb.xbe)      VA 0x0011CC66
	//   Ollie King                (OllieKing.xbe)   VA 0x000EEA26
	//   Ghost Squad               (vsg.xbe)         VA 0x00148626
	//   Wangan Midnight Maximum Tune 1 Export (V307.xbe)     VA 0x000B44D6
	//   Wangan Midnight Maximum Tune 2 Export (V322.xbe)     VA 0x0011C3A6
	//   Wangan Midnight Maximum Tune 2 Japan  (V322.xbe)     VA 0x0011C4F6
	//   Wangan MT1 Export test                (V307TEST.xbe) VA 0x000BB3D6
	//   Wangan MT2 Export test                (V322TEST.xbe) VA 0x00124886
	//   Wangan MT2 Japan  test                (V322TEST.xbe) VA 0x00124A16
	//   Crazy Taxi High Roller    (ctx_ac[r].xbe)   VA 0x000F8E86
	//   Crazy Taxi test           (ctx_ac_t.xbe)    VA 0x00057D56
	//   OutRun 2                  (outrun2.xbe or2) VA 0x000FF0F6
	//   OutRun 2 test             (Testmode.xbe or2) VA 0x0006EE16
	//   OutRun 2 (or2b)           (outrun2.xbe)     VA 0x000F3AC6
	//   OutRun 2 (or2b) test      (Testmode.xbe)    VA 0x0006D296
	//   OutRun 2 SP               (outrun2.xbe)     VA 0x0012EEA6
	//   OutRun 2 SP test          (Testmode.xbe)    VA 0x0006E3E6
	//   Sega Golf Club Network    (golf.xbe)        VA 0x00181FE6
	// No hit (test-menu-only XBEs, no watchdog): vsg_t.xbe, OKTest.xbe, vc3_t.xbe, golf_test.xbe
	if (GetXboxSymbolPointer("JvsNodeSendPacket") != nullptr) {
		static const uint8_t kJvsWatchdogPattern[] = {
			0x8B, 0x48, 0xF8,              // MOV ECX, [EAX-8]
			0x85, 0xC9,                    // TEST ECX, ECX
			0x7E, 0x0E,                    // JLE +14
			0x8B, 0x50, 0xFC,              // MOV EDX, [EAX-4]   (load v5[2])
			0x8B, 0x08,                    // MOV ECX, [EAX]      (load v5[3])
			0x42,                          // INC EDX
			0x41,                          // INC ECX
			0x89, 0x50, 0xFC,              // MOV [EAX-4], EDX   (v5[2] writeback) ← patch +14
			0x89, 0x08                     // MOV [EAX],   ECX   (v5[3] writeback) ← patch +17
		};
		static const size_t kPatternLen = sizeof(kJvsWatchdogPattern);

		const uintptr_t base = XBE_IMAGE_BASE;
		const uintptr_t end  = base + CxbxKrnl_Xbe->m_Header.dwSizeofImage - kPatternLen;

		bool patched = false;
		for (uintptr_t addr = base; addr <= end; addr++) {
			if (memcmp((const void*)addr, kJvsWatchdogPattern, kPatternLen) == 0) {
				// v5[2] writeback: 89 50 FC  → NOP NOP NOP
				const uintptr_t pV2 = addr + 14;
				DWORD oldProtect = 0;
				VirtualProtect((LPVOID)pV2, 3, PAGE_EXECUTE_READWRITE, &oldProtect);
				*(uint8_t*)(pV2 + 0) = 0x90;
				*(uint8_t*)(pV2 + 1) = 0x90;
				*(uint8_t*)(pV2 + 2) = 0x90;
				VirtualProtect((LPVOID)pV2, 3, oldProtect, &oldProtect);
				FlushInstructionCache(GetCurrentProcess(), (LPVOID)pV2, 3);

				// v5[3] writeback: 89 08  → NOP NOP
				const uintptr_t pV3 = addr + 17;
				VirtualProtect((LPVOID)pV3, 2, PAGE_EXECUTE_READWRITE, &oldProtect);
				*(uint8_t*)(pV3 + 0) = 0x90;
				*(uint8_t*)(pV3 + 1) = 0x90;
				VirtualProtect((LPVOID)pV3, 2, oldProtect, &oldProtect);
				FlushInstructionCache(GetCurrentProcess(), (LPVOID)pV3, 2);

				JvsLog("JVS watchdog patch applied: NOP'd v5[2] at 0x%08X and v5[3] at 0x%08X (pattern at 0x%08X)\n",
					(unsigned)pV2, (unsigned)pV3, (unsigned)addr);
				patched = true;
				break;
			}
		}
		if (!patched) {
			JvsLog("JVS watchdog patch: pattern not found in XBE image\n");
		}
	}

	// Spawn the Chihiro/JVS Input Thread
	std::thread(JvsInputThread).detach();
}

#undef CHIHIRO_PATH

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

	// JVS_SendCommand is a higher-level baseboard API used to send management commands
	// to the SC/QC microcontrollers (not the JVS I/O board directly).
	// Command 0x15: "CheckBoardReady" / sense-status query called at init.
	// Returning 0 (failure) causes the game to set an internal "board dead" flag,
	// which later fires Error 11 at a random time. Return 1 (success) instead.
	// a5 = output buffer pointer, a6 = output size pointer (both may be null).
	JvsLog("JVS_SendCommand: a1=0x%08X Command=0x%08X a3=0x%08X Length=%u a5=0x%08X a6=0x%08X a7=0x%08X a8=0x%08X -> returning 1 (success)\n",
		a1, Command, a3, Length, a5, a6, a7, a8);

	// Zero out the result size so the caller knows we produced no data
	if (a6 != 0) {
		*reinterpret_cast<DWORD*>(a6) = 0;
	}

	RETURN(1);
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

		JvsLog("JvsNodeReceivePacket: DeviceId=%u payloadSize=%u header=[%02X %02X %02X %02X %02X %02X]\n",
			DeviceId, payloadSize,
			Buffer[0], Buffer[1], Buffer[2], Buffer[3], Buffer[4], Buffer[5]);
	} else {
		JvsLog("JvsNodeReceivePacket: no payload ready (payloadSize=0)\n");
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
	JvsLog("JvsNodeSendPacket: Length=%u packetCount=%u raw=[%02X %02X %02X %02X %02X %02X %02X %02X]\n",
		Length, packetCount,
		Length > 0 ? Buffer[0] : 0, Length > 1 ? Buffer[1] : 0,
		Length > 2 ? Buffer[2] : 0, Length > 3 ? Buffer[3] : 0,
		Length > 4 ? Buffer[4] : 0, Length > 5 ? Buffer[5] : 0,
		Length > 6 ? Buffer[6] : 0, Length > 7 ? Buffer[7] : 0);
	uint8_t* packetPtr = &Buffer[2]; // First JVS packet starts at offset 2;

	for (unsigned i = 0; i < packetCount; i++) {
		// Skip the separator byte (should be 0x00)
		packetPtr++;

		// Send the packet to the connected I/O board
		size_t bytes = g_pJvsIo->SendPacket(packetPtr);
		JvsLog("JvsNodeSendPacket: packet[%u] consumed %zu bytes\n", i, bytes);

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

	JvsLog("JvsRTC_Write: a1=0x%08X a2=0x%08X [UNIMPLEMENTED]\n", a1, a2);
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

	JvsLog("JvsScReceiveMidi: a1=0x%08X a2=0x%08X a3=0x%08X [UNIMPLEMENTED]\n", a1, a2, a3);
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

	JvsLog("JvsScSendMidi: a1=0x%08X a2=0x%08X a3=0x%08X [UNIMPLEMENTED]\n", a1, a2, a3);
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
