#include "control.h"
#include "utils.h"
#include "pins.h"

// Global configuration variables
bool gPresenceDetect[AE_MAX_EFFECTS][2];
pthread_mutex_t gConfigMutex;
struct ae_config *gConfig; // mmap() into non-volatile memory

// Global preset list
extern pthread_mutex_t gPresetsMutex;
extern struct ae_preset gPresets[AE_BANK_COUNT][AE_PRESET_COUNT];

// Global GPIO banks
struct gpio gGPIOBanks[GPIO_COUNT] = { 0 };
struct gpio_ext gGPIOExtBanks[GPIO_EXT_COUNT] = { GPIO_EXT_LEDS_INIT(), GPIO_EXT_PRESENCE_INIT(), GPIO_EXT_BUTTONS_INIT() };

// Global device interfaces
struct mt8809 gSwitchMatrix = MT8809_PINS_INIT();
struct led_control gLedControl = LED_PINS_INIT();
struct presence_control gPresenceControl = PRESENCE_PINS_INIT();
struct button_control gButtonControl = BUTTON_PINS_INIT();
struct gpio_pin gBypassSwitch = BYPASS_PINS_INIT();
struct gpio_pin gControlSwitch[2] = CONTROL_PINS_INIT();

// I2C interfaces
struct i2c gI2C[GPIO_EXT_COUNT];

static int leds_set_one_hot(enum ae_led led) {

	int ret;
	bool ledOneHot[AE_LED_COUNT] = { 0 };
	ledOneHot[led] = TRUE;
	ret = led_control_set_all(&gLedControl, ledOneHot);
	return ret;
}

static int leds_set(enum ae_led led, bool value) {

	int ret;
	ret = led_control_set(&gLedControl, led, value);
	return ret;
}

static int apply_bypass(bool bypass) {

	int ret;
	ret = gpio_pin_set_value(&gBypassSwitch, !bypass); // Bypass active low
	return ret;
}

static int apply_control(bool control[2]) {

	int ret = 0;
	ret |= gpio_pin_set_value(&gControlSwitch[TIP], !control[TIP]); // Control is active low
	ret |= gpio_pin_set_value(&gControlSwitch[RING], !control[RING]); // Control is active low
	return ret;
}

static int apply_switch_configuration(int pedalOrder[AE_MAX_EFFECTS], bool enabled[AE_MAX_EFFECTS], bool presenceDetect[AE_MAX_EFFECTS][2], bool mute) {

	static uint64_t lastSwitchSet = 0;
	static bool lastMute = FALSE;

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
				if (presenceDetect[pedal][OUT] && presenceDetect[pedal][IN]) { // If presence detected
					address = lastInput + (pedal << 3); // Get crosspoint address for last pedal input and new pedal output
					switchSet |= ((uint64_t)1 << address);
					lastInput = pedal;
				}
			}
		}
	}

	// Route last input to output
	switchSet |= ((uint64_t)1 << lastInput);

	// Calculate switch sets with input disconnected for mute
	uint64_t switchSetNoInput = switchSet & 0x0101010101010101;
	uint64_t lastSwitchSetNoInput = lastSwitchSet & 0x0101010101010101;

	if (lastMute && !mute && (switchSetNoInput == lastSwitchSetNoInput)) { // We are unmuting only
		// Route input on mt8809
		ret = mt8809_set_switch(&gSwitchMatrix, log2(switchSetNoInput), 0x1);
		if (ret != 0) {
			return ret;
		}
	} else if (!lastMute && mute && (switchSetNoInput == lastSwitchSetNoInput)) { // We are muting only
		// Unroute input on mt8809
		ret = mt8809_set_switch(&gSwitchMatrix, log2(lastSwitchSetNoInput), 0x0);
		if (ret != 0) {
			return ret;
		}
	} else if (switchSet != lastSwitchSet) { // Only program the switch if a change has been made
		// Apply mute to switch set
		if (mute) {
			switchSet = switchSetNoInput;
		}

		// Reset switch (break-before make)
		mt8809_reset(&gSwitchMatrix);

		// Set switches on mt8809
		ret = mt8809_set_switches(&gSwitchMatrix, switchSet);
		if (ret != 0) {
			return ret;
		}
	}

	lastMute = mute;
	lastSwitchSet = switchSet;

	return 0;
}

static int load_preset(int bank, int preset) {

	int ret;

	// Lock preset mutex
	ret = pthread_mutex_lock(&gPresetsMutex);
	if (ret != 0) {
		return -1;
	}

	// Load preset into config
	memcpy(&gConfig->currPreset, &gPresets[bank][preset], sizeof(gConfig->currPreset));
	pthread_mutex_unlock(&gPresetsMutex);

	// Apply the preset to hardware
	ret = apply_switch_configuration(gConfig->currPreset.pedalOrder, gConfig->currPreset.enabled, gPresenceDetect, gConfig->muteEnabled);
	ret |= apply_control(gConfig->currPreset.controlEnabled);

	return ret;
}

