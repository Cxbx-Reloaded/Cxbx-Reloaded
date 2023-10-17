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

#define LOG_PREFIX CXBXR_MODULE::PSHB

#include <assert.h> // For assert()

#include "core\kernel\support\Emu.h"
#include "core\hle\D3D8\XbD3D8Types.h" // For X_D3DFORMAT, X_D3DDIRTYFLAG
#include "core\hle\D3D8\ResourceTracker.h"
#include "core\hle\D3D8\Direct3D9\Direct3D9.h" // For g_Xbox_VertexShader_Handle
#include "core\hle\D3D8\XbPushBuffer.h"
#include "core\hle\D3D8\XbConvert.h"
#include "core\hle\D3D8\Direct3D9\TextureStates.h"
#include "core\hle\D3D8\Direct3D9\RenderStates.h"
#include "core\hle\D3D8\FixedFunctionState.h"
#include <d3dx9math.h> // for D3DXMatrix, etc
#include "devices/video/nv2a.h" // For g_NV2A, PGRAPHState
#include "devices/video/nv2a_int.h" // For NV** defines
#include "Logging.h"
#include "core\hle\D3D8\Direct3D9\FixedFunctionVertexShaderState.hlsli"

// global state flags for NV2A/KelvinPrimitive, using same bit mask as xbox d3d, defined in XbD3D8Types.h. when content of Kelvin changed, set reresponded flag. then update corresponded state in D3D_draw_state_update()
DWORD NV2A_stateFlags = 0;

// global dirty flags for NV2A/KelvinPrimitive, using same bit mask as xbox d3d, defined in XbD3D8Types.h. when content of Kelvin changed, set reresponded flag. then update corresponded state in D3D_draw_state_update()
DWORD NV2A_DirtyFlags = 0xFFFFFFFF;
/*
// values used by xbox d3d to bit mask d3d dirty flag
#define X_D3DDIRTYFLAG_TEXTURE_STATE                      0x0000000F
#define X_D3DDIRTYFLAG_VERTEXFORMAT_VB                    0x00000010
#define X_D3DDIRTYFLAG_VERTEXFORMAT_UP                    0x00000020
#define X_VERTEXFORMAT_OFFSETS                            0x00000040
#define X_D3DDIRTYFLAG_POINTPARAMS                        0x00000100
#define X_D3DDIRTYFLAG_TRANSFORM                          0x00000200
#define X_D3DDIRTYFLAG_TEXTURE_TRANSFORM                  0x00000400
#define X_D3DDIRTYFLAG_COMBINERS                          0x00000800
#define X_D3DDIRTYFLAG_SPECFOG_COMBINER                   0x00002000
#define X_D3DDIRTYFLAG_SHADER_STAGE_PROGRAM               0x00004000
#define X_D3DDIRTYFLAG_LIGHTS                             0x00FF1000
#define X_SET_STATE_FLAGS                                 0x3FFFFF8F
// valuse for direct mode, when set, overwrite original api priority
// but from reversed code, even X_D3DDIRTYFLAG_DIRECT_INPUT is set,
// when SetTransform() was called, the ModelView/Composite matrix will still be update using the new transform.
// this makes sense, NV2A uses ModelView/Composite internally. so xbox always has to convert its transform to ModelView/Composite.
#define X_D3DDIRTYFLAG_DIRECT_INPUT                       0x40000000
#define X_D3DDIRTYFLAG_DIRECT_MODELVIEW                   0x80000000
*/

// TODO: Find somewhere to put this that doesn't conflict with xbox::
extern void CxbxUpdateHostTextures();
XboxRenderStateConverter NV2ARenderStates; // this var directly access Xbox internal renderstate variables.
XboxTextureStateConverter NV2ATextureStates; // this var directly access Xbox intern TextureState variables.
const char *NV2AMethodToString(DWORD dwMethod); // forward

static void DbgDumpMesh(WORD *pIndexData, DWORD dwCount);
extern std::map<std::string, xbox::addr_xt> g_SymbolAddresses;
extern void EmuKickOff(void);// in Direct3D9.cpp
extern void EmuKickOffWait(void);// in Direct3D9.cpp
extern bool g_nv2a_fifo_is_busy;// in Direct3D9.cpp
extern bool is_pushbuffer_recording(void);
/*
{
	//return (NV2A_stateFlags & X_STATE_RECORDPUSHBUFFER) != 0;
	extern xbox::dword_xt                  *g_pXbox_BeginPush_Buffer;
	return g_pXbox_BeginPush_Buffer == nullptr ? false : true;
}
*/
void backup_xbox_texture_state(void);
void restore_xbox_texture_state(void);
void pgraph_purge_dirty_and_state_flag(void);
void D3D_draw_state_update(NV2AState *d);
void EmuExecutePushBuffer
(
	xbox::X_D3DPushBuffer       *pPushBuffer,
	xbox::X_D3DFixup            *pFixup
)
{
	if (is_pushbuffer_recording()) {
		// we don't support RunPushBuffer() in pushbuffer recording, yet!.
		assert(0);
	}
	// Set pushbuffer parsing busy flag.
	//g_nv2a_fifo_is_busy = true;
	// KickOff xbox d3d pushbuffer first. 
	EmuKickOffWait();

	CxbxUpdateNativeD3DResources();
	//Check whether Fixup exists or not. 
	if (pFixup != xbox::zeroptr) {
		LOG_TEST_CASE("PushBuffer has fixups");
		//Interpret address of PushBuffer Data and Fixup Data
		UINT8* pPushBufferData = (UINT8*)pPushBuffer->Data;
		UINT8* pFixupData = (UINT8*)(pFixup->Data + pFixup->Run);
		UINT32 SizeInBytes = 0;
		UINT32 OffsetInBytes = 0;

		while (TRUE) {
			SizeInBytes = *(UINT32*)pFixupData;
			//If SizeInBytes==0xFFFFFFFF, end of Fixup Data.
			if (SizeInBytes == 0xFFFFFFFF)
				break;
			pFixupData += 4;
			OffsetInBytes = *(UINT32*)pFixupData;
			pFixupData += 4;
			//fixup must not exceed the pushbuffer data range.
			if ((OffsetInBytes + SizeInBytes) <= pPushBuffer->Size)
			{
				//disable fixup for now. currently fixup doesn't work correctly for realtime recorded pushbuffer.
				//that's because we patch HLE calls and for those calls without trampoline, pushbuffer command wasn't sent.
				//but fixup keep the reference and tried to fixup the pushbuffer command. for example, SetVertexConstants().
				//the 1st dword of fixup data is always a method, it should be exactly the same as the data in pushbuffer. if they are not the same, then there must be something wrong in pushbuffer.
				assert(*(DWORD*)(pPushBufferData + OffsetInBytes )== *(DWORD*)pFixupData);
				memcpy(pPushBufferData + OffsetInBytes, pFixupData, SizeInBytes);
			}
			pFixupData += SizeInBytes;
			/*
			When IDirect3DDevice8::RunPushBuffer is called with a fix-up object specified,
			it will parse the fix-up data pointed to by pFixup and with a byte offset of Run.
			The fix-up data is encoded as follows.The first DWORD is the size, in bytes,
			of the push-buffer fix-up to be modified.The second DWORD is the offset, in bytes,
			from the start of the push-buffer where the fix-up is to be modified.
			The subsequent DWORDS are the data to be copied. This encoding repeats for every fix-up to be done,
			until it terminates with a size value of 0xffffffff.
			The offsets must be in an increasing order.
            */
        }
	}
	//g_NV2AVertexAttributeFormat was set and updata every time NV097_SET_VERTEX_DATA_ARRAY_FORMAT was called.
	//the pushbuffer size must be subtracted with 4, because xbox d3d append 1 dwords at the end of original pushbuffer when creating a new pushbuffer,
	//the appended dword is 0xbaadf00d, and when execute the pushbuffer, it insert a jump command in that dword to jump back to main pushbuffer.
	//pDevice-PUSH. but here we do doing HLE, we are not using real push buffer and real dma. so exclude the last dword.
	// reset unnecessary NV2A dirty flags to prevent conflicts with HLE.
	pgraph_purge_dirty_and_state_flag();
	// set g_nv2a_is_pushpuffer_replay flag, for other code to know we're in replay state
	NV2A_stateFlags |= X_STATE_RUNPUSHBUFFERWASCALLED;
	// replay pushbuffer.
	EmuExecutePushBufferRaw((void*)pPushBuffer->Data, pPushBuffer->Size-4, (uint32_t **)nullptr, (uint32_t **)nullptr, (uint8_t *)nullptr);
	// update NV2A state before finish pushbuffer replay. some title use pushbuffer replay to perform Clear()/pixel shader setup, not including draw calls. test case: SVS
	// Retrieve NV2AState via the (LLE) NV2A device :
	extern NV2ADevice* g_NV2A;
	NV2AState *d = g_NV2A->GetDeviceState();
	// this shall update all NV2A texture/transform/vertex shader/pixel shader
	D3D_draw_state_update(d);
	// reset g_nv2a_is_pushpuffer_replay flag
	NV2A_stateFlags &= ~X_STATE_RUNPUSHBUFFERWASCALLED;
    return;
}

DWORD CxbxGetStrideFromVertexDeclaration(CxbxVertexDeclaration* pCxbxVertexDeclaration)
{
	DWORD Stride = 0;

	if (pCxbxVertexDeclaration) {
		// Test-case : Crash 'n' Burn [45530014]
		// Test-case : CrimsonSea [4B4F0002]
		// Test-case : Freedom Fighters
		// Test-case : Hot Wheels Stunt Track Challenge [54510089] 
		// Test-case : Inside Pitch 2003 [4D530034]
		// Test-case : Need for Speed Most Wanted [4541007B]
		// Test-case : Prince of Persia: The Sands of Time [5553001d]
		// Test-case : RPM Tuning [Top Gear RPM Tuning] [4B420007]
		// Test-case : SpyHunter 2 [4D57001B]
		//LOG_TEST_CASE("Non-FVF Vertex Shaders not yet (completely) supported for PushBuffer emulation!");

		if (pCxbxVertexDeclaration->NumberOfVertexStreams == 1) {
			// Note : This assumes that the only stream in use will be stream zero :
			Stride = pCxbxVertexDeclaration->VertexStreams[0].HostVertexStride;
		}
		else {
			LOG_TEST_CASE("Non-FVF Vertex Shaders with multiple streams not supported for PushBuffer emulation!");
		}
	}
	else {
		LOG_TEST_CASE("Missing Vertex Declaration not supported for PushBuffer emulation!");
	}

	return Stride;
}

//for inline_arrays
extern xbox::X_VERTEXATTRIBUTEFORMAT *g_InlineVertexBuffer_DeclarationOverride; // TMP glue
xbox::X_VERTEXATTRIBUTEFORMAT g_NV2AVertexAttributeFormat;

int D3D_StreamZeroStride = 0;

DWORD ABGR_to_ARGB(const uint32_t color)
{
	return (color & 0xFF00FF00) | ((color & 0x00FF0000) >> 16) | ((color & 0x000000FF) << 16);
}

void set_IVB_DECL_override(void)
{
	g_InlineVertexBuffer_DeclarationOverride = &g_NV2AVertexAttributeFormat;
}

void reset_IVB_DECL_override(void)
{
	g_InlineVertexBuffer_DeclarationOverride = nullptr;
}

static const int D3D_inline_attribute_size = 4 * sizeof(float); // Each inline vertex attribute is four floats wide
static const int D3D_inline_vertex_stride = X_VSH_MAX_ATTRIBUTES * D3D_inline_attribute_size; // The IVB reserves room for all 16 attributes per vertex
xbox::X_STREAMINPUT NV2A_StreamSource[X_VSH_MAX_STREAMS] = { 0 }; // Store the vertex buffer/stride info used by each attributes after vertex buffer grouping
xbox::X_D3DVertexBuffer NV2A_VertexBuffer[X_VSH_MAX_STREAMS] = { 0 };
unsigned NV2A_StreamCount = 0;// Store the stream count used by each attributes after vertex buffer grouping, strating from 1
// textures to store the conversion info from NV2A KelvinPrimitive.SetTexture[4]
xbox::X_D3DSurface NV2A_texture_stage_texture[xbox::X_D3DTS_STAGECOUNT];
// pointers to textures which store the converted texture from NV2A KelvinPrimitive.SetTexture[4]
xbox::X_D3DBaseTexture * g_pNV2A_SetTexture[xbox::X_D3DTS_STAGECOUNT]= { 0,0,0,0 };
// unused, replaced by NV2A_DirtyFlags & X_D3DDIRTYFLAG_TEXTURE_STATE_0 << stage;
// bool NV2A_SetTexture_dirty[xbox::X_D3DTS_STAGECOUNT]= { false,false,false,false };
// Backup of g_pXbox_SetTexture[]
xbox::X_D3DBaseTexture *pXbox_SetTexture_Backup[xbox::X_D3DTS_STAGECOUNT] = { 0,0,0,0 };
// update texture of texture stages using NV2A KelvinPrimitive.SetTexture[4]
void backup_xbox_texture_state(void)
{
	for (int i = 0; i < xbox::X_D3DTS_STAGECOUNT; i++)
		pXbox_SetTexture_Backup[i] = g_pXbox_SetTexture[i];
}
void restore_xbox_texture_state(void)
{
	for (int i = 0; i < xbox::X_D3DTS_STAGECOUNT; i++)
		g_pXbox_SetTexture[i] = pXbox_SetTexture_Backup[i];
}
//------------------------------------------------------------------------------
// g_LODBias
//
// A simple log2 table...

const float g_LODBias2x[] =
{
	0.000f, // 1.0
	0.585f, // 1.5
	1.000f, // 2.0
	1.322f, // 2.5
		1.585f, // 3.0
};

// Converts a floating point value to a long.
_declspec(naked) long FloatToLong(float f)
{
	_asm
	{
		// Note that this does a truncate, not a floor:

		cvttss2si eax, [esp + 4]
		ret 4
	}
}

FORCEINLINE DWORD Round(
	FLOAT f)
{
	return (DWORD)(f + 0.5f);//FloatToLong(f + 0.5f);
}
extern float CxbxrGetSuperSampleScale(void);
static inline DWORD FtoDW(FLOAT f) { return *((DWORD*)&f); }
static inline FLOAT DWtoF(DWORD f) { return *((FLOAT*)&f); }
extern std::map<UINT64, xbox::X_D3DSurface> g_TextureCache;

void CxbxrImpl_LazySetTextureState(NV2AState* d)
{
	PGRAPHState* pg = &d->pgraph;
	
	for (int stage = 0; stage < 4; stage++) {
		DWORD warp = 0;// NV097_SET_TEXTURE_ADDRESS_CYLWRAP_U_TRUE | NV097_SET_TEXTURE_ADDRESS_CYLWRAP_V_TRUE;
		// process texture stage texture info if it's dirty
		if ((NV2A_DirtyFlags & (X_D3DDIRTYFLAG_TEXTURE_STATE_0 << stage)) != 0) {
			// if the texture stage is disabled, pg->KelvinPrimitive.SetTexture[stage].Control0 when xbox d3d SetTexture(stage,0), but in actual situation Control0 isn't 0, Offset and Format are 0.
			// FIXME!! check (pg->KelvinPrimitive.SetTexture[stage].Control0 & NV097_SET_TEXTURE_CONTROL0_ENABLE) == 0 should be enough, but there are (pg->KelvinPrimitive.SetTexture[stage].Control0 & NV097_SET_TEXTURE_CONTROL0_ENABLE) != 0 and the texture is empty. could be HLE/NV2A confliction
			if ((pg->KelvinPrimitive.SetTexture[stage].Control0 & NV097_SET_TEXTURE_CONTROL0_ENABLE) == 0){// || pg->KelvinPrimitive.SetTexture[stage].Format == 0 || pg->KelvinPrimitive.SetTexture[stage].Offset == 0) {
				g_pNV2A_SetTexture[stage] = nullptr;
			}
			// texture stage enabled, convert the KelvinPrimitive.SetTexture[stage] to NV2A_texture_stage_texture[stage], and set g_pXbox_SetTexture[stage]
			else {
				bool bTextureFound = false;
				UINT64 key = ((UINT64)(pg->KelvinPrimitive.SetTexture[stage].Format) << 32) | pg->KelvinPrimitive.SetTexture[stage].Offset;
				auto it = g_TextureCache.find(key);
				if (it != g_TextureCache.end()){
					//preserve the whole texture incase we need to debug and the host side released it.
					NV2A_texture_stage_texture[stage] = it->second;
					extern DWORD GetXboxCommonResourceType(const xbox::dword_xt XboxResource_Common);
					g_pNV2A_SetTexture[stage] =(xbox::X_D3DBaseTexture*) & NV2A_texture_stage_texture[stage]; //&NV2A_texture_stage_texture[stage];
					if (GetXboxCommonResourceType(g_pNV2A_SetTexture[stage]->Common) != X_D3DCOMMON_TYPE_SURFACE)
						NV2A_texture_stage_texture[stage].Parent = nullptr;
					bTextureFound = true;
						// can't return directly since we still have to process certain texture related TextureStates
						//return;
				}
			    // if the texture is not found via the texture cache map, then we have to compose the texture via kelvin.
				if(!bTextureFound){
					g_pNV2A_SetTexture[stage] = (xbox::X_D3DBaseTexture*)&NV2A_texture_stage_texture[stage];
					NV2A_texture_stage_texture[stage].Data = pg->KelvinPrimitive.SetTexture[stage].Offset;
					NV2A_texture_stage_texture[stage].Format = pg->KelvinPrimitive.SetTexture[stage].Format;
					unsigned width = 0, height = 0, pitch = 0, depth = 0;

					DWORD format = pg->KelvinPrimitive.SetTexture[stage].Format;
					bool dma_select =
						GET_MASK(format, NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA) == 2;
					bool cubemap =
						format & NV097_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE;
					unsigned int border_source =
						format & NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE;
					unsigned int dimensionality =
						GET_MASK(format, NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY);
					unsigned int color_format =
						GET_MASK(format, NV097_SET_TEXTURE_FORMAT_COLOR);
					unsigned int levels =
						GET_MASK(format, NV097_SET_TEXTURE_FORMAT_MIPMAP_LEVELS);
					unsigned int log_width =
						GET_MASK(format, NV097_SET_TEXTURE_FORMAT_BASE_SIZE_U);
					unsigned int log_height =
						GET_MASK(format, NV097_SET_TEXTURE_FORMAT_BASE_SIZE_V);
					unsigned int log_depth =
						GET_MASK(format, NV097_SET_TEXTURE_FORMAT_BASE_SIZE_P);

					bool bCubeness = (format
						& NV097_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE) != 0;
					DWORD Dimensionality = (format & NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY) >> 4;
					// texture.Size could be 0
					// when size ==0, Control1 and ImageRect won't be updated. So we use a hack to always reset these two vars whenever NV097_SET_TEXTURE_OFFSET was hit.
					if (pg->KelvinPrimitive.SetTexture[stage].Control1 == 0 && pg->KelvinPrimitive.SetTexture[stage].ImageRect == 0) {
						// when log_width/height/depth !=0, which means the size was represent by these log values, so size would be zero.
						//if( (log_width!=0) || (log_height!=0)||(log_depth!=0)){
						NV2A_texture_stage_texture[stage].Size = 0;
					}
					//size!=0, using Control1 and ImageRect to convert pitch/height/width to texture.Size
					else {
						pitch = (pg->KelvinPrimitive.SetTexture[stage].Control1 & NV097_SET_TEXTURE_CONTROL1_IMAGE_PITCH) >> 16;
						width = (pg->KelvinPrimitive.SetTexture[stage].ImageRect & NV097_SET_TEXTURE_IMAGE_RECT_WIDTH) >> 16;
						height = (pg->KelvinPrimitive.SetTexture[stage].ImageRect & NV097_SET_TEXTURE_IMAGE_RECT_HEIGHT);

						width = (width - 1) & X_D3DSIZE_WIDTH_MASK;
						height = ((height - 1) << X_D3DSIZE_HEIGHT_SHIFT) & X_D3DSIZE_HEIGHT_MASK;
						pitch = ((pitch / 64) - 1) << X_D3DSIZE_PITCH_SHIFT;//&X_D3DSIZE_PITCH_MASK
						NV2A_texture_stage_texture[stage].Size = pitch | height | width;
					}
					NV2A_texture_stage_texture[stage].Common = 0x00040001;// fake xbox d3d resource,
					NV2A_texture_stage_texture[stage].Lock = 0;
					//default to texture, no parent
					NV2A_texture_stage_texture[stage].Parent = nullptr;
			    }// end of if (it != g_TextureCache.end())/else
				//update texture stage states
				DWORD filter, address, control0, magFilter, minFilter, colorSign, convolutionKernel;
				filter = pg->KelvinPrimitive.SetTexture[stage].Filter;//NV097_SET_TEXTURE_FILTER(stage)
#define D3DTSIGN_ASIGNED           0x10000000
#define D3DTSIGN_AUNSIGNED         0
#define D3DTSIGN_RSIGNED           0x20000000
#define D3DTSIGN_RUNSIGNED         0
#define D3DTSIGN_GSIGNED           0x40000000
#define D3DTSIGN_GUNSIGNED         0
#define D3DTSIGN_BSIGNED           0x80000000
#define D3DTSIGN_BUNSIGNED         0

				colorSign = pg->KelvinPrimitive.SetTexture[stage].Filter & 0xF0000000;// XboxTextureStates.Get(i, xbox::X_D3DTSS_COLORSIGN);
				NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORSIGN, colorSign);
				
				int intLodBias;
				intLodBias = (filter & NV097_SET_TEXTURE_FILTER_MIPMAP_LOD_BIAS);
				if ((intLodBias & 0x1000) != 0)
					intLodBias |= 0xFFFFF000;
				float fLodBias = float(intLodBias);
				float mipMapLodBias = fLodBias/256.0;

				NV2ATextureStates.Set(stage, xbox::X_D3DTSS_MIPMAPLODBIAS, FtoDW(mipMapLodBias));

				extern float CxbxrGetSuperSampleScale(void);
				float minScale = CxbxrGetSuperSampleScale();
				float SuperSampleLODBias = g_LODBias2x[Round(2.0f * minScale) - 2];

				DWORD colorKeyOp = pg->KelvinPrimitive.SetTexture[stage].Control0 & 0x3;
				DWORD ALPHAKILL = pg->KelvinPrimitive.SetTexture[stage].Control0 & 0x4;
				DWORD maxAnisotropy = (pg->KelvinPrimitive.SetTexture[stage].Control0 & NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO) >> 4;
				DWORD minMipFilter = xbox::X_D3DTEXF_NONE;
				magFilter = (filter & NV097_SET_TEXTURE_FILTER_MAG) >> 24;
				minFilter = (filter & NV097_SET_TEXTURE_FILTER_MIN) >> 16;
				minMipFilter = (minFilter - 1) / 2;
				minFilter = (minFilter & 1) ? 1 : 2;
				convolutionKernel = filter & NV097_SET_TEXTURE_FILTER_MIN | NV097_SET_TEXTURE_FILTER_CONVOLUTION_KERNEL | NV097_SET_TEXTURE_FILTER_MAG;
				if (((convolutionKernel& NV097_SET_TEXTURE_FILTER_MIN) == NV097_SET_TEXTURE_FILTER_MIN_CONVOLUTION_2D_LOD0)
					&&(magFilter == xbox::X_D3DTEXF_QUINCUNX)
					&&(minFilter == xbox::X_D3DTEXF_LINEAR)) {
					magFilter = xbox::X_D3DTEXF_QUINCUNX;
					if (((convolutionKernel & NV097_SET_TEXTURE_FILTER_CONVOLUTION_KERNEL) == NV097_SET_TEXTURE_FILTER_CONVOLUTION_KERNEL_GAUSSIAN_3)) {
						magFilter = xbox::X_D3DTEXF_GAUSSIANCUBIC;;
					}
					minFilter = magFilter;
				}
				//only reverse mag/min filter back to when maxAnisotropy is working
				else if((maxAnisotropy > 0)
					&&(magFilter== xbox::X_D3DTEXF_LINEAR)
					&&(minFilter == xbox::X_D3DTEXF_LINEAR)){
					//control0 |= NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO,maxAnisotropy;
					/*
					if ((minFilter == xbox::X_D3DTEXF_POINT) && (magFilter == xbox::X_D3DTEXF_POINT) && (maxAnisotropy == 0)) {//D3DTEXF_POINT=1
						//(minFilter < D3DTEXF_ANISOTROPIC) && (magFilter < D3DTEXF_ANISOTROPIC) // D3DTEXF_ANISOTROPIC=3
						//NV2ATextureStates.Set(stage, xbox::X_D3DTSS_MAXANISOTROPY, 0);
						minFilter = 3;
						magFilter = 3;
				    }
					else if ((minFilter == xbox::X_D3DTEXF_LINEAR) && (magFilter == xbox::X_D3DTEXF_LINEAR)&&(maxAnisotropy!=0)) {//D3DTEXF_LINEAR=2
						//(minFilter < D3DTEXF_ANISOTROPIC) && (magFilter < D3DTEXF_ANISOTROPIC)
						//NV2ATextureStates.Set(stage, xbox::X_D3DTSS_MAXANISOTROPY, maxAnisotropy + 1);
						maxAnisotropy++;
						minFilter = 3;
						magFilter = 3;
					}
					else {
						;//minFilter and magFilter untouched.
					}
					*/
					maxAnisotropy++;
					minFilter = xbox::X_D3DTEXF_ANISOTROPIC;
					magFilter = xbox::X_D3DTEXF_ANISOTROPIC;
				}
				// for other cases, we keep min/mag filters as is.
				NV2ATextureStates.Set(stage, xbox::X_D3DTSS_MAXANISOTROPY, maxAnisotropy);
				NV2ATextureStates.Set(stage, xbox::X_D3DTSS_MIPFILTER, minMipFilter);
				NV2ATextureStates.Set(stage, xbox::X_D3DTSS_MINFILTER, minFilter);
				NV2ATextureStates.Set(stage, xbox::X_D3DTSS_MAGFILTER, magFilter);

				DWORD COLORKEYCOLOR = pg->KelvinPrimitive.SetColorKeyColor[stage];
				DWORD maxMipMapLevel = (pg->KelvinPrimitive.SetTexture[stage].Control0 & NV097_SET_TEXTURE_CONTROL0_MIN_LOD_CLAMP)>>(18+8); 
				NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORKEYOP, colorKeyOp);
				NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAKILL, ALPHAKILL);
				NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORKEYCOLOR, COLORKEYCOLOR);
				NV2ATextureStates.Set(stage, xbox::X_D3DTSS_MAXMIPLEVEL, maxMipMapLevel);

				address = pg->KelvinPrimitive.SetTexture[stage].Address;
				warp=address & (NV097_SET_TEXTURE_ADDRESS_CYLWRAP_U_TRUE | NV097_SET_TEXTURE_ADDRESS_CYLWRAP_V_TRUE | NV097_SET_TEXTURE_ADDRESS_CYLWRAP_P_TRUE);
				NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ADDRESSU, address& NV097_SET_TEXTURE_ADDRESS_U);
				NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ADDRESSV, (address & NV097_SET_TEXTURE_ADDRESS_V)>>8);
				NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ADDRESSW, (address & NV097_SET_TEXTURE_ADDRESS_P)>>16);
			}
		}
		NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_WRAP0 + stage, warp);
	}
}
extern NV2ADevice* g_NV2A; //TMP GLUE
extern D3DMATRIX g_xbox_transform_ModelView;
extern D3DMATRIX g_xbox_transform_InverseModelView;
extern D3DMATRIX g_xbox_transform_Composite;
extern D3DMATRIX g_xbox_DirectModelView_View;
extern D3DMATRIX g_xbox_DirectModelView_InverseView;
extern D3DMATRIX g_xbox_DirectModelView_World;
extern D3DMATRIX g_xbox_DirectModelView_Projection;
extern bool g_VertexShader_dirty;

extern xbox::void_xt WINAPI CxbxrImpl_SetModelView
(
	CONST D3DMATRIX *pModelView,
	CONST D3DMATRIX *pInverseModelView,
	CONST D3DMATRIX *pComposite
);
bool g_xbox_transform_use_DirectModelView = false;
bool g_NV2A_transform_ModelView_dirty[4] = { false,false,false,false };
bool g_NV2A_transform_InverseModelView_dirty[4] = { false,false,false,false };
bool g_NV2A_transform_Composite_dirty = false;
// virtual pixle shader, convert content from KelvinPrimitive to the pixel shader below and call CxbxrImpl_SetPixelShader(pNV2A_PixelShader). 
xbox::X_PixelShader* pNV2A_PixelShader;
xbox::X_PixelShader NV2A_PixelShader;
xbox::X_D3DPIXELSHADERDEF NV2A_PSDef;
bool NV2A_ShaderOtherStageInputDirty=false;
bool NV2A_TextureFactorAllTheSame = false;
bool g_nv2a_use_Kelvin = false;
DWORD NV2A_colorOP[8];
DWORD NV2A_colorArg0[8];
DWORD NV2A_colorArg1[8];
DWORD NV2A_colorArg2[8];
DWORD NV2A_alphaOP[8];
DWORD NV2A_alphaArg0[8];
DWORD NV2A_alphaArg1[8];
DWORD NV2A_alphaArg2[8];
DWORD NV2A_resultArg[8];
xbox::X_D3DLIGHT8 NV2A_Light8[8] = {};
D3DCOLORVALUE   NV2A_SceneAmbient[2];
xbox::X_D3DMATERIAL8  NV2A_SceneMateirals[2];

xbox::X_D3DLIGHT8* CxbxrGetLight8Ptr(int lightNum)
{
	return  &NV2A_Light8[lightNum];
}
DWORD CxbxrNV2ALight8EnableMask(int lightNum)
{
	NV2AState* d = g_NV2A->GetDeviceState();
	PGRAPHState* pg = &d->pgraph;
	return (pg->KelvinPrimitive.SetLightEnableMask >> (lightNum << 1) & 0x03);
}
void pgraph_SetNV2AStateFlag(DWORD flag)
{
	NV2A_stateFlags |= flag;
}
void pgraph_ClearNV2AStateFlag(DWORD flag)
{
	NV2A_stateFlags &= ~flag;
}
bool pgraph_GetNV2AStateFlag(DWORD flag)
{
	return (NV2A_stateFlags & flag)!=0;//X_STATE_COMBINERNEEDSSPECULAR)!=0;
}
float * pgraph_get_NV2A_bumpenv_stage_address(unsigned int stage) {
	// Retrieve NV2AState via the (LLE) NV2A device :
	NV2AState *d = g_NV2A->GetDeviceState();
	PGRAPHState *pg = &d->pgraph;
	return &pg->KelvinPrimitive.SetTexture[stage].SetBumpEnvMat00;
}
void pgraph_use_NV2A_Kelvin(void)
{
	g_nv2a_use_Kelvin = true;
}
void pgraph_notuse_NV2A_Kelvin(void)
{
	g_nv2a_use_Kelvin = false;
}
bool is_pgraph_using_NV2A_Kelvin(void)
{
	return g_nv2a_use_Kelvin;
}
void pgraph_use_UserPixelShader(void)
{
	// set pixel shader pointers
	pNV2A_PixelShader = &NV2A_PixelShader;
	NV2A_PixelShader.pPSDef = &NV2A_PSDef;
	// set dirty flag
	//NV2A_DirtyFlags |= X_D3DDIRTYFLAG_SHADER_STAGE_PROGRAM;
}

void pgraph_use_FixedPixelShader(void)
{
	// set pixel shader pointer to null for fixed function pixel shader
	pNV2A_PixelShader = nullptr;
	// set dirty flag
	//NV2A_DirtyFlags |= X_D3DDIRTYFLAG_SHADER_STAGE_PROGRAM;
}
// called when SetModelVeiw(0,0,0) was called.
void pgraph_use_Transform(void)
{
	//g_xbox_transform_ModelView_dirty[0] = false;
	//g_xbox_transform_InverseModelView_dirty[0] =false;
	//g_xbox_transform_Composite_dirty = false;
	g_xbox_transform_use_DirectModelView = false;
}

void pgraph_use_DirectModelView(void)
{
	g_xbox_transform_use_DirectModelView = true;
}

bool is_pgraph_DirectModelView(void)
{
	return g_xbox_transform_use_DirectModelView;
}
bool pgraph_is_ModelView_dirty(void)
{
	return g_NV2A_transform_ModelView_dirty[0];
}

void pgraph_SetModelViewMatrixDirty(unsigned int index)
{
	//PGRAPHState *pg = &d->pgraph;
	//D3DXMatrixTranspose((D3DXMATRIX *)&g_xbox_transform_ModelView, (D3DXMATRIX *)&pg->KelvinPrimitive.SetModelViewMatrix0[0]);
	g_NV2A_transform_ModelView_dirty[index] = true;
	//g_xbox_transform_use_DirectModelView = true;
}
void pgraph_SetInverseModelViewMatrixDirty(unsigned int index)
{
	//PGRAPHState *pg = &d->pgraph;
	//g_xbox_transform_InverseModelView = *(D3DMATRIX *)(&pg->KelvinPrimitive.SetInverseModelViewMatrix0[0]);
	g_NV2A_transform_InverseModelView_dirty[index] = true;
	//g_xbox_transform_use_DirectModelView = true;
}
void pgraph_SetCompositeMatrixDirty(void)
{
	//PGRAPHState *pg = &d->pgraph;
	//D3DXMatrixTranspose((D3DXMATRIX *)&g_xbox_transform_Composite, (D3DXMATRIX *)&pg->KelvinPrimitive.SetCompositeMatrix[0]);
	g_NV2A_transform_Composite_dirty = true;
	//g_xbox_transform_use_DirectModelView = true;
}
// this function purges NV2A internal dirty flags and state flags. this shall be called before replaying a pushbuffer in order to clear any intermediate state left over by the HLE part running prior to the replay pushbuffer
void pgraph_purge_dirty_and_state_flag(void)
{
	NV2AState *d = g_NV2A->GetDeviceState();
	PGRAPHState *pg = &d->pgraph;
	// reset NV2A dirty flags
	NV2A_DirtyFlags = 0;
	// reset NV2A stage flags
	NV2A_stateFlags = 0;
	// reset shader other stage input dirty flag, prevent pixel shader misinterpretation.
	NV2A_ShaderOtherStageInputDirty = false;
	//reset texture stage dirty flags
	for(int i=0;i<4;i++){
		// reset SetTexture dirty flags, unused, replaced by NV2A_DirtyFlags & X_D3DDIRTYFLAG_TEXTURE_STATE_0 << stage;
		// NV2A_SetTexture_dirty[i] = false;
		// reset bumpenv dirty flags
		pg->bumpenv_dirty[i] = false;
	}
}
extern XboxTextureStateConverter XboxTextureStates;
extern XboxRenderStateConverter XboxRenderStates;
extern xbox::void_xt CxbxrImpl_SetPixelShader(xbox::dword_xt Handle);
extern void GetMultiSampleScaleRaw(float& xScale, float& yScale);// tmp glue


