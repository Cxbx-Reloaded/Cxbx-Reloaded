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
// *  (c) 2019 Luke Usher
// *
// *  All rights reserved
// *
// ******************************************************************

#include "JvsIo.h"
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <string>

JvsIo* g_pJvsIo;

//#define DEBUG_JVS_PACKETS
#include <vector>
#include <Windows.h>


// ============================================================================
// JVS logging
// ============================================================================
#ifdef JVS_LOG
FILE* g_JvsLogFile = nullptr;
static ULONGLONG g_JvsLogStartMs = 0;

void JvsLog(const char* fmt, ...)
{
	if (!g_JvsLogFile) return;
	va_list args;
	va_start(args, fmt);
	vfprintf(g_JvsLogFile, fmt, args);
	va_end(args);
	fflush(g_JvsLogFile);
}

// Returns milliseconds elapsed since the JVS log was opened
static ULONGLONG JvsElapsedMs()
{
	return GetTickCount64() - g_JvsLogStartMs;
}

void JvsIo::OpenLog(const std::string& dataFilePath)
{
	if (!dataFilePath.empty()) {
		std::string path = dataFilePath + "\\jvs_io.log";
		g_JvsLogFile = fopen(path.c_str(), "wt");
	}
	if (!g_JvsLogFile) {
		// Fallback: open in working directory
		g_JvsLogFile = fopen("jvs_io.log", "wt");
	}
	g_JvsLogStartMs = GetTickCount64();
	time_t now = time(nullptr);
	JvsLog("JVS I/O log opened %s", ctime(&now));
	JvsLog("BoardID: %s\n", BoardID.c_str());
	JvsLog("Emulated: CommandFormatRevision=0x%02X JvsVersion=0x%02X CommVersion=0x%02X\n\n",
		CommandFormatRevision, JvsVersion, CommunicationVersion);
}
#else
void JvsIo::OpenLog(const std::string& /*dataFilePath*/) {}
#endif

// Returns a human-readable name for a JVS command byte
static const char* JvsCommandName(uint8_t cmd)
{
	switch (cmd) {
		case 0xF0: return "F0_Reset";
		case 0xF1: return "F1_SetDeviceId";
		case 0x10: return "10_GetBoardId";
		case 0x11: return "11_GetCommandFormat";
		case 0x12: return "12_GetJvsRevision";
		case 0x13: return "13_GetCommunicationVersion";
		case 0x14: return "14_GetCapabilities";
		case 0x15: return "15_ConveyMainBoardId";
		case 0x20: return "20_ReadSwitchInputs";
		case 0x21: return "21_ReadCoinInputs";
		case 0x22: return "22_ReadAnalogInputs";
		case 0x26: return "26_ReadMiscSwitchInputs";
		case 0x2E: return "2E_ReadPayoutHopperStatus";
		case 0x2F: return "2F_RetransmitData";
		case 0x30: return "30_CoinDecrease";
		case 0x31: return "31_CoinIncrease";
		case 0x32: return "32_GeneralPurposeOutput";
		case 0x33: return "33_AnalogOutput";
		case 0x34: return "34_CharacterOutput";
		case 0x36: return "36_PayoutSubtractionOutput";
		case 0x37: return "37_GeneralPurposeOutput2";
		case 0x70: return "70_NamcoCustom";
		default:   return "??_Unknown";
	}
}

static void LogPacketHex(const char* prefix, const uint8_t* data, size_t len)
{
	JvsLog("%s (%zu bytes):", prefix, len);
	for (size_t i = 0; i < len; i++) JvsLog(" %02X", data[i]);
	JvsLog("\n");
}

// We will emulate SEGA 837-13551 IO Board
JvsIo::JvsIo(uint8_t* sense)
{
	pSense = sense;

	// Version info BCD Format: X.X
	CommandFormatRevision = 0x11;
	JvsVersion = 0x20;
	CommunicationVersion = 0x10;

	BoardID = "SEGA ENTERPRISES,LTD.;I/O BD JVS;837-13551";
}

