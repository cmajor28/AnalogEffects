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

#define GPIO0_START_ADDR	0x44E07000
#define GPIO0_END_ADDR		0x44E09000
#define GPIO0_SIZE			(GPIO0_END_ADDR - GPIO0_START_ADDR)

#define GPIO1_START_ADDR	0x4804C000
#define GPIO1_END_ADDR		0x4804E000
#define GPIO1_SIZE			(GPIO1_END_ADDR - GPIO1_START_ADDR)

#define GPIO2_START_ADDR	0x481AC000
#define GPIO2_END_ADDR		0x481AE000
#define GPIO2_SIZE			(GPIO2_END_ADDR - GPIO2_START_ADDR)

#define GPIO3_START_ADDR	0x481AE000
#define GPIO3_END_ADDR		0x481B0000
#define GPIO3_SIZE			(GPIO3_END_ADDR - GPIO3_START_ADDR)

#define GPIO_SETDATAOUT 0x194
#define GPIO_CLEARDATAOUT 0x190

#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void test_i2c() {

	int file;
	char *filename = "/dev/i2c-1";
	if ((file = open(filename, O_RDWR)) < 0) {
	    /* ERROR HANDLING: you can check errno to see what went wrong */
	    perror("Failed to open the i2c bus");
	    exit(1);
	}

	int addr = 0x21;          // The I2C address of the ADC
	if (ioctl(file, I2C_SLAVE, addr) < 0) {
	    printf("Failed to acquire bus access and/or talk to slave.\n");
	    /* ERROR HANDLING; you can check errno to see what went wrong */
	    exit(1);
	}

	char buf[10];
	int result;
	result = read(file, buf, 2);

	close(file);
}

int main(int argc, char *args[]) {

	static struct ae_preset presets[AE_BANK_COUNT][AE_PRESET_COUNT];
	int ret;

	ret = aeffects_init(presets);
	assert(ret == 0);

	struct ae_preset newPreset = {
			.controlEnabled = { TRUE, FALSE },
			.pedalOrder = { 1, 0, 0, 0, 0, 0, 0 },
			.enabled = { TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE },
			.bank = 1,
			.preset = 1
	};

	ret = aeffects_update(&newPreset);
	assert(ret == 0);

	memset(gPresenceDetect, 1, sizeof(bool) * AE_MAX_EFFECTS * 2);

	set_preset(1);
	//set_bypass(1);



	//ret = aeffects_uninit();
	//assert(ret == 0);


	while (1) {
		sleep(1);
	}

	return EXIT_SUCCESS;
}
