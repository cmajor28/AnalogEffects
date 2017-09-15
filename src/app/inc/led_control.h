#ifndef LED_CONTROL_H_
#define LED_CONTROL_H_

#include "pcf857x.h"
#include "utils.h"

enum ae_led {
	AE_LED_B1,
	AE_LED_B2,
	AE_LED_B3,
	AE_LED_B4,
	AE_LED_B5,
	AE_LED_B6,
	AE_LED_B7,
	AE_LED_B8,
	AE_LED_BU,
	AE_LED_BD,
	AE_LED_COUNT
};

struct led_control {
	struct gpio_ext_pin	pins[AE_LED_COUNT];
};

#define LED_CONTROL_INITIALIZER(pinList) { .pins = pinList }

int led_control_init(struct led_control *leds, bool ledOn[AE_LED_COUNT]);

int led_control_set(struct led_control *leds, enum ae_led ledPin, bool ledOn);

int led_control_set_all(struct led_control *leds, bool ledOn[AE_LED_COUNT]);

int led_control_get(struct led_control *leds, enum ae_led ledPin, bool *ledOn);

int led_control_get_all(struct led_control *leds, bool ledOn[AE_LED_COUNT]);

int led_control_uninit(struct led_control *leds);

#endif // LED_CONTROL_H_
