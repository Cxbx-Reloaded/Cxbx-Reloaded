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
// * CDirectSound::CreateSoundBuffer
// ******************************************************************
SOOVPA<14> CDirectSound_CreateSoundBuffer_1_0_4627 =
{
    0,  // Large == 0
    14, // Count == 14

    XREF_DSCREATESOUNDBUFFER,   // XRef Is  Saved
    0,                          // XRef Not Used

    {
        // CDirectSound_CreateSoundBuffer+0x23 : mov eax, 0x80004005
        { 0x23, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x24, 0x05 }, // (Offset,Value)-Pair #2
        { 0x25, 0x40 }, // (Offset,Value)-Pair #3
        { 0x27, 0x80 }, // (Offset,Value)-Pair #4

        // CDirectSound_CreateSoundBuffer+0x2A : push 0x24
        { 0x2A, 0x6A }, // (Offset,Value)-Pair #5
        { 0x2B, 0x24 }, // (Offset,Value)-Pair #6

        // CDirectSound_CreateSoundBuffer+0x4A : add esi, 0x7FF8FFF2
        { 0x4A, 0x81 }, // (Offset,Value)-Pair #7
        { 0x4B, 0xE6 }, // (Offset,Value)-Pair #8
        { 0x4C, 0xF2 }, // (Offset,Value)-Pair #9
        { 0x4D, 0xFF }, // (Offset,Value)-Pair #10
        { 0x4E, 0xF8 }, // (Offset,Value)-Pair #11
        { 0x4F, 0x7F }, // (Offset,Value)-Pair #12

        // CDirectSound_CreateSoundBuffer+0x99 : retn 0x10
        { 0x99, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x9A, 0x10 }, // (Offset,Value)-Pair #14
    }
};

