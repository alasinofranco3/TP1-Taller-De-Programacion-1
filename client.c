#include "client.h"
#include <stdbool.h>

int dbus_client_create(dbus_client_t *self, const char* file_name) {
	int status;
	self->id_counter = 0;

	if (file_name != NULL) {
		self->file = fopen(file_name, "r");
	} else {
		self->file = stdin;
	}

	if (!self->file) return ERROR;

	status = socket_create(&self->socket);
	if (status == ERROR) return ERROR;

	return 0;
}

void dbus_client_destroy(dbus_client_t *self) {
	socket_destroy(&self->socket);
	fclose(self->file);
}

int dbus_client_connect(dbus_client_t *self, const char* h, const char* p) {
	int status;

   	status = socket_connect(&self->socket, h, p);
   	if (status == ERROR) {
   		return 	ERROR;
   	}

   	return 0;
}

int dbus_client_send(dbus_client_t *self, resizable_buffer_t *buffer) {
	if (resizable_buffer_is_empty(buffer) == false) {
		self->id_counter ++;
		char server_answer [3];
		dbus_message_t message;
		if (dbus_message_create(&message)) {
			return ERROR;
		}	
		if (dbus_message_set(&message, buffer, self->id_counter) == ERROR) {
			dbus_message_destroy(&message);
			return ERROR;
		}
		if (dbus_message_send(&message, &self->socket) == ERROR) {
			dbus_message_destroy(&message);
			return ERROR;
		}
		if (socket_recv(&self->socket, server_answer, 3) == ERROR) {
			dbus_message_destroy(&message);
			return ERROR;
		}	
		printf("%#010x: %s", self->id_counter, server_answer);
		dbus_message_destroy(&message);
	}

	return 0;
}

int dbus_client_get_call(dbus_client_t *self, resizable_buffer_t * b, char * r){
	int status;
	parser_t parser;	

	status = resizable_buffer_save(b, r);
	if (status == ERROR) {
	 	return ERROR;
	}
	parser_create(&parser);
	status = parser_run(self->file, b, r);
	if (status == ERROR) {
		parser_destroy(&parser);
		return ERROR;
	}
	parser_destroy(&parser);
	return 0;
}