void CxbxrImpl_CommonSetTextureBumpEnv(NV2AState *d)
{
	PGRAPHState* pg = &d->pgraph;
	// for bmpenv, NV2A always uses stage 1~3. in user mode, d3d uses stage 1~3, in fixed mode, d3d uses stage 0~2.
	bool isUserMode = pNV2A_PixelShader==nullptr?false:true;
	unsigned int d3d_stage = (isUserMode) ? 1 : 0;
	unsigned int nv2a_stage;
	//for (nv2a_stage = 1; nv2a_stage <4; nv2a_stage++, d3d_stage++) {
	// default d3d_stage to be the same as NV2A_stage, and sets xbox TextureStates in the same stage. and we also set bumpenv in stage 0.
	d3d_stage = 0;
	if (is_pgraph_using_NV2A_Kelvin()) {
		for (nv2a_stage = 0; nv2a_stage < 4; nv2a_stage++, d3d_stage++) {
			NV2ATextureStates.Set(d3d_stage, xbox::X_D3DTSS_BUMPENVMAT00, FtoDW(pg->KelvinPrimitive.SetTexture[nv2a_stage].SetBumpEnvMat00));
			NV2ATextureStates.Set(d3d_stage, xbox::X_D3DTSS_BUMPENVMAT01, FtoDW(pg->KelvinPrimitive.SetTexture[nv2a_stage].SetBumpEnvMat01));
			NV2ATextureStates.Set(d3d_stage, xbox::X_D3DTSS_BUMPENVMAT11, FtoDW(pg->KelvinPrimitive.SetTexture[nv2a_stage].SetBumpEnvMat11));
			NV2ATextureStates.Set(d3d_stage, xbox::X_D3DTSS_BUMPENVMAT10, FtoDW(pg->KelvinPrimitive.SetTexture[nv2a_stage].SetBumpEnvMat10));
			NV2ATextureStates.Set(d3d_stage, xbox::X_D3DTSS_BUMPENVLSCALE, FtoDW(pg->KelvinPrimitive.SetTexture[nv2a_stage].SetBumpEnvScale));
			NV2ATextureStates.Set(d3d_stage, xbox::X_D3DTSS_BUMPENVLOFFSET, FtoDW(pg->KelvinPrimitive.SetTexture[nv2a_stage].SetBumpEnvOffset));
		}
	}
	else {
		for (nv2a_stage = 0; nv2a_stage < 4; nv2a_stage++, d3d_stage++) {
			XboxTextureStates.Set(d3d_stage, xbox::X_D3DTSS_BUMPENVMAT00, FtoDW(pg->KelvinPrimitive.SetTexture[nv2a_stage].SetBumpEnvMat00));
			XboxTextureStates.Set(d3d_stage, xbox::X_D3DTSS_BUMPENVMAT01, FtoDW(pg->KelvinPrimitive.SetTexture[nv2a_stage].SetBumpEnvMat01));
			XboxTextureStates.Set(d3d_stage, xbox::X_D3DTSS_BUMPENVMAT11, FtoDW(pg->KelvinPrimitive.SetTexture[nv2a_stage].SetBumpEnvMat11));
			XboxTextureStates.Set(d3d_stage, xbox::X_D3DTSS_BUMPENVMAT10, FtoDW(pg->KelvinPrimitive.SetTexture[nv2a_stage].SetBumpEnvMat10));
			XboxTextureStates.Set(d3d_stage, xbox::X_D3DTSS_BUMPENVLSCALE, FtoDW(pg->KelvinPrimitive.SetTexture[nv2a_stage].SetBumpEnvScale));
			XboxTextureStates.Set(d3d_stage, xbox::X_D3DTSS_BUMPENVLOFFSET, FtoDW(pg->KelvinPrimitive.SetTexture[nv2a_stage].SetBumpEnvOffset));
		}
	}
}
DWORD convert_NV2A_combiner_reg_to_xbox_reg(DWORD nvreg) {
	DWORD xboxreg = xbox::X_D3DTA_CURRENT;
	assert(nvreg >= NV097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_4 ||nvreg== NV097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_1);
	assert(nvreg <= NV097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_D);
	switch (nvreg) {
	//case NV097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_0:xboxreg = xbox::X_D3DTA_CURRENT; break; //FIXME!!! REG ZERo
	case NV097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_1:xboxreg = xbox::X_D3DTA_TFACTOR; break; //FIXME!!! REG CONST0
	//case NV097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_2:xboxreg = xbox::X_D3DTA_CURRENT; break; //FIXME!!! REG CONST1
	//case NV097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_3:xboxreg = xbox::X_D3DTA_CURRENT; break; //FIXME!!! REG FOG
	// only these cases below are valid
	case NV097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_4:xboxreg = xbox::X_D3DTA_DIFFUSE; break;
	case NV097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_5:xboxreg = xbox::X_D3DTA_SPECULAR; break;
	case NV097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_8://xboxreg = xbox::X_D3DTA_CURRENT; break;  //TEX0
	case NV097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_9://xboxreg = xbox::X_D3DTA_CURRENT; break;  //TEX1
	case NV097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_A://xboxreg = xbox::X_D3DTA_CURRENT; break;  //TEX2
	case NV097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_B:xboxreg = xbox::X_D3DTA_TEXTURE; break;  //TEX3
	case NV097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_C:xboxreg = xbox::X_D3DTA_CURRENT; break;
	case NV097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_D:xboxreg = xbox::X_D3DTA_TEMP; break;
	}
	return xboxreg;
}
//update only needed in fixed mode pixel shader
void CxbxrImpl_LazySetCombiners(NV2AState *d)
{
	PGRAPHState *pg = &d->pgraph;
	// FIXME!! in user program, the actual stage used could exceed 4, Otogi uses stage 0~5. xbox d3d colorOP/args setting with stage might not comply with the compiled result of user program.
	// there is not feasible way to reverse user program back to xbox d3d color OP.
	// fortunately we only need these reversed info for fixed function pixel shader.
	// in fixed mode, we only use stage 0~3, and when point sprite is enabled, only stage 3 will be used.
	int startStage = 0;
	// FIXME!!! if we start with stage 3, what will happen with stage 0~2? 
    // when point sprite is enabled, only stage 3 will be used. set colorOp in stage 0~2 disabled.
	if (pg->KelvinPrimitive.SetPointSmoothEnable != 0)
		startStage = 3;
	int stage = startStage;
	// xbox LazySetCombiners() start with both i and startStage as 3 when point sprite was enabled without updating i==0~2.
	// but even when startStage==3, the first updated stage in hardware still starts from 0; all unused combiner stages shall be zero out.
	// i for hardware combiner stage index:ranges 0~8
	// stage for xbox texture stage index: ranges 0~3, when point sprite was enabled, it starts with 3
	for (int i =0; stage < 4; i++,stage++) {

		auto colorOp = pg->KelvinPrimitive.SetCombinerColorICW[i];// = XboxTextureStates.Get(i, xbox::X_D3DTSS_COLOROP);// FIXME!!!

		auto colorICW = pg->KelvinPrimitive.SetCombinerColorICW[i];
		auto colorOCW = pg->KelvinPrimitive.SetCombinerColorOCW[i];

		auto alphaOp = pg->KelvinPrimitive.SetCombinerAlphaICW[i];

		auto alphaICW = pg->KelvinPrimitive.SetCombinerAlphaICW[i];
		auto alphaOCW = pg->KelvinPrimitive.SetCombinerAlphaOCW[i];

		auto resultarg = pg->KelvinPrimitive.SetCombinerColorOCW[i];
		NV2A_colorOP[i] = i == 0 ? xbox::X_D3DTOP_MODULATE : xbox::X_D3DTOP_DISABLE;
		// set default arg registers
		NV2A_colorArg0[i] = xbox::X_D3DTA_CURRENT;
		NV2A_colorArg1[i] = xbox::X_D3DTA_TEXTURE; //convert_NV2A_combiner_reg_to_xbox_reg(((colorICW >> 24) & 0xF)) ;// FIXME!!!,  reg 4 is D3DTA_DIFFUSE :0, not sure this direct shift is correct or not6
		NV2A_colorArg2[i] = xbox::X_D3DTA_CURRENT;//convert_NV2A_combiner_reg_to_xbox_reg(((colorICW) & 0xF)) ;// FIXME!!!,  reg 4 is D3DTA_DIFFUSE :0, not sure this direct shift is correct or not6
		NV2A_alphaOP[i] = i == 0 ? xbox::X_D3DTOP_SELECTARG1 : xbox::X_D3DTOP_DISABLE;
		// set default arg registers
		NV2A_alphaArg0[i] = xbox::X_D3DTA_CURRENT;
		NV2A_alphaArg1[i] = g_pNV2A_SetTexture[i] == nullptr ? xbox::X_D3DTA_DIFFUSE : xbox::X_D3DTA_TEXTURE;//convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF) ;// FIXME!!!,  reg 4 is D3DTA_DIFFUSE :0, not sure this direct shift is correct or not6
		NV2A_alphaArg2[i] = xbox::X_D3DTA_CURRENT;//convert_NV2A_combiner_reg_to_xbox_reg((alphaICW) & 0xF) ;// FIXME!!!,  reg 4 is D3DTA_DIFFUSE :0, not sure this direct shift is correct or not6
		// set result arg register
		// normally the result register is either C: current or D: temp, and located in bit 8~11 as sum of A*B+C*D, or in bit 4~7 as A*B.
		enum OPMode {
			OPMode_MAD,
		    OPMode_ADD,
		}OPMode;
		if (((resultarg >> 8) & 0xF) > 0) {
			NV2A_resultArg[i] = ((resultarg >> 8) & 0xF) == 0xC ? xbox::X_D3DTA_CURRENT : xbox::X_D3DTA_TEMP;
			OPMode = OPMode_MAD;
		}
		else if (((resultarg >> 4) & 0xF) > 0) {
			NV2A_resultArg[i] = ((resultarg >> 4) & 0xF) == 0xC ? xbox::X_D3DTA_CURRENT : xbox::X_D3DTA_TEMP;
			OPMode = OPMode_ADD;
		}
		/* // leave resultArg as defaul value, this happened when resultArg=0 and both colorOP and alphaOP = 0 which means both OPs are disabled.
		else {
			// we shouldn't be here because there is no feasible result register set.
			assert(0);
			// set resultArg to current.
			NV2A_resultArg[i]== xbox::X_D3DTA_CURRENT;

		}
		*/

		// set color OP with invalid value as flag to process later
		NV2A_colorOP[i] = (colorICW == 0 && colorOCW == 0) ? xbox::X_D3DTOP_DISABLE : xbox::X_D3DTOP_LAST + 1;
		// set alpha OP as X_D3DTOP_DISABLE
		NV2A_alphaOP[i] = (alphaICW == 0 && alphaOCW == 0) ? xbox::X_D3DTOP_DISABLE : xbox::X_D3DTOP_LAST + 1;

		NV2A_colorArg0[i] = 1;
		NV2A_alphaArg0[i] = 1;

		// if start stage both colorOP and alphaOP are D3DTOP_DISABLE, then all stage after it shall be D3DTOP_DISABLE.
		
		if (stage == 0) {//todo: 0 or start stage?
			// xbox convert stage 0 colorOP==X_D3DTOP_DISABLE to selec arg1 with arg1=current, and in stage 0, current==diffuse.
			// FIXME!! if stage 0 was disabled, should we still setup the combiner with default values just like xbox d3d does instead of skipping it?
			// and for PointSprite enabled, the combiner stage update starts at stage 3, not 0, so this condition will happen in stage 3. what shall we do with stage 0 and 1?
			//if ((colorICW == 0x04200000 || colorICW == 0x00002004)&& (alphaICW == (0x14200000)) && (alphaOCW == colorOCW)) {//(colorICW == (NV097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_4 | NV097_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_INVERT  )) && alphaICW == (colorICW | NV097_SET_COMBINER_COLOR_ICW_A_ALPHA )) { //,(0x10 & 0x20) << 23)
			//if ((colorICW == 0x04200000)&& pg->KelvinPrimitive.SetCombinerColorICW[1]==0) {//(colorICW == (NV097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_4 | NV097_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_INVERT  )) && alphaICW == (colorICW | NV097_SET_COMBINER_COLOR_ICW_A_ALPHA )) { //,(0x10 & 0x20) << 23)
				//NV2A_colorOP[i] = xbox::X_D3DTOP_DISABLE;
				//NV2A_alphaOP[i] = xbox::X_D3DTOP_DISABLE;
			//}
			if ((alphaICW == (0x14200000))&& pg->KelvinPrimitive.SetCombinerAlphaICW[1]==0) {//(colorICW == (NV097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_4 | NV097_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_INVERT  )) && alphaICW == (colorICW | NV097_SET_COMBINER_COLOR_ICW_A_ALPHA )) { //,(0x10 & 0x20) << 23)
				//NV2A_colorOP[i] = xbox::X_D3DTOP_DISABLE;
				NV2A_alphaOP[i] = xbox::X_D3DTOP_DISABLE;
			}
		}
		
		// FIXME!!! can we really continue the loop when colorOP == X_D3DTOP_DISABLE?
		if (NV2A_colorOP[i] == xbox::X_D3DTOP_DISABLE) {
			;// NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLOROP, xbox::X_D3DTOP_DISABLE);
		}
		else {
			// process color OP if it's not X_D3DTOP_LAST
			if (NV2A_colorOP[i] > xbox::X_D3DTOP_LAST) {
				// colorOP SelectARG1 , A == Arg1, B == 1, C == 0, D == 0
				if (((colorICW & 0xF0FFFFFF) == 0x00200000) && ((colorOCW & 0xFFFF00FF) == 0)) {
					NV2A_colorOP[i] = xbox::X_D3DTOP_SELECTARG1;
					// arg1 always source A
					NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF); // FIXME!!!,  reg 4 is D3DTA_DIFFUSE :0, not sure this direct shift is correct or not6
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG1, NV2A_colorArg1[i]);
				}
				// colorOP SelectARG2, A == 0, B ==0, C == 1, D == Arg2
				else if (((colorICW & 0xFFFFFF00) == 0x00002000) && ((colorOCW & 0xFFFF00FF) == 0)) {
					NV2A_colorOP[i] = xbox::X_D3DTOP_SELECTARG2;
					// arg2 always source D
					NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW) & 0xF);// FIXME!!!,  reg 4 is D3DTA_DIFFUSE :0, not sure this direct shift is correct or not6
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG2, NV2A_colorArg2[i]);
				}
				// OP modulate, A*B, C==0, D==0, test case PlayField sample, color op = Modulate, arg1=Txture, arg2=Texture|Alpha, colorICW=081819000
				else if ((colorICW & 0xE0E00FFF) == 0x0) {
					// arg1 in source A
					NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
					// arg2 in source B
					NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 16) & 0xF);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG1, NV2A_colorArg1[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG2, NV2A_colorArg2[i]);
					if ((colorOCW & 0x20000) != 0) {
						NV2A_colorOP[i] = xbox::X_D3DTOP_MODULATE4X;
					}
					else if ((colorOCW & 0x10000) != 0) {
						NV2A_colorOP[i] = xbox::X_D3DTOP_MODULATE2X;
					}
					else {
						NV2A_colorOP[i] = xbox::X_D3DTOP_MODULATE;
					}
				}
				// OP ADD, source A + D, A == Arg1, B == 1, C == 1, D == Arg2
				else if ((colorICW & 0xE0EFFFF0) == 0x00202000) {
					NV2A_colorOP[i] = xbox::X_D3DTOP_ADD;
					if ((colorOCW & 0x18000) == 0x8000)
						NV2A_colorOP[i] = xbox::X_D3DTOP_ADDSIGNED;
					if ((colorOCW & 0x18000) == 0x18000)
						NV2A_colorOP[i] = xbox::X_D3DTOP_ADDSIGNED2X;
					// arg1 in source A
					NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
					// arg2 in source D
					NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW) & 0xF);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG1, NV2A_colorArg1[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG2, NV2A_colorArg2[i]);
				}
				// D3DTOP_SUBTRACT,  source A - D, A == Arg1, B == 1, C == -1, D == Arg2
				else if ((colorICW & 0xE0EFFFF0) == 0x00204000) {
					NV2A_colorOP[i] = xbox::X_D3DTOP_SUBTRACT;
					// arg1 in source A
					NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
					// arg2 in source D
					NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW) & 0xF);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG1, NV2A_colorArg1[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG2, NV2A_colorArg2[i]);
				}
				// D3DTOP_ADDSMOOTH, A+C*D, A=Arg1, B=1, C=1-Arg1, D=Arg2
				else if (((colorICW & 0xE0FFE0E0) == 0x00202000) && (convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF) == convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 8) & 0xF))) {
					NV2A_colorOP[i] = xbox::X_D3DTOP_ADDSMOOTH;
					// arg1 in source A
					NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
					// arg2 in source D
					NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW) & 0xF);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG1, NV2A_colorArg1[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG2, NV2A_colorArg2[i]);

				}
				// D3DTOP_BLENDDIFFUSEALPHA/D3DTOP_BLENDCURRENTALPHA/D3DTOP_BLENDTEXTUREALPHA/D3DTOP_BLENDFACTORALPHA, A*B+C*D, A=Arg1, B=alphaOP-12 and CFLAG_ALPHAREPLICATE set , C=1-(alphaOP-12), D=Arg2
				// A+C*D, A=Arg1, B=alpha(alphaOP-12) and CFLAG_ALPHAREPLICATE set , C=alpha(1-Arg1) CFLAG_COMPLEMENT|CFLAG_ALPHAREPLICATE set , D=Arg2
				// verify flags of A/B/C/D, B=reg0, reg(A)==reg(C)
				else if (((colorICW & 0xF0F0F0F0) == 0x00103000) && (convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 16) & 0xF) == convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 8) & 0xF))) {
					NV2A_colorOP[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 16) & 0xF) + xbox::X_D3DTOP_BLENDDIFFUSEALPHA;
					// arg1 in source A
					NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
					// arg2 in source D
					NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW) & 0xF);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG1, NV2A_colorArg1[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG2, NV2A_colorArg2[i]);
				}
				// D3DTOP_BLENDTEXTUREALPHAPM, A*1+C*D, A=Arg1, B=1, C=alpha(1-D3DTA_TEXTURE) CFLAG_COMPLEMENT | CFLAG_ALPHAREPLICATE, D=Arg2,
				else if (((colorICW & 0xF0FFFFF0) == 0x00203200)) {
					NV2A_colorOP[i] = xbox::X_D3DTOP_BLENDTEXTUREALPHAPM;
					// arg1 in source A
					NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
					// arg2 in source D
					NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW) & 0xF);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG1, NV2A_colorArg1[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG2, NV2A_colorArg2[i]);
				}
				// D3DTOP_PREMODULATE, A=Arg1, if stage 0, B==D3DTA_TEXTURE, if stage1 or stage2 B=1, C and D not set
				else if ((i > 0 && i < 3 && ((colorICW & 0xE0FFFFFF) == 0x00200000)) || (i == 0 && ((colorICW & 0xF0FFFFFF) == 0x00020000))) {
					NV2A_colorOP[i] = xbox::X_D3DTOP_PREMODULATE;
					// arg1 in source A
					NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG1, NV2A_colorArg1[i]);
					//NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG2, NV2A_colorArg2[i]);
				}
				// D3DTOP_MODULATEALPHA_ADDCOLOR, A=Arg1, B=1,  C =alpha(Arg1) CFLAG_ALPHAREPLICATE set,  D =Arg2
				else if (((colorICW & 0xF0FFF0F0) == 0x00201000) && (convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF) == convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 8) & 0xF))) {
					NV2A_colorOP[i] = xbox::X_D3DTOP_MODULATEALPHA_ADDCOLOR;
					// arg1 in source A
					NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
					// arg2 in source D
					NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW) & 0xF);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG1, NV2A_colorArg1[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG2, NV2A_colorArg2[i]);
				}
				
				// D3DTOP_MODULATECOLOR_ADDALPHA, A=Arg1, B=Arg2,  C =alpha(Arg1) CFLAG_ALPHAREPLICATE set,  D =1
				else if (((colorICW & 0xF0F0F0FF) == 0x00001020) && (convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF) == convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 8) & 0xF))) {

					NV2A_colorOP[i] = xbox::X_D3DTOP_MODULATECOLOR_ADDALPHA;
					// arg1 in source A
					NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
					// arg2 in source D
					NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 16) & 0xF);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG1, NV2A_colorArg1[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG2, NV2A_colorArg2[i]);
				}
				
				// D3DTOP_MODULATEINVALPHA_ADDCOLOR, A=Arg1, B=1,  C =alpha(1-Arg1) CFLAG_ALPHAREPLICATE|CFLAG_COMPLEMENT ,  D =Arg2
				// // A=Arg1, B=1, C=alpha(1-Arg1), D=Arg2
				else if (((colorICW & 0xF0FFF0F0) == 0x00203000) && (convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF) == convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 8) & 0xF))) {
					NV2A_colorOP[i] = xbox::X_D3DTOP_MODULATEINVALPHA_ADDCOLOR;
					// arg1 in source A
					NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
					// arg2 in source D
					NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW) & 0xF);
				}
				
				// D3DTOP_MODULATEINVCOLOR_ADDALPHA, A=1-Arg1 CFLAG_COMPLEMENT, B=Arg2,  C =alpha(Arg1) CFLAG_ALPHAREPLICATE,  D =1
				//A=(1-arg1), B=Arg2, C=alpha(arg1), D=1
				else if (((colorICW & 0xF0F0F0FF) == 0x20001020) && (convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF) == convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 8) & 0xF))) {

					NV2A_colorOP[i] = xbox::X_D3DTOP_MODULATEINVCOLOR_ADDALPHA;
					// arg1 in source A
					NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
					// arg2 in source D
					NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 16) & 0xF);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG1, NV2A_colorArg1[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG2, NV2A_colorArg2[i]);
				}
				
				// D3DTOP_DOTPRODUCT3, A=Arg1 CFLAG_EXPANDNORMAL set, B=Arg2 CFLAG_EXPANDNORMAL set,  C and D default 0
				// OCW |= NV097_SET_COMBINER_COLOR_OCW_AB_DOT_ENABLE_TRUE| NV097_SET_COMBINER_COLOR_OCW_CD_DOT_ENABLE_FALSE | NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_AB_AB_DST_ENABLE | NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_CD_DISABLE
				//A=expanded(arg1)CFLAG_EXPANDNORMAL,B=expanded(arg1) CFLAG_EXPANDNORMAL, C=0, D=0
				//ocw = (ocw | 0x820000u) >> 4; alphaICW=0, alphaOCW=0
				else if (((colorICW & 0xF0F0FFFF) == 0x40400000) && ((colorOCW & 0x00082000) == 0x82000) && (alphaICW == 0) && (alphaOCW == 0)) {

					NV2A_colorOP[i] = xbox::X_D3DTOP_DOTPRODUCT3;
					// arg1 in source A
					NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
					// arg2 in source B
					NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 16) & 0xF);
					// result arg in OCW AB dst
					NV2A_resultArg[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorOCW >> 4) & 0xF);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG1, NV2A_colorArg1[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG2, NV2A_colorArg2[i]);
					//disable alphaOP in this stage. althou the alphaICW is 0 already.
					NV2A_alphaOP[i] = xbox::X_D3DTOP_DISABLE;
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAOP, NV2A_alphaOP[i]);
				}
				// D3DTOP_LERP, 
				//A=arg0, B=Arg1, C=1-arg0 CFLAG_COMPLEMENT, D=Arg2
				else if (((colorICW & 0xF0F0F0F0)== 0x00002000) && ((colorICW >> 24) & 0xF) == ((colorICW >> 8) & 0xF)) {

					NV2A_colorOP[i] = xbox::X_D3DTOP_LERP;
					// arg0 in source A
					NV2A_colorArg0[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
					// arg1 in source B
					NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 16) & 0xF);
					// arg2 in source D
					NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW) & 0xF);
					
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG0, NV2A_colorArg0[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG1, NV2A_colorArg1[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG2, NV2A_colorArg2[i]);
				}
				// D3DTOP_BUMPENVMAP/D3DTOP_BUMPENVMAPLUMINANCE, A=D3DTA_CURRENT,B=1, C and D default 0
				else if (((colorICW & 0xFFFFFFFF) == 0x01200000) ) {
					// FIXME!!! we can't tell D3DTOP_BUMPENVMAP/D3DTOP_BUMPENVMAPLUMINANCE here!. need a way.
					NV2A_colorOP[i] = xbox::X_D3DTOP_BUMPENVMAP;
					// arg1 in source A
					NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
					//NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG0, NV2A_colorArg0[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG1, NV2A_colorArg1[i]);
					//NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG2, NV2A_colorArg2[i]);
				}
				// D3DTOP_MULTIPLYADD, A=Arg0,B=1, C=Arg1, D=Arg2
				// FIXME!!! condition too simple, put this close to the end.
				else if ((colorICW & 0xF0FFF0F0) == 0x00200000) {

					NV2A_colorOP[i] = xbox::X_D3DTOP_MULTIPLYADD;
					// arg0 in source A
					NV2A_colorArg0[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
					// arg1 in source C
					NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 8) & 0xF);
					// arg2 in source D
					NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW) & 0xF);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG0, NV2A_colorArg0[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG1, NV2A_colorArg1[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG2, NV2A_colorArg2[i]);
				}
				// unknown condition, set to disable
				else {
					NV2A_colorOP[i] = xbox::X_D3DTOP_DISABLE;
					//FIXME!!! shall we continue loop here?
				}
				// xbox convert stage 0 colorOP==X_D3DTOP_DISABLE to selec arg1 with arg1=current, and in stage 0, current==diffuse.
				/*
				if (i == 0) {
					if (NV2A_colorArg0[i] == xbox::X_D3DTA_DIFFUSE) {
						NV2A_colorArg0[i] = xbox::X_D3DTA_CURRENT;
						NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG0, NV2A_colorArg0[i]);
					}
					if (NV2A_colorArg1[i] == xbox::X_D3DTA_DIFFUSE) {
						NV2A_colorArg1[i] = xbox::X_D3DTA_CURRENT;
						NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG1, NV2A_colorArg1[i]);
					}
					if (NV2A_colorArg2[i] == xbox::X_D3DTA_DIFFUSE) {
						NV2A_colorArg2[i] = xbox::X_D3DTA_CURRENT;
						NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG2, NV2A_colorArg2[i]);
					}
				}
				*/
			}
		}
		
		NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLOROP, NV2A_colorOP[i]);
		// FIXME!!! can we really continue the loop when colorOP == X_D3DTOP_DISABLE?
		/*
		// invalid alphaOP list
		D3DTOP_MODULATEALPHA_ADDCOLOR
		D3DTOP_MODULATECOLOR_ADDALPHA
		D3DTOP_MODULATEINVALPHA_ADDCOLOR
		D3DTOP_MODULATEINVCOLOR_ADDALPHA
		D3DTOP_DOTPRODUCT3
		D3DTOP_BUMPENVMAP
		D3DTOP_BUMPENVMAPLUMINANCE
		*/
		if (NV2A_alphaOP[i] == xbox::X_D3DTOP_DISABLE) {
			;//NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAOP, NV2A_alphaOP[i]);;
		}
		else {
			// now we process alphaOP
			if (NV2A_alphaOP[i] > xbox::X_D3DTOP_LAST) {
				// alphaOP SelectARG1  A=arg1, B=1
				if (((alphaICW & 0xF0FFFFFF) == 0x10200000) && ((alphaOCW & 0xFFFF00FF) == 0)) {
					NV2A_alphaOP[i] = xbox::X_D3DTOP_SELECTARG1;
					// arg1 always source A
					NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);// FIXME!!!,  reg 4 is D3DTA_DIFFUSE :0, not sure this direct shift is correct or not6
					//NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG0, NV2A_alphaArg0[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG1, NV2A_alphaArg1[i]);
					//NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG2, NV2A_alphaArg2[i]);

				}
				// alphaOP SelectARG2 A=0, B=0, C=1, D=arg2
				else if (((alphaICW & 0xFFFFFFF0) == 0x00002010) && ((alphaOCW & 0xFFFF00FF) == 0)) {
					NV2A_alphaOP[i] = xbox::X_D3DTOP_SELECTARG2;
					// arg2 always source D
					NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW) & 0xF);// FIXME!!!,  reg 4 is D3DTA_DIFFUSE :0, not sure this direct shift is correct or not6
					//NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG0, NV2A_alphaArg0[i]);
					//NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG1, NV2A_alphaArg1[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG2, NV2A_alphaArg2[i]);

				}
				// OP modulate, A*B, C==0, D==0, A=Arg1, B=Arg2, 
				else if ((alphaICW & 0xE0E00FFF) == 0x0000000) {
					// arg1 in source A
					NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
					// arg2 in source B
					NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 16) & 0xF);
					//NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG0, NV2A_alphaArg0[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG1, NV2A_alphaArg1[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG2, NV2A_alphaArg2[i]);

					if ((alphaOCW & 0x20000) != 0) {
						NV2A_alphaOP[i] = xbox::X_D3DTOP_MODULATE4X;
					}
					else if ((alphaOCW & 0x10000) != 0) {
						NV2A_alphaOP[i] = xbox::X_D3DTOP_MODULATE2X;
					}
					else {
						NV2A_alphaOP[i] = xbox::X_D3DTOP_MODULATE;
					}
				}
				// OP ADD, source A + D, A == Arg1, B == 1, C == 1, D == Arg2
				else if ((alphaICW & 0xF0FFFFF0) == 0x10202010) {
					NV2A_alphaOP[i] = xbox::X_D3DTOP_ADD;
					if ((alphaOCW & 0x18000) == 0x8000)
						NV2A_alphaOP[i] = xbox::X_D3DTOP_ADDSIGNED;
					if ((alphaOCW & 0x18000) == 0x18000)
						NV2A_alphaOP[i] = xbox::X_D3DTOP_ADDSIGNED2X;
					// arg1 in source A
					NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
					// arg2 in source D
					NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW) & 0xF);
					//NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG0, NV2A_alphaArg0[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG1, NV2A_alphaArg1[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG2, NV2A_alphaArg2[i]);

				}
				// D3DTOP_SUBTRACT,  source A - D, A == Arg1, B == 1, C == -1, D == Arg2
				else if ((alphaICW & 0xF0FFFFF0) == 0x10204010) {
					NV2A_alphaOP[i] = xbox::X_D3DTOP_SUBTRACT;
					// arg1 in source A
					NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
					// arg2 in source D
					NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW) & 0xF);
					//NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG0, NV2A_alphaArg0[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG1, NV2A_alphaArg1[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG2, NV2A_alphaArg2[i]);

				}
				// D3DTOP_ADDSMOOTH, A+C*D, A=Arg1, B=1, C=1-Arg1, D=Arg2
				// verify flags of A/B/C/D, B=reg0, reg(A)==reg(C)
				else if (((alphaICW & 0xF0FFF0F0) == 0x10203010) && (convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF) == convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 8) & 0xF))) {
					NV2A_alphaOP[i] = xbox::X_D3DTOP_ADDSMOOTH;
					// arg1 in source A
					NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
					// arg2 in source D
					NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW) & 0xF);
					//NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG0, NV2A_alphaArg0[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG1, NV2A_alphaArg1[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG2, NV2A_alphaArg2[i]);

				}
				// D3DTOP_BLENDDIFFUSEALPHA/D3DTOP_BLENDCURRENTALPHA/D3DTOP_BLENDTEXTUREALPHA/D3DTOP_BLENDFACTORALPHA,
				// A+C*D, A=Arg1, B=alpha(alphaOP-12) and CFLAG_ALPHAREPLICATE set , C=alpht(1-(alphaOP-12)) CFLAG_COMPLEMENT|CFLAG_ALPHAREPLICATE set , D=Arg2
				// verify flags of A/B/C/D, B=reg0, reg(A)==reg(C)
				// todo: could be colorOP only or alphaOP only?
				else if (((alphaICW & 0xF0F0F0F0) == 0x10103010) && (convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 16) & 0xF) == convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 8) & 0xF))) {
					NV2A_alphaOP[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 16) & 0xF) + xbox::X_D3DTOP_BLENDDIFFUSEALPHA;
					// arg1 in source A
					NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
					// arg2 in source D
					NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW) & 0xF);
					//NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG0, NV2A_alphaArg0[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG1, NV2A_alphaArg1[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG2, NV2A_alphaArg2[i]);

				}
				// D3DTOP_BLENDTEXTUREALPHAPM, A+C*D, A=Arg1, B=1, C=alpha(1-D3DTA_TEXTURE) CFLAG_COMPLEMENT | CFLAG_ALPHAREPLICATE, D=Arg2,
				// todo: could be colorOP only or alphaOP only?
				else if (((alphaICW & 0xF0FFFFF0) == 0x00203200)) {
					NV2A_alphaOP[i] = xbox::X_D3DTOP_BLENDTEXTUREALPHAPM;
					// arg1 in source A
					NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
					// arg2 in source D
					NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW) & 0xF);
					//NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG0, NV2A_alphaArg0[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG1, NV2A_alphaArg1[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG2, NV2A_alphaArg2[i]);

				}
				// D3DTOP_PREMODULATE, A=Arg1, if stage 0, B==D3DTA_TEXTURE, if stage1 or stage2 B=1, C and D default to 0
				else if((i > 0 && i < 3 && ((alphaICW & 0xF0FFFFFF) == 0x10200000)) || (i == 0 && ((alphaICW & 0xF0FFFFFF) == 0x10120000))) {
					NV2A_alphaOP[i] = xbox::X_D3DTOP_PREMODULATE;
					// arg1 in source A
					NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
					//NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG0, NV2A_alphaArg0[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG1, NV2A_alphaArg1[i]);
					//NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG2, NV2A_alphaArg2[i]);

				}
				/* // colorOP only
				// D3DTOP_MODULATEALPHA_ADDCOLOR, A=Arg1, B=1,  C =alpha(Arg1) CFLAG_ALPHAREPLICATE set,  D =Arg2
				else if ((alphaICW & 0xF0FFF0F0 == 0x00201000) && (convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF) == convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 8) & 0xF))) {
					NV2A_alphaOP[i] = xbox::X_D3DTOP_MODULATEALPHA_ADDCOLOR;
					// arg1 in source A
					NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
					// arg2 in source D
					NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW) & 0xF);
				}
				*/
				/* // colorOP only
				// D3DTOP_MODULATECOLOR_ADDALPHA, A=Arg1, B=Arg2,  C =alpha(Arg1) CFLAG_ALPHAREPLICATE set,  D =1
				else if (((alphaICW & 0xF0F0F0FF) == 0x00001020) && (convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF) == convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 8) & 0xF))) {

					NV2A_alphaOP[i] = xbox::X_D3DTOP_MODULATECOLOR_ADDALPHA;
					// arg1 in source A
					NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
					// arg2 in source D
					NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 16) & 0xF);
				}
				*/
				/* // colorOP only
				// D3DTOP_MODULATEINVALPHA_ADDCOLOR, A=Arg1, B=1,  C =alpha(Arg1) CFLAG_ALPHAREPLICATE CFLAG_COMPLEMENT set,  D =Arg2
				else if ((alphaICW & 0xF0FFF0F0 == 0x00203000) && (convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF) == convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 8) & 0xF))) {
					NV2A_alphaOP[i] = xbox::X_D3DTOP_MODULATEINVALPHA_ADDCOLOR;
					// arg1 in source A
					NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
					// arg2 in source D
					NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW) & 0xF);
				}
				*/
				/*
				// D3DTOP_MODULATEINVCOLOR_ADDALPHA, A=Arg1, B=Arg2,  C =alpha(Arg1) CFLAG_ALPHAREPLICATE CFLAG_COMPLEMENT set,  D =1
				else if (((alphaICW & 0xF0F0F0FF) == 0x00003020) && (convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF) == convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 8) & 0xF))) {

					NV2A_alphaOP[i] = xbox::X_D3DTOP_MODULATEINVCOLOR_ADDALPHA;
					// arg1 in source A
					NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
					// arg2 in source D
					NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 16) & 0xF);
				}
				*/
				/* // colorOP only
				// D3DTOP_DOTPRODUCT3, A=Arg1 CFLAG_EXPANDNORMAL set, B=Arg2 CFLAG_EXPANDNORMAL set,  C and D default 0
				// OCW |= NV097_SET_COMBINER_alpha_OCW_AB_DOT_ENABLE_TRUE| NV097_SET_COMBINER_alpha_OCW_CD_DOT_ENABLE_FALSE | NV097_SET_COMBINER_alpha_OCW_BLUETOALPHA_AB_AB_DST_ENABLE | NV097_SET_COMBINER_alpha_OCW_BLUETOALPHA_CD_DISABLE
				// only for color OP
				else if ((alphaICW & 0xF0F00000 == 0x40400000) && (alphaOCW & 0x00082000 == 0x82000)) {

					NV2A_alphaOP[i] = xbox::X_D3DTOP_DOTPRODUCT3;
					// arg1 in source A
					NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
					// arg2 in source D
					NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 16) & 0xF);
					// result arg in OCW AB dst
					NV2A_resultArg[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorOCW >> 4) & 0xF);
				}
				*/
				// D3DTOP_LERP, A=Arg0,B=Arg1, C=(1-Arg0) CFLAG_COMPLEMENT set, D=Arg2
				else if (((alphaICW & 0xF0F0F0F0) == 0x10103010) && ((alphaICW >> 24) & 0xF) == ((alphaICW >> 8) & 0xF)) {
					NV2A_alphaOP[i] = xbox::X_D3DTOP_LERP;
					// arg0 in source A
					NV2A_alphaArg0[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
					// arg1 in source B
					NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 16) & 0xF);
					// arg2 in source D
					NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW) & 0xF);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG0, NV2A_alphaArg0[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG1, NV2A_alphaArg1[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG2, NV2A_alphaArg2[i]);

				}
				// D3DTOP_BUMPENVMAP/D3DTOP_BUMPENVMAPLUMINANCE, A=D3DTA_CURRENT,B=1, C and D default 0
				/* // only for color OP
				else if ((alphaICW & 0xF0FFFFFF == 0x00200000) && convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF) == xbox::X_D3DTA_CURRENT) {
					// FIXME!!! we can't tell D3DTOP_BUMPENVMAP/D3DTOP_BUMPENVMAPLUMINANCE here!. need a way to tell the difference.
					NV2A_alphaOP[i] = xbox::X_D3DTOP_BUMPENVMAP;
					// arg1 in source A
					NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
				}
				*/
				// D3DTOP_MULTIPLYADD, A=Arg0,B=1, C=Arg1, D=Arg2
				// FIXME!!! condition too simple. so this case is arranged here almost in very last.
				else if (((alphaICW & 0xF0FFF0F0) == 0x10201010)) {

					NV2A_alphaOP[i] = xbox::X_D3DTOP_MULTIPLYADD;
					// arg0 in source A
					NV2A_alphaArg0[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
					// arg1 in source C
					NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 8) & 0xF);
					// arg2 in source D
					NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW) & 0xF);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG0, NV2A_alphaArg0[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG1, NV2A_alphaArg1[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG2, NV2A_alphaArg2[i]);

				}
				// unknown condition, set to disable
				else {
					NV2A_alphaOP[i] = xbox::X_D3DTOP_DISABLE;
				}
				// xbox convert stage 0 colorOP==X_D3DTOP_DISABLE to selec arg1 with arg1=current, and in stage 0, current==diffuse.
				if (i == 0) {
					if (NV2A_alphaArg0[i] == 0) {
						NV2A_alphaArg0[i] = 1;
						NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG0, NV2A_alphaArg0[i]);
					}
					if (NV2A_alphaArg1[i] == 0) {
						NV2A_alphaArg1[i] = 1;
						NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG0, NV2A_alphaArg1[i]);
					}
					if (NV2A_alphaArg2[i] == 0) {
						NV2A_alphaArg2[i] = 1;
						NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG0, NV2A_alphaArg2[i]);
					}
				}

			}
		}
		NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAOP, NV2A_alphaOP[i]);

		NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG0, NV2A_colorArg0[i]);
		NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG0, NV2A_alphaArg0[i]);
		NV2ATextureStates.Set(stage, xbox::X_D3DTSS_RESULTARG, NV2A_resultArg[i]);
	}

}

