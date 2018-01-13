#include "advent2017.h"

static inline uint64_t gcd(uint64_t x, uint64_t y)
{
	uint64_t tmp;
	while (x) tmp = x, x = y % x, y = tmp;
	return y;
}

void day13(input_t in)
{
	typedef struct {
		uint64_t mod;
		uint64_t mask;
	} modmask_t;

	modmask_t *mm = calloc(in.len, sizeof(*mm));

	uint64_t part1 = 0, part2 = 0, range = 0, depth;
	for (; in.len--; in.s++) {
		if (*in.s == ':') {
			depth = range;
			range = 0;
		} else if (*in.s >= '0') {
			range = 10 * range + *in.s - '0';
		} else if (*in.s == '\n') {
			uint64_t mod = (range - 1) * 2;
			if (depth % mod == 0) {
				// Add collision severity to Part 1
				part1 += depth * range;
			}
			range = 0;

			// Search for mod in the modmask list (add new
			// entry to end of list if not found)
			modmask_t *p;
			for (p = mm; p->mod && p->mod != mod; p++);
			p->mod = mod;

			// Set the corresponding bit in the mask
			p->mask |= 1L << (mod - depth % mod) % mod;
		}
	}

	/* Repeat each mask pattern until it fills the 64-bit mask,
	 * so that shorter masks can be coalesced into longer ones.
	 * (Also, find the end of the list.)
	 */
	modmask_t *e;
	for (e = mm; e->mod; e++) {
		for (int shift = e->mod; shift < 64; shift <<= 1) {
			e->mask |= e->mask << shift;
		}
	}

	// Coalesce mods that are multiples of each other
	for (modmask_t *p = mm; p < e; p++) {
		int gone = 0;
		for (modmask_t *q = mm; q != e; q++) {
			if ((q->mod > p->mod) && (q->mod % p->mod) == 0) {
				q->mask |= p->mask;
				gone = 1;
			}
		}
		if (gone) {
			// Remove the shorter mask the list
			*p-- = *--e;
		}
	}

	// Move all masks with a single valid remainder to the end of the list
	modmask_t *single = e;
	for (modmask_t *p = mm; p != single; p++) {
		uint64_t m = ~p->mask & ((1L << p->mod) - 1);
		if (__builtin_popcountl(m) == 1) {
			modmask_t tmp = *--single;
			*single = *p;
			*p-- = tmp;
		}
	}

	// Sieve to combine all single-remainder mods
	uint64_t rem = 0, mod = 1;
	if (e != single) {
		rem = __builtin_ffsl(~single->mask) - 1;
		for (e--; e != single; e--) {
			uint64_t r = __builtin_ffsl(~e->mask) - 1;
			while (rem % e->mod != r) {
				rem += single->mod;
			}
			single->mod *= e->mod / gcd(single->mod, e->mod);
		}
		mod = single->mod;
	}

	// Search for Part 2 answer in (k * mod + rem)
	for (part2 = rem; ; part2 += mod) {
		for (modmask_t *p = mm; p != e; p++) {
			if ((1L << (part2 % p->mod)) & p->mask) {
				goto nope;
			}
		}
		break;
nope:;
	}
	free(mm);

	printf("Day 13 Part 1: %lu\n", part1);
	printf("Day 13 Part 2: %lu\n", part2);
}
