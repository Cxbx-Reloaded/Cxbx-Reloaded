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
// *   Cxbx->Win32->CxbxKrnl->PixelShader.cpp
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
  (after second recompilation the shader does not work,
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
#include <CxbxKrnl/MemoryManager.h>

//#include <CxbxKrnl/EmuD3D8Types.h> // X_PSH_COMBINECOUNT

typedef uint8_t uint8; // TODO : Remove

#include <assert.h> // assert()

#include <process.h>
#include <locale.h>

//#include "EmuD3DPixelShader.h"

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
// MINUS1_TO_1_GL      :rgb->(r,g,b): 0x80=>-1.0, 0x0=>0.0, 0x7f=>1.0
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
	// ? 0x05L ?
	// ? 0x06L ?
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

// The input can have the following mappings applied :
//
// PS_INPUTMAPPING_UNSIGNED_IDENTITY : y = max(0,x)       =  1*max(0,x) + 0.0
// PS_INPUTMAPPING_UNSIGNED_INVERT   : y = 1 - max(0,x)   = -1*max(0,x) + 1.0
// PS_INPUTMAPPING_EXPAND_NORMAL     : y = 2*max(0,x) - 1 =  2*max(0,x) - 1.0
// PS_INPUTMAPPING_EXPAND_NEGATE     : y = 1 - 2*max(0,x) = -2*max(0,x) + 1.0
// PS_INPUTMAPPING_HALFBIAS_NORMAL   : y = max(0,x) - 1/2 =  1*max(0,x) - 0.5
// PS_INPUTMAPPING_HALFBIAS_NEGATE   : y = 1/2 - max(0,x) = -1*max(0,x) + 0.5
// PS_INPUTMAPPING_SIGNED_IDENTITY   : y = x              =  1*      x  + 0.0
// PS_INPUTMAPPING_SIGNED_NEGATE     : y = -x             = -1*      x  + 0.0
//
// (Note : I don't know for sure if the max() operation mentioned above is indeed what happens,
// as there's no further documentation available on this. Native Direct3D can clamp with the
// '_sat' instruction modifier, but that's not really the same as these Xbox1 input mappings.)
//
// When the input register is PS_ZERO, the above mappings result in the following constants:
//
// PS_REGISTER_NEGATIVE_ONE      (PS_INPUTMAPPING_EXPAND_NORMAL on zero)   : y = -1.0
// PS_REGISTER_NEGATIVE_ONE_HALF (PS_INPUTMAPPING_HALFBIAS_NORMAL on zero) : y = -0.5
// PS_REGISTER_ZERO itself                                                 : y =  0.0
// PS_REGISTER_ONE_HALF          (PS_INPUTMAPPING_HALFBIAS_NEGATE on zero) : y =  0.5
// PS_REGISTER_ONE               (PS_INPUTMAPPING_UNSIGNED_INVERT on zero) : y =  1.0
// (Note : It has no define, but PS_INPUTMAPPING_EXPAND_NEGATE on zero results in ONE too!)

enum PS_INPUTMAPPING
{
    PS_INPUTMAPPING_UNSIGNED_IDENTITY= 0x00L, // max(0,x)         OK for final combiner: y = abs(x)
    PS_INPUTMAPPING_UNSIGNED_INVERT=   0x20L, // 1 - max(0,x)     OK for final combiner: y = 1 - x
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

    PS_REGISTER_ONE=               PS_REGISTER_ZERO | PS_INPUTMAPPING_UNSIGNED_INVERT, // 0x20 OK for final combiner
    PS_REGISTER_NEGATIVE_ONE=      PS_REGISTER_ZERO | PS_INPUTMAPPING_EXPAND_NORMAL,   // 0x40 invalid for final combiner
    PS_REGISTER_ONE_HALF=          PS_REGISTER_ZERO | PS_INPUTMAPPING_HALFBIAS_NEGATE, // 0xa0 invalid for final combiner
    PS_REGISTER_NEGATIVE_ONE_HALF= PS_REGISTER_ZERO | PS_INPUTMAPPING_HALFBIAS_NORMAL, // 0x80 invalid for final combiner

	PS_REGISTER_CXBX_PROD =        PS_REGISTER_ZERO | PS_INPUTMAPPING_SIGNED_IDENTITY,  // Cxbx internal use
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

constexpr DWORD PS_ChannelMask = (DWORD)PS_CHANNEL_ALPHA;
constexpr DWORD PS_NoChannelMask = (DWORD)(~PS_ChannelMask);
constexpr DWORD PS_AlphaChannelsMask = (DWORD)(PS_ChannelMask | (PS_ChannelMask << 8) | (PS_ChannelMask << 16) | (PS_ChannelMask << 24));
constexpr DWORD PS_NoChannelsMask = (DWORD)(~PS_AlphaChannelsMask);

enum PS_FINALCOMBINERSETTING
{
    PS_FINALCOMBINERSETTING_CLAMP_SUM=     0x80, // V1+R0 sum clamped to [0,1]
    PS_FINALCOMBINERSETTING_COMPLEMENT_V1= 0x40, // unsigned invert mapping  (1 - v1) is used as an input to the sum rather than v1
    PS_FINALCOMBINERSETTING_COMPLEMENT_R0= 0x20, // unsigned invert mapping  (1 - r0) is used as an input to the sum rather than r0
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
    // 0x28 ?
    PS_COMBINEROUTPUT_SHIFTRIGHT_1=        0x30L, // y = x/2
    // 0x38 ?

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
	{/* PO_COMMENT */  /*mn:*/";", /*_Out: */ 0, /*_In: */ 0, /*note:*/"" }, //
	{/* PO_PS */  /*mn:*/"ps",   /*_Out: */ 0, /*_In: */ 0, /*note:*/"" }, // Must occur once, Xbox needs an x prefix {xps}, Native needs a 1.3 suffix {ps.1.3}
	{/* PO_DEF */ /*mn:*/"def",  /*_Out: */ 1, /*_In: */ 4, /*note:*/"" }, // Output must be a PARAM_C, arguments must be 4 floats [0.00f .. 1.00f]
#ifdef CXBX_USE_PS_2_0
	{/* PO_DCL */ /*mn:*/"dcl",  /*_Out: */ 1, /*_In: */ 0, /*note:*/"" },
#endif
	{/* PO_TEX */ /*mn:*/"tex",  /*_Out: */ 1, /*_In: */ 0, /*note:*/"" },
	{/* PO_TEXBEM */ /*mn:*/"texbem",  /*_Out: */ 1, /*_In: */ 1, /*note:*/"" },
	{/* PO_TEXBEML */ /*mn:*/"texbeml",  /*_Out: */ 1, /*_In: */ 1, /*note:*/"" },
	{/* PO_TEXBRDF */ /*mn:*/"texbrdf",  /*_Out: */ 1, /*_In: */ 1, /*note:*/"" }, // /*note: */ Not supported by Direct3D8 ?
	{/* PO_TEXCOORD */ /*mn:*/"texcoord",  /*_Out: */ 1, /*_In: */ 0, /*note:*/"" },
	{/* PO_TEXKILL */ /*mn:*/"texkill",  /*_Out: */ 1, /*_In: */ 0, /*note:*/"" },
	{/* PO_TEXDP3 */ /*mn:*/"texdp3",  /*_Out: */ 1, /*_In: */ 1, /*note:*/"" },
	{/* PO_TEXDP3TEX */ /*mn:*/"texdp3tex",  /*_Out: */ 1, /*_In: */ 1, /*note:*/"" },
	{/* PO_TEXM3X2TEX */ /*mn:*/"texm3x2tex",  /*_Out: */ 1, /*_In: */ 1, /*note:*/"" },
	{/* PO_TEXM3X2DEPTH */ /*mn:*/"texm3x2depth",  /*_Out: */ 1, /*_In: */ 1, /*note:*/"" }, // /*note: */ requires ps.1.3 and a preceding texm3x2pad
	{/* PO_TEXM3X3DIFF */ /*mn:*/"texm3x3diff",  /*_Out: */ 1, /*_In: */ 1, /*note:*/"" }, // /*note: */ Not supported by Direct3D8 ?
	{/* PO_TEXM3X3VSPEC */ /*mn:*/"texm3x3vspec",  /*_Out: */ 1, /*_In: */ 1, /*note:*/"" },
	{/* PO_TEXM3X3TEX */ /*mn:*/"texm3x3tex",  /*_Out: */ 1, /*_In: */ 1, /*note:*/"" }, // /*note: */ Uses a cube texture
	{/* PO_TEXREG2AR */ /*mn:*/"texreg2ar",  /*_Out: */ 1, /*_In: */ 1, /*note:*/"" },
	{/* PO_TEXREG2GB */ /*mn:*/"texreg2gb",  /*_Out: */ 1, /*_In: */ 1, /*note:*/"" },
	{/* PO_TEXM3X2PAD */ /*mn:*/"texm3x2pad",  /*_Out: */ 1, /*_In: */ 1, /*note:*/"" },
	{/* PO_TEXM3X3PAD */ /*mn:*/"texm3x3pad",  /*_Out: */ 1, /*_In: */ 1, /*note:*/"" },
	{/* PO_TEXM3X3SPEC */ /*mn:*/"texm3x3spec",  /*_Out: */ 1, /*_In: */ 2, /*note:*/"" },
	// Arithmetic opcodes :
	{/* PO_ADD */ /*mn:*/"add",  /*_Out: */ 1, /*_In: */ 2, /*note:*/"d0=s0+s1" },
	{/* PO_CMP */ /*mn:*/"cmp",  /*_Out: */ 1, /*_In: */ 3, /*note:*/"d0={s0>=0?s1:s2}" },
	{/* PO_CND */ /*mn:*/"cnd",  /*_Out: */ 1, /*_In: */ 3, /*note:*/"d1={s0.a>0.5?s1:s2}" }, // 1st input must be "r0.a"
	{/* PO_DP3 */ /*mn:*/"dp3",  /*_Out: */ 1, /*_In: */ 2, /*note:*/"d0=s0 dot3 s1" },
	{/* PO_DP4 */ /*mn:*/"dp4",  /*_Out: */ 1, /*_In: */ 2, /*note:*/"d0=s0 dot4 s1" },
	{/* PO_LRP */ /*mn:*/"lrp",  /*_Out: */ 1, /*_In: */ 3, /*note:*/"d0=s0*s1+{1-s0}*s2=s0*{s1-s2}+s2" },
	{/* PO_MAD */ /*mn:*/"mad",  /*_Out: */ 1, /*_In: */ 3, /*note:*/"d0=s0*s1+s2" },
	{/* PO_MOV */ /*mn:*/"mov",  /*_Out: */ 1, /*_In: */ 1, /*note:*/"d0=s0" },
	{/* PO_MUL */ /*mn:*/"mul",  /*_Out: */ 1, /*_In: */ 2, /*note:*/"d0=s0*s1" },
	{/* PO_NOP */ /*mn:*/"nop",  /*_Out: */ 0, /*_In: */ 0, /*note:*/"" },
	{/* PO_SUB */ /*mn:*/"sub",  /*_Out: */ 1, /*_In: */ 2, /*note:*/"d0=s0-s1" },
	// Xbox-only {NV2A} opcodes :
	{/* PO_XMMA */  /*mn:*/"xmma", /*_Out: */ 3, /*_In: */ 4, /*note:*/"d0=s0*s1, d1=s2*s3, d2={s0*s1}+{s2*s3}" },
	{/* PO_XMMC */  /*mn:*/"xmmc", /*_Out: */ 3, /*_In: */ 4, /*note:*/"d0=s0*s1, d1=s2*s3, d2={r0.a>0.5}?{s0*s1}:{s2*s3}" },
	{/* PO_XDM */ /*mn:*/"xdm",  /*_Out: */ 2, /*_In: */ 4, /*note:*/"d0=s0 dot s1, d1=s2*s3" },
	{/* PO_XDD */ /*mn:*/"xdd",  /*_Out: */ 2, /*_In: */ 4, /*note:*/"d0=s0 dot s1, d1=s2 dot s3" },
	{/* PO_XFC */ /*mn:*/"xfc",  /*_Out: */ 0, /*_In: */ 7, /*note:*/"r0.rgb=s0*s1+{1-s0}*s2+s3, r0.a=s6.a, prod=s4*s5, sum=r0+v1" }
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

constexpr int FakeRegNr_Sum = 2;
constexpr int FakeRegNr_Prod = 3;
constexpr int FakeRegNr_Xmm1 = 4;
constexpr int FakeRegNr_Xmm2 = 5;

constexpr int XFC_COMBINERSTAGENR = XTL::X_PSH_COMBINECOUNT; // Always call XFC 'stage 9', 1 after the 8th combiner

constexpr int PSH_XBOX_MAX_C_REGISTER_COUNT = 16;
constexpr int PSH_XBOX_MAX_R_REGISTER_COUNT = 2;
constexpr int PSH_XBOX_MAX_T_REGISTER_COUNT = 4;
constexpr int PSH_XBOX_MAX_V_REGISTER_COUNT = 2;
// Extra constants to support features not present in Native D3D :
constexpr int PSH_XBOX_CONSTANT_FOG = PSH_XBOX_MAX_C_REGISTER_COUNT; // = 16
constexpr int PSH_XBOX_CONSTANT_FC0 = PSH_XBOX_CONSTANT_FOG + 1; // = 17
constexpr int PSH_XBOX_CONSTANT_FC1 = PSH_XBOX_CONSTANT_FC0 + 1; // = 18
constexpr int PSH_XBOX_CONSTANT_MAX = PSH_XBOX_CONSTANT_FC1 + 1; // = 19

#ifdef CXBX_USE_PS_3_0
constexpr int PSH_PC_MAX_C_REGISTER_COUNT = 224; // ps 3.0
constexpr int PSH_PC_MAX_R_REGISTER_COUNT = 32; // ps 3.0
constexpr int PSH_PC_MAX_S_REGISTER_COUNT = 16; // ps 3.0
constexpr int PSH_PC_MAX_V_REGISTER_COUNT = 10; // ps 3.0
constexpr int PSH_PC_MAX_REGISTER_COUNT = 224;
#else
#ifdef CXBX_USE_PS_2_0
constexpr int PSH_PC_MAX_C_REGISTER_COUNT = 32; // ps 2.0
constexpr int PSH_PC_MAX_R_REGISTER_COUNT = 12; // ps 2.0
constexpr int PSH_PC_MAX_S_REGISTER_COUNT = 16; // ps 2.0
constexpr int PSH_PC_MAX_T_REGISTER_COUNT = 8; // ps 2.0
constexpr int PSH_PC_MAX_V_REGISTER_COUNT = 2; // ps 2.0
constexpr int PSH_PC_MAX_REGISTER_COUNT = 32;
#else // CXBX_USE_PS_1_3
constexpr int PSH_PC_MAX_C_REGISTER_COUNT = 8; // ps 1.3
constexpr int PSH_PC_MAX_R_REGISTER_COUNT = 2; // ps 1.3
constexpr int PSH_PC_MAX_T_REGISTER_COUNT = 4; // ps 1.3
constexpr int PSH_PC_MAX_V_REGISTER_COUNT = 2; // ps 1.3
constexpr int PSH_PC_MAX_REGISTER_COUNT = 8;
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
typedef DWORD PSH_ARG_MODIFIERs; // = set of PSH_ARG_MODIFIER;

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

struct RPSRegisterObject {
	bool IsAlpha;
	PS_REGISTER Reg;

	void Decode(uint8 Value, bool aIsAlpha);
	std::string DecodedToString();
};

struct RPSInputRegister : RPSRegisterObject {
	PS_CHANNEL Channel;
	PS_INPUTMAPPING InputMapping;

	void Decode(uint8 Value, bool aIsAlpha);
	std::string DecodedToString();
};

struct RPSCombinerOutput : RPSRegisterObject {
	RPSInputRegister Input1; // Called InputA or InputC (depending if it's inside the AB or CD combiner)
	RPSInputRegister Input2; // Called InputC or InputD (depending if it's inside the AB or CD combiner)
	bool DotProduct; // False=Multiply, True=DotProduct
	bool BlueToAlpha; // False=Alpha-to-Alpha, True=Blue-to-Alpha

	void Decode(uint8 Value, DWORD PSInputs, bool aIsAlpha);
};

struct RPSCombinerOutputMuxSum : RPSRegisterObject {
	RPSCombinerOutput OutputAB; // Contains InputA and InputB (as Input1 and Input2)
	RPSCombinerOutput OutputCD; // Contains InputC and InputD (as Input1 and Input2)
};

struct RPSCombinerStageChannel {
	RPSCombinerOutputMuxSum OutputSUM; // Contains OutputAB, OutputCD
	PS_COMBINEROUTPUT CombinerOutputFlags;
	bool AB_CD_SUM; // True=AB+CD, False=MUX(AB;CD) based on R0.a

	void Decode(DWORD PSInputs, DWORD PSOutputs, bool IsAlpha = false);
};

struct RPSCombinerStage {
	RPSCombinerStageChannel RGB;
	RPSCombinerStageChannel Alpha;
};

struct RPSFinalCombiner {
	RPSInputRegister InputA;
	RPSInputRegister InputB;
	RPSInputRegister InputC;
	RPSInputRegister InputD;
	RPSInputRegister InputE;
	RPSInputRegister InputF;
	RPSInputRegister InputG;

	PS_FINALCOMBINERSETTING FinalCombinerFlags;

	uint8 FinalCombinerC0Mapping;
	uint8 FinalCombinerC1Mapping;

	DWORD dwPS_GLOBALFLAGS;

	void Decode(const DWORD PSFinalCombinerInputsABCD, const DWORD PSFinalCombinerInputsEFG, const DWORD PSFinalCombinerConstants);
};

constexpr DWORD MASK_R = 0x001;
constexpr DWORD MASK_G = 0x002;
constexpr DWORD MASK_B = 0x004;
constexpr DWORD MASK_A = 0x008;
constexpr DWORD MASK_RGB = MASK_R | MASK_G | MASK_B;
constexpr DWORD MASK_RGBA = MASK_R | MASK_G | MASK_B | MASK_A;

enum
	TArgumentType {
	atInput, atOutput, atFinalCombiner
};

typedef struct _PSH_IMD_ARGUMENT {
	PSH_ARGUMENT_TYPE Type; // For parameters: R, T, V or C  For output : Discard, R, T or V
	int16 Address;           // Register address
	DWORD Mask;
	PSH_ARG_MODIFIERs Modifiers;
	float Multiplier;

	void SetConstValue(float Value);
	float GetConstValue();
	bool UsesRegister();
	bool IsRegister(PSH_ARGUMENT_TYPE aRegType, int16 aAddress); // overload;
	bool IsRegister(PSH_ARGUMENT_TYPE aRegType, int16 aAddress, DWORD aMask); // overload;
	void SetRegister(PSH_ARGUMENT_TYPE aRegType, int16 aAddress, DWORD aMask);
	std::string ToString();
	bool Decode(const DWORD Value, DWORD aMask, TArgumentType ArgumentType);
	void Invert();
	void Negate();
} PSH_IMD_ARGUMENT,
*PPSH_IMD_ARGUMENT;

//TPSH_IMD_ARGUMENTArray = array[0..(MaxInt div SizeOf(PSH_IMD_ARGUMENT)) - 1] of PSH_IMD_ARGUMENT;
//PPSH_IMD_ARGUMENTs = ^TPSH_IMD_ARGUMENTArray;

typedef struct _PSH_INTERMEDIATE_FORMAT {
	int CombinerStageNr;
	bool IsCombined;
	PSH_OPCODE Opcode;
	std::string CommentString;
	PSH_INST_MODIFIER Modifier;
	PSH_IMD_ARGUMENT Output[3]; // 3 = xmm* output count
	PSH_IMD_ARGUMENT Parameters[7]; // 7 = xfc parameter count

	void Initialize(const PSH_OPCODE aOpcode);
	std::string ToString();
	bool IsArithmetic();
	void ScaleOutput(float aFactor);
	bool ReadsFromRegister(PSH_ARGUMENT_TYPE aRegType, int aAddress); // overload;
	bool ReadsFromRegister(PSH_ARGUMENT_TYPE aRegType, int aAddress, DWORD aMask); // overload;
	bool WritesToRegister(PSH_ARGUMENT_TYPE aRegType, int aAddress); // overload;
	bool WritesToRegister(PSH_ARGUMENT_TYPE aRegType, int aAddress, DWORD aMask); // overload;
	void SwapParameter(const int Index1, const int Index2);
	void XSwapOutput();
	bool MoveRemovableParametersRight(const int Index1, const int Index2);
	bool XMoveNonRegisterOutputsRight();
	void XCopySecondOpcodeToFirst(const PSH_OPCODE aOpcode);
	bool Decode(DWORD CombinerStageNr, DWORD PSInputs, DWORD PSOutputs, DWORD aMask);
	bool DecodeFinalCombiner(DWORD aPSFinalCombinerInputsABCD, DWORD aPSFinalCombinerInputsEFG);
} PSH_INTERMEDIATE_FORMAT,
*PPSH_INTERMEDIATE_FORMAT;

typedef struct _PSH_RECOMPILED_SHADER {
	struct _PSH_RECOMPILED_SHADER *Next;
	XTL::X_D3DPIXELSHADERDEF PSDef;
	std::string NewShaderStr;
	DWORD ConvertedHandle;
	bool ConstInUse[PSH_XBOX_CONSTANT_MAX];
	int ConstMapping[PSH_XBOX_CONSTANT_MAX];
} PSH_RECOMPILED_SHADER,
*PPSH_RECOMPILED_SHADER;

struct PSH_XBOX_SHADER {
	// Reserve enough slots for all shaders, so we need space for 2 constants, 4 texture addressing codes and 5 lines per opcode : :
	PSH_INTERMEDIATE_FORMAT Intermediate[2 + XTL::X_D3DTS_STAGECOUNT + (XTL::X_PSH_COMBINECOUNT * 5) + 1];
	int IntermediateCount;

	PS_TEXTUREMODES PSTextureModes[XTL::X_D3DTS_STAGECOUNT];
	PS_DOTMAPPING PSDotMapping[XTL::X_D3DTS_STAGECOUNT];
	DWORD PSCompareMode[XTL::X_D3DTS_STAGECOUNT];
	int PSInputTexture[XTL::X_D3DTS_STAGECOUNT];

	PS_FINALCOMBINERSETTING FinalCombinerFlags;
	// Note : The following constants are only needed for PSH_XBOX_SHADER.DecodedToString,
	// they are not involved in the actual pixel shader recompilation anymore :
	RPSFinalCombiner FinalCombiner;
	RPSCombinerStage Combiners[XTL::X_PSH_COMBINECOUNT];
	int NumberOfCombiners;
	DWORD CombinerCountFlags; // For PS_COMBINERCOUNTFLAGS
							  // Read from CombinerCountFlags :
	bool CombinerMuxesOnMsb;
	bool CombinerHasUniqueC0;
	bool CombinerHasUniqueC1;

	std::string ToString();
	void Log(const std::string PhaseStr);
	PPSH_INTERMEDIATE_FORMAT NewIntermediate();
	void InsertIntermediate(PPSH_INTERMEDIATE_FORMAT pIntermediate, int Index);
	void DeleteIntermediate(int Index);
	void DeleteLastIntermediate();
	std::string OriginalToString(XTL::X_D3DPIXELSHADERDEF *pPSDef);
	PSH_RECOMPILED_SHADER Decode(XTL::X_D3DPIXELSHADERDEF *pPSDef);
	std::string DecodedToString(XTL::X_D3DPIXELSHADERDEF *pPSDef);
	bool DecodeTextureModes(XTL::X_D3DPIXELSHADERDEF *pPSDef);
	bool MoveRemovableParametersRight();
	void ConvertXboxOpcodesToNative(XTL::X_D3DPIXELSHADERDEF *pPSDef);
	bool ConvertConstantsToNative(XTL::X_D3DPIXELSHADERDEF *pPSDef, /*var OUT*/PSH_RECOMPILED_SHADER *Recompiled);
	bool RemoveUselessWrites();
	bool IsRegisterFreeFromIndexOnwards(int aIndex, PSH_ARGUMENT_TYPE aRegType, int16 aAddress);
	void ReplaceRegisterFromIndexOnwards(int aIndex,
		PSH_ARGUMENT_TYPE aSrcRegType, int16 aSrcAddress,
		PSH_ARGUMENT_TYPE aDstRegType, int16 aDstAddress);
	bool ConvertXMMToNative_Except3RdOutput(int i);
	void ConvertXMMAToNative(int i);
	void ConvertXMMCToNative(int i);
	void ConvertXDMToNative(int i);
	void ConvertXDDToNative(int i);
	void ConvertXFCToNative(int i);
	bool CombineInstructions();
	bool RemoveNops();
	bool SimplifyMOV(PPSH_INTERMEDIATE_FORMAT Cur);
	bool SimplifyADD(PPSH_INTERMEDIATE_FORMAT Cur);
	bool SimplifyMAD(PPSH_INTERMEDIATE_FORMAT Cur);
	bool SimplifySUB(PPSH_INTERMEDIATE_FORMAT Cur);
	bool SimplifyMUL(PPSH_INTERMEDIATE_FORMAT Cur);
	bool SimplifyLRP(PPSH_INTERMEDIATE_FORMAT Cur);
	bool FixupPixelShader();
	bool FixInvalidSrcSwizzle();
	bool FixMissingR0a();
	bool FixCoIssuedOpcodes();
};

/*
* Blueshogun's code (useful for debugging the PixelShader binary format)
*/

// PS Texture Modes
char* PS_TextureModes[/*PS_TEXTUREMODES*/] =
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
char* PS_DotMapping[/*PS_DOTMAPPING*/] =
{
	"PS_DOTMAPPING_ZERO_TO_ONE",		// 0x00
	"PS_DOTMAPPING_MINUS1_TO_1_D3D",	// 0x01
	"PS_DOTMAPPING_MINUS1_TO_1_GL",		// 0x02
	"PS_DOTMAPPING_MINUS1_TO_1",		// 0x03
	"PS_DOTMAPPING_HILO_1",				// 0x04
	"???",                              // 0x05
	"???",                              // 0x06
	"PS_DOTMAPPING_HILO_HEMISPHERE",	// 0x07
};

#if 0 // array unusable for bitflags
// PS CompareMode
char* PS_CompareMode[/*PS_COMPAREMODE*/] =
{
	"PS_COMPAREMODE_S_LT", // 0x00L
	"PS_COMPAREMODE_S_GE", // 0x01L

	"PS_COMPAREMODE_T_LT", // 0x00L
	"PS_COMPAREMODE_T_GE", // 0x02L

	"???",

	"PS_COMPAREMODE_R_LT", // 0x00L
	"PS_COMPAREMODE_R_GE", // 0x04L

	"???",
	"???",
	"???",

	"PS_COMPAREMODE_Q_LT", // 0x00L
	"PS_COMPAREMODE_Q_GE", // 0x08L
};
#endif

#if 1 // array unfit for bitflags
// PS CombinerCountFlags
char* PS_CombinerCountFlags[/*PS_COMBINERCOUNTFLAGS*/] =
{
	"PS_COMBINERCOUNT_MUX_LSB",		// 0x0000L, // mux on r0.a lsb
	"PS_COMBINERCOUNT_MUX_MSB",		// 0x0001L, // mux on r0.a msb

	"PS_COMBINERCOUNT_SAME_C0",		// 0x0000L, // c0 same in each stage
	"PS_COMBINERCOUNT_UNIQUE_C0",	// 0x0010L, // c0 unique in each stage

	"PS_COMBINERCOUNT_SAME_C1",		// 0x0000L, // c1 same in each stage
	"PS_COMBINERCOUNT_UNIQUE_C1",	// 0x0100L  // c1 unique in each stage
};
#endif

#if 0 // array unusable for bitflags
// PS InputMapping
char* PS_InputMapping[/*PS_INPUTMAPPING*/] =
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
#endif

#if 1 // array unfit for bitflags
// PS Register (note, a few have one space, to line up the output a little)
char *PS_RegisterStr[/*PS_REGISTER*/] =
{
	"PS_REGISTER_ZERO",      // 0x00L, // r
	"PS_REGISTER_DISCARD",   // 0x00L, // w
	"PS_REGISTER_C0 ",       // 0x01L, // r
	"PS_REGISTER_C1 ",       // 0x02L, // r
	"PS_REGISTER_FOG",       // 0x03L, // r
	"PS_REGISTER_V0 ",       // 0x04L, // r/w
	"PS_REGISTER_V1 ",       // 0x05L, // r/w
	"??", // 0x06
	"??", // 0x07
	"PS_REGISTER_T0 ",       // 0x08L, // r/w
	"PS_REGISTER_T1 ",       // 0x09L, // r/w
	"PS_REGISTER_T2 ",       // 0x0aL, // r/w
	"PS_REGISTER_T3 ",       // 0x0bL, // r/w
	"PS_REGISTER_R0 ",       // 0x0cL, // r/w
	"PS_REGISTER_R1 ",       // 0x0dL, // r/w
	"PS_REGISTER_V1R0_SUM",  // 0x0eL, // r
	"PS_REGISTER_EF_PROD",   // 0x0fL, // r

	"PS_REGISTER_ONE",               // PS_REGISTER_ZERO | PS_INPUTMAPPING_UNSIGNED_INVERT, // OK for final combiner
	"PS_REGISTER_NEGATIVE_ONE",      // PS_REGISTER_ZERO | PS_INPUTMAPPING_EXPAND_NORMAL,   // invalid for final combiner
	"PS_REGISTER_ONE_HALF",          // PS_REGISTER_ZERO | PS_INPUTMAPPING_HALFBIAS_NEGATE, // invalid for final combiner
	"PS_REGISTER_NEGATIVE_ONE_HALF"  // PS_REGISTER_ZERO | PS_INPUTMAPPING_HALFBIAS_NORMAL, // invalid for final combiner
};
#endif

// PS Channel
char* PS_ChannelStr[/*PS_CHANNEL*/] =
{
	"PS_CHANNEL_RGB",	// 0x00, // used as RGB source
	"PS_CHANNEL_BLUE",	// 0x00, // used as ALPHA source
	"PS_CHANNEL_ALPHA",	// 0x10, // used as RGB or ALPHA source
};

// PS FinalCombinerSetting
char* PS_FinalCombinerSettingStr[/*PS_FINALCOMBINERSETTING*/] =
{
	"PS_FINALCOMBINERSETTING_CLAMP_SUM",		// 0x80, // V1+R0 sum clamped to [0,1]
	"PS_FINALCOMBINERSETTING_COMPLEMENT_V1",	// 0x40, // unsigned invert mapping
	"PS_FINALCOMBINERSETTING_COMPLEMENT_R0",	// 0x20, // unsigned invert mapping
};

// PS CombineOutput
char* PS_CombineOutputStr[/*PS_COMBINEROUTPUT*/] =
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
char* PS_GlobalFlagsStr[/*PS_GLOBALFLAGS*/] =
{
	"PS_GLOBALFLAGS_NO_TEXMODE_ADJUST",     // 0x0000L, // don't adjust texture modes
	"PS_GLOBALFLAGS_TEXMODE_ADJUST",        // 0x0001L, // adjust texture modes according to set texture
};

const int CONST_NEG_ONE = -2;
const int CONST_NEG_HALF = -1;
const int CONST_ZERO = 0;
const int CONST_POS_HALF = 1; // Note : Instead of 0.5 we use 1 (so we can keep using integers)
const int CONST_POS_ONE = 2;

std::string PSCombinerOutputFlagsToStr(const DWORD dwFlags, bool IsAlpha = false)
{
	std::string Result = PS_CombineOutputStr[0 + ((dwFlags & 0x38) >> 3)];
	Result = Result + " | " + PS_CombineOutputStr[8 + ((dwFlags & PS_COMBINEROUTPUT_AB_DOT_PRODUCT) >> 1)];
	Result = Result + " | " + PS_CombineOutputStr[10 + ((dwFlags & PS_COMBINEROUTPUT_CD_DOT_PRODUCT) >> 0)];
	Result = Result + " | " + PS_CombineOutputStr[12 + ((dwFlags & PS_COMBINEROUTPUT_AB_CD_MUX) >> 2)];

	if (!IsAlpha) {
		if (dwFlags & PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA)
			Result = Result + " | " + PS_CombineOutputStr[6];

		if (dwFlags & PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA)
			Result = Result + " | " + PS_CombineOutputStr[7];
	}

	return Result;
}

std::string PSFinalCombinerSettingToStr(const DWORD dwPS_FINALCOMBINERSETTING)
{
	std::string Result = "";
	if (dwPS_FINALCOMBINERSETTING & PS_FINALCOMBINERSETTING_CLAMP_SUM)
		Result = Result + " | " + PS_FinalCombinerSettingStr[0];

	if (dwPS_FINALCOMBINERSETTING & PS_FINALCOMBINERSETTING_COMPLEMENT_V1)
		Result = Result + " | " + PS_FinalCombinerSettingStr[1];

	if (dwPS_FINALCOMBINERSETTING & PS_FINALCOMBINERSETTING_COMPLEMENT_R0)
		Result = Result + " | " + PS_FinalCombinerSettingStr[2];

	if (!Result.empty())
		Result.erase(0, 3);

	return Result;
}

/* PSH_IMD_ARGUMENT */

void PSH_IMD_ARGUMENT::SetConstValue(float Value)
{
	Type = PARAM_VALUE;
	Address = CONST_ZERO;
	Multiplier = Value;
	Modifiers = 0;
}

float PSH_IMD_ARGUMENT::GetConstValue()
{
  if (Type != PARAM_VALUE) {
    // Anything other than a value-parameter returns a value never checked for (:
    return INFINITY;
  }

  float Result = Multiplier;

  // y == 1-x     -> 0..1 >    1..0
  if ((Modifiers & ARGMOD_INVERT) > 0)    Result = 1.0f-Result;

  // y == -x      -> 0..1 >    0..-1
  if ((Modifiers & ARGMOD_NEGATE) > 0)    Result = -Result;

  // y ==  x-0.5  -> 0..1 > -0.5..0.5
  if ((Modifiers & ARGMOD_BIAS) > 0)      Result = Result-0.5f;

  // y ==  x*2    -> 0..1 >    0..2
  if ((Modifiers & ARGMOD_SCALE_X2) > 0)  Result = Result*2.0f;

  // y == (x*2)-1 -> 0..1 >   -1..1
  if ((Modifiers & ARGMOD_SCALE_BX2) > 0) Result = (Result*2.0f)-1.0f;

  // y ==  x*4    -> 0..1 >    0..4
  if ((Modifiers & ARGMOD_SCALE_X4) > 0)  Result = Result*4.0f;

  // y ==  x/2    -> 0..1 >    0..0.5
  if ((Modifiers & ARGMOD_SCALE_D2) > 0)  Result = Result/2.0f;

  return Result;
} // GetConstValue

bool PSH_IMD_ARGUMENT::UsesRegister()
{
	return (Type > PARAM_DISCARD);
}

bool PSH_IMD_ARGUMENT::IsRegister(PSH_ARGUMENT_TYPE aRegType, int16 aAddress)
{
	return (Type == aRegType)
		&& (Address == aAddress);
}

bool PSH_IMD_ARGUMENT::IsRegister(PSH_ARGUMENT_TYPE aRegType, int16 aAddress, DWORD aMask)
{
	return IsRegister(aRegType, aAddress)
        // Check the mask itself, but also 'mask-less' :
        && (((Mask & aMask) == aMask) || (Mask == 0));
}

void PSH_IMD_ARGUMENT::SetRegister(PSH_ARGUMENT_TYPE aRegType, int16 aAddress, DWORD aMask)
{
	Type = aRegType;
	Address = aAddress;
	Mask = aMask;
}

std::string PSH_IMD_ARGUMENT::ToString()
{
	std::string Result;

  if (Type == PARAM_VALUE)
  {
    Result = std::to_string(GetConstValue());	
    if (Result.find(".") > 0)
      Result = Result + 'f';

    return Result;
  }

  Result = PSH_ARGUMENT_TYPE_Str[Type];

  if (Type >= PARAM_R)
    Result = Result + std::to_string(Address);

  if (UsesRegister())
  {
    for (DWORD Modifier = ARGMOD_IDENTITY; Modifier < ARGMOD_BLUE_REPLICATE; Modifier++)
      if (Modifier & Modifiers) {
		char buffer[256];
		Result = std::string(buffer, sprintf(buffer, PSH_ARG_MODIFIER_Str[Modifier], Result.c_str()));
	  }

    if ((Mask > 0) && (Mask != MASK_RGBA))
    {
      Result = Result + '.';
      if ((Mask & MASK_R) > 0) Result = Result + 'r';
      if ((Mask & MASK_G) > 0) Result = Result + 'g';
      if ((Mask & MASK_B) > 0) Result = Result + 'b';
      if ((Mask & MASK_A) > 0) Result = Result + 'a';
    }
  }
  return Result;
} // ToString

bool PSH_IMD_ARGUMENT::Decode(const DWORD Value, DWORD aMask, TArgumentType ArgumentType)
{
	PS_REGISTER Reg;
	PS_INPUTMAPPING InputMapping;
	PS_CHANNEL Channel;

	bool Result = true;
  Address = 0;
  Mask = aMask;
  Modifiers = ARGMOD_IDENTITY;
  Multiplier = 1.0;

  // Determine PS_REGISTER for (this argument type :
  {
    Reg = (PS_REGISTER)(Value & 0xF);
    if (ArgumentType == atOutput)
    {
      // Output arguments may not write to C0 | C1, prevent that :
      if ((Reg == PS_REGISTER_C0) || (Reg == PS_REGISTER_C1))
        Reg = PS_REGISTER_CXBX_PROD; // unhandled case - will reach "invalid" else-block
    }
    else
    {
      // Input arguments (normal | final combiners) can use the extended PS_REGISTER values :
      if (Reg == PS_REGISTER_ZERO)
        Reg = (PS_REGISTER)(Value & 0xE0);

      // 'Signed Identity' flag on PS_REGISTER_ZERO has no meaning, treat as zero :
      if (Reg == PS_REGISTER_CXBX_PROD)
        Reg = PS_REGISTER_ZERO;

      // Prevent decoding final combiner registers outside that mode :
      if (ArgumentType != atFinalCombiner) 
        if ((Reg == PS_REGISTER_FOG) || (Reg == PS_REGISTER_V1R0_SUM) || (Reg == PS_REGISTER_EF_PROD))
          Reg = PS_REGISTER_CXBX_PROD; // unhandled case - will reach "invalid" else-block
    }
  }

  switch (Reg) {
    case PS_REGISTER_ZERO:
    {
      if (ArgumentType == atOutput)
      {
        // Mark output arguments as 'discard' & return that fact :
        Type = PARAM_DISCARD;
        Result = false;
      }
      else
        Type = PARAM_VALUE;

      Address = CONST_ZERO;
      Multiplier = 0.0f;
	  break;
    }
    case PS_REGISTER_C0:
      Type = PARAM_C;
	  break;
	case PS_REGISTER_C1:
    {
      Type = PARAM_C;
      Address = 1;
	  break;
	}
    case PS_REGISTER_V0:
      Type = PARAM_V;
	  break;
	case PS_REGISTER_V1:
    {
      Type = PARAM_V;
      Address = 1;
	  break;
	}
    case PS_REGISTER_T0:
      Type = PARAM_T;
	  break;
	case PS_REGISTER_T1:
    {
      Type = PARAM_T;
      Address = 1;
	  break;
	}
    case PS_REGISTER_T2:
    {
      Type = PARAM_T;
      Address = 2;
	  break;
	}
    case PS_REGISTER_T3:
    {
      Type = PARAM_T;
      Address = 3;
	  break;
	}
    case PS_REGISTER_R0:
      Type = PARAM_R;
	  break;
	case PS_REGISTER_R1:
    {
      Type = PARAM_R;
      Address = 1;
	  break;
	}
    // Registers only available when ArgumentType != atOutput (Reg is capped otherwise) :
    case PS_REGISTER_ONE:
    {
      Type = PARAM_VALUE;
      Address = CONST_POS_ONE;
      Multiplier = 1.0f;
	  break;
	}
    case PS_REGISTER_NEGATIVE_ONE:
    {
      Type = PARAM_VALUE;
      Address = CONST_NEG_ONE;
      Multiplier = -1.0f;
	  break;
	}
    case PS_REGISTER_ONE_HALF:
    {
      Type = PARAM_VALUE;
      Address = CONST_POS_HALF;
      Multiplier = 0.5f;
	  break;
	}
    case PS_REGISTER_NEGATIVE_ONE_HALF:
    {
      Type = PARAM_VALUE;
      Address = CONST_NEG_HALF;
      Multiplier = -0.5f;
	  break;
	}
    // Registers only available when ArgumentType == atFinalCombiner (Reg is capped otherwise) :
    case PS_REGISTER_FOG:
      Type = PARAM_FOG;
	  break;
	case PS_REGISTER_V1R0_SUM:
      Type = PARAM_V1R0_SUM;
	  break;
  case PS_REGISTER_EF_PROD:
      Type = PARAM_EF_PROD;
	  break;
  default :
    DbgPrintf("INVALID ARGUMENT!");

    Result = false;
  }

  // We're done if (this decoding is meant for (output parameters,
  // | when the input is a value-parameter (already read above) :
  if ((ArgumentType == atOutput)
  || (Type == PARAM_VALUE) )
    return Result;

  // Handle the Channel Designator :
  {
    Channel = (PS_CHANNEL)(Value & PS_CHANNEL_ALPHA);
    if (Channel == PS_CHANNEL_ALPHA)
      // Input comes from alpha portion of input register (valid for (both RGB & alpha portions) :
      Mask = MASK_A;
    else // == PS_CHANNEL_BLUE (for (Alpha step) == PS_CHANNEL_BLUE (for (RGB step) :
      if (aMask == MASK_A)
        // Input comes from b portion of input register (valid for (alpha portion only) :
        Mask = MASK_B; // Note : This is not the same as ARGMOD_BLUE_REPLICATE!
      else
        // Input comes from the RGB portion of the input register (valid for (RGB portion only) :
        Mask = aMask; // Note : Is already put here, but makes this code clearer
  }

  InputMapping = (PS_INPUTMAPPING)(Value & 0xe0);

//    ARGMOD_BIAS,
//
//    ARGMOD_SCALE_X2, ARGMOD_SCALE_BX2, ARGMOD_SCALE_X4, ARGMOD_SCALE_D2,
//
//    ARGMOD_SATURATE,
//
//    ARGMOD_ALPHA_REPLICATE, ARGMOD_BLUE_REPLICATE];

  switch (InputMapping) {
    case PS_INPUTMAPPING_UNSIGNED_IDENTITY:
      Modifiers = ARGMOD_IDENTITY;
	  break;
    case PS_INPUTMAPPING_UNSIGNED_INVERT:
      Modifiers = ARGMOD_INVERT;
	  break;
    case PS_INPUTMAPPING_EXPAND_NORMAL:
    {
      Modifiers = ARGMOD_SCALE_BX2;
      Multiplier = 2.0f * Multiplier;
	  break;
    }
    case PS_INPUTMAPPING_EXPAND_NEGATE:
    {
      Modifiers = ARGMOD_NEGATE;
      Multiplier = -Multiplier;
	  break;
	}
    case PS_INPUTMAPPING_HALFBIAS_NORMAL:
      Modifiers = ARGMOD_BIAS;
	  break;
//    case PS_INPUTMAPPING_HALFBIAS_NEGATE:
//      Modifiers = ARGMOD_IDENTITY; ???
//      break;
    case PS_INPUTMAPPING_SIGNED_IDENTITY:
      Modifiers = ARGMOD_IDENTITY;
	  break;
    case PS_INPUTMAPPING_SIGNED_NEGATE:
    {
      Modifiers = ARGMOD_NEGATE;
      Multiplier = -Multiplier;
	  break;
	}
  }
  return Result;
} // Decode

void PSH_IMD_ARGUMENT::Invert()
{
  if ((Modifiers & ARGMOD_INVERT) == 0)
    Modifiers = Modifiers | ARGMOD_INVERT;
  else
    Modifiers = Modifiers & ~ARGMOD_INVERT;
}

void PSH_IMD_ARGUMENT::Negate()
{
  if ((Modifiers & ARGMOD_NEGATE) == 0)
    Modifiers = Modifiers | ARGMOD_NEGATE;
  else
    Modifiers = Modifiers & ~ARGMOD_NEGATE;
}

/* PSH_INTERMEDIATE_FORMAT */

void PSH_INTERMEDIATE_FORMAT::Initialize(const PSH_OPCODE aOpcode)
{
  int i;

  ZeroMemory(this, sizeof(PSH_INTERMEDIATE_FORMAT)); // TODO : Use constructor (to prevent std::string CommentString leaks)
  Opcode = aOpcode;
  for (i = 0; i < 3; i++)
    Output[i].Multiplier = 1.0f;
  for (i = 0; i < 7; i++)
    Parameters[i].Multiplier = 1.0f;
}

std::string PSH_INTERMEDIATE_FORMAT::ToString()
{
  int i;
  char SeparatorChar;
  std::string Result;

  if (Opcode == PO_COMMENT)
  {
    Result = "; " + CommentString;
    return Result;
  }

  if (IsCombined)
    Result = "+";
  else
    Result = "";

  Result = Result + PSH_OPCODE_DEFS[Opcode].mn + PSH_INST_MODIFIER_Str[Modifier];

  // Output a comma-separated list of output registers :
  SeparatorChar = ' ';
  for (i = 0; i < PSH_OPCODE_DEFS[Opcode]._Out; i++)
  {
    Result = Result + SeparatorChar + Output[i].ToString();
    SeparatorChar = ',';
  }

  // if (this opcode has both output & input, put a space between them :
  if ((PSH_OPCODE_DEFS[Opcode]._Out > 0) && (PSH_OPCODE_DEFS[Opcode]._In > 0))
  {
    Result = Result + ",";
    SeparatorChar = ' ';
  }

  // Output a comma-separated list of parameters :
  for (i = 0; i < PSH_OPCODE_DEFS[Opcode]._In; i++)
  {
    Result = Result + SeparatorChar + Parameters[i].ToString();
    SeparatorChar = ',';
  }

  if ((!CommentString.empty())
  || (PSH_OPCODE_DEFS[Opcode].note != "")) 
    Result = Result + " ; " + PSH_OPCODE_DEFS[Opcode].note + " " + CommentString;
  return Result;
} // ToString

bool PSH_INTERMEDIATE_FORMAT::IsArithmetic()
{
  return (Opcode >= PO_ADD);
}

void PSH_INTERMEDIATE_FORMAT::ScaleOutput(float aFactor)
{
  assert(aFactor > 0.0f);

  if (aFactor == 1.0f)
    return;

  if (aFactor == 0.5f)
  {
    // Half the output modifier :
    switch (Modifier) {
      case INSMOD_X4:
        Modifier = INSMOD_X2;
		break;
	  case INSMOD_X2:
        Modifier = INSMOD_NONE;
		break;
	  case INSMOD_NONE:
        Modifier = INSMOD_D2;
		break;
	}

    return;
  }

  if (aFactor == 2.0f)
  {
    // Double the output modifier :
    switch (Modifier) {
	  case INSMOD_D2:
        Modifier = INSMOD_NONE;
		break;
	  case INSMOD_NONE:
        Modifier = INSMOD_X2;
		break;
	  case INSMOD_X2:
        Modifier = INSMOD_X4;
		break;
    }

    return;
  }
}

bool PSH_INTERMEDIATE_FORMAT::ReadsFromRegister(PSH_ARGUMENT_TYPE aRegType, int aAddress) // overload;
{
  int i;
  bool Result;

  // Check all parameters :
  for (i = 0; i < PSH_OPCODE_DEFS[Opcode]._In; i++)
  {
    // Check if (one of them reads from the given register :
    Result = Parameters[i].IsRegister(aRegType, aAddress);
    if (Result) 
      return true;
  }

  return false;
}

bool PSH_INTERMEDIATE_FORMAT::ReadsFromRegister(PSH_ARGUMENT_TYPE aRegType, int aAddress, DWORD aMask) // overload;
{
  int i;
  bool Result;

  // Check all parameters :
  for (i = 0; i < PSH_OPCODE_DEFS[Opcode]._In; i++)
  {
    // Check if (one of them reads from the given register :
    Result = Parameters[i].IsRegister(aRegType, aAddress, aMask);
    if (Result)
      return true;
  }

  return false;
}

bool PSH_INTERMEDIATE_FORMAT::WritesToRegister(PSH_ARGUMENT_TYPE aRegType, int aAddress) // overload;
{
  int i;
  bool Result;

  // Check the output :
  for (i = 0; i < PSH_OPCODE_DEFS[Opcode]._Out; i++)
  {
    // Check if (one of them writes to the given register :
    Result = Output[i].IsRegister(aRegType, aAddress);
    if (Result)
      return true;
  }

  return false;
}

bool PSH_INTERMEDIATE_FORMAT::WritesToRegister(PSH_ARGUMENT_TYPE aRegType, int aAddress, DWORD aMask) // overload;
{
  int i;
  bool Result;

  // Check the output :
  for (i = 0; i < PSH_OPCODE_DEFS[Opcode]._Out; i++)
  {
    // Check if (one of them writes to the given register :
    Result = Output[i].IsRegister(aRegType, aAddress, aMask);
    if (Result)
      return true;
  }

  return false;
}

void PSH_INTERMEDIATE_FORMAT::SwapParameter(const int Index1, const int Index2)
// Swaps two parameters.
{
  PSH_IMD_ARGUMENT TmpParameters;

  TmpParameters = Parameters[Index1];
  Parameters[Index1] = Parameters[Index2];
  Parameters[Index2] = TmpParameters;
}

void PSH_INTERMEDIATE_FORMAT::XSwapOutput()
// Swaps the two outputs, along with their arguments. Applies only to Xbox opcodes.
{
  PSH_IMD_ARGUMENT TmpOutput;

  // Swap output 0 with 1 :
  TmpOutput = Output[0];
  Output[0] = Output[1];
  Output[1] = TmpOutput;

  // Swap parameters 0 with 2 & & 1 with 3 :
  SwapParameter(0, 2);
  SwapParameter(1, 3);
}

bool PSH_INTERMEDIATE_FORMAT::MoveRemovableParametersRight(const int Index1, const int Index2)
// Swaps discarded (& const) parameters to the right position, to ease later conversions.
{
  bool Result = false;

  if ( (!Parameters[Index1].UsesRegister())
  && (Parameters[Index2].UsesRegister()))
  {
    SwapParameter(Index1, Index2);
    Result = true;
  }
  return Result;
}

bool PSH_INTERMEDIATE_FORMAT::XMoveNonRegisterOutputsRight()
// Swap discards & constants to the right position, to ease later conversions. Applies only to Xbox opcodes.
{
  bool Result = false;

  // First, check if (the left output is discarded, while the second isn't :
  if ( (!Output[0].UsesRegister())
  && (Output[1].UsesRegister())) 
  {
    // Swap the outputs, so the discarded version is positioned rightmost :
    XSwapOutput();
    Result = true;
  }

  // Also try to swap the parameters to the first operation :
  if (MoveRemovableParametersRight(0, 1))
    Result = true;

  // Idem for (the parameters to second operation :
  if (MoveRemovableParametersRight(2, 3))
    Result = true;
  return Result;
}

void PSH_INTERMEDIATE_FORMAT::XCopySecondOpcodeToFirst(const PSH_OPCODE aOpcode)
// Copies second opcode to first position, changing the opcode type on the fly.
{
  Opcode = aOpcode;
  Output[0] = Output[1];
  Parameters[0] = Parameters[2];
  Parameters[1] = Parameters[3];
}

bool PSH_INTERMEDIATE_FORMAT::Decode(DWORD CombinerStageNr, DWORD PSInputs, DWORD PSOutputs, DWORD aMask)
{
  DWORD CombinerOutputFlags;
  int i;

  bool Result = false;
  CombinerStageNr = CombinerStageNr;
  IsCombined = aMask == MASK_A;

  // Decode first two outputs :
  if (Output[0].Decode((PSOutputs >> 4) & 0xF, aMask, atOutput))
    Result = true;
  if (Output[1].Decode((PSOutputs >> 0) & 0xF, aMask, atOutput))
    Result = true;

  // Get the combiner output flags :
  CombinerOutputFlags = (PS_COMBINEROUTPUT)(PSOutputs >> 12);

  // Use that to choose between the four possible operations :
  // - xdd (dot/dot/discard) > calculating AB=A.B & CD=C.D
  // - xdm (dot/mul/discard) > calculating AB=A.B & CD=C*D
  // - xmmc (mul/mul/mux)    > calculating AB=A*B & CD=C*D & Mux=AB?CD
  // - xmma (mul/mul/sum)    > calculating AB=A*B & CD=C*D & Sum=AB+CD
  if ((CombinerOutputFlags & PS_COMBINEROUTPUT_AB_DOT_PRODUCT) > 0) // false=Multiply, true=DotProduct
  {
    if ((CombinerOutputFlags & PS_COMBINEROUTPUT_CD_DOT_PRODUCT) > 0) // false=Multiply, true=DotProduct
      Opcode = PO_XDD;
    else
      Opcode = PO_XDM;

    // Note : All arguments are already in-place for (these two opcodes.

    // No 3rd output; Assert that (PSOutputs >> 8) & 0xF == PS_REGISTER_DISCARD ?
  }
  else
  if ((CombinerOutputFlags & PS_COMBINEROUTPUT_CD_DOT_PRODUCT) > 0) // false=Multiply, true=DotProduct
  {
    // The first operation is a multiply, but the second is a dot-product;
    // There's no opcode for (that, but we can reverse the two & still use XDM :
    Opcode = PO_XDM;
    XSwapOutput();

    // No 3rd output; Assert that (PSOutputs >> 8) & 0xF == PS_REGISTER_DISCARD ?
  }
  else
  {
    if (/*AB_CD_SUM=*/(CombinerOutputFlags & PS_COMBINEROUTPUT_AB_CD_MUX) == 0) // true=AB+CD, false=MUX(AB,CD) based on R0.a
      Opcode = PO_XMMA;
    else
      Opcode = PO_XMMC;

    // This has a 3rd output, set that already :
    if (Output[2].Decode((PSOutputs >> 8) & 0xF, aMask, atOutput))
      Result = true;
  }

  if (Result)
  {
    // Handle the Output Mapping :
    switch (CombinerOutputFlags & 0x38) {
      case PS_COMBINEROUTPUT_BIAS:             Modifier = INSMOD_BIAS; break; // TODO : Fixup occurrances!
	  case PS_COMBINEROUTPUT_SHIFTLEFT_1:      Modifier = INSMOD_X2;   break;
	  case PS_COMBINEROUTPUT_SHIFTLEFT_1_BIAS: Modifier = INSMOD_BX2;  break; // TODO : Fixup occurrances!
	  case PS_COMBINEROUTPUT_SHIFTLEFT_2:      Modifier = INSMOD_X4;   break;
	  case PS_COMBINEROUTPUT_SHIFTRIGHT_1:     Modifier = INSMOD_D2;   break;
    default /*PS_COMBINEROUTPUT_IDENTITY*/:    Modifier = INSMOD_NONE; break;
    }

    if ((CombinerOutputFlags & PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA) > 0) // false=Alpha-to-Alpha, true=Blue-to-Alpha
    {
      // Note : The effect of this flag is not entirely clear - blue to alpha itself is an easy to understand operation,
      // but on what output does it operate? AB? | the mux_sum destination register (which doesn't occur when a dot
      // operation is executed)? What if (AB is discarded, but AB+CD is registered? Also, what happens to the other
      // color channels (R,G & A) in that register? The docs seem to imply that AB itself is not changed (as they
      // state that the alpha portion is not necessarily discarded), which would mean that only the mux_sum output
      // is influenced, but that would imply that this flag has no effect for (dot-products (XDD | XDM)...
      // & if (this is true, how ) the blue-to-alpha flags behave if (present on both AB & CD?

      // TODO : Rayman does this in some shaders, requires a fixup (as output.b is incorrect & not allowed)
      Output[0].Modifiers = Output[0].Modifiers | ARGMOD_BLUE_REPLICATE;
      Output[0].Mask = MASK_B;
      // TODO Handle blue-to-alpha flag (only valid for (RGB)
      // Note : We can't use the '+ ' prefix, as the blue channel is not determined yet!
      // Note 2: Pixel shader 1.1-1.3 'blue replicate' on source, uses an alpha destination write mask.
    }

    if ((CombinerOutputFlags & PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA) > 0) // false=Alpha-to-Alpha, true=Blue-to-Alpha
    {
      Output[1].Modifiers = Output[1].Modifiers | ARGMOD_BLUE_REPLICATE;
      Output[1].Mask = MASK_B;
    }

    // Decode all four inputs :
    for (i = 0; i < PSH_OPCODE_DEFS[Opcode]._In; i++)
      Parameters[i].Decode((PSInputs >> ((3-i) * 8)) & 0xFF, aMask, atInput);
  }
  return Result;
} // Decode

bool PSH_INTERMEDIATE_FORMAT::DecodeFinalCombiner(DWORD aPSFinalCombinerInputsABCD, DWORD aPSFinalCombinerInputsEFG)
{
  int i;
// Note : The sign bit is lost upon input to the final combiner!

//  The final combiner performs the following operations :
//
//    prod register == E*F                // PS_REGISTER_EF_PROD, useable in A,B,C,D,G
//
//    rgbout        == A*B + (1-A)*C + D  // lrp tmp.rgb, A, B, C       // Note : tmp can be r0 if ([A,B,C,D] * r0 == []
//                                       // add r0.rgb, tmp.rgb, D.rgb // Otherwise use a writable register from A;B | C
//
//    alphaout      == G.a                // mov r0.a, G.a              // Not necessary if (G == r0
//
//    (also the final combiner can read PS_REGISTER_V1R0_SUM, which is equal to v1 + r0)
//  Normal optimizations apply, like when A == PS_REGISTER_ZERO, all we have left is C + D (add r0.rgb, C.rgb, D.rgb)
//  Also, if (D == PS_REGISTER_ZERO, the add can be changed into a mov (if (the result isn't already in r0.rgb)

  // Note : Previously, XSokoban lost it's font rendering when the final combiner was emitted,
  // when disabled, the font reappeared (in various colors). This was because constants where
  // not properly set locally.

  Opcode = PO_XFC;
  CombinerStageNr = XFC_COMBINERSTAGENR;

  // Decode A,B,C & D :
  for (i = 0; i < 4; i++)
    Parameters[i].Decode((aPSFinalCombinerInputsABCD >> ((3-i) * 8)) & 0xFF, MASK_RGB/*?*/, atFinalCombiner);

  // Decode E,F & G :
  for (i = 0; i < 3; i++)
    Parameters[4+i].Decode((aPSFinalCombinerInputsEFG >> ((3-i) * 8)) & 0xFF, MASK_RGB/*?*/, atFinalCombiner);

  return true;
}

#if 0 // Dxbx

{ PSH_XBOX_SHADER }

function PSH_XBOX_SHADER.ToString: std::string;
var
  i: int;
{
  // First things first, set the pixel shader version
  // 1.1 allows reading from 2 textures (which we use in 'cnd') & reading from the .b (blue) channel
  // 1.3 allows the use of texm3x2depth (which can occur sometimes)
  // 2.0 allows up to r12, c32, t8 & s16 (requires Direct3D9)
  // 3.0 allows up to r32, c224, v10 (instead of t via dcl), s16 & vFace (which can ) two-sided lighting)
{$IFDEF DXBX_USE_PS_3_0}
  Result == 'ps_3_0'#13#10;
{$ELSE}
 {$IFDEF DXBX_USE_PS_2_0}
  Result == 'ps_2_0'#13#10;
 {$ELSE}
  Result == 'ps.1.3'#13#10;
 {$ENDIF}
{$ENDIF}
  for (i == 0 to IntermediateCount-1 )
    Result == Result + Intermediate[i].ToString + #13#10;
}

void PSH_XBOX_SHADER.Log(const PhaseStr: std::string);
{
  if (MayLog(lfUnit) 
  {
    DbgPrintf('New decoding - %s :', [PhaseStr]);
    DbgPrintf(ToString);
  }
}

function PSH_XBOX_SHADER.NewIntermediate(): PPSH_INTERMEDIATE_FORMAT;
{
  Result == &Intermediate[IntermediateCount];
  Result.Initialize(PO_COMMENT);
  ++(IntermediateCount);
}

void PSH_XBOX_SHADER.InsertIntermediate(pIntermediate: PPSH_INTERMEDIATE_FORMAT; Index: int);
var
  i: int;
{
  i == IntermediateCount - 1;
  while i >= Index )
  {
    Intermediate[i + 1] == Intermediate[i];
    --(i);
  }

  Intermediate[Index] == pIntermediate^;
  ++(IntermediateCount);
}

void PSH_XBOX_SHADER.DeleteIntermediate(Index: int);
var
  i: int;
{
  for (i == Index to IntermediateCount - 2 )
    Intermediate[i] == Intermediate[i + 1];

  --(IntermediateCount);
}

void PSH_XBOX_SHADER.DeleteLastIntermediate;
{
  if (IntermediateCount > 0 
    DeleteIntermediate(IntermediateCount - 1);
}

function PSH_XBOX_SHADER.OriginalToString(pPSDef: PX_D3DPIXELSHADERDEF): std::string;
{
  Result =Format('PSAphaInputs[8]              == 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X'#13#10 +
                  'PSFinalCombinerInputsABCD    == 0x%.08X'#13#10 +
                  'PSFinalCombinerInputsEFG     == 0x%.08X'#13#10 +
                  'PSConstant0[8]               == 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X'#13#10 +
                  'PSConstant1[8]               == 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X'#13#10 +
                  'PSAlphaOutputs[8]            == 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X'#13#10 +
                  'PSRGBInputs[8]               == 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X'#13#10 +
                  'PSCompareMode                == 0x%.08X'#13#10 +
                  'PSFinalCombinerConstant0     == 0x%.08X'#13#10 +
                  'PSFinalCombinerConstant1     == 0x%.08X'#13#10 +
                  'PSRGBOutputs[8]              == 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X 0x%.08X'#13#10 +
                  'PSCombinerCount              == 0x%.08X'#13#10 +
                  'PSTextureModes               == 0x%.08X'#13#10 +
                  'PSDotMapping                 == 0x%.08X'#13#10 +
                  'PSInputTexture               == 0x%.08X'#13#10 +
                  'PSC0Mapping                  == 0x%.08X'#13#10 +
                  'PSC1Mapping                  == 0x%.08X'#13#10 +
                  'PSFinalCombinerConstants     == 0x%.08X'#13#10,
                  [pPSDef.PSAlphaInputs[0], pPSDef.PSAlphaInputs[1], pPSDef.PSAlphaInputs[2], pPSDef.PSAlphaInputs[3],
                  pPSDef.PSAlphaInputs[4], pPSDef.PSAlphaInputs[5], pPSDef.PSAlphaInputs[6], pPSDef.PSAlphaInputs[7],
                  pPSDef.PSFinalCombinerInputsABCD,
                  pPSDef.PSFinalCombinerInputsEFG,
                  pPSDef.PSConstant0[0], pPSDef.PSConstant0[1], pPSDef.PSConstant0[2], pPSDef.PSConstant0[3],
                  pPSDef.PSConstant0[4], pPSDef.PSConstant0[5], pPSDef.PSConstant0[6], pPSDef.PSConstant0[7],
                  pPSDef.PSConstant1[0], pPSDef.PSConstant1[1], pPSDef.PSConstant1[2], pPSDef.PSConstant1[3],
                  pPSDef.PSConstant1[4], pPSDef.PSConstant1[5], pPSDef.PSConstant1[6], pPSDef.PSConstant1[7],
                  pPSDef.PSAlphaOutputs[0], pPSDef.PSAlphaOutputs[1], pPSDef.PSAlphaOutputs[2], pPSDef.PSAlphaOutputs[3],
                  pPSDef.PSAlphaOutputs[4], pPSDef.PSAlphaOutputs[5], pPSDef.PSAlphaOutputs[6], pPSDef.PSAlphaOutputs[7],
                  pPSDef.PSRGBInputs[0], pPSDef.PSRGBInputs[1], pPSDef.PSRGBInputs[2], pPSDef.PSRGBInputs[3],
                  pPSDef.PSRGBInputs[4], pPSDef.PSRGBInputs[5], pPSDef.PSRGBInputs[6], pPSDef.PSRGBInputs[7],
                  pPSDef.PSCompareMode,
                  pPSDef.PSFinalCombinerConstant0,
                  pPSDef.PSFinalCombinerConstant1,
                  pPSDef.PSRGBOutputs[0], pPSDef.PSRGBOutputs[1], pPSDef.PSRGBOutputs[2], pPSDef.PSRGBOutputs[3],
                  pPSDef.PSRGBOutputs[4], pPSDef.PSRGBOutputs[5], pPSDef.PSRGBOutputs[6], pPSDef.PSRGBOutputs[7],
                  pPSDef.PSCombinerCount,
                  pPSDef.PSTextureModes,
                  pPSDef.PSDotMapping,
                  pPSDef.PSInputTexture,
                  pPSDef.PSC0Mapping,
                  pPSDef.PSC1Mapping,
                  pPSDef.PSFinalCombinerConstants]);
}

function PSH_XBOX_SHADER.Decode(pPSDef: PX_D3DPIXELSHADERDEF): PSH_RECOMPILED_SHADER;
var
  i: int;
{
  ZeroMemory(&Result, sizeof(Result));
  Result.PSDef == pPSDef^;

  // Azurik likes to create & destroy the same shader every frame! O_o
  LogFlags == lfUnit;
  if (IsRunning(TITLEID_AZURIK) 
    LogFlags == LogFlags | lfExtreme;

  ZeroMemory(&Self, sizeof(Self));

  for (i == 0 to X_D3DTS_STAGECOUNT-1 )
  {
    PSTextureModes[i] == PS_TEXTUREMODES((pPSDef.PSTextureModes >> (i*5)) & 0x1F);
    PSCompareMode[i] == (pPSDef.PSCompareMode >> (i*4)) & 0xF;
  }

  PSDotMapping[0] == PS_DOTMAPPING(0);
  PSDotMapping[1] == PS_DOTMAPPING((pPSDef.PSDotMapping >> 0) & 0x7);
  PSDotMapping[2] == PS_DOTMAPPING((pPSDef.PSDotMapping >> 4) & 0x7);
  PSDotMapping[3] == PS_DOTMAPPING((pPSDef.PSDotMapping >> 8) & 0x7);

  PSInputTexture[0] == -1; // Stage 0 has no predecessors
  PSInputTexture[1] == 0; // Stage 1 can only use stage 0
  PSInputTexture[2] == (pPSDef.PSInputTexture >> 16) & 0x1; // Stage 2 can use stage 0 | 1
  PSInputTexture[3] == (pPSDef.PSInputTexture >> 20) & 0x3; // Stage 3 can only use stage 0, 1 | 2

  NumberOfCombiners == (pPSDef.PSCombinerCount >> 0) & 0xF;
  CombinerCountFlags == (pPSDef.PSCombinerCount >> 8);

  CombinerMuxesOnMsb == (CombinerCountFlags & PS_COMBINERCOUNT_MUX_MSB) > 0;
  CombinerHasUniqueC0 == (CombinerCountFlags & PS_COMBINERCOUNT_UNIQUE_C0) > 0;
  CombinerHasUniqueC1 == (CombinerCountFlags & PS_COMBINERCOUNT_UNIQUE_C1) > 0;

  // Backwards compatible decoding (purely for (logging) :
  {
    for (i == 0 to X_PSH_COMBINECOUNT - 1 )
    {
      Combiners[i].RGB.Decode(pPSDef.PSRGBInputs[i], pPSDef.PSRGBOutputs[i]);
      Combiners[i].Alpha.Decode(pPSDef.PSAlphaInputs[i], pPSDef.PSAlphaOutputs[i], {IsAlpha=}true);
    }

    FinalCombiner.Decode(pPSDef.PSFinalCombinerInputsABCD, pPSDef.PSFinalCombinerInputsEFG, pPSDef.PSFinalCombinerConstants);
  }

  for (i == 0 to NumberOfCombiners - 1 )
  {
    // Check that the RGB & Alpha inputs ) the same operation :
    if ( ((pPSDef.PSRGBInputs[i] & PS_NoChannelsMask) == (pPSDef.PSAlphaInputs[i] & PS_NoChannelsMask))
    // Check if (all RGB channels are set to read from PS_CHANNEL_RGB :
    & ((pPSDef.PSRGBInputs[i] & PS_AlphaChannelsMask) == 0)
    // Check if (all Alpha channels are set to read from PS_CHANNEL_ALPHA :
    & ((pPSDef.PSAlphaInputs[i] & PS_AlphaChannelsMask) == PS_AlphaChannelsMask)
    // Check that RGB & Alpha output to the same register(s) :
    & (pPSDef.PSRGBOutputs[i] == pPSDef.PSAlphaOutputs[i]) 
    {
      // In this case, we can convert RGB & Alpha together :
      if (not NewIntermediate.Decode(i, pPSDef.PSRGBInputs[i], pPSDef.PSRGBOutputs[i], MASK_RGBA) 
        DeleteLastIntermediate;
    }
    else
    {
      // Otherwise, we need to convert RGB & Alpha separately :
      if (not NewIntermediate.Decode(i, pPSDef.PSRGBInputs[i], pPSDef.PSRGBOutputs[i], MASK_RGB) 
        DeleteLastIntermediate;

      if (not NewIntermediate.Decode(i, pPSDef.PSAlphaInputs[i], pPSDef.PSAlphaOutputs[i], MASK_A) 
        DeleteLastIntermediate;
    }
  }

  if ((pPSDef.PSFinalCombinerInputsABCD > 0)
  | (pPSDef.PSFinalCombinerInputsEFG > 0) 
    if (NewIntermediate.DecodeFinalCombiner(pPSDef.PSFinalCombinerInputsABCD, pPSDef.PSFinalCombinerInputsEFG) 
    {
      FinalCombinerFlags == PS_FINALCOMBINERSETTING((pPSDef.PSFinalCombinerInputsEFG >> 0) & 0xFF);
//    dwPS_GLOBALFLAGS == (pPSDef.PSFinalCombinerConstants >> 8) & 0x1;
    }
    else
      DeleteLastIntermediate;

  // Dump the contents of the PixelShader def
  if (MayLog(LogFlags) 
    // dump pixel shader definition to std::string
    XTL_DumpPixelShaderToFile(pPSDef);

  if (MayLog(LogFlags) 
  {
    // print relevant contents to the debug console
    DbgPrintf(DecodedToString(pPSDef));
  }

  // TODO:
  // - Insert tex* & def instructions

  Log('Parse result');

  if (MoveRemovableParametersRight 
    Log('MoveRemovableParametersRight');

  if (RemoveNops() 
    Log('RemoveNops');

  if (RemoveUselessWrites 
    Log('RemoveUselessWrites');

  if (ConvertConstantsToNative(pPSDef, {Recompiled=}Result) 
    Log('ConvertConstantsToNative');

  ConvertXboxOpcodesToNative(pPSDef);
  Log('ConvertXboxOpcodesToNative');

  if (RemoveUselessWrites  // twice!
    Log('RemoveUselessWrites');

  // Resolve all differences :
  if (FixupPixelShader 
    Log('FixupPixelShader');

  // Handle Texture declarations :
  if (DecodeTextureModes(pPSDef) 
    Log('DecodeTextureModes');

  if (FixInvalidSrcSwizzle 
    Log('FixInvalidSrcSwizzle');

  if (FixMissingR0a 
    Log('FixMissingR0a');

  if (FixCoIssuedOpcodes 
    Log('FixCoIssuedOpcodes');

  Log('} result');

  Result.NewShaderStr == ToString;
}

function PSH_XBOX_SHADER.DecodedToString(pPSDef: PX_D3DPIXELSHADERDEF): std::string;
// Branch:shogun  Revision:0.8.1-Pre2  Translator:PatrickvL  Done:100
// print relevant contents to the debug console

  void _AddStr(const aStr: std::string); overload;
  {
    Result == Result + aStr + #13#10;
  }

  void _AddStr(const aStr: std::string; Args of const); overload;
  {
    _AddStr(DxbxFormat(aStr, Args));
  }

var
  i: int;
{
  Result == '';
  // Show the contents to the user
  _AddStr(#13#10'-----PixelShader Definition Contents-----');
  _AddStr(OriginalToString(pPSDef));

  if ((pPSDef.PSTextureModes > 0) 
  {
    _AddStr(#13#10'PSTextureModes ->'); // Texture addressing modes
    _AddStr('Stage 0: %s', [PS_TextureModesStr[PSTextureModes[0]]]);
    _AddStr('Stage 1: %s', [PS_TextureModesStr[PSTextureModes[1]]]);
    _AddStr('Stage 2: %s', [PS_TextureModesStr[PSTextureModes[2]]]);
    _AddStr('Stage 3: %s', [PS_TextureModesStr[PSTextureModes[3]]]);
  }

  if ((pPSDef.PSDotMapping > 0)  // Input mapping for (dot product modes
  {
    _AddStr(#13#10'PSDotMapping ->');
    _AddStr('Stage 1: %s', [PS_DotMappingStr[PSDotMapping[1]]]);
    _AddStr('Stage 2: %s', [PS_DotMappingStr[PSDotMapping[2]]]);
    _AddStr('Stage 3: %s', [PS_DotMappingStr[PSDotMapping[3]]]);
  }

  if ((pPSDef.PSCompareMode > 0)  // Compare modes for (clipplane texture mode
  {
    _AddStr(#13#10'PSCompareMode ->');
    _AddStr('Stage 0: %s', [PS_CompareModeStr[iif(PSCompareMode[0] == 0, 0, 1)]]);
    _AddStr('Stage 1: %s', [PS_CompareModeStr[iif(PSCompareMode[1] == 0, 2, 3)]]);
    _AddStr('Stage 2: %s', [PS_CompareModeStr[iif(PSCompareMode[2] == 0, 4, 5)]]);
    _AddStr('Stage 3: %s', [PS_CompareModeStr[iif(PSCompareMode[3] == 0, 6, 7)]]);
  }

  if ((pPSDef.PSInputTexture > 0)  // Texture source for (some texture modes
  {
    _AddStr(#13#10'PSInputTexture ->');
    _AddStr('Stage 1: %d', [PSInputTexture[1]]);
    _AddStr('Stage 2: %d', [PSInputTexture[2]]);
    _AddStr('Stage 3: %d', [PSInputTexture[3]]);
  }

  if ((pPSDef.PSCombinerCount > 0)  // Active combiner count (Stages 0-7)
  {
    _AddStr(#13#10'PSCombinerCount ->');
    _AddStr('Combiners: %d', [NumberOfCombiners]);
    _AddStr('Mux:       %s', [PS_CombinerCountFlagsStr[iif(CombinerCountFlags & PS_COMBINERCOUNT_MUX_MSB == 0, 0, 1)]]);
    _AddStr('C0:        %s', [PS_CombinerCountFlagsStr[iif(CombinerCountFlags & PS_COMBINERCOUNT_UNIQUE_C0 == 0, 2, 3)]]);
    _AddStr('C1:        %s', [PS_CombinerCountFlagsStr[iif(CombinerCountFlags & PS_COMBINERCOUNT_UNIQUE_C1 == 0, 4, 5)]]);
  }

  // Dxbx additions from here onwards :

  if (NumberOfCombiners > 0 
  for (i == 0 to NumberOfCombiners-1 ) // Loop over all combiner stages
  {
    _AddStr(#13#10);

    _AddStr('PSRGBOutputs[%d] AB: %s', [i, Combiners[i].RGB.OutputSUM.OutputAB.DecodedToString()]);
    _AddStr('PSRGBOutputs[%d] CD: %s', [i, Combiners[i].RGB.OutputSUM.OutputCD.DecodedToString()]);
    _AddStr('PSRGBOutputs[%d] SUM: %s', [i, Combiners[i].RGB.OutputSUM.DecodedToString()]);
    _AddStr('PSRGBOutputs[%d] flags: %s', [i, PSCombinerOutputFlagsToStr(Combiners[i].RGB.CombinerOutputFlags, {IsAlpha=}false)]);

    _AddStr(#13#10);
    _AddStr('PSRGBInputs[%d] A: %s', [i, Combiners[i].RGB.OutputSUM.OutputAB.Input1.DecodedToString()]);
    _AddStr('PSRGBInputs[%d] B: %s', [i, Combiners[i].RGB.OutputSUM.OutputAB.Input2.DecodedToString()]);
    _AddStr('PSRGBInputs[%d] C: %s', [i, Combiners[i].RGB.OutputSUM.OutputCD.Input1.DecodedToString()]);
    _AddStr('PSRGBInputs[%d] D: %s', [i, Combiners[i].RGB.OutputSUM.OutputCD.Input2.DecodedToString()]);

    _AddStr(#13#10);
    _AddStr('PSAlphaOutputs[%d] AB: %s', [i, Combiners[i].Alpha.OutputSUM.OutputAB.DecodedToString()]);
    _AddStr('PSAlphaOutputs[%d] CD: %s', [i, Combiners[i].Alpha.OutputSUM.OutputCD.DecodedToString()]);
    _AddStr('PSAlphaOutputs[%d] SUM: %s', [i, Combiners[i].Alpha.OutputSUM.DecodedToString()]);
    _AddStr('PSAlphaOutputs[%d] flags: %s', [i, PSCombinerOutputFlagsToStr(Combiners[i].Alpha.CombinerOutputFlags, {IsAlpha=}true)]);

    _AddStr(#13#10);
    _AddStr('PSAlphaInputs[%d] A: %s', [i, Combiners[i].Alpha.OutputSUM.OutputAB.Input1.DecodedToString()]);
    _AddStr('PSAlphaInputs[%d] B: %s', [i, Combiners[i].Alpha.OutputSUM.OutputAB.Input2.DecodedToString()]);
    _AddStr('PSAlphaInputs[%d] C: %s', [i, Combiners[i].Alpha.OutputSUM.OutputCD.Input1.DecodedToString()]);
    _AddStr('PSAlphaInputs[%d] D: %s', [i, Combiners[i].Alpha.OutputSUM.OutputCD.Input2.DecodedToString()]);

    _AddStr(#13#10);
    _AddStr('PSConstant0[%d] : %x', [i, pPSDef.PSConstant0[i]]); // C0 for (each stage
    _AddStr('PSConstant1[%d] : %x', [i, pPSDef.PSConstant1[i]]); // C1 for (each stage
  }

  if ((pPSDef.PSFinalCombinerInputsABCD > 0)
  | (pPSDef.PSFinalCombinerInputsEFG  > 0)  // Final combiner inputs
  {
    _AddStr(#13#10'PSFinalCombinerConstant0 : %x', [pPSDef.PSFinalCombinerConstant0]); // C0 in final combiner
    _AddStr('PSFinalCombinerConstant1 : %x', [pPSDef.PSFinalCombinerConstant1]); // C1 in final combiner

    _AddStr(#13#10'PSFinalCombinerInputsABCD ->');
    _AddStr('Input A: %s', [FinalCombiner.InputA.DecodedToString()]);
    _AddStr('Input B: %s', [FinalCombiner.InputB.DecodedToString()]);
    _AddStr('Input C: %s', [FinalCombiner.InputC.DecodedToString()]);
    _AddStr('Input D: %s', [FinalCombiner.InputD.DecodedToString()]);

    _AddStr(#13#10'PSFinalCombinerInputsEFG ->');
    _AddStr('Input E: %s', [FinalCombiner.InputE.DecodedToString()]);
    _AddStr('Input F: %s', [FinalCombiner.InputF.DecodedToString()]);
    _AddStr('Input G: %s', [FinalCombiner.InputG.DecodedToString()]);
    _AddStr('Final combiner setting: %s', [PSFinalCombinerSettingToStr(Ord(FinalCombiner.FinalCombinerFlags))]);

    _AddStr(#13#10'PSFinalCombinerConstants ->'); // Final combiner constant mapping
    _AddStr('Offset of D3D constant for (C0: %d', [FinalCombiner.FinalCombinerC0Mapping]);
    _AddStr('Offset of D3D constant for (C1: %d', [FinalCombiner.FinalCombinerC1Mapping]);
    _AddStr('Adjust texture flag: %s', [PS_GlobalFlagsStr[PS_GLOBALFLAGS(FinalCombiner.dwPS_GLOBALFLAGS)]]);
  }

  _AddStr(#13#10);
}

function PSH_XBOX_SHADER.DecodeTextureModes(pPSDef: PX_D3DPIXELSHADERDEF): bool;

  function _NextIs2D(Stage: int): bool;
  {
    if (Stage < X_D3DTS_STAGECOUNT-1 
      Result == PSTextureModes[Stage + 1] in [PS_TEXTUREMODES_DOT_ST, PS_TEXTUREMODES_DOT_ZW]
    else
      Result == false;
  }

var
  InsertPos: int;
  Ins: PSH_INTERMEDIATE_FORMAT;
  Stage: int;
{
  Result == false;

  InsertPos == -1;
  repeat
    ++(InsertPos);
  until Intermediate[InsertPos].Opcode != PO_DEF;

{$IFDEF DXBX_USE_PS_2_0}
  Ins.Initialize(PO_DCL);
  for (Stage == 0 to X_D3DTS_STAGECOUNT-1 )
  {
    if (PSTextureModes[Stage] != PS_TEXTUREMODES_NONE 
    {
      Ins.Output[0].SetRegister(PARAM_T, Stage, MASK_RGBA);
      InsertIntermediate(&Ins, InsertPos);
      ++(InsertPos);
      Result == true;
    }
  }
{$ENDIF}

  Ins.Initialize(PO_TEX);
  for (Stage == 0 to X_D3DTS_STAGECOUNT-1 )
  {
    // TODO : Apply conversions when PS_GLOBALFLAGS_TEXMODE_ADJUST is set (but ... how to check the texture type? read D3DRS_PSTEXTUREMODES?)

    // Convert the texture mode to a texture addressing instruction :
    case PSTextureModes[Stage] of // input == q,s,t,r (same layout as a,r,g,b, also known as w,x,y,z)
{$IFNDEF DXBX_USE_PS_2_0}
      PS_TEXTUREMODES_PROJECT2D: Ins.Opcode == PO_TEX; // argb == texture(r/q, s/q)      TODO : Apply the division via D3DTOP_BUMPENVMAP ?
      PS_TEXTUREMODES_PROJECT3D: Ins.Opcode == PO_TEX; // argb == texture(r/q, s/q, t/q) Note : 3d textures are sampled using PS_TEXTUREMODES_CUBEMAP
      PS_TEXTUREMODES_CUBEMAP: Ins.Opcode == PO_TEX; // argb == cubemap(r/q, s/q, t/q)
{$ENDIF}
      PS_TEXTUREMODES_PASSTHRU: Ins.Opcode == PO_TEXCOORD;
      PS_TEXTUREMODES_CLIPPLANE: Ins.Opcode == PO_TEXKILL;
      PS_TEXTUREMODES_BUMPENVMAP: Ins.Opcode == PO_TEXBEM;
      PS_TEXTUREMODES_BUMPENVMAP_LUM: Ins.Opcode == PO_TEXBEML;
//    PS_TEXTUREMODES_BRDF: Ins.Opcode == PO_TEXBRDF; // Note : Not supported by Direct3D8 ?
      PS_TEXTUREMODES_DOT_ST: Ins.Opcode == PO_TEXM3X2TEX;
      PS_TEXTUREMODES_DOT_ZW: Ins.Opcode == PO_TEXM3X2DEPTH; // Note : requires ps.1.3 & a preceding texm3x2pad
//    PS_TEXTUREMODES_DOT_RFLCT_DIFF: Ins.Opcode == PO_TEXM3X3DIFF; // Note : Not supported by Direct3D8 ?
      PS_TEXTUREMODES_DOT_RFLCT_SPEC: Ins.Opcode == PO_TEXM3X3VSPEC;
      PS_TEXTUREMODES_DOT_STR_3D: Ins.Opcode == PO_TEXM3X3TEX; // Note : Uses a 3d texture
      PS_TEXTUREMODES_DOT_STR_CUBE: Ins.Opcode == PO_TEXM3X3TEX; // Note : Uses a cube texture
      PS_TEXTUREMODES_DPNDNT_AR: Ins.Opcode == PO_TEXREG2AR;
      PS_TEXTUREMODES_DPNDNT_GB: Ins.Opcode == PO_TEXREG2GB;
      PS_TEXTUREMODES_DOTPRODUCT:
        if (_NextIs2D(Stage) 
          Ins.Opcode == PO_TEXM3X2PAD
        else
          Ins.Opcode == PO_TEXM3X3PAD;
      PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST: Ins.Opcode == PO_TEXM3X3SPEC; // Note : Needs 3 arguments!
    else
      continue;
    }

    Ins.Output[0].SetRegister(PARAM_T, Stage, 0);

    // for (those texture modes that need it, add the source stage as argument :
    if (PSH_OPCODE_DEFS[Ins.Opcode]._In >= 1 
    {
      Ins.Parameters[0].SetRegister(PARAM_T, PSInputTexture[Stage], 0);

      case PSDotMapping[Stage] of
        PS_DOTMAPPING_MINUS1_TO_1_D3D:
          Ins.Parameters[0].Modifiers == [ARGMOD_SCALE_BX2];
      }
    }

    if (PSH_OPCODE_DEFS[Ins.Opcode]._In >= 2 
    {
      // Add the third argument :
      case PSTextureModes[Stage] of
        PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST:
        {
          Ins.Parameters[1].SetRegister(PARAM_C, 0, 0);
          Ins.CommentString == 'Dxbx guess'; // TODO : Where ) we get the 3rd argument to this?
        }
      }
    }

//    // Warn about unprocessed flag :
//    if ((dwPS_GLOBALFLAGS & Ord(PS_GLOBALFLAGS_TEXMODE_ADJUST)) > 0 
//      Ins.CommentString == Ins.CommentString + ' PS_GLOBALFLAGS_TEXMODE_ADJUST unhandled!';

    InsertIntermediate(&Ins, InsertPos);
    ++(InsertPos);
    Result == true;
  }
}

function PSH_XBOX_SHADER.MoveRemovableParametersRight: bool;
var
  i: int;
{
  Result == false;

  // for (all opcodes, try to put constant & discarded arguments in the rightmost slot, to ease following analysis :
  i == IntermediateCount;
  while i > 0 )
  {
    --(i);

    case Intermediate[i].Opcode of
//      PO_SUB // 1-x is not the same as x-1, but can still be reduced - see SimplifySUB
      PO_ADD,
      PO_DP3,
      PO_DP4,
      PO_MUL: // All these opcodes have two swappable parameters, so try that :
        if (Intermediate[i].MoveRemovableParametersRight(0, 1) 
          Result == true;

      PO_XMMA,
      PO_XMMC,
      PO_XDD:
        if (Intermediate[i].XMoveNonRegisterOutputsRight() 
          Result == true;

      PO_XDM:
      {
        // Parameters may be swapped for (both dot & mul,
        // but the opcodes themselves may not, as we handle
        // both XDM operations separately below :
        if (Intermediate[i].MoveRemovableParametersRight(0, 1) 
          Result == true;

        if (Intermediate[i].MoveRemovableParametersRight(2, 3) 
          Result == true;
      }
    }
  }
} // MoveRemovableParametersRight

function PSH_XBOX_SHADER.ConvertConstantsToNative(pPSDef: PX_D3DPIXELSHADERDEF; var Recompiled: PSH_RECOMPILED_SHADER): bool;
var
  i, j: int;
  Cur: PPSH_INTERMEDIATE_FORMAT;
  CurArg: PPSH_IMD_ARGUMENT;
  NativeConstInUse [0..PSH_PC_MAX_C_REGISTER_COUNT-1] of bool;
  OriginalConstantNr: Int16;
  EmittedNewConstant: bool;
  NewIns: PSH_INTERMEDIATE_FORMAT;

  void _SetColor(var NewIns: PSH_INTERMEDIATE_FORMAT; ConstColor: D3DCOLOR);
  var
    XColor: D3DCOLORVALUE;
  {
    // Colors are defined in RGBA format, & range 0.0 - 1.0 (negative values
    // can be obtained by supplying PS_INPUTMAPPING_SIGNED_NEGATE to the combiner
    // that reads from these constants).
    XColor == D3DXColorFromDWord(ConstColor);
    NewIns.Parameters[0].SetConstValue(XColor.r);
    NewIns.Parameters[1].SetConstValue(XColor.g);
    NewIns.Parameters[2].SetConstValue(XColor.b);
    NewIns.Parameters[3].SetConstValue(XColor.a);
  }

  // Try to fixup constants above the limit (c7 for (PS.1.3) :
  function _MapConstant(ConstNr: int): int;
  {
    // 1-to-1 mapping for (constants that can be supported native (if (not used already) :
    if ((ConstNr < PSH_PC_MAX_C_REGISTER_COUNT) & (not NativeConstInUse[ConstNr]) 
    {
      Result == ConstNr;
      return;
    }

    // Assign not-yet-defined constants bottom-to-up :
    Result == 0;
    while Result < PSH_PC_MAX_C_REGISTER_COUNT )
    {
      if (not NativeConstInUse[Result] 
        return;

      ++(Result);
    }

    // Unresolved - fallback to 1st constant :
    if (Result >= PSH_PC_MAX_C_REGISTER_COUNT 
      Result == 0;

    EmuWarning('; Too many constants to emulate, this pixel shader will give unexpected output!');
  }

  function _HandleConst(XboxConst: int): int;
  var
    NativeConst: int;
  {
    if (not Recompiled.ConstInUse[XboxConst] 
    {
      // Determine & remember a new mapping to native :
      NativeConst == _MapConstant(XboxConst);
      NativeConstInUse[NativeConst] == true;
      Recompiled.ConstMapping[XboxConst] == NativeConst;
      Recompiled.ConstInUse[XboxConst] == true;
      // Make sure we can check this is a new constant (so we can emit a constant declaration
      // for (any final combiner constants - because those cannot be set via SetPixelShaderConstant) :
      EmittedNewConstant == true;
    }

    // Return the (previously) determined mapping :
    Result == Recompiled.ConstMapping[XboxConst];
  }

{
  Result == false;

  // Note : Recompiled.ConstMapping & Recompiled.ConstInUse[i] are still empty here.
  for (i == 0 to PSH_PC_MAX_C_REGISTER_COUNT - 1 )
    NativeConstInUse[i] == false;

  // Loop over all opcodes to update the constant-indexes (Xbox uses C0 & C1 in each combiner) :
  for (i == 0 to IntermediateCount - 1 )
  {
    // Loop over this opcodes' input arguments :
    Cur == &(Intermediate[i]);
    for (j == 0 to PSH_OPCODE_DEFS[Cur.Opcode]._In - 1 )
    {
      // Only handle arguments that address a constant register :
      CurArg == &(Cur.Parameters[j]);

      // The Fog register is not supported on PC so we convert it to a constant too :
      // (But only if (the MASK is not solely accessing the alpha-channel - we don't support that)
      if ((CurArg.Type == PARAM_FOG) 
      {
        if ((CurArg.Mask != MASK_A) 
        {
          CurArg.Type == PARAM_C;
          CurArg.Address == _HandleConst(PSH_XBOX_CONSTANT_FOG);
          CurArg.Mask == CurArg.Mask & (not MASK_A);
        }
        else
        {
          // Until we can get Alpha fog from the vertex shader somehow,
          // set it to a constant value, so these shaders (like appearing
          // in Dolphin samples) still compile & give reasonable output :
          CurArg.SetConstValue(1.0);
          Cur.CommentString == 'FOG.a not emulated, using 1.';
        }

        continue;
      }

      if (CurArg.Type != PARAM_C 
        continue;

      // Make sure we can detect new constants (& if (it was C0 | C1),
      // as we need this for (fixing up final combiner constants :
      EmittedNewConstant == false;
      OriginalConstantNr == CurArg.Address;

      // for (each constant being addressed, we find out which Xbox constant it is,
      // & map it to a native constant (as far as we have space for (them) :
      case CurArg.Address of
        0: // Handle C0 (if (present) :
        {
          // The final combiner has a separate C0 constant :
          if (Cur.CombinerStageNr == XFC_COMBINERSTAGENR 
            CurArg.Address == _HandleConst(PSH_XBOX_CONSTANT_FC0)
          else
          {
            // See if (C0 has a unique index per combiner stage :
            if (CombinerHasUniqueC0 
              // C0 actually ranges from c0 to c7, one for (each possible combiner stage (X_D3DRS_PSCONSTANT0_0..X_D3DRS_PSCONSTANT0_7) :
              CurArg.Address == _HandleConst(Cur.CombinerStageNr)
            else
              // Non-unique just reads the same C0 in every stage :
              CurArg.Address == _HandleConst(0);
          }
        }

        1: // Handle C1 (if (present) :
        {
          // The final combiner has a separate C1 constant :
          if (Cur.CombinerStageNr == XFC_COMBINERSTAGENR 
            CurArg.Address == _HandleConst(PSH_XBOX_CONSTANT_FC1)
          else
          {
            // See if (C1 has a unique index per combiner stage :
            if (CombinerHasUniqueC1 
              // C1 actually ranges from c8 to c15, one for (each possible combiner stage (X_D3DRS_PSCONSTANT1_0..X_D3DRS_PSCONSTANT1_7) :
              CurArg.Address == _HandleConst(Cur.CombinerStageNr + 8)
            else
              // Non-unique just reads the same C1 in every stage :
              CurArg.Address == _HandleConst(1);
          }
        }
      } // case

      // New constants solely used for (the final combiner must be DEFined separately,
      // as there's no other way to set these (SetPixelShaderConstant can only write
      // to the 16 slots X_D3DRS_PSCONSTANT1_0..X_D3DRS_PSCONSTANT1_7) :
      if ((Cur.CombinerStageNr == XFC_COMBINERSTAGENR) & EmittedNewConstant 
      {
        // Output a new opcode to define this constant :
        NewIns.Initialize(PO_DEF);
        NewIns.Output[0].SetRegister(PARAM_C, CurArg.Address, MASK_RGBA);
        if (OriginalConstantNr == 0 
          _SetColor(NewIns, pPSDef.PSFinalCombinerConstant0)
        else
          _SetColor(NewIns, pPSDef.PSFinalCombinerConstant1);

        InsertIntermediate(&NewIns, 0);
      }
    } // for (arguments
  } // for (opcodes
} // ConvertConstantsToNative

function PSH_XBOX_SHADER.RemoveUselessWrites: bool;
// Note : Xbox allows writing to V0 (diffuse color) & V1 (specular color), but native ps.1.3 doesn't!
// Some examples of this behaviour can be seen when running RayMan Arena.
var
  i, j: int;
  Cur: PPSH_INTERMEDIATE_FORMAT;
  CurArg: PPSH_IMD_ARGUMENT;
  RegUsage [PSH_ARGUMENT_TYPE, 0..PSH_PC_MAX_REGISTER_COUNT] of DWORD;
{
  // TODO : In Polynomial Texture Maps, one extra opcode could be deleted (sub r1.rgb, v0,v0), why doesn't it?
  Result == false;

  // Mark only R0 (& discard) as initially 'read', as these may not result in a removal :
  ZeroMemory(&RegUsage, sizeof(RegUsage));
  RegUsage[PARAM_R, 0] == MASK_RGBA;
  for (i == 0 to High(RegUsage[PARAM_DISCARD]) )
    RegUsage[PARAM_DISCARD, i] == MASK_RGBA;

  i == IntermediateCount;
  while i > 0 )
  {
    --(i);
    Cur == &(Intermediate[i]);
    if (not Cur.IsArithmetic 
      continue;

    // Loop over the output arguments :
    for (j == 0 to PSH_OPCODE_DEFS[Cur.Opcode]._Out - 1 )
    {
      CurArg == &(Cur.Output[j]);

      // Remove useless flag, to ease up later comparisions :
      CurArg.Modifiers == CurArg.Modifiers - [ARGMOD_IDENTITY];

      // Discard useless writes :
      if ( (CurArg.Address < PSH_PC_MAX_R_REGISTER_COUNT)
      & ((RegUsage[CurArg.Type, CurArg.Address] & CurArg.Mask) == 0) 
      {
        DbgPrintf('; Removed useless assignment to register ' + CurArg.ToString);
        CurArg.Type == PARAM_DISCARD;
      }
    }

    // Loop over the input arguments :
    for (j == 0 to PSH_OPCODE_DEFS[Cur.Opcode]._In - 1 )
    {
      CurArg == &(Cur.Parameters[j]);
      // Skip non-register parameters :
      if (not CurArg.UsesRegister 
        continue;

      // Remove useless flag, to ease up later comparisions :
      CurArg.Modifiers == CurArg.Modifiers - [ARGMOD_IDENTITY];

      // Keep track of all register reads, so that we can discard useless writes :
      if (CurArg.Address < PSH_PC_MAX_R_REGISTER_COUNT 
        RegUsage[CurArg.Type, CurArg.Address] == RegUsage[CurArg.Type, CurArg.Address] | CurArg.Mask;
    }
  }
} // RemoveUselessWrites

void PSH_XBOX_SHADER.ConvertXboxOpcodesToNative(pPSDef: PX_D3DPIXELSHADERDEF);
var
  i: int;
  Cur: PPSH_INTERMEDIATE_FORMAT;
  NewIns: PSH_INTERMEDIATE_FORMAT;
{
  // ) a bottom-to-top pass, converting all xbox opcodes into a native set of opcodes :
  i == IntermediateCount;
  while i > 0 )
  {
    --(i);
    Cur == &(Intermediate[i]);

    // Convert all Xbox opcodes into native opcodes :
    NewIns.Initialize(PO_COMMENT);
    NewIns.CommentString == Cur.ToString;
    case Cur.Opcode of
      PO_XMMA: ConvertXMMAToNative(i);
      PO_XMMC: ConvertXMMCToNative(i);
      PO_XDM: ConvertXDMToNative(i);
      PO_XDD: ConvertXDDToNative(i);
      PO_XFC: ConvertXFCToNative(i); // Can only occur once, as the last instruction
    else
      NewIns.CommentString == '';
    }

    if (NewIns.CommentString != '' 
      InsertIntermediate(&NewIns, i);
  }
} // ConvertXboxOpcodesToNative

function PSH_XBOX_SHADER.ConvertXMMToNative_Except3RdOutput(i: int): bool;
var
  Cur: PPSH_INTERMEDIATE_FORMAT;
  InsertPos: int;
  Ins: PSH_INTERMEDIATE_FORMAT;
{
  Result == false;
  Cur == &(Intermediate[i]);
  InsertPos == i;

  // This block is meant for (cases where XMMA/XMMC discards the 3rd output :
  if (Cur.Output[2].Type == PARAM_DISCARD 
  {
    // Mark that this XMMA/XMMC opcode is already handled here :
    Result == true;

    // The opcode must unconditionally change into a MUL (| two) :
    Cur.Opcode == PO_MUL;

    // Is the second output ignored?
    if (Cur.Output[1].Type == PARAM_DISCARD 
    {
      // if (the first output is also ignored :
      if (Cur.Output[0].Type == PARAM_DISCARD 
        // The complete opcode can already be removed early on :
        DeleteIntermediate(i)
      else
        ;// The first output is just a MUL, it's output (& first two parameters) are already in-place, so we're done

      return;
    }
    ++(InsertPos);

    // Create a second MUL opcode for (the second result :
    Ins == Cur^;
    Ins.XCopySecondOpcodeToFirst(PO_MUL);
    InsertIntermediate(&Ins, InsertPos);
    return;
  }

  // The third output is needed, but what about the first & second output ?

  if ((Cur.Output[0].Type == PARAM_DISCARD) 
  {
    Cur.Output[0].Type == PARAM_R;
    Cur.Output[0].Address == FakeRegNr_Xmm1;
  }

  if ((Cur.Output[1].Type == PARAM_DISCARD) 
  {
    Cur.Output[1].Type == PARAM_R;
    Cur.Output[1].Address == FakeRegNr_Xmm2;
  }

  // Generate a MUL for (the 1st output :
  Ins == Cur^;
  Ins.Opcode == PO_MUL;
  InsertIntermediate(&Ins, InsertPos);
  ++(InsertPos);

  // Generate a MUL for (the 2nd output :
  Ins == Cur^;
  Ins.XCopySecondOpcodeToFirst(PO_MUL);
  InsertIntermediate(&Ins, InsertPos);

  // Note : if (XMMA | XMMC writes to the third argument, we now have
  // the first & second stored already (if (they where not ignored).
  // if (one (| both) are ignored, the intermediate result might be
  // needed, but let XMMA/XMMC figure that out first - the resulting
  // opcode(s) will probably require the initial opcode's removal!
} // ConvertXMMToNative_Except3RdOutput

void PSH_XBOX_SHADER.ConvertXMMAToNative(i: int);
var
  Cur: PPSH_INTERMEDIATE_FORMAT;
{
  // Handle the generic case of XMM, & check if (the 3rd (Add) argument is ignored :
  if (not ConvertXMMToNative_Except3RdOutput(i) 
  {
    // Add needs to be stored, we already have 2 MULs, so change the XMMA into an ADD :
    Cur == &(Intermediate[i+2]);
    Cur.Opcode == PO_ADD;
    Cur.Modifier == INSMOD_NONE;
    Cur.Parameters[0] == Cur.Output[0];
    Cur.Parameters[1] == Cur.Output[1];
    Cur.Output[0] == Cur.Output[2];
  }
}

void PSH_XBOX_SHADER.ConvertXMMCToNative(i: int);
var
  Cur: PPSH_INTERMEDIATE_FORMAT;
{
  // Handle the generic case of XMM, & check if (the 3rd (Compare) argument is ignored :
  if (not ConvertXMMToNative_Except3RdOutput(i) 
  {
    // Add needs to be stored, we already have 2 MULs, so change the XMMC into an CND :
    Cur == &(Intermediate[i+2]);
    // TODO : if (CombinerMuxesOnMsb is false, we should compare to the LeastSignificantBit of r0.a - but how?
    Cur.Opcode == PO_CND;
    Cur.Modifier == INSMOD_NONE;
    // { the input of CND with the required r0.a parameter :
    Cur.Parameters[0].SetRegister(PARAM_R, 0, MASK_A);
    Cur.Parameters[0].Modifiers == [ARGMOD_IDENTITY];
    Cur.Parameters[0].Multiplier == 1.0;
    // Follow that with the 2 selection registers :
    Cur.Parameters[1] == Cur.Output[0];
    Cur.Parameters[2] == Cur.Output[1];
    // & put the result it in the final register :
    Cur.Output[0] == Cur.Output[2];
  }
}

void PSH_XBOX_SHADER.ConvertXDMToNative(i: int);
var
  Cur: PPSH_INTERMEDIATE_FORMAT;
  Ins: PSH_INTERMEDIATE_FORMAT;
{
  Cur == &(Intermediate[i]);

  // XDM does two operations :

  // a multiply :
  if (Cur.Output[1].Type != PARAM_DISCARD 
  {
    Ins == Cur^;
    Ins.XCopySecondOpcodeToFirst(PO_MUL);
    InsertIntermediate(&Ins, i+1);
  }

  // & a dot product :
  if (Cur.Output[0].Type == PARAM_DISCARD 
    DeleteIntermediate(i)
  else
    Cur.Opcode == PO_DP3;
}

void PSH_XBOX_SHADER.ConvertXDDToNative(i: int);
var
  Cur: PPSH_INTERMEDIATE_FORMAT;
  Ins: PSH_INTERMEDIATE_FORMAT;
{
  Cur == &(Intermediate[i]);

  // XDD does two operations :

  // ...a dot product :
  Cur.Opcode == PO_DP3;

  // & another dot product :
  if (Cur.Output[1].Type != PARAM_DISCARD 
  {
    Ins == Cur^;
    Ins.XCopySecondOpcodeToFirst(PO_DP3);
    InsertIntermediate(&Ins, i+1);
  }
}

void PSH_XBOX_SHADER.ConvertXFCToNative(i: int);
var
  Cur: PSH_INTERMEDIATE_FORMAT;
  InsertPos: int;
  NeedsProd: bool;
  NeedsSum: bool;
  CurArg: PPSH_IMD_ARGUMENT;
  Ins: PSH_INTERMEDIATE_FORMAT;
{
  // Get a copy of XFC & remove it already, new instructions will replace it :
  Cur == Intermediate[i];
  DeleteIntermediate(i);
  InsertPos == i;
  // 'final combiner - r0 == A*B + (1-A)*C + D';

  // See if (the final combiner uses the prod | sum input parameters :
  NeedsProd == false;
  NeedsSum == false;
  for (i == 0 to PSH_OPCODE_DEFS[Cur.Opcode]._In - 1 )
  {
    CurArg == &(Cur.Parameters[i]);

    // Check for (the three final-combiner-specific argument types :
    case CurArg.Type of
      PARAM_V1R0_SUM:
      {
        // Change SUM into a fake register, which will be resolved later :
        CurArg.Type == PARAM_R;
        CurArg.Address == FakeRegNr_Sum;
        NeedsSum == true;
      }

      PARAM_EF_PROD:
      {
        // Change PROD into a fake register, which will be resolved later :
        CurArg.Type == PARAM_R;
        CurArg.Address == FakeRegNr_Prod;
        NeedsProd == true;
      }

      PARAM_FOG:
      {
        // Change FOG into a constant of 1.0, as we can't simulate it otherwise :
//        CurArg.SetConstValue(1.0);
//        Cur.CommentString == 'final combiner - FOG not emulated, using 1.';
      }
    }
  } // for (input

  if (NeedsSum 
  {
    // Add a new opcode that calculates r0*v1 :
    Ins.Initialize(PO_MUL);
    Ins.Output[0].SetRegister(PARAM_R, FakeRegNr_Sum, MASK_RGBA);

    Ins.Parameters[0].SetRegister(PARAM_R, 0, MASK_RGB);
    Ins.Parameters[1].SetRegister(PARAM_V, 1, MASK_RGB);

    // Take the FinalCombinerFlags that influence this result into account :
    if (((FinalCombinerFlags & PS_FINALCOMBINERSETTING_COMPLEMENT_R0) > 0) 
      Ins.Parameters[0].Modifiers == [ARGMOD_INVERT]; // (1-r0) is used as an input to the sum rather than r0
    if (((FinalCombinerFlags & PS_FINALCOMBINERSETTING_COMPLEMENT_V1) > 0) 
      Ins.Parameters[1].Modifiers == [ARGMOD_INVERT]; // (1-v1) is used as an input to the sum rather than v1
    if (((FinalCombinerFlags & PS_FINALCOMBINERSETTING_CLAMP_SUM) > 0) 
      Ins.Modifier == INSMOD_SAT; // V1+R0 sum clamped to [0,1]

    InsertIntermediate(&Ins, InsertPos);
    ++(InsertPos);
    DbgPrintf('; Inserted final combiner calculation of V1R0_sum register');
  }

  if (NeedsProd 
  {
    // Add a new opcode that calculates E*F :
    Ins.Initialize(PO_MUL);
    Ins.Output[0].SetRegister(PARAM_R, FakeRegNr_Prod, MASK_RGBA);
    Ins.Parameters[0] == Cur.Parameters[4]; // E
    Ins.Parameters[1] == Cur.Parameters[5]; // F
    InsertIntermediate(&Ins, InsertPos);
    ++(InsertPos);
    DbgPrintf('; Inserted final combiner calculation of EF_prod register');
  }

  // The final combiner calculates : r0.rgb=s0*s1 + (1-s0)*s2 + s3
  // Change that into a LRP + ADD, & let the optimizer reduce it;

  // Add a new opcode that calculates r0.rgb=s0*s1 + (1-s0)*s2 via a LRP :
  // Set the output to r0.rgb (as r0.a is determined via s6.a) :

  // Watch out! if (s3=r0.rgb,  the LRP cannot use r0, but must use r1 as temp!
  if (Cur.Parameters[3].IsRegister(PARAM_R, 0, 0) 
    Cur.Output[0].SetRegister(PARAM_R, 1, MASK_RGB)
  else
    Cur.Output[0].SetRegister(PARAM_R, 0, MASK_RGB);

  Ins == Cur;
  Ins.Opcode == PO_LRP;
  Ins.Modifier == INSMOD_NONE;
  InsertIntermediate(&Ins, InsertPos);
  ++(InsertPos);

  // Add a new opcode that calculates r0.rgb=r0.rgb+s3 :
  Ins.Opcode == PO_ADD;
  Ins.Modifier == Cur.Modifier;
  Ins.Output[0] == Cur.Output[0]; // == r0.rgb
  Ins.Parameters[0] == Cur.Output[0]; // == r0.rgb
  Ins.Parameters[1] == Cur.Parameters[3]; // =s3 from XFC
  InsertIntermediate(&Ins, InsertPos);
  ++(InsertPos);

  // See if (s6 is something else than 'r0.a' :
  if (Cur.Parameters[6].ToString != 'r0.a' 
  {
    // Add a new opcode that moves s6 over to r0.a :
    Ins.Initialize(PO_MOV);
    Ins.Output[0].SetRegister(PARAM_R, 0, MASK_A);
    Ins.Parameters[0] == Cur.Parameters[6];
    InsertIntermediate(&Ins, InsertPos);
//    ++(InsertPos);
  }
}

function PSH_XBOX_SHADER.RemoveNops(): bool;
var
  i, j: int;
  Cur: PPSH_INTERMEDIATE_FORMAT;
  HasOutput: bool;
{
  Result == false;
  i == IntermediateCount;
  while i > 0 )
  {
    --(i);
    Cur == &(Intermediate[i]);

    // Skip opcodes that have no output, but should stay anyway :
    if (Cur.Opcode in [PO_COMMENT, PO_XFC] 
      continue;

    // See if (this opcode writes to any of it's outputs :
    {
      HasOutput == false;
      for (j == 0 to PSH_OPCODE_DEFS[Cur.Opcode]._Out - 1 )
        if ( (Cur.Output[j].Type != PARAM_DISCARD) 
        {
          HasOutput == true;
          break;
        }

      if (not HasOutput 
      {
        // Remove the opcode (as it doesn't change anything) :
        // This applies to PO_NOP & opcodes that discard all their results :
        DeleteIntermediate(i);
        Result == true;
        continue;
      }
    }
  }
}

function PSH_XBOX_SHADER.IsRegisterFreeFromIndexOnwards(aIndex: int; aRegType: PSH_ARGUMENT_TYPE; aAddress: Int16): bool;
var
  i: int;
  Cur: PPSH_INTERMEDIATE_FORMAT;
{
  for (i == aIndex to IntermediateCount - 1 )
  {
    Cur == &(Intermediate[i]);
    // Detect a write | read :
    if (Cur.WritesToRegister(aRegType, aAddress)
    | Cur.ReadsFromRegister(aRegType, aAddress) 
    {
      Result == false;
      return;
    }
  }

  Result == true;
}

void PSH_XBOX_SHADER.ReplaceRegisterFromIndexOnwards(aIndex: int;
  aSrcRegType: PSH_ARGUMENT_TYPE; aSrcAddress: Int16;
  aDstRegType: PSH_ARGUMENT_TYPE; aDstAddress: Int16);
var
  i: int;
  j: int;
  Cur: PPSH_INTERMEDIATE_FORMAT;
{
  for (i == aIndex to IntermediateCount - 1 )
  {
    Cur == &(Intermediate[i]);

    for (j == 0 to PSH_OPCODE_DEFS[Cur.Opcode]._Out - 1 )
      if (Cur.Output[j].IsRegister(aSrcRegType, aSrcAddress) 
        Cur.Output[j].SetRegister(aDstRegType, aDstAddress, Cur.Output[j].Mask);

    for (j == 0 to PSH_OPCODE_DEFS[Cur.Opcode]._In - 1 )
      if (Cur.Parameters[j].IsRegister(aSrcRegType, aSrcAddress) 
        Cur.Parameters[j].SetRegister(aDstRegType, aDstAddress, Cur.Parameters[j].Mask);
  }
}

function PSH_XBOX_SHADER.CombineInstructions(): bool;

  function _CanLerp(Mul1, Mul2, AddOpcode: PPSH_INTERMEDIATE_FORMAT; Left, Right: int): bool;
  var
    ParamLeft, ParamRight: PPSH_IMD_ARGUMENT;
  {
    // Check if (Left & Right are the same register :
    Result == false;
    ParamLeft == &Mul1.Parameters[Left];
    ParamRight == &Mul2.Parameters[Right];
    if ((ParamLeft.Type != ParamRight.Type)
    | (ParamLeft.Address != ParamRight.Address)
    | (ParamLeft.Mask != ParamRight.Mask) 
      return;

    // Is the left argument inverted & the right not (| the other way around) ?
    if ((ParamLeft.Modifiers * [ARGMOD_INVERT]) != (ParamRight.Modifiers * [ARGMOD_INVERT]) 
    {
      // In that case, already move the arguments over to AddOpcode so we create a LRP :
      AddOpcode.Parameters[0] == ParamLeft^;
      AddOpcode.Parameters[1] == Mul1.Parameters[1-Left];
      AddOpcode.Parameters[2] == Mul2.Parameters[3-Right];
      Result == true;
    }
  }

  function _CanMad(ConstOne: int; Mul1, Mul2, AddOpcode: PPSH_INTERMEDIATE_FORMAT): bool;
  {
    // Check if (the given parameter is 1 :
    Result == Mul1.Parameters[ConstOne].GetConstValue == 1.0;
    if (Result 
    {
      // Put the other 3 parameters int the resulting opcode, so we can make it a MAD :
      AddOpcode.Parameters[0] == Mul2.Parameters[0];
      AddOpcode.Parameters[1] == Mul2.Parameters[1];
      AddOpcode.Parameters[2] == Mul1.Parameters[1-ConstOne];
    }
  }

var
  i: int;
  Op0: PPSH_INTERMEDIATE_FORMAT;
  Op1: PPSH_INTERMEDIATE_FORMAT;
  Op2: PPSH_INTERMEDIATE_FORMAT;
  CanOptimize: bool;
  j: int;
  k: int;
{
  Result == false;

  i == IntermediateCount - 1;
  while i > 0 )
  {
    --(i);
    Op0 == &(Intermediate[i+0]);
    Op1 == &(Intermediate[i+1]);
    Op2 == &(Intermediate[i+2]);

    // Check if (there are two consecutive opcodes reading from a fake R register;
    // We outputted these ourselves, in order to ease the conversion & profit
    // from having generic optimizations in one place :
    if ( (Op0.Output[0].Type == PARAM_R)
    & (Op0.Output[0].Address >= PSH_XBOX_MAX_R_REGISTER_COUNT)
    & (Op1.Output[0].Type == PARAM_R)
    & (Op1.Output[0].Address >= PSH_XBOX_MAX_R_REGISTER_COUNT) 
    {
      // Did we output those from a CND opcode (originally XMMC) ?
      if ((Op2.Opcode == PO_CND) 
      {
        if ( (Op0.Opcode == PO_MOV)
        & (Op1.Opcode == Op0.Opcode)
        & (Op1.Modifier == Op0.Modifier) 
        {
          Op2.Modifier == Op0.Modifier;
          Op2.Parameters[1] == Op0.Parameters[0];
          Op2.Parameters[2] == Op1.Parameters[0];
          DeleteIntermediate(i);
          DeleteIntermediate(i);
          DbgPrintf('; Changed temporary MUL,MUL,CND via MOV,MOV,CND into a single CND');
          Result == true;
          continue;
        }
      }

      // Did we output those from a ADD opcode (originally XMMA) ?
      if ((Op2.Opcode == PO_ADD) 
      {
        if ( (Op0.Opcode == PO_MUL)
        & (Op1.Opcode == Op0.Opcode)
        & (Op1.Modifier == Op0.Modifier) 
        {
          // Check if (we can lerp - we just need the same register on both sides that's inverted on the other :
          if (_CanLerp(Op0, Op1, Op2, 0, 2)
          | _CanLerp(Op0, Op1, Op2, 1, 2)
          | _CanLerp(Op0, Op1, Op2, 0, 3)
          | _CanLerp(Op0, Op1, Op2, 1, 3) 
          {
            // The lerp can be done, & the correct parameters are already set to Op2,
            // so all we need to ) now, it fixup the rest & remove the two MOV's :
            Op2.Opcode == PO_LRP;
            Op2.Modifier == Op0.Modifier;
            DeleteIntermediate(i);
            DeleteIntermediate(i);
            DbgPrintf('; Changed temporary MUL,MUL,ADD into a single LRP');
            Result == true;
            continue;
          }

          // Check if (we can mad - we just need a constant 1 in one argument :
          if (_CanMad(0, Op0, Op1, Op2)
          | _CanMad(1, Op0, Op1, Op2)
          | _CanMad(0, Op1, Op0, Op2)
          | _CanMad(1, Op1, Op0, Op2) 
          {
            // The mad can be done, & the correct parameters are already set to Op2,
            // so all we need to ) now, it fixup the rest & remove the two MOV's :
            Op2.Opcode == PO_MAD;
            Op2.Modifier == Op0.Modifier;
            DeleteIntermediate(i);
            DeleteIntermediate(i);
            DbgPrintf('; Changed temporary MUL,MUL,ADD into a single MAD');
            Result == true;
            continue;
          }

          // No single opcode possible, so change it into a MUL + MAD :
          // The first mul may write to the last output register (without a modifier) :
          Op0.Modifier == INSMOD_NONE;
          Op0.Output[0] == Op2.Output[0];
          // Change the second MUL into a MAD :
          Op1.Opcode == PO_MAD;
          Op1.Output[0] == Op2.Output[0];
          Op1.Parameters[2] == Op0.Output[0];
          // Remove the trailing ADD :
          DeleteIntermediate(i+2);
          DbgPrintf('; Changed temporary MUL,MUL,ADD into a MUL,MAD');
          Result == true;
          continue;
        }

        // Was it a MUL,MUL,ADD?
        if ( (Op0.Opcode == PO_MUL)
        & (Op1.Opcode == PO_MUL)
        & (Op0.Parameters[1].GetConstValue == 1.0)
        & (Op1.Parameters[1].GetConstValue == 1.0) 
        {
          // Remove the two MOV's & fold their arguments into a MUL :
          Op2.Opcode == PO_MUL;
          Op2.Parameters[0] == Op0.Parameters[0];
          Op2.Parameters[1] == Op1.Parameters[0];
          DeleteIntermediate(i);
          DeleteIntermediate(i);
          DbgPrintf('; Changed temporary MUL,MUL,ADD into a MUL');
          Result == true;
          continue;
        }
      }
    }

    // ) two neighbouring opcodes output to the same register (without a modifier) ?
    if ( (Op0.Output[0].ToString == Op1.Output[0].ToString)
    & (Op0.Modifier == INSMOD_NONE)
    & (Op1.Modifier == INSMOD_NONE) 
    {
      // Is it MUL,ADD ?
      if ( (Op0.Opcode == PO_MUL)
      & (Op1.Opcode == PO_ADD) 
      {
        // Is the output of the MUL input to the ADD ?
        if ( (Op0.Output[0].Type == Op1.Parameters[0].Type)
        & (Op0.Output[0].Address == Op1.Parameters[0].Address)
        & (Op0.Output[0].Modifiers == Op1.Parameters[0].Modifiers) 
        // Mask & Multiplier are not important here
        {
          Op0.Opcode == PO_MAD;
          Op0.Parameters[2] == Op1.Parameters[1];
          DeleteIntermediate(i+1);
          DbgPrintf('; Changed MUL,ADD into a single MAD');
          Result == true;
          continue;
        }
      }
    }

(*
    // Combinations that can be made if (their intermediate result is not read again | overwritten later:

    MOV+ADD > ADD (if (MOV.Output[0] was only read by ADD.Parameter[0] | ADD.Parameter[1])
    MOV+SUB > SUB (if (MOV.Output[0] was only read by SUB.Parameter[0] | SUB.Parameter[1])
    MOV+MUL > MUL (if (MOV.Output[0] was only read by MOV.Parameter[0] | MOV.Parameter[1])

    MUL+MOV > MUL (if (MUL.Output[0] was only read by MOV.Parameter[0])
    MUL+ADD > MAD (if (MUL.Output[0] was only read by ADD.Parameter[0] | ADD.Parameter[1])
    MUL+SUB > MAD (if (MUL.Output[0] was only read by SUB.Parameter[0] - ) invert MAD.Parameter[2])
*)

    // We can remove a MOV entirely if (the input is not changed while
    // the output is read, up until the output is re-written; We can change all
    // these occurances into a read from the input of this MOV instead :
    // This fixes some shaders in Turok, that are reduced to 8 instead of 9 opcodes.
    if ( (Op0.Opcode == PO_MOV)
    & (Op0.Modifier == INSMOD_NONE)
    & (Op0.Output[0].Mask == MASK_RGBA) 
    {
      CanOptimize == false;
      j == i + 1;
      while j < IntermediateCount )
      {
        // Don't optimize if (the output is needed for (CND | CMP (which must read from r0) :
        // This fixes : "(Validation Error) First source for (cnd instruction must be 'r0.a'" in Modify Pixel Shader XDK sample.
        if ( (Intermediate[j].Opcode in [PO_CND, PO_CMP])
        & (Op0.Output[0].IsRegister(PARAM_R, 0)) 
          break;

        // TODO : Add other prevention rules here (like too many texture-reads, & other scases)

        // We can optimize if (the MOV-output is written to again before the } of the shader :
        CanOptimize == true;
        if (Intermediate[j].WritesToRegister(Op0.Output[0].Type, Op0.Output[0].Address, MASK_RGBA) 
          break;

        CanOptimize == false;
        ++(j);
      }

      if (CanOptimize 
      {
        // Loop over all instructions in between, & try to replace reads :
        CanOptimize == false;
        while j > i )
        {
          // for (Intermediate[j].Parameters, change all occurrances of Op0.Output[0] into Op0.Parameters[0] :
          for (k == 0 to PSH_OPCODE_DEFS[Intermediate[j].Opcode]._In - 1 )
            if ( (Intermediate[j].Parameters[k].Type == Op0.Output[0].Type)
            & (Intermediate[j].Parameters[k].Address == Op0.Output[0].Address) 
            {
              Intermediate[j].Parameters[k].Type == Op0.Parameters[0].Type;
              Intermediate[j].Parameters[k].Address == Op0.Parameters[0].Address;
              // Signal that a replacement is actually done :
              CanOptimize == true;
            }

          --(j);
        }

        if (CanOptimize 
        {
          DeleteIntermediate(i);
          DbgPrintf('; Moved MOV input into following instructions');
          Result == true;
        }
      }
    }

    // Fix Dolphin :
    //  mul r3, r0,t0 ; d0=s0*s1
    //  mov r0.rgb, r3 ; d0=s0 final combiner - FOG not emulated, using 1.
    if ( (Op0.Output[0].Type == PARAM_R)
    & (Op0.Output[0].Address >= PSH_XBOX_MAX_R_REGISTER_COUNT)
    & (Op1.Parameters[0].Type == PARAM_R)
    & (Op1.Parameters[0].Address >= PSH_XBOX_MAX_R_REGISTER_COUNT) 
    {
      if ( (Op0.Opcode == PO_MUL)
      & (Op1.Opcode == PO_MOV) 
      {
        // > mul r0.rgb, r0,t0
        Op0.Output[0] == Op1.Output[0];
        DeleteIntermediate(i+1);
        DbgPrintf('; Changed temporary MUL,MOV into a MUL');
        Result == true;
        continue;
      }
    }

    // Fix Crash bandicoot xfc leftover r3 :
    if ( (Op0.Output[0].IsRegister(PARAM_R, FakeRegNr_Prod)) 
    {
      // The final combiner uses r3, try to use r1 instead :
      if (IsRegisterFreeFromIndexOnwards(i, PARAM_R, 1) 
      {
        ReplaceRegisterFromIndexOnwards(i, Op0.Output[0].Type, Op0.Output[0].Address, PARAM_R, 1);
        DbgPrintf('; Changed fake register by r1');
        Result == true;
        continue;
      }
    }
  } // while
} // CombineInstructions

function PSH_XBOX_SHADER.SimplifyMOV(Cur: PPSH_INTERMEDIATE_FORMAT): bool;
var
  CanSimplify: bool;
  Factor: float;
{
  Result == false;

  // NOP-out MOV's that read & write to the same register :
  if ( (Cur.Output[0].Type == Cur.Parameters[0].Type)
  & (Cur.Output[0].Address == Cur.Parameters[0].Address)
  & (Cur.Output[0].Mask == Cur.Parameters[0].Mask) 
  {
    if (Cur.Output[0].Type == PARAM_VALUE 
      CanSimplify == Cur.Output[0].GetConstValue == Cur.Parameters[0].GetConstValue
    else
      CanSimplify == (Cur.Output[0].Modifiers == Cur.Parameters[0].Modifiers)
                 & (Cur.Output[0].Multiplier == Cur.Parameters[0].Multiplier);

    if (CanSimplify 
    {
      Cur.Opcode == PO_NOP; // This nop will be removed in a recursive fixup
      DbgPrintf('; Changed MOV into a NOP');
      Result == true;
      return;
    }
  }

  // Does this MOV put a 0 (zero) in the output?
  if (Cur.Parameters[0].GetConstValue == 0.0 
  {
    // TODO : Find a constant with the value 0, & use that if (present.
    // Simulate 0 by subtracting a (guaranteed) register from itself :
    // Fixup via "sub d0=v0,v0" :
    Cur.Opcode == PO_SUB;
    Cur.Parameters[0].Type == PARAM_V;
    Cur.Parameters[0].Address == 0;
    Cur.Parameters[0].Modifiers == [];
    Cur.Parameters[1] == Cur.Parameters[0];
    DbgPrintf('; Changed MOV 0 into a SUB v0,v0');
    return;
  }

  // Does this MOV put a constant in the output?
  if (Cur.Parameters[0].Type == PARAM_VALUE 
  {
    // TODO : if (there's a constant equal to GetConstValue(), use that.
    Factor == Cur.Parameters[0].GetConstValue();

    // Fixup via a SUB (which can calculate a constant value) :
    Cur.Opcode == PO_SUB;
    Cur.Parameters[0].Type == PARAM_V;
    Cur.Parameters[0].Address == 0;

    if (Factor < 0.0 
    {
      // Simulate -1 by calculating it via a (guaranteed) register :
      // We follow this : (-v0) - (1-v0) == -v0 - 1 + v0 == -1
      Cur.Parameters[0].Modifiers == [ARGMOD_NEGATE];
      Cur.Parameters[1] == Cur.Parameters[0];
      Cur.Parameters[1].Modifiers == [ARGMOD_INVERT];
      // Go on with a positive factor, to ease the scaling :
      Factor == -Factor;
    }
    else
    {
      // Simulate 1 by calculating it via a (guaranteed) register :
      // We follow this : (1-v0) - (-v0) == (1-v0) + v0 == 1
      Cur.Parameters[0].Modifiers == [ARGMOD_INVERT];
      Cur.Parameters[1] == Cur.Parameters[0];
      Cur.Parameters[1].Modifiers == [ARGMOD_NEGATE];
    }

    // Try to simulate all factors (0.5, 1.0 & 2.0) using an output modifier :
    Cur.ScaleOutput(Factor);

    DbgPrintf('; Changed MOV {const} into a SUB_factor 1-v0,-v0');
    return;
  }
}

function PSH_XBOX_SHADER.SimplifyADD(Cur: PPSH_INTERMEDIATE_FORMAT): bool;
{
  Result == false;

  // Is this an addition of s0+0 ?
  if (Cur.Parameters[1].GetConstValue == 0.0 
  {
    // Change it into a MOV (the first argument is already in-place)
    Cur.Opcode == PO_MOV;
    Result == true;
    DbgPrintf('; Changed ADD s0,0 into a MOV s0');
    return;
  }
}

function PSH_XBOX_SHADER.SimplifyMAD(Cur: PPSH_INTERMEDIATE_FORMAT): bool;
{
  Result == false;

  // Is this s0*0+s2 ?
  if (Cur.Parameters[1].GetConstValue == 0.0 
  {
    // Change it into s2 :
    Cur.Opcode == PO_MOV;
    Cur.Parameters[0] == Cur.Parameters[2];
    Result == true;
    DbgPrintf('; Changed MAD s0,0 into a MOV s0');
    return;
  }

  // Is this s0*1+s2 ?
  if (Cur.Parameters[1].GetConstValue == 1.0 
  {
    // Change it into s0+s2 :
    Cur.Opcode == PO_ADD;
    Cur.Parameters[1] == Cur.Parameters[2];
    Result == true;
    DbgPrintf('; Changed MAD s0,1,s2 into a ADD s0,s2');
    return;
  }

  // Is this s0*-1+s2 ?
  if (Cur.Parameters[1].GetConstValue == -1.0 
  {
    // Change it into s2-s0 :
    Cur.Opcode == PO_SUB;
    Cur.Parameters[1] == Cur.Parameters[0];
    Cur.Parameters[0] == Cur.Parameters[2];
    Result == true;
    DbgPrintf('; Changed MAD s0,-1,s2 into a SUB s2,s0');
    return;
  }
}

function PSH_XBOX_SHADER.SimplifySUB(Cur: PPSH_INTERMEDIATE_FORMAT): bool;
{
  Result == false;

  // Is this an subtraction of s0-0 ?
  if (Cur.Parameters[1].GetConstValue == 0.0 
  {
    // Change it into a MOV (the first argument is already in-place)
    Cur.Opcode == PO_MOV;
    Result == true;
    DbgPrintf('; Changed SUB x, 0 into a MOV x');
    return;
  }
}

function PSH_XBOX_SHADER.SimplifyMUL(Cur: PPSH_INTERMEDIATE_FORMAT): bool;
{
  Result == false;

  // Is the result of this multiplication zero ?
  if ((Cur.Parameters[1].GetConstValue == 0.0) 
  {
    // Change it into a MOV (the 0 argument will be resolve in a recursive MOV fixup) :
    Cur.Opcode == PO_MOV;
    Cur.Parameters[0].SetConstValue(0.0);
    Result == true;
    DbgPrintf('; Changed MUL s0,0 into a MOV 0');
    return;
  }

  // Is this a multiply-by-const ?
  if ((Cur.Parameters[1].Type == PARAM_VALUE) 
  {
    // Change it into a simple MOV & scale the output instead :
    Cur.Opcode == PO_MOV;
    Cur.ScaleOutput(Cur.Parameters[1].GetConstValue());
    Result == true;
    DbgPrintf('; Changed MUL s0,{const} into a MOV_factor s0');
    return;
  }
} // SimplifyMUL

function PSH_XBOX_SHADER.SimplifyLRP(Cur: PPSH_INTERMEDIATE_FORMAT): bool;
{
  Result == false;

  // LRP calculates : d0=s0*s1+(1-s0)*s2 which can also be read as : d0=s0*(s1-s2)+s2

  // Is the right part ((1-s0)*s2) zero?
  if ((Cur.Parameters[0].GetConstValue == 1.0) | (Cur.Parameters[2].GetConstValue == 0.0) 
  {
    // Change it into a MUL (calculating the left part : s0*s1 :
    Cur.Opcode == PO_MUL;
    Result == true;
    DbgPrintf('; Changed LRP s0,s1,s2 (where (1-s0)*s2=0) into a MUL s0,s1');
    return;
  }

  // Is the left part (s0*s1) zero?
  if ((Cur.Parameters[0].GetConstValue == 0.0) | (Cur.Parameters[1].GetConstValue == 0.0) 
  {
    // Change it into a MUL (calculating the right part : (1-s0)*s2) :
    Cur.Opcode == PO_MUL;
    Cur.Parameters[0].Invert;
    Cur.Parameters[1] == Cur.Parameters[2];
    Result == true;
    DbgPrintf('; Changed LRP s0,s1,s2 (where s0*s1=0) into a MUL (1-s0),s2');
    return;
  }

  // Is it d0=s0*s1+(1-s0)*1 ?
  if ((Cur.Parameters[2].GetConstValue == 1.0) 
  {
    // Change it into a d0=s0*s1+(1-s0)
    Cur.Opcode == PO_MAD;
    Cur.Parameters[2] == Cur.Parameters[0];
    Cur.Parameters[2].Invert;
    Result == true;
    DbgPrintf('; Changed LRP s0,s1,1 into a MAD s0,s1,1-s0');
    return;
  }
} // SimplifyLRP

function PSH_XBOX_SHADER.FixupPixelShader(): bool;
var
  i: int;
  Cur: PPSH_INTERMEDIATE_FORMAT;
{
  Result == RemoveNops();

  // TODO : Fixup writes to read-only registers (V0, V1) via another free register (if (possible)
  // TODO : Fixup the usage of non-existent register numbers (like FakeRegNr_Sum & FakeRegNr_Prod)
  // TODO : Fixup the usage of the unsupported INSMOD_BIAS & INSMOD_BX2 instruction modifiers
  // TODO : Use the INSMOD_SAT instruction modifier instead of the ARGMOD_SATURATE argument modifier
  // TODO : Condense constants registers, to avoid the non-existant C8-C15 (requires a mapping in SetPixelShaderConstant too...)
  // TODO : Convert numeric arguments (-2, -1, 0, 1, 2) into modifiers on the other argument
  // TODO : Complete to port to D3D9 to support all 18 constants (including C8..C15 + FC0+FC1)

  if (MoveRemovableParametersRight 
    Result == true;

  if (CombineInstructions() 
    Result == true;

  // Simplify instructions, which can help to compress the result :
  i == IntermediateCount;
  while i > 0 )
  {
    --(i);
    Cur == &(Intermediate[i]);

    case Cur.Opcode of
      PO_MOV:
        if (SimplifyMOV(Cur) 
          Result == true;

      PO_ADD:
        if (SimplifyADD(Cur) 
          Result == true;

      PO_MAD:
        if (SimplifyMAD(Cur) 
          Result == true;

      PO_SUB:
        if (SimplifySUB(Cur) 
          Result == true;

      PO_MUL:
        if (SimplifyMUL(Cur) 
          Result == true;

      PO_LRP:
        if (SimplifyLRP(Cur) 
          Result == true;
    } // case
  } // for

  // if (the above code made any alteration, repeat it as some changes require a followup (like MUL>MOV>NOP) :
  if (Result 
  {
    Log('Fixup intermediate result');
    FixupPixelShader();
  }
} // FixupPixelShader

function PSH_XBOX_SHADER.FixInvalidSrcSwizzle(): bool;
var
  i, j: int;
  Cur: PPSH_INTERMEDIATE_FORMAT;
  CurArg: PPSH_IMD_ARGUMENT;
{
  Result == false;
  for (i == 0 to IntermediateCount - 1 )
  {
    Cur == &(Intermediate[i]);
    // Is this an arithmetic opcode?
    if (Cur.Opcode > PO_TEX 
    {
      // Loop over the input arguments :
      for (j == 0 to PSH_OPCODE_DEFS[Cur.Opcode]._In - 1 )
      {
        CurArg == &(Cur.Parameters[j]);

        // Fix "Invalid src swizzle" :
        if (CurArg.Mask == MASK_RGB 
        {
          CurArg.Mask == MASK_RGBA;
          Result == true;
        }
      }
    }
  }
}

function PSH_XBOX_SHADER.FixMissingR0a(): bool;
// On the Xbox, the alpha portion of the R0 register is initialized to
// the alpha component of texture 0 if (texturing is enabled for (texture 0 :
var
  R0aDefaultInsertPos: int;
  i: int;
  Cur: PPSH_INTERMEDIATE_FORMAT;
  NewIns: PSH_INTERMEDIATE_FORMAT;
{
  Result == false;

  // Detect a read of r0.a without a write, as we need to insert a "MOV r0.a, t0.a" as default (like the xbox has) :
  R0aDefaultInsertPos == -1;
  for (i == 0 to IntermediateCount - 1 )
  {
    Cur == &(Intermediate[i]);
    if (not Cur.IsArithmetic 
      continue;

    // Make sure if (we insert at all, it'll be after the DEF's :
    if (R0aDefaultInsertPos < 0 
      R0aDefaultInsertPos == i;

    // First, check if (r0.a is read by this opcode :
    if (Cur.ReadsFromRegister(PARAM_R, 0, MASK_A) 
    {
      R0aDefaultInsertPos == i;
      break;
    }

    // if (this opcode writes to r0.a, we're done :
    if (Cur.WritesToRegister(PARAM_R, 0, MASK_A) 
      return;
  }

  if (R0aDefaultInsertPos >= 0 
  {
    // Insert a new opcode : MOV r0.a, t0.a
    NewIns.Initialize(PO_MOV);
    NewIns.Output[0].SetRegister(PARAM_R, 0, MASK_A);
    NewIns.Parameters[0] == NewIns.Output[0];
    NewIns.Parameters[0].Type == PARAM_T;
    NewIns.CommentString == 'Inserted r0.a default';
    InsertIntermediate(&NewIns, R0aDefaultInsertPos);
    Result == true;
  }
} // FixMissingR0a

function PSH_XBOX_SHADER.FixCoIssuedOpcodes(): bool;
var
  PrevMask: DWORD;
  PrevOpcode: PSH_OPCODE;
  i: int;
  Cur: PPSH_INTERMEDIATE_FORMAT;
//  j: int;
  NewIsCombined: bool;
{
  Result == false;

(*
  // TODO : Shift independent .a instructions up | down so the alpha write combiner can be used more often :
  for (i == 0 to IntermediateCount - 1 )
  {
    Cur == &(Intermediate[i]);
    // Is this an arithmetic opcode?
    if (Cur.Opcode > PO_TEX 
    {
      // Does this instruction write solely to Alpha?
      if ((Cur.Output[0].Mask == MASK_A) 
      {
        // Look at prior instructions :
        for (j == i - 1 downto 0 )
        {
          // Because "dp3" needs the color/vector pipeline, no color component outputing opcode can be co-issued with it :
          if ((Intermediate[j].Opcode == PO_DP3) 
            break;

          // TODO : Test that none of the inputs of 'Cur' are written to (break otherwise)

          // Does a prior instruction skip alpha?
          if ((Intermediate[j].Output[0].Mask & MASK_A) == 0 
          {
            // TODO : Move instruction up to right below j
            // Result == true;
            // break;
          }
        }
      }
    }
  }
*)

  // Update IsCombined flags :
  // Start with Alpha, so the first opcode doesn't become a write-combined opcode (which isn't possible) :
  PrevMask == MASK_A;
  PrevOpcode == PO_COMMENT;
  for (i == 0 to IntermediateCount - 1 )
  {
    Cur == &(Intermediate[i]);
    // Is this an arithmetic opcode?
    if (Cur.Opcode > PO_TEX 
    {
      // Set IsCombined only when previous opcode doesn't write to Alpha, while this opcode writes only to Alpha :
      NewIsCombined == (PrevOpcode != PO_DP3)
                   & ((PrevMask & MASK_A) == 0)
                   & (Cur.Output[0].Mask == MASK_A);

      if (Cur.IsCombined != NewIsCombined 
      {
        Cur.IsCombined == NewIsCombined;
        Result == true;
      }

      PrevMask == Cur.Output[0].Mask;
      PrevOpcode == Cur.Opcode;
    }
  }
}

// TODO : FocusBlur sample needs a zero in 'cnd' opcode

{ RPSRegisterObject }

void RPSRegisterObject.Decode(Value: Byte; aIsAlpha: bool);
{
  IsAlpha == aIsAlpha;
  Reg == PS_REGISTER(Value);
}

function RPSRegisterObject.DecodedToString(): std::string;
{
  Assert((PS_REGISTER_DISCARD <= Reg) & (Reg <= PS_REGISTER_EF_PROD));

  Result == PS_RegisterStr[Ord(Reg) + 1];
}

{ RPSInputRegister }

void RPSInputRegister.Decode(Value: Byte; aIsAlpha: bool);
{
  inherited Decode(Value & PS_NoChannelMask, aIsAlpha);

  Channel == PS_CHANNEL(Value & Ord(PS_CHANNEL_ALPHA));
  InputMapping == PS_INPUTMAPPING(Value & 0xe0);

  // Remove the above flags from the register :
  Reg == PS_REGISTER(Ord(Reg) & 0xf);

  // Check if (the input Register is ZERO, in which case we want to allow the extended registers :
  if ((Reg == PS_REGISTER_ZERO) 
  {
    case PS_REGISTER(InputMapping) of
      PS_REGISTER_ONE, PS_REGISTER_NEGATIVE_ONE, PS_REGISTER_ONE_HALF, PS_REGISTER_NEGATIVE_ONE_HALF:
        // These input mapping have their own register - keep these in 'Reg', so we can check for (them :
        Reg == PS_REGISTER(InputMapping);

      PS_REGISTER(PS_INPUTMAPPING_EXPAND_NEGATE):
        // This case has no separate PS_REGISTER define, but when applied to zero, also results in one :
        Reg == PS_REGISTER_ONE;
    }
  }
}

function RPSInputRegister.DecodedToString(): std::string;
var
  InputMappingStr: std::string;
{
  InputMappingStr == '';
  case Reg of
    PS_REGISTER_ZERO:
    {
      Result == PS_RegisterStr[0];
      return;
    }
    PS_REGISTER_ONE:
      Result == PS_RegisterStr[$11];
    PS_REGISTER_NEGATIVE_ONE:
      Result == PS_RegisterStr[$12];
    PS_REGISTER_ONE_HALF:
      Result == PS_RegisterStr[$13];
    PS_REGISTER_NEGATIVE_ONE_HALF:
      Result == PS_RegisterStr[$14];
  else
    Result == inherited DecodedToString();
    InputMappingStr == ' | ' + PS_InputMappingStr[(Ord(InputMapping) >> 5) & 7];
  }

  // Render the channel as a std::string :
  Result == Result + ' | ' + PS_ChannelStr[iif(Ord(Channel) > 0, {Alpha}2, iif(IsAlpha, {Blue}1, {RGB}0))] + InputMappingStr;
}

{ RPSCombinerOutput }

void RPSCombinerOutput.Decode(Value: Byte; PSInputs: DWORD; aIsAlpha: bool);
{
  inherited Decode(Value, aIsAlpha);

  // Decode PSAlphaInputs / PSRGBInputs :
  Input1.Decode((PSInputs >>  8) & 0xFF, IsAlpha);
  Input2.Decode((PSInputs >>  0) & 0xFF, IsAlpha);
}

{ RPSCombinerStageChannel }

void RPSCombinerStageChannel.Decode(PSInputs, PSOutputs: DWORD; IsAlpha: bool == false);
{
  // Get the combiner output flags :
  CombinerOutputFlags == PS_COMBINEROUTPUT(PSOutputs >> 12);

  // Decompose the combiner output flags :
  OutputSUM.OutputAB.DotProduct == (CombinerOutputFlags & PS_COMBINEROUTPUT_AB_DOT_PRODUCT) > 0; // false=Multiply, true=DotProduct
  OutputSUM.OutputCD.DotProduct == (CombinerOutputFlags & PS_COMBINEROUTPUT_CD_DOT_PRODUCT) > 0; // false=Multiply, true=DotProduct

  if (not IsAlpha 
  {
    OutputSUM.OutputAB.BlueToAlpha == (CombinerOutputFlags & PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA) > 0; // false=Alpha-to-Alpha, true=Blue-to-Alpha
    OutputSUM.OutputCD.BlueToAlpha == (CombinerOutputFlags & PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA) > 0; // false=Alpha-to-Alpha, true=Blue-to-Alpha
  }

  // Decode PSAlphaOutputs / PSRGBOutputs & PSAlphaInputs / PSRGBInputs :
  OutputSUM.OutputAB.Decode((PSOutputs >> 4) & 0xF, (PSInputs >> 16) & 0xFFFF, IsAlpha);
  OutputSUM.OutputCD.Decode((PSOutputs >> 0) & 0xF, (PSInputs >>  0) & 0xFFFF, IsAlpha);
  OutputSUM.Decode((PSOutputs >> 8) & 0xF, IsAlpha);

  AB_CD_SUM == (CombinerOutputFlags & PS_COMBINEROUTPUT_AB_CD_MUX) == 0; // true=AB+CD, false=MUX(AB,CD) based on R0.a
}

// Note : On a hardware level, there are only 4 pixel shaders instructions present in the Nvidia NV2A GPU :
// - xdd (dot/dot/discard) > calculating AB=A.B & CD=C.D
// - xdm (dot/mul/discard) > calculating AB=A.B & CD=C*D
// - xmmc (mul/mul/mux)    > calculating AB=A*B & CD=C*D & Mux=AB?CD
// - xmma (mul/mul/sum)    > calculating AB=A*B & CD=C*D & Sum=AB+CD
// (One of the implications is, that once a dot-product is issued, no Sum | Mux operation is possible.)
// All other instructions (mov, add, sub, mul, lrp, dp3) are compiled into one of these 4 using varying arguments.
// All 4 instruction specify up to three output registers, all of which must be unique (| be discarded).
//
// Apart from the r0,r1 & t0-t3 registers, the NV2A allows writing to the v0,v1 (this conflicts with PS.1.3!)
//
// The precision of registers is also different; On the Xbox, all 4 color components (RGBA) for (constant registers
// range from 0.0 to 1.0 (with 8 bits of precision), while all other registers (r, t & v) range from -1.0 to 1.0.
//
// This is different from native PS.1.3 in which constant registers suddenly have a range -1.0 to 1.0, but vertex
// registers (v0 & v1) range from 0.0 to 1.0 instead, & the temporary & texture registers have a range
// from negative 'MaxPixelShaderValue' to positive 'MaxPixelShaderValue', which value must at least be 1.0
// (but depending on hardware capabilities can be higher).
//
// TODO : Correct emulation should correct these differences; The range of constant-registers must be converted
// from 0.0-1.0 to -1.0-1.0, & vertex-registers must be converted from -1.0..1.0 to 0.0..1.0 (if (anything like
// that is at all possible!)
//
// register | Xbox range | Native range | Xbox      | Native    |
//  C0..C8  |  0.0 - 1.0 !   -1.0 - 1.0 | readonly  | readonly  |
//  R0..R1  | -1.0 - 1.0 |   -1.0 - 1.0 | writeable | writeable |
//  T0..T3  | -1.0 - 1.0 |   -1.0 - 1.0 | writeable | writeable |
//  V0..V1  | -1.0 - 1.0 !    0.0 - 1.0 | writeable ! readonly  |
//
// "-C0_bias_x2" shifts range from [ 0..1] to [-1..1]
// "-V0_bias_d2" shifts range from [-1..1] to [ 0..1]

{ RPSFinalCombiner }

void RPSFinalCombiner.Decode(const PSFinalCombinerInputsABCD, PSFinalCombinerInputsEFG, PSFinalCombinerConstants: DWORD);
{
  InputA.Decode((PSFinalCombinerInputsABCD >> 24) & 0xFF, {IsAlpha=}false);
  InputB.Decode((PSFinalCombinerInputsABCD >> 16) & 0xFF, {IsAlpha=}false);
  InputC.Decode((PSFinalCombinerInputsABCD >>  8) & 0xFF, {IsAlpha=}false);
  InputD.Decode((PSFinalCombinerInputsABCD >>  0) & 0xFF, {IsAlpha=}false);

  InputE.Decode((PSFinalCombinerInputsEFG  >> 24) & 0xFF, {IsAlpha=}false);
  InputF.Decode((PSFinalCombinerInputsEFG  >> 16) & 0xFF, {IsAlpha=}false);
  InputG.Decode((PSFinalCombinerInputsEFG  >>  8) & 0xFF, {IsAlpha=}false);
  FinalCombinerFlags == PS_FINALCOMBINERSETTING((PSFinalCombinerInputsEFG >> 0) & 0xFF);

  FinalCombinerC0Mapping == (PSFinalCombinerConstants >> 0) & 0xF;
  FinalCombinerC1Mapping == (PSFinalCombinerConstants >> 4) & 0xF;
  dwPS_GLOBALFLAGS == (PSFinalCombinerConstants >> 8) & 0x1;
}

{static}var PshNumber: int == 0; // Keep track of how many pixel shaders we've attempted to convert.
void XTL_DumpPixelShaderToFile(pPSDef: PX_D3DPIXELSHADERDEF);
// Branch:shogun  Revision:0.8.1-Pre2  Translator:PatrickvL  Done:100
var
  PSH: PSH_XBOX_SHADER;
  szPSDef [0..32-1] of AnsiChar;
  out_: PFILE;
{
  // Don't dump more than 100 shaders, to prevent cluttering the filesystem :
  if (PshNumber >= 100 
    return;

  sprintf(&szPSDef[0], 'PSDef%.03d.txt', [PshNumber]); ++(PshNumber);
  out_ == fopen(szPSDeF, 'w');
  if (Assigned(out_) 
  {
    fprintf(out_, PAnsiChar(AnsiString(PSH.OriginalToString(pPSDef))));
    fclose(out_);
  }
}

PSH_RECOMPILED_SHADER XTL_EmuRecompilePshDef(XTL::X_D3DPIXELSHADERDEF *pPSDef);
{
	PSH_XBOX_SHADER PSH;
	return PSH.Decode(pPSDef);
}

#endif // Dxbx

#define REVEL8N_PIXEL_SHADER_CHANGES

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
	printf("// Texture Modes\n");
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
		char *szNewCodeBuffer = (char*)malloc((strlen(szCode)+20)*sizeof(char));
		strncpy(szNewCodeBuffer, szCode, iPreRunLen);
		strcat(szNewCodeBuffer, "mov r0, t0.a\n");
		strcat(szNewCodeBuffer, &szCode[iPreRunLen]);
		strcpy(szCode, szNewCodeBuffer);

		free(szNewCodeBuffer);
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
			EmuWarning("Couldn't assemble recompiled pixel shader");
			if (pCompilationErrors)
			{
				EmuWarning((const char*)pCompilationErrors->GetBufferPointer());
			}
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		DbgPrintf("Pixel Shader : Exception while creating pixel shader 0x%.8X\n", pPSD);
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

#ifdef REVEL8N_PIXEL_SHADER_CHANGES
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
#ifdef REVEL8N_PIXEL_SHADER_CHANGES
		if(bInput[i])
		{
#endif
			strcat(szCompleteInput[i], szInput[i]);
#ifdef REVEL8N_PIXEL_SHADER_CHANGES
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
#ifdef REVEL8N_PIXEL_SHADER_CHANGES
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

#ifndef REVEL8N_PIXEL_SHADER_CHANGES
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

#ifdef REVEL8N_PIXEL_SHADER_CHANGES
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
#ifdef REVEL8N_PIXEL_SHADER_CHANGES
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
#ifdef REVEL8N_PIXEL_SHADER_CHANGES
			bOptimized &&
#endif
			(szCmd[0] || 
			(szOut[0] && !bVAccess[0]) || 
			(szOut1[0] && !bVAccess[1]) || 
			(szOutput[2][0] && !bVAccess[2]))) 
		{
//#ifndef REVEL8N_PIXEL_SHADER_CHANGES
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
				EmuWarning("EF Product and V1R0 register used at the same time!");
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

#ifdef REVEL8N_PIXEL_SHADER_CHANGES
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
#ifndef REVEL8N_PIXEL_SHADER_CHANGES
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
#ifndef REVEL8N_PIXEL_SHADER_CHANGES
				strcpy(szABCDInput[i], szRealInputs[i*2]);
#endif

				strcpy(szOps[i], "mov");

				strcpy(szInputs[i*2+1], "");
				strcpy(szRealInputs[i*2+1], "");

				bMov[i]=TRUE;
			}
		}
	}

#ifdef REVEL8N_PIXEL_SHADER_CHANGES
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
				EmuWarning("Pixel Shader: Destination modifier present!");
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
							EmuWarning("Pixel Shader: Destination not temporary register!");
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
				EmuWarning("Unhandled pixel shader instruction!");
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
// 					EmuWarning("Unhandled pixel shader instruction!");
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
// 					EmuWarning("Unhandled pixel shader instruction!");
// 				}
// 			}
// 			else
// 			{
// 				EmuWarning("Unhandled pixel shader instruction!");
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
#ifdef REVEL8N_PIXEL_SHADER_CHANGES
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

