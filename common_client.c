#include "common_client.h"


int client_create(client_t *self) {
	int status = socket_create(&self->skt);

	return status;
}

void client_destroy(client_t *self) {
	socket_shut_down(&self->skt);
	socket_destroy(&self->skt);
}

int client_connect(client_t *self, const char* h, const char* p) {
	int result = 0;
	result = socket_connect(&self->skt, h, p);
	if (result == ERROR) return ERROR;	
	return 0;
}

int client_send(client_t *self, char *message, int len) {
	int result;
	result = socket_send(&self->skt, message, len);
	//EN CASO DE ERROR EL SOCKET SE ENCARGA DE DESTRUIRSE
	return result;
}

int client_recv(client_t *self, char *buf, int size) {
	int result;
	result = socket_recv(&self->skt, buf, size);
	//EN CASO DE ERROR EL SOCKET SE ENCARGA DE DESTRUIRSE
	return result;
}