static int handle_button_pressed_event(enum ae_button button) {

	int ret;

	if (gConfig->pedalMode) {
		switch (button) {
		case AE_BUTTON_B1:
		case AE_BUTTON_B2:
		case AE_BUTTON_B3:
		case AE_BUTTON_B4:
		case AE_BUTTON_B5:
		case AE_BUTTON_B6:
		case AE_BUTTON_B7:
		case AE_BUTTON_B8:
			// Toggle pedal enabled
			gConfig->currPreset.enabled[button] = !gConfig->currPreset.enabled[button];
			leds_set(button, gConfig->currPreset.enabled[button]);
			ret = apply_switch_configuration(gConfig->currPreset.pedalOrder, gConfig->currPreset.enabled, gPresenceDetect, gConfig->muteEnabled);
			if (ret != 0) {
				return -1;
			}

			break;
		case AE_BUTTON_BU:
		case AE_BUTTON_BD:
			// Toggle control enabled
			gConfig->controlEnabled[button-AE_BUTTON_BU] = !gConfig->controlEnabled[button-AE_BUTTON_BU];
			leds_set(button, gConfig->currPreset.controlEnabled[button-AE_BUTTON_BU]);
			ret = apply_control(gConfig->controlEnabled);
			if (ret != 0) {
				return -1;
			}
			break;
		default:
			break;
		}
	} else {
		switch (button) {
		case AE_BUTTON_B1:
		case AE_BUTTON_B2:
		case AE_BUTTON_B3:
		case AE_BUTTON_B4:
		case AE_BUTTON_B5:
		case AE_BUTTON_B6:
		case AE_BUTTON_B7:
		case AE_BUTTON_B8:
			// Load new preset
			leds_set_one_hot(button);
			ret = load_preset(gConfig->currBank, button);
			if (ret != 0) {
				return ret;
			}
			ret = apply_switch_configuration(gConfig->currPreset.pedalOrder, gConfig->currPreset.enabled, gPresenceDetect, gConfig->muteEnabled);
			if (ret != 0) {
				return -1;
			}
			break;
		case AE_BUTTON_BU:
			// Increment bank
			gConfig->currBank = (gConfig->currBank + 1) % AE_BANK_COUNT;
			break;
		case AE_BUTTON_BD:
			// Decrement bank
			gConfig->currBank = (gConfig->currBank + AE_BANK_COUNT - 1) % AE_BANK_COUNT;
			break;
		default:
			break;
		}
	}

	return 0;
}

static int handle_button_held_event(enum ae_button button) {

	int ret;

	switch (button) {
	case AE_BUTTON_B8:
		// Toggle mode
		gConfig->pedalMode = !gConfig->pedalMode;
		break;
	case AE_BUTTON_BU:
		// Toggle bypass
		gConfig->bypassEnabled = !gConfig->bypassEnabled;
		ret = apply_bypass(gConfig->bypassEnabled);
		if (ret != 0) {
			return ret;
		}
		break;
	case AE_BUTTON_BD:
		// Toggle mute
		gConfig->muteEnabled = !gConfig->muteEnabled;
		ret = apply_switch_configuration(gConfig->currPreset.pedalOrder, gConfig->currPreset.enabled, gPresenceDetect, gConfig->muteEnabled);
		if (ret != 0) {
			return ret;
		}
		break;
	default:
		break;
	}

	return 0;
}

static int presence_control_update(void *context, enum ae_presence jack, bool presence) {

	(void)context; // Unused

	int ret;

	ret = pthread_mutex_lock(&gConfigMutex);
	if (ret != 0) {
		return -1;
	}

	gPresenceDetect[jack/2][jack%2] = presence; // gPresenceDetect is a [X][2] array while presence is indexing [X*2]

	// Apply new switch parameters if needed
	ret = apply_switch_configuration(gConfig->currPreset.pedalOrder, gConfig->currPreset.enabled, gPresenceDetect, gConfig->muteEnabled);
	if (ret != 0) {
		return ret;
	}

	pthread_mutex_unlock(&gConfigMutex);

	return ret;
}

static int button_control_update(void *context, enum ae_button button, enum button_state state) {

	(void)context; // Unused

	static enum button_state lastState[AE_BUTTON_COUNT] = { 0 };
	int ret;

	ret = pthread_mutex_lock(&gConfigMutex);
	if (ret != 0) {
		return -1;
	}

	switch (state) {
	case BUTTON_RELEASED:
		if (lastState[button] == BUTTON_PRESSED) {
			// The button has been pressed
			ret = handle_button_pressed_event(button);
		}
		break;
	case BUTTON_PRESSED:
		break;
	case BUTTON_HELD:
		if (lastState[button] == BUTTON_PRESSED) {
			// The button has been held down
			ret = handle_button_held_event(button);
		}
		break;
	default:
		ret = -1;
	}

	// Update last button state
	lastState[button] = state;

	pthread_mutex_unlock(&gConfigMutex);

	return ret;
}

