#include "presence_control.h"

static int presence_control_update(struct presence_irq_context *context) {

	bool value;

	gpio_ext_pin_get_value(&context->irq->gpioExtPin, &value);
	context->callback(context->context, context->jack, !value);

	return 0;
}

int presence_control_init(struct presence_control *presence, int (*callback)(void *, enum ae_presence, bool), void *context, bool initialPresence[AE_JACK_COUNT]) {

	bool value;

	PRINT("presence_control: Initializing control interface.\n");

	PRINT("presence_control: Creating presence detect IRQs.\n");

	// Initialize all irqs
	for (int i = 0; i < AE_JACK_COUNT; i++) {
		// Set presence detect update callback info
		presence->irqContextList[i].jack = i;
		presence->irqContextList[i].irq = &presence->irqs[i];
		presence->irqContextList[i].callback = callback;
		presence->irqContextList[i].context = context;

		// Initialize gpio ext irq
		gpio_ext_irq_init(	&presence->irqs[i],
							&presence->pins[i],
							(int (*)(void *))&presence_control_update,
							&presence->irqContextList[i],
							GPIO_DIR_IN,
							GPIO_SEN_BOTH);
	}

	presence->callback = callback;
	presence->context = context;

	PRINT("presence_control: Getting initial presence values.\n");

	// Get initial presence detect values
	for (int i = 0; i < AE_JACK_COUNT; i++) {
		gpio_ext_pin_get_value(&presence->irqs[i].gpioExtPin, &value);
		initialPresence[i] = !value;
	}

	PRINT("presence_control: Enabling presence detect IRQs.\n");

	// Enable irqs once init is done
	for (int i = 0; i < AE_JACK_COUNT; i++) {
		gpio_ext_irq_enable(&presence->irqs[i], TRUE);
	}

	return 0;
}

int presence_control_uninit(struct presence_control *presence) {

	PRINT("presence_control: Uninitializing control interface.\n");

	// Uninitialize all irqs
	for (int i = 0; i < AE_JACK_COUNT; i++) {
		gpio_ext_irq_uninit(&presence->irqs[i]);
	}

	memset(presence, 0, sizeof(*presence));

	return 0;
}
