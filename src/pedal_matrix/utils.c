#include "utils.h"

static int timer_thread_func(struct timer *timer) {

	int ret;

	do {
		// Sleep for timer duration
		ret = sleep2(timer->units, timer->duration);
		if (ret != 0) {
			return ret;
		}

		ret = pthread_mutex_lock(&timer->callbackMutex);
		if (ret != 0) {
			return -1;
		}

		// Fire timer
		timer->enabled = !timer->singleShot;
		timer->callback(timer->context);

		pthread_mutex_unlock(&timer->callbackMutex);
	} while (!timer->singleShot);

	timer->enabled = FALSE;

	return 0;
}

int sleep2(enum time_units units, time_t time) {

	int ret;

	// Select sleep function based on time give
	switch (units) {
	case SECONDS: {
		ret = sleep(time);
		break;
	}
	case MILLISECONDS: {
		ret = usleep(time * MICROSECONDS / MILLISECONDS);
		break;
	}
	case MICROSECONDS: {
		ret = usleep(time);
		break;
	}
	case NANOSECONDS: {
		struct timespec time_struct;
		time_struct.tv_sec = time * SECONDS / NANOSECONDS;
		time_struct.tv_nsec = time * SECONDS % NANOSECONDS;
		ret = nanosleep(&time_struct, NULL);
		break;
	}
	default:
		ret = -1;
		break;
	}

	return ret;
}

int write_to_file(const char *file, void *data, size_t size) {

	int fd;
	int count;

	fd = open(file, O_WRONLY);
	if (fd == -1) {
		PRINTE("open() failed!");
		return -1;
	}

	count = write(fd, data, size);
	close(fd);

	if (count != size) {
		PRINTE("write() failed!");
	}

	return (count == size ? 0 : -1);
}

int read_from_file(const char *file, void *data, size_t size) {

	int fd;
	int count;

	fd = open(file, O_RDONLY);
	if (fd == -1) {
		PRINTE("open() failed!");
		return -1;
	}

	count = read(fd, data, size);
	close(fd);

	if (count != size) {
		PRINTE("read() failed!");
	}

	return (count == size ? 0 : -1);
}

int timer_init(struct timer *timer, bool singleShot, enum time_units units, time_t duration, int (*callback)(void *), void *context) {

	pthread_mutexattr_t attr;

	timer->enabled = FALSE;
	timer->singleShot = singleShot;
	timer->units = units;
	timer->duration = duration;
	timer->callback = callback;
	timer->context = context;

	// Create recursive mutex
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&timer->callbackMutex, &attr);

	return 0;
}

int timer_enable(struct timer *timer, bool enable) {

	int ret;

	if (enable) {
		// Create timer thread
		ret = pthread_create(&timer->timerThread, NULL, (void *(*)(void *))&timer_thread_func, (void *)timer);
		if (ret != 0) {
			PRINTE("pthread_create() failed!");
			return -1;
		}
		ret = pthread_detach(timer->timerThread);
		if (ret != 0) {
			PRINTE("pthread_detach() failed!");
		}
	} else {
		ret = pthread_mutex_lock(&timer->callbackMutex);
		if (ret != 0) {
			PRINTE("pthread_mutex_lock() failed!");
			return -1;
		}
		// Kill timer thread
		ret = pthread_cancel(timer->timerThread);
		pthread_mutex_unlock(&timer->callbackMutex);
		if (ret != 0) {
			PRINTE("pthread_ccancel() failed!");
			return -1;
		}
	}

	timer->enabled = enable;

	return 0;
}

int timer_is_enabled(struct timer *timer, bool *enabled) {

	*enabled = timer->enabled;
	return 0;
}

int timer_uninit(struct timer *timer) {

	if (timer->enabled) pthread_cancel(timer->timerThread);
	pthread_mutex_destroy(&timer->callbackMutex);
	memset(timer, 0, sizeof(*timer));

	return 0;
}
