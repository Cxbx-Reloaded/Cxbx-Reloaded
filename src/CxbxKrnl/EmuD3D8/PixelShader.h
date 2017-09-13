// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;"````"    `;;;,  .,;;  ;;;"";;"   `;;;,  .,;;
// *  [[[             "[[,,[["   [[[__[[\.    "[[,,[["
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuD3D8->PixelShader.h
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
#ifndef PIXELSHADER_H
#define PIXELSHADER_H

#pragma once

#include "Cxbx.h"

/*---------------------------------------------------------------------------*/
/*  Texture configuration - The following members of the D3DPixelShaderDef   */
/*  structure define the addressing modes of each of the four texture stages:*/
/*      PSTextureModes                                                       */
/*      PSDotMapping                                                         */
/*      PSInputTexture                                                       */
/*      PSCompareMode                                                        */
/*---------------------------------------------------------------------------*/

// =========================================================================================================
// PSTextureModes
// --------.--------.--------.---xxxxx stage0
// --------.--------.------xx.xxx----- stage1
// --------.--------.-xxxxx--.-------- stage2
// --------.----xxxx.x-------.-------- stage3

#define PS_TEXTUREMODES(t0,t1,t2,t3) (((t3)<<15)|((t2)<<10)|((t1)<<5)|(t0))

/*
Texture modes:
NONE           :stage inactive
PROJECT2D      :argb = texture(s/q, t/q)
PROJECT3D      :argb = texture(s/q, t/q, r/q)
CUBEMAP        :argb = cubemap(s,t,r)
PASSTHRU       :argb = s,t,r,q
CLIPPLANE      :pixel not drawn if s,t,r, or q < 0.  PSCompareMode affects comparison
BUMPENVMAP     :argb=texture(s+mat00*src.r+mat01*src.g,
                             t+mat10*src.r+mat11*src.g)
                mat00 set via D3DTSS_BUMPENVMAT00, etc.
BUMPENVMAP_LUM :argb=texture(s+mat00*src.r+mat01*src.g,
                             t+mat10*src.r+mat11*src.g);
                rgb *= (lum_scale*src.b + lum_bias); (a is not affected)
                lum_scale set by D3DTSS_BUMPENVLSCALE
                lum_bias set by D3DTSS_BUMPENVLOFFSET
                mat00 set via D3DTSS_BUMPENVMAT00, etc.
BRDF           :argb = texture(eyeSigma, lightSigma, dPhi)
                       eyeSigma = Sigma of eye vector in spherical coordinates
                       lightSigma = Sigma of light vector in spherical coordinates
                       dPhi = Phi of eye - Phi of light
DOT_ST         :argb = texture(<DotResult of stage-1>, (s,t,r).(src.r,src.g,src.b))
DOT_ZW         :frag depth = (<DotResult of stage-1>/((s,t,r).(src.r,src.g,src.b))
DOT_RFLCT_DIFF :n = (<DotResult of stage-1>,(s,t,r).(src.r,src.g,src.b),<DotResult of stage+1>)
                argb = cubemap(n)
DOT_RFLCT_SPEC :n = (<DotResult of stage-2>,<DotResult of stage-1>,(s,t,r).(src.r,src.g,src.b))
                r = 2*n*(n.e)/(n.n) - e where e is eye vector built from q coord of each stage
                argb = cubemap(r)
DOT_STR_3D     :argb=texture((<DotResult of stage-2>,<DotResult of stage-1>,(s,t,r).(src.r,src.g,src.b)))
DOT_STR_CUBE   :argb=cubemap((<DotResult of stage-2>,<DotResult of stage-1>,(s,t,r).(src.r,src.g,src.b)))
DEPENDENT_AR   :argb = texture(src.a, src.r)
DEPENDENT_GB   :argb = texture(src.g, src.b)
DOTPRODUCT     :argb = (s,t,r).(src.r,src.g,src.b)
DOT_RFLCT_SPEC_CONST :n = (<DotResult of stage-2>,<DotResult of stage-1>,(s,t,r).(src.r,src.g,src.b))
                r = 2*n*(n.e)/(n.n) - e where e is eye vector set via SetEyeVector()
                argb = cubemap(r)
*/

enum PS_TEXTUREMODES
{                                 // valid in stage 0 1 2 3
    PS_TEXTUREMODES_NONE=                 0x00L, // * * * *
    PS_TEXTUREMODES_PROJECT2D=            0x01L, // * * * *
    PS_TEXTUREMODES_PROJECT3D=            0x02L, // * * * *
    PS_TEXTUREMODES_CUBEMAP=              0x03L, // * * * *
    PS_TEXTUREMODES_PASSTHRU=             0x04L, // * * * *
    PS_TEXTUREMODES_CLIPPLANE=            0x05L, // * * * *
    PS_TEXTUREMODES_BUMPENVMAP=           0x06L, // - * * *
    PS_TEXTUREMODES_BUMPENVMAP_LUM=       0x07L, // - * * *
    PS_TEXTUREMODES_BRDF=                 0x08L, // - - * *
    PS_TEXTUREMODES_DOT_ST=               0x09L, // - - * *
    PS_TEXTUREMODES_DOT_ZW=               0x0aL, // - - * *
    PS_TEXTUREMODES_DOT_RFLCT_DIFF=       0x0bL, // - - * -
    PS_TEXTUREMODES_DOT_RFLCT_SPEC=       0x0cL, // - - - *
    PS_TEXTUREMODES_DOT_STR_3D=           0x0dL, // - - - *
    PS_TEXTUREMODES_DOT_STR_CUBE=         0x0eL, // - - - *
    PS_TEXTUREMODES_DPNDNT_AR=            0x0fL, // - * * *
    PS_TEXTUREMODES_DPNDNT_GB=            0x10L, // - * * *
    PS_TEXTUREMODES_DOTPRODUCT=           0x11L, // - * * -
    PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST= 0x12L, // - - - *
    // 0x13-0x1f reserved
};


