#include "advent2017.h"

typedef struct chip {
	uint64_t a, b, busy;
	struct chip *na, *nb;
} chip_t;

static void build(uint64_t port, uint64_t strength, uint64_t length);

static chip_t **idx_a, **idx_b;
static uint64_t part1, part2, longest;

void day24(input_t in)
{
	chip_t *c = calloc(in.len, sizeof(*c)), *e = c;
	uint64_t n = 0;
	for (; in.len--; in.s++) {
		switch (*in.s) {
		    case '/':  e->a   = n; n = 0;          break;
		    case '\n': e++->b = n; n = 0;          break;
		    default:   n = n * 10 + *in.s - '0';   break;
		}
	}

	// Find the maximum port number
	uint64_t max = 0;
	for (chip_t *p = c; p != e; p++) {
		if (p->a > max) max = p->a;
		if (p->b > max) max = p->b;
	}

	// Create two indexes, one for the "a" port number, and one for "b"
	idx_a = calloc(max + 1, sizeof(*idx_a));
	idx_b = calloc(max + 1, sizeof(*idx_b));
	for (chip_t *p = c; p != e; p++) {
		p->na = idx_a[p->a]; idx_a[p->a] = p;
		// If the chip is a doublet, don't index it a second time
		if (p->a != p->b) {
			p->nb = idx_b[p->b]; idx_b[p->b] = p;
		}
	}

	// Solve it
	part1 = part2 = longest = 0;
	build(0, 0, 0);

	free(c); free(idx_a); free(idx_b);

	printf("Day 24 Part 1: %lu\n", part1);
	printf("Day 24 Part 2: %lu\n", part2);
}

static void build(uint64_t port, uint64_t strength, uint64_t length)
{
	if (strength > part1) {
		part1 = strength;
	}

	if (length > longest) {
		part2 = strength;
		longest = length;
	} else if (length == longest && strength > part2) {
		part2 = strength;
	}

	length++;
	strength += port;

	for (chip_t *p = idx_a[port]; p; p = p->na) {
		if (!p->busy) {
			p->busy = 1;
			build(p->b, strength + p->b, length);
			p->busy = 0;
		}
	}

	for (chip_t *p = idx_b[port]; p; p = p->nb) {
		if (!p->busy) {
			p->busy = 1;
			build(p->a, strength + p->a, length);
			p->busy = 0;
		}
	}
}
