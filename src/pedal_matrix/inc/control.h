#ifndef CONTROL_H_
#define CONTROL_H_

#include "aeffects.h"
#include <stdbool.h>

#define CONFIG_FILE "config.bin"

struct ae_config {
	bool				controlEnabled[2];
	bool				bypassEnabled;
	bool				muteEnabled;
	bool				pedalMode;
	bool				changeMade;
	int					currBank;
	struct ae_preset	currPreset;
};

enum {
	IN,
	OUT
};

enum {
	TIP,
	RING
};

int control_init();

int control_notify_update(int bank, int preset);

int control_uninit();

#endif // CONTROL_H_
