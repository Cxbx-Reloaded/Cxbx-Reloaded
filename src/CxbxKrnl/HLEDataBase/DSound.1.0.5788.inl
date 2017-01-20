// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->DSound.1.0.5788.cpp
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
OOVPA_XREF(CMcpxBuffer_GetStatus_1_0_5788, 11,

    XREF_DSBUFFERGETSTATUSA,
    XRefZero)

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
OOVPA_END;

// ******************************************************************
// * CDirectSound_SetI3DL2Listener
// ******************************************************************
OOVPA_XREF(CDirectSound_SetI3DL2Listener_1_0_5788, 8,

    XREF_DSSETI3DL2LISTENER,
    XRefZero)

        { 0x1D, 0xFF },
        { 0x3C, 0x00 },
        { 0x5B, 0x9A },
        { 0x7A, 0x10 },
        { 0x99, 0x00 },
        { 0xB8, 0x50 },
        { 0xD7, 0x00 },
        { 0xF6, 0x68 },
OOVPA_END;

// ******************************************************************
// * IDirectSound8_SetI3DL2Listener
// ******************************************************************
OOVPA_XREF(IDirectSound8_SetI3DL2Listener_1_0_5788, 9,

    XRefNoSaveIndex,
    XRefOne)

        { 0x19, XREF_DSSETI3DL2LISTENER },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0xFF },
        { 0x0E, 0x83 },
        { 0x12, 0xD9 },
        { 0x16, 0xC8 },
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_SetBufferData
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_SetBufferData_1_0_5788, 9,

    XREF_DSSETBUFFERDATA,
    XRefZero)

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
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetFormat
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetFormat_1_0_5788, 7,

    XREF_DirectSound_CDirectSoundVoice_SetFormat,
    XRefZero)

        { 0x07, 0x10 },
        { 0x13, 0x85 },
        { 0x19, 0x10 },
        { 0x22, 0x0C },
        { 0x2B, 0x14 },
        { 0x34, 0x85 },
        { 0x40, 0x5E },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetFormat
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetFormat_1_0_5788, 8,

    XREF_DirectSound_CDirectSoundBuffer_SetFormat,
    XRefOne)

        { 0x32, XREF_DirectSound_CDirectSoundVoice_SetFormat },

        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0x10 },
        { 0x3A, 0x74 },
        { 0x47, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetFormat
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetFormat_1_0_5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x15, XREF_DirectSound_CDirectSoundBuffer_SetFormat },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_Play
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_Play_1_0_5788, 11,

    XREF_DirectSound_CMcpxBuffer_Play,
    XRefZero)

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
OOVPA_END;

// ******************************************************************
// * DirectSound::CMcpxBuffer::Play
// ******************************************************************
OOVPA_XREF(DirectSound_CMcpxBuffer_Play_1_0_5788, 8,

    XREF_DirectSound_CMcpxBuffer_Play,
    XRefZero)

        { 0x06, 0x56 },
        { 0x0E, 0xF1 },
        { 0x16, 0xFF },
        { 0x1D, 0x03 },
        { 0x1E, 0xFF },
        { 0x26, 0x75 },
        { 0x2F, 0x8B },
        { 0x36, 0xC2 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CMcpxBuffer::Play
// ******************************************************************
OOVPA_XREF(DirectSound_CMcpxBuffer_Play2_1_0_5788, 7,

    XREF_DirectSound_CMcpxBuffer_Play2,
    XRefZero)

	{ 0x17, 0x43 },
	{ 0x2D, 0xF6 },
	{ 0x2E, 0xC3 },
	{ 0x2F, 0x04 },
        { 0xAD, 0xC2 },
	{ 0xAE, 0x04 },
	{ 0xAF, 0x00 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::Play
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_Play_1_0_5788, 9,

    XREF_DSOUNDPLAY2,
    XRefZero)

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x2E, 0x20 },
        { 0x33, 0x18 },
        { 0x39, 0x85 },
        { 0x44, 0xFF },
        { 0x4B, 0xC7 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_Play
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_Play_1_0_5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x1D, XREF_DSOUNDPLAY2 },

        { 0x03, 0x10 },
        { 0x08, 0xFF },
        { 0x0D, 0xC8 },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetHeadroom
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetHeadroom_1_0_5788, 7,

    XREF_DirectSound_CDirectSoundVoice_SetHeadroom,
    XRefZero)

        { 0x03, 0x04 },
        { 0x08, 0x48 },
        { 0x0D, 0x24 },
        { 0x12, 0x48 },
        { 0x17, 0x8B },
        { 0x1F, 0x5E },
        { 0x21, 0x08 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetHeadroom
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetHeadroom_1_0_5788, 8,

    XREF_DirectSound_CDirectSoundBuffer_SetHeadroom,
    XRefOne)

        { 0x32, XREF_DirectSound_CDirectSoundVoice_SetHeadroom },

        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0x10 },
        { 0x3A, 0x74 },
        { 0x47, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetHeadroom
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetHeadroom_1_0_5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x15, XREF_DirectSound_CDirectSoundBuffer_SetHeadroom },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_Lock
// ******************************************************************
OOVPA_NO_XREF(IDirectSoundBuffer_Lock_1_0_5788, 8)

        { 0x04, 0x75 },
        { 0x0A, 0x75 },
        { 0x10, 0x1C },
        { 0x16, 0x18 },
        { 0x1C, 0x1B },
        { 0x22, 0xC8 },
        { 0x2C, 0x5D },
        { 0x2E, 0x20 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoiceSettings::SetMixBins
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoiceSettings_SetMixBins_1_0_5788, 7,

    XREF_DSSETMIXBINSC,
    XRefZero)

        { 0x1B, 0xEB },
        { 0x32, 0x8B },
        { 0x4C, 0x7C },
        { 0x66, 0x00 },
        { 0x84, 0xF6 },
        { 0x9A, 0xB4 },
        { 0xB4, 0x00 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetMixBins
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetMixBins_1_0_5788, 8,

    XREF_DSSETMIXBINSB,
    XRefOne)

        { 0x0D, XREF_DSSETMIXBINSC },

        { 0x02, 0x74 },
        { 0x06, 0x74 },
        { 0x0A, 0x4E },
        { 0x11, 0x8B },
        { 0x12, 0x4E },
        { 0x19, 0x5E },
        { 0x1A, 0xC2 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetMixBins
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetMixBins_1_0_5788, 8,

    XREF_DSSETMIXBINSA,
    XRefOne)

        { 0x32, XREF_DSSETMIXBINSB },

        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0x10 },
        { 0x3A, 0x74 },
        { 0x47, 0x8B },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetMixBins
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundStream_SetMixBins_1_0_5788, 9,

    XRefNoSaveIndex,
    XRefOne)

        { 0x36, XREF_DSSETMIXBINSB },

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetMixBins
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMixBins_1_0_5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x15, XREF_DSSETMIXBINSA },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSound::SetMixBinHeadroom
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_SetMixBinHeadroom_1_0_5788, 8,

    XREF_DSSETMIXBINHEADROOMA,
    XRefZero)

        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x24 },
        { 0x3A, 0x5C },
        { 0x46, 0x85 },
        { 0x52, 0x15 },
        { 0x5E, 0x00 },
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetMixBinHeadroom
// ******************************************************************
OOVPA_XREF(IDirectSound_SetMixBinHeadroom_1_0_5788, 9,

    XRefNoSaveIndex,
    XRefOne)

        { 0x19, XREF_DSSETMIXBINHEADROOMA },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0xFF },
        { 0x0E, 0x83 },
        { 0x12, 0xD9 },
        { 0x16, 0xC8 },
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSound::SetPosition
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_SetPosition_1_0_5788, 7,

    XREF_DSSETPOSITIONA,
    XRefZero)

        { 0x0F, 0x00 },
        { 0x1E, 0xFF },
        { 0x2B, 0x8B },
        { 0x3A, 0x8B },
        { 0x49, 0x40 },
        { 0x58, 0xE8 },
        { 0x67, 0x15 },
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetPosition
// ******************************************************************
OOVPA_XREF(IDirectSound_SetPosition_1_0_5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x2D, XREF_DSSETPOSITIONA },

        { 0x05, 0x18 },
        { 0x0C, 0x83 },
        { 0x13, 0x8B },
        { 0x1A, 0xF8 },
        { 0x21, 0xD9 },
        { 0x28, 0xD9 },
        { 0x31, 0x5D },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSound::SetVelocity
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_SetVelocity_1_0_5788, 7,

    XREF_DSSETVELOCITYA,
    XRefZero)

        { 0x0F, 0x00 },
        { 0x1E, 0xFF },
        { 0x2B, 0x8B },
        { 0x3A, 0x8B },
        { 0x49, 0x4C },
        { 0x58, 0xE8 },
        { 0x67, 0x15 },
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetVelocity
// ******************************************************************
OOVPA_XREF(IDirectSound_SetVelocity_1_0_5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x2D, XREF_DSSETVELOCITYA },

        { 0x05, 0x18 },
        { 0x0C, 0x83 },
        { 0x13, 0x8B },
        { 0x1A, 0xF8 },
        { 0x21, 0xD9 },
        { 0x28, 0xD9 },
        { 0x31, 0x5D },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSound::CommitDeferredSettings
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_CommitDeferredSettings_1_0_5788, 7,

    XREF_COMMITDEFERREDSETTINGSA,
    XRefZero)

        { 0x12, 0xC0 },
        { 0x27, 0xB8 },
        { 0x3A, 0x75 },
        { 0x52, 0x8D },
        { 0x62, 0x8B },
        { 0x76, 0x89 },
        { 0x8E, 0xFF },