int control_init() {

	int ret;
	int fd;

	// Initialize gpio banks
	for (int i = 0; i < GPIO_COUNT; i++) {
		ret = gpio_init(&gGPIOBanks[i], i);
		if (ret != 0) {
			return ret;
		}
	}

	// Initialize gpio extender banks
	for (int i = 0; i < GPIO_EXT_COUNT; i++) {
		ret = i2c_init(&gI2C[i], I2C1, PCF8575_I2C_ADDR(i));
		if (ret != 0) {
			return ret;
		}
		ret = gpio_ext_init(&gGPIOExtBanks[i], PCF8575, &gI2C[i]);
		if (ret != 0) {
			return ret;
		}
	}

	// Initialize switch matrix
	ret = mt8809_init(&gSwitchMatrix);
	if (ret != 0) {
		return ret;
	}

	// Block entry points until we are fully initialized
	pthread_mutex_init(&gConfigMutex, NULL);
	ret = pthread_mutex_lock(&gConfigMutex);
	if (ret != 0) {
		return -1;
	}

	// Initialize button control
	ret = button_control_init(&gButtonControl, &button_control_update, NULL);
	if (ret != 0) {
		pthread_mutex_unlock(&gConfigMutex);
		return ret;
	}

	// Initialize presence control
	ret = presence_control_init(&gPresenceControl, &presence_control_update, NULL, &gPresenceDetect[0][0]);
	if (ret != 0) {
		pthread_mutex_unlock(&gConfigMutex);
		return ret;
	}

	// Open mem fd
	fd = open(CONFIG_FILE, O_RDWR | O_CREAT);
	if (fd == -1) {
		pthread_mutex_unlock(&gConfigMutex);
		return -1;
	}

	// Seek to max location
	ret = lseek(fd, sizeof(*gConfig), SEEK_SET);
	if (ret != 0) {
		pthread_mutex_unlock(&gConfigMutex);
		return -1;
	}

	// Write 0 to max location
	// Note: This ensures we have enough space to write the config
	ret = write(fd, "", 1);
	if (ret != 1) {
		pthread_mutex_unlock(&gConfigMutex);
		return -1;
	}

	// Do memory map
	gConfig = mmap(NULL, sizeof(*gConfig), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	// Don't need fd after mmap
	close(fd);

	// Check mmap result
	if (gConfig == MAP_FAILED) {
		pthread_mutex_unlock(&gConfigMutex);
		return -1;
	}

	// Set bypass
	ret = apply_bypass(gConfig->bypassEnabled);
	if (ret != 0) {
		pthread_mutex_unlock(&gConfigMutex);
		return -1;
	}

	// Set control
	ret = apply_control(gConfig->controlEnabled);

	// Apply switch configuration
	ret = apply_switch_configuration(gConfig->currPreset.pedalOrder, gConfig->currPreset.enabled, gPresenceDetect, gConfig->muteEnabled);
	if (ret != 0) {
		pthread_mutex_unlock(&gConfigMutex);
		return -1;
	}

	bool ledOn[AE_LED_COUNT] = { 0 };

	// Get initial LED values
	if (gConfig->pedalMode) {
		ledOn[AE_LED_BU] = gConfig->controlEnabled[TIP];
		ledOn[AE_LED_BD] = gConfig->controlEnabled[RING];
		for (int i = 0; i < AE_MAX_EFFECTS; i++) {
			ledOn[i] = gConfig->currPreset.enabled[i];
		}
	} else {
		ledOn[gConfig->currPreset.preset] = TRUE;
	}

	// Initialize led control
	ret = led_control_init(&gLedControl, ledOn);
	if (ret != 0) {
		pthread_mutex_unlock(&gConfigMutex);
		return ret;
	}

	// Open entry points
	pthread_mutex_unlock(&gConfigMutex);

	return 0;
}

int control_uninit() {

	int ret;

	// Block entry points
	ret = pthread_mutex_lock(&gConfigMutex);
	if (ret != 0) {
		return -1;
	}

	munmap(gConfig, sizeof(*gConfig));

	// Uninitialize control interfaces
	led_control_uninit(&gLedControl);
	button_control_uninit(&gButtonControl);
	presence_control_uninit(&gPresenceControl);

	// Uninitialize switch matrix
	mt8809_uninit(&gSwitchMatrix);

	// Uninitialize gpio extender banks
	for (int i = 0; i < GPIO_EXT_COUNT; i++) {
		gpio_ext_uninit(&gGPIOExtBanks[i]);
		i2c_uninit(&gI2C[i]);
	}

	// Uninitialize gpio banks
	for (int i = 0; i < GPIO_COUNT; i++) {
		gpio_uninit(&gGPIOBanks[i]);
	}

	// Destroy entry mutex
	pthread_mutex_unlock(&gConfigMutex);
	pthread_mutex_destroy(&gConfigMutex);

	return 0;
}
