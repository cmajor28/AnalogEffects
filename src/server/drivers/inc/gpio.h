#ifndef GPIO_H_
#define GPIO_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

enum {
	GPIO0,
	GPIO1,
	GPIO2,
	GPIO3
};

#define GPIO0_START_ADDR	0x44E07000
#define GPIO0_END_ADDR		0x44E09000
#define GPIO0_SIZE			(GPIO0_START_ADDR - GPIO0_END_ADDR)

#define GPIO1_START_ADDR	0x4804C000
#define GPIO1_END_ADDR		0x4804E000
#define GPIO1_SIZE			(GPIO1_START_ADDR - GPIO1_END_ADDR)

#define GPIO2_START_ADDR	0x481AC000
#define GPIO2_END_ADDR		0x481AE000
#define GPIO2_SIZE			(GPIO2_START_ADDR - GPIO2_END_ADDR)

#define GPIO3_START_ADDR	0x481AE000
#define GPIO3_END_ADDR		0x481B0000
#define GPIO3_SIZE			(GPIO3_START_ADDR - GPIO3_END_ADDR)

#define GPIO_REVISION			0x0000
#define GPIO_SYSCONFIG			0x0010
#define GPIO_EOI				0x0020
#define GPIO_IRQSTATUS_RAW_0	0x0024
#define GPIO_IRQSTATUS_RAW_1	0x0028
#define GPIO_IRQSTATUS_0		0x002C
#define GPIO_IRQSTATUS_1		0x0030
#define GPIO_IRQSTATUS_SET_0	0x0034
#define GPIO_IRQSTATUS_SET_1	0x0038
#define GPIO_IRQSTATUS_CLR_0	0x003C
#define GPIO_IRQSTATUS_CLR_1	0x0040
#define GPIO_IRQWAKEN_0			0x0044
#define GPIO_IRQWAKEN_1			0x0048
#define GPIO_SYSSTATUS			0x0114
#define GPIO_CTRL				0x0130
#define GPIO_OE					0x0134
#define GPIO_DATAIN				0x0138
#define GPIO_DATAOUT			0x013C
#define GPIO_LEVELDETECT0		0x0140
#define GPIO_LEVELDETECT1		0x0144
#define GPIO_RISINGDETECT		0x0148
#define GPIO_FALLINGDETECT		0x014C
#define GPIO_DEBOUNCENABLE		0x0150
#define GPIO_DEBOUNCINGTIME		0x0154
#define GPIO_CLEARDATAOUT		0x0190
#define GPIO_SETDATAOUT			0x0194

struct gpio {
	volatile void	*mmap_address;
	int				size;
};

int gpio_init(struct gpio *gpio, uint32_t bank);

int gpio_uninit(struct gpio *gpio);

int gpio_set_bit(struct gpio *gpio, uint32_t reg, uint8_t bit, uint8_t set);

int gpio_set_bits(struct gpio *gpio, uint32_t reg, uint32_t bits, uint32_t value);

int gpio_set_value(struct gpio *gpio, uint32_t reg, uint32_t value);

int gpio_get_bit(struct gpio *gpio, uint32_t reg, uint8_t bit, uint8_t *set);

int gpio_get_bits(struct gpio *gpio, uint32_t reg, uint32_t bits, uint32_t *value);

int gpio_get_value(struct gpio *gpio, uint32_t reg, uint32_t *value);

#endif // GPIO_H_
