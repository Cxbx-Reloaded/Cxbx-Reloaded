// Source : https://github.com/xqemu/xqemu/blob/master/hw/xbox/nv2a_int.h
/*
 * QEMU Geforce NV2A internal definitions
 *
 * Copyright (c) 2012 espes
 * Copyright (c) 2015 Jannik Vogel
 * Copyright (c) 2018 Matt Borgerson
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 or
 * (at your option) version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses/>.
 */

#ifndef HW_NV2A_INT_H
#define HW_NV2A_INT_H

#undef USE_SHADER_CACHE

#include <queue>
#include <thread>
#include <GL/glew.h>

#include "xbox_types.h" // For xbox::addr_xt

#include "qemu-thread.h" // For qemu_mutex, etc

#ifdef USE_SHADER_CACHE
#include "glib_compat.h" // For GHashTable, g_hash_table_new, g_hash_table_lookup, g_hash_table_insert
#endif
#include "common\util\gloffscreen\gloffscreen.h" // For GloContext, etc

#include "swizzle.h"

#include "nv2a_debug.h" // For HWADDR_PRIx, NV2A_DPRINTF, NV2A_GL_DPRINTF, etc.
#include "nv2a_shaders.h" // For ShaderBinding, etc
#include "nv2a_regs.h" // For NV2A_MAX_TEXTURES, etc





typedef xbox::addr_xt hwaddr; // Compatibility; Cxbx uses xbox::addr_xt, xqemu and OpenXbox use hwaddr 
typedef uint32_t value_t; // Compatibility; Cxbx values are uint32_t (xqemu and OpenXbox use uint64_t)

#define NV_PMC_SIZE                 0x001000
#define _NV_PFIFO_SIZE              0x002000 // Underscore prefix to prevent clash with NV_PFIFO_SIZE
#define NV_PVIDEO_SIZE              0x001000
#define NV_PTIMER_SIZE              0x001000
#define NV_PFB_SIZE                 0x001000
#define NV_PGRAPH_SIZE              0x002000
#define NV_PCRTC_SIZE               0x001000
#define NV_PRAMDAC_SIZE             0x001000

#define VSH_TOKEN_SIZE 4 // Compatibility; TODO : Move this to nv2a_vsh.h
#define MAX(a,b) ((a)>(b) ? (a) : (b)) // Compatibility
#define MIN(a,b) ((a)<(b) ? (a) : (b)) // Compatibility

#define g_free(x) free(x) // Compatibility
#define g_malloc(x) malloc(x) // Compatibility
#define g_malloc0(x) calloc(1, x) // Compatibility
#define g_realloc(x, y) realloc(x, y) // Compatibility

#undef USE_TEXTURE_CACHE

#if __cplusplus >= 201402L
#  define NV2A_CONSTEXPR constexpr
#else
#  define NV2A_CONSTEXPR static
#endif

// GCC implementation of FFS
static int ffs(int valu)
{
	int bit;

	if (valu == 0)
		return 0;

	for (bit = 1; !(valu & 1); bit++)
		valu >>= 1;

	return bit;
}

#define GET_MASK(v, mask) (((v) & (mask)) >> (ffs(mask)-1))

#define SET_MASK(v, mask, val)                            \
    do {                                                  \
        const unsigned int __val = (val);                 \
        const unsigned int __mask = (mask);               \
        (v) &= ~(__mask);                                 \
        (v) |= ((__val) << (ffs(__mask) - 1)) & (__mask); \
    } while (0)

// Power-of-two CASE statements
#define CASE_1(v, step) case (v)
#define CASE_2(v, step) CASE_1(v, step) : CASE_1(v + (step) * 1, step)
#define CASE_4(v, step) CASE_2(v, step) : CASE_2(v + (step) * 2, step)
#define CASE_8(v, step) CASE_4(v, step) : CASE_4(v + (step) * 4, step)
#define CASE_16(v, step) CASE_8(v, step) : CASE_8(v + (step) * 8, step)
#define CASE_32(v, step) CASE_16(v, step) : CASE_16(v + (step) * 16, step)
#define CASE_64(v, step) CASE_32(v, step) : CASE_32(v + (step) * 32, step)
#define CASE_128(v, step) CASE_64(v, step) : CASE_64(v + (step) * 64, step)
#define CASE_256(v, step) CASE_128(v, step) : CASE_128(v + (step) * 128, step)

// Non-power-of-two CASE statements
#define CASE_3(v, step) CASE_2(v, step) : CASE_1(v + (step) * 2, step)
#define CASE_6(v, step) CASE_4(v, step) : CASE_2(v + (step) * 4, step)

#define NV2A_DEVICE(obj) \
    OBJECT_CHECK(NV2AState, (obj), "nv2a")

//void reg_log_read(int block, hwaddr addr, uint64_t val);
//void reg_log_write(int block, hwaddr addr, uint64_t val);

enum FIFOEngine {
	ENGINE_SOFTWARE = 0,
	ENGINE_GRAPHICS = 1,
	ENGINE_DVD = 2,
};

typedef struct DMAObject {
	unsigned int dma_class;
	unsigned int dma_target;
	xbox::addr_xt address;
	xbox::addr_xt limit;
} DMAObject;

