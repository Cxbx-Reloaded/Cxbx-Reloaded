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
// *  59 Temple Place - Suite 330, Bostom, MA 02111-1307, USA.
// *
// *  (c) 2017-2020 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************
#pragma once

#include <Windows.h>
#include <mmreg.h>
#include "core/kernel/init/CxbxKrnl.h"
#include "core/hle/DSOUND/XbDSoundTypes.h"
#include "core/hle/DSOUND/XbDSoundFuncs.hpp"

#include "Logging.h"
#include "core/hle/DSOUND/DirectSound/DirectSoundLogging.hpp"
#include "core/hle/DSOUND/XbDSoundLogging.hpp"

// Only use as last resort instead of alternative duplicates (an error occur between keyboard and chair)
static void WFXformat_GeneratePCMFormat(
    uint16_t nChannels,
    uint32_t nSamplesPerSec,
    uint16_t wBitPerSample,
    PWAVEFORMATEXTENSIBLE Host_wfxFormat)
{
    Host_wfxFormat->Format.wFormatTag = WAVE_FORMAT_PCM;
    Host_wfxFormat->Format.nChannels = nChannels;
    Host_wfxFormat->Format.nSamplesPerSec = nSamplesPerSec;
    Host_wfxFormat->Format.wBitsPerSample = wBitPerSample;
    Host_wfxFormat->Format.nBlockAlign = wBitPerSample / 8 * nChannels;
    Host_wfxFormat->Format.nAvgBytesPerSec = nSamplesPerSec * Host_wfxFormat->Format.nBlockAlign;
}

static void WFXformat_GenerateXADPCMFormat(
    uint16_t nChannels,
    uint32_t nSamplesPerSec,
    PWAVEFORMATEXTENSIBLE Host_pwfxFormat)
{
    Host_pwfxFormat->Format.nChannels = nChannels;
    Host_pwfxFormat->Format.nSamplesPerSec = nSamplesPerSec;
    Host_pwfxFormat->Format.wBitsPerSample = 4; // Always 4 (even if they're mono or stereo)
    Host_pwfxFormat->Format.nBlockAlign = 36 * nChannels;
    // nAvgBytesPerSec's equation is confirmed.
    Host_pwfxFormat->Format.nAvgBytesPerSec = (nSamplesPerSec / 64/*Always 64 samples per block*/) * Host_pwfxFormat->Format.nBlockAlign;
}

// Convert XADPCM to PCM format helper function
static void XADPCM2PCMFormat(PWAVEFORMATEXTENSIBLE Host_pwfxFormat)
{

#if 0 //For testing purpose if XADPCM to PCM is not accurate.
    EmuLog(LOG_LEVEL::DEBUG,
        "EmuDSound: XADPCM WAVEFORMATEX\n"
        "{\n"
        "   wFormatTag              : 0x%.04hX\n"
        "   nChannels               : 0x%.02hd\n"
        "   nSamplesPerSec          : 0x%.08X\n"
        "   nAvgBytesPerSec         : 0x%.08X\n"
        "   nBlockAlign             : 0x%.02hd\n"
        "   wBitsPerSample          : 0x%.04hX\n"
        "   cbSize                  : 0x%.04hX\n"
        "}\n",
        Host_pwfxFormat->Format.wFormatTag,
        Host_pwfxFormat->Format.nChannels,
        Host_pwfxFormat->Format.nSamplesPerSec,
        Host_pwfxFormat->Format.nAvgBytesPerSec,
        Host_pwfxFormat->Format.nBlockAlign,
        Host_pwfxFormat->Format.wBitsPerSample,
        Host_pwfxFormat->Format.cbSize);
#endif

    //Xb_lpwfxFormat.wFormatTag;         /* format type */
    //Xb_lpwfxFormat.nChannels;          /* number of channels (i.e. mono, stereo...) */ NO CHANGE
    //Xb_lpwfxFormat.nSamplesPerSec;     /* sample rate */ NO CHANGE
    //Xb_lpwfxFormat.nAvgBytesPerSec;    /* for buffer estimation */
    //Xb_lpwfxFormat.nBlockAlign;        /* block size of data */
    //Xb_lpwfxFormat.wBitsPerSample;     /* number of bits per sample of mono data */
    //Xb_lpwfxFormat.cbSize;             /* the count in bytes of the size of extra information (after cbSize) */
    WFXformat_GeneratePCMFormat(Host_pwfxFormat->Format.nChannels,
        Host_pwfxFormat->Format.nSamplesPerSec,
        16,
        Host_pwfxFormat);
    //Enable this only if you have Xbox ADPCM Codec installed on your PC, or else it will fail every time.
    //This is just to verify format conversion is correct or not.
#if 0
    if (waveOutOpen(nullptr, WAVE_MAPPER, Xb_lpwfxFormat, NULL, NULL, WAVE_FORMAT_QUERY) != MMSYSERR_NOERROR) {
        return DSERR_BADFORMAT;
    }
#endif

#if 0 //For testing purpose if XADPCM to PCM is not accurate.
    EmuLog(LOG_LEVEL::DEBUG,
        "EmuDSound: Converted to PCM WAVEFORMATEX\n"
        "{\n"
        "   wFormatTag              : 0x%.04hX\n"
        "   nChannels               : 0x%.02hd\n"
        "   nSamplesPerSec          : 0x%.08X\n"
        "   nAvgBytesPerSec         : 0x%.08X\n"
        "   nBlockAlign             : 0x%.02hd\n"
        "   wBitsPerSample          : 0x%.04hX\n"
        "   cbSize                  : 0x%.04hX\n"
        "}\n",
        Host_pwfxFormat->Format.wFormatTag,
        Host_pwfxFormat->Format.nChannels,
        Host_pwfxFormat->Format.nSamplesPerSec,
        Host_pwfxFormat->Format.nAvgBytesPerSec,
        Host_pwfxFormat->Format.nBlockAlign,
        Host_pwfxFormat->Format.wBitsPerSample,
        Host_pwfxFormat->Format.cbSize);
#endif
}

