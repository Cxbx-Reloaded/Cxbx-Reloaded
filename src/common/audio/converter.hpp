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

#include <cstdint>

// Convert frequency to pitch helper
static inline int32_t converter_freq2pitch(uint32_t freq) {
    // NOTE: pitch = 0 is equal to 48 KHz.
    /* For research purpose of how to convert frequency to pitch and back to frequency.
    // Edit hertz variable to see the result.
    float hertz = 12000.0f;

    float hertzRatio = 48000.0f; // base frequency
    float pitchRatio = 4096.0f;  // pitch per octave

    // Convert hertz to pitch
    float pitch = log2(hertz / hertzRatio) * pitchRatio;

    // Convert pitch to hertz
    hertz = exp((pitch / pitchRatio) * log(2)) * hertzRatio;*/
    return static_cast<int32_t>(log2(freq / 48000.0f) * 4096.0f);
}

// Convert pitch to frequency helper
static inline uint32_t converter_pitch2freq(int32_t pitch) {
    //* See research documentation above for conversion example.
    return static_cast<uint32_t>(exp((pitch / 4096.0f) * log(2)) * 48000.0f);
}
