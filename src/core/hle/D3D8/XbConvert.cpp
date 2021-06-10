// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
// *  (c) 2002-2004 Aaron Robinson <caustik@caustik.com>
// *                Kingofc <kingofc@freenet.de>
// *
// *  All rights reserved
// *
// ******************************************************************

#define LOG_PREFIX CXBXR_MODULE::D3DCVT

#include "common\Settings.hpp" // for g_LibVersion_D3D8
#include "core\kernel\support\Emu.h"

#include "XbConvert.h"

// About format color components:
// A = alpha, byte : 0 = fully opaque, 255 = fully transparent
// X = ignore these component bits
// R = red
// G = green
// B = blue
// L = luminance, byte : 0 = pure black ARGB(1, 0,0,0) to 255 = pure white ARGB(1,255,255,255)
// P = pallete
enum _ComponentEncoding {
	NoCmpnts = 0, // Format doesn't contain any component (ARGB/QWVU)
	A1R5G5B5,
	X1R5G5B5, // NOTE : A=255
	A4R4G4B4,
	__R5G6B5, // NOTE : A=255
	A8R8G8B8,
	X8R8G8B8, // NOTE : A=255
	____R8B8, // NOTE : A takes R, G takes B
	____G8B8, // NOTE : A takes G, R takes B
	______A8, // TEST : R=G=B= 255
	__R6G5B5, // NOTE : A=255
	R5G5B5A1,
	R4G4B4A4,
	A8B8G8R8,
	B8G8R8A8,
	R8G8B8A8,
	______L8, // NOTE : A=255, R=G=B= L
	_____AL8, // NOTE : A=R=G=B= L
	_____L16, // NOTE : Actually G8B8, with A=R=255
	____A8L8, // NOTE : R=G=B= L
	____DXT1,
	____DXT3,
	____DXT5,
	______P8,
	____YUY2,
	____UYVY,
};

// Conversion functions copied from libyuv
// See https://chromium.googlesource.com/libyuv/libyuv/+/master/source/row_common.cc
void RGB565ToARGBRow_C(const uint8_t* src_rgb565, uint8_t* dst_argb, int width) {
	int x;
	for (x = 0; x < width; ++x) {
        uint8_t b = src_rgb565[0] & 0x1f;
        uint8_t g = (src_rgb565[0] >> 5) | ((src_rgb565[1] & 0x07) << 3);
        uint8_t r = src_rgb565[1] >> 3;
		dst_argb[0] = (b << 3) | (b >> 2);
		dst_argb[1] = (g << 2) | (g >> 4);
		dst_argb[2] = (r << 3) | (r >> 2);
		dst_argb[3] = 255u;
		dst_argb += 4;
		src_rgb565 += 2;
	}
}
void ARGB1555ToARGBRow_C(const uint8_t* src_argb1555,
    uint8_t* dst_argb,
	int width) {
	int x;
	for (x = 0; x < width; ++x) {
        uint8_t b = src_argb1555[0] & 0x1f;
        uint8_t g = (src_argb1555[0] >> 5) | ((src_argb1555[1] & 0x03) << 3);
        uint8_t r = (src_argb1555[1] & 0x7c) >> 2;
        uint8_t a = src_argb1555[1] >> 7;
		dst_argb[0] = (b << 3) | (b >> 2);
		dst_argb[1] = (g << 3) | (g >> 2);
		dst_argb[2] = (r << 3) | (r >> 2);
		dst_argb[3] = -a;
		dst_argb += 4;
		src_argb1555 += 2;
	}
}
void ARGB4444ToARGBRow_C(const uint8_t* src_argb4444,
    uint8_t* dst_argb,
	int width) {
	int x;
	for (x = 0; x < width; ++x) {
        uint8_t b = src_argb4444[0] & 0x0f;
        uint8_t g = src_argb4444[0] >> 4;
        uint8_t r = src_argb4444[1] & 0x0f;
        uint8_t a = src_argb4444[1] >> 4;
		dst_argb[0] = (b << 4) | b;
		dst_argb[1] = (g << 4) | g;
		dst_argb[2] = (r << 4) | r;
		dst_argb[3] = (a << 4) | a;
		dst_argb += 4;
		src_argb4444 += 2;
	}
}

// Cxbx color component conversion functions 
void X1R5G5B5ToARGBRow_C(const uint8_t* src_x1r5g5b5, uint8_t* dst_argb,
	int width) {
	int x;
	for (x = 0; x < width; ++x) {
        uint8_t b = src_x1r5g5b5[0] & 0x1f;
        uint8_t g = (src_x1r5g5b5[0] >> 5) | ((src_x1r5g5b5[1] & 0x03) << 3);
        uint8_t r = (src_x1r5g5b5[1] & 0x7c) >> 2;
		dst_argb[0] = (b << 3) | (b >> 2);
		dst_argb[1] = (g << 3) | (g >> 2);
		dst_argb[2] = (r << 3) | (r >> 2);
		dst_argb[3] = 255u;
		dst_argb += 4;
		src_x1r5g5b5 += 2;
	}
}

void A8R8G8B8ToARGBRow_C(const uint8_t* src_a8r8g8b8, uint8_t* dst_argb, int width) {
	memcpy(dst_argb, src_a8r8g8b8, width * sizeof(D3DCOLOR)); // Cxbx pass-through
}

void X8R8G8B8ToARGBRow_C(const uint8_t* src_x8r8g8b8, uint8_t* dst_argb, int width) {
	int x;
	for (x = 0; x < width; ++x) {
		uint8_t b = src_x8r8g8b8[0];
		uint8_t g = src_x8r8g8b8[1];
		uint8_t r = src_x8r8g8b8[2];
		dst_argb[0] = b;
		dst_argb[1] = g;
		dst_argb[2] = r;
		dst_argb[3] = 255u;
		dst_argb += 4;
		src_x8r8g8b8 += 4;
	}
}

void ____R8B8ToARGBRow_C(const uint8_t* src_r8b8, uint8_t* dst_argb, int width) {
	int x;
	for (x = 0; x < width; ++x) {
        uint8_t b = src_r8b8[0];
        uint8_t r = src_r8b8[1];
		dst_argb[0] = b;
		dst_argb[1] = b;
		dst_argb[2] = r;
		dst_argb[3] = r;
		dst_argb += 4;
		src_r8b8 += 2;
	}
}

void ____G8B8ToARGBRow_C(const uint8_t* src_g8b8, uint8_t* dst_argb, int width) {
	int x;
	for (x = 0; x < width; ++x) {
        uint8_t b = src_g8b8[0];
        uint8_t g = src_g8b8[1];
		dst_argb[0] = b;
		dst_argb[1] = g;
		dst_argb[2] = b;
		dst_argb[3] = g;
		dst_argb += 4;
		src_g8b8 += 2;
	}
}

void ______A8ToARGBRow_C(const uint8_t* src_a8, uint8_t* dst_argb, int width) {
	int x;
	for (x = 0; x < width; ++x) {
        uint8_t a = src_a8[0];
		dst_argb[0] = 255u;
		dst_argb[1] = 255u;
		dst_argb[2] = 255u;
		dst_argb[3] = a;
		dst_argb += 4;
		src_a8 += 1;
	}
}

void __R6G5B5ToARGBRow_C(const uint8_t* src_r6g5b5, uint8_t* dst_argb, int width) {
	int x;
	for (x = 0; x < width; ++x) {
        uint8_t b = src_r6g5b5[0] & 0x1f;
        uint8_t g = (src_r6g5b5[0] >> 5) | ((src_r6g5b5[1] & 0x03) << 3);
        uint8_t r = src_r6g5b5[1] >> 2;
		dst_argb[0] = (b << 3) | (b >> 2);
		dst_argb[1] = (g << 3) | (g >> 2);
		dst_argb[2] = (r << 2) | (r >> 4);
		dst_argb[3] = 255u;
		dst_argb += 4;
		src_r6g5b5 += 2;
	}
}

void R5G5B5A1ToARGBRow_C(const uint8_t* src_r5g5b5a1, uint8_t* dst_argb, int width) {
	int x;
	for (x = 0; x < width; ++x) {
        uint8_t a = src_r5g5b5a1[0] & 1;
        uint8_t b = (src_r5g5b5a1[0] & 0x3e) >> 1;
        uint8_t g = (src_r5g5b5a1[0] >> 6) | ((src_r5g5b5a1[1] & 0x07) << 2);
        uint8_t r = (src_r5g5b5a1[1] & 0xf8) >> 3;
		dst_argb[0] = (b << 3) | (b >> 2);
		dst_argb[1] = (g << 3) | (g >> 2);
		dst_argb[2] = (r << 3) | (r >> 2);
		dst_argb[3] = -a;
		dst_argb += 4;
		src_r5g5b5a1 += 2;
	}
}

void R4G4B4A4ToARGBRow_C(const uint8_t* src_r4g4b4a4, uint8_t* dst_argb, int width) {
	int x;
	for (x = 0; x < width; ++x) {
        uint8_t a = src_r4g4b4a4[0] & 0x0f;
        uint8_t b = src_r4g4b4a4[0] >> 4;
        uint8_t g = src_r4g4b4a4[1] & 0x0f;
        uint8_t r = src_r4g4b4a4[1] >> 4;
		dst_argb[0] = (b << 4) | b;
		dst_argb[1] = (g << 4) | g;
		dst_argb[2] = (r << 4) | r;
		dst_argb[3] = (a << 4) | a;
		dst_argb += 4;
		src_r4g4b4a4 += 2;
	}
}

void A8B8G8R8ToARGBRow_C(const uint8_t* src_a8b8g8r8, uint8_t* dst_argb, int width) {
	int x;
	for (x = 0; x < width; ++x) {
        uint8_t r = src_a8b8g8r8[0];
        uint8_t g = src_a8b8g8r8[1];
        uint8_t b = src_a8b8g8r8[3];
        uint8_t a = src_a8b8g8r8[4];
		dst_argb[0] = b;
		dst_argb[1] = g;
		dst_argb[2] = r;
		dst_argb[3] = a;
		dst_argb += 4;
		src_a8b8g8r8 += 4;
	}
}

void B8G8R8A8ToARGBRow_C(const uint8_t* src_b8g8r8a8, uint8_t* dst_argb, int width) {
	int x;
	for (x = 0; x < width; ++x) {
        uint8_t a = src_b8g8r8a8[0];
        uint8_t r = src_b8g8r8a8[1];
        uint8_t g = src_b8g8r8a8[3];
        uint8_t b = src_b8g8r8a8[4];
		dst_argb[0] = b;
		dst_argb[1] = g;
		dst_argb[2] = r;
		dst_argb[3] = a;
		dst_argb += 4;
		src_b8g8r8a8 += 4;
	}
}

void R8G8B8A8ToARGBRow_C(const uint8_t* src_r8g8b8a8, uint8_t* dst_argb, int width) {
	int x;
	for (x = 0; x < width; ++x) {
        uint8_t a = src_r8g8b8a8[0];
        uint8_t b = src_r8g8b8a8[1];
        uint8_t g = src_r8g8b8a8[3];
        uint8_t r = src_r8g8b8a8[4];
		dst_argb[0] = b;
		dst_argb[1] = g;
		dst_argb[2] = r;
		dst_argb[3] = a;
		dst_argb += 4;
		src_r8g8b8a8 += 4;
	}
}

void ______L8ToARGBRow_C(const uint8_t* src_l8, uint8_t* dst_argb, int width) {
	int x;
	for (x = 0; x < width; ++x) {
        uint8_t l = src_l8[0];
		dst_argb[0] = l;
		dst_argb[1] = l;
		dst_argb[2] = l;
		dst_argb[3] = 255u;
		dst_argb += 4;
		src_l8 += 1;
	}
}

void _____AL8ToARGBRow_C(const uint8_t* src_al8, uint8_t* dst_argb, int width) {
	int x;
	for (x = 0; x < width; ++x) {
        uint8_t l = src_al8[0];
		dst_argb[0] = l;
		dst_argb[1] = l;
		dst_argb[2] = l;
		dst_argb[3] = l;
		dst_argb += 4;
		src_al8 += 1;
	}
}

void _____L16ToARGBRow_C(const uint8_t* src_l16, uint8_t* dst_argb, int width) {
	int x;
	for (x = 0; x < width; ++x) {
        uint8_t b = src_l16[0];
        uint8_t g = src_l16[1];
		dst_argb[0] = b;
		dst_argb[1] = g;
		dst_argb[2] = 255u;
		dst_argb[3] = 255u;
		dst_argb += 4;
		src_l16 += 2;
	}
}

void ____A8L8ToARGBRow_C(const uint8_t* src_a8l8, uint8_t* dst_argb, int width) {
	int x;
	for (x = 0; x < width; ++x) {
        uint8_t l = src_a8l8[0];
        uint8_t a = src_a8l8[1];
		dst_argb[0] = l;
		dst_argb[1] = l;
		dst_argb[2] = l;
		dst_argb[3] = a;
		dst_argb += 4;
		src_a8l8 += 2;
	}
}

typedef struct TRGB32
{
	unsigned char B;
	unsigned char G;
	unsigned char R;
	unsigned char A;
} TRGB32;

// DXT1 info (MSDN Block Compression) : https://msdn.microsoft.com/en-us/library/bb694531.aspx
// https://msdn.microsoft.com/en-us/library/windows/desktop/bb147243(v=vs.85).aspx
void ____DXT1ToARGBRow_C(const uint8_t* data, uint8_t* dst_argb, int width) {
	int dst_pitch = *(int*)dst_argb; // dirty hack to avoid another argument
	int x;
	for (x = 0; x < width; x+=4) {
		// Read two 16-bit pixels
		uint16_t color_0 = data[0] | (data[1] << 8);
		uint16_t color_1 = data[2] | (data[3] << 8);

		// Read 5+6+5 bit color channels
        uint8_t b0 = color_0 & 0x1f;
        uint8_t g0 = (color_0 >> 5) & 0x3f;
        uint8_t r0 = color_0 >> 11;

        uint8_t b1 = color_1 & 0x1f;
        uint8_t g1 = (color_1 >> 5) & 0x3f;
        uint8_t r1 = color_1 >> 11;

		// Build first half of RGB32 color map (converting 5+6+5 to 8+8+8):
		TRGB32 colormap[4];
		colormap[0].B = (b0 << 3) | (b0 >> 2);
		colormap[0].G = (g0 << 2) | (g0 >> 4);
		colormap[0].R = (r0 << 3) | (r0 >> 2);
		colormap[0].A = 255u;

		colormap[1].B = (b1 << 3) | (b1 >> 2);
		colormap[1].G = (g1 << 2) | (g1 >> 4);
		colormap[1].R = (r1 << 3) | (r1 >> 2);
		colormap[1].A = 255u;

		// Build second half of RGB32 color map :
		if (color_0 > color_1)
		{
			// Make up new color : 2/3 A + 1/3 B :
			colormap[2].B = (uint8_t)((2 * colormap[0].B + 1 * colormap[1].B + 2) / 3);
			colormap[2].G = (uint8_t)((2 * colormap[0].G + 1 * colormap[1].G + 2) / 3);
			colormap[2].R = (uint8_t)((2 * colormap[0].R + 1 * colormap[1].R + 2) / 3);
			colormap[2].A = 255u;
			// Make up new color : 1/3 A + 2/3 B :
			colormap[3].B = (uint8_t)((1 * colormap[0].B + 2 * colormap[1].B + 2) / 3);
			colormap[3].G = (uint8_t)((1 * colormap[0].G + 2 * colormap[1].G + 2) / 3);
			colormap[3].R = (uint8_t)((1 * colormap[0].R + 2 * colormap[1].R + 2) / 3);
			colormap[3].A = 255u;
		}
		else
		{
			// Make up one new color : 1/2 A + 1/2 B :
			colormap[2].B = (uint8_t)((colormap[0].B + colormap[1].B + 1) / 2);
			colormap[2].G = (uint8_t)((colormap[0].G + colormap[1].G + 1) / 2);
			colormap[2].R = (uint8_t)((colormap[0].R + colormap[1].R + 1) / 2);
			colormap[2].A = 255u;

			colormap[3].B = 0u;
			colormap[3].G = 0u;
			colormap[3].R = 0u;
			colormap[3].A = 0u;
		}

        uint8_t indices0 = data[4];
        uint8_t indices1 = data[5];
		uint8_t indices2 = data[6];
		uint8_t indices3 = data[7];

		TRGB32 *dst_line0 = (TRGB32*)(dst_argb);
		TRGB32 *dst_line1 = (TRGB32*)(dst_argb + dst_pitch);
		TRGB32 *dst_line2 = (TRGB32*)(dst_argb + dst_pitch * 2);
		TRGB32 *dst_line3 = (TRGB32*)(dst_argb + dst_pitch * 3);

		dst_line0[0] = colormap[indices0 & 0x03];
		dst_line0[1] = colormap[(indices0 & 0x0c) >> 2];
		dst_line0[2] = colormap[(indices0 & 0x30) >> 4];
		dst_line0[3] = colormap[indices0 >> 6];

		dst_line1[0] = colormap[indices1 & 0x03];
		dst_line1[1] = colormap[(indices1 & 0x0c) >> 2];
		dst_line1[2] = colormap[(indices1 & 0x30) >> 4];
		dst_line1[3] = colormap[indices1 >> 6];

		dst_line2[0] = colormap[indices2 & 0x03];
		dst_line2[1] = colormap[(indices2 & 0x0c) >> 2];
		dst_line2[2] = colormap[(indices2 & 0x30) >> 4];

		dst_line2[3] = colormap[indices2 >> 6];

		dst_line3[0] = colormap[indices3 & 0x03];
		dst_line3[1] = colormap[(indices3 & 0x0c) >> 2];
		dst_line3[2] = colormap[(indices3 & 0x30) >> 4];
		dst_line3[3] = colormap[indices3 >> 6];

		data += 8;
		dst_argb += 16;
	}
}

