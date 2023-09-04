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


#include "RenderStates.h"
#include "Logging.h"
#include "core/hle/D3D8/Direct3D9/Direct3D9.h" // For g_pD3DDevice
#include "core/hle/D3D8/XbConvert.h"

void SetXboxMultiSampleType(xbox::X_D3DMULTISAMPLE_TYPE value);

bool XboxRenderStateConverter::Init()
{
    // Get render state
    if (g_SymbolAddresses.find("D3D_g_RenderState") != g_SymbolAddresses.end()) {
        D3D__RenderState = (uint32_t*)g_SymbolAddresses["D3D_g_RenderState"];
    }
    else {
        EmuLog(LOG_LEVEL::ERROR2, "D3D_g_RenderState was not found!");
        return false;
    }

    // Build a mapping of Cxbx Render State indexes to indexes within the current XDK
    BuildRenderStateMappingTable();

    // Set Initial Values
    StoreInitialValues();

    return true;
}


bool XboxRenderStateConverter::InitWithNV2A()
{
    // allocate 
    D3D__RenderState =(uint32_t*) malloc(sizeof(DWORD)*(xbox::X_D3DRS_LAST + 1));//(uint32_t*)g_SymbolAddresses["D3D_g_RenderState"];
    // Build a mapping of Cxbx Render State indexes to indexes within the current XDK
    BuildRenderStateMappingTable();

    // Set Initial Values
    StoreInitialValues();

    return true;
}

bool IsRenderStateAvailableInCurrentXboxD3D8Lib(RenderStateInfo& aRenderStateInfo)
{
	bool bIsRenderStateAvailable = (aRenderStateInfo.V <= g_LibVersion_D3D8);
	if (aRenderStateInfo.R > 0) { // Applies to xbox::X_D3DRS_MULTISAMPLETYPE
		// Note : X_D3DRS_MULTISAMPLETYPE seems the only render state that got
		// removed (from 4039 onwards), so we check that limitation here as well
		bIsRenderStateAvailable &= (g_LibVersion_D3D8 < aRenderStateInfo.R);
	}
	return bIsRenderStateAvailable;
}

void XboxRenderStateConverter::BuildRenderStateMappingTable()
{
    EmuLog(LOG_LEVEL::INFO, "Building Cxbx to XDK Render State Mapping Table");

    XboxRenderStateOffsets.fill(-1);

    int XboxIndex = 0;
    for (unsigned int RenderState = xbox::X_D3DRS_FIRST; RenderState <= xbox::X_D3DRS_LAST; RenderState++) {
        auto RenderStateInfo = GetDxbxRenderStateInfo(RenderState);
        if (IsRenderStateAvailableInCurrentXboxD3D8Lib(RenderStateInfo)) {
            XboxRenderStateOffsets[RenderState] = XboxIndex;
            EmuLog(LOG_LEVEL::INFO, "%s = %d", RenderStateInfo.S, XboxIndex);
            XboxIndex++;
            continue;
        }

        EmuLog(LOG_LEVEL::INFO, "%s Not Present", RenderStateInfo.S);
    }
}

void XboxRenderStateConverter::SetDirty()
{
    PreviousRenderStateValues.fill(-1);
}

void* XboxRenderStateConverter::GetPixelShaderRenderStatePointer()
{
    return &D3D__RenderState[xbox::X_D3DRS_PS_FIRST];
}

bool XboxRenderStateConverter::XboxRenderStateExists(uint32_t State)
{
    if (XboxRenderStateOffsets[State] >= 0) {
        return true;
    }

    return false;
}

bool XboxRenderStateConverter::XboxRenderStateValueChanged(uint32_t State)
{
    if (XboxRenderStateExists(State) && GetXboxRenderState(State) != PreviousRenderStateValues[State]) {
        return true;
    }

    return false;
}

