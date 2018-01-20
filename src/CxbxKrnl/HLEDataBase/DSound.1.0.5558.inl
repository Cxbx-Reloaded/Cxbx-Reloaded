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
// * DirectSound::CDirectSoundStream::SetMinDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMinDistance, 5558, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x3E, XREF_CDirectSoundVoice_SetMinDistance ),

        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x24 },
        { 0x3A, 0x1C },
        { 0x46, 0x74 },
        { 0x53, 0x8B },
OOVPA_END;

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
// * Rollback support signature(s)
// ******************************************************************
#define CMcpxStream_Flush_5558 CMcpxStream_Flush_4134
