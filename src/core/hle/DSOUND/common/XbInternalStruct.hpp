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
#pragma once

#include "Cxbx.h"
#include "core\hle\DSOUND\XbDSoundTypes.h"
#include "common/audio/converter.hpp"

namespace xbox {

// TODO: Everything, only small portions had been implemented.

// Note to all maintainers, make sure structure's class-like are not misalign.
// There is always a VMT (Virtual Method Table) pointer at offset 0x00 (uint32_t size).
// Afterward are depending on what internal data are placed. So, whenever implementing a functions.
// Whenever implementing functional codes, make sure they are at the bottom of the xbox's structure. Thanks!

struct audio_format {
    uint32_t                audio_codec;
    uint32_t                nChannels;
    uint32_t                cbSize;
    int32_t                 nSamplesPerSec;
    uint32_t                bitsPerSample;
};

struct CUnknownGenericManager {
    // construct vtable (or grab ptr to existing)
    CUnknownGenericManager() : ref_count(1) {}

    // all VMT (Virtual Method Table) are stored in local offset 0x00's pointer
    // TODO: Implement to use derived's override AddRef/Release. Also, each class has its own ref counter and is not shareable.
    virtual ~CUnknownGenericManager() {};                           // 0x00
    virtual uint32_t WINAPI AddRef() { return ref_count++; };       // 0x04
    virtual uint32_t WINAPI Release() { return ref_count--; };      // 0x08

    uint32_t ref_count;                                     // 0x04
};
// Require to verify there is no other unknown additional data by compiler itself.
static_assert(sizeof(CUnknownGenericManager) == 0x08);

struct CUnknownTemplate {
    // construct vtable (or grab ptr to existing)
    CUnknownTemplate();

    virtual ~CUnknownTemplate() {};                                             // 0x00
    virtual void WINAPI pUnknown_04() { throw std::exception("pUnknown_04"); }; // 0x04
    virtual void WINAPI pUnknown_08() { throw std::exception("pUnknown_08"); }; // 0x08
    virtual void WINAPI pUnknown_0C() { throw std::exception("pUnknown_0C"); }; // 0x0C
    virtual void WINAPI pUnknown_10() { throw std::exception("pUnknown_10"); }; // 0x10
    virtual void WINAPI pUnknown_14() { throw std::exception("pUnknown_14"); }; // 0x14
    virtual void WINAPI pUnknown_18() { throw std::exception("pUnknown_18"); }; // 0x18
    virtual void WINAPI pUnknown_1C() { throw std::exception("pUnknown_1C"); }; // 0x1C
    // If need to add more VMT (Virtual Method Table), add them above here.
    uint32_t ref_count;                                     // 0x04
};
// Require to verify there is no other unknown additional data by compiler itself.
static_assert(sizeof(CUnknownTemplate) == 0x08);

struct CMcpxVoiceClient: CUnknownTemplate {
    CMcpxVoiceClient() : settings(default_settings) {};
    // all VMT (Virtual Method Table) are stored in local offset 0x00's pointer
    virtual ~CMcpxVoiceClient() {};

    // CUnknownTemplate                                     // 0x00 - ???
    struct _settings
    {
        uint32_t            Unknown2[(0x300-8)/4];          // 0x08 - ???
    }
    settings;

    // global _settings for this class
    static _settings default_settings;
};
// Require to verify there is no other unknown additional data by compiler itself.
static_assert(sizeof(CMcpxVoiceClient) == 0x300);

struct CDirectSoundVoice : CUnknownGenericManager {
    CDirectSoundVoice(bool is3D);
    // all VMT (Virtual Method Table) are stored in local offset 0x00's pointer
    virtual ~CDirectSoundVoice();

    // CUnknownGenericManager                                   // 0x00 - ???
    union _settings {
        struct _unknown
        {
            uint32_t                unknown_08[0x300 / 4];      // 0x000 - 0x300 (unknown size, likely over 0x200 size.
        } unknown;

        struct _r4034_lower {
            xbox::addr                  p_unknown_08;               // 0x008
            uint16_t                unknown_0C;                 // 0x00C // zero'd - unknown
            XBOXADPCMWAVEFORMAT*    p_audio_format;             // 0x010 // Same as XBOXADPCMWAVEFORMAT / WAVEFORMATEX structure
            int32_t                 pitch;                      // 0x014 // Always init and custom pitch from SetFrequency, SetPitch, SetFormat, etc calls.
            int32_t                 volume;                     // 0x018 // default: (set volume - headroom)
            uint32_t                headroom;                   // 0x01C // default: (set headroom then update volume)
        } r4034_lower;

        struct _r4039_only {
            uint32_t                unknown_08;                 // 0x008
            uint32_t                audio_codec;                // 0x00C // Setter is 32 bit, yet getter is 16 bit integer reader
            uint32_t                nChannels;                  // 0x010
            uint32_t                cbSize;                     // 0x014
            int32_t                 nSamplesPerSec_default;     // 0x018 // Always original frequency for check if SetFrequency is given with 0 then use original one.
            uint32_t                bitsPerSample;              // 0x01C
            int32_t                 pitch;                      // 0x020 // Always init and custom pitch from SetFrequency, SetPitch, SetFormat, etc calls.
            int32_t                 volume;                     // 0x024 // (set volume - headroom)
            uint32_t                headroom;                   // 0x028 // (set headroom then update volume)
            uint32_t                unknown_2C[(0x300 - 0x2C) / 4]; // 0x02C - 0x300 (unknown size, likely over 0x200 size.
        } r4039_only;