typedef struct VertexAttribute {
	//=0x80000000 if the bbit 31 of vertex buffer address with offset  is set to 1.
	//we're using UMA, maybe we can directly use 0x8XXXXXXX range?
	bool dma_select;
	//vertex attribute vertex buffer address with offset
	xbox::addr_xt offset;

	/* inline arrays are packed in order?
	* Need to pass the offset to converted attributes */
	unsigned int inline_array_offset;

	// float inline_value[4]; replaced by pg->KelvinPrimitive.SetVertexData4f[].M

	//flag to indicate whether the attribute is set during attribute upload process. 
	bool set_by_inline_buffer = false; // Note : for now, this bool is identical to (inline_buffer != nullptr)

	unsigned int format;
	unsigned int size; /* size of the data type */
	unsigned int count; /* number of components */
	uint32_t stride;

	bool needs_conversion;
	uint8_t *converted_buffer=nullptr;
	unsigned int converted_elements;
	unsigned int converted_size;
	unsigned int converted_count;

	float *inline_buffer;

	GLint gl_count;
	GLenum gl_type;
	GLboolean gl_normalize;

	GLuint gl_converted_buffer;
	GLuint gl_inline_buffer;
} VertexAttribute;

typedef struct Surface {
	bool draw_dirty;
	bool buffer_dirty;
	bool write_enabled_cache;
	unsigned int pitch;

	xbox::addr_xt offset;
} Surface;

typedef struct SurfaceShape {
	unsigned int z_format;
	unsigned int color_format;
	unsigned int zeta_format;
	unsigned int log_width, log_height;
	unsigned int clip_x, clip_y;
	unsigned int clip_width, clip_height;
	unsigned int anti_aliasing;
} SurfaceShape;

typedef struct TextureShape {
	bool cubemap;
	unsigned int dimensionality;
	unsigned int color_format;
	unsigned int levels;
	unsigned int width, height, depth;

	unsigned int min_mipmap_level, max_mipmap_level;
	unsigned int pitch;
} TextureShape;

typedef struct TextureKey {
	TextureShape state;
	uint64_t data_hash;
	uint8_t* texture_data;
	uint8_t* palette_data;
} TextureKey;

typedef struct TextureBinding {
	GLenum gl_target;
	GLuint gl_texture;
	unsigned int refcnt;
} TextureBinding;

typedef struct KelvinState {
	xbox::addr_xt object_instance;
} KelvinState;

typedef struct ContextSurfaces2DState {
	xbox::addr_xt object_instance;
	xbox::addr_xt dma_image_source;
	xbox::addr_xt dma_image_dest;
	unsigned int color_format;
	unsigned int source_pitch, dest_pitch;
	xbox::addr_xt source_offset, dest_offset;
} ContextSurfaces2DState;

typedef struct ImageBlitState {
	xbox::addr_xt object_instance;
	xbox::addr_xt context_surfaces;
	unsigned int operation;
	unsigned int in_x, in_y;
	unsigned int out_x, out_y;
	unsigned int width, height;
} ImageBlitState;

