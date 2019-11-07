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
#define LOG_PREFIX CXBXR_MODULE::D3DST

#include "TextureStates.h"
#include "core\kernel\init\CxbxKrnl.h"
#include "core\kernel\support\Emu.h"
#include "Logging.h"
#include "EmuShared.h"
#include "core/hle/Intercept.hpp"
#include "RenderStates.h"
#include "core/hle/D3D8/Direct3D9/Direct3D9.h" // For g_pD3DDevice

typedef struct {
    char* S;                // String representation.
    bool IsSamplerState;    // True if the state maps to a Sampler State instead of Texture Stage
    DWORD PC;               // PC Index
} TextureStateInfo;

TextureStateInfo CxbxTextureStateInfo[] = {
    { "D3DTSS_ADDRESSU",                true,   D3DSAMP_ADDRESSU },
    { "D3DTSS_ADDRESSV",                true,   D3DSAMP_ADDRESSV },
    { "D3DTSS_ADDRESSW",                true,   D3DSAMP_ADDRESSW },
    { "D3DTSS_MAGFILTER",               true,   D3DSAMP_MAGFILTER },
    { "D3DTSS_MINFILTER",               true,   D3DSAMP_MINFILTER },
    { "D3DTSS_MIPFILTER",               true,   D3DSAMP_MIPFILTER },
    { "D3DTSS_MIPMAPLODBIAS",           true,   D3DSAMP_MIPMAPLODBIAS },
    { "D3DTSS_MAXMIPLEVEL",             true,   D3DSAMP_MAXMIPLEVEL },
    { "D3DTSS_MAXANISOTROPY",           true,   D3DSAMP_MAXANISOTROPY },
    { "D3DTSS_COLORKEYOP",              false,  0 },
    { "D3DTSS_COLORSIGN",               false,  0 },
    { "D3DTSS_ALPHAKILL",               false,  0 },
    { "D3DTSS_COLOROP",                 false,  D3DTSS_COLOROP },
    { "D3DTSS_COLORARG0",               false,  D3DTSS_COLORARG0 },
    { "D3DTSS_COLORARG1",               false,  D3DTSS_COLORARG1 },
    { "D3DTSS_COLORARG2",               false,  D3DTSS_COLORARG2 },
    { "D3DTSS_ALPHAOP",                 false,  D3DTSS_ALPHAOP },
    { "D3DTSS_ALPHAARG0",               false,  D3DTSS_ALPHAARG0 },
    { "D3DTSS_ALPHAARG1",               false,  D3DTSS_ALPHAARG1 },
    { "D3DTSS_ALPHAARG2",               false,  D3DTSS_ALPHAARG2 },
    { "D3DTSS_RESULTARG",               false,  D3DTSS_RESULTARG },
    { "D3DTSS_TEXTURETRANSFORMFLAGS",   false,  D3DTSS_TEXTURETRANSFORMFLAGS },
    { "D3DTSS_BUMPENVMAT00",            false,  D3DTSS_BUMPENVMAT00 },
    { "D3DTSS_BUMPENVMAT01",            false,  D3DTSS_BUMPENVMAT01 },
    { "D3DTSS_BUMPENVMAT11",            false,  D3DTSS_BUMPENVMAT11 },
    { "D3DTSS_BUMPENVMAT10",            false,  D3DTSS_BUMPENVMAT10 },
    { "D3DTSS_BUMPENVLSCALE",           false,  D3DTSS_BUMPENVLSCALE },
    { "D3DTSS_BUMPENVLOFFSET",          false,  D3DTSS_BUMPENVLOFFSET },
    { "D3DTSS_TEXCOORDINDEX",           false,  D3DTSS_TEXCOORDINDEX },
    { "D3DTSS_BORDERCOLOR",             true,   D3DSAMP_BORDERCOLOR },
    { "D3DTSS_COLORKEYCOLOR",           false,  0 },
};

bool XboxTextureStateConverter::Init(XboxRenderStateConverter* pState)
{
    // Deferred states start at 0, this menas that D3DDeferredTextureState IS D3D__TextureState
    // No further works is required to derive the offset
    if (g_SymbolAddresses.find("D3DDeferredTextureState") != g_SymbolAddresses.end()) {
        D3D__TextureState = (uint32_t*)g_SymbolAddresses["D3DDeferredTextureState"];
    } else {
        return false;
    }

    // Build a mapping of Cxbx Texture State indexes to indexes within the current XDK
    BuildTextureStateMappingTable();

    // Store a handle to the Xbox Render State manager
    // This is used to check for Point Sprites
    pXboxRenderStates = pState;

    return true;
}

