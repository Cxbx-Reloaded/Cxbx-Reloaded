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

#include <assert.h> // For assert()

#include "CxbxKrnl/Emu.h"
#include "CxbxKrnl/EmuXTL.h"
#include "CxbxKrnl/EmuD3D8Types.h" // For X_D3DFORMAT
#include "CxbxKrnl/ResourceTracker.h"
#include "devices/video/nv2a.h" // For PGRAPHState
#include "devices/video/nv2a_int.h" // For NV** defines

// TODO: Find somewhere to put this that doesn't conflict with XTL::
extern void EmuUpdateActiveTextureStages();
#ifdef CXBX_USE_D3D9
extern DWORD g_XboxBaseVertexIndex;
#endif

uint32  XTL::g_dwPrimaryPBCount = 0;
uint32 *XTL::g_pPrimaryPB = nullptr;

bool XTL::g_bStepPush = false;
bool XTL::g_bSkipPush = false;
bool XTL::g_bBrkPush  = false;

bool g_bPBSkipPusher = false;

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

	DWORD Stride = 0;

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
				#define COMMAND_TYPE_NORMAL 0 // Note : actual name not documented
				#define COMMAND_TYPE_JUMP   1
				#define COMMAND_TYPE_CALL   2
		uint32_t        method       : 11;       /*  2 .. 12 */
		uint32_t        subchannel   : 3;        /* 13 .. 15 */
		uint32_t        flags        : 2;        /* 16 .. 17 */
			// See https://envytools.readthedocs.io/en/latest/hw/fifo/dma-pusher.html#nv4-method-submission-commands
				#define COMMAND_FLAGS_METHOD                      0
				#define COMMAND_FLAGS_SLI_CONDITIONAL             1
				#define COMMAND_FLAGS_RETURN                      2
				#define COMMAND_FLAGS_LONG_NON_INCREASING_METHODS 3
		uint32_t        method_count : 11;       /* 18 .. 28 */
		uint32_t        instruction  : 3;        /* 29 .. 31 */
				#define COMMAND_INSTRUCTION_INCREASING_METHODS     0
				#define COMMAND_INSTRUCTION_OLD_JUMP               1
				#define COMMAND_INSTRUCTION_NON_INCREASING_METHODS 2
	};
	#define COMMAND_WORD_MASK_OLD_JMP 0x1FFFFFFC /*  2 .. 31 */
	#define COMMAND_WORD_MASK_JMP 0xFFFFFFFC     /*  2 .. 28 */
} nv_fifo_command;

PGRAPHState pgraph_state;

