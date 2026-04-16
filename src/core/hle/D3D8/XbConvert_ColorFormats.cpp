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

// _ComponentEncoding is now defined in XbConvert.h

// Bitfield extraction macros, reading a field from right-to-left specified bit widths.
// Invoker is responsible for passing in assigned and big enough data pointers.
// Input and return values are unsigned bytes.
// The *_top pairs replace a single AND (in extract) and two shifts (in expand) by just two shifts (one in extract and one in expand)
// Note : Below expressions must be embraced, otherwise macro-expansion would cause incorrect expressions!

// 3 components, 5:5:6
#define  extract1from556_top(src)  (src[0] << 2)
#define  extract1from556(src)  (src[0] & 0x3f)
#ifdef BIG_ENDIAN
#define  extract2from556(src) ((src[0] >> 6) | ((src[1] & 0x07) << 2))
#else // LITTLE_ENDIAN
#define  extract2from556(src) ((*(uint16_t*)src >> 6) & 0x1f)
#endif
#define  extract3from556(src)  (src[1] >> 3)

// 3 components, 5:6:5
#define  extract1from565_top(src)  (src[0] << 3)
#define  extract1from565(src)  (src[0] & 0x1f)
#ifdef BIG_ENDIAN
#define  extract2from565(src) ((src[0] >> 5) | ((src[1] & 0x07) << 3))
#else // LITTLE_ENDIAN
#define  extract2from565(src) ((*(uint16_t*)src >> 5) & 0x1f)
#endif
#define  extract3from565(src)  (src[1] >> 3)

// 3 components, 6:5:5
#define  extract1from655_top(src)  (src[0] << 3)
#define  extract1from655(src)  (src[0] & 0x1f)
#ifdef BIG_ENDIAN
#define  extract2from655(src) ((src[0] >> 5) | ((src[1] & 0x03) << 3))
#else // LITTLE_ENDIAN
#define  extract2from655(src) ((*(uint16_t*)src >> 5) & 0x1f)
#endif
#define  extract3from655(src)  (src[1] >> 2)

// 4 components, 1:5:5:5
#define extract1from1555_top(src)  (src[0] << 3)
#define extract1from1555(src)  (src[0] & 0x1f)
#ifdef BIG_ENDIAN
#define extract2from1555(src) ((src[0] >> 5) | ((src[1] & 0x03) << 3))
#define extract3from1555(src) ((src[1] >> 2) & 0x1f)
#else // LITTLE_ENDIAN
#define extract2from1555(src) ((*(uint16_t*)src >> 5) & 0x1f)
#define extract3from1555(src) ((*(uint16_t*)src >> 10) & 0x1f)
#endif
#define extract4from1555(src)  (src[1] >> 7)

// 4 components, 5:5:5:1
#define extract1from5551(src)  (src[0] & 1)
#define extract2from5551(src) ((src[0] >> 1) & 0x1f)
#ifdef BIG_ENDIAN
#define extract3from5551(src) ((src[0] >> 6) | ((src[1] & 0x07) << 2))
#else // LITTLE_ENDIAN
#define extract3from5551(src) ((*(uint16_t*)src >> 6) & 0x1f)
#endif
#define extract4from5551(src)  (src[1] >> 3)

// 4 components, 4:4:4:4
#define extract1from4444_top(src)  (src[0] << 4)
#define extract1from4444(src)  (src[0] & 0x0f)
#define extract2from4444(src)  (src[0] >> 4)
#define extract3from4444_top(src)  (src[1] << 4)
#define extract3from4444(src)  (src[1] & 0x0f)
#define extract4from4444(src)  (src[1] >> 4)

// Range expansion for less-than-8 bit unsigned values, towards 8-bit unsigned values
// by repeating the topmost bits, resulting in a linear range in the full 8-bits output.
// Note : Input values *must not* exceed the number of significant input bits.
// Guarding against that is caller's responsibility.

#if 0 // unused
inline uint8_t u_expand7(const uint8_t value)
{
	return (value << 1) | (value >> 6);
}
#endif

inline uint8_t u_expand6_top(const uint8_t value)
{
	return value | (value >> 6);
}

inline uint8_t u_expand6(const uint8_t value)
{
	return (value << 2) | (value >> 4);
}

inline uint8_t u_expand5_top(const uint8_t value)
{
	return value | (value >> 5);
}

inline uint8_t u_expand5(const uint8_t value)
{
	return (value << 3) | (value >> 2);
}

