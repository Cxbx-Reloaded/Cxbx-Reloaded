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
template <class BaseClass, typename MFT> inline void *MFPtoFP( MFT pMemFunc)
{
    union
    {
        MFT pMemFunc;
        void (*pFunc)();
    }
    ThisConv;

    ThisConv.pMemFunc = pMemFunc;

    return ThisConv.pFunc;
}

#pragma pack(1)

enum OOVPAType : uint16 {
	Small, // Meaning, use SOVP, in which Offset is an uint08
	Large, // Meaning, use LOVP, in which Offset is an uint16
};

// ******************************************************************
// * Optimized (Offset, Value)-Pair Array
// ******************************************************************
struct OOVPA
{
	// This OOVPA field (OOVPAType Type) indicates weither
	// this struct needs to be cast to SOOVPA or LOOVPA,
	// for OOVPATYPE.Small vs .Large
	// SOOVPA uses bytes for offset in the {Offset, Value}-pairs.
	// LOOVPA uses words for offset in the {Offset, Value}-pairs.
	// The value field in the {Offset, Value}-pairs is of type byte.
	OOVPAType Type : 1;

	// This OOVPA field (uint16 Count) indicates the number of
	// {Offset, Value}-pairs present in the Sovp or Lovp array,
	// available after casting this OOVPA to SOOVPA or LOOVPA.
	// (This Count INCLUDES optional leading {Offset, XREF_*-enum}-
	// pairs - see comment at XRefCount.)
	uint16 Count : 15;

	// This OOVPA field (uint08 XRefSaveIndex) contains either an
	// XREF_* enum value, or the XRefNoSaveIndex marker when there's
	// no XREF_* enum defined for this OOVPA.
	uint08 XRefSaveIndex;

	// This OOVPA field (uint08 XRefCount) contains the number of
	// {Offset, XREF_*-enum}-pairs that come before all other
	// {Offset, Value}-pairs.
	// (The {Offset, XREF_*-enum}-pairs are INCLUDED in OOVPA.Count)
	// (Also, see comments at XRefZero and XRefOne.)
	uint08 XRefCount;

	// Define SOVP and LOVP here to reduce type definition complexity.
	// (Otherwise, if defined in the template classes, that would mean
	// that for each template instance, the type is redefined. Let's
	// avoid that.)

	// Small (byte-sized) {Offset, Value}-pair(s)
	struct SOVP
	{
		uint08 Offset;
		uint08 Value;
	};

	// Large (word-sized) {Offset, Value}-pair(s)
	struct LOVP
	{
		uint16 Offset;
		uint08 Value;
	};
};

// This XRefNoSaveIndex constant, when set in the OOVPA.XRefSaveIndex
// field, functions as a marker indicating there's no XREF_* enum
// defined for the OOVPA.
const uint08 XRefNoSaveIndex = (uint08)-1;

// This XRefZero constant, when set in the OOVPA.XRefSaveIndex field,
// indicates there are no {offset, XREF_*-enum} present in the OOVPA.
const uint08 XRefZero = (uint08)0;

// This XRefOne constant, when set in the OOVPA.XRefSaveIndex field,
// indicates the OOVPA contains one (1) {offset, XREF_* enum} pair.
const uint08 XRefOne = (uint08)1;

// Note : Theoretically, there can be more than one {offset, XREF_*-enum}
// pair at the start of the OOVPA's, but there are no examples of that yet.
// (Also, EmuLocateFunction might not cater for this well enough?)


// ******************************************************************
// * Large Optimized (Offset,Value)-Pair Array
// ******************************************************************
template <uint16 COUNT> struct LOOVPA
{
	OOVPA Header;

	// Large (Offset,Value)-Pair(s)
	OOVPA::LOVP Lovp[COUNT];
};

// ******************************************************************
// * Small Optimized (Offset,Value)-Pair Array
// ******************************************************************
template <uint16 COUNT> struct SOOVPA
{
	OOVPA Header;

	// Small (Offset,Value)-Pair(s)
	OOVPA::SOVP Sovp[COUNT];
};

// ******************************************************************
// * OOVPATable
// ******************************************************************
struct OOVPATable
{
    OOVPA *Oovpa;

    void  *lpRedirect;

    #ifdef _DEBUG_TRACE
    char  *szFuncName;
    #endif
};

#define OOVPA_XREF_LARGE(Name, Count, XRefSaveIndex, XRefCount)	\
LOOVPA<Count> Name = { { /*OOVPAType*/Large, Count, XRefSaveIndex, XRefCount }, {

#define OOVPA_XREF(Name, Count, XRefSaveIndex, XRefCount)	\
SOOVPA<Count> Name = { { /*OOVPAType*/Small, Count, XRefSaveIndex, XRefCount }, {

#define OOVPA_NO_XREF_LARGE(Name, Count) \
OOVPA_XREF_LARGE(Name, Count, XRefNoSaveIndex, XRefZero)

#define OOVPA_NO_XREF(Name, Count) \
OOVPA_XREF(Name, Count, XRefNoSaveIndex, XRefZero)

#define OOVPA_ENTRY(Offset, Value) { Offset, Value },
#define OOVPA_END } }


#if _DEBUG_TRACE
#define OOVPA_TABLE_PATCH(Oovpa, Patch)	\
	{&Oovpa.Header, Patch, #Patch}
// TODO : _DEBUG_TRACE OOVPA_TABLE_* macro's :
// Cut Version off of Oovpa, and log separatly as "("#Version")"
#define OOVPA_TABLE_XREF(Oovpa)	\
	{&Oovpa.Header, 0, #Oovpa" (XRef)"}
#else
#define OOVPA_TABLE_PATCH(Oovpa, Patch)	\
	{&Oovpa.Header, Patch}
#define OOVPA_TABLE_XREF(Oovpa)	\
	{&Oovpa.Header, 0}
#endif

#pragma pack()

#endif
