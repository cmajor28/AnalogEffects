#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#define BLUETOOTH_ADDR_LEN 18

struct bluetooth {
	int		socket;
	char	address[BLUETOOTH_ADDR_LEN];
};

int bluetooth_init(struct bluetooth *bluetooth, char address[18]);

int bluetooth_uninit(struct bluetooth *bluetooth);

int bluetooth_read(struct bluetooth *bluetooth, void *buffer, unsigned int size);

int bluetooth_write(struct bluetooth *bluetooth, void *buffer, unsigned int size);

#endif // BLUETOOTH_H_
