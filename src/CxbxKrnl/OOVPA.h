// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->OOVPA.h
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
#ifndef OOVPA_H
#define OOVPA_H

#include "Cxbx.h"

// ******************************************************************
// * Take THIS C++ !!
// ******************************************************************
template <class BaseClass, typename MFT> inline void *MFPtoFP(MFT pMemFunc)
{
	union
	{
		MFT pMemFunc;
		void(*pFunc)();
	}
	ThisConv;

	ThisConv.pMemFunc = pMemFunc;

	return ThisConv.pFunc;
}

#pragma pack(1)

// ******************************************************************
// * Optimized (Offset, Value)-Pair Array
// ******************************************************************
struct OOVPA
{
	// This OOVPA field (uint08 Count) indicates the number of
	// {Offset, Value}-pairs present in the Lovp array,
	// available after casting this OOVPA to LOOVPA.
	// (This Count INCLUDES optional leading {Offset, XREF_*-enum}-
	// pairs - see comment at XRefCount.)
	uint08 Count;

	// This OOVPA field (uint08 XRefCount) contains the number of
	// {Offset, XREF_*-enum}-pairs that come before all other
	// {Offset, Value}-pairs.
	// (The {Offset, XREF_*-enum}-pairs are INCLUDED in OOVPA.Count)
	// (Also, see comments at XRefZero and XRefOne.)
	uint08 XRefCount;

	// This OOVPA field (uint16 XRefSaveIndex) contains either an
	// XREF_* enum value, or the XRefNoSaveIndex marker when there's
	// no XREF_* enum defined for this OOVPA.
	uint16 XRefSaveIndex;

	// Define LOVP here to reduce type definition complexity.
	// (Otherwise, if defined in the template classes, that would mean
	// that for each template instance, the type is redefined. Let's
	// avoid that.)

	// {Offset, Value}-pair(s)
	struct LOVP
	{
		uint16 Offset;
		uint08 Value;
	};
};

// This XRefZero constant, when set in the OOVPA.XRefCount field,
// indicates there are no {offset, XREF_*-enum} present in the OOVPA.
const uint08 XRefZero = (uint08)0;

// This XRefOne constant, when set in the OOVPA.XRefCount field,
// indicates the OOVPA contains one (1) {offset, XREF_* enum} pair.
const uint08 XRefOne = (uint08)1;
const uint08 XRefTwo = (uint08)2;

// Note : Theoretically, there can be more than one {Offset, XREF_*-enum}
// pair at the start of the OOVPA's, but there are no examples of that yet.

// This XRefNoSaveIndex constant, when set in the OOVPA.XRefSaveIndex
// field, functions as a marker indicating there's no XREF_* enum
// defined for the OOVPA.
const uint16 XRefNoSaveIndex = (uint16)-1;

// Macro used for storing an XRef {Offset, XRef}-Pair.
//
// XRefs are stored with Offset and Value swapped. This is to be able
// to store XRef values beyond 8 bits (for now limited to 16 bits).
// The price to pay for this is that the Offset is stored using 8 bits,
// meaning that offsets beyond 255 cannot be used, not problem for now.
#define XREF_ENTRY(Offset, XRef)	\
	{ XRef, Offset }

// UNUSED Macro for storing a normal (non-XRef) {Offset, Value}-Pair
// Offsets can go up to 16 bits, values are always one byte (8 bits)
#define OV_ENTRY(Offset, Value)	\
	{ Offset, Value }


// ******************************************************************
// * Large Optimized (Offset,Value)-Pair Array
// ******************************************************************
template <uint16 COUNT> struct LOOVPA
{
	OOVPA Header;

	// Large (Offset,Value)-Pair(s)
	OOVPA::LOVP Lovp[COUNT];
};

#define OOVPA_XREF(Name, Version, Count, XRefSaveIndex, XRefCount)	\
LOOVPA<Count> Name##_##Version = { { Count, XRefCount, XRefSaveIndex }, {

