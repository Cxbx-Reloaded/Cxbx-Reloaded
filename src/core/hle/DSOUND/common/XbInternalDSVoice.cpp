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
template<class T>
void GetFormat_4034_lower(T& settings, XTL::audio_format& format)
{
    format.audio_codec = settings.p_audio_format->wfx.wFormatTag = format.audio_codec;
    format.nChannels = settings.p_audio_format->wfx.nChannels;
    format.cbSize = settings.p_audio_format->wfx.cbSize;
    format.nSamplesPerSec = settings.p_audio_format->wfx.nSamplesPerSec;
    format.bitsPerSample = settings.p_audio_format->wfx.wBitsPerSample;
}
template<class T>
void GetFormat_4039_upper(T& settings, XTL::audio_format& format)
{
    format.audio_codec = settings.audio_codec;
    format.nChannels = settings.nChannels;
    format.cbSize = settings.cbSize;
    format.nSamplesPerSec = settings.nSamplesPerSec_default;
    format.bitsPerSample = settings.bitsPerSample;
}

// Interface for set format
template<class T>
void SetFormat_4034_lower(T& settings, XTL::audio_format format)
{
    settings.p_audio_format->wfx.wFormatTag = format.audio_codec;
    settings.p_audio_format->wfx.nChannels = static_cast<uint16_t>(format.nChannels);
    settings.p_audio_format->wfx.cbSize = static_cast<uint16_t>(format.cbSize);
    settings.p_audio_format->wfx.nSamplesPerSec = format.nSamplesPerSec;
    settings.p_audio_format->wfx.wBitsPerSample = static_cast<uint16_t>(format.bitsPerSample);
    if (format.audio_codec == WAVE_FORMAT_XBOX_ADPCM) {
        settings.p_audio_format->wSamplesPerBlock = 64;
    }
    settings.pitch = XTL::converter_freq2pitch(format.nSamplesPerSec);
}
template<class T>
void SetFormat_4039_only(T& settings, XTL::audio_format format)
{
    settings.audio_codec = format.audio_codec;
    settings.nChannels = format.nChannels;
    settings.cbSize = format.cbSize;
    settings.nSamplesPerSec_default = format.nSamplesPerSec;
    settings.bitsPerSample = format.bitsPerSample;
    settings.pitch = XTL::converter_freq2pitch(format.nSamplesPerSec);
}
template<class T>
void SetFormat_4134_upper(T& settings, XTL::audio_format format)
{
    settings.audio_codec = static_cast<uint16_t>(format.audio_codec);
    settings.nChannels = static_cast<uint8_t>(format.nChannels);
    settings.cbSize = static_cast<uint8_t>(format.cbSize);
    settings.nSamplesPerSec_default = format.nSamplesPerSec;
    settings.bitsPerSample = format.bitsPerSample;
    settings.pitch = XTL::converter_freq2pitch(format.nSamplesPerSec);
}

// Interface for get frequency
template<class T>
uint32_t GetFrequencyDefault_4034_lower(T& settings)
{
    return settings.p_audio_format->wfx.nSamplesPerSec;
}
template<class T>
uint32_t GetFrequencyDefault_4039_upper(T& settings)
{
    return settings.nSamplesPerSec_default;
}

// Interface for get pitch
template<class T>
int32_t GetPitch(T& settings)
{
    return settings.pitch;
}

// Interface for set pitch
template<class T>
void SetPitch(T& settings, int32_t pitch)
{
    settings.pitch = pitch;
}

// Interface for get volume
template<class T>
uint32_t GetVolume(T& settings)
{
    return settings.volume;
}

// Interface for set volume
template<class T>
void SetVolume(T& settings, uint32_t volume)
{
    settings.volume = volume - settings.headroom;
}

// Interface for get headroom
template<class T>
uint32_t GetHeadroom(T& settings)
{
    return settings.headroom;
}

// Interface for set headroom
template<class T>
void SetHeadroom(T& settings, uint32_t set_headroom)
{
    settings.volume = settings.volume - set_headroom - settings.headroom;
    settings.headroom = set_headroom;
}

void Init_Headroom(uint32_t& headroom, bool is3D)
{
    headroom = is3D? 0 : 600;
}
template<class T>
void Init(T& settings, bool is3D)
{
    Init_Headroom(settings.headroom, is3D);
    settings.volume = 0 - settings.headroom;
    settings.volume = 0 - settings.headroom;
}

