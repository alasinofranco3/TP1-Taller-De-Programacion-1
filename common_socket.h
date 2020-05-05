#ifndef COMMON_SOCKET_H
#define COMMON_SOCKET_H

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#define ERROR -1

typedef struct socket {
	int skt;
}socket_t;

/*En caso de error liberan los recursos asignados al struct
addrinfo utilizado */
int socket_create(socket_t *self);

void socket_destroy(socket_t *self);

/* Size es la cantidad de clientes que podra teneer 
en espera el skt_aceptador */
int socket_bind_and_listen(socket_t *self, const char* port, int size);

int socket_connect(socket_t *self, const char* h, const char* p);

/* En caso de error se destruyen el socket y el peerskt */
int socket_accept(socket_t *self, socket_t *peerskt);

/*Reactiva una direccion en caso de que esta estuviese en TIME WAIT */
int socket_reset(socket_t *self);

void socket_shut_down(socket_t *self);

/*Devuelve 0 en caso de exito o ERROR en caso de error.
En el segundo caso se encarga de destruir el socket */
int socket_send(socket_t *self, char *message, int len);

/* Esta funcion recibe datos hasta que se llene el buffer o
hasta que se cierre el socket remoto.
La funcion devuelve 0 en caso de que se cierre el socket remoto
,ERROR en caso de error o los caracteres recibidos en otro caso.
En caso de error la funcion se encarga de destruir el socket */
int socket_recv(socket_t *self, char *buffer, int size);

#endif
