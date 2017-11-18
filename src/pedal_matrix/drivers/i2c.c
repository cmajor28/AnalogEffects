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

	PRINT("i2c-%d: Initializing device 0x%02X.\n", bus, address);

	// Open i2c bus
	snprintf(filename, sizeof(filename), "/dev/i2c-%d", bus);
	fd = open(filename, O_RDWR);
	if (fd == -1) {
		PRINTE("open() failed!");
		return -1;
	}

	// Set device address
	ret = ioctl(fd, I2C_SLAVE, (int)address);
	if (ret == -1) {
		PRINTE("ioctl() failed!");
		return -1;
	}

	i2c->address = address;
	i2c->bus = bus;
	i2c->fd = fd;

	return 0;
}

int i2c_uninit(struct i2c *i2c) {

	PRINT("i2c-%d: Unnitializing device 0x%02X.\n", i2c->bus, i2c->address);
	close(i2c->fd);
	memset(i2c, 0, sizeof(*i2c));
	return 0;
}

int i2c_read(struct i2c *i2c, void *buffer, unsigned int size) {

	int ret;
	PRINTV("i2c-%d: Reading %u bytes from device 0x%02X.\n", i2c->bus, size, i2c->address);
	ret = read(i2c->fd, buffer, size);
	if (ret != size) {
		PRINTE("read() failed!");
		return -1;
	}

	return 0;
}

int i2c_write(struct i2c *i2c, void *buffer, unsigned int size) {

	int ret;
	PRINTV("i2c-%d: Writing %u bytes to device 0x%02X.\n", i2c->bus, size, i2c->address);
	ret = write(i2c->fd, buffer, size);
	if (ret != size) {
		PRINTE("write() failed!");
		return -1;
	}

	return 0;
}
