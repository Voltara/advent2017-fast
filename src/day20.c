#include <math.h>
#include "advent2017.h"

typedef int32_t v4si __attribute__ ((vector_size (16)));
typedef float   v4sf __attribute__ ((vector_size (16)));

typedef struct {
	v4si p, v, a;
	int gone;
} particle_t;

typedef struct {
	int32_t t;
	particle_t *x, *y;
} collision_t;

typedef struct {
	int32_t t;
	particle_t *p;
} hash_t;

// Uses timestamp `t` to invalidate the table between steps
#define SZ 2999
static inline particle_t * hash_collide(particle_t *p, int32_t t)
{
	static hash_t tbl[SZ] = { };
	uint64_t *u = (uint64_t *) &p->p, idx = u[0] % SZ;
	hash_t *h;
	while ((h = tbl + idx)->t == t) {
		uint64_t *v = (uint64_t *) &h->p->p;
		if (u[0] == v[0] && u[1] == v[1]) {
			return h->p;
		}
		if (++idx == SZ) idx = 0;
	}
	h->t = t, h->p = p;
	return 0;
}
#undef SZ

// Return first positive integer collision time, or 0 if none exists
static inline int32_t collision_time(particle_t *x, particle_t *y)
{
	/* Difference of `a*x^2 + (2v + a)*x + 2p` between particles.
	 * The equation is doubled to avoid dividing acceleration by 2;
	 * this does not affect the roots.  The `b` coefficient is negated
	 * for convenience.
	 */
	v4si a =   x->a - y->a;
	v4si b = ((y->v - x->v) << 1) - a;
	v4si c =  (x->p - y->p) << 1;

	// Check the discriminant
	v4si disc = b * b - 4 * a * c;
	if (__builtin_ia32_movmskps((v4sf) disc)) {
		return 0; // Imaginary
	}

	/* Floating-point sqrt is correct for all integer perfect squares,
	 * and the CPU is thoughtful enough to provide an instruction for it
	 */
	v4si disc_r = __builtin_ia32_cvtps2dq(__builtin_ia32_sqrtps(__builtin_ia32_cvtdq2ps(disc)));
	if (__builtin_ia32_movmskps(disc_r * disc_r != disc)) {
		return 0; // Irrational
	}

	v4si rv[2] = { b + disc_r, b - disc_r };
	a <<= 1; // `2a` for the denominator

	// No SIMD integer division means checking axes individually
	int32_t t0 = 1, t1, all = 1, r;
	for (size_t i = 0; i < 3; i++) {
		if (!a[i]) { // Linear?
			if (!b[i]) { // Constant?
				if (!c[i]) { // Same?
					continue;
				}
			} else if (c[i] % b[i] == 0) { // Integer root?
				r = c[i] / b[i];
				if (r > 0 && (all || r == t0 || r == t1)) {
					t0 = t1 = r;
					all = 0;
					continue;
				}
			}

			return 0;
		} else { // Quadratic?
			int32_t m[2], *mp = m;

			for (size_t j = 0; j < 2; j++) {
				if (rv[j][i] % a[i]) {
					continue; // Non-integer
				}
				r = rv[j][i] / a[i];
				if (r > 0 && (all || r == t0 || r == t1)) {
					*mp++ = r;
				}
			}
			all = 0;

			// Update the solution
			switch (mp - m) {
			    case 0: return 0;
			    case 1: t0 = t1 = m[0];       break;
			    case 2: t0 = m[0], t1 = m[1]; break;
			}
		}
	}

	return t0 < t1 ? t0 : t1;
}

// Returns minimum time to align the signs of p,v,a
static inline int32_t alignment_time(particle_t *x)
{
	int32_t t = 0;

	for (size_t i = 0; i < 3; i++) {
		int32_t a = x->a[i], v = x->v[i], p = x->p[i];

		// Make leading term positive
		if (a < 0 || !a && (v < 0 || !v && p < 0)) {
			a = -a, v = -v, p = -p;
		}

		// Already aligned?
		if (v >= 0 && p >= 0) {
			continue;
		}

		int32_t x;

		if (!a) { // Linear?
			x = (v - p - 1) / v;
			if (t < x) t = x;
			continue;
		} else { // Quadratic?
			x = (a - v - 1) / a;
			p += a * x * (x + 1) / 2 + v * x;
			v += a * x;
			if (p < 0) {
				int32_t b = 2 * v + a;
				int32_t c = 2 * p;
				// Guaranteed real, and we know which root
				int32_t disc_r = ceil(sqrt(b * b - 4 * a * c));
				x +=  (disc_r - b + 2 * a - 1) / (2 * a);
			}
		}

		if (t < x) {
			t = x;
		}
	}

	return t;
}