static void WFXformat_SyncPCMFormat(
    PWAVEFORMATEXTENSIBLE   Host_pwfxFormat)
{
    PWAVEFORMATEX ex_pwfxFormat = &Host_pwfxFormat->Format;
    // NOTE: This process check for 2+ channels whenever XADPCM or PCM format from xbox titles input in nChannels.
    // Since the host do not support 2+ channels on PCM only format. We must convert/update allocated wfxFormat to EXTENSIBLE
    // format which had been allocated enough to update.
    if (ex_pwfxFormat->nChannels > 2) {
        Host_pwfxFormat->Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
        Host_pwfxFormat->dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
        Host_pwfxFormat->Samples.wValidBitsPerSample = ex_pwfxFormat->wBitsPerSample;
        Host_pwfxFormat->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
        Host_pwfxFormat->Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);

        if (ex_pwfxFormat->nChannels >= 4) {
            Host_pwfxFormat->dwChannelMask |= SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT;
        }
        if (ex_pwfxFormat->nChannels == 6) {
            Host_pwfxFormat->dwChannelMask |= SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY;
        }
    }
    else {
        Host_pwfxFormat->Format.wFormatTag = WAVE_FORMAT_PCM;
        // Test case: Hulk crash due to cbSize is not a valid size.
        // If Xbox applications didn't enfore cbSize, we'll have to enforce from host side.
        // Also, we're enforcing it anyway.
        Host_pwfxFormat->Format.cbSize = 0;
    }
}

static void WFXformat_SyncXADPCMFormat(
    PWAVEFORMATEXTENSIBLE   Host_wfxFormat)
{
    XADPCM2PCMFormat(Host_wfxFormat);
    WFXformat_SyncPCMFormat(Host_wfxFormat);
}

typedef enum _CODEC_FORMAT {
    CF_PCM=0,
    CF_XADPCM,
    CF_UNKNOWN
} CODEC_FORMAT;

