// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->DSound.1.0.5344.cpp
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
// * DirectSound::CDirectSoundVoice::SetMinDistance
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetMinDistance, 5344, 7,

    XREF_DSVOICESETMINDISTANCE,
    XRefZero)

        { 0x05, 0x41 },
        { 0x0C, 0x00 },
        { 0x13, 0x38 },
        { 0x1A, 0x00 },
        { 0x21, 0xF6 },
        { 0x28, 0x51 },
        { 0x2F, 0xC0 },
OOVPA_END;
// ******************************************************************
// * DirectSound_CDirectSoundBuffer::SetMinDistance
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetMinDistance, 5344, 11,

    XREF_DSBUFFERSETMINDISTANCE,
    XRefOne)

        // DirectSound_CDirectSoundBuffer_SetMinDistance+0x39 : call [CDirectSoundVoice::SetMinDistance]
        XREF_ENTRY( 0x3A, XREF_DSVOICESETMINDISTANCE ),

        { 0x00, 0x56 },
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
// * DirectSound::CDirectSoundStream::SetMinDistance
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundStream_SetMinDistance, 5344, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x3E, XREF_DSVOICESETMINDISTANCE ),

        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x24 },
        { 0x3A, 0x1C },
        { 0x46, 0x74 },
        { 0x53, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetMinDistance
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMinDistance, 5344, 11,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetMinDistance+0x1C : call [CDirectSoundBuffer::SetMinDistance]
        XREF_ENTRY( 0x1D, XREF_DSBUFFERSETMINDISTANCE ),

        { 0x00, 0xFF },
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
// * DirectSound::CDirectSound::SetI3DL2Listener
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_SetI3DL2Listener, 5344, 8,

    XREF_DSSETI3DL2LISTENER,
    XRefZero)

        { 0x1E, 0x15 },
        { 0x3E, 0x88 },
        { 0x5E, 0x41 },
        { 0x7E, 0x00 },
        { 0x9E, 0x08 },
        { 0xBE, 0x41 },
        { 0xDE, 0x00 },
        { 0xFE, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetI3DL2Listener
// ******************************************************************
OOVPA_XREF(IDirectSound_SetI3DL2Listener, 5344, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_DSSETI3DL2LISTENER ),

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
// * DirectSound_CDirectSoundVoice::SetDistanceFactor
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetDistanceFactor, 5344, 7,

    XREF_DSVOICESETDISTANCEFACTOR,
    XRefZero)

        { 0x05, 0x41 },
        { 0x0C, 0x00 },
        { 0x13, 0x44 },
        { 0x1A, 0x00 },
        { 0x21, 0xF6 },
        { 0x28, 0x51 },
        { 0x2F, 0xC0 },
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundBuffer::SetDistanceFactor
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetDistanceFactor, 5344, 8,

    XREF_DSBUFFERSETDISTANCEFACTOR,
    XRefOne)

        XREF_ENTRY( 0x3A, XREF_DSVOICESETDISTANCEFACTOR ),

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xFF },
        { 0x40, 0x8B },
        { 0x4F, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetDistanceFactor
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetDistanceFactor, 5344, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_DSBUFFERSETDISTANCEFACTOR ),

        { 0x03, 0x0C },
        { 0x08, 0x8B },
        { 0x0D, 0x8B },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSound::SetDistanceFactor
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_SetDistanceFactor, 5344, 9,

    XREF_SETDISTANCEFACTORA,
    XRefZero)

        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x08 },
        { 0x35, 0x6C },
        { 0x3A, 0x48 },
        { 0x4A, 0x85 },
        { 0x53, 0xFF },
        { 0x5E, 0x00 },
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetDistanceFactor
// ******************************************************************
OOVPA_XREF(IDirectSound_SetDistanceFactor, 5344, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_SETDISTANCEFACTORA ),

        { 0x03, 0x0C },
        { 0x08, 0x8B },
        { 0x0D, 0x8B },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSound::SetDopplerFactor
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_SetDopplerFactor, 5344, 9,

    XREF_SETDOPPLERFACTOR,
    XRefZero)

        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x08 },
        { 0x35, 0x74 },
        { 0x3A, 0x48 },
        { 0x4A, 0x85 },
        { 0x53, 0xFF },
        { 0x5E, 0x00 },
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetDopplerFactor
// ******************************************************************
OOVPA_XREF(IDirectSound_SetDopplerFactor, 5344, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_SETDOPPLERFACTOR ),

        { 0x03, 0x0C },
        { 0x08, 0x8B },
        { 0x0D, 0x8B },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;


// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetMaxDistance
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetMaxDistance, 5344, 7,

    XREF_DSVOICESETMAXDISTANCE,
    XRefZero)

        { 0x05, 0x41 },
        { 0x0C, 0x00 },
        { 0x13, 0x3C },
        { 0x1A, 0x00 },
        { 0x21, 0xF6 },
        { 0x28, 0x51 },
        { 0x2F, 0xC0 },
OOVPA_END;
// ******************************************************************
// * DirectSound_CDirectSoundBuffer::SetMaxDistance
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetMaxDistance, 5344, 11,

    XREF_DSBUFFERSETMAXDISTANCE,
    XRefOne)

        // DirectSound_CDirectSoundBuffer_SetMaxDistance+0x39 : call [CDirectSoundVoice::SetMaxDistance]
        XREF_ENTRY( 0x3A, XREF_DSVOICESETMAXDISTANCE ),

        { 0x00, 0x56 },
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
OOVPA_XREF(IDirectSoundBuffer_SetMaxDistance, 5344, 11,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetMaxDistance+0x1C : call [CDirectSoundBuffer::SetMaxDistance]
        XREF_ENTRY( 0x1D, XREF_DSBUFFERSETMAXDISTANCE ),

        { 0x00, 0xFF },
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
// * DirectSound::CDirectSound::SetRolloffFactor
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_SetRolloffFactor, 5344, 9,

    XREF_SETROLLOFFFACTORA,
    XRefZero)

        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x08 },
        { 0x35, 0x70 },
        { 0x3A, 0x48 },
        { 0x4A, 0x85 },
        { 0x53, 0xFF },
        { 0x5E, 0x00 },
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetRolloffFactor
// ******************************************************************
OOVPA_XREF(IDirectSound_SetRolloffFactor, 5344, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_SETROLLOFFFACTORA ),

        { 0x03, 0x0C },
        { 0x08, 0x8B },
        { 0x0D, 0x8B },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// DirectSound_CDirectSoundVoice::SetMode
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetMode, 5344, 8,

    XREF_DSBUFFERSETMODEB,
    XRefZero)

        { 0x09, 0xB4 }, // (Offset,Value)-Pair #1
        { 0x10, 0x08 }, // (Offset,Value)-Pair #2
        { 0x13, 0x40 }, // (Offset,Value)-Pair #3
        { 0x15, 0x41 }, // (Offset,Value)-Pair #4
        { 0x18, 0x80 }, // (Offset,Value)-Pair #5
        { 0x19, 0xB4 }, // (Offset,Value)-Pair #6
        { 0x2E, 0x33 }, // (Offset,Value)-Pair #7
        { 0x31, 0x0C }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSound::SetPositionA
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_SetPositionA, 5344, 10,

    XREF_DSSETPOSITIONA,
    XRefZero)

        // DirectSound_CDirectSound_SetPositionA+0x2B : mov ecx, [ebp+0x08]
        { 0x2B, 0x8B }, // (Offset,Value)-Pair #1
        { 0x2C, 0x4D }, // (Offset,Value)-Pair #2
        { 0x2D, 0x08 }, // (Offset,Value)-Pair #3

        // DirectSound_CDirectSound_SetPositionA+0x37 : mov [edx+0x3C], edi
        { 0x37, 0x89 }, // (Offset,Value)-Pair #4
        { 0x38, 0x7A }, // (Offset,Value)-Pair #5
        { 0x39, 0x3C }, // (Offset,Value)-Pair #6

        // DirectSound_CDirectSound_SetPositionA+0x4C : or dword ptr [eax+34h], 1
        { 0x4C, 0x83 }, // (Offset,Value)-Pair #7
        { 0x4D, 0x48 }, // (Offset,Value)-Pair #8
        { 0x4E, 0x34 }, // (Offset,Value)-Pair #9
        { 0x4F, 0x01 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSound::SetVelocity
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_SetVelocity, 5344, 14,

    XREF_DSSETVELOCITYA,
    XRefZero)

        // DirectSound_CDirectSound_SetVelocity+0x24 : mov eax, 0x80004005
        { 0x24, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x25, 0x05 }, // (Offset,Value)-Pair #2
        { 0x26, 0x40 }, // (Offset,Value)-Pair #3
        { 0x27, 0x00 }, // (Offset,Value)-Pair #4
        { 0x28, 0x80 }, // (Offset,Value)-Pair #5

        // DirectSound_CDirectSound_SetVelocity+0x37 : mov [edx+0x48], edi
        { 0x37, 0x89 }, // (Offset,Value)-Pair #6
        { 0x38, 0x7A }, // (Offset,Value)-Pair #7
        { 0x39, 0x48 }, // (Offset,Value)-Pair #8

        // DirectSound_CDirectSound_SetVelocity+0x4C : or dword ptr [eax+34h], 2
        { 0x4C, 0x83 }, // (Offset,Value)-Pair #9
        { 0x4D, 0x48 }, // (Offset,Value)-Pair #10
        { 0x4E, 0x34 }, // (Offset,Value)-Pair #11
        { 0x4F, 0x02 }, // (Offset,Value)-Pair #12

        // DirectSound_CDirectSound_SetVelocity+0x70 : retn 0x14
        { 0x70, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x71, 0x14 }, // (Offset,Value)-Pair #14
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundVoiceSettings::SetMixBins
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoiceSettings_SetMixBins, 5344, 7,

    XREF_DSSETMIXBINSC,
    XRefZero)

        { 0x10, 0x10 },
        { 0x11, 0x74 },
        { 0x12, 0x07 },
        { 0x13, 0xB8 },
        { 0x3C, 0x50 },
        { 0x4C, 0x0F },
        { 0x5C, 0x46 },
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundVoice::SetPosition
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetPosition, 5344, 8,

    XREF_DSVOICESETPOSITION,
    XRefZero)

        // DirectSound_CDirectSoundVoice_SetPosition+0x0F : mov edx, [ebp+arg_4]
        { 0x0F, 0x8B },
        { 0x10, 0x55 },
        { 0x11, 0x0C },

        // DirectSound_CDirectSoundVoice_SetPosition+0x12 : mov [ecx+08h], edx
        { 0x12, 0x89 },
        { 0x13, 0x51 },
        { 0x14, 0x08 },

        // DirectSound_CDirectSoundVoice_SetPosition+0x50 : retn 14h
        { 0x50, 0x14 },
        { 0x51, 0x00 }
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundVoice::SetVelocity
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetVelocity, 5344, 8,

    XREF_DSVOICESETVELOCITY,
    XRefZero)

        // DirectSound_CDirectSoundVoice_SetVelocity+0x0F : mov edx, [ebp+arg_4]
        { 0x0F, 0x8B },
        { 0x10, 0x55 },
        { 0x11, 0x0C },

        // DirectSound_CDirectSoundVoice_SetVelocity+0x12 : mov [ecx+14h], edx
        { 0x12, 0x89 },
        { 0x13, 0x51 },
        { 0x14, 0x14 },

        // DirectSound_CDirectSoundVoice_SetVelocity+0x50 : retn 14h
        { 0x50, 0x14 },
        { 0x51, 0x00 }
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSound::SynchPlayback
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSound_SynchPlayback, 5344, 8,

    XREF_DSSYNCHPLAYBACKA,
    XRefZero)

        { 0x07, 0x3D },
        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x2D, 0x48 },
        { 0x2E, 0x0C },
        { 0x2F, 0x57 },
        { 0x36, 0xF6 },
        { 0x3B, 0x68 },
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSoundVoice::SetI3DL2Source
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetI3DL2Source, 5344, 10,

    XREF_DSVOICESETI3DL2SOURCE,
    XRefZero)

        // DirectSound_CDirectSoundVoice_SetI3DL2Source+0x12 : mov esi, [ecx]
        { 0x12, 0x8B },
        { 0x13, 0x31 },

        // DirectSound_CDirectSoundVoice_SetI3DL2Source+0x14 : mov [edx+80h], esi
        { 0x14, 0x89 },
        { 0x15, 0xB2 },
        { 0x16, 0x80 },

        // DirectSound_CDirectSoundVoice_SetI3DL2Source+0x50 : fld dword ptr [ecx+10h]
        { 0x50, 0xD9 },
        { 0x51, 0x41 },
        { 0x52, 0x10 },

        // DirectSound_CDirectSoundVoice_SetI3DL2Source+0xC7 : retn 0Ch
        { 0xC7, 0xC2 },
        { 0xC8, 0x0C }
