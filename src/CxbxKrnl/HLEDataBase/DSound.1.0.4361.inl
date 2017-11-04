// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->DSound.1.0.4361.inl
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
// * DirectSoundGetSampleTime
// ******************************************************************
OOVPA_NO_XREF(DirectSoundGetSampleTime, 4361, 8)

        // DirectSoundGetSampleTime+0x00 : xor eax, eax
        { 0x00, 0x33 },
        { 0x01, 0xC0 },

        // DirectSoundGetSampleTime+0x0A : mov eax, ds:0FE80200Ch
        { 0x0A, 0xA1 },
        { 0x0B, 0x0C },
        { 0x0C, 0x20 },
        { 0x0D, 0x80 },
        { 0x0E, 0xFE },

        // DirectSoundGetSampleTime+0x0F : retn
        { 0x0F, 0xC3 },
OOVPA_END;

#if 0 // Accurately CDirectSound_CreateSoundBuffer used 4134
// ******************************************************************
// * IDirectSound_CreateSoundBuffer
// ******************************************************************
OOVPA_NO_XREF(IDirectSound_CreateSoundBuffer, 4361, 9)

        // IDirectSound_CreateSoundBuffer+0x2A : push 0x24
        { 0x2A, 0x6A },
        { 0x2B, 0x24 },

        // IDirectSound_CreateSoundBuffer+0x3C : call [addr]
        { 0x3C, 0xE8 },

        // IDirectSound_CreateSoundBuffer+0x4A : and esi, 0x7FF8FFF2
        { 0x4A, 0x81 },
        { 0x4B, 0xE6 },
        { 0x4C, 0xF2 },
        { 0x4D, 0xFF },
        { 0x4E, 0xF8 },
        { 0x4F, 0x7F },
OOVPA_END;
#endif

#if 0 // Replaced with OOVPA 4134
// ******************************************************************
// * CMcpxStream_Pause
// ******************************************************************
OOVPA_XREF(CMcpxStream_Pause, 4361, 11,

    XREF_CMcpxStream_Pause,
    XRefZero)

        // CMcpxStream_Pause+0x1E : or eax, 4
        { 0x1E, 0x83 },
        { 0x1F, 0xC8 },
        { 0x20, 0x04 },

        // CMcpxStream_Pause+0x21 : jmp +0x0D
        { 0x21, 0xEB },
        { 0x22, 0x0D },

        // CMcpxStream_Pause+0x29 : movzx eax, word ptr [esi+0x12]
        { 0x29, 0x0F },
        { 0x2A, 0xB7 },
        { 0x2B, 0x46 },
        { 0x2C, 0x12 },

        // CMcpxStream_Pause+0x44 : retn 0x04
        { 0x44, 0xC2 },
        { 0x45, 0x04 },
OOVPA_END;
#endif

#if 0 // Replaced with generic OOVPA 4134
// ******************************************************************
// * CDirectSoundStream_Pause
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_Pause, 4361, 1+10,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_Pause+0x35 : call [CMcpxStream::Pause]
        XREF_ENTRY( 0x35, XREF_CMcpxStream_Pause ),

        // CDirectSoundStream_Pause+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 },
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // CDirectSoundStream_Pause+0x2C : mov ecx, [eax+0x24]
        { 0x2C, 0x8B },
        { 0x2D, 0x48 },
        { 0x2E, 0x24 },

        // CDirectSoundStream_Pause+0x4E : retn 0x08
        { 0x4E, 0xC2 },
        { 0x4F, 0x08 },
OOVPA_END;
#endif

#if 0 // Replaced with generic OOVPA 4039
// ******************************************************************
// * IDirectSoundBuffer_SetPlayRegion
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetPlayRegion, 4361, 1+8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_CDirectSoundBuffer_SetPlayRegion ),

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

