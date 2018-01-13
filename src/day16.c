#include "advent2017.h"

// Rotate left
static inline uint64_t rol(uint64_t x, int r)
{
	return x << r | x >> (64 - r);
}

// Rotate right
static inline uint64_t ror(uint64_t x, int r)
{
	return x >> r | x << (64 - r);
}

// Swap two nibbles in 64-bit integer
static inline uint64_t nibble_swap(uint64_t x, uint8_t a, uint8_t b)
{
	int r = ((a <<= 2) - (b <<= 2)) & 63;
	uint64_t ma = 15L << a, mb = 15L << b;
	return x & ~(ma | mb) | rol(x, r) & ma | ror(x, r) & mb;
}

// Return the inverse of a nibble permutation
static inline uint64_t inverse(uint64_t x) {
	uint64_t xi = 0;
	for (uint64_t i = 0; i != 16; i++) {
		xi |= i << ((15 & x >> (i << 2)) << 2);
	}
	return xi;
}

// Compose two nibble permutations
static inline uint64_t compose(uint64_t x, uint64_t y)
{
	uint64_t z = 0;
	for (int i = 0; i != 64; i += 4) {
		z |= (15 & x >> ((15 & y >> i) << 2)) << i;
	}
	return z;
}

// Format permutation for output
static const char * format(uint64_t x)
{
	static char s[17] = "";
	char *e = s;
	for (int i = 0; i != 64; i += 4) {
		*e++ = 'a' + (x >> i & 15);
	}
	return s;
}

void day16(input_t in)
{
	const uint64_t identity = 0xfedcba9876543210;
	uint64_t sx = identity, pi = identity;

	// Track the s+x and p moves separately
	uint8_t c = 0, a = 0, b;
	for (; in.len--; in.s++) {
		if (*in.s <= ',') {
			switch (c) {
			    case 'p': pi = nibble_swap(pi, a, b); break;
			    case 's': sx = rol(sx, a << 2);       break;
			    case 'x': sx = nibble_swap(sx, a, b); break;
			}
			a = c = 0;
		}
		else if (*in.s == '/') { b = a; a = 0;             }
		else if (*in.s >= 's') { c = *in.s;                }
		else if (*in.s == 'p') { c = 'p'; a = 15;          }
		else if (*in.s >= 'a') { a = *in.s - 'a';          }
		else if (*in.s >= '0') { a = a * 10 + *in.s - '0'; }
	}

	// The p moves created an inverted permutation, so fix that
	uint64_t p = inverse(pi);

	// Compose a billion times using exponentiation by squaring
	uint64_t sx_sq = sx, sx_billion = identity;
	uint64_t p_sq = p, p_billion = identity;
	for (uint64_t n = 1000000000L; n; n >>= 1) {
		if (n & 1) {
			sx_billion = compose(sx_billion, sx_sq);
			p_billion  = compose(p_billion, p_sq);
		}
		sx_sq = compose(sx_sq, sx_sq);
		p_sq  = compose(p_sq, p_sq);
	}

	uint64_t part1 = compose(p, sx);
	uint64_t part2 = compose(p_billion, sx_billion);

	printf("Day 16 Part 1: %.16s\n", format(part1));
	printf("Day 16 Part 2: %.16s\n", format(part2));
}
