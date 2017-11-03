// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->DSound.1.0.4134.inl
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
// * DirectSoundCreate
// ******************************************************************
OOVPA_NO_XREF(DirectSoundCreate, 4134, 9)
	// DirectSoundCreate+0x0B : movzx esi, al
	{ 0x0B, 0x0F },
	{ 0x0C, 0xB6 },
	{ 0x0D, 0xF0 },

					// DirectSoundCreate+0x12 : call [addr]
	{ 0x12, 0xE8 },

					// DirectSoundCreate+0x1B : jl +0x13
	{ 0x1B, 0x7C },
	{ 0x1C, 0x13 },

					// DirectSoundCreate+0x27 : sbb ecx, ecx
	{ 0x27, 0x1B },
	{ 0x28, 0xC9 },

					// DirectSoundCreate+0x43 : leave
	{ 0x43, 0xC9 },
OOVPA_END;

// ******************************************************************
// * DirectSoundDoWork
// ******************************************************************
// Generic OOVPA as of 4134 and newer
// TODO: DirectSoundDoWork 4134 has weak OV list, need to add more or at least use XREF support to be strengthen.
OOVPA_NO_XREF(DirectSoundDoWork, 4134, 7)

    { 0x06, 0x0F },
    { 0x0E, 0x85 },
    { 0x10, 0x74 },
    { 0x18, 0x85 },
    { 0x1C, 0x0B },
    { 0x22, 0xFF },
    { 0x28, 0xC3 },
OOVPA_END;

// ******************************************************************
// * CDirectSound::CreateSoundBuffer
// ******************************************************************
OOVPA_XREF(CDirectSound_CreateSoundBuffer, 4134, 14,

    XREF_CDirectSound_CreateSoundBuffer,
    XRefZero)

        // CDirectSound_CreateSoundBuffer+0x23 : mov eax, 0x80004005
        { 0x23, 0xB8 },
        { 0x24, 0x05 },
        { 0x25, 0x40 },
        { 0x27, 0x80 },

        // CDirectSound_CreateSoundBuffer+0x2A : push 0x24
        { 0x2A, 0x6A },
        { 0x2B, 0x24 },

        // CDirectSound_CreateSoundBuffer+0x4A : add esi, 0x7FF8FFF2
        { 0x4A, 0x81 },
        { 0x4B, 0xE6 },
        { 0x4C, 0xF2 },
        { 0x4D, 0xFF },
        { 0x4E, 0xF8 },
        { 0x4F, 0x7F },

        // CDirectSound_CreateSoundBuffer+0x99 : retn 0x10
        { 0x99, 0xC2 },
        { 0x9A, 0x10 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetLoopRegion
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetLoopRegion, 4134, 11,

    XREF_CDirectSoundBuffer_SetLoopRegion,
    XRefZero)

        // CDirectSoundBuffer_SetLoopRegion+0x24 : mov eax, 0x80004005
        { 0x24, 0xB8 },
        { 0x25, 0x05 },
        { 0x26, 0x40 },
        { 0x27, 0x00 },
        { 0x28, 0x80 },

        // CDirectSoundBuffer_SetLoopRegion+0x29 : jmp +0x55
        { 0x29, 0xEB },
        { 0x2A, 0x55 },

        // CDirectSoundBuffer_SetLoopRegion+0x56 : sub ecx, esi
        { 0x56, 0x2B },
        { 0x57, 0xCE },

        // CDirectSoundBuffer_SetLoopRegion+0x82 : retn 0x0C
        { 0x82, 0xC2 },
        { 0x83, 0x0C },
OOVPA_END;

#if 0 // Moved to 3936
// ******************************************************************
// * IDirectSoundBuffer8::SetLoopRegion
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetLoopRegion, 4134, 1+8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_CDirectSoundBuffer_SetLoopRegion ),

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
// * CDirectSound::SetI3DL2Listener
// ******************************************************************
OOVPA_XREF(CDirectSound_SetI3DL2Listener, 4134, 11,

    XREF_CDirectSound_SetI3DL2Listener,
    XRefZero)

        // CDirectSound_SetI3DL2Listener+0x3A : mov edi, 0x88780032
        { 0x3A, 0xBF },
        { 0x3B, 0x32 },
        { 0x3C, 0x00 },
        { 0x3D, 0x78 },
        { 0x3E, 0x88 },

        // CDirectSound_SetI3DL2Listener+0xA2 : fstp dword ptr[edx+0x94]
        { 0xA2, 0xD9 },
        { 0xA3, 0x9A },
        { 0xA4, 0x94 },

        // CDirectSound_SetI3DL2Listener+0xA8 : fld dword ptr[ecx+0x24]
        { 0xA8, 0xD9 },
        { 0xA9, 0x41 },
        { 0xAA, 0x24 },
OOVPA_END;
#if 0 // Replaced with generic OOVPA 3911
// ******************************************************************
// * IDirectSound_SetI3DL2Listener
// ******************************************************************
OOVPA_XREF(IDirectSound_SetI3DL2Listener, 4134, 1+11,

	XRefNoSaveIndex,
	XRefOne)

	// IDirectSound_SetI3DL2Listener+0x19 : call [CDirectSound::SetI3DL2Listener]
	XREF_ENTRY(0x19, XREF_CDirectSound_SetI3DL2Listener),

											   // IDirectSound_SetI3DL2Listener+0x04 : push [esp+0x0C]
	{ 0x04, 0xFF },
	{ 0x05, 0x74 },
	{ 0x06, 0x24 },
	{ 0x07, 0x0C },

					// IDirectSound_SetI3DL2Listener+0x0E : add eax, 0xFFFFFFF8
	{ 0x0E, 0x83 },
	{ 0x0F, 0xC0 },
	{ 0x10, 0xF8 },

					// IDirectSound_SetI3DL2Listener+0x13 : sbb ecx, ecx
	{ 0x13, 0x1B },
	{ 0x14, 0xC9 },

					// IDirectSound_SetI3DL2Listener+0x15 : and ecx, eax
	{ 0x15, 0x23 },
	{ 0x16, 0xC8 },
	OOVPA_END;
#endif
// ******************************************************************
// * CDirectSoundVoice_SetHeadroom
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(CDirectSoundVoice_SetHeadroom, 4134, 14,

    XREF_CDirectSoundVoice_SetHeadroom,
    XRefOne)

        // CDirectSoundVoice_SetHeadroom+0x1B : call [CMcpxVoiceClient::SetVolume]
        XREF_ENTRY( 0x1B, XREF_CMcpxVoiceClient_SetVolume ),

        { 0x00, 0x8B },
        { 0x08, 0x48 },
        { 0x0F, 0x2B },
        { 0x13, 0x1C },

        // CDirectSoundVoice_SetHeadroom+0x14 : mov dword ptr [eax+20h],esi
        { 0x14, 0x89 },
        { 0x15, 0x70 },
        { 0x16, 0x20 },

        // CDirectSoundVoice_SetHeadroom+0x14 : mov ecx,dword ptr [edx+0Ch]
        { 0x17, 0x8B },
        { 0x18, 0x4A },
        { 0x19, 0x0C },

        // CDirectSoundVoice_SetHeadroom+0x14 : call CMcpxVoiceClient::SetVolume
        { 0x1A, 0xE8 },

        // CDirectSoundVoice_SetHeadroom+0x14 :retn 8
        { 0x20, 0xC2 },
        { 0x21, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_SetHeadroom
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(CDirectSoundBuffer_SetHeadroom, 4134, 1+8,

	XREF_CDirectSoundBuffer_SetHeadroom,
	XRefOne)

        XREF_ENTRY( 0x32, XREF_CDirectSoundVoice_SetHeadroom ),

        { 0x00, 0x56 },

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
// * IDirectSoundBuffer_SetHeadroom
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetHeadroom, 4134, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetHeadroom+0x15 : call [CDirectSoundBuffer_SetHeadroom]
        XREF_ENTRY(0x15, XREF_CDirectSoundBuffer_SetHeadroom),

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
OOVPA_XREF(CDirectSoundVoiceSettings_SetMixBins, 4134, 11,

	XREF_CDirectSoundVoiceSettings_SetMixBins,
	XRefZero)

	// CDirectSoundVoiceSettings_SetMixBins+0x06 : jnz +0x1A
	{ 0x06, 0x75 },
	{ 0x07, 0x1A },

					// CDirectSoundVoiceSettings_SetMixBins+0x08 : test byte ptr [ecx+8], 0x10
	{ 0x08, 0xF6 },
	{ 0x09, 0x41 },
	{ 0x0A, 0x08 },
	{ 0x0B, 0x10 },

					// CDirectSoundVoiceSettings_SetMixBins+0x19 : shr eax, 1
	{ 0x19, 0xD1 },
	{ 0x1A, 0xE8 },

					// CDirectSoundVoiceSettings_SetMixBins+0x53 : cmp [ecx+24h], edx
	{ 0x53, 0x39 },
	{ 0x54, 0x51 },
	{ 0x55, 0x24 },
OOVPA_END;
#if 0 // Moved to 4039
// ******************************************************************
// * CDirectSoundVoice::SetMixBins
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMixBins, 4134, 1+16,

    XREF_CDirectSoundVoice_SetMixBins,
    XRefOne)

        // CDirectSoundVoice_SetMixBins+0x0D : call [CDirectSoundVoiceSettings::SetMixBins]
        XREF_ENTRY( 0x0D, XREF_CDirectSoundVoiceSettings_SetMixBins ),

        // CDirectSoundVoice_SetMixBins+0x01 : mov esi, [esp+8]
        { 0x01, 0x8B },
        { 0x02, 0x74 },
        { 0x03, 0x24 },
        { 0x04, 0x08 },

        // CDirectSoundVoice_SetMixBins+0x05 : push [esp+0x0C]
        { 0x05, 0xFF },
        { 0x06, 0x74 },
        { 0x07, 0x24 },
        { 0x08, 0x0C },

        // CDirectSoundVoice_SetMixBins+0x09 : mov ecx, [esi+0x10]
        { 0x09, 0x8B },
        { 0x0A, 0x4E },
        { 0x0B, 0x10 },

        // CDirectSoundVoice_SetMixBins+0x11 : mov ecx, [esi+0x0C]
        { 0x11, 0x8B },
        { 0x12, 0x4E },
        { 0x13, 0x0C },

        // CDirectSoundVoice_SetMixBins+0x1A : retn 0x08
        { 0x1A, 0xC2 },
        { 0x1B, 0x08 },
OOVPA_END;
#endif
// ******************************************************************
// * CDirectSoundBuffer::SetMixBins
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(CDirectSoundBuffer_SetMixBins, 4134, 1+16,

    XREF_CDirectSoundBuffer_SetMixBins,
    XRefOne)

        // CDirectSoundBuffer_SetMixBins+0x32 : call [CDirectSoundVoice::SetMixBins]
        XREF_ENTRY( 0x32, XREF_CDirectSoundVoice_SetMixBins ),

        // CDirectSoundBuffer_SetMixBins+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 },
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // CDirectSoundBuffer_SetMixBins+0x29 : push [esp+0x10]; push [esp+0x10]
        { 0x29, 0xFF },
        { 0x2A, 0x74 },
        { 0x2B, 0x24 },
        { 0x2C, 0x10 },
        { 0x2D, 0xFF },
        { 0x2E, 0x74 },
        { 0x2F, 0x24 },
        { 0x30, 0x10 },

        // CDirectSoundBuffer_SetMixBins+0x49 : pop edi
        { 0x49, 0x5F },

        // CDirectSoundBuffer_SetMixBins+0x4B : retn 0x08
        { 0x4B, 0xC2 },
        { 0x4C, 0x08 },
OOVPA_END;

#if 0 // Replaced with generic OOVPA 4039
// ******************************************************************
// * IDirectSoundBuffer_SetMixBins
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMixBins, 4134, 1+11,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetMixBins+0x15 : call [CDirectSoundBuffer::SetMixBins]
        XREF_ENTRY( 0x15, XREF_CDirectSoundBuffer_SetMixBins ),

        // IDirectSoundBuffer_SetMixBins+0x04 : push [esp+0x08]
        { 0x04, 0xFF },
        { 0x05, 0x74 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },

        // IDirectSoundBuffer_SetMixBins+0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 },
        { 0x0B, 0xC0 },
        { 0x0C, 0xE4 },

        // IDirectSoundBuffer_SetMixBins+0x0F : sbb ecx, ecx
        { 0x0F, 0x1B },
        { 0x10, 0xC9 },

        // IDirectSoundBuffer_SetMixBins+0x11 : and ecx, eax
        { 0x11, 0x23 },
        { 0x12, 0xC8 },
OOVPA_END;
#endif

// ******************************************************************
// * CDirectSound_SetPosition
// ******************************************************************
OOVPA_XREF(CDirectSound_SetPosition, 4134, 14,

    XREF_CDirectSound_SetPosition,
    XRefZero)

        // CDirectSound_SetPosition+0x00 : push ebp
        { 0x00, 0x55},

        // CDirectSound_SetPosition+0x2B : mov ecx, [ebp+0x08]
        { 0x2B, 0x8B },
        { 0x2C, 0x4D },
        { 0x2D, 0x08 },

        // CDirectSound_SetPosition+0x3F : mov [edx+0x3C], edi
        { 0x3F, 0x89 },
        { 0x40, 0x7A },
        { 0x41, 0x3C },

        // CDirectSound_SetPosition+0x4C : or word ptr [eax+0xA4], 0x01FF
        { 0x4C, 0x80 },
        { 0x4D, 0x88 },
        { 0x4E, 0xA4 },
        { 0x4F, 0x00 },
        { 0x52, 0xFF },

        // CDirectSound_SetPosition+0x73 : retn 0x14
        { 0x73, 0xC2 },
        { 0x74, 0x14 },
OOVPA_END;

