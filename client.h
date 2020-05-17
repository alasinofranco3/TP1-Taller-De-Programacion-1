#ifndef CLIENT_H
#define CLIENT_H

#include "common_dbus_message.h"
#include "common_parser.h"

typedef struct dbus_client{
	FILE* file;
	int id_counter;
	//client_t client;
	socket_t socket;
}dbus_client_t;

int dbus_client_create(dbus_client_t *self, const char* file_name);
void dbus_client_destroy(dbus_client_t *self);

/*Esta funcion nos permite obtener una llamada del archivo del cliente.
la linea se guarda en el buffer b y en el vector r se almacena la parte
de la linea que nos quedo sin procesar*/
int dbus_client_get_call(dbus_client_t *self, resizable_buffer_t *b, char* r);

/*intenta conectar al cliente al host h en el puerto p*/
int dbus_client_connect(dbus_client_t *self, const char* h, const char* p);

/*Construye el dbus_message a enviar al servidor a partir del buffer
recibido por parametro,lo envia y espera a recibir el OK del servidor*/
int dbus_client_send(dbus_client_t *self, resizable_buffer_t *buffer);


#endif
