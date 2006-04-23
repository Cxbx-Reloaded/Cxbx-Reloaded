// ******************************************************************
// * 
// * proj : OpenXDK
// *
// * desc : Open Source XBox Development Kit
// *
// * file : blitters.h
// *
// * note : A bunch of fast MMX blitters of various kinds
// *        Alpha blending, additive blending, you name it
// *        Many non-ASM substitutes needs writing if we're gonna
// *        support GNU C
// *
// ******************************************************************

#ifndef XBLITTERS_H
#define XBLITTERS_H

#include <openxdk.h>
#include "xgfx2d/bitmap.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#define DECLARE_BLITTER(name,parameter) \
  void name (uint32 *s, uint32 *d, int len, int parameter) 


//you're supposed to pass these ones into blit, check blit.h

DECLARE_BLITTER(normal_blit,none);   //just a straight blit
DECLARE_BLITTER(sprite_blit,none);   //ignores 0xFF00FF
DECLARE_BLITTER(additive_blit,none); //adds images together
DECLARE_BLITTER(additive_alpha_blit,alpha); //adds image multiplied with alpha to the other one
DECLARE_BLITTER(alpha_blit,none);  //alpha from image! yes, a true alpha blender
DECLARE_BLITTER(alphavalue_blit,alpha); //1 alpha value for the entire image
DECLARE_BLITTER(alphavalue_sprite_blit,none); //1 alpha value for the entire image, ignores black
DECLARE_BLITTER(alphavalue50_blit,none); //50% alpha value for the entire image (faster?)
DECLARE_BLITTER(multiply_blit,none); //like multiply in photoshop
DECLARE_BLITTER(invert_blit,none);   //odd :)
DECLARE_BLITTER(subtractive_blit,none); //subtracts one image from the other
DECLARE_BLITTER(colorize_sprite_blit,color); //uhm, for drawing colored fonts
		
#if defined(__cplusplus)
}
#endif

#endif
	
