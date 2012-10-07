// ******************************************************************
// * 
// * proj : OpenXDK
// *
// * desc : Open Source XBox Development Kit
// *
// * file : blit.c
// *
// * note : blitter with clipping, uses any one of the blitters from
// *        blitters.h per scanline
// *
// ******************************************************************

#ifndef XBLIT_H
#define XBLIT_H

#include <openxdk.h>
#include "xgfx2d/bitmap.h"

#if defined(__cplusplus)
extern "C"
{
#endif
	

//  INSTRUCTIONS
//  ************
//  pass in a blitter from blitters.h
//  dx,dy = destination x,y
//  sx,sy = source x,y
//  sw,sh = width and height of the copy
//  blitter = copying method, check blitters.h
//  parameter = if a blitter method needs a parameter pass it here,
//              else pass 0
//  Sample:
//  blit(source,dest,10,10,0,0,20,20,alphavalue_blit,128)
//  will blit the topleft 20x20 of source to 10,10 in dest,
//  blended ~50%  (128/255)
void blit(Bitmap *dest, Bitmap *src, 
	int dx, int dy, 
	int sx, int sy, int sw, int sh, 
	void (*blitter)(uint32*,uint32*,int,int), 
	int parameter);

//  just copies entire source bitmap to 0,0 of dest
void full_blit(Bitmap *dest, Bitmap *src, 
	void (*blitter)(uint32*,uint32*,int,int), 
	int parameter);

//  shortcut to copy entire source bitmap to a coordinate in dest
//  use for simple sprites
void blit_at(Bitmap *dest, Bitmap *src, 
	int x, int y, 
	void (*blitter)(uint32*,uint32*,int,int), 
	int parameter);
	
	
#if defined(__cplusplus)
}
#endif

#endif
