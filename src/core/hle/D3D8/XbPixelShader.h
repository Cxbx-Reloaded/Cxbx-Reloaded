// ******************************************************************
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
#ifndef XBPIXELSHADER_H
#define XBPIXELSHADER_H

#include "Cxbx.h"

#include "core\hle\D3D8\XbD3D8Types.h"

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
// --------.--------.--------.---xxxxx stage 0 PS_TEXTUREMODES
// --------.--------.------xx.xxx----- stage 1 PS_TEXTUREMODES
// --------.--------.-xxxxx--.-------- stage 2 PS_TEXTUREMODES
// --------.----xxxx.x-------.-------- stage 3 PS_TEXTUREMODES

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
                       eyeSigma = Sigma of eye vector in spherical coordinates, read from stage-2 as (16 bit phi,sigma)
                       lightSigma = Sigma of light vector in spherical coordinates, read from stage-1 as (16 bit phi,sigma)
                       dPhi = Phi of eye - Phi of light
DOT_ST         :argb = texture(<DotResult of stage-1>, (s,t,r).(src.r,src.g,src.b))
DOT_ZW         :frag depth = (<DotResult of stage-1>/((s,t,r).(src.r,src.g,src.b))
DOT_RFLCT_DIFF :n = (<DotResult of stage-1>,(s,t,r).(src.r,src.g,src.b),<DotResult of stage+1>)
                argb = cubemap(n)
DOT_RFLCT_SPEC :n = (<DotResult of stage-2>,<DotResult of stage-1>,(s,t,r).(src.r,src.g,src.b))
                r = 2*n*(n.e)/(n.n) - e where e is eye vector built from q texture coordinate of each stage
                argb = cubemap(r)
DOT_STR_3D     :argb=texture((<DotResult of stage-2>,<DotResult of stage-1>,(s,t,r).(src.r,src.g,src.b)))
DOT_STR_CUBE   :argb=cubemap((<DotResult of stage-2>,<DotResult of stage-1>,(s,t,r).(src.r,src.g,src.b)))
DEPENDENT_AR   :argb = texture(src.a, src.r)
DEPENDENT_GB   :argb = texture(src.g, src.b)
DOTPRODUCT     :argb = (s,t,r).(src.r,src.g,src.b)
DOT_RFLCT_SPEC_CONST :n = (<DotResult of stage-2>,<DotResult of stage-1>,(s,t,r).(src.r,src.g,src.b))
                r = 2*n*(n.e)/(n.n) - e where e is eye vector set via SetEyeVector() into c0
                argb = cubemap(r)
*/

enum PS_TEXTUREMODES
{                                 // valid in stage 0 1 2 3 Uses
    PS_TEXTUREMODES_NONE=                 0x00L, // * * * *
    PS_TEXTUREMODES_PROJECT2D=            0x01L, // * * * * Sample
    PS_TEXTUREMODES_PROJECT3D=            0x02L, // * * * * Sample
    PS_TEXTUREMODES_CUBEMAP=              0x03L, // * * * * Sample
    PS_TEXTUREMODES_PASSTHRU=             0x04L, // * * * *
    PS_TEXTUREMODES_CLIPPLANE=            0x05L, // * * * * PSCompareMode
    PS_TEXTUREMODES_BUMPENVMAP=           0x06L, // - * * * Sample, PSInputTexture
    PS_TEXTUREMODES_BUMPENVMAP_LUM=       0x07L, // - * * * Sample, PSInputTexture
    PS_TEXTUREMODES_BRDF=                 0x08L, // - - * *
    PS_TEXTUREMODES_DOT_ST=               0x09L, // - - * * Sample, PSInputTexture, PSDotMapping
    PS_TEXTUREMODES_DOT_ZW=               0x0aL, // - - * * PSInputTexture, PSDotMapping
    PS_TEXTUREMODES_DOT_RFLCT_DIFF=       0x0bL, // - - * - Sample, PSInputTexture, PSDotMapping
    PS_TEXTUREMODES_DOT_RFLCT_SPEC=       0x0cL, // - - - * Sample, PSInputTexture, PSDotMapping
    PS_TEXTUREMODES_DOT_STR_3D=           0x0dL, // - - - * Sample, PSInputTexture, PSDotMapping
    PS_TEXTUREMODES_DOT_STR_CUBE=         0x0eL, // - - - * Sample, PSInputTexture, PSDotMapping
    PS_TEXTUREMODES_DPNDNT_AR=            0x0fL, // - * * * Sample, PSInputTexture
    PS_TEXTUREMODES_DPNDNT_GB=            0x10L, // - * * * Sample, PSInputTexture
    PS_TEXTUREMODES_DOTPRODUCT=           0x11L, // - * * - PSInputTexture
    PS_TEXTUREMODES_DOT_RFLCT_SPEC_CONST= 0x12L, // - - - * Sample, PSInputTexture, PSDotMapping
    // 0x13-0x1f reserved

    PS_TEXTUREMODES_MASK=                 0x1fL
};

// =========================================================================================================
// PSDotMapping
// --------.--------.--------.-----xxx // stage 1 PS_DOTMAPPING
// --------.--------.--------.-xxx---- // stage 2 PS_DOTMAPPING
// --------.--------.-----xxx.-------- // stage 3 PS_DOTMAPPING

#define PS_DOTMAPPING(t0,t1,t2,t3) (((t3)<<8)|((t2)<<4)|(t1))

// Dot mappings over the output value of a (4 component 8 bit unsigned) texture stage register into a (3 component float) vector value, for use in a dot product calculation:
// PS_DOTMAPPING_ZERO_TO_ONE         :r8g8b8a8->(r,g,b):                                                   0x00=>0,                       0xff=>1 thus : output =                     (input / 0xff  )
// PS_DOTMAPPING_MINUS1_TO_1_D3D     :r8g8b8a8->(r,g,b):               0x00=>-128/127,         0x01=>-1,   0x80=>0,                       0xff=>1 thus : output =                                        ((input - 0x100  ) / 0x7f  )
// PS_DOTMAPPING_MINUS1_TO_1_GL      :r8g8b8a8->(r,g,b):                                       0x80=>-1,   0x00=>0,                       0x7f=>1 thus : output =  (input < 0x80  ) ? (input / 0x7f  ) : ((input - 0x100  ) / 0x80  ) (see https://en.wikipedia.org/wiki/Two's_complement)
// PS_DOTMAPPING_MINUS1_TO_1         :r8g8b8a8->(r,g,b):               0x80=>-128/127,        ?0x81=>-1,   0x00=>0,                       0x7f=>1 thus : output =  (input < 0x80  ) ? (input / 0x7f  ) : ((input - 0x100  ) / 0x7f  ) (see https://en.wikipedia.org/wiki/Two's_complement)
// PS_DOTMAPPING_HILO_1              :H16L16  ->(H,L,1):                                                 0x0000=>0,                     0xffff=>1 thus : output =                     (input / 0xffff)
// PS_DOTMAPPING_HILO_HEMISPHERE_D3D :H16L16  ->(H,L,sqrt(1-H*H-L*L)):?                      0x8000=>-1, 0x0000=>0, 0x7fff=32767/32768            thus : output =                                        ((input - 0x10000) / 0x7fff)
// PS_DOTMAPPING_HILO_HEMISPHERE_GL  :H16L16  ->(H,L,sqrt(1-H*H-L*L)):?                      0x8000=>-1, 0x0000=>0,                     0x7fff=>1 thus : output =  (input < 0x8000) ? (input / 0x7fff) : ((input - 0x10000) / 0x8000)
// PS_DOTMAPPING_HILO_HEMISPHERE     :H16L16  ->(H,L,sqrt(1-H*H-L*L)): 0x8000=>-32768/32767, 0x8001=>-1, 0x0000=>0,                     0x7fff=>1 thus : output =  (input < 0x8000) ? (input / 0x7fff) : ((input - 0x10000) / 0x7fff)

enum PS_DOTMAPPING
{                              // valid in stage 0 1 2 3
    PS_DOTMAPPING_ZERO_TO_ONE=         0x00L, // - * * *
    PS_DOTMAPPING_MINUS1_TO_1_D3D=     0x01L, // - * * *
    PS_DOTMAPPING_MINUS1_TO_1_GL=      0x02L, // - * * *
    PS_DOTMAPPING_MINUS1_TO_1=         0x03L, // - * * *
    PS_DOTMAPPING_HILO_1=              0x04L, // - * * *
    PS_DOTMAPPING_HILO_HEMISPHERE_D3D= 0x05L, // - * * *
    PS_DOTMAPPING_HILO_HEMISPHERE_GL=  0x06L, // - * * *
    PS_DOTMAPPING_HILO_HEMISPHERE=     0x07L, // - * * *

    PS_DOTMAPPING_MASK=                0x07L
};

// =========================================================================================================
// PSCompareMode
// --------.--------.--------.----xxxx // stage 0 PS_COMPAREMODE
// --------.--------.--------.xxxx---- // stage 1 PS_COMPAREMODE
// --------.--------.----xxxx.-------- // stage 2 PS_COMPAREMODE
// --------.--------.xxxx----.-------- // stage 3 PS_COMPAREMODE

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

    PS_COMPAREMODE_MASK= 0x0fL
};

