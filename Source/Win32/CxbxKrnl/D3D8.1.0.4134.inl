// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->D3D8.1.0.4134.cpp
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


// ******************************************************************
// * IDirect3DDevice8_SetRenderState_CullMode
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetRenderState_CullMode_1_0_4134 =
{
    0,  // Large == 0
    13, // Count == 13

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetRenderState_CullMode+0x19 : mov dword ptr [eax], 0x40308
        { 0x19, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x1B, 0x08 }, // (Offset,Value)-Pair #2
        { 0x1C, 0x03 }, // (Offset,Value)-Pair #3
        { 0x1D, 0x04 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_CullMode+0x24 : add eax, 8
        { 0x24, 0x83 }, // (Offset,Value)-Pair #5
        { 0x25, 0xC0 }, // (Offset,Value)-Pair #6
        { 0x26, 0x08 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetRenderState_CullMode+0x30 : retn 4
        { 0x30, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x31, 0x04 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetRenderState_CullMode+0x53 : add edx, 0x404
        { 0x53, 0x81 }, // (Offset,Value)-Pair #10
        { 0x54, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x55, 0x04 }, // (Offset,Value)-Pair #12
        { 0x56, 0x04 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * D3D8_1_0_4134
// ******************************************************************
OOVPATable D3D8_1_0_4134[] =
{
    // IDirect3D8::CreateDevice (* unchanged since 1.0.3925 *)
    {
        (OOVPA*)&IDirect3D8_CreateDevice_1_0_3925,

        XTL::EmuIDirect3D8_CreateDevice,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_CreateDevice" 
        #endif
    },
    // IDirect3DDevice8::SetRenderState_CullMode
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_CullMode_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_CullMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_CullMode" 
        #endif
    },
};

// ******************************************************************
// * D3D8_1_0_4134_SIZE
// ******************************************************************
uint32 D3D8_1_0_4134_SIZE = sizeof(D3D8_1_0_4134);
