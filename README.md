# advent2017-fast

[Advent of Code 2017](http://adventofcode.com/2017/) optimized C solutions.  The AoC creator issued a [challenge](https://www.reddit.com/r/adventofcode/comments/7m9mg8/all_years_all_days_solve_them_within_the_time/) to solve each of the puzzles within 250 ms.  I decided to take that a step further and do the whole month within that limit, and managed to get it down to 195 ms.

Here are the timings from an example run on a 4.20 GHz i7-7700K CPU.  The total is greater than the sum of the individual days because of rounding.

    Day  1       28 μs
    Day  2       15 μs
    Day  3        1 μs
    Day  4       63 μs
    Day  5   36,131 μs
    Day  6       82 μs
    Day  7      117 μs
    Day  8       60 μs
    Day  9       66 μs
    Day 10       87 μs
    Day 11       55 μs
    Day 12      100 μs
    Day 13        3 μs
    Day 14    4,574 μs
    Day 15  119,350 μs
    Day 16      127 μs
    Day 17      122 μs
    Day 18      206 μs
    Day 19       21 μs
    Day 20      798 μs
    Day 21       17 μs
    Day 22   28,369 μs
    Day 23      231 μs
    Day 24    3,241 μs
    Day 25    1,270 μs
    ------------------
    Total   195,145 μs

The input parsing is ugly not because of optimization, but rather as a result of continuing my self-imposed challenge during December to avoid using any Perl modules (or in the case of my C implementation, library functions.)  I later did away with that restriction because it was obscuring the intent of my code too much, but I never did bother going back to rewrite the input parsing.

In some of the solutions (especially where the naive solution is already blindingly fast) I decide to get a little silly.  So when you see that [SIMD](https://en.wikipedia.org/wiki/SIMD) [bitonic sort network](https://en.wikipedia.org/wiki/Bitonic_sorter) in Day 4, I did it because I thought it was interesting, not because it was particularly fast.
