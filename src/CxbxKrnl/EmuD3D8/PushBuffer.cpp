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
extern DWORD g_CachedIndexBase;
#endif

uint32  XTL::g_dwPrimaryPBCount = 0;
uint32 *XTL::g_pPrimaryPB = 0;

bool XTL::g_bStepPush = false;
bool XTL::g_bSkipPush = false;
bool XTL::g_bBrkPush  = false;

bool g_bPBSkipPusher = false;

static void DbgDumpMesh(WORD *pIndexData, DWORD dwCount);

// Determine the size (in floats) of the texture format (indexed 0 .. 3).
// This is the reverse of the D3DFVF_TEXCOORDSIZE[0..3] macros.
int XTL::DxbxFVF_GetTextureSize(DWORD dwFVF, int aTextureIndex)
{
	// See D3DFVF_TEXCOORDSIZE1()
	switch ((dwFVF >> ((aTextureIndex * 2) + 16)) & 3) {
	case D3DFVF_TEXTUREFORMAT1: return 1; // One floating point value
	case D3DFVF_TEXTUREFORMAT2: return 2; // Two floating point values
	case D3DFVF_TEXTUREFORMAT3: return 3; // Three floating point values
	case D3DFVF_TEXTUREFORMAT4: return 4; // Four floating point values
	default:
		//assert(false || "DxbxFVF_GetTextureSize : Unhandled case");
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
			Result += DxbxFVF_GetTextureSize(dwFVF, NrTextures) * sizeof(FLOAT);
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

    PVOID pIndexData = 0;
    PVOID pVertexData = 0;

    DWORD dwVertexShader = -1;

    // cache of last 4 indices
    WORD pIBMem[4] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};

    D3DPRIMITIVETYPE    PCPrimitiveType = (D3DPRIMITIVETYPE)-1;
    X_D3DPRIMITIVETYPE  XBPrimitiveType = X_D3DPT_INVALID;

    // TODO: This technically should be enabled
    XTL::EmuUpdateDeferredStates();
	EmuUpdateActiveTextureStages();

    #ifdef _DEBUG_TRACK_PB
    bool bShowPB = false;

    g_PBTrackTotal.insert(pdwPushData);

    if(g_PBTrackShowOnce.exists(pdwPushData))
    {
        g_PBTrackShowOnce.remove(pdwPushData);

        printf("\n");
        printf("\n");
        printf("  PushBuffer@0x%.08X...\n", pdwPushData);
        printf("\n");

        bShowPB = true;
    }
    #endif

    static IDirect3DIndexBuffer *pIndexBuffer=0;
    static IDirect3DVertexBuffer *pVertexBuffer=0;

    static uint maxIBSize = 0;
	UINT8 * pPushDataEnd= (UINT8 *)pdwPushData + dwSize;
	bool bLoopPush = true;
    while(bLoopPush)
    {
		//check if loop reaches end of pushbuffer.
		if ((UINT8 *)pdwPushData >= pPushDataEnd)
		{
			EmuWarning("Last pushbuffer instruction exceeds END of Data.\n");
			break;
		}
		DWORD dwCount = (*pdwPushData >> 18);
        DWORD dwMethod = (*pdwPushData & 0x3FFFF);
		DWORD bNoInc = *pdwPushData & 0x40000000;
		
		if (bNoInc != 0)
		{
			dwCount &= ~(bNoInc>>18);
			bNoInc = 1;
		}
		
		//always increase pdwPushData after we retrive the pushbuffer Data, so it always points to the next unhandled DWORD.
		pdwPushData++;

        // Interpret GPU Instruction
        switch(dwMethod) 
        {
			case 0x000017FC: // NVPB_SetBeginEnd, 1 DWORD parameter, D3DPUSH_SET_BEGIN_END, NV097_NO_OPERATION
			
				//pdwPushData++;

				#ifdef _DEBUG_TRACK_PB
				 if(bShowPB)
				{
					printf("  NVPB_SetBeginEnd(");
				}
				#endif
				//parameter==0 means SetEnd, EndPush()
				if(*pdwPushData == 0)
				{
					#ifdef _DEBUG_TRACK_PB
					if(bShowPB)
					{
						printf("DONE)\n");
					}
					#endif
					pdwPushData++;
					bLoopPush = false;
					  // EndPush(), done with BeginPush()
				}
				else
				{
					//BeginPush(), To be used as a replacement for DrawVerticesUP, the caller needs to set the vertex format using IDirect3DDevice8::SetVertexShader before calling BeginPush. All attributes in the vertex format must be padded DWORD multiples, and the vertex attributes must be specified in the canonical FVF ordering (position followed by weight, normal, diffuse, and so on).
					#ifdef _DEBUG_TRACK_PB
					if(bShowPB)
					{
						printf("PrimitiveType := %d)\n", *pdwPushData);
					}
					#endif
					//retrieve the D3DPRIMITIVETYPE info in parameter
					XBPrimitiveType = (X_D3DPRIMITIVETYPE)*pdwPushData;
					PCPrimitiveType = EmuXB2PC_D3DPrimitiveType(XBPrimitiveType);
					pdwPushData++;
				}
				break;
			
			case  0x1818 : // NVPB_InlineVertexArray, parameter size= dwCount*DWORD, represent D3DFVF data. NV097_INLINE_ARRAY
        
				//DWORD vertex data array, 
				//To be used as a replacement for DrawPrimitivesUP, the caller needs to set the vertex format using IDirect3DDevice8::SetVertexShader before calling BeginPush. All attributes in the vertex format must be padded DWORD multiples, and the vertex attributes must be specified in the canonical FVF ordering (position followed by weight, normal, diffuse, and so on).
				pVertexData = pdwPushData;
				

				// retrieve vertex shader
				DWORD dwVertexShader; 
				dwVertexShader = g_CurrentXboxVertexShaderHandle;
				
				if (VshHandleIsVertexShader(dwVertexShader)) 
				{
					//CxbxKrnlCleanup("Non-FVF Vertex Shaders not yet supported for PushBuffer emulation!");
					EmuWarning("Non-FVF Vertex Shaders not yet supported for PushBuffer emulation!");
					//dwVertexShader = 0;
				}
				//else if(dwVertexShader == 0)
				if (dwVertexShader == 0)
				{
					EmuWarning("FVF Vertex Shader is null");
					dwVertexShader = -1;
				}

				/*
				// create cached vertex buffer only once, with maxed out size
				if(pVertexBuffer == 0)
				{
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

					if(FAILED(hRet))
					 CxbxKrnlCleanup("Unable to create vertex buffer cache for PushBuffer emulation (0x1818, dwCount : %d)", dwCount);

				}

				// copy vertex data
				{
					uint08 *pData = 0;

					HRESULT hRet = pVertexBuffer->Lock(0, dwCount*4, &pData, 0);

					if(FAILED(hRet))
						CxbxKrnlCleanup("Unable to lock vertex buffer cache for PushBuffer emulation (0x1818, dwCount : %d)", dwCount);

					memcpy(pData, pVertexData, dwCount*4);

					pVertexBuffer->Unlock();
				}
				*/

				#ifdef _DEBUG_TRACK_PB
				if(bShowPB)
				{
					printf("NVPB_InlineVertexArray(...)\n");
					printf("  dwCount : %d\n", dwCount);
					printf("  dwVertexShader : 0x%08X\n", dwVertexShader);
				}
				#endif

				// render vertices
				if(dwVertexShader != -1)
				{
					// assert(VshHandleIsVertexFVF(dwVertexShader));

					DWORD dwStride = DxbxFVFToVertexSizeInBytes(dwVertexShader, /*bIncludeTextures=*/true);
					UINT VertexCount = (dwCount*sizeof(DWORD)) / dwStride;
					UINT PrimitiveCount = EmuD3DVertex2PrimitiveCount(XBPrimitiveType, VertexCount);

					VertexPatchDesc VPDesc;

					VPDesc.dwVertexCount = VertexCount;
					VPDesc.XboxPrimitiveType = XBPrimitiveType;
					VPDesc.dwHostPrimitiveCount = PrimitiveCount;
					VPDesc.dwStartVertex = 0;
					VPDesc.pXboxVertexStreamZeroData = pVertexData;
					VPDesc.uiXboxVertexStreamZeroStride = dwStride;
					VPDesc.hVertexShader = dwVertexShader;

					VertexPatcher VertPatch;

					bool bPatched = VertPatch.Apply(&VPDesc, NULL);

					g_pD3DDevice->DrawPrimitiveUP
					(
						PCPrimitiveType,
						VPDesc.dwHostPrimitiveCount,
						VPDesc.pXboxVertexStreamZeroData,
						VPDesc.uiXboxVertexStreamZeroStride
					);

					g_dwPrimPerFrame += VPDesc.dwHostPrimitiveCount;
				}
				//move pushpuffer to the end of vertex data.
				pdwPushData += dwCount;
				break;
					//I really have no idea how this code work. it seems to be very buggy. don't really understand it. this method is for ELEMENT32, but the code is meant for 16 bit Index???
			case 0x1808 : // NVPB_FixLoop, Index Array Data, . NV097_ARRAY_ELEMENT32
				//current indexed buffer processing doesn't support xbox extention PrimitiveType.
				if ((XBPrimitiveType == X_D3DPT_LINELOOP)|| (XBPrimitiveType == X_D3DPT_QUADLIST) || (XBPrimitiveType == X_D3DPT_QUADSTRIP) || (XBPrimitiveType == X_D3DPT_POLYGON))
				{
					EmuWarning("PushBuffer Processing with unsupported PrimitiveType! (%d)", (DWORD)XBPrimitiveType);
					//CxbxKrnlCleanup("XTL_EmuD3DDevice_DrawIndexedVertices : X_D3DPT_LINELOOP not unsupported yet!");
					// TODO : Close line-loops using a final single line, drawn from the end to the start vertex
					pdwPushData += dwCount;
					break;
				}
				//this code is buggy, close the emu here and warn users to issue this as a test case.
				CxbxKrnlCleanup("push-buffer processing doesn't support method 0x1808 correctly, report to developers!");

				#ifdef _DEBUG_TRACK_PB
				if(bShowPB)
				{
					printf("  NVPB_FixLoop(%d)\n", dwCount);
					printf("\n");
					printf("  Index Array Data...\n");

					WORD *pwVal;
					pwVal = (WORD*)(pdwPushData);

					for(uint s=0;s<dwCount;s++)
					{
						if(s%8 == 0) printf("\n  ");

						printf("  %.04X", *pwVal++);
					}

					printf("\n");
					printf("\n");
				}
				#endif
				//the pdwPushData is increased pointing to the first unhandled DWORD already, no need to add offset
				WORD *pwVal;
				pwVal = (WORD*)pdwPushData; 

				//!!!!!!don't understand this loop, the pIBMem[] is only 4 WORD, how can we copy the data with known size into it?******************************
				for(uint mi=0;mi<dwCount;mi++)
				{
					pIBMem[mi+2] = pwVal[mi];
				}

				// perform rendering
				if(pIBMem[0] != 0xFFFF)
				{
					HRESULT hRet;

					// TODO: depreciate maxIBSize after N milliseconds..then N milliseconds later drop down to new highest
					if((dwCount*2 + 2*2) > maxIBSize)
					{
						if(pIndexBuffer != 0)
						{
							pIndexBuffer->Release();
						}

						// https://msdn.microsoft.com/en-us/library/windows/desktop/bb147168(v=vs.85).aspx
						// "Managing Resources (Direct3D 9)"
						// suggests "for resources which change with high frequency" [...]
						// "D3DPOOL_DEFAULT along with D3DUSAGE_DYNAMIC should be used."
						const D3DPOOL D3DPool = D3DPOOL_DEFAULT; // Was D3DPOOL_MANAGED
						// https://msdn.microsoft.com/en-us/library/windows/desktop/bb172625(v=vs.85).aspx
						// "Buffers created with D3DPOOL_DEFAULT that do not specify D3DUSAGE_WRITEONLY may suffer a severe performance penalty."
						const DWORD D3DUsage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
						hRet = g_pD3DDevice->CreateIndexBuffer(dwCount*2 + 2*2, D3DUsage, D3DFMT_INDEX16, D3DPool, &pIndexBuffer

#ifdef CXBX_USE_D3D9
							, nullptr
#endif
						);

						maxIBSize = dwCount*2 + 2*2;
					}
					else
					{
						hRet = D3D_OK;
					}

					if(FAILED(hRet))
						CxbxKrnlCleanup("Unable to create index buffer for PushBuffer emulation (0x1808, dwCount : %d)", dwCount);

					// copy index data, !!! for unsupported Primitive Types, the indexed buffer shall be patched as the vertex buffer as well.
					{
						WORD *pData=0;

						pIndexBuffer->Lock(0, dwCount*2 + 2*2, (UCHAR**)&pData, NULL);

						memcpy(pData, pIBMem, dwCount*2 + 2*2);

						pIndexBuffer->Unlock();
					}

					// render indexed vertices
					{
						UINT PrimitiveCount = EmuD3DVertex2PrimitiveCount(XBPrimitiveType, dwCount + 2);
						VertexPatchDesc VPDesc;

						VPDesc.dwVertexCount = dwCount;
						VPDesc.XboxPrimitiveType = XBPrimitiveType;
						VPDesc.dwHostPrimitiveCount = PrimitiveCount;
						VPDesc.dwStartVertex = 0;
						VPDesc.pXboxVertexStreamZeroData = 0;
						VPDesc.uiXboxVertexStreamZeroStride = 0;
						// TODO: Set the current shader and let the patcher handle it..
						VPDesc.hVertexShader = g_CurrentXboxVertexShaderHandle;

						VertexPatcher VertPatch;

						bool bPatched = VertPatch.Apply(&VPDesc, NULL);

#ifdef CXBX_USE_D3D9
						g_pD3DDevice->SetIndices(pIndexBuffer);
#else
						g_pD3DDevice->SetIndices(pIndexBuffer, 0);
#endif

						#ifdef _DEBUG_TRACK_PB
						if(!g_PBTrackDisable.exists(pdwOrigPushData))
						{
						#endif

						if(!g_bPBSkipPusher)
						{
							if(IsValidCurrentShader())
							{
								g_pD3DDevice->DrawIndexedPrimitive
								(
									PCPrimitiveType, 
#ifdef CXBX_USE_D3D9
									g_CachedIndexBase,
#endif
									0,
									8*1024*1024, // dwCount*2
									0, 
									PrimitiveCount
								);

								g_dwPrimPerFrame += PrimitiveCount;
							}
						}

						#ifdef _DEBUG_TRACK_PB
						}
					 #endif

#ifdef CXBX_USE_D3D9
						g_pD3DDevice->SetIndices(nullptr);
#else
						g_pD3DDevice->SetIndices(0, 0);
#endif
					}
				}
				// the dwCount should be the DWORD data counts.
				pdwPushData += dwCount;
				break;

			case  0x1800 : // NVPB_InlineIndexArray,   NV097_ARRAY_ELEMENT16
				//current indexed buffer processing doesn't support xbox extention PrimitiveType.
				if ((XBPrimitiveType == X_D3DPT_LINELOOP) || (XBPrimitiveType == X_D3DPT_QUADLIST) || (XBPrimitiveType == X_D3DPT_QUADSTRIP) || (XBPrimitiveType == X_D3DPT_POLYGON))
				{
					EmuWarning("PushBuffer Processing with unsupported PrimitiveType! (%d)", (DWORD)XBPrimitiveType);
					//CxbxKrnlCleanup("XTL_EmuD3DDevice_DrawIndexedVertices : X_D3DPT_LINELOOP not unsupported yet!");
					// TODO : Close line-loops using a final single line, drawn from the end to the start vertex
					pdwPushData += dwCount;
					break;
				}
				//point pIndexData to the first parameter.
				pIndexData = pdwPushData;
				DWORD dwWordCount;
				DWORD dwByteCount;
				//multiply dwCount from DWORD count to WORD count
				dwWordCount=dwCount *2;
				//if no increment is not set, then there is one WORD less then the total dwCount*2 WORD data.
				//this definition is purely my guess, need confirmation.
				if (bNoInc ==0 )
				{
					dwCount -= 1;
				}

				dwByteCount = dwWordCount * 2;

				#ifdef _DEBUG_TRACK_PB
				if(bShowPB)
				{
					printf("  NVPB_InlineIndexArray(0x%.08X, %d)...\n", pIndexData, dwCount);
					printf("\n");
					printf("  Index Array Data...\n");

					WORD *pwVal;
					pwVal=(WORD*)pIndexData;

					for(uint s=0;s<dwCount;s++)
					{
						if(s%8 == 0) printf("\n  ");

						printf("  %.04X", *pwVal++);
					}

					printf("\n");

					XTL::IDirect3DVertexBuffer *pActiveVB = NULL;

					D3DVERTEXBUFFER_DESC VBDesc;

					BYTE *pVBData = 0;
					UINT  uiStride;

					// retrieve stream data
					g_pD3DDevice->GetStreamSource(0, &pActiveVB,
	#ifdef CXBX_USE_D3D9
						nullptr, // pOffsetInBytes
	#endif
						&uiStride);

					// retrieve stream desc
					pActiveVB->GetDesc(&VBDesc);

					// unlock just in case
					pActiveVB->Unlock();

					// grab ptr
					pActiveVB->Lock(0, 0, &pVBData, D3DLOCK_READONLY);

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

					// release ptr
					pActiveVB->Unlock();

					DbgDumpMesh((WORD*)pIndexData, dwCount);
				}
				#endif

				// perform rendering
				{
					HRESULT hRet;

					// TODO: depreciate maxIBSize after N milliseconds..then N milliseconds later drop down to new highest
					if(dwByteCount > maxIBSize)
					{
						if(pIndexBuffer != 0)
						{
							pIndexBuffer->Release();
						}

						// https://msdn.microsoft.com/en-us/library/windows/desktop/bb147168(v=vs.85).aspx
						// "Managing Resources (Direct3D 9)"
						// suggests "for resources which change with high frequency" [...]
						// "D3DPOOL_DEFAULT along with D3DUSAGE_DYNAMIC should be used."
						const D3DPOOL D3DPool = D3DPOOL_DEFAULT; // Was D3DPOOL_MANAGED
						// https://msdn.microsoft.com/en-us/library/windows/desktop/bb172625(v=vs.85).aspx
						// "Buffers created with D3DPOOL_DEFAULT that do not specify D3DUSAGE_WRITEONLY may suffer a severe performance penalty."
						const DWORD D3DUsage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
						hRet = g_pD3DDevice->CreateIndexBuffer(dwByteCount*2, D3DUsage, D3DFMT_INDEX16, D3DPool, &pIndexBuffer
	#ifdef CXBX_USE_D3D9
							, nullptr
	#endif
						);

						maxIBSize = dwByteCount;
					}
					else
					{
						hRet = D3D_OK;
					}

					if(FAILED(hRet))
						CxbxKrnlCleanup("Unable to create index buffer for PushBuffer emulation (0x1800, dwCount : %d)", dwCount);

					// copy index data !!! for unsupported Primitive Types, the indexed buffer shall be patched as the vertex buffer as well.
					{
						WORD *pData=0;

						pIndexBuffer->Lock(0, dwByteCount, (UCHAR**)&pData, NULL);
						// something wrong, the pIndexData was never initialized. perhaps it shall be point to the first unhandled pdwPushData DWORD?
						memcpy(pData, pIndexData, dwByteCount);

						// remember last 2 indices
						if(dwWordCount >= 2)
						{
							pIBMem[0] = pData[dwWordCount - 2];
							pIBMem[1] = pData[dwWordCount - 1];
						}
						else
						{
							pIBMem[0] = 0xFFFF;
						}

						pIndexBuffer->Unlock();
					}

					// render indexed vertices
					{
						UINT PrimitiveCount = EmuD3DVertex2PrimitiveCount(XBPrimitiveType, dwWordCount);
						VertexPatchDesc VPDesc;

						VPDesc.dwVertexCount = dwWordCount;
						VPDesc.XboxPrimitiveType = XBPrimitiveType;
						VPDesc.dwHostPrimitiveCount = PrimitiveCount;
						VPDesc.dwStartVertex = 0;
						VPDesc.pXboxVertexStreamZeroData = 0;
						VPDesc.uiXboxVertexStreamZeroStride = 0;
						// TODO: Set the current shader and let the patcher handle it..
						VPDesc.hVertexShader = g_CurrentXboxVertexShaderHandle;

						VertexPatcher VertPatch;

						bool bPatched = VertPatch.Apply(&VPDesc, NULL);

	#ifdef CXBX_USE_D3D9
						g_pD3DDevice->SetIndices(pIndexBuffer);
	#else
						g_pD3DDevice->SetIndices(pIndexBuffer, 0);
	#endif

						#ifdef _DEBUG_TRACK_PB
						if(!g_PBTrackDisable.exists(pdwOrigPushData))
						{
						#endif

						if(!g_bPBSkipPusher && IsValidCurrentShader())
						{
							g_pD3DDevice->DrawIndexedPrimitive
							(
								PCPrimitiveType, 
	#ifdef CXBX_USE_D3D9
								g_CachedIndexBase,
	#endif
								0, 
								/*dwCount*2*/8*1024*1024, 
								0, 
								PrimitiveCount
							);

							g_dwPrimPerFrame += PrimitiveCount;
						}

						#ifdef _DEBUG_TRACK_PB
						}
						#endif

#ifdef CXBX_USE_D3D9
						g_pD3DDevice->SetIndices(nullptr);
#else
						g_pD3DDevice->SetIndices(0, 0);
#endif
					}
				}
				//point pdwPushData to the next unprocessed DWORD
				pdwPushData += dwCount;
				break;

			case 0x00000100 :  //No Operation, followed parameters are no use. this operation triggers DPC which is not implemented in HLE
        
				//EmuWarning("NOP PushBuffer Operation (0x%.04X, %d)", dwMethod, dwCount);
				pdwPushData+=dwCount;
				break;

			case 0x00001ea4 : //D3DPUSH_SET_TRANSFORM_CONSTANT_LOAD  // Add 96 to constant index parameter, one parameter=CONSTANT + 96
		
				//retrive transform constant index and add 96 to it.
				//EmuWarning("TRANSFORM CONSTANT LOAD PushBuffer Operation (0x%.04X, %d)", dwMethod, dwCount);
				//EmuWarning("TRANSFORM CONSTANT LOAD PushBuffer Operation  Constant = %d)", *pdwPushData);
				pdwPushData++;
				break;

			case 0x00000b80 : //D3DPUSH_SET_TRANSFORM_CONSTANT      // Can't use NOINCREMENT_FLAG, parameters is constant matrix, 4X4 matrix hasa 16 DWRDS, maximum of 32 DWORD writes
			
				//load constant matrix to empty slot, then break;
				pdwPushData += dwCount;
				bLoopPush=false;
				break;
			
			default : // default case, handling any other unknown methods.
			
				EmuWarning("Unknown PushBuffer Operation (0x%.04X, %d)", dwMethod, dwCount);
				pdwPushData += dwCount;
				//shall we break the loop for pushbuffer processing here?
		}
        //pdwPushData++;
    }

    #ifdef _DEBUG_TRACK_PB
    if(bShowPB)
    {
        printf("\n");
        printf("CxbxDbg> ");
        fflush(stdout);
    }
    #endif

    if(g_bStepPush)
    {
        g_pD3DDevice->Present(0,0,0,0);
		Sleep(500);
    }
}

#ifdef _DEBUG_TRACK_PB
void DbgDumpMesh(WORD *pIndexData, DWORD dwCount)
{
    if(!XTL::IsValidCurrentShader() || (dwCount == 0))
        return;

    XTL::IDirect3DVertexBuffer *pActiveVB = NULL;

    XTL::D3DVERTEXBUFFER_DESC VBDesc;

    BYTE *pVBData = 0;
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
    pActiveVB->Lock(0, 0, &pVBData, D3DLOCK_READONLY);

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

		WORD *pwVal;
		pwVal = (WORD*)pIndexData;

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

	/*if( g_CurrentVertexShader != dwVertexShader )
	{
		printf( "g_CurrentVertexShader does not match FVF from GetVertexShader!\n"
					"g_CurrentVertexShader = 0x%.08X\n"
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