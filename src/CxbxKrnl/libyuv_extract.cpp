// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "libyuv_extract.h"

// The following code is an extract of libyuv to keep code size low (this may be revisited later).
// Source : https://github.com/lemenkov/libyuv/commit/7e936044d154b9fe159a67f9562e10b1ef1cb590

/* From libyuv\README.chromium :
Name: libyuv
URL: http://code.google.com/p/libyuv/
Version: 1514
License: BSD
License File: LICENSE

Description:
libyuv is an open source project that includes YUV conversion and scaling functionality.
*/

// From libyuv\include\libyuv\row.h :

// This struct is for Intel color conversion.
struct YuvConstants {
	int8 kUVToB[32];
	int8 kUVToG[32];
	int8 kUVToR[32];
	int16 kUVBiasB[16];
	int16 kUVBiasG[16];
	int16 kUVBiasR[16];
	int16 kYToRgb[16];
};

// From libyuv\include\libyuv\row.h :

#if defined(VISUALC_HAS_AVX2)
#define SIMD_ALIGNED(var) __declspec(align(32)) var
#else
#define SIMD_ALIGNED(var) __declspec(align(16)) var
#endif

// From libyuv\source\row_common.cc :

// llvm x86 is poor at ternary operator, so use branchless min/max.
#define USE_BRANCHLESS 1
#if USE_BRANCHLESS
static __inline int32 clamp0(int32 v) {
	return ((-(v) >> 31) & (v));
}

static __inline int32 clamp255(int32 v) {
	return (((255 - (v)) >> 31) | (v)) & 255;
}

static __inline uint32 Clamp(int32 val) {
	int v = clamp0(val);
	return (uint32)(clamp255(v));
}

static __inline uint32 Abs(int32 v) {
	int m = v >> 31;
	return (v + m) ^ m;
}
#else   // USE_BRANCHLESS
static __inline int32 clamp0(int32 v) {
	return (v < 0) ? 0 : v;
}

static __inline int32 clamp255(int32 v) {
	return (v > 255) ? 255 : v;
}

static __inline uint32 Clamp(int32 val) {
	int v = clamp0(val);
	return (uint32)(clamp255(v));
}

static __inline uint32 Abs(int32 v) {
	return (v < 0) ? -v : v;
}
#endif  // USE_BRANCHLESS

// From libyuv\source\row_common.cc :

// BT.601 YUV to RGB reference
//  R = (Y - 16) * 1.164              - V * -1.596
//  G = (Y - 16) * 1.164 - U *  0.391 - V *  0.813
//  B = (Y - 16) * 1.164 - U * -2.018

// Y contribution to R,G,B.  Scale and bias.
#define YG 18997  /* round(1.164 * 64 * 256 * 256 / 257) */
#define YGB -1160 /* 1.164 * 64 * -16 + 64 / 2 */

// U and V contributions to R,G,B.
#define UB -128 /* max(-128, round(-2.018 * 64)) */
#define UG 25   /* round(0.391 * 64) */
#define VG 52   /* round(0.813 * 64) */
#define VR -102 /* round(-1.596 * 64) */

// Bias values to subtract 16 from Y and 128 from U and V.
#define BB (UB * 128 + YGB)
#define BG (UG * 128 + VG * 128 + YGB)
#define BR (VR * 128 + YGB)

const struct YuvConstants SIMD_ALIGNED(kYuvI601Constants) = {
	{ UB, 0, UB, 0, UB, 0, UB, 0, UB, 0, UB, 0, UB, 0, UB, 0,
	UB, 0, UB, 0, UB, 0, UB, 0, UB, 0, UB, 0, UB, 0, UB, 0 },
	{ UG, VG, UG, VG, UG, VG, UG, VG, UG, VG, UG, VG, UG, VG, UG, VG,
	UG, VG, UG, VG, UG, VG, UG, VG, UG, VG, UG, VG, UG, VG, UG, VG },
	{ 0, VR, 0, VR, 0, VR, 0, VR, 0, VR, 0, VR, 0, VR, 0, VR,
	0, VR, 0, VR, 0, VR, 0, VR, 0, VR, 0, VR, 0, VR, 0, VR },
	{ BB, BB, BB, BB, BB, BB, BB, BB, BB, BB, BB, BB, BB, BB, BB, BB },
	{ BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG, BG },
	{ BR, BR, BR, BR, BR, BR, BR, BR, BR, BR, BR, BR, BR, BR, BR, BR },
	{ YG, YG, YG, YG, YG, YG, YG, YG, YG, YG, YG, YG, YG, YG, YG, YG } };