OOVPA_END;

// ******************************************************************
// * DirectSound::CMcpxBuffer::SetCurrentPosition
// ******************************************************************
OOVPA_XREF(DirectSound_CMcpxBuffer_SetCurrentPosition_1_0_5788, 7,

    XREF_DSBUFFERSETCURRENTPOSITIONB,
    XRefZero)

        { 0x1D, 0x00 },
        { 0x3C, 0x66 },
        { 0x5B, 0x00 },
        { 0x7C, 0x8D },
        { 0x99, 0xD1 },
        { 0xB8, 0x89 },
        { 0xDB, 0x5F },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetCurrentPosition
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetCurrentPosition_1_0_5788, 8,

    XREF_DSBUFFERSETCURRENTPOSITIONA,
    XRefOne)

        { 0x35, XREF_DSBUFFERSETCURRENTPOSITIONB },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x39, 0x85 },
        { 0x44, 0xFF },
        { 0x4B, 0xC7 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetCurrentPosition
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetCurrentPosition_1_0_5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x15, XREF_DSBUFFERSETCURRENTPOSITIONA },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::GetCurrentPosition
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_GetCurrentPosition_1_0_5788, 7,

    XREF_GETCURRENTPOSITION,
    XRefZero)

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0x74 },
        { 0x40, 0xF8 },
        { 0x4E, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_GetCurrentPosition
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_GetCurrentPosition_1_0_5788, 9,

    XRefNoSaveIndex,
    XRefOne)

        { 0x19, XREF_GETCURRENTPOSITION },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0xFF },
        { 0x0E, 0x83 },
        { 0x12, 0xD9 },
        { 0x16, 0xC8 },
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetLoopRegion
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetLoopRegion_1_0_5788, 7,

    XREF_DirectSound_CDirectSoundBuffer_SetLoopRegion,
    XRefZero)

        { 0x0F, 0x00 },
        { 0x24, 0xB8 },
        { 0x31, 0x55 },
        { 0x42, 0xC8 },
        { 0x53, 0x00 },
        { 0x64, 0x8B },
        { 0x77, 0xFF },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetLoopRegion
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetLoopRegion_1_0_5788, 9,

    XRefNoSaveIndex,
    XRefOne)

        { 0x19, XREF_DirectSound_CDirectSoundBuffer_SetLoopRegion },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0xFF },
        { 0x0E, 0x83 },
        { 0x12, 0xD9 },
        { 0x16, 0xC8 },
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
OOVPA_END;

