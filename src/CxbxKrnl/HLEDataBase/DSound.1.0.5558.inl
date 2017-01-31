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
OOVPA_NO_XREF(CDirectSound_SynchPlayback, 5558, 7)

        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0xE8 },
        { 0x3A, 0x0B },
        { 0x46, 0x8B },
OOVPA_END;

// ******************************************************************
// * CMcpxStream_Pause
// ******************************************************************
OOVPA_XREF(CMcpxStream_Pause, 5558, 11,

    XREF_DSSTREAMPAUSE,
    XRefZero)

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
OOVPA_END;

// ******************************************************************
// * CDirectSound::CommitDeferredSettings
// ******************************************************************
OOVPA_NO_XREF(CDirectSound_CommitDeferredSettings, 5558, 11)

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
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetDistanceFactorA
// ******************************************************************
OOVPA_XREF(CDirectSound_SetDistanceFactorA, 5558, 11,

    XREF_SETDISTANCEFACTORA,
    XRefZero)

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
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetRolloffFactor
// ******************************************************************
OOVPA_XREF(CDirectSound_SetRolloffFactor, 5558, 11,

    XREF_SETROLLOFFFACTORA,
    XRefZero)

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
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetDopplerFactor
// ******************************************************************
OOVPA_XREF(CDirectSound_SetDopplerFactor, 5558, 14,

    XREF_SETDOPPLERFACTOR,
    XRefZero)

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
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetMixBinHeadroom
// ******************************************************************
OOVPA_XREF(IDirectSound8_SetMixBinHeadroom, 5558, 9,

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
// * CDirectSound_SetI3DL2Listener
// ******************************************************************
OOVPA_XREF(CDirectSound_SetI3DL2Listener, 5558, 8,

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
OOVPA_XREF(IDirectSound8_SetI3DL2Listener, 5558, 9,

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
// * DirectSound::CFullHRTFSource::GetCenterVolume
// ******************************************************************
OOVPA_XREF(DirectSound_CFullHRTFSource_GetCenterVolume, 5558, 7,

    XREF_DirectSound_CFullHRTFSource_GetCenterVolume,
    XRefZero)

        { 0x13, 0xD9 },
        { 0x29, 0xDF },
        { 0x3D, 0x2D },
        { 0x54, 0xC1 },
        { 0x67, 0x4D },
        { 0x7E, 0xD9 },
        { 0x91, 0x10 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CHRTFSource::SetFullHRTF5Channel
// ******************************************************************
OOVPA_XREF(DirectSound_CHRTFSource_SetFullHRTF5Channel, 5558, 9,

    XREF_DirectSound_CHRTFSource_SetFullHRTF5Channel,
    XRefOne)

        { 0x4C, XREF_DirectSound_CFullHRTFSource_GetCenterVolume },

        { 0x14, 0xC7 },
        { 0x1E, 0xC7 },
        { 0x28, 0xC7 },
        { 0x3C, 0xC7 },
        { 0x46, 0xC7 },
        { 0x5A, 0xC7 },
        { 0x64, 0xC7 },
        { 0x6E, 0xC3 },
OOVPA_END;

// ******************************************************************
// * DirectSoundUseFullHRTF
// ******************************************************************
OOVPA_XREF(DirectSoundUseFullHRTF, 5558, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x0A, XREF_DirectSound_CHRTFSource_SetFullHRTF5Channel },

        { 0x06, 0x0F },
        { 0x07, 0xB6 },
        { 0x0E, 0x85 },
        { 0x0F, 0xF6 },
        { 0x12, 0x0B },
        { 0x18, 0xFF },
        { 0x1E, 0xC3 },
OOVPA_END;

// ******************************************************************
// * DirectSoundDoWork
// ******************************************************************
OOVPA_NO_XREF(DirectSoundDoWork, 5558, 7)

        { 0x06, 0x0F },
        { 0x0E, 0x85 },
        { 0x10, 0x74 },
        { 0x18, 0x85 },
        { 0x1C, 0x0B },
        { 0x22, 0xFF },
        { 0x28, 0xC3 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetBufferData
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetBufferData, 5558, 7,

    XREF_DSSETBUFFERDATA,
    XRefZero)

        { 0x14, 0x0C },
        { 0x2A, 0xB6 },
        { 0x40, 0xB8 },
        { 0x56, 0x8B },
        { 0x6C, 0x8B },
        { 0x82, 0x7C },
        { 0x98, 0x5E },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetBufferData
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetBufferData, 5558, 9,

    XRefNoSaveIndex,
    XRefOne)

        { 0x19, XREF_DSSETBUFFERDATA },

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
// * CMcpxBuffer::SetCurrentPosition
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_SetCurrentPosition, 5558, 7,

    XREF_SETCURRENTPOSITION,
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
// * CDirectSoundBuffer::SetCurrentPosition
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetCurrentPosition, 5558, 8,

    XREF_SETCURRENTPOSITION2,
    XRefOne)

        { 0x35, XREF_SETCURRENTPOSITION },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x39, 0x85 },
        { 0x44, 0xFF },
        { 0x4B, 0xC7 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetCurrentPosition
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetCurrentPosition, 5558, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x15, XREF_SETCURRENTPOSITION2 },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * CMcpxVoiceClient::SetLFO
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_SetLFO, 5558, 8,

    XREF_DirectSound_CMcpxVoiceClient_SetLFO,
    XRefZero)

        { 0x1E, 0x48 },
        { 0x3E, 0x31 },
        { 0x5E, 0x35 },
        { 0x7E, 0xE1 },
        { 0x9E, 0x8B },
        { 0xBE, 0x8B },
        { 0xDE, 0x05 },
        { 0xFE, 0x6C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetLFO
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetLFO, 5558, 7,

    XREF_DirectSound_CDirectSoundVoice_SetLFO,
    XRefOne)

        { 0x0C, XREF_DirectSound_CMcpxVoiceClient_SetLFO },

        { 0x01, 0x44 },
        { 0x04, 0xFF },
        { 0x07, 0x08 },
        { 0x0A, 0x0C },
        { 0x10, 0xC2 },
        { 0x11, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetLFO
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetLFO, 5558, 8,

    XREF_DirectSound_CDirectSoundBuffer_SetLFO,
    XRefOne)

        { 0x32, XREF_DirectSound_CDirectSoundVoice_SetLFO },

        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0x10 },
        { 0x3A, 0x74 },
        { 0x47, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetLFO
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetLFO, 5558, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x15, XREF_DirectSound_CDirectSoundBuffer_SetLFO },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * CMcpxVoiceClient_SetVolume
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_SetVolume, 5558, 13,

    XREF_DSSTREAMSETVOLUME,
    XRefZero)

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
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoiceSettings::SetMixBins
// ******************************************************************
OOVPA_XREF(CDirectSoundVoiceSettings_SetMixBins, 5558, 7,

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
// * CDirectSoundVoice::SetMixBins
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMixBins, 5558, 8,

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
// * CDirectSoundBuffer::SetMixBins
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetMixBins, 5558, 8,

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
// * IDirectSoundBuffer8_SetMixBins
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetMixBins, 5558, 8,

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
// * CDirectSoundBuffer::SetPlayRegion
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetPlayRegion, 5558, 8,

    XREF_DSBUFFERSETPLAYREGIONA,
    XRefZero)

        { 0x0F, 0x00 },
        { 0x1E, 0xFF },
        { 0x2E, 0x85 },
        { 0x3E, 0x3B },
        { 0x4E, 0x81 },
        { 0x5F, 0x8B },
        { 0x72, 0xFF },
        { 0x7E, 0x0C },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetPlayRegion
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetPlayRegion, 5558, 9,

    XRefNoSaveIndex,
    XRefOne)

        { 0x19, XREF_DSBUFFERSETPLAYREGIONA },

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
// * CDirectSoundBuffer::SetLoopRegion
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetLoopRegion, 5558, 7,

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
OOVPA_XREF(IDirectSoundBuffer8_SetLoopRegion, 5558, 9,

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
// * CMcpxBuffer::Play
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_Play, 5558, 8,

    XREF_DSOUNDPLAY,
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
// * CDirectSoundBuffer::Play
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_Play, 5558, 9,

    XREF_DSOUNDPLAY2,
    XRefZero) // TODO : Use XRefOne here if XREF_DSOUNDPLAY is enabled below

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
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_Play
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_Play, 5558, 8,

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
// * CMcpxBuffer_GetStatus
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_GetStatus, 5558, 11,

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
// * WaveFormat::CreateXboxAdpcmFormat
// ******************************************************************
OOVPA_XREF(WaveFormat_CreateXboxAdpcmFormat, 5558, 7,

    XREF_DirectSound_WaveFormat_CreateXboxAdpcmFormat,
    XRefZero)

        { 0x07, 0x08 },
        { 0x10, 0xE9 },
        { 0x19, 0x8D },
        { 0x22, 0x66 },
        { 0x2B, 0x04 },
        { 0x34, 0x66 },
        { 0x3D, 0x12 },
