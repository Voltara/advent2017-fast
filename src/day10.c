#include "advent2017.h"

// See knothash.c
void day10(input_t in)
{
	input_t in2 = in;

	// Trim whitespace
	while (in2.len && in2.s[in2.len - 1] <= ' ') in2.len--;

	// Split the key into numbers for Part 1
	uint8_t *k = calloc(in.len + 5, sizeof(*k)), *e = k;
	for (; in.len--; in.s++) {
		if (*in.s >= '0') {
			*e = *e * 10 + *in.s - '0';
		} else {
			e++;
		}
	}

	// Part 1
	uint8_t a[256], pos = 0, skip = 0;
	knothash_init(a);
	knothash_round(a, k, e, &pos, &skip);
	free(k);
	uint64_t part1 = a[0] * a[1];

	__uint128_t part2 = knothash(in2.s, in2.s + in2.len);

	printf("Day 10 Part 1: %lu\n", part1);
	printf("Day 10 Part 2: %016lx%016lx\n",
			(uint64_t) (part2 >> 64), (uint64_t) part2);
}