inline uint8_t u_expand4_top(const uint8_t value)
{
	return value | (value >> 4);
}

inline uint8_t u_expand4(const uint8_t value)
{
	return (value << 4) | value;
}

#if 0 // unused
inline uint8_t u_expand3(const uint8_t value)
{
	return (value << 5) | (value << 2) || (value >> 1);
}

inline uint8_t u_expand2(const uint8_t value)
{
	return (value << 6) | (value << 4) || (value << 2) | value;
}
#endif

inline uint8_t u_expand1(const uint8_t value)
{
	return -value; // bit hack : zero stays zero, 1 becomes -1 (all-ones)
}

// Range expansion for input with a sign bit
inline uint8_t s_expand6(const uint8_t value)
{
	return (value << 2) | ((value >> 3) & 0x03);
}

inline uint8_t s_expand5(const uint8_t value)
{
	return (value << 3) | ((value >> 1) & 0x07);
}


// Conversion functions derived from libyuv
// See https://chromium.googlesource.com/libyuv/libyuv/+/master/source/row_common.cc
//
// Here a few layouts, with capital letters indicating most significant bit:
// Note, that source and destination formats are declared right-to-left.
//
// ARGB     : byte[0]=Bbbbbbbb, byte[1]=Gggggggg, byte[2]=Rrrrrrrr, byte[3]=Aaaaaaaa (destination format)
//
// ARGB4444 : byte[0]=GgggBbbb, byte[1]=AaaaRrrr
// RGB565   : byte[0]=gggBbbbb, byte[1]=RrrrrGgg
// ARGB1555 : byte[0]=gggBbbbb, byte[1]=ARrrrrGg
// X1R5G5B5 : byte[0]=gggBbbbb, byte[1]=XRrrrrGg (X is ignored)
// R6G5B5   : byte[0]=gggBbbbb, byte[1]=RrrrrrGg
// L6V5U5   : byte[0]=vvvUuuuu, byte[1]=LlllllVv (Note : U and V, but not L, are stored in two's complement, signed, form)
// R5G5B5A1 : byte[0]=ggBbbbbA, byte[1]=RrrrrGgg
// R4G4B4A4 : byte[0]=BbbbAaaa, byte[1]=RrrrGggg

void RGB565ToARGBRow_C(const uint8_t* src_rgb565, uint8_t* dst_argb, int width)
{
	int x;
	for (x = 0; x < width; ++x) {
		uint8_t b5 = extract1from565_top(src_rgb565);
		uint8_t g6 = extract2from565(src_rgb565);
		uint8_t r5 = extract3from565(src_rgb565);
		dst_argb[0] = u_expand5_top(b5);
		dst_argb[1] = u_expand6(g6);
		dst_argb[2] = u_expand5(r5);
		dst_argb[3] = 255u;
		dst_argb += 4;
		src_rgb565 += 2;
	}
}

void ARGB1555ToARGBRow_C(const uint8_t* src_argb1555,
	uint8_t* dst_argb,
	int width)
{
	int x;
	for (x = 0; x < width; ++x) {
		uint8_t b5 = extract1from1555_top(src_argb1555);
		uint8_t g5 = extract2from1555(src_argb1555);
		uint8_t r5 = extract3from1555(src_argb1555);
		uint8_t a1 = extract4from1555(src_argb1555);
		dst_argb[0] = u_expand5_top(b5);
		dst_argb[1] = u_expand5(g5);
		dst_argb[2] = u_expand5(r5);
		dst_argb[3] = u_expand1(a1);
		dst_argb += 4;
		src_argb1555 += 2;
	}
}

void ARGB4444ToARGBRow_C(const uint8_t* src_argb4444,
	uint8_t* dst_argb,
	int width)
{
	int x;
	for (x = 0; x < width; ++x) {
		uint8_t b4 = extract1from4444_top(src_argb4444);
		uint8_t g4 = extract2from4444(src_argb4444);
		uint8_t r4 = extract3from4444_top(src_argb4444);
		uint8_t a4 = extract4from4444(src_argb4444);
		dst_argb[0] = u_expand4_top(b4);
		dst_argb[1] = u_expand4(g4);
		dst_argb[2] = u_expand4_top(r4);
		dst_argb[3] = u_expand4(a4);
		dst_argb += 4;
		src_argb4444 += 2;
	}
}

