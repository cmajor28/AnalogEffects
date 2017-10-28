#include "mt8809.h"
#include "utils.h"

static int enable_switches(struct mt8809 *mt8809, bool enable) {

	PRINT("mt8809: %s GPIO outputs.\n", enable ? "Enabling" : "Disabling");

	// Set output enable
	for (int i = 0; i < MT8809_PIN_COUNT; i++) {
		gpio_set_bit(mt8809->pinMap[i].gpio, GPIO_OE, mt8809->pinMap[i].pin, enable ? 0x0 : 0x1);
		gpio_set_one_hot(mt8809->pinMap[i].gpio, i == MT8809_RESET ? GPIO_CLEARDATAOUT : GPIO_SETDATAOUT, mt8809->pinMap[i].pin);
	}

	return 0;
}

static int clear_switches(struct mt8809 *mt8809) {

	PRINT("mt8809: Clearing switch matrix.\n");

	// Set active low reset to 0 (swapped due to level shifter)
	gpio_set_one_hot(mt8809->pinMap[MT8809_RESET].gpio, GPIO_SETDATAOUT, mt8809->pinMap[MT8809_RESET].pin);

	// Minimum 40ns pulse width
	//sleep2(NANOSECONDS, 40);

	// Set active low reset to 1 (swapped due to level shifter)
	gpio_set_one_hot(mt8809->pinMap[MT8809_RESET].gpio, GPIO_CLEARDATAOUT, mt8809->pinMap[MT8809_RESET].pin);

	// Minimum 10ns setup time
	//sleep2(NANOSECONDS, 10);

	// Switches are unset
	mt8809->switchSet = 0x0000000000000000;

	return 0;
}

int mt8809_init(struct mt8809 *mt8809) {

	PRINT("mt8809: Initializing device.\n");

	// Enable gpio outputs
	enable_switches(mt8809, TRUE);

	// Clear gpio outputs
	clear_switches(mt8809);

	return 0;
}

int mt8809_uninit(struct mt8809 *mt8809) {

	PRINT("mt8809: Uninitializing device.\n");

	// Clear gpio outputs
	clear_switches(mt8809);

	// Disable gpio outputs
	enable_switches(mt8809, FALSE);

	memset(mt8809, 0, sizeof(*mt8809));

	return 0;
}

int mt8809_reset(struct mt8809 *mt8809) {

	PRINT("mt8809: Resetting device.\n");

	// Clear gpio outputs
	clear_switches(mt8809);

	return 0;
}

int mt8809_set_switch(struct mt8809 *mt8809, uint8_t address, uint8_t set) {

	PRINT("mt8809: Setting address 0x%02X to %d.\n", address, (int)set);

	// Set active low cs to 0
	gpio_set_one_hot(mt8809->pinMap[MT8809_CS].gpio, GPIO_CLEARDATAOUT, mt8809->pinMap[MT8809_CS].pin);

	// Set data and address lines
	gpio_set_one_hot(mt8809->pinMap[MT8809_DATA].gpio, set ? GPIO_SETDATAOUT : GPIO_CLEARDATAOUT, mt8809->pinMap[MT8809_DATA].pin);
	for (int i = 0, j = MT8809_AX0; j <= MT8809_AY2; i++, j++) {
		bool pin = (address & (1 << i)) ? TRUE : FALSE;
		gpio_set_one_hot(mt8809->pinMap[j].gpio, pin ? GPIO_SETDATAOUT : GPIO_CLEARDATAOUT, mt8809->pinMap[j].pin);
	}

	// Minimum 10ns setup time
	//sleep2(NANOSECONDS, 10);

	// Set active low strobe to 0
	gpio_set_one_hot(mt8809->pinMap[MT8809_STROBE].gpio, GPIO_CLEARDATAOUT, mt8809->pinMap[MT8809_STROBE].pin);

	// Minimum 10ns setup time
	//sleep2(NANOSECONDS, 10);

	// Set active low strobe to 1
	gpio_set_one_hot(mt8809->pinMap[MT8809_STROBE].gpio, GPIO_SETDATAOUT, mt8809->pinMap[MT8809_STROBE].pin);

	// Minimum 10ns setup time
	//sleep2(NANOSECONDS, 10);

	// Set active low cs to 1
	gpio_set_one_hot(mt8809->pinMap[MT8809_CS].gpio, GPIO_SETDATAOUT, mt8809->pinMap[MT8809_CS].pin);

	// Minimum 10ns setup time
	//sleep2(NANOSECONDS, 10);

	mt8809->switchSet = (mt8809->switchSet & ~(1 << address)) | (set << address);

	return 0;
}

int mt8809_set_switches(struct mt8809 *mt8809, uint64_t switchSet) {

	PRINT("mt8809: Setting switches to 0x%016llX\n", (long long unsigned)switchSet);

	// Set active low cs to 0
	gpio_set_one_hot(mt8809->pinMap[MT8809_CS].gpio, GPIO_CLEARDATAOUT, mt8809->pinMap[MT8809_CS].pin);

	for (int i = 0; i < 64; i++) {
		// Set data and address lines
		bool set = (switchSet & (1 << i)) ? TRUE : FALSE;
		gpio_set_one_hot(mt8809->pinMap[MT8809_DATA].gpio, set ? GPIO_SETDATAOUT : GPIO_CLEARDATAOUT, mt8809->pinMap[MT8809_DATA].pin);
		for (int j = 0, k = MT8809_AX0; k <= MT8809_AY2; j++, k++) {
			bool pin = (i & (1 << j)) ? TRUE : FALSE;
			gpio_set_one_hot(mt8809->pinMap[k].gpio, pin ? GPIO_SETDATAOUT : GPIO_CLEARDATAOUT, mt8809->pinMap[k].pin);
		}

		// Minimum 10ns setup time
		//sleep2(NANOSECONDS, 10);

		// Set active low strobe to 0
		gpio_set_one_hot(mt8809->pinMap[MT8809_STROBE].gpio, GPIO_CLEARDATAOUT, mt8809->pinMap[MT8809_STROBE].pin);

		// Minimum 10ns setup time
		//sleep2(NANOSECONDS, 10);

		// Set active low strobe to 1
		gpio_set_one_hot(mt8809->pinMap[MT8809_STROBE].gpio, GPIO_SETDATAOUT, mt8809->pinMap[MT8809_STROBE].pin);

		// Minimum 10ns setup time
		//sleep2(NANOSECONDS, 10);
	}

	// Set active low cs to 1
	gpio_set_one_hot(mt8809->pinMap[MT8809_CS].gpio, GPIO_SETDATAOUT, mt8809->pinMap[MT8809_CS].pin);

	// Minimum 10ns setup time
	//sleep2(NANOSECONDS, 10);

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
