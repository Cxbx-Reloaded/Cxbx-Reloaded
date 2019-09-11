//#include <mmintrin.h> // - MMX
//#include <xmmintrin.h> // SSE
//#include <emmintrin.h> // SSE2
//#include <pmmintrin.h> // SSE3
#include <smmintrin.h> // SSE4.1
//#include <nmmintrin.h> // SSE4.2
//#include <immintrin.h> // AVX
#include "common\util\CPUID.h"
#include "WalkIndexBuffer.h"

// Walk an index buffer to find the minimum and maximum indices

// Default implementation
void WalkIndexBuffer_NoSIMD(INDEX16 & LowIndex, INDEX16 & HighIndex, INDEX16 * pIndexData, DWORD dwIndexCount)
{
	// Determine highest and lowest index in use 
	LowIndex = pIndexData[0];
	HighIndex = LowIndex;
	for (unsigned int i = 1; i < dwIndexCount; i++) {
		INDEX16 Index = pIndexData[i];
		if (LowIndex > Index)
			LowIndex = Index;
		if (HighIndex < Index)
			HighIndex = Index;
	}
}

//SSE 4.1 implementation
void WalkIndexBuffer_SSE41(INDEX16 & LowIndex, INDEX16 & HighIndex, INDEX16 * pIndexData, DWORD dwIndexCount)
{
	// We can fit 8 ushorts into 128 bit SIMD registers
	int iterations = dwIndexCount / 8;
	DWORD remainder = dwIndexCount % 8;

	// Fallback to basic function if we can't even min / max 2 registers together
	if (iterations < 2) {
		WalkIndexBuffer_NoSIMD(LowIndex, HighIndex, pIndexData, dwIndexCount);
		return;
	}

	// Initialize mins and maxes
	__m128i *unalignedIndices = (__m128i*) pIndexData;
	__m128i min = _mm_set1_epi16((short)(USHRT_MAX)); // cast as set1 only takes signed shorts
	__m128i max = _mm_setzero_si128();

	// Min / max over index data
	for (int i = 0; i < iterations; i++) {
		__m128i indices = _mm_loadu_si128(&unalignedIndices[i]);
		min = _mm_min_epu16(indices, min);
		max = _mm_max_epu16(indices, max);
	}

	// horizontal min
	min = _mm_minpos_epu16(min);

	// horizontal max (no maxpos, we invert and use minpos)
	max = _mm_subs_epu16(_mm_set1_epi16((short)(USHRT_MAX)), max); //invert
	max = _mm_minpos_epu16(max);

	// Get the min and max out
	LowIndex = (INDEX16) _mm_cvtsi128_si32(min);
	HighIndex = (INDEX16) USHRT_MAX - _mm_cvtsi128_si32(max); // invert back

	// Compare with the remaining values that didn't fit neatly into the SIMD registers
	for (DWORD i = dwIndexCount - remainder; i < dwIndexCount; i++) {
		if (pIndexData[i] < LowIndex)
			LowIndex = pIndexData[i];
		else if (pIndexData[i] > HighIndex)
			HighIndex = pIndexData[i];

	}
}

// TODO AVX2, AVX512 implementations

// Detect SSE support to select real implementation on first call
void(*WalkIndexBuffer)(INDEX16 &, INDEX16 &, INDEX16 *, DWORD) =
[](INDEX16 &LowIndex, INDEX16 &HighIndex, INDEX16 *pIndexData, DWORD dwIndexCount)
{
	SimdCaps supports;
	if (supports.SSE41())
		WalkIndexBuffer = WalkIndexBuffer_SSE41;
	else
		WalkIndexBuffer = WalkIndexBuffer_NoSIMD;

	WalkIndexBuffer(LowIndex, HighIndex, pIndexData, dwIndexCount);
};
