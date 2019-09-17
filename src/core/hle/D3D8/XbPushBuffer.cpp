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
#include "core\kernel\support\EmuXTL.h"
#include "core\hle\D3D8\XbD3D8Types.h" // For X_D3DFORMAT
#include "core\hle\D3D8\ResourceTracker.h"
#include "core\hle\D3D8\XbPushBuffer.h"
#include "core\hle\D3D8\XbState.h" // For CxbxUpdateNativeD3DResources, etc
#include "core\hle\D3D8\XbConvert.h"
#include "devices/video/nv2a.h" // For g_NV2A, PGRAPHState
#include "devices/video/nv2a_int.h" // For NV** defines
#include "Logging.h"

// TODO: Find somewhere to put this that doesn't conflict with XTL::
extern void EmuUpdateActiveTextureStages();
extern DWORD g_XboxBaseVertexIndex;

const char *NV2AMethodToString(DWORD dwMethod); // forward

static void DbgDumpMesh(WORD *pIndexData, DWORD dwCount);

// Determine the size (in number of floating point texture coordinates) of the texture format (indexed 0 .. 3).
// This is the reverse of the D3DFVF_TEXCOORDSIZE[0..3] macros.
int DxbxFVF_GetNumberOfTextureCoordinates(DWORD dwFVF, int aTextureIndex)
{
	// See D3DFVF_TEXCOORDSIZE1()
	switch ((dwFVF >> ((aTextureIndex * 2) + 16)) & 3) {
	case D3DFVF_TEXTUREFORMAT1: return 1; // One floating point value
	case D3DFVF_TEXTUREFORMAT2: return 2; // Two floating point values
	case D3DFVF_TEXTUREFORMAT3: return 3; // Three floating point values
	case D3DFVF_TEXTUREFORMAT4: return 4; // Four floating point values
	default:
		//assert(false || "DxbxFVF_GetNumberOfTextureCoordinates : Unhandled case");
		return 0;
	}
}