typedef struct NV097KelvinPrimitive{
	uint32_t Unknown_0000;				//0x00000000
	uint32_t Rev_0004[0xfc / 4];		//Reserved for NV_PGRAPH_XXX registers
	uint32_t NoOperation;				//0x00000100
	uint32_t Notify;
	uint32_t SetWarningEnable;
	uint32_t GetState;
	uint32_t WaitForIdle;
	uint32_t Rev_0114[0xc / 4];
	uint32_t SetFlipRead;
	uint32_t SetFlipWrite;
	uint32_t SetFlipModulo;
	uint32_t FlipIncrementWrite;
	uint32_t FlipStall;
	uint32_t Rev_0134[0xc / 4];
	uint32_t PmTrigger;
	uint32_t Rev_0144[0x3c / 4];
	uint32_t SetContextDmaNotifies;
	uint32_t SetContextDmaA;
	uint32_t SetContextDmaB;
	uint32_t Rev_018c[0x4 / 4];
	uint32_t SetContextDmaState;
	uint32_t SetContextDmaColor;
	uint32_t SetContextDmaZeta;
	uint32_t SetContextDmaVertexA;
	uint32_t SetContextDmaVertexB;
	uint32_t SetContextDmaSemaphore;
	uint32_t SetContextDmaReport;
	uint32_t Rev_01ac[0x54 / 4];
	uint32_t SetSurfaceClipHorizontal;	//0x00000200
	uint32_t SetSurfaceClipVertical;	//0x00000204
	uint32_t SetSurfaceFormat;
	uint32_t SetSurfacePitch;
	uint32_t SetSurfaceColorOffset;
	uint32_t SetSurfaceZetaOffset;
	uint32_t Rev_0218[0x48 / 4];
	uint32_t SetCombinerAlphaICW[8];	//0x00000260
	uint32_t Rev_0280[0x8 / 4];
	uint32_t SetCombinerSpecularFogCW0; //0x00000288
	uint32_t SetCombinerSpecularFogCW1; //0x0000028C
	uint32_t SetControl0;
	uint32_t SetLightControl;
	uint32_t SetColorMaterial;
	uint32_t SetFogMode;
	uint32_t SetFogGenMode;
	uint32_t SetFogEnable;
	uint32_t SetFogColor;
	uint32_t Rev_02ac[0x8 / 4];
	uint32_t SetWindowClipType;			//0x000002B4
	uint32_t Rev_02b8[0x8 / 4];
	uint32_t SetWindowClipHorizontal[8];//0x000002C0
	uint32_t SetWindowClipVertical[8];	//0x000002E0
	uint32_t SetAlphaTestEnable;		//0x00000300
	uint32_t SetBlendEnable;			//0x00000304
	uint32_t SetCullFaceEnable;			//0x00000308
	uint32_t SetDepthTestEnable;
	uint32_t SetDitherEnable;
	uint32_t SetLightingEnable;
	uint32_t SetPointParamsEnable;
	uint32_t SetPointSmoothEnable;
	uint32_t SetLineSmoothEnable;
	uint32_t SetPolySmoothEnable;
	uint32_t SetSkinMode;
	uint32_t SetStencilTestEnable;
	uint32_t SetPolyOffsetPointEnable;
	uint32_t SetPolyOffsetLineEnable;
	uint32_t SetPolyOffsetFillEnable;
	uint32_t SetAlphaFunc;
	uint32_t SetAlphaRef;
	uint32_t SetBlendFuncSfactor;
	uint32_t SetBlendFuncDfactor;
	uint32_t SetBlendColor;
	uint32_t SetBlendEquation;
	uint32_t SetDepthFunc;
	uint32_t SetColorMask;
	uint32_t SetDepthMask;
	uint32_t SetStencilMask;
	uint32_t SetStencilFunc;
	uint32_t SetStencilFuncRef;
	uint32_t SetStencilFuncMask;
	uint32_t SetStencilOpFail;
	uint32_t SetStencilOpZfail;
	uint32_t SetStencilOpZpass;
	uint32_t SetShadeMode;
	uint32_t SetLineWidth;
	float SetPolygonOffsetScaleFactor;
	float SetPolygonOffsetBias;
	uint32_t SetFrontPolygonMode;
	uint32_t SetBackPolygonMode;
	float SetClipMin;
	float SetClipMax;
	uint32_t SetCullFace;
	uint32_t SetFrontFace;
	uint32_t SetNormalizationEnable;
	float SetMaterialEmission[3];		//0x000003A8
	float SetMaterialAlpha;
	uint32_t SetSpecularEnable;
	uint32_t SetLightEnableMask;
	struct {
		uint32_t S;
		uint32_t T;
		uint32_t R;
		uint32_t Q;
	} SetTexgen[4];						//0x000003C0
	uint32_t Rev_0400[0x20 / 4];
	uint32_t SetTextureMatrixEnable[4];	//0x00000420
	uint32_t Rev_0430[0xc / 4];
	uint32_t SetPointSize;				//0x0000043C
	float SetProjectionMatrix[16];		//0x00000440
	float SetModelViewMatrix0[16];		//0x00000480
	float SetModelViewMatrix1[16];		//0x000004C0
	float SetModelViewMatrix2[16];		//0x00000500
	float SetModelViewMatrix3[16];		//0x00000540
	float SetInverseModelViewMatrix0[16];	//0x00000580
	float SetInverseModelViewMatrix1[16];	//0x000005C0
	float SetInverseModelViewMatrix2[16];	//0x00000600
	float SetInverseModelViewMatrix3[16];	//0x00000640
	float SetCompositeMatrix[16];		//0x00000680
	float SetTextureMatrix0[16];		//0x000006C0
	float SetTextureMatrix1[16];		//0x00000700
	float SetTextureMatrix2[16];		//0x00000740
	float SetTextureMatrix3[16];		//0x00000780
	uint32_t Rev_07c0[0x80 / 4];
	struct {
		float S[4];
		float T[4];
		float R[4];
		float Q[4];
	} SetTexgenPlane[4];				//0x00000840
	uint32_t Rev_0940[0x80 / 4];
	float SetFogParams[3];				//0x000009C0
	uint32_t SetTexgenViewModel;
	float SetFogPlane[4];				//0x000009D0
	float SetSpecularParams[6];			//0x000009E0
	uint32_t SetSwathWidth;
	uint32_t SetFlatShadeOp;
	uint32_t Rev_0a00[0x10 / 4];		//reserved for NV_PGRAPH_CONTROL_0~3
	float SetSceneAmbientColor[3];		//0x00000A10
	uint32_t Rev_0a1c[0x4 / 4];
	float SetViewportOffset[4];			//0x00000A20
	float SetPointParams[8];			//0x00000A30
	float SetEyePosition[4];			//0x00000A50
	uint32_t SetCombinerFactor0[8];		//0x00000A60
	uint32_t SetCombinerFactor1[8];		//0x00000A80
	uint32_t SetCombinerAlphaOCW[8];	//0x00000AA0
	uint32_t SetCombinerColorICW[8];	//0x00000AC0
	uint32_t SetColorKeyColor[4];		//0x00000AE0
	float SetViewportScale[4];			//0x00000AF0
	uint32_t SetTransformProgram[32];	//0x00000B00
	float SetTransformConstant[32];		//0x00000B80
	struct {
		float AmbientColor[3];
		float DiffuseColor[3];
		float SpecularColor[3];
		uint32_t Rev_0c24[0x1c / 4];//dd (7)
	} SetBackLight[8];					//0x00000C00
	uint32_t Rev_0e00[0x200 / 4];
	struct {
		float AmbientColor[3];
		float DiffuseColor[3];
		float SpecularColor[3];
		float LocalRange;
		float InfiniteHalfVector[3];
		float InfiniteDirection[3];
		float SpotFalloff[3];
		float SpotDirection[4];
		float LocalPosition[3];
		float LocalAttenuation[3];
		uint32_t Rev_1074[0xc / 4];
	} SetLight[8];						//0x00001000
	uint32_t Rev_1400[0x7c / 4];
	uint32_t SetStippleControl;
	uint32_t SetStipplePattern[32];		//0x00001480
	float SetVertex3f[3];				//0x00001500
	uint32_t Rev_150c[0xc / 4];
	float SetVertex4f[4];				//0x00001518
	int32_t SetVertex4s[2];				//0x00001528
	float SetNormal3f[3];				//0x00001530
	uint32_t Rev_153c[0x4 / 4];
	int32_t SetNormal3s[2];				//0x00001540
	uint32_t Rev_1548[0x8 / 4];
	float SetDiffuseColor4f[4];			//0x00001550
	float SetDiffuseColor3f[3];			//0x00001560
	uint32_t SetDiffuseColor4ub;
	float SetSpecularColor4f[4];		//0x00001570
	float SetSpecularColor3f[3];		//0x00001580
	uint32_t SetSpecularColor4ub;
	float SetTexcoord0_2f[2];			//0x00001590
	int32_t SetTexcoord0_2s;
	uint32_t Rev_159c[0x4 / 4];
	float SetTexcoord0_4f[4];			//0x000015A0
	int32_t SetTexcoord0_4s[2];			//0x000015B0
	float SetTexcoord1_2f[2];			//0x000015B8
	int32_t SetTexcoord1_2s;
	uint32_t Rev_15c4[0x4 / 4];
	float SetTexcoord1_4f[4];			//0x000015C8
	int32_t SetTexcoord1_4s[2];			//0x000015D8
	float SetTexcoord2_2f[2];			//0x000015E0
	int32_t SetTexcoord2_2s;
	uint32_t Rev_15ec[0x4 / 4];
	float SetTexcoord2_4f[4];			//0x000015F0
	int32_t SetTexcoord2_4s[2];			//0x00001600
	float SetTexcoord3_2f[2];			//0x00001608
	int32_t SetTexcoord3_2s;
	uint32_t Rev_1614[0xc / 4];
	float SetTexcoord3_4f[4];			//0x00001620
	int32_t SetTexcoord3_4s[2];			//0x00001630
	uint32_t Rev_1638[0x60 / 4];
	float SetFog1f;
	float SetWeight1f;
	float SetWeight2f[2];				//0x000016A0
	uint32_t Rev_16a8[0x8 / 4];
	float SetWeight3f[3];				//0x000016B0
	uint32_t SetEdgeFlag;
	float SetWeight4f[4];				//0x000016C0
	float SetTransformFixedConst3[4];	//0x000016D0
	float SetTransformFixedConst0[4];	//0x000016E0
	float SetTransformFixedConst1[4];	//0x000016F0
	float SetTransformFixedConst2[4];	//0x00001700
	uint32_t InvalidateVertexCacheFile;
	uint32_t InvalidateVertexFile;
	uint32_t TlNop;
	uint32_t TlSync;
	uint32_t SetVertexDataArrayOffset[16];	//0x00001720 See OpenGL vertex_attributes.offset + CONTIGUOUS_MEMORY_BASE
	uint32_t SetVertexDataArrayFormat[16];	//0x00001760 See OpenGL vertex_attributes.format and .size and .stride
	float SetBackSceneAmbientColor[3];	//0x000017A0
	float SetBackMaterialAlpha;
	float SetBackMaterialEmission[3];	//0x000017B0
	uint32_t SetLogicOpEnable;
	uint32_t SetLogicOp;
	uint32_t SetTwoSidedLightEn;
	uint32_t ClearReportValue;
	uint32_t SetZpassPixelCountEnable;
	uint32_t GetReport;
	float SetTLConstZero[3];			//0x000017D4
	float SetEyeDirection[3];			//0x000017E0
	float SetLinearFogConst[3];			//0x000017EC
	uint32_t SetShaderClipPlaneMode;
	uint32_t SetBeginEnd;
	uint32_t ArrayElement16;			//0x00001800
	uint32_t Rev_1804[0x4 / 4];
	uint32_t ArrayElement32;			//0x00001808
	uint32_t Rev_180c[0x4 / 4];
	uint32_t DrawArrays;				//0x00001810
	uint32_t Rev_1814[0x4 / 4];
	uint32_t InlineArray;				//0x00001818
	float SetEyeVector[3];				//0x0000181C
	uint32_t InlineVertexReuse;
	uint32_t Rev_182c[0x54 / 4];
	struct {
		float M[2];
	} SetVertexData2f[16];				//0x00001880
	int32_t SetVertexData2s[16];		//0x00001900
	int32_t SetVertexData4ub[16];		//0x00001940
	struct {
		int32_t M[2];
	} SetVertexData4s[16];				//0x00001980
	struct {
		float M[4];
	} SetVertexData4f[16];				//0x00001A00
	struct {
		uint32_t Offset;					//	0x00001B00 +i*0x40
		uint32_t Format;					//	0x00001B04 +i*0x40
		uint32_t Address;					//	0x00001B08 +i*0x40
		uint32_t Control0;					//	0x00001B0C +i*0x40
		uint32_t Control1;					//	0x00001B10 +i*0x40
		uint32_t Filter;					//	0x00001B14 +i*0x40
		uint32_t Rev_1b18[0x4 / 4];			//	0x00001B18 +i*0x40 			
		uint32_t ImageRect;					//	0x00001B1C +i*0x40
		uint32_t Palette;					//	0x00001B20 +i*0x40
		uint32_t BorderColor;				//	0x00001B24 +i*0x40
		//shall we make these 4 bump vars union bump_env_matrix[4]?
		float SetBumpEnvMat00;				//	0x00001B28 +i*0x40
		float SetBumpEnvMat01;				//	0x00001B2C +i*0x40
		float SetBumpEnvMat11;				//	0x00001B30 +i*0x40
		float SetBumpEnvMat10;				//	0x00001B34 +i*0x40
		float SetBumpEnvScale;				//	0x00001B38 +i*0x40
		float SetBumpEnvOffset;				//	0x00001B3C +i*0x40
	} SetTexture[4];						//0x00001B00
	uint32_t Rev_1c00[0x164 / 4];
	uint32_t ParkAttribute;					//0x00001D64
	uint32_t UnparkAttribute;
	uint32_t SetSemaphoreOffset;
	uint32_t BackEndWriteSemaphoreRelease;
	uint32_t TextureReadSemaphoreRelease;
	uint32_t SetZMinMaxControl;
	uint32_t SetAntiAliasingControl;
	uint32_t SetCompressZBufferEn;
	uint32_t SetOccludeZStencilEn;
	uint32_t Rev_1d88[0x4 / 4];
	uint32_t SetZStencilClearValue;			//0x 00001D8C
	uint32_t SetColorClearValue;
	uint32_t ClearSurface;
	uint32_t SetClearRectHorizontal;
	uint32_t SetClearRectVertical;			//0x00001D9C
	uint32_t Rev_1da0[0x40 / 4];
	uint32_t SetBeginPatch0;				//0x00001DE0
	uint32_t SetBeginPatch1;				//0x
	uint32_t SetBeginPatch2;				//0x
	uint32_t SetBeginPatch3;				//0x
	uint32_t SetEndPatch;					//0x00001DF0
	uint32_t SetBeginEndSwatch;
	uint32_t SetBeginEndCurve;
	uint32_t Rev_1dfc[0x4 / 4];
	float SetCurveCoefficients[4];			//0x00001E00
	uint32_t SetBeginTransition0;
	uint32_t SetBeginTransition1;
	uint32_t SetBeginTransition2;
	uint32_t SetEndTransition;
	uint32_t SetSpecularFogFactor[2];		//0x00001E20
	float SetBackSpecularParams[6];			//0x00001E28
	uint32_t SetCombinerColorOCW[8];		//0x00001E40
	uint32_t SetCombinerControl;
	uint32_t Rev_1e64[0x4 / 4];
	float SetShadowZSlopeThreshold;			//0x00001E68
	uint32_t SetShadowDepthFunc;
	uint32_t SetShaderStageProgram;
	uint32_t SetDotRGBMapping;
	uint32_t SetShaderOtherStageInput;		//0x00001E78
	uint32_t Rev_1e7c[0x4 / 4];				//0x00001E7C reserved for NV_PGRAPH_BLEND
	float SetTransformData[4];				//0x00001E80
	uint32_t LaunchTransformProgram;
	uint32_t SetTransformExecutionMode;
	uint32_t SetTransformProgramCxtWriteEn;
	uint32_t SetTransformProgramLoad;
	uint32_t SetTransformProgramStart;
	uint32_t SetTransformConstantLoad;		//0x00001EA4
	uint32_t Rev_1ea8[0x118 / 4];
	uint32_t DebugInit[10];					//0x00001FC0
	uint32_t Rev_1fe8[0x18 / 4];			//0x00001fe8 Reserved for NV_PGRAPH_XXX registers
} NV097KelvinPrimitive;