void JvsIo::Update()
{
	// Handle coin input
		static bool previousCoinButtonsState = false;
		bool currentCoinButtonState = GetAsyncKeyState('5');
		if (currentCoinButtonState && !previousCoinButtonsState) {
			Inputs.coins[0].coins += 1;
		}
		previousCoinButtonsState = currentCoinButtonState;

	// TODO: Update Jvs inputs based on user configuration
	// For now, hardcode the inputs for the game we are currently testing (Ollie King)
	Inputs.switches.player[0].start = GetAsyncKeyState('1');                                                        // Start
	Inputs.analog[1].value = GetAsyncKeyState(VK_LEFT) ? 0x9000 : (GetAsyncKeyState(VK_RIGHT) ? 0x7000 : 0x8000);   // Board Swing
	Inputs.switches.player[0].up = GetAsyncKeyState(VK_UP);                                                         // Board Front
	Inputs.switches.player[0].down = GetAsyncKeyState(VK_DOWN);                                                     // Board Rear
	Inputs.switches.player[0].button[0] = GetAsyncKeyState('A');                                                    // Left Button
	Inputs.switches.player[0].button[1] = GetAsyncKeyState('S');                                                    // Right Button
}

uint8_t JvsIo::GetDeviceId()
{
	return BroadcastPacket ? 0x00 : DeviceId;
}

int JvsIo::Jvs_Command_F0_Reset(uint8_t* data)
{
	uint8_t ensure_reset = data[1];

	if (ensure_reset == 0xD9) {
		// Set sense to 3 (2.5v) to instruct the baseboard we're ready.
		uint8_t prevSense = *pSense;
		*pSense = 3;
		ResponseBuffer.push_back(ReportCode::Handled); // Note : Without this, Chihiro software stops sending packets (but JVS V3 doesn't send this?)
		DeviceId = 0;
		JvsLog("  F0_Reset: sense %u->3 (2.5V, ready), DeviceId reset to 0\n", prevSense);
	}
#if 0 // TODO : Is the following required?
	else {
		ResponseBuffer.push_back(ReportCode::InvalidParameter);
	}
#endif

#if 0 // TODO : Is the following required?
	// Detect a consecutive reset
	if (data[2] == 0xF0) {
		// TODO : Probably ensure the second reset too : if (data[3] == 0xD9) {
		// TODO : Handle two consecutive reset's here?

		return 3;
	}
#endif

	return 1;
}

int JvsIo::Jvs_Command_F1_SetDeviceId(uint8_t* data)
{
	// Set Address
	uint8_t prevId = DeviceId;
	DeviceId = data[1];

	uint8_t prevSense = *pSense;
	*pSense = 0; // Set sense to 0v
	ResponseBuffer.push_back(ReportCode::Handled);
	JvsLog("  F1_SetDeviceId: DeviceId %u->%u, sense %u->0 (0V, assigned)\n", prevId, DeviceId, prevSense);

	return 1;
}

int JvsIo::Jvs_Command_10_GetBoardId()
{
	// Get Board ID
	ResponseBuffer.push_back(ReportCode::Handled);

	for (char& c : BoardID) {
		ResponseBuffer.push_back(c);
	}

	return 0;
}

int JvsIo::Jvs_Command_11_GetCommandFormat()
{
	ResponseBuffer.push_back(ReportCode::Handled);
	ResponseBuffer.push_back(CommandFormatRevision);

	return 0;
}

int JvsIo::Jvs_Command_12_GetJvsRevision()
{
	ResponseBuffer.push_back(ReportCode::Handled);
	ResponseBuffer.push_back(JvsVersion);

	return 0;
}

int JvsIo::Jvs_Command_13_GetCommunicationVersion()
{
	ResponseBuffer.push_back(ReportCode::Handled);
	ResponseBuffer.push_back(CommunicationVersion);

	return 0;
}

