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
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#define LOG_PREFIX CXBXR_MODULE::D3DST

#include "core\kernel\init\CxbxKrnl.h"
#include "core\kernel\support\Emu.h"
#include "core\hle\D3D8\Direct3D9\Direct3D9.h" // For g_pD3DDevice
#include "core\hle\D3D8\XbConvert.h" // For DxbxRenderStateInfo

// deferred state lookup tables
DWORD *EmuD3DDeferredRenderState = nullptr;
DWORD *EmuD3DDeferredTextureState = nullptr;

#include "core\hle\Intercept.hpp" // For g_SymbolAddresses
#include "common/Settings.hpp"    // For g_LibVersion_D3D8

void VerifyAndFixEmuDeferredRenderStateOffset()
{
    // Verify that EmuD3DDeferredRenderState is correct, if not, we can programatically correct it
    // We should also flag up a warning so this can be fixed upstream in XboxSymbolDatabase!
    // This is made possible by the registration of D3DRS_CULLMODE by XboxSymbolDatabase
    static bool verifiedRenderStateOffset = false;
    if (verifiedRenderStateOffset) {
        return;
    }

    verifiedRenderStateOffset = true;

    DWORD CullModeOffset = g_SymbolAddresses["D3DRS_CULLMODE"];

    // If we found a valid CullMode offset, verify the symbol location
    if (CullModeOffset == 0) {
        EmuLog(LOG_LEVEL::WARNING, "D3DRS_CULLMODE could not be found. Please update the XbSymbolDatabase submodule");
        return;
    }

    // Calculate index of D3DRS_CULLMODE for this XDK. We start counting from the first deferred state (D3DRS_FOGENABLE)
    DWORD CullModeIndex = 0;
    for (int i = XTL::X_D3DRS_FOGENABLE; i < XTL::X_D3DRS_CULLMODE; i++) {
        if (DxbxRenderStateInfo[i].V <= g_LibVersion_D3D8) {
            CullModeIndex++;
        }
    }

    // If the offset was incorrect, calculate the correct offset, log it, and fix it
    if ((DWORD)(&EmuD3DDeferredRenderState[CullModeIndex]) != CullModeOffset) {
        DWORD CorrectOffset = CullModeOffset - (CullModeIndex * sizeof(DWORD));
        EmuLog(LOG_LEVEL::WARNING, "EmuD3DDeferredRenderState returned by XboxSymbolDatabase (0x%08X) was incorrect. Correcting to be 0x%08X.\nPlease file an issue with the XbSymbolDatabase project", EmuD3DDeferredRenderState, CorrectOffset);
        EmuD3DDeferredRenderState = (DWORD*)CorrectOffset;
    }
}

