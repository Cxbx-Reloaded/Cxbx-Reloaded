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

#include "CxbxKrnl/Emu.h"
#include "CxbxKrnl/EmuXTL.h"
#include "CxbxKrnl/EmuD3D8Types.h" // For X_D3DFORMAT
#include "CxbxKrnl/ResourceTracker.h"
#include "devices/video/nv2a_int.h" // For NV** defines

// TODO: Find somewhere to put this that doesn't conflict with XTL::
extern void EmuUpdateActiveTextureStages();
#ifdef CXBX_USE_D3D9
extern DWORD g_XboxBaseVertexIndex;
#endif

uint32  XTL::g_dwPrimaryPBCount = 0;
uint32 *XTL::g_pPrimaryPB = 0;

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
	if (pFixup != NULL)
	{
		EmuWarning("PushBuffer has fixups\n");
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
			When IDirect3DDevice8::RunPushBuffer is called with a fix - up object specified,
			it will parse the fix - up data pointed to by pFixup and with a byte offset of Run.
			The fix - up data is encoded as follows.The first DWORD is the size, in bytes,
			of the push - buffer fix - up to be modified.The second DWORD is the offset, in bytes,
			from the start of the push - buffer where the fix - up is to be modified.
			The subsequent DWORDS are the data to be copied.This encoding repeats for every fix - up to be done,
			until it terminates with a size value of 0xffffffff.
			The offsets must be in an increasing order.
			*/
		}
	}
#ifdef _DEBUG_TRACK_PB
	DbgDumpPushBuffer((DWORD*)pPushBuffer->Data, pPushBuffer->Size);
#endif

    EmuExecutePushBufferRaw((DWORD*)pPushBuffer->Data, pPushBuffer->Size);

    return;
}

