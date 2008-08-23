// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->DSound.1.0.4432.cpp
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
SOOVPA<12> IDirectSound8_Release_1_0_4432 =
{
    0,  // Large == 0
    12, // Count == 12

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

        // IDirectSound8_Release+0x09 : sbb eax, eax
        { 0x09, 0x1B }, // (Offset,Value)-Pair #6
        { 0x0A, 0xC0 }, // (Offset,Value)-Pair #7

        // IDirectSound8_Release+0x0D : mov ecx, [eax]
        { 0x0D, 0x8B }, // (Offset,Value)-Pair #8
        { 0x0E, 0x08 }, // (Offset,Value)-Pair #9

        // IDirectSound8_Release+0x10 : call dword ptr [ecx+8]
        { 0x10, 0xFF }, // (Offset,Value)-Pair #10
        { 0x11, 0x51 }, // (Offset,Value)-Pair #11
        { 0x12, 0x08 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * CDirectSoundVoice::SetMaxDistance
// ******************************************************************
SOOVPA<9> CDirectSoundVoice_SetMaxDistance_1_0_4432 =
{
    0,  // Large == 0
    9,  // Count == 9

    XREF_DSVOICESETMAXDISTANCE, // XRef Is Saved
    0,                          // XRef Not Used

    {
        // CDirectSoundVoice_SetMaxDistance+0x0D : mov edx, [esp+arg_4]
        { 0x0D, 0x8B },
        { 0x0E, 0x54 },
        { 0x0F, 0x24 },
        { 0x10, 0x08 },

        // CDirectSoundVoice_SetMaxDistance+0x11 : mov [eax+38h], edx
        { 0x11, 0x89 },
        { 0x12, 0x50 },
        { 0x13, 0x38 },

        // CDirectSoundVoice_SetMaxDistance+0x30 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 }
    }
};

// ******************************************************************
// * CDirectSoundBuffer::SetMaxDistance
// ******************************************************************
SOOVPA<10> CDirectSoundBuffer_SetMaxDistance_1_0_4432 =
{
    0,  // Large == 0
    10, // Count == 10

    XREF_DSBUFFERSETMAXDISTANCE, // XRef Is Saved
    1,                           // XRef Is Used

    {
        // CDirectSoundBuffer_SetMaxDistance+0x39 : call [CDirectSoundVoice::SetMaxDistance]
        { 0x3A, XREF_DSVOICESETMAXDISTANCE },

        // CDirectSoundBuffer_SetMaxDistance+0x21 : mov eax, 80004005h
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // CDirectSoundBuffer_SetMaxDistance+0x32 : fstp    [esp+0Ch+var_C]
        { 0x32, 0xD9 },
        { 0x33, 0x1C },
        { 0x34, 0x24 },

        // CDirectSoundBuffer_SetMaxDistance+0x53 : retn 0Ch
        { 0x54, 0x0C },
        { 0x55, 0x00 }
    }
};

// ******************************************************************
// * IDirectSoundBuffer_SetMaxDistance
// ******************************************************************
SOOVPA<10> IDirectSoundBuffer_SetMaxDistance_1_0_4432 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    1,  // XRef Is Used

    {
        // IDirectSoundBuffer_SetMaxDistance+0x1C : call [CDirectSoundBuffer::SetMaxDistance]
        { 0x1D, XREF_DSBUFFERSETMAXDISTANCE },

        // IDirectSoundBuffer_SetMaxDistance+0x04 : fld [esp+4+arg_4]
        { 0x04, 0xD9 },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x0C },

        // IDirectSoundBuffer_SetMaxDistance+0x12 : add eax, 0FFFFFFE4
        { 0x12, 0x83 },
        { 0x13, 0xC0 },
        { 0x14, 0xE4 },

        // IDirectSoundBuffer_SetMaxDistance+0x21 : retn 0Ch
        { 0x22, 0x0C },
        { 0x23, 0x00 }
    }
};

// ******************************************************************
// * CDirectSoundVoice::SetMinDistance
// ******************************************************************
SOOVPA<9> CDirectSoundVoice_SetMinDistance_1_0_4432 =
{
    0,  // Large == 0
    9,  // Count == 9

    XREF_DSVOICESETMINDISTANCE, // XRef Is Saved
    0,                          // XRef Not Used

    {
        // CDirectSoundVoice_SetMinDistance+0x0D : mov edx, [esp+arg_4]
        { 0x0D, 0x8B },
        { 0x0E, 0x54 },
        { 0x0F, 0x24 },
        { 0x10, 0x08 },

        // CDirectSoundVoice_SetMinDistance+0x11 : mov [eax+34h], edx
        { 0x11, 0x89 },
        { 0x12, 0x50 },
        { 0x13, 0x34 },

        // CDirectSoundVoice_SetMinDistance+0x30 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 }
    }
};