typedef union {
	/* https://envytools.readthedocs.io/en/latest/hw/fifo/dma-pusher.html#the-commands-pre-gf100-format

	    123 12345678901 12 123 12345678901 12
		000 CCCCCCCCCCC 00 SSS MMMMMMMMMMM 00	increasing methods [NV4+]
		000 00000000000 10 000 00000000000 00	return [NV1A+, NV4-style only]
		001 JJJJJJJJJJJ JJ JJJ JJJJJJJJJJJ 00	old jump [NV4+, NV4-style only]
		010 CCCCCCCCCCC 00 SSS MMMMMMMMMMM 00	non-increasing methods [NV10+]
		JJJ JJJJJJJJJJJ JJ JJJ JJJJJJJJJJJ 01	jump [NV1A+, NV4-style only]
		JJJ JJJJJJJJJJJ JJ JJJ JJJJJJJJJJJ 10	call [NV1A+, NV4-style only]

		C = method Count, S = Subchannel, M = first Method, J = Jump address
	*/
	// Entire 32 bit command word, and an overlay for the above use-cases :
	uint32_t            word;                    /*  0 .. 31 */
	struct {
		uint32_t        type : 2;        /*  0 ..  1 */
			// See https://envytools.readthedocs.io/en/latest/hw/fifo/dma-pusher.html#nv4-control-flow-commands
#define COMMAND_TYPE_NONE        0
#define COMMAND_TYPE_JUMP_LONG   1
#define COMMAND_TYPE_CALL        2
		uint32_t        method : 11;       /*  2 .. 12 */
		uint32_t        subchannel : 3;        /* 13 .. 15 */
		uint32_t        flags : 2;        /* 16 .. 17 */
			// See https://envytools.readthedocs.io/en/latest/hw/fifo/dma-pusher.html#nv4-method-submission-commands
#define COMMAND_FLAGS_NONE                         0
#define COMMAND_FLAGS_SLI_CONDITIONAL              1 // (NV40+)
#define COMMAND_FLAGS_RETURN                       2
#define COMMAND_FLAGS_LONG_NON_INCREASING_METHODS  3 // [IB-mode only] 
		uint32_t        method_count : 11;       /* 18 .. 28 */
		uint32_t        instruction : 3;        /* 29 .. 31 */
#define COMMAND_INSTRUCTION_INCREASING_METHODS     0
#define COMMAND_INSTRUCTION_JUMP                   1
#define COMMAND_INSTRUCTION_NON_INCREASING_METHODS 2
	};
#define COMMAND_WORD_MASK_METHOD    0x00001FFC /*  2 .. 12 */ // See NV_PFIFO_CACHE1_DMA_STATE_METHOD, PUSH_METHOD_MASK
#define COMMAND_WORD_MASK_JUMP      0x1FFFFFFC /*  2 .. 31 */
#define COMMAND_WORD_MASK_JUMP_LONG 0xFFFFFFFC /*  2 .. 28 */
} nv_fifo_command;

