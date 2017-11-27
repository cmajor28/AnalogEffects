#include "control.h"
#include "control_interface.h"
#include "utils.h"
#include "pins.h"

// Global configuration variables
bool gPresenceDetect[AE_MAX_EFFECTS+1][2];
pthread_mutex_t gConfigMutex;
struct ae_config *gConfig; // mmap() into non-volatile memory
struct control_callbacks gControlCallbacks;

// Global preset list
extern pthread_mutex_t gPresetsMutex;
extern struct ae_preset gPresets[AE_BANK_COUNT][AE_PRESET_COUNT];

// Global GPIO banks
struct gpio gGPIOBanks[GPIO_COUNT] = { { 0 } };
struct gpio_ext gGPIOExtBanks[GPIO_EXT_COUNT] = { GPIO_EXT_PRESENCE_INIT(), GPIO_EXT_BUTTONS_INIT(), GPIO_EXT_LEDS_INIT() };

// Global device interfaces
struct mt8809 gSwitchMatrix = MT8809_PINS_INIT();
struct led_control gLedControl = LED_PINS_INIT();
struct presence_control gPresenceControl = PRESENCE_PINS_INIT();
struct button_control gButtonControl = BUTTON_PINS_INIT();
struct gpio_pin gControlSwitch[2] = CONTROL_PINS_INIT();

// I2C interfaces
struct i2c gI2C[GPIO_EXT_COUNT];

static int leds_blink_one(enum ae_led led) {

	int ret;
	ret = led_control_blink(&gLedControl, led, TRUE, LED_BLINK_PERIOD_MILLISECONDS);
	return ret;
}

static int leds_set_one_hot(enum ae_led led) {

	int ret;
	bool ledOneHot[AE_LED_COUNT] = { 0 };
	ledOneHot[led] = TRUE;
	ret = led_control_set_all(&gLedControl, ledOneHot);
	return ret;
}

static int leds_set(enum ae_led led, bool value) {

	int ret;
	ret = led_control_set(&gLedControl, led, value); // Set single led value
	return ret;
}

static int leds_set_all(bool controlEnabled[2], bool enabled[AE_MAX_EFFECTS]) {

	int ret;
	bool ledOn[AE_LED_COUNT] = { 0 };

	// Get all LED values
	ledOn[AE_LED_BU] = gConfig->currPreset.controlEnabled[TIP];
	ledOn[AE_LED_BD] = gConfig->currPreset.controlEnabled[RING];
	for (int i = 0; i < AE_MAX_EFFECTS; i++) {
		ledOn[i] = gConfig->currPreset.enabled[i];
	}
	ret = led_control_set_all(&gLedControl, ledOn);
	return ret;
}

static int apply_control(bool control[2]) {

	int ret = 0;
	PRINT("control: Setting control signals to { %d, %d }.\n", control[TIP], control[RING]);
	ret |= gpio_pin_set_value(&gControlSwitch[TIP], control[TIP]);
	ret |= gpio_pin_set_value(&gControlSwitch[RING], control[RING]);
	return ret;
}

