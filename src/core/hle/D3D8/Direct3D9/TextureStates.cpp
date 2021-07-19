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
#include "core/hle/D3D8/XbVertexShader.h" // For g_Xbox_VertexShaderMode and VertexShaderMode::FixedFunction
#include "core/hle/D3D8/Direct3D9/Direct3D9.h" // For g_pD3DDevice
#include <optional>

typedef struct {
    const char* S;          // String representation.
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
    // Deferred states start at 0, this means that D3DDeferredTextureState IS D3D__TextureState
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
    for (int State = xbox::X_D3DTSS_FIRST; State <= xbox::X_D3DTSS_LAST; State++) {
        int index = State;

        // On early XDKs, we need to shuffle the values around a little
        // TODO: Verify which XDK version this change occurred at
        // Values range 0-9 (D3DTSS_COLOROP to D3DTSS_TEXTURETRANSFORMFLAGS) become 12-21
        // Values 10-21 (D3DTSS_ADDRESSU to D3DTSS_ALPHAKILL) become 0-11
        bool bOldOrder = g_LibVersion_D3D8 <= 3948; // Verfied old order in 3944, new order in 4039

        if (bOldOrder) {
            if (State <= 9) {
                index += 12;
            } else if (State <= 21) {
                index -= 10;
            }
        }

        EmuLog(LOG_LEVEL::INFO, "%s = %d", CxbxTextureStateInfo[State].S, index);
        XboxTextureStateOffsets[index] = State;
    }
}

