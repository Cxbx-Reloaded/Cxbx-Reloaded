// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuD3D8Conv.cpp
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
#define _XBOXKRNL_LOCAL_

#include "Emu.h"

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace xd3d8
{
    #include "xd3d8.h"
};

#include "EmuD3D8.h"

// ******************************************************************
// * D3DVertexToPrimitive
// ******************************************************************
UINT xd3d8::D3DVertexToPrimitive[11][2] =
{
    {0, 0},
    {1, 0},
    {2, 0},
    {1, 1},
    {1, 1},
    {3, 0},
    {1, 2},
    {1, 2},
    {4, 0},
    {2, 2},
    {0, 0},
};

// ******************************************************************
// * EmuPrimitiveTypeLookup
// ******************************************************************
xd3d8::D3DPRIMITIVETYPE xd3d8::EmuPrimitiveTypeLookup[] = 
{
    /* NULL                 = 0         */ (xd3d8::D3DPRIMITIVETYPE)0,
    /* D3DPT_POINTLIST      = 1,        */ xd3d8::D3DPT_POINTLIST,
    /* D3DPT_LINELIST       = 2,        */ xd3d8::D3DPT_LINELIST,
    /* D3DPT_LINELOOP       = 3,  Xbox  */ xd3d8::D3DPT_LINELIST,
    /* D3DPT_LINESTRIP      = 4,        */ xd3d8::D3DPT_LINESTRIP,
    /* D3DPT_TRIANGLELIST   = 5,        */ xd3d8::D3DPT_TRIANGLELIST,
    /* D3DPT_TRIANGLESTRIP  = 6,        */ xd3d8::D3DPT_TRIANGLESTRIP,
    /* D3DPT_TRIANGLEFAN    = 7,        */ xd3d8::D3DPT_TRIANGLEFAN,
    /* D3DPT_QUADLIST       = 8,  Xbox  */ xd3d8::D3DPT_TRIANGLELIST,
    /* D3DPT_QUADSTRIP      = 9,  Xbox  */ xd3d8::D3DPT_TRIANGLELIST,
    /* D3DPT_POLYGON        = 10, Xbox  */ xd3d8::D3DPT_TRIANGLELIST,
    /* D3DPT_MAX            = 11,       */ (xd3d8::D3DPRIMITIVETYPE)11
};
