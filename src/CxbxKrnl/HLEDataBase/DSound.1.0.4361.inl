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

// ******************************************************************
// * DirectSound::CMcpxStream::Pause2
// ******************************************************************
// Generic OOVPA as of 4361 and newer
OOVPA_XREF(CMcpxStream_Pause_Ex, 4361, 1+8,

    XREF_CMcpxStream_Pause_Ex,
    XRefOne)

        XREF_ENTRY( 0x2B, XREF_CMcpxStream_Pause ),

        { 0x00, 0x55 },

        { 0x0D, 0x8B },

        { 0x1C, 0x6A },
        { 0x1D, 0x05 },

        { 0x23, 0x75 },

        { 0x2A, 0xE8 },

        { 0x36, 0xC2 },
        { 0x37, 0x0C },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundStream::PauseEx
// ******************************************************************
// Generic OOVPA as of 4361 and newer
OOVPA_XREF(CDirectSoundStream_PauseEx, 4361, 1+10,

    XREF_CDirectSoundStream_PauseEx,
    XRefOne)

        XREF_ENTRY( 0x3D, XREF_CMcpxStream_Pause_Ex ),

        { 0x00, 0x56 },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x41, 0x85 },
        { 0x4C, 0xFF },
        { 0x53, 0xC7 },

        { 0x56, 0xC2 },
        { 0x57, 0x10 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundStream_PauseEx
// ******************************************************************
// Generic OOVPA as of 4361 and newer
OOVPA_XREF(IDirectSoundStream_PauseEx, 4361, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x11, XREF_CDirectSoundStream_PauseEx ),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x24 },
        { 0x0E, 0x24 },
        { 0x10, 0xE8 },
        { 0x15, 0xC2 },
        { 0x16, 0x10 },
OOVPA_END;

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

// ******************************************************************
// * CDirectSoundVoice::SetDistanceFactor
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetDistanceFactor, 4361, 12,

    XREF_CDirectSoundVoice_SetDistanceFactor,
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

//TODO: Need further verification for 4361 titles, 4432 is last known lowest build match.
//Test case for 4361: Burnout (found a match yet is called twice for every time joystick is moved. Is this normal?)
// ******************************************************************
// * DirectSound::GetCaps
// ******************************************************************
OOVPA_XREF(CDirectSound_GetCaps, 4361, 13,

    XREF_CDirectSound_GetCaps,
    XRefZero)

        { 0x00, 0xE8 },
        { 0x0D, 0xB6 },

        { 0x2D, 0x8B },
        { 0x2E, 0x44 },
        { 0x2F, 0x24 },
        { 0x30, 0x08 },
        { 0x31, 0x89 },
        { 0x32, 0x10 },
        { 0x33, 0x8B },
        { 0x34, 0x15 },

        { 0x52, 0x03 },
        { 0x69, 0xC2 },
        { 0x6A, 0x08 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetRolloffCurve
// ******************************************************************
// Generic OOVPA as of 4361 and newer
OOVPA_XREF(CDirectSoundVoice_SetRolloffCurve, 4361, 12,

    XREF_CDirectSoundVoice_SetRolloffCurve,
    XRefZero)

        { 0x00, 0x8B },

        { 0x10, 0x08 },
        { 0x11, 0x89 },
        { 0x12, 0x51 },
        { 0x13, 0x70 }, // SetConeAngles 0x1C vs SetRolloffCurve 0x70
        { 0x14, 0x8B },
        { 0x15, 0x48 },
        { 0x16, 0x10 },
        { 0x17, 0x8B },
        { 0x18, 0x89 },

        { 0x23, 0x74 }, // SetConeAngles 0x20 vs SetRolloffCurve 0x74
        { 0x30, 0x04 }, // SetConeAngles 0x10 vs SetRolloffCurve 0x04
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetRolloffCurve
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetRolloffCurve, 4361, 1+7,

    XREF_CDirectSoundBuffer_SetRolloffCurve,
    XRefOne)

        XREF_ENTRY( 0x3A, XREF_CDirectSoundVoice_SetRolloffCurve ),

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x74 },
        { 0x35, 0xFF },
        { 0x40, 0x8B },
        { 0x4F, 0x8B },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetRolloffCurve
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetRolloffCurve, 4361, 1+7,

    XREF_CDirectSoundStream_SetRolloffCurve,
    XRefOne)

        XREF_ENTRY( 0x3E, XREF_CDirectSoundVoice_SetRolloffCurve ),

        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x74 },
        { 0x3A, 0x24 },
        { 0x46, 0x74 },
        { 0x53, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetRolloffCurve
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetRolloffCurve, 4361, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_CDirectSoundBuffer_SetRolloffCurve ),

        { 0x03, 0x10 },
        { 0x08, 0xFF },
        { 0x0D, 0xC8 },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundStream_SetRolloffCurve