extern void XTL::EmuExecutePushBufferRaw
(
	void *pPushData,
	uint32_t uSizeInBytes
)
{
	// Test case : Azurik (see https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/360)
	// Test-case : Crash 'n' Burn [45530014]
	// Test-case : CrimsonSea [4B4F0002]
	// Test-case : Freedom Fighters
	// Test-case : Hot Wheels Stunt Track Challenge [54510089] (while running hw2F.xbe)
	// Test case : Hunter Redeemer
	// Test-case : Inside Pitch 2003 [4D530034]
	// Test-case : Need for Speed Most Wanted [4541007B]
	// Test case : Otogi (see https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/pull/1113#issuecomment-385593814)
	// Test-case : Prince of Persia: The Sands of Time [5553001d]
	// Test case : RalliSport (see https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/904#issuecomment-362929801)
	// Test-case : RPM Tuning [Top Gear RPM Tuning] [4B420007]
	// Test-case : SpyHunter 2 [4D57001B]
	// Test case : Star Wars Jedi Academy (see https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/904#issuecomment-362929801)
	// Test case : Turok (in main menu)

	assert(pPushData);
	assert(uSizeInBytes >= 4);

	if (g_bSkipPush) {
		return;
	}

	static INDEX16 pg__inline_elements[NV2A_MAX_BATCH_LENGTH]; // Temporary 16 bit elements (until Direct3D drawing uses 32 bit indices)

	// EmuNV2A_PGRAPH immitation
	PGRAPHState *pg = &pgraph_state;
	memset(pg, 0, sizeof(PGRAPHState));

	// DMA Pusher state -- see https://envytools.readthedocs.io/en/latest/hw/fifo/dma-pusher.html#pusher-state
#if 0
	xbaddr dma_pushbuffer; // the pushbuffer and IB DMA object
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
	uint32_t dcount_shadow; // [NV5:] Number of already-processed methods in cmd]
	bool subr_active; // Subroutine active
	uint32_t *subr_return; // Subroutine return address
	bool big_endian; // Pushbuffer endian switch

	// DMA troubleshooting values -- see https://envytools.readthedocs.io/en/latest/hw/fifo/dma-pusher.html#errors
	uint32_t *dma_get_jmp_shadow; // value of dma_get before the last jump
	uint32_t rsvd_shadow; // the first word of last-read command
	uint32_t data_shadow; // the last-read data word

	// Initialize working variables
	dma_get = (uint32_t*)pPushData;
	dma_put = (uint32_t*)((xbaddr)pPushData + uSizeInBytes);
	dma_limit = (uint32_t*)((xbaddr)pPushData + uSizeInBytes);
	dma_state = {};
	dcount_shadow = 0;
	subr_active = false;
	subr_return = 0;
	big_endian = false;
	dma_get_jmp_shadow = 0;
	rsvd_shadow = 0;
	data_shadow = 0;

	// See https://envytools.readthedocs.io/en/latest/hw/fifo/dma-pusher.html#the-pusher-pseudocode-pre-gf100
    while (dma_get != dma_put) {
		// Check if loop reaches end of pushbuffer
		if (dma_get >= dma_limit) {
			LOG_TEST_CASE("Last pushbuffer instruction exceeds END of Data");
			break; // from while
		}

		nv_fifo_command command;

		// Read the command DWORD from the current push buffer pointer
		command.word = *dma_get++;

		// Check and handle command type, then instruction, then flags
		switch (command.type) {
		case COMMAND_TYPE_NORMAL:
			break; // fall through
		case COMMAND_TYPE_JUMP:
			LOG_TEST_CASE("Pushbuffer COMMAND_TYPE_JUMP");
			dma_get_jmp_shadow = dma_get;
			dma_get = (uint32_t *)(CONTIGUOUS_MEMORY_BASE | (command.word & COMMAND_WORD_MASK_JMP));
			continue;
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
			dma_get = (uint32_t *)(CONTIGUOUS_MEMORY_BASE | (command.word & COMMAND_WORD_MASK_JMP));
			continue;
		default:
			LOG_TEST_CASE("Pushbuffer COMMAND_TYPE unknown");
			return; // For now, don't even attempt to run through
		} // switch type

		switch (command.instruction) {
		case COMMAND_INSTRUCTION_INCREASING_METHODS:
			dma_state.ni = false;
			break;
		case COMMAND_INSTRUCTION_OLD_JUMP:
			LOG_TEST_CASE("Pushbuffer COMMAND_INSTRUCTION_OLD_JUMP");
			dma_get_jmp_shadow = dma_get;
			dma_get = (uint32_t *)(CONTIGUOUS_MEMORY_BASE | (command.word & COMMAND_WORD_MASK_OLD_JMP));
			continue;
		case COMMAND_INSTRUCTION_NON_INCREASING_METHODS:
			dma_state.ni = true;
			break;
		default:
			LOG_TEST_CASE("Pushbuffer COMMAND_INSTRUCTION unknown");
			return; // For now, don't even attempt to run through
		} // switch instruction

		switch (command.flags) {
		case COMMAND_FLAGS_METHOD: // Decode push buffer method & size (inverse of D3DPUSH_ENCODE)
			dma_state.mthd = command.method;
			dma_state.subc = command.subchannel;
			dma_state.mcnt = command.method_count;
			break; // fall through
		case COMMAND_FLAGS_RETURN: // Note : NV2A return is said not to work?
			if (command.word != 0x00020000) {
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
			return; // For now, don't even attempt to run through
		} // switch flags

		/* no command active - this is the first word of a new one */
		rsvd_shadow = command.word;
		dcount_shadow = 0;

		// Validate count
		if (dma_state.mcnt == 0) {
			// Test case : Turok (in main menu)
			//LOG_TEST_CASE("Pushbuffer count == 0");
			// When this happens, just skip the method
			continue;
		}

		/* data word of methods command */
		data_shadow = *dma_get;
#if 0
		if (!PULLER_KNOWS_MTHD(dma_state.mthd)) {
			throw DMA_PUSHER(INVALID_MTHD);
			return; // For now, don't even attempt to run through
		}

#endif
#if 0
		CACHE_PUSH(dma_state.subc, dma_state.mthd, word, dma_state.ni);
#endif

		// Skip all commands not intended for channel 0 (3D)
		if (command.subchannel > 0) {
			LOG_TEST_CASE("Pushbuffer subchannel > 0");
			dma_get += dma_state.mcnt; // Skip over arguments, so it always points to the next unhandled DWORD
			continue;
		}

        // Interpret 3D method
		while (dma_state.mcnt > 0) {
			uint32_t parameter = *dma_get++;
			DWORD dwMethod = dma_state.mthd << 2;
			switch (dwMethod) {

			case 0:
				LOG_TEST_CASE("Pushbuffer method == 0");
				break;

			case NV2A_VERTEX_BEGIN_END: { // 0x000017FC, NVPB_SetBeginEnd, 1 DWORD parameter, D3DPUSH_SET_BEGIN_END, NV097_NO_OPERATION
				if (parameter == 0) { // Parameter == 0 means SetEnd, EndPush()
					// Trigger all draws from here
					CxbxUpdateNativeD3DResources();

					if (pg->draw_arrays_length) {
//						NV2A_GL_DPRINTF(false, "Draw Arrays");
						assert(pg->inline_buffer_length == 0);
						assert(pg->inline_array_length == 0);
						assert(pg->inline_elements_length == 0);

#if 0
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
//						NV2A_GL_DPRINTF(false, "Inline Buffer");
						assert(pg->draw_arrays_length == 0);
						assert(pg->inline_array_length == 0);
						assert(pg->inline_elements_length == 0);

#if 0
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
//						NV2A_GL_DPRINTF(false, "Inline Array");
						assert(pg->draw_arrays_length == 0);
						assert(pg->inline_buffer_length == 0);
						assert(pg->inline_elements_length == 0);

#if 0
						unsigned int index_count = pgraph_bind_inline_array(d);
						glDrawArrays(pg->shader_binding->gl_primitive_mode, 0, index_count);
#else
						//DWORD vertex data array, 
						//To be used as a replacement for DrawVerticesUP, the caller needs to set the vertex format using IDirect3DDevice8::SetVertexShader before calling BeginPush. All attributes in the vertex format must be padded DWORD multiples, and the vertex attributes must be specified in the canonical FVF ordering (position followed by weight, normal, diffuse, and so on).
						// retrieve vertex shader
						DWORD dwVertexShader = g_CurrentXboxVertexShaderHandle;
						if (dwVertexShader == 0) {
							LOG_TEST_CASE("FVF Vertex Shader is null");
							dwVertexShader = -1;
						}

						// render vertices
						if (dwVertexShader != -1) {
							DWORD dwVertexStride = CxbxGetStrideFromVertexShaderHandle(dwVertexShader);
							if (dwVertexStride > 0) {
								UINT VertexCount = (pg->inline_array_length * sizeof(DWORD)) / dwVertexStride;
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

//						NV2A_GL_DPRINTF(false, "Inline Elements");

						assert(pg->draw_arrays_length == 0);
						assert(pg->inline_buffer_length == 0);
						assert(pg->inline_array_length == 0);

#if 0
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
						if (!g_bPBSkipPusher) {
							if (IsValidCurrentShader()) {
								unsigned int uiIndexCount = pg->inline_elements_length;
								CxbxDrawContext DrawContext = {};

								DrawContext.XboxPrimitiveType = (X_D3DPRIMITIVETYPE)pg->primitive_mode;
								DrawContext.dwVertexCount = EmuD3DIndexCountToVertexCount(DrawContext.XboxPrimitiveType, uiIndexCount);
								DrawContext.hVertexShader = g_CurrentXboxVertexShaderHandle;
								DrawContext.pIndexData = pg__inline_elements; // Used by GetVerticesInBuffer

								CxbxDrawIndexed(DrawContext);
							}
						}
#endif
					}
					else {
						LOG_TEST_CASE("EMPTY NV097_SET_BEGIN_END");
					}

					break; // EndPush(), done with BeginPush()
				}

				pg->primitive_mode = parameter; // Retrieve the D3DPRIMITIVETYPE info in parameter
				break;
			}

			case NV2A_VERTEX_DATA: { // 0x1818, NVPB_InlineVertexArray, parameter size= dwCount*DWORD, represent D3DFVF data. NV097_INLINE_ARRAY
				assert(pg->inline_array_length < NV2A_MAX_BATCH_LENGTH);
				pg->inline_array[
					pg->inline_array_length++] = parameter;
				break;
			}

			case NV2A_VB_ELEMENT_U32: { // 0x1808, NVPB_FixLoop, Index Array Data, . NV097_ARRAY_ELEMENT32
				// Test case : Turok (in main menu)
				//LOG_TEST_CASE("NV2A_VB_ELEMENT_U32");
				assert(pg->inline_elements_length < NV2A_MAX_BATCH_LENGTH);
#if 0
				pg->inline_elements[
#else
				pg__inline_elements[
#endif
					pg->inline_elements_length++] = parameter;
				break;
			}

			case NV2A_VB_ELEMENT_U16: { // 0x1800, NVPB_InlineIndexArray,   NV097_ARRAY_ELEMENT16
				//LOG_TEST_CASE("NV2A_VB_ELEMENT_U16");
				// Test case : Turok (in main menu)
				// Test case : Hunter Redeemer
				// Test case : Otogi (see https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/pull/1113#issuecomment-385593814)
				assert(pg->inline_elements_length < NV2A_MAX_BATCH_LENGTH);
#if 0
				pg->inline_elements[
#else
				pg__inline_elements[
#endif
					pg->inline_elements_length++] = parameter & 0xFFFF;
#if 0
				pg->inline_elements[
#else
				pg__inline_elements[
#endif
					pg->inline_elements_length++] = parameter >> 16;
				break;
			}

			case NV2A_NOP: { // 0x00000100, No Operation, followed parameters are no use. this operation triggers DPC which is not implemented in HLE
				//EmuWarning("NOP PushBuffer Operation (0x%.04X, %d)", dwMethod, dwCount);
				break;
			}

			case NV2A_VP_UPLOAD_CONST_ID: { // 0x00001ea4, D3DPUSH_SET_TRANSFORM_CONSTANT_LOAD  // Add 96 to constant index parameter, one parameter=CONSTANT + 96
				//retrive transform constant index and add 96 to it.
				LOG_TEST_CASE("NV2A_VP_UPLOAD_CONST_ID");
				//EmuWarning("TRANSFORM CONSTANT LOAD PushBuffer Operation (0x%.04X, %d)", dwMethod, dwCount);
				//EmuWarning("TRANSFORM CONSTANT LOAD PushBuffer Operation  Constant = %d)", *dma_get);
				break;
			}

			case NV2A_VP_UPLOAD_CONST(0): // 0x00000b80, D3DPUSH_SET_TRANSFORM_CONSTANT      // Can't use NOINCREMENT_FLAG, parameters is constant matrix, 4X4 matrix hasa 16 DWRDS, maximum of 32 DWORD writes
			case NV2A_VP_UPLOAD_CONST(1):
			case NV2A_VP_UPLOAD_CONST(2):
			case NV2A_VP_UPLOAD_CONST(3): {
				//load constant matrix to empty slot, then break;
				LOG_TEST_CASE("NV2A_VP_UPLOAD_CONST");
				break;
			}

			default: { // default case, handling any other unknown methods.
				char message[256] = {};
				sprintf(message, "Unhandled PushBuffer Operation : %s (0x%.04X)", NV2AMethodToString(dwMethod), dwMethod);
				LOG_TEST_CASE(message);
				break;
			}
			} // switch

			if (!dma_state.ni) {
				dma_state.mthd++;
			}

			dma_state.mcnt--;
			dcount_shadow++;
		} // while (dma_state.mcnt > 0)
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
