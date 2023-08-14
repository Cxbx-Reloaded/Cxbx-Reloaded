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
#include <d3dx9math.h> // for D3DXMatrix, etc
#include "devices/video/nv2a.h" // For g_NV2A, PGRAPHState
#include "devices/video/nv2a_int.h" // For NV** defines
#include "Logging.h"

// global state flags for NV2A/KelvinPrimitive, using same bit mask as xbox d3d, defined in XbD3D8Types.h. when content of Kelvin changed, set reresponded flag. then update corresponded state in D3D_draw_state_update()
DWORD NV2A_stateFlags = 0;

// global dirty flags for NV2A/KelvinPrimitive, using same bit mask as xbox d3d, defined in XbD3D8Types.h. when content of Kelvin changed, set reresponded flag. then update corresponded state in D3D_draw_state_update()
DWORD NV2A_DirtyFlags = 0;
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
xbox::X_STREAMINPUT D3D_Xbox_StreamSource[X_VSH_MAX_STREAMS] = { 0 }; // Store the vertex buffer/stride info used by each attributes after vertex buffer grouping
unsigned D3D_Xbox_StreamCount = 0;// Store the stream count used by each attributes after vertex buffer grouping, strating from 1
// textures to store the conversion info from NV2A KelvinPrimitive.SetTexture[4]
xbox::X_D3DBaseTexture NV2A_texture_stage_texture[xbox::X_D3DTS_STAGECOUNT];
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
	return (DWORD)FloatToLong(f + 0.5f);
}
extern float CxbxrGetSuperSampleScale(void);
static inline DWORD FtoDW(FLOAT f) { return *((DWORD*)&f); }
static inline FLOAT DWtoF(DWORD f) { return *((FLOAT*)&f); }

void CxbxrImpl_LazySetTextureState(NV2AState* d)
{
	PGRAPHState* pg = &d->pgraph;
	for (int stage = 0; stage < 4; stage++) {
		// process texture stage texture info if it's dirty
		if ((NV2A_DirtyFlags & (X_D3DDIRTYFLAG_TEXTURE_STATE_0 << stage)) != 0) {
			// if the texture stage is disabled, pg->KelvinPrimitive.SetTexture[stage].Control0 when xbox d3d SetTexture(stage,0), but in actual situation Control0 isn't 0, Offset and Format are 0.
			// FIXME!! check (pg->KelvinPrimitive.SetTexture[stage].Control0 & NV097_SET_TEXTURE_CONTROL0_ENABLE) == 0 should be enough, but there are (pg->KelvinPrimitive.SetTexture[stage].Control0 & NV097_SET_TEXTURE_CONTROL0_ENABLE) != 0 and the texture is empty. could be HLE/NV2A confliction
			if ((pg->KelvinPrimitive.SetTexture[stage].Control0 & NV097_SET_TEXTURE_CONTROL0_ENABLE) == 0 || pg->KelvinPrimitive.SetTexture[stage].Format == 0 || pg->KelvinPrimitive.SetTexture[stage].Offset == 0) {
				g_pNV2A_SetTexture[stage] = nullptr;
			}
			// texture stage enabled, convert the KelvinPrimitive.SetTexture[stage] to NV2A_texture_stage_texture[stage], and set g_pXbox_SetTexture[stage]
			else {
				g_pNV2A_SetTexture[stage] = &NV2A_texture_stage_texture[stage];
				NV2A_texture_stage_texture[stage].Data = pg->KelvinPrimitive.SetTexture[stage].Offset;
				NV2A_texture_stage_texture[stage].Format = pg->KelvinPrimitive.SetTexture[stage].Format;
				unsigned width = 0, height = 0, pitch = 0;
				pitch = (pg->KelvinPrimitive.SetTexture[stage].Control1 & NV097_SET_TEXTURE_CONTROL1_IMAGE_PITCH) >> 16;
				width = (pg->KelvinPrimitive.SetTexture[stage].ImageRect & NV097_SET_TEXTURE_IMAGE_RECT_WIDTH) >> 16;
				height = (pg->KelvinPrimitive.SetTexture[stage].ImageRect & NV097_SET_TEXTURE_IMAGE_RECT_HEIGHT);
				// texture.Size could be 0
				if (pg->KelvinPrimitive.SetTexture[stage].Control1 == 0 && pg->KelvinPrimitive.SetTexture[stage].ImageRect == 0) {
					NV2A_texture_stage_texture[stage].Size = 0;
				}
				//convert pitch/height/width to texture.Size
				else {
					width = (width - 1) & X_D3DSIZE_WIDTH_MASK;
					height = ((height - 1) << X_D3DSIZE_HEIGHT_SHIFT) & X_D3DSIZE_HEIGHT_MASK;
					pitch = ((pitch / 64) - 1) << X_D3DSIZE_PITCH_SHIFT;//&X_D3DSIZE_PITCH_MASK
					NV2A_texture_stage_texture[stage].Size = pitch | height | width;
				}
				NV2A_texture_stage_texture[stage].Common = 0x00040001;// fake xbox d3d resource,
				NV2A_texture_stage_texture[stage].Lock = 0;
				//update texture stage states
				DWORD filter, address, control0, magFilter, minFilter, colorSign, convolutionKernel;
				INT lodBias;
				filter = pg->KelvinPrimitive.SetTexture[stage].Filter;//NV097_SET_TEXTURE_FILTER(stage)
				/*
				DWORD filter
				= DRF_NUM(097, _SET_TEXTURE_FILTER, _MIPMAP_LOD_BIAS, lodBias)

				| convolutionKernel

				| MinFilter(minFilter, pTextureStates[D3DTSS_MIPFILTER])

				| DRF_NUMFAST(097, _SET_TEXTURE_FILTER, _MAG, magFilter)

				| colorSign;
				*/
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
				// bumpenv always force setting color sign to D3DTSIGN_GSIGNED | D3DTSIGN_BSIGNED
				if (colorSign == (NV097_SET_TEXTURE_FILTER_GSIGNED | NV097_SET_TEXTURE_FILTER_BSIGNED)) {
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLOROP, xbox::X_D3DTOP_BUMPENVMAP);
				}
				magFilter = (filter & NV097_SET_TEXTURE_FILTER_MAG) >> 24;
				minFilter = (filter & NV097_SET_TEXTURE_FILTER_MIN) >> 16;
				lodBias = (filter & NV097_SET_TEXTURE_FILTER_MIPMAP_LOD_BIAS);
				float LOD = (float)lodBias / 256.0;
				// Fixedme!!!
				/*
				 INT lodBias = Round(256.0f * (Floatify(pTextureStates[D3DTSS_MIPMAPLODBIAS])
										  + pDevice->m_SuperSampleLODBias));
				// from Present()
				pDevice->m_SuperSampleScale = minScale;
				pDevice->m_SuperSampleLODBias = g_LODBias2x[Round(2.0f * minScale) - 2];
				*/
				extern float CxbxrGetSuperSampleScale(void);
				float minScale = CxbxrGetSuperSampleScale();
				float SuperSampleLODBias = g_LODBias2x[Round(2.0f * minScale) - 2];
				LOD -= SuperSampleLODBias;
				NV2ATextureStates.Set(stage, xbox::X_D3DTSS_MIPMAPLODBIAS, FtoDW(LOD));
				convolutionKernel = filter;
				DWORD colorKeyOp = pg->KelvinPrimitive.SetTexture[stage].Control0 & 0x3;// colorkeyop in Contrlo0 bit 1:0 //XboxTextureStates.Get(i, xbox::X_D3DTSS_COLORKEYOP);
				DWORD ALPHAKILL = pg->KelvinPrimitive.SetTexture[stage].Control0 & 0x4;//  alphakill in Contrlo0 bit 2//XboxTextureStates.Get(i, xbox::X_D3DTSS_ALPHAKILL);
				//control0 |= DRF_NUMFAST(097, _SET_TEXTURE_CONTROL0, _LOG_MAX_ANISO,maxAnisotropy);
				DWORD maxAnisotropy = (pg->KelvinPrimitive.SetTexture[stage].Control0 & NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO) >> 4;
				if ((minFilter == xbox::X_D3DTEXF_POINT) && (magFilter == xbox::X_D3DTEXF_POINT) && (maxAnisotropy == 0)) {//D3DTEXF_POINT=1
					//(minFilter < D3DTEXF_ANISOTROPIC) && (magFilter < D3DTEXF_ANISOTROPIC) // D3DTEXF_ANISOTROPIC=3
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_MAXANISOTROPY, maxAnisotropy);
				}
				else if ((minFilter == xbox::X_D3DTEXF_LINEAR) && (magFilter == xbox::X_D3DTEXF_LINEAR)) {//D3DTEXF_LINEAR=2
					//(minFilter < D3DTEXF_ANISOTROPIC) && (magFilter < D3DTEXF_ANISOTROPIC)
					NV2ATextureStates.Set(stage, xbox::X_D3DTSS_MAXANISOTROPY, maxAnisotropy + 1);
				}
				else if ((minFilter == xbox::X_D3DTEXF_LINEAR) && (magFilter == 0x04)) {//NV097_SET_TEXTURE_FILTER_MAG_CONVOLUTION_2D_LOD0=0x4, D3DTEXF_LINEAR=2
					//(minFilter > D3DTEXF_ANISOTROPIC) ||	(magFilter > D3DTEXF_ANISOTROPIC)
					magFilter = xbox::X_D3DTEXF_QUINCUNX;
					if (((convolutionKernel& 0x0004000)!=0)){//NV097_SET_TEXTURE_FILTER_CONVOLUTION_KERNEL_GAUSSIAN_3=0x02<<13
						magFilter = xbox::X_D3DTEXF_GAUSSIANCUBIC;;//(minFilter == D3DTEXF_GAUSSIANCUBIC) ||	(magFilter == D3DTEXF_GAUSSIANCUBIC) //xbox::X_D3DTEXF_GAUSSIANCUBIC=5
					}
					;
				}else{
					;
				}
				NV2ATextureStates.Set(stage, xbox::X_D3DTSS_MINFILTER, minFilter);
				NV2ATextureStates.Set(stage, xbox::X_D3DTSS_MAGFILTER, magFilter);


				DWORD COLORKEYCOLOR = pg->KelvinPrimitive.SetColorKeyColor[stage];// XboxTextureStates.Get(i, xbox::X_D3DTSS_COLORKEYCOLOR);
				DWORD maxMipMapLevel = (pg->KelvinPrimitive.SetTexture[stage].Control0 & NV097_SET_TEXTURE_CONTROL0_MIN_LOD_CLAMP)>>(18+8); //D3DTSS_MAXMIPLEVEL
				NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORKEYOP, colorKeyOp);
				NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ALPHAKILL, ALPHAKILL);
				NV2ATextureStates.Set(stage, xbox::X_D3DTSS_COLORKEYCOLOR, COLORKEYCOLOR);
				NV2ATextureStates.Set(stage, xbox::X_D3DTSS_MAXMIPLEVEL, maxMipMapLevel);

				address = pg->KelvinPrimitive.SetTexture[stage].Address;
				NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ADDRESSU, address& NV097_SET_TEXTURE_ADDRESS_U);
				NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ADDRESSV, address & NV097_SET_TEXTURE_ADDRESS_V>>8);
				NV2ATextureStates.Set(stage, xbox::X_D3DTSS_ADDRESSW, address & NV097_SET_TEXTURE_ADDRESS_P>>16);
				//D3DTSS_TEXCOORDINDEX shall be unique for each stage.
				NV2ATextureStates.Set(stage, xbox::X_D3DTSS_TEXCOORDINDEX, stage);


			}
		}
		//reset texture stage dirty flag
		NV2A_DirtyFlags &= ~X_D3DDIRTYFLAG_TEXTURE_STATE;
	}
}
extern NV2ADevice* g_NV2A; //TMP GLUE
extern D3DMATRIX g_xbox_transform_ModelView;
extern D3DMATRIX g_xbox_transform_InverseModelView;
extern D3DMATRIX g_xbox_transform_Composite;
extern D3DMATRIX g_xbox_DirectModelView_View;
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
bool g_xbox_transform_ModelView_dirty[4] = { false,false,false,false, };
bool g_xbox_transform_InverseModelView_dirty[4] = { false,false,false,false, };
bool g_xbox_transform_Composite_dirty = false;
// virtual pixle shader, convert content from KelvinPrimitive to the pixel shader below and call CxbxrImpl_SetPixelShader(pNV2A_PixelShader). 
xbox::X_PixelShader* pNV2A_PixelShader;
xbox::X_PixelShader NV2A_PixelShader;
xbox::X_D3DPIXELSHADERDEF NV2A_PSDef;
bool NV2A_ShaderOtherStageInputDirty=false;
bool NV2A_TextureFactorAllTheSame = false;
bool g_nv2a_use_nv2a_bumpenv = false;
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
void pgraph_use_NV2A_bumpenv(void)
{
	g_nv2a_use_nv2a_bumpenv = true;
}
void pgraph_notuse_NV2A_bumpenv(void)
{
	g_nv2a_use_nv2a_bumpenv = false;
}
bool pgraph_is_NV2A_bumpenv(void)
{
	return g_nv2a_use_nv2a_bumpenv;
}
void pgraph_use_UserPixelShader(void)
{
	// set pixel shader pointers
	pNV2A_PixelShader = &NV2A_PixelShader;
	NV2A_PixelShader.pPSDef = &NV2A_PSDef;
	// set dirty flag
	NV2A_DirtyFlags |= X_D3DDIRTYFLAG_SHADER_STAGE_PROGRAM;
}

