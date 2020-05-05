#include "common_server.h"

int server_create(server_t *self) {
	int status = socket_create(&self->skt_acep);
	
	return status;
}

void server_destroy(server_t *self) {
	socket_shut_down(&self->peer_skt);
	socket_destroy(&self->peer_skt);
	socket_shut_down(&self->skt_acep);
	socket_destroy(&self->skt_acep);
}

int server_bind_and_listen(server_t *self, const char* port, int size) {
	int status;

	//RESETEAMOS EL SOCKET POR SI ESTUVIESE EN TIME WAIT
	status = socket_reset(&self->skt_acep);
	if (status == ERROR) {
		server_destroy(self);
		return ERROR;
	}

	status = socket_bind_and_listen(&self->skt_acep, port, size); 
	if (status == ERROR) return ERROR;
	
	return 0;
}

int server_accept(server_t *self) {
	int result;
	result = socket_accept(&self->skt_acep, &self->peer_skt);
	return result;
}

int server_send(server_t *self, char *message, int len) {
	int result;
	result = socket_send(&self->peer_skt, message, len);
	return result;
}

int server_recv(server_t *self, char *buf, int size) {
	int result;
	result = socket_recv(&self->peer_skt, buf, size);
	return result;
}