int JvsIo::Jvs_Command_14_GetCapabilities()
{
	ResponseBuffer.push_back(ReportCode::Handled);

	// Capabilities list (4 bytes each)

	// Input capabilities
	ResponseBuffer.push_back(CapabilityCode::PlayerSwitchButtonSets);
	ResponseBuffer.push_back(JVS_MAX_PLAYERS); // number of players
	ResponseBuffer.push_back(13); // 13 button switches per player
	ResponseBuffer.push_back(0);

	ResponseBuffer.push_back(CapabilityCode::CoinSlots);
	ResponseBuffer.push_back(JVS_MAX_COINS); // number of coin slots
	ResponseBuffer.push_back(0);
	ResponseBuffer.push_back(0);

	ResponseBuffer.push_back(CapabilityCode::AnalogInputs);
	ResponseBuffer.push_back(JVS_MAX_ANALOG); // number of analog input channels
	ResponseBuffer.push_back(16); // 16 bits per analog input channel
	ResponseBuffer.push_back(0);

	// Output capabilities
	ResponseBuffer.push_back(CapabilityCode::GeneralPurposeOutputs);
	ResponseBuffer.push_back(6); // number of outputs
	ResponseBuffer.push_back(0);
	ResponseBuffer.push_back(0);

	ResponseBuffer.push_back(CapabilityCode::EndOfCapabilities);

	return 0;
}

int JvsIo::Jvs_Command_15_ConveyMainBoardId(uint8_t* data, size_t remaining)
{
	// Main board sends its ID as a null-terminated string
	// We just acknowledge it
	ResponseBuffer.push_back(ReportCode::Handled);
	ResponseBuffer.push_back(0x01);
	ResponseBuffer.push_back(0x05);
	// Scan past the null-terminated string starting at data[1]
	int consumed = 0;
	for (size_t j = 1; j < remaining; j++) {
		consumed++;
		if (data[j] == 0x00) {
			break;
		}
	}

	return consumed;
}

int JvsIo::Jvs_Command_20_ReadSwitchInputs(uint8_t* data)
{
	static jvs_switch_player_inputs_t default_switch_player_input;
	uint8_t nr_switch_players = data[1];
	uint8_t bytesPerSwitchPlayerInput = data[2];

	ResponseBuffer.push_back(ReportCode::Handled);

	ResponseBuffer.push_back(Inputs.switches.system.GetByte0());

	for (int i = 0; i < nr_switch_players; i++) {
		for (int j = 0; j < bytesPerSwitchPlayerInput; j++) {
			// If a title asks for more switch player inputs than we support, pad with dummy data
			jvs_switch_player_inputs_t &switch_player_input = (i >= JVS_MAX_PLAYERS) ? default_switch_player_input : Inputs.switches.player[i];
			uint8_t value
				= (j == 0) ? switch_player_input.GetByte0()
				: (j == 1) ? switch_player_input.GetByte1()
				: 0; // Pad any remaining bytes with 0, as we don't have that many inputs available
			ResponseBuffer.push_back(value);
		}
	}

	return 2;
}

int JvsIo::Jvs_Command_21_ReadCoinInputs(uint8_t* data)
{
	static jvs_coin_slots_t default_coin_slot;
	uint8_t nr_coin_slots = data[1];
	
	ResponseBuffer.push_back(ReportCode::Handled);

	for (int i = 0; i < nr_coin_slots; i++) {
		const uint8_t bytesPerCoinSlot = 2;
		for (int j = 0; j < bytesPerCoinSlot; j++) {
			// If a title asks for more coin slots than we support, pad with dummy data
			jvs_coin_slots_t &coin_slot = (i >= JVS_MAX_COINS) ? default_coin_slot : Inputs.coins[i];
			uint8_t value
				= (j == 0) ? coin_slot.GetByte0()
				: (j == 1) ? coin_slot.GetByte1()
				: 0; // Pad any remaining bytes with 0, as we don't have that many inputs available
			ResponseBuffer.push_back(value);
		}
	}

	return 1;
}

