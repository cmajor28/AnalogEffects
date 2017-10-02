#ifndef I2C_H_
#define I2C_H_

#include "utils.h"

enum i2c_bus {
	I2C0,
	I2C1,
	I2C_COUNT
};

struct i2c {
	enum i2c_bus	bus;
	uint8_t			address;
	int				fd;
};

int i2c_init(struct i2c *i2c, enum i2c_bus bus, uint8_t address);

int i2c_uninit(struct i2c *i2c);

int i2c_read(struct i2c *i2c, void *buffer, unsigned int size);

int i2c_write(struct i2c *i2c, void *buffer, unsigned int size);

#endif // I2C_H_