OOVPA_END;

// ******************************************************************
// * XAudioCreateAdpcmFormat
// ******************************************************************
OOVPA_XREF(XAudioCreateAdpcmFormat, 5558, 2,

    XRefNoSaveIndex,
    XRefOne)

        { 0x01, XREF_DirectSound_WaveFormat_CreateXboxAdpcmFormat },

        { 0x00, 0xE9 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetFormat
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetFormat, 5558, 7,

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
// * CDirectSoundBuffer::SetFormat
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetFormat, 5558, 8,

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
OOVPA_XREF(IDirectSoundBuffer8_SetFormat, 5558, 8,

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
// * CDirectSoundVoice::SetConeAngles
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetConeAngles, 5558, 8,

    XREF_DirectSound_CDirectSoundVoice_SetConeAngles,
    XRefZero)

        { 0x07, 0x8B },
        { 0x10, 0x08 },
        { 0x13, 0x20 },
        { 0x19, 0xB4 },
        { 0x22, 0x51 },
        { 0x2B, 0x00 },
        { 0x34, 0x10 },
        { 0x3E, 0x33 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetConeAngles
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetConeAngles, 5558, 8,

    XREF_DirectSound_CDirectSoundBuffer_SetConeAngles,
    XRefOne)

        { 0x3A, XREF_DirectSound_CDirectSoundVoice_SetConeAngles },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x74 },
        { 0x35, 0xFF },
        { 0x40, 0x8B },
        { 0x4F, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetConeAngles
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetConeAngles, 5558, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x1D, XREF_DirectSound_CDirectSoundBuffer_SetConeAngles },

        { 0x03, 0x10 },
        { 0x08, 0xFF },
        { 0x0D, 0xC8 },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetMaxDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMaxDistance, 5558, 7,

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
// * CDirectSoundBuffer::SetMaxDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetMaxDistance, 5558, 11,

    XREF_DSBUFFERSETMAXDISTANCE,
    XRefOne)

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
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetMaxDistance
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMaxDistance, 5558, 11,

    XRefNoSaveIndex,
    XRefOne)

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
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetMinDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMinDistance, 5558, 7,

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
// * CDirectSoundBuffer::SetMinDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetMinDistance, 5558, 11,

    XREF_DSBUFFERSETMINDISTANCE,
    XRefOne)

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
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetMinDistance
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundStream_SetMinDistance, 5558, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x3E, XREF_DSVOICESETROLLOFFCURVE },

        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x24 },
        { 0x3A, 0x1C },
        { 0x46, 0x74 },
        { 0x53, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetMinDistance
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMinDistance, 5558, 11,

    XRefNoSaveIndex,
    XRefOne)

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
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::CommitDeferredSettings
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_CommitDeferredSettings, 5558, 6,

    XREF_DirectSound_CDirectSoundVoice_CommitDeferredSettings,
    XRefZero)

        { 0x01, 0x44 },
        { 0x04, 0x8B },
        { 0x07, 0xE8 },
        { 0x0C, 0x33 },
        { 0x0D, 0xC0 },
        { 0x10, 0x00 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetAllParameters
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetAllParameters, 5558, 14,

    XREF_DirectSound_CDirectSoundVoice_SetAllParameters,
    XRefOne)

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
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetAllParameters
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetAllParameters, 5558, 8,

    XREF_DirectSound_CDirectSoundBuffer_SetAllParameters,
    XRefOne)

        { 0x36, XREF_DirectSound_CDirectSoundVoice_SetAllParameters },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x74 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8::SetAllParameters
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetAllParameters, 5558, 9,

    XRefNoSaveIndex,
    XRefOne)

        { 0x19, XREF_DirectSound_CDirectSoundBuffer_SetAllParameters },

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
// * CDirectSoundVoice::SetPosition
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetPosition, 5558, 7,

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
// * CDirectSoundBuffer::SetPosition
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetPosition, 5558, 9,

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
// * IDirectSoundBuffer8::SetPosition
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetPosition, 5558, 8,

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
// * CDirectSoundVoice::SetVelocity
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetVelocity, 5558, 7,

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
// * CDirectSoundBuffer::SetVelocity
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetVelocity, 5558, 9,

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
// * IDirectSoundBuffer8_SetVelocity
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetVelocity, 5558, 8,

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