int JvsIo::Jvs_Command_22_ReadAnalogInputs(uint8_t* data)
{
	static jvs_analog_input_t default_analog;
	uint8_t nr_analog_inputs = data[1];

	ResponseBuffer.push_back(ReportCode::Handled);

	for (int i = 0; i < nr_analog_inputs; i++) {
		const uint8_t bytesPerAnalogInput = 2;
		for (int j = 0; j < bytesPerAnalogInput; j++) {
			// If a title asks for more analog input than we support, pad with dummy data
			jvs_analog_input_t &analog_input = (i >= JVS_MAX_ANALOG) ? default_analog : Inputs.analog[i];
			uint8_t value
				= (j == 0) ? analog_input.GetByte0()
				: (j == 1) ? analog_input.GetByte1()
				: 0; // Pad any remaining bytes with 0, as we don't have that many inputs available
			ResponseBuffer.push_back(value);
		}
	}

	return 1;
}

int JvsIo::Jvs_Command_26_ReadMiscSwitchInputs(uint8_t* data)
{
	uint8_t nr_bytes = data[1];

	ResponseBuffer.push_back(ReportCode::Handled);

	// Return requested number of bytes (all zeros - no misc switches active)
	for (int i = 0; i < nr_bytes; i++) {
		ResponseBuffer.push_back(0x00);
	}

	return 1;
}

int JvsIo::Jvs_Command_2E_ReadPayoutHopperStatus(uint8_t* data)
{
	uint8_t nr_slots = data[1];

	ResponseBuffer.push_back(ReportCode::Handled);

	// Return 4 bytes of hopper status per slot (all zeros = normal)
	for (int i = 0; i < nr_slots; i++) {
		ResponseBuffer.push_back(0x00);
		ResponseBuffer.push_back(0x00);
		ResponseBuffer.push_back(0x00);
		ResponseBuffer.push_back(0x00);
	}

	return 1;
}

int JvsIo::Jvs_Command_2F_RetransmitData()
{
	ResponseBuffer.push_back(ReportCode::Handled);

	return 0;
}

int JvsIo::Jvs_Command_30_CoinDecrease(uint8_t* data)
{
	uint8_t coinSlot = data[1];
	uint16_t coinCount = (data[2] << 8) | data[3];

	ResponseBuffer.push_back(ReportCode::Handled);

	// JVS coin slots are 1-based
	uint8_t slotIndex = coinSlot - 1;
	if (slotIndex < JVS_MAX_COINS) {
		if (Inputs.coins[slotIndex].coins >= coinCount) {
			Inputs.coins[slotIndex].coins -= coinCount;
		} else {
			Inputs.coins[slotIndex].coins = 0;
		}
	}

	return 3;
}

int JvsIo::Jvs_Command_32_GeneralPurposeOutput(uint8_t* data)
{
	uint8_t banks = data[1];

	ResponseBuffer.push_back(ReportCode::Handled);

	// TODO: Handle output

	// Input data size is 1 byte indicating the number of banks, followed by one byte per bank
	return 1 + banks;
}

int JvsIo::Jvs_Command_33_AnalogOutput(uint8_t* data)
{
	uint8_t channels = data[1];

	ResponseBuffer.push_back(ReportCode::Handled);

	// TODO: Handle analog output
	// Each channel has 2 bytes of data following the channel count
	return 1 + (channels * 2);
}

int JvsIo::Jvs_Command_34_CharacterOutput(uint8_t* data)
{
	uint8_t byteCount = data[1];

	ResponseBuffer.push_back(ReportCode::Handled);

	// TODO: Handle character output
	return 1 + byteCount;
}