extern float g_Xbox_ScreenSpaceOffset_x; //tmp glue
extern float g_Xbox_ScreenSpaceOffset_y; //tmp glue
extern void CxbxrImpl_GetTransform(	xbox::X_D3DTRANSFORMSTATETYPE State,	D3DMATRIX *pMatrix);
extern void GetRenderTargetBaseDimensions(float& x, float& y); //tmp glue
extern float g_Xbox_BackbufferScaleX;
extern float g_Xbox_BackbufferScaleY;
extern xbox::X_D3DSurface           *g_pXbox_RenderTarget;
extern xbox::X_D3DSurface           *g_pXbox_BackBufferSurface;
extern void CxbxrGetSuperSampleScaleXY(NV2AState* ,float& x, float& y);
extern void CxbxrGetScreenSpaceOffsetXY(float& x, float& y);
extern void CxbxrSetViewport(xbox::X_D3DVIEWPORT8& Viewport);
extern void CxbxrGetViewport(xbox::X_D3DVIEWPORT8& Viewport);
extern void CxbxrSetWNearFarInverseWFar(float& WNear, float& WFar, float& WInverseWFar);
extern void CxbxrGetWNearFarInverseWFar(float& WNear, float& WFar, float& WInverseWFar);
extern float CxbxrGetZScale(void);
extern void GetRenderTargetRawDimensions(float& x, float& y, xbox::X_D3DSurface* rt);
xbox::X_D3DVIEWPORT8 HLEViewport;
bool NV2A_viewport_dirty = false;
void pgraph_ComposeViewport(NV2AState *d)
{
	PGRAPHState *pg = &d->pgraph;
	HRESULT hRet;
	xbox::X_D3DVIEWPORT8 Viewport;

	float fm11, fm22, fm33, fm41, fm42, fm43, clipNear, clipFar;
	float SuperSampleScale, ScaleX, ScaleY, ScaleZ, ScreenSpaceOffsetX, ScreenSpaceOffsetY;
	//float aaOffsetX, aaOffsetY;
	
	CxbxrGetSuperSampleScaleXY(d,ScaleX, ScaleY);
	if (ScaleX <= 0)ScaleX = 1.0;
	if (ScaleY <= 0)ScaleY = 1.0;
	SuperSampleScale = MIN(ScaleX, ScaleY);

	CxbxrGetScreenSpaceOffsetXY(ScreenSpaceOffsetX, ScreenSpaceOffsetY);

	float  xViewport, yViewport;
	clipNear = pg->KelvinPrimitive.SetClipMin;
	clipFar = pg->KelvinPrimitive.SetClipMax;

	extern float g_ZScale;// TMP glue

	ScaleZ=g_ZScale;
	// FIXME!! xbox d3d SetViewport always  SetScissors(0,0,NULL) with the Viewport as sciccor RECT.
	// shall we get Viewport.X/Y/Width/Height via pg->KelvinPrimitive.SetWindowClipHorizontal/SetWindowClipVertical, the conversion should be more straight forward.
	// question will be, how to tell that the scrssor Rect is set by viewport or by other SetScissor() call?
	if (ScaleZ < 1.0)ScaleZ = 1.0;
	// NV2A viewport content depends on vertex shader mode
	// FIXME!! when swith from fixed mode to pass through, the shader mode was changed before viewport info was update. so we can rely on shader mode.
	// here we detect whether KelvinPrimitive.SetViewportOffset[2]==0.0 and KelvinPrimitive.SetViewportOffset[3]==0.0 instead
	float rendertargetBaseWidth;
	float rendertargetBaseHeight;

	float WNear=0.0, WFar=1.0, InverseWFar=1.0;
    // set default W parameters.
	CxbxrSetWNearFarInverseWFar(WNear, WFar, InverseWFar);

	//GetRenderTargetBaseDimensions(rendertargetBaseWidth, rendertargetBaseHeight);
	GetRenderTargetRawDimensions(rendertargetBaseWidth, rendertargetBaseHeight, g_pXbox_RenderTarget);
	if (g_Xbox_VertexShaderMode == VertexShaderMode::FixedFunction) {
	//if ((float)pg->KelvinPrimitive.SetViewportOffset[2] == 0.0 && (float)pg->KelvinPrimitive.SetViewportOffset[3] == 0.0) {
		xViewport = pg->KelvinPrimitive.SetViewportOffset[0];
		yViewport = pg->KelvinPrimitive.SetViewportOffset[1];
		// if use_w
		if ((pg->KelvinPrimitive.SetDepthTestEnable != D3DZB_FALSE) && (pg->KelvinPrimitive.SetZMinMaxControl== D3DZB_USEW)) {
			ScaleZ = clipFar;
			g_ZScale = ScaleZ;
			// FIXME !! need WNear *InverseWFar input
			// clipNear = pDevice->m_ZScale * pDevice->m_WNear * pDevice->m_InverseWFar;  pDevice->m_WNear * pDevice->m_InverseWFar=clipNear/pDevice->m_ZScale;
			
			float WNear, WFar, InverseWFar;
			// WNear=0.0, WFar=1.0,WInverseWFar=0.0
			if (clipNear != 0.0f) {
				;// TODO:no feasible way to compose WNear and WFar, we need these two vars to calculate Projection Matrix.
			}

			//Viewport.MinZ = ScaleZ /(clipNear / ScaleZ); //was ScaleZ/(WNear *InverseWFar);
			
		}
		else {
			Viewport.MinZ = clipNear / ScaleZ;
			Viewport.MaxZ = clipFar / ScaleZ;
		}
		//FIXME!! Viewport.Width/Height are not set in Fixed Mode. xbox d3d set it to 0x7FFFFFFF,
		// CxbxrImpl_SetViewport() will cap the Width/Height to render target width/height. anyway, we cap them to render target value here.
		
		Viewport.Width=rendertargetBaseWidth;
		Viewport.Height = rendertargetBaseHeight;


	}
	else {
		ScaleZ = clipFar;
		g_ZScale = ScaleZ;

		/*
		Push4f(pPush, 
                   NV097_SET_VIEWPORT_OFFSET(0), 
                   fm41 + xViewport, 
                   fm42 + yViewport, 
                   fm43, 
                   0.0f);
        
        Push4f(pPush + 5,
                   NV097_SET_VIEWPORT_SCALE(0), 
                   fm11, 
                   fm22, 
                   fm33, 
                   0.0f);
	    */
		fm11 = pg->KelvinPrimitive.SetViewportScale[0];
		fm22 = pg->KelvinPrimitive.SetViewportScale[1];
		fm33 = pg->KelvinPrimitive.SetViewportScale[2];
		fm41 = fm11;
		fm42 = -fm22;
		fm43 = pg->KelvinPrimitive.SetViewportOffset[2];

		xViewport = pg->KelvinPrimitive.SetViewportOffset[0] - fm41;
		yViewport = pg->KelvinPrimitive.SetViewportOffset[1] - fm42;

		/*
		    FLOAT fm11 = 0.5f * pDevice->m_Viewport.Width 
                              * pDevice->m_SuperSampleScaleX;
            FLOAT fm41 = fm11;
        
            FLOAT fm22 = -0.5f * pDevice->m_Viewport.Height 
                               * pDevice->m_SuperSampleScaleY;
            FLOAT fm42 = -fm22;
        
            FLOAT fm33 = pDevice->m_ZScale * (pDevice->m_Viewport.MaxZ - 
                                              pDevice->m_Viewport.MinZ);
            FLOAT fm43 = pDevice->m_ZScale * (pDevice->m_Viewport.MinZ);
		*/


		Viewport.MinZ = fm43 / ScaleZ;
		Viewport.MaxZ = (fm33 / ScaleZ) + Viewport.MinZ;
		Viewport.Height = fm22 / (-0.5*ScaleY);
		Viewport.Width = fm11 / (0.5*ScaleX);


	}
	// if ((pDevice->m_StateFlags & STATE_MULTISAMPLING) &&	(D3D__RenderState[D3DRS_MULTISAMPLEANTIALIAS]))
	// (pg->KelvinPrimitive.SetSurfaceFormat&(NV097_SET_SURFACE_FORMAT_ANTI_ALIASING_CENTER_CORNER_2| NV097_SET_SURFACE_FORMAT_ANTI_ALIASING_SQUARE_OFFSET_4))!=0 means we're in STATE_MULTISAMPLING
	//if ((GET_MASK(pg->KelvinPrimitive.SetSurfaceFormat, NV097_SET_SURFACE_FORMAT_ANTI_ALIASING)&(NV097_SET_SURFACE_FORMAT_ANTI_ALIASING_CENTER_CORNER_2| NV097_SET_SURFACE_FORMAT_ANTI_ALIASING_SQUARE_OFFSET_4))!=0)
		//&&(XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_MULTISAMPLEANTIALIAS)!=0))
	if((pg->KelvinPrimitive.SetAntiAliasingControl & NV097_SET_ANTI_ALIASING_CONTROL_ENABLE_TRUE) !=0)
	{
		xViewport += 0.5f;
		yViewport += 0.5f;
	}
	/*
	FLOAT xViewport = pDevice->m_Viewport.X * pDevice->m_SuperSampleScaleX
                    + pDevice->m_ScreenSpaceOffsetX;
    FLOAT yViewport = pDevice->m_Viewport.Y * pDevice->m_SuperSampleScaleY
                    + pDevice->m_ScreenSpaceOffsetY;
	*/
	Viewport.X = (xViewport - ScreenSpaceOffsetX) / ScaleX;
	Viewport.Y = (yViewport - ScreenSpaceOffsetX) / ScaleY;
	
	extern xbox::X_D3DVIEWPORT8 HLE_Viewport;
	extern xbox::X_D3DVIEWPORT8 g_Xbox_Viewport;
	//if (Viewport.Width != HLE_Viewport.Width || Viewport.Height != Viewport.Height || Viewport.X != HLE_Viewport.X || Viewport.Y != HLE_Viewport.Y)
	//if(refViewport.Width!=0x7FFFFFFF&& refViewport.Width!=0)
	    //assert(Viewport.Width == refViewport.Width);
	//if (refViewport.Height != 0x7FFFFFFF && refViewport.Height)
	    //assert(Viewport.Height == refViewport.Height!=0);
	//assert(Viewport.X == g_Xbox_Viewport.X);
	//assert(Viewport.Y == g_Xbox_Viewport.Y);
	CxbxrSetViewport(Viewport);
	//CxbxrImpl_SetViewport(&Viewport);
}

void CxbxrImpl_LazySetPointParameters(NV2AState* d)
{
	PGRAPHState* pg = &d->pgraph;
	HRESULT hRet;
	    // the LazySetPointParameters() converts the point size from float into DWORD and set it to pg->KelvinPrimitive.SetPointSize
		float FixedSize = pg->KelvinPrimitive.SetPointSize;
		// reversed to xbox d3d fixed point size
		float size = FixedSize/8.0;
		float max, min;
		float delta, factor, height;
		min = pg->KelvinPrimitive.SetPointParamsMin;
		if (min < 0.0f)min = 0.0f;
		delta = pg->KelvinPrimitive.SetPointParamsDeltaA;

		//float hostMinSize, hostMaxSize;
		//hRet = g_pD3DDevice->GetRenderState(D3DRS_POINTSIZE_MIN, (DWORD*)&hostMinSize);
		//hRet = g_pD3DDevice->GetRenderState(D3DRS_POINTSIZE_MAX, (DWORD*)&hostMaxSize);
		// D3D__RenderState[D3DRS_POINTSCALEENABLE]== false, set host point size only, disable host point scale
		if (pg->KelvinPrimitive.SetPointParamsEnable != 0) {
			max = (min + delta);

			//FIXEDME!!! we need viewport height, not sure including SuperSampleScaleY is correct or not.
			xbox::X_D3DVIEWPORT8 Viewport;
			if (NV2A_viewport_dirty == true) {
				// this only compose viewport from pgraph content 
				pgraph_ComposeViewport(d);
				// todo: actually setviewport with the composed viewport, currently we don't set the host viewport via pgraph content, yet. the SetViewport() is currently HLEed and not processed in pushbuffer.
				// D3DDevice_SetViewport() was patched and HLEed, here we only implement it when we're in RunPushbuffer().
				if ((NV2A_stateFlags & X_STATE_RUNPUSHBUFFERWASCALLED) != 0) {
					
					CxbxrGetViewport(Viewport);
					// our viewport will be update to host later.
					//CxbxrImpl_SetViewport(&Viewport);
				}
				NV2A_viewport_dirty = false;
			}
			CxbxrGetViewport(Viewport);
			height = Viewport.Height;
			factor = delta / (size * height);
			factor *= factor;
			float xboxScaleA = pg->KelvinPrimitive.SetPointParamsFactorMulA / factor;
			float xboxScaleB = pg->KelvinPrimitive.SetPointParamsFactorMulB / factor;
			float xboxScaleC = pg->KelvinPrimitive.SetPointParamsFactorMulC / factor;
			// assume host d3d uses the same min/max point size as xbox d3d
			NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_POINTSIZE_MIN, FtoDW(min));
			NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_POINTSIZE_MAX, FtoDW(max));
			/// set host point scale A/B/C
			NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_POINTSCALE_A, FtoDW(xboxScaleA));
			NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_POINTSCALE_B, FtoDW(xboxScaleB));
			NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_POINTSCALE_C, FtoDW(xboxScaleC));
		}
		else {
			max = 64.0;
			float ssScale = CxbxrGetSuperSampleScale();
			size /= ssScale;
		}
		// clamp size to min/max HostPointSize range
		if (size < min)size=min;
		// if (size > 64.0f)size = 64.0f; 
		if (size > max)size = max;// max already clamped to 64.0
		NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_POINTSCALEENABLE, pg->KelvinPrimitive.SetPointParamsEnable != 0);
		NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_POINTSPRITEENABLE, pg->KelvinPrimitive.SetPointSmoothEnable != 0);
		// set host fixed point size
		NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_POINTSIZE, FtoDW(size));
}

// NV2A transform matrices to be used as alternatives of xbox direct modelview matrices. these matrices will be used in UpdateVertexShaderStatesAndConstants() for fixed mode vertex shader.
D3DMATRIX g_NV2A_transform_ModelView;
D3DMATRIX g_NV2A_transform_InverseModelView;
D3DMATRIX g_NV2A_transform_Composite;
D3DMATRIX g_NV2A_transform_ViewportTransform;
D3DMATRIX g_NV2A_transform_ProjectionViewportTransform;
D3DMATRIX g_NV2A_DirectModelView_InverseWorldViewTransposed;
D3DMATRIX g_NV2A_DirectModelView_View;
D3DMATRIX g_NV2A_DirectModelView_InverseView;
D3DMATRIX g_NV2A_DirectModelView_World;
D3DMATRIX g_NV2A_DirectModelView_Projection;

void CxbxrImpl_GetViewportTransform(NV2AState* d)
{
	PGRAPHState* pg = &d->pgraph;
	HRESULT hRet;
	// compose viewport parameters via pgraph contents if needed
	// update Viewport info if needed. though this should already been updated before we got here.
	if (NV2A_viewport_dirty == true) {
		// this only compose viewport from pgraph content 
		pgraph_ComposeViewport(d);
		// todo: actually setviewport with the composed viewport, currently we don't set the host viewport via pgraph content, yet. the SetViewport() is currently HLEed and not processed in pushbuffer.
		//xbox::X_D3DVIEWPORT8 Viewport;
		//CxbxrGetViewport(Viewport);
		//CxbxrImpl_SetViewport(&Viewport);
	}
	NV2A_viewport_dirty = false;

	xbox::X_D3DVIEWPORT8 Viewport;
	CxbxrGetViewport(Viewport);

	D3DMATRIX viewportTransform;
	float fm11, fm22, fm33, fm41, fm42, fm43, fm44;
	float SuperSampleScaleX, SuperSampleScaleY,ZScale, ScreenSpaceOffsetX, ScreenSpaceOffsetY;
	
	CxbxrGetSuperSampleScaleXY(d,SuperSampleScaleX, SuperSampleScaleY);
	if (SuperSampleScaleX <= 0)SuperSampleScaleX = 1.0;
	if (SuperSampleScaleY <= 0)SuperSampleScaleY = 1.0;
	ZScale=CxbxrGetZScale();
	if (ZScale < 1.0)ZScale = 1.0;
	
	CxbxrGetScreenSpaceOffsetXY(ScreenSpaceOffsetX, ScreenSpaceOffsetY);

	// This is based on clipX = -1.0, clipY = 1.0

	fm11 = 0.5f * SuperSampleScaleX * Viewport.Width;
	fm41 = fm11;

	fm22 = -0.5f * SuperSampleScaleY * Viewport.Height;
	fm42 = -fm22;

	fm33 = ZScale * (Viewport.MaxZ - Viewport.MinZ);
	fm43 = ZScale * Viewport.MinZ;

	float WNear=0.0, WFar=1.0, InverseWFar=1.0;
	CxbxrGetWNearFarInverseWFar( WNear,  WFar, InverseWFar);

	//if (D3D__RenderState[D3DRS_ZENABLE] == D3DZB_USEW)
	if ((pg->KelvinPrimitive.SetDepthTestEnable != D3DZB_FALSE) && (pg->KelvinPrimitive.SetZMinMaxControl & NV097_SET_ZMIN_MAX_CONTROL_CULL_NEAR_FAR_EN_TRUE) == 0)
	{
		// We are doing w-buffering - scale w to optimize use of buffer:
		//     ZScale * InverseWFar=clipNear/WNear; we need WNear. if WNear==0.0, then this is not feasible.
		InverseWFar = 1.0; // TODO: we can't compose WNear, so use default 1.0 for InverseWFar here, this is a hack.
		fm44 = ZScale * InverseWFar;

		// Scale x, y, and z similarly to negate the effect of dividing
		// through by a scaled w:

		fm11 *= fm44;  fm22 *= fm44;  fm33 *= fm44;
		fm41 *= fm44;  fm42 *= fm44;  fm43 *= fm44;
	}
	else
	{
		fm44 = 1.0f;
	}

	ZeroMemory(&viewportTransform, sizeof(viewportTransform));

	viewportTransform._11 = fm11;
	viewportTransform._22 = fm22;
	viewportTransform._33 = fm33;
	viewportTransform._41 = fm41;
	viewportTransform._42 = fm42;
	viewportTransform._43 = fm43;
	viewportTransform._44 = fm44;

	// update cached ViewportTransform Matrix.
	g_NV2A_transform_ViewportTransform = viewportTransform;
}

void CxbxrImpl_LazySetTransform(NV2AState* d)
{
	PGRAPHState* pg = &d->pgraph;
	HRESULT hRet;


	// SetModelView() call only be calaled when pg->KelvinPrimitive.SetSkinMode==0.
	//
	// SetModelview() sets:
	// PushMatrixTransposed(pPush, NV097_SET_MODEL_VIEW_MATRIX0(0), pModelView);
	// PushMatrixTransposed(pPush + 17, NV097_SET_COMPOSITE_MATRIX(0), pComposite);
	// inverse modelview matrix is optional: PushInverseModelViewMatrix(pPush,NV097_SET_INVERSE_MODEL_VIEW_MATRIX0(0),pInverseModelView);
	// 
	// when pg->KelvinPrimitive.SetSkinMode==0, it's still possible that SetTransform() was called.
	// in this situation, LazySetsTransform() sets:
	// PushMatrixTransposed(pPush,NV097_SET_MODEL_VIEW_MATRIX0(0),	& modelView);
	// when TexGenInverseNeeded) ||	(D3D__RenderState[D3DRS_LIGHTING])
	// optional PushInverseModelViewMatrix(pPush,NV097_SET_INVERSE_MODEL_VIEW_MATRIX0(0),& inverseModelView);

	// this will update matrix world/view/projection using matrix ModelView and Composite
		//if (pgraph_is_ModelView_dirty()) {
			// FIXME! shall we allow the g_xbox_transform_Composite_dirty== false here? could titles assumes composite matrix could persist? all xbox d3d api update ModelView and Composite matrix in the same time.
			// update the modelview and composite matrix whenever either one matrix was dirty.
	// handle DirectModelView()
	// todo: figure out how to compose View matrix via NV2A content. here is hack using View matrix from xbox d3d.
	extern D3D8TransformState d3d8TransformState;
	g_xbox_DirectModelView_View= (D3DXMATRIX)d3d8TransformState.Transforms[xbox::X_D3DTS_VIEW];
	D3DXMatrixInverse((D3DXMATRIX*)&g_xbox_DirectModelView_InverseView, NULL, (D3DXMATRIX*)&g_xbox_DirectModelView_View);
	D3DMATRIX matUnit;
	memset(&matUnit._11, 0, sizeof(matUnit));
	matUnit._11 = 1.0;
	matUnit._22 = 1.0;
	matUnit._33 = 1.0;
	matUnit._44 = 1.0;
	// TODO: this is a hack, we set cached View  Matrix to unit matrix and leave all variables in the cached projection matrix.
	g_NV2A_DirectModelView_View = matUnit;
	// use X_D3DTS_VIEW matrix from xbox side instead of unit matrix.
	//CxbxrImpl_GetTransform(xbox::X_D3DTRANSFORMSTATETYPE::X_D3DTS_VIEW, &g_NV2A_DirectModelView_View);
	//calculate inverse matrix of X_D3DTS_VIEW
	D3DXMatrixInverse((D3DXMATRIX*)&g_NV2A_DirectModelView_InverseView, NULL, (D3DXMATRIX*)&g_NV2A_DirectModelView_View);
	D3DXMatrixTranspose((D3DXMATRIX*)&g_NV2A_transform_ModelView, (D3DXMATRIX*)&pg->KelvinPrimitive.SetModelViewMatrix[0][0]);
	D3DXMatrixTranspose((D3DXMATRIX*)&g_NV2A_transform_Composite, (D3DXMATRIX*)&pg->KelvinPrimitive.SetCompositeMatrix[0]);
	D3DXMatrixInverse((D3DXMATRIX*)&g_NV2A_transform_InverseModelView, NULL, (D3DXMATRIX*)&g_NV2A_transform_ModelView);
	//calculate world matrix
	D3DXMatrixMultiply((D3DXMATRIX*)&g_NV2A_DirectModelView_World, (D3DXMATRIX*)&g_NV2A_transform_ModelView, (D3DXMATRIX*)&g_NV2A_DirectModelView_InverseView);

#if 1 //pgraph always used modelview matrix, only difference is 
	if ((NV2A_DirtyFlags & X_D3DDIRTYFLAG_DIRECT_MODELVIEW) != 0) {
		// transpose KelvinPrimitive transform back to xbox d3d transform
		D3DXMatrixMultiply((D3DXMATRIX*)&g_NV2A_transform_ProjectionViewportTransform, (D3DXMATRIX*)&g_NV2A_transform_InverseModelView, (D3DXMATRIX*)&g_NV2A_transform_Composite);
	}
	else {
#endif
		// not in skinning mode
		if (pg->KelvinPrimitive.SetSkinMode == 0) {
			D3DXMatrixMultiply((D3DXMATRIX*)&g_NV2A_transform_ProjectionViewportTransform, (D3DXMATRIX*)&g_NV2A_transform_InverseModelView, (D3DXMATRIX*)&g_NV2A_transform_Composite);
		}
		// skinning mode, the commposite matrix doesn't include the ModelView matrix, only ViewPortTransform.
		// SetModelViewMatrix[1] was set.
		else {
			g_NV2A_transform_ProjectionViewportTransform = g_NV2A_transform_Composite; 
			D3DXMatrixMultiply((D3DXMATRIX*)&g_NV2A_transform_Composite, (D3DXMATRIX*)&g_NV2A_transform_ModelView, (D3DXMATRIX*)&g_NV2A_transform_ProjectionViewportTransform);
			//we got every thing we need already. 
			//CxbxrImpl_SetModelView(&g_NV2A_transform_ModelView, &g_NV2A_transform_InverseModelView, &g_NV2A_transform_Composite);
		}
	}

	// compose xbox side matrix for use in d3d vertex shader update.
	// update g_xbox_DirectModelView_InverseWorldViewTransposed for use in FVF mode vertex shader constant update routine
	D3DXMatrixTranspose((D3DXMATRIX*)&g_NV2A_DirectModelView_InverseWorldViewTransposed, (D3DXMATRIX*)&g_NV2A_transform_InverseModelView);
	// update g_xbox_DirectModelView_Projection from g_xbox_transform_PrjectionViewportTransform
	// set g_NV2A_transform_ViewportTransform
	CxbxrImpl_GetViewportTransform(d);

	D3DXMATRIX matInverseViewportTransform, matViewportTransform;

	D3DXMatrixInverse((D3DXMATRIX*)&matInverseViewportTransform, NULL, (D3DXMATRIX*)&g_NV2A_transform_ViewportTransform);

	D3DXMatrixMultiply((D3DXMATRIX*)&g_NV2A_DirectModelView_Projection, (D3DXMATRIX*)&g_NV2A_transform_ProjectionViewportTransform, (D3DXMATRIX*)&matInverseViewportTransform);
	// xbox GetTransform() returns the same matrix content as we calculated above.
	//CxbxrImpl_GetTransform(xbox::X_D3DTRANSFORMSTATETYPE::X_D3DTS_PROJECTION, &g_NV2A_DirectModelView_Projection);
	// clear pgraph transform matrix dirty flags.
	for (int i = 0; i < 4; i++) {
		// update InverseModelView matrix if only ModelView matrix is updated

		if ((g_NV2A_transform_ModelView_dirty[i] == true) || (g_NV2A_transform_InverseModelView_dirty[i] == false)) {
			D3DXMATRIX matModelViewTransposed;
			// InverseModelView transform in KelvinPrim is the same as xbox d3d transform, not transposed.
			// transpose ModelView back to xbox d3d matrix
			D3DXMatrixTranspose(&matModelViewTransposed, (D3DXMATRIX*)&pg->KelvinPrimitive.SetModelViewMatrix[i][0]);
			// update the InverModelView matrix
			D3DXMatrixInverse((D3DXMATRIX*)&pg->KelvinPrimitive.SetInverseModelViewMatrix[i][0], NULL, (D3DXMATRIX*)&pg->KelvinPrimitive.SetModelViewMatrix[i][0]);
		}
		// clear dirty flags
		g_NV2A_transform_ModelView_dirty[i] = false;
		g_NV2A_transform_InverseModelView_dirty[i] = false;
	}
	g_NV2A_transform_Composite_dirty = false;
	//}

	//these matrix will be used in UpdateFixedFunctionShaderLight(): view transform, and UpdateFixedFunctionVertexShaderState():  later in CxbxUpdateNativeD3DResources();
}
enum PS_TEXTUREMODES
{                                 // valid in stage 0 1 2 3
	PS_TEXTUREMODES_NONE = 0x00L, // * * * *
	PS_TEXTUREMODES_PROJECT2D = 0x01L, // * * * *
	PS_TEXTUREMODES_PROJECT3D = 0x02L, // * * * *
	PS_TEXTUREMODES_CUBEMAP = 0x03L, // * * * *
	PS_TEXTUREMODES_PASSTHRU = 0x04L, // * * * *
	PS_TEXTUREMODES_CLIPPLANE = 0x05L, // * * * *
	PS_TEXTUREMODES_BUMPENVMAP = 0x06L, // - * * *
	PS_TEXTUREMODES_BUMPENVMAP_LUM = 0x07L, // - * * *
	PS_TEXTUREMODES_BRDF = 0x08L, // - - * *
	PS_TEXTUREMODES_DOT_ST = 0x09L, // - - * *
	PS_TEXTUREMODES_DOT_ZW = 0x0aL, // - - * *
	PS_TEXTUREMODES_DOT_RFLCT_DIFF = 0x0bL, // - - * -
	PS_TEXTUREMODES_DOT_RFLCT_SPEC = 0x0cL, // - - - *
	PS_TEXTUREMODES_DOT_STR_3D = 0x0dL, // - - - *
	PS_TEXTUREMODES_DOT_STR_CUBE = 0x0eL, // - - - *
	PS_TEXTUREMODES_DPNDNT_AR = 0x0fL, // - * * *
	PS_TEXTUREMODES_DPNDNT_GB = 0x10L, // - * * *
	PS_TEXTUREMODES_DOTPRODUCT = 0x11L, // - * * -
	PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST = 0x12L, // - - - *
	// 0x13-0x1f reserved
};
void CxbxrImpl_LazySetShaderStageProgram(NV2AState* d)
{
	PGRAPHState* pg = &d->pgraph;
	HRESULT hRet;
	// update texture stage texture pointers and format per stageProgram
	DWORD stageProgram = pg->KelvinPrimitive.SetShaderStageProgram;
	NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_PSTEXTUREMODES, stageProgram);
	if (pNV2A_PixelShader == nullptr) {
		// update combiners, combiners must be update prior to pixel shader, because we have to compose colorOp before we compose fix funtion pixel shaders.
		if ((NV2A_DirtyFlags & X_D3DDIRTYFLAG_COMBINERS) != 0) {
			CxbxrImpl_LazySetCombiners(d);
			// clear dirty flag
			NV2A_DirtyFlags &= ~X_D3DDIRTYFLAG_COMBINERS;
		}
	}
	else { // user mode
		// Populate all required PSDef fields by copying over from KelvinPrimitive fields;
		// TODO : Refactor DxbxUpdateActivePixelShader to directly read from KelvinPrimitive (once we can otherwise drop PSDef entirely)
		memcpy(&NV2A_PSDef.PSAlphaInputs[0], &pg->KelvinPrimitive.SetCombinerAlphaICW[0], 8 * sizeof(DWORD));
		// only set NV2A_PSDef.PSFinalCombinerInputsABCD and NV2A_PSDef.PSFinalCombinerInputsEFG when KelvinPrimitive.SetCombinerSpecularFogCW0 or KelvinPrimitive.SetCombinerSpecularFogCW1 is dirty
		// FIXMED!!! this is only considering the pixel shader code itself.
		// but xbox d3d might set PSDef.PSFinalCombinerInputsABCD/PSDef.PSFinalCombinerInputsEFG when Fog state changed. in that condition, final combiner changed, shall we consider that situation and regegerate pixel shader?
		// in pushbuffer sample it sets the pixel shader without final combiner. but xbox d3d sets final combiner later because of fog state chaged. if we regenerate the pixel shader with changed final combiner, the rendering output would be wrong. 
		bool bHasFinalCombiner = true;// NV2A_stateFlags& X_STATE_COMBINERNEEDSSPECULAR != 0 ? true : false;
		// DxbxUpdateActivePixelShader() doesn't consider about the precondition, we have to treat the dirty flag here.
		NV2A_PSDef.PSFinalCombinerInputsABCD = bHasFinalCombiner ? pg->KelvinPrimitive.SetCombinerSpecularFogCW0 : 0;
		NV2A_PSDef.PSFinalCombinerInputsEFG = bHasFinalCombiner ? pg->KelvinPrimitive.SetCombinerSpecularFogCW1 : 0;
		//NV2A_stateFlags &= ~X_STATE_COMBINERNEEDSSPECULAR; // FIXME!!! shall we reset the flag here?
		memcpy(&NV2A_PSDef.PSConstant0[0], &pg->KelvinPrimitive.SetCombinerFactor0[0], (8 + 8 + 8 + 8) * sizeof(DWORD));
		/* Since the following fields are adjacent in Kelvin AND PSDef, above memcpy is extended to copy the entire range :
		memcpy(&NV2A_PSDef.PSConstant1[0], &pg->KelvinPrimitive.SetCombinerFactor1[0], 8 * sizeof(DWORD));
		memcpy(&NV2A_PSDef.PSAlphaOutputs[0], &pg->KelvinPrimitive.SetCombinerAlphaOCW[0], 8 * sizeof(DWORD));
		memcpy(&NV2A_PSDef.PSRGBInputs[0], &pg->KelvinPrimitive.SetCombinerColorICW[0], 8 * sizeof(DWORD)); */
		NV2A_PSDef.PSCompareMode = pg->KelvinPrimitive.SetShaderClipPlaneMode;
		NV2A_PSDef.PSFinalCombinerConstant0 = pg->KelvinPrimitive.SetSpecularFogFactor[0];
		NV2A_PSDef.PSFinalCombinerConstant1 = pg->KelvinPrimitive.SetSpecularFogFactor[1];
		memcpy(&NV2A_PSDef.PSRGBOutputs[0], &pg->KelvinPrimitive.SetCombinerColorOCW[0], (8 + 1) * sizeof(DWORD));
		/* Since the following field is adjacent in Kelvin AND PSDef, above memcpy is extended to copy the entire range :
		NV2A_PSDef.PSCombinerCount = pg->KelvinPrimitive.SetCombinerControl; */
		NV2A_PSDef.PSTextureModes = pg->KelvinPrimitive.SetShaderStageProgram; // Was : XboxRenderStates.GetXboxRenderState(xbox::X_D3DRS_PSTEXTUREMODES);
		NV2A_PSDef.PSDotMapping = pg->KelvinPrimitive.SetDotRGBMapping; // Note : Adjacent to above and below fields, but 3 assignments are cheaper than memcpy call overhead
		NV2A_PSDef.PSInputTexture = pg->KelvinPrimitive.SetShaderOtherStageInput;
		// set pixel shader
		pgraph_use_UserPixelShader();
	}
	//always update bumpEnv when ShaderStageProgram changed.
	CxbxrImpl_CommonSetTextureBumpEnv(d);
	// reset bumpenv_dirty[] flags, currently we're not referencing these flags. pixel shader generator will update bumpenv anyway.
	pg->bumpenv_dirty[0] = false;
	pg->bumpenv_dirty[1] = false;
	pg->bumpenv_dirty[2] = false;
	pg->bumpenv_dirty[3] = false;
}

