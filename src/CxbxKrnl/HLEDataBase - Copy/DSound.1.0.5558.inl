// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->DSound.1.0.5558.cpp
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
// * CDirectSound::SynchPlayback
// ******************************************************************
SOOVPA<7> CDirectSound_SynchPlayback_1_0_5558 =
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0xE8 },
        { 0x3A, 0x0B },
        { 0x46, 0x8B },
    }
};

// ******************************************************************
// * CMcpxStream_Pause
// ******************************************************************
SOOVPA<11> CMcpxStream_Pause_1_0_5558 =
{
    0,  // Large == 0
    11, // Count == 11

    XREF_DSSTREAMPAUSE, // XRef Is  Saved
    0,                  // XRef Not Used

    {
        // CMcpxStream_Pause+0x17 : and ax, 0xFFDF
        { 0x17, 0x66 }, // (Offset,Value)-Pair #1
        { 0x18, 0x25 }, // (Offset,Value)-Pair #2
        { 0x19, 0xDF }, // (Offset,Value)-Pair #3

        // CMcpxStream_Pause+0x32 : jmp +0x56
        { 0x32, 0xEB }, // (Offset,Value)-Pair #4
        { 0x33, 0x56 }, // (Offset,Value)-Pair #5

        // CMcpxStream_Pause+0x39 : movzx eax, word ptr [esi+0x12]
        { 0x39, 0x0F }, // (Offset,Value)-Pair #6
        { 0x3A, 0xB7 }, // (Offset,Value)-Pair #7
        { 0x3B, 0x46 }, // (Offset,Value)-Pair #8
        { 0x3C, 0x12 }, // (Offset,Value)-Pair #9

        // CMcpxStream_Pause+0x8B : retn 0x04
        { 0x8B, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x8C, 0x04 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * CDirectSound::CommitDeferredSettings
// ******************************************************************
SOOVPA<11> CDirectSound_CommitDeferredSettings_1_0_5558 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

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

        // CDirectSound_CommitDeferredSettings+0x6D : mov [eax+0x30], edi
        { 0x6D, 0x89 }, // (Offset,Value)-Pair #8
        { 0x6E, 0x78 }, // (Offset,Value)-Pair #9
        { 0x6F, 0x30 }, // (Offset,Value)-Pair #10

        // CDirectSound_CommitDeferredSettings+0x96 : leave
        { 0x96, 0xC9 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * CDirectSound::SetDistanceFactorA
// ******************************************************************
SOOVPA<11> CDirectSound_SetDistanceFactorA_1_0_5558 =
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

        // CDirectSound_SetDistanceFactorA+0x39 : or dword ptr[eax+0x30], 0x08
        { 0x39, 0x83 }, // (Offset,Value)-Pair #6
        { 0x3A, 0x48 }, // (Offset,Value)-Pair #7
        { 0x3B, 0x30 }, // (Offset,Value)-Pair #8
        { 0x3C, 0x08 }, // (Offset,Value)-Pair #9

        // CDirectSound_SetDistanceFactorA+0x4C : jz +0x0B
        { 0x4C, 0x74 }, // (Offset,Value)-Pair #10
        { 0x4D, 0x0B }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * CDirectSound::SetRolloffFactor
// ******************************************************************
SOOVPA<11> CDirectSound_SetRolloffFactor_1_0_5558 =
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

        // CDirectSound_SetRolloffFactor+0x39 : or dword ptr[eax+0x30], 0x10
        { 0x39, 0x83 }, // (Offset,Value)-Pair #6
        { 0x3A, 0x48 }, // (Offset,Value)-Pair #7
        { 0x3B, 0x30 }, // (Offset,Value)-Pair #8
        { 0x3C, 0x10 }, // (Offset,Value)-Pair #9

        // CDirectSound_SetRolloffFactor+0x4C : jz +0x0B
        { 0x4C, 0x74 }, // (Offset,Value)-Pair #10
        { 0x4D, 0x0B }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * CDirectSound::SetDopplerFactor
// ******************************************************************
SOOVPA<14> CDirectSound_SetDopplerFactor_1_0_5558 =
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

        // CDirectSound_SetDopplerFactor+0x39 : or dword ptr[eax+0x30], 0x20
        { 0x39, 0x83 }, // (Offset,Value)-Pair #9
        { 0x3A, 0x48 }, // (Offset,Value)-Pair #10
        { 0x3B, 0x30 }, // (Offset,Value)-Pair #11
        { 0x3C, 0x20 }, // (Offset,Value)-Pair #12

        // CDirectSound_SetDopplerFactor+0x4C : jz +0x0B
        { 0x4C, 0x74 }, // (Offset,Value)-Pair #13
        { 0x4D, 0x0B }, // (Offset,Value)-Pair #14
    }
};

// ******************************************************************
// * IDirectSound_SetMixBinHeadroom
// ******************************************************************
SOOVPA<9> IDirectSound8_SetMixBinHeadroom_1_0_5558 =
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x19, XREF_DSSETMIXBINHEADROOMA },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0xFF },
        { 0x0E, 0x83 },
        { 0x12, 0xD9 },
        { 0x16, 0xC8 },
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
    }
};

// ******************************************************************
// * CDirectSound_SetI3DL2Listener
// ******************************************************************
SOOVPA<8> CDirectSound_SetI3DL2Listener_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSSETI3DL2LISTENER,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x1D, 0xFF },
        { 0x3C, 0x00 },
        { 0x5B, 0x9A },
        { 0x7A, 0x10 },
        { 0x99, 0x00 },
        { 0xB8, 0x50 },
        { 0xD7, 0x00 },
        { 0xF6, 0x68 },
    }
};

// ******************************************************************
// * IDirectSound8_SetI3DL2Listener
// ******************************************************************
SOOVPA<9> IDirectSound8_SetI3DL2Listener_1_0_5558 =
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x19, XREF_DSSETI3DL2LISTENER },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0xFF },
        { 0x0E, 0x83 },
        { 0x12, 0xD9 },
        { 0x16, 0xC8 },
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
    }
};
// ******************************************************************
// * DirectSound::CFullHRTFSource::GetCenterVolume
// ******************************************************************
SOOVPA<7> DirectSound_CFullHRTFSource_GetCenterVolume_1_0_5558 =
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DirectSound_CFullHRTFSource_GetCenterVolume,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x13, 0xD9 },
        { 0x29, 0xDF },
        { 0x3D, 0x2D },
        { 0x54, 0xC1 },
        { 0x67, 0x4D },
        { 0x7E, 0xD9 },
        { 0x91, 0x10 },
    }
};

// ******************************************************************
// * DirectSound::CHRTFSource::SetFullHRTF5Channel
// ******************************************************************
SOOVPA<9> DirectSound_CHRTFSource_SetFullHRTF5Channel_1_0_5558 =
{
    0,  // Large == 0
    9,  // Count == 9

    XREF_DirectSound_CHRTFSource_SetFullHRTF5Channel,  // Xref Is Saved
    1,  // Xref Is Used

    {
        { 0x4C, XREF_DirectSound_CFullHRTFSource_GetCenterVolume },

        { 0x14, 0xC7 },
        { 0x1E, 0xC7 },
        { 0x28, 0xC7 },
        { 0x3C, 0xC7 },
        { 0x46, 0xC7 },
        { 0x5A, 0xC7 },
        { 0x64, 0xC7 },
        { 0x6E, 0xC3 },
    }
};

// ******************************************************************
// * DirectSoundUseFullHRTF
// ******************************************************************
SOOVPA<8> DirectSoundUseFullHRTF_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x0A, XREF_DirectSound_CHRTFSource_SetFullHRTF5Channel },

        { 0x06, 0x0F },
        { 0x07, 0xB6 },
        { 0x0E, 0x85 },
        { 0x0F, 0xF6 },
        { 0x12, 0x0B },
        { 0x18, 0xFF },
        { 0x1E, 0xC3 },
    }
};

// ******************************************************************
// * DirectSoundDoWork
// ******************************************************************
SOOVPA<7> DirectSoundDoWork_1_0_5558 =
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x06, 0x0F },
        { 0x0E, 0x85 },
        { 0x10, 0x74 },
        { 0x18, 0x85 },
        { 0x1C, 0x0B },
        { 0x22, 0xFF },
        { 0x28, 0xC3 },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetBufferData
// ******************************************************************
SOOVPA<7> CDirectSoundBuffer_SetBufferData_1_0_5558 =
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DSSETBUFFERDATA,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x14, 0x0C },
        { 0x2A, 0xB6 },
        { 0x40, 0xB8 },
        { 0x56, 0x8B },
        { 0x6C, 0x8B },
        { 0x82, 0x7C },
        { 0x98, 0x5E },
    }
};

// ******************************************************************
// * IDirectSoundBuffer8_SetBufferData
// ******************************************************************
SOOVPA<9> IDirectSoundBuffer8_SetBufferData_1_0_5558 =
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x19, XREF_DSSETBUFFERDATA },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0xFF },
        { 0x0E, 0x83 },
        { 0x12, 0xD9 },
        { 0x16, 0xC8 },
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
    }
};

// ******************************************************************
// * CMcpxBuffer::SetCurrentPosition
// ******************************************************************
SOOVPA<7> CMcpxBuffer_SetCurrentPosition_1_0_5558 =
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_SETCURRENTPOSITION,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x1D, 0x00 },
        { 0x3C, 0x66 },
        { 0x5B, 0x00 },
        { 0x7C, 0x8D },
        { 0x99, 0xD1 },
        { 0xB8, 0x89 },
        { 0xDB, 0x5F },
    }
};

