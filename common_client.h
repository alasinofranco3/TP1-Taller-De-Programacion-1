#ifndef COMMON_CLIENT_H
#define COMMON_CLIENT_H 

#include "common_socket.h"

typedef struct client {
	socket_t skt;
}client_t;

int client_create(client_t *self);
void client_destroy(client_t *self);

/* En caso de error se detruye el cliente.Ademas se liberan
los recursos asignados al struct addrinfo utilizado*/
int client_connect(client_t *self, const char* h, const char* p);

int client_send(client_t *self, char *message, int len);

int client_recv(client_t *self, char *buf, int size);

#endif