typedef enum class _DrawMode {
	None,
	DrawArrays,
	InlineArray,
	InlineElements,
	InlineBuffer,
} DrawMode;

typedef struct PGRAPHState {
	bool opengl_enabled; // == bLLE_GPU
	QemuMutex pgraph_lock;

	uint32_t pending_interrupts;
	uint32_t enabled_interrupts;
	QemuCond interrupt_cond;

	/* subchannels state we're not sure the location of... */
	ContextSurfaces2DState context_surfaces_2d;
	ImageBlitState image_blit;
	KelvinState kelvin;

	QemuCond fifo_access_cond;
	QemuCond flip_3d;
	//KelvinPrimitive.SetContextDmaColor ,  KelvinPrimitive.SetContextDmaZeta
	//xbox::addr_xt dma_color, dma_zeta;
	Surface surface_color, surface_zeta;
	unsigned int surface_type;
	SurfaceShape surface_shape;
	SurfaceShape last_surface_shape;
	//use pg->KelvinPrimitive.SetContextDmaA , pg->KelvinPrimitive.SetContextDmaB
	//xbox::addr_xt dma_a, dma_b;
#ifdef USE_TEXTURE_CACHE
	GLruCache *texture_cache;
#endif
	bool texture_dirty[NV2A_MAX_TEXTURES];
	TextureBinding *texture_binding[NV2A_MAX_TEXTURES];

#ifdef USE_SHADER_CACHE
	GHashTable *shader_cache;
#endif
	ShaderBinding *shader_binding;
	//KelvinPrimitive.SetTextureMatrix0Enable[4]
	//bool texture_matrix_enable[NV2A_MAX_TEXTURES];

	/* FIXME: Move to NV_PGRAPH_BUMPMAT... */
	//float bump_env_matrix[NV2A_MAX_TEXTURES][4]; /*Edit again, redundant, using KelvinPrimitive.SetTexture[].SetBumpEnvMat00 instead. Edit, change to 4 to adopt KelvinPrimitive. 3 allowed stages with 2x2 matrix each */

	GloContext *gl_context;
	GLuint gl_framebuffer;
	GLuint gl_color_buffer, gl_zeta_buffer;

	//KelvinPrimitive.SetContextDmaState
	//xbox::addr_xt dma_state;

	//use pg->KelvinPrimitive.SetContextDmaNotifies
	//xbox::addr_xt dma_notifies;

	//KelvinPrimitive.SetContextDmaSemaphore
	//xbox::addr_xt dma_semaphore;

	//KelvinPrimitive.SetContextDmaReport
	//xbox::addr_xt dma_report;
	xbox::addr_xt report_offset;
	//pg->KelvinPrimitive.SetZpassPixelCountEnable
	//bool zpass_pixel_count_enable;
	unsigned int zpass_pixel_count_result;
	unsigned int gl_zpass_pixel_count_query_count;
	GLuint* gl_zpass_pixel_count_queries;
	//KelvinPrimitive.SetContextDmaVertexA , KelvinPrimitive.SetContextDmaVertexB
	//xbox::addr_xt dma_vertex_a, dma_vertex_b;

	unsigned int primitive_mode; // Copy of KelvinPrimitive.SetBeginEnd

	//unsigned int clear_surface; using pg->KelvinPrimitive.ClearSurface or pg->regs[NV097_CLEAR_SURFACE] directly
    //pg->clear_surface  
	//pg->KelvinPrimitive.SetTransformProgramCxtWriteEn
	//bool enable_vertex_program_write;
	//add extra slot for edge case in token parsing. FLD_FINAL
	uint32_t vsh_program_slots[NV2A_MAX_TRANSFORM_PROGRAM_LENGTH+1][VSH_TOKEN_SIZE];

	uint32_t vsh_constants[NV2A_VERTEXSHADER_CONSTANTS][4];
	bool vsh_constants_dirty[NV2A_VERTEXSHADER_CONSTANTS];

	//pg->KelvinPrimitive.SetLight[8].{AmbientColor[3],DiffuseColor[3],SpecularColor[3],LocalRange,InfiniteHalfVector[3],InfiniteDirection[3],SpotFalloff[3],SpotDirection[4],LocalPosition[3],LocalAttenuation[3],Rev_1074[3]}
	/* lighting constant arrays */
	uint32_t ltctxa[NV2A_LTCTXA_COUNT][4];
	bool ltctxa_dirty[NV2A_LTCTXA_COUNT];

	//pg->KelvinPrimitive.SetLight[8].{AmbientColor[3],DiffuseColor[3],SpecularColor[3],}
	//KelvinPrimitive.SetBackLight[8]. {AmbientColor[3], DiffuseColor[3], SpecularColor[3], }
	uint32_t ltctxb[NV2A_LTCTXB_COUNT][4];
	bool ltctxb_dirty[NV2A_LTCTXB_COUNT];

	//pg->KelvinPrimitive.SetLight[8].LocalRange
	uint32_t ltc1[NV2A_LTC1_COUNT][4];
	bool ltc1_dirty[NV2A_LTC1_COUNT];

	//pg->KelvinPrimitive.SetLight[8].InfiniteHalfVector[3]
	// should figure out where these are in lighting context
	//float light_infinite_half_vector[NV2A_MAX_LIGHTS][3];

	//pg->KelvinPrimitive.SetLight[8].InfiniteDirection[3]
	//float light_infinite_direction[NV2A_MAX_LIGHTS][3];

	//pg->KelvinPrimitive.SetLight[8].LocalPosition[3]
	//float light_local_position[NV2A_MAX_LIGHTS][3];

	//pg->KelvinPrimitive.SetLight[8].LocalAttenuation[3]
	//float light_local_attenuation[NV2A_MAX_LIGHTS][3];

	VertexAttribute vertex_attributes[NV2A_VERTEXSHADER_ATTRIBUTES];
	//init in inline_array_length for drawUP draw calls, which vertices are pushed to pushbuffer, vertex attrs are set in KelvinPrimitive.SetVertexDataFormat[16]
	DrawMode draw_mode;
	unsigned int inline_array_length;
	uint32_t inline_array[NV2A_MAX_BATCH_LENGTH];
	GLuint gl_inline_array_buffer;
	//init in inline_elements_length for indexed draw calls, which vertex buffers are set in KelvinPrimitive.SetVertexDataOffset[16], vertex attrs are set in KelvinPrimitive.SetVertexDataFormat[16]
	unsigned int inline_elements_length;
	uint16_t inline_elements[NV2A_MAX_BATCH_LENGTH]; // Cxbx-Reloaded TODO : Restore uint32_t once D3D_draw_inline_elements can using that
	//init in inline_buffer_length for draw calls using BeginEng()/SetVertexDataColor()/SetVertexData4f(), which vertices are pushed to pushbuffer, vertex attrs must be collected during each SetVertexDataXX() calls.
	unsigned int inline_buffer_length;//this counts the total vertex count
	unsigned int inline_buffer_attr_length;//this counts the total attr counts. let's say if we have i vertices, and a attrs for each vertex, and inline_buffer_attr_length == i * a; this is for the ease of vertex setup process.
	//float inline_buffer[NV2A_MAX_BATCH_LENGTH*16*4];
	//init in inline_elements_length for non indexed draw calls, which vertex buffers are set in KelvinPrimitive.SetVertexDataOffset[16], vertex attrs are set in KelvinPrimitive.SetVertexDataFormat[16]
	unsigned int draw_arrays_length;
	unsigned int draw_arrays_max_count;

	/* FIXME: Unknown size, possibly endless, 1000 will do for now */
	GLint gl_draw_arrays_start[1000];
	GLsizei gl_draw_arrays_count[1000];

	GLuint gl_element_buffer;
	GLuint gl_memory_buffer;
	GLuint gl_vertex_array;
	
	bool waiting_for_nop;
	bool waiting_for_flip;
	bool waiting_for_context_switch;
	bool downloads_pending;
	bool gl_sync_pending;
	//for very few pgarph state vars which we couldn't remap to KelvinPrimitive, use pgraph_regs[] instead. check for codes access pg->regs[NV_PGRAPH_XXXX] and replace with pg->pgraph_regs[NV_PGRAPH_XXXX]
	uint32_t pgraph_regs[NV_PGRAPH_SIZE/4];//NV_PGRAPH_SIZE is in bytes, we're declare this array in dwords. for all access to this array, the register offset must be divided by 4.

	volatile union { //map regs[] with struct with members from reverse engineering xbox d3d code. trace xbox d3d call, follow the pushbuffer command/args, and corresponding NV097 methods, we can guess the meanings of them.
            uint32_t regs[0x2000/4];//NV097_XXX register offset is in bytes, we're declare this array in dwords. when use register offset as index, the register offset must be divided by 4.
			NV097KelvinPrimitive KelvinPrimitive;
	};

	//handle for fix function vertex shader.
	DWORD vsh_FVF_handle;


} PGRAPHState;