// ******************************************************************
// * IDirectSound_CreateSoundBuffer
// ******************************************************************
OOVPA_XREF(IDirectSound_CreateSoundBuffer, 4134, 1+11,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSound_CreateSoundBuffer+0x1D : call [CDirectSound::CreateSoundBuffer]
        XREF_ENTRY( 0x1D, XREF_CDirectSound_CreateSoundBuffer ),

        // IDirectSound_CreateSoundBuffer+0x04 : mov eax, [esp+8]
        { 0x04, 0x8B },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },

        // IDirectSound_CreateSoundBuffer+0x12 : add eax, 0xFFFFFFF8
        { 0x12, 0x83 },
        { 0x13, 0xC0 },
        { 0x14, 0xF8 },

        // IDirectSound_CreateSoundBuffer+0x17 : sbb ecx, ecx
        { 0x17, 0x1B },
        { 0x18, 0xC9 },

        // IDirectSound_CreateSoundBuffer+0x21 : retn 0x10
        { 0x21, 0xC2 },
        { 0x22, 0x10 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetFrequency
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetFrequency, 4134, 11,

    XREF_CDirectSoundVoice_SetFrequency,
    XRefZero)

        // CDirectSoundVoice_SetFrequency+0x0D : mov eax, [esi+0x10]
        { 0x0D, 0x8B },
        { 0x0E, 0x46 },
        { 0x0F, 0x10 },

        // CDirectSoundVoice_SetFrequency+0x10 : mov eax, [eax+0x10]
        { 0x10, 0x8B },
        { 0x11, 0x40 },
        { 0x12, 0x10 },

        // CDirectSoundVoice_SetFrequency+0x14 : call [abs]; push eax; push esi
        { 0x14, 0xE8 },
        { 0x19, 0x50 },
        { 0x1A, 0x56 },

        // CDirectSoundVoice_SetFrequency+0x22 : retn 0x08
        { 0x21, 0xC2 },
        { 0x22, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetFrequency
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetFrequency, 4134, 1+16,

    XREF_CDirectSoundBuffer_SetFrequency,
    XRefOne)

        // CDirectSoundBuffer_SetFrequency+0x32 : call [CDirectSoundVoice::SetFrequency]
        XREF_ENTRY( 0x32, XREF_CDirectSoundVoice_SetFrequency ),

        // CDirectSoundBuffer_SetFrequency+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 },
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // CDirectSoundBuffer_SetFrequency+0x29 : push [esp+0x10]; push [esp+0x10]
        { 0x29, 0xFF },
        { 0x2A, 0x74 },
        { 0x2B, 0x24 },
        { 0x2C, 0x10 },
        { 0x2D, 0xFF },
        { 0x2E, 0x74 },
        { 0x2F, 0x24 },
        { 0x30, 0x10 },

        // CDirectSoundBuffer_SetFrequency+0x49 : pop edi
        { 0x49, 0x5F },

        // CDirectSoundBuffer_SetFrequency+0x4B : retn 0x08
        { 0x4B, 0xC2 },
        { 0x4C, 0x08 },
OOVPA_END;

#if 0 // Moved to 4039
// ******************************************************************
// * IDirectSoundBuffer_SetFrequency
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetFrequency, 4134, 1+11,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetFrequency+0x15 : call [CDirectSound::SetFrequency]
        XREF_ENTRY( 0x15, XREF_CDirectSoundBuffer_SetFrequency ),

        // IDirectSoundBuffer_SetFrequency+0x04 : push [esp+0x08]
        { 0x04, 0xFF },
        { 0x05, 0x74 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },

        // IDirectSoundBuffer_SetFrequency0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 },
        { 0x0B, 0xC0 },
        { 0x0C, 0xE4 },

        // IDirectSoundBuffer_SetFrequency+0x0F : sbb ecx, ecx
        { 0x0F, 0x1B },
        { 0x10, 0xC9 },

        // IDirectSoundBuffer_SetFrequency+0x11 : and ecx, eax
        { 0x11, 0x23 },
        { 0x12, 0xC8 },
OOVPA_END;
#endif

// ******************************************************************
// * CMcpxVoiceClient_SetVolume
// ******************************************************************
// Generic OOVPA as of 4134 and newer.
OOVPA_XREF(CMcpxVoiceClient_SetVolume, 4134, 13,

    XREF_CMcpxVoiceClient_SetVolume,
    XRefZero)

        // CMcpxVoiceClient_SetVolume+0x2A : lea eax, [ecx+ecx*2]
        { 0x2A, 0x8D },
        { 0x2B, 0x04 },
        { 0x2C, 0x49 },

        // CMcpxVoiceClient_SetVolume+0x45 : movzx edx, word ptr [ecx]
        { 0x45, 0x0F },
        { 0x46, 0xB7 },
        { 0x47, 0x11 },

        // CMcpxVoiceClient_SetVolume+0x6C : mov edx, [ebp+eax*4-0x14]
        { 0x6C, 0x8B },
        { 0x6D, 0x54 },
        { 0x6E, 0x85 },
        { 0x6F, 0xEC },

        // CMcpxVoiceClient_SetVolume+0x84 : inc eax; inc ecx, inc ecx
        { 0x84, 0x40 },
        { 0x85, 0x41 },
        { 0x86, 0x41 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice_SetVolume
// ******************************************************************
// Generic OOVPA as of 4134 and newer.
OOVPA_XREF(CDirectSoundVoice_SetVolume, 4134, 1+10,

    XREF_CDirectSoundVoice_SetVolume,
    XRefOne)

        // CDirectSoundVoice_SetVolume+0x15 : call [CMcpxVoiceClient::SetVolume]
        XREF_ENTRY( 0x15, XREF_CMcpxVoiceClient_SetVolume ), 

        // CDirectSoundVoice_SetVolume+0x00 : mov ecx, [esp+0x04]
        { 0x00, 0x8B },
        { 0x01, 0x4C },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // CDirectSoundVoice_SetVolume+0x0B : sub edx, [eax+0x20]
        { 0x0B, 0x2B },
        { 0x0C, 0x50 },
        { 0x0D, 0x20 },

        // CDirectSoundVoice_SetVolume+0x11 : mov ecx, [ecx+0x0C]
        { 0x11, 0x8B },
        { 0x12, 0x49 },
        { 0x13, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetVolume
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetVolume, 4134, 1+8,

    XREF_CDirectSoundStream_SetVolume,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_SetVolume ),

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
// * CDirectSoundBuffer_Lock
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(CDirectSoundBuffer_Lock, 4134, 1+12,

    XREF_CDirectSoundBuffer_Lock,
    XRefOne)

        XREF_ENTRY( 0x48, XREF_CDirectSoundBuffer_GetCurrentPosition ),

        { 0x00, 0x55 },

        // CDirectSoundBuffer_Lock+0x37 : test [ebp+0x24], 1
        { 0x37, 0xF6 },
        { 0x38, 0x45 },
        { 0x39, 0x24 },
        { 0x3A, 0x01 },

        // CDirectSoundBuffer_Lock+0x5C : test byte ptr [ebp+24h],2
        { 0x53, 0xF6 },
        { 0x54, 0x45 },
        { 0x55, 0x24 },
        { 0x56, 0x02 },

        // CDirectSoundBuffer_Lock+0x81 : sub ecx,dword ptr [ebp+0Ch]
        { 0x81, 0x2B },
        { 0x82, 0x4D },
        { 0x83, 0x0C },

        // Major changes occur at 0x84 and later
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_SetBufferData
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_SetBufferData, 4134, 9,

    XREF_CMcpxBuffer_SetBufferData,
    XRefZero)

        // CMcpxBuffer_SetBufferData+0x1E : cmp eax, ebx
        { 0x1E, 0x3B },
        { 0x1F, 0xC3 },

        // CMcpxBuffer_SetBufferData+0x20 : jz +0x0C
        { 0x20, 0x74 },
        { 0x21, 0x0C },

        // CMcpxBuffer_SetBufferData+0x45 : mov [esi+0xB8], ecx
        { 0x45, 0x89 },
        { 0x46, 0x8E },
        { 0x47, 0xB8 },

        // CMcpxBuffer_SetBufferData+0x89 : retn 0x08
        { 0x89, 0xC2 },
        { 0x8A, 0x08 },
OOVPA_END;

#if 0 // Replaced with generic OOVPA 4134
// ******************************************************************
// * CDirectSoundBuffer_SetBufferData
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetBufferData, 4134, 1+9,

    XREF_CDirectSoundBuffer_SetBufferData,
    XRefOne)

        // CDirectSoundBuffer_SetBufferData+0x7A : call [CMcpxBuffer::SetBufferData]
        XREF_ENTRY( 0x7A, XREF_CMcpxBuffer_SetBufferData), 

        // CDirectSoundBuffer_SetBufferData+0x40 : mov eax, 0x80004005
        { 0x40, 0xB8 },
        { 0x41, 0x05 },
        { 0x42, 0x40 },
        { 0x43, 0x00 },
        { 0x44, 0x80 },

        // CDirectSoundBuffer_SetBufferData+0x5F : jz +0x33
        { 0x5F, 0x74 },
        { 0x60, 0x33 },

        // CDirectSoundBuffer_SetBufferData+0xAB : retn 0x0C
        { 0xAB, 0xC2 },
        { 0xAC, 0x0C },
OOVPA_END;
#endif

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetBufferData
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(CDirectSoundBuffer_SetBufferData, 4134, 19,

    XREF_CDirectSoundBuffer_SetBufferData,
    XRefZero)

        // CDirectSoundBuffer_SetBufferData+0x00 : push ebp
        { 0x00, 0x55 },

        // CDirectSoundBuffer_SetBufferData+0x0F : cmp dword ptr [ebp+10h],edi
        { 0x0F, 0x39 },
        { 0x10, 0x7D },
        { 0x11, 0x10 },

        // CDirectSoundBuffer_SetBufferData+0x12 : mov dword ptr [ebp+0Ch],edi
        { 0x12, 0x89 },
        { 0x13, 0x7D },
        { 0x14, 0x0C },

        // CDirectSoundBuffer_SetBufferData+0x4B : mov eax,dword ptr [esi+1Ch]
        { 0x4B, 0x8B },
        { 0x4C, 0x46 },
        { 0x4D, 0x1C },

        // CDirectSoundBuffer_SetBufferData+0x64 : mov ecx,dword ptr [esi+20h]
        { 0x64, 0x8B },
        { 0x65, 0x4E },
        { 0x66, 0x20 },

        // CDirectSoundBuffer_SetBufferData+0x8A : mov ecx,dword ptr [esi+20h]
        { 0x8A, 0x8B },
        { 0x8B, 0x4E },
        { 0x8C, 0x20 },

        // CDirectSoundBuffer_SetBufferData+0xAB : retn 0x0C
        { 0xAB, 0xC2 },
        { 0xAC, 0x0C },
        { 0xAD, 0x00 },
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_GetStatus
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_GetStatus, 4134, 11,

	XREF_CMcpxBuffer_GetStatus,
    XRefZero)

        // CMcpxBuffer_GetStatus+0x0A : mov esi, ecx
        { 0x0A, 0x8B },
        { 0x0B, 0xF1 },

        // CMcpxBuffer_GetStatus+0x20 : jz +0x18
        { 0x20, 0x74 },
        { 0x21, 0x18 },

        // CMcpxBuffer_GetStatus+0x22 : test word ptr [esi+0x12], 0x8002
        { 0x22, 0x66 },
        { 0x23, 0xF7 },
        { 0x24, 0x46 },
        { 0x25, 0x12 },

        // CMcpxBuffer_GetStatus+0x2C : test word ptr [esi+0x12], 0x200
        { 0x2C, 0x66 },
        { 0x2D, 0xF7 },
        { 0x2E, 0x46 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_GetStatus
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_GetStatus, 4134, 1+9,

	XREF_CDirectSoundBuffer_GetStatus,
	XRefOne)

	// CDirectSoundBuffer_GetStatus+0x35 : call [CMcpxBuffer::GetStatus]
	XREF_ENTRY(0x35, XREF_CMcpxBuffer_GetStatus), 

												// CDirectSoundBuffer_GetStatus+0x21 : mov eax, 0x80004005
	{ 0x21, 0xB8 },
	{ 0x22, 0x05 },
	{ 0x23, 0x40 },
	{ 0x24, 0x00 },
	{ 0x25, 0x80 },

					// CDirectSoundBuffer_GetStatus+0x3D : jz +0x0B
	{ 0x3D, 0x74 },
	{ 0x3E, 0x0B },

					// CDirectSoundBuffer_GetStatus+0x4E : retn 0x08
	{ 0x4E, 0xC2 },
	{ 0x4F, 0x08 },
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_SetCurrentPosition
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_SetCurrentPosition, 4134, 9,

	XREF_CMcpxBuffer_SetCurrentPosition,
    XRefZero)

        // CMcpxBuffer_SetCurrentPosition+0x11 : mov al, [esi+12]
        { 0x11, 0x8A },
        { 0x12, 0x46 },
        { 0x13, 0x12 },

        // CMcpxBuffer_SetCurrentPosition+0x5F : jb +0x10
        { 0x5F, 0x72 },
        { 0x60, 0x10 },

        // CMcpxBuffer_SetCurrentPosition+0x86 : movzx eax, byte ptr [esi+0x64]
        { 0x86, 0x0F },
        { 0x87, 0xB6 },
        { 0x88, 0x46 },
        { 0x89, 0x64 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_SetCurrentPosition
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetCurrentPosition, 4134, 1+9,

	XREF_CDirectSoundBuffer_SetCurrentPosition,
    XRefOne)

        // CDirectSoundBuffer_SetCurrentPosition+0x35 : call [CMcpxBuffer::SetCurrentPosition]
        XREF_ENTRY( 0x35, XREF_CMcpxBuffer_SetCurrentPosition), 

        // CDirectSoundBuffer_SetCurrentPosition+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 },
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // CDirectSoundBuffer_SetCurrentPosition+0x3D : jz +0x0B
        { 0x3D, 0x74 },
        { 0x3E, 0x0B },

        // CDirectSoundBuffer_SetCurrentPosition+0x3D : retn 0x08
        { 0x4E, 0xC2 },
        { 0x4F, 0x08 },
OOVPA_END;

#if 0 // Replaced with generic OOVPA 3911
// ******************************************************************
// * IDirectSoundBuffer_SetCurrentPosition
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetCurrentPosition, 4134, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetCurrentPosition+0x15 : call [CDirectSoundBuffer::SetCurrentPosition]
        XREF_ENTRY( 0x15, XREF_CDirectSoundBuffer_SetCurrentPosition), 

        // IDirectSoundBuffer_SetCurrentPosition+0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 },
        { 0x0B, 0xC0 },
        { 0x0C, 0xE4 },

        // IDirectSoundBuffer_SetCurrentPosition+0x0F : sbb ecx, ecx
        { 0x0F, 0x1B },
        { 0x10, 0xC9 },

        // IDirectSoundBuffer_SetCurrentPosition+0x11 : and ecx, eax
        { 0x11, 0x23 },
        { 0x12, 0xC8 },
