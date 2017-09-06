#include "utils.h"

int sleep_internal(int units, unsigned int time) {

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
		return -1;
	}

	count = write(fd, data, size);
	close(fd);

	return (count == size ? 0 : -1);
}

int read_from_file(const char *file, void *data, size_t size) {

	int fd;
	int count;

	fd = open(file, O_RDONLY);
	if (fd == -1) {
		return -1;
	}

	count = read(fd, data, size);
	close(fd);

	return (count == size ? 0 : -1);
}
