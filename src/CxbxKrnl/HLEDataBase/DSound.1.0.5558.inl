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
OOVPA_XREF(CDirectSound_SynchPlayback, 5558, 7,
           XREF_DSSYNCHPLAYBACKA,
           XRefZero)

        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0xE8 },
        { 0x3A, 0x0B },
        { 0x46, 0x8B },
OOVPA_END;

// ******************************************************************
// * CDirectSound::CommitDeferredSettings
// ******************************************************************
OOVPA_XREF(CDirectSound_CommitDeferredSettings, 5558, 11,

    XREF_CDirectSound_CommitDeferredSettings,
    XRefZero)

        // CDirectSound_CommitDeferredSettings+0x10 : movzx eax, al
        { 0x10, 0x0F },
        { 0x11, 0xB6 },
        { 0x12, 0xC0 },

        // CDirectSound_CommitDeferredSettings+0x27 : mov eax, 0x80004005
        { 0x27, 0xB8 },
        { 0x28, 0x05 },
        { 0x29, 0x40 },
        { 0x2B, 0x80 },

        // CDirectSound_CommitDeferredSettings+0x6D : mov [eax+0x30], edi
        { 0x6D, 0x89 },
        { 0x6E, 0x78 },
        { 0x6F, 0x30 },

        // CDirectSound_CommitDeferredSettings+0x96 : leave
        { 0x96, 0xC9 },
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetDistanceFactorA
// ******************************************************************
OOVPA_XREF(CDirectSound_SetDistanceFactor, 5558, 17,

    XREF_CDirectSound_SetDistanceFactor,
    XRefZero)

        // CDirectSound_SetDistanceFactor+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 },
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // CDirectSound_SetDistanceFactor+0x33 : mov [eax+68h], edx
        { 0x33, 0x89 },
        { 0x34, 0x50 },
        { 0x35, 0x68 },

        // CDirectSound_SetDistanceFactor+0x36 : mov eax, [ecx+8]
        { 0x36, 0x8B },
        { 0x37, 0x41 },
        { 0x38, 0x08 },

        // CDirectSound_SetDistanceFactor+0x39 : or dword ptr[eax+0x30], 0x08
        { 0x39, 0x83 },
        { 0x3A, 0x48 },
        { 0x3B, 0x30 },
        { 0x3C, 0x08 },

        // CDirectSound_SetDistanceFactor+0x4C : jz +0x0B
        { 0x4C, 0x74 },
        { 0x4D, 0x0B },
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetRolloffFactor
// ******************************************************************
OOVPA_XREF(CDirectSound_SetRolloffFactor, 5558, 17,

    XREF_CDirectSound_SetRolloffFactor,
    XRefZero)

        // CDirectSound_SetRolloffFactor+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 },
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // CDirectSound_SetRolloffFactor+0x33 : mov [eax+6Ch], edx
        { 0x33, 0x89 },
        { 0x34, 0x50 },
        { 0x35, 0x6C },

        // CDirectSound_SetRolloffFactor+0x36 : mov eax, [ecx+8]
        { 0x36, 0x8B },
        { 0x37, 0x41 },
        { 0x38, 0x08 },

        // CDirectSound_SetRolloffFactor+0x39 : or dword ptr[eax+0x30], 0x10
        { 0x39, 0x83 },
        { 0x3A, 0x48 },
        { 0x3B, 0x30 },
        { 0x3C, 0x10 },

        // CDirectSound_SetRolloffFactor+0x4C : jz +0x0B
        { 0x4C, 0x74 },
        { 0x4D, 0x0B },
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetDopplerFactor
// ******************************************************************
OOVPA_XREF(CDirectSound_SetDopplerFactor, 5558, 14,

    XREF_CDirectSound_SetDopplerFactor,
    XRefZero)

        // CDirectSound_SetDopplerFactor+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 },
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // CDirectSound_SetDopplerFactor+0x33 : mov [eax+0x70], edx
        { 0x33, 0x89 },
        { 0x34, 0x50 },
        { 0x35, 0x70 },

        // CDirectSound_SetDopplerFactor+0x39 : or dword ptr[eax+0x30], 0x20
        { 0x39, 0x83 },
        { 0x3A, 0x48 },
        { 0x3B, 0x30 },
        { 0x3C, 0x20 },

        // CDirectSound_SetDopplerFactor+0x4C : jz +0x0B
        { 0x4C, 0x74 },
        { 0x4D, 0x0B },
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetMixBinHeadroom
// ******************************************************************
OOVPA_XREF(IDirectSound_SetMixBinHeadroom, 5558, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_CDirectSound_SetMixBinHeadroom ),

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

    XREF_CDirectSound_SetI3DL2Listener,
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
// * DirectSound::CFullHRTFSource::GetCenterVolume
// ******************************************************************
OOVPA_XREF(CFullHRTFSource_GetCenterVolume, 5558, 7,

    XREF_CFullHRTFSource_GetCenterVolume,
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
OOVPA_XREF(CHRTFSource_SetFullHRTF5Channel, 5558, 9,

    XREF_CHRTFSource_SetFullHRTF5Channel,
    XRefOne)

        XREF_ENTRY( 0x4C, XREF_CFullHRTFSource_GetCenterVolume ),

        { 0x14, 0xC7 },
        { 0x1E, 0xC7 },
        { 0x28, 0xC7 },
        { 0x3C, 0xC7 },
        { 0x46, 0xC7 },
        { 0x5A, 0xC7 },
        { 0x64, 0xC7 },
        { 0x6E, 0xC3 },
OOVPA_END;

#if 0 // Moved to generic OOVPA 4134
// ******************************************************************
// * DirectSoundUseFullHRTF
// ******************************************************************
OOVPA_XREF(DirectSoundUseFullHRTF, 5558, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0A, XREF_CHRTFSource_SetFullHRTF5Channel ),

        { 0x06, 0x0F },
        { 0x07, 0xB6 },
        { 0x0E, 0x85 },
        { 0x0F, 0xF6 },
        { 0x12, 0x0B },
        { 0x18, 0xFF },
        { 0x1E, 0xC3 },
OOVPA_END;
#endif

#if 0 // Duplicate OOVPA found in 4134
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
#endif

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetBufferData
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetBufferData, 5558, 7,

	XREF_CDirectSoundBuffer_SetBufferData,
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
// * IDirectSoundBuffer_SetBufferData
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetBufferData, 5558, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_CDirectSoundBuffer_SetBufferData),

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

	XREF_CMcpxBuffer_SetCurrentPosition,
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

	XREF_CDirectSoundBuffer_SetCurrentPosition,
    XRefOne)

        XREF_ENTRY( 0x35, XREF_CMcpxBuffer_SetCurrentPosition),

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x39, 0x85 },
        { 0x44, 0xFF },
        { 0x4B, 0xC7 },
