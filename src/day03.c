#include "advent2017.h"

// Integer square root
static inline uint64_t isqrt(uint64_t x)
{
	uint64_t root = 0;
	uint64_t one = 1UL << 62 - (__builtin_clzl(x) & ~1);
	for (; one; one /= 4, root /= 2) {
		uint64_t square = root + one;
		if (square <= x) {
			root = square + one;
			x -= square;
		}
	}
	return root;
}

static inline uint64_t part1(uint64_t x)
{
	if (--x) {
		uint64_t ring = (isqrt(x) + 1) / 2;
		return ring + labs(x % (ring * 2) - ring);
	}
	return 0;
}

static inline uint64_t part2(uint64_t x)
{
	// 512 is more than enough to overflow a 64-bit integer
	uint64_t a[512] = {1,1,1,1,1,1,1,2,2};
	/* p = pointer to cell in current ring
	 * q = pointer to cell in next outer ring
	 * side = which side of the square currently
	 */
	uint64_t *p = a + 1, *q = a + 9, side = 0;
	for (uint64_t length = 2; ; length += 2) {
		goto first_side;
		while (++side & 3) {
			// Turn the corner
			(++q)[1] += p[-1];
			(++q)[1] += p[-1];
			*p += p[-2];
first_side:
			for (uint64_t i = length; i--; ) {
				if (x < *p) {
					// Found the answer
					return *p;
				}
				*q++ += *p;
				*q += *p;
				p[1] += *p;
				q[1] += *p++;
			}
		}
		// Advance to the next ring
		*q++ += *p;
		*q++ += *p;
		p[1] += p[-1];
	}
}

void day03(input_t in)
{
	uint64_t x = 0;
	for (; in.len--; in.s++) {
		if (*in.s >= '0') {
			x = x * 10 + *in.s - '0';
		}
	}
	printf("Day 03 Part 1: %lu\n", part1(x));
	printf("Day 03 Part 2: %lu\n", part2(x));
}
