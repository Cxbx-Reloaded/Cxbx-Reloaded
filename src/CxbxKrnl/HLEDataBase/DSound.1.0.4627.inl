// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->DSound.1.0.4627.inl
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


#if 0 // Replaced with generic OOVPA 3911
// ******************************************************************
// * IDirectSound_SetMixBinHeadroom
// ******************************************************************
OOVPA_XREF(IDirectSound_SetMixBinHeadroom, 4627, 1+11,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSound_SetMixBinHeadroom+0x19 : call [CDirectSound::SetMixBinHeadroom]
        XREF_ENTRY( 0x19, XREF_CDirectSound_SetMixBinHeadroom ),

        // IDirectSound_SetMixBinHeadroom+0x04 : push [esp+0x0C]
        { 0x04, 0xFF },
        { 0x05, 0x74 },
        { 0x06, 0x24 },
        { 0x07, 0x0C },

        // IDirectSound_SetMixBinHeadroom+0x0E : add eax, 0xFFFFFFF8
        { 0x0E, 0x83 },
        { 0x0F, 0xC0 },
        { 0x10, 0xF8 },

        // IDirectSound_SetMixBinHeadroom+0x13 : sbb ecx, ecx
        { 0x13, 0x1B },
        { 0x14, 0xC9 },

        // IDirectSound_SetMixBinHeadroom+0x15 : and ecx, eax
        { 0x15, 0x23 },
        { 0x16, 0xC8 },
OOVPA_END;
#endif

// ******************************************************************
// * CDirectSound::SetVelocity
// ******************************************************************
OOVPA_XREF(CDirectSound_SetVelocity, 4627, 15,

    XREF_CDirectSound_SetVelocity,
    XRefZero)

        // CDirectSound_SetVelocity+0x24 : mov eax, 0x80004005
        { 0x24, 0xB8 },
        { 0x25, 0x05 },
        { 0x26, 0x40 },
        { 0x27, 0x00 },
        { 0x28, 0x80 },

        // CDirectSound_SetVelocity+0x37 : mov [edx+0x44], edi
        { 0x37, 0x89 },
        { 0x38, 0x7A },
        { 0x39, 0x44 },

        // CDirectSound_SetVelocity+0x4C : or byte ptr [eax+0x0x00A4], 0x80
        { 0x4C, 0x80 },
        { 0x4D, 0x88 },
        { 0x4E, 0xA4 },
        { 0x4F, 0x00 },
        { 0x52, 0x80 },

        // CDirectSound_SetVelocity+0x73 : retn 0x14
        { 0x73, 0xC2 },
        { 0x74, 0x14 },
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetAllParametersA
// ******************************************************************
OOVPA_XREF(CDirectSound_SetAllParametersA, 4627, 10,

    XREF_CDirectSound_SetAllParameters,
    XRefZero)

        { 0x16, 0x68 },
        { 0x17, 0xE8 },
        { 0x18, 0xF7 },
        { 0x19, 0x2D },

        { 0x2F, 0xD9 },
        { 0x30, 0x41 },
        { 0x31, 0x04 },

        { 0x87, 0x8B },
        { 0x88, 0x50 },
        { 0x89, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetAllParametersA
// ******************************************************************
OOVPA_XREF(CDirectSound_SetAllParametersA, 4721, 10,

    XREF_CDirectSound_SetAllParameters,
    XRefZero)

        { 0x16, 0x68 },
        { 0x17, 0xD8 },
        { 0x18, 0x81 },
        { 0x19, 0x15 },

        { 0x2F, 0xD9 },
        { 0x30, 0x41 },
        { 0x31, 0x04 },

        { 0x87, 0x8B },
        { 0x88, 0x50 },
        { 0x89, 0x08 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSound::SetAllParameters
// ******************************************************************
OOVPA_XREF(CDirectSound_SetAllParameters, 4831, 8,

    XREF_CDirectSound_SetAllParameters,
    XRefZero)

        { 0x1B, 0xFF },
        { 0x38, 0x08 },
        { 0x55, 0x5A },
        { 0x72, 0xD9 },
        { 0x8F, 0x2C },
        { 0xAC, 0x50 },
        { 0xC9, 0x10 },
        { 0xE6, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSound_SetPosition
// ******************************************************************
OOVPA_XREF(CDirectSound_SetPosition, 4627, 11,

    XREF_CDirectSound_SetPosition,
    XRefZero)

        // CDirectSound_SetPosition+0x2B : mov ecx, [ebp+0x08]
        { 0x2B, 0x8B },
        { 0x2C, 0x4D },
        { 0x2D, 0x08 },

        // CDirectSound_SetPosition+0x3F : mov [edx+0x3C], edi
        { 0x3F, 0x89 },
        { 0x40, 0x7A },
        { 0x41, 0x3C },

        // CDirectSound_SetPosition+0x4C : or word ptr [eax+0xA4], 0x01FF
        { 0x4C, 0x66 },
        { 0x4D, 0x81 },
        { 0x4E, 0x88 },
        { 0x4F, 0xA4 },
        { 0x53, 0xFF },
OOVPA_END;

// ******************************************************************
// * DirectSoundCreateBuffer
// ******************************************************************
OOVPA_XREF(DirectSoundCreateBuffer, 4242, 1+11,

    XRefNoSaveIndex,
    XRefOne)

        // DirectSoundCreateBuffer+0x2F : call [CDirectSound::CreateSoundBuffer]
        XREF_ENTRY( 0x2F, XREF_CDirectSound_CreateSoundBuffer ),

        // DirectSoundCreateBuffer+0x04 : and [ebp-0x04], 0
        { 0x04, 0x83 },
        { 0x05, 0x65 },
        { 0x06, 0xFC },

        // DirectSoundCreateBuffer+0x08 : push ebx; push esi; push edi
        { 0x08, 0x53 },
        { 0x09, 0x56 },
        { 0x0A, 0x57 },

        // DirectSoundCreateBuffer+0x3C : call dword ptr [eax+8]
        { 0x3C, 0xFF },
        { 0x3D, 0x50 },
        { 0x3E, 0x08 },

        // DirectSoundCreateBuffer+0x54 : retn 0x08
        { 0x54, 0xC2 },
        { 0x55, 0x08 },
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_GetStatus
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_GetStatus, 4721, 17,

	XREF_CMcpxBuffer_GetStatus,
    XRefZero)

        // CMcpxBuffer_GetStatus+0x00 : movzx eax, word ptr [esi+0x12]
        { 0x00, 0x0F },
        { 0x01, 0xB7 },
        { 0x02, 0x41 },
        { 0x03, 0x12 },

		{ 0x07, 0xE1 },
		{ 0x10, 0x24 },
		{ 0x19, 0x0F },
		{ 0x22, 0x74 },

        // CMcpxBuffer_GetStatus+0x27 : mov ecx, 0x8001
        { 0x27, 0xB9 },
        { 0x28, 0x01 },
		{ 0x29, 0x80 },
		{ 0x2A, 0x00 },


		{ 0x2B, 0x80 },
		{ 0x34, 0x24 },

        // CMcpxBuffer_GetStatus+0x39 : mov [ecx], eax
        { 0x39, 0x89 },
        { 0x3A, 0x01 },

		{ 0x3D, 0x33 },
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_GetStatus
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_GetStatus, 4831, 13,

	XREF_CMcpxBuffer_GetStatus,
    XRefZero)

        { 0x00, 0x0F },

        { 0x16, 0x6A },
        { 0x17, 0x00 },
        { 0x18, 0x5A },

        { 0x1C, 0x42 },
        { 0x1D, 0xF6 },
        { 0x1E, 0xC4 },
        { 0x1F, 0x02 },

        { 0x29, 0xB9 },
        { 0x2A, 0x01 },
        { 0x2B, 0x80 },

        { 0x3F, 0xC2 },
        { 0x40, 0x04 },
OOVPA_END;

// ******************************************************************
// * CDirectSound_GetSpeakerConfig
// ******************************************************************
OOVPA_XREF(CDirectSound_GetSpeakerConfig, 4627, 14,

    XREF_CDirectSound_GetSpeakerConfig,
    XRefZero)

        { 0x00, 0xE8 },

        { 0x0B, 0x00 },
        { 0x14, 0x0B },
        { 0x20, 0xB8 },
        { 0x2A, 0x04 },

        { 0x35, 0x81 },
        //{ 0x36, 0xE1 }, (This value has changed, commented out to expand support for later revisions.)
        { 0x37, 0xFF },
        { 0x38, 0xFF },
        { 0x39, 0xFF },
        { 0x3A, 0x7F },

        { 0x40, 0x0B },
        { 0x4C, 0x33 },

        { 0x4A, 0xC2 },
        { 0x4B, 0x08 },
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_Play
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_Play, 4721, 11,

    XREF_CMcpxBuffer_Play,
    XRefZero)

        // CMcpxBuffer_Play+0x00 : push ebx
        { 0x00, 0x53 },

        // CMcpxBuffer_Play+0x2E : test byte ptr [eax+9], 20h
        { 0x2E, 0xF6 },
        { 0x2F, 0x40 },
        { 0x30, 0x09 },
        { 0x31, 0x20 },

        // CMcpxBuffer_Play+0x44 : test byte ptr [esi+12h], 2h
        { 0x44, 0xF6 },
        { 0x45, 0x46 },
        { 0x46, 0x12 },
        { 0x47, 0x02 },

        // CMcpxBuffer_Play+0x52 : xor eax, eax
        { 0x52, 0x33 },
        { 0x53, 0xC0 },
OOVPA_END;

// ******************************************************************
// * public: long __thiscall DirectSound::CMcpxBuffer::Play(unsigned long)
// ******************************************************************
// Generic OOVPA as of 4831 and newer
OOVPA_XREF(CMcpxBuffer_Play, 4831, 11,

    XREF_CMcpxBuffer_Play,
    XRefZero)

        // CMcpxBuffer_Play+0x00 : push ebx
        { 0x00, 0x53 },

        // CMcpxBuffer_Play+0x32 : test byte ptr [esi+12h], 42h
        { 0x32, 0xF6 },
        { 0x33, 0x46 },
        { 0x34, 0x12 },
        { 0x35, 0x42 },

        // CMcpxBuffer_Play+0x4D : test byte ptr [eax+9], 20h
        { 0x4D, 0xF6 },
        { 0x4E, 0x40 },
        { 0x4F, 0x09 },
        { 0x50, 0x20 },

        // CMcpxBuffer_Play+0x71 : xor eax, eax
        { 0x71, 0x33 },
        { 0x72, 0xC0 },
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_Play2
// ******************************************************************
#define CMcpxBuffer_Play2_4627 CMcpxBuffer_Play2_4361

// ******************************************************************
// * CDirectSound::SetDistanceFactorA
// ******************************************************************
OOVPA_XREF(CDirectSound_SetDistanceFactor, 4627, 17,

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

        // CDirectSound_SetDistanceFactor+0x39 : or byte ptr[eax+0xA4], 0xE0
        { 0x39, 0x80 },
        { 0x3A, 0x88 },
        { 0x3B, 0xA4 },
        { 0x3F, 0xE0 },

        // CDirectSound_SetDistanceFactor+0x4F : jz +0x0B
        { 0x4F, 0x74 },
        { 0x50, 0x0B },
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetDopplerFactor
// ******************************************************************
OOVPA_XREF(CDirectSound_SetDopplerFactor, 4627, 14,

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

        // CDirectSound_SetDopplerFactor+0x39 : or byte ptr[eax+0xA4], 0x80
        { 0x39, 0x80 },
        { 0x3A, 0x88 },
        { 0x3B, 0xA4 },
        { 0x3F, 0x80 },

        // CDirectSound_SetDopplerFactor+0x4F : jz +0x0B
        { 0x4F, 0x74 },
        { 0x50, 0x0B },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetPosition
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetPosition, 4627, 8,

    XREF_CDirectSoundVoice_SetPosition,
    XRefZero)

        // CDirectSoundVoice_SetPosition+0x0F : mov edx, [ebp+arg_4]
        { 0x0F, 0x8B },
        { 0x10, 0x55 },
        { 0x11, 0x0C },

        // CDirectSoundVoice_SetPosition+0x12 : mov [ecx+04h], edx
        { 0x12, 0x89 },
        { 0x13, 0x51 },
        { 0x14, 0x04 },

        // CDirectSoundVoice_SetPosition+0x51 : retn 14h
        { 0x52, 0x14 },
        { 0x53, 0x00 }
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetFormat
// ******************************************************************
// Generic OOVPA as of 4721 and newer
OOVPA_XREF(CDirectSoundVoice_SetFormat, 4721, 12,

    XREF_CDirectSoundVoice_SetFormat,
    XRefZero)

        { 0x00, 0x56 },

        { 0x06, 0x4E },
        { 0x0E, 0xE8 },
        { 0x18, 0x4E },
        { 0x25, 0xFF },
        { 0x2E, 0x0C },
        { 0x2F, 0xE8 }, // call CMcpxVoiceClient::SetMixBins
        { 0x34, 0x85 },
        { 0x38, 0x8B },
        { 0x3B, 0xE8 }, // call CMcpxVoiceClient::SetPitch
        { 0x41, 0xC2 },
        { 0x42, 0x08 },
OOVPA_END;

// ******************************************************************
// * CMcpxStream_Pause
// ******************************************************************
OOVPA_XREF(CMcpxStream_Pause, 4831, 11,

    XREF_CMcpxStream_Pause,
    XRefZero)

        // CMcpxStream_Pause+0x17 : and ax, 0xFFDF
        { 0x17, 0x66 },
        { 0x18, 0x25 },
        { 0x19, 0xDF },

        // CMcpxStream_Pause+0x32 : jmp +0x56
        { 0x32, 0xEB },
        { 0x33, 0x56 },

        // CMcpxStream_Pause+0x39 : movzx eax, word ptr [esi+0x12]
        { 0x39, 0x0F },
        { 0x3A, 0xB7 },
        { 0x3B, 0x46 },
        { 0x3C, 0x12 },

        // CMcpxStream_Pause+0x8B : retn 0x04
        { 0x8B, 0xC2 },
        { 0x8C, 0x04 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSound::EnableHeadphones (incorrect?)
// ******************************************************************
OOVPA_XREF(CDirectSound_EnableHeadphones, 4627, 16,

    XREF_CDirectSound_EnableHeadphones,
    XRefZero)

        // CDirectSound_EnableHeadphones+0x00 : push ebp
        { 0x00, 0x55 },

        // CDirectSound_EnableHeadphones+0x12 : movzx eax, al
        { 0x12, 0x0F },
        { 0x13, 0xB6 },
        { 0x14, 0xC0 },

        // CDirectSound_EnableHeadphones+0x29 : mov eax, 0x80004005
        { 0x29, 0xB8 },
        { 0x2A, 0x05 },
        { 0x2B, 0x40 },
        { 0x2C, 0x00 },
        { 0x2D, 0x80 },

        // CDirectSound_EnableHeadphones+0x59 : and eax, 0x7FFFFFFF
        { 0x59, 0x25 },
        { 0x5A, 0xFF },
        { 0x5B, 0xFF },
        { 0x5C, 0xFF },
        { 0x5D, 0x7F },

        // CDirectSound_EnableHeadphones+0x97 : leave
        //{ 0x97, 0xC9 },
        { 0x98, 0xC2 },
        { 0x99, 0x08 },
OOVPA_END;
#if 0 // Replaced with generic OOVPA 3911
// ******************************************************************
// * IDirectSound_EnableHeadphones
// ******************************************************************
OOVPA_XREF(IDirectSound_EnableHeadphones, 4627, 1+8,

	XRefNoSaveIndex,
	XRefOne)

		// IDirectSound_EnableHeadphones+0x14 : call [CDirectSound::EnableHeadphones]
		XREF_ENTRY( 0x15, XREF_CDirectSound_EnableHeadphones ),

		// IDirectSound_EnableHeadphones+0x04 : push dword ptr [esp+8]
		{ 0x04, 0xFF },
		{ 0x05, 0x74 },
		{ 0x06, 0x24 },
		{ 0x07, 0x08 },

		// IDirectSound_EnableHeadphones+0x13 : push ecx
		{ 0x13, 0x51 },

		// IDirectSound_EnableHeadphones+0x19 : retn 8
		{ 0x19, 0xC2 },
		{ 0x1A, 0x08 },
		{ 0x1B, 0x00 },
OOVPA_END;
#endif

// ******************************************************************
// * CDirectSound::SetAllParametersA
// ******************************************************************
OOVPA_XREF(CDirectSound_SetAllParametersA, 4831, 10,

    XREF_CDirectSound_SetAllParameters,
    XRefZero)

        { 0x16, 0x68 },
        { 0x17, 0x08 },
        { 0x18, 0x6E },
        { 0x19, 0x18 },

        { 0x2F, 0xD9 },
        { 0x30, 0x41 },
        { 0x31, 0x04 },

        { 0x87, 0x8B },
        { 0x88, 0x50 },
        { 0x89, 0x08 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSound::GetOutputLevels
// ******************************************************************
OOVPA_XREF(CDirectSound_GetOutputLevels, 4627, 7,

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
OOVPA_XREF(IDirectSound_GetOutputLevels, 4627, 1+8,

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
// * DirectSound::CMcpxBuffer::Pause
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_Pause, 4831, 7,

    XREF_CMcpxBuffer_Pause,
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
OOVPA_XREF(CDirectSoundBuffer_Pause, 4928, 1+10,

    XREF_CDirectSoundBuffer_Pause,
    XRefOne)

        XREF_ENTRY( 0x35, XREF_CMcpxBuffer_Pause ),

        { 0x00, 0x56 },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x39, 0x85 },
        { 0x44, 0xFF },
        { 0x4B, 0xC7 },
        { 0x4E, 0xC2 },
        { 0x4F, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_Pause
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_Pause, 4928, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_CDirectSoundBuffer_Pause ),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

#if 0 // Used 4134
// ******************************************************************
// * IDirectSoundBuffer_GetStatus
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_GetStatus, 4721, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_CDirectSoundBuffer_GetStatus ),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;
#endif

#if 0 // Replaced with generic OOVPA 4134
// ******************************************************************
// * DirectSound::CDirectSoundStream::SetMixBins
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMixBins, 4627, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_SetMixBins ),

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
OOVPA_END;
#endif

#if 0 // Moved to 4242
// ******************************************************************
// * DirectSound::CMcpxVoiceClient::SetEG
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_SetEG, 4627, 8,

    XREF_CMcpxVoiceClient_SetEG,
    XRefZero)

        { 0x1E, 0x75 },
        { 0x3E, 0x0F },
        { 0x5E, 0xE3 },
        { 0x7E, 0x4E },
        { 0x9E, 0x40 },
        { 0xBE, 0x50 },
        { 0xDE, 0x1C },
        { 0xFE, 0x1C },
OOVPA_END;
#endif
#if 0 // Moved to 4039
// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetEG
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetEG, 4627, 1+6,

    XREF_CDirectSoundVoice_SetEG,
    XRefOne)

        XREF_ENTRY( 0x0C, XREF_CMcpxVoiceClient_SetEG ),

        { 0x01, 0x44 },
        { 0x04, 0xFF },
        { 0x07, 0x08 },
        { 0x0A, 0x0C },
        { 0x10, 0xC2 },
        { 0x11, 0x08 },
OOVPA_END;
#endif
#if 0 // Moved to 4134
// ******************************************************************
// * DirectSound::CDirectSoundStream::SetEG
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetEG, 4627, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_SetEG ),

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
OOVPA_END;
#endif
#if 0 // Moved to 4242
// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetEG
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetEG, 4627, 1+7,

    XREF_CDirectSoundBuffer_SetEG,
    XRefOne)

        XREF_ENTRY( 0x32, XREF_CDirectSoundVoice_SetEG ),

        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0x10 },
        { 0x3A, 0x74 },
        { 0x47, 0x8B },
