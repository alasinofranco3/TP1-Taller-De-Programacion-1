#include <byteswap.h>
#include <arpa/inet.h>
#include "common_dbus_message.h"

//FUNCIONES PRIVADAS

static void set_first_4_bytes(char * buffer, int type) {
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

static int set_little_endian(int value) {
	value = htonl(value);
	value = bswap_32(value);
	return value;
}

static int dbus_message_header_add(dbus_message_t *self, 
	char* str, int par_type) {
	int buf_size = 9 + strlen(str);
	int padding = 8 - (buf_size % 8);
	buf_size += padding;
	
	char buf [buf_size];
	//COMPLETAMOS LOS PRIMEROS 4 BYTES DEL BUFFER CON EL FORMATO
	//CORRESPONDIENTE
	set_first_4_bytes(buf, par_type);

	//AGREGAMOS EL TAMANIO DEL STRING AL BUFFER EN FORMATO
	//LITTLE ENDIAN
	int *ptr;
	ptr = (int*)(buf + 4);
	*ptr = set_little_endian(strlen(str));

	//AGREGAMOS LOS CARACTERES DEL STRING,EN LA POSICION CORRECTA
	//SABIENDO QUE ANTES TENEMOS 8 BYTES OCUPADOS POR EL TAMANIO DEL STRING
	// Y EL FORMATO DEL PARAMETRO
	for (int i = 0; i < strlen(str); i++) {
		buf[4 + sizeof(int) + i] = str[i];
	}
	//PONEMOS EL \0 INDICANDO EL FIN DE STRING
	buf[4 + sizeof(int) + strlen(str)] = '\0';
	//AGREGRAMOS EL PADDING CORRESPONDIENTE AL FINAL DEL PARAMETRO
	for (int i = 0; i < padding; i++) {
		buf[5 + sizeof(int) + strlen(str) + i] = 0; 
	}

	//GUARDAMOS EL PARAMETRO CON EL FORMATO ADECUADO EN EL HEADER DEL 
	//MENSAJE
 	int s = resizable_buffer_byte_save(&self->header, buf, buf_size);
	if (s == ERROR) {
		return ERROR;
	}

	return 0;
}

static int dbus_message_header_add_args(dbus_message_t *self, char* args) {
	char* ptr;
	//BUSCAMOS LA CANTIDAD DE ARGUMENTOS QUE TENDRA NUESTRA LLAMADA 
	//SERAN LA CANTIDAD DE COMAS ENCONTRADAS +1
	char args_num = 1;
	ptr = strchr(args, ',');
	while (ptr != NULL) {
		args_num ++;
		ptr = strchr(ptr + 1, ',');
	}
	int buf_size = 6 + args_num;
	char buf [buf_size];
	//COMPLETAMOS LOS PRIMEROS 4 BYTES DEL BUFFER CON EL FORMATO
	//CORRESPONDIENTE
	set_first_4_bytes(buf, 9);
	//AGREGAMOS EL BYTE QUE INDICA LA CANTIDAD DE PARAMETROS 
	//AL BUFFER EN FORMATO LITTLE ENDIAN
	args_num = set_little_endian(args_num);
	buf [4] = args_num;
	//AGREGAMOS UNA S POR CADA PARAMETRO PARA INDICAR EL TIPO DE CADA
	//PARAMETRO,SABIENDO QUE ANTES TENEMOS 5 BYTES OCUPADOS, 1 PARA
	//INDICAR LA CANTIDAD DE PARAMETROS Y OTROS 4 PARA EL FORMATO DE LA FIRMA
	for (int i = 0; i < args_num; i++) {
		buf[5 + i] = 's';
	}
	//AGREGAMOS EL \0 CORRESPONDIENTE 
	buf[buf_size - 1] = '\0';
	//AGREGAMOS EL BUFFER CON EL CONTENIDO RELACIONADO A LA FIRMA DE LA LLAMADA
	//AL HEADER DE NUESTRO MENSAJE
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

	return 0;
}

int dbus_message_header_set(dbus_message_t *self, resizable_buffer_t *call) {
	char aux [call->size];
	strncpy(aux, call->buffer, call->size);
	char *read = aux;
	char *delim = strchr(read,' ');
	
	read = aux;
	delim = strchr(read,' ');
	*delim = '\0';

	if (dbus_message_header_add(self, read, 6) == ERROR) {
		return ERROR;
	}

	read = delim + 1;
	delim = strchr(read, ' ');
	*delim = '\0';
	if (dbus_message_header_add(self, read, 1) == ERROR) {
		return ERROR;
	}

	read = delim + 1;
	delim = strchr(read, ' ');
	*delim = '\0';
	if (dbus_message_header_add(self, read, 2) == ERROR) {
		return ERROR;
	}

	read = delim + 1;
	delim = strchr(read, '(');
	*delim = '\0';
	if (dbus_message_header_add(self, read, 3) == ERROR) {
		return ERROR;
	}

	read = delim + 1;
	delim = strchr(read, ')');
	*delim = '\0';
	if (read != NULL) {
		if (dbus_message_header_add_args(self, read) == ERROR) {
			return ERROR;
		}
	}	

	return 0;
}

int dbus_message_body_set(dbus_message_t *self, resizable_buffer_t *call) {
	char aux [call->size];
	strncpy(aux, call->buffer, call->size);
	char* read = aux;
	char* delim = strchr(read, '(');
	read = delim + 1;
	delim = strchr(read, ')');
	*delim = '\0';

	while (read != NULL) {	
		int *ptr; 
		delim = strchr(read, ',');
		if (delim != NULL) *delim = '\0';
		int buf_size = 5 + strlen(read);
		char buf [buf_size];
		ptr = (int*)(buf);
		*ptr = set_little_endian(strlen(read));
		for (int i = 0; i < strlen(read); i++) {
			buf[sizeof(int) + i] = read[i];
		}
		buf[sizeof(int) + strlen(read)] = '\0';
		if (resizable_buffer_byte_save(&self->body, buf, buf_size) == ERROR) {
			return ERROR;
		}
		
		if (delim == NULL) {
			read = NULL;
		}else {
			read = delim + 1;
		}
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

int dbus_message_send(dbus_message_t *self, socket_t *socket) {
	int status;
	
	status = socket_send(socket, self->info, 16);
	if (status == ERROR) {
		return ERROR;
	}

	status = socket_send(socket, self->header.buffer, self->header.size);
	if (status == ERROR) {
		return ERROR;
	}

	status = socket_send(socket, self->body.buffer, self->body.size);
	if (status == ERROR) {
		return ERROR;
	}



	return 0;
}
