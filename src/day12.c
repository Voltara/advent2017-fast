#include "advent2017.h"

typedef struct {
	uint64_t parent;
	uint64_t rank;
} prank_t;

static uint64_t find(prank_t *prank, uint64_t x)
{
	if (prank[x].parent != x) {
		prank[x].parent = find(prank, prank[x].parent);
	}
	return prank[x].parent;
}

static void unite(prank_t *prank, uint64_t x, uint64_t y)
{
	x = find(prank, x);
	y = find(prank, y);
	if (x != y) {
		if (prank[x].rank < prank[y].rank) {
			prank[x].parent = y;
		} else {
			prank[y].parent = x;
			prank[x].rank += prank[x].rank == prank[y].rank;
		}
	}
}

void day12(input_t in)
{
	prank_t *prank = calloc(in.len, sizeof(*prank));

	int64_t parent = 0, n = 0, have_number = 0, max = 0;

	for (; in.len--; in.s++) {
		if (*in.s >= '0' && *in.s <= '9') {
			n = n * 10 + *in.s - '0';
			have_number = 1;
		} else {
			if (have_number) {
				if (!prank[++n].parent) {
					prank[n].parent = n;
				}

				if (n > max) {
					max = n;
				}

				if (parent) {
					unite(prank, parent, n);
				} else {
					parent = n;
				}

				have_number = 0;
				n = 0;
			}

			if (*in.s == '\n') {
				parent = 0;
			}
		}
	}

	uint64_t part1 = 0, part2 = 0, group_one = find(prank, 1);
	for (uint64_t i = 1; i <= max; i++) {
		uint64_t j = find(prank, i);
		part1 += j == group_one;
		part2 += j == i;
	}

	free(prank);

	printf("Day 12 Part 1: %lu\n", part1);
	printf("Day 12 Part 2: %lu\n", part2);
}
