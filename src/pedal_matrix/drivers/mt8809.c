#include "mt8809.h"
#include "utils.h"

static int enable_switches(struct mt8809 *mt8809, bool enable) {

	// Set OE to 1 for each pin
	for (int i = 0; i < MT8809_PIN_COUNT; i++) {
		gpio_set_bit(mt8809->pinMap[i].gpio, GPIO_OE, mt8809->pinMap[i].pin, enable ? 0x1 : 0x0);
	}
	return 0;
}

static int clear_switches(struct mt8809 *mt8809) {

	// Set active low reset to 0
	gpio_set_bit(mt8809->pinMap[MT8809_RESET].gpio, GPIO_SETDATAOUT, mt8809->pinMap[MT8809_RESET].pin, 0x0);

	// Minimum 40ns pulse width
	sleep2(MICROSECONDS, 40);

	// Set active low reset to 1
	gpio_set_bit(mt8809->pinMap[MT8809_RESET].gpio, GPIO_SETDATAOUT, mt8809->pinMap[MT8809_RESET].pin, 0x1);

	// Switches are unset
	mt8809->switchSet = 0x0000000000000000;

	return 0;
}

int mt8809_init(struct mt8809 *mt8809) {

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

	// Set active low cs to 0
	gpio_set_bit(mt8809->pinMap[MT8809_CS].gpio, GPIO_SETDATAOUT, mt8809->pinMap[MT8809_CS].pin, 0x0);

	// Minimum 10ns setup time
	sleep2(MICROSECONDS, 10);

	// Set active low strobe to 0
	gpio_set_bit(mt8809->pinMap[MT8809_STROBE].gpio, GPIO_SETDATAOUT, mt8809->pinMap[MT8809_STROBE].pin, 0x0);

	// Minimum 10ns setup time
	sleep2(MICROSECONDS, 10);

	// Set data and address lines
	gpio_set_bit(mt8809->pinMap[MT8809_DATA].gpio, GPIO_SETDATAOUT, mt8809->pinMap[MT8809_DATA].pin, set);
	for (int i = 0, j = MT8809_AX0; j <= MT8809_AY2; i++, j++) {
		gpio_set_bit(mt8809->pinMap[j].gpio, GPIO_SETDATAOUT, mt8809->pinMap[j].pin, (address & (1 << i)) ? 0x1 : 0x0);
	}

	// Minimum 10ns setup time
	sleep2(MICROSECONDS, 10);

	// Set active low strobe to 1
	gpio_set_bit(mt8809->pinMap[MT8809_STROBE].gpio, GPIO_SETDATAOUT, mt8809->pinMap[MT8809_STROBE].pin, 0x1);

	// Minimum 10ns setup time
	sleep2(MICROSECONDS, 10);

	// Set active low cs to 1
	gpio_set_bit(mt8809->pinMap[MT8809_CS].gpio, GPIO_SETDATAOUT, mt8809->pinMap[MT8809_CS].pin, 0x1);

	return 0;
}

int mt8809_set_switches(struct mt8809 *mt8809, uint64_t switchSet) {

	// Set active low cs to 0
	gpio_set_bit(mt8809->pinMap[MT8809_CS].gpio, GPIO_SETDATAOUT, mt8809->pinMap[MT8809_CS].pin, 0x0);

	// Minimum 10ns setup time
	sleep2(MICROSECONDS, 10);

	for (int i = 0; i < 64; i++) {
		// Set active low strobe to 0
		gpio_set_bit(mt8809->pinMap[MT8809_STROBE].gpio, GPIO_SETDATAOUT, mt8809->pinMap[MT8809_STROBE].pin, 0x0);

		// Minimum 10ns setup time
		sleep2(MICROSECONDS, 10);

		// Set data and address lines
		gpio_set_bit(mt8809->pinMap[MT8809_DATA].gpio, GPIO_SETDATAOUT, mt8809->pinMap[MT8809_DATA].pin, (switchSet & (1 << i)) ? 0x1 : 0x0);
		for (int j = 0, k = MT8809_AX0; k <= MT8809_AY2; j++, k++) {
			gpio_set_bit(mt8809->pinMap[k].gpio, GPIO_SETDATAOUT, mt8809->pinMap[k].pin, (i & (1 << j)) ? 0x1 : 0x0);
		}

		// Minimum 10ns setup time
		sleep2(MICROSECONDS, 10);

		// Set active low strobe to 1
		gpio_set_bit(mt8809->pinMap[MT8809_STROBE].gpio, GPIO_SETDATAOUT, mt8809->pinMap[MT8809_STROBE].pin, 0x1);

		// Minimum 10ns setup time
		sleep2(MICROSECONDS, 10);
	}

	// Set active low cs to 1
	gpio_set_bit(mt8809->pinMap[MT8809_CS].gpio, GPIO_SETDATAOUT, mt8809->pinMap[MT8809_CS].pin, 0x1);

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