// =========================================================================================================
// PSDotMapping
// --------.--------.--------.-----xxx // stage1
// --------.--------.--------.-xxx---- // stage2
// --------.--------.-----xxx.-------- // stage3

#define PS_DOTMAPPING(t0,t1,t2,t3) (((t3)<<8)|((t2)<<4)|(t1))

// Mappings:
// ZERO_TO_ONE         :rgb->(r,g,b): 0x0=>0.0, 0xff=>1.0
// MINUS1_TO_1_D3D     :rgb->(r,g,b): 0x0=>-128/127, 0x01=>-1.0, 0x80=>0.0, 0xff=>1.0
// MINUS1_TO_1_GL      :rgb->(r,g,b): 0x80=>-1.0, 0x7f=>1.0
// MINUS1_TO_1         :rgb->(r,g,b): 0x80=>-128/127, 0x81=>-1.0, 0x0=>0.0, 0x7f=>1.0
// HILO_1              :HL->(H,L,1.0): 0x0000=>0.0, 0xffff=>1.0
// HILO_HEMISPHERE     :HL->(H,L,sqrt(1-H*H-L*L)): 0x8001=>-1.0, 0x0=>0.0, 0x7fff=>1.0, 0x8000=>-32768/32767

enum PS_DOTMAPPING
{                              // valid in stage 0 1 2 3
    PS_DOTMAPPING_ZERO_TO_ONE=         0x00L, // - * * *
    PS_DOTMAPPING_MINUS1_TO_1_D3D=     0x01L, // - * * *
    PS_DOTMAPPING_MINUS1_TO_1_GL=      0x02L, // - * * *
    PS_DOTMAPPING_MINUS1_TO_1=         0x03L, // - * * *
    PS_DOTMAPPING_HILO_1=              0x04L, // - * * *
    PS_DOTMAPPING_HILO_HEMISPHERE=     0x07L, // - * * *
};

// =========================================================================================================
// PSCompareMode
// --------.--------.--------.----xxxx // stage0
// --------.--------.--------.xxxx---- // stage1
// --------.--------.----xxxx.-------- // stage2
// --------.--------.xxxx----.-------- // stage3

#define PS_COMPAREMODE(t0,t1,t2,t3) (((t3)<<12)|((t2)<<8)|((t1)<<4)|(t0))

enum PS_COMPAREMODE
{
    PS_COMPAREMODE_S_LT= 0x00L,
    PS_COMPAREMODE_S_GE= 0x01L,

    PS_COMPAREMODE_T_LT= 0x00L,
    PS_COMPAREMODE_T_GE= 0x02L,

    PS_COMPAREMODE_R_LT= 0x00L,
    PS_COMPAREMODE_R_GE= 0x04L,

    PS_COMPAREMODE_Q_LT= 0x00L,
    PS_COMPAREMODE_Q_GE= 0x08L,
};

// =========================================================================================================
// PSInputTexture
// --------.-------x.--------.-------- // stage2
// --------.--xx----.--------.-------- // stage3
//
// Selects the other texture to use as an input in the following texture modes:
// DOT_ST, DOT_STR_3D, DOT_STR_CUBE, DOT_ZW, DOT_RFLCT_SPEC,
// DOT_RFLCT_DIFF, DPNDNT_AR, DPNDNT_GB, BUMPENVMAP,
// BUMPENVMAP_LUM, DOT_PRODUCT

#define PS_INPUTTEXTURE(t0,t1,t2,t3) (((t3)<<20)|((t2)<<16))


/*---------------------------------------------------------------------------------*/
/*  Color combiners - The following members of the D3DPixelShaderDef structure     */
/*  define the state for the eight stages of color combiners:                      */
/*      PSCombinerCount - Number of stages                                         */
/*      PSAlphaInputs[8] - Inputs for alpha portion of each stage                  */
/*      PSRGBInputs[8] - Inputs for RGB portion of each stage                      */
/*      PSConstant0[8] - Constant 0 for each stage                                 */
/*      PSConstant1[8] - Constant 1 for each stage                                 */
/*      PSFinalCombinerConstant0 - Constant 0 for final combiner                   */
/*      PSFinalCombinerConstant1 - Constant 1 for final combiner                   */
/*      PSAlphaOutputs[8] - Outputs for alpha portion of each stage                */
/*      PSRGBOutputs[8] - Outputs for RGB portion of each stage                    */
/*---------------------------------------------------------------------------------*/


