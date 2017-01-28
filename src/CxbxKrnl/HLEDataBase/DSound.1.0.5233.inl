// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->DSound.1.0.5233.cpp
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
// * DirectSound::CDirectSound::EnableHeadphones
// ******************************************************************
OOVPA_XREF(CDirectSound_EnableHeadphones, 5233, 8,

    XREF_DSENABLEHEADPHONES,
    XRefZero)

        { 0x16, 0x45 }, // (Offset,Value)-Pair #1
        { 0x1D, 0x0B }, // (Offset,Value)-Pair #2
        { 0x2A, 0x05 }, // (Offset,Value)-Pair #3
        { 0x46, 0x95 }, // (Offset,Value)-Pair #4
        { 0x56, 0x80 }, // (Offset,Value)-Pair #5
        { 0x5D, 0x7F }, // (Offset,Value)-Pair #6
        { 0x74, 0xA4 }, // (Offset,Value)-Pair #7
        { 0x99, 0x08 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * IDirectSound8_EnableHeadphones
// ******************************************************************
OOVPA_XREF(IDirectSound8_EnableHeadphones, 5233, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x15, XREF_DSENABLEHEADPHONES }, // (Offset,Value)-Pair #1

        // IDirectSound8_EnableHeadphones+0x0A : add eax, 0xFFFFFFF8
        { 0x0A, 0x83 }, // (Offset,Value)-Pair #2
        { 0x0B, 0xC0 }, // (Offset,Value)-Pair #3
        { 0x0C, 0xF8 }, // (Offset,Value)-Pair #4

        // IDirectSound8_EnableHeadphones+0x0F : sbb ecx, ecx
        { 0x0F, 0x1B }, // (Offset,Value)-Pair #5
        { 0x10, 0xC9 }, // (Offset,Value)-Pair #6

        // IDirectSound8_EnableHeadphones+0x19 : retn 0x08
        { 0x19, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x1A, 0x08 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_FlushEx
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_FlushEx, 5233, 11,

    XREF_DSFLUSHEX2,
    XRefZero)

        { 0x24, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x25, 0x05 }, // (Offset,Value)-Pair #2
        { 0x26, 0x40 }, // (Offset,Value)-Pair #3
        { 0x27, 0x00 }, // (Offset,Value)-Pair #4
        { 0x28, 0x80 }, // (Offset,Value)-Pair #5

        { 0x36, 0x74 }, // (Offset,Value)-Pair #6
        { 0x37, 0x12 }, // (Offset,Value)-Pair #7

        { 0x3E, 0xFF }, // (Offset,Value)-Pair #8
        { 0x40, 0x0C }, // (Offset,Value)-Pair #9

        { 0x67, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x68, 0x10 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_StopEx
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_StopEx, 5233, 9,

    XRefNoSaveIndex,
    XRefOne)

        { 0x11, XREF_DSFLUSHEX2 },  // (Offset,Value)-Pair #1

        { 0x00, 0xFF }, // (Offset,Value)-Pair #2
        { 0x03, 0x10 }, // (Offset,Value)-Pair #3

        { 0x04, 0xFF }, // (Offset,Value)-Pair #4
        { 0x07, 0x10 }, // (Offset,Value)-Pair #5

        { 0x08, 0xFF }, // (Offset,Value)-Pair #6
        { 0x0B, 0x10 }, // (Offset,Value)-Pair #7

        { 0x15, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x16, 0x10 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * CMcpxStream_Pause
// ******************************************************************
OOVPA_XREF(CMcpxStream_Pause, 5233, 11,

    XREF_DSSTREAMPAUSE,
    XRefZero)

        // CMcpxStream_Pause+0x17 : and ax, 0xFFDF
        { 0x17, 0x66 }, // (Offset,Value)-Pair #1
        { 0x18, 0x25 }, // (Offset,Value)-Pair #2
        { 0x19, 0xDF }, // (Offset,Value)-Pair #3

        // CMcpxStream_Pause+0x32 : jmp +0x56
        { 0x32, 0xEB }, // (Offset,Value)-Pair #4
        { 0x33, 0x56 }, // (Offset,Value)-Pair #5

        // CMcpxStream_Pause+0x39 : movzx eax, word ptr [esi+0x12]
        { 0x39, 0x0F }, // (Offset,Value)-Pair #6
        { 0x3A, 0xB7 }, // (Offset,Value)-Pair #7
        { 0x3B, 0x46 }, // (Offset,Value)-Pair #8
        { 0x3C, 0x12 }, // (Offset,Value)-Pair #9

        // CMcpxStream_Pause+0x8B : retn 0x04
        { 0x8B, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x8C, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// CMcpxAPU::SynchPlayback
// ******************************************************************
OOVPA_XREF(CMcpxAPU_SynchPlayback, 5233, 8,

    XREF_DSSYNCHPLAYBACKB,
    XRefZero)

        { 0x0C, 0x74 }, // (Offset,Value)-Pair #1
        { 0x1A, 0x1A }, // (Offset,Value)-Pair #2
        { 0x25, 0x53 }, // (Offset,Value)-Pair #3
        { 0x37, 0xF2 }, // (Offset,Value)-Pair #4
        { 0x58, 0xBC }, // (Offset,Value)-Pair #5
        { 0x84, 0x43 }, // (Offset,Value)-Pair #6
        { 0x9E, 0x64 }, // (Offset,Value)-Pair #7
        { 0xFF, 0x00 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * CDirectSound::SynchPlayback
// ******************************************************************
OOVPA_XREF(CDirectSound_SynchPlayback, 5233, 10,

    XREF_DSSYNCHPLAYBACKA,
    XRefOne)

        { 0x08, XREF_DSSYNCHPLAYBACKB }, // (Offset,Value)-Pair #1

        // CDirectSound_SynchPlayback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #2
        { 0x01, 0x44 }, // (Offset,Value)-Pair #3
        { 0x02, 0x24 }, // (Offset,Value)-Pair #4
        { 0x03, 0x04 }, // (Offset,Value)-Pair #5

        // CDirectSound_SynchPlayback+0x04 : mov ecx, [eax+0x0C]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #6
        { 0x05, 0x48 }, // (Offset,Value)-Pair #7
        { 0x06, 0x0C }, // (Offset,Value)-Pair #8

        // CDirectSound_SynchPlayback+0x0C : retn 0x04
        { 0x0C, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x0D, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSound8::SynchPlayback
// ******************************************************************
OOVPA_XREF(IDirectSound8_SynchPlayback, 5233, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x11, XREF_DSSYNCHPLAYBACKA }, // (Offset,Value)-Pair #1

        // IDirectSound8_SynchPlayback+0x06 : add eax, 0xFFFFFFF8
        { 0x06, 0x83 }, // (Offset,Value)-Pair #2
        { 0x07, 0xC0 }, // (Offset,Value)-Pair #3
        { 0x08, 0xF8 }, // (Offset,Value)-Pair #4

        // IDirectSound8_SynchPlayback+0x0B : sbb ecx, ecx
        { 0x0B, 0x1B }, // (Offset,Value)-Pair #5
        { 0x0C, 0xC9 }, // (Offset,Value)-Pair #6

        // IDirectSound8_SynchPlayback+0x15 : retn 0x04
        { 0x15, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x16, 0x04 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetFormat
// ******************************************************************
OOVPA_XREF(X_DirectSound_CDirectSoundVoice_SetFormat, 5233, 8,

    XREF_DirectSound_CDirectSoundVoice_SetFormat,
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
// CDirectSoundVoice::SetMode
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMode, 5233, 8,

    XREF_DSBUFFERSETMODEB,
    XRefZero)

        { 0x00, 0xF6 }, // (Offset,Value)-Pair #1
        { 0x07, 0x24 }, // (Offset,Value)-Pair #2
        { 0x0B, 0x10 }, // (Offset,Value)-Pair #3
        { 0x0E, 0xB4 }, // (Offset,Value)-Pair #4
        { 0x18, 0x3C }, // (Offset,Value)-Pair #5
        { 0x1A, 0x06 }, // (Offset,Value)-Pair #6
        { 0x21, 0x33 }, // (Offset,Value)-Pair #7
        { 0x24, 0x0C }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// CDirectSoundBuffer::SetMode
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetMode, 5233, 8,

    XREF_DSBUFFERSETMODEA,
    XRefOne)

        { 0x36, XREF_DSBUFFERSETMODEB }, // (Offset,Value)-Pair #1

        { 0x01, 0xE8 }, // (Offset,Value)-Pair #2
        { 0x15, 0x0B }, // (Offset,Value)-Pair #3
        { 0x22, 0x05 }, // (Offset,Value)-Pair #4
        { 0x27, 0x26 }, // (Offset,Value)-Pair #5
        { 0x34, 0x14 }, // (Offset,Value)-Pair #6
        { 0x3F, 0x0B }, // (Offset,Value)-Pair #7
        { 0x50, 0x0C }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetMode
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetMode, 5233, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x19, XREF_DSBUFFERSETMODEA }, // (Offset,Value)-Pair #1

        // IDirectSound8_EnableHeadphones+0x0E : add eax, 0xFFFFFFE4
        { 0x0E, 0x83 }, // (Offset,Value)-Pair #2
        { 0x0F, 0xC0 }, // (Offset,Value)-Pair #3
        { 0x10, 0xE4 }, // (Offset,Value)-Pair #4

        // IDirectSound8_EnableHeadphones+0x13 : sbb ecx, ecx
        { 0x13, 0x1B }, // (Offset,Value)-Pair #5
        { 0x14, 0xC9 }, // (Offset,Value)-Pair #6

        // IDirectSound8_EnableHeadphones+0x19 : retn 0x0C
        { 0x1D, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x1E, 0x0C }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetVolume
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetVolume, 5233, 8,

    XREF_DirectSound_CDirectSoundVoice_SetVolume,
    XRefOne)

        { 0x15, XREF_DSSTREAMSETVOLUME },

        { 0x02, 0x24 },
        { 0x06, 0x10 },
        { 0x0A, 0x08 },
        { 0x0E, 0x89 },
        { 0x12, 0x49 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetVolume
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetVolume, 5233, 8,

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
// * IDirectSoundBuffer8::SetVolume
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetVolume, 5233, 8,

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
// * CDirectSoundStream::SetVolume
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetVolume, 5233, 9,

    XRefNoSaveIndex,
    XRefOne)

        { 0x36, XREF_DirectSound_CDirectSoundVoice_SetVolume },

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
// * CDirectSoundStream::SetFrequency
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetFrequency, 5233, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x36, XREF_DSBUFFERSETFREQUENCYB },

        { 0x0E, 0xB6 },
        { 0x15, 0x0B },
        { 0x27, 0x26 },
        { 0x2A, 0x24 },
        { 0x3E, 0x74 },
        { 0x4D, 0x5F },
        { 0x50, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream::SetMixBins
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMixBins, 5233, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x36, XREF_DSSETMIXBINSB },

        { 0x0E, 0xB6 },
        { 0x15, 0x0B },
        { 0x27, 0x26 },
        { 0x2A, 0x24 },
        { 0x3E, 0x74 },
        { 0x4D, 0x5F },
        { 0x50, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetRolloffCurve
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetRolloffCurve, 5233, 8,

    XREF_DSVOICESETROLLOFFCURVE,
    XRefZero)

        { 0x07, 0x8B },
        { 0x10, 0x08 },
        { 0x12, 0x51 },
        { 0x19, 0xB4 },
        { 0x22, 0x51 },
        { 0x2B, 0x00 },
        { 0x34, 0x10 },
        { 0x3E, 0x33 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetRolloffCurve
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetRolloffCurve, 5233, 8,

    XREF_DSBUFFERSETROLLOFFCURVE,
    XRefOne)

        { 0x3A, XREF_DSVOICESETROLLOFFCURVE },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x74 },
        { 0x35, 0xFF },
        { 0x40, 0x8B },
        { 0x4F, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetRolloffCurve
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetRolloffCurve, 5233, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x1D, XREF_DSBUFFERSETROLLOFFCURVE },

        { 0x03, 0x10 },
        { 0x08, 0xFF },
        { 0x0D, 0xC8 },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetMaxDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMaxDistance, 5344, 7,

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
// * CDirectSoundBuffer::SetMaxDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetMaxDistance, 5344, 11,

    XREF_DSBUFFERSETMAXDISTANCE,
    XRefOne)

        // CDirectSoundBuffer_SetMaxDistance+0x39 : call [CDirectSoundVoice::SetMaxDistance]
        { 0x3A, XREF_DSVOICESETMAXDISTANCE },

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