OOVPA_END;
#endif

// ******************************************************************
// * CMcpxBuffer_GetCurrentPosition
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(CMcpxBuffer_GetCurrentPosition, 4134, 14,

    XREF_CMcpxBuffer_GetCurrentPosition,
    XRefZero)

        // CMcpxBuffer_GetCurrentPosition+0x00 : push ebp
        { 0x00, 0x55 },

        // CMcpxBuffer_GetCurrentPosition+0x0E : lea ecx, [ebp-0x08]
        { 0x0E, 0x8D },
        { 0x0F, 0x4D },
        { 0x10, 0xF8 },

        { 0x25, 0xE8 },

        // CMcpxBuffer_GetCurrentPosition+0x31 : jz +0x02
        { 0x31, 0x74 },
        { 0x32, 0x02 },

        { 0x44, 0xE8 },

        // CMcpxBuffer_GetCurrentPosition+0x5C : test word ptr [esi+12h],200h
        { 0x5C, 0x66 },
        { 0x5D, 0xF7 },
        { 0x5E, 0x46 },
        { 0x5F, 0x12 },
        { 0x60, 0x00 },
        { 0x61, 0x02 },

        // Offset 0x62 and later has changes.

        // CMcpxBuffer_GetCurrentPosition+0xA0 : div dword ptr [ecx+0xBC]
        //{ 0xA0, 0xF7 },
        //{ 0xA1, 0xB1 },
        //{ 0xA2, 0xBC },

        // CMcpxBuffer_GetCurrentPosition+0xD6 : retn 0x08
        //{ 0xD6, 0xC2 },
        //{ 0xD7, 0x08 },
        //{ 0xD8, 0x00 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_GetCurrentPosition
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_GetCurrentPosition, 4134, 1+9,

	XREF_CDirectSoundBuffer_GetCurrentPosition,
    XRefOne)

        // CDirectSoundBuffer_GetCurrentPosition+0x39 : call [CMcpxBuffer::GetCurrentPosition]
        XREF_ENTRY( 0x39, XREF_CMcpxBuffer_GetCurrentPosition), 

        // CDirectSoundBuffer_GetCurrentPosition+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 },
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // CDirectSoundBuffer_GetCurrentPosition+0x41 : jz +0x0B
        { 0x41, 0x74 },
        { 0x42, 0x0B },

        // CDirectSoundBuffer_GetCurrentPosition+0x52 : retn 0x08
        { 0x52, 0xC2 },
        { 0x53, 0x0C },
OOVPA_END;

#if 0 // Replaced with better OOVPA 4134
// ******************************************************************
// * CDirectSoundBuffer::Play
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_Play, 4134, 1+11,

	XREF_CDirectSoundBuffer_Play,
	XRefOne)

    XREF_ENTRY(0x35, XREF_CMcpxBuffer_Play),

	{ 0x05, 0xFF },
	{ 0x0D, 0x0F },
	{ 0x13, 0xF6 },
	{ 0x1B, 0xFF },
	{ 0x26, 0xEB },
	{ 0x2C, 0x8B },
	{ 0x2F, 0x57 },
	{ 0x30, 0xFF },
	{ 0x31, 0x74 },
	{ 0x32, 0x24 },
	{ 0x33, 0x18 },
OOVPA_END;
#endif

// ******************************************************************
// * CDirectSound::SetDistanceFactor
// ******************************************************************
OOVPA_XREF(CDirectSound_SetDistanceFactor, 4134, 17,

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

        // CDirectSound_SetDistanceFactor+0x39 : or dword ptr[eax+0xA4], 0x60
        { 0x39, 0x83 },
        { 0x3A, 0x88 },
        { 0x3B, 0xA4 },
        { 0x3F, 0x60 },

        // CDirectSound_SetDistanceFactor+0x4F : jz +0x0B
        { 0x4F, 0x74 },
        { 0x50, 0x0B },
OOVPA_END;
#if 0 // Replaced with generic OOVPA 3911
// ******************************************************************
// * IDirectSound_SetDistanceFactor
// ******************************************************************
OOVPA_XREF(IDirectSound_SetDistanceFactor, 4134, 1+10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSound_SetDistanceFactor+0x1D : call [CDirectSound::SetDistanceFactor]
        XREF_ENTRY( 0x1D, XREF_CDirectSound_SetDistanceFactor ), 

        // IDirectSound_SetDistanceFactor+0x04 : fld [esp+0x0C]
        { 0x04, 0xD9 },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x0C },

        // IDirectSound_SetDistanceFactor+0x0C : push ecx
        { 0x0C, 0x51 },

        // IDirectSound_SetDistanceFactor+0x12 : add eax, 0xFFFFFFF8
        { 0x12, 0x83 },
        { 0x13, 0xC0 },
        { 0x14, 0xF8 },

        // IDirectSound_SetDistanceFactor+0x21 : retn 0x0C
        { 0x21, 0xC2 },
        { 0x22, 0x0C },
OOVPA_END;
#endif
// ******************************************************************
// * CDirectSound::SetRolloffFactor
// ******************************************************************
OOVPA_XREF(CDirectSound_SetRolloffFactor, 4134, 17,

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

        // CDirectSound_SetRolloffFactor+0x39 : or dword ptr[eax+0xA4], 0x04
        { 0x39, 0x83 },
        { 0x3A, 0x88 },
        { 0x3B, 0xA4 },
        { 0x3F, 0x04 },

        // CDirectSound_SetRolloffFactor+0x4F : jz +0x0B
        { 0x4F, 0x74 },
        { 0x50, 0x0B },
OOVPA_END;

#if 0 // Duplicate OOVPA, using 3911 OOVPA instead
// ******************************************************************
// * IDirectSound_SetRolloffFactor
// ******************************************************************
OOVPA_XREF(IDirectSound_SetRolloffFactor, 4134, 1+10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSound_SetRolloffFactor+0x1D : call [CDirectSound::SetRolloffFactor]
        XREF_ENTRY( 0x1D, XREF_CDirectSound_SetRolloffFactor ), 

        // IDirectSound_SetRolloffFactor+0x04 : fld [esp+0x0C]
        { 0x04, 0xD9 },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x0C },

        // IDirectSound_SetRolloffFactor+0x0C : push ecx
        { 0x0C, 0x51 },

        // IDirectSound_SetRolloffFactor+0x12 : add eax, 0xFFFFFFF8
        { 0x12, 0x83 },
        { 0x13, 0xC0 },
        { 0x14, 0xF8 },

        // IDirectSound_SetRolloffFactor+0x21 : retn 0x0C
        { 0x21, 0xC2 },
        { 0x22, 0x0C },
OOVPA_END;
#endif

// ******************************************************************
// * CDirectSound::SetDopplerFactor
// ******************************************************************
OOVPA_XREF(CDirectSound_SetDopplerFactor, 4134, 14,

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

        // CDirectSound_SetDopplerFactor+0x39 : or dword ptr[eax+0xA4], 0x40
        { 0x39, 0x83 },
        { 0x3A, 0x88 },
        { 0x3B, 0xA4 },
        { 0x3F, 0x40 },

        // CDirectSound_SetDopplerFactor+0x4F : jz +0x0B
        { 0x4F, 0x74 },
        { 0x50, 0x0B },
OOVPA_END;
#if 0 // Replaced with generic OOVPA 3911
// ******************************************************************
// * IDirectSound_SetDopplerFactor
// ******************************************************************
OOVPA_XREF(IDirectSound_SetDopplerFactor, 4134, 1+10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSound_SetDopplerFactor+0x1D : call [CDirectSound::SetDopplerFactor]
        XREF_ENTRY( 0x1D, XREF_CDirectSound_SetDopplerFactor), 

        // IDirectSound_SetDopplerFactor+0x04 : fld [esp+0x0C]
        { 0x04, 0xD9 },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x0C },

        // IDirectSound_SetDopplerFactor+0x0C : push ecx
        { 0x0C, 0x51 },

        // IDirectSound_SetDopplerFactor+0x12 : add eax, 0xFFFFFFF8
        { 0x12, 0x83 },
        { 0x13, 0xC0 },
        { 0x14, 0xF8 },

        // IDirectSound_SetDopplerFactor+0x21 : retn 0x0C
        { 0x21, 0xC2 },
        { 0x22, 0x0C },
OOVPA_END;
#endif
// ******************************************************************
// * CDirectSound::CommitDeferredSettings
// ******************************************************************
OOVPA_XREF(CDirectSound_CommitDeferredSettings, 4134, 11,

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

        // CDirectSound_CommitDeferredSettings+0x5C : and [eax+0xA4], esi
        { 0x5C, 0x21 },
        { 0x5D, 0xB0 },
        { 0x5E, 0xA4 },

        // CDirectSound_CommitDeferredSettings+0x78 : leave
        { 0x78, 0xC9 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetMaxDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMaxDistance, 4134, 12,

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
        { 0x1F, 0x70 },

        // CDirectSoundVoice_SetMaxDistance+0x30 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 }
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetMaxDistance
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(CDirectSoundBuffer_SetMaxDistance, 4134, 1+10,

    XREF_CDirectSoundBuffer_SetMaxDistance,
    XRefOne)

        // CDirectSoundBuffer_SetMaxDistance+0x39 : call [CDirectSoundVoice::SetMaxDistance]
        XREF_ENTRY( 0x3A, XREF_CDirectSoundVoice_SetMaxDistance ),

        { 0x00, 0x56 },

        // CDirectSoundBuffer_SetMaxDistance+0x21 : mov eax, 80004005h
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // CDirectSoundBuffer_SetMaxDistance+0x32 : fstp    [esp+0Ch+var_C]
        { 0x32, 0xD9 },
        { 0x33, 0x1C },
        { 0x34, 0x24 },

        // CDirectSoundBuffer_SetMaxDistance+0x53 : retn 0Ch
        { 0x54, 0x0C },
        { 0x55, 0x00 }
OOVPA_END;

#if 0 // No longer used, replaced by generic 3936 version
// ******************************************************************
// * IDirectSoundBuffer_SetMaxDistance
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMaxDistance, 4134, 1+9,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetMaxDistance+0x1C : call [CDirectSoundBuffer::SetMaxDistance]
        XREF_ENTRY( 0x1D, XREF_CDirectSoundBuffer_SetMaxDistance ),

        // IDirectSoundBuffer_SetMaxDistance+0x04 : fld [esp+4+arg_4]
        { 0x04, 0xD9 },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x0C },

        // IDirectSoundBuffer_SetMaxDistance+0x12 : add eax, 0FFFFFFE4
        { 0x12, 0x83 },
        { 0x13, 0xC0 },
        { 0x14, 0xE4 },

        // IDirectSoundBuffer_SetMaxDistance+0x21 : retn 0Ch
        { 0x22, 0x0C },
        { 0x23, 0x00 }
OOVPA_END;
#endif
// ******************************************************************
// * CDirectSoundStream_SetMaxDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMaxDistance, 4134, 1+10,

    XREF_CDirectSoundStream_SetMaxDistance,
    XRefOne)

        XREF_ENTRY( 0x3E, XREF_CDirectSoundVoice_SetMaxDistance ),

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x3D, 0xE8 },
        { 0x48, 0x68 },
        { 0x53, 0x8B },

        { 0x57, 0xC2 },
        { 0x58, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetMinDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMinDistance, 4134, 12,

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
        { 0x1F, 0x70 },

        // CDirectSoundVoice_SetMinDistance+0x30 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 }
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetMinDistance
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(CDirectSoundBuffer_SetMinDistance, 4134, 1+10,

    XREF_CDirectSoundBuffer_SetMinDistance,
    XRefOne)

        // CDirectSoundBuffer_SetMinDistance+0x39 : call [CDirectSoundVoice::SetMinDistance]
        XREF_ENTRY( 0x3A, XREF_CDirectSoundVoice_SetMinDistance ),

        { 0x00, 0x56 },

        // CDirectSoundBuffer_SetMinDistance+0x21 : mov eax, 80004005h
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // CDirectSoundBuffer_SetMinDistance+0x32 : fstp    [esp+0Ch+var_C]
        { 0x32, 0xD9 },
        { 0x33, 0x1C },
        { 0x34, 0x24 },

        // CDirectSoundBuffer_SetMinDistance+0x53 : retn 0Ch
        { 0x54, 0x0C },
        { 0x55, 0x00 }
OOVPA_END;
#if 0 // No longer used, replaced by generic 3936 version
// ******************************************************************
// * IDirectSoundBuffer_SetMinDistance
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMinDistance, 4134, 1+9,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetMinDistance+0x1C : call [CDirectSoundBuffer::SetMinDistance]
        XREF_ENTRY( 0x1D, XREF_CDirectSoundBuffer_SetMinDistance ),

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
// * CDirectSoundStream_SetMinDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMinDistance, 4134, 1+10,

    XREF_CDirectSoundStream_SetMinDistance,
    XRefOne)

        XREF_ENTRY( 0x3E, XREF_CDirectSoundVoice_SetMinDistance ),

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x3D, 0xE8 },
        { 0x48, 0x68 },
        { 0x53, 0x8B },

        { 0x57, 0xC2 },
        { 0x58, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetRolloffFactor
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetRolloffFactor, 4134, 12,

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
        { 0x1F, 0x70 },

        // CDirectSoundVoice_SetRolloffFactor+0x30 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 }
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetRolloffFactor
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(CDirectSoundBuffer_SetRolloffFactor, 4134, 1+10,

    XREF_CDirectSoundBuffer_SetRolloffFactor,
    XRefOne)

        // CDirectSoundBuffer_SetRolloffFactor+0x39 : call [CDirectSoundVoice::SetRolloffFactor]
        XREF_ENTRY( 0x3A, XREF_CDirectSoundVoice_SetRolloffFactor ),

        { 0x00, 0x56 },

        // CDirectSoundBuffer_SetRolloffFactor+0x21 : mov eax, 80004005h
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // CDirectSoundBuffer_SetRolloffFactor+0x32 : fstp    [esp+0Ch+var_C]
        { 0x32, 0xD9 },
        { 0x33, 0x1C },
        { 0x34, 0x24 },

        // CDirectSoundBuffer_SetRolloffFactor+0x53 : retn 0Ch
        { 0x54, 0x0C },
        { 0x55, 0x00 }
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetRolloffFactor
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(IDirectSoundBuffer_SetRolloffFactor, 4134, 1+10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetRolloffFactor+0x1C : call [CDirectSoundBuffer::SetRolloffFactor]
        XREF_ENTRY( 0x1D, XREF_CDirectSoundBuffer_SetRolloffFactor ),

        // IDirectSoundBuffer_SetRolloffFactor+0x00 : push dword ptr [esp+0Ch]
        { 0x02, 0x24 },

        // IDirectSoundBuffer_SetRolloffFactor+0x04 : fld [esp+4+arg_4]
        { 0x04, 0xD9 },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x0C },

        // IDirectSoundBuffer_SetRolloffFactor+0x12 : add eax, 0FFFFFFE4
        { 0x12, 0x83 },
        { 0x13, 0xC0 },
        { 0x14, 0xE4 },

        // IDirectSoundBuffer_SetRolloffFactor+0x21 : retn 0Ch
        { 0x22, 0x0C },
        { 0x23, 0x00 }
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetRolloffFactor
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(CDirectSoundStream_SetRolloffFactor, 4134, 1+8,

    XREF_CDirectSoundStream_SetRolloffFactor,
    XRefOne)

        XREF_ENTRY( 0x3E, XREF_CDirectSoundVoice_SetRolloffFactor ),

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x3D, 0xE8 },
        { 0x48, 0x68 },
        { 0x53, 0x8B },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetDistanceFactor
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetDistanceFactor, 4134, 12,

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
        { 0x1F, 0x70 },

        // CDirectSoundVoice_SetDistanceFactor+0x30 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetDistanceFactor
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(CDirectSoundBuffer_SetDistanceFactor, 4134, 1+10,

    XREF_CDirectSoundBuffer_SetDistanceFactor,
    XRefOne)

        // CDirectSoundBuffer_SetDistanceFactor+0x39 : call [CDirectSoundVoice::SetDistanceFactor]
        XREF_ENTRY( 0x3A, XREF_CDirectSoundVoice_SetDistanceFactor ),

        { 0x00, 0x56 },

        // CDirectSoundBuffer_SetDistanceFactor+0x21 : mov eax, 80004005h
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // CDirectSoundBuffer_SetDistanceFactor+0x32 : fstp    [esp+0Ch+var_C]
        { 0x32, 0xD9 },
        { 0x33, 0x1C },
        { 0x34, 0x24 },

        // CDirectSoundBuffer_SetDistanceFactor+0x53 : retn 0Ch
        { 0x54, 0x0C },
        { 0x55, 0x00 }
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetDistanceFactor
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(IDirectSoundBuffer_SetDistanceFactor, 4134, 1+9,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetDistanceFactor+0x1C : call [CDirectSoundBuffer::SetDistanceFactor]
        XREF_ENTRY( 0x1D, XREF_CDirectSoundBuffer_SetDistanceFactor ),

        // IDirectSoundBuffer_SetDistanceFactor+0x04 : fld [esp+4+arg_4]
        { 0x04, 0xD9 },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x0C },

        // IDirectSoundBuffer_SetDistanceFactor+0x12 : add eax, 0FFFFFFE4
        { 0x12, 0x83 },
        { 0x13, 0xC0 },
        { 0x14, 0xE4 },

        // IDirectSoundBuffer_SetSetDistanceFactor+0x21 : retn 0Ch
        { 0x22, 0x0C },
        { 0x23, 0x00 }
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetConeAngles
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetConeAngles, 4134, 9,

    XREF_CDirectSoundVoice_SetConeAngles,
    XRefZero)

        // CDirectSoundVoice_SetConeAngles+0x0D : mov edx, [esp+arg_4]
        { 0x0D, 0x8B },
        { 0x0E, 0x54 },
        { 0x0F, 0x24 },
        { 0x10, 0x08 },

        // CDirectSoundVoice_SetConeAngles+0x11 : mov [ecx+1Ch], edx
        { 0x11, 0x89 },
        { 0x12, 0x51 },
        { 0x13, 0x1C },

        // CDirectSoundVoice_SetConeAngles+0x40 : retn 10h
        { 0x41, 0x10 },
        { 0x42, 0x00 }
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetConeAngles
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetConeAngles, 4134, 1+11,

    XREF_CDirectSoundBuffer_SetConeAngles,
    XRefOne)

        // CDirectSoundBuffer_SetConeAngles+0x39 : call [CDirectSoundVoice::SetConeAngles]
        XREF_ENTRY( 0x3A, XREF_CDirectSoundVoice_SetConeAngles ),

        // CDirectSoundBuffer_SetConeAngles+0x00 : push esi
        { 0x00, 0x56 },

        // CDirectSoundBuffer_SetConeAngles+0x21 : mov eax, 80004005h
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // CDirectSoundBuffer_SetConeAngles+0x31 : push [esp+0Ch+arg_4]
        { 0x31, 0xFF },
        { 0x32, 0x74 },
        { 0x33, 0x24 },
        { 0x34, 0x18 },

        // CDirectSoundBuffer_SetConeAngles+0x53 : retn 10h
        { 0x54, 0x10 },
        { 0x55, 0x00 }
