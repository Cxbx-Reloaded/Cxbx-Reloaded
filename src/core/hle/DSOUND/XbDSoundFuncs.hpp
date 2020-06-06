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

namespace XTL {
    namespace DSOUND {

    static inline uint16_t PresetChannelDefault(uint32_t Xb_flags)
    {
        if (Xb_flags & XTL_DSBCAPS_CTRL3D) {
            return 1; // 3D audio only accept mono channel
        }
        return 2; // Default to stereo channel
    }

    }
}
