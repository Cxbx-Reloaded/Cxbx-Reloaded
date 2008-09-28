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
SOOVPA<8> CDirectSound_EnableHeadphones_1_0_5233 =
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSENABLEHEADPHONES, // Xref Is Saved
    0,                       // Xref Not Used

    {
        { 0x16, 0x45 }, // (Offset,Value)-Pair #1
        { 0x1D, 0x0B }, // (Offset,Value)-Pair #2
        { 0x2A, 0x05 }, // (Offset,Value)-Pair #3
        { 0x46, 0x95 }, // (Offset,Value)-Pair #4
        { 0x56, 0x80 }, // (Offset,Value)-Pair #5
        { 0x5D, 0x7F }, // (Offset,Value)-Pair #6
        { 0x74, 0xA4 }, // (Offset,Value)-Pair #7
        { 0x99, 0x08 }, // (Offset,Value)-Pair #8
    }
};

// ******************************************************************
// * IDirectSound8_EnableHeadphones
// ******************************************************************
SOOVPA<8> IDirectSound8_EnableHeadphones_1_0_5233 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // XRef Not Saved
    1,  // XRef Is Used

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_FlushEx
// ******************************************************************
SOOVPA<11> CDirectSoundStream_FlushEx_1_0_5233 =
{
    0,  // Large == 0
    11, // Count == 11

    XREF_DSFLUSHEX2,  // XRef Is  Saved
    0,                // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirectSoundBuffer8_StopEx
// ******************************************************************
SOOVPA<9> IDirectSoundBuffer8_StopEx_1_0_5233 =
{
    0,  // Large == 0
    9,  // Count == 9

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
        { 0x11, XREF_DSFLUSHEX2 },  // (Offset,Value)-Pair #1

        { 0x00, 0xFF }, // (Offset,Value)-Pair #2
        { 0x03, 0x10 }, // (Offset,Value)-Pair #3

        { 0x04, 0xFF }, // (Offset,Value)-Pair #4
        { 0x07, 0x10 }, // (Offset,Value)-Pair #5

        { 0x08, 0xFF }, // (Offset,Value)-Pair #6
        { 0x0B, 0x10 }, // (Offset,Value)-Pair #7

        { 0x15, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x16, 0x10 }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * CMcpxStream_Pause
// ******************************************************************
SOOVPA<11> CMcpxStream_Pause_1_0_5233 =
{
    0,  // Large == 0
    11, // Count == 11

    XREF_DSSTREAMPAUSE, // XRef Is  Saved
    0,                  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// CMcpxAPU::SynchPlayback
// ******************************************************************
SOOVPA<8> CMcpxAPU_SynchPlayback_1_0_5233 =
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSSYNCHPLAYBACKB, // Xref Is Saved
    0,                     // Xref Not Used

    {
        { 0x0C, 0x74 }, // (Offset,Value)-Pair #1
        { 0x1A, 0x1A }, // (Offset,Value)-Pair #2
        { 0x25, 0x53 }, // (Offset,Value)-Pair #3
        { 0x37, 0xF2 }, // (Offset,Value)-Pair #4
        { 0x58, 0xBC }, // (Offset,Value)-Pair #5
        { 0x84, 0x43 }, // (Offset,Value)-Pair #6
        { 0x9E, 0x64 }, // (Offset,Value)-Pair #7
        { 0xFF, 0x00 }, // (Offset,Value)-Pair #8
    }
};

// ******************************************************************
// * CDirectSound::SynchPlayback
// ******************************************************************
SOOVPA<10> CDirectSound_SynchPlayback_1_0_5233 =
{
    0,  // Large == 0
    10, // Count == 10

    XREF_DSSYNCHPLAYBACKA, // Xref Is Saved
    1,                     // Xref Not Used

    {
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
    }
};

// ******************************************************************
// * IDirectSound8::SynchPlayback
// ******************************************************************
SOOVPA<8> IDirectSound8_SynchPlayback_1_0_5233 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // XRef Not Saved
    1,  // XRef Is Used

    {
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
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetFormat
// ******************************************************************
SOOVPA<8> X_DirectSound_CDirectSoundVoice_SetFormat_1_0_5233 =
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DirectSound_CDirectSoundVoice_SetFormat, // Xref Is Saved
    0,                                            // Xref Not Used

    {
        { 0x06, 0x4E },
        { 0x0E, 0xE8 },
        { 0x18, 0x4E },
        { 0x25, 0xFF },
        { 0x2E, 0x0C },
        { 0x34, 0x85 },
        { 0x38, 0x8B },
        { 0x43, 0x00 },
    }
};

// ******************************************************************
// CDirectSoundVoice::SetMode
// ******************************************************************
SOOVPA<8> CDirectSoundVoice_SetMode_1_0_5233 =
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSBUFFERSETMODEB, // Xref Is Saved
    0,                     // Xref Not Used

    {
        { 0x00, 0xF6 }, // (Offset,Value)-Pair #1
        { 0x07, 0x24 }, // (Offset,Value)-Pair #2
        { 0x0B, 0x10 }, // (Offset,Value)-Pair #3
        { 0x0E, 0xB4 }, // (Offset,Value)-Pair #4
        { 0x18, 0x3C }, // (Offset,Value)-Pair #5
        { 0x1A, 0x06 }, // (Offset,Value)-Pair #6
        { 0x21, 0x33 }, // (Offset,Value)-Pair #7
        { 0x24, 0x0C }, // (Offset,Value)-Pair #8
    }
};

// ******************************************************************
// CDirectSoundBuffer::SetMode
// ******************************************************************
SOOVPA<8> CDirectSoundBuffer_SetMode_1_0_5233 =
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSBUFFERSETMODEA, // Xref Is Saved
    1,                     // Xref Is Used

    {
        { 0x36, XREF_DSBUFFERSETMODEB }, // (Offset,Value)-Pair #1

        { 0x01, 0xE8 }, // (Offset,Value)-Pair #2
        { 0x15, 0x0B }, // (Offset,Value)-Pair #3
        { 0x22, 0x05 }, // (Offset,Value)-Pair #4
        { 0x27, 0x26 }, // (Offset,Value)-Pair #5
        { 0x34, 0x14 }, // (Offset,Value)-Pair #6
        { 0x3F, 0x0B }, // (Offset,Value)-Pair #7
        { 0x50, 0x0C }, // (Offset,Value)-Pair #8
    }
};