static int apply_switch_configuration(int pedalOrder[AE_MAX_EFFECTS], bool enabled[AE_MAX_EFFECTS], bool presenceDetect[AE_MAX_EFFECTS][2], bool bypass, bool mute) {

	static uint64_t lastSwitchSet = 0;
	static bool lastMute = FALSE;
	static bool lastBypass = FALSE;

	int ret;
	uint64_t switchSet = 0;
	int lastInput = 0;
	int pedal;
	int address;

	PRINT("control: Setting switch array.\n");

	// Route each input to an output
	for (int i = 0; i < AE_MAX_EFFECTS; i++) {
		pedal = pedalOrder[i];
		if (pedal > 0) { // If pedal exists
			if (enabled[pedal-1]) { // If enabled
				if (presenceDetect[pedal][OUT] && presenceDetect[pedal][IN]) { // If presence detected (presence is active low)
					address = lastInput + (pedal << 3); // Get crosspoint address for last pedal input and new pedal output
					switchSet |= ((uint64_t)1 << address);
					lastInput = pedal;
				}
			}
		}
	}

	// Route last input to output
	if (bypass) {
		switchSet |= ((uint64_t)1 << 0);
	} else {
		switchSet |= ((uint64_t)1 << lastInput);
	}

	// Disable input  and output if muting
	if (mute) {
		switchSet &= (~0x01010101010101FF);
	}

	// Calculate switch sets with input disconnected
	uint64_t switchSetNoInput = switchSet & (~0x0101010101010101);
	uint64_t lastSwitchSetNoInput = lastSwitchSet & (~0x0101010101010101);

	// Calculate switch sets with output disconnected
	uint64_t switchSetNoOutput = switchSet & (~0xFF);
	uint64_t lastSwitchSetNoOutput = lastSwitchSet & (~0xFF);

	// Calculate switch sets with output disconnected
	uint64_t switchSetNoInputOutput = switchSetNoInput & switchSetNoOutput;
	uint64_t lastSwitchSetNoInputOutput = lastSwitchSetNoInput & lastSwitchSetNoOutput;

	if (lastMute && !mute && (switchSetNoInputOutput == lastSwitchSetNoInputOutput)) { // We are unmuting only
		// Route input on mt8809
		PRINT("control: Disabling mute configuration.\n");
		uint8_t address1 = __builtin_ctzll(switchSet ^ switchSetNoInput); // __builtin_ctzll() counts the number of trailing zeros
		uint8_t address2 = __builtin_ctzll(switchSet ^ switchSetNoOutput); // __builtin_ctzll() counts the number of trailing zeros
		ret = mt8809_set_switch(&gSwitchMatrix, address1, 0x1);
		if (ret != 0) {
			return ret;
		}
		ret = mt8809_set_switch(&gSwitchMatrix, address2, 0x1);
		if (ret != 0) {
			return ret;
		}
	} else if (!lastMute && mute && (switchSetNoInputOutput == lastSwitchSetNoInputOutput)) { // We are muting only
		// Unroute input on mt8809
		PRINT("control: Enabling mute configuration.\n");
		uint8_t address1 = __builtin_ctzll(lastSwitchSet ^ lastSwitchSetNoInput); // __builtin_ctzll() counts the number of trailing zeros
		uint8_t address2 = __builtin_ctzll(lastSwitchSet ^ lastSwitchSetNoOutput); // __builtin_ctzll() counts the number of trailing zeros
		ret = mt8809_set_switch(&gSwitchMatrix, address2, 0x0);
		if (ret != 0) {
			return ret;
		}
		ret = mt8809_set_switch(&gSwitchMatrix, address1, 0x0);
		if (ret != 0) {
			return ret;
		}
	} else if (lastBypass && !bypass && (switchSetNoOutput == lastSwitchSetNoOutput)) { // We are unbypassing only
		// Route signal on mt8809
		PRINT("control: Disabling bypass configuration.\n");
		if (!mute) { // Mute has precedence
			uint8_t address1 = __builtin_ctzll(lastSwitchSet ^ lastSwitchSetNoOutput); // __builtin_ctzll() counts the number of trailing zeros
			uint8_t address2 = __builtin_ctzll(switchSet ^ switchSetNoOutput); // __builtin_ctzll() counts the number of trailing zeros
			ret = mt8809_set_switch(&gSwitchMatrix, address1, 0x0);
			if (ret != 0) {
				return ret;
			}
			ret = mt8809_set_switch(&gSwitchMatrix, address2, 0x1);
			if (ret != 0) {
				return ret;
			}
		}
	} else if (!lastBypass && bypass && (switchSetNoOutput == lastSwitchSetNoOutput)) { // We are bypassing only
		// Unroute signal on mt8809
		PRINT("control: Enabling bypass configuration\n");
		if (!mute) { // Mute has precedence
			uint8_t address1 = __builtin_ctzll(lastSwitchSet ^ lastSwitchSetNoOutput); // __builtin_ctzll() counts the number of trailing zeros
			uint8_t address2 = __builtin_ctzll(switchSet ^ switchSetNoOutput); // __builtin_ctzll() counts the number of trailing zeros
			ret = mt8809_set_switch(&gSwitchMatrix, address1, 0x0);
			if (ret != 0) {
				return ret;
			}
			ret = mt8809_set_switch(&gSwitchMatrix, address2, 0x1);
			if (ret != 0) {
				return ret;
			}
		}
	} else if (switchSet != lastSwitchSet) { // Only program the switch if a change has been made
		PRINT("control: Reprogramming switch array.\n");

		// Reset switch (break-before make)
		mt8809_reset(&gSwitchMatrix);

		// Set switches on mt8809
		ret = mt8809_set_switches(&gSwitchMatrix, switchSet);
		if (ret != 0) {
			return ret;
		}
	}

	lastMute = mute;
	lastBypass = bypass;
	lastSwitchSet = switchSet;

	return 0;
}

