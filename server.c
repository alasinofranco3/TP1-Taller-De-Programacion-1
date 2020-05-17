#include "server.h"

//FUNCIONES PRIVADAS

static int print_header_parameter(char endianness, char* ptr) {
	int padding;
	int len = *(int *)(ptr + 4);
	if (endianness != 'l') {
		len = bswap_32(len);
	}
	char* aux = malloc(sizeof(char) *(len + 1));
	strncpy(aux, ptr + 8, len);
	aux[len] = '\0';
	printf("%s\n", aux);
	free(aux);
	padding = 8 - ((9 + len) % 8);
	return (9 + len + padding);
}

static int call_sum_par_num_set(int*parameter_num, char* ptr) {
	int par_num = (int)*(ptr + 4);
	*parameter_num = par_num;
	return (6 + par_num);
}

//FUNCIONES PUBLICAS 

int dbus_server_create(dbus_server_t *self) {
	int status = socket_create(&self->skt_acep);
	return status;
}

void dbus_server_destroy(dbus_server_t *self) {
	socket_shut_down(&self->peer_skt);
	socket_destroy(&self->peer_skt);
	socket_shut_down(&self->skt_acep);
	socket_destroy(&self->skt_acep);
}

int dbus_server_bind_and_listen(dbus_server_t *self, const char* port) {
	int status;
	status = socket_bind_and_listen(&self->skt_acep, port, 1);
	
	if (status == ERROR) {
		return ERROR;
	}

	return 0;
}

int dbus_server_accept(dbus_server_t *self) {
	int result;
	result = socket_accept(&self->skt_acep, &self->peer_skt);
	return result;	
}

int dbus_server_recv(dbus_server_t *self) {
	bool remote_skt_closed = false;
	while (remote_skt_closed == false) {
		int status;
		struct call_sum summary;
		status = dbus_server_recv_info(self, &summary);
		if (status == ERROR) return ERROR;
		if (status == 0) remote_skt_closed = true;
		
		if (remote_skt_closed == false) {
			status = dbus_server_recv_header(self, &summary);
			if (status == ERROR) return ERROR;
			if (status == 0) remote_skt_closed = true;
			status = dbus_server_recv_body(self, &summary);
			if (status == ERROR) return ERROR;
			if (status == 0) remote_skt_closed = true;			
			socket_send(&self->peer_skt, "OK\n", 3);	
			if (remote_skt_closed == false){
				printf("\n");
			}
		}
	}

	return 0;
}

int dbus_server_recv_info(dbus_server_t *self, struct call_sum *summary) {
	int result;
	int id;
	char buffer [16];
	result = socket_recv(&self->peer_skt, buffer, 16);
	if (result == ERROR) return ERROR;

	if (result != 0) {
		summary->endian = buffer[0];
		summary->body_len = *(int *)(buffer + 4);
		id = *(int *)(buffer + 8);
		summary->header_size = *(int *)(buffer + 12);

		if (buffer[0] != 'l') {
			summary->body_len = bswap_32(summary->body_len);
			id = bswap_32(id);
			summary->header_size = bswap_32(summary->header_size);
		}

		printf("* Id: %#010x\n", id);
	}	

	return result;
}

int dbus_server_recv_header(dbus_server_t *self, struct call_sum *summary) {
	char* buffer = malloc(sizeof(char) * summary->header_size);
	int result = socket_recv(&self->peer_skt, buffer, summary->header_size);
	if (result == ERROR) return ERROR;
	char* ptr = buffer;
	int read = 0;
	while (read < summary->header_size && result != 0) {		
		ptr = &buffer[read];
		switch (*ptr) {
			case 6 :
				printf("* Destino: ");
				read += print_header_parameter(summary->endian, ptr);
				break;

			case 1 :
				printf("* Ruta: ");
				read += print_header_parameter(summary->endian, ptr);
				break;

			case 2 :
				printf("* Interfaz: ");
				read += print_header_parameter(summary->endian, ptr);
				break;

			case 3 :
				printf("* Metodo: ");
				read += print_header_parameter(summary->endian, ptr);
				break;

			case 9 :
				read += call_sum_par_num_set(&summary->parameter_num, ptr);
				break;
		}		
	}
	free(buffer);
	return result;
}


int dbus_server_recv_body(dbus_server_t *self, struct call_sum *summary) {
	int read = 0;
	if (summary->body_len == 0) {
		printf("\n");
		return 0;
	}
	char* buffer = malloc(sizeof(char) * summary->body_len);
	int result = socket_recv(&self->peer_skt, buffer, summary->body_len);
	if (result == ERROR) return ERROR;
	if (result != 0 && strlen(buffer) != 0) {
		printf("* Parametros:\n");

		while (read < summary->body_len && result != 0) {
			int len = *(int *)&buffer[read];
			if (summary->endian != 'l') {
				len = bswap_32(len);
			}
			char* parameter = malloc(sizeof(char) * (len + 1));
			memset(parameter, 0, len + 1);
			strncpy(parameter, &buffer[read + 4], len);
			printf("    * %s\n", parameter);
			read += (5 + len);
			free(parameter);
		}		
	}	
	free(buffer);
	return result;
}





