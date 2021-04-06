// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
// *  (c) 2019 Luke Usher
// *
// *  All rights reserved
// *
// ******************************************************************

#include <cstdint>
#include <array>
#include "core\hle\D3D8\XbD3D8Types.h"
#include <optional>

#define CXBX_D3DRS_UNSUPPORTED (xbox::X_D3DRS_LAST + 1)

class XboxRenderStateConverter;

class XboxTextureStateConverter
{
public:
    bool Init(XboxRenderStateConverter* state);
    void Apply();
    uint32_t Get(int textureStage, DWORD xboxState);

private:
    void BuildTextureStateMappingTable();
    DWORD GetHostTextureOpValue(DWORD XboxTextureOp);

    // Pointer to Xbox texture states
    // Note mappings may change between XDK versions
    uint32_t* D3D__TextureState = nullptr;
    std::array<int, xbox::X_D3DTSS_LAST + 1> XboxTextureStateOffsets;
    XboxRenderStateConverter* pXboxRenderStates;
    // Holds the last state that was set, so we don't set it again
    std::optional<DWORD> PreviousStates[xbox::X_D3DTS_STAGECOUNT][xbox::X_D3DTSS_LAST + 1] = {};
};