// DXT3 info : https://en.wikipedia.org/wiki/S3_Texture_Compression#DXT2_and_DXT3
void ____DXT3ToARGBRow_C(const uint8_t* data, uint8_t* dst_argb, int width) {
	int dst_pitch = *(int*)dst_argb; // dirty hack to avoid another argument
	int x;
	for (x = 0; x < width; x += 4) {
		// Read 16 pixels of 4-bit alpha channel data
		uint8_t alpha0 = data[0];
		uint8_t alpha1 = data[1];
		uint8_t alpha2 = data[2];
		uint8_t alpha3 = data[3];
		uint8_t alpha4 = data[4];
		uint8_t alpha5 = data[5];
		uint8_t alpha6 = data[6];
		uint8_t alpha7 = data[7];

		// Read two 16-bit pixels
        uint16_t color_0 = data[8] | (data[9] << 8);
        uint16_t color_1 = data[10] | (data[11] << 8);

		// Read 5+6+5 bit color channels
		uint8_t b0 = color_0 & 0x1f;
		uint8_t g0 = (color_0 >> 5) & 0x3f;
		uint8_t r0 = color_0 >> 11;

		uint8_t b1 = color_1 & 0x1f;
		uint8_t g1 = (color_1 >> 5) & 0x3f;
		uint8_t r1 = color_1 >> 11;

		// Build first half of RGB32 color map (converting 5+6+5 to 8+8+8):
		TRGB32 colormap[4];
		colormap[0].B = (b0 << 3) | (b0 >> 2);
		colormap[0].G = (g0 << 2) | (g0 >> 4);
		colormap[0].R = (r0 << 3) | (r0 >> 2);

		colormap[1].B = (b1 << 3) | (b1 >> 2);
		colormap[1].G = (g1 << 2) | (g1 >> 4);
		colormap[1].R = (r1 << 3) | (r1 >> 2);

		// Build second half of RGB32 color map :
		// Make up new color : 2/3 A + 1/3 B :
		colormap[2].B = (uint8_t)((2 * colormap[0].B + 1 * colormap[1].B + 2) / 3);
		colormap[2].G = (uint8_t)((2 * colormap[0].G + 1 * colormap[1].G + 2) / 3);
		colormap[2].R = (uint8_t)((2 * colormap[0].R + 1 * colormap[1].R + 2) / 3);
		// Make up new color : 1/3 A + 2/3 B :
		colormap[3].B = (uint8_t)((1 * colormap[0].B + 2 * colormap[1].B + 2) / 3);
		colormap[3].G = (uint8_t)((1 * colormap[0].G + 2 * colormap[1].G + 2) / 3);
		colormap[3].R = (uint8_t)((1 * colormap[0].R + 2 * colormap[1].R + 2) / 3);

		// Read 4 bytes worth of 2-bit color indices for 16 pixels
		uint8_t colori0 = data[12];
		uint8_t colori1 = data[13];
		uint8_t colori2 = data[14];
		uint8_t colori3 = data[15];

		TRGB32 *dst_line0 = (TRGB32*)(dst_argb);
		TRGB32 *dst_line1 = (TRGB32*)(dst_argb + dst_pitch);
		TRGB32 *dst_line2 = (TRGB32*)(dst_argb + dst_pitch * 2);
		TRGB32 *dst_line3 = (TRGB32*)(dst_argb + dst_pitch * 3);

		dst_line0[0] = colormap[colori0 & 0x03];
		dst_line0[0].A = (alpha0 & 0x0f) | (alpha0 << 4);
		dst_line0[1] = colormap[(colori0 & 0x0c) >> 2];
		dst_line0[1].A = (alpha0 & 0xf0) | (alpha0 >> 4);
		dst_line0[2] = colormap[(colori0 & 0x30) >> 4];
		dst_line0[2].A = (alpha1 & 0x0f) | (alpha1 << 4);
		dst_line0[3] = colormap[colori0 >> 6];
		dst_line0[3].A = (alpha1 & 0xf0) | (alpha1 >> 4);

		dst_line1[0] = colormap[colori1 & 0x03];
		dst_line1[0].A = (alpha2 & 0x0f) | (alpha2 << 4);
		dst_line1[1] = colormap[(colori1 & 0x0c) >> 2];
		dst_line1[1].A = (alpha2 & 0xf0) | (alpha2 >> 4);
		dst_line1[2] = colormap[(colori1 & 0x30) >> 4];
		dst_line1[2].A = (alpha3 & 0x0f) | (alpha3 << 4);
		dst_line1[3] = colormap[colori1 >> 6];
		dst_line1[3].A = (alpha3 & 0xf0) | (alpha3 >> 4);

		dst_line2[0] = colormap[colori2 & 0x03];
		dst_line2[0].A = (alpha4 & 0x0f) | (alpha4 << 4);
		dst_line2[1] = colormap[(colori2 & 0x0c) >> 2];
		dst_line2[1].A = (alpha4 & 0xf0) | (alpha4 >> 4);
		dst_line2[2] = colormap[(colori2 & 0x30) >> 4];
		dst_line2[2].A = (alpha5 & 0x0f) | (alpha5 << 4);
		dst_line2[3] = colormap[colori2 >> 6];
		dst_line2[3].A = (alpha5 & 0xf0) | (alpha5 >> 4);

		dst_line3[0] = colormap[colori3 & 0x03];
		dst_line3[0].A = (alpha6 & 0x0f) | (alpha6 << 4);
		dst_line3[1] = colormap[(colori3 & 0x0c) >> 2];
		dst_line3[1].A = (alpha6 & 0xf0) | (alpha6 >> 4);
		dst_line3[2] = colormap[(colori3 & 0x30) >> 4];
		dst_line3[2].A = (alpha7 & 0x0f) | (alpha7 << 4);
		dst_line3[3] = colormap[colori3 >> 6];
		dst_line3[3].A = (alpha7 & 0xf0) | (alpha7 >> 4);

		data += 16;
		dst_argb += 16;
	}
}

// DXT5 info : http://www.matejtomcik.com/Public/KnowHow/DXTDecompression/
void ____DXT5ToARGBRow_C(const uint8_t* data, uint8_t* dst_argb, int width) {
	int dst_pitch = *(int*)dst_argb; // dirty hack to avoid another argument
	int x;
	for (x = 0; x < width; x += 4) {
		// Read two 8-bit alphas
		uint8_t alphamap[8];
		alphamap[0] = data[0];
		alphamap[1] = data[1];

		// Build rest of alpha map
		if (alphamap[0] > alphamap[1]) {
			alphamap[2] = (6 * alphamap[0] + 1 * alphamap[1] + 6) / 7;
			alphamap[3] = (5 * alphamap[0] + 2 * alphamap[1] + 6) / 7;
			alphamap[4] = (4 * alphamap[0] + 3 * alphamap[1] + 6) / 7;
			alphamap[5] = (3 * alphamap[0] + 4 * alphamap[1] + 6) / 7;
			alphamap[6] = (2 * alphamap[0] + 5 * alphamap[1] + 6) / 7;
			alphamap[7] = (1 * alphamap[0] + 6 * alphamap[1] + 6) / 7;
		}
		else {
			alphamap[2] = (4 * alphamap[0] + 1 * alphamap[1] + 4) / 5;
			alphamap[3] = (3 * alphamap[0] + 2 * alphamap[1] + 4) / 5;
			alphamap[4] = (2 * alphamap[0] + 3 * alphamap[1] + 4) / 5;
			alphamap[5] = (1 * alphamap[0] + 4 * alphamap[1] + 4) / 5;
			alphamap[6] = 0u;
			alphamap[7] = 255u;
		}

		// Read 6 bytes worth of 3-bit alpha channal indices for 16 pixels
		uint8_t alphai0 = data[2];
		uint8_t alphai1 = data[3];
		uint8_t alphai2 = data[4];
		uint8_t alphai3 = data[5];
		uint8_t alphai4 = data[6];
		uint8_t alphai5 = data[7];

		// Read two 16-bit colors
		uint16_t color_0 = data[8] | (data[9] << 8);
        uint16_t color_1 = data[10] | (data[11] << 8);

		// Read 5+6+5 bit color channels
		uint8_t b0 = color_0 & 0x1f;
		uint8_t g0 = (color_0 >> 5) & 0x3f;
		uint8_t r0 = color_0 >> 11;

		uint8_t b1 = color_1 & 0x1f;
		uint8_t g1 = (color_1 >> 5) & 0x3f;
		uint8_t r1 = color_1 >> 11;

		// Build first half of RGB32 color map (converting 5+6+5 to 8+8+8):
		TRGB32 colormap[4];
		colormap[0].B = (b0 << 3) | (b0 >> 2);
		colormap[0].G = (g0 << 2) | (g0 >> 4);
		colormap[0].R = (r0 << 3) | (r0 >> 2);

		colormap[1].B = (b1 << 3) | (b1 >> 2);
		colormap[1].G = (g1 << 2) | (g1 >> 4);
		colormap[1].R = (r1 << 3) | (r1 >> 2);

		// Build second half of RGB32 color map :
		// Make up new color : 2/3 A + 1/3 B :
		colormap[2].B = (uint8_t)((2 * colormap[0].B + 1 * colormap[1].B + 2) / 3);
		colormap[2].G = (uint8_t)((2 * colormap[0].G + 1 * colormap[1].G + 2) / 3);
		colormap[2].R = (uint8_t)((2 * colormap[0].R + 1 * colormap[1].R + 2) / 3);
		// Make up new color : 1/3 A + 2/3 B :
		colormap[3].B = (uint8_t)((1 * colormap[0].B + 2 * colormap[1].B + 2) / 3);
		colormap[3].G = (uint8_t)((1 * colormap[0].G + 2 * colormap[1].G + 2) / 3);
		colormap[3].R = (uint8_t)((1 * colormap[0].R + 2 * colormap[1].R + 2) / 3);

		// Read 4 bytes worth of 2-bit color indices for 16 pixels
		uint8_t colori0 = data[12];
		uint8_t colori1 = data[13];
		uint8_t colori2 = data[14];
		uint8_t colori3 = data[15];

		TRGB32 *dst_line0 = (TRGB32*)(dst_argb);
		TRGB32 *dst_line1 = (TRGB32*)(dst_argb + dst_pitch);
		TRGB32 *dst_line2 = (TRGB32*)(dst_argb + dst_pitch * 2);
		TRGB32 *dst_line3 = (TRGB32*)(dst_argb + dst_pitch * 3);

		dst_line0[0] = colormap[colori0 & 0x03];
		dst_line0[0].A = alphamap[alphai0 & 0x07];
		dst_line0[1] = colormap[(colori0 & 0x0c) >> 2];
		dst_line0[1].A = alphamap[(alphai0 & 0x38 >> 3)];
		dst_line0[2] = colormap[(colori0 & 0x30) >> 4];
		dst_line0[2].A = alphamap[((alphai0 & 0xc0) >> 6) | (alphai1 & 0x01)];
		dst_line0[3] = colormap[colori0 >> 6];
		dst_line0[3].A = alphamap[(alphai1 & 0x0e) >> 1];

		dst_line1[0] = colormap[colori1 & 0x03];
		dst_line1[0].A = alphamap[(alphai1 & 0x70) >> 4];
		dst_line1[1] = colormap[(colori1 & 0x0c) >> 2];
		dst_line1[1].A = alphamap[((alphai1 & 0x80) >> 7) | ((alphai2 & 0x03) << 1)];
		dst_line1[2] = colormap[(colori1 & 0x30) >> 4];
		dst_line1[2].A = alphamap[(alphai2 & 0x1c) >> 2];
		dst_line1[3] = colormap[colori1 >> 6];
		dst_line1[3].A = alphamap[(alphai2 & 0xe0) >> 5];

		dst_line2[0] = colormap[colori2 & 0x03];
		dst_line2[0].A = alphamap[alphai3 & 0x07];
		dst_line2[1] = colormap[(colori2 & 0x0c) >> 2];
		dst_line2[1].A = alphamap[(alphai3 & 0x38 >> 3)];
		dst_line2[2] = colormap[(colori2 & 0x30) >> 4];
		dst_line2[2].A = alphamap[((alphai3 & 0xc0) >> 6) | (alphai4 & 0x01)];
		dst_line2[3] = colormap[colori2 >> 6];
		dst_line2[3].A = alphamap[(alphai4 & 0x0e) >> 1];

		dst_line3[0] = colormap[colori3 & 0x03];
		dst_line3[0].A = alphamap[(alphai4 & 0x70) >> 4];
		dst_line3[1] = colormap[(colori3 & 0x0c) >> 2];
		dst_line3[1].A = alphamap[((alphai4 & 0x80) >> 7) | ((alphai5 & 0x03) << 1)];
		dst_line3[2] = colormap[(colori3 & 0x30) >> 4];
		dst_line3[2].A = alphamap[(alphai5 & 0x1c) >> 2];
		dst_line3[3] = colormap[colori3 >> 6];
		dst_line3[3].A = alphamap[(alphai5 & 0xe0) >> 5];

		data += 16;
		dst_argb += 16;
	}
}

void ______P8ToARGBRow_C(const uint8_t* src_p8, uint8_t* dst_argb, int width) {
	TRGB32 *pTexturePalette = *(TRGB32 **)dst_argb; // dirty hack to avoid another argument
	int x;
	for (x = 0; x < width; ++x) {
		uint8_t p = src_p8[x];
		TRGB32 color = pTexturePalette[p];
		((TRGB32 *)dst_argb)[x] = color;
	}
}

static __inline int32_t clamp0(int32_t v) {
	return ((-(v) >> 31) & (v));
}

static __inline int32_t clamp255(int32_t v) {
	return (((255 - (v)) >> 31) | (v)) & 255;
}

static __inline uint32_t Clamp(int32_t val) {
	int v = clamp0(val);
	return (uint32_t)(clamp255(v));
}

#if defined(_MSC_VER) && !defined(__CLR_VER)
#define SIMD_ALIGNED(var) __declspec(align(16)) var
#define SIMD_ALIGNED32(var) __declspec(align(64)) var
typedef __declspec(align(32)) int16_t lvec16[16];
typedef __declspec(align(32)) int8_t lvec8[32];
#elif __GNUC__
#define SIMD_ALIGNED(var) __attribute__((aligned(16)))var
#define SIMD_ALIGNED32(var) __attribute__((aligned(64))) var
typedef __attribute__((aligned(32))) int16_t lvec16[16];
typedef __attribute__((aligned(32))) int8_t lvec8[32];
#endif