int JvsIo::Jvs_Command_36_PayoutSubtractionOutput(uint8_t* data)
{
	ResponseBuffer.push_back(ReportCode::Handled);

	// TODO: Handle payout subtraction output
	// Fixed 3 parameter bytes (slot, count high, count low)
	return 3;
}

int JvsIo::Jvs_Command_37_GeneralPurposeOutput2(uint8_t* data)
{
	ResponseBuffer.push_back(ReportCode::Handled);

	// TODO: Handle general purpose output 2
	// Fixed 2 parameter bytes
	return 2;
}

int JvsIo::Jvs_Command_70_NamcoCustom(uint8_t* data, size_t remaining)
{
	uint8_t subCommand = data[1];

	switch (subCommand) {
		case 0x18:
			ResponseBuffer.push_back(ReportCode::Handled);
			// Consumes entire remaining packet data
			return (int)remaining - 1;
		case 0x05:
			ResponseBuffer.push_back(ReportCode::Handled);
			// data[2] specifies the total bytes to consume
			return data[2] - 1;
		case 0x03:
			ResponseBuffer.push_back(ReportCode::Handled);
			ResponseBuffer.push_back(0x00);
			return 3;
		case 0x15:
		case 0x16:
			ResponseBuffer.push_back(ReportCode::Handled);
			return 3;
		default:
			ResponseBuffer.push_back(ReportCode::Handled);
			printf("JvsIo: Unknown Namco sub-command 0x70:%02X\n", subCommand);
			return 1;
	}
}

int JvsIo::Jvs_Command_78_80_SkipNamcoUnknownCustom()
{
	ResponseBuffer.push_back(ReportCode::Handled);

	// Skip 14 additional parameter bytes (15 total including command byte)
	return 14;
}

uint8_t JvsIo::GetByte(uint8_t* &buffer)
{
	uint8_t value = *buffer++;
#ifdef DEBUG_JVS_PACKETS
	printf(" %02X", value);
#endif
	return value;
}

uint8_t JvsIo::GetEscapedByte(uint8_t* &buffer)
{
	uint8_t value = GetByte(buffer);

	// Special case: 0xD0 is an exception byte that actually returns the next byte + 1
	if (value == ESCAPE_BYTE) {
		value = GetByte(buffer) + 1;
	}

	return value;
}

