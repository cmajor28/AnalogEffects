#include "pcf857x.h"

static int gpio_ext_write(struct gpio_ext *gpioExt, uint16_t values) {

	static pthread_mutex_t funcMutex = PTHREAD_MUTEX_INITIALIZER;
	int ret;
	char bytes[2] = { values & 0xFF , values >> 8 };

	ret = pthread_mutex_lock(&funcMutex);
	if (ret != 0) {
		return ret;
	}

	// Writes new values to pins
	ret = i2c_write(gpioExt->i2c, bytes, gpioExt->numPins / 8);
	if (ret != 0) {
		pthread_mutex_unlock(&funcMutex);
		return ret;
	}

	gpioExt->values = values;

	pthread_mutex_unlock(&funcMutex);
	return 0;
}

static int gpio_ext_read(struct gpio_ext *gpioExt) {

	static pthread_mutex_t funcMutex = PTHREAD_MUTEX_INITIALIZER;
	int ret;
	char bytes[2] = { 0, 0 };
	uint16_t values;
	int old, new;

	ret = pthread_mutex_lock(&funcMutex);
	if (ret != 0) {
		return ret;
	}

	// Read pins
	ret = i2c_read(gpioExt->i2c, bytes, gpioExt->numPins / 8);
	if (ret != 0) {
		pthread_mutex_unlock(&funcMutex);
		return ret;
	}

	values = (bytes[1] << 8) | bytes[0];

	for (int i = 0; i < gpioExt->numPins; i++) {
		// Calculate values
		old = gpioExt->values & (1 << i);
		new = values & (1 << i);

		// Generate callback
		if (gpioExt->irqList[i] && gpioExt->irqList[i]->enabled && gpioExt->irqList[i]->callback &&
				gpioExt->irqList[i]->direction == GPIO_DIR_IN &&
				(((old > new) && (gpioExt->irqList[i]->sensitivity == GPIO_SEN_FALLING)) ||
				((old < new) && (gpioExt->irqList[i]->sensitivity == GPIO_SEN_RISING)) ||
				((old != new) && (gpioExt->irqList[i]->sensitivity == GPIO_SEN_BOTH)))) {
			gpioExt->irqList[i]->callback(gpioExt->irqList[i]->context);
		}
	}

	gpioExt->values = values;

	pthread_mutex_unlock(&funcMutex);
	return 0;
}

int gpio_ext_init(struct gpio_ext *gpioExt, enum pcf857x_pin_count numPins, struct i2c *i2c) {

	int ret;

	gpioExt->values = 0x0000;
	gpioExt->i2c = i2c;
	gpioExt->numPins = numPins;
	memset(gpioExt->irqList, 0, sizeof(gpioExt->irqList));

	// We don't have to have an int pin
	if (gpioExt->gpioPin.gpio == NULL || gpioExt->gpioPin.pin == -1) {
		return 0;
	}

	// Setup irq for interrupt pin
	ret = gpio_irq_init(&gpioExt->irq, &gpioExt->gpioPin, (int (*)(void *))&gpio_ext_read, gpioExt, GPIO_DIR_IN, GPIO_SEN_FALLING);
	if (ret != 0) {
		return ret;
	}

	// Enable interrupt pin irq
	ret = gpio_irq_enable(&gpioExt->irq, TRUE);
	if (ret != 0) {
		return ret;
	}

	return 0;
}

int gpio_ext_uninit(struct gpio_ext *gpioExt) {

	int ret;

	// Only remove irq if pin exists
	if (gpioExt->gpioPin.gpio == NULL || gpioExt->gpioPin.pin == -1) {
		ret = gpio_irq_uninit(&gpioExt->irq);
		if (ret != 0) {
			return ret;
		}
	}

	memset(gpioExt, 0, sizeof(*gpioExt));

	return 0;
}

int gpio_ext_set_bit(struct gpio_ext *gpioExt, uint8_t bit, uint8_t set) {

	int ret;
	uint16_t values = gpioExt->values;

	// Calculate new values
	values ^= (-set ^ values) & (1 << bit);

	ret = gpio_ext_write(gpioExt, values);
	return ret;
}

int gpio_ext_set_bits(struct gpio_ext *gpioExt, uint16_t bits, uint16_t value) {

	int ret;
	uint16_t values = gpioExt->values;

	// Calculate new values
	values ^= (-bits ^ values) & value;

	ret = gpio_ext_write(gpioExt, values);
	return ret;
}

int gpio_ext_set_value(struct gpio_ext *gpioExt, uint16_t value) {

	int ret;
	ret = gpio_ext_write(gpioExt, value);
	return ret;
}

int gpio_ext_get_bit(struct gpio_ext *gpioExt, uint8_t bit, uint8_t *set) {

	int ret;

	ret = gpio_ext_read(gpioExt);
	if (ret != 0) {
		return ret;
	}

	*set = (gpioExt->values & (1 << bit)) >> bit;

	return 0;
}

int gpio_ext_get_bits(struct gpio_ext *gpioExt, uint16_t bits, uint16_t *value) {

	int ret;

	ret = gpio_ext_read(gpioExt);
	if (ret != 0) {
		return ret;
	}

	*value = gpioExt->values & bits;

	return 0;
}

int gpio_ext_get_value(struct gpio_ext *gpioExt, uint16_t *value) {

	int ret;

	ret = gpio_ext_read(gpioExt);
	if (ret != 0) {
		return ret;
	}

	*value = gpioExt->values;

	return 0;
}

int gpio_ext_irq_init(struct gpio_ext_irq *extIrq, struct gpio_ext_pin *gpioExtPin, int (*callback)(void *), void *context, int direction, int sensitivity) {

	extIrq->gpioExtPin = *gpioExtPin;
	extIrq->callback = callback;
	extIrq->context = context;
	extIrq->direction = direction;
	extIrq->sensitivity = sensitivity;
	extIrq->enabled = FALSE;
	gpioExtPin->gpioExt->irqList[gpioExtPin->pin] = extIrq;

	return 0;
}

int gpio_ext_irq_uninit(struct gpio_ext_irq *extIrq) {

	extIrq->gpioExtPin.gpioExt->irqList[extIrq->gpioExtPin.pin] = NULL;
	memset(extIrq, 0, sizeof(*extIrq));

	return 0;
}

int gpio_ext_irq_enable(struct gpio_ext_irq *extIrq, bool enable) {

	extIrq->enabled = enable;
	return 0;
}

int gpio_ext_irq_is_enabled(struct gpio_ext_irq *extIrq, bool *enabled) {

	*enabled = extIrq->enabled;
	return 0;
}

int gpio_ext_pin_set_value(struct gpio_ext_pin *gpioExtPin, bool value) {

	int ret;
	ret = gpio_ext_set_bit(gpioExtPin->gpioExt, gpioExtPin->pin, value ? 0x1 : 0x0);
	return ret;
}

int gpio_ext_pin_get_value(struct gpio_ext_pin *gpioExtPin, bool *value) {

	int ret;
	ret = gpio_ext_get_bit(gpioExtPin->gpioExt, gpioExtPin->pin, (uint8_t *)value);
	return ret;
}
