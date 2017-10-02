#include "gpio.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <poll.h>

static int gpio_irq_func(struct gpio_irq *irq) {

	int ret;
	struct pollfd fds;

	fds.fd = irq->fd;
	fds.events = POLLPRI | POLLERR;
	fds.revents = 0;

	// Wait for irq
	while (TRUE) {
		ret = poll(&fds, 1, 0);
		if (ret != 0) {
			return ret;
		}

		// Call isr
		if (irq->enabled && irq->callback) {
			irq->callback(irq->context);
		}
	}

	return 0;
}

int gpio_init(struct gpio *gpio, enum gpio_bank bank) {

	off_t startAddr;
	int size;

	// Get start address and size
	switch (bank) {
	case GPIO0:
		startAddr = GPIO0_START_ADDR;
		size      = GPIO0_SIZE;
		break;
	case GPIO1:
		startAddr = GPIO1_START_ADDR;
		size      = GPIO1_SIZE;
		break;
	case GPIO2:
		startAddr = GPIO2_START_ADDR;
		size      = GPIO2_SIZE;
		break;
	case GPIO3:
		startAddr = GPIO3_START_ADDR;
		size      = GPIO3_SIZE;
		break;
	default:
		return -1;
	}

	int fd;

	// Open mem fd
	fd = open("/dev/mem", O_RDWR);
	if (fd == -1) {
		return -1;
	}

	// Do memory map of registers
	gpio->mmap_address = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, startAddr);

	// Don't need fd after mmap
	close(fd);

	if (gpio->mmap_address == MAP_FAILED) {
		return -1;
	}

	gpio->size = size;
	gpio->bank = bank;

	return 0;
}

int gpio_uninit(struct gpio *gpio) {

	// Unmap memory
	munmap((void *)gpio->mmap_address, gpio->size);

	memset(gpio, 0, sizeof(*gpio));

	return 0;
}

int gpio_set_bit(struct gpio *gpio, uint32_t reg, uint8_t bit, uint8_t set) {

	// Calculate register location
	volatile uint32_t *loc = gpio->mmap_address + reg;
	uint32_t tmp = *loc;

	// Set bit to value
	tmp ^= (-set ^ tmp) & (1 << bit);
	*loc = tmp;

	return 0;
}

int gpio_set_bits(struct gpio *gpio, uint32_t reg, uint32_t bits, uint32_t value) {

	// Calculate register location
	volatile uint32_t *loc = gpio->mmap_address + reg;
	uint32_t tmp = *loc;

	// Set bit to value
	tmp ^= (-bits ^ tmp) & value;
	*loc = tmp;

	return 0;
}


int gpio_set_value(struct gpio *gpio, uint32_t reg, uint32_t value) {

	// Calculate register location
	volatile uint32_t *loc = gpio->mmap_address + reg;

	*loc = value;

	return 0;
}

int gpio_get_bit(struct gpio *gpio, uint32_t reg, uint8_t bit, uint8_t *set) {

	// Calculate register location
	volatile uint32_t *loc = gpio->mmap_address + reg;

	// Set bit to value
	*set = (*loc & (1 << bit)) >> bit;

	return 0;
}

int gpio_get_bits(struct gpio *gpio, uint32_t reg, uint32_t bits, uint32_t *value) {

	// Calculate register location
	volatile uint32_t *loc = gpio->mmap_address + reg;

	// Set bit to value
	*value = *loc & bits;

	return 0;
}

int gpio_get_value(struct gpio *gpio, uint32_t reg, uint32_t *value) {

	// Calculate register location
	volatile uint32_t *loc = gpio->mmap_address + reg;

	*value = *loc;

	return 0;
}

