#include "advent2017.h"

void day09(input_t in)
{
	uint64_t part1 = 0, part2 = 0, depth = 0, g = 0;
	for (; in.len--; in.s++) {
		switch (g | *in.s) {
		    case '}':  part1 += depth;
		    case '{':  depth += '|' - *in.s; break;
		    case '<':
		    case 0xbe: g ^= 0x80;            break;
		    case 0xa1: in.s++; in.len--;     break;
		    default:   part2 += !!g;         break;
		}
	}
	printf("Day 09 Part 1: %lu\n", part1);
	printf("Day 09 Part 2: %lu\n", part2);
}