OOVPA_END;
#if 0 // Moved to 4039
// ******************************************************************
// * IDirectSoundBuffer_SetConeAngles
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetConeAngles, 4134, 1+9,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetConeAngles+0x1C : call [CDirectSoundBuffer::SetConeAngles]
        XREF_ENTRY( 0x1D, XREF_CDirectSoundBuffer_SetConeAngles ),

        // IDirectSoundBuffer_SetConeAngles+0x04 : mov eax, [esp+4+arg_0]
        { 0x04, 0x8B },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },

        // IDirectSoundBuffer_SetConeAngles+0x12 : add eax, 0FFFFFFE4
        { 0x12, 0x83 },
        { 0x13, 0xC0 },
        { 0x14, 0xE4 },

        // IDirectSoundBuffer_SetConeAngles+0x21 : retn 10h
        { 0x22, 0x10 },
        { 0x23, 0x00 }
OOVPA_END;
#endif
// ******************************************************************
// * CDirectSoundVoice::SetConeOrientation
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetConeOrientation, 4134, 8,

    XREF_CDirectSoundVoice_SetConeOrientation,
    XRefZero)

        // CDirectSoundVoice_SetConeOrientation+0x0F : mov edx, [ebp+arg_4]
        { 0x0F, 0x8B },
        { 0x10, 0x55 },
        { 0x11, 0x0C },

        // CDirectSoundVoice_SetConeOrientation+0x12 : mov [ecx+24h], edx
        { 0x12, 0x89 },
        { 0x13, 0x51 },
        { 0x14, 0x24 },

        // CDirectSoundVoice_SetConeOrientation+0x4F : retn 14h
        { 0x50, 0x14 },
        { 0x51, 0x00 }
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetConeOrientation
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetConeOrientation, 4134, 1+8,

    XREF_CDirectSoundBuffer_SetConeOrientation,
    XRefOne)

        // CDirectSoundBuffer_SetConeOrientation+0x49 : call [CDirectSoundVoice::SetConeOrientation]
        XREF_ENTRY( 0x4A, XREF_CDirectSoundVoice_SetConeOrientation ),

        // CDirectSoundBuffer_SetConeOrientation+0x2B : fld [ebp+arg_C]
        { 0x2B, 0xD9 },
        { 0x2C, 0x45 },
        { 0x2D, 0x14 },

        // CDirectSoundBuffer_SetConeOrientation+0x43 : fstp [esp+18h+var_18]
        { 0x43, 0xD9 },
        { 0x44, 0x1C },
        { 0x45, 0x24 },

        // CDirectSoundBuffer_SetConeOrientation+0x64 : retn 14h
        { 0x65, 0x14 },
        { 0x66, 0x00 }
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetConeOrientation
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetConeOrientation, 4134, 1+8,

    XREF_CDirectSoundStream_SetConeOrientation,
    XRefOne)

        XREF_ENTRY( 0x4E, XREF_CDirectSoundVoice_SetConeOrientation ),

        { 0x00, 0x55 },
        { 0x0F, 0x00 },
        { 0x17, 0x74 },
        { 0x24, 0xB8 },
        { 0x2A, 0x3B },
        { 0x39, 0xEC },
        { 0x44, 0x24 },
        { 0x4D, 0xE8 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetConeOutsideVolume
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetConeOutsideVolume, 4134, 12,

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
        { 0x1F, 0x70 },

        // CDirectSoundVoice_SetConeOutsideVolume+0x30 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 }
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetConeOutsideVolume
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetConeOutsideVolume, 4134, 1+10,

    XREF_CDirectSoundBuffer_SetConeOutsideVolume,
    XRefOne)

        // CDirectSoundBuffer_SetConeOutsideVolume+0x35 : call [CDirectSoundVoice::SetConeOutsideVolume]
        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_SetConeOutsideVolume ),

        // CDirectSoundBuffer_SetConeOutsideVolume+0x21 : mov eax, 80004005h
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // CDirectSoundBuffer_SetConeOutsideVolume+0x31 : push [esp+0Ch+arg_0]
        { 0x31, 0xFF },
        { 0x32, 0x74 },
        { 0x33, 0x24 },
        { 0x34, 0x14 },

        // CDirectSoundBuffer_SetConeOutsideVolume+0x4F : retn 0Ch
        { 0x50, 0x0C },
        { 0x51, 0x00 }
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetConeOutsideVolume
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetConeOutsideVolume, 4134, 1+8,

    XREF_CDirectSoundStream_SetConeOutsideVolume,
    XRefOne)

        XREF_ENTRY( 0x3A, XREF_CDirectSoundVoice_SetConeOutsideVolume ),

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x21, 0xB8 },
        { 0x24, 0x00 },
        { 0x39, 0xE8 },
        { 0x3E, 0x85 },
        { 0x4F, 0x8B },
        { 0x53, 0xC2 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetPosition
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetPosition, 4134, 8,

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

        // CDirectSoundVoice_SetPosition+0x4F : retn 14h
        { 0x4F, 0xC2 },
        { 0x50, 0x14 }
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetPosition
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(CDirectSoundBuffer_SetPosition, 4134, 1+9,

    XREF_CDirectSoundBuffer_SetPosition,
    XRefOne)

        // CDirectSoundBuffer_SetPosition+0x49 : call [CDirectSoundVoice::SetPosition]
        XREF_ENTRY( 0x4A, XREF_CDirectSoundVoice_SetPosition ),

        { 0x00, 0x55 },

        // CDirectSoundBuffer_SetPosition+0x2B : fld [ebp+arg_C]
        { 0x2B, 0xD9 },
        { 0x2C, 0x45 },
        { 0x2D, 0x14 },

        // CDirectSoundBuffer_SetPosition+0x43 : fstp [esp+18h+var_18]
        { 0x43, 0xD9 },
        { 0x44, 0x1C },
        { 0x45, 0x24 },

        // CDirectSoundBuffer_SetPosition+0x64 : retn 14h
        { 0x65, 0x14 },
        { 0x66, 0x00 }
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetPosition
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetPosition, 4134, 1+10,

    XREF_CDirectSoundStream_SetPosition,
    XRefOne)

        XREF_ENTRY( 0x4E, XREF_CDirectSoundVoice_SetPosition ),

        { 0x00, 0x55 },
        { 0x0F, 0x00 },
        { 0x17, 0x74 },
        { 0x24, 0xB8 },
        { 0x2A, 0x3B },
        { 0x39, 0xEC },
        { 0x58, 0x68 },
        { 0x63, 0x8B },

        { 0x68, 0xC2 },
        { 0x69, 0x14 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetVelocity
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetVelocity, 4134, 8,

    XREF_CDirectSoundVoice_SetVelocity,
    XRefZero)

        // CDirectSoundVoice_SetVelocity+0x0F : mov edx, [ebp+arg_4]
        { 0x0F, 0x8B },
        { 0x10, 0x55 },
        { 0x11, 0x0C },

        // CDirectSoundVoice_SetVelocity+0x12 : mov [ecx+10h], edx
        { 0x12, 0x89 },
        { 0x13, 0x51 },
        { 0x14, 0x10 },

        // CDirectSoundVoice_SetVelocity+0x4F : retn 14h
        { 0x50, 0x14 },
        { 0x51, 0x00 }
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetVelocity
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(CDirectSoundBuffer_SetVelocity, 4134, 1+9,

    XREF_CDirectSoundBuffer_SetVelocity,
    XRefOne)

        // CDirectSoundBuffer_SetVelocity+0x49 : call [CDirectSoundVoice::SetVelocity]
        XREF_ENTRY( 0x4A, XREF_CDirectSoundVoice_SetVelocity ),

        { 0x00, 0x55 },

        // CDirectSoundBuffer_SetVelocity+0x2B : fld [ebp+arg_C]
        { 0x2B, 0xD9 },
        { 0x2C, 0x45 },
        { 0x2D, 0x14 },

        // CDirectSoundBuffer_SetVelocity+0x43 : fstp [esp+18h+var_18]
        { 0x43, 0xD9 },
        { 0x44, 0x1C },
        { 0x45, 0x24 },

        // CDirectSoundBuffer_SetVelocity+0x64 : retn 14h
        { 0x65, 0x14 },
        { 0x66, 0x00 }
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetVelocity
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetVelocity, 4134, 1+10,

    XREF_CDirectSoundStream_SetVelocity,
    XRefOne)

        XREF_ENTRY( 0x4E, XREF_CDirectSoundVoice_SetVelocity ),

        { 0x00, 0x55 },
        { 0x0F, 0x00 },
        { 0x17, 0x74 },
        { 0x24, 0xB8 },
        { 0x2A, 0x3B },
        { 0x35, 0x83 },
        { 0x40, 0x45 },
        { 0x63, 0x8B },

        { 0x68, 0xC2 },
        { 0x69, 0x14 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetDopplerFactor
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetDopplerFactor, 4134, 12,

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
        { 0x1F, 0x70 },

        // CDirectSoundVoice_SetDopplerFactor+0x31 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 }
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetDopplerFactor
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(CDirectSoundBuffer_SetDopplerFactor, 4134, 1+9,

    XREF_CDirectSoundBuffer_SetDopplerFactor,
    XRefOne)

        // CDirectSoundBuffer_SetDopplerFactor+0x39 : call [CDirectSoundVoice::SetDopplerFactor]
        XREF_ENTRY( 0x3A, XREF_CDirectSoundVoice_SetDopplerFactor),

        // CDirectSoundBuffer_SetDopplerFactor+0x2D : push    [esp+4+arg_8]
        { 0x2D, 0xFF },
        { 0x2E, 0x74 },
        { 0x2F, 0x24 },
        { 0x30, 0x14 },

        // CDirectSoundBuffer_SetDopplerFactor+0x32 : fstp [esp+0Ch+var_C]
        { 0x32, 0xD9 },
        { 0x33, 0x1C },
        { 0x34, 0x24 },

        // CDirectSoundBuffer_SetDopplerFactor+0x53 : retn 0Ch
        { 0x54, 0x0C },
        { 0x55, 0x00 }
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetDopplerFactor
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(IDirectSoundBuffer_SetDopplerFactor, 4134, 1+8,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetDopplerFactor+0x1C : call [CDirectSoundBuffer::SetDopplerFactor]
        XREF_ENTRY( 0x1D, XREF_CDirectSoundBuffer_SetDopplerFactor ),

        // IDirectSoundBuffer_SetDopplerFactor+0x08 : mov eax, [esp+4+arg_0]
        { 0x08, 0x8B },
        { 0x09, 0x44 },
        { 0x0A, 0x24 },
        { 0x0B, 0x08 },

        // IDirectSoundBuffer_SetDopplerFactor+0x19 : and ecx, eax
        { 0x19, 0x23 },
        { 0x1A, 0xC8 },

        // IDirectSoundBuffer_SetDopplerFactor+0x21 : retn 0Ch
        { 0x22, 0x0C },
        { 0x23, 0x00 }
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetI3DL2Source
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetI3DL2Source, 4134, 10,

    XREF_CDirectSoundVoice_SetI3DL2Source,
    XRefZero)

        // CDirectSoundVoice_SetI3DL2Source+0x12 : mov esi, [ecx]
        { 0x12, 0x8B },
        { 0x13, 0x31 },

        // CDirectSoundVoice_SetI3DL2Source+0x14 : mov [edx+4Ch], esi
        { 0x14, 0x89 },
        { 0x15, 0x72 },
        { 0x16, 0x4C },

        // CDirectSoundVoice_SetI3DL2Source+0x44 : fld dword ptr [ecx+10h] (not advise to use)
        { 0x44, 0xD9 },
        { 0x45, 0x41 },
        { 0x46, 0x10 },

        // CDirectSoundVoice_SetI3DL2Source+0xAC : retn 0Ch
        { 0xAD, 0x0C },
        { 0xAE, 0x00 }
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetI3DL2Source
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(CDirectSoundBuffer_SetI3DL2Source, 4134, 1+11,

    XREF_CDirectSoundBuffer_SetI3DL2Source,
    XRefOne)

        // CDirectSoundBuffer_SetI3DL2Source+0x35 : call [CDirectSoundVoice::SetI3DL2Source]
        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_SetI3DL2Source ),

        // CDirectSoundBuffer_SetI3DL2Source+0x00 : push esi
        { 0x00, 0x56 },

        // CDirectSoundBuffer_SetI3DL2Source+0x21 : mov eax, 80004005h
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // CDirectSoundBuffer_SetI3DL2Source+0x31 : push [esp+0Ch+arg_0]
        { 0x31, 0xFF },
        { 0x32, 0x74 },
        { 0x33, 0x24 },
        { 0x34, 0x14 },

        // CDirectSoundBuffer_SetI3DL2Source+0x4F : retn 0Ch
        { 0x50, 0x0C },
        { 0x51, 0x00 }
