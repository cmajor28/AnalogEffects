#ifndef PCF857X_H_
#define PCF857X_H_

#include "gpio.h"
#include "i2c.h"

enum pcf857x_pin_count {
	PCF8574 = 8,
	PCF8575 = 16,
	PCF857X_MAX_PINS = 16
};

struct gpio_ext_irq;

struct gpio_ext {
	struct gpio_pin			gpioPin;
	atomic uint16_t			values;
	struct i2c				*i2c;
	struct gpio_irq			irq;
	enum pcf857x_pin_count	numPins;
	struct gpio_ext_irq     *irqList[PCF857X_MAX_PINS];
};

struct gpio_ext_pin {
	struct gpio_ext	*gpioExt;
	unsigned int	pin;
};

struct gpio_ext_irq {
	struct gpio_ext_pin		gpioExtPin;
	int						(*callback)(void *);
	void					*context;
	enum gpio_direction		direction;
	enum gpio_sensitivity	sensitivity;
	atomic bool				enabled;
};

#define PCF8575_PIN_INITIALIZER(intPin) { .gpioPin = intPin }

#define GPIO_EXT_PIN_INITIALIZER(gpioExtBank, pinNum) { .gpioExt = gpioExtBank, .pin = pinNum }

int gpio_ext_init(struct gpio_ext *gpioExt, enum pcf857x_pin_count numPins, struct i2c *i2c);

int gpio_ext_uninit(struct gpio_ext *gpioExt);

int gpio_ext_set_bit(struct gpio_ext *gpioExt, uint8_t bit, uint8_t set);

int gpio_ext_set_bits(struct gpio_ext *gpioExt, uint16_t bits, uint16_t value);

int gpio_ext_set_value(struct gpio_ext *gpioExt, uint16_t value);

int gpio_ext_get_bit(struct gpio_ext *gpioExt, uint8_t bit, uint8_t *set);

int gpio_ext_get_bits(struct gpio_ext *gpioExt, uint16_t bits, uint16_t *value);

int gpio_ext_get_value(struct gpio_ext *gpioExt, uint16_t *value);

int gpio_ext_irq_init(struct gpio_ext_irq *extIrq, struct gpio_ext_pin *gpioExtPin, int (*callback)(void *), void *context, int direction, int sensitivity);

int gpio_ext_irq_uninit(struct gpio_ext_irq *extIrq);

int gpio_ext_irq_enable(struct gpio_ext_irq *extIrq, bool enable);

int gpio_ext_irq_is_enabled(struct gpio_ext_irq *extIrq, bool *enabled);

int gpio_ext_pin_set_value(struct gpio_ext_pin *gpioExtPin, bool value);

int gpio_ext_pin_get_value(struct gpio_ext_pin *gpioExtPin, bool *value);

#endif // PCF857X_H_