static void WFXformat_SanityFix(
    PWAVEFORMATEXTENSIBLE   Host_pwfxFormat,
    CODEC_FORMAT            codec_format,
    uint32_t                Xb_buffer_request_size,
    uint32_t                Xb_flags)
{
    WAVEFORMATEXTENSIBLE before_wfxFormat = *Host_pwfxFormat;
    bool isNotSanity = false;
    uint32_t nSamplesPerSec = Host_pwfxFormat->Format.nSamplesPerSec;

    // Generic enforcement
    // If Xbox applications supply invalid total channels, enforce to use either mono or stereo channel
    if (Host_pwfxFormat->Format.nChannels == 0 || Host_pwfxFormat->Format.nChannels > 6) {
        Host_pwfxFormat->Format.nChannels = XTL::DSOUND::PresetChannelDefault(Xb_flags);
        isNotSanity = true;
    }
    // If nSamplesPerSec is zero'd then use 44.1kHz by default
    if (nSamplesPerSec == 0) {
        Host_pwfxFormat->Format.nSamplesPerSec = 44100;
        isNotSanity = true;
    }

    if (Host_pwfxFormat->Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
        if (Host_pwfxFormat->Format.cbSize != sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX)) {
            Host_pwfxFormat->Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
            //isNotSanity = true;
        }
    }
    else {
        // Test case: Hulk crash due to cbSize is not a valid size.
        // If Xbox applications didn't enfore cbSize, we'll have to enforce from host side.
        /* NOTE: cbSize is already handled in WFXformat_SyncPCMFormat function as enforcement.
        if (Host_pwfxFormat->Format.cbSize != 0) {
            Host_pwfxFormat->Format.cbSize = 0;
            //isNotSanity = true;
        }
        */
    }

    // Generic handler
    if (codec_format == CF_PCM || codec_format == CF_UNKNOWN) {
        // If wBitsPerSample is zero'd then use 16 by default
        if (Host_pwfxFormat->Format.wBitsPerSample == 0) {
            isNotSanity = true;
        }

        // testcase: Phantasy Star Online Episode I & II made an attempt to use avg byte/second below sample/second requirement.
        // Defender is another title made an attempt to use avg byte/second way above the sample/second requirement.
        // In other word, this is a workaround to fix titles' mistake...
        uint32_t checkAvgBps = Host_pwfxFormat->Format.nSamplesPerSec * Host_pwfxFormat->Format.nBlockAlign;

        if (isNotSanity) {
            WFXformat_GeneratePCMFormat(Host_pwfxFormat->Format.nChannels, Host_pwfxFormat->Format.nSamplesPerSec, 16, Host_pwfxFormat);
        }
        else if (Host_pwfxFormat->Format.nAvgBytesPerSec != checkAvgBps) {
            Host_pwfxFormat->Format.nAvgBytesPerSec = checkAvgBps;
            isNotSanity = true;
        }
    }
    // CF_XADPCM
    else if (codec_format == CF_XADPCM) {
        // If wBitsPerSample is not equal to 4, then wfx format is not sane.
        if (Host_pwfxFormat->Format.wBitsPerSample != 4) {
            //Host_pwfxFormat->Format.wBitsPerSample = 4;
            isNotSanity = true;
        }

        if (isNotSanity) {
            WFXformat_GenerateXADPCMFormat(Host_pwfxFormat->Format.nChannels, Host_pwfxFormat->Format.nSamplesPerSec, Host_pwfxFormat);
        }
    }

    if (isNotSanity) {
        std::stringstream before, after;
        before << before_wfxFormat;
        after << Host_pwfxFormat;
        EmuLog(LOG_LEVEL::WARNING, "One or more values were not set properly before: %s\nvs after: %s", before.str().c_str(), after.str().c_str());
    }
}

