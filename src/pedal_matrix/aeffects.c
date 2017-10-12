#include "aeffects.h"
#include "control.h"
#include "utils.h"

pthread_mutex_t gPresetsMutex;
struct ae_preset gPresets[AE_BANK_COUNT][AE_PRESET_COUNT];

int aeffects_init(struct ae_preset presets[AE_BANK_COUNT][AE_PRESET_COUNT]) {

	int ret;

	PRINT("aeffects: Initializing library\n");

	// Initialize mutex
	pthread_mutex_init(&gPresetsMutex, NULL);

	// Store list of presets
	memcpy(gPresets, presets, sizeof(presets[0][0]) * AE_BANK_COUNT * AE_PRESET_COUNT);

	// Initialize control
	ret = control_init();
	if (ret != 0) {
		PRINT("aeffects: Failed to initialize library.\n");
		return -1;
	}

	PRINT("aeffects: Finished initializing library.\n");

	return 0;
}

int aeffects_update(struct ae_preset *preset) {

	int ret;

	PRINT("aeffects: Updating preset %d.", preset->preset);

	ret = pthread_mutex_lock(&gPresetsMutex);
	if (ret != 0) {
		PRINT_LOG("pthread_mutex_lock() failed!");
		return -1;
	}

	// Store updated preset
	memcpy(&gPresets[preset->bank - 1][preset->preset - 1], preset, sizeof(*preset)); // Index starts at 1

	pthread_mutex_unlock(&gPresetsMutex);

	return 0;
}

int aeffects_uninit() {

	int ret;

	PRINT("aeffects: Uninitializing library.\n");

	ret = pthread_mutex_lock(&gPresetsMutex);
	if (ret != 0) {
		PRINT_LOG("pthread_mutex_lock() failed!");
		return -1;
	}

	// Uninitialize control
	ret = control_uninit();
	if (ret != 0) {
		PRINT("aeffects: Failed to uninitialize library.\n");
		return -1;
	}

	// Reset presets
	memset(gPresets, 0, sizeof(gPresets));

	// Destroy mutex
	pthread_mutex_unlock(&gPresetsMutex);
	pthread_mutex_destroy(&gPresetsMutex);

	PRINT("aeffects: Finished uninitializing library.");

	return 0;
}
