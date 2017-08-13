// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->DSound.1.0.4361.cpp
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
        { 0x00, 0x33 }, // (Offset,Value)-Pair #1
        { 0x01, 0xC0 }, // (Offset,Value)-Pair #2

        // DirectSoundGetSampleTime+0x0A : mov eax, ds:0FE80200Ch
        { 0x0A, 0xA1 }, // (Offset,Value)-Pair #3
        { 0x0B, 0x0C }, // (Offset,Value)-Pair #4
        { 0x0C, 0x20 }, // (Offset,Value)-Pair #5
        { 0x0D, 0x80 }, // (Offset,Value)-Pair #6
        { 0x0E, 0xFE }, // (Offset,Value)-Pair #7

        // DirectSoundGetSampleTime+0x0F : retn
        { 0x0F, 0xC3 }, // (Offset,Value)-Pair #8
OOVPA_END;

#if 0 // Accurately CDirectSound_CreateSoundBuffer used 4134
// ******************************************************************
// * IDirectSound_CreateSoundBuffer
// ******************************************************************
OOVPA_NO_XREF(IDirectSound_CreateSoundBuffer, 4361, 9)

        // IDirectSound_CreateSoundBuffer+0x2A : push 0x24
        { 0x2A, 0x6A }, // (Offset,Value)-Pair #1
        { 0x2B, 0x24 }, // (Offset,Value)-Pair #2

        // IDirectSound_CreateSoundBuffer+0x3C : call [addr]
        { 0x3C, 0xE8 }, // (Offset,Value)-Pair #3

        // IDirectSound_CreateSoundBuffer+0x4A : and esi, 0x7FF8FFF2
        { 0x4A, 0x81 }, // (Offset,Value)-Pair #4
        { 0x4B, 0xE6 }, // (Offset,Value)-Pair #5
        { 0x4C, 0xF2 }, // (Offset,Value)-Pair #6
        { 0x4D, 0xFF }, // (Offset,Value)-Pair #7
        { 0x4E, 0xF8 }, // (Offset,Value)-Pair #8
        { 0x4F, 0x7F }, // (Offset,Value)-Pair #9
OOVPA_END;
#endif

