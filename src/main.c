#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include "advent2017.h"

static void load_input(input_t *input, const char *filename);
static void free_input(input_t *input);

int main(int argc, char **argv)
{
	struct timespec t0, t, tn = { };
	for (const advent_t *ap = advent2017; ap->fn; ap++) {
		input_t input;
		load_input(&input, ap->input_file);
		if (input.len == 0) {
			fprintf(stderr, "empty input: %s\n", ap->input_file);
			exit(EXIT_FAILURE);
		}

		clock_gettime(CLOCK_MONOTONIC_RAW, &t0);
		(*ap->fn)(input);
		clock_gettime(CLOCK_MONOTONIC_RAW, &t);

		free_input(&input);

		t.tv_sec -= t0.tv_sec;
		if ((t.tv_nsec -= t0.tv_nsec) < 0) {
			t.tv_nsec += 1000000000L;
			t.tv_sec--;
		}
		tn.tv_sec += t.tv_sec;
		tn.tv_nsec += t.tv_nsec;
		printf("[%ld us]\n", t.tv_sec * 1000000L + t.tv_nsec / 1000L);
	}
	printf("Total: %ld us\n", tn.tv_sec * 1000000L + tn.tv_nsec / 1000L);

	return 0;
}

void load_input(input_t *input, const char *filename)
{
	int fd = open(filename, O_RDONLY);
	if (fd == -1) {
		perror(filename);
		exit(EXIT_FAILURE);
	}
	input->len = lseek(fd, 0, SEEK_END);
	input->s = mmap(NULL, input->len, PROT_READ, MAP_PRIVATE, fd, 0);
	if (input->s == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}
	if (close(fd) == -1) {
		perror(filename);
		exit(EXIT_FAILURE);
	}
}

void free_input(input_t *input)
{
	if (munmap(input->s, input->len) == -1) {
		perror("munmap");
		exit(EXIT_FAILURE);
	}
}
