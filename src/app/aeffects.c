#include "aeffects.h"
#include "control.h"
#include "utils.h"

pthread_mutex_t gPresetsMutex;
struct ae_preset gPresets[AE_BANK_COUNT][AE_PRESET_COUNT];

int aeffects_init(struct ae_preset presets[AE_BANK_COUNT][AE_PRESET_COUNT]) {

	int ret;

	// Initialize mutex
	pthread_mutex_init(&gPresetsMutex, NULL);

	// Store list of presets
	memcpy(gPresets, presets, sizeof(presets[0][0]) * AE_BANK_COUNT * AE_PRESET_COUNT);

	// Initialize control
	ret = control_init();
	if (ret != 0) {
		return -1;
	}

	return 0;
}

int aeffects_update(struct ae_preset *preset) {

	int ret;

	ret = pthread_mutex_lock(&gPresetsMutex);
	if (ret != 0) {
		return -1;
	}

	// Store updated preset
	memcpy(&gPresets[preset->bank - 1][preset->preset - 1], preset, sizeof(*preset)); // Index starts at 1

	pthread_mutex_unlock(&gPresetsMutex);

	return 0;
}

int aeffects_uninit() {

	int ret;

	ret = pthread_mutex_lock(&gPresetsMutex);
	if (ret != 0) {
		return -1;
	}

	// Uninitialize control
	ret = control_uninit();
	if (ret != 0) {
		return -1;
	}

	// Reset presets
	memset(gPresets, 0, sizeof(gPresets));

	// Destroy mutex
	pthread_mutex_unlock(&gPresetsMutex);
	pthread_mutex_destroy(&gPresetsMutex);

	return 0;
}
