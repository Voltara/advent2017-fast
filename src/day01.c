#include "advent2017.h"

void day01(input_t in)
{
	uint64_t part1 = 0, part2 = 0;

	if (*in.s < '0') abort(); // safety

	// Trim trailing whitespace
	char *end = in.s + in.len;
	while (end[-1] < '0') {
		end--;
	}

	char  prev = end[-1];
	char *half = in.s + (end - in.s) / 2;

	// Solve Part 2 and first half of Part 1
	for (; half != end; prev = *in.s++, half++) {
		part1 += ( prev - '0') & -(*in.s ==  prev);
		part2 += (*half - '0') & -(*in.s == *half);
	}
	part2 *= 2;

	// Solve second half of Part 1
	for (; in.s != end; prev = *in.s++) {
		part1 += ( prev - '0') & -(*in.s == prev);
	}

	printf("Day 01 Part 1: %lu\n", part1);
	printf("Day 01 Part 2: %lu\n", part2);
}
