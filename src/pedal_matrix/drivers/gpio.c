#include "gpio.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <poll.h>

static int gpio_irq_func(struct gpio_irq *irq) {

	int ret;
	char tmp;
	bool val;
	bool lastVal;
	struct pollfd fds;
	bool debounceEnable = irq->debounceTime > 0;

	// Read initial value
	if (debounceEnable) {
		ret = gpio_pin_get_value(&irq->gpioPin, &val);
		if (ret != 0) {
			return ret;
		}
	}

	// Wait for irq
	while (TRUE) {
		lastVal = val;

		// Set fds fields
		fds.fd = irq->fd;
		fds.events = POLLPRI | POLLERR;
		fds.revents = 0;

		// Poll for input
		lseek(fds.fd, 0, SEEK_SET);
		read(fds.fd, &tmp, sizeof(tmp)); // Dummy read
		ret = poll(&fds, 1, -1);
		if (ret < 0) {
			PRINT_LOG("poll() failed!");
			return ret;
		} else if (ret == 0) {
			// This shouldn't happen
			continue;
		} else {
//			if (fds.revents & POLLERR) {
//				PRINT_LOG("poll() failed!");
//				continue;
//			}
		}

		// Debounce signal
		if (debounceEnable) {
			sleep2(MILLISECONDS, irq->debounceTime);

			// Read new value
			ret = gpio_pin_get_value(&irq->gpioPin, &val);
			if (ret != 0) {
				return ret;
			}

			// Check for valid change
			switch (irq->sensitivity) {
			case GPIO_SEN_RISING:
				if (val) {
					break;
				}
				continue;
			case GPIO_SEN_FALLING:
				if (!val) {
					break;
				}
				continue;
			case GPIO_SEN_BOTH:
				if (val != lastVal) {
					break;
				}
				continue;
			default:
				continue;
			}
		}

		// Call isr
		if (irq->enabled && irq->callback) {
			pthread_t isrThread;

			//irq->callback(irq->context);

			// Using a thread allows for better interrupt handling
			ret = pthread_create(&isrThread, NULL, (void * (*)(void *))irq->callback, irq->context);
			if (ret != 0) {
				PRINT_LOG("pthread_create() failed!");
				return ret;
			}
		}
	}

	return 0;
}

int gpio_wakeup(bool wake) {

	volatile void *mmapAddress;
	int fd;

	// Open mem fd
	fd = open("/dev/mem", O_RDWR);
	if (fd == -1) {
		PRINT_LOG("open() failed!");
		return -1;
	}

	// Do memory map of registers
	mmapAddress = mmap(0, L4_WKUP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, L4_WKUP_START_ADDR);

	// Don't need fd after mmap
	close(fd);

	if (mmapAddress == MAP_FAILED) {
		return -1;
	}

	if (wake) {
		// Wake GPIO interfaces
		for (int i = 0; i < GPIO_COUNT; i++) {
			volatile uint32_t *loc;
			switch (i) {
			case GPIO0:
				loc = mmapAddress + L4_WKUP_CM_WKUP_GPIO0_CLKCTRL_OFFSET;
				break;
			case GPIO1:
				loc = mmapAddress + L4_WKUP_CM_PER_GPIO1_CLKCTRL_OFFSET;
				break;
			case GPIO2:
				loc = mmapAddress + L4_WKUP_CM_PER_GPIO2_CLKCTRL_OFFSET;
				break;
			case GPIO3:
				loc = mmapAddress + L4_WKUP_CM_PER_GPIO3_CLKCTRL_OFFSET;
				break;
			}

			// Enable module
			if (*loc & L4_WKUP_IDLEST_MASK) {
				*loc |= L4_WKUP_MODULEMODE_ENABLE_MASK;
				while (*loc & L4_WKUP_IDLEST_MASK);
			}
		}
	} else {
		// Idle GPIO interfaces
		for (int i = GPIO_COUNT-1; i >= 0; i--) {
			volatile uint32_t *loc;
			switch (i) {
			case GPIO0:
				loc = mmapAddress + L4_WKUP_CM_WKUP_GPIO0_CLKCTRL_OFFSET;
				break;
			case GPIO1:
				loc = mmapAddress + L4_WKUP_CM_PER_GPIO1_CLKCTRL_OFFSET;
				break;
			case GPIO2:
				loc = mmapAddress + L4_WKUP_CM_PER_GPIO2_CLKCTRL_OFFSET;
				break;
			case GPIO3:
				loc = mmapAddress + L4_WKUP_CM_PER_GPIO3_CLKCTRL_OFFSET;
				break;
			}

			// Disable module
			if (!(*loc & L4_WKUP_IDLEST_MASK)) {
				*loc &= (~L4_WKUP_MODULEMODE_ENABLE_MASK);
				while (!(*loc & L4_WKUP_IDLEST_MASK));
			}
		}
	}

	// We don't need this memory anymore
	munmap((void *)mmapAddress, L4_WKUP_SIZE);

	return 0;
}

