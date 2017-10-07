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
#include <sys/mman.h>

#define TRUE true
#define FALSE false

#define atomic _Atomic

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) < (b) ? (b) : (a))

#define log2(val) (8*sizeof(val) - __builtin_clzll(val) - 1)
#define pow2(val) ((uint64_t)1 << (val))

#define PRINT(fmt, ...) fprintf(stderr, fmt,##__VA_ARGS__)

enum time_units {
	SECONDS      = 1,
	MILLISECONDS = 1000,
	MICROSECONDS = 1000000,
	NANOSECONDS  = 1000000000
};

int sleep2(enum time_units units, time_t time);

int write_to_file(const char *file, void *data, size_t size);

int read_from_file(const char *file, void *data, size_t size);

struct timer {
	atomic bool		enabled;
	bool			singleShot;
	pthread_t		timerThread;
	pthread_mutex_t	callbackMutex;
	enum time_units	units;
	time_t			duration;
	int				(*callback)(void *);
	void			*context;
};

int timer_init(struct timer *timer, bool singleShot, enum time_units units, time_t duration, int (*callback)(void *), void *context);

int timer_enable(struct timer *timer, bool enable);

int timer_is_enabled(struct timer *timer, bool *enabled);

int timer_uninit(struct timer *timer);

#endif // UTILS_H_