// ******************************************************************
// * IDirectSoundBuffer8_SetMode
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer8_SetMode_1_0_5233 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // XRef Not Saved
    1,  // XRef Is Used

    {
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
    }
};

// ******************************************************************
// * CDirectSoundVoice::SetVolume
// ******************************************************************
SOOVPA<8> CDirectSoundVoice_SetVolume_1_0_5233 =
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DirectSound_CDirectSoundVoice_SetVolume,  // Xref Is Saved
    1,                                             // Xref Is Used

    {
        { 0x15, XREF_DSSTREAMSETVOLUME },

        { 0x02, 0x24 },
        { 0x06, 0x10 },
        { 0x0A, 0x08 },
        { 0x0E, 0x89 },
        { 0x12, 0x49 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
    }
};

// ******************************************************************
// * CDirectSoundBuffer::SetVolume
// ******************************************************************
SOOVPA<8> CDirectSoundBuffer_SetVolume_1_0_5233 =
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DirectSound_CDirectSoundBuffer_SetVolume,  // Xref Is Saved
    1,                                              // Xref Is Used

    {
        { 0x32, XREF_DirectSound_CDirectSoundVoice_SetVolume },

        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0x10 },
        { 0x3A, 0x74 },
        { 0x47, 0x8B },
    }
};

// ******************************************************************
// * IDirectSoundBuffer8::SetVolume
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer8_SetVolume_1_0_5233 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // Xref Not Saved
    1,  // Xref Is  Used

    {
        { 0x15, XREF_DirectSound_CDirectSoundBuffer_SetVolume },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
    }
};

