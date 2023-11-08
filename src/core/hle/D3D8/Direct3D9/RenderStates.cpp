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
#define F_ONE 0x3f800000
#define F_64 0x42800000
typedef enum _X_D3DCMPFUNC {
    X_D3DCMP_NEVER = 0x200,
    X_D3DCMP_LESS = 0x201,
    X_D3DCMP_EQUAL = 0x202,
    X_D3DCMP_LESSEQUAL = 0x203,
    X_D3DCMP_GREATER = 0x204,
    X_D3DCMP_NOTEQUAL = 0x205,
    X_D3DCMP_GREATEREQUAL = 0x206,
    X_D3DCMP_ALWAYS = 0x207,
    X_D3DCMP_FORCE_DWORD = 0x7fffffff,
} X_D3DCMPFUNC;

typedef enum _X_D3DBLEND {
    X_D3DBLEND_ZERO = 0,
    X_D3DBLEND_ONE = 1,
    X_D3DBLEND_SRCCOLOR = 0x300,
    X_D3DBLEND_INVSRCCOLOR = 0x301,
    X_D3DBLEND_SRCALPHA = 0x302,
    X_D3DBLEND_INVSRCALPHA = 0x303,
    X_D3DBLEND_DESTALPHA = 0x304,
    X_D3DBLEND_INVDESTALPHA = 0x305,
    X_D3DBLEND_DESTCOLOR = 0x306,
    X_D3DBLEND_INVDESTCOLOR = 0x307,
    X_D3DBLEND_SRCALPHASAT = 0x308,
    X_D3DBLEND_CONSTANTCOLOR = 0x8001,
    X_D3DBLEND_INVCONSTANTCOLOR = 0x8002,
    X_D3DBLEND_CONSTANTALPHA = 0x8003,
    X_D3DBLEND_INVCONSTANTALPHA = 0x8004,
    X_D3DBLEND_FORCE_DWORD = 0x7fffffff,
} X_D3DBLEND;

typedef enum _X_D3DSHADEMODE {
    X_D3DSHADE_FLAT = 0x1d00,
    X_D3DSHADE_GOURAUD = 0x1d01,
    X_D3DSHADE_FORCE_DWORD = 0x7fffffff, /* force 32-bit size enum */
} X_D3DSHADEMODE;

typedef enum _X_D3DSTENCILOP {
    X_D3DSTENCILOP_KEEP = 0x1e00,
    X_D3DSTENCILOP_ZERO = 0,
    X_D3DSTENCILOP_REPLACE = 0x1e01,
    X_D3DSTENCILOP_INCRSAT = 0x1e02,
    X_D3DSTENCILOP_DECRSAT = 0x1e03,
    X_D3DSTENCILOP_INVERT = 0x150a,
    X_D3DSTENCILOP_INCR = 0x8507,
    X_D3DSTENCILOP_DECR = 0x8508,
    X_D3DSTENCILOP_FORCE_DWORD = 0x7fffffff,
} X_D3DSTENCILOP;

typedef enum _X_D3DSWATHWIDTH {
    X_D3DSWATH_8 = 0,
    X_D3DSWATH_16 = 1,
    X_D3DSWATH_32 = 2,
    X_D3DSWATH_64 = 3,
    X_D3DSWATH_128 = 4,
    X_D3DSWATH_OFF = 0xf,
    X_D3DSWATH_FORCE_DWORD = 0x7fffffff, /* force 32-bit size enum */
} X_D3DSWATHWIDTH;

typedef enum _X_D3DBLENDOP {
    X_D3DBLENDOP_ADD = 0x8006,
    X_D3DBLENDOP_SUBTRACT = 0x800a,
    X_D3DBLENDOP_REVSUBTRACT = 0x800b,
    X_D3DBLENDOP_MIN = 0x8007,
    X_D3DBLENDOP_MAX = 0x8008,
    X_D3DBLENDOP_ADDSIGNED = 0xf006,     // Xbox extension
    X_D3DBLENDOP_REVSUBTRACTSIGNED = 0xf005,     // Xbox extension
    X_D3DBLENDOP_FORCE_DWORD = 0x7fffffff, /* force 32-bit size enum */
} X_D3DBLENDOP;

typedef enum _X_D3DFOGMODE {
    X_D3DFOG_NONE = 0,
    X_D3DFOG_EXP = 1,
    X_D3DFOG_EXP2 = 2,
    X_D3DFOG_LINEAR = 3,

    X_D3DFOG_FORCE_DWORD = 0x7fffffff
} X_D3DFOGMODE;

