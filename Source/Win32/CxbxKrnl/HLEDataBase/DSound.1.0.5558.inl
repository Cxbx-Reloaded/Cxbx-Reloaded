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