DWORD XboxTextureStateConverter::GetHostTextureOpValue(DWORD Value)
{
    switch (Value) {
        case xbox::X_D3DTOP_DISABLE: return D3DTOP_DISABLE;
        case xbox::X_D3DTOP_SELECTARG1: return D3DTOP_SELECTARG1;
        case xbox::X_D3DTOP_SELECTARG2: return D3DTOP_SELECTARG2;
        case xbox::X_D3DTOP_MODULATE: return D3DTOP_MODULATE;
        case xbox::X_D3DTOP_MODULATE2X: return D3DTOP_MODULATE2X;
        case xbox::X_D3DTOP_MODULATE4X: return D3DTOP_MODULATE4X;
        case xbox::X_D3DTOP_ADD: return D3DTOP_ADD;
        case xbox::X_D3DTOP_ADDSIGNED: return D3DTOP_ADDSIGNED;
        case xbox::X_D3DTOP_ADDSIGNED2X: return D3DTOP_ADDSIGNED2X;
        case xbox::X_D3DTOP_SUBTRACT: return D3DTOP_SUBTRACT;
        case xbox::X_D3DTOP_ADDSMOOTH: return D3DTOP_ADDSMOOTH;
        case xbox::X_D3DTOP_BLENDDIFFUSEALPHA: return D3DTOP_BLENDDIFFUSEALPHA;
        case xbox::X_D3DTOP_BLENDCURRENTALPHA: return D3DTOP_BLENDCURRENTALPHA;
        case xbox::X_D3DTOP_BLENDTEXTUREALPHA: return D3DTOP_BLENDTEXTUREALPHA;
        case xbox::X_D3DTOP_BLENDFACTORALPHA: return D3DTOP_BLENDFACTORALPHA;
        case xbox::X_D3DTOP_BLENDTEXTUREALPHAPM: return D3DTOP_BLENDTEXTUREALPHAPM;
        case xbox::X_D3DTOP_PREMODULATE: return D3DTOP_PREMODULATE;
        case xbox::X_D3DTOP_MODULATEALPHA_ADDCOLOR: return D3DTOP_MODULATEALPHA_ADDCOLOR;
        case xbox::X_D3DTOP_MODULATECOLOR_ADDALPHA: return D3DTOP_MODULATECOLOR_ADDALPHA;
        case xbox::X_D3DTOP_MODULATEINVALPHA_ADDCOLOR: return D3DTOP_MODULATEINVALPHA_ADDCOLOR;
        case xbox::X_D3DTOP_MODULATEINVCOLOR_ADDALPHA: return D3DTOP_MODULATEINVCOLOR_ADDALPHA;
        case xbox::X_D3DTOP_DOTPRODUCT3: return D3DTOP_DOTPRODUCT3;
        case xbox::X_D3DTOP_MULTIPLYADD: return D3DTOP_MULTIPLYADD;
        case xbox::X_D3DTOP_LERP: return D3DTOP_LERP;
        case xbox::X_D3DTOP_BUMPENVMAP: return D3DTOP_BUMPENVMAP;
        case xbox::X_D3DTOP_BUMPENVMAPLUMINANCE: return D3DTOP_BUMPENVMAPLUMINANCE;
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
	// TODO use stage 3 when we roll our own point sprites after moving off D3D9
    bool pointSpriteOverride = false;
    bool pointSpritesEnabled = false;
    pointSpritesEnabled = pXboxRenderStates->GetXboxRenderState(xbox::X_D3DRS_POINTSPRITEENABLE);
    if (pointSpritesEnabled) {
        pointSpriteOverride = true;
    }

    for (int XboxStage = 0; XboxStage < xbox::X_D3DTS_STAGECOUNT; XboxStage++) {
        // If point sprites are enabled, we need to overwrite our existing state 0 with State 3 also
        DWORD HostStage = (pointSpriteOverride && XboxStage == 3) ? 0 : XboxStage;

        for (int State = xbox::X_D3DTSS_FIRST; State <= xbox::X_D3DTSS_LAST; State++) {
            // Read the value of the current stage/state from the Xbox data structure
            DWORD XboxValue = Get(XboxStage, State);
            DWORD PcValue = XboxValue;

            // If the state hasn't changed, skip setting it
            auto lastState = &PreviousStates[XboxStage][State];
            if (*lastState == XboxValue) {
                continue;
            }

            switch (State) {
                // These types map 1:1 but have some unsupported values
                case xbox::X_D3DTSS_ADDRESSU: case xbox::X_D3DTSS_ADDRESSV: case xbox::X_D3DTSS_ADDRESSW:
                    switch (XboxValue) {
                        case 0: // Let's ignore zero (its no known X_D3DTADDRESS_ mode, but logging this seems useless)
                        case xbox::X_D3DTADDRESS_WRAP:        // = 1 = D3DTADDRESS_WRAP = 1,
                        case xbox::X_D3DTADDRESS_MIRROR:      // = 2 = D3DTADDRESS_MIRROR = 2,
                        case xbox::X_D3DTADDRESS_CLAMP:       // = 3 = D3DTADDRESS_CLAMP = 3,
                        case xbox::X_D3DTADDRESS_BORDER:      // = 4 = D3DTADDRESS_BORDER = 4,
                            // These match host Direct3D 9 values, so no update necessary
                            break;
                        case xbox::X_D3DTADDRESS_CLAMPTOEDGE: // = 5
                            LOG_TEST_CASE("X_D3DTADDRESS_CLAMPTOEDGE unsupported, falling back to D3DTADDRESS_BORDER");
                            // D3DTADDRESS_BORDER is the closest host match, CLAMPTOEDGE is identical
                            // Except it has additional restrictions.
                            PcValue = D3DTADDRESS_BORDER;
                            break;
                        default:
                            EmuLog(LOG_LEVEL::WARNING, "Unsupported X_D3DTSS_ADDRESS? value %x", XboxValue);
                            PcValue = D3DTADDRESS_WRAP;
                            break;
                    }
                    break;
                case xbox::X_D3DTSS_MAGFILTER: case xbox::X_D3DTSS_MINFILTER: case xbox::X_D3DTSS_MIPFILTER:
                    switch (XboxValue) {
                        case xbox::X_D3DTEXF_NONE:        // = 0 = D3DTEXF_NONE = 0,        // filtering disabled (valid for mip filter only)
                        case xbox::X_D3DTEXF_POINT:       // = 1 = D3DTEXF_POINT = 1,       // nearest
                        case xbox::X_D3DTEXF_LINEAR:      // = 2 = D3DTEXF_LINEAR = 2,      // linear interpolation
                        case xbox::X_D3DTEXF_ANISOTROPIC: // = 3 = D3DTEXF_ANISOTROPIC = 3, // anisotropic
                            // These match host Direct3D 9 values, so no update necessary
                            break;
                        case xbox::X_D3DTEXF_QUINCUNX:    // = 4; // quincunx kernel (Xbox extension), also known as "flat cubic"
                            LOG_TEST_CASE("X_D3DTEXF_QUINCUNX unsupported, falling back to D3DTEXF_ANISOTROPIC");
                            PcValue = D3DTEXF_ANISOTROPIC;
                            break;
                        case xbox::X_D3DTEXF_GAUSSIANCUBIC: // = 5 // Xbox extension, different cubic kernel
                            // Direct3D 9 alternatives : 
                            // D3DTEXF_PYRAMIDALQUAD = 6,    // 4-sample tent
                            // D3DTEXF_GAUSSIANQUAD = 7,    // 4-sample gaussian
                            // D3DTEXF_CONVOLUTIONMONO = 8,    // Convolution filter for monochrome textures
                            LOG_TEST_CASE("X_D3DTEXF_QUINCUNX unsupported, falling back to D3DTEXF_GAUSSIANQUAD");
                            PcValue = D3DTEXF_GAUSSIANQUAD;
                            break;
                        default:
                            EmuLog(LOG_LEVEL::WARNING, "Unsupported X_D3DTSS_M??FILTER value %x", XboxValue);
                            PcValue = D3DTEXF_NONE;
                            break;
                    }
                    break;
                case xbox::X_D3DTSS_TEXCOORDINDEX: {
                    int texCoordIndex = XboxValue & 0x0000FFFF;
                    if (texCoordIndex > 3) {
                        LOG_TEST_CASE("TEXCOORDINDEX out of bounds, masking to lowest 2 bits");
                        texCoordIndex = XboxValue & 3;
                    }
                    switch (XboxValue & 0xFFFF0000) {
                        case X_D3DTSS_TCI_PASSTHRU:                    // = 0x00000000
                        case X_D3DTSS_TCI_CAMERASPACENORMAL:           // = 0x00010000
                        case X_D3DTSS_TCI_CAMERASPACEPOSITION:         // = 0x00020000
                        case X_D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR: // = 0x00030000
                            // These match host Direct3D 9 values, so no update necessary
                            break;
                        case X_D3DTSS_TCI_OBJECT:                      // = 0x00040000
                            // Collides with host Direct3D 9 D3DTSS_TCI_SPHEREMAP
                            // This value is not supported on host in Direct3D 9
                            // It probably means "TexGen ObjectLinear", or '(untransformed) object space identity mapping'
                            LOG_TEST_CASE("Xbox D3DTSS_TCI_OBJECT unsupported on host");
                            // Test-case : Terrain XDK sample
                            PcValue = texCoordIndex;
                            break;
                        case X_D3DTSS_TCI_SPHEREMAP:                   // = 0x00050000
                            // Convert Xbox sphere mapping bit to host Direct3D 9 (which uses a different bit)
                            PcValue = D3DTSS_TCI_SPHEREMAP | texCoordIndex;
                            break;
                        default:
                            EmuLog(LOG_LEVEL::WARNING, "Unsupported X_D3DTSS_TEXCOORDINDEX value %x", XboxValue);
                            PcValue = texCoordIndex;
                            break;
                    }
                    break;
                }
                // These types require value remapping for all supported values
                case xbox::X_D3DTSS_COLOROP: case xbox::X_D3DTSS_ALPHAOP:
                    PcValue = GetHostTextureOpValue(XboxValue);
                    break;
                // These types require no conversion, so we just pass through as-is
                case xbox::X_D3DTSS_COLORARG0: case xbox::X_D3DTSS_COLORARG1: case xbox::X_D3DTSS_COLORARG2:
                case xbox::X_D3DTSS_ALPHAARG0: case xbox::X_D3DTSS_ALPHAARG1: case xbox::X_D3DTSS_ALPHAARG2:
                case xbox::X_D3DTSS_RESULTARG: case xbox::X_D3DTSS_TEXTURETRANSFORMFLAGS:
                    break;
                case xbox::X_D3DTSS_BUMPENVMAT00: case xbox::X_D3DTSS_BUMPENVMAT01:
                case xbox::X_D3DTSS_BUMPENVMAT11: case xbox::X_D3DTSS_BUMPENVMAT10:
                case xbox::X_D3DTSS_BUMPENVLSCALE: case xbox::X_D3DTSS_BUMPENVLOFFSET:
#if 0 // New, doesn't work yet
                    continue; // Note : Since DxbxUpdateActivePixelShader() reads these too, you'd expect here we could skip, but alas. TODO: Fix PS HLSL to not depend on host D3D TSS
#endif
                case xbox::X_D3DTSS_BORDERCOLOR: case xbox::X_D3DTSS_MIPMAPLODBIAS:
                case xbox::X_D3DTSS_MAXMIPLEVEL: case xbox::X_D3DTSS_MAXANISOTROPY:
                    break;
                default:
                    // Only log missing state if it has a PC counterpart
                    if (CxbxTextureStateInfo[State].PC != 0) {
                        EmuLog(LOG_LEVEL::WARNING, "XboxTextureStateConverter::Apply(%s, 0x%.08X) is unimplemented!", CxbxTextureStateInfo[State].S, XboxValue);
                    }
                    break;
            }

            // Skip Texture States that don't have a defined PC counterpart
            if (CxbxTextureStateInfo[State].PC == 0) {
                continue;
            }

            if (CxbxTextureStateInfo[State].IsSamplerState) {
#ifdef CXBX_USE_D3D11
				static D3D11_SAMPLER_DESC g_GlobalSamplerDesc = {}; // TODO : Move to globals section

				switch (State) {
				case xbox::X_D3DTSS_ADDRESSU: g_GlobalSamplerDesc.AddressU = pcValue; break;
				case xbox::X_D3DTSS_ADDRESSV: g_GlobalSamplerDesc.AddressV = pcValue; break;
				case xbox::X_D3DTSS_ADDRESSW: g_GlobalSamplerDesc.AddressW = pcValue; break;
				case xbox::X_D3DTSS_MAGFILTER: g_GlobalSamplerDesc.Filter = pcValue; break; // TODO : Which Filter??
				case xbox::X_D3DTSS_MINFILTER: g_GlobalSamplerDesc.Filter = pcValue; break;	// TODO : Which Filter??
				case xbox::X_D3DTSS_MIPFILTER: g_GlobalSamplerDesc.Filter = pcValue; break;	// TODO : Which Filter??
				case xbox::X_D3DTSS_MIPMAPLODBIAS: g_GlobalSamplerDesc.MipLODBias = pcValue; break;
				case xbox::X_D3DTSS_MAXMIPLEVEL: g_GlobalSamplerDesc.MaxLOD = pcValue; break; // TODO : What about MinLOD?
				case xbox::X_D3DTSS_MAXANISOTROPY: g_GlobalSamplerDesc.MaxAnisotropy = pcValue; break;
				case xbox::X_D3DTSS_BORDERCOLOR: g_GlobalSamplerDesc.BorderColor = D3DCOLOR(pcValue); break;
				}

				ID3D11SamplerState *pSamplerState = nullptr;
				HRESULT hRet;

				hRet = g_pD3DDevice->CreateSamplerState(&g_GlobalSamplerDesc, &pSamplerState); // TODO : What about lifetime management?
				hRet = g_pD3DDeviceContext->PSSetSamplers(0, 1, pSamplerState);
#else
                g_pD3DDevice->SetSamplerState(HostStage, (D3DSAMPLERSTATETYPE)CxbxTextureStateInfo[State].PC, PcValue);
#endif
            } else {
#ifdef CXBX_USE_D3D11
				switch (State) {
				case xbox::X_D3DTSS_COLORKEYOP: break;
				case xbox::X_D3DTSS_COLORSIGN: break;
				case xbox::X_D3DTSS_ALPHAKILL: break;
				case xbox::X_D3DTSS_COLOROP: break;
				case xbox::X_D3DTSS_COLORARG0: break;
				case xbox::X_D3DTSS_COLORARG1: break;
				case xbox::X_D3DTSS_COLORARG2: break;
				case xbox::X_D3DTSS_ALPHAOP: break;
				case xbox::X_D3DTSS_ALPHAARG0: break;
				case xbox::X_D3DTSS_ALPHAARG1: break;
				case xbox::X_D3DTSS_ALPHAARG2: break;
				case xbox::X_D3DTSS_RESULTARG: break;
				case xbox::X_D3DTSS_TEXTURETRANSFORMFLAGS: break;
				case xbox::X_D3DTSS_BUMPENVMAT00: break;
				case xbox::X_D3DTSS_BUMPENVMAT01: break;
				case xbox::X_D3DTSS_BUMPENVMAT11: break;
				case xbox::X_D3DTSS_BUMPENVMAT10: break;
				case xbox::X_D3DTSS_BUMPENVLSCALE: break;
				case xbox::X_D3DTSS_BUMPENVLOFFSET: break;
				case xbox::X_D3DTSS_TEXCOORDINDEX: break;
				}
				// TODO : Are the above all catered for in our fixed function shader, and ignored otherwise?
				// If not, see which of D3D11_RASTERIZER_DESC1, D3D11_DEPTH_STENCIL_DESC, D3D11_BLEND_DESC1 covers this...
#else
                g_pD3DDevice->SetTextureStageState(HostStage, (D3DTEXTURESTAGESTATETYPE)CxbxTextureStateInfo[State].PC, PcValue);
#endif
            }

            // Record we set a state
            lastState->emplace(XboxValue);
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

// Normalize values which may have different mappings per XDK version
DWORD NormalizeValue(DWORD xboxState, DWORD value) {
    if (g_LibVersion_D3D8 <= 3948) {
        // D3DTOP verified old order in 3948, new order in 4039
        switch (xboxState) {
        case xbox::X_D3DTSS_COLOROP:
        case xbox::X_D3DTSS_ALPHAOP:
            switch (value) {
            case 13:
                return xbox::X_D3DTOP_BLENDTEXTUREALPHA;
            case 14:
                return xbox::X_D3DTOP_BLENDFACTORALPHA;
            case 15:
                return xbox::X_D3DTOP_BLENDTEXTUREALPHAPM;
            case 16:
                return xbox::X_D3DTOP_BLENDCURRENTALPHA;
            }
        }
    }

    return value;
}

uint32_t XboxTextureStateConverter::Get(int textureStage, DWORD xboxState) {
    if (textureStage < 0 || textureStage > 3)
        CxbxrAbort("Requested texture stage was out of range: %d", textureStage);
    if (xboxState < xbox::X_D3DTSS_FIRST || xboxState > xbox::X_D3DTSS_LAST)
        CxbxrAbort("Requested texture state was out of range: %d", xboxState);

    // Read the value of the current stage/state from the Xbox data structure
    DWORD rawValue = D3D__TextureState[(textureStage * xbox::X_D3DTS_STAGESIZE) + XboxTextureStateOffsets[xboxState]];

    return NormalizeValue(xboxState, rawValue);
}
