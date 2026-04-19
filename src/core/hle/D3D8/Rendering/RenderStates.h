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
#include "EmuShared.h"
#include "core\kernel\init\CxbxKrnl.h"
#include "core\kernel\support\Emu.h"
#include "core\hle\Intercept.hpp"
#include "core\hle\D3D8\XbD3D8Types.h"

class XboxRenderStateConverter
{
public:
    bool Init();
    void Apply();

    void* GetPixelShaderRenderStatePointer();

    bool XboxRenderStateExists(uint32_t state);
    void SetXboxRenderState(uint32_t State, uint32_t Value);
    void SetWireFrameMode(int mode);
    void SetDirty();
    uint32_t GetXboxRenderState(uint32_t State);
    float GetXboxRenderStateAsFloat(uint32_t State);
private:
    void VerifyAndFixDeferredRenderStateOffset();
    void DeriveRenderStateOffsetFromDeferredRenderStateOffset();
    void StoreInitialValues();
    void BuildRenderStateMappingTable();

    bool XboxRenderStateValueChanged(uint32_t State);

    void ApplySimpleRenderState(uint32_t State, uint32_t Value);
    void ApplyDeferredRenderState(uint32_t State, uint32_t Value);
    void ApplyComplexRenderState(uint32_t State, uint32_t Value);

    uint32_t* D3D__RenderState = nullptr;
    int WireFrameMode = 0;
    // NOTE: RenderStates are 32bit values, but using a 64bit value allows the upper bits to be used as a 'force dirty' flag
    std::array<uint64_t, xbox::X_D3DRS_LAST + 1> PreviousRenderStateValues;
    std::array<int, xbox::X_D3DRS_LAST + 1>  XboxRenderStateOffsets;
};