void pgraph_use_FixedPixelShader(void)
{
	// set pixel shader pointer to null for fixed function pixel shader
	pNV2A_PixelShader = nullptr;
	// set dirty flag
	NV2A_DirtyFlags |= X_D3DDIRTYFLAG_SHADER_STAGE_PROGRAM;
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

bool pgraph_is_DirectModelView(void)
{
	return g_xbox_transform_use_DirectModelView;
}
bool pgraph_is_ModelView_dirty(void)
{
	return g_xbox_transform_ModelView_dirty[0];
}

void pgraph_SetModelViewMatrixDirty(unsigned int index)
{
	//PGRAPHState *pg = &d->pgraph;
	//D3DXMatrixTranspose((D3DXMATRIX *)&g_xbox_transform_ModelView, (D3DXMATRIX *)&pg->KelvinPrimitive.SetModelViewMatrix0[0]);
	g_xbox_transform_ModelView_dirty[index] = true;
	//g_xbox_transform_use_DirectModelView = true;
}
void pgraph_SetInverseModelViewMatrixDirty(unsigned int index)
{
	//PGRAPHState *pg = &d->pgraph;
	//g_xbox_transform_InverseModelView = *(D3DMATRIX *)(&pg->KelvinPrimitive.SetInverseModelViewMatrix0[0]);
	g_xbox_transform_InverseModelView_dirty[index] = true;
	//g_xbox_transform_use_DirectModelView = true;
}
void pgraph_SetCompositeMatrixDirty(void)
{
	//PGRAPHState *pg = &d->pgraph;
	//D3DXMatrixTranspose((D3DXMATRIX *)&g_xbox_transform_Composite, (D3DXMATRIX *)&pg->KelvinPrimitive.SetCompositeMatrix[0]);
	g_xbox_transform_Composite_dirty = true;
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


void d3d_bumpenv_update(NV2AState *d, bool isUserMode)
{
	// for bmpenv, NV2A always uses stage 1~3. in user mode, d3d uses stage 1~3, in fixed mode, d3d uses stage 0~2.
	unsigned int d3d_stage = (isUserMode) ? 1 : 0;
	unsigned int nv2a_stage;
	for (nv2a_stage = 1; nv2a_stage <= 3; nv2a_stage++, d3d_stage) {
		//XboxTextureStates.Set(d3d_stage, xbox::X_D3DTSS_BUMPENVMAT00);
		;

	}
}
DWORD convert_NV2A_combiner_reg_to_xbox_reg(DWORD nvreg) {
	DWORD xboxreg = xbox::X_D3DTA_CURRENT;
	assert(nvreg >= NV097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_4);
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

	for (int i = 0; i < 4; i++) {

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
		// normally the result register is either C: current or D: temp, and located in bit 8~11 as sum of A*B+C*D, or in bit 5~7 as A*B.
		if (((resultarg >> 8) & 0xF) > 0) {
			NV2A_resultArg[i] = (resultarg >> 8) & 0xF == 0xC ? xbox::X_D3DTA_CURRENT : xbox::X_D3DTA_TEMP;
		}
		else {
			NV2A_resultArg[i] = (resultarg >> 4) & 0xF == 0xC ? xbox::X_D3DTA_CURRENT : xbox::X_D3DTA_TEMP;
		}
		unsigned int startStage = 0;
		// FIXME!!! we set colorOp as X_D3DTOP_DISABLE for stage 0~2 when point sprite was enabled.
		if (i < startStage) {
			NV2A_colorOP[i]= xbox::X_D3DTOP_DISABLE;
		}else if (i == startStage) {
			// FIXME!! if stage 0 was disabled, should we still setup the combiner with default values just like xbox d3d does instead of skipping it?
			// and for PointSprite enabled, the combiner stage update starts at stage 3, not 0, so this condition will happen in stage 3. what shall we do with stage 0 and 1?
			if ((colorICW == 0x04200000 || colorICW == 0x00002004)&& (alphaICW == (0x14200000)) && (alphaOCW == colorOCW)) {//(colorICW == (NV097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_4 | NV097_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_INVERT  )) && alphaICW == (colorICW | NV097_SET_COMBINER_COLOR_ICW_A_ALPHA )) { //,(0x10 & 0x20) << 23)
				NV2A_colorOP[i] = xbox::X_D3DTOP_DISABLE;
				NV2A_alphaOP[i] = xbox::X_D3DTOP_DISABLE;
				//NV2A_resultArg[i] = alphaOCW;
			}
			else if (alphaICW == 0 && alphaOCW == 0) {
				// set color OP with invalid value as flag to process later
				NV2A_colorOP[i] = xbox::X_D3DTOP_LAST + 1;
				// set alpha OP as X_D3DTOP_DISABLE
				NV2A_alphaOP[i] = xbox::X_D3DTOP_DISABLE;
			}
			else {
				// set both OP with invalid value as flag to process later
				NV2A_colorOP[i] = xbox::X_D3DTOP_LAST + 1;
				NV2A_alphaOP[i] = xbox::X_D3DTOP_LAST + 1;
			}
		}
		// for other stages, it's much easier to tell whether colorOp is disabled or not.
		else {
			if (colorICW == 0 && colorOCW == 0) {
				NV2A_colorOP[i] = xbox::X_D3DTOP_DISABLE;
			}
			// set OP with invalid value as flag to process later
			else {
				NV2A_colorOP[i] = xbox::X_D3DTOP_LAST+1;
			}
			if (alphaICW == 0 && alphaOCW == 0) {

				NV2A_alphaOP[i] = xbox::X_D3DTOP_DISABLE;
			}
			// set OP with invalid value as flag to process later
			else {
				NV2A_alphaOP[i] = xbox::X_D3DTOP_LAST + 1;
			}
		}
		// FIXME!!! can we really continue the loop when colorOP == X_D3DTOP_DISABLE?
		if (NV2A_colorOP[i] == xbox::X_D3DTOP_DISABLE) {
			continue;
		}
		/*
		if (NV2A_alphaOP[i] == xbox::X_D3DTOP_DISABLE) {
			;
		}
		if (NV2A_colorOP[i] == xbox::X_D3DTOP_DISABLE && NV2A_alphaOP[i] == xbox::X_D3DTOP_DISABLE) {
			;
		}
		*/
		// process color OP if it's not X_D3DTOP_LAST
		if (NV2A_colorOP[i] > xbox::X_D3DTOP_LAST) {
			// colorOP SelectARG1 , A == Arg1, B == 1, C == 0, D == 0
			if ((colorICW & 0xF0FFFFFF == 0x00200000) && (colorOCW & 0xFFFF00FF == 0)) {
				NV2A_colorOP[i] = xbox::X_D3DTOP_SELECTARG1;
				// arg1 always source A
				NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF); // FIXME!!!,  reg 4 is D3DTA_DIFFUSE :0, not sure this direct shift is correct or not6
			}
			// colorOP SelectARG2, A == 0, B ==0, C == 1, D == Arg2
			else if ((colorICW & 0xFFFFFF00 == 0x00002000) && (colorOCW & 0xFFFF00FF == 0)) {
				NV2A_colorOP[i] = xbox::X_D3DTOP_SELECTARG2;
				// arg2 always source D
				NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW) & 0xF);// FIXME!!!,  reg 4 is D3DTA_DIFFUSE :0, not sure this direct shift is correct or not6
			}
			// OP modulate, A*B, C==0, D==0
			else if (colorICW & 0xF0F0FFFF == 0x0) {
				// arg1 in source A
				NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
				// arg2 in source B
				NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 16) & 0xF);
				if (colorOCW & 0x20000 != 0) {
					NV2A_colorOP[i] = xbox::X_D3DTOP_MODULATE4X;
				}
				else if (colorOCW & 0x10000 != 0) {
					NV2A_colorOP[i] = xbox::X_D3DTOP_MODULATE2X;
				}
				else {
					NV2A_colorOP[i] = xbox::X_D3DTOP_MODULATE;
				}
			}
			// OP ADD, source A + D, A == Arg1, B == 1, C == 1, D == Arg2
			else if (colorICW & 0xF0FFFFF0 == 0x00202000) {
				NV2A_colorOP[i] = xbox::X_D3DTOP_ADD;
				if (colorOCW & 0x18000 == 0x8000)
					NV2A_colorOP[i] = xbox::X_D3DTOP_ADDSIGNED;
				if (colorOCW & 0x18000 == 0x18000)
					NV2A_colorOP[i] = xbox::X_D3DTOP_ADDSIGNED2X;
				// arg1 in source A
				NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
				// arg2 in source D
				NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW) & 0xF);
			}
			// D3DTOP_SUBTRACT,  source A - D, A == Arg1, B == 1, C == -1, D == Arg2
			else if (colorICW & 0xF0FFFFF0 == 0x00204000) {
				NV2A_colorOP[i] = xbox::X_D3DTOP_SUBTRACT;
				// arg1 in source A
				NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
				// arg2 in source D
				NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW) & 0xF);
			}
			// D3DTOP_ADDSMOOTH, A+C*D, A=Arg1, B=1, C=1-Arg1, D=Arg2
			else if ((colorICW & 0xF0FFF0F0 == 0x00200000) && (convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF) == convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 8) & 0xF)) && (colorICW&NV097_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_INVERT) != 0) {
				NV2A_colorOP[i] = xbox::X_D3DTOP_ADDSMOOTH;
				// arg1 in source A
				NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
				// arg2 in source D
				NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW) & 0xF);

			}
			// D3DTOP_BLENDDIFFUSEALPHA/D3DTOP_BLENDCURRENTALPHA/D3DTOP_BLENDTEXTUREALPHA/D3DTOP_BLENDFACTORALPHA, A+C*D, A=Arg1, B=alphaOP-12 and CFLAG_ALPHAREPLICATE set , C=1-Arg1, D=Arg2
			// A+C*D, A=Arg1, B=alpha(alphaOP-12) and CFLAG_ALPHAREPLICATE set , C=alpht(1-Arg1) CFLAG_COMPLEMENT|CFLAG_ALPHAREPLICATE set , D=Arg2
			// verify flags of A/B/C/D, B=reg0, reg(A)==reg(C)
			else if ((colorICW & 0xF0F0F0F0 == 0x00103000) && (convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF) == convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 8) & 0xF))) {
				NV2A_colorOP[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 16) & 0xF) + xbox::X_D3DTOP_BLENDDIFFUSEALPHA;
				// arg1 in source A
				NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
				// arg2 in source D
				NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW) & 0xF);
			}
			// D3DTOP_BLENDTEXTUREALPHAPM, A+C*D, A=Arg1, B=1, C=alpha(1-D3DTA_TEXTURE) CFLAG_COMPLEMENT | CFLAG_ALPHAREPLICATE, D=Arg2, 
			else if ((colorICW & 0xF0FFF0F0 == 0x00203000) && convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 8) & 0xF) == 2) {
				NV2A_colorOP[i] = xbox::X_D3DTOP_BLENDTEXTUREALPHAPM;
				// arg1 in source A
				NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
				// arg2 in source D
				NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW) & 0xF);
			}
			// D3DTOP_PREMODULATE, A=Arg1, if stage 0, B==D3DTA_TEXTURE, if stage1 or stage2 B=1, C and D not set
			else if ((i > 0 && i << 3 && (colorICW & 0xF0FFFFFF == 0x00200000)) || (i == 0 && (colorICW & 0xF0F0FFFF == 0x00000000) && convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 16) & 0xF) == xbox::X_D3DTA_TEXTURE)) {
				NV2A_colorOP[i] = xbox::X_D3DTOP_PREMODULATE;
				// arg1 in source A
				NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);

			}
			// D3DTOP_MODULATEALPHA_ADDCOLOR, A=Arg1, B=1,  C =alpha(Arg1) CFLAG_ALPHAREPLICATE set,  D =Arg2
			else if ((colorICW & 0xF0FFF0F0 == 0x00201000) && (convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF) == convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 8) & 0xF))) {
				NV2A_colorOP[i] = xbox::X_D3DTOP_MODULATEALPHA_ADDCOLOR;
				// arg1 in source A
				NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
				// arg2 in source D
				NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW) & 0xF);
			}
			// D3DTOP_MODULATECOLOR_ADDALPHA, A=Arg1, B=Arg2,  C =alpha(Arg1) CFLAG_ALPHAREPLICATE set,  D =1
			else if ((colorICW & 0xF0F0F0FF == 0x00001020) && (convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF) == convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 8) & 0xF))) {

				NV2A_colorOP[i] = xbox::X_D3DTOP_MODULATECOLOR_ADDALPHA;
				// arg1 in source A
				NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
				// arg2 in source D
				NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 16) & 0xF);
			}
			// D3DTOP_MODULATEINVALPHA_ADDCOLOR, A=Arg1, B=1,  C =1- alpha(Arg1) CFLAG_ALPHAREPLICATE CFLAG_COMPLEMENT set,  D =Arg2
			else if ((colorICW & 0xF0FFF0F0 == 0x00203000) && (convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF) == convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 8) & 0xF))) {
				NV2A_colorOP[i] = xbox::X_D3DTOP_MODULATEINVALPHA_ADDCOLOR;
				// arg1 in source A
				NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
				// arg2 in source D
				NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW) & 0xF);
			}
			// D3DTOP_MODULATEINVCOLOR_ADDALPHA, A=Arg1, B=Arg2,  C =alpha(Arg1) CFLAG_ALPHAREPLICATE CFLAG_COMPLEMENT set,  D =1
			else if ((colorICW & 0xF0F0F0FF == 0x00003020) && (convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF) == convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 8) & 0xF))) {

				NV2A_colorOP[i] = xbox::X_D3DTOP_MODULATEINVCOLOR_ADDALPHA;
				// arg1 in source A
				NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
				// arg2 in source D
				NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 16) & 0xF);
			}

			// D3DTOP_DOTPRODUCT3, A=Arg1 CFLAG_EXPANDNORMAL set, B=Arg2 CFLAG_EXPANDNORMAL set,  C and D default 0
			// OCW |= NV097_SET_COMBINER_COLOR_OCW_AB_DOT_ENABLE_TRUE| NV097_SET_COMBINER_COLOR_OCW_CD_DOT_ENABLE_FALSE | NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_AB_AB_DST_ENABLE | NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_CD_DISABLE
			else if ((colorICW & 0xF0F0FFFF == 0x40400000) && (colorOCW & 0x00082000 == 0x82000)&& (alphaICW==0) && (alphaOCW == 0)) {

				NV2A_colorOP[i] = xbox::X_D3DTOP_DOTPRODUCT3;
				// arg1 in source A
				NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
				// arg2 in source B
				NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 16) & 0xF);
				// result arg in OCW AB dst
				NV2A_resultArg[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorOCW >> 4) & 0xF);
			}
			// D3DTOP_LERP, A=Arg0,B=Arg1, C=(1-Arg1) CFLAG_COMPLEMENT set, D=Arg2
			else if ((colorICW & 0xF0F0F0F0 == 0x00002000) && ((colorICW >> 16) & 0xF) == ((colorICW >> 8) & 0xF)) {

				NV2A_colorOP[i] = xbox::X_D3DTOP_LERP;
				// arg0 in source A
				NV2A_colorArg0[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
				// arg1 in source B
				NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 16) & 0xF);
				// arg2 in source D
				NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW) & 0xF);
			}
			// D3DTOP_BUMPENVMAP/D3DTOP_BUMPENVMAPLUMINANCE, A=D3DTA_CURRENT,B=1, C and D default 0
			else if ((colorICW & 0xF0FFFFFF == 0x00200000) && convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF) == xbox::X_D3DTA_CURRENT) {
				// FIXME!!! we can't tell D3DTOP_BUMPENVMAP/D3DTOP_BUMPENVMAPLUMINANCE here!. need a way.
				NV2A_colorOP[i] = xbox::X_D3DTOP_BUMPENVMAP;
				// arg1 in source A
				NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
			}
			// D3DTOP_MULTIPLYADD, A=Arg0,B=1, C=Arg1, D=Arg2
			// FIXME!!! condition too simple, put this close to the end.
			else if ((colorICW & 0xF0FFF0F0 == 0x00200000) ) {

				NV2A_colorOP[i] = xbox::X_D3DTOP_MULTIPLYADD;
				// arg0 in source A
				NV2A_colorArg0[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 24) & 0xF);
				// arg1 in source C
				NV2A_colorArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW >> 8) & 0xF);
				// arg2 in source D
				NV2A_colorArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((colorICW) & 0xF);
			}
			// unknown condition, set to disable
			else {
				NV2A_colorOP[i] = xbox::X_D3DTOP_DISABLE;
				//FIXME!!! shall we continue loop here?
			}
		}
		// now we process alphaOP
		if (NV2A_alphaOP[i] > xbox::X_D3DTOP_LAST) {
			// alphaOP SelectARG1 
			if ((alphaICW & 0xF0FFFFFF == 0x10301010) && (alphaOCW & 0xFFFF00FF == 0)) {
				NV2A_alphaOP[i] = xbox::X_D3DTOP_SELECTARG1;
				// arg1 always source A
				NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);// FIXME!!!,  reg 4 is D3DTA_DIFFUSE :0, not sure this direct shift is correct or not6
			}
			// alphaOP SelectARG2
			else if ((alphaICW & 0xFFFFFFF0 == 0x10103010) && (alphaOCW & 0xFFFF00FF == 0)) {
				NV2A_alphaOP[i] = xbox::X_D3DTOP_SELECTARG2;
				// arg2 always source D
				NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW) & 0xF);// FIXME!!!,  reg 4 is D3DTA_DIFFUSE :0, not sure this direct shift is correct or not6
			}
			// OP modulate, A*B, C==0, D==0, A=Arg1, B=Arg2, 
			else if (alphaICW & 0xF0F0FFFF == 0x10101010) {
				// arg1 in source A
				NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
				// arg2 in source B
				NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 16) & 0xF);
				if (alphaOCW & 0x20000 != 0) {
					NV2A_alphaOP[i] = xbox::X_D3DTOP_MODULATE4X;
				}
				else if (alphaOCW & 0x10000 != 0) {
					NV2A_alphaOP[i] = xbox::X_D3DTOP_MODULATE2X;
				}
				else {
					NV2A_alphaOP[i] = xbox::X_D3DTOP_MODULATE;
				}
			}
			// OP ADD, source A + D, A == Arg1, B == 1, C == 1, D == Arg2
			else if (alphaICW & 0xF0FFFFF0 == 0x10303010) {
				NV2A_alphaOP[i] = xbox::X_D3DTOP_ADD;
				if (alphaOCW &0x18000 == 0x8000)
					NV2A_alphaOP[i] = xbox::X_D3DTOP_ADDSIGNED;
				if (alphaOCW & 0x18000 == 0x18000)
					NV2A_alphaOP[i] = xbox::X_D3DTOP_ADDSIGNED2X;
				// arg1 in source A
				NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
				// arg2 in source D
				NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW) & 0xF);
			}
			// D3DTOP_SUBTRACT,  source A - D, A == Arg1, B == 1, C == -1, D == Arg2
			else if (alphaICW & 0xF0FFFFF0 == 0x10305010) {
				NV2A_alphaOP[i] = xbox::X_D3DTOP_SUBTRACT;
				// arg1 in source A
				NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
				// arg2 in source D
				NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW) & 0xF);
			}
			// D3DTOP_ADDSMOOTH, A+C*D, A=Arg1, B=1, C=1-Arg1, D=Arg2
			// verify flags of A/B/C/D, B=reg0, reg(A)==reg(C)
			else if ((alphaICW & 0xF0FFF0F0 == 0x10301010)&&(convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF)== convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 8) & 0xF)) && (alphaICW&NV097_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_INVERT) != 0) {
				NV2A_alphaOP[i] = xbox::X_D3DTOP_ADDSMOOTH;
				// arg1 in source A
				NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
				// arg2 in source D
				NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW) & 0xF);

			}
			// D3DTOP_BLENDDIFFUSEALPHA/D3DTOP_BLENDCURRENTALPHA/D3DTOP_BLENDTEXTUREALPHA/D3DTOP_BLENDFACTORALPHA,
			// A+C*D, A=Arg1, B=alpha(alphaOP-12) and CFLAG_ALPHAREPLICATE set , C=alpht(1-Arg1) CFLAG_COMPLEMENT|CFLAG_ALPHAREPLICATE set , D=Arg2
			// verify flags of A/B/C/D, B=reg0, reg(A)==reg(C)
			else if ( (alphaICW & 0xF0F0F0F0 == 0x10103010) && ( convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF) == convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 8) & 0xF) ) ) {
				NV2A_alphaOP[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 16) & 0xF) + xbox::X_D3DTOP_BLENDDIFFUSEALPHA;
				// arg1 in source A
				NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
				// arg2 in source D
				NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW) & 0xF);
			}
			// D3DTOP_BLENDTEXTUREALPHAPM, A+C*D, A=Arg1, B=1, C=alpha(1-D3DTA_TEXTURE) CFLAG_COMPLEMENT | CFLAG_ALPHAREPLICATE, D=Arg2, 
			else if ((alphaICW & 0xF0FFF0F0 == 0x10303010)  && convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 8) & 0xF) == 2) {
				NV2A_alphaOP[i] = xbox::X_D3DTOP_BLENDTEXTUREALPHAPM;
				// arg1 in source A
				NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
				// arg2 in source D
				NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW) & 0xF);
			}
			// D3DTOP_PREMODULATE, A=Arg1, if stage 0, B==D3DTA_TEXTURE, if stage1 or stage2 B=1, C and D default to 0
			else if ((i > 0 && i << 3 && (alphaICW & 0xF0FFFFFF == 0x10301010)) || (i == 0 && (alphaICW & 0xF0F0FFFF == 0x10101010) && convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 16) & 0xF) == xbox::X_D3DTA_TEXTURE)) {
				NV2A_alphaOP[i] = xbox::X_D3DTOP_PREMODULATE;
				// arg1 in source A
				NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);

			}
			// D3DTOP_MODULATEALPHA_ADDCOLOR, A=Arg1, B=1,  C =alpha(Arg1) CFLAG_ALPHAREPLICATE set,  D =Arg2
			/* // only for color OP
			else if ((alphaICW & 0xF0FFF0F0 == 0x00201000) && (convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF) == convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 8) & 0xF))) {
				NV2A_alphaOP[i] = xbox::X_D3DTOP_MODULATEALPHA_ADDCOLOR;
				// arg1 in source A
				NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
				// arg2 in source D
				NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW) & 0xF);
			}
			// D3DTOP_MODULATECOLOR_ADDALPHA, A=Arg1, B=Arg2,  C =alpha(Arg1) CFLAG_ALPHAREPLICATE set,  D =1
			else if ((alphaICW & 0xF0F0F0FF == 0x00001020) && (convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF) == convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 8) & 0xF))) {

				NV2A_alphaOP[i] = xbox::X_D3DTOP_MODULATECOLOR_ADDALPHA;
				// arg1 in source A
				NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
				// arg2 in source D
				NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 16) & 0xF);
			}
			// D3DTOP_MODULATEINVALPHA_ADDCOLOR, A=Arg1, B=1,  C =alpha(Arg1) CFLAG_ALPHAREPLICATE CFLAG_COMPLEMENT set,  D =Arg2
			else if ((alphaICW & 0xF0FFF0F0 == 0x00203000) && (convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF) == convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 8) & 0xF))) {
				NV2A_alphaOP[i] = xbox::X_D3DTOP_MODULATEINVALPHA_ADDCOLOR;
				// arg1 in source A
				NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
				// arg2 in source D
				NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW) & 0xF);
			}
			// D3DTOP_MODULATEINVCOLOR_ADDALPHA, A=Arg1, B=Arg2,  C =alpha(Arg1) CFLAG_ALPHAREPLICATE CFLAG_COMPLEMENT set,  D =1
			else if ((alphaICW & 0xF0F0F0FF == 0x00003020) && (convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF) == convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 8) & 0xF))) {

				NV2A_alphaOP[i] = xbox::X_D3DTOP_MODULATEINVCOLOR_ADDALPHA;
				// arg1 in source A
				NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
				// arg2 in source D
				NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 16) & 0xF);
			}
			*/
			// D3DTOP_DOTPRODUCT3, A=Arg1 CFLAG_EXPANDNORMAL set, B=Arg2 CFLAG_EXPANDNORMAL set,  C and D default 0
			// OCW |= NV097_SET_COMBINER_alpha_OCW_AB_DOT_ENABLE_TRUE| NV097_SET_COMBINER_alpha_OCW_CD_DOT_ENABLE_FALSE | NV097_SET_COMBINER_alpha_OCW_BLUETOALPHA_AB_AB_DST_ENABLE | NV097_SET_COMBINER_alpha_OCW_BLUETOALPHA_CD_DISABLE
			/* // only for color OP
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
			// D3DTOP_LERP, A=Arg0,B=Arg1, C=(1-Arg1) CFLAG_COMPLEMENT set, D=Arg2
			else if ((alphaICW & 0xF0F0F0F0 == 0x10103010) && ((alphaICW >> 16) & 0xF) == ((alphaICW >> 8) & 0xF)) {

				NV2A_alphaOP[i] = xbox::X_D3DTOP_LERP;
				// arg0 in source A
				NV2A_alphaArg0[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
				// arg1 in source B
				NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 16) & 0xF);
				// arg2 in source D
				NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW) & 0xF);
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
			else if ((alphaICW & 0xF0FFF0F0 == 0x10301010) ) {

				NV2A_alphaOP[i] = xbox::X_D3DTOP_MULTIPLYADD;
				// arg0 in source A
				NV2A_alphaArg0[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 24) & 0xF);
				// arg1 in source C
				NV2A_alphaArg1[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW >> 8) & 0xF);
				// arg2 in source D
				NV2A_alphaArg2[i] = convert_NV2A_combiner_reg_to_xbox_reg((alphaICW) & 0xF);
			}
			// unknown condition, set to disable
			else {
			    NV2A_alphaOP[i] = xbox::X_D3DTOP_DISABLE;
            }
		}

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
extern void CxbxrGetSuperSampleScaleXY(float& x, float& y);
extern void CxbxrGetScreenSpaceOffsetXY(float& x, float& y);
extern void CxbxrSetViewport(xbox::X_D3DVIEWPORT8& Viewport);
extern void CxbxrGetViewport(xbox::X_D3DVIEWPORT8& Viewport);
extern void CxbxrSetWNearFarInverseWFar(float& WNear, float& WFar, float& WInverseWFar);
extern void CxbxrGetWNearFarInverseWFar(float& WNear, float& WFar, float& WInverseWFar);
extern float CxbxrGetZScale(void);


bool NV2A_viewport_dirty = false;
void pgraph_ComposeViewport(NV2AState *d)
{
	PGRAPHState *pg = &d->pgraph;
	HRESULT hRet;
	xbox::X_D3DVIEWPORT8 Viewport;

	float fm11, fm22, fm33, fm41, fm42, fm43, clipNear, clipFar;
	float SuperSampleScale, ScaleX, ScaleY, ScaleZ, ScreenSpaceOffsetX, ScreenSpaceOffsetY;
	//float aaOffsetX, aaOffsetY;
	
	CxbxrGetSuperSampleScaleXY(ScaleX, ScaleY);
	if (ScaleX < 1.0)ScaleX = 1.0;
	if (ScaleY < 1.0)ScaleY = 1.0;
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

	GetRenderTargetBaseDimensions(rendertargetBaseWidth, rendertargetBaseHeight);
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
		max = min + delta;
		if (max > 64.0f)max = 64.0f;

		float hostMinSize, hostMaxSize;
		hRet = g_pD3DDevice->GetRenderState(D3DRS_POINTSIZE_MIN, (DWORD*)&hostMinSize);
		hRet = g_pD3DDevice->GetRenderState(D3DRS_POINTSIZE_MAX, (DWORD*)&hostMaxSize);
		// D3D__RenderState[D3DRS_POINTSCALEENABLE]== false, set host point size only, disable host point scale
		if (pg->KelvinPrimitive.SetPointParamsEnable != 0) {
			//FIXEDME!!! we need viewport height, not sure including SuperSampleScaleY is correct or not.
			xbox::X_D3DVIEWPORT8 Viewport;
			if (NV2A_viewport_dirty == true) {
				// this only compose viewport from pgraph content 
				pgraph_ComposeViewport(d);
				// todo: actually setviewport with the composed viewport, currently we don't set the host viewport via pgraph content, yet. the SetViewport() is currently HLEed and not processed in pushbuffer.
				// D3DDevice_SetViewport() was patched and HLEed, here we only implement it when we're in RunPushbuffer().
				if ((NV2A_stateFlags & X_STATE_RUNPUSHBUFFERWASCALLED) != 0) {
					
					CxbxrGetViewport(Viewport);
					CxbxrImpl_SetViewport(&Viewport);
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
			NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_POINTSIZE_MIN, FtoDW(hostMinSize));
			NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_POINTSIZE_MAX, FtoDW(hostMaxSize));
			/// set host point scale A/B/C
			NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_POINTSCALE_A, FtoDW(xboxScaleA));
			NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_POINTSCALE_B, FtoDW(xboxScaleB));
			NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_POINTSCALE_C, FtoDW(xboxScaleC));
		}
		else {
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
	
	CxbxrGetSuperSampleScaleXY(SuperSampleScaleX, SuperSampleScaleY);
	if (SuperSampleScaleX < 1.0)SuperSampleScaleX = 1.0;
	if (SuperSampleScaleY < 1.0)SuperSampleScaleY = 1.0;
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
	extern D3DMATRIX g_xbox_transform_ViewportTransform;// tmp glue
	g_xbox_transform_ViewportTransform = viewportTransform;
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
	extern D3DMATRIX g_xbox_transform_ViewportTransform;// tmp glue
	extern D3DMATRIX g_xbox_transform_ProjectionViewportTransform;// tmp glue
	extern D3DMATRIX g_xbox_DirectModelView_InverseWorldViewTransposed;// tmp glue
	extern D3DMATRIX g_xbox_DirectModelView_View;// tmp glue
	extern D3DMATRIX g_xbox_DirectModelView_World;// tmp glue
	extern D3DMATRIX g_xbox_DirectModelView_Projection;// tmp glue


	D3DMATRIX matUnit;
	memset(&matUnit._11, 0, sizeof(matUnit));
	matUnit._11 = 1.0;
	matUnit._22 = 1.0;
	matUnit._33 = 1.0;
	matUnit._44 = 1.0;
	// TODO: this is a hack, we set cached View  Matrix to unit matrix and leave all variables in the cached projection matrix.
	g_xbox_DirectModelView_View = matUnit;

	D3DXMatrixTranspose((D3DXMATRIX*)&g_xbox_transform_ModelView, (D3DXMATRIX*)&pg->KelvinPrimitive.SetModelViewMatrix[0][0]);
	D3DXMatrixTranspose((D3DXMATRIX*)&g_xbox_transform_Composite, (D3DXMATRIX*)&pg->KelvinPrimitive.SetCompositeMatrix[0]);
	D3DXMatrixInverse((D3DXMATRIX*)&g_xbox_transform_InverseModelView, NULL, (D3DXMATRIX*)&g_xbox_transform_ModelView);

	if ((NV2A_DirtyFlags & X_D3DDIRTYFLAG_DIRECT_MODELVIEW) != 0) {
		// transpose KelvinPrimitive transform back to xbox d3d transform
		D3DXMatrixMultiply((D3DXMATRIX*)&g_xbox_transform_ProjectionViewportTransform, (D3DXMATRIX*)&g_xbox_transform_InverseModelView, (D3DXMATRIX*)&g_xbox_transform_Composite);
		// update projectionviewport transform for use in UpdateFixedFunctionShaderLight() and UpdateFixedFunctionVertexShaderState()
		//CxbxrImpl_SetModelView(&g_xbox_transform_ModelView, &g_xbox_transform_InverseModelView, &g_xbox_transform_Composite);
		//clear ModelView dirty flags.
			//g_xbox_transform_ModelView_dirty[0] = false;
			//g_xbox_transform_InverseModelView_dirty[0] = false;
	}
	// handle LazySetTransform();
	else {
		// not in skinning mode
		if (pg->KelvinPrimitive.SetSkinMode == 0) {
			D3DXMatrixMultiply((D3DXMATRIX*)&g_xbox_transform_ProjectionViewportTransform, (D3DXMATRIX*)&g_xbox_transform_InverseModelView, (D3DXMATRIX*)&g_xbox_transform_Composite);
			// update projectionviewport transform for use in UpdateFixedFunctionShaderLight() and UpdateFixedFunctionVertexShaderState()
			//CxbxrImpl_SetModelView(&g_xbox_transform_ModelView, &g_xbox_transform_InverseModelView, &g_xbox_transform_Composite);
		}
		// skinning mode, the commposite matrix doesn't include the ModelView matrix, only ViewPortTransform.
		// SetModelViewMatrix[1] was set.
		else {
			//D3DXMatrixInverse((D3DXMATRIX*)&g_xbox_transform_InverseModelView, NULL, (D3DXMATRIX*)&g_xbox_transform_ModelView);
			//D3DXMatrixMultiply((D3DXMATRIX*)&g_xbox_transform_ViewportTransform, (D3DXMATRIX*)&g_xbox_transform_InverseModelView, (D3DXMATRIX*)&g_xbox_transform_Composite);
			g_xbox_transform_ProjectionViewportTransform = g_xbox_transform_Composite;
			D3DXMatrixMultiply((D3DXMATRIX*)&g_xbox_transform_Composite, (D3DXMATRIX*)&g_xbox_transform_ModelView, (D3DXMATRIX*)&g_xbox_transform_ProjectionViewportTransform);
			CxbxrImpl_SetModelView(&g_xbox_transform_ModelView, &g_xbox_transform_InverseModelView, &g_xbox_transform_Composite);
		}
	}

	// compose xbox side matrix for use in d3d vertex shader update.
	// update g_xbox_DirectModelView_InverseWorldViewTransposed for use in FVF mode vertex shader constant update routine
	D3DXMatrixTranspose((D3DXMATRIX*)&g_xbox_DirectModelView_InverseWorldViewTransposed, (D3DXMATRIX*)&g_xbox_transform_InverseModelView);
	// update g_xbox_DirectModelView_Projection from g_xbox_transform_PrjectionViewportTransform
	// try to get g_xbox_transform_ViewportTransform first
	CxbxrImpl_GetViewportTransform(d); //g_xbox_transform_ViewportTransform will be set


	D3DXMATRIX matInverseViewportTransform, matViewportTransform;

	D3DXMatrixInverse((D3DXMATRIX*)&matInverseViewportTransform, NULL, (D3DXMATRIX*)&g_xbox_transform_ViewportTransform);

	D3DXMatrixMultiply((D3DXMATRIX*)&g_xbox_DirectModelView_Projection, (D3DXMATRIX*)&g_xbox_transform_ProjectionViewportTransform, (D3DXMATRIX*)&matInverseViewportTransform);

	// clear pgraph transform matrix dirty flags.
	for (int i = 0; i < 4; i++) {
		// update InverseModelView matrix if only ModelView matrix is updated

		if ((g_xbox_transform_ModelView_dirty[i] == true) || (g_xbox_transform_InverseModelView_dirty[i] == false)) {
			D3DXMATRIX matModelViewTransposed;
			// InverseModelView transform in KelvinPrim is the same as xbox d3d transform, not transposed.
			// transpose ModelView back to xbox d3d matrix
			D3DXMatrixTranspose(&matModelViewTransposed, (D3DXMATRIX*)&pg->KelvinPrimitive.SetModelViewMatrix[i][0]);
			// update the InverModelView matrix
			D3DXMatrixInverse((D3DXMATRIX*)&pg->KelvinPrimitive.SetInverseModelViewMatrix[i][0], NULL, (D3DXMATRIX*)&pg->KelvinPrimitive.SetModelViewMatrix[i][0]);
		}
		// clear dirty flags
		g_xbox_transform_ModelView_dirty[i] = false;
		g_xbox_transform_InverseModelView_dirty[i] = false;
	}
	g_xbox_transform_Composite_dirty = false;
	//}

	//these matrix will be used in UpdateFixedFunctionShaderLight(): view transform, and UpdateFixedFunctionVertexShaderState():  later in CxbxUpdateNativeD3DResources();
}

void CxbxrImpl_LazySetShaderStageProgram(NV2AState* d)
{
	PGRAPHState* pg = &d->pgraph;
	HRESULT hRet;
	// set use NV2A bumpenv flag, DxbxUpdateActivePixelShader()will pick up bumpenv from Kelvin
	pgraph_use_NV2A_bumpenv();

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
		bool bHasFinalCombiner = NV2A_stateFlags & X_STATE_COMBINERNEEDSSPECULAR!=0?true:false;
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
	// reset bumpenv_dirty[] flags, currently we're not referencing these flags. pixel shader generator will update bumpenv anyway.
	pg->bumpenv_dirty[0] = false;
	pg->bumpenv_dirty[1] = false;
	pg->bumpenv_dirty[2] = false;
	pg->bumpenv_dirty[3] = false;
	CxbxrImpl_SetPixelShader((xbox::dword_xt)pNV2A_PixelShader);
}

void CxbxrImpl_LazySetSpecFogCombiner(NV2AState* d)
{
	PGRAPHState* pg = &d->pgraph;
	HRESULT hRet;
	if (pg->KelvinPrimitive.SetFogEnable != 0) {
		float fogTableStart, fogTableEnd, fogTableDensity;
		DWORD fogTableMode, fogGenMode;
		float bias, scale, fogMode;
		fogGenMode = pg->KelvinPrimitive.SetFogGenMode;
		fogMode = pg->KelvinPrimitive.SetFogMode;
		bias = pg->KelvinPrimitive.SetFogParamsBias;
		scale = pg->KelvinPrimitive.SetFogParamsScale;
		// D3D__RenderState[D3DRS_RANGEFOGENABLE] == true fogGenMode =NV097_SET_FOG_GEN_MODE_V_RADIAL, else fogGenMode =NV097_SET_FOG_GEN_MODE_V_PLANAR
		// notice that when fogTableMode == D3DFOG_NONE, fogGenMode will be reset to NV097_SET_FOG_GEN_MODE_V_SPEC_ALPHA, in  that case, we won't be able to know what D3D__RenderState[D3DRS_RANGEFOGENABLE] should be
		bool bD3DRS_RangeFogEnable = fogGenMode == NV097_SET_FOG_GEN_MODE_V_RADIAL ? true : false;

		if (fogMode == NV097_SET_FOG_MODE_V_EXP2) {
			fogTableMode = D3DFOG_EXP2;
			// scale =  -fogTableDensity * (1 / (2 * sqrt(5.5452)))
			// scale = -fogTableDensity * (1.0f / (2.0f * 2.354824834249885f));
			fogTableDensity = -scale * (2.0f * 2.354824834249885f);

			//hRet = g_pD3DDevice->SetRenderState(D3DRS_FOGDENSITY, fogTableDensity);
			NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_FOGDENSITY, FtoDW(fogTableDensity));
		}
		else if (fogMode == NV097_SET_FOG_MODE_V_EXP) {
			fogTableMode = D3DFOG_EXP;
			// scale = -fogTableDensity * (1.0f / (2.0f * 5.5452f));
			fogTableDensity = -scale * (2.0f * 5.5452f);

			//hRet = g_pD3DDevice->SetRenderState(D3DRS_FOGDENSITY, fogTableDensity);
			NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_FOGDENSITY, FtoDW(fogTableDensity));
		}
		else if (fogMode == NV097_SET_FOG_MODE_V_LINEAR) {
			if (fogGenMode == NV097_SET_FOG_GEN_MODE_V_SPEC_ALPHA) {
				fogTableMode = D3DFOG_NONE;
				// RIXME!!! need to set bD3DRS_RangeFogEnable here because fogGenMode is not correctly verified.
				// D3DFOG_NONE : No fog effect, so set D3DRS_RangeFogEnable to false
				bD3DRS_RangeFogEnable = false;
			}else{
				fogTableMode = D3DFOG_LINEAR;
				FLOAT fogTableLinearScale;
				fogTableLinearScale = -scale;
				fogTableEnd = (bias - 1.0f) / fogTableLinearScale;
				// MAX_FOG_SCALE == 8192.0f
				if (fogTableLinearScale != 8192.0f) {
					// fogTableLinearScale = 1.0f / (fogTableEnd - fogTableStart);
					// fogTableStart = fogTableEnd - 1.0f/fogTableLinearScale
					fogTableStart = fogTableEnd - 1.0f / fogTableLinearScale;
				}
				else {
					//fogTableStart== fogTableEnd
					fogTableStart == fogTableEnd;
				}

				//hRet = g_pD3DDevice->SetRenderState(D3DRS_FOGSTART, fogTableStart);
				//hRet = g_pD3DDevice->SetRenderState(D3DRS_FOGEND, fogTableEnd);
				NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_FOGSTART, FtoDW(fogTableStart));
				NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_FOGEND, FtoDW(fogTableEnd));
			}

		}
		NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_FOGENABLE, true);
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
	if (pg->KelvinPrimitive.SetLightingEnable != false) {
		control = pg->KelvinPrimitive.SetLightControl;
		if (pg->KelvinPrimitive.SetSpecularEnable != false) {
			NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_LOCALVIEWER, (control& NV097_SET_LIGHT_CONTROL_LOCALEYE_TRUE)!=0? true:false);
		}
		colorMaterial = pg->KelvinPrimitive.SetColorMaterial;//NV097_SET_COLOR_MATERIAL, colorMaterial  // 0x298
		NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_COLORVERTEX, colorMaterial!=0?true:false);
		if(colorMaterial!=0){
			// retrive material source render state.
			for (int i = 0; i < 8; i++) {
				NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_BACKSPECULARMATERIALSOURCE+i, (colorMaterial>>(2*(7-i)))&0x03);
			}
		}
		lightEnableMask = pg->KelvinPrimitive.SetLightEnableMask;//Push1(pPush, NV097_SET_LIGHT_ENABLE_MASK, enableMask);      // 0x3bc;
		//set lights

		int lightNum;

		D3DVECTOR pos;
		D3DVECTOR dir;
		D3DVECTOR hv, EyeDirection;
		EyeDirection = { 0.0,0.0,-1.0 };

		FLOAT ambientR, ambientG, ambientB;
		FLOAT emissiveR, emissiveG, emissiveB;

		for (lightNum = 0; lightNum < 8; lightNum++) {
			bool bEnable;
			DWORD lightType = (lightEnableMask >> (lightNum << 1)) & 0x03;
			//default:
			NV2A_Light8[lightNum].Type = (D3DLIGHTTYPE)0;
			bEnable = false;

			switch (lightType) {
			case NV097_SET_LIGHT_ENABLE_MASK_LIGHT0_INFINITE://D3DLIGHT_DIRECTIONAL
				NV2A_Light8[lightNum].Type= D3DLIGHT_DIRECTIONAL;
				//NV097_SET_LIGHT_INFINITE_DIRECTION
				//Push1f(pPush,NV097_SET_LIGHT_LOCAL_RANGE(lightNum),	1e30f);
				NV2A_Light8[lightNum].Range = pg->KelvinPrimitive.SetLight[lightNum].LocalRange;
				dir.x = pg->KelvinPrimitive.SetLight[lightNum].InfiniteDirection[0];
				dir.y = pg->KelvinPrimitive.SetLight[lightNum].InfiniteDirection[1];
				dir.z = pg->KelvinPrimitive.SetLight[lightNum].InfiniteDirection[2];
				NV2A_Light8[lightNum].Direction = dir;
				bEnable = true;
				break;
			case NV097_SET_LIGHT_ENABLE_MASK_LIGHT0_LOCAL: //D3DLIGHT_POINT
				NV2A_Light8[lightNum].Type = D3DLIGHT_POINT;
				//Push1f(pPush,NV097_SET_LIGHT_LOCAL_RANGE(lightNum),	pLight->Light8.Range);
				NV2A_Light8[lightNum].Range = pg->KelvinPrimitive.SetLight[lightNum].LocalRange;
				NV2A_Light8[lightNum].Position.x = pg->KelvinPrimitive.SetLight[lightNum].LocalPosition[0];
				NV2A_Light8[lightNum].Position.y = pg->KelvinPrimitive.SetLight[lightNum].LocalPosition[1];
				NV2A_Light8[lightNum].Position.z = pg->KelvinPrimitive.SetLight[lightNum].LocalPosition[2];
				NV2A_Light8[lightNum].Attenuation0 = pg->KelvinPrimitive.SetLight[lightNum].LocalAttenuation[0];
				NV2A_Light8[lightNum].Attenuation1 = pg->KelvinPrimitive.SetLight[lightNum].LocalAttenuation[1];
				NV2A_Light8[lightNum].Attenuation2 = pg->KelvinPrimitive.SetLight[lightNum].LocalAttenuation[2];
				bEnable = true;
				break;
			case NV097_SET_LIGHT_ENABLE_MASK_LIGHT0_SPOT: //D3DLIGHT_SPOT
				NV2A_Light8[lightNum].Type = D3DLIGHT_SPOT;
				//Push1f(pPush,NV097_SET_LIGHT_LOCAL_RANGE(lightNum),	pLight->Light8.Range);
				NV2A_Light8[lightNum].Range = pg->KelvinPrimitive.SetLight[lightNum].LocalRange;
				NV2A_Light8[lightNum].Position.x = pg->KelvinPrimitive.SetLight[lightNum].LocalPosition[0];
				NV2A_Light8[lightNum].Position.y = pg->KelvinPrimitive.SetLight[lightNum].LocalPosition[1];
				NV2A_Light8[lightNum].Position.z = pg->KelvinPrimitive.SetLight[lightNum].LocalPosition[2];
				NV2A_Light8[lightNum].Attenuation0 = pg->KelvinPrimitive.SetLight[lightNum].LocalAttenuation[0];
				NV2A_Light8[lightNum].Attenuation1 = pg->KelvinPrimitive.SetLight[lightNum].LocalAttenuation[1];
				NV2A_Light8[lightNum].Attenuation2 = pg->KelvinPrimitive.SetLight[lightNum].LocalAttenuation[2];
				/*
				Explut(pLight->Light8.Falloff, &pLight->Falloff_L, &pLight->Falloff_M);
    
            pLight->Falloff_N = 1.0f + pLight->Falloff_L - pLight->Falloff_M;
                
            // Attenuate the spot direction to get falloff to work:
    
            FLOAT theta2 = Cos(0.5f * pLight->Light8.Theta);
            FLOAT phi2 = Cos(0.5f * pLight->Light8.Phi);
    
            // Handle case in which theta gets close to or overtakes phi, since 
            // hardware can't:
    
            if (phi2 >= theta2)     // Outer angle <= inner angle, oops
            {        
                // Make outer angle cosine slightly smaller:
    
                phi2 = 0.999f * theta2;  
            }
    
            pLight->Scale = nvInv(theta2 - phi2);
            pLight->W = -phi2 * pLight->Scale;
*/
				float Falloff_L = pg->KelvinPrimitive.SetLight[lightNum].SpotFalloff[0]; //todo: Falloff_L/Falloff_M/Falloff_N there should be 3 components but not found in Light8/Light9
				float Falloff_M = pg->KelvinPrimitive.SetLight[lightNum].SpotFalloff[1];
				float Falloff_N = pg->KelvinPrimitive.SetLight[lightNum].SpotFalloff[2];
				dir.x = pg->KelvinPrimitive.SetLight[lightNum].SpotDirection[0];
				dir.y = pg->KelvinPrimitive.SetLight[lightNum].SpotDirection[1];
				dir.z = pg->KelvinPrimitive.SetLight[lightNum].SpotDirection[2];
				float W = pg->KelvinPrimitive.SetLight[lightNum].SpotDirection[3]; // there is no W member of Light8
				// Fixme!!! there is no eazy way to reverse Falloff from Falloff_L/Falloff_M, so I put the default value 1.0f here.
				NV2A_Light8[lightNum].Falloff = 1.0f;
				D3DVECTOR dirNormal = dir;
				NormalizeVector3(&dirNormal);
				float Scale = dir.x / dirNormal.x; // there is no Scale member of Light8
				float phi2 = -1.0*W / Scale;
				float theta2 = phi2 + 1.0 / Scale;
				NV2A_Light8[lightNum].Theta = 2.0f*acos(theta2);
				NV2A_Light8[lightNum].Phi = 2.0f*acos(phi2);
				bEnable = true;

				break;
				
			}
			// Fixme!!! we need D3D__RenderState[D3DRS_AMBIENT] and materials set with SetMaterial()/SetBackMaterial() to reverse the light colors.
			// hack, we use colors in kelvin directly since these colors are composed with D3DRS_AMBIENT/SetMaterial/SetBackMaterial/ and the color of each light.
			NV2A_Light8[lightNum].Ambient =*(D3DCOLORVALUE*)&( pg->KelvinPrimitive.SetLight[lightNum].AmbientColor);
			NV2A_Light8[lightNum].Specular = *(D3DCOLORVALUE*)&(pg->KelvinPrimitive.SetLight[lightNum].SpecularColor);
			NV2A_Light8[lightNum].Diffuse = *(D3DCOLORVALUE*)&(pg->KelvinPrimitive.SetLight[lightNum].DiffuseColor);
			// here we calls Host D3D for SetLight() and LightEnable(). because originaly there two are called directly from HLE patch.
			HRESULT hRet = g_pD3DDevice->SetLight(lightNum, &NV2A_Light8[lightNum]);
			//DEBUG_D3DRESULT(hRet, "g_pD3DDevice->SetLight");
			hRet = g_pD3DDevice->LightEnable(lightNum, bEnable);
		}
		// Set scene ambient color,
		extern D3DCOLOR FromVector(D3DCOLORVALUE v);
		NV2A_SceneAmbient[0] = *(D3DCOLORVALUE*)&(pg->KelvinPrimitive.SetSceneAmbientColor);//NV097_SET_SCENE_AMBIENT_COLOR
		NV2A_SceneAmbient[0].a = 1.0;
		NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_AMBIENT, FromVector(NV2A_SceneAmbient[0]));
		NV2A_SceneAmbient[1] = *(D3DCOLORVALUE*)&(pg->KelvinPrimitive.SetBackSceneAmbientColor);//NV097_SET_BACK_SCENE_AMBIENT_COLOR
		NV2A_SceneAmbient[1].a = 1.0;
		NV2ARenderStates.SetXboxRenderState(xbox::X_D3DRS_BACKAMBIENT, FromVector(NV2A_SceneAmbient[1]));
		// Set scene mateiral emission and alpha
		NV2A_SceneMateirals[0].Diffuse = *(D3DCOLORVALUE*)&(pg->KelvinPrimitive.SetBackMaterialEmission);//NV097_SET_MATERIAL_EMISSION
		NV2A_SceneMateirals[0].Diffuse.a = pg->KelvinPrimitive.SetBackMaterialAlpha;//NV097_SET_MATERIAL_ALPHA
		// Fixme!!! here we set the specular power to default 1.0
		NV2A_SceneMateirals[0].Power = 1.0;
		NV2A_SceneMateirals[1].Diffuse = *(D3DCOLORVALUE*)&(pg->KelvinPrimitive.SetBackMaterialEmission);//NV097_SET_BACK_MATERIAL_EMISSION
		NV2A_SceneMateirals[1].Diffuse.a = pg->KelvinPrimitive.SetBackMaterialAlpha;//NV097_SET_BACK_MATERIAL_EMISSION
		// Fixme!!! here we set the specular power to default 1.0
		NV2A_SceneMateirals[1].Power = 1.0;
	}
	return;

}

