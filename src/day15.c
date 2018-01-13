#include "advent2017.h"

typedef uint64_t v4di __attribute__ ((vector_size (32)));

static const uint64_t factor_a = 16807;
static const v4di factor_a4 = { 984943658, 984943658, 984943658, 984943658};

static const uint64_t factor_b = 48271;
static const v4di factor_b4 = {1914720637,1914720637,1914720637,1914720637};

// (x * factor) % 2147483648
static inline uint64_t generate(uint64_t x, uint64_t factor)
{
	x *= factor;
	x =  (x >> 31) + (x & 0x7fffffff);
	x = ((x >> 31) + x) & 0x7fffffff;
	return x;
}

// 4-way SIMD version
static inline v4di generate4(v4di v, v4di factor)
{
	v = __builtin_ia32_pmuludq256(v, factor);
	v =  (v >> 31) + (v & 0x7fffffff);
	v = ((v >> 31) + v) & 0x7fffffff;
	return v;
}

void day15(input_t in)
{
	uint64_t a0 = 0, b0 = 0, part1, part2 = 0;

	for (; in.len--; in.s++) {
		if (*in.s >= '0' && *in.s <= '9') {
			b0 = b0 * 10 + *in.s - '0';
		} else if (!a0) {
			a0 = b0;
			b0 = 0;
		}
	}

	// Initialize vectors with the first four generations
	v4di va0 = {a0,0,0,0}, vb0 = {b0,0,0,0};
	for (int i = 1; i < 4; i++) {
		va0[i] = generate(va0[i - 1], factor_a);
		vb0[i] = generate(vb0[i - 1], factor_b);
	}

	// Part 1
	v4di va = va0, vb = vb0;
	v4di part1v = {0,0,0,0};
	for (size_t r = 10000000L; r--; ) {
		// Subtract because vector comparisons return -1 on true
		part1v -= (va & 0xffff) == (vb & 0xffff);
		va = generate4(va, factor_a4);
		vb = generate4(vb, factor_b4);
	}
	part1 = part1v[0] + part1v[1] + part1v[2] + part1v[3];

	// Part 2
	va = va0, vb = vb0;
	int mask_a = __builtin_ia32_movmskpd256((va & 3) == 0);
	int mask_b = __builtin_ia32_movmskpd256((vb & 7) == 0);
	for (size_t r = 5000000L; r--; ) {
		// Generate until both vectors have at least one valid value
		while (!mask_a) {
			va = generate4(va, factor_a4);
			mask_a = __builtin_ia32_movmskpd256((va & 3) == 0);
		}
		while (!mask_b) {
			vb = generate4(vb, factor_b4);
			mask_b = __builtin_ia32_movmskpd256((vb & 7) == 0);
		}

		// Find the index of first valid value
		int idx_a = __builtin_ffs(mask_a) - 1;
		int idx_b = __builtin_ffs(mask_b) - 1;

		// Clear the mask bits corresponding to the found values
		mask_a ^= 1 << idx_a;
		mask_b ^= 1 << idx_b;

		// Increment part2 if the lower 16 bits match
		part2 += (va[idx_a] & 0xffff) == (vb[idx_b] & 0xffff);
	}

	printf("Day 15 Part 1: %lu\n", part1);
	printf("Day 15 Part 2: %lu\n", part2);
}