// ******************************************************************
// * public: long __thiscall DirectSound::CMcpxBuffer::Play(unsigned long)
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_Play, 4361, 11,

    XREF_CMcpxBuffer_Play,
    XRefZero)


        // CMcpxBuffer_Play+0x0E : cmp ebx, edi
        { 0x0E, 0x3B },
        { 0x0F, 0xDF },

        // CMcpxBuffer_Play+0x1C : jz +0x09
        { 0x1C, 0x74 },
        { 0x1D, 0x09 },

        // CMcpxBuffer_Play+0x30 : test byte ptr [esi+0x12], 2
        { 0x30, 0xF6 },
        { 0x31, 0x46 },
        { 0x32, 0x12 },
        { 0x33, 0x02 },

        // CMcpxBuffer_Play+0x71 : call dword ptr [eax+0x18]
        { 0x71, 0xFF },
        { 0x72, 0x50 },
        { 0x73, 0x18 },
OOVPA_END;

#if 0 // Replaced with generic OOVPA 3911
// ******************************************************************
// * IDirectSoundBuffer_Play
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_Play, 4361, 1+8,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_Play+0x1D : call [CDirectSoundBuffer::Play]
        XREF_ENTRY( 0x1D, XREF_CDirectSoundBuffer_Play ), 

        // IDirectSoundBuffer_Play+0x0E : push [esp+0x10]
        { 0x0E, 0xFF },
        { 0x0F, 0x74 },
        { 0x10, 0x24 },
        { 0x11, 0x10 },

        // IDirectSoundBuffer_Play+0x17 : sbb ecx, ecx
        { 0x17, 0x1B },
        { 0x18, 0xC9 },

        // IDirectSoundBuffer_Play+0x19 : and ecx, eax
        { 0x19, 0x23 },
        { 0x1A, 0xC8 },
OOVPA_END;
#endif

#if 0 // Moved to 4242
// ******************************************************************
// * CMcpxBuffer_Stop
// ******************************************************************

OOVPA_XREF(CMcpxBuffer_Stop, 4361, 9,

	XREF_CMcpxBuffer_Stop,
    XRefZero)

        // CMcpxBuffer_Stop+0x0E : cmp al, 3
        { 0x0E, 0x3C },
        { 0x0F, 0x03 },

        // CMcpxBuffer_Stop+0x17 : jz +0x2A
        { 0x17, 0x74 },
        { 0x18, 0x2A },

        // CMcpxBuffer_Stop+0x19 : test [esp+0Ch+arg_0], 2
        { 0x19, 0xF6 },
        { 0x1A, 0x44 },
        { 0x1B, 0x24 },
        { 0x1C, 0x10 },
        { 0x1D, 0x02 },
OOVPA_END;
#endif
#if 0 // Moved to 4134
// ******************************************************************
// * CDirectSoundBuffer_Stop
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_Stop, 4361, 1+9,

    XREF_CDirectSoundBuffer_Stop,
    XRefOne)

        // CDirectSoundBuffer_Stop+0x33 : call [CMcpxBuffer::Stop]
        XREF_ENTRY( 0x33, XREF_CMcpxBuffer_Stop), 

        // CDirectSoundBuffer_Stop+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 },
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // CDirectSoundBuffer_Stop+0x3B : jz +0x0B
        { 0x3B, 0x74 },
        { 0x3C, 0x0B },

        // CDirectSoundBuffer_Stop+0x4C : retn 0x04
        { 0x4C, 0xC2 },
        { 0x4D, 0x04 },
OOVPA_END;
#endif
#if 0 //Using generic 3911
// ******************************************************************
// * IDirectSoundBuffer_Stop
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_Stop, 4361, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_Stop+0x11 : call [CDirectSoundBuffer::Stop]
        XREF_ENTRY( 0x11, XREF_CDirectSoundBuffer_Stop), 

        // IDirectSoundBuffer_Stop+0x06 : and eax, 0xFFFFFFE4
        { 0x06, 0x83 },
        { 0x07, 0xC0 },
        { 0x08, 0xE4 },

        // IDirectSoundBuffer_Stop+0x0B : sbb ecx, ecx
        { 0x0B, 0x1B },
        { 0x0C, 0xC9 },

        // IDirectSoundBuffer_Stop+0x0D : and ecx, eax
        { 0x0D, 0x23 },
        { 0x0E, 0xC8 },