// =========================================================================================================
// PSCombinerCount
// --------.--------.--------.----xxxx // number of combiners (1-8)
// --------.--------.-------x.-------- // mux bit (0= LSB, 1= MSB)
// --------.--------.---x----.-------- // separate C0
// --------.-------x.--------.-------- // separate C1

#define PS_COMBINERCOUNT(count, flags) (((flags)<<8)|(count))
// count is 1-8, flags contains one or more values from PS_COMBINERCOUNTFLAGS

enum PS_COMBINERCOUNTFLAGS
{
    PS_COMBINERCOUNT_MUX_LSB=     0x0000L, // mux on r0.a lsb
    PS_COMBINERCOUNT_MUX_MSB=     0x0001L, // mux on r0.a msb

    PS_COMBINERCOUNT_SAME_C0=     0x0000L, // c0 same in each stage
    PS_COMBINERCOUNT_UNIQUE_C0=   0x0010L, // c0 unique in each stage

    PS_COMBINERCOUNT_SAME_C1=     0x0000L, // c1 same in each stage
    PS_COMBINERCOUNT_UNIQUE_C1=   0x0100L  // c1 unique in each stage
};


// =========================================================================================================
// PSRGBInputs[0-7]
// PSAlphaInputs[0-7]
// PSFinalCombinerInputsABCD
// PSFinalCombinerInputsEFG
// --------.--------.--------.----xxxx // D register
// --------.--------.--------.---x---- // D channel (0= RGB/BLUE, 1= ALPHA)
// --------.--------.--------.xxx----- // D input mapping
// --------.--------.----xxxx.-------- // C register
// --------.--------.---x----.-------- // C channel (0= RGB/BLUE, 1= ALPHA)
// --------.--------.xxx-----.-------- // C input mapping
// --------.----xxxx.--------.-------- // B register
// --------.---x----.--------.-------- // B channel (0= RGB/BLUE, 1= ALPHA)
// --------.xxx-----.--------.-------- // B input mapping
// ----xxxx.--------.--------.-------- // A register
// ---x----.--------.--------.-------- // A channel (0= RGB/BLUE, 1= ALPHA)
// xxx-----.--------.--------.-------- // A input mapping

// examples:
//
// shader.PSRGBInputs[3]= PS_COMBINERINPUTS(
//     PS_REGISTER_T0 | PS_INPUTMAPPING_EXPAND_NORMAL     | PS_CHANNEL_RGB,
//     PS_REGISTER_C0 | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_ALPHA,
//     PS_REGISTER_ZERO,
//     PS_REGISTER_ZERO);
//
// shader.PSFinalCombinerInputsABCD= PS_COMBINERINPUTS(
//     PS_REGISTER_T0     | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_ALPHA,
//     PS_REGISTER_ZERO   | PS_INPUTMAPPING_EXPAND_NORMAL     | PS_CHANNEL_RGB,
//     PS_REGISTER_EFPROD | PS_INPUTMAPPING_UNSIGNED_INVERT   | PS_CHANNEL_RGB,
//     PS_REGISTER_ZERO);
//
// PS_FINALCOMBINERSETTING is set in 4th field of PSFinalCombinerInputsEFG with PS_COMBINERINPUTS
// example:
//
// shader.PSFinalCombinerInputsEFG= PS_COMBINERINPUTS(
//     PS_REGISTER_R0 | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_RGB,
//     PS_REGISTER_R1 | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_RGB,
//     PS_REGISTER_R1 | PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_CHANNEL_BLUE,
//    PS_FINALCOMBINERSETTING_CLAMP_SUM | PS_FINALCOMBINERSETTING_COMPLEMENT_R0);

#define PS_COMBINERINPUTS(a,b,c,d) (((a)<<24)|((b)<<16)|((c)<<8)|(d))
// For PSFinalCombinerInputsEFG,
//     a,b,c contain a value from PS_REGISTER, PS_CHANNEL, and PS_INPUTMAPPING for input E,F, and G
//     d contains values from PS_FINALCOMBINERSETTING
// For all other inputs,
//     a,b,c,d each contain a value from PS_REGISTER, PS_CHANNEL, and PS_INPUTMAPPING

enum PS_INPUTMAPPING
{
    PS_INPUTMAPPING_UNSIGNED_IDENTITY= 0x00L, // max(0,x)         OK for final combiner
    PS_INPUTMAPPING_UNSIGNED_INVERT=   0x20L, // 1 - max(0,x)     OK for final combiner
    PS_INPUTMAPPING_EXPAND_NORMAL=     0x40L, // 2*max(0,x) - 1   invalid for final combiner
    PS_INPUTMAPPING_EXPAND_NEGATE=     0x60L, // 1 - 2*max(0,x)   invalid for final combiner
    PS_INPUTMAPPING_HALFBIAS_NORMAL=   0x80L, // max(0,x) - 1/2   invalid for final combiner
    PS_INPUTMAPPING_HALFBIAS_NEGATE=   0xa0L, // 1/2 - max(0,x)   invalid for final combiner
    PS_INPUTMAPPING_SIGNED_IDENTITY=   0xc0L, // x                invalid for final combiner
    PS_INPUTMAPPING_SIGNED_NEGATE=     0xe0L, // -x               invalid for final combiner
};

