// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuD3D8->PixelShader.cpp
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
#define _XBOXKRNL_DEFEXTRN_

#include "CxbxKrnl/CxbxKrnl.h"
#include "CxbxKrnl/Emu.h"
#include "CxbxKrnl/EmuFS.h"
#include "CxbxKrnl/EmuAlloc.h"
#include "CxbxKrnl/EmuXTL.h"


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


void XTL::DumpPixelShaderDefToFile( X_D3DPIXELSHADERDEF* pPSDef )
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
	DumpPixelShaderDefToFile( pPSDef );

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