void D3D_draw_state_update(NV2AState *d)
{
	PGRAPHState *pg = &d->pgraph;
	HRESULT hRet;

	// update point params, Xbox takes everything from texture stage 3
	// hack since we're using all pgraph processing now.
	
	if ((NV2A_DirtyFlags & X_D3DDIRTYFLAG_POINTPARAMS) != 0) {
		CxbxrImpl_LazySetPointParameters(d);
		// clear dirty flag
		NV2A_DirtyFlags &= ~X_D3DDIRTYFLAG_POINTPARAMS;
	}

	// update combiners, combiners must be update prior to pixel shader, because we have to compose colorOp before we compose fix funtion pixel shaders.
	// only update combiners when in fixed pixel shader.
	if (pNV2A_PixelShader == nullptr) {
		if ((NV2A_DirtyFlags & X_D3DDIRTYFLAG_COMBINERS) != 0) {
			// only update the combiner state when we're in fixed function pixel shader
			if (pNV2A_PixelShader == nullptr)
				CxbxrImpl_LazySetCombiners(d);
			// clear dirty flag
			NV2A_DirtyFlags &= ~X_D3DDIRTYFLAG_COMBINERS;
		}
	}

	// update texture stage texture states, texture stage texture states must be update prior to pixel shader because pixel shader compilation depends on texture state input
	if ((NV2A_DirtyFlags & X_D3DDIRTYFLAG_TEXTURE_STATE) != 0) {
		CxbxrImpl_LazySetTextureState(d);
		// clear dirty flag
		NV2A_DirtyFlags &= ~X_D3DDIRTYFLAG_TEXTURE_STATE;
	}

	// update spec fog combiner state

	if ((NV2A_DirtyFlags & X_D3DDIRTYFLAG_SPECFOG_COMBINER) != 0) {
		CxbxrImpl_LazySetSpecFogCombiner(d);
		// clear dirty flag
		NV2A_DirtyFlags &= ~X_D3DDIRTYFLAG_SPECFOG_COMBINER;
	}

	// update pixel shader
	if ((NV2A_DirtyFlags & X_D3DDIRTYFLAG_SHADER_STAGE_PROGRAM) != 0) {
		// only update pixel shader when in pushbuffer replay mode, this is to solve the HLE/NV2A confliction 
		//if ((NV2A_stateFlags & X_STATE_RUNPUSHBUFFERWASCALLED) !=0) {

		CxbxrImpl_LazySetShaderStageProgram(d);
		//}

		// clear dirty flag xbox::dword_xt
		NV2A_DirtyFlags &= ~X_D3DDIRTYFLAG_SHADER_STAGE_PROGRAM;
	}

	// update texture transfoms
	if (NV2A_DirtyFlags & X_D3DDIRTYFLAG_TEXTURE_TRANSFORM) {
		CxbxrImpl_LazySetTextureTransform(d);
		NV2A_DirtyFlags &= ~X_D3DDIRTYFLAG_TEXTURE_TRANSFORM;
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
			if ((NV2A_stateFlags & X_STATE_RUNPUSHBUFFERWASCALLED) != 0) {
				xbox::X_D3DVIEWPORT8 Viewport;
				CxbxrGetViewport(Viewport);
				CxbxrImpl_SetViewport(&Viewport);
			}
			NV2A_viewport_dirty = false;
		}
	//}

	// update transfoms
	// update transform matrix using NV2A KevlvinPrimitive contents if we're in direct ModelView transform mode.
	if(NV2A_DirtyFlags & X_D3DDIRTYFLAG_TRANSFORM){
		CxbxrImpl_LazySetTransform(d);
		NV2A_DirtyFlags &= ~X_D3DDIRTYFLAG_TRANSFORM;
	}
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
				// Detect disabled slots by their format (0x02 : size count zero, type float) :
				if (SortedAttributes[i].size_and_type == xbox::X_D3DVSDT_NONE){
					SortedAttributes[i].offset = 0xFFFFFFFF; // Make sure disabled slots get sorted to the end
				}
				else {
					// TODO ? Reword : for draw_mode == DrawMode::InlineArray, only KelvinPrimitive.SetVertexDataArrayFormat[] was set/update. we have to compose the vertex offset ourselves?
					SortedAttributes[i].offset = pg->KelvinPrimitive.SetVertexDataArrayOffset[i];
				}
				assert(SortedAttributes[i].stride > 0);
				assert(SortedAttributes[i].stride <= D3D_inline_vertex_stride); // TODO : replace with actual NV2A maximum stride, might be 2048?
				break;
			}
		}

	}
	// Only necessary for pg->draw_mode == DrawMode::DrawArrays || pg->draw_mode == DrawMode::InlineElements, and the overhead must be reduced.
	if (pg->draw_mode == DrawMode::DrawArrays || pg->draw_mode == DrawMode::InlineElements) {
		// Sort out intermediate attribute slot array on memory offset (and on slot, to guarantee deterministic ordering when offsets overlap) :
		if (pg->draw_mode == DrawMode::InlineElements) {
			SortedAttributes[0].size_and_type = SortedAttributes[0].size_and_type;
		}

		std::sort(/*First=*/&SortedAttributes[0], /*Last=*/&SortedAttributes[X_VSH_MAX_ATTRIBUTES - 1], /*Pred=*/[](const auto& x, const auto& y)
			{ return std::tie(x.offset, x.slot_index) < std::tie(y.offset, y.slot_index); });

		assert(SortedAttributes[0].size_and_type != xbox::X_D3DVSDT_NONE); // After sorting, verify at least the first slot is in use (more is fine, none is bad)

		// Store stream zero's stride for possible use later on, D3D_StreamZeroStride was already set in other two modes DrawMode::InlineArray and DrawMode::InlineBuffer
		D3D_StreamZeroStride = SortedAttributes[0].stride;
	
		// Using our sorted attibutes, derive the g_NV2AVertexAttributeFormat slot values :
		DWORD current_stream_index = 0;
		D3D_Xbox_StreamCount = 0;
		uint32_t group_offset = SortedAttributes[0].offset;
		uint32_t group_stride = SortedAttributes[0].stride;
		//store the vertex buffer address of stream[current_stream_index]
		if (SortedAttributes[0].size_and_type != xbox::X_D3DVSDT_NONE) {
			D3D_Xbox_StreamSource[current_stream_index].VertexBuffer = (xbox::X_D3DVertexBuffer  *)group_offset;
			D3D_Xbox_StreamSource[current_stream_index].Stride = group_stride;
			D3D_Xbox_StreamCount++;
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
					D3D_Xbox_StreamSource[current_stream_index].VertexBuffer = (xbox::X_D3DVertexBuffer  *)group_offset;
					D3D_Xbox_StreamSource[current_stream_index].Stride = group_stride;
					D3D_Xbox_StreamCount++;
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
	// update other D3D states
	// FIXME!! CxbxUpdateNativeD3DResources() calls XboxRenderStates.Apply() and XboxTextureStates.Apply() to apply Xbox D3D states to Host.
	// so what we set to host here might be over write later. safer way is to update XboxTextureStates/XboxTextureStates and let CxbxUpdateNativeD3DResources() update them to host.
	// only update state to host when we're in pushbuffer replay mode. other wise HLE shall handle the host update task.
	//if ((NV2A_stateFlags & X_STATE_RUNPUSHBUFFERWASCALLED) != 0) {
	//}
	g_xbox_transform_use_DirectModelView = true;
	CxbxUpdateNativeD3DResources();
	g_xbox_transform_use_DirectModelView = false;
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

	for (unsigned array_index = 0; array_index < pg->draw_arrays_length; array_index++) {
		//the address in pg->KelvinPrimitive.SetVertexDataArrayOffset[] are offsets from VRAM base 0x80000000, we have to add the base address to get full address.
		//this is only assuming there was only one vertex buffer and the SetVertexDataArrayOffset[0] is the starting address of the vertex buffer. this code should be revised once we finish the vertex buffer lookup code in D3D_draw_state_update()
		DrawContext.pXboxVertexStreamZeroData = (PVOID)(pg->KelvinPrimitive.SetVertexDataArrayOffset[0] + CONTIGUOUS_MEMORY_BASE);
		DrawContext.pXboxIndexData = nullptr;
		DrawContext.dwVertexCount = pg->gl_draw_arrays_count[array_index];
		DrawContext.dwStartVertex = pg->gl_draw_arrays_start[array_index];
		//because CxbxDrawPrimitiveUP() can only handle dwStartVertex == 0, so we shift the pXboxVertexStreamZeroData to where dwStartVertex is, and reset dwStartVertex to 0.
		DrawContext.pXboxVertexStreamZeroData = (PVOID)((DWORD)DrawContext.pXboxVertexStreamZeroData + DrawContext.uiXboxVertexStreamZeroStride*DrawContext.dwStartVertex);
		DrawContext.dwStartVertex = 0;
		// draw arrays 
		CxbxDrawPrimitiveUP(DrawContext);
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
		//inline array
		CxbxDrawPrimitiveUP(DrawContext);

	//}
}

void D3D_draw_inline_elements(NV2AState *d)
{
	PGRAPHState *pg = &d->pgraph;

	CxbxDrawContext DrawContext = {};
	DrawContext.XboxPrimitiveType = (xbox::X_D3DPRIMITIVETYPE)pg->primitive_mode;
	DrawContext.uiXboxVertexStreamZeroStride = (pg->KelvinPrimitive.SetVertexDataArrayFormat[0] >> 8); // NV097_SET_VERTEX_DATA_ARRAY_FORMAT_STRIDE
	DrawContext.pXboxVertexStreamZeroData = (PVOID)(pg->KelvinPrimitive.SetVertexDataArrayOffset[0] + CONTIGUOUS_MEMORY_BASE);
	DrawContext.dwVertexCount = pg->inline_elements_length;
	DrawContext.dwStartVertex = 0;
	DrawContext.pXboxIndexData = d->pgraph.inline_elements;

	CxbxDrawIndexed(DrawContext);

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
                continue; // while
            case COMMAND_TYPE_CALL: // Note : NV2A call is used as jump, the sub_return is used as a flag to indicate where the jump was orginated.
                if (subr_active) {
                    LOG_TEST_CASE("Pushbuffer COMMAND_TYPE_CALL while another call was active!");
                    // TODO : throw DMA_PUSHER(CALL_SUBR_ACTIVE);
                    goto finish; // For now, don't even attempt to run through
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
