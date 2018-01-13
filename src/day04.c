#include "advent2017.h"

// Sorts the octets of a 64-bit integer in 38 instructions
static uint64_t bitonic_sort(uint64_t x)
{
	typedef char v8qi __attribute__ ((vector_size (8)));

	// Permutations for an 8-input bitonic sort network
	const v8qi pAABBCCDD = {1,0,3,2,5,4,7,6};
	const v8qi pABBACDDC = {3,2,1,0,7,6,5,4};
	const v8qi pABCDDCBA = {7,6,5,4,3,2,1,0};
	const v8qi pABABCDCD = {2,3,0,1,6,7,4,5};

	// Masks for complementing to reverse the sort order of selected
	// octets (avoids having to do a "max" in addition to "min")
	const v8qi m0F = {0,0,0,0,255,255,255,255};
	const v8qi m33 = {0,0,255,255,0,0,255,255};
	const v8qi m3C = {0,0,255,255,255,255,0,0};
	const v8qi m55 = {0,255,0,255,0,255,0,255};
	const v8qi m5A = {0,255,0,255,255,0,255,0};
	const v8qi m66 = {0,255,255,0,0,255,255,0};
	const v8qi m99 = {255,0,0,255,255,0,0,255};
	const v8qi mA5 = {255,0,255,0,0,255,0,255};

	v8qi v = *(v8qi *) &x;
	v = __builtin_ia32_pminub(__builtin_ia32_pxor(m55, v),
		__builtin_ia32_pxor(m55, __builtin_ia32_pshufb(v, pAABBCCDD)));
	v = __builtin_ia32_pminub(__builtin_ia32_pxor(m66, v),
		__builtin_ia32_pxor(m99, __builtin_ia32_pshufb(v, pABBACDDC)));
	v = __builtin_ia32_pminub(__builtin_ia32_pxor(m66, v),
		__builtin_ia32_pxor(m66, __builtin_ia32_pshufb(v, pAABBCCDD)));
	v = __builtin_ia32_pminub(__builtin_ia32_pxor(m5A, v),
		__builtin_ia32_pxor(mA5, __builtin_ia32_pshufb(v, pABCDDCBA)));
	v = __builtin_ia32_pminub(__builtin_ia32_pxor(m3C, v),
		__builtin_ia32_pxor(m3C, __builtin_ia32_pshufb(v, pABABCDCD)));
	v = __builtin_ia32_pminub(__builtin_ia32_pxor(m66, v),
		__builtin_ia32_pxor(m66, __builtin_ia32_pshufb(v, pAABBCCDD)));
	v = __builtin_ia32_pxor(m55, v);

	return *(uint64_t *) &v;
}

void day04(input_t in)
{
	uint64_t *a = calloc(in.len, sizeof(*a)), *e = a;

	uint64_t part1 = 0, part2 = 0, valid1 = 1, valid2 = 1, n = 0;
	for (; in.len--; in.s++) {
		if (*in.s >= 'a') {
			n = (n << 8) | *in.s;
		} else {
			if (valid2) {
				if (valid1) {
					for (uint64_t *p = a; p != e; p += 2) {
						valid1 &= n != *p;
					}
				}
				n = bitonic_sort(*e++ = n);
				for (uint64_t *p = a + 1; p != e; p += 2) {
					valid2 &= n != *p;
				}
				*e++ = n;
			}

			n = 0;

			if (*in.s == '\n') {
				// End of line
				part1 += valid1;
				part2 += valid2;
				valid1 = valid2 = 1;
				e = a;
			}
		}
	}

	free(a);

	printf("Day 04 Part 1: %lu\n", part1);
	printf("Day 04 Part 2: %lu\n", part2);
}