/*// ******************************************************************
// * CMcpxStream::Pause
// ******************************************************************
OOVPA_XREF(DirectSound_CMcpxStream_Pause, 5558, 7,

    XREF_DirectSound_CMcpxStream_Pause,
    XRefZero)

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
OOVPA_XREF(CDirectSoundStream_Pause, 5558, 8,

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
// * CDirectSoundVoice::SetDistanceFactor
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetDistanceFactor, 5558, 7,

    XREF_DSVOICESETDISTANCEFACTOR,
    XRefZero)

        { 0x05, 0x41 },
        { 0x0C, 0x00 },
        { 0x13, 0x44 },
        { 0x1A, 0x00 },
        { 0x21, 0xF6 },
        { 0x28, 0x51 },
        { 0x2F, 0xC0 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetDistanceFactor
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetDistanceFactor, 5558, 8,

    XREF_DSBUFFERSETDISTANCEFACTOR,
    XRefOne)

        { 0x3A, XREF_DSVOICESETDISTANCEFACTOR },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xFF },
        { 0x40, 0x8B },
        { 0x4F, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetDistanceFactor
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetDistanceFactor, 5558, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x1D, XREF_DSBUFFERSETDISTANCEFACTOR },

        { 0x03, 0x0C },
        { 0x08, 0x8B },
        { 0x0D, 0x8B },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetDopplerFactor
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetDopplerFactor, 5558, 7,

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
// * CDirectSoundBuffer::SetDopplerFactor
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetDopplerFactor, 5558, 8,

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
// * IDirectSoundBuffer8_SetDopplerFactor
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetDopplerFactor, 5558, 8,

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
// * IDirectSoundBuffer_Lock
// ******************************************************************
OOVPA_NO_XREF(IDirectSoundBuffer_Lock, 5558, 8)

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
// * DirectSound::CDirectSoundVoice::SetI3DL2Source
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetI3DL2Source, 5558, 7,

    XREF_DSVOICESETI3DL2SOURCE,
    XRefZero)

        { 0x18, 0x00 },
        { 0x32, 0x8B },
        { 0x4C, 0x8C },
        { 0x66, 0x50 },
        { 0x80, 0x89 },
        { 0x9A, 0x10 },
        { 0xB4, 0x49 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetI3DL2Source
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetI3DL2Source, 5558, 8,

    XREF_DSBUFFERSETI3DL2SOURCE,
    XRefOne)

        { 0x36, XREF_DSVOICESETI3DL2SOURCE },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x74 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetI3DL2Source
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetI3DL2Source, 5558, 9,

    XRefNoSaveIndex,
    XRefOne)

        { 0x19, XREF_DSBUFFERSETI3DL2SOURCE },

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
// * DirectSound::CDirectSoundVoice::SetAllParameters
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetAllParameters, 5558, 10,

    XREF_DirectSound_CDirectSoundVoice_SetAllParameters,
    XRefZero) // TODO : Use XRefOne here if XREF_DirectSound_CDirectSoundVoice_CommitDeferredSettings is enabled below

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
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetAllParameters
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetAllParameters, 5558, 8,

    XREF_DirectSound_CDirectSoundBuffer_SetAllParameters,
    XRefOne)

        { 0x36, XREF_DirectSound_CDirectSoundVoice_SetAllParameters },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x74 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetAllParameters
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetAllParameters, 5558, 9,

    XRefNoSaveIndex,
    XRefOne)

        { 0x19, XREF_DirectSound_CDirectSoundBuffer_SetAllParameters },

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
// * DirectSound::CDirectSoundVoice::SetHeadroom
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetHeadroom, 5558, 7,

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
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetHeadroom, 5558, 8,

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
// * DirectSound::CDirectSoundStream::SetHeadroom
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundStream_SetHeadroom, 5558, 9,

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
// * IDirectSoundBuffer_SetHeadroom
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetHeadroom, 5558, 8,

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
// * DirectSound::CDirectSoundBuffer::GetCurrentPosition
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_GetCurrentPosition, 5558, 7,

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
// * IDirectSoundBuffer_GetCurrentPosition@12
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_GetCurrentPosition, 5558, 9,

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
// * DirectSound::CDirectSoundStream::SetFormat
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundStream_SetFormat, 5558, 9,

    XRefNoSaveIndex,
    XRefOne)

        { 0x36, XREF_DirectSound_CDirectSoundVoice_SetFormat },

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
// * DirectSound::CDirectSoundStream::SetMixBins
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundStream_SetMixBins, 5558, 9,

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
// * DirectSound::CDirectSound::SetMixBinHeadroom
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_SetMixBinHeadroom, 5558, 8,

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
OOVPA_XREF(IDirectSound_SetMixBinHeadroom, 5558, 9,

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
// * DirectSound::CDirectSound::EnableHeadphones
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_EnableHeadphones, 5558, 8,

    XREF_DSENABLEHEADPHONES,
    XRefZero)

        { 0x1E, 0x3B },
        { 0x3E, 0xC1 },
        { 0x60, 0xEB },
        { 0x7E, 0x00 },
        { 0x9E, 0x75 },
        { 0xBE, 0x00 },
        { 0xDE, 0x59 },
        { 0xFE, 0x75 },
OOVPA_END;

// ******************************************************************
// * IDirectSound_EnableHeadphones
// ******************************************************************
OOVPA_XREF(IDirectSound_EnableHeadphones, 5558, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x15, XREF_DSENABLEHEADPHONES },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSound::GetOutputLevels
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_GetOutputLevels, 5558, 7,

    XREF_DirectSound_CDirectSound_GetOutputLevels,
    XRefZero)

        { 0x07, 0x10 },
        { 0x10, 0xF3 },
        { 0x19, 0x5E },
        { 0x22, 0xFE },
        { 0x2B, 0x85 },
        { 0x34, 0xA6 },
        { 0x3D, 0xD0 },
OOVPA_END;

// ******************************************************************
// * IDirectSound_GetOutputLevels
// ******************************************************************
OOVPA_XREF(IDirectSound_GetOutputLevels, 5558, 9,

    XRefNoSaveIndex,
    XRefOne)

        { 0x19, XREF_DirectSound_CDirectSound_GetOutputLevels },

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
// * XAudioDownloadEffectsImage
// ******************************************************************
OOVPA_NO_XREF(XAudioDownloadEffectsImage, 5558, 8)

        { 0x1E, 0xB6 },
        { 0x41, 0x83 },
        { 0x61, 0x8B },
        { 0x80, 0x85 },
        { 0x9E, 0x8B },
        { 0xBE, 0x6A },
        { 0xDE, 0x07 },
        { 0xFE, 0xF7 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSound::SetPosition
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_SetPosition, 5558, 7,

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
OOVPA_XREF(IDirectSound_SetPosition, 5558, 8,

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
OOVPA_XREF(DirectSound_CDirectSound_SetVelocity, 5558, 7,

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
OOVPA_XREF(IDirectSound_SetVelocity, 5558, 8,

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
// * DirectSound::CDirectSound::CreateSoundStream
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_CreateSoundStream, 5558, 7,

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
OOVPA_XREF(IDirectSound_CreateSoundStream, 5558, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x1D, XREF_DSCREATESOUNDSTREAM },

        { 0x03, 0x10 },
        { 0x08, 0xFF },
        { 0x0D, 0xC8 },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetRolloffCurve
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundStream_SetRolloffCurve, 5558, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x3E, XREF_DSVOICESETROLLOFFCURVE },

        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x74 },
        { 0x3A, 0x24 },
        { 0x46, 0x74 },
        { 0x53, 0x8B },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetFrequency
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundStream_SetFrequency, 5558, 9,

    XRefNoSaveIndex,
    XRefOne)

        { 0x36, XREF_DSBUFFERSETFREQUENCYB },

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
// * Direct, 5558
// ******************************************************************
OOVPATable DSound_5558[] = {

    // DirectSoundCreate (* unchanged since 4361 *)
    OOVPA_TABLE_ENTRY(DirectSoundCreate, 4361),
    // CDirectSound_CreateSoundBuffer (* unchanged since 4627 *)
    OOVPA_TABLE_XREF(CDirectSound_CreateSoundBuffer, 4627),
    // CDirectSound::SynchPlayback
    OOVPA_TABLE_ENTRY(CDirectSound_SynchPlayback, 5558),
    // CMcpxStream::Pause
    OOVPA_TABLE_XREF(CMcpxStream_Pause, 5558),
    // CDirectSoundStream::Pause
    OOVPA_TABLE_ENTRY(CDirectSoundStream_Pause, 5558),
    // CDirectSoundStream::Pause (* unchanged since 4361 *)
    OOVPA_TABLE_ENTRY(CDirectSoundStream_Pause, 4361),
    // CDirectSoundVoiceSettings_SetMixBinVolumes (* unchanged since 4627 *)
    OOVPA_TABLE_XREF(CDirectSoundVoiceSettings_SetMixBinVolumes, 4627),
    // CDirectSoundVoice_SetMixBinVolumes (* unchanged since 4627 *)
    OOVPA_TABLE_XREF(CDirectSoundVoice_SetMixBinVolumes, 4627),
    // CDirectSoundBuffer_SetMixBinVolumes (* unchanged since 4627 *)
    OOVPA_TABLE_XREF(CDirectSoundBuffer_SetMixBinVolumes, 4627),
    // IDirectSoundBuffer8_SetMixBinVolumes (* unchanged since 4627 *)
    OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetMixBinVolumes, 4627),
    // IDirectSound8::CreateSoundBuffer (* unchanged since 4627 *)
    OOVPA_TABLE_ENTRY(IDirectSound8_CreateSoundBuffer, 4627),
    // CDirectSound::CommitDeferredSettings
    OOVPA_TABLE_ENTRY(CDirectSound_CommitDeferredSettings, 5558),
    // CDirectSound::SetDistanceFactorA (XREF)
    OOVPA_TABLE_XREF(CDirectSound_SetDistanceFactorA, 5558),
    // IDirectSound8::SetDistanceFactor (* unchanged since 4432 *)
    OOVPA_TABLE_ENTRY(IDirectSound8_SetDistanceFactor, 4432),
    // CDirectSound::SetRolloffFactor (XREF)
    OOVPA_TABLE_XREF(CDirectSound_SetRolloffFactor, 5558),
    // IDirectSound8::SetRolloffFactor (* unchanged since 4627 *)
    OOVPA_TABLE_ENTRY(IDirectSound8_SetRolloffFactor, 4627),
    // CDirectSound::SetDopplerFactor (XREF)
    OOVPA_TABLE_XREF(CDirectSound_SetDopplerFactor, 5558),
    // CDirectSoundStream::FlushEx (XREF) (* unchanged since 5233 *)
    OOVPA_TABLE_XREF(CDirectSoundStream_FlushEx, 5233),
    // IDirectSoundBuffer8::StopEx (* unchanged since 5233 *)
    OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_StopEx, 5233),
    // IDirectSound8::SetDopplerFactor (* unchanged since 4627 *)
    OOVPA_TABLE_ENTRY(IDirectSound8_SetDopplerFactor, 4627),
    // CDirectSoundVoice::SetFrequency (XREF) (* unchanged since 4627 *)
    OOVPA_TABLE_XREF(CDirectSoundVoice_SetFrequency, 4627),
    // CDirectSoundBuffer::SetFrequency (* unchanged since 4627 *)
    OOVPA_TABLE_XREF(CDirectSoundBuffer_SetFrequency, 4627),
    // CDirectSoundStream::SetFrequency
    OOVPA_TABLE_PATCH(DirectSound_CDirectSoundStream_SetFrequency, 5558, CDirectSoundStream_SetFrequency),
    // IDirectSoundBuffer8::SetFrequency (* unchanged since 4627 *)
    OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetFrequency, 4627),
    // CMcpxVoiceClient_SetVolume
    OOVPA_TABLE_XREF(CMcpxVoiceClient_SetVolume, 5558),
    // CDirectSound_CreateSoundStream (* unchanged since 4361 *)
    OOVPA_TABLE_XREF(CDirectSound_CreateSoundStream, 4361),
    // DirectSoundCreateStream (* unchanged since 4361 *)
    OOVPA_TABLE_ENTRY(DirectSoundCreateStream, 4361),
    // IDirectSound8::Release (* unchanged since 3936 *)
    OOVPA_TABLE_ENTRY(IDirectSound8_Release, 3936),
    // IDirectSound8::DownloadEffectsImage (* unchanged since 3936 *)
    OOVPA_TABLE_ENTRY(IDirectSound8_DownloadEffectsImage, 3936),
    // IDirectSound8::SetOrientation (* unchanged since 3936 *)
    OOVPA_TABLE_ENTRY(IDirectSound8_SetOrientation, 3936),
    // CDirectSoundVoice::SetMaxDistance (XREF)
    OOVPA_TABLE_XREF(CDirectSoundVoice_SetMaxDistance, 5558),
    // CDirectSoundBuffer::SetMaxDistance (XREF)
    OOVPA_TABLE_XREF(CDirectSoundBuffer_SetMaxDistance, 5558),
    // IDirectSoundBuffer_SetMaxDistance
    OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetMaxDistance, 5558, IDirectSoundBuffer8_SetMaxDistance),
    // CDirectSoundVoice::SetMinDistance (XREF)
    OOVPA_TABLE_XREF(CDirectSoundVoice_SetMinDistance, 5558),
    // CDirectSoundBuffer::SetMinDistance (XREF)
    OOVPA_TABLE_XREF(CDirectSoundBuffer_SetMinDistance, 5558),
    // CDirectSoundStream::SetRolloffCurve (XREF)
    OOVPA_TABLE_XREF(DirectSound_CDirectSoundStream_SetRolloffCurve, 5558),
    // IDirectSoundBuffer_SetMinDistance
    OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetMinDistance, 5558, IDirectSoundBuffer8_SetMinDistance),
    // CDirectSoundVoice::SetRolloffFactor (XREF) (* unchanged since 4627 *)
    /* THESE ARE NOT CORRECT
    OOVPA_TABLE_XREF(CDirectSoundVoice_SetRolloffFactor, 4627),
    // CDirectSoundBuffer::SetRolloffFactor (XREF) (* unchanged since 4627 *)
    OOVPA_TABLE_XREF(CDirectSoundBuffer_SetRolloffFactor, 4627),
    // IDirectSoundBuffer_SetRolloffFactor (* unchanged since 4627 *)
    OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetRolloffFactor, 4627, IDirectSoundBuffer8_SetRolloffFactor),
    */
    // CDirectSoundVoice::SetDopplerFactor (XREF)
    OOVPA_TABLE_XREF(CDirectSoundVoice_SetDopplerFactor, 5558),
    // CDirectSoundBuffer::SetDopplerFactor (XREF)
    OOVPA_TABLE_XREF(CDirectSoundBuffer_SetDopplerFactor, 5558),
    // IDirectSoundBuffer8_SetDopplerFactor
    OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetDopplerFactor, 5558),
    // IDirectSoundBuffer_Stop (* unchanged since 4627 *)
    OOVPA_TABLE_PATCH(IDirectSoundBuffer_Stop, 4627, IDirectSoundBuffer8_Stop),
    //// CDirectSound_SetMixBinHeadroom
    //OOVPA_TABLE_XREF(CDirectSound_SetMixBinHeadroom, 4627), //// IDirectSound8_SetMixBinHeadroom
    //OOVPA_TABLE_ENTRY(IDirectSound8_SetMixBinHeadroom, 5558),
    // CDirectSound_SetI3DL2Listener
    OOVPA_TABLE_XREF(CDirectSound_SetI3DL2Listener, 5558),
    // IDirectSound8_SetI3DL2Listener
    OOVPA_TABLE_ENTRY(IDirectSound8_SetI3DL2Listener, 5558),

    // IDirectSoundBuffer8::Release (* unchanged since 3936 *)
    OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_Release, 3936),
    // DirectSound::CFullHRTFSource::GetCenterVolume (XREF)
    OOVPA_TABLE_XREF(DirectSound_CFullHRTFSource_GetCenterVolume, 5558),
    // DirectSound::CHRTFSource::SetFullHRTF5Channel (XREF)
    OOVPA_TABLE_XREF(DirectSound_CHRTFSource_SetFullHRTF5Channel, 5558),
    // DirectSoundUseFullHRTF
    OOVPA_TABLE_ENTRY(DirectSoundUseFullHRTF, 5558),
    // DirectSoundDoWork
    OOVPA_TABLE_ENTRY(DirectSoundDoWork, 5558),
    // CDirectSoundBuffer::SetBufferData
    OOVPA_TABLE_XREF(CDirectSoundBuffer_SetBufferData, 5558),
    // IDirectSoundBuffer8::SetBufferData
    OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetBufferData, 5558),
    // CMcpxBuffer::SetCurrentPosition
    OOVPA_TABLE_XREF(CMcpxBuffer_SetCurrentPosition, 5558),
    // CDirectSoundBuffer::SetCurrentPosition
    OOVPA_TABLE_XREF(CDirectSoundBuffer_SetCurrentPosition, 5558),
    // IDirectSoundBuffer8::SetCurrentPosition
    OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetCurrentPosition, 5558),
    // DirectSound::CMcpxVoiceClient::SetLFO (XREF)
    OOVPA_TABLE_XREF(CMcpxVoiceClient_SetLFO, 5558),
    // CDirectSoundVoice::SetLFO (XREF)
    OOVPA_TABLE_XREF(CDirectSoundVoice_SetLFO, 5558),
    // CDirectSoundBuffer::SetLFO (XREF)
    OOVPA_TABLE_XREF(CDirectSoundBuffer_SetLFO, 5558),
    // IDirectSoundBuffer8::SetLFO
    OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetLFO, 5558),
    // CDirectSoundVoice::SetVolume (XREF) (* unchanged since 5233 *)
    OOVPA_TABLE_XREF(CDirectSoundVoice_SetVolume, 5233),
    // CDirectSoundBuffer::SetVolume (XREF) (* unchanged since 5233 *)
    OOVPA_TABLE_XREF(CDirectSoundBuffer_SetVolume, 5233),
    // IDirectSoundBuffer8::SetVolume (* unchanged since 5233 *)
    OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetVolume, 5233),
    // CDirectSoundStream_SetVolume (* unchanged since 4627 *)
    OOVPA_TABLE_ENTRY(CDirectSoundStream_SetVolume, 4627),
    // CDirectSoundVoiceSettings_SetMixBins
    OOVPA_TABLE_XREF(CDirectSoundVoiceSettings_SetMixBins, 5558),
    // CDirectSoundVoice_SetMixBins
    OOVPA_TABLE_XREF(CDirectSoundVoice_SetMixBins, 5558),
    // CDirectSoundBuffer_SetMixBins
    OOVPA_TABLE_XREF(CDirectSoundBuffer_SetMixBins, 5558),
    // IDirectSoundBuffer8_SetMixBins
    OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetMixBins, 5558),
    // CDirectSoundBuffer::SetPlayRegion
    OOVPA_TABLE_XREF(CDirectSoundBuffer_SetPlayRegion, 5558),
    // IDirectSoundBuffer8::SetPlayRegion
    OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetPlayRegion, 5558),
    // CDirectSoundBuffer::SetLoopRegion (XREF)
    OOVPA_TABLE_XREF(CDirectSoundBuffer_SetLoopRegion, 5558),
    // IDirectSoundBuffer8::SetLoopRegion
    OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetLoopRegion, 5558),
    // CMcpxBuffer::Play
    OOVPA_TABLE_XREF(CMcpxBuffer_Play, 5558),
    // CDirectSoundBuffer::Play (XREF)
    OOVPA_TABLE_XREF(CDirectSoundBuffer_Play, 5558),
    // IDirectSoundBuffer_Play
    OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_Play, 5558),
    // CMcpxBuffer::GetStatus
    OOVPA_TABLE_XREF(CMcpxBuffer_GetStatus, 5558),
    // CDirectSoundBuffer::GetStatus
    OOVPA_TABLE_XREF(CDirectSoundBuffer_GetStatus, 4627),
    // IDirectSoundBuffer8::GetStatus
    OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_GetStatus, 4627),
    // WaveFormat::CreateXboxAdpcmFormat (XREF)
    OOVPA_TABLE_XREF(WaveFormat_CreateXboxAdpcmFormat, 5558),
    // XAudioCreateAdpcmFormat
    OOVPA_TABLE_ENTRY(XAudioCreateAdpcmFormat, 5558),
    // CDirectSoundVoice::SetFormat (XREF)
    OOVPA_TABLE_XREF(CDirectSoundVoice_SetFormat, 5558),

    // CDirectSoundBuffer::SetFormat (XREF)
    OOVPA_TABLE_XREF(CDirectSoundBuffer_SetFormat, 5558),
    // IDirectSoundBuffer8_SetFormat
    OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetFormat, 5558),
    // CDirectSoundVoice::SetConeAngles (XREF)
    OOVPA_TABLE_XREF(CDirectSoundVoice_SetConeAngles, 5558),
    // CDirectSoundBuffer::SetConeAngles (XREF)
    OOVPA_TABLE_XREF(CDirectSoundBuffer_SetConeAngles, 5558),
    // IDirectSoundBuffer_SetConeAngles
    OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetConeAngles, 5558),
    // CDirectSoundVoice::SetRolloffCurve (XREF) (* unchanged since 5233 *)
    OOVPA_TABLE_XREF(CDirectSoundVoice_SetRolloffCurve, 5233),

    // CDirectSoundBuffer::SetRolloffCurve (XREF) (* unchanged since 5233 *)
    OOVPA_TABLE_XREF(CDirectSoundBuffer_SetRolloffCurve, 5233),
    // CDirectSoundStream::SetRolloffCurve
    OOVPA_TABLE_PATCH(DirectSound_CDirectSoundStream_SetRolloffCurve, 5558, CDirectSoundStream_SetRolloffCurve),
    // IDirectSoundBuffer8::SetRolloffCurve (* unchanged since 5233 *)
    OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetRolloffCurve, 5233),

    // CDirectSoundVoice::CommitDeferredSettings (XREF)
    OOVPA_TABLE_XREF(CDirectSoundVoice_CommitDeferredSettings, 5558),
    // CDirectSoundVoice::SetAllParameters (XREF)
    /*
    OOVPA_TABLE_XREF(CDirectSoundVoice_SetAllParameters, 5558),
    // CDirectSoundBuffer::SetAllParameters (XREF)
    OOVPA_TABLE_XREF(CDirectSoundBuffer_SetAllParameters, 5558),
    // IDirectSoundBuffer8::SetAllParameters
    OOVPA_TABLE_PATCH(IDirectSoundBuffer8_SetAllParameters, 5558, IDirectSound8_SetAllParameters),     // Use that for now, EmuIDirectSoundBuffer8_SetAllParameters,
    */
    // CDirectSoundVoice::SetPosition (XREF)
    OOVPA_TABLE_XREF(CDirectSoundVoice_SetPosition, 5558),
    // CDirectSoundBuffer::SetPosition (XREF)
    OOVPA_TABLE_XREF(CDirectSoundBuffer_SetPosition, 5558),
    // IDirectSoundBuffer_SetPosition
    OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetPosition, 5558),
    // CDirectSoundVoice::SetVelocity (XREF)
    OOVPA_TABLE_XREF(CDirectSoundVoice_SetVelocity, 5558),
    // CDirectSoundBuffer::SetVelocity (XREF)
    OOVPA_TABLE_XREF(CDirectSoundBuffer_SetVelocity, 5558),
    // IDirectSoundBuffer::SetVelocity
    OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetVelocity, 5558),
    // CDirectSoundVoice::SetDistanceFactor (XREF)
    OOVPA_TABLE_XREF(CDirectSoundVoice_SetDistanceFactor, 5558),
    // CDirectSoundBuffer::SetDistanceFactor (XREF)
    OOVPA_TABLE_XREF(CDirectSoundBuffer_SetDistanceFactor, 5558),
    // IDirectSoundBuffer_SetDistanceFactor
    OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetDistanceFactor, 5558),
    // IDirectSoundStream::SetVolume (* unchanged since 5233 *)
    // (This is actually the XREF we are using)
    OOVPA_TABLE_PATCH(CDirectSoundStream_SetVolume, 5233, IDirectSoundStream_SetVolume),
    // DirectSoundCreateBuffer
    OOVPA_TABLE_ENTRY(DirectSoundCreateBuffer, 4627),
    // IDirectSoundBuffer_Lock
    OOVPA_TABLE_PATCH(IDirectSoundBuffer_Lock, 5558, IDirectSoundBuffer8_Lock),
    // CDirectSoundVoice::SetI3DL2Source (XREF)
    OOVPA_TABLE_XREF(DirectSound_CDirectSoundVoice_SetI3DL2Source, 5558),
    // CDirectSoundBuffer::SetI3DL2Source (XREF)
    OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_SetI3DL2Source, 5558),
    // IDirectSoundBuffer_SetI3DL2Source
    OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetI3DL2Source, 5558, IDirectSoundBuffer8_SetI3DL2Source),
    // CDirectSoundVoice::SetAllParameters (XREF)
    OOVPA_TABLE_XREF(DirectSound_CDirectSoundVoice_SetAllParameters, 5558),
    // CDirectSoundBuffer::SetAllParameters (XREF)
    OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_SetAllParameters, 5558),
    // IDirectSoundBuffer_SetAllParameters
    OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetAllParameters, 5558, IDirectSoundBuffer8_SetAllParameters),
    // DirectSound::CDirectSoundVoice::SetHeadroom (XREF)
    OOVPA_TABLE_XREF(DirectSound_CDirectSoundVoice_SetHeadroom, 5558),
    // DirectSound::CDirectSoundBuffer::SetHeadroom (XREF)
    OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_SetHeadroom, 5558),
    // IDirectSoundBuffer_SetHeadroom
    OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetHeadroom, 5558, IDirectSoundBuffer8_SetHeadroom),
    // DirectSound::CDirectSoundStream::SetHeadroom
    OOVPA_TABLE_PATCH(DirectSound_CDirectSoundStream_SetHeadroom, 5558, IDirectSoundStream_SetHeadroom),
    // CDirectSoundBuffer::GetCurrentPosition
    OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_GetCurrentPosition, 5558),
    // IDirectSoundBuffer8::GetCurrentPosition
    OOVPA_TABLE_PATCH(IDirectSoundBuffer_GetCurrentPosition, 5558, IDirectSoundBuffer8_GetCurrentPosition),
    // CDirectSoundStream::SetFormat (XREF)
    OOVPA_TABLE_PATCH(DirectSound_CDirectSoundStream_SetFormat, 5558, CDirectSoundStream_SetFormat),
    // CDirectSoundStream_SetMixBins
    OOVPA_TABLE_PATCH(DirectSound_CDirectSoundStream_SetMixBins, 5558, CDirectSoundStream_SetMixBins),
    // CDirectSound_SetMixBinHeadroom
    OOVPA_TABLE_XREF(DirectSound_CDirectSound_SetMixBinHeadroom, 5558),
    // IDirectSound8_SetMixBinHeadroom
    OOVPA_TABLE_PATCH(IDirectSound_SetMixBinHeadroom, 5558, IDirectSound8_SetMixBinHeadroom),
    // DirectSound::CDirectSound::EnableHeadphones (XRef)
    OOVPA_TABLE_XREF(DirectSound_CDirectSound_EnableHeadphones, 5558),
    // IDirectSound8::EnableHeadphones
    OOVPA_TABLE_PATCH(IDirectSound_EnableHeadphones, 5558, IDirectSound8_EnableHeadphones),
    // DirectSound::CDirectSound::GetOutputLevels (XREF)
    OOVPA_TABLE_XREF(DirectSound_CDirectSound_GetOutputLevels, 5558),
    // IDirectSound_GetOutputLevels
    OOVPA_TABLE_PATCH(IDirectSound_GetOutputLevels, 5558, IDirectSound8_GetOutputLevels),
    // XAudioDownloadEffectsImage
    OOVPA_TABLE_ENTRY(XAudioDownloadEffectsImage, 5558),
    // CDirectSound::SetPosition (XREF)
    OOVPA_TABLE_XREF(DirectSound_CDirectSound_SetPosition, 5558),
    // IDirectSound8::SetPosition
    OOVPA_TABLE_PATCH(IDirectSound_SetPosition, 5558, IDirectSound8_SetPosition),
    // CDirectSound::SetVelocity (XREF)
    OOVPA_TABLE_XREF(DirectSound_CDirectSound_SetVelocity, 5558),
    // IDirectSound8::SetVelocity
    OOVPA_TABLE_PATCH(IDirectSound_SetVelocity, 5558, IDirectSound8_SetVelocity),
    // CDirectSound_CreateSoundStream
    OOVPA_TABLE_XREF(DirectSound_CDirectSound_CreateSoundStream, 5558),
    // IDirectSound_CreateSoundStream
    OOVPA_TABLE_PATCH(IDirectSound_CreateSoundStream, 5558, IDirectSound8_CreateStream),
};

// ******************************************************************
// * DSound_5558_SIZE
// ******************************************************************
uint32 DSound_5558_SIZE = sizeof(DSound_5558);
