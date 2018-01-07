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

extern "C" const char *szHLELastCompileTime = __TIMESTAMP__;

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
    { Lib_D3D8,{ Sec_text, Sec_D3D }, D3D8_OOVPAV2, D3D8_OOVPA_SIZEV2 },

    // Cannot support LTCG in HLE
    //{ Lib_D3D8LTCG,{ Sec_D3D }, _OOVPAV2, _OOVPA_SIZEV2 },

    // Jarupxx mention this is not a requirement?
    //{ Lib_D3DX8,{ Sec_D3DX }, _OOVPAV2, _OOVPA_SIZEV2 },

    //
    { Lib_DSOUND,{ Sec_DSOUND }, DSound_OOVPAV2, DSound_OOVPA_SIZEV2 },

    // DSOUNDH is just meant to define hot fix, there is no seperate section
    //{ Lib_DSOUNDH,{ Sec_DSOUND }, DSound_OOVPAV2, DSound_OOVPA_SIZEV2 },

    //
    { Lib_XACTENG, { Sec_XACTENG }, XACTENG_OOVPAV2, XACTENG_OOVPA_SIZEV2 },

    // test case: Power Drome (Unluckily, it use LTCG version of the library.)
    //{ Lib_XACTENLT,{ Sec_XACTENG }, XACTENG_OOVPAV2, XACTENG_OOVPA_SIZEV2 },

    //
    { Lib_XAPILIB,{ Sec_text, Sec_XPP }, XAPILIB_OOVPAV2, XAPILIB_OOVPA_SIZEV2 },

    // Support inline functions in .text section
    { Lib_XGRAPHC,{ Sec_text, Sec_XGRPH }, XGRAPHC_OOVPAV2, XGRAPHC_OOVPA_SIZEV2 },

    // Cannot support LTCG in HLE
    //{ Lib_XGRAPHCL,{ Sec_XGRPH }, XGRAPHC_OOVPAV2, XGRAPHC_OOVPA_SIZEV2 },

    // Added Sec_text and Sec_XNET just in case.
    // TODO: Need to find out which function is only part of XOnlines.
    { Lib_XONLINE,{ Sec_text, Sec_XONLINE, Sec_XNET }, XONLINES_OOVPAV2, XONLINES_OOVPA_SIZEV2 },

    // Fun fact, XONLINES are split into 2 header sections.
    { Lib_XONLINES,{ Sec_text, Sec_XONLINE, Sec_XNET }, XONLINES_OOVPAV2, XONLINES_OOVPA_SIZEV2 },

    // Added Sec_text just in case.
    // TODO: Need to find out which function is only part of XNets.
    { Lib_XNET,{ Sec_text, Sec_XNET }, XNET_OOVPAV2, XNET_OOVPA_SIZEV2 },

    // XNETS only has XNET, might be true.
    { Lib_XNETS,{ Sec_text, Sec_XNET }, XNET_OOVPAV2, XNET_OOVPA_SIZEV2 },

    // test case: Stake
    { Lib_XNETN,{ Sec_text, Sec_XNET }, XNET_OOVPAV2, XNET_OOVPA_SIZEV2 },
};

// ******************************************************************
// * HLEDataBaseCount
// ******************************************************************
const uint32 HLEDataBaseCount = sizeof(HLEDataBase) / sizeof(HLEData);

// ******************************************************************
// * XRefDataBase
// ******************************************************************
xbaddr XRefDataBase[XREF_COUNT] = { 0 }; // Reset and populated by EmuHLEIntercept
