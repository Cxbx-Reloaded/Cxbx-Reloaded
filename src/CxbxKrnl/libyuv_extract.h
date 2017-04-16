// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Cxbx.h"

typedef int08 int8;
typedef uint08 uint8;

// From libyuv\include\libyuv\basic_types.h :
#define LIBYUV_API

LIBYUV_API
int YUY2ToARGB(const uint8* src_yuy2,
	int src_stride_yuy2,
	uint8* dst_argb,
	int dst_stride_argb,
	int width,
	int height);