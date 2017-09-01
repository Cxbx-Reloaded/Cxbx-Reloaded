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
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

#undef FIELD_OFFSET     // prevent macro redefinition warnings
#include <windows.h>

#include "CxbxKrnl.h" // For xbaddr

extern "C" const char *szHLELastCompileTime = __TIMESTAMP__;

const char *Lib_D3D8 = "D3D8";
const char *Lib_D3D8LTCG = "D3D8LTCG";
const char *Lib_D3DX8 = "D3DX8";
const char *Lib_DSOUND = "DSOUND";
const char *Lib_XACTENG = "XACTENG";
const char *Lib_XAPILIB = "XAPILIB";
const char *Lib_XGRAPHC = "XGRAPHC";
const char *Lib_XNETS = "XNETS";
const char *Lib_XONLINE = "XONLINE"; // TODO : Typo for XONLINES?
const char *Lib_XONLINES = "XONLINES";

#include "Emu.h"
#include "EmuXTL.h"
#include "HLEDataBase.h"
#include "HLEDataBase/Xapi.1.0.3911.inl"
#include "HLEDataBase/Xapi.1.0.4034.inl"
#include "HLEDataBase/Xapi.1.0.4134.inl"
#include "HLEDataBase/Xapi.1.0.4361.inl"
#include "HLEDataBase/Xapi.1.0.4432.inl"
#include "HLEDataBase/Xapi.1.0.4627.inl"
#include "HLEDataBase/Xapi.1.0.4721.inl"
#include "HLEDataBase/Xapi.1.0.5028.inl"
#include "HLEDataBase/Xapi.1.0.5233.inl"
#include "HLEDataBase/Xapi.1.0.5344.inl"
#include "HLEDataBase/Xapi.1.0.5558.inl"
#include "HLEDataBase/Xapi.1.0.5788.inl"
#include "HLEDataBase/Xapi.1.0.5849.inl"
#include "HLEDataBase/D3D8.1.0.3925.inl"
#include "HLEDataBase/D3D8.1.0.4034.inl"
#include "HLEDataBase/D3D8.1.0.4134.inl"
#include "HLEDataBase/D3D8.1.0.4361.inl"
#include "HLEDataBase/D3D8.1.0.4432.inl"
#include "HLEDataBase/D3D8.1.0.4627.inl"
#include "HLEDataBase/D3D8.1.0.5028.inl"
#include "HLEDataBase/D3D8.1.0.5233.inl"
#include "HLEDataBase/D3D8.1.0.5344.inl"
#include "HLEDataBase/D3D8.1.0.5558.inl"
#include "HLEDataBase/D3D8.1.0.5788.inl"
#include "HLEDataBase/D3D8.1.0.5849.inl"
#include "HLEDataBase/DSound.1.0.3936.inl"
#include "HLEDataBase/DSound.1.0.4134.inl"
#include "HLEDataBase/DSound.1.0.4361.inl"
#include "HLEDataBase/DSound.1.0.4432.inl"
#include "HLEDataBase/DSound.1.0.4627.inl"
#include "HLEDataBase/DSound.1.0.5028.inl"
#include "HLEDataBase/DSound.1.0.5233.inl"
#include "HLEDataBase/DSound.1.0.5344.inl"
#include "HLEDataBase/DSound.1.0.5558.inl"
#include "HLEDataBase/DSound.1.0.5788.inl"
#include "HLEDataBase/DSound.1.0.5849.inl"
#include "HLEDataBase/XG.1.0.3911.inl"
#include "HLEDataBase/XG.1.0.4034.inl"
#include "HLEDataBase/XG.1.0.4361.inl"
#include "HLEDataBase/XG.1.0.4432.inl"
#include "HLEDataBase/XG.1.0.4627.inl"
#include "HLEDataBase/XG.1.0.5028.inl"
#include "HLEDataBase/XG.1.0.5233.inl"
#include "HLEDataBase/XG.1.0.5344.inl"
#include "HLEDataBase/XG.1.0.5558.inl"
#include "HLEDataBase/XG.1.0.5788.inl"
#include "HLEDataBase/XG.1.0.5849.inl"
#include "HLEDataBase/XNet.1.0.3911.inl"
#include "HLEDataBase/XNet.1.0.4627.inl"
#include "HLEDataBase/XOnline.1.0.4361.inl"
#include "HLEDataBase/XOnline.1.0.4627.inl"
#include "HLEDataBase/XOnline.1.0.5028.inl"
#include "HLEDataBase/XOnline.1.0.5233.inl"
#include "HLEDataBase/XOnline.1.0.5344.inl"
#include "HLEDataBase/XOnline.1.0.5558.inl"
#include "HLEDataBase/XOnline.1.0.5788.inl"
#include "HLEDataBase/XOnline.1.0.5849.inl"
#include "HLEDataBase/XactEng.1.0.4627.inl"
#include "HLEDataBase/XactEng.1.0.5849.inl"


