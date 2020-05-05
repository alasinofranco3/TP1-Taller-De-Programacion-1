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

int dbus_client_send(dbus_client_t *self) {
	while (!feof(self->file)) {
		self->id_counter ++;
		resizable_buffer_t buffer;
		dbus_message_t message;
		char server_answer [3];
		if (resizable_buffer_create(&buffer, 1)) {
			dbus_client_destroy(self);
			return ERROR;
		}	
		if (dbus_message_create(&message)) {
			resizable_buffer_destroy(&buffer);
			dbus_client_destroy(self);
			return ERROR;
		}	
		if (dbus_client_get_call(self, &buffer, MAX_BUF_SIZE) == ERROR) {
			dbus_message_destroy(&message);
			return ERROR;
		}
		if (dbus_message_set(&message, &buffer, self->id_counter) == ERROR) {
			return ERROR;
		}
		if (dbus_message_send(&message, &self->client) == ERROR) {
			dbus_client_destroy(self);
			return ERROR;
		}

		
		if (client_recv(&self->client, server_answer, 3) == ERROR) return ERROR;
		printf("%#010x: %s", self->id_counter, server_answer);
	}	

	return 0;
}

int dbus_client_get_call(dbus_client_t *self, resizable_buffer_t * buf, int s){
	//HAGO ESTO PORQUE EL CPPLINT NO ME DEJA CREAR UN BUFFER CON TAMANIO S
	//LA IDEA ERA RECIBIR POR PARAMETRO LO MAXIMO QUE PODIAMOS LEER CON 
	//CADA FREAD
	int buf_len = MAX_BUF_SIZE;
	char aux_buffer[MAX_BUF_SIZE];
	memset(aux_buffer, 0, buf_len);
	char* ptr = NULL;
	int status;
	bool done = false;

	while (!feof(self->file) && ptr == NULL) {
		memset(aux_buffer, 0, buf_len);
		if (fread(aux_buffer, 1, buf_len - 1, self->file) < buf_len - 1) {
			done = true;
		}

		aux_buffer[buf_len - 1] = '\0';
		ptr = strchr(aux_buffer, '\n');
		if (ptr == NULL) { //NO ENCONTRE EL FIN DE LINEA
			 status = resizable_buffer_save(buf, aux_buffer);
			 if (status == ERROR) {
			 	dbus_client_destroy(self);
			 	return ERROR;
			 }
		}
	}	

	if (ptr != NULL) {//ENCONTRE EL FIN DE LINEA
		//COLOCO EL PUNTERO DEL ARCHIVO AL COMIENZO DE LA SIGUIENTE LINEA
		if (done == false) {
			fseek(self->file, -(sizeof(char) * (strlen(ptr) - 1)), SEEK_CUR);
		}
		//ptr = strtok(aux_buffer, "\n");
		status = resizable_buffer_n_save(buf, aux_buffer, buf_len - strlen(ptr) - 1);
		if (status == ERROR) {
		 	dbus_client_destroy(self);
		 	return ERROR;
		}
	}
	
	return 0;
}