// =========================================================================================================
// PSInputTexture
// --------.-------x.--------.-------- // stage 2
// --------.--xx----.--------.-------- // stage 3
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
// --------.--------.-------x.-------- // PS_COMBINERCOUNT_MUX_MSB bit (0= LSB, 1= MSB)
// --------.--------.---x----.-------- // PS_COMBINERCOUNT_UNIQUE_C0
// --------.-------x.--------.-------- // PS_COMBINERCOUNT_UNIQUE_C1

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
// --------.--------.--------.----xxxx // D PS_REGISTER
// --------.--------.--------.---x---- // D PS_CHANNEL (0= RGB/BLUE, 1= ALPHA)
// --------.--------.--------.xxx----- // D PS_INPUTMAPPING
// --------.--------.----xxxx.-------- // C PS_REGISTER
// --------.--------.---x----.-------- // C PS_CHANNEL (0= RGB/BLUE, 1= ALPHA)
// --------.--------.xxx-----.-------- // C PS_INPUTMAPPING
// --------.----xxxx.--------.-------- // B PS_REGISTER
// --------.---x----.--------.-------- // B PS_CHANNEL (0= RGB/BLUE, 1= ALPHA)
// --------.xxx-----.--------.-------- // B PS_INPUTMAPPING
// ----xxxx.--------.--------.-------- // A PS_REGISTER
// ---x----.--------.--------.-------- // A PS_CHANNEL (0= RGB/BLUE, 1= ALPHA)
// xxx-----.--------.--------.-------- // A PS_INPUTMAPPING

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

