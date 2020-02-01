// ******************************************************************
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
// *  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// *
// *  (c) 2019 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************

#include "Logging.h"
#include "DirectSoundLogging.hpp"
#include <mmreg.h>
#include "..\XbDSoundTypes.h"

FLAGS2STR_START(DS_BCAPS)
	FLAG2STR(DSBCAPS_PRIMARYBUFFER)
	FLAG2STR(DSBCAPS_STATIC)
	FLAG2STR(DSBCAPS_LOCHARDWARE)
	FLAG2STR(DSBCAPS_LOCSOFTWARE)
	FLAG2STR(DSBCAPS_CTRL3D)
	FLAG2STR(DSBCAPS_CTRLFREQUENCY)
	FLAG2STR(DSBCAPS_CTRLPAN)
	FLAG2STR(DSBCAPS_CTRLVOLUME)
	FLAG2STR(DSBCAPS_CTRLPOSITIONNOTIFY)
	FLAG2STR(DSBCAPS_CTRLFX)
	FLAG2STR(DSBCAPS_STICKYFOCUS)
	FLAG2STR(DSBCAPS_GLOBALFOCUS)
	FLAG2STR(DSBCAPS_GETCURRENTPOSITION2)
	FLAG2STR(DSBCAPS_MUTE3DATMAXDISTANCE)
	FLAG2STR(DSBCAPS_LOCDEFER)
	FLAG2STR(DSBCAPS_TRUEPLAYPOSITION)
FLAGS2STR_END_and_LOGRENDER(DS_BCAPS)

ENUM2STR_START(DS_RESULT)
	ENUM2STR_CASE(DS_OK)
	ENUM2STR_CASE(DS_NO_VIRTUALIZATION)
	ENUM2STR_CASE(DSERR_ALLOCATED)
	ENUM2STR_CASE(DSERR_CONTROLUNAVAIL)
	ENUM2STR_CASE(DSERR_INVALIDPARAM)
	ENUM2STR_CASE(DSERR_INVALIDCALL)
	ENUM2STR_CASE(DSERR_GENERIC)
	ENUM2STR_CASE(DSERR_PRIOLEVELNEEDED)
	ENUM2STR_CASE(DSERR_OUTOFMEMORY)
	ENUM2STR_CASE(DSERR_BADFORMAT)
	ENUM2STR_CASE(DSERR_UNSUPPORTED)
	ENUM2STR_CASE(DSERR_NODRIVER)
	ENUM2STR_CASE(DSERR_ALREADYINITIALIZED)
	ENUM2STR_CASE(DSERR_NOAGGREGATION)
	ENUM2STR_CASE(DSERR_BUFFERLOST)
	ENUM2STR_CASE(DSERR_OTHERAPPHASPRIO)
	ENUM2STR_CASE(DSERR_UNINITIALIZED)
	ENUM2STR_CASE(DSERR_NOINTERFACE)
	ENUM2STR_CASE(DSERR_ACCESSDENIED)
	ENUM2STR_CASE(DSERR_BUFFERTOOSMALL)
	ENUM2STR_CASE(DSERR_DS8_REQUIRED)
	ENUM2STR_CASE(DSERR_SENDLOOP)
	ENUM2STR_CASE(DSERR_BADSENDBUFFERGUID)
	ENUM2STR_CASE(DSERR_OBJECTNOTFOUND)
	ENUM2STR_CASE(DSERR_FXUNAVAILABLE)
ENUM2STR_END_and_LOGRENDER(DS_RESULT)

ENUM2STR_START(WAVEFORMAT_TAG)
	ENUM2STR_CASE(0)
	ENUM2STR_CASE(WAVE_FORMAT_PCM)
	ENUM2STR_CASE(WAVE_FORMAT_ADPCM)
	ENUM2STR_CASE(WAVE_FORMAT_XBOX_ADPCM)
	ENUM2STR_CASE(WAVE_FORMAT_EXTENSIBLE)
ENUM2STR_END_and_LOGRENDER(WAVEFORMAT_TAG)

#define WFC_MONO 1
#define WFC_STEREO 2
#define WFC_2POINT1 3
#define WFC_QUAD 4
#define WFC_5POINT0 5
#define WFC_5POINT1 6

ENUM2STR_START(WAVEFORMAT_CHANNEL)
	ENUM2STR_CASE(WFC_MONO)
	ENUM2STR_CASE(WFC_STEREO)
	ENUM2STR_CASE(WFC_2POINT1)
	ENUM2STR_CASE(WFC_QUAD)
	ENUM2STR_CASE(WFC_5POINT0)
	ENUM2STR_CASE(WFC_5POINT1)
ENUM2STR_END_and_LOGRENDER(WAVEFORMAT_CHANNEL)

