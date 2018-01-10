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

// http://stackoverflow.com/questions/5134523/msvc-doesnt-expand-va-args-correctly
// MSVC_EXPAND works around a Visual C++ problem, expanding __VA_ARGS__ incorrectly:
#define MSVC_EXPAND(x) x

#define STRINGIZEX(x) #x
#define STRINGIZE(x) STRINGIZEX(x)

#include <iterator>

// http://en.cppreference.com/w/cpp/iterator/size
#define OOVPA_TABLE_COUNT(x) std::size(x)

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

// Sometimes, there can be more than one {Offset, XREF_*-enum}
// pair at the start of the OOVPA's.
const uint08 XRefTwo = (uint08)2;
const uint08 XRefThree = (uint08)3;

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
#define XREF_ENTRY(Offset, XRef)    \
    { XRef, Offset }

// UNUSED Macro for storing a normal (non-XRef) {Offset, Value}-Pair
// Offsets can go up to 16 bits, values are always one byte (8 bits)
#define OV_ENTRY(Offset, Value)    \
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

#define OOVPA_XREF(Name, Version, Count, XRefSaveIndex, XRefCount)    \
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
    char  *szFuncName;
    uint16_t Version;// : 13; // 2^13 = 8192, enough to store lowest and higest possible Library Version number in
};

