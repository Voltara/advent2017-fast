#include "advent2017.h"

// Initialize knothash array
void knothash_init(uint8_t *a)
{
	for (size_t i = 0; i < 256; i++) {
		a[i] = i;
	}
}

// One round of the knothash
void knothash_round(uint8_t *a, const uint8_t *k, const uint8_t *e, uint8_t *pos, uint8_t *skip)
{
	for (; k != e; k++) {
		/* Reverse the next "*k" elements of the array.
		 * Wrap around is handled implicitly by using
		 * 8-bit array indices.
		 */
		uint8_t i = *pos + (*k >> 1), j = i - (~*k & 1);
		while (i != *pos) {
			uint8_t tmp = a[--i];
			a[i] = a[++j];
			a[j] = tmp;
		}
		*pos = j + ++*skip;
	}
}

__uint128_t knothash(const uint8_t *k, const uint8_t *e)
{
	const uint8_t pad[] = { 17, 31, 73, 47, 23 };

	uint8_t a[256], pos = 0, skip = 0;

	knothash_init(a);
	for (size_t i = 64; i--; ) {
		// Apply round function to the input array
		knothash_round(a, k, e, &pos, &skip);
		// Then apply it to the padding
		knothash_round(a, pad, pad + sizeof(pad), &pos, &skip);
	}

	// Convert "sparse hash" to 128-bit "dense hash"
	__uint128_t h = 0;
	for (uint8_t *p = a; p != a + 256; ) {
		h <<= 8;
		for (size_t i = 16; i--; ) {
			h ^= *p++;
		}
	}

	return h;
}