typedef enum _X_D3DMATERIALCOLORSOURCE {
    X_D3DMCS_MATERIAL = 0,
    X_D3DMCS_COLOR1 = 1,
    X_D3DMCS_COLOR2 = 2,

    X_D3DMCS_FORCE_DWORD = 0x7fffffff
} X_D3DMATERIALCOLORSOURCE;

typedef enum _X_D3DPATCHEDGESTYLE {
    X_D3DPATCHEDGE_DISCRETE = 0,
    X_D3DPATCHEDGE_CONTINUOUS = 1,

    X_D3DPATCHEDGE_FORCE_DWORD = 0x7fffffff,
} X_D3DPATCHEDGESTYLE;

typedef enum _X_D3DVERTEXBLENDFLAGS {
    X_D3DVBF_DISABLE = 0,     // Disable vertex blending
    X_D3DVBF_1WEIGHTS = 1,     // 2 matrix blending
    X_D3DVBF_2WEIGHTS = 3,     // 3 matrix blending
    X_D3DVBF_3WEIGHTS = 5,     // 4 matrix blending
    X_D3DVBF_2WEIGHTS2MATRICES = 2,     // Xbox extension
    X_D3DVBF_3WEIGHTS3MATRICES = 4,     // Xbox extension
    X_D3DVBF_4WEIGHTS4MATRICES = 6,     // Xbox extension

    X_D3DVBF_MAX = 7,
    X_D3DVBF_FORCE_DWORD = 0x7fffffff, // force 32-bit size enum

} X_D3DVERTEXBLENDFLAGS;

typedef enum _X_D3DFILLMODE {
    X_D3DFILL_POINT = 0x1b00,
    X_D3DFILL_WIREFRAME = 0x1b01,
    X_D3DFILL_SOLID = 0x1b02,
    X_D3DFILL_FORCE_DWORD = 0x7fffffff,
} X_D3DFILLMODE;

typedef enum _X_D3DFRONT {        // Xbox extension
    X_D3DFRONT_CW = 0x900,
    X_D3DFRONT_CCW = 0x901,
    X_D3DFRONT_FORCE_DWORD = 0x7fffffff, /* force 32-bit size enum */
} X_D3DFRONT;

typedef enum _X_D3DCULL {
    X_D3DCULL_NONE = 0,
    X_D3DCULL_CW = 0x900,
    X_D3DCULL_CCW = 0x901,
    X_D3DCULL_FORCE_DWORD = 0x7fffffff,
}X_D3DCULL;

typedef enum _X_D3DLOGICOP {   // Xbox extension
    X_D3DLOGICOP_NONE = 0,
    X_D3DLOGICOP_CLEAR = 0x1500,
    X_D3DLOGICOP_AND = 0x1501,
    X_D3DLOGICOP_AND_REVERSE = 0x1502,
    X_D3DLOGICOP_COPY = 0x1503,
    X_D3DLOGICOP_AND_INVERTED = 0x1504,
    X_D3DLOGICOP_NOOP = 0x1505,
    X_D3DLOGICOP_XOR = 0x1506,
    X_D3DLOGICOP_OR = 0x1507,
    X_D3DLOGICOP_NOR = 0x1508,
    X_D3DLOGICOP_EQUIV = 0x1509,
    X_D3DLOGICOP_INVERT = 0x150a,
    X_D3DLOGICOP_OR_REVERSE = 0x150b,
    X_D3DLOGICOP_COPY_INVERTED = 0x150c,
    X_D3DLOGICOP_OR_INVERTED = 0x150d,
    X_D3DLOGICOP_NAND = 0x150e,
    X_D3DLOGICOP_SET = 0x150f,
    X_D3DLOGICOP_FORCE_DWORD = 0x7fffffff, /* force 32-bit size enum */
} X_D3DLOGICOP;

typedef enum _X_D3DMULTISAMPLEMODE
{
    X_D3DMULTISAMPLEMODE_1X = 0,
    X_D3DMULTISAMPLEMODE_2X = 1,
    X_D3DMULTISAMPLEMODE_4X = 2,

    X_D3DMULTISAMPLEMODE_FORCE_DWORD = 0x7fffffff
} X_D3DMULTISAMPLETYPE;