// ******************************************************************
// * HLEDataBase
// ******************************************************************
#define HLE_ENTRY(Lib, DB, Version) \
	{Lib, Version, DB##_##Version, DB##_##Version##_SIZE }
//  For example, HLE_ENTRY(Lib_XAPILIB, XAPI, 3911) results in:
//  {Lib_XAPILIB, 3911, XAPI_3911, XAPI_3911_SIZE }

const HLEData HLEDataBase[] =
{
	HLE_ENTRY(Lib_XAPILIB, XAPI, 3911),
	HLE_ENTRY(Lib_XAPILIB, XAPI, 4034),
	HLE_ENTRY(Lib_XAPILIB, XAPI, 4134),
	HLE_ENTRY(Lib_XAPILIB, XAPI, 4361),
	HLE_ENTRY(Lib_XAPILIB, XAPI, 4432),
	HLE_ENTRY(Lib_XAPILIB, XAPI, 4627),
	HLE_ENTRY(Lib_XAPILIB, XAPI, 4721),
	HLE_ENTRY(Lib_XAPILIB, XAPI, 5028),
	HLE_ENTRY(Lib_XAPILIB, XAPI, 5233),
	HLE_ENTRY(Lib_XAPILIB, XAPI, 5344),
	HLE_ENTRY(Lib_XAPILIB, XAPI, 5558),
	HLE_ENTRY(Lib_XAPILIB, XAPI, 5788),
	HLE_ENTRY(Lib_XAPILIB, XAPI, 5849),

	HLE_ENTRY(Lib_D3D8, D3D8, 3925),
	HLE_ENTRY(Lib_D3D8, D3D8, 4034),
	HLE_ENTRY(Lib_D3D8, D3D8, 4134),
	HLE_ENTRY(Lib_D3D8, D3D8, 4361),
	HLE_ENTRY(Lib_D3D8, D3D8, 4432),
	HLE_ENTRY(Lib_D3D8, D3D8, 4627),
	HLE_ENTRY(Lib_D3D8, D3D8, 5028),
	HLE_ENTRY(Lib_D3D8, D3D8, 5233),
	HLE_ENTRY(Lib_D3D8, D3D8, 5344),
	HLE_ENTRY(Lib_D3D8, D3D8, 5558),
	HLE_ENTRY(Lib_D3D8, D3D8, 5788),
	HLE_ENTRY(Lib_D3D8, D3D8, 5849),

	HLE_ENTRY(Lib_DSOUND, DSound, 3936),
	HLE_ENTRY(Lib_DSOUND, DSound, 4134),
	HLE_ENTRY(Lib_DSOUND, DSound, 4361),
	HLE_ENTRY(Lib_DSOUND, DSound, 4432),
	HLE_ENTRY(Lib_DSOUND, DSound, 4627),
	HLE_ENTRY(Lib_DSOUND, DSound, 5028),
	HLE_ENTRY(Lib_DSOUND, DSound, 5233),
	HLE_ENTRY(Lib_DSOUND, DSound, 5344),
	HLE_ENTRY(Lib_DSOUND, DSound, 5558),
	HLE_ENTRY(Lib_DSOUND, DSound, 5788),
	HLE_ENTRY(Lib_DSOUND, DSound, 5849),

	HLE_ENTRY(Lib_XGRAPHC, XG, 3911),
	HLE_ENTRY(Lib_XGRAPHC, XG, 4034),
	HLE_ENTRY(Lib_XGRAPHC, XG, 4361),
	HLE_ENTRY(Lib_XGRAPHC, XG, 4432),
	HLE_ENTRY(Lib_XGRAPHC, XG, 4627),
	HLE_ENTRY(Lib_XGRAPHC, XG, 5028),
	HLE_ENTRY(Lib_XGRAPHC, XG, 5233),
	HLE_ENTRY(Lib_XGRAPHC, XG, 5344),
	HLE_ENTRY(Lib_XGRAPHC, XG, 5558),
	HLE_ENTRY(Lib_XGRAPHC, XG, 5788),
	HLE_ENTRY(Lib_XGRAPHC, XG, 5849),

	HLE_ENTRY(Lib_XNETS, XNet, 3911),
	HLE_ENTRY(Lib_XNETS, XNet, 4627),

	HLE_ENTRY(Lib_XONLINE, XOnline, 4361), // TODO : Typo for XONLINES?

	// TODO: Verify differences between XONLINE and XONLINES (if any)
	HLE_ENTRY(Lib_XONLINES, XOnline, 4627),
	HLE_ENTRY(Lib_XONLINES, XOnline, 5028),
	HLE_ENTRY(Lib_XONLINES, XOnline, 5233),
	HLE_ENTRY(Lib_XONLINES, XOnline, 5344),
	HLE_ENTRY(Lib_XONLINES, XOnline, 5558),
	HLE_ENTRY(Lib_XONLINES, XOnline, 5788),
	HLE_ENTRY(Lib_XONLINES, XOnline, 5849),

	HLE_ENTRY(Lib_XACTENG, XactEng, 4627),
	HLE_ENTRY(Lib_XACTENG, XactEng, 5849),
};

// ******************************************************************
// * HLEDataBaseCount
// ******************************************************************
extern const uint32 HLEDataBaseCount = sizeof(HLEDataBase) / sizeof(HLEData);

// ******************************************************************
// * XRefDataBase
// ******************************************************************
extern xbaddr XRefDataBase[XREF_COUNT] = { 0 }; // Reset and populated by EmuHLEIntercept