// ******************************************************************
// * DirectSound::CMcpxBuffer::Stop
// ******************************************************************
OOVPA_XREF(DirectSound_CMcpxBuffer_Stop_1_0_5788, 8,

    XREF_DSOUNDSTOP,
    XRefZero)

        { 0x06, 0x56 },
        { 0x0E, 0xF1 },
        { 0x16, 0xFF },
        { 0x1D, 0x04 },
        { 0x1E, 0xFF },
        { 0x26, 0x75 },
        { 0x2F, 0x8B },
        { 0x36, 0xC2 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::StopEx
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_StopEx_1_0_5788, 8,

    XREF_DSOUNDSTOP2,
    XRefOne)

        { 0x3D, XREF_DSOUNDSTOP },

        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x20 },
        { 0x3A, 0x24 },
        { 0x46, 0x0B },
        { 0x52, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_StopEx
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_StopEx_1_0_5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x1D, XREF_DSOUNDSTOP2 },

        { 0x03, 0x10 },
        { 0x08, 0xFF },
        { 0x0D, 0xC8 },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSound::SetRolloffFactor
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_SetRolloffFactor_1_0_5788, 9,

    XREF_SETROLLOFFFACTORA,
    XRefZero)

        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x08 },
        { 0x35, 0x6C },
        { 0x3A, 0x48 },
        { 0x4A, 0x85 },
        { 0x53, 0xFF },
        { 0x5E, 0x00 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSound::SetDopplerFactor
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_SetDopplerFactor_1_0_5788, 9,

    XREF_SETDOPPLERFACTOR,
    XRefZero)

        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x08 },
        { 0x35, 0x70 },
        { 0x3A, 0x48 },
        { 0x4A, 0x85 },
        { 0x53, 0xFF },
        { 0x5E, 0x00 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetPitch
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetPitch_1_0_5788, 6,

    XREF_DSBUFFERSETPITCHB,
    XRefZero)

        { 0x02, 0x24 },
        { 0x06, 0x10 },
        { 0x0A, 0x08 },
        { 0x0E, 0x8B },
        { 0x16, 0xC2 },
        { 0x17, 0x08 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetPitch
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetPitch_1_0_5788, 8,

    XREF_DSBUFFERSETPITCHA,
    XRefOne)

        { 0x32, XREF_DSBUFFERSETPITCHB },

        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0x10 },
        { 0x3A, 0x74 },
        { 0x47, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetPitch
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetPitch_1_0_5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x15, XREF_DSBUFFERSETPITCHA },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetMaxDistance
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetMaxDistance_1_0_5788, 7,

    XREF_DSVOICESETMAXDISTANCE,
    XRefZero)

        { 0x05, 0x41 },
        { 0x0C, 0x00 },
        { 0x13, 0x3C },
        { 0x1A, 0x00 },
        { 0x21, 0xF6 },
        { 0x28, 0x51 },
        { 0x2F, 0xC0 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetMinDistance
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetMinDistance_1_0_5788, 7,

    XREF_DSVOICESETMINDISTANCE,
    XRefZero)

        { 0x05, 0x41 },
        { 0x0C, 0x00 },
        { 0x13, 0x38 },
        { 0x1A, 0x00 },
        { 0x21, 0xF6 },
        { 0x28, 0x51 },
        { 0x2F, 0xC0 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetMaxDistance
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetMaxDistance_1_0_5788, 8,

    XREF_DSBUFFERSETMAXDISTANCE,
    XRefOne)

        { 0x3A, XREF_DSVOICESETMAXDISTANCE },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xFF },
        { 0x40, 0x8B },
        { 0x4F, 0x8B },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetMinDistance
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetMinDistance_1_0_5788, 8,

    XREF_DSBUFFERSETMINDISTANCE,
    XRefOne)

        { 0x3A, XREF_DSVOICESETMINDISTANCE },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xFF },
        { 0x40, 0x8B },
        { 0x4F, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetMaxDistance
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMaxDistance_1_0_5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x1D, XREF_DSBUFFERSETMAXDISTANCE },

        { 0x03, 0x0C },
        { 0x08, 0x8B },
        { 0x0D, 0x8B },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetMinDistance
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMinDistance_1_0_5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x1D, XREF_DSBUFFERSETMINDISTANCE },

        { 0x03, 0x0C },
        { 0x08, 0x8B },
        { 0x0D, 0x8B },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetVolume
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetVolume_1_0_5788, 7,

    XREF_DirectSound_CDirectSoundVoice_SetVolume,
    XRefZero)

        { 0x02, 0x24 },
        { 0x06, 0x10 },
        { 0x0A, 0x08 },
        { 0x0E, 0x89 },
        { 0x12, 0x49 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetVolume
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetVolume_1_0_5788, 8,

    XREF_DirectSound_CDirectSoundBuffer_SetVolume,
    XRefOne)

        { 0x32, XREF_DirectSound_CDirectSoundVoice_SetVolume },

        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0x10 },
        { 0x3A, 0x74 },
        { 0x47, 0x8B },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetVolume
// ******************************************************************
// * Not until we need it!
//OOVPA_XREF(DirectSound_CDirectSoundStream_SetVolume_1_0_5788, 9,
//
//    XREF_DirectSound_CDirectSoundStream_SetVolume,
//    XRefOne)
//
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
//OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetVolume
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetVolume_1_0_5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x15, XREF_DirectSound_CDirectSoundBuffer_SetVolume },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::PlayEx
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_PlayEx_1_0_5788, 8,

    XREF_DirectSound_CDirectSoundBuffer_PlayEx,
    XRefOne)

        { 0x3D, XREF_DirectSound_CMcpxBuffer_Play },

        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x20 },
        { 0x3A, 0x24 },
        { 0x46, 0x0B },
        { 0x52, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_PlayEx
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_PlayEx_1_0_5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x1D, XREF_DirectSound_CDirectSoundBuffer_PlayEx },

        { 0x03, 0x10 },
        { 0x08, 0xFF },
        { 0x0D, 0xC8 },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetRolloffFactor
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetRolloffFactor_1_0_5788, 7,

    XREF_DSVOICESETROLLOFFFACTOR,
    XRefZero)

        { 0x05, 0x41 },
        { 0x0C, 0x00 },
        { 0x13, 0x48 },
        { 0x1A, 0x00 },
        { 0x21, 0xF6 },
        { 0x28, 0x51 },
        { 0x2F, 0xC0 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetRolloffFactor
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetRolloffFactor_1_0_5788, 8,

    XREF_DSBUFFERSETROLLOFFFACTOR,
    XRefOne)

        { 0x3A, XREF_DSVOICESETROLLOFFFACTOR },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xFF },
        { 0x40, 0x8B },
        { 0x4F, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetRolloffFactor
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetRolloffFactor_1_0_5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x1D, XREF_DSBUFFERSETROLLOFFFACTOR },

        { 0x03, 0x0C },
        { 0x08, 0x8B },
        { 0x0D, 0x8B },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetDopplerFactor
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetDopplerFactor_1_0_5788, 7,

    XREF_DSVOICESETDOPPLERFACTOR,
    XRefZero)

        { 0x05, 0x41 },
        { 0x0C, 0x00 },
        { 0x13, 0x4C },
        { 0x1A, 0x00 },
        { 0x21, 0xF6 },
        { 0x28, 0x51 },
        { 0x2F, 0xC0 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetDopplerFactor
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetDopplerFactor_1_0_5788, 8,

    XREF_DSBUFFERSETDOPPLERFACTOR,
    XRefOne)

        { 0x3A, XREF_DSVOICESETDOPPLERFACTOR },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xFF },
        { 0x40, 0x8B },
        { 0x4F, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetDopplerFactor
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetDopplerFactor_1_0_5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x1D, XREF_DSBUFFERSETDOPPLERFACTOR },

        { 0x03, 0x0C },
        { 0x08, 0x8B },
        { 0x0D, 0x8B },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetPosition
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetPosition_1_0_5788, 7,

    XREF_DSVOICESETPOSITION,
    XRefZero)

        { 0x09, 0x8B },
        { 0x14, 0x08 },
        { 0x1F, 0x55 },
        { 0x2A, 0x00 },
        { 0x35, 0x10 },
        { 0x40, 0xF6 },
        { 0x4C, 0x33 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetPosition
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetPosition_1_0_5788, 9,

    XREF_DSBUFFERSETPOSITION,
    XRefOne)

        { 0x4A, XREF_DSVOICESETPOSITION },

        { 0x0F, 0x00 },
        { 0x18, 0x0B },
        { 0x25, 0x05 },
        { 0x32, 0x83 },
        { 0x3F, 0x04 },
        { 0x4E, 0x85 },
        { 0x59, 0xFF },
        { 0x66, 0x00 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetPosition
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetPosition_1_0_5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x2D, XREF_DSBUFFERSETPOSITION },

        { 0x05, 0x18 },
        { 0x0C, 0x83 },
        { 0x13, 0x8B },
        { 0x1A, 0xE4 },
        { 0x21, 0xD9 },
        { 0x28, 0xD9 },
        { 0x31, 0x5D },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetVelocity
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetVelocity_1_0_5788, 7,

    XREF_DSVOICESETVELOCITY,
    XRefZero)

        { 0x09, 0x8B },
        { 0x14, 0x14 },
        { 0x1F, 0x55 },
        { 0x2A, 0x00 },
        { 0x35, 0x10 },
        { 0x40, 0xF6 },
        { 0x4C, 0x33 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetVelocity
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetVelocity_1_0_5788, 9,

    XREF_DSBUFFERSETVELOCITY,
    XRefOne)

        { 0x4A, XREF_DSVOICESETVELOCITY },

        { 0x0F, 0x00 },
        { 0x18, 0x0B },
        { 0x25, 0x05 },
        { 0x32, 0x83 },
        { 0x3F, 0x04 },
        { 0x4E, 0x85 },
        { 0x59, 0xFF },
        { 0x66, 0x00 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetVelocity
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetVelocity_1_0_5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x2D, XREF_DSBUFFERSETVELOCITY },

        { 0x05, 0x18 },
        { 0x0C, 0x83 },
        { 0x13, 0x8B },
        { 0x1A, 0xE4 },
        { 0x21, 0xD9 },
        { 0x28, 0xD9 },
        { 0x31, 0x5D },
