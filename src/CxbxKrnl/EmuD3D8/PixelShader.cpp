// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuD3D.cpp
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
// *  (c) 2002-2003 kingofc <kingofc@freenet.de>
// *
// *  All rights reserved
// *
// ******************************************************************
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_LOCAL_

/*

  This is a parser for the Xbox Pixel Shader Format.

  With the help of this parser it is possible to generate
  Direct3D pixel shader assembly code.

  TODO:
  - fix BumpDemo
  (after second recompilation the sahder does not work,
  can also be something in CxbxKrnl because it looks like no
  textures are set. Check cubemap loading from resourcesd!!!)
  => seems to work now, the problem is that I don't know
     how it must look on a real xbox

  - add reference counting constants which were added as c variables
    if they are compiled away (optimization of the command, etc.)
	decrement the reference count and when it reaches 0 remove
	the constant (to save the num of vars)

  - add _sat feature
    * Support as instruction modifier,
	  if necessary as mov_sat x, y
*/

// ******************************************************************
// * prevent name collisions
// ******************************************************************
//namespace xboxkrnl
//{
//    #include <xboxkrnl/xboxkrnl.h>
//};

#include "CxbxKrnl/Emu.h"
#include "CxbxKrnl/EmuFS.h"
#include "CxbxKrnl/EmuAlloc.h"
#include "CxbxKrnl/EmuXTL.h"

#include <process.h>
#include <locale.h>

//#include "EmuD3DPixelShader.h"

#define REVEL8N_PIXEL_SAHDER_CHANGES

// help functions
char *pCodeBuffer=NULL;

void WriteCode(const char *str, ...)
{
	char szBuffer[256];
	va_list argp;

	va_start(argp, str);
	vsprintf(szBuffer, str, argp);
	va_end(argp);

	//printf("%s", szBuffer);
	if(pCodeBuffer)
		strcat(pCodeBuffer, szBuffer);
}

void InsertString(char *szStr, int iOffset, char *szInsert, int iInsertLen, int iRemoveLen);

inline void HandleInputOutput
(
	DWORD dwInput, 
	DWORD dwOutput, 
	BOOL bAlpha, 
	int iCStage, 
	BOOL bUniqueC0,
	BOOL bUniqueC1,
	int *iPSC0,
	int *iPSC1,

	BOOL bGlobalRGBA,

	BOOL bFinalCombiner
);

inline void GetRegister
(
	WORD wRegister, 
	char *szRegister, 
	BOOL bUniqueC0, 
	BOOL bUniqueC1, 
	int iCStage,
	int *pPSC0,
	int *pPSC1
);

inline void GetInputMapping(WORD wInputMapping, char *szInputMapping, char *szInputMappingAfter, char *szConst);
inline void GetChannel(WORD wInputChannel, char *szInput, BOOL bAlpha, BOOL bGlobalRGBA);

inline void GetOutputFlags
(
	WORD wOutputFlags, 
	char *szInstMod, 
	char *szABOp, 
	char *szCDOp, 
	char *szABCDOp,

	BOOL *bAB_BA,
	BOOL *bCD_BA,

	BOOL *bShl1Bias,
	BOOL *bBias
);

//inline BOOL CheckOpForMov(char *szOp, char *szInputs1, char *szInput2, char *szRegInput);
inline BOOL OptimizeOperation
(
	char *szOp, 
	char *szOp1, 

	char *szOp2, 
	char *szMod,
	
	char *szInputAB1, 
	char *szInputAB2, 

	char *szInputCD1, 
	char *szInputCD2, 

	char *szConstRegAB1,
	char *szConstRegAB2,
	char *szConstRegCD1,
	char *szConstRegCD2,

	char *szOutAB,
	char *szOutCD,
	char *szABCDOutput,
	
	char *szCommand
);

inline void ClearConstRegVars();
inline void CorrectConstToReg(char *szConst, int *pPSC0, int *pPSC1);

int iPreRunLen=0;

// This is set to true if an operation tries to read from r0
// before r0 was written, in that case we do the same as the xbox
// we write the value of t0.a to r0 ;-)
BOOL bR0WAccess=FALSE;
BOOL bR0Written=FALSE;
BOOL bR0AWritten=FALSE;
/*
BOOL bR1WAccess=FALSE;
BOOL bR1AWAccess=FALSE;
BOOL bR1RGBWAccess=FALSE;

BOOL bR1AWritten=FALSE;
BOOL bR1RGBWritten=FALSE;
BOOL bR1Written=FALSE;
*/
BOOL bR0AlphaOutput = FALSE;

BOOL bLastOpRGB = FALSE;

BOOL bEFProduct = FALSE;
BOOL bV1R0Reg = FALSE;