void CxbxrImpl_LazySetSpecFogCombiner(NV2AState* d)
{
	PGRAPHState* pg = &d->pgraph;
	HRESULT hRet;
	if (pg->KelvinPrimitive.SetFogEnable != 0) {
		float fogTableStart, fogTableEnd, fogTableDensity;
		DWORD fogTableMode, fogGenMode, fogMode;
		float bias, scale;
		fogGenMode = pg->KelvinPrimitive.SetFogGenMode;
		fogMode = pg->KelvinPrimitive.SetFogMode;
		bias = pg->KelvinPrimitive.SetFogParamsBias;
		scale = pg->KelvinPrimitive.SetFogParamsScale;
		// D3D__RenderState[D3DRS_RANGEFOGENABLE] == true fogGenMode =NV097_SET_FOG_GEN_MODE_V_RADIAL, else fogGenMode =NV097_SET_FOG_GEN_MODE_V_PLANAR
		// notice that when fogTableMode == D3DFOG_NONE, fogGenMode will be reset to NV097_SET_FOG_GEN_MODE_V_SPEC_ALPHA, in  that case, we won't be able to know what D3D__RenderState[D3DRS_RANGEFOGENABLE] should be
		bool bD3DRS_RangeFogEnable = fogGenMode == NV097_SET_FOG_GEN_MODE_V_RADIAL ? true : false;
		fogTableMode = D3DFOG_NONE;
		if (fogMode == NV097_SET_FOG_MODE_V_EXP2) {
			fogTableMode = D3DFOG_EXP2;
			// scale =  -fogTableDensity * (1 / (2 * sqrt(5.5452)))
			// scale = -fogTableDensity * (1.0f / (2.0f * 2.354824834249885f));
			
			fogTableDensity = -scale / 0.21233003f;

			//hRet = g_pD3DDevice->SetRenderState(D3DRS_FOGDENSITY, fogTableDensity);
			NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_FOGDENSITY, FtoDW(fogTableDensity));
		}
		else if (fogMode == NV097_SET_FOG_MODE_V_EXP) {
			fogTableMode = D3DFOG_EXP;
			// scale = -fogTableDensity * (1.0f / (2.0f * 5.5452f));
			fogTableDensity = -scale/ 0.090168074;

			//hRet = g_pD3DDevice->SetRenderState(D3DRS_FOGDENSITY, fogTableDensity);
			NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_FOGDENSITY, FtoDW(fogTableDensity));
		}
		else if (fogMode == NV097_SET_FOG_MODE_V_LINEAR) {
			if (fogGenMode == NV097_SET_FOG_GEN_MODE_V_SPEC_ALPHA) {
				fogTableMode = D3DFOG_NONE;
				// RIXME!!! need to set bD3DRS_RangeFogEnable here because fogGenMode is not correctly verified.
				// D3DFOG_NONE : No fog effect, so set D3DRS_RangeFogEnable to false
				//bD3DRS_RangeFogEnable = false;
			}else{
				fogTableMode = D3DFOG_LINEAR;
				FLOAT fogTableLinearScale;
				fogTableLinearScale = -scale;
				fogTableEnd = (bias - 1.0f) / fogTableLinearScale;
				// MAX_FOG_SCALE == 8192.0f
				if (fogTableLinearScale == 8192.0f) {
					//fogTableStart== fogTableEnd
					fogTableStart == fogTableEnd;
				}
				else {
					// fogTableLinearScale = 1.0f / (fogTableEnd - fogTableStart);
					// fogTableStart = fogTableEnd - 1.0f/fogTableLinearScale
					fogTableStart = fogTableEnd - 1.0f / fogTableLinearScale;
					
				}
				NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_FOGSTART, FtoDW(fogTableStart));
				NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_FOGEND, FtoDW(fogTableEnd));
			}

		}
		
		NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_FOGENABLE, true);
		/*
		fogGenMode = D3D__RenderState[D3DRS_RANGEFOGENABLE]
						 ? NV097_SET_FOG_GEN_MODE_V_RADIAL
						 : NV097_SET_FOG_GEN_MODE_V_PLANAR;
		*/
		bD3DRS_RangeFogEnable = (fogGenMode == NV097_SET_FOG_GEN_MODE_V_RADIAL)? true:false;
		
		NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_RANGEFOGENABLE, bD3DRS_RangeFogEnable);
		NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_FOGTABLEMODE, fogTableMode);
		uint32_t fog_color = pg->KelvinPrimitive.SetFogColor;
		/* Kelvin Kelvin fog color channels are ABGR, PGRAPH channels are ARGB */
		// fog color was handled by pgraph handler in NV097_SET_FOG_COLOR directly.
		NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_FOGCOLOR, ABGR_to_ARGB(fog_color));
		// D3D__RenderState[D3DRS_SPECULARENABLE] == true
		if ((pg->KelvinPrimitive.SetCombinerSpecularFogCW0 & NV097_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE) == NV097_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE_REG_SPECLIT) {//NV097_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE_REG_SPECLIT
			NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_SPECULARENABLE, true);
		}
		else {
			NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_SPECULARENABLE, false);
		}

	}
	else {
		NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_FOGENABLE, false);
		// D3D__RenderState[D3DRS_SPECULARENABLE] == true
		if ((pg->KelvinPrimitive.SetCombinerSpecularFogCW0 & NV097_SET_COMBINER_SPECULAR_FOG_CW0_D_SOURCE) == NV097_SET_COMBINER_SPECULAR_FOG_CW0_D_SOURCE_REG_SPECLIT) {
			NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_SPECULARENABLE, true);
		}
		else {
			NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_SPECULARENABLE, false);
		}
	}
}

void CxbxrImpl_LazySetTextureTransform(NV2AState* d)
{
	PGRAPHState* pg = &d->pgraph;
	HRESULT hRet;
	DWORD stage;

	for (stage = 0; stage < 4; stage++)
	{
		DWORD transformFlags;// = D3D__TextureState[stage]	[D3DTSS_TEXTURETRANSFORMFLAGS];
		//Push1(pPush, NV097_SET_TEXTURE_MATRIX_ENABLE(stage), FALSE);
		bool bTextureTransformEnable = pg->KelvinPrimitive.SetTextureMatrixEnable[stage];
        //default texCoordIndex
		DWORD texCoordIndex = stage;
		DWORD texgen = D3DTSS_TCI_PASSTHRU;
		if (!bTextureTransformEnable) {
			transformFlags = D3DTTFF_DISABLE;
			NV2ATextureStates.Set(stage, xbox::X_D3DTSS_TEXTURETRANSFORMFLAGS, transformFlags);
		}
		else {
			// Enable the transform:

			DWORD texCoord;// = D3D__TextureState[stage][D3DTSS_TEXCOORDINDEX];
			//DWORD texgen;// = texCoord & 0xffff0000;
			DWORD inCount,outCount;
			bool m2_0000 = false, m3_0001 = false, isProjected = false;
			// texture transform matrix in Kelvin are transposed. both input/output vectors are in order with (s,t,r,q). input/output q default to 1.0 unless projected output q was specified.
			// SetTextureMatrix[stage].m[2]=={ 0.0f, 0.0f, 0.0f, 0.0f}?
			if (pg->KelvinPrimitive.SetTextureMatrix[stage][8] == 0.0 && pg->KelvinPrimitive.SetTextureMatrix[stage][9] == 0.0
				&& pg->KelvinPrimitive.SetTextureMatrix[stage][10] == 0.0 && pg->KelvinPrimitive.SetTextureMatrix[stage][11] == 0.0)
				m2_0000 = true;

			// SetTextureMatrix[stage].m[3]=={ 0.0f, 0.0f, 0.0f, 1.0f}?
			if (pg->KelvinPrimitive.SetTextureMatrix[stage][12] == 0.0 && pg->KelvinPrimitive.SetTextureMatrix[stage][13] == 0.0
				&& pg->KelvinPrimitive.SetTextureMatrix[stage][14] == 0.0 && pg->KelvinPrimitive.SetTextureMatrix[stage][15] == 1.0)
				m3_0001 = true;

			if (m2_0000) {// SetTextureMatrix[stage].m[2]=={ 0.0f, 0.0f, 0.0f, 0.0f}?

				if (m3_0001) {// SetTextureMatrix[stage].m[3]=={ 0.0f, 0.0f, 0.0f, 1.0f}?
					// 220/320,outCount=2, D3DTTFF_PROJECTED=0
					outCount = 2;
					isProjected = false;
				}
				else {
					// 231/331, outCount=3,D3DTTFF_PROJECTED=1
					outCount = 3;
					isProjected = true;
				}
			}
			else if (m3_0001) {// SetTextureMatrix[stage].m[3]=={ 0.0f, 0.0f, 0.0f, 1.0f}?
				// 230/330, outCount=3, D3DTTFF_PROJECTED=0
				outCount = 3;
				isProjected = false;
			}
			else {
				// 241/341,  outCount=4, D3DTTFF_PROJECTED=1
				outCount = 4;
				isProjected = true;
			}

			transformFlags = outCount;
			if (isProjected)
				transformFlags |= D3DTTFF_PROJECTED;// D3DTTFF_PROJECTED=0x100

			inCount = (pg->KelvinPrimitive.SetVertexDataArrayFormat[stage + xbox::X_D3DVSDE_TEXCOORD0] & NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE) >> 4;//SLOT_TEXTURE0=9, Slot[i + SLOT_TEXTURE0].SizeAndType
			if (inCount == 0)
				inCount = 2;

			// compose texgen 

			// Initialize assuming D3DTSS_TCI_PASSTHRU:
			DWORD needsInverseModelViewState = false;
			DWORD mapToStage = stage;
			DWORD texgenMode = NV097_SET_TEXGEN_S_DISABLE;


			// Now specifically handle texgens:
			extern unsigned int kelvin_to_xbox_map_texgen(uint32_t parameter);
			texgen = kelvin_to_xbox_map_texgen(pg->KelvinPrimitive.SetTexgen[stage].S);
            //  whether texture coordinate with (0, 0, 0, 1) to get a '1' into 'W' because we leave Q disabled.
			// and SetTexgen[stage].s==t==r. reversed from D3DDevice_SetTextureState_TexCoordIndex()
			
			/*
			if (pg->KelvinPrimitive.SetTexgen[stage].S == pg->KelvinPrimitive.SetTexgen[stage].T && pg->KelvinPrimitive.SetTexgen[stage].S == pg->KelvinPrimitive.SetTexgen[stage].R &&
				pg->KelvinPrimitive.SetVertexData4ub[xbox::X_D3DVSDE_TEXCOORD0 + stage] == 0xFF000000)
			{
				// Since texgen is enabled, we obviously don't need to read texture 
				// coordinates from the vertex buffer for this stage.  Rather than 
				// add special logic in the vertex buffer setup code, we simply make
				// sure we map to this stage, and work under the assumption that
				// the caller won't specify a texture coordinate for this stage in
				// their FVF.

				if (texgenMode == NV097_SET_TEXGEN_S_OBJECT_LINEAR || texgenMode == NV097_SET_TEXGEN_S_SPHERE_MAP)     //0x2401 0x2402           
				{
					texgen = D3DTSS_TCI_SPHEREMAP;//NV097_SET_TEXGEN_S_OBJECT_LINEAR and NV097_SET_TEXGEN_S_SPHERE_MAP are actually not supported in xbox. the D3DTSS_TCI_SPHEREMAP doesn't exist in xbox d3d api.
				}
				else if (texgenMode == NV097_SET_TEXGEN_S_REFLECTION_MAP)  //0x8511
				{
					texgen = D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR;
					needsInverseModelViewState = true;
				}
				else if (texgenMode == NV097_SET_TEXGEN_S_NORMAL_MAP)//0x8512
				{
					texgen = D3DTSS_TCI_CAMERASPACENORMAL;
					needsInverseModelViewState = true;
				}
				else if (texgenMode == NV097_SET_TEXGEN_S_EYE_LINEAR)   // 0x2400
				{
					texgen = D3DTSS_TCI_CAMERASPACEPOSITION;// = 0x20000
				}
			}
			*/
			// Handle the re-mapping of a stage: xbox converts remapping before it sets Kelvin's vertex attribute and stream. so no need to remap if we compose vertex attr from kelvin. check StateUp()/StateVb()
			// NV097_SET_VERTEX_DATA_ARRAY_FORMAT and NV097_SET_VERTEX_DATA_ARRAY_OFFSET are update with slot mapping included. so we don't worry about it here.

			// compose texCoordIndex with stage mapping index and texgen, set it to defaul when we compose vertex attr from kelvin.
			//NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_WRAP0 + stage, warp);

			// todo: shall we compose the FVF pVertexShader->Dimensionality here with the inCount?
			// inCount = (pVertexShader->Dimensionality 	>> (8 * index)) & 0xff;
			// pVertexShader->Dimensionality|= incount<<  (8 * index);// index==stage here. the actual index shall be the render state WARP0~3 indexed with texture render state D3DTSS_TEXCOORDINDEX. but we can't reverse both variables with the inCount value directly.
			// so we arrume for each texture stage D3DTSS_TEXCOORDINDEX==stage, and each stage indexes WARP(stage). this is xbox default setting.
			NV2ATextureStates.Set(stage, xbox::X_D3DTSS_TEXTURETRANSFORMFLAGS, transformFlags);
			//extern FixedFunctionVertexShaderState ffShaderState;
			//ffShaderState.Transforms.Texture[] are updated in UpdateFixedFunctionVertexShaderState() with the same code below.
			//memcpy(&ffShaderState.Transforms.Texture[stage], &pg->KelvinPrimitive.SetTextureMatrix[0], sizeof(float) * 16);


			// todo:set transform dirty if needed
			//if (!(TexGenInverseNeeded) && (needsInverseModelViewState))
			//    NV2A_DirtyFlags |= X_D3DDIRTYFLAG_TRANSFORM;

			//TexGenInverseNeeded &= ~(1 << Stage);
			//TexGenInverseNeeded |= (needsInverseModelViewState << stage);
		}
		texCoordIndex = (texgen & 0xFFFF0000) | stage;
		NV2ATextureStates.Set(stage, xbox::X_D3DTSS_TEXCOORDINDEX, texCoordIndex);
		//reversed codes for reference. check Otogi for LazySetTextureTransform()
#if 0
			/*
			if (texgen)
			{
				inCount = 3;
			}
			else
			{
				DWORD index = texCoord & 0xffff;

				// If the index is invalid, just use (0, 0):

				inCount = (pDevice->m_pVertexShader->Dimensionality
					>> (8 * index)) & 0xff;

				if (inCount == 0)
					inCount = 2;// inCount is either 2 or 3, always. (s,t), or (s,t,r)
				/* inCount is the vertex attribute for the texture coordinates (s,t,r).. of texture used in this stage, which also presented in
				vertex_attribute.Slot[i + SLOT_TEXTURE0].SizeAndType
				= DRF_NUMFAST(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _SIZE, count)
					| DRF_DEF(097, _SET_VERTEX_DATA_ARRAY_FORMAT, _TYPE, _F);
				So we check NV097_SET_VERTEX_DATA_ARRAY_FORMAT for vertex_attribute->count, vertex attribute SLOT_TEXTURE0~3 correspondes to texture number 0~3, it's also texture stage number because xbox API requires texture stage 0 to use texture 0, stage 1 to use texture 1, etc.
				and we can compose the vertex shader's Dimensionality.

				outCount is the actual texture coordinate counts of the texture used in this stage, (s,t,r,q)...
				if (transformFlags & D3DTTFF_PROJECTED) != 0), then the outCount includes one projected coordinate "q". if the texture coordinate uses S,T, the outCount shall be 2, but is D3DTTFF_PROJECTED(0x100) was ORed in the transformFlags, then the outCount shall add 1 and become 3.
				when D3DTTFF_PROJECTED was set, the outPut coordinate should be devided by the projected value "q". so the output would become (s/q, t/q, r/q).
		    */
			/*
				DWORD outCount = transformFlags & 0xff;
				

				DWORD matrixType = (inCount << 8) |
					(outCount << 4) |
					((transformFlags & D3DTTFF_PROJECTED) != 0);

				D3DMATRIX* pMatrix
					= &pDevice->m_Transform[D3DTS_TEXTURE0 + stage];

				Push1(pPush, NV097_SET_TEXTURE_MATRIX_ENABLE(stage), TRUE);

				PushCount(pPush + 2,
					NV097_SET_TEXTURE_MATRIX0(0) + stage * 0x40,
					16);

				D3DMATRIX* pDst = (D3DMATRIX*)(pPush + 3);

				pPush += 19;

				// The way we emit the matrix varies widely because of D3D.
				//
				// A. When there are two incoming coordinates, we must move 
				//    the 3rd matrix row to the fourth, because D3D expects 
				//    the appended q to sit in the third slot, whereas in 
				//    the hardware, it sits in the 4th.
				// B. When the matrix is projective and there are only 3 
				//    outgoing coordinates (including q) we must shift the 
				//    value in the third outgoing slot (where D3D puts q) to 
				//    the 4th slot (where the hardware wants it) by moving the 
				//    3rd column to the 4th
				// C. When the matrix is non-projective, it becomes our job to 
				//    override the relevant row of the matrix, forcing the 
				//    outgoing q to 1.0
				//
				// Finally, when all is said and done, the matrix must be 
				// "transposed" to change it from D3D style to hardware style.
				// input vector will always be added a q coordinate as 1.0f, so if input cout==2, there will be a 3rd cooridnate q set with 1.0.
				// output will always has a q coordinate, when there is no projected output, the output q will alwaays be 1.0.
				// input vector(s,t,r,q), output vector(s,t,r,q). hardware matrix is transposed.
				// from the actual NV097_SET_TEXTURE_MATRIX(stage), we could compose the outCount and whether D3DTTFF_PROJECTED was set or not.
				//

				switch (matrixType)
				{
				case 0x220:
					// (s,t,1.0) in, (s,t,1.0) out
					SetTextureTransformMatrixType220(pDst, pMatrix);

					
					e
					FORCEINLINE VOID SetTextureTransformMatrixType220(
						D3DMATRIX * pDst,
						CONST D3DMATRIX * pMatrix)
					{
						Set4f(&pDst->m[0][0], pMatrix->_11, pMatrix->_21, 0.0f, pMatrix->_31);
						Set4f(&pDst->m[1][0], pMatrix->_12, pMatrix->_22, 0.0f, pMatrix->_32);
						Set4f(&pDst->m[2][0], 0.0f, 0.0f, 0.0f, 0.0f);
						Set4f(&pDst->m[3][0], 0.0f, 0.0f, 0.0f, 1.0f);
					}
					break;
				case 0x230:
					// (s,t,1.0) in, (s,t,r,1.0) out
					SetTextureTransformMatrixType230(pDst, pMatrix);

					FORCEINLINE VOID SetTextureTransformMatrixType230(
						D3DMATRIX* pDst,
						CONST D3DMATRIX* pMatrix)
					{
						Set4f(&pDst->m[0][0], pMatrix->_11, pMatrix->_21, 0.0f, pMatrix->_31);
						Set4f(&pDst->m[1][0], pMatrix->_12, pMatrix->_22, 0.0f, pMatrix->_32);
						Set4f(&pDst->m[2][0], pMatrix->_13, pMatrix->_23, 0.0f, pMatrix->_33);
						Set4f(&pDst->m[3][0], 0.0f, 0.0f, 0.0f, 1.0f);
					}
					break;
				case 0x231:
					// (s,t,1.0) in, (s,t,q) out
					SetTextureTransformMatrixType231(pDst, pMatrix);

					FORCEINLINE VOID SetTextureTransformMatrixType231(
						D3DMATRIX* pDst,
						CONST D3DMATRIX* pMatrix)
					{
						Set4f(&pDst->m[0][0], pMatrix->_11, pMatrix->_21, 0.0f, pMatrix->_31);
						Set4f(&pDst->m[1][0], pMatrix->_12, pMatrix->_22, 0.0f, pMatrix->_32);
						Set4f(&pDst->m[2][0], 0.0f, 0.0f, 0.0f, 0.0f);
						Set4f(&pDst->m[3][0], pMatrix->_13, pMatrix->_23, 0.0f, pMatrix->_33);
					}
					break;
				case 0x241:
					// (s,t,1.0) in, (s,t,r,q) out
					SetTextureTransformMatrixType241(pDst, pMatrix);

					FORCEINLINE VOID SetTextureTransformMatrixType241(
						D3DMATRIX* pDst,
						CONST D3DMATRIX* pMatrix)
					{
						Set4f(&pDst->m[0][0], pMatrix->_11, pMatrix->_21, 0.0f, pMatrix->_31);
						Set4f(&pDst->m[1][0], pMatrix->_12, pMatrix->_22, 0.0f, pMatrix->_32);
						Set4f(&pDst->m[2][0], pMatrix->_13, pMatrix->_23, 0.0f, pMatrix->_33);
						Set4f(&pDst->m[3][0], pMatrix->_14, pMatrix->_24, 0.0f, pMatrix->_34);
					}
					break;
				case 0x320:
					// (s,t,r,1.0) in, (s,t,1.0) out
					SetTextureTransformMatrixType320(pDst, pMatrix);

					FORCEINLINE VOID SetTextureTransformMatrixType320(
						D3DMATRIX* pDst,
						CONST D3DMATRIX* pMatrix)
					{
						Set4f(&pDst->m[0][0], pMatrix->_11, pMatrix->_21, pMatrix->_31, pMatrix->_41);
						Set4f(&pDst->m[1][0], pMatrix->_12, pMatrix->_22, pMatrix->_32, pMatrix->_42);
						Set4f(&pDst->m[2][0], 0.0f, 0.0f, 0.0f, 0.0f);
						Set4f(&pDst->m[3][0], 0.0f, 0.0f, 0.0f, 1.0f);
					}
					break;
				case 0x330:
					// (s,t,r,1.0) in, (s,t,r,1.0) out
					SetTextureTransformMatrixType330(pDst, pMatrix);

					FORCEINLINE VOID SetTextureTransformMatrixType330(
						D3DMATRIX* pDst,
						CONST D3DMATRIX* pMatrix)
					{
						Set4f(&pDst->m[0][0], pMatrix->_11, pMatrix->_21, pMatrix->_31, pMatrix->_41);
						Set4f(&pDst->m[1][0], pMatrix->_12, pMatrix->_22, pMatrix->_32, pMatrix->_42);
						Set4f(&pDst->m[2][0], pMatrix->_13, pMatrix->_23, pMatrix->_33, pMatrix->_43);
						Set4f(&pDst->m[3][0], 0.0f, 0.0f, 0.0f, 1.0f);
					}
					break;
				case 0x331:
					// (s,t,r,1.0) in, (s,t,q) out
					SetTextureTransformMatrixType331(pDst, pMatrix);

					FORCEINLINE VOID SetTextureTransformMatrixType331(
						D3DMATRIX* pDst,
						CONST D3DMATRIX* pMatrix)
					{
						Set4f(&pDst->m[0][0], pMatrix->_11, pMatrix->_21, pMatrix->_31, pMatrix->_41);
						Set4f(&pDst->m[1][0], pMatrix->_12, pMatrix->_22, pMatrix->_32, pMatrix->_42);
						Set4f(&pDst->m[2][0], 0.0f, 0.0f, 0.0f, 0.0f);
						Set4f(&pDst->m[3][0], pMatrix->_13, pMatrix->_23, pMatrix->_33, pMatrix->_43);
					}
					break;
				case 0x341:
					// (s,t,r,1.0) in, (s,t,r,q) out
					SetTextureTransformMatrixType341(pDst, pMatrix);

					FORCEINLINE VOID SetTextureTransformMatrixType341(
						D3DMATRIX* pDst,
						CONST D3DMATRIX* pMatrix)
					{
						Set4f(&pDst->m[0][0], pMatrix->_11, pMatrix->_21, pMatrix->_31, pMatrix->_41);
						Set4f(&pDst->m[1][0], pMatrix->_12, pMatrix->_22, pMatrix->_32, pMatrix->_42);
						Set4f(&pDst->m[2][0], pMatrix->_13, pMatrix->_23, pMatrix->_33, pMatrix->_43);
						Set4f(&pDst->m[3][0], pMatrix->_14, pMatrix->_24, pMatrix->_34, pMatrix->_44);
					}
					break;
				default:
					;//("Unhandled texture transform type\n");
				}
				*/
#endif
		
	}// stage for loop
	return;

}
// scale a 3-component vector

void ScaleVector3(D3DVECTOR* out, CONST D3DVECTOR* v1, FLOAT scale)
{
	out->x = scale * v1->x;
	out->y = scale * v1->y;
	out->z = scale * v1->z;
}

//---------------------------------------------------------------------------

// add two 3-component vectors

void AddVectors3(D3DVECTOR* out, CONST D3DVECTOR* v1, CONST D3DVECTOR* v2)
{
	out->x = v1->x + v2->x;
	out->y = v1->y + v2->y;
	out->z = v1->z + v2->z;
}

// substract two 3-component vectors

void SubVectors3(D3DVECTOR* out, CONST D3DVECTOR* v1, CONST D3DVECTOR* v2)
{
	out->x = v1->x - v2->x;
	out->y = v1->y - v2->y;
	out->z = v1->z - v2->z;
}
//---------------------------------------------------------------------------

// return the square of the magnitude of a 3-component vectors

FLOAT SquareMagnitude3(CONST D3DVECTOR* v)
{
	return (v->x * v->x + v->y * v->y + v->z * v->z);
}
//---------------------------------------------------------------------------

// returns  1 / sqrt(x)

static float _0_47 = 0.47f;
static float _1_47 = 1.47f;

float JBInvSqrt(const float x)
{
	DWORD y;
	float r;

	_asm
	{
		mov     eax, 07F000000h + 03F800000h  // (ONE_AS_INTEGER<<1) + ONE_AS_INTEGER
		sub     eax, x
		sar     eax, 1

		mov     y, eax                      // y
		fld     _0_47                       // 0.47
		fmul    DWORD PTR x                 // x*0.47

		fld     DWORD PTR y
		fld     st(0)                       // y y x*0.47
		fmul    st(0), st(1)                // y*y y x*0.47

		fld     _1_47                       // 1.47 y*y y x*0.47
		fxch    st(3)                       // x*0.47 y*y y 1.47
		fmulp   st(1), st(0)                // x*0.47*y*y y 1.47
		fsubp   st(2), st(0)                // y 1.47-x*0.47*y*y
		fmulp   st(1), st(0)                // result
		fstp    y
	}

	y &= 0x7FFFFFFF;  // make it positive
	r = DWtoF(y);

	// optional
	r = (3.0f - x * (r * r)) * r * 0.5f;    // remove for low accuracy

	return (r);
}
//---------------------------------------------------------------------------

//  1/x = 1 / sqrt(x*x) plus sign bit

float nvInv(float x)
{
	DWORD dwSign = *(DWORD*)&x & 0x80000000;
	float invSqRt = JBInvSqrt(x * x);
	DWORD dwInv = dwSign | *(DWORD*)&invSqRt;
	return (*(float*)(&dwInv));
}
//---------------------------------------------------------------------------
// normalize a 3-component vector

void NormalizeVector3(D3DVECTOR* v)
{
	FLOAT invmag = JBInvSqrt(SquareMagnitude3(v));
	assert(invmag > 0);
	v->x *= invmag;
	v->y *= invmag;
	v->z *= invmag;
}
const float g_Lar[32] =
{
	 0.000000f,  // error=0.687808
	-0.023353f,  // error=0.070106
	-0.095120f,  // error=0.010402
	-0.170208f,  // error=0.016597
	-0.251038f,  // error=0.021605
	-0.336208f,  // error=0.025186
	-0.421539f,  // error=0.027635
	-0.503634f,  // error=0.029262
	-0.579592f,  // error=0.030311
	-0.647660f,  // error=0.030994
	-0.708580f,  // error=0.031427
	-0.760208f,  // error=0.031702
	-0.803673f,  // error=0.031889
	-0.840165f,  // error=0.031995
	-0.871344f,  // error=0.032067
	-0.896105f,  // error=0.032105
	-0.916457f,  // error=0.032139
	-0.933262f,  // error=0.032165
	-0.946507f,  // error=0.032173
	-0.957755f,  // error=0.032285
	-0.966165f,  // error=0.032230
	-0.972848f,  // error=0.032189
	-0.978413f,  // error=0.032191
	-0.983217f,  // error=0.032718
	-0.986471f,  // error=0.032289
	-0.988778f,  // error=0.033091
	-0.991837f,  // error=0.035067
	-0.993452f,  // error=0.034156
	-0.994839f,  // error=0.034863
	-0.995434f,  // error=0.034785
	-0.996690f,  // error=0.033426
	-1.000000f
};

const float g_Mar[32] =
{
	-0.494592f,  // error=0.687808
	-0.494592f,  // error=0.070106
	-0.570775f,  // error=0.010402
	-0.855843f,  // error=0.016597
	-1.152452f,  // error=0.021605
	-1.436778f,  // error=0.025186
	-1.705918f,  // error=0.027635
	-1.948316f,  // error=0.029262
	-2.167573f,  // error=0.030311
	-2.361987f,  // error=0.030994
	-2.512236f,  // error=0.031427
	-2.652873f,  // error=0.031702
	-2.781295f,  // error=0.031889
	-2.890906f,  // error=0.031995
	-2.938739f,  // error=0.032067
	-3.017491f,  // error=0.032105
	-3.077762f,  // error=0.032139
	-3.099087f,  // error=0.032165
	-3.144977f,  // error=0.032173
	-3.100986f,  // error=0.032285
	-3.151608f,  // error=0.032230
	-3.212636f,  // error=0.032189
	-3.219419f,  // error=0.032191
	-3.079402f,  // error=0.032718
	-3.174922f,  // error=0.032289
	-3.469706f,  // error=0.033091
	-2.895668f,  // error=0.035067
	-2.959919f,  // error=0.034156
	-2.917150f,  // error=0.034863
	-3.600301f,  // error=0.034785
	-3.024990f,  // error=0.033426
	-3.300000f
};
const float LOG_64F = 4.15888308336f;
const float INV_LOG_2F = 1.44269504089f;

// Exponent
float Exp(float e)
{
	WORD istat;
	WORD fstat;

	_asm
	{
		fld[e]

		xor ch, ch; result is always positive
		fldl2e
		fmul; convert log base e to log base 2

		fld	st(0); copy TOS
		frndint; near round to integer
		ftst
		fstsw[istat]; save integer part status
		fwait
		fxch; NOS gets integer part
		fsub	st, st(1); TOS gets fraction
		ftst
		fstsw[fstat]; save fraction part status
		fabs
		f2xm1

		fld1
		fadd
		test[fstat + 1], 1; if fraction > 0 (TOS > 0)
		jz	ExpNoInvert;	 bypass 2 ^ x invert

		fld1
		fdivrp	st(1), st(0)

		ExpNoInvert:
		test[istat + 1], 040h; if integer part was zero
			jnz	ExpScaled;	 bypass scaling to avoid bug
			fscale; now TOS = 2 ^ x

			ExpScaled :
		or ch, ch; check for negate flag
			jz	expret
			fchs; negate result(negreal ^ odd integer)

			expret:
		fxch
			fstp st(0)
	}
}
// Log
_declspec(naked) float Log(float e)
{

	_asm
	{
		fldln2
		fld[esp + 4]
		fyl2x
		ret 4
	}
}