// ******************************************************************
// * IDirectSoundBuffer_SetMaxDistance
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMaxDistance, 5344, 11,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetMaxDistance+0x1C : call [CDirectSoundBuffer::SetMaxDistance]
        { 0x1D, XREF_DSBUFFERSETMAXDISTANCE },

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
// * DirectSound::CDirectSoundVoice::SetMinDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMinDistance, 5344, 7,

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
// * CDirectSoundBuffer::SetMinDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetMinDistance, 5344, 11,

    XREF_DSBUFFERSETMINDISTANCE,
    XRefOne)

        // CDirectSoundBuffer_SetMinDistance+0x39 : call [CDirectSoundVoice::SetMinDistance]
        { 0x3A, XREF_DSVOICESETMINDISTANCE },

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

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetMinDistance
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundStream_SetMinDistance, 5344, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x3E, XREF_DSVOICESETROLLOFFCURVE },

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
        { 0x1D, XREF_DSBUFFERSETMINDISTANCE },

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

        { 0x1D, XREF_SETROLLOFFFACTORA },

        { 0x03, 0x0C },
        { 0x08, 0x8B },
        { 0x0D, 0x8B },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetDistanceFactor
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetDistanceFactor, 5344, 7,

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
// * CDirectSoundBuffer::SetDistanceFactor
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetDistanceFactor, 5344, 8,

    XREF_DSBUFFERSETDISTANCEFACTOR,
    XRefOne)

        { 0x3A, XREF_DSVOICESETDISTANCEFACTOR },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xFF },
        { 0x40, 0x8B },
        { 0x4F, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetDistanceFactor
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetDistanceFactor, 5344, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x1D, XREF_DSBUFFERSETDISTANCEFACTOR },

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

        { 0x1D, XREF_SETDISTANCEFACTORA },

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

        { 0x1D, XREF_SETDOPPLERFACTOR },

        { 0x03, 0x0C },
        { 0x08, 0x8B },
        { 0x0D, 0x8B },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * Direct, 5233
