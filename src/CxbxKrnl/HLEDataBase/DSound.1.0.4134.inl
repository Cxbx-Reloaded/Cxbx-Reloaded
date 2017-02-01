// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->DSound.1.0.4134.cpp
// *
// *  This file is part of the Cxbx project.
// *
// *  Cxbx is free software; you can redistribute it
// *  and/or modify it under the terms of the GNU General Public
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
// * DirectSoundCreate
// ******************************************************************
OOVPA_NO_XREF(DirectSoundCreate, 4134, 9)

        // DirectSoundCreate+0x0B : movzx esi, al
        { 0x0B, 0x0F }, // (Offset,Value)-Pair #1
        { 0x0C, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x0D, 0xF0 }, // (Offset,Value)-Pair #3

        // DirectSoundCreate+0x12 : call [addr]
        { 0x12, 0xE8 }, // (Offset,Value)-Pair #4

        // DirectSoundCreate+0x1B : jl +0x13
        { 0x1B, 0x7C }, // (Offset,Value)-Pair #5
        { 0x1C, 0x13 }, // (Offset,Value)-Pair #6

        // DirectSoundCreate+0x27 : sbb ecx, ecx
        { 0x27, 0x1B }, // (Offset,Value)-Pair #7
        { 0x28, 0xC9 }, // (Offset,Value)-Pair #8

        // DirectSoundCreate+0x43 : leave
        { 0x43, 0xC9 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * IDirectSound8_CreateSoundBuffer
// ******************************************************************
OOVPA_NO_XREF(IDirectSound8_CreateSoundBuffer, 4134, 9)

        // IDirectSound8_CreateSoundBuffer+0x2A : push 0x24
        { 0x2A, 0x6A }, // (Offset,Value)-Pair #1
        { 0x2B, 0x24 }, // (Offset,Value)-Pair #2

        // IDirectSound8_CreateSoundBuffer+0x3C : call [addr]
        { 0x3C, 0xE8 }, // (Offset,Value)-Pair #3

        // IDirectSound8_CreateSoundBuffer+0x4A : and esi, 0x7FF8FFF2
        { 0x4A, 0x81 }, // (Offset,Value)-Pair #4
        { 0x4B, 0xE6 }, // (Offset,Value)-Pair #5
        { 0x4C, 0xF2 }, // (Offset,Value)-Pair #6
        { 0x4D, 0xFF }, // (Offset,Value)-Pair #7
        { 0x4E, 0xF8 }, // (Offset,Value)-Pair #8
        { 0x4F, 0x7F }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetHeadroom
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetHeadroom, 4134, 7,

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
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetHeadroom, 4134, 8,

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
OOVPA_XREF(IDirectSoundBuffer_SetHeadroom, 4134, 8,

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
OOVPA_NO_XREF(IDirectSoundBuffer_Lock, 4134, 8)

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
// * DirectSound::CDirectSoundVoice::SetVolume
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetVolume, 4134, 7,

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
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetVolume, 4134, 8,

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
// * IDirectSoundBuffer_SetVolume
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetVolume, 4134, 8,

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
// * DirectSound::CDirectSound::SetI3DL2Listener
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_SetI3DL2Listener, 4134, 7,

    XREF_DSSETI3DL2LISTENER,
    XRefZero)
        { 0x1E, 0x15 },
        { 0x3E, 0x88 },
        { 0x5E, 0x5A },
        { 0x7E, 0x89 },
        { 0x9E, 0x20 },
        { 0xBE, 0x00 },
        { 0xDE, 0x75 },
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetI3DL2Listener
// ******************************************************************
OOVPA_XREF(IDirectSound_SetI3DL2Listener, 4134, 9,

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
// * DirectSound::CDirectSoundBuffer::SetBufferData
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetBufferData, 4134, 7,

    XREF_DSSETBUFFERDATA2,
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
OOVPA_XREF(IDirectSoundBuffer_SetBufferData, 4134, 9,

    XRefNoSaveIndex,
    XRefOne)

        { 0x19, XREF_DSSETBUFFERDATA2 },

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
// * DirectSound::CDirectSound::GetEffectData
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_GetEffectData, 4134, 8,

    XREF_DirectSound_CDirectSound_GetEffectData,
    XRefZero)
        { 0x0A, 0x3D },
        { 0x16, 0xF6 },
        { 0x24, 0xB8 },
        { 0x2E, 0x8B },
        { 0x3A, 0x14 },
        { 0x46, 0x85 },
        { 0x52, 0x15 },
        { 0x5E, 0x00 },
OOVPA_END;

// ******************************************************************
// * IDirectSound_GetEffectData
// ******************************************************************
OOVPA_XREF(IDirectSound_GetEffectData, 4134, 9,

    XRefNoSaveIndex,
    XRefOne)

        { 0x1F, XREF_DirectSound_CDirectSound_GetEffectData },

        { 0x03, 0xFF },
        { 0x08, 0x08 },
        { 0x0D, 0xC8 },
        { 0x12, 0xC0 },
        { 0x17, 0xF7 },
        { 0x1C, 0xC8 },
        { 0x23, 0x5D },
        { 0x26, 0x00 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoiceSettings::SetMixBins
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoiceSettings_SetMixBins, 4134, 7,

    XREF_DSSETMIXBINSC,
    XRefZero)
        { 0x0E, 0xB8 },
        { 0x22, 0x83 },
        { 0x2E, 0x71 },
        { 0x3E, 0xB6 },
        { 0x4E, 0x41 },
        { 0x5E, 0x00 },
        { 0x6E, 0x00 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetMixBins
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetMixBins, 4134, 8,

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
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetMixBins, 4134, 8,

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
// * IDirectSoundBuffer_SetMixBins
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMixBins, 4134, 8,

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
// * DirectSoundCreate, 4134
// ******************************************************************
OOVPATable DSound_4134[] = {

    // DirectSoundCreate
	OOVPA_TABLE_ENTRY(DirectSoundCreate, 4134),
	// IDirectSound8::AddRef (* unchanged since 3936 *)
	OOVPA_TABLE_ENTRY(IDirectSound8_AddRef, 3936),
	// IDirectSound8::Release (* unchanged since 3936 *)
	OOVPA_TABLE_ENTRY(IDirectSound8_Release, 3936),
	// IDirectSound8::DownloadEffectsImage (* unchanged since 3936 *)
	OOVPA_TABLE_ENTRY(IDirectSound8_DownloadEffectsImage, 3936),
	// IDirectSound8::CreateSoundBuffer
	OOVPA_TABLE_ENTRY(IDirectSound8_CreateSoundBuffer, 4134),
	// DirectSound::CDirectSoundVoice::SetHeadroom (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundVoice_SetHeadroom, 4134),
	// DirectSound::CDirectSoundBuffer::SetHeadroom (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_SetHeadroom, 4134),
	// IDirectSoundBuffer_SetHeadroom
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetHeadroom, 4134, IDirectSoundBuffer8_SetHeadroom),
	// IDirectSoundBuffer8::Lock
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_Lock, 4134, IDirectSoundBuffer8_Lock),
	// DirectSound::CDirectSoundVoice::SetVolume (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundVoice_SetVolume, 4134),
	// DirectSound::CDirectSoundBuffer::SetVolume (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_SetVolume, 4134),
	// IDirectSoundBuffer_SetVolume (XREF)
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetVolume, 4134, IDirectSoundBuffer8_SetVolume),
	// CDirectSound_SetI3DL2Listener
	OOVPA_TABLE_XREF(DirectSound_CDirectSound_SetI3DL2Listener, 4134),
	// IDirectSound8_SetI3DL2Listener
	OOVPA_TABLE_PATCH(IDirectSound_SetI3DL2Listener, 4134, IDirectSound8_SetI3DL2Listener),
	// CDirectSound_GetEffectData
	OOVPA_TABLE_XREF(DirectSound_CDirectSound_GetEffectData, 4134),
	// IDirectSound8_SetI3DL2Listener
	OOVPA_TABLE_PATCH(IDirectSound_GetEffectData, 4134, IDirectSound8_GetEffectData),
	// CDirectSoundBuffer::SetBufferData
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_SetBufferData, 4134),
	// IDirectSoundBuffer8::SetBufferData
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetBufferData, 4134, IDirectSoundBuffer8_SetBufferData),
	// IDirectSound8_CreateStream
	OOVPA_TABLE_ENTRY(IDirectSound8_CreateStream, 3936),
	// CDirectSoundVoiceSettings::SetMixBins
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundVoiceSettings_SetMixBins, 4134),
    // CDirectSoundVoice::SetMixBins
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundVoice_SetMixBins, 4134),
    // CDirectSoundBuffer::SetMixBins
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_SetMixBins, 4134),
	// IDirectSoundBuffer8::SetMixBins
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetMixBins, 4134, IDirectSoundBuffer8_SetMixBins),
};

// ******************************************************************
// * DSound_4134_SIZE
// ******************************************************************
uint32 DSound_4134_SIZE = sizeof(DSound_4134);