OOVPA_END;

// ******************************************************************
// * DirectSound::CMcpxBuffer::Pause
// ******************************************************************
OOVPA_XREF(DirectSound_CMcpxBuffer_Pause_1_0_5788, 7,

    XREF_DirectSound_CMcpxBuffer_Pause,
    XRefZero)

        { 0x0D, 0x4D },
        { 0x1C, 0x44 },
        { 0x2B, 0x8B },
        { 0x3A, 0x75 },
        { 0x49, 0x7D },
        { 0x58, 0xEB },
        { 0x67, 0xE8 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::Pause
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_Pause_1_0_5788, 8,

    XREF_DirectSound_CDirectSoundBuffer_Pause,
    XRefOne)

        { 0x35, XREF_DirectSound_CMcpxBuffer_Pause },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x39, 0x85 },
        { 0x44, 0xFF },
        { 0x4B, 0xC7 },
OOVPA_END;
// ******************************************************************
// * IDirectSoundBuffer_Pause
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_Pause_1_0_5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x15, XREF_DirectSound_CDirectSoundBuffer_Pause },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSound::CreateSoundStream
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_CreateSoundStream_1_0_5788, 7,

    XREF_DSCREATESOUNDSTREAM,
    XRefZero)

        { 0x11, 0xD8 },
        { 0x24, 0x05 },
        { 0x37, 0x74 },
        { 0x4A, 0x81 },
        { 0x5D, 0xE8 },
        { 0x70, 0x85 },
        { 0x83, 0xFF },
OOVPA_END;

// ******************************************************************
// * IDirectSound_CreateSoundStream
// ******************************************************************
//OOVPA_XREF(IDirectSound_CreateSoundStream_1_0_5788, 8,
//
//    XRefNoSaveIndex,
//    XRefOne)
//
//        { 0x1D, XREF_DirectSound_CDirectSound_CreateSoundStream },
//
//        { 0x03, 0x10 },
//        { 0x08, 0xFF },
//        { 0x0D, 0xC8 },
//        { 0x12, 0x83 },
//        { 0x17, 0x1B },
//        { 0x1C, 0xE8 },
//        { 0x21, 0xC2 },
//OOVPA_END;

// ******************************************************************
// * DirectSoundCreateStream
// ******************************************************************
OOVPA_XREF(DirectSoundCreateStream_1_0_5788, 9,

    XRefNoSaveIndex,
    XRefOne)

        { 0x2F, XREF_DSCREATESOUNDSTREAM },

        { 0x09, 0x56 },
        { 0x14, 0x45 },
        { 0x1F, 0x8B },
        { 0x2A, 0xFF },
        { 0x35, 0x85 },
        { 0x40, 0xFF },
        { 0x4E, 0x5F },
        { 0x56, 0x00 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetMixBinVolumes
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundStream_SetMixBinVolumes_1_0_5788, 9,

    XRefNoSaveIndex,
    XRefOne)

        { 0x36, XREF_DSSETMIXBINVOLUMESB },

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
OOVPA_END;

