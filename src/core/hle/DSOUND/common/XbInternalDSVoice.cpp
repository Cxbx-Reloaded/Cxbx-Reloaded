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
// *  (c) 2020 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************

#include "common\Settings.hpp" // for g_LibVersion_DSOUND
#include "XbInternalStruct.hpp"

// Interface for get format
void GetFormat_4034_lower(XTL::CDirectSoundVoice::_u& u, XTL::audio_format& format)
{
    format.audio_codec = u.settings_4034_lower.p_audio_format->wfx.wFormatTag = format.audio_codec;
    format.nChannels = u.settings_4034_lower.p_audio_format->wfx.nChannels;
    format.cbSize = u.settings_4034_lower.p_audio_format->wfx.cbSize;
    format.nSamplesPerSec = u.settings_4034_lower.p_audio_format->wfx.nSamplesPerSec;
    format.bitsPerSample = u.settings_4034_lower.p_audio_format->wfx.wBitsPerSample;
}
void GetFormat_4039_only(XTL::CDirectSoundVoice::_u& u, XTL::audio_format& format)
{
    format.audio_codec = u.settings_4039_only.audio_codec;
    format.nChannels = u.settings_4039_only.nChannels;
    format.cbSize = u.settings_4039_only.cbSize;
    format.nSamplesPerSec = u.settings_4039_only.nSamplesPerSec_default;
    format.bitsPerSample = u.settings_4039_only.bitsPerSample;
}
void GetFormat_4134_upper(XTL::CDirectSoundVoice::_u& u, XTL::audio_format& format)
{
    format.audio_codec = u.settings_4134_upper.audio_codec;
    format.nChannels = u.settings_4134_upper.nChannels;
    format.cbSize = u.settings_4134_upper.cbSize;
    format.nSamplesPerSec = u.settings_4134_upper.nSamplesPerSec_default;
    format.bitsPerSample = u.settings_4134_upper.bitsPerSample;
}

// Interface for set format
void SetFormat_4034_lower(XTL::CDirectSoundVoice::_u& u, XTL::audio_format format)
{
    u.settings_4034_lower.p_audio_format->wfx.wFormatTag = format.audio_codec;
    u.settings_4034_lower.p_audio_format->wfx.nChannels = static_cast<uint16_t>(format.nChannels);
    u.settings_4034_lower.p_audio_format->wfx.cbSize = static_cast<uint16_t>(format.cbSize);
    u.settings_4034_lower.p_audio_format->wfx.nSamplesPerSec = format.nSamplesPerSec;
    u.settings_4034_lower.p_audio_format->wfx.wBitsPerSample = static_cast<uint16_t>(format.bitsPerSample);
    if (format.audio_codec == WAVE_FORMAT_XBOX_ADPCM) {
        u.settings_4034_lower.p_audio_format->wSamplesPerBlock = 64;
    }
    u.settings_4039_only.pitch = XTL::converter_freq2pitch(format.nSamplesPerSec);
}
void SetFormat_4039_only(XTL::CDirectSoundVoice::_u& u, XTL::audio_format format)
{
    u.settings_4039_only.audio_codec = format.audio_codec;
    u.settings_4039_only.nChannels = format.nChannels;
    u.settings_4039_only.cbSize = format.cbSize;
    u.settings_4039_only.nSamplesPerSec_default = format.nSamplesPerSec;
    u.settings_4039_only.bitsPerSample = format.bitsPerSample;
    u.settings_4039_only.pitch = XTL::converter_freq2pitch(format.nSamplesPerSec);
}
void SetFormat_4134_upper(XTL::CDirectSoundVoice::_u& u, XTL::audio_format format)
{
    u.settings_4134_upper.audio_codec = static_cast<uint16_t>(format.audio_codec);
    u.settings_4134_upper.nChannels = static_cast<uint8_t>(format.nChannels);
    u.settings_4134_upper.cbSize = static_cast<uint8_t>(format.cbSize);
    u.settings_4134_upper.nSamplesPerSec_default = format.nSamplesPerSec;
    u.settings_4134_upper.bitsPerSample = format.bitsPerSample;
    u.settings_4134_upper.pitch = XTL::converter_freq2pitch(format.nSamplesPerSec);
}