static CODEC_FORMAT WFXformat_SyncHostFormat(
    void*                   Host_wfx_ptr,
    const void*             Xb_wfx_ptr,
    uint32_t                Xb_buffer_request_size,
    uint32_t                Xb_flags,
    XTL::CDirectSoundVoice* Xb_Voice)
{
    PWAVEFORMATEXTENSIBLE Xb_wfxFormat = (PWAVEFORMATEXTENSIBLE)Xb_wfx_ptr;
    PWAVEFORMATEXTENSIBLE Host_wfxFormat = (PWAVEFORMATEXTENSIBLE)Host_wfx_ptr;
    CODEC_FORMAT codec_format_ret = CF_PCM;
    bool require_validate = true;
    XTL::audio_format xb_format;

    // If no format is provided, then use default.
    if (Xb_wfx_ptr == xbnullptr) {
        WFXformat_GeneratePCMFormat(XTL::DSOUND::PresetChannelDefault(Xb_flags), 44100, 16, Host_wfxFormat);
        require_validate = false;
    }
    // HACK: This is a special sound buffer, create dummy WAVEFORMATEX data.
    // It's supposed to recieve data rather than generate it.  Buffers created
    // with flags DSBCAPS_MIXIN, DSBCAPS_FXIN, and DSBCAPS_FXIN2 will have no
    // WAVEFORMATEX structure by default.
    else if ((Xb_flags & (XTL_DSBCAPS_MIXIN | XTL_DSBCAPS_FXIN | XTL_DSBCAPS_FXIN2)) > 0) {
        EmuLog(LOG_LEVEL::WARNING, "Creating dummy WAVEFORMATEX (pdsbd->Xb_lpwfxFormat = xbnullptr)...");
        WFXformat_GeneratePCMFormat(XTL::DSOUND::PresetChannelDefault(Xb_flags), 44100, 16, Host_wfxFormat);
        require_validate = false;
    }
    // Otherwise, let's process given format.
    else {
        switch (Xb_wfxFormat->Format.wFormatTag) {
            case WAVE_FORMAT_PCM:
                // No further steps require here.
                break;
            case WAVE_FORMAT_XBOX_ADPCM:
                codec_format_ret = CF_XADPCM;
                break;
            case WAVE_FORMAT_EXTENSIBLE:
                if (Xb_wfxFormat->SubFormat.Data1 = WAVE_FORMAT_PCM) {
                    // No further steps require here.
                }
                else if (Xb_wfxFormat->SubFormat.Data1 == WAVE_FORMAT_XBOX_ADPCM) {
                    codec_format_ret = CF_XADPCM;
                }
                else {
                    codec_format_ret = CF_UNKNOWN;
                    require_validate = false;
                    std::stringstream oss;
                    oss << Xb_wfxFormat->SubFormat;
                    std::string msg = "Please report unknown extensible format : " + oss.str();
                    LOG_TEST_CASE(msg.c_str());
                }
                break;
            // Both 0 and default will use static structure until given a valid one.
            case 0:
                // NOTE: This is currently a hack for ability to create buffer class with DSBCAPS_LOCDEFER flag.
                WFXformat_GeneratePCMFormat(XTL::DSOUND::PresetChannelDefault(Xb_flags), 44100, 16, Host_wfxFormat);
                require_validate = false;
                LOG_TEST_CASE("WAVE_FORMAT_(0) found");
                break;
            default:
                codec_format_ret = CF_UNKNOWN;
                require_validate = false;
                EmuLog(LOG_LEVEL::WARNING, "Unknown Xbox format: %08X", Xb_wfxFormat->Format.wFormatTag);
                LOG_TEST_CASE("Unknown Xbox format, check your log output");
                break;
        }

        if (require_validate) {
            // First, perform sanity fix
            WFXformat_SanityFix(Host_wfxFormat, codec_format_ret, Xb_buffer_request_size, Xb_flags);
        }

        // Then re-convert to Host's PCM format.
        if (codec_format_ret == CF_PCM) {
            WFXformat_SyncPCMFormat(Host_wfxFormat);
        }
        else if (codec_format_ret == CF_XADPCM) {
            WFXformat_SyncXADPCMFormat(Host_wfxFormat);
        }
        // Any unknown formats will be using default PCM format.
        else {
            WFXformat_GeneratePCMFormat(XTL::DSOUND::PresetChannelDefault(Xb_flags), 44100, 16, Host_wfxFormat);
        }
    }
    // Forward xbox format to internal XTL::CDirectSoundVoice class.
    xb_format.audio_codec = (codec_format_ret == CF_XADPCM ? WAVE_FORMAT_XBOX_ADPCM : WAVE_FORMAT_PCM);
    xb_format.nChannels = Host_wfxFormat->Format.nChannels;
    xb_format.cbSize = (codec_format_ret == CF_XADPCM ? 4 : 0);
    xb_format.nSamplesPerSec = Host_wfxFormat->Format.nSamplesPerSec;
    xb_format.bitsPerSample = (codec_format_ret == CF_XADPCM ? 4 : Host_wfxFormat->Format.wBitsPerSample);
    Xb_Voice->SetFormat(xb_format);

    return codec_format_ret;
}