// ******************************************************************
// * DirectSound::CMcpxStream::Pause
// ******************************************************************
OOVPA_XREF(DirectSound_CMcpxStream_Pause_1_0_5788, 7,

    XREF_DSSTREAMPAUSE,
    XRefZero)

        { 0x10, 0x07 },
        { 0x23, 0x0F },
        { 0x34, 0x83 },
        { 0x46, 0xF8 },
        { 0x58, 0x03 },
        { 0x6A, 0xBB },
        { 0x7C, 0x00 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundStream::Pause
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundStream_Pause_1_0_5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x35, XREF_DSSTREAMPAUSE },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x39, 0x85 },
        { 0x44, 0xFF },
        { 0x4B, 0xC7 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundStream::FlushEx
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundStream_FlushEx_1_0_5788, 7,

    XREF_DSFLUSHEX2,
    XRefZero)

        { 0x0F, 0x00 },
        { 0x1E, 0xFF },
        { 0x28, 0x80 },
        { 0x36, 0x74 },
        { 0x46, 0x8B },
        { 0x52, 0xFF },
        { 0x62, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundStream_FlushEx
// ******************************************************************
OOVPA_XREF(IDirectSoundStream_FlushEx_1_0_5788, 9,

    XRefNoSaveIndex,
    XRefOne)

        { 0x11, XREF_DSFLUSHEX2 },

        { 0x01, 0x74 },
        { 0x04, 0xFF },
        { 0x07, 0x10 },
        { 0x0A, 0x24 },
        { 0x0D, 0x74 },
        { 0x10, 0xE8 },
        { 0x15, 0xC2 },
        { 0x16, 0x10 },
OOVPA_END;

// ******************************************************************
// * DirectSoundDoWork
// ******************************************************************
OOVPA_NO_XREF(DirectSoundDoWork_1_0_5788, 7)

        { 0x06, 0x0F },
        { 0x0E, 0x85 },
        { 0x10, 0x74 },
        { 0x18, 0x85 },
        { 0x1C, 0x0B },
        { 0x22, 0xFF },
        { 0x28, 0xC3 },
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetEffectData
// ******************************************************************
// * No XRefs! Woohoo!
// ******************************************************************
OOVPA_NO_XREF(IDirectSound_SetEffectData_1_0_5788, 7)

        { 0x04, 0x75 },
        { 0x0A, 0x75 },
        { 0x10, 0x14 },
        { 0x16, 0x10 },
        { 0x1C, 0x1B },
        { 0x26, 0x5D },
        { 0x28, 0x18 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CMemoryManager::PoolAlloc
// ******************************************************************
OOVPA_NO_XREF(DirectSound_CMemoryManager_PoolAlloc_1_0_5788, 10)

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
OOVPA_END;

// ******************************************************************
// * XFileCreateMediaObjectAsync
// ******************************************************************
OOVPA_NO_XREF(XFileCreateMediaObjectAsync_1_0_5788, 8)

        { 0x0A, 0xC0 },
        { 0x16, 0xF8 },
        { 0x22, 0xE6 },
        { 0x2E, 0x20 },
        { 0x3D, 0x8B },
        { 0x49, 0x8B },
        { 0x52, 0x06 },
        { 0x5E, 0x0C },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetPitch
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundStream_SetPitch_1_0_5788, 12,

    XRefNoSaveIndex,
    XRefOne)

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
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetHeadroom
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundStream_SetHeadroom_1_0_5788, 9,

    XRefNoSaveIndex,
    XRefOne)

        { 0x36, XREF_DirectSound_CDirectSoundVoice_SetHeadroom },

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
OOVPA_END;