// ******************************************************************
// * CDirectSoundBuffer::SetCurrentPosition
// ******************************************************************
SOOVPA<8> CDirectSoundBuffer_SetCurrentPosition_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_SETCURRENTPOSITION2,  // Xref Is Saved
    1,  // Xref Is Used

    {
        { 0x35, XREF_SETCURRENTPOSITION },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x39, 0x85 },
        { 0x44, 0xFF },
        { 0x4B, 0xC7 },
    }
};

// ******************************************************************
// * IDirectSoundBuffer8_SetCurrentPosition
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer8_SetCurrentPosition_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x15, XREF_SETCURRENTPOSITION2 },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
    }
};

// ******************************************************************
// * CMcpxVoiceClient::SetLFO
// ******************************************************************
SOOVPA<8> CMcpxVoiceClient_SetLFO_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DirectSound_CMcpxVoiceClient_SetLFO,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x1E, 0x48 },
        { 0x3E, 0x31 },
        { 0x5E, 0x35 },
        { 0x7E, 0xE1 },
        { 0x9E, 0x8B },
        { 0xBE, 0x8B },
        { 0xDE, 0x05 },
        { 0xFE, 0x6C },
    }
};
// ******************************************************************
// * CDirectSoundVoice::SetLFO
// ******************************************************************
SOOVPA<7> CDirectSoundVoice_SetLFO_1_0_5558 =
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DirectSound_CDirectSoundVoice_SetLFO,  // Xref Is Saved
    1,  // Xref Is Used

    {
        { 0x0C, XREF_DirectSound_CMcpxVoiceClient_SetLFO },

        { 0x01, 0x44 },
        { 0x04, 0xFF },
        { 0x07, 0x08 },
        { 0x0A, 0x0C },
        { 0x10, 0xC2 },
        { 0x11, 0x08 },
    }
};

// ******************************************************************
// * CDirectSoundBuffer::SetLFO
// ******************************************************************
SOOVPA<8> CDirectSoundBuffer_SetLFO_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DirectSound_CDirectSoundBuffer_SetLFO,  // Xref Is Saved
    1,  // Xref Is Used

    {
        { 0x32, XREF_DirectSound_CDirectSoundVoice_SetLFO },

        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0x10 },
        { 0x3A, 0x74 },
        { 0x47, 0x8B },
    }
};

// ******************************************************************
// * IDirectSoundBuffer_SetLFO
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer8_SetLFO_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x15, XREF_DirectSound_CDirectSoundBuffer_SetLFO },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
    }
};

// ******************************************************************
// * CMcpxVoiceClient_SetVolume
// ******************************************************************
SOOVPA<13> CMcpxVoiceClient_SetVolume_1_0_5558 =
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
// * CDirectSoundVoiceSettings::SetMixBins
// ******************************************************************
SOOVPA<7> CDirectSoundVoiceSettings_SetMixBins_1_0_5558 =
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DSSETMIXBINSC,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x1B, 0xEB },
        { 0x32, 0x8B },
        { 0x4C, 0x7C },
        { 0x66, 0x00 },
        { 0x84, 0xF6 },
        { 0x9A, 0xB4 },
        { 0xB4, 0x00 },
    }
};

// ******************************************************************
// * CDirectSoundVoice::SetMixBins
// ******************************************************************
SOOVPA<8> CDirectSoundVoice_SetMixBins_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSSETMIXBINSB,  // Xref Is Saved
    1,  // Xref Is Used

    {
        { 0x0D, XREF_DSSETMIXBINSC },

        { 0x02, 0x74 },
        { 0x06, 0x74 },
        { 0x0A, 0x4E },
        { 0x11, 0x8B },
        { 0x12, 0x4E },
        { 0x19, 0x5E },
        { 0x1A, 0xC2 },
    }
};

// ******************************************************************
// * CDirectSoundBuffer::SetMixBins
// ******************************************************************
SOOVPA<8> CDirectSoundBuffer_SetMixBins_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSSETMIXBINSA,  // Xref Is Saved
    1,  // Xref Is Used

    {
        { 0x32, XREF_DSSETMIXBINSB },

        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0x10 },
        { 0x3A, 0x74 },
        { 0x47, 0x8B },
    }
};

// ******************************************************************
// * IDirectSoundBuffer8_SetMixBins
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer8_SetMixBins_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x15, XREF_DSSETMIXBINSA },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
    }
};

// ******************************************************************
// * CDirectSoundBuffer::SetPlayRegion
// ******************************************************************
SOOVPA<8> CDirectSoundBuffer_SetPlayRegion_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSBUFFERSETPLAYREGIONA,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x0F, 0x00 },
        { 0x1E, 0xFF },
        { 0x2E, 0x85 },
        { 0x3E, 0x3B },
        { 0x4E, 0x81 },
        { 0x5F, 0x8B },
        { 0x72, 0xFF },
        { 0x7E, 0x0C },
    }
};

// ******************************************************************
// * IDirectSoundBuffer8_SetPlayRegion
// ******************************************************************
SOOVPA<9> IDirectSoundBuffer8_SetPlayRegion_1_0_5558 =
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x19, XREF_DSBUFFERSETPLAYREGIONA },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0xFF },
        { 0x0E, 0x83 },
        { 0x12, 0xD9 },
        { 0x16, 0xC8 },
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
    }
};
// ******************************************************************
// * CDirectSoundBuffer::SetLoopRegion
// ******************************************************************
SOOVPA<7> CDirectSoundBuffer_SetLoopRegion_1_0_5558 =
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DirectSound_CDirectSoundBuffer_SetLoopRegion,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x0F, 0x00 },
        { 0x24, 0xB8 },
        { 0x31, 0x55 },
        { 0x42, 0xC8 },
        { 0x53, 0x00 },
        { 0x64, 0x8B },
        { 0x77, 0xFF },
    }
};
// ******************************************************************
// * IDirectSoundBuffer_SetLoopRegion
// ******************************************************************
SOOVPA<9> IDirectSoundBuffer8_SetLoopRegion_1_0_5558 =
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x19, XREF_DirectSound_CDirectSoundBuffer_SetLoopRegion },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0xFF },
        { 0x0E, 0x83 },
        { 0x12, 0xD9 },
        { 0x16, 0xC8 },
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
    }
};

// ******************************************************************
// * CMcpxBuffer::Play
// ******************************************************************
SOOVPA<8> CMcpxBuffer_Play_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSOUNDPLAY,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x06, 0x56 },
        { 0x0E, 0xF1 },
        { 0x16, 0xFF },
        { 0x1D, 0x03 },
        { 0x1E, 0xFF },
        { 0x26, 0x75 },
        { 0x2F, 0x8B },
        { 0x36, 0xC2 },
    }
};

// ******************************************************************
// * CDirectSoundBuffer::Play
// ******************************************************************
SOOVPA<9> CDirectSoundBuffer_Play_1_0_5558 =
{
    0,  // Large == 0
    9,  // Count == 10

    XREF_DSOUNDPLAY2,  // Xref Is Saved
    0,  // Xref Is Used

    {
        //{ 0x35, XREF_DSOUNDPLAY },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x2E, 0x20 },
        { 0x33, 0x18 },
        { 0x39, 0x85 },
        { 0x44, 0xFF },
        { 0x4B, 0xC7 },
    }
};

// ******************************************************************
// * IDirectSoundBuffer_Play
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer8_Play_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x1D, XREF_DSOUNDPLAY2 },

        { 0x03, 0x10 },
        { 0x08, 0xFF },
        { 0x0D, 0xC8 },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
    }
};

// ******************************************************************
// * CMcpxBuffer_GetStatus
// ******************************************************************
SOOVPA<11> CMcpxBuffer_GetStatus_1_0_5558 =
{
    0,  // Large == 0
    11, // Count == 11

    XREF_DSBUFFERGETSTATUSA,    // XRef Is  Saved
    0,                          // XRef Not Used

    {
        // CMcpxBuffer_GetStatus+0x04 : mov ecx, eax
        { 0x04, 0x8B }, // (Offset,Value)-Pair #1
        { 0x05, 0xC8 }, // (Offset,Value)-Pair #2

        // CMcpxBuffer_GetStatus+0x0C : jnz +0x1B
        { 0x0C, 0x75 }, // (Offset,Value)-Pair #3
        { 0x0D, 0x1B }, // (Offset,Value)-Pair #4

        // CMcpxBuffer_GetStatus+0x12 : test ax, 0x444
        { 0x12, 0x66 }, // (Offset,Value)-Pair #5
        { 0x13, 0xA9 }, // (Offset,Value)-Pair #6
        { 0x14, 0x44 }, // (Offset,Value)-Pair #7

        // CMcpxBuffer_GetStatus+0x29 : mov ecx, 0x8001
        { 0x29, 0xB9 }, // (Offset,Value)-Pair #8
        { 0x2A, 0x01 }, // (Offset,Value)-Pair #9
        { 0x2B, 0x80 }, // (Offset,Value)-Pair #10
        { 0x2C, 0x00 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * WaveFormat::CreateXboxAdpcmFormat
// ******************************************************************
SOOVPA<7> WaveFormat_CreateXboxAdpcmFormat_1_0_5558 =
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DirectSound_WaveFormat_CreateXboxAdpcmFormat,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x07, 0x08 },
        { 0x10, 0xE9 },
        { 0x19, 0x8D },
        { 0x22, 0x66 },
        { 0x2B, 0x04 },
        { 0x34, 0x66 },
        { 0x3D, 0x12 },
    }
};

