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

//TODO: Need further verification for 4361 titles, 4432 is last known lowest build match.
//Test case for 4361: Burnout (found a match yet is called twice for every time joystick is moved. Is this normal?)
// ******************************************************************
// * DirectSound::GetCaps
// ******************************************************************
OOVPA_XREF(CDirectSound_GetCaps, 4361, 7,

    XREF_CDirectSound_GetCaps,
    XRefZero)

        { 0x0C, 0x0F },
        { 0x0D, 0xB6 },
        { 0x0E, 0xC8 },
        { 0x46, 0x8B },
        { 0x4C, 0x8B },
        { 0x69, 0xC2 },
        { 0x6A, 0x08 },
OOVPA_END;

#if 0 // Used 3936
// ******************************************************************
// * IDirectSound_Release
// ******************************************************************
OOVPA_NO_XREF(IDirectSound_Release, 4432, 12)

        // IDirectSound_Release+0x04 : lea ecx, [eax-8]
        { 0x04, 0x8D },
        { 0x05, 0x48 },
        { 0x06, 0xF8 },

        // IDirectSound_Release+0x07 : neg eax
        { 0x07, 0xF7 },
        { 0x08, 0xD8 },

        // IDirectSound_Release+0x09 : sbb eax, eax
        { 0x09, 0x1B },
        { 0x0A, 0xC0 },

        // IDirectSound_Release+0x0D : mov ecx, [eax]
        { 0x0D, 0x8B },
        { 0x0E, 0x08 },

        // IDirectSound_Release+0x10 : call dword ptr [ecx+8]
        { 0x10, 0xFF },
        { 0x11, 0x51 },
        { 0x12, 0x08 },
OOVPA_END;
#endif
#if 0 // Used 4134
// ******************************************************************

// ******************************************************************
// * CDirectSoundVoiceSettings::SetMixBins
// ******************************************************************
OOVPA_XREF(CDirectSoundVoiceSettings_SetMixBins, 4432, 10,

    XREF_DSSETMIXBINSC,
    XRefZero)

        // CDirectSoundVoiceSettings_SetMixBins+0x06 : jnz +0x1A
        { 0x06, 0x75 },
        { 0x07, 0x1A },

        // CDirectSoundVoiceSettings_SetMixBins+0x08 : test byte ptr [ecx+8], 0x10
        { 0x08, 0xF6 },
        { 0x09, 0x41 },
        { 0x0A, 0x08 },
        { 0x0B, 0x10 },

        // CDirectSoundVoiceSettings_SetMixBins+0x2B : jbe +0x2B
        { 0x2B, 0x76 },
        { 0x2C, 0x2B },

        // CDirectSoundVoiceSettings_SetMixBins+0xB2 : retn 0x04
        { 0xB2, 0xC2 },
        { 0xB3, 0x04 },
OOVPA_END;
#endif

// ******************************************************************
// * DSound_4432
// ******************************************************************
OOVPATable DSound_4432[] = {

	REGISTER_OOVPA(DirectSoundCreate, 4134, PATCH),
	REGISTER_OOVPA(DirectSoundDoWork, 4134, PATCH),
	REGISTER_OOVPA(DirectSoundGetSampleTime, 4361, PATCH),
	REGISTER_OOVPA(IDirectSound_CreateSoundStream, 3936, PATCH),
	REGISTER_OOVPA(CDirectSound_CreateSoundStream, 4134, XREF),
	REGISTER_OOVPA(DirectSoundCreateStream, 4134, PATCH),
	REGISTER_OOVPA(CMcpxStream_Pause, 4361, XREF),
	REGISTER_OOVPA(CDirectSoundStream_Pause, 4361, PATCH),
	REGISTER_OOVPA(IDirectSound_AddRef, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound_Release, 3936, PATCH),
	REGISTER_OOVPA(CDirectSound_CreateSoundBuffer, 4134, XREF),
	REGISTER_OOVPA(IDirectSound_CreateSoundBuffer, 4134, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_Release, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_SetLoopRegion, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetLoopRegion, 3936, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_SetBufferData, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetBufferData, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetBufferData, 4134, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_SetCurrentPosition, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetCurrentPosition, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetCurrentPosition, 4134, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_Stop, 4361, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_Stop, 4361, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Stop, 4361, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetMaxDistance, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMaxDistance, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMaxDistance, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetMinDistance, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMinDistance, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMinDistance, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_SetI3DL2Listener, 4134, XREF),
	REGISTER_OOVPA(IDirectSound_SetI3DL2Listener, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoiceSettings_SetMixBins, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetMixBins, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMixBins, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMixBins, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_CommitDeferredSettings, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_SetDistanceFactor, 4134, XREF),
	REGISTER_OOVPA(IDirectSound_SetDistanceFactor, 4134, PATCH),
	REGISTER_OOVPA(IDirectSound_DownloadEffectsImage, 3936, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_AddRef, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_SetPlayRegion, 4361, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetPlayRegion, 4361, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_Unlock, 3936, PATCH),
	REGISTER_OOVPA(CDirectSound_GetCaps, 4361, XREF),
	REGISTER_OOVPA(IDirectSound_GetCaps, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetPitch, 4039, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetPitch, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetPitch, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetPitch, 4134, XREF),
};

// ******************************************************************
// * DSound_4432_SIZE
// ******************************************************************
uint32 DSound_4432_SIZE = sizeof(DSound_4432);