// C reference code that mimics the YUV assembly.
static __inline void YuvPixel(uint8 y,
	uint8 u,
	uint8 v,
	uint8* b,
	uint8* g,
	uint8* r,
	const struct YuvConstants* yuvconstants) {
	int ub = yuvconstants->kUVToB[0];
	int ug = yuvconstants->kUVToG[0];
	int vg = yuvconstants->kUVToG[1];
	int vr = yuvconstants->kUVToR[1];
	int bb = yuvconstants->kUVBiasB[0];
	int bg = yuvconstants->kUVBiasG[0];
	int br = yuvconstants->kUVBiasR[0];
	int yg = yuvconstants->kYToRgb[0];

	uint32 y1 = (uint32)(y * 0x0101 * yg) >> 16;
	*b = Clamp((int32)(-(u * ub) + y1 + bb) >> 6);
	*g = Clamp((int32)(-(u * ug + v * vg) + y1 + bg) >> 6);
	*r = Clamp((int32)(-(v * vr) + y1 + br) >> 6);
}

void YUY2ToARGBRow_C(const uint8* src_yuy2,
	uint8* rgb_buf,
	const struct YuvConstants* yuvconstants,
	int width) {
	int x;
	for (x = 0; x < width - 1; x += 2) {
		YuvPixel(src_yuy2[0], src_yuy2[1], src_yuy2[3], rgb_buf + 0, rgb_buf + 1,
			rgb_buf + 2, yuvconstants);
		rgb_buf[3] = 255;
		YuvPixel(src_yuy2[2], src_yuy2[1], src_yuy2[3], rgb_buf + 4, rgb_buf + 5,
			rgb_buf + 6, yuvconstants);
		rgb_buf[7] = 255;
		src_yuy2 += 4;
		rgb_buf += 8;  // Advance 2 pixels.
	}
	if (width & 1) {
		YuvPixel(src_yuy2[0], src_yuy2[1], src_yuy2[3], rgb_buf + 0, rgb_buf + 1,
			rgb_buf + 2, yuvconstants);
		rgb_buf[3] = 255;
	}
}

// Convert YUY2 to ARGB.
LIBYUV_API
int YUY2ToARGB(const uint8* src_yuy2,
	int src_stride_yuy2,
	uint8* dst_argb,
	int dst_stride_argb,
	int width,
	int height) {
	int y;
	void(*YUY2ToARGBRow)(const uint8* src_yuy2, uint8* dst_argb,
		const struct YuvConstants* yuvconstants, int width) =
		YUY2ToARGBRow_C;
	if (!src_yuy2 || !dst_argb || width <= 0 || height == 0) {
		return -1;
	}
	// Negative height means invert the image.
	if (height < 0) {
		height = -height;
		src_yuy2 = src_yuy2 + (height - 1) * src_stride_yuy2;
		src_stride_yuy2 = -src_stride_yuy2;
	}
	// Coalesce rows.
	if (src_stride_yuy2 == width * 2 && dst_stride_argb == width * 4) {
		width *= height;
		height = 1;
		src_stride_yuy2 = dst_stride_argb = 0;
	}
#if defined(HAS_YUY2TOARGBROW_SSSE3)
	if (TestCpuFlag(kCpuHasSSSE3)) {
		YUY2ToARGBRow = YUY2ToARGBRow_Any_SSSE3;
		if (IS_ALIGNED(width, 16)) {
			YUY2ToARGBRow = YUY2ToARGBRow_SSSE3;
		}
	}
#endif
#if defined(HAS_YUY2TOARGBROW_AVX2)
	if (TestCpuFlag(kCpuHasAVX2)) {
		YUY2ToARGBRow = YUY2ToARGBRow_Any_AVX2;
		if (IS_ALIGNED(width, 32)) {
			YUY2ToARGBRow = YUY2ToARGBRow_AVX2;
		}
	}
#endif
#if defined(HAS_YUY2TOARGBROW_NEON)
	if (TestCpuFlag(kCpuHasNEON)) {
		YUY2ToARGBRow = YUY2ToARGBRow_Any_NEON;
		if (IS_ALIGNED(width, 8)) {
			YUY2ToARGBRow = YUY2ToARGBRow_NEON;
		}
	}
#endif
#if defined(HAS_YUY2TOARGBROW_MSA)
	if (TestCpuFlag(kCpuHasMSA)) {
		YUY2ToARGBRow = YUY2ToARGBRow_Any_MSA;
		if (IS_ALIGNED(width, 8)) {
			YUY2ToARGBRow = YUY2ToARGBRow_MSA;
		}
	}
#endif
	for (y = 0; y < height; ++y) {
		YUY2ToARGBRow(src_yuy2, dst_argb, &kYuvI601Constants, width);
		src_yuy2 += src_stride_yuy2;
		dst_argb += dst_stride_argb;
	}
	return 0;
}
