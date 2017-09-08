// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->DSound.1.0.4039.inl
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
// * DirectSoundCreate
// ******************************************************************
OOVPA_NO_XREF(DirectSoundCreate, 4039, 9)
    // DirectSoundCreate+0x0B : mov esi, eax
    { 0x0B, 0x8B },
    { 0x0C, 0xF0 },

    // DirectSoundCreate+0x11 : call [addr]
    { 0x11, 0xE8 },

    // DirectSoundCreate+0x1A : jl +0x13
    { 0x1A, 0x7C },
    { 0x1B, 0x13 },

    // DirectSoundCreate+0x26 : sbb ecx, ecx
    { 0x26, 0x1B },
    { 0x27, 0xC9 },

    // DirectSoundCreate+0x43 : retn 0x0C
    { 0x43, 0xC2 },
    { 0x44, 0x0C },
OOVPA_END;

// ******************************************************************
// * DirectSoundCreateBuffer
// ******************************************************************
OOVPA_XREF(DirectSoundCreateBuffer, 4039, 11,

           XRefNoSaveIndex,
           XRefOne)

    // DirectSoundCreateBuffer+0x2B : call [CDirectSound::CreateSoundBuffer]
    XREF_ENTRY(0x2C, XREF_DSCREATESOUNDBUFFER),

    // DirectSoundCreateBuffer+0x04 : and [ebp-0x04], 0
    { 0x04, 0x83 },
    { 0x05, 0x65 },
    { 0x06, 0xFC },

    // DirectSoundCreateBuffer+0x08 : push esi; push edi
    { 0x08, 0x56 },
    { 0x09, 0x57 },

    // DirectSoundCreateBuffer+0x32 : lea eax, [ebp-0x04]
    { 0x32, 0x8D },
    { 0x33, 0x45 },
    { 0x34, 0xFC },

    // DirectSoundCreateBuffer+0x4F : retn 0x08
    { 0x4F, 0xC2 },
    { 0x50, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSound::CreateSoundBuffer
// ******************************************************************
OOVPA_XREF(CDirectSound_CreateSoundBuffer, 4039, 15,

           XREF_DSCREATESOUNDBUFFER,
           XRefZero)
    // CDirectSound_CreateSoundBuffer+0x00 : push ebp
    { 0x00, 0x55 },

    // CDirectSound_CreateSoundBuffer+0x25 : mov eax, 0x80004005
    { 0x25, 0xB8 },
    { 0x26, 0x05 },
    { 0x27, 0x40 },
    { 0x29, 0x80 },

    // CDirectSound_CreateSoundBuffer+0x2E : push 0x24
    { 0x2E, 0x6A },
    { 0x2F, 0x24 },

    // CDirectSound_CreateSoundBuffer+0x4C : add esi, 0x7FF8FFF2
    { 0x4C, 0x81 },
    { 0x4D, 0xE6 },
    { 0x4E, 0xF2 },
    { 0x4F, 0xFF },
    { 0x50, 0xF8 },
    { 0x51, 0x7F },

    // CDirectSound_CreateSoundBuffer+0x9C : retn 0x10
    { 0x9C, 0xC2 },
    { 0x9D, 0x10 },
OOVPA_END;

// ******************************************************************
// * CDirectSound_DoWork
// ******************************************************************
OOVPA_XREF(CDirectSound_DoWork, 4039, 13,

    XREF_DSDOWORKA,
    XRefZero)

        // CDirectSound_DoWork+0x0D : mov esi, eax
        { 0x0D, 0x8B },
        { 0x0E, 0xF0 },

        // CDirectSound_DoWork+0x0F : jnz +0x0C
        { 0x0F, 0x75 },
        { 0x10, 0x0C },

        // CDirectSound_DoWork+0x11 : mov eax, [esp+4+arg_0]
        { 0x11, 0x8B },
        { 0x12, 0x44 },
        { 0x13, 0x24 },
        { 0x14, 0x08 },

        // CDirectSound_DoWork+0x15 : mov ecx, [eax+0Ch]
        { 0x15, 0x8B },
        { 0x16, 0x48 },
        { 0x17, 0x0C },

        // CDirectSound_DoWork+0x24 : retn 0x04
        { 0x2D, 0xC2 },
        { 0x2E, 0x04 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetLoopRegion
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetLoopRegion, 4039, 16,

    XREF_CDirectSoundBuffer_SetLoopRegion,
    XRefZero)

        // CDirectSoundBuffer_SetLoopRegion+0x23 : mov eax, 0x80004005
        { 0x23, 0xB8 }, // Was 4134 Offset -0x01h
        { 0x24, 0x05 },
        { 0x25, 0x40 },
        { 0x26, 0x00 },
        { 0x27, 0x80 },

        // CDirectSoundBuffer_SetLoopRegion+0x3C : lea ebx, [esi+ecx]
        { 0x3C, 0x8D },
        { 0x3D, 0x1C },
        { 0x3E, 0x0E },

        // CDirectSoundBuffer_SetLoopRegion+0x3F : cmp ebx, [eax+148h]
        { 0x3F, 0x3B }, // Was 4134 Offset 0x40 cmp ebx, [eax+0C4h] 
        { 0x40, 0x98 },

        // CDirectSoundBuffer_SetLoopRegion+0x5D : mov [eax+150h], ecx
        { 0x61, 0x00 },
        { 0x62, 0x00 },

        // CDirectSoundBuffer_SetLoopRegion+0x63 : mov ecx, [edx+20h]
        { 0x63, 0x8B },
        { 0x64, 0x4A },
        { 0x65, 0x20 },

        // CDirectSoundBuffer_SetLoopRegion+0x66 : call CMcpxBuffer_SetLoopRegion
        { 0x66, 0xE8 },
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetDistanceFactor
// ******************************************************************
OOVPA_XREF(CDirectSound_SetDistanceFactor, 4039, 17,

    XREF_SETDISTANCEFACTORA,
    XRefZero)

        // CDirectSound_SetDistanceFactor+0x20 : mov eax, 0x80004005
        { 0x20, 0xB8 },
        { 0x21, 0x05 },
        { 0x22, 0x40 },
        { 0x23, 0x00 },
        { 0x24, 0x80 },

        // CDirectSound_SetDistanceFactor+0x32 : mov [eax+48h], edx
        { 0x32, 0x89 },
        { 0x33, 0x50 },
        { 0x34, 0x48 }, // Value is CDirectSound_SetDistanceFactor +0x00h

        // CDirectSound_SetDistanceFactor+0x35 : mov eax, [ecx+8]
        { 0x35, 0x8B },
        { 0x36, 0x41 },
        { 0x37, 0x08 },

        // CDirectSound_SetDistanceFactor+0x38 : or dword ptr [eax+84h], 60h
        { 0x38, 0x83 },
        { 0x39, 0x88 },
        { 0x3A, 0x84 }, // 4039 0x84 vs 4134 0xA4
        { 0x3E, 0x60 },

        // CDirectSound_SetDistanceFactor+0x4E : jz +0x0B
        { 0x4E, 0x74 },
        { 0x4F, 0x0B },
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetRolloffFactor
// ******************************************************************
OOVPA_XREF(CDirectSound_SetRolloffFactor, 4039, 17,

    XREF_SETROLLOFFFACTORA,
    XRefZero)

        // CDirectSound_SetRolloffFactor+0x20 : mov eax, 0x80004005
        { 0x20, 0xB8 },
        { 0x21, 0x05 },
        { 0x22, 0x40 },
        { 0x23, 0x00 },
        { 0x24, 0x80 },

        // CDirectSound_SetRolloffFactor+0x33 : mov [eax+4Ch], edx
        { 0x32, 0x89 },
        { 0x33, 0x50 },
        { 0x34, 0x4C }, // Value is CDirectSound_SetDistanceFactor +0x04h

        // CDirectSound_SetRolloffFactor+0x36 : mov eax, [ecx+8]
        { 0x35, 0x8B },
        { 0x36, 0x41 },
        { 0x37, 0x08 },

        // CDirectSound_SetRolloffFactor+0x39 : or dword ptr[eax+0x84], 0x04
        { 0x38, 0x83 },
        { 0x39, 0x88 },
        { 0x3A, 0x84 }, // 4039 0x84 vs 4134 0xA4
        { 0x3E, 0x04 },

        // CDirectSound_SetRolloffFactor+0x4E : jz +0x0B
        { 0x4E, 0x74 },
        { 0x4F, 0x0B },
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetDopplerFactor
// ******************************************************************
OOVPA_XREF(CDirectSound_SetDopplerFactor, 4039, 14,

    XREF_SETDOPPLERFACTOR,
    XRefZero)

        // CDirectSound_SetDopplerFactor+0x20 : mov eax, 0x80004005
        { 0x20, 0xB8 },
        { 0x21, 0x05 },
        { 0x22, 0x40 },
        { 0x23, 0x00 },
        { 0x24, 0x80 },

        // CDirectSound_SetDopplerFactor+0x32 : mov [eax+0x50], edx
        { 0x32, 0x89 },
        { 0x33, 0x50 },
        { 0x34, 0x50 }, // Value is CDirectSound_SetDistanceFactor+ 0x08h

        // CDirectSound_SetDopplerFactor+0x38 : or dword ptr[eax+0x84], 0x40
        { 0x38, 0x83 },
        { 0x39, 0x88 },
        { 0x3A, 0x84 }, // 4039 0x84 vs 4134 0xA4 
        { 0x3E, 0x40 },

        // CDirectSound_SetDopplerFactor+0x4E : jz +0x0B
        { 0x4E, 0x74 },
        { 0x4F, 0x0B },
OOVPA_END;

// ******************************************************************
// * CMcpxVoiceClient_SetVolume
// ******************************************************************
// Verified with Agent Under Fire title.
OOVPA_XREF(CMcpxVoiceClient_SetVolume, 4039, 13,

    XREF_DSSTREAMSETVOLUME,
    XRefZero)

        // CMcpxVoiceClient_SetVolume+0x15 : test byte ptr [esi+90h], 1
        { 0x15, 0xF6 },
        { 0x16, 0x86 },
        { 0x17, 0x90 },
        { 0x18, 0x00 },
        { 0x19, 0x00 },
        { 0x1A, 0x00 },
        { 0x1B, 0x01 },

        // CMcpxVoiceClient_SetVolume+0x1C : jz +0x75
        { 0x1C, 0x74 },
        { 0x1D, 0x75 },

        // CMcpxVoiceClient_SetVolume+0x73 : mov edx, [ebp+eax*4-0x14]
        { 0x73, 0x8B }, // Was 4134 Offset -0x07h
        { 0x74, 0x54 },
        { 0x75, 0x85 },
        { 0x76, 0xEC },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetVolume
// ******************************************************************
// Verified with Agent Under Fire.
OOVPA_XREF(CDirectSoundVoice_SetVolume, 4039, 13,

    XREF_CDirectSoundVoice_SetVolume,
    XRefOne)

        // CDirectSoundVoice_SetVolume+0x15 : call [CMcpxVoiceClient::SetVolume]
        XREF_ENTRY( 0x15, XREF_DSSTREAMSETVOLUME ),

        // CDirectSoundVoice_SetVolume+0x04 : mov eax, [ecx+10h]
        { 0x04, 0x8B },
        { 0x05, 0x41 },
        { 0x06, 0x10 },

        // CDirectSoundVoice_SetVolume+0x07 : mov edx, [esp+arg_4]
        { 0x07, 0x8B },
        { 0x08, 0x54 },
        { 0x09, 0x24 },
        { 0x0A, 0x08 },

        // CDirectSoundVoice_SetVolume+0x0B : sub edx, [eax+28h]
        { 0x0B, 0x2B },
        { 0x0C, 0x50 },
        { 0x0D, 0x28 }, // 4039 0x28 vs 4134 0x20

        // CDirectSoundVoice_SetVolume+0x19 : retn 8
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetVolume
// ******************************************************************
// Verified with Agent Under Fire.
OOVPA_XREF(CDirectSoundBuffer_SetVolume, 4039, 10,

    XREF_CDirectSoundBuffer_SetVolume,
    XRefOne)

        // CDirectSoundBuffer_SetVolume+0x31 : call [CMcpxVoiceClient::SetVolume]
        XREF_ENTRY( 0x31, XREF_CDirectSoundVoice_SetVolume ), // Was 4134 Offset -0x01h

        // CDirectSoundBuffer_SetVolume+0x00 : push esi
        { 0x00, 0x56 },

        // CDirectSoundBuffer_SetVolume+0x25 : jmp +0x22
        { 0x25, 0xEB },
        { 0x26, 0x22 },

        // CDirectSoundBuffer_SetVolume+0x2C : push [esp+0x10]
        { 0x2C, 0xFF },
        { 0x2D, 0x74 },
        { 0x2E, 0x24 },
        { 0x2F, 0x10 },

        // CDirectSoundBuffer_SetVolume+0x4A : retn 0x08
        { 0x4A, 0xC2 },
        { 0x4B, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer::SetVolume
// ******************************************************************
// Verified with Agent Under Fire.
// Side note: It is compatible down to XDK 3911. Except 3911 is calling to CDirectSoundVoice_SetVolume directly.
OOVPA_XREF(IDirectSoundBuffer_SetVolume, 4039, 8,

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

// ******************************************************************
// * CDirectSoundStream_SetVolume
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetVolume, 4039, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x35, XREF_CDirectSoundVoice_SetVolume ), // Was 4134 Offset -0x01h

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x13, 0x74 },
        { 0x20, 0xB8 },
        { 0x29, 0x24 },
        { 0x34, 0xE8 },
        { 0x3F, 0x68 },
        { 0x4A, 0x8B },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice_SetHeadroom
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetHeadroom, 4039, 12,

    XREF_CDirectSoundVoice_SetHeadroom,
    XRefZero)

        { 0x00, 0x8B },
        { 0x08, 0x48 },
        { 0x0F, 0x2B },

        { 0x13, 0x24 }, // 4039 0x24 vs 4134 0x1C
        { 0x14, 0x89 },
        { 0x15, 0x70 },
        { 0x16, 0x28 }, // 4039 0x28 vs 4134 0x20
        { 0x17, 0x8B },
        { 0x18, 0x4A },
        { 0x19, 0x0C },
        { 0x1A, 0xE8 },

        { 0x21, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_SetHeadroom
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetHeadroom, 4039, 8,

	XREF_DSBUFFERSETHEADROOMA,
	XRefOne)

        XREF_ENTRY( 0x31, XREF_CDirectSoundVoice_SetHeadroom ), // Was 4134 Offset -0x01h

        { 0x00, 0x56 },
        { 0x11, 0x85 },
        { 0x1B, 0x15 },
        { 0x25, 0xEB },
        { 0x2F, 0x10 },
        { 0x39, 0x74 },
        { 0x46, 0x8B },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_SetBufferData
