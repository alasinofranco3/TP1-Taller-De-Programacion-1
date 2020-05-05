#ifndef CLIENT_H
#define CLIENT_H

#define MAX_BUF_SIZE 32

#include "common_dbus_message.h"

typedef struct dbus_client{
	FILE* file;
	int id_counter;
	client_t client;
}dbus_client_t;

int dbus_client_create(dbus_client_t *self, const char* file_name);
void dbus_client_destroy(dbus_client_t *self);

/*Esta funcion nos permite obtener una llamada del archivo del cliente.
s determina el tamanio del buffer que vamos a usar para leer el archivo*/
int dbus_client_get_call(dbus_client_t *self, resizable_buffer_t *buf, int s);

/*intenta conectar al cliente al host h en el puerto p*/
int dbus_client_connect(dbus_client_t *self, const char* h, const char* p);
int dbus_client_send(dbus_client_t *self);


#endif