static inline uint64_t part1(particle_t *P, particle_t *e)
{
	size_t *slow = calloc(e - P, sizeof(*slow)), *se = slow;
	uint32_t min_a = ~0, d_min = ~0;
	uint64_t part1 = 0;

	// Find list of particles with minimum Manhattan acceleration
	for (particle_t *x = P; x != e; x++) {
		v4si av = __builtin_ia32_pabsd128(x->a);
		uint32_t a = av[0] + av[1] + av[2];
		if (a <= min_a) {
			if (a < min_a) {
				min_a = a;
				se = slow;
			}
			*se++ = x - P;
		}
	}

	// Find minimum time required to align all signs
	int32_t t = 0;
	for (size_t *xi = slow; xi != se; xi++) {
		int32_t t1 = alignment_time(P + *xi);
		if (t < t1) t = t1;
	}

	// Find particle closest to origin at that time
	v4si tv = { t, t, t, t };
	for (size_t *xi = slow; xi != se; xi++) {
		particle_t *x = P + *xi;
		v4si dv = __builtin_ia32_pabsd128(
				tv * (tv + 1) / 2 * x->a + tv * x->v + x->p);
		uint32_t d = dv[0] + dv[1] + dv[2];
		if (d < d_min) {
			part1 = *xi;
			d_min = d;
		}
	}

	free(slow);

	return part1;
}

static inline uint64_t part2(particle_t *P, particle_t *e)
{
	/* Simulate 40 steps to reduce particle count and speed up
	 * next phase, secretly already solving Part 2, because that
	 * is enough to resolve all collisions in my input.  (Change
	 * the 40 to 0 to prove the second phase works.  It will
	 * increase solution time from 0.8ms to 2.1ms.)
	 */
	for (int32_t t = 1; t <= 40; t++) {
		for (particle_t *p = P; p != e; p++) {
			if (p->gone) {
				*p-- = *--e;
			} else {
				// Advance the particle one step
				p->p += (p->v += p->a);

				particle_t *q = hash_collide(p, t);
				if (q) {
					p->gone = q->gone = 1;
				}
			}
		}
	}

	// Clean up after final simulation pass
	for (particle_t *p = P; p != e; p++) {
		if (p->gone) {
			*p-- = *--e;
		}
	}

	size_t nP = e - P;
	collision_t *C = calloc(nP * (nP - 1) / 2, sizeof(*C)), *ce = C;

	// Compute collision time for all particle pairs
	for (particle_t *x = P; x < e - 1; x++) {
		for (particle_t *y = x + 1; y != e; y++) {
			int32_t t = collision_time(x, y);
			if (t) {
				ce->t = t, ce->x = x, ce->y = y;
				ce++;
			}
		}
	}

	// Sort by collision time (unnecessary for my input)
	int cmp(const void *a, const void *b) {
		int32_t ta = ((const collision_t *) a)->t;
		int32_t tb = ((const collision_t *) b)->t;
		return ta < tb ? -1 : ta > tb;
	}
	qsort(C, ce - C, sizeof(*C), cmp);

	for (collision_t *c = C; c != ce; c++) {
		// Ignore collisions involving previously annihilated particles
		if (c->x->gone && c->x->gone < c->t) continue;
		if (c->y->gone && c->y->gone < c->t) continue;

		nP -= !c->x->gone + !c->y->gone;
		c->x->gone = c->y->gone = c->t;
	}

	free(C);

	return nP;
}

void day20(input_t in)
{
	particle_t *P = calloc(in.len / 18 + 1, sizeof(*P)), *e = P;
	for (int32_t neg = 0, x = 0, a[9], *ae = a; in.len--; in.s++) {
		if (*in.s == '-') {
			neg = 1;
		} else if (*in.s >= '0' && *in.s <= '9') {
			if (!x) {
				if (ae - a == 9) abort();
				*ae = 0, x = 1;
			}
			*ae = *ae * 10 + *in.s - '0';
		} else {
			if (x) {
				if (neg) *ae = -*ae;
				ae++, neg = x = 0;
			}
			if (*in.s == '\n') {
				e->p = (v4si){ a[0], a[1], a[2], 0 };
				e->v = (v4si){ a[3], a[4], a[5], 0 };
				e->a = (v4si){ a[6], a[7], a[8], 0 };
				e++; ae = a;
			}
		}
	}

	printf("Day 20 Part 1: %lu\n", part1(P, e));
	printf("Day 20 Part 2: %lu\n", part2(P, e));

	free(P);
}
