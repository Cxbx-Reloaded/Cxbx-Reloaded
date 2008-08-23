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
    // IDirectSoundBuffer_SetPosition (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirectSoundBuffer_SetPosition_1_0_4627,

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
    // IDirectSoundBuffer_SetVelocity (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirectSoundBuffer_SetVelocity_1_0_4627,

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
};

// ******************************************************************
// * DSound_1_0_5233_SIZE
// ******************************************************************
uint32 DSound_1_0_5233_SIZE = sizeof(DSound_1_0_5233);
