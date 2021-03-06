#ifndef GPIO_H_
#define GPIO_H_

#include "utils.h"

#define L4_WKUP_START_ADDR	0x44C00000
#define L4_WKUP_END_ADDR	0x45000000
#define L4_WKUP_SIZE		(L4_WKUP_END_ADDR - L4_WKUP_START_ADDR)

#define L4_WKUP_MODULEMODE_ENABLE_MASK	0x02
#define L4_WKUP_IDLEST_MASK				(0x03 << 16)

#define L4_WKUP_CM_WKUP_OFFSET	(0x44E00400 - L4_WKUP_START_ADDR)
#define L4_WKUP_CM_PER_OFFSET	(0x44E00000 - L4_WKUP_START_ADDR)

#define L4_WKUP_CM_WKUP_GPIO0_CLKCTRL_OFFSET	(L4_WKUP_CM_WKUP_OFFSET + 0x08)
#define L4_WKUP_CM_PER_GPIO1_CLKCTRL_OFFSET		(L4_WKUP_CM_PER_OFFSET + 0xAC)
#define L4_WKUP_CM_PER_GPIO2_CLKCTRL_OFFSET		(L4_WKUP_CM_PER_OFFSET + 0xB0)
#define L4_WKUP_CM_PER_GPIO3_CLKCTRL_OFFSET		(L4_WKUP_CM_PER_OFFSET + 0xB4)

#define GPIO0_START_ADDR	0x44E07000
#define GPIO0_END_ADDR		0x44E09000
#define GPIO0_SIZE			(GPIO0_END_ADDR - GPIO0_START_ADDR)

#define GPIO1_START_ADDR	0x4804C000
#define GPIO1_END_ADDR		0x4804E000
#define GPIO1_SIZE			(GPIO1_END_ADDR - GPIO1_START_ADDR)

#define GPIO2_START_ADDR	0x481AC000
#define GPIO2_END_ADDR		0x481AE000
#define GPIO2_SIZE			(GPIO2_END_ADDR - GPIO2_START_ADDR)

#define GPIO3_START_ADDR	0x481AE000
#define GPIO3_END_ADDR		0x481B0000
#define GPIO3_SIZE			(GPIO3_END_ADDR - GPIO3_START_ADDR)

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

enum gpio_bank {
	GPIO0,
	GPIO1,
	GPIO2,
	GPIO3,
	GPIO_COUNT
};

enum gpio_direction {
	GPIO_DIR_NONE,
	GPIO_DIR_IN,
	GPIO_DIR_OUT
};

enum gpio_sensitivity {
	GPIO_SEN_NONE,
	GPIO_SEN_RISING,
	GPIO_SEN_FALLING,
	GPIO_SEN_BOTH
};

struct gpio {
	volatile void	*mmapAddress;
	unsigned int	size;
	enum gpio_bank	bank;
};

struct gpio_pin {
	struct gpio	*gpio;
	int			pin;
};

struct gpio_irq {
	struct gpio_pin			gpioPin;
	int						(*callback)(void *);
	void					*context;
	enum gpio_direction		direction;
	enum gpio_sensitivity	sensitivity;
	int						debounceTime;
	atomic bool				enabled;
	int						fd;
	pthread_t				intThread;
};

#define GPIO_PIN_INITIALIZER(gpioBank, pinNum) { .gpio = gpioBank, .pin = pinNum }

int gpio_wakeup(bool wake);

int gpio_init(struct gpio *gpio, enum gpio_bank bank);

int gpio_uninit(struct gpio *gpio);

int gpio_set_bit(struct gpio *gpio, uint32_t reg, uint8_t bit, uint8_t set);

int gpio_set_bits(struct gpio *gpio, uint32_t reg, uint32_t bits, uint32_t value);

int gpio_set_value(struct gpio *gpio, uint32_t reg, uint32_t value);

int gpio_set_one_hot(struct gpio *gpio, uint32_t reg, uint8_t bit);

int gpio_get_bit(struct gpio *gpio, uint32_t reg, uint8_t bit, uint8_t *set);

int gpio_get_bits(struct gpio *gpio, uint32_t reg, uint32_t bits, uint32_t *value);

int gpio_get_value(struct gpio *gpio, uint32_t reg, uint32_t *value);

int gpio_irq_init(struct gpio_irq *irq, struct gpio_pin *gpioPin, int (*callback)(void *), void *context, enum gpio_direction direction, enum gpio_sensitivity sensitivity, int debounceTime);

int gpio_irq_uninit(struct gpio_irq *irq);

int gpio_irq_enable(struct gpio_irq *irq, bool enable);

int gpio_irq_is_enabled(struct gpio_irq *irq, bool *enabled);

int gpio_pin_set_value(struct gpio_pin *gpioPin, bool value);

int gpio_pin_get_value(struct gpio_pin *gpioPin, bool *value);

#endif // GPIO_H_