void UpdateDeferredRenderStates()
{
    // Certain D3DRS values need to be checked on each Draw[Indexed]Vertices
    if (EmuD3DDeferredRenderState != 0) {
        // Loop through all deferred render states
        for (unsigned int RenderState = XTL::X_D3DRS_FOGENABLE; RenderState <= XTL::X_D3DRS_PRESENTATIONINTERVAL; RenderState++) {
            // If the current state is not present within our desired XDK, skip it
            if (DxbxRenderStateInfo[RenderState].V >= g_LibVersion_D3D8) {
                continue;
            }

            uint8_t index = RenderState - XTL::X_D3DRS_FOGENABLE;
            DWORD Value = EmuD3DDeferredRenderState[index];

            // Convert from Xbox Data Formats to PC
            switch (RenderState) {
                case XTL::X_D3DRS_FOGSTART:
                case XTL::X_D3DRS_FOGEND: {
                    // HACK: If the fog start/fog-end are negative, make them positive
                    // This fixes Smashing Drive on non-nvidia hardware
                    // Cause appears to be non-nvidia drivers clamping values < 0 to 0
                    // Resulting in the fog formula becoming (0 - d) / 0, which breaks rendering
                    // This prevents that scenario for screen-space fog, *hopefully* without breaking eye-based fog also
                    float fogValue = *(float*)&Value;
                    if (fogValue < 0.0f) {
                        LOG_TEST_CASE("FOGSTART/FOGEND below 0");
                        fogValue = std::abs(fogValue);
                        Value = *(DWORD*)&fogValue;
                    }
                } break;
                case XTL::X_D3DRS_FOGENABLE:
                    if (g_LibVersion_D3D8 == 3925) {
                        // HACK: Many 3925 games only show a black screen if fog is enabled
                        // Initially, this was thought to be bad offsets, but it has been verified to be correct
                        // Until we find out the true underlying cause, disable fog and carry on
                        // Test Cases: Halo, Silent Hill 2.
                        LOG_TEST_CASE("Applying 3925 fog disable hack");
                        Value = false;
                    }
                    break;
                case XTL::X_D3DRS_FOGTABLEMODE:
                case XTL::X_D3DRS_FOGDENSITY:
                case XTL::X_D3DRS_RANGEFOGENABLE:
                case XTL::X_D3DRS_LIGHTING:
                case XTL::X_D3DRS_SPECULARENABLE:
                case XTL::X_D3DRS_LOCALVIEWER:
                case XTL::X_D3DRS_COLORVERTEX:
                case XTL::X_D3DRS_SPECULARMATERIALSOURCE:
                case XTL::X_D3DRS_DIFFUSEMATERIALSOURCE:
                case XTL::X_D3DRS_AMBIENTMATERIALSOURCE:
                case XTL::X_D3DRS_EMISSIVEMATERIALSOURCE:
                case XTL::X_D3DRS_AMBIENT:
                case XTL::X_D3DRS_POINTSIZE:
                case XTL::X_D3DRS_POINTSIZE_MIN:
                case XTL::X_D3DRS_POINTSPRITEENABLE:
                case XTL::X_D3DRS_POINTSCALEENABLE:
                case XTL::X_D3DRS_POINTSCALE_A:
                case XTL::X_D3DRS_POINTSCALE_B:
                case XTL::X_D3DRS_POINTSCALE_C:
                case XTL::X_D3DRS_POINTSIZE_MAX:
                case XTL::X_D3DRS_PATCHEDGESTYLE:
                case XTL::X_D3DRS_PATCHSEGMENTS:
                    // These render states require no conversion, so we can use them as-is
                    break;
                case XTL::X_D3DRS_BACKSPECULARMATERIALSOURCE:
                case XTL::X_D3DRS_BACKDIFFUSEMATERIALSOURCE:
                case XTL::X_D3DRS_BACKAMBIENTMATERIALSOURCE:
                case XTL::X_D3DRS_BACKEMISSIVEMATERIALSOURCE:
                case XTL::X_D3DRS_BACKAMBIENT:
                case XTL::X_D3DRS_SWAPFILTER:
                    // These states are unsupported by the host and are ignored (for now)
                    continue;
                case XTL::X_D3DRS_PRESENTATIONINTERVAL: {
                    // Store this as an override for our frame limiter
                    // Games can use this to limit certain scenes to a desired target framerate for a specific scene
                    // If this value is not set, or is set to 0, the default interval passed to CreateDevice is used
                    extern DWORD g_PresentationIntervalOverride;
                    g_PresentationIntervalOverride = Value;
                } continue;
                case XTL::X_D3DRS_WRAP0:
                case XTL::X_D3DRS_WRAP1:
                case XTL::X_D3DRS_WRAP2:
                case XTL::X_D3DRS_WRAP3: {
                    DWORD OldValue = Value;
                    Value = 0;

                    Value |= (OldValue & 0x00000010) ? D3DWRAPCOORD_0 : 0;
                    Value |= (OldValue & 0x00001000) ? D3DWRAPCOORD_1 : 0;
                    Value |= (OldValue & 0x00100000) ? D3DWRAPCOORD_2 : 0;
                    Value |= (OldValue & 0x01000000) ? D3DWRAPCOORD_3 : 0;
                } break;
                default:
                    EmuLog(LOG_LEVEL::WARNING, "Unimplemented Deferred Render State: %s", DxbxRenderStateInfo[RenderState].S);
                    continue;
            }

            if (DxbxRenderStateInfo[RenderState].PC == 0) {
                continue;
            }

            g_pD3DDevice->SetRenderState(DxbxRenderStateInfo[RenderState].PC, Value);
        }
    }
}

DWORD GetDeferredTextureStateFromIndex(DWORD State)
{
    // On early XDKs, we need to shuffle the values around a little
    // TODO: Verify which XDK version this change occurred at
    if (g_LibVersion_D3D8 <= 3948) {
        // Values range 0-9 (D3DTSS_COLOROP to D3DTSS_TEXTURETRANSFORMFLAGS) become 12-21
        if (State <= 9) {
            return State + 12;
        }

        // All Remaining values 10-21 (D3DTSS_ADDRESSU to D3DTSS_ALPHAKILL) become 0-11
        return State - 10;
    }

    // On later XDKs, the mapping is identical to our representation
    return State;
}

