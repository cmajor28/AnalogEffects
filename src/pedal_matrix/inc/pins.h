#ifndef PINS_H_
#define PINS_H_

#include "gpio.h"
#include "pcf857x.h"
#include "mt8809.h"
#include "led_control.h"
#include "presence_control.h"
#include "button_control.h"

enum {
	GPIO_EXT_PRESENCE,
	GPIO_EXT_BUTTONS,
	GPIO_EXT_LEDS,
	GPIO_EXT_COUNT
};

#define ARRAY_INITIALIZER(...) __VA_ARGS__

#define PCF8575_I2C_ADDR(gpioExt) (0x2 | (1 << (gpioExt))) // 0b0100<A2><A1><A0> with one hot encoding

extern struct gpio gGPIOBanks[GPIO_COUNT];
extern struct gpio_ext gGPIOExtBanks[GPIO_EXT_COUNT];

#define GPIO_EXT_LEDS_INIT()																					\
		PCF8575_PIN_INITIALIZER(GPIO_PIN_INITIALIZER(NULL, -1))					/* int (no pin connected) */

#define GPIO_EXT_PRESENCE_INIT()															\
		PCF8575_PIN_INITIALIZER(GPIO_PIN_INITIALIZER(&gGPIOBanks[GPIO3], 21))	/* int */

#define GPIO_EXT_BUTTONS_INIT()																\
		PCF8575_PIN_INITIALIZER(GPIO_PIN_INITIALIZER(&gGPIOBanks[GPIO3], 24))	/* int */

#define LED_PINS_INIT()																		\
		LED_CONTROL_INITIALIZER(															\
			ARRAY_INITIALIZER({																\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_LEDS], 0),	/* led_b1 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_LEDS], 1),	/* led_b2 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_LEDS], 2),	/* led_b3 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_LEDS], 3),	/* led_b4 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_LEDS], 4),	/* led_b5 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_LEDS], 5),	/* led_b6 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_LEDS], 6),	/* led_b7 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_LEDS], 7),	/* led_b8 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_LEDS], 8),	/* led_bu */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_LEDS], 9)	/* led_bd */	\
			})																				\
		)



#define PRESENCE_PINS_INIT()																		\
		PRESENCE_CONTROL_INITIALIZER(																\
			ARRAY_INITIALIZER({																		\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_PRESENCE], 0),		/* jack_i0 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_PRESENCE], 1),		/* jack_i1 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_PRESENCE], 2),		/* jack_i2 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_PRESENCE], 3),		/* jack_i3 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_PRESENCE], 4),		/* jack_i4 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_PRESENCE], 5),		/* jack_i5 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_PRESENCE], 6),		/* jack_i6 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_PRESENCE], 7),		/* jack_i7 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_PRESENCE], 8),		/* jack_o0 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_PRESENCE], 9),		/* jack_o1 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_PRESENCE], 10),	/* jack_o2 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_PRESENCE], 11),	/* jack_o3 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_PRESENCE], 12),	/* jack_o4 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_PRESENCE], 13),	/* jack_o5 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_PRESENCE], 14),	/* jack_o6 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_PRESENCE], 15)		/* jack_o7 */	\
			})																						\
		)

#define BUTTON_PINS_INIT()																		\
		BUTTON_CONTROL_INITIALIZER(																\
			ARRAY_INITIALIZER({																	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_BUTTONS], 0),	/* button_b1 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_BUTTONS], 1),	/* button_b2 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_BUTTONS], 2),	/* button_b3 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_BUTTONS], 3),	/* button_b4 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_BUTTONS], 4),	/* button_b5 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_BUTTONS], 5),	/* button_b6 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_BUTTONS], 6),	/* button_b7 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_BUTTONS], 7),	/* button_b8 */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_BUTTONS], 8),	/* button_bu */	\
				GPIO_EXT_PIN_INITIALIZER(&gGPIOExtBanks[GPIO_EXT_BUTTONS], 9)	/* button_bd */	\
			})																					\
		)

#define MT8809_PINS_INIT()												\
		MT8809_PIN_INITIALIZER(	&gGPIOBanks[GPIO1], 4,	/* reset */		\
								&gGPIOBanks[GPIO2], 1,	/* cs */		\
								&gGPIOBanks[GPIO0], 26,	/* strobe */	\
								&gGPIOBanks[GPIO1], 14,	/* data */		\
								&gGPIOBanks[GPIO0], 22,	/* ax0 */		\
								&gGPIOBanks[GPIO0], 27,	/* ax1 */		\
								&gGPIOBanks[GPIO1], 15,	/* ax2 */		\
								&gGPIOBanks[GPIO1], 13,	/* ay0 */		\
								&gGPIOBanks[GPIO1], 23,	/* ay1 */		\
								&gGPIOBanks[GPIO1], 12)	/* ay2 */		\

#define BYPASS_PINS_INIT()											\
		GPIO_PIN_INITIALIZER(&gGPIOBanks[GPIO1], 17)	/* en */

#define CONTROL_PINS_INIT()											\
{																	\
		GPIO_PIN_INITIALIZER(&gGPIOBanks[GPIO3], 14),	/* en_a */	\
		GPIO_PIN_INITIALIZER(&gGPIOBanks[GPIO3], 16)	/* en_a */	\
}

#endif // PINS_H_