// ******************************************************************
// * XAudioCreateAdpcmFormat
// ******************************************************************
SOOVPA<2> XAudioCreateAdpcmFormat_1_0_5558 =
{
    0,  // Large == 0
    2,  // Count == 2

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x01, XREF_DirectSound_WaveFormat_CreateXboxAdpcmFormat },

        { 0x00, 0xE9 },
    }
};

// ******************************************************************
// * CDirectSoundVoice::SetFormat
// ******************************************************************
SOOVPA<7> CDirectSoundVoice_SetFormat_1_0_5558 =
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DirectSound_CDirectSoundVoice_SetFormat,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x07, 0x10 },
        { 0x13, 0x85 },
        { 0x19, 0x10 },
        { 0x22, 0x0C },
        { 0x2B, 0x14 },
        { 0x34, 0x85 },
        { 0x40, 0x5E },
    }
};

// ******************************************************************
// * CDirectSoundBuffer::SetFormat
// ******************************************************************
SOOVPA<8> CDirectSoundBuffer_SetFormat_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DirectSound_CDirectSoundBuffer_SetFormat,  // Xref Is Saved
    1,  // Xref Is Used

    {
        { 0x32, XREF_DirectSound_CDirectSoundVoice_SetFormat },

        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0x10 },
        { 0x3A, 0x74 },
        { 0x47, 0x8B },
    }
};

// ******************************************************************
// * IDirectSoundBuffer8_SetFormat
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer8_SetFormat_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x15, XREF_DirectSound_CDirectSoundBuffer_SetFormat },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
    }
};

// ******************************************************************
// * CDirectSoundVoice::SetConeAngles
// ******************************************************************
SOOVPA<8> CDirectSoundVoice_SetConeAngles_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DirectSound_CDirectSoundVoice_SetConeAngles,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x07, 0x8B },
        { 0x10, 0x08 },
        { 0x13, 0x20 },
        { 0x19, 0xB4 },
        { 0x22, 0x51 },
        { 0x2B, 0x00 },
        { 0x34, 0x10 },
        { 0x3E, 0x33 },
    }
};

// ******************************************************************
// * CDirectSoundBuffer::SetConeAngles
// ******************************************************************
SOOVPA<8> CDirectSoundBuffer_SetConeAngles_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DirectSound_CDirectSoundBuffer_SetConeAngles,  // Xref Is Saved
    1,  // Xref Is Used

    {
        { 0x3A, XREF_DirectSound_CDirectSoundVoice_SetConeAngles },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x74 },
        { 0x35, 0xFF },
        { 0x40, 0x8B },
        { 0x4F, 0x8B },
    }
};

// ******************************************************************
// * IDirectSoundBuffer8_SetConeAngles
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer8_SetConeAngles_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x1D, XREF_DirectSound_CDirectSoundBuffer_SetConeAngles },

        { 0x03, 0x10 },
        { 0x08, 0xFF },
        { 0x0D, 0xC8 },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetMaxDistance
// ******************************************************************
SOOVPA<7> CDirectSoundVoice_SetMaxDistance_1_0_5558 =
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DSVOICESETMAXDISTANCE,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x05, 0x41 },
        { 0x0C, 0x00 },
        { 0x13, 0x3C },
        { 0x1A, 0x00 },
        { 0x21, 0xF6 },
        { 0x28, 0x51 },
        { 0x2F, 0xC0 },
    }
};
// ******************************************************************
// * CDirectSoundBuffer::SetMaxDistance
// ******************************************************************
SOOVPA<11> CDirectSoundBuffer_SetMaxDistance_1_0_5558 =
{
    0,  // Large == 0
    11, // Count == 11

    XREF_DSBUFFERSETMAXDISTANCE, // XRef Is Saved
    1,                           // XRef Is Used

    {
        // CDirectSoundBuffer_SetMaxDistance+0x39 : call [CDirectSoundVoice::SetMaxDistance]
        { 0x3A, XREF_DSVOICESETMAXDISTANCE },

        { 0x00, 0x56 },
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
SOOVPA<11> IDirectSoundBuffer_SetMaxDistance_1_0_5558 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    1,  // XRef Is Used

    {
        // IDirectSoundBuffer_SetMaxDistance+0x1C : call [CDirectSoundBuffer::SetMaxDistance]
        { 0x1D, XREF_DSBUFFERSETMAXDISTANCE },

        { 0x00, 0xFF },
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
// * DirectSound::CDirectSoundVoice::SetMinDistance
// ******************************************************************
SOOVPA<7> CDirectSoundVoice_SetMinDistance_1_0_5558 =
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DSVOICESETMINDISTANCE,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x05, 0x41 },
        { 0x0C, 0x00 },
        { 0x13, 0x38 },
        { 0x1A, 0x00 },
        { 0x21, 0xF6 },
        { 0x28, 0x51 },
        { 0x2F, 0xC0 },
    }
};
// ******************************************************************
// * CDirectSoundBuffer::SetMinDistance
// ******************************************************************
SOOVPA<11> CDirectSoundBuffer_SetMinDistance_1_0_5558 =
{
    0,  // Large == 0
    11, // Count == 11

    XREF_DSBUFFERSETMINDISTANCE, // XRef Is Saved
    1,                           // XRef Is Used

    {
        // CDirectSoundBuffer_SetMinDistance+0x39 : call [CDirectSoundVoice::SetMinDistance]
        { 0x3A, XREF_DSVOICESETMINDISTANCE },

        { 0x00, 0x56 },
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
// * DirectSound::CDirectSoundStream::SetMinDistance
// ******************************************************************
SOOVPA<8> DirectSound_CDirectSoundStream_SetMinDistance_1_0_5558 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x3E, XREF_DSVOICESETROLLOFFCURVE },

        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x24 },
        { 0x3A, 0x1C },
        { 0x46, 0x74 },
        { 0x53, 0x8B },
    }
};

// ******************************************************************
// * IDirectSoundBuffer_SetMinDistance
// ******************************************************************
SOOVPA<11> IDirectSoundBuffer_SetMinDistance_1_0_5558 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    1,  // XRef Is Used

    {
        // IDirectSoundBuffer_SetMinDistance+0x1C : call [CDirectSoundBuffer::SetMinDistance]
        { 0x1D, XREF_DSBUFFERSETMINDISTANCE },

        { 0x00, 0xFF },
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
// * CDirectSoundVoice::CommitDeferredSettings
// ******************************************************************
SOOVPA<6> CDirectSoundVoice_CommitDeferredSettings_1_0_5558 =
{
    0,  // Large == 0
    6,  // Count == 6

    XREF_DirectSound_CDirectSoundVoice_CommitDeferredSettings,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x01, 0x44 },
        { 0x04, 0x8B },
        { 0x07, 0xE8 },
        { 0x0C, 0x33 },
        { 0x0D, 0xC0 },
        { 0x10, 0x00 },
    }
};

// ******************************************************************
// * CDirectSoundVoice::SetAllParameters
// ******************************************************************
SOOVPA<14> CDirectSoundVoice_SetAllParameters_1_0_5558 =
{
    0,  // Large == 0
    14,  // Count == 14

    XREF_DirectSound_CDirectSoundVoice_SetAllParameters,  // Xref Is Saved
    1,  // Xref Is Used

    {
        { 0x2A, XREF_DirectSound_CDirectSoundVoice_CommitDeferredSettings },

        { 0x00, 0x8B },
        { 0x01, 0x4c },
        { 0x02, 0x24 },
        { 0x03, 0x04 },
        { 0x11, 0x89 },
        { 0x12, 0x50 },
        { 0x13, 0x4C },
        { 0x14, 0x8B },
        { 0x15, 0x41 },
        { 0x16, 0x10 },
        { 0x30, 0xC2 },
        { 0x31, 0x0C },
        { 0x32, 0x00 },
    }
};

// ******************************************************************
// * CDirectSoundBuffer::SetAllParameters
// ******************************************************************
SOOVPA<8> CDirectSoundBuffer_SetAllParameters_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DirectSound_CDirectSoundBuffer_SetAllParameters,  // Xref Is Saved
    1,  // Xref Is Used

    {
        { 0x36, XREF_DirectSound_CDirectSoundVoice_SetAllParameters },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x74 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
    }
};

// ******************************************************************
// * IDirectSoundBuffer8::SetAllParameters
// ******************************************************************
SOOVPA<9> IDirectSoundBuffer8_SetAllParameters_1_0_5558 =
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x19, XREF_DirectSound_CDirectSoundBuffer_SetAllParameters },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0xFF },
        { 0x0E, 0x83 },
        { 0x12, 0xD9 },
        { 0x16, 0xC8 },
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
    }
};

// ******************************************************************
// * CDirectSoundVoice::SetPosition
// ******************************************************************
SOOVPA<7> CDirectSoundVoice_SetPosition_1_0_5558 =
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DSVOICESETPOSITION,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x09, 0x8B },
        { 0x14, 0x08 },
        { 0x1F, 0x55 },
        { 0x2A, 0x00 },
        { 0x35, 0x10 },
        { 0x40, 0xF6 },
        { 0x4C, 0x33 },
    }
};

// ******************************************************************
// * CDirectSoundBuffer::SetPosition
// ******************************************************************
SOOVPA<9> CDirectSoundBuffer_SetPosition_1_0_5558 =
{
    0,  // Large == 0
    9,  // Count == 9

    XREF_DSBUFFERSETPOSITION,  // Xref Is Saved
    1,  // Xref Is Used

    {
        { 0x4A, XREF_DSVOICESETPOSITION },

        { 0x0F, 0x00 },
        { 0x18, 0x0B },
        { 0x25, 0x05 },
        { 0x32, 0x83 },
        { 0x3F, 0x04 },
        { 0x4E, 0x85 },
        { 0x59, 0xFF },
        { 0x66, 0x00 },
    }
};

