// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->DSound.1.0.5849.cpp
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
// * CMcpxBuffer_GetStatus
// ******************************************************************
SOOVPA<11> CMcpxBuffer_GetStatus_1_0_5849 =
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
// * CDirectSound_SetI3DL2Listener
// ******************************************************************
SOOVPA<8> CDirectSound_SetI3DL2Listener_1_0_5849 =
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
SOOVPA<9> IDirectSound8_SetI3DL2Listener_1_0_5849 =
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,	// Xref is used

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
// * CMcpxBuffer_SetBufferData
// ******************************************************************
SOOVPA<9> CMcpxBuffer_SetBufferData_1_0_5849 =
{
    0,  // Large == 0
    9,  // Count == 9

    XREF_DSSETBUFFERDATA,   // XRef Is  Saved
    XRefNotUsed,

    {
        // CMcpxBuffer_SetBufferData+0x1E : cmp eax, ebx
        { 0x1E, 0x3B }, // (Offset,Value)-Pair #1
        { 0x1F, 0xC3 }, // (Offset,Value)-Pair #2

        // CMcpxBuffer_SetBufferData+0x20 : jz +0x0C
        { 0x20, 0x74 }, // (Offset,Value)-Pair #3
        { 0x21, 0x0C }, // (Offset,Value)-Pair #4

        // CMcpxBuffer_SetBufferData+0x45 : mov [esi+0xBC], ecx
        { 0x45, 0x89 }, // (Offset,Value)-Pair #5
        { 0x46, 0x8E }, // (Offset,Value)-Pair #6
        { 0x47, 0xBC }, // (Offset,Value)-Pair #7

        // CMcpxBuffer_SetBufferData+0x89 : retn 0x08
        { 0x89, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x8A, 0x08 }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetFormat
// ******************************************************************
SOOVPA<7> DirectSound_CDirectSoundVoice_SetFormat_1_0_5849 =
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
// * DirectSound::CDirectSoundBuffer::SetFormat
// ******************************************************************
SOOVPA<8> DirectSound_CDirectSoundBuffer_SetFormat_1_0_5849 =
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DirectSound_CDirectSoundBuffer_SetFormat,  // Xref Is Saved
    1,

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
SOOVPA<8> IDirectSoundBuffer8_SetFormat_1_0_5849 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

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
// * CMcpxBuffer_Play
// ******************************************************************
SOOVPA<11> CMcpxBuffer_Play_1_0_5849 =
{
    0,  // Large == 0
    11, // Count == 11

    XREF_DirectSound_CMcpxBuffer_Play,    // XRef Is  Saved
    0,                  // XRef Not Used

    {
        // CMcpxBuffer_Play+0x0B : test dword ptr [eax+8], 0x182000
        { 0x0B, 0xF7 }, // (Offset,Value)-Pair #1
        { 0x0C, 0x40 }, // (Offset,Value)-Pair #2

        // CMcpxBuffer_Play+0x13 : jz +0x09
        { 0x13, 0x74 }, // (Offset,Value)-Pair #3
        { 0x14, 0x05 }, // (Offset,Value)-Pair #4

        // CMcpxBuffer_Play+0x32 : test byte ptr [esi+0x12], 0x42
        { 0x32, 0xF6 }, // (Offset,Value)-Pair #5
        { 0x33, 0x46 }, // (Offset,Value)-Pair #6
        { 0x34, 0x12 }, // (Offset,Value)-Pair #7
        { 0x35, 0x42 }, // (Offset,Value)-Pair #8

        // CMcpxBuffer_Play+0xA4 : call dword ptr [eax+0x1C]
        { 0xA4, 0xFF }, // (Offset,Value)-Pair #9
        { 0xA5, 0x50 }, // (Offset,Value)-Pair #10
        { 0xA6, 0x1C }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * DirectSound::CMcpxBuffer::Play
// ******************************************************************
SOOVPA<8> DirectSound_CMcpxBuffer_Play_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DirectSound_CMcpxBuffer_Play,  // Xref Is Saved
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
// * DirectSound::CMcpxBuffer::Play
// ******************************************************************
SOOVPA<7> DirectSound_CMcpxBuffer_Play2_1_0_5849 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DirectSound_CMcpxBuffer_Play2,  // Xref Is Saved
    XRefNotUsed,

    {
		{ 0x17, 0x43 },
		{ 0x2D, 0xF6 },
		{ 0x2E, 0xC3 },
		{ 0x2F, 0x04 },
        { 0xAD, 0xC2 },
		{ 0xAE, 0x04 },
		{ 0xAF, 0x00 },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::Play
// ******************************************************************
SOOVPA<9> DirectSound_CDirectSoundBuffer_Play_1_0_5849 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XREF_DSOUNDPLAY2,  // Xref Is Saved
    XRefNotUsed,

    {
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
SOOVPA<8> IDirectSoundBuffer_Play_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

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
// * DirectSound::CDirectSoundVoice::SetHeadroom
// ******************************************************************
SOOVPA<7> DirectSound_CDirectSoundVoice_SetHeadroom_1_0_5849 = 
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
SOOVPA<8> DirectSound_CDirectSoundBuffer_SetHeadroom_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DirectSound_CDirectSoundBuffer_SetHeadroom,  // Xref Is Saved
    1,

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
// * IDirectSoundBuffer_SetHeadroom
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer_SetHeadroom_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

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
// * IDirectSoundBuffer_Lock
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer_Lock_1_0_5849 = 
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
// * DirectSound::CDirectSoundVoiceSettings::SetMixBins
// ******************************************************************
SOOVPA<7> DirectSound_CDirectSoundVoiceSettings_SetMixBins_1_0_5849 = 
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
// * DirectSound::CDirectSoundVoice::SetMixBins
// ******************************************************************
SOOVPA<8> DirectSound_CDirectSoundVoice_SetMixBins_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSSETMIXBINSB,  // Xref Is Saved
    1,

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
// * DirectSound::CDirectSoundBuffer::SetMixBins
// ******************************************************************
SOOVPA<8> DirectSound_CDirectSoundBuffer_SetMixBins_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSSETMIXBINSA,  // Xref Is Saved
    1,

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
// * DirectSound::CDirectSoundStream::SetMixBins
// ******************************************************************
SOOVPA<9> DirectSound_CDirectSoundStream_SetMixBins_1_0_5849 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,

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
// * IDirectSoundBuffer_SetMixBins
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer_SetMixBins_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

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
// * DirectSound::CDirectSound::SetMixBinHeadroom
// ******************************************************************
SOOVPA<8> DirectSound_CDirectSound_SetMixBinHeadroom_1_0_5849 = 
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
SOOVPA<9> IDirectSound_SetMixBinHeadroom_1_0_5849 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,

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
// * DirectSound::CDirectSound::SetPosition
// ******************************************************************
SOOVPA<7> DirectSound_CDirectSound_SetPosition_1_0_5849 = 
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
SOOVPA<8> IDirectSound_SetPosition_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

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
SOOVPA<7> DirectSound_CDirectSound_SetVelocity_1_0_5849 = 
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
SOOVPA<8> IDirectSound_SetVelocity_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

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
// * DirectSound::CDirectSound::CommitDeferredSettings
// ******************************************************************
SOOVPA<7> DirectSound_CDirectSound_CommitDeferredSettings_1_0_5849 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_COMMITDEFERREDSETTINGSA,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x12, 0xC0 },
        { 0x27, 0xB8 },
        { 0x3A, 0x75 },
        { 0x52, 0x8D },
        { 0x62, 0x8B },
        { 0x76, 0x89 },
        { 0x8E, 0xFF },
    }
};

// ******************************************************************
// * DirectSound::CMcpxBuffer::SetCurrentPosition
// ******************************************************************
SOOVPA<7> DirectSound_CMcpxBuffer_SetCurrentPosition_1_0_5849 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DSBUFFERSETCURRENTPOSITIONB,  // Xref Is Saved
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
// * DirectSound::CDirectSoundBuffer::SetCurrentPosition
// ******************************************************************
SOOVPA<8> DirectSound_CDirectSoundBuffer_SetCurrentPosition_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSBUFFERSETCURRENTPOSITIONA,  // Xref Is Saved
    1,

    {
        { 0x35, XREF_DSBUFFERSETCURRENTPOSITIONB },

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
// * IDirectSoundBuffer_SetCurrentPosition
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer_SetCurrentPosition_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

    {
        { 0x15, XREF_DSBUFFERSETCURRENTPOSITIONA },

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
SOOVPA<7> DirectSound_CDirectSoundBuffer_GetCurrentPosition_1_0_5849 = 
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
// * IDirectSoundBuffer_GetCurrentPosition
// ******************************************************************
SOOVPA<9> IDirectSoundBuffer_GetCurrentPosition_1_0_5849 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,

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
// * DirectSound::CDirectSoundBuffer::SetLoopRegion
// ******************************************************************
SOOVPA<7> DirectSound_CDirectSoundBuffer_SetLoopRegion_1_0_5849 = 
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
SOOVPA<9> IDirectSoundBuffer_SetLoopRegion_1_0_5849 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,

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
// * DirectSound::CMcpxBuffer::Stop
// ******************************************************************
SOOVPA<8> DirectSound_CMcpxBuffer_Stop_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSOUNDSTOP,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x06, 0x56 },
        { 0x0E, 0xF1 },
        { 0x16, 0xFF },
        { 0x1D, 0x04 },
        { 0x1E, 0xFF },
        { 0x26, 0x75 },
        { 0x2F, 0x8B },
        { 0x36, 0xC2 },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::StopEx
// ******************************************************************
SOOVPA<8> DirectSound_CDirectSoundBuffer_StopEx_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSOUNDSTOP2,  // Xref Is Saved
    1,

    {
        { 0x3D, XREF_DSOUNDSTOP },

        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x20 },
        { 0x3A, 0x24 },
        { 0x46, 0x0B },
        { 0x52, 0x8B },
    }
};

// ******************************************************************
// * IDirectSoundBuffer_StopEx
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer_StopEx_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

    {
        { 0x1D, XREF_DSOUNDSTOP2 },

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
// * DirectSound::CDirectSound::SetRolloffFactor
// ******************************************************************
SOOVPA<9> DirectSound_CDirectSound_SetRolloffFactor_1_0_5849 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XREF_SETROLLOFFFACTORA,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x08 },
        { 0x35, 0x6C },
        { 0x3A, 0x48 },
        { 0x4A, 0x85 },
        { 0x53, 0xFF },
        { 0x5E, 0x00 },
    }
};

