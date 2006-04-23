// ******************************************************************
// * 
// * proj : OpenXDK
// *
// * desc : Open Source XBox Development Kit
// *
// * file : drawprim.h
// *
// * note : A bunch of drawing primitives
// *
// ******************************************************************


#ifndef XDRAWPRIM_H
#define XDRAWPRIM_H

#include <openxdk.h>
#include "xgfx2d/bitmap.h"

#if defined(__cplusplus)
extern "C"
{
#endif

//simple straightforward functions
void clear(Bitmap *bmp, uint32 col);
void putpixel(Bitmap *bmp, int x, int y, uint32 color);
void line(Bitmap *bmp, int x1, int y1, int x2, int y2, uint32 color);

//horizontal line from x1 to x2 at y
void hline(Bitmap *bmp, int x1, int y, int x2, uint32 color);
//vertical line from y1 to y2 at x
void vline(Bitmap *bmp, int x, int y1, int y2, uint32 color);

//rectangle (boundary)
void rect(Bitmap *bmp, int x1, int y1, int x2, int y2, uint32 color);

//filled rectangle
void rectfill(Bitmap *bmp, int x1, int y1, int x2, int y2, uint32 color);

//circle (not filled)
void circle(Bitmap *bmp, int x, int y, int radius, uint32 color);

//more advanced stuff:

//antialiased pixel
//use this for smooth scrolling starfields and stuff :) 
//this one takes floats
void aa_pixel_float(Bitmap *bmp, float x,float y,int color);
//this one takes x and y in 16.16 fixed point (slightly faster)
void aa_pixel(Bitmap *bmp, int x, int y, int color);

// draws a single alpha blended pixel, used by bilinear_pixel and aa_line
//this one takes x and y in 16.16 fixed point
void alpha_pixel(Bitmap *bmp, int x, int y, uint32 sc, int alpha);

// nice antialiased line drawer
// alpha from 0..255, means transparency
void aa_line(Bitmap *bmp, float x1, float y1, float x2, float y2, uint32 color, int alpha);

#if defined(__cplusplus)
}
#endif

#endif
	