// ******************************************************************
// * CMcpxStream_Pause
// ******************************************************************
OOVPA_XREF(CMcpxStream_Pause, 4361, 11,

    XREF_DSSTREAMPAUSE,
    XRefZero)

        // CMcpxStream_Pause+0x1E : or eax, 4
        { 0x1E, 0x83 }, // (Offset,Value)-Pair #1
        { 0x1F, 0xC8 }, // (Offset,Value)-Pair #2
        { 0x20, 0x04 }, // (Offset,Value)-Pair #3

        // CMcpxStream_Pause+0x21 : jmp +0x0D
        { 0x21, 0xEB }, // (Offset,Value)-Pair #4
        { 0x22, 0x0D }, // (Offset,Value)-Pair #5

        // CMcpxStream_Pause+0x29 : movzx eax, word ptr [esi+0x12]
        { 0x29, 0x0F }, // (Offset,Value)-Pair #6
        { 0x2A, 0xB7 }, // (Offset,Value)-Pair #7
        { 0x2B, 0x46 }, // (Offset,Value)-Pair #8
        { 0x2C, 0x12 }, // (Offset,Value)-Pair #9

        // CMcpxStream_Pause+0x44 : retn 0x04
        { 0x44, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x45, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_Pause
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_Pause, 4361, 11,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_Pause+0x35 : call [CMcpxStream::Pause]
        XREF_ENTRY( 0x35, XREF_DSSTREAMPAUSE ), // (Offset,Value)-Pair #1

        // CDirectSoundStream_Pause+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 }, // (Offset,Value)-Pair #2
        { 0x22, 0x05 }, // (Offset,Value)-Pair #3
        { 0x23, 0x40 }, // (Offset,Value)-Pair #4
        { 0x24, 0x00 }, // (Offset,Value)-Pair #5
        { 0x25, 0x80 }, // (Offset,Value)-Pair #6

        // CDirectSoundStream_Pause+0x2C : mov ecx, [eax+0x24]
        { 0x2C, 0x8B }, // (Offset,Value)-Pair #7
        { 0x2D, 0x48 }, // (Offset,Value)-Pair #8
        { 0x2E, 0x24 }, // (Offset,Value)-Pair #9

        // CDirectSoundStream_Pause+0x4E : retn 0x08
        { 0x4E, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x4F, 0x08 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetPlayRegion
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetPlayRegion, 4361, 10,

    XREF_DSBUFFERSETPLAYREGIONA,
    XRefZero)

        // CDirectSoundBuffer_SetPlayRegion+0x10 : movzx edi, al
        { 0x10, 0x0F }, // (Offset,Value)-Pair #1
        { 0x11, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x12, 0xF8 }, // (Offset,Value)-Pair #3

        // CDirectSoundBuffer_SetPlayRegion+0x17 : jz +0x0B
        { 0x17, 0x74 }, // (Offset,Value)-Pair #4
        { 0x18, 0x0B }, // (Offset,Value)-Pair #5

        // CDirectSoundBuffer_SetPlayRegion+0x46 : mov esi, 0x88780032
        { 0x46, 0xBE }, // (Offset,Value)-Pair #6
        { 0x47, 0x32 }, // (Offset,Value)-Pair #7
        { 0x48, 0x00 }, // (Offset,Value)-Pair #8
        { 0x49, 0x78 }, // (Offset,Value)-Pair #9
        { 0x4A, 0x88 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetPlayRegion
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetPlayRegion, 4361, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_DSBUFFERSETPLAYREGIONA ),

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
// * public: long __thiscall DirectSound::CMcpxBuffer::Play(unsigned long)
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_Play, 4361, 11,

    XREF_CMcpxBuffer_Play,
    XRefZero)


        // CMcpxBuffer_Play+0x0E : cmp ebx, edi
        { 0x0E, 0x3B }, // (Offset,Value)-Pair #1
        { 0x0F, 0xDF }, // (Offset,Value)-Pair #2

        // CMcpxBuffer_Play+0x1C : jz +0x09
        { 0x1C, 0x74 }, // (Offset,Value)-Pair #3
        { 0x1D, 0x09 }, // (Offset,Value)-Pair #4

        // CMcpxBuffer_Play+0x30 : test byte ptr [esi+0x12], 2
        { 0x30, 0xF6 }, // (Offset,Value)-Pair #5
        { 0x31, 0x46 }, // (Offset,Value)-Pair #6
        { 0x32, 0x12 }, // (Offset,Value)-Pair #7
        { 0x33, 0x02 }, // (Offset,Value)-Pair #8

        // CMcpxBuffer_Play+0x71 : call dword ptr [eax+0x18]
        { 0x71, 0xFF }, // (Offset,Value)-Pair #9
        { 0x72, 0x50 }, // (Offset,Value)-Pair #10
        { 0x73, 0x18 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_Play
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_Play, 4361, 10,

    XREF_CDirectSoundBuffer_Play,
    XRefOne)

        // CDirectSoundBuffer_Play+0x35 : call [CMcpxBuffer::Play]
        XREF_ENTRY( 0x35, XREF_CMcpxBuffer_Play ),  // (Offset,Value)-Pair #1

        // CDirectSoundBuffer_Play+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 }, // (Offset,Value)-Pair #2
        { 0x22, 0x05 }, // (Offset,Value)-Pair #3
        { 0x23, 0x40 }, // (Offset,Value)-Pair #4
        { 0x24, 0x00 }, // (Offset,Value)-Pair #5
        { 0x25, 0x80 }, // (Offset,Value)-Pair #6

        // CDirectSoundBuffer_Play+0x3D : jz +0x0B
        { 0x3D, 0x74 }, // (Offset,Value)-Pair #7
        { 0x3E, 0x0B }, // (Offset,Value)-Pair #8

        // CDirectSoundBuffer_Play+0x4E : retn 0x10
        { 0x4E, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x4F, 0x10 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_Play
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_Play, 4361, 9,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_Play+0x1D : call [CDirectSoundBuffer::Play]
        XREF_ENTRY( 0x1D, XREF_CDirectSoundBuffer_Play ),  // (Offset,Value)-Pair #1

        // IDirectSoundBuffer_Play+0x0E : push [esp+0x10]
        { 0x0E, 0xFF }, // (Offset,Value)-Pair #2
        { 0x0F, 0x74 }, // (Offset,Value)-Pair #3
        { 0x10, 0x24 }, // (Offset,Value)-Pair #4
        { 0x11, 0x10 }, // (Offset,Value)-Pair #5

        // IDirectSoundBuffer_Play+0x17 : sbb ecx, ecx
        { 0x17, 0x1B }, // (Offset,Value)-Pair #6
        { 0x18, 0xC9 }, // (Offset,Value)-Pair #7

        // IDirectSoundBuffer_Play+0x19 : and ecx, eax
        { 0x19, 0x23 }, // (Offset,Value)-Pair #8
        { 0x1A, 0xC8 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_Stop
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_Stop, 4361, 9,

	XREF_CMcpxBuffer_Stop,
    XRefZero)

        // CMcpxBuffer_Stop+0x0E : cmp al, 3
        { 0x0E, 0x3C }, // (Offset,Value)-Pair #1
        { 0x0F, 0x03 }, // (Offset,Value)-Pair #2

        // CMcpxBuffer_Stop+0x17 : jz +0x2A
        { 0x17, 0x74 }, // (Offset,Value)-Pair #3
        { 0x18, 0x2A }, // (Offset,Value)-Pair #4

        // CMcpxBuffer_Stop+0x19 : test [esp+0Ch+arg_0], 2
        { 0x19, 0xF6 }, // (Offset,Value)-Pair #5
        { 0x1A, 0x44 }, // (Offset,Value)-Pair #6
        { 0x1B, 0x24 }, // (Offset,Value)-Pair #7
        { 0x1C, 0x10 }, // (Offset,Value)-Pair #8
        { 0x1D, 0x02 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_Stop
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_Stop, 4361, 10,

    XREF_CDirectSoundBuffer_Stop,
    XRefOne)

        // CDirectSoundBuffer_Stop+0x33 : call [CMcpxBuffer::Stop]
        XREF_ENTRY( 0x33, XREF_CMcpxBuffer_Stop),  // (Offset,Value)-Pair #1

        // CDirectSoundBuffer_Stop+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 }, // (Offset,Value)-Pair #2
        { 0x22, 0x05 }, // (Offset,Value)-Pair #3
        { 0x23, 0x40 }, // (Offset,Value)-Pair #4
        { 0x24, 0x00 }, // (Offset,Value)-Pair #5
        { 0x25, 0x80 }, // (Offset,Value)-Pair #6

        // CDirectSoundBuffer_Stop+0x3B : jz +0x0B
        { 0x3B, 0x74 }, // (Offset,Value)-Pair #7
        { 0x3C, 0x0B }, // (Offset,Value)-Pair #8

        // CDirectSoundBuffer_Stop+0x4C : retn 0x04
        { 0x4C, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x4D, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_Stop
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_Stop, 4361, 8,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_Stop+0x11 : call [CDirectSoundBuffer::Stop]
        XREF_ENTRY( 0x11, XREF_CDirectSoundBuffer_Stop),  // (Offset,Value)-Pair #1

        // IDirectSoundBuffer_Stop+0x06 : and eax, 0xFFFFFFE4
        { 0x06, 0x83 }, // (Offset,Value)-Pair #2
        { 0x07, 0xC0 }, // (Offset,Value)-Pair #3
        { 0x08, 0xE4 }, // (Offset,Value)-Pair #4

        // IDirectSoundBuffer_Stop+0x0B : sbb ecx, ecx
        { 0x0B, 0x1B }, // (Offset,Value)-Pair #5
        { 0x0C, 0xC9 }, // (Offset,Value)-Pair #6

        // IDirectSoundBuffer_Stop+0x0D : and ecx, eax
        { 0x0D, 0x23 }, // (Offset,Value)-Pair #7
        { 0x0E, 0xC8 }, // (Offset,Value)-Pair #8
OOVPA_END;

#if 0 // Used 4134
// ******************************************************************
// * DirectSound::CDirectSoundVoiceSettings::SetMixBinVolumes
// ******************************************************************
OOVPA_XREF(CDirectSoundVoiceSettings_SetMixBinVolumes, 4361, 7,

    XREF_DSSETMIXBINVOLUMESC,
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
OOVPA_XREF(CDirectSoundVoice_SetMixBinVolumes, 4361, 8,

    XREF_DSSETMIXBINVOLUMESB,
    XRefOne)

        XREF_ENTRY( 0x0D, XREF_DSSETMIXBINVOLUMESC ),

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
OOVPA_XREF(CDirectSoundBuffer_SetMixBinVolumes, 4361, 8,

    XREF_DSSETMIXBINVOLUMESA,
    XRefOne)

        XREF_ENTRY( 0x32, XREF_DSSETMIXBINVOLUMESB ),

        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0x10 },
        { 0x3A, 0x74 },
        { 0x47, 0x8B },
OOVPA_END;
#endif
// ******************************************************************
// * DirectSound::CDirectSoundBuffer::PlayEx
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_PlayEx, 4361, 8,

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

// ******************************************************************
// * CMcpxBuffer::Stop2
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_Stop2, 4361, 10,

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

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::StopEx
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_StopEx, 4361, 8,

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

// ******************************************************************
// * IDirectSoundBuffer_StopEx
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_StopEx, 4361, 8,

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

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetFormat
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetFormat, 4361, 8,

    XREF_CDirectSoundVoice_SetFormat,
    XRefZero)

        { 0x07, 0x4E },
        { 0x10, 0xE8 },
        { 0x19, 0xD8 },
        { 0x22, 0xFF },
        { 0x2B, 0x0C },
        { 0x34, 0xFF },
        { 0x3F, 0x8B },
        { 0x46, 0x00 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetFormat
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetFormat, 4361, 8,

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
OOVPA_XREF(IDirectSoundBuffer_SetFormat, 4361, 8,

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
// * DirectSound::CDirectSoundStream::SetFormat
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetFormat, 4361, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_SetFormat ),

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

// ******************************************************************
// * CDirectSoundVoice::SetMaxDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMaxDistance, 4361, 12,

    XREF_DSVOICESETMAXDISTANCE,
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

    XREF_DSVOICESETMINDISTANCE,
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

    XREF_DSVOICESETROLLOFFFACTOR,
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
// * CDirectSoundVoice::SetDistanceFactor
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetDistanceFactor, 4361, 12,

    XREF_DSVOICESETDISTANCEFACTOR,
    XRefZero)

        // CDirectSoundVoice_SetDistanceFactor+0x0D : mov edx, [esp+arg_4]
        { 0x0D, 0x8B },
        { 0x0E, 0x54 },
        { 0x0F, 0x24 },
        { 0x10, 0x08 },

        // CDirectSoundVoice_SetDistanceFactor+0x11 : mov [eax+40h], edx
        { 0x11, 0x89 },
        { 0x12, 0x50 },
        { 0x13, 0x40 },

        { 0x14, 0x8B },
        { 0x1D, 0x83 },
        { 0x1F, 0x78 },

        // CDirectSoundVoice_SetDistanceFactor+0x30 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetConeOutsideVolume
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetConeOutsideVolume, 4361, 12,

    XREF_DSVOICESETCONEOUTSIDEVOLUME,
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
// * CDirectSoundVoice::SetDopplerFactor
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetDopplerFactor, 4361, 12,

	XREF_CDirectSoundVoice_SetDopplerFactor,
    XRefZero)

        // CDirectSoundVoice_SetDopplerFactor+0x0D : mov edx, [esp+arg_4]
        { 0x0D, 0x8B },
        { 0x0E, 0x54 },
        { 0x0F, 0x24 },
        { 0x10, 0x08 },

        // CDirectSoundVoice_SetDopplerFactor+0x12 : mov [eax+48h], edx
        { 0x11, 0x89 },
        { 0x12, 0x50 },
        { 0x13, 0x48 },

        { 0x14, 0x8B },
        { 0x1D, 0x83 },
        { 0x1F, 0x78 },

        // CDirectSoundVoice_SetDopplerFactor+0x31 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 }
