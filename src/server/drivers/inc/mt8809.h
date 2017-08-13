#ifndef MT8809_H_
#define MT8809_H_

#include "gpio.h"

struct mt8809_pin_map {
	uint8_t	reset;
	uint8_t	cs;
	uint8_t	strobe;
	uint8_t data;
	uint8_t	ax0;
	uint8_t	ax1;
	uint8_t	ax2;
	uint8_t	ay0;
	uint8_t	ay1;
	uint8_t	ay2;
};

struct mt8809 {
	struct gpio				*gpio_bank;
	struct mt8809_pin_map	pin_map;
	uint64_t				switch_set;

};

int mt8809_init(struct mt8809 *mt8809, struct gpio *gpio, struct mt8809_pin_map *pin_map);

int mt8809_uninit(struct mt8809 *mt8809);

int mt8809_reset(struct mt8809 *mt8809);

int mt8809_set_switch(struct mt8809 *mt8809, uint8_t address, uint8_t set);

int mt8809_set_switches(struct mt8809 *mt8809, uint64_t switch_set);

int mt8809_get_switch(struct mt8809 *mt8809, uint8_t address, uint8_t *set);

int mt8809_get_switches(struct mt8809 *mt8809, uint64_t *switch_set);

#endif // MT8809_H_