// ******************************************************************
OOVPATable DSound_5233[] = {

    // DirectSoundCreate (* unchanged since 4361 *)
	OOVPA_TABLE_ENTRY(DirectSoundCreate, 4361),
	// DirectSoundDoWork (* unchanged since 4627 *)
	OOVPA_TABLE_ENTRY(DirectSoundDoWork, 4627),
    // CDirectSound::CreateSoundBuffer (XREF) (* unchanged since 4627 *)
	OOVPA_TABLE_XREF(CDirectSound_CreateSoundBuffer, 4627),
    // CMcpxAPU::SynchPlayback (XRef)
	OOVPA_TABLE_XREF(CMcpxAPU_SynchPlayback, 5233),
    // CDirectSound::SynchPlayback (XRef)
	OOVPA_TABLE_XREF(CDirectSound_SynchPlayback, 5233),
	// IDirectSound_SynchPlayback
	OOVPA_TABLE_ENTRY(IDirectSound8_SynchPlayback, 5233),
    // CMcpxStream::Pause (XRef)
	OOVPA_TABLE_XREF(CMcpxStream_Pause, 5233),
	// CDirectSoundStream::Pause (* unchanged since 4361 *)
	OOVPA_TABLE_ENTRY(CDirectSoundStream_Pause, 4361),
    // CDirectSound_SetI3DL2Listener (XREF) (* unchanged since 4432 *)
	OOVPA_TABLE_XREF(CDirectSound_SetI3DL2Listener, 4432),
	// IDirectSound8_SetI3DL2Listener (* unchanged since 4432 *)
	OOVPA_TABLE_ENTRY(IDirectSound8_SetI3DL2Listener, 4432),
    // CDirectSound::SetVelocity (XREF) (* unchanged since 4627 *)
	OOVPA_TABLE_XREF(CDirectSound_SetVelocity, 4627),
	// IDirectSound8::SetVelocity (* unchanged since 3936 *)
	OOVPA_TABLE_ENTRY(IDirectSound8_SetVelocity, 3936),
    // CDirectSoundVoiceSettings::SetMixBins (* unchanged since 4432 *)
	OOVPA_TABLE_XREF(CDirectSoundVoiceSettings_SetMixBins, 4432),
    // CDirectSoundVoice::SetMixBins (* unchanged since 4432 *)
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetMixBins, 4432),
    // CDirectSoundBuffer::SetMixBins (* unchanged since 4432 *)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetMixBins, 4432),
	// IDirectSoundBuffer8::SetMixBins (* unchanged since 4432 *)
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetMixBins, 4432),
	// CDirectSoundStream::SetMixBins
	OOVPA_TABLE_ENTRY(CDirectSoundStream_SetMixBins, 5233),
    // CDirectSound::SetPositionA (XREF) (* unchanged since 4627 *)
	OOVPA_TABLE_XREF(CDirectSound_SetPositionA, 4627),
	// IDirectSound8::SetPosition (* unchanged since 3936 *)
	OOVPA_TABLE_ENTRY(IDirectSound8_SetPosition, 3936),
	// DirectSoundCreateBuffer (* unchanged since 4627 *)
	OOVPA_TABLE_ENTRY(DirectSoundCreateBuffer, 4627),
	// IDirectSound8::CreateSoundBuffer (* unchanged since 4627 *)
	OOVPA_TABLE_ENTRY(IDirectSound8_CreateSoundBuffer, 4627),
    // CDirectSoundVoice::SetFrequency (XREF) (* unchanged since 4627 *)
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetFrequency, 4627),
    // CDirectSoundBuffer::SetFrequency (XREF) (* unchanged since 4627 *)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetFrequency, 4627),
	// IDirectSoundBuffer8::SetFrequency (* unchanged since 4627 *)
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetFrequency, 4627),
	// CDirectSoundStream::SetFrequency
	OOVPA_TABLE_ENTRY(CDirectSoundStream_SetFrequency, 5233),
    // CMcpxVoiceClient_SetVolume (XREF) (* unchanged since 4627 *)
	OOVPA_TABLE_XREF(CMcpxVoiceClient_SetVolume, 4627),
    // CDirectSoundVoice::SetVolume (XREF)
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetVolume, 5233),
    // CDirectSoundBuffer::SetVolume (XREF)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetVolume, 5233),
	// IDirectSoundBuffer8::SetVolume
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetVolume, 5233),
	// CDirectSoundStream_SetVolume (* unchanged since 4627 *)
	OOVPA_TABLE_ENTRY(CDirectSoundStream_SetVolume, 4627),
    // IDirectSoundBuffer8::LockA (XREF)
	OOVPA_TABLE_XREF(IDirectSoundBuffer8_LockA, 4627),
	// IDirectSoundBuffer8::Lock (* unchanged since 3936 *)
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_Lock, 3936),
    // CDirectSound_CreateSoundStream (XREF) (* unchanged since 4361 *)
	OOVPA_TABLE_XREF(CDirectSound_CreateSoundStream, 4361),
	// DirectSoundCreateStream (* unchanged since 4361 *)
	OOVPA_TABLE_ENTRY(DirectSoundCreateStream, 4361),
	// IDirectSound8::Release (* unchanged since 3936 *)
	OOVPA_TABLE_ENTRY(IDirectSound8_Release, 3936),
    // CMcpxBuffer::SetBufferData (XREF) (* unchanged since 4361 *)
	OOVPA_TABLE_XREF(CMcpxBuffer_SetBufferData, 4361),
    // CDirectSoundBuffer::SetBufferData (XREF) (* unchanged since 4361 *)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetBufferData, 4361),
	// IDirectSoundBuffer8::SetBufferData (* unchanged since 4361 *)
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetBufferData, 4361),
    // CMcpxBuffer::GetStatusB (XRef) (* unchanged since 4627 *)
	OOVPA_TABLE_XREF(CMcpxBuffer_GetStatusB, 4627),
    // CDirectSoundBuffer::GetStatus (XRef) (* unchanged since 4627 *)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_GetStatus, 4627),
	// IDirectSoundBuffer8::GetStatus (* unchanged since 4627 *)
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_GetStatus, 4627),
    // CMcpxBuffer::GetCurrentPosition (XRef) (* unchanged since 4361 *)
	OOVPA_TABLE_XREF(CMcpxBuffer_GetCurrentPosition, 4361),
    // CMcpxBuffer::Play (XREF) (* unchanged since 4928 *)
	OOVPA_TABLE_XREF(CMcpxBuffer_Play, 4928),
    // CDirectSoundBuffer::Play (XREF) (* unchanged since 4361 *)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_Play, 4361),
	// IDirectSoundBuffer8::Play (* unchanged since 4361 *)
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_Play, 4361),
    // CDirectSoundStream::FlushEx (XREF)
	OOVPA_TABLE_XREF(CDirectSoundStream_FlushEx, 5233),
	// IDirectSoundBuffer8::StopEx
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_StopEx, 5233),
    // DirectSound::CDirectSound::EnableHeadphones (XRef)
	OOVPA_TABLE_XREF(CDirectSound_EnableHeadphones, 5233),
	// IDirectSound8::EnableHeadphones
	OOVPA_TABLE_ENTRY(IDirectSound8_EnableHeadphones, 5233),
	// IDirectSound8::DownloadEffectsImage (* unchanged since 3936 *)
	OOVPA_TABLE_ENTRY(IDirectSound8_DownloadEffectsImage, 3936),
	// IDirectSound8::SetOrientation (* unchanged since 3936 *)
	OOVPA_TABLE_ENTRY(IDirectSound8_SetOrientation, 3936),
	// CDirectSound::SetDistanceFactorA (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSound_SetDistanceFactor, 5344),
	// IDirectSound8::SetDistanceFactor
	OOVPA_TABLE_PATCH(IDirectSound_SetDistanceFactor, 5344, IDirectSound8_SetDistanceFactor),
    // CDirectSound::SetDistanceFactorA (XREF) (* unchanged since 4627 *)
	OOVPA_TABLE_XREF(CDirectSound_SetDistanceFactorA, 4627),
	// IDirectSound8::SetDistanceFactor (* unchanged since 4432 *)
	OOVPA_TABLE_ENTRY(IDirectSound8_SetDistanceFactor, 4432),
    // CDirectSound::SetRolloffFactor (XREF) (* unchanged since 4627 *)
	OOVPA_TABLE_XREF(CDirectSound_SetRolloffFactor, 4627),
	// IDirectSound8::SetRolloffFactor (* unchanged since 4627 *)
	OOVPA_TABLE_ENTRY(IDirectSound8_SetRolloffFactor, 4627),
	// CDirectSound::SetDopplerFactor (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSound_SetDopplerFactor, 5344),
	// IDirectSound8::SetDopplerFactor
	OOVPA_TABLE_PATCH(IDirectSound_SetDopplerFactor, 5344, IDirectSound8_SetDopplerFactor),
    // CDirectSound::SetDopplerFactor (XREF) (* unchanged since 4627 *)
	OOVPA_TABLE_XREF(CDirectSound_SetDopplerFactor, 4627),
	// IDirectSound8::SetDopplerFactor (* unchanged since 4627 *)
	OOVPA_TABLE_ENTRY(IDirectSound8_SetDopplerFactor, 4627),
	// CDirectSound::CommitDeferredSettings (* unchanged since 4432 *)
	OOVPA_TABLE_ENTRY(CDirectSound_CommitDeferredSettings, 4432),
	// CDirectSoundVoice::SetMaxDistance (XREF)
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetMaxDistance, 5344),
    // CDirectSoundBuffer::SetMaxDistance (XREF)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetMaxDistance, 5344),
	// IDirectSoundBuffer_SetMaxDistance
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetMaxDistance, 5344, IDirectSoundBuffer8_SetMaxDistance),
	// CDirectSoundVoice::SetMinDistance (XREF)
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetMinDistance, 5344),
    // CDirectSoundBuffer::SetMinDistance (XREF)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetMinDistance, 5344),
	// IDirectSoundBuffer_SetMinDistance
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetMinDistance, 5344, IDirectSoundBuffer8_SetMinDistance),
    // CDirectSoundVoice::SetMinDistance (XREF) (* unchanged since 4432 *)
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetMinDistance, 4432),
    // CDirectSoundBuffer::SetMinDistance (XREF) (* unchanged since 4432 *)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetMinDistance, 4432),
	// IDirectSoundBuffer_SetMinDistance (* unchanged since 4432 *)
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetMinDistance, 4432, IDirectSoundBuffer8_SetMinDistance),
    // CDirectSoundVoice::SetMaxDistance (XREF) (* unchanged since 4432 *)
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetMaxDistance, 4432),
    // CDirectSoundBuffer::SetMaxDistance (XREF) (* unchanged since 4432 *)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetMaxDistance, 4432),
	// IDirectSoundBuffer_SetMaxDistance (* unchanged since 4432 *)
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetMaxDistance, 4432, IDirectSoundBuffer8_SetMaxDistance),
    // CDirectSoundVoice::SetPosition (XREF) (* unchanged since 4627 *)
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetPosition, 4627),
    // CDirectSoundBuffer::SetPosition (XREF) (* unchanged since 4627 *)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetPosition, 4627),
	// IDirectSoundBuffer8_SetPosition (* unchanged since 3936 *)
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetPosition, 3936),
    // CDirectSoundVoice::SetVelocity (XREF) (* unchanged since 4627 *)
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetVelocity, 4627),
    // CDirectSoundBuffer::SetVelocity (XREF) (* unchanged since 4627 *)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetVelocity, 4627),
	// IDirectSoundBuffer8_SetVelocity (* unchanged since 3936 *)
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetVelocity, 3936),
    // CDirectSoundVoice::SetI3DL2Source (XREF) (* unchanged since 4627 *)
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetI3DL2Source, 4627),
    // CDirectSoundBuffer::SetI3DL2Source (XREF) (* unchanged since 4627 *)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetI3DL2Source, 4627),
	// IDirectSoundBuffer_SetI3DL2Source (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetI3DL2Source, 4627, IDirectSoundBuffer8_SetI3DL2Source),
	// IDirectSoundBuffer_Stop (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_Stop, 4627, IDirectSoundBuffer8_Stop),
	// IDirectSoundBuffer8::Release (* unchanged since 3936 *)
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_Release, 3936),
    // IDirectSoundStream::SetVolume
    // (This is actually the XREF we are using)
	OOVPA_TABLE_PATCH(CDirectSoundStream_SetVolume, 5233, IDirectSoundStream_SetVolume),
    // DirectSound::CDirectSoundVoice::SetFormat (XRef)
	OOVPA_TABLE_XREF(X_DirectSound_CDirectSoundVoice_SetFormat, 5233),
    // DirectSound::CDirectSoundBuffer::SetFormat (XRef) (* unchanged since 4627 *)
	OOVPA_TABLE_XREF(X_DirectSound_CDirectSoundBuffer_SetFormat, 4627),
    // IDirectSoundBuffer8_SetFormat (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(X_IDirectSoundBuffer_SetFormat, 4627, IDirectSoundBuffer8_SetFormat),
    // CDirectSoundVoice::SetMode (XRef)
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetMode, 5233),
    // CDirectSoundBuffer::SetMode (XRef)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetMode, 5233),
	// IDirectSoundBuffer_SetMode
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetMode, 5233),
    // CDirectSoundVoice::SetRolloffCurve (XREF)
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetRolloffCurve, 5233),
    // CDirectSoundBuffer::SetRolloffCurve (XREF)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetRolloffCurve, 5233),
	// IDirectSoundBuffer8::SetRolloffCurve
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetRolloffCurve, 5233),
	// IDirectSound8::AddRef (* unchanged since 3936 *)
	OOVPA_TABLE_ENTRY(IDirectSound8_AddRef, 3936),
	// CDirectSound_SetI3DL2Listener
	OOVPA_TABLE_XREF(DirectSound_CDirectSound_SetI3DL2Listener, 5344),
	// IDirectSound8_SetI3DL2Listener
	OOVPA_TABLE_PATCH(IDirectSound_SetI3DL2Listener, 5344, IDirectSound8_SetI3DL2Listener),
	// CDirectSound::SetRolloffFactor (XREF)`
	OOVPA_TABLE_XREF(DirectSound_CDirectSound_SetRolloffFactor, 5344),
	// IDirectSound8::SetRolloffFactor
	OOVPA_TABLE_PATCH(IDirectSound_SetRolloffFactor, 5344, IDirectSound8_SetRolloffFactor),
#if 0 // Add when needed...
	// CDirectSoundVoice::SetDistanceFactor (XREF)
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetDistanceFactor, 5558),
    // CDirectSoundBuffer::SetDistanceFactor (XREF)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetDistanceFactor, 5558),
	// IDirectSoundBuffer_SetDistanceFactor
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetDistanceFactor, 5558),
#endif
};

// ******************************************************************
// * DSound_5233_SIZE
// ******************************************************************
uint32 DSound_5233_SIZE = sizeof(DSound_5233);
