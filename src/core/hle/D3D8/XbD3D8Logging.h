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

// Additional types, exclusively for logging (not really enums) :
enum D3DVS20CAPS;
enum D3DPS20CAPS;
enum _D3DCAPS;
enum D3DCAPS2;
enum D3DCAPS3;
enum D3DPRESENT_INTERVAL;
enum D3DCURSORCAPS;
enum D3DDEVCAPS;
enum D3DPMISCCAPS;
enum D3DLINECAPS;
enum D3DPRASTERCAPS;
enum D3DPCMPCAPS;
enum D3DPBLENDCAPS;
enum D3DPSHADECAPS;
enum D3DPTEXTURECAPS;
enum D3DPTFILTERCAPS;
enum D3DPTADDRESSCAPS;
enum D3DSTENCILCAPS;
enum D3DTEXOPCAPS;
enum D3DFVFCAPS;
enum D3DVTXPCAPS;
enum D3DDEVCAPS2;
enum D3DDTCAPS;

namespace XTL {

enum X_D3DUSAGE;
enum X_D3DCOMMON_TYPE;
enum X_D3DRESOURCE_COMMON;
enum X_D3DRESOURCE_FORMAT;
enum X_D3DRESOURCE_SIZE;

} // end of namespace XTL

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

ENUM2STR_HEADER(D3DCUBEMAP_FACES)
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

namespace XTL {

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

ENUM2STR_HEADER(X_D3DCULL)
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

} // end of namespace XTL

#endif _EMU_D3D8_LOGGING_H
