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

	status = client_create(&self->client);
	if (status == ERROR) return ERROR;

	return 0;
}

void dbus_client_destroy(dbus_client_t *self) {
	client_destroy(&self->client);
	fclose(self->file);
}

int dbus_client_connect(dbus_client_t *self, const char* h, const char* p) {
	int status;

   	status = client_connect(&self->client, h, p);
   	if (status == ERROR) {
   		dbus_client_destroy(self);
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
			resizable_buffer_destroy(buffer);
			dbus_client_destroy(self);
			return ERROR;
		}	
		if (dbus_message_set(&message, buffer, self->id_counter) == ERROR) {
			return ERROR;
		}
		if (dbus_message_send(&message, &self->client) == ERROR) {
			dbus_client_destroy(self);
			return ERROR;
		}
		if (client_recv(&self->client, server_answer, 3) == ERROR) return ERROR;
		printf("%#010x: %s", self->id_counter, server_answer);
	}else{
		resizable_buffer_destroy(buffer);
	}

	return 0;
}

int dbus_client_get_call(dbus_client_t *self, resizable_buffer_t * b, char * r){
	int status;
	parser_t parser;
	
	if (resizable_buffer_create(b, 1)) {
		dbus_client_destroy(self);
		return ERROR;
	}	
	status = resizable_buffer_save(b, r);
	if (status == ERROR) {
	 	dbus_client_destroy(self);
	 	return ERROR;
	}
	
	parser_create(&parser);
	
	status = parser_run(self->file, b, r);
	
	if (status == ERROR) {
		parser_destroy(&parser);
		dbus_client_destroy(self);
		return ERROR;
	}
	parser_destroy(&parser);

	return 0;
}
