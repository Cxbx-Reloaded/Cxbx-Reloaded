// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *  This file is part of the Cxbx-Reloaded project.
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
// *  All rights reserved
// *
// ******************************************************************

#define LOG_PREFIX CXBXR_MODULE::CXBXR
#define LOG_PREFIX_INIT CXBXR_MODULE::INIT

#include <cstdint>
#include <string>
#include <vector>

#include "core\kernel\init\CxbxKrnl.h"

static std::vector<xbox::addr_xt> g_RdtscPatches;

#define OPCODE_PATCH_RDTSC 0x90EF  // OUT DX, EAX; NOP

bool IsRdtscInstruction(xbox::addr_xt addr)
{
	// First the fastest check - does addr contain exact patch from PatchRdtsc?
	// Second check - is addr on the rdtsc patch list?
	return (*(uint16_t*)addr == OPCODE_PATCH_RDTSC)
		// Note : It's not needed to check for g_SkipRdtscPatching,
		// as when that's set, the g_RdtscPatches vector will be empty
		// anyway, failing this lookup :
		&& (std::find(g_RdtscPatches.begin(), g_RdtscPatches.end(), addr) != g_RdtscPatches.end());
}

static void PatchRdtsc(xbox::addr_xt addr)
{
	// Patch away rdtsc with an opcode we can intercept
	// We use a privilaged instruction rather than int 3 for debugging
	// When using int 3, attached debuggers trap and rdtsc is used often enough
	// that it makes Cxbx-Reloaded unusable
	// A privilaged instruction (like OUT) does not suffer from this
	EmuLogInit(LOG_LEVEL::DEBUG, "Patching rdtsc opcode at 0x%.8X", (DWORD)addr);
	*(uint16_t*)addr = OPCODE_PATCH_RDTSC;
	g_RdtscPatches.push_back(addr);
}

static const uint8_t rdtsc_pattern[] = {
	0x89,//{ 0x0F,0x31,0x89 },   // two false positives in Stranger's Wrath
	0xC3,//{ 0x0F,0x31,0xC3 },
	0x8B,//{ 0x0F,0x31,0x8B },   // one false positive in Sonic Rider .text 88 5C 0F 31, two false positives in Stranger's Wrath
	0xB9,//{ 0x0F,0x31,0xB9 },
	0xC7,//{ 0x0F,0x31,0xC7 },
	0x8D,//{ 0x0F,0x31,0x8D },
	0x68,//{ 0x0F,0x31,0x68 },
	0x5A,//{ 0x0F,0x31,0x5A },
	0x29,//{ 0x0F,0x31,0x29 },
	0xF3,//{ 0x0F,0x31,0xF3 },
	0xE9,//{ 0x0F,0x31,0xE9 },
	0x2B,//{ 0x0F,0x31,0x2B },
	0x50,//{ 0x0F,0x31,0x50 },	// 0x50 only used in ExaSkeleton .text , but encounter false positive in RalliSport .text 83 E2 0F 31
	0x0F,//{ 0x0F,0x31,0x0F },
	0x3B,//{ 0x0F,0x31,0x3B },
	0xD9,//{ 0x0F,0x31,0xD9 },
	0x57,//{ 0x0F,0x31,0x57 },
	0xB9,//{ 0x0F,0x31,0xB9 },
	0x85,//{ 0x0F,0x31,0x85 },
	0x83,//{ 0x0F,0x31,0x83 },
	0x33,//{ 0x0F,0x31,0x33 },
	0xF7,//{ 0x0F,0x31,0xF7 }, // one false positive in Stranger's Wrath
	0x8A,//{ 0x0F,0x31,0x8A }, // 8A and 56 only apears in RalliSport 2 .text , need to watch whether any future false positive.
	0x56,//{ 0x0F,0x31,0x56 }
	0x6A,                      // 6A, 39, EB, F6, A1, 01 only appear in Unreal Championship, 01 is at WMVDEC section
	0x39,
	0xEB,
	0xF6,
	0xA1,
	0x01,                      // one false positive in Group S Challenge [1.05] .text E8 0F 31 01 00
	0xA3
};
static const int sizeof_rdtsc_pattern = sizeof(rdtsc_pattern);