void Explut(float n, float* l, float* m)
{
	float idx, f, a;
	long  i;

	if (n < 1.f) {
		a = (n == 0.f) ? 0.f : (float)Exp(-LOG_64F / n);
		*l = -a;
		*m = 1.f - (1.f - a) * n;
	}
	else {
		idx = 3.f * (float)Log(n) * INV_LOG_2F;
		i = FloatToLong(idx);
		f = idx - i;

		*l = g_Lar[i] * (1.f - f) + g_Lar[i + 1] * f;
		*m = g_Mar[i] * (1.f - f) + g_Mar[i + 1] * f;
	}
}
void SetSceneAmbientAndMaterialEmission(NV2AState* d)
{
	PGRAPHState* pg = &d->pgraph;
	FLOAT ambientR, ambientG, ambientB;
	FLOAT emissiveR, emissiveG, emissiveB,alpha;

	xbox::X_D3DMATERIAL8 Material;
	DWORD ambient;

	//Material.Diffuse.a=alpha;

	DWORD colorMaterial = pg->KelvinPrimitive.SetColorMaterial;//NV097_SET_COLOR_MATERIAL, colorMaterial  // 0x298
	// Set scene ambient color,
	extern D3DCOLOR FromVector(D3DCOLORVALUE v);
	D3DCOLORVALUE fullColor = { 1.0,1.0,1.0,1.0 };
	D3DCOLORVALUE zeroColor = { 0,0,0,0 };

	//set material's ambient/diffuse/specular color to full color since the xbox material color was multiplied in each light color except the emissive color of each material.
	NV2A_SceneMateirals[0].Ambient = fullColor;
	NV2A_SceneMateirals[0].Diffuse = fullColor;
	NV2A_SceneMateirals[0].Specular = fullColor;
	
	NV2A_SceneMateirals[1].Ambient = fullColor;
	NV2A_SceneMateirals[1].Diffuse = fullColor;
	NV2A_SceneMateirals[1].Specular = fullColor;


	// Set scene mateiral emission and alpha
	NV2A_SceneMateirals[0].Emissive = *(D3DCOLORVALUE*)&(pg->KelvinPrimitive.SetMaterialEmission);//NV097_SET_MATERIAL_EMISSION
	NV2A_SceneMateirals[0].Diffuse.a = pg->KelvinPrimitive.SetMaterialAlpha;//NV097_SET_MATERIAL_ALPHA

	NV2A_SceneMateirals[1].Emissive = *(D3DCOLORVALUE*)&(pg->KelvinPrimitive.SetBackMaterialEmission);//NV097_SET_BACK_MATERIAL_EMISSION
	NV2A_SceneMateirals[1].Diffuse.a = pg->KelvinPrimitive.SetBackMaterialAlpha;//NV097_SET_BACK_MATERIAL_EMISSION

	/*
	NV2A_SceneAmbient[0] = *(D3DCOLORVALUE*)&(pg->KelvinPrimitive.SetSceneAmbientColor);//NV097_SET_SCENE_AMBIENT_COLOR
	NV2A_SceneAmbient[0].a = 1.0;
	NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_AMBIENT, FromVector(NV2A_SceneAmbient[0]));

	NV2A_SceneAmbient[1] = *(D3DCOLORVALUE*)&(pg->KelvinPrimitive.SetBackSceneAmbientColor);//NV097_SET_BACK_SCENE_AMBIENT_COLOR
	NV2A_SceneAmbient[1].a = 1.0;
	NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_BACKAMBIENT, FromVector(NV2A_SceneAmbient[1]));
    */

/*
*
DWORD i = D3D__RenderState[D3DRS_TWOSIDEDLIGHTING];
DWORD ambient = D3D__RenderState[D3DRS_AMBIENT];
D3DMATERIAL8* pMaterial = &pDevice->m_Material;

PushCount(pPush, NV097_SET_SCENE_AMBIENT_COLOR(0), 3);
PushCount(pPush + 4, NV097_SET_MATERIAL_EMISSION(0), 3);
PushCount(pPush + 8, NV097_SET_MATERIAL_ALPHA, 1);
*/
	float colorScale = 2.1;
	emissiveR = pg->KelvinPrimitive.SetMaterialEmission[0];
	emissiveG = pg->KelvinPrimitive.SetMaterialEmission[1];
	emissiveB = pg->KelvinPrimitive.SetMaterialEmission[2];

	ambientR = pg->KelvinPrimitive.SetSceneAmbientColor[0];
	ambientG = pg->KelvinPrimitive.SetSceneAmbientColor[1];
	ambientB = pg->KelvinPrimitive.SetSceneAmbientColor[2];

	alpha = pg->KelvinPrimitive.SetMaterialAlpha;

	if ((colorMaterial & 0xC) !=0)//		NV097_SET_COLOR_MATERIAL_AMBIENT_MATERIAL bit 3:2
	{
		/*
		 // Ambient is being pulled from the vertex, use D3DMCS_COLOR1 or D3DMCS_COLOR2 as material ambient source:

			emissiveR = ambientR;
			emissiveG = ambientG;
			emissiveB = ambientB;

			ambientR = pMaterial->Emissive.r;
			ambientG = pMaterial->Emissive.g;
			ambientB = pMaterial->Emissive.b;
		*/
		NV2A_SceneMateirals[0].Emissive.r= ambientR;
		NV2A_SceneMateirals[0].Emissive.g= ambientG;
		NV2A_SceneMateirals[0].Emissive.b= ambientB;
		NV2A_SceneMateirals[0].Emissive.a = 1.0f;
		// value of material ambient color is don't care since the color source is from default color.
		NV2A_SceneMateirals[0].Ambient.r = 1.0f;
		NV2A_SceneMateirals[0].Ambient.g = 1.0f;
		NV2A_SceneMateirals[0].Ambient.b = 1.0f;
		NV2A_SceneMateirals[0].Ambient.a = 1.0f;

		ambientR = emissiveR;
		ambientG = emissiveG;
		ambientB = emissiveB;
	}
	else if ((colorMaterial &0x3)!=0) //NV097_SET_COLOR_MATERIAL_EMISSIVE_MATERIAL bit 1:0
	{
		/*
		// Emissive is being pulled from the vertex, and ambient is not:

		ambientR = ambientR * pMaterial->Ambient.r;
		ambientG = ambientG * pMaterial->Ambient.g;
		ambientB = ambientB * pMaterial->Ambient.b;

		emissiveR = 1.0f;
		emissiveG = 1.0f;
		emissiveB = 1.0f;
		*/
		// value of material emissive color is don't care since the color source is from default color.
		NV2A_SceneMateirals[0].Emissive.r = 1.0f;
		NV2A_SceneMateirals[0].Emissive.g = 1.0f;
		NV2A_SceneMateirals[0].Emissive.b = 1.0f;
		NV2A_SceneMateirals[0].Emissive.a = 1.0f;

		//ambientR = ambientR / NV2A_SceneMateirals[0].Ambient.r;
		//ambientG = ambientG / NV2A_SceneMateirals[0].Ambient.g;
		//ambientB = ambientB / NV2A_SceneMateirals[0].Ambient.b;
        //hack:set material ambient to 1.0, and leave all ambientR/G/B to scene ambient.
		//todo:figure out either to leave ambientR/G/b to scene ambient or material enbient is better.
		NV2A_SceneMateirals[0].Ambient.r = 1.0f;
		NV2A_SceneMateirals[0].Ambient.g = 1.0f;
		NV2A_SceneMateirals[0].Ambient.b = 1.0f;
		NV2A_SceneMateirals[0].Ambient.a = 1.0f;
	}
	else
	{
		/*
		 // Neither ambient nor emissive is being pulled from the vertex:

			ambientR = (ambientR * pMaterial->Ambient.r) + pMaterial->Emissive.r;
			ambientG = (ambientG * pMaterial->Ambient.g) + pMaterial->Emissive.g;
			ambientB = (ambientB * pMaterial->Ambient.b) + pMaterial->Emissive.b;

			emissiveR = 0.0f;
			emissiveG = 0.0f;
			emissiveB = 0.0f;
		*/
		//hack:set material ambient to 0.0, and leave all ambientR/G/B to scene ambient.
		//todo:figure out either to leave ambientR/G/b to scene ambient or material enbient is better.
		NV2A_SceneMateirals[0].Emissive.r = 0.0f;
		NV2A_SceneMateirals[0].Emissive.g = 0.0f;
		NV2A_SceneMateirals[0].Emissive.b = 0.0f;
		NV2A_SceneMateirals[0].Emissive.a = 0.0f;

		//hack:set material ambient to 1.0, and leave all ambientR/G/B to scene ambient.
		//todo:figure out either to leave ambientR/G/b to scene ambient or material enbient is better.
		NV2A_SceneMateirals[0].Ambient.r = 1.0f;
		NV2A_SceneMateirals[0].Ambient.g = 1.0f;
		NV2A_SceneMateirals[0].Ambient.b = 1.0f;
		NV2A_SceneMateirals[0].Ambient.a = 1.0f;
		/*
		ambientR = (ambientR - NV2A_SceneMateirals[0].Emissive.r) / NV2A_SceneMateirals[0].Ambient.r;
		ambientG = (ambientG - NV2A_SceneMateirals[0].Emissive.g) / NV2A_SceneMateirals[0].Ambient.g;
		ambientB = (ambientB - NV2A_SceneMateirals[0].Emissive.b) / NV2A_SceneMateirals[0].Ambient.b;
		*/
	}
	NV2A_SceneAmbient[0].r = colorScale * ambientR;//NV097_SET_SCENE_AMBIENT_COLOR
	NV2A_SceneAmbient[0].g = colorScale * ambientG;
	NV2A_SceneAmbient[0].b = colorScale * ambientB;
	NV2A_SceneAmbient[0].a = alpha;
	NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_AMBIENT, FromVector(NV2A_SceneAmbient[0]));

	//skip back scene ambient and back material ambient/emissive setting when D3DRS_TWOSIDELIGHTING is not enabled.
	if (pg->KelvinPrimitive.SetTwoSidedLightEn == 0)
		return;

	// set back side ambient and materials
	emissiveR = pg->KelvinPrimitive.SetBackMaterialEmission[0];
	emissiveG = pg->KelvinPrimitive.SetBackMaterialEmission[1];
	emissiveB = pg->KelvinPrimitive.SetBackMaterialEmission[2];

	ambientR = pg->KelvinPrimitive.SetBackSceneAmbientColor[0];
	ambientG = pg->KelvinPrimitive.SetBackSceneAmbientColor[1];
	ambientB = pg->KelvinPrimitive.SetBackSceneAmbientColor[2];

	alpha = pg->KelvinPrimitive.SetMaterialAlpha;

	if ((colorMaterial & 0xC) != 0)//		NV097_SET_COLOR_MATERIAL_AMBIENT_MATERIAL bit 3:2
	{
		/*
		 // Ambient is being pulled from the vertex, use D3DMCS_COLOR1 or D3DMCS_COLOR2 as material ambient source:

			emissiveR = ambientR;
			emissiveG = ambientG;
			emissiveB = ambientB;

			ambientR = pMaterial->Emissive.r;
			ambientG = pMaterial->Emissive.g;
			ambientB = pMaterial->Emissive.b;
		*/
		NV2A_SceneMateirals[1].Emissive.r = ambientR;
		NV2A_SceneMateirals[1].Emissive.g = ambientG;
		NV2A_SceneMateirals[1].Emissive.b = ambientB;
		NV2A_SceneMateirals[1].Emissive.a = 1.0f;
		// value of material ambient color is don't care since the color source is from default color.
		NV2A_SceneMateirals[1].Ambient.r = 1.0f;
		NV2A_SceneMateirals[1].Ambient.g = 1.0f;
		NV2A_SceneMateirals[1].Ambient.b = 1.0f;
		NV2A_SceneMateirals[1].Ambient.a = 1.0f;

		ambientR = emissiveR;
		ambientG = emissiveG;
		ambientB = emissiveB;
	}
	else if ((colorMaterial & 0x3) != 0) //NV097_SET_COLOR_MATERIAL_EMISSIVE_MATERIAL bit 1:0
	{
		/*
		// Emissive is being pulled from the vertex, and ambient is not:

		ambientR = ambientR * pMaterial->Ambient.r;
		ambientG = ambientG * pMaterial->Ambient.g;
		ambientB = ambientB * pMaterial->Ambient.b;

		emissiveR = 1.0f;
		emissiveG = 1.0f;
		emissiveB = 1.0f;
		*/
		// value of material emissive color is don't care since the color source is from default color.
		NV2A_SceneMateirals[1].Emissive.r = 1.0f;
		NV2A_SceneMateirals[1].Emissive.g = 1.0f;
		NV2A_SceneMateirals[1].Emissive.b = 1.0f;
		NV2A_SceneMateirals[1].Emissive.a = 1.0f;

		//ambientR = ambientR / NV2A_SceneMateirals[0].Ambient.r;
		//ambientG = ambientG / NV2A_SceneMateirals[0].Ambient.g;
		//ambientB = ambientB / NV2A_SceneMateirals[0].Ambient.b;
		//hack:set material ambient to 1.0, and leave all ambientR/G/B to scene ambient.
		//todo:figure out either to leave ambientR/G/b to scene ambient or material enbient is better.
		NV2A_SceneMateirals[1].Ambient.r = 1.0f;
		NV2A_SceneMateirals[1].Ambient.g = 1.0f;
		NV2A_SceneMateirals[1].Ambient.b = 1.0f;
		NV2A_SceneMateirals[1].Ambient.a = 1.0f;
	}
	else
	{
		/*
		 // Neither ambient nor emissive is being pulled from the vertex:

			ambientR = (ambientR * pMaterial->Ambient.r) + pMaterial->Emissive.r;
			ambientG = (ambientG * pMaterial->Ambient.g) + pMaterial->Emissive.g;
			ambientB = (ambientB * pMaterial->Ambient.b) + pMaterial->Emissive.b;

			emissiveR = 0.0f;
			emissiveG = 0.0f;
			emissiveB = 0.0f;
		*/
		//hack:set material ambient to 0.0, and leave all ambientR/G/B to scene ambient.
		//todo:figure out either to leave ambientR/G/b to scene ambient or material enbient is better.
		NV2A_SceneMateirals[1].Emissive.r = 0.0f;
		NV2A_SceneMateirals[1].Emissive.g = 0.0f;
		NV2A_SceneMateirals[1].Emissive.b = 0.0f;
		NV2A_SceneMateirals[1].Emissive.a = 0.0f;

		//hack:set material ambient to 1.0, and leave all ambientR/G/B to scene ambient.
		//todo:figure out either to leave ambientR/G/b to scene ambient or material enbient is better.
		NV2A_SceneMateirals[1].Ambient.r = 1.0f;
		NV2A_SceneMateirals[1].Ambient.g = 1.0f;
		NV2A_SceneMateirals[1].Ambient.b = 1.0f;
		NV2A_SceneMateirals[1].Ambient.a = 1.0f;
		/*
		ambientR = (ambientR - NV2A_SceneMateirals[0].Emissive.r) / NV2A_SceneMateirals[0].Ambient.r;
		ambientG = (ambientG - NV2A_SceneMateirals[0].Emissive.g) / NV2A_SceneMateirals[0].Ambient.g;
		ambientB = (ambientB - NV2A_SceneMateirals[0].Emissive.b) / NV2A_SceneMateirals[0].Ambient.b;
		*/
	}
	NV2A_SceneAmbient[1].r = colorScale * ambientR;//NV097_SET_SCENE_AMBIENT_COLOR
	NV2A_SceneAmbient[1].g = colorScale * ambientG;
	NV2A_SceneAmbient[1].b = colorScale * ambientB;
	NV2A_SceneAmbient[1].a = alpha;
	NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_BACKAMBIENT, FromVector(NV2A_SceneAmbient[1]));

}

