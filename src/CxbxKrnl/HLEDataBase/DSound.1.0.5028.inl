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
// * XFileCreateMediaObjectEx
// ******************************************************************
OOVPA_NO_XREF(XFileCreateMediaObjectEx, 5028, 8)

        { 0x03, 0x56 },
        { 0x22, 0x1B },
        { 0x89, 0x1B },
        { 0xA9, 0x85 },
        { 0xAA, 0xF6 },
        { 0xAB, 0x7C },
        { 0xAC, 0x0C },
        { 0xAD, 0x57 },
OOVPA_END;

// ******************************************************************
// * DSound_5028
// ******************************************************************
OOVPATable DSound_5028[] = {

	REGISTER_OOVPA(DirectSoundCreate, 4134, PATCH),
	REGISTER_OOVPA(DirectSoundDoWork, 4134, PATCH),
	REGISTER_OOVPA(DirectSoundGetSampleTime, 4361, PATCH),
	REGISTER_OOVPA(CDirectSound_CreateSoundBuffer, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetPlayRegion, 4361, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetPlayRegion, 4361, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_SetLoopRegion, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetLoopRegion, 4039, PATCH),
	REGISTER_OOVPA(CDirectSound_SetI3DL2Listener, 4134, XREF),
	REGISTER_OOVPA(IDirectSound_SetI3DL2Listener, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_SetMixBinHeadroom, 4627, XREF),
	REGISTER_OOVPA(IDirectSound_SetMixBinHeadroom, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_SetHeadroom, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetHeadroom, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_SetVelocity, 4627, XREF),
	REGISTER_OOVPA(IDirectSound_SetVelocity, 3936, PATCH),
	REGISTER_OOVPA(CDirectSound_SetAllParametersA, 4627, XREF),
	REGISTER_OOVPA(CDirectSound_SetAllParametersA, 4721, XREF),
	REGISTER_OOVPA(CDirectSound_SetAllParametersA, 4831, XREF),
	REGISTER_OOVPA(CDirectSound_SetAllParameters, 4831, XREF),
	REGISTER_OOVPA(IDirectSound_SetAllParameters, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundVoiceSettings_SetMixBins, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetMixBins, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMixBins, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMixBins, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoiceSettings_SetMixBinVolumes, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetMixBinVolumes, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMixBinVolumes, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMixBinVolumes2, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetMixBinVolumes2, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_SetPositionA, 4627, XREF),
	REGISTER_OOVPA(CDirectSound_SetPositionA, 4134, XREF), // TODO: Find a cure for laziness...
	REGISTER_OOVPA(IDirectSound_SetPosition, 3936, PATCH),
	REGISTER_OOVPA(DirectSoundCreateBuffer, 4242, PATCH),
	REGISTER_OOVPA(IDirectSound_CreateSoundBuffer, 4134, PATCH),
	REGISTER_OOVPA(IDirectSound_AddRef, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetFrequency, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetFrequency, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetFrequency, 4039, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_SetVolume, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetVolume, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetVolume, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetVolume, 4039, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetVolume, 4134, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_LockA, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Lock, 3936, PATCH),
	REGISTER_OOVPA(CDirectSound_CreateSoundStream, 4134, XREF),
	REGISTER_OOVPA(IDirectSound_CreateSoundStream, 3936, PATCH),
	REGISTER_OOVPA(DirectSoundCreateStream, 4134, PATCH),
	REGISTER_OOVPA(CMcpxStream_Pause, 4361, XREF),
	REGISTER_OOVPA(CMcpxStream_Pause, 4831, XREF),
	REGISTER_OOVPA(CDirectSoundStream_Pause, 4361, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_SetBufferData, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetBufferData, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetBufferData, 4134, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_GetStatus, 4134, XREF),
	REGISTER_OOVPA(CMcpxBuffer_GetStatus, 4721, XREF),
	REGISTER_OOVPA(CMcpxBuffer_GetStatus, 4831, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_GetStatus, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_GetStatus, 4134, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_SetCurrentPosition, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetCurrentPosition, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetCurrentPosition, 4134, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_GetCurrentPosition, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_GetCurrentPosition, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_GetCurrentPosition, 3936, PATCH),
	REGISTER_OOVPA(CDirectSound_GetSpeakerConfig, 4627, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_Play, 4361, XREF),
	REGISTER_OOVPA(CMcpxBuffer_Play, 4721, XREF),
	REGISTER_OOVPA(CMcpxBuffer_Play, 4831, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_Play, 4361, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Play, 4361, PATCH),
	REGISTER_OOVPA(IDirectSound_Release, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound_DownloadEffectsImage, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound_SetOrientation, 3936, PATCH),
	REGISTER_OOVPA(CDirectSound_SetDistanceFactor, 4134, XREF),
	REGISTER_OOVPA(CDirectSound_SetDistanceFactor, 4627, XREF),
	REGISTER_OOVPA(IDirectSound_SetDistanceFactor, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_SetRolloffFactor, 4134, XREF),
	REGISTER_OOVPA(IDirectSound_SetRolloffFactor, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_SetDopplerFactor, 4134, XREF),
	REGISTER_OOVPA(CDirectSound_SetDopplerFactor, 4627, XREF),
	REGISTER_OOVPA(IDirectSound_SetDopplerFactor, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_CommitDeferredSettings, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetMaxDistance, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMaxDistance, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMaxDistance, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetMaxDistance, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetMinDistance, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMinDistance, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMinDistance, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetMinDistance, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetRolloffFactor, 4134, XREF), // s+
	REGISTER_OOVPA(CDirectSoundBuffer_SetRolloffFactor, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetRolloffFactor, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetRolloffFactor, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetDistanceFactor, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetDistanceFactor, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetDistanceFactor, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetConeAngles, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetConeAngles, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetConeAngles, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetConeOrientation, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetConeOrientation, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetConeOrientation, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetConeOrientation, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetConeOutsideVolume, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetConeOutsideVolume, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetConeOutsideVolume, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetConeOutsideVolume, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetPosition, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetPosition, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetPosition, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetPosition, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetVelocity, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetVelocity, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetVelocity, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetVelocity, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetDopplerFactor, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetDopplerFactor, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetDopplerFactor, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetI3DL2Source, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetI3DL2Source, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetI3DL2Source, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetI3DL2Source, 4134, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_Stop, 4039, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_Release, 3936, PATCH), // +s
	REGISTER_OOVPA(CDirectSoundVoice_SetFormat, 4721, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetFormat, 4242, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetFormat, 4242, PATCH),
	REGISTER_OOVPA(CDirectSound_EnableHeadphones, 4627, XREF),
	REGISTER_OOVPA(IDirectSound_EnableHeadphones, 4627, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_AddRef, 3936, PATCH),
	REGISTER_OOVPA(CDirectSound_GetOutputLevels, 4627, XREF),
	REGISTER_OOVPA(IDirectSound_GetOutputLevels, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetAllParameters, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetAllParameters, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetAllParameters, 4134, PATCH), // Use that for now. Okay, it's your call pal...
	REGISTER_OOVPA(CDirectSoundStream_SetAllParameters, 4134, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_Pause, 4831, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_Pause, 4928, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Pause, 4928, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_GetStatus, 4721, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_GetStatus, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetMixBins, 4627, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_SetEG, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetEG, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetEG, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_SetEG, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetEG, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_FlushEx, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundStream_FlushEx, 4627, PATCH),
	REGISTER_OOVPA(XAudioDownloadEffectsImage, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetMode, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMode, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMode, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetMode, 4134, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_SetFilter, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetFilter, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetFilter, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetFilter, 4627, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_SetFilter, 4134, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_Play2, 4361, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_PlayEx, 4361, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_PlayEx, 3936, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_Stop, 4361, XREF),
	REGISTER_OOVPA(CMcpxBuffer_Stop2, 4361, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_StopEx, 4361, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_StopEx, 4361, PATCH),
	REGISTER_OOVPA(CDirectSound_SetVelocity, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Stop, 4039, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_SetPitch, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetPitch, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetHeadroom, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetHeadroom, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetOutputBuffer, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetOutputBuffer, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetOutputBuffer, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetOutputBuffer, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetRolloffCurve, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetRolloffCurve, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetRolloffCurve, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetRolloffCurve, 4627, PATCH),
	REGISTER_OOVPA(XFileCreateMediaObjectEx, 5028, PATCH),
	REGISTER_OOVPA(XWaveFileCreateMediaObject, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_SetNotificationPositions, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetNotificationPositions, 4627, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_SetLFO, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetLFO, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetLFO, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetLFO, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetLFO, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetPitch, 4627, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_Unlock, 3936, PATCH),
	REGISTER_OOVPA(CDirectSound_GetCaps, 4361, XREF),
	REGISTER_OOVPA(IDirectSound_GetCaps, 3936, PATCH),
};

// ******************************************************************
// * DSound_5028_SIZE
// ******************************************************************
uint32 DSound_5028_SIZE = sizeof(DSound_5028);