void PatchRdtscInstructions()
{
	uint8_t rdtsc[2] = { 0x0F, 0x31 };

	// Iterate through each CODE section
	for (uint32_t sectionIndex = 0; sectionIndex < CxbxKrnl_Xbe->m_Header.dwSections; sectionIndex++) {
		if (!CxbxKrnl_Xbe->m_SectionHeader[sectionIndex].dwFlags.bExecutable) {
			continue;
		}

		// Skip some segments known to never contain rdtsc (to avoid false positives)
		if (std::string(CxbxKrnl_Xbe->m_szSectionName[sectionIndex]) == "DSOUND"
			|| std::string(CxbxKrnl_Xbe->m_szSectionName[sectionIndex]) == "XGRPH"
			|| std::string(CxbxKrnl_Xbe->m_szSectionName[sectionIndex]) == ".data"
			|| std::string(CxbxKrnl_Xbe->m_szSectionName[sectionIndex]) == ".rdata"
			|| std::string(CxbxKrnl_Xbe->m_szSectionName[sectionIndex]) == "XMV"
			|| std::string(CxbxKrnl_Xbe->m_szSectionName[sectionIndex]) == "XONLINE"
			|| std::string(CxbxKrnl_Xbe->m_szSectionName[sectionIndex]) == "MDLPL") {
			continue;
		}

		EmuLogInit(LOG_LEVEL::INFO, "Searching for rdtsc in section %s", CxbxKrnl_Xbe->m_szSectionName[sectionIndex]);
		xbox::addr_xt startAddr = CxbxKrnl_Xbe->m_SectionHeader[sectionIndex].dwVirtualAddr;
		//rdtsc is two bytes instruction, it needs at least one opcode byte after it to finish a function, so the endAddr need to substract 3 bytes.
		xbox::addr_xt endAddr = startAddr + CxbxKrnl_Xbe->m_SectionHeader[sectionIndex].dwSizeofRaw - 3;
		for (xbox::addr_xt addr = startAddr; addr <= endAddr; addr++)
		{
			if (memcmp((void*)addr, rdtsc, 2) == 0)
			{
				uint8_t next_byte = *(uint8_t*)(addr + 2);
				// If the following byte matches the known pattern.
				int i = 0;
				for (i = 0; i < sizeof_rdtsc_pattern; i++)
				{
					if (next_byte == rdtsc_pattern[i])
					{
						if (next_byte == 0x8B)
						{
							if (*(uint8_t*)(addr - 2) == 0x88 && *(uint8_t*)(addr - 1) == 0x5C)
							{
								EmuLogInit(LOG_LEVEL::INFO, "Skipped false positive: rdtsc pattern  0x%.2X, @ 0x%.8X", next_byte, (DWORD)addr);
								continue;
							}

							if (*(uint8_t*)(addr - 2) == 0x24 && *(uint8_t*)(addr - 1) == 0x0C)
							{
								EmuLogInit(LOG_LEVEL::INFO, "Skipped false positive: rdtsc pattern  0x%.2X, @ 0x%.8X", next_byte, (DWORD)addr);
								continue;
							}
						}
						if (next_byte == 0x89)
						{
							if (*(uint8_t*)(addr + 4) == 0x8B && *(uint8_t*)(addr - 5) == 0x04)
							{
								EmuLogInit(LOG_LEVEL::INFO, "Skipped false positive: rdtsc pattern  0x%.2X, @ 0x%.8X", next_byte, (DWORD)addr);
								continue;
							}

						}
						if (next_byte == 0x50)
						{
							if (*(uint8_t*)(addr - 2) == 0x83 && *(uint8_t*)(addr - 1) == 0xE2)
							{
								EmuLogInit(LOG_LEVEL::INFO, "Skipped false positive: rdtsc pattern  0x%.2X, @ 0x%.8X", next_byte, (DWORD)addr);
								continue;
							}

						}
						if (next_byte == 0x01)
						{
							if (*(uint8_t*)(addr - 1) == 0xE8 && *(uint8_t*)(addr + 3) == 0x00)
							{
								EmuLogInit(LOG_LEVEL::INFO, "Skipped false positive: rdtsc pattern  0x%.2X, @ 0x%.8X", next_byte, (DWORD)addr);
								continue;
							}

						}
						if (next_byte == 0xF7)
						{
							if (*(uint8_t*)(addr - 1) == 0xE8 && *(uint8_t*)(addr + 3) == 0xFF)
							{
								EmuLogInit(LOG_LEVEL::INFO, "Skipped false positive: rdtsc pattern  0x%.2X, @ 0x%.8X", next_byte, (DWORD)addr);
								continue;
							}

						}
						PatchRdtsc(addr);
						//the first for loop already increment addr per loop. we only increment one more time so the addr will point to the byte next to the found rdtsc instruction. this is important since there is at least one case that two rdtsc instructions are next to each other.
						addr += 1;
						//if a match found, break the pattern matching loop and keep looping addr for next rdtsc.
						break;
					}
				}
				if (i >= sizeof_rdtsc_pattern)
				{
					//no pattern matched, keep record for detections we treat as non-rdtsc for future debugging.
					EmuLogInit(LOG_LEVEL::INFO, "Skipped potential rdtsc: Unknown opcode pattern  0x%.2X, @ 0x%.8X", next_byte, (DWORD)addr);
				}
			}
		}
	}

	EmuLogInit(LOG_LEVEL::INFO, "Done patching rdtsc, total %d rdtsc instructions patched", g_RdtscPatches.size());
}