void XboxRenderStateConverter::SetXboxRenderState(uint32_t State, uint32_t Value)
{
    if (!XboxRenderStateExists(State)) {
        EmuLog(LOG_LEVEL::WARNING, "Attempt to write a Renderstate (%s) that does not exist in the current D3D8 XDK Version (%d)", GetDxbxRenderStateInfo(State).S, g_LibVersion_D3D8);
        return;
    }

    D3D__RenderState[XboxRenderStateOffsets[State]] = Value;
}

uint32_t XboxRenderStateConverter::GetXboxRenderState(uint32_t State)
{
    if (!XboxRenderStateExists(State)) {
        EmuLog(LOG_LEVEL::WARNING, "Attempt to read a Renderstate (%s) that does not exist in the current D3D8 XDK Version (%d)", GetDxbxRenderStateInfo(State).S, g_LibVersion_D3D8);
        return 0;
    }

    return D3D__RenderState[XboxRenderStateOffsets[State]];
}

float XboxRenderStateConverter::GetXboxRenderStateAsFloat(uint32_t State)
{
    if (!XboxRenderStateExists(State)) {
        EmuLog(LOG_LEVEL::WARNING, "Attempt to read a Renderstate (%s) that does not exist in the current D3D8 XDK Version (%d)", GetDxbxRenderStateInfo(State).S, g_LibVersion_D3D8);
        return 0;
    }

    return *reinterpret_cast<float*>(&(D3D__RenderState[XboxRenderStateOffsets[State]]));
}
// copy whole internal buffer to buffer via argument
void XboxRenderStateConverter::CopyTo(uint32_t* ptarget) {
    // Read the value of the current stage/state from the Xbox data structure
    memcpy(ptarget, D3D__RenderState, sizeof(DWORD) * (xbox::X_D3DTSS_LAST + 1));

    return;
}
// return internal buffer pointer
uint32_t* XboxRenderStateConverter::Buffer(void) {

    // Read the value of the current stage/state from the Xbox data structure
    return D3D__RenderState;
}

void XboxRenderStateConverter::StoreInitialValues()
{
    for (unsigned int RenderState = xbox::X_D3DRS_FIRST; RenderState <= xbox::X_D3DRS_LAST; RenderState++) {
        // Skip Render States that don't exist within this XDK
        if (!XboxRenderStateExists(RenderState)) {
            continue;
        }

        PreviousRenderStateValues[RenderState] = GetXboxRenderState(RenderState);
    }
}

void XboxRenderStateConverter::SetWireFrameMode(int wireframe)
{
    WireFrameMode = wireframe;

    // Wireframe mode changed, so we must force the Fill Mode renderstate to dirty
    // At next call to Apply, the desired WireFrame mode will be set
    PreviousRenderStateValues[xbox::X_D3DRS_FILLMODE] = -1;
}

void XboxRenderStateConverter::Apply()
{
    // Iterate through each RenderState and set the associated host render state
    // We start counting at X_D3DRS_SIMPLE_FIRST, to skip the pixel shader renderstates handled elsewhere
    for (unsigned int RenderState = xbox::X_D3DRS_SIMPLE_FIRST; RenderState <= xbox::X_D3DRS_LAST; RenderState++) {
        // Skip any renderstate that does not exist in the current XDK, or have not changed since the previous update call
        // Also skip PSTextureModes, which is a special case used by Pixel Shaders
        if (!XboxRenderStateExists(RenderState) || !XboxRenderStateValueChanged(RenderState) || RenderState == xbox::X_D3DRS_PSTEXTUREMODES) {
            continue;
        }

        auto Value = GetXboxRenderState(RenderState);
        EmuLog(LOG_LEVEL::DEBUG, "XboxRenderStateConverter::Apply(%s, %X)\n", GetDxbxRenderStateInfo(RenderState).S, Value);

        if (RenderState <= xbox::X_D3DRS_SIMPLE_LAST) {
            ApplySimpleRenderState(RenderState, Value);
        } else if (RenderState <= xbox::X_D3DRS_DEFERRED_LAST) {
            ApplyDeferredRenderState(RenderState, Value);
        } else if (RenderState <= xbox::X_D3DRS_COMPLEX_LAST) {
            ApplyComplexRenderState(RenderState, Value);
        }

        PreviousRenderStateValues[RenderState] = Value;
    }
}