DWORD CxbxGetStrideFromVertexShaderHandle(DWORD dwVertexShader)
{
	using namespace XTL;

	DWORD Stride = 0;

	if (VshHandleIsVertexShader(dwVertexShader)) {
		// Test-case : Crash 'n' Burn [45530014]
		// Test-case : CrimsonSea [4B4F0002]
		// Test-case : Hot Wheels Stunt Track Challenge [54510089] 
		// Test-case : Inside Pitch 2003 [4D530034]
		// Test-case : Need for Speed Most Wanted [4541007B]
		// Test-case : Prince of Persia: The Sands of Time [5553001d]
		// Test-case : RPM Tuning [Top Gear RPM Tuning] [4B420007]
		// Test-case : SpyHunter 2 [4D57001B]
		LOG_TEST_CASE("Non-FVF Vertex Shaders not yet (completely) supported for PushBuffer emulation!");

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

extern void XTL::EmuExecutePushBufferRaw
(
    DWORD                 *pdwPushData,
	ULONG					dwSize
)
{
    if(g_bSkipPush)
        return;

	if (!pdwPushData) {
		EmuWarning("pdwPushData is null");
		return;
	}

    DWORD *pdwOrigPushData = pdwPushData;

    INDEX16 *pIndexData = NULL;
    PVOID pVertexData = NULL;

    //DWORD dwVertexShader = -1;

    // cache of last 4 indices
	INDEX16 pIBMem[4] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};

    X_D3DPRIMITIVETYPE  XboxPrimitiveType = X_D3DPT_INVALID;

	CxbxUpdateNativeD3DResources();

#ifdef _DEBUG_TRACK_PB
    bool bShowPB = false;

    g_PBTrackTotal.insert(pdwPushData);

    if (g_PBTrackShowOnce.exists(pdwPushData)) {
        g_PBTrackShowOnce.remove(pdwPushData);

        printf("\n");
        printf("\n");
        printf("  PushBuffer@0x%.08X...\n", pdwPushData);
        printf("\n");

        bShowPB = true;
    }

#define LOG_TRACK_PB(message, ...) \
	if (bShowPB) { \
		printf(message, __VA_ARGS__); \
	}
#else
#define LOG_TRACK_PB(message, ...)
#endif

    // static IDirect3DVertexBuffer *pVertexBuffer = nullptr;

    static uint maxIBSize = 0;
	UINT8 * pPushDataEnd= (UINT8 *)pdwPushData + dwSize;

    while (true) {

		// Check if loop reaches end of pushbuffer
		if ((UINT8 *)pdwPushData >= pPushDataEnd) {
			if ((UINT8 *)pdwPushData > pPushDataEnd) {
				LOG_TEST_CASE("Last pushbuffer instruction exceeds END of Data");
			}

			break; // from while(true)
		}

		// Sources : NV_PFIFO_CACHE1_DMA_STATE, https://github.com/haxar/xexec/blob/master/hw/gpu/nv2a.c#L220 and
		// https://github.com/PatrickvL/Dxbx/blob/master/Source/Delphi/src/DxbxKrnl/EmuD3D8/uPushBuffer.pas#L126
		union {
			uint32_t            field;
			struct {
				uint32_t        jump         : 1;       /*  0 */
				uint32_t        call         : 1;       /*  1 */
				uint32_t        method       : 11;      /*  2 */
				uint32_t        subchannel   : 3;       /* 13 */
				uint32_t        bit_15       : 1;       /* 16 */
				uint32_t        bit_16       : 1;       /* 17 */
				uint32_t        method_count : 11;      /* 18 */
				uint32_t        instruction  : 3;       /* 29 */
			};
			uint32_t            jmp_inst_addrress : 29;
		} command;

		// Known values for the command.instruction bitmask (three bits = 2^3 = 6 instructions max)
		typedef enum {
			Increment = 0,
			Jump = 1,
			NoIncrement = 2
		} Instruction;

		// Read the command DWORD from the current push buffer pointer
		command.field = *pdwPushData;

		// First, check and handle a jump or call indicator
		if (command.jump) {
			LOG_TEST_CASE("Pushbuffer jump");
			pdwPushData = (DWORD*)(CONTIGUOUS_MEMORY_BASE + (command.field & ~3));
			continue;
		}

		if (command.call) {
			LOG_TEST_CASE("Pushbuffer call");
			// Note : NV2A return is said not to work, so handle this as a jump :
			pdwPushData = (DWORD*)(CONTIGUOUS_MEMORY_BASE + (command.field & ~3));
			continue;
		}

		// Decode and handle the push buffer instruction
		bool bInc;
		switch (command.instruction) {
		case Instruction::Increment:
			bInc = true;
			break;
		case Instruction::Jump:
			LOG_TEST_CASE("Pushbuffer instruction jump");
			pdwPushData = (DWORD*)(CONTIGUOUS_MEMORY_BASE + command.jmp_inst_addrress);
			continue;
		case Instruction::NoIncrement:
			bInc = false;
			break;
		default:
			LOG_TEST_CASE("Pushbuffer instruction unknown!");
			// Skip the method itself, and to be safe, also the method data
			pdwPushData += 1 + command.method_count;
			continue;
		}

		// Decode push buffer method & size (inverse of D3DPUSH_ENCODE)
		NV2AMETHOD dwMethod = command.method << 2; // Note : two least significant bits are zero (not a jmp or call)
		DWORD dwCount = command.method_count;

		// Validate count
		if (dwCount == 0) {
			// Test case : Turok (in main menu)
			//LOG_TEST_CASE("Pushbuffer count == 0");
			// When this happens, just skip the method
			++pdwPushData;
			continue;
		}

		// Remember the address of the arguments
		DWORD *pdwPushArguments = ++pdwPushData;
		// Skip over the arguments already, so it always points to the next unhandled DWORD.
		pdwPushData += dwCount;

		// Skip all commands not intended for channel 0 (3D)
		if (command.subchannel > 0) {
			LOG_TEST_CASE("Pushbuffer subchannel > 0");
			continue;
		}

        // Interpret 3D method
		while (dwCount > 0) {
			// Test case : Azurik (see https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/360)
			// Test case : RalliSport (see https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/904#issuecomment-362929801)
			// Test case : Star Wars Jedi Academy (see https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/904#issuecomment-362929801)
			// Assume the command will be handled completely (down-adjustments may happen)
			int HandledCount = dwCount;

			switch (dwMethod) {

			case 0: {
				LOG_TEST_CASE("Pushbuffer method == 0");
				break;
			}

			case NV2A_VERTEX_BEGIN_END: { // 0x000017FC, NVPB_SetBeginEnd, 1 DWORD parameter, D3DPUSH_SET_BEGIN_END, NV097_NO_OPERATION
				LOG_TRACK_PB("  NVPB_SetBeginEnd(");

				// Parameter == 0 means SetEnd, EndPush()
				if (*pdwPushArguments == 0) {
					LOG_TRACK_PB("DONE)\n");
					break; // EndPush(), done with BeginPush()
				}

				// BeginPush(), To be used as a replacement for DrawVerticesUP, the caller needs to set the vertex format using IDirect3DDevice8::SetVertexShader before calling BeginPush. All attributes in the vertex format must be padded DWORD multiples, and the vertex attributes must be specified in the canonical FVF ordering (position followed by weight, normal, diffuse, and so on).
				LOG_TRACK_PB("PrimitiveType := %d)\n", *pdwPushArguments);

				// Retrieve the D3DPRIMITIVETYPE info in parameter
				XboxPrimitiveType = (X_D3DPRIMITIVETYPE)*pdwPushArguments;
				break;
			}

			case NV2A_VERTEX_DATA: { // 0x1818, NVPB_InlineVertexArray, parameter size= dwCount*DWORD, represent D3DFVF data. NV097_INLINE_ARRAY
				//DWORD vertex data array, 
				//To be used as a replacement for DrawVerticesUP, the caller needs to set the vertex format using IDirect3DDevice8::SetVertexShader before calling BeginPush. All attributes in the vertex format must be padded DWORD multiples, and the vertex attributes must be specified in the canonical FVF ordering (position followed by weight, normal, diffuse, and so on).
				pVertexData = pdwPushArguments;

				// retrieve vertex shader
				DWORD dwVertexShader = g_CurrentXboxVertexShaderHandle;

				if (dwVertexShader == 0) {
					LOG_TEST_CASE("FVF Vertex Shader is null");
					dwVertexShader = -1;
				}

				/*
				// create cached vertex buffer only once, with maxed out size
				if (pVertexBuffer == 0) {
					HRESULT hRet = g_pD3DDevice->CreateVertexBuffer(
						2047*sizeof(DWORD),
						D3DUSAGE_WRITEONLY,
						dwVertexShader,
						D3DPOOL_MANAGED,
						&pVertexBuffer
#ifdef CXBX_USE_D3D9
						, nullptr
#endif
					);
					if (FAILED(hRet))
						CxbxKrnlCleanup("Unable to create vertex buffer cache for PushBuffer emulation (0x1818, dwCount : %d)", dwCount);
				}

				// copy vertex data
				{
					uint08 *pData = 0;
					HRESULT hRet = pVertexBuffer->Lock(0, dwCount*4, &pData, 0);

					if (FAILED(hRet))
						CxbxKrnlCleanup("Unable to lock vertex buffer cache for PushBuffer emulation (0x1818, dwCount : %d)", dwCount);

					memcpy(pData, pVertexData, dwCount*4);
					pVertexBuffer->Unlock();
				}
				*/

				LOG_TRACK_PB("NVPB_InlineVertexArray(...)\n");
				LOG_TRACK_PB("  dwCount : %d\n", dwCount);
				LOG_TRACK_PB("  dwVertexShader : 0x%08X\n", dwVertexShader);

				// render vertices
				if (dwVertexShader != -1) {
					DWORD dwVertexStride = CxbxGetStrideFromVertexShaderHandle(dwVertexShader);
					if (dwVertexStride > 0) {
						UINT VertexCount = (dwCount * sizeof(DWORD)) / dwVertexStride;
						CxbxDrawContext DrawContext = {};

						DrawContext.XboxPrimitiveType = XboxPrimitiveType;
						DrawContext.dwVertexCount = VertexCount;
						DrawContext.pXboxVertexStreamZeroData = pVertexData;
						DrawContext.uiXboxVertexStreamZeroStride = dwVertexStride;
						DrawContext.hVertexShader = dwVertexShader;

						CxbxDrawPrimitiveUP(DrawContext);
					}
				}

				break;
			}

			case NV2A_VB_ELEMENT_U32: { // 0x1808, NVPB_FixLoop, Index Array Data, . NV097_ARRAY_ELEMENT32
				// Test case : Turok (in main menu)
				//LOG_TEST_CASE("NV2A_VB_ELEMENT_U32");
#ifdef _DEBUG_TRACK_PB
				if (bShowPB) {
					LOG_TRACK_PB("  NVPB_FixLoop(%u)\n\n  Index Array Data...\n", dwCount);
					INDEX16 *pIndices = (INDEX16*)(pdwPushArguments + 1);
					for (uint s = 0;s < dwCount; s++) {
						if (s % 8 == 0)
							printf("\n  ");

						printf("  %.04X", *pIndices++);
					}

					printf("\n");
					printf("\n");
				}
#endif
				//the pdwPushArguments pointing to the first unhandled DWORD already, no need to add offset
				INDEX16 *pIndices = (INDEX16*)pdwPushArguments;
				//!!!!!!don't understand this loop, the pIBMem[] is only 4 WORD, how can we copy the data with known size into it?******************************
				for (uint mi = 0; mi < dwCount; mi++) {
					pIBMem[mi + 2] = pIndices[mi];
				}

				// perform rendering
				if (pIBMem[0] != 0xFFFF) {
					UINT uiIndexCount = dwCount + 2;
#ifdef _DEBUG_TRACK_PB
					if (!g_PBTrackDisable.exists(pdwOrigPushData))
#endif
						// render indexed vertices
					{
						if (!g_bPBSkipPusher) {
							if (IsValidCurrentShader()) {
								CxbxDrawContext DrawContext = {};

								DrawContext.XboxPrimitiveType = XboxPrimitiveType;
								DrawContext.dwVertexCount = EmuD3DIndexCountToVertexCount(XboxPrimitiveType, uiIndexCount);
								DrawContext.hVertexShader = g_CurrentXboxVertexShaderHandle;
								DrawContext.pIndexData = pIBMem; // Used by GetVerticesInBuffer

								CxbxDrawIndexed(DrawContext);
							}
						}
					}
				}

				break;
			}

			case NV2A_VB_ELEMENT_U16: { // 0x1800, NVPB_InlineIndexArray,   NV097_ARRAY_ELEMENT16
				// Test case : Turok (in main menu)
				// Test case : Hunter Redeemer
				// Test case : Otogi (see https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/pull/1113#issuecomment-385593814)
				//LOG_TEST_CASE("NV2A_VB_ELEMENT_U16");
				// Points pIndexData to the first parameter.
				pIndexData = (INDEX16*)pdwPushArguments;
				//multiply dwCount from DWORD count to WORD count
				UINT dwIndexCount = dwCount * 2;
#if 0 // TODO : Is the following nonsense?
				//if no increment is not set, then there is one WORD less then the total dwCount*2 WORD data.
				//this definition is purely my guess, need confirmation.
				if (!bInc) {
					dwIndexCount -= 1;
				}
#endif

#ifdef _DEBUG_TRACK_PB
				if (bShowPB) {
					LOG_TRACK_PB("  NVPB_InlineIndexArray(0x%p, %u)...\n\n  Index Array Data...\n", pIndexData, dwIndexCount);
					INDEX16 *pIndices = pIndexData;
					for (uint s = 0;s < dwIndexCount; s++) {
						if (s % 8 == 0)
							printf("\n  ");

						printf("  %.04X", *pIndices++);
					}

					printf("\n");

					XTL::IDirect3DVertexBuffer *pActiveVB = NULL;
					D3DVERTEXBUFFER_DESC VBDesc;
					D3DLockData *pVBData = nullptr;
					UINT  uiStride;

					// retrieve stream data
					g_pD3DDevice->GetStreamSource(0, &pActiveVB,
#ifdef CXBX_USE_D3D9
						nullptr, // pOffsetInBytes
#endif
						&uiStride);

					pActiveVB->GetDesc(&VBDesc);
					pActiveVB->Unlock(); // unlock just in case
					//pActiveVB->Lock(0, 0, &pVBData, D3DLOCK_READONLY); // grab ptr
					// print out stream data
					{
						printf("\n");
						printf("  Vertex Stream Data (0x%.08X)...\n", pActiveVB);
						printf("\n");
						printf("  Format : %d\n", VBDesc.Format);
						printf("  Size   : %d bytes\n", VBDesc.Size);
						printf("  FVF    : 0x%.08X\n", VBDesc.FVF);
						printf("\n");
					}

					//pActiveVB->Unlock();
					pActiveVB->Release();
					DbgDumpMesh((WORD*)pIndexData, dwIndexCount);
				}
#endif

				// perform rendering
				{
					// copy index data
					{
						// remember last 2 indices
						if (dwIndexCount >= 2) { // TODO : Is 2 indices enough for all primitive types?
							pIBMem[0] = pIndexData[dwIndexCount - 2];
							pIBMem[1] = pIndexData[dwIndexCount - 1];
						}
						else {
							pIBMem[0] = 0xFFFF;
						}
					}

					// render indexed vertices
#ifdef _DEBUG_TRACK_PB
					if (!g_PBTrackDisable.exists(pdwOrigPushData))
#endif
					{
						if (!g_bPBSkipPusher) {
							if (IsValidCurrentShader()) {
								CxbxDrawContext DrawContext = {};

								DrawContext.XboxPrimitiveType = XboxPrimitiveType;
								DrawContext.dwVertexCount = EmuD3DIndexCountToVertexCount(XboxPrimitiveType, dwIndexCount);
								DrawContext.hVertexShader = g_CurrentXboxVertexShaderHandle;
								DrawContext.pIndexData = pIndexData; // Used by GetVerticesInBuffer

								CxbxDrawIndexed(DrawContext);
							}
						}
					}
				}

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
				//EmuWarning("TRANSFORM CONSTANT LOAD PushBuffer Operation  Constant = %d)", *pdwPushArguments);
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
				sprintf(message, "Unhandled PushBuffer Operation : %s (0x%.04X, %d)", NV2AMethodToString(dwMethod), dwMethod, dwCount);
				LOG_TEST_CASE(message);
				break;
			}
			} // switch

			// Since some instructions use less arguments, we repeat this loop
			// for the next instruction so any leftover values are handled there :
			pdwPushArguments += HandledCount;
			dwCount -= HandledCount;
			if (bInc) {
				dwMethod += 4;
			}
		} // while (dwCount > 0)
    } // while (true)

#ifdef _DEBUG_TRACK_PB
    if (bShowPB) {
        printf("\n");
        printf("CxbxDbg> ");
        fflush(stdout);
    }
#endif

    if (g_bStepPush) {
        g_pD3DDevice->Present(0,0,0,0);
		Sleep(500);
    }
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

#ifdef _DEBUG_TRACK_PB
void DbgDumpMesh(WORD *pIndexData, DWORD dwCount)
{
	if (dwCount == 0)
		return;

	if(!XTL::IsValidCurrentShader())
        return;

    XTL::IDirect3DVertexBuffer *pActiveVB = NULL;

    XTL::D3DVERTEXBUFFER_DESC VBDesc;

    BYTE *pVBData = nullptr;
    UINT  uiStride;

    // retrieve stream data
    g_pD3DDevice->GetStreamSource(0, &pActiveVB,
#ifdef CXBX_USE_D3D9
		nullptr, // pOffsetInBytes
#endif
		&uiStride);

    char szFileName[128];
    sprintf(szFileName, "D:\\_cxbx\\mesh\\CxbxMesh-0x%.08X.x", pIndexData);
    FILE *dbgVertices = fopen(szFileName, "wt");

    // retrieve stream desc
    pActiveVB->GetDesc(&VBDesc);

    // unlock just in case
    pActiveVB->Unlock();

    // grab ptr
    pActiveVB->Lock(0, 0, (D3DLockData **)&pVBData, D3DLOCK_READONLY);

    // print out stream data
    {
        uint32 maxIndex = 0;

        WORD *pwChk = (WORD*)pIndexData;

        for(uint chk=0;chk<dwCount;chk++)
        {
            DWORD x = *pwChk++;

            if(x > maxIndex)
                maxIndex = x;
        }

        if(maxIndex > ((VBDesc.Size/uiStride) - 1))
            maxIndex = (VBDesc.Size / uiStride) - 1;

        fprintf(dbgVertices, "xof 0303txt 0032\n");
        fprintf(dbgVertices, "\n");
        fprintf(dbgVertices, "//\n");
        fprintf(dbgVertices, "//  Vertex Stream Data (0x%.08X)...\n", pActiveVB);
        fprintf(dbgVertices, "//\n");
        fprintf(dbgVertices, "//  Format : %d\n", VBDesc.Format);
        fprintf(dbgVertices, "//  Size   : %d bytes\n", VBDesc.Size);
        fprintf(dbgVertices, "//  FVF    : 0x%.08X\n", VBDesc.FVF);
        fprintf(dbgVertices, "//  iCount : %d\n", dwCount/2);
        fprintf(dbgVertices, "//\n");
        fprintf(dbgVertices, "\n");
        fprintf(dbgVertices, "Frame SCENE_ROOT {\n");
        fprintf(dbgVertices, "\n");
        fprintf(dbgVertices, "  FrameTransformMatrix {\n");
        fprintf(dbgVertices, "    1.000000,0.000000,0.000000,0.000000,\n");
        fprintf(dbgVertices, "    0.000000,1.000000,0.000000,0.000000,\n");
        fprintf(dbgVertices, "    0.000000,0.000000,1.000000,0.000000,\n");
        fprintf(dbgVertices, "    0.000000,0.000000,0.000000,1.000000;;\n");
        fprintf(dbgVertices, "  }\n");
        fprintf(dbgVertices, "\n");
        fprintf(dbgVertices, "  Frame Turok1 {\n");
        fprintf(dbgVertices, "\n");
        fprintf(dbgVertices, "    FrameTransformMatrix {\n");
        fprintf(dbgVertices, "      1.000000,0.000000,0.000000,0.000000,\n");
        fprintf(dbgVertices, "      0.000000,1.000000,0.000000,0.000000,\n");
        fprintf(dbgVertices, "      0.000000,0.000000,1.000000,0.000000,\n");
        fprintf(dbgVertices, "      0.000000,0.000000,0.000000,1.000000;;\n");
        fprintf(dbgVertices, "    }\n");
        fprintf(dbgVertices, "\n");
        fprintf(dbgVertices, "    Mesh {\n");
        fprintf(dbgVertices, "      %d;\n", maxIndex+1);

        uint max = maxIndex+1;
        for(uint v=0;v<max;v++)
        {
            fprintf(dbgVertices, "      %f;%f;%f;%s\n",
                *(FLOAT*)&pVBData[v*uiStride+0],
                *(FLOAT*)&pVBData[v*uiStride+4],
                *(FLOAT*)&pVBData[v*uiStride+8],
                (v < (max - 1)) ? "," : ";");
        }

        fprintf(dbgVertices, "      %d;\n", dwCount - 2);

        WORD *pwVal = (WORD*)pIndexData;

        max = dwCount;

        DWORD a = *pwVal++;
        DWORD b = *pwVal++;
        DWORD c = *pwVal++;

        DWORD la = a,lb = b,lc = c;

        for(uint i=2;i<max;i++)
        {
            fprintf(dbgVertices, "      3;%d,%d,%d;%s\n",
                a,b,c, (i < (max - 1)) ? "," : ";");

            a = b;
            b = c;
            c = *pwVal++;

            la = a;
            lb = b;
            lc = c;
        }

        fprintf(dbgVertices, "    }\n");
        fprintf(dbgVertices, "  }\n");
        fprintf(dbgVertices, "}\n");

        fclose(dbgVertices);
    }

    // release ptr
    pActiveVB->Unlock();
}

void XTL::DbgDumpPushBuffer( DWORD* PBData, DWORD dwSize )
{
	static int PbNumber = 0;	// Keep track of how many push buffers we've attemted to convert.
	DWORD dwVertexShader;
	char szPB[512];			

	// Prevent dumping too many of these!
	if( PbNumber > 300 )
		return;

	// Get a copy of the current vertex shader
	g_pD3DDevice->GetVertexShader( &dwVertexShader );

	/*if( g_CurrentXboxVertexShaderHandle != dwVertexShader )
	{
		printf( "g_CurrentXboxVertexShaderHandle does not match FVF from GetVertexShader!\n"
					"g_CurrentXboxVertexShaderHandle = 0x%.08X\n"
					"GetVertexShader = 0x%.08X\n" );
	}*/
	
	if( dwVertexShader > 0xFFFF )
	{
		EmuWarning( "Cannot dump pushbuffer without an FVF (programmable shaders not supported)" );
		return;
	}

	sprintf( szPB, "D:\\cxbx\\_pushbuffer\\pushbuffer%.03d.txt", PbNumber++ );

	// Create a new file for this pushbuffer's data
	HANDLE hFile = CreateFile( szPB, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );
	if( hFile == INVALID_HANDLE_VALUE )
		EmuWarning("Error creating pushbuffer file!");

	DWORD dwBytesWritten;

	// Write pushbuffer data to the file.
	// TODO: Cache the 32-bit XXHash32::hash() of each pushbuffer to ensure that the same
	// pushbuffer is not written twice within a given emulation session.
	WriteFile( hFile, &g_CurrentXboxVertexShaderHandle, sizeof( DWORD ), &dwBytesWritten, NULL );
	WriteFile( hFile, PBData, dwSize, &dwBytesWritten, NULL );

	// Close handle
	CloseHandle( hFile );
}

#endif