enum PS_REGISTER
{
    PS_REGISTER_ZERO=              0x00L, // r
    PS_REGISTER_DISCARD=           0x00L, // w
    PS_REGISTER_C0=                0x01L, // r
    PS_REGISTER_C1=                0x02L, // r
    PS_REGISTER_FOG=               0x03L, // r
    PS_REGISTER_V0=                0x04L, // r/w
    PS_REGISTER_V1=                0x05L, // r/w
    PS_REGISTER_T0=                0x08L, // r/w
    PS_REGISTER_T1=                0x09L, // r/w
    PS_REGISTER_T2=                0x0aL, // r/w
    PS_REGISTER_T3=                0x0bL, // r/w
    PS_REGISTER_R0=                0x0cL, // r/w
    PS_REGISTER_R1=                0x0dL, // r/w
    PS_REGISTER_V1R0_SUM=          0x0eL, // r
    PS_REGISTER_EF_PROD=           0x0fL, // r

    PS_REGISTER_ONE=               PS_REGISTER_ZERO | PS_INPUTMAPPING_UNSIGNED_INVERT, // OK for final combiner
    PS_REGISTER_NEGATIVE_ONE=      PS_REGISTER_ZERO | PS_INPUTMAPPING_EXPAND_NORMAL,   // invalid for final combiner
    PS_REGISTER_ONE_HALF=          PS_REGISTER_ZERO | PS_INPUTMAPPING_HALFBIAS_NEGATE, // invalid for final combiner
    PS_REGISTER_NEGATIVE_ONE_HALF= PS_REGISTER_ZERO | PS_INPUTMAPPING_HALFBIAS_NORMAL, // invalid for final combiner
};

// FOG ALPHA is only available in final combiner
// V1R0_SUM and EF_PROD are only available in final combiner (A,B,C,D inputs only)
// V1R0_SUM_ALPHA and EF_PROD_ALPHA are not available
// R0_ALPHA is initialized to T0_ALPHA in stage0

enum PS_CHANNEL
{
    PS_CHANNEL_RGB=   0x00, // used as RGB source
    PS_CHANNEL_BLUE=  0x00, // used as ALPHA source
    PS_CHANNEL_ALPHA= 0x10, // used as RGB or ALPHA source
};


enum PS_FINALCOMBINERSETTING
{
    PS_FINALCOMBINERSETTING_CLAMP_SUM=     0x80, // V1+R0 sum clamped to [0,1]

    PS_FINALCOMBINERSETTING_COMPLEMENT_V1= 0x40, // unsigned invert mapping

    PS_FINALCOMBINERSETTING_COMPLEMENT_R0= 0x20, // unsigned invert mapping
};

// =========================================================================================================
// PSRGBOutputs[0-7]
// PSAlphaOutputs[0-7]
// --------.--------.--------.----xxxx // CD register
// --------.--------.--------.xxxx---- // AB register
// --------.--------.----xxxx.-------- // SUM register
// --------.--------.---x----.-------- // CD output (0= multiply, 1= dot product)
// --------.--------.--x-----.-------- // AB output (0= multiply, 1= dot product)
// --------.--------.-x------.-------- // AB_CD mux/sum select (0= sum, 1= mux)
// --------.------xx.x-------.-------- // Output mapping
// --------.-----x--.--------.-------- // CD blue to alpha
// --------.----x---.--------.-------- // AB blue to alpha

#define PS_COMBINEROUTPUTS(ab,cd,mux_sum,flags) (((flags)<<12)|((mux_sum)<<8)|((ab)<<4)|(cd))
// ab,cd,mux_sum contain a value from PS_REGISTER
// flags contains values from PS_COMBINEROUTPUT

enum PS_COMBINEROUTPUT
{
    PS_COMBINEROUTPUT_IDENTITY=            0x00L, // y = x
    PS_COMBINEROUTPUT_BIAS=                0x08L, // y = x - 0.5
    PS_COMBINEROUTPUT_SHIFTLEFT_1=         0x10L, // y = x*2
    PS_COMBINEROUTPUT_SHIFTLEFT_1_BIAS=    0x18L, // y = (x - 0.5)*2
    PS_COMBINEROUTPUT_SHIFTLEFT_2=         0x20L, // y = x*4
    PS_COMBINEROUTPUT_SHIFTRIGHT_1=        0x30L, // y = x/2

    PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA=    0x80L, // RGB only

    PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA=    0x40L, // RGB only

    PS_COMBINEROUTPUT_AB_MULTIPLY=         0x00L,
    PS_COMBINEROUTPUT_AB_DOT_PRODUCT=      0x02L, // RGB only

    PS_COMBINEROUTPUT_CD_MULTIPLY=         0x00L,
    PS_COMBINEROUTPUT_CD_DOT_PRODUCT=      0x01L, // RGB only

    PS_COMBINEROUTPUT_AB_CD_SUM=           0x00L, // 3rd output is AB+CD
    PS_COMBINEROUTPUT_AB_CD_MUX=           0x04L, // 3rd output is MUX(AB,CD) based on R0.a
};

// AB_CD register output must be DISCARD if either AB_DOT_PRODUCT or CD_DOT_PRODUCT are set

