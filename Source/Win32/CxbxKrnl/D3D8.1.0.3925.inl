// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->D3D8.1.0.3925.cpp
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
// * IDirect3D8_CreateDevice
// ******************************************************************
SOOVPA<8> IDirect3D8_CreateDevice_1_0_3925 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3D8_CreateDevice+0x07 : jnz +0x0A
        { 0x07, 0x75 }, // (Offset,Value)-Pair #1
        { 0x08, 0x0A }, // (Offset,Value)-Pair #2

        // IDirect3D8_CreateDevice+0x79 : mov eax, esi
        { 0x79, 0x8B }, // (Offset,Value)-Pair #3
        { 0x7A, 0xC6 }, // (Offset,Value)-Pair #4

        // IDirect3D8_CreateDevice+0x7C : retn 0x18
        { 0x7C, 0xC2 }, // (Offset,Value)-Pair #5
        { 0x7D, 0x18 }, // (Offset,Value)-Pair #6

        // IDirect3D8_CreateDevice+0x90 : retn 0x18
        { 0x90, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x91, 0x18 }, // (Offset,Value)-Pair #8
    }
};

// ******************************************************************
// * IDirect3DDevice8_Present
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_Present_1_0_3925 =
{
    0,  // Large == 0
    10, // Count == 10

    // TENTATIVE!

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_Present+0x0A : push 2
        { 0x0A, 0x6A }, // (Offset,Value)-Pair #1
        { 0x0B, 0x02 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_Present+0x11 : mov eax, [esi+2518]
        { 0x11, 0x8B }, // (Offset,Value)-Pair #3
        { 0x12, 0x86 }, // (Offset,Value)-Pair #4
        { 0x13, 0x18 }, // (Offset,Value)-Pair #5
        { 0x14, 0x25 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_Present+0xBE : shr ebx, 1
        { 0xBE, 0xD1 }, // (Offset,Value)-Pair #7
        { 0xBF, 0xEB }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_Present+0xC0 : xor edi, edi
        { 0xC0, 0x33 }, // (Offset,Value)-Pair #9
        { 0xC1, 0xFF }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * D3D8_1_0_3925
// ******************************************************************
OOVPATable D3D8_1_0_3925[] =
{
    // IDirect3D8::CreateDevice
    {
        (OOVPA*)&IDirect3D8_CreateDevice_1_0_3925,

        XTL::EmuIDirect3D8_CreateDevice,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_CreateDevice" 
        #endif
    },
    // IDirect3DDevice8::Present
    {
        (OOVPA*)&IDirect3DDevice8_Present_1_0_3925,

        XTL::EmuIDirect3DDevice8_Present,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Present"
        #endif
    },
};

// ******************************************************************
// * D3D8_1_0_3925_SIZE
// ******************************************************************
uint32 D3D8_1_0_3925_SIZE = sizeof(D3D8_1_0_3925);
