#include <stdlib.h>
#include <assert.h>
#include "aeffects.h"
#include "utils.h"

int main(int argc, char *args[]) {

	static struct ae_preset presets[AE_BANK_COUNT][AE_PRESET_COUNT];
	int ret;

	ret = aeffects_init(presets);
	assert(ret == 0);

	while (1) {
		sleep(1);
	}

	ret = aeffects_uninit();
	assert(ret == 0);


	return EXIT_SUCCESS;
}
