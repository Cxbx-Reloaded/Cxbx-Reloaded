// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuD3D8->VertexShader.cpp
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
// *  (c) 2002-2004 Aaron Robinson <caustik@caustik.com>
// *                Kingofc <kingofc@freenet.de>
// *
// *  All rights reserved
// *
// ******************************************************************
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

#include "Emu.h"

// prevent name collisions
namespace XTL
{
    #include "EmuXTL.h"
};

extern XTL::LPDIRECT3DDEVICE8 g_pD3DDevice8;  // Direct3D8 Device

#include <string>

// recompile xbox vertex shader declaration
void XTL::EmuRecompileVSHDeclaration
(
    DWORD  *pDeclaration,
    DWORD   Handle
)
{
    X_D3DVertexShader *pD3DVertexShader = (X_D3DVertexShader*)Handle;

    // Note: This fixed pipeline generation technique is a hack!
    pD3DVertexShader->Handle = D3DFVF_XYZ;

    DbgPrintf("DWORD pdwVSHDecl[] = \n");
    DbgPrintf("{\n");

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

                DbgPrintf("    D3DVSD_NOP(),\n");
            }
            break;

            case D3DVSD_TOKEN_STREAM:
            {
                // D3DVSD_STREAM_TESS
                if(pDeclaration[v] & D3DVSD_STREAMTESSMASK)
                {
                    DbgPrintf("    D3DVSD_STREAM_TESS(),\n");
                }
                // D3DVSD_STREAM
                else
                {
                    DWORD dwStreamNumber = (pDeclaration[v] & D3DVSD_STREAMNUMBERMASK) >> D3DVSD_STREAMNUMBERSHIFT;

                    DbgPrintf("    D3DVSD_STREAM(%d),\n", dwStreamNumber);
                }
            }
            break;

            case D3DVSD_TOKEN_STREAMDATA:
            {
                // D3DVSD_SKIPBYTES
                if(pDeclaration[v] & 0x18000000)
                {
                    DWORD dwSkipBytesCount = (pDeclaration[v] & D3DVSD_SKIPCOUNTMASK) >> D3DVSD_SKIPCOUNTSHIFT;

                    DbgPrintf("    D3DVSD_SKIPBYTES(%d),\n", dwSkipBytesCount);

                    // Todo: Patching, if necessary?
                    // ???[v] = D3DVSD_SKIP(dwSkipBytesCount / sizeof(DWORD));
                }
                // D3DVSD_SKIP
                else if(pDeclaration[v] & 0x10000000)
                {
                    DWORD dwSkipCount = (pDeclaration[v] & D3DVSD_SKIPCOUNTMASK) >> D3DVSD_SKIPCOUNTSHIFT;

                    DbgPrintf("    D3DVSD_SKIP(%d),\n", dwSkipCount);
                }
                // D3DVSD_REG
                else
                {
                    DbgPrintf("    D3DVSD_REG(");

                    DWORD dwVertexRegister = (pDeclaration[v] & D3DVSD_VERTEXREGMASK) >> D3DVSD_VERTEXREGSHIFT;
                    DWORD dwNewVertexRegister;

                    // Todo: Fixed Function
                    dwNewVertexRegister = dwVertexRegister;

                    DbgPrintf("%d, ", dwNewVertexRegister);

                    DWORD dwDataType = (pDeclaration[v] >> D3DVSD_DATATYPESHIFT) & 0xFF;
                    DWORD dwNewDataType = 0;

                    switch(dwDataType)
                    {
                        case 0x12:
                            DbgPrintf("D3DVSDT_FLOAT1");
                            dwNewDataType = D3DVSDT_FLOAT1;
                            pD3DVertexShader->Handle |= D3DFVF_XYZ;
                            break;
                        case 0x22:
                            DbgPrintf("D3DVSDT_FLOAT2");
                            dwNewDataType = D3DVSDT_FLOAT2;
                            break;
                        case 0x32:
                            DbgPrintf("D3DVSDT_FLOAT3");
                            dwNewDataType = D3DVSDT_FLOAT3;
                            pD3DVertexShader->Handle |= D3DFVF_NORMAL;
                            break;
                        case 0x42:
                            DbgPrintf("D3DVSDT_FLOAT4");
                            dwNewDataType = D3DVSDT_FLOAT4;
                            pD3DVertexShader->Handle |= D3DFVF_DIFFUSE;
                            break;
                        case 0x40:
                            DbgPrintf("D3DVSDT_D3DCOLOR");
                            dwNewDataType = D3DVSDT_D3DCOLOR;
                            pD3DVertexShader->Handle |= D3DFVF_SPECULAR;
                            break;
                        case 0x25:
                            DbgPrintf("D3DVSDT_SHORT2");
                            dwNewDataType = D3DVSDT_SHORT2;
                            break;
                        case 0x45:
                            DbgPrintf("D3DVSDT_SHORT4");
                            dwNewDataType = D3DVSDT_SHORT4;
                            break;
                        case 0x11:
                            DbgPrintf("D3DVSDT_NORMSHORT1 /* xbox ext. */");
                            dwNewDataType = D3DVSDT_FLOAT1; // hmm, emulation?
                            break;
                        case 0x21:
                            DbgPrintf("D3DVSDT_NORMSHORT2 /* xbox ext. */");
                            dwNewDataType = D3DVSDT_SHORT2;
                            break;
                        case 0x31:
                            DbgPrintf("D3DVSDT_NORMSHORT3 /* xbox ext. */");
                            dwNewDataType = D3DVSDT_FLOAT3;
                            pD3DVertexShader->Handle |= D3DFVF_NORMAL;
                            break;
                        case 0x41:
                            DbgPrintf("D3DVSDT_NORMSHORT4 /* xbox ext. */");
                            dwNewDataType = D3DVSDT_SHORT4;
                            break;
                        case 0x16:
                            DbgPrintf("D3DVSDT_NORMPACKED3 /* xbox ext. */");
                            dwNewDataType = D3DVSDT_FLOAT3;//0xFF;
                            break;
                        case 0x15:
                            DbgPrintf("D3DVSDT_SHORT1 /* xbox ext. */");
                            dwNewDataType = D3DVSDT_FLOAT1;
                            break;
                        case 0x35:
                            DbgPrintf("D3DVSDT_SHORT3 /* xbox ext. */");
                            dwNewDataType = D3DVSDT_FLOAT3;
                            break;
                        case 0x14:
                            DbgPrintf("D3DVSDT_PBYTE1 /* xbox ext. */");
                            dwNewDataType = 0xFF;
                            break;
                        case 0x24:
                            DbgPrintf("D3DVSDT_PBYTE2 /* xbox ext. */");
                            dwNewDataType = 0xFF;
                            break;
                        case 0x34:
                            DbgPrintf("D3DVSDT_PBYTE3 /* xbox ext. */");
                            dwNewDataType = 0xFF;
                            break;
                        case 0x44:
                            DbgPrintf("D3DVSDT_PBYTE4 /* xbox ext. */");
                            dwNewDataType = 0xFF;
                            break;
                        case 0x72:
                            DbgPrintf("D3DVSDT_FLOAT2H /* xbox ext. */");
                            dwNewDataType = D3DVSDT_FLOAT2;
                            break;
                        case 0x02:
                            DbgPrintf("D3DVSDT_NONE /* xbox ext. */");
                            dwNewDataType = 0xFF;
                            break;
                        default:
                            EmuWarning("Unknown data type for D3DVSD_REG: 0x%02X\n", dwDataType);
                            break;
                    }

                    // Todo: Patching, if necessary?
                    // ???[v] = D3DVSD_REG(dwNewVertexRegister, dwNewDataType);

                    DbgPrintf("),\n");

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

                    DbgPrintf("    D3DVSD_TESSUV(");

                    // Todo: Fixed Function

                    DbgPrintf("%d),\n", dwNewVR);

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

                    DbgPrintf("    D3DVSD_TESSNORMAL(%d, %d),\n", dwNewVRIn, dwNewVROut);

                    // TODO: ?!?!?
                    // ???[v] = D3DVSD_TESSNORMAL(dwNewVRIn, dwNewVROut);
                }
                break;
            }
            break;

            case D3DVSD_TOKEN_CONSTMEM:
            {
                // D3DVSD_CONST
                DbgPrintf("D3DVSD_CONST(");

                DWORD dwConstantAddress = ((pDeclaration[v] >> D3DVSD_CONSTADDRESSSHIFT) & 0xFF) - 96;
                DWORD dwCount           = (pDeclaration[v] & D3DVSD_CONSTCOUNTMASK) >> D3DVSD_CONSTCOUNTSHIFT;

                DbgPrintf("%d, %d),\n", dwConstantAddress, dwCount);

                // TODO: ?!?!?
                // ???[v] = D3DVSD_CONST(dwConstantAddress, dwCount);

                unsigned int cv=v;
                for(v;(v-cv)<dwCount*4;v++)
                    printf("    0x%08X,\n", pDeclaration[v]);

                break;
            }

            default:
            {
                EmuCleanup("Unknown VSH Declaration Token Type! (0x%.08X)", dwTokenType);
            }
            break;
        }
    }

    DbgPrintf("    D3DVSD_END()\n");
    DbgPrintf("}\n");

    return;
}