#define OOVPA_TABLE_ENTRY_FULL(Oovpa, DebugName, Version) \
    { & Oovpa ## _ ## Version.Header, DebugName, Version }

#define PATCH PATCH/* most common registration, Symbol indicates both an OOVPA and Patch */

#define XREF XREF/* registration of an XRef-only OOVPA, for which no Patch is present */

#define UNPATCHED UNPATCHED/* registration is remain unpatched,
                                can be activate via HLE Cache file by removing _UNPATCHED at the end. */

/* Use XREF define to knowledge it is reference purpose only.*/
#define REGISTER_OOVPA_XREF(Symbol, TYPE, Version) \
    OOVPA_TABLE_ENTRY_FULL(Symbol, #Symbol, Version)

/* Use PATCH define only for functions with FUNC_EXPORTS included.*/
#define REGISTER_OOVPA_PATCH(Symbol, TYPE, Version) \
    OOVPA_TABLE_ENTRY_FULL(Symbol, #Symbol, Version)

/* UNPATCHED define will still perform scan and skip function's patch for all revisions only.
 * This is only effective for functions with "FUNC_EXPORTS" bypass purpose.
 * XREF remain unaffected and will perform task normally.
 * NOTICE: Do not use UNPATCHED on XREF OOVPA! Or developers will be confused. */
#define REGISTER_OOVPA_UNPATCHED(Symbol, TYPE, Version) \
    OOVPA_TABLE_ENTRY_FULL(Symbol, STRINGIZEX(Symbol## _ ##TYPE), Version)

// TODO: Once LLE is full support, replace all UNPATCHED back to PATCH, then remove TYPE from SECOND line below only.
#define REGISTER_OOVPA(Symbol, TYPE, Version) \
    MSVC_EXPAND(REGISTER_OOVPA_##TYPE(Symbol, TYPE, Version))
//    { &(Symbol ## _ ## Version).Header, #Symbol, Version, Flags }

// Based on https://codecraft.co/2014/11/25/variadic-macros-tricks/
// and https://groups.google.com/d/msg/comp.std.c/d-6Mj5Lko_s/jqonQLK20HcJ
#define REGISTER_OOVPA_0(...)
 #define REGISTER_OOVPA_1(Symbol, TYPE, Version) REGISTER_OOVPA(Symbol, TYPE, Version)
 #define REGISTER_OOVPA_2(Symbol, TYPE, Version, ...) REGISTER_OOVPA(Symbol, TYPE, Version), MSVC_EXPAND(REGISTER_OOVPA_1(Symbol, TYPE, __VA_ARGS__))
 #define REGISTER_OOVPA_3(Symbol, TYPE, Version, ...) REGISTER_OOVPA(Symbol, TYPE, Version), MSVC_EXPAND(REGISTER_OOVPA_2(Symbol, TYPE, __VA_ARGS__))
 #define REGISTER_OOVPA_4(Symbol, TYPE, Version, ...) REGISTER_OOVPA(Symbol, TYPE, Version), MSVC_EXPAND(REGISTER_OOVPA_3(Symbol, TYPE, __VA_ARGS__))
 #define REGISTER_OOVPA_5(Symbol, TYPE, Version, ...) REGISTER_OOVPA(Symbol, TYPE, Version), MSVC_EXPAND(REGISTER_OOVPA_4(Symbol, TYPE, __VA_ARGS__))
 #define REGISTER_OOVPA_6(Symbol, TYPE, Version, ...) REGISTER_OOVPA(Symbol, TYPE, Version), MSVC_EXPAND(REGISTER_OOVPA_5(Symbol, TYPE, __VA_ARGS__))
 #define REGISTER_OOVPA_7(Symbol, TYPE, Version, ...) REGISTER_OOVPA(Symbol, TYPE, Version), MSVC_EXPAND(REGISTER_OOVPA_6(Symbol, TYPE, __VA_ARGS__))
 #define REGISTER_OOVPA_8(Symbol, TYPE, Version, ...) REGISTER_OOVPA(Symbol, TYPE, Version), MSVC_EXPAND(REGISTER_OOVPA_7(Symbol, TYPE, __VA_ARGS__))
 #define REGISTER_OOVPA_9(Symbol, TYPE, Version, ...) REGISTER_OOVPA(Symbol, TYPE, Version), MSVC_EXPAND(REGISTER_OOVPA_8(Symbol, TYPE, __VA_ARGS__))
 #define REGISTER_OOVPA_10(Symbol, TYPE, Version, ...) REGISTER_OOVPA(Symbol, TYPE, Version), MSVC_EXPAND(REGISTER_OOVPA_9(Symbol, TYPE, __VA_ARGS__))
 #define REGISTER_OOVPA_11(Symbol, TYPE, Version, ...) REGISTER_OOVPA(Symbol, TYPE, Version), MSVC_EXPAND(REGISTER_OOVPA_10(Symbol, TYPE, __VA_ARGS__))
 #define REGISTER_OOVPA_12(Symbol, TYPE, Version, ...) REGISTER_OOVPA(Symbol, TYPE, Version), MSVC_EXPAND(REGISTER_OOVPA_11(Symbol, TYPE, __VA_ARGS__))
 #define REGISTER_OOVPA_13(Symbol, TYPE, Version, ...) REGISTER_OOVPA(Symbol, TYPE, Version), MSVC_EXPAND(REGISTER_OOVPA_12(Symbol, TYPE, __VA_ARGS__))
 #define REGISTER_OOVPA_14(Symbol, TYPE, Version, ...) REGISTER_OOVPA(Symbol, TYPE, Version), MSVC_EXPAND(REGISTER_OOVPA_13(Symbol, TYPE, __VA_ARGS__))
 #define REGISTER_OOVPA_15(Symbol, TYPE, Version, ...) REGISTER_OOVPA(Symbol, TYPE, Version), MSVC_EXPAND(REGISTER_OOVPA_14(Symbol, TYPE, __VA_ARGS__))
 #define REGISTER_OOVPA_16(Symbol, TYPE, Version, ...) REGISTER_OOVPA(Symbol, TYPE, Version), MSVC_EXPAND(REGISTER_OOVPA_15(Symbol, TYPE, __VA_ARGS__))
 #define REGISTER_OOVPA_17(Symbol, TYPE, Version, ...) REGISTER_OOVPA(Symbol, TYPE, Version), MSVC_EXPAND(REGISTER_OOVPA_16(Symbol, TYPE, __VA_ARGS__))
 #define REGISTER_OOVPA_18(Symbol, TYPE, Version, ...) REGISTER_OOVPA(Symbol, TYPE, Version), MSVC_EXPAND(REGISTER_OOVPA_17(Symbol, TYPE, __VA_ARGS__))
 #define REGISTER_OOVPA_19(Symbol, TYPE, Version, ...) REGISTER_OOVPA(Symbol, TYPE, Version), MSVC_EXPAND(REGISTER_OOVPA_18(Symbol, TYPE, __VA_ARGS__))

// Accept any number of args >= N, but expand to just the Nth one. In this case,
// we have settled on 20 as N. We could pick a different number by adjusting
// the count of throwaway args before N. Note that this macro is preceded by
// an underscore--it's an implementation detail, not something we expect people
// to call directly.
#define _GET_NTH_ARG( \
     _19,  _18,  _17,  _16,  _15,  _14,  _13,  _12,  _11,  _10, \
     _9,  _8,  _7,  _6,  _5,  _4,  _3,  _2,  _1,  _0, \
    N, ...) N

// NOTE: A reminder for all of developers, TYPE argument is meant for research documentation.
// (eventually UNPATCHED will be replace back to PATCH once LLE is fully supportive in the future)
#define REGISTER_OOVPAS(Symbol, TYPE, ...) MSVC_EXPAND(_GET_NTH_ARG("ignored", __VA_ARGS__, \
    REGISTER_OOVPA_19, REGISTER_OOVPA_18, REGISTER_OOVPA_17, REGISTER_OOVPA_16, REGISTER_OOVPA_15, \
    REGISTER_OOVPA_14, REGISTER_OOVPA_13, REGISTER_OOVPA_12, REGISTER_OOVPA_11, REGISTER_OOVPA_10, \
    REGISTER_OOVPA_9, REGISTER_OOVPA_8, REGISTER_OOVPA_7, REGISTER_OOVPA_6, REGISTER_OOVPA_5, \
    REGISTER_OOVPA_4, REGISTER_OOVPA_3, REGISTER_OOVPA_2, REGISTER_OOVPA_1, REGISTER_OOVPA_0)(Symbol, TYPE, __VA_ARGS__))


#pragma pack()

#endif
