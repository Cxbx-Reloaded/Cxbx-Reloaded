// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->DSound.1.0.4627.cpp
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
// * CDirectSound::SetMixBinHeadroom
// ******************************************************************
OOVPA_XREF(CDirectSound_SetMixBinHeadroom, 4627, 15,

    XREF_DSSETMIXBINHEADROOMA,
    XRefZero)

        // CDirectSound_SetMixBinHeadroom+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 },
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // CDirectSound_SetMixBinHeadroom+0x34 : mov bl, [esp+0x14]
        { 0x34, 0x8A },
        { 0x35, 0x5C },
        { 0x36, 0x24 },
        { 0x37, 0x14 },

        // CDirectSound_SetMixBinHeadroom+0x39 : mov [edx+ecx+0x14], bl
        { 0x39, 0x88 },
        { 0x3A, 0x5C },
        { 0x3B, 0x0A },
        { 0x3C, 0x14 },

        // CDirectSound_SetMixBinHeadroom+0x5C : retn 0x0C
        { 0x5C, 0xC2 },
        { 0x5D, 0x0C },
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetMixBinHeadroom
// ******************************************************************
OOVPA_XREF(IDirectSound_SetMixBinHeadroom, 4627, 12,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSound_SetMixBinHeadroom+0x19 : call [CDirectSound::SetMixBinHeadroom]
        XREF_ENTRY( 0x19, XREF_DSSETMIXBINHEADROOMA ),

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

// ******************************************************************
// * CDirectSound::SetVelocity
// ******************************************************************
OOVPA_XREF(CDirectSound_SetVelocity, 4627, 15,

    XREF_DSSETVELOCITYA,
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

    XREF_DSSETALLPARAMETERSA,
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

    XREF_DSSETALLPARAMETERSA,
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

    XREF_DSSETALLPARAMETERSA,
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
// * CDirectSound::SetPositionA
// ******************************************************************
OOVPA_XREF(CDirectSound_SetPositionA, 4627, 11,

    XREF_DSSETPOSITIONA,
    XRefZero)

        // CDirectSound_SetPositionA+0x2B : mov ecx, [ebp+0x08]
        { 0x2B, 0x8B },
        { 0x2C, 0x4D },
        { 0x2D, 0x08 },

        // CDirectSound_SetPositionA+0x3F : mov [edx+0x3C], edi
        { 0x3F, 0x89 },
        { 0x40, 0x7A },
        { 0x41, 0x3C },

        // CDirectSound_SetPositionA+0x4C : or word ptr [eax+0xA4], 0x01FF
        { 0x4C, 0x66 },
        { 0x4D, 0x81 },
        { 0x4E, 0x88 },
        { 0x4F, 0xA4 },
        { 0x53, 0xFF },
OOVPA_END;

// ******************************************************************
// * DirectSoundCreateBuffer
// ******************************************************************
OOVPA_XREF(DirectSoundCreateBuffer, 4242, 12,

    XRefNoSaveIndex,
    XRefOne)

        // DirectSoundCreateBuffer+0x2F : call [CDirectSound::CreateSoundBuffer]
        XREF_ENTRY( 0x2F, XREF_DSCREATESOUNDBUFFER ),

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
OOVPA_NO_XREF(CDirectSound_GetSpeakerConfig, 4627, 7)

        { 0x0B, 0x00 },
        { 0x14, 0x0B },
        { 0x20, 0xB8 },
        { 0x2A, 0x04 },
        { 0x35, 0x81 },
        { 0x40, 0x0B },
        { 0x4C, 0x33 },
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_Play
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_Play, 4721, 10,

    XREF_CMcpxBuffer_Play,
    XRefZero)

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
OOVPA_XREF(CMcpxBuffer_Play, 4831, 10,

    XREF_CMcpxBuffer_Play,
    XRefZero)

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

    XREF_SETDISTANCEFACTORA,
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

    XREF_SETDOPPLERFACTOR,
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

    XREF_DSVOICESETPOSITION,
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
OOVPA_XREF(CDirectSoundVoice_SetFormat, 4721, 8,

    XREF_CDirectSoundVoice_SetFormat,
    XRefZero)

        { 0x06, 0x4E },
        { 0x0E, 0xE8 },
        { 0x18, 0x4E },
        { 0x25, 0xFF },
        { 0x2E, 0x0C },
        { 0x34, 0x85 },
        { 0x38, 0x8B },
        { 0x43, 0x00 },
OOVPA_END;

// ******************************************************************
// * CMcpxStream_Pause
// ******************************************************************
OOVPA_XREF(CMcpxStream_Pause, 4831, 11,

    XREF_DSSTREAMPAUSE,
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
OOVPA_XREF(CDirectSound_EnableHeadphones, 4627, 15,

    XREF_CDirectSound_EnableHeadphones,
    XRefZero)

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

// ******************************************************************
// * IDirectSound_EnableHeadphones
// ******************************************************************
OOVPA_XREF(IDirectSound_EnableHeadphones, 4627, 9,

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

/*
// ******************************************************************
// * CDirectSoundVoice::SetHeadrom
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetHeadroom, 4627, 9,

	XREF_CDirectSoundVoice_SetHeadroom,
	XRefOne) // Note : This was -1, which is used nowhere else and probably an error.

		XREF_ENTRY( 0x1B, XREF_DSSTREAMSETVOLUME ),

		{ 0x0A, 0x56 },
		{ 0x0F, 0x2B },
		{ 0x10, 0xCE },
		{ 0x17, 0x8B },
		{ 0x18, 0x4A },
		{ 0x19, 0x0C },
		{ 0x20, 0xC2 },
		{ 0x21, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream::SetHeadroom
// ******************************************************************
OOVPA_NO_XREF(CDirectSoundStream_SetHeadroom, 4627, 12,

	XRefNoSaveIndex,
	XRefOne)

		XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_SetHeadroom ),

		{ 0x21, 0x8B },
		{ 0x22, 0x05 },
		{ 0x23, 0x40 },
		{ 0x24, 0x00 },
		{ 0x25, 0x80 },
		{ 0x2C, 0x57 },
		{ 0x31, 0x83 },
		{ 0x32, 0xC0 },
		{ 0x33, 0x04 },
		{ 0x4F, 0xC2 },
		{ 0x50, 0x08 },
OOVPA_END;
*/

// ******************************************************************
// * CDirectSound::SetAllParametersA
// ******************************************************************
OOVPA_XREF(CDirectSound_SetAllParametersA, 4831, 10,

    XREF_DSSETALLPARAMETERSA,
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
OOVPA_XREF(IDirectSound_GetOutputLevels, 4627, 9,

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
OOVPA_XREF(CDirectSoundBuffer_Pause, 4928, 8,

    XREF_CDirectSoundBuffer_Pause,
    XRefOne)

        XREF_ENTRY( 0x35, XREF_CMcpxBuffer_Pause ),

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x39, 0x85 },
        { 0x44, 0xFF },
        { 0x4B, 0xC7 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_Pause
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_Pause, 4928, 8,

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

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::GetStatus
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_GetStatus, 4721, 8,

    XREF_CDirectSoundBuffer_GetStatus,
    XRefOne)

        XREF_ENTRY( 0x35, XREF_CMcpxBuffer_GetStatus ),

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x39, 0x85 },
        { 0x44, 0xFF },
        { 0x4B, 0xC7 },
OOVPA_END;
#if 0 // Used 4134
// ******************************************************************
// * IDirectSoundBuffer_GetStatus
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_GetStatus, 4721, 8,

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

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetMixBins
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMixBins, 4627, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_DSSETMIXBINSB ),

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
OOVPA_END;

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

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetEG
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetEG, 4627, 7,

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

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetEG
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetEG, 4627, 8,

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

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetEG
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetEG, 4627, 8,

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

// ******************************************************************
// * IDirectSoundBuffer_SetEG
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetEG, 4627, 8,

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

// ******************************************************************
// * DirectSound::CDirectSoundStream::FlushEx
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_FlushEx, 4627, 14,

    XREF_DSFLUSHEX2,
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
OOVPA_XREF(IDirectSoundStream_FlushEx, 4627, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x11, XREF_DSFLUSHEX2 ),

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
OOVPA_NO_XREF(XAudioDownloadEffectsImage, 4627, 8)

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
// * DirectSound::CDirectSoundStream::SetFilter
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetFilter, 4627, 8,

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

#if 0 // Moved to 4134
// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetPitch
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetPitch, 4627, 8,

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
OOVPA_XREF(IDirectSoundBuffer_SetPitch, 4627, 8,

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

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetHeadroom
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetHeadroom, 4627, 8,

    XREF_CDirectSoundVoice_SetHeadroom,
    XRefOne)

	// CDirectSoundVoice_SetHeadroom+0x1B : call [CMcpxVoiceClient::SetVolume]
        XREF_ENTRY( 0x1B, XREF_DSSTREAMSETVOLUME ),

        { 0x03, 0x04 },
        { 0x08, 0x48 },
        { 0x0D, 0x24 },
        { 0x12, 0x48 },
        { 0x17, 0x8B },
        { 0x1F, 0x5E },
        { 0x21, 0x08 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetHeadroom
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetHeadroom, 4627, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x36, /*XREF_CDirectSoundVoice_SetHeadroom*/ XREF_DSBUFFERSETHEADROOMA ),

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
OOVPA_END;

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

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetOutputBuffer
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetOutputBuffer, 4627, 8,

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

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetOutputBuffer
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetOutputBuffer, 4627, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_SetOutputBuffer ),

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetOutputBuffer
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetOutputBuffer, 4627, 8,

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

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetRolloffCurve
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetRolloffCurve, 4627, 11,

    XREF_DSVOICESETROLLOFFCURVE,
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
OOVPA_XREF(CDirectSoundBuffer_SetRolloffCurve, 4627, 8,

    XREF_DSBUFFERSETROLLOFFCURVE,
    XRefOne)

        XREF_ENTRY( 0x3A, XREF_DSVOICESETROLLOFFCURVE ),

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
OOVPA_XREF(CDirectSoundStream_SetRolloffCurve, 4627, 8,

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
// * IDirectSoundBuffer_SetRolloffCurve
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetRolloffCurve, 4627, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_DSBUFFERSETROLLOFFCURVE ),

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
OOVPA_NO_XREF(XWaveFileCreateMediaObject, 4627, 7)

        { 0x0D, 0x0B },
        { 0x1C, 0xF7 },
        { 0x2B, 0x07 },
        { 0x3A, 0xF0 },
        { 0x49, 0x10 },
        { 0x5C, 0x8B },
        { 0x67, 0x07 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetNotificationPositions
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetNotificationPositions, 4627, 10,

    XREF_CDirectSoundBuffer_SetNotificationPositions,
    XRefZero)

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

// ******************************************************************
// * IDirectSoundBuffer_SetNotificationPositions
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetNotificationPositions, 4627, 9,

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

// ******************************************************************
// * CDirectSoundVoice::SetLFO
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetLFO, 4627, 7,

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

// ******************************************************************
// * CDirectSoundBuffer::SetLFO
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetLFO, 4627, 8,

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

// ******************************************************************
// * IDirectSoundBuffer_SetLFO
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetLFO, 4627, 8,

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

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetLFO
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetLFO, 4627, 10,

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

#if 0 // Moved to 4134
// ******************************************************************
// * DirectSound::CDirectSoundStream::SetPitch
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetPitch, 4627, 12,

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

// ******************************************************************
// * DSound_4627
// ******************************************************************
OOVPATable DSound_4627[] = {

	REGISTER_OOVPA(DirectSoundCreate, 4134, PATCH),
	REGISTER_OOVPA(DirectSoundDoWork, 4134, PATCH),
	REGISTER_OOVPA(DirectSoundGetSampleTime, 4361, PATCH),
	REGISTER_OOVPA(CDirectSound_CreateSoundBuffer, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetPlayRegion, 4361, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetPlayRegion, 4361, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_SetLoopRegion, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetLoopRegion, 3911, PATCH),
	REGISTER_OOVPA(CDirectSound_SetI3DL2Listener, 4134, XREF),
	REGISTER_OOVPA(IDirectSound_SetI3DL2Listener, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_SetMixBinHeadroom, 4627, XREF),
	REGISTER_OOVPA(IDirectSound_SetMixBinHeadroom, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_SetHeadroom, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetHeadroom, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_SetVelocity, 4627, XREF),
	REGISTER_OOVPA(IDirectSound_SetVelocity, 3936, PATCH),
	REGISTER_OOVPA(CDirectSound_SetAllParametersA, 4627, XREF),
	REGISTER_OOVPA(CDirectSound_SetAllParametersA, 4721, XREF),
	REGISTER_OOVPA(CDirectSound_SetAllParametersA, 4831, XREF),
	REGISTER_OOVPA(CDirectSound_SetAllParameters, 4831, XREF),
	REGISTER_OOVPA(IDirectSound_SetAllParameters, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundVoiceSettings_SetMixBins, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetMixBins, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMixBins, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMixBins, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoiceSettings_SetMixBinVolumes, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetMixBinVolumes, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMixBinVolumes, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMixBinVolumes2, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_SetPositionA, 4627, XREF),
	REGISTER_OOVPA(CDirectSound_SetPositionA, 4134, XREF), // TODO: Find a cure for laziness...
	REGISTER_OOVPA(IDirectSound_SetPosition, 3936, PATCH),
	REGISTER_OOVPA(DirectSoundCreateBuffer, 4242, PATCH),
	REGISTER_OOVPA(IDirectSound_CreateSoundBuffer, 3911, PATCH),
	REGISTER_OOVPA(IDirectSound_AddRef, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetFrequency, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetFrequency, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetFrequency, 4039, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_SetVolume, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetVolume, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetVolume, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetVolume, 4039, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetVolume, 4134, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_LockA, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Lock, 3936, PATCH),
	REGISTER_OOVPA(CDirectSound_CreateSoundStream, 4134, XREF),
	REGISTER_OOVPA(IDirectSound_CreateSoundStream, 3936, PATCH),
	REGISTER_OOVPA(DirectSoundCreateStream, 4134, PATCH),
	REGISTER_OOVPA(CMcpxStream_Pause, 4361, XREF),
	REGISTER_OOVPA(CMcpxStream_Pause, 4831, XREF),
	REGISTER_OOVPA(CDirectSoundStream_Pause, 4361, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_SetBufferData, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetBufferData, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetBufferData, 4134, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_GetStatus, 4134, XREF),
	REGISTER_OOVPA(CMcpxBuffer_GetStatus, 4721, XREF),
	REGISTER_OOVPA(CMcpxBuffer_GetStatus, 4831, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_GetStatus, 4721, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_GetStatus, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_GetStatus, 4134, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_SetCurrentPosition, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetCurrentPosition, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetCurrentPosition, 4134, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_GetCurrentPosition, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_GetCurrentPosition, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_GetCurrentPosition, 3936, PATCH),
	REGISTER_OOVPA(CDirectSound_GetSpeakerConfig, 4627, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_Play, 4361, XREF),
	REGISTER_OOVPA(CMcpxBuffer_Play, 4721, XREF),
	REGISTER_OOVPA(CMcpxBuffer_Play, 4831, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_Play, 4361, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Play, 4361, PATCH),
	REGISTER_OOVPA(IDirectSound_Release, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound_DownloadEffectsImage, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound_SetOrientation, 3936, PATCH),
	REGISTER_OOVPA(CDirectSound_SetDistanceFactor, 4134, XREF),
	REGISTER_OOVPA(CDirectSound_SetDistanceFactor, 4627, XREF),
	REGISTER_OOVPA(IDirectSound_SetDistanceFactor, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_SetRolloffFactor, 4134, XREF),
	REGISTER_OOVPA(IDirectSound_SetRolloffFactor, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_SetDopplerFactor, 4134, XREF),
	REGISTER_OOVPA(CDirectSound_SetDopplerFactor, 4627, XREF),
	REGISTER_OOVPA(IDirectSound_SetDopplerFactor, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_CommitDeferredSettings, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetMaxDistance, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMaxDistance, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMaxDistance, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetMinDistance, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMinDistance, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMinDistance, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetRolloffFactor, 4134, XREF), // s+
	REGISTER_OOVPA(CDirectSoundBuffer_SetRolloffFactor, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetRolloffFactor, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetDistanceFactor, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetDistanceFactor, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetDistanceFactor, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetConeAngles, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetConeAngles, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetConeAngles, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetConeOrientation, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetConeOrientation, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetConeOrientation, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetConeOutsideVolume, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetConeOutsideVolume, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetConeOutsideVolume, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetPosition, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetPosition, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetPosition, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetVelocity, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetVelocity, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetVelocity, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetDopplerFactor, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetDopplerFactor, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetDopplerFactor, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetI3DL2Source, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetI3DL2Source, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetI3DL2Source, 4134, PATCH),
    REGISTER_OOVPA(CMcpxBuffer_Stop, 4242, XREF),
    REGISTER_OOVPA(CDirectSoundBuffer_Stop, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Stop, 3911, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_Release, 3936, PATCH), // +s
	REGISTER_OOVPA(CDirectSoundVoice_SetFormat, 4242, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetFormat, 4721, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetFormat, 4242, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetFormat, 4242, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetFormat, 4242, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetMixBinVolumes2, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_EnableHeadphones, 4627, XREF),
	REGISTER_OOVPA(IDirectSound_EnableHeadphones, 4627, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_AddRef, 3936, PATCH),
	REGISTER_OOVPA(CDirectSound_GetOutputLevels, 4627, XREF),
	REGISTER_OOVPA(IDirectSound_GetOutputLevels, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetAllParameters, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetAllParameters, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetAllParameters, 4134, PATCH), // Use that for now. Okay, it's your call pal...
	REGISTER_OOVPA(CMcpxBuffer_Pause, 4831, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_Pause, 4928, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Pause, 4928, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_GetStatus, 4721, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetMixBins, 4627, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_SetEG, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetEG, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetEG, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_SetEG, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetEG, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_FlushEx, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundStream_FlushEx, 4627, PATCH),
	REGISTER_OOVPA(XAudioDownloadEffectsImage, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetMode, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMode, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMode, 4134, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_SetFilter, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetFilter, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetFilter, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetFilter, 4627, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_SetFilter, 4134, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_Play2, 4361, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_PlayEx, 4361, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_PlayEx, 3936, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_Stop2, 4361, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_StopEx, 4361, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_StopEx, 4361, PATCH),
	REGISTER_OOVPA(CDirectSound_SetVelocity, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetPitch, 4039, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetPitch, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetPitch, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetPitch, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetHeadroom, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetHeadroom, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetOutputBuffer, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetOutputBuffer, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetOutputBuffer, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetOutputBuffer, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetRolloffCurve, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetRolloffCurve, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetRolloffCurve, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetRolloffCurve, 4627, PATCH),
	REGISTER_OOVPA(XFileCreateMediaObjectEx, 4627, PATCH),
	REGISTER_OOVPA(XWaveFileCreateMediaObject, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_SetNotificationPositions, 4627, XREF), // Uncommenting these fixes dashboard 4920
	REGISTER_OOVPA(IDirectSoundBuffer_SetNotificationPositions, 4627, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_SetLFO, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetLFO, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetLFO, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetLFO, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetLFO, 4627, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_Unlock, 3936, PATCH),
	REGISTER_OOVPA(CDirectSound_GetCaps, 4361, XREF),
	REGISTER_OOVPA(IDirectSound_GetCaps, 3936, PATCH),
};

// ******************************************************************
// * DSound_4627_SIZE
// ******************************************************************
uint32 DSound_4627_SIZE = sizeof(DSound_4627);