// Cxbx color component conversion functions 
void X1R5G5B5ToARGBRow_C(const uint8_t* src_x1r5g5b5, uint8_t* dst_argb,
	int width)
{
	int x;
	for (x = 0; x < width; ++x) {
		uint8_t b5 = extract1from1555_top(src_x1r5g5b5);
		uint8_t g5 = extract2from1555(src_x1r5g5b5);
		uint8_t r5 = extract3from1555(src_x1r5g5b5);
		// Note : X1 is ignored, so no extract4from1555
		dst_argb[0] = u_expand5_top(b5);
		dst_argb[1] = u_expand5(g5);
		dst_argb[2] = u_expand5(r5);
		dst_argb[3] = 255u;
		dst_argb += 4;
		src_x1r5g5b5 += 2;
	}
}

void A8R8G8B8ToARGBRow_C(const uint8_t* src_a8r8g8b8, uint8_t* dst_argb, int width)
{
	memcpy(dst_argb, src_a8r8g8b8, width * sizeof(D3DCOLOR)); // Cxbx pass-through
}

void X8R8G8B8ToARGBRow_C(const uint8_t* src_x8r8g8b8, uint8_t* dst_argb, int width)
{
	int x;
	for (x = 0; x < width; ++x) {
		uint8_t b8 = src_x8r8g8b8[0];
		uint8_t g8 = src_x8r8g8b8[1];
		uint8_t r8 = src_x8r8g8b8[2];
		dst_argb[0] = b8;
		dst_argb[1] = g8;
		dst_argb[2] = r8;
		dst_argb[3] = 255u;
		dst_argb += 4;
		src_x8r8g8b8 += 4;
	}
}

void ____R8B8ToARGBRow_C(const uint8_t* src_r8b8, uint8_t* dst_argb, int width)
{
	int x;
	for (x = 0; x < width; ++x) {
		uint8_t b8 = src_r8b8[0];
		uint8_t r8 = src_r8b8[1];
		dst_argb[0] = b8;
		dst_argb[1] = b8;
		dst_argb[2] = r8;
		dst_argb[3] = r8;
		dst_argb += 4;
		src_r8b8 += 2;
	}
}

void ____G8B8ToARGBRow_C(const uint8_t* src_g8b8, uint8_t* dst_argb, int width)
{
	int x;
	for (x = 0; x < width; ++x) {
		uint8_t b8 = src_g8b8[0];
		uint8_t g8 = src_g8b8[1];
		dst_argb[0] = b8;
		dst_argb[1] = g8;
		dst_argb[2] = b8;
		dst_argb[3] = g8;
		dst_argb += 4;
		src_g8b8 += 2;
	}
}

void ______A8ToARGBRow_C(const uint8_t* src_a8, uint8_t* dst_argb, int width)
{
	int x;
	for (x = 0; x < width; ++x) {
		uint8_t a8 = src_a8[0];
		dst_argb[0] = 255u;
		dst_argb[1] = 255u;
		dst_argb[2] = 255u;
		dst_argb[3] = a8;
		dst_argb += 4;
		src_a8 += 1;
	}
}

void __R6G5B5ToARGBRow_C(const uint8_t* src_r6g5b5, uint8_t* dst_argb, int width)
{
	int x;
	for (x = 0; x < width; ++x) {
		uint8_t b5 = extract1from655_top(src_r6g5b5);
		uint8_t g5 = extract2from655(src_r6g5b5);
		uint8_t r6 = extract3from655(src_r6g5b5);
		dst_argb[0] = u_expand5_top(b5);
		dst_argb[1] = u_expand5(g5);
		dst_argb[2] = u_expand6(r6);
		dst_argb[3] = 255u;
		dst_argb += 4;
		src_r6g5b5 += 2;
	}
}

void __L6V5U5ToARGBRow_C(const uint8_t* src_l6v5u5, uint8_t* dst_argb, int width)
{
	int x;
	for (x = 0; x < width; ++x) {
		// Note : U and V are stored in two's complement (signed) form, but since
		// ARGB doesn't support signed values, we convert the range into unsigned
		// and rely on COLORSIGN support in our pixel shaders to convert it back!
		// However, be aware that this causes different values after normalization!
		// See https://github.com/wine-staging/wine-patched/blob/9ac6333e9bad75d6c23b9de604c71a41c151f65b/dlls/wined3d/utils.c#L699-L729
		// and https://www.khronos.org/opengl/wiki/Normalized_Integer
		uint8_t u5 = (extract1from655(src_l6v5u5) + 0x10) & 0x1f;
		uint8_t v5 = (extract2from655(src_l6v5u5) + 0x10) & 0x1f;
		uint8_t l6 = extract3from655(src_l6v5u5);
		// Note : Countrary to R6G5B5 (which gets converted to B8, G8, R8, A8=255),
		// L6U5V5 does not reverse its components (and becomes L8, V8, U8, A8=255)!
		dst_argb[0] = u_expand6(l6);
		dst_argb[1] = u_expand5(v5);
		dst_argb[2] = u_expand5(u5);
		dst_argb[3] = 255u;
		dst_argb += 4;
		src_l6v5u5 += 2;
	}
}

