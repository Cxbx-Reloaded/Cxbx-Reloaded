// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuD3D8VertexShader.cpp
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
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_LOCAL_

// prevent name collisions
namespace xboxkrnl
{
    #include <xboxkrnl/xboxkrnl.h>
};

#include "Emu.h"
#include "EmuFS.h"
#include "EmuShared.h"

// prevent name collisions
namespace XTL
{
    #include "EmuXTL.h"
};

extern XTL::LPDIRECT3DDEVICE8 g_pD3DDevice8;  // Direct3D8 Device

// recompile xbox vertex shader declaration
void XTL::EmuRecompileVSHDeclaration
(
    DWORD    *pDeclaration
)
{
    #ifdef _DEBUG_TRACE
    printf("DWORD pdwVSHDecl[] = \n");
    printf("{\n");
    #endif

    for(uint32 v=0;pDeclaration[v] != 0xFFFFFFFF;v++)
    {
        DWORD dwTokenType = (pDeclaration[v] & D3DVSD_TOKENTYPEMASK) >> D3DVSD_TOKENTYPESHIFT;

        switch(dwTokenType)
        {
            case D3DVSD_TOKEN_NOP:
            {
                // D3DVSD_NOP
                if(pDeclaration[v] != 0)
                    EmuCleanup("VSH Declaration Token NOP has parameters!");

                #ifdef _DEBUG_TRACE
                printf("    D3DVSD_NOP(),\n");
                #endif
            }
            break;

            case D3DVSD_TOKEN_STREAM:
            {
                // D3DVSD_STREAM_TESS
                if(pDeclaration[v] & D3DVSD_STREAMTESSMASK)
                {
                    #ifdef _DEBUG_TRACE
                    printf("    D3DVSD_STREAM_TESS(),\n");
                    #endif
                }
                // D3DVSD_STREAM
                else
                {
                    DWORD dwStreamNumber = (pDeclaration[v] & D3DVSD_STREAMNUMBERMASK) >> D3DVSD_STREAMNUMBERSHIFT;

                    #ifdef _DEBUG_TRACE
                    printf("    D3DVSD_STREAM(%d),\n", dwStreamNumber);
                    #endif
                }
            }
            break;

            case D3DVSD_TOKEN_STREAMDATA:
            {
                // D3DVSD_SKIPBYTES
                if(pDeclaration[v] & 0x18000000)
                {
                    DWORD dwSkipBytesCount = (pDeclaration[v] & D3DVSD_SKIPCOUNTMASK) >> D3DVSD_SKIPCOUNTSHIFT;

                    #ifdef _DEBUG_TRACE
                    printf("    D3DVSD_SKIPBYTES(%d),\n", dwSkipBytesCount);
                    #endif

                    // Todo: Patching, if necessary?
                    // ???[v] = D3DVSD_SKIP(dwSkipBytesCount / sizeof(DWORD));
                }
                // D3DVSD_SKIP
                else if(pDeclaration[v] & 0x10000000)
                {
                    DWORD dwSkipCount = (pDeclaration[v] & D3DVSD_SKIPCOUNTMASK) >> D3DVSD_SKIPCOUNTSHIFT;

                    #ifdef _DEBUG_TRACE
                    printf("    D3DVSD_SKIP(%d),\n", dwSkipCount);
                    #endif
                }
                // D3DVSD_REG
                else
                {
                    #ifdef _DEBUG_TRACE
                    printf("    D3DVSD_REG(");
                    #endif

                    DWORD dwVertexRegister = (pDeclaration[v] & D3DVSD_VERTEXREGMASK) >> D3DVSD_VERTEXREGSHIFT;
                    DWORD dwNewVertexRegister;

                    // Todo: Fixed Function
                    dwNewVertexRegister = dwVertexRegister;

                    #ifdef _DEBUG_TRACE
                    printf("%d, ", dwNewVertexRegister);
                    #endif

					DWORD dwDataType = (pDeclaration[v] >> D3DVSD_DATATYPESHIFT) & 0xFF;
					DWORD dwNewDataType = 0;

					switch(dwDataType)
					{
						case 0x12:
                            #ifdef _DEBUG_TRACE
							printf("D3DVSDT_FLOAT1");
                            #endif
							dwNewDataType = D3DVSDT_FLOAT1; 
							break;
						case 0x22:
                            #ifdef _DEBUG_TRACE
							printf("D3DVSDT_FLOAT2");
                            #endif
							dwNewDataType = D3DVSDT_FLOAT2; 
							break;
						case 0x32:
                            #ifdef _DEBUG_TRACE
							printf("D3DVSDT_FLOAT3");
                            #endif
							dwNewDataType = D3DVSDT_FLOAT3; 
							break;
						case 0x42:
                            #ifdef _DEBUG_TRACE
							printf("D3DVSDT_FLOAT4");
                            #endif
							dwNewDataType = D3DVSDT_FLOAT4; 
							break;
						case 0x40:
                            #ifdef _DEBUG_TRACE
							printf("D3DVSDT_D3DCOLOR");
                            #endif
							dwNewDataType = D3DVSDT_D3DCOLOR; 
							break;
						case 0x25:
                            #ifdef _DEBUG_TRACE
							printf("D3DVSDT_SHORT2");
                            #endif
							dwNewDataType = D3DVSDT_SHORT2; 
							break;
						case 0x45:
                            #ifdef _DEBUG_TRACE
							printf("D3DVSDT_SHORT4");
                            #endif
							dwNewDataType = D3DVSDT_SHORT4; 
							break;
						case 0x11:
                            #ifdef _DEBUG_TRACE
							printf("D3DVSDT_NORMSHORT1 /* xbox ext. */");
                            #endif
							dwNewDataType = D3DVSDT_FLOAT1; // hmm, emulation?
							break;
						case 0x21:
                            #ifdef _DEBUG_TRACE
							printf("D3DVSDT_NORMSHORT2 /* xbox ext. */");
                            #endif
							dwNewDataType = D3DVSDT_SHORT2;
							break;
						case 0x31:
                            #ifdef _DEBUG_TRACE
							printf("D3DVSDT_NORMSHORT3 /* xbox ext. */");
                            #endif
							dwNewDataType = D3DVSDT_FLOAT3;
							break;
						case 0x41:
                            #ifdef _DEBUG_TRACE
							printf("D3DVSDT_NORMSHORT4 /* xbox ext. */");
                            #endif
							dwNewDataType = D3DVSDT_SHORT4;
							break;
						case 0x16:
                            #ifdef _DEBUG_TRACE
							printf("D3DVSDT_NORMPACKED3 /* xbox ext. */");
                            #endif
							dwNewDataType = D3DVSDT_FLOAT3;//0xFF;
							break;
						case 0x15:
                            #ifdef _DEBUG_TRACE
							printf("D3DVSDT_SHORT1 /* xbox ext. */");
                            #endif
							dwNewDataType = D3DVSDT_FLOAT1;
							break;
						case 0x35:
                            #ifdef _DEBUG_TRACE
							printf("D3DVSDT_SHORT3 /* xbox ext. */");
                            #endif
							dwNewDataType = D3DVSDT_FLOAT3;
							break;
						case 0x14:
                            #ifdef _DEBUG_TRACE
							printf("D3DVSDT_PBYTE1 /* xbox ext. */");
                            #endif
							dwNewDataType = 0xFF;
							break;
						case 0x24:
                            #ifdef _DEBUG_TRACE
							printf("D3DVSDT_PBYTE2 /* xbox ext. */");
                            #endif
							dwNewDataType = 0xFF;
							break;
						case 0x34:
                            #ifdef _DEBUG_TRACE
							printf("D3DVSDT_PBYTE3 /* xbox ext. */");
                            #endif
							dwNewDataType = 0xFF;
							break;
						case 0x44:
                            #ifdef _DEBUG_TRACE
							printf("D3DVSDT_PBYTE4 /* xbox ext. */");
                            #endif
							dwNewDataType = 0xFF;
							break;
						case 0x72:
                            #ifdef _DEBUG_TRACE
							printf("D3DVSDT_FLOAT2H /* xbox ext. */");
                            #endif
							dwNewDataType = D3DVSDT_FLOAT2;
							break;
						case 0x02:
                            #ifdef _DEBUG_TRACE
							printf("D3DVSDT_NONE /* xbox ext. */");
                            #endif
							dwNewDataType = 0xFF;
							break;
						default:
                            EmuWarning("Unknown data type for D3DVSD_REG: 0x%02X\n", dwDataType);
							break;
					}

                    // Todo: Patching, if necessary?
                    // ???[v] = D3DVSD_REG(dwNewVertexRegister, dwNewDataType);

                    #ifdef _DEBUG_TRACE
                    printf("),\n");
                    #endif

                    if(dwNewDataType == 0xFF)
                        EmuWarning("Fatal Mismatch : No fitting type!");
                }
            }
            break;

            case D3DVSD_TOKEN_TESSELLATOR:
            {
				// D3DVSD_TESSUV
				if(pDeclaration[v] & 0x10000000)
				{
					DWORD dwVertexRegister = (pDeclaration[v] & D3DVSD_VERTEXREGMASK) >> D3DVSD_VERTEXREGSHIFT;
					DWORD dwNewVR = dwVertexRegister;

                    #ifdef _DEBUG_TRACE
					printf("    D3DVSD_TESSUV(");
                    #endif

                    // Todo: Fixed Function

                    #ifdef _DEBUG_TRACE
                    printf("%d),\n", dwNewVR);
                    #endif

                    // TODO: ?!?!?
					// ???[v] = D3DVSD_TESSUV(dwNewVR);
				}
				// D3DVSD_TESSNORMAL
				else
				{
					DWORD dwVertexRegisterIn  = (pDeclaration[v] & D3DVSD_VERTEXREGINMASK) >> D3DVSD_VERTEXREGINSHIFT;
					DWORD dwVertexRegisterOut = (pDeclaration[v] & D3DVSD_VERTEXREGMASK)   >> D3DVSD_VERTEXREGSHIFT;

					DWORD dwNewVRIn  = dwVertexRegisterIn;
					DWORD dwNewVROut = dwVertexRegisterOut;

                    // Todo: Fixed Function

                    #ifdef _DEBUG_TRACE
                    printf("    D3DVSD_TESSNORMAL(%d, %d),\n", dwNewVRIn, dwNewVROut);
                    #endif

                    // TODO: ?!?!?
					// ???[v] = D3DVSD_TESSNORMAL(dwNewVRIn, dwNewVROut);
				}
				break;
            }
            break;

            case D3DVSD_TOKEN_CONSTMEM:
			{
				// D3DVSD_CONST
                #ifdef _DEBUG_TRACE
				printf("D3DVSD_CONST(");
                #endif

                DWORD dwConstantAddress = ((pDeclaration[v] >> D3DVSD_CONSTADDRESSSHIFT) & 0xFF) - 96;
				DWORD dwCount			= (pDeclaration[v] & D3DVSD_CONSTCOUNTMASK) >> D3DVSD_CONSTCOUNTSHIFT;

                #ifdef _DEBUG_TRACE
				printf("%d, %d),\n", dwConstantAddress, dwCount);
                #endif

                // TODO: ?!?!?
                // ???[v] = D3DVSD_CONST(dwConstantAddress, dwCount);

				unsigned int cv=v;
				for(v;(v-cv)<dwCount*4;v++)
				{
                    #ifdef _DEBUG_TRACE
					printf("    0x%08X,\n", pDeclaration[v]);
                    #endif
				}
				break;
			}

            default:
            {
                EmuCleanup("Unknown VSH Declaration Token Type! (0x%.08X)", dwTokenType);
            }
            break;
        }
    }

    #ifdef _DEBUG_TRACE
    printf("    D3DVSD_END()\n");
    printf("}\n");
    #endif

    return;
}