// ******************************************************************
// * DirectSound::CDirectSound::SetDopplerFactor
// ******************************************************************
SOOVPA<9> DirectSound_CDirectSound_SetDopplerFactor_1_0_5849 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XREF_SETDOPPLERFACTOR,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x08 },
        { 0x35, 0x70 },
        { 0x3A, 0x48 },
        { 0x4A, 0x85 },
        { 0x53, 0xFF },
        { 0x5E, 0x00 },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetPitch
// ******************************************************************
SOOVPA<6> DirectSound_CDirectSoundVoice_SetPitch_1_0_5849 = 
{
    0,  // Large == 0
    6,  // Count == 6

    XREF_DSBUFFERSETPITCHB,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x02, 0x24 },
        { 0x06, 0x10 },
        { 0x0A, 0x08 },
        { 0x0E, 0x8B },
        { 0x16, 0xC2 },
        { 0x17, 0x08 },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetPitch
// ******************************************************************
SOOVPA<8> DirectSound_CDirectSoundBuffer_SetPitch_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSBUFFERSETPITCHA,  // Xref Is Saved
    1,

    {
        { 0x32, XREF_DSBUFFERSETPITCHB },

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
// * IDirectSoundBuffer_SetPitch
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer8_SetPitch_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

    {
        { 0x15, XREF_DSBUFFERSETPITCHA },

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
// * DirectSound::CDirectSoundVoice::SetMaxDistance
// ******************************************************************
SOOVPA<7> DirectSound_CDirectSoundVoice_SetMaxDistance_1_0_5849 = 
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
// * DirectSound::CDirectSoundVoice::SetMinDistance
// ******************************************************************
SOOVPA<7> DirectSound_CDirectSoundVoice_SetMinDistance_1_0_5849 = 
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
// * DirectSound::CDirectSoundBuffer::SetMaxDistance
// ******************************************************************
SOOVPA<8> DirectSound_CDirectSoundBuffer_SetMaxDistance_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSBUFFERSETMAXDISTANCE,  // Xref Is Saved
    1,

    {
        { 0x3A, XREF_DSVOICESETMAXDISTANCE },

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
// * DirectSound::CDirectSoundBuffer::SetMinDistance
// ******************************************************************
SOOVPA<8> DirectSound_CDirectSoundBuffer_SetMinDistance_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSBUFFERSETMINDISTANCE,  // Xref Is Saved
    1,

    {
        { 0x3A, XREF_DSVOICESETMINDISTANCE },

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
// * IDirectSoundBuffer_SetMaxDistance
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer_SetMaxDistance_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

    {
        { 0x1D, XREF_DSBUFFERSETMAXDISTANCE },

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
// * IDirectSoundBuffer_SetMinDistance
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer_SetMinDistance_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

    {
        { 0x1D, XREF_DSBUFFERSETMINDISTANCE },

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
// * DirectSound::CDirectSoundVoice::SetVolume
// ******************************************************************
SOOVPA<7> DirectSound_CDirectSoundVoice_SetVolume_1_0_5849 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DirectSound_CDirectSoundVoice_SetVolume,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x02, 0x24 },
        { 0x06, 0x10 },
        { 0x0A, 0x08 },
        { 0x0E, 0x89 },
        { 0x12, 0x49 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetVolume
// ******************************************************************
SOOVPA<8> DirectSound_CDirectSoundBuffer_SetVolume_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DirectSound_CDirectSoundBuffer_SetVolume,  // Xref Is Saved
    1,

    {
        { 0x32, XREF_DirectSound_CDirectSoundVoice_SetVolume },

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
// * DirectSound::CDirectSoundStream::SetVolume
// ******************************************************************
// * Not until we need it!
//SOOVPA<9> DirectSound_CDirectSoundStream_SetVolume_1_0_5849 = 
//{
//    0,  // Large == 0
//    9,  // Count == 9
//
//    XREF_DirectSound_CDirectSoundStream_SetVolume,  // Xref Is Saved
//    XRefNotUsed,
//
//    {
//        { 0x36, XREF_DirectSound_CDirectSoundVoice_SetVolume },
//
//        { 0x00, 0x56 },
//        { 0x0C, 0x00 },
//        { 0x14, 0x74 },
//        { 0x21, 0xB8 },
//        { 0x2A, 0x24 },
//        { 0x35, 0xE8 },
//        { 0x40, 0x68 },
//        { 0x4B, 0x8B },
//    }
//};

// ******************************************************************
// * IDirectSoundBuffer_SetVolume
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer_SetVolume_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

    {
        { 0x15, XREF_DirectSound_CDirectSoundBuffer_SetVolume },

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
// * DirectSound::CDirectSoundBuffer::PlayEx
// ******************************************************************
SOOVPA<8> DirectSound_CDirectSoundBuffer_PlayEx_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DirectSound_CDirectSoundBuffer_PlayEx,  // Xref Is Saved
    1,

    {
        { 0x3D, XREF_DirectSound_CMcpxBuffer_Play },

        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x20 },
        { 0x3A, 0x24 },
        { 0x46, 0x0B },
        { 0x52, 0x8B },
    }
};

// ******************************************************************
// * IDirectSoundBuffer_PlayEx
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer_PlayEx_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

    {
        { 0x1D, XREF_DirectSound_CDirectSoundBuffer_PlayEx },

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
// * DirectSound::CDirectSoundVoice::SetRolloffFactor
// ******************************************************************
SOOVPA<7> DirectSound_CDirectSoundVoice_SetRolloffFactor_1_0_5849 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DSVOICESETROLLOFFFACTOR,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x05, 0x41 },
        { 0x0C, 0x00 },
        { 0x13, 0x48 },
        { 0x1A, 0x00 },
        { 0x21, 0xF6 },
        { 0x28, 0x51 },
        { 0x2F, 0xC0 },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetRolloffFactor
// ******************************************************************
SOOVPA<8> DirectSound_CDirectSoundBuffer_SetRolloffFactor_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSBUFFERSETROLLOFFFACTOR,  // Xref Is Saved
    1,

    {
        { 0x3A, XREF_DSVOICESETROLLOFFFACTOR },

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
// * IDirectSoundBuffer_SetRolloffFactor
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer_SetRolloffFactor_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

    {
        { 0x1D, XREF_DSBUFFERSETROLLOFFFACTOR },

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
// * DirectSound::CDirectSoundVoice::SetDopplerFactor
// ******************************************************************
SOOVPA<7> DirectSound_CDirectSoundVoice_SetDopplerFactor_1_0_5849 = 
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
// * DirectSound::CDirectSoundBuffer::SetDopplerFactor
// ******************************************************************
SOOVPA<8> DirectSound_CDirectSoundBuffer_SetDopplerFactor_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSBUFFERSETDOPPLERFACTOR,  // Xref Is Saved
    1,

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
// * IDirectSoundBuffer_SetDopplerFactor
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer_SetDopplerFactor_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

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
// * DirectSound::CDirectSoundVoice::SetPosition
// ******************************************************************
SOOVPA<7> DirectSound_CDirectSoundVoice_SetPosition_1_0_5849 = 
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
// * DirectSound::CDirectSoundBuffer::SetPosition
// ******************************************************************
SOOVPA<9> DirectSound_CDirectSoundBuffer_SetPosition_1_0_5849 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XREF_DSBUFFERSETPOSITION,  // Xref Is Saved
    1,

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
// * IDirectSoundBuffer_SetPosition
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer_SetPosition_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

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
// * DirectSound::CDirectSoundVoice::SetVelocity
// ******************************************************************
SOOVPA<7> DirectSound_CDirectSoundVoice_SetVelocity_1_0_5849 = 
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
// * DirectSound::CDirectSoundBuffer::SetVelocity
// ******************************************************************
SOOVPA<9> DirectSound_CDirectSoundBuffer_SetVelocity_1_0_5849 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XREF_DSBUFFERSETVELOCITY,  // Xref Is Saved
    1,

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
// * IDirectSoundBuffer_SetVelocity
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer_SetVelocity_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

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

// ******************************************************************
// * DirectSound::CMcpxBuffer::Pause
// ******************************************************************
SOOVPA<7> DirectSound_CMcpxBuffer_Pause_1_0_5849 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DirectSound_CMcpxBuffer_Pause,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x0D, 0x4D },
        { 0x1C, 0x44 },
        { 0x2B, 0x8B },
        { 0x3A, 0x75 },
        { 0x49, 0x7D },
        { 0x58, 0xEB },
        { 0x67, 0xE8 },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::Pause
// ******************************************************************
SOOVPA<8> DirectSound_CDirectSoundBuffer_Pause_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DirectSound_CDirectSoundBuffer_Pause,  // Xref Is Saved
    1,

    {
        { 0x35, XREF_DirectSound_CMcpxBuffer_Pause },

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
// * IDirectSoundBuffer_Pause
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer_Pause_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

    {
        { 0x15, XREF_DirectSound_CDirectSoundBuffer_Pause },

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
// * DirectSound::CDirectSound::CreateSoundStream
// ******************************************************************
SOOVPA<7> DirectSound_CDirectSound_CreateSoundStream_1_0_5849 = 
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
//SOOVPA<8> IDirectSound_CreateSoundStream_1_0_5849 = 
//{
//    0,  // Large == 0
//    8,  // Count == 8
//
//    XRefNotSaved,
//    XRefNotUsed,
//
//    {
//        { 0x1D, XREF_DirectSound_CDirectSound_CreateSoundStream },
//
//        { 0x03, 0x10 },
//        { 0x08, 0xFF },
//        { 0x0D, 0xC8 },
//        { 0x12, 0x83 },
//        { 0x17, 0x1B },
//        { 0x1C, 0xE8 },
//        { 0x21, 0xC2 },
//    }
//};

// ******************************************************************
// * DirectSoundCreateStream
// ******************************************************************
SOOVPA<9> DirectSoundCreateStream_1_0_5849 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,

    {
        { 0x2F, XREF_DSCREATESOUNDSTREAM },

        { 0x09, 0x56 },
        { 0x14, 0x45 },
        { 0x1F, 0x8B },
        { 0x2A, 0xFF },
        { 0x35, 0x85 },
        { 0x40, 0xFF },
        { 0x4E, 0x5F },
        { 0x56, 0x00 },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetMixBinVolumes
// ******************************************************************
SOOVPA<9> DirectSound_CDirectSoundStream_SetMixBinVolumes_1_0_5849 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,

    {
        { 0x36, XREF_DSSETMIXBINVOLUMESB },

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
// * DirectSound::CMcpxStream::Pause
// ******************************************************************
SOOVPA<7> DirectSound_CMcpxStream_Pause_1_0_5849 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DSSTREAMPAUSE,  // Xref Is Saved
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
};

// ******************************************************************
// * DirectSound::CDirectSoundStream::Pause
// ******************************************************************
SOOVPA<8> DirectSound_CDirectSoundStream_Pause_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

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
// * DirectSound::CDirectSoundStream::FlushEx
// ******************************************************************
SOOVPA<7> DirectSound_CDirectSoundStream_FlushEx_1_0_5849 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DSFLUSHEX2,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x0F, 0x00 },
        { 0x1E, 0xFF },
        { 0x28, 0x80 },
        { 0x36, 0x74 },
        { 0x46, 0x8B },
        { 0x52, 0xFF },
        { 0x62, 0x8B },
    }
};

// ******************************************************************
// * IDirectSoundStream_FlushEx
// ******************************************************************
SOOVPA<9> IDirectSoundStream_FlushEx_1_0_5849 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,

    {
        { 0x11, XREF_DSFLUSHEX2 },

        { 0x01, 0x74 },
        { 0x04, 0xFF },
        { 0x07, 0x10 },
        { 0x0A, 0x24 },
        { 0x0D, 0x74 },
        { 0x10, 0xE8 },
        { 0x15, 0xC2 },
        { 0x16, 0x10 },
    }
};

// ******************************************************************
// * DirectSoundDoWork
// ******************************************************************
SOOVPA<7> DirectSoundDoWork_1_0_5849 = 
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
// * IDirectSound_SetEffectData
// ******************************************************************
// * No XRefs! Woohoo!
// ******************************************************************
SOOVPA<7> IDirectSound_SetEffectData_1_0_5849 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x04, 0x75 },
        { 0x0A, 0x75 },
        { 0x10, 0x14 },
        { 0x16, 0x10 },
        { 0x1C, 0x1B },
        { 0x26, 0x5D },
        { 0x28, 0x18 },
    }
};

// ******************************************************************
// * DirectSound::CMemoryManager::PoolAlloc
// ******************************************************************
SOOVPA<10> DirectSound_CMemoryManager_PoolAlloc_1_0_5849 = 
{
	0,	// Large == 0
	10,	// Count == 10

	XRefNotSaved,
	XRefNotUsed,

	{
		{ 0x24, 0x83 },
		{ 0x25, 0x7C },
		{ 0x26, 0x24 },
		{ 0x27, 0x10 },
		{ 0x28, 0x00 },
		{ 0x37, 0xF3 },
		{ 0x38, 0xAB },
		{ 0x44, 0xC2 },
		{ 0x45, 0x0C },
		{ 0x46, 0x00 },
	}
};

// ******************************************************************
// * XFileCreateMediaObjectAsync
// ******************************************************************
SOOVPA<8> XFileCreateMediaObjectAsync_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x0A, 0xC0 },
        { 0x16, 0xF8 },
        { 0x22, 0xE6 },
        { 0x2E, 0x20 },
        { 0x3D, 0x8B },
        { 0x49, 0x8B },
        { 0x52, 0x06 },
        { 0x5E, 0x0C },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetPitch
// ******************************************************************
SOOVPA<12> DirectSound_CDirectSoundStream_SetPitch_1_0_5849 = 
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    1,

    {
        { 0x36, XREF_DSBUFFERSETPITCHB },

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
		{ 0x4F, 0xC2 },
		{ 0x50, 0x08 },
		{ 0x51, 0x00 },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetHeadroom
// ******************************************************************
SOOVPA<9> DirectSound_CDirectSoundStream_SetHeadroom_1_0_5849 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,

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
// * Direct_1_0_5849
// ******************************************************************
OOVPATable DSound_1_0_5849[] =
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
    // IDirectSoundBuffer8::SetPlayRegion (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirectSoundBuffer8_SetPlayRegion_1_0_4361,

        XTL::EmuIDirectSoundBuffer8_SetPlayRegion,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetPlayRegion"
        #endif
    },
    // CMcpxBuffer::SetBufferData
    {
        (OOVPA*)&CMcpxBuffer_SetBufferData_1_0_5849, 0,

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
    // CMcpxBuffer::GetStatus
    {
        (OOVPA*)&CMcpxBuffer_GetStatus_1_0_5849, 0,

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
    // CDirectSound_SetI3DL2Listener
    {
        (OOVPA*)&CDirectSound_SetI3DL2Listener_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound::SetI3DL2Listener (XREF)"
        #endif
    },
    // IDirectSound8_SetI3DL2Listener
    {
        (OOVPA*)&IDirectSound8_SetI3DL2Listener_1_0_5849,

        XTL::EmuIDirectSound8_SetI3DL2Listener,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetI3DL2Listener"
        #endif
    },
    // DirectSound::CDirectSoundVoice::SetFormat (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundVoice_SetFormat_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "DirectSound::CDirectSoundVoice::SetFormat (XREF)"
        #endif

    },
    // DirectSound::CDirectSoundBuffer::SetFormat (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundBuffer_SetFormat_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "DirectSound::CDirectSoundBuffer::SetFormat (XREF)"
        #endif
    },
    // IDirectSoundBuffer8_SetFormat
    {
        (OOVPA*)&IDirectSoundBuffer8_SetFormat_1_0_5849,

        XTL::EmuIDirectSoundBuffer8_SetFormat,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer_SetFormat@8"
        #endif
    },
    // CDirectSoundVoiceSettings_SetMixBinVolumes
    {
        (OOVPA*)&CDirectSoundVoiceSettings_SetMixBinVolumes_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoiceSettings::SetMixBinVolumes (XREF)"
        #endif
    },
    // CDirectSoundVoice_SetMixBinVolumes
    {
        (OOVPA*)&CDirectSoundVoice_SetMixBinVolumes_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetMixBinVolumes (XREF)"
        #endif
    },
    // CDirectSoundBuffer_SetMixBinVolumes
    {
        (OOVPA*)&CDirectSoundBuffer_SetMixBinVolumes_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetMixBinVolumes (XREF)"
        #endif
    },
    // IDirectSoundBuffer8_SetMixBinVolumes
    {
        (OOVPA*)&IDirectSoundBuffer8_SetMixBinVolumes_1_0_4627,

        XTL::EmuIDirectSoundBuffer8_SetMixBinVolumes,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetMixBinVolumes"
        #endif
    },
	// CDirectSoundStream_SetMixBinVolumes
	{
		(OOVPA*)&DirectSound_CDirectSoundStream_SetMixBinVolumes_1_0_5849,
		XTL::EmuCDirectSoundStream_SetMixBinVolumes2,

		#ifdef _DEBUG_TRACE
		"CDirectSoundStream::SetMixBinVolumes (XREF) ****"
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
    // CDirectSoundVoice::SetFrequency (XREF)
    {
        (OOVPA*)&CDirectSoundVoice_SetFrequency_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice_SetFrequency (XRef)"
        #endif
    },
    // CDirectSoundBuffer::SetFrequency
    {
        (OOVPA*)&CDirectSoundBuffer_SetFrequency_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSoundBuffer_SetFrequency (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::SetFrequency
    {
        (OOVPA*)&IDirectSoundBuffer8_SetFrequency_1_0_4627,

        XTL::EmuIDirectSoundBuffer8_SetFrequency,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetFrequency"
        #endif
    },
    // CMcpxVoiceClient_SetVolume
    {
        (OOVPA*)&CMcpxVoiceClient_SetVolume_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxVoiceClient::SetVolume (XREF)"
        #endif
    },
    // CDirectSoundStream_SetVolume
    {
        (OOVPA*)&CDirectSoundStream_SetVolume_1_0_4627,

        XTL::EmuCDirectSoundStream_SetVolume,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSoundStream_SetVolume"
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
		(OOVPA*)&DirectSound_CDirectSoundVoice_SetMaxDistance_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetMaxDistance (XRef)"
        #endif
    },
    // CDirectSoundBuffer::SetMaxDistance (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundBuffer_SetMaxDistance_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetMaxDistance (XRef)"
        #endif
    },
    // IDirectSoundBuffer_SetMaxDistance
    {
        (OOVPA*)&IDirectSoundBuffer_SetMaxDistance_1_0_5849,
        XTL::EmuIDirectSoundBuffer8_SetMaxDistance,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetMaxDistance"
        #endif
    },
	// CDirectSoundVoice::SetMinDistance (XREF) 
    {
		(OOVPA*)&DirectSound_CDirectSoundVoice_SetMinDistance_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetMinDistance (XRef)"
        #endif
    },
    // CDirectSoundBuffer::SetMinDistance (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundBuffer_SetMinDistance_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetMinDistance (XRef)"
        #endif
    },
    // IDirectSoundBuffer_SetMinDistance
    {
        (OOVPA*)&IDirectSoundBuffer_SetMinDistance_1_0_5849,
        XTL::EmuIDirectSoundBuffer8_SetMinDistance,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetMinDistance"
        #endif
    },
    // CMcpxBuffer::Play 
    {
        (OOVPA*)&DirectSound_CMcpxBuffer_Play_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxBuffer_Play (XREF)"
        #endif
    },
	// CMcpxBuffer::Play
    /*{
        (OOVPA*)&CMcpxBuffer_Play_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxBuffer_Play2 (XREF)"
        #endif
    },*/
    // IDirectSoundBuffer_Stop (Possibly weak, but quite OK for 4627 DSOUND)
    {
        (OOVPA*)&IDirectSoundBuffer_Stop_1_0_4627,

        XTL::EmuIDirectSoundBuffer8_Stop,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_Stop"
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
	// DirectSound::CDirectSoundVoice::SetHeadroom (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundVoice_SetHeadroom_1_0_5849,
        0,

        #ifdef _DEBUG_TRACE
        "DirectSound::CDirectSoundVoice::SetHeadroom (XREF)"
        #endif
    },
	// DirectSound::CDirectSoundBuffer::SetHeadroom (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundBuffer_SetHeadroom_1_0_5849,
        0,

        #ifdef _DEBUG_TRACE
        "DirectSound::CDirectSoundBuffer::SetHeadroom (XREF)"
        #endif
	},
	// IDirectSoundBuffer_SetHeadroom
    {
        (OOVPA*)&IDirectSoundBuffer_SetHeadroom_1_0_5849,
        XTL::EmuIDirectSoundBuffer8_SetHeadroom,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer_SetHeadroom"
        #endif
    },
	// IDirectSoundBuffer_Lock
    {
        (OOVPA*)&IDirectSoundBuffer_Lock_1_0_5849,
        XTL::EmuIDirectSoundBuffer8_Lock,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer_Lock"
        #endif
    },
	// DirectSound::CDirectSoundVoiceSettings::SetMixBins (XREF)
	{
		(OOVPA*)&DirectSound_CDirectSoundVoiceSettings_SetMixBins_1_0_5849,
		0,

		#ifdef _DEBUG_TRACE
		"DirectSound::CDirectSoundVoiceSettings::SetMixBins (XREF)"
		#endif
	},
	// DirectSound::CDirectSoundVoice::SetMixBins (XREF)
	{
		(OOVPA*)&DirectSound_CDirectSoundVoice_SetMixBins_1_0_5849,
		0,

		#ifdef _DEBUG_TRACE
		"DirectSound::CDirectSoundVoice::SetMixBins (XREF)"
		#endif
	},
	// DirectSound::CDirectSoundBuffer::SetMixBins (XREF)
	{
		(OOVPA*)&DirectSound_CDirectSoundBuffer_SetMixBins_1_0_5849,
		0,

		#ifdef _DEBUG_TRACE
		"DirectSound::CDirectSoundBuffer::SetMixBins (XREF)"
		#endif
	},
	// IDirectSoundBuffer_SetMixBins
	{
		(OOVPA*)&IDirectSoundBuffer_SetMixBins_1_0_5849,
		XTL::EmuIDirectSoundBuffer8_SetMixBins,

		#ifdef _DEBUG_TRACE
		"IDirectSoundBuffer_SetMixBins"
		#endif
	},
	// DirectSound::CDirectSoundStream::SetMixBins (XREF)
	{
		(OOVPA*)&DirectSound_CDirectSoundStream_SetMixBins_1_0_5849,
		XTL::EmuCDirectSoundStream_SetMixBins,

		#ifdef _DEBUG_TRACE
		"DirectSound::CDirectSoundStream::SetMixBins"
		#endif
	},
	// CDirectSound_SetMixBinHeadroom
    {
        (OOVPA*)&DirectSound_CDirectSound_SetMixBinHeadroom_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound::SetMixBinHeadroom (XREF)"
        #endif
    },
    // IDirectSound8_SetMixBinHeadroom
    {
        (OOVPA*)&IDirectSound_SetMixBinHeadroom_1_0_5849,

        XTL::EmuIDirectSound8_SetMixBinHeadroom,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound_SetMixBinHeadroom"
        #endif
    },
	// CDirectSound::SetPosition (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSound_SetPosition_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetPosition (XRef)"
        #endif
    },
    // IDirectSound8::SetPosition
    {
        (OOVPA*)&IDirectSound_SetPosition_1_0_5849,

        XTL::EmuIDirectSound8_SetPosition,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound_SetPosition"
        #endif
    },
	// CDirectSound::SetVelocity (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSound_SetVelocity_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetVelocity (XRef)"
        #endif
    },
    // IDirectSound8::SetVelocity
    {
        (OOVPA*)&IDirectSound_SetVelocity_1_0_5849,

        XTL::EmuIDirectSound8_SetVelocity,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetVelocity"
        #endif
    },
	// CDirectSound::CommitDeferredSettings
    {
        (OOVPA*)&DirectSound_CDirectSound_CommitDeferredSettings_1_0_5849,

        XTL::EmuCDirectSound_CommitDeferredSettings,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSound_CommitDeferredSettings"
        #endif
    },
	// DirectSoundCreateBuffer
    {
        (OOVPA*)&DirectSoundCreateBuffer_1_0_4627,

        XTL::EmuDirectSoundCreateBuffer,

        #ifdef _DEBUG_TRACE
		"EmuDirectSoundCreateBuffer (4627)"
        #endif
    },
	// CMcpxBuffer::SetCurrentPosition
    {
        (OOVPA*)&DirectSound_CMcpxBuffer_SetCurrentPosition_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxBuffer_SetCurrentPosition (XREF)"
        #endif
    },
    // CDirectSoundBuffer::SetCurrentPosition
    {
        (OOVPA*)&DirectSound_CDirectSoundBuffer_SetCurrentPosition_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer_SetCurrentPosition (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::SetCurrentPosition
    {
        (OOVPA*)&IDirectSoundBuffer_SetCurrentPosition_1_0_5849,

        XTL::EmuIDirectSoundBuffer8_SetCurrentPosition,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetCurrentPosition"
        #endif
    },
	// CDirectSoundBuffer::GetCurrentPosition
    {
        (OOVPA*)&DirectSound_CDirectSoundBuffer_GetCurrentPosition_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer_GetCurrentPosition (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::GetCurrentPosition
    {
        (OOVPA*)&IDirectSoundBuffer_GetCurrentPosition_1_0_5849,

        XTL::EmuIDirectSoundBuffer8_GetCurrentPosition,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_GetCurrentPosition"
        #endif
    },
	// CDirectSoundBuffer::SetLoopRegion (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundBuffer_SetLoopRegion_1_0_5849,0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetLoopRegion (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::SetLoopRegion
    {
        (OOVPA*)&IDirectSoundBuffer_SetLoopRegion_1_0_5849,

        XTL::EmuIDirectSoundBuffer8_SetLoopRegion,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetLoopRegion"
        #endif
    },
	// CMcpxBuffer_Stop
	{
        (OOVPA*)&DirectSound_CMcpxBuffer_Stop_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxBuffer_Stop (XREF)"
        #endif
    },
	// CDirectSoundBuffer_Stop
	{
        (OOVPA*)&DirectSound_CDirectSoundBuffer_StopEx_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer_StopEx (XREF)"
        #endif
    },
	// CDirectSoundBuffer_Stop
	{
        (OOVPA*)&DirectSound_CDirectSoundBuffer_StopEx_1_0_5849, 
		XTL::EmuIDirectSoundBuffer8_StopEx,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer_StopEx"
        #endif
    },
	// CDirectSound::SetRolloffFactor (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSound_SetRolloffFactor_1_0_5849, 0,

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
        (OOVPA*)&DirectSound_CDirectSound_SetDopplerFactor_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetDopplerFactor (XRef)"
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
	// CDirectSoundVoice::SetPitch
    {
        (OOVPA*)&DirectSound_CDirectSoundVoice_SetPitch_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
		"CDirectSoundVoice::SetPitch (XREF)"
        #endif
    },
	// IDirectSoundBuffer8::SetPitch
    {
        (OOVPA*)&DirectSound_CDirectSoundBuffer_SetPitch_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
		"CDirectSoundBuffer::SetPitch (XREF)"
        #endif
    },
	// IDirectSoundBuffer8::SetPitch
    {
        (OOVPA*)&IDirectSoundBuffer8_SetPitch_1_0_5849,

        XTL::EmuIDirectSoundBuffer8_SetPitch,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetPitch"
        #endif
    },
	// CDirectSoundVoice::SetVolume (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundVoice_SetVolume_1_0_5849,0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetVolume (XREF)"
        #endif
    },
    // CDirectSoundBuffer::SetVolume (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundBuffer_SetVolume_1_0_5849,0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetVolume (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::SetVolume
    {
        (OOVPA*)&IDirectSoundBuffer_SetVolume_1_0_5849,

        XTL::EmuIDirectSoundBuffer8_SetVolume,

        #ifdef _DEBUG_TRACE
        "EmuIIDirectSoundBuffer8_SetVolume"
        #endif
    },
	// DirectSound::CDirectSoundBuffer::PlayEx (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundBuffer_PlayEx_1_0_5849,
        0,

        #ifdef _DEBUG_TRACE
        "DirectSound::CDirectSoundBuffer::PlayEx (XREF)"
        #endif
    },
    // IDirectSoundBuffer_PlayEx 
    {
        (OOVPA*)&IDirectSoundBuffer_PlayEx_1_0_5849,
        XTL::EmuIDirectSoundBuffer8_PlayEx,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer_PlayEx"
        #endif
    },
	// CDirectSoundVoice::SetRolloffFactor (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundVoice_SetRolloffFactor_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetRolloffFactor (XRef)"
        #endif
    },
    // CDirectSoundBuffer::SetRolloffFactor (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundBuffer_SetRolloffFactor_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetRolloffFactor (XRef)"
        #endif
    },
    // IDirectSoundBuffer_SetRolloffFactor
    {
        (OOVPA*)&IDirectSoundBuffer_SetRolloffFactor_1_0_5849,
        XTL::EmuIDirectSoundBuffer8_SetRolloffFactor,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetRolloffFactor"
        #endif
    },
	// CDirectSoundVoice::SetDopplerFactor (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundVoice_SetDopplerFactor_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetDopplerFactor (XRef)"
        #endif
    },
    // CDirectSoundBuffer::SetDopplerFactor (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundBuffer_SetDopplerFactor_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetDopplerFactor (XRef)"
        #endif
    },
    // IDirectSoundBuffer8_SetDopplerFactor
    {
        (OOVPA*)&IDirectSoundBuffer_SetDopplerFactor_1_0_5849,
        XTL::EmuIDirectSoundBuffer8_SetDopplerFactor,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetDopplerFactor"
        #endif
    },
	// CDirectSoundVoice::SetPosition (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundVoice_SetPosition_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetPosition (XRef)"
        #endif
    },
    // CDirectSoundBuffer::SetPosition (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundBuffer_SetPosition_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetPosition (XRef)"
        #endif
    },
    // IDirectSoundBuffer8_SetPosition
	{
        (OOVPA*)&IDirectSoundBuffer_SetPosition_1_0_5849,
        XTL::EmuIDirectSoundBuffer8_SetPosition,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetPosition"
        #endif
    },
	// CDirectSoundVoice::SetVelocity (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundVoice_SetVelocity_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetVelocity (XRef)"
        #endif
    },
    // CDirectSoundBuffer::SetVelocity (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundBuffer_SetVelocity_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetVelocity (XRef)"
        #endif
    },
    // IDirectSoundBuffer8_SetVelocity
	{
        (OOVPA*)&IDirectSoundBuffer_SetVelocity_1_0_5849,
        XTL::EmuIDirectSoundBuffer8_SetVelocity,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetVelocity"
        #endif
    },
	// DirectSound_CMcpxBuffer_Pause_1_0_5849
	{
		(OOVPA*)&DirectSound_CMcpxBuffer_Pause_1_0_5849, 0,

		#ifdef _DEBUG_TRACE
		"CMcpxBuffer::Pause (XREF)"
		#endif
	},
	// CDirectSoundBuffer::Pause
    {
        (OOVPA*)&DirectSound_CDirectSoundBuffer_Pause_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
		"CDirectSoundBuffer::Pause (XREF)"
        #endif
    },
	// IDirectSoundBuffer8_Pause
    {
        (OOVPA*)&IDirectSoundBuffer_Pause_1_0_5849, 
			
		XTL::EmuIDirectSoundBuffer8_Pause,

        #ifdef _DEBUG_TRACE
		"IDirectSoundBuffer8_Pause"
        #endif
    },
	// CDirectSound_CreateSoundStream
    {
        (OOVPA*)&DirectSound_CDirectSound_CreateSoundStream_1_0_5849,
            
        XTL::EmuDirectSoundCreateStream,

        #ifdef _DEBUG_TRACE
        "CDirectSound::CreateSoundStream (XREF)"
        #endif
    },
    // DirectSoundCreateStream
    {
        (OOVPA*)&DirectSoundCreateStream_1_0_5849,

        XTL::EmuDirectSoundCreateStream,

        #ifdef _DEBUG_TRACE
        "EmuDirectSoundCreateStream"
        #endif
    },
	// CMcpxStream::Pause
    {
        (OOVPA*)&DirectSound_CMcpxStream_Pause_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxStream_Pause (XREF)"
        #endif
    },
    // CDirectSoundStream::Pause
	{
        (OOVPA*)&DirectSound_CDirectSoundStream_Pause_1_0_5849,

        XTL::EmuCDirectSoundStream_Pause,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_Pause"
        #endif
    },
	// CDirectSoundStream::FlushEx (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundStream_FlushEx_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream::FlushEx (XREF)"
        #endif
    },
	// IDirectSoundStream_FlushEx
    {
        (OOVPA*)&IDirectSoundStream_FlushEx_1_0_5849, 
		XTL::EmuIDirectSoundStream_FlushEx,

        #ifdef _DEBUG_TRACE
        "IDirectSoundStream_FlushEx"
        #endif
    },
	// DirectSoundDoWork
    {
        (OOVPA*)&DirectSoundDoWork_1_0_5849,

        XTL::EmuDirectSoundDoWork,

        #ifdef _DEBUG_TRACE
        "EmuDirectSoundDoWork"
        #endif
    },
	// CDirectSound::SynchPlayback (* unchanged since 5558 *)
    {
        (OOVPA*)&CDirectSound_SynchPlayback_1_0_5558,

        XTL::EmuCDirectSound_SynchPlayback,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SynchPlayback_1_0_5558"
        #endif
    },
	// XAudioDownloadEffectsImage (* unchanged since 4627 *)
	{
        (OOVPA*)&XAudioDownloadEffectsImage_1_0_4627,
        XTL::EmuXAudioDownloadEffectsImage,

        #ifdef _DEBUG_TRACE
        "EmuXAudioDownloadEffectsImage"
        #endif
    },
	// IDirectSoundStream_FlushEx
    {
        (OOVPA*)&IDirectSound_SetEffectData_1_0_5849, 
		XTL::EmuIDirectSound8_SetEffectData,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound_SetEffectData"
        #endif
    },
	// DirectSound::CMemoryManager::PoolAlloc
	{
        (OOVPA*)&DirectSound_CMemoryManager_PoolAlloc_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
		"DirectSound::CMemoryManager::PoolAlloc (XREF)"
        #endif
    },
	// XFileCreateMediaObjectAsync
	{
        (OOVPA*)&XFileCreateMediaObjectAsync_1_0_5849, 
			
		XTL::EmuXFileCreateMediaObjectAsync,

        #ifdef _DEBUG_TRACE
		"XFileCreateMediaObjectAsync"
        #endif
    },
	// CDirectSoundStream::SetFormat (* unchanged since 5558 *)
    {
        (OOVPA*)&DirectSound_CDirectSoundStream_SetFormat_1_0_5558, 
		XTL::EmuCDirectSoundStream_SetFormat,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream::SetFormat"
        #endif
    },
	// CDirectSoundStream::SetPitch
    {
        (OOVPA*)&DirectSound_CDirectSoundStream_SetPitch_1_0_5849, 
		XTL::EmuCDirectSoundStream_SetPitch,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream::SetPitch"
        #endif
    },
	// DirectSound::CDirectSoundStream::SetHeadroom
    {
        (OOVPA*)&DirectSound_CDirectSoundStream_SetHeadroom_1_0_5849,
		XTL::EmuIDirectSoundStream_SetHeadroom,

        #ifdef _DEBUG_TRACE
        "DirectSound::CDirectSoundStream::SetHeadroom"
        #endif
	},
	// CMcpxBuffer::Play2
    {
        (OOVPA*)&DirectSound_CMcpxBuffer_Play2_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxBuffer_Play2 (XREF)"
        #endif
    },
	// CDirectSoundBuffer::Play 
    {
        (OOVPA*)&DirectSound_CDirectSoundBuffer_Play_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer_Play (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::Play 
    {
        (OOVPA*)&IDirectSoundBuffer_Play_1_0_5849,

        XTL::EmuIDirectSoundBuffer8_Play,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_Play"
        #endif
    },
};

// ******************************************************************
// * DSound_1_0_5849_SIZE
// ******************************************************************
uint32 DSound_1_0_5849_SIZE = sizeof(DSound_1_0_5849);