// ******************************************************************
// * CDirectSoundBuffer::SetMinDistance
// ******************************************************************
SOOVPA<10> CDirectSoundBuffer_SetMinDistance_1_0_4432 =
{
    0,  // Large == 0
    10, // Count == 10

    XREF_DSBUFFERSETMINDISTANCE, // XRef Is Saved
    1,                           // XRef Is Used

    {
        // CDirectSoundBuffer_SetMinDistance+0x39 : call [CDirectSoundVoice::SetMinDistance]
        { 0x3A, XREF_DSVOICESETMINDISTANCE },

        // CDirectSoundBuffer_SetMinDistance+0x21 : mov eax, 80004005h
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // CDirectSoundBuffer_SetMinDistance+0x32 : fstp    [esp+0Ch+var_C]
        { 0x32, 0xD9 },
        { 0x33, 0x1C },
        { 0x34, 0x24 },

        // CDirectSoundBuffer_SetMinDistance+0x53 : retn 0Ch
        { 0x54, 0x0C },
        { 0x55, 0x00 }
    }
};

// ******************************************************************
// * IDirectSoundBuffer_SetMinDistance
// ******************************************************************
SOOVPA<10> IDirectSoundBuffer_SetMinDistance_1_0_4432 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    1,  // XRef Is Used

    {
        // IDirectSoundBuffer_SetMinDistance+0x1C : call [CDirectSoundBuffer::SetMinDistance]
        { 0x1D, XREF_DSBUFFERSETMINDISTANCE },

        // IDirectSoundBuffer_SetMinDistance+0x04 : fld [esp+4+arg_4]
        { 0x04, 0xD9 },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x0C },

        // IDirectSoundBuffer_SetMinDistance+0x12 : add eax, 0FFFFFFE4
        { 0x12, 0x83 },
        { 0x13, 0xC0 },
        { 0x14, 0xE4 },

        // IDirectSoundBuffer_SetMinDistance+0x21 : retn 0Ch
        { 0x22, 0x0C },
        { 0x23, 0x00 }
    }
};