        struct _r4134_upper {
            uint32_t                unknown_08;                 // 0x008
            uint16_t                audio_codec;                // 0x00C
            uint8_t                 nChannels;                  // 0x00E
            uint8_t                 cbSize;                     // 0x00F
            uint32_t                nSamplesPerSec_default;     // 0x010 // Always original frequency for check if SetFrequency is given with 0 then use original one.
            uint32_t                bitsPerSample;              // 0x014
            int32_t                 pitch;                      // 0x018 // Always init and custom pitch from SetFrequency, SetPitch, SetFormat, etc calls.
            int32_t                 volume;                     // 0x01C // (set volume - headroom)
            uint32_t                headroom;                   // 0x020 // (set headroom then update volume)
            uint32_t                unknown_24[(0x300 - 0x24) / 4]; // 0x024 - 0x300 (unknown size, likely over 0x200 size.
        } r4134_upper;
    } settings;
    static_assert(sizeof(_settings) == 0x300); // Not really require

    // Generic interface without need to check xdk's build revision every time.
    typedef void            (*pGetFormat)(_settings& settings, audio_format& format);
    typedef void            (*pSetFormat)(_settings& settings, audio_format format);
    typedef uint32_t        (*pGetUint32)(_settings& settings);
    typedef void            (*pSetUint32)(_settings& settings, uint32_t value);
    typedef int32_t         (*pGetInt32)(_settings& settings);
    typedef void            (*pSetInt32)(_settings& settings, int32_t value);
    struct {
        pGetFormat          GetFormat;
        pSetFormat          SetFormat;
        pGetUint32          GetFrequencyDefault;
        pGetInt32           GetPitch;
        pSetInt32           SetPitch;
        pGetInt32           GetVolume;
        pSetInt32           SetVolume;
        pGetUint32          GetHeadroom;
        pSetUint32          SetHeadroom;
    } funcs;
    static_assert(sizeof(funcs) == 0x24); // Not really require

    inline void GetFormat(audio_format& format) {
        funcs.GetFormat(settings, format);
    };
    inline void SetFormat(audio_format format) {
        funcs.SetFormat(settings, format);
    };
    inline uint32_t GetFrequencyDefault() {
        return funcs.GetFrequencyDefault(settings);
    };
    inline int32_t GetPitch() {
        return funcs.GetPitch(settings);
    };
    inline void SetPitch(int32_t pitch) {
        funcs.SetPitch(settings, pitch);
    };
    inline int32_t GetVolume() {
        return funcs.GetVolume(settings);
    };
    inline void SetVolume(int32_t volume) {
        funcs.SetVolume(settings, volume);
    };
    inline uint32_t GetHeadroom() {
        return funcs.GetHeadroom(settings);
    };
    inline void SetHeadroom(uint32_t headroom) {
        funcs.SetHeadroom(settings, headroom);
    };
};
// Require to verify there is no other unknown additional data by compiler itself.
static_assert(sizeof(CDirectSoundVoice) == sizeof(CUnknownGenericManager) + sizeof(CDirectSoundVoice::_settings) + sizeof(CDirectSoundVoice::funcs));

struct DSBUFFER_S : CUnknownTemplate {

    // CUnknownTemplate    unknown_00;                      // Offset 0x00 // -0x1C
    struct DSBUFFER_C {
        uint32_t            unknown_08;                     // Offset 0x08 // -0x14
        CMcpxVoiceClient*   p_CMcpxVoiceClient;             // Offset 0x0C // -0x10
        CDirectSoundVoice*  p_CDSVoice;                     // Offset 0x10 // -0x0C
        xbox::addr              p_unknown_14;                   // Offset 0x14 // -0x08 // (points to this address)
        xbox::addr              p_unknown_18;                   // Offset 0x18 // -0x04 // (points to above address)
    } dsb_c;
    static_assert(sizeof(DSBUFFER_C) == 0x14);

    struct DSBUFFER_I {
        CDirectSoundVoice*  p_CDSVoice;                     // Offset 0x1C //  0x00 // Same as p_CDSVoice (above); pThis
        CMcpxVoiceClient*   p_CMcpxVoiceClient;             // Offset 0x20 //  0x04 // Same as p_CMcpxVoiceClient (above)
        xbox::addr              p_unknown_24;                   // Offset 0x24 //  0x08
        xbox::addr              p_unknown_28;                   // Offset 0x28 //  0x0C
        uint32_t            unknown_2C;                     // Offset 0x2C //  0x10 // was integer, later shift to offset 0x30
        uint32_t            unknown_30;                     // Offset 0x30 //  0x14 // later shifted from offset 0x2C; integer
    } dsb_i;
    static_assert(sizeof(DSBUFFER_I) == 0x18);

    DSBUFFER_S(bool is3D) {
        init(is3D);
    }
    virtual ~DSBUFFER_S() {
        delete dsb_c.p_CMcpxVoiceClient;
        delete dsb_c.p_CDSVoice;
    }
    virtual void init_member() {
        dsb_c.unknown_08 = 0;
        dsb_i.p_unknown_24 = xbox::zero;
        dsb_i.p_unknown_28 = xbox::zero;
        dsb_i.unknown_2C = xbox::zero;
        dsb_i.unknown_30 = xbox::zero;
    }
    virtual void init(bool is3D) {
        init_member();
        dsb_c.p_CMcpxVoiceClient = new CMcpxVoiceClient(); dsb_i.p_CMcpxVoiceClient = dsb_c.p_CMcpxVoiceClient;
        dsb_c.p_CDSVoice = new CDirectSoundVoice(is3D); dsb_i.p_CDSVoice = dsb_c.p_CDSVoice;
        dsb_c.p_unknown_18 = reinterpret_cast<xbox::addr>(&dsb_c.p_unknown_14);
        dsb_c.p_unknown_14 = dsb_c.p_unknown_18;
    }
};
// Require to verify there is no other unknown additional data by compiler itself.
static_assert(sizeof(DSBUFFER_S) == 0x34);

}
