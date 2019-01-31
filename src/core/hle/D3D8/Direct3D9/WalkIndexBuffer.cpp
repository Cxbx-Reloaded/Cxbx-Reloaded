//#include <mmintrin.h> // - MMX
//#include <xmmintrin.h> // SSE
//#include <emmintrin.h> // SSE2
//#include <pmmintrin.h> // SSE3
#include <smmintrin.h> // SSE4.1
//#include <nmmintrin.h> // SSE4.2
//#include <immintrin.h> // AVX

#include "core\kernel\support\Emu.h"
#include "core\kernel\support\EmuXTL.h"

#include "common\util\CPUID.h"
#include "WalkIndexBuffer.h"

void WalkIndexBuffer_SSE41(XTL::INDEX16 & LowIndex, XTL::INDEX16 & HighIndex, XTL::INDEX16 * pIndexData, DWORD dwIndexCount);

void Init_SIMD
(
	XTL::INDEX16 &LowIndex,
	XTL::INDEX16 &HighIndex,
	XTL::INDEX16 *pIndexData,
	DWORD dwIndexCount
)
{
	SimdCaps supports;
	if (supports.SSE41())
		WalkIndexBuffer_SIMD = WalkIndexBuffer_SSE41;
	else
		WalkIndexBuffer_SIMD = WalkIndexBuffer;

	WalkIndexBuffer_SIMD(LowIndex, HighIndex, pIndexData, dwIndexCount);
}

void WalkIndexBuffer(XTL::INDEX16 & LowIndex, XTL::INDEX16 & HighIndex, XTL::INDEX16 * pIndexData, DWORD dwIndexCount)
{
	// Determine highest and lowest index in use 
	LowIndex = pIndexData[0];
	HighIndex = LowIndex;
	for (uint i = 1; i < dwIndexCount; i++) {
		XTL::INDEX16 Index = pIndexData[i];
		if (LowIndex > Index)
			LowIndex = Index;
		if (HighIndex < Index)
			HighIndex = Index;
	}
}

void WalkIndexBuffer_SSE41(XTL::INDEX16 & LowIndex, XTL::INDEX16 & HighIndex, XTL::INDEX16 * pIndexData, DWORD dwIndexCount)
{
	// We can fit 8 ushorts into 128 bit SIMD registers
	int iterations = dwIndexCount / 8;
	DWORD remainder = dwIndexCount % 8;

	// Fallback to basic function if we can't even min / max 2 registers together
	if (iterations < 2) {
		WalkIndexBuffer(LowIndex, HighIndex, pIndexData, dwIndexCount);
		return;
	}

	__m128i *unalignedIndices = (__m128i*) pIndexData;
	__m128i min = _mm_set1_epi16(USHRT_MAX);
	__m128i max = _mm_setzero_si128();

	// Min / max over index data
	for (int i = 0; i < iterations; i++) {
		__m128i indices = _mm_loadu_si128(&unalignedIndices[i]);
		min = _mm_min_epu16(indices, min);
		max = _mm_max_epu16(indices, max);
	}

	// horizontal min
	min = _mm_minpos_epu16(min);

	// horizontal max (using minpos)
	max = _mm_subs_epu16(_mm_set1_epi16(USHRT_MAX), max); //invert
	max = _mm_minpos_epu16(max);

	// Get the min and max out
	LowIndex = (XTL::INDEX16) _mm_cvtsi128_si32(min);
	HighIndex = (XTL::INDEX16) USHRT_MAX - _mm_cvtsi128_si32(max);

	// Compare with the remaining values that didn't fit neatly into the SIMD registers
	for (DWORD i = dwIndexCount - remainder; i < dwIndexCount; i++) {
		if (pIndexData[i] < LowIndex)
			LowIndex = pIndexData[i];
		else if (pIndexData[i] > HighIndex)
			HighIndex = pIndexData[i];

	}
}

// TODO AVX2, AVX512