void XboxRenderStateConverter::ApplySimpleRenderState(uint32_t State, uint32_t Value)
{
	auto RenderStateInfo = GetDxbxRenderStateInfo(State);

    switch (State) {
        case xbox::X_D3DRS_COLORWRITEENABLE: {
            DWORD OrigValue = Value;
            Value = 0;

            if (OrigValue & (1L << 16)) {
                Value |= D3DCOLORWRITEENABLE_RED;
            }

            if (OrigValue & (1L << 8)) {
                Value |= D3DCOLORWRITEENABLE_GREEN;
            }

            if (OrigValue & (1L << 0)) {
                Value |= D3DCOLORWRITEENABLE_BLUE;
            }

            if (OrigValue & (1L << 24)) {
                Value |= D3DCOLORWRITEENABLE_ALPHA;
            }
        } break;
        case xbox::X_D3DRS_SHADEMODE:
            Value = EmuXB2PC_D3DSHADEMODE(Value);
            break;
        case xbox::X_D3DRS_BLENDOP:
            Value = EmuXB2PC_D3DBLENDOP(Value);
            break;
        case xbox::X_D3DRS_SRCBLEND:
        case xbox::X_D3DRS_DESTBLEND:
            Value = EmuXB2PC_D3DBLEND(Value);
            break;
        case xbox::X_D3DRS_ZFUNC:
        case xbox::X_D3DRS_ALPHAFUNC:
        case xbox::X_D3DRS_STENCILFUNC:
            Value = EmuXB2PC_D3DCMPFUNC(Value);
            break;
        case xbox::X_D3DRS_STENCILZFAIL:
        case xbox::X_D3DRS_STENCILPASS:
            Value = EmuXB2PC_D3DSTENCILOP(Value);
            break;
        case xbox::X_D3DRS_ALPHATESTENABLE:
        case xbox::X_D3DRS_ALPHABLENDENABLE:
        case xbox::X_D3DRS_BLENDCOLOR:
        case xbox::X_D3DRS_ALPHAREF: case xbox::X_D3DRS_ZWRITEENABLE:
        case xbox::X_D3DRS_DITHERENABLE: case xbox::X_D3DRS_STENCILREF:
        case xbox::X_D3DRS_STENCILMASK: case xbox::X_D3DRS_STENCILWRITEMASK:
            // These render states require no conversion, so we simply
            // allow SetRenderState to be called with no changes
            break;
        default:
            // Only log missing state if it has a PC counterpart
            if (RenderStateInfo.PC != 0) {
                EmuLog(LOG_LEVEL::WARNING, "ApplySimpleRenderState(%s, 0x%.08X) is unimplemented!", RenderStateInfo.S, Value);
            }
            return;
    }

    // Skip RenderStates that don't have a defined PC counterpart
    if (RenderStateInfo.PC == 0) {
        return;
    }

    g_pD3DDevice->SetRenderState((D3DRENDERSTATETYPE)(RenderStateInfo.PC), Value);
}