HRESULT XTL::CreatePixelShaderFunction(X_D3DPIXELSHADERDEF *pPSD, LPD3DXBUFFER* ppRecompiled)
{
	char szCode[9000] = {0};

	pCodeBuffer = szCode;
	pCodeBuffer[0] = 0;

	ClearConstRegVars();

	#ifdef _DEBUG_TRACE
	printf("*** D3DPIXELSHADERDEF: \n");
	int i=0;
	for(int i=0; i<8; i++)
		printf("pPSD->PSAlphaInputs[%d]				: 0x%08X\n", i, pPSD->PSAlphaInputs[i]);
	printf("pPSD->PSFinalCombinerInputsABCD			: 0x%08X\n", pPSD->PSFinalCombinerInputsABCD);
	printf("pPSD->PSFinalCombinerInputsEFG			: 0x%08X\n", pPSD->PSFinalCombinerInputsEFG);
	for(i=0; i<8; i++)
		printf("pPSD->PSConstant0[%d]				: 0x%08X\n", i, pPSD->PSConstant0[i]);
	for(i=0; i<8; i++)
		printf("pPSD->PSConstant1[%d]				: 0x%08X\n", i, pPSD->PSConstant1[i]);
	for(i=0; i<8; i++)
		printf("pPSD->PSAlphaOutputs[%d]				: 0x%08X\n", i, pPSD->PSAlphaOutputs[i]);
	for(i=0; i<8; i++)
		printf("pPSD->PSRGBInputs[%d]				: 0x%08X\n", i, pPSD->PSRGBInputs[i]);

	printf("pPSD->PSCompareMode				: 0x%08X\n", pPSD->PSCompareMode);
	printf("pPSD->PSFinalCombinerConstant0			: 0x%08X\n", pPSD->PSFinalCombinerConstant0);
	printf("pPSD->PSFinalCombinerConstant1			: 0x%08X\n", pPSD->PSFinalCombinerConstant1);

	for(i=0; i<8; i++)
		printf("pPSD->PSRGBOutputs[%d]				: 0x%08X\n", i, pPSD->PSRGBOutputs[i]);

	printf("pPSD->PSCombinerCount				: 0x%08X\n", pPSD->PSCombinerCount);
	printf("pPSD->PSTextureModes				: 0x%08X\n", pPSD->PSTextureModes);
	printf("pPSD->PSDotMapping				: 0x%08X\n", pPSD->PSDotMapping);
	printf("pPSD->PSInputTexture				: 0x%08X\n", pPSD->PSInputTexture);
	printf("pPSD->PSC0Mapping				: 0x%08X\n", pPSD->PSC0Mapping);
	printf("pPSD->PSC1Mapping				: 0x%08X\n", pPSD->PSC1Mapping);
	printf("pPSD->PSFinalCombinerConstants			: 0x%08X\n", pPSD->PSFinalCombinerConstants);
	printf("*** END\n");
	#endif

	iPreRunLen=0;
	bR0Written=FALSE;
	bR0AWritten=FALSE;
	bR0WAccess=FALSE;
	bR0AlphaOutput=FALSE;
	bLastOpRGB=FALSE;

	bEFProduct = FALSE;
	bV1R0Reg = FALSE;
/*
	bR1WAccess=FALSE;
	bR1AWAccess=FALSE;
	bR1RGBWAccess=FALSE;

	bR1AWritten=FALSE;
	bR1RGBWritten=FALSE;
	bR1Written=FALSE;
*/
	// ps.1.3
	WriteCode("ps.1.3\n");

	printf("\n// Xbox hard coded pixel shader\n\n");

	// Combiner Count
	WORD wCombinerStages = (WORD) ( pPSD->PSCombinerCount     & 0xFF);
	WORD wCombinerFlags  = (WORD) ((pPSD->PSCombinerCount>>8) & 0xFF);

	// Constant mapping
	int iPSC0[8] = {0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF};
	int iPSC1[8] = {0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF};
	bool bMappedCn[16] = {false};

	int j=0;

	// Constants for all stages
	printf("// --------------------------\n");
	printf("// Constants\n");
	printf("// --------------------------\n");

	int iMap=0;
	for(j=0; j<wCombinerStages; j++)
	{
		/*print_bin(dwC0); printf("\n");
		print_bin(dwC1); printf("\n");
		float fC00 = FtoDW(dwC0);
		printf("fC0: %03.2f\n", fC00);*/

		iPSC0[j] = (pPSD->PSC0Mapping>>(j*4)) & 0x0F;
		iPSC1[j] = (pPSD->PSC1Mapping>>(j*4)) & 0x0F;

		if (j != 0)
		{
			if ((wCombinerFlags & 0x0010) == 0)
			{
				iPSC0[j] = iPSC0[0];
			}
			if ((wCombinerFlags & 0x0100) == 0)
			{
				iPSC1[j] = iPSC1[0];
			}
		}

		if (iPSC0[j] != 15)
		{
			// def cx, xval0, xval1, xval2, xval3
			DWORD dwC0 = pPSD->PSConstant0[j];
			printf("pPSD.PSConstant0[%d] = 0x%08X;\n", j, dwC0);

			float fC0[4];
			DWORD dwFloat0_0 = ((dwC0>>16) & 0xFF);
			DWORD dwFloat0_1 = ((dwC0>> 8) & 0xFF);
			DWORD dwFloat0_2 = ( dwC0      & 0xFF);
			DWORD dwFloat0_3 = ((dwC0>>24) & 0xFF);

			fC0[0] = ((float)dwFloat0_0) / 255.0f;
			fC0[1] = ((float)dwFloat0_1) / 255.0f;
			fC0[2] = ((float)dwFloat0_2) / 255.0f;
			fC0[3] = ((float)dwFloat0_3) / 255.0f;

			//WriteCode("def c%d, %ff, %ff, %ff, %ff\n", iPSC0[j], fC0[0],fC0[1],fC0[2],fC0[3]);

			bMappedCn[iPSC0[j]] = true;
		}

		if (iPSC1[j] != 15)
		{
			DWORD dwC1 = pPSD->PSConstant1[j];
			printf("pPSD.PSConstant1[%d] = 0x%08X;\n", j, dwC1);

			float fC1[4];
			DWORD dwFloat1_0 = ((dwC1>>16) & 0xFF);
			DWORD dwFloat1_1 = ((dwC1>> 8) & 0xFF);
			DWORD dwFloat1_2 = ( dwC1      & 0xFF);
			DWORD dwFloat1_3 = ((dwC1>>24) & 0xFF);

			fC1[0] = ((float)dwFloat1_0) / 255.0f;
			fC1[1] = ((float)dwFloat1_1) / 255.0f;
			fC1[2] = ((float)dwFloat1_2) / 255.0f;
			fC1[3] = ((float)dwFloat1_3) / 255.0f;

			//WriteCode("def c%d, %ff, %ff, %ff, %ff\n", iPSC1[j], fC1[0],fC1[1],fC1[2],fC1[3]);

			bMappedCn[iPSC1[j]] = true;
		}
	}

	int iFinalC0 = 0;
	int iFinalC1 = 1;

	if(pPSD->PSFinalCombinerConstants)
	{
		iFinalC0 = (int)( pPSD->PSFinalCombinerConstants       & 0x0F);
		iFinalC1 = (int)((pPSD->PSFinalCombinerConstants >> 4) & 0x0F);

// 		if(iFinalC0 == 15) {
// 			iFinalC0=iMap;
// 			iMap++;
// 		}
// 		if(iFinalC1 == 15)
// 			iFinalC1=iMap;

		DWORD dwFinalConstFlags = (pPSD->PSFinalCombinerConstants >> 8);

		printf("pPSD.PSFinalCombinerConstants = PS_FINALCOMBINERCONSTANTS(%d,%d,",
			iFinalC0, iFinalC1);

		if(dwFinalConstFlags & 0x01)
			printf("PS_GLOBALFLAGS_TEXMODE_ADJUST");
		else
			printf("PS_GLOBALFLAGS_NO_TEXMODE_ADJUST");

		printf(");\n");

		if(iFinalC0 != 0xF) // pPSD->PSFinalCombinerConstant0)
		{
			printf("\n// --------------------------\n");
			printf("// Final Combiner C0\n");
			printf("// --------------------------\n");
			printf("pPSD.PSFinalCombinerConstant0 = 0x%08X\n", pPSD->PSFinalCombinerConstant0);

			float fC0[4];
			DWORD dwFloat0_0 = ((pPSD->PSFinalCombinerConstant0>>16) & 0xFF);
			DWORD dwFloat0_1 = ((pPSD->PSFinalCombinerConstant0>> 8) & 0xFF);
			DWORD dwFloat0_2 = ( pPSD->PSFinalCombinerConstant0      & 0xFF);
			DWORD dwFloat0_3 = ((pPSD->PSFinalCombinerConstant0>>24) & 0xFF);

			fC0[0] = dwFloat0_0 / 255.0f;
			fC0[1] = dwFloat0_1 / 255.0f;
			fC0[2] = dwFloat0_2 / 255.0f;
			fC0[3] = dwFloat0_3 / 255.0f;

			//if(pPSD->PSFinalCombinerConstant0)
			//	WriteCode("def c%d, %ff, %ff, %ff, %ff\n", iFinalC0, fC0[0],fC0[1],fC0[2],fC0[3]);

			bMappedCn[iFinalC0] = true;
		}
		if(iFinalC1 != 0xF) //pPSD->PSFinalCombinerConstant1)
		{
			printf("\n// --------------------------\n");
			printf("// Final Combiner C1\n");
			printf("// --------------------------\n");
			printf("pPSD.PSFinalCombinerConstant1 = 0x%08X\n", pPSD->PSFinalCombinerConstant1);

			float fC1[4];
			DWORD dwFloat1_0 = ((pPSD->PSFinalCombinerConstant1>>16) & 0xFF);
			DWORD dwFloat1_1 = ((pPSD->PSFinalCombinerConstant1>> 8) & 0xFF);
			DWORD dwFloat1_2 = ( pPSD->PSFinalCombinerConstant1      & 0xFF);
			DWORD dwFloat1_3 = ((pPSD->PSFinalCombinerConstant1>>24) & 0xFF);

			fC1[0] = dwFloat1_0 / 255.0f;
			fC1[1] = dwFloat1_1 / 255.0f;
			fC1[2] = dwFloat1_2 / 255.0f;
			fC1[3] = dwFloat1_3 / 255.0f;

			//if(pPSD->PSFinalCombinerConstant1)
			//	WriteCode("def c%d, %ff, %ff, %ff, %ff\n", iFinalC1, fC1[0],fC1[1],fC1[2],fC1[3]);

			bMappedCn[iFinalC1] = true;
		}
	}

	for (j = 0; j < 16; ++j)
	{
		if (bMappedCn[j])
			WriteCode("def c%d, 0.0f, 0.0f, 0.0f, 0.0f\n", j);
	}

	// Texture input for all stages (0 - 3)
	WORD wTexInput[4];
	wTexInput[0] = 0;
	wTexInput[1] = 0; // here the only possible previous stage is 0
	wTexInput[2] = (WORD) ((pPSD->PSInputTexture >> 16) & 0xF);
	wTexInput[3] = (WORD) ((pPSD->PSInputTexture >> 20) & 0xF);

	printf("\n// --------------------------\n");
	printf("// Input Textures\n");
	printf("// --------------------------\n");
	printf("pPSD.PSInputTexture = PS_INPUTTEXTURE(%d,%d,%d,%d);\n",
		wTexInput[0],wTexInput[1],wTexInput[2],wTexInput[3]);

	printf("\n// --------------------------\n");
	printf("// Terxture Modes\n");
	printf("// --------------------------\n");
	printf("pPSD.PSTextureModes = PS_TEXTUREMODES(\n");

	WORD wTexModes[4];

	wTexModes[0] = (WORD) ( pPSD->PSTextureModes & 0x1F);
	wTexModes[1] = (WORD) ((pPSD->PSTextureModes>>5) & 0x1F);
	wTexModes[2] = (WORD) ((pPSD->PSTextureModes>>10) & 0x1F);
	wTexModes[3] = (WORD) ((pPSD->PSTextureModes>>15) & 0x1F);

	//char szTmp[256]="\0";
	//char szPhaseInit[256]="\0";

	// The cases were reordered to get best performance during 
	// the switch.
	// The texture modes are not in all stages valid, so the
	// ones which are only in stage three allowed can be 
	// compared at the and of the switch/case.
	for(j=0; j<4; j++)
	{
		switch(wTexModes[j])
		{
			// tex can be used on 2D, 3D and cubemap textures
			case 0x00:
				printf("PS_TEXTUREMODES_NONE");
				break;
			case 0x01:
				printf("PS_TEXTUREMODES_PROJECT2D");
				WriteCode("tex t%d\n", j);

				//WriteCode("texld r%d, t%d\n", j+2, j);
				//sprintf(szTmp, "texld r%d, t%d\n", j+2, j);
				//strcat(szPhaseInit, szTmp);
				break;
			case 0x02:
				printf("PS_TEXTUREMODES_PROJECT3D");
				WriteCode("tex t%d\n", j);

				//WriteCode("texld r%d, t%d\n", j+2, j);
				//sprintf(szTmp, "texld r%d, t%d\n", j+2, j);
				//strcat(szPhaseInit, szTmp);
				break;
			case 0x03:
				printf("PS_TEXTUREMODES_CUBEMAP");
				WriteCode("tex t%d\n", j);

				//WriteCode("texld r%d, t%d\n", j+2, j);
				//sprintf(szTmp, "texld r%d, t%d\n", j+2, j);
				//strcat(szPhaseInit, szTmp);
				break;
			case 0x04:
				printf("PS_TEXTUREMODES_PASSTHRU");
				WriteCode("texcoord t%d\n", j);
				break;
			case 0x05:
				printf("PS_TEXTUREMODES_CLIPPLANE");
				WriteCode("texkill t%d\n", j);
				break;
			case 0x06:
				printf("PS_TEXTUREMODES_BUMPENVMAP");
				WriteCode("texbem t%d, t%d\n", j, wTexInput[j]);
				break;
			case 0x07:
				printf("PS_TEXTUREMODES_BUMPENVMAP_LUM");
				WriteCode("texbeml t%d, t%d\n", j, wTexInput[j]);
				break;
			case 0x11:
				printf("PS_TEXTUREMODES_DOTPRODUCT");

				// PS_TEXTUREMODES_DOT_RFLCT_SPEC
				// PS_TEXTUREMODES_DOT_STR_3D
				// PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST
				//  are only on stage 3 valid
				if((wTexModes[3] == 0x0C) ||
				   (wTexModes[3] == 0x0D) ||
				   (wTexModes[3] == 0x12)) 
					WriteCode("texm3x3pad");
				else		   
					WriteCode("texm3x2pad");

				WriteCode(" t%d, t%d\n", j, wTexInput[j]);
				break;
			case 0x08:
				printf("PS_TEXTUREMODES_BRDF");
				WriteCode("texbrdf t%d\n", j);
				break;
			case 0x09:
				printf("PS_TEXTUREMODES_DOT_ST");
				WriteCode("texm3x2tex t%d, t%d\n", j, wTexInput[j]);
				break;
			case 0x0A:
				printf("PS_TEXTUREMODES_DOT_ZW");
				//WriteCode("texm3x2depth t%d, t%d\n", j, wTexInput[j]);
				WriteCode("texm3x2tex t%d, t%d\n", j, wTexInput[j]);
				break;
			case 0x0F:
				printf("PS_TEXTUREMODES_DPNDNT_AR");
				WriteCode("texreg2ar t%d, t%d\n", j, wTexInput[j]);
				break;
			case 0x10:
				printf("PS_TEXTUREMODES_DPNDNT_GB");
				WriteCode("texreg2gb t%d, t%d\n", j, wTexInput[j]);
				break;
			case 0x0B:
				// TODO: third param!!!
				printf("PS_TEXTUREMODES_DOT_RFLCT_DIFF");
				WriteCode("texm3x3diff t%d, t%d, c0\n", j, wTexInput[j]);
				break;
			case 0x0C:
				// TODO: third param!!!
				printf("PS_TEXTUREMODES_DOT_RFLCT_SPEC");
				WriteCode("texm3x3spec t%d, t%d, c0\n", j, wTexInput[j]);
				break;
			case 0x0D:
				printf("PS_TEXTUREMODES_DOT_STR_3D");
				WriteCode("texm3x3tex t%d, t%d\n", j, wTexInput[j]);
				break;
			case 0x0E:
				printf("PS_TEXTUREMODES_DOT_STR_CUBE");
				WriteCode("texm3x3vspec t%d, t%d\n", j, wTexInput[j]);
				break;
			case 0x12:
				// TODO: third param!!!
				printf("PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST");
				WriteCode("texm3x3spec t%d, t%d, c0\n", j, wTexInput[j]);
				break;
			default:
				printf("Unknown texture mode %d for stage %d!\n", wTexModes[j], j);
				break;
		}
		if(j!=3)
			printf(", ");
	}
	printf(");\n");

	//WriteCode(szPhaseInit);

	//WriteCode("mov r1, t1.a\n");

	// Handle stage code
	// Save the length of the tex/def stuff
	iPreRunLen = strlen(szCode);

	printf("\n// --------------------------\n");
	printf("// Combiner Count\n");
	printf("// --------------------------\n");
	printf("pPSD.PSCombinerCount = PS_COMBINERCOUNT(%d, ", wCombinerStages);

	// MSB/LSB
	if(wCombinerFlags & 0x0001)
		printf("PS_COMBINERCOUNT_MUX_MSB");
	else
		printf("PS_COMBINERCOUNT_MUX_LSB");
	printf(" | ");

	// C0
	if(wCombinerFlags & 0x0010) {
		printf("PS_COMBINERCOUNT_UNIQUE_C0");
	} else
		printf("PS_COMBINERCOUNT_SAME_C0");
	printf(" | ");

	// C1
	if(wCombinerFlags & 0x0100)
		printf("PS_COMBINERCOUNT_UNIQUE_C1");
	else
		printf("PS_COMBINERCOUNT_SAME_C1");
	printf(");\n");

	// print constant mapping
	printf("\n// --------------------------\n");
	printf("// Constant Mappings\n");
	printf("// --------------------------\n");
	printf("pPSD.PSC0Mapping = PS_CONSTANTMAPPING(%d,%d,%d,%d,%d,%d,%d,%d);\n"
		   "pPSD.PSC1Mapping = PS_CONSTANTMAPPING(%d,%d,%d,%d,%d,%d,%d,%d);\n",
		   iPSC0[0],iPSC0[1],iPSC0[2],iPSC0[3],iPSC0[4],iPSC0[5],iPSC0[6],iPSC0[7],
		   iPSC1[0],iPSC1[1],iPSC1[2],iPSC1[3],iPSC1[4],iPSC1[5],iPSC1[6],iPSC1[7]);

	for(j=0; j<wCombinerStages; j++)
	{
		/*if(j==4)
		{
			WriteCode("phase\n");
			WriteCode("texld r0, r0\n");
			WriteCode("texld r1, r1\n");
			WriteCode(szPhaseInit);
		}*/

		WriteCode("\n; Stage %d\n", j);

		// Compare the RGB with alpha operation.
		// If both is the same do it only for one and leave out the masks.
		// 
		// the mask removes the channel
		if(
			((pPSD->PSRGBInputs[j] & 0xEFEFEFEF) == (pPSD->PSAlphaInputs[j] & 0xEFEFEFEF)) &&
			(pPSD->PSRGBOutputs[j] == pPSD->PSAlphaOutputs[j])
			)
		{
			printf("\n// --------------------------\n");
			printf("// RGB/Alpha\n");
			printf("// --------------------------\n");

			HandleInputOutput(
			pPSD->PSRGBInputs[j], 
			pPSD->PSRGBOutputs[j], 
			FALSE, 
			j, 
			wCombinerFlags & 0x0010 ? 1 : 0,
			wCombinerFlags & 0x0100 ? 1 : 0,
			iPSC0,
			iPSC1,
			
			TRUE,

			FALSE);
		} else {

			printf("\n// --------------------------\n");
			printf("// RGB\n");
			printf("// --------------------------\n");

			// PSRGBInputs/Outputs
			HandleInputOutput(
				pPSD->PSRGBInputs[j], 
				pPSD->PSRGBOutputs[j], 
				FALSE, 
				j, 
				wCombinerFlags & 0x0010 ? 1 : 0,
				wCombinerFlags & 0x0100 ? 1 : 0,
				iPSC0,
				iPSC1,
				FALSE,
				FALSE);

			printf("\n// --------------------------\n");
			printf("// Alpha\n");
			printf("// --------------------------\n");

			// PSAlphaInputs/Outputs
			HandleInputOutput(
				pPSD->PSAlphaInputs[j], 
				pPSD->PSAlphaOutputs[j], 
				TRUE, 
				j, 
				wCombinerFlags & 0x0010 ? 1 : 0,
				wCombinerFlags & 0x0100 ? 1 : 0,
				iPSC0,
				iPSC1,
				FALSE,
				FALSE);
		}
	}

	if(pPSD->PSFinalCombinerInputsABCD || pPSD->PSFinalCombinerInputsEFG)
	{
		printf("\n// --------------------------\n");
		printf("// Final Combiner\n");
		printf("// --------------------------\n");

		HandleInputOutput(
			pPSD->PSFinalCombinerInputsABCD, 
			pPSD->PSFinalCombinerInputsEFG, 
			FALSE, 
			0, 
			wCombinerFlags & 0x0010 ? 1 : 0,
			wCombinerFlags & 0x0100 ? 1 : 0,
			&iFinalC0,
			&iFinalC1,
			FALSE,
			TRUE);
	}

	if(!bR0AlphaOutput && !bR0WAccess)
	{
		//char szNull[20] = "0";
		//CorrectConstToReg(szNull, iPSC0, iPSC1);

		// pair only if final combiner,
		// this one would write to rgb or the last instruction wrote to rgb (bLastOpRGB=TRUE)
		if(bLastOpRGB || (pPSD->PSFinalCombinerInputsABCD || pPSD->PSFinalCombinerInputsEFG))
			WriteCode("+ ");

		//WriteCode("mov r0.a, r2.a\n");
		WriteCode("mov r0.a, t0.a\n");
	}

	// and this:
	// The Xbox sets r0 to the alpha channel of texture 0, so it can be read before written!
	if(bR0WAccess)
	{
		char *szNewCodeBuffer = (char *)CxbxMalloc((strlen(szCode)+20)*sizeof(char));
		strncpy(szNewCodeBuffer, szCode, iPreRunLen);
		strcat(szNewCodeBuffer, "mov r0, t0.a\n");
		strcat(szNewCodeBuffer, &szCode[iPreRunLen]);
		strcpy(szCode, szNewCodeBuffer);

		CxbxFree(szNewCodeBuffer);
	}
	/*DbgPrintf("r1 case! ... ");
	if(bR1WAccess || bR1AWAccess || bR1RGBWAccess)
	{
		char szChannel[6]="\0";
		char szCat[24]="\0";

		if(!bR1WAccess && bR1AWAccess) strcpy(szChannel, ".a");
		else if(!bR1WAccess && bR1RGBWAccess) strcpy(szChannel, ".rgb");

		sprintf(szCat, "mov r1%s, t1%s\n", szChannel, szChannel);

		char *szNewCodeBuffer = (char *)malloc((strlen(szCode)+50)*sizeof(char));
		strncpy(szNewCodeBuffer, szCode, iPreRunLen);
		strcat(szNewCodeBuffer, szCat);
		strcat(szNewCodeBuffer, &szCode[iPreRunLen]);
		strcpy(szCode, szNewCodeBuffer);
		free(szNewCodeBuffer);
	}
	DbgPrintf("end\n");*/

	printf("\n*** RESULT: \n%s\n", szCode);
	printf("*** PIXEL SHADER CREATION FINISHED!\n");
	pCodeBuffer = NULL;

	DumpPixelShaderDefToFile(pPSD, szCode);

	//if(bR1WAccess || bR1AWAccess || bR1RGBWAccess)
	//	Sleep(3000);

	XTL::LPD3DXBUFFER pCompilationErrors = NULL;

	HRESULT hRet = D3D_OK;

	__try
	{
		szCode[5] = '4';
		hRet = D3DXAssembleShader(szCode,
			strlen(szCode),
			0, //D3DXASM_SKIPVALIDATION,
			NULL,
			ppRecompiled,
			&pCompilationErrors);

		if (FAILED(hRet))
		{
			if (pCompilationErrors)
			{
				pCompilationErrors->Release();
				pCompilationErrors = NULL;
			}

			szCode[5] = '3';
			hRet = D3DXAssembleShader(szCode,
				strlen(szCode),
				0, //D3DXASM_SKIPVALIDATION,
				NULL,
				ppRecompiled,
				&pCompilationErrors);
		}

		if (FAILED(hRet))
		{
			EmuWarning("Couldn't assemble recompiled vertex shader\n");
			if (pCompilationErrors)
			{
				EmuWarning((const char*)pCompilationErrors->GetBufferPointer());
			}
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		DbgPrintf("Pixel Shader : Exception while creating pixel shader 0x%.8X", pPSD);
	}
	if (pCompilationErrors)
	{
		pCompilationErrors->Release();
	}

	return hRet;
}

#define DEF_VAR_TABLE_LEN	7
char szVar[][10] =
{
	"r0",
	"r1",
	"t0",
	"t1",
	"t2",
	"t3",
	"t4"
};

inline void HandleInputOutput
(
	DWORD dwInput, 
	DWORD dwOutput, 
	BOOL bAlpha, 
	int iCStage, 
	BOOL bUniqueC0,
	BOOL bUniqueC1,
	int *iPSC0,
	int *iPSC1,

	BOOL bGlobalRGBA,

	BOOL bFinalCombiner
)
{
	// INPUTS
	if(bFinalCombiner)	printf("\npPSD.PSFinalCombinerInputsABCD = PS_COMBINERINPUTS(\n");
	else if(bAlpha)		printf("\npPSD.PSAlphaInputs[%d] = PS_COMBINERINPUTS(\n", iCStage);
	else				printf("\npPSD.PSRGBInputs[%d] = PS_COMBINERINPUTS(\n", iCStage);

	WORD wCombinerInputs[4]; // 0=a, 1=b, 2=c, 3=d
	wCombinerInputs[0] = (WORD) ((dwInput>>24) & 0xFF);
	wCombinerInputs[1] = (WORD) ((dwInput>>16) & 0xFF);
	wCombinerInputs[2] = (WORD) ((dwInput>> 8) & 0xFF);
	wCombinerInputs[3] = (WORD) ( dwInput      & 0xFF);

	char szInput[4][20] = {0};
	char szConst[4][20] = {0};
	char szInputMapping[4][20] = {0};
	char szInputMappingAfter[4][20] = {0};
	char szChannels[4][5] = {0};

	// Go through inputs
	int i=0;
	for(i=0; i<4; i++)
	{
		szInput[i][0]=0x00; // Fast way to zero a string ;-)
		szConst[i][0]=0x00;
		szInputMapping[i][0]=0x00;
		szInputMappingAfter[i][0]=0x00;
		szChannels[i][0]=0x00;

		GetRegister(wCombinerInputs[i] & 0xF, szInput[i], bUniqueC0, bUniqueC1, iCStage, iPSC0, iPSC1);
		
		if(strcmp(szInput[i], "r0")==0)
		{
			if(!bR0AWritten)
				strcpy(szInput[i], "t0");

			if(!bR0Written) {
				strcpy(szInput[i], "t0");
				//bR0WAccess=TRUE;
			}
		}

		printf(" | ");
		GetInputMapping(wCombinerInputs[i] & 0x1E0, szInputMapping[i], szInputMappingAfter[i], szConst[i]);
		printf(" | ");
		GetChannel(wCombinerInputs[i] & 0x10, szChannels[i], bAlpha, bGlobalRGBA);
		printf(",\n");

		if((wCombinerInputs[i] & 0xF)==0x00)
			szInput[i][0]=0x00;

		// 6928: check this as I doubt whether it works really like that
		/*if(strcmp(szInput[i], "r1")==0)
		{
		//	DbgPrintf("channel: %s\n", szChannels[i]);
		//	Sleep(3000);

			if((strcmp(szChannels[i], ".a")==0) && (!bR1AWritten)) {
				bR1AWAccess=TRUE;

				strcpy(szInput[i], " t1");
			} else if((strcmp(szChannels[i], ".rgb")==0) && (!bR1RGBWritten)) {
				bR1RGBWAccess=TRUE;

				strcpy(szInput[i], " t1");
			} else if(!bR1Written) {
				bR1WAccess=TRUE;

				strcpy(szInput[i], " t1");
			}

			if(bR1AWAccess && bR1RGBWAccess)
				bR1WAccess=TRUE;

			//if(bR1AWAccess || bR1RGBWAccess)
			//	strcpy(szInput[i], "t1");
		}*/

		//printf("\n*** szInput[%d]: %s\n", i, szInput[i]);
	}

	// Input stuff
	BOOL bInput[4] = {0, 0, 0, 0};
	if(szInput[0][0]) bInput[0]=TRUE;
	if(szInput[1][0]) bInput[1]=TRUE;
	if(szInput[2][0]) bInput[2]=TRUE;
	if(szInput[3][0]) bInput[3]=TRUE;

#ifdef REVEL8N_PIXEL_SAHDER_CHANGES
	// Correct param if a constant is used!
	if(!bInput[0])
		CorrectConstToReg(szConst[0], iPSC0, iPSC1);
	if(!bInput[1])
		CorrectConstToReg(szConst[1], iPSC0, iPSC1);
	if(!bInput[2])
		CorrectConstToReg(szConst[2], iPSC0, iPSC1);
	if(!bInput[3])
		CorrectConstToReg(szConst[3], iPSC0, iPSC1);

	bool bEmptyChannel = false;
#endif

	char szCompleteInput[4][20] = {0};
	for(i=0; i<4; i++)
	{
		strcpy(szCompleteInput[i], szInputMapping[i]);
#ifdef REVEL8N_PIXEL_SAHDER_CHANGES
		if(bInput[i])
		{
#endif
			strcat(szCompleteInput[i], szInput[i]);
#ifdef REVEL8N_PIXEL_SAHDER_CHANGES
			bEmptyChannel = bEmptyChannel || (szChannels[i][0] == 0);
		}
		else
			strcat(szCompleteInput[i], &szConst[i][4]);
#endif
		strcat(szCompleteInput[i], szInputMappingAfter[i]);
		strcat(szCompleteInput[i], szChannels[i]);
	}

	printf(");\n");

	if(!bFinalCombiner)
	{
		// OUTPUTS
		if(bAlpha)	printf("\npPSD.PSAlphaOutputs[%d] = PS_COMBINEROUTPUTS(\n", iCStage);
		else		printf("\npPSD.PSRGBOutputs[%d] = PS_COMBINEROUTPUTS(\n", iCStage);

		WORD wCombinerOutputs[3]; // 0=d0 (ab), 1=d1 (cd), 2=d2 (mux_sum)
		wCombinerOutputs[0]       = (WORD) ((dwOutput>> 4) & 0xF);
		wCombinerOutputs[1]       = (WORD) ( dwOutput      & 0xF);
		wCombinerOutputs[2]       = (WORD) ((dwOutput>> 8) & 0xF);
		WORD wCombinerOutputFlags = (WORD) ((dwOutput>>12) & 0xFF);

		char szOutput[3][10] = {0};
		char szOutputMod[10]="\0";

		char szABOp[10]="\0";
		char szCDOp[10]="\0";
		char szABCDOp[10]="\0";

		BOOL bAB_B2A;
		BOOL bCD_B2A;

		BOOL bR0Now = FALSE;
		BOOL bR0ANow = FALSE;
		BOOL bVAccess[3] = {0,0,0};

		BOOL bOpRGB_Current = FALSE;
		BOOL bCurrOpRealAlpha = FALSE;

		// Go through outputs
		for(i=0; i<3; i++)
		{
			szOutput[i][0]=0x00; // Fast way to zero a string ;-)

			GetRegister(wCombinerOutputs[i], szOutput[i], bUniqueC0, bUniqueC1, iCStage, iPSC0, iPSC1);
			if(strcmp(szOutput[i], "r0")==0) 
			{ 
				bR0Now=TRUE; 

				// this checks for output to r0.a
				if(bGlobalRGBA || (!bGlobalRGBA && bAlpha))
					bR0AlphaOutput=TRUE;
			}

			if((strcmp(szOutput[i], "v0")==0) || (strcmp(szOutput[i], "v1")==0)) { bVAccess[i] = TRUE; }

			/*BOOL bR1_Written = FALSE;
			if(strcmp(szOutput[i], "r1")==0) 
			bR1_Written=TRUE;*/

			// check channel!
			if(!bGlobalRGBA && bAlpha)
			{
				strcat(szOutput[i], ".a");
				bCurrOpRealAlpha = TRUE;

				if(bR0Now)
					bR0ANow=TRUE;

				/*if(bR1_Written) 
				bR1AWritten=TRUE;*/
			}
			else if(!bGlobalRGBA && !bAlpha
#ifdef REVEL8N_PIXEL_SAHDER_CHANGES
				&& !bEmptyChannel
#endif
				) 
			{
				strcat(szOutput[i], ".rgb");

				if(wCombinerOutputs[i])
					bOpRGB_Current = TRUE;

				/*if(bR1_Written) 
				bR1RGBWritten=TRUE;*/
			}
			else
			{
				/*if(bR1_Written) 
				bR1Written=TRUE;*/

				if(bR0Now)
					bR0ANow=TRUE;
			}

			printf(",\n");

			if(wCombinerOutputs[i]==0x00)
				szOutput[i][0]=0x00;

			//printf("\n*** szOutput[%d]: %s\n", i, szOutput[i]);
		}

		BOOL bBias=FALSE;
		BOOL bSh1Bias=FALSE;

		GetOutputFlags(
			wCombinerOutputFlags, 
			szOutputMod,

			szABOp,
			szCDOp,
			szABCDOp,

			&bAB_B2A,
			&bCD_B2A,

			&bSh1Bias,
			&bBias);

		if(bR0Now)
			bR0Written=TRUE;

		if(bR0ANow)
			bR0AWritten=TRUE;

		printf(");\n");

		// Find output for the operations
		char szOut[10]="\0";
		char szOut1[10]="\0";

		//printf("|****| %s |****|\n", szOutput[1]);

		if(szOutput[0][0])
			strcpy(szOut, szOutput[0]);
		if(szOutput[1][0])
			strcpy(szOut1, szOutput[1]);

#ifndef REVEL8N_PIXEL_SAHDER_CHANGES
		if(szOutput[2][0]) 
		{
			/*
			//EmuWarningMsg("THIS IS WRONG, FIX ME!");
			//if(!szOutput[1][0])
			//	strcpy(szOut1, szOutput[2]);
			DbgPrintf("(!szOutput[0][0] || !szOutput[1][0]) && szOutput[2][0] = TRUE!\n");

			BOOL bUsable=TRUE;
			for(i=2; i<4; i++)
			{
			if((strcmp(szOutput[2], szInput[i])==0)  || (strcmp(szOutput[2], szOut1)==0)) {
			bUsable=FALSE;
			}
			}
			if(bUsable && !szOutput[0][0]) 
			{

			strcpy(szOut, szOutput[2]);

			DbgPrintf("BUsable = TRUE, new output: %s\n", szOut);

			}
			else {
			printf("!WARNING!: The operation uses the output register also as input!"
			"Trying to find a free output register. It is possible that the pixel shader "
			"will generate garbage because the new free one contains data used "
			"in an other comming operation!\n\n");

			for(int j=0; j<DEF_VAR_TABLE_LEN; j++)
			{
			// look for a free one
			BOOL bFoundOne=TRUE;
			for(i=2; i<4; i++)
			{
			if((strcmp(szVar[j], szInput[i])==0) || (strcmp(szVar[j], szOut1)==0)) 
			bFoundOne=FALSE;
			else {
			bFoundOne=TRUE;
			goto found;
			}

			}
			found:
			if(bFoundOne && !szOutput[0][0]) {
			strcpy(szOut, szVar[j]);
			//if(!bGlobalRGBA && bAlpha)
			//	strcat(szOut, ".a");
			//else if(!bGlobalRGBA && !bAlpha)
			//	strcat(szOut, ".rgb");
			break;
			}
			}
			}
			//} else { // no CD operation
			//	strcpy(szOut,  szOutput[0]);
			//	strcpy(szOut1, szOutput[0]);
			//}
			*/

			if(!szOutput[1][0])
			{
				strcpy(szOut, "t2");
				//strcpy(szOut, "r4");
			}
			if(!szOutput[0][0])
			{
				strcpy(szOut1, "t3");
				//strcpy(szOut1, "r5");
			}
		}
#endif

#ifndef REVEL8N_PIXEL_SHADER_CHANGES
		// Correct param if a constant is used!
		if(!bInput[0])
			CorrectConstToReg(szConst[0], iPSC0, iPSC1);
		if(!bInput[1])
			CorrectConstToReg(szConst[1], iPSC0, iPSC1);
		if(!bInput[2])
			CorrectConstToReg(szConst[2], iPSC0, iPSC1);
		if(!bInput[3])
			CorrectConstToReg(szConst[3], iPSC0, iPSC1);
#endif

		// As first do the ab operation
		char szCmd[100]="\0";
		BOOL bOptimized = OptimizeOperation(
			szABOp, 
			szCDOp, 
			bVAccess[2] ? "" : szABCDOp, 
			szOutputMod, 

#ifdef REVEL8N_PIXEL_SAHDER_CHANGES
			bInput[0] ? szInput[0] : szConst[0],
			bInput[1] ? szInput[1] : szConst[1],
			bInput[2] ? szInput[2] : szConst[2],
			bInput[3] ? szInput[3] : szConst[3],
#else
			bInput[0] ? szCompleteInput[0] : szConst[0],
			bInput[1] ? szCompleteInput[1] : szConst[1],
			bInput[2] ? szCompleteInput[2] : szConst[2],
			bInput[3] ? szCompleteInput[3] : szConst[3],
#endif

			// the real input now
#ifdef REVEL8N_PIXEL_SAHDER_CHANGES
			szCompleteInput[0],
			szCompleteInput[1],
			szCompleteInput[2],
			szCompleteInput[3],
#else
			bInput[0] ? szCompleteInput[0] : &szConst[0][4],
			bInput[1] ? szCompleteInput[1] : &szConst[1][4],
			bInput[2] ? szCompleteInput[2] : &szConst[2][4],
			bInput[3] ? szCompleteInput[3] : &szConst[3][4],
#endif

			szOut,
			szOut1,
			szOutput[2],

			szCmd);

		// TODO: check for problems with strange bias stuff
		//if(!bGlobalRGBA && bAlpha && (szOut[0] || szOut1[0] || szOutput[2][0]))
		if (bLastOpRGB && (!bOpRGB_Current) && bCurrOpRealAlpha && 
#ifdef REVEL8N_PIXEL_SAHDER_CHANGES
			bOptimized &&
#endif
			(szCmd[0] || 
			(szOut[0] && !bVAccess[0]) || 
			(szOut1[0] && !bVAccess[1]) || 
			(szOutput[2][0] && !bVAccess[2]))) 
		{
//#ifndef REVEL8N_PIXEL_SAHDER_CHANGES
			WriteCode("+ ");
//#endif
		}

		if(szCmd[0] || 
			(szOut[0] && !bVAccess[0]) || 
			(szOut1[0] && !bVAccess[1]) || 
			(szOutput[2][0] && !bVAccess[2]))
		{
			bLastOpRGB = bOpRGB_Current;
		}

		if(szCmd[0])
		{
			WriteCode("%s", szCmd);
		}
		else 
		{

			if(szOut[0] && !bVAccess[0])
			{
				BOOL bBiasDone=FALSE;
				if(bBias) {
					if((strcmp(szABOp, "mov")==0) && !szInputMappingAfter[0][0]) {
						bBiasDone=TRUE;
						WriteCode("mov%s %s, %s_bias\n", szOutputMod, szOut, szCompleteInput[0]);
					}
				}

				if(!bBiasDone)
					WriteCode("%s%s %s, %s%s%s\n", 
					szABOp, 
					szOutputMod,

					szOut,

					bInput[0] ? szCompleteInput[0] : &szConst[0][4],
					(bInput[1] || szConst[1][4]) ? ", " : "",
					bInput[1] ? szCompleteInput[1] : &szConst[1][4]);

				if(bBias && !bBiasDone)
					WriteCode("mov %s, %s_bias\n", szOut, szOut);

				if(bSh1Bias)
					WriteCode("mov_x2 %s, %s_bias\n", szOut, szOut);

				// Blue To Alpha
				//if(bAB_B2A)
				//	WriteCode("mov %s.a %s.b\n", szOut, szOut);
			}

			if((bInput[2] || szConst[2][0]) && ((bInput[3] || szConst[3][0]) || (strcmp(szCDOp, "mov")==0)) && szOut1[0])
			{
				if(szOut1[0] && !bVAccess[1])
				{
					BOOL bBiasDone=FALSE;
					if(bBias) {
						if((strcmp(szABOp, "mov")==0) && !szInputMappingAfter[2][0]) {
							bBiasDone=TRUE;
							WriteCode("mov%s %s, %s_bias\n", szOutputMod, szOut1, szCompleteInput[2]);
						}
					}

					if(!bBiasDone)
						WriteCode("%s%s %s, %s%s%s\n", 
						szCDOp, 
						szOutputMod,

						szOut1,

						bInput[2] ? szCompleteInput[2] : &szConst[2][4],
						(bInput[3] || szConst[3][4]) ? ", " : "",
						bInput[3] ? szCompleteInput[3] : &szConst[3][4]);

					if(bBias && !bBiasDone)
						WriteCode("mov %s, %s_bias\n", szOut1, szOut1);
					if(bSh1Bias)
						WriteCode("mov_x2 %s, %s_bias\n", szOut1, szOut1);
				}

				// Blue To Alpha
				//if(bCD_B2A)
				//	WriteCode("mov %s.a %s.b\n", szOut1, szOut1);
				//}
			}

			if(szOutput[2][0] && !bVAccess[2])
			{
				/*BOOL bBiasDone=FALSE;
				if(bBias) {
				if((strcmp(szABOp, "mov")==0) && !szInputMappingAfter[2][0]) {
				bBiasDone=TRUE;
				WriteCode("mov%s %s, %s_bias\n", szOutputMod, szOut1, szCompleteInput[2]);
				}
				}*/

				WriteCode("%s%s %s, %s%s%s\n", 
					szABCDOp, 
					szOutputMod,

					szOutput[2],
					szOut,
					szOut1[0] ? ", " : "",
					szOut1);

				if(bBias)
					WriteCode("mov %s, %s_bias\n", szOutput[2], szOutput[2]);
				if(bSh1Bias)
					WriteCode("mov_x2 %s, %s_bias\n", szOutput[2], szOutput[2]);
			}

		}

	} // end of !bFinalCombiner
	else 
	{
		//* only ~~~%!?&&$3§~~~ support yet !!!

		WriteCode("\n; Final Combiner\n");
		printf("pPSD.PSFinalCombinerInputsEFG = PS_COMBINERINPUTS(\n");

		DWORD dwV1R0_EFProd_Flags	= dwOutput & 0xFF; 

		WORD wEFG[3];
		wEFG[0]	= (WORD) ((dwOutput >> 24) & 0xFF);
		wEFG[1]	= (WORD) ((dwOutput >> 16) & 0xFF);
		wEFG[2]	= (WORD) ((dwOutput >>  8) & 0xFF);

		BOOL bInputEFG[3] = {0, 0, 0};
		char szCompleteInputEFG[3][10];

		char szInputEFG[3][10];
		char szInputMappingEFG[3][10];
		char szInputMappingAfterEFG[3][10];
		char szConstEFG[3][10];

		for(i=0; i<3; i++)
		{
			szInputEFG[i][0]=0x00;
			szInputMappingEFG[i][0]=0x00;
			szInputMappingAfterEFG[i][0]=0x00;
			szConstEFG[i][0]=0x00;

			GetRegister(wEFG[i] & 0xF, szInputEFG[i], bUniqueC0, bUniqueC1, 0, iPSC0, iPSC1);
			printf(" | ");
			GetInputMapping(wEFG[i] & 0x1E0, szInputMappingEFG[i], szInputMappingAfterEFG[i], szConstEFG[i]);
			printf(" | ");
			GetChannel(wEFG[i] & 0x10, szInputEFG[i], bAlpha, FALSE);
			printf(", \n");

			strcpy(szCompleteInputEFG[i], szInputMappingEFG[i]);
			strcat(szCompleteInputEFG[i], szInputEFG[i]);
			strcat(szCompleteInputEFG[i], szInputMappingAfterEFG[i]);

			if(szInputEFG[i][0])
				bInputEFG[i]=TRUE;
			else
			{
				// add that constant as a reg
				CorrectConstToReg(szConstEFG[i], iPSC0, iPSC1);
			}
		}

		if(dwV1R0_EFProd_Flags  & 0x20)
			printf("PS_FINALCOMBINERSETTINGS_COMPLEMENT_R0");
		else if(dwV1R0_EFProd_Flags  & 0x40)
			printf("PS_FINALCOMBINERSETTINGS_COMPLEMENT_V1");
		else if(dwV1R0_EFProd_Flags  & 0x80)
			printf("PS_FINALCOMBINERSETTINGS_CLAMP_SUM");
		else
			printf("0");

		printf(");\n");

		if (bV1R0Reg)
		{
			char sMod[10] = {0};
			char sV1[10] = {0};
			char sR0[10] = {0};
			if(dwV1R0_EFProd_Flags  & 0x20)
				strcpy(sR0, "1-");
			else if(dwV1R0_EFProd_Flags  & 0x40)
				strcpy(sV1, "1-");
			else if(dwV1R0_EFProd_Flags  & 0x80)
				strcpy(sMod, "_sat");

			if (bEFProduct)
			{
				EmuWarning("EF Product and V1R0 register used at the same time!\n");
			}
			else
			{
				WriteCode("; (v1 + r0)\nadd%s r0, %sr0, %sv1\n\n", sMod, sR0, sV1);
			}
		}

		// only we we will use this later in final combiner stuff!!
		// all inputs are known now, so check:
		if(bEFProduct) {

			// r0 = E * F (E or F must be the r0 calculated before otherwise the stage results
			// are lost, problem???
			if(!
				((!bInputEFG[0] && szConstEFG[0][0]=='0') &&
				(!bInputEFG[1] && szConstEFG[1][0]=='0'))) {
					WriteCode(";E * F\nmul r0, %s, %s\n\n", bInputEFG[0] ? szCompleteInputEFG[0] : &szConstEFG[0][4],
						bInputEFG[1] ? szCompleteInputEFG[1] : &szConstEFG[1][4]);
			}

		}

		// Now the result:

		// What is done by the final combiner:
		// final color = s0*s1 + (1-s0)*s2 + s3

		// lrp r0, s0, s1, s2
		// add r0, r0, s3
		// s0 = szInput[0]
		// s1 = szInput[1]
		// s2 = szInput[2]
		// s3 = szInput[3]

		// Check whether it is a mov r0, r0
		// for example: lrp r0, 1, r0, 0
		// r0 = 1*r0 + (1-1)*r0 + 0
		// --> r0 = r0

		for(i=0; i<4; i++)
		{
			if(!bInput[i])
				CorrectConstToReg(szConst[i], iPSC0, iPSC1);
		}

		if(!((!bInput[0]) && (szConst[0][0] == '1') && (strncmp(szCompleteInput[1], "r0", 2)==0))) 
		{
			// cases for s2
			// s2 == 0 --> final color = s0*s1 + s3
			if((!bInput[2]) && (szConst[2][0] == '0'))
			{
				WriteCode("mul r0.rgb, %s, %s\n",
					bInput[0] ? szCompleteInput[0] : &szConst[0][4],
					bInput[1] ? szCompleteInput[1] : &szConst[1][4]);
			} 
			// s0 == 0 --> final color = s2 + s3
			else if((!bInput[0]) && (szConst[0][0] == '0')) {
				// Check whether s2 is r0!!!
				if(!(bInput[2] && (strncmp(szCompleteInput[2], "r0", 2)==0)))
					WriteCode("mov r0.rgb, %s\n",
					bInput[2] ? szCompleteInput[2] : &szConst[2][4]);
			}
			// s0 == 1 --> final color = s1 + s3
			else if((!bInput[0]) && (szConst[0][0] == '1')) {
				// Check whether s1 is r0!!!
				if(!(bInput[1] && (strncmp(szCompleteInput[1], "r0", 2)==0)))
					WriteCode("mov r0.rgb, %s\n",
					bInput[1] ? szCompleteInput[1] : &szConst[1][4]);
			}
			// no special cases
			else if(bInput[2] || bInput[0])
			{
				WriteCode("lrp r0.rgb, %s, %s, %s\n",
					bInput[0] ? szCompleteInput[0] : &szConst[0][4],
					bInput[1] ? szCompleteInput[1] : &szConst[1][4],
					bInput[2] ? szCompleteInput[2] : &szConst[2][4]);
			}
		}
		// case for s3
		if(bInput[3] || (szConst[3][0] != '0'))
			WriteCode("add r0.rgb, r0, %s\n", bInput[3] ? szCompleteInput[3] : &szConst[3][4]);

		// Alpha ouput (G)
		if(bInputEFG[2] && (strncmp(szInputEFG[2], "r0", 2)!=0))
		{
			bR0AlphaOutput=TRUE;

			WriteCode("mov r0.a, %s\n",
				bInputEFG[2] ? szCompleteInputEFG[2] : &szConstEFG[2][4]);
		}

		//else
		//	WriteCode("mov r0.a, v0.a\n");
		//*/
		//Sleep(3000);
	}
}

inline void GetRegister(WORD wRegister, char *szRegister, BOOL bUniqueC0, BOOL bUniqueC1, int iCStage, int *iPSC0, int *iPSC1)
{
	// Determine register
	switch(wRegister)
	{
		case 0x00:
			printf("PS_REGISTER_ZERO");
			break;
		case 0x01: // read
			printf("PS_REGISTER_C0");
			if(bUniqueC0)
				sprintf(szRegister, "c%d", iPSC0[iCStage]);
			else
				strcpy(szRegister, "c0");
			break;
		case 0x02: // read
			printf("PS_REGISTER_C1");
			if(bUniqueC0)
				sprintf(szRegister, "c%d", iPSC1[iCStage]);
			else
				strcpy(szRegister, "c1");
			break;
		case 0x03: // read
		{
			printf("PS_REGISTER_FOG");

			char szOneHalf[40] = "0.5\0";
			CorrectConstToReg(szOneHalf, iPSC0, iPSC1);

			strcpy(szRegister, &szOneHalf[4]); // Unsupported
			break;
		}
		case 0x04: // read/(write ???)
			printf("PS_REGISTER_V0");
			strcpy(szRegister, "v0");
			break;
		case 0x05: // read/(write ???)
			printf("PS_REGISTER_V1");
			strcpy(szRegister, "v1");
			break;
		case 0x08: // read/write
			printf("PS_REGISTER_T0");
			strcpy(szRegister, "t0");
			//strcpy(szRegister, "r2");
			break;
		case 0x09: // read/write
			printf("PS_REGISTER_T1");
			strcpy(szRegister, "t1");
			//strcpy(szRegister, "r3");
			break;
		case 0x0A: // read/write
			printf("PS_REGISTER_T2");
			strcpy(szRegister, "t2");
			//strcpy(szRegister, "r4");
			break;
		case 0x0B: // read/write
			printf("PS_REGISTER_T3");
			strcpy(szRegister, "t3");
			//strcpy(szRegister, "r5");
			break;
		case 0x0C: // read/write
			printf("PS_REGISTER_R0");
			strcpy(szRegister, "r0");
			break;
		case 0x0D: // read/write
			printf("PS_REGISTER_R1");
			strcpy(szRegister, "r1");
			break;
		case 0x0E: // read
			printf("PS_REGISTER_V1R0_SUM");

			bV1R0Reg = TRUE;
			strcpy(szRegister, "r0"); //"V1R0");//(v1+r0)");
			break;
		case 0x0F:
			printf("PS_REGISTER_EF_PROD");

			// we save it in r0
			bEFProduct = TRUE;
			strcpy(szRegister, "r0");/* e * f --> combiner input */
			break;
		default:
			printf("/*Unknown register %d*/", wRegister);
			break;
	}
}

inline void GetInputMapping(WORD wInputMapping, char *szInputMapping, char *szInputMappingAfter, char *szConst)
{
	strcpy(szConst, "0");
	switch(wInputMapping)
	{
		case 0x00: // max(0,x)       [ok for final combiner]
			printf("PS_INPUTMAPPING_UNSIGNED_IDENTITY");
			break;
		case 0x20: // 1 - max(0,x)   [ok for final combiner]
			printf("PS_INPUTMAPPING_UNSIGNED_INVERT");
			strcpy(szInputMapping, "1-");
			strcpy(szConst, "1");
			break;
		case 0x40: // 2*max(0,x) - 1 [invalid for final combiner]
			printf("PS_INPUTMAPPING_EXPAND_NORMAL");
			strcpy(szInputMappingAfter, "_bx2"); // right???
			strcpy(szConst, "-1");
			break;
		case 0x60: // 1 - 2*max(0,x) [invalid for final combiner]
			printf("PS_INPUTMAPPING_EXPAND_NEGATE");

			strcpy(szInputMapping, "-");
			strcpy(szInputMappingAfter, "_bx2");
			strcpy(szConst, "1");
			break;
		case 0x80: // max(0,x) - 1/2 [invalid for final combiner]
			printf("PS_INPUTMAPPING_HALFBIAS_NORMAL");
			strcpy(szInputMappingAfter, "_bias");

			strcpy(szConst, "-0.5");
			break;
		case 0xA0: // 1/2 - max(0,x) [invalid for final combiner]
			printf("PS_INPUTMAPPING_HALFBIAS_NEGATE");

			strcpy(szConst, "0.5");

			// Negate is run last if combined with bias
			strcpy(szInputMapping, "-");
			strcpy(szInputMappingAfter, "_bias");
			break;
		case 0xC0: // x              [invalid for final combiner]
			printf("PS_INPUTMAPPING_SIGNED_IDENTITY");
			break;
		case 0xE0: // -x             [invalid for final combiner]
			printf("PS_INPUTMAPPING_SIGNED_NEGATE");
			strcpy(szInputMapping, "-");
			break;
		default:
			printf("/*Unknown input mapping %d!*/", wInputMapping);
			break;
	}
}

inline void GetChannel(WORD wInputChannel, char *szInput, BOOL bAlpha, BOOL bGlobalRGBA)
{
	switch(wInputChannel)
	{
		case 0x00:
			if(bAlpha) {
				printf("PS_CHANNEL_BLUE");
				strcat(szInput, ".b");
			} else {
				printf("PS_CHANNEL_RGB");

				//if (!bGlobalRGBA)
				//	strcat(szInput, ".rgb");
			}
			break;
		case 0x10:
			printf("PS_CHANNEL_ALPHA");

			// TODO: check this || !bAlpha, it should mean that alpha channel
			// is detected in a RGB register, then it must be set also
			// if both commands the same are (in that case it has to be RGB!)
			if (!bGlobalRGBA || !bAlpha)
				strcat(szInput, ".a");
			break;
		default:
			printf("/*Unknown channel %d!*/", wInputChannel);
			break;
	}
}

inline void GetOutputFlags
(
	WORD wOutputFlags, 
	char *szInstMod, 
	char *szABOp, 
	char *szCDOp, 
	char *szABCDOp,

	BOOL *bAB_BA,
	BOOL *bCD_BA,

	BOOL *bShl1Bias,
	BOOL *bBias
)
{
	// Output mapping 
	if(wOutputFlags & 0x08)
	{
		printf("PS_COMBINEROUTPUT_BIAS"); // y = x - 0.5
		//strcpy(szInstMod, "_bias");

		// Only over this:
		// mov y, y_bias
		(*bBias)=TRUE;
	}
	else if(wOutputFlags & 0x10) 
	{
		printf("PS_COMBINEROUTPUT_SHIFTLEFT_1");  // y = x*2
		strcpy(szInstMod, "_x2");
	}
	else if(wOutputFlags & 0x18) 
	{
		printf("PS_COMBINEROUTPUT_SHIFTLEFT_1_BIAS"); // y = (x - 0.5)*2

		//strcpy(szInstMod, "_x2");
		// what is missing is a subtraction of 1
		// --> 2 * (x - 0.5) = 2x - 1

		// But this won't work because we would have to do 2 movs
		// to subtract 1
		// Let's do this: mov_x2 y, y_bias
		(*bShl1Bias)=TRUE;
	}
	else if(wOutputFlags & 0x20)
	{
		printf("PS_COMBINEROUTPUT_SHIFTLEFT_2");  // y = x*4
		strcpy(szInstMod, "_x4");
	}
	else if(wOutputFlags & 0x30)
	{
		printf("PS_COMBINEROUTPUT_SHIFTRIGHT_1"); // y = x/2
		strcpy(szInstMod, "_d2");
	}
	else
		printf("PS_COMBINEROUTPUT_IDENTITY");

	printf(" | ");

	// MUX operation
	if(wOutputFlags & 0x04) {
		printf("PS_COMBINEROUTPUT_AB_CD_MUX");
		strcpy(szABCDOp, "cnd");

		if((!bR0Written) || (!bR0AWritten))
			bR0WAccess=TRUE;
	} 
	else
	{
		printf("PS_COMBINEROUTPUT_AB_CD_SUM"); // 3rd output is AB+CD
		strcpy(szABCDOp, "add");
	}

	printf(" | ");

	// Function for ab side
	if(wOutputFlags & 0x02)
	{
		printf("PS_COMBINEROUTPUT_AB_DOT_PRODUCT"); // RGB only
		strcpy(szABOp, "dp3");
	} else {
		printf("PS_COMBINEROUTPUT_AB_MULTIPLY");
		strcpy(szABOp, "mul");
	}

	printf(" | ");

	// Functiomn for cd side
	if(wOutputFlags & 0x01) 
	{
		printf("!!!PS_COMBINEROUTPUT_CD_DOT_PRODUCT!!!"); // RGB only
		strcpy(szCDOp, "dp3");
	} else {
		printf("PS_COMBINEROUTPUT_CD_MULTIPLY");
		strcpy(szCDOp, "mul");
	}

	// Blue to alpha for ab side
	if(wOutputFlags & 0x80) {
		printf(" | PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA"); // RGB only
		(*bAB_BA)=TRUE;
	} else (*bAB_BA)=FALSE;

	// Blue to alpha for cd side
	if(wOutputFlags & 0x40) {
		printf(" | PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA"); // RGB only
		(*bCD_BA)=TRUE;
	} else (*bCD_BA)=FALSE;
}

enum OpType
{
	OPTYPE_NOP = -1,
	OPTYPE_MOV = 0,
	OPTYPE_ADD,
	OPTYPE_MUL,
	OPTYPE_DP3,
	OPTYPE_CND,
};

inline BOOL OptimizeOperation
(
	char *szOp, 
	char *szOp1, 

	char *szOp2, 
	char *szMod,
	
	char *szInputAB1, 
	char *szInputAB2, 

	char *szInputCD1, 
	char *szInputCD2, 

	char *szConstRegAB1,
	char *szConstRegAB2,
	char *szConstRegCD1,
	char *szConstRegCD2,

	char *szOutAB,
	char *szOutCD,
	char *szABCDOutput,
	
	char *szCommand)
{
	printf("----------\nszOp: |%s|\nszOp1: |%s|\nszOp2: |%s|\nszMod: |%s|\n"
           "szInputAB1: |%s|\nszInputAB2: |%s|\nszInputCD1: |%s|\nszInputCD2: |%s|\n"
		   "szOutAB: |%s|\nszOutCD: |%s|\nszABCDOutput: |%s|\n",
		szOp, szOp1, szOp2, szMod, szInputAB1, szInputAB2, szInputCD1, szInputCD2,
		szOutAB, szOutCD, szABCDOutput);

	char szABCDInput[2][10];
	szABCDInput[0][0]=0x00;
	szABCDInput[1][0]=0x00;

	szCommand[0]=0x00;

	char *szOps[3];
	szOps[0] = szOp;
	szOps[1] = szOp1;
	szOps[2] = szOp2;

	char *szInputs[4];
	szInputs[0] = szInputAB1;
	szInputs[1] = szInputAB2;
	szInputs[2] = szInputCD1;
	szInputs[3] = szInputCD2;

	char *szRealInputs[4];
	szRealInputs[0] = szConstRegAB1;
	szRealInputs[1] = szConstRegAB2;
	szRealInputs[2] = szConstRegCD1;
	szRealInputs[3] = szConstRegCD2;

#ifdef REVEL8N_PIXEL_SAHDER_CHANGES
	char *szOutputs[3];
	szOutputs[0] = szOutAB;
	szOutputs[1] = szOutCD;
	szOutputs[2] = szABCDOutput;
#endif

	// TODO: check mov: other operations like lrp
	// are ignored because of a shitty mul with 1
	BOOL bMov[3]={0, 0, 0};

	int i=0;
	for(i=0; i<2; i++)
	{
		//printf("szOps[i]: %s\n", szOps[i]);
		//printf("szInputs[i*2+1]: %s\n", szInputs[i*2+1]);
		if(strcmp(szOps[i], "mul")==0) 
		{
			// If it is a mul, it can also be only a mov
			if(strcmp(szInputs[i*2], "1")==0)  {
				//strcpy(szABCDInput[i], szInputs[i*2+1]);
#ifndef REVEL8N_PIXEL_SAHDER_CHANGES
				strcpy(szABCDInput[i], szRealInputs[i*2+1]);
#endif

				strcpy(szOps[i], "mov");

				strcpy(szInputs[i*2], szInputs[i*2+1]);
				strcpy(szRealInputs[i*2], szRealInputs[i*2+1]);

				strcpy(szInputs[i*2+1], "");
				strcpy(szRealInputs[i*2+1], "");

				bMov[i]=TRUE;

			} else if(strcmp(szInputs[i*2+1], "1")==0) {
				//strcpy(szABCDInput[i], szInputs[i*2]);
#ifndef REVEL8N_PIXEL_SAHDER_CHANGES
				strcpy(szABCDInput[i], szRealInputs[i*2]);
#endif

				strcpy(szOps[i], "mov");

				strcpy(szInputs[i*2+1], "");
				strcpy(szRealInputs[i*2+1], "");

				bMov[i]=TRUE;
			}
		}
	}

#ifdef REVEL8N_PIXEL_SAHDER_CHANGES
	OpType eOpTypes[3] = {OPTYPE_NOP, OPTYPE_NOP, OPTYPE_NOP};
	for (i = 0; i < 3; ++i)
	{
		if (strcmp(szOps[i], "mov") == 0)
			eOpTypes[i] = OPTYPE_MOV;
		else if (strcmp(szOps[i], "add") == 0)
			eOpTypes[i] = OPTYPE_ADD;
		else if (strcmp(szOps[i], "mul") == 0)
			eOpTypes[i] = OPTYPE_MUL;
		else if (strcmp(szOps[i], "dp3") == 0)
			eOpTypes[i] = OPTYPE_DP3;
		else if (strcmp(szOps[i], "cnd") == 0)
			eOpTypes[i] = OPTYPE_CND;
		else
			eOpTypes[i] = OPTYPE_NOP;
	}

	bool bHandled = false;
	int iOffset = 0;
	int iOpCount = 0;
	if (szOps[2][0] && szOutputs[2][0] && szOutputs[2][0] != 'v')
	{
		if (!szOutputs[0][0] &&
			!szOutputs[1][0])
		{
			if (szMod[0])
			{
				EmuWarning("Pixel Shader: Destination modifier present!\n");
			}
			switch (eOpTypes[2])
			{
			case OPTYPE_ADD:
				{
					if (eOpTypes[0] == OPTYPE_MOV &&
						eOpTypes[1] == OPTYPE_MOV)
					{
						iOffset += sprintf(szCommand + iOffset, "add%s %s, %s, %s\n",
							szMod, szOutputs[2], szRealInputs[0], szRealInputs[2]);
						++iOpCount;
						bHandled = true;
					}
					else if (eOpTypes[0] == OPTYPE_MOV &&
						eOpTypes[1] == OPTYPE_MUL)
					{
						iOffset += sprintf(szCommand + iOffset, "mad%s %s, %s, %s, %s\n",
							szMod, szOutputs[2], szRealInputs[2], szRealInputs[3], szRealInputs[0]);
						bHandled = true;
						++iOpCount;
					}
					else if (eOpTypes[0] == OPTYPE_MUL &&
						eOpTypes[1] == OPTYPE_MOV)
					{
						iOffset += sprintf(szCommand + iOffset, "mad%s %s, %s, %s, %s\n",
							szMod, szOutputs[2], szRealInputs[0], szRealInputs[1], szRealInputs[2]);
						bHandled = true;
						++iOpCount;
					}
					else if (eOpTypes[0] == OPTYPE_MUL &&
						eOpTypes[1] == OPTYPE_MUL)
					{
						// nice, mul, mul, add can be converted to lrp
						// lrp r0, t0, t1, c2
						// --> r0 = t0 * t1 + (1-t0) * c2
						// or  r0 = c2 + t0 * (t1 - c2), but that would mean we have to mul in the ABCD op
						//                               and that is not possible

						for(i=0; i<2; i++)
						{
							// To match the first option, the first input of the AB/CD op must inverted
							BOOL bInvert[2] = {0, 0};
							if((szRealInputs[2*i][0] == '1') && (szRealInputs[2*i][1] == '-'))
								//if((szInputs[2*i][0] == '1') && (szInputs[2*i][1] == '-'))
								bInvert[0]=TRUE;

							if((szRealInputs[2*i+1][0] == '1') && (szRealInputs[2*i+1][1] == '-'))
								//if((szInputs[2*i+1][0] == '1') && (szInputs[2*i+1][1] == '-'))
								bInvert[1]=TRUE;

							//printf("szInputs[2*i]: %s\nszInputs[2*i+1]: %s\n", szInputs[2*i], szInputs[2*i+1]);
							//printf("bInvert[0]: %d\nbInvert[1]: %d\n", bInvert[0], bInvert[1]);

							if((bInvert[0] || bInvert[1]) && (!(bInvert[0] && bInvert[1])))
							{
								char szParam[3][10] = {0};
								char szRealParam0[10] = {0};
								if(bInvert[0]) 
								{
									// copy over the not inverted param
									strcpy(szParam[i+1], /*szInputs*/szRealInputs[2*i+1]);

									// and the inverted
									strcpy(szParam[0], &szInputs[2*i][2]);
									strcpy(szRealParam0, &szRealInputs[2*i][2]);
								} 
								else if(bInvert[1]) 
								{
									// copy over the not inverted param
									strcpy(szParam[i+1], /*szInputs*/szRealInputs[2*i]);

									// and the inverted
									strcpy(szParam[0], &szInputs[2*i+1][2]);
									strcpy(szRealParam0, &szRealInputs[2*i+1][2]);
								}
								int iOtherOp = i == 0 ? 1 : 0;

								bHandled = true;
								if (strcmp(szRealInputs[2*iOtherOp], szRealParam0/*szParam[0]*/)==0)
									strcpy(szParam[iOtherOp+1], /*szInputs*/szRealInputs[2*iOtherOp+1]);
								else if (strcmp(szRealInputs[2*iOtherOp+1], szRealParam0/*szParam[0]*/)==0)
									strcpy(szParam[iOtherOp+1], /*szInputs*/szRealInputs[2*iOtherOp]);
								else
									bHandled = false;
								if (bHandled)
								{
									// ok, we have it
									iOffset += sprintf(szCommand, "lrp%s %s, %s, %s, %s\n",
										szMod, szABCDOutput, szRealParam0/*szParam[0]*/, szParam[1], szParam[2]);
									++iOpCount;
									break;
								}
							}
						}

						if (!bHandled)
						{
							iOffset += sprintf(szCommand + iOffset, "mul r1, %s, %s\n",
								szRealInputs[0], szRealInputs[1]);
							++iOpCount;
							iOffset += sprintf(szCommand + iOffset, "mad%s %s, %s, %s, r1\n",
								szMod, szOutputs[2], szRealInputs[2], szRealInputs[3]);
							++iOpCount;

							bHandled = true;
						}
					}
				}
				break;
			case OPTYPE_CND:
				{
					if (eOpTypes[0] == OPTYPE_MOV &&
						eOpTypes[1] == OPTYPE_MOV)
					{
						iOffset += sprintf(szCommand + iOffset, "cnd%s %s, r0.a, %s, %s\n",
							szMod, szOutputs[2], szRealInputs[2], szRealInputs[0]);
						++iOpCount;
						bHandled = true;
					}
					else if (eOpTypes[0] == OPTYPE_MUL &&
						eOpTypes[1] == OPTYPE_MUL)
					{
						if (szOutputs[2][0] != 'r')
						{
							EmuWarning("Pixel Shader: Destination not temporary register!\n");
						}
						// ab input
						iOffset += sprintf(szCommand + iOffset, "mul%s r1, %s, %s\n",
							szMod, szRealInputs[0], szRealInputs[1]);
						++iOpCount;
						// cd input
						iOffset += sprintf(szCommand + iOffset, "mul%s r0, %s, %s\n",
							szMod, szRealInputs[2], szRealInputs[3]);
						++iOpCount;
						// abcd output
						iOffset += sprintf(szCommand + iOffset, "cnd%s %s, r0.a, r0, r1\n",
							szMod, szOutputs[2]);
						++iOpCount;
						bHandled = true;
					}
				}
				break;
			}
			if (!bHandled && strcmp(szOps[2], "add") == 0)
			{
				if ((strcmp(szOps[0], "mov")==0))
				{
					if ((strcmp(szOps[1], "mul")==0))
					{
						char szParam[10]="\0";

						if(strcmp(szInputCD1, "-1")==0)
							strcpy(szParam, szInputCD2);
						else if(strcmp(szInputCD2, "-1")==0)
							strcpy(szParam, szInputCD1);

						if(szParam[0] && szConstRegAB1[0] && szABCDOutput[0]) 
						{
							iOffset += sprintf(szCommand, "sub%s %s, %s, %s\n",
								szMod, szABCDOutput, szConstRegAB1, szParam);
							bHandled = true;
							++iOpCount;
						}
// 						else
// 						{
// 							iOffset += sprintf(szCommand + iOffset, "mad%s %s, %s, %s, %s\n",
// 								szMod, szOutputs[2], szRealInputs[2], szRealInputs[3], szRealInputs[0]);
// 							bHandled = true;
// 							++iOpCount;
// 						}
					}
				}
// 				else if ((strcmp(szOps[0], "mul")==0))
// 				{
// 					if ((strcmp(szOps[1], "mov")==0))
// 					{
// 						iOffset += sprintf(szCommand + iOffset, "mad%s %s, %s, %s, %s\n",
// 							szMod, szOutputs[2], szRealInputs[0], szRealInputs[1], szRealInputs[2]);
// 						bHandled = true;
// 						++iOpCount;
// 					}
// 					else if ((strcmp(szOps[1], "mul")==0))
// 					{
// 						// nice, mul, mul, add can be converted to lrp
// 						// lrp r0, t0, t1, c2
// 						// --> r0 = t0 * t1 + (1-t0) * c2
// 						// or  r0 = c2 + t0 * (t1 - c2), but that would mean we have to mul in the ABCD op
// 						//                               and that is not possible
// 
// 						for(i=0; i<2; i++)
// 						{
// 							// To match the first option, the first input of the AB/CD op must inverted
// 							BOOL bInvert[2] = {0, 0};
// 							if((szRealInputs[2*i][0] == '1') && (szRealInputs[2*i][1] == '-'))
// 							//if((szInputs[2*i][0] == '1') && (szInputs[2*i][1] == '-'))
// 								bInvert[0]=TRUE;
// 
// 							if((szRealInputs[2*i+1][0] == '1') && (szRealInputs[2*i+1][1] == '-'))
// 							//if((szInputs[2*i+1][0] == '1') && (szInputs[2*i+1][1] == '-'))
// 								bInvert[1]=TRUE;
// 
// 							//printf("szInputs[2*i]: %s\nszInputs[2*i+1]: %s\n", szInputs[2*i], szInputs[2*i+1]);
// 							//printf("bInvert[0]: %d\nbInvert[1]: %d\n", bInvert[0], bInvert[1]);
// 
// 							if((bInvert[0] || bInvert[1]) && (!(bInvert[0] && bInvert[1])))
// 							{
// 								char szParam[3][10];
// 								char szRealParam0[10];
// 								if(bInvert[0]) 
// 								{
// 									// copy over the not inverted param
// 									strcpy(szParam[2], /*szInputs*/szRealInputs[2*i+1]);
// 
// 									// and the inverted
// 									strcpy(szParam[0], &szInputs[2*i][2]);
// 									strcpy(szRealParam0, &szRealInputs[2*i][2]);
// 								} 
// 								else if(bInvert[1]) 
// 								{
// 									// copy over the not inverted param
// 									strcpy(szParam[2], /*szInputs*/szRealInputs[2*i]);
// 
// 									// and the inverted
// 									strcpy(szParam[0], &szInputs[2*i+1][2]);
// 									strcpy(szRealParam0, &szRealInputs[2*i+1][2]);
// 								}
// 								int iOtherOp = i == 0 ? 1 : 0;
// 
// 								bHandled = true;
// 								if (strcmp(szRealInputs[2*iOtherOp], szRealParam0/*szParam[0]*/)==0)
// 									strcpy(szParam[1], /*szInputs*/szRealInputs[2*iOtherOp+1]);
// 								else if (strcmp(szRealInputs[2*iOtherOp+1], szRealParam0/*szParam[0]*/)==0)
// 									strcpy(szParam[1], /*szInputs*/szRealInputs[2*iOtherOp]);
// 								else
// 									bHandled = false;
// 								if (bHandled)
// 								{
// 									// ok, we have it
// 									iOffset += sprintf(szCommand, "lrp%s %s, %s, %s, %s\n",
// 										szMod, szABCDOutput, szRealParam0/*szParam[0]*/, szParam[1], szParam[2]);
// 									++iOpCount;
// 									break;
// 								}
// 							}
// 						}
// 
// 						if (!bHandled)
// 						{
// 							iOffset += sprintf(szCommand + iOffset, "mul r1, %s, %s\n",
// 								szRealInputs[0], szRealInputs[1]);
// 							++iOpCount;
// 							iOffset += sprintf(szCommand + iOffset, "mad%s %s, %s, %s, r1\n",
// 								szMod, szOutputs[2], szRealInputs[2], szRealInputs[3]);
// 							++iOpCount;
// 
// 							bHandled = true;
// 						}
// 					}
// 				}
			}
		}
	}

	if (!bHandled)
	{
		for (i = 0; i < 2; ++i)
		{
			if (szOps[i][0] && szOutputs[i][0] && szOutputs[i][0] != 'v')
			{
				++iOpCount;
				// copy output value to final input
				strcpy(szABCDInput[i], szOutputs[i]);
				// insert command
				iOffset += sprintf(szCommand + iOffset, "%s%s %s, %s\n", szOps[i], szMod, szOutputs[i], szRealInputs[i * 2 + 0]);

				// if there are more parameters...
				if (szRealInputs[i * 2 + 1][0])
				{
					// backspace of the newline character
					--iOffset;
					// insert remaining parameters
					iOffset += sprintf(szCommand + iOffset, ", %s\n", szRealInputs[i * 2 + 1]);
				}
				bHandled = true;
			}
		}

// 		if (szOutputs[2][0])
// 		{
// 			if(!szOutputs[1][0])
// 				strcpy(szOutputs[1], "r0");
// 			if(!szOutputs[0][0])
// 				strcpy(szOutputs[0], "r1");
// 		}

		if (szOps[2][0] && szOutputs[2][0] && szOutputs[2][0] != 'v')
		{
			switch (eOpTypes[2])
			{
			case OPTYPE_ADD:
				{
					if (szABCDInput[0][0] &&
						szABCDInput[1][0])
					{
						iOffset += sprintf(szCommand + iOffset, "add%s %s, %s, %s\n",
							szMod, szOutputs[2], szABCDInput[0], szABCDInput[1]);
						++iOpCount;
						bHandled = true;
					}
					else if (szABCDInput[0][0] &&
						!szABCDInput[1][0])
					{
						switch (eOpTypes[1])
						{
						case OPTYPE_MUL:
							{
								iOffset += sprintf(szCommand + iOffset, "mad%s %s, %s, %s, %s\n",
									szMod, szOutputs[2], szRealInputs[2], szRealInputs[3], szABCDInput[0]);
								++iOpCount;
								bHandled = true;
							}
							break;
						case OPTYPE_DP3:
							{
								{
									++iOpCount;
									// insert command
									iOffset += sprintf(szCommand + iOffset, "%s%s r1, %s\n", szOps[1], szMod, szRealInputs[2]);

									// if there are more parameters...
									if (szRealInputs[3][0])
									{
										// backspace of the newline character
										--iOffset;
										// insert remaining parameters
										iOffset += sprintf(szCommand + iOffset, ", %s\n", szRealInputs[3]);
									}
								}
								{
									iOffset += sprintf(szCommand + iOffset, "add%s %s, %s, r1\n",
										szMod, szOutputs[2], szABCDInput[0]);
									++iOpCount;
									bHandled = true;
								}
							}
							break;
						default:
							break;
						}
					}
					else if (!szABCDInput[0][0] &&
						szABCDInput[1][0])
					{
						switch (eOpTypes[0])
						{
						case OPTYPE_MUL:
							{
								iOffset += sprintf(szCommand + iOffset, "mad%s %s, %s, %s, %s\n",
									szMod, szOutputs[2], szRealInputs[0], szRealInputs[1], szABCDInput[1]);
								++iOpCount;
								bHandled = true;
							}
							break;
						case OPTYPE_DP3:
							{
								{
									++iOpCount;
									// insert command
									iOffset += sprintf(szCommand + iOffset, "%s%s r1, %s\n", szOps[0], szMod, szRealInputs[0]);

									// if there are more parameters...
									if (szRealInputs[1][0])
									{
										// backspace of the newline character
										--iOffset;
										// insert remaining parameters
										iOffset += sprintf(szCommand + iOffset, ", %s\n", szRealInputs[1]);
									}
								}
								{
									iOffset += sprintf(szCommand + iOffset, "add%s %s, r1, %s\n",
										szMod, szOutputs[2], szABCDInput[1]);
									++iOpCount;
									bHandled = true;
								}
							}
							break;
						default:
							break;
						}
					}
				}
				break;
			case OPTYPE_CND:
				{
					if (szABCDInput[0][0] &&
						szABCDInput[1][0])
					{
						iOffset += sprintf(szCommand + iOffset, "cnd%s %s, r0.a, %s, %s\n",
							szMod, szOutputs[2], szABCDInput[1], szABCDInput[0]);
						++iOpCount;
						bHandled = true;
					}
					else if (szABCDInput[0][0] &&
						!szABCDInput[1][0])
					{
						{
							++iOpCount;
							// insert command
							iOffset += sprintf(szCommand + iOffset, "%s%s r1, %s\n", szOps[1], szMod, szRealInputs[2]);

							// if there are more parameters...
							if (szRealInputs[3][0])
							{
								// backspace of the newline character
								--iOffset;
								// insert remaining parameters
								iOffset += sprintf(szCommand + iOffset, ", %s\n", szRealInputs[3]);
							}
						}
						{
							iOffset += sprintf(szCommand + iOffset, "cnd%s %s, r0.a, r1, %s\n",
								szMod, szOutputs[2], szABCDInput[0]);
							++iOpCount;
							bHandled = true;
						}
					}
					else if (!szABCDInput[0][0] &&
						szABCDInput[1][0])
					{
						{
							++iOpCount;
							// insert command
							iOffset += sprintf(szCommand + iOffset, "%s%s r1, %s\n", szOps[0], szMod, szRealInputs[0]);

							// if there are more parameters...
							if (szRealInputs[1][0])
							{
								// backspace of the newline character
								--iOffset;
								// insert remaining parameters
								iOffset += sprintf(szCommand + iOffset, ", %s\n", szRealInputs[1]);
							}
						}
						{
							iOffset += sprintf(szCommand + iOffset, "cnd%s %s, r0.a, %s, r1\n",
								szMod, szOutputs[2], szABCDInput[1]);
							++iOpCount;
							bHandled = true;
						}
					}
				}
				break;
			}
			if (!bHandled)
			{
				EmuWarning("Unhandled pixel shader instruction!\n");
			}
// 			if (strcmp(szOps[2], "add") == 0)
// 			{
// 				if (szABCDInput[0][0] &&
// 					szABCDInput[1][0])
// 				{
// 					iOffset += sprintf(szCommand + iOffset, "add%s %s, %s, %s\n",
// 						szMod, szOutputs[2], szABCDInput[1], szABCDInput[0]);
// 					++iOpCount;
// 					bHandled = true;
// 				}
// 				else
// 				{
// 					EmuWarning("Unhandled pixel shader instruction!\n");
// 				}
// 			}
// 			else if (strcmp(szOps[2], "cnd") == 0)
// 			{
// 				if (szABCDInput[0][0] &&
// 					szABCDInput[1][0])
// 				{
// 					iOffset += sprintf(szCommand + iOffset, "cnd%s %s, r0.a, %s, %s\n",
// 						szMod, szOutputs[2], szABCDInput[1], szABCDInput[0]);
// 					++iOpCount;
// 					bHandled = true;
// 				}
// 				else
// 				{
// 					EmuWarning("Unhandled pixel shader instruction!\n");
// 				}
// 			}
// 			else
// 			{
// 				EmuWarning("Unhandled pixel shader instruction!\n");
// 			}
		}
	}

	if(szCommand[0])
		printf("new command:\n%s\n", szCommand);
	return (bHandled && (iOpCount == 1)) ? (TRUE) : (FALSE);
#endif

	if(
		(strcmp(szOp,  "mul")==0) &&
		(strcmp(szOp1, "mov")==0) && //bMov[1]					 &&
		(strcmp(szOp2, "add")==0) &&
		szABCDOutput[0])
	{
		sprintf(szCommand, "mad%s %s, %s, %s, %s\n", 
			szMod, szABCDOutput, 
			/*szInput*/szConstRegAB1, 
			/*szInput*/szConstRegAB2, 
			/*szInput*/szConstRegCD1 /*because it's a mov now*/);
	}
	else if(
		(strcmp(szOp,  "mul")==0) &&
		(strcmp(szOp1, "mul")==0) &&
		(strcmp(szOp2, "add")==0) &&
		szABCDOutput[0]) // TODO: check that strange lrp/ABCDOutput[0]=0 case
	{
		// nice, mul, mul, add can be converted to lrp
		// lrp r0, t0, t1, c2
		// --> r0 = t0 * t1 + (1-t0) * c2
		// or  r0 = c2 + t0 * (t1 - c2), but that would mean we have to mul in the ABCD op
		//                               and that is not possible

		for(i=0; i<2; i++)
		{
			// To match the first option, the first input of the AB/CD op must inverted
			BOOL bInvert[2] = {0, 0};
			if((szInputs[2*i][0] == '1') && (szInputs[2*i][1] == '-'))
				bInvert[0]=TRUE;

			if((szInputs[2*i+1][0] == '1') && (szInputs[2*i+1][1] == '-'))
				bInvert[1]=TRUE;

			//printf("szInputs[2*i]: %s\nszInputs[2*i+1]: %s\n", szInputs[2*i], szInputs[2*i+1]);
			//printf("bInvert[0]: %d\nbInvert[1]: %d\n", bInvert[0], bInvert[1]);

			if((bInvert[0] || bInvert[1]) && (!(bInvert[0] && bInvert[1])))
			{
				char szParam[3][10];
				char szRealParam0[10];
				if(bInvert[0]) 
				{
					// copy over the not inverted param
					strcpy(szParam[2], /*szInputs*/szRealInputs[2*i+1]);

					// and the inverted
					strcpy(szParam[0], &szInputs[2*i][2]);
					strcpy(szRealParam0, &szRealInputs[2*i][2]);
				} 
				else if(bInvert[1]) 
				{
					// copy over the not inverted param
					strcpy(szParam[2], /*szInputs*/szRealInputs[2*i]);

					// and the inverted
					strcpy(szParam[0], &szInputs[2*i+1][2]);
					strcpy(szRealParam0, &szRealInputs[2*i+1][2]);
				}
				int iOtherOp = i == 0 ? 1 : 0;

				if(strcmp(szInputs[2*iOtherOp], szParam[0])==0)
					strcpy(szParam[1], /*szInputs*/szRealInputs[2*iOtherOp+1]);
				else
					strcpy(szParam[1], /*szInputs*/szRealInputs[2*iOtherOp]);
				// ok, we have it
				sprintf(szCommand, "lrp%s %s, %s, %s, %s\n",
					szMod, szABCDOutput, szRealParam0/*szParam[0]*/, szParam[1], szParam[2]);

				break;
			}
		}
	} else if(strcmp(szOp2, "cnd")==0) {
#ifdef REVEL8N_PIXEL_SAHDER_CHANGES
		int iOffset = 0;
		int i = 0;
		for (i = 0; i < 2; ++i)
		{
			if (strcmp(szOps[i], "mul")==0)
			{
				strcpy(szABCDInput[i], szOutputs[i]);
				iOffset += sprintf(szCommand + iOffset, "mul %s, %s, %s\n", szOutputs[i], szRealInputs[i * 2 + 0], szRealInputs[i * 2 + 1]);
			}
		}
		sprintf(szCommand + iOffset, "cnd%s %s, %s, %s, %s\n",
			szMod, szABCDOutput, "r0.a", szABCDInput[1], szABCDInput[0]);
#else
		sprintf(szCommand, "cnd%s %s, %s, %s, %s\n",
			szMod, szABCDOutput, "r0.a", szABCDInput[1], szABCDInput[0]);
#endif

		bMov[1]=0;
		bMov[0]=0;
	} else if(
		(strcmp(szOp,  "mov")==0) &&
		(strcmp(szOp1, "mul")==0) &&
		(strcmp(szOp2, "add")==0))
	{
		char szParam[10]="\0";

		if(strcmp(szInputCD1, "-1")==0)
			strcpy(szParam, szInputCD2);
		else if(strcmp(szInputCD2, "-1")==0)
			strcpy(szParam, szInputCD1);
		
		if(szParam[0] && szConstRegAB1[0] && szABCDOutput[0]) 
		{
			sprintf(szCommand, "sub%s %s, %s, %s\n",
				szMod, szABCDOutput, szConstRegAB1, szParam);
		}

	}
//do_operation_with_new_input:

	if(bMov[0] && bMov[1] && szABCDOutput[0]) {
		sprintf(szCommand, "%s%s %s, %s, %s\n", szOp2, szMod, szABCDOutput, szABCDInput[0], szABCDInput[1]);
	}

	if(szCommand[0])
		printf("new command: %s", szCommand);
	return TRUE;
}

float fConstants[20] = {0.0f};
int iConstants[20] = {0};
int iConstCount=0;

inline void ClearConstRegVars()
{
	iConstCount=0;
	memset(fConstants, 0x00, 20*sizeof(float));
	memset(iConstants, 0x00, 20*sizeof(int));
}

inline void CorrectConstToReg(char *szConst, int *pPSC0, int *pPSC1)
{
	printf("Looking for %s\n", szConst);
	float fConst = (float)atof(szConst);
	
	// check whether we already saved it
	int i=0;
	for(i=0; i<iConstCount; i++) {
		if(fConst == fConstants[i]) {
			sprintf(&szConst[4], "c%d", iConstants[i]);
			goto CorrectConstToReg_done;
		}
	}

	for(i=2; i<8; i++) 
	{
		// Go through all stages and check whether used
		BOOL bUsed=FALSE;
		for(int j=0; j<8; j++) {
			if((pPSC0[j] == i)||(pPSC1[j] == i)||(iConstants[j] == i)) {
				bUsed=TRUE;
				break;
			}
		}

		if(!bUsed) 
		{
			fConstants[iConstCount] = fConst;
			iConstants[iConstCount] = i;
			sprintf(&szConst[4], "c%d", i);
			printf("detected: %lf:%s\n", fConst, &szConst[4]);

			// Add this const to the beginning of the psh
			char str[100];
			char *szNewCodeBuffer = (char *)malloc((strlen(pCodeBuffer)+70)*sizeof(char));
			strncpy(szNewCodeBuffer, pCodeBuffer, 7);
			szNewCodeBuffer[7]=0x00;
			sprintf(str, "def c%d, %ff, %ff, %ff, %ff\n", i, 
				fConst,fConst,fConst,fConst);
			iPreRunLen+=strlen(str);
			strcat(szNewCodeBuffer, str);
			strcat(szNewCodeBuffer, &pCodeBuffer[7]);
			strcpy(pCodeBuffer, szNewCodeBuffer);

			free(szNewCodeBuffer);

			iConstCount++;

			printf("added: %s\n", str);
			//Sleep(3000);
			break;
		}
	}
CorrectConstToReg_done:;
}

// check
bool XTL::IsValidPixelShader(void)
{
	if(g_CurrentPixelShader && (g_CurrentPixelShader->dwStatus != 0))
		return false;
	return true;
}

/*
 * Blueshogun's code (useful for debugging the PixelShader binary format)
 */

// PS Texture Modes
char* PS_TextureModes[] = 
{
	"PS_TEXTUREMODES_NONE",						// 0x00
    "PS_TEXTUREMODES_PROJECT2D",				// 0x01
    "PS_TEXTUREMODES_PROJECT3D",				// 0x02
    "PS_TEXTUREMODES_CUBEMAP",					// 0x03
    "PS_TEXTUREMODES_PASSTHRU",					// 0x04
    "PS_TEXTUREMODES_CLIPPLANE",				// 0x05
    "PS_TEXTUREMODES_BUMPENVMAP",				// 0x06
    "PS_TEXTUREMODES_BUMPENVMAP_LUM",			// 0x07
    "PS_TEXTUREMODES_BRDF",						// 0x08
    "PS_TEXTUREMODES_DOT_ST",					// 0x09
    "PS_TEXTUREMODES_DOT_ZW",					// 0x0A
    "PS_TEXTUREMODES_DOT_RFLCT_DIFF",			// 0x0B
    "PS_TEXTUREMODES_DOT_RFLCT_SPEC",			// 0x0C
    "PS_TEXTUREMODES_DOT_STR_3D",				// 0x0D
    "PS_TEXTUREMODES_DOT_STR_CUBE",				// 0x0E
    "PS_TEXTUREMODES_DPNDNT_AR",				// 0x0F
    "PS_TEXTUREMODES_DPNDNT_GB",				// 0x10
    "PS_TEXTUREMODES_DOTPRODUCT",				// 0x11
    "PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST",		// 0x12
	"???",										// 0x13
	"???",										// 0x14
	"???",										// 0x15
	"???",										// 0x16
	"???",										// 0x17
	"???",										// 0x18
	"???",										// 0x19
	"???",										// 0x1A
	"???",										// 0x1B
	"???",										// 0x1C
	"???",										// 0x1D
	"???",										// 0x1E
	"???",										// 0x1F
};

// PS DotMapping
char* PS_DotMapping[] = 
{                            
    "PS_DOTMAPPING_ZERO_TO_ONE",		// 0x00
    "PS_DOTMAPPING_MINUS1_TO_1_D3D",	// 0x01
    "PS_DOTMAPPING_MINUS1_TO_1_GL",		// 0x02
    "PS_DOTMAPPING_MINUS1_TO_1",		// 0x03
    "PS_DOTMAPPING_HILO_1",				// 0x04
    "PS_DOTMAPPING_HILO_HEMISPHERE",	// 0x07
};

// PS CompareMode
char* PS_CompareMode[] = 
{
    "PS_COMPAREMODE_S_LT", // 0x00L
    "PS_COMPAREMODE_S_GE", // 0x01L

    "PS_COMPAREMODE_T_LT", // 0x00L
    "PS_COMPAREMODE_T_GE", // 0x02L

    "PS_COMPAREMODE_R_LT", // 0x00L
    "PS_COMPAREMODE_R_GE", // 0x04L

    "PS_COMPAREMODE_Q_LT", // 0x00L
    "PS_COMPAREMODE_Q_GE", // 0x08L
};

// PS CombinerCountFlags
char* PS_CombinerCountFlags[] = 
{
    "PS_COMBINERCOUNT_MUX_LSB",		// 0x0000L, // mux on r0.a lsb
    "PS_COMBINERCOUNT_MUX_MSB",		// 0x0001L, // mux on r0.a msb

    "PS_COMBINERCOUNT_SAME_C0",		// 0x0000L, // c0 same in each stage
    "PS_COMBINERCOUNT_UNIQUE_C0",	// 0x0010L, // c0 unique in each stage

    "PS_COMBINERCOUNT_SAME_C1",		// 0x0000L, // c1 same in each stage
    "PS_COMBINERCOUNT_UNIQUE_C1",	// 0x0100L  // c1 unique in each stage
};

// PS InputMapping
char* PS_InputMapping[] = 
{
    "PS_INPUTMAPPING_UNSIGNED_IDENTITY",	// 0x00L, // max(0,x)         OK for final combiner
    "PS_INPUTMAPPING_UNSIGNED_INVERT",		// 0x20L, // 1 - max(0,x)     OK for final combiner
    "PS_INPUTMAPPING_EXPAND_NORMAL",		// 0x40L, // 2*max(0,x) - 1   invalid for final combiner
    "PS_INPUTMAPPING_EXPAND_NEGATE",		// 0x60L, // 1 - 2*max(0,x)   invalid for final combiner
    "PS_INPUTMAPPING_HALFBIAS_NORMAL",		// 0x80L, // max(0,x) - 1/2   invalid for final combiner
    "PS_INPUTMAPPING_HALFBIAS_NEGATE",		// 0xa0L, // 1/2 - max(0,x)   invalid for final combiner
    "PS_INPUTMAPPING_SIGNED_IDENTITY",		// 0xc0L, // x                invalid for final combiner
    "PS_INPUTMAPPING_SIGNED_NEGATE",		// 0xe0L, // -x               invalid for final combiner
};

// PS Register
char* PS_Register[] = 
{
    "PS_REGISTER_ZERO",		// 0x00L, // r
    "PS_REGISTER_DISCARD",	// 0x00L, // w
    "PS_REGISTER_C0",		// 0x01L, // r
    "PS_REGISTER_C1",		// 0x02L, // r
    "PS_REGISTER_FOG",		// 0x03L, // r
    "PS_REGISTER_V0",       // 0x04L, // r/w
    "PS_REGISTER_V1",       // 0x05L, // r/w
    "PS_REGISTER_T0",       // 0x08L, // r/w
    "PS_REGISTER_T1",       // 0x09L, // r/w
    "PS_REGISTER_T2",       // 0x0aL, // r/w
    "PS_REGISTER_T3",       // 0x0bL, // r/w
    "PS_REGISTER_R0",       // 0x0cL, // r/w
    "PS_REGISTER_R1",       // 0x0dL, // r/w
    "PS_REGISTER_V1R0_SUM", // 0x0eL, // r
    "PS_REGISTER_EF_PROD",  // 0x0fL, // r

	"PS_REGISTER_ONE",				// PS_REGISTER_ZERO | PS_INPUTMAPPING_UNSIGNED_INVERT, // OK for final combiner
    "PS_REGISTER_NEGATIVE_ONE",		// PS_REGISTER_ZERO | PS_INPUTMAPPING_EXPAND_NORMAL,   // invalid for final combiner
    "PS_REGISTER_ONE_HALF",			// PS_REGISTER_ZERO | PS_INPUTMAPPING_HALFBIAS_NEGATE, // invalid for final combiner
    "PS_REGISTER_NEGATIVE_ONE_HALF",// PS_REGISTER_ZERO | PS_INPUTMAPPING_HALFBIAS_NORMAL, // invalid for final combiner
};

// PS Channel
char* PS_Channel[] = 
{
    "PS_CHANNEL_RGB",	// 0x00, // used as RGB source
    "PS_CHANNEL_BLUE",	// 0x00, // used as ALPHA source
    "PS_CHANNEL_ALPHA",	// 0x10, // used as RGB or ALPHA source
};

// PS FinalCombinerSetting
char* PS_FinalCombinerSetting[] = 
{
    "PS_FINALCOMBINERSETTING_CLAMP_SUM",		// 0x80, // V1+R0 sum clamped to [0,1]

    "PS_FINALCOMBINERSETTING_COMPLEMENT_V1",	// 0x40, // unsigned invert mapping

    "PS_FINALCOMBINERSETTING_COMPLEMENT_R0",	// 0x20, // unsigned invert mapping
};

// PS CombineOutput
char* PS_CombineOutput[] = 
{
    "PS_COMBINEROUTPUT_IDENTITY",			// 0x00L, // y = x
    "PS_COMBINEROUTPUT_BIAS",				// 0x08L, // y = x - 0.5
    "PS_COMBINEROUTPUT_SHIFTLEFT_1",		// 0x10L, // y = x*2
    "PS_COMBINEROUTPUT_SHIFTLEFT_1_BIAS",	// 0x18L, // y = (x - 0.5)*2
    "PS_COMBINEROUTPUT_SHIFTLEFT_2",		// 0x20L, // y = x*4
    "PS_COMBINEROUTPUT_SHIFTRIGHT_1",		// 0x30L, // y = x/2

    "PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA",	// 0x80L, // RGB only

    "PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA",	// 0x40L, // RGB only

    "PS_COMBINEROUTPUT_AB_MULTIPLY",		// 0x00L,
    "PS_COMBINEROUTPUT_AB_DOT_PRODUCT",		// 0x02L, // RGB only

    "PS_COMBINEROUTPUT_CD_MULTIPLY",		// 0x00L,
    "PS_COMBINEROUTPUT_CD_DOT_PRODUCT",		// 0x01L, // RGB only

    "PS_COMBINEROUTPUT_AB_CD_SUM",			// 0x00L, // 3rd output is AB+CD
    "PS_COMBINEROUTPUT_AB_CD_MUX",			// 0x04L, // 3rd output is MUX(AB,CD) based on R0.a
};

// PS GlobalFlags
char* PS_GlobalFlags[] = 
{
	"PS_GLOBALFLAGS_NO_TEXMODE_ADJUST",     // 0x0000L, // don't adjust texture modes
    "PS_GLOBALFLAGS_TEXMODE_ADJUST",        // 0x0001L, // adjust texture modes according to set texture
};


void XTL::DumpPixelShaderDefToFile( X_D3DPIXELSHADERDEF* pPSDef, const char* pszCode /*= NULL*/ )
{
	static int PshNumber = 0;	// Keep track of how many pixel shaders we've attemted to convert.
	char szPSDef[512];			

	sprintf( szPSDef, "PSDef%.03d.txt", PshNumber++ );

	FILE* out = fopen( szPSDef, "w" );
	if( out )
	{
		fprintf( out, "PSAphaInputs[8]              = 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X\n"
					  "PSFinalCombinerInputsABCD    = 0x%.08X\n"
					  "PSFinalCombinerInputsEFG     = 0x%.08X\n"
					  "PSConstant0[8]               = 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X\n"
					  "PSConstant1[8]               = 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X\n"
					  "PSAlphaOutputs[8]            = 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X\n"
					  "PSRGBInputs[8]               = 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X\n"
					  "PSCompareMode                = 0x%.08X\n"
					  "PSFinalCombinerConstant0     = 0x%.08X\n"
					  "PSFinalCombinerConstant1     = 0x%.08X\n"
					  "PSRGBOutputs[8]              = 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X\n"
					  "PSCombinerCount              = 0x%.08X\n"
					  "PSTextureModes               = 0x%.08X\n"
					  "PSDotMapping                 = 0x%.08X\n"
					  "PSInputTexture               = 0x%.08X\n"
					  "PSC0Mapping                  = 0x%.08X\n"
					  "PSC1Mapping                  = 0x%.08X\n"
					  "PSFinalCombinerConstants     = 0x%.08X\n",
					  pPSDef->PSAlphaInputs[0], pPSDef->PSAlphaInputs[1], pPSDef->PSAlphaInputs[2], pPSDef->PSAlphaInputs[3],
					  pPSDef->PSAlphaInputs[4], pPSDef->PSAlphaInputs[5], pPSDef->PSAlphaInputs[6], pPSDef->PSAlphaInputs[7],
					  pPSDef->PSFinalCombinerInputsABCD,
					  pPSDef->PSFinalCombinerInputsEFG,
					  pPSDef->PSConstant0[0], pPSDef->PSConstant0[1], pPSDef->PSConstant0[2], pPSDef->PSConstant0[3], 
					  pPSDef->PSConstant0[4], pPSDef->PSConstant0[5], pPSDef->PSConstant0[6], pPSDef->PSConstant0[7], 
					  pPSDef->PSConstant1[0], pPSDef->PSConstant1[1], pPSDef->PSConstant1[2], pPSDef->PSConstant1[3], 
					  pPSDef->PSConstant1[4], pPSDef->PSConstant1[5], pPSDef->PSConstant1[6], pPSDef->PSConstant1[7], 
					  pPSDef->PSAlphaOutputs[0], pPSDef->PSAlphaOutputs[1], pPSDef->PSAlphaOutputs[2], pPSDef->PSAlphaOutputs[3], 
					  pPSDef->PSAlphaOutputs[4], pPSDef->PSAlphaOutputs[5], pPSDef->PSAlphaOutputs[6], pPSDef->PSAlphaOutputs[7], 
					  pPSDef->PSRGBInputs[0], pPSDef->PSRGBInputs[1], pPSDef->PSRGBInputs[2], pPSDef->PSRGBInputs[3], 
					  pPSDef->PSRGBInputs[4], pPSDef->PSRGBInputs[5], pPSDef->PSRGBInputs[6], pPSDef->PSRGBInputs[7], 
					  pPSDef->PSCompareMode,
					  pPSDef->PSFinalCombinerConstant0,
					  pPSDef->PSFinalCombinerConstant1,
					  pPSDef->PSRGBOutputs[0], pPSDef->PSRGBOutputs[1], pPSDef->PSRGBOutputs[2], pPSDef->PSRGBOutputs[3], 
					  pPSDef->PSRGBOutputs[4], pPSDef->PSRGBOutputs[5], pPSDef->PSRGBOutputs[6], pPSDef->PSRGBOutputs[7], 
					  pPSDef->PSCombinerCount,
					  pPSDef->PSTextureModes,
					  pPSDef->PSDotMapping,
					  pPSDef->PSInputTexture,
					  pPSDef->PSC0Mapping,
					  pPSDef->PSC1Mapping,
					  pPSDef->PSFinalCombinerConstants );
		if (pszCode)
		{
			fprintf(out, "\n\n%s\n", pszCode);
		}

		fclose( out );
	}
}

// print relevant contents to the debug console
void XTL::PrintPixelShaderDefContents( X_D3DPIXELSHADERDEF* pPSDef )
{
	// Show the contents to the user
	if( pPSDef )
	{
		DbgPshPrintf( "\n-----PixelShader Def Contents-----\n" );

		if( pPSDef->PSTextureModes )
		{
			DWORD dwPSTexMode0 = ( pPSDef->PSTextureModes >> 0 ) & 0x1F;
			DWORD dwPSTexMode1 = ( pPSDef->PSTextureModes >> 5 ) & 0x1F;
			DWORD dwPSTexMode2 = ( pPSDef->PSTextureModes >> 10 ) & 0x1F;
			DWORD dwPSTexMode3 = ( pPSDef->PSTextureModes >> 15 ) & 0x1F;

			DbgPshPrintf( "PSTextureModes ->\n" );
			DbgPshPrintf( "Stage 0: %s\n", PS_TextureModes[dwPSTexMode0] );
			DbgPshPrintf( "Stage 1: %s\n", PS_TextureModes[dwPSTexMode1] );
			DbgPshPrintf( "Stage 2: %s\n", PS_TextureModes[dwPSTexMode2] );
			DbgPshPrintf( "Stage 3: %s\n", PS_TextureModes[dwPSTexMode3] );
		}

		if( pPSDef->PSDotMapping )
		{
			DWORD dwPSDMStage1 = ( pPSDef->PSDotMapping >> 0 ) & 0x7;
			DWORD dwPSDMStage2 = ( pPSDef->PSDotMapping >> 4 ) & 0x7;
			DWORD dwPSDMStage3 = ( pPSDef->PSDotMapping >> 8 ) & 0x7;

			DbgPshPrintf( "PSDotMapping ->\n" );
			DbgPshPrintf( "Stage 1: %s\n", PS_DotMapping[dwPSDMStage1] );
			DbgPshPrintf( "Stage 2: %s\n", PS_DotMapping[dwPSDMStage2] );
			DbgPshPrintf( "Stage 3: %s\n", PS_DotMapping[dwPSDMStage3] );
		}

		if( pPSDef->PSCompareMode )
		{
			DWORD dwPSCMStage0 = ( pPSDef->PSCompareMode >> 0 ) & 0xF;
			DWORD dwPSCMStage1 = ( pPSDef->PSCompareMode >> 4 ) & 0xF;
			DWORD dwPSCMStage2 = ( pPSDef->PSCompareMode >> 8 ) & 0xF;
			DWORD dwPSCMStage3 = ( pPSDef->PSCompareMode >> 12 ) & 0xF;

			DbgPshPrintf( "PSCompareMode ->\n" );
			DbgPshPrintf( "Stage 0: %s\n", PS_TextureModes[dwPSCMStage0 == 0 ? 0 : 1] );
			DbgPshPrintf( "Stage 1: %s\n", PS_TextureModes[dwPSCMStage1 == 0 ? 2 : 3] );
			DbgPshPrintf( "Stage 2: %s\n", PS_TextureModes[dwPSCMStage2 == 0 ? 4 : 5] );
			DbgPshPrintf( "Stage 3: %s\n", PS_TextureModes[dwPSCMStage3 == 0 ? 6 : 7] );
		}

		if( pPSDef->PSInputTexture )
		{
			DWORD dwPSITStage2 = ( pPSDef->PSInputTexture >> 16 ) & 0x1;
			DWORD dwPSITStage3 = ( pPSDef->PSInputTexture >> 20 ) & 0x3;

			DbgPshPrintf( "PSInputTexture ->\n" );
			DbgPshPrintf( "Stage 2: %s\n", PS_TextureModes[dwPSITStage2] );
			DbgPshPrintf( "Stage 3: %s\n", PS_TextureModes[dwPSITStage3] );
		}

		if( pPSDef->PSCombinerCount )
		{
			DWORD dwPSCCNumCombiners = ( pPSDef->PSCombinerCount >> 0 ) & 0xF;
			DWORD dwPSCCMux = ( pPSDef->PSCombinerCount >> 8 ) & 0x1;
			DWORD dwPSCCC0 = ( pPSDef->PSCombinerCount >> 12 ) & 0x1;
			DWORD dwPSCCC1 = ( pPSDef->PSCombinerCount >> 16 ) & 0x1;

			DbgPshPrintf( "PSCombinerCount ->\n" );
			DbgPshPrintf( "Combiners: %d\n", dwPSCCNumCombiners );
			DbgPshPrintf( "Mux:       %s\n", PS_CombinerCountFlags[dwPSCCMux] );
			DbgPshPrintf( "C0:        %s\n", PS_CombinerCountFlags[dwPSCCC0 == 0 ? 2 : 3] );
			DbgPshPrintf( "C1:        %s\n", PS_CombinerCountFlags[dwPSCCC1 == 0 ? 4 : 5] );
		}

		/*for( int i = 0; i > 7; i++ )
		{
			if( pPSDef->PSRGBInputs[i] )
			{*/
	}
}

HRESULT XTL::EmuRecompilePshDef( X_D3DPIXELSHADERDEF* pPSDef, LPD3DXBUFFER* ppRecompiled )
{
	char szPshString[2048];		// I'm sure that's big enough...
	
	// Dump the contents of the PixelShader def
#ifdef _DEBUG_TRACK_PS
//	DumpPixelShaderDefToFile( pPSDef );

	// Azurik like to create and destroy the same shader every frame! O_o
//	PrintPixelShaderDefContents( pPSDef );
#endif

	// First things first, set the pixel shader version
	// TODO: ps.1.1 might be a better idea...
	sprintf(szPshString, "%s", "ps.1.0\n");

	// Handle Texture declarations
	if(pPSDef->PSTextureModes != 0)
	{
	}

	return S_OK;
}
