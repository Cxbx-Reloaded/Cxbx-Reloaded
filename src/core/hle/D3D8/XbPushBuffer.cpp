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
#include "core\hle\D3D8\XbD3D8Types.h" // For X_D3DFORMAT
#include "core\hle\D3D8\ResourceTracker.h"
#include "core\hle\D3D8\Direct3D9\Direct3D9.h" // For g_Xbox_VertexShader_Handle
#include "core\hle\D3D8\XbPushBuffer.h"
#include "core\hle\D3D8\XbConvert.h"
#include "devices/video/nv2a.h" // For g_NV2A, PGRAPHState
#include "devices/video/nv2a_int.h" // For NV** defines
#include "Logging.h"

// TODO: Find somewhere to put this that doesn't conflict with xbox::
extern void CxbxUpdateHostTextures();

const char *NV2AMethodToString(DWORD dwMethod); // forward

static void DbgDumpMesh(WORD *pIndexData, DWORD dwCount);

void EmuExecutePushBuffer
(
	xbox::X_D3DPushBuffer       *pPushBuffer,
	xbox::X_D3DFixup            *pFixup
)
{
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
    EmuExecutePushBufferRaw((void*)pPushBuffer->Data, pPushBuffer->Size, (uint32_t **)nullptr, (uint32_t **)nullptr, (uint8_t *)nullptr);

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

void HLE_draw_arrays(NV2AState *d)
{
	// PGRAPHState *pg = &d->pgraph;

	LOG_TEST_CASE("HLE_draw_arrays");

	LOG_UNIMPLEMENTED(); // TODO : Implement HLE_draw_arrays
}

void HLE_draw_inline_buffer(NV2AState *d)
{
	// PGRAPHState *pg = &d->pgraph;

	LOG_TEST_CASE("HLE_draw_inline_buffer");

	LOG_UNIMPLEMENTED(); // TODO : Implement HLE_draw_inline_buffer
}
extern int pgraph_get_inline_array_stride(PGRAPHState *pg);
extern int g_InlineVertexBuffer_DeclarationOverride; // TMP glue
xbox::X_VERTEXATTRIBUTEFORMAT g_NV2AInlineArrayVertexBuffer_AttributeFormat = {};

void HLE_draw_inline_array(NV2AState *d)
{
	PGRAPHState *pg = &d->pgraph;

	//DWORD vertex data array, 
	// To be used as a replacement for DrawVerticesUP, the caller needs to set the vertex format using IDirect3DDevice8::SetVertexShader before calling BeginPush.
	// All attributes in the vertex format must be padded DWORD multiples, and the vertex attributes must be specified in the canonical FVF ordering
	// (position followed by weight, normal, diffuse, and so on).
	// retrieve vertex shader
	if (g_Xbox_VertexShader_Handle == 0) {
		LOG_TEST_CASE("FVF Vertex Shader is null");
	}
	// render vertices
	else {
		//DWORD dwVertexStride = CxbxGetStrideFromVertexDeclaration(CxbxGetVertexDeclaration());
		//vertex stride can be set via NV097_SET_VERTEX_DATA_ARRAY_FORMAT, which is then stored in pg->vertex_attributes[slot].stride where slot is the vertex shader slot being used.
		//don't know how to retrive the slot, set it as 0 here for now.



		DWORD dwVertexStride =pgraph_get_inline_array_stride(pg);
		int uiStride = 0;
		for (int reg = 0; reg < X_VSH_MAX_ATTRIBUTES; reg++) {
			//format was set in NV097_SET_VERTEX_DATA_ARRAY_FORMAT
			//g_NV2AInlineArrayVertexBuffer_AttributeFormat.Slots[reg].Format = pg->vertex_attributes[reg].format;
			g_NV2AInlineArrayVertexBuffer_AttributeFormat.Slots[reg].Offset = uiStride;
			uiStride += pg->vertex_attributes[reg].count * pg->vertex_attributes[reg].size;
			//this should work, and much easier.
			g_NV2AInlineArrayVertexBuffer_AttributeFormat.Slots[reg].Format = pg->KelvinPrimitive.SetVertexDataArrayFormat[reg] & 0xFF;
		}
        //uiStride should end up as overall vertex stride.


		if (dwVertexStride > 0) {
			UINT VertexCount = (pg->inline_array_length * sizeof(DWORD)) / dwVertexStride;
			CxbxDrawContext DrawContext = {};

			DrawContext.pXboxIndexData = false;
			DrawContext.XboxPrimitiveType = (xbox::X_D3DPRIMITIVETYPE)pg->primitive_mode;
			DrawContext.dwVertexCount = VertexCount;
			DrawContext.pXboxVertexStreamZeroData = pg->inline_array;
			DrawContext.uiXboxVertexStreamZeroStride = dwVertexStride;

			// Arrange for g_NV2AInlineArrayVertexBuffer_AttributeFormat to be returned in CxbxGetVertexDeclaration,
			// so that our above composed declaration will be used for the next draw :
			g_InlineVertexBuffer_DeclarationOverride = 2;

			CxbxDrawPrimitiveUP(DrawContext);

			// Now that we've drawn, stop our override in CxbxGetVertexDeclaration :
			g_InlineVertexBuffer_DeclarationOverride = 0;

		}
	}
}

void HLE_draw_inline_elements(NV2AState *d)
{
	PGRAPHState *pg = &d->pgraph;

	unsigned int uiIndexCount = pg->inline_elements_length;
	CxbxDrawContext DrawContext = {};

		DrawContext.XboxPrimitiveType = (xbox::X_D3DPRIMITIVETYPE)pg->primitive_mode;
	DrawContext.dwVertexCount = uiIndexCount;
	DrawContext.pXboxIndexData = d->pgraph.inline_elements;

	CxbxDrawIndexed(DrawContext);
}

DWORD ABGR_to_ARGB(const uint32_t color)
{
	return (color & 0xFF00FF00) | ((color & 0x00FF0000) >> 16) | ((color & 0x000000FF) << 16);
}

void HLE_draw_state_update(NV2AState *d)
{
	PGRAPHState *pg = &d->pgraph;

	CxbxUpdateNativeD3DResources();

	HRESULT hRet;

//	hRet = g_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, xtBOOL); // NV2A_FOG_ENABLE
//	hRet = g_pD3DDevice->SetRenderState(D3DRS_FOGTABLEMODE, xtD3DFOGMODE); // NV2A_FOG_MODE
//	hRet = g_pD3DDevice->SetRenderState(D3DRS_FOGSTART, xtFloat); // NV2A_FOG_COORD_DIST
//	hRet = g_pD3DDevice->SetRenderState(D3DRS_FOGEND, xtFloat); // NV2A_FOG_MODE
//	hRet = g_pD3DDevice->SetRenderState(D3DRS_FOGDENSITY, xtFloat); // NV2A_FOG_EQUATION_CONSTANT / NV2A_FOG_EQUATION_LINEAR / NV2A_FOG_EQUATION_QUADRATIC
	// NV2A_FOG_PLANE?
	// NV2A_SET_LINEAR_FOG_CONST?
//	hRet = g_pD3DDevice->SetRenderState(D3DRS_RANGEFOGENABLE, xtBOOL); // NV2A_FOG_COORD_DIST
	// Unused : D3DRS_FOGVERTEXMODE

	uint32_t fog_color = pg->KelvinPrimitive.SetFogColor;
	hRet = g_pD3DDevice->SetRenderState(D3DRS_FOGCOLOR, ABGR_to_ARGB(fog_color)); // NV2A_FOG_COLOR

// Hint : see DxbxRenderStateInfo table for all known Xbox states, their data type and NV2A method
// Also, see D3DDevice_SetRenderState_Simple call EmuXB2PC_* conversion functions for some render states

//	hRet = g_pD3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, xtBool); // NV2A_NORMALIZE_ENABLE
//	hRet = g_pD3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, xtD3DCOLOR); // NV2A_RC_CONSTANT_COLOR0(0) NV_PGRAPH_COMBINEFACTOR0
//	hRet = g_pD3DDevice->SetRenderState(D3DRS_DEPTHBIAS, xtFloat); // NV2A_POLYGON_OFFSET_FACTOR, NV2A_POLYGON_OFFSET_UNITS, NV2A_POLYGON_OFFSET_POINT_ENABLE, NV2A_POLYGON_OFFSET_LINE_ENABLE, NV2A_POLYGON_OFFSET_FILL_ENABLE, XB2PC conversion needed
//	hRet = g_pD3DDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, xtBool); // Was D3DRS_EDGEANTIALIAS, corresponds to NV2A_LINE_SMOOTH_ENABLE and NV2A_POLYGON_SMOOTH_ENABLE
//	hRet = g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, dwFillMode); // NV2A_POLYGON_MODE_FRONT, EmuXB2PC_* conversion needed
//	hRet = g_pD3DDevice->SetRenderState(D3DRS_VERTEXBLEND, Value); // NV2A_SKIN_MODE
//	hRet = g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, Value); // NV2A_CULL_FACE
//	hRet = g_pD3DDevice->SetRenderState(D3DRS_STENCILFAIL, Value); // NV2A_STENCIL_OP_FAIL
//	hRet = g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, Value); // NV2A_DEPTH_TEST_ENABLE
//	hRet = g_pD3DDevice->SetRenderState(D3DRS_STENCILENABLE, Value); // NV2A_STENCIL_ENABLE
//	hRet = g_pD3DDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, Value); // NV2A_MULTISAMPLE_CONTROL
//	hRet = g_pD3DDevice->SetRenderState(D3DRS_MULTISAMPLEMASK, Value); // NV2A_MULTISAMPLE_CONTROL
//	hRet = g_pD3DDevice->SetRenderState(D3DRS_WRAP0, dwConv); // NV2A_TX_WRAP(0), EmuXB2PC_* conversion needed
//	hRet = g_pD3DDevice->SetRenderState(D3DRS_WRAP1, dwConv); // NV2A_TX_WRAP(1), EmuXB2PC_* conversion needed

//  g_pD3DDevice->SetRenderState(D3DRS_LINEPATTERN, Value); // NV2A_POLYGON_STIPPLE_PATTERN? Seems unused in Xbox

	LOG_INCOMPLETE(); // TODO : Read state from pgraph, convert to D3D
}

void HLE_draw_clear(NV2AState *d)
{
	// PGRAPHState *pg = &d->pgraph;

	CxbxUpdateNativeD3DResources();

	LOG_INCOMPLETE(); // TODO : Read state from pgraph, convert to D3D (call EMUPATCH(D3DDevice_Clear)?)
}

// Import pgraph_draw_* variables, declared in EmuNV2A_PGRAPH.cpp :
extern void(*pgraph_draw_arrays)(NV2AState *d);
extern void(*pgraph_draw_inline_buffer)(NV2AState *d);
extern void(*pgraph_draw_inline_array)(NV2AState *d);
extern void(*pgraph_draw_inline_elements)(NV2AState *d);
extern void(*pgraph_draw_state_update)(NV2AState *d);
extern void(*pgraph_draw_clear)(NV2AState *d);

void HLE_init_pgraph_plugins()
{
	/* attach HLE Direct3D render plugins */
	pgraph_draw_arrays = HLE_draw_arrays;
	pgraph_draw_inline_buffer = HLE_draw_inline_buffer;
	pgraph_draw_inline_array = HLE_draw_inline_array;
	pgraph_draw_inline_elements = HLE_draw_inline_elements;
	pgraph_draw_state_update = HLE_draw_state_update;
	pgraph_draw_clear = HLE_draw_clear;
}
//int pgraph_handle_method(NV2AState *d, unsigned int subchannel,
//unsigned int method, uint32_t parameter,
//uint32_t *parameters, size_t method count,
//size_t max_lookahead_words)
extern int pgraph_handle_method(
	NV2AState *d,
	unsigned int subchannel,
	unsigned int method,
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

float *HLE_get_NV2A_vertex_attribute_value_pointer(unsigned slot)
{
	NV2AState* dev = g_NV2A->GetDeviceState();
	PGRAPHState *pg = &(dev->pgraph);

	// See CASE_16(NV097_SET_VERTEX_DATA4UB, 4) in LLE pgraph_handle_method()
	VertexAttribute *vertex_attribute = &pg->vertex_attributes[slot];
	return vertex_attribute->inline_value;
}

uint32_t HLE_read_NV2A_vertex_program_slot(unsigned program_load, unsigned slot)
{
	NV2AState* dev = g_NV2A->GetDeviceState();
	PGRAPHState* pg = &(dev->pgraph);

	// See CASE_32(NV097_SET_TRANSFORM_PROGRAM, 4) in LLE pgraph_handle_method()
	assert(program_load < NV2A_MAX_TRANSFORM_PROGRAM_LENGTH);
	uint32_t value = pg->program_data[program_load][slot % 4];

	return value;
}

float *HLE_get_NV2A_vertex_constant_float4_ptr(unsigned const_index)
{
	NV2AState* dev = g_NV2A->GetDeviceState();
	PGRAPHState* pg = &(dev->pgraph);

	// See CASE_32(NV097_SET_TRANSFORM_CONSTANT, 4) in LLE pgraph_handle_method()
	assert(const_index < NV2A_VERTEXSHADER_CONSTANTS);
	return (float*)&(pg->vsh_constants[const_index][0]);
}

// For now, skip the cache, but handle the pgraph method directly
// Note : Here's where the method gets multiplied by four!
// Note 2 : d is read from local scope, and ni is unused (same in LLE)
// Note 3 : Keep EmuExecutePushBufferRaw skipping all commands not intended for channel 0 (3D)
// Note 4 : Prevent a crash during shutdown when g_NV2A gets deleted
#define CACHE_PUSH(subc, mthd, word, dma_get, mcnt,max_words_available, ni) \
	if (subc == 0) { \
		if (g_NV2A) { \
			pgraph_handle_method(d, subc, mthd << 2, word, dma_get, mcnt,max_words_available); \
		} \
	}

typedef union {
/* https://envytools.readthedocs.io/en/latest/hw/fifo/dma-pusher.html#the-commands-pre-gf100-format

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
		uint32_t        type         : 2;        /*  0 ..  1 */
			// See https://envytools.readthedocs.io/en/latest/hw/fifo/dma-pusher.html#nv4-control-flow-commands
				#define COMMAND_TYPE_NONE        0
				#define COMMAND_TYPE_JUMP_LONG   1
				#define COMMAND_TYPE_CALL        2
		uint32_t        method       : 11;       /*  2 .. 12 */
		uint32_t        subchannel   : 3;        /* 13 .. 15 */
		uint32_t        flags        : 2;        /* 16 .. 17 */
			// See https://envytools.readthedocs.io/en/latest/hw/fifo/dma-pusher.html#nv4-method-submission-commands
				#define COMMAND_FLAGS_NONE                         0
				#define COMMAND_FLAGS_SLI_CONDITIONAL              1 // (NV40+)
				#define COMMAND_FLAGS_RETURN                       2
				#define COMMAND_FLAGS_LONG_NON_INCREASING_METHODS  3 // [IB-mode only] 
		uint32_t        method_count : 11;       /* 18 .. 28 */
		uint32_t        instruction  : 3;        /* 29 .. 31 */
				#define COMMAND_INSTRUCTION_INCREASING_METHODS     0
				#define COMMAND_INSTRUCTION_JUMP                   1
				#define COMMAND_INSTRUCTION_NON_INCREASING_METHODS 2
	};
	#define COMMAND_WORD_MASK_JUMP      0x1FFFFFFC /*  2 .. 31 */
	#define COMMAND_WORD_MASK_JUMP_LONG 0xFFFFFFFC /*  2 .. 28 */
} nv_fifo_command;

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
        bool ni; // Current command's NI (non-increasing) flag
        bool lenp;// Current command's long non-increasing method flag
    } dma_state;

    static uint32_t dcount_shadow = 0; // [NV5:] Number of already-processed methods in cmd]
    static bool subr_active = false; // Subroutine active
    static uint32_t *subr_return = nullptr; // Subroutine return address
    // static bool big_endian = false; // Pushbuffer endian switch

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

    // NV-4-style PFIFO DMA command stream pusher
    // See https://envytools.readthedocs.io/en/latest/hw/fifo/dma-pusher.html#the-pusher-pseudocode-pre-gf100
    while (dma_get != dma_put) {
        // Check if loop reaches end of pushbuffer
        if (dma_get >= dma_limit) {
            LOG_TEST_CASE("Last pushbuffer instruction exceeds END of Data");
            // TODO : throw DMA_PUSHER(MEM_FAULT);
            return; // For now, don't even attempt to run through
        }

        // Read a DWORD from the current push buffer pointer
        uint32_t * word_ptr = dma_get;//shadow copy of dma_get before it gets advanced after the word was read.
        word = *dma_get++;
        /* now, see if we're in the middle of a long command */
        if (dma_state.lenp) {
            dma_state.lenp = 0;
            dma_state.mcnt = word & 0xffffff;
            continue;// while for next word
        }
        else if (dma_state.mcnt) {
            /* data word of methods command */
            data_shadow = word;
#if 0
            if (!PULLER_KNOWS_MTHD(dma_state.mthd)) {
                throw DMA_PUSHER(INVALID_MTHD);				
                return; // For now, don't even attempt to run through
            }
            
#endif
            //words still available in the pushbuffer.
            uint32_t max_words_available = ((uint32_t)dma_limit - (uint32_t)word_ptr) / 4;

            //pgraph_handle_method(
            //	d,						//NV2AState
            //	subc,					//Subchannel
            //	mthd << 2,				//method
            //	word,					//first parameter
            //	dma_get,				//parameters, pointer to 1st parameter, which is exact parameter in the args.
            //	mcnt,					//method count
            //	max_words_available);   //words still available in the pushbuffer

            uint32_t num_processed = 1;

            //this is not supposed to happen. if the pushbuffer is correct, and the method handler works right. method count always represents the dwords of
            //the parameters required by the method. so at least there should be method count dwords left in the buffer for the method to use.
            if (dma_state.mcnt > max_words_available) { 
                LOG_TEST_CASE("Pushbuffer data not enough for method count!");
                if (p_dma_get != nullptr)
                    *p_dma_get = (uint32_t*)((uint32_t)dma_put- (uint32_t)dma);       //update the pfifo_dma_get to the end of pushbuffer if we're called via pfifo_run_pusher()
                return;//we shall not run through this situation.
            }
            
            if (dma_state.subc < 8) {//subchannel must be less than 8
                
                    if (g_NV2A) {
                        //return num_processed, the words processed here by the method handler. so the caller can advance the dma_get pointer of the pushbuffer
                        //num_processed default to 1, which represent the first parameter passed in this call.
                        //but that word is advanced by the caller already. it's the caller's duty to subtract that word from the num_processed;
						uint32_t method = dma_state.mthd << 2;
						num_processed=pgraph_handle_method(
                            d,
                            dma_state.subc,
                            method,
                            word,
                            word_ptr, //it's the address where we read the word. we can't use dma_get here because dma_get was advanced after word was read.
                            dma_state.mcnt,
                            max_words_available);
                    } 
            }

            //if (!dma_state.ni) {
            //	dma_state.mthd++;
            //}

            //if (num_processed > 1) {
                dma_get=word_ptr+ dma_state.mcnt;//num_processed default to 1.
                if (p_dma_get != nullptr)
                    *p_dma_get = (uint32_t*)((uint32_t)dma_get - (uint32_t)dma);       //update the pfifo_dma_get if we're called via pfifo_run_pusher()
				dcount_shadow += num_processed;
				dma_state.mcnt= 0;
                //if (dma_state.mcnt != 0) {
                    //assert(dma_state.mcnt == 0);
                //    LOG_TEST_CASE("Pushbuffer method count not decreased to 0 after method processing");
                //    dma_state.mcnt = 0;
                // }
                
            //}
            //else {
                //dma_state.mcnt--;
                //dcount_shadow++;
            //}
            continue; // while for next word
        }

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
                if (p_dma_get != nullptr)
                    *p_dma_get = (uint32_t*)((uint32_t)dma_get - (uint32_t)dma);       //update the pfifo_dma_get if we're called via pfifo_run_pusher()
                continue; // while
            }
            break; // fall through
        case COMMAND_TYPE_JUMP_LONG:
            LOG_TEST_CASE("Pushbuffer COMMAND_TYPE_JUMP_LONG");
            dma_get_jmp_shadow = dma_get;
            dma_get = (uint32_t *)(CONTIGUOUS_MEMORY_BASE | (word & COMMAND_WORD_MASK_JUMP_LONG));
            if (p_dma_get != nullptr)
                *p_dma_get = (uint32_t*)((uint32_t)dma_get - (uint32_t)dma);       //update the pfifo_dma_get if we're called via pfifo_run_pusher()
            continue; // while
        case COMMAND_TYPE_CALL: // Note : NV2A call is used as jump, the sub_return is used as a flag to indicate where the jump was orginated.
            if (subr_active) {
                LOG_TEST_CASE("Pushbuffer COMMAND_TYPE_CALL while another call was active!");
                // TODO : throw DMA_PUSHER(CALL_SUBR_ACTIVE);
				if (p_dma_get != nullptr)
					*p_dma_get = (uint32_t*)((uint32_t)dma_put - (uint32_t)dma); //update the pfifo_dma_get to the end of pushbuffer if we're called via pfifo_run_pusher()
				return; // For now, don't even attempt to run through
            }
            else {
                LOG_TEST_CASE("Pushbuffer COMMAND_TYPE_CALL");
            }

            subr_return = dma_get;
            subr_active = true;
            dma_get = (uint32_t *)(CONTIGUOUS_MEMORY_BASE | (word & COMMAND_WORD_MASK_JUMP_LONG));
            if (p_dma_get != nullptr)
                *p_dma_get = (uint32_t*)((uint32_t)dma_get - (uint32_t)dma);       //update the pfifo_dma_get if we're called via pfifo_run_pusher()
            continue; // while
        default:
            LOG_TEST_CASE("Pushbuffer COMMAND_TYPE unknown");
            // TODO : throw DMA_PUSHER(INVALID_CMD);
            if (p_dma_get != nullptr)
                *p_dma_get = (uint32_t*)((uint32_t)dma_put - (uint32_t)dma);       //update the pfifo_dma_get to the end of pushbuffer if we're called via pfifo_run_pusher()
            return; // For now, don't even attempt to run through
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
            if (p_dma_get != nullptr)
                *p_dma_get = (uint32_t*)((uint32_t)dma_put - (uint32_t)dma);       //update the pfifo_dma_get to the end of pushbuffer if we're called via pfifo_run_pusher()
            return; // For now, don't even attempt to run through
        } // switch instruction

        switch (command.flags) {
        case COMMAND_FLAGS_NONE: // Decode push buffer method & size (inverse of D3DPUSH_ENCODE)
            dma_state.mthd = command.method;
            dma_state.subc = command.subchannel;
            dma_state.mcnt = command.method_count;
            break; // fall through
        case COMMAND_FLAGS_RETURN: // Note : NV2A return shouldn't be used because the NV2A call is used as jump and the subr_return is used as an flag of where the jump was from.
			//considering move this case to the same code block with old jump, RETURN can be tested with word & 0xE0030003 == 0x00020000. 
			LOG_TEST_CASE("Pushbuffer COMMAND_FLAGS_RETURN, not supposed to be used!");
            if (word != 0x00020000) {
                LOG_TEST_CASE("Pushbuffer COMMAND_FLAGS_RETURN with additional bits?!");
				if (p_dma_get != nullptr)
					*p_dma_get = (uint32_t*)((uint32_t)dma_put - (uint32_t)dma); //update the pfifo_dma_get to the end of pushbuffer if we're called via pfifo_run_pusher()
				return; // For now, don't even attempt to run through
            }
            else {
                LOG_TEST_CASE("Pushbuffer COMMAND_FLAGS_RETURN");
            }

            if (!subr_active) {
                LOG_TEST_CASE("Pushbuffer COMMAND_FLAGS_RETURN while another call was active!");
                // TODO : throw DMA_PUSHER(RET_SUBR_INACTIVE);
				if (p_dma_get != nullptr)
					*p_dma_get = (uint32_t*)((uint32_t)dma_put - (uint32_t)dma); //update the pfifo_dma_get to the end of pushbuffer if we're called via pfifo_run_pusher()
				return; // For now, don't even attempt to run through
            }

            dma_get = subr_return;
            if (p_dma_get != nullptr)
                *p_dma_get = (uint32_t*)((uint32_t)dma_get - (uint32_t)dma);       //update the pfifo_dma_get if we're called via pfifo_run_pusher()
            subr_active = false;
            continue; // while
        case COMMAND_FLAGS_LONG_NON_INCREASING_METHODS:
            dma_state.lenp = true;
            LOG_TEST_CASE("Pushbuffer COMMAND_FLAGS_LONG_NON_INCREASING_METHODS");
            break;
        default:
            if (command.flags == COMMAND_FLAGS_SLI_CONDITIONAL) {
                LOG_TEST_CASE("Pushbuffer COMMAND_FLAGS_SLI_CONDITIONAL (NV40+) not available on NV2A");
            } else if (command.flags == COMMAND_FLAGS_LONG_NON_INCREASING_METHODS) {
                LOG_TEST_CASE("Pushbuffer COMMAND_FLAGS_LONG_NON_INCREASING_METHODS [IB-mode only] not available on NV2A");
                /// No need to do: dma_state.mthd = command.method; dma_state.ni = true;
                /// dma_state.mcnt = *dma_get++ & 0x00FFFFFF; // Long NI method command count is read from low 24 bits of next word
                /// dma_get += dma_state.mcnt; // To be safe, skip method data
                /// continue;
            } else {
                LOG_TEST_CASE("Pushbuffer COMMAND_FLAGS unknown");
            }

            /// dma_get += command.method_count; // To be safe, skip method data
            /// continue;
            // TODO : throw DMA_PUSHER(INVALID_CMD);
            if (p_dma_get != nullptr)
                *p_dma_get = (uint32_t*)((uint32_t)dma_put - (uint32_t)dma);       //update the pfifo_dma_get to the end of pushbuffer if we're called via pfifo_run_pusher()
            return; // For now, don't even attempt to run through
        } // switch flags

        dcount_shadow = 0;
    } // while (dma_get != dma_put)
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