// ******************************************************************
// Generic OOVPA as of 4361 and newer
OOVPA_XREF(IDirectSoundStream_SetRolloffCurve, 4361, 1+1,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundStream_SetRolloffCurve+0x00 : jmp [CDirectSoundStream_SetRolloffCurve]
        XREF_ENTRY( 0x01, XREF_CDirectSoundStream_SetRolloffCurve ),

        // IDirectSoundStream_SetRolloffCurve+0x00 : jmp 0x........
        { 0x00, 0xE9 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSound::GetOutputLevels
// ******************************************************************
OOVPA_XREF(CDirectSound_GetOutputLevels, 4361, 12,

    XREF_CDirectSound_GetOutputLevels,
    XRefZero)

        { 0x00, 0x56 },
        { 0x07, 0x10 },

        { 0x18, 0x5F },
        { 0x19, 0x5E },
        { 0x1A, 0x74 },
        { 0x1B, 0x25 },
        { 0x1C, 0x89 },
        { 0x1D, 0x02 },
        { 0x1E, 0xA3 },
        { 0x1F, 0xB0 },

        { 0x43, 0xC2 },
        { 0x44, 0x0C },
OOVPA_END;

// ******************************************************************
// * IDirectSound_GetOutputLevels
// ******************************************************************
OOVPA_XREF(IDirectSound_GetOutputLevels, 4361, 1+8,

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
// * XFileCreateMediaObject
// ******************************************************************
// Generic OOVPA as of 4361 and newer
OOVPA_XREF(XFileCreateMediaObject, 4361, 10,

    XRefNoSaveIndex,
    XRefZero)

        { 0x00, 0x55 },
        { 0x01, 0x8B },
        { 0x03, 0x56 },

        { 0x34, 0x18 },
        { 0x37, 0x14 },
        { 0x3A, 0x10 },
        { 0x3D, 0x0C },
        { 0x40, 0x08 },

        { 0x67, 0xC2 },
        { 0x68, 0x18 },
OOVPA_END;

// ******************************************************************
// * XFileCreateMediaObjectEx
// ******************************************************************
// Generic OOVPA as of 4361 and newer
OOVPA_NO_XREF(XFileCreateMediaObjectEx, 4361, 12)

        { 0x00, 0x56 },
        { 0x1F, 0x1B },

        { 0x2D, 0x78 },
        { 0x2E, 0x1C },
        { 0x2F, 0xFF },
        { 0x30, 0x74 },
        { 0x31, 0x24 },
        { 0x32, 0x0C },
        { 0x33, 0x57 },
        { 0x34, 0xE8 },

        { 0x59, 0xC2 },
        { 0x5A, 0x08 },
OOVPA_END;

// ******************************************************************
// * XWaveFileCreateMediaObject
// ******************************************************************
// Generic OOVPA as of 4361 and newer
OOVPA_NO_XREF(XWaveFileCreateMediaObject, 4361, 12)

        { 0x00, 0x56 },
        { 0x1C, 0xF7 },

        { 0x3C, 0xF6 },
        { 0x3D, 0x7C },
        { 0x3E, 0x23 },
        { 0x3F, 0x83 },
        { 0x40, 0x7C },
        { 0x41, 0x24 },
        { 0x42, 0x10 },
        { 0x43, 0x00 },

        { 0x70, 0xC2 },
        { 0x71, 0x0C },
OOVPA_END;

// ******************************************************************
// * XWaveFileCreateMediaObjectEx
// ******************************************************************
// Generic OOVPA as of 4361 and newer
OOVPA_XREF(XWaveFileCreateMediaObjectEx, 4361, 12,

    XRefNoSaveIndex,
    XRefZero)

        { 0x0D, 0x0B },
        { 0x1C, 0xF7 },

        { 0x40, 0xEB },
        { 0x41, 0x0A },
        { 0x42, 0xFF },
        { 0x43, 0x74 },
        { 0x44, 0x24 },
        { 0x45, 0x10 },
        { 0x46, 0x57 },
        { 0x47, 0xE8 },

        { 0x6C, 0xC2 },
        { 0x6D, 0x0C },
OOVPA_END;

// ******************************************************************
// * DirectSound::CMemoryManager::PoolAlloc
// ******************************************************************
OOVPA_NO_XREF(DSound_CMemoryManager_PoolAlloc, 4361, 11)

        { 0x00, 0x56 },
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
