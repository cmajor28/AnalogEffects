#include "bluetooth.h"
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

int bluetooth_init(struct bluetooth *bluetooth, char address[BLUETOOTH_ADDR_LEN]) {

	int ret;
	struct sockaddr_rc sockAddr;
	int sock;

	// Create bluetooth socket
	sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	if (sock == -1) {
		return -1;
	}

	sockAddr.rc_family = AF_BLUETOOTH;
	sockAddr.rc_channel = (uint8_t)1;
	str2ba(address, &sockAddr.rc_bdaddr);

	// Connect to the socket
	ret = connect(sock, (struct sockaddr *)&sockAddr, sizeof(sockAddr));
	if (ret == -1) {
		close(sock);
	}

	bluetooth->socket = sock;
	memcpy(bluetooth->address, address, sizeof(*address) * BLUETOOTH_ADDR_LEN);

	return 0;
}

int bluetooth_uninit(struct bluetooth *bluetooth) {

	close(bluetooth->socket);
	memset(bluetooth, 0, sizeof(*bluetooth));

	return 0;
}

int bluetooth_read(struct bluetooth *bluetooth, void *buffer, unsigned int size) {

	int ret;
	ret = read(bluetooth->socket, buffer, size);
	if (ret != size) {
		return -1;
	}

	return 0;
}

int bluetooth_write(struct bluetooth *bluetooth, void *buffer, unsigned int size) {

	int ret;
	ret = write(bluetooth->socket, buffer, size);
	if (ret != size) {
		return -1;
	}

	return 0;
}