struct YuvConstants {
  lvec8 kUVToB;
  lvec8 kUVToG;
  lvec8 kUVToR;
  lvec16 kUVBiasB;
  lvec16 kUVBiasG;
  lvec16 kUVBiasR;
  lvec16 kYToRgb;
};

// BT.601 YUV to RGB reference
//  R = (Y - 16) * 1.164              - V * -1.596
//  G = (Y - 16) * 1.164 - U *  0.391 - V *  0.813
//  B = (Y - 16) * 1.164 - U * -2.018

// Y contribution to R,G,B.  Scale and bias.
#define YG 18997 /* round(1.164 * 64 * 256 * 256 / 257) */
#define YGB -1160 /* 1.164 * 64 * -16 + 64 / 2 */

// U and V contributions to R,G,B.
#define UB -128 /* max(-128, round(-2.018 * 64)) */
#define UG 25 /* round(0.391 * 64) */
#define VG 52 /* round(0.813 * 64) */
#define VR -102 /* round(-1.596 * 64) */

// Bias values to subtract 16 from Y and 128 from U and V.
#define BB (UB * 128            + YGB)
#define BG (UG * 128 + VG * 128 + YGB)
#define BR            (VR * 128 + YGB)

// BT.601 constants for YUV to RGB.
const YuvConstants SIMD_ALIGNED(kYuvIConstants) = {
  { UB, 0, UB, 0, UB, 0, UB, 0, UB, 0, UB, 0, UB, 0, UB, 0,
    UB, 0, UB, 0, UB, 0, UB, 0, UB, 0, UB, 0, UB, 0, UB, 0 },
  { UG, VG, UG, VG, UG, VG, UG, VG, UG, VG, UG, VG, UG, VG, UG, VG,
    UG, VG, UG, VG, UG, VG, UG, VG, UG, VG, UG, VG, UG, VG, UG, VG },
  { 0, VR, 0, VR, 0, VR, 0, VR, 0, VR, 0, VR, 0, VR, 0, VR,
    0, VR, 0, VR, 0, VR, 0, VR, 0, VR, 0, VR, 0, VR, 0, VR },
  { BB, BB, BB, BB, BB, BB, BB, BB, BB, BB, BB, BB, BB, BB, BB, BB },
  { BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG },
  { BR, BR, BR, BR, BR, BR, BR, BR, BR, BR, BR, BR, BR, BR, BR, BR },
  { YG, YG, YG, YG, YG, YG, YG, YG, YG, YG, YG, YG, YG, YG, YG, YG }
};

// C reference code that mimics the YUV assembly.
static __inline void YuvPixel(uint8_t y, uint8_t u, uint8_t v,
	uint8_t* b, uint8_t* g, uint8_t* r,
	const struct YuvConstants* yuvconstants) {
	int ub = yuvconstants->kUVToB[0];
	int ug = yuvconstants->kUVToG[0];
	int vg = yuvconstants->kUVToG[1];
	int vr = yuvconstants->kUVToR[1];
	int bb = yuvconstants->kUVBiasB[0];
	int bg = yuvconstants->kUVBiasG[0];
	int br = yuvconstants->kUVBiasR[0];
	int yg = yuvconstants->kYToRgb[0];

	uint32_t y1 = (uint32_t)(y * 0x0101 * yg) >> 16;
	*b = (uint8_t)Clamp((int32_t)(-(u * ub) + y1 + bb) >> 6);
	*g = (uint8_t)Clamp((int32_t)(-(u * ug + v * vg) + y1 + bg) >> 6);
	*r = (uint8_t)Clamp((int32_t)(-(v * vr) + y1 + br) >> 6);
}

void ____YUY2ToARGBRow_C(const uint8_t* src_yuy2,
	uint8_t* rgb_buf,
	int width) {
	const struct YuvConstants* yuvconstants = &kYuvIConstants; // hack to avoid another argument
	int x;
	for (x = 0; x < width - 1; x += 2) {
		YuvPixel(src_yuy2[0], src_yuy2[1], src_yuy2[3],
			rgb_buf + 0, rgb_buf + 1, rgb_buf + 2, yuvconstants);
		rgb_buf[3] = 255;
		YuvPixel(src_yuy2[2], src_yuy2[1], src_yuy2[3],
			rgb_buf + 4, rgb_buf + 5, rgb_buf + 6, yuvconstants);
		rgb_buf[7] = 255;
		src_yuy2 += 4;
		rgb_buf += 8;  // Advance 2 pixels.
	}
	if (width & 1) {
		YuvPixel(src_yuy2[0], src_yuy2[1], src_yuy2[3],
			rgb_buf + 0, rgb_buf + 1, rgb_buf + 2, yuvconstants);
		rgb_buf[3] = 255;
	}
}

void ____UYVYToARGBRow_C(const uint8_t* src_uyvy,
    uint8_t* rgb_buf,
	int width) {
	const struct YuvConstants* yuvconstants = &kYuvIConstants; // hack to avoid another argument
	int x;
	for (x = 0; x < width - 1; x += 2) {
		YuvPixel(src_uyvy[1], src_uyvy[0], src_uyvy[2],
			rgb_buf + 0, rgb_buf + 1, rgb_buf + 2, yuvconstants);
		rgb_buf[3] = 255;
		YuvPixel(src_uyvy[3], src_uyvy[0], src_uyvy[2],
			rgb_buf + 4, rgb_buf + 5, rgb_buf + 6, yuvconstants);
		rgb_buf[7] = 255;
		src_uyvy += 4;
		rgb_buf += 8;  // Advance 2 pixels.
	}
	if (width & 1) {
		YuvPixel(src_uyvy[1], src_uyvy[0], src_uyvy[2],
			rgb_buf + 0, rgb_buf + 1, rgb_buf + 2, yuvconstants);
		rgb_buf[3] = 255;
	}
}

static const FormatToARGBRow ComponentConverters[] = {
	nullptr, // NoCmpnts,
	ARGB1555ToARGBRow_C, // A1R5G5B5,
	X1R5G5B5ToARGBRow_C, // X1R5G5B5, // Test : Convert X into 255
	ARGB4444ToARGBRow_C, // A4R4G4B4,
	  RGB565ToARGBRow_C, // __R5G6B5, // NOTE : A=255
	A8R8G8B8ToARGBRow_C, // A8R8G8B8,
	X8R8G8B8ToARGBRow_C, // X8R8G8B8, // Test : Convert X into 255
	____R8B8ToARGBRow_C, // ____R8B8, // NOTE : A takes R, G takes B
	____G8B8ToARGBRow_C, // ____G8B8, // NOTE : A takes G, R takes B
	______A8ToARGBRow_C, // ______A8,
	__R6G5B5ToARGBRow_C, // __R6G5B5,
	R5G5B5A1ToARGBRow_C, // R5G5B5A1,
	R4G4B4A4ToARGBRow_C, // R4G4B4A4,
	A8B8G8R8ToARGBRow_C, // A8B8G8R8,
	B8G8R8A8ToARGBRow_C, // B8G8R8A8,
	R8G8B8A8ToARGBRow_C, // R8G8B8A8,
	______L8ToARGBRow_C, // ______L8, // NOTE : A=255, R=G=B= L
	_____AL8ToARGBRow_C, // _____AL8, // NOTE : A=R=G=B= L
	_____L16ToARGBRow_C, // _____L16, // NOTE : Actually G8B8, with A=R=255
	____A8L8ToARGBRow_C, // ____A8L8, // NOTE : R=G=B= L
	____DXT1ToARGBRow_C, // ____DXT1
	____DXT3ToARGBRow_C, // ____DXT3
	____DXT5ToARGBRow_C, // ____DXT5
	______P8ToARGBRow_C, // ______P8
	____YUY2ToARGBRow_C, // ____YUY2
	____UYVYToARGBRow_C, // ____UYVY
};

enum _FormatStorage {
	Undfnd = 0, // Undefined
	Linear,
	Swzzld, // Swizzled
	Cmprsd // Compressed
};

enum _FormatUsage {
	Texture = 0,
	RenderTarget,
	DepthBuffer
};

typedef struct _FormatInfo {
	uint8_t bits_per_pixel;
	_FormatStorage stored;
	_ComponentEncoding components;
	CXBXFORMAT pc;
	_FormatUsage usage;
	const char *warning;
} FormatInfo;