struct ReverseExpluteInfo
{
	float Power;
	float N;
	float N1;
};
// specular power look up table, look up power via N or N1. This table is generated by stepping Power from 0 to 100 in 0.1 and calculate N/N1.
// todo: N1 is not used in look up, we could remove N1 from 
ReverseExpluteInfo g_ReverseExpluteTable[]= 
{
//Power	,   N	    ,   N1
{ 	0	, 	0	    , 	0	    },//index 0, look up power with N1 from N1=0.0 to N1<=0.299999  // when N1<=0.6, we could use a quick hack Power=N1*2.0f
{ 	0.1	, 	0.1     , 	0.05	},
{ 	0.2	, 	0.2     , 	0.1	    },
{ 	0.3	, 	0.299999, 	0.15	},
{ 	0.4	, 	0.399957, 	0.2	    },
{ 	0.5	, 	0.499634, 	0.25	},
{ 	0.6	, 	0.598438, 	0.299999},// when N1> 0.299999f and N1<=1.0f, we could use Power= 0.2125f*N1*N1+1.8075f*N1+0.0417f
{ 	0.7	, 	0.695531, 	0.349991},
{ 	0.8	, 	0.790056, 	0.399957},
{ 	0.9	, 	0.881298, 	0.449859},
{ 	1	, 	1.49459	, 	0.499634},
{ 	1.1	, 	1.48496	, 	0.549194},
{ 	1.2	, 	1.47616	, 	0.598438},
{ 	1.3	, 	1.47184	, 	0.647254},
{ 	1.4	, 	1.47325	, 	0.695531},
{ 	1.5	, 	1.47457	, 	0.743164},
{ 	1.6	, 	1.48284	, 	0.790056},
{ 	1.7	, 	1.53794	, 	0.836124},
{ 	1.8	, 	1.58988	, 	0.881298},
{ 	1.9	, 	1.63902	, 	0.925521},//index 19, if N1<=0.925521, use N1 as key to look up Power.// else if N>=1.63902f, use N as key to look up Power.
{ 	2	, 	1.68564	, 	1.49459	},    
{ 	2.1	, 	1.7312	, 	1.48966	},
{ 	2.2	, 	1.77465	, 	1.48496	},
{ 	2.3	, 	1.81616	, 	1.48047	},
{ 	2.4	, 	1.85591	, 	1.47616	},
{ 	2.5	, 	1.89403	, 	1.47204	},
{ 	2.6	, 	1.92841	, 	1.47184	},
{ 	2.7	, 	1.96094	, 	1.47256	},
{ 	2.8	, 	1.99228	, 	1.47325	},
{ 	2.9	, 	2.02253	, 	1.47392	},
{ 	3	, 	2.05175	, 	1.47457	},
{ 	3.1	, 	2.08002	, 	1.4752	},
{ 	3.2	, 	2.10686	, 	1.48284	},
{ 	3.3	, 	2.13134	, 	1.5108	},
{ 	3.4	, 	2.15509	, 	1.53794	},
{ 	3.5	, 	2.17815	, 	1.56428	},
{ 	3.6	, 	2.20056	, 	1.58988	},
{ 	3.7	, 	2.22236	, 	1.61478	},
{ 	3.8	, 	2.24357	, 	1.63902	},
{ 	3.9	, 	2.26424	, 	1.66263	},
{ 	4	, 	2.28438	, 	1.68563	},
{ 	4.1	, 	2.30151	, 	1.7087	},
{ 	4.2	, 	2.31823	, 	1.7312	},
{ 	4.3	, 	2.33456	, 	1.75318	},
{ 	4.4	, 	2.35051	, 	1.77465	},
{ 	4.5	, 	2.3661	, 	1.79563	},
{ 	4.6	, 	2.38135	, 	1.81616	},
{ 	4.7	, 	2.39627	, 	1.83624	},
{ 	4.8	, 	2.41087	, 	1.85591	},
{ 	4.9	, 	2.42518	, 	1.87516	},
{ 	5	, 	2.4392	, 	1.89403	},
{ 	5.1	, 	2.45206	, 	1.91167	},
{ 	5.2	, 	2.4641	, 	1.92841	},
{ 	5.3	, 	2.47592	, 	1.94482	},
{ 	5.4	, 	2.48751	, 	1.96094	},
{ 	5.5	, 	2.49889	, 	1.97675	},
{ 	5.6	, 	2.51007	, 	1.99228	},
{ 	5.7	, 	2.52104	, 	2.00754	},
{ 	5.8	, 	2.53183	, 	2.02253	},
{ 	5.9	, 	2.54243	, 	2.03727	},
{ 	6	, 	2.55286	, 	2.05175	},
{ 	6.1	, 	2.56311	, 	2.066	},
{ 	6.2	, 	2.57319	, 	2.08002	},
{ 	6.3	, 	2.58312	, 	2.09381	},
{ 	6.4	, 	2.5923	, 	2.10686	},
{ 	6.5	, 	2.60078	, 	2.11919	},
{ 	6.6	, 	2.60913	, 	2.13134	},
{ 	6.7	, 	2.61735	, 	2.1433	},
{ 	6.8	, 	2.62546	, 	2.15509	},
{ 	6.9	, 	2.63344	, 	2.1667	},
{ 	7	, 	2.64131	, 	2.17815	},
{ 	7.1	, 	2.64906	, 	2.18943	},
{ 	7.2	, 	2.65671	, 	2.20056	},
{ 	7.3	, 	2.66425	, 	2.21153	},
{ 	7.4	, 	2.67169	, 	2.22236	},
{ 	7.5	, 	2.67903	, 	2.23304	},
{ 	7.6	, 	2.68628	, 	2.24357	},
{ 	7.7	, 	2.69343	, 	2.25397	},
{ 	7.79999,2.70048	, 	2.26424	},
{ 	7.89999,2.70745	, 	2.27437	},
{ 	7.99999,2.71433	, 	2.28438	},
{ 	8.09999,2.71913	, 	2.293	},
{ 	8.2	, 	2.72387	, 	2.30151	},
{ 	8.3	, 	2.72856	, 	2.30992	},
{ 	8.4	, 	2.73319	, 	2.31823	},
{ 	8.5	, 	2.73777	, 	2.32644	},
{ 	8.6	, 	2.74229	, 	2.33456	},
{ 	8.7	, 	2.74676	, 	2.34258	},
{ 	8.8	, 	2.75118	, 	2.35051	},
{ 	8.9	, 	2.75555	, 	2.35835	},
{ 	9	, 	2.75986	, 	2.3661	},
{ 	9.1	, 	2.76414	, 	2.37376	},
{ 	9.2	, 	2.76836	, 	2.38135	},
{ 	9.3	, 	2.77254	, 	2.38885	},
{ 	9.4	, 	2.77668	, 	2.39627	},
{ 	9.5	, 	2.78077	, 	2.40361	},
{ 	9.6	, 	2.78482	, 	2.41087	},
{ 	9.7	, 	2.78882	, 	2.41806	},
{ 	9.8	, 	2.79279	, 	2.42518	},
{ 	9.9	, 	2.79671	, 	2.43222	},
{ 	10	, 	2.8006	, 	2.4392	},
{ 	10.1, 	2.80444	, 	2.44595	},
{ 	10.2, 	2.80824	, 	2.45206	},
{ 	10.3, 	2.812	, 	2.45811	},
{ 	10.4, 	2.81572	, 	2.4641	},
{ 	10.5, 	2.81941	, 	2.47004	},
{ 	10.6, 	2.82306	, 	2.47592	},
{ 	10.7, 	2.82668	, 	2.48174	},
{ 	10.8, 	2.83026	, 	2.48751	},
{ 	10.9, 	2.83381	, 	2.49323	},
{ 	11	, 	2.83733	, 	2.49889	},
{ 	11.1, 	2.84081	, 	2.5045	},
{ 	11.2, 	2.84427	, 	2.51007	},
{ 	11.3, 	2.84769	, 	2.51558	},
{ 	11.4, 	2.85109	, 	2.52104	},
{ 	11.5, 	2.85445	, 	2.52646	},
{ 	11.6, 	2.85779	, 	2.53183	},
{ 	11.7, 	2.86109	, 	2.53715	},
{ 	11.8, 	2.86437	, 	2.54243	},
{ 	11.9, 	2.86762	, 	2.54767	},
{ 	12	, 	2.87085	, 	2.55286	},
{ 	12.1, 	2.87404	, 	2.558	},
{ 	12.2, 	2.87722	, 	2.56311	},
{ 	12.3, 	2.88036	, 	2.56817	},
{ 	12.4, 	2.88348	, 	2.57319	},
{ 	12.5, 	2.88657	, 	2.57818	},
{ 	12.6, 	2.88964	, 	2.58312	},
{ 	12.7, 	2.89269	, 	2.58802	},
{ 	12.8, 	2.89557	, 	2.5923	},
{ 	12.9, 	2.89843	, 	2.59656	},
{ 	13	, 	2.90127	, 	2.60078	},
{ 	13.1, 	2.90409	, 	2.60497	},
{ 	13.2, 	2.90689	, 	2.60913	},
{ 	13.3, 	2.90966	, 	2.61326	},
{ 	13.4, 	2.91242	, 	2.61735	},
{ 	13.5, 	2.91515	, 	2.62142	},
{ 	13.6, 	2.91786	, 	2.62546	},
{ 	13.7, 	2.92056	, 	2.62946	},
{ 	13.8, 	2.92323	, 	2.63344	},
{ 	13.9, 	2.92589	, 	2.63739	},
{ 	14	, 	2.92852	, 	2.64131	},
{ 	14.1, 	2.93114	, 	2.6452	},
{ 	14.2, 	2.93374	, 	2.64906	},
{ 	14.3, 	2.93632	, 	2.6529	},
{ 	14.4, 	2.93888	, 	2.65671	},
{ 	14.5, 	2.94143	, 	2.6605	},
{ 	14.6, 	2.94395	, 	2.66426	},
{ 	14.7, 	2.94646	, 	2.66799	},
{ 	14.8, 	2.94896	, 	2.6717	},
{ 	14.9, 	2.95143	, 	2.67538	},
{ 	15	, 	2.95389	, 	2.67904	},
{ 	15.1, 	2.95634	, 	2.68267	},
{ 	15.2, 	2.95876	, 	2.68628	},
{ 	15.3, 	2.96117	, 	2.68986	},
{ 	15.4, 	2.96357	, 	2.69343	},
{ 	15.5, 	2.96595	, 	2.69697	},
{ 	15.6, 	2.96831	, 	2.70048	},
{ 	15.7, 	2.97066	, 	2.70398	},
{ 	15.8, 	2.973	, 	2.70745	},
{ 	15.9, 	2.97532	, 	2.7109	},
{ 	16	, 	2.97762	, 	2.71433	},
{ 	16.1, 	2.97959	, 	2.71674	},
{ 	16.2, 	2.98155	, 	2.71913	},
{ 	16.3, 	2.9835	, 	2.72151	},
{ 	16.4, 	2.98544	, 	2.72387	},
{ 	16.5, 	2.98736	, 	2.72622	},
{ 	16.6, 	2.98927	, 	2.72856	},
{ 	16.7, 	2.99117	, 	2.73088	},
{ 	16.8, 	2.99306	, 	2.73319	},
{ 	16.9, 	2.99494	, 	2.73549	},
{ 	17	, 	2.99681	, 	2.73777	},
{ 	17.1, 	2.99866	, 	2.74003	},
{ 	17.2, 	3.00051	, 	2.74229	},
{ 	17.3, 	3.00234	, 	2.74453	},
{ 	17.4, 	3.00417	, 	2.74676	},
{ 	17.5, 	3.00598	, 	2.74897	},
{ 	17.6, 	3.00778	, 	2.75118	},
{ 	17.7, 	3.00958	, 	2.75337	},
{ 	17.8, 	3.01136	, 	2.75555	},
{ 	17.9, 	3.01313	, 	2.75771	},
{ 	18	, 	3.0149	, 	2.75987	},
{ 	18.1, 	3.01665	, 	2.76201	},
{ 	18.2, 	3.01839	, 	2.76414	},
{ 	18.3, 	3.02013	, 	2.76626	},
{ 	18.4, 	3.02185	, 	2.76836	},
{ 	18.5, 	3.02357	, 	2.77046	},
{ 	18.6, 	3.02527	, 	2.77254	},
{ 	18.7, 	3.02697	, 	2.77462	},
{ 	18.8, 	3.02866	, 	2.77668	},
{ 	18.9, 	3.03034	, 	2.77873	},
{ 	19	, 	3.03201	, 	2.78077	},
{ 	19.1, 	3.03367	, 	2.7828	},
{ 	19.2, 	3.03532	, 	2.78482	},
{ 	19.3, 	3.03697	, 	2.78683	},
{ 	19.4, 	3.0386	, 	2.78882	},
{ 	19.5, 	3.04023	, 	2.79081	},
{ 	19.6, 	3.04185	, 	2.79279	},
{ 	19.7, 	3.04346	, 	2.79476	},
{ 	19.8, 	3.04506	, 	2.79671	},
{ 	19.9, 	3.04665	, 	2.79866	},
{ 	20	, 	3.04824	, 	2.8006	},
{ 	20.1, 	3.04982	, 	2.80253	},
{ 	20.2, 	3.05089	, 	2.80444	},// index 242, end of array, array element count 243
#if 0
{ 	20.3	, 	3.05124	, 	2.80635	},
{ 	20.4	, 	3.0516	, 	2.80824	},
{ 	20.5	, 	3.05195	, 	2.81012	},
{ 	20.6	, 	3.0523	, 	2.812	},
{ 	20.7	, 	3.05265	, 	2.81386	},
{ 	20.8	, 	3.053	, 	2.81572	},
{ 	20.9	, 	3.05334	, 	2.81757	},
{ 	21	, 	3.05369	, 	2.81941	},
{ 	21.1	, 	3.05403	, 	2.82124	},
{ 	21.2	, 	3.05437	, 	2.82306	},
{ 	21.3	, 	3.05471	, 	2.82487	},
{ 	21.4	, 	3.05505	, 	2.82668	},
{ 	21.5	, 	3.05538	, 	2.82847	},
{ 	21.6	, 	3.05572	, 	2.83026	},
{ 	21.7	, 	3.05605	, 	2.83204	},
{ 	21.8	, 	3.05638	, 	2.83381	},
{ 	21.9	, 	3.05671	, 	2.83557	},
{ 	22	, 	3.05704	, 	2.83733	},
{ 	22.1	, 	3.05737	, 	2.83908	},
{ 	22.2	, 	3.05769	, 	2.84082	},
{ 	22.3	, 	3.05802	, 	2.84255	},
{ 	22.4	, 	3.05834	, 	2.84427	},
{ 	22.5	, 	3.05866	, 	2.84599	},
{ 	22.6	, 	3.05898	, 	2.84769	},
{ 	22.7001	, 	3.0593	, 	2.8494	},
{ 	22.8001	, 	3.05962	, 	2.85109	},
{ 	22.9001	, 	3.05993	, 	2.85277	},
{ 	23.0001	, 	3.06025	, 	2.85445	},
{ 	23.1001	, 	3.06056	, 	2.85612	},
{ 	23.2001	, 	3.06087	, 	2.85779	},
{ 	23.3001	, 	3.06118	, 	2.85945	},
{ 	23.4001	, 	3.06149	, 	2.8611	},
{ 	23.5001	, 	3.0618	, 	2.86274	},
{ 	23.6001	, 	3.0621	, 	2.86437	},
{ 	23.7001	, 	3.06241	, 	2.866	},
{ 	23.8001	, 	3.06271	, 	2.86762	},
{ 	23.9001	, 	3.06301	, 	2.86924	},
{ 	24.0001	, 	3.06331	, 	2.87085	},
{ 	24.1001	, 	3.06361	, 	2.87245	},
{ 	24.2001	, 	3.06391	, 	2.87405	},
{ 	24.3001	, 	3.06421	, 	2.87563	},
{ 	24.4001	, 	3.0645	, 	2.87722	},
{ 	24.5001	, 	3.0648	, 	2.87879	},
{ 	24.6001	, 	3.06509	, 	2.88036	},
{ 	24.7001	, 	3.06539	, 	2.88192	},
{ 	24.8001	, 	3.06568	, 	2.88348	},
{ 	24.9001	, 	3.06597	, 	2.88503	},
{ 	25.0001	, 	3.06626	, 	2.88657	},
{ 	25.1001	, 	3.06654	, 	2.88811	},
{ 	25.2001	, 	3.06683	, 	2.88964	},
{ 	25.3001	, 	3.06712	, 	2.89117	},
{ 	25.4001	, 	3.06741	, 	2.89269	},
{ 	25.5001	, 	3.06833	, 	2.89413	},
{ 	25.6001	, 	3.06924	, 	2.89557	},
{ 	25.7001	, 	3.07015	, 	2.89701	},
{ 	25.8001	, 	3.07106	, 	2.89843	},
{ 	25.9001	, 	3.07197	, 	2.89986	},
{ 	26.0001	, 	3.07287	, 	2.90127	},
{ 	26.1001	, 	3.07376	, 	2.90269	},
{ 	26.2001	, 	3.07466	, 	2.90409	},
{ 	26.3001	, 	3.07555	, 	2.90549	},
{ 	26.4001	, 	3.07643	, 	2.90689	},
{ 	26.5001	, 	3.07732	, 	2.90828	},
{ 	26.6001	, 	3.0782	, 	2.90966	},
{ 	26.7001	, 	3.07907	, 	2.91104	},
{ 	26.8001	, 	3.07995	, 	2.91242	},
{ 	26.9001	, 	3.08082	, 	2.91379	},
{ 	27.0001	, 	3.08169	, 	2.91515	},
{ 	27.1001	, 	3.08255	, 	2.91651	},
{ 	27.2001	, 	3.08341	, 	2.91786	},
{ 	27.3001	, 	3.08427	, 	2.91921	},
{ 	27.4001	, 	3.08512	, 	2.92056	},
{ 	27.5001	, 	3.08597	, 	2.9219	},
{ 	27.6001	, 	3.08682	, 	2.92323	},
{ 	27.7001	, 	3.08767	, 	2.92456	},
{ 	27.8001	, 	3.08851	, 	2.92589	},
{ 	27.9001	, 	3.08935	, 	2.92721	},
{ 	28.0001	, 	3.09018	, 	2.92852	},
{ 	28.1001	, 	3.09102	, 	2.92983	},
{ 	28.2001	, 	3.09185	, 	2.93114	},
{ 	28.3001	, 	3.09267	, 	2.93244	},
{ 	28.4001	, 	3.0935	, 	2.93374	},
{ 	28.5001	, 	3.09432	, 	2.93503	},
{ 	28.6001	, 	3.09514	, 	2.93632	},
{ 	28.7001	, 	3.09595	, 	2.9376	},
{ 	28.8001	, 	3.09677	, 	2.93888	},
{ 	28.9001	, 	3.09758	, 	2.94016	},
{ 	29.0001	, 	3.09838	, 	2.94143	},
{ 	29.1001	, 	3.09919	, 	2.94269	},
{ 	29.2001	, 	3.09999	, 	2.94395	},
{ 	29.3001	, 	3.10079	, 	2.94521	},
{ 	29.4001	, 	3.10158	, 	2.94646	},
{ 	29.5001	, 	3.10238	, 	2.94771	},
{ 	29.6001	, 	3.10317	, 	2.94896	},
{ 	29.7001	, 	3.10396	, 	2.9502	},
{ 	29.8001	, 	3.10474	, 	2.95143	},
{ 	29.9001	, 	3.10553	, 	2.95266	},
{ 	30.0001	, 	3.10631	, 	2.95389	},
{ 	30.1001	, 	3.10708	, 	2.95512	},
{ 	30.2001	, 	3.10786	, 	2.95634	},
{ 	30.3001	, 	3.10863	, 	2.95755	},
{ 	30.4001	, 	3.1094	, 	2.95876	},
{ 	30.5001	, 	3.11017	, 	2.95997	},
{ 	30.6001	, 	3.11093	, 	2.96117	},
{ 	30.7001	, 	3.1117	, 	2.96237	},
{ 	30.8001	, 	3.11246	, 	2.96357	},
{ 	30.9001	, 	3.11321	, 	2.96476	},
{ 	31.0001	, 	3.11397	, 	2.96595	},
{ 	31.1001	, 	3.11472	, 	2.96713	},
{ 	31.2001	, 	3.11547	, 	2.96831	},
{ 	31.3001	, 	3.11622	, 	2.96949	},
{ 	31.4001	, 	3.11696	, 	2.97066	},
{ 	31.5001	, 	3.11771	, 	2.97183	},
{ 	31.6001	, 	3.11845	, 	2.973	},
{ 	31.7001	, 	3.11919	, 	2.97416	},
{ 	31.8001	, 	3.11992	, 	2.97532	},
{ 	31.9001	, 	3.12066	, 	2.97647	},
{ 	32.0001	, 	3.12139	, 	2.97762	},
{ 	32.1001	, 	3.12193	, 	2.97861	},
{ 	32.2001	, 	3.12246	, 	2.97959	},
{ 	32.3001	, 	3.123	, 	2.98058	},
{ 	32.4001	, 	3.12353	, 	2.98155	},
{ 	32.5001	, 	3.12407	, 	2.98253	},
{ 	32.6001	, 	3.1246	, 	2.9835	},
{ 	32.7001	, 	3.12513	, 	2.98447	},
{ 	32.8001	, 	3.12565	, 	2.98544	},
{ 	32.9001	, 	3.12618	, 	2.9864	},
{ 	33.0001	, 	3.1267	, 	2.98736	},
{ 	33.1001	, 	3.12723	, 	2.98832	},
{ 	33.2001	, 	3.12775	, 	2.98927	},
{ 	33.3001	, 	3.12827	, 	2.99022	},
{ 	33.4001	, 	3.12878	, 	2.99117	},
{ 	33.5001	, 	3.1293	, 	2.99212	},
{ 	33.6001	, 	3.12982	, 	2.99306	},
{ 	33.7001	, 	3.13033	, 	2.994	},
{ 	33.8001	, 	3.13084	, 	2.99494	},
{ 	33.9001	, 	3.13135	, 	2.99588	},
{ 	34.0001	, 	3.13186	, 	2.99681	},
{ 	34.1001	, 	3.13237	, 	2.99774	},
{ 	34.2001	, 	3.13287	, 	2.99866	},
{ 	34.3	, 	3.13338	, 	2.99959	},
{ 	34.4	, 	3.13388	, 	3.00051	},
{ 	34.5	, 	3.13438	, 	3.00143	},
{ 	34.6	, 	3.13488	, 	3.00234	},
{ 	34.7	, 	3.13538	, 	3.00326	},
{ 	34.8	, 	3.13588	, 	3.00417	},
{ 	34.9	, 	3.13637	, 	3.00508	},
{ 	35	, 	3.13687	, 	3.00598	},
{ 	35.1	, 	3.13736	, 	3.00688	},
{ 	35.2	, 	3.13785	, 	3.00778	},
{ 	35.3	, 	3.13834	, 	3.00868	},
{ 	35.4	, 	3.13883	, 	3.00958	},
{ 	35.5	, 	3.13932	, 	3.01047	},
{ 	35.6	, 	3.13981	, 	3.01136	},
{ 	35.7	, 	3.14029	, 	3.01225	},
{ 	35.8	, 	3.14077	, 	3.01313	},
{ 	35.9	, 	3.14126	, 	3.01402	},
{ 	36	, 	3.14174	, 	3.0149	},
{ 	36.1	, 	3.14222	, 	3.01577	},
{ 	36.2	, 	3.14269	, 	3.01665	},
{ 	36.3	, 	3.14317	, 	3.01752	},
{ 	36.4	, 	3.14364	, 	3.01839	},
{ 	36.5	, 	3.14412	, 	3.01926	},
{ 	36.6	, 	3.14459	, 	3.02013	},
{ 	36.7	, 	3.14506	, 	3.02099	},
{ 	36.8	, 	3.14553	, 	3.02185	},
{ 	36.9	, 	3.146	, 	3.02271	},
{ 	37	, 	3.14647	, 	3.02357	},
{ 	37.1	, 	3.14694	, 	3.02442	},
{ 	37.2	, 	3.1474	, 	3.02527	},
{ 	37.3	, 	3.14786	, 	3.02612	},
{ 	37.4	, 	3.14833	, 	3.02697	},
{ 	37.5	, 	3.14879	, 	3.02782	},
{ 	37.6	, 	3.14925	, 	3.02866	},
{ 	37.7	, 	3.14971	, 	3.0295	},
{ 	37.8	, 	3.15017	, 	3.03034	},
{ 	37.9	, 	3.15062	, 	3.03117	},
{ 	38	, 	3.15108	, 	3.03201	},
{ 	38.1	, 	3.15153	, 	3.03284	},
{ 	38.2	, 	3.15198	, 	3.03367	},
{ 	38.3	, 	3.15244	, 	3.0345	},
{ 	38.4	, 	3.15289	, 	3.03532	},
{ 	38.5	, 	3.15334	, 	3.03614	},
{ 	38.6	, 	3.15378	, 	3.03696	},
{ 	38.7	, 	3.15423	, 	3.03778	},
{ 	38.8	, 	3.15468	, 	3.0386	},
{ 	38.9	, 	3.15512	, 	3.03941	},
{ 	39	, 	3.15556	, 	3.04023	},
{ 	39.1	, 	3.15601	, 	3.04104	},
{ 	39.2	, 	3.15645	, 	3.04185	},
{ 	39.3	, 	3.15689	, 	3.04265	},
{ 	39.4	, 	3.15733	, 	3.04346	},
{ 	39.5	, 	3.15777	, 	3.04426	},
{ 	39.6	, 	3.1582	, 	3.04506	},
{ 	39.7	, 	3.15864	, 	3.04586	},
{ 	39.8	, 	3.15907	, 	3.04665	},
{ 	39.9	, 	3.15951	, 	3.04745	},
{ 	40	, 	3.15994	, 	3.04824	},
{ 	40.1	, 	3.16037	, 	3.04903	},
{ 	40.2	, 	3.1608	, 	3.04982	},
{ 	40.3	, 	3.16123	, 	3.0506	},
{ 	40.4	, 	3.16135	, 	3.05089	},
{ 	40.5	, 	3.16139	, 	3.05107	},
{ 	40.6	, 	3.16144	, 	3.05124	},
{ 	40.7	, 	3.16149	, 	3.05142	},
{ 	40.7999	, 	3.16154	, 	3.0516	},
{ 	40.8999	, 	3.16159	, 	3.05177	},
{ 	40.9999	, 	3.16163	, 	3.05195	},
{ 	41.0999	, 	3.16168	, 	3.05213	},
{ 	41.1999	, 	3.16173	, 	3.0523	},
{ 	41.2999	, 	3.16178	, 	3.05248	},
{ 	41.3999	, 	3.16182	, 	3.05265	},
{ 	41.4999	, 	3.16187	, 	3.05282	},
{ 	41.5999	, 	3.16192	, 	3.053	},
{ 	41.6999	, 	3.16196	, 	3.05317	},
{ 	41.7999	, 	3.16201	, 	3.05334	},
{ 	41.8999	, 	3.16206	, 	3.05352	},
{ 	41.9999	, 	3.1621	, 	3.05369	},
{ 	42.0999	, 	3.16215	, 	3.05386	},
{ 	42.1999	, 	3.1622	, 	3.05403	},
{ 	42.2999	, 	3.16224	, 	3.0542	},
{ 	42.3999	, 	3.16229	, 	3.05437	},
{ 	42.4999	, 	3.16234	, 	3.05454	},
{ 	42.5999	, 	3.16238	, 	3.05471	},
{ 	42.6999	, 	3.16243	, 	3.05488	},
{ 	42.7999	, 	3.16247	, 	3.05505	},
{ 	42.8999	, 	3.16252	, 	3.05522	},
{ 	42.9999	, 	3.16256	, 	3.05538	},
{ 	43.0999	, 	3.16261	, 	3.05555	},
{ 	43.1999	, 	3.16266	, 	3.05572	},
{ 	43.2999	, 	3.1627	, 	3.05589	},
{ 	43.3999	, 	3.16275	, 	3.05605	},
{ 	43.4999	, 	3.16279	, 	3.05622	},
{ 	43.5999	, 	3.16284	, 	3.05638	},
{ 	43.6999	, 	3.16288	, 	3.05655	},
{ 	43.7999	, 	3.16293	, 	3.05671	},
{ 	43.8999	, 	3.16297	, 	3.05688	},
{ 	43.9999	, 	3.16301	, 	3.05704	},
{ 	44.0999	, 	3.16306	, 	3.0572	},
{ 	44.1999	, 	3.1631	, 	3.05737	},
{ 	44.2999	, 	3.16315	, 	3.05753	},
{ 	44.3999	, 	3.16319	, 	3.05769	},
{ 	44.4999	, 	3.16324	, 	3.05786	},
{ 	44.5999	, 	3.16328	, 	3.05802	},
{ 	44.6999	, 	3.16332	, 	3.05818	},
{ 	44.7999	, 	3.16337	, 	3.05834	},
{ 	44.8999	, 	3.16341	, 	3.0585	},
{ 	44.9999	, 	3.16345	, 	3.05866	},
{ 	45.0999	, 	3.1635	, 	3.05882	},
{ 	45.1999	, 	3.16354	, 	3.05898	},
{ 	45.2999	, 	3.16358	, 	3.05914	},
{ 	45.3999	, 	3.16363	, 	3.0593	},
{ 	45.4999	, 	3.16367	, 	3.05946	},
{ 	45.5999	, 	3.16371	, 	3.05962	},
{ 	45.6999	, 	3.16376	, 	3.05977	},
{ 	45.7999	, 	3.1638	, 	3.05993	},
{ 	45.8999	, 	3.16384	, 	3.06009	},
{ 	45.9999	, 	3.16388	, 	3.06025	},
{ 	46.0999	, 	3.16393	, 	3.0604	},
{ 	46.1999	, 	3.16397	, 	3.06056	},
{ 	46.2999	, 	3.16401	, 	3.06071	},
{ 	46.3999	, 	3.16405	, 	3.06087	},
{ 	46.4999	, 	3.1641	, 	3.06102	},
{ 	46.5999	, 	3.16414	, 	3.06118	},
{ 	46.6999	, 	3.16418	, 	3.06133	},
{ 	46.7999	, 	3.16422	, 	3.06149	},
{ 	46.8999	, 	3.16426	, 	3.06164	},
{ 	46.9999	, 	3.16431	, 	3.0618	},
{ 	47.0999	, 	3.16435	, 	3.06195	},
{ 	47.1999	, 	3.16439	, 	3.0621	},
{ 	47.2999	, 	3.16443	, 	3.06225	},
{ 	47.3998	, 	3.16447	, 	3.06241	},
{ 	47.4998	, 	3.16451	, 	3.06256	},
{ 	47.5998	, 	3.16455	, 	3.06271	},
{ 	47.6998	, 	3.16459	, 	3.06286	},
{ 	47.7998	, 	3.16464	, 	3.06301	},
{ 	47.8998	, 	3.16468	, 	3.06316	},
{ 	47.9998	, 	3.16472	, 	3.06331	},
{ 	48.0998	, 	3.16476	, 	3.06346	},
{ 	48.1998	, 	3.1648	, 	3.06361	},
{ 	48.2998	, 	3.16484	, 	3.06376	},
{ 	48.3998	, 	3.16488	, 	3.06391	},
{ 	48.4998	, 	3.16492	, 	3.06406	},
{ 	48.5998	, 	3.16496	, 	3.06421	},
{ 	48.6998	, 	3.165	, 	3.06436	},
{ 	48.7998	, 	3.16504	, 	3.0645	},
{ 	48.8998	, 	3.16508	, 	3.06465	},
{ 	48.9998	, 	3.16512	, 	3.0648	},
{ 	49.0998	, 	3.16516	, 	3.06495	},
{ 	49.1998	, 	3.1652	, 	3.06509	},
{ 	49.2998	, 	3.16524	, 	3.06524	},
{ 	49.3998	, 	3.16528	, 	3.06538	},
{ 	49.4998	, 	3.16532	, 	3.06553	},
{ 	49.5998	, 	3.16536	, 	3.06568	},
{ 	49.6998	, 	3.1654	, 	3.06582	},
{ 	49.7998	, 	3.16544	, 	3.06597	},
{ 	49.8998	, 	3.16548	, 	3.06611	},
{ 	49.9998	, 	3.16552	, 	3.06626	},
{ 	50.0998	, 	3.16555	, 	3.0664	},
{ 	50.1998	, 	3.16559	, 	3.06654	},
{ 	50.2998	, 	3.16563	, 	3.06669	},
{ 	50.3998	, 	3.16567	, 	3.06683	},
{ 	50.4998	, 	3.16571	, 	3.06697	},
{ 	50.5998	, 	3.16575	, 	3.06711	},
{ 	50.6998	, 	3.16579	, 	3.06726	},
{ 	50.7998	, 	3.16583	, 	3.06741	},
{ 	50.8998	, 	3.16611	, 	3.06787	},
{ 	50.9998	, 	3.16639	, 	3.06833	},
{ 	51.0998	, 	3.16667	, 	3.06878	},
{ 	51.1998	, 	3.16694	, 	3.06924	},
{ 	51.2998	, 	3.16722	, 	3.0697	},
{ 	51.3998	, 	3.16749	, 	3.07015	},
{ 	51.4998	, 	3.16777	, 	3.07061	},
{ 	51.5998	, 	3.16804	, 	3.07106	},
{ 	51.6998	, 	3.16831	, 	3.07151	},
{ 	51.7998	, 	3.16859	, 	3.07196	},
{ 	51.8998	, 	3.16886	, 	3.07241	},
{ 	51.9998	, 	3.16913	, 	3.07286	},
{ 	52.0998	, 	3.1694	, 	3.07331	},
{ 	52.1998	, 	3.16967	, 	3.07376	},
{ 	52.2998	, 	3.16994	, 	3.07421	},
{ 	52.3998	, 	3.17021	, 	3.07466	},
{ 	52.4998	, 	3.17048	, 	3.0751	},
{ 	52.5998	, 	3.17075	, 	3.07555	},
{ 	52.6998	, 	3.17102	, 	3.07599	},
{ 	52.7998	, 	3.17129	, 	3.07643	},
{ 	52.8998	, 	3.17156	, 	3.07687	},
{ 	52.9998	, 	3.17182	, 	3.07732	},
{ 	53.0998	, 	3.17209	, 	3.07776	},
{ 	53.1998	, 	3.17236	, 	3.0782	},
{ 	53.2998	, 	3.17262	, 	3.07863	},
{ 	53.3998	, 	3.17289	, 	3.07907	},
{ 	53.4998	, 	3.17315	, 	3.07951	},
{ 	53.5998	, 	3.17341	, 	3.07995	},
{ 	53.6998	, 	3.17368	, 	3.08038	},
{ 	53.7998	, 	3.17394	, 	3.08082	},
{ 	53.8997	, 	3.1742	, 	3.08125	},
{ 	53.9997	, 	3.17446	, 	3.08168	},
{ 	54.0997	, 	3.17473	, 	3.08212	},
{ 	54.1997	, 	3.17499	, 	3.08255	},
{ 	54.2997	, 	3.17525	, 	3.08298	},
{ 	54.3997	, 	3.17551	, 	3.08341	},
{ 	54.4997	, 	3.17577	, 	3.08384	},
{ 	54.5997	, 	3.17603	, 	3.08427	},
{ 	54.6997	, 	3.17628	, 	3.08469	},
{ 	54.7997	, 	3.17654	, 	3.08512	},
{ 	54.8997	, 	3.1768	, 	3.08555	},
{ 	54.9997	, 	3.17706	, 	3.08597	},
{ 	55.0997	, 	3.17731	, 	3.0864	},
{ 	55.1997	, 	3.17757	, 	3.08682	},
{ 	55.2997	, 	3.17783	, 	3.08724	},
{ 	55.3997	, 	3.17808	, 	3.08766	},
{ 	55.4997	, 	3.17834	, 	3.08809	},
{ 	55.5997	, 	3.17859	, 	3.08851	},
{ 	55.6997	, 	3.17884	, 	3.08893	},
{ 	55.7997	, 	3.1791	, 	3.08935	},
{ 	55.8997	, 	3.17935	, 	3.08976	},
{ 	55.9997	, 	3.1796	, 	3.09018	},
{ 	56.0997	, 	3.17985	, 	3.0906	},
{ 	56.1997	, 	3.18011	, 	3.09101	},
{ 	56.2997	, 	3.18036	, 	3.09143	},
{ 	56.3997	, 	3.18061	, 	3.09184	},
{ 	56.4997	, 	3.18086	, 	3.09226	},
{ 	56.5997	, 	3.18111	, 	3.09267	},
{ 	56.6997	, 	3.18136	, 	3.09308	},
{ 	56.7997	, 	3.18161	, 	3.0935	},
{ 	56.8997	, 	3.18186	, 	3.09391	},
{ 	56.9997	, 	3.1821	, 	3.09432	},
{ 	57.0997	, 	3.18235	, 	3.09473	},
{ 	57.1997	, 	3.1826	, 	3.09514	},
{ 	57.2997	, 	3.18285	, 	3.09554	},
{ 	57.3997	, 	3.18309	, 	3.09595	},
{ 	57.4997	, 	3.18334	, 	3.09636	},
{ 	57.5997	, 	3.18358	, 	3.09676	},
{ 	57.6997	, 	3.18383	, 	3.09717	},
{ 	57.7997	, 	3.18407	, 	3.09757	},
{ 	57.8997	, 	3.18432	, 	3.09798	},
{ 	57.9997	, 	3.18456	, 	3.09838	},
{ 	58.0997	, 	3.1848	, 	3.09878	},
{ 	58.1997	, 	3.18505	, 	3.09919	},
{ 	58.2997	, 	3.18529	, 	3.09959	},
{ 	58.3997	, 	3.18553	, 	3.09999	},
{ 	58.4997	, 	3.18577	, 	3.10039	},
{ 	58.5997	, 	3.18601	, 	3.10079	},
{ 	58.6997	, 	3.18626	, 	3.10118	},
{ 	58.7997	, 	3.1865	, 	3.10158	},
{ 	58.8997	, 	3.18674	, 	3.10198	},
{ 	58.9997	, 	3.18698	, 	3.10238	},
{ 	59.0997	, 	3.18722	, 	3.10277	},
{ 	59.1997	, 	3.18745	, 	3.10317	},
{ 	59.2997	, 	3.18769	, 	3.10356	},
{ 	59.3997	, 	3.18793	, 	3.10396	},
{ 	59.4997	, 	3.18817	, 	3.10435	},
{ 	59.5997	, 	3.18841	, 	3.10474	},
{ 	59.6997	, 	3.18864	, 	3.10513	},
{ 	59.7997	, 	3.18888	, 	3.10552	},
{ 	59.8997	, 	3.18912	, 	3.10591	},
{ 	59.9997	, 	3.18935	, 	3.1063	},
{ 	60.0997	, 	3.18959	, 	3.10669	},
{ 	60.1997	, 	3.18982	, 	3.10708	},
{ 	60.2997	, 	3.19006	, 	3.10747	},
{ 	60.3997	, 	3.19029	, 	3.10786	},
{ 	60.4996	, 	3.19052	, 	3.10824	},
{ 	60.5996	, 	3.19076	, 	3.10863	},
{ 	60.6996	, 	3.19099	, 	3.10901	},
{ 	60.7996	, 	3.19122	, 	3.1094	},
{ 	60.8996	, 	3.19145	, 	3.10978	},
{ 	60.9996	, 	3.19169	, 	3.11017	},
{ 	61.0996	, 	3.19192	, 	3.11055	},
{ 	61.1996	, 	3.19215	, 	3.11093	},
{ 	61.2996	, 	3.19238	, 	3.11131	},
{ 	61.3996	, 	3.19261	, 	3.11169	},
{ 	61.4996	, 	3.19284	, 	3.11207	},
{ 	61.5996	, 	3.19307	, 	3.11245	},
{ 	61.6996	, 	3.1933	, 	3.11283	},
{ 	61.7996	, 	3.19353	, 	3.11321	},
{ 	61.8996	, 	3.19376	, 	3.11359	},
{ 	61.9996	, 	3.19398	, 	3.11397	},
{ 	62.0996	, 	3.19421	, 	3.11434	},
{ 	62.1996	, 	3.19444	, 	3.11472	},
{ 	62.2996	, 	3.19467	, 	3.11509	},
{ 	62.3996	, 	3.19489	, 	3.11547	},
{ 	62.4996	, 	3.19512	, 	3.11584	},
{ 	62.5996	, 	3.19534	, 	3.11622	},
{ 	62.6996	, 	3.19557	, 	3.11659	},
{ 	62.7996	, 	3.19579	, 	3.11696	},
{ 	62.8996	, 	3.19602	, 	3.11733	},
{ 	62.9996	, 	3.19624	, 	3.1177	},
{ 	63.0996	, 	3.19647	, 	3.11808	},
{ 	63.1996	, 	3.19669	, 	3.11845	},
{ 	63.2996	, 	3.19692	, 	3.11881	},
{ 	63.3996	, 	3.19714	, 	3.11918	},
{ 	63.4996	, 	3.19736	, 	3.11955	},
{ 	63.5996	, 	3.19758	, 	3.11992	},
{ 	63.6996	, 	3.19781	, 	3.12029	},
{ 	63.7996	, 	3.19803	, 	3.12065	},
{ 	63.8996	, 	3.19825	, 	3.12102	},
{ 	63.9996	, 	3.19847	, 	3.12138	},
{ 	64.0996	, 	3.1981	, 	3.12165	},
{ 	64.1996	, 	3.19773	, 	3.12192	},
{ 	64.2996	, 	3.19735	, 	3.12219	},
{ 	64.3996	, 	3.19698	, 	3.12246	},
{ 	64.4996	, 	3.19661	, 	3.12273	},
{ 	64.5996	, 	3.19624	, 	3.123	},
{ 	64.6996	, 	3.19587	, 	3.12326	},
{ 	64.7996	, 	3.1955	, 	3.12353	},
{ 	64.8996	, 	3.19513	, 	3.1238	},
{ 	64.9996	, 	3.19477	, 	3.12406	},
{ 	65.0996	, 	3.1944	, 	3.12433	},
{ 	65.1996	, 	3.19403	, 	3.12459	},
{ 	65.2996	, 	3.19366	, 	3.12486	},
{ 	65.3996	, 	3.1933	, 	3.12512	},
{ 	65.4996	, 	3.19293	, 	3.12539	},
{ 	65.5996	, 	3.19257	, 	3.12565	},
{ 	65.6996	, 	3.1922	, 	3.12591	},
{ 	65.7996	, 	3.19184	, 	3.12618	},
{ 	65.8996	, 	3.19148	, 	3.12644	},
{ 	65.9996	, 	3.19111	, 	3.1267	},
{ 	66.0996	, 	3.19075	, 	3.12696	},
{ 	66.1996	, 	3.19039	, 	3.12722	},
{ 	66.2996	, 	3.19003	, 	3.12748	},
{ 	66.3996	, 	3.18967	, 	3.12775	},
{ 	66.4996	, 	3.18931	, 	3.12801	},
{ 	66.5996	, 	3.18895	, 	3.12826	},
{ 	66.6996	, 	3.18859	, 	3.12852	},
{ 	66.7996	, 	3.18823	, 	3.12878	},
{ 	66.8996	, 	3.18788	, 	3.12904	},
{ 	66.9995	, 	3.18752	, 	3.1293	},
{ 	67.0995	, 	3.18716	, 	3.12956	},
{ 	67.1995	, 	3.18681	, 	3.12981	},
{ 	67.2995	, 	3.18645	, 	3.13007	},
{ 	67.3995	, 	3.1861	, 	3.13033	},
{ 	67.4995	, 	3.18574	, 	3.13058	},
{ 	67.5995	, 	3.18539	, 	3.13084	},
{ 	67.6995	, 	3.18503	, 	3.1311	},
{ 	67.7995	, 	3.18468	, 	3.13135	},
{ 	67.8995	, 	3.18433	, 	3.1316	},
{ 	67.9995	, 	3.18398	, 	3.13186	},
{ 	68.0995	, 	3.18363	, 	3.13211	},
{ 	68.1995	, 	3.18328	, 	3.13237	},
{ 	68.2995	, 	3.18293	, 	3.13262	},
{ 	68.3995	, 	3.18258	, 	3.13287	},
{ 	68.4995	, 	3.18223	, 	3.13312	},
{ 	68.5995	, 	3.18188	, 	3.13338	},
{ 	68.6995	, 	3.18153	, 	3.13363	},
{ 	68.7995	, 	3.18118	, 	3.13388	},
{ 	68.8995	, 	3.18083	, 	3.13413	},
{ 	68.9995	, 	3.18049	, 	3.13438	},
{ 	69.0995	, 	3.18014	, 	3.13463	},
{ 	69.1995	, 	3.1798	, 	3.13488	},
{ 	69.2995	, 	3.17945	, 	3.13513	},
{ 	69.3995	, 	3.17911	, 	3.13538	},
{ 	69.4995	, 	3.17876	, 	3.13563	},
{ 	69.5995	, 	3.17842	, 	3.13588	},
{ 	69.6995	, 	3.17807	, 	3.13613	},
{ 	69.7995	, 	3.17773	, 	3.13637	},
{ 	69.8995	, 	3.17739	, 	3.13662	},
{ 	69.9995	, 	3.17705	, 	3.13687	},
{ 	70.0995	, 	3.17671	, 	3.13711	},
{ 	70.1995	, 	3.17637	, 	3.13736	},
{ 	70.2995	, 	3.17602	, 	3.13761	},
{ 	70.3995	, 	3.17568	, 	3.13785	},
{ 	70.4995	, 	3.17535	, 	3.1381	},
{ 	70.5995	, 	3.17501	, 	3.13834	},
{ 	70.6995	, 	3.17467	, 	3.13859	},
{ 	70.7995	, 	3.17433	, 	3.13883	},
{ 	70.8995	, 	3.17399	, 	3.13907	},
{ 	70.9995	, 	3.17366	, 	3.13932	},
{ 	71.0995	, 	3.17332	, 	3.13956	},
{ 	71.1995	, 	3.17298	, 	3.1398	},
{ 	71.2995	, 	3.17265	, 	3.14005	},
{ 	71.3995	, 	3.17231	, 	3.14029	},
{ 	71.4995	, 	3.17198	, 	3.14053	},
{ 	71.5995	, 	3.17164	, 	3.14077	},
{ 	71.6995	, 	3.17131	, 	3.14101	},
{ 	71.7995	, 	3.17098	, 	3.14125	},
{ 	71.8995	, 	3.17064	, 	3.14149	},
{ 	71.9995	, 	3.17031	, 	3.14173	},
{ 	72.0995	, 	3.16998	, 	3.14197	},
{ 	72.1995	, 	3.16965	, 	3.14221	},
{ 	72.2995	, 	3.16932	, 	3.14245	},
{ 	72.3995	, 	3.16899	, 	3.14269	},
{ 	72.4995	, 	3.16866	, 	3.14293	},
{ 	72.5995	, 	3.16833	, 	3.14317	},
{ 	72.6995	, 	3.168	, 	3.14341	},
{ 	72.7995	, 	3.16767	, 	3.14364	},
{ 	72.8995	, 	3.16734	, 	3.14388	},
{ 	72.9995	, 	3.16701	, 	3.14412	},
{ 	73.0995	, 	3.16669	, 	3.14435	},
{ 	73.1995	, 	3.16636	, 	3.14459	},
{ 	73.2995	, 	3.16603	, 	3.14483	},
{ 	73.3995	, 	3.16571	, 	3.14506	},
{ 	73.4995	, 	3.16538	, 	3.1453	},
{ 	73.5994	, 	3.16506	, 	3.14553	},
{ 	73.6994	, 	3.16473	, 	3.14577	},
{ 	73.7994	, 	3.16441	, 	3.146	},
{ 	73.8994	, 	3.16409	, 	3.14623	},
{ 	73.9994	, 	3.16376	, 	3.14647	},
{ 	74.0994	, 	3.16344	, 	3.1467	},
{ 	74.1994	, 	3.16312	, 	3.14693	},
{ 	74.2994	, 	3.16279	, 	3.14717	},
{ 	74.3994	, 	3.16247	, 	3.1474	},
{ 	74.4994	, 	3.16215	, 	3.14763	},
{ 	74.5994	, 	3.16183	, 	3.14786	},
{ 	74.6994	, 	3.16151	, 	3.1481	},
{ 	74.7994	, 	3.16119	, 	3.14833	},
{ 	74.8994	, 	3.16087	, 	3.14856	},
{ 	74.9994	, 	3.16055	, 	3.14879	},
{ 	75.0994	, 	3.16023	, 	3.14902	},
{ 	75.1994	, 	3.15992	, 	3.14925	},
{ 	75.2994	, 	3.1596	, 	3.14948	},
{ 	75.3994	, 	3.15928	, 	3.14971	},
{ 	75.4994	, 	3.15896	, 	3.14994	},
{ 	75.5994	, 	3.15865	, 	3.15016	},
{ 	75.6994	, 	3.15833	, 	3.15039	},
{ 	75.7994	, 	3.15802	, 	3.15062	},
{ 	75.8994	, 	3.1577	, 	3.15085	},
{ 	75.9994	, 	3.15739	, 	3.15108	},
{ 	76.0994	, 	3.15707	, 	3.1513	},
{ 	76.1994	, 	3.15676	, 	3.15153	},
{ 	76.2994	, 	3.15644	, 	3.15176	},
{ 	76.3994	, 	3.15613	, 	3.15198	},
{ 	76.4994	, 	3.15582	, 	3.15221	},
{ 	76.5994	, 	3.15551	, 	3.15243	},
{ 	76.6994	, 	3.15519	, 	3.15266	},
{ 	76.7994	, 	3.15488	, 	3.15288	},
{ 	76.8994	, 	3.15457	, 	3.15311	},
{ 	76.9994	, 	3.15426	, 	3.15333	},
{ 	77.0994	, 	3.15395	, 	3.15356	},
{ 	77.1994	, 	3.15364	, 	3.15378	},
{ 	77.2994	, 	3.15333	, 	3.15401	},
{ 	77.3994	, 	3.15302	, 	3.15423	},
{ 	77.4994	, 	3.15271	, 	3.15445	},
{ 	77.5994	, 	3.15241	, 	3.15468	},
{ 	77.6994	, 	3.1521	, 	3.1549	},
{ 	77.7994	, 	3.15179	, 	3.15512	},
{ 	77.8994	, 	3.15148	, 	3.15534	},
{ 	77.9994	, 	3.15118	, 	3.15556	},
{ 	78.0994	, 	3.15087	, 	3.15578	},
{ 	78.1994	, 	3.15056	, 	3.15601	},
{ 	78.2994	, 	3.15026	, 	3.15623	},
{ 	78.3994	, 	3.14995	, 	3.15645	},
{ 	78.4994	, 	3.14965	, 	3.15667	},
{ 	78.5994	, 	3.14934	, 	3.15689	},
{ 	78.6994	, 	3.14904	, 	3.15711	},
{ 	78.7994	, 	3.14874	, 	3.15733	},
{ 	78.8994	, 	3.14843	, 	3.15755	},
{ 	78.9994	, 	3.14813	, 	3.15776	},
{ 	79.0994	, 	3.14783	, 	3.15798	},
{ 	79.1994	, 	3.14753	, 	3.1582	},
{ 	79.2994	, 	3.14722	, 	3.15842	},
{ 	79.3994	, 	3.14692	, 	3.15864	},
{ 	79.4994	, 	3.14662	, 	3.15885	},
{ 	79.5994	, 	3.14632	, 	3.15907	},
{ 	79.6994	, 	3.14602	, 	3.15929	},
{ 	79.7994	, 	3.14572	, 	3.15951	},
{ 	79.8994	, 	3.14542	, 	3.15972	},
{ 	79.9994	, 	3.14512	, 	3.15994	},
{ 	80.0993	, 	3.14482	, 	3.16015	},
{ 	80.1993	, 	3.14453	, 	3.16037	},
{ 	80.2993	, 	3.14423	, 	3.16058	},
{ 	80.3993	, 	3.14393	, 	3.1608	},
{ 	80.4993	, 	3.14363	, 	3.16101	},
{ 	80.5993	, 	3.14334	, 	3.16123	},
{ 	80.6993	, 	3.14338	, 	3.16132	},
{ 	80.7993	, 	3.1436	, 	3.16134	},
{ 	80.8993	, 	3.14383	, 	3.16137	},
{ 	80.9993	, 	3.14405	, 	3.16139	},
{ 	81.0993	, 	3.14428	, 	3.16142	},
{ 	81.1993	, 	3.14451	, 	3.16144	},
{ 	81.2993	, 	3.14473	, 	3.16147	},
{ 	81.3993	, 	3.14495	, 	3.16149	},
{ 	81.4993	, 	3.14518	, 	3.16151	},
{ 	81.5993	, 	3.1454	, 	3.16154	},
{ 	81.6993	, 	3.14563	, 	3.16156	},
{ 	81.7993	, 	3.14585	, 	3.16159	},
{ 	81.8993	, 	3.14607	, 	3.16161	},
{ 	81.9993	, 	3.1463	, 	3.16163	},
{ 	82.0993	, 	3.14652	, 	3.16166	},
{ 	82.1993	, 	3.14674	, 	3.16168	},
{ 	82.2993	, 	3.14696	, 	3.1617	},
{ 	82.3993	, 	3.14719	, 	3.16173	},
{ 	82.4993	, 	3.14741	, 	3.16175	},
{ 	82.5993	, 	3.14763	, 	3.16178	},
{ 	82.6993	, 	3.14785	, 	3.1618	},
{ 	82.7993	, 	3.14807	, 	3.16182	},
{ 	82.8993	, 	3.14829	, 	3.16185	},
{ 	82.9993	, 	3.14851	, 	3.16187	},
{ 	83.0993	, 	3.14873	, 	3.16189	},
{ 	83.1993	, 	3.14895	, 	3.16192	},
{ 	83.2993	, 	3.14917	, 	3.16194	},
{ 	83.3993	, 	3.14939	, 	3.16196	},
{ 	83.4993	, 	3.14961	, 	3.16199	},
{ 	83.5993	, 	3.14983	, 	3.16201	},
{ 	83.6993	, 	3.15005	, 	3.16203	},
{ 	83.7993	, 	3.15026	, 	3.16206	},
{ 	83.8993	, 	3.15048	, 	3.16208	},
{ 	83.9993	, 	3.1507	, 	3.1621	},
{ 	84.0993	, 	3.15092	, 	3.16213	},
{ 	84.1993	, 	3.15113	, 	3.16215	},
{ 	84.2993	, 	3.15135	, 	3.16217	},
{ 	84.3993	, 	3.15157	, 	3.1622	},
{ 	84.4993	, 	3.15178	, 	3.16222	},
{ 	84.5993	, 	3.152	, 	3.16224	},
{ 	84.6993	, 	3.15222	, 	3.16227	},
{ 	84.7993	, 	3.15243	, 	3.16229	},
{ 	84.8993	, 	3.15265	, 	3.16231	},
{ 	84.9993	, 	3.15286	, 	3.16234	},
{ 	85.0993	, 	3.15308	, 	3.16236	},
{ 	85.1993	, 	3.15329	, 	3.16238	},
{ 	85.2993	, 	3.1535	, 	3.16241	},
{ 	85.3993	, 	3.15372	, 	3.16243	},
{ 	85.4993	, 	3.15393	, 	3.16245	},
{ 	85.5993	, 	3.15415	, 	3.16247	},
{ 	85.6993	, 	3.15436	, 	3.1625	},
{ 	85.7993	, 	3.15457	, 	3.16252	},
{ 	85.8993	, 	3.15479	, 	3.16254	},
{ 	85.9993	, 	3.155	, 	3.16256	},
{ 	86.0993	, 	3.15521	, 	3.16259	},
{ 	86.1993	, 	3.15542	, 	3.16261	},
{ 	86.2993	, 	3.15563	, 	3.16263	},
{ 	86.3993	, 	3.15585	, 	3.16266	},
{ 	86.4993	, 	3.15606	, 	3.16268	},
{ 	86.5993	, 	3.15627	, 	3.1627	},
{ 	86.6992	, 	3.15648	, 	3.16272	},
{ 	86.7992	, 	3.15669	, 	3.16275	},
{ 	86.8992	, 	3.1569	, 	3.16277	},
{ 	86.9992	, 	3.15711	, 	3.16279	},
{ 	87.0992	, 	3.15732	, 	3.16281	},
{ 	87.1992	, 	3.15753	, 	3.16284	},
{ 	87.2992	, 	3.15774	, 	3.16286	},
{ 	87.3992	, 	3.15795	, 	3.16288	},
{ 	87.4992	, 	3.15816	, 	3.1629	},
{ 	87.5992	, 	3.15837	, 	3.16293	},
{ 	87.6992	, 	3.15857	, 	3.16295	},
{ 	87.7992	, 	3.15878	, 	3.16297	},
{ 	87.8992	, 	3.15899	, 	3.16299	},
{ 	87.9992	, 	3.1592	, 	3.16301	},
{ 	88.0992	, 	3.15941	, 	3.16304	},
{ 	88.1992	, 	3.15961	, 	3.16306	},
{ 	88.2992	, 	3.15982	, 	3.16308	},
{ 	88.3992	, 	3.16003	, 	3.1631	},
{ 	88.4992	, 	3.16023	, 	3.16313	},
{ 	88.5992	, 	3.16044	, 	3.16315	},
{ 	88.6992	, 	3.16065	, 	3.16317	},
{ 	88.7992	, 	3.16085	, 	3.16319	},
{ 	88.8992	, 	3.16106	, 	3.16321	},
{ 	88.9992	, 	3.16126	, 	3.16324	},
{ 	89.0992	, 	3.16147	, 	3.16326	},
{ 	89.1992	, 	3.16167	, 	3.16328	},
{ 	89.2992	, 	3.16188	, 	3.1633	},
{ 	89.3992	, 	3.16208	, 	3.16332	},
{ 	89.4992	, 	3.16229	, 	3.16335	},
{ 	89.5992	, 	3.16249	, 	3.16337	},
{ 	89.6992	, 	3.16269	, 	3.16339	},
{ 	89.7992	, 	3.1629	, 	3.16341	},
{ 	89.8992	, 	3.1631	, 	3.16343	},
{ 	89.9992	, 	3.1633	, 	3.16345	},
{ 	90.0992	, 	3.16351	, 	3.16348	},
{ 	90.1992	, 	3.16371	, 	3.1635	},
{ 	90.2992	, 	3.16391	, 	3.16352	},
{ 	90.3992	, 	3.16411	, 	3.16354	},
{ 	90.4992	, 	3.16432	, 	3.16356	},
{ 	90.5992	, 	3.16452	, 	3.16358	},
{ 	90.6992	, 	3.16472	, 	3.16361	},
{ 	90.7992	, 	3.16492	, 	3.16363	},
{ 	90.8992	, 	3.16512	, 	3.16365	},
{ 	90.9992	, 	3.16532	, 	3.16367	},
{ 	91.0992	, 	3.16552	, 	3.16369	},
{ 	91.1992	, 	3.16572	, 	3.16371	},
{ 	91.2992	, 	3.16592	, 	3.16373	},
{ 	91.3992	, 	3.16612	, 	3.16376	},
{ 	91.4992	, 	3.16632	, 	3.16378	},
{ 	91.5992	, 	3.16652	, 	3.1638	},
{ 	91.6992	, 	3.16672	, 	3.16382	},
{ 	91.7992	, 	3.16692	, 	3.16384	},
{ 	91.8992	, 	3.16712	, 	3.16386	},
{ 	91.9992	, 	3.16732	, 	3.16388	},
{ 	92.0992	, 	3.16752	, 	3.16391	},
{ 	92.1992	, 	3.16772	, 	3.16393	},
{ 	92.2992	, 	3.16791	, 	3.16395	},
{ 	92.3992	, 	3.16811	, 	3.16397	},
{ 	92.4992	, 	3.16831	, 	3.16399	},
{ 	92.5992	, 	3.16851	, 	3.16401	},
{ 	92.6992	, 	3.1687	, 	3.16403	},
{ 	92.7992	, 	3.1689	, 	3.16405	},
{ 	92.8992	, 	3.1691	, 	3.16407	},
{ 	92.9992	, 	3.16929	, 	3.1641	},
{ 	93.0992	, 	3.16949	, 	3.16412	},
{ 	93.1992	, 	3.16969	, 	3.16414	},
{ 	93.2991	, 	3.16988	, 	3.16416	},
{ 	93.3991	, 	3.17008	, 	3.16418	},
{ 	93.4991	, 	3.17027	, 	3.1642	},
{ 	93.5991	, 	3.17047	, 	3.16422	},
{ 	93.6991	, 	3.17066	, 	3.16424	},
{ 	93.7991	, 	3.17086	, 	3.16426	},
{ 	93.8991	, 	3.17105	, 	3.16428	},
{ 	93.9991	, 	3.17125	, 	3.16431	},
{ 	94.0991	, 	3.17144	, 	3.16433	},
{ 	94.1991	, 	3.17164	, 	3.16435	},
{ 	94.2991	, 	3.17183	, 	3.16437	},
{ 	94.3991	, 	3.17202	, 	3.16439	},
{ 	94.4991	, 	3.17222	, 	3.16441	},
{ 	94.5991	, 	3.17241	, 	3.16443	},
{ 	94.6991	, 	3.1726	, 	3.16445	},
{ 	94.7991	, 	3.1728	, 	3.16447	},
{ 	94.8991	, 	3.17299	, 	3.16449	},
{ 	94.9991	, 	3.17318	, 	3.16451	},
{ 	95.0991	, 	3.17337	, 	3.16453	},
{ 	95.1991	, 	3.17357	, 	3.16455	},
{ 	95.2991	, 	3.17376	, 	3.16457	},
{ 	95.3991	, 	3.17395	, 	3.16459	},
{ 	95.4991	, 	3.17414	, 	3.16461	},
{ 	95.5991	, 	3.17433	, 	3.16464	},
{ 	95.6991	, 	3.17452	, 	3.16466	},
{ 	95.7991	, 	3.17471	, 	3.16468	},
{ 	95.8991	, 	3.1749	, 	3.1647	},
{ 	95.9991	, 	3.17509	, 	3.16472	},
{ 	96.0991	, 	3.17528	, 	3.16474	},
{ 	96.1991	, 	3.17547	, 	3.16476	},
{ 	96.2991	, 	3.17566	, 	3.16478	},
{ 	96.3991	, 	3.17585	, 	3.1648	},
{ 	96.4991	, 	3.17604	, 	3.16482	},
{ 	96.5991	, 	3.17623	, 	3.16484	},
{ 	96.6991	, 	3.17642	, 	3.16486	},
{ 	96.7991	, 	3.17661	, 	3.16488	},
{ 	96.8991	, 	3.1768	, 	3.1649	},
{ 	96.9991	, 	3.17699	, 	3.16492	},
{ 	97.0991	, 	3.17718	, 	3.16494	},
{ 	97.1991	, 	3.17736	, 	3.16496	},
{ 	97.2991	, 	3.17755	, 	3.16498	},
{ 	97.3991	, 	3.17774	, 	3.165	},
{ 	97.4991	, 	3.17793	, 	3.16502	},
{ 	97.5991	, 	3.17811	, 	3.16504	},
{ 	97.6991	, 	3.1783	, 	3.16506	},
{ 	97.7991	, 	3.17849	, 	3.16508	},
{ 	97.8991	, 	3.17868	, 	3.1651	},
{ 	97.9991	, 	3.17886	, 	3.16512	},
{ 	98.0991	, 	3.17905	, 	3.16514	},
{ 	98.1991	, 	3.17923	, 	3.16516	},
{ 	98.2991	, 	3.17942	, 	3.16518	},
{ 	98.3991	, 	3.17961	, 	3.1652	},
{ 	98.4991	, 	3.17979	, 	3.16522	},
{ 	98.5991	, 	3.17998	, 	3.16524	},
{ 	98.6991	, 	3.18016	, 	3.16526	},
{ 	98.7991	, 	3.18035	, 	3.16528	},
{ 	98.8991	, 	3.18053	, 	3.1653	},
{ 	98.9991	, 	3.18072	, 	3.16532	},
{ 	99.0991	, 	3.1809	, 	3.16534	},
{ 	99.1991	, 	3.18109	, 	3.16536	},
{ 	99.2991	, 	3.18127	, 	3.16538	},
{ 	99.3991	, 	3.18145	, 	3.1654	},
{ 	99.4991	, 	3.18164	, 	3.16542	},
{ 	99.5991	, 	3.18182	, 	3.16544	},
{ 	99.6991	, 	3.182	, 	3.16546	},
{ 	99.799	, 	3.18219	, 	3.16548	},
{ 	99.899	, 	3.18237	, 	3.1655	},
{ 	99.999	, 	3.18255	, 	3.16552	},
{ 	100.099	, 	3.18274	, 	3.16553	},
#endif
};