// =========================================================================================================
// PSC0Mapping
// PSC1Mapping
// --------.--------.--------.----xxxx // offset of D3D constant for stage 0
// --------.--------.--------.xxxx---- // offset of D3D constant for stage 1
// --------.--------.----xxxx.-------- // offset of D3D constant for stage 2
// --------.--------.xxxx----.-------- // offset of D3D constant for stage 3
// --------.----xxxx.--------.-------- // offset of D3D constant for stage 4
// --------.xxxx----.--------.-------- // offset of D3D constant for stage 5
// ----xxxx.--------.--------.-------- // offset of D3D constant for stage 6
// xxxx----.--------.--------.-------- // offset of D3D constant for stage 7

#define PS_CONSTANTMAPPING(s0,s1,s2,s3,s4,s5,s6,s7) \
     (((DWORD)(s0)&0xf)<< 0) | (((DWORD)(s1)&0xf)<< 4) | \
     (((DWORD)(s2)&0xf)<< 8) | (((DWORD)(s3)&0xf)<<12) | \
     (((DWORD)(s4)&0xf)<<16) | (((DWORD)(s5)&0xf)<<20) | \
     (((DWORD)(s6)&0xf)<<24) | (((DWORD)(s7)&0xf)<<28)
// s0-s7 contain the offset of the D3D constant that corresponds to the
// c0 or c1 constant in stages 0 through 7.  These mappings are only used in
// SetPixelShaderConstant().

// =========================================================================================================
// PSFinalCombinerConstants
// --------.--------.--------.----xxxx // offset of D3D constant for C0
// --------.--------.--------.xxxx---- // offset of D3D constant for C1
// --------.--------.-------x.-------- // Adjust texture flag

#define PS_FINALCOMBINERCONSTANTS(c0,c1,flags) (((DWORD)(flags) << 8) | ((DWORD)(c0)&0xf)<< 0) | (((DWORD)(c1)&0xf)<< 4)
// c0 and c1 contain the offset of the D3D constant that corresponds to the
// constants in the final combiner.  These mappings are only used in
// SetPixelShaderConstant().  Flags contains values from PS_GLOBALFLAGS

enum PS_GLOBALFLAGS
{
    // if this flag is set, the texture mode for each texture stage is adjusted as follows:
    //     if set texture is a cubemap,
    //         change PS_TEXTUREMODES_PROJECT2D to PS_TEXTUREMODES_CUBEMAP
    //         change PS_TEXTUREMODES_PROJECT3D to PS_TEXTUREMODES_CUBEMAP
    //         change PS_TEXTUREMODES_DOT_STR_3D to PS_TEXTUREMODES_DOT_STR_CUBE
    //     if set texture is a volume texture,
    //         change PS_TEXTUREMODES_PROJECT2D to PS_TEXTUREMODES_PROJECT3D
    //         change PS_TEXTUREMODES_CUBEMAP to PS_TEXTUREMODES_PROJECT3D
    //         change PS_TEXTUREMODES_DOT_STR_CUBE to PS_TEXTUREMODES_DOT_STR_3D
    //     if set texture is neither cubemap or volume texture,
    //         change PS_TEXTUREMODES_PROJECT3D to PS_TEXTUREMODES_PROJECT2D
    //         change PS_TEXTUREMODES_CUBEMAP to PS_TEXTUREMODES_PROJECT2D

    PS_GLOBALFLAGS_NO_TEXMODE_ADJUST=     0x0000L, // don"t adjust texture modes
    PS_GLOBALFLAGS_TEXMODE_ADJUST=        0x0001L, // adjust texture modes according to set texture
};

#if 0 // Dxbx translation

enum PSH_OPCODE
{
	PO_COMMENT,
	PO_PS,
	PO_DEF,
#ifdef CXBX_USE_PS_2_0
	PO_DCL,
#endif
	PO_TEX,
	PO_TEXBEM,
	PO_TEXBEML,
	PO_TEXBRDF, // Xbox ext.
	PO_TEXCOORD,
	PO_TEXKILL,
	PO_TEXDP3, // Note : ps.1.3 only
	PO_TEXDP3TEX, // Note : ps.1.3 only
	PO_TEXM3X2TEX,
	PO_TEXM3X2DEPTH, // Note : requires ps.1.3 and a preceding texm3x2pad
	PO_TEXM3X3DIFF, // Xbox ext.
	PO_TEXM3X3VSPEC,
	PO_TEXM3X3TEX, // Note : Uses a cube texture
	PO_TEXREG2AR,
	PO_TEXREG2GB,
	PO_TEXM3X2PAD, // Note : Must be combined with texm3x2tex or texm3x2depth
	PO_TEXM3X3PAD,
	PO_TEXM3X3SPEC, // NOTE : NEEDS 3 ARGUMENTS!
	// Direct3D8 arithmetic instructions :
	PO_ADD,
	PO_CMP,
	PO_CND,
	PO_DP3,  // dp3 d, s1,s2                : d=s0 dot s1 (replicated to all channels, .rgb=color only, .a=color+alpha)
	PO_DP4,  // dp3 d, s1,s2                : d.r=d.g=d.b=d.a=(s1.r*s2.r)+(s1.g*s2.g)+(s1.b*s2.b)+(s1.a*s2.a)
	PO_LRP,
	PO_MAD,
	PO_MOV,
	PO_MUL,
	PO_NOP,
	PO_SUB,
	// Xbox1 opcodes :
	PO_XMMA,
	PO_XMMC,
	PO_XDM,
	PO_XDD,
	PO_XFC
};

