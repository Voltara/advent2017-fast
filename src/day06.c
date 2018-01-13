#include "advent2017.h"

typedef uint8_t v16qi __attribute__ ((vector_size (16)));

// Because what is good is not always what is interesting
static inline uint64_t max_nibble(uint64_t x, int *shift)
{
	const v16qi p0 = {7,15,6,14,5,13,4,12,3,11,2,10,1,9,0,8};
	const v16qi p1 = {15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0};
	const v16qi p2 = {3,6,4,0,2,7,1,5,5,1,7,2,0,4,6,3};
	const v16qi p3 = {2,5,0,2,0,1,5,1,1,5,1,0,2,0,5,2};

	union {
		uint64_t u[2];
		v16qi v;
	} u = { .u = { x >> 4, x } };

	u.v &= 0xf;
	v16qi v0 = __builtin_ia32_pshufb128(u.v, p0);
	u.v = __builtin_ia32_pmaxub128(u.v, v0);
	u.v = __builtin_ia32_pmaxub128(u.v, __builtin_ia32_pshufb128(u.v, p1));
	u.v = __builtin_ia32_pmaxub128(u.v, __builtin_ia32_pshufb128(u.v, p2));
	u.v = __builtin_ia32_pmaxub128(u.v, __builtin_ia32_pshufb128(u.v, p3));

	*shift = 4 * (16 - __builtin_ffs(__builtin_ia32_pmovmskb128(u.v == v0)));

	return u.v[0];
}

/* The state of the memory banks is stored as 16 4-bit integers
 * packed into a uint64_t.  It is possible for a puzzle input to
 * overflow a 4-bit counter; this does not happen with my input.
 * (See previous comment.)
 */
void day06(input_t in)
{
	const size_t sz = 8191;
	struct {
		uint64_t state;
		uint64_t step;
	} *h = calloc(sz, sizeof(*h));

	uint64_t x = 0;
	for (uint64_t n = 0, i = 0; in.len--; in.s++) {
		if (*in.s < '0') {
			x = (x << 4) | n;
			n = 0;
		} else {
			n = n * 10 + *in.s - '0';
		}
	}

	uint64_t part1 = 0, part2 = 0;
	for (size_t step = 0; ; step++) {
		// Check if this state repeats a previous state
		size_t idx = x % sz;
		while (h[idx].state) {
			if (h[idx].state == x) {
				part2 = (part1 = step) - h[idx].step;
				goto done;
			}
			if (++idx == sz) idx = 0;
		}
		h[idx].state = x;
		h[idx].step = step;

		// Get max value and its address
		int shift;
		uint64_t max = max_nibble(x, &shift);

		// Split the max value up into an equivalent number of ones
		uint64_t add = -(1UL << 64 - max * 4) & 0x1111111111111111L;

		// Rotate the ones into position
		add = (add << shift) | (add >> 64 - shift);

		// Redistribute (subtract max value, add ones)
		x += add - (max << shift);
	}

done:
	free(h);

	printf("Day 06 Part 1: %lu\n", part1);
	printf("Day 06 Part 2: %lu\n", part2);
}
