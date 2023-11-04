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
extern bool g_nv2a_fifo_is_busy;// in Direct3D9.cpp
extern bool is_pushbuffer_recording(void);
void backup_xbox_texture_state(void);
void restore_xbox_texture_state(void);
void pgraph_purge_dirty_and_state_flag(void);
void D3D_draw_state_update(NV2AState *d);
//this function is no longer used since we can't call pgraph method handler directly in xbox api side, or we would introduce memory corruption in pushbuffer.
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

// log2 table 
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
				//DWORD control0;
				DWORD filter, address, magFilter, minFilter, colorSign, convolutionKernel;
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
				
				int32_t intLodBias;
				intLodBias = (filter & NV097_SET_TEXTURE_FILTER_MIPMAP_LOD_BIAS);
				// sign-extend highest (sign-)bit of NV097_SET_TEXTURE_FILTER_MIPMAP_LOD_BIAS :
				if (intLodBias & 0x1000)
					intLodBias |= 0xFFFFF000;
				float fLodBias = (float)intLodBias;
				float mipMapLodBias = fLodBias/256.0f;

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
}

void pgraph_use_FixedPixelShader(void)
{
	// set pixel shader pointer to null for fixed function pixel shader
	pNV2A_PixelShader = nullptr;
}
// called when SetModelVeiw(0,0,0) was called.
void pgraph_use_Transform(void)
{
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
	g_NV2A_transform_ModelView_dirty[index] = true;
}
void pgraph_SetInverseModelViewMatrixDirty(unsigned int index)
{
	g_NV2A_transform_InverseModelView_dirty[index] = true;
}
void pgraph_SetCompositeMatrixDirty(void)
{
	g_NV2A_transform_Composite_dirty = true;
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
	//default d3d_stage to be the same as NV2A_stage, and sets xbox TextureStates in the same stage. and we also set bumpenv in stage 0.
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
	// in fixed mode, we only use stage 0~3, and when point sprite is enabled, combiner set to texture stage state 3 will be redirect to NV2A combiner stage 0, but the point sprite texture remains in stage 3.
	int startStage = 0;
	// FIXME!!! there is a stage over ride in Apply() of TextureStageStates class. When point sprite was enabled, it will override stage 0 with state from stage 3.
	if (pg->KelvinPrimitive.SetPointSmoothEnable != 0)
		startStage = 3;
	int stage = startStage;
	// xbox LazySetCombiners() start with both i and startStage as 3 when point sprite was enabled without updating i==0~2. // this is questionable since the actual combiner content suggest that when point sprite is enabled, only combiner stage 0 will be set.
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
			if ((colorICW == 0x04200000)&& (pg->KelvinPrimitive.SetCombinerColorICW[1]==0)) {//(colorICW == (NV097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_4 | NV097_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_INVERT  )) && alphaICW == (colorICW | NV097_SET_COMBINER_COLOR_ICW_A_ALPHA )) { //,(0x10 & 0x20) << 23)
				NV2A_colorOP[i] = xbox::X_D3DTOP_DISABLE;
				NV2A_alphaOP[i] = xbox::X_D3DTOP_DISABLE;
			}
			
			if ((alphaICW == (0x14200000))&& pg->KelvinPrimitive.SetCombinerAlphaICW[1]==0) {//(colorICW == (NV097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_4 | NV097_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_INVERT  )) && alphaICW == (colorICW | NV097_SET_COMBINER_COLOR_ICW_A_ALPHA )) { //,(0x10 & 0x20) << 23)
				NV2A_alphaOP[i] = xbox::X_D3DTOP_DISABLE;
			}
		}
		
		if (NV2A_colorOP[i] != xbox::X_D3DTOP_DISABLE) {
			// process color OP if it's not X_D3DTOP_LAST
			if (NV2A_colorOP[i] > xbox::X_D3DTOP_LAST) {
				// colorOP SelectARG1 , A == Arg1, B == 1, C == 0, D == 0
				if (((colorICW & 0xF0FFFFFF) == 0x00200000) && ((colorOCW & 0xFFFC00FF) == 0)) {
					NV2A_colorOP[i] = xbox::X_D3DTOP_SELECTARG1;
					// arg1 always source A
					NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF); // FIXME!!!,  reg 4 is D3DTA_DIFFUSE :0, not sure this direct shift is correct or not 6
					//special case happen with RSC. ICW =0x04201000, OCW=00010C000, which means result ARG = (arg1*1+0)<<1==2*arg1. we use arg1+arg1 to replace the multiplication.
					/*
					if ((colorOCW & NV097_SET_COMBINER_COLOR_OCW_OP_SHIFTLEFTBY1_BIAS) == NV097_SET_COMBINER_COLOR_OCW_OP_SHIFTLEFTBY1) {
						//special case
						NV2A_colorOP[i] = xbox::X_D3DTOP_ADD;
						NV2A_colorArg2[i]= NV2A_colorArg1[i];
						NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORARG2, NV2A_colorArg2[i]);
					}
					*/
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
			}
		}
		
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
		if (NV2A_alphaOP[i] != xbox::X_D3DTOP_DISABLE) {
			// now we process alphaOP
			if (NV2A_alphaOP[i] > xbox::X_D3DTOP_LAST) {
				// alphaOP SelectARG1  A=arg1, B=1
				if (((alphaICW & 0xF0FFFFFF) == 0x10200000) && ((alphaOCW & 0xFFFC00FF) == 0)) {
					NV2A_alphaOP[i] = xbox::X_D3DTOP_SELECTARG1;
					// arg1 always source A
					NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);// FIXME!!!,  reg 4 is D3DTA_DIFFUSE :0, not sure this direct shift is correct or not6
					/*
					if ((alphaOCW & NV097_SET_COMBINER_COLOR_OCW_OP_SHIFTLEFTBY1_BIAS) == NV097_SET_COMBINER_COLOR_OCW_OP_SHIFTLEFTBY1) {
						//special case
						NV2A_alphaOP[i] = xbox::X_D3DTOP_ADD;
						NV2A_alphaArg2[i] = NV2A_alphaArg1[i];
						NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG2, NV2A_alphaArg2[i]);
					}
					*/
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG1, NV2A_alphaArg1[i]);
				}
				// alphaOP SelectARG2 A=0, B=0, C=1, D=arg2
				else if (((alphaICW & 0xFFFFFFF0) == 0x00002010) && ((alphaOCW & 0xFFFF00FF) == 0)) {
					NV2A_alphaOP[i] = xbox::X_D3DTOP_SELECTARG2;
					// arg2 always source D
					NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW) & 0xF);// FIXME!!!,  reg 4 is D3DTA_DIFFUSE :0, not sure this direct shift is correct or not6
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG2, NV2A_alphaArg2[i]);

				}
				// OP modulate, A*B, C==0, D==0, A=Arg1, B=Arg2, 
				else if ((alphaICW & 0xE0E00FFF) == 0x0000000) {
					// arg1 in source A
					NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
					// arg2 in source B
					NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 16) & 0xF);
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
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG1, NV2A_alphaArg1[i]);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG2, NV2A_alphaArg2[i]);

				}
				// D3DTOP_PREMODULATE, A=Arg1, if stage 0, B==D3DTA_TEXTURE, if stage1 or stage2 B=1, C and D default to 0
				else if((i > 0 && i < 3 && ((alphaICW & 0xF0FFFFFF) == 0x10200000)) || (i == 0 && ((alphaICW & 0xF0FFFFFF) == 0x10120000))) {
					NV2A_alphaOP[i] = xbox::X_D3DTOP_PREMODULATE;
					// arg1 in source A
					NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAARG1, NV2A_alphaArg1[i]);
				}
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

		NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLOROP, NV2A_colorOP[i]);
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
	xbox::X_D3DVIEWPORT8 Viewport = {};

	float fm11, fm22, fm33, fm41, fm42, fm43, clipNear, clipFar;
	float SuperSampleScale, ScaleX, ScaleY, ScaleZ, ScreenSpaceOffsetX, ScreenSpaceOffsetY;
	
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
	extern VertexShaderMode g_NV2A_VertexShaderMode;//tmp glue
	if (g_NV2A_VertexShaderMode == VertexShaderMode::FixedFunction) {
	//if ((float)pg->KelvinPrimitive.SetViewportOffset[2] == 0.0 && (float)pg->KelvinPrimitive.SetViewportOffset[3] == 0.0) {
		xViewport = pg->KelvinPrimitive.SetViewportOffset[0];
		yViewport = pg->KelvinPrimitive.SetViewportOffset[1];
		// if use_w
		if ((pg->KelvinPrimitive.SetDepthTestEnable != D3DZB_FALSE) && (pg->KelvinPrimitive.SetZMinMaxControl== D3DZB_USEW)) {
			ScaleZ = clipFar;
			g_ZScale = ScaleZ;
			// FIXME !! need WNear *InverseWFar input
			// clipNear = pDevice->m_ZScale * pDevice->m_WNear * pDevice->m_InverseWFar;  pDevice->m_WNear * pDevice->m_InverseWFar=clipNear/pDevice->m_ZScale;
			
			//float WNear, WFar, InverseWFar;
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
		
		Viewport.Width= (DWORD)rendertargetBaseWidth;
		Viewport.Height = (DWORD)rendertargetBaseHeight;
	}
	else {
		ScaleZ = clipFar;
		g_ZScale = ScaleZ;
		fm11 = pg->KelvinPrimitive.SetViewportScale[0];
		fm22 = pg->KelvinPrimitive.SetViewportScale[1];
		fm33 = pg->KelvinPrimitive.SetViewportScale[2];
		fm41 = fm11;
		fm42 = -fm22;
		fm43 = pg->KelvinPrimitive.SetViewportOffset[2];

		xViewport = pg->KelvinPrimitive.SetViewportOffset[0] - fm41;
		yViewport = pg->KelvinPrimitive.SetViewportOffset[1] - fm42;

		Viewport.MinZ = fm43 / ScaleZ;
		Viewport.MaxZ = (fm33 / ScaleZ) + Viewport.MinZ;
		Viewport.Height = (DWORD)(fm22 / (-0.5*ScaleY));
		Viewport.Width = (DWORD)(fm11 / (0.5*ScaleX));


	}
	if((pg->KelvinPrimitive.SetAntiAliasingControl & NV097_SET_ANTI_ALIASING_CONTROL_ENABLE_TRUE) !=0)
	{
		xViewport += 0.5f;
		yViewport += 0.5f;
	}
	Viewport.X = (DWORD)((xViewport - ScreenSpaceOffsetX) / ScaleX);
	Viewport.Y = (DWORD)((yViewport - ScreenSpaceOffsetX) / ScaleY);
	
	extern xbox::X_D3DVIEWPORT8 HLE_Viewport;
	extern xbox::X_D3DVIEWPORT8 g_Xbox_Viewport;
	CxbxrSetViewport(Viewport);
}

