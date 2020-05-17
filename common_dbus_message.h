#ifndef COMMON_DBUS_MESSAGE_H
#define COMMON_DBUS_MESSAGE_H 

#include "common_resizable_buffer.h"
#include "common_socket.h"

#define INFO_SIZE 16

typedef struct dbus_message {
	char info [INFO_SIZE];
	resizable_buffer_t header;
	resizable_buffer_t body;
}dbus_message_t;

int dbus_message_create(dbus_message_t *self);
void dbus_message_destroy(dbus_message_t *self);

/*Setea el dbus_message con la informacion de la llamada 
que le pasan por paraemtro*/
int dbus_message_set(dbus_message_t *self, resizable_buffer_t *call, int id);

//Funcion interna de dbus_message_set
int dbus_message_header_set(dbus_message_t *self, resizable_buffer_t *call);

//Funcion interna de dbus_message_set
int dbus_message_body_set(dbus_message_t *self, resizable_buffer_t *call);

//Funcion interna de dbus_message_set
void dbus_message_info_set(dbus_message_t *self, int id_counter);

//Al finalizar,se destruye el dbus_message
int dbus_message_send(dbus_message_t *self, socket_t *socket);



#endif