static const FormatInfo FormatInfos[] = {
	// Notes :
	// * This table should use a native format that most closely mirrors the Xbox format,
	// and (for now) MUST use a format that uses the same number of bits/bytes per pixel,
	// as otherwise locked buffers wouldn't be of the same size as on the Xbox, which
	// could lead to out-of-bounds access violations.
	// * Each format for which the host D3D8 doesn't have an identical native format,
	// and does specify color components (other than NoCmpnts), MUST specify this fact
	// in the warning field, so EmuXBFormatRequiresConversionToARGB can return a conversion
	// to ARGB is needed (which is implemented in EMUPATCH(D3DResource_Register).

	// D3D9 D3FFORMAT to D3D11 DXGI_FORMAT mapping : https://docs.microsoft.com/en-us/windows/win32/direct3d10/d3d10-graphics-programming-guide-resources-legacy-formats
	#define	DXGI_FORMAT_NOT_AVAILABLE DXGI_FORMAT_UNKNOWN // TODO : Replace below occurences with a suitable format and corresponding conversion

	/* 0x00 X_D3DFMT_L8           */ {  8, Swzzld, ______L8, _9_11(D3DFMT_L8,       DXGI_FORMAT_R8_UNORM)      },
	/* 0x01 X_D3DFMT_AL8          */ {  8, Swzzld, _____AL8, _9_11(D3DFMT_L8,       DXGI_FORMAT_R8_UNORM)      , Texture, "X_D3DFMT_AL8 -> D3DFMT_L8" },
	/* 0x02 X_D3DFMT_A1R5G5B5     */ { 16, Swzzld, A1R5G5B5, _9_11(D3DFMT_A1R5G5B5, DXGI_FORMAT_B5G5R5A1_UNORM)},
	/* 0x03 X_D3DFMT_X1R5G5B5     */ { 16, Swzzld, X1R5G5B5, _9_11(D3DFMT_X1R5G5B5, DXGI_FORMAT_NOT_AVAILABLE) , RenderTarget },
	/* 0x04 X_D3DFMT_A4R4G4B4     */ { 16, Swzzld, A4R4G4B4, _9_11(D3DFMT_A4R4G4B4, DXGI_FORMAT_B4G4R4A4_UNORM)},
	/* 0x05 X_D3DFMT_R5G6B5       */ { 16, Swzzld, __R5G6B5, _9_11(D3DFMT_R5G6B5,   DXGI_FORMAT_B5G6R5_UNORM)  , RenderTarget },
	/* 0x06 X_D3DFMT_A8R8G8B8     */ { 32, Swzzld, A8R8G8B8, _9_11(D3DFMT_A8R8G8B8, DXGI_FORMAT_B8G8R8A8_UNORM), RenderTarget },
	/* 0x07 X_D3DFMT_X8R8G8B8     */ { 32, Swzzld, X8R8G8B8, _9_11(D3DFMT_X8R8G8B8, DXGI_FORMAT_B8G8R8X8_UNORM), RenderTarget }, // Alias : X_D3DFMT_X8L8V8U8 
	/* 0x08 undefined             */ {},
	/* 0x09 undefined             */ {},
	/* 0x0A undefined             */ {},
	/* 0x0B X_D3DFMT_P8           */ {  8, Swzzld, ______P8, _9_11(D3DFMT_P8,       DXGI_FORMAT_R8_UINT)       , Texture, "X_D3DFMT_P8 -> D3DFMT_L8" }, // 8-bit palletized
	/* 0x0C X_D3DFMT_DXT1         */ {  4, Cmprsd, ____DXT1, _9_11(D3DFMT_DXT1,     DXGI_FORMAT_BC1_UNORM)     }, // opaque/one-bit alpha // NOTE : DXT1 is half byte per pixel, so divide Size and Pitch calculations by two!
	/* 0x0D undefined             */ {},
	/* 0x0E X_D3DFMT_DXT3         */ {  8, Cmprsd, ____DXT3, _9_11(D3DFMT_DXT3,     DXGI_FORMAT_BC2_UNORM)     }, // Alias : X_D3DFMT_DXT2 // linear alpha
	/* 0x0F X_D3DFMT_DXT5         */ {  8, Cmprsd, ____DXT5, _9_11(D3DFMT_DXT5,     DXGI_FORMAT_BC3_UNORM)     }, // Alias : X_D3DFMT_DXT4 // interpolated alpha
	/* 0x10 X_D3DFMT_LIN_A1R5G5B5 */ { 16, Linear, A1R5G5B5, _9_11(D3DFMT_A1R5G5B5, DXGI_FORMAT_B5G5R5A1_UNORM)},
	/* 0x11 X_D3DFMT_LIN_R5G6B5   */ { 16, Linear, __R5G6B5, _9_11(D3DFMT_R5G6B5,   DXGI_FORMAT_B5G6R5_UNORM)  , RenderTarget },
	/* 0x12 X_D3DFMT_LIN_A8R8G8B8 */ { 32, Linear, A8R8G8B8, _9_11(D3DFMT_A8R8G8B8, DXGI_FORMAT_B8G8R8A8_UNORM), RenderTarget },
	/* 0x13 X_D3DFMT_LIN_L8       */ {  8, Linear, ______L8, _9_11(D3DFMT_L8,       DXGI_FORMAT_R8_UNORM)      , RenderTarget },
	/* 0x14 undefined             */ {},
	/* 0x15 undefined             */ {},
	/* 0x16 X_D3DFMT_LIN_R8B8     */ { 16, Linear, ____R8B8, _9_11(D3DFMT_R5G6B5,   DXGI_FORMAT_B5G6R5_UNORM)  , Texture, "X_D3DFMT_LIN_R8B8 -> D3DFMT_R5G6B5" },
	/* 0x17 X_D3DFMT_LIN_G8B8     */ { 16, Linear, ____G8B8, _9_11(D3DFMT_R5G6B5,   DXGI_FORMAT_B5G6R5_UNORM)  , RenderTarget, "X_D3DFMT_LIN_G8B8 -> D3DFMT_R5G6B5" }, // Alias : X_D3DFMT_LIN_V8U8
	/* 0x18 undefined             */ {},
	/* 0x19 X_D3DFMT_A8           */ {  8, Swzzld, ______A8, _9_11(D3DFMT_A8,       DXGI_FORMAT_A8_UNORM)      , Texture, "X_D3DFMT_A8 -> D3DFMT_A8R8G8B8" },  // D3D9 sets RGB = 0 instead of 1
	/* 0x1A X_D3DFMT_A8L8         */ { 16, Swzzld, ____A8L8, _9_11(D3DFMT_A8L8,     DXGI_FORMAT_NOT_AVAILABLE) },
	/* 0x1B X_D3DFMT_LIN_AL8      */ {  8, Linear, _____AL8, _9_11(D3DFMT_L8,       DXGI_FORMAT_R8_UNORM)      , Texture, "X_D3DFMT_LIN_AL8 -> D3DFMT_L8" },
	/* 0x1C X_D3DFMT_LIN_X1R5G5B5 */ { 16, Linear, X1R5G5B5, _9_11(D3DFMT_X1R5G5B5, DXGI_FORMAT_NOT_AVAILABLE) , RenderTarget },
	/* 0x1D X_D3DFMT_LIN_A4R4G4B4 */ { 16, Linear, A4R4G4B4, _9_11(D3DFMT_A4R4G4B4, DXGI_FORMAT_B4G4R4A4_UNORM)},
	/* 0x1E X_D3DFMT_LIN_X8R8G8B8 */ { 32, Linear, X8R8G8B8, _9_11(D3DFMT_X8R8G8B8, DXGI_FORMAT_B8G8R8X8_UNORM), RenderTarget }, // Alias : X_D3DFMT_LIN_X8L8V8U8
	/* 0x1F X_D3DFMT_LIN_A8       */ {  8, Linear, ______A8, _9_11(D3DFMT_A8,       DXGI_FORMAT_A8_UNORM)      , Texture, "X_D3DFMT_LIN_A8 -> D3DFMT_A8R8G8B8" }, // D3D9 sets RGB = 0 instead of 1
	/* 0x20 X_D3DFMT_LIN_A8L8     */ { 16, Linear, ____A8L8, _9_11(D3DFMT_A8L8,     DXGI_FORMAT_NOT_AVAILABLE) },
	/* 0x21 undefined             */ {},
	/* 0x22 undefined             */ {},
	/* 0x23 undefined             */ {},
	/* 0x24 X_D3DFMT_YUY2         */ { 16, Linear, ____YUY2, _9_11(D3DFMT_YUY2,     DXGI_FORMAT_YUY2)          }, // DXGI_FORMAT_NOT_AVAILABLE ?
	/* 0x25 X_D3DFMT_UYVY         */ { 16, Linear, ____UYVY, _9_11(D3DFMT_UYVY,     DXGI_FORMAT_NOT_AVAILABLE) },
	/* 0x26 undefined             */ {},
	/* 0x27 X_D3DFMT_L6V5U5       */ { 16, Swzzld, __R6G5B5, _9_11(D3DFMT_L6V5U5,   DXGI_FORMAT_NOT_AVAILABLE) }, // Alias : X_D3DFMT_R6G5B5 // XQEMU NOTE : This might be signed
	/* 0x28 X_D3DFMT_V8U8         */ { 16, Swzzld, ____G8B8, _9_11(D3DFMT_V8U8,     DXGI_FORMAT_R8G8_SNORM)    }, // Alias : X_D3DFMT_G8B8 // XQEMU NOTE : This might be signed
	/* 0x29 X_D3DFMT_R8B8         */ { 16, Swzzld, ____R8B8, _9_11(D3DFMT_R5G6B5,   DXGI_FORMAT_B5G6R5_UNORM)  , Texture, "X_D3DFMT_R8B8 -> D3DFMT_R5G6B5" }, // XQEMU NOTE : This might be signed
	/* 0x2A X_D3DFMT_D24S8        */ { 32, Swzzld, NoCmpnts, _9_11(D3DFMT_D24S8,    DXGI_FORMAT_NOT_AVAILABLE) , DepthBuffer },
	/* 0x2B X_D3DFMT_F24S8        */ { 32, Swzzld, NoCmpnts, _9_11(D3DFMT_D24FS8,   DXGI_FORMAT_NOT_AVAILABLE) , DepthBuffer },
	/* 0x2C X_D3DFMT_D16          */ { 16, Swzzld, NoCmpnts, _9_11(D3DFMT_D16,      DXGI_FORMAT_D16_UNORM)     , DepthBuffer }, // Note : X_D3DFMT_D16 is always lockable on Xbox, D3DFMT_D16 on host is not, but D3DFMT_D16_LOCKABLE often fails SetRenderTarget.
	/* 0x2D X_D3DFMT_F16          */ { 16, Swzzld, NoCmpnts, _9_11(D3DFMT_D16,      DXGI_FORMAT_D16_UNORM)     , DepthBuffer, "X_D3DFMT_F16 -> D3DFMT_D16" }, // HACK : PC doesn't have D3DFMT_F16 (Float vs Int) // TODO : Use D3DFMT_R16F?
	/* 0x2E X_D3DFMT_LIN_D24S8    */ { 32, Linear, NoCmpnts, _9_11(D3DFMT_D24S8,    DXGI_FORMAT_NOT_AVAILABLE) , DepthBuffer },
	/* 0x2F X_D3DFMT_LIN_F24S8    */ { 32, Linear, NoCmpnts, _9_11(D3DFMT_D24FS8,   DXGI_FORMAT_NOT_AVAILABLE) , DepthBuffer },
	/* 0x30 X_D3DFMT_LIN_D16      */ { 16, Linear, NoCmpnts, _9_11(D3DFMT_D16,      DXGI_FORMAT_D16_UNORM)     , DepthBuffer }, // Note : X_D3DFMT_D16 is always lockable on Xbox, D3DFMT_D16 on host is not, but D3DFMT_D16_LOCKABLE often fails SetRenderTarget.
	/* 0x31 X_D3DFMT_LIN_F16      */ { 16, Linear, NoCmpnts, _9_11(D3DFMT_D16,      DXGI_FORMAT_D16_UNORM)     , DepthBuffer, "X_D3DFMT_LIN_F16 -> D3DFMT_D16" }, // HACK : PC doesn't have D3DFMT_F16 (Float vs Int) // TODO : Use D3DFMT_R16F?
	/* 0x32 X_D3DFMT_L16          */ { 16, Swzzld, _____L16, _9_11(D3DFMT_L16,      DXGI_FORMAT_R16_UNORM)     },
	/* 0x33 X_D3DFMT_V16U16       */ { 32, Swzzld, NoCmpnts, _9_11(D3DFMT_V16U16,   DXGI_FORMAT_R16G16_SNORM)  },
	/* 0x34 undefined             */ {},
	/* 0x35 X_D3DFMT_LIN_L16      */ { 16, Linear, _____L16, _9_11(D3DFMT_L16,      DXGI_FORMAT_R16_UNORM)     },
	/* 0x36 X_D3DFMT_LIN_V16U16   */ { 32, Linear, NoCmpnts, _9_11(D3DFMT_V16U16,   DXGI_FORMAT_R16G16_SNORM)  }, // Note : Seems unused on Xbox
	/* 0x37 X_D3DFMT_LIN_L6V5U5   */ { 16, Linear, __R6G5B5, _9_11(D3DFMT_L6V5U5,   DXGI_FORMAT_NOT_AVAILABLE) }, // Alias : X_D3DFMT_LIN_R6G5B5
	/* 0x38 X_D3DFMT_R5G5B5A1     */ { 16, Swzzld, R5G5B5A1, _9_11(D3DFMT_A1R5G5B5, DXGI_FORMAT_B5G5R5A1_UNORM), Texture, "X_D3DFMT_R5G5B5A1 -> D3DFMT_A1R5G5B5" },
	/* 0x39 X_D3DFMT_R4G4B4A4     */ { 16, Swzzld, R4G4B4A4, _9_11(D3DFMT_A4R4G4B4, DXGI_FORMAT_B4G4R4A4_UNORM), Texture, "X_D3DFMT_R4G4B4A4 -> D3DFMT_A4R4G4B4" },
	/* 0x3A X_D3DFMT_Q8W8V8U8     */ { 32, Swzzld, A8B8G8R8, _9_11(D3DFMT_Q8W8V8U8, DXGI_FORMAT_R8G8B8A8_SNORM)}, // Alias : X_D3DFMT_A8B8G8R8 // Note : D3DFMT_A8B8G8R8=32 D3DFMT_Q8W8V8U8=63 // TODO : Needs testcase.
	/* 0x3B X_D3DFMT_B8G8R8A8     */ { 32, Swzzld, B8G8R8A8, _9_11(D3DFMT_A8R8G8B8, DXGI_FORMAT_B8G8R8A8_UNORM), Texture, "X_D3DFMT_B8G8R8A8 -> D3DFMT_A8R8G8B8" },
	/* 0x3C X_D3DFMT_R8G8B8A8     */ { 32, Swzzld, R8G8B8A8, _9_11(D3DFMT_A8R8G8B8, DXGI_FORMAT_B8G8R8A8_UNORM), Texture, "X_D3DFMT_R8G8B8A8 -> D3DFMT_A8R8G8B8" },
	/* 0x3D X_D3DFMT_LIN_R5G5B5A1 */ { 16, Linear, R5G5B5A1, _9_11(D3DFMT_A1R5G5B5, DXGI_FORMAT_B5G5R5A1_UNORM), Texture, "X_D3DFMT_LIN_R5G5B5A1 -> D3DFMT_A1R5G5B5" },
	/* 0x3E X_D3DFMT_LIN_R4G4B4A4 */ { 16, Linear, R4G4B4A4, _9_11(D3DFMT_A4R4G4B4, DXGI_FORMAT_B4G4R4A4_UNORM), Texture, "X_D3DFMT_LIN_R4G4B4A4 -> D3DFMT_A4R4G4B4" },
	/* 0x3F X_D3DFMT_LIN_A8B8G8R8 */ { 32, Linear, A8B8G8R8, _9_11(D3DFMT_A8B8G8R8, DXGI_FORMAT_R8G8B8A8_UNORM)}, // Note : D3DFMT_A8B8G8R8=32 D3DFMT_Q8W8V8U8=63 // TODO : Needs testcase.
	/* 0x40 X_D3DFMT_LIN_B8G8R8A8 */ { 32, Linear, B8G8R8A8, _9_11(D3DFMT_A8R8G8B8, DXGI_FORMAT_B8G8R8A8_UNORM), Texture, "X_D3DFMT_LIN_B8G8R8A8 -> D3DFMT_A8R8G8B8" },
	/* 0x41 X_D3DFMT_LIN_R8G8B8A8 */ { 32, Linear, R8G8B8A8, _9_11(D3DFMT_A8R8G8B8, DXGI_FORMAT_B8G8R8A8_UNORM), Texture, "X_D3DFMT_LIN_R8G8B8A8 -> D3DFMT_A8R8G8B8" },
#if 0
	/* 0x42 to 0x63 undefined     */ {},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},
	/* 0x64 X_D3DFMT_VERTEXDATA   */ {  8, Linear, NoCmpnts, D3DFMT_VERTEXDATA },
	/* 0x65 X_D3DFMT_INDEX16      */ { 16, Linear, NoCmpnts, D3DFMT_INDEX16    }, // Dxbx addition : X_D3DFMT_INDEX16 is not an Xbox format, but used internally
#endif
};

const FormatToARGBRow EmuXBFormatComponentConverter(xbox::X_D3DFORMAT Format)
{
	if (Format <= xbox::X_D3DFMT_LIN_R8G8B8A8)
		if (FormatInfos[Format].components != NoCmpnts)
			return ComponentConverters[FormatInfos[Format].components];

	return nullptr;
}

// Is there a converter available from the supplied format to ARGB?
bool EmuXBFormatCanBeConvertedToARGB(xbox::X_D3DFORMAT Format)
{
	const FormatToARGBRow info = EmuXBFormatComponentConverter(Format);
	return (info != nullptr);
}

// Returns if convertion to ARGB is required. This is the case when
// the format has a warning message and there's a converter present.
bool EmuXBFormatRequiresConversionToARGB(xbox::X_D3DFORMAT Format)
{
	if (FormatInfos[Format].warning != nullptr)
		if (EmuXBFormatCanBeConvertedToARGB(Format))
			return true;

	return false;
}

DWORD EmuXBFormatBitsPerPixel(xbox::X_D3DFORMAT Format)
{
	if (Format <= xbox::X_D3DFMT_LIN_R8G8B8A8)
		if (FormatInfos[Format].bits_per_pixel > 0) // TODO : Remove this
			return FormatInfos[Format].bits_per_pixel;

	return 16; // TODO : 8
}

DWORD EmuXBFormatBytesPerPixel(xbox::X_D3DFORMAT Format)
{
	return ((EmuXBFormatBitsPerPixel(Format) + 4) / 8);
}

BOOL EmuXBFormatIsCompressed(xbox::X_D3DFORMAT Format)
{
	if (Format <= xbox::X_D3DFMT_LIN_R8G8B8A8)
		return (FormatInfos[Format].stored == Cmprsd);

	return false;
}

BOOL EmuXBFormatIsLinear(xbox::X_D3DFORMAT Format)
{
	if (Format <= xbox::X_D3DFMT_LIN_R8G8B8A8)
		return (FormatInfos[Format].stored == Linear);

	return (Format == xbox::X_D3DFMT_VERTEXDATA); // TODO : false;
}

BOOL EmuXBFormatIsSwizzled(xbox::X_D3DFORMAT Format)
{
	if (Format <= xbox::X_D3DFMT_LIN_R8G8B8A8)
		return (FormatInfos[Format].stored == Swzzld);

	return false;
}

BOOL EmuXBFormatIsRenderTarget(xbox::X_D3DFORMAT Format)
{
	if (Format <= xbox::X_D3DFMT_LIN_R8G8B8A8)
		return (FormatInfos[Format].usage == RenderTarget);

	return false;
}

BOOL EmuXBFormatIsDepthBuffer(xbox::X_D3DFORMAT Format)
{
	if (Format <= xbox::X_D3DFMT_LIN_R8G8B8A8)
		return (FormatInfos[Format].usage == DepthBuffer);

	return false;
}

CXBXFORMAT EmuXB2PC_D3DFormat(xbox::X_D3DFORMAT Format)
{
	if (Format <= xbox::X_D3DFMT_LIN_R8G8B8A8 && Format >= 0) // The last bit prevents crashing (Metal Slug 3)
	{
		const FormatInfo *info = &FormatInfos[Format];
		if (info->warning != nullptr) {
			EmuLog(LOG_LEVEL::DEBUG, "EmuXB2PC_D3DFormat %s", info->warning);
		}

		return info->pc;
	}

	switch (Format) {
	case xbox::X_D3DFMT_VERTEXDATA:
#ifdef CXBX_USE_D3D11
		[[fallthrough]]; // This case is unlikely to ever get passed in here anyway
#else
		return D3DFMT_VERTEXDATA;
#endif
	case xbox::X_D3DFMT_UNKNOWN: [[fallthrough]]; // Test-case : Metal Slug 3?
	case ((xbox::X_D3DFORMAT)0xffffffff):
		return _9_11(D3DFMT_UNKNOWN, DXGI_FORMAT_UNKNOWN); // TODO -oCXBX: Not sure if this counts as swizzled or not...
	default:
		CxbxrAbort("EmuXB2PC_D3DFormat: Unknown Format (0x%.08X)", Format);
	}

	return _9_11(D3DFMT_UNKNOWN, DXGI_FORMAT_UNKNOWN);
}

xbox::X_D3DFORMAT EmuPC2XB_D3DFormat(D3DFORMAT Format, bool bPreferLinear)
{
	xbox::X_D3DFORMAT result;
	switch(Format)
	{
	case D3DFMT_YUY2:
		result = xbox::X_D3DFMT_YUY2;
		break;
	case D3DFMT_UYVY:
		result = xbox::X_D3DFMT_UYVY;
		break;
	case D3DFMT_R5G6B5:
		result = bPreferLinear ? xbox::X_D3DFMT_LIN_R5G6B5 : xbox::X_D3DFMT_R5G6B5;
		break;
	case D3DFMT_D24S8:
		result = bPreferLinear ? xbox::X_D3DFMT_LIN_D24S8 : xbox::X_D3DFMT_D24S8;
		break;
	case D3DFMT_DXT5:
		result = xbox::X_D3DFMT_DXT5; // Compressed
		break;
	case D3DFMT_DXT4:
		result = xbox::X_D3DFMT_DXT4;  // Compressed // Same as xbox::X_D3DFMT_DXT5
		break;
	case D3DFMT_DXT3:
		result = xbox::X_D3DFMT_DXT3; // Compressed
		break;
	case D3DFMT_DXT2:
		result = xbox::X_D3DFMT_DXT2; // Compressed // Same as xbox::X_D3DFMT_DXT3
		break;
	case D3DFMT_DXT1:
		result = xbox::X_D3DFMT_DXT1; // Compressed
		break;
	case D3DFMT_A1R5G5B5:
		result = bPreferLinear ? xbox::X_D3DFMT_LIN_A1R5G5B5 : xbox::X_D3DFMT_A1R5G5B5;
		break;
	case D3DFMT_X8R8G8B8:
		result = bPreferLinear ? xbox::X_D3DFMT_LIN_X8R8G8B8 : xbox::X_D3DFMT_X8R8G8B8;
		break;
	case D3DFMT_A8R8G8B8:
		result = bPreferLinear ? xbox::X_D3DFMT_LIN_A8R8G8B8 : xbox::X_D3DFMT_A8R8G8B8;
		break;
	case D3DFMT_A4R4G4B4:
		result = bPreferLinear ? xbox::X_D3DFMT_LIN_A4R4G4B4 : xbox::X_D3DFMT_A4R4G4B4;
		break;
	case D3DFMT_X1R5G5B5:
		result = bPreferLinear ? xbox::X_D3DFMT_LIN_X1R5G5B5 : xbox::X_D3DFMT_X1R5G5B5;
		break;
	case D3DFMT_A8:
		result = bPreferLinear ? xbox::X_D3DFMT_LIN_A8 : xbox::X_D3DFMT_A8;
		break;
	case D3DFMT_L8:
		result = bPreferLinear ? xbox::X_D3DFMT_LIN_L8 : xbox::X_D3DFMT_L8;
		break;
	case D3DFMT_D16:
		result = bPreferLinear ? xbox::X_D3DFMT_LIN_D16 : xbox::X_D3DFMT_D16;
		break;
	case D3DFMT_D16_LOCKABLE:
		result = xbox::X_D3DFMT_D16_LOCKABLE;
		break; 
	case D3DFMT_UNKNOWN:
		result = ((xbox::X_D3DFORMAT)0xffffffff); // TODO : return xbox::X_D3DFMT_UNKNOWN ?
		break;
	// Dxbx additions :
	case D3DFMT_L6V5U5:
		result = bPreferLinear ? xbox::X_D3DFMT_LIN_L6V5U5 : xbox::X_D3DFMT_L6V5U5;
		break;
	case D3DFMT_V8U8:
		result = bPreferLinear ? xbox::X_D3DFMT_LIN_V8U8 : xbox::X_D3DFMT_V8U8;
		break;
	case D3DFMT_V16U16:
		result = bPreferLinear ? xbox::X_D3DFMT_LIN_V16U16 : xbox::X_D3DFMT_V16U16;
		break;
	case D3DFMT_VERTEXDATA:
		result = xbox::X_D3DFMT_VERTEXDATA;
		break;
	default:
		CxbxrAbort("EmuPC2XB_D3DFormat: Unknown Format (%d)", Format);
	}

	return result;
}

