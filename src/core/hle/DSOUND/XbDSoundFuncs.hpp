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
#include "XbDSoundTypes.h"

namespace xbox {
    namespace DSOUND {

    static inline uint16_t PresetChannelDefault(uint32_t Xb_flags)
    {
        if (Xb_flags & XTL_DSBCAPS_CTRL3D) {
            return 1; // 3D audio only accept mono channel
        }
        return 2; // Default to stereo channel
    }

    // A helper class, converting old (pre-XDK 4039) mixbin format to the new (XDK 4039 and newer) format
    class CMixBinConverter
    {
    public:
        CMixBinConverter(xbox::dword_xt dwMixBinMask, const xbox::long_xt* alVolumes) {

            if (dwMixBinMask != 0) {
                for (unsigned int i = 0; i < 32; i++) {
                    if ((dwMixBinMask & (1 << i)) != 0) {
                        xbox::X_DSMIXBINVOLUMEPAIR pair { i, alVolumes != nullptr ? *alVolumes++ : 0 };
                        m_volumePairs.emplace_back(pair);
                    }
                }

                m_mixBins.dwCount = static_cast<xbox::dword_xt>(m_volumePairs.size());
                m_mixBins.lpMixBinVolumePairs = m_volumePairs.data();
            }
            else {
                m_mixBins.dwCount = 0;
                m_mixBins.lpMixBinVolumePairs = nullptr;
            }
        }

        xbox::X_LPDSMIXBINS GetMixBins() { return m_mixBins.dwCount != 0 ? &m_mixBins : nullptr; }

    private:
        std::vector<xbox::X_DSMIXBINVOLUMEPAIR> m_volumePairs;
        xbox::X_DSMIXBINS m_mixBins;
    };

    }
}
