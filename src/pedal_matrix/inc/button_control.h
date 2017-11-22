#ifndef BUTTON_CONTROL_H_
#define BUTTON_CONTROL_H_

#include "pcf857x.h"
#include "utils.h"

enum ae_button {
	AE_BUTTON_B1,
	AE_BUTTON_B2,
	AE_BUTTON_B3,
	AE_BUTTON_B4,
	AE_BUTTON_B5,
	AE_BUTTON_B6,
	AE_BUTTON_B7,
	AE_BUTTON_B8,
	AE_BUTTON_BU,
	AE_BUTTON_BD,
	AE_BUTTON_COUNT
};

#define BUTTON_HOLD_TIME_MILLISECONDS 1500

enum button_state {
	BUTTON_RELEASED,
	BUTTON_PRESSED,
	BUTTON_HELD
};

struct button_irq_context {
	enum ae_button		button;
	struct gpio_ext_irq	*irq;
	struct timer		holdTimer;
	enum button_state	state;
	int					(*callback)(void *, enum ae_button, enum button_state);
	void				*context;
};

struct button_control {
	struct gpio_ext_pin			pins[AE_BUTTON_COUNT];
	struct gpio_ext_irq			irqs[AE_BUTTON_COUNT];
	struct button_irq_context	irqContextList[AE_BUTTON_COUNT];
	int							(*callback)(void *, enum ae_button, enum button_state);
	void						*context;
};

#define BUTTON_CONTROL_INITIALIZER(pinList) { .pins = pinList }

int button_control_init(struct button_control *buttons, int (*callback)(void *, enum ae_button, enum button_state), void *context);

int button_control_uninit(struct button_control *buttons);

#endif // BUTTON_CONTROL_H_
