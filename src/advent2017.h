#ifndef _ADVENT2017_H
#define _ADVENT2017_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

typedef struct {
	char *s;
	size_t len;
} input_t;

typedef struct {
	void (*fn)(input_t);
	const char *input_file;
} advent_t;

void knothash_init(uint8_t *a);
void knothash_round(uint8_t *a, const uint8_t *k, const uint8_t *e, uint8_t *pos, uint8_t *skip);
__uint128_t knothash(const uint8_t *k, const uint8_t *e);

void day01(input_t);
void day02(input_t);
void day03(input_t);
void day04(input_t);
void day05(input_t);
void day06(input_t);
void day07(input_t);
void day08(input_t);
void day09(input_t);
void day10(input_t);
void day11(input_t);
void day12(input_t);
void day13(input_t);
void day14(input_t);
void day15(input_t);
void day16(input_t);
void day17(input_t);
void day18(input_t);
void day19(input_t);
void day20(input_t);
void day21(input_t);
void day22(input_t);
void day23(input_t);
void day24(input_t);
void day25(input_t);

static const advent_t advent2017[] = {
	{ day01, "input/day01.txt" },
	{ day02, "input/day02.txt" },
	{ day03, "input/day03.txt" },
	{ day04, "input/day04.txt" },
	{ day05, "input/day05.txt" },
	{ day06, "input/day06.txt" },
	{ day07, "input/day07.txt" },
	{ day08, "input/day08.txt" },
	{ day09, "input/day09.txt" },
	{ day10, "input/day10.txt" },
	{ day11, "input/day11.txt" },
	{ day12, "input/day12.txt" },
	{ day13, "input/day13.txt" },
	{ day14, "input/day14.txt" },
	{ day15, "input/day15.txt" },
	{ day16, "input/day16.txt" },
	{ day17, "input/day17.txt" },
	{ day18, "input/day18.txt" },
	{ day19, "input/day19.txt" },
	{ day20, "input/day20.txt" },
	{ day21, "input/day21.txt" },
	{ day22, "input/day22.txt" },
	{ day23, "input/day23.txt" },
	{ day24, "input/day24.txt" },
	{ day25, "input/day25.txt" },
	{ 0, 0 }
};

#endif