void CxbxrImpl_LazySetPointParameters(NV2AState* d)
{
	PGRAPHState* pg = &d->pgraph;
	    // the LazySetPointParameters() converts the point size from float into DWORD and set it to pg->KelvinPrimitive.SetPointSize
		float FixedSize = (float)pg->KelvinPrimitive.SetPointSize;
		// reversed to xbox d3d fixed point size
		float size = FixedSize/8.0f;
		float max, min;
		float delta, factor, height;
		min = pg->KelvinPrimitive.SetPointParamsMin;
		if (min < 0.0f)min = 0.0f;
		delta = pg->KelvinPrimitive.SetPointParamsDeltaA;

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
				}
				NV2A_viewport_dirty = false;
			}
			CxbxrGetViewport(Viewport);
			height = (float)Viewport.Height;
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
	// compose viewport parameters via pgraph contents if needed
	// update Viewport info if needed. though this should already been updated before we got here.
	if (NV2A_viewport_dirty == true) {
		// this only compose viewport from pgraph content 
		pgraph_ComposeViewport(d);
		// todo: actually setviewport with the composed viewport, currently we don't set the host viewport via pgraph content, yet. the SetViewport() is currently HLEed and not processed in pushbuffer.
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

	// SetModelView() call only be calaled when pg->KelvinPrimitive.SetSkinMode==0.
	//
	// SetModelview() sets:
	// PushMatrixTransposed(pPush, NV097_SET_MODEL_VIEW_MATRIX0, pModelView);
	// PushMatrixTransposed(pPush + 17, NV097_SET_COMPOSITE_MATRIX, pComposite);
	// inverse modelview matrix is optional: PushInverseModelViewMatrix(pPush,NV097_SET_INVERSE_MODEL_VIEW_MATRIX0,pInverseModelView);
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
	D3DMATRIX matUnit = {};
	matUnit._11 = 1.0f;
	matUnit._22 = 1.0f;
	matUnit._33 = 1.0f;
	matUnit._44 = 1.0f;
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

    //pgraph always used modelview matrix, only difference is 
	if ((NV2A_DirtyFlags & X_D3DDIRTYFLAG_DIRECT_MODELVIEW) != 0) {
		// transpose KelvinPrimitive transform back to xbox d3d transform
		D3DXMatrixMultiply((D3DXMATRIX*)&g_NV2A_transform_ProjectionViewportTransform, (D3DXMATRIX*)&g_NV2A_transform_InverseModelView, (D3DXMATRIX*)&g_NV2A_transform_Composite);
	}
	else {
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
	// CxbxrImpl_GetTransform(xbox::X_D3DTRANSFORMSTATETYPE::X_D3DTS_PROJECTION, &g_NV2A_DirectModelView_Projection);
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
			fogTableDensity = -scale / 0.21233003f;
			NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_FOGDENSITY, FtoDW(fogTableDensity));
		}
		else if (fogMode == NV097_SET_FOG_MODE_V_EXP) {
			fogTableMode = D3DFOG_EXP;
			fogTableDensity = -scale/ 0.090168074f;
			NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_FOGDENSITY, FtoDW(fogTableDensity));
		}
		else if (fogMode == NV097_SET_FOG_MODE_V_LINEAR) {
			if (fogGenMode == NV097_SET_FOG_GEN_MODE_V_SPEC_ALPHA) {
				fogTableMode = D3DFOG_NONE;
				// FIXME!!! need to set bD3DRS_RangeFogEnable here because fogGenMode is not correctly verified.
				// D3DFOG_NONE : No fog effect, so set D3DRS_RangeFogEnable to false
				// bD3DRS_RangeFogEnable = false;
			}else{
				fogTableMode = D3DFOG_LINEAR;
				FLOAT fogTableLinearScale;
				fogTableLinearScale = -scale;
				fogTableEnd = (bias - 1.0f) / fogTableLinearScale;
				// MAX_FOG_SCALE == 8192.0f
				if (fogTableLinearScale == 8192.0f) {
					fogTableStart = fogTableEnd;
				}
				else {
					fogTableStart = fogTableEnd - 1.0f / fogTableLinearScale;
				}
				NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_FOGSTART, FtoDW(fogTableStart));
				NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_FOGEND, FtoDW(fogTableEnd));
			}

		}
		
		NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_FOGENABLE, true);
		bD3DRS_RangeFogEnable = (fogGenMode == NV097_SET_FOG_GEN_MODE_V_RADIAL)? true:false;
		
		NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_RANGEFOGENABLE, bD3DRS_RangeFogEnable);
		NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_FOGTABLEMODE, fogTableMode);
		uint32_t fog_color = pg->KelvinPrimitive.SetFogColor;
		/* Kelvin fog color channels are ABGR, PGRAPH channels are ARGB */
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
	DWORD stage;

	for (stage = 0; stage < 4; stage++)
	{
		DWORD transformFlags;// = D3D__TextureState[stage]	[D3DTSS_TEXTURETRANSFORMFLAGS];
		bool bTextureTransformEnable = pg->KelvinPrimitive.SetTextureMatrixEnable[stage];
        //default texCoordIndex
		DWORD texCoordIndex = stage;
		DWORD texgen = D3DTSS_TCI_PASSTHRU;
		if (!bTextureTransformEnable) {
			transformFlags = D3DTTFF_DISABLE;
			NV2ATextureStates.Set(stage, xbox::X_D3DTSS_TEXTURETRANSFORMFLAGS, transformFlags);
		}
		else {
			// Enable the texture transform:
			//DWORD texCoord;// = D3D__TextureState[stage][D3DTSS_TEXCOORDINDEX];
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
            // whether texture coordinate with (0, 0, 0, 1) to get a '1' into 'W' because we leave Q disabled.
			// and SetTexgen[stage].s==t==r. reversed from D3DDevice_SetTextureState_TexCoordIndex()
			
			// Handle the re-mapping of a stage: xbox converts remapping before it sets Kelvin's vertex attribute and stream. so no need to remap if we compose vertex attr from kelvin. check StateUp()/StateVb()
			// NV097_SET_VERTEX_DATA_ARRAY_FORMAT and NV097_SET_VERTEX_DATA_ARRAY_OFFSET are update with slot mapping included. so we don't worry about it here.

			// compose texCoordIndex with stage mapping index and texgen, set it to defaul when we compose vertex attr from kelvin.
			// NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_WRAP0 + stage, warp);

			// todo: shall we compose the FVF pVertexShader->Dimensionality here with the inCount?
			// inCount = (pVertexShader->Dimensionality 	>> (8 * index)) & 0xff;
			// pVertexShader->Dimensionality|= incount<<  (8 * index);// index==stage here. the actual index shall be the render state WARP0~3 indexed with texture render state D3DTSS_TEXCOORDINDEX. but we can't reverse both variables with the inCount value directly.
			// so we arrume for each texture stage D3DTSS_TEXCOORDINDEX==stage, and each stage indexes WARP(stage). this is xbox default setting.
			NV2ATextureStates.Set(stage, xbox::X_D3DTSS_TEXTURETRANSFORMFLAGS, transformFlags);
		}
		texCoordIndex = (texgen & 0xFFFF0000) | stage;
		NV2ATextureStates.Set(stage, xbox::X_D3DTSS_TEXCOORDINDEX, texCoordIndex);
		//reversed codes for reference. check Otogi for LazySetTextureTransform()
	}
	return;
}

// scale a 3-component vector
void ScaleVector3(D3DVECTOR* out, CONST D3DVECTOR* v1, FLOAT scale)
{
	out->x = scale * v1->x;
	out->y = scale * v1->y;
	out->z = scale * v1->z;
}

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

// return the square of the magnitude of a 3-component vectors
FLOAT SquareMagnitude3(CONST D3DVECTOR* v)
{
	return (v->x * v->x + v->y * v->y + v->z * v->z);
}

// returns  1 / sqrt(x)
static float _0_47 = 0.47f;
static float _1_47 = 1.47f;

