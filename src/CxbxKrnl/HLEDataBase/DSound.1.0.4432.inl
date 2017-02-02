// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->DSound.1.0.4432.cpp
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
// * IDirectSound8_Release
// ******************************************************************
OOVPA_NO_XREF(IDirectSound8_Release, 4432, 12)

        // IDirectSound8_Release+0x04 : lea ecx, [eax-8]
        { 0x04, 0x8D }, // (Offset,Value)-Pair #1
        { 0x05, 0x48 }, // (Offset,Value)-Pair #2
        { 0x06, 0xF8 }, // (Offset,Value)-Pair #3

        // IDirectSound8_Release+0x07 : neg eax
        { 0x07, 0xF7 }, // (Offset,Value)-Pair #4
        { 0x08, 0xD8 }, // (Offset,Value)-Pair #5

        // IDirectSound8_Release+0x09 : sbb eax, eax
        { 0x09, 0x1B }, // (Offset,Value)-Pair #6
        { 0x0A, 0xC0 }, // (Offset,Value)-Pair #7

        // IDirectSound8_Release+0x0D : mov ecx, [eax]
        { 0x0D, 0x8B }, // (Offset,Value)-Pair #8
        { 0x0E, 0x08 }, // (Offset,Value)-Pair #9

        // IDirectSound8_Release+0x10 : call dword ptr [ecx+8]
        { 0x10, 0xFF }, // (Offset,Value)-Pair #10
        { 0x11, 0x51 }, // (Offset,Value)-Pair #11
        { 0x12, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetMaxDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMaxDistance, 4432, 9,

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

        // CDirectSoundVoice_SetMaxDistance+0x30 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 }
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetMaxDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetMaxDistance, 4432, 10,

    XREF_DSBUFFERSETMAXDISTANCE,
    XRefOne)

        // CDirectSoundBuffer_SetMaxDistance+0x39 : call [CDirectSoundVoice::SetMaxDistance]
        { 0x3A, XREF_DSVOICESETMAXDISTANCE },

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
OOVPA_XREF(IDirectSoundBuffer_SetMaxDistance, 4432, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetMaxDistance+0x1C : call [CDirectSoundBuffer::SetMaxDistance]
        { 0x1D, XREF_DSBUFFERSETMAXDISTANCE },

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
// * CDirectSoundVoice::SetMinDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMinDistance, 4432, 9,

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

        // CDirectSoundVoice_SetMinDistance+0x30 : retn 0Ch
        { 0x31, 0x0C },
        { 0x32, 0x00 }
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetMinDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetMinDistance, 4432, 10,

    XREF_DSBUFFERSETMINDISTANCE,
    XRefOne)

        // CDirectSoundBuffer_SetMinDistance+0x39 : call [CDirectSoundVoice::SetMinDistance]
        { 0x3A, XREF_DSVOICESETMINDISTANCE },

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
OOVPA_XREF(IDirectSoundBuffer_SetMinDistance, 4432, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetMinDistance+0x1C : call [CDirectSoundBuffer::SetMinDistance]
        { 0x1D, XREF_DSBUFFERSETMINDISTANCE },

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
// * CDirectSound::SetI3DL2Listener
// ******************************************************************
OOVPA_XREF(CDirectSound_SetI3DL2Listener, 4432, 11,

    XREF_DSSETI3DL2LISTENER,
    XRefZero)

        // CDirectSound_SetI3DL2Listener+0x3A : mov edi, 0x88780032
        { 0x3A, 0xBF }, // (Offset,Value)-Pair #1
        { 0x3B, 0x32 }, // (Offset,Value)-Pair #2
        { 0x3C, 0x00 }, // (Offset,Value)-Pair #3
        { 0x3D, 0x78 }, // (Offset,Value)-Pair #4
        { 0x3E, 0x88 }, // (Offset,Value)-Pair #5

        // CDirectSound_SetI3DL2Listener+0xA2 : fstp dword ptr[edx+0x94]
        { 0xA2, 0xD9 }, // (Offset,Value)-Pair #6
        { 0xA3, 0x9A }, // (Offset,Value)-Pair #7
        { 0xA4, 0x94 }, // (Offset,Value)-Pair #8

        // CDirectSound_SetI3DL2Listener+0xA8 : fld dword ptr[ecx+0x24]
        { 0xA8, 0xD9 }, // (Offset,Value)-Pair #9
        { 0xA9, 0x41 }, // (Offset,Value)-Pair #10
        { 0xAA, 0x24 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirectSound8_SetI3DL2Listener
// ******************************************************************
OOVPA_XREF(IDirectSound8_SetI3DL2Listener, 4432, 12,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSound8_SetI3DL2Listener+0x19 : call [CDirectSound::SetI3DL2Listener]
        { 0x19, XREF_DSSETI3DL2LISTENER }, // (Offset,Value)-Pair #1

        // IDirectSound8_SetI3DL2Listener+0x04 : push [esp+0x0C]
        { 0x04, 0xFF }, // (Offset,Value)-Pair #2
        { 0x05, 0x74 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x0C }, // (Offset,Value)-Pair #5

        // IDirectSound8_SetI3DL2Listener+0x0E : add eax, 0xFFFFFFF8
        { 0x0E, 0x83 }, // (Offset,Value)-Pair #6
        { 0x0F, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x10, 0xF8 }, // (Offset,Value)-Pair #8

        // IDirectSound8_SetI3DL2Listener+0x13 : sbb ecx, ecx
        { 0x13, 0x1B }, // (Offset,Value)-Pair #9
        { 0x14, 0xC9 }, // (Offset,Value)-Pair #10

        // IDirectSound8_SetI3DL2Listener+0x15 : and ecx, eax
        { 0x15, 0x23 }, // (Offset,Value)-Pair #11
        { 0x16, 0xC8 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoiceSettings::SetMixBins
// ******************************************************************
OOVPA_XREF(CDirectSoundVoiceSettings_SetMixBins, 4432, 10,

    XREF_DSSETMIXBINSC,
    XRefZero)

        // CDirectSoundVoiceSettings_SetMixBins+0x06 : jnz +0x1A
        { 0x06, 0x75 }, // (Offset,Value)-Pair #1
        { 0x07, 0x1A }, // (Offset,Value)-Pair #2

        // CDirectSoundVoiceSettings_SetMixBins+0x08 : test byte ptr [ecx+8], 0x10
        { 0x08, 0xF6 }, // (Offset,Value)-Pair #3
        { 0x09, 0x41 }, // (Offset,Value)-Pair #4
        { 0x0A, 0x08 }, // (Offset,Value)-Pair #5
        { 0x0B, 0x10 }, // (Offset,Value)-Pair #6

        // CDirectSoundVoiceSettings_SetMixBins+0x2B : jbe +0x2B
        { 0x2B, 0x76 }, // (Offset,Value)-Pair #7
        { 0x2C, 0x2B }, // (Offset,Value)-Pair #8

        // CDirectSoundVoiceSettings_SetMixBins+0xB2 : retn 0x04
        { 0xB2, 0xC2 }, // (Offset,Value)-Pair #9
        { 0xB3, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetMixBins
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMixBins, 4432, 17,

    XREF_DSSETMIXBINSB,
    XRefOne)

        // CDirectSoundVoice_SetMixBins+0x0D : call [CDirectSoundVoiceSettings::SetMixBins]
        { 0x0D, XREF_DSSETMIXBINSC }, // (Offset,Value)-Pair #1

        // CDirectSoundVoice_SetMixBins+0x01 : mov esi, [esp+8]
        { 0x01, 0x8B }, // (Offset,Value)-Pair #2
        { 0x02, 0x74 }, // (Offset,Value)-Pair #3
        { 0x03, 0x24 }, // (Offset,Value)-Pair #4
        { 0x04, 0x08 }, // (Offset,Value)-Pair #5

        // CDirectSoundVoice_SetMixBins+0x05 : push [esp+0x0C]
        { 0x05, 0xFF }, // (Offset,Value)-Pair #6
        { 0x06, 0x74 }, // (Offset,Value)-Pair #7
        { 0x07, 0x24 }, // (Offset,Value)-Pair #8
        { 0x08, 0x0C }, // (Offset,Value)-Pair #9

        // CDirectSoundVoice_SetMixBins+0x09 : mov ecx, [esi+0x10]
        { 0x09, 0x8B }, // (Offset,Value)-Pair #10
        { 0x0A, 0x4E }, // (Offset,Value)-Pair #11
        { 0x0B, 0x10 }, // (Offset,Value)-Pair #12

        // CDirectSoundVoice_SetMixBins+0x11 : mov ecx, [esi+0x0C]
        { 0x11, 0x8B }, // (Offset,Value)-Pair #13
        { 0x12, 0x4E }, // (Offset,Value)-Pair #14
        { 0x13, 0x0C }, // (Offset,Value)-Pair #15

        // CDirectSoundVoice_SetMixBins+0x1A : retn 0x08
        { 0x1A, 0xC2 }, // (Offset,Value)-Pair #16
        { 0x1B, 0x08 }, // (Offset,Value)-Pair #17
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetMixBins
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetMixBins, 4432, 17,

    XREF_DSSETMIXBINSA,
    XRefOne)

        // CDirectSoundBuffer_SetMixBins+0x32 : call [CDirectSoundVoice::SetMixBins]
        { 0x32, XREF_DSSETMIXBINSB }, // (Offset,Value)-Pair #1

        // CDirectSoundBuffer_SetMixBins+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 }, // (Offset,Value)-Pair #2
        { 0x22, 0x05 }, // (Offset,Value)-Pair #3
        { 0x23, 0x40 }, // (Offset,Value)-Pair #4
        { 0x24, 0x00 }, // (Offset,Value)-Pair #5
        { 0x25, 0x80 }, // (Offset,Value)-Pair #6

        // CDirectSoundBuffer_SetMixBins+0x29 : push [esp+0x10]; push [esp+0x10]
        { 0x29, 0xFF }, // (Offset,Value)-Pair #7
        { 0x2A, 0x74 }, // (Offset,Value)-Pair #8
        { 0x2B, 0x24 }, // (Offset,Value)-Pair #9
        { 0x2C, 0x10 }, // (Offset,Value)-Pair #10
        { 0x2D, 0xFF }, // (Offset,Value)-Pair #11
        { 0x2E, 0x74 }, // (Offset,Value)-Pair #12
        { 0x2F, 0x24 }, // (Offset,Value)-Pair #13
        { 0x30, 0x10 }, // (Offset,Value)-Pair #14

        // CDirectSoundBuffer_SetMixBins+0x49 : pop edi
        { 0x49, 0x5F }, // (Offset,Value)-Pair #15

        // CDirectSoundBuffer_SetMixBins+0x4B : retn 0x08
        { 0x4B, 0xC2 }, // (Offset,Value)-Pair #16
        { 0x4C, 0x08 }, // (Offset,Value)-Pair #17
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetMixBins
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetMixBins, 4432, 12,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer8_SetMixBins+0x15 : call [CDirectSoundBuffer::SetMixBins]
        { 0x15, XREF_DSSETMIXBINSA }, // (Offset,Value)-Pair #1

        // IDirectSoundBuffer8_SetMixBins+0x04 : push [esp+0x08]
        { 0x04, 0xFF }, // (Offset,Value)-Pair #2
        { 0x05, 0x74 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5

        // IDirectSoundBuffer8_SetMixBins+0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 }, // (Offset,Value)-Pair #6
        { 0x0B, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x0C, 0xE4 }, // (Offset,Value)-Pair #8

        // IDirectSoundBuffer8_SetMixBins+0x0F : sbb ecx, ecx
        { 0x0F, 0x1B }, // (Offset,Value)-Pair #9
        { 0x10, 0xC9 }, // (Offset,Value)-Pair #10

        // IDirectSoundBuffer8_SetMixBins+0x11 : and ecx, eax
        { 0x11, 0x23 }, // (Offset,Value)-Pair #11
        { 0x12, 0xC8 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * CDirectSound::CommitDeferredSettings
// ******************************************************************
OOVPA_NO_XREF(CDirectSound_CommitDeferredSettings, 4432, 11)

        // CDirectSound_CommitDeferredSettings+0x10 : movzx eax, al
        { 0x10, 0x0F }, // (Offset,Value)-Pair #1
        { 0x11, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x12, 0xC0 }, // (Offset,Value)-Pair #3

        // CDirectSound_CommitDeferredSettings+0x27 : mov eax, 0x80004005
        { 0x27, 0xB8 }, // (Offset,Value)-Pair #4
        { 0x28, 0x05 }, // (Offset,Value)-Pair #5
        { 0x29, 0x40 }, // (Offset,Value)-Pair #6
        { 0x2B, 0x80 }, // (Offset,Value)-Pair #7

        // CDirectSound_CommitDeferredSettings+0x5C : and [eax+0xA4], esi
        { 0x5C, 0x21 }, // (Offset,Value)-Pair #8
        { 0x5D, 0xB0 }, // (Offset,Value)-Pair #9
        { 0x5E, 0xA4 }, // (Offset,Value)-Pair #10

        // CDirectSound_CommitDeferredSettings+0x78 : leave
        { 0x78, 0xC9 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetDistanceFactorA
// ******************************************************************
OOVPA_XREF(CDirectSound_SetDistanceFactorA, 4432, 11,

    XREF_SETDISTANCEFACTORA,
    XRefZero)

        // CDirectSound_SetDistanceFactorA+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x22, 0x05 }, // (Offset,Value)-Pair #2
        { 0x23, 0x40 }, // (Offset,Value)-Pair #3
        { 0x24, 0x00 }, // (Offset,Value)-Pair #4
        { 0x25, 0x80 }, // (Offset,Value)-Pair #5

        // CDirectSound_SetDistanceFactorA+0x39 : or dword ptr[eax+0xA4], 0x60
        { 0x39, 0x83 }, // (Offset,Value)-Pair #6
        { 0x3A, 0x88 }, // (Offset,Value)-Pair #7
        { 0x3B, 0xA4 }, // (Offset,Value)-Pair #8
        { 0x3F, 0x60 }, // (Offset,Value)-Pair #9

        // CDirectSound_SetDistanceFactorA+0x4F : jz +0x0B
        { 0x4F, 0x74 }, // (Offset,Value)-Pair #10
        { 0x50, 0x0B }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirectSound8_SetDistanceFactor
// ******************************************************************
OOVPA_XREF(IDirectSound8_SetDistanceFactor, 4432, 11,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSound8_SetDistanceFactor+0x1D : call [CDirectSound::SetDistanceFactor]
        { 0x1D, XREF_SETDISTANCEFACTORA },  // (Offset,Value)-Pair #1

        // IDirectSound8_SetDistanceFactor+0x04 : fld [esp+0x0C]
        { 0x04, 0xD9 }, // (Offset,Value)-Pair #2
        { 0x05, 0x44 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x0C }, // (Offset,Value)-Pair #5

        // IDirectSound8_SetDistanceFactor+0x0C : push ecx
        { 0x0C, 0x51 }, // (Offset,Value)-Pair #6

        // IDirectSound8_SetDistanceFactor+0x12 : add eax, 0xFFFFFFF8
        { 0x12, 0x83 }, // (Offset,Value)-Pair #7
        { 0x13, 0xC0 }, // (Offset,Value)-Pair #8
        { 0x14, 0xF8 }, // (Offset,Value)-Pair #9

        // IDirectSound8_SetDistanceFactor+0x21 : retn 0x0C
        { 0x21, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x22, 0x0C }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetPositionA
// ******************************************************************
OOVPA_XREF(CDirectSound_SetPositionA, 4432, 11,

    XREF_DSSETPOSITIONA,
    XRefZero)

        // CDirectSound_SetPositionA+0x2B : mov ecx, [ebp+0x08]
        { 0x2B, 0x8B }, // (Offset,Value)-Pair #1
        { 0x2C, 0x4D }, // (Offset,Value)-Pair #2
        { 0x2D, 0x08 }, // (Offset,Value)-Pair #3

        // CDirectSound_SetPositionA+0x3F : mov [edx+0x3C], edi
        { 0x3F, 0x89 }, // (Offset,Value)-Pair #4
        { 0x40, 0x7A }, // (Offset,Value)-Pair #5
        { 0x41, 0x3C }, // (Offset,Value)-Pair #6

        // CDirectSound_SetPositionA+0x4C : or word ptr [eax+0xA4], 0x01FF
        { 0x4C, 0x80 }, // (Offset,Value)-Pair #7
        { 0x4D, 0x88 }, // (Offset,Value)-Pair #8
        { 0x4E, 0xA4 }, // (Offset,Value)-Pair #9
        { 0x4F, 0x00 }, // (Offset,Value)-Pair #10
        { 0x52, 0xFF }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetVelocity
// ******************************************************************
OOVPA_XREF(CDirectSound_SetVelocity, 4432, 15,

    XREF_DSSETVELOCITYA,
    XRefZero)

        // CDirectSound_SetVelocity+0x24 : mov eax, 0x80004005
        { 0x24, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x25, 0x05 }, // (Offset,Value)-Pair #2
        { 0x26, 0x40 }, // (Offset,Value)-Pair #3
        { 0x27, 0x00 }, // (Offset,Value)-Pair #4
        { 0x28, 0x80 }, // (Offset,Value)-Pair #5

        // CDirectSound_SetVelocity+0x37 : mov [edx+0x44], edi
        { 0x37, 0x89 }, // (Offset,Value)-Pair #6
        { 0x38, 0x7A }, // (Offset,Value)-Pair #7
        { 0x39, 0x44 }, // (Offset,Value)-Pair #8

        // CDirectSound_SetVelocity+0x4C : or dword ptr [eax+0x0A4], 0x40
        { 0x4C, 0x83 }, // (Offset,Value)-Pair #9
        { 0x4D, 0x88 }, // (Offset,Value)-Pair #10
        { 0x4E, 0xA4 }, // (Offset,Value)-Pair #11
        { 0x4F, 0x00 }, // (Offset,Value)-Pair #12
        { 0x52, 0x40 }, // (Offset,Value)-Pair #13

        // CDirectSound_SetVelocity+0x73 : retn 0x14
        { 0x73, 0xC2 }, // (Offset,Value)-Pair #14
        { 0x74, 0x14 }, // (Offset,Value)-Pair #15
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetPosition
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetPosition, 4432, 8,

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
// * DirectSoundCreate, 4432
// ******************************************************************
OOVPATable DSound_4432[] = {

    // DirectSoundCreate (* unchanged since 4361 *)
	OOVPA_TABLE_ENTRY(DirectSoundCreate, 4361),
	// IDirectSound8_CreateStream
	OOVPA_TABLE_ENTRY(IDirectSound8_CreateStream, 3936),
    // CDirectSound_CreateSoundStream (* unchanged since 4361 *)
	OOVPA_TABLE_XREF(CDirectSound_CreateSoundStream, 4361),
	// DirectSoundCreateStream (* unchanged since 4361 *)
	OOVPA_TABLE_ENTRY(DirectSoundCreateStream, 4361),
    // CMcpxStream::Pause (* unchanged since 4361 *)
	OOVPA_TABLE_XREF(CMcpxStream_Pause, 4361),
	// CDirectSoundStream::Pause (* unchanged since 4361 *)
	OOVPA_TABLE_ENTRY(CDirectSoundStream_Pause, 4361),
	// IDirectSound8::AddRef (* unchanged since 3936 *)
	OOVPA_TABLE_ENTRY(IDirectSound8_AddRef, 3936),
	// IDirectSound8::Release
	OOVPA_TABLE_ENTRY(IDirectSound8_Release, 4432),
	// IDirectSound8::CreateSoundBuffer (* unchanged since 4361 *)
	OOVPA_TABLE_ENTRY(IDirectSound8_CreateSoundBuffer, 4361),
	// IDirectSoundBuffer8::Release (* unchanged since 3936 *)
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_Release, 3936),
    // CDirectSoundBuffer::SetLoopRegion (XREF) (* unchanged since 4361 *)
    OOVPA_TABLE_XREF(CDirectSoundBuffer_SetLoopRegion, 4361),
	// IDirectSoundBuffer8::SetLoopRegion (* unchanged since 4361 *)
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetLoopRegion, 4361),
    // CMcpxBuffer::SetBufferData (* unchanged since 4361 *)
	OOVPA_TABLE_XREF(CMcpxBuffer_SetBufferData, 4361),
    // CDirectSoundBuffer::SetBufferData (* unchanged since 4361 *)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetBufferData, 4361),
	// IDirectSoundBuffer8::SetBufferData (* unchanged since 4361 *)
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetBufferData, 4361),
    // CMcpxBuffer::SetCurrentPosition (* unchanged since 4361 *)
	OOVPA_TABLE_XREF(CMcpxBuffer_SetCurrentPosition, 4361),
    // CDirectSoundBuffer::SetCurrentPosition (* unchanged since 4361 *)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetCurrentPosition, 4361),
	// IDirectSoundBuffer8::SetCurrentPosition (* unchanged since 4361 *)
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetCurrentPosition, 4361),
    // CMcpxBuffer::Stop (* unchanged since 4361 *)
	OOVPA_TABLE_XREF(CMcpxBuffer_Stop, 4361),
    // CDirectSoundBuffer::Stop (* unchanged since 4361 *)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_Stop, 4361),
	// IDirectSoundBuffer8::Stop (* unchanged since 4361 *)
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_Stop, 4361),
    // CDirectSoundVoice::SetMaxDistance (XREF)
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetMaxDistance, 4432),
    // CDirectSoundBuffer::SetMaxDistance (XREF)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetMaxDistance, 4432),
	// IDirectSoundBuffer_SetMaxDistance
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetMaxDistance, 4432, IDirectSoundBuffer8_SetMaxDistance),
    // CDirectSoundVoice::SetMinDistance (XREF)
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetMinDistance, 4432),
    // CDirectSoundBuffer::SetMinDistance (XREF)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetMinDistance, 4432),
	// IDirectSoundBuffer_SetMinDistance
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetMinDistance, 4432, IDirectSoundBuffer8_SetMinDistance),
    // CDirectSound_SetI3DL2Listener
	OOVPA_TABLE_XREF(CDirectSound_SetI3DL2Listener, 4432),
	// IDirectSound8_SetI3DL2Listener
	OOVPA_TABLE_ENTRY(IDirectSound8_SetI3DL2Listener, 4432),
    // CDirectSoundVoiceSettings_SetMixBins
	OOVPA_TABLE_XREF(CDirectSoundVoiceSettings_SetMixBins, 4432),
    // CDirectSoundVoice_SetMixBins
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetMixBins, 4432),
    // CDirectSoundBuffer_SetMixBins
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetMixBins, 4432),
	// IDirectSoundBuffer8_SetMixBins
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetMixBins, 4432),
	// CDirectSound::CommitDeferredSettings
	OOVPA_TABLE_ENTRY(CDirectSound_CommitDeferredSettings, 4432),
    // CDirectSound::SetDistanceFactorA (XREF)
	OOVPA_TABLE_XREF(CDirectSound_SetDistanceFactorA, 4432),
	// IDirectSound8::SetDistanceFactor
	OOVPA_TABLE_ENTRY(IDirectSound8_SetDistanceFactor, 4432),
	// IDirectSound8::DownloadEffectsImage (* unchanged since 3936 *)
	OOVPA_TABLE_ENTRY(IDirectSound8_DownloadEffectsImage, 3936),
	// IDirectSoundBuffer_AddRef (* unchanged since 3936 *)
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_AddRef, 3936),
};

// ******************************************************************
// * DSound_4432_SIZE
// ******************************************************************
uint32 DSound_4432_SIZE = sizeof(DSound_4432);