// Interface for get frequency
uint32_t GetFrequencyDefault_4034_lower(XTL::CDirectSoundVoice::_u& u)
{
    return u.settings_4034_lower.p_audio_format->wfx.nSamplesPerSec;
}
uint32_t GetFrequencyDefault_4039_only(XTL::CDirectSoundVoice::_u& u)
{
    return u.settings_4039_only.nSamplesPerSec_default;
}
uint32_t GetFrequencyDefault_4134_upper(XTL::CDirectSoundVoice::_u& u)
{
    return u.settings_4134_upper.nSamplesPerSec_default;
}

// Interface for get pitch
int32_t GetPitch_4034_lower(XTL::CDirectSoundVoice::_u& u)
{
    return u.settings_4034_lower.pitch;
}
int32_t GetPitch_4039_only(XTL::CDirectSoundVoice::_u& u)
{
    return u.settings_4039_only.pitch;
}
int32_t GetPitch_4134_upper(XTL::CDirectSoundVoice::_u& u)
{
    return u.settings_4134_upper.pitch;
}

// Interface for set pitch
void SetPitch_4034_lower(XTL::CDirectSoundVoice::_u& u, int32_t pitch)
{
    u.settings_4034_lower.pitch = pitch;
}
void SetPitch_4039_only(XTL::CDirectSoundVoice::_u& u, int32_t pitch)
{
    u.settings_4039_only.pitch = pitch;
}
void SetPitch_4134_upper(XTL::CDirectSoundVoice::_u& u, int32_t pitch)
{
    u.settings_4134_upper.pitch = pitch;
}

// Interface for get volume
uint32_t GetVolume_4034_lower(XTL::CDirectSoundVoice::_u& u)
{
    return u.settings_4034_lower.volume;
}
uint32_t GetVolume_4039_only(XTL::CDirectSoundVoice::_u& u)
{
    return u.settings_4039_only.volume;
}
uint32_t GetVolume_4134_upper(XTL::CDirectSoundVoice::_u& u)
{
    return u.settings_4134_upper.volume;
}

void SetVolume_calc(uint32_t& cur_volume, uint32_t headroom, uint32_t set_volume)
{
    cur_volume = set_volume - headroom;
}
// Interface for set volume
void SetVolume_4034_lower(XTL::CDirectSoundVoice::_u& u, uint32_t volume)
{
    SetVolume_calc(u.settings_4034_lower.volume, u.settings_4034_lower.headroom, volume);
}
void SetVolume_4039_only(XTL::CDirectSoundVoice::_u& u, uint32_t volume)
{
    SetVolume_calc(u.settings_4039_only.volume, u.settings_4039_only.headroom, volume);
}
void SetVolume_4134_upper(XTL::CDirectSoundVoice::_u& u, uint32_t volume)
{
    SetVolume_calc(u.settings_4134_upper.volume, u.settings_4134_upper.headroom, volume);
}

// Interface for get headroom
uint32_t GetHeadroom_4034_lower(XTL::CDirectSoundVoice::_u& u)
{
    return u.settings_4034_lower.headroom;
}
uint32_t GetHeadroom_4039_only(XTL::CDirectSoundVoice::_u& u)
{
    return u.settings_4039_only.headroom;
}
uint32_t GetHeadroom_4134_upper(XTL::CDirectSoundVoice::_u& u)
{
    return u.settings_4134_upper.headroom;
}

void SetHeadroom_calc(uint32_t& cur_headroom, uint32_t& volume, uint32_t set_headroom)
{
    volume = volume - set_headroom - cur_headroom;
    cur_headroom = set_headroom;
}
// Interface for set headroom
void SetHeadroom_4034_lower(XTL::CDirectSoundVoice::_u& u, uint32_t headroom)
{
    SetHeadroom_calc(u.settings_4034_lower.headroom, u.settings_4034_lower.volume, headroom);
}
void SetHeadroom_4039_only(XTL::CDirectSoundVoice::_u& u, uint32_t headroom)
{
    SetHeadroom_calc(u.settings_4039_only.headroom, u.settings_4039_only.volume, headroom);
}
void SetHeadroom_4134_upper(XTL::CDirectSoundVoice::_u& u, uint32_t headroom)
{
    SetHeadroom_calc(u.settings_4134_upper.headroom, u.settings_4134_upper.volume, headroom);
}