OOVPA_END;

#if 0 // Replaced with generic OOVPA 3911
// ******************************************************************
// * IDirectSoundBuffer_SetCurrentPosition
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetCurrentPosition, 5558, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_CDirectSoundBuffer_SetCurrentPosition),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;
#endif

// ******************************************************************
// * CDirectSoundVoiceSettings::SetMixBins
// ******************************************************************
OOVPA_XREF(CDirectSoundVoiceSettings_SetMixBins, 5558, 7,

    XREF_CDirectSoundVoiceSettings_SetMixBins,
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

    XREF_CDirectSoundVoice_SetMixBins,
    XRefOne)

        XREF_ENTRY( 0x0D, XREF_CDirectSoundVoiceSettings_SetMixBins ),

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

    XREF_CDirectSoundBuffer_SetMixBins,
    XRefOne)

        XREF_ENTRY( 0x32, XREF_CDirectSoundVoice_SetMixBins ),

        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0x10 },
        { 0x3A, 0x74 },
        { 0x47, 0x8B },
OOVPA_END;

#if 0 // Replaced with generic OOVPA 4039
// ******************************************************************
// * IDirectSoundBuffer_SetMixBins
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMixBins, 5558, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_CDirectSoundBuffer_SetMixBins ),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;
#endif

// ******************************************************************
// * CDirectSoundBuffer::SetPlayRegion
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetPlayRegion, 5558, 8,

    XREF_CDirectSoundBuffer_SetPlayRegion,
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
// * CDirectSoundBuffer::SetLoopRegion
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetLoopRegion, 5558, 7,

    XREF_CDirectSoundBuffer_SetLoopRegion,
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
// * public: long __thiscall DirectSound::CMcpxBuffer::Play(__int64,unsigned long)
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_Play2, 5558, 12,

    XREF_CMcpxBuffer_Play2,
    XRefZero)

        { 0x00, 0x55 },
        { 0x01, 0x8B },
        { 0x02, 0xEC },
        { 0x18, 0x0C },
        { 0x19, 0xFF },
        { 0x1A, 0x75 },
        { 0x1B, 0x08 },
        { 0x1C, 0x6A },
        { 0x1D, 0x03 },
        { 0x1E, 0xFF },
        { 0x1F, 0x50 },
        { 0x36, 0xC2 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::Play
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_Play, 5558, 9,

    XREF_CDirectSoundBuffer_Play,
    XRefZero) // TODO : Use XRefOne here if XREF_CDirectSoundBuffer_Play is enabled below

        //XREF_ENTRY( 0x35, XREF_CDirectSoundBuffer_Play ),

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

