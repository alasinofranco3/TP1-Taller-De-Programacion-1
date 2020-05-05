#include <byteswap.h>
#include <arpa/inet.h>
#include "common_dbus_message.h"

//FUNCIONES PRIVADAS

void set_first_4_bytes(char * buffer, int type) {
	buffer[0] = type;
	buffer[1] = 1;
	
	if (type == 1) {
		buffer[2] = 'o';
	} else if (type == 9) {
		buffer[2] = 'g';
	} else {
		buffer[2] = 's';
	}

	buffer[3] = 0;
}

int set_little_endian(int value) {
	value = htonl(value);
	value = bswap_32(value);
	return value;
}

void clean(dbus_message_t *self, resizable_buffer_t *call) {
	resizable_buffer_destroy(call);
	dbus_message_destroy(self);
}

int dbus_message_header_add(dbus_message_t *self, char* str, int par_type) {
	int buf_size = 9 + strlen(str);
	int padding = 8 - (buf_size % 8);
	buf_size += padding;
	
	char buf [buf_size];
	set_first_4_bytes(buf, par_type);

	int *ptr;
	ptr = (int*)(buf + 4);
	*ptr = set_little_endian(strlen(str));

	for (int i = 0; i < strlen(str); i++) {
		buf[4 + sizeof(int) + i] = str[i];
	}
	buf[4 + sizeof(int) + strlen(str)] = '\0';
	for (int i = 0; i < padding; i++) {
		buf[5 + sizeof(int) + strlen(str) + i] = 0; 
	}

 	int s = resizable_buffer_byte_save(&self->header, buf, buf_size);
	if (s == ERROR) {
		return ERROR;
	}

	return 0;
}

int dbus_message_header_add_args(dbus_message_t *self, char* args) {
	char* ptr;
	char args_num = 1;
	ptr = strchr(args, ',');
	while (ptr != NULL) {
		args_num ++;
		ptr = strchr(ptr + 1, ',');
	}
	int buf_size = 6 + args_num;
	char buf [buf_size];
	set_first_4_bytes(buf, 9);
	args_num = set_little_endian(args_num);
	buf [4] = args_num;
	for (int i = 0; i < args_num; i++) {
		buf[5 + i] = 's';
	}
	
	buf[buf_size - 1] = '\0';

	int s = resizable_buffer_byte_save(&self->header, buf, buf_size);
	if (s == ERROR) {
		return ERROR;
	}
	return 0;
}


//FUNCIONES PUBLICAS

int dbus_message_create(dbus_message_t *self){
	//QUIZAS DEBERIAMOS GUARDAR EL VECTOR INFO EN MEMORIA
	int status;
	memset(self->info, 0, 16);
	status = resizable_buffer_create(&self->header, 0);
	if (status == ERROR) return ERROR;
	status = resizable_buffer_create(&self->body, 0);
	if (status == ERROR) {
		resizable_buffer_destroy(&self->header);
		return ERROR;
	}

	return 0;
}

void dbus_message_destroy(dbus_message_t *self){
	resizable_buffer_destroy(&self->header);
	resizable_buffer_destroy(&self->body);
}

int dbus_message_set(dbus_message_t *self, resizable_buffer_t *call, int id) {
	int status;
	status = dbus_message_header_set(self, call);
	if (status == ERROR) return ERROR;
	
	status = dbus_message_body_set(self, call);
	if (status == ERROR) return ERROR;

	dbus_message_info_set(self, id);

	resizable_buffer_destroy(call);

	return 0;
}

int dbus_message_header_set(dbus_message_t *self, resizable_buffer_t *call) {
	//char aux [call->size];
	//strncpy(aux, call->buffer, call->size);
	//char *delim = " ()";
	//char *tk = strtok(aux, delim);
	//char* ptr = strchr(aux, ' ');
	char dest[call->size], path[call->size], inter[call->size];
	char method[call->size], parameters[call->size];
	sscanf(call->buffer, "%s %s %s %s(%s)", dest, path, inter, method, parameters);
	char *ptr = strchr(method, '(');
	*ptr = '\0';

	if (dbus_message_header_add(self, dest, 6) == ERROR) {
		clean(self, call);
		return ERROR;
	}
	//tk = strtok(NULL, delim);
	//ptr = strchr(ptr + 1, ' ');
	if (dbus_message_header_add(self, path, 1) == ERROR) {
		clean(self, call);
		return ERROR;
	}
	//tk = strtok(NULL, delim);
	if (dbus_message_header_add(self, inter, 2) == ERROR) {
		clean(self, call);
		return ERROR;
	}
	//tk = strtok(NULL, delim);
	if (dbus_message_header_add(self, method, 3) == ERROR) {
		clean(self, call);
		return ERROR;
	}
	//tk = strtok(NULL, delim);
	if (parameters != NULL) {
		if (dbus_message_header_add_args(self, parameters) == ERROR) {
			clean(self, call);
			return ERROR;
		}
	}	

	return 0;
}

int dbus_message_body_set(dbus_message_t *self, resizable_buffer_t *call) {
	char aux [call->size];
	strncpy(aux, call->buffer, call->size);
	char* delim = "(,)";
	strtok(aux, delim);
	char* parameter = strtok(NULL, delim);

	while (parameter != NULL && strchr(parameter, '\n') == NULL) {	
		int *ptr; 
		int buf_size = 5 + strlen(parameter);
		char buf [buf_size];
		ptr = (int*)(buf);
		*ptr = set_little_endian(strlen(parameter));
		for (int i = 0; i < strlen(parameter); i++) {
			buf[sizeof(int) + i] = parameter[i];
		}
		buf[sizeof(int) + strlen(parameter)] = '\0';
		if (resizable_buffer_byte_save(&self->body, buf, buf_size) == ERROR) {
			clean(self, call);
			return ERROR;
		}
		
		parameter = strtok(NULL, delim);		
	}

	return 0;
}

void dbus_message_info_set(dbus_message_t *self, int id_counter) {
	int *body_len, *header_size, *id;
	self->info[0] = 'l';
	self->info[1] = 1;
	self->info[2] = 0;
	self->info[3] = 1;

	body_len = (int*)(self->info + 4);
	*body_len = set_little_endian(self->body.size);
	id = (int*)(self->info + 8);
	*id = set_little_endian(id_counter);
	header_size = (int*)(self->info + 12);
	*header_size = set_little_endian(self->header.size);
}

int dbus_message_send(dbus_message_t *self, client_t *client) {
	int status;
	
	status = client_send(client, self->info, 16);
	if (status == ERROR) {
		dbus_message_destroy(self);
		return ERROR;
	}

	status = client_send(client, self->header.buffer, self->header.size);
	if (status == ERROR) {
		dbus_message_destroy(self);
		return ERROR;
	}

	status = client_send(client, self->body.buffer, self->body.size);
	if (status == ERROR) {
		dbus_message_destroy(self);
		return ERROR;
	}

	dbus_message_destroy(self);

	return 0;
}
