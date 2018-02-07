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
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase.cpp
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
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#define _XBOXKRNL_DEFEXTRN_

#undef FIELD_OFFSET     // prevent macro redefinition warnings
#include <windows.h>

#include "CxbxKrnl.h" // For xbaddr
#include "HLEDataBase.h" // For PairScanLibSec

const char *Lib_D3D8 = "D3D8";
const char *Sec_D3D = "D3D";
const char *Lib_D3D8LTCG = "D3D8LTCG";
const char *Lib_D3DX8 = "D3DX8";
const char *Sec_D3DX = "D3DX";
const char *Lib_DSOUND = "DSOUND";
const char *Sec_DSOUND = Lib_DSOUND;
const char *Lib_XACTENG = "XACTENG";
const char *Sec_XACTENG = Lib_XACTENG;
const char *Lib_XAPILIB = "XAPILIB";
const char *Sec_XPP = "XPP";
const char *Sec_text = ".text";
const char *Lib_XGRAPHC = "XGRAPHC";
const char *Sec_XGRPH = "XGRPH";
const char *Lib_XNET = "XNET";
const char *Lib_XNETN = "XNETN";
const char *Lib_XNETS = "XNETS";
const char *Lib_XONLINE = "XONLINE"; // TODO : Typo for XONLINES?
const char *Sec_XONLINE = Lib_XONLINE;
const char *Lib_XONLINES = "XONLINES";
const char *Sec_XNET = "XNET";

#include "Emu.h"
#include "EmuXTL.h"
#include "HLEDataBase.h"
#include "HLEDataBase/Xapi.OOVPA.inl"
#include "HLEDataBase/D3D8.OOVPA.inl"
#include "HLEDataBase/DSound.OOVPA.inl"
#include "HLEDataBase/XG.OOVPA.inl"
#include "HLEDataBase/XNET.OOVPA.inl"
#include "HLEDataBase/XOnline.OOVPA.inl"
#include "HLEDataBase/XactEng.OOVPA.inl"


// ******************************************************************
// * HLEDataBase
// ******************************************************************

const HLEData HLEDataBase[] = {
    // Support inline functions in .text section
    { Lib_D3D8,{ Sec_text, Sec_D3D }, D3D8_OOVPAV2, D3D8_OOVPA_COUNT },

    // Cannot support LTCG in HLE
    //{ Lib_D3D8LTCG,{ Sec_D3D }, _OOVPAV2, _OOVPA_COUNT },

    // Jarupxx mention this is not a requirement?
    //{ Lib_D3DX8,{ Sec_D3DX }, _OOVPAV2, _OOVPA_COUNT },

    //
    { Lib_DSOUND,{ Sec_DSOUND }, DSound_OOVPAV2, DSound_OOVPA_COUNT },

    // DSOUNDH is just meant to define hot fix, there is no separate section
    //{ Lib_DSOUNDH,{ Sec_DSOUND }, DSound_OOVPAV2, DSound_OOVPA_COUNT },

    //
    { Lib_XACTENG, { Sec_XACTENG }, XACTENG_OOVPAV2, XACTENG_OOVPA_COUNT },

    // test case: Power Drome (Unluckily, it use LTCG version of the library.)
    //{ Lib_XACTENLT,{ Sec_XACTENG }, XACTENG_OOVPAV2, XACTENG_OOVPA_COUNT },

    //
    { Lib_XAPILIB,{ Sec_text, Sec_XPP }, XAPILIB_OOVPAV2, XAPILIB_OOVPA_COUNT },

    // Support inline functions in .text section
    { Lib_XGRAPHC,{ Sec_text, Sec_XGRPH }, XGRAPHC_OOVPAV2, XGRAPHC_OOVPA_COUNT },

    // Cannot support LTCG in HLE
    //{ Lib_XGRAPHCL,{ Sec_XGRPH }, XGRAPHC_OOVPAV2, XGRAPHC_OOVPA_COUNT },

    // Added Sec_text and Sec_XNET just in case.
    // TODO: Need to find out which function is only part of XOnlines.
    { Lib_XONLINE,{ Sec_text, Sec_XONLINE, Sec_XNET }, XONLINES_OOVPAV2, XONLINES_OOVPA_COUNT },

    // Fun fact, XONLINES are split into 2 header sections.
    { Lib_XONLINES,{ Sec_text, Sec_XONLINE, Sec_XNET }, XONLINES_OOVPAV2, XONLINES_OOVPA_COUNT },

    // Added Sec_text just in case.
    // TODO: Need to find out which function is only part of XNets.
    { Lib_XNET,{ Sec_text, Sec_XNET }, XNET_OOVPAV2, XNET_OOVPA_COUNT },

    // XNETS only has XNET, might be true.
    { Lib_XNETS,{ Sec_text, Sec_XNET }, XNET_OOVPAV2, XNET_OOVPA_COUNT },

    // test case: Stake
    { Lib_XNETN,{ Sec_text, Sec_XNET }, XNET_OOVPAV2, XNET_OOVPA_COUNT },
};

