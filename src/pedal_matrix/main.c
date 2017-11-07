#include <stdlib.h>
#include "aeffects.h"
#include "utils.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <poll.h>
#include <assert.h>

extern bool gPresenceDetect[AE_MAX_EFFECTS][2];

#include "i2c.h"
#include "control.h"
#include "control_interface.h"
#include "utils.h"
#include "pins.h"

extern struct i2c gI2C[GPIO_EXT_COUNT];

int test_i2c(int dev) {

	char buffer[2];
	i2c_read(gI2C + dev, buffer, sizeof(buffer));
	fprintf(stderr, "Read: 0x%04X\n", (unsigned)buffer[0] | ((unsigned)buffer[1] << 8));
	return 0;
}

int main(int argc, char *args[]) {

	static struct ae_preset presets[AE_BANK_COUNT][AE_PRESET_COUNT];
	int ret;

	ret = aeffects_init(presets);
	assert(ret == 0);

	struct ae_preset newPreset1 = {
			.controlEnabled = { TRUE, FALSE },
			.pedalOrder = { 7, 0, 0, 0, 0, 0, 0 },
			.enabled = { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE },
			.bank = 1,
			.preset = 1
	};

	struct ae_preset newPreset2 = {
			.controlEnabled = { TRUE, FALSE },
			.pedalOrder = { 7, 6, 0, 0, 0, 0, 0 },
			.enabled = { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE },
			.bank = 1,
			.preset = 2
	};

	ret = aeffects_update(&newPreset1);
	assert(ret == 0);

	ret = aeffects_update(&newPreset2);
	assert(ret == 0);

	memset(gPresenceDetect, 0, sizeof(bool) * (AE_MAX_EFFECTS+1) * 2);

	set_preset(1);
	set_preset(2);
	//set_bypass(1);



	//ret = aeffects_uninit();
	//assert(ret == 0);


	while (1) {
		sleep(1);
	}

	return EXIT_SUCCESS;
}
