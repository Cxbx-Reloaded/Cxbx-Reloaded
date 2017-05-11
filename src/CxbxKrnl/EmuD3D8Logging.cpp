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
// *   Cxbx->EmuD3D8Logging.cpp
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
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

#include "Logging.h"
#include "EmuD3D8Types.h"
#include "EmuD3D8Logging.h"

// prevent name collisions
namespace xboxkrnl
{

//
// Host D3D LOGRENDER(Type) implementations
//

}; // end of namespace xboxkrnl
   
   
// prevent name collisions
namespace XTL
{

//
// Xbox D3D LOGRENDER(Type) implementations
//

#if 0
LOGRENDER(D3DVIEWPORT8)
{
	return os
		LOGRENDER_MEMBER(X)
		LOGRENDER_MEMBER(Y)
		LOGRENDER_MEMBER(Width)
		LOGRENDER_MEMBER(Height)
		LOGRENDER_MEMBER(MinZ)
		LOGRENDER_MEMBER(MaxZ);
}

LOGRENDER(X_D3DResource)
{
	return os
		LOGRENDER_MEMBER(Common)
		LOGRENDER_MEMBER(Data)
		LOGRENDER_MEMBER(Lock);
}
#endif

}; // end of namespace XTL
