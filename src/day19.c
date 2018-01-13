#include "advent2017.h"

void day19(input_t in)
{
	// Find the maze entrance
	char *p;
	for (p = in.s; *p == ' '; p++);

	// Find the width
	char *e;
	for (e = p + 1; *e++ != '\n'; );
	int64_t width = e - in.s;

	char *part1 = calloc(in.len, sizeof(*part1));
	e = part1;

	int64_t part2 = 1;

	for (int64_t dx = 0, dy = 1; ; part2++) {
		// Gobble up a Part 1 letter
		if (*p >= 'A' && *p <= 'Z') {
			*e++ = *p;
		}

		// Ahead?
		char *next = p + dy * width + dx;
		if (*next != ' ') { p = next; continue; }

		// Left?
		int64_t tmp = dx; dx = -dy; dy = tmp;
		next = p + dy * width + dx;
		if (*next != ' ') { p = next; continue; }

		// Right?
		dx = -dx; dy = -dy;
		next = p + dy * width + dx;
		if (*next != ' ') { p = next; continue; }

		// Done
		break;
	}

	printf("Day 19 Part 1: %s\n", part1);
	printf("Day 19 Part 2: %ld\n", part2);

	free(part1);
}