// ******************************************************************
// * IDirectSoundBuffer8::SetPosition
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer8_SetPosition_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x2D, XREF_DSBUFFERSETPOSITION },

        { 0x05, 0x18 },
        { 0x0C, 0x83 },
        { 0x13, 0x8B },
        { 0x1A, 0xE4 },
        { 0x21, 0xD9 },
        { 0x28, 0xD9 },
        { 0x31, 0x5D },
    }
};
// ******************************************************************
// * CDirectSoundVoice::SetVelocity
// ******************************************************************
SOOVPA<7> CDirectSoundVoice_SetVelocity_1_0_5558 =
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DSVOICESETVELOCITY,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x09, 0x8B },
        { 0x14, 0x14 },
        { 0x1F, 0x55 },
        { 0x2A, 0x00 },
        { 0x35, 0x10 },
        { 0x40, 0xF6 },
        { 0x4C, 0x33 },
    }
};

// ******************************************************************
// * CDirectSoundBuffer::SetVelocity
// ******************************************************************
SOOVPA<9> CDirectSoundBuffer_SetVelocity_1_0_5558 =
{
    0,  // Large == 0
    9,  // Count == 9

    XREF_DSBUFFERSETVELOCITY,  // Xref Is Saved
    1,  // Xref Is Used

    {
        { 0x4A, XREF_DSVOICESETVELOCITY },

        { 0x0F, 0x00 },
        { 0x18, 0x0B },
        { 0x25, 0x05 },
        { 0x32, 0x83 },
        { 0x3F, 0x04 },
        { 0x4E, 0x85 },
        { 0x59, 0xFF },
        { 0x66, 0x00 },
    }
};
// ******************************************************************
// * IDirectSoundBuffer8_SetVelocity
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer8_SetVelocity_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x2D, XREF_DSBUFFERSETVELOCITY },

        { 0x05, 0x18 },
        { 0x0C, 0x83 },
        { 0x13, 0x8B },
        { 0x1A, 0xE4 },
        { 0x21, 0xD9 },
        { 0x28, 0xD9 },
        { 0x31, 0x5D },
    }
};

/*// ******************************************************************
// * CMcpxStream::Pause
// ******************************************************************
SOOVPA<7> DirectSound_CMcpxStream_Pause_1_0_5558 =
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DirectSound_CMcpxStream_Pause,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x10, 0x07 },
        { 0x23, 0x0F },
        { 0x34, 0x83 },
        { 0x46, 0xF8 },
        { 0x58, 0x03 },
        { 0x6A, 0xBB },
        { 0x7C, 0x00 },
    }
};*/

// ******************************************************************
// * CDirectSoundStream::Pause
// ******************************************************************
SOOVPA<8> CDirectSoundStream_Pause_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x35, XREF_DSSTREAMPAUSE },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x39, 0x85 },
        { 0x44, 0xFF },
        { 0x4B, 0xC7 },
    }
};

// ******************************************************************
// * CDirectSoundVoice::SetDistanceFactor
// ******************************************************************
SOOVPA<7> CDirectSoundVoice_SetDistanceFactor_1_0_5558 =
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DSVOICESETDISTANCEFACTOR,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x05, 0x41 },
        { 0x0C, 0x00 },
        { 0x13, 0x44 },
        { 0x1A, 0x00 },
        { 0x21, 0xF6 },
        { 0x28, 0x51 },
        { 0x2F, 0xC0 },
    }
};

// ******************************************************************
// * CDirectSoundBuffer::SetDistanceFactor
// ******************************************************************
SOOVPA<8> CDirectSoundBuffer_SetDistanceFactor_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSBUFFERSETDISTANCEFACTOR,  // Xref Is Saved
    1,  // Xref Is Used

    {
        { 0x3A, XREF_DSVOICESETDISTANCEFACTOR },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xFF },
        { 0x40, 0x8B },
        { 0x4F, 0x8B },
    }
};

// ******************************************************************
// * IDirectSoundBuffer8_SetDistanceFactor
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer8_SetDistanceFactor_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x1D, XREF_DSBUFFERSETDISTANCEFACTOR },

        { 0x03, 0x0C },
        { 0x08, 0x8B },
        { 0x0D, 0x8B },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
    }
};

// ******************************************************************
// * CDirectSoundVoice::SetDopplerFactor
// ******************************************************************
SOOVPA<7> CDirectSoundVoice_SetDopplerFactor_1_0_5558 =
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DSVOICESETDOPPLERFACTOR,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x05, 0x41 },
        { 0x0C, 0x00 },
        { 0x13, 0x4C },
        { 0x1A, 0x00 },
        { 0x21, 0xF6 },
        { 0x28, 0x51 },
        { 0x2F, 0xC0 },
    }
};

// ******************************************************************
// * CDirectSoundBuffer::SetDopplerFactor
// ******************************************************************
SOOVPA<8> CDirectSoundBuffer_SetDopplerFactor_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSBUFFERSETDOPPLERFACTOR,  // Xref Is Saved
    1,  // Xref Is Used

    {
        { 0x3A, XREF_DSVOICESETDOPPLERFACTOR },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xFF },
        { 0x40, 0x8B },
        { 0x4F, 0x8B },
    }
};

// ******************************************************************
// * IDirectSoundBuffer8_SetDopplerFactor
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer8_SetDopplerFactor_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x1D, XREF_DSBUFFERSETDOPPLERFACTOR },

        { 0x03, 0x0C },
        { 0x08, 0x8B },
        { 0x0D, 0x8B },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
    }
};
// ******************************************************************
// * IDirectSoundBuffer_Lock
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer_Lock_1_0_5558 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x04, 0x75 },
        { 0x0A, 0x75 },
        { 0x10, 0x1C },
        { 0x16, 0x18 },
        { 0x1C, 0x1B },
        { 0x22, 0xC8 },
        { 0x2C, 0x5D },
        { 0x2E, 0x20 },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetI3DL2Source
// ******************************************************************
SOOVPA<7> DirectSound_CDirectSoundVoice_SetI3DL2Source_1_0_5558 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DSVOICESETI3DL2SOURCE,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x18, 0x00 },
        { 0x32, 0x8B },
        { 0x4C, 0x8C },
        { 0x66, 0x50 },
        { 0x80, 0x89 },
        { 0x9A, 0x10 },
        { 0xB4, 0x49 },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetI3DL2Source
// ******************************************************************
SOOVPA<8> DirectSound_CDirectSoundBuffer_SetI3DL2Source_1_0_5558 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSBUFFERSETI3DL2SOURCE,  // Xref Is Saved
    1,  // Xref Is Used

    {
        { 0x36, XREF_DSVOICESETI3DL2SOURCE },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x74 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
    }
};

// ******************************************************************
// * IDirectSoundBuffer_SetI3DL2Source
// ******************************************************************
SOOVPA<9> IDirectSoundBuffer_SetI3DL2Source_1_0_5558 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x19, XREF_DSBUFFERSETI3DL2SOURCE },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0xFF },
        { 0x0E, 0x83 },
        { 0x12, 0xD9 },
        { 0x16, 0xC8 },
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetAllParameters
// ******************************************************************
SOOVPA<10> DirectSound_CDirectSoundVoice_SetAllParameters_1_0_5558 = 
{
    0,  // Large == 0
    10,  // Count == 10

    XREF_DirectSound_CDirectSoundVoice_SetAllParameters,  // Xref Is Saved
    0,  // Xref Is Used

    {
//        { 0x12F, XREF_DirectSound_CDirectSoundVoice_CommitDeferredSettings },

        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x1D, 0x10 },
        { 0x3C, 0x8B },
        { 0x5B, 0x92 },
        { 0x7A, 0xB4 },
        { 0x99, 0x00 },
        { 0xB8, 0x00 },
        { 0xD7, 0x00 },
        { 0xF6, 0x89 },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetAllParameters
// ******************************************************************
SOOVPA<8> DirectSound_CDirectSoundBuffer_SetAllParameters_1_0_5558 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DirectSound_CDirectSoundBuffer_SetAllParameters,  // Xref Is Saved
    1,  // Xref Is Used

    {
        { 0x36, XREF_DirectSound_CDirectSoundVoice_SetAllParameters },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x74 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
    }
};

// ******************************************************************
// * IDirectSoundBuffer_SetAllParameters
// ******************************************************************
SOOVPA<9> IDirectSoundBuffer_SetAllParameters_1_0_5558 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x19, XREF_DirectSound_CDirectSoundBuffer_SetAllParameters },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0xFF },
        { 0x0E, 0x83 },
        { 0x12, 0xD9 },
        { 0x16, 0xC8 },
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetHeadroom
// ******************************************************************
SOOVPA<7> DirectSound_CDirectSoundVoice_SetHeadroom_1_0_5558 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DirectSound_CDirectSoundVoice_SetHeadroom,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x03, 0x04 },
        { 0x08, 0x48 },
        { 0x0D, 0x24 },
        { 0x12, 0x48 },
        { 0x17, 0x8B },
        { 0x1F, 0x5E },
        { 0x21, 0x08 },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetHeadroom
// ******************************************************************
SOOVPA<8> DirectSound_CDirectSoundBuffer_SetHeadroom_1_0_5558 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DirectSound_CDirectSoundBuffer_SetHeadroom,  // Xref Is Saved
    1,  // Xref Is Used

    {
        { 0x32, XREF_DirectSound_CDirectSoundVoice_SetHeadroom },

        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0x10 },
        { 0x3A, 0x74 },
        { 0x47, 0x8B },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetHeadroom
// ******************************************************************
SOOVPA<9> DirectSound_CDirectSoundStream_SetHeadroom_1_0_5558 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x36, XREF_DirectSound_CDirectSoundVoice_SetHeadroom },

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
    }
};

// ******************************************************************
// * IDirectSoundBuffer_SetHeadroom
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer_SetHeadroom_1_0_5558 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x15, XREF_DirectSound_CDirectSoundBuffer_SetHeadroom },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::GetCurrentPosition
// ******************************************************************
SOOVPA<7> DirectSound_CDirectSoundBuffer_GetCurrentPosition_1_0_5558 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_GETCURRENTPOSITION,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0x74 },
        { 0x40, 0xF8 },
        { 0x4E, 0x8B },
    }
};

// ******************************************************************
// * IDirectSoundBuffer_GetCurrentPosition@12
// ******************************************************************
SOOVPA<9> IDirectSoundBuffer_GetCurrentPosition_1_0_5558 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x19, XREF_GETCURRENTPOSITION },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0xFF },
        { 0x0E, 0x83 },
        { 0x12, 0xD9 },
        { 0x16, 0xC8 },
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetFormat
// ******************************************************************
SOOVPA<9> DirectSound_CDirectSoundStream_SetFormat_1_0_5558 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x36, XREF_DirectSound_CDirectSoundVoice_SetFormat },

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetMixBins
// ******************************************************************
SOOVPA<9> DirectSound_CDirectSoundStream_SetMixBins_1_0_5558 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x36, XREF_DSSETMIXBINSB },

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
    }
};

// ******************************************************************
// * DirectSound::CDirectSound::SetMixBinHeadroom
// ******************************************************************
SOOVPA<8> DirectSound_CDirectSound_SetMixBinHeadroom_1_0_5558 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSSETMIXBINHEADROOMA,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x24 },
        { 0x3A, 0x5C },
        { 0x46, 0x85 },
        { 0x52, 0x15 },
        { 0x5E, 0x00 },
    }
};

// ******************************************************************
// * IDirectSound_SetMixBinHeadroom
// ******************************************************************
SOOVPA<9> IDirectSound_SetMixBinHeadroom_1_0_5558 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x19, XREF_DSSETMIXBINHEADROOMA },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0xFF },
        { 0x0E, 0x83 },
        { 0x12, 0xD9 },
        { 0x16, 0xC8 },
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
    }
};

// ******************************************************************
// * DirectSound::CDirectSound::EnableHeadphones
// ******************************************************************
SOOVPA<8> DirectSound_CDirectSound_EnableHeadphones_1_0_5558 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSENABLEHEADPHONES,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x1E, 0x3B },
        { 0x3E, 0xC1 },
        { 0x60, 0xEB },
        { 0x7E, 0x00 },
        { 0x9E, 0x75 },
        { 0xBE, 0x00 },
        { 0xDE, 0x59 },
        { 0xFE, 0x75 },
    }
};

// ******************************************************************
// * IDirectSound_EnableHeadphones
// ******************************************************************
SOOVPA<8> IDirectSound_EnableHeadphones_1_0_5558 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x15, XREF_DSENABLEHEADPHONES },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
    }
};

// ******************************************************************
// * DirectSound::CDirectSound::GetOutputLevels
// ******************************************************************
SOOVPA<7> DirectSound_CDirectSound_GetOutputLevels_1_0_5558 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DirectSound_CDirectSound_GetOutputLevels,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x07, 0x10 },
        { 0x10, 0xF3 },
        { 0x19, 0x5E },
        { 0x22, 0xFE },
        { 0x2B, 0x85 },
        { 0x34, 0xA6 },
        { 0x3D, 0xD0 },
    }
};

// ******************************************************************
// * IDirectSound_GetOutputLevels
// ******************************************************************
SOOVPA<9> IDirectSound_GetOutputLevels_1_0_5558 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x19, XREF_DirectSound_CDirectSound_GetOutputLevels },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0xFF },
        { 0x0E, 0x83 },
        { 0x12, 0xD9 },
        { 0x16, 0xC8 },
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
    }
};

// ******************************************************************
// * XAudioDownloadEffectsImage
// ******************************************************************
SOOVPA<8> XAudioDownloadEffectsImage_1_0_5558 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x1E, 0xB6 },
        { 0x41, 0x83 },
        { 0x61, 0x8B },
        { 0x80, 0x85 },
        { 0x9E, 0x8B },
        { 0xBE, 0x6A },
        { 0xDE, 0x07 },
        { 0xFE, 0xF7 },
    }
};

// ******************************************************************
// * DirectSound::CDirectSound::SetPosition
// ******************************************************************
SOOVPA<7> DirectSound_CDirectSound_SetPosition_1_0_5558 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DSSETPOSITIONA,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x0F, 0x00 },
        { 0x1E, 0xFF },
        { 0x2B, 0x8B },
        { 0x3A, 0x8B },
        { 0x49, 0x40 },
        { 0x58, 0xE8 },
        { 0x67, 0x15 },
    }
};

// ******************************************************************
// * IDirectSound_SetPosition
// ******************************************************************
SOOVPA<8> IDirectSound_SetPosition_1_0_5558 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x2D, XREF_DSSETPOSITIONA },

        { 0x05, 0x18 },
        { 0x0C, 0x83 },
        { 0x13, 0x8B },
        { 0x1A, 0xF8 },
        { 0x21, 0xD9 },
        { 0x28, 0xD9 },
        { 0x31, 0x5D },
    }
};

// ******************************************************************
// * DirectSound::CDirectSound::SetVelocity
// ******************************************************************
SOOVPA<7> DirectSound_CDirectSound_SetVelocity_1_0_5558 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DSSETVELOCITYA,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x0F, 0x00 },
        { 0x1E, 0xFF },
        { 0x2B, 0x8B },
        { 0x3A, 0x8B },
        { 0x49, 0x4C },
        { 0x58, 0xE8 },
        { 0x67, 0x15 },
    }
};

// ******************************************************************
// * IDirectSound_SetVelocity
// ******************************************************************
SOOVPA<8> IDirectSound_SetVelocity_1_0_5558 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x2D, XREF_DSSETVELOCITYA },

        { 0x05, 0x18 },
        { 0x0C, 0x83 },
        { 0x13, 0x8B },
        { 0x1A, 0xF8 },
        { 0x21, 0xD9 },
        { 0x28, 0xD9 },
        { 0x31, 0x5D },
    }
};

// ******************************************************************
// * DirectSound::CDirectSound::CreateSoundStream
// ******************************************************************
SOOVPA<7> DirectSound_CDirectSound_CreateSoundStream_1_0_5558 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DSCREATESOUNDSTREAM,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x11, 0xD8 },
        { 0x24, 0x05 },
        { 0x37, 0x74 },
        { 0x4A, 0x81 },
        { 0x5D, 0xE8 },
        { 0x70, 0x85 },
        { 0x83, 0xFF },
    }
};

// ******************************************************************
// * IDirectSound_CreateSoundStream
// ******************************************************************
SOOVPA<8> IDirectSound_CreateSoundStream_1_0_5558 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x1D, XREF_DSCREATESOUNDSTREAM },

        { 0x03, 0x10 },
        { 0x08, 0xFF },
        { 0x0D, 0xC8 },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetRolloffCurve
// ******************************************************************
SOOVPA<8> DirectSound_CDirectSoundStream_SetRolloffCurve_1_0_5558 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x3E, XREF_DSVOICESETROLLOFFCURVE },

        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x74 },
        { 0x3A, 0x24 },
        { 0x46, 0x74 },
        { 0x53, 0x8B },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetFrequency
// ******************************************************************
SOOVPA<9> DirectSound_CDirectSoundStream_SetFrequency_1_0_5558 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x36, XREF_DSBUFFERSETFREQUENCYB },

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
    }
};

