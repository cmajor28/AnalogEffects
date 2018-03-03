#include "led_control.h"

static int led_blink_func(struct led_control_blink_params *params) {

	int ret;
	bool value = TRUE;

	while (TRUE) {
		sleep2(MILLISECONDS, params->period / 2);
		ret = gpio_ext_pin_set_value(params->pin, value);
		if (ret != 0) {
			return ret;
		}
		value = !value;
	}

	return 0;
}

static int cleanup_blink_params(struct led_control *leds, enum ae_led led) {

	int ret;

	if (leds->blinkParams[led].blinkThread != 0) {
		// Cancel blink thread
		ret = pthread_cancel(leds->blinkParams[led].blinkThread);
		if (ret != 0) {
			PRINTE("pthread_cancel() failed!");
			return ret;
		}

		// Wait for thread to exit
		ret = pthread_join(leds->blinkParams[led].blinkThread, NULL);
		if (ret != 0) {
			PRINTE("pthread_join() failed!");
			return ret;
		}

		memset(&leds->blinkParams[led], 0, sizeof(leds->blinkParams[led]));
	}

	return 0;
}

int led_control_init(struct led_control *leds, bool ledOn[AE_LED_COUNT]) {

	int ret = 0;

	PRINT("led_control: Initializing interface.\n");

	memset(leds->blinkParams, 0, sizeof(leds->blinkParams));

	for (int i = 0; i < AE_LED_COUNT; i++) {
		ret |= gpio_ext_pin_set_value(&leds->pins[i], ledOn[i]);
	}

	return ret;
}

int led_control_blink(struct led_control *leds, enum ae_led led, bool enable, unsigned long period) {

	int ret;

	PRINT("led_control: %s LED %d blinking.\n", enable ? "Enabling" : "Disabling", led);

	// Clear old blink parameters
	ret = cleanup_blink_params(leds, led);
	if (ret != 0) {
		return ret;
	}

	// If blink enabled, set parameters and create blink thread
	if (enable) {
		leds->blinkParams[led].period = period;
		leds->blinkParams[led].pin = &leds->pins[led];

		// Create blink thread
		ret = pthread_create(&leds->blinkParams[led].blinkThread, NULL, (void * (*)(void *))&led_blink_func, &leds->blinkParams[led]);
		if (ret != 0) {
			PRINTE("pthread_create() failed!");
			return ret;
		}
	}

	return 0;
}

int led_control_set(struct led_control *leds, enum ae_led led, bool ledOn) {

	int ret;
	PRINT("led_control: Setting LED %d to %d.\n", led, ledOn);
	ret = cleanup_blink_params(leds, led); // Clear blink params
	ret |= gpio_ext_pin_set_value(&leds->pins[led], ledOn);
	return ret;
}

int led_control_set_all(struct led_control *leds, bool ledOn[AE_LED_COUNT]) {

	int ret = 0;

	for (int i = 0; i < AE_LED_COUNT; i++) {
		ret |= cleanup_blink_params(leds, i); // Clear blink parameters
		PRINT("led_control: Setting LED %d to %d.\n", i, ledOn[i]);
		ret |= gpio_ext_pin_set_value(&leds->pins[i], ledOn[i]);
	}

	return ret;
}

int led_control_get(struct led_control *leds, enum ae_led led, bool *ledOn) {

	int ret;
	ret = gpio_ext_pin_get_value(&leds->pins[led], ledOn);
	return ret;
}

int led_control_get_all(struct led_control *leds, bool ledOn[AE_LED_COUNT]) {

	int ret = 0;

	for (int i = 0; i < AE_LED_COUNT; i++) {
		ret |= gpio_ext_pin_get_value(&leds->pins[i], &ledOn[i]);
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