OOVPA_END;

#if 0 // Moved to 4039
// ******************************************************************
// * IDirectSoundBuffer_SetI3DL2Source
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetI3DL2Source, 4134, 1+9,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetI3DL2Source+0x18 : call [CDirectSoundBuffer::SetI3DL2Source]
        XREF_ENTRY( 0x19, XREF_CDirectSoundBuffer_SetI3DL2Source ),

        // IDirectSoundBuffer_SetI3DL2Source+0x04 : push [esp+arg_8]
        { 0x04, 0xFF },
        { 0x05, 0x74 },
        { 0x06, 0x24 },
        { 0x07, 0x0C },

        // IDirectSoundBuffer_SetI3DL2Source+0x0E : add eax, 0FFFFFFE4h
        { 0x0E, 0x83 },
        { 0x0F, 0xC0 },
        { 0x10, 0xE4 },

        // IDirectSoundBuffer_SetI3DL2Source+0x1D : retn 0Ch
        { 0x1E, 0x0C },
        { 0x1F, 0x00 }
OOVPA_END;
#endif

// ******************************************************************
// * CDirectSoundStream_SetI3DL2Source
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetI3DL2Source, 4134, 1+8,

    XREF_CDirectSoundStream_SetI3DL2Source,
    XRefOne)

        XREF_ENTRY( 0x3A, XREF_CDirectSoundVoice_SetI3DL2Source ),

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x39, 0xE8 },
        { 0x44, 0x68 },
        { 0x4F, 0x8B },
OOVPA_END;

#if 0 // Moved to 4039
// ******************************************************************
// * IDirectSoundBuffer_Stop
// ******************************************************************
OOVPA_NO_XREF(IDirectSoundBuffer_Stop, 4134, 11)

        // IDirectSoundBuffer_Stop+0x00 : mov eax, [esp+arg_0]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // IDirectSoundBuffer_Stop+0x04 : mov ecx, eax
        { 0x04, 0x8B },
        { 0x05, 0xC8 },

        // IDirectSoundBuffer_Stop+0x06 : add eax, 0FFFFFFE4h
        { 0x06, 0x83 },
        { 0x07, 0xC0 },
        { 0x08, 0xE4 },

        // IDirectSoundBuffer_Stop+0x15 : retn 4
        { 0x16, 0x04 },
        { 0x17, 0x00 }
OOVPA_END;
#endif

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetAllParameters
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetAllParameters, 4134, 8,

    XREF_CDirectSoundVoice_SetAllParameters,
    XRefZero)

        { 0x1E, 0x92 },
        { 0x3E, 0x00 },
        { 0x5E, 0x00 },
        { 0x7E, 0x89 },
        { 0x9E, 0x28 },
        { 0xBE, 0x41 },
        { 0xDE, 0x8B },
        { 0xFE, 0x10 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetAllParameters
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetAllParameters, 4134, 1+7,

    XREF_CDirectSoundBuffer_SetAllParameters,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_SetAllParameters ),

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x74 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
OOVPA_END;
#if 0 // Moved to 4039
// ******************************************************************
// * IDirectSoundBuffer_SetAllParameters
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetAllParameters, 4134, 1+8,

    XRefNoSaveIndex,
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
#endif
// ******************************************************************
// * CDirectSoundStream_SetAllParameters
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetAllParameters, 4134, 1+8,

    XREF_CDirectSoundStream_SetAllParameters,
    XRefOne)

        XREF_ENTRY( 0x3A, XREF_CDirectSoundVoice_SetAllParameters ),

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x39, 0xE8 },
        { 0x44, 0x68 },
        { 0x4F, 0x8B },
OOVPA_END;
#if 0 //Replaced by generic 3911
// ******************************************************************
// * IDirectSoundBuffer_GetStatus
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_GetStatus, 4134, 1+8,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_GetStatus+0x15 : call [CDirectSoundBuffer::GetStatus]
        XREF_ENTRY( 0x15, XREF_CDirectSoundBuffer_GetStatus), 

        // IDirectSoundBuffer_GetStatus+0x04 : push [esp+0x08]
        { 0x04, 0xFF },
        { 0x05, 0x74 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },

        // IDirectSoundBuffer_GetStatus+0x0F : sbb ecx, ecx
        { 0x0F, 0x1B },
        { 0x10, 0xC9 },

        // IDirectSoundBuffer_GetStatus+0x11 : and ecx, eax
        { 0x11, 0x23 },
        { 0x12, 0xC8 },
OOVPA_END;
#endif
// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetMode
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMode, 4134, 14,

    XREF_CDirectSoundVoice_SetMode,
    XRefZero)

        { 0x00, 0xF6 },

        { 0x03, 0x0C },
        { 0x07, 0x24 },
        { 0x08, 0x04 },
        { 0x0B, 0x10 },
        { 0x0D, 0x89 },
        { 0x0E, 0xB4 },
        { 0x12, 0x8B },
        { 0x17, 0x51 },
        { 0x18, 0x3C },
        { 0x1C, 0xE8 },
        { 0x21, 0x33 },

        { 0x23, 0xC2 },
        { 0x24, 0x0C },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetMode
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(CDirectSoundBuffer_SetMode, 4134, 1+8,

    XREF_CDirectSoundBuffer_SetMode,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_SetMode ),

        { 0x00, 0x56 },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x74 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
OOVPA_END;
#if 0 // Duplicate OOVPA, replacing with generic 4039
// ******************************************************************
// * IDirectSoundBuffer_SetMode
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMode, 4134, 1+8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_CDirectSoundBuffer_SetMode ),

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
// * CDirectSoundStream_SetMode
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMode, 4134, 1+10,

    XREF_CDirectSoundStream_SetMode,
    XRefOne)

        XREF_ENTRY( 0x3A, XREF_CDirectSoundVoice_SetMode ),

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x39, 0xE8 },
        { 0x44, 0x68 },
        { 0x4F, 0x8B },

        { 0x53, 0xC2 },
        { 0x54, 0x0C },
OOVPA_END;

// ******************************************************************
// * DirectSound::CMcpxVoiceClient::SetFilter
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(CMcpxVoiceClient_SetFilter, 4134, 12,

    XREF_CMcpxVoiceClient_SetFilter,
    XRefZero)

        { 0x00, 0x55 },

        { 0x23, 0x6A },
        { 0x24, 0x06 },

        { 0x47, 0x83 },
        { 0x48, 0xE0 },
        { 0x49, 0x03 },

        { 0x50, 0xC1 },
        { 0x51, 0xE8 },
        { 0x52, 0x12 },

        { 0x53, 0x83 },
        { 0x54, 0xE0 },
        { 0x55, 0x07 },

        //NOTE: Before offset 0x65, there has been no changes throughout all revisions.
OOVPA_END;

#if 0 // Replaced with generic OOVPA 4134, see above
// ******************************************************************
// * DirectSound::CMcpxVoiceClient::SetFilter
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_SetFilter, 4134, 8,

    XREF_CMcpxVoiceClient_SetFilter,
    XRefZero)

        { 0x1E, 0xFF },
        { 0x3E, 0x73 },
        { 0x5E, 0x3E },
        { 0x7E, 0x0B },
        { 0x9E, 0xF6 },
        { 0xBE, 0x89 },
        { 0xDE, 0x0F },
        { 0xFE, 0xB7 },
OOVPA_END;
#endif

#if 0 // Moved to 4039
// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetFilter
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetFilter, 4134, 1+6,

    XREF_CDirectSoundVoice_SetFilter,
    XRefOne)

        XREF_ENTRY( 0x0C, XREF_CMcpxVoiceClient_SetFilter ),

        { 0x01, 0x44 },
        { 0x04, 0xFF },
        { 0x07, 0x08 },
        { 0x0A, 0x0C },
        { 0x10, 0xC2 },
        { 0x11, 0x08 },
OOVPA_END;
#endif

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetFilter
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetFilter, 4134, 1+7,

    XREF_CDirectSoundBuffer_SetFilter,
    XRefOne)

        XREF_ENTRY( 0x32, XREF_CDirectSoundVoice_SetFilter ),

        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0x10 },
        { 0x3A, 0x74 },
        { 0x47, 0x8B },
OOVPA_END;

#if 0 // Moved to 4039
// ******************************************************************
// * IDirectSoundBuffer_SetFilter
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetFilter, 4134, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_CDirectSoundBuffer_SetFilter ),

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
// * CDirectSound::SetVelocity
// ******************************************************************
OOVPA_XREF(CDirectSound_SetVelocity, 4134, 16,

    XREF_CDirectSound_SetVelocity,
    XRefZero)

        // CDirectSound_SetVelocity+0x00 : push ebp
        { 0x00, 0x55},

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

        // CDirectSound_SetVelocity+0x4C : or dword ptr [eax+0x0A4], 0x40
        { 0x4C, 0x83 },
        { 0x4D, 0x88 },
        { 0x4E, 0xA4 },
        { 0x4F, 0x00 },
        { 0x52, 0x40 },

        // CDirectSound_SetVelocity+0x73 : retn 0x14
        { 0x73, 0xC2 },
        { 0x74, 0x14 },
OOVPA_END;

#if 0 // Moved to 4039
// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetPitch
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetPitch, 4134, 6,

    XREF_DSBUFFERSETPITCHB,
    XRefZero)

        { 0x02, 0x24 },
        { 0x06, 0x10 },
        { 0x0A, 0x08 },
        { 0x0E, 0x8B },
        { 0x16, 0xC2 },
        { 0x17, 0x08 },
OOVPA_END;
#endif

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetPitch
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetPitch, 4134, 1+7,

    XREF_CDirectSoundBuffer_SetPitch,
    XRefOne)

        XREF_ENTRY( 0x32, XREF_CDirectSoundVoice_SetPitch ),

        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0x10 },
        { 0x3A, 0x74 },
        { 0x47, 0x8B },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetPitch
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetPitch, 4134, 1+11,

    XREF_CDirectSoundStream_SetPitch,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_SetPitch ),

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