XTL::CDirectSoundVoice::CDirectSoundVoice(bool is3D)
{
    u = { 0 };

    if (g_LibVersion_DSOUND < 4039) {
        u.settings_4034_lower.p_audio_format = new XBOXADPCMWAVEFORMAT;
        memset(&u.settings_4034_lower.p_audio_format->wfx, 0, sizeof(XBOXADPCMWAVEFORMAT));

        Init<_u::_settings_4034_lower>(u.settings_4034_lower, is3D);

        funcs.GetFormat = reinterpret_cast<pGetFormat>(::GetFormat_4034_lower<_u::_settings_4034_lower>);
        funcs.SetFormat = reinterpret_cast<pSetFormat>(::SetFormat_4034_lower<_u::_settings_4034_lower>);
        funcs.GetFrequencyDefault = reinterpret_cast<pGetUint32>(::GetFrequencyDefault_4034_lower<_u::_settings_4034_lower>);
        funcs.GetPitch = reinterpret_cast<pGetInt32>(::GetPitch<_u::_settings_4034_lower>);
        funcs.SetPitch = reinterpret_cast<pSetInt32>(::SetPitch<_u::_settings_4034_lower>);
        funcs.GetVolume = reinterpret_cast<pGetUint32>(::GetVolume<_u::_settings_4034_lower>);
        funcs.SetVolume = reinterpret_cast<pSetUint32>(::SetVolume<_u::_settings_4034_lower>);
        funcs.GetHeadroom = reinterpret_cast<pGetUint32>(::GetHeadroom<_u::_settings_4034_lower>);
        funcs.SetHeadroom = reinterpret_cast<pSetUint32>(::SetHeadroom<_u::_settings_4034_lower>);
    }
    else if (g_LibVersion_DSOUND == 4039) {

        Init<_u::_settings_4039_only>(u.settings_4039_only, is3D);

        funcs.GetFormat = reinterpret_cast<pGetFormat>(::GetFormat_4039_upper<_u::_settings_4039_only>);
        funcs.SetFormat = reinterpret_cast<pSetFormat>(::SetFormat_4039_only<_u::_settings_4039_only>);
        funcs.GetFrequencyDefault = reinterpret_cast<pGetUint32>(::GetFrequencyDefault_4039_upper<_u::_settings_4039_only>);
        funcs.GetPitch = reinterpret_cast<pGetInt32>(::GetPitch<_u::_settings_4039_only>);
        funcs.SetPitch = reinterpret_cast<pSetInt32>(::SetPitch<_u::_settings_4039_only>);
        funcs.GetVolume = reinterpret_cast<pGetUint32>(::GetVolume<_u::_settings_4039_only>);
        funcs.SetVolume = reinterpret_cast<pSetUint32>(::SetVolume<_u::_settings_4039_only>);
        funcs.GetHeadroom = reinterpret_cast<pGetUint32>(::GetHeadroom<_u::_settings_4039_only>);
        funcs.SetHeadroom = reinterpret_cast<pSetUint32>(::SetHeadroom<_u::_settings_4039_only>);
    }
    else {

        Init<_u::_settings_4134_upper>(u.settings_4134_upper, is3D);

        funcs.GetFormat = reinterpret_cast<pGetFormat>(::GetFormat_4039_upper<_u::_settings_4134_upper>);
        funcs.SetFormat = reinterpret_cast<pSetFormat>(::SetFormat_4134_upper<_u::_settings_4134_upper>);
        funcs.GetFrequencyDefault = reinterpret_cast<pGetUint32>(::GetFrequencyDefault_4039_upper<_u::_settings_4134_upper>);
        funcs.GetPitch = reinterpret_cast<pGetInt32>(::GetPitch<_u::_settings_4134_upper>);
        funcs.SetPitch = reinterpret_cast<pSetInt32>(::SetPitch<_u::_settings_4134_upper>);
        funcs.GetVolume = reinterpret_cast<pGetUint32>(::GetVolume<_u::_settings_4134_upper>);
        funcs.SetVolume = reinterpret_cast<pSetUint32>(::SetVolume<_u::_settings_4134_upper>);
        funcs.GetHeadroom = reinterpret_cast<pGetUint32>(::GetHeadroom<_u::_settings_4134_upper>);
        funcs.SetHeadroom = reinterpret_cast<pSetUint32>(::SetHeadroom<_u::_settings_4134_upper>);
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