int gpio_init(struct gpio *gpio, enum gpio_bank bank) {

	off_t startAddr;
	int size;

	PRINT("gpio-%d: Initializing gpio.\n", bank);

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
		PRINT_LOG("open() failed!");
		return -1;
	}

	// Do memory map of registers
	gpio->mmapAddress = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, startAddr);

	// Don't need fd after mmap
	close(fd);

	if (gpio->mmapAddress == MAP_FAILED) {
		return -1;
	}

	gpio->size = size;
	gpio->bank = bank;

	return 0;
}

int gpio_uninit(struct gpio *gpio) {

	PRINT("gpio-%d: Uninitializing gpio.\n", gpio->bank);

	// Unmap memory
	munmap((void *)gpio->mmapAddress, gpio->size);

	memset(gpio, 0, sizeof(*gpio));

	return 0;
}

int gpio_set_bit(struct gpio *gpio, uint32_t reg, uint8_t bit, uint8_t set) {

	PRINTV("gpio-%d: Setting reg 0x%04X bit %u to %u.\n", gpio->bank, reg, bit, set);

	// Calculate register location
	volatile uint32_t *loc = gpio->mmapAddress + reg;
	uint32_t tmp = *loc;

	// Set bit to value
	tmp ^= (-set ^ tmp) & (1 << bit);
	*loc = tmp;

	return 0;
}

int gpio_set_bits(struct gpio *gpio, uint32_t reg, uint32_t bits, uint32_t value) {

	PRINTV("gpio-%d: Setting reg 0x%04X bits 0x%08X to 0x%08X.\n", gpio->bank, reg, bits, value);

	// Calculate register location
	volatile uint32_t *loc = gpio->mmapAddress + reg;
	uint32_t tmp = *loc;

	// Set bit to value
	tmp ^= (-bits ^ tmp) & value;
	*loc = tmp;

	return 0;
}


int gpio_set_value(struct gpio *gpio, uint32_t reg, uint32_t value) {

	PRINTV("gpio-%d: Setting reg 0x%04X to 0x%08X.\n", gpio->bank, reg, value);

	// Calculate register location
	volatile uint32_t *loc = gpio->mmapAddress + reg;

	*loc = value;

	return 0;
}

int gpio_set_one_hot(struct gpio *gpio, uint32_t reg, uint8_t bit) {

	PRINTV("gpio-%d: Setting reg 0x%04X to 0x%08X.\n", gpio->bank, reg, 1 << bit);

	// Calculate register location
	volatile uint32_t *loc = gpio->mmapAddress + reg;
	uint32_t tmp = (1 << bit);

	// Set bit to value (one-hot)
	*loc = tmp;

	return 0;
}

int gpio_get_bit(struct gpio *gpio, uint32_t reg, uint8_t bit, uint8_t *set) {

	// Calculate register location
	volatile uint32_t *loc = gpio->mmapAddress + reg;

	// Set bit to value
	*set = (*loc & (1 << bit)) >> bit;

	return 0;
}

int gpio_get_bits(struct gpio *gpio, uint32_t reg, uint32_t bits, uint32_t *value) {

	// Calculate register location
	volatile uint32_t *loc = gpio->mmapAddress + reg;

	// Set bit to value
	*value = *loc & bits;

	return 0;
}

int gpio_get_value(struct gpio *gpio, uint32_t reg, uint32_t *value) {

	// Calculate register location
	volatile uint32_t *loc = gpio->mmapAddress + reg;

	*value = *loc;

	return 0;
}

