// ******************************************************************
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
// *  (c) 2017 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef _EMU_D3D8_LOGGING_H
#define _EMU_D3D8_LOGGING_H

#include "Logging.h"
#include "XbD3D8Types.h"

extern const char* D3DErrorString(HRESULT hResult); // Implemented in Direct3D9.cpp

#define DEBUG_D3DRESULT(hRet, message) \
	do { \
		LOG_CHECK_ENABLED(LOG_LEVEL::DEBUG) { \
			if (FAILED(hRet)) \
				if(g_bPrintfOn) \
					printf("%s%s : %s D3D error (0x%.08X: %s)\n", _logThreadPrefix.c_str(), _logFuncPrefix.c_str(), message, hRet, D3DErrorString(hRet)); \
		} \
	} while (0)

// Additional types, exclusively for logging (not really enums) :
enum D3DVS20CAPS : int;
enum D3DPS20CAPS : int;
enum _D3DCAPS : int;
enum D3DCAPS2 : int;
enum D3DCAPS3 : int;
enum D3DPRESENT_INTERVAL : int;
enum D3DCURSORCAPS : int;
enum D3DDEVCAPS : int;
enum D3DPMISCCAPS : int;
enum D3DLINECAPS : int;
enum D3DPRASTERCAPS : int;
enum D3DPCMPCAPS : int;
enum D3DPBLENDCAPS : int;
enum D3DPSHADECAPS : int;
enum D3DPTEXTURECAPS : int;
enum D3DPTFILTERCAPS : int;
enum D3DPTADDRESSCAPS : int;
enum D3DSTENCILCAPS : int;
enum D3DTEXOPCAPS : int;
enum D3DFVFCAPS : int;
enum D3DVTXPCAPS : int;
enum D3DDEVCAPS2 : int;
enum D3DDTCAPS : int;

namespace xbox {

enum X_D3DUSAGE : int;
enum X_D3DCOMMON_TYPE : int;
enum X_D3DRESOURCE_COMMON : int;
enum X_D3DRESOURCE_FORMAT : int;
enum X_D3DRESOURCE_SIZE : int;

} // end of namespace xbox

//
// Headers for rendering host D3D enum types :
//

FLAGS2STR_HEADER(D3DVS20CAPS) // Not really an enum
FLAGS2STR_HEADER(D3DPS20CAPS) // Not really an enum
ENUM2STR_HEADER(D3DDEVTYPE)
FLAGS2STR_HEADER(_D3DCAPS) // Not really an enum
FLAGS2STR_HEADER(D3DCAPS2) // Not really an enum
FLAGS2STR_HEADER(D3DCAPS3) // Not really an enum
FLAGS2STR_HEADER(D3DPRESENT_INTERVAL) // Not really an enum
FLAGS2STR_HEADER(D3DCURSORCAPS) // Not really an enum
FLAGS2STR_HEADER(D3DDEVCAPS) // Not really an enum
FLAGS2STR_HEADER(D3DPMISCCAPS) // Not really an enum
FLAGS2STR_HEADER(D3DLINECAPS) // Not really an enum
FLAGS2STR_HEADER(D3DPRASTERCAPS) // Not really an enum
FLAGS2STR_HEADER(D3DPCMPCAPS) // Not really an enum
FLAGS2STR_HEADER(D3DPBLENDCAPS) // Not really an enum
FLAGS2STR_HEADER(D3DPSHADECAPS) // Not really an enum
FLAGS2STR_HEADER(D3DPTEXTURECAPS) // Not really an enum
FLAGS2STR_HEADER(D3DPTFILTERCAPS) // Not really an enum
FLAGS2STR_HEADER(D3DPTADDRESSCAPS) // Not really an enum
FLAGS2STR_HEADER(D3DSTENCILCAPS) // Not really an enum
FLAGS2STR_HEADER(D3DTEXOPCAPS) // Not really an enum
FLAGS2STR_HEADER(D3DFVFCAPS) // Not really an enum
FLAGS2STR_HEADER(D3DVTXPCAPS) // Not really an enum
FLAGS2STR_HEADER(D3DDEVCAPS2) // Not really an enum
FLAGS2STR_HEADER(D3DDTCAPS) // Not really an enum

ENUM2STR_HEADER(D3DFORMAT)
ENUM2STR_HEADER(D3DPOOL)

//
// Host D3D LOGRENDER_HEADER(Type) declarations
//

LOGRENDER_HEADER(D3DVSHADERCAPS2_0)
LOGRENDER_HEADER(D3DPSHADERCAPS2_0)

LOGRENDER_HEADER(D3DCAPS)
LOGRENDER_HEADER(D3DLOCKED_RECT)
LOGRENDER_HEADER(RECT)

namespace xbox {

//
// Headers for rendering Xbox D3D enum types :
//

// TODO: fill out these enumeration tables for convienance
//ENUM2STR_HEADER(X_D3DBLENDOP)
//ENUM2STR_HEADER(X_D3DBLEND)
//ENUM2STR_HEADER(X_D3DCMPFUNC)
//ENUM2STR_HEADER(X_D3DFILLMODE)
//ENUM2STR_HEADER(X_D3DSHADEMODE)
//ENUM2STR_HEADER(X_D3DSTENCILOP)
//ENUM2STR_HEADER(X_D3DTEXTURESTAGESTATETYPE)

ENUM2STR_HEADER(X_D3DCUBEMAP_FACES)
ENUM2STR_HEADER(X_D3DCULL)
//ENUM2STR_HEADER(X_D3DDEVTYPE)
ENUM2STR_HEADER(X_D3DFORMAT)
ENUM2STR_HEADER(X_D3DPRIMITIVETYPE)
ENUM2STR_HEADER(X_D3DRESOURCETYPE)
ENUM2STR_HEADER(X_D3DSET_DEPTH_CLIP_PLANES_FLAGS)
FLAGS2STR_HEADER(X_D3DUSAGE) // Not really an enum

//
// Cxbx D3D LOGRENDER(Type) implementations
//

ENUM2STR_HEADER(X_D3DCOMMON_TYPE)
LOGRENDER_HEADER(X_D3DRESOURCE_COMMON)
LOGRENDER_HEADER(X_D3DRESOURCE_FORMAT)
LOGRENDER_HEADER(X_D3DRESOURCE_SIZE)

//
// Xbox D3D LOGRENDER_HEADER(Type) declarations
//

LOGRENDER_HEADER(X_D3DVIEWPORT8)
LOGRENDER_HEADER(X_D3DDISPLAYMODE)
LOGRENDER_HEADER(X_D3DResource)
LOGRENDER_HEADER(X_D3DPixelContainer)

} // end of namespace xbox

#endif _EMU_D3D8_LOGGING_H