DWORD EmuXB2PC_D3DLock(DWORD Flags)
{
    DWORD NewFlags = 0;

    // Need to convert the flags, TODO: fix the xbox extensions
//    if(Flags & X_D3DLOCK_NOFLUSH)
//        NewFlags ^= 0;

	if(Flags & X_D3DLOCK_NOOVERWRITE)
        NewFlags |= D3DLOCK_NOOVERWRITE;

//	if(Flags & X_D3DLOCK_TILED)
//        NewFlags ^= 0;

	if(Flags & X_D3DLOCK_READONLY)
        NewFlags |= D3DLOCK_READONLY;

    return NewFlags;
}

// convert from xbox to pc multisample formats
D3DMULTISAMPLE_TYPE EmuXB2PC_D3DMultiSampleFormat(DWORD Type)
{
	D3DMULTISAMPLE_TYPE result;
	switch (Type & 0xFFFF)
	{
	case xbox::X_D3DMULTISAMPLE_NONE:
		result = D3DMULTISAMPLE_NONE;
		break;
	case xbox::X_D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR: 
	case xbox::X_D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX: 
	case xbox::X_D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR: 
	case xbox::X_D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR:
		result = D3DMULTISAMPLE_2_SAMPLES;
		break;
	case xbox::X_D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR: 
	case xbox::X_D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN: 
	case xbox::X_D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR: 
	case xbox::X_D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN:
		result = D3DMULTISAMPLE_4_SAMPLES;
		break;
	case xbox::X_D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN: 
	case xbox::X_D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN:
		result = D3DMULTISAMPLE_9_SAMPLES;
		break;
	default:
		EmuLog(LOG_LEVEL::WARNING, "Unknown Multisample Type (0x%X)!\x0d\x0a.", Type);
		result = D3DMULTISAMPLE_NONE;
	}
	return result;
}

// lookup table for converting vertex count to primitive count
const unsigned g_XboxPrimitiveTypeInfo[11][2] =
{
	// First number is the starting number of vertices the draw requires
	// Second number the number of vertices per primitive
	// Example : Triangle list, has no starting vertices, and uses 3 vertices for each triangle
	// Example : Triangle strip, starts with 2 vertices, and adds 1 for each triangle
    {0, 0}, // NULL
	{0, 1}, // X_D3DPT_POINTLIST
	{0, 2}, // X_D3DPT_LINELIST
	{1, 1}, // X_D3DPT_LINELOOP
	{1, 1}, // X_D3DPT_LINESTRIP
	{0, 3}, // X_D3DPT_TRIANGLELIST
	{2, 1}, // X_D3DPT_TRIANGLESTRIP
	{2, 1}, // X_D3DPT_TRIANGLEFAN
	{0, 4}, // X_D3DPT_QUADLIST
	{2, 2}, // X_D3DPT_QUADSTRIP
	{0, 1}, // X_D3DPT_POLYGON
};

// conversion table for xbox->pc primitive types
const D3DPRIMITIVETYPE g_XboxPrimitiveTypeToHost[] =
{
    /* NULL                   = 0         */ (D3DPRIMITIVETYPE)0,
    /* X_D3DPT_POINTLIST      = 1,        */ D3DPT_POINTLIST,
    /* X_D3DPT_LINELIST       = 2,        */ D3DPT_LINELIST,
    /* X_D3DPT_LINELOOP       = 3,  Xbox  */ D3DPT_LINESTRIP,
    /* X_D3DPT_LINESTRIP      = 4,        */ D3DPT_LINESTRIP,
    /* X_D3DPT_TRIANGLELIST   = 5,        */ D3DPT_TRIANGLELIST,
    /* X_D3DPT_TRIANGLESTRIP  = 6,        */ D3DPT_TRIANGLESTRIP,
    /* X_D3DPT_TRIANGLEFAN    = 7,        */ D3DPT_TRIANGLEFAN,
    /* X_D3DPT_QUADLIST       = 8,  Xbox  */ D3DPT_TRIANGLELIST,
    /* X_D3DPT_QUADSTRIP      = 9,  Xbox  */ D3DPT_TRIANGLESTRIP,
    /* X_D3DPT_POLYGON        = 10, Xbox  */ D3DPT_TRIANGLEFAN,
    /* X_D3DPT_MAX            = 11,       */ (D3DPRIMITIVETYPE)11
};

void EmuUnswizzleBox
(
	CONST PVOID pSrcBuff,
	CONST DWORD dwWidth,
	CONST DWORD dwHeight,
	CONST DWORD dwDepth,
	CONST DWORD dwBytesPerPixel,
	CONST PVOID pDstBuff,
	CONST DWORD dwDstRowPitch,
	CONST DWORD dwDstSlicePitch
) // Source : Dxbx
{
	DWORD dwMaskX = 0, dwMaskY = 0, dwMaskZ = 0;
	for (unsigned int i=1, j=1; (i < dwWidth) || (i < dwHeight) || (i < dwDepth); i <<= 1) {
		if (i < dwWidth) {
			dwMaskX |= j;
			j <<= 1;
		}

		if (i < dwHeight) {
			dwMaskY |= j;
			j <<= 1;
		}

		if (i < dwDepth) {
			dwMaskZ |= j;
			j <<= 1;
		}
	}

	const DWORD dwStartX = 0;
	const DWORD dwStartY = 0;
	const DWORD dwStartZ = 0;

	DWORD dwZ = dwStartZ;
	switch (dwBytesPerPixel) {
	case 1: {
		const uint8_t *pSrc = (uint8_t *)pSrcBuff;
		uint8_t *pDestSlice = (uint8_t *)pDstBuff;

		for (unsigned int z = 0; z < dwDepth; z++) {
			uint8_t *pDestRow = pDestSlice;
			DWORD dwY = dwStartY;
			for (unsigned int y = 0; y < dwHeight; y++) {
				DWORD dwYZ = dwY | dwZ;
				DWORD dwX = dwStartX;
				for (unsigned int x = 0; x < dwWidth; x++) {
					unsigned int delta = dwX | dwYZ;
					pDestRow[x] = pSrc[delta]; // copy one pixel
					dwX = (dwX - dwMaskX) & dwMaskX; // step to next pixel in source
				}

				pDestRow += dwDstRowPitch; // / 1; // = / dwBPP; // step to next line in destination
				dwY = (dwY - dwMaskY) & dwMaskY; // step to next line in source
			}

			pDestSlice += dwDstSlicePitch; // / 1; // = / dwBPP; // step to next level in destination
			dwZ = (dwZ - dwMaskZ) & dwMaskZ; // step to next level in source
		}
		break;
	}
	case 2: {
		const uint16_t *pSrc = (uint16_t *)pSrcBuff;
		uint16_t *pDestSlice = (uint16_t *)pDstBuff;

		for (unsigned int z = 0; z < dwDepth; z++) {
			uint16_t *pDestRow = pDestSlice;
			DWORD dwY = dwStartY;
			for (unsigned int y = 0; y < dwHeight; y++) {
				DWORD dwYZ = dwY | dwZ;
				DWORD dwX = dwStartX;
				for (unsigned int x = 0; x < dwWidth; x++) {
					unsigned int delta = dwX | dwYZ;
					pDestRow[x] = pSrc[delta]; // copy one pixel
					dwX = (dwX - dwMaskX) & dwMaskX; // step to next pixel in source
				}

				pDestRow += dwDstRowPitch / 2; // = dwBPP; // step to next line in destination
				dwY = (dwY - dwMaskY) & dwMaskY; // step to next line in source
			}

			pDestSlice += dwDstSlicePitch / 2; // = dwBPP; // step to next level in destination
			dwZ = (dwZ - dwMaskZ) & dwMaskZ; // step to next level in source
		}
		break;
	}
	case 4: {
		const uint32_t *pSrc = (uint32_t *)pSrcBuff;
		uint32_t *pDestSlice = (uint32_t *)pDstBuff;

		for (unsigned int z = 0; z < dwDepth; z++) {
			uint32_t *pDestRow = pDestSlice;
			DWORD dwY = dwStartY;
			for (unsigned int y = 0; y < dwHeight; y++) {
				DWORD dwYZ = dwY | dwZ;
				DWORD dwX = dwStartX;
				for (unsigned int x = 0; x < dwWidth; x++) {
					unsigned int delta = dwX | dwYZ;
					pDestRow[x] = pSrc[delta]; // copy one pixel
					dwX = (dwX - dwMaskX) & dwMaskX; // step to next pixel in source
				}

				pDestRow += dwDstRowPitch / 4; // = dwBPP; // step to next line in destination
				dwY = (dwY - dwMaskY) & dwMaskY; // step to next line in source
			}

			pDestSlice += dwDstSlicePitch / 4; // = dwBPP; // step to next level in destination
			dwZ = (dwZ - dwMaskZ) & dwMaskZ; // step to next level in source
		}
		break;
	}
	}
} // EmuUnswizzleBox NOPATCH