// ******************************************************************
// * CDirectSoundStream::SetVolume
// ******************************************************************
SOOVPA<9> CDirectSoundStream_SetVolume_1_0_5233 =
{
    0,  // Large == 0
    9,  // Count == 9

    -1, // Xref Not Saved
    1,  // Xref Is Used

    {
        { 0x36, XREF_DirectSound_CDirectSoundVoice_SetVolume },

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
    }
};

// ******************************************************************
// * CDirectSoundStream::SetFrequency
// ******************************************************************
SOOVPA<8> CDirectSoundStream_SetFrequency_1_0_5233 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
        { 0x36, XREF_DSBUFFERSETFREQUENCYB },

        { 0x0E, 0xB6 },
        { 0x15, 0x0B },
        { 0x27, 0x26 },
        { 0x2A, 0x24 },
        { 0x3E, 0x74 },
        { 0x4D, 0x5F },
        { 0x50, 0x08 },
    }
};

// ******************************************************************
// * CDirectSoundStream::SetMixBins
// ******************************************************************
SOOVPA<8> CDirectSoundStream_SetMixBins_1_0_5233 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
        { 0x36, XREF_DSSETMIXBINSB },

        { 0x0E, 0xB6 },
        { 0x15, 0x0B },
        { 0x27, 0x26 },
        { 0x2A, 0x24 },
        { 0x3E, 0x74 },
        { 0x4D, 0x5F },
        { 0x50, 0x08 },
    }
};

// ******************************************************************
// * CDirectSoundVoice::SetRolloffCurve
// ******************************************************************
SOOVPA<8> CDirectSoundVoice_SetRolloffCurve_1_0_5233 =
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSVOICESETROLLOFFCURVE,  // Xref Is Saved
    0,                            // Xref Not Used

    {
        { 0x07, 0x8B },
        { 0x10, 0x08 },
        { 0x12, 0x51 },
        { 0x19, 0xB4 },
        { 0x22, 0x51 },
        { 0x2B, 0x00 },
        { 0x34, 0x10 },
        { 0x3E, 0x33 },
    }
};

// ******************************************************************
// * CDirectSoundBuffer::SetRolloffCurve
// ******************************************************************
SOOVPA<8> CDirectSoundBuffer_SetRolloffCurve_1_0_5233 =
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSBUFFERSETROLLOFFCURVE,  // Xref Is Saved
    1,                             // Xref Is Used

    {
        { 0x3A, XREF_DSVOICESETROLLOFFCURVE },

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x74 },
        { 0x35, 0xFF },
        { 0x40, 0x8B },
        { 0x4F, 0x8B },
    }
};

// ******************************************************************
// * IDirectSoundBuffer8_SetRolloffCurve
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer8_SetRolloffCurve_1_0_5233 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // Xref Not Saved
    1,  // Xref Is Used

    {
        { 0x1D, XREF_DSBUFFERSETROLLOFFCURVE },

        { 0x03, 0x10 },
        { 0x08, 0xFF },
        { 0x0D, 0xC8 },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
    }
};

