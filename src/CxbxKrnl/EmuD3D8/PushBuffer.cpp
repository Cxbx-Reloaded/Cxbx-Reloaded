// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuD3D->PushBuffer.cpp
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
#define _XBOXKRNL_DEFEXTRN_

#define LOG_PREFIX "PSHB"

#include <assert.h> // For assert()

#include "CxbxKrnl/Emu.h"
#include "CxbxKrnl/EmuXTL.h"
#include "CxbxKrnl/EmuD3D8Types.h" // For X_D3DFORMAT
#include "CxbxKrnl/ResourceTracker.h"
#include "devices/video/nv2a.h" // For PGRAPHState
#include "devices/video/nv2a_int.h" // For NV** defines
#include "Logging.h"

// TODO: Find somewhere to put this that doesn't conflict with XTL::
extern void EmuUpdateActiveTextureStages();
extern DWORD g_XboxBaseVertexIndex;

const char *NV2AMethodToString(DWORD dwMethod); // forward

static void DbgDumpMesh(WORD *pIndexData, DWORD dwCount);

// Determine the size (in number of floating point texture coordinates) of the texture format (indexed 0 .. 3).
// This is the reverse of the D3DFVF_TEXCOORDSIZE[0..3] macros.
int XTL::DxbxFVF_GetNumberOfTextureCoordinates(DWORD dwFVF, int aTextureIndex)
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
UINT XTL::DxbxFVFToVertexSizeInBytes(DWORD dwFVF, BOOL bIncludeTextures)
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
		Result += sizeof(XTL::D3DCOLOR);
	}

	if (dwFVF & D3DFVF_SPECULAR) {
		Result += sizeof(XTL::D3DCOLOR);
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

void XTL::EmuExecutePushBuffer
(
    X_D3DPushBuffer       *pPushBuffer,
    X_D3DFixup            *pFixup
)
{
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

		CxbxVertexShader *pVertexShader = MapXboxVertexShaderHandleToCxbxVertexShader(dwVertexShader);
		if (pVertexShader) {
			if (pVertexShader->VertexShaderInfo.NumberOfVertexStreams == 1) {
				// Note : This assumes that the only stream in use will be stream zero :
				Stride = pVertexShader->VertexShaderInfo.VertexStreams[0].HostVertexStride;
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

PGRAPHState pgraph_state = {}; // global, as inside a function it crashes (during initalization?)

void HLE_pgraph_handle_method(
	PGRAPHState *pg, // compatiblity, instead of NV2AState *d,
	unsigned int subchannel,
	unsigned int method,
	uint32_t parameter)
{
	using namespace XTL;

	LOG_INIT // Allows use of DEBUG_D3DRESULT

	// Skip all commands not intended for channel 0 (3D)
	if (subchannel > 0) {
		LOG_TEST_CASE("Pushbuffer subchannel > 0");
		return; // For now, don't even attempt to run through
	}

#if 1 // Temporarily, use this array of 16 bit elements (until HLE drawing uses 32 bit indices, like LLE)
	static INDEX16 pg__inline_elements_16[NV2A_MAX_BATCH_LENGTH];
	#define pg__inline_elements pg__inline_elements_16
#else
	#define pg__inline_elements pg->inline_elements
#endif

	switch (method) {

	case 0: {
		LOG_TEST_CASE("Pushbuffer method == 0");
		break;
	}
	case NV097_NO_OPERATION: { // 0x00000100, NV2A_NOP, No Operation, followed parameters are no use. this operation triggers DPC which is not implemented in HLE
		break;
	}
	case NV097_SET_DEPTH_FUNC: { // 0x00000354
		// Test-case : Whiplash
		SET_MASK(pg->regs[NV_PGRAPH_CONTROL_0], NV_PGRAPH_CONTROL_0_ZFUNC,
			parameter & 0xF);
		break;
	}
	case NV097_SET_DEPTH_TEST_ENABLE: { // 0x0000030C, NV2A_DEPTH_TEST_ENABLE
		// Test-case : Whiplash
		SET_MASK(pg->regs[NV_PGRAPH_CONTROL_0], NV_PGRAPH_CONTROL_0_ZENABLE,
			parameter);
		break;
	}
	case NV097_SET_TRANSFORM_CONSTANT: // 0x00000B80, NV2A_VP_UPLOAD_CONST(0), D3DPUSH_SET_TRANSFORM_CONSTANT
	case NV2A_VP_UPLOAD_CONST(1):
	case NV2A_VP_UPLOAD_CONST(2):
	case NV2A_VP_UPLOAD_CONST(3): {
		// Can't use NOINCREMENT_FLAG, parameters is constant matrix, 4X4 matrix has 16 DWORDs, maximum of 32 DWORD writes
		//load constant matrix to empty slot
		LOG_TEST_CASE("NV2A_VP_UPLOAD_CONST");
		break;
	}
	case NV097_SET_BEGIN_END: { // 0x000017FC, NV2A_VERTEX_BEGIN_END, D3DPUSH_SET_BEGIN_END, 1 DWORD parameter
		if (parameter == 0) { // Parameter == 0 means SetEnd, EndPush()
			// Trigger all draws from here
			if (pg->draw_arrays_length) {
				LOG_TEST_CASE("PushBuffer : Draw Arrays");
				assert(pg->inline_buffer_length == 0);
				assert(pg->inline_array_length == 0);
				assert(pg->inline_elements_length == 0);

#if 0 // LLE OpenGL
				pgraph_bind_vertex_attributes(d, pg->draw_arrays_max_count,
					false, 0);
				glMultiDrawArrays(pg->shader_binding->gl_primitive_mode,
					pg->gl_draw_arrays_start,
					pg->gl_draw_arrays_count,
					pg->draw_arrays_length);
#else
				// TODO : Implement this
#endif
			}
			else if (pg->inline_buffer_length) {
				LOG_TEST_CASE("PushBuffer : Inline Buffer");
				assert(pg->draw_arrays_length == 0);
				assert(pg->inline_array_length == 0);
				assert(pg->inline_elements_length == 0);

#if 0 // LLE OpenGL
				for (i = 0; i < NV2A_VERTEXSHADER_ATTRIBUTES; i++) {
					VertexAttribute *vertex_attribute = &pg->vertex_attributes[i];
					if (vertex_attribute->inline_buffer) {
						glBindBuffer(GL_ARRAY_BUFFER,
							vertex_attribute->gl_inline_buffer);
						glBufferData(GL_ARRAY_BUFFER,
							pg->inline_buffer_length
							* sizeof(float) * 4,
							vertex_attribute->inline_buffer,
							GL_DYNAMIC_DRAW);
						/* Clear buffer for next batch */
						g_free(vertex_attribute->inline_buffer);
						vertex_attribute->inline_buffer = NULL;
						glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, 0, 0);
						glEnableVertexAttribArray(i);
					}
					else {
						glDisableVertexAttribArray(i);
						glVertexAttrib4fv(i, vertex_attribute->inline_value);
					}
				}

				glDrawArrays(pg->shader_binding->gl_primitive_mode,
					0, pg->inline_buffer_length);
#else
				// TODO : Implement this
#endif
			}
			else if (pg->inline_array_length) {
//				NV2A_GL_DPRINTF(false, "Inline Array");
				assert(pg->draw_arrays_length == 0);
				assert(pg->inline_buffer_length == 0);
				assert(pg->inline_elements_length == 0);

#if 0 // LLE OpenGL
				unsigned int index_count = pgraph_bind_inline_array(d);
				glDrawArrays(pg->shader_binding->gl_primitive_mode, 0, index_count);
#else
				//DWORD vertex data array, 
				//To be used as a replacement for DrawVerticesUP, the caller needs to set the vertex format using IDirect3DDevice8::SetVertexShader before calling BeginPush. All attributes in the vertex format must be padded DWORD multiples, and the vertex attributes must be specified in the canonical FVF ordering (position followed by weight, normal, diffuse, and so on).
				// retrieve vertex shader
				XTL::DWORD dwVertexShader = g_CurrentXboxVertexShaderHandle;
				if (dwVertexShader == 0) {
					LOG_TEST_CASE("FVF Vertex Shader is null");
					dwVertexShader = -1;
				}

				// render vertices
				if (dwVertexShader != -1) {
					XTL::DWORD dwVertexStride = CxbxGetStrideFromVertexShaderHandle(dwVertexShader);
					if (dwVertexStride > 0) {
						XTL::UINT VertexCount = (pg->inline_array_length * sizeof(XTL::DWORD)) / dwVertexStride;
						CxbxDrawContext DrawContext = {};

						DrawContext.XboxPrimitiveType = (X_D3DPRIMITIVETYPE)pg->primitive_mode;
						DrawContext.dwVertexCount = VertexCount;
						DrawContext.pXboxVertexStreamZeroData = pg->inline_array;
						DrawContext.uiXboxVertexStreamZeroStride = dwVertexStride;
						DrawContext.hVertexShader = dwVertexShader;

						CxbxDrawPrimitiveUP(DrawContext);
					}
				}
#endif
			}
			else if (pg->inline_elements_length) {
//				NV2A_GL_DPRINTF(false, "Inline Elements");
				assert(pg->draw_arrays_length == 0);
				assert(pg->inline_buffer_length == 0);
				assert(pg->inline_array_length == 0);

#if 0 // LLE OpenGL
				uint32_t max_element = 0;
				uint32_t min_element = (uint32_t)-1;
				for (i = 0; i<pg->inline_elements_length; i++) {
					max_element = MAX(pg->inline_elements[i], max_element);
					min_element = MIN(pg->inline_elements[i], min_element);
				}

				pgraph_bind_vertex_attributes(d, max_element + 1, false, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg->gl_element_buffer);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER,
					pg->inline_elements_length * 4,
					pg->inline_elements,
					GL_DYNAMIC_DRAW);
				glDrawRangeElements(pg->shader_binding->gl_primitive_mode,
					min_element, max_element,
					pg->inline_elements_length,
					GL_UNSIGNED_INT,
					(void*)0);
#else
				if (IsValidCurrentShader()) {
					unsigned int uiIndexCount = pg->inline_elements_length;
					CxbxDrawContext DrawContext = {};

					DrawContext.XboxPrimitiveType = (X_D3DPRIMITIVETYPE)pg->primitive_mode;
					DrawContext.dwVertexCount = EmuD3DIndexCountToVertexCount(DrawContext.XboxPrimitiveType, uiIndexCount);
					DrawContext.hVertexShader = g_CurrentXboxVertexShaderHandle;
					DrawContext.pIndexData = pg__inline_elements; // Used by GetVerticesInBuffer

					CxbxDrawIndexed(DrawContext);
				}
#endif
			}
			else {
				LOG_TEST_CASE("EMPTY NV097_SET_BEGIN_END");
			}
		}
		else {
//			NV2A_GL_DGROUP_BEGIN("NV097_SET_BEGIN_END: 0x%x", parameter);
			assert(parameter <= NV097_SET_BEGIN_END_OP_POLYGON);

			pg->primitive_mode = parameter; // Retrieve the D3DPRIMITIVETYPE info in parameter

			CxbxUpdateNativeD3DResources();

			pg->inline_elements_length = 0;
			pg->inline_array_length = 0;
			pg->inline_buffer_length = 0;
			pg->draw_arrays_length = 0;
			pg->draw_arrays_max_count = 0;
		}

		break;
	}
	case NV097_ARRAY_ELEMENT16: { // 0x1800, NV2A_VB_ELEMENT_U16
		//LOG_TEST_CASE("NV2A_VB_ELEMENT_U16");
		// Test-case : Turok (in main menu)
		// Test-case : Hunter Redeemer
		// Test-case : Otogi (see https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/pull/1113#issuecomment-385593814)
		assert(pg->inline_elements_length < NV2A_MAX_BATCH_LENGTH);
		pg__inline_elements[
			pg->inline_elements_length++] = parameter & 0xFFFF;
		pg__inline_elements[
			pg->inline_elements_length++] = parameter >> 16;
		break;
	}
	case NV097_ARRAY_ELEMENT32: { // 0x1808, NV2A_VB_ELEMENT_U32, Index Array Data
		//LOG_TEST_CASE("NV2A_VB_ELEMENT_U32");
		// Test-case : Turok (in main menu)
		assert(pg->inline_elements_length < NV2A_MAX_BATCH_LENGTH);
		pg__inline_elements[
			pg->inline_elements_length++] = parameter;
		break;
	}
	case NV097_INLINE_ARRAY: { // 0x1818, NV2A_VERTEX_DATA, parameter size= dwCount*DWORD, represent D3DFVF data
		assert(pg->inline_array_length < NV2A_MAX_BATCH_LENGTH);
		pg->inline_array[
			pg->inline_array_length++] = parameter;
		break;
	}
	case NV097_SET_TRANSFORM_EXECUTION_MODE: { // 0x00001E94, NV2A_ENGINE
		// Test-case : Whiplash
		SET_MASK(pg->regs[NV_PGRAPH_CSV0_D], NV_PGRAPH_CSV0_D_MODE,
			GET_MASK(parameter,
				NV097_SET_TRANSFORM_EXECUTION_MODE_MODE));
		SET_MASK(pg->regs[NV_PGRAPH_CSV0_D], NV_PGRAPH_CSV0_D_RANGE_MODE,
			GET_MASK(parameter,
				NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE));
		break;
	}
	case NV097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN: { // 0x00001E98, NV2A_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN
		// Test-case : Whiplash
		pg->enable_vertex_program_write = parameter;
		break;
	}
	case NV097_SET_TRANSFORM_CONSTANT_LOAD: { // 0x00001EA4, NV2A_VP_UPLOAD_CONST_ID, D3DPUSH_SET_TRANSFORM_CONSTANT_LOAD
		// Add 96 to constant index parameter, one parameter=CONSTANT + 96
		// Retrieve transform constant index and add 96 to it.
		LOG_TEST_CASE("NV2A_VP_UPLOAD_CONST_ID");
		break;
	}
	default: { // default case, handling any other unknown methods.
		char message[256] = {};
		sprintf(message, "Unhandled PushBuffer Operation : %s (0x%.04X)", NV2AMethodToString(method), method);
		LOG_TEST_CASE(message);
		break;
	}
	} // switch
}

// For now, skip the cache, but handle the pgraph method directly
// Note : Here's where the method gets multiplied by four!
// Note 2 : pg is read from local scope, and ni is unused (same in LLE)
#define CACHE_PUSH(subc, mthd, word, ni) \
	HLE_pgraph_handle_method(pg, subc, mthd << 2, word)

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

extern void XTL::EmuExecutePushBufferRaw
(
	void *pPushData,
	uint32_t uSizeInBytes
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

	// EmuNV2A_PGRAPH immitation
	PGRAPHState *pg = &pgraph_state;

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