// The input has PS_INPUTMAPPING applied
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
    PS_INPUTMAPPING_UNSIGNED_IDENTITY= 0x00L, //         max(0,x)       =  1*max(0,x) + 0.0   OK for final combiner: y = abs(x)
    PS_INPUTMAPPING_UNSIGNED_INVERT=   0x20L, //     1 - max(0,x)       = -1*max(0,x) + 1.0   OK for final combiner: y = 1 - x
    PS_INPUTMAPPING_EXPAND_NORMAL=     0x40L, //     2 * max(0,x) - 1   =  2*max(0,x) - 1.0   invalid for final combiner
    PS_INPUTMAPPING_EXPAND_NEGATE=     0x60L, // 1 - 2 * max(0,x)       = -2*max(0,x) + 1.0   invalid for final combiner
    PS_INPUTMAPPING_HALFBIAS_NORMAL=   0x80L, //         max(0,x) - 1/2 =  1*max(0,x) - 0.5   invalid for final combiner
    PS_INPUTMAPPING_HALFBIAS_NEGATE=   0xa0L, // 1/2 -   max(0,x)       = -1*max(0,x) + 0.5   invalid for final combiner
    PS_INPUTMAPPING_SIGNED_IDENTITY=   0xc0L, //               x        =  1*      x  + 0.0   invalid for final combiner
    PS_INPUTMAPPING_SIGNED_NEGATE=     0xe0L, //              -x        = -1*      x  + 0.0   invalid for final combiner

    PS_INPUTMAPPING_MASK=              0xe0L
};

