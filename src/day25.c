#include "advent2017.h"

/* Tape size, in 64-bit units.  For my input, 256 (16384 bits, or
 * a max distance of 8192 from start) is enough; increase it if necessary.
 */
#define TAPESZ 4096

/* Cache size, see below.  If the program hangs, it may be because
 * the hash table filled up.  In that case, try making it bigger.
 * Choose a prime number.  My input uses 318 cache slots.
 */
#define CACHESZ 8191

typedef struct {
	int64_t write, move;
	int8_t  next_state;
} mach_t;

/* Memoizing a Turing machine?  How else can we expect it to run
 * through 12.4 million steps in just 1.3 milliseconds?
 */
typedef struct {
	/* a = The 64 symbols to the left of the cursor
	 * b = The symbol at the cursor and 63 symbols to the right
	 * next_a, next_b = The new tape contents
	 */
	uint64_t a, b, next_a, next_b;

	/* How many steps it took before the cursor moved a distance
	 * of 64 away (left or right) from where it began
	 */
	uint64_t steps;

	// Starting and ending state
	int8_t   state, next_state;

	// Move direction (-1 or 1); distance is always exactly 64 symbols
	int8_t   move;
} hash_t;

static hash_t cache[CACHESZ] = { };

static inline hash_t * lookup(uint64_t a, uint64_t b, int8_t s)
{
	size_t idx = (a ^ b ^ s) % CACHESZ;
	hash_t *h = cache + idx;
	while (h->move && (h->a != a || h->b != b || h->state != s)) {
		if (++idx == CACHESZ) idx = 0;
		h = cache + idx;
	}
	return h;
}

void day25(input_t in)
{
	int8_t *s = calloc(in.len, sizeof(*s)), *se = s;
	uint64_t steps = 0, n = 0, x = 0;

	// Collect all the interesting letters from the input
	for (; in.len--; in.s++) {
		if (*in.s >= 'A' && *in.s <= 'F' && (in.s[1] == '.' || in.s[1] == ':')) {
			*se++ = *in.s - 'A';      // State names
		} else if (*in.s >= '0' && *in.s <= '9') {
			n = n * 10 + *in.s - '0'; // Numbers
			x = 1;
		} else if (*in.s == ' ' && in.s[1] == 'l') {
			*se++ = -1;               // Left
		} else if (*in.s == ' ' && in.s[1] == 'r') {
			*se++ = 1;                // Right
		} else if (x) {
			if (steps) {
				*se++ = n & 1;    // Binary values
			} else {
				steps = n;        // The number of steps
			}
			n = x = 0;
		}
	}

	/* Should have picked up 55 values in the list:
	 *    start state + 9 values for each of 6 states 
	 */
	if (se - s != 55) abort();

	/* The Turing machine transition function is stored as
	 * a flat array of 6 states x 2 symbols:
	 *   { A1, A0, B1, B0, ..., F0 }
	 * State numbers are doubled to accommodate this structure.
	 */
	mach_t tm[12];
	int8_t *p = s + 1;
	for (size_t i = 6; i--; ) {
		size_t state = *p++ * 2;
		for (size_t j = 2; j--; ) {
			mach_t *m = &tm[state + !*p++];
			m->write      = -*p++; // 00000... or 11111...
			m->move       =  *p++;
			m->next_state =  *p++ * 2;
		}
	}

	// Initial state
	int8_t state = s[0] * 2;
	free(s);

	/* The tape consists of 1-bit symbols packed into 64-bit integers;
	 * the cursor begins in the middle.
	 */
	uint64_t tape[TAPESZ] = { }, *t = tape + TAPESZ / 2, mask = 1;

	uint64_t *window = t - 1;
	hash_t *h = lookup(0, 0, state);
	h->steps = steps;
	h->state = state;

	while (steps--) {
		// Look up the transition function
		uint64_t symbol = *t & mask; // Masking tape?
		mach_t *m = &tm[state + !symbol];

		// Write new symbol
		*t ^= symbol ^ m->write & mask;

		// Update mask
		if (m->move > 0) {
			mask <<= 1; // Right
		} else {
			mask >>= 1; // Left
		}

		if (mask) {
			state = m->next_state;
			continue;
		}

		// Advance tape position and reset the mask
		if (m->move > 0) {
			mask = 1;
			t++;
		} else {
			mask = 0x8000000000000000L;
			t--;
		}

		if (!(h && (t - window) & 2)) {
			// Cache is disabled, or cursor still within window
			state = m->next_state;
			continue;
		}

		if (m->move > 0) {
			state = m->next_state;
		} else {
			/* Back up a step because the cache expects the
			 * cursor mask to be in a specific position
			 */
			*++t ^= symbol ^ m->write & (mask = 1);
			steps++;
		}

		// Write the results to cache
		h->next_a = window[0];
		h->next_b = window[1];
		h->next_state = state;
		h->steps -= steps;
		h->move = m->move;

		while (h) {
			window = t - 1;
			h = lookup(window[0], window[1], state);
			if (!h->move) {
				// Cache miss
				h->a = window[0];
				h->b = window[1];
				h->state = state;
				h->steps = steps;
				break;
			} else if (h->steps > steps) {
				// Too many steps; disable cache
				h = NULL;
				break;
			} else {
				// Cache hit
				window[0] = h->next_a;
				window[1] = h->next_b;
				state     = h->next_state;
				steps    -= h->steps;
				if (h->move > 0) t++; else t--;
			}
		}
	}

	uint64_t part1 = 0;
	for (t = tape; t != tape + TAPESZ; t++) {
		part1 += __builtin_popcountl(*t);
	}

	printf("Day 25 Part 1: %lu\n", part1);
}
