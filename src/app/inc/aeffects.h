#ifndef AEFFECTS_H_
#define AEFFECTS_H_

#include <stdbool.h>

#define AE_MAX_EFFECTS 7
#define AE_PRESET_COUNT 8
#define AE_BANK_COUNT 16

struct ae_preset {
	int		bank;
	int		preset;
	int		pedalOrder[AE_MAX_EFFECTS];
	bool	enabled[AE_MAX_EFFECTS];
};

int aeffects_init(struct ae_preset presets[AE_BANK_COUNT][AE_PRESET_COUNT]);

int aeffects_update(struct ae_preset *preset);

int aeffects_uninit();

#endif // AEFFECTS_H_
