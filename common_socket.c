#define _POSIX_C_SOURCE 200112L 
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "common_socket.h"

//FUNCIONES PRIVADAS
static int error_in_getaddrinfo(int status, socket_t *skt) {
	if (status != 0) { 
  		printf("Error in getaddrinfo: %s\n", gai_strerror(status));
      	socket_destroy(skt);
      	return ERROR;
   	}
   	return 0;
}

static void set_TCP_options(struct addrinfo *hints) {
	memset(hints, 0, sizeof(struct addrinfo));
	hints->ai_family = AF_INET;
	hints->ai_socktype = SOCK_STREAM;
	hints->ai_flags = 0;
}


//FUNCIONES PUBLICAS
int socket_create(socket_t *self) {
	struct addrinfo hints;
	set_TCP_options(&hints);
	
	self->skt = socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);

	if (self->skt == -1) {
		printf("Error in socket():%s\n", strerror(errno));
		return ERROR;
	}
	
	return 0;		
}

void socket_destroy(socket_t *self) {
	close(self->skt);
}


int socket_bind_and_listen(socket_t *self, const char* port, int size) {
	int status;
	struct addrinfo *results, *ptr;
	struct addrinfo hints;
	bool success = false;

	set_TCP_options(&hints);
	
	status = getaddrinfo(NULL, port, &hints, &results);

	if (error_in_getaddrinfo(status, self) == ERROR) return ERROR;


	for (ptr = results; ptr != NULL && success == false; ptr = ptr->ai_next) {
		status = bind(self->skt, ptr->ai_addr, ptr->ai_addrlen);
		status = listen(self->skt, size);
		success = (status != ERROR);
	}

	freeaddrinfo(results);
	
	if (success == false) {
		printf("Binding and listening failed,no valid adrresses remaining\n");
		socket_destroy(self);
		return ERROR;
	}

	return 0;	
}

int socket_connect(socket_t *self, const char* h, const char* p) {
	int status = 0;
	bool connected = false;
	struct addrinfo *results, *ptr;
	struct addrinfo hints;

	set_TCP_options(&hints);

	status = getaddrinfo(h, p, &hints, &results);

	if (error_in_getaddrinfo(status, self) == ERROR) return ERROR;

	for (ptr = results; ptr != NULL && connected == false; ptr = ptr->ai_next) {
		// NO HACE FALTA CREAR EL SOCKET ACA PORQUE YA LO TENEMOS CREADO
		//DESDE ANTES USANDO LAS MISMAS HINTS QUE RESULT
		status = connect(self->skt, ptr->ai_addr, ptr->ai_addrlen);
		//NO CHEQUEAMOS SI HAY ERROR YA QUE TODAVIA PODRIAMOS 
		//CONECTARNOS CON LAS DIRECCIONES SIGUIENTES
		connected = (status != ERROR);
	}

	freeaddrinfo(results);

	if (connected == false) {
		socket_destroy(self);
		return ERROR;
	}

	return 0; 
}

int socket_accept(socket_t *self, socket_t *peerskt) {
	peerskt->skt = accept(self->skt, NULL, NULL);
	
	if (peerskt->skt == -1) {
		printf("Error in accept: %s\n", strerror(errno));
		socket_destroy(self);
		socket_destroy(peerskt);
		return ERROR;
	}

	return 0;
}

int socket_reset(socket_t *self) {
	int status;
	int val = 1;
	status = setsockopt(self->skt, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    if (status == -1) {
		printf("Error in reset: %s\n", strerror(errno));
		socket_destroy(self);
    	return ERROR;
    }
  
   return 0;
}

void socket_shut_down(socket_t *self) {
	shutdown(self->skt, SHUT_RDWR);
}

int socket_send(socket_t *self, char *message, int len) {
	int sent = 0;
	int result; 
	bool remote_skt_closed = false;

	while (remote_skt_closed == false && sent < len) {
		result = send(self->skt, &message[sent], (len - sent), MSG_NOSIGNAL);
		
		if (result == 0) {  //SI RESULT VALE CERO NOS CERRARON EL SKT
			remote_skt_closed = true;
		} else if (result == -1) {
			printf("Error in send: %s\n", strerror(errno));
			socket_shut_down(self);
			socket_destroy(self);
			return ERROR;
		} else {
			sent += result;
		}
	}
	
	if (remote_skt_closed) return 0;
	return sent;
}

int socket_recv(socket_t *self, char *buffer, int size) {
	int received = 0;
	int result;
	bool skt_closed = false;

	while (received < size && skt_closed == false) {
		result = recv(self->skt, &buffer[received], (size - received), 0);
		
		if (result == -1) {
			printf("Error in recv: %s\n", strerror(errno));
			socket_shut_down(self);
			socket_destroy(self);
			return ERROR;
		} else if (result == 0) { //SI received VALE CERO NOS CERRARON EL SKT
			skt_closed = true;
		} else {
			received += result;
		}
	}
	
	if (skt_closed) return 0; 
	return received;
}