OOVPA_END;
#endif
#if 0 // Used 4134
// ******************************************************************
// * DirectSound::CDirectSoundVoiceSettings::SetMixBinVolumes
// ******************************************************************
OOVPA_XREF(CDirectSoundVoiceSettings_SetMixBinVolumes, 4361, 7,

    XREF_CDirectSoundVoiceSettings_SetMixBinVolumes,
    XRefZero)

        { 0x03, 0x24 },
        { 0x08, 0x16 },
        { 0x0D, 0x46 },
        { 0x12, 0x8B },
        { 0x17, 0x42 },
        { 0x1C, 0x3B },
        { 0x21, 0x5E },
OOVPA_END;
#endif
#if 0 // Used 4134
// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetMixBinVolumes
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMixBinVolumes, 4361, 1+7,

    XREF_CDirectSoundVoice_SetMixBinVolumes,
    XRefOne)

        XREF_ENTRY( 0x0D, XREF_CDirectSoundVoiceSettings_SetMixBinVolumes ),

        { 0x02, 0x74 },
        { 0x06, 0x74 },
        { 0x0A, 0x4E },
        { 0x11, 0x8B },
        { 0x12, 0x4E },
        { 0x19, 0x5E },
        { 0x1A, 0xC2 },
OOVPA_END;
#endif
#if 0 // Used 4134
// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetMixBinVolumes
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetMixBinVolumes, 4361, 1+7,

    XREF_CDirectSoundBuffer_SetMixBinVolumes,
    XRefOne)

        XREF_ENTRY( 0x32, XREF_CDirectSoundVoice_SetMixBinVolumes ),

        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0x10 },
        { 0x3A, 0x74 },
        { 0x47, 0x8B },
OOVPA_END;
#endif
#if 0 // Moved to 4134
// ******************************************************************
// * DirectSound::CDirectSoundBuffer::PlayEx
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_PlayEx, 4361, 1+7,

    XREF_CDirectSoundBuffer_PlayEx,
    XRefOne)

        XREF_ENTRY( 0x3D, XREF_CMcpxBuffer_Play2),

        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x20 },
        { 0x3A, 0x24 },
        { 0x46, 0x0B },
        { 0x52, 0x8B },
OOVPA_END;
#endif

// ******************************************************************
// * CMcpxBuffer::Stop2
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_Stop2, 4361, 1+9,

    XREF_CMcpxBuffer_Stop2,
    XRefOne)

        XREF_ENTRY( 0x2B, XREF_CMcpxBuffer_Stop),

        // CMcpxBuffer_Stop2+0x03 : mov eax, [ebp+arg_0]
        { 0x03, 0x8B },
        { 0x04, 0x45 },
        { 0x05, 0x08 },
        // CMcpxBuffer_Stop2+0x14 : mov eax, [esi]
        { 0x14, 0x8B },
        { 0x15, 0x06 },
        // CMcpxBuffer_Stop2+0x25 : mov edi, eax
        { 0x2F, 0x8B },
        { 0x30, 0xF8 },
        // CMcpxBuffer_Stop2+0x36 : retn 0xC
        { 0x36, 0xC2 },
        { 0x37, 0x0C },
OOVPA_END;

#if 0 // Moved to 4134
// ******************************************************************
// * DirectSound::CDirectSoundBuffer::StopEx
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_StopEx, 4361, 1+7,

    XREF_CDirectSoundBuffer_StopEx,
    XRefOne)

        XREF_ENTRY( 0x3D, XREF_CMcpxBuffer_Stop2 ),

        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x20 },
        { 0x3A, 0x24 },
        { 0x46, 0x0B },
        { 0x52, 0x8B },
OOVPA_END;
#endif
#if 0 // Moved to 3911
// ******************************************************************
// * IDirectSoundBuffer_StopEx
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_StopEx, 4361, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_CDirectSoundBuffer_StopEx ),

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
// * public: long __thiscall DirectSound::CMcpxBuffer::Play(__int64,unsigned long)
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_Play2, 4361, 8,

    XREF_CMcpxBuffer_Play2,
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