typedef struct OverlayState {
	bool video_buffer_use;
	int pitch;
	bool is_transparent;
#ifdef DEBUG
	hwaddr base;
	hwaddr limit;
#endif
	hwaddr offset;
	uint32_t in_height;
	uint32_t in_width;
	int out_x;
	int out_y;
	int out_width;
	int out_height;

	bool covers_framebuffer;
	int old_in_width;
	int old_in_height;
	int old_pitch;
	GLuint gl_texture;
} OverlayState;

typedef struct NV2AState {
    // PCIDevice dev;
    // qemu_irq irq;
    bool exiting;
	bool enable_overlay = false;

    // VGACommonState vga;
    // GraphicHwOps hw_ops;
    // QEMUTimer *vblank_timer;

    // MemoryRegion *vram;
    // MemoryRegion vram_pci;
    uint8_t *vram_ptr;
    size_t vram_size;
    // MemoryRegion ramin;
	struct {
		uint8_t *ramin_ptr;
		size_t ramin_size;
	} pramin;

    // MemoryRegion mmio;
    // MemoryRegion block_mmio[NV_NUM_BLOCKS];

    struct {
        uint32_t pending_interrupts;
        uint32_t enabled_interrupts;
		uint32_t regs[NV_PMC_SIZE]; // Not in xqemu/openxbox? TODO : union
    } pmc;

    struct {
        uint32_t pending_interrupts;
        uint32_t enabled_interrupts;
		uint32_t regs[_NV_PFIFO_SIZE]; // TODO : union
		QemuMutex pfifo_lock;
		std::thread puller_thread;
		QemuCond puller_cond;
		std::thread pusher_thread;
		QemuCond pusher_cond;
    } pfifo;

    struct {
		uint32_t pending_interrupts;
		uint32_t enabled_interrupts;
		//QemuCond interrupt_cond; // pvideo.interrupt_cond not used (yet)
		OverlayState overlays[2]; // NV2A supports 2 video overlays
		uint32_t regs[NV_PVIDEO_SIZE]; // TODO : union
    } pvideo;

    struct {
        uint32_t pending_interrupts;
        uint32_t enabled_interrupts;
        uint32_t numerator;
        uint32_t denominator;
        uint32_t alarm_time;
		uint32_t regs[NV_PTIMER_SIZE]; // Not in xqemu/openxbox? TODO : union
    } ptimer;

    struct {
		uint32_t regs[NV_PFB_SIZE]; // TODO : union
    } pfb;

    struct PGRAPHState pgraph;

    struct {
        uint32_t pending_interrupts;
        uint32_t enabled_interrupts;
        hwaddr start;
		uint32_t regs[NV_PCRTC_SIZE]; // Not in xqemu/openxbox? TODO : union
    } pcrtc;

    struct {
        uint32_t core_clock_coeff;
        uint64_t core_clock_freq;
        uint32_t memory_clock_coeff;
        uint32_t video_clock_coeff;
		uint32_t regs[NV_PRAMDAC_SIZE]; // Not in xqemu/openxbox? TODO : union
    } pramdac;

	// PRMCIO (Actually the VGA controller)
	struct {
		uint8_t cr_index;
		uint8_t cr[256]; /* CRT registers */
	} prmcio; // Not in xqemu/openxbox?
} NV2AState;

typedef value_t(*read_func)(NV2AState *d, hwaddr addr); //, unsigned int size);
typedef void(*write_func)(NV2AState *d, hwaddr addr, value_t val); //, unsigned int size);

typedef struct {
	read_func read;
	write_func write;
} MemoryRegionOps;

#if 0
// Valid after PCI init :
#define NV20_REG_BASE_KERNEL 0xFD000000

typedef volatile DWORD *PPUSH;

typedef struct {
	DWORD Ignored[0x10];
	PPUSH Put; // On Xbox1, this field is only written to by the CPU (the GPU uses this as a trigger to start executing from the given address)
	PPUSH Get; // On Xbox1, this field is only read from by the CPU (the GPU reflects in here where it is/stopped executing)
	PPUSH Reference; // TODO : xbox::addr_xt / void* / DWORD ? 
	DWORD Ignored2[0x7ED];
} Nv2AControlDma;
#endif

#endif