OOVPA_END;
#endif
#if 0 // Moved to 4039
// ******************************************************************
// * IDirectSoundBuffer_SetEG
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetEG, 4627, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_CDirectSoundBuffer_SetEG ),

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
// * DirectSound::CDirectSoundStream::FlushEx
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_FlushEx, 4627, 14,

    XREF_CDirectSoundStream_FlushEx,
    XRefZero)

        { 0x0F, 0x00 },
        { 0x18, 0x0B },
        { 0x24, 0xB8 },
        { 0x25, 0x05 },
        { 0x26, 0x40 },
        { 0x27, 0x00 },
        { 0x28, 0x80 },
        { 0x29, 0xEB },
        { 0x2A, 0x36 },
        { 0x2B, 0x83 },
        { 0x2C, 0x7D },
        { 0x3F, 0x75 },
        { 0x4D, 0x85 },
        { 0x59, 0x15 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundStream_FlushEx
// ******************************************************************
OOVPA_XREF(IDirectSoundStream_FlushEx, 4627, 1+8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x11, XREF_CDirectSoundStream_FlushEx ),

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
// * XAudioDownloadEffectsImage
// ******************************************************************
// TODO: Need to evaluate with 5558 title, I believe it needs more OVs.
OOVPA_NO_XREF(XAudioDownloadEffectsImage, 4361, 8)

        { 0x1E, 0xB6 },
        { 0x41, 0x83 },
        { 0x61, 0x8B },
        { 0x80, 0x85 },
        { 0x9E, 0x8B },
        { 0xBE, 0x6A },
        { 0xDE, 0x07 },
        { 0xFE, 0xF7 },
OOVPA_END;
#if 0 // Moved to 4134
// ******************************************************************
// * DirectSound::CDirectSoundStream::SetFilter
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetFilter, 4627, 1+7,

    XREF_CDirectSoundStream_SetFilter,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_SetFilter ),

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
OOVPA_END;
#endif
#if 0 // Moved to 4134
// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetPitch
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetPitch, 4627, 1+7,

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
#endif
#if 0 // No longer used, replaced by generic 3936 version
// ******************************************************************
// * IDirectSoundBuffer_SetPitch
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetPitch, 4627, 1+7,

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
#endif