int gpio_irq_init(struct gpio_irq *irq, struct gpio_pin *gpioPin, int (*callback)(void *), void *context, enum gpio_direction direction, enum gpio_sensitivity sensitivity, int debounceTime) {

	int gpioNumber = gpioPin->gpio->bank * 32 + gpioPin->pin;
	int ret;
	static char fileBuffer[256];
	char *dirStr;
	char *senStr;
	char numStr[4];

	PRINT("gpio-%d: Initializing IRQ on pin %d.\n", gpioPin->gpio->bank, gpioPin->pin);

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

	PRINT("gpio-%d: Pin %d IRQ - dir=%s sen=%s.\n", gpioPin->gpio->bank, gpioPin->pin, dirStr, senStr);

	// Enable gpio export
	snprintf(numStr, sizeof(numStr), "%d", gpioNumber);
	write_to_file("/sys/class/gpio/unexport", numStr, strlen(numStr)+1); // Just in case
	ret = write_to_file("/sys/class/gpio/export", numStr, strlen(numStr)+1);
	if (ret != 0) {
		return ret;
	}

	// Set direction
	snprintf(fileBuffer, sizeof(fileBuffer), "/sys/class/gpio/gpio%d/direction", gpioNumber);
	ret = write_to_file(fileBuffer, dirStr, strlen(dirStr)+1);
	if (ret != 0) {
		return ret;
	}

	// Set sensitivity
	snprintf(fileBuffer, sizeof(fileBuffer), "/sys/class/gpio/gpio%d/edge", gpioNumber);
	ret = write_to_file(fileBuffer, senStr, strlen(senStr)+1);
	if (ret != 0) {
		return ret;
	}

	// Open file for int
	snprintf(fileBuffer, sizeof(fileBuffer), "/sys/class/gpio/gpio%d/value", gpioNumber);
	irq->fd = open(fileBuffer, O_RDONLY | O_NONBLOCK);
	if (irq->fd == -1) {
		PRINT_LOG("open() failed!");
		return -1;
	}

	irq->gpioPin = *gpioPin;
	irq->callback = callback;
	irq->context = context;
	irq->direction = direction;
	irq->sensitivity = sensitivity;
	irq->debounceTime = debounceTime;
	irq->enabled = FALSE;

	// Create thread to handle interrupt
	ret = pthread_create(&irq->intThread, NULL, (void *(*)(void *))&gpio_irq_func, irq);
	if (ret != 0) {
		PRINT_LOG("pthread_create() failed!");
		close(irq->fd);
		return ret;
	}

	return 0;
}

int gpio_irq_uninit(struct gpio_irq *irq) {

	int gpioNumber = irq->gpioPin.gpio->bank * 32 + irq->gpioPin.pin;
	int ret;
	char numStr[4];

	PRINT("gpio-%d: Uninitializing IRQ on pin %d.\n", irq->gpioPin.gpio->bank, irq->gpioPin.pin);

	// Cancel interrupt thread
	ret = pthread_cancel(irq->intThread);
	if (ret != 0) {
		PRINT_LOG("pthread_cancel() failed!");
		return ret;
	}

	// Wait for thread to exit
	ret = pthread_join(irq->intThread, NULL);
	if (ret != 0) {
		PRINT_LOG("pthread_join() failed!");
		return ret;
	}

	// Close int file
	close(irq->fd);

	// Disable gpio export
	snprintf(numStr, sizeof(numStr), "%d", gpioNumber);
	ret = write_to_file("/sys/class/gpio/unexport", numStr, strlen(numStr)+1);
	if (ret != 0) {
		return ret;
	}

	memset(irq, 0, sizeof(*irq));

	return 0;
}

int gpio_irq_enable(struct gpio_irq *irq, bool enable) {

	PRINT("gpio-%d: %s IRQ on pin %d.\n", irq->gpioPin.gpio->bank, enable ? "Enabling" : "Disabling", irq->gpioPin.pin);
	irq->enabled = enable;
	return 0;
}

int gpio_irq_is_enabled(struct gpio_irq *irq, bool *enabled) {

	*enabled = irq->enabled;
	return 0;
}

int gpio_pin_set_value(struct gpio_pin *gpioPin, bool value) {

	int ret;
	ret = gpio_set_one_hot(gpioPin->gpio, value ? GPIO_SETDATAOUT : GPIO_CLEARDATAOUT, gpioPin->pin);
	return ret;
}

int gpio_pin_get_value(struct gpio_pin *gpioPin, bool *value) {

	int ret;
	ret = gpio_get_bit(gpioPin->gpio, GPIO_DATAIN, gpioPin->pin, (uint8_t *)value);
	return ret;
}
