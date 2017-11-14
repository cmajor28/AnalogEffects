#include "led_control.h"

int led_control_init(struct led_control *leds, bool ledOn[AE_LED_COUNT]) {

	int ret = 0;

	PRINT("led_control: Initializing interface.\n");

	for (int i = 0; i < AE_LED_COUNT; i++) {
		ret |= gpio_ext_pin_set_value(&leds->pins[i], !ledOn[i]);
	}

	return ret;
}

int led_control_set(struct led_control *leds, enum ae_led led, bool ledOn) {

	int ret;
	PRINT("led_control: Setting LED %d to %d.\n", led, ledOn);
	ret = gpio_ext_pin_set_value(&leds->pins[led], !ledOn); // Active low
	return ret;
}

int led_control_set_all(struct led_control *leds, bool ledOn[AE_LED_COUNT]) {

	int ret = 0;

	for (int i = 0; i < AE_LED_COUNT; i++) {
		PRINT("led_control: Setting LED %d to %d.\n", i, ledOn[i]);
		ret |= gpio_ext_pin_set_value(&leds->pins[i], !ledOn[i]); // Active low
	}

	return ret;
}

int led_control_get(struct led_control *leds, enum ae_led led, bool *ledOn) {

	int ret;
	ret = gpio_ext_pin_get_value(&leds->pins[led], ledOn);
	*ledOn = !(*ledOn); // Active low
	return ret;
}

int led_control_get_all(struct led_control *leds, bool ledOn[AE_LED_COUNT]) {

	int ret = 0;

	for (int i = 0; i < AE_LED_COUNT; i++) {
		ret |= gpio_ext_pin_get_value(&leds->pins[i], &ledOn[i]);
		ledOn[i] = !ledOn[i]; // Active low
	}

	return ret;
}

int led_control_uninit(struct led_control *leds) {

	int ret = 0;

	PRINT("led_control: Initializing interface.\n");

	for (int i = 0; i < AE_LED_COUNT; i++) {
		ret |= gpio_ext_pin_set_value(&leds->pins[i], 0);
	}

	return ret;
}