void JvsIo::HandlePacket(jvs_packet_header_t* header, std::vector<uint8_t>& packet)
{
	// It's possible for a JVS packet to contain multiple commands, so we must iterate through it
	ResponseBuffer.push_back(StatusCode::StatusOkay); // Assume we'll handle the command just fine
	for (size_t i = 0; i < packet.size(); i++) {

		BroadcastPacket = packet[i] >= 0xF0; // Set a flag when broadcast packet

		uint8_t* command_data = &packet[i];
		switch (packet[i]) {
			// Broadcast Commands
			case 0xF0: i += Jvs_Command_F0_Reset(command_data); break;
			case 0xF1: i += Jvs_Command_F1_SetDeviceId(command_data); break;
			// Init Commands
			case 0x10: i += Jvs_Command_10_GetBoardId(); break;
			case 0x11: i += Jvs_Command_11_GetCommandFormat();	break;
			case 0x12: i += Jvs_Command_12_GetJvsRevision(); break;
			case 0x13: i += Jvs_Command_13_GetCommunicationVersion(); break;
			case 0x14: i += Jvs_Command_14_GetCapabilities(); break;
			case 0x15: i += Jvs_Command_15_ConveyMainBoardId(command_data, packet.size() - i); break;
			// Data I/O Commands
			case 0x20: i += Jvs_Command_20_ReadSwitchInputs(command_data); break;
			case 0x21: i += Jvs_Command_21_ReadCoinInputs(command_data); break;
			case 0x22: i += Jvs_Command_22_ReadAnalogInputs(command_data); break;
			case 0x26: i += Jvs_Command_26_ReadMiscSwitchInputs(command_data); break;
			case 0x2E: i += Jvs_Command_2E_ReadPayoutHopperStatus(command_data); break;
			case 0x2F: i += Jvs_Command_2F_RetransmitData(); break;
			case 0x30:
			case 0x31: i += Jvs_Command_30_CoinDecrease(command_data); break;
			// Output Commands
			case 0x32: i += Jvs_Command_32_GeneralPurposeOutput(command_data); break;
			case 0x33: i += Jvs_Command_33_AnalogOutput(command_data); break;
			case 0x34: i += Jvs_Command_34_CharacterOutput(command_data); break;
			case 0x36: i += Jvs_Command_36_PayoutSubtractionOutput(command_data); break;
			case 0x37: i += Jvs_Command_37_GeneralPurposeOutput2(command_data); break;
			// Manufacturer-specific Commands
			case 0x70: i += Jvs_Command_70_NamcoCustom(command_data, packet.size() - i); break;
			case 0x78:
			case 0x79:
			case 0x7A:
			case 0x7B:
			case 0x7C:
			case 0x7D:
			case 0x7E:
			case 0x7F:
			case 0x80: i += Jvs_Command_78_80_SkipNamcoUnknownCustom(); break;
			default:
				// Do NOT set UnsupportedCommand in the packet status byte — that signals to the game that the
				// IO board is entirely disconnected (causes "Error 11 JVS I/O not connected" in Virtua Cop 3
				// when the cabinet sends a header-light LED command with no registered handler).
				// Instead, append InvalidParameter in the report for this specific command so the game sees
				// a valid response and knows the board is alive.  We must still stop processing further
				// commands in this packet because we don't know the unknown command's parameter byte count.
				ResponseBuffer.push_back(ReportCode::InvalidParameter);
				JvsLog("  UNKNOWN command 0x%02X at offset %zu — returning InvalidParameter (board stays connected)\n", packet[i], i);
				printf("JvsIo::HandlePacket: Unhandled Command %02X (acknowledged with InvalidParameter)\n", packet[i]);
				return;
		}
	}
}

size_t JvsIo::SendPacket(uint8_t* buffer)
{
	std::lock_guard<std::mutex> lock(IoBoardMutex);
	// Remember where the buffer started (so we can calculate the number of bytes we've handled)
	uint8_t* buffer_start = buffer;

	// Scan the packet header
	jvs_packet_header_t header;

	// First, read the sync byte
#ifdef DEBUG_JVS_PACKETS
	printf("JvsIo::SendPacket:");
#endif
	header.sync = GetByte(buffer); // Do not unescape the sync-byte!
	if (header.sync != SYNC_BYTE) {
#ifdef DEBUG_JVS_PACKETS
		printf(" [Missing SYNC_BYTE!]\n");
#endif
		// If it's wrong, return we've processed (actually, skipped) one byte
		return 1;
	}

	// Read the target and count bytes.
	// NOTE: JVS over Chihiro USB is a raw byte stream — the escape-byte mechanism (0xD0)
	// is a physical RS-485 layer concern and is NOT applied by the Xbox USB driver.
	// Using GetEscapedByte here would misinterpret any data byte that happens to equal
	// ESCAPE_BYTE (0xD0), consuming the following byte (often the real checksum) as the
	// escape suffix and cascading the mis-alignment across several subsequent packets.
	header.target = GetByte(buffer);
	header.count = GetByte(buffer);

	// Calculate the checksum
	uint8_t actual_checksum = header.target + header.count;

	// Decode the payload data
	std::vector<uint8_t> packet;
	for (int i = 0; i < header.count - 1; i++) { // Note : -1 to avoid adding the checksum byte to the packet
		uint8_t value = GetByte(buffer);
		packet.push_back(value);
		actual_checksum += value;
	}

	// Read the checksum from the last byte (raw — not escaped, see note above)
	uint8_t packet_checksum = GetByte(buffer);
#ifdef DEBUG_JVS_PACKETS
	printf("\n");
#endif

	// Verify checksum - skip packet if invalid
	ResponseBuffer.clear();
	if (packet_checksum != actual_checksum) {
		ResponseBuffer.push_back(StatusCode::ChecksumError);
		JvsLog("SendPacket: CHECKSUM ERROR target=0x%02X count=%u expected=0x%02X got=0x%02X — discarding packet\n",
			header.target, header.count, actual_checksum, packet_checksum);
	} else {
		// If the packet was intended for us, we need to handle it
		if (header.target == TARGET_BROADCAST || header.target == DeviceId) {
			// Log full raw packet bytes
			JvsLog("[+%llums] SendPacket -> target=0x%02X (DeviceId=%u) count=%u payload(%zu):",
				JvsElapsedMs(), header.target, DeviceId, header.count, packet.size());
			for (size_t pi = 0; pi < packet.size(); pi++)
				JvsLog(" %02X", packet[pi]);
			JvsLog("\n");
			HandlePacket(&header, packet);
		} else {
			JvsLog("SendPacket: packet for target=0x%02X ignored (our DeviceId=%u)\n",
				header.target, DeviceId);
		}
	}

	// Calculate and return the total packet size including header
	size_t total_packet_size = buffer - buffer_start;

	return total_packet_size;
}

