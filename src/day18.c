#include "advent2017.h"

typedef struct { int64_t op, a, b; } prog_t;

// Operations common to both parts
#define COMMON_OPS() \
	case 0x00004: r[a] += r[b]; break; \
	case 0x00404: r[a] +=   b ; break; \
	case 0x00005: r[a]  = r[b]; break; \
	case 0x00505: r[a]  =   b ; break; \
	case 0x0000f: r[a] %= r[b]; break; \
	case 0x00f0f: r[a] %=   b ; break; \
	case 0x00015: r[a] *= r[b]; break; \
	case 0x01515: r[a] *=   b ; break; \
	case 0x00007: if (r[a] > 0) p += r[b] - 1; break; \
	case 0x00707: if (r[a] > 0) p +=   b  - 1; break; \
	case 0x70707: if (  a  > 0) p +=   b  - 1; break

static int64_t part1(prog_t *prog, prog_t *e)
{
	int64_t r[256] = { };
	int64_t part1 = 0;

	for (prog_t *p = prog; p >= prog && p < e; p++) {
		int64_t a = p->a, b = p->b;
		switch (p->op) {
		    COMMON_OPS();
		    case 0x00003: if (part1) return part1;
		    case 0x0000e: part1 = r[b]; break;
		    case 0x00e0e: part1 =   b ; break;
		}
	}

	return 0;
}

static int64_t part2(prog_t *prog, prog_t *e)
{
	// 'R'egisters and 'Q'ueues for both programs
	int64_t R[2][256] = { }, Q[2][256];
	// Front/back indices of the queues, which are circular arrays
	uint8_t front[2] = { }, back[2] = { };
	// Instruction pointers
	prog_t *P[2] = { prog, prog };

	// Initialize 'p' register of program 1
	R[1][16] = 1;

	// Switch to program 0
	int64_t *r = R[0];
	prog_t *p = P[0];

	int64_t part2 = 0;
	for (int i = 0; ; p++) {
		// Check if "rcv" with empty queue
		if (p->op == 0x00003 && front[i] == back[i]) {
			// Context switch
			P[i] = p;
			p = P[i = !i];
			r = R[i];

			// Break on deadlock
			if (p->op == 0x00003 && front[i] == back[i]) {
				break;
			}
		}

		int64_t a = p->a, b = p->b;
		switch (p->op) {
		    COMMON_OPS();
		    case 0x00003: r[b] = Q[i][front[i]++];              break;
		    case 0x0000e: Q[!i][back[!i]++] = r[b]; part2 += i; break;
		    case 0x00e0e: Q[!i][back[!i]++] =   b ; part2 += i; break;
		}
	}

	return part2;
}

void day18(input_t in)
{
	prog_t *prog = calloc(in.len, sizeof(*prog)), *e = prog;
	int64_t neg = 0, a = 0, n = 0;

	for (; in.len--; in.s++) {
		if (*in.s == '-') {
			neg = 1;
		} else if (*in.s >= 'a') {
			/* Pack strings into 64-bit integer, and
			 * translate a-z to the numbers 1-26
			 */
			a = (a << 8) | (*in.s - 'a' + 1);
		} else if (*in.s >= '0') {
			n = n * 10 + *in.s - '0';
		} else {
			if (!a) {
				if (neg) n = -n;
				e->a = e->b;
				e->b = n;
				/* Create a unique opcode based on the
				 * count of numeric operands.  Example:
				 *     0x00004 = add r0 r1
				 *     0x00404 = add r0 n
				 * There is ambiguity which doesn't matter:
				 *     0x00707 = jgz r0 n
				 *     0x00707 = jgz n r0  (not used)
				 */
				e->op |= e->op << 8;
			} else if (e->op) {
				e->a = e->b;
				e->b = a;
			} else {
				/* Keep only the second letter of the opcode,
				 * which is distinct
				 */
				e->op = (a >> 8) & 255;
			}
			neg = a = n = 0;
			if (*in.s == '\n') e++;
		}
	}

	printf("Day 18 Part 1: %ld\n", part1(prog, e));
	printf("Day 18 Part 2: %ld\n", part2(prog, e));

	free(prog);
}