OOVPA_END;

// ******************************************************************
// * DirectSound_CDirectSound::CommitDeferredSettings
// ******************************************************************
OOVPA_NO_XREF(DirectSound_CDirectSound_CommitDeferredSettings, 5344, 11)

        // DirectSound_CDirectSound_CommitDeferredSettings+0x11 : movzx eax, al
        { 0x11, 0x0F }, // (Offset,Value)-Pair #1
        { 0x12, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x13, 0xC0 }, // (Offset,Value)-Pair #3

        // DirectSound_CDirectSound_CommitDeferredSettings+0x27 : mov eax, 0x80004005
        { 0x28, 0xB8 }, // (Offset,Value)-Pair #4
        { 0x29, 0x05 }, // (Offset,Value)-Pair #5
        { 0x2A, 0x40 }, // (Offset,Value)-Pair #6
        { 0x2C, 0x80 }, // (Offset,Value)-Pair #7

        // DirectSound_CDirectSound_CommitDeferredSettings+0x6D : mov [eax+0x34], edi
        { 0x6E, 0x89 }, // (Offset,Value)-Pair #8
        { 0x6F, 0x78 }, // (Offset,Value)-Pair #9
        { 0x70, 0x34 }, // (Offset,Value)-Pair #10

        // DirectSound_CDirectSound_CommitDeferredSettings+0x97 : leave
        { 0x97, 0xC9 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * Direct, 5344
// ******************************************************************
OOVPATable DSound_5344[] = {

	REGISTER_OOVPA(DirectSoundCreate, 4361, PATCH),
	REGISTER_OOVPA(DirectSoundDoWork, 4627, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_CreateSoundBuffer, 4627, XREF),
	REGISTER_OOVPA(CMcpxAPU_SynchPlayback, 5233, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSound_SynchPlayback, 5344, XREF),
	REGISTER_OOVPA(IDirectSound_SynchPlayback, 5233, PATCH),
	REGISTER_OOVPA(CMcpxStream_Pause, 5233, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_Pause, 4361, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetI3DL2Listener, 4432, DISABLED),
	REGISTER_OOVPA(IDirectSound_SetI3DL2Listener, 4432, DISABLED),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetVelocity, 5344, XREF),
	REGISTER_OOVPA(IDirectSound_SetVelocity, 3936, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoiceSettings_SetMixBins, 5344, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetMixBins, 4432, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetMixBins, 4432, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMixBins, 4432, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetMixBins, 5233, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetPositionA, 5344, XREF),
	REGISTER_OOVPA(IDirectSound_SetPosition, 3936, PATCH),
	REGISTER_OOVPA(DirectSoundCreateBuffer, 4627, PATCH),
	REGISTER_OOVPA(IDirectSound_CreateSoundBuffer, 4627, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetFrequency, 4627, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetFrequency, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetFrequency, 4627, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetFrequency, 5233, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_SetVolume, 4627, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetVolume, 5233, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetVolume, 5233, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetVolume, 5233, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetVolume, 4627, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_LockA, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Lock, 3936, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_CreateSoundStream, 4361, XREF),
	REGISTER_OOVPA(DirectSoundCreateStream, 4361, PATCH),
	REGISTER_OOVPA(IDirectSound_Release, 3936, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_SetBufferData, 4361, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetBufferData, 4361, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetBufferData, 4361, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_GetStatusB, 4627, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_GetStatus, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_GetStatus, 4627, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_GetCurrentPosition, 4361, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_GetCurrentPosition, 4361, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_GetCurrentPosition, 3936, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_Play, 4928, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_Play, 4361, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Play, 4361, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_FlushEx, 5233, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_StopEx, 5233, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_EnableHeadphones, 5233, XREF),
	REGISTER_OOVPA(IDirectSound8_EnableHeadphones, 5233, PATCH),
	REGISTER_OOVPA(IDirectSound_DownloadEffectsImage, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound_SetOrientation, 3936, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetDistanceFactor, 5344, XREF),
	REGISTER_OOVPA(IDirectSound_SetDistanceFactor, 5344, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetDistanceFactorA, 4627, XREF),
	REGISTER_OOVPA(IDirectSound_SetDistanceFactor, 4432, DISABLED),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetRolloffFactor, 4627, DISABLED),
	REGISTER_OOVPA(IDirectSound_SetRolloffFactor, 4627, DISABLED),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetDopplerFactor, 5344, XREF),
	REGISTER_OOVPA(IDirectSound_SetDopplerFactor, 5344, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetDopplerFactor, 4627, DISABLED),
	REGISTER_OOVPA(IDirectSound_SetDopplerFactor, 4627, DISABLED),
	REGISTER_OOVPA(DirectSound_CDirectSound_CommitDeferredSettings, 5344, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetMaxDistance, 5344, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetMaxDistance, 5344, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMaxDistance, 5344, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetMinDistance, 5344, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetMinDistance, 5344, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetMinDistance, 5344, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMinDistance, 5344, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetMinDistance, 4432, DISABLED),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetMinDistance, 4432, DISABLED),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMinDistance, 4432, DISABLED),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetMaxDistance, 4432, DISABLED),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetMaxDistance, 4432, DISABLED),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMaxDistance, 4432, DISABLED),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetPosition, 5344, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetPosition, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetPosition, 3936, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetVelocity, 5344, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetVelocity, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetVelocity, 3936, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetI3DL2Source, 5344, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetI3DL2Source, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetI3DL2Source, 4627, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_Stop, 4627, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_Release, 3936, PATCH),
	REGISTER_OOVPA(IDirectSoundStream_SetVolume, 5233, ALIAS, DirectSound_CDirectSoundStream_SetVolume), // (This is actually the XREF we are using)
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetFormat, 5233, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetFormat, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetFormat, 4627, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetMode, 5233, DISABLED),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetMode, 5344, XREF), // Add when needed...
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetMode, 5233, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMode, 5233, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetRolloffCurve, 5233, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetRolloffCurve, 5233, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetRolloffCurve, 5233, PATCH),
	REGISTER_OOVPA(IDirectSound_AddRef, 3936, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetI3DL2Listener, 5344, XREF),
	REGISTER_OOVPA(IDirectSound_SetI3DL2Listener, 5344, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetRolloffFactor, 5344, XREF),
	REGISTER_OOVPA(IDirectSound_SetRolloffFactor, 5344, PATCH),
    REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetLoopRegion, 4361, XREF),
    REGISTER_OOVPA(IDirectSoundBuffer_SetLoopRegion, 4361, PATCH),
    REGISTER_OOVPA(CMcpxBuffer_SetCurrentPosition, 4361, XREF),
    REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetCurrentPosition, 4361, XREF),
    REGISTER_OOVPA(IDirectSoundBuffer_SetCurrentPosition, 4361, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetPitch, 4627, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetPitch, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetPitch, 4627, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetMixBinHeadroom, 4627, XREF),
	REGISTER_OOVPA(IDirectSound_SetMixBinHeadroom, 4627, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetHeadroom, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetHeadroom, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetHeadroom, 4134, PATCH),
	// REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetDistanceFactor, 5558, XREF), // Add when needed...
	// REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetDistanceFactor, 5558, XREF), // Add when needed...
	// REGISTER_OOVPA(IDirectSoundBuffer_SetDistanceFactor, 5558, PATCH), // Add when needed...
};

// ******************************************************************
// * DSound_5344_SIZE
// ******************************************************************
uint32 DSound_5344_SIZE = sizeof(DSound_5344);
