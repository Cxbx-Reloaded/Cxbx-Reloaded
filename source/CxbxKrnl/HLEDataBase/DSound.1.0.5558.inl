// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->DSound.1.0.5558.cpp
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
// * CDirectSound::SynchPlayback
// ******************************************************************
SOOVPA<7> CDirectSound_SynchPlayback_1_0_5558 = 
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0xE8 },
        { 0x3A, 0x0B },
        { 0x46, 0x8B },
    }
};

// ******************************************************************
// * CMcpxStream_Pause
// ******************************************************************
SOOVPA<11> CMcpxStream_Pause_1_0_5558 =
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
// * CDirectSound::CommitDeferredSettings
// ******************************************************************
SOOVPA<11> CDirectSound_CommitDeferredSettings_1_0_5558 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // CDirectSound_CommitDeferredSettings+0x10 : movzx eax, al
        { 0x10, 0x0F }, // (Offset,Value)-Pair #1
        { 0x11, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x12, 0xC0 }, // (Offset,Value)-Pair #3

        // CDirectSound_CommitDeferredSettings+0x27 : mov eax, 0x80004005
        { 0x27, 0xB8 }, // (Offset,Value)-Pair #4
        { 0x28, 0x05 }, // (Offset,Value)-Pair #5
        { 0x29, 0x40 }, // (Offset,Value)-Pair #6
        { 0x2B, 0x80 }, // (Offset,Value)-Pair #7

        // CDirectSound_CommitDeferredSettings+0x6D : mov [eax+0x30], edi
        { 0x6D, 0x89 }, // (Offset,Value)-Pair #8
        { 0x6E, 0x78 }, // (Offset,Value)-Pair #9
        { 0x6F, 0x30 }, // (Offset,Value)-Pair #10

        // CDirectSound_CommitDeferredSettings+0x96 : leave
        { 0x96, 0xC9 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * CDirectSound::SetDistanceFactorA
// ******************************************************************
SOOVPA<11> CDirectSound_SetDistanceFactorA_1_0_5558 =
{
    0,  // Large == 0
    11, // Count == 11

    XREF_SETDISTANCEFACTORA,// XRef Is Saved
    0,                      // XRef Not Used

    {
        // CDirectSound_SetDistanceFactorA+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x22, 0x05 }, // (Offset,Value)-Pair #2
        { 0x23, 0x40 }, // (Offset,Value)-Pair #3
        { 0x24, 0x00 }, // (Offset,Value)-Pair #4
        { 0x25, 0x80 }, // (Offset,Value)-Pair #5

        // CDirectSound_SetDistanceFactorA+0x39 : or dword ptr[eax+0x30], 0x08
        { 0x39, 0x83 }, // (Offset,Value)-Pair #6
        { 0x3A, 0x48 }, // (Offset,Value)-Pair #7
        { 0x3B, 0x30 }, // (Offset,Value)-Pair #8
        { 0x3C, 0x08 }, // (Offset,Value)-Pair #9

        // CDirectSound_SetDistanceFactorA+0x4C : jz +0x0B
        { 0x4C, 0x74 }, // (Offset,Value)-Pair #10
        { 0x4D, 0x0B }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * CDirectSound::SetRolloffFactor
// ******************************************************************
SOOVPA<11> CDirectSound_SetRolloffFactor_1_0_5558 =
{
    0,  // Large == 0
    11, // Count == 11

    XREF_SETROLLOFFFACTORA, // XRef Is Saved
    0,                      // XRef Not Used

    {
        // CDirectSound_SetRolloffFactor+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x22, 0x05 }, // (Offset,Value)-Pair #2
        { 0x23, 0x40 }, // (Offset,Value)-Pair #3
        { 0x24, 0x00 }, // (Offset,Value)-Pair #4
        { 0x25, 0x80 }, // (Offset,Value)-Pair #5

        // CDirectSound_SetRolloffFactor+0x39 : or dword ptr[eax+0x30], 0x10
        { 0x39, 0x83 }, // (Offset,Value)-Pair #6
        { 0x3A, 0x48 }, // (Offset,Value)-Pair #7
        { 0x3B, 0x30 }, // (Offset,Value)-Pair #8
        { 0x3C, 0x10 }, // (Offset,Value)-Pair #9

        // CDirectSound_SetRolloffFactor+0x4C : jz +0x0B
        { 0x4C, 0x74 }, // (Offset,Value)-Pair #10
        { 0x4D, 0x0B }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * CDirectSound::SetDopplerFactor
// ******************************************************************
SOOVPA<14> CDirectSound_SetDopplerFactor_1_0_5558 =
{
    0,  // Large == 0
    14, // Count == 14

    XREF_SETDOPPLERFACTOR,  // XRef Is Saved
    0,                      // XRef Not Used

    {
        // CDirectSound_SetDopplerFactor+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x22, 0x05 }, // (Offset,Value)-Pair #2
        { 0x23, 0x40 }, // (Offset,Value)-Pair #3
        { 0x24, 0x00 }, // (Offset,Value)-Pair #4
        { 0x25, 0x80 }, // (Offset,Value)-Pair #5

        // CDirectSound_SetDopplerFactor+0x33 : mov [eax+0x70], edx
        { 0x33, 0x89 }, // (Offset,Value)-Pair #6
        { 0x34, 0x50 }, // (Offset,Value)-Pair #7
        { 0x35, 0x70 }, // (Offset,Value)-Pair #8

        // CDirectSound_SetDopplerFactor+0x39 : or dword ptr[eax+0x30], 0x20
        { 0x39, 0x83 }, // (Offset,Value)-Pair #9
        { 0x3A, 0x48 }, // (Offset,Value)-Pair #10
        { 0x3B, 0x30 }, // (Offset,Value)-Pair #11
        { 0x3C, 0x20 }, // (Offset,Value)-Pair #12

        // CDirectSound_SetDopplerFactor+0x4C : jz +0x0B
        { 0x4C, 0x74 }, // (Offset,Value)-Pair #13
        { 0x4D, 0x0B }, // (Offset,Value)-Pair #14
    }
};

// ******************************************************************
// * CDirectSoundStream_FlushEx
// ******************************************************************
SOOVPA<11> CDirectSoundStream_FlushEx_1_0_5558 =
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
SOOVPA<9> IDirectSoundBuffer8_StopEx_1_0_5558 =
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
// * Direct_1_0_5558
// ******************************************************************
OOVPATable DSound_1_0_5558[] =
{
    // DirectSoundCreate (* unchanged since 4361 *)
    {
        (OOVPA*)&DirectSoundCreate_1_0_4361,

        XTL::EmuDirectSoundCreate,

        #ifdef _DEBUG_TRACE
        "EmuDirectSoundCreate" 
        #endif
    },
    // CDirectSound_CreateSoundBuffer (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSound_CreateSoundBuffer_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound::CreateSoundBuffer (XREF)" 
        #endif
    },
    // CDirectSound::SynchPlayback
    {
        (OOVPA*)&CDirectSound_SynchPlayback_1_0_5558,

        XTL::EmuCDirectSound_SynchPlayback,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SynchPlayback_1_0_5558"
        #endif
    },
    // CMcpxStream::Pause
    {
        (OOVPA*)&CMcpxStream_Pause_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxStream_Pause (XREF)"
        #endif
    },
    // CDirectSoundStream::Pause (* unchanged since 4361 *)
    {
        (OOVPA*)&CDirectSoundStream_Pause_1_0_4361,
            
        XTL::EmuCDirectSoundStream_Pause,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_Pause"
        #endif
    },
    // CDirectSoundVoiceSettings_SetMixBinVolumes (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSoundVoiceSettings_SetMixBinVolumes_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoiceSettings::SetMixBinVolumes (XREF)" 
        #endif
    },
    // CDirectSoundVoice_SetMixBinVolumes (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSoundVoice_SetMixBinVolumes_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetMixBinVolumes (XREF)" 
        #endif
    },
    // CDirectSoundBuffer_SetMixBinVolumes (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSoundBuffer_SetMixBinVolumes_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetMixBinVolumes (XREF)" 
        #endif
    },
    // IDirectSoundBuffer8_SetMixBinVolumes (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirectSoundBuffer8_SetMixBinVolumes_1_0_4627,

        XTL::EmuIDirectSoundBuffer8_SetMixBinVolumes,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetMixBinVolumes"
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
    // CDirectSound::CommitDeferredSettings
    {
        (OOVPA*)&CDirectSound_CommitDeferredSettings_1_0_5558,

        XTL::EmuCDirectSound_CommitDeferredSettings,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSound_CommitDeferredSettings" 
        #endif
    },
    // CDirectSound::SetDistanceFactorA (XREF)
    {
        (OOVPA*)&CDirectSound_SetDistanceFactorA_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetDistanceFactorA (XRef)"
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
    // CDirectSound::SetRolloffFactor (XREF)
    {
        (OOVPA*)&CDirectSound_SetRolloffFactor_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetRolloffFactor (XRef)"
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
    // CDirectSound::SetDopplerFactor (XREF)
    {
        (OOVPA*)&CDirectSound_SetDopplerFactor_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetDopplerFactor (XRef)"
        #endif
    },
    // CDirectSoundStream_FlushEx
    {
        (OOVPA*)&CDirectSoundStream_FlushEx_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream::FlushEx (XREF)" 
        #endif
    },
    // IDirectSoundBuffer8::StopEx
    {
        (OOVPA*)&IDirectSoundBuffer8_StopEx_1_0_5558,

        XTL::EmuIDirectSoundBuffer8_StopEx,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_StopEx"
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
    // CDirectSoundVoice::SetFrequency (XREF) (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSoundVoice_SetFrequency_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice_SetFrequency (XRef)"
        #endif
    },
    // CDirectSoundBuffer::SetFrequency (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSoundBuffer_SetFrequency_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSoundBuffer_SetFrequency (XREF)" 
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
    // CMcpxVoiceClient_SetVolume (* unchanged since 4627 *)
    {
        (OOVPA*)&CMcpxVoiceClient_SetVolume_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxVoiceClient::SetVolume (XREF)" 
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
    // CDirectSound_CreateSoundStream (* unchanged since 4361 *)
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
    // CDirectSoundVoice::SetMaxDistance (XREF) (* unchanged since 4432 *)
    {
        (OOVPA*)&CDirectSoundVoice_SetMaxDistance_1_0_4432, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetMaxDistance (XRef)" 
        #endif
    },
    // CDirectSoundBuffer::SetMaxDistance (XREF) (* unchanged since 4432 *)
    {
        (OOVPA*)&CDirectSoundBuffer_SetMaxDistance_1_0_4432, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetMaxDistance (XRef)" 
        #endif
    },
    // IDirectSoundBuffer_SetMaxDistance (* unchanged since 4432 *)
    {
        (OOVPA*)&IDirectSoundBuffer_SetMaxDistance_1_0_4432,
        XTL::EmuIDirectSoundBuffer8_SetMaxDistance,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetMaxDistance" 
        #endif
    },
    // CDirectSoundVoice::SetMinDistance (XREF) (* unchanged since 4432 *)
    {
        (OOVPA*)&CDirectSoundVoice_SetMinDistance_1_0_4432, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetMinDistance (XRef)" 
        #endif
    },
    // CDirectSoundVoice::SetRolloffFactor (XREF) (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSoundVoice_SetRolloffFactor_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetRolloffFactor (XRef)"
        #endif
    },
    // CDirectSoundBuffer::SetRolloffFactor (XREF) (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSoundBuffer_SetRolloffFactor_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetRolloffFactor (XRef)"
        #endif
    },
    // IDirectSoundBuffer_SetRolloffFactor (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirectSoundBuffer_SetRolloffFactor_1_0_4627,
        XTL::EmuIDirectSoundBuffer8_SetRolloffFactor,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetRolloffFactor" 
        #endif
    },
    // CDirectSoundVoice::SetDopplerFactor (XREF) (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSoundVoice_SetDopplerFactor_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetDopplerFactor (XRef)"
        #endif
    },
    // CDirectSoundBuffer::SetDopplerFactor (XREF) (* unchanged since 4627 *)
    {
        (OOVPA*)&CDirectSoundBuffer_SetDopplerFactor_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetDopplerFactor (XRef)"
        #endif
    },
    // IDirectSoundBuffer_SetDopplerFactor (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirectSoundBuffer_SetDopplerFactor_1_0_4627,
        XTL::EmuIDirectSoundBuffer8_SetDopplerFactor,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetDopplerFactor" 
        #endif
    },
    // IDirectSoundBuffer_Stop (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirectSoundBuffer_Stop_1_0_4627,
        XTL::EmuIDirectSoundBuffer8_Stop,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_Stop"
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
};

// ******************************************************************
// * DSound_1_0_5558_SIZE
// ******************************************************************
uint32 DSound_1_0_5558_SIZE = sizeof(DSound_1_0_5558);