// Notes :
// * Most renderstates were introduced in the (lowest known) XDK version : 3424
// * Some titles use XDK version 3911
// * The lowest XDK version that has been verified is : 3944
// * All renderstates marked 3424 are also verified to be present in 3944
// * Twenty-three additional renderstates were introduced after 3944 and up to 4627;
// *   D3DRS_DEPTHCLIPCONTROL, D3DRS_STIPPLEENABLE, D3DRS_SIMPLE_UNUSED8..D3DRS_SIMPLE_UNUSED1,
// *   D3DRS_SWAPFILTER, D3DRS_PRESENTATIONINTERVAL, D3DRS_DEFERRED_UNUSED8..D3DRS_DEFERRED_UNUSED1,
// *   D3DRS_MULTISAMPLEMODE, D3DRS_MULTISAMPLERENDERTARGETMODE, and D3DRS_SAMPLEALPHA
// * One renderstate, D3DRS_MULTISAMPLETYPE, was removed (after 3944, before 4039, perhaps even 4034)
// * Around when D3DRS_MULTISAMPLETYPE was removed, D3DRS_MULTISAMPLEMODE was introduced (after 3944, before or at 4039, perhaps even 4034)
// * We MUST list exact versions for all above mentioned renderstates, since their inserts impacts mapping!
// * Renderstates verified to be introduced at 4039 or earlier, may have been introduced at 4034 or earlier
// * Renderstates were finalized in 4627 (so no change after that version)
// * XDK versions that have been verified : 3944, 4039, 4134, 4242, 4361, 4432, 4531, 4627, 4721, 4831, 4928, 5028, 5120, 5233, 5344, 5455, 5558, 5659, 5788, 5849, 5933
// * Renderstates with uncertain validity are marked "Verified absent in #XDK#" and/or "present in #XDK#". Some have "Might be introduced "... "in between" or "around #XDK#"
// * Renderstates after D3DRS_MULTISAMPLEMASK have no host DX9 D3DRS mapping, thus no impact
const RenderStateInfo DxbxRenderStateInfo[1+xbox::X_D3DRS_DONOTCULLUNCOMPRESSED] = {

	// String                                 Ord  Version Type                   Method              Native
	{ "D3DRS_PSALPHAINPUTS0"              /*=   0*/, 3424, xtDWORD,               NV2A_RC_IN_ALPHA(0) },
	{ "D3DRS_PSALPHAINPUTS1"              /*=   1*/, 3424, xtDWORD,               NV2A_RC_IN_ALPHA(1) },
	{ "D3DRS_PSALPHAINPUTS2"              /*=   2*/, 3424, xtDWORD,               NV2A_RC_IN_ALPHA(2) },
	{ "D3DRS_PSALPHAINPUTS3"              /*=   3*/, 3424, xtDWORD,               NV2A_RC_IN_ALPHA(3) },
	{ "D3DRS_PSALPHAINPUTS4"              /*=   4*/, 3424, xtDWORD,               NV2A_RC_IN_ALPHA(4) },
	{ "D3DRS_PSALPHAINPUTS5"              /*=   5*/, 3424, xtDWORD,               NV2A_RC_IN_ALPHA(5) },
	{ "D3DRS_PSALPHAINPUTS6"              /*=   6*/, 3424, xtDWORD,               NV2A_RC_IN_ALPHA(6) },
	{ "D3DRS_PSALPHAINPUTS7"              /*=   7*/, 3424, xtDWORD,               NV2A_RC_IN_ALPHA(7) },
	{ "D3DRS_PSFINALCOMBINERINPUTSABCD"   /*=   8*/, 3424, xtDWORD,               NV2A_RC_FINAL0 },
	{ "D3DRS_PSFINALCOMBINERINPUTSEFG"    /*=   9*/, 3424, xtDWORD,               NV2A_RC_FINAL1 },
	{ "D3DRS_PSCONSTANT0_0"               /*=  10*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR0(0) },
	{ "D3DRS_PSCONSTANT0_1"               /*=  11*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR0(1) },
	{ "D3DRS_PSCONSTANT0_2"               /*=  12*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR0(2) },
	{ "D3DRS_PSCONSTANT0_3"               /*=  13*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR0(3) },
	{ "D3DRS_PSCONSTANT0_4"               /*=  14*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR0(4) },
	{ "D3DRS_PSCONSTANT0_5"               /*=  15*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR0(5) },
	{ "D3DRS_PSCONSTANT0_6"               /*=  16*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR0(6) },
	{ "D3DRS_PSCONSTANT0_7"               /*=  17*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR0(7) },
	{ "D3DRS_PSCONSTANT1_0"               /*=  18*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR1(0) },
	{ "D3DRS_PSCONSTANT1_1"               /*=  19*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR1(1) },
	{ "D3DRS_PSCONSTANT1_2"               /*=  20*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR1(2) },
	{ "D3DRS_PSCONSTANT1_3"               /*=  21*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR1(3) },
	{ "D3DRS_PSCONSTANT1_4"               /*=  22*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR1(4) },
	{ "D3DRS_PSCONSTANT1_5"               /*=  23*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR1(5) },
	{ "D3DRS_PSCONSTANT1_6"               /*=  24*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR1(6) },
	{ "D3DRS_PSCONSTANT1_7"               /*=  25*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR1(7) },
	{ "D3DRS_PSALPHAOUTPUTS0"             /*=  26*/, 3424, xtDWORD,               NV2A_RC_OUT_ALPHA(0) },
	{ "D3DRS_PSALPHAOUTPUTS1"             /*=  27*/, 3424, xtDWORD,               NV2A_RC_OUT_ALPHA(1) },
	{ "D3DRS_PSALPHAOUTPUTS2"             /*=  28*/, 3424, xtDWORD,               NV2A_RC_OUT_ALPHA(2) },
	{ "D3DRS_PSALPHAOUTPUTS3"             /*=  29*/, 3424, xtDWORD,               NV2A_RC_OUT_ALPHA(3) },
	{ "D3DRS_PSALPHAOUTPUTS4"             /*=  30*/, 3424, xtDWORD,               NV2A_RC_OUT_ALPHA(4) },
	{ "D3DRS_PSALPHAOUTPUTS5"             /*=  31*/, 3424, xtDWORD,               NV2A_RC_OUT_ALPHA(5) },
	{ "D3DRS_PSALPHAOUTPUTS6"             /*=  32*/, 3424, xtDWORD,               NV2A_RC_OUT_ALPHA(6) },
	{ "D3DRS_PSALPHAOUTPUTS7"             /*=  33*/, 3424, xtDWORD,               NV2A_RC_OUT_ALPHA(7) },
	{ "D3DRS_PSRGBINPUTS0"                /*=  34*/, 3424, xtDWORD,               NV2A_RC_IN_RGB(0) },
	{ "D3DRS_PSRGBINPUTS1"                /*=  35*/, 3424, xtDWORD,               NV2A_RC_IN_RGB(1) },
	{ "D3DRS_PSRGBINPUTS2"                /*=  36*/, 3424, xtDWORD,               NV2A_RC_IN_RGB(2) },
	{ "D3DRS_PSRGBINPUTS3"                /*=  37*/, 3424, xtDWORD,               NV2A_RC_IN_RGB(3) },
	{ "D3DRS_PSRGBINPUTS4"                /*=  38*/, 3424, xtDWORD,               NV2A_RC_IN_RGB(4) },
	{ "D3DRS_PSRGBINPUTS5"                /*=  39*/, 3424, xtDWORD,               NV2A_RC_IN_RGB(5) },
	{ "D3DRS_PSRGBINPUTS6"                /*=  40*/, 3424, xtDWORD,               NV2A_RC_IN_RGB(6) },
	{ "D3DRS_PSRGBINPUTS7"                /*=  41*/, 3424, xtDWORD,               NV2A_RC_IN_RGB(7) },
	{ "D3DRS_PSCOMPAREMODE"               /*=  42*/, 3424, xtDWORD,               NV2A_TX_SHADER_CULL_MODE },
	{ "D3DRS_PSFINALCOMBINERCONSTANT0"    /*=  43*/, 3424, xtDWORD,               NV2A_RC_COLOR0 },
	{ "D3DRS_PSFINALCOMBINERCONSTANT1"    /*=  44*/, 3424, xtDWORD,               NV2A_RC_COLOR1 },
	{ "D3DRS_PSRGBOUTPUTS0"               /*=  45*/, 3424, xtDWORD,               NV2A_RC_OUT_RGB(0) },
	{ "D3DRS_PSRGBOUTPUTS1"               /*=  46*/, 3424, xtDWORD,               NV2A_RC_OUT_RGB(1) },
	{ "D3DRS_PSRGBOUTPUTS2"               /*=  47*/, 3424, xtDWORD,               NV2A_RC_OUT_RGB(2) },
	{ "D3DRS_PSRGBOUTPUTS3"               /*=  48*/, 3424, xtDWORD,               NV2A_RC_OUT_RGB(3) },
	{ "D3DRS_PSRGBOUTPUTS4"               /*=  49*/, 3424, xtDWORD,               NV2A_RC_OUT_RGB(4) },
	{ "D3DRS_PSRGBOUTPUTS5"               /*=  50*/, 3424, xtDWORD,               NV2A_RC_OUT_RGB(5) },
	{ "D3DRS_PSRGBOUTPUTS6"               /*=  51*/, 3424, xtDWORD,               NV2A_RC_OUT_RGB(6) },
	{ "D3DRS_PSRGBOUTPUTS7"               /*=  52*/, 3424, xtDWORD,               NV2A_RC_OUT_RGB(7) },
	{ "D3DRS_PSCOMBINERCOUNT"             /*=  53*/, 3424, xtDWORD,               NV2A_RC_ENABLE },
	{ "D3DRS_PSTEXTUREMODES_RESERVED"     /*=  54*/, 3424, xtDWORD,               NV2A_CLEAR_VALUE }, // Dxbx note : This takes the slot of X_D3DPIXELSHADERDEF.PSTextureModes, set by D3DDevice_SetRenderState_LogicOp?
	{ "D3DRS_PSDOTMAPPING"                /*=  55*/, 3424, xtDWORD,               NV2A_TX_SHADER_DOTMAPPING },
	{ "D3DRS_PSINPUTTEXTURE"              /*=  56*/, 3424, xtDWORD,               NV2A_TX_SHADER_PREVIOUS },
	// End of "pixel-shader" render states, continuing with "simple" render states :
	{ "D3DRS_ZFUNC"                       /*=  57*/, 3424, xtD3DCMPFUNC,          NV2A_DEPTH_FUNC, D3DRS_ZFUNC },
	{ "D3DRS_ALPHAFUNC"                   /*=  58*/, 3424, xtD3DCMPFUNC,          NV2A_ALPHA_FUNC_FUNC, D3DRS_ALPHAFUNC },
	{ "D3DRS_ALPHABLENDENABLE"            /*=  59*/, 3424, xtBOOL,                NV2A_BLEND_FUNC_ENABLE, D3DRS_ALPHABLENDENABLE, "TRUE to enable alpha blending" },
	{ "D3DRS_ALPHATESTENABLE"             /*=  60*/, 3424, xtBOOL,                NV2A_ALPHA_FUNC_ENABLE, D3DRS_ALPHATESTENABLE, "TRUE to enable alpha tests" },
	{ "D3DRS_ALPHAREF"                    /*=  61*/, 3424, xtBYTE,                NV2A_ALPHA_FUNC_REF, D3DRS_ALPHAREF },
	{ "D3DRS_SRCBLEND"                    /*=  62*/, 3424, xtD3DBLEND,            NV2A_BLEND_FUNC_SRC, D3DRS_SRCBLEND },
	{ "D3DRS_DESTBLEND"                   /*=  63*/, 3424, xtD3DBLEND,            NV2A_BLEND_FUNC_DST, D3DRS_DESTBLEND },
	{ "D3DRS_ZWRITEENABLE"                /*=  64*/, 3424, xtBOOL,                NV2A_DEPTH_WRITE_ENABLE, D3DRS_ZWRITEENABLE, "TRUE to enable Z writes" },
	{ "D3DRS_DITHERENABLE"                /*=  65*/, 3424, xtBOOL,                NV2A_DITHER_ENABLE, D3DRS_DITHERENABLE, "TRUE to enable dithering" },
	{ "D3DRS_SHADEMODE"                   /*=  66*/, 3424, xtD3DSHADEMODE,        NV2A_SHADE_MODEL, D3DRS_SHADEMODE },
	{ "D3DRS_COLORWRITEENABLE"            /*=  67*/, 3424, xtD3DCOLORWRITEENABLE, NV2A_COLOR_MASK, D3DRS_COLORWRITEENABLE }, // *_ALPHA, etc. per-channel write enable
	{ "D3DRS_STENCILZFAIL"                /*=  68*/, 3424, xtD3DSTENCILOP,        NV2A_STENCIL_OP_ZFAIL, D3DRS_STENCILZFAIL, "Operation to do if stencil test passes and Z test fails" },
	{ "D3DRS_STENCILPASS"                 /*=  69*/, 3424, xtD3DSTENCILOP,        NV2A_STENCIL_OP_ZPASS, D3DRS_STENCILPASS, "Operation to do if both stencil and Z tests pass" },
	{ "D3DRS_STENCILFUNC"                 /*=  70*/, 3424, xtD3DCMPFUNC,          NV2A_STENCIL_FUNC_FUNC, D3DRS_STENCILFUNC },
	{ "D3DRS_STENCILREF"                  /*=  71*/, 3424, xtBYTE,                NV2A_STENCIL_FUNC_REF, D3DRS_STENCILREF, "BYTE reference value used in stencil test" },
	{ "D3DRS_STENCILMASK"                 /*=  72*/, 3424, xtBYTE,                NV2A_STENCIL_FUNC_MASK, D3DRS_STENCILMASK, "BYTE mask value used in stencil test" },
	{ "D3DRS_STENCILWRITEMASK"            /*=  73*/, 3424, xtBYTE,                NV2A_STENCIL_MASK, D3DRS_STENCILWRITEMASK, "BYTE write mask applied to values written to stencil buffer" },
	{ "D3DRS_BLENDOP"                     /*=  74*/, 3424, xtD3DBLENDOP,          NV2A_BLEND_EQUATION, D3DRS_BLENDOP },
	{ "D3DRS_BLENDCOLOR"                  /*=  75*/, 3424, xtD3DCOLOR,            NV2A_BLEND_COLOR, D3DRS_BLENDFACTOR, "D3DCOLOR for D3DBLEND_CONSTANTCOLOR" },
	// D3D9 D3DRS_BLENDFACTOR : D3DCOLOR used for a constant blend factor during alpha blending for devices that support D3DPBLENDCAPS_BLENDFACTOR
	{ "D3DRS_SWATHWIDTH"                  /*=  76*/, 3424, xtD3DSWATH,            NV2A_SWATH_WIDTH },
	{ "D3DRS_POLYGONOFFSETZSLOPESCALE"    /*=  77*/, 3424, xtFloat,               NV2A_POLYGON_OFFSET_FACTOR, D3DRS_UNSUPPORTED, "float Z factor for shadow maps" },
	{ "D3DRS_POLYGONOFFSETZOFFSET"        /*=  78*/, 3424, xtFloat,               NV2A_POLYGON_OFFSET_UNITS },
	{ "D3DRS_POINTOFFSETENABLE"           /*=  79*/, 3424, xtBOOL,                NV2A_POLYGON_OFFSET_POINT_ENABLE },
	{ "D3DRS_WIREFRAMEOFFSETENABLE"       /*=  80*/, 3424, xtBOOL,                NV2A_POLYGON_OFFSET_LINE_ENABLE },
	{ "D3DRS_SOLIDOFFSETENABLE"           /*=  81*/, 3424, xtBOOL,                NV2A_POLYGON_OFFSET_FILL_ENABLE },
	{ "D3DRS_DEPTHCLIPCONTROL"            /*=  82*/, 4432, xtD3DDCC,              NV2A_DEPTHCLIPCONTROL }, // Verified absent in 4361, present in 4432  TODO : Might be introduced around 4400?
	{ "D3DRS_STIPPLEENABLE"               /*=  83*/, 4627, xtBOOL,                NV2A_POLYGON_STIPPLE_ENABLE }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_SIMPLE_UNUSED8"              /*=  84*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_SIMPLE_UNUSED7"              /*=  85*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_SIMPLE_UNUSED6"              /*=  86*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_SIMPLE_UNUSED5"              /*=  87*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_SIMPLE_UNUSED4"              /*=  88*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_SIMPLE_UNUSED3"              /*=  89*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_SIMPLE_UNUSED2"              /*=  90*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_SIMPLE_UNUSED1"              /*=  91*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	// End of "simple" render states, continuing with "deferred" render states :
	// Verified as XDK 3911 Deferred RenderStates (3424 yet to do)
	{ "D3DRS_FOGENABLE"                   /*=  92*/, 3424, xtBOOL,                NV2A_FOG_ENABLE, D3DRS_FOGENABLE }, // TRUE to enable fog blending
	{ "D3DRS_FOGTABLEMODE"                /*=  93*/, 3424, xtD3DFOGMODE,          NV2A_FOG_MODE, D3DRS_FOGTABLEMODE }, // D3DFOGMODE
	{ "D3DRS_FOGSTART"                    /*=  94*/, 3424, xtFloat,               NV2A_FOG_COORD_DIST, D3DRS_FOGSTART }, // float fog start (for both vertex and pixel fog)
	{ "D3DRS_FOGEND"                      /*=  95*/, 3424, xtFloat,               NV2A_FOG_MODE, D3DRS_FOGEND }, // float fog end
	{ "D3DRS_FOGDENSITY"                  /*=  96*/, 3424, xtFloat,               NV2A_FOG_EQUATION_CONSTANT, D3DRS_FOGDENSITY }, // float fog density // + NV2A_FOG_EQUATION_LINEAR + NV2A_FOG_EQUATION_QUADRATIC
	{ "D3DRS_RANGEFOGENABLE"              /*=  97*/, 3424, xtBOOL,                NV2A_FOG_COORD_DIST, D3DRS_RANGEFOGENABLE }, // TRUE to enable range-based fog
	{ "D3DRS_WRAP0"                       /*=  98*/, 3424, xtD3DWRAP,             NV2A_TX_WRAP(0), D3DRS_WRAP0 }, // D3DWRAP* flags (D3DWRAP_U, D3DWRAPCOORD_0, etc.) for 1st texture coord.
	{ "D3DRS_WRAP1"                       /*=  99*/, 3424, xtD3DWRAP,             NV2A_TX_WRAP(1), D3DRS_WRAP1 }, // D3DWRAP* flags (D3DWRAP_U, D3DWRAPCOORD_0, etc.) for 2nd texture coord.
	{ "D3DRS_WRAP2"                       /*= 100*/, 3424, xtD3DWRAP,             NV2A_TX_WRAP(2), D3DRS_WRAP2 }, // D3DWRAP* flags (D3DWRAP_U, D3DWRAPCOORD_0, etc.) for 3rd texture coord.
	{ "D3DRS_WRAP3"                       /*= 101*/, 3424, xtD3DWRAP,             NV2A_TX_WRAP(3), D3DRS_WRAP3 }, // D3DWRAP* flags (D3DWRAP_U, D3DWRAPCOORD_0, etc.) for 4th texture coord.
	{ "D3DRS_LIGHTING"                    /*= 102*/, 3424, xtBOOL,                NV2A_LIGHT_MODEL, D3DRS_LIGHTING }, // TRUE to enable lighting // TODO : Needs push-buffer data conversion
	{ "D3DRS_SPECULARENABLE"              /*= 103*/, 3424, xtBOOL,                NV2A_RC_FINAL0, D3DRS_SPECULARENABLE }, // TRUE to enable specular
	{ "D3DRS_LOCALVIEWER"                 /*= 104*/, 3424, xtBOOL,                0, D3DRS_LOCALVIEWER }, // TRUE to enable camera-relative specular highlights
	{ "D3DRS_COLORVERTEX"                 /*= 105*/, 3424, xtBOOL,                0, D3DRS_COLORVERTEX }, // TRUE to enable per-vertex color
	{ "D3DRS_BACKSPECULARMATERIALSOURCE"  /*= 106*/, 3424, xtD3DMCS,              0 }, // D3DMATERIALCOLORSOURCE (Xbox extension) nsp.
	{ "D3DRS_BACKDIFFUSEMATERIALSOURCE"   /*= 107*/, 3424, xtD3DMCS,              0 }, // D3DMATERIALCOLORSOURCE (Xbox extension) nsp.
	{ "D3DRS_BACKAMBIENTMATERIALSOURCE"   /*= 108*/, 3424, xtD3DMCS,              0 }, // D3DMATERIALCOLORSOURCE (Xbox extension) nsp.
	{ "D3DRS_BACKEMISSIVEMATERIALSOURCE"  /*= 109*/, 3424, xtD3DMCS,              0 }, // D3DMATERIALCOLORSOURCE (Xbox extension) nsp.
	{ "D3DRS_SPECULARMATERIALSOURCE"      /*= 110*/, 3424, xtD3DMCS,              NV2A_COLOR_MATERIAL, D3DRS_SPECULARMATERIALSOURCE }, // D3DMATERIALCOLORSOURCE
	{ "D3DRS_DIFFUSEMATERIALSOURCE"       /*= 111*/, 3424, xtD3DMCS,              0, D3DRS_DIFFUSEMATERIALSOURCE }, // D3DMATERIALCOLORSOURCE
	{ "D3DRS_AMBIENTMATERIALSOURCE"       /*= 112*/, 3424, xtD3DMCS,              0, D3DRS_AMBIENTMATERIALSOURCE }, // D3DMATERIALCOLORSOURCE
	{ "D3DRS_EMISSIVEMATERIALSOURCE"      /*= 113*/, 3424, xtD3DMCS,              0, D3DRS_EMISSIVEMATERIALSOURCE }, // D3DMATERIALCOLORSOURCE
	{ "D3DRS_BACKAMBIENT"                 /*= 114*/, 3424, xtD3DCOLOR,            NV2A_LIGHT_MODEL_BACK_SIDE_PRODUCT_AMBIENT_PLUS_EMISSION_R }, // D3DCOLOR (Xbox extension) // ..NV2A_MATERIAL_FACTOR_BACK_B nsp. Was NV2A_LIGHT_MODEL_BACK_AMBIENT_R
	{ "D3DRS_AMBIENT"                     /*= 115*/, 3424, xtD3DCOLOR,            NV2A_LIGHT_MODEL_FRONT_SIDE_PRODUCT_AMBIENT_PLUS_EMISSION_R, D3DRS_AMBIENT }, // D3DCOLOR // ..NV2A_LIGHT_MODEL_FRONT_AMBIENT_B + NV2A_MATERIAL_FACTOR_FRONT_R..NV2A_MATERIAL_FACTOR_FRONT_A  Was NV2A_LIGHT_MODEL_FRONT_AMBIENT_R
	{ "D3DRS_POINTSIZE"                   /*= 116*/, 3424, xtFloat,               NV2A_POINT_PARAMETER(0), D3DRS_POINTSIZE }, // float point size
	{ "D3DRS_POINTSIZE_MIN"               /*= 117*/, 3424, xtFloat,               0, D3DRS_POINTSIZE_MIN }, // float point size min threshold
	{ "D3DRS_POINTSPRITEENABLE"           /*= 118*/, 3424, xtBOOL,                NV2A_POINT_SMOOTH_ENABLE, D3DRS_POINTSPRITEENABLE }, // TRUE to enable point sprites
	{ "D3DRS_POINTSCALEENABLE"            /*= 119*/, 3424, xtBOOL,                NV2A_POINT_PARAMETERS_ENABLE, D3DRS_POINTSCALEENABLE }, // TRUE to enable point size scaling
	{ "D3DRS_POINTSCALE_A"                /*= 120*/, 3424, xtFloat,               0, D3DRS_POINTSCALE_A }, // float point attenuation A value
	{ "D3DRS_POINTSCALE_B"                /*= 121*/, 3424, xtFloat,               0, D3DRS_POINTSCALE_B }, // float point attenuation B value
	{ "D3DRS_POINTSCALE_C"                /*= 122*/, 3424, xtFloat,               0, D3DRS_POINTSCALE_C }, // float point attenuation C value
	{ "D3DRS_POINTSIZE_MAX"               /*= 123*/, 3424, xtFloat,               0, D3DRS_POINTSIZE_MAX }, // float point size max threshold
	{ "D3DRS_PATCHEDGESTYLE"              /*= 124*/, 3424, xtDWORD,               0, D3DRS_PATCHEDGESTYLE }, // D3DPATCHEDGESTYLE
	{ "D3DRS_PATCHSEGMENTS"               /*= 125*/, 3424, xtDWORD,               0 }, // DWORD number of segments per edge when drawing patches, nsp (D3DRS_PATCHSEGMENTS exists in Direct3D 8, but not in 9)
	// TODO -oDxbx : Is X_D3DRS_SWAPFILTER really a xtD3DMULTISAMPLE_TYPE?
	{ "D3DRS_SWAPFILTER"                  /*= 126*/, 4034, xtD3DMULTISAMPLE_TYPE, 0, D3DRS_UNSUPPORTED, "D3DTEXF_LINEAR etc. filter to use for Swap" }, // nsp. Verified absent in 3944, present in 4034.  4034 state based on test-case : The Simpsons Road Rage
	{ "D3DRS_PRESENTATIONINTERVAL"        /*= 127*/, 4627, xtDWORD,               0 }, // nsp. Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_DEFERRED_UNUSED8"            /*= 128*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_DEFERRED_UNUSED7"            /*= 129*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_DEFERRED_UNUSED6"            /*= 130*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_DEFERRED_UNUSED5"            /*= 131*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_DEFERRED_UNUSED4"            /*= 132*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_DEFERRED_UNUSED3"            /*= 133*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_DEFERRED_UNUSED2"            /*= 134*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_DEFERRED_UNUSED1"            /*= 135*/, 4627, xtDWORD,               0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	// End of "deferred" render states, continuing with "complex" render states :
	{ "D3DRS_PSTEXTUREMODES"              /*= 136*/, 3424, xtDWORD,               0 }, // This is where pPSDef->PSTextureModes is stored (outside the pPSDEF - see DxbxUpdateActivePixelShader)
	{ "D3DRS_VERTEXBLEND"                 /*= 137*/, 3424, xtD3DVERTEXBLENDFLAGS, NV2A_SKIN_MODE, D3DRS_VERTEXBLEND },
	{ "D3DRS_FOGCOLOR"                    /*= 138*/, 3424, xtD3DCOLOR,            NV2A_FOG_COLOR, D3DRS_FOGCOLOR }, // SwapRgb
	{ "D3DRS_FILLMODE"                    /*= 139*/, 3424, xtD3DFILLMODE,         NV2A_POLYGON_MODE_FRONT, D3DRS_FILLMODE },
	{ "D3DRS_BACKFILLMODE"                /*= 140*/, 3424, xtD3DFILLMODE,         NV2A_POLYGON_MODE_BACK }, // nsp.
	{ "D3DRS_TWOSIDEDLIGHTING"            /*= 141*/, 3424, xtBOOL,                0 }, // nsp.  // FIXME map from NV2A_LIGHT_MODEL
	{ "D3DRS_NORMALIZENORMALS"            /*= 142*/, 3424, xtBOOL,                NV2A_NORMALIZE_ENABLE, D3DRS_NORMALIZENORMALS },
	{ "D3DRS_ZENABLE"                     /*= 143*/, 3424, xtBOOL,                NV2A_DEPTH_TEST_ENABLE, D3DRS_ZENABLE }, // D3DZBUFFERTYPE?
	{ "D3DRS_STENCILENABLE"               /*= 144*/, 3424, xtBOOL,                NV2A_STENCIL_ENABLE, D3DRS_STENCILENABLE },
	{ "D3DRS_STENCILFAIL"                 /*= 145*/, 3424, xtD3DSTENCILOP,        NV2A_STENCIL_OP_FAIL, D3DRS_STENCILFAIL },
	{ "D3DRS_FRONTFACE"                   /*= 146*/, 3424, xtD3DFRONT,            NV2A_FRONT_FACE }, // nsp.
	{ "D3DRS_CULLMODE"                    /*= 147*/, 3424, xtD3DCULL,             NV2A_CULL_FACE, D3DRS_CULLMODE },
	{ "D3DRS_TEXTUREFACTOR"               /*= 148*/, 3424, xtD3DCOLOR,            NV2A_RC_CONSTANT_COLOR0(0), D3DRS_TEXTUREFACTOR },
	{ "D3DRS_ZBIAS"                       /*= 149*/, 3424, xtLONG,                0, D3DRS_DEPTHBIAS }, // Was D3DRS_ZBIAS
	{ "D3DRS_LOGICOP"                     /*= 150*/, 3424, xtD3DLOGICOP,          NV2A_COLOR_LOGIC_OP_OP }, // nsp.
	{ "D3DRS_EDGEANTIALIAS"               /*= 151*/, 3424, xtBOOL,                NV2A_LINE_SMOOTH_ENABLE, D3DRS_ANTIALIASEDLINEENABLE }, // Was D3DRS_EDGEANTIALIAS. Dxbx note : No Xbox ext. (according to Direct3D8) !
	{ "D3DRS_MULTISAMPLEANTIALIAS"        /*= 152*/, 3424, xtBOOL,                NV2A_MULTISAMPLE_CONTROL, D3DRS_MULTISAMPLEANTIALIAS },
	{ "D3DRS_MULTISAMPLEMASK"             /*= 153*/, 3424, xtDWORD,               NV2A_MULTISAMPLE_CONTROL, D3DRS_MULTISAMPLEMASK },
	{ "D3DRS_MULTISAMPLETYPE"             /*= 154*/, 3424, xtD3DMULTISAMPLE_TYPE, 0, D3DRS_UNSUPPORTED, "aliasses D3DMULTISAMPLE_TYPE, removed from 4034 onward", 4034 }, // Verified present in 3944, removed in 4034. 4034 state based on test-case : The Simpsons Road Rage
	{ "D3DRS_MULTISAMPLEMODE"             /*= 155*/, 4034, xtD3DMULTISAMPLEMODE,  0 }, // D3DMULTISAMPLEMODE for the backbuffer. Verified absent in 3944, present in 4034.  4034 state based on test-case : The Simpsons Road Rage
	{ "D3DRS_MULTISAMPLERENDERTARGETMODE" /*= 156*/, 4034, xtD3DMULTISAMPLEMODE,  NV2A_RT_FORMAT }, // Verified absent in 3944, present in 4034. Presence in 4034 is based on test-case : The Simpsons Road Rage
	{ "D3DRS_SHADOWFUNC"                  /*= 157*/, 3424, xtD3DCMPFUNC,          NV2A_TX_RCOMP },
	{ "D3DRS_LINEWIDTH"                   /*= 158*/, 3424, xtFloat,               NV2A_LINE_WIDTH },
	{ "D3DRS_SAMPLEALPHA"                 /*= 159*/, 4627, xtD3DSAMPLEALPHA,      0 }, // Verified absent in 4531, present in 4627  TODO : might be introduced in between?
	{ "D3DRS_DXT1NOISEENABLE"             /*= 160*/, 3424, xtBOOL,                NV2A_CLEAR_DEPTH_VALUE },
	{ "D3DRS_YUVENABLE"                   /*= 161*/, 3911, xtBOOL,                NV2A_CONTROL0 }, // Verified present in 3944
	{ "D3DRS_OCCLUSIONCULLENABLE"         /*= 162*/, 3911, xtBOOL,                NV2A_OCCLUDE_ZSTENCIL_EN }, // Verified present in 3944
	{ "D3DRS_STENCILCULLENABLE"           /*= 163*/, 3911, xtBOOL,                NV2A_OCCLUDE_ZSTENCIL_EN }, // Verified present in 3944
	{ "D3DRS_ROPZCMPALWAYSREAD"           /*= 164*/, 3911, xtBOOL,                0 }, // Verified present in 3944
	{ "D3DRS_ROPZREAD"                    /*= 165*/, 3911, xtBOOL,                0 }, // Verified present in 3944
	{ "D3DRS_DONOTCULLUNCOMPRESSED"       /*= 166*/, 3911, xtBOOL,                0 } // Verified present in 3944
};

