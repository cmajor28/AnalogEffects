#include "button_control.h"

static int button_state_changed(struct button_irq_context *context) {

	int ret;
	bool value;

	gpio_ext_pin_get_value(&context->irq->gpioExtPin, &value);

	switch (context->state) {
	case BUTTON_RELEASED:
		if (value) {
			// This should not happen
		} else {
			context->state = BUTTON_PRESSED;
			ret = timer_enable(&context->holdTimer, TRUE); // Start hold timer
			if (ret != 0) {
				// Don't handle this because we can recover
			}
		}
		break;
	case BUTTON_PRESSED:
		if (value) {
			context->state = BUTTON_RELEASED;
			ret = timer_enable(&context->holdTimer, FALSE); // Stop hold timer
			if (ret != 0) {
				// Don't handle this because we can recover
			}
		} else {
			// Button was held
			context->state = BUTTON_HELD;
		}
		break;
	case BUTTON_HELD:
		if (value) {
			// Button was released
			context->state = BUTTON_RELEASED;
		} else {
			// This should not happen
		}
		break;
	default:
		return -1;
	}

	context->callback(context->context, context->button, context->state);

	return 0;
}

int button_control_init(struct button_control *buttons, int (*callback)(void *, enum ae_button, enum button_state), void *context) {

	PRINT("button_control: Initializing control interface.\n");

	PRINT("button_control: Creating button press IRQs.\n");

	// Initialize all irqs
	for (int i = 0; i < AE_BUTTON_COUNT; i++) {
		// Set button update callback info
		buttons->irqContextList[i].button = i;
		buttons->irqContextList[i].irq = &buttons->irqs[i];
		buttons->irqContextList[i].state = BUTTON_RELEASED;
		buttons->irqContextList[i].callback = callback;
		buttons->irqContextList[i].context = context;

		// Create hold timer
		timer_init(	&buttons->irqContextList[i].holdTimer,
					TRUE,
					MILLISECONDS,
					BUTTON_HOLD_TIME_MILLISECONDS,
					(int (*)(void *))&button_state_changed,
					&buttons->irqContextList[i]);

		// Initialize gpio ext irq
		gpio_ext_irq_init(	&buttons->irqs[i],
							&buttons->pins[i],
							(int (*)(void *))&button_state_changed,
							&buttons->irqContextList[i],
							GPIO_DIR_IN,
							GPIO_SEN_BOTH);

	}

	buttons->callback = callback;
	buttons->context = context;

	PRINT("button_control: Enabling button press IRQs.\n");

	// Enable irqs once init is done
	for (int i = 0; i < AE_BUTTON_COUNT; i++) {
		gpio_ext_irq_enable(&buttons->irqs[i], TRUE);
	}

	return 0;
}

int button_control_uninit(struct button_control *buttons) {

	PRINT("button_control: Uninitializing interface.\n");

	// Uninitialize all irqs
	for (int i = 0; i < AE_BUTTON_COUNT; i++) {
		gpio_ext_irq_uninit(&buttons->irqs[i]);
	}

	memset(buttons, 0, sizeof(*buttons));

	return 0;
}