// ******************************************************************
// * CDirectSound::SetI3DL2Listener
// ******************************************************************
SOOVPA<12> CDirectSound_SetI3DL2Listener_1_0_4627 =
{
    0,  // Large == 0
    12, // Count == 12

    XREF_DSSETI3DL2LISTENER,    // XRef Is  Saved
    0,                          // XRef Not Used

    {
        // CDirectSound_SetI3DL2Listener+0x3A : mov edi, 0x88780032
        { 0x3A, 0xBF }, // (Offset,Value)-Pair #1
        { 0x3B, 0x32 }, // (Offset,Value)-Pair #2
        { 0x3C, 0x00 }, // (Offset,Value)-Pair #3
        { 0x3D, 0x78 }, // (Offset,Value)-Pair #4
        { 0x3E, 0x88 }, // (Offset,Value)-Pair #5

        // CDirectSound_SetI3DL2Listener+0xA2 : fstp dword ptr[edx+0x94]
        { 0xA2, 0xD9 }, // (Offset,Value)-Pair #6
        { 0xA3, 0x9A }, // (Offset,Value)-Pair #7
        { 0xA4, 0x94 }, // (Offset,Value)-Pair #8

        // CDirectSound_SetI3DL2Listener+0xDC : jnz +0x06
        { 0xDC, 0x75 }, // (Offset,Value)-Pair #9
        { 0xDD, 0x06 }, // (Offset,Value)-Pair #10

        // CDirectSound_SetI3DL2Listener+0xF7 : retn 0x0C
        { 0xF7, 0xC2 }, // (Offset,Value)-Pair #11
        { 0xF8, 0x0C }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * CDirectSound::SetMixBinHeadroom
// ******************************************************************
SOOVPA<15> CDirectSound_SetMixBinHeadroom_1_0_4627 =
{
    0,  // Large == 0
    15, // Count == 15

    XREF_DSSETMIXBINHEADROOMA,  // XRef Is  Saved
    0,                          // XRef Not Used

    {
        // CDirectSound_SetMixBinHeadroom+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x22, 0x05 }, // (Offset,Value)-Pair #2
        { 0x23, 0x40 }, // (Offset,Value)-Pair #3
        { 0x24, 0x00 }, // (Offset,Value)-Pair #4
        { 0x25, 0x80 }, // (Offset,Value)-Pair #5

        // CDirectSound_SetMixBinHeadroom+0x34 : mov bl, [esp+0x14]
        { 0x34, 0x8A }, // (Offset,Value)-Pair #6
        { 0x35, 0x5C }, // (Offset,Value)-Pair #7
        { 0x36, 0x24 }, // (Offset,Value)-Pair #8
        { 0x37, 0x14 }, // (Offset,Value)-Pair #9

        // CDirectSound_SetMixBinHeadroom+0x39 : mov [edx+ecx+0x14], bl
        { 0x39, 0x88 }, // (Offset,Value)-Pair #10
        { 0x3A, 0x5C }, // (Offset,Value)-Pair #11
        { 0x3B, 0x0A }, // (Offset,Value)-Pair #12
        { 0x3C, 0x14 }, // (Offset,Value)-Pair #13

        // CDirectSound_SetMixBinHeadroom+0x5C : retn 0x0C
        { 0x5C, 0xC2 }, // (Offset,Value)-Pair #14
        { 0x5D, 0x0C }, // (Offset,Value)-Pair #15
    }
};

// ******************************************************************
// * DirectSoundCreateBuffer
// ******************************************************************
SOOVPA<12> DirectSoundCreateBuffer_1_0_4627 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
        // DirectSoundCreateBuffer+0x2F : call [CDirectSound::CreateSoundBuffer]
        { 0x2F, XREF_DSCREATESOUNDBUFFER }, // (Offset,Value)-Pair #1

        // DirectSoundCreateBuffer+0x04 : and [ebp-0x04], 0
        { 0x04, 0x83 }, // (Offset,Value)-Pair #2
        { 0x05, 0x65 }, // (Offset,Value)-Pair #3
        { 0x06, 0xFC }, // (Offset,Value)-Pair #4

        // DirectSoundCreateBuffer+0x08 : push ebx; push esi; push edi
        { 0x08, 0x53 }, // (Offset,Value)-Pair #5
        { 0x09, 0x56 }, // (Offset,Value)-Pair #6
        { 0x0A, 0x57 }, // (Offset,Value)-Pair #7

        // DirectSoundCreateBuffer+0x3C : call dword ptr [eax+8]
        { 0x3C, 0xFF }, // (Offset,Value)-Pair #8
        { 0x3D, 0x50 }, // (Offset,Value)-Pair #9
        { 0x3E, 0x08 }, // (Offset,Value)-Pair #10

        // DirectSoundCreateBuffer+0x54 : retn 0x08
        { 0x54, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x55, 0x08 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirectSound8_SetI3DL2Listener
// ******************************************************************
SOOVPA<12> IDirectSound8_SetI3DL2Listener_1_0_4627 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
        // IDirectSound8_SetI3DL2Listener+0x19 : call [CDirectSound::SetI3DL2Listener]
        { 0x19, XREF_DSSETI3DL2LISTENER }, // (Offset,Value)-Pair #1

        // IDirectSound8_SetI3DL2Listener+0x04 : push [esp+0x0C]
        { 0x04, 0xFF }, // (Offset,Value)-Pair #2
        { 0x05, 0x74 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x0C }, // (Offset,Value)-Pair #5

        // IDirectSound8_SetI3DL2Listener+0x0E : add eax, 0xFFFFFFF8
        { 0x0E, 0x83 }, // (Offset,Value)-Pair #6
        { 0x0F, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x10, 0xF8 }, // (Offset,Value)-Pair #8

        // IDirectSound8_SetI3DL2Listener+0x13 : sbb ecx, ecx
        { 0x13, 0x1B }, // (Offset,Value)-Pair #9
        { 0x14, 0xC9 }, // (Offset,Value)-Pair #10

        // IDirectSound8_SetI3DL2Listener+0x15 : and ecx, eax
        { 0x15, 0x23 }, // (Offset,Value)-Pair #11
        { 0x16, 0xC8 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirectSound8_SetMixBinHeadroom
// ******************************************************************
SOOVPA<12> IDirectSound8_SetMixBinHeadroom_1_0_4627 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
        // IDirectSound8_SetMixBinHeadroom+0x19 : call [CDirectSound::SetMixBinHeadroom]
        { 0x19, XREF_DSSETMIXBINHEADROOMA }, // (Offset,Value)-Pair #1

        // IDirectSound8_SetMixBinHeadroom+0x04 : push [esp+0x0C]
        { 0x04, 0xFF }, // (Offset,Value)-Pair #2
        { 0x05, 0x74 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x0C }, // (Offset,Value)-Pair #5

        // IDirectSound8_SetMixBinHeadroom+0x0E : add eax, 0xFFFFFFF8
        { 0x0E, 0x83 }, // (Offset,Value)-Pair #6
        { 0x0F, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x10, 0xF8 }, // (Offset,Value)-Pair #8

        // IDirectSound8_SetMixBinHeadroom+0x13 : sbb ecx, ecx
        { 0x13, 0x1B }, // (Offset,Value)-Pair #9
        { 0x14, 0xC9 }, // (Offset,Value)-Pair #10

        // IDirectSound8_SetMixBinHeadroom+0x15 : and ecx, eax
        { 0x15, 0x23 }, // (Offset,Value)-Pair #11
        { 0x16, 0xC8 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirectSound8_CreateSoundBuffer
// ******************************************************************
SOOVPA<12> IDirectSound8_CreateSoundBuffer_1_0_4627 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
        // IDirectSound8_CreateSoundBuffer+0x1D : call [CDirectSound::CreateSoundBuffer]
        { 0x1D, XREF_DSCREATESOUNDBUFFER }, // (Offset,Value)-Pair #1

        // IDirectSound8_CreateSoundBuffer+0x04 : mov eax, [esp+8]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #2
        { 0x05, 0x44 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5

        // IDirectSound8_CreateSoundBuffer+0x12 : add eax, 0xFFFFFFF8
        { 0x12, 0x83 }, // (Offset,Value)-Pair #6
        { 0x13, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x14, 0xF8 }, // (Offset,Value)-Pair #8

        // IDirectSound8_CreateSoundBuffer+0x17 : sbb ecx, ecx
        { 0x17, 0x1B }, // (Offset,Value)-Pair #9
        { 0x18, 0xC9 }, // (Offset,Value)-Pair #10

        // IDirectSound8_CreateSoundBuffer+0x21 : retn 0x10
        { 0x21, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x22, 0x10 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * CMcpxVoiceClient_SetVolume
// ******************************************************************
SOOVPA<13> CMcpxVoiceClient_SetVolume_1_0_4361 =
{
    0,  // Large == 0
    13, // Count == 13

    XREF_DSSTREAMSETVOLUME, // XRef Is  Saved
    0,                      // XRef Not Used

    {
        // CMcpxVoiceClient_SetVolume+0x2A : lea eax, [ecx+ecx*2]
        { 0x2A, 0x8D }, // (Offset,Value)-Pair #1
        { 0x2B, 0x04 }, // (Offset,Value)-Pair #2
        { 0x2C, 0x49 }, // (Offset,Value)-Pair #3

        // CMcpxVoiceClient_SetVolume+0x45 : movzx edx, word ptr [ecx]
        { 0x45, 0x0F }, // (Offset,Value)-Pair #4
        { 0x46, 0xB7 }, // (Offset,Value)-Pair #5
        { 0x47, 0x11 }, // (Offset,Value)-Pair #6

        // CMcpxVoiceClient_SetVolume+0x6C : mov edx, [ebp+eax*4-0x14]
        { 0x6C, 0x8B }, // (Offset,Value)-Pair #7
        { 0x6D, 0x54 }, // (Offset,Value)-Pair #8
        { 0x6E, 0x85 }, // (Offset,Value)-Pair #9
        { 0x6F, 0xEC }, // (Offset,Value)-Pair #10

        // CMcpxVoiceClient_SetVolume+0x84 : inc eax; inc ecx, inc ecx
        { 0x84, 0x40 }, // (Offset,Value)-Pair #11
        { 0x85, 0x41 }, // (Offset,Value)-Pair #12
        { 0x86, 0x41 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * CDirectSoundStream_SetVolume
// ******************************************************************
SOOVPA<11> CDirectSoundStream_SetVolume_1_0_4361 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
        // CDirectSoundBuffer_SetBufferData+0x15 : call [CMcpxVoiceClient::SetVolume]
        { 0x15, XREF_DSSTREAMSETVOLUME },  // (Offset,Value)-Pair #1

        // CDirectSoundBuffer_SetBufferData+0x00 : mov ecx, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #2
        { 0x01, 0x4C }, // (Offset,Value)-Pair #3
        { 0x02, 0x24 }, // (Offset,Value)-Pair #4
        { 0x03, 0x04 }, // (Offset,Value)-Pair #5

        // CDirectSoundBuffer_SetBufferData+0x0B : sub edx, [eax+0x20]
        { 0x0B, 0x2B }, // (Offset,Value)-Pair #6
        { 0x0C, 0x50 }, // (Offset,Value)-Pair #7
        { 0x0D, 0x20 }, // (Offset,Value)-Pair #8

        // CDirectSoundBuffer_SetBufferData+0x11 : mov ecx, [ecx+0x0C]
        { 0x11, 0x8B }, // (Offset,Value)-Pair #9
        { 0x12, 0x49 }, // (Offset,Value)-Pair #10
        { 0x13, 0x0C }, // (Offset,Value)-Pair #11
    }
};

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
// * CDirectSound::SetDistanceFactorA
// ******************************************************************
SOOVPA<11> CDirectSound_SetDistanceFactorA_1_0_4627 =
{
    0,  // Large == 0
    11, // Count == 11

    XREF_SETDISTANCEFACTORA,// XRef Is Saved
    0,                      // XRef Not Used

    {
        // CDirectSound_SetDistanceFactorA+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x22, 0x05 }, // (Offset,Value)-Pair #2
        { 0x23, 0x40 }, // (Offset,Value)-Pair #3
        { 0x24, 0x00 }, // (Offset,Value)-Pair #4
        { 0x25, 0x80 }, // (Offset,Value)-Pair #5

        // CDirectSound_SetDistanceFactorA+0x39 : or byte ptr[eax+0xA4], 0xE0
        { 0x39, 0x80 }, // (Offset,Value)-Pair #6
        { 0x3A, 0x88 }, // (Offset,Value)-Pair #7
        { 0x3B, 0xA4 }, // (Offset,Value)-Pair #8
        { 0x3F, 0xE0 }, // (Offset,Value)-Pair #9

        // CDirectSound_SetDistanceFactorA+0x4F : jz +0x0B
        { 0x4F, 0x74 }, // (Offset,Value)-Pair #10
        { 0x50, 0x0B }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirectSound8_SetDistanceFactor
// ******************************************************************
SOOVPA<11> IDirectSound8_SetDistanceFactor_1_0_4627 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
        // IDirectSound8_SetDistanceFactor+0x1D : call [CDirectSound::SetDistanceFactor]
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
// * CDirectSound::SetRolloffFactor
// ******************************************************************
SOOVPA<11> CDirectSound_SetRolloffFactor_1_0_4627 =
{
    0,  // Large == 0
    11, // Count == 11

    XREF_SETROLLOFFFACTORA, // XRef Is Saved
    0,                      // XRef Not Used

    {
        // CDirectSound_SetRolloffFactor+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x22, 0x05 }, // (Offset,Value)-Pair #2
        { 0x23, 0x40 }, // (Offset,Value)-Pair #3
        { 0x24, 0x00 }, // (Offset,Value)-Pair #4
        { 0x25, 0x80 }, // (Offset,Value)-Pair #5

        // CDirectSound_SetRolloffFactor+0x39 : or dword ptr[eax+0xA4], 0x04
        { 0x39, 0x83 }, // (Offset,Value)-Pair #6
        { 0x3A, 0x88 }, // (Offset,Value)-Pair #7
        { 0x3B, 0xA4 }, // (Offset,Value)-Pair #8
        { 0x3F, 0x04 }, // (Offset,Value)-Pair #9

        // CDirectSound_SetRolloffFactor+0x4F : jz +0x0B
        { 0x4F, 0x74 }, // (Offset,Value)-Pair #10
        { 0x50, 0x0B }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirectSound8_SetRolloffFactor
// ******************************************************************
SOOVPA<11> IDirectSound8_SetRolloffFactor_1_0_4627 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
        // IDirectSound8_SetRolloffFactor+0x1D : call [CDirectSound::SetRolloffFactor]
        { 0x1D, XREF_SETROLLOFFFACTORA },  // (Offset,Value)-Pair #1

        // IDirectSound8_SetRolloffFactor+0x04 : fld [esp+0x0C]
        { 0x04, 0xD9 }, // (Offset,Value)-Pair #2
        { 0x05, 0x44 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x0C }, // (Offset,Value)-Pair #5

        // IDirectSound8_SetRolloffFactor+0x0C : push ecx
        { 0x0C, 0x51 }, // (Offset,Value)-Pair #6

        // IDirectSound8_SetRolloffFactor+0x12 : add eax, 0xFFFFFFF8
        { 0x12, 0x83 }, // (Offset,Value)-Pair #7
        { 0x13, 0xC0 }, // (Offset,Value)-Pair #8
        { 0x14, 0xF8 }, // (Offset,Value)-Pair #9

        // IDirectSound8_SetRolloffFactor+0x21 : retn 0x0C
        { 0x21, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x22, 0x0C }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * CDirectSound::SetDopplerFactor
// ******************************************************************
SOOVPA<14> CDirectSound_SetDopplerFactor_1_0_4627 =
{
    0,  // Large == 0
    14, // Count == 14

    XREF_SETDOPPLERFACTOR,  // XRef Is Saved
    0,                      // XRef Not Used

    {
        // CDirectSound_SetDopplerFactor+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x22, 0x05 }, // (Offset,Value)-Pair #2
        { 0x23, 0x40 }, // (Offset,Value)-Pair #3
        { 0x24, 0x00 }, // (Offset,Value)-Pair #4
        { 0x25, 0x80 }, // (Offset,Value)-Pair #5

        // CDirectSound_SetDopplerFactor+0x33 : mov [eax+0x70], edx
        { 0x33, 0x89 }, // (Offset,Value)-Pair #6
        { 0x34, 0x50 }, // (Offset,Value)-Pair #7
        { 0x35, 0x70 }, // (Offset,Value)-Pair #8

        // CDirectSound_SetDopplerFactor+0x39 : or byte ptr[eax+0xA4], 0x80
        { 0x39, 0x80 }, // (Offset,Value)-Pair #9
        { 0x3A, 0x88 }, // (Offset,Value)-Pair #10
        { 0x3B, 0xA4 }, // (Offset,Value)-Pair #11
        { 0x3F, 0x80 }, // (Offset,Value)-Pair #12

        // CDirectSound_SetDopplerFactor+0x4F : jz +0x0B
        { 0x4F, 0x74 }, // (Offset,Value)-Pair #13
        { 0x50, 0x0B }, // (Offset,Value)-Pair #14
    }
};

// ******************************************************************
// * IDirectSound8_SetDopplerFactor
// ******************************************************************
SOOVPA<11> IDirectSound8_SetDopplerFactor_1_0_4627 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
        // IDirectSound8_SetDopplerFactor+0x1D : call [CDirectSound::SetDopplerFactor]
        { 0x1D, XREF_SETDOPPLERFACTOR},  // (Offset,Value)-Pair #1

        // IDirectSound8_SetDopplerFactor+0x04 : fld [esp+0x0C]
        { 0x04, 0xD9 }, // (Offset,Value)-Pair #2
        { 0x05, 0x44 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x0C }, // (Offset,Value)-Pair #5

        // IDirectSound8_SetDopplerFactor+0x0C : push ecx
        { 0x0C, 0x51 }, // (Offset,Value)-Pair #6

        // IDirectSound8_SetDopplerFactor+0x12 : add eax, 0xFFFFFFF8
        { 0x12, 0x83 }, // (Offset,Value)-Pair #7
        { 0x13, 0xC0 }, // (Offset,Value)-Pair #8
        { 0x14, 0xF8 }, // (Offset,Value)-Pair #9

        // IDirectSound8_SetDopplerFactor+0x21 : retn 0x0C
        { 0x21, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x22, 0x0C }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * CDirectSound::CommitDeferredSettings
// ******************************************************************
SOOVPA<11> CDirectSound_CommitDeferredSettings_1_0_4627 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // CDirectSound_CommitDeferredSettings+0x10 : movzx eax, al
        { 0x10, 0x0F }, // (Offset,Value)-Pair #1
        { 0x11, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x12, 0xC0 }, // (Offset,Value)-Pair #3

        // CDirectSound_CommitDeferredSettings+0x27 : mov eax, 0x80004005
        { 0x27, 0xB8 }, // (Offset,Value)-Pair #4
        { 0x28, 0x05 }, // (Offset,Value)-Pair #5
        { 0x29, 0x40 }, // (Offset,Value)-Pair #6
        { 0x2B, 0x80 }, // (Offset,Value)-Pair #7

        // CDirectSound_CommitDeferredSettings+0x5C : and [eax+0xA4], esi
        { 0x5C, 0x21 }, // (Offset,Value)-Pair #8
        { 0x5D, 0xB0 }, // (Offset,Value)-Pair #9
        { 0x5E, 0xA4 }, // (Offset,Value)-Pair #10

        // CDirectSound_CommitDeferredSettings+0x78 : leave
        { 0x78, 0xC9 }, // (Offset,Value)-Pair #11
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
    // CDirectSound_CreateSoundBuffer
    {
        (OOVPA*)&CDirectSound_CreateSoundBuffer_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound::CreateSoundBuffer (XREF)" 
        #endif
    },
    // CDirectSound_SetI3DL2Listener
    {
        (OOVPA*)&CDirectSound_SetI3DL2Listener_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound::SetI3DL2Listener (XREF)" 
        #endif
    },
    // CDirectSound_SetMixBinHeadroom
    {
        (OOVPA*)&CDirectSound_SetMixBinHeadroom_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound::SetMixBinHeadroom (XREF)" 
        #endif
    },
    // DirectSoundCreateBuffer
    {
        (OOVPA*)&DirectSoundCreateBuffer_1_0_4627,

        XTL::EmuDirectSoundCreateBuffer,

        #ifdef _DEBUG_TRACE
        "EmuDirectSoundCreateBuffer" 
        #endif
    },
    // IDirectSound8::CreateSoundBuffer
    {
        (OOVPA*)&IDirectSound8_CreateSoundBuffer_1_0_4627,

        XTL::EmuIDirectSound8_CreateSoundBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_CreateSoundBuffer" 
        #endif
    },
    // IDirectSound8_SetI3DL2Listener
    {
        (OOVPA*)&IDirectSound8_SetI3DL2Listener_1_0_4627,

        XTL::EmuIDirectSound8_SetI3DL2Listener,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetI3DL2Listener" 
        #endif
    },
    // IDirectSound8_SetMixBinHeadroom
    {
        (OOVPA*)&IDirectSound8_SetMixBinHeadroom_1_0_4627,

        XTL::EmuIDirectSound8_SetMixBinHeadroom,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetMixBinHeadroom"
        #endif
    },
    // CMcpxVoiceClient_SetVolume
    {
        (OOVPA*)&CMcpxVoiceClient_SetVolume_1_0_4361, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxVoiceClient::SetVolume (XREF)" 
        #endif
    },
    // CDirectSoundStream_SetVolume
    {
        (OOVPA*)&CDirectSoundStream_SetVolume_1_0_4361, 
            
        XTL::EmuCDirectSoundStream_SetVolume,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSoundStream_SetVolume" 
        #endif
    },
    // CDirectSound_CreateSoundStream (* unchanged since 4361 *)
    {
        (OOVPA*)&CDirectSound_CreateSoundStream_1_0_4361, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound::CreateSoundStream (XREF)" 
        #endif
    },
    // DirectSoundCreateStream (* unchanged since 4361 *)
    {
        (OOVPA*)&DirectSoundCreateStream_1_0_4361,

        XTL::EmuDirectSoundCreateStream,

        #ifdef _DEBUG_TRACE
        "EmuDirectSoundCreateStream" 
        #endif
    },
    // CMcpxBuffer::SetBufferData (* unchanged since 4361 *)
    {
        (OOVPA*)&CMcpxBuffer_SetBufferData_1_0_4361, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxBuffer_SetBufferData (XREF)"
        #endif
    },
    // CDirectSoundBuffer::SetBufferData (* unchanged since 4361 *)
    {
        (OOVPA*)&CDirectSoundBuffer_SetBufferData_1_0_4361, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer_SetBufferData (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::SetBufferData (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirectSoundBuffer8_SetBufferData_1_0_4361, 
            
        XTL::EmuIDirectSoundBuffer8_SetBufferData,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetBufferData"
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
    // IDirectSound8::DownloadEffectsImage (* unchanged since 3936 *)
    {
        (OOVPA*)&IDirectSound8_DownloadEffectsImage_1_0_3936,

        XTL::EmuIDirectSound8_DownloadEffectsImage,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_DownloadEffectsImage" 
        #endif
    },
    // IDirectSound8::SetOrientation (* unchanged since 3936 *)
    {
        (OOVPA*)&IDirectSound8_SetOrientation_1_0_3936,

        XTL::EmuIDirectSound8_SetOrientation,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetOrientation" 
        #endif
    },
    // CDirectSound::SetDistanceFactorA (XREF)
    {
        (OOVPA*)&CDirectSound_SetDistanceFactorA_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetDistanceFactorA (XRef)"
        #endif
    },
    // IDirectSound8::SetDistanceFactor
    {
        (OOVPA*)&IDirectSound8_SetDistanceFactor_1_0_4627,

        XTL::EmuIDirectSound8_SetDistanceFactor,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetDistanceFactor" 
        #endif
    },
    // CDirectSound::SetRolloffFactor (XREF)
    {
        (OOVPA*)&CDirectSound_SetRolloffFactor_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetRolloffFactor (XRef)"
        #endif
    },
    // IDirectSound8::SetRolloffFactor
    {
        (OOVPA*)&IDirectSound8_SetRolloffFactor_1_0_4627,

        XTL::EmuIDirectSound8_SetRolloffFactor,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetRolloffFactor" 
        #endif
    },
    // CDirectSound::SetDopplerFactor (XREF)
    {
        (OOVPA*)&CDirectSound_SetDopplerFactor_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetDopplerFactor (XRef)"
        #endif
    },
    // IDirectSound8::SetDopplerFactor
    {
        (OOVPA*)&IDirectSound8_SetDopplerFactor_1_0_4627,

        XTL::EmuIDirectSound8_SetDopplerFactor,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetDopplerFactor" 
        #endif
    },
    // CDirectSound::CommitDeferredSettings
    {
        (OOVPA*)&CDirectSound_CommitDeferredSettings_1_0_4627,

        XTL::EmuCDirectSound_CommitDeferredSettings,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSound_CommitDeferredSettings" 
        #endif
    },
};

// ******************************************************************
// * DSound_1_0_4627_SIZE
// ******************************************************************
uint32 DSound_1_0_4627_SIZE = sizeof(DSound_1_0_4627);