const struct { char *mn; int _Out; int _In; char *note; } PSH_OPCODE_DEFS[/*PSH_OPCODE*/] = {
	// Pixel shader header opcodes (must be specified in this order) :
	{/* PO_COMMENT */  /*mn:*/";", /*_Out: */ 0, /*_In: */ 0, /*note:*/""}, //
	{/* PO_PS */  /*mn:*/"ps",   /*_Out: */ 0, /*_In: */ 0, /*note:*/""}, // Must occur once, Xbox needs an x prefix {xps}, Native needs a 1.3 suffix {ps.1.3}
	{/* PO_DEF */ /*mn:*/"def",  /*_Out: */ 1, /*_In: */ 4, /*note:*/""}, // Output must be a PARAM_C, arguments must be 4 floats [0.00f .. 1.00f]
#ifdef CXBX_USE_PS_2_0
	{/* PO_DCL */ /*mn:*/"dcl",  /*_Out: */ 1, /*_In: */ 0, /*note:*/""},
#endif
	{/* PO_TEX */ /*mn:*/"tex",  /*_Out: */ 1, /*_In: */ 0, /*note:*/""},
	{/* PO_TEXBEM */ /*mn:*/"texbem",  /*_Out: */ 1, /*_In: */ 1, /*note:*/""},
	{/* PO_TEXBEML */ /*mn:*/"texbeml",  /*_Out: */ 1, /*_In: */ 1, /*note:*/""},
	{/* PO_TEXBRDF */ /*mn:*/"texbrdf",  /*_Out: */ 1, /*_In: */ 1, /*note:*/""}, // /*note: */ Not supported by Direct3D8 ?
	{/* PO_TEXCOORD */ /*mn:*/"texcoord",  /*_Out: */ 1, /*_In: */ 0, /*note:*/""},
	{/* PO_TEXKILL */ /*mn:*/"texkill",  /*_Out: */ 1, /*_In: */ 0, /*note:*/""},
	{/* PO_TEXDP3 */ /*mn:*/"texdp3",  /*_Out: */ 1, /*_In: */ 1, /*note:*/""},
	{/* PO_TEXDP3TEX */ /*mn:*/"texdp3tex",  /*_Out: */ 1, /*_In: */ 1, /*note:*/""},
	{/* PO_TEXM3X2TEX */ /*mn:*/"texm3x2tex",  /*_Out: */ 1, /*_In: */ 1, /*note:*/""},
	{/* PO_TEXM3X2DEPTH */ /*mn:*/"texm3x2depth",  /*_Out: */ 1, /*_In: */ 1, /*note:*/""}, // /*note: */ requires ps.1.3 and a preceding texm3x2pad
	{/* PO_TEXM3X3DIFF */ /*mn:*/"texm3x3diff",  /*_Out: */ 1, /*_In: */ 1, /*note:*/""}, // /*note: */ Not supported by Direct3D8 ?
	{/* PO_TEXM3X3VSPEC */ /*mn:*/"texm3x3vspec",  /*_Out: */ 1, /*_In: */ 1, /*note:*/""},
	{/* PO_TEXM3X3TEX */ /*mn:*/"texm3x3tex",  /*_Out: */ 1, /*_In: */ 1, /*note:*/""}, // /*note: */ Uses a cube texture
	{/* PO_TEXREG2AR */ /*mn:*/"texreg2ar",  /*_Out: */ 1, /*_In: */ 1, /*note:*/""},
	{/* PO_TEXREG2GB */ /*mn:*/"texreg2gb",  /*_Out: */ 1, /*_In: */ 1, /*note:*/""},
	{/* PO_TEXM3X2PAD */ /*mn:*/"texm3x2pad",  /*_Out: */ 1, /*_In: */ 1, /*note:*/""},
	{/* PO_TEXM3X3PAD */ /*mn:*/"texm3x3pad",  /*_Out: */ 1, /*_In: */ 1, /*note:*/""},
	{/* PO_TEXM3X3SPEC */ /*mn:*/"texm3x3spec",  /*_Out: */ 1, /*_In: */ 2, /*note:*/""},
	// Arithmetic opcodes :
	{/* PO_ADD */ /*mn:*/"add",  /*_Out: */ 1, /*_In: */ 2, /*note:*/"d0=s0+s1"},
	{/* PO_CMP */ /*mn:*/"cmp",  /*_Out: */ 1, /*_In: */ 3, /*note:*/"d0={s0>=0?s1:s2}"},
	{/* PO_CND */ /*mn:*/"cnd",  /*_Out: */ 1, /*_In: */ 3, /*note:*/"d1={s0.a>0.5?s1:s2}"}, // 1st input must be "r0.a"
	{/* PO_DP3 */ /*mn:*/"dp3",  /*_Out: */ 1, /*_In: */ 2, /*note:*/"d0=s0 dot3 s1"},
	{/* PO_DP4 */ /*mn:*/"dp4",  /*_Out: */ 1, /*_In: */ 2, /*note:*/"d0=s0 dot4 s1"},
	{/* PO_LRP */ /*mn:*/"lrp",  /*_Out: */ 1, /*_In: */ 3, /*note:*/"d0=s0*s1+{1-s0}*s2=s0*{s1-s2}+s2"},
	{/* PO_MAD */ /*mn:*/"mad",  /*_Out: */ 1, /*_In: */ 3, /*note:*/"d0=s0*s1+s2"},
	{/* PO_MOV */ /*mn:*/"mov",  /*_Out: */ 1, /*_In: */ 1, /*note:*/"d0=s0"},
	{/* PO_MUL */ /*mn:*/"mul",  /*_Out: */ 1, /*_In: */ 2, /*note:*/"d0=s0*s1"},
	{/* PO_NOP */ /*mn:*/"nop",  /*_Out: */ 0, /*_In: */ 0, /*note:*/""},
	{/* PO_SUB */ /*mn:*/"sub",  /*_Out: */ 1, /*_In: */ 2, /*note:*/"d0=s0-s1"},
	// Xbox-only {NV2A} opcodes :
	{/* PO_XMMA */  /*mn:*/"xmma", /*_Out: */ 3, /*_In: */ 4, /*note:*/"d0=s0*s1, d1=s2*s3, d2={s0*s1}+{s2*s3}"},
	{/* PO_XMMC */  /*mn:*/"xmmc", /*_Out: */ 3, /*_In: */ 4, /*note:*/"d0=s0*s1, d1=s2*s3, d2={r0.a>0.5}?{s0*s1}:{s2*s3}"},
	{/* PO_XDM */ /*mn:*/"xdm",  /*_Out: */ 2, /*_In: */ 4, /*note:*/"d0=s0 dot s1, d1=s2*s3"},
	{/* PO_XDD */ /*mn:*/"xdd",  /*_Out: */ 2, /*_In: */ 4, /*note:*/"d0=s0 dot s1, d1=s2 dot s3"},
	{/* PO_XFC */ /*mn:*/"xfc",  /*_Out: */ 0, /*_In: */ 7, /*note:*/"r0.rgb=s0*s1+{1-s0}*s2+s3, r0.a=s6.a, prod=s4*s5, sum=r0+v1"}
};