enum PS_REGISTER
{
    PS_REGISTER_ZERO=              0x00L, // r    A.k.a. _REG_0
    PS_REGISTER_DISCARD=           0x00L, //   w  A.k.a. _REG_0
    PS_REGISTER_C0=                0x01L, // r    A.k.a. _REG_1
    PS_REGISTER_C1=                0x02L, // r    A.k.a. _REG_2
    PS_REGISTER_FOG=               0x03L, // r    A.k.a. _REG_3
    PS_REGISTER_V0=                0x04L, // r/w  A.k.a. _REG_4
    PS_REGISTER_V1=                0x05L, // r/w  A.k.a. _REG_5
    PS_REGISTER_T0=                0x08L, // r/w  A.k.a. _REG_8
    PS_REGISTER_T1=                0x09L, // r/w  A.k.a. _REG_9
    PS_REGISTER_T2=                0x0aL, // r/w  A.k.a. _REG_A
    PS_REGISTER_T3=                0x0bL, // r/w  A.k.a. _REG_B
    PS_REGISTER_R0=                0x0cL, // r/w  A.k.a. _REG_C
    PS_REGISTER_R1=                0x0dL, // r/w  A.k.a. _REG_D
    PS_REGISTER_V1R0_SUM=          0x0eL, // r    A.k.a. _REG_SPECLIT
    PS_REGISTER_EF_PROD=           0x0fL, // r    A.k.a. _REG_EF_PROD

    PS_REGISTER_MASK=              0x0fL,

    // These constant values can be represented as a combination of 0, and an input modifier
    // But they're not registers
    // PS_REGISTER_ONE=               PS_REGISTER_ZERO | PS_INPUTMAPPING_UNSIGNED_INVERT, // 0x20 r OK for final combiner
    // PS_REGISTER_NEGATIVE_ONE=      PS_REGISTER_ZERO | PS_INPUTMAPPING_EXPAND_NORMAL,   // 0x40 r invalid for final combiner
    // PS_REGISTER_ONE_HALF=          PS_REGISTER_ZERO | PS_INPUTMAPPING_HALFBIAS_NEGATE, // 0xa0 r invalid for final combiner
    // PS_REGISTER_NEGATIVE_ONE_HALF= PS_REGISTER_ZERO | PS_INPUTMAPPING_HALFBIAS_NORMAL, // 0x80 r invalid for final combiner

    // Cxbx extension; Separate final combiner constant registers  (values not encoded on NV2A, as outside of available bits range) :
    PS_REGISTER_FC0=              0x10,
    PS_REGISTER_FC1=              0x11,
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

    PS_CHANNEL_MASK=  0x10
};

enum PS_FINALCOMBINERSETTING
{
    PS_FINALCOMBINERSETTING_CLAMP_SUM=     0x80, // V1+R0 sum clamped to [0,1]                                                       A.k.a. NV097_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_CLAMP_TRUE
    PS_FINALCOMBINERSETTING_COMPLEMENT_V1= 0x40, // unsigned invert mapping  (1 - v1) is used as an input to the sum rather than v1  A.k.a. NV097_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_ADD_INVERT_R5_TRUE
    PS_FINALCOMBINERSETTING_COMPLEMENT_R0= 0x20, // unsigned invert mapping  (1 - r0) is used as an input to the sum rather than r0  A.k.a. NV097_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_ADD_INVERT_R12_TRUE
};