void JvsIo::SendByte(uint8_t* &buffer, uint8_t value)
{
	*buffer++ = value;
}

void JvsIo::SendEscapedByte(uint8_t* &buffer, uint8_t value)
{
	// Special case: Send an exception byte followed by value - 1
	if (value == SYNC_BYTE || value == ESCAPE_BYTE) {
		SendByte(buffer, ESCAPE_BYTE);
		value--;
	}

	SendByte(buffer, value);
}

size_t JvsIo::ReceivePacket(uint8_t* buffer)
{
	std::lock_guard<std::mutex> lock(IoBoardMutex);
	if (ResponseBuffer.empty()) {
		return 0;
	}

	// Build a JVS response packet containing the payload
	jvs_packet_header_t header;
	header.sync = SYNC_BYTE;
	header.target = TARGET_MASTER_DEVICE;
	header.count = (uint8_t)ResponseBuffer.size() + 1; // Set data size to payload + 1 checksum byte
	// TODO : What if count overflows (meaning : responses are bigger than 255 bytes); Should we split it over multiple packets??

	// Remember where the buffer started (so we can calculate the number of bytes we've send)
	uint8_t* buffer_start = buffer;

	// Send the header bytes
	SendByte(buffer, header.sync); // Do not escape the sync byte!
	SendByte(buffer, header.target);
	SendByte(buffer, header.count);

	// Calculate the checksum
	uint8_t packet_checksum = header.target + header.count;

	// Encode the payload data
	for (size_t i = 0; i < ResponseBuffer.size(); i++) {
		uint8_t value = ResponseBuffer[i];
		SendByte(buffer, value);
		packet_checksum += value;
	}

	// Write the checksum to the last byte
	SendByte(buffer, packet_checksum);

	ResponseBuffer.clear();

	// Calculate an return the total packet size including header
	size_t total_packet_size = buffer - buffer_start;
#ifdef DEBUG_JVS_PACKETS

	printf("JvsIo::ReceivePacket:");
	for (size_t i = 0; i < total_packet_size; i++) {
		printf(" %02X", buffer_start[i]);
	}

	printf("\n");
#endif
	// Log outgoing response
#ifdef JVS_LOG
	if (g_JvsLogFile && total_packet_size > 0) {
		JvsLog("[+%llums] ", JvsElapsedMs());
		LogPacketHex("ReceivePacket <-", buffer_start, total_packet_size);
	}
#endif
	return total_packet_size;
}
