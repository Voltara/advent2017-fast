#include "advent2017.h"

void day11(input_t in)
{
	uint64_t part1 = 0, part2 = 0, x = 0, y = 0;
	for (; in.len--; in.s++) {
		switch (*in.s) {
		    case 's': y += in.s[1] != 'e'; break;
		    case 'e': x++;                 break;
		    case 'w': x--;                 break;
		    case 'n': y -= in.s[1] != 'w'; break;
		    default:
			      part1 = (abs(x) + abs(y) + abs(x + y)) / 2;
			      if (part2 < part1) {
				      part2 = part1;
			      }
		}
	}
	printf("Day 11 Part 1: %lu\n", part1);
	printf("Day 11 Part 2: %lu\n", part2);
}
