// https://stackoverflow.com/questions/1666093/cpuid-implementations-in-c

#ifndef CPUID_H
#define CPUID_H

#ifdef _WIN32
#include <limits.h>
#include <intrin.h>
#include <bitset>
typedef unsigned __int32  uint32_t;

#else
#include <stdint.h>
#endif

class CPUID {
	uint32_t regs[4];

public:
	explicit CPUID(unsigned i) {
#ifdef _WIN32
		__cpuid((int *)regs, (int)i);

#else
		asm volatile
			("cpuid" : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
				: "a" (i), "c" (0));
		// ECX is set to zero for CPUID function 4
#endif
	}

	const std::bitset<32> &EAX() const { return (std::bitset<32> &)regs[0]; }
	const std::bitset<32> &EBX() const { return (std::bitset<32> &)regs[1]; }
	const std::bitset<32> &ECX() const { return (std::bitset<32> &)regs[2]; }
	const std::bitset<32> &EDX() const { return (std::bitset<32> &)regs[3]; }
};

class SimdCaps {

public:
	const bool SSE(void) { return f_1.EDX()[25]; }
	const bool SSE2(void) { return f_1.EDX()[26]; }
	const bool SSE3(void) { return f_1.ECX()[0]; }
	const bool SSSE3(void) { return f_1.ECX()[9]; }
	const bool SSE41(void) { return f_1.ECX()[19]; }
	const bool SSE42(void) { return f_1.ECX()[20]; }
	const bool AVX(void) { return f_1.ECX()[1]; }
	const bool AVX2(void) { return f_7.EBX()[5]; }

private:
	const CPUID f_1 = CPUID(1);
	const CPUID f_7 = CPUID(7);
};

static SimdCaps bob;

#endif // CPUID_H
