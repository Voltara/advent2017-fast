#include "advent2017.h"

static inline uint64_t swap9(uint64_t x) {
	return x << 8 & 256 | x << 4 & 160 | x & 84 | x >> 4 & 10 | x >> 8 & 1;
}

static inline uint64_t flip9(uint64_t x) {
	return x << 6 & 448 | x & 56 | x >> 6 & 7;
}

static inline uint64_t swap4(uint64_t x) {
	return x << 3 & 8 | x & 6 | x >> 3 & 1;
}

static inline uint64_t flip4(uint64_t x) {
	return x << 2 & 12 | x >> 2 & 3;
}

static inline void perm9(uint64_t *map, uint64_t x, uint64_t id) {
	for (size_t i = 4; i--; ) {
		x = swap9(x); map[x] = id;
		x = flip9(x); map[x] = id;
	}
}

static inline void perm4(uint64_t *map, uint64_t x, uint64_t id) {
	for (size_t i = 4; i--; ) {
		x = swap4(x); map[x] = id;
		x = flip4(x); map[x] = id;
	}
}

// Initialize the evolution table
static inline void initialize(uint64_t evo[3][108][9],
		uint64_t *next, uint64_t *map4, uint64_t *map9)
{
	for (size_t i = 6; i < 108; i++) {
		uint64_t ac = next[i] & 0xcccc; ac |= ac >> 2;
		uint64_t bd = next[i] & 0x3333; bd |= bd >> 2;
		uint64_t a, b, c, d, x, y;

		evo[1][i][0] = a = map4[ac >> 10 & 15];
		evo[1][i][1] = b = map4[bd >>  8 & 15];
		evo[1][i][2] = c = map4[ac >>  2 & 15];
		evo[1][i][3] = d = map4[bd >>  0 & 15];

		evo[2][i][0] = map9[a = next[a]];
		evo[2][i][1] = map9[b = next[b]];
		evo[2][i][2] = map9[c = next[c]];
		evo[2][i][3] = map9[d = next[d]];

		x = a >> 3 & 56 | b >> 6; y = a & 56 | b >> 3 & 7;
		evo[0][i][0] = x >> 2 & 12 | y >> 4 & 3;
		evo[0][i][1] = x >> 0 & 12 | y >> 2 & 3;
		evo[0][i][2] = x << 2 & 12 | y >> 0 & 3;

		x = a << 3 & 56 | b & 7; y = c >> 3 & 56 | d >> 6;
		evo[0][i][3] = x >> 2 & 12 | y >> 4 & 3;
		evo[0][i][4] = x >> 0 & 12 | y >> 2 & 3;
		evo[0][i][5] = x << 2 & 12 | y >> 0 & 3;

		x = c & 56 | d >> 3 & 7; y = c << 3 & 56 | d & 7;
		evo[0][i][6] = x >> 2 & 12 | y >> 4 & 3;
		evo[0][i][7] = x >> 0 & 12 | y >> 2 & 3;
		evo[0][i][8] = x << 2 & 12 | y >> 0 & 3;

		for (size_t j = 0; j < 9; j++) {
			evo[0][i][j] = map9[next[map4[evo[0][i][j]]]];
		}
	}
}

static inline void evolve(uint64_t counts[2][108], size_t idx,
		const uint64_t evo[3][108][9], size_t k)
{
	for (size_t i = 0; i < 108; i++) {
		counts[idx][i] = 0;
	}
	for (size_t i = 0; i < 108; i++) {
		if (counts[!idx][i]) {
			for (size_t j = k ? 4 : 9; j--; ) {
				counts[idx][evo[k][i][j]] += counts[!idx][i];
			}
		}
	}
}

static inline uint64_t count_bits(uint64_t *counts, uint64_t *pic)
{
	uint64_t total = 0;
	for (size_t i = 0; i < 108; i++) {
		total += counts[i] * __builtin_popcountl(pic[i]);
	}
	return total;
}

void day21(input_t in)
{
	uint64_t map9[512] = { }, map4[16] = { }, pic[108], next[108];
	uint64_t evo[3][108][9] = { }, id, id4 = 0, id9 = 6, w = 0;
	for (uint64_t x = 0; in.len--; in.s++) {
		switch (*in.s) {
		    case '#':
			x = x << 1 | 1;
			w++;
			break;
		    case '.':
			x = x << 1;
			w++;
			break;
		    case '=':
			if (w == 9) {
				if (id9 == 108) abort();
				perm9(map9, x, id = id9++);
			} else if (w == 4) {
				if (id4 == 6) abort();
				perm4(map4, x, id = id4++);
			} else {
				abort();
			}
			pic[id] = x;
			w = x = 0;
			break;
		    case '\n':
			next[id] = x;
			w = x = 0;
			break;
		}
	}

	if (id4 != 6 || id9 != 108) abort();

	initialize(evo, next, map4, map9);

	/* Initial state is octal 0217:
	 *     010
	 *     001
	 *     111
	 */
	uint64_t counts[2][108] = { };
	size_t i = 0;
	counts[i][map9[0217]] = 1;

	// Evolve 3 steps
	evolve(counts, i = !i, evo, 0);

	// Evolve 2 more steps (total 5) for Part 1
	evolve(counts, i = !i, evo, 2);
	uint64_t part1 = count_bits(counts[i], pic);

	// Discard previous and evolve 15 steps (total 18) for Part 2
	i = !i;
	for (size_t k = 5; k--; ) {
		evolve(counts, i = !i, evo, 0);
	}
	uint64_t part2 = count_bits(counts[i], pic);

	printf("Day 21 Part 1: %lu\n", part1);
	printf("Day 21 Part 2: %lu\n", part2);
}
