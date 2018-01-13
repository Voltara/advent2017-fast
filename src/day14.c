#include "advent2017.h"

typedef struct {
	uint16_t parent;
	uint16_t rank;
} prank_t;

static uint16_t find(prank_t *prank, uint16_t x)
{
	if (prank[x].parent != x) {
		prank[x].parent = find(prank, prank[x].parent);
	}
	return prank[x].parent;
}

static void unite(prank_t *prank, uint16_t x, uint16_t y)
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

static void unite_mask(prank_t *prank, __uint128_t m, size_t base, size_t offset)
{
	// Cheat: Fails when there is a gap of 64 or more zeroes (unlikely)
	base--;
	for (int s; s = __builtin_ffsl(m); m >>= s) {
		base += s;
		unite(prank, base, base + offset);
	}
}

void day14(input_t in)
{
	uint64_t part1 = 0, part2 = 0;

	// Trim trailing shitespace
	while (in.len && in.s[in.len - 1] <= ' ') in.len--;

	// Make copy of input concatenated with "-0"
	uint8_t *k = calloc(in.len + 5, sizeof(*k)), *e = k + in.len;
	memcpy(k, in.s, in.len);
	*e++ = '-';
	*e++ = '0';

	__uint128_t g[128];
	for (int i = 0; i < 128; i++) {
		g[i] = knothash(k, e);

		part1 += __builtin_popcountl((uint64_t) g[i]) +
		         __builtin_popcountl((uint64_t) (g[i] >> 64));

		// Increment the counter
		for (uint8_t *p = e - 1; ; p--) {
			if (*p == '-') {
				p[1] = '1';
				*e++ = '0';
				break;
			} else if ((*p)++ < '9') {
				break;
			} else {
				*p = '0';
			}
		}
	}
	free(k);

	// Initialize union-find table
	prank_t prank[16384] = { };
	for (size_t i = 0; i < 16384; i++) {
		prank[i].parent = i;
	}

	// Unite adjacent 1-bits in the grid
	__uint128_t prev = 0;
	for (size_t i = 0; i < 128; prev = g[i++]) {
		size_t base = 128 * i;
		// Horizontal pairs
		unite_mask(prank, g[i] & (g[i] >> 1), base, 1);
		// Vertical pairs
		unite_mask(prank, g[i] & prev, base, -128);
	}

	// Count how many 1-bits in the grid are group leaders
	for (size_t i = 0; i < 128; i++) {
		size_t base = 128 * i - 1;
		for (int s; s = __builtin_ffsl(g[i]); g[i] >>= s) {
			base += s;
			part2 += base == find(prank, base);
		}
	}

	printf("Day 14 Part 1: %lu\n", part1);
	printf("Day 14 Part 2: %lu\n", part2);
}
