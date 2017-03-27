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
// * DirectSound_CDirectSound::CreateSoundBuffer
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_CreateSoundBuffer, 4134, 14,

    XREF_DSCREATESOUNDBUFFER,
    XRefZero)

        // DirectSound_CDirectSound_CreateSoundBuffer+0x23 : mov eax, 0x80004005
        { 0x23, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x24, 0x05 }, // (Offset,Value)-Pair #2
        { 0x25, 0x40 }, // (Offset,Value)-Pair #3
        { 0x27, 0x80 }, // (Offset,Value)-Pair #4

        // DirectSound_CDirectSound_CreateSoundBuffer+0x2A : push 0x24
        { 0x2A, 0x6A }, // (Offset,Value)-Pair #5
        { 0x2B, 0x24 }, // (Offset,Value)-Pair #6

        // DirectSound_CDirectSound_CreateSoundBuffer+0x4A : add esi, 0x7FF8FFF2
        { 0x4A, 0x81 }, // (Offset,Value)-Pair #7
        { 0x4B, 0xE6 }, // (Offset,Value)-Pair #8
        { 0x4C, 0xF2 }, // (Offset,Value)-Pair #9
        { 0x4D, 0xFF }, // (Offset,Value)-Pair #10
        { 0x4E, 0xF8 }, // (Offset,Value)-Pair #11
        { 0x4F, 0x7F }, // (Offset,Value)-Pair #12

        // DirectSound_CDirectSound_CreateSoundBuffer+0x99 : retn 0x10
        { 0x99, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x9A, 0x10 }, // (Offset,Value)-Pair #14
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundBuffer::SetLoopRegion
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetLoopRegion, 4134, 11,

    XREF_DirectSound_CDirectSoundBuffer_SetLoopRegion,
    XRefZero)

        // DirectSound_CDirectSoundBuffer_SetLoopRegion+0x24 : mov eax, 0x80004005
        { 0x24, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x25, 0x05 }, // (Offset,Value)-Pair #2
        { 0x26, 0x40 }, // (Offset,Value)-Pair #3
        { 0x27, 0x00 }, // (Offset,Value)-Pair #4
        { 0x28, 0x80 }, // (Offset,Value)-Pair #5

        // DirectSound_CDirectSoundBuffer_SetLoopRegion+0x29 : jmp +0x55
        { 0x29, 0xEB }, // (Offset,Value)-Pair #6
        { 0x2A, 0x55 }, // (Offset,Value)-Pair #7

        // DirectSound_CDirectSoundBuffer_SetLoopRegion+0x56 : sub ecx, esi
        { 0x56, 0x2B }, // (Offset,Value)-Pair #8
        { 0x57, 0xCE }, // (Offset,Value)-Pair #9

        // DirectSound_CDirectSoundBuffer_SetLoopRegion+0x82 : retn 0x0C
        { 0x82, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x83, 0x0C }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8::SetLoopRegion
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetLoopRegion, 4134, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_DirectSound_CDirectSoundBuffer_SetLoopRegion ),

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
// * DirectSound_CDirectSound::SetI3DL2Listener
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_SetI3DL2Listener, 4134, 11,

    XREF_DSSETI3DL2LISTENER,
    XRefZero)

        // DirectSound_CDirectSound_SetI3DL2Listener+0x3A : mov edi, 0x88780032
        { 0x3A, 0xBF }, // (Offset,Value)-Pair #1
        { 0x3B, 0x32 }, // (Offset,Value)-Pair #2
        { 0x3C, 0x00 }, // (Offset,Value)-Pair #3
        { 0x3D, 0x78 }, // (Offset,Value)-Pair #4
        { 0x3E, 0x88 }, // (Offset,Value)-Pair #5

        // DirectSound_CDirectSound_SetI3DL2Listener+0xA2 : fstp dword ptr[edx+0x94]
        { 0xA2, 0xD9 }, // (Offset,Value)-Pair #6
        { 0xA3, 0x9A }, // (Offset,Value)-Pair #7
        { 0xA4, 0x94 }, // (Offset,Value)-Pair #8

        // DirectSound_CDirectSound_SetI3DL2Listener+0xA8 : fld dword ptr[ecx+0x24]
        { 0xA8, 0xD9 }, // (Offset,Value)-Pair #9
        { 0xA9, 0x41 }, // (Offset,Value)-Pair #10
        { 0xAA, 0x24 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetI3DL2Listener
// ******************************************************************
OOVPA_XREF(IDirectSound_SetI3DL2Listener, 4134, 12,

	XRefNoSaveIndex,
	XRefOne)

	// IDirectSound_SetI3DL2Listener+0x19 : call [CDirectSound::SetI3DL2Listener]
	XREF_ENTRY(0x19, XREF_DSSETI3DL2LISTENER), // (Offset,Value)-Pair #1

											   // IDirectSound_SetI3DL2Listener+0x04 : push [esp+0x0C]
	{ 0x04, 0xFF }, // (Offset,Value)-Pair #2
	{ 0x05, 0x74 }, // (Offset,Value)-Pair #3
	{ 0x06, 0x24 }, // (Offset,Value)-Pair #4
	{ 0x07, 0x0C }, // (Offset,Value)-Pair #5

					// IDirectSound_SetI3DL2Listener+0x0E : add eax, 0xFFFFFFF8
	{ 0x0E, 0x83 }, // (Offset,Value)-Pair #6
	{ 0x0F, 0xC0 }, // (Offset,Value)-Pair #7
	{ 0x10, 0xF8 }, // (Offset,Value)-Pair #8

					// IDirectSound_SetI3DL2Listener+0x13 : sbb ecx, ecx
	{ 0x13, 0x1B }, // (Offset,Value)-Pair #9
	{ 0x14, 0xC9 }, // (Offset,Value)-Pair #10

					// IDirectSound_SetI3DL2Listener+0x15 : and ecx, eax
	{ 0x15, 0x23 }, // (Offset,Value)-Pair #11
	{ 0x16, 0xC8 }, // (Offset,Value)-Pair #12
	OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetHeadroomA
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetHeadroomA, 4134, 7,

	XREF_DSBUFFERSETHEADROOMA,
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
// * IDirectSoundBuffer_SetHeadroom
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetHeadroom, 4134, 8,
	XRefNoSaveIndex,
	XRefOne)

	// IDirectSoundBuffer_SetHeadroom+0x32 : call [IDirectSoundBuffer_SetHeadroomA]
	XREF_ENTRY(0x32, XREF_DSBUFFERSETHEADROOMA),

	{ 0x0C, 0x00 },
	{ 0x12, 0x85 },
	{ 0x1C, 0x15 },
	{ 0x26, 0xEB },
	{ 0x30, 0x10 },
	{ 0x3A, 0x74 },
	{ 0x47, 0x8B },
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundVoiceSettings::SetMixBins
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoiceSettings_SetMixBins, 4134, 11,

	XREF_DSSETMIXBINSC,
	XRefZero)

	// DirectSound_CDirectSoundVoiceSettings_SetMixBins+0x06 : jnz +0x1A
	{ 0x06, 0x75 }, // (Offset,Value)-Pair #1
	{ 0x07, 0x1A }, // (Offset,Value)-Pair #2

					// DirectSound_CDirectSoundVoiceSettings_SetMixBins+0x08 : test byte ptr [ecx+8], 0x10
	{ 0x08, 0xF6 }, // (Offset,Value)-Pair #3
	{ 0x09, 0x41 }, // (Offset,Value)-Pair #4
	{ 0x0A, 0x08 }, // (Offset,Value)-Pair #5
	{ 0x0B, 0x10 }, // (Offset,Value)-Pair #6

					// DirectSound_CDirectSoundVoiceSettings_SetMixBins+0x19 : shr eax, 1
	{ 0x19, 0xD1 }, // (Offset,Value)-Pair #7
	{ 0x1A, 0xE8 }, // (Offset,Value)-Pair #8

					// DirectSound_CDirectSoundVoiceSettings_SetMixBins+0x53 : cmp [ecx+24h], edx
	{ 0x53, 0x39 }, // (Offset,Value)-Pair #9
	{ 0x54, 0x51 }, // (Offset,Value)-Pair #10
	{ 0x55, 0x24 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundVoice::SetMixBins
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetMixBins, 4134, 17,

    XREF_DSSETMIXBINSB,
    XRefOne)

        // DirectSound_CDirectSoundVoice_SetMixBins+0x0D : call [CDirectSoundVoiceSettings::SetMixBins]
        XREF_ENTRY( 0x0D, XREF_DSSETMIXBINSC ), // (Offset,Value)-Pair #1

        // DirectSound_CDirectSoundVoice_SetMixBins+0x01 : mov esi, [esp+8]
        { 0x01, 0x8B }, // (Offset,Value)-Pair #2
        { 0x02, 0x74 }, // (Offset,Value)-Pair #3
        { 0x03, 0x24 }, // (Offset,Value)-Pair #4
        { 0x04, 0x08 }, // (Offset,Value)-Pair #5

        // DirectSound_CDirectSoundVoice_SetMixBins+0x05 : push [esp+0x0C]
        { 0x05, 0xFF }, // (Offset,Value)-Pair #6
        { 0x06, 0x74 }, // (Offset,Value)-Pair #7
        { 0x07, 0x24 }, // (Offset,Value)-Pair #8
        { 0x08, 0x0C }, // (Offset,Value)-Pair #9

        // DirectSound_CDirectSoundVoice_SetMixBins+0x09 : mov ecx, [esi+0x10]
        { 0x09, 0x8B }, // (Offset,Value)-Pair #10
        { 0x0A, 0x4E }, // (Offset,Value)-Pair #11
        { 0x0B, 0x10 }, // (Offset,Value)-Pair #12

        // DirectSound_CDirectSoundVoice_SetMixBins+0x11 : mov ecx, [esi+0x0C]
        { 0x11, 0x8B }, // (Offset,Value)-Pair #13
        { 0x12, 0x4E }, // (Offset,Value)-Pair #14
        { 0x13, 0x0C }, // (Offset,Value)-Pair #15

        // DirectSound_CDirectSoundVoice_SetMixBins+0x1A : retn 0x08
        { 0x1A, 0xC2 }, // (Offset,Value)-Pair #16
        { 0x1B, 0x08 }, // (Offset,Value)-Pair #17
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundBuffer::SetMixBins
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetMixBins, 4134, 17,

    XREF_DSSETMIXBINSA,
    XRefOne)

        // DirectSound_CDirectSoundBuffer_SetMixBins+0x32 : call [CDirectSoundVoice::SetMixBins]
        XREF_ENTRY( 0x32, XREF_DSSETMIXBINSB ), // (Offset,Value)-Pair #1

        // DirectSound_CDirectSoundBuffer_SetMixBins+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 }, // (Offset,Value)-Pair #2
        { 0x22, 0x05 }, // (Offset,Value)-Pair #3
        { 0x23, 0x40 }, // (Offset,Value)-Pair #4
        { 0x24, 0x00 }, // (Offset,Value)-Pair #5
        { 0x25, 0x80 }, // (Offset,Value)-Pair #6

        // DirectSound_CDirectSoundBuffer_SetMixBins+0x29 : push [esp+0x10]; push [esp+0x10]
        { 0x29, 0xFF }, // (Offset,Value)-Pair #7
        { 0x2A, 0x74 }, // (Offset,Value)-Pair #8
        { 0x2B, 0x24 }, // (Offset,Value)-Pair #9
        { 0x2C, 0x10 }, // (Offset,Value)-Pair #10
        { 0x2D, 0xFF }, // (Offset,Value)-Pair #11
        { 0x2E, 0x74 }, // (Offset,Value)-Pair #12
        { 0x2F, 0x24 }, // (Offset,Value)-Pair #13
        { 0x30, 0x10 }, // (Offset,Value)-Pair #14

        // DirectSound_CDirectSoundBuffer_SetMixBins+0x49 : pop edi
        { 0x49, 0x5F }, // (Offset,Value)-Pair #15

        // DirectSound_CDirectSoundBuffer_SetMixBins+0x4B : retn 0x08
        { 0x4B, 0xC2 }, // (Offset,Value)-Pair #16
        { 0x4C, 0x08 }, // (Offset,Value)-Pair #17
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetMixBins
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMixBins, 4134, 12,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetMixBins+0x15 : call [CDirectSoundBuffer::SetMixBins]
        XREF_ENTRY( 0x15, XREF_DSSETMIXBINSA ), // (Offset,Value)-Pair #1

        // IDirectSoundBuffer_SetMixBins+0x04 : push [esp+0x08]
        { 0x04, 0xFF }, // (Offset,Value)-Pair #2
        { 0x05, 0x74 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5

        // IDirectSoundBuffer_SetMixBins+0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 }, // (Offset,Value)-Pair #6
        { 0x0B, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x0C, 0xE4 }, // (Offset,Value)-Pair #8

        // IDirectSoundBuffer_SetMixBins+0x0F : sbb ecx, ecx
        { 0x0F, 0x1B }, // (Offset,Value)-Pair #9
        { 0x10, 0xC9 }, // (Offset,Value)-Pair #10

        // IDirectSoundBuffer_SetMixBins+0x11 : and ecx, eax
        { 0x11, 0x23 }, // (Offset,Value)-Pair #11
        { 0x12, 0xC8 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSound::SetPositionA
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_SetPositionA, 4134, 11,

    XREF_DSSETPOSITIONA,
    XRefZero)

        // DirectSound_CDirectSound_SetPositionA+0x2B : mov ecx, [ebp+0x08]
        { 0x2B, 0x8B }, // (Offset,Value)-Pair #1
        { 0x2C, 0x4D }, // (Offset,Value)-Pair #2
        { 0x2D, 0x08 }, // (Offset,Value)-Pair #3

        // DirectSound_CDirectSound_SetPositionA+0x3F : mov [edx+0x3C], edi
        { 0x3F, 0x89 }, // (Offset,Value)-Pair #4
        { 0x40, 0x7A }, // (Offset,Value)-Pair #5
        { 0x41, 0x3C }, // (Offset,Value)-Pair #6

        // DirectSound_CDirectSound_SetPositionA+0x4C : or word ptr [eax+0xA4], 0x01FF
        { 0x4C, 0x80 }, // (Offset,Value)-Pair #7
        { 0x4D, 0x88 }, // (Offset,Value)-Pair #8
        { 0x4E, 0xA4 }, // (Offset,Value)-Pair #9
        { 0x4F, 0x00 }, // (Offset,Value)-Pair #10
        { 0x52, 0xFF }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirectSound_CreateSoundBuffer
// ******************************************************************
OOVPA_XREF(IDirectSound_CreateSoundBuffer, 4134, 12,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSound_CreateSoundBuffer+0x1D : call [CDirectSound::CreateSoundBuffer]
        XREF_ENTRY( 0x1D, XREF_DSCREATESOUNDBUFFER ), // (Offset,Value)-Pair #1

        // IDirectSound_CreateSoundBuffer+0x04 : mov eax, [esp+8]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #2
        { 0x05, 0x44 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5

        // IDirectSound_CreateSoundBuffer+0x12 : add eax, 0xFFFFFFF8
        { 0x12, 0x83 }, // (Offset,Value)-Pair #6
        { 0x13, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x14, 0xF8 }, // (Offset,Value)-Pair #8

        // IDirectSound_CreateSoundBuffer+0x17 : sbb ecx, ecx
        { 0x17, 0x1B }, // (Offset,Value)-Pair #9
        { 0x18, 0xC9 }, // (Offset,Value)-Pair #10

        // IDirectSound_CreateSoundBuffer+0x21 : retn 0x10
        { 0x21, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x22, 0x10 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundVoice::SetFrequency
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetFrequency, 4134, 11,

    XREF_DSBUFFERSETFREQUENCYB,
    XRefZero)

        // DirectSound_CDirectSoundVoice_SetFrequency+0x0D : mov eax, [esi+0x10]
        { 0x0D, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0E, 0x46 }, // (Offset,Value)-Pair #2
        { 0x0F, 0x10 }, // (Offset,Value)-Pair #3

        // DirectSound_CDirectSoundVoice_SetFrequency+0x10 : mov eax, [eax+0x10]
        { 0x10, 0x8B }, // (Offset,Value)-Pair #4
        { 0x11, 0x40 }, // (Offset,Value)-Pair #5
        { 0x12, 0x10 }, // (Offset,Value)-Pair #6

        // DirectSound_CDirectSoundVoice_SetFrequency+0x14 : call [abs]; push eax; push esi
        { 0x14, 0xE8 }, // (Offset,Value)-Pair #7
        { 0x19, 0x50 }, // (Offset,Value)-Pair #8
        { 0x1A, 0x56 }, // (Offset,Value)-Pair #9

        // DirectSound_CDirectSoundVoice_SetFrequency+0x22 : retn 0x08
        { 0x21, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x22, 0x08 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundBuffer::SetFrequency
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetFrequency, 4134, 17,

    XREF_DSBUFFERSETFREQUENCYA,
    XRefOne)

        // DirectSound_CDirectSoundBuffer_SetFrequency+0x32 : call [CDirectSoundVoice::SetFrequency]
        XREF_ENTRY( 0x32, XREF_DSBUFFERSETFREQUENCYB ), // (Offset,Value)-Pair #1

        // DirectSound_CDirectSoundBuffer_SetFrequency+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 }, // (Offset,Value)-Pair #2
        { 0x22, 0x05 }, // (Offset,Value)-Pair #3
        { 0x23, 0x40 }, // (Offset,Value)-Pair #4
        { 0x24, 0x00 }, // (Offset,Value)-Pair #5
        { 0x25, 0x80 }, // (Offset,Value)-Pair #6

        // DirectSound_CDirectSoundBuffer_SetFrequency+0x29 : push [esp+0x10]; push [esp+0x10]
        { 0x29, 0xFF }, // (Offset,Value)-Pair #7
        { 0x2A, 0x74 }, // (Offset,Value)-Pair #8
        { 0x2B, 0x24 }, // (Offset,Value)-Pair #9
        { 0x2C, 0x10 }, // (Offset,Value)-Pair #10
        { 0x2D, 0xFF }, // (Offset,Value)-Pair #11
        { 0x2E, 0x74 }, // (Offset,Value)-Pair #12
        { 0x2F, 0x24 }, // (Offset,Value)-Pair #13
        { 0x30, 0x10 }, // (Offset,Value)-Pair #14

        // DirectSound_CDirectSoundBuffer_SetFrequency+0x49 : pop edi
        { 0x49, 0x5F }, // (Offset,Value)-Pair #15

        // DirectSound_CDirectSoundBuffer_SetFrequency+0x4B : retn 0x08
        { 0x4B, 0xC2 }, // (Offset,Value)-Pair #16
        { 0x4C, 0x08 }, // (Offset,Value)-Pair #17
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetFrequency
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetFrequency, 4134, 12,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetFrequency+0x15 : call [CDirectSound::SetFrequency]
        XREF_ENTRY( 0x15, XREF_DSBUFFERSETFREQUENCYA ), // (Offset,Value)-Pair #1

        // IDirectSoundBuffer_SetFrequency+0x04 : push [esp+0x08]
        { 0x04, 0xFF }, // (Offset,Value)-Pair #2
        { 0x05, 0x74 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5

        // IDirectSoundBuffer_SetFrequency0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 }, // (Offset,Value)-Pair #6
        { 0x0B, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x0C, 0xE4 }, // (Offset,Value)-Pair #8

        // IDirectSoundBuffer_SetFrequency+0x0F : sbb ecx, ecx
        { 0x0F, 0x1B }, // (Offset,Value)-Pair #9
        { 0x10, 0xC9 }, // (Offset,Value)-Pair #10

        // IDirectSoundBuffer_SetFrequency+0x11 : and ecx, eax
        { 0x11, 0x23 }, // (Offset,Value)-Pair #11
        { 0x12, 0xC8 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * CMcpxVoiceClient_SetVolume
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_SetVolume, 4134, 13,

    XREF_DSSTREAMSETVOLUME,
    XRefZero)

        // CMcpxVoiceClient_SetVolume+0x2A : lea eax, [ecx+ecx*2]
        { 0x2A, 0x8D }, // (Offset,Value)-Pair #1
        { 0x2B, 0x04 }, // (Offset,Value)-Pair #2
        { 0x2C, 0x49 }, // (Offset,Value)-Pair #3

        // CMcpxVoiceClient_SetVolume+0x45 : movzx edx, word ptr [ecx]
        { 0x45, 0x0F }, // (Offset,Value)-Pair #4
        { 0x46, 0xB7 }, // (Offset,Value)-Pair #5
        { 0x47, 0x11 }, // (Offset,Value)-Pair #6

        // CMcpxVoiceClient_SetVolume+0x6C : mov edx, [ebp+eax*4-0x14]
        { 0x6C, 0x8B }, // (Offset,Value)-Pair #7
        { 0x6D, 0x54 }, // (Offset,Value)-Pair #8
        { 0x6E, 0x85 }, // (Offset,Value)-Pair #9
        { 0x6F, 0xEC }, // (Offset,Value)-Pair #10

        // CMcpxVoiceClient_SetVolume+0x84 : inc eax; inc ecx, inc ecx
        { 0x84, 0x40 }, // (Offset,Value)-Pair #11
        { 0x85, 0x41 }, // (Offset,Value)-Pair #12
        { 0x86, 0x41 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundVoice_SetVolume
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetVolume, 4134, 11,

    XREF_DirectSound_CDirectSoundVoice_SetVolume,
    XRefOne)

        // DirectSound_CDirectSoundVoice_SetVolume+0x15 : call [CMcpxVoiceClient::SetVolume]
        XREF_ENTRY( 0x15, XREF_DSSTREAMSETVOLUME ),  // (Offset,Value)-Pair #1

        // DirectSound_CDirectSoundVoice_SetVolume+0x00 : mov ecx, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #2
        { 0x01, 0x4C }, // (Offset,Value)-Pair #3
        { 0x02, 0x24 }, // (Offset,Value)-Pair #4
        { 0x03, 0x04 }, // (Offset,Value)-Pair #5

        // DirectSound_CDirectSoundVoice_SetVolume+0x0B : sub edx, [eax+0x20]
        { 0x0B, 0x2B }, // (Offset,Value)-Pair #6
        { 0x0C, 0x50 }, // (Offset,Value)-Pair #7
        { 0x0D, 0x20 }, // (Offset,Value)-Pair #8

        // DirectSound_CDirectSoundVoice_SetVolume+0x11 : mov ecx, [ecx+0x0C]
        { 0x11, 0x8B }, // (Offset,Value)-Pair #9
        { 0x12, 0x49 }, // (Offset,Value)-Pair #10
        { 0x13, 0x0C }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundStream_SetVolume
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundStream_SetVolume, 4134, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_DirectSound_CDirectSoundVoice_SetVolume ),

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
// * IDirectSoundBuffer_LockA
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_LockA, 4134, 13,

    XREF_DSBUFFERLOCKA,
    XRefZero)

        // IDirectSoundBuffer_LockA+0x37 : test [ebp+0x24], 1
        { 0x37, 0xF6 }, // (Offset,Value)-Pair #1
        { 0x38, 0x45 }, // (Offset,Value)-Pair #2
        { 0x39, 0x24 }, // (Offset,Value)-Pair #3
        { 0x3A, 0x01 }, // (Offset,Value)-Pair #4

        // IDirectSoundBuffer_LockA+0x5C : mov eax, [eax+0xBC]
        { 0x5C, 0x8B }, // (Offset,Value)-Pair #5
        { 0x5D, 0x80 }, // (Offset,Value)-Pair #6
        { 0x5E, 0xBC }, // (Offset,Value)-Pair #7
        { 0x5F, 0x00 }, // (Offset,Value)-Pair #8

        // IDirectSoundBuffer_LockA+0xA4 : jnb +0x11; mov esi, [esi+0x1C]
        { 0xA2, 0x73 }, // (Offset,Value)-Pair #9
        { 0xA3, 0x11 }, // (Offset,Value)-Pair #10
        { 0xA4, 0x8B }, // (Offset,Value)-Pair #11
        { 0xA5, 0x76 }, // (Offset,Value)-Pair #12
        { 0xA6, 0x1C }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_SetBufferData
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_SetBufferData, 4134, 9,

    XREF_DSSETBUFFERDATA,
    XRefZero)

        // CMcpxBuffer_SetBufferData+0x1E : cmp eax, ebx
        { 0x1E, 0x3B }, // (Offset,Value)-Pair #1
        { 0x1F, 0xC3 }, // (Offset,Value)-Pair #2

        // CMcpxBuffer_SetBufferData+0x20 : jz +0x0C
        { 0x20, 0x74 }, // (Offset,Value)-Pair #3
        { 0x21, 0x0C }, // (Offset,Value)-Pair #4

        // CMcpxBuffer_SetBufferData+0x45 : mov [esi+0xB8], ecx
        { 0x45, 0x89 }, // (Offset,Value)-Pair #5
        { 0x46, 0x8E }, // (Offset,Value)-Pair #6
        { 0x47, 0xB8 }, // (Offset,Value)-Pair #7

        // CMcpxBuffer_SetBufferData+0x89 : retn 0x08
        { 0x89, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x8A, 0x08 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundBuffer_SetBufferData
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetBufferData, 4134, 10,

    XREF_DSSETBUFFERDATA2,
    XRefOne)

        // DirectSound_CDirectSoundBuffer_SetBufferData+0x7A : call [CMcpxBuffer::SetBufferData]
        XREF_ENTRY( 0x7A, XREF_DSSETBUFFERDATA ),  // (Offset,Value)-Pair #1

        // DirectSound_CDirectSoundBuffer_SetBufferData+0x40 : mov eax, 0x80004005
        { 0x40, 0xB8 }, // (Offset,Value)-Pair #2
        { 0x41, 0x05 }, // (Offset,Value)-Pair #3
        { 0x42, 0x40 }, // (Offset,Value)-Pair #4
        { 0x43, 0x00 }, // (Offset,Value)-Pair #5
        { 0x44, 0x80 }, // (Offset,Value)-Pair #6

        // DirectSound_CDirectSoundBuffer_SetBufferData+0x5F : jz +0x33
        { 0x5F, 0x74 }, // (Offset,Value)-Pair #7
        { 0x60, 0x33 }, // (Offset,Value)-Pair #8

        // DirectSound_CDirectSoundBuffer_SetBufferData+0xAB : retn 0x0C
        { 0xAB, 0xC2 }, // (Offset,Value)-Pair #9
        { 0xAC, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetBufferData
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetBufferData, 4134, 8,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetBufferData+0x19 : call [CDirectSoundBuffer::SetBufferData]
        XREF_ENTRY( 0x19, XREF_DSSETBUFFERDATA2 ),  // (Offset,Value)-Pair #1

        // IDirectSoundBuffer_SetBufferData+0x0E : add eax, 0xFFFFFFE4
        { 0x0E, 0x83 }, // (Offset,Value)-Pair #2
        { 0x0F, 0xC0 }, // (Offset,Value)-Pair #3
        { 0x10, 0xE4 }, // (Offset,Value)-Pair #4

        // IDirectSoundBuffer_SetBufferData+0x13 : sbb ecx, ecx
        { 0x13, 0x1B }, // (Offset,Value)-Pair #5
        { 0x14, 0xC9 }, // (Offset,Value)-Pair #6

        // IDirectSoundBuffer_SetBufferData+0x15 : and ecx, eax
        { 0x15, 0x23 }, // (Offset,Value)-Pair #7
        { 0x16, 0xC8 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_GetStatus
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_GetStatus, 4134, 11,

    XREF_DSBUFFERGETSTATUSA,
    XRefZero)

        // CMcpxBuffer_GetStatus+0x0A : mov esi, ecx
        { 0x0A, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0B, 0xF1 }, // (Offset,Value)-Pair #2

        // CMcpxBuffer_GetStatus+0x20 : jz +0x18
        { 0x20, 0x74 }, // (Offset,Value)-Pair #3
        { 0x21, 0x18 }, // (Offset,Value)-Pair #4

        // CMcpxBuffer_GetStatus+0x22 : test word ptr [esi+0x12], 0x8002
        { 0x22, 0x66 }, // (Offset,Value)-Pair #5
        { 0x23, 0xF7 }, // (Offset,Value)-Pair #6
        { 0x24, 0x46 }, // (Offset,Value)-Pair #7
        { 0x25, 0x12 }, // (Offset,Value)-Pair #8

        // CMcpxBuffer_GetStatus+0x2C : test word ptr [esi+0x12], 0x200
        { 0x2C, 0x66 }, // (Offset,Value)-Pair #9
        { 0x2D, 0xF7 }, // (Offset,Value)-Pair #10
        { 0x2E, 0x46 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundBuffer_GetStatus
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_GetStatus, 4134, 10,

	XREF_DSBUFFERGETSTATUSB,
	XRefOne)

	// DirectSound_CDirectSoundBuffer_GetStatus+0x35 : call [CMcpxBuffer::GetStatus]
	XREF_ENTRY(0x35, XREF_DSBUFFERGETSTATUSA),  // (Offset,Value)-Pair #1

												// DirectSound_CDirectSoundBuffer_GetStatus+0x21 : mov eax, 0x80004005
	{ 0x21, 0xB8 }, // (Offset,Value)-Pair #2
	{ 0x22, 0x05 }, // (Offset,Value)-Pair #3
	{ 0x23, 0x40 }, // (Offset,Value)-Pair #4
	{ 0x24, 0x00 }, // (Offset,Value)-Pair #5
	{ 0x25, 0x80 }, // (Offset,Value)-Pair #6

					// DirectSound_CDirectSoundBuffer_GetStatus+0x3D : jz +0x0B
	{ 0x3D, 0x74 }, // (Offset,Value)-Pair #7
	{ 0x3E, 0x0B }, // (Offset,Value)-Pair #8

					// DirectSound_CDirectSoundBuffer_GetStatus+0x4E : retn 0x08
	{ 0x4E, 0xC2 }, // (Offset,Value)-Pair #9
	{ 0x4F, 0x08 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_SetCurrentPosition
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_SetCurrentPosition, 4134, 9,

    XREF_SETCURRENTPOSITION,
    XRefZero)

        // CMcpxBuffer_SetCurrentPosition+0x11 : mov al, [esi+12]
        { 0x11, 0x8A }, // (Offset,Value)-Pair #1
        { 0x12, 0x46 }, // (Offset,Value)-Pair #2
        { 0x13, 0x12 }, // (Offset,Value)-Pair #3

        // CMcpxBuffer_SetCurrentPosition+0x5F : jb +0x10
        { 0x5F, 0x72 }, // (Offset,Value)-Pair #4
        { 0x60, 0x10 }, // (Offset,Value)-Pair #5

        // CMcpxBuffer_SetCurrentPosition+0x86 : movzx eax, byte ptr [esi+0x64]
        { 0x86, 0x0F }, // (Offset,Value)-Pair #6
        { 0x87, 0xB6 }, // (Offset,Value)-Pair #7
        { 0x88, 0x46 }, // (Offset,Value)-Pair #8
        { 0x89, 0x64 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundBuffer_SetCurrentPosition
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetCurrentPosition, 4134, 10,

    XREF_SETCURRENTPOSITION2,
    XRefOne)

        // DirectSound_CDirectSoundBuffer_SetCurrentPosition+0x35 : call [CMcpxBuffer::SetCurrentPosition]
        XREF_ENTRY( 0x35, XREF_SETCURRENTPOSITION ),  // (Offset,Value)-Pair #1

        // DirectSound_CDirectSoundBuffer_SetCurrentPosition+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 }, // (Offset,Value)-Pair #2
        { 0x22, 0x05 }, // (Offset,Value)-Pair #3
        { 0x23, 0x40 }, // (Offset,Value)-Pair #4
        { 0x24, 0x00 }, // (Offset,Value)-Pair #5
        { 0x25, 0x80 }, // (Offset,Value)-Pair #6

        // DirectSound_CDirectSoundBuffer_SetCurrentPosition+0x3D : jz +0x0B
        { 0x3D, 0x74 }, // (Offset,Value)-Pair #7
        { 0x3E, 0x0B }, // (Offset,Value)-Pair #8

        // DirectSound_CDirectSoundBuffer_SetCurrentPosition+0x3D : retn 0x08
        { 0x4E, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x4F, 0x08 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetCurrentPosition
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetCurrentPosition, 4134, 8,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetCurrentPosition+0x15 : call [CDirectSoundBuffer::SetCurrentPosition]
        XREF_ENTRY( 0x15, XREF_SETCURRENTPOSITION2 ),  // (Offset,Value)-Pair #1

        // IDirectSoundBuffer_SetCurrentPosition+0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 }, // (Offset,Value)-Pair #2
        { 0x0B, 0xC0 }, // (Offset,Value)-Pair #3
        { 0x0C, 0xE4 }, // (Offset,Value)-Pair #4

        // IDirectSoundBuffer_SetCurrentPosition+0x0F : sbb ecx, ecx
        { 0x0F, 0x1B }, // (Offset,Value)-Pair #5
        { 0x10, 0xC9 }, // (Offset,Value)-Pair #6

        // IDirectSoundBuffer_SetCurrentPosition+0x11 : and ecx, eax
        { 0x11, 0x23 }, // (Offset,Value)-Pair #7
        { 0x12, 0xC8 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_GetCurrentPosition
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_GetCurrentPosition, 4134, 11,

    XREF_GETCURRENTPOSITION,
    XRefZero)

        // CMcpxBuffer_GetCurrentPosition+0x0E : lea ecx, [ebp-0x08]
        { 0x0E, 0x8D }, // (Offset,Value)-Pair #1
        { 0x0F, 0x4D }, // (Offset,Value)-Pair #2
        { 0x10, 0xF8 }, // (Offset,Value)-Pair #3

        // CMcpxBuffer_GetCurrentPosition+0x31 : jz +0x02
        { 0x31, 0x74 }, // (Offset,Value)-Pair #4
        { 0x32, 0x02 }, // (Offset,Value)-Pair #5

        // CMcpxBuffer_GetCurrentPosition+0xA0 : div dword ptr [ecx+0xBC]
        { 0xA0, 0xF7 }, // (Offset,Value)-Pair #6
        { 0xA1, 0xB1 }, // (Offset,Value)-Pair #7
        { 0xA2, 0xBC }, // (Offset,Value)-Pair #8

        // CMcpxBuffer_GetCurrentPosition+0xD6 : retn 0x08
        { 0xD6, 0xC2 }, // (Offset,Value)-Pair #9
        { 0xD7, 0x08 }, // (Offset,Value)-Pair #10
        { 0xD8, 0x00 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundBuffer_GetCurrentPosition
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_GetCurrentPosition, 4134, 10,

    XREF_GETCURRENTPOSITION2,
    XRefOne)

        // DirectSound_CDirectSoundBuffer_GetCurrentPosition+0x39 : call [CMcpxBuffer::GetCurrentPosition]
        XREF_ENTRY( 0x39, XREF_GETCURRENTPOSITION ),  // (Offset,Value)-Pair #1

        // DirectSound_CDirectSoundBuffer_GetCurrentPosition+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 }, // (Offset,Value)-Pair #2
        { 0x22, 0x05 }, // (Offset,Value)-Pair #3
        { 0x23, 0x40 }, // (Offset,Value)-Pair #4
        { 0x24, 0x00 }, // (Offset,Value)-Pair #5
        { 0x25, 0x80 }, // (Offset,Value)-Pair #6

        // DirectSound_CDirectSoundBuffer_GetCurrentPosition+0x41 : jz +0x0B
        { 0x41, 0x74 }, // (Offset,Value)-Pair #7
        { 0x42, 0x0B }, // (Offset,Value)-Pair #8

        // DirectSound_CDirectSoundBuffer_GetCurrentPosition+0x3D : retn 0x08
        { 0x52, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x53, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundBuffer::Play
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_Play, 4134, 11,

	XREF_DSOUNDPLAY,
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
// * DirectSound_CDirectSound::SetDistanceFactorA
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_SetDistanceFactorA, 4134, 11,

    XREF_SETDISTANCEFACTORA,
    XRefZero)

        // DirectSound_CDirectSound_SetDistanceFactorA+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x22, 0x05 }, // (Offset,Value)-Pair #2
        { 0x23, 0x40 }, // (Offset,Value)-Pair #3
        { 0x24, 0x00 }, // (Offset,Value)-Pair #4
        { 0x25, 0x80 }, // (Offset,Value)-Pair #5

        // DirectSound_CDirectSound_SetDistanceFactorA+0x39 : or dword ptr[eax+0xA4], 0x60
        { 0x39, 0x83 }, // (Offset,Value)-Pair #6
        { 0x3A, 0x88 }, // (Offset,Value)-Pair #7
        { 0x3B, 0xA4 }, // (Offset,Value)-Pair #8
        { 0x3F, 0x60 }, // (Offset,Value)-Pair #9

        // DirectSound_CDirectSound_SetDistanceFactorA+0x4F : jz +0x0B
        { 0x4F, 0x74 }, // (Offset,Value)-Pair #10
        { 0x50, 0x0B }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetDistanceFactor
// ******************************************************************
OOVPA_XREF(IDirectSound_SetDistanceFactor, 4134, 11,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSound_SetDistanceFactor+0x1D : call [CDirectSound::SetDistanceFactor]
        XREF_ENTRY( 0x1D, XREF_SETDISTANCEFACTORA ),  // (Offset,Value)-Pair #1

        // IDirectSound_SetDistanceFactor+0x04 : fld [esp+0x0C]
        { 0x04, 0xD9 }, // (Offset,Value)-Pair #2
        { 0x05, 0x44 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x0C }, // (Offset,Value)-Pair #5

        // IDirectSound_SetDistanceFactor+0x0C : push ecx
        { 0x0C, 0x51 }, // (Offset,Value)-Pair #6

        // IDirectSound_SetDistanceFactor+0x12 : add eax, 0xFFFFFFF8
        { 0x12, 0x83 }, // (Offset,Value)-Pair #7
        { 0x13, 0xC0 }, // (Offset,Value)-Pair #8
        { 0x14, 0xF8 }, // (Offset,Value)-Pair #9

        // IDirectSound_SetDistanceFactor+0x21 : retn 0x0C
        { 0x21, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x22, 0x0C }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSound::SetRolloffFactor
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_SetRolloffFactor, 4134, 11,

    XREF_SETROLLOFFFACTORA,
    XRefZero)

        // DirectSound_CDirectSound_SetRolloffFactor+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x22, 0x05 }, // (Offset,Value)-Pair #2
        { 0x23, 0x40 }, // (Offset,Value)-Pair #3
        { 0x24, 0x00 }, // (Offset,Value)-Pair #4
        { 0x25, 0x80 }, // (Offset,Value)-Pair #5

        // DirectSound_CDirectSound_SetRolloffFactor+0x39 : or dword ptr[eax+0xA4], 0x04
        { 0x39, 0x83 }, // (Offset,Value)-Pair #6
        { 0x3A, 0x88 }, // (Offset,Value)-Pair #7
        { 0x3B, 0xA4 }, // (Offset,Value)-Pair #8
        { 0x3F, 0x04 }, // (Offset,Value)-Pair #9

        // DirectSound_CDirectSound_SetRolloffFactor+0x4F : jz +0x0B
        { 0x4F, 0x74 }, // (Offset,Value)-Pair #10
        { 0x50, 0x0B }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetRolloffFactor
// ******************************************************************
OOVPA_XREF(IDirectSound_SetRolloffFactor, 4134, 11,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSound_SetRolloffFactor+0x1D : call [CDirectSound::SetRolloffFactor]
        XREF_ENTRY( 0x1D, XREF_SETROLLOFFFACTORA ),  // (Offset,Value)-Pair #1

        // IDirectSound_SetRolloffFactor+0x04 : fld [esp+0x0C]
        { 0x04, 0xD9 }, // (Offset,Value)-Pair #2
        { 0x05, 0x44 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x0C }, // (Offset,Value)-Pair #5

        // IDirectSound_SetRolloffFactor+0x0C : push ecx
        { 0x0C, 0x51 }, // (Offset,Value)-Pair #6

        // IDirectSound_SetRolloffFactor+0x12 : add eax, 0xFFFFFFF8
        { 0x12, 0x83 }, // (Offset,Value)-Pair #7
        { 0x13, 0xC0 }, // (Offset,Value)-Pair #8
        { 0x14, 0xF8 }, // (Offset,Value)-Pair #9

        // IDirectSound_SetRolloffFactor+0x21 : retn 0x0C
        { 0x21, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x22, 0x0C }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSound::SetDopplerFactor
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_SetDopplerFactor, 4134, 14,

    XREF_SETDOPPLERFACTOR,
    XRefZero)

        // DirectSound_CDirectSound_SetDopplerFactor+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x22, 0x05 }, // (Offset,Value)-Pair #2
        { 0x23, 0x40 }, // (Offset,Value)-Pair #3
        { 0x24, 0x00 }, // (Offset,Value)-Pair #4
        { 0x25, 0x80 }, // (Offset,Value)-Pair #5

        // DirectSound_CDirectSound_SetDopplerFactor+0x33 : mov [eax+0x70], edx
        { 0x33, 0x89 }, // (Offset,Value)-Pair #6
        { 0x34, 0x50 }, // (Offset,Value)-Pair #7
        { 0x35, 0x70 }, // (Offset,Value)-Pair #8

        // DirectSound_CDirectSound_SetDopplerFactor+0x39 : or dword ptr[eax+0xA4], 0x40
        { 0x39, 0x83 }, // (Offset,Value)-Pair #9
        { 0x3A, 0x88 }, // (Offset,Value)-Pair #10
        { 0x3B, 0xA4 }, // (Offset,Value)-Pair #11
        { 0x3F, 0x40 }, // (Offset,Value)-Pair #12

        // DirectSound_CDirectSound_SetDopplerFactor+0x4F : jz +0x0B
        { 0x4F, 0x74 }, // (Offset,Value)-Pair #13
        { 0x50, 0x0B }, // (Offset,Value)-Pair #14
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetDopplerFactor
// ******************************************************************
OOVPA_XREF(IDirectSound_SetDopplerFactor, 4134, 11,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSound_SetDopplerFactor+0x1D : call [CDirectSound::SetDopplerFactor]
        XREF_ENTRY( 0x1D, XREF_SETDOPPLERFACTOR),  // (Offset,Value)-Pair #1

        // IDirectSound_SetDopplerFactor+0x04 : fld [esp+0x0C]
        { 0x04, 0xD9 }, // (Offset,Value)-Pair #2
        { 0x05, 0x44 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x0C }, // (Offset,Value)-Pair #5

        // IDirectSound_SetDopplerFactor+0x0C : push ecx
        { 0x0C, 0x51 }, // (Offset,Value)-Pair #6

        // IDirectSound_SetDopplerFactor+0x12 : add eax, 0xFFFFFFF8
        { 0x12, 0x83 }, // (Offset,Value)-Pair #7
        { 0x13, 0xC0 }, // (Offset,Value)-Pair #8
        { 0x14, 0xF8 }, // (Offset,Value)-Pair #9

        // IDirectSound_SetDopplerFactor+0x21 : retn 0x0C
        { 0x21, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x22, 0x0C }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSound::CommitDeferredSettings
// ******************************************************************
OOVPA_NO_XREF(DirectSound_CDirectSound_CommitDeferredSettings, 4134, 11)

        // DirectSound_CDirectSound_CommitDeferredSettings+0x10 : movzx eax, al
        { 0x10, 0x0F }, // (Offset,Value)-Pair #1
        { 0x11, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x12, 0xC0 }, // (Offset,Value)-Pair #3

        // DirectSound_CDirectSound_CommitDeferredSettings+0x27 : mov eax, 0x80004005
        { 0x27, 0xB8 }, // (Offset,Value)-Pair #4
        { 0x28, 0x05 }, // (Offset,Value)-Pair #5
        { 0x29, 0x40 }, // (Offset,Value)-Pair #6
        { 0x2B, 0x80 }, // (Offset,Value)-Pair #7

        // DirectSound_CDirectSound_CommitDeferredSettings+0x5C : and [eax+0xA4], esi
        { 0x5C, 0x21 }, // (Offset,Value)-Pair #8
        { 0x5D, 0xB0 }, // (Offset,Value)-Pair #9
        { 0x5E, 0xA4 }, // (Offset,Value)-Pair #10

        // DirectSound_CDirectSound_CommitDeferredSettings+0x78 : leave
        { 0x78, 0xC9 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundVoice::SetMaxDistance
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetMaxDistance, 4134, 9,

    XREF_DSVOICESETMAXDISTANCE,
    XRefZero)

        // DirectSound_CDirectSoundVoice_SetMaxDistance+0x0D : mov edx, [esp+arg_4]
        { 0x0D, 0x8B },
        { 0x0E, 0x54 },
        { 0x0F, 0x24 },
        { 0x10, 0x08 },

        // DirectSound_CDirectSoundVoice_SetMaxDistance+0x11 : mov [eax+38h], edx
        { 0x11, 0x89 },
        { 0x12, 0x50 },
        { 0x13, 0x38 },

        // DirectSound_CDirectSoundVoice_SetMaxDistance+0x30 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 }
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundBuffer::SetMaxDistance
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetMaxDistance, 4134, 10,

    XREF_DSBUFFERSETMAXDISTANCE,
    XRefOne)

        // DirectSound_CDirectSoundBuffer_SetMaxDistance+0x39 : call [CDirectSoundVoice::SetMaxDistance]
        XREF_ENTRY( 0x3A, XREF_DSVOICESETMAXDISTANCE ),

        // DirectSound_CDirectSoundBuffer_SetMaxDistance+0x21 : mov eax, 80004005h
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // DirectSound_CDirectSoundBuffer_SetMaxDistance+0x32 : fstp    [esp+0Ch+var_C]
        { 0x32, 0xD9 },
        { 0x33, 0x1C },
        { 0x34, 0x24 },

        // DirectSound_CDirectSoundBuffer_SetMaxDistance+0x53 : retn 0Ch
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
// * DirectSound_CDirectSoundStream_SetMaxDistance
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundStream_SetMaxDistance, 4134, 9,

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
// * DirectSound_CDirectSoundVoice::SetMinDistance
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetMinDistance, 4134, 9,

    XREF_DSVOICESETMINDISTANCE,
    XRefZero)

        // DirectSound_CDirectSoundVoice_SetMinDistance+0x0D : mov edx, [esp+arg_4]
        { 0x0D, 0x8B },
        { 0x0E, 0x54 },
        { 0x0F, 0x24 },
        { 0x10, 0x08 },

        // DirectSound_CDirectSoundVoice_SetMinDistance+0x11 : mov [eax+34h], edx
        { 0x11, 0x89 },
        { 0x12, 0x50 },
        { 0x13, 0x34 },

        // DirectSound_CDirectSoundVoice_SetMinDistance+0x30 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 }
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundBuffer::SetMinDistance
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetMinDistance, 4134, 10,

    XREF_DSBUFFERSETMINDISTANCE,
    XRefOne)

        // DirectSound_CDirectSoundBuffer_SetMinDistance+0x39 : call [CDirectSoundVoice::SetMinDistance]
        XREF_ENTRY( 0x3A, XREF_DSVOICESETMINDISTANCE ),

        // DirectSound_CDirectSoundBuffer_SetMinDistance+0x21 : mov eax, 80004005h
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // DirectSound_CDirectSoundBuffer_SetMinDistance+0x32 : fstp    [esp+0Ch+var_C]
        { 0x32, 0xD9 },
        { 0x33, 0x1C },
        { 0x34, 0x24 },

        // DirectSound_CDirectSoundBuffer_SetMinDistance+0x53 : retn 0Ch
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
// * DirectSound_CDirectSoundStream_SetMinDistance
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundStream_SetMinDistance, 4134, 9,

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
// * DirectSound_CDirectSoundVoice::SetRolloffFactor
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetRolloffFactor, 4134, 9,

    XREF_DSVOICESETROLLOFFFACTOR,
    XRefZero)

        // DirectSound_CDirectSoundVoice_SetRolloffFactor+0x0D : mov edx, [esp+arg_4]
        { 0x0D, 0x8B },
        { 0x0E, 0x54 },
        { 0x0F, 0x24 },
        { 0x10, 0x08 },

        // DirectSound_CDirectSoundVoice_SetRolloffFactor+0x11 : mov [eax+44h], edx
        { 0x11, 0x89 },
        { 0x12, 0x50 },
        { 0x13, 0x44 },

        // DirectSound_CDirectSoundVoice_SetRolloffFactor+0x30 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 }
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundBuffer::SetRolloffFactor
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetRolloffFactor, 4134, 10,

    XREF_DSBUFFERSETROLLOFFFACTOR,
    XRefOne)

        // DirectSound_CDirectSoundBuffer_SetRolloffFactor+0x39 : call [CDirectSoundVoice::SetRolloffFactor]
        XREF_ENTRY( 0x3A, XREF_DSVOICESETROLLOFFFACTOR ),

        // DirectSound_CDirectSoundBuffer_SetRolloffFactor+0x21 : mov eax, 80004005h
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // DirectSound_CDirectSoundBuffer_SetRolloffFactor+0x32 : fstp    [esp+0Ch+var_C]
        { 0x32, 0xD9 },
        { 0x33, 0x1C },
        { 0x34, 0x24 },

        // DirectSound_CDirectSoundBuffer_SetRolloffFactor+0x53 : retn 0Ch
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
// * DirectSound_CDirectSoundStream_SetRolloffFactor
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundStream_SetRolloffFactor, 4134, 9,

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
// * DirectSound_CDirectSoundVoice::SetDistanceFactor
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetDistanceFactor, 4134, 9,

    XREF_DSVOICESETDISTANCEFACTOR,
    XRefZero)

        // DirectSound_CDirectSoundVoice_SetDistanceFactor+0x0D : mov edx, [esp+arg_4]
        { 0x0D, 0x8B },
        { 0x0E, 0x54 },
        { 0x0F, 0x24 },
        { 0x10, 0x08 },

        // DirectSound_CDirectSoundVoice_SetDistanceFactor+0x11 : mov [eax+40h], edx
        { 0x11, 0x89 },
        { 0x12, 0x50 },
        { 0x13, 0x40 },

        // DirectSound_CDirectSoundVoice_SetDistanceFactor+0x30 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 }
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundBuffer::SetDistanceFactor
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetDistanceFactor, 4134, 10,

    XREF_DSBUFFERSETDISTANCEFACTOR,
    XRefOne)

        // DirectSound_CDirectSoundBuffer_SetDistanceFactor+0x39 : call [CDirectSoundVoice::SetDistanceFactor]
        XREF_ENTRY( 0x3A, XREF_DSVOICESETDISTANCEFACTOR ),

        // DirectSound_CDirectSoundBuffer_SetDistanceFactor+0x21 : mov eax, 80004005h
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // DirectSound_CDirectSoundBuffer_SetDistanceFactor+0x32 : fstp    [esp+0Ch+var_C]
        { 0x32, 0xD9 },
        { 0x33, 0x1C },
        { 0x34, 0x24 },

        // DirectSound_CDirectSoundBuffer_SetDistanceFactor+0x53 : retn 0Ch
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
// * DirectSound_CDirectSoundVoice::SetConeAngles
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetConeAngles, 4134, 9,

    XREF_DSVOICESETCONEANGLES,
    XRefZero)

        // DirectSound_CDirectSoundVoice_SetConeAngles+0x0D : mov edx, [esp+arg_4]
        { 0x0D, 0x8B },
        { 0x0E, 0x54 },
        { 0x0F, 0x24 },
        { 0x10, 0x08 },

        // DirectSound_CDirectSoundVoice_SetConeAngles+0x11 : mov [ecx+1Ch], edx
        { 0x11, 0x89 },
        { 0x12, 0x51 },
        { 0x13, 0x1C },

        // DirectSound_CDirectSoundVoice_SetConeAngles+0x40 : retn 10h
        { 0x41, 0x10 },
        { 0x42, 0x00 }
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundBuffer::SetConeAngles
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetConeAngles, 4134, 11,

    XREF_DSBUFFERSETCONEANGLES,
    XRefOne)

        // DirectSound_CDirectSoundBuffer_SetConeAngles+0x39 : call [CDirectSoundVoice::SetConeAngles]
        XREF_ENTRY( 0x3A, XREF_DSVOICESETCONEANGLES ),

        // DirectSound_CDirectSoundBuffer_SetConeAngles+0x21 : mov eax, 80004005h
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // DirectSound_CDirectSoundBuffer_SetConeAngles+0x31 : push [esp+0Ch+arg_4]
        { 0x31, 0xFF },
        { 0x32, 0x74 },
        { 0x33, 0x24 },
        { 0x34, 0x18 },

        // DirectSound_CDirectSoundBuffer_SetConeAngles+0x53 : retn 10h
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
// * DirectSound_CDirectSoundVoice::SetConeOrientation
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetConeOrientation, 4134, 8,

    XREF_DSVOICESETCONEORIENTATION,
    XRefZero)

        // DirectSound_CDirectSoundVoice_SetConeOrientation+0x0F : mov edx, [ebp+arg_4]
        { 0x0F, 0x8B },
        { 0x10, 0x55 },
        { 0x11, 0x0C },

        // DirectSound_CDirectSoundVoice_SetConeOrientation+0x12 : mov [ecx+24h], edx
        { 0x12, 0x89 },
        { 0x13, 0x51 },
        { 0x14, 0x24 },

        // DirectSound_CDirectSoundVoice_SetConeOrientation+0x4F : retn 14h
        { 0x50, 0x14 },
        { 0x51, 0x00 }
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundBuffer::SetConeOrientation
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetConeOrientation, 4134, 9,

    XREF_DSBUFFERSETCONEORIENTATION,
    XRefOne)

        // DirectSound_CDirectSoundBuffer_SetConeOrientation+0x49 : call [CDirectSoundVoice::SetConeOrientation]
        XREF_ENTRY( 0x4A, XREF_DSVOICESETCONEORIENTATION ),

        // DirectSound_CDirectSoundBuffer_SetConeOrientation+0x2B : fld [ebp+arg_C]
        { 0x2B, 0xD9 },
        { 0x2C, 0x45 },
        { 0x2D, 0x14 },

        // DirectSound_CDirectSoundBuffer_SetConeOrientation+0x43 : fstp [esp+18h+var_18]
        { 0x43, 0xD9 },
        { 0x44, 0x1C },
        { 0x45, 0x24 },

        // DirectSound_CDirectSoundBuffer_SetConeOrientation+0x64 : retn 14h
        { 0x65, 0x14 },
        { 0x66, 0x00 }
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetConeOrientation
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetConeOrientation, 4134, 9,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetConeOrientation+0x2C : call [CDirectSoundBuffer::SetConeOrientation]
        XREF_ENTRY( 0x2D, XREF_DSBUFFERSETCONEORIENTATION ),

        // IDirectSoundBuffer_SetConeOrientation+0x06 : fld [ebp+arg_C]
        { 0x06, 0xD9 },
        { 0x07, 0x45 },
        { 0x08, 0x14 },

        // IDirectSoundBuffer_SetConeOrientation+0x28 : fstp [esp+10h+var_10]
        { 0x28, 0xD9 },
        { 0x29, 0x1C },
        { 0x2A, 0x24 },

        // IDirectSoundBuffer_SetConeOrientation+0x32 : retn 14h
        { 0x33, 0x14 },
        { 0x34, 0x00 }
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundStream_SetConeOrientation
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundStream_SetConeOrientation, 4134, 9,

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
// * DirectSound_CDirectSoundVoice::SetConeOutsideVolume
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetConeOutsideVolume, 4134, 9,

    XREF_DSVOICESETCONEOUTSIDEVOLUME,
    XRefZero)

        // DirectSound_CDirectSoundVoice_SetConeOutsideVolume+0x0D : mov edx, [esp+arg_4]
        { 0x0D, 0x8B },
        { 0x0E, 0x54 },
        { 0x0F, 0x24 },
        { 0x10, 0x08 },

        // DirectSound_CDirectSoundVoice_SetConeOutsideVolume+0x11 : mov [eax+30h], edx
        { 0x11, 0x89 },
        { 0x12, 0x50 },
        { 0x13, 0x30 },

        // DirectSound_CDirectSoundVoice_SetConeOutsideVolume+0x30 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 }
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundBuffer::SetConeOutsideVolume
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetConeOutsideVolume, 4134, 11,

    XREF_DSBUFFERSETCONEOUTSIDEVOLUME,
    XRefOne)

        // DirectSound_CDirectSoundBuffer_SetConeOutsideVolume+0x35 : call [CDirectSoundVoice::SetConeOutsideVolume]
        XREF_ENTRY( 0x36, XREF_DSVOICESETCONEOUTSIDEVOLUME ),

        // DirectSound_CDirectSoundBuffer_SetConeOutsideVolume+0x21 : mov eax, 80004005h
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // DirectSound_CDirectSoundBuffer_SetConeOutsideVolume+0x31 : push [esp+0Ch+arg_0]
        { 0x31, 0xFF },
        { 0x32, 0x74 },
        { 0x33, 0x24 },
        { 0x34, 0x14 },

        // DirectSound_CDirectSoundBuffer_SetConeOutsideVolume+0x4F : retn 0Ch
        { 0x50, 0x0C },
        { 0x51, 0x00 }
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetConeOutsideVolume
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetConeOutsideVolume, 4134, 9,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetConeOutsideVolume+0x18 : call [CDirectSoundBuffer::SetConeOutsideVolume]
        XREF_ENTRY( 0x19, XREF_DSBUFFERSETCONEOUTSIDEVOLUME ),

        // IDirectSoundBuffer_SetConeOutsideVolume+0x0A : push [esp+4+arg_4]
        { 0x0A, 0xFF },
        { 0x0B, 0x74 },
        { 0x0C, 0x24 },
        { 0x0D, 0x0C },

        // IDirectSoundBuffer_SetConeOutsideVolume+0x15 : and ecx, eax
        { 0x15, 0x23 },
        { 0x16, 0xC8 },

        // IDirectSoundBuffer_SetConeOutsideVolume+0x1D : retn 0Ch
        { 0x1E, 0x0C },
        { 0x1F, 0x00 }
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundStream_SetConeOutsideVolume
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundStream_SetConeOutsideVolume, 4134, 9,

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
// * DirectSound_CDirectSoundVoice::SetPosition
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetPosition, 4134, 8,

    XREF_DSVOICESETPOSITION,
    XRefZero)

        // DirectSound_CDirectSoundVoice_SetPosition+0x0F : mov edx, [ebp+arg_4]
        { 0x0F, 0x8B },
        { 0x10, 0x55 },
        { 0x11, 0x0C },

        // DirectSound_CDirectSoundVoice_SetPosition+0x12 : mov [ecx+04h], edx
        { 0x12, 0x89 },
        { 0x13, 0x51 },
        { 0x14, 0x04 },

        // DirectSound_CDirectSoundVoice_SetPosition+0x4F : retn 14h
        { 0x4F, 0xC2 },
        { 0x50, 0x14 }
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundBuffer::SetPosition
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetPosition, 4134, 9,

    XREF_DSBUFFERSETPOSITION,
    XRefOne)

        // DirectSound_CDirectSoundBuffer_SetPosition+0x49 : call [CDirectSoundVoice::SetPosition]
        XREF_ENTRY( 0x4A, XREF_DSVOICESETPOSITION ),

        // DirectSound_CDirectSoundBuffer_SetPosition+0x2B : fld [ebp+arg_C]
        { 0x2B, 0xD9 },
        { 0x2C, 0x45 },
        { 0x2D, 0x14 },

        // DirectSound_CDirectSoundBuffer_SetPosition+0x43 : fstp [esp+18h+var_18]
        { 0x43, 0xD9 },
        { 0x44, 0x1C },
        { 0x45, 0x24 },

        // DirectSound_CDirectSoundBuffer_SetPosition+0x64 : retn 14h
        { 0x65, 0x14 },
        { 0x66, 0x00 }
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundStream_SetPosition
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundStream_SetPosition, 4134, 9,

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
// * DirectSound_CDirectSoundVoice::SetVelocity
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetVelocity, 4134, 8,

    XREF_DSVOICESETVELOCITY,
    XRefZero)

        // DirectSound_CDirectSoundVoice_SetVelocity+0x0F : mov edx, [ebp+arg_4]
        { 0x0F, 0x8B },
        { 0x10, 0x55 },
        { 0x11, 0x0C },

        // DirectSound_CDirectSoundVoice_SetVelocity+0x12 : mov [ecx+10h], edx
        { 0x12, 0x89 },
        { 0x13, 0x51 },
        { 0x14, 0x10 },

        // DirectSound_CDirectSoundVoice_SetVelocity+0x4F : retn 14h
        { 0x50, 0x14 },
        { 0x51, 0x00 }
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundBuffer::SetVelocity
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetVelocity, 4134, 9,

    XREF_DSBUFFERSETVELOCITY,
    XRefOne)

        // DirectSound_CDirectSoundBuffer_SetVelocity+0x49 : call [CDirectSoundVoice::SetVelocity]
        XREF_ENTRY( 0x4A, XREF_DSVOICESETVELOCITY ),

        // DirectSound_CDirectSoundBuffer_SetVelocity+0x2B : fld [ebp+arg_C]
        { 0x2B, 0xD9 },
        { 0x2C, 0x45 },
        { 0x2D, 0x14 },

        // DirectSound_CDirectSoundBuffer_SetVelocity+0x43 : fstp [esp+18h+var_18]
        { 0x43, 0xD9 },
        { 0x44, 0x1C },
        { 0x45, 0x24 },

        // DirectSound_CDirectSoundBuffer_SetVelocity+0x64 : retn 14h
        { 0x65, 0x14 },
        { 0x66, 0x00 }
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundStream_SetVelocity
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundStream_SetVelocity, 4134, 9,

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
// * DirectSound_CDirectSoundVoice::SetDopplerFactor
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetDopplerFactor, 4134, 9,

    XREF_DSVOICESETDOPPLERFACTOR,
    XRefZero)

        // DirectSound_CDirectSoundVoice_SetDopplerFactor+0x0D : mov edx, [esp+arg_4]
        { 0x0D, 0x8B },
        { 0x0E, 0x54 },
        { 0x0F, 0x24 },
        { 0x10, 0x08 },

        // DirectSound_CDirectSoundVoice_SetDopplerFactor+0x12 : mov [eax+48h], edx
        { 0x11, 0x89 },
        { 0x12, 0x50 },
        { 0x13, 0x48 },

        // DirectSound_CDirectSoundVoice_SetDopplerFactor+0x31 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 }
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundBuffer::SetDopplerFactor
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetDopplerFactor, 4134, 10,

    XREF_DSBUFFERSETDOPPLERFACTOR,
    XRefOne)

        // DirectSound_CDirectSoundBuffer_SetDopplerFactor+0x39 : call [CDirectSoundVoice::SetDopplerFactor]
        XREF_ENTRY( 0x3A, XREF_DSVOICESETDOPPLERFACTOR ),

        // DirectSound_CDirectSoundBuffer_SetDopplerFactor+0x2D : push    [esp+4+arg_8]
        { 0x2D, 0xFF },
        { 0x2E, 0x74 },
        { 0x2F, 0x24 },
        { 0x30, 0x14 },

        // DirectSound_CDirectSoundBuffer_SetDopplerFactor+0x32 : fstp [esp+0Ch+var_C]
        { 0x32, 0xD9 },
        { 0x33, 0x1C },
        { 0x34, 0x24 },

        // DirectSound_CDirectSoundBuffer_SetDopplerFactor+0x53 : retn 0Ch
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
// * DirectSound_CDirectSoundVoice::SetI3DL2Source
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetI3DL2Source, 4134, 10,

    XREF_DSVOICESETI3DL2SOURCE,
    XRefZero)

        // DirectSound_CDirectSoundVoice_SetI3DL2Source+0x12 : mov esi, [ecx]
        { 0x12, 0x8B },
        { 0x13, 0x31 },

        // DirectSound_CDirectSoundVoice_SetI3DL2Source+0x14 : mov [edx+4Ch], esi
        { 0x14, 0x89 },
        { 0x15, 0x72 },
        { 0x16, 0x4C },

        // DirectSound_CDirectSoundVoice_SetI3DL2Source+0x44 : fld dword ptr [ecx+10h]
        { 0x44, 0xD9 },
        { 0x45, 0x41 },
        { 0x46, 0x10 },

        // DirectSound_CDirectSoundVoice_SetI3DL2Source+0xAC : retn 0Ch
        { 0xAD, 0x0C },
        { 0xAE, 0x00 }
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundBuffer::SetI3DL2Source
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetI3DL2Source, 4134, 11,

    XREF_DSBUFFERSETI3DL2SOURCE,
    XRefOne)

        // DirectSound_CDirectSoundBuffer_SetI3DL2Source+0x35 : call [CDirectSoundVoice::SetI3DL2Source]
        XREF_ENTRY( 0x36, XREF_DSVOICESETI3DL2SOURCE ),

        // DirectSound_CDirectSoundBuffer_SetI3DL2Source+0x21 : mov eax, 80004005h
        { 0x22, 0x05 },
        { 0x23, 0x40 },
        { 0x24, 0x00 },
        { 0x25, 0x80 },

        // DirectSound_CDirectSoundBuffer_SetI3DL2Source+0x31 : push [esp+0Ch+arg_0]
        { 0x31, 0xFF },
        { 0x32, 0x74 },
        { 0x33, 0x24 },
        { 0x34, 0x14 },

        // DirectSound_CDirectSoundBuffer_SetI3DL2Source+0x4F : retn 0Ch
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
// * DirectSound_CDirectSoundStream_SetI3DL2Source
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundStream_SetI3DL2Source, 4134, 9,

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

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetAllParameters
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetAllParameters, 4134, 8,

    XREF_DirectSound_CDirectSoundVoice_SetAllParameters,
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
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetAllParameters, 4134, 8,

    XREF_DirectSound_CDirectSoundBuffer_SetAllParameters,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_DirectSound_CDirectSoundVoice_SetAllParameters ),

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

        XREF_ENTRY( 0x19, XREF_DirectSound_CDirectSoundBuffer_SetAllParameters ),

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
// * DirectSound_CDirectSoundStream_SetAllParameters
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundStream_SetAllParameters, 4134, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x3A, XREF_DirectSound_CDirectSoundVoice_SetAllParameters ),

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
        XREF_ENTRY( 0x15, XREF_DSBUFFERGETSTATUSB ),  // (Offset,Value)-Pair #1

        // IDirectSoundBuffer_GetStatus+0x04 : push [esp+0x08]
        { 0x04, 0xFF }, // (Offset,Value)-Pair #2
        { 0x05, 0x74 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5

        // IDirectSoundBuffer_GetStatus+0x0F : sbb ecx, ecx
        { 0x0F, 0x1B }, // (Offset,Value)-Pair #6
        { 0x10, 0xC9 }, // (Offset,Value)-Pair #7

        // IDirectSoundBuffer_GetStatus+0x11 : and ecx, eax
        { 0x11, 0x23 }, // (Offset,Value)-Pair #8
        { 0x12, 0xC8 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetMode
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetMode, 4134, 7,

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
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetMode, 4134, 8,

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
// * DirectSound_CDirectSoundStream_SetMode
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundStream_SetMode, 4134, 9,

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
OOVPA_XREF(DirectSound_CMcpxVoiceClient_SetFilter, 4134, 8,

    XREF_DirectSound_CMcpxVoiceClient_SetFilter,
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
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetFilter, 4134, 7,

    XREF_DirectSound_CDirectSoundVoice_SetFilter,
    XRefOne)

        XREF_ENTRY( 0x0C, XREF_DirectSound_CMcpxVoiceClient_SetFilter ),

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
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetFilter, 4134, 8,

    XREF_DirectSound_CDirectSoundBuffer_SetFilter,
    XRefOne)

        XREF_ENTRY( 0x32, XREF_DirectSound_CDirectSoundVoice_SetFilter ),

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

        XREF_ENTRY( 0x15, XREF_DirectSound_CDirectSoundBuffer_SetFilter ),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSound::SetVelocity
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_SetVelocity, 4134, 15,

    XREF_DSSETVELOCITYA,
    XRefZero)

        // DirectSound_CDirectSound_SetVelocity+0x24 : mov eax, 0x80004005
        { 0x24, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x25, 0x05 }, // (Offset,Value)-Pair #2
        { 0x26, 0x40 }, // (Offset,Value)-Pair #3
        { 0x27, 0x00 }, // (Offset,Value)-Pair #4
        { 0x28, 0x80 }, // (Offset,Value)-Pair #5

        // DirectSound_CDirectSound_SetVelocity+0x37 : mov [edx+0x44], edi
        { 0x37, 0x89 }, // (Offset,Value)-Pair #6
        { 0x38, 0x7A }, // (Offset,Value)-Pair #7
        { 0x39, 0x44 }, // (Offset,Value)-Pair #8

        // DirectSound_CDirectSound_SetVelocity+0x4C : or dword ptr [eax+0x0A4], 0x40
        { 0x4C, 0x83 }, // (Offset,Value)-Pair #9
        { 0x4D, 0x88 }, // (Offset,Value)-Pair #10
        { 0x4E, 0xA4 }, // (Offset,Value)-Pair #11
        { 0x4F, 0x00 }, // (Offset,Value)-Pair #12
        { 0x52, 0x40 }, // (Offset,Value)-Pair #13

        // DirectSound_CDirectSound_SetVelocity+0x73 : retn 0x14
        { 0x73, 0xC2 }, // (Offset,Value)-Pair #14
        { 0x74, 0x14 }, // (Offset,Value)-Pair #15
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetPitch
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetPitch, 4134, 6,

    XREF_DSBUFFERSETPITCHB,
    XRefZero)

        { 0x02, 0x24 },
        { 0x06, 0x10 },
        { 0x0A, 0x08 },
        { 0x0E, 0x8B },
        { 0x16, 0xC2 },
        { 0x17, 0x08 },
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundBuffer::SetVolume
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetVolume, 4134, 11,

    XREF_DirectSound_CDirectSoundBuffer_SetVolume,
    XRefZero)

        // DirectSound_CDirectSoundBuffer_SetVolume+0x0D : movzx esi, al
        { 0x0D, 0x0F }, // (Offset,Value)-Pair #1
        { 0x0E, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x0F, 0xF0 }, // (Offset,Value)-Pair #3

        // DirectSound_CDirectSoundBuffer_SetVolume+0x26 : jmp +0x22
        { 0x26, 0xEB }, // (Offset,Value)-Pair #4
        { 0x27, 0x22 }, // (Offset,Value)-Pair #5

        // DirectSound_CDirectSoundBuffer_SetVolume+0x2D : push [esp+0x10]
        { 0x2D, 0xFF }, // (Offset,Value)-Pair #6
        { 0x2E, 0x74 }, // (Offset,Value)-Pair #7
        { 0x2F, 0x24 }, // (Offset,Value)-Pair #8
        { 0x30, 0x10 }, // (Offset,Value)-Pair #9

        // DirectSound_CDirectSoundBuffer_SetVolume+0x4B : retn 0x08
        { 0x4B, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x4C, 0x08 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8::SetVolume
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetVolume, 4134, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_DirectSound_CDirectSoundBuffer_SetVolume ),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSound::CreateSoundStream
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_CreateSoundStream, 4134, 14,

    XREF_DSCREATESOUNDSTREAM,
    XRefZero)

        // DirectSound_CDirectSound_CreateSoundStream+0x23 : mov eax, 0x80004005
        { 0x23, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x24, 0x05 }, // (Offset,Value)-Pair #2
        { 0x25, 0x40 }, // (Offset,Value)-Pair #3
        { 0x27, 0x80 }, // (Offset,Value)-Pair #4

        // DirectSound_CDirectSound_CreateSoundStream+0x2A : push 0x28
        { 0x2A, 0x6A }, // (Offset,Value)-Pair #5
        { 0x2B, 0x28 }, // (Offset,Value)-Pair #6

        // DirectSound_CDirectSound_CreateSoundStream+0x4A : add esi, 0x7FF8FFF2
        { 0x4A, 0x81 }, // (Offset,Value)-Pair #7
        { 0x4B, 0xE6 }, // (Offset,Value)-Pair #8
        { 0x4C, 0xF2 }, // (Offset,Value)-Pair #9
        { 0x4D, 0xFF }, // (Offset,Value)-Pair #10
        { 0x4E, 0xF8 }, // (Offset,Value)-Pair #11
        { 0x4F, 0x7F }, // (Offset,Value)-Pair #12

        // DirectSound_CDirectSound_CreateSoundStream+0x8E : retn 0x10
        { 0x8E, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x8F, 0x10 }, // (Offset,Value)-Pair #14
