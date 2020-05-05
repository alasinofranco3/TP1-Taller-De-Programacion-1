#ifndef COMMON_SERVER_H
#define COMMON_SERVER_H 

#include "common_socket.h"

typedef struct server {
	socket_t skt_acep;
	socket_t peer_skt;
}server_t;

int server_create(server_t *self);
void server_destroy(server_t *self);

/* En caso de error se detruye el servidor,junto con sus sockets.Ademas se liberan
los recursos asignados al struct addrinfo utilizado*/
int server_bind_and_listen(server_t *self, const char* port, int size);

int server_accept(server_t *self);
int server_send(server_t *self, char *message, int len);
int server_recv(server_t *self, char *buf, int size);


#endif