#if 0 // Replaced with generic OOVPA 3911
// ******************************************************************
// * IDirectSoundBuffer_Play
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_Play, 5558, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_CDirectSoundBuffer_Play ),

        { 0x03, 0x10 },
        { 0x08, 0xFF },
        { 0x0D, 0xC8 },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;
#endif

// ******************************************************************
// * CMcpxBuffer_GetStatus
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_GetStatus, 5558, 11,

	XREF_CMcpxBuffer_GetStatus,
    XRefZero)

        // CMcpxBuffer_GetStatus+0x04 : mov ecx, eax
        { 0x04, 0x8B },
        { 0x05, 0xC8 },

        // CMcpxBuffer_GetStatus+0x0C : jnz +0x1B
        { 0x0C, 0x75 },
        { 0x0D, 0x1B },

        // CMcpxBuffer_GetStatus+0x12 : test ax, 0x444
        { 0x12, 0x66 },
        { 0x13, 0xA9 },
        { 0x14, 0x44 },

        // CMcpxBuffer_GetStatus+0x29 : mov ecx, 0x8001
        { 0x29, 0xB9 },
        { 0x2A, 0x01 },
        { 0x2B, 0x80 },
        { 0x2C, 0x00 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetFormat
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetFormat, 5558, 7,

    XREF_CDirectSoundVoice_SetFormat,
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

    XREF_CDirectSoundBuffer_SetFormat,
    XRefOne)

        XREF_ENTRY( 0x32, XREF_CDirectSoundVoice_SetFormat ),

        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0x10 },
        { 0x3A, 0x74 },
        { 0x47, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetFormat
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetFormat, 5558, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_CDirectSoundBuffer_SetFormat ),

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

    XREF_CDirectSoundVoice_SetConeAngles,
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

    XREF_CDirectSoundBuffer_SetConeAngles,
    XRefOne)

        XREF_ENTRY( 0x3A, XREF_CDirectSoundVoice_SetConeAngles ),

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x74 },
        { 0x35, 0xFF },
        { 0x40, 0x8B },
        { 0x4F, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetConeAngles
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetConeAngles, 5558, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_CDirectSoundBuffer_SetConeAngles ),

        { 0x03, 0x10 },
        { 0x08, 0xFF },
        { 0x0D, 0xC8 },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetMinDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMinDistance, 5558, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x3E, XREF_DSVOICESETROLLOFFCURVE ),

        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x24 },
        { 0x3A, 0x1C },
        { 0x46, 0x74 },
        { 0x53, 0x8B },