static int load_preset(int bank, int preset) {

	int ret;

	PRINT("control: Loading bank %d preset %d.\n", bank+1, preset+1);

	// Lock preset mutex
	ret = pthread_mutex_lock(&gPresetsMutex);
	if (ret != 0) {
		PRINTE("pthread_mutex_lock() failed!");
		return -1;
	}

	// Load preset into config
	memcpy(&gConfig->currPreset, &gPresets[bank][preset], sizeof(gConfig->currPreset));

	// Validate preset
	gConfig->currPreset.bank = bank;
	gConfig->currPreset.preset = preset;

	pthread_mutex_unlock(&gPresetsMutex);

	// Apply the preset to hardware
	ret = apply_switch_configuration(gConfig->currPreset.pedalOrder, gConfig->currPreset.enabled, gPresenceDetect, gConfig->bypassEnabled, gConfig->muteEnabled);
	ret |= apply_control(gConfig->currPreset.controlEnabled);

	return ret;
}

static int set_new_preset(int preset) {

	int ret;

	PRINT("control: Setting preset to %d.\n", preset+1);

	// Load new preset into config
	ret = load_preset(gConfig->currBank, preset);
	if (ret != 0) {
		return ret;
	}

	if (gConfig->pedalMode) {
		leds_set_all(gConfig->currPreset.controlEnabled, gConfig->currPreset.enabled);
	} else {
		leds_set_one_hot(preset);
	}

	gConfig->changeMade = FALSE;
	return 0;
}

static int set_new_bank(int bank) {

	PRINT("control: Setting bank to %d.\n", bank+1);
	gConfig->currBank = bank;
	gConfig->changeMade = TRUE;
	if (!gConfig->pedalMode) {
		leds_blink_one(gConfig->currPreset.preset);
	}
	return 0;
}

static int set_pedal_mode_enabled(bool enable) {

	gConfig->pedalMode = enable;

	PRINT("control: %s pedal mode.\n", enable ? "Enabling" : "Disabling");

	if (gConfig->pedalMode) {
		leds_set_all(gConfig->currPreset.controlEnabled, gConfig->currPreset.enabled);
	} else {
		leds_set_one_hot(gConfig->currPreset.preset);
		if (gConfig->changeMade) {
			leds_blink_one(gConfig->currPreset.preset);
		}
	}
	return 0;
}

static int set_pedal_enabled(int pedal, bool enable) {

	int ret;

	gConfig->currPreset.enabled[pedal] = enable;

	PRINT("control: %s pedal %d.\n", enable ? "Enabling" : "Disabling", pedal+1);

	// Apply new switch configuration
	ret = apply_switch_configuration(gConfig->currPreset.pedalOrder, gConfig->currPreset.enabled, gPresenceDetect, gConfig->bypassEnabled, gConfig->muteEnabled);
	if (ret != 0) {
		return ret;
	}

	gConfig->changeMade = TRUE;
	leds_set(pedal, gConfig->currPreset.enabled[pedal]);
	return 0;
}

static int set_control_enabled(int control, bool enable) {

	int ret;

	gConfig->currPreset.controlEnabled[control] = enable;

	PRINT("control: %s control %d.\n", enable ? "Enabling" : "Disabling", control);

	// Set new control signals
	ret = apply_control(gConfig->currPreset.controlEnabled);
	if (ret != 0) {
		return ret;
	}

	gConfig->changeMade = TRUE;
	leds_set(control + AE_BUTTON_BU, gConfig->currPreset.controlEnabled[control]);
	return 0;
}

