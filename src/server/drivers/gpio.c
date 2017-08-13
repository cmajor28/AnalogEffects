#include "gpio.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int gpio_init(struct gpio *gpio, uint32_t bank) {

	if (gpio == NULL) {
		// TODO
	}

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
		// TODO
		break;
	}

	int fd;

	// Open mem fd
	fd = open("/dev/mem", O_RDWR);

	if (fd == -1) {
		// TODO
	}

	// Do memory map
	gpio->mmap_address = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, startAddr);

	// Don't need fd after mmap
	close(fd);

	if (gpio->mmap_address == MAP_FAILED) {
		close(fd);
		// TODO
	}

	return 0;
}

int gpio_uninit(struct gpio *gpio) {

	if (gpio == NULL) {
		// TODO
	}

	// Unmap memory
	munmap((void *)gpio->mmap_address, gpio->size);

	return 0;
}

int gpio_set_bit(struct gpio *gpio, uint32_t reg, uint8_t bit, uint8_t set) {

	if (gpio == NULL) {
		// TODO
	}

	// Calculate register location
	volatile uint32_t *loc = gpio->mmap_address + reg;

	// Set bit to value
	*loc ^= (-set ^ *loc) & (1 << bit);

	return 0;
}

int gpio_set_bits(struct gpio *gpio, uint32_t reg, uint32_t bits, uint32_t value) {

	if (gpio == NULL) {
		// TODO
	}

	// Calculate register location
	volatile uint32_t *loc = gpio->mmap_address + reg;

	// Set bit to value
	*loc ^= (-bits ^ *loc) & value;

	return 0;
}


int gpio_set(struct gpio *gpio, uint32_t reg, uint32_t value) {

	if (gpio == NULL) {
		// TODO
	}

	// Calculate register location
	volatile uint32_t *loc = gpio->mmap_address + reg;

	*loc = value;

	return 0;
}

int gpio_get_bit(struct gpio *gpio, uint32_t reg, uint8_t bit, uint8_t *set) {

	if (gpio == NULL || set == NULL) {
		// TODO
	}

	// Calculate register location
	volatile uint32_t *loc = gpio->mmap_address + reg;

	// Set bit to value
	*set = (*loc & (1 << bit)) >> bit;

	return 0;
}

int gpio_get_bits(struct gpio *gpio, uint32_t reg, uint32_t bits, uint32_t *value) {

	if (gpio == NULL || value == NULL) {
		// TODO
	}

	// Calculate register location
	volatile uint32_t *loc = gpio->mmap_address + reg;

	// Set bit to value
	*value = *loc & bits;

	return 0;
}

int gpio_get(struct gpio *gpio, uint32_t reg, uint32_t *value) {

	if (gpio == NULL || value == NULL) {
		// TODO
	}

	// Calculate register location
	volatile uint32_t *loc = gpio->mmap_address + reg;

	*value = *loc;

	return 0;
}
