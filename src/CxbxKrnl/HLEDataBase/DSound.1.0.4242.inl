// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->DSound.1.0.4242.inl
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
// *  (c) 2017 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************

// ******************************************************************
// * CMcpxVoiceClient::SetLFO
// ******************************************************************
// Generic OOVPA as of 4242 and newer
OOVPA_XREF(CMcpxVoiceClient_SetLFO, 4242, 12,

    XREF_CMcpxVoiceClient_SetLFO,
    XRefZero)

        { 0x00, 0x55 },
        { 0x14, 0x8B },

        { 0x18, 0x08 },
        { 0x19, 0x85 },
        { 0x1A, 0xC9 },
        { 0x1B, 0x75 },
        { 0x1C, 0x4A },
        { 0x1D, 0x39 },
        { 0x1E, 0x48 },
        { 0x1F, 0x04 },

        { 0x50, 0x31 },
        { 0x51, 0x4E },
OOVPA_END;

// ******************************************************************
// * CMcpxVoiceClient::SetEG
// ******************************************************************
// Generic OOVPA as of 4242 and newer
OOVPA_XREF(CMcpxVoiceClient_SetEG, 4242, 14,

    XREF_CMcpxVoiceClient_SetEG,
    XRefZero)

        { 0x00, 0x55 },
        { 0x01, 0x8B },

        { 0x16, 0x8B },
        { 0x17, 0x45 },
        { 0x18, 0x08 },
        { 0x19, 0x8B },
        { 0x1A, 0x08 },
        { 0x1B, 0x85 },
        { 0x1C, 0xC9 },
        { 0x1D, 0x75 },
        { 0x1E, 0x75 },
        { 0x1F, 0x8B },

        { 0x50, 0x56 },
        { 0x51, 0x24 },
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_Stop
// ******************************************************************
//Generic OOVPA as of 4242 and newer.
OOVPA_XREF(CMcpxBuffer_Stop, 4242, 9,

	XREF_CMcpxBuffer_Stop,
    XRefZero)

        // CMcpxBuffer_Stop+0x0E : cmp al, 3
        { 0x0E, 0x3C },
        { 0x0F, 0x03 },

        // CMcpxBuffer_Stop+0x17 : jne +0x2A
        { 0x17, 0x74 },
        { 0x18, 0x2A },

        // CMcpxBuffer_Stop+0x19 : test [esp+10h], 2
        { 0x19, 0xF6 },
        { 0x1A, 0x44 },
        { 0x1B, 0x24 },
        { 0x1C, 0x10 },
        { 0x1D, 0x02 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetNotificationPositions
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetNotificationPositions, 4242, 11,

    XREF_CDirectSoundBuffer_SetNotificationPositions,
    XRefZero)

        // CDirectSoundBuffer_SetNotificationPositions+0x00 : push ebx
        { 0x00, 0x53 },

        { 0x0C, 0x00 },
        { 0x1B, 0xFF },
        { 0x25, 0x80 },

        { 0x2C, 0x0C },
        { 0x2D, 0x8B },
        { 0x2E, 0x4E },
        { 0x2F, 0x1C },

        { 0x30, 0x57 },

        { 0x47, 0xE8 },
        { 0x5D, 0x8B },
OOVPA_END;

// ******************************************************************
// * CDirectSound_GetSpeakerConfig
// ******************************************************************
OOVPA_XREF(CDirectSound_GetSpeakerConfig, 4242, 12,

    XREF_CDirectSound_GetSpeakerConfig,
    XRefZero)

        { 0x00, 0xE8 },
        { 0x20, 0xB8 },

        { 0x27, 0x8B },
        { 0x28, 0x4C },
        { 0x29, 0x24 },
        { 0x2A, 0x04 },
        { 0x2B, 0x8B },
        { 0x2C, 0x49 },
        { 0x2D, 0x08 },
        { 0x2E, 0x8B },

        { 0x4E, 0xC2 },
        { 0x4F, 0x08 },
OOVPA_END;