void SetSpecularPower(NV2AState* d)
{
	PGRAPHState* pg = &d->pgraph;
	//NV097_SET_SPECULAR_PARAMS(0) 
	float L = pg->KelvinPrimitive.SetSpecularParams[0];
	float M = pg->KelvinPrimitive.SetSpecularParams[1];
	float N = pg->KelvinPrimitive.SetSpecularParams[2];
	float L1 = pg->KelvinPrimitive.SetSpecularParams[3];
	float M1 = pg->KelvinPrimitive.SetSpecularParams[4];
	float N1 = pg->KelvinPrimitive.SetSpecularParams[5];
	//NV2AMaterials[0].Power = N1 * 2;
	if (N1 <= 0 || N <= 0) {
		NV2A_SceneMateirals[0].Power = 0;
	}
	else if (N1 > 2.8006f) {
		NV2A_SceneMateirals[0].Power = 20.0f;
	}
	else if (N1 <= 0.299999f) {
		NV2A_SceneMateirals[0].Power = N1 * 2;
	}
	else if (N1 <= 0.925521f) {
		//index 19, if N1<=0.925521, use N1 as key to look up Power.// else if N>=1.63902f, use N as key to look up Power.
		NV2A_SceneMateirals[0].Power = 0.2125f * N1 * N1 + 1.8075f * N1 + 0.0417f;
	}else{
		for (int i = 20; i <= 242; i++) {
			if (g_ReverseExpluteTable[i].N > N) {
				NV2A_SceneMateirals[0].Power = (g_ReverseExpluteTable[i].N + g_ReverseExpluteTable[i - 1].N) / 2.0f;
				break;
			}
		}
	}
	//skip back back material specular power setting when D3DRS_TWOSIDELIGHTING is not enabled.
	if (pg->KelvinPrimitive.SetTwoSidedLightEn == 0)
		return;
	// NV097_SET_BACK_SPECULAR_PARAMS(0)
	L = pg->KelvinPrimitive.SetBackSpecularParams[0];
	M = pg->KelvinPrimitive.SetBackSpecularParams[1];
	N = pg->KelvinPrimitive.SetBackSpecularParams[2];
	L1 = pg->KelvinPrimitive.SetBackSpecularParams[3];
	M1 = pg->KelvinPrimitive.SetBackSpecularParams[4];
	N1 = pg->KelvinPrimitive.SetBackSpecularParams[5];
	//NV2AMaterials[0].Power = N1 * 2;
	if (N1 <= 0 || N <= 0) {
		NV2A_SceneMateirals[1].Power = 0;
	}
	else if (N1 > 2.8006f) {
		NV2A_SceneMateirals[1].Power = 20.0f;
	}
	else if (N1 <= 0.299999f) {
		NV2A_SceneMateirals[1].Power = N1 * 2;
	}
	else if (N1 <= 0.925521f) {
		//index 19, if N1<=0.925521, use N1 as key to look up Power.
		NV2A_SceneMateirals[1].Power = 0.2125f * N1 * N1 + 1.8075f * N1 + 0.0417f;
	}
	else {
		// else if N>=1.63902f, use N as key to look up Power.
		for (int i = 20; i <= 242; i++) {
			if (g_ReverseExpluteTable[i].N > N) {
				NV2A_SceneMateirals[1].Power = (g_ReverseExpluteTable[i].N + g_ReverseExpluteTable[i - 1].N) / 2.0f;
				break;
			}
		}
	}
}


void XformBy4x3(D3DVECTOR * res, CONST D3DVECTOR * v, FLOAT w, CONST D3DMATRIX * m)
{				
	res->x = v->x*m->_11 + v->y*m->_21 + v->z*m->_31 + w*m->_41;
	res->y = v->x*m->_12 + v->y*m->_22 + v->z*m->_32 + w*m->_42;
	res->z = v->x*m->_13 + v->y*m->_23 + v->z*m->_33 + w*m->_43;
				
}
// Fixme!!! we need D3D__RenderState[D3DRS_AMBIENT] and materials set with SetMaterial()/SetBackMaterial() to reverse the light colors.
void CxbxrImpl_LazySetLights(NV2AState* d)
{
	PGRAPHState* pg = &d->pgraph;
	HRESULT hRet;
	NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_LIGHTING, pg->KelvinPrimitive.SetLightingEnable);
	NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_TWOSIDEDLIGHTING, pg->KelvinPrimitive.SetTwoSidedLightEn);
	//pg->KelvinPrimitive.SetLight[8].{AmbientColor[3],DiffuseColor[3],SpecularColor[3],LocalRange,InfiniteHalfVector[3],InfiniteDirection[3],SpotFalloff[3],SpotDirection[4],LocalPosition[3],LocalAttenuation[3],Rev_1074[3]
	DWORD control = 0;
	DWORD colorMaterial = 0;
	DWORD lightEnableMask = 0;
	bool bLightintEnable = pg->KelvinPrimitive.SetLightingEnable != false;
	//xbox sets D3DRS_LightEnable to false when vertex shader is not fixed mode.
	if (g_Xbox_VertexShaderMode != VertexShaderMode::FixedFunction)bLightintEnable = true;
	//NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_LIGHTING, bLightintEnable);
	if (bLightintEnable) {
		control = pg->KelvinPrimitive.SetLightControl;
		if (pg->KelvinPrimitive.SetSpecularEnable != false) {
			NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_LOCALVIEWER, (control& NV097_SET_LIGHT_CONTROL_LOCALEYE_TRUE)!=0? true:false);
		}
		colorMaterial = pg->KelvinPrimitive.SetColorMaterial;//NV097_SET_COLOR_MATERIAL, colorMaterial  // 0x298
		NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_COLORVERTEX, colorMaterial!=0?true:false);
		// retrive material source render state.
		for (int i = 0; i < 8; i++) {
			NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_BACKSPECULARMATERIALSOURCE+i, (colorMaterial>>(2*(7-i)))&0x03);
		}
		// upate scene material setting
		SetSceneAmbientAndMaterialEmission(d);
		// check X_D3DRS_SPECULARENABLE, only update scene material power if it's enabled.
		if (pg->KelvinPrimitive.SetSpecularEnable != 0) {
			SetSpecularPower(d);
		}
		else {
			NV2A_SceneMateirals[0].Power = 0.0f;
			NV2A_SceneMateirals[1].Power = 0.0f;
		}
		extern xbox::hresult_xt WINAPI CxbxrImpl_SetMaterial(CONST xbox::X_D3DMATERIAL8 * pMaterial);
		//CxbxrImpl_SetMaterial(&NV2A_SceneMateirals[0]);
		//calls host api directly since in CxbxrImpl_SetMaterial() it updates the fixed mode vertex shader states directly which we have to distinguish from xbox api.
		HRESULT hRet = g_pD3DDevice->SetMaterial(&NV2A_SceneMateirals[0]);
		lightEnableMask = pg->KelvinPrimitive.SetLightEnableMask;//Push1(pPush, NV097_SET_LIGHT_ENABLE_MASK, enableMask);      // 0x3bc;
		//set lights
		int lightNum;
		D3DVECTOR pos;
		D3DVECTOR dir;
		D3DVECTOR hv, EyeDirection;
		D3DVECTOR NV2ALightdir, tmpDir, NV2ALightPosition, tmpPosition;
		EyeDirection = { 0.0,0.0,-1.0 };
		for (lightNum = 0; lightNum < 8; lightNum++) {
			bool bEnable;
			DWORD lightType = (lightEnableMask >> (lightNum << 1)) & 0x03;
			//default:
			NV2A_Light8[lightNum].Type = (D3DLIGHTTYPE)0;
			bEnable = false;

			switch (lightType) {

			case NV097_SET_LIGHT_ENABLE_MASK_LIGHT0_OFF: //light disable
				hRet = g_pD3DDevice->LightEnable(lightNum, false);
				continue;
				break;
				
			case NV097_SET_LIGHT_ENABLE_MASK_LIGHT0_INFINITE://D3DLIGHT_DIRECTIONAL
				NV2A_Light8[lightNum].Type= D3DLIGHT_DIRECTIONAL;
				NV2A_Light8[lightNum].Range = pg->KelvinPrimitive.SetLight[lightNum].LocalRange;
				NV2ALightdir.x = pg->KelvinPrimitive.SetLight[lightNum].InfiniteDirection[0];
				NV2ALightdir.y = pg->KelvinPrimitive.SetLight[lightNum].InfiniteDirection[1];
				NV2ALightdir.z = pg->KelvinPrimitive.SetLight[lightNum].InfiniteDirection[2];
				
				XformBy4x3(&tmpDir, &NV2ALightdir,0.0f, &g_xbox_DirectModelView_InverseView);

				NV2A_Light8[lightNum].Direction.x =-tmpDir.x;
				NV2A_Light8[lightNum].Direction.y =-tmpDir.y;
				NV2A_Light8[lightNum].Direction.z =-tmpDir.z;
				bEnable = true;
				break;
			case NV097_SET_LIGHT_ENABLE_MASK_LIGHT0_LOCAL: //D3DLIGHT_POINT
				NV2A_Light8[lightNum].Type = D3DLIGHT_POINT;
				//Push1f(pPush,NV097_SET_LIGHT_LOCAL_RANGE(lightNum),	pLight->Light8.Range);
				NV2A_Light8[lightNum].Range = pg->KelvinPrimitive.SetLight[lightNum].LocalRange;
				NV2ALightPosition.x = pg->KelvinPrimitive.SetLight[lightNum].LocalPosition[0];
				NV2ALightPosition.y = pg->KelvinPrimitive.SetLight[lightNum].LocalPosition[1];
				NV2ALightPosition.z = pg->KelvinPrimitive.SetLight[lightNum].LocalPosition[2];

				XformBy4x3(&tmpPosition, &NV2ALightPosition, 1.0f, &g_xbox_DirectModelView_InverseView);

				NV2A_Light8[lightNum].Position.x = tmpPosition.x;
				NV2A_Light8[lightNum].Position.y = tmpPosition.y;
				NV2A_Light8[lightNum].Position.z = tmpPosition.z;
				NV2A_Light8[lightNum].Attenuation0 = pg->KelvinPrimitive.SetLight[lightNum].LocalAttenuation[0];
				NV2A_Light8[lightNum].Attenuation1 = pg->KelvinPrimitive.SetLight[lightNum].LocalAttenuation[1];
				NV2A_Light8[lightNum].Attenuation2 = pg->KelvinPrimitive.SetLight[lightNum].LocalAttenuation[2];
				bEnable = true;
				break;
			case NV097_SET_LIGHT_ENABLE_MASK_LIGHT0_SPOT: //D3DLIGHT_SPOT
				NV2A_Light8[lightNum].Type = D3DLIGHT_SPOT;
				NV2A_Light8[lightNum].Range = pg->KelvinPrimitive.SetLight[lightNum].LocalRange;
				NV2ALightPosition.x = pg->KelvinPrimitive.SetLight[lightNum].LocalPosition[0];
				NV2ALightPosition.y = pg->KelvinPrimitive.SetLight[lightNum].LocalPosition[1];
				NV2ALightPosition.z = pg->KelvinPrimitive.SetLight[lightNum].LocalPosition[2];

				XformBy4x3(&tmpPosition, &NV2ALightPosition, 1.0f, &g_xbox_DirectModelView_InverseView);

				NV2A_Light8[lightNum].Position.x = tmpPosition.x;
				NV2A_Light8[lightNum].Position.y = tmpPosition.y;
				NV2A_Light8[lightNum].Position.z = tmpPosition.z;
				NV2A_Light8[lightNum].Attenuation0 = pg->KelvinPrimitive.SetLight[lightNum].LocalAttenuation[0];
				NV2A_Light8[lightNum].Attenuation1 = pg->KelvinPrimitive.SetLight[lightNum].LocalAttenuation[1];
				NV2A_Light8[lightNum].Attenuation2 = pg->KelvinPrimitive.SetLight[lightNum].LocalAttenuation[2];
				float Falloff_L = pg->KelvinPrimitive.SetLight[lightNum].SpotFalloff[0]; //todo: Falloff_L/Falloff_M/Falloff_N there should be 3 components but not found in Light8/Light9
				float Falloff_M = pg->KelvinPrimitive.SetLight[lightNum].SpotFalloff[1];
				float Falloff_N = pg->KelvinPrimitive.SetLight[lightNum].SpotFalloff[2];
				NV2ALightdir.x = pg->KelvinPrimitive.SetLight[lightNum].SpotDirection[0];
				NV2ALightdir.y = pg->KelvinPrimitive.SetLight[lightNum].SpotDirection[1];
				NV2ALightdir.z = pg->KelvinPrimitive.SetLight[lightNum].SpotDirection[2];

				XformBy4x3(&tmpDir, &NV2ALightdir, 0.0f, &g_xbox_DirectModelView_InverseView);

				NV2A_Light8[lightNum].Direction.x = -tmpDir.x;
				NV2A_Light8[lightNum].Direction.y = -tmpDir.y;
				NV2A_Light8[lightNum].Direction.z = -tmpDir.z;
				float W = pg->KelvinPrimitive.SetLight[lightNum].SpotDirection[3]; // there is no W member of Light8
				// Fixme!!! there is no eazy way to reverse Falloff from Falloff_L/Falloff_M, so I put the default value 1.0f here.
				NV2A_Light8[lightNum].Falloff = 1.0f;
				D3DVECTOR dirNormal = NV2A_Light8[lightNum].Direction;
				NormalizeVector3(&dirNormal);
				float Scale = NV2A_Light8[lightNum].Direction.x / dirNormal.x; // there is no Scale member of Light8
				float phi2 = -1.0*W / Scale;
				float theta2 = phi2 + 1.0 / Scale;
				NV2A_Light8[lightNum].Theta = 2.0f*acos(theta2);
				NV2A_Light8[lightNum].Phi = 2.0f*acos(phi2);
				bEnable = true;
				break;
			}
			// Fixme!!! we need D3D__RenderState[D3DRS_AMBIENT] and materials set with SetMaterial()/SetBackMaterial() to reverse the light colors.
			// hack, we use colors in kelvin directly since these colors are composed with D3DRS_AMBIENT/SetMaterial/SetBackMaterial/ and the color of each light.
			// hack, use a constant color scale to scale up light color.
			float colorScale = 2.1;
			NV2A_Light8[lightNum].Ambient.r = colorScale * pg->KelvinPrimitive.SetLight[lightNum].AmbientColor[0];
			NV2A_Light8[lightNum].Ambient.g = colorScale * pg->KelvinPrimitive.SetLight[lightNum].AmbientColor[1];
			NV2A_Light8[lightNum].Ambient.b = colorScale * pg->KelvinPrimitive.SetLight[lightNum].AmbientColor[2];
			NV2A_Light8[lightNum].Ambient.a = 1.0f;
			NV2A_Light8[lightNum].Specular.r = colorScale * pg->KelvinPrimitive.SetLight[lightNum].SpecularColor[0];
			NV2A_Light8[lightNum].Specular.g = colorScale * pg->KelvinPrimitive.SetLight[lightNum].SpecularColor[1];
			NV2A_Light8[lightNum].Specular.b = colorScale * pg->KelvinPrimitive.SetLight[lightNum].SpecularColor[2];
			NV2A_Light8[lightNum].Specular.a= 1.0f;
			NV2A_Light8[lightNum].Diffuse.r = colorScale * pg->KelvinPrimitive.SetLight[lightNum].DiffuseColor[0];
			NV2A_Light8[lightNum].Diffuse.g = colorScale * pg->KelvinPrimitive.SetLight[lightNum].DiffuseColor[1];
			NV2A_Light8[lightNum].Diffuse.b = colorScale * pg->KelvinPrimitive.SetLight[lightNum].DiffuseColor[2];
			NV2A_Light8[lightNum].Diffuse.a = 1.0f;
			// here we calls Host D3D for SetLight() and LightEnable(). because originaly there two are called directly from HLE patch.
			HRESULT hRet = g_pD3DDevice->SetLight(lightNum, &NV2A_Light8[lightNum]);
			//DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetLight");
			hRet = g_pD3DDevice->LightEnable(lightNum, bEnable);
		}
	}
    else {
		;
	}
	return;

}

void D3D_draw_state_update(NV2AState* d)
{
	PGRAPHState* pg = &d->pgraph;
	HRESULT hRet;

	// update transfoms
	// update transform matrix using NV2A KevlvinPrimitive contents if we're in direct ModelView transform mode.
	if (NV2A_DirtyFlags & X_D3DDIRTYFLAG_TRANSFORM) {
		CxbxrImpl_LazySetTransform(d);
		NV2A_DirtyFlags &= ~X_D3DDIRTYFLAG_TRANSFORM;
	}

	// update point params, Xbox takes everything from texture stage 3
	// hack since we're using all pgraph processing now.

	if ((NV2A_DirtyFlags & X_D3DDIRTYFLAG_POINTPARAMS) != 0) {
		CxbxrImpl_LazySetPointParameters(d);
		// clear dirty flag
		NV2A_DirtyFlags &= ~X_D3DDIRTYFLAG_POINTPARAMS;
	}

	// update texture stage texture states, texture stage texture states must be update prior to pixel shader because pixel shader compilation depends on texture state input
	if ((NV2A_DirtyFlags & X_D3DDIRTYFLAG_TEXTURE_STATE) != 0) {
		CxbxrImpl_LazySetTextureState(d);
		// clear dirty flag
		NV2A_DirtyFlags &= ~X_D3DDIRTYFLAG_TEXTURE_STATE;
	}
	// xbox only update texture transfoms if we're in fixed mode vertex shader
	// but we also need to update the texgen in texCoordIndex for program shader, so we always update the variables.
	//if (g_Xbox_VertexShaderMode == VertexShaderMode::FixedFunction){
	if (NV2A_DirtyFlags & X_D3DDIRTYFLAG_TEXTURE_TRANSFORM) {
		CxbxrImpl_LazySetTextureTransform(d);
		NV2A_DirtyFlags &= ~X_D3DDIRTYFLAG_TEXTURE_TRANSFORM;
	}
	//}
	// update pixel shader when either X_D3DDIRTYFLAG_SHADER_STAGE_PROGRAM| X_D3DDIRTYFLAG_COMBINERS| X_D3DDIRTYFLAG_SPECFOG_COMBINER flag was set
	if ((NV2A_DirtyFlags & (X_D3DDIRTYFLAG_SHADER_STAGE_PROGRAM| X_D3DDIRTYFLAG_COMBINERS| X_D3DDIRTYFLAG_SPECFOG_COMBINER)) != 0) {

		// update combiners, combiners must be update prior to pixel shader, because we have to compose colorOp before we compose fix funtion pixel shaders.
		// only update combiners when in fixed pixel shader.
		if (pNV2A_PixelShader == nullptr) {
			if ((NV2A_DirtyFlags & X_D3DDIRTYFLAG_COMBINERS) != 0) {
				// only update the combiner state when we're in fixed function pixel shader
				CxbxrImpl_LazySetCombiners(d);
				// clear dirty flag
				NV2A_DirtyFlags &= ~X_D3DDIRTYFLAG_COMBINERS;
			}
		}

		// update spec fog combiner state
		if ((NV2A_DirtyFlags & X_D3DDIRTYFLAG_SPECFOG_COMBINER) != 0) {
			CxbxrImpl_LazySetSpecFogCombiner(d);
			// clear dirty flag
			NV2A_DirtyFlags &= ~X_D3DDIRTYFLAG_SPECFOG_COMBINER;
		}

		CxbxrImpl_LazySetShaderStageProgram(d);

		// clear dirty flag xbox::dword_xt
		NV2A_DirtyFlags &= ~X_D3DDIRTYFLAG_SHADER_STAGE_PROGRAM;
	}

	// update texture transfoms
	if (NV2A_DirtyFlags & X_D3DDIRTYFLAG_LIGHTS) {
		CxbxrImpl_LazySetLights(d);
		NV2A_DirtyFlags &= ~X_D3DDIRTYFLAG_LIGHTS;
	}

	// update viewport state when we're in pushbuffer replay mode.
	// if ((NV2A_stateFlags & X_STATE_RUNPUSHBUFFERWASCALLED) != 0) {
		// CxbxUpdateNativeD3DResources() calls CxbxUpdateHostViewport() to update host viewpot, we call pgraph_SetViewport(d) to convert NV2A viewport content to xbox vieport and then call CxbxrImpl_SetViewport() there.
		// only update viewport if viewport got dirty.
		if (NV2A_viewport_dirty == true) {
			// this only compose viewport from pgraph content 
			pgraph_ComposeViewport(d);
			// todo: actually setviewport with the composed viewport, currently we don't set the host viewport via pgraph content, yet. the SetViewport() is currently HLEed and not processed in pushbuffer.
			// D3DDevice_SetViewport() was patched and HLEed, here we only implement it when we're in RunPushbuffer().
			//if ((NV2A_stateFlags & X_STATE_RUNPUSHBUFFERWASCALLED) != 0) {
				xbox::X_D3DVIEWPORT8 Viewport;
				CxbxrGetViewport(Viewport);
				// no need to call SetViewport here.in CxbxUpdateNativeD3DResources(), CxbxUpdateHostViewport() will pickup our viewport.
                /*
				Viewport.X = pg->KelvinPrimitive.SetSurfaceClipHorizontal & 0xFFFF;
				Viewport.Width = (pg->KelvinPrimitive.SetSurfaceClipHorizontal >> 16) & 0xFFFF;
				Viewport.Y = pg->KelvinPrimitive.SetSurfaceClipVertical & 0xFFFF;
				Viewport.Height = (pg->KelvinPrimitive.SetSurfaceClipVertical >> 16) & 0xFFFF;
				*/
				CxbxrImpl_SetViewport(&Viewport);
			//}
			NV2A_viewport_dirty = false;
		}
	//}

	// Note, that g_Xbox_VertexShaderMode should be left untouched,
	// because except for the declaration override, the Xbox shader (either FVF
	// or a program, or even passthrough shaders) should still be in effect!


	// setup vertes format, and vertex buffer from here.
	// Derive vertex attribute layout, using an intermediate array
	struct { int slot_index; uint32_t offset; uint32_t stride; uint32_t size_and_type; } SortedAttributes[X_VSH_MAX_ATTRIBUTES];
	uint32_t inline_offset = 0; // This applies only to IVB draw. TODO : instead of nullptr, start at the memory address of an IVB-dedicated host D3D VertexBuffer
	for (int i = 0; i < X_VSH_MAX_ATTRIBUTES; i++) { // X_VSH_MAX_ATTRIBUTES == NV2A_VERTEXSHADER_ATTRIBUTES
		SortedAttributes[i].slot_index = i; // Store index of each slot, used in ordering predicate, and to allow access to original slot after sorting
		switch (pg->draw_mode)
		{
			case DrawMode::InlineArray: {
				// for draw_mode == DrawMode::InlineArray, we don't have KelvinPrimitive.SetVertexDataArrayOffset[] set, we have to compose the vertex offset and stride ourselves
				g_NV2AVertexAttributeFormat.Slots[i].StreamIndex = 0; // only above zero for multi-buffer draws (so, always zero for IVB)
				g_NV2AVertexAttributeFormat.Slots[i].Offset = inline_offset; // offset from the starting attribute inside indicated stream buffer
				g_NV2AVertexAttributeFormat.Slots[i].Format = pg->KelvinPrimitive.SetVertexDataArrayFormat[i] & (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE | NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE);
				g_NV2AVertexAttributeFormat.Slots[i].TessellationType = 0; // TODO or ignore?
				g_NV2AVertexAttributeFormat.Slots[i].TessellationSource = 0; // TODO or ignore?

				inline_offset += pg->vertex_attributes[i].count*pg->vertex_attributes[i].size;
				D3D_StreamZeroStride = inline_offset;
				break;
		    }
			case DrawMode::InlineBuffer: {
				// for draw_mode == DrawMode::InlineBuffer, we don't have KelvinPrimitive.SetVertexDataArrayFormat[] set, we have to compose the vertex format and offset ourselves
				g_NV2AVertexAttributeFormat.Slots[i].StreamIndex = 0; // only above zero for multi-buffer draws (so, always zero for IVB)
				g_NV2AVertexAttributeFormat.Slots[i].Offset = inline_offset; // offset from the starting attribute inside indicated stream buffer
				g_NV2AVertexAttributeFormat.Slots[i].TessellationType = 0; // TODO or ignore?
				g_NV2AVertexAttributeFormat.Slots[i].TessellationSource = 0; // TODO or ignore?

				if (pg->vertex_attributes[i].set_by_inline_buffer) {
					g_NV2AVertexAttributeFormat.Slots[i].Format = xbox::X_D3DVSDT_FLOAT4 ; // either 0x42 or 0x02
					inline_offset += D3D_inline_attribute_size;
				}
				else {
					g_NV2AVertexAttributeFormat.Slots[i].Format = xbox::X_D3DVSDT_NONE; // either 0x42 or 0x02
				}
				D3D_StreamZeroStride = inline_offset;
				// TODO : If we're going to allocate and use one single (16 attribute-wide) vertex buffer for IVB drawing, offsets need to take this into account
				break;
			}
			case DrawMode::DrawArrays:
			case DrawMode::InlineElements:{
				// TODO ? Reword : update update vertex buffer/stream if draw_mode == DrawMode::DrawArrays or InlineElements. KelvinPrimitive.SetVertexDataArrayOffset[] is only set/update in these two draw modes
				SortedAttributes[i].stride = pg->KelvinPrimitive.SetVertexDataArrayFormat[i] >> 8; // NV097_SET_VERTEX_DATA_ARRAY_FORMAT_STRIDE // The byte increment to step from the start of one vertex attribute to the next
				SortedAttributes[i].size_and_type = pg->KelvinPrimitive.SetVertexDataArrayFormat[i] & (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE | NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE);
				// when the attribute is reference to a none existed stream, both the stride and offset will be 0;
				// todo: shall we simplify this condition to check whether stride == 0?
				// Detect disabled slots by their format (0x02 : size count zero, type float) :
				if (SortedAttributes[i].size_and_type == xbox::X_D3DVSDT_NONE){
					SortedAttributes[i].offset = 0xFFFFFFFF; // Make sure disabled slots get sorted to the end
				}
				else {
					// TODO ? Reword : for draw_mode == DrawMode::InlineArray, only KelvinPrimitive.SetVertexDataArrayFormat[] was set/update. we have to compose the vertex offset ourselves?
					SortedAttributes[i].offset = pg->KelvinPrimitive.SetVertexDataArrayOffset[i];
				}
				if (SortedAttributes[i].stride == 0) {
					SortedAttributes[i].size_and_type = xbox::X_D3DVSDT_NONE;
					SortedAttributes[i].offset = 0xFFFFFFFF; // Make sure disabled slots get sorted to the end
				}
				//stride could be 0 when a none existed stream was referenced.
				//assert(SortedAttributes[i].stride > 0);
				assert(SortedAttributes[i].stride <= D3D_inline_vertex_stride); // TODO : replace with actual NV2A maximum stride, might be 2048?
				break;
			}
		}

	}
	// Only necessary for pg->draw_mode == DrawMode::DrawArrays || pg->draw_mode == DrawMode::InlineElements, and the overhead must be reduced.
	if (pg->draw_mode == DrawMode::DrawArrays || pg->draw_mode == DrawMode::InlineElements) {
		// Sort out intermediate attribute slot array on memory offset (and on slot, to guarantee deterministic ordering when offsets overlap) :
		/* redunt code.
		if (pg->draw_mode == DrawMode::InlineElements) {
			SortedAttributes[0].size_and_type = SortedAttributes[0].size_and_type;
		}
		*/
		std::sort(/*First=*/&SortedAttributes[0], /*Last=*/&SortedAttributes[X_VSH_MAX_ATTRIBUTES - 1], /*Pred=*/[](const auto& x, const auto& y)
			{ return std::tie(x.offset, x.slot_index) < std::tie(y.offset, y.slot_index); });

		assert(SortedAttributes[0].size_and_type != xbox::X_D3DVSDT_NONE); // After sorting, verify at least the first slot is in use (more is fine, none is bad)

		// Store stream zero's stride for possible use later on, D3D_StreamZeroStride was already set in other two modes DrawMode::InlineArray and DrawMode::InlineBuffer
		D3D_StreamZeroStride = SortedAttributes[0].stride;
	
		// Using our sorted attibutes, derive the g_NV2AVertexAttributeFormat slot values :
		DWORD current_stream_index = 0;
		NV2A_StreamCount = 0;
		uint32_t group_offset = SortedAttributes[0].offset;
		uint32_t group_stride = SortedAttributes[0].stride;
		//store the vertex buffer address of stream[current_stream_index]
		if (SortedAttributes[0].size_and_type != xbox::X_D3DVSDT_NONE) {
			((xbox::X_D3DResource*)(&NV2A_StreamSource[current_stream_index].VertexBuffer))->Data = group_offset;//(xbox::X_D3DVertexBuffer  *)
			NV2A_VertexBuffer[current_stream_index].Data= group_offset;
			NV2A_StreamSource[current_stream_index].VertexBuffer = &NV2A_VertexBuffer[current_stream_index];
			NV2A_StreamSource[current_stream_index].Stride = group_stride;
			NV2A_StreamCount++;
		}
		for (int i = 0; i < X_VSH_MAX_ATTRIBUTES; i++) { // identical to NV2A_VERTEXSHADER_ATTRIBUTES
			DWORD current_delta = 0;
			DWORD current_format = SortedAttributes[i].size_and_type; // Put this in a variable, to make the assignments to g_NV2AVertexAttributeFormat consistent
			// Is this attribute in-use?
			if (current_format != xbox::X_D3DVSDT_NONE) {
				// Does this attribute start outside the current group of attributes?
				if (SortedAttributes[i].offset >= group_offset + group_stride) {
					assert(pg->draw_mode != DrawMode::InlineBuffer); // IVB drawing should stay within one group

					// Start a new stream, that contains the next group of attributes
					current_stream_index++;
					group_offset = SortedAttributes[i].offset;
					group_stride = SortedAttributes[i].stride;
					//store the vertex buffer address of stream[current_stream_index]
					NV2A_VertexBuffer[current_stream_index].Data = group_offset;
					NV2A_StreamSource[current_stream_index].VertexBuffer = &NV2A_VertexBuffer[current_stream_index];
					NV2A_StreamSource[current_stream_index].Stride = group_stride;
					NV2A_StreamCount++;
				}
				else { // this attribute starts inside the current vertex stride. it's using the same vertex buffer.
					assert(group_offset <= SortedAttributes[i].offset); // Verify all attributes in the same group lie next to the group starting offset
					assert(group_stride == SortedAttributes[i].stride); // Verify all attributes in the same group share an identical stride
				}

				// Calculate the difference between this attribute, and its group's starting, memory offset:
				current_delta = SortedAttributes[i].offset - group_offset;
			}

			// With above determined values, populate each slot in g_NV2AVertexAttributeFormat :
			int index = SortedAttributes[i].slot_index;
			g_NV2AVertexAttributeFormat.Slots[index].StreamIndex = current_stream_index; // only above zero for multi-buffer draws (so, always zero for IVB)
			g_NV2AVertexAttributeFormat.Slots[index].Offset = current_delta; // offset from the starting attribute inside indicated stream buffer
			g_NV2AVertexAttributeFormat.Slots[index].Format = current_format;
			g_NV2AVertexAttributeFormat.Slots[index].TessellationType = 0; // TODO or ignore?
			g_NV2AVertexAttributeFormat.Slots[index].TessellationSource = 0; // TODO or ignore?
		}
	}
	//check for duplicated texcoord slots, if a texcoord slot is duplicated with other texcoord slot prior to it, set texcoord index of current slot to the prior duplicated slot and disable current slot.
	for (int currentSlot = xbox::X_D3DVSDE_TEXCOORD0 + 1, currentStage=1; currentStage <4; currentSlot++, currentStage++) {
		// skip to next loop if current stage texture is not set.
		if (g_pNV2A_SetTexture[currentStage] == nullptr)
			break;
		//only check for duplications when current slot is not unused
		//if (g_NV2AVertexAttributeFormat.Slots[current].Format!= xbox::X_D3DVSDT_NONE)
			//only check texcoord slots prior to current slot, always set texcoord index of later texture stages to prior texture stages.
		for (int priorSlot = xbox::X_D3DVSDE_TEXCOORD0, priorStage=0; priorSlot < currentSlot; priorSlot++,priorStage++) {
			// skip to next loop if prior stage texture is not set.
			if (g_pNV2A_SetTexture[priorStage] == nullptr)
				break;
			// compare stream index/slot offset to see if two slots uses the same texcoord.
			if (g_NV2AVertexAttributeFormat.Slots[currentSlot].StreamIndex == g_NV2AVertexAttributeFormat.Slots[priorSlot].StreamIndex
				&& g_NV2AVertexAttributeFormat.Slots[currentSlot].Offset == g_NV2AVertexAttributeFormat.Slots[priorSlot].Offset) {
				
				//retrive texgen from NV2ATextureStates
				DWORD texcoordIndex = NV2ATextureStates.Get(currentSlot - xbox::X_D3DVSDE_TEXCOORD0, xbox::X_D3DTSS_TEXCOORDINDEX);
				DWORD texgen=texcoordIndex & 0xFFFF0000;
				//update texcoordIndex
				texcoordIndex = texgen|((priorStage) & 0x0000FFFF);
				/* //todo: update texcoord index should be enough because the indexed prior stage warp shall be the same as the warp set in current stage in NV2A.
				DWORD address = pg->KelvinPrimitive.SetTexture[currentStage].Address;
				DWORD warp = address & (NV097_SET_TEXTURE_ADDRESS_CYLWRAP_U_TRUE | NV097_SET_TEXTURE_ADDRESS_CYLWRAP_V_TRUE | NV097_SET_TEXTURE_ADDRESS_CYLWRAP_P_TRUE);
				NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_WRAP0+ priorStage, warp);
				*/
				//store updated texcoordIndex
				NV2ATextureStates.Set(currentSlot - xbox::X_D3DVSDE_TEXCOORD0, xbox::X_D3DTSS_TEXCOORDINDEX, texcoordIndex);
				//disable current slot
				g_NV2AVertexAttributeFormat.Slots[currentSlot].Offset = 0;
				g_NV2AVertexAttributeFormat.Slots[currentSlot].Format = xbox::X_D3DVSDT_NONE;
				break;
			}
		}
	}

	// update other D3D states
	// FIXME!! CxbxUpdateNativeD3DResources() calls XboxRenderStates.Apply() and XboxTextureStates.Apply() to apply Xbox D3D states to Host.
	// so what we set to host here might be over write later. safer way is to update XboxTextureStates/XboxTextureStates and let CxbxUpdateNativeD3DResources() update them to host.
	// only update state to host when we're in pushbuffer replay mode. other wise HLE shall handle the host update task.
	//if ((NV2A_stateFlags & X_STATE_RUNPUSHBUFFERWASCALLED) != 0) {
	//}
	CxbxUpdateNativeD3DResources();
	LOG_INCOMPLETE(); // TODO : Read state from pgraph, convert to D3D
}

