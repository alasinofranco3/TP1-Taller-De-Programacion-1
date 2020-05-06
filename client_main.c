#include "client.h"


int main(int argc, char const *argv[]) {
	int status;
	dbus_client_t dbus_client;
	resizable_buffer_t buffer;
	char line_remains [MAX_BUF_SIZE];
	memset(line_remains, 0, MAX_BUF_SIZE);
	
	status = dbus_client_create(&dbus_client, argv[3]);
	if (status == ERROR) return ERROR;
	status = dbus_client_connect(&dbus_client, argv[1], argv[2]);
	if (status == ERROR) return ERROR;

	while (!feof(dbus_client.file)) {
		status = dbus_client_get_call(&dbus_client, &buffer, line_remains);
		if  (status == ERROR) return ERROR;
		status = dbus_client_send(&dbus_client, &buffer);
		if (status == ERROR) return ERROR;
	}
	dbus_client_destroy(&dbus_client);
	return 0;
}
