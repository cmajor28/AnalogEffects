#include "mt8809.h"
#include "utils.h"

static int enable_switches(struct mt8809 *mt8809, bool enable) {

	// Get gpio pin bitmap
	uint32_t pinmap  = ((1 << mt8809->pin_map.ax0) |
						(1 << mt8809->pin_map.ax1) |
						(1 << mt8809->pin_map.ax2) |
						(1 << mt8809->pin_map.ay0) |
						(1 << mt8809->pin_map.ay1) |
						(1 << mt8809->pin_map.ay2) |
						(1 << mt8809->pin_map.cs) |
						(1 << mt8809->pin_map.strobe) |
						(1 << mt8809->pin_map.reset) |
						(1 << mt8809->pin_map.data));

	gpio_set_bits(mt8809->gpio_bank, GPIO_OE, pinmap, enable ? 0xFFFFFFFF : 0x0000000);

	return 0;
}

static int clear_switches(struct mt8809 *mt8809) {

	gpio_set_bit(mt8809->gpio_bank, GPIO_SETDATAOUT, 1 << mt8809->pin_map.reset, 0);

	SLEEP(NANOSECONDS, 40);

	gpio_set_bit(mt8809->gpio_bank, GPIO_SETDATAOUT, 1 << mt8809->pin_map.reset, 1);

	mt8809->switch_set = 0x0000000000000000;

	return 0;
}

int mt8809_init(struct mt8809 *mt8809, struct gpio *gpio, struct mt8809_pin_map *pin_map) {

	if (mt8809 == NULL || gpio == NULL || pin_map == NULL) {
		// TODO
	}

	mt8809->gpio_bank = gpio;
	memcpy(&mt8809->pin_map, pin_map, sizeof(mt8809->pin_map));

	// Enable gpio outputs
	enable_switches(mt8809, TRUE);

	// Clear gpio outputs
	clear_switches(mt8809);

	return 0;
}

int mt8809_uninit(struct mt8809 *mt8809) {

	if (mt8809 == NULL) {
		// TODO
	}

	clear_switches(mt8809);

	memset(mt8809, 0, sizeof(*mt8809));

	return 0;
}

int mt8809_reset(struct mt8809 *mt8809) {

	if (mt8809 == NULL) {
		// TODO
	}

	clear_switches(mt8809);

	return 0;
}

int mt8809_set_switch(struct mt8809 *mt8809, uint8_t address, uint8_t set) {

	if (mt8809 == NULL) {
		// TODO
	}

	// Get gpio pin bitmap
	uint32_t pinmap  = ((1 << mt8809->pin_map.ax0) |
						(1 << mt8809->pin_map.ax1) |
						(1 << mt8809->pin_map.ax2) |
						(1 << mt8809->pin_map.ay0) |
						(1 << mt8809->pin_map.ay1) |
						(1 << mt8809->pin_map.ay2) |
						(1 << mt8809->pin_map.data));

	// Get gpio pin bitmap values
	uint32_t values  = (((address & 0x20) ? (1 << mt8809->pin_map.ax0) : 0x0) |
						((address & 0x10) ? (1 << mt8809->pin_map.ax1) : 0x0) |
						((address & 0x08) ? (1 << mt8809->pin_map.ax2) : 0x0) |
						((address & 0x04) ? (1 << mt8809->pin_map.ay0) : 0x0) |
						((address & 0x02) ? (1 << mt8809->pin_map.ay1) : 0x0) |
						((address & 0x01) ? (1 << mt8809->pin_map.ay2) : 0x0) |
						((set) ? (1 << mt8809->pin_map.data) : 0x0));

	gpio_set_bit(mt8809->gpio_bank, GPIO_SETDATAOUT, 1 << mt8809->pin_map.cs, 0);

	SLEEP(NANOSECONDS, 10);

	gpio_set_bit(mt8809->gpio_bank, GPIO_SETDATAOUT, 1 << mt8809->pin_map.strobe, 0);

	SLEEP(NANOSECONDS, 10);

	gpio_set_bits(mt8809->gpio_bank, GPIO_SETDATAOUT, pinmap, values);

	SLEEP(NANOSECONDS, 10);

	gpio_set_bit(mt8809->gpio_bank, GPIO_SETDATAOUT, 1 << mt8809->pin_map.strobe, 1);

	SLEEP(NANOSECONDS, 10);

	gpio_set_bit(mt8809->gpio_bank, GPIO_SETDATAOUT, 1 << mt8809->pin_map.cs, 1);

	return 0;
}

int mt8809_set_switches(struct mt8809 *mt8809, uint64_t switch_set) {

	if (mt8809 == NULL) {
		// TODO
	}

	// Get gpio pin bitmap
	uint32_t pinmap  = ((1 << mt8809->pin_map.ax0) |
						(1 << mt8809->pin_map.ax1) |
						(1 << mt8809->pin_map.ax2) |
						(1 << mt8809->pin_map.ay0) |
						(1 << mt8809->pin_map.ay1) |
						(1 << mt8809->pin_map.ay2) |
						(1 << mt8809->pin_map.data));

	uint32_t values;

	gpio_set_bit(mt8809->gpio_bank, GPIO_SETDATAOUT, 1 << mt8809->pin_map.cs, 0);

	SLEEP(NANOSECONDS, 10);

	for (size_t i = 0; i < 64; i++) {

		// Get gpio pin bitmap values
		values = (((i & 0x20) ? (1 << mt8809->pin_map.ax0) : 0x0) |
                  ((i & 0x10) ? (1 << mt8809->pin_map.ax1) : 0x0) |
                  ((i & 0x08) ? (1 << mt8809->pin_map.ax2) : 0x0) |
                  ((i & 0x04) ? (1 << mt8809->pin_map.ay0) : 0x0) |
                  ((i & 0x02) ? (1 << mt8809->pin_map.ay1) : 0x0) |
                  ((i & 0x01) ? (1 << mt8809->pin_map.ay2) : 0x0) |
                  ((switch_set & (1 << i)) ? (1 << mt8809->pin_map.data) : 0x0));

		gpio_set_bit(mt8809->gpio_bank, GPIO_SETDATAOUT, 1 << mt8809->pin_map.strobe, 0);

		SLEEP(NANOSECONDS, 10);

		gpio_set_bits(mt8809->gpio_bank, GPIO_SETDATAOUT, pinmap, values);

		SLEEP(NANOSECONDS, 10);

		gpio_set_bit(mt8809->gpio_bank, GPIO_SETDATAOUT, 1 << mt8809->pin_map.strobe, 1);

		SLEEP(NANOSECONDS, 10);
	}

	gpio_set_bit(mt8809->gpio_bank, GPIO_SETDATAOUT, 1 << mt8809->pin_map.cs, 1);

	mt8809->switch_set = switch_set;

	return 0;
}

int mt8809_get_switch(struct mt8809 *mt8809, uint8_t address, uint8_t *set) {

	if (mt8809 == NULL || set == NULL) {
		// TODO
	}

	*set = (mt8809->switch_set & (1 << address)) ? 0x01 : 0x0;

	return 0;
}

int mt8809_get_switches(struct mt8809 *mt8809, uint64_t *switch_set) {

	if (mt8809 == NULL || switch_set == NULL) {
		// TODO
	}

	*switch_set = mt8809->switch_set;

	return 0;
}