#if 0 // For research purpose, need to strengthen it up.
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
#endif

// ******************************************************************
// * CDirectSoundVoice::SetMaxDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMaxDistance, 4361, 12,

    XREF_CDirectSoundVoice_SetMaxDistance,
    XRefZero)

        // CDirectSoundVoice_SetMaxDistance+0x0D : mov edx, [esp+arg_4]
        { 0x0D, 0x8B },
        { 0x0E, 0x54 },
        { 0x0F, 0x24 },
        { 0x10, 0x08 },

        // CDirectSoundVoice_SetMaxDistance+0x11 : mov [eax+38h], edx
        { 0x11, 0x89 },
        { 0x12, 0x50 },
        { 0x13, 0x38 },

        { 0x14, 0x8B },
        { 0x1D, 0x83 },
        { 0x1F, 0x78 },

        // CDirectSoundVoice_SetMaxDistance+0x30 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 }
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetMinDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMinDistance, 4361, 12,

    XREF_CDirectSoundVoice_SetMinDistance,
    XRefZero)

        // CDirectSoundVoice_SetMinDistance+0x0D : mov edx, [esp+arg_4]
        { 0x0D, 0x8B },
        { 0x0E, 0x54 },
        { 0x0F, 0x24 },
        { 0x10, 0x08 },

        // CDirectSoundVoice_SetMinDistance+0x11 : mov [eax+34h], edx
        { 0x11, 0x89 },
        { 0x12, 0x50 },
        { 0x13, 0x34 },

        { 0x14, 0x8B },
        { 0x1D, 0x83 },
        { 0x1F, 0x78 },

        // CDirectSoundVoice_SetMinDistance+0x30 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 }
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetRolloffFactor
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetRolloffFactor, 4361, 12,

    XREF_CDirectSoundVoice_SetRolloffFactor,
    XRefZero)

        // CDirectSoundVoice_SetRolloffFactor+0x0D : mov edx, [esp+arg_4]
        { 0x0D, 0x8B },
        { 0x0E, 0x54 },
        { 0x0F, 0x24 },
        { 0x10, 0x08 },

        // CDirectSoundVoice_SetRolloffFactor+0x11 : mov [eax+44h], edx
        { 0x11, 0x89 },
        { 0x12, 0x50 },
        { 0x13, 0x44 },

        { 0x14, 0x8B },
        { 0x1D, 0x83 },
        { 0x1F, 0x78 },

        // CDirectSoundVoice_SetRolloffFactor+0x30 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 }
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetConeOutsideVolume
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetConeOutsideVolume, 4361, 12,

    XREF_CDirectSoundVoice_SetConeOutsideVolume,
    XRefZero)

        // CDirectSoundVoice_SetConeOutsideVolume+0x0D : mov edx, [esp+arg_4]
        { 0x0D, 0x8B },
        { 0x0E, 0x54 },
        { 0x0F, 0x24 },
        { 0x10, 0x08 },

        // CDirectSoundVoice_SetConeOutsideVolume+0x11 : mov [eax+30h], edx
        { 0x11, 0x89 },
        { 0x12, 0x50 },
        { 0x13, 0x30 },

        { 0x14, 0x8B },
        { 0x1D, 0x83 },
        { 0x1F, 0x78 },

        // CDirectSoundVoice_SetConeOutsideVolume+0x30 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 }
OOVPA_END;

// ******************************************************************
// * CMcpxStream_Flush
// ******************************************************************
OOVPA_XREF(CMcpxStream_Flush, 4361, 10,
    XREF_CMcpxStream_Flush,
    XRefZero)

        { 0x00, 0x55 },
        { 0x01, 0x8B },

        { 0x12, 0x33 },

        { 0x3D, 0x83 },
        { 0x3E, 0xFE },
        { 0x3F, 0x06 }, //4242 only has 0x05

        { 0x56, 0xE8 },

        { 0x67, 0xE8 },

        // Might not be a requirement? Aka comment this out might will enable support detection later XDK revisions.
        { 0xD1, 0xC9 },
        { 0xD2, 0xC3 },
OOVPA_END;