static int set_bypass_enabled(bool enable) {

	int ret;

	gConfig->bypassEnabled = enable;

	PRINT("control: %s bypass mode.\n", enable ? "Enabling" : "Disabling");

	// Apply new switch configuration
	ret = apply_switch_configuration(gConfig->currPreset.pedalOrder, gConfig->currPreset.enabled, gPresenceDetect, gConfig->bypassEnabled, gConfig->muteEnabled);
	if (ret != 0) {
		return ret;
	}

	return 0;
}

static int set_mute_enabled(bool enable) {

	int ret;

	gConfig->muteEnabled = enable;

	PRINT("control: %s mute mode.\n", enable ? "Enabling" : "Disabling");

	// Apply new switch configuration
	ret = apply_switch_configuration(gConfig->currPreset.pedalOrder, gConfig->currPreset.enabled, gPresenceDetect, gConfig->bypassEnabled, gConfig->muteEnabled);
	if (ret != 0) {
		return ret;
	}

	return 0;
}

static int handle_button_pressed_event(enum ae_button button) {

	int ret;

	PRINT("control: Button %d pressed.\n", button);

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
			ret = set_pedal_enabled(button, !gConfig->currPreset.enabled[button]);
			break;
		case AE_BUTTON_BU:
		case AE_BUTTON_BD:
			// Toggle control enabled
			ret = set_control_enabled(button-AE_BUTTON_BU, !gConfig->currPreset.controlEnabled[button-AE_BUTTON_BU]);
			break;
		default:
			ret = -1;
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
			ret = set_new_preset(button);
			if (gControlCallbacks.presetChanged) {
				gControlCallbacks.presetChanged(gConfig->currPreset.preset+1);
			}
			break;
		case AE_BUTTON_BU:
			// Increment bank
			ret = set_new_bank((gConfig->currBank + 1) % AE_BANK_COUNT);
			if (gControlCallbacks.bankChanged) {
				gControlCallbacks.bankChanged(gConfig->currBank+1);
			}
			break;
		case AE_BUTTON_BD:
			// Decrement bank
			ret = set_new_bank((gConfig->currBank + AE_BANK_COUNT - 1) % AE_BANK_COUNT);
			if (gControlCallbacks.bankChanged) {
				gControlCallbacks.bankChanged(gConfig->currBank+1);
			}
			break;
		default:
			ret = -1;
		}
	}

	return ret;
}

static int handle_button_held_event(enum ae_button button) {

	int ret;

	PRINT("control: Button %d held.\n", button);

	switch (button) {
	case AE_BUTTON_B1:
		// Toggle mode
		ret = set_pedal_mode_enabled(!gConfig->pedalMode);
		if (gControlCallbacks.modeChanged) {
			gControlCallbacks.modeChanged(gConfig->pedalMode);
		}
		break;
	case AE_BUTTON_B2:
	case AE_BUTTON_B3:
	case AE_BUTTON_B4:
	case AE_BUTTON_B5:
	case AE_BUTTON_B6:
	case AE_BUTTON_B7:
	case AE_BUTTON_B8:
		ret = 0;
		break;
	case AE_BUTTON_BU:
		// Toggle bypass
		ret = set_bypass_enabled(!gConfig->bypassEnabled);
		if (gControlCallbacks.bypassEnabled) {
			gControlCallbacks.bypassEnabled(gConfig->bypassEnabled);
		}
		break;
	case AE_BUTTON_BD:
		// Toggle mute
		ret = set_mute_enabled(!gConfig->muteEnabled);
		if (gControlCallbacks.muteEnabled) {
			gControlCallbacks.muteEnabled(gConfig->muteEnabled);
		}
		break;
	default:
		ret = -1;
		break;
	}

	return ret;
}