#define OOVPA_NO_XREF(Name, Version, Count) \
OOVPA_XREF(Name, Version, Count, XRefNoSaveIndex, XRefZero)

#define OOVPA_END } }


// ******************************************************************
// * OOVPATable
// ******************************************************************
struct OOVPATable
{
	OOVPA *Oovpa;
	void  *emuPatch;
	char  *szFuncName;
	uint16_t Version : 13; // 2^13 = 8192, enough to store lowest and higest possible Library Version number in
	uint16_t Flags : 3;
};

const uint16_t Flag_IsLTCG = 1; // Indicates an entry that registers an LTCG OOVPA
const uint16_t Flag_DontScan = 2; // Indicates an entry that's currently disabled and thus shouldn't be searched for
const uint16_t Flag_Reserved = 4;

#define OOVPA_TABLE_ENTRY_FULL(Oovpa, Patch, DebugName, Version, Flags) \
	{ & Oovpa ## _ ## Version.Header, Patch, DebugName, Version, Flags }

// REGISTER_OOVPA is the ONLY allowed macro for registrations.
// Registrations MUST stay sorted to prevent duplicates and maintain overview.
// The TYPE argument MUST be PATCH, XREF, ALIAS, EMUTHIS, LTCG or DISABLED (see below).
// ONLY use ALIAS when absolutely required (when OOVPA identifier cannot follow Patch)
// ONLY use LTCG for LTCG OOVPA's (HLE support for these is flacky at best)
// DO NOT comment out registrations, but use TYPE DISABLED instead.
#define REGISTER_OOVPA(Symbol, Version, TYPE, ...) \
	REGISTER_OOVPA_##TYPE(Symbol, Version, __VA_ARGS__)

#define PATCH /* most common registration, Symbol indicates both an OOVPA and Patch */
#define REGISTER_OOVPA_PATCH(Symbol, Version, ...) \
	OOVPA_TABLE_ENTRY_FULL(Symbol, XTL::EMUPATCH(Symbol), #Symbol ## "_" ## #Version, Version, 0)

#define XREF /* registration of an XRef-only OOVPA, for which no Patch is present */
#define REGISTER_OOVPA_XREF(Symbol, Version, ...) \
	OOVPA_TABLE_ENTRY_FULL(Symbol, nullptr, #Symbol ## "_" ## #Version ## " (XRef)", Version, 0)

#define ALIAS /* registration of a Patch using an alternatively named OOVPA */
#define REGISTER_OOVPA_ALIAS(Symbol, Version, AliasOovpa) \
	OOVPA_TABLE_ENTRY_FULL(AliasOovpa, XTL::EMUPATCH(Symbol), #AliasOovpa ## "_" ## #Version, Version, 0)

#define EMUTHIS /* registration of an EmuThis-derived function */
#define REGISTER_OOVPA_EMUTHIS(Symbol, Version, ...) \
	OOVPA_TABLE_ENTRY_FULL(Symbol, MFPtoFP<XTL::EmuThis>(&XTL::EmuThis::EMUPATCH(Symbol)), #Symbol ## "_" ## #Version, Version, 0)

#define LTCG /* registration of a Patch using a LTCG specific OOVPA */
#define REGISTER_OOVPA_LTCG(Symbol, Version, ...) \
	OOVPA_TABLE_ENTRY_FULL(Symbol ## _LTCG, XTL::EMUPATCH(Symbol), #Symbol ## "_LTCG_" ## #Version ## " (LTCG)", Version, Flag_IsLTCG)

#define DISABLED /* registration is (temporarily) disabled by a flag */
#define REGISTER_OOVPA_DISABLED(Symbol, Version, ...) \
	OOVPA_TABLE_ENTRY_FULL(Symbol, nullptr, #Symbol ## "_" ## #Version ## " (Disabled)", Version, Flag_DontScan)


#pragma pack()

#endif