// ******************************************************************
// * CDirectSound::SetI3DL2Listener
// ******************************************************************
SOOVPA<11> CDirectSound_SetI3DL2Listener_1_0_4432 =
{
    0,  // Large == 0
    11, // Count == 12

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

        // CDirectSound_SetI3DL2Listener+0xA8 : fld dword ptr[ecx+0x24]
        { 0xA8, 0xD9 }, // (Offset,Value)-Pair #9
        { 0xA9, 0x41 }, // (Offset,Value)-Pair #10
        { 0xAA, 0x24 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirectSound8_SetI3DL2Listener
// ******************************************************************
SOOVPA<12> IDirectSound8_SetI3DL2Listener_1_0_4432 =
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
// * CDirectSoundVoiceSettings::SetMixBins
// ******************************************************************
SOOVPA<10> CDirectSoundVoiceSettings_SetMixBins_1_0_4432 =
{
    0,  // Large == 0
    10, // Count == 10

    XREF_DSSETMIXBINSC, // XRef Is  Saved
    0,                  // XRef Not Used

    {
        // CDirectSoundVoiceSettings_SetMixBins+0x06 : jnz +0x1A
        { 0x06, 0x75 }, // (Offset,Value)-Pair #1
        { 0x07, 0x1A }, // (Offset,Value)-Pair #2

        // CDirectSoundVoiceSettings_SetMixBins+0x08 : test byte ptr [ecx+8], 0x10
        { 0x08, 0xF6 }, // (Offset,Value)-Pair #3
        { 0x09, 0x41 }, // (Offset,Value)-Pair #4
        { 0x0A, 0x08 }, // (Offset,Value)-Pair #5
        { 0x0B, 0x10 }, // (Offset,Value)-Pair #6

        // CDirectSoundVoiceSettings_SetMixBins+0x2B : jbe +0x2B
        { 0x2B, 0x76 }, // (Offset,Value)-Pair #7
        { 0x2C, 0x2B }, // (Offset,Value)-Pair #8

        // CDirectSoundVoiceSettings_SetMixBins+0xB2 : retn 0x04
        { 0xB2, 0xC2 }, // (Offset,Value)-Pair #9
        { 0xB3, 0x04 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * CDirectSoundVoice::SetMixBins
// ******************************************************************
SOOVPA<17> CDirectSoundVoice_SetMixBins_1_0_4432 =
{
    0,  // Large == 0
    17, // Count == 17

    XREF_DSSETMIXBINSB, // XRef Is Saved
    1,                  // XRef Is Used

    {
        // CDirectSoundVoice_SetMixBins+0x0D : call [CDirectSoundVoiceSettings::SetMixBins]
        { 0x0D, XREF_DSSETMIXBINSC }, // (Offset,Value)-Pair #1

        // CDirectSoundVoice_SetMixBins+0x01 : mov esi, [esp+8]
        { 0x01, 0x8B }, // (Offset,Value)-Pair #2
        { 0x02, 0x74 }, // (Offset,Value)-Pair #3
        { 0x03, 0x24 }, // (Offset,Value)-Pair #4
        { 0x04, 0x08 }, // (Offset,Value)-Pair #5

        // CDirectSoundVoice_SetMixBins+0x05 : push [esp+0x0C]
        { 0x05, 0xFF }, // (Offset,Value)-Pair #6
        { 0x06, 0x74 }, // (Offset,Value)-Pair #7
        { 0x07, 0x24 }, // (Offset,Value)-Pair #8
        { 0x08, 0x0C }, // (Offset,Value)-Pair #9

        // CDirectSoundVoice_SetMixBins+0x09 : mov ecx, [esi+0x10]
        { 0x09, 0x8B }, // (Offset,Value)-Pair #10
        { 0x0A, 0x4E }, // (Offset,Value)-Pair #11
        { 0x0B, 0x10 }, // (Offset,Value)-Pair #12

        // CDirectSoundVoice_SetMixBins+0x11 : mov ecx, [esi+0x0C]
        { 0x11, 0x8B }, // (Offset,Value)-Pair #13
        { 0x12, 0x4E }, // (Offset,Value)-Pair #14
        { 0x13, 0x0C }, // (Offset,Value)-Pair #15

        // CDirectSoundVoice_SetMixBins+0x1A : retn 0x08
        { 0x1A, 0xC2 }, // (Offset,Value)-Pair #16
        { 0x1B, 0x08 }, // (Offset,Value)-Pair #17
    }
};

// ******************************************************************
// * CDirectSoundBuffer::SetMixBins
// ******************************************************************
SOOVPA<17> CDirectSoundBuffer_SetMixBins_1_0_4432 =
{
    0,  // Large == 0
    17, // Count == 17

    XREF_DSSETMIXBINSA, // XRef Is Saved
    1,                  // XRef Is Used

    {
        // CDirectSoundBuffer_SetMixBins+0x32 : call [CDirectSoundVoice::SetMixBins]
        { 0x32, XREF_DSSETMIXBINSB }, // (Offset,Value)-Pair #1

        // CDirectSoundBuffer_SetMixBins+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 }, // (Offset,Value)-Pair #2
        { 0x22, 0x05 }, // (Offset,Value)-Pair #3
        { 0x23, 0x40 }, // (Offset,Value)-Pair #4
        { 0x24, 0x00 }, // (Offset,Value)-Pair #5
        { 0x25, 0x80 }, // (Offset,Value)-Pair #6

        // CDirectSoundBuffer_SetMixBins+0x29 : push [esp+0x10]; push [esp+0x10]
        { 0x29, 0xFF }, // (Offset,Value)-Pair #7
        { 0x2A, 0x74 }, // (Offset,Value)-Pair #8
        { 0x2B, 0x24 }, // (Offset,Value)-Pair #9
        { 0x2C, 0x10 }, // (Offset,Value)-Pair #10
        { 0x2D, 0xFF }, // (Offset,Value)-Pair #11
        { 0x2E, 0x74 }, // (Offset,Value)-Pair #12
        { 0x2F, 0x24 }, // (Offset,Value)-Pair #13
        { 0x30, 0x10 }, // (Offset,Value)-Pair #14

        // CDirectSoundBuffer_SetMixBins+0x49 : pop edi
        { 0x49, 0x5F }, // (Offset,Value)-Pair #15

        // CDirectSoundBuffer_SetMixBins+0x4B : retn 0x08
        { 0x4B, 0xC2 }, // (Offset,Value)-Pair #16
        { 0x4C, 0x08 }, // (Offset,Value)-Pair #17
    }
};

// ******************************************************************
// * IDirectSoundBuffer8_SetMixBins
// ******************************************************************
SOOVPA<12> IDirectSoundBuffer8_SetMixBins_1_0_4432 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
        // IDirectSoundBuffer8_SetMixBins+0x15 : call [CDirectSoundBuffer::SetMixBins]
        { 0x15, XREF_DSSETMIXBINSA }, // (Offset,Value)-Pair #1

        // IDirectSoundBuffer8_SetMixBins+0x04 : push [esp+0x08]
        { 0x04, 0xFF }, // (Offset,Value)-Pair #2
        { 0x05, 0x74 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5

        // IDirectSoundBuffer8_SetMixBins+0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 }, // (Offset,Value)-Pair #6
        { 0x0B, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x0C, 0xE4 }, // (Offset,Value)-Pair #8

        // IDirectSoundBuffer8_SetMixBins+0x0F : sbb ecx, ecx
        { 0x0F, 0x1B }, // (Offset,Value)-Pair #9
        { 0x10, 0xC9 }, // (Offset,Value)-Pair #10

        // IDirectSoundBuffer8_SetMixBins+0x11 : and ecx, eax
        { 0x11, 0x23 }, // (Offset,Value)-Pair #11
        { 0x12, 0xC8 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * CDirectSound::CommitDeferredSettings
// ******************************************************************
SOOVPA<11> CDirectSound_CommitDeferredSettings_1_0_4432 =
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
// * CDirectSound::SetDistanceFactorA
// ******************************************************************
SOOVPA<11> CDirectSound_SetDistanceFactorA_1_0_4432 =
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

        // CDirectSound_SetDistanceFactorA+0x39 : or dword ptr[eax+0xA4], 0x60
        { 0x39, 0x83 }, // (Offset,Value)-Pair #6
        { 0x3A, 0x88 }, // (Offset,Value)-Pair #7
        { 0x3B, 0xA4 }, // (Offset,Value)-Pair #8
        { 0x3F, 0x60 }, // (Offset,Value)-Pair #9

        // CDirectSound_SetDistanceFactorA+0x4F : jz +0x0B
        { 0x4F, 0x74 }, // (Offset,Value)-Pair #10
        { 0x50, 0x0B }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirectSound8_SetDistanceFactor
// ******************************************************************
SOOVPA<11> IDirectSound8_SetDistanceFactor_1_0_4432 =
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
// * DirectSoundCreate_1_0_4432
// ******************************************************************
OOVPATable DSound_1_0_4432[] =
{
    // DirectSoundCreate (* unchanged since 4361 *)
    {
        (OOVPA*)&DirectSoundCreate_1_0_4361,

        XTL::EmuDirectSoundCreate,

        #ifdef _DEBUG_TRACE
        "EmuDirectSoundCreate"
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
    // CMcpxStream::Pause (* unchanged since 4361 *)
    {
        (OOVPA*)&CMcpxStream_Pause_1_0_4361, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxStream_Pause (XREF)"
        #endif
    },
    // CDirectSoundStream::Pause (* unchanged since 4361 *)
    {
        (OOVPA*)&CDirectSoundStream_Pause_1_0_4361,

        XTL::EmuCDirectSoundStream_Pause,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_Pause"
        #endif
    },
    // IDirectSound8::AddRef (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirectSound8_AddRef_1_0_4361,

        XTL::EmuIDirectSound8_AddRef,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_AddRef"
        #endif
    },
    // IDirectSound8::Release
    {
        (OOVPA*)&IDirectSound8_Release_1_0_4432,

        XTL::EmuIDirectSound8_Release,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_Release"
        #endif
    },
    // IDirectSound8::CreateSoundBuffer (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirectSound8_CreateSoundBuffer_1_0_4361,

        XTL::EmuIDirectSound8_CreateSoundBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_CreateSoundBuffer"
        #endif
    },
    // IDirectSoundBuffer8::Release (* unchanged since 3936 *)
    {
        (OOVPA*)&IDirectSoundBuffer8_Release_1_0_3936,

        XTL::EmuIDirectSoundBuffer8_Release,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_Release"
        #endif
    },
    // IDirectSoundBuffer8::SetLoopRegion (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirectSoundBuffer8_SetLoopRegion_1_0_4361,

        XTL::EmuIDirectSoundBuffer8_SetLoopRegion,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetLoopRegion"
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
    // CMcpxBuffer::SetCurrentPosition (* unchanged since 4361 *)
    {
        (OOVPA*)&CMcpxBuffer_SetCurrentPosition_1_0_4361, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxBuffer_SetCurrentPosition (XREF)"
        #endif
    },
    // CDirectSoundBuffer::SetCurrentPosition (* unchanged since 4361 *)
    {
        (OOVPA*)&CDirectSoundBuffer_SetCurrentPosition_1_0_4361, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer_SetCurrentPosition (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::SetCurrentPosition (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirectSoundBuffer8_SetCurrentPosition_1_0_4361,

        XTL::EmuIDirectSoundBuffer8_SetCurrentPosition,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetCurrentPosition"
        #endif
    },
    // CMcpxBuffer::Stop (* unchanged since 4361 *)
    {
        (OOVPA*)&CMcpxBuffer_Stop_1_0_4361, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxBuffer_Stop (XREF)"
        #endif
    },
    // CDirectSoundBuffer::Stop (* unchanged since 4361 *)
    {
        (OOVPA*)&CDirectSoundBuffer_Stop_1_0_4361, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer_Stop (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::Stop (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirectSoundBuffer8_Stop_1_0_4361,

        XTL::EmuIDirectSoundBuffer8_Stop,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_Stop"
        #endif
    },
    // CDirectSoundVoice::SetMaxDistance (XREF)
    {
        (OOVPA*)&CDirectSoundVoice_SetMaxDistance_1_0_4432, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetMaxDistance (XRef)"
        #endif
    },
    // CDirectSoundBuffer::SetMaxDistance (XREF)
    {
        (OOVPA*)&CDirectSoundBuffer_SetMaxDistance_1_0_4432, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetMaxDistance (XRef)"
        #endif
    },
    // IDirectSoundBuffer_SetMaxDistance
    {
        (OOVPA*)&IDirectSoundBuffer_SetMaxDistance_1_0_4432,
        XTL::EmuIDirectSoundBuffer8_SetMaxDistance,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetMaxDistance"
        #endif
    },
    // CDirectSoundVoice::SetMinDistance (XREF)
    {
        (OOVPA*)&CDirectSoundVoice_SetMinDistance_1_0_4432, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetMinDistance (XRef)"
        #endif
    },
    // CDirectSoundBuffer::SetMinDistance (XREF)
    {
        (OOVPA*)&CDirectSoundBuffer_SetMinDistance_1_0_4432, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetMinDistance (XRef)"
        #endif
    },
    // IDirectSoundBuffer_SetMinDistance
    {
        (OOVPA*)&IDirectSoundBuffer_SetMinDistance_1_0_4432,
        XTL::EmuIDirectSoundBuffer8_SetMinDistance,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetMinDistance"
        #endif
    },
    // CDirectSound_SetI3DL2Listener
    {
        (OOVPA*)&CDirectSound_SetI3DL2Listener_1_0_4432, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound::SetI3DL2Listener (XREF)"
        #endif
    },
    // IDirectSound8_SetI3DL2Listener
    {
        (OOVPA*)&IDirectSound8_SetI3DL2Listener_1_0_4432,

        XTL::EmuIDirectSound8_SetI3DL2Listener,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetI3DL2Listener"
        #endif
    },
    // CDirectSoundVoiceSettings_SetMixBins
    {
        (OOVPA*)&CDirectSoundVoiceSettings_SetMixBins_1_0_4432, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoiceSettings::SetMixBins (XREF)"
        #endif
    },
    // CDirectSoundVoice_SetMixBins
    {
        (OOVPA*)&CDirectSoundVoice_SetMixBins_1_0_4432, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetMixBins (XREF)"
        #endif
    },
    // CDirectSoundBuffer_SetMixBins
    {
        (OOVPA*)&CDirectSoundBuffer_SetMixBins_1_0_4432, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetMixBins (XREF)"
        #endif
    },
    // IDirectSoundBuffer8_SetMixBins
    {
        (OOVPA*)&IDirectSoundBuffer8_SetMixBins_1_0_4432,

        XTL::EmuIDirectSoundBuffer8_SetMixBins,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetMixBins"
        #endif
    },
    // CDirectSound::CommitDeferredSettings
    {
        (OOVPA*)&CDirectSound_CommitDeferredSettings_1_0_4432,

        XTL::EmuCDirectSound_CommitDeferredSettings,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSound_CommitDeferredSettings"
        #endif
    },
    // CDirectSound::SetDistanceFactorA (XREF)
    {
        (OOVPA*)&CDirectSound_SetDistanceFactorA_1_0_4432, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetDistanceFactorA (XRef)"
        #endif
    },
    // IDirectSound8::SetDistanceFactor
    {
        (OOVPA*)&IDirectSound8_SetDistanceFactor_1_0_4432,

        XTL::EmuIDirectSound8_SetDistanceFactor,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetDistanceFactor"
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
};

// ******************************************************************
// * DSound_1_0_4432_SIZE
// ******************************************************************
uint32 DSound_1_0_4432_SIZE = sizeof(DSound_1_0_4432);
