#ifndef UTILS_H_
#define UTILS_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#define TRUE true
#define FALSE false

#define atomic _Atomic

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))

enum {
	SECONDS      = 1,
	MILLISECONDS = 1000,
	MICROSECONDS = 1000000,
	NANOSECONDS  = 1000000000
};

int sleep_internal(int units, unsigned int time);

#define SLEEP(units, time) sleep_internal(units, time)

int write_to_file(const char *file, void *data, size_t size);

int read_from_file(const char *file, void *data, size_t size);

#endif // UTILS_H_