// =========================================================================================================
// PSRGBOutputs[0-7]
// PSAlphaOutputs[0-7]
// --------.--------.--------.----xxxx // CD output PS_REGISTER
// --------.--------.--------.xxxx---- // AB output PS_REGISTER
// --------.--------.----xxxx.-------- // AB_CD output PS_REGISTER  Note : Must be PS_REGISTER_DISCARD if either PS_COMBINEROUTPUT_AB_DOT_PRODUCT or PS_COMBINEROUTPUT_CD_DOT_PRODUCT are set
// --------.--------.---x----.-------- // PS_COMBINEROUTPUT_CD_DOT_PRODUCT (CD output 0= multiply, 1= dot product)
// --------.--------.--x-----.-------- // PS_COMBINEROUTPUT_AB_DOT_PRODUCT (AB output 0= multiply, 1= dot product)
// --------.--------.-x------.-------- // PS_COMBINEROUTPUT_AB_CD_MUX (AB_CD mux/sum select 0= sum, 1= mux)
// --------.------xx.x-------.-------- // PS_COMBINEROUTPUT_OUTPUTMAPPING
// --------.-----x--.--------.-------- // PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA
// --------.----x---.--------.-------- // PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA

#define PS_COMBINEROUTPUTS(ab,cd,mux_sum,flags) (((flags)<<12)|((mux_sum)<<8)|((ab)<<4)|(cd))
// ab,cd,mux_sum contain a value from PS_REGISTER
// flags contains values from PS_COMBINEROUTPUT

enum PS_COMBINEROUTPUT_OUTPUTMAPPING
{
    PS_COMBINEROUTPUT_OUTPUTMAPPING_IDENTITY=            0x00L, // y =  x
    PS_COMBINEROUTPUT_OUTPUTMAPPING_BIAS=                0x08L, // y = (x - 0.5)
    PS_COMBINEROUTPUT_OUTPUTMAPPING_SHIFTLEFT_1=         0x10L, // y =  x        * 2
    PS_COMBINEROUTPUT_OUTPUTMAPPING_SHIFTLEFT_1_BIAS=    0x18L, // y = (x - 0.5) * 2
    PS_COMBINEROUTPUT_OUTPUTMAPPING_SHIFTLEFT_2=         0x20L, // y =  x        * 4
    PS_COMBINEROUTPUT_OUTPUTMAPPING_SHIFTLEFT_2_BIAS=    0x28L, // y = (x - 0.5) * 4  Note : Cxbx inferred method; May not be supported on NV2A
    PS_COMBINEROUTPUT_OUTPUTMAPPING_SHIFTRIGHT_1=        0x30L, // y =  x        / 2
    PS_COMBINEROUTPUT_OUTPUTMAPPING_SHIFTRIGHT_1_BIAS=   0x38L, // y = (x - 0.5) / 2  Note : Cxbx inferred method; May not be supported on NV2A

    PS_COMBINEROUTPUT_OUTPUTMAPPING_MASK=                0x38L
};

enum PS_COMBINEROUTPUT
{
    PS_COMBINEROUTPUT_CD_MULTIPLY=         0x00L,
    PS_COMBINEROUTPUT_CD_DOT_PRODUCT=      0x01L, // RGB only

    PS_COMBINEROUTPUT_AB_MULTIPLY=         0x00L,
    PS_COMBINEROUTPUT_AB_DOT_PRODUCT=      0x02L, // RGB only

    PS_COMBINEROUTPUT_AB_CD_SUM=           0x00L, // 3rd output is AB+CD
    PS_COMBINEROUTPUT_AB_CD_MUX=           0x04L, // 3rd output is MUX(AB,CD) based on R0.a

    PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA=    0x40L, // RGB only

    PS_COMBINEROUTPUT_AB_BLUE_TO_ALPHA=    0x80L, // RGB only
};

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

    PS_GLOBALFLAGS_NO_TEXMODE_ADJUST=     0x0000L, // don't adjust texture modes
    PS_GLOBALFLAGS_TEXMODE_ADJUST=        0x0001L, // adjust texture modes according to set texture

    PS_GLOBALFLAGS_SHIFT= 8
};


constexpr int PSH_XBOX_MAX_C_REGISTER_COUNT = 16;
constexpr int PSH_XBOX_MAX_R_REGISTER_COUNT = 2;
constexpr int PSH_XBOX_MAX_T_REGISTER_COUNT = 4;
constexpr int PSH_XBOX_MAX_V_REGISTER_COUNT = 2;


