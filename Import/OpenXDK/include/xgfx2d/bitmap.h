// ******************************************************************
// * 
// * proj : OpenXDK
// *
// * desc : Open Source XBox Development Kit
// *
// * file : bitmap.h
// *
// * note : Simple 2D Bitmap library
// *
// ******************************************************************

#ifndef XBITMAP_H
#define XBITMAP_H

#include "openxdk.h"

#if defined(__cplusplus)
extern "C"
{
#endif


//you shouldn't need to mess with this if you use the functions in this lib,
//but feel free to :)
//data = the raw 32-bit bitmap data
//w = width in pixels
//h = height in pixels
//pitch = memory distance between rows, in pixels (you don't need to care)

typedef struct
{
	uint32 *data;
	int w, h;
	int pitch;	// pitch is in pixels, not bytes as many libs do
				// will in most cases be same as w
} Bitmap;


//creates a 32-bit bitmap, sized WxH
Bitmap *create_bitmap(int w, int h);

//gets the screen bitmap, so you can draw to it easily
//you have to have inited VGA before calling this
//also currently UNTESTED, but should work :D
Bitmap *get_screen_bitmap();

//destroys a bitmap
void destroy_bitmap(Bitmap *bmp);

//image loaders
Bitmap *load_tga(char *filename);

#if defined(__cplusplus)
}
#endif

#endif