// ******************************************************************
// * CDirectSoundBuffer::SetVolume
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetVolume, 4134, 1+11,

    XREF_CDirectSoundBuffer_SetVolume,
    XRefOne)

        // CDirectSoundBuffer_SetVolume+0x32 : call [CDirectSoundVoice::SetVolume]
        XREF_ENTRY( 0x32, XREF_CDirectSoundVoice_SetVolume ),

        // CDirectSoundBuffer_SetVolume+0x0D : movzx esi, al
        { 0x0D, 0x0F },
        { 0x0E, 0xB6 },
        { 0x0F, 0xF0 },

        // CDirectSoundBuffer_SetVolume+0x26 : jmp +0x22
        { 0x26, 0xEB },
        { 0x27, 0x22 },

        // CDirectSoundBuffer_SetVolume+0x2D : push [esp+0x10]
        { 0x2D, 0xFF },
        { 0x2E, 0x74 },
        { 0x2F, 0x24 },
        { 0x30, 0x10 },

        // CDirectSoundBuffer_SetVolume+0x4B : retn 0x08
        { 0x4B, 0xC2 },
        { 0x4C, 0x08 },
OOVPA_END;

#if 0 // Moved to 4039
// ******************************************************************
// * IDirectSoundBuffer8::SetVolume
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetVolume, 4134, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_CDirectSoundBuffer_SetVolume ),

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
// * CDirectSound::CreateSoundStream
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(CDirectSound_CreateSoundStream, 4134, 14,

    XREF_CDirectSound_CreateSoundStream,
    XRefZero)

        // CDirectSound_CreateSoundStream+0x23 : mov eax, 0x80004005
        { 0x23, 0xB8 },
        { 0x24, 0x05 },
        { 0x25, 0x40 },
        { 0x27, 0x80 },

        // CDirectSound_CreateSoundStream+0x2A : push 0x28
        { 0x2A, 0x6A },
        { 0x2B, 0x28 },

        // CDirectSound_CreateSoundStream+0x4A : add esi, 0x7FF8FFF2
        { 0x4A, 0x81 },
        { 0x4B, 0xE6 },
        { 0x4C, 0xF2 },
        { 0x4D, 0xFF },
        { 0x4E, 0xF8 },
        { 0x4F, 0x7F },

        // CDirectSound_CreateSoundStream+0x8E : retn 0x10
        { 0x8E, 0xC2 },
        { 0x8F, 0x10 },
OOVPA_END;

// ******************************************************************
// * DirectSoundCreateStream
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(DirectSoundCreateStream, 4134, 1+11,

    XRefNoSaveIndex,
    XRefOne)

        // DirectSoundCreateStream+0x2F : call [CDirectSound::CreateSoundStream]
        XREF_ENTRY( 0x2F, XREF_CDirectSound_CreateSoundStream ),

        // DirectSoundCreateStream+0x04 : and [ebp-0x04], 0
        { 0x04, 0x83 },
        { 0x05, 0x65 },
        { 0x06, 0xFC },

        // DirectSoundCreateStream+0x08 : push ebx; push esi; push edi
        { 0x08, 0x53 },
        { 0x09, 0x56 },
        { 0x0A, 0x57 },

        // DirectSoundCreateStream+0x3C : call dword ptr [eax+8]
        { 0x3C, 0xFF },
        { 0x3D, 0x50 },
        { 0x3E, 0x08 },

        // DirectSoundCreateStream+0x54 : retn 0x08
        { 0x54, 0xC2 },
        { 0x55, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoiceSettings::SetMixBinVolumes
// ******************************************************************
// Generic OOVPA as of 4134 and newer.
OOVPA_XREF(CDirectSoundVoiceSettings_SetMixBinVolumes, 4134, 10,

    XREF_CDirectSoundVoiceSettings_SetMixBinVolumes,
    XRefZero)

        // CDirectSoundVoiceSettings_SetMixBinVolumes+0x09 : jbe +0x16
        { 0x09, 0x76 },
        { 0x0A, 0x16 },

        // CDirectSoundVoiceSettings_SetMixBinVolumes+0x18 : mov [ecx+edi*4+0x30], eax
        { 0x18, 0x89 },
        { 0x19, 0x44 },
        { 0x1A, 0xB9 },
        { 0x1B, 0x30 },

        // CDirectSoundVoiceSettings_SetMixBinVolumes+0x1E : jb +0xEC
        { 0x1E, 0x72 },
        { 0x1F, 0xEC },

        // CDirectSoundVoiceSettings_SetMixBinVolumes+0x22 : retn 0x04
        { 0x22, 0xC2 },
        { 0x23, 0x04 },
OOVPA_END;

#if 0 // Moved to 4039
// ******************************************************************
// * CDirectSoundVoice::SetMixBinVolumes
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMixBinVolumes, 4134, 1+16,

    XREF_CDirectSoundVoice_SetMixBinVolumes,
    XRefOne)

        // CDirectSoundVoice_SetMixBinVolumes+0x0D : call [CDirectSoundVoiceSettings::SetMixBinVolumes]
        XREF_ENTRY( 0x0D, XREF_CDirectSoundVoiceSettings_SetMixBinVolumes ),

        // CDirectSoundVoice_SetMixBinVolumes+0x01 : mov esi, [esp+8]
        { 0x01, 0x8B },
        { 0x02, 0x74 },
        { 0x03, 0x24 },
        { 0x04, 0x08 },

        // CDirectSoundVoice_SetMixBinVolumes+0x05 : push [esp+0x0C]
        { 0x05, 0xFF },
        { 0x06, 0x74 },
        { 0x07, 0x24 },
        { 0x08, 0x0C },

        // CDirectSoundVoice_SetMixBinVolumes+0x09 : mov ecx, [esi+0x10]
        { 0x09, 0x8B },
        { 0x0A, 0x4E },
        { 0x0B, 0x10 },

        // CDirectSoundVoice_SetMixBinVolumes+0x11 : mov ecx, [esi+0x0C]
        { 0x11, 0x8B },
        { 0x12, 0x4E },
        { 0x13, 0x0C },

        // CDirectSoundVoice_SetMixBinVolumes+0x1A : retn 0x08
        { 0x1A, 0xC2 },
        { 0x1B, 0x08 },
OOVPA_END;
#endif

// ******************************************************************
// * CDirectSoundBuffer::SetMixBinVolumes
// ******************************************************************
//Generic OOVPA as of 4134 and newer
OOVPA_XREF(CDirectSoundBuffer_SetMixBinVolumes, 4134, 1+16,

    XREF_CDirectSoundBuffer_SetMixBinVolumes,
    XRefOne)

        // CDirectSoundBuffer_SetMixBinVolumes+0x32 : call [CDirectSoundVoice::SetMixBinVolumes]
        XREF_ENTRY( 0x32, XREF_CDirectSoundVoice_SetMixBinVolumes ),

        // CDirectSoundBuffer_SetMixBinVolumes+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 },
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // CDirectSoundBuffer_SetMixBinVolumes+0x29 : push [esp+0x10]; push [esp+0x10]
        { 0x29, 0xFF },
        { 0x2A, 0x74 },
        { 0x2B, 0x24 },
        { 0x2C, 0x10 },
        { 0x2D, 0xFF },
        { 0x2E, 0x74 },
        { 0x2F, 0x24 },
        { 0x30, 0x10 },

        // CDirectSoundBuffer_SetMixBinVolumes+0x49 : pop edi
        { 0x49, 0x5F },

        // CDirectSoundBuffer_SetMixBinVolumes+0x4B : retn 0x08
        { 0x4B, 0xC2 },
        { 0x4C, 0x08 },
OOVPA_END;
#if 0 // Moved to 4039
// ******************************************************************
// * IDirectSoundBuffer_SetMixBinVolumes_8
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMixBinVolumes_8, 4134, 1+11,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetMixBinVolumes_8+0x15 : call [CDirectSoundBuffer::SetMixBinVolumes]
        XREF_ENTRY( 0x15, XREF_CDirectSoundBuffer_SetMixBinVolumes ),

        // IDirectSoundBuffer_SetMixBinVolumes_8+0x04 : push [esp+0x08]
        { 0x04, 0xFF },
        { 0x05, 0x74 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },

        // IDirectSoundBuffer_SetMixBinVolumes_8+0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 },
        { 0x0B, 0xC0 },
        { 0x0C, 0xE4 },

        // IDirectSoundBuffer_SetMixBinVolumes_8+0x0F : sbb ecx, ecx
        { 0x0F, 0x1B },
        { 0x10, 0xC9 },

        // IDirectSoundBuffer_SetMixBinVolumes_8+0x11 : and ecx, eax
        { 0x11, 0x23 },
        { 0x12, 0xC8 },
OOVPA_END;
#endif
#if 0 // Moved to 4039
// ******************************************************************
// * CDirectSoundStream_SetMixBinVolumes_8
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMixBinVolumes_8, 4134, 1+11,

    XREF_CDirectSoundStream_SetMixBinVolumes_8,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_SetMixBinVolumes ),

        { 0x0D, 0x0F },
        { 0x0E, 0xB6 },
        { 0x0F, 0xF0 },
        { 0x21, 0xB8 },
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },
        { 0x4F, 0xC2 },
        { 0x50, 0x08 },
        { 0x51, 0x00 },
OOVPA_END;
#endif
// ******************************************************************
// * CDirectSound_SetEffectData
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(CDirectSound_SetEffectData, 4134, 14, // Also for 5849

    XREF_CDirectSound_SetEffectData,
    XRefZero)

        { 0x00, 0x55 },
        { 0x0F, 0x00 },

        { 0x24, 0xB8 },
        { 0x25, 0x05 },
        { 0x26, 0x40 },
        { 0x27, 0x00 },
        { 0x28, 0x80 },

        { 0x29, 0xEB },
        { 0x2A, 0x32 },

        { 0x2B, 0x8B },
        { 0x2C, 0x45 },

        { 0x44, 0xE8 },

        { 0x5F, 0xC2 },
        { 0x60, 0x18 },
OOVPA_END;
#if 0 // Replaced with generic OOVPA 3911
// ******************************************************************
// * IDirectSound_SetEffectData
// ******************************************************************
OOVPA_XREF(IDirectSound_SetEffectData, 4134, 1+12,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x22, XREF_CDirectSound_SetEffectData ),

        { 0x04, 0x75 },
        { 0x0F, 0x75 },
        { 0x18, 0xD9 },
        { 0x19, 0xFF },
        { 0x1A, 0x75 },
        { 0x1B, 0x0C },
        { 0x1C, 0x1B },
        { 0x1D, 0xC9 },
        { 0x1E, 0x23 },
        { 0x1F, 0xC8 },
        { 0x26, 0x5D },
        { 0x28, 0x18 },
OOVPA_END;
#endif
// ******************************************************************
// * CMcpxBuffer_Stop
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_Stop, 4134, 9,

	XREF_CMcpxBuffer_Stop,
    XRefZero)

        // CMcpxBuffer_Stop+0x0B : cmp al, 3
        { 0x0B, 0x3C },
        { 0x0C, 0x03 },

        // CMcpxBuffer_Stop+0x1A : jne +0x59
        { 0x1A, 0x75 },
        { 0x1B, 0x59 },

        // CMcpxBuffer_Stop+0x1C : test [esp+0Ch], 2
        { 0x1C, 0xF6 },
        { 0x1D, 0x44 },
        { 0x1E, 0x24 },
        { 0x1F, 0x0C },
        { 0x20, 0x02 },
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer::Stop2
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_Stop2, 4134, 1+8,

    XREF_CMcpxBuffer_Stop2,
    XRefOne)

        // CMcpxBuffer_Stop2+0x2B : call [CMcpxBuffer::Stop]
        XREF_ENTRY( 0x2B, XREF_CMcpxBuffer_Stop ),

        // CMcpxBuffer_Stop2+0x00 : push ebp
        { 0x00, 0x55 },

        // CMcpxBuffer_Stop2+0x11 : push [ebp+arg_8]
        { 0x11, 0xFF },
        { 0x12, 0x75 },
        { 0x13, 0x10 },

        // CMcpxBuffer_Stop2+0x23 : jnz +0x0C
        { 0x23, 0x75 },
        { 0x24, 0x0C },

        // CMcpxBuffer_Stop2+0x36 : push esi
        { 0x36, 0xC2 },
        { 0x37, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_Stop
// ******************************************************************
//Generic OOVPA as of 4134 and newer.
OOVPA_XREF(CDirectSoundBuffer_Stop, 4134, 1+12,

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

        // CDirectSoundBuffer_Stop+0x2C : mov ecx,[eax+20]
        { 0x2C, 0x8B },
        { 0x2D, 0x48 },
        { 0x2E, 0x20 },

        // CDirectSoundBuffer_Stop+0x3B : jz +0x0B
        { 0x3B, 0x74 },
        { 0x3C, 0x0B },

        // CDirectSoundBuffer_Stop+0x4C : retn 0x04
        { 0x4C, 0xC2 },
        { 0x4D, 0x04 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream::SetEG
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetEG, 4134, 1+8,

    XREF_CDirectSoundStream_SetEG,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_SetEG ),

        { 0x00, 0x56 }, //Prevent incorrect offset difference from 4039 revision.

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream::SetFilter
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetFilter, 4134, 1+8,

    XREF_CDirectSoundStream_SetFilter,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_SetFilter ),

        { 0x00, 0x56 }, //Prevent incorrect offset difference from 4039 revision.

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream::SetLFO
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetLFO, 4134, 1+10,

    XREF_CDirectSoundStream_SetLFO,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_SetLFO ),

        { 0x00, 0x56 }, //Prevent incorrect offset difference from 4039 revision.

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

// ******************************************************************
// * CDirectSound_SetOrientation
// ******************************************************************
OOVPA_XREF(CDirectSound_SetOrientation, 4134, 13,

    XREF_CDirectSound_SetOrientation,
    XRefZero)

        // CDirectSound_SetOrientation+0x00 : push ebp
        { 0x00 , 0x55 },

        { 0x31 , 0x8B },
        { 0x33 , 0x0C },
        { 0x3A , 0x8B },
        { 0x3C , 0x10 },
        { 0x43 , 0x8B },
        { 0x45 , 0x14 },
        { 0x4C , 0x8B },
        { 0x4E , 0x18 },
        { 0x55 , 0x8B },
        { 0x57 , 0x1C },
        { 0x5E , 0x8B },
        { 0x60 , 0x20 },
OOVPA_END;