// ******************************************************************
// Verified with Agent Under Fire.
OOVPA_XREF(CDirectSoundBuffer_SetBufferData, 4039, 16,

    XREF_DSSETBUFFERDATA,
    XRefZero)

        // CDirectSoundBuffer_SetBufferData+0x3D : mov eax, 0x80004005
        { 0x3D, 0xB8 }, // Was 4134 Offset -0x03h
        { 0x3E, 0x05 },
        { 0x3F, 0x40 },
        { 0x40, 0x00 },
        { 0x41, 0x80 },

        // CDirectSoundBuffer_SetBufferData+0x48 : mov eax, [esi+1Ch]
        { 0x48, 0x8B },
        { 0x49, 0x46 },
        { 0x4A, 0x1C },

        // CDirectSoundBuffer_SetBufferData+0x4B : cmp  ebx, [eax+13Ch]
        { 0x4B, 0x3B },
        { 0x4C, 0x98 },
        { 0x4D, 0x3C },
        { 0x4E, 0x01 },
        { 0x4F, 0x00 },
        { 0x50, 0x00 },

        // CDirectSoundBuffer_SetBufferData+0xA8 : retn 0Ch
        { 0xA8, 0xC2 },
        { 0xA9, 0x0C },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetPitch
// ******************************************************************
// Verified with Agent Under Fire.
// Same as 4134
OOVPA_XREF(CDirectSoundVoice_SetPitch, 4039, 6,

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
// * CDirectSoundBuffer::SetPitch
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetPitch, 4039, 13,

    XREF_DSBUFFERSETPITCHA,
    XRefOne)

        // CDirectSoundBuffer_SetPitch+0x31 : call [CDirectSoundVoice::SetFrequency]
        XREF_ENTRY( 0x31, XREF_DSBUFFERSETPITCHB ), // Was 4134 Offset -0x01h

        // CDirectSoundBuffer_SetPitch+0x00 : push esi
        { 0x00, 0x56 },

        // CDirectSoundBuffer_SetPitch+0x28 : push [esp+0x10]; push [esp+0x10]
        { 0x28, 0xFF },
        { 0x29, 0x74 },
        { 0x2A, 0x24 },
        { 0x2B, 0x10 },
        { 0x2C, 0xFF },
        { 0x2D, 0x74 },
        { 0x2E, 0x24 },
        { 0x2F, 0x10 },

        // CDirectSoundBuffer_SetPitch+0x48 : pop edi
        { 0x48, 0x5F },

        // CDirectSoundBuffer_SetPitch+0x4A : retn 0x08
        { 0x4A, 0xC2 },
        { 0x4B, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetFrequency
// ******************************************************************
// Verified with Agent Under Fire.
OOVPA_XREF(CDirectSoundVoice_SetFrequency, 4039, 11,

    XREF_CDirectSoundVoice_SetFrequency,
    XRefZero)

        // CDirectSoundVoice_SetFrequency+0x0D : mov eax, [esi+0x10]
        { 0x0D, 0x8B },
        { 0x0E, 0x46 },
        { 0x0F, 0x10 },

        // CDirectSoundVoice_SetFrequency+0x10 : mov eax, [eax+0x18]
        { 0x10, 0x8B },
        { 0x11, 0x40 },
        { 0x12, 0x18 }, // 4039 0x18 vs 4134 0x10

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
// Verified with Agent Under Fire.
OOVPA_XREF(CDirectSoundBuffer_SetFrequency, 4039, 13,

    XREF_DSBUFFERSETFREQUENCYA,
    XRefOne)

        // CDirectSoundBuffer_SetFrequency+0x31 : call [CDirectSoundVoice::SetFrequency]
        XREF_ENTRY( 0x31, XREF_CDirectSoundVoice_SetFrequency ), // Was 4134 Offset -0x01h

        // CDirectSoundBuffer_SetFrequency+0x00 : push esi
        { 0x00, 0x56 },

        // CDirectSoundBuffer_SetFrequency+0x28 : push [esp+0x10]; push [esp+0x10]
        { 0x28, 0xFF },
        { 0x29, 0x74 },
        { 0x2A, 0x24 },
        { 0x2B, 0x10 },
        { 0x2C, 0xFF },
        { 0x2D, 0x74 },
        { 0x2E, 0x24 },
        { 0x2F, 0x10 },

        // CDirectSoundBuffer_SetFrequency+0x48 : pop edi
        { 0x48, 0x5F },

        // CDirectSoundBuffer_SetFrequency+0x4A : retn 0x08
        { 0x4A, 0xC2 },
        { 0x4B, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetFrequency
// ******************************************************************
// Verified with Agent Under Fire.
OOVPA_XREF(IDirectSoundBuffer_SetFrequency, 4039, 12,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetFrequency+0x15 : call [CDirectSound::SetFrequency]
        XREF_ENTRY( 0x15, XREF_DSBUFFERSETFREQUENCYA),

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

// ******************************************************************
// * CDirectSoundBuffer_Stop
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_Stop, 4039, 12,

    XREF_CDirectSoundBuffer_Stop,
    XRefZero)

        // CDirectSoundBuffer_Stop+0x33 : call [CMcpxBuffer::Stop]
        //XREF_ENTRY( 0x30, XREF_CMcpxBuffer_Stop), //TODO: is this CMcpxBuffer::Stop or something else?

        // CDirectSoundBuffer_Stop+0x20 : mov eax, 0x80004005
        { 0x20, 0xB8 },
        { 0x21, 0x05 },
        { 0x22, 0x40 },
        { 0x23, 0x00 },
        { 0x24, 0x80 },

        // CDirectSoundBuffer_Stop+0x2B : mov ecx,[eax+20]
        { 0x2B, 0x8B },
        { 0x2C, 0x48 },
        { 0x2D, 0x20 },

        // CDirectSoundBuffer_Stop+0x38 : jz +0x0B
        { 0x38, 0x74 },
        { 0x39, 0x0B },

        // CDirectSoundBuffer_Stop+0x49 : retn 0x04
        { 0x49, 0xC2 },
        { 0x4A, 0x04 },
OOVPA_END;
#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * IDirectSoundBuffer_Stop
// ******************************************************************
OOVPA_NO_XREF(IDirectSoundBuffer_Stop, 4039, 11)

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
// * CDirectSound::CommitDeferredSettings
// ******************************************************************
// Verified with Agent Under Fire.
OOVPA_NO_XREF(CDirectSound_CommitDeferredSettings, 4039, 14)

        // CDirectSound_CommitDeferredSettings+0x00 : push    ebp
        { 0x00, 0x55 },

        // CDirectSound_CommitDeferredSettings+0x24 : mov eax, 0x80004005
        { 0x24, 0xB8 }, // Was 4134 Offset -0x03h
        { 0x25, 0x05 },
        { 0x26, 0x40 },
        { 0x27, 0x00 },
        { 0x28, 0x80 },

        // CDirectSound_CommitDeferredSettings+0x52 : and dword ptr [eax+84h], 0
        { 0x52, 0x83 },
        { 0x53, 0xA0 },
        { 0x54, 0x84 },
        { 0x55, 0x00 },
        { 0x56, 0x00 },
        { 0x57, 0x00 },
        { 0x58, 0x00 },

        // CDirectSound_CommitDeferredSettings+0x78 : leave
        { 0x6F, 0xC9 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::Play
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_Play, 4039, 13,

    XREF_CDirectSoundBuffer_Play,
    XRefZero)

    //CDirectSoundBuffer_Play+0x00 : push esi
    { 0x00, 0x56 },

    //CDirectSoundBuffer_Play+0x20 : mov eax, 80004005h
    { 0x20, 0xB8 },
    { 0x21, 0x05 },
    { 0x22, 0x40 },
    { 0x23, 0x00 },
    { 0x24, 0x80 },

    //CDirectSoundBuffer_Play+0x2F : mov ecx, dword ptr [esp+18h]
    { 0x2F , 0xFF },
    { 0x30 , 0x74 },
    { 0x31 , 0x24 },
    { 0x32 , 0x18 },

    //CDirectSoundBuffer_Play+0x4D : ret 10h
    { 0x4D , 0xC2 },
    { 0x4E , 0x10 },
    { 0x4F , 0x00 },
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_GetCurrentPosition
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_GetCurrentPosition, 4039, 17,

    XREF_CMcpxBuffer_GetCurrentPosition,
    XRefZero)

        // CMcpxBuffer_GetCurrentPosition+0x16 : mov eax, [esi+0x00000090]
        { 0x16, 0x8B },
        { 0x17, 0x86 },
        { 0x18, 0x90 },
        { 0x19, 0x00 },
        //{ 0x1A, 0x00 },
        //{ 0x1B, 0x00 },

        // CMcpxBuffer_GetCurrentPosition+0x21 : jnz +0x00000088
        { 0x21, 0x0F },
        { 0x22, 0x85 },
        { 0x23, 0x88 },
        { 0x24, 0x00 },
        //{ 0x25, 0x00 },
        //{ 0x26, 0x00 },

        // CMcpxBuffer_GetCurrentPosition+0x8E : div dword ptr [ecx+0x00000140]
        { 0xA5, 0xF7 },
        { 0xA6, 0xB1 },
        { 0xA7, 0x40 },
        { 0xA8, 0x01 },
        { 0xA9, 0x00 },
        { 0xAA, 0x00 },

        // CMcpxBuffer_GetCurrentPosition+0xC8 : retn 0x08
        { 0xDB, 0xC2 },
        { 0xDC, 0x08 },
        { 0xDD, 0x00 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_GetCurrentPosition
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_GetCurrentPosition, 4039, 10,

	XREF_CDirectSoundBuffer_GetCurrentPosition,
    XRefOne)

        // CDirectSoundBuffer_GetCurrentPosition+0x38 : call [CMcpxBuffer::GetCurrentPosition]
        XREF_ENTRY( 0x38, XREF_CMcpxBuffer_GetCurrentPosition), 

        // CDirectSoundBuffer_GetCurrentPosition+0x20 : mov eax, 0x80004005
        { 0x20, 0xB8 },
        { 0x21, 0x05 },
        { 0x22, 0x40 },
        { 0x23, 0x00 },
        { 0x24, 0x80 },

        // CDirectSoundBuffer_GetCurrentPosition+0x40 : jz +0x0B
        { 0x40, 0x74 },
        { 0x41, 0x0B },

        // CDirectSoundBuffer_GetCurrentPosition+0x51 : retn 0x08
        { 0x51, 0xC2 },
        { 0x52, 0x0C },
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_GetStatus
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_GetStatus, 4039, 13,

    XREF_CMcpxBuffer_GetStatus,
    XRefZero)

        // CMcpxBuffer_GetStatus+0x14 : mov ecx, [ebp+0x08]
        { 0x14, 0x8B },
        { 0x15, 0x4D },
        { 0x16, 0x08 },

        // CMcpxBuffer_GetStatus+0x1A : xor edx, edx
        { 0x1A, 0x33 },
        { 0x1B, 0xD2 },

        // CMcpxBuffer_GetStatus+0x22 : inc edx
        { 0x22, 0x42 },

        // CMcpxBuffer_GetStatus+0x1C : jz +0x17
        { 0x26, 0x74 },
        { 0x27, 0x15 },

        // CMcpxBuffer_GetStatus+0x2F : mov [ecx], 5
        { 0x37, 0xC7 },
        { 0x38, 0x01 },
        { 0x39, 0x05 },

        // CMcpxBuffer_GetStatus+0x48 : retn 0x04
        { 0x48, 0xC2 },
        { 0x49, 0x04 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_GetStatus
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_GetStatus, 4039, 10,

    XREF_CDirectSoundBuffer_GetStatus,
    XRefOne)

        // CDirectSoundBuffer_GetStatus+0x14 : call [CMcpxBuffer::GetStatus]
        XREF_ENTRY( 0x34, XREF_CMcpxBuffer_GetStatus),

        // CDirectSoundBuffer_GetStatus+0x2B : mov ecx, [eax+0x20]
        { 0x2B, 0x8B },
        { 0x2C, 0x48 },
        { 0x2D, 0x20 },

        // CDirectSoundBuffer_GetStatus+0x2F : push [esp+0x10]
        { 0x2F, 0xFF },
        { 0x30, 0x74 },
        { 0x31, 0x24 },
        { 0x32, 0x10 },

        // CDirectSoundBuffer_GetStatus+0x4D : retn 0x08
        { 0x4D, 0xC2 },
        { 0x4E, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_Lock
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_Lock, 4039, 13,

    XREF_DSBUFFERLOCKA,
    XRefZero)

        // CDirectSoundBuffer_Lock+0x34 : test [ebp+0x24], 1
        { 0x34, 0xF6 },
        { 0x35, 0x45 },
        { 0x36, 0x24 },
        { 0x37, 0x01 },

        // CDirectSoundBuffer_Lock+0x59 : mov eax, [eax+0x140]
        { 0x59, 0x8B },
        { 0x5A, 0x80 },
        { 0x5B, 0x40 },
        { 0x5C, 0x01 },

        // CDirectSoundBuffer_Lock+0x9F : jnb +0x11; mov esi, [esi+0x1C]
        { 0x9F, 0x73 },
        { 0xA0, 0x11 },
        { 0xA1, 0x8B },
        { 0xA2, 0x76 },
        { 0xA3, 0x1C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetPlayRegion
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetPlayRegion, 4039, 13,

    XREF_DSBUFFERSETPLAYREGIONA,
    XRefZero)

        { 0x00, 0x55 },
        { 0x18, 0x68 },
        { 0x23, 0xB8 },

        { 0x37, 0x14 },
        { 0x38, 0x8B },
        { 0x39, 0x75 },
        { 0x3A, 0x0C },
        { 0x3B, 0x03 },
        { 0x3C, 0xF0 },
        { 0x3D, 0x3B },
        { 0x3E, 0xB1 },

        { 0x7C, 0xC2 },
        { 0x7D, 0x0C },
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_SetCurrentPosition
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_SetCurrentPosition, 4039, 9,

	XREF_CMcpxBuffer_SetCurrentPosition,
    XRefZero)

        // CMcpxBuffer_SetCurrentPosition+0x11 : lea eax, [esi+90h]
        { 0x11, 0x8D },
        { 0x12, 0x86 },
        { 0x13, 0x90 },

        // CMcpxBuffer_SetCurrentPosition+0x43 : jb +0x10
        { 0x43, 0x72 },
        { 0x44, 0x10 },

        // CMcpxBuffer_SetCurrentPosition+0x86 : mov eax, [esi+0E4h]
        { 0x6A, 0x8B },
        { 0x6B, 0x86 },
        { 0x6C, 0xE4 },
        { 0x6D, 0x00 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_SetCurrentPosition
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetCurrentPosition, 4039, 11,

	XREF_CDirectSoundBuffer_SetCurrentPosition,
    XRefOne)

        // CDirectSoundBuffer_SetCurrentPosition+0x34 : call [CMcpxBuffer::SetCurrentPosition]
        XREF_ENTRY( 0x34, XREF_CMcpxBuffer_SetCurrentPosition), 

        // CDirectSoundBuffer_SetCurrentPosition+0x00 : push esi
        { 0x00, 0x56 },

        // CDirectSoundBuffer_SetCurrentPosition+0x20 : mov eax, 0x80004005
        { 0x20, 0xB8 },
        { 0x21, 0x05 },
        { 0x22, 0x40 },
        { 0x23, 0x00 },
        { 0x24, 0x80 },

        // CDirectSoundBuffer_SetCurrentPosition+0x3C : jz +0x0B
        { 0x3C, 0x74 },
        { 0x3D, 0x0B },

        // CDirectSoundBuffer_SetCurrentPosition+0x4D : retn 0x08
        { 0x4D, 0xC2 },
        { 0x4E, 0x08 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CMcpxVoiceClient::SetFilter
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_SetFilter, 4039, 12,

    XREF_CMcpxVoiceClient_SetFilter,
    XRefZero)

        { 0x08, 0xF8 },
        { 0x12, 0xE8 },

        { 0x20, 0x00 },
        { 0x21, 0x74 },
        { 0x22, 0x13 },
        { 0x23, 0x6A },
        { 0x24, 0x06 },
        { 0x25, 0x59 },
        { 0x26, 0x8D },
        { 0x27, 0x7D },

        { 0x47, 0x83 },
        { 0x4D, 0x8B },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetFilter
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetFilter, 4039, 7,

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
// * CDirectSoundBuffer::SetFilter
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetFilter, 4039, 8,

    XREF_CDirectSoundBuffer_SetFilter,
    XRefOne)

        XREF_ENTRY( 0x31, XREF_CDirectSoundVoice_SetFilter ),

        { 0x00, 0x56 },
        { 0x11, 0x85 },
        { 0x1B, 0x15 },
        { 0x25, 0xEB },
        { 0x2F, 0x10 },
        { 0x39, 0x74 },
        { 0x46, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetFilter
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetFilter, 4039, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetFilter+0x15 : call [CDirectSoundBuffer_SetFilter]
        XREF_ENTRY( 0x15, XREF_CDirectSoundBuffer_SetFilter),

        // IDirectSoundBuffer_SetFilter+0x04 : push [esp+0x08]
        { 0x04, 0xFF },
        { 0x05, 0x74 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },

        // IDirectSoundBuffer_SetFilter+0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 },
        { 0x0B, 0xC0 },
        { 0x0C, 0xE4 },

        // IDirectSoundBuffer_SetFilter+0x19 : retn 0x08
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream::SetFilter
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetFilter, 4039, 8,

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

// ******************************************************************
// * CDirectSoundVoiceSettings::SetMixBins
// ******************************************************************
OOVPA_XREF(CDirectSoundVoiceSettings_SetMixBins, 4039, 13,

        XREF_DSSETMIXBINSC,
        XRefZero)

        // CDirectSoundVoiceSettings_SetMixBins+0x06 : jnz +0x19
        { 0x06, 0x75 },
        { 0x07, 0x19 },

        // CDirectSoundVoiceSettings_SetMixBins+0x08 : test byte ptr [ecx+8], 0x10
        { 0x08, 0xF6 },
        { 0x09, 0x41 },
        { 0x0A, 0x08 },
        { 0x0B, 0x10 },

        // CDirectSoundVoiceSettings_SetMixBins+0x18 : shr eax, 1
        { 0x18, 0xD1 },
        { 0x19, 0xE8 },

        // CDirectSoundVoiceSettings_SetMixBins+0x4E : mov edx, [ecx+2Ch]
        { 0x4E, 0x8B },
        { 0x4F, 0x51 },
        { 0x50, 0x2C },

        // CDirectSoundVoiceSettings_SetMixBins+0x51 : cmp edx, [eax]
        { 0x51, 0x3B },
        { 0x52, 0x10 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetMixBins
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMixBins, 4039, 17,

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
OOVPA_XREF(CDirectSoundBuffer_SetMixBins, 4039, 18,

    XREF_DSSETMIXBINSA,
    XRefOne)

        // CDirectSoundBuffer_SetMixBins+0x31 : call [CDirectSoundVoice::SetMixBins]
        XREF_ENTRY( 0x31, XREF_DSSETMIXBINSB ),

        // CDirectSoundBuffer_SetMixBins+0x00 : push esi
        { 0x00, 0x56 },

        // CDirectSoundBuffer_SetMixBins+0x20 : mov eax, 0x80004005
        { 0x20, 0xB8 },
        { 0x21, 0x05 },
        { 0x22, 0x40 },
        { 0x23, 0x00 },
        { 0x24, 0x80 },

        // CDirectSoundBuffer_SetMixBins+0x29 : push [esp+0x10]; push [esp+0x10]
        { 0x28, 0xFF },
        { 0x29, 0x74 },
        { 0x2A, 0x24 },
        { 0x2B, 0x10 },
        { 0x2C, 0xFF },
        { 0x2D, 0x74 },
        { 0x2E, 0x24 },
        { 0x2F, 0x10 },

        // CDirectSoundBuffer_SetMixBins+0x48 : pop edi
        { 0x48, 0x5F },

        // CDirectSoundBuffer_SetMixBins+0x4A : retn 0x08
        { 0x4A, 0xC2 },
        { 0x4B, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoiceSettings::SetMixBinVolumes
// ******************************************************************
OOVPA_XREF(CDirectSoundVoiceSettings_SetMixBinVolumes, 4039, 10,

    XREF_DSSETMIXBINVOLUMESC,
    XRefZero)

        // CDirectSoundVoiceSettings_SetMixBinVolumes+0x09 : jbe +0x16
        { 0x09, 0x76 },
        { 0x0A, 0x16 },

        // CDirectSoundVoiceSettings_SetMixBinVolumes+0x18 : mov [ecx+edi*4+0x50], eax
        { 0x18, 0x89 },
        { 0x19, 0x44 },
        { 0x1A, 0xB9 },
        { 0x1B, 0x50 }, // 4039 0x50 vs 4134 0x30

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
OOVPA_XREF(CDirectSoundVoice_SetMixBinVolumes, 4039, 17,

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
OOVPA_XREF(CDirectSoundBuffer_SetMixBinVolumes, 4039, 18,

    XREF_DSSETMIXBINVOLUMESA,
    XRefOne)

        // CDirectSoundBuffer_SetMixBinVolumes+0x31 : call [CDirectSoundVoice::SetMixBinVolumes]
        XREF_ENTRY( 0x31, XREF_DSSETMIXBINVOLUMESB ),

        // CDirectSoundBuffer_SetMixBinVolumes+0x00 : push esi
        { 0x00, 0x56 },

        // CDirectSoundBuffer_SetMixBinVolumes+0x20 : mov eax, 0x80004005
        { 0x20, 0xB8 },
        { 0x21, 0x05 },
        { 0x22, 0x40 },
        { 0x23, 0x00 },
        { 0x24, 0x80 },

        // CDirectSoundBuffer_SetMixBinVolumes+0x28 : push [esp+0x10]; push [esp+0x10]
        { 0x28, 0xFF },
        { 0x29, 0x74 },
        { 0x2A, 0x24 },
        { 0x2B, 0x10 },
        { 0x2C, 0xFF },
        { 0x2D, 0x74 },
        { 0x2E, 0x24 },
        { 0x2F, 0x10 },

        // CDirectSoundBuffer_SetMixBinVolumes+0x48 : pop edi
        { 0x48, 0x5F },

        // CDirectSoundBuffer_SetMixBinVolumes+0x4A : retn 0x08
        { 0x4A, 0xC2 },
        { 0x4B, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetMixBinVolumes2
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMixBinVolumes2, 4039, 12,

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
// * CDirectSound::CreateSoundStream
// ******************************************************************
OOVPA_XREF(CDirectSound_CreateSoundStream, 4039, 15,

    XREF_DSCREATESOUNDSTREAM,
    XRefZero)

        // CDirectSound_CreateSoundStream+0x25 : mov eax, 0x80004005
        { 0x25, 0xB8 },

        // CDirectSound_CreateSoundStream+0x2A : jmp 0x62
        { 0x2A, 0xEB },
        { 0x2B, 0x62 },

        // CDirectSound_CreateSoundStream+0x2C : push 0x1
        { 0x2C, 0x6A },
        { 0x2D, 0x01 },

        // CDirectSound_CreateSoundStream+0x2E : push 0x28
        { 0x2E, 0x6A },
        { 0x2F, 0x28 },

        // CDirectSound_CreateSoundStream+0x4A : add esi, 0x7FF8FFF2
        { 0x4C, 0x81 },
        { 0x4D, 0xE6 },
        { 0x4E, 0xF2 },
        { 0x4F, 0xFF },
        { 0x50, 0xF8 },
        { 0x51, 0x7F },

        // CDirectSound_CreateSoundStream+0x91 : retn 0x10
        { 0x91, 0xC2 },
        { 0x92, 0x10 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_StopEx
// ******************************************************************
// Similar pattern of version 3911
OOVPA_XREF(CDirectSoundBuffer_StopEx, 4039, 12,

    XREF_CDirectSoundBuffer_StopEx,
    XRefZero)

        { 0x00, 0x55 },
        { 0x23, 0xB8 },

        { 0x49, 0xEB },
        { 0x4A, 0x11 },
        { 0x4B, 0xFF },
        { 0x4C, 0x75 },
        { 0x4D, 0x10 },
        { 0x4E, 0x8B },
        { 0x4F, 0x45 },
        { 0x50, 0x08 },

        { 0x72, 0xC2 },
        { 0x73, 0x10 },
OOVPA_END;

// ******************************************************************
// * CMcpxVoiceClient::SetLFO
// ******************************************************************
// Similar pattern of version 3911
OOVPA_XREF(CMcpxVoiceClient_SetLFO, 4039, 13,

    XREF_CMcpxVoiceClient_SetLFO,
    XRefZero)

        { 0x00, 0x55 },
        { 0x25, 0xEB },

        { 0x57, 0x72 },
        { 0x58, 0xF3 },
        { 0x59, 0x33 },
        { 0x5A, 0xC0 },
        { 0x5B, 0x85 },
        { 0x5C, 0xC9 },
        { 0x5D, 0x76 },
        { 0x5E, 0x62 },
        { 0x5F, 0x8B },

        { 0xCD, 0xC2 },
        { 0xCE, 0x04 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetLFO
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetLFO, 4039, 7,

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
OOVPA_XREF(CDirectSoundBuffer_SetLFO, 4039, 18,

    XREF_CDirectSoundBuffer_SetLFO,
    XRefOne)

        // CDirectSoundBuffer_SetLFO+0x31 : call [CDirectSoundVoice_SetLFO]
        XREF_ENTRY( 0x31, XREF_CDirectSoundVoice_SetLFO),

        // CDirectSoundBuffer_SetLFO+0x00 : push esi
        { 0x00, 0x56 },

        // CDirectSoundBuffer_SetLFO+0x20 : mov eax, 0x80004005
        { 0x20, 0xB8 },
        { 0x21, 0x05 },
        { 0x22, 0x40 },
        { 0x23, 0x00 },
        { 0x24, 0x80 },

        // CDirectSoundBuffer_SetLFO+0x28 : push [esp+0x10]; push [esp+0x10]
        { 0x28, 0xFF },
        { 0x29, 0x74 },
        { 0x2A, 0x24 },
        { 0x2B, 0x10 },
        { 0x2C, 0xFF },
        { 0x2D, 0x74 },
        { 0x2E, 0x24 },
        { 0x2F, 0x10 },

        // CDirectSoundBuffer_SetLFO+0x48 : pop edi
        { 0x48, 0x5F },

        // CDirectSoundBuffer_SetLFO+0x4A : retn 0x08
        { 0x4A, 0xC2 },
        { 0x4B, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetLFO
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetLFO, 4039, 8,

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
// * CDirectSoundStream::SetLFO
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetLFO, 4039, 10,

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

// ******************************************************************
// * CMcpxVoiceClient::SetEG
// ******************************************************************
// Similar pattern of version 3911
OOVPA_XREF(CMcpxVoiceClient_SetEG, 4039, 12,

    XREF_CMcpxVoiceClient_SetEG,
    XRefZero)

        { 0x00, 0x55 },
        { 0x25, 0xEB },

        { 0x3C, 0x84 },
        { 0x3D, 0x86 },
        { 0x3E, 0x00 },
        { 0x3F, 0x00 },
        { 0x40, 0x00 },
        { 0x41, 0x8B },
        { 0x42, 0x86 },
        { 0x43, 0xE4 },

        { 0xD3, 0xC2 },
        { 0xD4, 0x04 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetEG
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetEG, 4039, 7,

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
// * CDirectSoundBuffer::SetEG
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetEG, 4039, 18,

    XREF_CDirectSoundBuffer_SetEG,
    XRefOne)

        // CDirectSoundBuffer_SetEG+0x31 : call [CDirectSoundVoice_SetEG]
        XREF_ENTRY( 0x31, XREF_CDirectSoundVoice_SetEG),

        // CDirectSoundBuffer_SetEG+0x00 : push esi
        { 0x00, 0x56 },

        // CDirectSoundBuffer_SetEG+0x20 : mov eax, 0x80004005
        { 0x20, 0xB8 },
        { 0x21, 0x05 },
        { 0x22, 0x40 },
        { 0x23, 0x00 },
        { 0x24, 0x80 },

        // CDirectSoundBuffer_SetEG+0x28 : push [esp+0x10]; push [esp+0x10]
        { 0x28, 0xFF },
        { 0x29, 0x74 },
        { 0x2A, 0x24 },
        { 0x2B, 0x10 },
        { 0x2C, 0xFF },
        { 0x2D, 0x74 },
        { 0x2E, 0x24 },
        { 0x2F, 0x10 },

        // CDirectSoundBuffer_SetEG+0x48 : pop edi
        { 0x48, 0x5F },

        // CDirectSoundBuffer_SetEG+0x4A : retn 0x08
        { 0x4A, 0xC2 },
        { 0x4B, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetEG
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetEG, 4039, 8,

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
// * CDirectSoundStream::SetEG
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetEG, 4039, 8,

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
// * CDirectSoundVoice::SetOutputBuffer
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetOutputBuffer, 4039, 7,

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
// * CDirectSoundBuffer_SetOutputBuffer
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetOutputBuffer, 4039, 18,

    XREF_CDirectSoundBuffer_SetOutputBuffer,
    XRefOne)

        // CDirectSoundBuffer_SetOutputBuffer+0x31 : call [XREF_CDirectSoundVoice_SetOutputBuffer]
        XREF_ENTRY( 0x31, XREF_CDirectSoundVoice_SetOutputBuffer),

        // CDirectSoundBuffer_SetOutputBuffer+0x00 : push esi
        { 0x00, 0x56 },

        // CDirectSoundBuffer_SetOutputBuffer+0x20 : mov eax, 0x80004005
        { 0x20, 0xB8 },
        { 0x21, 0x05 },
        { 0x22, 0x40 },
        { 0x23, 0x00 },
        { 0x24, 0x80 },

        // CDirectSoundBuffer_SetOutputBuffer+0x28 : push [esp+0x10]; push [esp+0x10]
        { 0x28, 0xFF },
        { 0x29, 0x74 },
        { 0x2A, 0x24 },
        { 0x2B, 0x10 },
        { 0x2C, 0xFF },
        { 0x2D, 0x74 },
        { 0x2E, 0x24 },
        { 0x2F, 0x10 },

        // CDirectSoundBuffer_SetOutputBuffer+0x48 : pop edi
        { 0x48, 0x5F },

        // CDirectSoundBuffer_SetOutputBuffer+0x4A : retn 0x08
        { 0x4A, 0xC2 },
        { 0x4B, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetConeOutsideVolume
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetConeOutsideVolume, 4039, 15,

    XREF_DSVOICESETCONEOUTSIDEVOLUME,
    XRefZero)

        // CDirectSoundVoice_SetConeOutsideVolume+0x07 : mov edx, [esp+arg_4]
        { 0x07, 0x8B },
        { 0x08, 0x54 },
        { 0x09, 0x24 },
        { 0x0A, 0x08 },

        // CDirectSoundVoice_SetConeOutsideVolume+0x11 : mov [eax+104h], edx
        { 0x0B, 0x89 },
        { 0x0C, 0x90 },
        { 0x0D, 0x04 },
        { 0x0E, 0x01 },
        { 0x0F, 0x00 },
        { 0x10, 0x00 },

        { 0x11, 0x8B },
        { 0x14, 0x83 },
        { 0x1A, 0x10 },

        // CDirectSoundVoice_SetConeOutsideVolume+0x2A : retn 0Ch
        { 0x2A, 0xC2 },
        { 0x2B, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetConeOutsideVolume
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetConeOutsideVolume, 4039, 12,

    XREF_DSBUFFERSETCONEOUTSIDEVOLUME,
    XRefOne)

        // CDirectSoundBuffer_SetConeOutsideVolume+0x35 : call [CDirectSoundVoice::SetConeOutsideVolume]
        XREF_ENTRY( 0x35, XREF_DSVOICESETCONEOUTSIDEVOLUME ),

        // CDirectSoundBuffer_SetConeOutsideVolume+0x00 : push esi
        { 0x00, 0x56 },

        // CDirectSoundBuffer_SetConeOutsideVolume+0x21 : mov eax, 80004005h
        { 0x21, 0x05 },
        { 0x22, 0x40 },
        { 0x23, 0x00 },
        { 0x24, 0x80 },

        // CDirectSoundBuffer_SetConeOutsideVolume+0x30 : push [esp+0Ch+arg_0]
        { 0x30, 0xFF },
        { 0x31, 0x74 },
        { 0x32, 0x24 },
        { 0x33, 0x14 },

        // CDirectSoundBuffer_SetConeOutsideVolume+0x4E : retn 0Ch
        { 0x4E, 0xC2 },
        { 0x4F, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetMaxDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMaxDistance, 4039, 14,

    XREF_CDirectSoundVoice_SetMaxDistance,
    XRefZero)

        // CDirectSoundVoice_SetMaxDistance+0x07 : mov edx, [esp+arg_4]
        { 0x07, 0x8B },
        { 0x08, 0x54 },
        { 0x09, 0x24 },
        { 0x0A, 0x08 },

        // CDirectSoundVoice_SetMaxDistance+0x0B : mov [eax+10h], edx
        { 0x0B, 0x89 },
        { 0x0C, 0x90 },
        { 0x0D, 0x08 }, // SetMaxDistance 0x0C VS SetMinDistance 0x08

        // CDirectSoundVoice_SetMaxDistance+0x14 : or dword ptr [eax+138h], 4
        { 0x14, 0x83 },
        { 0x15, 0x88 },
        { 0x18, 0x00 },
        { 0x19, 0x00 },
        { 0x1A, 0x04 },

        // CDirectSoundVoice_SetMaxDistance+0x2A : retn 0Ch
        { 0x2A, 0xC2 },
        { 0x2B, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetMaxDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetMaxDistance, 4039, 11,

    XREF_CDirectSoundBuffer_SetMaxDistance,
    XRefOne)

        // CDirectSoundBuffer_SetMaxDistance+0x39 : call [CDirectSoundVoice_SetMaxDistance]
        XREF_ENTRY( 0x39, XREF_CDirectSoundVoice_SetMaxDistance ),

        // CDirectSoundBuffer_SetMaxDistance+0x00 : push esi
        { 0x00, 0x56 },

        // CDirectSoundBuffer_SetMaxDistance+0x21 : mov eax, 80004005h
        { 0x21, 0x05 },
        { 0x22, 0x40 },
        { 0x23, 0x00 },
        { 0x24, 0x80 },

        // CDirectSoundBuffer_SetMaxDistance+0x31 : fstp [esp+0Ch+var_C]
        { 0x31, 0xD9 },
        { 0x32, 0x1C },
        { 0x33, 0x24 },

        // CDirectSoundBuffer_SetMaxDistance+0x52 : retn 0Ch
        { 0x53, 0x0C },
        { 0x54, 0x00 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetMinDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMinDistance, 4039, 14,

    XREF_CDirectSoundVoice_SetMinDistance,
    XRefZero)

        // CDirectSoundVoice_SetMinDistance+0x07 : mov edx, [esp+arg_4]
        { 0x07, 0x8B },
        { 0x08, 0x54 },
        { 0x09, 0x24 },
        { 0x0A, 0x08 },

        // CDirectSoundVoice_SetMinDistance+0x0B : mov [eax+10h], edx
        { 0x0B, 0x89 },
        { 0x0C, 0x90 },
        { 0x0D, 0x0C }, // SetMaxDistance 0x0C VS SetMinDistance 0x08

        // CDirectSoundVoice_SetMinDistance+0x14 : or dword ptr [eax+138h], 4
        { 0x14, 0x83 },
        { 0x15, 0x88 },
        { 0x18, 0x00 },
        { 0x19, 0x00 },
        { 0x1A, 0x04 },

        // CDirectSoundVoice_SetMinDistance+0x2A : retn 0Ch
        { 0x2A, 0xC2 },
        { 0x2B, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetMinDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetMinDistance, 4039, 11,

    XREF_CDirectSoundBuffer_SetMinDistance,
    XRefOne)

        // CDirectSoundBuffer_SetMinDistance+0x39 : call [CDirectSoundVoice_SetMinDistance]
        XREF_ENTRY( 0x39, XREF_CDirectSoundVoice_SetMinDistance ),

        // CDirectSoundBuffer_SetMinDistance+0x00 : push esi
        { 0x00, 0x56 },

        // CDirectSoundBuffer_SetMinDistance+0x21 : mov eax, 80004005h
        { 0x21, 0x05 },
        { 0x22, 0x40 },
        { 0x23, 0x00 },
        { 0x24, 0x80 },

        // CDirectSoundBuffer_SetMinDistance+0x31 : fstp [esp+0Ch+var_C]
        { 0x31, 0xD9 },
        { 0x32, 0x1C },
        { 0x33, 0x24 },

        // CDirectSoundBuffer_SetMinDistance+0x52 : retn 0Ch
        { 0x53, 0x0C },
        { 0x54, 0x00 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetMode
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMode, 4039, 12,

    XREF_DSBUFFERSETMODEB,
    XRefZero)

        { 0x00, 0xF6 },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x0C },
        { 0x04, 0x01 },
        { 0x05, 0x8B },
        { 0x06, 0x44 },
        { 0x07, 0x24 },

        { 0x16, 0x75 },
        { 0x19, 0xE8 },
        { 0x20, 0xC2 },
        { 0x21, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetMode
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetMode, 4039, 12,

    XREF_DSBUFFERSETMODEA,
    XRefOne)

        // CDirectSoundBuffer_SetMode+0x35 : call [CDirectSoundVoice::SetMode]
        XREF_ENTRY( 0x35, XREF_DSBUFFERSETMODEB ),

        // CDirectSoundBuffer_SetMode+0x00 : push esi
        { 0x00, 0x56 },

        // CDirectSoundBuffer_SetMode+0x21 : mov eax, 80004005h
        { 0x21, 0x05 },
        { 0x22, 0x40 },
        { 0x23, 0x00 },
        { 0x24, 0x80 },

        // CDirectSoundBuffer_SetMode+0x30 : push [esp+0Ch+arg_0]
        { 0x30, 0xFF },
        { 0x31, 0x74 },
        { 0x32, 0x24 },
        { 0x33, 0x14 },

        // CDirectSoundBuffer_SetMode+0x4E : retn 0Ch
        { 0x4E, 0xC2 },
        { 0x4F, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetPosition
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetPosition, 4039, 12,

    XREF_DSVOICESETPOSITION,
    XRefZero)

        { 0x05, 0x08 },
        { 0x1C, 0xDC },

        { 0x31, 0x00 },
        { 0x32, 0x00 },
        { 0x33, 0xFF }, // SetPosition 0xFF vs SetConeOrientation 0x40
        { 0x34, 0xF6 },
        { 0x35, 0x45 },
        { 0x36, 0x18 },
        { 0x37, 0x01 },
        { 0x38, 0x5E },

        { 0x44, 0xC2 },
        { 0x45, 0x14 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetPosition
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetPosition, 4039, 10,

    XREF_DSBUFFERSETPOSITION,
    XRefOne)

        // CDirectSoundBuffer_SetPosition+0x49 : call [CDirectSoundVoice::SetPosition]
        XREF_ENTRY( 0x49, XREF_DSVOICESETPOSITION ),

        // CDirectSoundBuffer_SetPosition+0x00 : push ebp
        { 0x00, 0x55 },

        // CDirectSoundBuffer_SetPosition+0x2A : fld [ebp+arg_C]
        { 0x2A, 0xD9 },
        { 0x2B, 0x45 },
        { 0x2C, 0x14 },

        // CDirectSoundBuffer_SetPosition+0x42 : fstp [esp+18h+var_18]
        { 0x42, 0xD9 },
        { 0x43, 0x1C },
        { 0x44, 0x24 },

        // CDirectSoundBuffer_SetPosition+0x63 : retn 14h
        { 0x63, 0xC2 },
        { 0x64, 0x14 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetVelocity
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetVelocity, 4039, 12,

    XREF_DSVOICESETVELOCITY,
    XRefZero)

        { 0x05, 0x08 },
        { 0x1C, 0xE8 },

        { 0x31, 0x00 },
        { 0x32, 0x00 },
        { 0x33, 0x40 }, // SetPosition 0xFF vs SetVelocity 0x40
        { 0x34, 0xF6 },
        { 0x35, 0x45 },
        { 0x36, 0x18 },
        { 0x37, 0x01 },
        { 0x38, 0x5E },

        { 0x44, 0xC2 },
        { 0x45, 0x14 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetVelocity
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetVelocity, 4039, 10,

    XREF_DSBUFFERSETVELOCITY,
    XRefOne)

        // CDirectSoundBuffer_SetVelocity+0x49 : call [CDirectSoundVoice::SetVelocity]
        XREF_ENTRY( 0x49, XREF_DSVOICESETVELOCITY ),

        // CDirectSoundBuffer_SetVelocity+0x00 : push ebp
        { 0x00, 0x55 },

        // CDirectSoundBuffer_SetVelocity+0x2A : fld [ebp+arg_C]
        { 0x2A, 0xD9 },
        { 0x2B, 0x45 },
        { 0x2C, 0x14 },

        // CDirectSoundBuffer_SetVelocity+0x42 : fstp [esp+18h+var_18]
        { 0x42, 0xD9 },
        { 0x43, 0x1C },
        { 0x44, 0x24 },

        // CDirectSoundBuffer_SetVelocity+0x63 : retn 14h
        { 0x63, 0xC2 },
        { 0x64, 0x14 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetConeOrientation
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetConeOrientation, 4039, 12,

    XREF_DSVOICESETCONEORIENTATION,
    XRefZero)

        { 0x05, 0x08 },
        { 0x1C, 0xFC },

        { 0x31, 0x00 },
        { 0x32, 0x00 },
        { 0x33, 0x18 }, // SetPosition 0xFF vs SetConeOrientation 0x40
        { 0x34, 0xF6 },
        { 0x35, 0x45 },
        { 0x36, 0x18 },
        { 0x37, 0x01 },
        { 0x38, 0x5E },

        { 0x44, 0xC2 },
        { 0x45, 0x14 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetConeOrientation
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetConeOrientation, 4039, 10,

    XREF_DSBUFFERSETCONEORIENTATION,
    XRefOne)

        // CDirectSoundBuffer_SetConeOrientation+0x49 : call [CDirectSoundVoice::SetConeOrientation]
        XREF_ENTRY( 0x49, XREF_DSVOICESETCONEORIENTATION ),

        // CDirectSoundBuffer_SetConeOrientation+0x00 : push ebp
        { 0x00, 0x55 },

        // CDirectSoundBuffer_SetConeOrientation+0x2A : fld [ebp+arg_C]
        { 0x2A, 0xD9 },
        { 0x2B, 0x45 },
        { 0x2C, 0x14 },

        // CDirectSoundBuffer_SetConeOrientation+0x42 : fstp [esp+18h+var_18]
        { 0x42, 0xD9 },
        { 0x43, 0x1C },
        { 0x44, 0x24 },

        // CDirectSoundBuffer_SetConeOrientation+0x63 : retn 14h
        { 0x63, 0xC2 },
        { 0x64, 0x14 },
OOVPA_END;
