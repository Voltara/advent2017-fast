#include "advent2017.h"

#define GRID_SIZE 10 // Power of 2: 1024 x 1024

static inline uint64_t rol(uint64_t x, int r) {
	return x << r | x >> (64 - r);
}

static inline uint64_t ror(uint64_t x, int r) {
	return x >> r | x << (64 - r);
}

static inline uint16_t rol16(uint16_t x, int r) {
	return x << r | x >> (16 - r);
}

static inline uint16_t ror16(uint16_t x, int r) {
	return x >> r | x << (16 - r);
}

/* The Part 2 grid is tiled into 2x2 blocks of cells, and the
 * states of each tile's quadrants are packed into an 8-bit integer.
 * The simulation runs at the tile level, using the lookup table
 * built by this function.
 *
 * The table is indexed by the tuple:
 *     direction_bit  (1 bit)
 *     quadrant       (2 bits)
 *     state          (8 bits)
 *
 * And maps to:
 *     next_idx (direction:2, direction_bit:1, quadrant:2, state:8)
 *     steps    (how many steps were taken, range 1-8)
 *     infects  (how many infections happened, range 0-2)
 *
 * Quadrants are numbered:
 *     0 1
 *     3 2
 *
 * This numbering facilitates dense packing of the table by reducing
 * the direction to a single bit, considering each quadrant has only
 * two borders.  Using 0=right, 1=down, 2=left, 3=up, the valid
 * arrival directions per quadrant are:
 *     0(0,1) 1(1,2)
 *     3(3,0) 2(2,3)
 *
 * The direction_bit is simply the difference between quadrant and direction.
 *
 * The next_idx field includes a copy of the full 2-bit direction to
 * reduce the amount of work done by the simulation loop.
 *
 * This method reduces execution time by 33%.  Larger tiles increase table
 * size and code complexity too much: 16-bit 4x2 tiles increase the
 * combinations from 2048 to 786432, making it memory bandwidth bound
 * and slower than 2x2 tiles.
 */
typedef struct {
	uint16_t next_idx;
	uint8_t  steps;
	uint8_t  infects;
} trans_t;

static void init_part2_table(trans_t *tbl)
{
	for (uint16_t i = 0; i < 2048; i++) {
		uint16_t state =   i & 255;
		uint16_t quad  =  (i >> 7) & 6;
		uint16_t dir   = ((i >> 9) + quad) & 6;
		uint16_t move_bit;
		tbl[i].steps   = 0;
		tbl[i].infects = 0;
		do {
			tbl[i].steps++;
			// Rotate the quadrant's state into position
			state = ror16(state, quad);
			switch (state & 3) {
			    case 0: dir = (dir - 2) & 6; break;
			    case 1: tbl[i].infects++;    break;
			    case 2: dir = (dir + 2) & 6; break;
			    case 3: dir ^= 4;            break;
			}
			// Increment the state and rotate back
			if ((++state & 3) == 0) state -= 4;
			state = rol16(state, quad);
			switch (dir) {
			    case 0: move_bit = (quad ^= 2) + 2; break;
			    case 2: move_bit = (quad ^= 6);     break;
			    case 4: move_bit = (quad ^= 2) - 2; break;
			    case 6: move_bit = (quad ^= 6) ^ 4; break;
			}
		} while (move_bit & 4);
moved:
		dir = (dir ^ quad) & 2 | (dir << 1);
		tbl[i].next_idx = state | (quad << 7) | (dir << 9);
	}
}

void day22(input_t in)
{
	// 25x25 only
	if (in.len != 650) abort();

	// Allocate the Part 1 and Part 2 grids
#define G1DIM (1L << (GRID_SIZE) - 3)
	uint64_t *g1 = calloc(G1DIM * G1DIM, sizeof(*g1));
#define G2DIM (G1DIM << 2)
	uint8_t  *g2 = calloc(G2DIM * G2DIM, sizeof(*g2));

	// Roughly center the input in the grid
	size_t x0 = G1DIM / 2 - 2, y0 = x0;
	size_t X0 = G2DIM / 2 - 7, Y0 = X0;

	size_t x = x0, y = y0, s = 0;
	size_t X = X0, Y = Y0, S = 7;
	for (; in.len--; in.s++) {
		if (*in.s == '\n') {
			x = x0;
			if ((s = (s & ~7L) + 8) == 64) y++, s = 0;

			X = X0;
			if (S & 4) Y++, S = 1; else S = 7;
		} else {
			g1[G1DIM * y + x] |= (*in.s & 1L) << s++;
			if (!(s & 7)) x++, s -= 8;

			g2[G2DIM * Y + X] |= (*in.s & 1L) << S;
			S ^= 2;
			if (S == 1 || S == 7) X++;
		}
	}

	// Part 1
	uint64_t part1 = 0, mask = 1L << 36, dir = 0;
	uint64_t *g1p = &g1[G1DIM * (y0 + 1) + (x0 + 1)];
	for (size_t i = 10000; i--; ) {
		if ((*g1p ^= mask) & mask) {
			dir--, part1++;
		} else {
			dir++;
		}
		switch (dir & 3) {
		    case 0:
			if ((mask = ror(mask, 8)) & 0xff00000000000000L) {
				g1p -= G1DIM;
			}
			break;
		    case 1:
			if ((mask = rol(mask, 1)) & 0x0101010101010101L) {
				g1p++, mask = ror(mask, 8);
			}
			break;
		    case 2:
			if ((mask = rol(mask, 8)) & 0x00000000000000ffL) {
				g1p += G1DIM;
			}
			break;
		    case 3:
			if ((mask = ror(mask, 1)) & 0x8080808080808080L) {
				g1p--, mask = rol(mask, 8);
			}
			break;
		}
	}
	free(g1);

	// Tile-level simulation using the lookup table
	trans_t tbl[2048] = { };
	init_part2_table(tbl);
	uint64_t part2 = 0, steps = 10000000;
	uint16_t idx = 0x300; // Quadrant 3 facing up
	uint8_t *g = &g2[G2DIM * (Y0 + 6) + (X0 + 6)];
	for (;;) {
		idx = (idx & 0x700) | *g;
		if (tbl[idx].steps > steps) {
			break;
		}
		steps    -= tbl[idx].steps;
		part2    += tbl[idx].infects;
		*g = idx  = tbl[idx].next_idx;
		switch (idx >> 11) {
		    case 0: g++;        break;
		    case 1: g += G2DIM; break;
		    case 2: g--;        break;
		    case 3: g -= G2DIM; break;
		}
	}

	// Step-by-step simulation for the remainder
	uint16_t quad = (idx >> 7) & 6;
	uint16_t state = *g;
	dir = idx >> 11;
	while (steps--) {
		state = ror16(state, quad);
		switch (state & 3) {
		    case 0: dir = (dir - 1) & 3; break;
		    case 1: part2++;             break;
		    case 2: dir = (dir + 1) & 3; break;
		    case 3: dir ^= 2;            break;
		}
		if ((++state & 3) == 0) state -= 4;
		state = rol16(state, quad);
		switch (dir) {
		    case 0: quad ^= 2; break;
		    case 1: quad ^= 6; break;
		    case 2: quad ^= 2; break;
		    case 3: quad ^= 6; break;
		}
	}
	free(g2);

	printf("Day 22 Part 1: %lu\n", part1);
	printf("Day 22 Part 2: %lu\n", part2);
}