DWORD Map3911ToCxbxD3DTOP(DWORD Value)
{
    switch (Value) {
        case 1: return XTL::X_D3DTOP_DISABLE;
        case 2: return XTL::X_D3DTOP_SELECTARG1;
        case 3: return XTL::X_D3DTOP_SELECTARG2;
        case 4: return XTL::X_D3DTOP_MODULATE;
        case 5: return XTL::X_D3DTOP_MODULATE2X;
        case 6: return XTL::X_D3DTOP_MODULATE4X;
        case 7: return XTL::X_D3DTOP_ADD;
        case 8: return XTL::X_D3DTOP_ADDSIGNED;
        case 9: return XTL::X_D3DTOP_ADDSIGNED2X;
        case 10: return XTL::X_D3DTOP_SUBTRACT;
        case 11: return XTL::X_D3DTOP_ADDSMOOTH;
        case 12: return XTL::X_D3DTOP_BLENDDIFFUSEALPHA;
        case 13: return XTL::X_D3DTOP_BLENDTEXTUREALPHA;
        case 14: return XTL::X_D3DTOP_BLENDFACTORALPHA;
        case 15: return XTL::X_D3DTOP_BLENDTEXTUREALPHAPM;
        case 16: return XTL::X_D3DTOP_BLENDCURRENTALPHA;
        case 17: return XTL::X_D3DTOP_PREMODULATE;
        case 18: return XTL::X_D3DTOP_MODULATEALPHA_ADDCOLOR;
        case 19: return XTL::X_D3DTOP_MODULATECOLOR_ADDALPHA;
        case 20: return XTL::X_D3DTOP_MODULATEINVALPHA_ADDCOLOR;
        case 21: return XTL::X_D3DTOP_MODULATEINVCOLOR_ADDALPHA;
        case 22: return XTL::X_D3DTOP_BUMPENVMAP;
        case 23: return XTL::X_D3DTOP_BUMPENVMAPLUMINANCE;
        case 24: return XTL::X_D3DTOP_DOTPRODUCT3;
        case 25: return XTL::X_D3DTOP_MULTIPLYADD;
        case 26: return XTL::X_D3DTOP_LERP;
    }

    EmuLog(LOG_LEVEL::WARNING, "Unknown X_D3DTOP Value");
    return Value;
}

DWORD TranslateXDKSpecificD3DTOP(DWORD Value)
{
    // TODO: Determine when exactly these values changed
    // So far, 4134 is the earliest version we've seen using these mappings
    // But this may change
    if (g_LibVersion_D3D8 >= 4134) {
        // For these XDKs, the mapping has been confirmed to match our internal mapping
        return Value;
    }

    return Map3911ToCxbxD3DTOP(Value);
}