// recompile xbox vertex shader function
extern void XTL::EmuRecompileVSHFunction
(
    DWORD  *pFunction,
    DWORD **pRecompiled
)
{
    _asm int 3

    // output d3d8 shader assembly code
    std::string asm_shad_str;   // full shader assembly code (built inline)

    // ** <instruction decoding>  **
    int iR[4], iC[2], iSW[2], iV;

    // maximum acceptable paramters for current instruction
    DWORD dwMaxParamsMAC, dwMaxParamsILU, dwMaxParams;

    // quick string lookup for components XYZW
    char *pszComponents[4] = {"x", "y", "z", "w"};

    // XYZW component flags (output, r# temp registers)
    BOOL bOComponent[4], bRComponent[4];

    // output register flag
    BOOL bOutRegBit;

    // xyzw negation flags
    BOOL bNegateParams[4];
    // ** </instruction decoding> **

    // version information
    BOOL bVS_XVS = FALSE, bXVSVersion = FALSE;

    // nv2a vertex shader function header
    NV2A_HEADER *pNV2AHeader = (NV2A_HEADER*)pFunction;

    DbgPrintf("Recompiling Vertex Shader...(instructions: %d, unknown: %d)\n", pNV2AHeader->NumInst, pNV2AHeader->Unknown);
    DbgPrintf("\n");

    if(pNV2AHeader->Type != NV2A_MICROCODE_DEF_TYPE)
        EmuCleanup("Invalid NV2A Header Definition Type : 0x%.08X", pNV2AHeader->Type);

    // skip cursor past nv2a header
    pFunction += sizeof(NV2A_HEADER)/sizeof(DWORD);

    switch(pNV2AHeader->Version)
    {
        case NV2A_MICROCODE_DEF_VS_XVS:
            bVS_XVS = TRUE;
            DbgPrintf("vs.1.1\n");
            DbgPrintf("\n");
            break;

        case NV2A_MICROCODE_DEF_XVSS:
            DbgPrintf("xvss.1.1\n");
            DbgPrintf("\n");
            break;

        case NV2A_MICROCODE_DEF_XVSW:
            DbgPrintf("xvsw.1.1\n");
            DbgPrintf("\n");
            break;

        default:
            EmuCleanup("Unknown NV2A Vertex Shader Version (0x%.08X)", pNV2AHeader->Version);
            break;
    }

    // we'll always recompile to vs 1.1
    asm_shad_str += "vs.1.1\n";

    // perform an inline recompile of the nv2a vertex shader code
    do
    {
        // instruction decoding
        BOOL bOComponents = FALSE, bRComponents = FALSE;
        BOOL bRPairedCMps = FALSE, bCPairedCMps = FALSE;
        BOOL bPaired = FALSE;

        // MAC/ILU for current instruction
        UINT uiMAC = (pFunction[1] >> 21) & 0x0F; // seems to be ok, right shift of 21 and a 4 bit mask
        UINT uiILU = (pFunction[1] >> 25) & 0x0F; // 21+4=25 -> right shift of 25 and a bit mask of 4 bits

        // current instruction parameter
        UINT uiCurMAC = 0, uiCurILU = 0;

        // parameter caching
        UINT uiParams[4];

        // instruction generation
        char *mac_inst_str=0;       // MAC instruction string
        char *ilu_inst_str=0;       // ILU instruction string
        char  mac_parm_str[32]="";  // MAC parameter string
        char  ilu_parm_str[32]="";  // ILU parameter srting

        switch(uiMAC)
        {
            case 0x00:
                break;

            case 0x01:
                DbgPrintf("MAC instruction : mov\n");
                mac_inst_str = "mov";
                dwMaxParamsMAC = 2;
                break;

            case 0x02:
                DbgPrintf("MAC instruction : mul\n");
                mac_inst_str = "mul";
                dwMaxParamsMAC = 3;
                break;

            case 0x03:
                DbgPrintf("MAC instruction : add\n");
                mac_inst_str = "add";
                dwMaxParamsMAC = 3;
                break;

            case 0x04:
                DbgPrintf("MAC instruction : mad\n");
                mac_inst_str = "mad";
                dwMaxParamsMAC = 4;
                break;

            case 0x05:
                DbgPrintf("MAC instruction : dp3\n");
                mac_inst_str = "dp3";
                dwMaxParamsMAC = 3;
                break;

            case 0x06:
                DbgPrintf("MAC instruction : dph\n");
                dwMaxParamsMAC = 3;
                bXVSVersion = TRUE;
                EmuCleanup("NV2A 'dph' instruction is not yet supported!");
                break;

            case 0x07:
                DbgPrintf("MAC instruction : dp4\n");
                mac_inst_str = "dp4";
                dwMaxParamsMAC = 3;
                break;

            case 0x08:
                DbgPrintf("MAC instruction : dst\n");
                mac_inst_str = "dst";
                dwMaxParamsMAC = 3;
                break;

            case 0x09:
                DbgPrintf("MAC instruction : min\n");
                mac_inst_str = "min";
                dwMaxParamsMAC = 3;
                break;

            case 0x0A:
                DbgPrintf("MAC instruction : max\n");
                mac_inst_str = "max";
                dwMaxParamsMAC = 3;
                break;

            case 0x0B:
                DbgPrintf("MAC instruction : slt\n");
                mac_inst_str = "max";
                dwMaxParamsMAC = 3;
                break;

            case 0x0C:
                DbgPrintf("MAC instruction : sge\n");
                mac_inst_str = "sge";
                dwMaxParamsMAC = 3;
                break;

            case 0x0D:
                DbgPrintf("MAC instruction : mov a0.x\n"); /* arl */
                mac_inst_str = "mov";
                strcpy(mac_parm_str, "a0.x");
                // Todo: Most likely increment pointer and set params to 0
                uiCurMAC = 1;
                dwMaxParamsMAC = 2;
                break;

            default:
                EmuCleanup("Unknown Vertex Shader MAC : 0x%.08X", uiMAC);
        }

        switch(uiILU)
        {
            case 0x00:
                break;

            case 0x01:
                DbgPrintf("ILU instruction : mov\n"); /* imv */
                ilu_inst_str = "mov";
                dwMaxParamsILU = 2;
                break;

            case 0x02:
                DbgPrintf("ILU instruction : rcp\n");
                ilu_inst_str = "mul";
                dwMaxParamsILU = 2;
                break;

            case 0x03:
                DbgPrintf("ILU instruction : rcc\n");
                // HACK: using non-clamping variant
                ilu_inst_str = "rcp";
                dwMaxParamsILU = 2;
                EmuWarning("NV2A 'rcc' instruction mapped to rcp (no clamping)!");
                break;

            case 0x04:
                DbgPrintf("ILU instruction : rsq\n");
                ilu_inst_str = "rsq";
                dwMaxParamsILU = 2;
                break;

            case 0x05:
                DbgPrintf("ILU instruction : exp\n");
                ilu_inst_str = "exp";
                dwMaxParamsILU = 2;
                break;

            case 0x06:
                DbgPrintf("ILU instruction : log\n");
                ilu_inst_str = "log";
                dwMaxParamsILU = 2;
                break;

            case 0x07:
                DbgPrintf("ILU instruction : lit\n");
                ilu_inst_str = "lit";
                dwMaxParamsILU = 2;
                break;

            default:
                EmuCleanup("Unknown Vertex Shader ILU : 0x%.08X", uiILU);
        }

        if(uiILU != 0)
            dwMaxParams = dwMaxParamsILU;
        else if(uiMAC)
            dwMaxParams = dwMaxParamsMAC;
        else
            dwMaxParams = 0;

        bPaired = (uiMAC != 0) && (uiILU != 0);

        if( (uiMAC == 0) && (uiILU == 0) )
        {
            asm_shad_str += "nop\n";
        }
        else
        {   
            // input registers
            iV = (pFunction[1] >> 9) & 0x0F;

            // 2 constant register addresses
            iC[0] = (pFunction[1] >> 13) & 0x1F;
            iC[1] = (pFunction[3] >>  3) & 0x1F;

            // 4 temporary register addresses
            iR[0] =  (pFunction[3] >> 20) & 0x0F;
            iR[1] =  (pFunction[2] >> 28) & 0x0F;
            iR[2] =  (pFunction[2] >> 13) & 0x0F;
            iR[3] = ((pFunction[2] & 0x03) <<2) | ( (pFunction[3] >> 30) & 0x03);

            // 2 switches for 2 c registers
            iSW[0] = (pFunction[1] >> 18) & 0x07; // 3 bits for 8 cases??
            iSW[1] = (pFunction[3] >>  8) & 0x07;

            // check output register bit
            bOutRegBit = (pFunction[3] >> 11) & 0x01;

            for(int i=0;i<2;i++)
            {
                if(iSW[i] != 7)
                {
                    if( (iSW[i] > 5) )
                        EmuCleanup("Unknown Vertex Shader Constant Type (0x%.08X)", iSW[i]);

                    iC[i] -= 96 - iSW[i]*32;
                }
            }

            DbgPrintf("v%d\n", iV);
		    DbgPrintf("c%d\n", iC[0]);
		    DbgPrintf("c%d\n", iC[1]);
		    DbgPrintf("r%d (nr 1)\n", iR[0]);
		    DbgPrintf("r%d (nr 2)\n", iR[1]);
		    DbgPrintf("r%d (nr 3)\n", iR[2]);
		    DbgPrintf("r%d (nr 4)\n", iR[3]);

            // components of normal variables r as output
            bRPairedCMps = ((pFunction[3] >> 16) & 0x0F) ? TRUE : FALSE;

            // components of c as output
            bCPairedCMps = ((pFunction[3] >> 24) & 0x0F) ? TRUE : FALSE;

            // decode output/temp register XYZW component flags
            {
                int p = bPaired ? 19 : 27;

                for(int i=0;i<4;i++)
                {
                    bOComponent[i] = (pFunction[3] >> (15-i)) & 0x01;
                    bRComponent[i] = (pFunction[3] >> ( p-i)) & 0x01;

                    bOComponents |= bOComponent[i];
                    bRComponents |= bRComponent[i];
                }
            }
            //*** end

            // the only r register the ILU operation can write to is r1
            if(bPaired && bRComponents)
            { 
                DbgPrintf("Paired Register : r1");

                strcat(mac_parm_str, "r1");

                if(!(bRComponent[0] && bRComponent[1] && bRComponent[2] && bRComponent[3]))
                {
                    strcat(mac_parm_str, ".");
                    DbgPrintf(".");

                    for(int i=0;i<4;i++)
                    {
                        if(bRComponent[i])
                        {
                            strcat(mac_parm_str, pszComponents[i]);
                            DbgPrintf(pszComponents[i]);
                        }
                    }
                }

                DbgPrintf("\n");

                uiCurMAC++;
            }

            if(bRComponents || (!bOutRegBit))
            {
                // Todo: same instruction, different output?

                if(!bOutRegBit)
                {
                    DbgPrintf("Register : c%d", iC[1]);

                    char szBuff[8];

                    if(uiCurMAC > 0)
                        sprintf(szBuff, ", c%d", iC[1]);
                    else
                        sprintf(szBuff, "c%d", iC[1]);

                    strcat(mac_parm_str, szBuff);

                    if(bOComponents && !(bOComponent[0] && bOComponent[1] && bOComponent[2] && bOComponent[3]))
                    {
                        strcat(mac_parm_str, ".");
                        DbgPrintf(".");

                        for(int i=0;i<4;i++)
                        {
                            if(bOComponent[i])
                            {
                                strcat(mac_parm_str, pszComponents[i]);
                                DbgPrintf(pszComponents[i]);
                            }
                        }
                    }

                    DbgPrintf("\n");

                    uiCurMAC++;
                }
                else 
                {
                    DbgPrintf("Register : r%d", iR[0]);

                    char szBuff[8];

                    if(uiCurMAC > 0)
                        sprintf(szBuff, ", r%d", iR[0]);
                    else
                        sprintf(szBuff, "r%d", iR[0]);

                    strcat(mac_parm_str, szBuff);

                    if(bRComponents && !(bRComponent[0] && bRComponent[1] && bRComponent[2] && bRComponent[3]))
                    {
                        strcat(mac_parm_str, ".");
                        DbgPrintf(".");

                        for(int i=0;i<4;i++)
                        {
                            if(bRComponent[i])
                            {
                                strcat(mac_parm_str, pszComponents[i]);
                                DbgPrintf(pszComponents[i]);
                            }
                        }
                    }

                    DbgPrintf("\n");

                    uiCurMAC++;
                }
            }
            else
            {
                // output registers: oPos (1) 4D, oBn (2) 4D, oDn (2) 4D, oFog (1 float), oPts (1 float), oTn (4) 4D
		        int iOutReg = (pFunction[i+3] >> 3) & 0x0F;

                if(uiCurMAC == 0)
                {
                    uiCurMAC++;

                    static char *pszPick[] = 
                    {
                        "oPos", // 0
                        "????", // 1
                        "????", // 2
                        "oD0",  // 3
                        "oD1",  // 4
                        "oFog", // 5
                        "oPts", // 6
                        "oB0",  // 7
                        "oB1",  // 8
                        "oT0",  // 9
                        "oT1",  // 10
                        "oT2",  // 11
                        "oT3",  // 12
                    };

                    if(iOutReg == 15)
                    {
                        if(uiMAC != 13)
                        {
                            bRComponents = TRUE;

                            for(int i=0;i<4;i++)
                            {
                                bRComponent[i] = (pFunction[3] >> (19-i)) & 0x01;
                            }
                        }
                    }
                    else if(iOutReg == 1 || iOutReg == 2 || (iOutReg > 12))
                    {
                        EmuCleanup("Unsupported VertexShader output register (0x%.08X", iOutReg);
                    }
                    else
                    {
                        DbgPrintf("Register : %s", pszPick[iOutReg]);
                        strcat(mac_parm_str, pszPick[iOutReg]);
                    }

                    if(bOComponents && !(bOComponent[0] && bOComponent[1] && bOComponent[2] && bOComponent[3]))
                    {
                        strcat(mac_parm_str, ".");
                        DbgPrintf(".");

                        for(int i=0;i<4;i++)
                        {
                            if(bOComponent[i])
                            {
                                strcat(mac_parm_str, pszComponents[i]);
                                DbgPrintf(pszComponents[i]);
                            }
                        }
                    }

                    DbgPrintf("\n");

                    uiCurMAC++;
                }
            }

            // only input registers can be the 2nd paramter (v, r, c, a)
		    if(bPaired || ((uiMAC==0) && (uiILU !=0)))
			    uiParams[1] = (pFunction[3] >> 28) & 0x03;
		    else
			    uiParams[1] = (pFunction[2] >> 26) & 0x03;

		    if((uiMAC == 3) && !bPaired)
			    uiParams[2] = (pFunction[3] >> 28) & 0x03;
		    else
			    uiParams[2] = (pFunction[2] >> 11) & 0x03;

	        uiParams[3] = (pFunction[3] >> 28) & 0x03; // instructions like mad uses this

		    // negation
		    // output cannot be negated, bNegateParams[0] = 0
		    if((dwMaxParams == 2) && ((uiMAC != 13) && !bPaired))
			    bNegateParams[1] = (pFunction[2] >> 10) & 0x01;
		    else
			    bNegateParams[1] = (pFunction[1] >>  8) & 0x01;

		    if((uiMAC == 3) && !bPaired)
			    bNegateParams[2] = (pFunction[2] >> 10) & 0x01;
		    else
			    bNegateParams[2] = (pFunction[2] >> 25) & 0x01;

		    bNegateParams[3] = (pFunction[2] >> 10) & 0x01;

            // relative address to a0.x
		    BOOL bAddA0toCA = (pFunction[3] >> 1) & 0x01;

            // start at 2nd param
            {
                for(DWORD i=1;i<dwMaxParams;i++)
                {
                    strcat(mac_parm_str, ", ");

                    if(bNegateParams[i])
                        strcat(mac_parm_str, "-");

                    switch(uiParams[i])
                    {
                        case 1:
                            if(bPaired || ((uiMAC == 3) && (i == 2)) || ((dwMaxParams == 2) && (uiMAC != 1) && (i == 1)))
                            {
                                char szBuff[8];
                                sprintf(szBuff, "r%d", iR[3]);
                                strcat(mac_parm_str, szBuff);
                            }
                            else
                            {
                                char szBuff[8];
                                sprintf(szBuff, "r%d", iR[i]);
                                strcat(mac_parm_str, szBuff);
                            }
                            break;

                        case 2:
                            char szBuff[8];
                            sprintf(szBuff, "v%d", iV);
                            strcat(mac_parm_str, szBuff);
                            break;

                        case 3:
					        if(bAddA0toCA)
                            {
                                char szBuff[8];
                                sprintf(szBuff, "c[a0.x + %d]", iC[0]);
                                strcat(mac_parm_str, szBuff);
                            }
					        else
                            {
                                char szBuff[8];
                                sprintf(szBuff, "c%d", iC[0]);
                                strcat(mac_parm_str, szBuff);

						        // c[-1] to c[-96] requires xvs
						        if(iC[0] < 0)
                                    EmuCleanup("c[-1] to c[-96] requires XVS");
					        }
                            break;


                        default:
                            EmuCleanup("Unsupported parameter in VertexShader (0x%.08X)", uiParams[i]);
                            break;
                    }
                }
            }

            if(uiMAC != 0)
                DbgPrintf("%s %s\n", mac_inst_str, mac_parm_str);

            if(uiILU != 0)
                DbgPrintf("%s %s\n", ilu_inst_str, ilu_parm_str);

            if( (uiMAC != 0) && (uiILU != 0) )
            {
            }
        }
//        _asm int 3

        pFunction+=4;
    }
    while(!(pFunction[3] & 0x00000001));

    _asm int 3
}
