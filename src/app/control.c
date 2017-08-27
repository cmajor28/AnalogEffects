#include "control.h"
#include "utils.h"
#include "gpio.h"
#include "pcf857x.h"
#include "mt8809.h"
#include <stdio.h>

bool gPresenceDetect[AE_MAX_EFFECTS+1][2];
FILE *gConfigFile;
pthread_mutex_t gConfigMutex;
struct ae_config gConfig;

extern struct pthread_mutex_t gPresetsMutex;
extern struct ae_preset gPresets[AE_BANK_COUNT][AE_PRESET_COUNT];

struct gpio gGPIOBanks[GPIO_COUNT];
struct gpio_ext gGPIOExtBanks[3];
struct mt8809 gSwitchMatrix;
struct gpio_ext_pin gBypassSwitch;
struct gpio_ext_pin gLedOutputs[AE_MAX_EFFECTS+3];
struct gpio_ext_pin gPresenceInputs[AE_MAX_EFFECTS+1][2];
struct gpio_ext_pin gButtonsInputs[AE_MAX_EFFECTS+3];

static int get_last_configuration(struct ae_config *lastConfig) {

	int count;

	count = fread(lastConfig, sizeof(*lastConfig), 1, gConfigFile);
	if (count != sizeof(*lastConfig)) {
		return -1;
	}

	return 0;
}

static int set_last_configuration(struct ae_config *lastConfig) {

	int count;

	count = fwrite(lastConfig, sizeof(*lastConfig), 1, gConfigFile);
	if (count != sizeof(*lastConfig)) {
		return -1;
	}

	return 0;
}

static int apply_bypass(bool bypass, struct gpio_ext_pin *bypassSwitch) {

	int ret;
	ret = gpio_ext_pin_set_value(bypassSwitch, bypass);
	return ret;
}

static int apply_switch_configuration(int pedalOrder[AE_MAX_EFFECTS], bool enabled[AE_MAX_EFFECTS], bool presenceDetect[AE_MAX_EFFECTS+1][2], struct mt8809 *mt8809) {

	int ret;
	uint64_t switchSet = 0;
	int lastInput = 0;
	int pedal;
	int address;

	// Route each input to an output
	for (int i = 0; i < AE_MAX_EFFECTS; i++) {
		pedal = pedalOrder[i];
		if (pedal > 0) { // If pedal exists
			if (enabled[pedal]) { // If enabled
				if (presenceDetect[pedal-1][OUT] && presenceDetect[pedal][IN]) { // If presence detected
					address = lastInput + ((pedal - 1) << 3); // Get crosspoint address for last pedal input and new pedal output
					switchSet |= ((uint64_t)1 << address);
					lastInput = pedal;
				}
			}
		}
	}

	// Route last input to output
	switchSet |= ((uint64_t)1 << (0x38 | lastInput));

	// Set switches on mt8809
	ret = mt8809_set_switches(mt8809, switchSet);
	if (ret != 0) {
		return -1;
	}

	return 0;
}

int control_init() {

	int ret;

	// Initialize hardware
	// TODO

	// Open config file and leave it open
	gConfigFile = fopen(CONFIG_FILE, "rw+");
	if (gConfigFile == NULL) {
		return -1;
	}

	// Read last configuration from file
	ret = get_last_configuration(&gConfig);
	if (ret != 0) {
		fclose(gConfigFile);
		return -1;
	}

	// Apply configuration
	if (gConfig.pedalMode) {
		// Apply bypass
		ret = apply_bypass(gConfig.bypassEnabled, &gBypassSwitch);
		if (ret != 0) {
			return -1;
		}

		// Apply switch configuration
		ret = apply_switch_configuration(gConfig.currPreset.pedalOrder, gConfig.enabled, gPresenceDetect, &gSwitchMatrix);
		if (ret != 0) {
			return -1;
		}
	} else {
		// Disable bypass
		ret = apply_bypass(FALSE, NULL);
		if (ret != 0) {
			return -1;
		}

		// Apply switch configuration
		ret = apply_switch_configuration(gConfig.currPreset.pedalOrder, gConfig.currPreset.enabled, gPresenceDetect, &gSwitchMatrix);
		if (ret != 0) {
			return -1;
		}
	}

	return 0;
}

int control_uninit() {

	fclose(gConfigFile);
	memset(&gConfig, 0, sizeof(gConfig));

	// Uninitialize hardware
	// TODO

	return 0;
}