void __L6V5U5ToX8L8V8U8Row_C(const uint8_t* src_l6v5u5, uint8_t* dst_xlvu, int width)
{
	int x;
	for (x = 0; x < width; ++x) {
		uint8_t u5 = extract1from655(src_l6v5u5);
		uint8_t v5 = extract2from655(src_l6v5u5);
		uint8_t l6 = extract3from655(src_l6v5u5);
		dst_xlvu[0] = s_expand5(u5);
		dst_xlvu[1] = s_expand5(v5);
		dst_xlvu[2] = u_expand6(l6);
		dst_xlvu[3] = 255u;
		dst_xlvu += 4;
		src_l6v5u5 += 2;
	}
}

void R5G5B5A1ToARGBRow_C(const uint8_t* src_r5g5b5a1, uint8_t* dst_argb, int width)
{
	int x;
	for (x = 0; x < width; ++x) {
		uint8_t a1 = extract1from5551(src_r5g5b5a1);
		uint8_t b5 = extract2from5551(src_r5g5b5a1);
		uint8_t g5 = extract3from5551(src_r5g5b5a1);
		uint8_t r5 = extract4from5551(src_r5g5b5a1);
		dst_argb[0] = u_expand5(b5);
		dst_argb[1] = u_expand5(g5);
		dst_argb[2] = u_expand5(r5);
		dst_argb[3] = u_expand1(a1);
		dst_argb += 4;
		src_r5g5b5a1 += 2;
	}
}

void R4G4B4A4ToARGBRow_C(const uint8_t* src_r4g4b4a4, uint8_t* dst_argb, int width)
{
	int x;
	for (x = 0; x < width; ++x) {
		uint8_t a4 = extract1from4444_top(src_r4g4b4a4);
		uint8_t b4 = extract2from4444(src_r4g4b4a4);
		uint8_t g4 = extract3from4444_top(src_r4g4b4a4);
		uint8_t r4 = extract4from4444(src_r4g4b4a4);
		dst_argb[0] = u_expand4(b4);
		dst_argb[1] = u_expand4_top(g4);
		dst_argb[2] = u_expand4(r4);
		dst_argb[3] = u_expand4_top(a4);
		dst_argb += 4;
		src_r4g4b4a4 += 2;
	}
}

void A8B8G8R8ToARGBRow_C(const uint8_t* src_a8b8g8r8, uint8_t* dst_argb, int width)
{
	int x;
	for (x = 0; x < width; ++x) {
		uint8_t r8 = src_a8b8g8r8[0];
		uint8_t g8 = src_a8b8g8r8[1];
		uint8_t b8 = src_a8b8g8r8[2];
		uint8_t a8 = src_a8b8g8r8[3];
		dst_argb[0] = b8;
		dst_argb[1] = g8;
		dst_argb[2] = r8;
		dst_argb[3] = a8;
		dst_argb += 4;
		src_a8b8g8r8 += 4;
	}
}

void B8G8R8A8ToARGBRow_C(const uint8_t* src_b8g8r8a8, uint8_t* dst_argb, int width)
{
	int x;
	for (x = 0; x < width; ++x) {
		uint8_t a8 = src_b8g8r8a8[0];
		uint8_t r8 = src_b8g8r8a8[1];
		uint8_t g8 = src_b8g8r8a8[2];
		uint8_t b8 = src_b8g8r8a8[3];
		dst_argb[0] = b8;
		dst_argb[1] = g8;
		dst_argb[2] = r8;
		dst_argb[3] = a8;
		dst_argb += 4;
		src_b8g8r8a8 += 4;
	}
}

