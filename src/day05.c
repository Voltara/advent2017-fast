#include "advent2017.h"

// TODO Is there a way to solve this without iterating?
void day05(input_t in)
{
	int16_t *a = calloc(in.len, sizeof(*a)), *e = a;
	for (int16_t neg = 0; in.len--; in.s++) {
		if (*in.s < '-') {
			if (neg) {
				*e = -*e;
				if (e + *e < a) {
					// Escape by jumping forward instead
					*e = 32767;
				}
				neg = 0;
			}
			e++;
		} else if (*in.s == '-') {
			neg = 1;
		} else {
			*e = *e * 10 + *in.s - '0';
		}
	}

	uint64_t part1 = 0, part2 = 0;
	for (int16_t *p = a; p < e; part1++) {
		p += *p + p[e - a]++;
	}

	for (int16_t *p = a; p < e; part2++) {
		if (*p < 3) {
			p += (*p)++;
		} else {
			p += (*p)--;
		}
	}

	free(a);

	printf("Day 05 Part 1: %lu\n", part1);
	printf("Day 05 Part 2: %lu\n", part2);
}
