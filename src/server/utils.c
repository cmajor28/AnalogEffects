#include "utils.h"

int sleep_internal(int units, unsigned int time) {

	int ret;
	switch (units) {
	case SECONDS: {
		ret = sleep(time);
		break;
	}
	case MILLISECONDS: {
		ret = usleep(time * MICROSECONDS / MILLISECONDS)
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
