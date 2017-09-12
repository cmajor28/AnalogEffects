#include "led_control.h"

int led_control_init(struct led_control *leds, bool ledOn[AE_LED_COUNT]) {

	for (int i = 0; i < AE_LED_COUNT; i++) {
		gpio_ext_pin_set_value(&leds->pins[i], ledOn[i]);
	}

	return 0;
}

int led_control_update(struct led_control *leds, enum ae_led led, bool ledOn) {

	gpio_ext_pin_set_value(&leds->pins[led], ledOn);

	return 0;
}

int led_control_uninit(struct led_control *leds) {

	for (int i = 0; i < AE_LED_COUNT; i++) {
		gpio_ext_pin_set_value(&leds->pins[i], 0);
	}

	return 0;
}