OOVPA_END;
#if 0 // Duplicate OOVPA, replacing with generic OOVPA 3911
// ******************************************************************
// * IDirectSoundBuffer_SetMinDistance
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMinDistance, 5558, 11,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetMinDistance+0x1C : call [CDirectSoundBuffer::SetMinDistance]
        XREF_ENTRY( 0x1D, XREF_CDirectSoundBuffer_SetMinDistance ),

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
#endif
// ******************************************************************
// * CDirectSoundVoice::CommitDeferredSettings
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_CommitDeferredSettings, 5558, 6,

    XREF_CDirectSoundVoice_CommitDeferredSettings,
    XRefZero)

        { 0x01, 0x44 },
        { 0x04, 0x8B },
        { 0x07, 0xE8 },
        { 0x0C, 0x33 },
        { 0x0D, 0xC0 },
        { 0x10, 0x00 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetDopplerFactor
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetDopplerFactor, 5558, 20,

	XREF_CDirectSoundVoice_SetDopplerFactor,
    XRefOne)

        XREF_ENTRY( 0x2A, XREF_CDirectSoundVoice_CommitDeferredSettings ),

        { 0x00, 0x8B },
        { 0x01, 0x4c },
        { 0x02, 0x24 },
        { 0x03, 0x04 },
		{ 0x05, 0x41 },
		{ 0x0C, 0x00 },
		{ 0x11, 0x89 },
        { 0x12, 0x50 },
        { 0x13, 0x4C },
        { 0x14, 0x8B },
        { 0x15, 0x41 },
        { 0x16, 0x10 },
		{ 0x1A, 0x00 },
		{ 0x21, 0xF6 },
		{ 0x28, 0x51 },
		{ 0x2F, 0xC0 },
        { 0x30, 0xC2 },
        { 0x31, 0x0C },
        { 0x32, 0x00 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8::SetAllParameters
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetAllParameters, 5558, 9,

	XREF_CDirectSoundVoice_SetAllParameters,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_CDirectSoundBuffer_SetAllParameters ),

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

    XREF_CDirectSoundVoice_SetPosition,
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

    XREF_CDirectSoundBuffer_SetPosition,
    XRefOne)

        XREF_ENTRY( 0x4A, XREF_CDirectSoundVoice_SetPosition ),

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
OOVPA_XREF(IDirectSoundBuffer_SetPosition, 5558, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x2D, XREF_CDirectSoundBuffer_SetPosition ),

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

    XREF_CDirectSoundVoice_SetVelocity,
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

    XREF_CDirectSoundBuffer_SetVelocity,
    XRefOne)

        XREF_ENTRY( 0x4A, XREF_CDirectSoundVoice_SetVelocity ),

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
OOVPA_XREF(IDirectSoundBuffer_SetVelocity, 5558, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x2D, XREF_CDirectSoundBuffer_SetVelocity ),

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
OOVPA_XREF(CMcpxStream_Pause, 5558, 7,

    XREF_CMcpxStream_Pause,
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

#if 0 // Replaced with generic OOVPA 4134
// ******************************************************************
// * CDirectSoundStream::Pause
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_Pause, 5558, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x35, XREF_CMcpxStream_Pause ),

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x39, 0x85 },
        { 0x44, 0xFF },
        { 0x4B, 0xC7 },
OOVPA_END;
#endif

// ******************************************************************
// * CDirectSoundVoice::SetDistanceFactor
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetDistanceFactor, 5558, 12,

    XREF_DSVOICESETDISTANCEFACTOR,
    XRefZero)

        { 0x05, 0x41 },
        { 0x0C, 0x00 },
        { 0x10, 0x08 },
        { 0x11, 0x89 },
        { 0x12, 0x50 },
        { 0x13, 0x44 },
        { 0x14, 0x8B },
        { 0x1D, 0x80 },
        { 0x1F, 0x02 },
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

        XREF_ENTRY( 0x3A, XREF_DSVOICESETDISTANCEFACTOR ),

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xFF },
        { 0x40, 0x8B },
        { 0x4F, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetDistanceFactor
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetDistanceFactor, 5558, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_DSBUFFERSETDISTANCEFACTOR ),

        { 0x03, 0x0C },
        { 0x08, 0x8B },
        { 0x0D, 0x8B },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetDopplerFactor
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetDopplerFactor, 5558, 8,

    XREF_DSBUFFERSETDOPPLERFACTOR,
    XRefOne)

        XREF_ENTRY( 0x3A, XREF_CDirectSoundVoice_SetDopplerFactor),

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
OOVPA_XREF(IDirectSoundBuffer_SetDopplerFactor, 5558, 8,

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
OOVPA_XREF(CDirectSoundVoice_SetI3DL2Source, 5558, 7,

    XREF_CDirectSoundVoice_SetI3DL2Source,
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
OOVPA_XREF(CDirectSoundBuffer_SetI3DL2Source, 5558, 8,

    XREF_CDirectSoundBuffer_SetI3DL2Source,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_SetI3DL2Source ),

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

        XREF_ENTRY( 0x19, XREF_CDirectSoundBuffer_SetI3DL2Source ),

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
OOVPA_XREF(CDirectSoundVoice_SetAllParameters, 5558, 10,

    XREF_CDirectSoundVoice_SetAllParameters,
    XRefZero) // TODO : Use XRefOne here if XREF_CDirectSoundVoice_CommitDeferredSettings is enabled below

//        XREF_ENTRY( 0x12F, XREF_CDirectSoundVoice_CommitDeferredSettings ),

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
// * DirectSound::CDirectSoundVoice::SetHeadroom
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetHeadroom, 5558, 7,

    XREF_CDirectSoundVoice_SetHeadroom,
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
OOVPA_XREF(CDirectSoundBuffer_SetHeadroom, 5558, 8,

    XREF_CDirectSoundBuffer_SetHeadroom,
    XRefOne)

        XREF_ENTRY( 0x32, XREF_CDirectSoundVoice_SetHeadroom ),

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
OOVPA_XREF(IDirectSoundBuffer_SetHeadroom, 5558, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_CDirectSoundBuffer_SetHeadroom ),

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
OOVPA_XREF(CDirectSoundBuffer_GetCurrentPosition, 5558, 7,

	XREF_CDirectSoundBuffer_GetCurrentPosition,
    XRefZero)

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0x74 },
        { 0x40, 0xF8 },
        { 0x4E, 0x8B },
OOVPA_END;
#if 0 //Replaced with generic 3911
// ******************************************************************
// * IDirectSoundBuffer_GetCurrentPosition@12
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_GetCurrentPosition, 5558, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_CDirectSoundBuffer_GetCurrentPosition),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0xFF },
        { 0x0E, 0x83 },
        { 0x12, 0xD9 },
        { 0x16, 0xC8 },
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
OOVPA_END;
#endif

