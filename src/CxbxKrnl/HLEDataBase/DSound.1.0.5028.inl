// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->DSound.1.0.5028.cpp
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
// * DirectSound::CDirectSoundVoice::SetFormat
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetFormat, 5028, 8,

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
// * Direct, 5028
// ******************************************************************
OOVPATable DSound_5028[] = {

	REGISTER_OOVPA(DirectSoundCreate, 4134, PATCH),
	REGISTER_OOVPA(DirectSoundDoWork, 4134, PATCH),
	REGISTER_OOVPA(DirectSoundGetSampleTime, 4627, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_CreateSoundBuffer, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetPlayRegion, 4361, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetPlayRegion, 4361, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetLoopRegion, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetLoopRegion, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetI3DL2Listener, 4134, XREF),
	REGISTER_OOVPA(IDirectSound_SetI3DL2Listener, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetMixBinHeadroom, 4627, XREF),
	REGISTER_OOVPA(IDirectSound_SetMixBinHeadroom, 4627, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_SetHeadroomA, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetHeadroom, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetVelocity, 4627, XREF),
	REGISTER_OOVPA(IDirectSound_SetVelocity, 3936, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetAllParametersA, 4627, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetAllParametersA, 4721, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetAllParametersA, 4831, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetAllParameters, 4928, XREF),
	REGISTER_OOVPA(IDirectSound_SetAllParameters, 3936, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoiceSettings_SetMixBins, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetMixBins, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetMixBins, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMixBins, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoiceSettings_SetMixBinVolumes, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetMixBinVolumes, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetMixBinVolumes, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMixBinVolumes2, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetMixBinVolumes2, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetPositionA, 4627, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetPositionA, 4134, XREF), // TODO: Find a cure for laziness...
	REGISTER_OOVPA(IDirectSound_SetPosition, 3936, PATCH),
	REGISTER_OOVPA(DirectSoundCreateBuffer, 4627, PATCH),
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
	REGISTER_OOVPA(DirectSound_CDirectSound_CreateSoundStream, 4361, XREF),
	REGISTER_OOVPA(IDirectSound_CreateSoundStream, 3936, PATCH),
	REGISTER_OOVPA(DirectSoundCreateStream, 4361, PATCH),
	REGISTER_OOVPA(CMcpxStream_Pause, 4361, XREF),
	REGISTER_OOVPA(CMcpxStream_Pause, 4928, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_Pause, 4361, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_SetBufferData, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetBufferData, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetBufferData, 4134, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_GetStatus, 4134, XREF),
	REGISTER_OOVPA(CMcpxBuffer_GetStatus, 4721, XREF),
    // REGISTER_OOVPA(CMcpxBuffer_GetStatus, 4928, XREF)
	REGISTER_OOVPA(CMcpxBuffer_GetStatus, 4831, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_GetStatus, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_GetStatus, 4134, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_SetCurrentPosition, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetCurrentPosition, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetCurrentPosition, 4134, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_GetCurrentPosition, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_GetCurrentPosition, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_GetCurrentPosition, 3936, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_GetSpeakerConfig, 4627, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_Play, 4361, XREF),
	REGISTER_OOVPA(CMcpxBuffer_Play, 4721, XREF),
	REGISTER_OOVPA(CMcpxBuffer_Play, 4928, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_Play, 4361, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Play, 4361, PATCH),
	REGISTER_OOVPA(IDirectSound_Release, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound_DownloadEffectsImage, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound_SetOrientation, 3936, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetDistanceFactorA, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetDistanceFactorA, 4627, XREF),
	REGISTER_OOVPA(IDirectSound_SetDistanceFactor, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetRolloffFactor, 4134, XREF),
	REGISTER_OOVPA(IDirectSound_SetRolloffFactor, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetDopplerFactor, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetDopplerFactor, 4627, XREF),
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
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetPosition, 4627, XREF),
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
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetFormat, 5028, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetFormat, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetFormat, 4627, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_EnableHeadphones, 4627, XREF),
	REGISTER_OOVPA(IDirectSound_EnableHeadphones, 4627, PATCH),
	// REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetHeadroom, 4627, XREF),
	// REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetHeadroom, 4627, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_AddRef, 3936, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_GetOutputLevels, 4627, XREF),
	REGISTER_OOVPA(IDirectSound_GetOutputLevels, 4627, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetAllParameters, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetAllParameters, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetAllParameters, 4134, PATCH), // Use that for now. Okay, it's your call pal...
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetAllParameters, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CMcpxBuffer_Pause, 4928, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_Pause, 4928, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Pause, 4928, PATCH),
	REGISTER_OOVPA(DirectSound_CMcpxBuffer_GetStatus, 4721, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_GetStatus, 4721, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetMixBins, 4627, PATCH),
	REGISTER_OOVPA(DirectSound_CMcpxVoiceClient_SetEG, 4627, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetEG, 4627, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetEG, 4627, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetEG, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetEG, 4627, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_FlushEx, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundStream_FlushEx, 4627, PATCH),
	REGISTER_OOVPA(XAudioDownloadEffectsImage, 4627, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetMode, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetMode, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMode, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetMode, 4134, PATCH),
	REGISTER_OOVPA(DirectSound_CMcpxVoiceClient_SetFilter, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetFilter, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetFilter, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetFilter, 4627, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_SetFilter, 4134, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_PlayA, 4361, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_PlayEx, 4361, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_PlayEx, 3936, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_Stop, 4361, XREF),
	REGISTER_OOVPA(CMcpxBuffer_Stop2, 4361, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_StopEx, 4361, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_StopEx, 4361, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSound_SetVelocity, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetPitch, 4134, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetPitch, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetPitch, 4627, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetHeadroom, 4627, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetHeadroom, 4627, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetOutputBuffer, 4627, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetOutputBuffer, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetOutputBuffer, 4627, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetOutputBuffer, 4627, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetRolloffCurve, 4627, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetRolloffCurve, 4627, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetRolloffCurve, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetRolloffCurve, 4627, PATCH),
	REGISTER_OOVPA(XFileCreateMediaObjectEx, 4627, PATCH),
	REGISTER_OOVPA(XWaveFileCreateMediaObject, 4627, PATCH),
	// REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetNotificationPositions, 4627, XREF),
	// REGISTER_OOVPA(IDirectSoundBuffer_SetNotificationPositions, 4627, XREF),
	REGISTER_OOVPA(CMcpxVoiceClient_SetLFO, 4627, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundVoice_SetLFO, 4627, XREF),
	REGISTER_OOVPA(DirectSound_CDirectSoundBuffer_SetLFO, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetLFO, 4627, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetLFO, 4627, PATCH),
	REGISTER_OOVPA(DirectSound_CDirectSoundStream_SetPitch, 4627, PATCH),
};

// ******************************************************************
// * DSound_5028_SIZE
// ******************************************************************
uint32 DSound_5028_SIZE = sizeof(DSound_5028);
