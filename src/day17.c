#include "advent2017.h"

static inline int64_t part1(int64_t n)
{
	int64_t pos = 0;
	// Find the index of the 2017th insertion
	for (int64_t i = 1; i <= 2017; i++) {
		pos = (pos + n) % i + 1;
	}
	// Reverse the simulation to find the value which follows
	int64_t part1, want = (pos + 1) % 2017;
	for (part1 = 2017; pos != want; part1--) {
		want -= pos < want;
		if ((pos -= n + 1) < 0) {
			pos += part1;
		}
	}
	return part1;
}

static inline int64_t part2(int64_t n)
{
	/* The position 1 value changes only once per wraparound.  The
	 * skip distance grows exponentially, so this loop runs in
	 * log time.  The number of iterations can be estimated:
	 *
	 *     input * (1 + log(50000000 / input))
	 *
	 * For my input 382, this gives an estimate of 4882.767, which
	 * is close to the actual number 4889.
	 *
	 * This formula is a refinement of `input * log(50000000)` that
	 * accounts for the faster initial growth due to integer rounding
	 * until the simulated array size is at least `input` length.
	 */
	int64_t part2;
	for (int64_t pos = 0, i = 0; i < 50000000L; pos++) {
		if (pos == 1) {
			part2 = i;
		}
		int64_t skip = (i - pos) / n + 1;
		pos += skip * (n + 1) - 1;
		pos %= (i += skip);
	}
	return part2;
}

void day17(input_t in)
{
	int64_t n = 0;
	for (; in.len--; in.s++) {
		if (*in.s >= '0') {
			n = n * 10 + *in.s - '0';
		}
	}

	printf("Day 17 Part 1: %ld\n", part1(n));
	printf("Day 17 Part 2: %ld\n", part2(n));
}
