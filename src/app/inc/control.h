#ifndef CONTROL_H_
#define CONTROL_H_

#include "aeffects.h"
#include <stdbool.h>

#define CONFIG_FILE "config.bin"

struct ae_config {
	bool				bypassEnabled;
	bool				pedalMode;
	struct ae_preset	currPreset;
	bool				enabled[AE_MAX_EFFECTS];
};

enum {
	IN,
	OUT
};

int control_init();

int control_uninit();

#endif // CONTROL_H_