#if 0 // Replaced with generic OOVPA 4134
// ******************************************************************
// * DirectSound::CDirectSoundStream::SetMixBins
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMixBins, 5558, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_SetMixBins ),

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
OOVPA_END;
#endif

// ******************************************************************
// * DirectSound::CDirectSound::SetMixBinHeadroom
// ******************************************************************
OOVPA_XREF(CDirectSound_SetMixBinHeadroom, 5558, 8,

    XREF_CDirectSound_SetMixBinHeadroom,
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
// * DirectSound::CDirectSound::EnableHeadphones
// ******************************************************************
OOVPA_XREF(CDirectSound_EnableHeadphones, 5558, 8,

    XREF_CDirectSound_EnableHeadphones,
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
#if 0 // Replaced with generic OOVPA 3911
// ******************************************************************
// * IDirectSound_EnableHeadphones
// ******************************************************************
OOVPA_XREF(IDirectSound_EnableHeadphones, 5558, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_CDirectSound_EnableHeadphones ),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;
#endif
// ******************************************************************
// * DirectSound::CDirectSound::GetOutputLevels
// ******************************************************************
OOVPA_XREF(CDirectSound_GetOutputLevels, 5558, 7,

    XREF_CDirectSound_GetOutputLevels,
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

        XREF_ENTRY( 0x19, XREF_CDirectSound_GetOutputLevels ),

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
OOVPA_XREF(CDirectSound_SetPosition, 5558, 7,

    XREF_CDirectSound_SetPosition,
    XRefZero)

        { 0x0F, 0x00 },
        { 0x1E, 0xFF },
        { 0x2B, 0x8B },
        { 0x3A, 0x8B },
        { 0x49, 0x40 },
        { 0x58, 0xE8 },
        { 0x67, 0x15 },
OOVPA_END;
#if 0 // Replaced with generic OOVPA 3911
// ******************************************************************
// * IDirectSound_SetPosition
// ******************************************************************
OOVPA_XREF(IDirectSound_SetPosition, 5558, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x2D, XREF_CDirectSound_SetPosition ),

        { 0x05, 0x18 },
        { 0x0C, 0x83 },
        { 0x13, 0x8B },
        { 0x1A, 0xF8 },
        { 0x21, 0xD9 },
        { 0x28, 0xD9 },
        { 0x31, 0x5D },
OOVPA_END;
#endif
// ******************************************************************
// * DirectSound::CDirectSound::SetVelocity
// ******************************************************************
OOVPA_XREF(CDirectSound_SetVelocity, 5558, 7,

    XREF_CDirectSound_SetVelocity,
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

        XREF_ENTRY( 0x2D, XREF_CDirectSound_SetVelocity ),

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
OOVPA_XREF(CDirectSound_CreateSoundStream, 5558, 7,

    XREF_CDirectSound_CreateSoundStream,
    XRefZero)

        { 0x11, 0xD8 },
        { 0x24, 0x05 },
        { 0x37, 0x74 },
        { 0x4A, 0x81 },
        { 0x5D, 0xE8 },
        { 0x70, 0x85 },
        { 0x83, 0xFF },
OOVPA_END;
#if 0 //Replaced with generic OOVPA 3911
// ******************************************************************
// * IDirectSound_CreateSoundStream
// ******************************************************************
OOVPA_XREF(IDirectSound_CreateSoundStream, 5558, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_CDirectSound_CreateSoundStream ),

        { 0x03, 0x10 },
        { 0x08, 0xFF },
        { 0x0D, 0xC8 },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;
#endif
// ******************************************************************
// * DirectSound::CDirectSoundStream::SetRolloffCurve
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetRolloffCurve, 5558, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x3E, XREF_DSVOICESETROLLOFFCURVE ),

        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x74 },
        { 0x3A, 0x24 },
        { 0x46, 0x74 },
        { 0x53, 0x8B },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSound::SetAllParameters
// ******************************************************************
OOVPA_XREF(CDirectSound_SetAllParameters, 5558, 8,

    XREF_CDirectSound_SetAllParameters,
    XRefZero)

        { 0x0C, 0x00 },
        { 0x0E, 0xB6 },
        { 0x10, 0x74 },
        { 0x12, 0x85 },
        { 0x14, 0x74 },
        { 0x16, 0x68 },
        { 0x2A, 0x00 },
        { 0x2B, 0x8B },
OOVPA_END;

#if 0 // Replaced with generic OOVPA 4134, this OOVPA is not a reliable signature
// ******************************************************************
// * DirectSound::CMcpxVoiceClient::SetFilter
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_SetFilter, 5558, 10,

    XREF_CMcpxVoiceClient_SetFilter,
    XRefZero)

        { 0x05, 0x20 },
        { 0x07, 0x65 },
        { 0x09, 0x00 },
        { 0x0B, 0x56 },
        { 0x0F, 0x8D },
        { 0x17, 0x8B },
        { 0x19, 0x08 },
        { 0x1B, 0xF6 },
        { 0x1D, 0xFF },
        { 0x1F, 0x00 },
OOVPA_END;
#endif

// ******************************************************************
// * DirectSound::CDirectSoundVoice::Use3DVoiceData
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_Use3DVoiceData, 5558, 9,

    XREF_CDirectSoundVoice_Use3DVoiceData,
    XRefZero)

        { 0x04, 0x00 },
        { 0x07, 0x24 },
        { 0x0B, 0x10 },
        { 0x0F, 0x48 },
        { 0x12, 0xEB },
        { 0x16, 0x0B },
        { 0x18, 0x33 },
        { 0x1A, 0xC2 },
        { 0x1B, 0x08 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::Use3DVoiceData
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_Use3DVoiceData, 5558, 9,

    XREF_CDirectSoundBuffer_Use3DVoiceData,
    XRefOne)

        XREF_ENTRY( 0x31, XREF_CDirectSoundVoice_Use3DVoiceData ),

        { 0x0C, 0x0F },
        { 0x10, 0x16 },
        { 0x15, 0x68 },
        { 0x36, 0xC9 },
        { 0x39, 0x74 },
        { 0x3B, 0x68 },
        { 0x49, 0xC2 },
        { 0x4A, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer::Use3DVoiceData
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_Use3DVoiceData, 5558, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_CDirectSoundBuffer_Use3DVoiceData ),

        { 0x06, 0x24 },
        { 0x08, 0x8B },
        { 0x0C, 0xE4 },
        { 0x0E, 0xD9 },
        { 0x11, 0x23 },
        { 0x14, 0xE8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundStream::Use3DVoiceData
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_Use3DVoiceData, 5558, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x35, XREF_CDirectSoundVoice_Use3DVoiceData ),

        { 0x0C, 0x0F },
        { 0x10, 0x16 },
        { 0x15, 0x68 },
        { 0x3A, 0xC9 },
        { 0x3D, 0x74 },
        { 0x3F, 0x68 },
        { 0x4D, 0xC2 },
        { 0x4E, 0x08 },
OOVPA_END;
