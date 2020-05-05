#include "server.h"


int main(int argc, char const *argv[]) {
	int status;
	dbus_server_t dbus_server;
	status = dbus_server_create(&dbus_server);
	if (status == ERROR) return ERROR;
	status = dbus_server_bind_and_listen(&dbus_server, argv[1]);
	if (status == ERROR) return ERROR;
	status = dbus_server_accept(&dbus_server);
	if (status == ERROR) return ERROR;
	status = dbus_server_recv(&dbus_server);
	if (status == ERROR) return ERROR;
	dbus_server_destroy(&dbus_server);
	return 0;
}