// ******************************************************************
// * Direct_1_0_5788
// ******************************************************************
OOVPATable DSound_1_0_5788[] = {

    // DirectSoundCreate (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(DirectSoundCreate_1_0_4361, DirectSoundCreate),
    // CDirectSound_CreateSoundBuffer
	OOVPA_TABLE_XREF(CDirectSound_CreateSoundBuffer_1_0_4627),
	// IDirectSoundBuffer8::SetPlayRegion (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirectSoundBuffer8_SetPlayRegion_1_0_4361, IDirectSoundBuffer8_SetPlayRegion),
    // CMcpxBuffer::SetBufferData
	OOVPA_TABLE_XREF(CMcpxBuffer_SetBufferData_1_0_5788),
    // CDirectSoundBuffer::SetBufferData (* unchanged since 4361 *)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetBufferData_1_0_4361),
	// IDirectSoundBuffer8::SetBufferData (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirectSoundBuffer8_SetBufferData_1_0_4361, IDirectSoundBuffer8_SetBufferData),
    // CMcpxBuffer::GetStatus
	OOVPA_TABLE_XREF(CMcpxBuffer_GetStatus_1_0_5788),
    // CDirectSoundBuffer::GetStatus
	OOVPA_TABLE_XREF(CDirectSoundBuffer_GetStatus_1_0_4627),
	// IDirectSoundBuffer8::GetStatus
	OOVPA_TABLE_PATCH(IDirectSoundBuffer8_GetStatus_1_0_4627, IDirectSoundBuffer8_GetStatus),
    // CDirectSound_SetI3DL2Listener
	OOVPA_TABLE_XREF(CDirectSound_SetI3DL2Listener_1_0_5788),
	// IDirectSound8_SetI3DL2Listener
	OOVPA_TABLE_PATCH(IDirectSound8_SetI3DL2Listener_1_0_5788, IDirectSound8_SetI3DL2Listener),
    // DirectSound::CDirectSoundVoice::SetFormat (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundVoice_SetFormat_1_0_5788),

    // DirectSound::CDirectSoundBuffer::SetFormat (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_SetFormat_1_0_5788),
	// IDirectSoundBuffer8_SetFormat
	OOVPA_TABLE_PATCH(IDirectSoundBuffer8_SetFormat_1_0_5788, IDirectSoundBuffer8_SetFormat),
    // CDirectSoundVoice::SetConeAngles (XREF)
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetConeAngles_1_0_5558),
    // CDirectSoundBuffer::SetConeAngles (XREF)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetConeAngles_1_0_5558),
	// IDirectSoundBuffer_SetConeAngles
	OOVPA_TABLE_PATCH(IDirectSoundBuffer8_SetConeAngles_1_0_5558, IDirectSoundBuffer8_SetConeAngles),
    // CDirectSoundVoiceSettings_SetMixBinVolumes
	OOVPA_TABLE_XREF(CDirectSoundVoiceSettings_SetMixBinVolumes_1_0_4627),
    // CDirectSoundVoice_SetMixBinVolumes
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetMixBinVolumes_1_0_4627),
    // CDirectSoundBuffer_SetMixBinVolumes
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetMixBinVolumes_1_0_4627),
	// IDirectSoundBuffer8_SetMixBinVolumes
	OOVPA_TABLE_PATCH(IDirectSoundBuffer8_SetMixBinVolumes_1_0_4627, IDirectSoundBuffer8_SetMixBinVolumes),
	// CDirectSoundStream_SetMixBinVolumes
	OOVPA_TABLE_PATCH(DirectSound_CDirectSoundStream_SetMixBinVolumes_1_0_5788, CDirectSoundStream_SetMixBinVolumes2),
    // CDirectSoundVoice::SetRolloffCurve (XREF) (* unchanged since 5233 *)
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetRolloffCurve_1_0_5233),
    // CDirectSoundBuffer::SetRolloffCurve (XREF) (* unchanged since 5233 *)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetRolloffCurve_1_0_5233),
	// CDirectSoundStream::SetRolloffCurve
	OOVPA_TABLE_PATCH(DirectSound_CDirectSoundStream_SetRolloffCurve_1_0_5558, CDirectSoundStream_SetRolloffCurve),
	// IDirectSoundBuffer8::SetRolloffCurve (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(IDirectSoundBuffer8_SetRolloffCurve_1_0_5233, IDirectSoundBuffer8_SetRolloffCurve),
	// IDirectSound8::CreateSoundBuffer
	OOVPA_TABLE_PATCH(IDirectSound8_CreateSoundBuffer_1_0_4627, IDirectSound8_CreateSoundBuffer),
    // CDirectSoundVoice::SetFrequency (XREF)
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetFrequency_1_0_4627),
    // CDirectSoundBuffer::SetFrequency
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetFrequency_1_0_4627),
	// IDirectSoundBuffer8::SetFrequency
	OOVPA_TABLE_PATCH(IDirectSoundBuffer8_SetFrequency_1_0_4627, IDirectSoundBuffer8_SetFrequency),
	// CDirectSoundStream_SetVolume
	OOVPA_TABLE_PATCH(CDirectSoundStream_SetVolume_1_0_4627, CDirectSoundStream_SetVolume),
	// IDirectSound8::Release (* unchanged since 3936 *)
	OOVPA_TABLE_PATCH(IDirectSound8_Release_1_0_3936, IDirectSound8_Release),
	// IDirectSound8::DownloadEffectsImage (* unchanged since 3936 *)
	OOVPA_TABLE_PATCH(IDirectSound8_DownloadEffectsImage_1_0_3936, IDirectSound8_DownloadEffectsImage),
	// IDirectSound8::SetOrientation (* unchanged since 3936 *)
	OOVPA_TABLE_PATCH(IDirectSound8_SetOrientation_1_0_3936, IDirectSound8_SetOrientation),
    // CDirectSoundVoice::SetMaxDistance (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundVoice_SetMaxDistance_1_0_5788),
    // CDirectSoundBuffer::SetMaxDistance (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_SetMaxDistance_1_0_5788),
	// IDirectSoundBuffer_SetMaxDistance
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetMaxDistance_1_0_5788, IDirectSoundBuffer8_SetMaxDistance),
	// CDirectSoundVoice::SetMinDistance (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundVoice_SetMinDistance_1_0_5788),
    // CDirectSoundBuffer::SetMinDistance (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_SetMinDistance_1_0_5788),
	// IDirectSoundBuffer_SetMinDistance
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetMinDistance_1_0_5788, IDirectSoundBuffer8_SetMinDistance),
    // CMcpxBuffer::Play
	OOVPA_TABLE_XREF(DirectSound_CMcpxBuffer_Play_1_0_5788),
	// CMcpxBuffer::Play
    /*
	OOVPA_TABLE_XREF(CMcpxBuffer_Play_1_0_5788),
	*/
    // IDirectSoundBuffer_Stop (Possibly weak, but quite OK for 4627 DSOUND)
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_Stop_1_0_4627, IDirectSoundBuffer8_Stop),
	// IDirectSoundBuffer8::Release (* unchanged since 3936 *)
	OOVPA_TABLE_PATCH(IDirectSoundBuffer8_Release_1_0_3936, IDirectSoundBuffer8_Release),
    // DirectSound::CFullHRTFSource::GetCenterVolume (XREF)
	OOVPA_TABLE_XREF(DirectSound_CFullHRTFSource_GetCenterVolume_1_0_5558),
    // DirectSound::CHRTFSource::SetFullHRTF5Channel (XREF)
	OOVPA_TABLE_XREF(DirectSound_CHRTFSource_SetFullHRTF5Channel_1_0_5558),
	// DirectSoundUseFullHRTF
	OOVPA_TABLE_PATCH(DirectSoundUseFullHRTF_1_0_5558, DirectSoundUseFullHRTF),
	// DirectSound::CDirectSoundVoice::SetHeadroom (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundVoice_SetHeadroom_1_0_5788),
	// DirectSound::CDirectSoundBuffer::SetHeadroom (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_SetHeadroom_1_0_5788),
	// IDirectSoundBuffer_SetHeadroom
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetHeadroom_1_0_5788, IDirectSoundBuffer8_SetHeadroom),
	// CDirectSoundVoice::SetAllParameters (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundVoice_SetAllParameters_1_0_5558),
    // CDirectSoundBuffer::SetAllParameters (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_SetAllParameters_1_0_5558),
	// IDirectSoundBuffer_SetAllParameters
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetAllParameters_1_0_5558, IDirectSoundBuffer8_SetAllParameters),
	// IDirectSoundBuffer_Lock
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_Lock_1_0_5788, IDirectSoundBuffer8_Lock),
	// CDirectSoundVoice::SetI3DL2Source (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundVoice_SetI3DL2Source_1_0_5558),
    // CDirectSoundBuffer::SetI3DL2Source (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_SetI3DL2Source_1_0_5558),
	// IDirectSoundBuffer_SetI3DL2Source
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetI3DL2Source_1_0_5558, IDirectSoundBuffer8_SetI3DL2Source),
	// DirectSound::CDirectSoundVoiceSettings::SetMixBins (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundVoiceSettings_SetMixBins_1_0_5788),
	// DirectSound::CDirectSoundVoice::SetMixBins (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundVoice_SetMixBins_1_0_5788),
	// DirectSound::CDirectSoundBuffer::SetMixBins (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_SetMixBins_1_0_5788),
	// IDirectSoundBuffer_SetMixBins
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetMixBins_1_0_5788, IDirectSoundBuffer8_SetMixBins),
	// DirectSound::CDirectSoundStream::SetMixBins (XREF)
	OOVPA_TABLE_PATCH(DirectSound_CDirectSoundStream_SetMixBins_1_0_5788, CDirectSoundStream_SetMixBins),
	// CDirectSound_SetMixBinHeadroom
	OOVPA_TABLE_XREF(DirectSound_CDirectSound_SetMixBinHeadroom_1_0_5788),
	// IDirectSound8_SetMixBinHeadroom
	OOVPA_TABLE_PATCH(IDirectSound_SetMixBinHeadroom_1_0_5788, IDirectSound8_SetMixBinHeadroom),
	// CDirectSound::SetPosition (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSound_SetPosition_1_0_5788),
	// IDirectSound8::SetPosition
	OOVPA_TABLE_PATCH(IDirectSound_SetPosition_1_0_5788, IDirectSound8_SetPosition),
	// CDirectSound::SetVelocity (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSound_SetVelocity_1_0_5788),
	// IDirectSound8::SetVelocity
	OOVPA_TABLE_PATCH(IDirectSound_SetVelocity_1_0_5788, IDirectSound8_SetVelocity),
    // CDirectSoundVoice::SetDistanceFactor (XREF)
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetDistanceFactor_1_0_5558),
    // CDirectSoundBuffer::SetDistanceFactor (XREF)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetDistanceFactor_1_0_5558),
	// IDirectSoundBuffer_SetDistanceFactor
	OOVPA_TABLE_PATCH(IDirectSoundBuffer8_SetDistanceFactor_1_0_5558, IDirectSoundBuffer8_SetDistanceFactor),
	// CDirectSound::CommitDeferredSettings
	OOVPA_TABLE_PATCH(DirectSound_CDirectSound_CommitDeferredSettings_1_0_5788, CDirectSound_CommitDeferredSettings),
	// DirectSoundCreateBuffer
	OOVPA_TABLE_PATCH(DirectSoundCreateBuffer_1_0_4627, DirectSoundCreateBuffer),
	// CMcpxBuffer::SetCurrentPosition
	OOVPA_TABLE_XREF(DirectSound_CMcpxBuffer_SetCurrentPosition_1_0_5788),
    // CDirectSoundBuffer::SetCurrentPosition
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_SetCurrentPosition_1_0_5788),
	// IDirectSoundBuffer8::SetCurrentPosition
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetCurrentPosition_1_0_5788, IDirectSoundBuffer8_SetCurrentPosition),
    // DirectSound::CMcpxVoiceClient::SetLFO (XREF)
	OOVPA_TABLE_XREF(CMcpxVoiceClient_SetLFO_1_0_5558),
    // CDirectSoundVoice::SetLFO (XREF)
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetLFO_1_0_5558),
    // CDirectSoundBuffer::SetLFO (XREF)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetLFO_1_0_5558),
	// IDirectSoundBuffer8::SetLFO
	OOVPA_TABLE_PATCH(IDirectSoundBuffer8_SetLFO_1_0_5558, IDirectSoundBuffer8_SetLFO),
	// CDirectSoundBuffer::GetCurrentPosition
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_GetCurrentPosition_1_0_5788),
	// IDirectSoundBuffer8::GetCurrentPosition
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_GetCurrentPosition_1_0_5788, IDirectSoundBuffer8_GetCurrentPosition),
    // CDirectSoundBuffer::SetPlayRegion
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetPlayRegion_1_0_5558),
	// IDirectSoundBuffer8::SetPlayRegion
	OOVPA_TABLE_PATCH(IDirectSoundBuffer8_SetPlayRegion_1_0_5558, IDirectSoundBuffer8_SetPlayRegion),
	// CDirectSoundBuffer::SetLoopRegion (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_SetLoopRegion_1_0_5788),
	// IDirectSoundBuffer8::SetLoopRegion
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetLoopRegion_1_0_5788, IDirectSoundBuffer8_SetLoopRegion),
	// CMcpxBuffer_Stop
	OOVPA_TABLE_XREF(DirectSound_CMcpxBuffer_Stop_1_0_5788),
	// CDirectSoundBuffer_Stop
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_StopEx_1_0_5788),
	// CDirectSoundBuffer_Stop
	OOVPA_TABLE_PATCH(DirectSound_CDirectSoundBuffer_StopEx_1_0_5788, IDirectSoundBuffer8_StopEx),
    // CDirectSound::SetDistanceFactorA (XREF)
	OOVPA_TABLE_XREF(CDirectSound_SetDistanceFactorA_1_0_5558),
	// IDirectSound8::SetDistanceFactor (* unchanged since 4432 *)
	OOVPA_TABLE_PATCH(IDirectSound8_SetDistanceFactor_1_0_4432, IDirectSound8_SetDistanceFactor),
	// CDirectSound::SetRolloffFactor (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSound_SetRolloffFactor_1_0_5788),
	// IDirectSound8::SetRolloffFactor (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirectSound8_SetRolloffFactor_1_0_4627, IDirectSound8_SetRolloffFactor),
	// CDirectSound::SetDopplerFactor (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSound_SetDopplerFactor_1_0_5788),
	// IDirectSound8::SetDopplerFactor (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirectSound8_SetDopplerFactor_1_0_4627, IDirectSound8_SetDopplerFactor),
	// CDirectSoundVoice::SetPitch
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundVoice_SetPitch_1_0_5788),
	// IDirectSoundBuffer8::SetPitch
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_SetPitch_1_0_5788),
	// IDirectSoundBuffer8::SetPitch
	OOVPA_TABLE_PATCH(IDirectSoundBuffer8_SetPitch_1_0_5788, IDirectSoundBuffer8_SetPitch),
    // CMcpxVoiceClient_SetVolume
	OOVPA_TABLE_XREF(CMcpxVoiceClient_SetVolume_1_0_5558),
   // CDirectSoundVoice::SetVolume (XREF) (* unchanged since 5233 *)
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetVolume_1_0_5233),
    // CDirectSoundBuffer::SetVolume (XREF) (* unchanged since 5233 *)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetVolume_1_0_5233),
	// IDirectSoundBuffer8::SetVolume (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(IDirectSoundBuffer8_SetVolume_1_0_5233, IDirectSoundBuffer8_SetVolume),
    // IDirectSoundStream::SetVolume (* unchanged since 5233 *)
    // (This is actually the XREF we are using)
	OOVPA_TABLE_PATCH(CDirectSoundStream_SetVolume_1_0_5233, IDirectSoundStream_SetVolume),
	// DirectSound::CDirectSoundBuffer::PlayEx (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_PlayEx_1_0_5788),
	// IDirectSoundBuffer_PlayEx
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_PlayEx_1_0_5788, IDirectSoundBuffer8_PlayEx),
	// CDirectSoundVoice::SetRolloffFactor (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundVoice_SetRolloffFactor_1_0_5788),
    // CDirectSoundBuffer::SetRolloffFactor (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_SetRolloffFactor_1_0_5788),
	// IDirectSoundBuffer_SetRolloffFactor
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetRolloffFactor_1_0_5788, IDirectSoundBuffer8_SetRolloffFactor),
    // CDirectSoundVoice::SetFrequency (XREF) (* unchanged since 4627 *)
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetFrequency_1_0_4627),
    // CDirectSoundBuffer::SetFrequency (* unchanged since 4627 *)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetFrequency_1_0_4627),
	// CDirectSoundStream::SetFrequency
	OOVPA_TABLE_PATCH(DirectSound_CDirectSoundStream_SetFrequency_1_0_5558, CDirectSoundStream_SetFrequency),
	// CDirectSoundVoice::SetDopplerFactor (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundVoice_SetDopplerFactor_1_0_5788),
    // CDirectSoundBuffer::SetDopplerFactor (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_SetDopplerFactor_1_0_5788),
	// IDirectSoundBuffer8_SetDopplerFactor
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetDopplerFactor_1_0_5788, IDirectSoundBuffer8_SetDopplerFactor),
	// CDirectSoundVoice::SetPosition (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundVoice_SetPosition_1_0_5788),
    // CDirectSoundBuffer::SetPosition (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_SetPosition_1_0_5788),
	// IDirectSoundBuffer8_SetPosition
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetPosition_1_0_5788, IDirectSoundBuffer8_SetPosition),
	// CDirectSoundVoice::SetVelocity (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundVoice_SetVelocity_1_0_5788),
    // CDirectSoundBuffer::SetVelocity (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_SetVelocity_1_0_5788),
	// IDirectSoundBuffer8_SetVelocity
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetVelocity_1_0_5788, IDirectSoundBuffer8_SetVelocity),
	// DirectSound_CMcpxBuffer_Pause_1_0_5788
	OOVPA_TABLE_XREF(DirectSound_CMcpxBuffer_Pause_1_0_5788),
	// CDirectSoundBuffer::Pause
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_Pause_1_0_5788),
	// IDirectSoundBuffer8_Pause
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_Pause_1_0_5788, IDirectSoundBuffer8_Pause),
	// CDirectSound_CreateSoundStream
	OOVPA_TABLE_PATCH(DirectSound_CDirectSound_CreateSoundStream_1_0_5788, DirectSoundCreateStream),
	// DirectSoundCreateStream
	OOVPA_TABLE_PATCH(DirectSoundCreateStream_1_0_5788, DirectSoundCreateStream),
	// CMcpxStream::Pause
	OOVPA_TABLE_XREF(DirectSound_CMcpxStream_Pause_1_0_5788),
	// CDirectSoundStream::Pause
	OOVPA_TABLE_PATCH(DirectSound_CDirectSoundStream_Pause_1_0_5788, CDirectSoundStream_Pause),
	// CDirectSoundStream::FlushEx (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundStream_FlushEx_1_0_5788),
	// IDirectSoundStream_FlushEx
	OOVPA_TABLE_PATCH(IDirectSoundStream_FlushEx_1_0_5788, IDirectSoundStream_FlushEx),
	// DirectSoundDoWork
	OOVPA_TABLE_PATCH(DirectSoundDoWork_1_0_5788, DirectSoundDoWork),
	// CDirectSound::SynchPlayback (* unchanged since 5558 *)
	OOVPA_TABLE_PATCH(CDirectSound_SynchPlayback_1_0_5558, CDirectSound_SynchPlayback),
	// DirectSound::CDirectSound::EnableHeadphones (XRef)
	OOVPA_TABLE_XREF(DirectSound_CDirectSound_EnableHeadphones_1_0_5558),
	// IDirectSound8::EnableHeadphones
	OOVPA_TABLE_PATCH(IDirectSound_EnableHeadphones_1_0_5558, IDirectSound8_EnableHeadphones),
	// XAudioDownloadEffectsImage (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(XAudioDownloadEffectsImage_1_0_4627, XAudioDownloadEffectsImage),
	// DirectSound::CDirectSound::GetOutputLevels (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSound_GetOutputLevels_1_0_5558),
	// DirectSound::CDirectSound::GetOutputLevels (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSound_GetOutputLevels_1_0_5558),
	// IDirectSound_GetOutputLevels
	OOVPA_TABLE_PATCH(IDirectSound_GetOutputLevels_1_0_5558, IDirectSound8_GetOutputLevels),
	// IDirectSoundStream_FlushEx
	OOVPA_TABLE_PATCH(IDirectSound_SetEffectData_1_0_5788, IDirectSound8_SetEffectData),
	// DirectSound::CMemoryManager::PoolAlloc
	OOVPA_TABLE_XREF(DirectSound_CMemoryManager_PoolAlloc_1_0_5788),
	// XFileCreateMediaObjectAsync
	OOVPA_TABLE_PATCH(XFileCreateMediaObjectAsync_1_0_5788, XFileCreateMediaObjectAsync),
    // WaveFormat::CreateXboxAdpcmFormat (XREF)
	OOVPA_TABLE_XREF(WaveFormat_CreateXboxAdpcmFormat_1_0_5558),
    // XAudioCreateAdpcmFormat
	OOVPA_TABLE_PATCH(XAudioCreateAdpcmFormat_1_0_5558, XAudioCreateAdpcmFormat),
	// CDirectSoundStream::SetFormat (* unchanged since 5558 *)
	OOVPA_TABLE_PATCH(DirectSound_CDirectSoundStream_SetFormat_1_0_5558, CDirectSoundStream_SetFormat),
	// CDirectSoundStream::SetPitch
	OOVPA_TABLE_PATCH(DirectSound_CDirectSoundStream_SetPitch_1_0_5788, CDirectSoundStream_SetPitch),
	// DirectSound::CDirectSoundStream::SetHeadroom
	OOVPA_TABLE_PATCH(DirectSound_CDirectSoundStream_SetHeadroom_1_0_5788, IDirectSoundStream_SetHeadroom),
	// CDirectSound_CreateSoundStream
	OOVPA_TABLE_XREF(DirectSound_CDirectSound_CreateSoundStream_1_0_5558),
	// IDirectSound_CreateSoundStream
	OOVPA_TABLE_PATCH(IDirectSound_CreateSoundStream_1_0_5558, IDirectSound8_CreateStream),
	// CMcpxBuffer::Play2
	OOVPA_TABLE_XREF(DirectSound_CMcpxBuffer_Play2_1_0_5788),
	// CDirectSoundBuffer::Play
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_Play_1_0_5788),
	// IDirectSoundBuffer8::Play
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_Play_1_0_5788, IDirectSoundBuffer8_Play),
	// CMcpxVoiceClient::SetEG
	OOVPA_TABLE_XREF(DirectSound_CMcpxVoiceClient_SetEG_1_0_4627),
	// CDirectSoundVoice::SetEG (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundVoice_SetEG_1_0_4627),
	// DirectSound::CDirectSoundStream::SetEG
	OOVPA_TABLE_PATCH(DirectSound_CDirectSoundStream_SetEG_1_0_4627, CDirectSoundStream_SetEG),
	// DirectSound::CDirectSoundBuffer::SetEG (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_SetEG_1_0_4627),
	// IDirectSoundBuffer::SetEG
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetEG_1_0_4627, IDirectSoundBuffer8_SetEG),
};

// ******************************************************************
// * DSound_1_0_5788_SIZE
// ******************************************************************
uint32 DSound_1_0_5788_SIZE = sizeof(DSound_1_0_5788);
