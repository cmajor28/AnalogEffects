#ifndef PRESENCE_CONTROL_H_
#define PRESENCE_CONTROL_H_

#include "pcf857x.h"
#include "utils.h"

enum ae_presence {
	AE_JACK_I0,
	AE_JACK_O0,
	AE_JACK_I1,
	AE_JACK_O1,
	AE_JACK_I2,
	AE_JACK_O2,
	AE_JACK_I3,
	AE_JACK_O3,
	AE_JACK_I4,
	AE_JACK_O4,
	AE_JACK_I5,
	AE_JACK_O5,
	AE_JACK_I6,
	AE_JACK_O6,
	AE_JACK_I7,
	AE_JACK_O7,
	AE_JACK_COUNT
};

struct presence_irq_context {
	enum ae_presence	jack;
	struct gpio_ext_irq	*irq;
	int					(*callback)(void *, enum ae_presence, bool);
	void				*context;
};

struct presence_control {
	struct gpio_ext_pin			pins[AE_JACK_COUNT];
	struct gpio_ext_irq			irqs[AE_JACK_COUNT];
	struct presence_irq_context	irqContextList[AE_JACK_COUNT];
	int							(*callback)(void *, enum ae_presence, bool);
	void						*context;
};

#define PRESENCE_CONTROL_INITIALIZER(pinList) { .pins = pinList }

int presence_control_init(struct presence_control *presence, int (*callback)(void *, enum ae_presence, bool), void *context, bool initialPresence[AE_JACK_COUNT]);

int presence_control_uninit(struct presence_control *presence);

#endif // PRESENCE_CONTROL_H_