LOGRENDER(WAVEFORMATEX)
{
	return os
		LOGRENDER_MEMBER_TYPE(WAVEFORMAT_TAG, wFormatTag)
		LOGRENDER_MEMBER_TYPE(WAVEFORMAT_CHANNEL, nChannels)
		LOGRENDER_MEMBER(nSamplesPerSec)
		LOGRENDER_MEMBER(nAvgBytesPerSec)
		LOGRENDER_MEMBER(nBlockAlign)
		LOGRENDER_MEMBER(wBitsPerSample)
		LOGRENDER_MEMBER(cbSize)
		;
}

LOGRENDER(GUID)
{
	return os
		LOGRENDER_MEMBER(Data1)
		LOGRENDER_MEMBER(Data2)
		LOGRENDER_MEMBER(Data3)
		LOGRENDER_MEMBER(Data4)
		;
}

LOGRENDER(WAVEFORMATEXTENSIBLE)
{
	if (value.Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
		return os
			LOGRENDER_MEMBER_TYPE(WAVEFORMATEX, Format)
			LOGRENDER_MEMBER(Samples.wValidBitsPerSample)
			LOGRENDER_MEMBER(dwChannelMask)
			LOGRENDER_MEMBER_TYPE(GUID, SubFormat)
			;
	}
	else {
		return os
			LOGRENDER_MEMBER_TYPE(WAVEFORMATEX, Format)
			;
	}
}

LOGRENDER(DSBUFFERDESC)
{
	return os
		LOGRENDER_MEMBER(dwSize)
		LOGRENDER_MEMBER_TYPE(DS_BCAPS, dwFlags)
		LOGRENDER_MEMBER(dwBufferBytes)
		LOGRENDER_MEMBER(dwReserved)
		LOGRENDER_MEMBER_TYPE(LPWAVEFORMATEX, lpwfxFormat)
		;
}

LOGRENDER(D3DVECTOR)
{
	return os
		LOGRENDER_MEMBER(x)
		LOGRENDER_MEMBER(y)
		LOGRENDER_MEMBER(z)
		;
}

std::string DirectSoundErrorString(HRESULT hResult)
{
	switch (hResult) {
		case DS_OK:
			return "";
		case DS_NO_VIRTUALIZATION:
			return "The call succeeded, but we had to substitute the 3D algorithm";
		case DSERR_ALLOCATED:
			return "The call failed because resources(such as a priority level) were already being used by another caller";
		case DSERR_CONTROLUNAVAIL:
			return "The control (vol, pan, etc.) requested by the caller is not available";
		case DSERR_INVALIDPARAM:
			return "An invalid parameter was passed to the returning function";
		case DSERR_INVALIDCALL:
			return "This call is not valid for the current state of this object";
		case DSERR_GENERIC:
			return "An undetermined error occurred inside the DirectSound subsystem";
		case DSERR_PRIOLEVELNEEDED:
			return "The caller does not have the priority level required for the function to succeed";
		case DSERR_OUTOFMEMORY:
			return "Not enough free memory is available to complete the operation";
		case DSERR_BADFORMAT:
			return "The specified WAVE format is not supported";
		case DSERR_UNSUPPORTED:
			return "The function called is not supported at this time";
		case DSERR_NODRIVER:
			return "No sound driver is available for use";
		case DSERR_ALREADYINITIALIZED:
			return "This object is already initialized";
		case DSERR_NOAGGREGATION:
			return "This object does not support aggregation";
		case DSERR_BUFFERLOST:
			return "The buffer memory has been lost, and must be restored";
		case DSERR_OTHERAPPHASPRIO:
			return "Another app has a higher priority level, preventing this call from succeeding";
		case DSERR_UNINITIALIZED:
			return "This object has not been initialized";
		case DSERR_NOINTERFACE:
			return "The requested COM interface is not available";
		case DSERR_ACCESSDENIED:
			return "Access is denied";
		case DSERR_BUFFERTOOSMALL:
			return "Tried to create a DSBCAPS_CTRLFX buffer shorter than DSBSIZE_FX_MIN milliseconds";
		case DSERR_DS8_REQUIRED:
			return "Attempt to use DirectSound 8 functionality on an older DirectSound object";
		case DSERR_SENDLOOP:
			return "A circular loop of send effects was detected";
		case DSERR_BADSENDBUFFERGUID:
			return "The GUID specified in an audiopath file does not match a valid MIXIN buffer";
		case DSERR_OBJECTNOTFOUND:
			return "The object requested was not found (numerically equal to DMUS_E_NOT_FOUND)";
		case DSERR_FXUNAVAILABLE:
			return "The effects requested could not be found on the system, or they were found"
			       "but in the wrong order, or in the wrong hardware/software locations.";
		default:
			return "Unknown error";
	}
}