void R8G8B8A8ToARGBRow_C(const uint8_t* src_r8g8b8a8, uint8_t* dst_argb, int width)
{
	int x;
	for (x = 0; x < width; ++x) {
		uint8_t a8 = src_r8g8b8a8[0];
		uint8_t b8 = src_r8g8b8a8[1];
		uint8_t g8 = src_r8g8b8a8[2];
		uint8_t r8 = src_r8g8b8a8[3];
		dst_argb[0] = b8;
		dst_argb[1] = g8;
		dst_argb[2] = r8;
		dst_argb[3] = a8;
		dst_argb += 4;
		src_r8g8b8a8 += 4;
	}
}

void ______L8ToARGBRow_C(const uint8_t* src_l8, uint8_t* dst_argb, int width)
{
	int x;
	for (x = 0; x < width; ++x) {
		uint8_t l8 = src_l8[0];
		dst_argb[0] = l8;
		dst_argb[1] = l8;
		dst_argb[2] = l8;
		dst_argb[3] = 255u;
		dst_argb += 4;
		src_l8 += 1;
	}
}

void _____AL8ToARGBRow_C(const uint8_t* src_al8, uint8_t* dst_argb, int width)
{
	int x;
	for (x = 0; x < width; ++x) {
		uint8_t l8 = src_al8[0];
		dst_argb[0] = l8;
		dst_argb[1] = l8;
		dst_argb[2] = l8;
		dst_argb[3] = l8;
		dst_argb += 4;
		src_al8 += 1;
	}
}

void _____L16ToARGBRow_C(const uint8_t* src_l16, uint8_t* dst_argb, int width)
{
	int x;
	for (x = 0; x < width; ++x) {
		uint8_t b8 = src_l16[0];
		uint8_t g8 = src_l16[1];
		dst_argb[0] = b8;
		dst_argb[1] = g8;
		dst_argb[2] = 255u;
		dst_argb[3] = 255u;
		dst_argb += 4;
		src_l16 += 2;
	}
}

void ____A8L8ToARGBRow_C(const uint8_t* src_a8l8, uint8_t* dst_argb, int width)
{
	int x;
	for (x = 0; x < width; ++x) {
		uint8_t l8 = src_a8l8[0];
		uint8_t a8 = src_a8l8[1];
		dst_argb[0] = l8;
		dst_argb[1] = l8;
		dst_argb[2] = l8;
		dst_argb[3] = a8;
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
void ____DXT1ToARGBRow_C(const uint8_t* data, uint8_t* dst_argb, int width)
{
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
		colormap[0].B = u_expand5(b0);
		colormap[0].G = u_expand6(g0);
		colormap[0].R = u_expand5(r0);
		colormap[0].A = 255u;

		colormap[1].B = u_expand5(b1);
		colormap[1].G = u_expand6(g1);
		colormap[1].R = u_expand5(r1);
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
void ____DXT3ToARGBRow_C(const uint8_t* data, uint8_t* dst_argb, int width)
{
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
		colormap[0].B = u_expand5(b0);
		colormap[0].G = u_expand6(g0);
		colormap[0].R = u_expand5(r0);

		colormap[1].B = u_expand5(b1);
		colormap[1].G = u_expand6(g1);
		colormap[1].R = u_expand5(r1);

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
void ____DXT5ToARGBRow_C(const uint8_t* data, uint8_t* dst_argb, int width)
{
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
		colormap[0].B = u_expand5(b0);
		colormap[0].G = u_expand6(g0);
		colormap[0].R = u_expand5(r0);

		colormap[1].B = u_expand5(b1);
		colormap[1].G = u_expand6(g1);
		colormap[1].R = u_expand5(r1);

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

void ______P8ToARGBRow_C(const uint8_t* src_p8, uint8_t* dst_argb, int width)
{
	TRGB32 *pTexturePalette = *(TRGB32 **)dst_argb; // dirty hack to avoid another argument
	int x;
	for (x = 0; x < width; ++x) {
		uint8_t p = src_p8[x];
		TRGB32 color = pTexturePalette[p];
		((TRGB32 *)dst_argb)[x] = color;
	}
}

static __inline int32_t clamp0(int32_t v)
{
	return ((-(v) >> 31) & (v));
}

static __inline int32_t clamp255(int32_t v)
{
	return (((255 - (v)) >> 31) | (v)) & 255;
}

static __inline uint32_t Clamp(int32_t val)
{
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
	int width)
{
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

const FormatToARGBRow ComponentConverters[] = {
	nullptr,             // NoCmpnts,
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
	__L6V5U5ToARGBRow_C, // __L6V5U5,
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