const RenderStateInfo& GetDxbxRenderStateInfo(int State)
{
	return DxbxRenderStateInfo[State];
}

/*Direct3D8 states unused :
	D3DRS_LINEPATTERN
	D3DRS_LASTPIXEL
	D3DRS_ZVISIBLE
	D3DRS_WRAP4
	D3DRS_WRAP5
	D3DRS_WRAP6
	D3DRS_WRAP7
	D3DRS_CLIPPING
	D3DRS_FOGVERTEXMODE
	D3DRS_CLIPPLANEENABLE
	D3DRS_SOFTWAREVERTEXPROCESSING
	D3DRS_DEBUGMONITORTOKEN
	D3DRS_INDEXEDVERTEXBLENDENABLE
	D3DRS_TWEENFACTOR
	D3DRS_POSITIONORDER
	D3DRS_NORMALORDER

Direct3D9 states unused :
	D3DRS_SCISSORTESTENABLE = 174,
	D3DRS_SLOPESCALEDEPTHBIAS = 175,
	D3DRS_ANTIALIASEDLINEENABLE = 176,
	D3DRS_MINTESSELLATIONLEVEL = 178,
	D3DRS_MAXTESSELLATIONLEVEL = 179,
	D3DRS_ADAPTIVETESS_X = 180,
	D3DRS_ADAPTIVETESS_Y = 181,
	D3DRS_ADAPTIVETESS_Z = 182,
	D3DRS_ADAPTIVETESS_W = 183,
	D3DRS_ENABLEADAPTIVETESSELLATION = 184,
	D3DRS_TWOSIDEDSTENCILMODE = 185,   // BOOL enable/disable 2 sided stenciling
	D3DRS_CCW_STENCILFAIL = 186,   // D3DSTENCILOP to do if ccw stencil test fails
	D3DRS_CCW_STENCILZFAIL = 187,   // D3DSTENCILOP to do if ccw stencil test passes and Z test fails
	D3DRS_CCW_STENCILPASS = 188,   // D3DSTENCILOP to do if both ccw stencil and Z tests pass
	D3DRS_CCW_STENCILFUNC = 189,   // D3DCMPFUNC fn.  ccw Stencil Test passes if ((ref & mask) stencilfn (stencil & mask)) is true
	D3DRS_COLORWRITEENABLE1 = 190,   // Additional ColorWriteEnables for the devices that support D3DPMISCCAPS_INDEPENDENTWRITEMASKS
	D3DRS_COLORWRITEENABLE2 = 191,   // Additional ColorWriteEnables for the devices that support D3DPMISCCAPS_INDEPENDENTWRITEMASKS
	D3DRS_COLORWRITEENABLE3 = 192,   // Additional ColorWriteEnables for the devices that support D3DPMISCCAPS_INDEPENDENTWRITEMASKS
	D3DRS_SRGBWRITEENABLE = 194,   // Enable rendertarget writes to be DE-linearized to SRGB (for formats that expose D3DUSAGE_QUERY_SRGBWRITE)
	D3DRS_DEPTHBIAS = 195,
	D3DRS_WRAP8 = 198,   // Additional wrap states for vs_3_0+ attributes with D3DDECLUSAGE_TEXCOORD
	D3DRS_WRAP9 = 199,
	D3DRS_WRAP10 = 200,
	D3DRS_WRAP11 = 201,
	D3DRS_WRAP12 = 202,
	D3DRS_WRAP13 = 203,
	D3DRS_WRAP14 = 204,
	D3DRS_WRAP15 = 205,
	D3DRS_SEPARATEALPHABLENDENABLE = 206,  // TRUE to enable a separate blending function for the alpha channel
	D3DRS_SRCBLENDALPHA = 207,  // SRC blend factor for the alpha channel when D3DRS_SEPARATEDESTALPHAENABLE is TRUE
	D3DRS_DESTBLENDALPHA = 208,  // DST blend factor for the alpha channel when D3DRS_SEPARATEDESTALPHAENABLE is TRUE
	D3DRS_BLENDOPALPHA = 209   // Blending operation for the alpha channel when D3DRS_SEPARATEDESTALPHAENABLE is TRUE
*/

xbox::X_D3DFORMAT GetXboxPixelContainerFormat(const xbox::dword_xt XboxPixelContainer_Format)
{
	xbox::X_D3DFORMAT d3d_format = (xbox::X_D3DFORMAT)((XboxPixelContainer_Format & X_D3DFORMAT_FORMAT_MASK) >> X_D3DFORMAT_FORMAT_SHIFT);
	return d3d_format;
}

xbox::X_D3DFORMAT GetXboxPixelContainerFormat(const xbox::X_D3DPixelContainer* pXboxPixelContainer)
{
	// Don't pass in unassigned Xbox pixel container
	assert(pXboxPixelContainer != xbox::zeroptr);

	return GetXboxPixelContainerFormat(pXboxPixelContainer->Format);
}