// ******************************************************************
// * Direct_1_0_5233
// ******************************************************************
OOVPATable DSound_1_0_5233[] =
{
    // DirectSoundCreate (* unchanged since 4361 *)
    {
        (OOVPA*)&DirectSoundCreate_1_0_4361,

        XTL::EmuDirectSoundCreate,

        #ifdef _DEBUG_TRACE
        "EmuDirectSoundCreate"
        #endif
    },
    // DirectSoundDoWork (* unchanged since 4627 *)
    {
        (OOVPA*)&DirectSoundDoWork_1_0_4627,

        XTL::EmuDirectSoundDoWork,

        #ifdef _DEBUG_TRACE
        "EmuDirectSoundDoWork"
        #endif
    },
    // CDirectSound::CreateSoundBuffer (XREF) (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSound_CreateSoundBuffer_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound::CreateSoundBuffer (XREF)"
        #endif
    },
    // CMcpxAPU::SynchPlayback (XRef)
    {
        (OOVPA*)&CMcpxAPU_SynchPlayback_1_0_5233, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxAPU::SynchPlayback (XRef)"
        #endif
    },
    // CDirectSound::SynchPlayback (XRef)
    {
        (OOVPA*)&CDirectSound_SynchPlayback_1_0_5233, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound::SynchPlayback (XRef)"
        #endif
    },
    // IDirectSound_SynchPlayback
    {
        (OOVPA*)&IDirectSound8_SynchPlayback_1_0_5233,

        XTL::EmuIDirectSound8_SynchPlayback,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SynchPlayback"
        #endif
    },
    // CMcpxStream::Pause (XRef)
    {
        (OOVPA*)&CMcpxStream_Pause_1_0_5233, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxStream_Pause (XRef)"
        #endif
    },
    // CDirectSoundStream::Pause (* unchanged since 4361 *)
    {
        (OOVPA*)&CDirectSoundStream_Pause_1_0_4361,

        XTL::EmuCDirectSoundStream_Pause,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSoundStream_Pause"
        #endif
    },
    // CDirectSound_SetI3DL2Listener (XREF) (* unchanged since 4432 *)
    {
        (OOVPA*)&CDirectSound_SetI3DL2Listener_1_0_4432, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound::SetI3DL2Listener (XREF)"
        #endif
    },
    // IDirectSound8_SetI3DL2Listener (* unchanged since 4432 *)
    {
        (OOVPA*)&IDirectSound8_SetI3DL2Listener_1_0_4432,

        XTL::EmuIDirectSound8_SetI3DL2Listener,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetI3DL2Listener"
        #endif
    },
    // CDirectSound::SetVelocity (XREF) (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSound_SetVelocity_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound::SetVelocityA (XREF)"
        #endif
    },
    // IDirectSound8::SetVelocity (* unchanged since 3936 *)
    {
        (OOVPA*)&IDirectSound8_SetVelocity_1_0_3936,

        XTL::EmuIDirectSound8_SetVelocity,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetVelocity"
        #endif
    },
    // CDirectSoundVoiceSettings::SetMixBins (* unchanged since 4432 *)
    {
        (OOVPA*)&CDirectSoundVoiceSettings_SetMixBins_1_0_4432, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoiceSettings::SetMixBins (XREF)"
        #endif
    },
    // CDirectSoundVoice::SetMixBins (* unchanged since 4432 *)
    {
        (OOVPA*)&CDirectSoundVoice_SetMixBins_1_0_4432, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetMixBins (XREF)"
        #endif
    },
    // CDirectSoundBuffer::SetMixBins (* unchanged since 4432 *)
    {
        (OOVPA*)&CDirectSoundBuffer_SetMixBins_1_0_4432, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetMixBins (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::SetMixBins (* unchanged since 4432 *)
    {
        (OOVPA*)&IDirectSoundBuffer8_SetMixBins_1_0_4432,

        XTL::EmuIDirectSoundBuffer8_SetMixBins,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetMixBins"
        #endif
    },
    // CDirectSoundStream::SetMixBins
    {
        (OOVPA*)&CDirectSoundStream_SetMixBins_1_0_5233,

        XTL::EmuCDirectSoundStream_SetMixBins,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSoundStream_SetMixBins"
        #endif
    },
    // CDirectSound::SetPositionA (XREF) (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSound_SetPositionA_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound::SetPositionA (XREF)"
        #endif
    },
    // IDirectSound8::SetPosition (* unchanged since 3936 *)
    {
        (OOVPA*)&IDirectSound8_SetPosition_1_0_3936,

        XTL::EmuIDirectSound8_SetPosition,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetPosition"
        #endif
    },
    // DirectSoundCreateBuffer (* unchanged since 4627 *)
    {
        (OOVPA*)&DirectSoundCreateBuffer_1_0_4627,

        XTL::EmuDirectSoundCreateBuffer,

        #ifdef _DEBUG_TRACE
        "EmuDirectSoundCreateBuffer"
        #endif
    },
    // IDirectSound8::CreateSoundBuffer (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirectSound8_CreateSoundBuffer_1_0_4627,

        XTL::EmuIDirectSound8_CreateSoundBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_CreateSoundBuffer"
        #endif
    },
    // CDirectSoundVoice::SetFrequency (XREF) (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSoundVoice_SetFrequency_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetFrequency (XREF)"
        #endif
    },
    // CDirectSoundBuffer::SetFrequency (XREF) (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSoundBuffer_SetFrequency_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSoundBuffer::SetFrequency (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::SetFrequency (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirectSoundBuffer8_SetFrequency_1_0_4627,

        XTL::EmuIDirectSoundBuffer8_SetFrequency,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetFrequency"
        #endif
    },
    // CDirectSoundStream::SetFrequency
    {
        (OOVPA*)&CDirectSoundStream_SetFrequency_1_0_5233,

        XTL::EmuCDirectSoundStream_SetFrequency,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSoundStream_SetFrequency"
        #endif
    },
    // CMcpxVoiceClient_SetVolume (XREF) (* unchanged since 4627 *)
    {
        (OOVPA*)&CMcpxVoiceClient_SetVolume_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxVoiceClient::SetVolume (XREF)"
        #endif
    },
    // CDirectSoundVoice::SetVolume (XREF)
    {
        (OOVPA*)&CDirectSoundVoice_SetVolume_1_0_5233,0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetVolume (XREF)"
        #endif
    },
    // CDirectSoundBuffer::SetVolume (XREF)
    {
        (OOVPA*)&CDirectSoundBuffer_SetVolume_1_0_5233,0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetVolume (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::SetVolume
    {
        (OOVPA*)&IDirectSoundBuffer8_SetVolume_1_0_5233,

        XTL::EmuIDirectSoundBuffer8_SetVolume,

        #ifdef _DEBUG_TRACE
        "EmuIIDirectSoundBuffer8_SetVolume"
        #endif
    },
    // CDirectSoundStream_SetVolume (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSoundStream_SetVolume_1_0_4627,

        XTL::EmuCDirectSoundStream_SetVolume,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSoundStream_SetVolume"
        #endif
    },
    // IDirectSoundBuffer8::LockA (XREF)
    {
        (OOVPA*)&IDirectSoundBuffer8_LockA_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_LockA (XRef)"
        #endif
    },
    // IDirectSoundBuffer8::Lock (* unchanged since 3936 *)
    {
        (OOVPA*)&IDirectSoundBuffer8_Lock_1_0_3936,

        XTL::EmuIDirectSoundBuffer8_Lock,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_Lock"
        #endif
    },
    // CDirectSound_CreateSoundStream (XREF) (* unchanged since 4361 *)
    {
        (OOVPA*)&CDirectSound_CreateSoundStream_1_0_4361, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound::CreateSoundStream (XREF)"
        #endif
    },
    // DirectSoundCreateStream (* unchanged since 4361 *)
    {
        (OOVPA*)&DirectSoundCreateStream_1_0_4361,

        XTL::EmuDirectSoundCreateStream,

        #ifdef _DEBUG_TRACE
        "EmuDirectSoundCreateStream"
        #endif
    },
    // IDirectSound8::Release (* unchanged since 3936 *)
    {
        (OOVPA*)&IDirectSound8_Release_1_0_3936,

        XTL::EmuIDirectSound8_Release,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_Release"
        #endif
    },
    // CMcpxBuffer::SetBufferData (XREF) (* unchanged since 4361 *)
    {
        (OOVPA*)&CMcpxBuffer_SetBufferData_1_0_4361, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxBuffer::SetBufferData (XREF)"
        #endif
    },
    // CDirectSoundBuffer::SetBufferData (XREF) (* unchanged since 4361 *)
    {
        (OOVPA*)&CDirectSoundBuffer_SetBufferData_1_0_4361, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetBufferData (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::SetBufferData (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirectSoundBuffer8_SetBufferData_1_0_4361,

        XTL::EmuIDirectSoundBuffer8_SetBufferData,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetBufferData"
        #endif
    },
    // CMcpxBuffer::GetStatusB (XRef) (* unchanged since 4627 *)
    {
        (OOVPA*)&CMcpxBuffer_GetStatusB_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxBuffer_GetStatusB (XRef)"
        #endif
    },
    // CDirectSoundBuffer::GetStatus (XRef) (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSoundBuffer_GetStatus_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer_GetStatus (XRef)"
        #endif
    },
    // IDirectSoundBuffer8::GetStatus (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirectSoundBuffer8_GetStatus_1_0_4627,

        XTL::EmuIDirectSoundBuffer8_GetStatus,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_GetStatus"
        #endif
    },
    // CMcpxBuffer::GetCurrentPosition (XRef) (* unchanged since 4361 *)
    {
        (OOVPA*)&CMcpxBuffer_GetCurrentPosition_1_0_4361, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxBuffer_GetCurrentPosition (XRef)"
        #endif
    },
    // CMcpxBuffer::Play (XREF) (* unchanged since 4928 *)
    {
        (OOVPA*)&CMcpxBuffer_Play_1_0_4928, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxBuffer::Play (XREF)"
        #endif
    },
    // CDirectSoundBuffer::Play (XREF) (* unchanged since 4361 *)
    {
        (OOVPA*)&CDirectSoundBuffer_Play_1_0_4361, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::Play (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::Play (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirectSoundBuffer8_Play_1_0_4361,

        XTL::EmuIDirectSoundBuffer8_Play,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_Play"
        #endif
    },
    // CDirectSoundStream::FlushEx (XREF)
    {
        (OOVPA*)&CDirectSoundStream_FlushEx_1_0_5233, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream::FlushEx (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::StopEx
    {
        (OOVPA*)&IDirectSoundBuffer8_StopEx_1_0_5233,

        XTL::EmuIDirectSoundBuffer8_StopEx,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_StopEx"
        #endif
    },
    // IDirectSound8::Release (* unchanged since 3936 *)
    {
        (OOVPA*)&IDirectSound8_Release_1_0_3936,

        XTL::EmuIDirectSound8_Release,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_Release"
        #endif
    },
    // DirectSound::CDirectSound::EnableHeadphones (XRef)
    {
        (OOVPA*)&CDirectSound_EnableHeadphones_1_0_5233, 0,

        #ifdef _DEBUG_TRACE
        "DirectSound::CDirectSound::EnableHeadphones (XRef)"
        #endif
    },
    // IDirectSound8::EnableHeadphones
    {
        (OOVPA*)&IDirectSound8_EnableHeadphones_1_0_5233,

        XTL::EmuIDirectSound8_EnableHeadphones,

        #ifdef _DEBUG_TRACE
        "IDirectSound8_EnableHeadphones"
        #endif
    },
    // IDirectSound8::DownloadEffectsImage (* unchanged since 3936 *)
    {
        (OOVPA*)&IDirectSound8_DownloadEffectsImage_1_0_3936,

        XTL::EmuIDirectSound8_DownloadEffectsImage,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_DownloadEffectsImage"
        #endif
    },
    // IDirectSound8::SetOrientation (* unchanged since 3936 *)
    {
        (OOVPA*)&IDirectSound8_SetOrientation_1_0_3936,

        XTL::EmuIDirectSound8_SetOrientation,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetOrientation"
        #endif
    },
    // CDirectSound::SetDistanceFactorA (XREF) (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSound_SetDistanceFactorA_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound::SetDistanceFactorA (XREF)"
        #endif
    },
    // IDirectSound8::SetDistanceFactor (* unchanged since 4432 *)
    {
        (OOVPA*)&IDirectSound8_SetDistanceFactor_1_0_4432,

        XTL::EmuIDirectSound8_SetDistanceFactor,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetDistanceFactor"
        #endif
    },
    // CDirectSound::SetRolloffFactor (XREF) (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSound_SetRolloffFactor_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound::SetRolloffFactor (XREF)"
        #endif
    },
    // IDirectSound8::SetRolloffFactor (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirectSound8_SetRolloffFactor_1_0_4627,

        XTL::EmuIDirectSound8_SetRolloffFactor,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetRolloffFactor"
        #endif
    },
    // CDirectSound::SetDopplerFactor (XREF) (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSound_SetDopplerFactor_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound::SetDopplerFactor (XREF)"
        #endif
    },
    // IDirectSound8::SetDopplerFactor (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirectSound8_SetDopplerFactor_1_0_4627,

        XTL::EmuIDirectSound8_SetDopplerFactor,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetDopplerFactor"
        #endif
    },
    // CDirectSound::CommitDeferredSettings (* unchanged since 4432 *)
    {
        (OOVPA*)&CDirectSound_CommitDeferredSettings_1_0_4432,

        XTL::EmuCDirectSound_CommitDeferredSettings,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSound_CommitDeferredSettings"
        #endif
    },
    // CDirectSoundVoice::SetMaxDistance (XREF) (* unchanged since 4432 *)
    {
        (OOVPA*)&CDirectSoundVoice_SetMaxDistance_1_0_4432, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetMaxDistance (XREF)"
        #endif
    },
    // CDirectSoundBuffer::SetMaxDistance (XREF) (* unchanged since 4432 *)
    {
        (OOVPA*)&CDirectSoundBuffer_SetMaxDistance_1_0_4432, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetMaxDistance (XREF)"
        #endif
    },
    // IDirectSoundBuffer_SetMaxDistance (* unchanged since 4432 *)
    {
        (OOVPA*)&IDirectSoundBuffer_SetMaxDistance_1_0_4432,

        XTL::EmuIDirectSoundBuffer8_SetMaxDistance,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetMaxDistance"
        #endif
    },
    // CDirectSoundVoice::SetMinDistance (XREF) (* unchanged since 4432 *)
    {
        (OOVPA*)&CDirectSoundVoice_SetMinDistance_1_0_4432, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetMinDistance (XREF)"
        #endif
    },
    // CDirectSoundBuffer::SetMinDistance (XREF) (* unchanged since 4432 *)
    {
        (OOVPA*)&CDirectSoundBuffer_SetMinDistance_1_0_4432, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetMinDistance (XREF)"
        #endif
    },
    // IDirectSoundBuffer_SetMinDistance (* unchanged since 4432 *)
    {
        (OOVPA*)&IDirectSoundBuffer_SetMinDistance_1_0_4432,

        XTL::EmuIDirectSoundBuffer8_SetMinDistance,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetMinDistance"
        #endif
    },
    // CDirectSoundVoice::SetPosition (XREF) (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSoundVoice_SetPosition_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetPosition (XREF)"
        #endif
    },
    // CDirectSoundBuffer::SetPosition (XREF) (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSoundBuffer_SetPosition_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetPosition (XREF)"
        #endif
    },
    // IDirectSoundBuffer8_SetPosition (* unchanged since 3936 *)
    {
        (OOVPA*)&IDirectSoundBuffer8_SetPosition_1_0_3936,

        XTL::EmuIDirectSoundBuffer8_SetPosition,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetPosition"
        #endif
    },
    // CDirectSoundVoice::SetVelocity (XREF) (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSoundVoice_SetVelocity_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetVelocity (XREF)"
        #endif
    },
    // CDirectSoundBuffer::SetVelocity (XREF) (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSoundBuffer_SetVelocity_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetVelocity (XREF)"
        #endif
    },
    // IDirectSoundBuffer8_SetVelocity (* unchanged since 3936 *)
    {
        (OOVPA*)&IDirectSoundBuffer8_SetVelocity_1_0_3936,

        XTL::EmuIDirectSoundBuffer8_SetVelocity,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetVelocity"
        #endif
    },
    // CDirectSoundVoice::SetI3DL2Source (XREF) (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSoundVoice_SetI3DL2Source_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetI3DL2Source (XREF)"
        #endif
    },
    // CDirectSoundBuffer::SetI3DL2Source (XREF) (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSoundBuffer_SetI3DL2Source_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetI3DL2Source (XREF)"
        #endif
    },
    // IDirectSoundBuffer_SetI3DL2Source (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirectSoundBuffer_SetI3DL2Source_1_0_4627,

        XTL::EmuIDirectSoundBuffer8_SetI3DL2Source,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetI3DL2Source"
        #endif
    },
    // IDirectSoundBuffer_Stop (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirectSoundBuffer_Stop_1_0_4627,

        XTL::EmuIDirectSoundBuffer8_Stop,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_Stop"
        #endif
    },
    // IDirectSoundBuffer8::Release (* unchanged since 3936 *)
    {
        (OOVPA*)&IDirectSoundBuffer8_Release_1_0_3936,

        XTL::EmuIDirectSoundBuffer8_Release,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_Release"
        #endif
    },
    // IDirectSoundStream::SetVolume
    // (This is actually the XREF we are using)
    {
        (OOVPA*)&CDirectSoundStream_SetVolume_1_0_5233,

        XTL::EmuIDirectSoundStream_SetVolume,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundStream_SetVolume"
        #endif
    },
    // DirectSound::CDirectSoundVoice::SetFormat (XRef)
    {
        (OOVPA*)&X_DirectSound_CDirectSoundVoice_SetFormat_1_0_5233, 0,

        #ifdef _DEBUG_TRACE
        "DirectSound::CDirectSoundVoice::SetFormat (XRef)"
        #endif
    },
    // DirectSound::CDirectSoundBuffer::SetFormat (XRef) (* unchanged since 4627 *)
    {
        (OOVPA*)&X_DirectSound_CDirectSoundBuffer_SetFormat_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "DirectSound::CDirectSoundBuffer::SetFormat (XRef)"
        #endif
    },
    // IDirectSoundBuffer8_SetFormat (* unchanged since 4627 *)
    {
        (OOVPA*)&X_IDirectSoundBuffer_SetFormat_1_0_4627,
        XTL::EmuIDirectSoundBuffer8_SetFormat,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetFormat"
        #endif

    },
    // CDirectSoundVoice::SetMode (XRef)
    {
        (OOVPA*)&CDirectSoundVoice_SetMode_1_0_5233, 0,

        #ifdef _DEBUG_TRACE
            "CDirectSoundVoice::SetMode (XRef)"
        #endif
    },
    // CDirectSoundBuffer::SetMode (XRef)
    {
        (OOVPA*)&CDirectSoundBuffer_SetMode_1_0_5233, 0,

        #ifdef _DEBUG_TRACE
            "CDirectSoundBuffer::SetMode (XRef)"
        #endif
    },
    // IDirectSoundBuffer_SetMode
    {
        (OOVPA*)&IDirectSoundBuffer8_SetMode_1_0_5233,
        XTL::EmuIDirectSoundBuffer8_SetMode,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetMode"
        #endif

    },
    // CDirectSoundVoice::SetRolloffCurve (XREF)
    {
        (OOVPA*)&CDirectSoundVoice_SetRolloffCurve_1_0_5233, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetRolloffCurve (XREF)"
        #endif

    },
    // CDirectSoundBuffer::SetRolloffCurve (XREF)
    {
        (OOVPA*)&CDirectSoundBuffer_SetRolloffCurve_1_0_5233, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetRolloffCurve (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::SetRolloffCurve
    {
        (OOVPA*)&IDirectSoundBuffer8_SetRolloffCurve_1_0_5233,

        XTL::EmuIDirectSoundBuffer8_SetRolloffCurve,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetRolloffCurve"
        #endif
    },
};

// ******************************************************************
// * DSound_1_0_5233_SIZE
// ******************************************************************
uint32 DSound_1_0_5233_SIZE = sizeof(DSound_1_0_5233);
