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
OOVPA_XREF(CMcpxBuffer_GetStatus, 5788, 11,

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
OOVPA_XREF(CDirectSound_SetI3DL2Listener, 5788, 8,

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
OOVPA_XREF(IDirectSound8_SetI3DL2Listener, 5788, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_DSSETI3DL2LISTENER ),

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
OOVPA_XREF(CMcpxBuffer_SetBufferData, 5788, 9,

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
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetFormat, 5788, 7,

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
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetFormat, 5788, 8,

    XREF_DirectSound_CDirectSoundBuffer_SetFormat,
    XRefOne)

        XREF_ENTRY( 0x32, XREF_DirectSound_CDirectSoundVoice_SetFormat ),

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
OOVPA_XREF(IDirectSoundBuffer8_SetFormat, 5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_DirectSound_CDirectSoundBuffer_SetFormat ),

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
OOVPA_XREF(CMcpxBuffer_Play, 5788, 11,

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
OOVPA_XREF(DirectSound_CMcpxBuffer_Play, 5788, 8,

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
OOVPA_XREF(DirectSound_CMcpxBuffer_Play2, 5788, 7,

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
OOVPA_XREF(DirectSound_CDirectSoundBuffer_Play, 5788, 9,

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
OOVPA_XREF(IDirectSoundBuffer_Play, 5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_DSOUNDPLAY2 ),

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
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetHeadroom, 5788, 7,

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
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetHeadroom, 5788, 8,

    XREF_DirectSound_CDirectSoundBuffer_SetHeadroom,
    XRefOne)

        XREF_ENTRY( 0x32, XREF_DirectSound_CDirectSoundVoice_SetHeadroom ),

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
OOVPA_XREF(IDirectSoundBuffer_SetHeadroom, 5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_DirectSound_CDirectSoundBuffer_SetHeadroom ),

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
OOVPA_NO_XREF(IDirectSoundBuffer_Lock, 5788, 8)

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
OOVPA_XREF(DirectSound_CDirectSoundVoiceSettings_SetMixBins, 5788, 7,

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
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetMixBins, 5788, 8,

    XREF_DSSETMIXBINSB,
    XRefOne)

        XREF_ENTRY( 0x0D, XREF_DSSETMIXBINSC ),

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
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetMixBins, 5788, 8,

    XREF_DSSETMIXBINSA,
    XRefOne)

        XREF_ENTRY( 0x32, XREF_DSSETMIXBINSB ),

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
OOVPA_XREF(DirectSound_CDirectSoundStream_SetMixBins, 5788, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_DSSETMIXBINSB ),

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
OOVPA_XREF(IDirectSoundBuffer_SetMixBins, 5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_DSSETMIXBINSA ),

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
OOVPA_XREF(DirectSound_CDirectSound_SetMixBinHeadroom, 5788, 8,

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
OOVPA_XREF(IDirectSound_SetMixBinHeadroom, 5788, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_DSSETMIXBINHEADROOMA ),

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
OOVPA_XREF(DirectSound_CDirectSound_SetPosition, 5788, 7,

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
OOVPA_XREF(IDirectSound_SetPosition, 5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x2D, XREF_DSSETPOSITIONA ),

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
OOVPA_XREF(DirectSound_CDirectSound_SetVelocity, 5788, 7,

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
OOVPA_XREF(IDirectSound_SetVelocity, 5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x2D, XREF_DSSETVELOCITYA ),

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
OOVPA_XREF(DirectSound_CDirectSound_CommitDeferredSettings, 5788, 7,

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
OOVPA_XREF(DirectSound_CMcpxBuffer_SetCurrentPosition, 5788, 7,

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
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetCurrentPosition, 5788, 8,

    XREF_DSBUFFERSETCURRENTPOSITIONA,
    XRefOne)

        XREF_ENTRY( 0x35, XREF_DSBUFFERSETCURRENTPOSITIONB ),

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
OOVPA_XREF(IDirectSoundBuffer_SetCurrentPosition, 5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_DSBUFFERSETCURRENTPOSITIONA ),

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
OOVPA_XREF(DirectSound_CDirectSoundBuffer_GetCurrentPosition, 5788, 7,

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
OOVPA_XREF(IDirectSoundBuffer_GetCurrentPosition, 5788, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_GETCURRENTPOSITION ),

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
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetLoopRegion, 5788, 7,

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
OOVPA_XREF(IDirectSoundBuffer_SetLoopRegion, 5788, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_DirectSound_CDirectSoundBuffer_SetLoopRegion ),

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
OOVPA_XREF(DirectSound_CMcpxBuffer_Stop, 5788, 8,

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
OOVPA_XREF(DirectSound_CDirectSoundBuffer_StopEx, 5788, 8,

    XREF_DSOUNDSTOP2,
    XRefOne)

        XREF_ENTRY( 0x3D, XREF_DSOUNDSTOP ),

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
OOVPA_XREF(IDirectSoundBuffer_StopEx, 5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_DSOUNDSTOP2 ),

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
OOVPA_XREF(DirectSound_CDirectSound_SetRolloffFactor, 5788, 9,

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
OOVPA_XREF(DirectSound_CDirectSound_SetDopplerFactor, 5788, 9,

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
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetPitch, 5788, 6,

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
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetPitch, 5788, 8,

    XREF_DSBUFFERSETPITCHA,
    XRefOne)

        XREF_ENTRY( 0x32, XREF_DSBUFFERSETPITCHB ),

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
OOVPA_XREF(IDirectSoundBuffer8_SetPitch, 5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_DSBUFFERSETPITCHA ),

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
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetMaxDistance, 5788, 7,

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
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetMinDistance, 5788, 7,

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
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetMaxDistance, 5788, 8,

    XREF_DSBUFFERSETMAXDISTANCE,
    XRefOne)

        XREF_ENTRY( 0x3A, XREF_DSVOICESETMAXDISTANCE ),

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
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetMinDistance, 5788, 8,

    XREF_DSBUFFERSETMINDISTANCE,
    XRefOne)

        XREF_ENTRY( 0x3A, XREF_DSVOICESETMINDISTANCE ),

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
OOVPA_XREF(IDirectSoundBuffer_SetMaxDistance, 5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_DSBUFFERSETMAXDISTANCE ),

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
OOVPA_XREF(IDirectSoundBuffer_SetMinDistance, 5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_DSBUFFERSETMINDISTANCE ),

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
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetVolume, 5788, 7,

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
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetVolume, 5788, 8,

    XREF_DirectSound_CDirectSoundBuffer_SetVolume,
    XRefOne)

        XREF_ENTRY( 0x32, XREF_DirectSound_CDirectSoundVoice_SetVolume ),

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
//OOVPA_XREF(DirectSound_CDirectSoundStream_SetVolume, 5788, 9,
//
//    XREF_DirectSound_CDirectSoundStream_SetVolume,
//    XRefOne)
//
//        XREF_ENTRY( 0x36, XREF_DirectSound_CDirectSoundVoice_SetVolume ),
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
OOVPA_XREF(IDirectSoundBuffer_SetVolume, 5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_DirectSound_CDirectSoundBuffer_SetVolume ),

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
OOVPA_XREF(DirectSound_CDirectSoundBuffer_PlayEx, 5788, 8,

    XREF_DirectSound_CDirectSoundBuffer_PlayEx,
    XRefOne)

        XREF_ENTRY( 0x3D, XREF_DirectSound_CMcpxBuffer_Play ),

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
OOVPA_XREF(IDirectSoundBuffer_PlayEx, 5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_DirectSound_CDirectSoundBuffer_PlayEx ),

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
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetRolloffFactor, 5788, 7,

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
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetRolloffFactor, 5788, 8,

    XREF_DSBUFFERSETROLLOFFFACTOR,
    XRefOne)

        XREF_ENTRY( 0x3A, XREF_DSVOICESETROLLOFFFACTOR ),

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
OOVPA_XREF(IDirectSoundBuffer_SetRolloffFactor, 5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_DSBUFFERSETROLLOFFFACTOR ),

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
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetDopplerFactor, 5788, 7,

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
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetDopplerFactor, 5788, 8,

    XREF_DSBUFFERSETDOPPLERFACTOR,
    XRefOne)

        XREF_ENTRY( 0x3A, XREF_DSVOICESETDOPPLERFACTOR ),

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
OOVPA_XREF(IDirectSoundBuffer_SetDopplerFactor, 5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_DSBUFFERSETDOPPLERFACTOR ),

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
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetPosition, 5788, 7,

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
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetPosition, 5788, 9,

    XREF_DSBUFFERSETPOSITION,
    XRefOne)

        XREF_ENTRY( 0x4A, XREF_DSVOICESETPOSITION ),

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
OOVPA_XREF(IDirectSoundBuffer_SetPosition, 5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x2D, XREF_DSBUFFERSETPOSITION ),

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
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetVelocity, 5788, 7,

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
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetVelocity, 5788, 9,

    XREF_DSBUFFERSETVELOCITY,
    XRefOne)

        XREF_ENTRY( 0x4A, XREF_DSVOICESETVELOCITY ),

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
OOVPA_XREF(IDirectSoundBuffer_SetVelocity, 5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x2D, XREF_DSBUFFERSETVELOCITY ),

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
OOVPA_XREF(DirectSound_CMcpxBuffer_Pause, 5788, 7,

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
OOVPA_XREF(DirectSound_CDirectSoundBuffer_Pause, 5788, 8,

    XREF_DirectSound_CDirectSoundBuffer_Pause,
    XRefOne)

        XREF_ENTRY( 0x35, XREF_DirectSound_CMcpxBuffer_Pause ),

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
OOVPA_XREF(IDirectSoundBuffer_Pause, 5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_DirectSound_CDirectSoundBuffer_Pause ),

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
OOVPA_XREF(DirectSound_CDirectSound_CreateSoundStream, 5788, 7,

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
//OOVPA_XREF(IDirectSound_CreateSoundStream, 5788, 8,
//
//    XRefNoSaveIndex,
//    XRefOne)
//
//        XREF_ENTRY( 0x1D, XREF_DirectSound_CDirectSound_CreateSoundStream ),
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
OOVPA_XREF(DirectSoundCreateStream, 5788, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x2F, XREF_DSCREATESOUNDSTREAM ),

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
OOVPA_XREF(DirectSound_CDirectSoundStream_SetMixBinVolumes, 5788, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_DSSETMIXBINVOLUMESB ),

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
OOVPA_XREF(DirectSound_CMcpxStream_Pause, 5788, 7,

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
OOVPA_XREF(DirectSound_CDirectSoundStream_Pause, 5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x35, XREF_DSSTREAMPAUSE ),

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
OOVPA_XREF(DirectSound_CDirectSoundStream_FlushEx, 5788, 7,

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
OOVPA_XREF(IDirectSoundStream_FlushEx, 5788, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x11, XREF_DSFLUSHEX2 ),

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
OOVPA_NO_XREF(DirectSoundDoWork, 5788, 7)

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
OOVPA_NO_XREF(IDirectSound_SetEffectData, 5788, 7)

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
OOVPA_NO_XREF(DirectSound_CMemoryManager_PoolAlloc, 5788, 10)

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
OOVPA_NO_XREF(XFileCreateMediaObjectAsync, 5788, 8)

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
#define DirectSound_CDirectSoundStream_SetPitch_5788 DirectSound_CDirectSoundStream_SetPitch_4627

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetHeadroom
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundStream_SetHeadroom, 5788, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_DirectSound_CDirectSoundVoice_SetHeadroom ),

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
// * DirectSound::CDirectSound::SetAllParameters
// ******************************************************************
#define DirectSound_CDirectSound_SetAllParameters_5788 DirectSound_CDirectSound_SetAllParameters_5558

// ******************************************************************
// * Direct, 5788
// ******************************************************************
OOVPATable DSound_5788[] = {

	REGISTER_OOVPA(DirectSoundCreate, 4361, PATCH),
	REGISTER_OOVPA(CDirectSound_CreateSoundBuffer, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_SetPlayRegion, 4361, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_SetBufferData, 5788, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetBufferData, 4361, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_SetBufferData, 4361, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_GetStatus, 5788, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_GetStatus, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_GetStatus, 4627, PATCH),
	REGISTER_OOVPA(CDirectSound_SetI3DL2Listener, 5788, XREF),
	REGISTER_OOVPA(IDirectSound8_SetI3DL2Listener, 5788, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetFormat, 5788, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetFormat, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_SetFormat, 5788, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetConeAngles, 5558, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetConeAngles, 5558, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_SetConeAngles, 5558, PATCH),
	REGISTER_OOVPA(CDirectSoundVoiceSettings_SetMixBinVolumes, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetMixBinVolumes, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMixBinVolumes, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_SetMixBinVolumes, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetMixBinVolumes2, 5788, ALIAS, DirectSound_CDirectSoundStream_SetMixBinVolumes),
	REGISTER_OOVPA(CDirectSoundVoice_SetRolloffCurve, 5233, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetRolloffCurve, 5233, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetRolloffCurve, 5558, ALIAS, DirectSound_CDirectSoundStream_SetRolloffCurve),
	REGISTER_OOVPA(IDirectSoundBuffer8_SetRolloffCurve, 5233, PATCH),
	REGISTER_OOVPA(IDirectSound8_CreateSoundBuffer, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetFrequency, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetFrequency, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_SetFrequency, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetVolume, 4627, PATCH),
	REGISTER_OOVPA(IDirectSound8_Release, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound8_DownloadEffectsImage, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound8_SetOrientation, 3936, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetMaxDistance, 5788, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetMaxDistance, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_SetMaxDistance, 5788, ALIAS, IDirectSoundBuffer_SetMaxDistance),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetMinDistance, 5788, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetMinDistance, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_SetMinDistance, 5788, ALIAS, IDirectSoundBuffer_SetMinDistance),
	REGISTER_OOVPA(DirectSound_CMcpxBuffer_Play, 5788, XREF),
	// REGISTER_OOVPA(CMcpxBuffer_Play, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_Stop, 4627, ALIAS, IDirectSoundBuffer_Stop), // (Possibly weak, but quite OK for 4627 DSOUND)
	REGISTER_OOVPA(IDirectSoundBuffer8_Release, 3936, PATCH),
	REGISTER_OOVPA(DirectSound_CFullHRTFSource_GetCenterVolume, 5558, XREF),
	REGISTER_OOVPA(DirectSound_CHRTFSource_SetFullHRTF5Channel, 5558, XREF),
	REGISTER_OOVPA(DirectSoundUseFullHRTF, 5558, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetHeadroom, 5788, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetHeadroom, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_SetHeadroom, 5788, ALIAS, IDirectSoundBuffer_SetHeadroom),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetAllParameters, 5558, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetAllParameters, 5558, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_SetAllParameters, 5558, ALIAS, IDirectSoundBuffer_SetAllParameters),
	REGISTER_OOVPA(IDirectSoundBuffer8_Lock, 5788, ALIAS, IDirectSoundBuffer_Lock),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetI3DL2Source, 5558, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetI3DL2Source, 5558, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_SetI3DL2Source, 5558, ALIAS, IDirectSoundBuffer_SetI3DL2Source),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoiceSettings_SetMixBins, 5788, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetMixBins, 5788, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetMixBins, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_SetMixBins, 5788, ALIAS, IDirectSoundBuffer_SetMixBins),
	REGISTER_OOVPA(CDirectSoundStream_SetMixBins, 5788, ALIAS, DirectSound_CDirectSoundStream_SetMixBins),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetMixBinHeadroom, 5788, XREF),
	REGISTER_OOVPA(IDirectSound8_SetMixBinHeadroom, 5788, ALIAS, IDirectSound_SetMixBinHeadroom),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetPosition, 5788, XREF),
	REGISTER_OOVPA(IDirectSound8_SetPosition, 5788, ALIAS, IDirectSound_SetPosition),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetVelocity, 5788, XREF),
	REGISTER_OOVPA(IDirectSound8_SetVelocity, 5788, ALIAS, IDirectSound_SetVelocity),
	REGISTER_OOVPA(CDirectSoundVoice_SetDistanceFactor, 5558, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetDistanceFactor, 5558, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_SetDistanceFactor, 5558, PATCH),
	REGISTER_OOVPA(CDirectSound_CommitDeferredSettings, 5788, ALIAS, DirectSound_CDirectSound_CommitDeferredSettings),
	REGISTER_OOVPA(DirectSoundCreateBuffer, 4627, PATCH),
	REGISTER_OOVPA(DirectSound_CMcpxBuffer_SetCurrentPosition, 5788, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetCurrentPosition, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_SetCurrentPosition, 5788, ALIAS, IDirectSoundBuffer_SetCurrentPosition),
	REGISTER_OOVPA(CMcpxVoiceClient_SetLFO, 5558, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetLFO, 5558, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetLFO, 5558, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_SetLFO, 5558, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_GetCurrentPosition, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_GetCurrentPosition, 5788, ALIAS, IDirectSoundBuffer_GetCurrentPosition),
	REGISTER_OOVPA(CDirectSoundBuffer_SetPlayRegion, 5558, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_SetPlayRegion, 5558, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetLoopRegion, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_SetLoopRegion, 5788, ALIAS, IDirectSoundBuffer_SetLoopRegion),
	REGISTER_OOVPA(DirectSound_CMcpxBuffer_Stop, 5788, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_StopEx, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_StopEx, 5788, ALIAS, DirectSound_CDirectSoundBuffer_StopEx),
	REGISTER_OOVPA(CDirectSound_SetDistanceFactorA, 5558, XREF),
	REGISTER_OOVPA(IDirectSound8_SetDistanceFactor, 4432, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetRolloffFactor, 5788, XREF),
	REGISTER_OOVPA(IDirectSound8_SetRolloffFactor, 4627, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetDopplerFactor, 5788, XREF),
	REGISTER_OOVPA(IDirectSound8_SetDopplerFactor, 4627, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetPitch, 5788, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetPitch, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_SetPitch, 5788, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_SetVolume, 5558, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetVolume, 5233, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetVolume, 5233, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_SetVolume, 5233, PATCH),
	REGISTER_OOVPA(IDirectSoundStream_SetVolume, 5233, ALIAS, CDirectSoundStream_SetVolume), // This is actually the XREF we are using
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_PlayEx, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_PlayEx, 5788, ALIAS, IDirectSoundBuffer_PlayEx),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetRolloffFactor, 5788, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetRolloffFactor, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_SetRolloffFactor, 5788, ALIAS, IDirectSoundBuffer_SetRolloffFactor),
	REGISTER_OOVPA(CDirectSoundVoice_SetFrequency, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetFrequency, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetFrequency, 5558, ALIAS, DirectSound_CDirectSoundStream_SetFrequency),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetDopplerFactor, 5788, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetDopplerFactor, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_SetDopplerFactor, 5788, ALIAS, IDirectSoundBuffer_SetDopplerFactor),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetPosition, 5788, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetPosition, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_SetPosition, 5788, ALIAS, IDirectSoundBuffer_SetPosition),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetVelocity, 5788, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetVelocity, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_SetVelocity, 5788, ALIAS, IDirectSoundBuffer_SetVelocity),
	REGISTER_OOVPA(DirectSound_CMcpxBuffer_Pause, 5788, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_Pause, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_Pause, 5788, ALIAS, IDirectSoundBuffer_Pause),
	REGISTER_OOVPA(DirectSoundCreateStream, 5788, ALIAS, DirectSound_CDirectSound_CreateSoundStream),
	REGISTER_OOVPA(DirectSoundCreateStream, 5788, PATCH),
	REGISTER_OOVPA(DirectSound_CMcpxStream_Pause, 5788, XREF),
	REGISTER_OOVPA(CDirectSoundStream_Pause, 5788, ALIAS, DirectSound_CDirectSoundStream_Pause),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_FlushEx, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundStream_FlushEx, 5788, PATCH),
	REGISTER_OOVPA(DirectSoundDoWork, 5788, PATCH),
	REGISTER_OOVPA(CDirectSound_SynchPlayback, 5558, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_EnableHeadphones, 5558, XREF),
	REGISTER_OOVPA(IDirectSound8_EnableHeadphones, 5558, ALIAS, IDirectSound_EnableHeadphones),
	REGISTER_OOVPA(XAudioDownloadEffectsImage, 4627, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_GetOutputLevels, 5558, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSound_GetOutputLevels, 5558, XREF),
	REGISTER_OOVPA(IDirectSound8_GetOutputLevels, 5558, ALIAS, IDirectSound_GetOutputLevels),
	REGISTER_OOVPA(IDirectSound8_SetEffectData, 5788, ALIAS, IDirectSound_SetEffectData),
	REGISTER_OOVPA(DirectSound_CMemoryManager_PoolAlloc, 5788, XREF),
	REGISTER_OOVPA(XFileCreateMediaObjectAsync, 5788, PATCH),
	REGISTER_OOVPA(WaveFormat_CreateXboxAdpcmFormat, 5558, XREF),
	REGISTER_OOVPA(XAudioCreateAdpcmFormat, 5558, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetFormat, 5558, ALIAS, DirectSound_CDirectSoundStream_SetFormat),
	REGISTER_OOVPA(CDirectSoundStream_SetPitch, 5788, ALIAS, DirectSound_CDirectSoundStream_SetPitch),
	REGISTER_OOVPA(IDirectSoundStream_SetHeadroom, 5788, ALIAS, DirectSound_CDirectSoundStream_SetHeadroom),
	REGISTER_OOVPA(DirectSound_CDirectSound_CreateSoundStream, 5558, XREF),
	REGISTER_OOVPA(IDirectSound8_CreateStream, 5558, ALIAS, IDirectSound_CreateSoundStream),
	REGISTER_OOVPA(DirectSound_CMcpxBuffer_Play2, 5788, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_Play, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_Play, 5788, ALIAS, IDirectSoundBuffer_Play),
	REGISTER_OOVPA(DirectSound_CMcpxVoiceClient_SetEG, 4627, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetEG, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetEG, 4627, ALIAS, DirectSound_CDirectSoundStream_SetEG),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetEG, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer8_SetEG, 4627, ALIAS, IDirectSoundBuffer_SetEG),
	REGISTER_OOVPA(IDirectSound8_AddRef, 3936, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetAllParameters, 5788, XREF),
	REGISTER_OOVPA(IDirectSound8_SetAllParameters, 3936, PATCH),
};

// ******************************************************************
// * DSound_5788_SIZE
// ******************************************************************
uint32 DSound_5788_SIZE = sizeof(DSound_5788);