void XboxRenderStateConverter::ApplyDeferredRenderState(uint32_t State, uint32_t Value)
{
	auto RenderStateInfo = GetDxbxRenderStateInfo(State);

	// Convert from Xbox Data Formats to PC
    switch (State) {
        case xbox::X_D3DRS_FOGSTART:
        case xbox::X_D3DRS_FOGEND: {
            // HACK: If the fog start/fog-end are negative, make them positive
            // This fixes Smashing Drive on non-nvidia hardware
            // Cause appears to be non-nvidia drivers clamping values < 0 to 0
            // Resulting in the fog formula becoming (0 - d) / 0, which breaks rendering
            // This prevents that scenario for screen-space fog, *hopefully* without breaking eye-based fog also
            float fogValue = *(float*)& Value;
            if (fogValue < 0.0f) {
                LOG_TEST_CASE("FOGSTART/FOGEND below 0");
                fogValue = std::abs(fogValue);
                Value = *(DWORD*)& fogValue;
            }
        } break;
        case xbox::X_D3DRS_FOGENABLE:
        case xbox::X_D3DRS_FOGTABLEMODE:
        case xbox::X_D3DRS_FOGDENSITY:
        case xbox::X_D3DRS_RANGEFOGENABLE:
        case xbox::X_D3DRS_LIGHTING:
        case xbox::X_D3DRS_SPECULARENABLE:
        case xbox::X_D3DRS_LOCALVIEWER:
        case xbox::X_D3DRS_COLORVERTEX:
        case xbox::X_D3DRS_SPECULARMATERIALSOURCE:
        case xbox::X_D3DRS_DIFFUSEMATERIALSOURCE:
        case xbox::X_D3DRS_AMBIENTMATERIALSOURCE:
        case xbox::X_D3DRS_EMISSIVEMATERIALSOURCE:
        case xbox::X_D3DRS_AMBIENT:
        case xbox::X_D3DRS_POINTSIZE:
        case xbox::X_D3DRS_POINTSIZE_MIN:
        case xbox::X_D3DRS_POINTSPRITEENABLE:
        case xbox::X_D3DRS_POINTSCALEENABLE:
        case xbox::X_D3DRS_POINTSCALE_A:
        case xbox::X_D3DRS_POINTSCALE_B:
        case xbox::X_D3DRS_POINTSCALE_C:
        case xbox::X_D3DRS_POINTSIZE_MAX:
        case xbox::X_D3DRS_PATCHEDGESTYLE:
        case xbox::X_D3DRS_PATCHSEGMENTS:
            // These render states require no conversion, so we can use them as-is
            break;
        case xbox::X_D3DRS_BACKSPECULARMATERIALSOURCE:
        case xbox::X_D3DRS_BACKDIFFUSEMATERIALSOURCE:
        case xbox::X_D3DRS_BACKAMBIENTMATERIALSOURCE:
        case xbox::X_D3DRS_BACKEMISSIVEMATERIALSOURCE:
        case xbox::X_D3DRS_BACKAMBIENT:
        case xbox::X_D3DRS_SWAPFILTER:
            // These states are unsupported by the host and are ignored (for now)
            return;
        case xbox::X_D3DRS_PRESENTATIONINTERVAL: {
            // Store this as an override for our frame limiter
            // Games can use this to limit certain scenes to a desired target framerate for a specific scene
            // If this value is not set, or is set to 0, the default interval passed to CreateDevice is used
            extern xbox::dword_xt g_Xbox_PresentationInterval_Override;
            g_Xbox_PresentationInterval_Override = Value;
        } return;
        case xbox::X_D3DRS_WRAP0:
        case xbox::X_D3DRS_WRAP1:
        case xbox::X_D3DRS_WRAP2:
        case xbox::X_D3DRS_WRAP3: {
            DWORD OldValue = Value;
            Value = 0;

            Value |= (OldValue & 0x00000010) ? D3DWRAPCOORD_0 : 0;
            Value |= (OldValue & 0x00001000) ? D3DWRAPCOORD_1 : 0;
            Value |= (OldValue & 0x00100000) ? D3DWRAPCOORD_2 : 0;
            Value |= (OldValue & 0x01000000) ? D3DWRAPCOORD_3 : 0;
        } break;
        default:
            // Only log missing state if it has a PC counterpart
            if (RenderStateInfo.PC != 0) {
                EmuLog(LOG_LEVEL::WARNING, "ApplyDeferredRenderState(%s, 0x%.08X) is unimplemented!", RenderStateInfo.S, Value);
            }
            return;
    }

    // Skip RenderStates that don't have a defined PC counterpart
    if (RenderStateInfo.PC == 0) {
        return;
    }

    g_pD3DDevice->SetRenderState(RenderStateInfo.PC, Value);
}