OOVPA_END;

// ******************************************************************
// * DSound_4361
// ******************************************************************
OOVPATable DSound_4361[] = {

	REGISTER_OOVPA(DirectSoundCreate, 4134, PATCH),
	REGISTER_OOVPA(DirectSoundDoWork, 4134, PATCH),
	REGISTER_OOVPA(DirectSoundGetSampleTime, 4361, PATCH),
	REGISTER_OOVPA(CDirectSound_CreateSoundStream, 4134, XREF),
	REGISTER_OOVPA(IDirectSound_CreateSoundStream, 3936, PATCH),
	REGISTER_OOVPA(DirectSoundCreateStream, 4134, PATCH),
	REGISTER_OOVPA(CMcpxStream_Pause, 4361, XREF),
	REGISTER_OOVPA(CDirectSoundStream_Pause, 4361, PATCH),
	REGISTER_OOVPA(IDirectSound_AddRef, 3936, PATCH),
	REGISTER_OOVPA(CDirectSound_CreateSoundBuffer, 4134, XREF),
	REGISTER_OOVPA(IDirectSound_CreateSoundBuffer, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_SetPlayRegion, 4361, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetPlayRegion, 4361, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_SetLoopRegion, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetLoopRegion, 4134, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_SetVolume, 4134, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_SetBufferData, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetBufferData, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetBufferData, 4134, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_SetCurrentPosition, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetCurrentPosition, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetCurrentPosition, 4134, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_GetCurrentPosition, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_GetCurrentPosition, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_GetCurrentPosition, 3936, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_Play, 4361, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_Play, 4361, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Play, 4361, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_Stop, 4361, XREF),
	REGISTER_OOVPA(CMcpxBuffer_Stop2, 4361, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_Stop, 4361, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Stop, 4361, PATCH),
	REGISTER_OOVPA(CDirectSoundVoiceSettings_SetMixBinVolumes, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetMixBinVolumes, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMixBinVolumes, 4134, XREF),
	REGISTER_OOVPA(CMcpxBuffer_Play2, 4361, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_PlayEx, 4361, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_PlayEx, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_StopEx, 4361, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_StopEx, 4361, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_AddRef, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound_Release, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundVoiceSettings_SetMixBins, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetMixBins, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMixBins, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMixBins, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetFormat, 4361, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetFormat, 4361, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetFormat, 4361, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetFormat, 4361, PATCH),
};

// ******************************************************************
// * DSound_4361_SIZE
// ******************************************************************
uint32 DSound_4361_SIZE = sizeof(DSound_4361);
