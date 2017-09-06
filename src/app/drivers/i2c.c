#include "i2c.h"
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int i2c_init(struct i2c *i2c, enum i2c_bus bus, uint8_t address) {

	int ret;
	int fd;
	char filename[12];

	// Open i2c bus
	snprintf(filename, sizeof(filename), "/dev/i2c-%d", bus);
	fd = open(filename, O_RDWR);
	if (fd == -1) {
		return -1;
	}

	// Set device address
	ret = ioctl(fd, I2C_SLAVE, (int)address);
	if (ret == -1) {
		return -1;
	}

	i2c->address = address;
	i2c->bus = bus;
	i2c->fd = fd;

	return 0;
}

int i2c_uninit(struct i2c *i2c) {

	close(i2c->fd);
	memset(i2c, 0, sizeof(*i2c));
	return 0;
}

int i2c_read(struct i2c *i2c, void *buffer, unsigned int size) {

	int ret;
	ret = read(i2c->fd, buffer, size);
	if (ret != size) {
		return -1;
	}

	return 0;
}

int i2c_write(struct i2c *i2c, void *buffer, unsigned int size) {

	int ret;
	ret = write(i2c->fd, buffer, size);
	if (ret != size) {
		return -1;
	}

	return 0;
}