void Init_Headroom(uint32_t& headroom, bool is3D)
{
    headroom = is3D? 0 : 600;
}
void Init_4034_lower(XTL::CDirectSoundVoice::_u::_settings_4034_lower& settings, bool is3D)
{
    Init_Headroom(settings.headroom, is3D);
    settings.volume = 0 - settings.headroom;
}
void Init_4039_only(XTL::CDirectSoundVoice::_u::_settings_4039_only& settings, bool is3D)
{
    Init_Headroom(settings.headroom, is3D);
    settings.volume = 0 - settings.headroom;
}
void Init_4134_upper(XTL::CDirectSoundVoice::_u::_settings_4134_upper& settings, bool is3D)
{
    Init_Headroom(settings.headroom, is3D);
    settings.volume = 0 - settings.headroom;
}

XTL::CDirectSoundVoice::CDirectSoundVoice(bool is3D)
{
    memset(u.settings_interface.unknown_08, 0, sizeof(u.settings_interface));

    if (g_LibVersion_DSOUND < 4039) {
        u.settings_4034_lower.p_audio_format = new XBOXADPCMWAVEFORMAT;
        memset(&u.settings_4034_lower.p_audio_format->wfx, 0, sizeof(XBOXADPCMWAVEFORMAT));

        Init_4034_lower(u.settings_4034_lower, is3D);

        funcs.GetFormat = GetFormat_4034_lower;
        funcs.SetFormat = SetFormat_4034_lower;
        funcs.GetFrequencyDefault = GetFrequencyDefault_4034_lower;
        funcs.GetPitch = GetPitch_4034_lower;
        funcs.SetPitch = SetPitch_4034_lower;
        funcs.GetVolume = GetVolume_4034_lower;
        funcs.SetVolume = SetVolume_4034_lower;
        funcs.GetHeadroom = GetHeadroom_4034_lower;
        funcs.SetHeadroom = SetHeadroom_4034_lower;
    }
    else if (g_LibVersion_DSOUND == 4039) {

        Init_4039_only(u.settings_4039_only, is3D);

        funcs.GetFormat = GetFormat_4039_only;
        funcs.SetFormat = SetFormat_4039_only;
        funcs.GetFrequencyDefault = GetFrequencyDefault_4039_only;
        funcs.GetPitch = GetPitch_4039_only;
        funcs.SetPitch = SetPitch_4039_only;
        funcs.GetVolume = GetVolume_4039_only;
        funcs.SetVolume = SetVolume_4039_only;
        funcs.GetHeadroom = GetHeadroom_4039_only;
        funcs.SetHeadroom = SetHeadroom_4039_only;
    }
    else {

        Init_4134_upper(u.settings_4134_upper, is3D);

        funcs.GetFormat = GetFormat_4134_upper;
        funcs.SetFormat = SetFormat_4134_upper;
        funcs.GetFrequencyDefault = GetFrequencyDefault_4134_upper;
        funcs.GetPitch = GetPitch_4134_upper;
        funcs.SetPitch = SetPitch_4134_upper;
        funcs.GetVolume = GetVolume_4134_upper;
        funcs.SetVolume = SetVolume_4134_upper;
        funcs.GetHeadroom = GetHeadroom_4134_upper;
        funcs.SetHeadroom = SetHeadroom_4134_upper;
    }
}

XTL::CDirectSoundVoice::~CDirectSoundVoice()
{
    if (g_LibVersion_DSOUND < 4039) {
        if (!u.settings_4034_lower.p_audio_format) {
            delete u.settings_4034_lower.p_audio_format;
            u.settings_4034_lower.p_audio_format = nullptr;
        }
    }
}
