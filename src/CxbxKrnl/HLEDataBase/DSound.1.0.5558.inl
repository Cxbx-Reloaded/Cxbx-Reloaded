// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->DSound.1.0.5558.inl
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

#if 0 // Replaced with generic OOVPA 5455
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
#endif

// ******************************************************************
// * IDirectSound_SetMixBinHeadroom
// ******************************************************************
OOVPA_XREF(IDirectSound_SetMixBinHeadroom, 5558, 1+8,

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
OOVPA_XREF(CHRTFSource_SetFullHRTF5Channel, 5558, 1+8,

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
OOVPA_XREF(DirectSoundUseFullHRTF, 5558, 1+7,

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

#if 0 // Replaced with better and stronger OOVPA 4134
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
#endif

#if 0 // Replaced with generic OOVPA 3911
// ******************************************************************
// * IDirectSoundBuffer_SetCurrentPosition
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetCurrentPosition, 5558, 1+7,

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

#if 0 // Replaced with generic OOVPA 4039
// ******************************************************************
// * IDirectSoundBuffer_SetMixBins
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMixBins, 5558, 1+7,

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

#if 0 // Replaced with generic OOVPA 3911
// ******************************************************************
// * IDirectSoundBuffer_Play
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_Play, 5558, 1+7,

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

#if 0 // Replaced with OOVPA 4831
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
#endif

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetMinDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMinDistance, 5558, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x3E, XREF_CDirectSoundVoice_SetRolloffCurve ),

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
OOVPA_XREF(IDirectSoundBuffer_SetMinDistance, 5558, 1+10,

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
// * IDirectSoundBuffer8::SetAllParameters
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetAllParameters, 5558, 1+8,

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
OOVPA_XREF(CDirectSoundStream_Pause, 5558, 1+7,

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

#if 0 //Replaced with generic 3911
// ******************************************************************
// * IDirectSoundBuffer_GetCurrentPosition@12
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_GetCurrentPosition, 5558, 1+8,

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
OOVPA_XREF(CDirectSoundStream_SetMixBins, 5558, 1+8,

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

#if 0 // Replaced with generic OOVPA 3911
// ******************************************************************
// * IDirectSound_EnableHeadphones
// ******************************************************************
OOVPA_XREF(IDirectSound_EnableHeadphones, 5558, 1+7,

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

#if 0 // Replaced with generic OOVPA 3911
// ******************************************************************
// * IDirectSound_SetPosition
// ******************************************************************
OOVPA_XREF(IDirectSound_SetPosition, 5558, 1+7,

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

#if 0 //Replaced with generic OOVPA 3911
// ******************************************************************
// * IDirectSound_CreateSoundStream
// ******************************************************************
OOVPA_XREF(IDirectSound_CreateSoundStream, 5558, 1+7,

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
OOVPA_XREF(CDirectSoundBuffer_Use3DVoiceData, 5558, 1+8,

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
OOVPA_XREF(IDirectSoundBuffer_Use3DVoiceData, 5558, 1+8,

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
OOVPA_XREF(CDirectSoundStream_Use3DVoiceData, 5558, 1+8,

    XREF_CDirectSoundStream_Use3DVoiceData,
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

// ******************************************************************
// * IDirectSoundStream_Use3DVoiceData
// ******************************************************************
// Generic OOVPA as of ____? and newer (note: is not introduced in 5344)
OOVPA_XREF(IDirectSoundStream_Use3DVoiceData, 5558, 1+1,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundStream_Use3DVoiceData+0x00 : jmp [CDirectSoundStream_Use3DVoiceData]
        XREF_ENTRY(0x01, XREF_CDirectSoundStream_Use3DVoiceData),

        // IDirectSoundStream_Use3DVoiceData+0x00 : jmp 0x........
        { 0x00, 0xE9 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoice::Set3DVoiceData
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_Set3DVoiceData, 5558, 9,

    XREF_CDirectSoundVoice_Set3DVoiceData,
    XRefZero)

        { 0x00, 0x8B },
        { 0x04, 0x8B },

        { 0x28, 0x01 },
        { 0x3C, 0x02 },
        { 0x50, 0x04 },
        { 0x73, 0x08 },
        { 0x96, 0x10 },
        { 0xB9, 0x20 },
        { 0xCD, 0x40 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::Set3DVoiceData
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_Set3DVoiceData, 5558, 1+8,

    XREF_CDirectSoundBuffer_Set3DVoiceData,
    XRefOne)

        XREF_ENTRY( 0x32, XREF_CDirectSoundVoice_Set3DVoiceData ),

        { 0x00, 0x56 },
        { 0x11, 0x16 },
        { 0x16, 0x68 },
        { 0x36, 0x85 },
        { 0x3A, 0x74 },
        { 0x3C, 0x68 },
        { 0x4B, 0xC2 },
        { 0x4C, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer::Set3DVoiceData
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_Set3DVoiceData, 5558, 1+8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_CDirectSoundBuffer_Set3DVoiceData ),

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
// * DirectSound::CDirectSoundStream::Set3DVoiceData
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_Set3DVoiceData, 5558, 1+8,

    XREF_CDirectSoundStream_Set3DVoiceData,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_Set3DVoiceData ),

        { 0x00, 0x56 },
        { 0x11, 0x16 },
        { 0x16, 0x68 },
        { 0x35, 0xE8 },
        { 0x3E, 0x74 },
        { 0x40, 0x68 },
        { 0x4F, 0xC2 },
        { 0x50, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundStream_Set3DVoiceData
// ******************************************************************
// Generic OOVPA as of ____? and newer (note: is not introduced in 5344)
OOVPA_XREF(IDirectSoundStream_Set3DVoiceData, 5558, 1+1,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundStream_Set3DVoiceData+0x00 : jmp [CDirectSoundStream_Set3DVoiceData]
        XREF_ENTRY(0x01, XREF_CDirectSoundStream_Set3DVoiceData),

        // IDirectSoundStream_Set3DVoiceData+0x00 : jmp 0x........
        { 0x00, 0xE9 },
OOVPA_END;