enum PSH_ARGUMENT_TYPE
{
	PARAM_VALUE,      // Xbox only; Numberic constants used in Xbox-only opcodes
	PARAM_DISCARD,    // Xbox only;
	PARAM_FOG,        // Final combiner only; Read-only register fog register
	PARAM_V1R0_SUM,   // Final combiner only; Read-only register that contains the result of V1+R0
	PARAM_EF_PROD,    // Final combiner only; Read-only register that contains the result of final combiner parameters E * F
	PARAM_R,          // Temporary registers (unassigned except r0.a, which on NV2A is initially set to t0.a)
	PARAM_T,          // Textures
	PARAM_V,          // Vertex colors
	PARAM_C           // Constant registers, set by def opcodes or SetPixelShaderConstant
};

const char *PSH_ARGUMENT_TYPE_Str[/*PSH_ARGUMENT_TYPE*/] = {
	//  Prefix        #  r/w   Input?  Output?  Note
	"",        // *  r     No      No       Used for numeric constants like -1, 0, 1
	"discard", // *  w     No      Yes      Only for xbox opcodes (native opcodes have single output - discards must be removed)
	"fog",     // 1  r     Yes     No       Only for final combiner parameter
	"sum",     // 1  r     Yes     No       Only for final combiner parameter
	"prod",    // 1  r     Yes     No       Only for final combiner parameter
	"r",       // 2  r/w   Yes     Yes      We fake a few extra registers and resolve them in FixupPixelShader
	"t",       // 4  r/w   Yes     Yes      D3D9 cannot write to these!
	"v",       // 2  r     Yes     Yes
	"c"        // 16 r     Yes     No       Xbox has 8*c0,c1=16, while PC D3D8 has only 8, we try to reduce that in FixupPixelShader
};

const
int FakeRegNr_Sum = 2;
int FakeRegNr_Prod = 3;
int FakeRegNr_Xmm1 = 4;
int FakeRegNr_Xmm2 = 5;

// TODO : Translate From Dxbx :
//const
//int XFC_COMBINERSTAGENR = X_PSH_COMBINECOUNT; // Always call XFC "stage 9", 1 after the 8th combiner

int PSH_XBOX_MAX_C_REGISTER_COUNT = 16;
int PSH_XBOX_MAX_R_REGISTER_COUNT = 2;
int PSH_XBOX_MAX_T_REGISTER_COUNT = 4;
int PSH_XBOX_MAX_V_REGISTER_COUNT = 2;
// Extra constants to support features not present in Native D3D :
int PSH_XBOX_CONSTANT_FOG = PSH_XBOX_MAX_C_REGISTER_COUNT; // = 16
int PSH_XBOX_CONSTANT_FC0 = PSH_XBOX_CONSTANT_FOG + 1; // = 17
int PSH_XBOX_CONSTANT_FC1 = PSH_XBOX_CONSTANT_FC0 + 1; // = 18
int PSH_XBOX_CONSTANT_MAX = PSH_XBOX_CONSTANT_FC1 + 1; // = 19

