// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuD3D8VertexShader.h
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
#ifndef EMUD3D8VERTEXSHADER_H
#define EMUD3D8VERTEXSHADER_H

// nv2a microcode header
typedef struct _NV2A_HEADER
{
    BYTE Type;
    BYTE Version;
    BYTE NumInst;
    BYTE Unknown;
}
NV2A_HEADER;

#define NV2A_MICROCODE_DEF_TYPE		0x78 // x
#define NV2A_MICROCODE_DEF_VS_XVS	0x20 // for vs, xvs
#define NV2A_MICROCODE_DEF_XVSS		0x73 // for xvss
#define NV2A_MICROCODE_DEF_XVSW		0x77 // for xvsw

// recompile xbox vertex shader declaration
extern void EmuRecompileVSHDeclaration
(
    DWORD  *pDeclaration,
    DWORD   Handle
);

// recompile xbox vertex shader function
extern void EmuRecompileVSHFunction
(
    DWORD  *pFunction,
    DWORD **pRecompiled
);

#endif