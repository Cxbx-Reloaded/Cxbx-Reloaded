#ifndef WALKINDEXBUFFER_H
#define WALKINDEXBUFFER_H

#include "core\kernel\support\Emu.h"
#include "core\kernel\support\EmuXTL.h"

void WalkIndexBuffer
(
	XTL::INDEX16 &LowIndex,
	XTL::INDEX16 &HighIndex,
	XTL::INDEX16 *pIndexData,
	DWORD dwIndexCount
);


void Init_SIMD
(
	XTL::INDEX16 &LowIndex,
	XTL::INDEX16 &HighIndex,
	XTL::INDEX16 *pIndexData,
	DWORD dwIndexCount
);

static void(*WalkIndexBuffer_SIMD)
(
	XTL::INDEX16 &LowIndex,
	XTL::INDEX16 &HighIndex,
	XTL::INDEX16 *pIndexData,
	DWORD dwIndexCount
) = Init_SIMD;


#endif