// ******************************************************************
// * Direct_1_0_5558
// ******************************************************************
OOVPATable DSound_1_0_5558[] =
{
    // DirectSoundCreate (* unchanged since 4361 *)
    {
        (OOVPA*)&DirectSoundCreate_1_0_4361,

        XTL::EmuDirectSoundCreate,

        #ifdef _DEBUG_TRACE
        "EmuDirectSoundCreate"
        #endif
    },
    // CDirectSound_CreateSoundBuffer (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSound_CreateSoundBuffer_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound::CreateSoundBuffer (XREF)"
        #endif
    },
    // CDirectSound::SynchPlayback
    {
        (OOVPA*)&CDirectSound_SynchPlayback_1_0_5558,

        XTL::EmuCDirectSound_SynchPlayback,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SynchPlayback_1_0_5558"
        #endif
    },
    // CMcpxStream::Pause
    {
        (OOVPA*)&CMcpxStream_Pause_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxStream_Pause (XREF)"
        #endif
    },
    // CDirectSoundStream::Pause
    {
        (OOVPA*)&CDirectSoundStream_Pause_1_0_5558,

        XTL::EmuCDirectSoundStream_Pause,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_Pause"
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
    // CDirectSoundVoiceSettings_SetMixBinVolumes (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSoundVoiceSettings_SetMixBinVolumes_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoiceSettings::SetMixBinVolumes (XREF)"
        #endif
    },
    // CDirectSoundVoice_SetMixBinVolumes (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSoundVoice_SetMixBinVolumes_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetMixBinVolumes (XREF)"
        #endif
    },
    // CDirectSoundBuffer_SetMixBinVolumes (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSoundBuffer_SetMixBinVolumes_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetMixBinVolumes (XREF)"
        #endif
    },
    // IDirectSoundBuffer8_SetMixBinVolumes (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirectSoundBuffer8_SetMixBinVolumes_1_0_4627,

        XTL::EmuIDirectSoundBuffer8_SetMixBinVolumes,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetMixBinVolumes"
        #endif
    },
    // IDirectSound8::CreateSoundBuffer (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirectSound8_CreateSoundBuffer_1_0_4627,

        XTL::EmuIDirectSound8_CreateSoundBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_CreateSoundBuffer"
        #endif
    },
    // CDirectSound::CommitDeferredSettings
    {
        (OOVPA*)&CDirectSound_CommitDeferredSettings_1_0_5558,

        XTL::EmuCDirectSound_CommitDeferredSettings,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSound_CommitDeferredSettings"
        #endif
    },
    // CDirectSound::SetDistanceFactorA (XREF)
    {
        (OOVPA*)&CDirectSound_SetDistanceFactorA_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetDistanceFactorA (XRef)"
        #endif
    },
    // IDirectSound8::SetDistanceFactor (* unchanged since 4432 *)
    {
        (OOVPA*)&IDirectSound8_SetDistanceFactor_1_0_4432,

        XTL::EmuIDirectSound8_SetDistanceFactor,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetDistanceFactor"
        #endif
    },
    // CDirectSound::SetRolloffFactor (XREF)
    {
        (OOVPA*)&CDirectSound_SetRolloffFactor_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetRolloffFactor (XRef)"
        #endif
    },
    // IDirectSound8::SetRolloffFactor (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirectSound8_SetRolloffFactor_1_0_4627,

        XTL::EmuIDirectSound8_SetRolloffFactor,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetRolloffFactor"
        #endif
    },
    // CDirectSound::SetDopplerFactor (XREF)
    {
        (OOVPA*)&CDirectSound_SetDopplerFactor_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetDopplerFactor (XRef)"
        #endif
    },
    // CDirectSoundStream::FlushEx (XREF) (* unchanged since 5233 *)
    {
        (OOVPA*)&CDirectSoundStream_FlushEx_1_0_5233, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream::FlushEx (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::StopEx (* unchanged since 5233 *)
    {
        (OOVPA*)&IDirectSoundBuffer8_StopEx_1_0_5233,

        XTL::EmuIDirectSoundBuffer8_StopEx,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_StopEx"
        #endif
    },
    // IDirectSound8::SetDopplerFactor (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirectSound8_SetDopplerFactor_1_0_4627,

        XTL::EmuIDirectSound8_SetDopplerFactor,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetDopplerFactor"
        #endif
    },
    // CDirectSoundVoice::SetFrequency (XREF) (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSoundVoice_SetFrequency_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice_SetFrequency (XRef)"
        #endif
    },
    // CDirectSoundBuffer::SetFrequency (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSoundBuffer_SetFrequency_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSoundBuffer_SetFrequency (XREF)"
        #endif
    },
	// CDirectSoundStream::SetFrequency
    {
        (OOVPA*)&DirectSound_CDirectSoundStream_SetFrequency_1_0_5558,

        XTL::EmuCDirectSoundStream_SetFrequency,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSoundStream_SetFrequency"
        #endif
    },
    // IDirectSoundBuffer8::SetFrequency (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirectSoundBuffer8_SetFrequency_1_0_4627,

        XTL::EmuIDirectSoundBuffer8_SetFrequency,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetFrequency"
        #endif
    },
    // CMcpxVoiceClient_SetVolume
    {
        (OOVPA*)&CMcpxVoiceClient_SetVolume_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxVoiceClient::SetVolume (XREF)"
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
    // IDirectSound8::Release (* unchanged since 3936 *)
    {
        (OOVPA*)&IDirectSound8_Release_1_0_3936,

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
    // CDirectSoundVoice::SetMaxDistance (XREF)
    {
        (OOVPA*)&CDirectSoundVoice_SetMaxDistance_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetMaxDistance (XRef)"
        #endif
    },
    // CDirectSoundBuffer::SetMaxDistance (XREF)
    {
        (OOVPA*)&CDirectSoundBuffer_SetMaxDistance_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetMaxDistance (XRef)"
        #endif
    },
    // IDirectSoundBuffer_SetMaxDistance
    {
        (OOVPA*)&IDirectSoundBuffer_SetMaxDistance_1_0_5558,
        XTL::EmuIDirectSoundBuffer8_SetMaxDistance,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetMaxDistance"
        #endif
    },
    // CDirectSoundVoice::SetMinDistance (XREF)
    {
        (OOVPA*)&CDirectSoundVoice_SetMinDistance_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetMinDistance (XRef)"
        #endif
    },
    // CDirectSoundBuffer::SetMinDistance (XREF)
    {
        (OOVPA*)&CDirectSoundBuffer_SetMinDistance_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetMinDistance (XRef)"
        #endif
    },
	// CDirectSoundStream::SetRolloffCurve (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundStream_SetRolloffCurve_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream::SetRolloffCurve"
        #endif
    },
    // IDirectSoundBuffer_SetMinDistance
    {
        (OOVPA*)&IDirectSoundBuffer_SetMinDistance_1_0_5558,
        XTL::EmuIDirectSoundBuffer8_SetMinDistance,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetMinDistance"
        #endif
    },
    // CDirectSoundVoice::SetRolloffFactor (XREF) (* unchanged since 4627 *)
    /*{ THESE ARE NOT CORRECT
        (OOVPA*)&CDirectSoundVoice_SetRolloffFactor_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetRolloffFactor (XRef)"
        #endif
    },
    // CDirectSoundBuffer::SetRolloffFactor (XREF) (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSoundBuffer_SetRolloffFactor_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetRolloffFactor (XRef)"
        #endif
    },
    // IDirectSoundBuffer_SetRolloffFactor (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirectSoundBuffer_SetRolloffFactor_1_0_4627,
        XTL::EmuIDirectSoundBuffer8_SetRolloffFactor,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetRolloffFactor"
        #endif
    },*/
    // CDirectSoundVoice::SetDopplerFactor (XREF)
    {
        (OOVPA*)&CDirectSoundVoice_SetDopplerFactor_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetDopplerFactor (XRef)"
        #endif
    },
    // CDirectSoundBuffer::SetDopplerFactor (XREF)
    {
        (OOVPA*)&CDirectSoundBuffer_SetDopplerFactor_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetDopplerFactor (XRef)"
        #endif
    },
    // IDirectSoundBuffer8_SetDopplerFactor
    {
        (OOVPA*)&IDirectSoundBuffer8_SetDopplerFactor_1_0_5558,
        XTL::EmuIDirectSoundBuffer8_SetDopplerFactor,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetDopplerFactor"
        #endif
    },
    // IDirectSoundBuffer_Stop (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirectSoundBuffer_Stop_1_0_4627,
        XTL::EmuIDirectSoundBuffer8_Stop,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_Stop"
        #endif
    },
    //// CDirectSound_SetMixBinHeadroom
    //{
    //    (OOVPA*)&CDirectSound_SetMixBinHeadroom_1_0_4627, 0,

    //    #ifdef _DEBUG_TRACE
    //    "CDirectSound::SetMixBinHeadroom (XREF)"
    //    #endif
    //},
    //// IDirectSound8_SetMixBinHeadroom
    //{
    //    (OOVPA*)&IDirectSound8_SetMixBinHeadroom_1_0_5558,

    //    XTL::EmuIDirectSound8_SetMixBinHeadroom,

    //    #ifdef _DEBUG_TRACE
    //    "EmuIDirectSound8_SetMixBinHeadroom"
    //    #endif
    //},
    // CDirectSound_SetI3DL2Listener
    {
        (OOVPA*)&CDirectSound_SetI3DL2Listener_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound::SetI3DL2Listener (XREF)"
        #endif
    },
    // IDirectSound8_SetI3DL2Listener
    {
        (OOVPA*)&IDirectSound8_SetI3DL2Listener_1_0_5558,

        XTL::EmuIDirectSound8_SetI3DL2Listener,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetI3DL2Listener"
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
    // DirectSound::CFullHRTFSource::GetCenterVolume (XREF)
    {
        (OOVPA*)&DirectSound_CFullHRTFSource_GetCenterVolume_1_0_5558,

        0,

        #ifdef _DEBUG_TRACE
        "DirectSound::CFullHRTFSource::GetCenterVolume (XREF)"
        #endif
    },
    // DirectSound::CHRTFSource::SetFullHRTF5Channel (XREF)
    {
        (OOVPA*)&DirectSound_CHRTFSource_SetFullHRTF5Channel_1_0_5558,

        0,

        #ifdef _DEBUG_TRACE
        "DirectSound::CHRTFSource::SetFullHRTF5Channel (XREF)"
        #endif
    },
    // DirectSoundUseFullHRTF
    {
        (OOVPA*)&DirectSoundUseFullHRTF_1_0_5558,

        XTL::EmuDirectSoundUseFullHRTF,

        #ifdef _DEBUG_TRACE
        "DirectSoundUseFullHRTF"
        #endif
    },
    // DirectSoundDoWork
    {
        (OOVPA*)&DirectSoundDoWork_1_0_5558,

        XTL::EmuDirectSoundDoWork,

        #ifdef _DEBUG_TRACE
        "EmuDirectSoundDoWork"
        #endif
    },
    // CDirectSoundBuffer::SetBufferData
    {
        (OOVPA*)&CDirectSoundBuffer_SetBufferData_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer_SetBufferData (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::SetBufferData
    {
        (OOVPA*)&IDirectSoundBuffer8_SetBufferData_1_0_5558,

        XTL::EmuIDirectSoundBuffer8_SetBufferData,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetBufferData"
        #endif
    },
    // CMcpxBuffer::SetCurrentPosition
    {
        (OOVPA*)&CMcpxBuffer_SetCurrentPosition_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxBuffer_SetCurrentPosition (XREF)"
        #endif
    },
    // CDirectSoundBuffer::SetCurrentPosition
    {
        (OOVPA*)&CDirectSoundBuffer_SetCurrentPosition_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer_SetCurrentPosition (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::SetCurrentPosition
    {
        (OOVPA*)&IDirectSoundBuffer8_SetCurrentPosition_1_0_5558,

        XTL::EmuIDirectSoundBuffer8_SetCurrentPosition,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetCurrentPosition"
        #endif
    },
    // DirectSound::CMcpxVoiceClient::SetLFO (XREF)
    {
        (OOVPA*)&CMcpxVoiceClient_SetLFO_1_0_5558,0,

        #ifdef _DEBUG_TRACE
        "CMcpxVoiceClient::SetLFO (XREF)"
        #endif
    },
    // CDirectSoundVoice::SetLFO (XREF)
    {
        (OOVPA*)&CDirectSoundVoice_SetLFO_1_0_5558,0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetLFO (XREF)"
        #endif
    },
    // CDirectSoundBuffer::SetLFO (XREF)
    {
        (OOVPA*)&CDirectSoundBuffer_SetLFO_1_0_5558,0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetLFO (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::SetLFO
    {
        (OOVPA*)&IDirectSoundBuffer8_SetLFO_1_0_5558,

        XTL::EmuIDirectSoundBuffer8_SetLFO,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetLFO"
        #endif
    },
    // CDirectSoundVoice::SetVolume (XREF) (* unchanged since 5233 *)
    {
        (OOVPA*)&CDirectSoundVoice_SetVolume_1_0_5233,0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetVolume (XREF)"
        #endif
    },
    // CDirectSoundBuffer::SetVolume (XREF) (* unchanged since 5233 *)
    {
        (OOVPA*)&CDirectSoundBuffer_SetVolume_1_0_5233,0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetVolume (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::SetVolume (* unchanged since 5233 *)
    {
        (OOVPA*)&IDirectSoundBuffer8_SetVolume_1_0_5233,

        XTL::EmuIDirectSoundBuffer8_SetVolume,

        #ifdef _DEBUG_TRACE
        "EmuIIDirectSoundBuffer8_SetVolume"
        #endif
    },
    // CDirectSoundStream_SetVolume (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSoundStream_SetVolume_1_0_4627,

        XTL::EmuCDirectSoundStream_SetVolume,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSoundStream_SetVolume"
        #endif
    },
    // CDirectSoundVoiceSettings_SetMixBins
    {
        (OOVPA*)&CDirectSoundVoiceSettings_SetMixBins_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoiceSettings::SetMixBins (XREF)"
        #endif
    },
    // CDirectSoundVoice_SetMixBins
    {
        (OOVPA*)&CDirectSoundVoice_SetMixBins_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetMixBins (XREF)"
        #endif
    },
    // CDirectSoundBuffer_SetMixBins
    {
        (OOVPA*)&CDirectSoundBuffer_SetMixBins_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetMixBins (XREF)"
        #endif
    },
    // IDirectSoundBuffer8_SetMixBins
    {
        (OOVPA*)&IDirectSoundBuffer8_SetMixBins_1_0_5558,

        XTL::EmuIDirectSoundBuffer8_SetMixBins,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetMixBins"
        #endif
    },
    // CDirectSoundBuffer::SetPlayRegion
    {
        (OOVPA*)&CDirectSoundBuffer_SetPlayRegion_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetPlayRegion (XRef)"
        #endif
    },
    // IDirectSoundBuffer8::SetPlayRegion
    {
        (OOVPA*)&IDirectSoundBuffer8_SetPlayRegion_1_0_5558,

        XTL::EmuIDirectSoundBuffer8_SetPlayRegion,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetPlayRegion"
        #endif
    },
    // CDirectSoundBuffer::SetLoopRegion (XREF)
    {
        (OOVPA*)&CDirectSoundBuffer_SetLoopRegion_1_0_5558,0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetLoopRegion (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::SetLoopRegion
    {
        (OOVPA*)&IDirectSoundBuffer8_SetLoopRegion_1_0_5558,

        XTL::EmuIDirectSoundBuffer8_SetLoopRegion,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetLoopRegion"
        #endif
    },
    // CMcpxBuffer::Play
    {
        (OOVPA*)&CMcpxBuffer_Play_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxBuffer_Play (XREF)"
        #endif
    },
    // CDirectSoundBuffer::Play (XREF)
    {
        (OOVPA*)&CDirectSoundBuffer_Play_1_0_5558,0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::Play (XREF)"
        #endif
    },
    // IDirectSoundBuffer_Play
    {
        (OOVPA*)&IDirectSoundBuffer8_Play_1_0_5558,

        XTL::EmuIDirectSoundBuffer8_Play,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_Play"
        #endif
    },
    // CMcpxBuffer::GetStatus
    {
        (OOVPA*)&CMcpxBuffer_GetStatus_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxBuffer_GetStatus (XREF)"
        #endif
    },
    // CDirectSoundBuffer::GetStatus
    {
        (OOVPA*)&CDirectSoundBuffer_GetStatus_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer_GetStatus (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::GetStatus
    {
        (OOVPA*)&IDirectSoundBuffer8_GetStatus_1_0_4627,

        XTL::EmuIDirectSoundBuffer8_GetStatus,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_GetStatus"
        #endif
    },
    // WaveFormat::CreateXboxAdpcmFormat (XREF)
    {
        (OOVPA*)&WaveFormat_CreateXboxAdpcmFormat_1_0_5558,0,

        #ifdef _DEBUG_TRACE
        "WaveFormat::CreateXboxAdpcmFormat (XREF)"
        #endif
    },
    // XAudioCreateAdpcmFormat
    {
        (OOVPA*)&XAudioCreateAdpcmFormat_1_0_5558,

        XTL::EmuXAudioCreateAdpcmFormat,

        #ifdef _DEBUG_TRACE
        "XAudioCreateAdpcmFormat"
        #endif
    },
    // CDirectSoundVoice::SetFormat (XREF)
    {
        (OOVPA*)&CDirectSoundVoice_SetFormat_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetFormat (XREF)"
        #endif

    },
    // CDirectSoundBuffer::SetFormat (XREF)
    {
        (OOVPA*)&CDirectSoundBuffer_SetFormat_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetFormat (XREF)"
        #endif
    },
    // IDirectSoundBuffer8_SetFormat
    {
        (OOVPA*)&IDirectSoundBuffer8_SetFormat_1_0_5558,

        XTL::EmuIDirectSoundBuffer8_SetFormat,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer_SetFormat"
        #endif
    },
    // CDirectSoundVoice::SetConeAngles (XREF)
    {
        (OOVPA*)&CDirectSoundVoice_SetConeAngles_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetConeAngles (XRef)"
        #endif
    },
    // CDirectSoundBuffer::SetConeAngles (XREF)
    {
        (OOVPA*)&CDirectSoundBuffer_SetConeAngles_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetConeAngles (XRef)"
        #endif
    },
    // IDirectSoundBuffer_SetConeAngles
    {
        (OOVPA*)&IDirectSoundBuffer8_SetConeAngles_1_0_5558,
        XTL::EmuIDirectSoundBuffer8_SetConeAngles,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetConeAngles"
        #endif
    },
    // CDirectSoundVoice::SetRolloffCurve (XREF) (* unchanged since 5233 *)
    {
        (OOVPA*)&CDirectSoundVoice_SetRolloffCurve_1_0_5233, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetRolloffCurve (XREF)"
        #endif

    },
    // CDirectSoundBuffer::SetRolloffCurve (XREF) (* unchanged since 5233 *)
    {
        (OOVPA*)&CDirectSoundBuffer_SetRolloffCurve_1_0_5233, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetRolloffCurve (XREF)"
        #endif
    },
	// CDirectSoundStream::SetRolloffCurve
    {
        (OOVPA*)&DirectSound_CDirectSoundStream_SetRolloffCurve_1_0_5558, 
		XTL::EmuCDirectSoundStream_SetRolloffCurve,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetRolloffCurve"
        #endif
    },
    // IDirectSoundBuffer8::SetRolloffCurve (* unchanged since 5233 *)
    {
        (OOVPA*)&IDirectSoundBuffer8_SetRolloffCurve_1_0_5233,

        XTL::EmuIDirectSoundBuffer8_SetRolloffCurve,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetRolloffCurve"
        #endif
    },
    // CDirectSoundVoice::CommitDeferredSettings (XREF)
    {
        (OOVPA*)&CDirectSoundVoice_CommitDeferredSettings_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::CommitDeferredSettings (XREF)"
        #endif
    },
    // CDirectSoundVoice::SetAllParameters (XREF)
    /*{
        (OOVPA*)&CDirectSoundVoice_SetAllParameters_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetAllParameters (XREF)*************"
        #endif
    },
    // CDirectSoundBuffer::SetAllParameters (XREF)
    {
        (OOVPA*)&CDirectSoundBuffer_SetAllParameters_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetAllParameters (XREF)***************"
        #endif
    },
    // IDirectSoundBuffer8::SetAllParameters
    {
        (OOVPA*)&IDirectSoundBuffer8_SetAllParameters_1_0_5558,

        XTL::EmuIDirectSound8_SetAllParameters,     // Use that for now, EmuIDirectSoundBuffer8_SetAllParameters,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetAllParameters***************"
        #endif
    },*/
    // CDirectSoundVoice::SetPosition (XREF)
    {
        (OOVPA*)&CDirectSoundVoice_SetPosition_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetPosition (XRef)"
        #endif
    },
    // CDirectSoundBuffer::SetPosition (XREF)
    {
        (OOVPA*)&CDirectSoundBuffer_SetPosition_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetPosition (XRef)"
        #endif
    },
    // IDirectSoundBuffer_SetPosition
    {
        (OOVPA*)&IDirectSoundBuffer8_SetPosition_1_0_5558,
        XTL::EmuIDirectSoundBuffer8_SetPosition,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetPosition"
        #endif
    },
    // CDirectSoundVoice::SetVelocity (XREF)
    {
        (OOVPA*)&CDirectSoundVoice_SetVelocity_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetVelocity (XRef)"
        #endif
    },
    // CDirectSoundBuffer::SetVelocity (XREF)
    {
        (OOVPA*)&CDirectSoundBuffer_SetVelocity_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetVelocity (XRef)"
        #endif
    },
    // IDirectSoundBuffer::SetVelocity
    {
        (OOVPA*)&IDirectSoundBuffer8_SetVelocity_1_0_5558,
        XTL::EmuIDirectSoundBuffer8_SetVelocity,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetVelocity"
        #endif
    },
    // CDirectSoundVoice::SetDistanceFactor (XREF)
    {
        (OOVPA*)&CDirectSoundVoice_SetDistanceFactor_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetDistanceFactor (XRef)"
        #endif
    },
    // CDirectSoundBuffer::SetDistanceFactor (XREF)
    {
        (OOVPA*)&CDirectSoundBuffer_SetDistanceFactor_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetDistanceFactor (XRef)"
        #endif
    },
    // IDirectSoundBuffer_SetDistanceFactor
    {
        (OOVPA*)&IDirectSoundBuffer8_SetDistanceFactor_1_0_5558,
        XTL::EmuIDirectSoundBuffer8_SetDistanceFactor,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetDistanceFactor"
        #endif
    },
    // IDirectSoundStream::SetVolume (* unchanged since 5233 *)
    // (This is actually the XREF we are using)
    {
        (OOVPA*)&CDirectSoundStream_SetVolume_1_0_5233,

        XTL::EmuIDirectSoundStream_SetVolume,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundStream_SetVolume"
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
	// IDirectSoundBuffer_Lock
	{
		(OOVPA*)&IDirectSoundBuffer_Lock_1_0_5558,

		XTL::EmuIDirectSoundBuffer8_Lock,

		#ifdef _DEBUG_TRACE
		"EmuIDirectSoundBuffer8_Lock"
		#endif
	},
	// CDirectSoundVoice::SetI3DL2Source (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundVoice_SetI3DL2Source_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetI3DL2Source (XREF)"
        #endif
    },
    // CDirectSoundBuffer::SetI3DL2Source (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundBuffer_SetI3DL2Source_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetI3DL2Source (XREF)"
        #endif
    },
    // IDirectSoundBuffer_SetI3DL2Source
    {
        (OOVPA*)&IDirectSoundBuffer_SetI3DL2Source_1_0_5558,

        XTL::EmuIDirectSoundBuffer8_SetI3DL2Source,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetI3DL2Source"
        #endif
    },
	// CDirectSoundVoice::SetAllParameters (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundVoice_SetAllParameters_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetAllParameters (XREF)"
        #endif
    },
    // CDirectSoundBuffer::SetAllParameters (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundBuffer_SetAllParameters_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetAllParameters (XREF)"
        #endif
    },
    // IDirectSoundBuffer_SetAllParameters
    {
        (OOVPA*)&IDirectSoundBuffer_SetAllParameters_1_0_5558,

        XTL::EmuIDirectSoundBuffer8_SetAllParameters,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetAllParameters"
        #endif
    },
	// DirectSound::CDirectSoundVoice::SetHeadroom (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundVoice_SetHeadroom_1_0_5558,
        0,

        #ifdef _DEBUG_TRACE
        "DirectSound::CDirectSoundVoice::SetHeadroom (XREF)"
        #endif
    },
	// DirectSound::CDirectSoundBuffer::SetHeadroom (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundBuffer_SetHeadroom_1_0_5558,
        0,

        #ifdef _DEBUG_TRACE
        "DirectSound::CDirectSoundBuffer::SetHeadroom (XREF)"
        #endif
	},
	// IDirectSoundBuffer_SetHeadroom
    {
        (OOVPA*)&IDirectSoundBuffer_SetHeadroom_1_0_5558,
        XTL::EmuIDirectSoundBuffer8_SetHeadroom,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer_SetHeadroom"
        #endif
    },
	// DirectSound::CDirectSoundStream::SetHeadroom
    {
        (OOVPA*)&DirectSound_CDirectSoundStream_SetHeadroom_1_0_5558,
		XTL::EmuIDirectSoundStream_SetHeadroom,

        #ifdef _DEBUG_TRACE
        "DirectSound::CDirectSoundStream::SetHeadroom"
        #endif
	},
	// CDirectSoundBuffer::GetCurrentPosition
    {
        (OOVPA*)&DirectSound_CDirectSoundBuffer_GetCurrentPosition_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer_GetCurrentPosition (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::GetCurrentPosition
    {
        (OOVPA*)&IDirectSoundBuffer_GetCurrentPosition_1_0_5558,

        XTL::EmuIDirectSoundBuffer8_GetCurrentPosition,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_GetCurrentPosition"
        #endif
    },
	// CDirectSoundStream::SetFormat (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundStream_SetFormat_1_0_5558, 
		XTL::EmuCDirectSoundStream_SetFormat,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream::SetFormat"
        #endif
    },
	// CDirectSoundStream_SetMixBins
    {
        (OOVPA*)&DirectSound_CDirectSoundStream_SetMixBins_1_0_5558, 
		XTL::EmuCDirectSoundStream_SetMixBins,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream::SetMixBins"
        #endif
    },
	// CDirectSound_SetMixBinHeadroom
    {
        (OOVPA*)&DirectSound_CDirectSound_SetMixBinHeadroom_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound::SetMixBinHeadroom (XREF)"
        #endif
    },
    // IDirectSound8_SetMixBinHeadroom
    {
        (OOVPA*)&IDirectSound_SetMixBinHeadroom_1_0_5558,

        XTL::EmuIDirectSound8_SetMixBinHeadroom,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetMixBinHeadroom"
        #endif
    },
	// DirectSound::CDirectSound::EnableHeadphones (XRef)
    {
		(OOVPA*)&DirectSound_CDirectSound_EnableHeadphones_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "DirectSound::CDirectSound::EnableHeadphones (XRef)"
        #endif
    },
    // IDirectSound8::EnableHeadphones
    {
        (OOVPA*)&IDirectSound_EnableHeadphones_1_0_5558,

        XTL::EmuIDirectSound8_EnableHeadphones,

        #ifdef _DEBUG_TRACE
        "IDirectSound8_EnableHeadphones"
        #endif
    },
	// DirectSound::CDirectSound::GetOutputLevels (XREF)
	{
		(OOVPA*)&DirectSound_CDirectSound_GetOutputLevels_1_0_5558,
		0,

		#ifdef _DEBUG_TRACE
		"DirectSound::CDirectSound::GetOutputLevels (XREF)"
		#endif
	},
	// IDirectSound_GetOutputLevels
	{
		(OOVPA*)&IDirectSound_GetOutputLevels_1_0_5558,
		XTL::EmuIDirectSound8_GetOutputLevels,

		#ifdef _DEBUG_TRACE
		"IDirectSound_GetOutputLevels"
		#endif
	},
	// XAudioDownloadEffectsImage
	{
		(OOVPA*)&XAudioDownloadEffectsImage_1_0_5558,
		XTL::EmuXAudioDownloadEffectsImage,

		#ifdef _DEBUG_TRACE
		"XAudioDownloadEffectsImage"
		#endif
	},
	// CDirectSound::SetPosition (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSound_SetPosition_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetPosition (XRef)"
        #endif
    },
    // IDirectSound8::SetPosition
    {
        (OOVPA*)&IDirectSound_SetPosition_1_0_5558,

        XTL::EmuIDirectSound8_SetPosition,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound_SetPosition"
        #endif
    },
	// CDirectSound::SetVelocity (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSound_SetVelocity_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetVelocity (XRef)"
        #endif
    },
    // IDirectSound8::SetVelocity
    {
        (OOVPA*)&IDirectSound_SetVelocity_1_0_5558,

        XTL::EmuIDirectSound8_SetVelocity,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetVelocity"
        #endif
    },
	// CDirectSound_CreateSoundStream
    {
        (OOVPA*)&DirectSound_CDirectSound_CreateSoundStream_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound::CreateSoundStream (XREF)"
        #endif
    },
	// IDirectSound_CreateSoundStream
    {
        (OOVPA*)&IDirectSound_CreateSoundStream_1_0_5558, 
		XTL::EmuIDirectSound8_CreateStream,

        #ifdef _DEBUG_TRACE
        "IDirectSound_CreateSoundStream"
        #endif
    },
};

// ******************************************************************
// * DSound_1_0_5558_SIZE
// ******************************************************************
uint32 DSound_1_0_5558_SIZE = sizeof(DSound_1_0_5558);