#if 0 // Moved to 4039
// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetOutputBuffer
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetOutputBuffer, 4627, 7,

    XREF_CDirectSoundVoice_SetOutputBuffer,
    XRefZero)

        { 0x09, 0xF7 },
        { 0x14, 0x8B },
        { 0x1F, 0x3B },
        { 0x2A, 0xE8 },
        { 0x35, 0x8B },
        { 0x40, 0x74 },
        { 0x4B, 0xD8 },
OOVPA_END;
#endif

#if 0 // Replaced with generic OOVPA 4134
// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetOutputBuffer
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetOutputBuffer, 4627, 1+7,

    XREF_CDirectSoundBuffer_SetOutputBuffer,
    XRefOne)

        XREF_ENTRY( 0x32, XREF_CDirectSoundVoice_SetOutputBuffer ),

        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0x10 },
        { 0x3A, 0x74 },
        { 0x47, 0x8B },
OOVPA_END;
#endif

#if 0 // Moved to 3911
// ******************************************************************
// * IDirectSoundBuffer_SetOutputBuffer
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetOutputBuffer, 4627, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_CDirectSoundBuffer_SetOutputBuffer ),

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
// * DirectSound::CDirectSoundVoice::SetRolloffCurve
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetRolloffCurve, 4627, 11,

    XREF_CDirectSoundVoice_SetRolloffCurve,
    XRefZero)

        { 0x07, 0x8B },
        { 0x10, 0x08 },
        { 0x11, 0x89 },
        { 0x12, 0x51 },
        { 0x13, 0x70 },
        { 0x14, 0x8B },
        { 0x19, 0xB4 },
        { 0x22, 0x51 },
        { 0x2B, 0x00 },
        { 0x34, 0x10 },
        { 0x3E, 0x33 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetRolloffCurve
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetRolloffCurve, 4627, 1+7,

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
OOVPA_XREF(CDirectSoundStream_SetRolloffCurve, 4627, 1+7,

    XRefNoSaveIndex,
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
OOVPA_XREF(IDirectSoundBuffer_SetRolloffCurve, 4627, 1+7,

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
// * DirectSound::CMemoryManager::MemAlloc
// ******************************************************************
// * FOR DEBUGGING USE ONLY!
// ******************************************************************
OOVPA_NO_XREF(CMemoryManager_MemAlloc, 4627, 10)

		// DirectSound::CMemoryManager::PoolAlloc + 0x1B: cmp [esp+4+0xC], 0
		{ 0x1B, 0x83 },
		{ 0x1C, 0x7C },
		{ 0x1D, 0x24 },
		{ 0x1E, 0x10 },
		{ 0x1F, 0x00 },
		// DirectSound::CMemoryManager::PoolAlloc + 0x33: and ecx, 3
		{ 0x33, 0x83 },
		{ 0x34, 0xE1 },
		{ 0x35, 0x03 },
		// DirectSound::CMemoryManager::PoolAlloc + 0x65: retn 0xC
		{ 0x65, 0xC2 },
		{ 0x66, 0x0C }
OOVPA_END;

// ******************************************************************
// * XWaveFileCreateMediaObject
// ******************************************************************
OOVPA_NO_XREF(XWaveFileCreateMediaObject, 4627, 10)

        { 0x0D, 0x0B },
        { 0x1C, 0xF7 },
        { 0x2B, 0x07 },
        { 0x3A, 0xF0 },
        { 0x3F, 0x83 },
        { 0x49, 0x10 },
        { 0x5C, 0x8B },
        { 0x67, 0x07 },
        { 0x70, 0xC2 },
        { 0x71, 0x0C },
OOVPA_END;

// ******************************************************************
// * XFileCreateMediaObjectEx
// ******************************************************************
OOVPA_NO_XREF(XFileCreateMediaObjectEx, 4627, 7)

        { 0x0A, 0xC0 },
        { 0x16, 0xF8 },
        { 0x22, 0xE6 },
        { 0x2E, 0x1C },
        { 0x3A, 0xF0 },
        { 0x46, 0x4C },
        { 0x52, 0xFF },
OOVPA_END;
#if 0 // Duplicate OOVPA from 3911
// ******************************************************************
// * IDirectSoundBuffer_SetNotificationPositions
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetNotificationPositions, 4627, 1+8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_CDirectSoundBuffer_SetNotificationPositions ),

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
#if 0 // Moved to 4242
// ******************************************************************
// * CMcpxVoiceClient::SetLFO
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_SetLFO, 4627, 8,

    XREF_CMcpxVoiceClient_SetLFO,
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
#endif
#if 0 // Moved to 4039
// ******************************************************************
// * CDirectSoundVoice::SetLFO
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetLFO, 4627, 1+6,

    XREF_CDirectSoundVoice_SetLFO,
    XRefOne)

        XREF_ENTRY( 0x0C, XREF_CMcpxVoiceClient_SetLFO ),

        { 0x01, 0x44 },
        { 0x04, 0xFF },
        { 0x07, 0x08 },
        { 0x0A, 0x0C },
        { 0x10, 0xC2 },
        { 0x11, 0x08 },
OOVPA_END;
#endif
#if 0 // Moved to 4242
// ******************************************************************
// * CDirectSoundBuffer::SetLFO
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetLFO, 4627, 1+7,

    XREF_CDirectSoundBuffer_SetLFO,
    XRefOne)

        XREF_ENTRY( 0x32, XREF_CDirectSoundVoice_SetLFO ),

        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0x10 },
        { 0x3A, 0x74 },
        { 0x47, 0x8B },
