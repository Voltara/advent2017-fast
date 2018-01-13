#include "advent2017.h"

typedef struct {
	uint64_t weight;
	uint64_t child[];
} info_t;

static const size_t sz = 4093;
typedef struct {
	uint64_t id;
	uint64_t parent;
	info_t *info;
} hash_t;

static hash_t * hfind(hash_t *h, uint64_t id)
{
	size_t idx = id % sz;
	while (h[idx].id) {
		if (h[idx].id == id) {
			return h + idx;
		}
		if (++idx == sz) idx = 0;
	}
	h[idx].id = id;
	return h + idx;
}

/* a = Packed array of info_t structures:
 *     weight, child, child, 0, weight, child, 0, ...
 * b = List of IDs
 */
void day07(input_t in)
{
	hash_t *h = calloc(sz, sizeof(*h));
	uint64_t *a = calloc(in.len, sizeof(*a)), *ae = a;
	uint64_t *be = a + in.len, *b = be;

	uint64_t id = 0, weight = 0, is_child = 0;
	for (; in.len--; in.s++) {
		if (*in.s >= 'a') {
			id = (id << 8) | *in.s;
		} else if (*in.s == ')') {
			is_child = 1;
			*ae++ = weight;
		} else if (*in.s >= '0') {
			weight = weight * 10 + *in.s - '0';
		} else {
			if (id) {
				if (is_child) {
					hfind(h, id)->parent = *b;
					*ae++ = id;
				} else {
					hfind(h, id)->info = (info_t *) ae;
					*--b = id;
				}
				id = 0;
			}

			if (*in.s == '\n') {
				// End of line
				is_child = weight = 0;
				*ae++ = 0;
			}
		}
	}

	/* Toposort the ID list by parent pointer.  In the resulting list,
	 * each ID will precede its parent in the tree; the root will be last.
	 * This sort clobbers the parent pointers, which will no longer
	 * be needed.
	 */
	for (uint64_t *t = be; t != b; ) {
		for (uint64_t *p = b; p < t; p++) {
			hash_t *z = hfind(h, *p);
			if (!z->parent) {
				uint64_t tmp = *--t; *t = *p; *p = tmp;
				for (uint64_t *q = z->info->child; *q; q++) {
					hfind(h, *q)->parent = 0;
				}
			}
		}
	}

	uint64_t part2 = 0;

	/* Bubble node weights up the tree, starting at leaves.
	 * Break on the first unbalanced node encountered.
	 */
	info_t *bad = NULL;
	for (uint64_t *p = b; p != be; p++) {
		info_t *zi = hfind(h, *p)->info;
		uint64_t *child = zi->child;
		if (!*child) {
			// Skip leaf nodes
			continue;
		}

		// Lookup first child and accumulate its weight
		info_t *ci = hfind(h, *child)->info;
		zi->weight += ci->weight;

		// Iterate over remaining children, accumulating weight
		for (uint64_t *q = child + 1; *q; q++) {
			info_t *qi = hfind(h, *q)->info;
			zi->weight += qi->weight;

			if (qi->weight == ci->weight) {
				// Still balanced
				continue;
			}

			// Found the unbalanced node
			uint64_t tiebreaker = child[1 + (q == child + 1)];
			info_t *ti = hfind(h, tiebreaker)->info;
			part2 = ti->weight;
			bad = (part2 == ci->weight) ? qi : ci;

			// Get bad child's original weight
			for (uint64_t *c = bad->child; *c; c++) {
				part2 -= hfind(h, *c)->info->weight;
			}

			goto done;
		}
	}
done:;
	// Convert root ID to string for Part 1
	char part1[8] = "", *p = part1;
	for (be--, *be <<= __builtin_clzl(*be) & 56; *be; *be <<= 8) {
		*p++ = *be >> 56;
	}

	free(a);
	free(h);

	printf("Day 07 Part 1: %.8s\n", part1);
	printf("Day 07 Part 2: %lu\n", part2);
}
