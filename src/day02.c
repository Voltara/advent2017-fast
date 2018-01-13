#include "advent2017.h"

void day02(input_t in)
{
	uint64_t part1 = 0, part2 = 0;

	uint64_t *a = calloc(in.len, sizeof(*a)), *e = a;
	uint64_t min = ~0UL, max = 0, quotient = 0, n = 0;

	for (; in.len--; in.s++) {
		if (*in.s >= '0') {
			n = n * 10 + *in.s - '0';
		} else {
			// Check for new min/max
			if (n < min) min = n;
			if (n > max) max = n;

			// Check for divisibility with previous numbers
			if (!quotient) {
				for (uint64_t *p = a; p < e; p++) {
					if (n % *p == 0) {
						quotient = n / *p;
						break;
					}
					if (*p % n == 0) {
						quotient = *p / n;
						break;
					}
				}
			}

			if (*in.s != '\n') {
				// Append number to array
				*e++ = n;
			} else {
				// End of line
				part1 += max - min;
				part2 += quotient;
				min = ~0UL;
				max = 0;
				quotient = 0;
				e = a;
			}

			n = 0;
		}
	}

	free(a);

	printf("Day 02 Part 1: %lu\n", part1);
	printf("Day 02 Part 2: %lu\n", part2);
}