void D3D_draw_clear(NV2AState *d)
{
	PGRAPHState *pg = &d->pgraph;
	xbox::dword_xt Count;
	D3DRECT  *pRects,Rect;
	xbox::dword_xt Flags;
	D3DCOLOR Color;
	float Z;
	xbox::dword_xt Stencil;

	Flags = pg->KelvinPrimitive.ClearSurface;
	Color = pg->KelvinPrimitive.SetColorClearValue;
	Z = pg->KelvinPrimitive.SetZStencilClearValue;
	Stencil = Z;
	
	DWORD x1, x2, y1, y2;
	// FIXME, x1,x2, y2, y2 are premultiplied with supersamplescale already. shall we reverse the scale back before calling EMUPATCH(D3DDevice_Clear)?
	x1 = (pg->KelvinPrimitive.SetClearRectHorizontal & 0xFFFF);
	x2 = ((pg->KelvinPrimitive.SetClearRectHorizontal>>16) & 0xFFFF)+1;
	y1 = (pg->KelvinPrimitive.SetClearRectVertical & 0xFFFF);
	y2 = ((pg->KelvinPrimitive.SetClearRectVertical >> 16) & 0xFFFF) + 1;
	Rect.x1 = x1;
	Rect.x2 = x2;
	Rect.y1 = y1;
	Rect.y2 = y2;
	pRects = &Rect;
	Count = 1;
	DWORD HostFlags = 0;
	// Clear requires the Xbox viewport to be applied
	// CxbxUpdateNativeD3DResources();

	// make adjustments to parameters to make sense with windows d3d
	{
		if (Flags & xbox::X_D3DCLEAR_TARGET) {
			// TODO: D3DCLEAR_TARGET_A, *R, *G, *B don't exist on windows
			if ((Flags & xbox::X_D3DCLEAR_TARGET) != xbox::X_D3DCLEAR_TARGET)
				EmuLog(LOG_LEVEL::WARNING, "Unsupported : Partial D3DCLEAR_TARGET flag(s) for D3DDevice_Clear : 0x%.08X", Flags & xbox::X_D3DCLEAR_TARGET);

			HostFlags |= D3DCLEAR_TARGET;
		}

		// Do not needlessly clear Z Buffer
		if (Flags & xbox::X_D3DCLEAR_ZBUFFER) {
			if (pg->KelvinPrimitive.SetDepthTestEnable!=0)//was (g_bHasDepth)
				HostFlags |= D3DCLEAR_ZBUFFER;
			else
				EmuLog(LOG_LEVEL::WARNING, "Unsupported : D3DCLEAR_ZBUFFER flag for D3DDevice_Clear without ZBuffer");
		}

		// Only clear depth buffer and stencil if present
		//
		// Avoids following DirectX Debug Runtime error report
		//    [424] Direct3D8: (ERROR) :Invalid flag D3DCLEAR_ZBUFFER: no zbuffer is associated with device. Clear failed. 
		if (Flags & xbox::X_D3DCLEAR_STENCIL) {
			if (pg->KelvinPrimitive.SetStencilTestEnable!=0)// was (g_bHasStencil)
				HostFlags |= D3DCLEAR_STENCIL;
			else
				EmuLog(LOG_LEVEL::WARNING, "Unsupported : D3DCLEAR_STENCIL flag for D3DDevice_Clear without ZBuffer");
		}

		if (Flags & ~(xbox::X_D3DCLEAR_TARGET | xbox::X_D3DCLEAR_ZBUFFER | xbox::X_D3DCLEAR_STENCIL))
			EmuLog(LOG_LEVEL::WARNING, "Unsupported Flag(s) for D3DDevice_Clear : 0x%.08X", Flags & ~(xbox::X_D3DCLEAR_TARGET | xbox::X_D3DCLEAR_ZBUFFER | xbox::X_D3DCLEAR_STENCIL));
	}

	HRESULT hRet;

	hRet = g_pD3DDevice->Clear(Count, pRects, HostFlags, Color, Z, Stencil);
}

void D3D_draw_arrays(NV2AState *d)
{
	PGRAPHState *pg = &d->pgraph;

	LOG_TEST_CASE("D3D_draw_arrays");

	//LOG_INCOMPLETE(); // TODO : Implement D3D_draw_arrays

	if (NV2A_StreamCount == 0)
		return;

	//DWORD vertex data array, 
	// To be used as a replacement for DrawVertices, the caller needs to set the vertex format using IDirect3DDevice8::SetVertexInput before calling BeginPush.
	// All attributes in the vertex format must be padded DWORD multiples, and the vertex attributes must be specified in the canonical FVF ordering
	// (position followed by weight, normal, diffuse, and so on).
	// retrieve vertex shader
	// render vertices

	CxbxDrawContext DrawContext = {};
	DrawContext.XboxPrimitiveType = (xbox::X_D3DPRIMITIVETYPE)pg->primitive_mode;

	//this is assuming that all attributes are using the same vertex buffer and ordered with the same offset as in the slot.
	//could be wrong, need polished to use each pg->KelvinPrimitive.SetVertexDataArrayOffset[] for each attributes.
	
	//assert(D3D_StreamZeroStride > 0);
	DrawContext.uiXboxVertexStreamZeroStride = D3D_StreamZeroStride;
	
	extern void CxbxDrawPrimitive(CxbxDrawContext & DrawContext);

	if (NV2A_StreamCount == 1) {
		for (unsigned array_index = 0; array_index < pg->draw_arrays_length; array_index++) {
			//todo: make sure the stream input would be properly set when D3D_Xbox_StreamCount > 1
			// draw arrays

			//the address in pg->KelvinPrimitive.SetVertexDataArrayOffset[] are offsets from VRAM base 0x80000000, we have to add the base address to get full address.
			//this is only assuming there was only one vertex buffer and the SetVertexDataArrayOffset[0] is the starting address of the vertex buffer. this code should be revised once we finish the vertex buffer lookup code in D3D_draw_state_update()
			DrawContext.pXboxVertexStreamZeroData = (PVOID)(NV2A_StreamSource[0].VertexBuffer->Data | CONTIGUOUS_MEMORY_BASE);
			DrawContext.pXboxIndexData = nullptr;
			DrawContext.dwVertexCount = pg->gl_draw_arrays_count[array_index];
			DrawContext.dwStartVertex = pg->gl_draw_arrays_start[array_index];
			//because CxbxDrawPrimitiveUP() can only handle dwStartVertex == 0, so we shift the pXboxVertexStreamZeroData to where dwStartVertex is, and reset dwStartVertex to 0.
			DrawContext.pXboxVertexStreamZeroData = (PVOID)((DWORD)DrawContext.pXboxVertexStreamZeroData + DrawContext.uiXboxVertexStreamZeroStride * DrawContext.dwStartVertex);
			DrawContext.dwStartVertex = 0;

			CxbxDrawPrimitiveUP(DrawContext);

		}
	}
	else {
		/*
		// get max vertex count in all arrays.
		unsigned int maxVtxCount = pg->gl_draw_arrays_count[0];
		for (int i = 1; i < pg->draw_arrays_length; i++) {
			if (maxVtxCount < pg->gl_draw_arrays_count[i])
				maxVtxCount = pg->gl_draw_arrays_count[i];
		}

		// setup temp index buffer to call CxbxDrawIndexed()
		INDEX16* tmpIndex = (INDEX16*)malloc(maxVtxCount * sizeof(INDEX16));
		for (int i = 0; i < maxVtxCount; i++)
			DrawContext.pXboxIndexData[i] = i;
		*/
		for (unsigned array_index = 0; array_index < pg->draw_arrays_length; array_index++) {
			//todo: make sure the stream input would be properly set when D3D_Xbox_StreamCount > 1

			//the address in pg->KelvinPrimitive.SetVertexDataArrayOffset[] are offsets from VRAM base 0x80000000, we have to add the base address to get full address.
			//set pXboxVertexStreamZeroData = 0 to let CxbxDrawPrimitive() setup stream inputs.
			DrawContext.pXboxVertexStreamZeroData = 0;
			DrawContext.pXboxIndexData = nullptr;
			DrawContext.dwBaseVertexIndex = 0;
			DrawContext.dwVertexCount = pg->gl_draw_arrays_count[array_index];
			DrawContext.dwStartVertex = pg->gl_draw_arrays_start[array_index];
			// keep this assert() here to let us check whether there was really a draw call using multiple stream input or not.
			//assert(D3D_Xbox_StreamCount == 1);
			// todo: check the vertex buffer/stream input convert code for multiple stream input and the effect of StartVertex.
			CxbxDrawPrimitive(DrawContext);
		}
		//free the temp index buffer
		//free(tmpIndex);
	}
}

void D3D_draw_inline_buffer(NV2AState *d)
{
	PGRAPHState *pg = &d->pgraph;

	LOG_TEST_CASE("D3D_draw_inline_buffer");

	LOG_INCOMPLETE(); // TODO : Finish implementation of D3D_draw_inline_buffer

	//currently we composed all attributes into one vertex buffer. this is slow but it's working and verified with HLE already.
	
	for (unsigned vtx_cnt = 0; vtx_cnt < pg->inline_buffer_length; vtx_cnt++) {
		unsigned uiStride = 0;
		for (unsigned slot = 0; slot < X_VSH_MAX_ATTRIBUTES; slot++) {
			if (pg->vertex_attributes[slot].set_by_inline_buffer) {
				pg->vertex_attributes[slot].offset = uiStride;
				memcpy(pg->inline_buffer + (vtx_cnt * D3D_StreamZeroStride + pg->vertex_attributes[slot].offset) / 4,
					pg->vertex_attributes[slot].inline_buffer + vtx_cnt * 4,
					4 * sizeof(float));
				uiStride += 4 * sizeof(float);
			}
		}
	}
	
	CxbxDrawContext DrawContext = {};
	DrawContext.pXboxIndexData = nullptr;
	DrawContext.XboxPrimitiveType = (xbox::X_D3DPRIMITIVETYPE)pg->primitive_mode;
	DrawContext.dwVertexCount = pg->inline_buffer_length;
	DrawContext.dwStartVertex = 0;
	DrawContext.pXboxVertexStreamZeroData = pg->inline_buffer;
	//assert(D3D_StreamZeroStride > 0);
	DrawContext.uiXboxVertexStreamZeroStride = D3D_StreamZeroStride;
	//assert(D3D_Xbox_StreamCount == 1);
	CxbxDrawPrimitiveUP(DrawContext);
	//reset vertex attribute setting flags.
	for (unsigned slot = 0; slot < X_VSH_MAX_ATTRIBUTES; slot++) {
		pg->vertex_attributes[slot].set_by_inline_buffer = false;
	}
}


void D3D_draw_inline_array(NV2AState *d)
{
   	PGRAPHState *pg = &d->pgraph;

	//DWORD vertex data array, 
	// To be used as a replacement for DrawVerticesUP, the caller needs to set the vertex format using IDirect3DDevice8::SetVertexShader before calling BeginPush.
	// All attributes in the vertex format must be padded DWORD multiples, and the vertex attributes must be specified in the canonical FVF ordering
	// (position followed by weight, normal, diffuse, and so on).
	// retrieve vertex shader
	//if (g_Xbox_VertexShader_Handle == 0) {
	//	LOG_TEST_CASE("FVF Vertex Shader is null");
	//}
	// render vertices
	//else {
		CxbxDrawContext DrawContext = {};
		DrawContext.pXboxIndexData = nullptr;
		assert((xbox::X_D3DPRIMITIVETYPE)pg->primitive_mode != xbox::X_D3DPT_INVALID);
		DrawContext.XboxPrimitiveType = (xbox::X_D3DPRIMITIVETYPE)pg->primitive_mode;
		//get vertex size in dword or float.
		//assert(D3D_StreamZeroStride > 0);
		if (D3D_StreamZeroStride == 0)
			return;// test case: GUN, call inline arrays without setting up vertex data array format.
		DWORD dwVertexSizeDwords = D3D_StreamZeroStride / sizeof(float);
		//pg->inline_array_length was advanced every time we receive a dword/float from pushbuffer.
		//here we convert it to the actual vertex count.
		DrawContext.dwVertexCount = pg->inline_array_length / dwVertexSizeDwords;
		//DrawContext.dwStartVertex = 0;
		DrawContext.pXboxVertexStreamZeroData = pg->inline_array;
		DrawContext.uiXboxVertexStreamZeroStride = D3D_StreamZeroStride;
		//assert(D3D_Xbox_StreamCount == 1);
		//inline array
		CxbxDrawPrimitiveUP(DrawContext);

	//}
}

void D3D_draw_inline_elements(NV2AState *d)
{
	PGRAPHState *pg = &d->pgraph;

	if (NV2A_StreamCount == 0)
		return;

	CxbxDrawContext DrawContext = {};
	DrawContext.XboxPrimitiveType = (xbox::X_D3DPRIMITIVETYPE)pg->primitive_mode;
	DrawContext.uiXboxVertexStreamZeroStride = (pg->KelvinPrimitive.SetVertexDataArrayFormat[0] >> 8); // NV097_SET_VERTEX_DATA_ARRAY_FORMAT_STRIDE
	//set pXboxVertexStreamZeroData=0 because we're not using DrawIndexedPrimitiveUp. let CxbxDrawIndexed() convert the stream/vertex buffer.
	DrawContext.pXboxVertexStreamZeroData = (PVOID)(NV2A_StreamSource[0].VertexBuffer->Data | CONTIGUOUS_MEMORY_BASE);//(PVOID)(pg->KelvinPrimitive.SetVertexDataArrayOffset[0] | CONTIGUOUS_MEMORY_BASE);//0
	DrawContext.dwVertexCount = pg->inline_elements_length;
	DrawContext.dwStartVertex = 0;
	DrawContext.pXboxIndexData = d->pgraph.inline_elements;
	extern void CxbxDrawIndexedPrimitiveUP(CxbxDrawContext & DrawContext);
	//todo: for some cases, there is only one stream input when dealing with pgraph kelvin, and we could simply use codes similar to CxbxDrawPrimitiveUP() and call g_pD3DDevice->DrawIndexedPrimitiveUP() to simplify the code and save some time in copying vetex buffers.
	/*
	if (D3D_Xbox_StreamCount == 1) {
		CxbxDrawIndexedPrimitiveUP(DrawContext);
	}
	// xdk sample DohphinClassic uses 2 stream input, and declare 3 streams in vertex shader decl.
	// Otogi also uses multiple stream input.
	else {
	*/
		DrawContext.pXboxVertexStreamZeroData = 0;
		CxbxDrawIndexed(DrawContext);
	//}

}

// Import pgraph_draw_* variables, declared in EmuNV2A_PGRAPH.cpp :
extern void(*pgraph_draw_state_update)(NV2AState *d);
extern void(*pgraph_draw_clear)(NV2AState *d);
extern void(*pgraph_draw_arrays)(NV2AState *d);
extern void(*pgraph_draw_inline_buffer)(NV2AState *d);
extern void(*pgraph_draw_inline_array)(NV2AState *d);
extern void(*pgraph_draw_inline_elements)(NV2AState *d);

void HLE_init_pgraph_plugins()
{
	/* Attach Direct3D render plugins */
	pgraph_draw_state_update = D3D_draw_state_update;
	pgraph_draw_clear = D3D_draw_clear;
	pgraph_draw_arrays = D3D_draw_arrays;
	pgraph_draw_inline_buffer = D3D_draw_inline_buffer;
	pgraph_draw_inline_array = D3D_draw_inline_array;
	pgraph_draw_inline_elements = D3D_draw_inline_elements;
}

//int pgraph_handle_method(NV2AState *d, unsigned int subchannel,
//unsigned int method, uint32_t parameter,
//uint32_t *parameters, size_t method count,
//size_t max_lookahead_words)
extern int pgraph_handle_method(
	NV2AState *d,
	unsigned int subchannel,
	uint32_t command_word,
	uint32_t parameter,
	uint32_t *parameters,
	uint32_t method_count,
	uint32_t max_lookahead_words);

// LLE NV2A
extern NV2ADevice* g_NV2A;

uint32_t HLE_read_NV2A_pgraph_register(const int reg)
{
	NV2AState* dev = g_NV2A->GetDeviceState();
	PGRAPHState *pg = &(dev->pgraph);
	return pg->regs[reg];
}
/* //disable for now, use HLE_get_NV2A_vertex_program_slot_ptr(const DWORD slot_index) instead.
uint32_t HLE_read_NV2A_vertex_program_slot(unsigned program_load, unsigned slot)
{
	NV2AState* dev = g_NV2A->GetDeviceState();
	PGRAPHState* pg = &(dev->pgraph);

	// See CASE_32(NV097_SET_TRANSFORM_PROGRAM, 4) in LLE pgraph_handle_method()
	assert(program_load < NV2A_MAX_TRANSFORM_PROGRAM_LENGTH);
	uint32_t value = pg->vsh_program_slots[program_load][slot % 4];

	return value;
}
*/
float *HLE_get_NV2A_vertex_constant_float4_ptr(unsigned const_index)
{
	NV2AState* dev = g_NV2A->GetDeviceState();
	PGRAPHState* pg = &(dev->pgraph);

	// See CASE_32(NV097_SET_TRANSFORM_CONSTANT, 4) in LLE pgraph_handle_method()
	assert(const_index < NV2A_VERTEXSHADER_CONSTANTS);
	return (float*)&(pg->vsh_constants[const_index][0]);
}

xbox::dword_xt * HLE_get_NV2A_vertex_program_slot_ptr(const DWORD slot_index)
{
	NV2AState* dev = g_NV2A->GetDeviceState();
	PGRAPHState* pg = &(dev->pgraph);

	// See CASE_32(NV097_SET_TRANSFORM_CONSTANT, 4) in LLE pgraph_handle_method()
	assert(slot_index < NV2A_VERTEXSHADER_CONSTANTS);
	return (xbox::dword_xt*)&(pg->vsh_program_slots[slot_index][0]);
}


// For now, skip the cache, but handle the pgraph method directly
// Note 2 : d is read from local scope, and ni is unused (same in LLE)
// Note 3 : Keep EmuExecutePushBufferRaw skipping all commands not intended for channel 0 (3D)
// Note 4 : Prevent a crash during shutdown when g_NV2A gets deleted
#define CACHE_PUSH(subc, mthd, word, dma_get, mcnt,max_words_available, ni) \
	if (subc == 0) { \
		if (g_NV2A) { \
			pgraph_handle_method(d, subc, mthd, word, dma_get, mcnt,max_words_available); \
		} \
	}
uint32_t * g_NV2A_pushbuffer_base_addr = nullptr;
inline uint32_t get_NV2A_pushbuffer_size(void)
{
	return 0x400000;//regular xbe uses 0x80000, Otogi uses 0x400000, we'll have to figure out a general way to retrive the correct info.
}
extern void EmuExecutePushBufferRaw
(
    void *pPushData,
    uint32_t uSizeInBytes,
    uint32_t **p_dma_get, //pass in pointer of dma_get, when called via pfifo_run_pusher()
    uint32_t **p_dma_put,  //pass in pointer of dma_put, when called via pfifo_run_pusher()
	uint8_t *dma
)
{
    // Test-case : Azurik (see https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/360)
    // Test-case : Crash 'n' Burn [45530014]
    // Test-case : CrimsonSea [4B4F0002]
    // Test-case : Freedom Fighters
    // Test-case : Hot Wheels Stunt Track Challenge [54510089] (while running hw2F.xbe)
    // Test-case : Hunter Redeemer
    // Test-case : Inside Pitch 2003 [4D530034]
    // Test-case : Need for Speed Most Wanted [4541007B]
    // Test-case : Otogi (see https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/pull/1113#issuecomment-385593814)
    // Test-case : Prince of Persia: The Sands of Time [5553001d]
    // Test-case : RalliSport (see https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/904#issuecomment-362929801)
    // Test-case : RPM Tuning [Top Gear RPM Tuning] [4B420007]
    // Test-case : SpyHunter 2 [4D57001B]
    // Test-case : Star Wars Jedi Academy (see https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/904#issuecomment-362929801)
    // Test-case : Turok (in main menu)
    // Test-case : Whiplash

    assert(pPushData);
    assert(uSizeInBytes >= 4);

    // Retrieve NV2AState via the (LLE) NV2A device :
    NV2AState *d = g_NV2A->GetDeviceState();
    d->pgraph.pgraph_regs[NV_PGRAPH_CTX_CONTROL/4] |= NV_PGRAPH_CTX_CONTROL_CHID; // avoid assert in pgraph_handle_method()

    // DMA Pusher state -- see https://envytools.readthedocs.io/en/latest/hw/fifo/dma-pusher.html#pusher-state
#if 0
    static xbox::addr_xt dma_pushbuffer; // the pushbuffer and IB DMA object
#endif
    uint32_t *dma_limit; // pushbuffer size limit
    uint32_t *dma_put; // pushbuffer current end address
    uint32_t *dma_get; //pushbuffer current read address
    struct {
        xbox::NV2AMETHOD mthd; // Current method
        uint32_t subc; // :3 = Current subchannel
        uint32_t mcnt; // :24 = Current method count
        // Note : dcount_shadow is listed here in envytools "Pusher state" overview, but isn't prefixed with dma_struct, so moved below
        bool ni; // Current command's NI (non-increasing) flag
    } dma_state;
    static uint32_t dcount_shadow = 0; // [NV5:] Number of already-processed methods in cmd
    // TODO : static uint32_t ref; // reference counter [shared with puller]
    static bool subr_active = false; // Subroutine active
    static uint32_t *subr_return = nullptr; // subroutine return address
    // TODO : static bool big_endian = false; // pushbuffer endian switch
    // End of DMA pusher state

    // DMA troubleshooting values -- see https://envytools.readthedocs.io/en/latest/hw/fifo/dma-pusher.html#errors
    static uint32_t *dma_get_jmp_shadow = nullptr; // value of dma_get before the last jump
    static uint32_t rsvd_shadow = 0; // the first word of last-read command
    static uint32_t data_shadow = 0; // the last-read data word

    // Overlay, to ease decoding the PFIFO command word
    union {
        uint32_t word;
        nv_fifo_command command;
    };

    // Initialize working variables
    dma_limit = (uint32_t*)((xbox::addr_xt)pPushData + uSizeInBytes); // TODO : If this an absolute addresss?
    dma_put = (uint32_t*)((xbox::addr_xt)pPushData + uSizeInBytes);
    dma_get = (uint32_t*)pPushData;
    dma_state = {};
	if (dma_get == dma_put) {
		//dma starving, shall we wait or sleep for a while?
		goto finish;
	}

	if (g_NV2A_pushbuffer_base_addr != nullptr) {
		dma_limit = (uint32_t *)(((DWORD)g_NV2A_pushbuffer_base_addr) + get_NV2A_pushbuffer_size());//pushbuffer size 0x80000, xbox reserved area in the end 0x100 - 4
	}
	if (dma_limit < dma_put) {
		//this happeded in Otogi. it seems Otogi uses total pushbuffer length larger than 0x80000.
		dma_limit = dma_put;
	}
	if (dma_get > dma_put) {
		//we're in segment switch phase, proceed parsing and get ready for a jump command.
		LOG_TEST_CASE("Pushbuffer segment switch to base");
	}

	// NV-4-style PFIFO DMA command stream pusher
    // See https://envytools.readthedocs.io/en/latest/hw/fifo/dma-pusher.html#the-pusher-pseudocode-pre-gf100
    while (dma_get != dma_put) {
        if (p_dma_get != nullptr)
            *p_dma_get = (uint32_t*)((uint32_t)dma_get - (uint32_t)dma);       //update the pfifo_dma_get if we're called via pfifo_run_pusher()

        // Check if loop reaches end of pushbuffer
		if (g_NV2A_pushbuffer_base_addr != nullptr){
			if (dma_get >= dma_limit) {
				LOG_TEST_CASE("Last pushbuffer instruction exceeds END of Data");
				// reset dma_get to dma_put.
				assert(0);
				goto finish; // For now, don't even attempt to run through
			}
		}

        // Read a DWORD from the current push buffer pointer
        uint32_t * word_ptr = dma_get;//shadow copy of dma_get before it gets advanced after the word was read.
        word = *dma_get++;

        if (dma_state.mcnt == 0) {
            /* no command active - this is the first word of a new one */
            rsvd_shadow = word;
            // Check and handle command type, then instruction, then flags
            switch (command.type) {

                //***************************we need to check the jump address once we encounter these methods.
                //because the jump address is obviously not in the VRAM range. perhaps a memory range transform must be applied first?

            case COMMAND_TYPE_NONE:
                if (command.instruction == COMMAND_INSTRUCTION_JUMP) {//old jump
                    LOG_TEST_CASE("Pushbuffer COMMAND_INSTRUCTION_JUMP");
                    dma_get_jmp_shadow = dma_get;
                    dma_get = (uint32_t *)(CONTIGUOUS_MEMORY_BASE | (word & COMMAND_WORD_MASK_JUMP));
                    continue; // while
                }
                if ((command.instruction == COMMAND_INSTRUCTION_INCREASING_METHODS) && (command.flags == COMMAND_FLAGS_RETURN)) {
                    // Note : NV2A return shouldn't be used because the NV2A call is used as jump and the subr_return is used as an flag of where the jump was from.
                    LOG_TEST_CASE("Pushbuffer COMMAND_FLAGS_RETURN, not supposed to be used!");
                    if (word != (COMMAND_FLAGS_RETURN << 16)) {
                        LOG_TEST_CASE("Pushbuffer COMMAND_FLAGS_RETURN with additional bits?!");
                        goto finish; // For now, don't even attempt to run through
                    }
                    else {
                        LOG_TEST_CASE("Pushbuffer COMMAND_FLAGS_RETURN");
                    }

                    if (!subr_active) {
                        LOG_TEST_CASE("Pushbuffer COMMAND_FLAGS_RETURN while another call was active!");
                        // TODO : throw DMA_PUSHER(RET_SUBR_INACTIVE);
                        goto finish; // For now, don't even attempt to run through
                    }

                    dma_get = subr_return;
                    subr_active = false;
                    continue; // while
                }
                break;
            case COMMAND_TYPE_JUMP_LONG:
                LOG_TEST_CASE("Pushbuffer COMMAND_TYPE_JUMP_LONG");
				//only set g_NV2A_pushbuffer_base_addr when we're jumping from the starting address of VRAM, and g_NV2A_pushbuffer_base_addr is not set yet.
				if (g_NV2A_pushbuffer_base_addr == nullptr && dma_get == (uint32_t*)0x80000004) {
					g_NV2A_pushbuffer_base_addr = (uint32_t *)(CONTIGUOUS_MEMORY_BASE | (word & COMMAND_WORD_MASK_JUMP_LONG));
				}
				dma_get_jmp_shadow = dma_get;
                dma_get = (uint32_t *)(CONTIGUOUS_MEMORY_BASE | (word & COMMAND_WORD_MASK_JUMP_LONG));
                //the first long jump is the jump to pushbuffer base set in CDevice init.

				// NV2A uses long jump as subroutine call return. so we always clear subr_active here.
				subr_active = false;

                continue; // while
            case COMMAND_TYPE_CALL: // Note : NV2A call is used as jump, the sub_return is used as a flag to indicate where the jump was orginated.
				
				
				if (subr_active) {
                    LOG_TEST_CASE("Pushbuffer COMMAND_TYPE_CALL while another call was active!");
                    // TODO : throw DMA_PUSHER(CALL_SUBR_ACTIVE);
					// treat call normally. 
                    //goto finish; // For now, don't even attempt to run through
                }
                else {
                    LOG_TEST_CASE("Pushbuffer COMMAND_TYPE_CALL");
                }
				
                subr_return = dma_get;
                subr_active = true;
                dma_get = (uint32_t *)(CONTIGUOUS_MEMORY_BASE | (word & COMMAND_WORD_MASK_JUMP_LONG));
                continue; // while
            default:
                LOG_TEST_CASE("Pushbuffer COMMAND_TYPE unknown");
                // TODO : throw DMA_PUSHER(INVALID_CMD);
                goto finish; // For now, don't even attempt to run through
            } // switch type

            switch (command.instruction) {
            case COMMAND_INSTRUCTION_INCREASING_METHODS:
                dma_state.ni = false;
                break;
            //case COMMAND_INSTRUCTION_JUMP:  //move jump handler to the front, together with other jump/call handlers.
            case COMMAND_INSTRUCTION_NON_INCREASING_METHODS:
                dma_state.ni = true;
                break;
            default:
                LOG_TEST_CASE("Pushbuffer COMMAND_INSTRUCTION unknown");
                // TODO : throw DMA_PUSHER(INVALID_CMD);
                goto finish; // For now, don't even attempt to run through
            } // switch instruction

            switch (command.flags) {
            case COMMAND_FLAGS_NONE: // Decode push buffer method & size (inverse of D3DPUSH_ENCODE)
                dma_state.mthd = command.method;
                dma_state.subc = command.subchannel;
                dma_state.mcnt = command.method_count;
                break;
            case COMMAND_FLAGS_SLI_CONDITIONAL:
                LOG_TEST_CASE("Pushbuffer COMMAND_FLAGS_SLI_CONDITIONAL (NV40+) not available on NV2A");
                break;
            case COMMAND_FLAGS_RETURN:
                LOG_TEST_CASE("Pushbuffer COMMAND_FLAGS_RETURN should already be handled earlier");
                break;
            case COMMAND_FLAGS_LONG_NON_INCREASING_METHODS:
                LOG_TEST_CASE("Pushbuffer COMMAND_FLAGS_LONG_NON_INCREASING_METHODS [IB-mode only] not available on NV2A");
                break;
            } // switch flags

            dcount_shadow = 0;
            continue; // while for next word
        } // if (dma_state.mcnt == 0)

        /* data word of methods command */
        data_shadow = word;
#if 0
        if (!PULLER_KNOWS_MTHD(dma_state.mthd)) {
            throw DMA_PUSHER(INVALID_MTHD);
            return; // For now, don't even attempt to run through
        }

#endif
        //words still available in the pushbuffer.
        uint32_t max_words_available = (dma_get>dma_put)?((uint32_t)dma_limit - (uint32_t)word_ptr) / 4: ((uint32_t)dma_put - (uint32_t)word_ptr) / 4;
        uint32_t num_processed = 1;

        if (dma_state.mcnt > max_words_available) { 
            //this is not supposed to happen. if the pushbuffer is correct, and the method handler works right. method count always represents the dwords of
            //the parameters required by the method. so at least there should be method count dwords left in the buffer for the method to use.
            LOG_TEST_CASE("Pushbuffer data not enough for method count!");
            goto finish;//we shall not run through this situation.
        }

        if (g_NV2A) {
            //return num_processed, the words processed here by the method handler. so the caller can advance the dma_get pointer of the pushbuffer
            //num_processed default to 1, which represent the first parameter passed in this call.
            //but that word is advanced by the caller already. it's the caller's duty to subtract that word from the num_processed;
            num_processed = pgraph_handle_method(
                d,						//NV2AState
                dma_state.subc,			//Subchannel
                rsvd_shadow,			//command_word // Was dma_state.mthd, but nowadays we need the full 32 bit command word in there (to allow a generic non-increment check)
                word,					//first parameter
                word_ptr,				//parameters, pointer to 1st parameter, which is exact parameter in the args. // It's the address where we read the word. we can't use dma_get here because dma_get was advanced after word was read.
                dma_state.mcnt,			//method count
                max_words_available);	//words still available in the pushbuffer
        }

#if 0 // Disabled, but why?
        if (!dma_state.ni) {
            dma_state.mthd++;
        }

        if (num_processed > 1) {
#endif
            dma_get = word_ptr + dma_state.mcnt;//num_processed default to 1.
            dcount_shadow += num_processed;
            dma_state.mcnt = 0;
#if 0 // Disabled, but why?
            if (dma_state.mcnt != 0) {
               assert(dma_state.mcnt == 0);
               LOG_TEST_CASE("Pushbuffer method count not decreased to 0 after method processing");
               dma_state.mcnt = 0;
            }
        }
        else {
            dma_state.mcnt--;
            dcount_shadow++;
        }
#endif
    } // while (dma_get != dma_put)

	if (dma_get != dma_put) {
		return;
	}

	//dma starving, shall we wait or sleep for a while?
finish:
	if (p_dma_get != nullptr)
		*p_dma_get = (uint32_t*)((uint32_t)dma_put - (uint32_t)dma); //update the pfifo_dma_get to the end of pushbuffer if we're called via pfifo_run_pusher()
}

const char *NV2AMethodToString(DWORD dwMethod)
{
	switch (dwMethod) {

#define ENUM_RANGED_ToString_N(Name, Method, Pitch, N) \
	case Name(N): return #Name "((" #N ")*" #Pitch ")";

#define ENUM_RANGED_ToString_1(Name, Method, Pitch) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 0)

#define ENUM_RANGED_ToString_2(Name, Method, Pitch) \
	ENUM_RANGED_ToString_1(Name, Method, Pitch) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 1)

#define ENUM_RANGED_ToString_3(Name, Method, Pitch) \
	ENUM_RANGED_ToString_2(Name, Method, Pitch) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 2)

#define ENUM_RANGED_ToString_4(Name, Method, Pitch) \
	ENUM_RANGED_ToString_3(Name, Method, Pitch) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 3) 

#define ENUM_RANGED_ToString_6(Name, Method, Pitch) \
	ENUM_RANGED_ToString_4(Name, Method, Pitch) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 4) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 5)

#define ENUM_RANGED_ToString_8(Name, Method, Pitch) \
	ENUM_RANGED_ToString_6(Name, Method, Pitch) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 6) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 7)

#define ENUM_RANGED_ToString_10(Name, Method, Pitch) \
	ENUM_RANGED_ToString_8(Name, Method, Pitch) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 8) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 9) \

#define ENUM_RANGED_ToString_16(Name, Method, Pitch) \
	ENUM_RANGED_ToString_10(Name, Method, Pitch) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 10) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 11) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 12) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 13) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 14) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 15)

#define ENUM_RANGED_ToString_32(Name, Method, Pitch) \
	ENUM_RANGED_ToString_16(Name, Method, Pitch) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 16) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 17) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 18) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 19) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 20) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 21) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 22) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 23) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 24) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 25) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 26) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 27) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 28) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 29) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 30) \
	ENUM_RANGED_ToString_N(Name, Method, Pitch, 31)

#define ENUM_METHOD_ToString(Name, Method) case Method: return #Name;
#define ENUM_RANGED_ToString(Name, Method, Pitch, Repeat) ENUM_RANGED_ToString_##Repeat(Name, Method, Pitch)
#define ENUM_BITFLD_Ignore(Name, Value)
#define ENUM_VALUE_Ignore(Name, Value)

	ENUM_NV2A(ENUM_METHOD_ToString, ENUM_RANGED_ToString, ENUM_BITFLD_Ignore, ENUM_VALUE_Ignore)

	default:
		return "UNLABLED";
	}
}