void XboxTextureStateConverter::BuildTextureStateMappingTable()
{
    EmuLog(LOG_LEVEL::INFO, "Building Cxbx to XDK Texture State Mapping Table");
    for (int State = XTL::X_D3DTSS_FIRST; State <= XTL::X_D3DTSS_LAST; State++) {
        int index = State;

        // On early XDKs, we need to shuffle the values around a little
        // TODO: Verify which XDK version this change occurred at
        // Values range 0-9 (D3DTSS_COLOROP to D3DTSS_TEXTURETRANSFORMFLAGS) become 12-21
        // Values 10-21 (D3DTSS_ADDRESSU to D3DTSS_ALPHAKILL) become 0-11
        if (g_LibVersion_D3D8 <= 3948) {
            if (State <= 9) {
                index += 12;
            } else if (State <= 21) {
                index -= 10;
            }
        }

        EmuLog(LOG_LEVEL::INFO, "%s = %d", CxbxTextureStateInfo[State].S, index);
        XboxTextureStateOffsets[State] = index;
    }
}

DWORD XboxTextureStateConverter::GetHostTextureOpValue(DWORD Value)
{
    switch (Value) {
        case XTL::X_D3DTOP_DISABLE: return D3DTOP_DISABLE;
        case XTL::X_D3DTOP_SELECTARG1: return D3DTOP_SELECTARG1;
        case XTL::X_D3DTOP_SELECTARG2: return D3DTOP_SELECTARG2;
        case XTL::X_D3DTOP_MODULATE: return D3DTOP_MODULATE;
        case XTL::X_D3DTOP_MODULATE2X: return D3DTOP_MODULATE2X;
        case XTL::X_D3DTOP_MODULATE4X: return D3DTOP_MODULATE4X;
        case XTL::X_D3DTOP_ADD: return D3DTOP_ADD;
        case XTL::X_D3DTOP_ADDSIGNED: return D3DTOP_ADDSIGNED;
        case XTL::X_D3DTOP_ADDSIGNED2X: return D3DTOP_ADDSIGNED2X;
        case XTL::X_D3DTOP_SUBTRACT: return D3DTOP_SUBTRACT;
        case XTL::X_D3DTOP_ADDSMOOTH: return D3DTOP_ADDSMOOTH;
        case XTL::X_D3DTOP_BLENDDIFFUSEALPHA: return D3DTOP_BLENDDIFFUSEALPHA;
        case XTL::X_D3DTOP_BLENDCURRENTALPHA: return D3DTOP_BLENDCURRENTALPHA;
        case XTL::X_D3DTOP_BLENDTEXTUREALPHA: return D3DTOP_BLENDTEXTUREALPHA;
        case XTL::X_D3DTOP_BLENDFACTORALPHA: return D3DTOP_BLENDFACTORALPHA;
        case XTL::X_D3DTOP_BLENDTEXTUREALPHAPM: return D3DTOP_BLENDTEXTUREALPHAPM;
        case XTL::X_D3DTOP_PREMODULATE: return D3DTOP_PREMODULATE;
        case XTL::X_D3DTOP_MODULATEALPHA_ADDCOLOR: return D3DTOP_MODULATEALPHA_ADDCOLOR;
        case XTL::X_D3DTOP_MODULATECOLOR_ADDALPHA: return D3DTOP_MODULATECOLOR_ADDALPHA;
        case XTL::X_D3DTOP_MODULATEINVALPHA_ADDCOLOR: return D3DTOP_MODULATEINVALPHA_ADDCOLOR;
        case XTL::X_D3DTOP_MODULATEINVCOLOR_ADDALPHA: return D3DTOP_MODULATEINVCOLOR_ADDALPHA;
        case XTL::X_D3DTOP_DOTPRODUCT3: return D3DTOP_DOTPRODUCT3;
        case XTL::X_D3DTOP_MULTIPLYADD: return D3DTOP_MULTIPLYADD;
        case XTL::X_D3DTOP_LERP: return D3DTOP_LERP;
        case XTL::X_D3DTOP_BUMPENVMAP: return D3DTOP_BUMPENVMAP;
        case XTL::X_D3DTOP_BUMPENVMAPLUMINANCE: return D3DTOP_BUMPENVMAPLUMINANCE;
    }

    EmuLog(LOG_LEVEL::WARNING, "Unsupported D3DTOP Value (%d)", Value);
    return D3DTOP_DISABLE;

}

