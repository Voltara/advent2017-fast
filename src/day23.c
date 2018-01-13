#include "advent2017.h"

void day23(input_t in)
{
	/* Grab the first number in the file and make a bunch of
	 * assumptions about the rest of the input
	 */
	uint64_t n = 0;
	for (; in.len--; in.s++) {
		if (*in.s >= '0' && *in.s <= '9') {
			n = 10 * n + *in.s - '0';
		} else if (n) {
			break;
		}
	}

	/* The assembly code multiplies every pair of numbers in the
	 * range 2 through n-1, for (n-2)^2 multplications
	 */
	uint64_t part1 = (n - 2) * (n - 2);

	/* Part 2 counts how many numbers are composite, starting with
	 * 100 * (n + 1000), incrementing by 17, and stopping when
	 * 1001 numbers have been tested.
	 */
	uint64_t part2 = 0;
	n = 100 * (n + 1000);
	for (size_t i = 1001; i--; n += 17) {
		for (uint64_t f = 2, s = 4; s <= n; s += (++f << 1) - 1) {
			if (n % f == 0) {
				part2++;
				break;
			}
		}
	}

	printf("Day 23 Part 1: %lu\n", part1);
	printf("Day 23 Part 2: %lu\n", part2);
}
