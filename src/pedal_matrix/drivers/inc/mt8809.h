#ifndef MT8809_H_
#define MT8809_H_

#include "gpio.h"

enum mt8809_pins {
	MT8809_RESET,
	MT8809_CS,
	MT8809_STROBE,
	MT8809_DATA,
	MT8809_AX0,
	MT8809_AX1,
	MT8809_AX2,
	MT8809_AY0,
	MT8809_AY1,
	MT8809_AY2,
	MT8809_PIN_COUNT
};

struct mt8809 {
	struct gpio_pin	pinMap[MT8809_PIN_COUNT];
	uint64_t		switchSet;

};

#define MT8809_PIN_INITIALIZER(	gpio_reset,		pin_reset,	\
								gpio_cs,		pin_cs,		\
								gpio_strobe,	pin_strobe,	\
								gpio_data,		pin_data,	\
								gpio_ax0,		pin_ax0,	\
								gpio_ax1,		pin_ax1,	\
								gpio_ax2,		pin_ax2,	\
								gpio_ay0,		pin_ay0,	\
								gpio_ay1,		pin_ay1,	\
								gpio_ay2,		pin_ay2)	\
								{ \
									.pinMap = { \
										{ .gpio = gpio_reset,	.pin = pin_reset	},	\
										{ .gpio = gpio_cs,		.pin = pin_cs		},	\
										{ .gpio = gpio_strobe,	.pin = pin_strobe	},	\
										{ .gpio = gpio_data,	.pin = pin_data		},	\
										{ .gpio = gpio_ax0,		.pin = pin_ax0		},	\
										{ .gpio = gpio_ax1,		.pin = pin_ax1		},	\
										{ .gpio = gpio_ax2,		.pin = pin_ax2		},	\
										{ .gpio = gpio_ay0,		.pin = pin_ay0		},	\
										{ .gpio = gpio_ay1,		.pin = pin_ay1		},	\
										{ .gpio = gpio_ay2,		.pin = pin_ay2		}	\
									} \
								}

int mt8809_init(struct mt8809 *mt8809);

int mt8809_uninit(struct mt8809 *mt8809);

int mt8809_reset(struct mt8809 *mt8809);

int mt8809_set_switch(struct mt8809 *mt8809, uint8_t address, uint8_t set);

int mt8809_set_switches(struct mt8809 *mt8809, uint64_t switchSet);

int mt8809_get_switch(struct mt8809 *mt8809, uint8_t address, uint8_t *set);

int mt8809_get_switches(struct mt8809 *mt8809, uint64_t *switchSet);

#endif // MT8809_H_