void UpdateDeferredTextureStates()
{
    // Iterate through all deferred texture states/stages

    // Track if we need to overwrite state 0 with 3 because of Point Sprites
    // The Xbox NV2A uses only Stage 3 for point-sprites, so we emulate this
    // by mapping Stage 3 to Stage 0, and disabling all stages > 0
    bool pointSpriteOverride = false;
    if (EmuD3DDeferredRenderState[XTL::X_D3DRS_POINTSPRITEENABLE - XTL::X_D3DRS_FOGENABLE] == TRUE) {
        pointSpriteOverride = true;
    }

    for (int XboxStage = 0; XboxStage < XTL::X_D3DTS_STAGECOUNT; XboxStage++) {
        // If point sprites are enabled, we need to overwrite our existing state 0 with State 3 also
        DWORD HostStage = (pointSpriteOverride && XboxStage == 3) ? 0 : XboxStage;

        for (int StateIndex = XTL::X_D3DTSS_DEFERRED_FIRST; StateIndex <= XTL::X_D3DTSS_DEFERRED_LAST; StateIndex++) {
            // Read the value of the current stage/state from the Xbox data structure
            DWORD Value = EmuD3DDeferredTextureState[(XboxStage * XTL::X_D3DTS_STAGESIZE) + StateIndex];
            
            // Convert the index of the current state to an index that we can use
            // This handles the case when XDKs have different state values
            DWORD XboxState = GetDeferredTextureStateFromIndex(StateIndex);

            switch (XboxState) {
                case XTL::X_D3DTSS_ADDRESSU: case XTL::X_D3DTSS_ADDRESSV: case XTL::X_D3DTSS_ADDRESSW:
                    if (Value == XTL::X_D3DTADDRESS_CLAMPTOEDGE) {
                        EmuLog(LOG_LEVEL::WARNING, "ClampToEdge is unsupported");
                        break;
                    }

                    //  These states match the PC counterpart IDs
                    g_pD3DDevice->SetSamplerState(HostStage, (D3DSAMPLERSTATETYPE)(XboxState + 1), Value);
                    break;
                case XTL::X_D3DTSS_MAGFILTER: case XTL::X_D3DTSS_MINFILTER: case XTL::X_D3DTSS_MIPFILTER:
                    if (Value == XTL::X_D3DTEXF_QUINCUNX) {
                        EmuLog(LOG_LEVEL::WARNING, "QuinCunx is unsupported");
                        break;
                    }

                    //  These states (when incremented by 2) match the PC counterpart IDs
                    g_pD3DDevice->SetSamplerState(HostStage, (D3DSAMPLERSTATETYPE)(XboxState + 2), Value);
                    break;
                case XTL::X_D3DTSS_MIPMAPLODBIAS:
                    g_pD3DDevice->SetSamplerState(HostStage, D3DSAMP_MIPMAPLODBIAS, Value);
                    break;
                case XTL::X_D3DTSS_MAXMIPLEVEL:
                    g_pD3DDevice->SetSamplerState(HostStage, D3DSAMP_MAXMIPLEVEL, Value);
                    break;
                case XTL::X_D3DTSS_MAXANISOTROPY:
                    g_pD3DDevice->SetSamplerState(HostStage, D3DSAMP_MAXANISOTROPY, Value);
                    break;
                case XTL::X_D3DTSS_COLORKEYOP: // Xbox ext
                    // Logging Disabled: Causes Dashboard to slow down massively
                    //EmuLog(LOG_LEVEL::WARNING, "D3DTSS_COLORKEYOP is unsupported");
                    break;
                case XTL::X_D3DTSS_COLORSIGN: // Xbox ext
                    // Logging Disabled: Causes Dashboard to slow down massively
                    //EmuLog(LOG_LEVEL::WARNING, "D3DTSS_COLORSIGN is unsupported");
                    break;
                case XTL::X_D3DTSS_ALPHAKILL: // Xbox ext
                    // Logging Disabled: Causes Dashboard to slow down massively
                    //EmuLog(LOG_LEVEL::WARNING, "D3DTSS_ALPHAKILL is unsupported");
                    break;
                case XTL::X_D3DTSS_COLOROP:
                    switch (TranslateXDKSpecificD3DTOP(Value)) {
                        case XTL::X_D3DTOP_DISABLE:
                            g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLOROP, D3DTOP_DISABLE);
                            break;
                        case XTL::X_D3DTOP_SELECTARG1:
                            g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
                            break;
                        case XTL::X_D3DTOP_SELECTARG2:
                            g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
                            break;
                        case XTL::X_D3DTOP_MODULATE:
                            g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLOROP, D3DTOP_MODULATE);
                            break;
                        case XTL::X_D3DTOP_MODULATE2X:
                            g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
                            break;
                        case XTL::X_D3DTOP_MODULATE4X:
                            g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLOROP, D3DTOP_MODULATE4X);
                            break;
                        case XTL::X_D3DTOP_ADD:
                            g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLOROP, D3DTOP_ADD);
                            break;
                        case XTL::X_D3DTOP_ADDSIGNED:
                            g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLOROP, D3DTOP_ADDSIGNED);
                            break;
                        case XTL::X_D3DTOP_ADDSIGNED2X:
                            g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLOROP, D3DTOP_ADDSIGNED2X);
                            break;
                        case XTL::X_D3DTOP_SUBTRACT:
                            g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLOROP, D3DTOP_SUBTRACT);
                            break;
                        case XTL::X_D3DTOP_ADDSMOOTH:
                            g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLOROP, D3DTOP_ADDSMOOTH);
                            break;
                        case XTL::X_D3DTOP_BLENDDIFFUSEALPHA:
                            g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLOROP, D3DTOP_BLENDDIFFUSEALPHA);
                            break;
                        case XTL::X_D3DTOP_BLENDCURRENTALPHA:
                            g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLOROP, D3DTOP_BLENDCURRENTALPHA);
                            break;
                        case XTL::X_D3DTOP_BLENDTEXTUREALPHA:
                            g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA);
                            break;
                        case XTL::X_D3DTOP_BLENDFACTORALPHA:
                            g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLOROP, D3DTOP_BLENDFACTORALPHA);
                            break;
                        case XTL::X_D3DTOP_BLENDTEXTUREALPHAPM:
                            g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHAPM);
                            break;
                        case XTL::X_D3DTOP_PREMODULATE:
                            g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLOROP, D3DTOP_PREMODULATE);
                            break;
                        case XTL::X_D3DTOP_MODULATEALPHA_ADDCOLOR:
                            g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLOROP, D3DTOP_MODULATEALPHA_ADDCOLOR);
                            break;
                        case XTL::X_D3DTOP_MODULATECOLOR_ADDALPHA:
                            g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLOROP, D3DTOP_MODULATECOLOR_ADDALPHA);
                            break;
                        case XTL::X_D3DTOP_MODULATEINVALPHA_ADDCOLOR:
                            g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLOROP, D3DTOP_MODULATEINVALPHA_ADDCOLOR);
                            break;
                        case XTL::X_D3DTOP_MODULATEINVCOLOR_ADDALPHA:
                            g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLOROP, D3DTOP_MODULATEINVCOLOR_ADDALPHA);
                            break;
                        case XTL::X_D3DTOP_DOTPRODUCT3:
                            g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLOROP, D3DTOP_DOTPRODUCT3);
                            break;
                        case XTL::X_D3DTOP_MULTIPLYADD:
                            g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLOROP, D3DTOP_MULTIPLYADD);
                            break;
                        case XTL::X_D3DTOP_LERP:
                            g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLOROP, D3DTOP_LERP);
                            break;
                        case XTL::X_D3DTOP_BUMPENVMAP:
                            g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLOROP, D3DTOP_BUMPENVMAP);
                            break;
                        case XTL::X_D3DTOP_BUMPENVMAPLUMINANCE:
                            g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLOROP, D3DTOP_BUMPENVMAPLUMINANCE);
                            break;
                        default:
                            EmuLog(LOG_LEVEL::WARNING, "Unsupported D3DTSS_COLOROP Value (%d)", Value);
                            break;
                        }
                    break;
                case XTL::X_D3DTSS_COLORARG0:
                    g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLORARG0, Value);
                    break;
                case XTL::X_D3DTSS_COLORARG1:
                    g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLORARG1, Value);
                    break;
                case XTL::X_D3DTSS_COLORARG2:
                    g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_COLORARG2, Value);
                    break;
                case XTL::X_D3DTSS_ALPHAOP:
                    // TODO: Use a lookup table, this is not always a 1:1 map (same as D3DTSS_COLOROP)
                    if (Value != X_D3DTSS_UNK) {
                        switch (TranslateXDKSpecificD3DTOP(Value)) {
                            case XTL::X_D3DTOP_DISABLE:
                                g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
                                break;
                            case XTL::X_D3DTOP_SELECTARG1:
                                g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
                                break;
                            case XTL::X_D3DTOP_SELECTARG2:
                                g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
                                break;
                            case XTL::X_D3DTOP_MODULATE:
                                g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
                                break;
                            case XTL::X_D3DTOP_MODULATE2X:
                                g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAOP, D3DTOP_MODULATE2X);
                                break;
                            case XTL::X_D3DTOP_MODULATE4X:
                                g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAOP, D3DTOP_MODULATE4X);
                                break;
                            case XTL::X_D3DTOP_ADD:
                                g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAOP, D3DTOP_ADD);
                                break;
                            case XTL::X_D3DTOP_ADDSIGNED:
                                g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAOP, D3DTOP_ADDSIGNED);
                                break;
                            case XTL::X_D3DTOP_ADDSIGNED2X:
                                g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAOP, D3DTOP_ADDSIGNED2X);
                                break;
                            case XTL::X_D3DTOP_SUBTRACT:
                                g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAOP, D3DTOP_SUBTRACT);
                                break;
                            case XTL::X_D3DTOP_ADDSMOOTH:
                                g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAOP, D3DTOP_ADDSMOOTH);
                                break;
                            case XTL::X_D3DTOP_BLENDDIFFUSEALPHA:
                                g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAOP, D3DTOP_BLENDDIFFUSEALPHA);
                                break;
                            case XTL::X_D3DTOP_BLENDTEXTUREALPHA:
                                g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAOP, D3DTOP_BLENDTEXTUREALPHA);
                                break;
                            case XTL::X_D3DTOP_BLENDFACTORALPHA:
                                g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAOP, D3DTOP_BLENDFACTORALPHA);
                                break;
                            case XTL::X_D3DTOP_BLENDTEXTUREALPHAPM:
                                g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAOP, D3DTOP_BLENDTEXTUREALPHAPM);
                                break;
                            case XTL::X_D3DTOP_BLENDCURRENTALPHA:
                                g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAOP, D3DTOP_BLENDCURRENTALPHA);
                                break;
                            case XTL::X_D3DTOP_PREMODULATE:
                                g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAOP, D3DTOP_PREMODULATE);
                                break;
                            case XTL::X_D3DTOP_MODULATEALPHA_ADDCOLOR:
                                g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAOP, D3DTOP_MODULATEALPHA_ADDCOLOR);
                                break;
                            case XTL::X_D3DTOP_MODULATECOLOR_ADDALPHA:
                                g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAOP, D3DTOP_MODULATECOLOR_ADDALPHA);
                                break;
                            case XTL::X_D3DTOP_MODULATEINVALPHA_ADDCOLOR:
                                g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAOP, D3DTOP_MODULATEINVALPHA_ADDCOLOR);
                                break;
                            case XTL::X_D3DTOP_MODULATEINVCOLOR_ADDALPHA:
                                g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAOP, D3DTOP_MODULATEINVCOLOR_ADDALPHA);
                                break;
                            case XTL::X_D3DTOP_DOTPRODUCT3:
                                g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAOP, D3DTOP_DOTPRODUCT3);
                                break;
                            case XTL::X_D3DTOP_MULTIPLYADD:
                                g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAOP, D3DTOP_MULTIPLYADD);
                                break;
                            case XTL::X_D3DTOP_LERP:
                                g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAOP, D3DTOP_LERP);
                                break;
                            case XTL::X_D3DTOP_BUMPENVMAP:
                                g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAOP, D3DTOP_BUMPENVMAP);
                                break;
                            case XTL::X_D3DTOP_BUMPENVMAPLUMINANCE:
                                g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAOP, D3DTOP_BUMPENVMAPLUMINANCE);
                                break;
                            default:
                                EmuLog(LOG_LEVEL::WARNING, "Unsupported D3DTSS_ALPHAOP Value (%d)", Value);
                                break;
                      }
                    }
                    break;
                case XTL::X_D3DTSS_ALPHAARG0:
                    g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAARG0, Value);
                    break;
                case XTL::X_D3DTSS_ALPHAARG1:
                    g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAARG1, Value);
                    break;
                case XTL::X_D3DTSS_ALPHAARG2:
                    g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_ALPHAARG2, Value);
                    break;
                case XTL::X_D3DTSS_RESULTARG:
                    g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_RESULTARG, Value);
                    break;
                case XTL::X_D3DTSS_TEXTURETRANSFORMFLAGS:
                    g_pD3DDevice->SetTextureStageState(HostStage, D3DTSS_TEXTURETRANSFORMFLAGS, Value);
                    break;
                default:
                    EmuLog(LOG_LEVEL::WARNING, "Unkown Xbox D3DTSS Value: %d", XboxState);
                    break;
            }
        }

        // Make sure we only do this once
        if (pointSpriteOverride && XboxStage == 3) {
            pointSpriteOverride = false;
            XboxStage--;
        }
    }

    if (EmuD3DDeferredRenderState[XTL::X_D3DRS_POINTSPRITEENABLE - XTL::X_D3DRS_FOGENABLE] == TRUE) {
        IDirect3DBaseTexture *pTexture;

        // set the point sprites texture
        g_pD3DDevice->GetTexture(3, &pTexture);
        g_pD3DDevice->SetTexture(0, pTexture);

        // disable all other stages
        g_pD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
        g_pD3DDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

        // no need to actually copy here, since it was handled in the loop above
    }
}

// ******************************************************************
// * patch: UpdateDeferredStates
// ******************************************************************
void EmuUpdateDeferredStates()
{
    VerifyAndFixEmuDeferredRenderStateOffset();
    UpdateDeferredRenderStates();
    UpdateDeferredTextureStates();
}
