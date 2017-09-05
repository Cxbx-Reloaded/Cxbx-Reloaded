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

    XREF_DSCREATESOUNDBUFFER,
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
OOVPA_XREF(IDirectSoundBuffer_SetLoopRegion, 4134, 9,

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

    XREF_DSSETI3DL2LISTENER,
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

// ******************************************************************
// * IDirectSound_SetI3DL2Listener
// ******************************************************************
OOVPA_XREF(IDirectSound_SetI3DL2Listener, 4134, 12,

	XRefNoSaveIndex,
	XRefOne)

	// IDirectSound_SetI3DL2Listener+0x19 : call [CDirectSound::SetI3DL2Listener]
	XREF_ENTRY(0x19, XREF_DSSETI3DL2LISTENER),

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


// ******************************************************************
// * CDirectSoundVoice_SetHeadroom
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetHeadroom, 4134, 12,

    XREF_CDirectSoundVoice_SetHeadroom,
    XRefZero)

        { 0x00, 0x8B },
        { 0x08, 0x48 },
        { 0x0F, 0x2B },
        { 0x13, 0x1C },
        { 0x14, 0x89 },
        { 0x15, 0x70 },
        { 0x16, 0x20 },
        { 0x17, 0x8B },
        { 0x18, 0x4A },
        { 0x19, 0x0C },
        { 0x1A, 0xE8 },
        { 0x21, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_SetHeadroom
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetHeadroom, 4134, 8,

	XREF_CDirectSoundBuffer_SetHeadroom,
	XRefOne)

        XREF_ENTRY( 0x32, XREF_CDirectSoundVoice_SetHeadroom ),

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

// ******************************************************************
// * CDirectSoundVoiceSettings::SetMixBins
// ******************************************************************
OOVPA_XREF(CDirectSoundVoiceSettings_SetMixBins, 4134, 11,

	XREF_DSSETMIXBINSC,
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

// ******************************************************************
// * CDirectSoundVoice::SetMixBins
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMixBins, 4134, 17,

    XREF_DSSETMIXBINSB,
    XRefOne)

        // CDirectSoundVoice_SetMixBins+0x0D : call [CDirectSoundVoiceSettings::SetMixBins]
        XREF_ENTRY( 0x0D, XREF_DSSETMIXBINSC ),

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

// ******************************************************************
// * CDirectSoundBuffer::SetMixBins
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetMixBins, 4134, 17,

    XREF_DSSETMIXBINSA,
    XRefOne)

        // CDirectSoundBuffer_SetMixBins+0x32 : call [CDirectSoundVoice::SetMixBins]
        XREF_ENTRY( 0x32, XREF_DSSETMIXBINSB ),

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

// ******************************************************************
// * IDirectSoundBuffer_SetMixBins
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMixBins, 4134, 12,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetMixBins+0x15 : call [CDirectSoundBuffer::SetMixBins]
        XREF_ENTRY( 0x15, XREF_DSSETMIXBINSA ),

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