// Dxbx Note: This code appeared in EmuExecutePushBufferRaw and occured
// in EmuFlushIVB too, so it's generalize in this single implementation.
UINT DxbxFVFToVertexSizeInBytes(DWORD dwFVF, BOOL bIncludeTextures)
{
/*
	X_D3DFVF_POSITION_MASK    = $00E; // Dec  /2  #fl

	X_D3DFVF_XYZ              = $002; //  2 > 1 > 3
	X_D3DFVF_XYZRHW           = $004; //  4 > 2 > 4
	X_D3DFVF_XYZB1            = $006; //  6 > 3 > 4
	X_D3DFVF_XYZB2            = $008; //  8 > 4 > 5
	X_D3DFVF_XYZB3            = $00a; // 10 > 5 > 6
	X_D3DFVF_XYZB4            = $00c; // 12 > 6 > 7
*/
	// Divide the D3DFVF by two, this gives almost the number of floats needed for the format :
	UINT Result = (dwFVF & D3DFVF_POSITION_MASK) >> 1;
	if (Result >= (D3DFVF_XYZB1 >> 1)) {
		// Any format from D3DFVF_XYZB1 and above need 1 extra float :
		Result++;
	}
	else {
		// The other formats (XYZ and XYZRHW) need 2 extra floats :
		Result += 2;
	}

	// Express the size in bytes, instead of floats :
	Result *= sizeof(FLOAT);

	// D3DFVF_NORMAL cannot be combined with D3DFVF_XYZRHW :
	if ((dwFVF & D3DFVF_POSITION_MASK) != D3DFVF_XYZRHW) {
		if (dwFVF & D3DFVF_NORMAL) {
			Result += sizeof(FLOAT) * 3;
		}
	}

	if (dwFVF & D3DFVF_DIFFUSE) {
		Result += sizeof(D3DCOLOR);
	}

	if (dwFVF & D3DFVF_SPECULAR) {
		Result += sizeof(D3DCOLOR);
	}

	if (bIncludeTextures) {
		int NrTextures = ((dwFVF & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT);
		while (NrTextures > 0) {
			NrTextures--;
			Result += DxbxFVF_GetNumberOfTextureCoordinates(dwFVF, NrTextures) * sizeof(FLOAT);
		}
	}

	return Result;
}

void EmuExecutePushBuffer
(
	XTL::X_D3DPushBuffer       *pPushBuffer,
	XTL::X_D3DFixup            *pFixup
)
{
	using namespace XTL;

	//Check whether Fixup exists or not. 
	if (pFixup != NULL) {
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
    EmuExecutePushBufferRaw((void*)pPushBuffer->Data, pPushBuffer->Size);

    return;
}

DWORD CxbxGetStrideFromVertexShaderHandle(DWORD dwVertexShader)
{
	using namespace XTL;

	XTL::DWORD Stride = 0;

	if (VshHandleIsVertexShader(dwVertexShader)) {
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

		CxbxVertexShader *pCxbxVertexShader = GetCxbxVertexShader(dwVertexShader);
		if (pCxbxVertexShader) {
			if (pCxbxVertexShader->VertexShaderInfo.NumberOfVertexStreams == 1) {
				// Note : This assumes that the only stream in use will be stream zero :
				Stride = pCxbxVertexShader->VertexShaderInfo.VertexStreams[0].HostVertexStride;
			}
			else {
				LOG_TEST_CASE("Non-FVF Vertex Shaders with multiple streams not supported for PushBuffer emulation!");
			}
		}
	}
	else {
		if (VshHandleIsFVF(dwVertexShader)) {
			Stride = DxbxFVFToVertexSizeInBytes(dwVertexShader, /*bIncludeTextures=*/true);
		}
		else {
			LOG_TEST_CASE("Invalid Vertex Shader not supported for PushBuffer emulation!");
		}
	}

	return Stride;
}

void HLE_draw_arrays(NV2AState *d)
{
	// PGRAPHState *pg = &d->pgraph;

	using namespace XTL;

	LOG_TEST_CASE("HLE_draw_arrays");

	LOG_UNIMPLEMENTED(); // TODO : Implement HLE_draw_arrays
}

void HLE_draw_inline_buffer(NV2AState *d)
{
	// PGRAPHState *pg = &d->pgraph;

	using namespace XTL;

	LOG_TEST_CASE("HLE_draw_inline_buffer");

	LOG_UNIMPLEMENTED(); // TODO : Implement HLE_draw_inline_buffer
}

void HLE_draw_inline_array(NV2AState *d)
{
	PGRAPHState *pg = &d->pgraph;

	using namespace XTL;

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
		XTL::DWORD dwVertexStride = CxbxGetStrideFromVertexShaderHandle(g_Xbox_VertexShader_Handle);
		if (dwVertexStride > 0) {
			XTL::UINT VertexCount = (pg->inline_array_length * sizeof(XTL::DWORD)) / dwVertexStride;
			CxbxDrawContext DrawContext = {};

			DrawContext.XboxPrimitiveType = (X_D3DPRIMITIVETYPE)pg->primitive_mode;
			DrawContext.dwVertexCount = VertexCount;
			DrawContext.pXboxVertexStreamZeroData = pg->inline_array;
			DrawContext.uiXboxVertexStreamZeroStride = dwVertexStride;

			CxbxDrawPrimitiveUP(DrawContext);
		}
	}
}

void HLE_draw_inline_elements(NV2AState *d)
{
	PGRAPHState *pg = &d->pgraph;

	using namespace XTL;

	if (IsValidCurrentShader()) {
		unsigned int uiIndexCount = pg->inline_elements_length;
		CxbxDrawContext DrawContext = {};

		DrawContext.XboxPrimitiveType = (X_D3DPRIMITIVETYPE)pg->primitive_mode;
		DrawContext.dwVertexCount = EmuD3DIndexCountToVertexCount(DrawContext.XboxPrimitiveType, uiIndexCount);
		DrawContext.pIndexData = d->pgraph.inline_elements; // Used by GetVerticesInBuffer

		CxbxDrawIndexed(DrawContext);
	}
}

DWORD ABGR_to_ARGB(const uint32_t color)
{
	return (color & 0xFF00FF00) | ((color & 0x00FF0000) >> 16) | ((color & 0x000000FF) << 16);
}

void HLE_draw_state_update(NV2AState *d)
{
	PGRAPHState *pg = &d->pgraph;

	using namespace XTL;

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

	uint32_t fog_color = pg->regs[NV_PGRAPH_FOGCOLOR];
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

	using namespace XTL;

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

extern void pgraph_handle_method(
	NV2AState *d,
	unsigned int subchannel,
	unsigned int method,
	uint32_t parameter);

// LLE NV2A
extern NV2ADevice* g_NV2A;

uint32_t HLE_read_NV2A_pgraph_register(const int reg)
{
	NV2AState* dev = g_NV2A->GetDeviceState();
	PGRAPHState *pg = &(dev->pgraph);
	return pg->regs[reg];
}

void HLE_write_NV2A_vertex_attribute_slot(unsigned slot, uint32_t parameter)
{
	// Write value to LLE NV2A device
	pgraph_handle_method(g_NV2A->GetDeviceState(),
		/*subchannel=*/0,
		/*method=*/NV097_SET_VERTEX_DATA4UB + (4 * slot),
		parameter);
}

uint32_t HLE_read_NV2A_vertex_attribute_slot(unsigned slot)
{
	NV2AState* dev = g_NV2A->GetDeviceState();
	PGRAPHState *pg = &(dev->pgraph);

	// See CASE_16(NV097_SET_VERTEX_DATA4UB, 4) in LLE pgraph_handle_method()
	VertexAttribute *vertex_attribute = &pg->vertex_attributes[slot];
	// Inverse of D3DDevice_SetVertexDataColor
	uint8_t a = uint8_t(vertex_attribute->inline_value[0] * 255.0f);
	uint8_t b = uint8_t(vertex_attribute->inline_value[1] * 255.0f);
	uint8_t c = uint8_t(vertex_attribute->inline_value[2] * 255.0f);
	uint8_t d = uint8_t(vertex_attribute->inline_value[3] * 255.0f);
	uint32_t value = a + (b << 8) + (c << 16) + (d << 24);

	return value;
}

// For now, skip the cache, but handle the pgraph method directly
// Note : Here's where the method gets multiplied by four!
// Note 2 : d is read from local scope, and ni is unused (same in LLE)
// Note 3 : Keep EmuExecutePushBufferRaw skipping all commands not intended for channel 0 (3D)
// Note 4 : Prevent a crash during shutdown when g_NV2A gets deleted
#define CACHE_PUSH(subc, mthd, word, ni) \
	if (subc == 0) { \
		if (g_NV2A) { \
			pgraph_handle_method(d, subc, mthd << 2, word); \
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
	uint32_t uSizeInBytes
)
{
	using namespace XTL; // for logging

	HLE_init_pgraph_plugins(); // TODO : Move to more approriate spot

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
	d->pgraph.regs[NV_PGRAPH_CTX_CONTROL] |= NV_PGRAPH_CTX_CONTROL_CHID; // avoid assert in pgraph_handle_method()


	// DMA Pusher state -- see https://envytools.readthedocs.io/en/latest/hw/fifo/dma-pusher.html#pusher-state
#if 0
	static xbaddr dma_pushbuffer; // the pushbuffer and IB DMA object
#endif
	uint32_t *dma_limit; // pushbuffer size limit
	uint32_t *dma_put; // pushbuffer current end address
	uint32_t *dma_get; //pushbuffer current read address
	struct {
		NV2AMETHOD mthd; // Current method
		uint32_t subc; // :3 = Current subchannel
		uint32_t mcnt; // :24 = Current method count
		bool ni; // Current command's NI (non-increasing) flag
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
	dma_limit = (uint32_t*)((xbaddr)pPushData + uSizeInBytes); // TODO : If this an absolute addresss?
	dma_put = (uint32_t*)((xbaddr)pPushData + uSizeInBytes);
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
		word = *dma_get++;
		/* now, see if we're in the middle of a command */
		if (dma_state.mcnt) {
			/* data word of methods command */
			data_shadow = word;
#if 0
			if (!PULLER_KNOWS_MTHD(dma_state.mthd)) {
				throw DMA_PUSHER(INVALID_MTHD);				
				return; // For now, don't even attempt to run through
			}

#endif
			CACHE_PUSH(dma_state.subc, dma_state.mthd, word, dma_state.ni);
			if (!dma_state.ni) {
				dma_state.mthd++;
			}

			dma_state.mcnt--;
			dcount_shadow++;
			continue; // while
		}

		/* no command active - this is the first word of a new one */
		rsvd_shadow = word;
		// Check and handle command type, then instruction, then flags
		switch (command.type) {
		case COMMAND_TYPE_NONE:
			break; // fall through
		case COMMAND_TYPE_JUMP_LONG:
			LOG_TEST_CASE("Pushbuffer COMMAND_TYPE_JUMP_LONG");
			dma_get_jmp_shadow = dma_get;
			dma_get = (uint32_t *)(CONTIGUOUS_MEMORY_BASE | (word & COMMAND_WORD_MASK_JUMP_LONG));
			continue; // while
		case COMMAND_TYPE_CALL: // Note : NV2A return is said not to work?
			if (subr_active) {
				LOG_TEST_CASE("Pushbuffer COMMAND_TYPE_CALL while another call was active!");
				// TODO : throw DMA_PUSHER(CALL_SUBR_ACTIVE);
				return; // For now, don't even attempt to run through
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
			return; // For now, don't even attempt to run through
		} // switch type

		switch (command.instruction) {
		case COMMAND_INSTRUCTION_INCREASING_METHODS:
			dma_state.ni = false;
			break;
		case COMMAND_INSTRUCTION_JUMP:
			LOG_TEST_CASE("Pushbuffer COMMAND_INSTRUCTION_JUMP");
			dma_get_jmp_shadow = dma_get;
			dma_get = (uint32_t *)(CONTIGUOUS_MEMORY_BASE | (word & COMMAND_WORD_MASK_JUMP));
			continue; // while
		case COMMAND_INSTRUCTION_NON_INCREASING_METHODS:
			dma_state.ni = true;
			break;
		default:
			LOG_TEST_CASE("Pushbuffer COMMAND_INSTRUCTION unknown");
			// TODO : throw DMA_PUSHER(INVALID_CMD);
			return; // For now, don't even attempt to run through
		} // switch instruction

		switch (command.flags) {
		case COMMAND_FLAGS_NONE: // Decode push buffer method & size (inverse of D3DPUSH_ENCODE)
			dma_state.mthd = command.method;
			dma_state.subc = command.subchannel;
			dma_state.mcnt = command.method_count;
			break; // fall through
		case COMMAND_FLAGS_RETURN: // Note : NV2A return is said not to work?
			if (word != 0x00020000) {
				LOG_TEST_CASE("Pushbuffer COMMAND_FLAGS_RETURN with additional bits?!");
				return; // For now, don't even attempt to run through
			}
			else {
				LOG_TEST_CASE("Pushbuffer COMMAND_FLAGS_RETURN");
			}

			if (!subr_active) {
				LOG_TEST_CASE("Pushbuffer COMMAND_FLAGS_RETURN while another call was active!");
				// TODO : throw DMA_PUSHER(RET_SUBR_INACTIVE);
				return; // For now, don't even attempt to run through
			}

			dma_get = subr_return;
			subr_active = false;
			continue; // while
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
			return; // For now, don't even attempt to run through
		} // switch flags

		dcount_shadow = 0;
    } // while (dma_get != dma_put)
}

const char *NV2AMethodToString(DWORD dwMethod)
{
	using namespace XTL; // for NV2A symbols

	switch (dwMethod) {

#define ENUM_RANGED_ToString_N(Name, Method, Pitch, N) \
	case Name(N): return #Name ## "((" #N ")*" #Pitch ## ")";

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
