#include "mt8809.h"
#include "utils.h"

static int enable_switches(struct mt8809 *mt8809, bool enable) {

	// Get gpio pin bitmap
	uint32_t pinmap  = ((1 << mt8809->pinMap.ax0) |
						(1 << mt8809->pinMap.ax1) |
						(1 << mt8809->pinMap.ax2) |
						(1 << mt8809->pinMap.ay0) |
						(1 << mt8809->pinMap.ay1) |
						(1 << mt8809->pinMap.ay2) |
						(1 << mt8809->pinMap.cs) |
						(1 << mt8809->pinMap.strobe) |
						(1 << mt8809->pinMap.reset) |
						(1 << mt8809->pinMap.data));

	// Set OE to 1 for each pin
	gpio_set_bits(mt8809->gpioBank, GPIO_OE, pinmap, enable ? 0xFFFFFFFF : 0x0000000);

	return 0;
}

static int clear_switches(struct mt8809 *mt8809) {

	// Set active low reset to 0
	gpio_set_bit(mt8809->gpioBank, GPIO_SETDATAOUT, mt8809->pinMap.reset, 0);

	// Minimum 40ns pulse width
	SLEEP(NANOSECONDS, 40);

	// Set active low reset to 1
	gpio_set_bit(mt8809->gpioBank, GPIO_SETDATAOUT, mt8809->pinMap.reset, 1);

	// Switches are unset
	mt8809->switchSet = 0x0000000000000000;

	return 0;
}

int mt8809_init(struct mt8809 *mt8809, struct gpio *gpio, struct mt8809_pin_map *pinMap) {

	mt8809->gpioBank = gpio;
	memcpy(&mt8809->pinMap, pinMap, sizeof(mt8809->pinMap));

	// Enable gpio outputs
	enable_switches(mt8809, TRUE);

	// Clear gpio outputs
	clear_switches(mt8809);

	return 0;
}

int mt8809_uninit(struct mt8809 *mt8809) {

	// Clear gpio outputs
	clear_switches(mt8809);

	// Disable gpio outputs
	enable_switches(mt8809, FALSE);

	memset(mt8809, 0, sizeof(*mt8809));

	return 0;
}

int mt8809_reset(struct mt8809 *mt8809) {

	// Clear gpio outputs
	clear_switches(mt8809);

	return 0;
}

int mt8809_set_switch(struct mt8809 *mt8809, uint8_t address, uint8_t set) {

	// Get gpio pin bitmap
	uint32_t pinmap  = ((1 << mt8809->pinMap.ax0) |
						(1 << mt8809->pinMap.ax1) |
						(1 << mt8809->pinMap.ax2) |
						(1 << mt8809->pinMap.ay0) |
						(1 << mt8809->pinMap.ay1) |
						(1 << mt8809->pinMap.ay2) |
						(1 << mt8809->pinMap.data));

	// Get gpio pin bitmap values
	uint32_t values  = (((address & 0x01) ? (1 << mt8809->pinMap.ax0) : 0x0) |
						((address & 0x02) ? (1 << mt8809->pinMap.ax1) : 0x0) |
						((address & 0x04) ? (1 << mt8809->pinMap.ax2) : 0x0) |
						((address & 0x08) ? (1 << mt8809->pinMap.ay0) : 0x0) |
						((address & 0x10) ? (1 << mt8809->pinMap.ay1) : 0x0) |
						((address & 0x20) ? (1 << mt8809->pinMap.ay2) : 0x0) |
						((set) ? (1 << mt8809->pinMap.data) : 0x0));

	// Set active low cs to 0
	gpio_set_bit(mt8809->gpioBank, GPIO_SETDATAOUT, mt8809->pinMap.cs, 0);

	// Minimum 10ns setup time
	SLEEP(NANOSECONDS, 10);

	// Set active low strobe to 0
	gpio_set_bit(mt8809->gpioBank, GPIO_SETDATAOUT, mt8809->pinMap.strobe, 0);

	// Minimum 10ns setup time
	SLEEP(NANOSECONDS, 10);

	// Set data and address lines
	gpio_set_bits(mt8809->gpioBank, GPIO_SETDATAOUT, pinmap, values);

	// Minimum 10ns setup time
	SLEEP(NANOSECONDS, 10);

	// Set active low strobe to 1
	gpio_set_bit(mt8809->gpioBank, GPIO_SETDATAOUT, mt8809->pinMap.strobe, 1);

	// Minimum 10ns setup time
	SLEEP(NANOSECONDS, 10);

	// Set active low cs to 1
	gpio_set_bit(mt8809->gpioBank, GPIO_SETDATAOUT, mt8809->pinMap.cs, 1);

	return 0;
}

int mt8809_set_switches(struct mt8809 *mt8809, uint64_t switchSet) {

	// Get gpio pin bitmap
	uint32_t pinmap  = ((1 << mt8809->pinMap.ax0) |
						(1 << mt8809->pinMap.ax1) |
						(1 << mt8809->pinMap.ax2) |
						(1 << mt8809->pinMap.ay0) |
						(1 << mt8809->pinMap.ay1) |
						(1 << mt8809->pinMap.ay2) |
						(1 << mt8809->pinMap.data));

	uint32_t values;

	// Set active low cs to 0
	gpio_set_bit(mt8809->gpioBank, GPIO_SETDATAOUT, mt8809->pinMap.cs, 0);

	// Minimum 10ns setup time
	SLEEP(NANOSECONDS, 10);

	for (size_t i = 0; i < 64; i++) {

		// Get gpio pin bitmap values
		values = (((i & 0x01) ? (1 << mt8809->pinMap.ax0) : 0x0) |
                  ((i & 0x02) ? (1 << mt8809->pinMap.ax1) : 0x0) |
                  ((i & 0x04) ? (1 << mt8809->pinMap.ax2) : 0x0) |
                  ((i & 0x08) ? (1 << mt8809->pinMap.ay0) : 0x0) |
                  ((i & 0x10) ? (1 << mt8809->pinMap.ay1) : 0x0) |
                  ((i & 0x20) ? (1 << mt8809->pinMap.ay2) : 0x0) |
                  ((switchSet & (1 << i)) ? (1 << mt8809->pinMap.data) : 0x0));

		// Set active low strobe to 0
		gpio_set_bit(mt8809->gpioBank, GPIO_SETDATAOUT, mt8809->pinMap.strobe, 0);

		// Minimum 10ns setup time
		SLEEP(NANOSECONDS, 10);

		// Set data and address lines
		gpio_set_bits(mt8809->gpioBank, GPIO_SETDATAOUT, pinmap, values);

		// Minimum 10ns setup time
		SLEEP(NANOSECONDS, 10);

		// Set active low strobe to 1
		gpio_set_bit(mt8809->gpioBank, GPIO_SETDATAOUT, mt8809->pinMap.strobe, 1);

		// Minimum 10ns setup time
		SLEEP(NANOSECONDS, 10);
	}

	// Set active low cs to 1
	gpio_set_bit(mt8809->gpioBank, GPIO_SETDATAOUT, mt8809->pinMap.cs, 1);

	mt8809->switchSet = switchSet;

	return 0;
}

int mt8809_get_switch(struct mt8809 *mt8809, uint8_t address, uint8_t *set) {

	*set = (mt8809->switchSet & (1 << address)) ? 0x1 : 0x0;

	return 0;
}

int mt8809_get_switches(struct mt8809 *mt8809, uint64_t *switchSet) {

	*switchSet = mt8809->switchSet;

	return 0;
}