// ******************************************************************
// * CDirectSound_DownloadEffectsImage
// ******************************************************************
OOVPA_XREF(CDirectSound_DownloadEffectsImage, 4134, 18,

    XREF_CDirectSound_DownloadEffectsImage,
    XRefZero)

        // CDirectSound_DownloadEffectsImage+0x00 : push ebp
        { 0x00, 0x55 },

        // CDirectSound_DownloadEffectsImage+0x2C : mov esi,dword ptr [ebp+8]
        { 0x2C, 0x8B },
        { 0x2D, 0x75 },
        { 0x2E, 0x08 },

        // CDirectSound_DownloadEffectsImage+0x2F : push dword ptr [ebp+14h]
        { 0x2F, 0xFF },
        { 0x30, 0x75 },
        { 0x31, 0x14 },

        // CDirectSound_DownloadEffectsImage+0x32 : mov ecx,dword ptr [esi+8]
        { 0x32, 0x8B },
        { 0x33, 0x4E },
        { 0x34, 0x08 },

        // CDirectSound_DownloadEffectsImage+0x3A :  push dword ptr [ebp+18h]
        { 0x3A, 0xFF },
        //{ 0x3B, 0x75 },
        { 0x3C, 0x18 },

        // CDirectSound_DownloadEffectsImage+0x40 : push dword ptr [ebp+10h]
        { 0x40, 0xFF },
        //{ 0x41, 0x75 },
        { 0x42, 0x10 },

        // CDirectSound_DownloadEffectsImage+0x43 : push dword ptr [ebp+0Ch]
        { 0x43, 0xFF },
        //{ 0x44, 0x75 },
        { 0x45, 0x0C },

        // CDirectSound_DownloadEffectsImage+0x61 : ret 14h
        { 0x61, 0xC2 },
        { 0x62, 0x14 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream::Release
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_Release, 4134, 1+11,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x32, XREF_DSound_CRefCount_Release ),

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },

        { 0x2D, 0x83 },
        { 0x2E, 0xC0 },
        { 0x2F, 0x04 },

        { 0x47, 0x8B },

        { 0x4B, 0xC2 },
        { 0x4C, 0x04 },
        { 0x4D, 0x00 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CFullHRTFSource::GetCenterVolume
// ******************************************************************
OOVPA_XREF(CFullHRTFSource_GetCenterVolume, 4134, 9,

    XREF_CFullHRTFSource_GetCenterVolume,
    XRefZero)

        { 0x00, 0x55 },

        { 0x22, 0xD9 },
        { 0x62, 0xDF },

        { 0xB6, 0xF0 },
        { 0xB7, 0xD8 },
        { 0xB8, 0xFF },
        { 0xB9, 0xFF },

        { 0xBC, 0xC2 },
        { 0xBD, 0x04 },
OOVPA_END;

// ******************************************************************
// * DirectSoundUseFullHRTF
// ******************************************************************
// Generic OOVPA as of 4134 and newer.
OOVPA_XREF(DirectSoundUseFullHRTF, 4134, 1+7,

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

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetMixBins
// ******************************************************************
// Generic OOVPA as of 4134 and newer.
OOVPA_XREF(CDirectSoundStream_SetMixBins, 4134, 1+9,

    XREF_CDirectSoundStream_SetMixBins,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_SetMixBins ),

        { 0x00, 0x56 },

        { 0x0C, 0x00 },

        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },

        { 0x4F, 0xC2 },
        { 0x50, 0x08 },
OOVPA_END;

// ******************************************************************
// * CMcpxStream_Pause
// ******************************************************************
OOVPA_XREF(CMcpxStream_Pause, 4134, 11,

    XREF_CMcpxStream_Pause,
    XRefZero)

        // CMcpxStream_Pause+0x00 : push ebp
        { 0x00, 0x55 },

        // CMcpxStream_Pause+0x1E : or eax, 4
        { 0x1E, 0x83 },
        { 0x1F, 0xC8 },
        { 0x20, 0x04 },

        // CMcpxStream_Pause+0x21 : jmp +0x0D
        { 0x21, 0xEB },
        { 0x22, 0x0D },

        // CMcpxStream_Pause+0x2D : and eax,-05
        { 0x2D, 0x83 },
        { 0x2E, 0xE0 },
        { 0x2F, 0xFB },

        // CMcpxStream_Pause+0x44 : retn 0x04
        { 0x44, 0xC2 },
        { 0x45, 0x04 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_Pause
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(CDirectSoundStream_Pause, 4134, 1+11,

    XREF_CDirectSoundStream_Pause,
    XRefOne)

        // CDirectSoundStream_Pause+0x34 : call [CMcpxStream::Pause]
        XREF_ENTRY( 0x35, XREF_CMcpxStream_Pause ),

        // CDirectSoundStream_Pause+0x00 : push esi
        { 0x00, 0x56 },

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

// ******************************************************************
// * CMcpxVoiceClient::SetLFO
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_SetLFO, 4134, 10,

    XREF_CMcpxVoiceClient_SetLFO,
    XRefZero)

        // CMcpxVoiceClient_SetLFO+0x00 : push ebp
        { 0x00, 0x55 },

        // CMcpxVoiceClient_SetLFO+0x38 : jz +0x71
        { 0x38, 0x74 },
        { 0x39, 0x71 },

        // CMcpxVoiceClient_SetLFO+0x3A : movzx ecx, byte ptr [esi+64h]
        { 0x3A, 0x0F },
        { 0x3B, 0xB6 },
        { 0x3C, 0x4E },
        { 0x3D, 0x64 },

        // CMcpxVoiceClient_SetLFO+0x59 : movzx ecx, word ptr [eax]
        { 0x59, 0x0F },
        { 0x5A, 0xB7 },
        { 0x5B, 0x08 },
OOVPA_END;

// ******************************************************************
// * CMcpxVoiceClient::SetEG
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_SetEG, 4134, 12,

    XREF_CMcpxVoiceClient_SetEG,
    XRefZero)

        { 0x00, 0x55 },
        { 0x25, 0xEB },

        { 0x38, 0x74 },
        { 0x39, 0x7B },

        { 0x3E, 0x8D },
        { 0x3F, 0x0C },
        { 0x40, 0xC0 },

        { 0xAE, 0x41 },
        { 0xAF, 0x40 },
        { 0xB0, 0x40 },

        { 0xC1, 0xC2 },
        { 0xC2, 0x04 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetEG
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetEG, 4134, 1+7,

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
// * CDirectSoundBuffer::SetLFO
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetLFO, 4134, 1+7,

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
// * CDirectSound::SetAllParameters
// ******************************************************************
OOVPA_XREF(CDirectSound_SetAllParameters, 4134, 11,

    XREF_CDirectSound_SetAllParameters,
    XRefZero)

        { 0x00, 0x56 },
        { 0x0D, 0x0F },

        { 0x42, 0xD9 },
        { 0x43, 0x5A },
        { 0x44, 0x3C },

        { 0x4B, 0xD9 },
        { 0x4C, 0x5A },
        { 0x4D, 0x40 },

        { 0x54, 0xD9 },
        { 0x55, 0x5A },
        { 0x56, 0x44 },

        // Removed both OVs to support 4361 titles
        //{ 0xE3, 0xC2 },
        //{ 0xE4, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_GetInfo
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_NO_XREF(CDirectSoundStream_GetInfo, 4134, 17)

        //CDirectSoundStream_GetInfo+0x00 : push ebx
        { 0x00, 0x53 },

        //CDirectSoundStream_GetInfo+0x2E : mov edi, dword ptr [esp + 10h]
        { 0x2E, 0x8B },
        { 0x2F, 0x7C },
        { 0x30, 0x24 },
        { 0x31, 0x10 },

        //CDirectSoundStream_GetInfo+0x38 : mov, dword ptr [edi + 24h]
        { 0x38, 0x8B },
        { 0x39, 0x4F },
        { 0x3A, 0x24 },

        //CDirectSoundStream_GetInfo+0x43 : mov eax, dword ptr [edi + 20h]
        { 0x43, 0x8B },
        { 0x44, 0x47 },
        { 0x45, 0x20 },

        //CDirectSoundStream_GetInfo+0x49 : and dword ptr [esi + 8], 0
        { 0x49, 0x83 },
        { 0x4A, 0x66 },
        { 0x4B, 0x08 },
        { 0x4C, 0x00 },

        //CDirectSoundStream_GetInfo+0x64 : ret 8
        { 0x64, 0xC2 },
        { 0x65, 0x08 },
OOVPA_END;

// ******************************************************************
// * CMcpxStream_Flush
// ******************************************************************
OOVPA_XREF(CMcpxStream_Flush, 4134, 10,
    XREF_CMcpxStream_Flush,
    XRefZero)

        { 0x00, 0x55 },
        { 0x01, 0x8B },

        { 0x12, 0x33 },

        { 0x3D, 0x83 },
        { 0x3E, 0xFE },
        { 0x3F, 0x04 },

        { 0x56, 0xE8 },

        { 0x67, 0xE8 },

        { 0xD0, 0xC9 },
        { 0xD1, 0xC3 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_Flush
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_Flush, 4134, 1+8,
    XRefNoSaveIndex,
    XRefOne)

        //CDirectSoundStream_Flush+0x30 : call [CMcpxStream_Flush]
        XREF_ENTRY( 0x31, XREF_CMcpxStream_Flush ),

        { 0x00, 0x56 },

        { 0x28, 0x8B },
        { 0x2B, 0x08 },

        { 0x2C, 0x8B },
        { 0x2E, 0x24 },

        { 0x30, 0xE8 },

        //CDirectSoundStream_Flush+0x4A : ret 4
        { 0x4A, 0xC2 },
        { 0x4B, 0x04 },
OOVPA_END;

// ******************************************************************
// * CMcpxStream_Flush
// ******************************************************************
OOVPA_XREF(CMcpxStream_GetStatus, 4134, 14,
    XREF_CMcpxStream_GetStatus,
    XRefZero)

        { 0x00, 0x55 },
        { 0x01, 0x8B },

        // Added 2 OVs to enable support 4242 titles and later (if any has occurred).
        { 0x2F, 0x03 },
        { 0x32, 0x03 },

        // Just a note, this asm code is unique
        { 0x3D, 0x80 },
        { 0x3E, 0x48 },
        { 0x3F, 0x02 },
        { 0x40, 0x02 },

        { 0x4B, 0x81 },
        { 0x4C, 0xC9 },
        { 0x4D, 0x00 },
        { 0x4E, 0x00 },
        { 0x4F, 0x04 },
        { 0x50, 0x00 },

        // Removed due to 4242 has a return asm code at offset 0x75.
        //{ 0x65, 0xC2 },
        //{ 0x66, 0x04 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_GetStatus
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(CDirectSoundStream_GetStatus, 4134, 1+11,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_GetStatus+0x34 : call [CMcpxStream::GetStatus]
        XREF_ENTRY( 0x35, XREF_CMcpxStream_GetStatus ),

        // CDirectSoundStream_GetStatus+0x00 : push esi
        { 0x00, 0x56 },

        // CDirectSoundStream_GetStatus+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 },
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // CDirectSoundStream_GetStatus+0x2C : mov ecx, [eax+0x24]
        { 0x2C, 0x8B },
        { 0x2D, 0x48 },
        { 0x2E, 0x24 },

        // CDirectSoundStream_GetStatus+0x4E : retn 0x08
        { 0x4E, 0xC2 },
        { 0x4F, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_Process
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_NO_XREF(CDirectSoundStream_Process, 4134, 15)

        { 0x00, 0x55 },

        { 0x2D, 0x8B },
        { 0x2F, 0x08 },

        { 0x30, 0x8B },
        { 0x32, 0x24 },

        //CDirectSoundStream_Process+0x49 : mov eax, 88780032h
        { 0x4A, 0x32 },
        { 0x4B, 0x00 },
        { 0x4C, 0x78 },
        { 0x4D, 0x88 },

        { 0x52, 0xFF },
        { 0x54, 0x0C },

        { 0x55, 0x8B },
        { 0x57, 0x24 },

        { 0x74, 0xC2 },
        { 0x75, 0x0C },
OOVPA_END;

// ******************************************************************
// * CMcpxStream_Discontinuity
// ******************************************************************
OOVPA_XREF(CMcpxStream_Discontinuity, 4134, 1+6,
    XREF_CMcpxStream_Discontinuity,
    XRefOne)

        //CMcpxStream_Discontinuity+0x1C : call [CMcpxStream_Flush]
        XREF_ENTRY ( 0x1D, XREF_CMcpxStream_Flush ),

        { 0x00, 0x56 },

        { 0x01, 0x8B },

        { 0x1A, 0x8B },

        { 0x1C, 0xE8 },

        { 0x23, 0x5E },

        { 0x24, 0xC3 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_Discontinuity
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(CDirectSoundStream_Discontinuity, 4134, 1+8,
    XRefNoSaveIndex,
    XRefOne)

        //CDirectSoundStream_Discontinuity+0x30 : call [CMcpxStream_Discontinuity]
        XREF_ENTRY( 0x31, XREF_CMcpxStream_Discontinuity ),

        { 0x00, 0x56 },

        { 0x28, 0x8B },
        { 0x2B, 0x08 },

        { 0x2C, 0x8B },
        { 0x2E, 0x24 },

        { 0x30, 0xE8 },

        //CDirectSoundStream_Discontinuity+0x4A : ret 4
        { 0x4A, 0xC2 },
        { 0x4B, 0x04 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CMemoryManager::PoolAlloc
// ******************************************************************
OOVPA_NO_XREF(DSound_CMemoryManager_PoolAlloc, 4134, 11)

        { 0x00, 0x56 },

        { 0x22, 0x83 },
        { 0x23, 0x7C },
        { 0x24, 0x24 },
        { 0x25, 0x10 },
        { 0x26, 0x00 },

        { 0x35, 0xF3 },
        { 0x36, 0xAB },

        { 0x42, 0xC2 },
        { 0x43, 0x0C },
        { 0x44, 0x00 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_AddRef
// ******************************************************************
// NOTE: Has identical function to DirectSound::CAc97MediaObject::AddRef
OOVPA_XREF(CDirectSoundStream_AddRef, 4134, 11,
    XRefNoSaveIndex,
    XRefZero)

        { 0x00, 0xE8 },

        { 0x27, 0x8B },
        { 0x28, 0x44 },

        { 0x2B, 0xFF },
        { 0x2C, 0x40 },
        { 0x2D, 0x08 },

        { 0x31, 0x8B },
        { 0x32, 0x70 },
        { 0x33, 0x08 },

        //CDirectSoundStream_AddRef+0x44 : ret 4
        { 0x44, 0xC2 },
        { 0x45, 0x04 },
OOVPA_END;

// ******************************************************************
// * CDirectSound::GetCaps
// ******************************************************************
OOVPA_XREF(CDirectSound_GetCaps, 4134, 12,

    XREF_CDirectSound_GetCaps,
    XRefZero)

        { 0x00, 0x57 },
        { 0x16, 0x68 },

        { 0x37, 0x44 },
        { 0x38, 0x24 },
        { 0x39, 0x18 },
        { 0x3A, 0x8B },
        { 0x3B, 0x48 },
        { 0x3C, 0x0C },
        { 0x3D, 0x56 },
        { 0x3E, 0xE8 },

        { 0x6D, 0xC2 },
        { 0x6E, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_SetOutputBuffer
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetOutputBuffer, 4134, 1+17,

    XREF_CDirectSoundBuffer_SetOutputBuffer,
    XRefOne)

        // CDirectSoundBuffer_SetOutputBuffer+0x31 : call [XREF_CDirectSoundVoice_SetOutputBuffer]
        XREF_ENTRY( 0x32, XREF_CDirectSoundVoice_SetOutputBuffer),

        // CDirectSoundBuffer_SetOutputBuffer+0x00 : push esi
        { 0x00, 0x56 },

        // CDirectSoundBuffer_SetOutputBuffer+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 },
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // CDirectSoundBuffer_SetOutputBuffer+0x29 : push [esp+0x10]; push [esp+0x10]
        { 0x29, 0xFF },
        { 0x2A, 0x74 },
        { 0x2B, 0x24 },
        { 0x2C, 0x10 },
        { 0x2D, 0xFF },
        { 0x2E, 0x74 },
        { 0x2F, 0x24 },
        { 0x30, 0x10 },

        // CDirectSoundBuffer_SetOutputBuffer+0x49 : pop edi
        { 0x49, 0x5F },

        // CDirectSoundBuffer_SetOutputBuffer+0x4B : retn 0x08
        { 0x4B, 0xC2 },
        { 0x4C, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetMixBinHeadroom
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(CDirectSound_SetMixBinHeadroom, 4134, 18,

    XREF_CDirectSound_SetMixBinHeadroom,
    XRefZero)

        // CDirectSound_SetMixBinHeadroom+0x00 : push esi
        { 0x00, 0x56 },

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

        // CDirectSound_SetMixBinHeadroom+0x39 : mov [edx+ecx+0x__], bl
        { 0x39, 0x88 },
        { 0x3A, 0x5C },
        { 0x3B, 0x0A },
        //{ 0x3C, 0x14 }, was changed to 0x10 for 5455 and later

        // CDirectSound_SetMixBinHeadroom+0x3E : mov ecx, dword ptr [eax+0x0C]
        { 0x3E, 0x8B },
        { 0x3F, 0x48 },
        { 0x40, 0x0C },

        // CDirectSound_SetMixBinHeadroom+0x5C : retn 0x0C
        { 0x5C, 0xC2 },
        { 0x5D, 0x0C },
OOVPA_END;

#if 0 // Moved to 4242
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
#endif

// ******************************************************************
// * CDirectSoundStream::SetFrequency
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetFrequency, 4134, 1+11,

    XREF_CDirectSoundStream_SetFrequency,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_SetFrequency ),

        { 0x00, 0x56 },

        { 0x0C, 0x00 },
        // je XXXXXXXX + 0x0B
        { 0x14, 0x74 },
        { 0x15, 0x0B },
        // jmp XXXXXXXX + 0x26
        { 0x26, 0xEB },
        { 0x27, 0x26 },

        { 0x2A, 0x24 },
        // je XXXXXXXX + 0x0B
        { 0x3E, 0x74 },
        { 0x3F, 0x0B },

        // ret 8
        { 0x4F, 0xC2 },
        { 0x50, 0x08 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetOutputBuffer
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetOutputBuffer, 4134, 1+8,

    XREF_CDirectSoundStream_SetOutputBuffer,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_SetOutputBuffer ),

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
// * CDirectSoundStream_SetConeAngles
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetConeAngles, 4134, 1+8,

    XREF_CDirectSoundStream_SetConeAngles,
    XRefOne)

        XREF_ENTRY( 0x3E, XREF_CDirectSoundVoice_SetConeAngles ),

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },

        { 0x30, 0x18 },
        { 0x33, 0x04 },
        { 0x37, 0x18 },

        { 0x3D, 0xE8 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetHeadroom
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetHeadroom, 4134, 1+8,

    XREF_CDirectSoundStream_SetHeadroom,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_SetHeadroom ),

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
// * public: long __thiscall DirectSound::CMcpxBuffer::Play(unsigned long)
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_Play, 4134, 13,

    XREF_CMcpxBuffer_Play,
    XRefZero)

        // CMcpxBuffer_Play+0x00 : push ebx
        { 0x00, 0x53 },

        // CMcpxBuffer_Play+0x0E : cmp ebx, edi
        { 0x0E, 0x3B },
        { 0x0F, 0xDF },

        // CMcpxBuffer_Play+0x18 : test byte ptr [eax+09],0x20
        { 0x18, 0xF6 },
        { 0x19, 0x40 },
        { 0x1A, 0x09 },
        { 0x1B, 0x20 },

        // CMcpxBuffer_Play+0x1C : jz +0x09
        { 0x1C, 0x74 },
        { 0x1D, 0x09 },

        // CMcpxBuffer_Play+0x30 : test byte ptr [esi+0x12], 2
        { 0x30, 0xF6 },
        { 0x31, 0x46 },
        { 0x32, 0x12 },
        { 0x33, 0x02 },
OOVPA_END;

#if 0 // Moved to 4039
// ******************************************************************
// * public: long __thiscall DirectSound::CMcpxBuffer::Play(__int64,unsigned long)
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_Play2, 4134, 1+8,

    XREF_CMcpxBuffer_Play2,
    XRefOne)

        // CMcpxBuffer_Play2+0x2B : call [CMcpxBuffer::Play]
        XREF_ENTRY( 0x2B, XREF_CMcpxBuffer_Play ),

        // CMcpxBuffer_Play2+0x00 : push ebp
        { 0x00, 0x55 },

        // CMcpxBuffer_Play2+0x11 : push [ebp+arg_8]
        { 0x11, 0xFF },
        { 0x12, 0x75 },
        { 0x13, 0x10 },

        // CMcpxBuffer_Play2+0x23 : jnz +0x0C
        { 0x23, 0x75 },
        { 0x24, 0x0C },

        // CMcpxBuffer_Play2+0x36 : push esi
        { 0x36, 0xC2 },
        { 0x37, 0x0C },
OOVPA_END;
#endif

// ******************************************************************
// * CDirectSoundBuffer_Play
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_Play, 4134, 1+10,

    XREF_CDirectSoundBuffer_Play,
    XRefOne)

        // CDirectSoundBuffer_Play+0x35 : call [CMcpxBuffer::Play]
        XREF_ENTRY( 0x35, XREF_CMcpxBuffer_Play ),

        // CDirectSoundBuffer_Play+0x00 : push esi
        { 0x00, 0x56 },

        // CDirectSoundBuffer_Play+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 },
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // CDirectSoundBuffer_Play+0x3D : jz +0x0B
        { 0x3D, 0x74 },
        { 0x3E, 0x0B },

        // CDirectSoundBuffer_Play+0x4E : retn 0x10
        { 0x4E, 0xC2 },
        { 0x4F, 0x10 },
OOVPA_END;

// ******************************************************************
// * CDirectSound_GetEffectData
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(CDirectSound_GetEffectData, 4134, 14,

    XREF_CDirectSound_GetEffectData,
    XRefZero)

        { 0x00, 0x55 },
        { 0x0F, 0x00 },

        { 0x24, 0xB8 },
        { 0x25, 0x05 },
        { 0x26, 0x40 },
        { 0x27, 0x00 },
        { 0x28, 0x80 },

        { 0x29, 0xEB },
        { 0x2A, 0x2F },

        { 0x2B, 0x8B },
        { 0x2C, 0x45 },

        { 0x41, 0xE8 },

        { 0x5C, 0xC2 },
        { 0x5D, 0x14 },
OOVPA_END;

// ******************************************************************
// * CDirectSound_CommitEffectData
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(CDirectSound_CommitEffectData, 4134, 16,

    XREF_CDirectSound_CommitEffectData,
    XRefZero)

        { 0x00, 0x56 },
        { 0x0C, 0x00 },

        { 0x21, 0xB8 },
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        { 0x26, 0xEB },
        { 0x27, 0x24 },

        { 0x28, 0x8B },
        { 0x29, 0x44 },

        { 0x32, 0x6A },
        { 0x34, 0x6A },

        { 0x36, 0xE8 },

        { 0x4D, 0xC2 },
        { 0x4E, 0x04 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetDistanceFactor
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(CDirectSoundStream_SetDistanceFactor, 4134, 1+8,

    XREF_CDirectSoundStream_SetDistanceFactor,
    XRefOne)

        XREF_ENTRY( 0x3E, XREF_CDirectSoundVoice_SetDistanceFactor ),

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x3D, 0xE8 },
        { 0x48, 0x68 },
        { 0x53, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundStream_SetDistanceFactor
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(IDirectSoundStream_SetDistanceFactor, 4134, 1+8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x11, XREF_CDirectSoundStream_SetDistanceFactor),

        { 0x00, 0xFF },
        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x09, 0xD9 },
        { 0x0C, 0xFF },
        { 0x0E, 0x24 },
        { 0x15, 0xC2 },
        { 0x16, 0x0C },
OOVPA_END;

// ******************************************************************
// * IDirectSoundStream_SetRolloffFactor
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(IDirectSoundStream_SetRolloffFactor, 4134, 1+8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x11, XREF_CDirectSoundStream_SetRolloffFactor),

        { 0x00, 0xFF },
        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x09, 0xD9 },
        { 0x0C, 0xFF },
        { 0x0E, 0x24 },
        { 0x15, 0xC2 },
        { 0x16, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetDopplerFactor
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(CDirectSoundStream_SetDopplerFactor, 4134, 1+8,

    XREF_CDirectSoundStream_SetDopplerFactor,
    XRefOne)

        XREF_ENTRY( 0x3E, XREF_CDirectSoundVoice_SetDopplerFactor),

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x3D, 0xE8 },
        { 0x48, 0x68 },
        { 0x53, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundStream_SetDopplerFactor
// ******************************************************************
// Generic OOVPA as of 4134 and newer
OOVPA_XREF(IDirectSoundStream_SetDopplerFactor, 4134, 1+8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x11, XREF_CDirectSoundStream_SetDopplerFactor),

        { 0x00, 0xFF },
        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x09, 0xD9 },
        { 0x0C, 0xFF },
        { 0x0E, 0x24 },
        { 0x15, 0xC2 },
        { 0x16, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetMixBinVolumes_8
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMixBinVolumes_8, 4134, 1+11,

    XREF_CDirectSoundStream_SetMixBinVolumes_8,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_SetMixBinVolumes ),

        { 0x00, 0x56 },

        { 0x0C, 0x00 },
        { 0x0E, 0xB6 },
        { 0x0F, 0xF0 },
        { 0x21, 0xB8 },
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },
        { 0x4F, 0xC2 },
        { 0x50, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetPlayRegion
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetPlayRegion, 4134, 10,

    XREF_CDirectSoundBuffer_SetPlayRegion,
    XRefZero)

        // CDirectSoundBuffer_SetPlayRegion+0x10 : movzx edi, al
        { 0x10, 0x0F },
        { 0x11, 0xB6 },
        { 0x12, 0xF8 },

        // CDirectSoundBuffer_SetPlayRegion+0x17 : jz +0x0B
        { 0x17, 0x74 },
        { 0x18, 0x0B },

        // CDirectSoundBuffer_SetPlayRegion+0x46 : mov esi, 0x88780032
        { 0x46, 0xBE },
        { 0x47, 0x32 },
        { 0x48, 0x00 },
        { 0x49, 0x78 },
        { 0x4A, 0x88 },