// ******************************************************************
// * HLEDataBaseCount
// ******************************************************************
const uint32 HLEDataBaseCount = sizeof(HLEDataBase) / sizeof(HLEDataBase[0]);

// ******************************************************************
// * GetHLEDataBaseHash
// ******************************************************************

// TODO Write a constexpr variation on these methods
namespace HashHelpers
{
	namespace Internal
	{
		// Adapted from https://gist.github.com/underscorediscovery/81308642d0325fd386237cfa3b44785c
		const uint32 fnv1aprime = 0x1000193;
		void hash_fnv1a(uint32& hash, const void* key, const uint32 len)
		{
			const char* data = (char*)key;
			for (uint32 i = 0; i < len; ++i) {
				uint8_t value = data[i];
				hash ^= value;
				hash *= fnv1aprime;
			}
		}

		void HashAssumedLOOVPA(uint32& Hash, const OOVPA* pAssumedLOOVPA)
		{
			// Number of offset-value pairs in the "Header" LOOVPA structure
			uint32 Size = pAssumedLOOVPA->Count * sizeof(OOVPA::LOVP);

			// Size of "Header" structure
			Size += sizeof(OOVPA);

			// Part 1: The array of OOVPA::LOVP items
			hash_fnv1a(Hash, pAssumedLOOVPA, Size);
		}

		void HashOOVPATable(uint32& Hash, const OOVPATable* pTable)
		{
			// Part 1: function name string
			if (pTable->szFuncName != nullptr) {
				hash_fnv1a(Hash, pTable->szFuncName, strlen(pTable->szFuncName));
			}

			// Part 2: version number
			hash_fnv1a(Hash, &pTable->Version, sizeof(pTable->Version));

			// Part 3: LOOVPA
			if (pTable->Oovpa) {
				HashAssumedLOOVPA(Hash, pTable->Oovpa);
			}
		}

		void HashHLEData(uint32& Hash, const HLEData* pData)
		{
			for (uint32 i = 0; i < pData->OovpaTableCount; ++i) {
				HashOOVPATable(Hash, &pData->OovpaTable[i]);
			}
		}
	}

	const uint32 HashHLEDataArray(const HLEData* pDataArray, uint32 Count)
	{
		uint32 Hash = 0x811c9dc5;
		for (uint32 i = 0; i < Count; ++i) {
			Internal::HashHLEData(Hash, pDataArray + i);
		}
		return Hash;
	}
}

uint32 GetHLEDataBaseHash()
{
	// Calculate this just once
	static uint32 CalculatedHash = HashHelpers::HashHLEDataArray(HLEDataBase, HLEDataBaseCount);
	return CalculatedHash;
}

// ******************************************************************
// * XRefDataBase
// ******************************************************************
xbaddr XRefDataBase[XREF_COUNT] = { 0 }; // Reset and populated by EmuHLEIntercept