CONST UINT32 g_DefaultRenderStates[][2] =
{
{     xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_ZFUNC            ,      _X_D3DCMPFUNC::X_D3DCMP_LESSEQUAL },
{              xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_ALPHAFUNC                 ,      X_D3DCMP_ALWAYS },
{                      xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_ALPHABLENDENABLE          ,      FALSE },
{                      xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_ALPHATESTENABLE           ,      FALSE },
{                          xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_ALPHAREF                  ,      0 },
{               xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_SRCBLEND                  ,      X_D3DBLEND_ONE },
{              xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_DESTBLEND                 ,      X_D3DBLEND_ZERO },
{                       xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_ZWRITEENABLE              ,      TRUE },
{                      xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_DITHERENABLE              ,      FALSE },
{           xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_SHADEMODE                 ,      X_D3DSHADE_GOURAUD },
{    xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_COLORWRITEENABLE          ,      xbox::X_D3DCOLORWRITEENABLE_ALL },
{          xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_STENCILZFAIL              ,      X_D3DSTENCILOP_KEEP },
{          xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_STENCILPASS               ,      X_D3DSTENCILOP_KEEP },
{              xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_STENCILFUNC               ,      X_D3DCMP_ALWAYS },
{                          xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_STENCILREF                ,      0 },
{                 xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_STENCILMASK               ,      0xffffffff },
{                 xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_STENCILWRITEMASK          ,      0xffffffff },
{             xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_BLENDOP                   ,      X_D3DBLENDOP_ADD },
{                          xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_BLENDCOLOR                ,      0 },
{               xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_SWATHWIDTH                ,      X_D3DSWATH_128 },
{                          xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_POLYGONOFFSETZSLOPESCALE  ,      0 },
{                          xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_POLYGONOFFSETZOFFSET      ,      0 },
{                      xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_POINTOFFSETENABLE         ,      FALSE },
{                      xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_WIREFRAMEOFFSETENABLE     ,      FALSE },
{                      xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_SOLIDOFFSETENABLE         ,      FALSE },
{                      xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FOGENABLE                 ,      FALSE },
{                xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FOGTABLEMODE              ,      X_D3DFOG_NONE },
{                          xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FOGSTART                  ,      0 },
{                      xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FOGEND                    ,      F_ONE },
{                      xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FOGDENSITY                ,      F_ONE },
{                      xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_RANGEFOGENABLE            ,      FALSE },
{                          xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_WRAP0                     ,      0 },
{                          xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_WRAP1                     ,      0 },
{                          xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_WRAP2                     ,      0 },
{                          xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_WRAP3                     ,      0 },
{                       xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_LIGHTING                  ,      TRUE },
{                      xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_SPECULARENABLE            ,      FALSE },
{                       xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_LOCALVIEWER               ,      TRUE },
{                       xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_COLORVERTEX               ,      TRUE },
{              xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_BACKSPECULARMATERIALSOURCE,      X_D3DMCS_COLOR2 },
{              xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_BACKDIFFUSEMATERIALSOURCE ,      X_D3DMCS_COLOR1 },
{            xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_BACKAMBIENTMATERIALSOURCE ,      X_D3DMCS_MATERIAL },
{            xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_BACKEMISSIVEMATERIALSOURCE,      X_D3DMCS_MATERIAL },
{              xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_SPECULARMATERIALSOURCE    ,      X_D3DMCS_COLOR2 },
{              xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_DIFFUSEMATERIALSOURCE     ,      X_D3DMCS_COLOR1 },
{            xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_AMBIENTMATERIALSOURCE     ,      X_D3DMCS_MATERIAL },
{            xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_EMISSIVEMATERIALSOURCE    ,      X_D3DMCS_MATERIAL },
{                          xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_BACKAMBIENT               ,      0 },
{                          xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_AMBIENT                   ,      0 },
{                      xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_POINTSIZE                 ,      F_ONE },
{                          xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_POINTSIZE_MIN             ,      0 },
{                          xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_POINTSPRITEENABLE         ,      0 },
{                          xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_POINTSCALEENABLE          ,      0 },
{                      xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_POINTSCALE_A              ,      F_ONE },
{                          xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_POINTSCALE_B              ,      0 },
{                          xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_POINTSCALE_C              ,      0 },
{                       xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_POINTSIZE_MAX             ,      F_64 },
{      xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_PATCHEDGESTYLE            ,      X_D3DPATCHEDGE_DISCRETE },
{                      xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_PATCHSEGMENTS             ,      F_ONE },
{                 xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_SWAPFILTER,      0xdeadbeef },
{                          xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_PSTEXTUREMODES            ,      0 },
{             xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_VERTEXBLEND               ,      X_D3DVBF_DISABLE },
{                          xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FOGCOLOR                  ,      0 },
{              xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FILLMODE                  ,      X_D3DFILL_SOLID },
{              xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_BACKFILLMODE              ,      X_D3DFILL_SOLID },
{                      xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_TWOSIDEDLIGHTING          ,      FALSE },
{                      xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_NORMALIZENORMALS          ,      FALSE },
{                 xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_ZENABLE,      D3DZB_USEW },
{                      xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_STENCILENABLE             ,      FALSE },
{          xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_STENCILFAIL               ,      X_D3DSTENCILOP_KEEP },
{                xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_FRONTFACE                 ,      X_D3DFRONT_CW },
{                xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_CULLMODE                  ,      X_D3DCULL_CCW },
{                 xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_TEXTUREFACTOR             ,      0xffffffff },
{                          xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_ZBIAS                     ,      0 },
{            xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_LOGICOP                   ,      X_D3DLOGICOP_NONE },
{                      xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_EDGEANTIALIAS             ,      FALSE },
{                       xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_MULTISAMPLEANTIALIAS      ,      TRUE },
{                 xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_MULTISAMPLEMASK           ,      0xffffffff },
{                 xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_MULTISAMPLEMODE,      0xdeadbeef },
{      xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_MULTISAMPLERENDERTARGETMODE,      X_D3DMULTISAMPLEMODE_1X },
{               xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_SHADOWFUNC                ,      D3DCMP_NEVER },
{                      xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_LINEWIDTH                 ,      F_ONE },
{                       xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_DXT1NOISEENABLE,      TRUE },
{                      xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_YUVENABLE,      FALSE },
{                       xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_OCCLUSIONCULLENABLE,      TRUE },
{                       xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_STENCILCULLENABLE,      TRUE },
{                      xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_ROPZCMPALWAYSREAD,      FALSE },
{                      xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_ROPZREAD,      FALSE },
{                      xbox::_X_D3DRENDERSTATETYPE::X_D3DRS_DONOTCULLUNCOMPRESSED,      FALSE },
// use 0xFFFFFFFF for both elements to indicate the end of array.
{  0xFFFFFFFF,0xFFFFFFFF }
};

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
    assert(D3D__RenderState != nullptr);
    memset(D3D__RenderState, 0, sizeof(DWORD) * (xbox::X_D3DRS_LAST + 1));
    // Build a mapping of Cxbx Render State indexes to indexes within the current XDK
    BuildRenderStateMappingTable();
    // steup default xbox render state values.
    // another way is to use XboxRenderStages.GetXboxRenderState() to get xbox values and set to NV2ARenderStages
    int i = 0;
    while (g_DefaultRenderStates[i][0]!=0xFFFFFFFF) {
        unsigned int RenderState = g_DefaultRenderStates[i][0];
        unsigned int value = g_DefaultRenderStates [i][1];
        // Skip Render States that don't exist within this XDK
        if (!XboxRenderStateExists(RenderState)) {
            i++;
            continue;
        }
        SetXboxRenderState(RenderState,value);
        i++;
    }

    // StoreInitialValues() stores the initial value to a cached old value array for Apply() to check whether the value was changed or not.
    // this is not what we need in LLE. we don't store the chached old value in init stage so the first time Apply() was called, all state will be set to host.
    // StoreInitialValues();

    return true;
}

bool IsRenderStateAvailableInCurrentXboxD3D8Lib(const RenderStateInfo& aRenderStateInfo)
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
        const RenderStateInfo& RenderStateInfo = GetDxbxRenderStateInfo(RenderState);
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
void XboxRenderStateConverter::CopyTo(uint32_t* ptarget)
{
    // Read the value of the current stage/state from the Xbox data structure
    memcpy(ptarget, D3D__RenderState, sizeof(DWORD) * (xbox::X_D3DTSS_LAST + 1));

    return;
}

// return internal buffer pointer
uint32_t* XboxRenderStateConverter::Buffer(void)
{

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
    const RenderStateInfo& RenderStateInfo = GetDxbxRenderStateInfo(State);

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
	const RenderStateInfo& RenderStateInfo = GetDxbxRenderStateInfo(State);

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
    const RenderStateInfo& RenderStateInfo = GetDxbxRenderStateInfo(State);

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