OOVPA_END;

// ******************************************************************
// * DirectSoundCreateStream
// ******************************************************************
OOVPA_XREF(DirectSoundCreateStream, 4134, 12,

    XRefNoSaveIndex,
    XRefOne)

        // DirectSoundCreateStream+0x2F : call [CDirectSound::CreateSoundStream]
        XREF_ENTRY( 0x2F, XREF_DSCREATESOUNDSTREAM ), // (Offset,Value)-Pair #1

        // DirectSoundCreateStream+0x04 : and [ebp-0x04], 0
        { 0x04, 0x83 }, // (Offset,Value)-Pair #2
        { 0x05, 0x65 }, // (Offset,Value)-Pair #3
        { 0x06, 0xFC }, // (Offset,Value)-Pair #4

        // DirectSoundCreateStream+0x08 : push ebx; push esi; push edi
        { 0x08, 0x53 }, // (Offset,Value)-Pair #5
        { 0x09, 0x56 }, // (Offset,Value)-Pair #6
        { 0x0A, 0x57 }, // (Offset,Value)-Pair #7

        // DirectSoundCreateStream+0x3C : call dword ptr [eax+8]
        { 0x3C, 0xFF }, // (Offset,Value)-Pair #8
        { 0x3D, 0x50 }, // (Offset,Value)-Pair #9
        { 0x3E, 0x08 }, // (Offset,Value)-Pair #10

        // DirectSoundCreateStream+0x54 : retn 0x08
        { 0x54, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x55, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundVoiceSettings::SetMixBinVolumes
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoiceSettings_SetMixBinVolumes, 4134, 10,

    XREF_DSSETMIXBINVOLUMESC,
    XRefZero)

        // DirectSound_CDirectSoundVoiceSettings_SetMixBinVolumes+0x09 : jbe +0x16
        { 0x09, 0x76 }, // (Offset,Value)-Pair #1
        { 0x0A, 0x16 }, // (Offset,Value)-Pair #2

        // DirectSound_CDirectSoundVoiceSettings_SetMixBinVolumes+0x18 : mov [ecx+edi*4+0x30], eax
        { 0x18, 0x89 }, // (Offset,Value)-Pair #3
        { 0x19, 0x44 }, // (Offset,Value)-Pair #4
        { 0x1A, 0xB9 }, // (Offset,Value)-Pair #5
        { 0x1B, 0x30 }, // (Offset,Value)-Pair #6

        // DirectSound_CDirectSoundVoiceSettings_SetMixBinVolumes+0x1E : jb +0xEC
        { 0x1E, 0x72 }, // (Offset,Value)-Pair #7
        { 0x1F, 0xEC }, // (Offset,Value)-Pair #8

        // DirectSound_CDirectSoundVoiceSettings_SetMixBinVolumes+0x22 : retn 0x04
        { 0x22, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x23, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundVoice::SetMixBinVolumes
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetMixBinVolumes, 4134, 17,

    XREF_DSSETMIXBINVOLUMESB,
    XRefOne)

        // DirectSound_CDirectSoundVoice_SetMixBinVolumes+0x0D : call [CDirectSoundVoiceSettings::SetMixBinVolumes]
        XREF_ENTRY( 0x0D, XREF_DSSETMIXBINVOLUMESC ), // (Offset,Value)-Pair #1

        // DirectSound_CDirectSoundVoice_SetMixBinVolumes+0x01 : mov esi, [esp+8]
        { 0x01, 0x8B }, // (Offset,Value)-Pair #2
        { 0x02, 0x74 }, // (Offset,Value)-Pair #3
        { 0x03, 0x24 }, // (Offset,Value)-Pair #4
        { 0x04, 0x08 }, // (Offset,Value)-Pair #5

        // DirectSound_CDirectSoundVoice_SetMixBinVolumes+0x05 : push [esp+0x0C]
        { 0x05, 0xFF }, // (Offset,Value)-Pair #6
        { 0x06, 0x74 }, // (Offset,Value)-Pair #7
        { 0x07, 0x24 }, // (Offset,Value)-Pair #8
        { 0x08, 0x0C }, // (Offset,Value)-Pair #9

        // DirectSound_CDirectSoundVoice_SetMixBinVolumes+0x09 : mov ecx, [esi+0x10]
        { 0x09, 0x8B }, // (Offset,Value)-Pair #10
        { 0x0A, 0x4E }, // (Offset,Value)-Pair #11
        { 0x0B, 0x10 }, // (Offset,Value)-Pair #12

        // DirectSound_CDirectSoundVoice_SetMixBinVolumes+0x11 : mov ecx, [esi+0x0C]
        { 0x11, 0x8B }, // (Offset,Value)-Pair #13
        { 0x12, 0x4E }, // (Offset,Value)-Pair #14
        { 0x13, 0x0C }, // (Offset,Value)-Pair #15

        // DirectSound_CDirectSoundVoice_SetMixBinVolumes+0x1A : retn 0x08
        { 0x1A, 0xC2 }, // (Offset,Value)-Pair #16
        { 0x1B, 0x08 }, // (Offset,Value)-Pair #17
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundBuffer::SetMixBinVolumes
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetMixBinVolumes, 4134, 17,

    XREF_DSSETMIXBINVOLUMESA,
    XRefOne)

        // DirectSound_CDirectSoundBuffer_SetMixBinVolumes+0x32 : call [CDirectSoundVoice::SetMixBinVolumes]
        XREF_ENTRY( 0x32, XREF_DSSETMIXBINVOLUMESB ), // (Offset,Value)-Pair #1

        // DirectSound_CDirectSoundBuffer_SetMixBinVolumes+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 }, // (Offset,Value)-Pair #2
        { 0x22, 0x05 }, // (Offset,Value)-Pair #3
        { 0x23, 0x40 }, // (Offset,Value)-Pair #4
        { 0x24, 0x00 }, // (Offset,Value)-Pair #5
        { 0x25, 0x80 }, // (Offset,Value)-Pair #6

        // DirectSound_CDirectSoundBuffer_SetMixBinVolumes+0x29 : push [esp+0x10]; push [esp+0x10]
        { 0x29, 0xFF }, // (Offset,Value)-Pair #7
        { 0x2A, 0x74 }, // (Offset,Value)-Pair #8
        { 0x2B, 0x24 }, // (Offset,Value)-Pair #9
        { 0x2C, 0x10 }, // (Offset,Value)-Pair #10
        { 0x2D, 0xFF }, // (Offset,Value)-Pair #11
        { 0x2E, 0x74 }, // (Offset,Value)-Pair #12
        { 0x2F, 0x24 }, // (Offset,Value)-Pair #13
        { 0x30, 0x10 }, // (Offset,Value)-Pair #14

        // DirectSound_CDirectSoundBuffer_SetMixBinVolumes+0x49 : pop edi
        { 0x49, 0x5F }, // (Offset,Value)-Pair #15

        // DirectSound_CDirectSoundBuffer_SetMixBinVolumes+0x4B : retn 0x08
        { 0x4B, 0xC2 }, // (Offset,Value)-Pair #16
        { 0x4C, 0x08 }, // (Offset,Value)-Pair #17
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetMixBinVolumes
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMixBinVolumes, 4134, 12,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetMixBinVolumes+0x15 : call [CDirectSoundBuffer::SetMixBinVolumes]
        XREF_ENTRY( 0x15, XREF_DSSETMIXBINVOLUMESA ), // (Offset,Value)-Pair #1

        // IDirectSoundBuffer_SetMixBinVolumes+0x04 : push [esp+0x08]
        { 0x04, 0xFF }, // (Offset,Value)-Pair #2
        { 0x05, 0x74 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5

        // IDirectSoundBuffer_SetMixBinVolumes+0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 }, // (Offset,Value)-Pair #6
        { 0x0B, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x0C, 0xE4 }, // (Offset,Value)-Pair #8

        // IDirectSoundBuffer_SetMixBinVolumes+0x0F : sbb ecx, ecx
        { 0x0F, 0x1B }, // (Offset,Value)-Pair #9
        { 0x10, 0xC9 }, // (Offset,Value)-Pair #10

        // IDirectSoundBuffer_SetMixBinVolumes+0x11 : and ecx, eax
        { 0x11, 0x23 }, // (Offset,Value)-Pair #11
        { 0x12, 0xC8 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundStream_SetMixBinVolumes
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundStream_SetMixBinVolumes, 4134, 12,

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
// * Direct, 4134
// ******************************************************************
OOVPATable DSound_4134[] = {

	REGISTER_OOVPA(DirectSoundCreate, 4134, PATCH),
	REGISTER_OOVPA(DirectSoundDoWork, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_CreateSoundStream, 4134, XREF),
	REGISTER_OOVPA(IDirectSound_CreateSoundStream, 3936, PATCH),
	REGISTER_OOVPA(DirectSoundCreateStream, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_CreateSoundBuffer, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetLoopRegion, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetLoopRegion, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetI3DL2Listener, 4134, XREF),
	REGISTER_OOVPA(IDirectSound_SetI3DL2Listener, 4134, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_SetHeadroomA, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetHeadroom, 4134, PATCH),
	REGISTER_OOVPA(IDirectSound_SetVelocity, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound_SetAllParameters, 3936, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoiceSettings_SetMixBins, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetMixBins, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetMixBins, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMixBins, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetPositionA, 4134, XREF), // TODO: Find a cure for laziness...
	REGISTER_OOVPA(IDirectSound_SetPosition, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound_CreateSoundBuffer, 4134, PATCH),
	REGISTER_OOVPA(IDirectSound_AddRef, 3936, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetFrequency, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetFrequency, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetFrequency, 4134, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_SetVolume, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetVolume, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetVolume, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetVolume, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetVolume, 4134, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_LockA, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Lock, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound_CreateSoundStream, 3936, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_SetBufferData, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetBufferData, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetBufferData, 4134, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_GetStatus, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_GetStatus, 4134, XREF),
	REGISTER_OOVPA(CMcpxBuffer_SetCurrentPosition, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetCurrentPosition, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetCurrentPosition, 4134, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_GetCurrentPosition, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_GetCurrentPosition, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_GetCurrentPosition, 3936, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_Play, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Play, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound_Release, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound_DownloadEffectsImage, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound_SetOrientation, 3936, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetDistanceFactorA, 4134, XREF),
	REGISTER_OOVPA(IDirectSound_SetDistanceFactor, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetRolloffFactor, 4134, XREF),
	REGISTER_OOVPA(IDirectSound_SetRolloffFactor, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetDopplerFactor, 4134, XREF),
	REGISTER_OOVPA(IDirectSound_SetDopplerFactor, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_CommitDeferredSettings, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetMaxDistance, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetMaxDistance, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMaxDistance, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetMaxDistance, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetMinDistance, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetMinDistance, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMinDistance, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetMinDistance, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetRolloffFactor, 4134, XREF), // s+
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetRolloffFactor, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetRolloffFactor, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetRolloffFactor, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetDistanceFactor, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetDistanceFactor, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetDistanceFactor, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetConeAngles, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetConeAngles, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetConeAngles, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetConeOrientation, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetConeOrientation, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetConeOrientation, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetConeOrientation, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetConeOutsideVolume, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetConeOutsideVolume, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetConeOutsideVolume, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetConeOutsideVolume, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetPosition, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetPosition, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetPosition, 3936, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetPosition, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetVelocity, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetVelocity, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetVelocity, 3936, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetVelocity, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetDopplerFactor, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetDopplerFactor, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetDopplerFactor, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetI3DL2Source, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetI3DL2Source, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetI3DL2Source, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetI3DL2Source, 4134, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_Stop, 4134, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_Release, 3936, PATCH), // +s
	REGISTER_OOVPA(IDirectSoundBuffer_AddRef, 3936, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetAllParameters, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetAllParameters, 4134, XREF),
	REGISTER_OOVPA(IDirectSound_SetAllParameters, 4134, ALIAS, IDirectSoundBuffer_SetAllParameters), // Use that for now. Okay, it's your call pal...
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetAllParameters, 4134, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_GetStatus, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetMode, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetMode, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMode, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetMode, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CMcpxVoiceClient_SetFilter, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetFilter, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetFilter, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetFilter, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetVelocity, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetPitch, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoiceSettings_SetMixBinVolumes, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetMixBinVolumes, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetMixBinVolumes, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMixBinVolumes, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetMixBinVolumes, 4134, PATCH),
};

// ******************************************************************
// * DSound_4134_SIZE
// ******************************************************************
uint32 DSound_4134_SIZE = sizeof(DSound_4134);
