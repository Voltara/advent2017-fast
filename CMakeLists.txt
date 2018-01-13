cmake_minimum_required (VERSION 3.5)
project (advent2017)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native -flax-vector-conversions")

add_executable(advent2017
	src/main.c
	src/knothash.c
	src/day01.c
	src/day02.c
	src/day03.c
	src/day04.c
	src/day05.c
	src/day06.c
	src/day07.c
	src/day08.c
	src/day09.c
	src/day10.c
	src/day11.c
	src/day12.c
	src/day13.c
	src/day14.c
	src/day15.c
	src/day16.c
	src/day17.c
	src/day18.c
	src/day19.c
	src/day20.c
	src/day21.c
	src/day22.c
	src/day23.c
	src/day24.c
	src/day25.c
	)

target_link_libraries(advent2017 m)
