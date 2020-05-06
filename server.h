#ifndef SERVER_H
#define SERVER_H 

#include "common_server.h"
#include <byteswap.h>
#include <stdlib.h>

typedef struct dbus_server {
	server_t server;
}dbus_server_t;

struct call_sum {
	char endian;
	int header_size;
	int body_len;
	int parameter_num;
};

int dbus_server_create(dbus_server_t *self);
void dbus_server_destroy(dbus_server_t *self);

int dbus_server_bind_and_listen(dbus_server_t *self, const char* port);
int dbus_server_accept(dbus_server_t *self);

/*Recibe el mensaje del cliente y le envia OK indicando que recibio
el mensaje correctamente*/
int dbus_server_recv(dbus_server_t *self);

//Funcion interna dbus_server_recv.
int dbus_server_recv_info(dbus_server_t *self, struct call_sum *summary);

//Funcion interna dbus_server_recv.
int dbus_server_recv_header(dbus_server_t *self, struct call_sum *summary);

//Funcion interna dbus_server_recv.
int dbus_server_recv_body(dbus_server_t *self, struct call_sum *summary);

#endif
