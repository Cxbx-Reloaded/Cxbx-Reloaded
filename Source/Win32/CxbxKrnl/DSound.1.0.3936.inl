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
// * CDirectSound::SetDistanceFactorB
// ******************************************************************
SOOVPA<13> CDirectSound_SetDistanceFactorB_1_0_3936 =
{
    0,  // Large == 0
    13, // Count == 13

    XREF_SETDISTANCEFACTORB,// XRef Is Saved
    0,                      // XRef Not Used

    {
        // CDirectSound_SetDistanceFactorB+0x04 : or dword ptr [ecx+0x01B4], 0x60
        { 0x04, 0x83 }, // (Offset,Value)-Pair #1
        { 0x05, 0x89 }, // (Offset,Value)-Pair #2
        { 0x06, 0xB4 }, // (Offset,Value)-Pair #3
        { 0x07, 0x01 }, // (Offset,Value)-Pair #4
        { 0x08, 0x00 }, // (Offset,Value)-Pair #5
        { 0x09, 0x00 }, // (Offset,Value)-Pair #6
        { 0x0A, 0x60 }, // (Offset,Value)-Pair #7

        // CDirectSound_SetDistanceFactorB+0x0B : cmp [esp+0x08], 0
        { 0x0B, 0x83 }, // (Offset,Value)-Pair #8
        { 0x0C, 0x7C }, // (Offset,Value)-Pair #9
        { 0x0D, 0x24 }, // (Offset,Value)-Pair #10
        { 0x0E, 0x08 }, // (Offset,Value)-Pair #11

        // CDirectSound_SetDistanceFactorB+0x12 : mov [ecx+0x0178], eax
        { 0x12, 0x78 }, // (Offset,Value)-Pair #12
        { 0x13, 0x01 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * CDirectSound::SetDistanceFactorA
// ******************************************************************
SOOVPA<12> CDirectSound_SetDistanceFactorA_1_0_3936 =
{
    0,  // Large == 0
    12, // Count == 11

    XREF_SETDISTANCEFACTORA,// XRef Is Saved
    1,                      // XRef Is Used

    {
        // CDirectSound_SetDistanceFactorA+0x23 : call [CDirectSound::SetDistanceFactorB]
        { 0x23, XREF_SETDISTANCEFACTORB }, // (Offset,Value)-Pair #1

        // CDirectSound_SetDistanceFactorA+0x00 : push esi; push edi
        { 0x00, 0x56 }, // (Offset,Value)-Pair #2
        { 0x01, 0x57 }, // (Offset,Value)-Pair #3

        // CDirectSound_SetDistanceFactorA+0x07 : fld [esp+0x10]
        { 0x07, 0xD9 }, // (Offset,Value)-Pair #4
        { 0x08, 0x44 }, // (Offset,Value)-Pair #5
        { 0x09, 0x24 }, // (Offset,Value)-Pair #6
        { 0x0A, 0x10 }, // (Offset,Value)-Pair #7

        // CDirectSound_SetDistanceFactorA+0x11 : not eax; and eax, 1
        { 0x11, 0xF7 }, // (Offset,Value)-Pair #8
        { 0x12, 0xD0 }, // (Offset,Value)-Pair #9
        { 0x13, 0x83 }, // (Offset,Value)-Pair #10
        { 0x14, 0xE0 }, // (Offset,Value)-Pair #11
        { 0x15, 0x01 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirectSound8_SetDistanceFactor
// ******************************************************************
SOOVPA<11> IDirectSound8_SetDistanceFactor_1_0_3936 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    // TODO: Tentative (may not be SetDistanceFactor)

    {
        // IDirectSound8_SetDistanceFactor+0x1D : call [CDirectSound::SetDistanceFactorA]
        { 0x1D, XREF_SETDISTANCEFACTORA },  // (Offset,Value)-Pair #1

        // IDirectSound8_SetDistanceFactor+0x04 : fld [esp+0x0C]
        { 0x04, 0xD9 }, // (Offset,Value)-Pair #2
        { 0x05, 0x44 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x0C }, // (Offset,Value)-Pair #5

        // IDirectSound8_SetDistanceFactor+0x0C : push ecx
        { 0x0C, 0x51 }, // (Offset,Value)-Pair #6

        // IDirectSound8_SetDistanceFactor+0x12 : add eax, 0xFFFFFFF8
        { 0x12, 0x83 }, // (Offset,Value)-Pair #7
        { 0x13, 0xC0 }, // (Offset,Value)-Pair #8
        { 0x14, 0xF8 }, // (Offset,Value)-Pair #9

        // IDirectSound8_SetDistanceFactor+0x21 : retn 0x0C
        { 0x21, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x22, 0x0C }, // (Offset,Value)-Pair #11
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
    // CDirectSound::SetDistanceFactorB (XREF)
    {
        (OOVPA*)&CDirectSound_SetDistanceFactorB_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetDistanceFactorB (XRef)"
        #endif
    },
    // CDirectSound::SetDistanceFactorA (XREF)
    {
        (OOVPA*)&CDirectSound_SetDistanceFactorA_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetDistanceFactorA (XRef)"
        #endif
    },
    // IDirectSound8::SetDistanceFactor
    {
        (OOVPA*)&IDirectSound8_SetDistanceFactor_1_0_3936,

        XTL::EmuIDirectSound8_SetDistanceFactor,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetDistanceFactor" 
        #endif
    },
};

// ******************************************************************
// * DSound_1_0_3936_SIZE
// ******************************************************************
uint32 DSound_1_0_3936_SIZE = sizeof(DSound_1_0_3936);