struct RPSRegisterObject {
	PS_REGISTER Reg;

	void Decode(uint8_t Value);
};

struct RPSInputRegister : RPSRegisterObject {
	PS_CHANNEL Channel;
	PS_INPUTMAPPING InputMapping;

	void Decode(uint8_t Value, unsigned stage_nr, bool isRGB);
};

struct RPSCombinerOutput : RPSRegisterObject {
	RPSInputRegister Input[2]; // Called Input A and B, or C and D (depending if it's inside the AB or CD combiner)
	bool DotProduct; // False=Multiply, True=DotProduct
	unsigned BlueToAlpha; // 0=Alpha-to-Alpha, 1=Blue-to-Alpha

	void Decode(uint8_t Value, uint16_t PSInputs, unsigned stage_nr, bool isRGB);
};

struct RPSCombinerStageChannel {
	RPSCombinerOutput OutputCD; // Contains InputC and InputD (as Input1 and Input2)
	RPSCombinerOutput OutputAB; // Contains InputA and InputB (as Input1 and Input2)
	RPSRegisterObject OutputMUX_SUM;
	bool AB_CD_MUX; // False=AB+CD, True=MUX(AB,CD) based on R0.a
	PS_COMBINEROUTPUT_OUTPUTMAPPING CombinerOutputMapping;

	void Decode(uint32_t PSInputs, uint32_t PSOutputs, unsigned stage_nr, bool isRGB);
};

struct RPSCombinerStage {
	RPSCombinerStageChannel RGB;
	RPSCombinerStageChannel Alpha;
};

struct RPSFinalCombiner {
	RPSInputRegister Input[7];
	bool ComplementV1;
	bool ComplementR0;
	bool ClampSum;

	void Decode(const uint32_t PSFinalCombinerInputsABCD, const uint32_t PSFinalCombinerInputsEFG);
};

struct DecodedRegisterCombiner {
	PS_TEXTUREMODES PSTextureModes[xbox::X_D3DTS_STAGECOUNT];
	PS_DOTMAPPING PSDotMapping[xbox::X_D3DTS_STAGECOUNT];
	bool PSCompareMode[xbox::X_D3DTS_STAGECOUNT][4]; // True in [0] = PS_COMPAREMODE_S_GE, [1] = PS_COMPAREMODE_T_GE, [2] = PS_COMPAREMODE_R_GE, [3] PS_COMPAREMODE_Q_GE (so, STRQ>0, otherwise <0)
	int PSInputTexture[xbox::X_D3DTS_STAGECOUNT];

	bool CombinerMuxesOnMsb;
	bool CombinerHasUniqueC0;
	bool CombinerHasUniqueC1;
	unsigned NumberOfCombiners;
	RPSCombinerStage Combiners[xbox::X_PSH_COMBINECOUNT];
	bool hasFinalCombiner;
	RPSFinalCombiner FinalCombiner;
	bool TexModeAdjust;
	// Variables
	bool AlphaKill[4]; // X_D3DTSS_ALPHAKILL

	static void GetPSTextureModes(xbox::X_D3DPIXELSHADERDEF* pPSDef, PS_TEXTUREMODES psTextureModes[xbox::X_D3DTS_STAGECOUNT]);
	static void GetPSDotMapping(xbox::X_D3DPIXELSHADERDEF* pPSDef, PS_DOTMAPPING psDotMapping[xbox::X_D3DTS_STAGECOUNT]);
	static void GetPSCompareModes(xbox::X_D3DPIXELSHADERDEF* pPSDef, bool psCompareModes[xbox::X_D3DTS_STAGECOUNT][4]);
	static void GetPSInputTexture(xbox::X_D3DPIXELSHADERDEF* pPSDef, int psInputTexture[xbox::X_D3DTS_STAGECOUNT]);
	void Decode(xbox::X_D3DPIXELSHADERDEF *pPSDef);
};

extern bool g_UseFixedFunctionPixelShader;

// PatrickvL's Dxbx pixel shader translation
void DxbxUpdateActivePixelShader(); // NOPATCH

#endif // PIXELSHADER_H