void CxbxGetPixelContainerMeasures
(
	xbox::X_D3DPixelContainer* pPixelContainer,
	// TODO : Add X_D3DCUBEMAP_FACES argument
	DWORD dwMipMapLevel, // unused - TODO : Use
	UINT* pWidth,
	UINT* pHeight,
	UINT* pDepth,
	UINT* pRowPitch,
	// Slice pitch (does not include mipmaps!)
	UINT* pSlicePitch
)
{
	DWORD Size = pPixelContainer->Size;
	xbox::X_D3DFORMAT X_Format = GetXboxPixelContainerFormat(pPixelContainer);

	if (Size != 0)
	{
		*pDepth = 1;
		*pWidth = ((Size & X_D3DSIZE_WIDTH_MASK) /* >> X_D3DSIZE_WIDTH_SHIFT*/) + 1;
		*pHeight = ((Size & X_D3DSIZE_HEIGHT_MASK) >> X_D3DSIZE_HEIGHT_SHIFT) + 1;
		*pRowPitch = (((Size & X_D3DSIZE_PITCH_MASK) >> X_D3DSIZE_PITCH_SHIFT) + 1) * X_D3DTEXTURE_PITCH_ALIGNMENT;
	}
	else
	{
		DWORD l2w = (pPixelContainer->Format & X_D3DFORMAT_USIZE_MASK) >> X_D3DFORMAT_USIZE_SHIFT;
		DWORD l2h = (pPixelContainer->Format & X_D3DFORMAT_VSIZE_MASK) >> X_D3DFORMAT_VSIZE_SHIFT;
		DWORD l2d = (pPixelContainer->Format & X_D3DFORMAT_PSIZE_MASK) >> X_D3DFORMAT_PSIZE_SHIFT;
		DWORD dwBPP = EmuXBFormatBitsPerPixel(X_Format);

		*pDepth = 1 << l2d;
		*pHeight = 1 << l2h;
		*pWidth = 1 << l2w;
		*pRowPitch = (*pWidth) * dwBPP / 8;
	}

	*pSlicePitch = (*pRowPitch) * (*pHeight);

	if (EmuXBFormatIsCompressed(X_Format)) {
		*pRowPitch *= 4;
	}
}

bool ConvertD3DTextureToARGBBuffer(
	xbox::X_D3DFORMAT X_Format,
	uint8_t* pSrc,
	int SrcWidth, int SrcHeight, int SrcRowPitch, int SrcSlicePitch,
	uint8_t* pDst, int DstRowPitch, int DstSlicePitch,
	unsigned int uiDepth ,
	xbox::PVOID pPalleteData
)
{
	const FormatToARGBRow ConvertRowToARGB = EmuXBFormatComponentConverter(X_Format);
	if (ConvertRowToARGB == nullptr)
		return false; // Unhandled conversion

	uint8_t* unswizleBuffer = nullptr;
	if (EmuXBFormatIsSwizzled(X_Format)) {
		unswizleBuffer = (uint8_t*)malloc(SrcSlicePitch * uiDepth); // TODO : Reuse buffer when performance is important
		// First we need to unswizzle the texture data
		EmuUnswizzleBox(
			pSrc, SrcWidth, SrcHeight, uiDepth,
			EmuXBFormatBytesPerPixel(X_Format),
			// Note : use src pitch on dest, because this is an intermediate step :
			unswizleBuffer, SrcRowPitch, SrcSlicePitch
		);
		// Convert colors from the unswizzled buffer
		pSrc = unswizleBuffer;
	}

	int AdditionalArgument;
	if (X_Format == xbox::X_D3DFMT_P8)
		AdditionalArgument = (int)pPalleteData;
	else
		AdditionalArgument = DstRowPitch;

	if (EmuXBFormatIsCompressed(X_Format)) {
		if (SrcWidth < 4 || SrcHeight < 4) {
			// HACK: The compressed DXT conversion code currently writes more pixels than it should, which can cause a crash.
			// This code will get hit when converting compressed texture mipmaps on hardware that somehow doesn't support DXT natively
			// (or lied when Cxbx asked it if it does!)
			EmuLog(LOG_LEVEL::WARNING, "Converting DXT textures smaller than a block is not currently implemented. Ignoring conversion!");
			return true;
		}

		// All compressed formats (DXT1, DXT3 and DXT5) encode blocks of 4 pixels on 4 lines
		SrcHeight = (SrcHeight + 3) / 4;
		DstRowPitch *= 4;
	}

	uint8_t* pSrcSlice = pSrc;
	uint8_t* pDstSlice = pDst;
	for (unsigned int z = 0; z < uiDepth; z++) {
		uint8_t* pSrcRow = pSrcSlice;
		uint8_t* pDstRow = pDstSlice;
		for (int y = 0; y < SrcHeight; y++) {
			*(int*)pDstRow = AdditionalArgument; // Dirty hack, to avoid an extra parameter to all conversion callbacks
			ConvertRowToARGB(pSrcRow, pDstRow, SrcWidth);
			pSrcRow += SrcRowPitch;
			pDstRow += DstRowPitch;
		}

		pSrcSlice += SrcSlicePitch;
		pDstSlice += DstSlicePitch;
	}

	if (unswizleBuffer)
		free(unswizleBuffer);

	return true;
}

// Called by WndMain::LoadGameLogo() to load game logo bitmap
uint8_t* ConvertD3DTextureToARGB(
	xbox::X_D3DPixelContainer* pXboxPixelContainer,
	uint8_t* pSrc,
	int* pWidth, int* pHeight,
	xbox::PVOID pPalleteData // default = zeroptr
)
{
	// Avoid allocating pDest when ConvertD3DTextureToARGBBuffer will fail anyway
	xbox::X_D3DFORMAT X_Format = GetXboxPixelContainerFormat(pXboxPixelContainer);
	const FormatToARGBRow ConvertRowToARGB = EmuXBFormatComponentConverter(X_Format);
	if (ConvertRowToARGB == nullptr)
		return nullptr; // Unhandled conversion

	unsigned int SrcDepth, SrcRowPitch, SrcSlicePitch;
	CxbxGetPixelContainerMeasures(
		pXboxPixelContainer,
		0, // dwMipMapLevel
		(UINT*)pWidth,
		(UINT*)pHeight,
		&SrcDepth,
		&SrcRowPitch,
		&SrcSlicePitch
	);

	// Now we know ConvertD3DTextureToARGBBuffer will do it's thing, allocate the resulting buffer
	int DstDepth = 1; // for now TODO : Use SrcDepth when supporting volume textures
	int DstRowPitch = (*pWidth) * sizeof(DWORD); // = sizeof ARGB pixel. TODO : Is this correct?
	int DstSlicePitch = DstRowPitch * (*pHeight); // TODO : Is this correct?
	int DstSize = DstSlicePitch * DstDepth;
	uint8_t* pDst = (uint8_t*)malloc(DstSize);

	// And convert the source towards that buffer
	/*ignore result*/ConvertD3DTextureToARGBBuffer(
		X_Format,
		pSrc, *pWidth, *pHeight, SrcRowPitch, SrcSlicePitch,
		pDst, DstRowPitch, DstSlicePitch,
		DstDepth,
		pPalleteData);

	// NOTE : Caller must take ownership!
	return pDst;
}

void CxbxSetPixelContainerHeader
(
	xbox::X_D3DPixelContainer* pPixelContainer,
	DWORD				Common,
	UINT				Width,
	UINT				Height,
	UINT				Levels,
	xbox::X_D3DFORMAT	Format,
	UINT				Dimensions,
	UINT				Pitch
)
{
	// Set X_D3DResource field(s) :
	pPixelContainer->Common = Common;
	// DON'T SET pPixelContainer->Data
	// DON'T SET pPixelContainer->Lock

	// Are Width and Height both a power of two?
	DWORD l2w; _BitScanReverse(&l2w, Width); // MSVC intrinsic; GCC has __builtin_clz
	DWORD l2h; _BitScanReverse(&l2h, Height);
	DWORD l2d = 0; // TODO : Set this via input argument
	if (((1 << l2w) == Width) && ((1 << l2h) == Height)) {
		Width = Height = Pitch = 1; // When setting Format, clear Size field
	}
	else {
		l2w = l2h = l2d = 0; // When setting Size, clear D3DFORMAT_USIZE, VSIZE and PSIZE
	}

	// Set X_D3DPixelContainer field(s) :
	pPixelContainer->Format = 0
		| ((Dimensions << X_D3DFORMAT_DIMENSION_SHIFT) & X_D3DFORMAT_DIMENSION_MASK)
		| (((DWORD)Format << X_D3DFORMAT_FORMAT_SHIFT) & X_D3DFORMAT_FORMAT_MASK)
		| ((Levels << X_D3DFORMAT_MIPMAP_SHIFT) & X_D3DFORMAT_MIPMAP_MASK)
		| ((l2w << X_D3DFORMAT_USIZE_SHIFT) & X_D3DFORMAT_USIZE_MASK)
		| ((l2h << X_D3DFORMAT_VSIZE_SHIFT) & X_D3DFORMAT_VSIZE_MASK)
		| ((l2d << X_D3DFORMAT_PSIZE_SHIFT) & X_D3DFORMAT_PSIZE_MASK)
		;
	pPixelContainer->Size = 0
		| (((Width - 1) /*X_D3DSIZE_WIDTH_SHIFT*/) & X_D3DSIZE_WIDTH_MASK)
		| (((Height - 1) << X_D3DSIZE_HEIGHT_SHIFT) & X_D3DSIZE_HEIGHT_MASK)
		| (((Pitch - 1) << X_D3DSIZE_PITCH_SHIFT) & X_D3DSIZE_PITCH_MASK)
		;
}

#if 0
/* Generic swizzle function, usable for both x and y dimensions.
When passing x, Max should be 2*height, and Shift should be 0
When passing y, Max should be width, and Shift should be 1 */
uint32 Swizzle(uint32 value, uint32 max, uint32 shift)
{
	uint32 result;

	if (value < max)
		result = value;
	else
		result = value % max;

	// The following is based on https://graphics.stanford.edu/~seander/bithacks.html#InterleaveBMN :
	// --------------------------------11111111111111111111111111111111
	result = (result | (result << 8)) & 0x00FF00FF; // 0000000000000000111111111111111100000000000000001111111111111111
	result = (result | (result << 4)) & 0x0F0F0F0F; // 0000111100001111000011110000111100001111000011110000111100001111
	result = (result | (result << 2)) & 0x33333333; // 0011001100110011001100110011001100110011001100110011001100110011
	result = (result | (result << 1)) & 0x55555555; // 0101010101010101010101010101010101010101010101010101010101010101

	result = result << shift; // y counts twice :      1010101010101010101010101010101010101010101010101010101010101010

	if (value >= max)
		result += (value / max) * max * max >> (1 - shift);  // x halves this

	return result;
}


typedef uint16 TRGB16;

// test-case: Frogger, Turok, Crazy Taxi 3 and many more
bool WndMain::ReadS3TCFormatIntoBitmap(uint32 format, unsigned char *data, uint32 dataSize, int width, int height, int pitch, void*& bitmap)
{
	uint16 color[3];
	TRGB32 color32b[4];
	uint32 r, g, b, r1, g1, b1, pixelmap, j;
	int k, p, x, y;

	r = g = b = r1 = g1 = b1 = pixelmap = 0;
	j = k = p = x = y = 0;

	// sanity checks
	if (format != xbox::X_D3DFMT_DXT1 && format != xbox::X_D3DFMT_DXT3 && format != xbox::X_D3DFMT_DXT5)
		return false;
	if (!(width > 0) || !(height > 0))
		return false;

	while (j < dataSize) {

		if (format != xbox::X_D3DFMT_DXT1) // Skip xbox::X_D3DFMT_DXT3 and xbox::X_D3DFMT_DXT5 alpha data (ported from Dxbx)
			j += 8;

		// Read two 16-bit pixels
		color[0] = (data[j + 0] << 0) + (data[j + 1] << 8);
		color[1] = (data[j + 2] << 0) + (data[j + 3] << 8);

		// Read 5+6+5 bit color channels and convert them to 8+8+8 bit :
		r = ((color[0] >> 11) & 31) * 255 / 31;
		g = ((color[0] >> 5) & 63) * 255 / 63;
		b = ((color[0]) & 31) * 255 / 31;

		r1 = ((color[1] >> 11) & 31) * 255 / 31;
		g1 = ((color[1] >> 5) & 63) * 255 / 63;
		b1 = ((color[1]) & 31) * 255 / 31;

		// Build first half of RGB32 color map :
		color32b[0].R = (unsigned char)r;
		color32b[0].G = (unsigned char)g;
		color32b[0].B = (unsigned char)b;

		color32b[1].R = (unsigned char)r1;
		color32b[1].G = (unsigned char)g1;
		color32b[1].B = (unsigned char)b1;

		// Build second half of RGB32 color map :
		if (color[0] > color[1])
		{
			// Make up 2 new colors, 1/3 A + 2/3 B and 2/3 A + 1/3 B :
			color32b[2].R = (unsigned char)((r + r + r1 + 2) / 3);
			color32b[2].G = (unsigned char)((g + g + g1 + 2) / 3);
			color32b[2].B = (unsigned char)((b + b + b1 + 2) / 3);

			color32b[3].R = (unsigned char)((r + r1 + r1 + 2) / 3);
			color32b[3].G = (unsigned char)((g + g1 + g1 + 2) / 3);
			color32b[3].B = (unsigned char)((b + b1 + b1 + 2) / 3);
		}
		else
		{
			// Make up one new color : 1/2 A + 1/2 B :
			color32b[2].R = (unsigned char)((r + r1) / 2);
			color32b[2].G = (unsigned char)((g + g1) / 2);
			color32b[2].B = (unsigned char)((b + b1) / 2);

			color32b[3].R = (unsigned char)0;
			color32b[3].G = (unsigned char)0;
			color32b[3].B = (unsigned char)0;
		}

		x = (k / 2) % width;
		y = (k / 2) / width * 4;

		// Forza Motorsport needs this safety measure, as it has dataSize=147456, while we need 16384 bytes :
		if (y >= height)
			break;

		pixelmap = (data[j + 4] << 0)
			+ (data[j + 5] << 8)
			+ (data[j + 6] << 16)
			+ (data[j + 7] << 24);

		for (p = 0; p < 16; p++)
		{
			((TRGB32*)bitmap)[x + (p & 3) + (pitch / sizeof(TRGB32) * (y + (p >> 2)))] = color32b[pixelmap & 3];
			pixelmap >>= 2;
		};

		j += 8;
		k += 8;

	}
	return true;
}

// test-case: Baku Baku 2 (Homebrew)
bool WndMain::ReadSwizzledFormatIntoBitmap(uint32 format, unsigned char *data, uint32 dataSize, int width, int height, int pitch, void*& bitmap)
{
	uint32* xSwizzle;
	int x, y;
	uint32	ySwizzle;
	TRGB32* yscanline;

	// sanity checks
	if (format != xbox::X_D3DFMT_A8R8G8B8 && format != xbox::X_D3DFMT_X8R8G8B8)
		return false;
	if (!(width > 0) || !(height > 0))
		return false;

	xSwizzle = (uint32*)malloc(sizeof(uint32) * width);
	if (xSwizzle == nullptr)
		return false;

	for (x = 0; x < width; x++)
		xSwizzle[x] = Swizzle(x, (height * 2), 0);

	// Loop over all lines :
	for (y = 0; y < height; y++)
	{
		// Calculate y-swizzle :
		ySwizzle = Swizzle(y, width, 1);

		// Copy whole line in one go (using pre-calculated x-swizzle) :
		yscanline = &((TRGB32*)bitmap)[y*(pitch / sizeof(TRGB32))];

		for (x = 0; x < width; x++)
			yscanline[x] = ((TRGB32*)data)[xSwizzle[x] + ySwizzle];
	}

	free(xSwizzle);
	return true;
}

// UNTESTED - Need test-case! (Sorry I wasn't able to find a game using this format)
bool WndMain::ReadSwizzled16bitFormatIntoBitmap(uint32 format, unsigned char *data, uint32 dataSize, int width, int height, int pitch, void*& bitmap)
{
	uint32* xSwizzle;
	int x, y;
	uint32	ySwizzle;
	TRGB16* yscanline;

	// sanity checks
	if (format != xbox::X_D3DFMT_R5G6B5)
		return false;
	if (!(width > 0) || !(height > 0))
		return false;

	xSwizzle = (uint32*)malloc(sizeof(uint32) * width);
	if (xSwizzle == nullptr)
		return false;

	for (x = 0; x < width; x++)
		xSwizzle[x] = Swizzle(x, (height * 2), 0);

	// Loop over all lines :
	for (y = 0; y < height; y++)
	{
		// Calculate y-swizzle :
		ySwizzle = Swizzle(y, width, 1);

		// Copy whole line in one go (using pre-calculated x-swizzle) :
		yscanline = &((TRGB16*)bitmap)[y*(pitch / sizeof(TRGB16))];
		for (x = 0; x < width; x++)
			yscanline[x] = ((TRGB16*)data)[xSwizzle[x] + ySwizzle];
	}

	free(xSwizzle);
	return true;
}
#endif