#ifdef CXBX_USE_PS_3_0
int PSH_PC_MAX_C_REGISTER_COUNT = 224; // ps 3.0
int PSH_PC_MAX_R_REGISTER_COUNT = 32; // ps 3.0
int PSH_PC_MAX_S_REGISTER_COUNT = 16; // ps 3.0
int PSH_PC_MAX_V_REGISTER_COUNT = 10; // ps 3.0
int PSH_PC_MAX_REGISTER_COUNT = 224;
#else
#ifdef CXBX_USE_PS_2_0
int PSH_PC_MAX_C_REGISTER_COUNT = 32; // ps 2.0
int PSH_PC_MAX_R_REGISTER_COUNT = 12; // ps 2.0
int PSH_PC_MAX_S_REGISTER_COUNT = 16; // ps 2.0
int PSH_PC_MAX_T_REGISTER_COUNT = 8; // ps 2.0
int PSH_PC_MAX_V_REGISTER_COUNT = 2; // ps 2.0
int PSH_PC_MAX_REGISTER_COUNT = 32;
#else // CXBX_USE_PS_1_3
int PSH_PC_MAX_C_REGISTER_COUNT = 8; // ps 1.3
int PSH_PC_MAX_R_REGISTER_COUNT = 2; // ps 1.3
int PSH_PC_MAX_T_REGISTER_COUNT = 4; // ps 1.3
int PSH_PC_MAX_V_REGISTER_COUNT = 2; // ps 1.3
int PSH_PC_MAX_REGISTER_COUNT = 8;
#endif
#endif

enum PSH_INST_MODIFIER {
	INSMOD_NONE, // y =  x
	INSMOD_BIAS, // y =  x - 0.5         // Xbox only : TODO : Fixup occurrances!
	INSMOD_X2,   // y =  x        *   2
	INSMOD_BX2,  // y = (x - 0.5) *   2  // Xbox only : TODO : Fixup occurrances!
	INSMOD_X4,   // y =  x        *   4
	INSMOD_D2,   // y =  x        * 0.5
	INSMOD_SAT   // Xbox doesn"t support this, but has ARGMOD_SATURATE instead
};

const char *PSH_INST_MODIFIER_Str[/*PSH_INST_MODIFIER*/] = {
	"",
	"_bias",
	"_x2",
	"_bx2",
	"_x4",
	"_d2",
	"_sat"
};

// Four argument modifiers (applied in this order) :
// 1: Inversion (invert or negate : "1-" or "-")
// 2: Apply bias ("_bias")
// 3: Apply scale ("_x2", "_bx2", "_x4", or "_d2")
// 4: Apply clamp ("_sat")
enum PSH_ARG_MODIFIER {
	ARGMOD_IDENTITY,        // y = x

	ARGMOD_INVERT,          // y = 1-x     -> 0..1 >    1..0
	ARGMOD_NEGATE,          // y = -x      -> 0..1 >    0..-1

	ARGMOD_BIAS,            // y =  x-0.5  -> 0..1 > -0.5..0.5

	ARGMOD_SCALE_X2,        // y =  x*2    -> 0..1 >    0..2
	ARGMOD_SCALE_BX2,       // y = (x*2)-1 -> 0..1 >   -1..1
	ARGMOD_SCALE_X4,        // y =  x*4    -> 0..1 >    0..4
	ARGMOD_SCALE_D2,        // y =  x/2    -> 0..1 >    0..0.5

	ARGMOD_SATURATE,        // Xbox - not available in PS1.3 (can be done on output instead)

	ARGMOD_ALPHA_REPLICATE,
	ARGMOD_BLUE_REPLICATE   // PS1.1-PS1.3 only allow this if destination writemask = .a
};
// TODO : Translate From Dxbx :
// PSH_ARG_MODIFIERs = set of PSH_ARG_MODIFIER;

const char *PSH_ARG_MODIFIER_Str[/*PSH_ARG_MODIFIER*/] = {
	"%s",

	"1-%s",
	"-%s",

	"%s_bias",

	"%s_x2",
	"%s_bx2",
	"%s_x4",
	"%s_d2",

	"%s_sat",

	"%s", // .a is added via Mask
	"%s"  // .b idem
};

#endif // 0 Dxbx translation

// dump pixel shader definition to file
void DumpPixelShaderDefToFile( X_D3DPIXELSHADERDEF* pPSDef, const char* pszCode );
// print relevant contents to the debug console
void PrintPixelShaderDefContents( X_D3DPIXELSHADERDEF* pDSDef );
// Recompile Xbox PixelShader def
HRESULT EmuRecompilePshDef( X_D3DPIXELSHADERDEF* pPSDef, LPD3DXBUFFER* ppRecompiled );

/*
 * Kingofc"s pixel shader functions
 */
// Pixel Shader Stuff
HRESULT CreatePixelShaderFunction(X_D3DPIXELSHADERDEF *pPSD, LPD3DXBUFFER* ppRecompiled);

// check
bool IsValidPixelShader(void);


#ifdef _DEBUG_TRACK_PS
#define DbgPshPrintf if(g_bPrintfOn) printf
#else
inline void null_func_psh(...) { }
#define DbgPshPrintf XTL::null_func_psh
#endif

#endif // PIXELSHADER_H