// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->DSound.1.0.3936.cpp
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
// * DirectSoundCreate
// ******************************************************************
SOOVPA<9> DirectSoundCreate_1_0_3936 =
{
    0,  // Large == 0
    9,  // Count == 9

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // DirectSoundCreate+0x23 : add eax, 8
        { 0x23, 0x83 }, // (Offset,Value)-Pair #1
        { 0x24, 0xC0 }, // (Offset,Value)-Pair #2
        { 0x25, 0x08 }, // (Offset,Value)-Pair #3

        // DirectSoundCreate+0x34 : push 0x1C
        { 0x34, 0x6A }, // (Offset,Value)-Pair #4
        { 0x35, 0x1C }, // (Offset,Value)-Pair #5

        // DirectSoundCreate+0x75 : sbb eax, eax
        { 0x75, 0x1B }, // (Offset,Value)-Pair #6
        { 0x76, 0xC0 }, // (Offset,Value)-Pair #7

        // DirectSoundCreate+0x9B : retn 0x0C
        { 0x9B, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x9C, 0x0C }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * DirectSoundCreate_1_0_3936
// ******************************************************************
OOVPATable DSound_1_0_3936[] =
{
    // DirectSoundCreate
    {
        (OOVPA*)&DirectSoundCreate_1_0_3936,

        XTL::EmuDirectSoundCreate,

        #ifdef _DEBUG_TRACE
        "EmuDirectSoundCreate" 
        #endif
    },
};

// ******************************************************************
// * DSound_1_0_3936_SIZE
// ******************************************************************
uint32 DSound_1_0_3936_SIZE = sizeof(DSound_1_0_3936);
