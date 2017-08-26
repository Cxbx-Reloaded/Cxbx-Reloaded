// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDatabase->DSound.OOVPA.inl
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
#ifndef DSOUND_OOVPA_INL
#define DSOUND_OOVPA_INL

#include "../OOVPA.h"

#include "DSound.1.0.3936.inl"
#include "DSound.1.0.4134.inl"
#include "DSound.1.0.4361.inl"
#include "DSound.1.0.4432.inl"
#include "DSound.1.0.4627.inl"
#include "DSound.1.0.5028.inl"
#include "DSound.1.0.5233.inl"
#include "DSound.1.0.5344.inl"
#include "DSound.1.0.5558.inl"
#include "DSound.1.0.5788.inl"
#include "DSound.1.0.5849.inl"


// ******************************************************************
// * DSOUND_OOVPA
// ******************************************************************
OOVPATable DSound_OOVPAV2[] = {
    REGISTER_OOVPAS(CDirectSoundBuffer_GetCurrentPosition, XREF, 3936, 4134, 5558),
    REGISTER_OOVPAS(CDirectSoundBuffer_GetStatus, XREF, 3936, 4134, 4721),
    REGISTER_OOVPAS(CDirectSoundBuffer_Pause, XREF, 4928),
    REGISTER_OOVPAS(CDirectSoundBuffer_Play, XREF, 3936, 4134, 4361, 5558, 5788),
    REGISTER_OOVPAS(CDirectSoundBuffer_PlayEx, XREF, 3936, 4361, 5788),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetAllParameters, XREF, 4134),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetBufferData, XREF, 4134, 5558, 5788),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetConeAngles, XREF, 4134, 5558),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetConeOrientation, XREF, 3936, 4134),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetConeOutsideVolume, XREF, 3936, 4134),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetCurrentPosition, XREF, 4134, 5558, 5788),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetDistanceFactor, XREF, 4134, 5558),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetDopplerFactor, XREF, 4134, 5558),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetEG, XREF, 4627),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetHeadroom, XREF, 4134, 5344, 5558),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetFilter, XREF, 4134),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetLFO, XREF, 4627),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetFormat, XREF, 4242, 5558),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetFrequency, XREF, 4134),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetI3DL2Source, XREF, 4134, 5558),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetLoopRegion, XREF, 4134, 5558),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetMaxDistance, XREF, 3936, 4134, 5344, 5788),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetMinDistance, XREF, 3936, 4134, 5344, 5788),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetMixBins, XREF, 4134, 5558),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetMixBinVolumes, XREF, 4134),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetMode, XREF, 4134, 5233),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetNotificationPositions, XREF, 3936, 4627), // Uncommenting these fixes dashboard 4920 (from 4627 comment)
    REGISTER_OOVPAS(CDirectSoundBuffer_SetOutputBuffer, XREF, 4627),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetPitch, XREF, 4627),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetPlayRegion, XREF, 4361, 5558),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetPosition, XREF, 3936, 4134, 5558),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetRolloffCurve, XREF, 4627),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetRolloffFactor, XREF, 4134, 5788),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetVelocity, XREF, 3936, 4134, 5558),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetVolume, XREF, 4134, 5233),
    REGISTER_OOVPAS(CDirectSoundBuffer_Stop, XREF, 3936, 4361),
    REGISTER_OOVPAS(CDirectSoundBuffer_StopEx, XREF, 4361),
    REGISTER_OOVPAS(CDirectSoundBuffer_Use3DVoiceData, XREF, 5558),
    REGISTER_OOVPAS(CDirectSoundStream_FlushEx, XREF, 4627, 5233, 5788),
    REGISTER_OOVPAS(CDirectSoundStream_Pause, PATCH, 3936, 4361, 5558),
    REGISTER_OOVPAS(CDirectSoundStream_SetAllParameters, PATCH, 3936, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_SetConeAngles, PATCH, 3936),
    REGISTER_OOVPAS(CDirectSoundStream_SetConeAnglesA, XREF, 3936),
    REGISTER_OOVPAS(CDirectSoundStream_SetConeAnglesB, XREF, 3936),
    REGISTER_OOVPAS(CDirectSoundStream_SetConeOrientation, PATCH, 3936, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_SetConeOrientationA, XREF, 3936),
    REGISTER_OOVPAS(CDirectSoundStream_SetConeOrientationB, XREF, 3936),
    REGISTER_OOVPAS(CDirectSoundStream_SetConeOrientationC, XREF, 3936),
    REGISTER_OOVPAS(CDirectSoundStream_SetConeOutsideVolume, PATCH, 3936, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_SetConeOutsideVolumeA, XREF, 3936),
    REGISTER_OOVPAS(CDirectSoundStream_SetConeOutsideVolumeB, XREF, 3936),
    REGISTER_OOVPAS(CDirectSoundStream_SetEG, PATCH, 4627),
    REGISTER_OOVPAS(CDirectSoundStream_SetFilter, PATCH, 4627),
    REGISTER_OOVPAS(CDirectSoundStream_SetLFO, PATCH, 4627),
    REGISTER_OOVPAS(CDirectSoundStream_SetFormat, PATCH, 4242),
    REGISTER_OOVPAS(CDirectSoundStream_SetFrequency, PATCH, 3936, 5233),
    REGISTER_OOVPAS(CDirectSoundStream_SetFrequencyA, XREF, 3936),
    REGISTER_OOVPAS(CDirectSoundStream_SetFrequencyB, XREF, 3936),
    REGISTER_OOVPAS(CDirectSoundStream_SetHeadroom, PATCH, 3936, 4627, 5558),
    REGISTER_OOVPAS(CDirectSoundStream_SetI3DL2Source, PATCH, 3936, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_SetMaxDistance, PATCH, 3936, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_SetMaxDistanceA, XREF, 3936),
    REGISTER_OOVPAS(CDirectSoundStream_SetMaxDistanceB, XREF, 3936),
    REGISTER_OOVPAS(CDirectSoundStream_SetMaxDistanceC, XREF, 3936),
    REGISTER_OOVPAS(CDirectSoundStream_SetMinDistance, PATCH, 3936, 4134, 5344),
    REGISTER_OOVPAS(CDirectSoundStream_SetMinDistanceA, XREF, 3936),
    REGISTER_OOVPAS(CDirectSoundStream_SetMinDistanceB, XREF, 3936),
    REGISTER_OOVPAS(CDirectSoundStream_SetMinDistanceC, XREF, 3936),
    REGISTER_OOVPAS(CDirectSoundStream_SetMixBins, PATCH, 3936, 4627, 5233, 5558),
    REGISTER_OOVPAS(CDirectSoundStream_SetMixBinVolumes, PATCH, 3936), //TODO: What's the difference from this and below?
    REGISTER_OOVPAS(CDirectSoundStream_SetMixBinVolumes2, PATCH, 4134, 5788), //TODO: What's the difference from this and above?
    REGISTER_OOVPAS(CDirectSoundStream_SetMode, PATCH, 3936, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_SetOutputBuffer, PATCH, 4627),
    REGISTER_OOVPAS(CDirectSoundStream_SetPitch, PATCH, 4627, 5849),
    REGISTER_OOVPAS(CDirectSoundStream_SetPosition, PATCH, 3936, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_SetPositionA, XREF, 3936),
    REGISTER_OOVPAS(CDirectSoundStream_SetPositionB, XREF, 3936),
    REGISTER_OOVPAS(CDirectSoundStream_SetPositionC, XREF, 3936),
    REGISTER_OOVPAS(CDirectSoundStream_SetRolloffCurve, XREF, 4627, 5558),
    REGISTER_OOVPAS(CDirectSoundStream_SetRolloffFactor, PATCH, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_SetVelocity, PATCH, 3936, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_SetVelocityA, XREF, 3936),
    REGISTER_OOVPAS(CDirectSoundStream_SetVelocityB, XREF, 3936),
    REGISTER_OOVPAS(CDirectSoundStream_SetVelocityC, XREF, 3936),
    REGISTER_OOVPAS(CDirectSoundStream_SetVolume, PATCH, 3936, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_Use3DVoiceData, XREF, 5558),
    REGISTER_OOVPAS(CDirectSoundVoiceSettings_SetMixBins, XREF, 4134, 5344, 5558),
    REGISTER_OOVPAS(CDirectSoundVoiceSettings_SetMixBinVolumes, XREF, 4134),
    REGISTER_OOVPAS(CDirectSoundVoice_CommitDeferredSettings, XREF, 5558),
    REGISTER_OOVPAS(CDirectSoundVoice_SetAllParameters, XREF, 3936, 4134, 5344, 5558),
    REGISTER_OOVPAS(CDirectSoundVoice_SetConeAngles, XREF, 3936, 4134, 5344, 5558),
    REGISTER_OOVPAS(CDirectSoundVoice_SetConeOrientation, XREF, 4134, 5344),
    REGISTER_OOVPAS(CDirectSoundVoice_SetConeOutsideVolume, XREF, 4134, 4361, 5344),
    REGISTER_OOVPAS(CDirectSoundVoice_SetFilter, XREF, 4134),
    REGISTER_OOVPAS(CDirectSoundVoice_SetLFO, XREF, 4627),
    REGISTER_OOVPAS(CDirectSoundVoice_SetFormat, XREF, 4242, 4721, 5558),
    REGISTER_OOVPAS(CDirectSoundVoice_SetDistanceFactor, XREF, 4134, 4361, 4627, 5344, 5558),
    REGISTER_OOVPAS(CDirectSoundVoice_SetDopplerFactor, XREF, 4134, 4361, 4627, 5344, 5558),
    REGISTER_OOVPAS(CDirectSoundVoice_SetEG, XREF, 4627),
    REGISTER_OOVPAS(CDirectSoundVoice_SetFrequency, XREF, 4134),
    REGISTER_OOVPAS(CDirectSoundVoice_SetHeadroom, XREF, 3936, 4134, 4627, 5558),
    REGISTER_OOVPAS(CDirectSoundVoice_SetI3DL2Source, XREF, 3936, 4134, 5344, 5558),
    REGISTER_OOVPAS(CDirectSoundVoice_SetMaxDistance, XREF, 4134, 4361, 5344),
    REGISTER_OOVPAS(CDirectSoundVoice_SetMinDistance, XREF, 4134, 4361, 5344),
    REGISTER_OOVPAS(CDirectSoundVoice_SetMixBins, XREF, 3936, 4134, 5558),
    REGISTER_OOVPAS(CDirectSoundVoice_SetMixBinVolumes, XREF, 3936, 4134),
    REGISTER_OOVPAS(CDirectSoundVoice_SetMode, XREF, 3936, 4134, 5233, 5344),
    REGISTER_OOVPAS(CDirectSoundVoice_SetOutputBuffer, XREF, 4627),
    REGISTER_OOVPAS(CDirectSoundVoice_SetPitch, XREF, 4134),
    REGISTER_OOVPAS(CDirectSoundVoice_SetPosition, XREF, 4134, 4627, 5344, 5558),
    REGISTER_OOVPAS(CDirectSoundVoice_SetRolloffCurve, XREF, 4627, 5344),
    REGISTER_OOVPAS(CDirectSoundVoice_SetRolloffFactor, XREF, 4134, 4361, 5344, 5788), // s+ (from 4134's comment)
    REGISTER_OOVPAS(CDirectSoundVoice_SetVelocity, XREF, 4134, 5344),
    REGISTER_OOVPAS(CDirectSoundVoice_SetVolume, XREF, 4134, 5233),
    REGISTER_OOVPAS(CDirectSoundVoice_Use3DVoiceData, XREF, 5558),
    REGISTER_OOVPAS(CDirectSound_CommitDeferredSettings, PATCH, 3936, 4134, 5344, 5558, 5788),
    REGISTER_OOVPAS(CDirectSound_CommitDeferredSettingsA, XREF, 3936),
    REGISTER_OOVPAS(CDirectSound_CommitDeferredSettingsB, XREF, 3936),
    REGISTER_OOVPAS(CDirectSound_CreateSoundBuffer, XREF, 3936, 4134),
    REGISTER_OOVPAS(CDirectSound_CreateSoundStream, XREF, 3936, 4134, 5558),
    REGISTER_OOVPAS(CDirectSound_EnableHeadphones, XREF, 3936, 4627, 5233, 5344, 5558),
    REGISTER_OOVPAS(CDirectSound_GetCaps, XREF, 3936, 4361),
    REGISTER_OOVPAS(CDirectSound_GetOutputLevels, XREF, 4627, 5558),
    REGISTER_OOVPAS(CDirectSound_GetSpeakerConfig, PATCH, 4627),
    REGISTER_OOVPAS(CDirectSound_SetAllParameters, XREF, 3936, 4831, 5558),
    REGISTER_OOVPAS(CDirectSound_SetAllParametersA, XREF, 4627, 4721, 4831),
    REGISTER_OOVPAS(CDirectSound_SetDistanceFactor, XREF, 4134, 4627, 5344, 5558),
    REGISTER_OOVPAS(CDirectSound_SetDistanceFactorA, XREF, 3936),
    REGISTER_OOVPAS(CDirectSound_SetDistanceFactorB, XREF, 3936),
    REGISTER_OOVPAS(CDirectSound_SetDopplerFactor, XREF, 3936, 4134, 4627, 5344, 5558, 5788),
    REGISTER_OOVPAS(CDirectSound_SetEffectData, XREF, 4134),
    REGISTER_OOVPAS(CDirectSound_SetI3DL2Listener, XREF, 3936, 4134, 5344, 5558),
    REGISTER_OOVPAS(CDirectSound_SetMixBinHeadroom, XREF, 4627, 5558),
    REGISTER_OOVPAS(CDirectSound_SetMixBinHeadroomA, XREF, 3936),
    REGISTER_OOVPAS(CDirectSound_SetMixBinHeadroomB, XREF, 3936),
    REGISTER_OOVPAS(CDirectSound_SetPosition, XREF, 5558),
    REGISTER_OOVPAS(CDirectSound_SetPositionA, XREF, 3936, 4134, 4627, 5344), // TODO: Find a cure for laziness... (from 4134's comment)
    REGISTER_OOVPAS(CDirectSound_SetPositionB, XREF, 3936),
    REGISTER_OOVPAS(CDirectSound_SetRolloffFactor, XREF, 4134, 5344, 5558, 5788, 5849),
    REGISTER_OOVPAS(CDirectSound_SetRolloffFactorA, XREF, 3936),
    REGISTER_OOVPAS(CDirectSound_SetRolloffFactorB, XREF, 3936),
    REGISTER_OOVPAS(CDirectSound_SetVelocity, XREF, 4134, 4627, 5344, 5558),
    REGISTER_OOVPAS(CDirectSound_SetVelocityA, XREF, 3936),
    REGISTER_OOVPAS(CDirectSound_SetVelocityB, XREF, 3936),
    REGISTER_OOVPAS(CDirectSound_SynchPlayback, XREF, 5233, 5344, 5558),
    REGISTER_OOVPAS(CMcpxAPU_Set3dDopplerFactor, XREF, 3936),
    REGISTER_OOVPAS(CMcpxAPU_Set3dParameters, XREF, 3936),
    REGISTER_OOVPAS(CMcpxAPU_SynchPlayback, XREF, 5233),
    REGISTER_OOVPAS(CMcpxBuffer_GetCurrentPosition, XREF, 3936, 4134),
    REGISTER_OOVPAS(CMcpxBuffer_GetCurrentPosition2, XREF, 3936),
    REGISTER_OOVPAS(CMcpxBuffer_GetStatus, XREF, 3936, 4134, 4721, 4831, 5558),
    REGISTER_OOVPAS(CMcpxBuffer_Pause, XREF, 4831),
    REGISTER_OOVPAS(CMcpxBuffer_Play, XREF, 3925, 4361, 4721, 4831), // NOTE: ?Play@CMcpxBuffer@DirectSound@@QAEJK@Z
    REGISTER_OOVPAS(CMcpxBuffer_Play2, XREF, 4361, 5558), // NOTE: ?Play@CMcpxBuffer@DirectSound@@QAEJ_JK@Z
    REGISTER_OOVPAS(CMcpxBuffer_SetBufferData, XREF, 4134, 5788),
    REGISTER_OOVPAS(CMcpxBuffer_SetCurrentPosition, XREF, 4134, 5558, 5788),
    REGISTER_OOVPAS(CMcpxBuffer_Stop, XREF, 4361),
    REGISTER_OOVPAS(CMcpxBuffer_Stop2, XREF, 4361),
    REGISTER_OOVPAS(CMcpxStream_Pause, XREF, 4361, 4831, 5788),
    REGISTER_OOVPAS(CMcpxVoiceClient_Commit3dSettings, XREF, 3936),
    REGISTER_OOVPAS(CMcpxVoiceClient_Set3dMode, XREF, 3936),
    REGISTER_OOVPAS(CMcpxVoiceClient_Set3dParameters, XREF, 3936),
    REGISTER_OOVPAS(CMcpxVoiceClient_SetEG, XREF, 4627),
    REGISTER_OOVPAS(CMcpxVoiceClient_SetFilter, XREF, 4134, 5344, 5558),
    REGISTER_OOVPAS(CMcpxVoiceClient_SetLFO, XREF, 4627),
    REGISTER_OOVPAS(CMcpxVoiceClient_SetI3DL2Source, XREF, 3936),
    REGISTER_OOVPAS(CMcpxVoiceClient_SetMixBins, XREF, 3936),
    REGISTER_OOVPAS(CMcpxVoiceClient_SetVolume, XREF, 3936, 4134),
    REGISTER_OOVPAS(CSensaura3d_GetFullHRTFFilterPair, XREF, 3925, 3936),
    REGISTER_OOVPAS(DirectSoundCreate, PATCH, 3936, 4134),
    REGISTER_OOVPAS(DirectSoundCreateBuffer, PATCH, 4242),
    REGISTER_OOVPAS(DirectSoundCreateStream, PATCH, 4134, 5788),
    REGISTER_OOVPAS(DirectSoundDoWork, PATCH, 3936, 4134, 5558),
    REGISTER_OOVPAS(DirectSoundDoWorkA, XREF, 3936),
    REGISTER_OOVPAS(DirectSoundDoWorkB, XREF, 3936),
    REGISTER_OOVPAS(DirectSoundEnterCriticalSection, XREF, 3936),
    REGISTER_OOVPAS(DirectSoundGetSampleTime, PATCH, 3936, 4361),
    REGISTER_OOVPAS(DirectSoundUseFullHRTF, PATCH, 3925, 5558),
    REGISTER_OOVPAS(IDirectSoundBuffer_AddRef, PATCH, 3936),
    REGISTER_OOVPAS(IDirectSoundBuffer_GetCurrentPosition, PATCH, 3936, 5558),
    REGISTER_OOVPAS(IDirectSoundBuffer_GetStatus, PATCH, 3936, 4134),
    REGISTER_OOVPAS(IDirectSoundBuffer_Lock, PATCH, 3936, 5558),
    REGISTER_OOVPAS(IDirectSoundBuffer_LockA, XREF, 3936, 4134),
    REGISTER_OOVPAS(IDirectSoundBuffer_Pause, PATCH, 4928),
    REGISTER_OOVPAS(IDirectSoundBuffer_Play, PATCH, 3936, 4361, 5558),
    REGISTER_OOVPAS(IDirectSoundBuffer_PlayEx, PATCH, 3936),
    REGISTER_OOVPAS(IDirectSoundBuffer_Release, PATCH, 3936), // +s (from 4134's comment)
    REGISTER_OOVPAS(IDirectSoundBuffer_SetAllParameters, PATCH, 4134), // Use that for now. Okay, it's your call pal... (from 4134's comment)
    REGISTER_OOVPAS(IDirectSoundBuffer_SetBufferData, PATCH, 3936, 4134, 5558, 5788),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetBufferDataA, XREF, 3936),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetConeAngles, PATCH, 3936, 4134, 5558),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetConeOrientation, PATCH, 3936),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetConeOutsideVolume, PATCH, 3936),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetDistanceFactor, PATCH, 4134, 5558),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetDopplerFactor, PATCH, 4134, 5558),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetCurrentPosition, PATCH, 3936, 4134, 5558, 5788),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetCurrentPositionA, XREF, 3936),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetCurrentPositionB, XREF, 3936),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetEG, PATCH, 4627),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetLFO, PATCH, 4627),
    REGISTER_OOVPAS(IDirectSoundStream_FlushEx, PATCH, 4627),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetFilter, PATCH, 4134),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetFormat, PATCH, 4242, 5558),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetFrequency, PATCH, 3936, 4134),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetHeadroom, PATCH, 3936, 4134, 5344, 5558),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetHeadroomA, XREF, 3936),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetI3DL2Source, PATCH, 3936, 4134, 5558),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetLoopRegion, PATCH, 4134),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetMaxDistance, PATCH, 3936, 4134, 5344, 5788),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetMinDistance, PATCH, 3936, 4134, 5558, 5788),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetMixBins, PATCH, 3936, 4134, 5558),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetMixBinVolumes, PATCH, 3936), //TODO: What's the difference from this and below?
    REGISTER_OOVPAS(IDirectSoundBuffer_SetMixBinVolumes2, PATCH, 4134), //TODO: What's the difference from this and above?
    REGISTER_OOVPAS(IDirectSoundBuffer_SetMode, PATCH, 3936, 4134, 5233),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetNotificationPositions, PATCH, 3936, 4627),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetOutputBuffer, PATCH, 4627),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetPitch, PATCH, 3936, 4627),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetPitchA, XREF, 3936),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetPitchB, XREF, 3936),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetPlayRegion, PATCH, 3936, 4361),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetPlayRegionA, XREF, 3936),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetPosition, PATCH, 3936, 5558),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetRolloffCurve, PATCH, 4627),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetRolloffFactor, PATCH, 4134, 5788),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetVelocity, PATCH, 3936, 5558),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetVolume, PATCH, 3936, 4134),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetVolumeA, XREF, 3936),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetVolumeB, XREF, 3936),
    REGISTER_OOVPAS(IDirectSoundBuffer_Stop, PATCH, 3936, 4134, 4361),
    REGISTER_OOVPAS(IDirectSoundBuffer_StopEx, PATCH, 4361),
    REGISTER_OOVPAS(IDirectSoundBuffer_Unlock, PATCH, 3936),
    REGISTER_OOVPAS(IDirectSoundBuffer_Use3DVoiceData, PATCH, 5558),
    REGISTER_OOVPAS(IDirectSound_AddRef, PATCH, 3936),
    REGISTER_OOVPAS(IDirectSound_CreateSoundBuffer, PATCH, 3936/*, 4134, 4361*/),
    REGISTER_OOVPAS(IDirectSound_CreateSoundStream, PATCH, 3936, 5558),
    REGISTER_OOVPAS(IDirectSound_DownloadEffectsImage, PATCH, 3936),
    REGISTER_OOVPAS(IDirectSound_EnableHeadphones, PATCH, 3936, 4627, 5233, 5558),
    REGISTER_OOVPAS(IDirectSound_GetCaps, PATCH, 3936),
    REGISTER_OOVPAS(IDirectSound_GetOutputLevels, PATCH, 4627, 5558),
    REGISTER_OOVPAS(IDirectSound_Release, PATCH, 3936),
    REGISTER_OOVPAS(IDirectSound_SetAllParameters, PATCH, 3936),
    REGISTER_OOVPAS(IDirectSound8_SetAllParameters, PATCH, 3936),
    REGISTER_OOVPAS(IDirectSound_SetDistanceFactor, PATCH, 3936, 4134),
    REGISTER_OOVPAS(IDirectSound_SetDopplerFactor, PATCH, 3936, 4134),
    REGISTER_OOVPAS(IDirectSound_SetEffectData, PATCH, 4134, 5344),
    REGISTER_OOVPAS(IDirectSound_SetI3DL2Listener, PATCH, 3936, 4134),
    REGISTER_OOVPAS(IDirectSound_SetMixBinHeadroom, PATCH, 3936, 4627, 5558),
    REGISTER_OOVPAS(IDirectSound_SetOrientation, PATCH, 3936),
    REGISTER_OOVPAS(IDirectSound_SetPosition, PATCH, 3936, 5558),
    REGISTER_OOVPAS(IDirectSound_SetRolloffFactor, PATCH, 3936, 4134/*, 5344*/),
    REGISTER_OOVPAS(IDirectSound_SetVelocity, PATCH, 3936, 5558),
    REGISTER_OOVPAS(IDirectSound_SynchPlayback, PATCH, 5233),


    REGISTER_OOVPAS(CFullHRTFSource_GetCenterVolume, XREF, 5558),
    REGISTER_OOVPAS(CHRTFSource_SetFullHRTF5Channel, XREF, 5558),


    REGISTER_OOVPAS(WaveFormat_CreateXboxAdpcmFormat, XREF, 5344),
    REGISTER_OOVPAS(XAudioDownloadEffectsImage, PATCH, 4627, 5558, 5788),
    REGISTER_OOVPAS(XAudioCreateAdpcmFormat, PATCH, 5344),
    REGISTER_OOVPAS(XFileCreateMediaObjectAsync, PATCH, 5788),
    REGISTER_OOVPAS(XFileCreateMediaObjectEx, PATCH, 4627, 5028),
    REGISTER_OOVPAS(XWaveFileCreateMediaObject, PATCH, 4627),


    REGISTER_OOVPAS(CMemoryManager_PoolAlloc, XREF, 5788), //TODO: Why is this in here?
};

// ******************************************************************
// * DSOUND_OOVPA_SIZE
// ******************************************************************
uint32 DSound_OOVPA_SIZEV2 = sizeof(DSound_OOVPAV2);

#endif