// ******************************************************************
// * CFullHrtfSource_GetHrtfFilterPair
// ******************************************************************
OOVPA_XREF(CFullHrtfSource_GetHrtfFilterPair, 4242, 15,

    XREF_CFullHrtfSource_GetHrtfFilterPair,
    XRefZero)

        { 0x00, 0x56 },
        { 0x01, 0x8B },
        { 0x02, 0x74 },
        { 0x03, 0x24 },
        { 0x04, 0x08 },
        { 0x05, 0xD9 },
        { 0x06, 0x46 },
        { 0x07, 0x14 },
        { 0x08, 0x51 },
        { 0x09, 0xD8 },
        { 0x0A, 0x1D },

        { 0x1A, 0x05 },
        { 0x1F, 0xEB },

        { 0x58, 0xD8 },
        { 0x59, 0x05 },
OOVPA_END;

// ******************************************************************
// * CHrtfSource_SetAlgorithm_FullHrtf
// ******************************************************************
OOVPA_XREF(CHrtfSource_SetAlgorithm_FullHrtf, 4242, 1+8,

    XREF_CHrtfSource_SetAlgorithm_FullHrtf,
    XRefOne)

        XREF_ENTRY( 0x17, XREF_CFullHrtfSource_GetHrtfFilterPair ),

        { 0x00, 0x83 },
        { 0x01, 0x25 },
        { 0x06, 0x00 },
        { 0x07, 0xC7 },
        { 0x08, 0x05 },
        { 0x11, 0xC7 },
        { 0x12, 0x05 },
        { 0x1B, 0xC3 },
OOVPA_END;

// ******************************************************************
// * DirectSoundUseFullHRTF
// ******************************************************************
OOVPA_XREF(DirectSoundUseFullHRTF, 4242, 1+8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY(0x13, XREF_CHrtfSource_SetAlgorithm_FullHrtf ),

        { 0x00, 0x56 },
        { 0x01, 0xE8 },
        { 0x06, 0x83 },
        { 0x07, 0x3D },
        { 0x0C, 0x02 },
        { 0x1A, 0x74 },
        { 0x1B, 0x0B },
        { 0x1C, 0x68 },
OOVPA_END;

// ******************************************************************
// * CLightHrtfSource_GetHrtfFilterPair
// ******************************************************************
OOVPA_XREF(CLightHrtfSource_GetHrtfFilterPair, 4242, 16,

    XREF_CLightHrtfSource_GetHrtfFilterPair,
    XRefZero)

        { 0x00, 0x56 },
        { 0x01, 0x8B },
        { 0x02, 0x74 },
        { 0x03, 0x24 },
        { 0x04, 0x08 },
        { 0x05, 0xD9 },
        { 0x06, 0x46 },
        { 0x07, 0x10 },
        { 0x08, 0x51 },
        { 0x09, 0xD9 },
        { 0x0A, 0xE1 },
        { 0x0B, 0xD8 },
        { 0x0C, 0x05 },

        { 0x19, 0x99 },
        { 0x1A, 0x6A },

        { 0x8B, 0x89 },
OOVPA_END;

// ******************************************************************
// * CHrtfSource_SetAlgorithm_LightHrtf
// ******************************************************************
OOVPA_XREF(CHrtfSource_SetAlgorithm_LightHrtf, 4242, 1+11,

    XREF_CHrtfSource_SetAlgorithm_LightHrtf,
    XRefOne)

        XREF_ENTRY(0x1A, XREF_CLightHrtfSource_GetHrtfFilterPair ),

        { 0x00, 0xC7 },
        { 0x01, 0x05 },
        { 0x06, 0x01 },
        { 0x07, 0x00 },
        { 0x08, 0x00 },
        { 0x09, 0x00 },
        { 0x0A, 0xC7 },
        { 0x0B, 0x05 },
        { 0x14, 0xC7 },
        { 0x15, 0x05 },
        { 0x1E, 0xC3 },
OOVPA_END;

// ******************************************************************
// * DirectSoundUseLightHRTF
// ******************************************************************
OOVPA_XREF(DirectSoundUseLightHRTF, 4242, 1+8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY(0x13, XREF_CHrtfSource_SetAlgorithm_LightHrtf ),

        { 0x00, 0x56 },
        { 0x01, 0xE8 },
        { 0x06, 0x83 },
        { 0x07, 0x3D },
        { 0x0C, 0x02 },
        { 0x1A, 0x74 },
        { 0x1B, 0x0B },
        { 0x1C, 0x68 },
OOVPA_END;