static int presence_control_update(void *context, enum ae_presence jack, bool presence) {

	(void)context; // Unused

	int ret;

	PRINT("control: Presence detect jack %d is %d.\n", jack, presence);

	ret = pthread_mutex_lock(&gConfigMutex);
	if (ret != 0) {
		PRINTE("pthread_mutex_lock() failed!");
		return -1;
	}

	gPresenceDetect[jack/2][jack%2] = presence; // gPresenceDetect is a [X][2] array while presence is indexing [X*2]

	// Apply new switch parameters if needed
	ret = apply_switch_configuration(gConfig->currPreset.pedalOrder, gConfig->currPreset.enabled, gPresenceDetect, gConfig->bypassEnabled, gConfig->muteEnabled);
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
		PRINTE("pthread_mutex_lock() failed!");
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

	PRINT("control: Initializing GPIO banks.\n");

	// We need to wake up gpio banks first
	ret = gpio_wakeup(TRUE);
	if (ret != 0) {
		return ret;
	}

	// Initialize gpio banks
	for (int i = 0; i < GPIO_COUNT; i++) {
		ret = gpio_init(&gGPIOBanks[i], i);
		if (ret != 0) {
			return ret;
		}
	}

	PRINT("control: Initializing GPIO extender banks.\n");

	// Initialize gpio extender banks
	for (int i = 0; i < GPIO_EXT_COUNT; i++) {
		ret = i2c_init(&gI2C[i], I2C1, PCF8575_I2C_ADDR(i));
		if (ret != 0) {
			return ret;
		}
		ret = gpio_ext_init(&gGPIOExtBanks[i], PCF8575, &gI2C[i], PCF8575_INT_DEBOUNCE_TIME(i));
		if (ret != 0) {
			return ret;
		}
	}

	PRINT("control: Initializing switch matrix.\n");

	// Initialize switch matrix
	ret = mt8809_init(&gSwitchMatrix);
	if (ret != 0) {
		return ret;
	}

	PRINT("control: Initializing control mutex.\n");

	// Block entry points until we are fully initialized
	pthread_mutexattr_t recursiveMutexAttr;
	pthread_mutexattr_init(&recursiveMutexAttr);
    pthread_mutexattr_settype(&recursiveMutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&gConfigMutex, &recursiveMutexAttr);
	ret = pthread_mutex_lock(&gConfigMutex);
	if (ret != 0) {
		PRINTE("pthread_mutex_lock() failed!");
		return -1;
	}

	PRINT("control: Initializing button control.\n");

	// Initialize button control
	ret = button_control_init(&gButtonControl, &button_control_update, NULL);
	if (ret != 0) {
		pthread_mutex_unlock(&gConfigMutex);
		return ret;
	}

	PRINT("control: Initializing presence control.\n");

	// Initialize presence control
	ret = presence_control_init(&gPresenceControl, &presence_control_update, NULL, &gPresenceDetect[0][0]);
	if (ret != 0) {
		pthread_mutex_unlock(&gConfigMutex);
		return ret;
	}

	PRINT("control: Mapping config into non-volatile memory.\n");

	// Open mem fd
	fd = open(CONFIG_FILE, O_RDWR | O_CREAT);
	if (fd == -1) {
		PRINTE("open() failed!");
		pthread_mutex_unlock(&gConfigMutex);
		return -1;
	}

	// Seek to max location
	ret = lseek(fd, sizeof(*gConfig), SEEK_SET);
	if (ret != sizeof(*gConfig)) {
		PRINTE("lseek() failed!");
		pthread_mutex_unlock(&gConfigMutex);
		return -1;
	}

	// Write 0 to max location
	// Note: This ensures we have enough space to write the config
	ret = write(fd, "", 1);
	if (ret < 1) {
		PRINTE("write() failed!");
		pthread_mutex_unlock(&gConfigMutex);
		return -1;
	}

	// Do memory map
	gConfig = mmap(NULL, sizeof(*gConfig), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	// Don't need fd after mmap
	close(fd);

	// Check mmap result
	if (gConfig == MAP_FAILED) {
		PRINTE("mmap() failed!");
		pthread_mutex_unlock(&gConfigMutex);
		return -1;
	}

	PRINT("control: Setting initial device configuration.\n");

	// Set control
	ret = apply_control(gConfig->currPreset.controlEnabled);

	// Apply switch configuration
	ret = apply_switch_configuration(gConfig->currPreset.pedalOrder, gConfig->currPreset.enabled, gPresenceDetect, gConfig->bypassEnabled, gConfig->muteEnabled);
	if (ret != 0) {
		pthread_mutex_unlock(&gConfigMutex);
		return -1;
	}

	bool ledOn[AE_LED_COUNT] = { 0 };

	// Get initial LED values
	if (gConfig->pedalMode) {
		ledOn[AE_LED_BU] = gConfig->currPreset.controlEnabled[TIP];
		ledOn[AE_LED_BD] = gConfig->currPreset.controlEnabled[RING];
		for (int i = 0; i < AE_MAX_EFFECTS; i++) {
			ledOn[i] = gConfig->currPreset.enabled[i];
		}
	} else {
		ledOn[gConfig->currPreset.preset] = TRUE;
	}

	PRINT("control: Initializing led control.\n");

	// Initialize led control
	ret = led_control_init(&gLedControl, ledOn);
	if (ret != 0) {
		pthread_mutex_unlock(&gConfigMutex);
		return ret;
	}

	// Blink LED if change is made
	if (gConfig->changeMade && !gConfig->pedalMode) {
		leds_blink_one(gConfig->currPreset.preset);
	}

	// Open entry points
	pthread_mutex_unlock(&gConfigMutex);

	PRINT("control: Initialization done.\n");

	return 0;
}

int control_notify_update(int bank, int preset) {

	int ret;

	ret = pthread_mutex_lock(&gConfigMutex);
	if (ret != 0) {
		PRINTE("pthread_mutex_lock() failed!");
		return -1;
	}

	// Blink LED if change is made to current preset
	if (gConfig->currPreset.bank == bank && gConfig->currPreset.preset == preset && !gConfig->pedalMode) {
		leds_blink_one(gConfig->currPreset.preset);
	}
	pthread_mutex_unlock(&gConfigMutex);
	return ret;
}

int control_uninit() {

	int ret;

	// Block entry points
	ret = pthread_mutex_lock(&gConfigMutex);
	if (ret != 0) {
		PRINTE("pthread_mutex_lock() failed!");
		return -1;
	}

	munmap(gConfig, sizeof(*gConfig));

	// Uninitialize control interfaces
	PRINT("control: Uninitializing led control.\n");
	led_control_uninit(&gLedControl);
	PRINT("control: Uninitializing button control.\n");
	button_control_uninit(&gButtonControl);
	PRINT("control: Uninitializing presence control.\n");
	presence_control_uninit(&gPresenceControl);

	PRINT("control: Uninitializing switch matrix.\n");

	// Uninitialize switch matrix
	mt8809_uninit(&gSwitchMatrix);

	PRINT("control: Uninitializing GPIO extender banks.\n");

	// Uninitialize gpio extender banks
	for (int i = 0; i < GPIO_EXT_COUNT; i++) {
		gpio_ext_uninit(&gGPIOExtBanks[i]);
		i2c_uninit(&gI2C[i]);
	}

	PRINT("control: Uninitializing GPIO banks.\n");

	// Uninitialize gpio banks
	for (int i = 0; i < GPIO_COUNT; i++) {
		gpio_uninit(&gGPIOBanks[i]);
	}

	// Turn off gpio banks
	gpio_wakeup(FALSE);

	PRINT("control: Uninitializing control mutex.\n");

	// Destroy entry mutex
	pthread_mutex_unlock(&gConfigMutex);
	pthread_mutex_destroy(&gConfigMutex);

	PRINT("control: Uninitialization done.\n");

	return 0;
}

int register_callbacks(int (*presetChanged)(int), int (*bankChanged)(int), int (*modeChanged)(bool), int (*bypassEnabled)(bool), int (*muteEnabled)(bool)) {

	int ret;

	PRINT("control: Registering callbacks.\n");

	ret = pthread_mutex_lock(&gConfigMutex);
	if (ret != 0) {
		PRINTE("pthread_mutex_lock() failed!");
		return -1;
	}

	gControlCallbacks.presetChanged = presetChanged;
	gControlCallbacks.bankChanged = bankChanged;
	gControlCallbacks.modeChanged = modeChanged;
	gControlCallbacks.bypassEnabled = bypassEnabled;
	gControlCallbacks.muteEnabled = muteEnabled;

	pthread_mutex_unlock(&gConfigMutex);
	return ret;
}

int set_preset(int preset) {

	int ret;

	PRINT("control: set_preset() called.\n");

	ret = pthread_mutex_lock(&gConfigMutex);
	if (ret != 0) {
		PRINTE("pthread_mutex_lock() failed!");
		return -1;
	}

	ret = set_new_preset(preset - 1); // 0 indexed
	pthread_mutex_unlock(&gConfigMutex);
	return ret;
}

int set_bank(int bank) {

	int ret;

	PRINT("control: set_bank() called.\n");

	ret = pthread_mutex_lock(&gConfigMutex);
	if (ret != 0) {
		PRINTE("pthread_mutex_lock() failed!");
		return -1;
	}

	ret = set_new_bank(bank - 1); // 0 indexed
	pthread_mutex_unlock(&gConfigMutex);
	return ret;
}

int set_mode(bool pedalMode) {

	int ret;

	PRINT("control: set_mode() called.\n");

	ret = pthread_mutex_lock(&gConfigMutex);
	if (ret != 0) {
		PRINTE("pthread_mutex_lock() failed!");
		return -1;
	}

	ret = set_pedal_mode_enabled(pedalMode);
	pthread_mutex_unlock(&gConfigMutex);
	return ret;
}

int set_bypass(bool enabled) {

	int ret;

	PRINT("control: set_bypass() called.\n");

	ret = pthread_mutex_lock(&gConfigMutex);
	if (ret != 0) {
		PRINTE("pthread_mutex_lock() failed!");
		return -1;
	}

	ret = set_bypass_enabled(enabled);
	pthread_mutex_unlock(&gConfigMutex);
	return ret;
}

int set_mute(bool enabled) {

	int ret;

	PRINT("control: set_mute() called.\n");

	ret = pthread_mutex_lock(&gConfigMutex);
	if (ret != 0) {
		PRINTE("pthread_mutex_lock() failed!");
		return -1;
	}

	ret = set_mute_enabled(enabled);
	pthread_mutex_unlock(&gConfigMutex);
	return ret;
}

int get_preset(int *preset) {

	int ret;

	ret = pthread_mutex_lock(&gConfigMutex);
	if (ret != 0) {
		PRINTE("pthread_mutex_lock() failed!");
		return -1;
	}

    *preset = gConfig->currPreset.preset + 1;
	pthread_mutex_unlock(&gConfigMutex);
	return ret;
}

int get_bank(int *bank) {

	int ret;

	ret = pthread_mutex_lock(&gConfigMutex);
	if (ret != 0) {
		PRINTE("pthread_mutex_lock() failed!");
		return -1;
	}

    *bank = gConfig->currPreset.bank + 1;
	pthread_mutex_unlock(&gConfigMutex);
	return ret;
}

int get_mode(bool *pedalMode) {

	int ret;

	ret = pthread_mutex_lock(&gConfigMutex);
	if (ret != 0) {
		PRINTE("pthread_mutex_lock() failed!");
		return -1;
	}

	*pedalMode = gConfig->pedalMode;
	pthread_mutex_unlock(&gConfigMutex);
	return ret;
}

int get_bypass(bool *enabled) {

	int ret;

	ret = pthread_mutex_lock(&gConfigMutex);
	if (ret != 0) {
		PRINTE("pthread_mutex_lock() failed!");
		return -1;
	}

    *enabled = gConfig->bypassEnabled;
	pthread_mutex_unlock(&gConfigMutex);
	return ret;
}

int get_mute(bool *enabled) {

	int ret;

	ret = pthread_mutex_lock(&gConfigMutex);
	if (ret != 0) {
		PRINTE("pthread_mutex_lock() failed!");
		return -1;
	}

	*enabled = gConfig->muteEnabled;
	pthread_mutex_unlock(&gConfigMutex);
	return ret;
}