int gpio_irq_init(struct gpio_irq *irq, struct gpio_pin *gpioPin, int (*callback)(void *), void *context, enum gpio_direction direction, enum gpio_sensitivity sensitivity) {

	int gpioNumber = gpioPin->gpio->bank * 32 + gpioPin->pin;
	int ret;
	static char fileBuffer[256];
	char *dirStr;
	char *senStr;
	char numStr[4];

	// Set direction string
	switch (direction) {
	case GPIO_DIR_IN:
		dirStr = "in";
		break;
	case GPIO_DIR_OUT:
		dirStr = "out";
		break;
	case GPIO_DIR_NONE:
	default:
		return -1;
	}

	// Set sensitivity string
	switch (sensitivity) {
	case GPIO_SEN_NONE:
		senStr = "none";
		break;
	case GPIO_SEN_RISING:
		senStr = "rising";
		break;
	case GPIO_SEN_FALLING:
		senStr = "falling";
		break;
	case GPIO_SEN_BOTH:
		senStr = "both";
		break;
	default:
		return -1;
	}

	// Enable gpio export
	snprintf(numStr, sizeof(numStr), "%d", gpioNumber);
	ret = write_to_file("sys/class/gpio/export", numStr, strlen(numStr));
	if (ret != 0) {
		return ret;
	}

	// Set direction
	snprintf(fileBuffer, sizeof(fileBuffer), "/sys/class/gpio/gpio%d/direction", gpioNumber);
	ret = write_to_file(fileBuffer, dirStr, strlen(dirStr));
	if (ret != 0) {
		return ret;
	}

	// Set sensitivity
	snprintf(fileBuffer, sizeof(fileBuffer), "/sys/class/gpio/gpio%d/edge", gpioNumber);
	ret = write_to_file(fileBuffer, senStr, strlen(senStr));
	if (ret != 0) {
		return ret;
	}

	// Open file for int
	snprintf(fileBuffer, sizeof(fileBuffer), "/sys/class/gpio/gpio%d/value", gpioNumber);
	irq->fd = open(fileBuffer, O_RDONLY);
	if (irq->fd == -1) {
		return -1;
	}

	// Create thread to handle interrupt
	ret = pthread_create(&irq->intThread, NULL, (void *(*)(void *))&gpio_irq_func, irq);
	if (ret != 0) {
		close(irq->fd);
		return ret;
	}

	irq->gpioPin = *gpioPin;
	irq->callback = callback;
	irq->context = context;
	irq->direction = direction;
	irq->sensitivity = sensitivity;
	irq->enabled = FALSE;

	return 0;
}

int gpio_irq_uninit(struct gpio_irq *irq) {

	int gpioNumber = irq->gpioPin.gpio->bank * 32 + irq->gpioPin.pin;
	int ret;
	char numStr[4];

	// Cancel interrupt thread
	ret = pthread_cancel(irq->intThread);
	if (ret != 0) {
		return ret;
	}

	// Wait for thread to exit
	ret = pthread_join(irq->intThread, NULL);
	if (ret != 0) {
		return ret;
	}

	// Close int file
	close(irq->fd);

	// Disable gpio export
	snprintf(numStr, sizeof(numStr), "%d", gpioNumber);
	ret = write_to_file("sys/class/gpio/unexport", numStr, strlen(numStr));
	if (ret != 0) {
		return ret;
	}

	memset(irq, 0, sizeof(*irq));

	return 0;
}

int gpio_irq_enable(struct gpio_irq *irq, bool enable) {

	irq->enabled = enable;
	return 0;
}

int gpio_irq_is_enabled(struct gpio_irq *irq, bool *enabled) {

	*enabled = irq->enabled;
	return 0;
}

int gpio_pin_set_value(struct gpio_pin *gpioPin, bool value) {

	int ret;
	ret = gpio_set_bit(gpioPin->gpio, GPIO_DATAOUT, gpioPin->pin, value ? 0x1 : 0x0);
	return ret;
}

int gpio_pin_get_value(struct gpio_pin *gpioPin, bool *value) {

	int ret;
	ret = gpio_get_bit(gpioPin->gpio, GPIO_DATAIN, gpioPin->pin, (uint8_t *)value);
	return ret;
}