void XboxTextureStateConverter::Apply()
{
    // Iterate through all texture states/stages

    // Track if we need to overwrite state 0 with 3 because of Point Sprites
    // The Xbox NV2A uses only Stage 3 for point-sprites, so we emulate this
    // by mapping Stage 3 to Stage 0, and disabling all stages > 0
    bool pointSpriteOverride = false;
    bool pointSpritesEnabled = pXboxRenderStates->GetXboxRenderState(XTL::X_D3DRS_POINTSPRITEENABLE);
    if (pointSpritesEnabled) {
        pointSpriteOverride = true;
    }

    for (int XboxStage = 0; XboxStage < XTL::X_D3DTS_STAGECOUNT; XboxStage++) {
        // If point sprites are enabled, we need to overwrite our existing state 0 with State 3 also
        DWORD HostStage = (pointSpriteOverride && XboxStage == 3) ? 0 : XboxStage;

        for (int StateIndex = XTL::X_D3DTSS_FIRST; StateIndex <= XTL::X_D3DTSS_LAST; StateIndex++) {
            // Read the value of the current stage/state from the Xbox data structure
            DWORD Value = D3D__TextureState[(XboxStage * XTL::X_D3DTS_STAGESIZE) + StateIndex];

            // Convert the index of the current state to an index that we can use
            // This handles the case when XDKs have different state values
            DWORD State = XboxTextureStateOffsets[StateIndex];

            switch (State) {
                // These types map 1:1 but have some unsupported values
                case XTL::X_D3DTSS_ADDRESSU: case XTL::X_D3DTSS_ADDRESSV: case XTL::X_D3DTSS_ADDRESSW:
                    if (Value == XTL::X_D3DTADDRESS_CLAMPTOEDGE) {
                        EmuLog(LOG_LEVEL::WARNING, "D3DTADDRESS_CLAMPTOEDGE is unsupported");
                        // D3DTADDRESS_BORDER is the closest host match, CLAMPTOEDGE is identical
                        // Except it has additional restrictions.
                        Value = D3DTADDRESS_BORDER; 
                        break;
                    }
                    break;
                case XTL::X_D3DTSS_MAGFILTER: case XTL::X_D3DTSS_MINFILTER: case XTL::X_D3DTSS_MIPFILTER:
                    if (Value == XTL::X_D3DTEXF_QUINCUNX) {
                        EmuLog(LOG_LEVEL::WARNING, "D3DTEXF_QUINCUNX is unsupported");
                        // Fallback to D3DTEXF_ANISOTROPIC 
                        Value = D3DTEXF_ANISOTROPIC;
                        break;
                    }
                    break;
                case XTL::X_D3DTSS_TEXCOORDINDEX:
                    switch (Value) {
                        case 0x00040000:
                            // This value is TCI_OBJECT on Xbox,which is not supported by the host
                            // In this case, we reset to 0.
                            EmuLog(LOG_LEVEL::WARNING, "EmuD3DDevice_SetTextureState_TexCoordIndex: D3DTSS_TCI_OBJECT is unsupported", Value);
                            Value = 0;
                            break;
                        case 0x00050000:
                            // This value is TCI_SPHERE on Xbox, let's map it to D3DTSS_TCI_SPHEREMAP for the host
                            Value = D3DTSS_TCI_SPHEREMAP;
                            break;
                    }
                    break;
                // These types require value remapping for all supported values
                case XTL::X_D3DTSS_COLOROP: case XTL::X_D3DTSS_ALPHAOP:
                    Value = GetHostTextureOpValue(Value);
                    break;
                // These types  require no conversion, so we just pass through as-is
                case XTL::X_D3DTSS_COLORARG0: case XTL::X_D3DTSS_COLORARG1: case XTL::X_D3DTSS_COLORARG2:
                case XTL::X_D3DTSS_ALPHAARG0: case XTL::X_D3DTSS_ALPHAARG1: case XTL::X_D3DTSS_ALPHAARG2:
                case XTL::X_D3DTSS_RESULTARG: case XTL::X_D3DTSS_TEXTURETRANSFORMFLAGS:
                case XTL::X_D3DTSS_BUMPENVMAT00: case XTL::X_D3DTSS_BUMPENVMAT01:
                case XTL::X_D3DTSS_BUMPENVMAT11: case XTL::X_D3DTSS_BUMPENVMAT10:
                case XTL::X_D3DTSS_BUMPENVLSCALE: case XTL::X_D3DTSS_BUMPENVLOFFSET:
                case XTL::X_D3DTSS_BORDERCOLOR: case XTL::X_D3DTSS_MIPMAPLODBIAS:
                case XTL::X_D3DTSS_MAXMIPLEVEL: case XTL::X_D3DTSS_MAXANISOTROPY:
                    break;
                default:
                    // Only log missing state if it has a PC counterpart
                    if (CxbxTextureStateInfo[State].PC != 0) {
                        EmuLog(LOG_LEVEL::WARNING, "XboxTextureStateConverter::Apply(%s, 0x%.08X) is unimplemented!", CxbxTextureStateInfo[State].S, Value);
                    }
                    break;
            }

            // Skip Texture States that don't have a defined PC counterpart
            if (CxbxTextureStateInfo[State].PC == 0) {
                continue;
            }

            if (CxbxTextureStateInfo[State].IsSamplerState) {
                g_pD3DDevice->SetSamplerState(HostStage, (D3DSAMPLERSTATETYPE)CxbxTextureStateInfo[State].PC, Value);
            } else {
                g_pD3DDevice->SetTextureStageState(HostStage, (D3DTEXTURESTAGESTATETYPE)CxbxTextureStateInfo[State].PC, Value);
            }
        }

        // Make sure we only do this once
        if (pointSpriteOverride && XboxStage == 3) {
            pointSpriteOverride = false;
            XboxStage--;
        }
    }

    if (pointSpritesEnabled) {
        IDirect3DBaseTexture* pTexture;

        // set the point sprites texture
        g_pD3DDevice->GetTexture(3, &pTexture);
        g_pD3DDevice->SetTexture(0, pTexture);

        // Avoid a dangling reference that would lead to a memory leak
        if (pTexture != nullptr)
            pTexture->Release();

        // disable all other stages
        g_pD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
        g_pD3DDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

        // no need to actually copy here, since it was handled in the loop above
    }
}