OOVPA_END;

// ******************************************************************
// * CDirectSound::EnableHeadphones
// ******************************************************************
OOVPA_XREF(CDirectSound_EnableHeadphones, 4134, 12,

    XREF_CDirectSound_EnableHeadphones,
    XRefZero)

        { 0x00, 0x55 },
        { 0x29, 0xB8 },

        { 0x4C, 0x83 },
        { 0x4D, 0x7D },
        { 0x4E, 0x0C },
        { 0x4F, 0x00 },
        { 0x50, 0x74 },
        { 0x51, 0x07 },
        { 0x52, 0x0D },
        { 0x53, 0x00 },

        { 0x82, 0xC2 },
        { 0x83, 0x08 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetFormat
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetFormat, 4134, 1+7,

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
// * DirectSound::CDirectSoundStream::SetFormat
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetFormat, 4134, 1+8,

    XREF_CDirectSoundStream_SetFormat,
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
// * DirectSound::CDirectSoundBuffer::PlayEx
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_PlayEx, 4134, 1+7,

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
// * DirectSound::CDirectSoundBuffer::StopEx
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_StopEx, 4134, 1+7,

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
// * DirectSoundCreateBuffer
// ******************************************************************
OOVPA_XREF(DirectSoundCreateBuffer, 4134, 1+11,

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
// * DirectSound::CDirectSoundStream::FlushEx
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_FlushEx, 4134, 14,

    XREF_CDirectSoundStream_FlushEx,
    XRefZero)

        { 0x00, 0x55 },
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
OOVPA_XREF(IDirectSoundStream_FlushEx, 4134, 1+8,

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
OOVPA_NO_XREF(XAudioDownloadEffectsImage, 4134, 8)

        { 0x1E, 0xB6 },
        { 0x41, 0x83 },
        { 0x61, 0x8B },
        { 0x80, 0x85 },
        { 0x9E, 0x8B },
        { 0xBE, 0x6A },
        { 0xDE, 0x07 },
        { 0xFE, 0xF7 },
OOVPA_END;
