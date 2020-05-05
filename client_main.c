#include "client.h"


int main(int argc, char const *argv[]) {
	int status;
	dbus_client_t dbus_client;
	status = dbus_client_create(&dbus_client, argv[3]);
	if (status == ERROR) return ERROR;
	status = dbus_client_connect(&dbus_client, argv[1], argv[2]);
	if (status == ERROR) return ERROR;
	status = dbus_client_send(&dbus_client);
	if (status == ERROR) return ERROR;
	dbus_client_destroy(&dbus_client);
	return 0;
}