OOVPA_END;
#endif
#if 0 // Moved to 4039
// ******************************************************************
// * IDirectSoundBuffer_SetLFO
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetLFO, 4627, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_CDirectSoundBuffer_SetLFO ),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;
#endif
#if 0 // Moved to 4134
// ******************************************************************
// * DirectSound::CDirectSoundStream::SetLFO
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetLFO, 4627, 1+9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_SetLFO ),

        { 0x0C, 0x00 },
        { 0x0F, 0xF0 },
        { 0x12, 0x85 },
        { 0x15, 0x0B },
        { 0x3A, 0x85 },
        { 0x3D, 0xF8 },
        { 0x40, 0x68 },
        { 0x4F, 0xC2 },
        { 0x50, 0x08 },
OOVPA_END;
#endif
#if 0 // Moved to 4134
// ******************************************************************
// * DirectSound::CDirectSoundStream::SetPitch
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetPitch, 4627, 1+11,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_DSBUFFERSETPITCHB ),

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
        { 0x4F, 0xC2 },
        { 0x50, 0x08 },
        { 0x51, 0x00 },
OOVPA_END;
#endif

// ******************************************************************
// * CDirectSoundVoice::SetDistanceFactor
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetDistanceFactor, 4627, 12,

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
        { 0x1D, 0x80 },
        { 0x1F, 0x78 },

        // CDirectSoundVoice_SetDistanceFactor+0x30 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetDopplerFactor
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetDopplerFactor, 4627, 12,

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
        { 0x1D, 0x80 },
        { 0x1F, 0x78 },

        // CDirectSoundVoice_SetDopplerFactor+0x31 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 }
OOVPA_END;