void XboxRenderStateConverter::ApplyComplexRenderState(uint32_t State, uint32_t Value)
{
	auto RenderStateInfo = GetDxbxRenderStateInfo(State);

	switch (State) {
        case xbox::X_D3DRS_VERTEXBLEND:
            // convert from Xbox X_D3DVERTEXBLENDFLAGS to PC D3DVERTEXBLENDFLAGS enumeration
            switch (Value) {
            case xbox::X_D3DVBF_DISABLE: Value = D3DVBF_DISABLE; break;
            case xbox::X_D3DVBF_1WEIGHTS: Value = D3DVBF_1WEIGHTS; break;
            case xbox::X_D3DVBF_2WEIGHTS: Value = D3DVBF_2WEIGHTS; break;
            case xbox::X_D3DVBF_3WEIGHTS: Value = D3DVBF_3WEIGHTS; break;
            default:
                LOG_TEST_CASE("Unsupported D3DVERTEXBLENDFLAGS (%d)");
                return;
            }
            break;
        case xbox::X_D3DRS_FILLMODE:
            Value = EmuXB2PC_D3DFILLMODE(Value);

            if (WireFrameMode > 0) {
                if (WireFrameMode == 1) {
                    Value = D3DFILL_WIREFRAME;
                } else {
                    Value = D3DFILL_POINT;
                }
            }
            break;
        case xbox::X_D3DRS_CULLMODE:
            switch (Value) {
                case xbox::X_D3DCULL_NONE: Value = D3DCULL_NONE; break;
                case xbox::X_D3DCULL_CW: Value = D3DCULL_CW; break;
                case xbox::X_D3DCULL_CCW: Value = D3DCULL_CCW;break;
                default: LOG_TEST_CASE("EmuD3DDevice_SetRenderState_CullMode: Unknown Cullmode");
            }
            break;
        case xbox::X_D3DRS_ZBIAS: {
            FLOAT Biased = static_cast<FLOAT>(Value) * -0.000005f;
            Value = *reinterpret_cast<const DWORD*>(&Biased);
        } break;
        // These states require no conversions, so can just be passed through to the host directly
        case xbox::X_D3DRS_FOGCOLOR:
        case xbox::X_D3DRS_NORMALIZENORMALS:
        case xbox::X_D3DRS_ZENABLE:
        case xbox::X_D3DRS_STENCILENABLE:
        case xbox::X_D3DRS_TEXTUREFACTOR:
        case xbox::X_D3DRS_EDGEANTIALIAS:
        case xbox::X_D3DRS_MULTISAMPLEANTIALIAS:
        case xbox::X_D3DRS_MULTISAMPLEMASK:
            break;
        case xbox::X_D3DRS_STENCILFAIL:
            Value = EmuXB2PC_D3DSTENCILOP(Value);
            break;
        case xbox::X_D3DRS_MULTISAMPLETYPE://D3DRS_MULTISAMPLEMODE
            SetXboxMultiSampleType(Value);
            break;
        default:
            // Only log missing state if it has a PC counterpart
            if (RenderStateInfo.PC != 0) {
                EmuLog(LOG_LEVEL::WARNING, "ApplyComplexRenderState(%s, 0x%.08X) is unimplemented!", RenderStateInfo.S, Value);
            }
            return;
    }

    // Skip RenderStates that don't have a defined PC counterpart
    if (RenderStateInfo.PC == 0) {
        return;
    }

    g_pD3DDevice->SetRenderState(RenderStateInfo.PC, Value);
}
