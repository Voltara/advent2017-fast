#include "advent2017.h"

const size_t sz = 8191;
typedef struct {
	int64_t id;
	int64_t value;
} hash_t;

// Lookup register by name, return pointer to value
static int64_t * hfind(hash_t *h, uint64_t id)
{
	size_t idx = (uint64_t) id % sz;
	while (h[idx].id) {
		if (h[idx].id == id) {
			return &h[idx].value;
		}
		if (++idx == sz) idx = 0;
	}
	h[idx].id = id;
	return &h[idx].value;
}

void day08(input_t in)
{
	hash_t *h = calloc(sz, sizeof(*h));

	int64_t part1 = 1L << 63, part2 = part1;

	int64_t tok[8] = { }, *t = tok, neg = 0;
	for (; in.len--; in.s++) {
		if (*in.s == '-') {
			neg = 1;
		} else if (*in.s >= '0' && *in.s <= '9') {
			*t = *t * 10 + *in.s - '0';
		} else if (*in.s > ' ') {
			*t = (*t << 8) | *in.s;
		} else {
			if (*t) {
				if (neg) *t = -*t;
				if (++t - tok == 8) abort();
				*t = neg = 0;
			}

			if (*in.s == '\n') {
				// Negate the increment for "dec"
				int64_t inc = (tok[1] == 0x646563) ? -tok[2] : tok[2];
				int64_t *lv = hfind(h, tok[0]); // lvalue
				int64_t cv = *hfind(h, tok[4]); // comparison value
				switch (tok[5]) {
				    case 0x3c:   if (cv <  tok[6]) *lv += inc; break;
				    case 0x3e:   if (cv >  tok[6]) *lv += inc; break;
				    case 0x3d3d: if (cv == tok[6]) *lv += inc; break;
				    case 0x3c3d: if (cv <= tok[6]) *lv += inc; break;
				    case 0x3e3d: if (cv >= tok[6]) *lv += inc; break;
				    case 0x213d: if (cv != tok[6]) *lv += inc; break;
				}

				if (*lv > part2) {
					part2 = *lv;
				}

				*(t = tok) = 0;
			}
		}
	}

	// Get max value from register hash table for Part 1
	for (size_t i = 0; i < sz; i++) {
		if (h[i].id && h[i].value > part1) {
			part1 = h[i].value;
		}
	}

	free(h);

	printf("Day 08 Part 1: %ld\n", part1);
	printf("Day 08 Part 2: %ld\n", part2);
}
