// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->DSound.1.0.4627.cpp
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
// * IDirectSound8_Release
// ******************************************************************
SOOVPA<10> IDirectSound8_Release_1_0_4627 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirectSound8_Release+0x04 : lea ecx, [eax-8]
        { 0x04, 0x8D }, // (Offset,Value)-Pair #1
        { 0x05, 0x48 }, // (Offset,Value)-Pair #2
        { 0x06, 0xF8 }, // (Offset,Value)-Pair #3

        // IDirectSound8_Release+0x07 : neg eax
        { 0x07, 0xF7 }, // (Offset,Value)-Pair #4
        { 0x08, 0xD8 }, // (Offset,Value)-Pair #5

        // IDirectSound8_Release+0x10 : call dword ptr [ecx+8]
        { 0x10, 0xFF }, // (Offset,Value)-Pair #6
        { 0x11, 0x51 }, // (Offset,Value)-Pair #7
        { 0x12, 0x08 }, // (Offset,Value)-Pair #8

        // IDirectSound8_Release+0x13 : retn 0x04
        { 0x13, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x14, 0x04 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirectSound8_DownloadEffectsImage
// ******************************************************************
SOOVPA<11> IDirectSound8_DownloadEffectsImage_1_0_4627 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirectSound8_DownloadEffectsImage+0x03 : push [ebp+0x18]
        { 0x03, 0xFF }, // (Offset,Value)-Pair #1
        { 0x04, 0x75 }, // (Offset,Value)-Pair #2
        { 0x05, 0x18 }, // (Offset,Value)-Pair #3

        // IDirectSound8_DownloadEffectsImage+0x06 : mov eax, [ebp+0x08]
        { 0x06, 0x8B }, // (Offset,Value)-Pair #4
        { 0x07, 0x45 }, // (Offset,Value)-Pair #5
        { 0x08, 0x08 }, // (Offset,Value)-Pair #6

        // IDirectSound8_DownloadEffectsImage+0x11 : add eax, 0xFFFFFFF8
        { 0x11, 0x83 }, // (Offset,Value)-Pair #7
        { 0x12, 0xC0 }, // (Offset,Value)-Pair #8
        { 0x13, 0xF8 }, // (Offset,Value)-Pair #9

        // IDirectSound8_DownloadEffectsImage+0x24 : retn 0x14
        { 0x24, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x25, 0x14 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * DirectSoundCreate_1_0_4627
// ******************************************************************
OOVPATable DSound_1_0_4627[] =
{
    // DirectSoundCreate (* unchanged since 4361 *)
    {
        (OOVPA*)&DirectSoundCreate_1_0_4361,

        XTL::EmuDirectSoundCreate,

        #ifdef _DEBUG_TRACE
        "EmuDirectSoundCreate" 
        #endif
    },
    // IDirectSound8::Release
    {
        (OOVPA*)&IDirectSound8_Release_1_0_4627,

        XTL::EmuIDirectSound8_Release,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_Release" 
        #endif
    },
    // IDirectSound8::DownloadEffectsImage
    {
        (OOVPA*)&IDirectSound8_DownloadEffectsImage_1_0_4627,

        XTL::EmuIDirectSound8_DownloadEffectsImage,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_DownloadEffectsImage" 
        #endif
    },
};

// ******************************************************************
// * DSound_1_0_4627_SIZE
// ******************************************************************
uint32 DSound_1_0_4627_SIZE = sizeof(DSound_1_0_4627);
