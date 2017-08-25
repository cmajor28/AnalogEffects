#include "gpio.h"
#include "utils.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <poll.h>

struct gpio_irq_context {
	int		(*callback)(void *);
	void	*context;
	int		fd;
};

static int gpio_irq_func(struct gpio_irq_context *params) {

	int ret;
	struct pollfd fds;

	fds.fd = params->fd;
	fds.events = POLLPRI | POLLERR;
	fds.revents = 0;

	while (TRUE) {
		ret = poll(&fds, 1, 0);
		if (ret != 0) {
			return ret;
		}

		if (params->callback) {
			params->callback(params->context);
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

	// Do memory map
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

	// Set bit to value
	*loc ^= (-set ^ *loc) & (1 << bit);

	return 0;
}

int gpio_set_bits(struct gpio *gpio, uint32_t reg, uint32_t bits, uint32_t value) {

	// Calculate register location
	volatile uint32_t *loc = gpio->mmap_address + reg;

	// Set bit to value
	*loc ^= (-bits ^ *loc) & value;

	return 0;
}


int gpio_set(struct gpio *gpio, uint32_t reg, uint32_t value) {

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

int gpio_get(struct gpio *gpio, uint32_t reg, uint32_t *value) {

	// Calculate register location
	volatile uint32_t *loc = gpio->mmap_address + reg;

	*value = *loc;

	return 0;
}

int gpio_irq_init(struct gpio_irq *irq, struct gpio *gpio, uint8_t pin, int (*callback)(void *), void *context, int direction, int sensitivity) {

	int gpio_number = gpio->bank * 32 + pin;
	int fd;
	int ret;
	static char file_buffer[255];
	char *dir_str;
	char *sen_str;
	char num_str[4];

	switch (direction) {
	case GPIO_DIR_IN:
		dir_str = "in";
		break;
	case GPIO_DIR_OUT:
		dir_str = "out";
		break;
	case GPIO_DIR_NONE:
	default:
		return -1;
	}

	switch (sensitivity) {
	case GPIO_SEN_NONE:
		sen_str = "none";
		break;
	case GPIO_SEN_RISING:
		sen_str = "rising";
		break;
	case GPIO_SEN_FALLING:
		sen_str = "falling";
		break;
	case GPIO_SEN_BOTH:
		sen_str = "both";
		break;
	default:
		return -1;
	}

	// Enable gpio export
	itoa(gpio_number, num_str, 10);
	ret = write_to_file("sys/class/gpio/export", num_str, strlen(num_str));

	if (ret != 0) {
		return -1;
	}

	// Set direction
	snprintf(file_buffer, sizeof(file_buffer), "/sys/class/gpio/gpio%d/direction", gpio_number);
	write_to_file(file_buffer, dir_str, strlen(dir_str));

	if (ret != 0) {
		return -1;
	}

	// Set sensitivity
	snprintf(file_buffer, sizeof(file_buffer), "/sys/class/gpio/gpio%d/edge", gpio_number);
	write_to_file(file_buffer, sen_str, strlen(sen_str));

	if (ret != 0) {
		return -1;
	}

	struct gpio_irq_context gpio_irq_ctx;

	gpio_irq_ctx.callback = callback;
	gpio_irq_ctx.context = context;
	gpio_irq_ctx.fd = fd;

	// Create thread to handle interrupt
	ret = pthread_create(&irq->int_thread, NULL, &gpio_irq_func, &gpio_irq_ctx);

	if (ret != 0) {
		close(fd);
	}

	irq->gpio = gpio;
	irq->callback = callback;
	irq->context = context;
	irq->direction = direction;
	irq->sensitivity = sensitivity;
	irq->enabled = FALSE;

	return 0;
}

int gpio_irq_uninit(struct gpio_irq *irq) {

	return 0;
}

int gpio_irq_set_callback(int (*callback)(void *), void *context) {

	return 0;
}

int gpio_irq_set_direction(struct gpio_irq *irq, int direction) {

	return 0;
}

int gpio_irq_get_direction(struct gpio_irq *irq, int *direction) {

	return 0;
}

int gpio_irq_set_sensitivity(struct gpio_irq *irq, int sensitivity) {

	return 0;
}

int gpio_irq_get_sensitivity(struct gpio_irq *irq, int *sensitivity) {

	return 0;
}

int gpio_irq_enable(struct gpio_irq *irq, bool enable) {

	return 0;
}

int gpio_irq_is_enabled(struct gpio_irq *irq, bool *enabled) {

	return 0;
}

int gpio_pin_set_value(struct gpio_pin *gpioPin, bool value) {

	int ret;
	ret = gpio_set_bit(gpioPin->gpio, GPIO_DATAOUT, gpioPin->pin, value ? 0x1 : 0x0);
	return ret;
}

int gpio_pin_get_value(struct gpio_pin *gpioPin, bool *value) {

	int ret;
	ret = gpio_get_bit(gpioPin->gpio, GPIO_DATAIN, gpioPin->pin, value);
	return ret;
}
