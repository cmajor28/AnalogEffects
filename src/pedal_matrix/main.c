#include <stdlib.h>
#include "aeffects.h"
#include "utils.h"
#include <assert.h>

extern bool gPresenceDetect[AE_MAX_EFFECTS][2];

int main(int argc, char *args[]) {

	static struct ae_preset presets[AE_BANK_COUNT][AE_PRESET_COUNT];
	int ret;

	ret = aeffects_init(presets);
	assert(ret == 0);

	struct ae_preset newPreset = {
			.controlEnabled = { TRUE, FALSE },
			.pedalOrder = { 2, 3, 1, 0, 0, 0, 0 },
			.enabled = { TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE },
			.bank = 1,
			.preset = 1
	};

	ret = aeffects_update(&newPreset);
	assert(ret == 0);

	memset(gPresenceDetect, 1, sizeof(bool) * AE_MAX_EFFECTS * 2);

	set_preset(1);

	//ret = aeffects_uninit();
	//assert(ret == 0);

	while (1) {
		sleep(1);
	}

	return EXIT_SUCCESS;
}