float JBInvSqrt(const float x)
{
	DWORD y = 0;
	float r = 0.f;

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

// normalize a 3-component vector
void NormalizeVector3(D3DVECTOR* v)
{
	FLOAT invmag = JBInvSqrt(SquareMagnitude3(v));
	assert(invmag > 0);
	v->x *= invmag;
	v->y *= invmag;
	v->z *= invmag;
}

void SetSceneAmbientAndMaterialEmission(NV2AState* d)
{
	PGRAPHState* pg = &d->pgraph;
	FLOAT ambientR, ambientG, ambientB;
	FLOAT emissiveR, emissiveG, emissiveB,alpha;

	//xbox::X_D3DMATERIAL8 Material;
	//DWORD ambient;

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

	float colorScale = 2.1f;
	emissiveR = pg->KelvinPrimitive.SetMaterialEmission[0];
	emissiveG = pg->KelvinPrimitive.SetMaterialEmission[1];
	emissiveB = pg->KelvinPrimitive.SetMaterialEmission[2];

	ambientR = pg->KelvinPrimitive.SetSceneAmbientColor[0];
	ambientG = pg->KelvinPrimitive.SetSceneAmbientColor[1];
	ambientB = pg->KelvinPrimitive.SetSceneAmbientColor[2];

	alpha = pg->KelvinPrimitive.SetMaterialAlpha;

	if ((colorMaterial & 0xC) !=0)//		NV097_SET_COLOR_MATERIAL_AMBIENT_MATERIAL bit 3:2
	{
		NV2A_SceneMateirals[0].Emissive.r= ambientR;
		NV2A_SceneMateirals[0].Emissive.g= ambientG;
		NV2A_SceneMateirals[0].Emissive.b= ambientB;
		NV2A_SceneMateirals[0].Emissive.a = 1.0f;

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

		NV2A_SceneMateirals[0].Emissive.r = 1.0f;
		NV2A_SceneMateirals[0].Emissive.g = 1.0f;
		NV2A_SceneMateirals[0].Emissive.b = 1.0f;
		NV2A_SceneMateirals[0].Emissive.a = 1.0f;

        //hack:set material ambient to 1.0, and leave all ambientR/G/B to scene ambient.
		//todo:figure out either to leave ambientR/G/b to scene ambient or material enbient is better.
		NV2A_SceneMateirals[0].Ambient.r = 1.0f;
		NV2A_SceneMateirals[0].Ambient.g = 1.0f;
		NV2A_SceneMateirals[0].Ambient.b = 1.0f;
		NV2A_SceneMateirals[0].Ambient.a = 1.0f;
	}
	else
	{
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
		NV2A_SceneMateirals[1].Emissive.r = ambientR;
		NV2A_SceneMateirals[1].Emissive.g = ambientG;
		NV2A_SceneMateirals[1].Emissive.b = ambientB;
		NV2A_SceneMateirals[1].Emissive.a = 1.0f;

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
		// value of material emissive color is don't care since the color source is from default color.
		NV2A_SceneMateirals[1].Emissive.r = 1.0f;
		NV2A_SceneMateirals[1].Emissive.g = 1.0f;
		NV2A_SceneMateirals[1].Emissive.b = 1.0f;
		NV2A_SceneMateirals[1].Emissive.a = 1.0f;

		//hack:set material ambient to 1.0, and leave all ambientR/G/B to scene ambient.
		//todo:figure out either to leave ambientR/G/b to scene ambient or material enbient is better.
		NV2A_SceneMateirals[1].Ambient.r = 1.0f;
		NV2A_SceneMateirals[1].Ambient.g = 1.0f;
		NV2A_SceneMateirals[1].Ambient.b = 1.0f;
		NV2A_SceneMateirals[1].Ambient.a = 1.0f;
	}
	else
	{
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
	float N1;//N1 could be removed since we're only look up Power with N.
};
// specular power look up table, look up power via N or N1. This table is generated by stepping Power from 0 to 100 in 0.1 and calculate N/N1.
// todo: N1 is not used in look up, we could remove N1 from 
ReverseExpluteInfo g_ReverseExpluteTable[]= 
{
//Power	,   N	    ,   N1
{ 	0.f	, 	0.f	    , 	0.f	    },//index 0f, look up power with N1f rom N1=0.0 to N1<=0.299999  // when N1<=0.6f, we could use a quick hack Power=N1*2.0f
{ 	0.1f	, 	0.1f     , 	0.05f	},
{ 	0.2f	, 	0.2f     , 	0.1f	    },
{ 	0.3f	, 	0.299999f, 	0.15f	},
{ 	0.4f	, 	0.399957f, 	0.2f	    },
{ 	0.5f	, 	0.499634f, 	0.25f	},
{ 	0.6f	, 	0.598438f, 	0.299999f},// when N1> 0.299999f and N1<=1.0ff, we could use Power= 0.2125f*N1*N1+1.8075f*N1+0.0417f
{ 	0.7f	, 	0.695531f, 	0.349991f},
{ 	0.8f	, 	0.790056f, 	0.399957f},
{ 	0.9f	, 	0.881298f, 	0.449859f},
{ 	1.f	, 	1.49459f	, 	0.499634f},
{ 	1.1f	, 	1.48496f	, 	0.549194f},
{ 	1.2f	, 	1.47616f	, 	0.598438f},
{ 	1.3f	, 	1.47184f	, 	0.647254f},
{ 	1.4f	, 	1.47325f	, 	0.695531f},
{ 	1.5f	, 	1.47457f	, 	0.743164f},
{ 	1.6f	, 	1.48284f	, 	0.790056f},
{ 	1.7f	, 	1.53794f	, 	0.836124f},
{ 	1.8f	, 	1.58988f	, 	0.881298f},
{ 	1.9f	, 	1.63902f	, 	0.925521f},//index 19f, if N1<=0.925521f, use N1 as key to look up Power.// else if N>=1.63902ff, use N as key to look up Power.
{ 	2.f	, 	1.68564f	, 	1.49459f	},
{ 	2.1f	, 	1.7312f	, 	1.48966f	},
{ 	2.2f	, 	1.77465f	, 	1.48496f	},
{ 	2.3f	, 	1.81616f	, 	1.48047f	},
{ 	2.4f	, 	1.85591f	, 	1.47616f	},
{ 	2.5f	, 	1.89403f	, 	1.47204f	},
{ 	2.6f	, 	1.92841f	, 	1.47184f	},
{ 	2.7f	, 	1.96094f	, 	1.47256f	},
{ 	2.8f	, 	1.99228f	, 	1.47325f	},
{ 	2.9f	, 	2.02253f	, 	1.47392f	},
{ 	3.f	, 	2.05175f	, 	1.47457f	},
{ 	3.1f	, 	2.08002f	, 	1.4752f	},
{ 	3.2f	, 	2.10686f	, 	1.48284f	},
{ 	3.3f	, 	2.13134f	, 	1.5108f	},
{ 	3.4f	, 	2.15509f	, 	1.53794f	},
{ 	3.5f	, 	2.17815f	, 	1.56428f	},
{ 	3.6f	, 	2.20056f	, 	1.58988f	},
{ 	3.7f	, 	2.22236f	, 	1.61478f	},
{ 	3.8f	, 	2.24357f	, 	1.63902f	},
{ 	3.9f	, 	2.26424f	, 	1.66263f	},
{ 	4.f	, 	2.28438f	, 	1.68563f	},
{ 	4.1f	, 	2.30151f	, 	1.7087f	},
{ 	4.2f	, 	2.31823f	, 	1.7312f	},
{ 	4.3f	, 	2.33456f	, 	1.75318f	},
{ 	4.4f	, 	2.35051f	, 	1.77465f	},
{ 	4.5f	, 	2.3661f	, 	1.79563f	},
{ 	4.6f	, 	2.38135f	, 	1.81616f	},
{ 	4.7f	, 	2.39627f	, 	1.83624f	},
{ 	4.8f	, 	2.41087f	, 	1.85591f	},
{ 	4.9f	, 	2.42518f	, 	1.87516f	},
{ 	5.f	, 	2.4392f	, 	1.89403f	},
{ 	5.1f	, 	2.45206f	, 	1.91167f	},
{ 	5.2f	, 	2.4641f	, 	1.92841f	},
{ 	5.3f	, 	2.47592f	, 	1.94482f	},
{ 	5.4f	, 	2.48751f	, 	1.96094f	},
{ 	5.5f	, 	2.49889f	, 	1.97675f	},
{ 	5.6f	, 	2.51007f	, 	1.99228f	},
{ 	5.7f	, 	2.52104f	, 	2.00754f	},
{ 	5.8f	, 	2.53183f	, 	2.02253f	},
{ 	5.9f	, 	2.54243f	, 	2.03727f	},
{ 	6.f	, 	2.55286f	, 	2.05175f	},
{ 	6.1f	, 	2.56311f	, 	2.066f	},
{ 	6.2f	, 	2.57319f	, 	2.08002f	},
{ 	6.3f	, 	2.58312f	, 	2.09381f	},
{ 	6.4f	, 	2.5923f	, 	2.10686f	},
{ 	6.5f	, 	2.60078f	, 	2.11919f	},
{ 	6.6f	, 	2.60913f	, 	2.13134f	},
{ 	6.7f	, 	2.61735f	, 	2.1433f	},
{ 	6.8f	, 	2.62546f	, 	2.15509f	},
{ 	6.9f	, 	2.63344f	, 	2.1667f	},
{ 	7.f	, 	2.64131f	, 	2.17815f	},
{ 	7.1f	, 	2.64906f	, 	2.18943f	},
{ 	7.2f	, 	2.65671f	, 	2.20056f	},
{ 	7.3f	, 	2.66425f	, 	2.21153f	},
{ 	7.4f	, 	2.67169f	, 	2.22236f	},
{ 	7.5f	, 	2.67903f	, 	2.23304f	},
{ 	7.6f	, 	2.68628f	, 	2.24357f	},
{ 	7.7f	, 	2.69343f	, 	2.25397f	},
{ 	7.79999f,2.70048f	, 	2.26424f	},
{ 	7.89999f,2.70745f	, 	2.27437f	},
{ 	7.99999f,2.71433f	, 	2.28438f	},
{ 	8.09999f,2.71913f	, 	2.293f	},
{ 	8.2f	, 	2.72387f	, 	2.30151f	},
{ 	8.3f	, 	2.72856f	, 	2.30992f	},
{ 	8.4f	, 	2.73319f	, 	2.31823f	},
{ 	8.5f	, 	2.73777f	, 	2.32644f	},
{ 	8.6f	, 	2.74229f	, 	2.33456f	},
{ 	8.7f	, 	2.74676f	, 	2.34258f	},
{ 	8.8f	, 	2.75118f	, 	2.35051f	},
{ 	8.9f	, 	2.75555f	, 	2.35835f	},
{ 	9.f	, 	2.75986f	, 	2.3661f	},
{ 	9.1f	, 	2.76414f	, 	2.37376f	},
{ 	9.2f	, 	2.76836f	, 	2.38135f	},
{ 	9.3f	, 	2.77254f	, 	2.38885f	},
{ 	9.4f	, 	2.77668f	, 	2.39627f	},
{ 	9.5f	, 	2.78077f	, 	2.40361f	},
{ 	9.6f	, 	2.78482f	, 	2.41087f	},
{ 	9.7f	, 	2.78882f	, 	2.41806f	},
{ 	9.8f	, 	2.79279f	, 	2.42518f	},
{ 	9.9f	, 	2.79671f	, 	2.43222f	},
{ 	10.f	, 	2.8006f	, 	2.4392f	},
{ 	10.1f, 	2.80444f	, 	2.44595f	},
{ 	10.2f, 	2.80824f	, 	2.45206f	},
{ 	10.3f, 	2.812f	, 	2.45811f	},
{ 	10.4f, 	2.81572f	, 	2.4641f	},
{ 	10.5f, 	2.81941f	, 	2.47004f	},
{ 	10.6f, 	2.82306f	, 	2.47592f	},
{ 	10.7f, 	2.82668f	, 	2.48174f	},
{ 	10.8f, 	2.83026f	, 	2.48751f	},
{ 	10.9f, 	2.83381f	, 	2.49323f	},
{ 	11.f	, 	2.83733f	, 	2.49889f	},
{ 	11.1f, 	2.84081f	, 	2.5045f	},
{ 	11.2f, 	2.84427f	, 	2.51007f	},
{ 	11.3f, 	2.84769f	, 	2.51558f	},
{ 	11.4f, 	2.85109f	, 	2.52104f	},
{ 	11.5f, 	2.85445f	, 	2.52646f	},
{ 	11.6f, 	2.85779f	, 	2.53183f	},
{ 	11.7f, 	2.86109f	, 	2.53715f	},
{ 	11.8f, 	2.86437f	, 	2.54243f	},
{ 	11.9f, 	2.86762f	, 	2.54767f	},
{ 	12.f	, 	2.87085f	, 	2.55286f	},
{ 	12.1f, 	2.87404f	, 	2.558f	},
{ 	12.2f, 	2.87722f	, 	2.56311f	},
{ 	12.3f, 	2.88036f	, 	2.56817f	},
{ 	12.4f, 	2.88348f	, 	2.57319f	},
{ 	12.5f, 	2.88657f	, 	2.57818f	},
{ 	12.6f, 	2.88964f	, 	2.58312f	},
{ 	12.7f, 	2.89269f	, 	2.58802f	},
{ 	12.8f, 	2.89557f	, 	2.5923f	},
{ 	12.9f, 	2.89843f	, 	2.59656f	},
{ 	13.f	, 	2.90127f	, 	2.60078f	},
{ 	13.1f, 	2.90409f	, 	2.60497f	},
{ 	13.2f, 	2.90689f	, 	2.60913f	},
{ 	13.3f, 	2.90966f	, 	2.61326f	},
{ 	13.4f, 	2.91242f	, 	2.61735f	},
{ 	13.5f, 	2.91515f	, 	2.62142f	},
{ 	13.6f, 	2.91786f	, 	2.62546f	},
{ 	13.7f, 	2.92056f	, 	2.62946f	},
{ 	13.8f, 	2.92323f	, 	2.63344f	},
{ 	13.9f, 	2.92589f	, 	2.63739f	},
{ 	14.f	, 	2.92852f	, 	2.64131f	},
{ 	14.1f, 	2.93114f	, 	2.6452f	},
{ 	14.2f, 	2.93374f	, 	2.64906f	},
{ 	14.3f, 	2.93632f	, 	2.6529f	},
{ 	14.4f, 	2.93888f	, 	2.65671f	},
{ 	14.5f, 	2.94143f	, 	2.6605f	},
{ 	14.6f, 	2.94395f	, 	2.66426f	},
{ 	14.7f, 	2.94646f	, 	2.66799f	},
{ 	14.8f, 	2.94896f	, 	2.6717f	},
{ 	14.9f, 	2.95143f	, 	2.67538f	},
{ 	15.f	, 	2.95389f	, 	2.67904f	},
{ 	15.1f, 	2.95634f	, 	2.68267f	},
{ 	15.2f, 	2.95876f	, 	2.68628f	},
{ 	15.3f, 	2.96117f	, 	2.68986f	},
{ 	15.4f, 	2.96357f	, 	2.69343f	},
{ 	15.5f, 	2.96595f	, 	2.69697f	},
{ 	15.6f, 	2.96831f	, 	2.70048f	},
{ 	15.7f, 	2.97066f	, 	2.70398f	},
{ 	15.8f, 	2.973f	, 	2.70745f	},
{ 	15.9f, 	2.97532f	, 	2.7109f	},
{ 	16.f	, 	2.97762f	, 	2.71433f	},
{ 	16.1f, 	2.97959f	, 	2.71674f	},
{ 	16.2f, 	2.98155f	, 	2.71913f	},
{ 	16.3f, 	2.9835f	, 	2.72151f	},
{ 	16.4f, 	2.98544f	, 	2.72387f	},
{ 	16.5f, 	2.98736f	, 	2.72622f	},
{ 	16.6f, 	2.98927f	, 	2.72856f	},
{ 	16.7f, 	2.99117f	, 	2.73088f	},
{ 	16.8f, 	2.99306f	, 	2.73319f	},
{ 	16.9f, 	2.99494f	, 	2.73549f	},
{ 	17.f	, 	2.99681f	, 	2.73777f	},
{ 	17.1f, 	2.99866f	, 	2.74003f	},
{ 	17.2f, 	3.00051f	, 	2.74229f	},
{ 	17.3f, 	3.00234f	, 	2.74453f	},
{ 	17.4f, 	3.00417f	, 	2.74676f	},
{ 	17.5f, 	3.00598f	, 	2.74897f	},
{ 	17.6f, 	3.00778f	, 	2.75118f	},
{ 	17.7f, 	3.00958f	, 	2.75337f	},
{ 	17.8f, 	3.01136f	, 	2.75555f	},
{ 	17.9f, 	3.01313f	, 	2.75771f	},
{ 	18.f	, 	3.0149f	, 	2.75987f	},
{ 	18.1f, 	3.01665f	, 	2.76201f	},
{ 	18.2f, 	3.01839f	, 	2.76414f	},
{ 	18.3f, 	3.02013f	, 	2.76626f	},
{ 	18.4f, 	3.02185f	, 	2.76836f	},
{ 	18.5f, 	3.02357f	, 	2.77046f	},
{ 	18.6f, 	3.02527f	, 	2.77254f	},
{ 	18.7f, 	3.02697f	, 	2.77462f	},
{ 	18.8f, 	3.02866f	, 	2.77668f	},
{ 	18.9f, 	3.03034f	, 	2.77873f	},
{ 	19.f	, 	3.03201f	, 	2.78077f	},
{ 	19.1f, 	3.03367f	, 	2.7828f	},
{ 	19.2f, 	3.03532f	, 	2.78482f	},
{ 	19.3f, 	3.03697f	, 	2.78683f	},
{ 	19.4f, 	3.0386f	, 	2.78882f	},
{ 	19.5f, 	3.04023f	, 	2.79081f	},
{ 	19.6f, 	3.04185f	, 	2.79279f	},
{ 	19.7f, 	3.04346f	, 	2.79476f	},
{ 	19.8f, 	3.04506f	, 	2.79671f	},
{ 	19.9f, 	3.04665f	, 	2.79866f	},
{ 	20.f	, 	3.04824f	, 	2.8006f	},
{ 	20.1f, 	3.04982f	, 	2.80253f	},
{ 	20.2f, 	3.05089f	, 	2.80444f	},// index 242f, end of arrayf, array element count 243
#if 0
{ 	20.3f	, 	3.05124f	, 	2.80635f	},
{ 	20.4f	, 	3.0516f	, 	2.80824f	},
{ 	20.5f	, 	3.05195f	, 	2.81012f	},
{ 	20.6f	, 	3.0523f	, 	2.812f	},
{ 	20.7f	, 	3.05265f	, 	2.81386f	},
{ 	20.8f	, 	3.053f	, 	2.81572f	},
{ 	20.9f	, 	3.05334f	, 	2.81757f	},
{ 	21.f	, 	3.05369f	, 	2.81941f	},
{ 	21.1f	, 	3.05403f	, 	2.82124f	},
{ 	21.2f	, 	3.05437f	, 	2.82306f	},
{ 	21.3f	, 	3.05471f	, 	2.82487f	},
{ 	21.4f	, 	3.05505f	, 	2.82668f	},
{ 	21.5f	, 	3.05538f	, 	2.82847f	},
{ 	21.6f	, 	3.05572f	, 	2.83026f	},
{ 	21.7f	, 	3.05605f	, 	2.83204f	},
{ 	21.8f	, 	3.05638f	, 	2.83381f	},
{ 	21.9f	, 	3.05671f	, 	2.83557f	},
{ 	22.f	, 	3.05704f	, 	2.83733f	},
{ 	22.1f	, 	3.05737f	, 	2.83908f	},
{ 	22.2f	, 	3.05769f	, 	2.84082f	},
{ 	22.3f	, 	3.05802f	, 	2.84255f	},
{ 	22.4f	, 	3.05834f	, 	2.84427f	},
{ 	22.5f	, 	3.05866f	, 	2.84599f	},
{ 	22.6f	, 	3.05898f	, 	2.84769f	},
{ 	22.7001f	, 	3.0593f	, 	2.8494f	},
{ 	22.8001f	, 	3.05962f	, 	2.85109f	},
{ 	22.9001f	, 	3.05993f	, 	2.85277f	},
{ 	23.0001f	, 	3.06025f	, 	2.85445f	},
{ 	23.1001f	, 	3.06056f	, 	2.85612f	},
{ 	23.2001f	, 	3.06087f	, 	2.85779f	},
{ 	23.3001f	, 	3.06118f	, 	2.85945f	},
{ 	23.4001f	, 	3.06149f	, 	2.8611f	},
{ 	23.5001f	, 	3.0618f	, 	2.86274f	},
{ 	23.6001f	, 	3.0621f	, 	2.86437f	},
{ 	23.7001f	, 	3.06241f	, 	2.866f	},
{ 	23.8001f	, 	3.06271f	, 	2.86762f	},
{ 	23.9001f	, 	3.06301f	, 	2.86924f	},
{ 	24.0001f	, 	3.06331f	, 	2.87085f	},
{ 	24.1001f	, 	3.06361f	, 	2.87245f	},
{ 	24.2001f	, 	3.06391f	, 	2.87405f	},
{ 	24.3001f	, 	3.06421f	, 	2.87563f	},
{ 	24.4001f	, 	3.0645f	, 	2.87722f	},
{ 	24.5001f	, 	3.0648f	, 	2.87879f	},
{ 	24.6001f	, 	3.06509f	, 	2.88036f	},
{ 	24.7001f	, 	3.06539f	, 	2.88192f	},
{ 	24.8001f	, 	3.06568f	, 	2.88348f	},
{ 	24.9001f	, 	3.06597f	, 	2.88503f	},
{ 	25.0001f	, 	3.06626f	, 	2.88657f	},
{ 	25.1001f	, 	3.06654f	, 	2.88811f	},
{ 	25.2001f	, 	3.06683f	, 	2.88964f	},
{ 	25.3001f	, 	3.06712f	, 	2.89117f	},
{ 	25.4001f	, 	3.06741f	, 	2.89269f	},
{ 	25.5001f	, 	3.06833f	, 	2.89413f	},
{ 	25.6001f	, 	3.06924f	, 	2.89557f	},
{ 	25.7001f	, 	3.07015f	, 	2.89701f	},
{ 	25.8001f	, 	3.07106f	, 	2.89843f	},
{ 	25.9001f	, 	3.07197f	, 	2.89986f	},
{ 	26.0001f	, 	3.07287f	, 	2.90127f	},
{ 	26.1001f	, 	3.07376f	, 	2.90269f	},
{ 	26.2001f	, 	3.07466f	, 	2.90409f	},
{ 	26.3001f	, 	3.07555f	, 	2.90549f	},
{ 	26.4001f	, 	3.07643f	, 	2.90689f	},
{ 	26.5001f	, 	3.07732f	, 	2.90828f	},
{ 	26.6001f	, 	3.0782f	, 	2.90966f	},
{ 	26.7001f	, 	3.07907f	, 	2.91104f	},
{ 	26.8001f	, 	3.07995f	, 	2.91242f	},
{ 	26.9001f	, 	3.08082f	, 	2.91379f	},
{ 	27.0001f	, 	3.08169f	, 	2.91515f	},
{ 	27.1001f	, 	3.08255f	, 	2.91651f	},
{ 	27.2001f	, 	3.08341f	, 	2.91786f	},
{ 	27.3001f	, 	3.08427f	, 	2.91921f	},
{ 	27.4001f	, 	3.08512f	, 	2.92056f	},
{ 	27.5001f	, 	3.08597f	, 	2.9219f	},
{ 	27.6001f	, 	3.08682f	, 	2.92323f	},
{ 	27.7001f	, 	3.08767f	, 	2.92456f	},
{ 	27.8001f	, 	3.08851f	, 	2.92589f	},
{ 	27.9001f	, 	3.08935f	, 	2.92721f	},
{ 	28.0001f	, 	3.09018f	, 	2.92852f	},
{ 	28.1001f	, 	3.09102f	, 	2.92983f	},
{ 	28.2001f	, 	3.09185f	, 	2.93114f	},
{ 	28.3001f	, 	3.09267f	, 	2.93244f	},
{ 	28.4001f	, 	3.0935f	, 	2.93374f	},
{ 	28.5001f	, 	3.09432f	, 	2.93503f	},
{ 	28.6001f	, 	3.09514f	, 	2.93632f	},
{ 	28.7001f	, 	3.09595f	, 	2.9376f	},
{ 	28.8001f	, 	3.09677f	, 	2.93888f	},
{ 	28.9001f	, 	3.09758f	, 	2.94016f	},
{ 	29.0001f	, 	3.09838f	, 	2.94143f	},
{ 	29.1001f	, 	3.09919f	, 	2.94269f	},
{ 	29.2001f	, 	3.09999f	, 	2.94395f	},
{ 	29.3001f	, 	3.10079f	, 	2.94521f	},
{ 	29.4001f	, 	3.10158f	, 	2.94646f	},
{ 	29.5001f	, 	3.10238f	, 	2.94771f	},
{ 	29.6001f	, 	3.10317f	, 	2.94896f	},
{ 	29.7001f	, 	3.10396f	, 	2.9502f	},
{ 	29.8001f	, 	3.10474f	, 	2.95143f	},
{ 	29.9001f	, 	3.10553f	, 	2.95266f	},
{ 	30.0001f	, 	3.10631f	, 	2.95389f	},
{ 	30.1001f	, 	3.10708f	, 	2.95512f	},
{ 	30.2001f	, 	3.10786f	, 	2.95634f	},
{ 	30.3001f	, 	3.10863f	, 	2.95755f	},
{ 	30.4001f	, 	3.1094f	, 	2.95876f	},
{ 	30.5001f	, 	3.11017f	, 	2.95997f	},
{ 	30.6001f	, 	3.11093f	, 	2.96117f	},
{ 	30.7001f	, 	3.1117f	, 	2.96237f	},
{ 	30.8001f	, 	3.11246f	, 	2.96357f	},
{ 	30.9001f	, 	3.11321f	, 	2.96476f	},
{ 	31.0001f	, 	3.11397f	, 	2.96595f	},
{ 	31.1001f	, 	3.11472f	, 	2.96713f	},
{ 	31.2001f	, 	3.11547f	, 	2.96831f	},
{ 	31.3001f	, 	3.11622f	, 	2.96949f	},
{ 	31.4001f	, 	3.11696f	, 	2.97066f	},
{ 	31.5001f	, 	3.11771f	, 	2.97183f	},
{ 	31.6001f	, 	3.11845f	, 	2.973f	},
{ 	31.7001f	, 	3.11919f	, 	2.97416f	},
{ 	31.8001f	, 	3.11992f	, 	2.97532f	},
{ 	31.9001f	, 	3.12066f	, 	2.97647f	},
{ 	32.0001f	, 	3.12139f	, 	2.97762f	},
{ 	32.1001f	, 	3.12193f	, 	2.97861f	},
{ 	32.2001f	, 	3.12246f	, 	2.97959f	},
{ 	32.3001f	, 	3.123f	, 	2.98058f	},
{ 	32.4001f	, 	3.12353f	, 	2.98155f	},
{ 	32.5001f	, 	3.12407f	, 	2.98253f	},
{ 	32.6001f	, 	3.1246f	, 	2.9835f	},
{ 	32.7001f	, 	3.12513f	, 	2.98447f	},
{ 	32.8001f	, 	3.12565f	, 	2.98544f	},
{ 	32.9001f	, 	3.12618f	, 	2.9864f	},
{ 	33.0001f	, 	3.1267f	, 	2.98736f	},
{ 	33.1001f	, 	3.12723f	, 	2.98832f	},
{ 	33.2001f	, 	3.12775f	, 	2.98927f	},
{ 	33.3001f	, 	3.12827f	, 	2.99022f	},
{ 	33.4001f	, 	3.12878f	, 	2.99117f	},
{ 	33.5001f	, 	3.1293f	, 	2.99212f	},
{ 	33.6001f	, 	3.12982f	, 	2.99306f	},
{ 	33.7001f	, 	3.13033f	, 	2.994f	},
{ 	33.8001f	, 	3.13084f	, 	2.99494f	},
{ 	33.9001f	, 	3.13135f	, 	2.99588f	},
{ 	34.0001f	, 	3.13186f	, 	2.99681f	},
{ 	34.1001f	, 	3.13237f	, 	2.99774f	},
{ 	34.2001f	, 	3.13287f	, 	2.99866f	},
{ 	34.3f	, 	3.13338f	, 	2.99959f	},
{ 	34.4f	, 	3.13388f	, 	3.00051f	},
{ 	34.5f	, 	3.13438f	, 	3.00143f	},
{ 	34.6f	, 	3.13488f	, 	3.00234f	},
{ 	34.7f	, 	3.13538f	, 	3.00326f	},
{ 	34.8f	, 	3.13588f	, 	3.00417f	},
{ 	34.9f	, 	3.13637f	, 	3.00508f	},
{ 	35.f	, 	3.13687f	, 	3.00598f	},
{ 	35.1f	, 	3.13736f	, 	3.00688f	},
{ 	35.2f	, 	3.13785f	, 	3.00778f	},
{ 	35.3f	, 	3.13834f	, 	3.00868f	},
{ 	35.4f	, 	3.13883f	, 	3.00958f	},
{ 	35.5f	, 	3.13932f	, 	3.01047f	},
{ 	35.6f	, 	3.13981f	, 	3.01136f	},
{ 	35.7f	, 	3.14029f	, 	3.01225f	},
{ 	35.8f	, 	3.14077f	, 	3.01313f	},
{ 	35.9f	, 	3.14126f	, 	3.01402f	},
{ 	36.f	, 	3.14174f	, 	3.0149f	},
{ 	36.1f	, 	3.14222f	, 	3.01577f	},
{ 	36.2f	, 	3.14269f	, 	3.01665f	},
{ 	36.3f	, 	3.14317f	, 	3.01752f	},
{ 	36.4f	, 	3.14364f	, 	3.01839f	},
{ 	36.5f	, 	3.14412f	, 	3.01926f	},
{ 	36.6f	, 	3.14459f	, 	3.02013f	},
{ 	36.7f	, 	3.14506f	, 	3.02099f	},
{ 	36.8f	, 	3.14553f	, 	3.02185f	},
{ 	36.9f	, 	3.146f	, 	3.02271f	},
{ 	37.f	, 	3.14647f	, 	3.02357f	},
{ 	37.1f	, 	3.14694f	, 	3.02442f	},
{ 	37.2f	, 	3.1474f	, 	3.02527f	},
{ 	37.3f	, 	3.14786f	, 	3.02612f	},
{ 	37.4f	, 	3.14833f	, 	3.02697f	},
{ 	37.5f	, 	3.14879f	, 	3.02782f	},
{ 	37.6f	, 	3.14925f	, 	3.02866f	},
{ 	37.7f	, 	3.14971f	, 	3.0295f	},
{ 	37.8f	, 	3.15017f	, 	3.03034f	},
{ 	37.9f	, 	3.15062f	, 	3.03117f	},
{ 	38f	, 	3.15108f	, 	3.03201f	},
{ 	38.1f	, 	3.15153f	, 	3.03284f	},
{ 	38.2f	, 	3.15198f	, 	3.03367f	},
{ 	38.3f	, 	3.15244f	, 	3.0345f	},
{ 	38.4f	, 	3.15289f	, 	3.03532f	},
{ 	38.5f	, 	3.15334f	, 	3.03614f	},
{ 	38.6f	, 	3.15378f	, 	3.03696f	},
{ 	38.7f	, 	3.15423f	, 	3.03778f	},
{ 	38.8f	, 	3.15468f	, 	3.0386f	},
{ 	38.9f	, 	3.15512f	, 	3.03941f	},
{ 	39.f	, 	3.15556f	, 	3.04023f	},
{ 	39.1f	, 	3.15601f	, 	3.04104f	},
{ 	39.2f	, 	3.15645f	, 	3.04185f	},
{ 	39.3f	, 	3.15689f	, 	3.04265f	},
{ 	39.4f	, 	3.15733f	, 	3.04346f	},
{ 	39.5f	, 	3.15777f	, 	3.04426f	},
{ 	39.6f	, 	3.1582f	, 	3.04506f	},
{ 	39.7f	, 	3.15864f	, 	3.04586f	},
{ 	39.8f	, 	3.15907f	, 	3.04665f	},
{ 	39.9f	, 	3.15951f	, 	3.04745f	},
{ 	40f	, 	3.15994f	, 	3.04824f	},
{ 	40.1f	, 	3.16037f	, 	3.04903f	},
{ 	40.2f	, 	3.1608f	, 	3.04982f	},
{ 	40.3f	, 	3.16123f	, 	3.0506f	},
{ 	40.4f	, 	3.16135f	, 	3.05089f	},
{ 	40.5f	, 	3.16139f	, 	3.05107f	},
{ 	40.6f	, 	3.16144f	, 	3.05124f	},
{ 	40.7f	, 	3.16149f	, 	3.05142f	},
{ 	40.7999f	, 	3.16154f	, 	3.0516f	},
{ 	40.8999f	, 	3.16159f	, 	3.05177f	},
{ 	40.9999f	, 	3.16163f	, 	3.05195f	},
{ 	41.0999f	, 	3.16168f	, 	3.05213f	},
{ 	41.1999f	, 	3.16173f	, 	3.0523f	},
{ 	41.2999f	, 	3.16178f	, 	3.05248f	},
{ 	41.3999f	, 	3.16182f	, 	3.05265f	},
{ 	41.4999f	, 	3.16187f	, 	3.05282f	},
{ 	41.5999f	, 	3.16192f	, 	3.053f	},
{ 	41.6999f	, 	3.16196f	, 	3.05317f	},
{ 	41.7999f	, 	3.16201f	, 	3.05334f	},
{ 	41.8999f	, 	3.16206f	, 	3.05352f	},
{ 	41.9999f	, 	3.1621f	, 	3.05369f	},
{ 	42.0999f	, 	3.16215f	, 	3.05386f	},
{ 	42.1999f	, 	3.1622f	, 	3.05403f	},
{ 	42.2999f	, 	3.16224f	, 	3.0542f	},
{ 	42.3999f	, 	3.16229f	, 	3.05437f	},
{ 	42.4999f	, 	3.16234f	, 	3.05454f	},
{ 	42.5999f	, 	3.16238f	, 	3.05471f	},
{ 	42.6999f	, 	3.16243f	, 	3.05488f	},
{ 	42.7999f	, 	3.16247f	, 	3.05505f	},
{ 	42.8999f	, 	3.16252f	, 	3.05522f	},
{ 	42.9999f	, 	3.16256f	, 	3.05538f	},
{ 	43.0999f	, 	3.16261f	, 	3.05555f	},
{ 	43.1999f	, 	3.16266f	, 	3.05572f	},
{ 	43.2999f	, 	3.1627f	, 	3.05589f	},
{ 	43.3999f	, 	3.16275f	, 	3.05605f	},
{ 	43.4999f	, 	3.16279f	, 	3.05622f	},
{ 	43.5999f	, 	3.16284f	, 	3.05638f	},
{ 	43.6999f	, 	3.16288f	, 	3.05655f	},
{ 	43.7999f	, 	3.16293f	, 	3.05671f	},
{ 	43.8999f	, 	3.16297f	, 	3.05688f	},
{ 	43.9999f	, 	3.16301f	, 	3.05704f	},
{ 	44.0999f	, 	3.16306f	, 	3.0572f	},
{ 	44.1999f	, 	3.1631f	, 	3.05737f	},
{ 	44.2999f	, 	3.16315f	, 	3.05753f	},
{ 	44.3999f	, 	3.16319f	, 	3.05769f	},
{ 	44.4999f	, 	3.16324f	, 	3.05786f	},
{ 	44.5999f	, 	3.16328f	, 	3.05802f	},
{ 	44.6999f	, 	3.16332f	, 	3.05818f	},
{ 	44.7999f	, 	3.16337f	, 	3.05834f	},
{ 	44.8999f	, 	3.16341f	, 	3.0585f	},
{ 	44.9999f	, 	3.16345f	, 	3.05866f	},
{ 	45.0999f	, 	3.1635f	, 	3.05882f	},
{ 	45.1999f	, 	3.16354f	, 	3.05898f	},
{ 	45.2999f	, 	3.16358f	, 	3.05914f	},
{ 	45.3999f	, 	3.16363f	, 	3.0593f	},
{ 	45.4999f	, 	3.16367f	, 	3.05946f	},
{ 	45.5999f	, 	3.16371f	, 	3.05962f	},
{ 	45.6999f	, 	3.16376f	, 	3.05977f	},
{ 	45.7999f	, 	3.1638f	, 	3.05993f	},
{ 	45.8999f	, 	3.16384f	, 	3.06009f	},
{ 	45.9999f	, 	3.16388f	, 	3.06025f	},
{ 	46.0999f	, 	3.16393f	, 	3.0604f	},
{ 	46.1999f	, 	3.16397f	, 	3.06056f	},
{ 	46.2999f	, 	3.16401f	, 	3.06071f	},
{ 	46.3999f	, 	3.16405f	, 	3.06087f	},
{ 	46.4999f	, 	3.1641f	, 	3.06102f	},
{ 	46.5999f	, 	3.16414f	, 	3.06118f	},
{ 	46.6999f	, 	3.16418f	, 	3.06133f	},
{ 	46.7999f	, 	3.16422f	, 	3.06149f	},
{ 	46.8999f	, 	3.16426f	, 	3.06164f	},
{ 	46.9999f	, 	3.16431f	, 	3.0618f	},
{ 	47.0999f	, 	3.16435f	, 	3.06195f	},
{ 	47.1999f	, 	3.16439f	, 	3.0621f	},
{ 	47.2999f	, 	3.16443f	, 	3.06225f	},
{ 	47.3998f	, 	3.16447f	, 	3.06241f	},
{ 	47.4998f	, 	3.16451f	, 	3.06256f	},
{ 	47.5998f	, 	3.16455f	, 	3.06271f	},
{ 	47.6998f	, 	3.16459f	, 	3.06286f	},
{ 	47.7998f	, 	3.16464f	, 	3.06301f	},
{ 	47.8998f	, 	3.16468f	, 	3.06316f	},
{ 	47.9998f	, 	3.16472f	, 	3.06331f	},
{ 	48.0998f	, 	3.16476f	, 	3.06346f	},
{ 	48.1998f	, 	3.1648f	, 	3.06361f	},
{ 	48.2998f	, 	3.16484f	, 	3.06376f	},
{ 	48.3998f	, 	3.16488f	, 	3.06391f	},
{ 	48.4998f	, 	3.16492f	, 	3.06406f	},
{ 	48.5998f	, 	3.16496f	, 	3.06421f	},
{ 	48.6998f	, 	3.165f	, 	3.06436f	},
{ 	48.7998f	, 	3.16504f	, 	3.0645f	},
{ 	48.8998f	, 	3.16508f	, 	3.06465f	},
{ 	48.9998f	, 	3.16512f	, 	3.0648f	},
{ 	49.0998f	, 	3.16516f	, 	3.06495f	},
{ 	49.1998f	, 	3.1652f	, 	3.06509f	},
{ 	49.2998f	, 	3.16524f	, 	3.06524f	},
{ 	49.3998f	, 	3.16528f	, 	3.06538f	},
{ 	49.4998f	, 	3.16532f	, 	3.06553f	},
{ 	49.5998f	, 	3.16536f	, 	3.06568f	},
{ 	49.6998f	, 	3.1654f	, 	3.06582f	},
{ 	49.7998f	, 	3.16544f	, 	3.06597f	},
{ 	49.8998f	, 	3.16548f	, 	3.06611f	},
{ 	49.9998f	, 	3.16552f	, 	3.06626f	},
{ 	50.0998f	, 	3.16555f	, 	3.0664f	},
{ 	50.1998f	, 	3.16559f	, 	3.06654f	},
{ 	50.2998f	, 	3.16563f	, 	3.06669f	},
{ 	50.3998f	, 	3.16567f	, 	3.06683f	},
{ 	50.4998f	, 	3.16571f	, 	3.06697f	},
{ 	50.5998f	, 	3.16575f	, 	3.06711f	},
{ 	50.6998f	, 	3.16579f	, 	3.06726f	},
{ 	50.7998f	, 	3.16583f	, 	3.06741f	},
{ 	50.8998f	, 	3.16611f	, 	3.06787f	},
{ 	50.9998f	, 	3.16639f	, 	3.06833f	},
{ 	51.0998f	, 	3.16667f	, 	3.06878f	},
{ 	51.1998f	, 	3.16694f	, 	3.06924f	},
{ 	51.2998f	, 	3.16722f	, 	3.0697f	},
{ 	51.3998f	, 	3.16749f	, 	3.07015f	},
{ 	51.4998f	, 	3.16777f	, 	3.07061f	},
{ 	51.5998f	, 	3.16804f	, 	3.07106f	},
{ 	51.6998f	, 	3.16831f	, 	3.07151f	},
{ 	51.7998f	, 	3.16859f	, 	3.07196f	},
{ 	51.8998f	, 	3.16886f	, 	3.07241f	},
{ 	51.9998f	, 	3.16913f	, 	3.07286f	},
{ 	52.0998f	, 	3.1694f	, 	3.07331f	},
{ 	52.1998f	, 	3.16967f	, 	3.07376f	},
{ 	52.2998f	, 	3.16994f	, 	3.07421f	},
{ 	52.3998f	, 	3.17021f	, 	3.07466f	},
{ 	52.4998f	, 	3.17048f	, 	3.0751f	},
{ 	52.5998f	, 	3.17075f	, 	3.07555f	},
{ 	52.6998f	, 	3.17102f	, 	3.07599f	},
{ 	52.7998f	, 	3.17129f	, 	3.07643f	},
{ 	52.8998f	, 	3.17156f	, 	3.07687f	},
{ 	52.9998f	, 	3.17182f	, 	3.07732f	},
{ 	53.0998f	, 	3.17209f	, 	3.07776f	},
{ 	53.1998f	, 	3.17236f	, 	3.0782f	},
{ 	53.2998f	, 	3.17262f	, 	3.07863f	},
{ 	53.3998f	, 	3.17289f	, 	3.07907f	},
{ 	53.4998f	, 	3.17315f	, 	3.07951f	},
{ 	53.5998f	, 	3.17341f	, 	3.07995f	},
{ 	53.6998f	, 	3.17368f	, 	3.08038f	},
{ 	53.7998f	, 	3.17394f	, 	3.08082f	},
{ 	53.8997f	, 	3.1742f	, 	3.08125f	},
{ 	53.9997f	, 	3.17446f	, 	3.08168f	},
{ 	54.0997f	, 	3.17473f	, 	3.08212f	},
{ 	54.1997f	, 	3.17499f	, 	3.08255f	},
{ 	54.2997f	, 	3.17525f	, 	3.08298f	},
{ 	54.3997f	, 	3.17551f	, 	3.08341f	},
{ 	54.4997f	, 	3.17577f	, 	3.08384f	},
{ 	54.5997f	, 	3.17603f	, 	3.08427f	},
{ 	54.6997f	, 	3.17628f	, 	3.08469f	},
{ 	54.7997f	, 	3.17654f	, 	3.08512f	},
{ 	54.8997f	, 	3.1768f	, 	3.08555f	},
{ 	54.9997f	, 	3.17706f	, 	3.08597f	},
{ 	55.0997f	, 	3.17731f	, 	3.0864f	},
{ 	55.1997f	, 	3.17757f	, 	3.08682f	},
{ 	55.2997f	, 	3.17783f	, 	3.08724f	},
{ 	55.3997f	, 	3.17808f	, 	3.08766f	},
{ 	55.4997f	, 	3.17834f	, 	3.08809f	},
{ 	55.5997f	, 	3.17859f	, 	3.08851f	},
{ 	55.6997f	, 	3.17884f	, 	3.08893f	},
{ 	55.7997f	, 	3.1791f	, 	3.08935f	},
{ 	55.8997f	, 	3.17935f	, 	3.08976f	},
{ 	55.9997f	, 	3.1796f	, 	3.09018f	},
{ 	56.0997f	, 	3.17985f	, 	3.0906f	},
{ 	56.1997f	, 	3.18011f	, 	3.09101f	},
{ 	56.2997f	, 	3.18036f	, 	3.09143f	},
{ 	56.3997f	, 	3.18061f	, 	3.09184f	},
{ 	56.4997f	, 	3.18086f	, 	3.09226f	},
{ 	56.5997f	, 	3.18111f	, 	3.09267f	},
{ 	56.6997f	, 	3.18136f	, 	3.09308f	},
{ 	56.7997f	, 	3.18161f	, 	3.0935f	},
{ 	56.8997f	, 	3.18186f	, 	3.09391f	},
{ 	56.9997f	, 	3.1821f	, 	3.09432f	},
{ 	57.0997f	, 	3.18235f	, 	3.09473f	},
{ 	57.1997f	, 	3.1826f	, 	3.09514f	},
{ 	57.2997f	, 	3.18285f	, 	3.09554f	},
{ 	57.3997f	, 	3.18309f	, 	3.09595f	},
{ 	57.4997f	, 	3.18334f	, 	3.09636f	},
{ 	57.5997f	, 	3.18358f	, 	3.09676f	},
{ 	57.6997f	, 	3.18383f	, 	3.09717f	},
{ 	57.7997f	, 	3.18407f	, 	3.09757f	},
{ 	57.8997f	, 	3.18432f	, 	3.09798f	},
{ 	57.9997f	, 	3.18456f	, 	3.09838f	},
{ 	58.0997f	, 	3.1848f	, 	3.09878f	},
{ 	58.1997f	, 	3.18505f	, 	3.09919f	},
{ 	58.2997f	, 	3.18529f	, 	3.09959f	},
{ 	58.3997f	, 	3.18553f	, 	3.09999f	},
{ 	58.4997f	, 	3.18577f	, 	3.10039f	},
{ 	58.5997f	, 	3.18601f	, 	3.10079f	},
{ 	58.6997f	, 	3.18626f	, 	3.10118f	},
{ 	58.7997f	, 	3.1865f	, 	3.10158f	},
{ 	58.8997f	, 	3.18674f	, 	3.10198f	},
{ 	58.9997f	, 	3.18698f	, 	3.10238f	},
{ 	59.0997f	, 	3.18722f	, 	3.10277f	},
{ 	59.1997f	, 	3.18745f	, 	3.10317f	},
{ 	59.2997f	, 	3.18769f	, 	3.10356f	},
{ 	59.3997f	, 	3.18793f	, 	3.10396f	},
{ 	59.4997f	, 	3.18817f	, 	3.10435f	},
{ 	59.5997f	, 	3.18841f	, 	3.10474f	},
{ 	59.6997f	, 	3.18864f	, 	3.10513f	},
{ 	59.7997f	, 	3.18888f	, 	3.10552f	},
{ 	59.8997f	, 	3.18912f	, 	3.10591f	},
{ 	59.9997f	, 	3.18935f	, 	3.1063f	},
{ 	60.0997f	, 	3.18959f	, 	3.10669f	},
{ 	60.1997f	, 	3.18982f	, 	3.10708f	},
{ 	60.2997f	, 	3.19006f	, 	3.10747f	},
{ 	60.3997f	, 	3.19029f	, 	3.10786f	},
{ 	60.4996f	, 	3.19052f	, 	3.10824f	},
{ 	60.5996f	, 	3.19076f	, 	3.10863f	},
{ 	60.6996f	, 	3.19099f	, 	3.10901f	},
{ 	60.7996f	, 	3.19122f	, 	3.1094f	},
{ 	60.8996f	, 	3.19145f	, 	3.10978f	},
{ 	60.9996f	, 	3.19169f	, 	3.11017f	},
{ 	61.0996f	, 	3.19192f	, 	3.11055f	},
{ 	61.1996f	, 	3.19215f	, 	3.11093f	},
{ 	61.2996f	, 	3.19238f	, 	3.11131f	},
{ 	61.3996f	, 	3.19261f	, 	3.11169f	},
{ 	61.4996f	, 	3.19284f	, 	3.11207f	},
{ 	61.5996f	, 	3.19307f	, 	3.11245f	},
{ 	61.6996f	, 	3.1933f	, 	3.11283f	},
{ 	61.7996f	, 	3.19353f	, 	3.11321f	},
{ 	61.8996f	, 	3.19376f	, 	3.11359f	},
{ 	61.9996f	, 	3.19398f	, 	3.11397f	},
{ 	62.0996f	, 	3.19421f	, 	3.11434f	},
{ 	62.1996f	, 	3.19444f	, 	3.11472f	},
{ 	62.2996f	, 	3.19467f	, 	3.11509f	},
{ 	62.3996f	, 	3.19489f	, 	3.11547f	},
{ 	62.4996f	, 	3.19512f	, 	3.11584f	},
{ 	62.5996f	, 	3.19534f	, 	3.11622f	},
{ 	62.6996f	, 	3.19557f	, 	3.11659f	},
{ 	62.7996f	, 	3.19579f	, 	3.11696f	},
{ 	62.8996f	, 	3.19602f	, 	3.11733f	},
{ 	62.9996f	, 	3.19624f	, 	3.1177f	},
{ 	63.0996f	, 	3.19647f	, 	3.11808f	},
{ 	63.1996f	, 	3.19669f	, 	3.11845f	},
{ 	63.2996f	, 	3.19692f	, 	3.11881f	},
{ 	63.3996f	, 	3.19714f	, 	3.11918f	},
{ 	63.4996f	, 	3.19736f	, 	3.11955f	},
{ 	63.5996f	, 	3.19758f	, 	3.11992f	},
{ 	63.6996f	, 	3.19781f	, 	3.12029f	},
{ 	63.7996f	, 	3.19803f	, 	3.12065f	},
{ 	63.8996f	, 	3.19825f	, 	3.12102f	},
{ 	63.9996f	, 	3.19847f	, 	3.12138f	},
{ 	64.0996f	, 	3.1981f	, 	3.12165f	},
{ 	64.1996f	, 	3.19773f	, 	3.12192f	},
{ 	64.2996f	, 	3.19735f	, 	3.12219f	},
{ 	64.3996f	, 	3.19698f	, 	3.12246f	},
{ 	64.4996f	, 	3.19661f	, 	3.12273f	},
{ 	64.5996f	, 	3.19624f	, 	3.123f	},
{ 	64.6996f	, 	3.19587f	, 	3.12326f	},
{ 	64.7996f	, 	3.1955f	, 	3.12353f	},
{ 	64.8996f	, 	3.19513f	, 	3.1238f	},
{ 	64.9996f	, 	3.19477f	, 	3.12406f	},
{ 	65.0996f	, 	3.1944f	, 	3.12433f	},
{ 	65.1996f	, 	3.19403f	, 	3.12459f	},
{ 	65.2996f	, 	3.19366f	, 	3.12486f	},
{ 	65.3996f	, 	3.1933f	, 	3.12512f	},
{ 	65.4996f	, 	3.19293f	, 	3.12539f	},
{ 	65.5996f	, 	3.19257f	, 	3.12565f	},
{ 	65.6996f	, 	3.1922f	, 	3.12591f	},
{ 	65.7996f	, 	3.19184f	, 	3.12618f	},
{ 	65.8996f	, 	3.19148f	, 	3.12644f	},
{ 	65.9996f	, 	3.19111f	, 	3.1267f	},
{ 	66.0996f	, 	3.19075f	, 	3.12696f	},
{ 	66.1996f	, 	3.19039f	, 	3.12722f	},
{ 	66.2996f	, 	3.19003f	, 	3.12748f	},
{ 	66.3996f	, 	3.18967f	, 	3.12775f	},
{ 	66.4996f	, 	3.18931f	, 	3.12801f	},
{ 	66.5996f	, 	3.18895f	, 	3.12826f	},
{ 	66.6996f	, 	3.18859f	, 	3.12852f	},
{ 	66.7996f	, 	3.18823f	, 	3.12878f	},
{ 	66.8996f	, 	3.18788f	, 	3.12904f	},
{ 	66.9995f	, 	3.18752f	, 	3.1293f	},
{ 	67.0995f	, 	3.18716f	, 	3.12956f	},
{ 	67.1995f	, 	3.18681f	, 	3.12981f	},
{ 	67.2995f	, 	3.18645f	, 	3.13007f	},
{ 	67.3995f	, 	3.1861f	, 	3.13033f	},
{ 	67.4995f	, 	3.18574f	, 	3.13058f	},
{ 	67.5995f	, 	3.18539f	, 	3.13084f	},
{ 	67.6995f	, 	3.18503f	, 	3.1311f	},
{ 	67.7995f	, 	3.18468f	, 	3.13135f	},
{ 	67.8995f	, 	3.18433f	, 	3.1316f	},
{ 	67.9995f	, 	3.18398f	, 	3.13186f	},
{ 	68.0995f	, 	3.18363f	, 	3.13211f	},
{ 	68.1995f	, 	3.18328f	, 	3.13237f	},
{ 	68.2995f	, 	3.18293f	, 	3.13262f	},
{ 	68.3995f	, 	3.18258f	, 	3.13287f	},
{ 	68.4995f	, 	3.18223f	, 	3.13312f	},
{ 	68.5995f	, 	3.18188f	, 	3.13338f	},
{ 	68.6995f	, 	3.18153f	, 	3.13363f	},
{ 	68.7995f	, 	3.18118f	, 	3.13388f	},
{ 	68.8995f	, 	3.18083f	, 	3.13413f	},
{ 	68.9995f	, 	3.18049f	, 	3.13438f	},
{ 	69.0995f	, 	3.18014f	, 	3.13463f	},
{ 	69.1995f	, 	3.1798f	, 	3.13488f	},
{ 	69.2995f	, 	3.17945f	, 	3.13513f	},
{ 	69.3995f	, 	3.17911f	, 	3.13538f	},
{ 	69.4995f	, 	3.17876f	, 	3.13563f	},
{ 	69.5995f	, 	3.17842f	, 	3.13588f	},
{ 	69.6995f	, 	3.17807f	, 	3.13613f	},
{ 	69.7995f	, 	3.17773f	, 	3.13637f	},
{ 	69.8995f	, 	3.17739f	, 	3.13662f	},
{ 	69.9995f	, 	3.17705f	, 	3.13687f	},
{ 	70.0995f	, 	3.17671f	, 	3.13711f	},
{ 	70.1995f	, 	3.17637f	, 	3.13736f	},
{ 	70.2995f	, 	3.17602f	, 	3.13761f	},
{ 	70.3995f	, 	3.17568f	, 	3.13785f	},
{ 	70.4995f	, 	3.17535f	, 	3.1381f	},
{ 	70.5995f	, 	3.17501f	, 	3.13834f	},
{ 	70.6995f	, 	3.17467f	, 	3.13859f	},
{ 	70.7995f	, 	3.17433f	, 	3.13883f	},
{ 	70.8995f	, 	3.17399f	, 	3.13907f	},
{ 	70.9995f	, 	3.17366f	, 	3.13932f	},
{ 	71.0995f	, 	3.17332f	, 	3.13956f	},
{ 	71.1995f	, 	3.17298f	, 	3.1398f	},
{ 	71.2995f	, 	3.17265f	, 	3.14005f	},
{ 	71.3995f	, 	3.17231f	, 	3.14029f	},
{ 	71.4995f	, 	3.17198f	, 	3.14053f	},
{ 	71.5995f	, 	3.17164f	, 	3.14077f	},
{ 	71.6995f	, 	3.17131f	, 	3.14101f	},
{ 	71.7995f	, 	3.17098f	, 	3.14125f	},
{ 	71.8995f	, 	3.17064f	, 	3.14149f	},
{ 	71.9995f	, 	3.17031f	, 	3.14173f	},
{ 	72.0995f	, 	3.16998f	, 	3.14197f	},
{ 	72.1995f	, 	3.16965f	, 	3.14221f	},
{ 	72.2995f	, 	3.16932f	, 	3.14245f	},
{ 	72.3995f	, 	3.16899f	, 	3.14269f	},
{ 	72.4995f	, 	3.16866f	, 	3.14293f	},
{ 	72.5995f	, 	3.16833f	, 	3.14317f	},
{ 	72.6995f	, 	3.168f	, 	3.14341f	},
{ 	72.7995f	, 	3.16767f	, 	3.14364f	},
{ 	72.8995f	, 	3.16734f	, 	3.14388f	},
{ 	72.9995f	, 	3.16701f	, 	3.14412f	},
{ 	73.0995f	, 	3.16669f	, 	3.14435f	},
{ 	73.1995f	, 	3.16636f	, 	3.14459f	},
{ 	73.2995f	, 	3.16603f	, 	3.14483f	},
{ 	73.3995f	, 	3.16571f	, 	3.14506f	},
{ 	73.4995f	, 	3.16538f	, 	3.1453f	},
{ 	73.5994f	, 	3.16506f	, 	3.14553f	},
{ 	73.6994f	, 	3.16473f	, 	3.14577f	},
{ 	73.7994f	, 	3.16441f	, 	3.146f	},
{ 	73.8994f	, 	3.16409f	, 	3.14623f	},
{ 	73.9994f	, 	3.16376f	, 	3.14647f	},
{ 	74.0994f	, 	3.16344f	, 	3.1467f	},
{ 	74.1994f	, 	3.16312f	, 	3.14693f	},
{ 	74.2994f	, 	3.16279f	, 	3.14717f	},
{ 	74.3994f	, 	3.16247f	, 	3.1474f	},
{ 	74.4994f	, 	3.16215f	, 	3.14763f	},
{ 	74.5994f	, 	3.16183f	, 	3.14786f	},
{ 	74.6994f	, 	3.16151f	, 	3.1481f	},
{ 	74.7994f	, 	3.16119f	, 	3.14833f	},
{ 	74.8994f	, 	3.16087f	, 	3.14856f	},
{ 	74.9994f	, 	3.16055f	, 	3.14879f	},
{ 	75.0994f	, 	3.16023f	, 	3.14902f	},
{ 	75.1994f	, 	3.15992f	, 	3.14925f	},
{ 	75.2994f	, 	3.1596f	, 	3.14948f	},
{ 	75.3994f	, 	3.15928f	, 	3.14971f	},
{ 	75.4994f	, 	3.15896f	, 	3.14994f	},
{ 	75.5994f	, 	3.15865f	, 	3.15016f	},
{ 	75.6994f	, 	3.15833f	, 	3.15039f	},
{ 	75.7994f	, 	3.15802f	, 	3.15062f	},
{ 	75.8994f	, 	3.1577f	, 	3.15085f	},
{ 	75.9994f	, 	3.15739f	, 	3.15108f	},
{ 	76.0994f	, 	3.15707f	, 	3.1513f	},
{ 	76.1994f	, 	3.15676f	, 	3.15153f	},
{ 	76.2994f	, 	3.15644f	, 	3.15176f	},
{ 	76.3994f	, 	3.15613f	, 	3.15198f	},
{ 	76.4994f	, 	3.15582f	, 	3.15221f	},
{ 	76.5994f	, 	3.15551f	, 	3.15243f	},
{ 	76.6994f	, 	3.15519f	, 	3.15266f	},
{ 	76.7994f	, 	3.15488f	, 	3.15288f	},
{ 	76.8994f	, 	3.15457f	, 	3.15311f	},
{ 	76.9994f	, 	3.15426f	, 	3.15333f	},
{ 	77.0994f	, 	3.15395f	, 	3.15356f	},
{ 	77.1994f	, 	3.15364f	, 	3.15378f	},
{ 	77.2994f	, 	3.15333f	, 	3.15401f	},
{ 	77.3994f	, 	3.15302f	, 	3.15423f	},
{ 	77.4994f	, 	3.15271f	, 	3.15445f	},
{ 	77.5994f	, 	3.15241f	, 	3.15468f	},
{ 	77.6994f	, 	3.1521f	, 	3.1549f	},
{ 	77.7994f	, 	3.15179f	, 	3.15512f	},
{ 	77.8994f	, 	3.15148f	, 	3.15534f	},
{ 	77.9994f	, 	3.15118f	, 	3.15556f	},
{ 	78.0994f	, 	3.15087f	, 	3.15578f	},
{ 	78.1994f	, 	3.15056f	, 	3.15601f	},
{ 	78.2994f	, 	3.15026f	, 	3.15623f	},
{ 	78.3994f	, 	3.14995f	, 	3.15645f	},
{ 	78.4994f	, 	3.14965f	, 	3.15667f	},
{ 	78.5994f	, 	3.14934f	, 	3.15689f	},
{ 	78.6994f	, 	3.14904f	, 	3.15711f	},
{ 	78.7994f	, 	3.14874f	, 	3.15733f	},
{ 	78.8994f	, 	3.14843f	, 	3.15755f	},
{ 	78.9994f	, 	3.14813f	, 	3.15776f	},
{ 	79.0994f	, 	3.14783f	, 	3.15798f	},
{ 	79.1994f	, 	3.14753f	, 	3.1582f	},
{ 	79.2994f	, 	3.14722f	, 	3.15842f	},
{ 	79.3994f	, 	3.14692f	, 	3.15864f	},
{ 	79.4994f	, 	3.14662f	, 	3.15885f	},
{ 	79.5994f	, 	3.14632f	, 	3.15907f	},
{ 	79.6994f	, 	3.14602f	, 	3.15929f	},
{ 	79.7994f	, 	3.14572f	, 	3.15951f	},
{ 	79.8994f	, 	3.14542f	, 	3.15972f	},
{ 	79.9994f	, 	3.14512f	, 	3.15994f	},
{ 	80.0993f	, 	3.14482f	, 	3.16015f	},
{ 	80.1993f	, 	3.14453f	, 	3.16037f	},
{ 	80.2993f	, 	3.14423f	, 	3.16058f	},
{ 	80.3993f	, 	3.14393f	, 	3.1608f	},
{ 	80.4993f	, 	3.14363f	, 	3.16101f	},
{ 	80.5993f	, 	3.14334f	, 	3.16123f	},
{ 	80.6993f	, 	3.14338f	, 	3.16132f	},
{ 	80.7993f	, 	3.1436f	, 	3.16134f	},
{ 	80.8993f	, 	3.14383f	, 	3.16137f	},
{ 	80.9993f	, 	3.14405f	, 	3.16139f	},
{ 	81.0993f	, 	3.14428f	, 	3.16142f	},
{ 	81.1993f	, 	3.14451f	, 	3.16144f	},
{ 	81.2993f	, 	3.14473f	, 	3.16147f	},
{ 	81.3993f	, 	3.14495f	, 	3.16149f	},
{ 	81.4993f	, 	3.14518f	, 	3.16151f	},
{ 	81.5993f	, 	3.1454f	, 	3.16154f	},
{ 	81.6993f	, 	3.14563f	, 	3.16156f	},
{ 	81.7993f	, 	3.14585f	, 	3.16159f	},
{ 	81.8993f	, 	3.14607f	, 	3.16161f	},
{ 	81.9993f	, 	3.1463f	, 	3.16163f	},
{ 	82.0993f	, 	3.14652f	, 	3.16166f	},
{ 	82.1993f	, 	3.14674f	, 	3.16168f	},
{ 	82.2993f	, 	3.14696f	, 	3.1617f	},
{ 	82.3993f	, 	3.14719f	, 	3.16173f	},
{ 	82.4993f	, 	3.14741f	, 	3.16175f	},
{ 	82.5993f	, 	3.14763f	, 	3.16178f	},
{ 	82.6993f	, 	3.14785f	, 	3.1618f	},
{ 	82.7993f	, 	3.14807f	, 	3.16182f	},
{ 	82.8993f	, 	3.14829f	, 	3.16185f	},
{ 	82.9993f	, 	3.14851f	, 	3.16187f	},
{ 	83.0993f	, 	3.14873f	, 	3.16189f	},
{ 	83.1993f	, 	3.14895f	, 	3.16192f	},
{ 	83.2993f	, 	3.14917f	, 	3.16194f	},
{ 	83.3993f	, 	3.14939f	, 	3.16196f	},
{ 	83.4993f	, 	3.14961f	, 	3.16199f	},
{ 	83.5993f	, 	3.14983f	, 	3.16201f	},
{ 	83.6993f	, 	3.15005f	, 	3.16203f	},
{ 	83.7993f	, 	3.15026f	, 	3.16206f	},
{ 	83.8993f	, 	3.15048f	, 	3.16208f	},
{ 	83.9993f	, 	3.1507f	, 	3.1621f	},
{ 	84.0993f	, 	3.15092f	, 	3.16213f	},
{ 	84.1993f	, 	3.15113f	, 	3.16215f	},
{ 	84.2993f	, 	3.15135f	, 	3.16217f	},
{ 	84.3993f	, 	3.15157f	, 	3.1622f	},
{ 	84.4993f	, 	3.15178f	, 	3.16222f	},
{ 	84.5993f	, 	3.152f	, 	3.16224f	},
{ 	84.6993f	, 	3.15222f	, 	3.16227f	},
{ 	84.7993f	, 	3.15243f	, 	3.16229f	},
{ 	84.8993f	, 	3.15265f	, 	3.16231f	},
{ 	84.9993f	, 	3.15286f	, 	3.16234f	},
{ 	85.0993f	, 	3.15308f	, 	3.16236f	},
{ 	85.1993f	, 	3.15329f	, 	3.16238f	},
{ 	85.2993f	, 	3.1535f	, 	3.16241f	},
{ 	85.3993f	, 	3.15372f	, 	3.16243f	},
{ 	85.4993f	, 	3.15393f	, 	3.16245f	},
{ 	85.5993f	, 	3.15415f	, 	3.16247f	},
{ 	85.6993f	, 	3.15436f	, 	3.1625f	},
{ 	85.7993f	, 	3.15457f	, 	3.16252f	},
{ 	85.8993f	, 	3.15479f	, 	3.16254f	},
{ 	85.9993f	, 	3.155f	, 	3.16256f	},
{ 	86.0993f	, 	3.15521f	, 	3.16259f	},
{ 	86.1993f	, 	3.15542f	, 	3.16261f	},
{ 	86.2993f	, 	3.15563f	, 	3.16263f	},
{ 	86.3993f	, 	3.15585f	, 	3.16266f	},
{ 	86.4993f	, 	3.15606f	, 	3.16268f	},
{ 	86.5993f	, 	3.15627f	, 	3.1627f	},
{ 	86.6992f	, 	3.15648f	, 	3.16272f	},
{ 	86.7992f	, 	3.15669f	, 	3.16275f	},
{ 	86.8992f	, 	3.1569f	, 	3.16277f	},
{ 	86.9992f	, 	3.15711f	, 	3.16279f	},
{ 	87.0992f	, 	3.15732f	, 	3.16281f	},
{ 	87.1992f	, 	3.15753f	, 	3.16284f	},
{ 	87.2992f	, 	3.15774f	, 	3.16286f	},
{ 	87.3992f	, 	3.15795f	, 	3.16288f	},
{ 	87.4992f	, 	3.15816f	, 	3.1629f	},
{ 	87.5992f	, 	3.15837f	, 	3.16293f	},
{ 	87.6992f	, 	3.15857f	, 	3.16295f	},
{ 	87.7992f	, 	3.15878f	, 	3.16297f	},
{ 	87.8992f	, 	3.15899f	, 	3.16299f	},
{ 	87.9992f	, 	3.1592f	, 	3.16301f	},
{ 	88.0992f	, 	3.15941f	, 	3.16304f	},
{ 	88.1992f	, 	3.15961f	, 	3.16306f	},
{ 	88.2992f	, 	3.15982f	, 	3.16308f	},
{ 	88.3992f	, 	3.16003f	, 	3.1631f	},
{ 	88.4992f	, 	3.16023f	, 	3.16313f	},
{ 	88.5992f	, 	3.16044f	, 	3.16315f	},
{ 	88.6992f	, 	3.16065f	, 	3.16317f	},
{ 	88.7992f	, 	3.16085f	, 	3.16319f	},
{ 	88.8992f	, 	3.16106f	, 	3.16321f	},
{ 	88.9992f	, 	3.16126f	, 	3.16324f	},
{ 	89.0992f	, 	3.16147f	, 	3.16326f	},
{ 	89.1992f	, 	3.16167f	, 	3.16328f	},
{ 	89.2992f	, 	3.16188f	, 	3.1633f	},
{ 	89.3992f	, 	3.16208f	, 	3.16332f	},
{ 	89.4992f	, 	3.16229f	, 	3.16335f	},
{ 	89.5992f	, 	3.16249f	, 	3.16337f	},
{ 	89.6992f	, 	3.16269f	, 	3.16339f	},
{ 	89.7992f	, 	3.1629f	, 	3.16341f	},
{ 	89.8992f	, 	3.1631f	, 	3.16343f	},
{ 	89.9992f	, 	3.1633f	, 	3.16345f	},
{ 	90.0992f	, 	3.16351f	, 	3.16348f	},
{ 	90.1992f	, 	3.16371f	, 	3.1635f	},
{ 	90.2992f	, 	3.16391f	, 	3.16352f	},
{ 	90.3992f	, 	3.16411f	, 	3.16354f	},
{ 	90.4992f	, 	3.16432f	, 	3.16356f	},
{ 	90.5992f	, 	3.16452f	, 	3.16358f	},
{ 	90.6992f	, 	3.16472f	, 	3.16361f	},
{ 	90.7992f	, 	3.16492f	, 	3.16363f	},
{ 	90.8992f	, 	3.16512f	, 	3.16365f	},
{ 	90.9992f	, 	3.16532f	, 	3.16367f	},
{ 	91.0992f	, 	3.16552f	, 	3.16369f	},
{ 	91.1992f	, 	3.16572f	, 	3.16371f	},
{ 	91.2992f	, 	3.16592f	, 	3.16373f	},
{ 	91.3992f	, 	3.16612f	, 	3.16376f	},
{ 	91.4992f	, 	3.16632f	, 	3.16378f	},
{ 	91.5992f	, 	3.16652f	, 	3.1638f	},
{ 	91.6992f	, 	3.16672f	, 	3.16382f	},
{ 	91.7992f	, 	3.16692f	, 	3.16384f	},
{ 	91.8992f	, 	3.16712f	, 	3.16386f	},
{ 	91.9992f	, 	3.16732f	, 	3.16388f	},
{ 	92.0992f	, 	3.16752f	, 	3.16391f	},
{ 	92.1992f	, 	3.16772f	, 	3.16393f	},
{ 	92.2992f	, 	3.16791f	, 	3.16395f	},
{ 	92.3992f	, 	3.16811f	, 	3.16397f	},
{ 	92.4992f	, 	3.16831f	, 	3.16399f	},
{ 	92.5992f	, 	3.16851f	, 	3.16401f	},
{ 	92.6992f	, 	3.1687f	, 	3.16403f	},
{ 	92.7992f	, 	3.1689f	, 	3.16405f	},
{ 	92.8992f	, 	3.1691f	, 	3.16407f	},
{ 	92.9992f	, 	3.16929f	, 	3.1641f	},
{ 	93.0992f	, 	3.16949f	, 	3.16412f	},
{ 	93.1992f	, 	3.16969f	, 	3.16414f	},
{ 	93.2991f	, 	3.16988f	, 	3.16416f	},
{ 	93.3991f	, 	3.17008f	, 	3.16418f	},
{ 	93.4991f	, 	3.17027f	, 	3.1642f	},
{ 	93.5991f	, 	3.17047f	, 	3.16422f	},
{ 	93.6991f	, 	3.17066f	, 	3.16424f	},
{ 	93.7991f	, 	3.17086f	, 	3.16426f	},
{ 	93.8991f	, 	3.17105f	, 	3.16428f	},
{ 	93.9991f	, 	3.17125f	, 	3.16431f	},
{ 	94.0991f	, 	3.17144f	, 	3.16433f	},
{ 	94.1991f	, 	3.17164f	, 	3.16435f	},
{ 	94.2991f	, 	3.17183f	, 	3.16437f	},
{ 	94.3991f	, 	3.17202f	, 	3.16439f	},
{ 	94.4991f	, 	3.17222f	, 	3.16441f	},
{ 	94.5991f	, 	3.17241f	, 	3.16443f	},
{ 	94.6991f	, 	3.1726f	, 	3.16445f	},
{ 	94.7991f	, 	3.1728f	, 	3.16447f	},
{ 	94.8991f	, 	3.17299f	, 	3.16449f	},
{ 	94.9991f	, 	3.17318f	, 	3.16451f	},
{ 	95.0991f	, 	3.17337f	, 	3.16453f	},
{ 	95.1991f	, 	3.17357f	, 	3.16455f	},
{ 	95.2991f	, 	3.17376f	, 	3.16457f	},
{ 	95.3991f	, 	3.17395f	, 	3.16459f	},
{ 	95.4991f	, 	3.17414f	, 	3.16461f	},
{ 	95.5991f	, 	3.17433f	, 	3.16464f	},
{ 	95.6991f	, 	3.17452f	, 	3.16466f	},
{ 	95.7991f	, 	3.17471f	, 	3.16468f	},
{ 	95.8991f	, 	3.1749f	, 	3.1647f	},
{ 	95.9991f	, 	3.17509f	, 	3.16472f	},
{ 	96.0991f	, 	3.17528f	, 	3.16474f	},
{ 	96.1991f	, 	3.17547f	, 	3.16476f	},
{ 	96.2991f	, 	3.17566f	, 	3.16478f	},
{ 	96.3991f	, 	3.17585f	, 	3.1648f	},
{ 	96.4991f	, 	3.17604f	, 	3.16482f	},
{ 	96.5991f	, 	3.17623f	, 	3.16484f	},
{ 	96.6991f	, 	3.17642f	, 	3.16486f	},
{ 	96.7991f	, 	3.17661f	, 	3.16488f	},
{ 	96.8991f	, 	3.1768f	, 	3.1649f	},
{ 	96.9991f	, 	3.17699f	, 	3.16492f	},
{ 	97.0991f	, 	3.17718f	, 	3.16494f	},
{ 	97.1991f	, 	3.17736f	, 	3.16496f	},
{ 	97.2991f	, 	3.17755f	, 	3.16498f	},
{ 	97.3991f	, 	3.17774f	, 	3.165f	},
{ 	97.4991f	, 	3.17793f	, 	3.16502f	},
{ 	97.5991f	, 	3.17811f	, 	3.16504f	},
{ 	97.6991f	, 	3.1783f	, 	3.16506f	},
{ 	97.7991f	, 	3.17849f	, 	3.16508f	},
{ 	97.8991f	, 	3.17868f	, 	3.1651f	},
{ 	97.9991f	, 	3.17886f	, 	3.16512f	},
{ 	98.0991f	, 	3.17905f	, 	3.16514f	},
{ 	98.1991f	, 	3.17923f	, 	3.16516f	},
{ 	98.2991f	, 	3.17942f	, 	3.16518f	},
{ 	98.3991f	, 	3.17961f	, 	3.1652f	},
{ 	98.4991f	, 	3.17979f	, 	3.16522f	},
{ 	98.5991f	, 	3.17998f	, 	3.16524f	},
{ 	98.6991f	, 	3.18016f	, 	3.16526f	},
{ 	98.7991f	, 	3.18035f	, 	3.16528f	},
{ 	98.8991f	, 	3.18053f	, 	3.1653f	},
{ 	98.9991f	, 	3.18072f	, 	3.16532f	},
{ 	99.0991f	, 	3.1809f	, 	3.16534f	},
{ 	99.1991f	, 	3.18109f	, 	3.16536f	},
{ 	99.2991f	, 	3.18127f	, 	3.16538f	},
{ 	99.3991f	, 	3.18145f	, 	3.1654f	},
{ 	99.4991f	, 	3.18164f	, 	3.16542f	},
{ 	99.5991f	, 	3.18182f	, 	3.16544f	},
{ 	99.6991f	, 	3.182f	, 	3.16546f	},
{ 	99.799f	, 	3.18219f	, 	3.16548f	},
{ 	99.899f	, 	3.18237f	, 	3.1655f	},
{ 	99.999f	, 	3.18255f	, 	3.16552f	},
{ 	100.099f	, 	3.18274f	, 	3.16553f	},
#endif
};
// todo: this function is not tested yet. need test sample.
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
	NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_LIGHTING, pg->KelvinPrimitive.SetLightingEnable);
	NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_TWOSIDEDLIGHTING, pg->KelvinPrimitive.SetTwoSidedLightEn);
	//pg->KelvinPrimitive.SetLight[8].{AmbientColor[3],DiffuseColor[3],SpecularColor[3],LocalRange,InfiniteHalfVector[3],InfiniteDirection[3],SpotFalloff[3],SpotDirection[4],LocalPosition[3],LocalAttenuation[3],Rev_1074[3]
	DWORD control = 0;
	DWORD colorMaterial = 0;
	DWORD lightEnableMask = 0;
	bool bLightintEnable = pg->KelvinPrimitive.SetLightingEnable != false;
	//xbox sets D3DRS_LightEnable to false when vertex shader is not fixed mode.
	extern VertexShaderMode g_NV2A_VertexShaderMode;//tmp glue
	if (g_NV2A_VertexShaderMode != VertexShaderMode::FixedFunction)
		bLightintEnable = true;
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
		//calls host api directly since in CxbxrImpl_SetMaterial() it updates the fixed mode vertex shader states directly which we have to distinguish from xbox api.
		HRESULT hRet = g_pD3DDevice->SetMaterial(&NV2A_SceneMateirals[0]);
		lightEnableMask = pg->KelvinPrimitive.SetLightEnableMask;//Push1(pPush, NV097_SET_LIGHT_ENABLE_MASK, enableMask);      // 0x3bc;
		//set lights
		int lightNum;
		//D3DVECTOR pos;
		//D3DVECTOR dir;
		//D3DVECTOR hv;
		D3DVECTOR EyeDirection = { 0.0,0.0,-1.0 };
		D3DVECTOR NV2ALightdir = {};
		D3DVECTOR tmpDir;
		D3DVECTOR NV2ALightPosition = {};
		D3DVECTOR tmpPosition;
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
				float phi2 = -1.0f*W / Scale;
				float theta2 = phi2 + 1.0f / Scale;
				NV2A_Light8[lightNum].Theta = 2.0f*acos(theta2);
				NV2A_Light8[lightNum].Phi = 2.0f*acos(phi2);
				bEnable = true;
				break;
			}
			// Fixme!!! we need D3D__RenderState[D3DRS_AMBIENT] and materials set with SetMaterial()/SetBackMaterial() to reverse the light colors.
			// hack, we use colors in kelvin directly since these colors are composed with D3DRS_AMBIENT/SetMaterial/SetBackMaterial/ and the color of each light.
			// here we use a constant color scale to scale up light color. scale comes from the actual result of HighDynamicRange sample.
			float colorScale = 2.1f;
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
			hRet = g_pD3DDevice->LightEnable(lightNum, bEnable);
		}
	}
	return;
}
extern std::map<UINT64, xbox::X_D3DSurface> g_RenderTargetCache;
void D3D_draw_state_update(NV2AState* d)
{
	PGRAPHState* pg = &d->pgraph;

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
	// CxbxUpdateNativeD3DResources() calls CxbxUpdateHostViewport() to update host viewpot, we call pgraph_SetViewport(d) to convert NV2A viewport content to xbox vieport and then call CxbxrImpl_SetViewport() there.
	// only update viewport if viewport got dirty.
	if (NV2A_viewport_dirty == true) {
		// this only compose viewport from pgraph content 
		pgraph_ComposeViewport(d);
		// todo: actually setviewport with the composed viewport, currently we don't set the host viewport via pgraph content, yet. the SetViewport() is currently HLEed and not processed in pushbuffer.
		// D3DDevice_SetViewport() was patched and HLEed, here we only implement it when we're in RunPushbuffer().
		xbox::X_D3DVIEWPORT8 Viewport;
		CxbxrGetViewport(Viewport);
		// no need to call SetViewport here.in CxbxUpdateNativeD3DResources(), CxbxUpdateHostViewport() will pickup our viewport.
		// CxbxrImpl_SetViewport(&Viewport);
		NV2A_viewport_dirty = false;
	}

	// Note, that g_Xbox_VertexShaderMode should be left untouched,
	// because except for the declaration override, the Xbox shader (either FVF
	// or a program, or even passthrough shaders) should still be in effect!

	// setup vertes format, and vertex buffer from here.
	// Derive vertex attribute layout, using an intermediate array
	struct { int slot_index; uint32_t offset; uint32_t stride; uint32_t size_and_type; } SortedAttributes[X_VSH_MAX_ATTRIBUTES] = {};
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
		// check each stream if it's within the data range of dirty render targets in g_RenderTargetCache
		for (unsigned int streamIndex = 0; streamIndex < NV2A_StreamCount; streamIndex++) {
			DWORD streamData= NV2A_StreamSource[streamIndex].VertexBuffer->Data;
			
			for (auto it = g_RenderTargetCache.begin(); it != g_RenderTargetCache.end(); it++) {
                //calculate xbox surface data size.
				// todo: calculate and cache the xbox data size in CxbxrImpl_SetRenderTarget to save overheads here.
				//process host surface data transfer to xbox data with data conversion.
				xbox::X_D3DSurface* pXboxSurface = &it->second;
				//UINT XBWidth, XBHeight, XBDepth, XBRowPitch, XBSlicePitch;
				extern void CxbxGetPixelContainerMeasures
				(
					xbox::X_D3DPixelContainer* pPixelContainer,
					// TODO : Add X_D3DCUBEMAP_FACES argument
					DWORD dwMipMapLevel, // unused - TODO : Use
					UINT* pWidth,
					UINT* pHeight,
					UINT* pDepth,
					UINT* pRowPitch,
					// Slice pitch (does not include mipmaps!)
					UINT* pSlicePitch
				);
				//CxbxGetPixelContainerMeasures(pXboxSurface, 0, &XBWidth, &XBHeight, &XBDepth, &XBRowPitch, &XBSlicePitch);
				extern void LoadSurfaceDataFromHost(xbox::X_D3DSurface* pXboxSurface);
				//if (streamData >= pXboxSurface->Data && streamData < (pXboxSurface->Data + XBSlicePitch)) {
				// todo: this is a simplified version of data over lap checking. it's quicker, but might be missing certain edge cases.
				if (streamData == pXboxSurface->Data ) {
					//transfer host data to xbox if the stream data over lapped with dirty render target surface data range.
					LoadSurfaceDataFromHost(pXboxSurface);
					//delete the render target from dirty render target surface cache since we update the xbox data.
					g_RenderTargetCache.erase(it);
					break;
				}
			}
		}
		//check for duplicated texcoord slots, if a texcoord slot is duplicated with other texcoord slot prior to it, set texcoord index of current slot to the prior duplicated slot and disable current slot.
		for (int currentSlot = xbox::X_D3DVSDE_TEXCOORD0 + 1, currentStage = 1; currentStage < 4; currentSlot++, currentStage++) {
			// skip to next loop if current stage texture is not set.
			if (g_pNV2A_SetTexture[currentStage] == nullptr)
				break;
			//only check for duplications when current slot is not unused
			//if (g_NV2AVertexAttributeFormat.Slots[current].Format!= xbox::X_D3DVSDT_NONE)
				//only check texcoord slots prior to current slot, always set texcoord index of later texture stages to prior texture stages.
			for (int priorSlot = xbox::X_D3DVSDE_TEXCOORD0, priorStage = 0; priorSlot < currentSlot; priorSlot++, priorStage++) {
				// skip to next loop if prior stage texture is not set.
				if (g_pNV2A_SetTexture[priorStage] == nullptr)
					break;
				// compare stream index/slot offset to see if two slots uses the same texcoord.
				if (g_NV2AVertexAttributeFormat.Slots[currentSlot].StreamIndex == g_NV2AVertexAttributeFormat.Slots[priorSlot].StreamIndex
					&& g_NV2AVertexAttributeFormat.Slots[currentSlot].Offset == g_NV2AVertexAttributeFormat.Slots[priorSlot].Offset) {

					//retrive texgen from NV2ATextureStates
					DWORD texcoordIndex = NV2ATextureStates.Get(currentSlot - xbox::X_D3DVSDE_TEXCOORD0, xbox::X_D3DTSS_TEXCOORDINDEX);
					DWORD texgen = texcoordIndex & 0xFFFF0000;
					//update texcoordIndex
					texcoordIndex = texgen | ((priorStage) & 0x0000FFFF);
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
	}
	// todo: call CxbxrImpl_SetVertexShader() here. There are certain varialbes needs to be initialized, mostly setup unexisting vertex attribute defaults.
	extern bool g_NV2AVertexShader_dirty; // tmp glue
	extern void pgraph_SetVertexShader(NV2AState* d);// tmp glue, XBVertexShader.cpp
	if (g_NV2AVertexShader_dirty)
		pgraph_SetVertexShader(d);
	// update other D3D states
	CxbxUpdateNativeD3DResources();
	LOG_INCOMPLETE(); // TODO : Read state from pgraph, convert to D3D
}

void D3D_draw_clear(NV2AState *d)
{
	PGRAPHState *pg = &d->pgraph;
	xbox::dword_xt Count;
	D3DRECT *pRects;
	D3DRECT Rect = {};
	xbox::dword_xt Flags;
	D3DCOLOR Color;
	float Z;
	xbox::dword_xt Stencil;

	Flags = pg->KelvinPrimitive.ClearSurface;
	Color = pg->KelvinPrimitive.SetColorClearValue;
	Z = (float)pg->KelvinPrimitive.SetZStencilClearValue;
	Stencil = pg->KelvinPrimitive.SetZStencilClearValue; // Was Z, but would require casting through float and back to dword
	
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
        uint32_t max_words_available = (dma_get>dma_put)
			? ((uint32_t)dma_limit - (uint32_t)word_ptr) / 4
			: ((uint32_t)dma_put - (uint32_t)word_ptr) / 4;
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