// ******************************************************************
// * CDirectSound_SetPosition
// ******************************************************************
OOVPA_XREF(CDirectSound_SetPosition, 4134, 11,

    XREF_DSSETPOSITIONA,
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
        { 0x4C, 0x80 },
        { 0x4D, 0x88 },
        { 0x4E, 0xA4 },
        { 0x4F, 0x00 },
        { 0x52, 0xFF },
OOVPA_END;

// ******************************************************************
// * IDirectSound_CreateSoundBuffer
// ******************************************************************
OOVPA_XREF(IDirectSound_CreateSoundBuffer, 4134, 12,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSound_CreateSoundBuffer+0x1D : call [CDirectSound::CreateSoundBuffer]
        XREF_ENTRY( 0x1D, XREF_DSCREATESOUNDBUFFER ),

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
OOVPA_XREF(CDirectSoundBuffer_SetFrequency, 4134, 17,

    XREF_DSBUFFERSETFREQUENCYA,
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
OOVPA_XREF(IDirectSoundBuffer_SetFrequency, 4134, 12,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetFrequency+0x15 : call [CDirectSound::SetFrequency]
        XREF_ENTRY( 0x15, XREF_DSBUFFERSETFREQUENCYA ),

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
OOVPA_XREF(CMcpxVoiceClient_SetVolume, 4134, 13,

    XREF_DSSTREAMSETVOLUME,
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
OOVPA_XREF(CDirectSoundVoice_SetVolume, 4134, 11,

    XREF_CDirectSoundVoice_SetVolume,
    XRefOne)

        // CDirectSoundVoice_SetVolume+0x15 : call [CMcpxVoiceClient::SetVolume]
        XREF_ENTRY( 0x15, XREF_DSSTREAMSETVOLUME ), 

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
OOVPA_XREF(CDirectSoundStream_SetVolume, 4134, 9,

    XRefNoSaveIndex,
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
OOVPA_XREF(CDirectSoundBuffer_Lock, 4134, 13,

    XREF_DSBUFFERLOCKA,
    XRefZero)

        // CDirectSoundBuffer_Lock+0x37 : test [ebp+0x24], 1
        { 0x37, 0xF6 },
        { 0x38, 0x45 },
        { 0x39, 0x24 },
        { 0x3A, 0x01 },

        // CDirectSoundBuffer_Lock+0x5C : mov eax, [eax+0xBC]
        { 0x5C, 0x8B },
        { 0x5D, 0x80 },
        { 0x5E, 0xBC },
        { 0x5F, 0x00 },

        // CDirectSoundBuffer_Lock+0xA4 : jnb +0x11; mov esi, [esi+0x1C]
        { 0xA2, 0x73 },
        { 0xA3, 0x11 },
        { 0xA4, 0x8B },
        { 0xA5, 0x76 },
        { 0xA6, 0x1C },
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

// ******************************************************************
// * CDirectSoundBuffer_SetBufferData
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetBufferData, 4134, 10,

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

// ******************************************************************
// * IDirectSoundBuffer_SetBufferData
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetBufferData, 4134, 8,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetBufferData+0x19 : call [CDirectSoundBuffer::SetBufferData]
        XREF_ENTRY( 0x19, XREF_CDirectSoundBuffer_SetBufferData), 

        // IDirectSoundBuffer_SetBufferData+0x0E : add eax, 0xFFFFFFE4
        { 0x0E, 0x83 },
        { 0x0F, 0xC0 },
        { 0x10, 0xE4 },

        // IDirectSoundBuffer_SetBufferData+0x13 : sbb ecx, ecx
        { 0x13, 0x1B },
        { 0x14, 0xC9 },

        // IDirectSoundBuffer_SetBufferData+0x15 : and ecx, eax
        { 0x15, 0x23 },
        { 0x16, 0xC8 },
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
OOVPA_XREF(CDirectSoundBuffer_GetStatus, 4134, 10,

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
OOVPA_XREF(CDirectSoundBuffer_SetCurrentPosition, 4134, 10,

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

// ******************************************************************
// * IDirectSoundBuffer_SetCurrentPosition
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetCurrentPosition, 4134, 8,

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

// ******************************************************************
// * CMcpxBuffer_GetCurrentPosition
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_GetCurrentPosition, 4134, 11,

	XREF_CMcpxBuffer_GetCurrentPosition,
    XRefZero)

        // CMcpxBuffer_GetCurrentPosition+0x0E : lea ecx, [ebp-0x08]
        { 0x0E, 0x8D },
        { 0x0F, 0x4D },
        { 0x10, 0xF8 },

        // CMcpxBuffer_GetCurrentPosition+0x31 : jz +0x02
        { 0x31, 0x74 },
        { 0x32, 0x02 },

        // CMcpxBuffer_GetCurrentPosition+0xA0 : div dword ptr [ecx+0xBC]
        { 0xA0, 0xF7 },
        { 0xA1, 0xB1 },
        { 0xA2, 0xBC },

        // CMcpxBuffer_GetCurrentPosition+0xD6 : retn 0x08
        { 0xD6, 0xC2 },
        { 0xD7, 0x08 },
        { 0xD8, 0x00 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_GetCurrentPosition
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_GetCurrentPosition, 4134, 10,

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

        // CDirectSoundBuffer_GetCurrentPosition+0x3D : retn 0x08
        { 0x52, 0xC2 },
        { 0x53, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::Play
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_Play, 4134, 11,

	XREF_CDirectSoundBuffer_Play,
	XRefZero)

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

// ******************************************************************
// * CDirectSound::SetDistanceFactorA
// ******************************************************************
OOVPA_XREF(CDirectSound_SetDistanceFactor, 4134, 17,

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

        // CDirectSound_SetDistanceFactor+0x39 : or dword ptr[eax+0xA4], 0x60
        { 0x39, 0x83 },
        { 0x3A, 0x88 },
        { 0x3B, 0xA4 },
        { 0x3F, 0x60 },

        // CDirectSound_SetDistanceFactor+0x4F : jz +0x0B
        { 0x4F, 0x74 },
        { 0x50, 0x0B },
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetDistanceFactor
// ******************************************************************
OOVPA_XREF(IDirectSound_SetDistanceFactor, 4134, 11,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSound_SetDistanceFactor+0x1D : call [CDirectSound::SetDistanceFactor]
        XREF_ENTRY( 0x1D, XREF_SETDISTANCEFACTORA ), 

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

// ******************************************************************
// * CDirectSound::SetRolloffFactor
// ******************************************************************
OOVPA_XREF(CDirectSound_SetRolloffFactor, 4134, 17,

    XREF_SETROLLOFFFACTORA,
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

// ******************************************************************
// * IDirectSound_SetRolloffFactor
// ******************************************************************
OOVPA_XREF(IDirectSound_SetRolloffFactor, 4134, 11,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSound_SetRolloffFactor+0x1D : call [CDirectSound::SetRolloffFactor]
        XREF_ENTRY( 0x1D, XREF_SETROLLOFFFACTORA ), 

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

// ******************************************************************
// * CDirectSound::SetDopplerFactor
// ******************************************************************
OOVPA_XREF(CDirectSound_SetDopplerFactor, 4134, 14,

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

        // CDirectSound_SetDopplerFactor+0x39 : or dword ptr[eax+0xA4], 0x40
        { 0x39, 0x83 },
        { 0x3A, 0x88 },
        { 0x3B, 0xA4 },
        { 0x3F, 0x40 },

        // CDirectSound_SetDopplerFactor+0x4F : jz +0x0B
        { 0x4F, 0x74 },
        { 0x50, 0x0B },
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetDopplerFactor
// ******************************************************************
OOVPA_XREF(IDirectSound_SetDopplerFactor, 4134, 11,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSound_SetDopplerFactor+0x1D : call [CDirectSound::SetDopplerFactor]
        XREF_ENTRY( 0x1D, XREF_SETDOPPLERFACTOR), 

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

// ******************************************************************
// * CDirectSound::CommitDeferredSettings
// ******************************************************************
OOVPA_NO_XREF(CDirectSound_CommitDeferredSettings, 4134, 11)

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
        { 0x1F, 0x70 },

        // CDirectSoundVoice_SetMaxDistance+0x30 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 }
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetMaxDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetMaxDistance, 4134, 10,

    XREF_DSBUFFERSETMAXDISTANCE,
    XRefOne)

        // CDirectSoundBuffer_SetMaxDistance+0x39 : call [CDirectSoundVoice::SetMaxDistance]
        XREF_ENTRY( 0x3A, XREF_DSVOICESETMAXDISTANCE ),

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

// ******************************************************************
// * IDirectSoundBuffer_SetMaxDistance
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMaxDistance, 4134, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetMaxDistance+0x1C : call [CDirectSoundBuffer::SetMaxDistance]
        XREF_ENTRY( 0x1D, XREF_DSBUFFERSETMAXDISTANCE ),

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

// ******************************************************************
// * CDirectSoundStream_SetMaxDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMaxDistance, 4134, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x3E, XREF_DSVOICESETMAXDISTANCE ),

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
// * CDirectSoundVoice::SetMinDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMinDistance, 4134, 12,

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
        { 0x1F, 0x70 },

        // CDirectSoundVoice_SetMinDistance+0x30 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 }
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetMinDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetMinDistance, 4134, 10,

    XREF_DSBUFFERSETMINDISTANCE,
    XRefOne)

        // CDirectSoundBuffer_SetMinDistance+0x39 : call [CDirectSoundVoice::SetMinDistance]
        XREF_ENTRY( 0x3A, XREF_DSVOICESETMINDISTANCE ),

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

// ******************************************************************
// * IDirectSoundBuffer_SetMinDistance
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMinDistance, 4134, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetMinDistance+0x1C : call [CDirectSoundBuffer::SetMinDistance]
        XREF_ENTRY( 0x1D, XREF_DSBUFFERSETMINDISTANCE ),

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

// ******************************************************************
// * CDirectSoundStream_SetMinDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMinDistance, 4134, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x3E, XREF_DSVOICESETMINDISTANCE ),

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
// * CDirectSoundVoice::SetRolloffFactor
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetRolloffFactor, 4134, 12,

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
        { 0x1F, 0x70 },

        // CDirectSoundVoice_SetRolloffFactor+0x30 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 }
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetRolloffFactor
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetRolloffFactor, 4134, 10,

    XREF_DSBUFFERSETROLLOFFFACTOR,
    XRefOne)

        // CDirectSoundBuffer_SetRolloffFactor+0x39 : call [CDirectSoundVoice::SetRolloffFactor]
        XREF_ENTRY( 0x3A, XREF_DSVOICESETROLLOFFFACTOR ),

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
OOVPA_XREF(IDirectSoundBuffer_SetRolloffFactor, 4134, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetRolloffFactor+0x1C : call [CDirectSoundBuffer::SetRolloffFactor]
        XREF_ENTRY( 0x1D, XREF_DSBUFFERSETROLLOFFFACTOR ),

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
OOVPA_XREF(CDirectSoundStream_SetRolloffFactor, 4134, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x3E, XREF_DSVOICESETROLLOFFFACTOR ),

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
        { 0x1F, 0x70 },

        // CDirectSoundVoice_SetDistanceFactor+0x30 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetDistanceFactor
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetDistanceFactor, 4134, 10,

    XREF_DSBUFFERSETDISTANCEFACTOR,
    XRefOne)

        // CDirectSoundBuffer_SetDistanceFactor+0x39 : call [CDirectSoundVoice::SetDistanceFactor]
        XREF_ENTRY( 0x3A, XREF_DSVOICESETDISTANCEFACTOR ),

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
OOVPA_XREF(IDirectSoundBuffer_SetDistanceFactor, 4134, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetDistanceFactor+0x1C : call [CDirectSoundBuffer::SetDistanceFactor]
        XREF_ENTRY( 0x1D, XREF_DSBUFFERSETDISTANCEFACTOR ),

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

    XREF_DSVOICESETCONEANGLES,
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
OOVPA_XREF(CDirectSoundBuffer_SetConeAngles, 4134, 11,

    XREF_DSBUFFERSETCONEANGLES,
    XRefOne)

        // CDirectSoundBuffer_SetConeAngles+0x39 : call [CDirectSoundVoice::SetConeAngles]
        XREF_ENTRY( 0x3A, XREF_DSVOICESETCONEANGLES ),

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

// ******************************************************************
// * IDirectSoundBuffer_SetConeAngles
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetConeAngles, 4134, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetConeAngles+0x1C : call [CDirectSoundBuffer::SetConeAngles]
        XREF_ENTRY( 0x1D, XREF_DSBUFFERSETCONEANGLES ),

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

// ******************************************************************
// * CDirectSoundVoice::SetConeOrientation
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetConeOrientation, 4134, 8,

    XREF_DSVOICESETCONEORIENTATION,
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
OOVPA_XREF(CDirectSoundBuffer_SetConeOrientation, 4134, 9,

    XREF_DSBUFFERSETCONEORIENTATION,
    XRefOne)

        // CDirectSoundBuffer_SetConeOrientation+0x49 : call [CDirectSoundVoice::SetConeOrientation]
        XREF_ENTRY( 0x4A, XREF_DSVOICESETCONEORIENTATION ),

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
OOVPA_XREF(CDirectSoundStream_SetConeOrientation, 4134, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x4E, XREF_DSVOICESETCONEORIENTATION ),

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
        { 0x1F, 0x70 },

        // CDirectSoundVoice_SetConeOutsideVolume+0x30 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 }
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetConeOutsideVolume
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetConeOutsideVolume, 4134, 11,

    XREF_DSBUFFERSETCONEOUTSIDEVOLUME,
    XRefOne)

        // CDirectSoundBuffer_SetConeOutsideVolume+0x35 : call [CDirectSoundVoice::SetConeOutsideVolume]
        XREF_ENTRY( 0x36, XREF_DSVOICESETCONEOUTSIDEVOLUME ),

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
OOVPA_XREF(CDirectSoundStream_SetConeOutsideVolume, 4134, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x3A, XREF_DSVOICESETCONEOUTSIDEVOLUME ),

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

        // CDirectSoundVoice_SetPosition+0x4F : retn 14h
        { 0x4F, 0xC2 },
        { 0x50, 0x14 }
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetPosition
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetPosition, 4134, 9,

    XREF_DSBUFFERSETPOSITION,
    XRefOne)

        // CDirectSoundBuffer_SetPosition+0x49 : call [CDirectSoundVoice::SetPosition]
        XREF_ENTRY( 0x4A, XREF_DSVOICESETPOSITION ),

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
OOVPA_XREF(CDirectSoundStream_SetPosition, 4134, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x4E, XREF_DSVOICESETPOSITION ),

        { 0x00, 0x55 },
        { 0x0F, 0x00 },
        { 0x17, 0x74 },
        { 0x24, 0xB8 },
        { 0x2A, 0x3B },
        { 0x39, 0xEC },
        { 0x58, 0x68 },
        { 0x63, 0x8B },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetVelocity
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetVelocity, 4134, 8,

    XREF_DSVOICESETVELOCITY,
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
OOVPA_XREF(CDirectSoundBuffer_SetVelocity, 4134, 9,

    XREF_DSBUFFERSETVELOCITY,
    XRefOne)

        // CDirectSoundBuffer_SetVelocity+0x49 : call [CDirectSoundVoice::SetVelocity]
        XREF_ENTRY( 0x4A, XREF_DSVOICESETVELOCITY ),

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
OOVPA_XREF(CDirectSoundStream_SetVelocity, 4134, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x4E, XREF_DSVOICESETVELOCITY ),

        { 0x00, 0x55 },
        { 0x0F, 0x00 },
        { 0x17, 0x74 },
        { 0x24, 0xB8 },
        { 0x2A, 0x3B },
        { 0x35, 0x83 },
        { 0x40, 0x45 },
        { 0x63, 0x8B },
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
OOVPA_XREF(CDirectSoundBuffer_SetDopplerFactor, 4134, 10,

    XREF_DSBUFFERSETDOPPLERFACTOR,
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
OOVPA_XREF(IDirectSoundBuffer_SetDopplerFactor, 4134, 9,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetDopplerFactor+0x1C : call [CDirectSoundBuffer::SetDopplerFactor]
        XREF_ENTRY( 0x1D, XREF_DSBUFFERSETDOPPLERFACTOR ),

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

    XREF_DSVOICESETI3DL2SOURCE,
    XRefZero)

        // CDirectSoundVoice_SetI3DL2Source+0x12 : mov esi, [ecx]
        { 0x12, 0x8B },
        { 0x13, 0x31 },

        // CDirectSoundVoice_SetI3DL2Source+0x14 : mov [edx+4Ch], esi
        { 0x14, 0x89 },
        { 0x15, 0x72 },
        { 0x16, 0x4C },

        // CDirectSoundVoice_SetI3DL2Source+0x44 : fld dword ptr [ecx+10h]
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
OOVPA_XREF(CDirectSoundBuffer_SetI3DL2Source, 4134, 11,

    XREF_DSBUFFERSETI3DL2SOURCE,
    XRefOne)

        // CDirectSoundBuffer_SetI3DL2Source+0x35 : call [CDirectSoundVoice::SetI3DL2Source]
        XREF_ENTRY( 0x36, XREF_DSVOICESETI3DL2SOURCE ),

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

// ******************************************************************
// * IDirectSoundBuffer_SetI3DL2Source
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetI3DL2Source, 4134, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetI3DL2Source+0x18 : call [CDirectSoundBuffer::SetI3DL2Source]
        XREF_ENTRY( 0x19, XREF_DSBUFFERSETI3DL2SOURCE ),

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

// ******************************************************************
// * CDirectSoundStream_SetI3DL2Source
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetI3DL2Source, 4134, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x3A, XREF_DSVOICESETI3DL2SOURCE ),

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
OOVPA_XREF(CDirectSoundBuffer_SetAllParameters, 4134, 8,

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

// ******************************************************************
// * IDirectSoundBuffer_SetAllParameters
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetAllParameters, 4134, 9,

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

// ******************************************************************
// * CDirectSoundStream_SetAllParameters
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetAllParameters, 4134, 9,

    XRefNoSaveIndex,
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

// ******************************************************************
// * IDirectSoundBuffer_GetStatus
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_GetStatus, 4134, 9,

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

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetMode
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMode, 4134, 7,

    XREF_DSBUFFERSETMODEB,
    XRefZero)

        { 0x03, 0x0C },
        { 0x08, 0x04 },
        { 0x0D, 0x89 },
        { 0x12, 0x8B },
        { 0x17, 0x51 },
        { 0x1C, 0xE8 },
        { 0x21, 0x33 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetMode
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetMode, 4134, 8,

    XREF_DSBUFFERSETMODEA,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_DSBUFFERSETMODEB ),

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x74 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetMode
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMode, 4134, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_DSBUFFERSETMODEA ),

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
// * CDirectSoundStream_SetMode
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMode, 4134, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x3A, XREF_DSBUFFERSETMODEB ),

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x39, 0xE8 },
        { 0x44, 0x68 },
        { 0x4F, 0x8B },
OOVPA_END;

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

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetFilter
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetFilter, 4134, 7,

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

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetFilter
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetFilter, 4134, 8,

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

// ******************************************************************
// * IDirectSoundBuffer_SetFilter
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetFilter, 4134, 8,

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

// ******************************************************************
// * CDirectSound::SetVelocity
// ******************************************************************
OOVPA_XREF(CDirectSound_SetVelocity, 4134, 15,

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
OOVPA_XREF(CDirectSoundBuffer_SetPitch, 4134, 8,

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

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetPitch
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetPitch, 4134, 12,

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

// ******************************************************************
// * CDirectSoundBuffer::SetVolume
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetVolume, 4134, 12,

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
OOVPA_XREF(IDirectSoundBuffer_SetVolume, 4134, 8,

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
OOVPA_XREF(CDirectSound_CreateSoundStream, 4134, 14,

    XREF_DSCREATESOUNDSTREAM,
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
OOVPA_XREF(DirectSoundCreateStream, 4134, 12,

    XRefNoSaveIndex,
    XRefOne)

        // DirectSoundCreateStream+0x2F : call [CDirectSound::CreateSoundStream]
        XREF_ENTRY( 0x2F, XREF_DSCREATESOUNDSTREAM ),

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
OOVPA_XREF(CDirectSoundVoiceSettings_SetMixBinVolumes, 4134, 10,

    XREF_DSSETMIXBINVOLUMESC,
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

// ******************************************************************
// * CDirectSoundVoice::SetMixBinVolumes
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMixBinVolumes, 4134, 17,

    XREF_DSSETMIXBINVOLUMESB,
    XRefOne)

        // CDirectSoundVoice_SetMixBinVolumes+0x0D : call [CDirectSoundVoiceSettings::SetMixBinVolumes]
        XREF_ENTRY( 0x0D, XREF_DSSETMIXBINVOLUMESC ),

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

// ******************************************************************
// * CDirectSoundBuffer::SetMixBinVolumes
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetMixBinVolumes, 4134, 17,

    XREF_DSSETMIXBINVOLUMESA,
    XRefOne)

        // CDirectSoundBuffer_SetMixBinVolumes+0x32 : call [CDirectSoundVoice::SetMixBinVolumes]
        XREF_ENTRY( 0x32, XREF_DSSETMIXBINVOLUMESB ),

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

// ******************************************************************
// * IDirectSoundBuffer_SetMixBinVolumes2
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMixBinVolumes2, 4134, 12,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetMixBinVolumes2+0x15 : call [CDirectSoundBuffer::SetMixBinVolumes]
        XREF_ENTRY( 0x15, XREF_DSSETMIXBINVOLUMESA ),

        // IDirectSoundBuffer_SetMixBinVolumes2+0x04 : push [esp+0x08]
        { 0x04, 0xFF },
        { 0x05, 0x74 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },

        // IDirectSoundBuffer_SetMixBinVolumes2+0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 },
        { 0x0B, 0xC0 },
        { 0x0C, 0xE4 },

        // IDirectSoundBuffer_SetMixBinVolumes2+0x0F : sbb ecx, ecx
        { 0x0F, 0x1B },
        { 0x10, 0xC9 },

        // IDirectSoundBuffer_SetMixBinVolumes2+0x11 : and ecx, eax
        { 0x11, 0x23 },
        { 0x12, 0xC8 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetMixBinVolumes2
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMixBinVolumes2, 4134, 12,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_DSSETMIXBINVOLUMESB ),

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

// ******************************************************************
// * CDirectSound_SetEffectData
// ******************************************************************
OOVPA_XREF(CDirectSound_SetEffectData, 4134, 13, // Also for 5849

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
        { 0x60, 0x18 },
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetEffectData
// ******************************************************************
OOVPA_XREF(IDirectSound_SetEffectData, 4134, 13,

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
// * CDirectSoundBuffer_Stop
// ******************************************************************
//Generic OOVPA as of 4134 and newer.
OOVPA_XREF(CDirectSoundBuffer_Stop, 4134, 13,

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
// * Direct, 4134
// ******************************************************************
OOVPATable DSound_4134[] = {

	REGISTER_OOVPA(DirectSoundCreate, 4134, PATCH),
	REGISTER_OOVPA(DirectSoundDoWork, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_CreateSoundStream, 4134, XREF),
	REGISTER_OOVPA(IDirectSound_CreateSoundStream, 3936, PATCH),
	REGISTER_OOVPA(DirectSoundCreateStream, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_CreateSoundBuffer, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetLoopRegion, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetLoopRegion, 3911, PATCH),
	REGISTER_OOVPA(CDirectSound_SetI3DL2Listener, 4134, XREF),
	REGISTER_OOVPA(IDirectSound_SetI3DL2Listener, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetHeadroom, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetHeadroom, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetHeadroom, 4134, PATCH),
	REGISTER_OOVPA(IDirectSound_SetVelocity, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound_SetAllParameters, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundVoiceSettings_SetMixBins, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetMixBins, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMixBins, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMixBins, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_SetPosition, 4134, XREF), // TODO: Find a cure for laziness...
	REGISTER_OOVPA(IDirectSound_SetPosition, 3936, PATCH),
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
	REGISTER_OOVPA(CDirectSoundBuffer_Lock, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Lock, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound_CreateSoundStream, 3936, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_SetBufferData, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetBufferData, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetBufferData, 4134, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_GetStatus, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_GetStatus, 4134, XREF),
	REGISTER_OOVPA(CMcpxBuffer_SetCurrentPosition, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetCurrentPosition, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetCurrentPosition, 4134, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_GetCurrentPosition, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_GetCurrentPosition, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_GetCurrentPosition, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_Play, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Play, 3911, PATCH),
	REGISTER_OOVPA(IDirectSound_Release, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound_DownloadEffectsImage, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound_SetOrientation, 3936, PATCH),
	REGISTER_OOVPA(CDirectSound_SetDistanceFactor, 4134, XREF),
	REGISTER_OOVPA(IDirectSound_SetDistanceFactor, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_SetRolloffFactor, 4134, XREF),
	REGISTER_OOVPA(IDirectSound_SetRolloffFactor, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_SetDopplerFactor, 4134, XREF),
	REGISTER_OOVPA(IDirectSound_SetDopplerFactor, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_CommitDeferredSettings, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetMaxDistance, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMaxDistance, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMaxDistance, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetMaxDistance, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetMinDistance, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMinDistance, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMinDistance, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetMinDistance, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetRolloffFactor, 4134, XREF), // s+
	REGISTER_OOVPA(CDirectSoundBuffer_SetRolloffFactor, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetRolloffFactor, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetRolloffFactor, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetDistanceFactor, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetDistanceFactor, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetDistanceFactor, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetConeAngles, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetConeAngles, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetConeAngles, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetConeOrientation, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetConeOrientation, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetConeOrientation, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetConeOrientation, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetConeOutsideVolume, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetConeOutsideVolume, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetConeOutsideVolume, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetConeOutsideVolume, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetPosition, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetPosition, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetPosition, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetPosition, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetVelocity, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetVelocity, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetVelocity, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetVelocity, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetDopplerFactor, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetDopplerFactor, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetDopplerFactor, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetI3DL2Source, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetI3DL2Source, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetI3DL2Source, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetI3DL2Source, 4134, PATCH),
    REGISTER_OOVPA(CMcpxBuffer_Stop, 4134, XREF),
    REGISTER_OOVPA(CDirectSoundBuffer_Stop, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Stop, 3911, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_Release, 3936, PATCH), // +s
	REGISTER_OOVPA(IDirectSoundBuffer_AddRef, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetAllParameters, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetAllParameters, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetAllParameters, 4134, PATCH), // Use that for now. Okay, it's your call pal...
	REGISTER_OOVPA(CDirectSoundStream_SetAllParameters, 4134, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_GetStatus, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetMode, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMode, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMode, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetMode, 4134, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_SetFilter, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetFilter, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetFilter, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetFilter, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_SetVelocity, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetPitch, 4039, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetPitch, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetPitch, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetPitch, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundVoiceSettings_SetMixBinVolumes, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetMixBinVolumes, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMixBinVolumes, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMixBinVolumes2, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetMixBinVolumes2, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_SetEffectData, 4134, XREF),
	REGISTER_OOVPA(IDirectSound_SetEffectData, 4134, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_Unlock, 3936, PATCH),
};

// ******************************************************************
// * DSound_4134_SIZE
// ******************************************************************
uint32 DSound_4134_SIZE = sizeof(DSound_4134);
